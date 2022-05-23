/*
* halFSP_QSPI.c- Sigmastar
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

//#include <common.h>
#include <linux/string.h>
#include <ms_platform.h>
#include <halFSP_QSPI_reg.h>

#if defined(FSP_QSPI_DUAL_BANK)
u8 FSPQSPI_mode = FPS_QSPI_NONPM;
#endif

void HAL_REG_Write2Byte(u32 u32_address, u16 u16_val)
{
    *(unsigned short volatile*)(RIU_BASE_ADDR + u32_address) = u16_val;
}

void HAL_REG_WriteByte(u32 u32_address, u8 u8_val)
{
    *(unsigned char volatile*)(RIU_BASE_ADDR + u32_address) = u8_val;
}

void HAL_CHIP_Write2Byte(u32 u32_address, u16 u16_val)
{
    // RIU[CHIP_BASE_ADDR + (u32RegAddr << 1)] = u16Val;
    *(unsigned short volatile*)(RIU_BASE_ADDR + CHIP_BASE_ADDR + (u32_address << 2)) = u16_val;
}


void HAL_PM_SLEEP_Write2Byte(u32 u32_address, u16 u16_val)
{
    *(unsigned short volatile*)(RIU_BASE_ADDR + PM_SLEEP_BASE_ADDR + (u32_address << 2))= u16_val;
}


void HAL_FSP_WriteByte(u32 u32_address, u8 u8_val)
{
    u32 u32_fsp_address;

    u32_fsp_address = FSP_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_fsp_address = FSP_BASE_ADDR;
    }
#endif
    *(unsigned char volatile*)(RIU_BASE_ADDR + u32_fsp_address + u32_address) = u8_val;
}

void HAL_FSP_Write2Byte(u32 u32_address, u16 u16_val)
{
    u32 u32_fsp_address;

    u32_fsp_address = FSP_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_fsp_address = FSP_BASE_ADDR;
    }
#endif

    *(unsigned short volatile*)(RIU_BASE_ADDR + u32_fsp_address + u32_address) = u16_val;
}

void HAL_QSPI_WriteByte(u32 u32_address, u16 u8_val)
{
    u32 u32_qspi_address;

    u32_qspi_address = QSPI_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_qspi_address = QSPI_BASE_ADDR;
    }
#endif
    *(unsigned char volatile*)(RIU_BASE_ADDR + u32_qspi_address + u32_address) = u8_val;
}

void HAL_QSPI_Write2Byte(u32 u32_address, u16 u16_val)
{
    u32 u32_qspi_address;

    u32_qspi_address = QSPI_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_qspi_address = QSPI_BASE_ADDR;
    }
#endif

    *(unsigned short volatile*)(RIU_BASE_ADDR + u32_qspi_address + u32_address) = u16_val;

}

void HAL_QSPI_Write2Byte_Mask(u32 u32_address, u16 u16_val,u16 u16_mask)
{
    u32 u32_qspi_address;
    u32 u16_reg_timeout_val;

    u32_qspi_address = QSPI_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_qspi_address = QSPI_BASE_ADDR;
    }
#endif
    u16_reg_timeout_val = (*(unsigned short volatile *)(RIU_BASE_ADDR + u32_qspi_address + u32_address));
    u16_reg_timeout_val = (u16_reg_timeout_val & ~u16_mask) | (u16_val & u16_mask);
    *(unsigned short volatile *)(RIU_BASE_ADDR + u32_qspi_address + u32_address) = u16_reg_timeout_val;
}

u8 HAL_QSPI_ReadByte(u32 u32_address)
{
    u32 u32_qspi_address;

    u32_qspi_address = QSPI_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_qspi_address = QSPI_BASE_ADDR;
    }
#endif

    return (*(unsigned char volatile *)(RIU_BASE_ADDR + u32_qspi_address + u32_address));
}
u16 HAL_FSP_Read2Byte(u32 u32_address)
{
    u32 u32_fsp_address;

    u32_fsp_address = FSP_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_fsp_address = FSP_BASE_ADDR;
    }
#endif

    return(*(unsigned short volatile*) (RIU_BASE_ADDR + u32_fsp_address + u32_address));
}

u8 HAL_FSP_ReadByte(u32 u32_address)
{
    u32 u32_fsp_address;

    u32_fsp_address = FSP_PMBASE_ADDR;

#if defined(FSP_QSPI_DUAL_BANK)
    if (FPS_QSPI_NONPM == FSPQSPI_mode)
    {
        u32_fsp_address = FSP_BASE_ADDR;
    }
#endif

    return (*(unsigned char volatile*)(RIU_BASE_ADDR + u32_fsp_address + u32_address));
}

