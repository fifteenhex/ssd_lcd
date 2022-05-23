/*
* hal_disp_mop.h- Sigmastar
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

#ifndef _HAL_DISP_MOP_H_
#define _HAL_DISP_MOP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_HAL_DISP_MOPID_00     = 0,
    E_HAL_DISP_MOP_NUM      = 1,
} HalDispMopId_e;


typedef enum
{
    E_HAL_DISP_MOPS_ID_00     = 0,
} HalDispMopgGwinId_e;

typedef enum
{
    E_HAL_DISP_LAYER_MOPS_ID        = 0,
    E_HAL_DISP_LAYER_ID_NUM         = 1,
} HalDispVideoLayerId_e;

typedef enum
{
    E_HAL_DISP_MOPROT_NUM      = 0,
} HalDispMopRotId_e;
/*
typedef enum
{
    E_HAL_DISP_LAYER_MOPG01_01          = 0,
    E_HAL_DISP_LAYER_MOPG01_11          = 1,
    E_HAL_DISP_LAYER_MOPG01_21          = 2,
    E_HAL_DISP_LAYER_MOPG01_31          = 3,
    E_HAL_DISP_LAYER_MOPG01_41          = 4,
    E_HAL_DISP_LAYER_MOPG01_51          = 5,
    E_HAL_DISP_LAYER_MOPG01_61          = 6,
    E_HAL_DISP_LAYER_MOPG01_71          = 7,
    E_HAL_DISP_LAYER_MOPG01_SHADOW_NUM  = 8,
} HalDispMopg01SwShadow_e;

typedef enum
{
    E_HAL_DISP_LAYER_MOPG02_01          = 0,
    E_HAL_DISP_LAYER_MOPG02_11          = 1,
    E_HAL_DISP_LAYER_MOPG02_21          = 2,
    E_HAL_DISP_LAYER_MOPG02_31          = 3,
    E_HAL_DISP_LAYER_MOPG02_41          = 4,
    E_HAL_DISP_LAYER_MOPG02_51          = 5,
    E_HAL_DISP_LAYER_MOPG02_61          = 6,
    E_HAL_DISP_LAYER_MOPG02_71          = 7,
    E_HAL_DISP_LAYER_MOPG02_SHADOW_NUM  = 8,
} HalDispMopg02SwShadow_e;


typedef enum
{
    E_HAL_DISP_LAYER_MOPS_51          = 0,
    E_HAL_DISP_LAYER_MOPS_SHADOW_NUM  = 1,
} HalDispMopsSwShadow_e;
*/

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_MOP_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
//SW shadow
INTERFACE void HalMopsShadowInit(void);

