/*
* drvSPINAND.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/

#include <common.h>

#include <drvSPINAND.h>
#include <drvFSP_QSPI.h>
#include <halFSP_QSPI.h>
#include <mdrvSNI.h>
#include <hal_bdma.h>

extern void invalidate_dcache_range(unsigned long start, unsigned long stop);
extern void flush_dcache_range(unsigned long start, unsigned long stop);
extern void chip_flush_miu_pipe(void);

#define SPI_NAND_CMD_RFC_DUMMY                  0x08
#define MIU0_BASE                               0x20000000

DRV_SPINAND_INFO_t st_spinand_drv_info;

static u8 (*_DRV_SPINAND_read_from_cache)(u16, u8*, u32);
static u8 (*_DRV_SPINAND_program_load_data)(u8*, u32);

static u8 _DRV_SPINAND_get_cmd_mode(u8 u8_cmd)
{
    switch (u8_cmd)
    {
        case SPI_NAND_CMD_QPL:
        case 0x6B:
        case SPI_NAND_CMD_QRPL:
        case 0xEB:
            return FSP_QUAD_MODE;
        case 0x3B:
        case 0xBB:
            return FSP_DUAL_MODE;
        default:
            return FSP_SINGLE_MODE;
    }
}

static u8 _DRV_SPINAND_return_status(u8 u8_status)
{
    if (ERR_SPINAND_INVALID <= u8_status)
    {
        return ERR_SPINAND_DEVICE_FAILURE;
    }

    return u8_status;
}
u8 DRV_SPINAND_receive_data(u8* pu8_buf, u32 u32_size)
{
    u8 u8_rd_size;

    DRV_FSP_init(FSP_USE_SINGLE_CMD);

    while (0 != u32_size)
    {
        DRV_FSP_set_which_cmd_size(1, 0);
        u8_rd_size = DRV_FSP_set_rbf_size_after_which_cmd(1, u32_size);

        DRV_FSP_trigger();

        if (!DRV_FSP_is_done())
        {
            break;
        }

        u32_size -= DRV_FSP_read_rbf(pu8_buf, u8_rd_size);
        pu8_buf += u8_rd_size;

        DRV_FSP_clear_trigger();
    }

    if (0 != u32_size)
    {
        return ERR_SPINAND_TIMEOUT;
    }
	
    return ERR_SPINAND_SUCCESS;
}

u8 DRV_SPINAND_simple_transmission(u8* pu8_buf, u32 u32_size)
{
    u8 u8_wr_size;

    DRV_FSP_init(FSP_USE_SINGLE_CMD);

    while (0 != u32_size)
    {
        u8_wr_size = DRV_FSP_set_which_cmd_size(1, u32_size);

        if(pu8_buf)
        {
            DRV_FSP_write_wbf(pu8_buf, u8_wr_size);
            pu8_buf += u8_wr_size;
        }

        DRV_FSP_trigger();

        if (!DRV_FSP_is_done())
        {
            break;
        }

        DRV_FSP_clear_trigger();
        u32_size -= u8_wr_size;
    }

    if (0 != u32_size)
    {
        return ERR_SPINAND_TIMEOUT;
    }

    return ERR_SPINAND_SUCCESS;
}

u8 DRV_SPINAND_complete_transmission(u8* pu8_buf, u32 u32_size)
{
    u8 u8_status;

    DRV_QSPI_pull_cs(0);
    u8_status = DRV_SPINAND_simple_transmission(pu8_buf, u32_size);
    DRV_QSPI_pull_cs(1);

    return _DRV_SPINAND_return_status(u8_status);
}

u8 DRV_SPINAND_reset(void)
{
    u8 u8_cmd;
    u8_cmd = SPI_NAND_CMD_RESET;

    return DRV_SPINAND_complete_transmission(&u8_cmd, 1);
}

u8 DRV_SPINAND_reset_status(void)
{
    if (ERR_SPINAND_SUCCESS != DRV_SPINAND_reset())
    {
        return ERR_SPINAND_TIMEOUT;
    }

    return DRV_SPINAND_check_status();
}

u8 DRV_SPINAND_write_enable(void)
{
    u8 u8_cmd;
    u8_cmd = SPI_NAND_CMD_WREN;

    return DRV_SPINAND_complete_transmission(&u8_cmd, 1);
}

u8 DRV_SPINAND_write_disable(void)
{
    u8 u8_cmd;
    u8_cmd = SPI_NAND_CMD_WRDIS;

    return DRV_SPINAND_complete_transmission(&u8_cmd, 1);
}

u8 DRV_SPINAND_get_features(u8 u8_address, u8 *pu8_data, u8 u8_size)
{
    u8 u8_status;
    u16 u16_cmd;

    u16_cmd = SPI_NAND_CMD_GF | (u8_address << 8);

    DRV_QSPI_pull_cs(0);

    u8_status = DRV_SPINAND_simple_transmission((u8*)&u16_cmd, 2);
    if (ERR_SPINAND_SUCCESS == u8_status)
    {
        u8_status = DRV_SPINAND_receive_data(pu8_data, u8_size);
    }

    DRV_QSPI_pull_cs(1);

    return _DRV_SPINAND_return_status(u8_status);

}

u8 DRV_SPINAND_get_status_register(u8 *u8_status)
{
    return DRV_SPINAND_get_features(SPI_NAND_REG_STAT, u8_status, 1);
}

u8 DRV_SPINAND_check_status(void)
{
    u8 u8_status;
    u32 u32_timeout;

    u32_timeout = st_spinand_drv_info.u32_maxWaitTime;

    do
    {
        u8_status = 0;

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_status_register(&u8_status))
        {
            return ERR_SPINAND_DEVICE_FAILURE;
        }

        udelay(10);

        if (10 < u32_timeout)
        {
            u32_timeout -= 10;
        }
        else
        {
            u32_timeout = 0;
        }

        if (0 == u32_timeout)
        {
            return ERR_SPINAND_TIMEOUT;
        }

    } while (u8_status & SPI_NAND_STAT_OIP);

    if (u8_status & SPI_NAND_STAT_P_FAIL)
    {
        return ERR_SPINAND_P_FAIL;
    }

    if (u8_status & SPI_NAND_STAT_E_FAIL)
    {
        return ERR_SPINAND_E_FAIL;
    }

    u8_status &= ECC_STATUS_MASK;

    if (0 < u8_status)
    {
        if (ECC_NOT_CORRECTED == u8_status)
        {
            return ERR_SPINAND_ECC_NOT_CORRECTED;
        }

        if (ECC_STATUS_RESERVED == u8_status)
        {
            return ERR_SPINAND_ECC_RESERVED;
        }
        return ERR_SPINAND_ECC_CORRECTED;
    }
    return ERR_SPINAND_SUCCESS;
}

u8 DRV_SPINAND_set_features(u8 u8_address, u8 *pu8_data, u8 u8_size)
{
    u8 u8_status;
    u16 u16_cmd;

    u16_cmd = SPI_NAND_CMD_SF | (u8_address << 8);

    DRV_QSPI_pull_cs(0);
    u8_status = DRV_SPINAND_simple_transmission((u8*)&u16_cmd, 2);

    if (ERR_SPINAND_SUCCESS == u8_status)
    {
        u8_status = DRV_SPINAND_simple_transmission(pu8_data, u8_size);
    }

    DRV_QSPI_pull_cs(1);
    return _DRV_SPINAND_return_status(u8_status);
}

u8 DRV_SPINAND_cmd_read_id(void)
{
    u16 u16_cmd;

    u16_cmd = SPI_NAND_CMD_RDID | (0x00 << 8);

    return DRV_SPINAND_simple_transmission((u8*)&u16_cmd, 2);
}

u8 DRV_SPINAND_read_id(u8 *pu8_ids, u8 u8_bytes)
{
    u8 u8_status;

    DRV_QSPI_pull_cs(0);

    u8_status = DRV_SPINAND_cmd_read_id();
    if (ERR_SPINAND_SUCCESS == u8_status)
    {
        u8_status = DRV_SPINAND_receive_data(pu8_ids, u8_bytes);
    }

    DRV_QSPI_pull_cs(1);
    return _DRV_SPINAND_return_status(u8_status);
}

u8 DRV_SPINAND_page_read(u32 u32_row_address)
{
    u32 u32_cmd;

    u32_cmd = SPI_NAND_CMD_PGRD | (((u32_row_address >> 16) & 0xff) << 8) | (((u32_row_address >> 8) & 0xff) << 16) | ((u32_row_address & 0xff) << 24);

    return DRV_SPINAND_complete_transmission((u8*)&u32_cmd, 4);
}

u8 DRV_SPINAND_page_read_with_status(u32 u32_row_address)
{
    if (ERR_SPINAND_SUCCESS != DRV_SPINAND_page_read(u32_row_address))
    {
        return ERR_SPINAND_DEVICE_FAILURE;
    }

    return DRV_SPINAND_check_status();
}

static u8 _DRV_SPINAND_read_from_cache_by_riu(u16 u16_col_address, u8 *pu8_data, u32 u32_size)
{
    u8 au8_cmd[4];
    u8 u8_dummy;
    u8 u8_status;

    DRV_QSPI_pull_cs(0);
    do
    {
        au8_cmd[0] = (u16_col_address >> 8) & 0xff;
        au8_cmd[1] = u16_col_address & 0xff;

        if (ERR_SPINAND_SUCCESS != (u8_status = DRV_SPINAND_simple_transmission(&st_spinand_drv_info.u8_rfc, 1)))
        {
            break;
        }

        u8_dummy = (st_spinand_drv_info.u8_dummy * 1) / 8;
        
        if (0xEB == st_spinand_drv_info.u8_rfc)
        {
            u8_dummy = (st_spinand_drv_info.u8_dummy * 4) / 8;
            DRV_FSP_set_access_mode(_DRV_SPINAND_get_cmd_mode(st_spinand_drv_info.u8_rfc));
        }
        else if(0xBB == st_spinand_drv_info.u8_rfc)
        {
            u8_dummy = (st_spinand_drv_info.u8_dummy * 2) / 8;
            DRV_FSP_set_access_mode(_DRV_SPINAND_get_cmd_mode(st_spinand_drv_info.u8_rfc));
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_simple_transmission(au8_cmd, 2))
        {
            break;
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_simple_transmission(NULL, u8_dummy))
        {
            break;
        }

        DRV_FSP_set_access_mode(_DRV_SPINAND_get_cmd_mode(st_spinand_drv_info.u8_rfc));

        if(ERR_SPINAND_SUCCESS == u8_status)
        {
            u8_status = DRV_SPINAND_receive_data(pu8_data, u32_size);
        }

        DRV_FSP_set_access_mode(FSP_SINGLE_MODE);
    } while (0);

    DRV_QSPI_pull_cs(1);
    return _DRV_SPINAND_return_status(u8_status);

}

static u8 _DRV_SPINAND_read_from_cache_by_bdma(u16 u16_col_address, u8 *pu8_data, u32 u32_size)
{
    u8 u8_status = 0;
    HalBdmaParam_t tBdmaParam;

    u8_status = ERR_SPINAND_SUCCESS;
    invalidate_dcache_range((u32)pu8_data, (u32)pu8_data + u32_size);
    DRV_QSPI_pull_cs(0);
    DRV_QSPI_use_2bytes_address_mode(DRV_QSPI_cmd_to_mode(st_spinand_drv_info.u8_rfc), st_spinand_drv_info.u8_dummy);
    memset(&tBdmaParam, 0, sizeof(HalBdmaParam_t));
    tBdmaParam.bIntMode          = 0; //0:use polling mode
    tBdmaParam.ePathSel          = HAL_BDMA_SPI_TO_MIU0;
    tBdmaParam.eSrcDataWidth     = HAL_BDMA_DATA_BYTE_8;
    tBdmaParam.eDstDataWidth     = HAL_BDMA_DATA_BYTE_16;
    tBdmaParam.eDstAddrMode      = HAL_BDMA_ADDR_INC; //address increase
    tBdmaParam.u32TxCount        = u32_size;
    tBdmaParam.u32Pattern        = 0;
    tBdmaParam.pSrcAddr          = (void*)(u32)u16_col_address;
    tBdmaParam.pDstAddr          = (void*)(pu8_data - MIU0_BASE);
    tBdmaParam.pfTxCbFunc        = NULL;

    if (HAL_BDMA_PROC_DONE != HalBdma_DoTransfer(HAL_BDMA_CH0, &tBdmaParam))
    {
        u8_status = ERR_SPINAND_BDMA_FAILURE;
    }

    DRV_QSPI_use_2bytes_address_mode(DRV_QSPI_cmd_to_mode(SPI_NAND_CMD_RFC), SPI_NAND_CMD_RFC_DUMMY);
    DRV_QSPI_pull_cs(1);

    return _DRV_SPINAND_return_status(u8_status);
}


u8 DRV_SPINAND_program_execute(u32 u32_row_address)
{
    u32 u32_cmd;

    u32_cmd = 0;
    u32_cmd = SPI_NAND_CMD_PE | (((u32_row_address >> 16) & 0xff) << 8) | ((u32_row_address >> 8) & 0xff) << 16 | (u32_row_address & 0xff) << 24;

    return DRV_SPINAND_complete_transmission((u8*)&u32_cmd, 4);
}

u8 DRV_SPINAND_cmd_normal_read_from_cache(u16 u16_col_address)
{
    u8 au8_cmd[4];

    au8_cmd[0] = SPI_NAND_CMD_RFC;
    au8_cmd[1] = u16_col_address >> 8;
    au8_cmd[2] = u16_col_address;
    au8_cmd[3] = 0x00; //dummy cycle

    return DRV_SPINAND_simple_transmission(au8_cmd, 4);
}

u8 DRV_SPINAND_cmd_read_status_register(void)
{
    u16 u16_cmd;

    u16_cmd = SPI_NAND_CMD_GF | (SPI_NAND_REG_STAT << 8);

    return DRV_SPINAND_simple_transmission((u8*)&u16_cmd, 2);
}

u8 DRV_SPINAND_read_from_cache(u16 u16_col_address, u8 *pu8_data, u32 u32_size)
{
    u8 u8_status;

    /*pu8_data and u32_size align to 64,for cache. u16_col_address align to 16,patch for bdma*/
    if (0 != ((u32)pu8_data % 64) || 0 != (u32_size % 64) || 0 != (u16_col_address % 16) )
    {
        u8_status = _DRV_SPINAND_read_from_cache_by_riu(u16_col_address, pu8_data, u32_size);
    }
    else
    {
        u8_status = _DRV_SPINAND_read_from_cache(u16_col_address, pu8_data, u32_size);
    }

    return _DRV_SPINAND_return_status(u8_status);

}

