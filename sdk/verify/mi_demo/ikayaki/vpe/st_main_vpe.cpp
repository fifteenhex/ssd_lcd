/*
* XXX.c - Sigmastar
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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string>

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include "dictionary.h"
#include "iniparser.h"
#include "st_cus3a.h"

#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "Live555RTSPServer.hh"

//#include "mi_rgn.h"
#include "mi_sensor.h"
#include "mi_sensor_datatype.h"
#include "mi_isp.h"
#include "mi_iqserver.h"
#include "mi_eptz.h"
#include "mi_ldc.h"
#include "mi_divp.h"
#include "st_cus3a.h"
#if ((defined CONFIG_SIGMASTAR_CHIP_P3) && CONFIG_SIGMASTAR_CHIP_P3 == 1 )
#define SUPPORT_LDC 0
#else
#define SUPPORT_LDC 1
#endif


using namespace std;

#define RTSP_LISTEN_PORT        554
#define YUV422_PIXEL_PER_BYTE  2
static Live555RTSPServer *g_pRTSPServer = NULL;

#define PATH_PREFIX                "/mnt"

int s32LoadIQBin = 1;
#define NONHDR_PATH                "/customer/nohdr.bin"
#define HDR_PATH                "/customer/hdr.bin"

#define ST_MAX_PORT_NUM (8)
#define ST_MAX_SCL_NUM (5) //0,1,2,5,6 scl, 3 isp dma, 4 ir, 7 vir port

#define ST_MAX_SENSOR_NUM (3)

#define ST_MAX_VIF_DEV_NUM (3)
#define ST_MAX_VIF_CHN_PERDEV (4)
#define ST_MAX_VIF_CHN_NUM (ST_MAX_VIF_DEV_NUM * ST_MAX_VIF_CHN_PERDEV)
#define ST_MAX_VIF_OUTPORT_NUM (2)

#define ST_MAX_VPE_INPORT_NUM (3)
#define ST_MAX_VPECHN_NUM (16)

#define ST_MAX_VENC_NUM   (16)
#define ST_LDC_MAX_VIEWNUM (4)

#define ASCII_COLOR_GREEN                        "\033[1;32m"
#define ASCII_COLOR_END                          "\033[0m"
#define ASCII_COLOR_RED                          "\033[1;31m"

#define DBG_INFO(fmt, args...) printf(ASCII_COLOR_GREEN"%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);
#define DBG_ERR(fmt, args...) printf(ASCII_COLOR_RED"%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);

#if (defined CONFIG_SIGMASTAR_CHIP_I6E) && (CONFIG_SIGMASTAR_CHIP_I6E == 1)
    MI_U16 u16MaxW = 3840;
    MI_U16 u16MaxH = 2160;
    #define ST_VPE_VIR_PORTID (0xFF)
#elif ((defined CONFIG_SIGMASTAR_CHIP_M6) && (CONFIG_SIGMASTAR_CHIP_M6 == 1))
    MI_U16 u16MaxW = 3840;
    MI_U16 u16MaxH = 2160;
	#define ST_VPE_VIR_PORTID (7)
#elif ((defined CONFIG_SIGMASTAR_CHIP_P3) && CONFIG_SIGMASTAR_CHIP_P3 == 1)
    MI_U16 u16MaxW = 1920;
    MI_U16 u16MaxH = 1080;
    #define ST_VPE_VIR_PORTID (0xFF)
#else
    MI_U16 u16MaxW = 2688;
    MI_U16 u16MaxH = 1944;
    #define ST_VPE_VIR_PORTID (3)
#endif

typedef struct ST_BindParam_s
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BindType_e       eBindType;
    MI_U32 u32BindParam;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
}ST_BindParam_t;

typedef struct ST_InputFile_Attr_s
{
    MI_U32 u32Width;
    MI_U32 u32Height;
    MI_SYS_PixelFormat_e ePixelFormat;
    char InputFilePath[256];
    pthread_mutex_t mutex;
    pthread_t pPutDatathread;
    MI_U8 u8VpeInPortId;
}ST_InputFile_Attr_t;
typedef struct ST_Sensor_Attr_s
{
    MI_U32 u32BindVifDev;
    MI_BOOL bUsed;
    MI_BOOL bCreate;
    MI_BOOL bPlaneMode;
    MI_U8 u8ResIndex;
    MI_BOOL u8Mirror;
    MI_BOOL u8Flip;
}ST_Sensor_Attr_t;

typedef struct ST_VifPortAttr_s
{
    MI_BOOL    bCreate;
    MI_BOOL    bUsed;
    MI_SYS_WindowRect_t      stCapRect;
    MI_SYS_WindowSize_t      stDestSize;
    MI_SYS_PixelFormat_e     ePixFormat;
    MI_U32 u32FrameModeLineCount;
    MI_U32 u32DestFrameRate; //for bakeend set bind src frame rate, use max value
    MI_SYS_BindType_e       eBindType;

    MI_U16 u16Depth;
    MI_U16 u16UserDepth;
}ST_VifPortAttr_t;

typedef struct ST_VifChnAttr_s
{
    MI_BOOL    bCreate;
    MI_BOOL    bUsed;
    ST_VifPortAttr_t  stVifOutPortAttr[ST_MAX_VIF_OUTPORT_NUM];
}ST_VifChnAttr_t;

typedef struct ST_VifDevAttr_s
{
    MI_BOOL    bCreate;
    MI_BOOL    bUsed;
    MI_VIF_Dev2SnrPadMuxCfg_t stBindSensor;
    MI_BOOL    bNeedSetVifDev2SnrPad;

    MI_VIF_WorkMode_e       eWorkMode;
    MI_VIF_HDRType_e        eHDRType;
    MI_U32                  u32DevStitchMask;
    ST_VifChnAttr_t stVifChnAttr[ST_MAX_VIF_CHN_PERDEV];
}ST_VifDevAttr_t;

typedef struct ST_VifModeAttr_s
{
    ST_VifDevAttr_t stVifDevAttr[ST_MAX_VIF_DEV_NUM];
}ST_VifModAttr_t;

typedef struct ST_PortAttr_s
{
    MI_BOOL bUsed;
    MI_U32  u32BindVencChan;
    MI_BOOL bMirror;
    MI_BOOL bFlip;
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_SYS_WindowSize_t  stOrigPortSize;
    MI_SYS_WindowRect_t  stOrigPortCrop;

    MI_SYS_WindowRect_t  stPortCrop;
    MI_SYS_WindowSize_t  stPortSize;
    MI_S32 s32DumpBuffNum;
    char FilePath[256];
    pthread_mutex_t Portmutex;
    pthread_t pGetDatathread;

    MI_U16 u16Depth;
    MI_U16 u16UserDepth;
}ST_PortAttr_t;

typedef struct ST_VpeInPortAttr_s
{
    MI_BOOL bUsed;
    ST_BindParam_t stBindParam;
}ST_VpeInPortAttr_t;

typedef struct ST_VpeChannelAttr_s
{
    MI_U8 u8ChnId;
    MI_BOOL bUsed;
    MI_BOOL bCreate;
    ST_VpeInPortAttr_t stInputPortAttr[ST_MAX_VPE_INPORT_NUM];

    ST_PortAttr_t        stVpePortAttr[ST_MAX_PORT_NUM];
    MI_VPE_HDRType_e        eHdrType;
    MI_VPE_3DNR_Level_e     e3DNRLevel;
    MI_SYS_Rotate_e         eVpeRotate;
    MI_BOOL                 bChnMirror;
    MI_BOOL                 bChnFlip;
    MI_SYS_WindowRect_t     stOrgVpeChnCrop;
    MI_BOOL                 bEnLdc;
    MI_U32                  u32ChnPortMode;
    MI_VPE_RunningMode_e    eRunningMode;
    ST_InputFile_Attr_t         stInputFileAttr ;
    MI_SYS_WindowRect_t     stVpeChnCrop;

    char IqCfgbin_Path[128];
    char LdcCfgbin_Path[128];

#if SUPPORT_LDC
    mi_eptz_config_param tconfig_para;
#endif
    MI_U32 u32ViewNum;
    LDC_BIN_HANDLE          ldcBinBuffer[ST_LDC_MAX_VIEWNUM];
    MI_U32                  u32LdcBinSize[ST_LDC_MAX_VIEWNUM];
    MI_S32  s32Rot[ST_LDC_MAX_VIEWNUM];
}ST_VpeChannelAttr_t;

typedef struct ST_VencAttr_s
{
    MI_U32     u32BindVpeChn;
    MI_U32     u32BindVpePort;
    MI_SYS_BindType_e  eBindType;
    MI_U32  u32BindParam;

    MI_VENC_CHN vencChn;
    MI_VENC_ModType_e eType;
    MI_U32    u32Width;
    MI_U32     u32Height;
    char szStreamName[128];
    MI_BOOL bUsed;
    MI_BOOL bStart;
}ST_VencAttr_t;

typedef struct ST_DivpAttr_s
{
    ST_PortAttr_t  stDivpPort;
}ST_DivpAttr_t;

static MI_S32 gbPreviewByVenc = FALSE;
static MI_S32 gbMutiVpeChnNum = -1;
static ST_Sensor_Attr_t  gstSensorAttr[ST_MAX_SENSOR_NUM];
static ST_VifModAttr_t   gstVifModule;
static ST_VpeChannelAttr_t gstVpeChnattr[ST_MAX_VPECHN_NUM];
static ST_VencAttr_t gstVencattr[ST_MAX_VENC_NUM];
static MI_BOOL bExit = FALSE;
static ST_DivpAttr_t gstDivpAttr;
static MI_U32 u32ChnNum=0;
static MI_U32 u32VencChnNum = 0;

void ST_Flush(void)
{
    char c;

    while((c = getchar()) != '\n' && c != EOF);
}

void *ST_OpenStream(char const *szStreamName, void *arg)
{
    MI_U32 i = 0;
    MI_S32 s32Ret = MI_SUCCESS;

    for(i = 0; i < ST_MAX_VENC_NUM; i ++)
    {
        if(!strncmp(szStreamName, gstVencattr[i].szStreamName,
                    strlen(szStreamName)))
        {
            break;
        }
    }

    if(i >= ST_MAX_VENC_NUM)
    {
        ST_ERR("not found this stream, \"%s\"", szStreamName);
        return NULL;
    }

    ST_VencAttr_t *pstVencAttr = &gstVencattr[i];

    if(pstVencAttr[i].eType != E_MI_VENC_MODTYPE_JPEGE)
    {
        s32Ret = MI_VENC_RequestIdr(pstVencAttr->vencChn, TRUE);

        ST_DBG("open stream \"%s\" success, chn:%d\n", szStreamName, pstVencAttr->vencChn);

        if(MI_SUCCESS != s32Ret)
        {
            ST_WARN("request IDR fail, error:%x\n", s32Ret);
        }
    }
    return pstVencAttr;
}

MI_U32 u32GetCnt=0, u32ReleaseCnt=0;
int ST_VideoReadStream(void *handle, unsigned char *ucpBuf, int BufLen, struct timeval *p_Timestamp, void *arg)
{
    MI_SYS_BufInfo_t stBufInfo;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 len = 0;
    MI_U32 u32DevId = 0;
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack;
    MI_VENC_ChnStat_t stStat;
    MI_VENC_CHN vencChn ;

    if(handle == NULL)
    {
        return -1;
    }

    ST_VencAttr_t *pstStreamInfo = (ST_VencAttr_t *)handle;

    vencChn = pstStreamInfo->vencChn;

    if(pstStreamInfo->bStart == FALSE)
        return 0;

    s32Ret = MI_VENC_GetChnDevid(vencChn, &u32DevId);

    if(MI_SUCCESS != s32Ret)
    {
        ST_INFO("MI_VENC_GetChnDevid %d error, %X\n", vencChn, s32Ret);
    }

    memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
    memset(&stStream, 0, sizeof(stStream));
    memset(&stPack, 0, sizeof(stPack));
    stStream.pstPack = &stPack;
    stStream.u32PackCount = 1;
    s32Ret = MI_VENC_Query(vencChn, &stStat);

    if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
    {
        return 0;
    }

    s32Ret = MI_VENC_GetStream(vencChn, &stStream, 40);

    if(MI_SUCCESS == s32Ret)
    {
        u32GetCnt++;
        len = stStream.pstPack[0].u32Len;
        memcpy(ucpBuf, stStream.pstPack[0].pu8Addr, MIN(len, BufLen));

        s32Ret = MI_VENC_ReleaseStream(vencChn, &stStream);
        if(s32Ret != MI_SUCCESS)
        {
            ST_WARN("RELEASE venc buffer fail\n");
        }
        u32ReleaseCnt ++;
        return len;
    }

    return 0;
}

int ST_CloseStream(void *handle, void *arg)
{
    if(handle == NULL)
    {
        return -1;
    }

    ST_VencAttr_t *pstStreamInfo = (ST_VencAttr_t *)handle;

    ST_DBG("close \"%s\" success\n", pstStreamInfo->szStreamName);
    return 0;
}

MI_S32 ST_RtspServerStart(void)
{
    unsigned int rtspServerPortNum = RTSP_LISTEN_PORT;
    int iRet = 0;
    char *urlPrefix = NULL;
    int arraySize = ARRAY_SIZE(gstVencattr);
    ST_VencAttr_t *pstStreamAttr = gstVencattr;
    int i = 0;
    ServerMediaSession *mediaSession = NULL;
    ServerMediaSubsession *subSession = NULL;
    Live555RTSPServer *pRTSPServer = NULL;

    pRTSPServer = new Live555RTSPServer();

    if(pRTSPServer == NULL)
    {
        ST_ERR("malloc error\n");
        return -1;
    }

    iRet = pRTSPServer->SetRTSPServerPort(rtspServerPortNum);

    while(iRet < 0)
    {
        rtspServerPortNum++;

        if(rtspServerPortNum > 65535)
        {
            ST_INFO("Failed to create RTSP server: %s\n", pRTSPServer->getResultMsg());
            delete pRTSPServer;
            pRTSPServer = NULL;
            return -2;
        }

        iRet = pRTSPServer->SetRTSPServerPort(rtspServerPortNum);
    }

    urlPrefix = pRTSPServer->rtspURLPrefix();

    for(i = 0; i < arraySize; i ++)
    {
        if(pstStreamAttr[i].bUsed != TRUE)
            continue;

        printf("=================URL===================\n");
        printf("%s%s\n", urlPrefix, pstStreamAttr[i].szStreamName);
        printf("=================URL===================\n");

        pRTSPServer->createServerMediaSession(mediaSession,
                                              pstStreamAttr[i].szStreamName,
                                              NULL, NULL);

        if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H264E)
        {
            subSession = WW_H264VideoFileServerMediaSubsession::createNew(
                             *(pRTSPServer->GetUsageEnvironmentObj()),
                             pstStreamAttr[i].szStreamName,
                             ST_OpenStream,
                             ST_VideoReadStream,
                             ST_CloseStream, 30);
        }
        else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H265E)
        {
            subSession = WW_H265VideoFileServerMediaSubsession::createNew(
                             *(pRTSPServer->GetUsageEnvironmentObj()),
                             pstStreamAttr[i].szStreamName,
                             ST_OpenStream,
                             ST_VideoReadStream,
                             ST_CloseStream, 30);
        }
        else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_JPEGE)
        {
            subSession = WW_JPEGVideoFileServerMediaSubsession::createNew(
                             *(pRTSPServer->GetUsageEnvironmentObj()),
                             pstStreamAttr[i].szStreamName,
                             ST_OpenStream,
                             ST_VideoReadStream,
                             ST_CloseStream, 30);
        }

        pRTSPServer->addSubsession(mediaSession, subSession);
        pRTSPServer->addServerMediaSession(mediaSession);
    }

    pRTSPServer->Start();

    g_pRTSPServer = pRTSPServer;

    return 0;
}

MI_S32 ST_RtspServerStop(void)
{
    if(g_pRTSPServer)
    {
        g_pRTSPServer->Join();
        delete g_pRTSPServer;
        g_pRTSPServer = NULL;
    }

    return 0;
}

MI_S32 ST_WriteOneFrame(FILE *fp, int offset, char *pDataFrame, int line_offset, int line_size, int lineNumber)
{
    int size = 0;
    int i = 0;
    char *pData = NULL;
    int yuvSize = line_size;
    MI_S32 s32Ret = -1;

    for(i = 0; i < lineNumber; i++)
    {
        pData = pDataFrame + line_offset * i;
        yuvSize = line_size;

        do
        {
            if(yuvSize < 256)
            {
                size = yuvSize;
            }
            else
            {
                size = 256;
            }

            size = fwrite(pData, 1, size, fp);

            if(size == 0)
            {
                break;
            }
            else if(size < 0)
            {
                break;
            }

            pData += size;
            yuvSize -= size;
        }
        while(yuvSize > 0);
        s32Ret = MI_SUCCESS;
    }

    return s32Ret;
}

MI_S32 ST_GetVpeOutputData(MI_U32 u32ChnNum)
{
    MI_S32  s32Portid = 0;
    MI_S32  Vpechn = 0;
    MI_S32  s32DumpBuffNum =0;
    char sFilePath[128];
    time_t stTime = 0;

    ST_PortAttr_t *pstVpePortAttr = NULL;
    MI_VPE_PortMode_t stVpePortMode;
    memset(&stVpePortMode, 0x0, sizeof(MI_VPE_PortMode_t));
    memset(&stTime, 0, sizeof(stTime));

    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &Vpechn);
        ST_Flush();

    }
    else
    {
        for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", Vpechn);
                break;
            }
        }
    }
    if(Vpechn >= ST_MAX_VPECHN_NUM)
    {
        printf("chnid %d > max %d \n", Vpechn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    printf("select port id:");
    scanf("%d", &s32Portid);
    ST_Flush();

    printf("Dump Buffer Num:");
    scanf("%d", &s32DumpBuffNum);
    ST_Flush();

    printf("write file path:\n");
    scanf("%s", sFilePath);
    ST_Flush();

    if(s32Portid >= ST_MAX_PORT_NUM)
    {
        printf("port %d, not valid 0~3 \n", s32Portid);
        return 0;
    }

    pstVpePortAttr = &gstVpeChnattr[Vpechn].stVpePortAttr[s32Portid];

    if(pstVpePortAttr->bUsed != TRUE)
    {
        printf("port %d, not valid \n", s32Portid);
        return 0;
    }

    pthread_mutex_lock(&pstVpePortAttr->Portmutex);

    if(s32Portid != ST_VPE_VIR_PORTID)
    {
        STCHECKRESULT(MI_VPE_GetPortMode(Vpechn, s32Portid, &stVpePortMode));
        sprintf(pstVpePortAttr->FilePath, "%s/vpeport%d_%dx%d_pixel%d_%ld.raw", sFilePath, s32Portid, stVpePortMode.u16Width, stVpePortMode.u16Height, stVpePortMode.ePixelFormat, time(&stTime));
    }
    else
    {
        MI_DIVP_CHN DivpChn =0;
        MI_DIVP_OutputPortAttr_t stDivpOutPutAttr;
        memset(&stDivpOutPutAttr, 0x0, sizeof(MI_DIVP_OutputPortAttr_t));

        STCHECKRESULT(MI_DIVP_GetOutputPortAttr(DivpChn, &stDivpOutPutAttr));
        sprintf(pstVpePortAttr->FilePath, "%s/Divpport%d_%dx%d_pixel%d_%ld.raw", sFilePath, s32Portid, stDivpOutPutAttr.u32Width, stDivpOutPutAttr.u32Height, stDivpOutPutAttr.ePixelFormat, time(&stTime));
    }

    pstVpePortAttr->s32DumpBuffNum = s32DumpBuffNum;

    pthread_mutex_unlock(&pstVpePortAttr->Portmutex);

    return 0;

}

MI_S32 ST_VpeDisablePort(MI_U32 u32ChnNum)
{
    MI_S32  s32Portid = 0;
    ST_PortAttr_t *pstVpePortAttr = NULL;

    MI_S32  Vpechn = 0;
    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &Vpechn);
        ST_Flush();

    }
    else
    {
        for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", Vpechn);
                break;
            }
        }
    }
    if(Vpechn >= ST_MAX_VPECHN_NUM)
    {
        printf("chnid %d > max %d \n", Vpechn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    printf("select port id:");
    scanf("%d", &s32Portid);
    ST_Flush();

    if(s32Portid >= ST_MAX_PORT_NUM)
    {
        printf("port %d, not valid 0~%d \n", s32Portid, ST_MAX_PORT_NUM);
        return 0;
    }

    pstVpePortAttr = &gstVpeChnattr[Vpechn].stVpePortAttr[s32Portid];
    pstVpePortAttr->bUsed = FALSE;

    STCHECKRESULT(MI_VPE_DisablePort(Vpechn, s32Portid));

    return 0;
}

MI_S32 ST_GetVencOut()
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack;
    MI_U32 u32BypassCnt = 0;
    MI_S32 s32DumpBuffNum = 0;
    MI_S32  VencChn = 0;
    MI_VENC_Pack_t *pstPack = NULL;
    MI_U32  i=0;
    FILE *fp = NULL;
    char FilePath[256];
    char sFilePath[128];
    time_t stTime = 0;
    MI_VENC_ChnStat_t stStat;
    MI_VENC_ChnAttr_t stVencAttr;
    memset(&stVencAttr, 0x0, sizeof(MI_VENC_ChnAttr_t));

    memset(&stStat, 0x0, sizeof(MI_VENC_ChnStat_t));
    memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
    memset(&stPack, 0, sizeof(MI_VENC_Pack_t));
    stStream.pstPack = &stPack;
    stStream.u32PackCount = 1;

    printf("select venc chn id:");
    scanf("%d", &VencChn);
    ST_Flush();

    printf("Dump Buffer Num:");
    scanf("%d", &s32DumpBuffNum);
    ST_Flush();

    printf("write file path:\n");
    scanf("%s", sFilePath);
    ST_Flush();

    s32Ret = MI_VENC_GetChnAttr(VencChn, &stVencAttr);

    if(s32Ret != MI_SUCCESS)
    {
        printf("channel %d, not valid \n", VencChn);
        return 0;
    }

    if(stVencAttr.stVeAttr.eType == E_MI_VENC_MODTYPE_JPEGE)
        sprintf(FilePath, "%s/venc_%ld.jpg", sFilePath, time(&stTime));
    else
        sprintf(FilePath, "%s/venc_%ld.es", sFilePath, time(&stTime));

    fp = fopen(FilePath,"wb");

    if(fp == NULL)
    {
        printf("open file %s fail \n",FilePath);
        return 0;
    }

    while(s32DumpBuffNum > 0)
    {
        s32Ret = MI_VENC_Query(VencChn, &stStat);
        if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
        {
            usleep(1 * 1000);
            continue;
        }

        s32Ret = MI_VENC_GetStream(VencChn, &stStream, 100);
        if(MI_SUCCESS == s32Ret)
        {
            if (0 == u32BypassCnt)
            {
                printf("##########Start to write file %s, id %d !!!#####################\n",FilePath, s32DumpBuffNum);

                for(i = 0; i < stStream.u32PackCount; i ++)
                {
                    pstPack = &stStream.pstPack[i];
                    fwrite(pstPack->pu8Addr + pstPack->u32Offset, pstPack->u32Len - pstPack->u32Offset, 1, fp);
                }
                printf("##########End to write file id %d !!!#####################\n", s32DumpBuffNum);

                s32DumpBuffNum --;
            }
            else
            {
                u32BypassCnt--;
                printf("Bypasss frame %d !\n", u32BypassCnt);
            }
            s32Ret = MI_VENC_ReleaseStream(VencChn, &stStream);
            if(MI_SUCCESS != s32Ret)
            {
                ST_DBG("MI_VENC_ReleaseStream fail, ret:0x%x\n", s32Ret);
            }
        }

        usleep(200 * 1000);
    }
    fclose(fp);

    return 0;
}

MI_S32 ST_ReadLdcTableBin(const char *pConfigPath, LDC_BIN_HANDLE *tldc_bin, MI_U32 *pu32BinSize)
{
    struct stat statbuff;
    MI_U8 *pBufData = NULL;
    MI_S32 s32Fd = 0;
    MI_U32 u32Size = 0;

    if (pConfigPath == NULL)
    {
        ST_ERR("File path null!\n");
        return MI_ERR_LDC_ILLEGAL_PARAM;
    }
    printf("Read file %s\n", pConfigPath);
    memset(&statbuff, 0, sizeof(struct stat));
    if(stat(pConfigPath, &statbuff) < 0)
    {
        ST_ERR("Bb table file not exit!\n");
        return MI_ERR_LDC_ILLEGAL_PARAM;
    }
    else
    {
        if (statbuff.st_size == 0)
        {
            ST_ERR("File size is zero!\n");
            return MI_ERR_LDC_ILLEGAL_PARAM;
        }
        u32Size = statbuff.st_size;
    }
    s32Fd = open(pConfigPath, O_RDONLY);
    if (s32Fd < 0)
    {
        ST_ERR("Open file[%d] error!\n", s32Fd);
        return MI_ERR_LDC_ILLEGAL_PARAM;
    }
    pBufData = (MI_U8 *)malloc(u32Size);
    if (!pBufData)
    {
        ST_ERR("Malloc error!\n");
        close(s32Fd);

        return MI_ERR_LDC_ILLEGAL_PARAM;
    }

    memset(pBufData, 0, u32Size);
    read(s32Fd, pBufData, u32Size);
    close(s32Fd);

    *tldc_bin = pBufData;
    *pu32BinSize = u32Size;

    printf("%d: read buffer %p \n",__LINE__, pBufData);
    printf("%d: &bin address %p, *binbuffer %p \n",__LINE__, tldc_bin, *tldc_bin);

    //free(pBufData);

    return MI_SUCCESS;
}

MI_S32 ST_GetLdcCfgViewNum(mi_LDC_MODE eLdcMode)
{
    MI_U32 u32ViewNum = 0;

    switch(eLdcMode)
    {
        case LDC_MODE_1R:
        case LDC_MODE_1P_CM:
        case LDC_MODE_1P_WM:
        case LDC_MODE_1O:
        case LDC_MODE_1R_WM:
            u32ViewNum = 1;
            break;
        case LDC_MODE_2P_CM:
        case LDC_MODE_2P_DM:
            u32ViewNum = 2;
            break;
        case LDC_MODE_4R_CM:
        case LDC_MODE_4R_WM:
            u32ViewNum = 4;
            break;
        default:
            printf("########### ldc mode %d err \n", eLdcMode);
            break;
    }

    printf("view num %d \n", u32ViewNum);
    return u32ViewNum;
}

MI_S32 ST_Parse_LibarayCfgFilePath(char *pLdcLibCfgPath, mi_eptz_config_param *ptconfig_para)
{
    mi_eptz_err err_state = MI_EPTZ_ERR_NONE;

    printf("cfg file path %s\n", pLdcLibCfgPath);
    //check cfg file, in out path with bin position

    err_state = mi_eptz_config_parse(pLdcLibCfgPath, ptconfig_para);
    if (err_state != MI_EPTZ_ERR_NONE)
    {
        printf("confile file read error: %d\n", err_state);
        return err_state;
    }

    printf("ldc mode %d \n", ptconfig_para->ldc_mode);
    return 0;
}

MI_S32 ST_Libaray_CreatBin(MI_S32 s32ViewId, mi_eptz_config_param *ptconfig_para, LDC_BIN_HANDLE *ptldc_bin, MI_U32 *pu32LdcBinSize, MI_S32 s32Rot)
{
#if SUPPORT_LDC
    unsigned char* pWorkingBuffer;
    int working_buf_len = 0;
    mi_eptz_err err_state = MI_EPTZ_ERR_NONE;
    EPTZ_DEV_HANDLE eptz_handle = NULL;

    mi_eptz_para teptz_para;
    memset(&teptz_para, 0x0, sizeof(mi_eptz_para));

    printf("view %d rot %d\n", s32ViewId, s32Rot);

    working_buf_len = mi_eptz_get_buffer_info(ptconfig_para);
    pWorkingBuffer = (unsigned char*)malloc(working_buf_len);
    if (pWorkingBuffer == NULL)
    {
        printf("buffer allocate error\n");
        return MI_EPTZ_ERR_MEM_ALLOCATE_FAIL;
    }

   // printf("%s:%d working_buf_len %d \n", __FUNCTION__, __LINE__, working_buf_len);

    //EPTZ init
    teptz_para.ptconfig_para = ptconfig_para; //ldc configure

  //  printf("%s:%d ptconfig_para %p, pWorkingBuffer %p, working_buf_len %d\n", __FUNCTION__, __LINE__, teptz_para.ptconfig_para,
    //    pWorkingBuffer, working_buf_len);

    eptz_handle =  mi_eptz_runtime_init(pWorkingBuffer, working_buf_len, &teptz_para);
    if (eptz_handle == NULL)
    {
        printf("EPTZ init error\n");
        return MI_EPTZ_ERR_NOT_INIT;
    }

    teptz_para.pan = 0;
    teptz_para.tilt = -60;
    if(ptconfig_para->ldc_mode == 1)
        teptz_para.tilt = 60;
    teptz_para.rotate = s32Rot;
    teptz_para.zoom = 150.00;
    teptz_para.out_rot = 0;
    teptz_para.view_index = s32ViewId;

    //Gen bin files from 0 to 360 degree
    switch (ptconfig_para->ldc_mode)
    {
        case LDC_MODE_4R_CM:  //LDC_MODE_4R_CM/Desk, if in desk mount mode, tilt is nagetive.
            teptz_para.view_index = s32ViewId;
            teptz_para.pan = 0;
            teptz_para.tilt = -50; //In CM mode, tilt is positive, but in desk mode, tilt is negative.
            teptz_para.rotate = s32Rot;
            teptz_para.zoom = 150;
            teptz_para.out_rot = 0;
            err_state = (mi_eptz_err)mi_eptz_runtime_map_gen(eptz_handle,(mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;
        case LDC_MODE_4R_WM:  //LDC_MODE_4R_WM
            teptz_para.view_index = s32ViewId;
            teptz_para.pan = 0;
            teptz_para.tilt = 50; //In CM mode, tilt is positive, but in desk mode, tilt is negative.
            teptz_para.rotate = s32Rot;
            teptz_para.zoom = 150;
            teptz_para.out_rot = 0;
            err_state = (mi_eptz_err)mi_eptz_runtime_map_gen(eptz_handle,(mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;
        case LDC_MODE_1R:  //LDC_MODE_1R CM/Desk,  if in desk mount mode, tilt is negative.
            teptz_para.view_index = s32ViewId;
            teptz_para.pan = 0;
            teptz_para.tilt = 0; //In CM mode, tilt is positive, but in desk mode, tilt is negative.
            teptz_para.rotate = s32Rot;
            teptz_para.zoom = 150;
            teptz_para.out_rot = 0;
            err_state = (mi_eptz_err)mi_eptz_runtime_map_gen(eptz_handle,(mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;
        case LDC_MODE_1R_WM:  //LDC_MODE_1R WM
            teptz_para.view_index = s32ViewId;
            teptz_para.pan = 0;
            teptz_para.tilt = 50; //In CM mode, tilt is positive, but in desk mode, tilt is negative.
            teptz_para.rotate = s32Rot;
            teptz_para.zoom = 150;
            teptz_para.out_rot = 0;

            err_state = (mi_eptz_err)mi_eptz_runtime_map_gen(eptz_handle,(mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;

        case LDC_MODE_2P_CM:  //LDC_MODE_2P_CM
        case LDC_MODE_2P_DM:  //LDC_MODE_2P_DM
        case LDC_MODE_1P_CM:  //LDC_MODE_1P_CM
            //Set the input parameters for donut mode
            if(s32Rot > 180)
            {
                //Degree 180 ~ 360
                teptz_para.view_index = s32ViewId;
                teptz_para.r_inside = 550;
                teptz_para.r_outside = 10;
                teptz_para.theta_start = s32Rot;
                teptz_para.theta_end = s32Rot+360;
            }
            else
            {
                //Degree 180 ~ 0
                teptz_para.view_index = s32ViewId;
                teptz_para.r_inside = 10;
                teptz_para.r_outside = 550;
                teptz_para.theta_start = s32Rot;
                teptz_para.theta_end = s32Rot+360;
            }
            err_state = (mi_eptz_err)mi_donut_runtime_map_gen(eptz_handle, (mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;
        case LDC_MODE_1P_WM:  //LDC_MODE_1P wall mount.
            teptz_para.view_index = s32ViewId;
            teptz_para.pan = 0;
            teptz_para.tilt = 0;
            teptz_para.zoom_h = 100;
            teptz_para.zoom_v = 100;
            err_state = (mi_eptz_err)mi_erp_runtime_map_gen(eptz_handle,(mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[EPTZ ERR] =  %d !! \n", err_state);
            }
            break;
        case LDC_MODE_1O:    //bypass mode
            teptz_para.view_index = 0; //view index
            printf("begin mi_bypass_runtime_map_gen \n");
            err_state = (mi_eptz_err)mi_bypass_runtime_map_gen(eptz_handle, (mi_eptz_para*)&teptz_para, ptldc_bin, (int *)pu32LdcBinSize);
            if (err_state != MI_EPTZ_ERR_NONE)
            {
                printf("[MODE %d ERR] =  %d !! \n", LDC_MODE_1O, err_state);
                return err_state;
            }

            printf("end mi_bypass_runtime_map_gen\n");
            break;
        default :
             printf("********************err ldc mode %d \n", ptconfig_para->ldc_mode);
             return 0;
    }
#if 0
    FILE *fp = NULL;
    fp = fopen("/mnt/ldc.bin","wb");
    fwrite(ptldc_bin, *pu32LdcBinSize, 1, fp);
    fclose(fp);
#endif
    //Free bin buffer
    /*
    err_state = mi_eptz_buffer_free((LDC_BIN_HANDLE)tldc_bin);
    if (err_state != MI_EPTZ_ERR_NONE)
    {
        printf("[MI EPTZ ERR] =  %d !! \n", err_state);
    }*/
    //release working buffer
    free(pWorkingBuffer);
