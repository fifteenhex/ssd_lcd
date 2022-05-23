/*
* halFSP_QSPI.h- Sigmastar
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

#ifndef _HAL_FSP_QSPI_H
#define _HAL_FSP_QSPI_H

//#include "stdcomp.h"

#define REG_SPI_SW_CS_EN                        0x01
#define REG_SPI_SW_CS_PULL_HIGH                 0x02
void HAL_REG_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_REG_WriteByte(u32 u32_address, u8 u8_val);
void HAL_CHIP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_PM_SLEEP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_FSP_WriteByte(u32 u32_address, u8 u8_val);
void HAL_FSP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_QSPI_WriteByte(u32 u32_address, u16 u8_val);
void HAL_QSPI_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_QSPI_Write2Byte_Mask(u32 u32_address, u16 u16_val,u16 u16_mask);
u16 HAL_FSP_Read2Byte(u32 u32_address);
u8 HAL_FSP_ReadByte(u32 u32_address);
u8 HAL_QSPI_ReadByte(u32 u32_address);

#endif
