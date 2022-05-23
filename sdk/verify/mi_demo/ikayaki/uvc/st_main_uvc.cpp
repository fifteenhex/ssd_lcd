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
#include <poll.h>

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include "st_uvc.h"

#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "Live555RTSPServer.hh"

#include "mi_rgn.h"
#include "mi_divp.h"
#include "mi_divp_datatype.h"

#include "mi_od.h"
#include "mi_md.h"

#include "mi_vdf.h"
#include "mi_ao.h"
#include "mi_isp.h"
#include "mi_iqserver.h"

#include "linux_list.h"


#define ENABLE_SSNN_FD           (0)
#if ENABLE_SSNN_FD
#include <math.h>
#include <string.h>
#include "ssnn.h"
#include "mi_aio_datatype.h"

#define SSNN_FD_INPUT_W         (320)
#define SSNN_FD_INPUT_H         (240)

static int g_divp_in_width  = 0;
static int g_divp_in_height = 0;

#endif


#define RTSP_LISTEN_PORT        554
#define USB_CAMERA0_INDEX          0
#define USB_CAMERA1_INDEX          1
#define USB_CAMERA2_INDEX          2
#define UVC_STREAM0                "uvc_stream0"
#define UVC_STREAM1                "uvc_stream1"
#define UVC_STREAM2                "uvc_stream2"
#define MAX_UVC_DEV_NUM             3
#define PATH_PREFIX                "/mnt"
#define DEBUG_ES_FILE            0

#define RGN_OSD_TIME_START        8
#define RGN_OSD_MAX_NUM         4
#define RGN_OSD_TIME_WIDTH        180
#define RGN_OSD_TIME_HEIGHT        32

#define DOT_FONT_FILE            "/customer/mi_demo/gb2312.hzk"

#define MAX_CAPTURE_NUM            4
#define CAPTURE_PATH            "/mnt/capture"

#define MI_AUDIO_SAMPLE_PER_FRAME 768
#define AO_INPUT_FILE            "8K_16bit_STERO_30s.wav"
#define AO_OUTPUT_FILE            "./tmp.pcm"

#define DIVP_CHN_FOR_OSD        0
#define DIVP_CHN_FOR_DLA        1
#define DIVP_CHN_FOR_VDF        2
#define DIVP_CHN_FOR_ROTATION    3
#define DIVP_CHN_FOR_SCALE        3
#define VENC_CHN_FOR_CAPTURE    12

#define SCALE_TEST    0
#define USE_STREAM_FILE 0
#define DIVP_CHN_FOR_RESOLUTION 0
#define DIVP_SCALE_IPNUT_FILE    "vpe0_port0_1920x1088_0000.yuv"

#define MAX_CHN_NEED_OSD        4

#define RAW_W                     384
#define RAW_H                     288
#define PANEL_DIVP_ROTATE        0

#define RGB_TO_CRYCB(r, g, b)                                                            \
        (((unsigned int)(( 0.439f * (r) - 0.368f * (g) - 0.071f * (b)) + 128.0f)) << 16) |    \
        (((unsigned int)(( 0.257f * (r) + 0.564f * (g) + 0.098f * (b)) + 16.0f)) << 8) |        \
        (((unsigned int)((-0.148f * (r) - 0.291f * (g) + 0.439f * (b)) + 128.0f)))

#define IQ_FILE_PATH    "/customer/iq_api.bin"

#define MAX_RGN_COVER_W               8192
#define MAX_RGN_COVER_H               8192

#define RGN_OSD_HANDLE                    0
#define RGN_FOR_VDF_BEGIN                12

#define MAX_FULL_RGN_NULL                3

#ifdef ALIGN_UP
#undef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#else
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#endif
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif
struct ST_Stream_Attr_T
{
    MI_BOOL        bEnable;
    ST_Sys_Input_E enInput;
    MI_SNR_PAD_ID_e enSnrPad;
    MI_U32     u32VifDevId;
    MI_U32     u32InputChn;
    MI_U32     u32InputPort;
    MI_U32     divpChn;
    MI_U32     divpPort;
    MI_S32     vencChn;
    MI_VENC_ModType_e eType;
    float      f32Mbps;
    MI_U32     u32Width;
    MI_U32     u32Height;

    MI_U32 enFunc;
    const char    *pszStreamName;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindPara;
};

typedef struct
{
    MI_S32 s32UseOnvif;     //0:not use, else use
    MI_S32 s32UseVdf;         // 0: not use, else use
    MI_S32 s32LoadIQ;        // 0: not load, else load IQ file
    MI_S32 s32HDRtype;
    ST_Sensor_Type_T enSensorType;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
    MI_U8 u8SnrPad;
    MI_S8 s8SnrResIndex;
} ST_Config_S;

typedef struct {
    MI_U32  fcc;
    MI_U32  u32Width;
    MI_U32  u32Height;
    MI_U32  u32FrameRate;
    MI_SYS_ChnPort_t dstChnPort;
} ST_UvcSetting_Attr_T;

typedef struct VENC_STREAMS_s {
    bool used;
    MI_VENC_Stream_t stStream;
} VENC_STREAMS_t;

typedef struct {
    VENC_STREAMS_t *pstuserptr_stream;
} ST_Uvc_Resource_t;

typedef struct {
    char name[20];
    int dev_index;
    ST_UVC_Handle_h handle;
    ST_UvcSetting_Attr_T setting;
    ST_Uvc_Resource_t res;
} ST_UvcDev_t;

typedef struct {
    int devnum;
    ST_UvcDev_t dev[];
} ST_UvcSrc_t;

static MI_BOOL g_bExit = FALSE;
static ST_Config_S g_stConfig;
static ST_UvcSrc_t * g_UvcSrc;
static MI_U8 g_bitrate[MAX_UVC_DEV_NUM] = {0, 0, 0};
static MI_U8 g_maxbuf_cnt = 3;
static MI_U64 g_reqIdr_cnt = 0;
static MI_U8 g_enable_iqserver = 0;
static MI_U8 g_load_iq_bin = 0;
static MI_U32 g_device_num = 1;
static char g_IspBinPath[64] = {0};

static struct ST_Stream_Attr_T g_stStreamAttr[] =
{
    [USB_CAMERA0_INDEX] =
    {
        .bEnable = TRUE,
        .enInput = ST_Sys_Input_VPE,
        .enSnrPad = E_MI_SNR_PAD_ID_0,
        .u32VifDevId = 0,
        .u32InputChn = 0,
        .u32InputPort = 0,
        .divpChn = 0,
        .divpPort = 0,
        .vencChn = 0,
        .eType = E_MI_VENC_MODTYPE_H264E,
        .f32Mbps = 2.0,
        .u32Width = 1920,
        .u32Height = 1080,
        .enFunc = ST_Sys_Func_UVC,
        .pszStreamName = UVC_STREAM2,
#if ENABLE_SSNN_FD
        .eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE,
#else
        .eBindType = E_MI_SYS_BIND_TYPE_REALTIME,
#endif
        .u32BindPara = 0,
    },
    [USB_CAMERA1_INDEX] =
    {
        .bEnable = TRUE,
        .enInput = ST_Sys_Input_VPE,
        .enSnrPad = E_MI_SNR_PAD_ID_1,
        .u32VifDevId = 1,
        .u32InputChn = 1,
        .u32InputPort = 0,
        .divpChn = 1,
        .divpPort = 0,
        .vencChn = 1,
        .eType = E_MI_VENC_MODTYPE_JPEGE,
        .f32Mbps = 2.0,
        .u32Width = 1920,
        .u32Height = 1080,
        .enFunc = ST_Sys_Func_UVC,
        .pszStreamName = UVC_STREAM1,
        .eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE,
        .u32BindPara = 0,
    },
    [USB_CAMERA2_INDEX] =
    {
        .bEnable = FALSE,
        .enInput = ST_Sys_Input_VPE,
        .enSnrPad = E_MI_SNR_PAD_ID_2,
        .u32VifDevId = 2,
        .u32InputChn = 0,
        .u32InputPort = 2,
        .vencChn = 3,
        .eType = E_MI_VENC_MODTYPE_H264E,
        .f32Mbps = 2.0,
        .u32Width = 1920,
        .u32Height = 1080,
        .enFunc = ST_Sys_Func_UVC,
        .pszStreamName = UVC_STREAM2,
        .eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE,//E_MI_SYS_BIND_TYPE_HW_RING,
        .u32BindPara = 0,
    },
};

/* audio related */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"{
#endif
#include <cerrno>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "mi_sys_datatype.h"
#include "mi_sys.h"
#include "mi_ao.h"
#include "mi_ai.h"
#include "st_common.h"
#include "st_uac.h"
#include "pcm.h"
#include <sound/asound.h>
#include "mixer.h"
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#define AI_DEV_ID_MAX   (4)
#define AO_DEV_ID_MAX   (1)

#define AI_DEV_AMIC     (0)
#define AI_DEV_DMIC     (1)
#define AI_DEV_I2S_RX   (2)
#define AI_DEV_LineIn   (3)
#define AI_DEV_I2S_RX_AND_SRC   (4)

#define AO_DEV_LineOut  (0)
#define AO_DEV_I2S_TX   (1)

#define AI_VOLUME_AMIC_MIN      (0)
#define AI_VOLUME_AMIC_MAX      (21)
#define AI_VOLUME_DMIC_MIN      (-60)
#define AI_VOLUME_DMIC_MAX      (3)
#define AI_VOLUME_LINEIN_MIN    (0)
#define AI_VOLUME_LINEIN_MAX    (7)

typedef struct {
    MI_BOOL bComfortNoiseEnable;
    MI_S16 s16DelaySample;
    MI_U32 u32AecSupfreq[6];
    MI_U32 u32AecSupIntensity[7];
    MI_S32 s32Reserved;
} ST_AI_AecConfig_t;

