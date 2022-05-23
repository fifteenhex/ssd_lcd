/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "mi_disp.h"
#include "mi_panel.h"
#include "mi_sensor.h"
#include "mi_sensor_datatype.h"


#define USE_SNR_PAD_ID_1  (1)

struct ST_Panel_Attr_T
{
    MI_U32              u32Width;
    MI_U32              u32Height;
    MI_SNR_PAD_ID_e     eSnrPad;
    MI_VIF_DEV          s32vifDev;
};


static struct ST_Panel_Attr_T g_stPanelAttr[]=
{

#if USE_SNR_PAD_ID_1
    {
            .u32Width = 1024,
            .u32Height = 600,
            .eSnrPad   = E_MI_SNR_PAD_ID_0,
            .s32vifDev = 0,
    },
#else
    {
        .u32Width = 1024,
        .u32Height = 600,
        .eSnrPad   = E_MI_SNR_PAD_ID_2,
        .s32vifDev = 1,
    },
#endif
};

#define SENSOR_RGB_FPS 25
void ST_Flush(void)
{
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}


MI_U32 ST_PanelStart(void)
{
    ST_Panel_Attr_T *pstPanelAttr = g_stPanelAttr;

    MI_DISP_DEV DispDev = 0;
    MI_U32 u32DispWidth = pstPanelAttr->u32Width;
    MI_U32 u32DispHeight = pstPanelAttr->u32Height;
    
    MI_PANEL_IntfType_e eIntfType = E_MI_PNL_INTF_TTL;

    /************************************************
    Step1:  panel init
    *************************************************/
    STCHECKRESULT(MI_PANEL_Init(eIntfType));

    /************************************************
    Step2:  set disp pub 
    *************************************************/
    MI_DISP_PubAttr_t stPubAttr;
    memset(&stPubAttr, 0x0, sizeof(MI_DISP_PubAttr_t));
    
    stPubAttr.u32BgColor = YUYV_BLACK;
    stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    stPubAttr.eIntfSync =  E_MI_DISP_OUTPUT_USER;
    STCHECKRESULT(MI_DISP_SetPubAttr(DispDev,  &stPubAttr));
    STCHECKRESULT(MI_DISP_Enable(DispDev));
    
    /************************************************
    Step3:  set layer
    *************************************************/
    MI_DISP_LAYER            DispLayer = 0;
    MI_SYS_PixelFormat_e     ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;  
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    

    memset(&stLayerAttr, 0x0, sizeof(MI_DISP_VideoLayerAttr_t));

    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width = u32DispWidth;
    stLayerAttr.stVidLayerDispWin.u16Height = u32DispHeight;

    stLayerAttr.stVidLayerSize.u16Width = u32DispWidth;
    stLayerAttr.stVidLayerSize.u16Height = u32DispHeight;

    stLayerAttr.ePixFormat = ePixFormat;

    STCHECKRESULT(MI_DISP_BindVideoLayer(DispLayer,DispDev));
    STCHECKRESULT(MI_DISP_SetVideoLayerAttr(DispLayer, &stLayerAttr));
    STCHECKRESULT(MI_DISP_EnableVideoLayer(DispLayer));
    
    /************************************************
    Step4:  set inputport
    *************************************************/
    MI_U8 u8DispInport = 0;
    
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;    
    
    //MI_PANEL_ParamConfig_t stPanelParam;
    
    //memset(&stPanelParam, 0x0, sizeof(MI_PANEL_ParamConfig_t));
    memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
    memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
    
    //MI_PANEL_GetPanelParam(&stPanelParam);
    
    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width =u32DispWidth;
    stInputPortAttr.stDispWin.u16Height = u32DispHeight;
    
    stInputPortAttr.u16SrcWidth =u32DispWidth;
    stInputPortAttr.u16SrcHeight = u32DispHeight;
    
    stWinRect.u16Width = u32DispWidth;
    stWinRect.u16Height = u32DispHeight;
        

    printf("%s:%d layer:%d port:%d srcwidth:%d srcheight:%d x:%d y:%d outwidth:%d outheight:%d\n",__FUNCTION__,__LINE__,
        DispLayer,u8DispInport,
        stInputPortAttr.u16SrcWidth,stInputPortAttr.u16SrcHeight,
        stInputPortAttr.stDispWin.u16X,stInputPortAttr.stDispWin.u16Y,
        stInputPortAttr.stDispWin.u16Width,stInputPortAttr.stDispWin.u16Height);
    
    STCHECKRESULT(MI_DISP_SetInputPortAttr(DispLayer, u8DispInport, &stInputPortAttr));
    STCHECKRESULT(MI_DISP_EnableInputPort(DispLayer, u8DispInport));
    STCHECKRESULT(MI_DISP_SetInputPortSyncMode(DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));


    /************************************************
    Step5:  bind vpe->disp
    *************************************************/
    ST_Sys_BindInfo_T stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId =  0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;

    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    
   
    return MI_SUCCESS;
}

