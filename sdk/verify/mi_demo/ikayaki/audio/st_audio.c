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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "cJSON.h"

#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_ai.h"
#include "mi_ao.h"

#define AI_VOLUME_AMIC_ANALOG_MIN   (0)
#define AI_VOLUME_AMIC_ANALOG_MAX   (21)
#define AI_VOLUME_LINEIN_ANALOG_MAX (7)

#define AI_VOLUME_DIGITAL_MIN       (-60)
#define AI_VOLUME_DIGITAL_MAX       (30)

#define AI_VOLUME_DMIC_2CHN_MIN     (-60)
#define AI_VOLUME_DMIC_2CHN_MAX     (30)

#define AI_VOLUME_DMIC_4CHN_MIN     (0)
#define AI_VOLUME_DMIC_4CHN_MAX     (4)

#define AO_VOLUME_MIN               (-60)
#define AO_VOLUME_MAX               (30)

#define WAV_PCM                     (0x1)

#define MI_AUDIO_SAMPLE_PER_FRAME   (256)

#define AI_DMA_BUFFER_MAX_SIZE      (256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE      (128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE      (64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE      (256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE      (128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE      (64 * 1024)

#define MIU_WORD_BYTE_SIZE          (8)
#define USER_BUF_DEPTH              (4)
#define TOTAL_BUF_DEPTH             (8)

#define JSON_BUF_SIZE               (30 * 1024)

#define AI_RUNNING_MAX_DEV_NUM      (2)
#define AO_RUNNING_MAX_DEV_NUM      (2)