typedef struct {
    MI_AUDIO_AlgorithmMode_e eMode;
    MI_U32 u32NrIntensityBand[6];
    MI_U32   u32NrIntensity[7];
    MI_U32   u32NrSmoothLevel;
    MI_AUDIO_NrSpeed_e eNrSpeed;
}ST_AUDIO_AnrConfig_t;

MI_BOOL bEnableAI = FALSE;
MI_BOOL bEnableAO= FALSE;
MI_BOOL bAiEnableVqe = FALSE;
MI_BOOL bAiEnableNr = FALSE;
MI_BOOL  bAiEnableAec = FALSE;
MI_BOOL bAoEnableVqe = FALSE;
MI_BOOL bAoEnableNr = FALSE;
MI_U32 u32AiSampleRate = E_MI_AUDIO_SAMPLE_RATE_48000;
MI_U32 u32AoSampleRate = E_MI_AUDIO_SAMPLE_RATE_48000;
MI_S32 AiVolume;
MI_AUDIO_DEV AoDevId = AO_DEV_LineOut;
MI_AO_CHN AoChn;
MI_AUDIO_DEV AiDevId = AI_DEV_DMIC;
MI_AI_CHN AiChn;

ST_UAC_Handle_h p_handle;
ST_UAC_Config_t uac_playback_config;
ST_UAC_Handle_h c_handle;
ST_UAC_Config_t uac_capture_config;

pthread_t Aotid;
pthread_t Aitid;
pthread_t Voltid;
#if ENABLE_SSNN_FD
pthread_t SsnnFdTid;
#endif

MI_AI_AecConfig_t stAecCfg = {
    .bComfortNoiseEnable = FALSE,
    .s16DelaySample = 0,
    .u32AecSupfreq = {4, 6, 36, 49, 50, 51},
    .u32AecSupIntensity = {5, 4, 4, 5, 10, 10, 10},
};

MI_AUDIO_AnrConfig_t stAnrCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    .u32NrIntensity = {15},
    .u32NrSmoothLevel = {10},
    .eNrSpeed = E_MI_AUDIO_NR_SPEED_MID,
};

#define AUDIO_ENABLE
#ifdef AUDIO_ENABLE

/* param's check is not complete */
MI_BOOL checkAudioParam(void)
{
    MI_BOOL bCheckPass = FALSE;

    do {
        if(bEnableAI)
        {
            if(AiDevId < 0 || AiDevId > AI_DEV_ID_MAX)
            {
                printf("Invalid AiDevId!!!\n");
                break;
            }

            if(
                    (E_MI_AUDIO_SAMPLE_RATE_8000 != u32AiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != u32AiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_32000 != u32AiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != u32AiSampleRate)
            )
            {
                printf("Invalid Ai Sample Rate!!!\n");
                break;
            }

            if(bAiEnableAec)
            {
                if((E_MI_AUDIO_SAMPLE_RATE_8000 != u32AiSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != u32AiSampleRate))
                {
                    printf("Aec only support 8K/16K!!!\n");
                    break;
                }

                if(AI_DEV_I2S_RX == AiDevId)
                {
                    printf("I2S RX not support AEC!!!\n");
                    break;
                }
            }

            if(bAiEnableVqe)
            {
                if(
                        (E_MI_AUDIO_SAMPLE_RATE_8000 != u32AiSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != u32AiSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != u32AiSampleRate)
                )
                {
                    printf("Vqe only support 8K/16/48K!!!\n");
                    break;
                }
            }
        }

        if(bEnableAO)
        {
            if((AoDevId < 0) || (AoDevId > AO_DEV_ID_MAX))
            {
                printf("Invalid AoDevId!!!\n");
                break;
            }

            if(
                    (E_MI_AUDIO_SAMPLE_RATE_8000 != u32AoSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != u32AoSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_32000 != u32AoSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != u32AoSampleRate)
            )
            {
                printf("Invalid Ao Sample Rate!!!\n");
                break;
            }
        }

        bCheckPass = TRUE;
    }while(0);

    return bCheckPass;
}

MI_S32 ST_AO_Init(void)
{
    MI_S32 ret;
    MI_AUDIO_Attr_t stAttr;

    stAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
    stAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 8;
    stAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAttr.u32PtNumPerFrm = 1024;
    stAttr.u32ChnCnt = 2;   //u32ChnCnt=1 in master branch, u32ChnCnt=2 in stable branch
    stAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    stAttr.eSamplerate = (MI_AUDIO_SampleRate_e)u32AoSampleRate;

    /* set ao public attr */
    ret = MI_AO_SetPubAttr(AoDevId, &stAttr);
    if(MI_SUCCESS != ret)
    {
        printf("set ao %d attr err:0x%x\n", AoDevId, ret);
        return ret;
    }

    /* enable ao device */
    ret = MI_AO_Enable(AoDevId);
    if(MI_SUCCESS != ret)
    {
        printf("enable ao dev %d err:0x%x\n", AoDevId, ret);
        return ret;
    }

    ret = MI_AO_EnableChn(AoDevId, AoChn);
    if(MI_SUCCESS != ret)
    {
        printf("enable ao dev %d chn %d err:0x%x\n", AoDevId, AoChn, ret);
        return ret;
    }

    if(bAoEnableVqe)
    {
        MI_AO_VqeConfig_t stAoVqeConfig;
        stAoVqeConfig.bAgcOpen = FALSE;
        stAoVqeConfig.bAnrOpen = bAoEnableNr;
        stAoVqeConfig.bEqOpen = FALSE;
        stAoVqeConfig.bHpfOpen = FALSE;
        stAoVqeConfig.s32FrameSample = 128;
        stAoVqeConfig.s32WorkSampleRate = (MI_AUDIO_SampleRate_e)u32AoSampleRate;
        memcpy(&stAoVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));

        ret = MI_AO_SetVqeAttr(AoDevId, AoChn, &stAoVqeConfig);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AO_SetVqeAttr failed\n");
            return ret;
        }

        ret = MI_AO_EnableVqe(AoDevId, AoChn);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AO_EnableVqe failed\n");
            return ret;
        }
    }

    return MI_SUCCESS;
}

MI_S32 ST_AO_Deinit(void)
{
     /* disable ao device */
     MI_S32 ret;

    if(bAoEnableVqe)
    {
        ret = MI_AO_DisableVqe(AoDevId, AoChn);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AO_DisableVqe failed\n");
            return ret;
        }
    }

    ret = MI_AO_DisableChn(AoDevId, AoChn);
    if(MI_SUCCESS != ret)
    {
        printf("MI_AO_DisableChn failed\n");
        return ret;
    }

    ret = MI_AO_Disable(AoDevId);
    if(MI_SUCCESS != ret)
    {
        printf("disable ao dev %d err:0x%x\n", AoDevId, ret);
        return ret;
    }

    return MI_SUCCESS;
}

void* aoSendFrame(void* data)
{
    MI_S32 ret;
    ST_UAC_Frame_t stFrame ={};
    stFrame.length = ST_UAC_GetPcm_BufSize(p_handle);
    stFrame.data = malloc(stFrame.length);

    while(!g_bExit)
    {
        memset(stFrame.data, 0, ST_UAC_GetPcm_BufSize(p_handle));

        ret = ST_UAC_GetFrame(p_handle, &stFrame);
        if(MI_SUCCESS != ret)
        {
            if(EBUSY != errno)
                printf("ST_UAC_GetFrame failed:%s %d\n",  strerror(errno), errno);

            usleep(1000);
            continue;
        }

        MI_AUDIO_Frame_t stAoSendFrame;
        stAoSendFrame.u32Len[0] = stFrame.length;
        stAoSendFrame.apVirAddr[0] = stFrame.data;
        stAoSendFrame.apVirAddr[1] = NULL;

        do {
            ret = MI_AO_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);
        }while(MI_AO_ERR_NOBUF == ret);

        if(MI_SUCCESS != ret)
        {
            printf("MI_AO_SendFrame failed!\n");
            break;
        }
    }

    free(stFrame.data);
    ret = 0;
    pthread_exit(&ret);
}