MI_U32 ST_PanelStop(void)
{
    MI_U32 DispLayer = 0;
    MI_U32 DispInport = 0;
    MI_U32 DispDev = 0;

    ST_Sys_BindInfo_T stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    
    STCHECKRESULT(MI_DISP_DisableInputPort(DispLayer, DispInport));
    
    STCHECKRESULT(MI_DISP_DisableVideoLayer(DispLayer));
    STCHECKRESULT(MI_DISP_UnBindVideoLayer(DispLayer, DispDev));
    
    STCHECKRESULT(MI_DISP_Disable(DispLayer));
    STCHECKRESULT(MI_PANEL_DeInit());

    return 0;
}

MI_U32 ST_BaseModuleInit(void)
{
    ST_Panel_Attr_T *pstPanelAttr = g_stPanelAttr;
 
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    MI_U32 u32ChocieRes =0;
    
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_VPE_3DNR_Level_e en3dNrLevel = E_MI_VPE_3DNR_LEVEL_OFF;
    MI_SYS_Rotate_e enRotation = E_MI_SYS_ROTATE_NONE;
    
    MI_SNR_PAD_ID_e eSnrPad = pstPanelAttr->eSnrPad;
    MI_VIF_DEV s32vifDev = pstPanelAttr->s32vifDev;
    MI_VIF_CHN s32vifChn = s32vifDev*4;

    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    /************************************************
    Step1:  init SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());

    if(eVifHdrType > 0)
        MI_SNR_SetPlaneMode(eSnrPad, TRUE);
    else
        MI_SNR_SetPlaneMode(eSnrPad, FALSE);

    MI_SNR_QueryResCount(eSnrPad, &u32ResCount);
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        MI_SNR_GetRes(eSnrPad, u8ResIndex, &stRes);
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }
#if 0
    printf("choice which resolution use, cnt %d\n", u32ResCount);
    do
    {
        scanf("%d", &u32ChocieRes);
        ST_Flush();
        MI_SNR_QueryResCount(eSnrPad, &u32ResCount);
        if(u32ChocieRes >= u32ResCount)
        {
            printf("choice err res %d > =cnt %d\n", u32ChocieRes, u32ResCount);
        }
    }while(u32ChocieRes >= u32ResCount);
#endif
    u32ChocieRes = 0;
    printf("You select %d res\n", u32ChocieRes);

    MI_SNR_SetRes(eSnrPad,u32ChocieRes);
    MI_SNR_Enable(eSnrPad);

    MI_SNR_GetPadInfo(eSnrPad, &stPad0Info);
    MI_SNR_GetPlaneInfo(eSnrPad, 0, &stSnrPlane0Info);
	MI_SNR_SetFps(eSnrPad,SENSOR_RGB_FPS);

    //g_u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    //g_u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
    
    /************************************************
    Step2:  init VIF(for IPC, only one dev)
    *************************************************/
    MI_VIF_DevAttr_t stDevAttr;
    memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

    stDevAttr.eIntfMode = stPad0Info.eIntfMode;
    stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
    stDevAttr.eHDRType = eVifHdrType;
    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        stDevAttr.eClkEdge = stPad0Info.unIntfAttr.stBt656Attr.eClkEdge;
    else
        stDevAttr.eClkEdge = E_MI_VIF_CLK_EDGE_DOUBLE;
    
    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
        stDevAttr.eDataSeq =stPad0Info.unIntfAttr.stMipiAttr.eDataYUVOrder;
    else
        stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;

    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        memcpy(&stDevAttr.stSyncAttr, &stPad0Info.unIntfAttr.stBt656Attr.stSyncAttr, sizeof(MI_VIF_SyncAttr_t));

    stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL;

    ExecFunc(MI_VIF_SetDevAttr(s32vifDev, &stDevAttr), MI_SUCCESS);
    ExecFunc(MI_VIF_EnableDev(s32vifDev), MI_SUCCESS);
    //STCHECKRESULT(ST_Vif_EnableDev(s32vifDev, eVifHdrType, &stPad0Info));

    ST_VIF_PortInfo_T stVifPortInfoInfo;
    memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
    stVifPortInfoInfo.u32RectX = 0;
    stVifPortInfoInfo.u32RectY = 0;
    stVifPortInfoInfo.u32RectWidth = u32CapWidth;
    stVifPortInfoInfo.u32RectHeight = u32CapHeight;
    stVifPortInfoInfo.u32DestWidth = u32CapWidth;
    stVifPortInfoInfo.u32DestHeight = u32CapHeight;
    stVifPortInfoInfo.eFrameRate = eFrameRate;
    stVifPortInfoInfo.ePixFormat = ePixFormat;
    STCHECKRESULT(ST_Vif_CreatePort(s32vifChn, 0, &stVifPortInfoInfo));
    STCHECKRESULT(ST_Vif_StartPort(0, s32vifChn, 0));
    
    if (enRotation != E_MI_SYS_ROTATE_NONE)
    {
        MI_BOOL bMirror = FALSE, bFlip = FALSE;

        switch(enRotation)
        {
        case E_MI_SYS_ROTATE_NONE:
            bMirror= FALSE;
            bFlip = FALSE;
            break;
        case E_MI_SYS_ROTATE_90:
            bMirror = FALSE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_180:
            bMirror = TRUE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_270:
            bMirror = TRUE;
            bFlip = FALSE;
            break;
        default:
            break;
        }

        MI_SNR_SetOrien(eSnrPad, bMirror, bFlip);
    }
    memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
	if(eSnrPad == E_MI_SNR_PAD_ID_0)
		stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR0;
	else if(eSnrPad == E_MI_SNR_PAD_ID_1)
		stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR1;
	else if(eSnrPad == E_MI_SNR_PAD_ID_2)
		stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR2;
	else if(eSnrPad == E_MI_SNR_PAD_ID_3)
		stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR3;
	else
		stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR_INVALID;
    stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
    stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
    stVpeChannelInfo.u32X = 0;
    stVpeChannelInfo.u32Y = 0;
    stVpeChannelInfo.u16VpeCropW = 0;
    stVpeChannelInfo.u16VpeCropH = 0;
    stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;//E_MI_VPE_RUN_REALTIME_MODE;
    stVpeChannelInfo.eFormat = ePixFormat;
    stVpeChannelInfo.e3DNRLevel = en3dNrLevel;
    stVpeChannelInfo.eHDRtype = eVpeHdrType;
    stVpeChannelInfo.bRotation = FALSE;
    STCHECKRESULT(ST_Vpe_CreateChannel(0, &stVpeChannelInfo));
    STCHECKRESULT(ST_Vpe_StartChannel(0));
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = s32vifDev;
    stBindInfo.stSrcChnPort.u32ChnId = s32vifChn;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;//E_MI_SYS_BIND_TYPE_REALTIME;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    ST_VPE_PortInfo_T stVpePortInfo;
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = 0;
    stVpePortInfo.u16OutputWidth = pstPanelAttr->u32Width;
    stVpePortInfo.u16OutputHeight = pstPanelAttr->u32Height;
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    STCHECKRESULT(ST_Vpe_StartPort(0 , &stVpePortInfo));
    
    return MI_SUCCESS;
}