#endif

    return 0;
}
MI_S32 ST_GetLdcBinBuffer(ST_VpeChannelAttr_t *pstVpeChnattr)
{
#if SUPPORT_LDC
    MI_U8 i =0;
    MI_U32 u32ViewNum = 0;
    MI_S32 s32Cfg_Param = 0;
    mi_eptz_config_param *ptconfig_para = &pstVpeChnattr->tconfig_para;
    MI_S32 *ps32Rot = pstVpeChnattr->s32Rot;
    char *pCfgFilePath = pstVpeChnattr->LdcCfgbin_Path;
    MI_U32 *pu32ViewNum = &pstVpeChnattr->u32ViewNum;
    LDC_BIN_HANDLE *ptldc_bin = pstVpeChnattr->ldcBinBuffer;
    MI_U32 *pu32LdcBinSize = pstVpeChnattr->u32LdcBinSize;
    MI_U16 u16PathSize = strlen(pCfgFilePath);
    std::string path = pCfgFilePath;
    std::string sub;
    MI_U8 tmp = path.rfind(".");
    sub = path.substr(tmp, u16PathSize);
    const char *p = sub.data();

    if(!strcmp(p, ".bin"))
    {
        s32Cfg_Param = 1;
    }
    else if(!strcmp(p, ".cfg"))
    {
        s32Cfg_Param = 0;
    }
    else
    {
        printf("path name %s err, need use .bin or .cfg \n", pCfgFilePath);
    }

    if(s32Cfg_Param == 0)
    {
        ST_Parse_LibarayCfgFilePath(pCfgFilePath, ptconfig_para);
        u32ViewNum = ST_GetLdcCfgViewNum(ptconfig_para->ldc_mode);
        for(i=0; i<u32ViewNum; i++)
        {
            ST_Libaray_CreatBin(i, ptconfig_para, &ptldc_bin[i], &pu32LdcBinSize[i], ps32Rot[i]);
        }
        *pu32ViewNum = u32ViewNum;
    }
    else
    {
        ST_ReadLdcTableBin(pCfgFilePath, ptldc_bin, pu32LdcBinSize);
        *pu32ViewNum = 1;
    }
#endif
    return 0;
}


