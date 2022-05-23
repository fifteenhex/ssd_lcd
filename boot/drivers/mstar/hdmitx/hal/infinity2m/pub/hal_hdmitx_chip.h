/*
* hal_hdmitx_chip.h- Sigmastar
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

#ifndef _HAL_HDMITX_CHIP_H_
#define _HAL_HDMITX_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_HDMITX_CTX_INST_MAX        2


#define HAL_HDMITX_CLK_NUM              3

#define CLK_MHZ(x)                      (x*1000000)
#define HAL_HDMITX_CLK_HDMI             0
#define HAL_HDMITX_CLK_DISP_432         CLK_MHZ(432)
#define HAL_HDMITX_CLK_DISP_216         CLK_MHZ(216)

#define HAL_HDMITX_CLK_ON_SETTING \
{ \
    1, 1, 1,\
}

#define HAL_HDMITX_CLK_OFF_SETTING \
{ \
    0, 0, 0,\
}


#define HAL_HDMITX_CLK_RATE_SETTING \
{ \
    HAL_HDMITX_CLK_HDMI, \
    HAL_HDMITX_CLK_DISP_432, \
    HAL_HDMITX_CLK_DISP_216, \
}

#define HAL_HDMITX_CLK_MUX_ATTR \
{ \
    1, 0, 0, \
}


//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


#endif