MI_U32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;

 #if USE_SNR_PAD_ID_1
    MI_U32 u32VifDev = 0;
    MI_U32 u32VifChn = 0;
 #else
    MI_U32 u32VifDev = 1;
    MI_U32 u32VifChn = 4;
#endif
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = u32VifDev;
    stBindInfo.stSrcChnPort.u32ChnId = u32VifChn;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    /************************************************
    Step1:  destory VPE
    *************************************************/
    STCHECKRESULT(ST_Vpe_StopChannel(0));
    STCHECKRESULT(ST_Vpe_DestroyChannel(0));

    /************************************************
    Step2:  destory VIF
    *************************************************/
    STCHECKRESULT(ST_Vif_StopPort(u32VifChn, 0));
    STCHECKRESULT(ST_Vif_DisableDev(u32VifDev));

    /************************************************
    Step3:  destory SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Exit());

    return MI_SUCCESS;
}

int main(int argc, char **argv)
{

    STCHECKRESULT(ST_BaseModuleInit());
    STCHECKRESULT(ST_PanelStart());
    
    for(;;)
    {
        char cmd = 0xff;
        printf("input 'q' exit\n");
        scanf("%c", &cmd);
        ST_Flush();
        if('q' == cmd)
        {
            break;
        }
    }
    
    STCHECKRESULT(ST_PanelStop());
    STCHECKRESULT(ST_BaseModuleUnInit());

    return MI_SUCCESS;
}