MI_S32 ST_AI_Init(void)
{
    MI_S32 ret;
    MI_AUDIO_Attr_t stAttr;
    MI_SYS_ChnPort_t stAiChnOutputPort;

    stAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAttr.eSamplerate = (MI_AUDIO_SampleRate_e)u32AiSampleRate;
    stAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    stAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAttr.u32ChnCnt = 1;
    stAttr.u32CodecChnCnt = 0; // useless
    stAttr.u32FrmNum = 16;
    stAttr.u32PtNumPerFrm = 1024;
    stAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
    stAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 8;
    stAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;

    /* set public attribute of AI device */
    printf("ai init,aidevid:%d\n",AiDevId);
    ret = MI_AI_SetPubAttr(AiDevId, &stAttr);
    if(MI_SUCCESS != ret)
    {
        printf("set ai %d attr err:0x%x\n", AiDevId, ret);
        return ret;
    }

    /* enable AI device */
    ret = MI_AI_Enable(AiDevId);
    if(MI_SUCCESS != ret)
    {
        printf("enable ai %d err:0x%x\n", AiDevId, ret);
        return ret;
    }

    /* set buffer depth */
    stAiChnOutputPort.eModId = E_MI_MODULE_ID_AI;
    stAiChnOutputPort.u32DevId = AiDevId;
    stAiChnOutputPort.u32ChnId = AiChn;
    stAiChnOutputPort.u32PortId = 0;
    MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort, 1, 8);

    /* MI_AI_SetVqeVolume */
    switch(AiDevId)
    {
        case AI_DEV_AMIC:
            ret = MI_AI_SetVqeVolume(AiDevId, AiChn, AI_VOLUME_AMIC_MAX);
            if(MI_SUCCESS != ret)
            {
                printf("MI_AI_SetVqeVolume failed\n");
                return ret;
            }
            break;

         case AI_DEV_DMIC:
            ret = MI_AI_SetVqeVolume(AiDevId, AiChn, AI_VOLUME_DMIC_MAX);
            if(MI_SUCCESS != ret)
            {
                printf("MI_AI_SetVqeVolume failed\n");
                return ret;
            }
            break;

        case AI_DEV_LineIn:
            ret = MI_AI_SetVqeVolume(AiDevId, AiChn, AI_VOLUME_LINEIN_MAX);
            if(MI_SUCCESS != ret)
            {
                printf("MI_AI_SetVqeVolume failed\n");
                return ret;
            }
            break;

        default:
            break;
    }

    /* enable AI Channel */
    ret = MI_AI_EnableChn(AiDevId, AiChn);
    if(MI_SUCCESS != ret)
    {
        printf("enable Dev%d Chn%d err:0x%x\n", AiDevId, AiChn, ret);
        return ret;
    }
    if(bAiEnableVqe)
    {
        MI_AI_VqeConfig_t   stAiVqeConfig;
        stAiVqeConfig.u32ChnNum = 1;
        stAiVqeConfig.bAecOpen = bAiEnableAec;
        stAiVqeConfig.bAgcOpen = FALSE;
        stAiVqeConfig.bAnrOpen = bAiEnableNr;
        stAiVqeConfig.bEqOpen = FALSE;
        stAiVqeConfig.bHpfOpen = FALSE;
        stAiVqeConfig.s32WorkSampleRate = (MI_AUDIO_SampleRate_e)u32AiSampleRate;
        memcpy(&stAiVqeConfig.stAecCfg, &stAecCfg, sizeof(MI_AI_AecConfig_t));
        memcpy(&stAiVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));

        ret = MI_AI_SetVqeAttr(AiDevId, AiChn, 0, 0, &stAiVqeConfig);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AI_SetVqeAttr failed\n");
            return ret;
        }

        ret = MI_AI_EnableVqe(AiDevId, AiChn);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AI_EnableVqe failed\n");
            return ret;
        }
    }

    return MI_SUCCESS;
}

MI_S32 ST_AI_Deinit(void)
{
    MI_S32 ret;

    if(bAiEnableVqe)
    {
        ret = MI_AI_DisableVqe(AiDevId, AiChn);
        if(MI_SUCCESS != ret)
        {
            printf("MI_AI_DisableVqe failed\n");
            return ret;
        }
    }

    /* disable AI Channel */
    ret = MI_AI_DisableChn(AiDevId, AiChn);
    if(MI_SUCCESS != ret)
    {
        printf("disable Dev%d Chn%d err:0x%x\n", AiDevId, AiChn, ret);
        return ret;
    }
    /* disable AI Device */
    ret = MI_AI_Disable(AiDevId);
    if(MI_SUCCESS != ret)
    {
        printf("disable ai %d err:0x%x\n", AiDevId, ret);
        return ret;
    }

    return MI_SUCCESS;
}

void* aiGetFrame(void* data)
{
    MI_S32 ret;
    ST_UAC_Frame_t stFrame ={};

    while(!g_bExit)
    {
        MI_AUDIO_Frame_t stAiChFrame;
        ret = MI_AI_GetFrame(AiDevId, AiChn, &stAiChFrame, NULL, -1);
        if(MI_SUCCESS == ret)
        {
            stFrame.data = stAiChFrame.apVirAddr[0];
            stFrame.length = stAiChFrame.u32Len[0];
            //printf("get ai frame:%d\n",stFrame.length);
            ret = ST_UAC_SendFrame(c_handle, &stFrame);
            if(MI_SUCCESS != ret)
            {
                if(EBUSY != errno)
                    printf("ST_UAC_SendFrame failed:%s %d\n",  strerror(errno), errno);

                MI_AI_ReleaseFrame(AiDevId, AiChn, &stAiChFrame, NULL);
                usleep(1000);
                continue;
            }

            MI_AI_ReleaseFrame(AiDevId, AiChn, &stAiChFrame, NULL);
        }
        else
        {
            printf("MI_AI_GetFrame failed!\n");
            break;
        }
    }

    ret = 0;
    pthread_exit(&ret);
}

MI_U32 inline Vol_to_Db(MI_U32 Vol, MI_U32 Min_Vol, MI_U32 Max_Vol)
{
    switch(AiDevId)
    {
        case AI_DEV_AMIC:
            return (Vol * (AI_VOLUME_AMIC_MAX - AI_VOLUME_AMIC_MIN) / (Max_Vol - Min_Vol)) + AI_VOLUME_AMIC_MIN;
            break;

         case AI_DEV_DMIC:
            return (Vol * (AI_VOLUME_DMIC_MAX - AI_VOLUME_DMIC_MIN) / (Max_Vol - Min_Vol)) + AI_VOLUME_DMIC_MIN;
            break;

        case AI_DEV_LineIn:
            return (Vol * (AI_VOLUME_LINEIN_MAX - AI_VOLUME_LINEIN_MIN) / (Max_Vol - Min_Vol)) + AI_VOLUME_LINEIN_MIN;
            break;

        default:
            break;
    }
}

void* aiSetVol(void* data)
{
    MI_U32 ret = MI_SUCCESS;

    struct mixer *mixer = mixer_open(0);
    if(!mixer)
    {
        fprintf(stderr, "Failed to open mixer\n");
        ret = EXIT_FAILURE;
        pthread_exit(&ret);
    }

    struct mixer_ctl *ctl;
    int min, max;

    ret = mixer_subscribe_events(mixer, 1);
    if(MI_SUCCESS != ret)
    {
        fprintf(stderr, "Failed to subscribe events\n");
        pthread_exit(&ret);
    }

    ctl = mixer_get_ctl(mixer, 0);
    min = mixer_ctl_get_range_min(ctl);
    max = mixer_ctl_get_range_max(ctl);
    if(!ctl)
    {
        fprintf(stderr, "Invalid mixer control\n");
        ret = EXIT_FAILURE;
        pthread_exit(&ret);
    }

    while(!g_bExit)
    {
        ret = mixer_wait_event(mixer, 1000);
        if(ret == 1)
        {
            ret = mixer_ctl_get_event(ctl, 0);
            if(MI_SUCCESS != ret)
            {
                printf("mixer_ctl_get_event failed\n");
                break;
            }

            AiVolume = mixer_ctl_get_value(ctl, 0);

            /* MI_AI_SetVqeVolume */
            MI_S32 s32VolumeDb = Vol_to_Db(AiVolume, min, max);
            ret = MI_AI_SetVqeVolume(AiDevId, AiChn, s32VolumeDb);
            if(MI_SUCCESS != ret)
            {
                printf("MI_AI_SetVqeVolume failed\n");
                break;
            }
            printf("current volume is %d\n", s32VolumeDb);
        }
    }

    ret = mixer_subscribe_events(mixer, 0);
    if(MI_SUCCESS != ret)
    {
        fprintf(stderr, "Failed to subscribe events\n");
        pthread_exit(&ret);
    }

    mixer_close(mixer);
    pthread_exit(&ret);
}

MI_S32 ST_AudioModuleInit(void)
{
    if(bEnableAO)
    {
        MI_S32 ret;

        ret = ST_AO_Init();
        if(MI_SUCCESS != ret)
        {
            printf("ST_AO_Init failed!\n");
            return ret;
        }

        ST_UAC_Config(&uac_playback_config, PCM_IN, 2, u32AoSampleRate, 1024, 4, PCM_FORMAT_S16_LE);
        ret = ST_UAC_Init(&p_handle, &uac_playback_config);
        if(MI_SUCCESS != ret)
        {
            printf("ST_UAC_Init failed!\n");
            return ret;
        }

        pthread_create(&Aotid, NULL, aoSendFrame, NULL);
        printf("create audio playback thread.\n");
    }

    if(bEnableAI)
    {
        printf("enable ai\n");
        MI_U32 ret;

        ret = ST_AI_Init();
        if(MI_SUCCESS != ret)
        {
            printf("ST_AI_Init failed!\n");
            return ret;
        }
        ST_UAC_Config(&uac_capture_config, PCM_OUT, 2, u32AiSampleRate, 1024, 4, PCM_FORMAT_S16_LE);
        ret = ST_UAC_Init(&c_handle, &uac_capture_config);
        if(MI_SUCCESS != ret)
        {
            printf("ST_UAC_Init failed!\n");
            return ret;
        }
        pthread_create(&Aitid, NULL, aiGetFrame, NULL);
        printf("create audio capture thread.\n");
        #if 0
        if(AI_DEV_AMIC == AiDevId || AI_DEV_DMIC == AiDevId || AI_DEV_LineIn == AiDevId)
        {
            pthread_create(&Voltid, NULL, aiSetVol, NULL);
            printf("create volume control thread.\n");
        }
        else
        {
            printf("this mode do not support volume control.\n");
        }
        #endif
    }

    return MI_SUCCESS;
}

MI_S32 ST_AudioModuleUnInit(void)
{
    if(bEnableAO)
    {
        pthread_join(Aotid, NULL);
        printf("join Ao thread done.\n");

        ST_UAC_Uninit(p_handle);
        ST_AO_Deinit();
    }

    if(bEnableAI)
    {
        if(AI_DEV_AMIC == AiDevId || AI_DEV_DMIC == AiDevId || AI_DEV_LineIn == AiDevId)
        {
            pthread_join(Voltid, NULL);
            printf("join Vol thread done.\n");
        }

        pthread_join(Aitid, NULL);
        printf("join Ai thread done.\n");

        ST_UAC_Uninit(c_handle);
        ST_AI_Deinit();
    }

    return MI_SUCCESS;
}