MI_S32 ST_SetLdcOnOff(MI_U32 u32ChnNum)
{
    MI_S32 s32LdcOnoff = 0;
    MI_VPE_ChannelPara_t stVpeChnParam;
    memset(&stVpeChnParam, 0x0, sizeof(MI_VPE_ChannelPara_t));

    MI_S32  Vpechn = 0;
    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &Vpechn);
        ST_Flush();

    }
    else
    {
        for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", Vpechn);
                break;
            }
        }
    }
    if(Vpechn >= ST_MAX_VPECHN_NUM)
    {
        printf("chnid %d > max %d \n", Vpechn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];

    printf("Set Ldc ON(1), OFF(0): \n");
    scanf("%d", &s32LdcOnoff);
    ST_Flush();

    if(s32LdcOnoff == TRUE && pstVpeChnattr->bEnLdc == FALSE)
    {
        printf("ldc onoff %d, before enldc %d need set bin path \n", s32LdcOnoff, pstVpeChnattr->bEnLdc);
        printf("set Ldc libaray cfg path:  ");
        scanf("%s", pstVpeChnattr->LdcCfgbin_Path);
        ST_Flush();

        ST_ReadLdcTableBin(pstVpeChnattr->LdcCfgbin_Path, &pstVpeChnattr->ldcBinBuffer[0], &pstVpeChnattr->u32LdcBinSize[0]);
    }

    /************************************************
    Step1: Stop Vpe (Wait driver all buffer done)
    *************************************************/
    STCHECKRESULT(MI_VPE_StopChannel(Vpechn));

    /************************************************
    Step2: Set Ldc on/off Param
    *************************************************/
    STCHECKRESULT(MI_VPE_GetChannelParam(Vpechn, &stVpeChnParam));
    printf("get channel param  benldc %d, bmirror %d, bflip %d, e3dnrlevel %d, hdrtype %d \n",
        stVpeChnParam.bEnLdc, stVpeChnParam.bMirror,stVpeChnParam.bFlip,stVpeChnParam.e3DNRLevel,stVpeChnParam.eHDRType);

    stVpeChnParam.bEnLdc = s32LdcOnoff;

    STCHECKRESULT(MI_VPE_SetChannelParam(Vpechn, &stVpeChnParam));

    /************************************************
    Step3: if Ldc First On Set Ldc Bin
    *************************************************/
    if(s32LdcOnoff == TRUE && pstVpeChnattr->bEnLdc == FALSE)
    {
        STCHECKRESULT(MI_VPE_LDCBegViewConfig(Vpechn));

        STCHECKRESULT(MI_VPE_LDCSetViewConfig(Vpechn, pstVpeChnattr->ldcBinBuffer[0], pstVpeChnattr->u32LdcBinSize[0]));

        STCHECKRESULT(MI_VPE_LDCEndViewConfig(Vpechn));

        //free(pstVpeChnattr->ldcBinBuffer);

#if SUPPORT_LDC
        if(mi_eptz_buffer_free(pstVpeChnattr->ldcBinBuffer[0]) != MI_EPTZ_ERR_NONE)
        {
            printf("[MI EPTZ ERR]   %d !! \n", __LINE__);
        }
#endif
        pstVpeChnattr->bEnLdc = TRUE;
    }

    /************************************************
    Step4: Start Vpe
    *************************************************/
    STCHECKRESULT(MI_VPE_StartChannel(Vpechn));

    return 0;
}
MI_S32 vpe_OpenSourceFile(const char *pFileName, int *pSrcFd)
{
    int src_fd = open(pFileName, O_RDONLY);
    if (src_fd < 0)
    {
        perror("open");
        return -1;
    }
    *pSrcFd = src_fd;

    return TRUE;
}
MI_S32 vpe_GetOneFrame(int srcFd, char *pData, int yuvSize)
{
    off_t current = lseek(srcFd,0L, SEEK_CUR);
    off_t end = lseek(srcFd,0L, SEEK_END);

    if((end - current) == 0 || (end - current) < yuvSize)
    {
        lseek(srcFd,0,SEEK_SET);
        current = lseek(srcFd,0,SEEK_CUR);
        //end = lseek(srcFd,0L,SEEK_END);
    }
    /*if ((end - current) < yuvSize)
    {
        return -1;
    }*/
    lseek(srcFd, current, SEEK_SET);
    if (read(srcFd, pData, yuvSize) == yuvSize)
    {
        return 1;
    }

    return 0;
}
void * ST_PutVpeInputDataThread(void * args)
{
    MI_SYS_ChnPort_t stVpeChnInput;
    MI_SYS_BUF_HANDLE hHandle = 0;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BufInfo_t stBufInfo;
    ST_VpeChannelAttr_t *pstVpeChnattr  = (ST_VpeChannelAttr_t *)(args);
    MI_S32 s32Ret;
    MI_U32 frame_size = 0;
    int src_fd;
    time_t stTime = 0;
    char src_file[256];
    MI_U8 u8Chnid = pstVpeChnattr->u8ChnId;

    memset(&stVpeChnInput, 0x0, sizeof(MI_SYS_ChnPort_t));
    memset(&stBufConf, 0x0, sizeof(MI_SYS_BufConf_t));
    memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));

    stVpeChnInput.eModId = E_MI_MODULE_ID_VPE;
    stVpeChnInput.u32DevId = 0;
    stVpeChnInput.u32ChnId = u8Chnid;
    stVpeChnInput.u32PortId = pstVpeChnattr->stInputFileAttr.u8VpeInPortId;

    printf("ST_PutVpeInputDataThread start get input  buffer\n");
    stBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.u64TargetPts = time(&stTime);
    stBufConf.stFrameCfg.eFormat = pstVpeChnattr->stInputFileAttr.ePixelFormat;
    stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
    stBufConf.stFrameCfg.u16Width = pstVpeChnattr->stInputFileAttr.u32Width;
    stBufConf.stFrameCfg.u16Height = pstVpeChnattr->stInputFileAttr.u32Height;
    sprintf(src_file,"%s",pstVpeChnattr->stInputFileAttr.InputFilePath);
    s32Ret = vpe_OpenSourceFile(src_file, &src_fd);
    if(s32Ret < 0)
    {
        printf("open file fail!\n");
        return NULL;
    }

    while (!bExit)
    {
        if(MI_SUCCESS == MI_SYS_ChnInputPortGetBuf(&stVpeChnInput,&stBufConf,&stBufInfo,&hHandle,0))
        {
            //start user put int buffer
            frame_size = stBufInfo.stFrameData.u32BufSize;
            if(1 == vpe_GetOneFrame(src_fd, (char *)stBufInfo.stFrameData.pVirAddr[0], frame_size))
            {
                    s32Ret = MI_SYS_ChnInputPortPutBuf(hHandle,&stBufInfo,FALSE);
            }
            else
            {
                    printf("get frame fail,Drop \n");
                    s32Ret = MI_SYS_ChnInputPortPutBuf(hHandle,&stBufInfo,TRUE);
                    printf("MI_SYS_ChnInputPortPutBuf s32Ret %d\n",s32Ret);

            }
        }
        usleep(10*1000);
    }
    close(src_fd);

    return NULL;
}
void * ST_GetVpeOutputDataThread(void * args)
{
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hHandle;
    MI_U8  u8Params = *((MI_U8 *)(args));
    MI_U8  u8Chnid = u8Params / ST_MAX_PORT_NUM;
    MI_U8  u8Portid = u8Params % ST_MAX_PORT_NUM;
    FILE *fp = NULL;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 s32Fd = 0;
    fd_set read_fds;
    struct timeval TimeoutVal;
    ST_PortAttr_t *pstVpePortAttr = &gstVpeChnattr[u8Chnid].stVpePortAttr[u8Portid];
    MI_BOOL bFileOpen = FALSE;
    MI_ModuleId_e eModeId = E_MI_MODULE_ID_VPE;
    MI_SYS_ChnPort_t stChnPort;
    MI_U32 u32VencChn = pstVpePortAttr->u32BindVencChan < ST_MAX_VENC_NUM ? pstVpePortAttr->u32BindVencChan: 0;
    ST_VencAttr_t *pstVencAttr = &gstVencattr[u32VencChn];

    if(u8Portid == ST_VPE_VIR_PORTID)
    {
        u8Portid =0;
        u8Chnid = 0;
        eModeId = E_MI_MODULE_ID_DIVP;
    }

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = eModeId;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = u8Chnid;
    stChnPort.u32PortId = u8Portid;
    s32Ret = MI_SYS_GetFd(&stChnPort,&s32Fd);
    if(s32Ret != MI_SUCCESS)
    {
        ST_ERR("MI_SYS_GetFd error %d\n",s32Ret);
        return NULL;
    }

    while (!bExit)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd,&read_fds);
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if(s32Ret < 0)
        {
            ST_ERR("select fail\n");
            usleep(10 * 1000);
            continue;
        }
        else if(s32Ret == 0)
        {
            if(pstVpePortAttr->u16UserDepth !=0)
            {
                ST_ERR("time out\n");
            }
            usleep(10 * 1000);
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd,&read_fds))
            {
                pthread_mutex_lock(&pstVpePortAttr->Portmutex);
                if(pstVpePortAttr->s32DumpBuffNum > 0 && bFileOpen == FALSE)
                {
                    fp = fopen(pstVpePortAttr->FilePath ,"wb");
                    if(fp == NULL)
                    {
                        printf("file %s open fail\n", pstVpePortAttr->FilePath);
                        pstVpePortAttr->s32DumpBuffNum = 0;
                        pthread_mutex_unlock(&pstVpePortAttr->Portmutex);
                        continue;
                    }
                    else
                    {
                        bFileOpen = TRUE;
                        printf("open file %s \n", pstVpePortAttr->FilePath);
                    }
                }

                if(pstVpePortAttr->bUsed == TRUE && pstVencAttr->eBindType != E_MI_SYS_BIND_TYPE_REALTIME)//vpe realtime bind get output map viraddr fail
                {
                    if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hHandle))
                    {
                        //printf("get out success \n");
                        if(pstVpePortAttr->s32DumpBuffNum > 0)
                        {
                            pstVpePortAttr->s32DumpBuffNum--;
                            printf(
"=======begin writ port %d file id %d, file path %s, bufsize %d, stride %d, height %d\n", u8Portid, pstVpePortAttr->s32DumpBuffNum, pstVpePortAttr->FilePath,
                                stBufInfo.stFrameData.u32BufSize,stBufInfo.stFrameData.u32Stride[0], stBufInfo.stFrameData.u16Height);

                            fwrite(stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.u32BufSize, 1, fp);
                            printf(
"=======end   writ port %d file id %d, file path %s \n", u8Portid, pstVpePortAttr->s32DumpBuffNum, pstVpePortAttr->FilePath);
                        }

                        //printf("begin release out \n");
                        MI_SYS_ChnOutputPortPutBuf(hHandle);
                        //printf("end release out \n");
                    }
                }

                if(bFileOpen == TRUE && pstVpePortAttr->s32DumpBuffNum == 0)
                {
                    fclose(fp);
                    bFileOpen = FALSE;
                    printf("close file %s \n", pstVpePortAttr->FilePath);
                }

                pthread_mutex_unlock(&pstVpePortAttr->Portmutex);
            }
        }
    }
    return NULL;
}

#if 0
void * ST_GetVpeOutputDataThread(void * args)
{
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hHandle;
    MI_U8  u8Params = *((MI_U8 *)(args));
    MI_U8  u8Chnid = u8Params / ST_MAX_PORT_NUM;
    MI_U8  u8Portid = u8Params % ST_MAX_PORT_NUM;
    FILE *fp = NULL;
    ST_PortAttr_t *pstVpePortAttr = &gstVpeChnattr[u8Chnid].stVpePortAttr[u8Portid];
    MI_BOOL bFileOpen = FALSE;
    MI_ModuleId_e eModeId = E_MI_MODULE_ID_VPE;
    MI_SYS_ChnPort_t stChnPort;
    MI_U32 u32VencChn = pstVpePortAttr->u32BindVencChan < ST_MAX_VENC_NUM ? pstVpePortAttr->u32BindVencChan: 0;
    ST_VencAttr_t *pstVencAttr = &gstVencattr[u32VencChn];

    if(u8Portid == ST_VPE_VIR_PORTID)
    {
        u8Portid =0;
        u8Chnid = 0;
        eModeId = E_MI_MODULE_ID_DIVP;
    }

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = eModeId;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = u8Chnid;
    stChnPort.u32PortId = u8Portid;

    while (!bExit)
    {
        pthread_mutex_lock(&pstVpePortAttr->Portmutex);

        if(pstVpePortAttr->s32DumpBuffNum > 0 && bFileOpen == FALSE)
        {
            fp = fopen(pstVpePortAttr->FilePath ,"wb");
            if(fp == NULL)
            {
                printf("file %s open fail\n", pstVpePortAttr->FilePath);
                pstVpePortAttr->s32DumpBuffNum = 0;
                pthread_mutex_unlock(&pstVpePortAttr->Portmutex);
                continue;
            }
            else
            {
                bFileOpen = TRUE;
                printf("open file %s \n", pstVpePortAttr->FilePath);
            }
        }

        if(pstVpePortAttr->bUsed == TRUE && pstVencAttr->eBindType != E_MI_SYS_BIND_TYPE_REALTIME)//vpe realtime bind get output map viraddr fail
        {
            if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hHandle))
            {
                //printf("get out success \n");
                if(pstVpePortAttr->s32DumpBuffNum > 0)
                {
                    pstVpePortAttr->s32DumpBuffNum--;
                    printf(
"=======begin writ port %d file id %d, file path %s, bufsize %d, stride %d, height %d\n", u8Portid, pstVpePortAttr->s32DumpBuffNum, pstVpePortAttr->FilePath,
                        stBufInfo.stFrameData.u32BufSize,stBufInfo.stFrameData.u32Stride[0], stBufInfo.stFrameData.u16Height);

                    fwrite(stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.u32BufSize, 1, fp);
                    printf(
"=======end   writ port %d file id %d, file path %s \n", u8Portid, pstVpePortAttr->s32DumpBuffNum, pstVpePortAttr->FilePath);
                }

                //printf("begin release out \n");
                MI_SYS_ChnOutputPortPutBuf(hHandle);
                //printf("end release out \n");
            }
        }

        if(bFileOpen == TRUE && pstVpePortAttr->s32DumpBuffNum == 0)
        {
            fclose(fp);
            bFileOpen = FALSE;
            printf("close file %s \n", pstVpePortAttr->FilePath);
        }

        pthread_mutex_unlock(&pstVpePortAttr->Portmutex);
        usleep(10*1000);
    }

    return NULL;
}
#endif
/*
void *ST_IQthread(void * args)
{
    MI_VIF_ChnPortAttr_t stVifAttr;
    MI_VPE_ChannelPara_t stVpeParam;
    MI_VPE_HDRType_e  eLastHdrType = E_MI_VPE_HDR_TYPE_MAX;
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    MI_U8  u8ispreadycnt = 0;

    memset(&stVifAttr, 0x0, sizeof(MI_VIF_ChnPortAttr_t));
    memset(&stVpeParam, 0x0, sizeof(MI_VPE_ChannelPara_t));

    MI_IQSERVER_Open(1920, 1080, 0);

    while(1)
    {
        if(u8ispreadycnt > 100)
        {
            printf("%s:%d, isp ready time out \n", __FUNCTION__, __LINE__);
            u8ispreadycnt = 0;
        }

        MI_ISP_IQ_GetParaInitStatus(0, &status);
        if(status.stParaAPI.bFlag != 1)
        {
            usleep(10*1000);
            u8ispreadycnt++;
            continue;
        }

        u8ispreadycnt = 0;

        MI_VPE_GetChannelParam(0, &stVpeParam);
        if(eLastHdrType != stVpeParam.eHDRType)
        {
            printf("hdr type change before %d, current %d, load api bin\n", eLastHdrType, stVpeParam.eHDRType);
            MI_ISP_API_CmdLoadBinFile(0, (char *)((stVpeParam.eHDRType>0) ? HDR_PATH : NONHDR_PATH),  1234);
        }
        eLastHdrType = stVpeParam.eHDRType;

        usleep(10*1000);
    }

    return  NULL;
}*/

MI_S32 ST_CreatDivpChn(MI_DIVP_CHN DivpChn, MI_U32 u32MaxWidth, MI_U32 u32MaxHeight)
{
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stDivpPortAttr;
    ST_DivpAttr_t *pstDivpChnAttr = &gstDivpAttr;
    memset(&stDivpChnAttr, 0x0, sizeof(MI_DIVP_ChnAttr_t));
    memset(&stDivpPortAttr, 0x0, sizeof(MI_DIVP_OutputPortAttr_t));

    stDivpChnAttr.u32MaxWidth = u32MaxWidth;
    stDivpChnAttr.u32MaxHeight = u32MaxHeight;
    stDivpChnAttr.stCropRect.u16X = pstDivpChnAttr->stDivpPort.stPortCrop.u16X;
    stDivpChnAttr.stCropRect.u16Y = pstDivpChnAttr->stDivpPort.stPortCrop.u16Y;
    stDivpChnAttr.stCropRect.u16Width = pstDivpChnAttr->stDivpPort.stPortCrop.u16Width;
    stDivpChnAttr.stCropRect.u16Height = pstDivpChnAttr->stDivpPort.stPortCrop.u16Height;
    stDivpChnAttr.bVerMirror = pstDivpChnAttr->stDivpPort.bFlip;
    stDivpChnAttr.bHorMirror = pstDivpChnAttr->stDivpPort.bMirror;
    STCHECKRESULT(MI_DIVP_CreateChn(DivpChn, &stDivpChnAttr));

    stDivpPortAttr.u32Width = pstDivpChnAttr->stDivpPort.stPortSize.u16Width;
    stDivpPortAttr.u32Height = pstDivpChnAttr->stDivpPort.stPortSize.u16Height;
    stDivpPortAttr.ePixelFormat = pstDivpChnAttr->stDivpPort.ePixelFormat;
    STCHECKRESULT(MI_DIVP_SetOutputPortAttr(DivpChn, &stDivpPortAttr));

    STCHECKRESULT(MI_DIVP_StartChn(DivpChn));

    return 0;
}

MI_S32 ST_DestroyDivpChn(MI_DIVP_CHN DivpChn)
{
    STCHECKRESULT(MI_DIVP_StopChn(DivpChn));
    STCHECKRESULT(MI_DIVP_DestroyChn(DivpChn));

    return 0;
}
MI_S32 ST_VencStart(MI_U32 u32MaxVencWidth, MI_U32 u32MaxVencHeight, MI_U32 u32VencChn)
{
    MI_U32 u32VenBitRate = 0;
    MI_U32 u32VifDev = 0;
    MI_U32 u32Fps =0;
    MI_U32 u32VpeChn=0;
    ST_VencAttr_t *pstStreamAttr = &gstVencattr[u32VencChn];
    ST_Sensor_Attr_t *pstSensorAttr = NULL;
    ST_VifDevAttr_t *pstVifDevtAttr = NULL;
    ST_VpeChannelAttr_t *pstVpeChnAttr = NULL;
    MI_SNR_PAD_ID_e eSnrPadId = E_MI_SNR_PAD_ID_0;
    MI_VENC_ChnAttr_t stChnAttr;

    u32VpeChn = pstStreamAttr->u32BindVpeChn;
    pstVpeChnAttr = &gstVpeChnattr[u32VpeChn];
    u32VifDev = pstVpeChnAttr->stInputPortAttr[0].stBindParam.stChnPort.u32DevId;
    pstVifDevtAttr = &gstVifModule.stVifDevAttr[u32VifDev];
    eSnrPadId = (MI_SNR_PAD_ID_e)pstVifDevtAttr->stBindSensor.eSensorPadID;
    pstSensorAttr = &gstSensorAttr[eSnrPadId];

    if(pstSensorAttr->bUsed)
        MI_SNR_GetFps(eSnrPadId, &u32Fps);
    else
        u32Fps = 30;
    //When put YUV file in VPE,sensor is not use,set default fps =30

   // ST_VencAttr_t *pstStreamAttr = &gstVencattr[u32VencChn];
    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));

    u32VenBitRate = ((pstStreamAttr->u32Width * pstStreamAttr->u32Height + 500000)/1000000)*1024*1024;
    if(u32VenBitRate == 0)
        u32VenBitRate = 2*1024*1024;

    //pstStreamAttr->u32Width = gstVpeChnattr[u32VpeChn].stVpePortAttr[i].stPortSize.u16Width;
   // pstStreamAttr->u32Height = gstVpeChnattr[u32VpeChn].stVpePortAttr[i].stPortSize.u16Height;

    DBG_INFO("chn %d, pichwidth %d, height %d, MaxWidth %d, MaxHeight %d bitrate %d, fps %d \n", u32VencChn,
        pstStreamAttr->u32Width, pstStreamAttr->u32Height, u32MaxVencWidth, u32MaxVencHeight, u32VenBitRate, u32Fps);
    if(pstStreamAttr->eType == E_MI_VENC_MODTYPE_H264E)
    {
        stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = pstStreamAttr->u32Width;
        stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = pstStreamAttr->u32Height;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = u32MaxVencWidth;
        stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
        stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = u32MaxVencHeight;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = u32VenBitRate;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = u32Fps;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
    }
    else if(pstStreamAttr->eType == E_MI_VENC_MODTYPE_H265E)
    {
        stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = pstStreamAttr->u32Width;
        stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = pstStreamAttr->u32Height;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth = u32MaxVencWidth;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight = u32MaxVencHeight;
        stChnAttr.stVeAttr.stAttrH265e.bByFrame = TRUE;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265CBR;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = u32VenBitRate;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateNum = u32Fps;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32Gop = 30;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32StatTime = 0;

    }
    else if(pstStreamAttr->eType == E_MI_VENC_MODTYPE_JPEGE)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = pstStreamAttr->u32Width;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = pstStreamAttr->u32Height;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = u32MaxVencWidth;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32MaxVencHeight;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32BitRate = 30;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateNum = u32Fps;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateDen = 1;
    }
    stChnAttr.stVeAttr.eType = pstStreamAttr->eType;
    STCHECKRESULT(ST_Venc_CreateChannel(u32VencChn, &stChnAttr));

    MI_VENC_InputSourceConfig_t stVencSourceCfg;
    if(pstStreamAttr->eBindType == E_MI_SYS_BIND_TYPE_HW_RING)
    {
        pstStreamAttr->u32BindParam = pstStreamAttr->u32Height;
        stVencSourceCfg.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_RING_ONE_FRM;
    }
    else
        stVencSourceCfg.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_NORMAL_FRMBASE;

    MI_VENC_SetInputSourceConfig(u32VencChn, &stVencSourceCfg);
    STCHECKRESULT(ST_Venc_StartChannel(u32VencChn));


    pstStreamAttr->bStart = TRUE;

    return MI_SUCCESS;
}