u8 DRV_SPINAND_block_erase(u32 u32_row_address)
{
    u8 au8_cmd[4];

    if (ERR_SPINAND_SUCCESS != DRV_SPINAND_write_enable())
    {
        return ERR_SPINAND_DEVICE_FAILURE;
    }
    DRV_SPINAND_check_status();
    au8_cmd[0] = SPI_NAND_CMD_BE;
    au8_cmd[1] = (u32_row_address >> 16) & 0xff;
    au8_cmd[2] = (u32_row_address >> 8) & 0xff;
    au8_cmd[3] = u32_row_address & 0xff;

    return DRV_SPINAND_complete_transmission(au8_cmd, 4);
}

static u8 _DRV_SPINAND_program_load_data_by_bdma(u8* pu8_data, u32 u32_size)
{
    u8 u8_status = ERR_SPINAND_SUCCESS;
    u32 u32_write_bytes;

    HalBdmaParam_t    tBdmaParam;
    flush_dcache_range((unsigned long)pu8_data, (unsigned long)pu8_data + u32_size);
    chip_flush_miu_pipe();

    while(0 != u32_size)
    {
        u32_write_bytes = DRV_FSP_enable_outside_wbf(0, 0, u32_size);
        DRV_FSP_init(FSP_USE_SINGLE_CMD);
        DRV_FSP_set_which_cmd_size(1, 0);

        memset(&tBdmaParam, 0, sizeof(HalBdmaParam_t));
        tBdmaParam.bIntMode          = 0; //0:use polling mode
        tBdmaParam.ePathSel          = HAL_BDMA_MIU0_TO_SPI;
        tBdmaParam.eSrcDataWidth     = HAL_BDMA_DATA_BYTE_16;
        tBdmaParam.eDstDataWidth     = HAL_BDMA_DATA_BYTE_8;
        tBdmaParam.eDstAddrMode      = HAL_BDMA_ADDR_INC; //address increase
        tBdmaParam.u32TxCount        = u32_write_bytes;
        tBdmaParam.u32Pattern        = 0;
        tBdmaParam.pSrcAddr          = (void*)(pu8_data - MIU0_BASE);
        tBdmaParam.pDstAddr          = 0;
        tBdmaParam.pfTxCbFunc        = NULL;

        DRV_FSP_trigger();

        if (HAL_BDMA_PROC_DONE != HalBdma_DoTransfer(HAL_BDMA_CH0, &tBdmaParam))
        {
            u8_status = ERR_SPINAND_BDMA_FAILURE;
            break;
        }

        if (!DRV_FSP_is_done())
        {
            u8_status = ERR_SPINAND_TIMEOUT;
        }

        DRV_FSP_clear_trigger();
        pu8_data += u32_write_bytes;
        u32_size -= u32_write_bytes;
    }

    DRV_FSP_disable_outside_wbf();
    return _DRV_SPINAND_return_status(u8_status);
}

