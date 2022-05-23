/*
* mhal_hdmitx.h- Sigmastar
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


/**
 * \defgroup HAL_HDMITX_group  HAL_HDMITX driver
 * @{
 */
#ifndef __MHAL_HDMITX_H__
#define __MHAL_HDMITX_H__

#include "mhal_hdmitx_datatype.h"
//=============================================================================
// API
//=============================================================================


#ifndef __MHAL_HDMITX_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

// Create / Get/ Destroy Instance
INTERFACE MhalHdmitxRet_e MhalHdmitxCreateInstance(void **pCtx, u32 u32Id);
INTERFACE MhalHdmitxRet_e MhalHdmitxGetInstance(void **pCtx, u32 u32Id);
INTERFACE MhalHdmitxRet_e MhalHdmitxDestroyInstance(void *pCtx);


// Hdmitx Attr
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttrBegin(void *pCtx);
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttr(void *pCtx, MhalHdmitxAttrConfig_t *pstAttrCfg);
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttrEnd(void *pCtx);


// Mute for Video/Audio/AvMute
INTERFACE MhalHdmitxRet_e MhalHdmitxSetMute(void *pCtx, MhalHdmitxMuteConfig_t *pstMuteCfg);

// Output Signal on/off
INTERFACE MhalHdmitxRet_e MhalHdmitxSetSignal(void *pCtx, MhalHdmitxSignalConfig_t *pstSignalCfg);

// Analog Driving Current
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAnalogDrvCur(void *pCtx, MhalHdmitxAnaloDrvCurConfig_t *pstDrvCurCfg);

// info Frame
INTERFACE MhalHdmitxRet_e MhalHdmitxSetInfoFrame(void *pCtx, MhalHdmitxInfoFrameConfig_t *pstInfoFrameCfg);

// Sink Info
INTERFACE MhalHdmitxRet_e MhalHdmitxGetSinkInfo(void *pCtx, MhalHdmitxSinkInfoConfig_t *pstSinkInfoCfg);

// Debug Level
INTERFACE MhalHdmitxRet_e MhalHdmitxSetDebugLevel(void *pCtx, u32 u32DbgLevel);

// Hpd GpioNum
INTERFACE MhalHdmitxRet_e MhalHdmitxSetHpdConfig(void *pCtx, MhalHdmitxHpdConfig_t *pstHpdCfg);

#endif