MI_S32 ST_VencStop(MI_U32 u32VencChn)
{
    MI_U32 u32vencChn = u32VencChn;
    ST_VencAttr_t *pstStreamAttr = &gstVencattr[u32vencChn];

    STCHECKRESULT(ST_Venc_StopChannel(u32vencChn));
    STCHECKRESULT(ST_Venc_DestoryChannel(u32vencChn));

    pstStreamAttr->bStart = FALSE;

    return MI_SUCCESS;
}

MI_S32 ST_SensorModuleInit(MI_SNR_PAD_ID_e eSnrPad)
{
    MI_SNR_PAD_ID_e eSnrPadId = eSnrPad;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_U8 u8ChocieRes =gstSensorAttr[eSnrPad].u8ResIndex;
    MI_S32 s32Input =0;
    MI_SNR_Res_t stRes;
    ST_Sensor_Attr_t *pstSensorAttr = NULL;
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));

    pstSensorAttr = &gstSensorAttr[eSnrPadId];

    /************************************************
    Step2:  init Sensor
    *************************************************/
    if(pstSensorAttr->bPlaneMode ==TRUE)
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, FALSE));
    }
    else
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, TRUE));
    }

    STCHECKRESULT(MI_SNR_QueryResCount(eSnrPadId, &u32ResCount));
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        STCHECKRESULT(MI_SNR_GetRes(eSnrPadId, u8ResIndex, &stRes));
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }
    if(u8ChocieRes >= u32ResCount && u8ChocieRes != 0xff)
    {
        printf("res set err  %d > =cnt %d\n", u8ChocieRes, u32ResCount);
        return TRUE;
    }
    else if(u8ChocieRes == 0xff)
    {
        printf("choice which resolution use, cnt %d\n", u32ResCount);
        do
        {
            scanf("%d", &s32Input);
            u8ChocieRes = (MI_U8)s32Input;
            ST_Flush();
            STCHECKRESULT(MI_SNR_QueryResCount(eSnrPadId, &u32ResCount));
            if(u8ChocieRes >= u32ResCount)
            {
                printf("choice err res %d > =cnt %d\n", u8ChocieRes, u32ResCount);
            }
        }while(u8ChocieRes >= u32ResCount);
        printf("You select %d res\n", u8ChocieRes);
    }
    printf("Rest %d\n", u8ChocieRes);

    STCHECKRESULT(MI_SNR_SetOrien(eSnrPadId, pstSensorAttr->u8Mirror, pstSensorAttr->u8Flip));
    STCHECKRESULT(MI_SNR_SetRes(eSnrPadId,u8ChocieRes));
    STCHECKRESULT(MI_SNR_Enable(eSnrPadId));

    return MI_SUCCESS;
}
MI_S32 ST_VifModuleInit(MI_VIF_DEV VifDev)
{
    MI_VIF_DEV vifDev = VifDev;
    MI_U8 vifchnPerDev = 0;
    MI_VIF_CHN vifChn =0;
    MI_VIF_PORT vifPort = 0;
    ST_VifDevAttr_t *pstVifDevAttr = &gstVifModule.stVifDevAttr[vifDev];
    MI_SNR_PAD_ID_e eSnrPadId = (MI_SNR_PAD_ID_e)pstVifDevAttr->stBindSensor.eSensorPadID;
    MI_U32 u32PlaneId = pstVifDevAttr->stBindSensor.u32PlaneID;

    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));

   // MI_U8 i=0;

    /************************************************
    Step3:  init VIF
    *************************************************/
    /*MI_VIF_Dev2SnrPadMuxCfg_t stVifDev[4];
    stVifDev[0].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
    stVifDev[0].u32PlaneID = 0xff;
    stVifDev[1].eSensorPadID = E_MI_VIF_SNRPAD_ID_2;
    stVifDev[1].u32PlaneID = 0xff;
    stVifDev[2].eSensorPadID = E_MI_VIF_SNRPAD_ID_1;
    stVifDev[2].u32PlaneID = 0xff;
    stVifDev[3].eSensorPadID = E_MI_VIF_SNRPAD_ID_3;
    stVifDev[3].u32PlaneID = 0xff;
    MI_S32 MI_VIF_SetDev2SnrPadMux(stVifDev, 4)*/

    MI_VIF_DevAttr_t stDevAttr;
    memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

    if(pstVifDevAttr->bUsed == TRUE
       && pstVifDevAttr->bCreate == FALSE)
    {
        STCHECKRESULT(MI_SNR_GetPadInfo(eSnrPadId, &stPad0Info));
        STCHECKRESULT(MI_SNR_GetPlaneInfo(eSnrPadId, u32PlaneId, &stSnrPlane0Info));

        pstVifDevAttr->bCreate = TRUE;

        stDevAttr.eIntfMode = stPad0Info.eIntfMode;
        stDevAttr.eWorkMode = pstVifDevAttr->eWorkMode;
        stDevAttr.eHDRType = pstVifDevAttr->eHDRType;
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

        STCHECKRESULT(MI_VIF_SetDevAttr(vifDev, &stDevAttr));
        STCHECKRESULT(MI_VIF_EnableDev(vifDev));
    }

    for(vifchnPerDev=0; vifchnPerDev< ST_MAX_VIF_CHN_PERDEV; vifchnPerDev++)
    {
        for(vifPort=0; vifPort< ST_MAX_VIF_OUTPORT_NUM; vifPort++)
        {
            ST_VifPortAttr_t *pstVifChnPortAttr = &pstVifDevAttr->stVifChnAttr[vifchnPerDev].stVifOutPortAttr[vifPort];
            if(pstVifChnPortAttr->bUsed == TRUE
                && pstVifChnPortAttr->bCreate== FALSE)
            {
                vifChn = VifDev*ST_MAX_VIF_CHN_PERDEV+vifchnPerDev;

                MI_VIF_ChnPortAttr_t stVifPortInfo;
                memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));

                if(pstVifChnPortAttr->stCapRect.u16Width == 0
                    || pstVifChnPortAttr->stCapRect.u16Height == 0)
                {
                    pstVifChnPortAttr->stCapRect.u16X = stSnrPlane0Info.stCapRect.u16X;
                    pstVifChnPortAttr->stCapRect.u16Y = stSnrPlane0Info.stCapRect.u16Y;
                    pstVifChnPortAttr->stCapRect.u16Width = stSnrPlane0Info.stCapRect.u16Width;
                    pstVifChnPortAttr->stCapRect.u16Height = stSnrPlane0Info.stCapRect.u16Height;
                }

                if(pstVifChnPortAttr->stDestSize.u16Width == 0
                    || pstVifChnPortAttr->stDestSize.u16Height ==0)
                {
                    pstVifChnPortAttr->stDestSize.u16Width = stSnrPlane0Info.stCapRect.u16Width;
                    pstVifChnPortAttr->stDestSize.u16Height = stSnrPlane0Info.stCapRect.u16Height;
                }

                if(pstVifChnPortAttr->ePixFormat >= E_MI_SYS_PIXEL_FRAME_FORMAT_MAX)
                {
                    if(stSnrPlane0Info.eBayerId >= E_MI_SYS_PIXEL_BAYERID_MAX)
                    {
                        pstVifChnPortAttr->ePixFormat = stSnrPlane0Info.ePixel;
                    }
                    else
                        pstVifChnPortAttr->ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
                }

                stVifPortInfo.stCapRect.u16X = pstVifChnPortAttr->stCapRect.u16X;
                stVifPortInfo.stCapRect.u16Y = pstVifChnPortAttr->stCapRect.u16Y;
                stVifPortInfo.stCapRect.u16Width =  pstVifChnPortAttr->stCapRect.u16Width;
                stVifPortInfo.stCapRect.u16Height = pstVifChnPortAttr->stCapRect.u16Height;
                stVifPortInfo.stDestSize.u16Width = pstVifChnPortAttr->stCapRect.u16Width;
                stVifPortInfo.stDestSize.u16Height = pstVifChnPortAttr->stCapRect.u16Height;
                printf("sensor bayerid %d, bit mode %d \n", stSnrPlane0Info.eBayerId, stSnrPlane0Info.ePixPrecision);
                stVifPortInfo.ePixFormat = pstVifChnPortAttr->ePixFormat;
                //stVifPortInfo.u32FrameModeLineCount for lowlantancy mode

                if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
                {
                    stVifPortInfo.eFrameRate = E_MI_VIF_FRAMERATE_FULL;
                    stVifPortInfo.eCapSel = E_MI_SYS_FIELDTYPE_BOTH;
                    stVifPortInfo.eScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
                }
                STCHECKRESULT(MI_VIF_SetChnPortAttr(vifChn, vifPort, &stVifPortInfo));
                STCHECKRESULT(MI_VIF_EnableChnPort(vifChn, vifPort));

                pstVifChnPortAttr->bCreate = TRUE;
            }
        }
    }

    return MI_SUCCESS;
}
MI_S32 ST_VpeModuleInit(MI_VPE_CHANNEL Vpechn)
{
    MI_VPE_CHANNEL vpechn = Vpechn;
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_DEV vifDev = 0;
    MI_VIF_CHN vifChn = 0;
    MI_U8 vifChnPerDev=0;
    MI_VIF_PORT vifPort = 0;
    MI_U8 i=0;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_VifDevAttr_t *pstVifDevAttr = NULL;
    ST_VifPortAttr_t *pstVifPortAttr = NULL;
    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[vpechn];
    vifChn = pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32ChnId;
    vifPort = pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32PortId;
    vifDev = pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32DevId;
    vifChnPerDev = vifChn%ST_MAX_VIF_CHN_PERDEV;
    pstVifDevAttr = &gstVifModule.stVifDevAttr[vifDev];
    pstVifPortAttr = &pstVifDevAttr->stVifChnAttr[vifChnPerDev].stVifOutPortAttr[vifPort];

    if(pstVifPortAttr->bUsed)
    {
        MI_VIF_ChnPortAttr_t stVifPortInfo;
        memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));
        STCHECKRESULT(MI_VIF_GetChnPortAttr(vifChn, vifPort, &stVifPortInfo));
        printf("vif pixel %d \n",stVifPortInfo.ePixFormat);

        u32CapWidth = stVifPortInfo.stDestSize.u16Width;
        u32CapHeight = stVifPortInfo.stDestSize.u16Height;
        ePixFormat = stVifPortInfo.ePixFormat;
    }
    else
    {
        u32CapWidth = gstVpeChnattr[vpechn].stInputFileAttr.u32Width;
        u32CapHeight = gstVpeChnattr[vpechn].stInputFileAttr.u32Height;
        ePixFormat = gstVpeChnattr[vpechn].stInputFileAttr.ePixelFormat;
    }

    if( pstVpeChnattr->eRunningMode == E_MI_VPE_RUN_CAM_MODE
        &&(ePixFormat == E_MI_SYS_PIXEL_FRAME_YUV422_YUYV
        || ePixFormat == E_MI_SYS_PIXEL_FRAME_YUV422_UYVY
        || ePixFormat == E_MI_SYS_PIXEL_FRAME_YUV422_YVYU
        || ePixFormat == E_MI_SYS_PIXEL_FRAME_YUV422_VYUY))
    {
        pstVpeChnattr->eRunningMode = E_MI_VPE_RUN_DVR_MODE;
    }

    /************************************************
    Step1:  init VPE (create one VPE)
    *************************************************/
    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_VPE_ChannelPara_t stChannelVpeParam;

    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelAttr_t));
    memset(&stChannelVpeParam, 0x00, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeAttr.u16MaxW = u32CapWidth;
    stChannelVpeAttr.u16MaxH = u32CapHeight;
    stChannelVpeAttr.ePixFmt = ePixFormat;
    stChannelVpeAttr.eRunningMode = pstVpeChnattr->eRunningMode;
    if(pstVpeChnattr->eRunningMode == E_MI_VPE_RUN_DVR_MODE)
        stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR_INVALID;
    else
        stChannelVpeAttr.eSensorBindId = (MI_VPE_SensorChannel_e)(pstVifDevAttr->stBindSensor.eSensorPadID+1);
    stChannelVpeAttr.bEnLdc = pstVpeChnattr->bEnLdc;
    stChannelVpeAttr.u32ChnPortMode = pstVpeChnattr->u32ChnPortMode;
    stChannelVpeAttr.eHDRType = pstVpeChnattr->eHdrType;
    STCHECKRESULT(MI_VPE_CreateChannel(vpechn, &stChannelVpeAttr));

    stChannelVpeParam.eHDRType = pstVpeChnattr->eHdrType;
    stChannelVpeParam.e3DNRLevel = pstVpeChnattr->e3DNRLevel;
    stChannelVpeParam.bMirror = pstVpeChnattr->bChnMirror;
    stChannelVpeParam.bFlip = pstVpeChnattr->bChnFlip;
    stChannelVpeParam.bEnLdc = pstVpeChnattr->bEnLdc;
    STCHECKRESULT(MI_VPE_SetChannelParam(vpechn, &stChannelVpeParam));

    if(pstVpeChnattr->bEnLdc == TRUE)
    {
        pstVpeChnattr->s32Rot[0] = 0;
        pstVpeChnattr->s32Rot[1] = 90;
        pstVpeChnattr->s32Rot[2] = 180;
        pstVpeChnattr->s32Rot[3] = 270;//parame for test
        ST_GetLdcBinBuffer(pstVpeChnattr);
    }

    if(pstVpeChnattr->ldcBinBuffer[0] != NULL)
    {
        MI_VPE_LDCBegViewConfig(vpechn);

        for(i=0; i<pstVpeChnattr->u32ViewNum; i++)
        {
            MI_VPE_LDCSetViewConfig(vpechn, pstVpeChnattr->ldcBinBuffer[i], pstVpeChnattr->u32LdcBinSize[i]);
#if SUPPORT_LDC

            //free(pstVpeChnattr->ldcBinBuffer);
            if(mi_eptz_buffer_free(pstVpeChnattr->ldcBinBuffer[i]) != MI_EPTZ_ERR_NONE)
            {
                printf("[MI EPTZ ERR]   %d !! \n", __LINE__);
            }
#endif
        }
        STCHECKRESULT(MI_VPE_LDCEndViewConfig(vpechn));
    }
    else
        printf("##############benldc %d, ldc bin buffer %p \n",pstVpeChnattr->bEnLdc, pstVpeChnattr->ldcBinBuffer);
    STCHECKRESULT(MI_VPE_SetChannelRotation(vpechn, pstVpeChnattr->eVpeRotate));

    if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
       || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
     {
        if(pstVpeChnattr->stOrgVpeChnCrop.u16Height !=0)
            pstVpeChnattr->stVpeChnCrop.u16X = u32CapHeight - pstVpeChnattr->stOrgVpeChnCrop.u16Y-pstVpeChnattr->stOrgVpeChnCrop.u16Height;
        else
            pstVpeChnattr->stVpeChnCrop.u16X = 0;

        pstVpeChnattr->stVpeChnCrop.u16Y = pstVpeChnattr->stOrgVpeChnCrop.u16X;
        pstVpeChnattr->stVpeChnCrop.u16Width = pstVpeChnattr->stOrgVpeChnCrop.u16Height;
        pstVpeChnattr->stVpeChnCrop.u16Height = pstVpeChnattr->stOrgVpeChnCrop.u16Width;
    }
    else
    {
        pstVpeChnattr->stVpeChnCrop.u16X = pstVpeChnattr->stOrgVpeChnCrop.u16X;
        pstVpeChnattr->stVpeChnCrop.u16Y = pstVpeChnattr->stOrgVpeChnCrop.u16Y;
        pstVpeChnattr->stVpeChnCrop.u16Width = pstVpeChnattr->stOrgVpeChnCrop.u16Width;
        pstVpeChnattr->stVpeChnCrop.u16Height = pstVpeChnattr->stOrgVpeChnCrop.u16Height;
    }

    STCHECKRESULT(MI_VPE_SetChannelCrop(vpechn, &pstVpeChnattr->stVpeChnCrop));

    STCHECKRESULT(MI_VPE_StartChannel (vpechn));

    for(i=0; i<ST_MAX_PORT_NUM; i++)
    {
        MI_VPE_PortMode_t stVpeMode;
        MI_VENC_CHN vencChn = 0;
        MI_U32  u32SclSourceHeight = 0;
        memset(&stVpeMode, 0, sizeof(stVpeMode));

        ST_PortAttr_t *pstVpePortAttr = &pstVpeChnattr->stVpePortAttr[i];

        vencChn = pstVpePortAttr->u32BindVencChan;
        ST_VencAttr_t *pstVencAttr = &gstVencattr[vencChn];

        if(pstVpePortAttr->bUsed == TRUE)
        {
            if(i==3)//port3 not scaling
            {
                pstVpePortAttr->stOrigPortSize.u16Width = u32CapWidth;
                pstVpePortAttr->stOrigPortSize.u16Height = u32CapHeight;
            }
            else if(i==4)//port4 IR port width/height is source 1/2
            {
                pstVpePortAttr->stOrigPortSize.u16Width = u32CapWidth/2;
                pstVpePortAttr->stOrigPortSize.u16Height = u32CapHeight/2;
            }

            if(pstVpeChnattr->u32ChnPortMode ==0
                || pstVpeChnattr->stVpeChnCrop.u16Width ==0
                || pstVpeChnattr->stVpeChnCrop.u16Height ==0)
            {
                u32SclSourceHeight = u32CapHeight;
            }
            else
            {
                u32SclSourceHeight =  pstVpeChnattr->stOrgVpeChnCrop.u16Height;
            }

            if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
               || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
            {
                if(pstVpePortAttr->stOrigPortCrop.u16Height !=0)
                    pstVpePortAttr->stPortCrop.u16X = u32SclSourceHeight - pstVpePortAttr->stOrigPortCrop.u16Y - pstVpePortAttr->stOrigPortCrop.u16Height;
                else
                    pstVpePortAttr->stPortCrop.u16X = 0;

                pstVpePortAttr->stPortCrop.u16Y = pstVpePortAttr->stOrigPortCrop.u16X;
                pstVpePortAttr->stPortCrop.u16Width = pstVpePortAttr->stOrigPortCrop.u16Height;
                pstVpePortAttr->stPortCrop.u16Height = pstVpePortAttr->stOrigPortCrop.u16Width;

                pstVpePortAttr->stPortSize.u16Width = pstVpePortAttr->stOrigPortSize.u16Height;
                pstVpePortAttr->stPortSize.u16Height = pstVpePortAttr->stOrigPortSize.u16Width;
            }
            else
            {
                pstVpePortAttr->stPortCrop.u16X = pstVpePortAttr->stOrigPortCrop.u16X;
                pstVpePortAttr->stPortCrop.u16Y = pstVpePortAttr->stOrigPortCrop.u16Y;
                pstVpePortAttr->stPortCrop.u16Width = pstVpePortAttr->stOrigPortCrop.u16Width;
                pstVpePortAttr->stPortCrop.u16Height = pstVpePortAttr->stOrigPortCrop.u16Height;

                pstVpePortAttr->stPortSize.u16Width = pstVpePortAttr->stOrigPortSize.u16Width;
                pstVpePortAttr->stPortSize.u16Height = pstVpePortAttr->stOrigPortSize.u16Height;
            }

            if(i!=3 && i!=4 && i!=ST_VPE_VIR_PORTID) //isp dma/ir/vir port not support crop
                STCHECKRESULT(MI_VPE_SetPortCrop(vpechn, i, &pstVpeChnattr->stVpePortAttr[i].stPortCrop));

            stVpeMode.u16Width = pstVpeChnattr->stVpePortAttr[i].stPortSize.u16Width;
            stVpeMode.u16Height = pstVpeChnattr->stVpePortAttr[i].stPortSize.u16Height;
            stVpeMode.ePixelFormat = pstVpeChnattr->stVpePortAttr[i].ePixelFormat;
            stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
            stVpeMode.bMirror =pstVpeChnattr->stVpePortAttr[i].bMirror;
            stVpeMode.bFlip = pstVpeChnattr->stVpePortAttr[i].bFlip;

            if(i!=ST_VPE_VIR_PORTID) //virport no support size/pixel config
                STCHECKRESULT(MI_VPE_SetPortMode(vpechn, i, &stVpeMode));

            STCHECKRESULT(MI_VPE_EnablePort(vpechn, i));

            MI_SYS_ChnPort_t stChnPort;
            stChnPort.eModId = E_MI_MODULE_ID_VPE;
            stChnPort.u32DevId = 0;
            stChnPort.u32ChnId = vpechn;
            stChnPort.u32PortId = i;

            MI_SYS_SetChnOutputPortDepth(&stChnPort , pstVpePortAttr->u16UserDepth, pstVpePortAttr->u16Depth);

            if(gbPreviewByVenc == TRUE)
            {
                MI_U32 u32MaxWidth =0, u32MaxHeight =0;
                ST_Sys_BindInfo_T stBindInfo;

                MI_U32 u32DevId = -1;
                MI_U32 u32VencChn = 0;
                if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
                    || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
                {
                    u32MaxWidth = u16MaxH;
                    u32MaxHeight = u16MaxW;
                }
                else
                {
                    u32MaxWidth = u16MaxW;
                    u32MaxHeight = u16MaxH;
                }
                pstVencAttr->u32Width = pstVpeChnattr->stVpePortAttr[i].stPortSize.u16Width;
                pstVencAttr->u32Height = pstVpeChnattr->stVpePortAttr[i].stPortSize.u16Height;
                u32VencChn =  pstVpePortAttr->u32BindVencChan;

                STCHECKRESULT(ST_VencStart(u32MaxWidth,u32MaxHeight,u32VencChn));

                ExecFunc(MI_VENC_GetChnDevid(u32VencChn, &u32DevId), MI_SUCCESS);

                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));

                if(i != ST_VPE_VIR_PORTID)
                {
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = pstVencAttr->u32BindVpeChn;
                    stBindInfo.stSrcChnPort.u32PortId = pstVencAttr->u32BindVpePort;
                }
                else
                {
                    MI_DIVP_CHN DivpChn =0;
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = DivpChn;
                    stBindInfo.stSrcChnPort.u32PortId = 0;
                }

                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
                stBindInfo.stDstChnPort.u32DevId = u32DevId;
                stBindInfo.stDstChnPort.u32ChnId = u32VencChn;
                stBindInfo.stDstChnPort.u32PortId = 0;

                stBindInfo.u32SrcFrmrate = 30;
                stBindInfo.u32DstFrmrate = 30;
                stBindInfo.eBindType = pstVencAttr->eBindType;
                stBindInfo.u32BindParam = pstVencAttr->u32BindParam;
                STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            }
        }
    }
    if(ST_VPE_VIR_PORTID < ST_MAX_PORT_NUM
        && pstVpeChnattr->stVpePortAttr[ST_VPE_VIR_PORTID].bUsed == TRUE)
    {
        MI_DIVP_CHN DivpChn = 0;
        MI_U32 u32MaxWidth = u16MaxW, u32MaxHeight=u16MaxH;

        ST_DivpAttr_t *pstDivpChnAttr = &gstDivpAttr;

        memcpy(&pstDivpChnAttr->stDivpPort, &pstVpeChnattr->stVpePortAttr[ST_VPE_VIR_PORTID], sizeof(ST_PortAttr_t));

        ST_CreatDivpChn(DivpChn, u32MaxWidth, u32MaxHeight);

        /************************************************
        Step2:  bind VPE port7->DIVP
        *************************************************/
        ST_Sys_BindInfo_T stBindInfo;
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = vpechn;
        stBindInfo.stSrcChnPort.u32PortId = ST_VPE_VIR_PORTID;

        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
        stBindInfo.stDstChnPort.u32DevId = 0;
        stBindInfo.stDstChnPort.u32ChnId = DivpChn;
        stBindInfo.stDstChnPort.u32PortId = 0;

        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

        MI_SYS_ChnPort_t stChnPort;
        stChnPort.eModId = E_MI_MODULE_ID_DIVP;
        stChnPort.u32DevId = 0;
        stChnPort.u32ChnId = DivpChn;
        stChnPort.u32PortId = 0;

        MI_SYS_SetChnOutputPortDepth(&stChnPort , pstDivpChnAttr->stDivpPort.u16UserDepth, pstDivpChnAttr->stDivpPort.u16Depth);
    }

    for(i=0; i<ST_MAX_VPE_INPORT_NUM; i++)
    {
        vifChn = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32ChnId;
        vifPort = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32PortId;
        vifDev = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32DevId;
        vifChnPerDev = vifChn%ST_MAX_VIF_CHN_PERDEV;

        pstVifDevAttr = &gstVifModule.stVifDevAttr[vifDev];
        pstVifPortAttr = &pstVifDevAttr->stVifChnAttr[vifChnPerDev].stVifOutPortAttr[vifPort];
        if(pstVpeChnattr->stInputPortAttr[i].bUsed ==TRUE
            && pstVifPortAttr->bUsed == TRUE
)
        {
            ST_Sys_BindInfo_T stBindInfo;
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
            stBindInfo.stSrcChnPort.u32DevId = vifDev;
            stBindInfo.stSrcChnPort.u32ChnId = vifChn;
            stBindInfo.stSrcChnPort.u32PortId = vifPort;

            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = vpechn;
            stBindInfo.stDstChnPort.u32PortId = i;

            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = pstVifPortAttr->eBindType;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
    }

    return MI_SUCCESS;
}
MI_S32 ST_SysModuleInit()
{
    ST_Sys_Init();
    return MI_SUCCESS;
}
MI_S32 ST_BaseModuleInit()
{
    MI_U8 eSnrPad = 0;
    MI_U32 u32VifDev = 0;
    MI_VPE_CHANNEL vpechn = 0;

    STCHECKRESULT(ST_SysModuleInit());

    for(eSnrPad=0; eSnrPad<ST_MAX_SENSOR_NUM; eSnrPad++)
    {
        ST_Sensor_Attr_t *pstSensorAttr = &gstSensorAttr[eSnrPad];
        if(pstSensorAttr->bUsed == TRUE && pstSensorAttr->bCreate == FALSE)
        {
            pstSensorAttr->bCreate = TRUE;
            STCHECKRESULT(ST_SensorModuleInit((MI_SNR_PAD_ID_e)eSnrPad));
        }
    }

    for(u32VifDev=0; u32VifDev<ST_MAX_VIF_DEV_NUM; u32VifDev++)
    {
        STCHECKRESULT(ST_VifModuleInit(u32VifDev));
    }

    for(vpechn=0; vpechn<ST_MAX_VPECHN_NUM; vpechn++)
    {
        ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[vpechn];
        if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == FALSE)
        {
            pstVpeChnattr->bCreate = TRUE;
            STCHECKRESULT(ST_VpeModuleInit(vpechn));
        }
    }

    return MI_SUCCESS;
}