static u8 _DRV_SPINAND_load_data_to_cache(const u8* pu8_data, u32 u32_size)
{
    u8 u8_status = ERR_SPINAND_SUCCESS;
    u32 u32_bytes_to_load;

    while (0 != u32_size)
    {
        u32_bytes_to_load = u32_size;
        /*align to 64,for cache*/
        if (0 != ((u32)pu8_data % 64))
        {
            u32_bytes_to_load = 64 - ((u32)pu8_data % 64);

            if (u32_size < u32_bytes_to_load)
            {
                u32_bytes_to_load = u32_size;
            }
        }
        else if (0 != ((u32)u32_size % 64))/*align to 64,for cache*/
        {
            u32_bytes_to_load = u32_size % 64;

            if (64 < u32_size)
            {
                u32_bytes_to_load = u32_size - u32_bytes_to_load;
            }
        }

        if (0 != (u32_bytes_to_load % 64))/*align to 64,for cache*/
        {
            u8_status = DRV_SPINAND_simple_transmission((u8 *)pu8_data, u32_bytes_to_load);
        }
        else
        {
            u8_status = _DRV_SPINAND_program_load_data((u8*)pu8_data, u32_bytes_to_load);
        }

        if (ERR_SPINAND_SUCCESS != u8_status)
        {
            break;
        }

        u32_size -= u32_bytes_to_load;
        pu8_data += u32_bytes_to_load;
    }

    return _DRV_SPINAND_return_status(u8_status);

}

