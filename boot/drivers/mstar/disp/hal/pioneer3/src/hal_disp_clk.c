/*
* hal_disp_clk.c- Sigmastar
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

#define _HAL_DISP_CLK_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "hal_disp_clk.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
void HalDispSetClkOdclk0(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0 : 1;
    u16Val |= ( u32ClkRate & 0x03) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_SC_GP_CTRL_35_L, u16Val, 0x000F);
}


void HalDispGetClkOdclk0(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_SC_GP_CTRL_35_L) & 0x000F;

    *pbEn = (u16Val & 0x01) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x03;
}

void HalDispSetClkOdclk1(bool bEn, u32 u32ClkRate)
{
/*    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= ( u32ClkRate & 0x03) << 10;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_22_L, u16Val, 0x0F00);
*/}


void HalDispGetClkOdclk1(bool *pbEn, u32 *pu32ClkRate)
{
/*    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_22_L) & 0x00F0;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x03;
*/}

void HalDispSetClkLcd(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0 : 1;
    u16Val |= (u32ClkRate & 0x03) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_SC_GP_CTRL_38_L, u16Val, 0x000F);
}

void HalDispGetClkLcd(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_SC_GP_CTRL_38_L);
    *pbEn = u16Val & 0x01 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x03;
}

void HalDispSetClkLcdSrc(bool bEn, u32 u32ClkRate)
{
/*    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x03) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_24_L, u16Val, 0x000F);
*/}

void HalDispGetClkLcdSrc(bool *pbEn, u32 *pu32ClkRate)
{
/*    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_24_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x03;
*/}

u32 HalDispClkMapMopToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(240) ? 0x03 :
                u32ClkRate <= CLK_MHZ(320) ? 0x00 :
                u32ClkRate <= CLK_MHZ(384) ? 0x01 :
                u32ClkRate <= CLK_MHZ(480) ? 0x02 :
                                             0x02;
    return u32ClkIdx;
}

u32 HalDispClkMapMopToRate(u32 u32ClkIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkIdx == 0x00 ? CLK_MHZ(320) :
                 u32ClkIdx == 0x01 ? CLK_MHZ(384) :
                 u32ClkIdx == 0x02 ? CLK_MHZ(288) :
                 u32ClkIdx == 0x03 ? CLK_MHZ(240) :
                                     CLK_MHZ(320);
    return u32ClkRate;
}

void HalDispClkSetMop(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x0007) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_54_L, u16Val, 0x000F);
}

void HalDispClkGetMop(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_54_L) & 0x000F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x0007;
}
u32 HalDispClkMapDisp432ToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(432) ? 0x00 : 0x00;
    return u32ClkIdx;
}

u32 HalDispClkMapDisp432ToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(432) : 0;
    return u32ClkRate;
}

void HalDispClkSetDisp432(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x0003) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_53_L, u16Val, 0x000F);
}

void HalDispClkGetDisp432(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_53_L) & 0x000F;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x0003;
}

u32 HalDispClkMapDisp216ToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(216) ? 0x00 : 0x00;
    return u32ClkIdx;
}

u32 HalDispClkMapDisp216ToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(216) : 0;
    return u32ClkRate;
}

void HalDispClkSetDisp216(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x1000;
    u16Val |= ( u32ClkRate & 0x0003) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_53_L, u16Val, 0x0F00);
}

void HalDispClkGetDisp216(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_53_L) & 0x0F00;

    *pbEn = (u16Val & 0x1000) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0003;
}

u32 HalDispClkMapDispPixelToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate == 10 ? 0x07 :
                u32ClkRate <= CLK_MHZ(54)  ? 0x05 :
                u32ClkRate <= CLK_MHZ(123) ? 0x02 :
                u32ClkRate <= CLK_MHZ(36)  ? 0x04 :
                u32ClkRate <= CLK_MHZ(144) ? 0x03 :
                u32ClkRate <= CLK_MHZ(172) ? 0x00 :
                u32ClkRate <= CLK_MHZ(192) ? 0x01 :
                                             0x06;
    return u32ClkIdx;
}

u32 HalDispClkMapDispPixelToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0x07? 10 :
                 u32ClkRateIdx == 0x05? CLK_MHZ(54) :
                 u32ClkRateIdx == 0x02? CLK_MHZ(123) :
                 u32ClkRateIdx == 0x04? CLK_MHZ(36) :
                 u32ClkRateIdx == 0x03? CLK_MHZ(144) :
                 u32ClkRateIdx == 0x00? CLK_MHZ(172) :
                 u32ClkRateIdx == 0x01? CLK_MHZ(192) :
                                        0x06;
    return u32ClkRate;
}


void HalDispClkSetDispPixel(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x000F) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_63_L, u16Val, 0x001F);
}

void HalDispClkGetDispPixel(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_63_L) & 0x001F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x000F;
}

void HalDispClkSetGpCtrlCfg(void *pCtx)
{
    // Odclk_0
    HalDispSetClkOdclk0(1, HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK);

    // disp_pixel_0
    HalDispClkSetDispPixel(1, HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL);
}


void HalDispClkInit(bool bEn)
{
    HalDispSetClkOdclk0(bEn, HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK);
}