MI_S32 ST_VpeModuleUnInit(MI_VPE_CHANNEL Vpechn)
{
    MI_VPE_CHANNEL vpechn = Vpechn;
    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[vpechn];
    ST_DivpAttr_t *pstDivpChnAttr = &gstDivpAttr;
    MI_DIVP_CHN DivpChn = 0;
    MI_U32 i = 0;
    ST_Sys_BindInfo_T stBindInfo;

    if(pstDivpChnAttr->stDivpPort.bUsed == TRUE)
    {
        /************************************************
            Step1:  unbind Vpe Port7->DIVP
        *************************************************/
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = vpechn;
        stBindInfo.stSrcChnPort.u32PortId = ST_VPE_VIR_PORTID;

        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
        stBindInfo.stDstChnPort.u32DevId = 0;
        stBindInfo.stDstChnPort.u32ChnId = DivpChn;
        stBindInfo.stDstChnPort.u32PortId = 0;

        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

        ST_DestroyDivpChn(DivpChn);
    }
    for(i=0; i<ST_MAX_VPE_INPORT_NUM; i++)
    {
        MI_VIF_CHN vifChn = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32ChnId;;
        MI_VIF_PORT vifPort = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32PortId;
        MI_VIF_DEV vifDev = pstVpeChnattr->stInputPortAttr[i].stBindParam.stChnPort.u32DevId;;
        MI_U32 vifChnPerDev = vifChn%ST_MAX_VIF_CHN_PERDEV;

        ST_VifDevAttr_t *pstVifDevAttr = &gstVifModule.stVifDevAttr[vifDev];
        ST_VifPortAttr_t *pstVifPortAttr = &pstVifDevAttr->stVifChnAttr[vifChnPerDev].stVifOutPortAttr[vifPort];

        if(pstVpeChnattr->stInputPortAttr[i].bUsed ==TRUE
            && pstVifPortAttr->bUsed == TRUE
)
        {
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
            stBindInfo.stSrcChnPort.u32DevId = vifDev;
            stBindInfo.stSrcChnPort.u32ChnId = vifChn;
            stBindInfo.stSrcChnPort.u32PortId = vifPort;

            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = vpechn;
            stBindInfo.stDstChnPort.u32PortId = i;

            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
        }
    }
    /************************************************
    Step2:  destory VPE
    *************************************************/
    for(i = 0; i < ST_MAX_PORT_NUM; i ++)
    {
        if(pstVpeChnattr->stVpePortAttr[i].bUsed == TRUE)
        {
            if(gbPreviewByVenc == TRUE)
            {
                MI_U32 u32DevId = -1;
                MI_VENC_CHN vencChn= pstVpeChnattr->stVpePortAttr[i].u32BindVencChan;
                ST_Sys_BindInfo_T stBindInfo;

                ST_VencAttr_t *pstStreamAttr = &gstVencattr[vencChn];
                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));

                ExecFunc(MI_VENC_GetChnDevid(vencChn, &u32DevId), MI_SUCCESS);
                if(i != ST_VPE_VIR_PORTID)
                {
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr->u32BindVpeChn;
                    stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr->u32BindVpePort;
                }
                else
                {
                    MI_DIVP_CHN DivpChn =0;
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = DivpChn;
                    stBindInfo.stSrcChnPort.u32PortId = 0;
                }

                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
                stBindInfo.stDstChnPort.u32DevId = u32DevId;
                stBindInfo.stDstChnPort.u32ChnId = vencChn;
                stBindInfo.stDstChnPort.u32PortId = 0;

                stBindInfo.u32SrcFrmrate = 30;
                stBindInfo.u32DstFrmrate = 30;
                STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
                STCHECKRESULT(ST_VencStop(vencChn));
            }

            STCHECKRESULT(MI_VPE_DisablePort(vpechn, i));
        }
    }
    STCHECKRESULT(MI_VPE_StopChannel(vpechn));
    STCHECKRESULT(MI_VPE_DestroyChannel(vpechn));

    return MI_SUCCESS;
}

MI_S32 ST_VifModuleUnInit(MI_VIF_DEV VifDev)
{
    MI_VIF_DEV vifDev = VifDev;
    MI_VIF_CHN vifChn = 0;
    MI_VIF_CHN vifchnPerDev =0;
    MI_VIF_PORT vifPort=0;
    for(vifchnPerDev=0; vifchnPerDev< ST_MAX_VIF_CHN_PERDEV; vifchnPerDev++)
    {
        for(vifPort=0; vifPort< ST_MAX_VIF_OUTPORT_NUM; vifPort++)
        {
            ST_VifPortAttr_t *pstVifChnPortAttr = &gstVifModule.stVifDevAttr[vifDev].stVifChnAttr[vifchnPerDev].stVifOutPortAttr[vifPort];
            if(pstVifChnPortAttr->bUsed == TRUE
                && pstVifChnPortAttr->bCreate== TRUE
                )
            {
                vifChn = VifDev*ST_MAX_VIF_CHN_PERDEV+vifchnPerDev;

                MI_VIF_DisableChnPort(vifChn, vifPort);
                pstVifChnPortAttr->bCreate = FALSE;
            }
        }
        gstVifModule.stVifDevAttr[vifDev].stVifChnAttr[vifchnPerDev].bCreate=FALSE;
    }

    if(gstVifModule.stVifDevAttr[vifDev].bCreate == TRUE)
    {
        STCHECKRESULT(MI_VIF_DisableDev(vifDev));
        gstVifModule.stVifDevAttr[vifDev].bCreate = FALSE;
    }

    return MI_SUCCESS;
}
MI_S32 ST_SensorModuleUnInit(MI_SNR_PAD_ID_e eSnrPad)
{
    MI_SNR_PAD_ID_e eSnrPadId = eSnrPad;

   STCHECKRESULT(MI_SNR_Disable(eSnrPadId));

    return MI_SUCCESS;
}
MI_S32 ST_SysModuleUnInit()
{
    MI_SYS_Exit();
    return MI_SUCCESS;
}
MI_S32 ST_BaseModuleUnInit()
{
    int i = 0;
    MI_U32 u32VifDev=0;
    MI_U8 eSnrPad = 0;
    
    for(i=0; i<ST_MAX_VPECHN_NUM; i++)
    {
        MI_VPE_CHANNEL vpechn = i;
        ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[vpechn];
        if(pstVpeChnattr->bUsed == TRUE)
        {
            if(pstVpeChnattr->bCreate == TRUE)
            {
                pstVpeChnattr->bCreate = FALSE;
                STCHECKRESULT(ST_VpeModuleUnInit(vpechn));
            }
        }
    }

    for(u32VifDev=0; u32VifDev<ST_MAX_VIF_DEV_NUM; u32VifDev++)
    {
        STCHECKRESULT(ST_VifModuleUnInit(u32VifDev));
    }

    for(eSnrPad=0; eSnrPad<ST_MAX_SENSOR_NUM; eSnrPad++)
    {
        ST_Sensor_Attr_t *pstSensorAttr = &gstSensorAttr[eSnrPad];
        if(pstSensorAttr->bUsed == TRUE && pstSensorAttr->bCreate == TRUE)
        {
            pstSensorAttr->bCreate = FALSE;
            STCHECKRESULT(ST_SensorModuleUnInit((MI_SNR_PAD_ID_e)eSnrPad));
        }
    }

    STCHECKRESULT(ST_SysModuleUnInit());
    return MI_SUCCESS;
}

MI_S32 ST_ParserIni(char *pIniPath)