//MOPG settings
/*INTERFACE void HalDispMopgSwReset(HalDispMopId_e eMopId, void *pCmdq);
INTERFACE void HalDispMopgSetAutoStretchWinSizeEn(void *pCtx, HalDispMopId_e eMopId, bool bEnAuto);
INTERFACE void HalDispMopgSetStretchWinSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopgSetHextSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Hext);
INTERFACE void HalDispMopgSetPipeDelay(void *pCtx, HalDispMopId_e eMopId, u8 u8PipeDelay);
INTERFACE void HalDispMopgInit(void *pCtx);
INTERFACE void HalDispMopgGwinEn(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, bool bEn);
INTERFACE void HalDispMopgSetYAddr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u32 u32PhyAddr);
INTERFACE void HalDispMopgSetAddr16Offset(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u8 u8Offset);
INTERFACE void HalDispMopgSetCAddr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u32 u32PhyAddr);
INTERFACE void HalDispMopgSetGwinParam(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16Hst, u16 u16Vst, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopgSetPitch(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16Pitch);
INTERFACE void HalDispMopgSetSourceParam(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16SrcWidth, u16 u16SrcHeight);
INTERFACE void HalDispMopgSetHScaleFac(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16HRatio);
INTERFACE void HalDispMopgSetVScaleFac(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16VRatio);
INTERFACE void HalDispMopgSetLineBufStr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16GwinHstr, u8 u8WinPri);
*/
//MOPS settings
INTERFACE void HalDispMopsSwReset(HalDispMopId_e eMopId, void *pCmdq);
INTERFACE void HalDispMopsSetAutoStretchWinSizeEn(void *pCtx, HalDispMopId_e eMopId, bool bEnAuto);
INTERFACE void HalDispMopsSetStretchWinSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopsSetHextSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Hext);
INTERFACE void HalDispMopsSetPipeDelay(void *pCtx, HalDispMopId_e eMopId, u8 u8PipeDelay);
INTERFACE void HalDispMopsInit(void *pCtx);
INTERFACE void HalDispMopsGwinEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopsSetYAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopsSetAddr16Offset(void *pCtx, HalDispMopId_e eMopId, u8 u8Offset);
INTERFACE void HalDispMopsSetCAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopsSetGwinParam(void *pCtx, HalDispMopId_e eMopId, u16 u16Hst, u16 u16Vst, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopsSetPitch(void *pCtx, HalDispMopId_e eMopId, u16 u16Pitch);
INTERFACE void HalDispMopsSetSourceParam(void *pCtx, HalDispMopId_e eMopId, u16 u16SrcWidth, u16 u16SrcHeight);
INTERFACE void HalDispMopsSetHScaleFac(void *pCtx, HalDispMopId_e eMopId, u32 u16HRatio);
INTERFACE void HalDispMopsSetVScaleFac(void *pCtx, HalDispMopId_e eMopId, u32 u16VRatio);
INTERFACE void HalDispMopsSetHvspIn(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopsSetHvspOut(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopsSetColorFmt(void *pCtx, HalDispMopId_e eMopId, HalDispPixelFormat_e eFmt);
INTERFACE void HalDispMopsHwCropEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopsHwCropInSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height);
INTERFACE void HalDispMopsHwCropHst(void *pCtx, HalDispMopId_e eMopId, u16 u16Val);
INTERFACE void HalDispMopsHwCropHsize(void *pCtx, HalDispMopId_e eMopId, u16 u16Val);
INTERFACE void HalDispMopsHwCropVst(void *pCtx, HalDispMopId_e eMopId, u16 u16Val);
INTERFACE void HalDispMopsHwCropVsize(void *pCtx, HalDispMopId_e eMopId, u16 u16Val);

//INTERFACE void HalDispMopsSetRingEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
//INTERFACE void HalDispMopsSetRingSrc(void *pCtx, HalDispMopId_e eMopId, bool bImi);
//INTERFACE void HalDispMopsSetRingBuffHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16RingBufHeight);
//INTERFACE void HalDispMopsSetRingBuffStartLine(void *pCtx, HalDispMopId_e eMopId, u16 u16RingBufStartLine);

//MOP common
INTERFACE void HalDispMopDbBfWr(void *pCtx, HalDispMopId_e eMopId);
INTERFACE void HalDispMopSetClk(bool bEn, u32 u32ClkRate);
INTERFACE void HalDispMopGetClk(bool *pbEn, u32 *pu32ClkRate);

//MOP Rotate
/*INTERFACE void HalDispMopRotDbBfWr(void *pCtx, HalDispMopId_e eMopId, HalDispMopRotId_e eMopRotId);
INTERFACE void HalDispMopRot0DbfEn(HalDispMopId_e eMopId, bool bEn, void *pCmdq);
INTERFACE void HalDispMopRot1DbfEn(HalDispMopId_e eMopId, bool bEn, void *pCmdq);
INTERFACE void HalDispMopRot0En(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot1En(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot0BlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch);
INTERFACE void HalDispMopRot1BlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch);
INTERFACE void HalDispMopRot0SourceWidth(void *pCtx, HalDispMopId_e eMopId, u16 u16inputWidth);
INTERFACE void HalDispMopRot0SourceHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16inputHeight);
INTERFACE void HalDispMopRot1SourceWidth(void *pCtx, HalDispMopId_e eMopId, u16 u16inputWidth);
INTERFACE void HalDispMopRot1SourceHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16inputHeight);
INTERFACE void HalDispMopRot0SetReadYAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot0SetReadCAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot1SetReadYAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot1SetReadCAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot0SetWriteYAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot0SetWriteCAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot1SetWriteYAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot1SetWriteCAddr(void *pCtx, HalDispMopId_e eMopId, u32 u32PhyAddr);
INTERFACE void HalDispMopRot0SetRotateMode(void *pCtx, HalDispMopId_e eMopId, HalDispRotateMode_e eRotAng);
INTERFACE void HalDispMopRot1SetRotateMode(void *pCtx, HalDispMopId_e eMopId, HalDispRotateMode_e eRotAng);
INTERFACE void HalDispMopRot0SetPixDummy(void *pCtx, HalDispMopId_e eMopId, u16 u16DummyPix);
INTERFACE void HalDispMopRot1SetPixDummy(void *pCtx, HalDispMopId_e eMopId, u16 u16DummyPix);
INTERFACE void HalDispMopRot0SetRotateCropXEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot0SetRotateCropYEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot1SetRotateCropXEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot1SetRotateCropYEn(void *pCtx, HalDispMopId_e eMopId, bool bEn);
INTERFACE void HalDispMopRot0SetRotateCropXClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp);
INTERFACE void HalDispMopRot1SetRotateCropXClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp);
INTERFACE void HalDispMopRot0SetRotateCropYClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp);
INTERFACE void HalDispMopRot1SetRotateCropYClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp);
INTERFACE void HalDispMopRotInit(void *pCtx);
*/

#undef INTERFACE
#endif