#else

MI_BOOL checkAudioParam(void)
{
    return TRUE;
}

MI_S32 ST_AudioModuleInit(void)
{
    return MI_SUCCESS;
}

MI_S32 ST_AudioModuleUnInit(void)
{
    return MI_SUCCESS;
}

#endif

void ST_Flush(void)
{
    char c;

    while((c = getchar()) != '\n' && c != EOF);
}
#define DEVICE_TO_PORT(x) (4*x)


#if ENABLE_SSNN_FD
static bool gbSsnnFdRun = false;
void* ssnnFdThread(void* data)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 s32Fd = 0;
    fd_set read_fds;
    struct timeval TimeoutVal;
    char szFileName[128];
    int fd = 0;
    MI_U32 u32GetFramesCount = 0;
    MI_BOOL _bWriteFile = TRUE;
    unsigned char *pRGB = NULL;
    unsigned char *pDivpOutbuf = NULL;
    unsigned char *pTmpbuf = NULL;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    int index = *((int *)data);
    int pos_x1, pos_y1;
    int pos_x2, pos_y2;

    int status = 0;
    network_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.target_height = 320;
    cfg.target_width = 240;
    cfg.num_threads = 1;
    cfg.max_detection = 100;
    cfg.prob_thresh = 0.3;
    cfg.nms_thresh = 0.5;
    cfg.init_network_index = 0;  // choose the FD model

    printf("[Fun:%s - Line:%d] SSNNFD thread Enter!\n", __FUNCTION__, __LINE__);

    // == Initialization == //
    NetworkHandle *handle = NULL;
    status = Init_Network(&handle, &cfg);
    if(status)
    {
        printf("Error: Initialization failed.\n");
        return 0;
    }

    stChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = pstStreamAttr[index].divpChn;
    stChnPort.u32PortId = pstStreamAttr[index].divpPort;
    s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);
    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_GetFd 0, error, %X\n", s32Ret);
        return NULL;
    }
    s32Ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 3);
    if (MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_SetChnOutputPortDepth err:%x, chn:%d,port:%d\n", s32Ret,
                stChnPort.u32ChnId, stChnPort.u32PortId);
        return NULL;
    }
    sprintf(szFileName, "/tmp/divp%d.es", stChnPort.u32ChnId);
    printf("start to record %s\n", szFileName);
    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        ST_ERR("create %s fail\n", szFileName);
    }
    int loop = 1;
    while (gbSsnnFdRun == true)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);
        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret < 0)
        {
            ST_ERR("select failed!\n");
            continue;
        }
        else if(s32Ret == 0)
        {
            ST_ERR("get divp frame time out\n");
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd, &read_fds))
            {
                s32Ret = MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &stBufHandle);
                if(MI_SUCCESS != s32Ret)
                {
                    continue;
                }

                if(pRGB == NULL)
                {
                    pRGB = (unsigned char *)malloc(stBufInfo.stFrameData.u32BufSize);
                    memset(pRGB, 0, stBufInfo.stFrameData.u32BufSize);
                }

                pDivpOutbuf = (unsigned char *)stBufInfo.stFrameData.pVirAddr[0];
                pTmpbuf = pRGB;
                //unsigned char swap=0;
                for(int i=0; i<(int)stBufInfo.stFrameData.u32BufSize; i+=4)
                {
                    memcpy(pTmpbuf, pDivpOutbuf+1, 3);

                    //swap = pTmpbuf[0];
                    //pTmpbuf[0] = pTmpbuf[2];
                    //pTmpbuf[2] = swap;

                    pDivpOutbuf += 4;
                    pTmpbuf += 3;
                }

                // == Forward computation == //
                cfg.target_height = stBufInfo.stFrameData.u16Height;
                cfg.target_width = stBufInfo.stFrameData.u16Width;
                status = Forward_Network(handle, pRGB, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u16Width, 3);
                if(status)
                {
                    printf("Error: Forward computation failed.\n");
                    return 0;
                }

                status = Get_Detection(handle, (int)stBufInfo.stFrameData.u16Height, (int)stBufInfo.stFrameData.u16Width);
                if(handle->num_detection)
                {
                    printf("num_detection:%d\n", handle->num_detection);
                    for(int i=0; i< handle->num_detection; i++)
                    {
                        pos_x1 = (int)((float)(g_divp_in_width*handle->boxes[i].x_min)/(float)SSNN_FD_INPUT_W);
                        pos_y1 = (int)((float)(g_divp_in_height*handle->boxes[i].y_min)/(float)SSNN_FD_INPUT_H);
                        pos_x2 = (int)((float)(g_divp_in_width*handle->boxes[i].x_max)/(float)SSNN_FD_INPUT_W);
                        pos_y2 = (int)((float)(g_divp_in_height*handle->boxes[i].y_max)/(float)SSNN_FD_INPUT_H);

                        printf("    Face %d, (%d, %d) - (%d, %d)\n", (i+1), pos_x1, pos_y1, pos_x2, pos_y2);
                    }
                }

                if(loop < 30)
                {
                    loop ++;
                    if (fd > 0 && loop == 25)
                    {
                        if(_bWriteFile)
                        {
                            //write(fd, pRGB, stBufInfo.stFrameData.u32BufSize*3/4);
                            printf("========> Width:%d  Height:%d Bufsize:%d\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32BufSize);
                        }
                        close(fd);
                        fd = 0;
                    }
                }

                ++u32GetFramesCount;

                //printf("channelId[%u] u32GetFramesCount[%u]\n", stChnPort.u32ChnId, u32GetFramesCount);
                //printf("========> Width:%d  Height:%d Bufsize:%d\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32BufSize);
                MI_SYS_ChnOutputPortPutBuf(stBufHandle);
                usleep(200*1000);

                //break;
            }
        }
    }

    Release_Network(&handle);

    if (fd > 0)
    {
        close(fd);
        fd = -1;
    }

    s32Ret = 0;
    pthread_exit(&s32Ret);

    printf("[Fun:%s - Line:%d] SSNNFD thread Exit!\n", __FUNCTION__, __LINE__);
}
#endif
MI_S32 ST_BaseModuleInit(ST_Config_S* pstConfig, int device_num)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_VIF_WorkMode_e eVifWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    MI_U32 u32ChocieRes =0;
    MI_SNR_PAD_ID_e ePADId = E_MI_SNR_PAD_ID_0;
    ST_Stream_Attr_T *pstStreamAttr = NULL;
    for(int i=0; i<device_num; i++)
    {
        memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
        memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
        memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

        pstStreamAttr = &g_stStreamAttr[i];
        ePADId = pstStreamAttr->enSnrPad;
        ST_DBG("Snr pad id:%d\n", (int)ePADId);

        MI_SYS_Init();
        if(pstConfig->s32HDRtype > 0)
            MI_SNR_SetPlaneMode(ePADId, TRUE);
        else
            MI_SNR_SetPlaneMode(ePADId, FALSE);

        MI_SNR_QueryResCount(ePADId, &u32ResCount);
        for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
        {
            MI_SNR_GetRes(ePADId, u8ResIndex, &stRes);
            printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
            u8ResIndex,
            stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
            stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
            stRes.u32MaxFps,stRes.u32MinFps,
            stRes.strResDesc);
        }

        MI_S8 s8SnrResIndex = pstConfig->s8SnrResIndex;

        if(s8SnrResIndex < 0)
        {
            printf("choice which resolution use, cnt %d\n", u32ResCount);
            do
            {
                scanf("%d", &u32ChocieRes);
                ST_Flush();
                MI_SNR_QueryResCount(ePADId, &u32ResCount);
                if(u32ChocieRes >= u32ResCount)
                {
                    printf("choice err res %d > =cnt %d\n", u32ChocieRes, u32ResCount);
                }
            }while(u32ChocieRes >= u32ResCount);
        }
        else
        {
            if((MI_U32)s8SnrResIndex >= u32ResCount)
            {
                ST_ERR("snr index:%d exceed u32ResCount:%d, set default index 0\n", s8SnrResIndex, u32ResCount);
                s8SnrResIndex = 0;
            }

            u32ChocieRes = s8SnrResIndex;
        }
        printf("You select %d res\n", u32ChocieRes);

        MI_SNR_SetRes(ePADId,u32ChocieRes);
        MI_SNR_Enable(ePADId);

        MI_SNR_GetPadInfo(ePADId, &stPad0Info);
        MI_SNR_GetPlaneInfo(ePADId, 0, &stSnrPlane0Info);

        u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
        u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
        eFrameRate = E_MI_VIF_FRAMERATE_FULL;
        ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

        /************************************************
        Step1:  init SYS
        *************************************************/
        STCHECKRESULT(ST_Sys_Init());

        if(ePADId == E_MI_SNR_PAD_ID_1)
        {
            //Bind vif dev 0 to sensor pad 1, because only vif dev0 can use realtime mode.
            MI_VIF_Dev2SnrPadMuxCfg_t stDev2SnrPad[4];
            stDev2SnrPad[0].eSensorPadID = E_MI_VIF_SNRPAD_ID_0;
            stDev2SnrPad[0].u32PlaneID = 0xff;
            stDev2SnrPad[1].eSensorPadID = E_MI_VIF_SNRPAD_ID_1;
            stDev2SnrPad[1].u32PlaneID = 0xff;
            stDev2SnrPad[2].eSensorPadID = E_MI_VIF_SNRPAD_ID_2;
            stDev2SnrPad[2].u32PlaneID = 0xff;
            stDev2SnrPad[3].eSensorPadID = E_MI_VIF_SNRPAD_ID_3;
            stDev2SnrPad[3].u32PlaneID = 0xff;

            MI_VIF_SetDev2SnrPadMux(stDev2SnrPad, 4);
        }

        /************************************************
        Step2:  init VIF(for IPC, only one dev)
        *************************************************/
        eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
        if ((device_num==1) &&
        ((pstStreamAttr->eBindType == E_MI_SYS_BIND_TYPE_HW_RING) ||
        (pstStreamAttr->eBindType == E_MI_SYS_BIND_TYPE_REALTIME)))
        {
            eVifWorkMode = E_MI_VIF_WORK_MODE_RGB_REALTIME;
        } else {
            eVifWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
            pstStreamAttr->eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
        }

        STCHECKRESULT(ST_Vif_EnableDev(pstStreamAttr->u32VifDevId, eVifWorkMode, eVifHdrType, &stPad0Info));

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
        STCHECKRESULT(ST_Vif_CreatePort(DEVICE_TO_PORT(pstStreamAttr->u32VifDevId), 0, &stVifPortInfoInfo));
        STCHECKRESULT(ST_Vif_StartPort(pstStreamAttr->u32VifDevId, DEVICE_TO_PORT(pstStreamAttr->u32VifDevId), 0));

        //if (enRotation != E_MI_SYS_ROTATE_NONE)
        {
            MI_BOOL bMirror = FALSE, bFlip = FALSE;
            //ExecFunc(MI_VPE_SetChannelRotation(0, enRotation), MI_SUCCESS);

            switch(pstConfig->enRotation)
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

            MI_SNR_SetOrien(ePADId, bMirror, bFlip);
            MI_VPE_SetChannelRotation(0, pstConfig->enRotation);
        }

        memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));

        stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
        stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
        stVpeChannelInfo.u32X = 0;
        stVpeChannelInfo.u32Y = 0;
        stVpeChannelInfo.u16VpeCropW = 0;
        stVpeChannelInfo.u16VpeCropH = 0;
        if ((device_num ==1) &&
        ((pstStreamAttr->eBindType == E_MI_SYS_BIND_TYPE_REALTIME) ||
        (pstStreamAttr->eBindType == E_MI_SYS_BIND_TYPE_HW_RING)))
        {
            stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
            stBindInfo.eBindType = pstStreamAttr->eBindType;
        } else {
            stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;
            stBindInfo.eBindType = pstStreamAttr->eBindType;
        }
        stVpeChannelInfo.eFormat = ePixFormat;
        stVpeChannelInfo.e3DNRLevel = pstConfig->en3dNrLevel;
        stVpeChannelInfo.eHDRtype = eVpeHdrType;
        stVpeChannelInfo.bRotation = FALSE;

        stVpeChannelInfo.eBindSensorId = (MI_VPE_SensorChannel_e)(ePADId + 1);
        STCHECKRESULT(ST_Vpe_CreateChannel(pstStreamAttr->u32InputChn, &stVpeChannelInfo));
        STCHECKRESULT(ST_Vpe_StartChannel(pstStreamAttr->u32InputChn));

        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
        stBindInfo.stSrcChnPort.u32DevId = pstStreamAttr->u32VifDevId;
        stBindInfo.stSrcChnPort.u32ChnId = DEVICE_TO_PORT(pstStreamAttr->u32VifDevId);
        stBindInfo.stSrcChnPort.u32PortId = 0;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stDstChnPort.u32DevId = 0;
        stBindInfo.stDstChnPort.u32ChnId = pstStreamAttr->u32InputChn;
        stBindInfo.stDstChnPort.u32PortId = pstStreamAttr->u32InputPort;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    }

    if(g_enable_iqserver)
    {
        ST_DBG("MI_IQSERVER_Open...\n");
        STCHECKRESULT(MI_IQSERVER_Open(u32CapWidth, u32CapHeight, 0));
    }

    return MI_SUCCESS;
}

