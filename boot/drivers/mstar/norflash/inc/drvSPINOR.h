/*
* drvSPINOR.h- Sigmastar
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

#ifndef _DRV_SPINOR_H_
#define _DRV_SPINOR_H_
#include <common.h>
#define SPI_TO_MIU_PATCH

// please refer to the serial flash datasheet
#define SPI_NOR_CMD_READ		    (0x03)
#define SPI_NOR_CMD_FASTREAD	    (0x0B)
#define SPI_NOR_CMD_RDID		    (0x9F)
#define SPI_NOR_CMD_WREN		    (0x06)
#define SPI_NOR_CMD_WRDIS		    (0x04)
#define SPI_NOR_CMD_SE			    (0x20)
#define SPI_NOR_CMD_32BE		    (0x52)
#define SPI_NOR_CMD_64BE		    (0xD8)
#define SPI_NOR_CMD_CE			    (0xC7)
#define SPI_NOR_CMD_PP			    (0x02)
#define SPI_NOR_CMD_QP			    (0x32)
#define SPI_NOR_CMD_4PP			    (0x38)
#define SPI_NOR_CMD_QR_6B		    (0x6B)
#define SPI_NOR_CMD_QR_EB		    (0xEB)

#define SPI_NOR_CMD_RDSR		    (0x05)
#define SPI_NOR_CMD_RDSR2		    (0x35)
#define SPI_NOR_CMD_RDSR3		    (0x15)
#define SPI_NOR_CMD_WRSR		    (0x01)
#define SPI_NOR_CMD_WRSR2		    (0x31)
#define SPI_NOR_CMD_WRSR3		    (0x11)

  // support for 256Mb up MIX flash
#define SPI_NOR_CMD_WREAR		    (0xC5)
#define SPI_NOR_CMD_RESET           (0X99)
#define SPI_NOR_CMD_EN_RESET        (0X66)


#define SPI_NOR_READ_DUMMY              0x08
#define SPI_NOR_DEFAULT_ERASE_SIZE      (4 << 10)
#define SPI_NOR_BUSY                    (0x01)
#define SPI_NOR_16MB       		(1 << 24)
#define SPI_NOR_16MB_MASK       ((1 << 24) - 1)

#define SPI_NOR_ERASE_SIZE_4K       SPI_NOR_CMD_SE
#define SPI_NOR_ERASE_SIZE_32K      SPI_NOR_CMD_32BE
#define SPI_NOR_ERASE_SIZE_64K      SPI_NOR_CMD_64BE


typedef enum _SPINOR_ERROR_NUM {
    ERR_SPINOR_SUCCESS =           0x00,
    ERR_SPINOR_RESET_FAIL,
    ERR_SPINOR_E_FAIL,
    ERR_SPINOR_P_FAIL,
    ERR_SPINOR_INVALID,
    ERR_SPINOR_TIMEOUT,
    ERR_SPINOR_DEVICE_FAILURE,
    ERR_SPINOR_BDMA_FAILURE,
} SPINOR_FLASH_ERRNO_e;

typedef struct {
    u8 u8_readData;
    u8 u8_dummy;
    u8 u8_pageProgram;
    u32 u32_programStage;
    u32 u32_time_wait;
} DRV_SPINNOR_INFO_t;

u8 DRV_SPINOR_read_status(u8 u8_cmd, u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_read_status1(u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_read_status2(u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_read_status3(u8 *pu8_status, u8 u8_size);

u8 DRV_SPINOR_write_status(u8 u8_cmd, u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_write_status1(u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_write_status2(u8 *pu8_status, u8 u8_size);
u8 DRV_SPINOR_write_status3(u8 *pu8_status, u8 u8_size);

void DRV_SPINOR_setup_time_wait(u32 u32_time_wait);
void DRV_SPINOR_setup_access(u8 u8_read, u8 u8_dummy, u8 u8_program);
u8 DRV_SPINOR_init(void);
u8 DRV_SPINOR_read(u32 u32_address, u8 *pu8_data, u32 u32_size);
u8 DRV_SPINOR_program_page(u32 u32_address, u8 *pu8_data, u32 u32_size);
u8 DRV_SPINOR_block_erase(u32 u32_address);
u8 DRV_SPINOR_sector_erase(u32 u32_address);
u8 DRV_SPINOR_check_status(void);
u8 DRV_SPINOR_read_id(u8 *pu8_ids, u8 u8_bytes);
u8 DRV_SPINOR_receive_data(u8* pu8_buf, u32 u32_size);
u8 DRV_SPINOR_single_transfer(u8* pu8_buf, u32 u32_size);
u8 DRV_SPINOR_burst_transfer(u8* pu8_buf, u32 u32_size);
void DRV_SPINOR_use_bdma(u8 u8_enabled);

#endif
