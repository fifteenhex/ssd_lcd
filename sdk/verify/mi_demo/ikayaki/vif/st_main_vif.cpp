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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "mi_sensor.h"
#include "mi_sensor_datatype.h"

//#define USE_SNR_PAD_ID_1  (1)

#define SENSOR_RGB_FPS 10


static MI_S8*   ps8VifRawOutputPath = NULL;
static MI_S8    gs8SnrPad = -1;
static MI_U32   gu32RawCount = 0;

void ST_Flush(void)
{
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

void display_help(void)
{
    printf("----- Get Vif Raw test -----\n");

    printf("Vif test option :\n");
    printf("-a : Sensor Pad\n");
    printf("-n : Output Counts\n");
    printf("-o : Vif Raw output Path\n");

    return;
}

MI_U32 ST_BaseModuleInit(void)
{
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

    MI_SNR_PAD_ID_e eSnrPad = (MI_SNR_PAD_ID_e)gs8SnrPad;
    MI_VIF_DEV s32vifDev = (MI_VIF_DEV)gs8SnrPad;
    MI_VIF_CHN s32vifChn = (MI_VIF_CHN)(gs8SnrPad * 4);

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
    //STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    ST_VPE_PortInfo_T stVpePortInfo;
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = 0;
    stVpePortInfo.u16OutputWidth = 1280;
    stVpePortInfo.u16OutputHeight = 720;
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    STCHECKRESULT(ST_Vpe_StartPort(0 , &stVpePortInfo)); //bind to disp for panenl & divp for barcode scanning

    return MI_SUCCESS;
}

MI_U32 ST_BaseModuleUnInit(void)
{
    MI_U32 u32VifDev = 0;
    MI_U32 u32VifChn = 0;

    u32VifDev = gs8SnrPad;
    u32VifChn = gs8SnrPad * 4;
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

static MI_S32 createOutputFileName(MI_U32 u32RawIdx, MI_S8* ps8OutputPath, MI_S8* ps8OutputFileName)
{
    char as8Tmp[512] = {0};

    memset(as8Tmp, 0, sizeof(as8Tmp));
    if ('/' == ps8OutputPath[strlen((char *)ps8OutputPath) - 1])
    {
        sprintf(as8Tmp, "%s", ps8OutputPath);
    }
    else
    {
        sprintf(as8Tmp, "%s/", ps8OutputPath);
    }
    strcat((char *)ps8OutputFileName, as8Tmp);

    //set Chn num
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "Snr%d_VifRaw%d", gs8SnrPad, u32RawIdx);
    strcat((char *)ps8OutputFileName, as8Tmp);

    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, ".raw");
    strcat((char *)ps8OutputFileName, as8Tmp);

    return 0;
}