MI_S32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
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
    STCHECKRESULT(ST_Vif_StopPort(0, 0));
    STCHECKRESULT(ST_Vif_DisableDev(0));

    /************************************************
    Step3:  destory SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Exit());

    if(g_enable_iqserver)
    {
        MI_IQSERVER_Close();
    }
    MI_SYS_Exit();
    return MI_SUCCESS;
}

void *ST_DIVPGetResult(void *args)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 s32Fd = 0;
    fd_set read_fds;
    struct timeval TimeoutVal;
    char szFileName[128];
    int fd = 0;
    MI_U32 u32GetFramesCount = 0;
    MI_BOOL _bWriteFile = TRUE;

    stChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = DIVP_CHN_FOR_VDF;
    stChnPort.u32PortId = 0;

    s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);
    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_GetFd 0, error, %X\n", s32Ret);
        return NULL;
    }
    s32Ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 3);
    if (MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_SetChnOutputPortDepth err:%x, chn:%d,port:%d\n", s32Ret,
            stChnPort.u32ChnId, stChnPort.u32PortId);
        return NULL;
    }

    sprintf(szFileName, "divp%d.es", stChnPort.u32ChnId);
    printf("start to record %s\n", szFileName);
    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        ST_ERR("create %s fail\n", szFileName);
    }

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);

        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;

        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if(s32Ret < 0)
        {
            ST_ERR("select failed!\n");
            //  usleep(10 * 1000);
            continue;
        }
        else if(s32Ret == 0)
        {
            ST_ERR("get divp frame time out\n");
            //usleep(10 * 1000);
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd, &read_fds))
            {
                s32Ret = MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &stBufHandle);

                if(MI_SUCCESS != s32Ret)
                {
                    //ST_ERR("MI_SYS_ChnOutputPortGetBuf err, %x\n", s32Ret);
                    continue;
                }

                // save one Frame YUV data
                if (fd > 0)
                {
                    if(_bWriteFile)
                    {
                        write(fd, stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0] +
                            stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[1] /2);
                    }

                }

                ++u32GetFramesCount;
                printf("channelId[%u] u32GetFramesCount[%u]\n", stChnPort.u32ChnId, u32GetFramesCount);

                MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            }
        }
    }

    if (fd > 0)
    {
        close(fd);
        fd = -1;
    }

    printf("exit record\n");
    return NULL;
}

void ST_DefaultConfig(ST_Config_S *pstConfig)
{
    pstConfig->s32UseOnvif     = 0;
    pstConfig->s32UseVdf    = 0;
    pstConfig->s32LoadIQ    = 0;
    pstConfig->s32HDRtype    = 0;
    pstConfig->enSensorType = ST_Sensor_Type_IMX291;
    pstConfig->enRotation = E_MI_SYS_ROTATE_NONE;
}

void ST_DefaultArgs(ST_Config_S *pstConfig)
{
    memset(pstConfig, 0, sizeof(ST_Config_S));
    ST_DefaultConfig(pstConfig);

    pstConfig->s32UseOnvif = 0;
    pstConfig->s32UseVdf = 0;
    pstConfig->s32LoadIQ = 0;
    pstConfig->enRotation = E_MI_SYS_ROTATE_NONE;
    pstConfig->en3dNrLevel = E_MI_VPE_3DNR_LEVEL1;
    pstConfig->s8SnrResIndex = -1;
}

void ST_HandleSig(MI_S32 signo)
{
    if(signo == SIGINT)
    {
        ST_INFO("catch Ctrl + C, exit normally\n");

        g_bExit = TRUE;
    }
}

static MI_S32 UVC_Init(void *uvc)
{
    return MI_SUCCESS;
}

static MI_S32 UVC_Deinit(void *uvc)
{
    return MI_SUCCESS;
}

static ST_UvcDev_t * Get_UVC_Device(void *uvc)
{
    ST_UVC_Device_t *pdev = (ST_UVC_Device_t*)uvc;

    for (int i = 0; i < g_UvcSrc->devnum;i++)
    {
        ST_UvcDev_t *dev = &g_UvcSrc->dev[i];
        if (!strcmp(dev->name, pdev->name))
        {
            return dev;
        }
    }
    return NULL;
}

static MI_S32 UVC_UP_FinishBuffer(void *uvc,ST_UVC_BufInfo_t *bufInfo)
{
    ST_UvcDev_t *dev = Get_UVC_Device(uvc);
    MI_S32 s32Ret = MI_SUCCESS;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_SYS_BUF_HANDLE stBufHandle = 0;
    VENC_STREAMS_t * pUserptrStream = NULL;
    MI_U32 VencChn = pstStreamAttr[dev->dev_index].vencChn;

    if (!dev)
        return -1;

    switch(dev->setting.fcc) {
        case V4L2_PIX_FMT_YUYV:
            stBufHandle = bufInfo->b.handle;

            s32Ret = MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            if(MI_SUCCESS!=s32Ret)
            {
                printf("%s Release Frame Failed\n", __func__);
                return s32Ret;
            }
            break;
        case V4L2_PIX_FMT_NV12:
        case V4L2_PIX_FMT_GREY:
            stBufHandle = bufInfo->b.handle;

            s32Ret = MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            if(MI_SUCCESS!=s32Ret)
            {
                printf("%s Release Frame Failed\n", __func__);
                return s32Ret;
            }
            break;
        case V4L2_PIX_FMT_MJPEG:
        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
            pUserptrStream = (VENC_STREAMS_t*)bufInfo->b.handle;

            s32Ret = MI_VENC_ReleaseStream(VencChn, &pUserptrStream->stStream);
            if (MI_SUCCESS != s32Ret)
            {
                printf("%s Release Frame Failed\n", __func__);
                return s32Ret;
            }

            pUserptrStream->used = false;
            break;
    }
    return MI_SUCCESS;
}

static MI_S32 UVC_UP_FillBuffer(void *uvc,ST_UVC_BufInfo_t *bufInfo)
{
    static int flag = 0;
    ST_UvcDev_t * dev = Get_UVC_Device(uvc);
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_SYS_ChnPort_t dstChnPort;
    VENC_STREAMS_t * pUserptrStream = NULL;
    MI_VENC_Stream_t * pstStream = NULL;
    MI_VENC_Pack_t stPack[4];
    MI_VENC_ChnStat_t stStat;
    MI_U32 VencChn = pstStreamAttr[dev->dev_index].vencChn;

    if (!dev)
        return -1;

    dstChnPort = dev->setting.dstChnPort;
    switch(dev->setting.fcc) {
        case V4L2_PIX_FMT_YUYV:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                return -EINVAL;

            bufInfo->b.start = (long unsigned int)stBufInfo.stFrameData.pVirAddr[0];
            bufInfo->length = stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0];

            bufInfo->b.handle = (long unsigned int)stBufHandle;
            break;
        case V4L2_PIX_FMT_NV12:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                return -EINVAL;

            bufInfo->b.start = (long unsigned int)stBufInfo.stFrameData.pVirAddr[0];
            bufInfo->length = stBufInfo.stFrameData.u16Height
                     * (stBufInfo.stFrameData.u32Stride[0] + stBufInfo.stFrameData.u32Stride[1] / 2);
            bufInfo->b.handle = (long unsigned int)stBufHandle;
            break;
        case V4L2_PIX_FMT_GREY:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                return -EINVAL;

            bufInfo->b.start = (long unsigned int)stBufInfo.stFrameData.pVirAddr[0];
            bufInfo->length = stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u16Width;
            bufInfo->b.handle = (long unsigned int)stBufHandle;

            bufInfo->ir_status = flag % 2;
            flag++;
            break;
        case V4L2_PIX_FMT_MJPEG:
        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
            for (int i = 0;i < g_maxbuf_cnt; i++)
            {
                if (!dev->res.pstuserptr_stream[i].used)
                {
                    pUserptrStream = &dev->res.pstuserptr_stream[i];
                    break;
                }
            }
            if (!pUserptrStream)
            {
                return -EINVAL;
            }
            pstStream = &pUserptrStream->stStream;

            memset(&stPack, 0, sizeof(MI_VENC_Pack_t) * 4);
            pstStream->pstPack = stPack;

            s32Ret = MI_VENC_Query(VencChn, &stStat);
            if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
                return -EINVAL;

            pstStream->u32PackCount = 1;//only need 1 packet

            s32Ret = MI_VENC_GetStream(VencChn, pstStream, 40);
            if (MI_SUCCESS != s32Ret)
                return -EINVAL;

            bufInfo->b.start = (long unsigned int)pstStream->pstPack[0].pu8Addr;
            bufInfo->length = pstStream->pstPack[0].u32Len;

            bufInfo->b.handle = (long unsigned int)pUserptrStream;
            pUserptrStream->used = true;

            if (g_reqIdr_cnt++<5)
            {
                MI_VENC_RequestIdr(VencChn, TRUE);
            }
            break;
        default:
            return -EINVAL;
    }
    return MI_SUCCESS;
}

static MI_S32 UVC_MM_FillBuffer(void *uvc,ST_UVC_BufInfo_t *bufInfo)
{
    static int flag = 0;
    ST_UvcDev_t * dev = Get_UVC_Device(uvc);
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32Size, i;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_SYS_ChnPort_t dstChnPort;
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack[4];
    MI_VENC_ChnStat_t stStat;

    MI_U8 *u8CopyData = (MI_U8 *)bufInfo->b.buf;
    MI_U32 *pu32length = (MI_U32 *)&bufInfo->length;
    MI_U32 VencChn = pstStreamAttr[dev->dev_index].vencChn;

    if (!dev)
        return -1;

    dstChnPort = dev->setting.dstChnPort;

    switch(dev->setting.fcc) {
        case V4L2_PIX_FMT_YUYV:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
            {
                return -EINVAL;
            }

            *pu32length = stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0];
            memcpy(u8CopyData, stBufInfo.stFrameData.pVirAddr[0], *pu32length);

            s32Ret = MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                printf("%s Release Frame Failed\n", __func__);

            break;
        case V4L2_PIX_FMT_NV12:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                return -EINVAL;

            *pu32length = stBufInfo.stFrameData.u16Height
                    * (stBufInfo.stFrameData.u32Stride[0] + stBufInfo.stFrameData.u32Stride[1] / 2);
            memcpy(u8CopyData, stBufInfo.stFrameData.pVirAddr[0],
                            stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0]);
            u8CopyData += stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0];
            memcpy(u8CopyData, stBufInfo.stFrameData.pVirAddr[1],
                            stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[1]/2);

            s32Ret = MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                printf("%s Release Frame Failed\n", __func__);
            break;
        case V4L2_PIX_FMT_GREY:
            memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
            memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

            s32Ret = MI_SYS_ChnOutputPortGetBuf(&dstChnPort, &stBufInfo, &stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                return -EINVAL;

            *pu32length = stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u16Width;
            memcpy(u8CopyData, stBufInfo.stFrameData.pVirAddr[0],
                            stBufInfo.stFrameData.u16Height * stBufInfo.stFrameData.u32Stride[0]);

            bufInfo->ir_status = flag % 2;
            flag++;

            s32Ret = MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            if(MI_SUCCESS!=s32Ret)
                printf("%s Release Frame Failed\n", __func__);
            break;




        case V4L2_PIX_FMT_MJPEG:
        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
            memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
            memset(&stPack, 0, sizeof(MI_VENC_Pack_t) * 4);
            stStream.pstPack = stPack;

            s32Ret = MI_VENC_Query(VencChn, &stStat);
            if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
                return -EINVAL;

            stStream.u32PackCount = stStat.u32CurPacks;

            s32Ret = MI_VENC_GetStream(VencChn, &stStream, 40);
            if (MI_SUCCESS != s32Ret)
                return -EINVAL;

            for(i = 0;i < stStat.u32CurPacks; i++)
            {
                u32Size = stStream.pstPack[i].u32Len;
                memcpy(u8CopyData,stStream.pstPack[i].pu8Addr, u32Size);
                u8CopyData += u32Size;
            }
            *pu32length = u8CopyData - (MI_U8 *)bufInfo->b.buf;
            bufInfo->is_tail = true;//default is frameEnd
            s32Ret = MI_VENC_ReleaseStream(VencChn, &stStream);
            if (MI_SUCCESS != s32Ret)
                printf("%s Release Frame Failed\n", __func__);

            if (g_reqIdr_cnt++<5)
            {
 //               MI_VENC_RequestIdr(VencChn, TRUE);
            }
            break;
        default:
            printf("unknown format %d\n", dev->setting.fcc);
            return -EINVAL;
    }
    return MI_SUCCESS;
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

    MI_ISP_AE_FLICKER_TYPE_e Flicker= SS_AE_FLICKER_TYPE_50HZ;
    MI_ISP_AE_SetFlicker(0, &Flicker);

    printf("MI_ISP_AE_SetFlicker!\n");
    return 0;
}


static MI_S32 UVC_StartCapture(void *uvc,Stream_Params_t format)
{
    ST_UvcDev_t *dev = Get_UVC_Device(uvc);
    ST_Stream_Attr_T *pstStreamAttr;
    MI_SYS_ChnPort_t *dstChnPort;
    ST_Sys_BindInfo_T stBindInfo;
#if ENABLE_SSNN_FD
    ST_Sys_BindInfo_T stDivpBindInfo;
#endif
    MI_U32 u32Width, u32Height;

    /************************************************
    Step0:  Initial general param
    *************************************************/
    if (!dev)
        return -1;

    memset(&dev->setting, 0x00, sizeof(dev->setting));
    dev->setting.fcc = format.fcc;
    dev->setting.u32Width = format.width;
    dev->setting.u32Height = format.height;
    dev->setting.u32FrameRate = format.frameRate;

    dstChnPort = &dev->setting.dstChnPort;
    pstStreamAttr = g_stStreamAttr;
    u32Width = dev->setting.u32Width;;
    u32Height = dev->setting.u32Height;;

    /************************************************
    Step1:  start VPE port
    *************************************************/
    MI_U32 VpeChn = pstStreamAttr[dev->dev_index].u32InputChn;
    MI_U32 VpePortId = pstStreamAttr[dev->dev_index].u32InputPort;
    ST_VPE_PortInfo_T stVpePortInfo;

    stVpePortInfo.DepVpeChannel   = VpeChn;
    stVpePortInfo.u16OutputWidth  = u32Width;
    stVpePortInfo.u16OutputHeight = u32Height;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;

    /************************************************
    Step2: Init Venc Option
    ************************************************/
    MI_U32 VencChn = pstStreamAttr[dev->dev_index].vencChn;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_U32  u32VenBitRate =0;
    MI_U32 VencDev = 0;
    bool bByFrame = true;

    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));

    /************************************************
    Step3: Init Bind Option
    ************************************************/
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = VpeChn;
    stBindInfo.stSrcChnPort.u32PortId = VpePortId;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = dev->setting.u32FrameRate;
    stBindInfo.u32DstFrmrate = dev->setting.u32FrameRate;

    if (dev->setting.fcc != V4L2_PIX_FMT_MJPEG &&
        dev->setting.fcc != V4L2_PIX_FMT_H264 &&
        dev->setting.fcc != V4L2_PIX_FMT_MJPEG)
    {
        stBindInfo.eBindType = pstStreamAttr[dev->dev_index].eBindType;
        stBindInfo.u32BindParam = u32Height;
    }
    else
    {
        stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
        stBindInfo.u32BindParam = 0;
    }

    if (u32Width * u32Height > 1920 *1080)
    {
        u32VenBitRate = 1024 * 1024 * 4;
    }
    else if(u32Width * u32Height < 640*480)
    {
        u32VenBitRate = 1024 * 500;
    }
    else
    {
        u32VenBitRate = 1024 * 1024 * 2;
    }

    if (g_bitrate[dev->dev_index])
        u32VenBitRate = g_bitrate[dev->dev_index] * 1024 * 1024;

    if (!dev->res.pstuserptr_stream)
    {
        dev->res.pstuserptr_stream = (VENC_STREAMS_t*)calloc(g_maxbuf_cnt, sizeof(VENC_STREAMS_t));
    }

    g_reqIdr_cnt = 0;

    switch(dev->setting.fcc) {
        case V4L2_PIX_FMT_YUYV:
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            STCHECKRESULT(ST_Vpe_StartPort(VpePortId, &stVpePortInfo));

            *dstChnPort = stBindInfo.stSrcChnPort;
            if(stVpePortInfo.u16OutputWidth < 1280)
                STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(dstChnPort, 2, 3));
            else 
                STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(dstChnPort, 1, 2));
            break;
        case V4L2_PIX_FMT_NV12:
        case V4L2_PIX_FMT_GREY:
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(ST_Vpe_StartPort(VpePortId, &stVpePortInfo));

            *dstChnPort = stBindInfo.stSrcChnPort;

            if(stVpePortInfo.u16OutputWidth < 1280)
                STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(dstChnPort, 2, 3));
            else 
                STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(dstChnPort, 1, 2));
            break;
        case V4L2_PIX_FMT_MJPEG:
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(ST_Vpe_StartPort(VpePortId, &stVpePortInfo));

            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
            stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateDen = 1;
            stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = u32Width;
            stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = u32Height;
            stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth =  u32Width;
            stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = ALIGN_UP(u32Height, 16);
            stChnAttr.stVeAttr.stAttrJpeg.bByFrame = true;
            stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;

            STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));

            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            STCHECKRESULT(MI_VENC_SetMaxStreamCnt(VencChn, g_maxbuf_cnt+2));
            STCHECKRESULT(ST_Venc_StartChannel(VencChn));

            *dstChnPort = stBindInfo.stDstChnPort;
            break;
        case V4L2_PIX_FMT_H264:
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(ST_Vpe_StartPort(VpePortId, &stVpePortInfo));

            stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = u32Width;
            stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = u32Height;
            stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = u32Width;
            stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = u32Height;
            stChnAttr.stVeAttr.stAttrH264e.bByFrame = bByFrame;
            stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
            stChnAttr.stVeAttr.stAttrH264e.u32Profile = 1;

            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = u32VenBitRate;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 90;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
            stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_H264E;
            STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));
           {
                MI_VENC_ParamH264SliceSplit_t stSliceSplit = {true, 17};
                MI_VENC_SetH264SliceSplit(VencChn, &stSliceSplit);
            }
            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            STCHECKRESULT(MI_VENC_SetMaxStreamCnt(VencChn, g_maxbuf_cnt+2));
            STCHECKRESULT(ST_Venc_StartChannel(VencChn));

            *dstChnPort = stBindInfo.stDstChnPort;
            break;
        case V4L2_PIX_FMT_H265:
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(ST_Vpe_StartPort(VpePortId, &stVpePortInfo));

            stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = u32Width;
            stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = u32Height;
            stChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth = u32Width;
            stChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight = u32Height;
            stChnAttr.stVeAttr.stAttrH265e.bByFrame = bByFrame;

            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265CBR;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = u32VenBitRate;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32Gop = 90;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32FluctuateLevel = 0;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32StatTime = 0;
            stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_H265E;
            STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));

            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            STCHECKRESULT(MI_VENC_SetMaxStreamCnt(VencChn, g_maxbuf_cnt+2));
            STCHECKRESULT(ST_Venc_StartChannel(VencChn));

            *dstChnPort = stBindInfo.stDstChnPort;
            break;
        default:
            return -EINVAL;
    }

