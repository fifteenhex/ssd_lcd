/*
* hal_pnl.c- Sigmastar
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

#define _HAL_PNL_C_


//#include "cam_os_wrapper.h"
#include "drv_pnl_os.h"


#include "hal_pnl_common.h"


#include "pnl_debug.h"
#include "hal_pnl_util.h"
#include "hal_pnl_reg.h"
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl_lpll_tbl.h"

//#include "cam_clkgen.h"


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DCLK_12D5MHZ       12500000
#define DCLK_25MHZ         25000000
#define DCLK_50MHZ         50000000
#define DCLK_100MHZ        100000000
#define DCLK_187D5MHZ      187500000

#define IsDclkLess12D5M(Dclk)     ((Dclk) <= DCLK_12D5MHZ)
#define IsDclk12D5MTo25M(Dclk)    ((Dclk > DCLK_12D5MHZ) && (Dclk <= DCLK_25MHZ))
#define IsDclk25MTo50M(Dclk)      ((Dclk > DCLK_25MHZ) && (Dclk <= DCLK_50MHZ))
#define IsDclk50MTo100M(Dclk)     ((Dclk > DCLK_50MHZ) && (Dclk <= DCLK_100MHZ))
#define IsDclk100MTo187D5M(Dclk)  ((Dclk > DCLK_100MHZ) && (Dclk <= DCLK_187D5MHZ))


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 gu32JTAGmode = 0;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

u16 HalPnlGetLpllIdx(u32 u32Dclk)
{
    u16 u16Idx = 0;

	W2BYTEMSK(0x1038CC, 0x0c, 0x0F);
    if(IsDclkLess12D5M(u32Dclk))
    {
        u16Idx = 0xFF;
    }
    else if(IsDclk12D5MTo25M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ;
    }
    else if(IsDclk25MTo50M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ;
    }
    else if(IsDclk50MTo100M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ;
    }
    else if(IsDclk100MTo187D5M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ;
    }
    else
    {
        u16Idx = 0xFF;
    }
    return u16Idx;
}


void HalPnlSetClkSrc(u8 u8ClkSrc)
{
    if(u8ClkSrc == 0)
        W2BYTEMSK(0x1038CC, 0x0008, 0x000F);///
    else
        W2BYTEMSK(0x1038CC, 0x000C, 0x000F);
}

u16 HalPnlGetLpllGain(u16 u16Idx)
{
    return u16LoopGain[u16Idx];
}

u16 HalPnlGetLpllDiv(u16 u16Idx)
{
    return u16LoopDiv[u16Idx];
}


void HalPnlSetChipTop(bool bEn, HalPnlOutputFormatBitMode_e enOutBitMode, u8 u8PadMode)
{
    #if 0
    struct device_node *pDevNode;
	u32 u32ttlMode;
	u16 nConfigValue;
	int ret;
	pDevNode = of_find_compatible_node(NULL, NULL, "sstar,pnl");
    if(pDevNode)
    {
    	 ret = CamofPropertyReadU32(pDevNode, "ttl-24bit-mode", &u32ttlMode);
		 if (ret == 0)
         {
		 	//printk("%s, %ld, ttl 24 bit pad mux \n",__FUNCTION__,u32ttlMode);
		 	nConfigValue = R2BYTE(0x103CD8);
		 	nConfigValue &= ~(0x01<<4);
         	nConfigValue |= (u32ttlMode<<4);
         	W2BYTEMSK(0x103CD8, 0xFFFF, nConfigValue);
		 }
		 ret = CamofPropertyReadU32(pDevNode, "ttl-16bit-mode", &u32ttlMode);
		 if (ret == 0)
         {
		 	//printk("%s, %ld, ttl 16 bit pad mux \n",__FUNCTION__,u32ttlMode);

		 	nConfigValue = R2BYTE(0x103CD8);
		 	nConfigValue &= ~(0x07<<0);
         	nConfigValue |= (u32ttlMode<<0);
         	W2BYTEMSK(0x103CD8, 0xFFFF, nConfigValue);
		 }
		 
    }
    #else
    u16 nConfigValue;
    int ret;
    char *cmdline;
    long unsigned int u32PadMode;
    if(enOutBitMode == E_HAL_PNL_OUTPUT_565BIT_MODE) {

        cmdline = getenv("ttl_16bit_pad");
        if(cmdline == NULL) {
            PNL_ERR("[HALPNL] %s %d:: get env ttl_16bit_pad failed, getenv return null \n",__FUNCTION__, __LINE__);
            return;
        }
        ret = strict_strtoul(cmdline, 10, (long unsigned int *)&u32PadMode);
        if(ret)
            PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL] %s %d:: 16bit PadMode (%d)\n",__FUNCTION__, __LINE__,(int)u32PadMode);

        nConfigValue = R2BYTE(0x103CD8);
        nConfigValue &= ~(0x03);
        nConfigValue |= (u32PadMode);

        if(bEn == 1) {
            W2BYTEMSK(0x103CD8, nConfigValue ,0x0007 );
        }
        else {
            W2BYTEMSK(0x103CD8, 0           , 0x0007);
        }
    }
    else {
        nConfigValue = R2BYTE(0x103CD8);
        nConfigValue &= ~(0x01<<4);
        nConfigValue |= (1<<4);
        W2BYTEMSK(0x103CD8, nConfigValue , 0x0010);
    }
    #endif
}


void HalPnlUnifiedSetTTLPadMux(HalPnlOutputFormatBitMode_e enOutputFormat, u32 u32Mode)
{
    u16 nConfigValue = 0;

    if (enOutputFormat == E_HAL_PNL_OUTPUT_565BIT_MODE)
    {
        nConfigValue = R2BYTE(0x103CD8);
        nConfigValue &= ~(0x07<<0);
        nConfigValue |= (u32Mode<<0);
        W2BYTEMSK(0x103CD8, 0xFFFF, nConfigValue);
    }
    else
    {
        nConfigValue = R2BYTE(0x103CD8);
        nConfigValue &= ~(0x01<<4);
        nConfigValue |= (u32Mode<<4);
        W2BYTEMSK(0x103CD8, 0xFFFF, nConfigValue);
    }
}

void HalPnlUnifiedSetBT656PadMux(u8 u8Mode)
{

    u16 nConfigValue = 0;
    nConfigValue = R2BYTE(0x103CC0);
    nConfigValue &= ~(0x03<<4);
    nConfigValue |= (u8Mode<<4);
    W2BYTEMSK(0x103CC0, 0xFFFF, nConfigValue);
}

void HalPnlIfSetOutFormatConfig(u16 eFormatMode)
{
  //printk("%s, %d, mode \n",__FUNCTION__,eFormatMode);
  if(eFormatMode == 3)
  {
    W2BYTEMSK(REG_SCL2_10_L, 0x8000, 0x8000);
    W2BYTEMSK(REG_SCL2_24_L, 0x80, 0x80);
  }
  else{
    W2BYTEMSK(REG_SCL2_10_L, 0x0000, 0x8000);
    W2BYTEMSK(REG_SCL2_24_L, 0x0000, 0x80);
  }
}
void HalPnlDumpLpllSetting(u16 u16Idx)
{
    #if 1
    u16 u16RegIdx;

    for(u16RegIdx=0; u16RegIdx < HAL_PNL_LPLL_REG_NUM; u16RegIdx++)
    {

        if(LPLLSettingTBL[u16Idx][u16RegIdx].address == 0xFF)
        {
            //DrvSclOsDelayTask(LPLLSettingTBL[u16Idx][u16RegIdx].value);
            continue;
        }

        W2BYTEMSK((REG_SCL_LPLL_BASE | ((u32)LPLLSettingTBL[u16Idx][u16RegIdx].address *2)),
                  LPLLSettingTBL[u16Idx][u16RegIdx].value,
                  LPLLSettingTBL[u16Idx][u16RegIdx].mask);
    }
    #endif

}

void HalPnlSetLpllSet(u32 u32LpllSet)
{

    #if 1
    u16 u16LpllSet_Lo, u16LpllSet_Hi;

    u16LpllSet_Lo = (u16)(u32LpllSet & 0x0000FFFF);
    u16LpllSet_Hi = (u16)((u32LpllSet & 0x00FF0000) >> 16);
    W2BYTE(REG_SCL_LPLL_48_L, u16LpllSet_Lo);
    W2BYTE(REG_SCL_LPLL_49_L, u16LpllSet_Hi);
    #endif
}



void HalPnlSetVSyncSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_01_L, u16Val, 0x07FF);
}

void HalPnlSetVSyncEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_02_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_03_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_04_L, u16Val, 0x07FF);
}

void HalPnlSetVdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_05_L, u16Val, 0x07FF);
}

void HalPnlSetVdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_06_L, u16Val, 0x07FF);
}

void HalPnlSetVtt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_07_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_09_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0A_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0B_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0C_L, u16Val, 0x07FF);
}

void HalPnlSetHdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0D_L, u16Val, 0x07FF);
}

void HalPnlSetHdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0E_L, u16Val, 0x07FF);
}

void HalPnlSetHtt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0F_L, u16Val, 0x07FF);
}

void HalPnlSetOpenLpllCLK(u8 bLpllClk)
{

    //W2BYTEMSK(REG_SCL_CLK_66_L, bLpllClk ? 0x000C : 0x0001, 0x000F);
}

void HalPnlSetFrameColorEn(bool bEn)
{
    W2BYTEMSK(REG_SCL2_10_L, bEn ? BIT8 : 0, BIT8);
}

void HalPnlSetFrameColor(u16 u16R, u16 u16G, u16 u16B)
{
    W2BYTE(REG_SCL2_11_L, (u16B | (u16G << 8)));
    W2BYTE(REG_SCL2_12_L, u16R);
}

void HalPnlSetClkInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT0 : 0, BIT0);
}

void HalPnlSetVsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT1 : 0, BIT1);
}

void HalPnlSetHsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT2 : 0, BIT2);
}

void HalPnlSetDeInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT3 : 0, BIT3);
}

void HalPnlSetVsynRefMd(bool bEn)
{
    W2BYTEMSK(REG_SCL0_2D_L, bEn ? BIT4 : 0, BIT4);
}

void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk)
{
    W2BYTEMSK(u32Reg, u16Val, u16Msk);
}

void HalPnlResetOdclk(bool bEn)
{
    W2BYTEMSK(REG_SCL0_01_L, bEn ? BIT2 : 0, BIT2);
}