static MI_S32 ST_Get_Vif_Raw()
{
    MI_S32   s32Ret = -1;
    MI_S8    s8OutputFileName[512];
    MI_S32   rawFd = -1;
    MI_S32 s32Fd = 0;
    fd_set read_fds;
    struct timeval TimeoutVal;

    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE stBufHandle;

    stChnPort.eModId = E_MI_MODULE_ID_VIF;
    stChnPort.u32DevId = gs8SnrPad;
    stChnPort.u32ChnId = gs8SnrPad * 4;
    stChnPort.u32PortId = 0;


    for (MI_U32 u32RawIdx = 0; u32RawIdx < gu32RawCount; u32RawIdx++)
    {
        memset(s8OutputFileName, 0x0, sizeof(s8OutputFileName));
        createOutputFileName(u32RawIdx, ps8VifRawOutputPath, s8OutputFileName);
        printf("OutputName:%s\n", s8OutputFileName);

        rawFd = open((const char *)s8OutputFileName, O_RDWR | O_CREAT, 0777);
        if (rawFd < 0)
        {
            printf("Open output file path:%s fail \n", s8OutputFileName);
            printf("error:%s", strerror(errno));
            return -1;
        }

        s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);
        if(MI_SUCCESS != s32Ret)
        {
            ST_ERR("MI_SYS_GetFd 0, error, %X\n", s32Ret);
            return -1;
        }

        s32Ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 3, 6);
        if (MI_SUCCESS != s32Ret)
        {
            ST_ERR("MI_SYS_SetChnOutputPortDepth err:%x, chn:%d,port:%d\n", s32Ret, stChnPort.u32ChnId, stChnPort.u32PortId);
            return -1;
        }

        while (1)
        {
            FD_ZERO(&read_fds);
            FD_SET(s32Fd, &read_fds);
            TimeoutVal.tv_sec = 1;
            TimeoutVal.tv_usec = 0;
            s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
            if(s32Ret < 0)
            {
                ST_ERR("select failed!\n");
                // usleep(10 * 1000);
                continue;
            }
            else if(s32Ret == 0)
            {
                ST_ERR("get vif raw data time out\n");
                //usleep(10 * 1000);
                continue;
            }
            else
            {
                if(FD_ISSET(s32Fd, &read_fds))
                {
                    s32Ret = MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &stBufHandle);
                    if(MI_SUCCESS == s32Ret)
                    {
                        //ST_ERR("MI_SYS_ChnOutputPortGetBuf successfully, %x\n", s32Ret);
                        break;
                    }
                }
            }
        }

        // save Vif Raw data
        if (rawFd > 0)
        {
            s32Ret = write(rawFd, stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.u32BufSize);
            if (s32Ret < 0)
            {
                printf("write RAW data failed, u32RawIdx:%d, size:%d\n", u32RawIdx, stBufInfo.stRawData.u32ContentSize);
                return -1;
            }
            printf("Framedata Format:%d, Size:%d, Width:%d Height:%d\n", stBufInfo.stFrameData.ePixelFormat, stBufInfo.stFrameData.u32BufSize,stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height);
        }
        close(rawFd);
        rawFd = -1;

        printf("Sensor Pad[%d] u32GetFramesCount[%d]\n", gs8SnrPad, u32RawIdx);
        MI_SYS_ChnOutputPortPutBuf(stBufHandle);

    }

    return MI_SUCCESS;

}

int main(int argc, char **argv)
{
    MI_S32      s32Opt = 0;
    MI_S8 arrayVifRawOutputPath[512];

    memset(arrayVifRawOutputPath, 0, 512);


    // parsing command line
    while ((s32Opt = getopt(argc, argv, "a:n:o:")) != -1)
    {
        switch(s32Opt)
        {
            // set vif pad , from 0 to 1
            case 'a':
                {
                    gs8SnrPad = (MI_S8)atoi(optarg);
                }
                break;

            // set output picture counts
            case 'n':
                {
                    gu32RawCount = (MI_U32)atoi(optarg);
                }
                break;

            // set vif raw output path
            case 'o':
                {
                    ps8VifRawOutputPath = (MI_S8*)optarg;
                }
                break;

            default:
                display_help();
                return -1;
        }
    }


    if(NULL == ps8VifRawOutputPath)
    {
        ps8VifRawOutputPath = arrayVifRawOutputPath;
        sprintf((char *)ps8VifRawOutputPath, "/tmp");
    }

    if(-1 == gs8SnrPad)
    {
        gs8SnrPad = 0;
    }

    if(0 == gu32RawCount)
    {
        gu32RawCount = 3;
    }

    printf("====> The app will output Snr%d %d Raw into dir:%s\n", gs8SnrPad, gu32RawCount, ps8VifRawOutputPath);
    STCHECKRESULT(ST_Sys_Init());

    STCHECKRESULT(ST_BaseModuleInit());
    ST_Get_Vif_Raw();

    printf("====> Out put Raw finished!\n\n");

    for(;;)
    {
        char cmd = 0xff;
        printf("input 'q' exit\n");
        scanf("%c", &cmd);
        ST_Flush();
        if('q' == cmd || 'Q' == cmd)
        {
            break;
        }
        usleep(1000);
    }


    STCHECKRESULT(ST_BaseModuleUnInit());

    return MI_SUCCESS;
}
