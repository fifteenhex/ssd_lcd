/*
* drv_hdmitx_ctx.h- Sigmastar
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

#ifndef _DRV_HDMITX_CTX_H_
#define _DRV_HDMITX_CTX_H_


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------


typedef struct
{
    bool bInit;
    bool bAvMute;
    bool bVideoOnOff;
    bool bAudioOnOff;
    bool bSignal;
    HalHdmitxStatusFlag_e enStatusFlag;
    HalHdmitxHpdConfig_t stHpdCfg;
    HalHdmitxAnaloDrvCurConfig_t stAnalogDrvCurCfg;
    HalHdmitxAttrConfig_t stAttrCfg;
    HalHdmitxInfoFrameConfig_t stInfoFrameCfg;
}DrvHdmitxCtxHalContain_t;

typedef struct
{
    bool bUsed;
    s32  s32CtxId;
    DrvHdmitxCtxHalContain_t *pstHalCtx;
}DrvHdmitxCtxConfig_t;

typedef struct
{
    s32  s32CtxId;
}DrvHdmitxCtxAllocConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _DRV_HDMITX_CTX_C_
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE bool DrvHdmitxCtxInit(void);
INTERFACE bool DrvHdmitxCtxDeInit(void);
INTERFACE bool DrvHdmitxCtxAllocate(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx);
INTERFACE bool DrvHdmitxCtxFree(DrvHdmitxCtxConfig_t *pCtx);
INTERFACE bool DrvHdmitxCtxIsAllFree(void);
INTERFACE bool DrvHdmitxCtxGet(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx);



#undef INTERFACE

#endif
