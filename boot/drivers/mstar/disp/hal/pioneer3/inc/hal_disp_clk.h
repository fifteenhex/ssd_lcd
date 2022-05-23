/*
* hal_disp_clk.h- Sigmastar
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

#ifndef _HAL_DISP_CLK_H_
#define _HAL_DISP_CLK_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#define HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL        0x06


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_CLK_GP_CTRL_NONE     = 0x0000,
    E_HAL_DISP_CLK_GP_CTRL_HDMI     = 0x0001,
    E_HAL_DISP_CLK_GP_CTRL_LCD      = 0x0002,
    E_HAL_DISP_CLK_GP_CTRL_DAC      = 0x0004,
    E_HAL_DISP_CLK_GP_CTRL_MIPIDSI  = 0x0008,
    E_HAL_DISP_CLK_GP_CTRL_CVBS     = 0x0010,

    E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC = 0x0005,
}HalDispClkGpCtrlType_e;


typedef struct
{
    bool bEn;
    HalDispClkGpCtrlType_e eType;
}HalDispClkGpCtrlConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_CLK_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void HalDispGetClkOdclk0(bool *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispSetClkOdclk0(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispGetClkOdclk1(bool *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispSetClkOdclk1(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispSetClkLcd(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispGetClkLcd(bool *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispSetClkLcdSrc(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispGetClkLcdSrc(bool *pbEn, u32 *pu32ClkRate);
INTERFACE u32  HalDispClkMapMopToIdx(u32 u32ClkRate);
INTERFACE u32  HalDispClkMapMopToRate(u32 u32ClkIdx);
INTERFACE void HalDispClkSetMop(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetMop(bool *pbEn, u32 *pu32ClkRate);
INTERFACE u32  HalDispClkMapDisp432ToIdx(u32 u32ClkRate);
INTERFACE u32  HalDispClkMapDisp432ToRate(u32 u32ClkRateIdx);
INTERFACE void HalDispClkSetDisp432(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetDisp432(bool *pbEn, u32 *pu32ClkRate);
INTERFACE u32  HalDispClkMapDisp216ToIdx(u32 u32ClkRate);
INTERFACE u32  HalDispClkMapDisp216ToRate(u32 u32ClkRateIdx);
INTERFACE void HalDispClkSetDisp216(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetDisp216(bool *pbEn, u32 *pu32ClkRate);
INTERFACE u32  HalDispClkMapDispPixelToIdx(u32 u32ClkRate);
INTERFACE u32  HalDispClkMapDispPixelToRate(u32 u32ClkRateIdx);
INTERFACE void HalDispClkSetDispPixel(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetDispPixel(bool *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetGpCtrlCfg(void *pCtx);
INTERFACE void HalDispClkInit(bool bEn);
#undef INTERFACE
#endif