u8 DRV_SPINAND_cmd_program_load(u16 u16_col_address)
{
    u32 u32_cmd;

    u32_cmd = 0;
    u32_cmd |= st_spinand_drv_info.u8_programLoad;
    u32_cmd |= ((u16_col_address >> 8) & 0xff) << 8;
    u32_cmd |= (u16_col_address& 0xff) << 16;

    return DRV_SPINAND_simple_transmission((u8*)&u32_cmd, 3);
}

u8 DRV_SPINAND_random_program_load(u16 u16_col_address, u8 *pu8_data, u32 u32_size)
{
    u8 u8_status;
    u32 u32_cmd;

    u32_cmd = 0;
    u32_cmd |= st_spinand_drv_info.u8_randomLoad;
    u32_cmd |= (u16_col_address >> 8 & 0xff) << 8;
    u32_cmd |= (u16_col_address & 0xff) << 16;

    DRV_QSPI_pull_cs(0);

    if (ERR_SPINAND_SUCCESS == (u8_status = DRV_SPINAND_simple_transmission((u8*)&u32_cmd, 3)))
    {
        DRV_FSP_set_access_mode(_DRV_SPINAND_get_cmd_mode(st_spinand_drv_info.u8_randomLoad));
        u8_status = _DRV_SPINAND_load_data_to_cache(pu8_data, u32_size);
        DRV_FSP_set_access_mode(FSP_SINGLE_MODE);
    }

    DRV_QSPI_pull_cs(1);

    return _DRV_SPINAND_return_status(u8_status);

}