#if ENABLE_SSNN_FD
    int index = 0;
    if(gbSsnnFdRun == false)
    {
        MI_DIVP_CHN u32ChnId =  pstStreamAttr[dev->dev_index].divpChn;
        MI_DIVP_ChnAttr_t stDivpChnAttr;
        MI_DIVP_OutputPortAttr_t stDivpOutputPortAttr;
        memset(&stDivpChnAttr,0,sizeof(MI_DIVP_ChnAttr_t));
        memset(&stDivpOutputPortAttr,0,sizeof(MI_DIVP_OutputPortAttr_t));
        stDivpChnAttr.bHorMirror = false;
        stDivpChnAttr.bVerMirror = false;
        stDivpChnAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
        stDivpChnAttr.eRotateType = E_MI_SYS_ROTATE_NONE;
        stDivpChnAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
        stDivpChnAttr.stCropRect.u16X = 0;
        stDivpChnAttr.stCropRect.u16Y = 0;
        stDivpChnAttr.stCropRect.u16Width = u32Width;
        stDivpChnAttr.stCropRect.u16Height = u32Height;
        stDivpChnAttr.u32MaxWidth = u32Width;
        stDivpChnAttr.u32MaxHeight = u32Height;
        MI_DIVP_CreateChn(u32ChnId,&stDivpChnAttr);

        MI_DIVP_GetChnAttr(u32ChnId,&stDivpChnAttr);
        stDivpChnAttr.stCropRect.u16X = 0;
        stDivpChnAttr.stCropRect.u16Y = 0;
        stDivpChnAttr.stCropRect.u16Width = u32Width;
        stDivpChnAttr.stCropRect.u16Height = u32Height;
        MI_DIVP_SetChnAttr(u32ChnId,&stDivpChnAttr);
        stDivpOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
        stDivpOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_ARGB8888;
        stDivpOutputPortAttr.u32Width =SSNN_FD_INPUT_W;
        stDivpOutputPortAttr.u32Height = SSNN_FD_INPUT_H;
        g_divp_in_width = u32Width;
        g_divp_in_height = u32Height;

        MI_DIVP_SetOutputPortAttr(u32ChnId,&stDivpOutputPortAttr);
        MI_DIVP_StartChn(u32ChnId);

        memset(&stDivpBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stDivpBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stDivpBindInfo.stSrcChnPort.u32DevId = 0;
        stDivpBindInfo.stSrcChnPort.u32ChnId = VpeChn;
        stDivpBindInfo.stSrcChnPort.u32PortId = VpePortId;

        stDivpBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
        stDivpBindInfo.stDstChnPort.u32ChnId = pstStreamAttr[dev->dev_index].divpChn;
        stDivpBindInfo.stDstChnPort.u32PortId = pstStreamAttr[dev->dev_index].divpPort;

        stDivpBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
        stDivpBindInfo.u32SrcFrmrate = dev->setting.u32FrameRate;
        stDivpBindInfo.u32DstFrmrate = dev->setting.u32FrameRate;

        STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stDivpBindInfo.stDstChnPort, 1, 2));
        STCHECKRESULT(ST_Sys_Bind(&stDivpBindInfo));

        gbSsnnFdRun = true;
        index = dev->dev_index;
        pthread_create(&SsnnFdTid, NULL, ssnnFdThread, (void *)(&index));
        printf("[Fun:%s - Line:%d] Create SsnnFd Init finished.\n", __FUNCTION__, __LINE__);
    }
