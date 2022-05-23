/*
* hal_disp_mop_if.c- Sigmastar
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

#define _HAL_DISP_MOP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "hal_disp_common.h"

#include "hal_disp_mop.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PARSING_MOP_ROT_ANGLE(x) (\
                            x == E_HAL_DISP_ROTATE_NONE    ? "Rotate 0"   :   \
                            x == E_HAL_DISP_ROTATE_90      ? "Rotate 90"  :   \
                            x == E_HAL_DISP_ROTATE_180     ? "Rotate 180" :   \
                            x == E_HAL_DISP_ROTATE_270     ? "Rotate 270" :   \
                            "UNKNOWN")

#define Count_str_align(crop_x) ((crop_x>>4)<<4)

#define Count_end_align(crop_x) (((crop_x)&0xF) ? ((((crop_x)>>4)+1) <<4): (crop_x))


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8 gbMopgInitNum[E_HAL_DISP_MOP_NUM] = {0};//for InputPort

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

HalDispMopId_e _HalDispMopIfMopIdMap(HalDispVideoLayerId_e eVdLyId)
{
    HalDispMopId_e eHalMopId = 0;
/*    switch(eVdLyId)
    {
        case E_HAL_DISP_LAYER_MOPG_ID:
        case E_HAL_DISP_LAYER_MOPS_ID:
            eHalMopId = E_HAL_DISP_MOPID_00;
            break;
        case E_HAL_DISP_LAYER_MOPG_VIDEO1_ID:
        case E_HAL_DISP_LAYER_MOPS_VIDEO1_ID:
            eHalMopId = E_HAL_DISP_MOPID_01;
            break;
        default:
            DISP_ERR("[HALMOP]%s %d, Video Layer ID %d not support\n", __FUNCTION__, __LINE__,eVdLyId);
            eHalMopId = E_HAL_DISP_MOPID_00;
            break;
    }
*/
    return eHalMopId;
}

u8 _HalDispMopIfFmtBpp(HalDispPixelFormat_e eFmt)
{
    u8 u8Bpp = 1;
    switch(eFmt)
    {
        case E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV:
            u8Bpp = 2;
            break;
        case E_HAL_DISP_PIXEL_FRAME_ARGB8888:
            u8Bpp = 4;
            break;
        default:
            u8Bpp = 1;
            break;
    }

    return u8Bpp;
}


//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
HalDispQueryRet_e HalDispMopIfGetInfoInputPortInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstInputPortContain->u16FlipFrontPorchCnt = 0;

    return enRet;
}