u8 DRV_SPINAND_program_load(u16 u16_col_address, u8* pu8_data, u32 u32_size)
{
    u8 u8_status;
    u32 u32_cmd;

    u32_cmd = 0;
    u32_cmd |= st_spinand_drv_info.u8_programLoad;
    u32_cmd |= ((u16_col_address >> 8) & 0xff) << 8;
    u32_cmd |= (u16_col_address& 0xff) << 16;

    DRV_QSPI_pull_cs(0);
    if (ERR_SPINAND_SUCCESS == (u8_status = DRV_SPINAND_simple_transmission((u8*)&u32_cmd, 3)))
    {
        DRV_FSP_set_access_mode(_DRV_SPINAND_get_cmd_mode(st_spinand_drv_info.u8_programLoad));
        u8_status = _DRV_SPINAND_load_data_to_cache(pu8_data, u32_size);
        DRV_FSP_set_access_mode(FSP_SINGLE_MODE);
    }

    DRV_QSPI_pull_cs(1);

    return _DRV_SPINAND_return_status(u8_status);

}

void DRV_SPINAND_setup_timeout(u32 u32_timeout)
{
    st_spinand_drv_info.u32_maxWaitTime = u32_timeout;
}

void DRV_SPINAND_use_bdma(u8 u8_enabled)
{
    if (u8_enabled)
    {
        _DRV_SPINAND_read_from_cache = _DRV_SPINAND_read_from_cache_by_bdma;
        _DRV_SPINAND_program_load_data = _DRV_SPINAND_program_load_data_by_bdma;
    }
    else
    {
        _DRV_SPINAND_read_from_cache = _DRV_SPINAND_read_from_cache_by_riu;
        _DRV_SPINAND_program_load_data = DRV_SPINAND_simple_transmission;
    }
}