#endif

    if(g_load_iq_bin)
    {
        ST_DoSetIqBin(0, g_IspBinPath);
        g_load_iq_bin = 0;
    }


     printf("Capture u32Width: %d, u32height: %d, format: %s\n",u32Width,u32Height,
            dev->setting.fcc==V4L2_PIX_FMT_YUYV ? "YUYV":(dev->setting.fcc==V4L2_PIX_FMT_NV12 ? "NV12": (dev->setting.fcc==V4L2_PIX_FMT_GREY ? "GREY":
            (dev->setting.fcc==V4L2_PIX_FMT_MJPEG ?"MJPEG":(dev->setting.fcc==V4L2_PIX_FMT_H264 ? "H264":"H265")))));

    return MI_SUCCESS;
}

static MI_S32 UVC_StopCapture(void *uvc)
{
    ST_UvcDev_t *dev = Get_UVC_Device(uvc);
    ST_Sys_BindInfo_T stBindInfo;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_U32 VpeChn, VpePortId, VencDev, VencChn;

    if (!dev)
        return -1;

    /************************************************
    SSNN:  Release
    *************************************************/
#if ENABLE_SSNN_FD

    if(gbSsnnFdRun == true)
    {
        /************************************************
        Step0: Exit the SSNN FD thread
        *************************************************/
        gbSsnnFdRun = false;
        pthread_join(SsnnFdTid, NULL);
        printf("join SsnnFdTid thread done.\n");

        /************************************************
        Step1: UnBind VPE & DIVP
        *************************************************/
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr[dev->dev_index].u32InputChn;
        stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[dev->dev_index].u32InputPort;

        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
        stBindInfo.stDstChnPort.u32ChnId = pstStreamAttr[dev->dev_index].divpChn;
        stBindInfo.stDstChnPort.u32PortId = pstStreamAttr[dev->dev_index].divpPort;

        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = dev->setting.u32FrameRate;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

        /************************************************
        Step2: Stop & destroy DIVP channel
        *************************************************/
        STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 0, 0));
        STCHECKRESULT(MI_DIVP_StopChn(pstStreamAttr[dev->dev_index].divpChn));
        STCHECKRESULT(MI_DIVP_DestroyChn(pstStreamAttr[dev->dev_index].divpChn));

        g_divp_in_width = 0;
        g_divp_in_height = 0;

        printf("[Fun:%s - Line:%d] SSNN FD release resources successfully\n", __FUNCTION__, __LINE__);
    }