#define ExecFunc(func, _ret_) \
do{ \
    MI_S32 s32TmpRet;   \
    printf("%d Start test: %s\n", __LINE__, #func);\
    s32TmpRet = func;   \
    if (s32TmpRet != _ret_)\
    {\
        printf("AUDIO_TEST [%d] %s exec function failed, result:0x%x\n",__LINE__, #func, s32TmpRet);\
        return 1;\
    }\
    else\
    {\
        printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);\
    }\
    printf("%d End test: %s\n", __LINE__, #func);   \
}while(0);

/*=============================================================*/
// Global Variable definition
/*=============================================================*/
typedef enum
{
    E_AI_SOUND_MODE_MONO = 0,
    E_AI_SOUND_MODE_STEREO,
    E_AI_SOUND_MODE_QUEUE,
} AiSoundMode_e;

typedef enum
{
    E_SOUND_MODE_MONO = 0, /* mono */
    E_SOUND_MODE_STEREO = 1, /* stereo */
} SoundMode_e;

typedef enum
{
    E_SAMPLE_RATE_8000 = 8000, /* 8kHz sampling rate */
    E_SAMPLE_RATE_16000 = 16000, /* 16kHz sampling rate */
    E_SAMPLE_RATE_32000 = 32000, /* 32kHz sampling rate */
    E_SAMPLE_RATE_48000 = 48000, /* 48kHz sampling rate */
} SampleRate_e;

typedef enum
  {
      E_AENC_TYPE_G711A = 0,
      E_AENC_TYPE_G711U,
      E_AENC_TYPE_G726_16,
      E_AENC_TYPE_G726_24,
      E_AENC_TYPE_G726_32,
      E_AENC_TYPE_G726_40,
      PCM,
  } AencType_e;

typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;

typedef struct AiChnPrivateData_s
{
    MI_U32 u32DevIdx;
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;
    MI_S32 s32Fd;
    MI_U32 u32TotalSize;
    MI_S32 s32EchoFd[2];
    MI_U32 u32EchoTotalSize[2];
    MI_AUDIO_Attr_t stDevAttr;
    pthread_t getFrameThreadId;
} AiChnPrivateData_t;

typedef struct AoChnPrivateData_s
{
    MI_U32 u32DevIdx;
    MI_AUDIO_DEV AoDevId;
    MI_S32 s32Fd;
    MI_U32 u32NeedSize;
    pthread_t sendFrameThreadId;
} AoChnPrivateData_t;

typedef struct JsonConfigInfo_s
{
    MI_U8       u8ChipName[128];
    MI_BOOL     bEnableAI;
    MI_BOOL     bEnableAO;
    MI_U32      u32AiDevNumFromConfigFile;
    MI_AUDIO_DEV s32AiDevId[AI_RUNNING_MAX_DEV_NUM];
    MI_AUDIO_Attr_t stAiAttr[AI_RUNNING_MAX_DEV_NUM];
    MI_U8       u8DumpDataPath[256];
    MI_S32      s32AiVolume[AO_RUNNING_MAX_DEV_NUM][2];
    MI_U32      u32AoDevNumFromConfigFile;
    MI_AUDIO_DEV s32AoDevId[AI_RUNNING_MAX_DEV_NUM];
    MI_AUDIO_Attr_t stAoAttr[AO_RUNNING_MAX_DEV_NUM];
    MI_U8       u8PlayBackPath[AO_RUNNING_MAX_DEV_NUM][256];
    MI_S32      s32AoVolume[AO_RUNNING_MAX_DEV_NUM];
}JsonConfigInfo_t;

typedef struct CreateDataFile_s
{
    MI_AUDIO_DEV AiDevId;
    MI_U32 u32ChnIdx;
    MI_U8* pu8ChipName;
    MI_AUDIO_Attr_t* pstAttr;
    MI_U8* pu8OutputPath;
    MI_U8* pu8OutputFileName;
} CreateDataFile_t;

typedef struct CreateSrcFile_s
{
    MI_AUDIO_DEV AiDevId;
    MI_U32 u32ChnIdx;
    MI_U32 u32SrcChnIdx;
    MI_U8* pu8ChipName;
    MI_AUDIO_Attr_t* pstAttr;
    MI_U8* pu8OutputPath;
    MI_U8* pu8OutputFileName;
} CreateSrcFile_t;

static JsonConfigInfo_t info;
static MI_U32   u32RunTime = 0;
static MI_BOOL  bAiExit = FALSE;
static MI_BOOL  bAoExit = FALSE;

static AiChnPrivateData_t stAiChnPriv[AI_RUNNING_MAX_DEV_NUM][MI_AUDIO_MAX_CHN_NUM];
static AoChnPrivateData_t stAoChnPriv[AO_RUNNING_MAX_DEV_NUM];

MI_U8* pu8CustomerConfigJsonFile = NULL;
MI_U8  u8OutputFileName[1024];

void signalHandler(int signo)
{
    switch (signo){
        case SIGALRM:
        case SIGINT:
            printf("Catch signal!!!\n");
            bAiExit = TRUE;
            bAoExit = TRUE;
            break;
   }
   return;
}

void setTimer(MI_U32 u32RunTime)
{
    struct itimerval new_value, old_value;
    if (0 != u32RunTime)
    {
        new_value.it_value.tv_sec = u32RunTime;
        new_value.it_value.tv_usec = 0;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &new_value, &old_value);
    }
    return;
}

MI_BOOL checkCustomerConfigJsonFile(void)
{
    MI_U32 u32Adc01ChannelCount[] = {1, 2};
    MI_U32 u32Dmic4ChnChannelCount[] = {1, 2, 4};
    MI_U32 u32Dmic2ChnChannelCount[] = {1, 2};
    MI_U32 u32I2sRxTdmChannelCount[] = {2, 4, 8};
    MI_U32 u32I2sRxChannelCount[] = {2};
    MI_U32 u32Adc23ChannelCount[] = {1, 2};
    MI_U32 u32Adc2ChannelCount[] = {1};
    MI_U32 u32Amic_DmicAndI2sRxChanneCount[] = {4};
    MI_U32 u32AllAdcChannelCount[] = {4};
    MI_U32 u32SimplePcmRxChannelCount[] = {1};
    MI_U32 u32I2sRxAndSrcChannelCount[] = {2, 4};
    MI_U32 u32AiDevIdMax;
    MI_U32 u32AoDevIdMax;
    //MI_U32 u32AoChannelCount[] = {1, 2};
    MI_U32 u32DevIdx;
    MI_U32 u32ChannelCount;
    MI_BOOL bMatch = FALSE;
    MI_U32 u32ArraySize, u32ArrayIdx;
    MI_U32* pu32DevChnArray = NULL;
    MI_S32 s32Ret;
    MI_S32 s32Fd;
    WaveFileHeader_t stWavHeaderInput;

    if ((0 != strcmp((const char *)(info.u8ChipName), "Mercury6"))
        && (0 != strcmp((const char *)(info.u8ChipName), "Infinity2M"))
        && (0 != strcmp((const char *)(info.u8ChipName), "Infinity6E"))
        && (0 != strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
        && (0 != strcmp((const char *)(info.u8ChipName), "Infinity6"))
        && (0 != strcmp((const char *)(info.u8ChipName), "Pioneer3")))
    {
        printf("failed to check chip name[%s].\n",(info.u8ChipName));
        return FALSE;
    }

    if ((FALSE == info.bEnableAI) && (FALSE == info.bEnableAO))
    {
        printf("AI and AO are not enabled.\n");
        return FALSE;
    }

    if (0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
    {
        u32AiDevIdMax = 5;
        u32AoDevIdMax = 4;
    }
    else if (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
    {
        u32AiDevIdMax = 8;
        u32AoDevIdMax = 7;
    }
    else if (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M"))
    {
        u32AiDevIdMax = 5;
        u32AoDevIdMax = 3;
    }
    else if (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E"))
    {
        u32AiDevIdMax = 4;
        u32AoDevIdMax = 1;
    }
    else if (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
    {
        u32AiDevIdMax = 4;
        u32AoDevIdMax = 1;
    }
    else if (0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
    {
        u32AiDevIdMax = 3;
        u32AoDevIdMax = 2;
    }

    // check Ai param
    if (TRUE == info.bEnableAI)
    {
        if (0 == strcmp("", (const char *)info.u8DumpDataPath))
        {
            printf("invalid dump path[%s].\n", info.u8DumpDataPath);
            return FALSE;
        }

        for (u32DevIdx = 0; u32DevIdx < info.u32AiDevNumFromConfigFile; u32DevIdx++)
        {
            // check device id
            if ((info.s32AiDevId[u32DevIdx] < 0) || (info.s32AiDevId[u32DevIdx] > u32AiDevIdMax))
            {
                printf("invalid AI device id[%d], only support[0, %d] on this chip.\n", info.s32AiDevId[u32DevIdx], u32AiDevIdMax);
                return FALSE;
            }

            if ((E_MI_AUDIO_SAMPLE_RATE_8000 != info.stAiAttr[u32DevIdx].eSamplerate)
                && (E_MI_AUDIO_SAMPLE_RATE_16000 != info.stAiAttr[u32DevIdx].eSamplerate)
                && (E_MI_AUDIO_SAMPLE_RATE_32000 != info.stAiAttr[u32DevIdx].eSamplerate)
                && (E_MI_AUDIO_SAMPLE_RATE_48000 != info.stAiAttr[u32DevIdx].eSamplerate))
            {
                printf("invalid AI sample rate[%d], only support 8000/16000/32000/48000Hz on this chip.\n", info.stAiAttr[u32DevIdx].eSamplerate);
                return FALSE;
            }

            if ((6 == info.s32AiDevId[u32DevIdx])
                && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                if(E_MI_AUDIO_SAMPLE_RATE_48000 == info.stAiAttr[u32DevIdx].eSamplerate)
                {
                    printf("invalid AI sample rate[%d], Dev%d only support 8000/16000/32000Hz on this chip.\n", info.stAiAttr[u32DevIdx].eSamplerate, info.s32AiDevId[u32DevIdx]);
                    return FALSE;
                }
            }

            if (E_MI_AUDIO_BIT_WIDTH_16 != info.stAiAttr[u32DevIdx].eBitwidth)
            {
                printf("invalid bit width, only support 16bit on this chip.\n");
                return FALSE;
            }

            if ((E_MI_AUDIO_SOUND_MODE_MONO != info.stAiAttr[u32DevIdx].eSoundmode)
                && (E_MI_AUDIO_SOUND_MODE_STEREO != info.stAiAttr[u32DevIdx].eSoundmode)
                && (E_MI_AUDIO_SOUND_MODE_QUEUE != info.stAiAttr[u32DevIdx].eSoundmode))
            {
                printf("invalid sound mode[%d].\n", info.stAiAttr[u32DevIdx].eSoundmode);
                return FALSE;
            }

            if ((E_MI_AUDIO_SOUND_MODE_MONO == info.stAiAttr[u32DevIdx].eSoundmode)
                || (E_MI_AUDIO_SOUND_MODE_QUEUE == info.stAiAttr[u32DevIdx].eSoundmode))
            {
                u32ChannelCount = info.stAiAttr[u32DevIdx].u32ChnCnt;
            }
            else if (E_MI_AUDIO_SOUND_MODE_STEREO == info.stAiAttr[u32DevIdx].eSoundmode)
            {
                u32ChannelCount = 2 * info.stAiAttr[u32DevIdx].u32ChnCnt;
            }

            // check channel count
            // ADC0/1
            if ((0 == info.s32AiDevId[u32DevIdx]) || (3 == info.s32AiDevId[u32DevIdx]))
            {
                u32ArraySize = sizeof(u32Adc01ChannelCount) / sizeof(u32Adc01ChannelCount[0]);
                pu32DevChnArray = u32Adc01ChannelCount;
            }
            // Dmic
            else if (1 == info.s32AiDevId[u32DevIdx])
            {
                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                {
                    u32ArraySize = sizeof(u32Dmic4ChnChannelCount) / sizeof(u32Dmic4ChnChannelCount[0]);
                    pu32DevChnArray = u32Dmic4ChnChannelCount;
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    u32ArraySize = sizeof(u32Dmic2ChnChannelCount) / sizeof(u32Dmic2ChnChannelCount[0]);
                    pu32DevChnArray = u32Dmic2ChnChannelCount;
                }
            }
            // I2S RX
            else if ((2 == info.s32AiDevId[u32DevIdx])
                || ((4 == info.s32AiDevId[u32DevIdx])
                    && ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                        || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))))
            {
                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    u32ArraySize = sizeof(u32I2sRxTdmChannelCount) / sizeof(u32I2sRxTdmChannelCount[0]);
                    pu32DevChnArray = u32I2sRxTdmChannelCount;
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                {
                    u32ArraySize = sizeof(u32I2sRxChannelCount) / sizeof(u32I2sRxChannelCount[0]);
                    pu32DevChnArray = u32I2sRxChannelCount;
                }

                if ((4 == info.s32AiDevId[u32DevIdx])
                    && ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                        || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E"))))
                {
                    u32ArraySize = sizeof(u32I2sRxChannelCount) / sizeof(u32I2sRxChannelCount[0]);
                    pu32DevChnArray = u32I2sRxChannelCount;
                }
            }
            // Amic(ADC2/3)
            else if ((4 == info.s32AiDevId[u32DevIdx])
                && ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))))
            {
                if (0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                {
                    u32ArraySize = sizeof(u32Adc23ChannelCount) / sizeof(u32Adc23ChannelCount[0]);
                    pu32DevChnArray = u32Adc23ChannelCount;
                }
                else if (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                {
                    u32ArraySize = sizeof(u32Adc2ChannelCount) / sizeof(u32Adc2ChannelCount[0]);
                    pu32DevChnArray = u32Adc2ChannelCount;
                }
            }
            // Amic/Dmic+I2S RX
            else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity2M"))
                && ((4 == info.s32AiDevId[u32DevIdx]) || (5 == info.s32AiDevId[u32DevIdx])))
            {
                u32ArraySize = sizeof(u32Amic_DmicAndI2sRxChanneCount) / sizeof(u32Amic_DmicAndI2sRxChanneCount[0]);
                pu32DevChnArray = u32Amic_DmicAndI2sRxChanneCount;
            }
            // ADC0/1/2/3
            else if ((5 == info.s32AiDevId[u32DevIdx])
                && ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6")) || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))))
            {
                u32ArraySize = sizeof(u32AllAdcChannelCount) / sizeof(u32AllAdcChannelCount[0]);
                pu32DevChnArray = u32AllAdcChannelCount;
            }
            // DMIC 2CHN
            else if ((6 == info.s32AiDevId[u32DevIdx])
                && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                u32ArraySize = sizeof(u32Dmic2ChnChannelCount) / sizeof(u32Dmic2ChnChannelCount[0]);
                pu32DevChnArray = u32Dmic2ChnChannelCount;
            }
            // Simple Pcm Rx
            else if ((7 == info.s32AiDevId[u32DevIdx])
                && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                u32ArraySize = sizeof(u32SimplePcmRxChannelCount) / sizeof(u32SimplePcmRxChannelCount[0]);
                pu32DevChnArray = u32SimplePcmRxChannelCount;
            }
            else if ((8 == info.s32AiDevId[u32DevIdx])
                && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                u32ArraySize = sizeof(u32I2sRxAndSrcChannelCount) / sizeof(u32I2sRxAndSrcChannelCount[0]);
                pu32DevChnArray = u32I2sRxAndSrcChannelCount;
            }

            for (u32ArrayIdx = 0; u32ArrayIdx < u32ArraySize; u32ArrayIdx++)
            {
                if (u32ChannelCount == pu32DevChnArray[u32ArrayIdx])
                {
                    bMatch = TRUE;
                    break;
                }
            }

            if (FALSE == bMatch)
            {
                printf("invalid channel count[%d], sound mode[%d].\n",
                    info.stAiAttr[u32DevIdx].u32ChnCnt, info.stAiAttr[u32DevIdx].eSoundmode);
                return FALSE;
            }

            // check gain
            // Amic(ADC0/1 ADC2/3 ADC0/1/2/3)
            if ((0 == info.s32AiDevId[u32DevIdx])
                || (((0 == strcmp((const char *)(info.u8ChipName), "Mercury6")) || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
                    && ((4 == info.s32AiDevId[u32DevIdx]) || (5 == info.s32AiDevId[u32DevIdx]))))
            {
                if ((info.s32AiVolume[u32DevIdx][0] < AI_VOLUME_AMIC_ANALOG_MIN) || (info.s32AiVolume[u32DevIdx][0] > AI_VOLUME_AMIC_ANALOG_MAX))
                {
                    printf("invalid analog gain[%d], only support [0, 21] on Mercury6.\n", info.s32AiVolume[u32DevIdx][0]);
                    return FALSE;
                }

                if ((info.s32AiVolume[u32DevIdx][1] < AI_VOLUME_DIGITAL_MIN) || (info.s32AiVolume[u32DevIdx][1] > AI_VOLUME_DIGITAL_MAX))
                {
                    printf("invalid digital gain[%d], only support [-60, 30] on Mercury6.\n", info.s32AiVolume[u32DevIdx][1]);
                    return FALSE;
                }
            }
            // Dmic
            else if (1 == info.s32AiDevId[u32DevIdx])
            {
                if ((info.s32AiVolume[u32DevIdx][0] < AI_VOLUME_DMIC_4CHN_MIN) || (info.s32AiVolume[u32DevIdx][0] > AI_VOLUME_DMIC_4CHN_MAX))
                {
                    printf("invalid analog gain[%d], only support [0, 4] on Mercury6.\n", info.s32AiVolume[u32DevIdx][0]);
                    return FALSE;
                }
            }
            else if ((6 == info.s32AiDevId[u32DevIdx]) && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                if ((info.s32AiVolume[u32DevIdx][0] < AI_VOLUME_DMIC_2CHN_MIN) || (info.s32AiVolume[u32DevIdx][0] > AI_VOLUME_DMIC_2CHN_MAX))
                {
                    printf("invalid analog gain[%d], only support [-60, 30] on Pioneer3.\n", info.s32AiVolume[u32DevIdx][0]);
                    return FALSE;
                }
            }
            // Line in(ADC0/1)
            else if (3 == info.s32AiDevId[u32DevIdx])
            {
                if ((info.s32AiVolume[u32DevIdx][0] < AI_VOLUME_AMIC_ANALOG_MIN) || (info.s32AiVolume[u32DevIdx][0] > AI_VOLUME_LINEIN_ANALOG_MAX))
                {
                    printf("invalid analog gain[%d], only support [0, 7] on Mercury6.\n", info.s32AiVolume[u32DevIdx][0]);
                    return FALSE;
                }

                if ((info.s32AiVolume[u32DevIdx][1] < AI_VOLUME_DIGITAL_MIN) || (info.s32AiVolume[u32DevIdx][1] > AI_VOLUME_DIGITAL_MAX))
                {
                    printf("invalid digital gain[%d], only support [-60, 30] on Mercury6.\n", info.s32AiVolume[u32DevIdx][1]);
                    return FALSE;
                }
            }

            // check I2S RX
            if ((2 == info.s32AiDevId[u32DevIdx])
                || ((4 == info.s32AiDevId[u32DevIdx])
                    && ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0")) || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E"))))
                || (((4 == info.s32AiDevId[u32DevIdx]) || (5 == info.s32AiDevId[u32DevIdx])) && (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                || ((8 == info.s32AiDevId[u32DevIdx]) && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))))
            {
                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_MODE_I2S_MASTER != info.stAiAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_I2S_SLAVE != info.stAiAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_TDM_MASTER != info.stAiAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_TDM_SLAVE != info.stAiAttr[u32DevIdx].eWorkmode))
                    {
                        printf("invalid I2s Rx work mode[%d].\n", info.stAiAttr[u32DevIdx].eWorkmode);
                        return FALSE;
                    }
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M"))
                    || ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")) && (4 == info.s32AiDevId[u32DevIdx]))
                    || (((4 == info.s32AiDevId[u32DevIdx]) || (5 == info.s32AiDevId[u32DevIdx])) && (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M"))))
                {
                    if ((E_MI_AUDIO_MODE_I2S_MASTER != info.stAiAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_I2S_SLAVE != info.stAiAttr[u32DevIdx].eWorkmode))
                    {
                        printf("invalid I2s Rx work mode[%d].\n", info.stAiAttr[u32DevIdx].eWorkmode);
                        return FALSE;
                    }
                }

                if ((E_MI_AUDIO_I2S_FMT_I2S_MSB != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt)
                    && (E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt))
                {
                    printf("invalid I2s Rx Format[%d].\n", info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt);
                    return FALSE;
                }

                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_I2S_MCLK_0 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_12_288M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_16_384M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_18_432M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_24_576M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_24M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_48M != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk))
                    {
                        printf("invalid I2s Rx Mclk[%d].\n", info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk);
                        return FALSE;
                    }
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                {
                    if ((E_MI_AUDIO_I2S_MCLK_0 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk))
                    {
                        printf("invalid I2s Rx Mclk[%d].\n", info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk);
                        return FALSE;
                    }
                }

                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_MODE_TDM_MASTER == info.stAiAttr[u32DevIdx].eWorkmode)
                        || (E_MI_AUDIO_MODE_TDM_SLAVE == info.stAiAttr[u32DevIdx].eWorkmode))
                    {
                        if ((4 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots)
                            && (8 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots))
                        {
                            printf("invalid I2S Rx tdm slot number[%d], only support 4/8 slots on this chip.\n",
                                info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots);
                            return FALSE;
                        }
                    }

                    if ((E_MI_AUDIO_BIT_WIDTH_16 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth)
                        && (E_MI_AUDIO_BIT_WIDTH_32 != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth))
                    {
                        printf("invalid I2S Rx bit width[%d], only support 16/32 bit on this chip\n",
                                info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth);
                        return FALSE;
                    }
                }

                if (((4 == u32ChannelCount) || (8 == u32ChannelCount))
                    && ((E_MI_AUDIO_MODE_TDM_MASTER != info.stAiAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_TDM_SLAVE != info.stAiAttr[u32DevIdx].eWorkmode)))
                {
                    printf("I2s Rx 4/8 channels only support tdm master or tdm slave work mode.\n");
                    return FALSE;
                }
            }

            // check simple pcm rx
            else if ((7 == info.s32AiDevId[u32DevIdx])
                || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                if (E_MI_AUDIO_MODE_I2S_MASTER != info.stAiAttr[u32DevIdx].eWorkmode)
                {
                    printf("Simple Pcm Rx only support i2s master mode.\n");
                    return FALSE;
                }

                if (TRUE != info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock)
                {
                    printf("Simple Pcm Rx only support 4 wire mode.\n");
                    return FALSE;
                }
            }
        }
    }

    // check AO param
    if (TRUE == info.bEnableAO)
    {
        for (u32DevIdx = 0; u32DevIdx < info.u32AoDevNumFromConfigFile; u32DevIdx++)
        {
            if (0 == strcmp("", (const char *)(info.u8PlayBackPath[u32DevIdx])))
            {
                printf("invalid playback path[%s].\n", info.u8PlayBackPath[u32DevIdx]);
                return FALSE;
            }

            s32Fd = open((const char *)(info.u8PlayBackPath[u32DevIdx]), O_RDONLY, 0666);
            if (s32Fd < 0)
            {
                printf("failed to open file[%s] error[%s].\n", (char*)(info.u8PlayBackPath), strerror(errno));
                return FALSE;
            }

            memset(&stWavHeaderInput, 0, sizeof(stWavHeaderInput));
            s32Ret = read(s32Fd, &stWavHeaderInput, sizeof(WaveFileHeader_t));
            if (s32Ret < 0)
            {
                printf("failed to read wav header form file[%s].\n", (char *)(info.u8PlayBackPath));
                close(s32Fd);
                return FALSE;
            }

            switch (stWavHeaderInput.wave.dwSamplesPerSec)
            {
                case 8000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_8000;
                    break;

                case 11025:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_11025;
                    break;

                case 12000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_12000;
                    break;

                case 16000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_16000;
                    break;

                case 22050:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_22050;
                    break;

                case 24000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_24000;
                    break;

                case 32000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_32000;
                    break;

                case 44100:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_44100;
                    break;

                case 48000:
                    info.stAoAttr[u32DevIdx].eSamplerate = E_MI_AUDIO_SAMPLE_RATE_48000;
                    break;

                default:
                    printf("invalid sample rate[%d], only support 8000/11025/12000/16000/22050/24000/32000/44100/48000Hz on this chip.\n",
                        stWavHeaderInput.wave.dwSamplesPerSec);
                    close(s32Fd);
                    return FALSE;
            }

            switch (stWavHeaderInput.wave.wBitsPerSample)
            {
                case 16:
                    info.stAoAttr[u32DevIdx].eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
                    break;

                default:
                    printf("invalid bit width[%d], only support 16bit on this chip.\n", stWavHeaderInput.wave.wBitsPerSample);
                    close(s32Fd);
                    return FALSE;
            }

            switch (stWavHeaderInput.wave.wChannels)
            {
                case 1:
                    info.stAoAttr[u32DevIdx].eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
                    info.stAoAttr[u32DevIdx].u32ChnCnt = 1;
                    break;

                case 2:
                    info.stAoAttr[u32DevIdx].eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
                    info.stAoAttr[u32DevIdx].u32ChnCnt = 1;
                    break;

                default:
                    printf("invalid channel count[%d], only support 1/2 channel on this chip.\n", stWavHeaderInput.wave.wChannels);
                    close(s32Fd);
                    return FALSE;
            }

            close(s32Fd);

            if ((info.s32AoDevId[u32DevIdx] < 0) || (info.s32AoDevId[u32DevIdx] > u32AoDevIdMax))
            {
                printf("invalid AO device id[%d], only support[0, %d] on this chip.\n", info.s32AoDevId[u32DevIdx], u32AoDevIdMax);
                return FALSE;
            }

            if ((info.s32AoVolume[u32DevIdx] < AO_VOLUME_MIN) || (info.s32AoVolume[u32DevIdx] > AO_VOLUME_MAX))
            {
                printf("invalid AO volume[%d], only support[-60, 30] on this chip.\n", info.s32AoVolume[u32DevIdx]);
                return FALSE;
            }

            // DAC0/DAC1
            if ((((0 == strcmp((const char *)(info.u8ChipName), "Mercury6")) || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
                && ((2 == info.s32AoDevId[u32DevIdx]) || (3 == info.s32AoDevId[u32DevIdx])))
                && (1 != stWavHeaderInput.wave.wChannels))
            {
                printf("DAC0/DAC1 only support 1Chn Mono on Mercury6/Pioneer3.\n");
                return FALSE;
            }

            // Hdmi
            if ((((2 == info.s32AoDevId[u32DevIdx]) || (3 == info.s32AoDevId[u32DevIdx]))
                && (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                || ((4 == info.s32AoDevId[u32DevIdx]) && (0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))))
            {
                if ((32000 != stWavHeaderInput.wave.dwSamplesPerSec) && (48000 != stWavHeaderInput.wave.dwSamplesPerSec))
                {
                    printf("Hdmi only supports 32000/48000Hz.\n");
                    return FALSE;
                }
            }

            // I2S Tx
            if ((1 == info.s32AoDevId[u32DevIdx]) || ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6")) && (2 == info.s32AiDevId[u32DevIdx])))
            {
                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_MODE_I2S_MASTER != info.stAoAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_I2S_SLAVE != info.stAoAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_TDM_MASTER != info.stAoAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_TDM_SLAVE != info.stAoAttr[u32DevIdx].eWorkmode))
                    {
                        printf("invalid I2s Tx work mode[%d].\n", info.stAoAttr[u32DevIdx].eWorkmode);
                        return FALSE;
                    }
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                {
                    if ((E_MI_AUDIO_MODE_I2S_MASTER != info.stAoAttr[u32DevIdx].eWorkmode)
                        && (E_MI_AUDIO_MODE_I2S_SLAVE != info.stAoAttr[u32DevIdx].eWorkmode))
                    {
                        printf("invalid I2s Tx work mode[%d].\n", info.stAoAttr[u32DevIdx].eWorkmode);
                        return FALSE;
                    }
                }

                if ((E_MI_AUDIO_SAMPLE_RATE_8000 != info.stAoAttr[u32DevIdx].eSamplerate)
                    && (E_MI_AUDIO_SAMPLE_RATE_16000 != info.stAoAttr[u32DevIdx].eSamplerate)
                    && (E_MI_AUDIO_SAMPLE_RATE_32000 != info.stAoAttr[u32DevIdx].eSamplerate)
                    && (E_MI_AUDIO_SAMPLE_RATE_48000 != info.stAoAttr[u32DevIdx].eSamplerate))
                {
                    printf("invalid sample rate[%d], I2s Tx only support 8000/16000/32000/48000Hz on this chip.\n",
                        info.stAoAttr[u32DevIdx].eSamplerate);
                        return FALSE;
                }

                if ((E_MI_AUDIO_I2S_FMT_I2S_MSB != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt)
                    && (E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt))
                {
                    printf("invalid I2s Tx Format[%d].\n", info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt);
                    return FALSE;
                }

                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_I2S_MCLK_0 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_12_288M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_16_384M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_18_432M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_24_576M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_24M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                        && (E_MI_AUDIO_I2S_MCLK_48M != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk))
                    {
                        printf("invalid I2s Rx Mclk[%d].\n", info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk);
                        return FALSE;
                    }
                }
                else if ((0 == strcmp((const char *)(info.u8ChipName), "Infinity6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6B0"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity2M")))
                {
                    if ((E_MI_AUDIO_I2S_MCLK_0 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk))
                    {
                        printf("invalid I2s Rx Mclk[%d].\n", info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk);
                        return FALSE;
                    }
                }

                if ((0 == strcmp((const char *)(info.u8ChipName), "Mercury6"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3"))
                    || (0 == strcmp((const char *)(info.u8ChipName), "Infinity6E")))
                {
                    if ((E_MI_AUDIO_MODE_TDM_MASTER == info.stAoAttr[u32DevIdx].eWorkmode)
                        || (E_MI_AUDIO_MODE_TDM_SLAVE == info.stAoAttr[u32DevIdx].eWorkmode))
                    {
                        if ((4 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots)
                            && (8 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots))
                        {
                            printf("invalid I2S Tx tdm slot number[%d], only support 4/8 slots on Mercury6\n",
                                info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots);
                            return FALSE;
                        }
                    }

                    if ((E_MI_AUDIO_BIT_WIDTH_16 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth)
                            && (E_MI_AUDIO_BIT_WIDTH_32 != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth))
                    {
                        printf("invalid I2S Tx bit width[%d], only support 16/32 bit on this chip\n",
                                info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth);
                        return FALSE;
                    }
                }
            }

            // simple pcm tx
            if ((7 == info.s32AoDevId[u32DevIdx])
                && (0 == strcmp((const char *)(info.u8ChipName), "Pioneer3")))
            {
                if (E_MI_AUDIO_MODE_I2S_MASTER != info.stAoAttr[u32DevIdx].eWorkmode)
                {
                    printf("Simple Pcm Tx only support i2s master mode.\n");
                    return FALSE;
                }

                if (TRUE != info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock)
                {
                    printf("Simple Pcm Tx only support 4 wire mode.\n");
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

void initTimer(void)
{
    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);

    setTimer(u32RunTime);
    return;
}

void printJsonInfo(void)
{
    MI_U32 u32DevIdx;
    printf("Chip name:%s.\n", (info.u8ChipName));

    if (info.bEnableAI)
    {
        printf("======= Ai Param ======\n");

        // output path
        printf("AI OutPut Path:%s\n", info.u8DumpDataPath);

        printf("====== Device Info ======\n");

        for (u32DevIdx = 0; u32DevIdx < info.u32AiDevNumFromConfigFile; u32DevIdx++)
        {
            // device info
            printf("Device:\t");
            if (0 == strcmp("Mercury6", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic0_1\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein0_1\n");
                        break;

                    case 4:
                        printf("Amic2_3\n");
                        break;

                    case 5:
                        printf("Amic0_1_2_3\n");
                        break;
                }
            }
            else if (0 == strcmp("Pioneer3", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic0_1\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein0_1\n");
                        break;

                    case 4:
                        printf("Amic2\n");
                        break;

                    case 5:
                        printf("Amic0_1_2\n");
                        break;

                    case 6:
                        printf("Dmic 2Chn\n");
                        break;

                    case 7:
                        printf("Simple Pcm Rx\n");
                        break;
                    case 8:
                        printf("I2S(2chn) + SRC\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity2M", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein\n");
                        break;

                    case 4:
                        printf("Amic2Chn + I2s RX\n");
                        break;

                    case 5:
                        printf("Dmic2Chn + I2s RX\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6E", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein\n");
                        break;

                    case 4:
                        printf("I2S RX + SRC\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6B0", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein\n");
                        break;

                    case 4:
                        printf("I2S RX + SRC\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AiDevId[u32DevIdx])
                {
                    case 0:
                        printf("Amic\n");
                        break;

                    case 1:
                        printf("Dmic\n");
                        break;

                    case 2:
                        printf("I2S RX\n");
                        break;

                    case 3:
                        printf("Linein\n");
                        break;
                }
            }

            printf("Sample rate:\t%d\n", info.stAiAttr[u32DevIdx].eSamplerate);
            printf("Bit width:\t");
            switch (info.stAiAttr[u32DevIdx].eBitwidth)
            {
                case E_MI_AUDIO_BIT_WIDTH_16:
                    printf("16bit\n");
                    break;

                default:
                    exit(0);
            }

            printf("Sound mode:\t");
            switch (info.stAiAttr[u32DevIdx].eSoundmode)
            {
                case E_MI_AUDIO_SOUND_MODE_MONO:
                    printf("Mono\n");
                    break;

                case E_MI_AUDIO_SOUND_MODE_STEREO:
                    printf("Stereo\n");
                    break;

                case E_MI_AUDIO_SOUND_MODE_QUEUE:
                    printf("Queue\n");
                    break;

                default:
                    exit(0);
            }

            printf("MI Channel count:\t%d\n", info.stAiAttr[u32DevIdx].u32ChnCnt);

            if (2 != info.s32AiDevId[u32DevIdx])
            {
                printf("Volume:\t[%d, %d]\n", info.s32AiVolume[u32DevIdx][0], info.s32AiVolume[u32DevIdx][1]);
            }

            if ((2 == info.s32AiDevId[u32DevIdx])
                || (((0 == strcmp("Infinity6B0", (const char *)(info.u8ChipName)))
                    || (0 == strcmp("Infinity6E", (const char *)(info.u8ChipName))))
                        && (4 == info.s32AiDevId[u32DevIdx]))
                || ((8 == info.s32AiDevId[u32DevIdx]) && (0 == strcmp("Pioneer3", (const char *)(info.u8ChipName)))))
            {
                printf("Work mode:\t");
                switch (info.stAiAttr[u32DevIdx].eWorkmode)
                {
                    case E_MI_AUDIO_MODE_I2S_MASTER:
                        printf("I2s master\n");
                        break;

                    case E_MI_AUDIO_MODE_I2S_SLAVE:
                        printf("I2s slave\n");
                        break;

                    case E_MI_AUDIO_MODE_TDM_MASTER:
                        printf("TDM master\n");
                        break;

                    case E_MI_AUDIO_MODE_TDM_SLAVE:
                        printf("TDM slave\n");
                        break;

                    default:
                        exit(0);
                }

                printf("I2s format:\t");
                switch (info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt)
                {
                    case E_MI_AUDIO_I2S_FMT_I2S_MSB:
                        printf("I2s format\n");
                        break;

                    case E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB:
                        printf("Left justify i2s format\n");
                        break;
                }

                printf("I2s Mclk:\t");
                switch (info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                {
                    case E_MI_AUDIO_I2S_MCLK_0:
                        printf("disbale mclk\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_12_288M:
                        printf("12.288M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_16_384M:
                        printf("16.384M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_18_432M:
                        printf("18.432M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_24_576M:
                        printf("24.576M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_24M:
                        printf("24M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_48M:
                        printf("48M\n");
                        break;
                }

                printf("I2s sync clock:\t");
                switch (info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock)
                {
                    case 0:
                        printf("FALSE\n");
                        break;

                    case 1:
                        printf("TRUE\n");
                        break;
                }

                printf("I2s tdm slots:\t%d\n", info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots);
            }

            printf("\n");
        }
        printf("====== Device Info End ======\n");

        printf("======= Ai Param End ======\n");

        printf("\n\n");
    }

    if (info.bEnableAO)
    {
        printf("======= Ao Param ======\n");

        for (u32DevIdx = 0; u32DevIdx < info.u32AoDevNumFromConfigFile; u32DevIdx++)
        {
            printf("====== Device Info ======\n");
            // device info
            printf("Device:\t");
            if (0 == strcmp("Mercury6", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut0_1\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;

                    case 2:
                        printf("LineOut0\n");
                        break;

                    case 3:
                        printf("LineOut1\n");
                        break;

                    case 4:
                        printf("Hdmi+LineOut0_1\n");
                        break;
                }
            }
            else if (0 == strcmp("Pioneer3", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut0_1\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;

                    case 2:
                        printf("LineOut0\n");
                        break;

                    case 3:
                        printf("LineOut1\n");
                        break;

                    case 4:
                        printf("HeadPhone\n");
                        break;

                    case 5:
                        printf("Lineout(mix ADC2)+HeadPhone\n");
                        break;

                    case 6:
                        printf("Lineout+HeadPhone\n");
                        break;

                    case 7:
                        printf("Simple Pcm Tx\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity2M", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;

                    case 2:
                        printf("Hdmi\n");
                        break;

                    case 3:
                        printf("Hdmi+LineOut\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6E", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6B0", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;
                }
            }
            else if (0 == strcmp("Infinity6", (const char *)(info.u8ChipName)))
            {
                switch (info.s32AoDevId[u32DevIdx])
                {
                    case 0:
                        printf("LineOut\n");
                        break;

                    case 1:
                        printf("I2S TX\n");
                        break;

                    case 2:
                        printf("I2S TX + LineOut\n");
                        break;
                }
            }

            printf("Sample rate:\t%d\n", info.stAoAttr[u32DevIdx].eSamplerate);
            printf("Bit width:\t");
            switch (info.stAoAttr[u32DevIdx].eBitwidth)
            {
                case E_MI_AUDIO_BIT_WIDTH_16:
                    printf("16bit\n");
                    break;

                default:
                    exit(0);
            }

            printf("Sound mode:\t");
            switch (info.stAoAttr[u32DevIdx].eSoundmode)
            {
                case E_MI_AUDIO_SOUND_MODE_MONO:
                    printf("Mono\n");
                    break;

                case E_MI_AUDIO_SOUND_MODE_STEREO:
                    printf("Stereo\n");
                    break;

                case E_MI_AUDIO_SOUND_MODE_QUEUE:
                    printf("Queue\n");
                    break;

                default:
                    exit(0);
            }

            printf("MI Channel count:\t%d\n", info.stAoAttr[u32DevIdx].u32ChnCnt);

            if (2 != info.s32AoDevId[u32DevIdx])
            {
                printf("Volume:\t%d\n", info.s32AoVolume[u32DevIdx]);
            }

            if ((1 == info.s32AiDevId[u32DevIdx])
                || (((0 == strcmp("Infinity6", (const char *)(info.u8ChipName))))
                        && (2 == info.s32AiDevId[u32DevIdx])))
            {
                printf("Work mode:\t");
                switch (info.stAoAttr[u32DevIdx].eWorkmode)
                {
                    case E_MI_AUDIO_MODE_I2S_MASTER:
                        printf("I2s master\n");
                        break;

                    case E_MI_AUDIO_MODE_I2S_SLAVE:
                        printf("I2s slave\n");
                        break;

                    case E_MI_AUDIO_MODE_TDM_MASTER:
                        printf("TDM master\n");
                        break;

                    case E_MI_AUDIO_MODE_TDM_SLAVE:
                        printf("TDM slave\n");
                        break;

                    default:
                        exit(0);
                }

                printf("I2s format:\t");
                switch (info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt)
                {
                    case E_MI_AUDIO_I2S_FMT_I2S_MSB:
                        printf("I2s format\n");
                        break;

                    case E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB:
                        printf("Left justify i2s format\n");
                        break;

                    default:
                        exit(0);
                }

                printf("I2s Mclk:\t");
                switch (info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk)
                {
                    case E_MI_AUDIO_I2S_MCLK_0:
                        printf("disbale mclk\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_12_288M:
                        printf("12.288M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_16_384M:
                        printf("16.384M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_18_432M:
                        printf("18.432M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_24_576M:
                        printf("24.576M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_24M:
                        printf("24M\n");
                        break;

                    case E_MI_AUDIO_I2S_MCLK_48M:
                        printf("48M\n");
                        break;

                    default:
                        exit(0);
                }

                printf("I2s sync clock:\t");
                switch (info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock)
                {
                    case 0:
                        printf("FALSE\n");
                        break;

                    case 1:
                        printf("TRUE\n");
                        break;

                    default:
                        exit(0);
                }

                printf("I2s tdm slots:\t%d\n", info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots);
            }

            printf("\n");
        }

        printf("====== Device Info End ======\n");

        printf("======= Ao Param End ======\n");
    }
}

int addWaveHeader(WaveFileHeader_t* tWavHead, AencType_e eAencType, SoundMode_e eSoundMode, SampleRate_e eSampleRate, int raw_len)
{
    tWavHead->chRIFF[0] = 'R';
    tWavHead->chRIFF[1] = 'I';
    tWavHead->chRIFF[2] = 'F';
    tWavHead->chRIFF[3] = 'F';

    tWavHead->chWAVE[0] = 'W';
    tWavHead->chWAVE[1] = 'A';
    tWavHead->chWAVE[2] = 'V';
    tWavHead->chWAVE[3] = 'E';

    tWavHead->chFMT[0] = 'f';
    tWavHead->chFMT[1] = 'm';
    tWavHead->chFMT[2] = 't';
    tWavHead->chFMT[3] = 0x20;
    tWavHead->dwFMTLen = 0x10;

    if (eAencType == PCM)
    {
        if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x1;
        tWavHead->wave.wBitsPerSample = 16; //16bit
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = 1024;
    }

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(WaveFileHeader_t) - 8;

    return 0;
}

MI_BOOL createOutputFileName(CreateDataFile_t* pFileInfo)
{
    char tmpStr[1024] = {0};

    if (NULL == pFileInfo)
    {
        printf("NULL pointer.\n");
        return FALSE;
    }

    if ((NULL == pFileInfo->pu8ChipName)
        || (NULL == pFileInfo->pstAttr)
        || (NULL == pFileInfo->pu8OutputPath)
        || (NULL == pFileInfo->pu8OutputFileName))
    {
        printf("NULL pointer.\n");
        return FALSE;
    }

    memset(tmpStr, 0, sizeof(tmpStr));
    if ('/' == pFileInfo->pu8OutputPath[strlen((char *)(pFileInfo->pu8OutputPath)) - 1])
    {
        sprintf(tmpStr, "%s", pFileInfo->pu8OutputPath);
    }
    else
    {
        sprintf(tmpStr, "%s/", pFileInfo->pu8OutputPath);
    }
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    if (0 == strcmp("Mercury6", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic0_1_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein0_1_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic2_3_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Amic0_1_2_3_");
                break;
        }
    }
    else if (0 == strcmp("Pioneer3", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic0_1_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein0_1_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic2_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Amic0_1_2_");
                break;

            case 6:
                sprintf(tmpStr, "%s", "Dmic_2Chn_");
                break;

            case 7:
                sprintf(tmpStr, "%s", "Simple_Pcm_Rx_");
                break;
        }
    }
    else if (0 == strcmp("Infinity2M", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic+I2sRX_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Dmic+I2sRX_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6E", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "I2sRX+Src_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6B0", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "I2sRX+Src_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;
        }
    }

    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set Chn num
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "Chn%d_", pFileInfo->u32ChnIdx);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile SampleRate
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "%dK", pFileInfo->pstAttr->eSamplerate / 1000);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile BitWidth
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "_%dbit", 16);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile Channel Mode
    memset(tmpStr, 0, sizeof(tmpStr));
    if (E_MI_AUDIO_SOUND_MODE_MONO == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_MONO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_STEREO == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_STEREO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_QUEUE");
    }

    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, ".wav");
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);
    return TRUE;
}

MI_BOOL createSrcOutputFileName(CreateSrcFile_t* pFileInfo)
{
    char tmpStr[1024] = {0};

    if (NULL == pFileInfo)
    {
        printf("NULL pointer.\n");
        return FALSE;
    }

    if ((NULL == pFileInfo->pu8ChipName)
        || (NULL == pFileInfo->pstAttr)
        || (NULL == pFileInfo->pu8OutputPath)
        || (NULL == pFileInfo->pu8OutputFileName))
    {
        printf("NULL pointer.\n");
        return FALSE;
    }

    memset(tmpStr, 0, sizeof(tmpStr));
    if ('/' == pFileInfo->pu8OutputPath[strlen((char *)(pFileInfo->pu8OutputPath)) - 1])
    {
        sprintf(tmpStr, "%s", pFileInfo->pu8OutputPath);
    }
    else
    {
        sprintf(tmpStr, "%s/", pFileInfo->pu8OutputPath);
    }
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    if (0 == strcmp("Mercury6", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic0_1_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein0_1_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic2_3_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Amic0_1_2_3_");
                break;
        }
    }
    else if (0 == strcmp("Pioneer3", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic0_1_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein0_1_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic2_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Amic0_1_2_");
                break;

            case 6:
                sprintf(tmpStr, "%s", "Dmic_2Chn_");
                break;

            case 7:
                sprintf(tmpStr, "%s", "Simple_Pcm_Rx_");
                break;
        }
    }
    else if (0 == strcmp("Infinity2M", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "Amic+I2sRX_");
                break;

            case 5:
                sprintf(tmpStr, "%s", "Dmic+I2sRX_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6E", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "I2sRX+Src_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6B0", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;

            case 4:
                sprintf(tmpStr, "%s", "I2sRX+Src_");
                break;
        }
    }
    else if (0 == strcmp("Infinity6", (const char *)(pFileInfo->pu8ChipName)))
    {
        switch (pFileInfo->AiDevId)
        {
            case 0:
                sprintf(tmpStr, "%s", "Amic_");
                break;

            case 1:
                sprintf(tmpStr, "%s", "Dmic_");
                break;

            case 2:
                sprintf(tmpStr, "%s", "I2sRx_");
                break;

            case 3:
                sprintf(tmpStr, "%s", "Linein_");
                break;
        }
    }

    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set Chn num
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "Chn%d_", pFileInfo->u32ChnIdx);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    // set Src num
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "ECho%d_", pFileInfo->u32SrcChnIdx);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile SampleRate
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "%dK", pFileInfo->pstAttr->eSamplerate / 1000);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile BitWidth
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, "_%dbit", 16);
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);

    //set OutpuFile Channel Mode
    memset(tmpStr, 0, sizeof(tmpStr));
    if (E_MI_AUDIO_SOUND_MODE_MONO == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_MONO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_STEREO == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_STEREO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pFileInfo->pstAttr->eSoundmode)
    {
        sprintf(tmpStr, "_QUEUE");
    }

    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);
    memset(tmpStr, 0, sizeof(tmpStr));
    sprintf(tmpStr, ".wav");
    strcat((char *)(pFileInfo->pu8OutputFileName), tmpStr);
    return TRUE;
}

void* aiGetChnPortBuf(void* data)
{
    AiChnPrivateData_t* pstChnPrivateData = (AiChnPrivateData_t*)data;
    MI_AUDIO_Frame_t stAiChFrame;
    MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32Ret;
    MI_U32 u32ChnIdx, u32ChnIdxStart, u32ChnIdxEnd;
    struct timeval tv_before, tv_after;
    MI_S64 before_us, after_us;
    WaveFileHeader_t stWavHead;
    SoundMode_e eAiWavSoundMode;
    MI_U32 u32SrcChnIdx, u32SrcChnCnt = 2;

    memset(&stAiChFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    if ((E_MI_AUDIO_SOUND_MODE_MONO == pstChnPrivateData->stDevAttr.eSoundmode)
        || (E_MI_AUDIO_SOUND_MODE_STEREO == pstChnPrivateData->stDevAttr.eSoundmode))
    {
        u32ChnIdxStart = pstChnPrivateData->AiChn;
        u32ChnIdxEnd = u32ChnIdxStart + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pstChnPrivateData->stDevAttr.eSoundmode)
    {
        u32ChnIdxStart = 0;
        u32ChnIdxEnd = pstChnPrivateData->stDevAttr.u32ChnCnt;
    }

    while (FALSE == bAiExit)
    {
        s32Ret = MI_AI_GetFrame(pstChnPrivateData->AiDevId, pstChnPrivateData->AiChn, &stAiChFrame, &stAecFrame, -1);
        if (MI_SUCCESS == s32Ret)
        {
            for (u32ChnIdx = u32ChnIdxStart; u32ChnIdx < u32ChnIdxEnd; u32ChnIdx++)
            {
                gettimeofday(&tv_before, NULL);

                if ((E_MI_AUDIO_SOUND_MODE_MONO == pstChnPrivateData->stDevAttr.eSoundmode)
                    || (E_MI_AUDIO_SOUND_MODE_STEREO == pstChnPrivateData->stDevAttr.eSoundmode))
                {
                    write(pstChnPrivateData->s32Fd, stAiChFrame.apVirAddr[0], stAiChFrame.u32Len[0]);
                }
                else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pstChnPrivateData->stDevAttr.eSoundmode)
                {
                    write(stAiChnPriv[pstChnPrivateData->u32DevIdx][u32ChnIdx].s32Fd, stAiChFrame.apVirAddr[u32ChnIdx], stAiChFrame.u32Len[u32ChnIdx]);
                }

                gettimeofday(&tv_after, NULL);
                before_us = tv_before.tv_sec * 1000000 + tv_before.tv_usec;
                after_us = tv_after.tv_sec * 1000000 + tv_after.tv_usec;
                if (after_us - before_us > 10 * 1000)
                {
                    printf("Dev%d Chn:%d, cost time:%lldus = %lldms.\n",
                        pstChnPrivateData->AiDevId,
                        u32ChnIdx, after_us - before_us, (after_us - before_us) / 1000);
                }
            }
            pstChnPrivateData->u32TotalSize += stAiChFrame.u32Len[0];

            //printf("stAecFrame.bValid)[%d].\n", stAecFrame.bValid);
            if (stAecFrame.bValid)
            {
                for (u32SrcChnIdx = 0; u32SrcChnIdx < u32SrcChnCnt; u32SrcChnIdx++)
                {
                    if (pstChnPrivateData->s32EchoFd[u32SrcChnIdx]
                        && stAecFrame.stRefFrame.apVirAddr[u32SrcChnIdx]
                        && (0 != stAecFrame.stRefFrame.u32Len[u32SrcChnIdx]))
                    {
                        write(pstChnPrivateData->s32EchoFd[u32SrcChnIdx],
                            stAecFrame.stRefFrame.apVirAddr[u32SrcChnIdx],
                            stAecFrame.stRefFrame.u32Len[u32SrcChnIdx]);
                        pstChnPrivateData->u32EchoTotalSize[u32SrcChnIdx] += stAecFrame.stRefFrame.u32Len[u32SrcChnIdx];
                    }
                }
            }

            MI_AI_ReleaseFrame(pstChnPrivateData->AiDevId, pstChnPrivateData->AiChn, &stAiChFrame, &stAecFrame);
        }
        else
        {
            printf("Dev%dChn%d get frame failed!!! error:0x%x\n", pstChnPrivateData->AiDevId, pstChnPrivateData->AiChn, s32Ret);
        }
    }

    if ((E_MI_AUDIO_SOUND_MODE_MONO == pstChnPrivateData->stDevAttr.eSoundmode)
            || (E_MI_AUDIO_SOUND_MODE_QUEUE == pstChnPrivateData->stDevAttr.eSoundmode))
    {
        eAiWavSoundMode = E_SOUND_MODE_MONO;
    }
    else if (E_MI_AUDIO_SOUND_MODE_STEREO == pstChnPrivateData->stDevAttr.eSoundmode)
    {
        eAiWavSoundMode = E_SOUND_MODE_STEREO;
    }

    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
    addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, pstChnPrivateData->stDevAttr.eSamplerate, pstChnPrivateData->u32TotalSize);
    for (u32ChnIdx = u32ChnIdxStart; u32ChnIdx < u32ChnIdxEnd; u32ChnIdx++)
    {
        lseek(stAiChnPriv[pstChnPrivateData->u32DevIdx][u32ChnIdx].s32Fd, 0, SEEK_SET);
        write(stAiChnPriv[pstChnPrivateData->u32DevIdx][u32ChnIdx].s32Fd, &stWavHead, sizeof(WaveFileHeader_t));
        close(stAiChnPriv[pstChnPrivateData->u32DevIdx][u32ChnIdx].s32Fd);
    }

    for (u32SrcChnIdx = 0; u32SrcChnIdx < u32SrcChnCnt; u32SrcChnIdx++)
    {
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, pstChnPrivateData->stDevAttr.eSamplerate,
            pstChnPrivateData->u32EchoTotalSize[u32SrcChnIdx]);
        lseek(pstChnPrivateData->s32EchoFd[u32SrcChnIdx], 0, SEEK_SET);
        write(pstChnPrivateData->s32EchoFd[u32SrcChnIdx], &stWavHead, sizeof(WaveFileHeader_t));
        close(pstChnPrivateData->s32EchoFd[u32SrcChnIdx]);
    }

    return NULL;
}

void* aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    AoChnPrivateData_t* pstChnPrivateData = (AoChnPrivateData_t*)data;
    MI_AUDIO_Frame_t stAoSendFrame;
    MI_U8* pu8TempBuf = NULL;

    pu8TempBuf = (MI_U8*)malloc(pstChnPrivateData->u32NeedSize);
    if (NULL == pu8TempBuf)
    {
        printf("Dev%d failed to alloc buffer.\n", pstChnPrivateData->AoDevId);
    }
    else
    {
        memset(pu8TempBuf, 0, sizeof(pstChnPrivateData->u32NeedSize));
    }

    while (FALSE == bAoExit)
    {
        s32Ret = read(pstChnPrivateData->s32Fd, pu8TempBuf, pstChnPrivateData->u32NeedSize);
        if (s32Ret != pstChnPrivateData->u32NeedSize)
        {
            lseek(pstChnPrivateData->s32Fd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(pstChnPrivateData->s32Fd, pu8TempBuf, pstChnPrivateData->u32NeedSize);
            if (s32Ret < 0)
            {
                printf("Dev%d input file does not has enough data!!!\n", pstChnPrivateData->AoDevId);
                break;
            }
        }

        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        stAoSendFrame.apVirAddr[0] = pu8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do{
            s32Ret = MI_AO_SendFrame(pstChnPrivateData->AoDevId, 0, &stAoSendFrame, 30);
        }while((s32Ret == MI_AO_ERR_NOBUF) && (FALSE == bAoExit));

        if (s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: Dev%d MI_AO_SendFrame fail, error is 0x%x: \n", pstChnPrivateData->AoDevId, s32Ret);
        }
    }

    free(pu8TempBuf);
    return NULL;
}

MI_S32 initAi(void)
{
    MI_U32 u32DevIdx;
    MI_U32 u32RunningDevMaxOfChip;
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN u32ChnIdx, u32ChnCnt;
    MI_U32 u32SrcChnIdx, u32SrcChnCnt = 2;
    MI_AUDIO_Attr_t stAiSetAttr;
    WaveFileHeader_t stAiWavHead;
    MI_SYS_ChnPort_t stAiChnOutputPort;
    MI_AI_ChnParam_t stAiChnParam;
    MI_S32 s32Ret;
    CreateDataFile_t stCreateDataFile;
    CreateSrcFile_t stCreateSrcFile;

    if (0 == strcmp("Mercury6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Pioneer3", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Infinity2M", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6E", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6B0", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }

    for (u32DevIdx = 0; (u32DevIdx < info.u32AiDevNumFromConfigFile) && (u32DevIdx < u32RunningDevMaxOfChip); u32DevIdx++)
    {
        AiDevId = info.s32AiDevId[u32DevIdx];

        stAiSetAttr.eSamplerate = info.stAiAttr[u32DevIdx].eSamplerate;
        stAiSetAttr.eBitwidth = info.stAiAttr[u32DevIdx].eBitwidth;
        stAiSetAttr.eWorkmode = info.stAiAttr[u32DevIdx].eWorkmode;
        stAiSetAttr.eSoundmode = info.stAiAttr[u32DevIdx].eSoundmode;
        stAiSetAttr.u32PtNumPerFrm = stAiSetAttr.eSamplerate / 16;
        stAiSetAttr.u32ChnCnt = info.stAiAttr[u32DevIdx].u32ChnCnt;
        stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt;
        stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk;
        stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock;
        stAiSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots;
        stAiSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = info.stAiAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth;

        ExecFunc(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS);
        ExecFunc(MI_AI_Enable(AiDevId), MI_SUCCESS);

        u32ChnCnt = stAiSetAttr.u32ChnCnt;

        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            // memset stAiChnPriv
            memset(&stAiChnPriv[u32DevIdx][u32ChnIdx], 0, sizeof(stAiChnPriv[u32DevIdx][u32ChnIdx]));

            // create data file
            memset(u8OutputFileName, 0x00, sizeof(u8OutputFileName));
            printf("Dev%d Chn%d chipName:%s dumpPath:%s.\n", AiDevId, u32ChnIdx, (info.u8ChipName), info.u8DumpDataPath);
            memset(&stCreateDataFile, 0x00, sizeof(stCreateDataFile));
            stCreateDataFile.AiDevId = AiDevId;
            stCreateDataFile.u32ChnIdx = u32ChnIdx;
            stCreateDataFile.pu8ChipName = (info.u8ChipName);
            stCreateDataFile.pstAttr = &stAiSetAttr;
            stCreateDataFile.pu8OutputPath = info.u8DumpDataPath;
            stCreateDataFile.pu8OutputFileName = u8OutputFileName;
            createOutputFileName(&stCreateDataFile);

            stAiChnPriv[u32DevIdx][u32ChnIdx].s32Fd = open((const char *)u8OutputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
            if (stAiChnPriv[u32DevIdx][u32ChnIdx].s32Fd < 0)
            {
                printf("failed to open output file path[%s], error[%s].\n", u8OutputFileName, strerror(errno));
                exit(0);
            }

            memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
            s32Ret = write(stAiChnPriv[u32DevIdx][u32ChnIdx].s32Fd, &stAiWavHead, sizeof(WaveFileHeader_t));
            if (s32Ret < 0)
            {
                printf("write wav head failed\n");
                return -1;
            }

            // create src file
            if (((E_MI_AUDIO_SOUND_MODE_MONO == info.stAiAttr[u32DevIdx].eSoundmode)
                    || (E_MI_AUDIO_SOUND_MODE_STEREO == info.stAiAttr[u32DevIdx].eSoundmode))
                || ((E_MI_AUDIO_SOUND_MODE_QUEUE == info.stAiAttr[u32DevIdx].eSoundmode) && (0 == u32ChnIdx)))
            {
                for (u32SrcChnIdx = 0; u32SrcChnIdx < u32SrcChnCnt; u32SrcChnIdx++)
                {
                    memset(u8OutputFileName, 0x00, sizeof(u8OutputFileName));
                    printf("Dev%d Chn%d EchoChn%d chipName:%s dumpPath:%s.\n", AiDevId, u32ChnIdx, u32SrcChnIdx, (info.u8ChipName), info.u8DumpDataPath);
                    memset(&stCreateSrcFile, 0x00, sizeof(stCreateSrcFile));
                    stCreateSrcFile.AiDevId = AiDevId;
                    stCreateSrcFile.u32ChnIdx = u32ChnIdx;
                    stCreateSrcFile.u32SrcChnIdx = u32SrcChnIdx;
                    stCreateSrcFile.pu8ChipName = (info.u8ChipName);
                    stCreateSrcFile.pstAttr = &stAiSetAttr;
                    stCreateSrcFile.pu8OutputPath = info.u8DumpDataPath;
                    stCreateSrcFile.pu8OutputFileName = u8OutputFileName;
                    createSrcOutputFileName(&stCreateSrcFile);

                    stAiChnPriv[u32DevIdx][u32ChnIdx].s32EchoFd[u32SrcChnIdx] = open((const char *)u8OutputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
                    if (stAiChnPriv[u32DevIdx][u32ChnIdx].s32EchoFd[u32SrcChnIdx] < 0)
                    {
                        printf("failed to open output file path[%s], error[%s].\n", u8OutputFileName, strerror(errno));
                        exit(0);
                    }

                    memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
                    s32Ret = write(stAiChnPriv[u32DevIdx][u32ChnIdx].s32EchoFd[u32SrcChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
                    if (s32Ret < 0)
                    {
                        printf("write wav head failed\n");
                        return -1;
                    }
                }
            }

            // set depth
            if ((E_MI_AUDIO_SOUND_MODE_MONO == stAiSetAttr.eSoundmode)
                || (E_MI_AUDIO_SOUND_MODE_STEREO == stAiSetAttr.eSoundmode)
                || ((E_MI_AUDIO_SOUND_MODE_QUEUE == stAiSetAttr.eSoundmode) && (0 == u32ChnIdx)))
            {
                memset(&stAiChnOutputPort, 0, sizeof(stAiChnOutputPort));
                stAiChnOutputPort.eModId = E_MI_MODULE_ID_AI;
                stAiChnOutputPort.u32DevId = AiDevId;
                stAiChnOutputPort.u32ChnId = u32ChnIdx;
                stAiChnOutputPort.u32PortId = 0;
                ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort, USER_BUF_DEPTH, TOTAL_BUF_DEPTH), MI_SUCCESS);
            }

            // set gain
            if (0 == strcmp("Mercury6", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                    case 4:
                    case 5:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;
                }
            }
            else if (0 == strcmp("Pioneer3", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                    case 4:
                    case 5:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                    case 6:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;
                }
            }
            else if (0 == strcmp("Infinity2M", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;

                    case 4:
                        {
                            if ((((E_MI_AUDIO_SOUND_MODE_MONO == stAiSetAttr.eSoundmode)
                                        || (E_MI_AUDIO_SOUND_MODE_QUEUE == stAiSetAttr.eSoundmode))
                                    && ((0 == u32ChnIdx) || (1 == u32ChnIdx)))
                                || ((E_MI_AUDIO_SOUND_MODE_STEREO == stAiSetAttr.eSoundmode) && (0 == u32ChnIdx)))
                            {
                                memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                                stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                                stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                                stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                                ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                            }
                        }
                        break;

                    case 5:
                        {
                            if ((((E_MI_AUDIO_SOUND_MODE_MONO == stAiSetAttr.eSoundmode)
                                        || (E_MI_AUDIO_SOUND_MODE_QUEUE == stAiSetAttr.eSoundmode))
                                    && ((0 == u32ChnIdx) || (1 == u32ChnIdx)))
                                || ((E_MI_AUDIO_SOUND_MODE_STEREO == stAiSetAttr.eSoundmode) && (0 == u32ChnIdx)))
                            {
                                ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                            }
                        }
                        break;
                }
            }
            else if (0 == strcmp("Infinity6E", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;
                }
            }
            else if (0 == strcmp("Infinity6B0", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;
                }
            }
            else if (0 == strcmp("Infinity6", (const char*)(info.u8ChipName)))
            {
                switch (AiDevId)
                {
                    case 0:
                    case 3:
                        {
                            memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
                            stAiChnParam.stChnGain.bEnableGainSet = TRUE;
                            stAiChnParam.stChnGain.s16FrontGain = info.s32AiVolume[u32DevIdx][0];
                            stAiChnParam.stChnGain.s16RearGain = info.s32AiVolume[u32DevIdx][1];
                            ExecFunc(MI_AI_SetChnParam(AiDevId, u32ChnIdx, &stAiChnParam), MI_SUCCESS);
                        }
                        break;

                    case 1:
                        {
                            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, info.s32AiVolume[u32DevIdx][0]), MI_SUCCESS);
                        }
                        break;
                }
            }

            // enable channel
            // init channel private data
            // create getFrame thread
            if ((E_MI_AUDIO_SOUND_MODE_MONO == stAiSetAttr.eSoundmode)
                || (E_MI_AUDIO_SOUND_MODE_STEREO == stAiSetAttr.eSoundmode)
                || ((E_MI_AUDIO_SOUND_MODE_QUEUE == stAiSetAttr.eSoundmode) && (0 == u32ChnIdx)))
            {
                ExecFunc(MI_AI_EnableChn(AiDevId, u32ChnIdx), MI_SUCCESS);

                stAiChnPriv[u32DevIdx][u32ChnIdx].u32DevIdx = u32DevIdx;
                stAiChnPriv[u32DevIdx][u32ChnIdx].AiDevId = AiDevId;
                stAiChnPriv[u32DevIdx][u32ChnIdx].AiChn = u32ChnIdx;
                stAiChnPriv[u32DevIdx][u32ChnIdx].u32TotalSize = 0;
                memcpy(&(stAiChnPriv[u32DevIdx][u32ChnIdx].stDevAttr), &stAiSetAttr, sizeof(stAiSetAttr));

                pthread_create(&stAiChnPriv[u32DevIdx][u32ChnIdx].getFrameThreadId, NULL, aiGetChnPortBuf, &stAiChnPriv[u32DevIdx][u32ChnIdx]);
            }
        }
    }
    return 0;
}

MI_S32 initAo(void)
{
    MI_AUDIO_DEV AoDevId;
    MI_U32 u32RunningDevMaxOfChip;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr;
    MI_U32 u32PhyChnNum;
    WaveFileHeader_t stWavHeaderInput;
    MI_U32 u32NeedSize;
    MI_U32 u32DevIdx;

    if (0 == strcmp("Mercury6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Pioneer3", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Infinity2M", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6E", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6B0", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }

    for (u32DevIdx = 0; (u32DevIdx < info.u32AoDevNumFromConfigFile) && (u32DevIdx < u32RunningDevMaxOfChip); u32DevIdx++)
    {
        AoDevId = info.s32AoDevId[u32DevIdx];
        memset(&stWavHeaderInput, 0, sizeof(WaveFileHeader_t));
        memset(&stAoChnPriv[u32DevIdx], 0x0, sizeof(stAoChnPriv[u32DevIdx]));

        stAoChnPriv[u32DevIdx].s32Fd = open((const char *)info.u8PlayBackPath[u32DevIdx], O_RDONLY, 0666);
        if (stAoChnPriv[u32DevIdx].s32Fd < 0)
        {
            printf("failed to open input file[%s], error[%s] \n", info.u8PlayBackPath[u32DevIdx], strerror(errno));
            exit(0);
        }

        s32Ret = read(stAoChnPriv[u32DevIdx].s32Fd, &stWavHeaderInput, sizeof(WaveFileHeader_t));
        if (s32Ret < 0)
        {
            printf("failed to read wav header!!!\n");
            exit(0);
        }

        memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
        stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
        stAoSetAttr.eBitwidth = info.stAoAttr[u32DevIdx].eBitwidth;
        stAoSetAttr.eWorkmode = info.stAoAttr[u32DevIdx].eWorkmode;
        if (stWavHeaderInput.wave.wChannels == 2)
        {
            stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
        }
        else if(stWavHeaderInput.wave.wChannels == 1)
        {
            stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
        }

        stAoSetAttr.u32ChnCnt = 1;
        stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
        stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.bSyncClock;
        stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eFmt;
        stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eMclk;
        stAoSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots;
        stAoSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = info.stAoAttr[u32DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth;

        ExecFunc(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS);
        ExecFunc(MI_AO_Enable(AoDevId), MI_SUCCESS);
        ExecFunc(MI_AO_SetVolume(AoDevId, 0, info.s32AoVolume[u32DevIdx], E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
        ExecFunc(MI_AO_EnableChn(AoDevId, 0), MI_SUCCESS);

        switch (stAoSetAttr.eSoundmode)
        {
            case E_MI_AUDIO_SOUND_MODE_MONO:
                u32PhyChnNum = 1;
                break;

            case E_MI_AUDIO_SOUND_MODE_STEREO:
                u32PhyChnNum = 2;
                break;

            default:
                exit(0);
        }

        u32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * u32PhyChnNum * stAoSetAttr.u32ChnCnt;
        u32NeedSize = u32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * u32PhyChnNum) * (stAoSetAttr.u32ChnCnt * 2 * u32PhyChnNum);

        stAoChnPriv[u32DevIdx].u32DevIdx = u32DevIdx;
        stAoChnPriv[u32DevIdx].AoDevId = AoDevId;
        stAoChnPriv[u32DevIdx].u32NeedSize = u32NeedSize;

        pthread_create(&(stAoChnPriv[u32DevIdx].sendFrameThreadId), NULL, aoSendFrame, &stAoChnPriv[u32DevIdx]);
    }

    return 0;
}

MI_S32 deinitAi(void)
{
    MI_U32 u32ChnIdx, u32ChnCnt;
    MI_U32 u32DevIdx;
    MI_U32 u32RunningDevMaxOfChip;
    MI_AUDIO_DEV AiDevId;

    if (0 == strcmp("Mercury6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Pioneer3", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Infinity2M", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6E", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6B0", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }

    for (u32DevIdx = 0; (u32DevIdx < info.u32AiDevNumFromConfigFile) && (u32DevIdx < u32RunningDevMaxOfChip); u32DevIdx++)
    {
        AiDevId = info.s32AiDevId[u32DevIdx];
        if ((E_MI_AUDIO_SOUND_MODE_MONO == info.stAiAttr[u32DevIdx].eSoundmode)
            || (E_MI_AUDIO_SOUND_MODE_STEREO == info.stAiAttr[u32DevIdx].eSoundmode))
        {
            u32ChnIdx = 0;
            u32ChnCnt = info.stAiAttr[u32DevIdx].u32ChnCnt;
        }
        else if (E_MI_AUDIO_SOUND_MODE_QUEUE == info.stAiAttr[u32DevIdx].eSoundmode)
        {
            u32ChnIdx = 0;
            u32ChnCnt = 1;
        }

        for (; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            pthread_join(stAiChnPriv[u32DevIdx][u32ChnIdx].getFrameThreadId, NULL);
            ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
        }
        ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);
    }

    return 0;
}

MI_S32 deinitAo(void)
{
    MI_U32 u32RunningDevMaxOfChip;
    MI_AUDIO_DEV AoDevId;
    MI_U32 u32DevIdx;

    if (0 == strcmp("Mercury6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Pioneer3", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 2;
    }
    else if (0 == strcmp("Infinity2M", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6E", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6B0", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }
    else if (0 == strcmp("Infinity6", (const char*)(info.u8ChipName)))
    {
        u32RunningDevMaxOfChip = 1;
    }

    for (u32DevIdx = 0; (u32DevIdx < info.u32AoDevNumFromConfigFile) && (u32DevIdx < u32RunningDevMaxOfChip); u32DevIdx++)
    {
        pthread_join(stAoChnPriv[u32DevIdx].sendFrameThreadId, NULL);
        AoDevId = info.s32AoDevId[u32DevIdx];
        ExecFunc(MI_AO_DisableChn(AoDevId, 0), MI_SUCCESS);
        ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);
    }
    return 0;
}

int checkCustomerConfigJsonFileReadable(MI_U8* pu8CustomerConfigJsonFile)
{
    MI_S32 s32Ret = 0;
    if (NULL == pu8CustomerConfigJsonFile)
    {
        printf("customer config json file path is NULL.\n");
        return FALSE;
    }

    s32Ret = access((const char *)pu8CustomerConfigJsonFile, R_OK | F_OK);
    if (-1 == s32Ret)
    {
        printf("customer config json file is not exist or readable.");
        return FALSE;
    }

    return TRUE;
}

int loadCustomerConfigJsonFile(MI_U8* pu8CustomerConfigJsonFile)
{
    FILE*       pCustomerConfigJsonFile = NULL;
    MI_U32      u32FileSize;
    MI_U8*      pu8JsonString = NULL;
    cJSON*      pJsonRoot = NULL;
    cJSON*      pChipNode = NULL;
    cJSON*      pEnableFlagNode = NULL;
    cJSON*      pDeviceInfo = NULL;
    cJSON*      pDevIdNode = NULL;
    cJSON*      pSampleRateNode = NULL;
    cJSON*      pBitWidthNode = NULL;
    cJSON*      pWorkModeNode = NULL;
    cJSON*      pSoundModeNode = NULL;
    cJSON*      pChannelCountNode = NULL;
    cJSON*      pI2sInfoNode = NULL;
    cJSON*      pI2sFormatNode = NULL;
    cJSON*      pI2sMclkNode = NULL;
    cJSON*      pI2sSyncClockNode = NULL;
    cJSON*      pI2sTdmSlotsNode = NULL;
    cJSON*      pI2sBitWidthNode = NULL;
    cJSON*      pDumpPathNode = NULL;
    cJSON*      pPlaybackFileNode = NULL;
    cJSON*      pAiVolumeNode = NULL;
    cJSON*      pAoVolumeNode = NULL;
    MI_U8       u8DevIdx, u8DevNum;
    MI_U8       u8AiGainIdx;
    MI_S32      s32Ret = FALSE;

    memset(&info, 0x0, sizeof(info));

    if (NULL == pu8CustomerConfigJsonFile)
    {
        printf("customer config json file path is NULL.\n");
        goto ERR_RETURN;
    }

    pCustomerConfigJsonFile = fopen((const char *)pu8CustomerConfigJsonFile, "r+");
    if (NULL == pCustomerConfigJsonFile)
    {
        printf("failed to open customer config file.\n");
        goto ERR_RETURN;
    }

    fseek(pCustomerConfigJsonFile, 0, SEEK_END);
    u32FileSize = ftell(pCustomerConfigJsonFile);
    fseek(pCustomerConfigJsonFile, 0, SEEK_SET);

    pu8JsonString = (MI_U8*)malloc(JSON_BUF_SIZE);
    if (NULL == pu8JsonString)
    {
        printf("failed to malloc buf for json.\n");
        goto CLOSE_FILE;
    }
    else
    {
        memset(pu8JsonString, 0x0, JSON_BUF_SIZE);
    }

    s32Ret = fread(pu8JsonString, u32FileSize, 1, pCustomerConfigJsonFile);
    if (s32Ret != 1)
    {
        printf("failed to read json string from customer config file.\n");
        goto FREE_JSON_BUF;
    }

    pJsonRoot = cJSON_Parse((const char *)pu8JsonString);
    if (NULL == pJsonRoot)
    {
        printf("failed to parse json string.\n");
        goto FREE_JSON_BUF;
    }

    pChipNode = cJSON_GetObjectItem(pJsonRoot, "chip");
    if (NULL == pChipNode)
    {
        printf("failed to find \"chip\" node from json file.\n");
        goto DELECT_JSON_ROOT;
    }

    strcpy((char *)(info.u8ChipName), pChipNode->valuestring);

    pEnableFlagNode = cJSON_GetObjectItem(pJsonRoot, "enable audio input");
    if (NULL == pEnableFlagNode)
    {
        printf("failed to find \"enable audio input\" node from json file.\n");
        goto DELECT_JSON_ROOT;
    }

    info.bEnableAI = pEnableFlagNode->valueint;

    if (TRUE == info.bEnableAI)
    {
        pDumpPathNode = cJSON_GetObjectItem(pJsonRoot, "dump path");
        if (NULL == pDumpPathNode)
        {
            printf("failed to find \"dump path\" node from json file.\n");
            goto DELECT_JSON_ROOT;
        }

        strcpy((char *)(info.u8DumpDataPath), pDumpPathNode->valuestring);

        pDeviceInfo = cJSON_GetObjectItem(pJsonRoot, "audio input device");
        if (NULL == pDeviceInfo)
        {
            printf("failed to find \"audio input device\" node from json file.\n");
            goto DELECT_JSON_ROOT;
        }

        u8DevNum = cJSON_GetArraySize(pDeviceInfo);

        for (u8DevIdx = 0; ((u8DevIdx < AI_RUNNING_MAX_DEV_NUM) && (u8DevIdx < u8DevNum)); u8DevIdx++)
        {
            pDevIdNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "device id");
            if (NULL == pDevIdNode)
            {
                printf("failed to find \"device id\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }
            info.s32AiDevId[u8DevIdx] = pDevIdNode->valueint;

            pSampleRateNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "sample rate");
            if (NULL == pSampleRateNode)
            {
                printf("failed to find \"sample rate\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            info.stAiAttr[u8DevIdx].eSamplerate = pSampleRateNode->valueint;

            pBitWidthNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "bit width");
            if (NULL == pBitWidthNode)
            {
                printf("failed to find \"bit width\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pBitWidthNode->valueint)
            {
                case 16:
                    info.stAiAttr[u8DevIdx].eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
                    break;
            }

            pWorkModeNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "work mode");
            if (NULL == pWorkModeNode)
            {
                printf("failed to find \"work mode\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pWorkModeNode->valueint)
            {
                case 0:
                    info.stAiAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
                    break;
                case 1:
                    info.stAiAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
                    break;
                case 2:
                    info.stAiAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_TDM_MASTER;
                    break;
                case 3:
                    info.stAiAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_TDM_SLAVE;
                    break;
            }

            pSoundModeNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "sound mode");
            if (NULL == pSoundModeNode)
            {
                printf("failed to find \"sound mode\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pSoundModeNode->valueint)
            {
                case 0:
                    info.stAiAttr[u8DevIdx].eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
                    break;
                case 1:
                    info.stAiAttr[u8DevIdx].eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
                    break;
                case 2:
                    info.stAiAttr[u8DevIdx].eSoundmode = E_MI_AUDIO_SOUND_MODE_QUEUE;
                    break;
            }

            pChannelCountNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "channel count");
            if (NULL == pChannelCountNode)
            {
                printf("failed to find \"channel count\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            info.stAiAttr[u8DevIdx].u32ChnCnt = pChannelCountNode->valueint;

            pAiVolumeNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "volume");
            if (NULL != pAiVolumeNode)
            {
                for (u8AiGainIdx = 0; u8AiGainIdx < cJSON_GetArraySize(pAiVolumeNode) || u8AiGainIdx < 2; u8AiGainIdx++)
                {
                    info.s32AiVolume[u8DevIdx][u8AiGainIdx] = cJSON_GetArrayItem(pAiVolumeNode, u8AiGainIdx)->valueint;
                }
            }

            pI2sInfoNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "i2s info");
            if (NULL == pI2sInfoNode)
            {
                printf("failed to find \"i2s info\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            pI2sFormatNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s format");
            if (NULL == pI2sFormatNode)
            {
                printf("failed to find \"i2s format\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch(pI2sFormatNode->valueint)
            {
                case 0:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
                    break;

                case 1:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB;
                    break;
            }

            pI2sMclkNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s mclk");
            if (NULL == pI2sMclkNode)
            {
                printf("failed to find \"i2s mclk\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch(pI2sMclkNode->valueint)
            {
                case 0:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
                    break;

                case 1:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_12_288M;
                    break;

                case 2:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_16_384M;
                    break;

                case 3:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_18_432M;
                    break;

                case 4:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_24_576M;
                    break;

                case 5:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_24M;
                    break;

                case 6:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_48M;
                    break;
            }

            pI2sSyncClockNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s sync clock");
            if (NULL == pI2sSyncClockNode)
            {
                printf("failed to find \"i2s sync clock\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pI2sSyncClockNode->valueint)
            {
                case 0:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
                    break;

                case 1:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
                    break;
            }

            pI2sTdmSlotsNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s tdm slots");
            if (NULL == pI2sTdmSlotsNode)
            {
                printf("failed to find \"i2s tdm slots\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots = pI2sTdmSlotsNode->valueint;

            pI2sBitWidthNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s bit width");
            if (NULL == pI2sBitWidthNode)
            {
                printf("failed to find \"i2s bit width\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pI2sBitWidthNode->valueint)
            {
                case 16:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;
                    break;

                case 24:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_24;
                    break;

                case 32:
                    info.stAiAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_32;
                    break;

                default:
                    break;
            }
        }

        info.u32AiDevNumFromConfigFile = u8DevIdx;
    }

    pEnableFlagNode = cJSON_GetObjectItem(pJsonRoot, "enable audio output");
    if (NULL == pEnableFlagNode)
    {
        printf("failed to find \"enable audio output\" node from json file.\n");
        goto DELECT_JSON_ROOT;
    }

    info.bEnableAO = pEnableFlagNode->valueint;

    if (TRUE == info.bEnableAO)
    {
        pDeviceInfo = cJSON_GetObjectItem(pJsonRoot, "audio output device");
        if (NULL == pDeviceInfo)
        {
            printf("failed to find \"audio output device\" node from json file.\n");
            goto DELECT_JSON_ROOT;
        }

        u8DevNum = cJSON_GetArraySize(pDeviceInfo);

        for (u8DevIdx = 0; ((u8DevIdx < AO_RUNNING_MAX_DEV_NUM) && (u8DevIdx < u8DevNum)); u8DevIdx++)
        {
            pDevIdNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "device id");
            if (NULL == pDevIdNode)
            {
                printf("failed to find \"device id\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }
            info.s32AoDevId[u8DevIdx] = pDevIdNode->valueint;

            pWorkModeNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "work mode");
            if (NULL == pWorkModeNode)
            {
                printf("failed to find \"work mode\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pWorkModeNode->valueint)
            {
                case 0:
                    info.stAoAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
                    break;
                case 1:
                    info.stAoAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
                    break;
                case 2:
                    info.stAoAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_TDM_MASTER;
                    break;
                case 3:
                    info.stAoAttr[u8DevIdx].eWorkmode = E_MI_AUDIO_MODE_TDM_SLAVE;
                    break;
            }

            pAoVolumeNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "volume");
            if (NULL != pAoVolumeNode)
            {
                info.s32AoVolume[u8DevIdx] = pAoVolumeNode->valueint;
            }

            pPlaybackFileNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "playback file");
            if (NULL == pPlaybackFileNode)
            {
                printf("failed to find \"playback file\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            strcpy((char *)(info.u8PlayBackPath[u8DevIdx]), pPlaybackFileNode->valuestring);

            pI2sInfoNode = cJSON_GetObjectItem(cJSON_GetArrayItem(pDeviceInfo, u8DevIdx), "i2s info");
            if (NULL == pI2sInfoNode)
            {
                printf("failed to find \"i2s info\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            pI2sFormatNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s format");
            if (NULL == pI2sFormatNode)
            {
                printf("failed to find \"i2s format\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch(pI2sFormatNode->valueint)
            {
                case 0:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
                    break;

                case 1:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_LEFT_JUSTIFY_MSB;
                    break;
            }

            pI2sMclkNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s mclk");
            if (NULL == pI2sMclkNode)
            {
                printf("failed to find \"i2s mclk\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch(pI2sMclkNode->valueint)
            {
                case 0:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
                    break;

                case 1:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_12_288M;
                    break;

                case 2:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_16_384M;
                    break;

                case 3:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_18_432M;
                    break;

                case 4:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_24_576M;
                    break;

                case 5:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_24M;
                    break;

                case 6:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_48M;
                    break;
            }

            pI2sSyncClockNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s sync clock");
            if (NULL == pI2sSyncClockNode)
            {
                printf("failed to find \"i2s sync clock\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pI2sSyncClockNode->valueint)
            {
                case 0:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
                    break;

                case 1:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
                    break;
            }

            pI2sTdmSlotsNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s tdm slots");
            if (NULL == pI2sTdmSlotsNode)
            {
                printf("failed to find \"i2s tdm slots\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.u32TdmSlots = pI2sTdmSlotsNode->valueint;

            pI2sBitWidthNode = cJSON_GetObjectItem(pI2sInfoNode, "i2s bit width");
            if (NULL == pI2sBitWidthNode)
            {
                printf("failed to find \"i2s bit width\" node from json file.\n");
                goto DELECT_JSON_ROOT;
            }

            switch (pI2sBitWidthNode->valueint)
            {
                case 16:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;
                    break;

                case 24:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_24;
                    break;

                case 32:
                    info.stAoAttr[u8DevIdx].WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_32;
                    break;

                default:
                    break;
            }
        }

        info.u32AoDevNumFromConfigFile = u8DevIdx;
    }

    s32Ret = TRUE;

DELECT_JSON_ROOT:
    cJSON_Delete(pJsonRoot);
FREE_JSON_BUF:
    free(pu8JsonString);
CLOSE_FILE:
    fclose(pCustomerConfigJsonFile);
ERR_RETURN:
    return s32Ret;
}

void display_help(void)
{
    printf("----- audio demo -----\n");
    printf("-t : AI/AO run time(s)\n");
    printf("-f : audio config json file\n");
    return;
}

int main (int argc, char *argv[])
{
    MI_S32      s32Opt = 0;

    // parsing command line
    while ((s32Opt = getopt(argc, argv, "t:f:")) != -1)
    {
        switch(s32Opt)
        {
            // run time
            case 't':
                {
                    u32RunTime = atoi(optarg);
                }
                break;

            // config file
            case 'f':
                {
                    pu8CustomerConfigJsonFile = (MI_U8 *)(optarg);
                }
                break;

            case '?':
                if(optopt == 't')
                {
                    printf("Missing run time, please -t 'run time second' \n");
                }
                else if(optopt == 'f')
                {
                    printf("Missing customer config file, please -f 'customer config file path' \n");
                }
                else
                {
                    printf("Invalid option received \n");
                }

            default:
                display_help();
                exit(0);
        }
    }

    if (NULL == pu8CustomerConfigJsonFile)
    {
        display_help();
        exit(0);
    }

    if (FALSE == checkCustomerConfigJsonFileReadable(pu8CustomerConfigJsonFile))
    {
        exit(0);
    }

    if (FALSE == loadCustomerConfigJsonFile(pu8CustomerConfigJsonFile))
    {
        exit(0);
    }

    if (FALSE == checkCustomerConfigJsonFile())
    {
        exit(0);
    }

    initTimer();
    printJsonInfo();

    ExecFunc(MI_SYS_Init(), MI_SUCCESS);

    // enable ai
    if (info.bEnableAI)
    {
        ExecFunc(initAi(), MI_SUCCESS);
    }

    // enable ao
    if (info.bEnableAO)
    {
        ExecFunc(initAo(), MI_SUCCESS);
    }

    // disable ai
    if (info.bEnableAI)
    {
        ExecFunc(deinitAi(), MI_SUCCESS);
    }

    // disable ao
    if (info.bEnableAO)
    {
        ExecFunc(deinitAo(), MI_SUCCESS);
    }

    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
    return 0;
}