{
    MI_SNR_PAD_ID_e eSnrPad = E_MI_SNR_PAD_ID_0;
    ST_Sensor_Attr_t *pstSensorAttr = NULL;
    ST_VifDevAttr_t *pstVifDevAttr = NULL;
    ST_VpeChannelAttr_t *pstVpeChnAttr = NULL;
    MI_S8   s8VpeInputPortId =0;

    ST_PortAttr_t   *pstVpePortAttr = NULL;
    char *string= NULL;
    MI_U8 i=0,j=0;
    MI_U32 u32VifDev = 0;
    MI_S32 u32VpeChn = 0;
    time_t stTime = 0;

    printf("inipath %s \n", pIniPath);

    dictionary *pstDict = iniparser_load(pIniPath);

    if(pstDict == NULL)
        return -1;

    printf("pstDict %p \n", pstDict);
    gbPreviewByVenc = iniparser_getint(pstDict, ":UseVenc", 0);

    eSnrPad = (MI_SNR_PAD_ID_e)iniparser_getint(pstDict, ":SensorPad", 0);
    pstSensorAttr = &gstSensorAttr[eSnrPad];
    pstSensorAttr->u8ResIndex = iniparser_getint(pstDict, ":ResIndex", -1);
    pstSensorAttr->bUsed = iniparser_getint(pstDict, ":UseSensor",1);
    pstSensorAttr->u8Mirror = iniparser_getint(pstDict, ":SensorMirror",0);
    pstSensorAttr->u8Flip = iniparser_getint(pstDict, ":SensorFlip",0);

    u32VifDev = iniparser_getint(pstDict, ":VifDev", 0);
    if(u32VifDev >= ST_MAX_VIF_DEV_NUM)
    {
        printf("VIF dev %d err > max %d \n", u32VifDev, ST_MAX_VIF_DEV_NUM);
        return -1;
    }

    pstVifDevAttr = &gstVifModule.stVifDevAttr[u32VifDev];
    //got vif channel can find the binded SensorPad
    pstVifDevAttr->bUsed = pstSensorAttr->bUsed;
    pstVifDevAttr->eHDRType=(MI_VIF_HDRType_e) iniparser_getint(pstDict, ":HDR", 0);
    MI_VIF_CHN vifChn = iniparser_getint(pstDict, ":vifChn", -1);
    MI_U8 vifChnPerDev = 0;
    MI_VIF_PORT vifPort = iniparser_getint(pstDict, ":vifOutPortId", -1);
    vifChn = (vifChn==-1)?u32VifDev*4: vifChn;
    vifChnPerDev = vifChn%ST_MAX_VIF_CHN_PERDEV;
    vifPort = (vifPort==-1)?0: vifPort;
    if(vifChn>=ST_MAX_VIF_CHN_NUM)
    {
        printf("VIF chn %d  err > max %d \n", vifChn, ST_MAX_VIF_CHN_NUM);
        return -1;
    }

    if(vifPort>= ST_MAX_VIF_OUTPORT_NUM)
    {
        printf("VIF port %d err > max %d \n", vifPort, ST_MAX_VIF_OUTPORT_NUM);
        return -1;
    }

    pstVifDevAttr->stBindSensor.eSensorPadID = (MI_VIF_SNRPad_e)eSnrPad;
    pstVifDevAttr->stBindSensor.u32PlaneID = vifChnPerDev;

    ST_VifChnAttr_t *pstVifChnattr = &pstVifDevAttr->stVifChnAttr[vifChnPerDev];
    ST_VifPortAttr_t *pstVifPort = &pstVifChnattr->stVifOutPortAttr[vifPort];
    pstVifChnattr->bUsed = pstVifDevAttr->bUsed;
    pstVifPort->bUsed = pstVifDevAttr->bUsed;
    string = iniparser_getstring(pstDict, ":RunMode", "ERR");
    if(!strcmp(string, (char *)"RealTime"))
    {
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_RGB_REALTIME;
    }
    else if(!strcmp(string, "FrameMode"))
    {
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
    }
    else if(!strcmp(string, "1Multi")
        || !strcmp(string, "DVR"))
    {
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_1MULTIPLEX;
    }
    else if(!strcmp(string, "2Multi"))
    {
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_2MULTIPLEX;
    }
    else if(!strcmp(string, "4Multi"))
    {
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_4MULTIPLEX;
    }
    else
    {
        printf("runmode parse %s err, please use RealTime/FrameMode/1Multi/2Multi/4Multi. default use RealTime \n", string);
        pstVifDevAttr->eWorkMode = E_MI_VIF_WORK_MODE_RGB_REALTIME;
    }

    if(pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_RGB_REALTIME)
    {
        pstVifPort->eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
    }
    else
    {
        pstVifPort->eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    }

    pstVifPort->stCapRect.u16X = iniparser_getint(pstDict,":vifPortCropX",0);
    pstVifPort->stCapRect.u16Y = iniparser_getint(pstDict,":vifPortCropY",0);
    pstVifPort->stCapRect.u16Width = iniparser_getint(pstDict,":vifPortCropW",0);
    pstVifPort->stCapRect.u16Height = iniparser_getint(pstDict,":vifPortCropH",0);

    pstVifPort->stDestSize.u16Width = iniparser_getint(pstDict,":vifPortW",0);
    pstVifPort->stDestSize.u16Height = iniparser_getint(pstDict,":vifPortH",0);

    pstVifPort->ePixFormat = (MI_SYS_PixelFormat_e)iniparser_getint(pstDict,":vifpixel",0xffff);

    pstSensorAttr->u32BindVifDev = u32VifDev;

    u32VpeChn = iniparser_getint(pstDict,":VpeChn",0xffff);
    u32VpeChn =(u32VpeChn != 0xffff) ? u32VpeChn : u32VifDev;
    pstVpeChnAttr = &gstVpeChnattr[u32VpeChn];
    pstVpeChnAttr->stInputFileAttr.u32Width = iniparser_getint(pstDict, ":FileWidth", 0);
    pstVpeChnAttr->stInputFileAttr.u32Height = iniparser_getint(pstDict, ":FileHeight", 0);
    pstVpeChnAttr->stInputFileAttr.ePixelFormat = (MI_SYS_PixelFormat_e)iniparser_getint(pstDict, ":FileType", 0);
    pstVpeChnAttr->stInputFileAttr.u8VpeInPortId = (MI_SYS_PixelFormat_e)iniparser_getint(pstDict, ":FileInPort", 0);
    pstVpeChnAttr->u8ChnId = u32VpeChn;
    pstVpeChnAttr->bUsed = TRUE;
    s8VpeInputPortId = iniparser_getint(pstDict,":vpeInPortId",-1);
    s8VpeInputPortId = (s8VpeInputPortId == -1)?0: s8VpeInputPortId;
    gbMutiVpeChnNum = iniparser_getint(pstDict,":MutiVpeChnNum",-1);

    //got vpe channel can find the binded vif dev
    pstVpeChnAttr->stInputPortAttr[s8VpeInputPortId].stBindParam.stChnPort.eModId = E_MI_MODULE_ID_VIF;
    pstVpeChnAttr->stInputPortAttr[s8VpeInputPortId].stBindParam.stChnPort.u32DevId = u32VifDev;
    pstVpeChnAttr->stInputPortAttr[s8VpeInputPortId].stBindParam.stChnPort.u32ChnId = vifChn;
    pstVpeChnAttr->stInputPortAttr[s8VpeInputPortId].stBindParam.stChnPort.u32PortId = vifPort;
    pstVpeChnAttr->stInputPortAttr[s8VpeInputPortId].bUsed = TRUE;

    string = iniparser_getstring(pstDict, ":YuvFilePath", "ERR");
    if(!strcmp((const char *)string, (const char *)"NULL"))
    {
        //printf("YuvFilePath NULL \n");
    }
    else if(!strcmp((const char *)string, (const char *)"ERR"))
    {
        printf("kerword YuvFilePath use err \n");
    }
    else
    {
        MI_U16  u16size = strlen(string);
        memcpy(pstVpeChnAttr->stInputFileAttr.InputFilePath, string, u16size);
        printf("pstInputFileAttr:%s \n",pstVpeChnAttr->stInputFileAttr.InputFilePath);
    }

    if(pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_RGB_REALTIME)
    {
        pstVpeChnAttr->eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
    }
    else if(pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_RGB_FRAMEMODE)
    {
        pstVpeChnAttr->eRunningMode = E_MI_VPE_RUN_CAM_MODE;
    }
    else if(pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_1MULTIPLEX
        || pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_2MULTIPLEX
        || pstVifDevAttr->eWorkMode == E_MI_VIF_WORK_MODE_4MULTIPLEX)
    {
        pstVpeChnAttr->eRunningMode = E_MI_VPE_RUN_DVR_MODE;
    }

    pstVpeChnAttr->eHdrType = (MI_VPE_HDRType_e)pstVifDevAttr->eHDRType;
    pstVpeChnAttr->e3DNRLevel = (MI_VPE_3DNR_Level_e)iniparser_getint(pstDict, ":NRLevel", 0);
    pstVpeChnAttr->eVpeRotate = (MI_SYS_Rotate_e)iniparser_getint(pstDict, ":Rotation", 0);
    pstVpeChnAttr->bChnMirror = (MI_SYS_Rotate_e)iniparser_getint(pstDict, ":ChnMirror", 0);
    pstVpeChnAttr->bChnFlip = (MI_SYS_Rotate_e)iniparser_getint(pstDict, ":ChnFlip", 0);
    pstVpeChnAttr->stOrgVpeChnCrop.u16X = iniparser_getint(pstDict, ":ChnCropX", 0);
    pstVpeChnAttr->stOrgVpeChnCrop.u16Y = iniparser_getint(pstDict, ":ChnCropY", 0);
    pstVpeChnAttr->stOrgVpeChnCrop.u16Width = iniparser_getint(pstDict, ":ChnCropW", 0);
    pstVpeChnAttr->stOrgVpeChnCrop.u16Height = iniparser_getint(pstDict, ":ChnCropH", 0);
    pstVpeChnAttr->u32ChnPortMode = iniparser_getint(pstDict, ":ChnPortMode", 0);
    if(E_MI_VPE_HDR_TYPE_OFF== pstVpeChnAttr->eHdrType
      || E_MI_VPE_HDR_TYPE_EMBEDDED == pstVpeChnAttr->eHdrType
      || E_MI_VPE_HDR_TYPE_LI== pstVpeChnAttr->eHdrType)
    {
        pstSensorAttr->bPlaneMode = TRUE;
    }
    string = iniparser_getstring(pstDict, ":LdcBinPath", "ERR");
    if(!strcmp((const char *)string, (const char *)"NULL"))
    {
        pstVpeChnAttr->bEnLdc = FALSE;
    }
    else if(!strcmp((const char *)string, (const char *)"ERR"))
    {
        printf("kerword LdcBinPath use err \n");
    }
    else
    {
        MI_U16  u16size = strlen(string);
        pstVpeChnAttr->bEnLdc = TRUE;
        memcpy(pstVpeChnAttr->LdcCfgbin_Path, string, u16size);
    }
    string = iniparser_getstring(pstDict, ":IqBinPath", "ERR");
    if(!strcmp((const char *)string, (const char *)"NULL"))
    {
        printf("IQ Bin Path NULL \n");
    }
    else if(!strcmp((const char *)string,(const char *)"ERR"))
    {
        printf("kerword IqBinPath use err \n");
    }
    else
    {
        MI_U16 u16size = strlen(string);

        memcpy(pstVpeChnAttr->IqCfgbin_Path,string,u16size);

        printf("IqBinPath:%s \n",pstVpeChnAttr->IqCfgbin_Path);
    }

    for(i=0; i<ST_MAX_PORT_NUM; i++)
    {
        char PortString[128] = {0};
        pstVpePortAttr = &pstVpeChnAttr->stVpePortAttr[i];


        sprintf(PortString, ":port%dUse", i);
        pstVpePortAttr->bUsed = iniparser_getint(pstDict, PortString, 0);
        if(pstVpePortAttr->bUsed == TRUE)
        {
            MI_U16 u16VencChn = u32VencChnNum++;
            if(u16VencChn >= ST_MAX_VENC_NUM)
            {
                printf("u16VencChn=%d > max %d\n",u16VencChn,ST_MAX_VENC_NUM);
                iniparser_freedict(pstDict);
                return 0;
            }
            ST_VencAttr_t *pstVencAttr = &gstVencattr[u16VencChn];
            pstVpePortAttr->u32BindVencChan = u16VencChn;

            sprintf(PortString, ":port%dPortCropX", i);
            pstVpePortAttr->stOrigPortCrop.u16X = iniparser_getint(pstDict, PortString, 0);
            sprintf(PortString, ":port%dPortCropY", i);
            pstVpePortAttr->stOrigPortCrop.u16Y = iniparser_getint(pstDict, PortString, 0);
            sprintf(PortString, ":port%dPortCropW", i);
            pstVpePortAttr->stOrigPortCrop.u16Width = iniparser_getint(pstDict, PortString, 0);
            sprintf(PortString, ":port%dPortCropH", i);
            pstVpePortAttr->stOrigPortCrop.u16Height = iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%dmirror", i);
            pstVpePortAttr->bMirror = iniparser_getint(pstDict, PortString, 0);
            sprintf(PortString, ":port%dflip", i);
            pstVpePortAttr->bFlip = iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%dW", i);
            pstVpePortAttr->stOrigPortSize.u16Width = iniparser_getint(pstDict, PortString, 0);
            sprintf(PortString, ":port%dH", i);
            pstVpePortAttr->stOrigPortSize.u16Height = iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%dPixel", i);
            pstVpePortAttr->ePixelFormat = (MI_SYS_PixelFormat_e)iniparser_getint(pstDict, PortString, 0);

            if(gbPreviewByVenc == TRUE)
            {
                pstVencAttr->bUsed = TRUE;
                sprintf(PortString, ":port%dBindtype", i);
                pstVencAttr->eBindType = (MI_SYS_BindType_e)iniparser_getint(pstDict, PortString, 0);
                sprintf(PortString, ":port%dEncodeType", i);
                pstVencAttr->eType = (MI_VENC_ModType_e)iniparser_getint(pstDict, PortString, 0);

                sprintf(pstVencAttr->szStreamName, "video%d", u16VencChn);
                pstVencAttr->u32BindParam = 0;
                //got venc channel can find the binded vpe channel
                pstVencAttr->u32BindVpeChn = u32VpeChn;

                pstVencAttr->u32BindVpePort = i;
                pstVencAttr->vencChn = u16VencChn;

                if(pstVencAttr->eBindType == E_MI_SYS_BIND_TYPE_REALTIME
                    && pstVencAttr->eType == E_MI_VENC_MODTYPE_JPEGE)
                    pstVpePortAttr->ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
                else
                    pstVpePortAttr->ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            }

            sprintf(PortString, ":port%duserdepth", i);
            pstVpePortAttr->u16UserDepth = (MI_U16)iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%ddepth", i);
            pstVpePortAttr->u16Depth = (MI_U16)iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%dDumpBuffNum", i);
            pstVpePortAttr->s32DumpBuffNum = (MI_S32)iniparser_getint(pstDict, PortString, 0);

            sprintf(PortString, ":port%dOutPutPath", i);
            string = iniparser_getstring(pstDict, PortString, "ERR");

            printf("i %d, string:%s \n",i,string);
            if(!strcmp((const char *)string, (const char *)"NULL"))
            {
                printf("OutPutPath NULL \n");
            }
            else if(!strcmp((const char *)string,(const char *)"ERR"))
            {
                printf("kerword OutPutPath use err \n");
            }
            else
            {
                sprintf(pstVpePortAttr->FilePath, "%s/vpeport%d_%dx%d_pixel%d_%ld.raw", string, i, pstVpePortAttr->stOrigPortSize.u16Width, pstVpePortAttr->stOrigPortSize.u16Height, pstVpePortAttr->ePixelFormat, time(&stTime));
                printf("OutPutFile_Path:%s \n",pstVpePortAttr->FilePath);
            }

            if(pstVpePortAttr->u16Depth ==0)
                pstVpePortAttr->u16Depth =4;

            if(pstVpePortAttr->u16UserDepth ==0 && gbPreviewByVenc == FALSE)
                pstVpePortAttr->u16UserDepth = 1;
        }
    }
    
    if(pstSensorAttr->bUsed == FALSE
        && pstVpeChnAttr->stInputFileAttr.u8VpeInPortId == 0)
    {
        std::string path = pstVpeChnAttr->stInputFileAttr.InputFilePath;
        std::string sub1,sub2;
        MI_U8 tmp1 = path.rfind(".");
        sub1 = path.substr(tmp1);
        const char *format = sub1.data();
    
        if(!strcmp(format, ".yuv"))
        {
            pstVpeChnAttr->eRunningMode = E_MI_VPE_RUN_DVR_MODE;
        }
        else if(!strcmp(format, ".raw"))
        {
            pstVpeChnAttr->eRunningMode = E_MI_VPE_RUN_CAM_MODE;
        }
    
        printf("not use sensor, vpe inputport 0 file %s, set runmode %d \n",pstVpeChnAttr->stInputFileAttr.InputFilePath, pstVpeChnAttr->eRunningMode);
    }

    for(i=1; i<gbMutiVpeChnNum; i++)
    {
        MI_VPE_CHANNEL VpeChn = (MI_VPE_CHANNEL)i;
        memcpy(&gstVpeChnattr[VpeChn], &gstVpeChnattr[u32VpeChn], sizeof(ST_VpeChannelAttr_t));
        for(j=0; j<ST_MAX_PORT_NUM; j++)
        {
            MI_U32 OrigVencChn = gstVpeChnattr[u32VpeChn].stVpePortAttr[j].u32BindVencChan;

            pstVpePortAttr = &gstVpeChnattr[VpeChn].stVpePortAttr[j];
            if(pstVpePortAttr->bUsed == FALSE)
                continue;

            MI_U16 u16VencChn = u32VencChnNum++;
            if(u16VencChn >= ST_MAX_VENC_NUM)
            {
                printf("u16VencChn=%d > max %d\n",u16VencChn,ST_MAX_VENC_NUM);
                return 0;
            }
            pstVpePortAttr->u32BindVencChan = u16VencChn;

            memcpy(&gstVencattr[pstVpePortAttr->u32BindVencChan], &gstVencattr[OrigVencChn], sizeof(ST_VencAttr_t));
            gstVencattr[pstVpePortAttr->u32BindVencChan].vencChn = pstVpePortAttr->u32BindVencChan;
            gstVencattr[pstVpePortAttr->u32BindVencChan].u32BindVpeChn = VpeChn;
            sprintf(gstVencattr[pstVpePortAttr->u32BindVencChan].szStreamName, "video%d", gstVencattr[pstVpePortAttr->u32BindVencChan].vencChn);
        }

        u32ChnNum++;
    }
    iniparser_freedict(pstDict);

    return MI_SUCCESS;
}

MI_BOOL ST_DoGetVifRawData(MI_S32 HDRtype)
{
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    MI_VIF_DEV vifDev = 1;
    MI_VIF_CHN vifChn = 4;

    MI_SNR_GetPadInfo(E_MI_SNR_PAD_ID_0, &stPad0Info);
    MI_SNR_GetPlaneInfo(E_MI_SNR_PAD_ID_0, 0, &stSnrPlane0Info);
    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

    /************************************************
    Step2:  init VIF(for IPC, only one dev)
    *************************************************/
    MI_VIF_Dev2SnrPadMuxCfg_t stVifDevMap[4];
    memset(stVifDevMap, 0xff, sizeof(MI_VIF_Dev2SnrPadMuxCfg_t)*4);

    if(HDRtype > 0)
    {
       /* stVifDevMap[0].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
        stVifDevMap[0].u32PlaneID = 1;
        stVifDevMap[1].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
        stVifDevMap[1].u32PlaneID = 0;
        stVifDevMap[2].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
        stVifDevMap[2].u32PlaneID = 1;*/
        printf("HDR ON not support");
        return 0;
    }
    else
    {
        stVifDevMap[0].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
        stVifDevMap[0].u32PlaneID = 0XFF;
        stVifDevMap[1].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
        stVifDevMap[1].u32PlaneID = 0XFF;
    }
    printf("devmap %p\n", stVifDevMap);
    MI_VIF_SetDev2SnrPadMux(stVifDevMap, 4);

    MI_VIF_DevAttr_t stDevAttr;
    memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

    stDevAttr.eIntfMode = stPad0Info.eIntfMode;
    stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
    stDevAttr.eHDRType = E_MI_VIF_HDR_TYPE_OFF;
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

    ExecFunc(MI_VIF_SetDevAttr(vifDev, &stDevAttr), MI_SUCCESS);
    ExecFunc(MI_VIF_EnableDev(vifDev), MI_SUCCESS);

    ST_VIF_PortInfo_T stVifPortInfoInfo;
    memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
    stVifPortInfoInfo.u32RectX = 0;
    stVifPortInfoInfo.u32RectY = 0;
    stVifPortInfoInfo.u32RectWidth = u32CapWidth;
    stVifPortInfoInfo.u32RectHeight = u32CapHeight;
    stVifPortInfoInfo.u32DestWidth = u32CapWidth;
    stVifPortInfoInfo.u32DestHeight = u32CapHeight;
    stVifPortInfoInfo.eFrameRate = eFrameRate;
    stVifPortInfoInfo.ePixFormat = ePixFormat;//E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_GR;
    STCHECKRESULT(ST_Vif_CreatePort(vifChn, 0, &stVifPortInfoInfo));
    STCHECKRESULT(ST_Vif_StartPort(0, vifChn, 0));
    {
        MI_SYS_ChnPort_t stChnPort;
        MI_SYS_BufInfo_t stBufInfo;
        MI_SYS_BUF_HANDLE hHandle;
        MI_S32 s32WriteCnt = 0;
        FILE *fp = NULL;
        char aName[128];
        struct timeval CurTime;
        MI_U64 u64Time = 0;

        memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
        stChnPort.eModId = E_MI_MODULE_ID_VIF;
        stChnPort.u32DevId = 0;
        stChnPort.u32ChnId = vifChn;
        stChnPort.u32PortId = 0;

        gettimeofday(&CurTime,NULL);
        u64Time = CurTime.tv_sec*1000;

        sprintf(aName, "/mnt/dump_vif4_port0_%dx%d_pts%llu.yuv",u32CapWidth,u32CapHeight, u64Time);

        fp = fopen(aName,"wb");
        if(fp == NULL)
            printf("file open fail\n");

        while (s32WriteCnt < 10)
        {
            if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hHandle))
            {
                int size = stBufInfo.stFrameData.u32BufSize;
                fwrite(stBufInfo.stFrameData.pVirAddr[0], size, 1, fp);
                s32WriteCnt++;
                printf("\t vif(%d) size(%d) get buf cnt (%d)...w(%d)...h(%d)..\n", vifChn, size, s32WriteCnt, stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height);
                MI_SYS_ChnOutputPortPutBuf(hHandle);
            }
            usleep(10*1000);
        }
        fclose(fp);
    }
    STCHECKRESULT(ST_Vif_StopPort(vifChn, 0));
    STCHECKRESULT(ST_Vif_DisableDev(vifDev));

    return MI_SUCCESS;
}



