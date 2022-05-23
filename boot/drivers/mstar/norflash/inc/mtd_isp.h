/*
* mtd_isp.h- Sigmastar
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

#ifndef _MTD_ISP_H_
#define _MTD_ISP_H_
#include <common.h>

typedef struct {
    u32 u32_eraseSize;
    u32 u32_writeSize;
    u32 u32_capacity;
} FLASH_INFO_t;
#define FLASH_CIS_LOAD_OFFSET           0x08
#define FLASH_SEARCH_END                (0x10000)

#define FLASH_DEFAULT_SNI_OFFSET        0x20000

#define FLASH_GET_OFFSET(offset)            (SPI_BASE_ADDR + (offset))

#define BIT(x)                  (1 << (x))
#define BITS(_bits_)            (BIT(((1)? _bits_) + 1) - BIT(((0)? _bits_)))
#define BIT_FILTER(x)           while(1) {\
                                if ((x) & 0x1) break; \
                                (x) = (x) >> 1; \
                                \}

#endif