void DRV_SPINAND_setup_access(u8 u8_read, u8 u8_dummy_cycle, u8 u8_program, u8 u8_random)
{
    if (st_spinand_drv_info.u8_rfc != u8_read || st_spinand_drv_info.u8_dummy != u8_dummy_cycle)
    {
        st_spinand_drv_info.u8_rfc = u8_read;
        st_spinand_drv_info.u8_dummy = u8_dummy_cycle;
        printf("[SPINAND] RFC ues command 0x%x with 0x%02x dummy clock.\r\n", st_spinand_drv_info.u8_rfc, st_spinand_drv_info.u8_dummy);
    }

    if (st_spinand_drv_info.u8_programLoad != u8_program || st_spinand_drv_info.u8_randomLoad != u8_random)
    {
        st_spinand_drv_info.u8_programLoad = u8_program;
        st_spinand_drv_info.u8_randomLoad = u8_random;
        printf("[SPINAND] Program load with command 0x%02x.\r\n", st_spinand_drv_info.u8_programLoad);
        printf("[SPINAND] Random load with command 0x%02x.\r\n", st_spinand_drv_info.u8_randomLoad);
    }

    return;
}

u8 DRV_SPINAND_init(void)
{
    DRV_FSP_QSPI_init();

    st_spinand_drv_info.u8_rfc = SPI_NAND_CMD_RFC;
    st_spinand_drv_info.u8_dummy = SPI_NAND_CMD_RFC_DUMMY;
    st_spinand_drv_info.u8_programLoad = SPI_NAND_CMD_PL;
    st_spinand_drv_info.u8_randomLoad = SPI_NAND_CMD_RPL;
    st_spinand_drv_info.u32_maxWaitTime = -1;
    DRV_QSPI_use_2bytes_address_mode(DRV_QSPI_cmd_to_mode(st_spinand_drv_info.u8_rfc), st_spinand_drv_info.u8_dummy);
    _DRV_SPINAND_read_from_cache = _DRV_SPINAND_read_from_cache_by_riu;
    _DRV_SPINAND_program_load_data = DRV_SPINAND_simple_transmission;
    return DRV_SPINAND_reset_status();
}