MI_BOOL ST_DoChangeHdrRes(MI_U32 u32ChnNum)
{
    MI_SNR_PAD_ID_e eSnrPad;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_VIF_DEV vifDev = 0;
    MI_VIF_CHN vifChnPerDev = 0;
    MI_VIF_PORT vifPort =0;
    MI_VPE_CHANNEL  VpeChn = 0;
    MI_U8 u8VpeInPortId =0;
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));

    ST_VpeChannelAttr_t *pstVpeChnattr = NULL;
    ST_VifDevAttr_t  *pstVifDevAttr = NULL;
    ST_VifPortAttr_t *pstVifPortAttr = NULL;

    MI_S32 select = 0;

    if(u32ChnNum > 1)
    {
        printf("select Channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("Channel %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    pstVpeChnattr = &gstVpeChnattr[VpeChn];
    vifDev = pstVpeChnattr->stInputPortAttr[u8VpeInPortId].stBindParam.stChnPort.u32DevId;
    vifPort = pstVpeChnattr->stInputPortAttr[u8VpeInPortId].stBindParam.stChnPort.u32PortId;

    pstVifDevAttr = &gstVifModule.stVifDevAttr[vifDev];
    pstVifPortAttr = &gstVifModule.stVifDevAttr[vifDev].stVifChnAttr[vifChnPerDev].stVifOutPortAttr[vifPort];
    eSnrPad = (MI_SNR_PAD_ID_e)pstVifDevAttr->stBindSensor.eSensorPadID;

    /************************************************
    Step1: VPE Stop ==> wait driver all buffer done
    *************************************************/
    STCHECKRESULT(MI_VPE_StopChannel(VpeChn));

    /************************************************
    Step2:  Destory VIF
    *************************************************/
    ST_VifModuleUnInit(vifDev);

    /************************************************
    Step3:  Destory Sensor
    *************************************************/
    STCHECKRESULT(MI_SNR_Disable(eSnrPad));

    /************************************************
    Step4: Choice Hdr Type
    *************************************************/
    printf("Use HDR ?\n 0 not use, 1 use VC, 2 use DOL, 3 use EMBEDDED, 4 use LI\n");
    printf("sony sensor(ex imx307) use DOL, sc sensor(ex sc4238) use VC\n");
    scanf("%d", &select);
    ST_Flush();
    printf("You select %d HDR\n", select);

    if(select == 0)
    {
        pstVpeChnattr->eHdrType = E_MI_VPE_HDR_TYPE_OFF;
    }
    else if(select == 1)
    {
        pstVpeChnattr->eHdrType = E_MI_VPE_HDR_TYPE_VC;
    }
    else if(select == 2)
    {
        pstVpeChnattr->eHdrType = E_MI_VPE_HDR_TYPE_DOL;
    }
    else if(select == 3)
    {
        pstVpeChnattr->eHdrType = E_MI_VPE_HDR_TYPE_EMBEDDED;
    }
    else if(select == 4)
    {
        pstVpeChnattr->eHdrType = E_MI_VPE_HDR_TYPE_LI;
    }
    else
    {
        printf("select hdrtype %d not support \n", select);
        return 0;
    }

    /************************************************
    Step5:  Init Sensor
    *************************************************/
    if(pstVpeChnattr->eHdrType > 0)
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, TRUE));
    }
    else
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, FALSE));
    }

    MI_U8 u8ResIndex =0;
    MI_U32 u32ResCount =0;
    MI_SNR_Res_t stRes;
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    STCHECKRESULT(MI_SNR_QueryResCount(eSnrPad, &u32ResCount));
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

    printf("select res\n");
    scanf("%d", &select);
    ST_Flush();

    if((MI_U32)select >= u32ResCount)
        return 0;

    STCHECKRESULT(MI_SNR_SetRes(eSnrPad,(MI_U8)select));
    STCHECKRESULT(MI_SNR_Enable(eSnrPad));

    STCHECKRESULT(MI_SNR_GetPadInfo(eSnrPad, &stPad0Info));
    STCHECKRESULT(MI_SNR_GetPlaneInfo(eSnrPad, 0, &stSnrPlane0Info));

    /************************************************
    Step6:  init VIF
    *************************************************/
    memset(&pstVifPortAttr->stCapRect, 0x0, sizeof(MI_SYS_WindowRect_t));
    memset(&pstVifPortAttr->stDestSize, 0x0, sizeof(MI_SYS_WindowSize_t));
    pstVifPortAttr->ePixFormat = E_MI_SYS_PIXEL_FRAME_FORMAT_MAX; //use sensor param
    pstVifDevAttr->eHDRType = (MI_VIF_HDRType_e)pstVpeChnattr->eHdrType;
    ST_VifModuleInit(vifDev);

    /************************************************
    Step6: Vpe Start
    *************************************************/
    MI_VPE_ChannelPara_t stVpeChParam;
    memset(&stVpeChParam, 0x0, sizeof(MI_VPE_ChannelPara_t));

    STCHECKRESULT(MI_VPE_GetChannelParam(VpeChn, &stVpeChParam));
    stVpeChParam.eHDRType = pstVpeChnattr->eHdrType;
    STCHECKRESULT(MI_VPE_SetChannelParam(VpeChn, &stVpeChParam));

    STCHECKRESULT(MI_VPE_StartChannel(VpeChn));

    return 0;
}


MI_BOOL ST_DoChangeRotate(MI_U32 u32ChnNum)
{
    MI_S32 s32Rotation = 0;
    MI_S32 s32Mirror = 0;
    MI_S32 s32Flip = 0;
    MI_U8 i=0;
    MI_VIF_CHN vifChn=0;
    MI_VIF_PORT vifPort=0;
    MI_VPE_CHANNEL  VpeChn = 0;
    MI_U32 u32MaxVencWidth =0, u32MaxVencHeight =0;

    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("VpeChn %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
    ST_VencAttr_t *pstVencattr = NULL;
    vifChn = (MI_VIF_CHN)pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32ChnId;

    printf("rotation(0:0, 1:90, 2:180, 3:270):");
    scanf("%d", &s32Rotation);
    ST_Flush();

    printf("bmirror 0: FALSE, 1:TRUE :");
    scanf("%d", &s32Mirror);
    ST_Flush();

    printf("bFlip 0: FALSE, 1:TRUE :");
    scanf("%d", &s32Flip);
    ST_Flush();

    pstVpeChnattr->eVpeRotate = (MI_SYS_Rotate_e)s32Rotation;
    pstVpeChnattr->bChnFlip = s32Flip;
    pstVpeChnattr->bChnMirror = s32Mirror;

    /************************************************
    Step1: Stop Venc (Because rot will change preview resolution)
    *************************************************/
    for(i=0;i<ST_MAX_PORT_NUM;i++)
    {
        MI_U32 u32VencChn = pstVpeChnattr->stVpePortAttr[i].u32BindVencChan;
        pstVencattr = &gstVencattr[u32VencChn];
        ST_PortAttr_t *pstVpePortAttr = &pstVpeChnattr->stVpePortAttr[i];
        if(pstVpePortAttr->bUsed)
        {
            if(gbPreviewByVenc == TRUE)
                ST_VencStop(u32VencChn);
        }
    }

    /************************************************
    Step2: Stop Vpe (Wait driver all buffer done)
    *************************************************/
    STCHECKRESULT(MI_VPE_StopChannel(VpeChn));

    /************************************************
    Step3: Disable Vif Port(Realtime mode Change Rot Will Change Isp Cfg, need stop push vif stream)
    *************************************************/
    if(pstVpeChnattr->eRunningMode == E_MI_VPE_RUN_REALTIME_MODE)
        STCHECKRESULT(MI_VIF_DisableChnPort(vifChn, vifPort));

    /************************************************
    Step4: Set Vpe Rot/ChnMirror/ChnFlip
    *************************************************/
    STCHECKRESULT(MI_VPE_SetChannelRotation(VpeChn, pstVpeChnattr->eVpeRotate));

    MI_VPE_ChannelPara_t stChnParam;
    memset(&stChnParam, 0x0, sizeof(MI_VPE_ChannelPara_t));

    STCHECKRESULT(MI_VPE_GetChannelParam(VpeChn, &stChnParam));
    stChnParam.bMirror = pstVpeChnattr->bChnMirror;
    stChnParam.bFlip = pstVpeChnattr->bChnFlip;
    STCHECKRESULT(MI_VPE_SetChannelParam(VpeChn, &stChnParam));

    /************************************************
    Step5: Rot switch Vpe Port size
    *************************************************/
    for(i=0; i<ST_MAX_PORT_NUM; i++)
    {
        MI_VPE_PortMode_t stVpeMode;
        MI_U32 u32VencChn = pstVpeChnattr->stVpePortAttr[i].u32BindVencChan;
        ST_PortAttr_t *pstVpePortAttr = &pstVpeChnattr->stVpePortAttr[i];
        MI_U32  u32SclSourceHeight = 0;

        memset(&stVpeMode, 0x0, sizeof(MI_VPE_PortMode_t));

        if(pstVpeChnattr->stVpePortAttr[i].bUsed == TRUE)
        {
            STCHECKRESULT(MI_VPE_GetPortMode(VpeChn , i, &stVpeMode));

            if(pstVpeChnattr->u32ChnPortMode ==0
                || pstVpeChnattr->stVpeChnCrop.u16Width ==0
                || pstVpeChnattr->stVpeChnCrop.u16Height ==0)
            {
                MI_VIF_ChnPortAttr_t  stChnAttr;
                memset(&stChnAttr, 0x0, sizeof(MI_VIF_ChnPortAttr_t));
                STCHECKRESULT(MI_VIF_GetChnPortAttr(vifChn, 0,  &stChnAttr));

                u32SclSourceHeight = stChnAttr.stCapRect.u16Height;
            }
            else
            {
                u32SclSourceHeight =  pstVpeChnattr->stOrgVpeChnCrop.u16Height;
            }

            if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
               || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
            {
                if(pstVpePortAttr->stOrigPortCrop.u16Height !=0)
                    pstVpePortAttr->stPortCrop.u16X = u32SclSourceHeight - pstVpePortAttr->stOrigPortCrop.u16Y - pstVpePortAttr->stOrigPortCrop.u16Height;
                else
                    pstVpePortAttr->stPortCrop.u16X = 0;

                pstVpePortAttr->stPortCrop.u16Y = pstVpePortAttr->stOrigPortCrop.u16X;
                pstVpePortAttr->stPortCrop.u16Width = pstVpePortAttr->stOrigPortCrop.u16Height;
                pstVpePortAttr->stPortCrop.u16Height = pstVpePortAttr->stOrigPortCrop.u16Width;

                pstVpePortAttr->stPortSize.u16Width = pstVpePortAttr->stOrigPortSize.u16Height;
                pstVpePortAttr->stPortSize.u16Height = pstVpePortAttr->stOrigPortSize.u16Width;

                u32MaxVencWidth = u16MaxH;
                u32MaxVencHeight = u16MaxW;
            }
            else
            {
                pstVpePortAttr->stPortCrop.u16X = pstVpePortAttr->stOrigPortCrop.u16X;
                pstVpePortAttr->stPortCrop.u16Y = pstVpePortAttr->stOrigPortCrop.u16Y;
                pstVpePortAttr->stPortCrop.u16Width = pstVpePortAttr->stOrigPortCrop.u16Width;
                pstVpePortAttr->stPortCrop.u16Height = pstVpePortAttr->stOrigPortCrop.u16Height;

                pstVpePortAttr->stPortSize.u16Width = pstVpePortAttr->stOrigPortSize.u16Width;
                pstVpePortAttr->stPortSize.u16Height = pstVpePortAttr->stOrigPortSize.u16Height;

                u32MaxVencWidth = u16MaxW;
                u32MaxVencHeight = u16MaxH;
            }
            if(i < ST_MAX_SCL_NUM)
            {
                STCHECKRESULT(MI_VPE_SetPortCrop(VpeChn, i, &pstVpePortAttr->stPortCrop));
            }
            stVpeMode.u16Width = pstVpePortAttr->stPortSize.u16Width;
            stVpeMode.u16Height = pstVpePortAttr->stPortSize.u16Height;
            STCHECKRESULT(MI_VPE_SetPortMode(VpeChn , i, &stVpeMode));

            pstVencattr[u32VencChn].u32Width = stVpeMode.u16Width;
            pstVencattr[u32VencChn].u32Height = stVpeMode.u16Height;
        }
    }

    /************************************************
    Step7: Start Vif
    *************************************************/
    if(pstVpeChnattr->eRunningMode == E_MI_VPE_RUN_REALTIME_MODE)
        STCHECKRESULT(MI_VIF_EnableChnPort(vifChn, vifPort));

    /************************************************
    Step8: Start Vpe
    *************************************************/
    STCHECKRESULT(MI_VPE_StartChannel (VpeChn));

    /************************************************
    Step9: Start Venc
    *************************************************/
    for(i=0;i<ST_MAX_PORT_NUM;i++)
    {
        MI_U32 u32VencChn = pstVpeChnattr->stVpePortAttr[i].u32BindVencChan;
        ST_PortAttr_t *pstVpePortAttr = &pstVpeChnattr->stVpePortAttr[i];
        if(pstVpePortAttr->bUsed)
        {
            if(gbPreviewByVenc == TRUE)
                ST_VencStart(u32MaxVencWidth, u32MaxVencHeight, u32VencChn);
        }
    }

    return 0;
}

MI_BOOL ST_ChangePortMode(MI_U32 u32ChnNum)
{
    MI_S32 s32Portid = 0;
    MI_S32 s32PortPixelFormat =0;
    MI_S32 s32PortMirror=0, s32PortFlip=0;
    MI_S32 s32PortW=0, s32PortH=0;
    MI_VPE_PortMode_t  stVpePortMode;
    memset(&stVpePortMode, 0x0, sizeof(MI_VPE_PortMode_t));

    MI_U32  VpeChn =0;

    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("VpeChn %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    printf("select port id:");
    scanf("%d", &s32Portid);
    ST_Flush();

    if(s32Portid >= ST_MAX_PORT_NUM)
    {
        printf("port %d, not valid \n", s32Portid);
        return 0;
    }

    printf("port %d bmirror:", s32Portid);
    scanf("%d", &s32PortMirror);
    ST_Flush();

    printf("port %d bflip:", s32Portid);
    scanf("%d", &s32PortFlip);
    ST_Flush();

    printf("port %d port width:", s32Portid);
    scanf("%d", &s32PortW);
    ST_Flush();

    printf("port %d port height:", s32Portid);
    scanf("%d", &s32PortH);
    ST_Flush();

    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
    ST_PortAttr_t *pstVpePortAttr = &pstVpeChnattr->stVpePortAttr[s32Portid];
    MI_U32 U32VencChn = pstVpePortAttr->u32BindVencChan;
    ST_VencAttr_t *pstVencattr = &gstVencattr[U32VencChn];
    STCHECKRESULT(MI_VPE_GetPortMode(VpeChn, s32Portid, &stVpePortMode));

    if(gbPreviewByVenc == FALSE)
    {
        printf("port %d port pixel:", s32Portid);
        printf("yuv422:0, argb8888:1, abgr8888:2, bgra8888:3, yuv420:11\n");
        scanf("%d", &s32PortPixelFormat);
        ST_Flush();
    }
    else
        s32PortPixelFormat = stVpePortMode.ePixelFormat;

    /************************************************
    Step1: Stop Venc (Because rot will change preview resolution)
    *************************************************/
    if(gbPreviewByVenc == TRUE)
    {
        MI_U8 VencChn = pstVencattr->vencChn;
        MI_U32 u32DevId = 0;
        ST_Sys_BindInfo_T stBindInfo;

        ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);

        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = pstVencattr->u32BindVpeChn;
        stBindInfo.stSrcChnPort.u32PortId = pstVencattr->u32BindVpePort;

        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
        stBindInfo.stDstChnPort.u32DevId = u32DevId;
        stBindInfo.stDstChnPort.u32ChnId = VencChn;
        stBindInfo.stDstChnPort.u32PortId = 0;

        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

        if(pstVencattr->bUsed == TRUE)
            ExecFunc(MI_VENC_StopRecvPic(pstVencattr->vencChn), MI_SUCCESS);
    }

    /************************************************
    Step2: disable Vpe port
    *************************************************/
    STCHECKRESULT(MI_VPE_DisablePort(VpeChn, s32Portid));

    /************************************************
    Step3: Set Port Mode
    *************************************************/
    pstVpePortAttr->bMirror = s32PortMirror;
    pstVpePortAttr->bFlip = s32PortFlip;
    pstVpePortAttr->stOrigPortSize.u16Width = s32PortW;
    pstVpePortAttr->stOrigPortSize.u16Height = s32PortH;
    pstVpePortAttr->ePixelFormat = (MI_SYS_PixelFormat_e)s32PortPixelFormat;
    pstVpePortAttr->bUsed = TRUE;

    stVpePortMode.bMirror = pstVpePortAttr->bMirror;
    stVpePortMode.bFlip = pstVpePortAttr->bFlip;
    stVpePortMode.ePixelFormat = pstVpePortAttr->ePixelFormat;
    if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
        || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
    {
        stVpePortMode.u16Width = pstVpePortAttr->stOrigPortSize.u16Height;
        stVpePortMode.u16Height = pstVpePortAttr->stOrigPortSize.u16Width;
    }
    else
    {
        stVpePortMode.u16Width = pstVpePortAttr->stOrigPortSize.u16Width;
        stVpePortMode.u16Height = pstVpePortAttr->stOrigPortSize.u16Height;
    }

    STCHECKRESULT(MI_VPE_SetPortMode(VpeChn, s32Portid, &stVpePortMode));

    /************************************************
    Step4: Set Venc Channel Pitch
    *************************************************/
    if(gbPreviewByVenc == TRUE)
    {
        MI_VENC_ChnAttr_t stChnAttr;
        memset(&stChnAttr, 0x0, sizeof(MI_VENC_ChnAttr_t));

        pstVencattr->u32Width = stVpePortMode.u16Width;
        pstVencattr->u32Height = stVpePortMode.u16Height;

        if(pstVencattr->bUsed == TRUE)
        {
            ExecFunc(MI_VENC_GetChnAttr(pstVencattr->vencChn, &stChnAttr), MI_SUCCESS);
            if(pstVencattr->eType == E_MI_VENC_MODTYPE_H264E)
            {
                stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = stVpePortMode.u16Width;
                stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = stVpePortMode.u16Height;
            }
            else if(pstVencattr->eType == E_MI_VENC_MODTYPE_H264E)
            {
                stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = stVpePortMode.u16Width;
                stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = stVpePortMode.u16Height;
            }
            else if(pstVencattr->eType == E_MI_VENC_MODTYPE_H264E)
            {
                stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = stVpePortMode.u16Width;
                stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = stVpePortMode.u16Height;
            }
            ExecFunc(MI_VENC_SetChnAttr(pstVencattr->vencChn, &stChnAttr), MI_SUCCESS);

            if(pstVencattr->eBindType == E_MI_SYS_BIND_TYPE_HW_RING)
            {
                pstVencattr->u32BindParam = pstVencattr->u32Height;
            }
        }
        else
            printf("port %d, venc buse %d \n", s32Portid, pstVencattr->bUsed);

		printf(">>>>>>>pstVencattr->u32Height=%d,pstVencattr->u32BindParam=%d,pstVencattr->eType =%d\n",pstVencattr->u32Height,pstVencattr->u32BindParam,pstVencattr->eType );
    }

    /************************************************
    Step5: Enable Vpe Port
    *************************************************/
    STCHECKRESULT(MI_VPE_EnablePort(VpeChn, s32Portid));

    /************************************************
    Step6: Start Venc
    *************************************************/
    if(gbPreviewByVenc == TRUE)
    {
        ST_Sys_BindInfo_T stBindInfo;
        MI_U8 VencChn = pstVencattr->vencChn;
        MI_U32 u32DevId = 0;

        if(pstVencattr->bUsed == TRUE)
            ExecFunc(MI_VENC_StartRecvPic(pstVencattr->vencChn), MI_SUCCESS);

        ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);

        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = pstVencattr->u32BindVpeChn;
        stBindInfo.stSrcChnPort.u32PortId = pstVencattr->u32BindVpePort;

        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
        stBindInfo.stDstChnPort.u32DevId = u32DevId;
        stBindInfo.stDstChnPort.u32ChnId = VencChn;
        stBindInfo.stDstChnPort.u32PortId = 0;

        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        stBindInfo.eBindType = pstVencattr->eBindType;
        stBindInfo.u32BindParam = pstVencattr->u32BindParam;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    }

    return 0;

}

