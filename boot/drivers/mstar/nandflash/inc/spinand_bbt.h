/*
* spinand_bbt.h- Sigmastar
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

#ifndef __SPINAND_BBT_H__
#define __SPINAND_BBT_H__

#include <common.h>

#define    BBT_HEADER               0x42425448
#define    NO_ERR_BLOCK             0xFFFFFFFF
#define    BLOCK_INFO_NUM           1024
#define    FACTORY_BAD_BLOCK        0x00
#define    ECC_CORRECT_BLOCK        0x01
#define    RUN_TIME_BAD_BLOCK       0x02
#define    ECC_CORR_NOTFIX_BLOCK    0x03
#define    GOOD_BLOCK               0x0f
#define    CIS_PART_NAME            "CIS"



typedef struct block_info //1 byte records the status of 2 blocks
{
    u8 b0 : 4;    //block0, 0000:factory bad block  0001:Ecc correctable  0010:running-time bad block  1111:good block
    u8 b1 : 4;    //block0, 0000:factory bad block  0001:Ecc correctable  0010:running-time bad block  1111:good block
}BLOCK_INFO_t;

typedef struct bbt_info
{
    u32 u32_Header;                               //the header of bbt 0x42425448(bbth)
    u32 u32_Crc32;                                //the crc value of bbt
    BLOCK_INFO_t stBlkInfo[BLOCK_INFO_NUM];       //record the blocks' status,the maximum Block number is 1024*2=2048
}BBT_INFO_t;

u8 nand_load_bbt(u32 u32_address);
u8 nand_save_bbt(u32 u32_address, u8 u8_update);
u8 nand_bbt_get_blk_info(u32 u32Offset);
void nand_bbt_fill_blk_info(u32 u32Offset, u32 u32_address, u8 u8BlkType);

#endif