void HalDispMopIfSetInputPortInit(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    static bool sbMopsHwInit = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;


    if(pstVidLayerContain)
    {
        if(sbMopsHwInit==0)
        {
            HalDispMopsInit(pCtx);
            sbMopsHwInit=1;
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld VidoeLayer Not Bind\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEnable = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pbEnable = (bool *)pCfg;

    if(pstInputPortContain->bEnable == 0 && *pbEnable == 1)
    {
        pstInputPortContain->u16FlipFrontPorchCnt = 0;
    }

    pstInputPortContain->bEnable = *pbEnable;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, video_id=%ld, Port_id:%ld settings\n", __FUNCTION__, __LINE__,pstVidLayerContain->u32VidLayerId, pstInputPortContain->u32PortId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d\n", __FUNCTION__, __LINE__, *pbEnable);

    return enRet;
}

void HalDispMopIfSetInputPortEnable(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEnable = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pbEnable = (bool *)pCfg;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)&&
       (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        HalDispMopsGwinEn(pCtx, eMopId, *pbEnable);
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;

    memcpy(&pstInputPortContain->stAttr, pstHalInputPortCfg, sizeof(HalDispInputPortAttr_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, video_id=%ld, Port_id:%ld settings\n", __FUNCTION__, __LINE__,pstVidLayerContain->u32VidLayerId, pstInputPortContain->u32PortId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Disp(%d %d %d %d) Src(%d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->stDispWin.u16X, pstHalInputPortCfg->stDispWin.u16Y,
        pstHalInputPortCfg->stDispWin.u16Width, pstHalInputPortCfg->stDispWin.u16Height,
        pstHalInputPortCfg->u16SrcWidth, pstHalInputPortCfg->u16SrcHeight);

    return enRet;
}

void HalDispMopIfSetInputPortAttr(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u32 u16Hratio = 0x100000;
    u32 u16Vratio = 0x100000;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)&&
       (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        HalDispMopsSetSourceParam(pCtx, eMopId, pstHalInputPortCfg->u16SrcWidth,
                                  pstHalInputPortCfg->u16SrcHeight);
        HalDispMopsSetGwinParam(pCtx, eMopId, pstHalInputPortCfg->stDispWin.u16X,
                                pstHalInputPortCfg->stDispWin.u16Y,
                                pstHalInputPortCfg->stDispWin.u16Width,
                                pstHalInputPortCfg->stDispWin.u16Height);
        HalDispMopsSetHvspIn(pCtx,  eMopId,
                              pstHalInputPortCfg->u16SrcWidth,
                              pstHalInputPortCfg->u16SrcHeight);
        HalDispMopsSetHvspOut(pCtx,  eMopId,
                              pstHalInputPortCfg->stDispWin.u16Width,
                              pstHalInputPortCfg->stDispWin.u16Height);
        if(pstHalInputPortCfg->stDispWin.u16Width > pstHalInputPortCfg->u16SrcWidth)
        {
            u16Hratio = ((pstHalInputPortCfg->u16SrcWidth*1048576)/pstHalInputPortCfg->stDispWin.u16Width);
            HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
        }
        else
        {
            u16Hratio = 0x100000;
            HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
        }
        if(pstHalInputPortCfg->stDispWin.u16Height > pstHalInputPortCfg->u16SrcHeight)
        {
            u16Vratio = ((pstHalInputPortCfg->u16SrcHeight*1048576)/pstHalInputPortCfg->stDispWin.u16Height);
            HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
        }
        else
        {
            u16Vratio = 0x100000;
            HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortFlip(void *pCtx, void *pCfg)
{
    u16 i;
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVideoFrameData_t *pstFramedata = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    //DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    //pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[0]; // only device_0 is valid
    pstFramedata = (HalDispVideoFrameData_t *)pCfg;

    if(enRet == E_HAL_DISP_QUERY_RET_OK)
    {
        memcpy(&pstInputPortContain->stFrameData, pstFramedata, sizeof(HalDispVideoFrameData_t));

        for(i=0; i<3; i++)
         {
             if(pstInputPortContain->stFrameData.au64PhyAddr[i]&0xF)//physical address should be 16 align
             {
                 enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                 DISP_ERR("%s %d, PHY_ADDR should be 16 align, addr[%d](%08llx)\n", __FUNCTION__, __LINE__,i, pstInputPortContain->stFrameData.au64PhyAddr[i]);
             }

             if(pstInputPortContain->stFrameData.au32Stride[i]&0xF)//stride should be 16 align
             {
                 enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                 DISP_ERR("%s %d, Stride should be 16 align, addr[%d](%ld)\n", __FUNCTION__, __LINE__,i, pstInputPortContain->stFrameData.au32Stride[i]);
             }
         }

        if(pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV ||
           pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422)
        {
            for(i=0; i<3; i++)
            {
                pstInputPortContain->stFrameData.au32Stride[i] /= 2;

               if(i==1)
               {
                   pstInputPortContain->stFrameData.au64PhyAddr[1] =
                    pstInputPortContain->stFrameData.au64PhyAddr[0] + 0x10;
               }
            }
        }

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Fmt:%s, PhyAddr(%08llx, %08llx, %08llx), Stride(%ld %ld %ld)\n",
            __FUNCTION__, __LINE__,
            PARSING_HAL_PIXEL_FMT(pstInputPortContain->stFrameData.ePixelFormat),
            pstInputPortContain->stFrameData.au64PhyAddr[0], pstInputPortContain->stFrameData.au64PhyAddr[1], pstInputPortContain->stFrameData.au64PhyAddr[2],
            pstInputPortContain->stFrameData.au32Stride[0], pstInputPortContain->stFrameData.au32Stride[1], pstInputPortContain->stFrameData.au32Stride[2]);

    }

    return enRet;
}

void HalDispMopIfSetInputPortFlip(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u64 u64Yaddr=0;
    u64 u64Caddr=0;
    u8 u8AddrOffset=0;
    HalDispMopId_e eMopId = 0;
    u8 u8Mlt = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);


    u8Mlt = _HalDispMopIfFmtBpp(pstInputPortContain->stFrameData.ePixelFormat);

    //Y Address, after counting crop, Yaddr = Yaddr + offset_y*stride + offset_x
    u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0] +
               pstInputPortContain->stCrop.u16Y * pstInputPortContain->stFrameData.au32Stride[0]*u8Mlt+
               (((pstInputPortContain->stCrop.u16X>>4)<<4))*u8Mlt;
    //C Address, after counting crop, Caddr = Caddr + offset_y/2*stride + offset_x
    if((pstInputPortContain->stFrameData.ePixelFormat==E_HAL_DISP_PIXEL_FRAME_YUV_MST_420)||
       (pstInputPortContain->stFrameData.ePixelFormat==E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_420))
    {
        u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1] +
                   (pstInputPortContain->stCrop.u16Y/2) * pstInputPortContain->stFrameData.au32Stride[0]+
                   ((pstInputPortContain->stCrop.u16X>>4)<<4);
    }
    else
    {
        u64Caddr = 0;
    }

    u8AddrOffset = (u8)(pstInputPortContain->stCrop.u16X & 0xF);

    if((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)&&
       (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        HalDispMopsSetYAddr(pCtx, eMopId, u64Yaddr);
        HalDispMopsSetCAddr(pCtx, eMopId, u64Caddr);
        HalDispMopsSetPitch(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);
        HalDispMopsSetAddr16Offset(pCtx, eMopId, u8AddrOffset);
        HalDispMopsSetColorFmt(pCtx, eMopId, pstInputPortContain->stFrameData.ePixelFormat);
        if(u8AddrOffset)
        {
            HalDispMopsSetSourceParam(pCtx, eMopId, (((pstInputPortContain->stAttr.u16SrcWidth+u8AddrOffset)>>4)+1)<<4,pstInputPortContain->stAttr.u16SrcHeight);
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void HalDispMopIfSetInputPortRotate(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVidWinRect_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstHalInputPortCfg = (HalDispVidWinRect_t *)pCfg;

    if((pstHalInputPortCfg->u16X&0x1)||
       (pstHalInputPortCfg->u16Y&0x1)||
       (pstHalInputPortCfg->u16Width&0x1)||
       (pstHalInputPortCfg->u16Height&0x1))//crop parameters should be all 2 align
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, crop parameters should be 2 align\n", __FUNCTION__, __LINE__);
        DISP_ERR("%s %d, Crop(%d %d %d %d) not support\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->u16X, pstHalInputPortCfg->u16Y,
        pstHalInputPortCfg->u16Width, pstHalInputPortCfg->u16Height);
    }

    memcpy(&pstInputPortContain->stCrop, pstHalInputPortCfg, sizeof(HalDispVidWinRect_t));

    pstInputPortContain->stAttr.u16SrcWidth = pstHalInputPortCfg->u16Width;
    pstInputPortContain->stAttr.u16SrcHeight = pstHalInputPortCfg->u16Height;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Crop(%d %d %d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->u16X, pstHalInputPortCfg->u16Y,
        pstHalInputPortCfg->u16Width, pstHalInputPortCfg->u16Height);

    return enRet;
}



void HalDispMopIfSetInputPortCrop(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVidWinRect_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u32 u32Hratio = 0x100000;
    u32 u32Vratio = 0x100000;
    HalDispMopId_e eMopId = 0;
    u16 u16TempWidth = 0;
    u16 u16Offset = 0;
    u16 u16Crop16Offset = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispVidWinRect_t *)pCfg;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Crop(%d %d %d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->u16X, pstHalInputPortCfg->u16Y,
        pstHalInputPortCfg->u16Width, pstHalInputPortCfg->u16Height);

    if((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)&&
       (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        u16Crop16Offset = (pstInputPortContain->stCrop.u16X & 0xF);
        u16TempWidth = pstHalInputPortCfg->u16Width + u16Crop16Offset;
        u16Offset = (u16TempWidth & 0xF);

        if(u16Offset || u16Crop16Offset)
        {
            HalDispMopsHwCropEn(pCtx, eMopId, 1);
            if(u16Offset)
            {
                u16TempWidth = (((u16TempWidth>>4)+1)<<4);
            }
        }
        else
        {
            HalDispMopsHwCropEn(pCtx, eMopId, 0);
        }
        HalDispMopsHwCropHst(pCtx, eMopId, u16Crop16Offset);
        HalDispMopsHwCropHsize(pCtx, eMopId, pstHalInputPortCfg->u16Width);
        HalDispMopsHwCropVsize(pCtx, eMopId, pstHalInputPortCfg->u16Height);
        HalDispMopsHwCropInSize(pCtx, eMopId, u16TempWidth, pstHalInputPortCfg->u16Height);
        HalDispMopsSetHvspIn(pCtx,  eMopId,
                              pstHalInputPortCfg->u16Width,
                              pstHalInputPortCfg->u16Height);
        HalDispMopsSetSourceParam(pCtx, eMopId, u16TempWidth,
                                  pstHalInputPortCfg->u16Height);
        if(pstInputPortContain->stAttr.stDispWin.u16Width > pstHalInputPortCfg->u16Width)
        {
            u32Hratio = ((pstHalInputPortCfg->u16Width*1048576)/pstInputPortContain->stAttr.stDispWin.u16Width);
            HalDispMopsSetHScaleFac(pCtx, eMopId, u32Hratio);
        }
        else
        {
            u32Hratio = 0x100000;
            HalDispMopsSetHScaleFac(pCtx, eMopId, u32Hratio);
        }
        if(pstInputPortContain->stAttr.stDispWin.u16Height > pstHalInputPortCfg->u16Height)
        {
            u32Vratio = ((pstHalInputPortCfg->u16Height*1048576)/pstInputPortContain->stAttr.stDispWin.u16Height);
            HalDispMopsSetVScaleFac(pCtx, eMopId, u32Vratio);
        }
        else
        {
            u32Vratio = 0x100000;
            HalDispMopsSetVScaleFac(pCtx, eMopId, u32Vratio);
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void HalDispMopIfSetInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e HalDispMopIfGetInfoStretchWinSize(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispVideoLayerAttr_t *stVdInf = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    stVdInf = (HalDispVideoLayerAttr_t *)pCfg;

    memcpy(&pstVidLayerContain->stAttr, stVdInf, sizeof(HalDispVideoLayerAttr_t));

    return enRet;
}

void HalDispMopIfSetInfoStretchWinSize(void *pCtx, void *pCfg)
{
/*    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispMopId_e eMopId = 0;
    HalDispVideoLayerAttr_t *stVdInf = NULL;
    u16 u16Width = 0;
    u16 u16Height = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    stVdInf = (HalDispVideoLayerAttr_t *)pCfg;

    u16Width = stVdInf->stVidLayerDispWin.u16Width;
    u16Height = stVdInf->stVidLayerDispWin.u16Height;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    HalDispMopgSetStretchWinSize(pCtx, eMopId, u16Width, u16Height);
    HalDispMopsSetStretchWinSize(pCtx, eMopId, u16Width, u16Height);
*/
}

void HalDispMopIfSetInfoHextSize(void *pCtx, void *pCfg)
{
/*    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    DrvDispCtxDeviceContain_t *pstDevContain;
    HalDispMopId_e eMopId = 0;
    u16 u16Hext = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    pstDevContain = (DrvDispCtxDeviceContain_t *)pstVidLayerContain->pstDevCtx;

    u16Hext = pstDevContain->stDevTimingCfg.u16Htotal - pstDevContain->stDevTimingCfg.u16Hactive - 9;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, debug, Htotal=%d, Hactive=%d, Hext=%d\n", __FUNCTION__, __LINE__, pstDevContain->stDevTimingCfg.u16Htotal
                                                                  , pstDevContain->stDevTimingCfg.u16Hactive, u16Hext);

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    HalDispMopsSetHextSize(pCtx, eMopId, u16Hext);
*/
}

void HalDispMopIfInputPortDeinit(void)
{

}

void HalDispMopIfSetInfoStretchWinMop0Auto(void *pCtx, bool bEnAuto)
{
//    HalDispMopgSetAutoStretchWinSizeEn(pCtx, E_HAL_DISP_MOPID_00, bEnAuto);
//    HalDispMopsSetAutoStretchWinSizeEn(pCtx, E_HAL_DISP_MOPID_00, bEnAuto);
}

void HalDispMopIfSetDdbfWr(void *pCtx)
{
/*    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_00,E_HAL_DISP_MOPROT_ROT0_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_00,E_HAL_DISP_MOPROT_ROT1_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_01,E_HAL_DISP_MOPROT_ROT0_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_01,E_HAL_DISP_MOPROT_ROT1_ID);

    HalDispMopDbBfWr(pCtx, E_HAL_DISP_MOPID_00);
    HalDispMopDbBfWr(pCtx, E_HAL_DISP_MOPID_01);
*/}