MI_BOOL ST_DoSetChnCrop(MI_U32 u32ChnNum)
{
    MI_S32 s32ChannelCropX =0, s32ChannelCropY=0,s32ChannelCropW=0,s32ChannelCropH =0;
    MI_SYS_WindowRect_t stVpeChnCrop;
    memset(&stVpeChnCrop, 0x0, sizeof(MI_SYS_WindowRect_t));

    MI_U32  VpeChn =0;

    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("VpeChn %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];

    printf("Channel Crop x:");
    scanf("%d", &s32ChannelCropX);
    ST_Flush();

    printf("Channel Crop y:");
    scanf("%d", &s32ChannelCropY);
    ST_Flush();

    printf("Channel Crop width:");
    scanf("%d", &s32ChannelCropW);
    ST_Flush();

    printf("Channel Crop height:");
    scanf("%d", &s32ChannelCropH);
    ST_Flush();

    pstVpeChnattr->stOrgVpeChnCrop.u16X = s32ChannelCropX;
    pstVpeChnattr->stOrgVpeChnCrop.u16Y = s32ChannelCropY;
    pstVpeChnattr->stOrgVpeChnCrop.u16Width = s32ChannelCropW;
    pstVpeChnattr->stOrgVpeChnCrop.u16Height = s32ChannelCropH;

    if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
        || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
    {
        stVpeChnCrop.u16X = pstVpeChnattr->stOrgVpeChnCrop.u16Y;
        stVpeChnCrop.u16Y = pstVpeChnattr->stOrgVpeChnCrop.u16X;
        stVpeChnCrop.u16Width = pstVpeChnattr->stOrgVpeChnCrop.u16Height;
        stVpeChnCrop.u16Height = pstVpeChnattr->stOrgVpeChnCrop.u16Width;
    }
    else
    {
        stVpeChnCrop.u16X = pstVpeChnattr->stOrgVpeChnCrop.u16X;
        stVpeChnCrop.u16Y = pstVpeChnattr->stOrgVpeChnCrop.u16Y;
        stVpeChnCrop.u16Width = pstVpeChnattr->stOrgVpeChnCrop.u16Width;
        stVpeChnCrop.u16Height = pstVpeChnattr->stOrgVpeChnCrop.u16Height;
    }

    STCHECKRESULT(MI_VPE_SetChannelCrop(VpeChn,&stVpeChnCrop));

    return 0;
}

MI_BOOL ST_DoSetChnZoom(MI_U32 u32ChnNum)
{
    float r = 16.0/9;
    MI_U16 ystep = 8;
    MI_U16 xstep =ALIGN_UP((MI_U16)(r*ystep), 2);
    int oriW = 0, oriH = 0;
    MI_SYS_WindowRect_t stCropInfo;
    MI_VIF_ChnPortAttr_t stVifPortInfo;
    MI_U32 u32SleepTimeUs = 0;
    MI_U32 u32Fps =0;
    MI_S32 s32ZoomPosition = 0;
    MI_S32 s32PortZoom = 0;
    MI_BOOL bZoomDone = TRUE;
    memset(&stVifPortInfo, 0x0, sizeof(MI_VIF_ChnPortAttr_t));
    memset(&stCropInfo, 0, sizeof(MI_SYS_WindowRect_t));

    MI_VPE_CHANNEL  VpeChn = 0;
    MI_U8 u8VpeInPortId =0;
    MI_VIF_CHN  VifChn = 0;
    MI_VIF_CHN  VifPort = 0;
    MI_VIF_DEV  VifDev = 0;
    MI_SNR_PAD_ID_e eSnrPad = E_MI_SNR_PAD_ID_0;
    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("VpeChn %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }
    ST_VpeChannelAttr_t *pstVpeChnAttr = &gstVpeChnattr[VpeChn];
    VifChn = pstVpeChnAttr->stInputPortAttr[u8VpeInPortId].stBindParam.stChnPort.u32ChnId;
    VifDev = pstVpeChnAttr->stInputPortAttr[u8VpeInPortId].stBindParam.stChnPort.u32DevId;
    VifPort = pstVpeChnAttr->stInputPortAttr[u8VpeInPortId].stBindParam.stChnPort.u32PortId;
    ST_VifDevAttr_t *pstVifChnAttr = &gstVifModule.stVifDevAttr[VifDev];
    eSnrPad = (MI_SNR_PAD_ID_e)pstVifChnAttr->stBindSensor.eSensorPadID;
    ST_Sensor_Attr_t *pstSensorAttr = &gstSensorAttr[eSnrPad];

    MI_VIF_GetChnPortAttr(VifChn,0,&stVifPortInfo);
    oriW = stVifPortInfo.stCapRect.u16Width;
    oriH = stVifPortInfo.stCapRect.u16Height;
    if(pstSensorAttr->bUsed == false)
    {
        oriW = pstVpeChnAttr->stInputFileAttr.u32Width;
        oriH = pstVpeChnAttr->stInputFileAttr.u32Height;
    }
    MI_SNR_GetFps(eSnrPad, &u32Fps);

    u32SleepTimeUs = 1000000/u32Fps;
    printf("fps %d, sleeptime %d \n", u32Fps, u32SleepTimeUs);

    printf("set zoom position: 1.vif, 2.vpe isp dma, 3.vpe scl pre-crop");
    scanf("%d", &s32ZoomPosition);
    ST_Flush();

    if(s32ZoomPosition == 3)
    {
        printf("select which port zoom: 0:port0, 1:port1, 2:port2, 3: all port \n");
        scanf("%d", &s32PortZoom);
        ST_Flush();
    }

    while(1)
    {
        if(bZoomDone == TRUE)
        {
            stCropInfo.u16X += xstep;
            stCropInfo.u16Y += ystep;
            stCropInfo.u16Width = oriW - (2 * stCropInfo.u16X);
            stCropInfo.u16Height = oriH -(2 * stCropInfo.u16Y);

            stCropInfo.u16Width = ALIGN_UP(stCropInfo.u16Width, 2);
            stCropInfo.u16Height = ALIGN_UP(stCropInfo.u16Height, 2);

            if(stCropInfo.u16Width < 660 || stCropInfo.u16Height < 360)
            {
                bZoomDone = FALSE;
            }
        }
        else
        {
            stCropInfo.u16X -= xstep;
            stCropInfo.u16Y -= ystep;
            stCropInfo.u16Width = oriW - (2 * stCropInfo.u16X);
            stCropInfo.u16Height = oriH -(2 * stCropInfo.u16Y);

            stCropInfo.u16Width = ALIGN_UP(stCropInfo.u16Width, 2);
            stCropInfo.u16Height = ALIGN_UP(stCropInfo.u16Height, 2);

            if(stCropInfo.u16Width > oriW || stCropInfo.u16Height > oriH)
            {
                break;
            }
        }

        if(s32ZoomPosition == 1)
        {
            MI_VIF_ChnPortAttr_t stChnPortAttr;
            ExecFunc(MI_VIF_GetChnPortAttr(VifChn, VifPort, &stChnPortAttr), MI_SUCCESS);

            memcpy(&stChnPortAttr.stCapRect, &stCropInfo, sizeof(MI_SYS_WindowRect_t));

            stChnPortAttr.stDestSize.u16Width = stCropInfo.u16Width;
            stChnPortAttr.stDestSize.u16Height = stCropInfo.u16Height;

            ExecFunc(MI_VIF_SetChnPortAttr(VifChn, VifPort, &stChnPortAttr), MI_SUCCESS);
        }
        else if(s32ZoomPosition == 2)
        {
            STCHECKRESULT(MI_VPE_SetChannelCrop(VpeChn,  &stCropInfo));
            STCHECKRESULT(MI_VPE_GetChannelCrop(VpeChn,  &stCropInfo));
        }
        else if(s32ZoomPosition == 3)
        {
            if(s32PortZoom == 3)
            {
                MI_VPE_SetPortCrop(VpeChn, 0, &stCropInfo);
                MI_VPE_SetPortCrop(VpeChn, 1, &stCropInfo);
                MI_VPE_SetPortCrop(VpeChn, 2, &stCropInfo);
            }
            else
                MI_VPE_SetPortCrop(VpeChn, s32PortZoom, &stCropInfo);
        }
        printf("after crop down x:%d y:%d w:%d h:%d\n", stCropInfo.u16X, stCropInfo.u16Y, stCropInfo.u16Width, stCropInfo.u16Height);

        //ST_Flush();

        usleep(u32SleepTimeUs);
    }

    return 0;
}


MI_BOOL ST_DoSetPortCrop(MI_U32 u32ChnNum)
{
    MI_S32 s32Portid = 0;
    MI_S32 s32PortCropX =0, s32PortCropY=0,s32PortCropW=0,s32PortCropH =0;
    MI_SYS_WindowRect_t stPortCropSize;
    memset(&stPortCropSize, 0x0, sizeof(MI_SYS_WindowRect_t));
    MI_U32  VpeChn =0;

    if(u32ChnNum > 1)
    {
        printf("select channel id:");
        scanf("%d", &VpeChn);
        ST_Flush();

    }
    else
    {
        for(VpeChn=0; VpeChn<ST_MAX_VPECHN_NUM; VpeChn++)
        {
            ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
            if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
            {
                printf("use vpe chn %d \n", VpeChn);
                break;
            }
        }
    }
    if(VpeChn >= ST_MAX_VPECHN_NUM)
    {
        printf("VpeChn %d > max %d \n", VpeChn, ST_MAX_VPECHN_NUM);
        return 0;
    }

    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[VpeChn];
    ST_PortAttr_t *pstVpePortAttr = pstVpeChnattr->stVpePortAttr;

    printf("select port id:");
    scanf("%d", &s32Portid);
    ST_Flush();

    if(s32Portid >= ST_MAX_PORT_NUM || pstVpePortAttr[s32Portid].bUsed != TRUE)
    {
        printf("port %d, not valid \n", s32Portid);
        return 0;
    }

     printf("port %d port crop x:", s32Portid);
    scanf("%d", &s32PortCropX);
    ST_Flush();

    printf("port %d port crop y:", s32Portid);
    scanf("%d", &s32PortCropY);
    ST_Flush();

    printf("port %d port crop width:", s32Portid);
    scanf("%d", &s32PortCropW);
    ST_Flush();

    printf("port %d port crop height:", s32Portid);
    scanf("%d", &s32PortCropH);
    ST_Flush();

    pstVpePortAttr[s32Portid].stOrigPortCrop.u16X = s32PortCropX;
    pstVpePortAttr[s32Portid].stOrigPortCrop.u16Y = s32PortCropY;
    pstVpePortAttr[s32Portid].stOrigPortCrop.u16Width = s32PortCropW;
    pstVpePortAttr[s32Portid].stOrigPortCrop.u16Height = s32PortCropH;

    if(pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_90
        || pstVpeChnattr->eVpeRotate == E_MI_SYS_ROTATE_270)
    {
        stPortCropSize.u16X = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Y;
        stPortCropSize.u16Y = pstVpePortAttr[s32Portid].stOrigPortCrop.u16X;
        stPortCropSize.u16Width = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Height;
        stPortCropSize.u16Height = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Width;
    }
    else
    {
        stPortCropSize.u16X = pstVpePortAttr[s32Portid].stOrigPortCrop.u16X;
        stPortCropSize.u16Y = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Y;
        stPortCropSize.u16Width = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Width;
        stPortCropSize.u16Height = pstVpePortAttr[s32Portid].stOrigPortCrop.u16Height;
    }

    STCHECKRESULT(MI_VPE_SetPortCrop(VpeChn , s32Portid, &stPortCropSize));

    return 0;
}
MI_BOOL ST_DoSetIqBin(MI_VPE_CHANNEL Vpechn,char *pConfigPath)
{
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    MI_U8  u8ispreadycnt = 0;
    if (strlen(pConfigPath) == 0)
    {
        printf("IQ Bin File path NULL!\n");
        return FALSE;
    }

    do
    {
        if(u8ispreadycnt > 100)
        {
            printf("%s:%d, isp ready time out \n", __FUNCTION__, __LINE__);
            u8ispreadycnt = 0;
            break;
        }

        MI_ISP_IQ_GetParaInitStatus(Vpechn, &status);
        if(status.stParaAPI.bFlag != 1)
        {
            usleep(300*1000);
            u8ispreadycnt++;
            continue;
        }

        u8ispreadycnt = 0;

        printf("loading api bin...path:%s\n",pConfigPath);
        MI_ISP_API_CmdLoadBinFile(Vpechn, (char *)pConfigPath, 1234);

        usleep(10*1000);
    }while(!status.stParaAPI.bFlag);

    return 0;
}

int ST_InitParam()
{
    gstSensorAttr[E_MI_SNR_PAD_ID_0].u32BindVifDev = 0;
    gstSensorAttr[E_MI_SNR_PAD_ID_1].u32BindVifDev = 2;
    gstSensorAttr[E_MI_SNR_PAD_ID_2].u32BindVifDev = 1;

    return 0;
}

int main(int argc, char **argv)
{
    MI_U8  i = 0, j=0, size = 0;
    char sFilePath[128];
    MI_VPE_CHANNEL Vpechn=0;

    ST_InitParam();

    for(i=1; i< argc; i++)
    {
        if(MI_SUCCESS != ST_ParserIni(argv[i]))
        {
            printf("parse init fail \n");
            return 0;
        }
        u32ChnNum++;
    }

    STCHECKRESULT(ST_BaseModuleInit());

    ST_RtspServerStart();

#if 0
    pthread_t pIQthread;
    pthread_create(&pIQthread, NULL, ST_IQthread, NULL);
#endif

    for(i=0;i<ST_MAX_VPECHN_NUM;i++)
    {
        if(gstVpeChnattr[i].bUsed == FALSE)
            continue;

        size = strlen(gstVpeChnattr[i].stInputFileAttr.InputFilePath);
         if(0 != size)
        {
            printf("channel %d create putVpeDataThread\n",i);
            MI_VPE_StopChannel(gstVpeChnattr[i].u8ChnId);
            pthread_create(& gstVpeChnattr[i].stInputFileAttr.pPutDatathread, NULL, ST_PutVpeInputDataThread,(void*)& gstVpeChnattr[i] );
            usleep(10*1000);
            MI_VPE_StartChannel(gstVpeChnattr[i].u8ChnId);
        }
    }

    MI_U8 u8PortId[ST_MAX_VPECHN_NUM*ST_MAX_PORT_NUM];

    for(j=0; j< ST_MAX_VPECHN_NUM; j++)
    {
        for(i=0; i< ST_MAX_PORT_NUM; i++)
        {
            if(gstVpeChnattr[j].bUsed == FALSE || gstVpeChnattr[j].stVpePortAttr[i].bUsed == FALSE)
                continue;
            u8PortId[j*ST_MAX_PORT_NUM+i] = j*ST_MAX_PORT_NUM+i;
            pthread_mutex_init(&gstVpeChnattr[j].stVpePortAttr[i].Portmutex, NULL);
            pthread_create(&gstVpeChnattr[j].stVpePortAttr[i].pGetDatathread, NULL, ST_GetVpeOutputDataThread, (void *)(&u8PortId[j*ST_MAX_PORT_NUM+i]));
        }
    }

    for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
    {
        ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
        if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
        {
            MI_U8 vifChn=0, vifPort=0;

            vifChn = pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32ChnId;
            vifPort = pstVpeChnattr->stInputPortAttr[0].stBindParam.stChnPort.u32PortId;

            if(pstVpeChnattr->eRunningMode == E_MI_VPE_RUN_DVR_MODE)
            {
                DBG_INFO("chn %d, dvr mode, not open iqserver \n", Vpechn);
                continue;
            }

            MI_VIF_ChnPortAttr_t stVifPortInfo;
            memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));
            STCHECKRESULT(MI_VIF_GetChnPortAttr(vifChn, vifPort, &stVifPortInfo));

            STCHECKRESULT(MI_IQSERVER_Open(stVifPortInfo.stDestSize.u16Width, stVifPortInfo.stDestSize.u16Height, Vpechn));
        }
    }

    for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
    {
        ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
        size = strlen(pstVpeChnattr->IqCfgbin_Path);
        if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE
            && 0 != size)
        {
            printf("pstVpeChnattr->IqCfgbin_Path = %s, vpechn %d\n",pstVpeChnattr->IqCfgbin_Path,Vpechn);
            ST_DoSetIqBin(Vpechn,pstVpeChnattr->IqCfgbin_Path);
        }
    }


    while(!bExit)
    {
        MI_U32 u32Select = 0xff;
        printf("select 0: change Hdr res \n");
        printf("select 2: change rotate\n");
        printf("select 3: change chancrop\n");
        printf("select 4: change portMode\n");
        printf("select 5: change portcrop\n");
        printf("select 6: Get port buffer\n");
        printf("select 7: disable port \n");
        printf("select 8: get venc out \n");
#if SUPPORT_LDC
        printf("select 9: Ldc on/off \n");
#endif
        printf("select 10: vpe chn zoom\n");
        printf("select 11: set api bin\n");
        printf("select 12: exit\n");
        printf("select 13: enable cus3a demo\n");
        scanf("%d", &u32Select);
        ST_Flush();
        if(u32Select == 0)
            bExit = ST_DoChangeHdrRes(u32ChnNum);
        else if(u32Select == 2)
        {
            bExit =ST_DoChangeRotate(u32ChnNum);
        }
        else if(u32Select == 3)
        {
            bExit =ST_DoSetChnCrop(u32ChnNum);
        }
        else if(u32Select == 4)
        {
            bExit =ST_ChangePortMode(u32ChnNum);
        }
        else if(u32Select == 5)
        {
            bExit =ST_DoSetPortCrop(u32ChnNum);
        }
        else if(u32Select == 6)
        {
            bExit =ST_GetVpeOutputData(u32ChnNum);
        }
        else if(u32Select == 7)
        {
            bExit =ST_VpeDisablePort(u32ChnNum);
        }
        else if(u32Select == 8)
        {
            bExit =ST_GetVencOut();
        }
#if SUPPORT_LDC
        else if(u32Select == 9)
        {
            bExit =ST_SetLdcOnOff(u32ChnNum);
        }
#endif
        else if(u32Select == 10)
        {
            bExit =ST_DoSetChnZoom(u32ChnNum);
        }
        else if(u32Select == 11)
        {
            printf("Dynamic load IQ bin\n");
            if(u32ChnNum > 1)
            {
                printf("select channel id:");
                scanf("%d", &Vpechn);
                ST_Flush();

            }
            else
            {
                for(Vpechn=0; Vpechn<ST_MAX_VPECHN_NUM; Vpechn++)
                {
                    ST_VpeChannelAttr_t *pstVpeChnattr = &gstVpeChnattr[Vpechn];
                    if(pstVpeChnattr->bUsed==TRUE &&  pstVpeChnattr->bCreate == TRUE)
                    {
                        printf("use vpe chn %d \n", Vpechn);
                        break;
                    }
                }
            }
            if(Vpechn >= ST_MAX_VPECHN_NUM)
            {
                printf("VpeChn %d > max %d \n", Vpechn, ST_MAX_VPECHN_NUM);
                return FALSE;
            }

            printf("write API Bin file path:\n");
            scanf("%s", sFilePath);
            ST_Flush();
            bExit =ST_DoSetIqBin(Vpechn,sFilePath);
        }
        else if(u32Select == 12)
        {
            bExit = TRUE;
        }
        else if(u32Select == 13)
        {
            ST_EnableCustomize3A();
        }

        usleep(100 * 1000);
    }

    usleep(100 * 1000);
    for(i=0;i<ST_MAX_VPECHN_NUM;i++)
    {
        if(gstVpeChnattr[i].bUsed == FALSE)
            continue;

        size = strlen(gstVpeChnattr[i].stInputFileAttr.InputFilePath);
        if(0 != size)
        {
            pthread_cancel(gstVpeChnattr[i].stInputFileAttr.pPutDatathread);
        }
    }

    for(j=0; j< ST_MAX_VPECHN_NUM; j++)
    {
        for(i=0; i< ST_MAX_PORT_NUM; i++)
        {
            void *retarg = NULL;
            if(gstVpeChnattr[j].bUsed == FALSE || gstVpeChnattr[j].stVpePortAttr[i].bUsed == FALSE)
                continue;
            pthread_cancel(gstVpeChnattr[j].stVpePortAttr[i].pGetDatathread);
            pthread_join(gstVpeChnattr[j].stVpePortAttr[i].pGetDatathread, &retarg);
        }

        for(i=0; i< ST_MAX_PORT_NUM; i++)
        {
            pthread_mutex_destroy(&gstVpeChnattr[j].stVpePortAttr[i].Portmutex);
        }
    }

    ST_RtspServerStop();

    ST_BaseModuleUnInit();

    u32ChnNum=0;
    u32VencChnNum=0;
    gbMutiVpeChnNum =0;

    memset(&gstSensorAttr, 0x0, sizeof(gstSensorAttr));
    memset(&gstVifModule, 0x0, sizeof(gstVifModule));
    memset(&gstVpeChnattr, 0x0, sizeof(gstVpeChnattr));
    memset(&gstVencattr, 0x0, sizeof(gstVencattr));

    return 0;
}