#endif

    /************************************************
    Step0:  General Param Set
    *************************************************/
    VpeChn = pstStreamAttr[dev->dev_index].u32InputChn;
    VpePortId = pstStreamAttr[dev->dev_index].u32InputPort,
    VencChn = pstStreamAttr[dev->dev_index].vencChn,

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = VpeChn;
    stBindInfo.stSrcChnPort.u32PortId = VpePortId;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = dev->setting.u32FrameRate;

    if (dev->res.pstuserptr_stream)
    {
        free(dev->res.pstuserptr_stream);
        dev->res.pstuserptr_stream = NULL;
    }

    /************************************************
    Step1:  Stop All Other Modules
    *************************************************/
    switch(dev->setting.fcc) {
        case V4L2_PIX_FMT_YUYV:
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            break;
        case V4L2_PIX_FMT_NV12:
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            break;
        case V4L2_PIX_FMT_GREY:
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            break;
        case V4L2_PIX_FMT_MJPEG:
            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            STCHECKRESULT(ST_Venc_StopChannel(VencChn));
            STCHECKRESULT(ST_Venc_DestoryChannel(VencChn));
            break;
        case V4L2_PIX_FMT_H264:
            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            STCHECKRESULT(ST_Venc_StopChannel(VencChn));
            STCHECKRESULT(ST_Venc_DestoryChannel(VencChn));
            break;
        case V4L2_PIX_FMT_H265:
            STCHECKRESULT(MI_VENC_GetChnDevid(VencChn, &VencDev));
            stBindInfo.stDstChnPort.u32DevId = VencDev;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
            STCHECKRESULT(ST_Vpe_StopPort(VpeChn, VpePortId));
            STCHECKRESULT(ST_Venc_StopChannel(VencChn));
            STCHECKRESULT(ST_Venc_DestoryChannel(VencChn));
            break;
        default:
            return -EINVAL;
    }

    return MI_SUCCESS;
}

MI_S32 ST_UvcDeinit()
{
    if (!g_UvcSrc)
        return -1;

    for (int i = 0; i < g_UvcSrc->devnum; i++)
    {
        ST_UvcDev_t *dev = &g_UvcSrc->dev[i];
        STCHECKRESULT(ST_UVC_StopDev((dev->handle)));
        STCHECKRESULT(ST_UVC_DestroyDev(dev->handle));
        STCHECKRESULT(ST_UVC_Uninit(dev->handle));
    }
    return MI_SUCCESS;
}


MI_S32 ST_UvcInitDev(ST_UvcDev_t *dev, MI_U32 maxpacket, MI_U8 mult, MI_U8 burst, MI_U8 c_intf, MI_U8 s_intf, MI_S32 mode)
{
    ST_UVC_Setting_t pstSet={g_maxbuf_cnt, maxpacket, mult, burst, c_intf, s_intf, (UVC_IO_MODE_e)mode, USB_ISOC_MODE};
    ST_UVC_MMAP_BufOpts_t m = {UVC_MM_FillBuffer};
    ST_UVC_USERPTR_BufOpts_t u = {UVC_UP_FillBuffer, UVC_UP_FinishBuffer};

    ST_UVC_OPS_t fops = { UVC_Init,
                          UVC_Deinit,
                          {{}},
                          UVC_StartCapture,
                          UVC_StopCapture};
    if (mode==UVC_MEMORY_MMAP)
        fops.m = m;
    else
        fops.u = u;

    printf(ASCII_COLOR_YELLOW "ST_UvcInitDev: name:%s bufcnt:%d mult:%d burst:%d ci:%d si:%d, Mode:%s" ASCII_COLOR_END "\n",
                    dev->name, g_maxbuf_cnt, mult, burst, c_intf, s_intf, mode==UVC_MEMORY_MMAP?"mmap":"userptr");

    ST_UVC_ChnAttr_t pstAttr ={pstSet,fops};
    STCHECKRESULT(ST_UVC_Init(dev->name, dev->dev_index, &dev->handle));
    STCHECKRESULT(ST_UVC_CreateDev(dev->handle, &pstAttr));
    STCHECKRESULT(ST_UVC_StartDev(dev->handle));
    return MI_SUCCESS;
}

MI_S32 ST_UvcInit(MI_S32 devnum, MI_U32 *maxpacket, MI_U8 *mult, MI_U8 *burst, MI_U8 *intf, MI_S32 mode)
{
    char devnode[20] = "/dev/video0";

    if (devnum > MAX_UVC_DEV_NUM)
    {
        printf(ASCII_COLOR_YELLOW "%s Max Uvc Dev Num %d\n" ASCII_COLOR_END "\n", __func__, MAX_UVC_DEV_NUM);
        devnum = MAX_UVC_DEV_NUM;
    }

    g_UvcSrc = (ST_UvcSrc_t*)malloc(sizeof(g_UvcSrc) + sizeof(ST_UvcDev_t) * devnum);
    memset(g_UvcSrc, 0, sizeof(g_UvcSrc) + sizeof(ST_UvcDev_t) * devnum);
    g_UvcSrc->devnum = devnum;

    for (int i = 0; i < devnum; i++)
    {
        ST_UvcDev_t *dev = &g_UvcSrc->dev[i];
        sprintf(devnode, "/dev/video%d", i);
        dev->dev_index = i;
        memcpy(dev->name, devnode, sizeof(devnode));
        ST_UvcInitDev(dev, maxpacket[i], mult[i], burst[i], intf[2*i], intf[2*i+1], mode);
    }
    return MI_SUCCESS;
}

void ST_DoExitProc(void *args)
{
    g_bExit = TRUE;
}

static void help_message(char **argv)
{
    printf("\n");
    printf("usage: %s \n", argv[0]);
    printf(" -a set sensor pad\n");
    printf(" -A set sensor resolution index\n");
    printf(" -b bitrate\n");
    printf(" -B burst -m mult -p maxpacket\n");
    printf(" -M 0:mmap, 1:userptr\n");
    printf(" -N num of uvc stream\n");
    printf(" -i set iq api.bin,ex: -i \"/customer/imx415_api.bin\" \n");
    printf(" -I c_intf,s_intf\n");
    printf("    c_intf: control interface\n");
    printf("    s_intf: streaming interface\n");
    printf(" -t Trace level (0-6) \n");
    printf(" -q open iqserver\n");
    printf(" -h help message\n");
    printf("multi stram param, using ',' to seperate\n");
    printf("\nExample: %s -N2 -m0,0 -M1 -I0,1,2,3\n", argv[0]);
    printf("\n");
}

#define PARSE_PARM(instance)\
{\
    int i = 0; \
    do { \
        if (!i) {\
            p = strtok(optarg, ","); \
        } \
        if (p) {\
            instance[i++] = atoi(p); \
        } \
    } while((p = strtok(NULL, ","))!=NULL); \
}

int main(int argc, char **argv)
{
    char *p;
    MI_U32 maxpacket[MAX_UVC_DEV_NUM] = {1024, 1024,192};
    MI_U8 mult[MAX_UVC_DEV_NUM] = {2, 2, 0},
          burst[MAX_UVC_DEV_NUM] = {0, 0, 0};
    MI_U8 intf[2 * MAX_UVC_DEV_NUM] = {0};
    MI_S32 result = 0, mode = UVC_MEMORY_MMAP;
    MI_S32 trace_level = UVC_DBG_ERR;
    struct sigaction sigAction;
    sigAction.sa_handler = ST_HandleSig;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    sigaction(SIGINT, &sigAction, NULL);

    ST_DefaultArgs(&g_stConfig);
//set bitrate
    while((result = getopt(argc, argv, "a:A:b:B:M:N:m:p:I:t:d:i:qh")) != -1)
    {
        switch(result)
        {
            case 'a':
                g_stConfig.u8SnrPad  = strtol(optarg, NULL, 10);
                break;

            case 'A':
                g_stConfig.s8SnrResIndex = strtol(optarg, NULL, 10);
                break;

            case 'b':
                PARSE_PARM(g_bitrate);
                break;
            case 'B':
                PARSE_PARM(burst);
                break;
            case 'm':
                PARSE_PARM(mult);
                break;
            case 'M':
                mode = strtol(optarg, NULL, 10);
                break;
            case 'N':
                g_device_num = strtol(optarg, NULL, 10);
                break;
            case 'p':
                PARSE_PARM(maxpacket);
                break;
            case 'I':
                PARSE_PARM(intf);
                break;
            case 't':
                trace_level = strtol(optarg, NULL, 10);
                break;
            case 'q':
                g_enable_iqserver = TRUE;
                break;
            case 'h':
                help_message(argv);
                return 0;
            case 'd':
                bEnableAI = TRUE;
                AiDevId = strtol(optarg, NULL, 10);
                printf("AiDevId:%d\n",AiDevId);
            case 'i':
                strcpy(g_IspBinPath, optarg);
                ST_DBG("g_IspBinPath:%s\n", g_IspBinPath);
                g_load_iq_bin = TRUE;
                break;
            default: break;
        }
    }
    STCHECKRESULT(ST_Sys_Init());
    if(bEnableAI == TRUE)
    {
        STCHECKRESULT(ST_AudioModuleInit());
    }
    STCHECKRESULT(ST_BaseModuleInit(&g_stConfig, g_device_num));
    ST_UVC_SetTraceLevel(trace_level);
    ST_UvcInit(g_device_num, maxpacket, mult, burst, intf, mode);

    while(!g_bExit)
    {
        usleep(100 * 1000);
    }
    usleep(100 * 1000);
    ST_UvcDeinit();
    STCHECKRESULT(ST_BaseModuleUnInit());
    if(bEnableAI == TRUE)
    {
        STCHECKRESULT(ST_AudioModuleUnInit());
    }
    STCHECKRESULT(ST_Sys_Exit());

    return 0;
}

