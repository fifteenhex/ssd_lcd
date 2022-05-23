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

#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_ai.h"
#include "mi_ao.h"

/* add the header file start */
#include "mi_aio_datatype.h"
#include "mi_ao_datatype.h"

#include "AudioSRCProcess.h"
#include "AudioProcess.h"
#include "AudioAecProcess.h"
#include "AudioAedProcess.h"
#include "AudioSslProcess.h"
#include "AudioBfProcess.h"

#include "g711.h"
#include "g726.h"
/* add the header file end */

#define AUDIO_DEMO_ADD_ALG   (1)
#define AUDIO_DEMO_TEMP_SIZE (50 * 1024)

#define WAV_G711A 	(0x06)
#define WAV_G711U 	(0x07)
#define WAV_G726 	(0x45)

#define WAV_PCM  	(0x1)

#define G726_16 	(2)
#define G726_24 	(3)
#define G726_32 	(4)
#define G726_40 	(5)

// AENC Marco
#define AUDIO_AI_G711A 0
#define AUDIO_AI_G711U 1
#define AUDIO_AI_AENC_G726_UNIT (16)
#define AUDIO_AI_AENC_G726_UNIT_MAX (1200)

// ADEC Marco
#define AUDIO_AO_G711A 0
#define AUDIO_AO_G711U 1
#define AUDIO_AO_ADEC_G726_UNIT (60)
#define AUDIO_AO_ADEC_G726_UNIT_MAX (1200)

#define APP_AI_DUMP_AEC_ENABLE  (1)

#define AI_AMIC_CHN_MAX 			(2)

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
    || defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
#define AI_DMIC_CHN_MAX				(2)
#elif (defined (CONFIG_SIGMASTAR_CHIP_I2M) \
           || defined (CONFIG_SIGMASTAR_CHIP_M6) \
           || defined (CONFIG_SIGMASTAR_CHIP_P3))
#define AI_DMIC_CHN_MAX		        (4)
#endif

#define AI_I2S_NOR_CHN 				(2)
#define AI_LINE_IN_CHN_MAX 	        (2)

#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
#define AI_AMIC_AND_I2S_RX_CHN_MAX 	(4)
#define AI_DMIC_AND_I2S_RX_CHN_MAX 	(4)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0))
#define AI_DEV_ID_MAX   (4)
#elif (defined(CONFIG_SIGMASTAR_CHIP_I6))
#define AI_DEV_ID_MAX   (3)
#elif (defined CONFIG_SIGMASTAR_CHIP_I2M)
#define AI_DEV_ID_MAX   (6)
#elif (defined CONFIG_SIGMASTAR_CHIP_M6)
#define AI_DEV_ID_MAX   (5)
#elif (defined CONFIG_SIGMASTAR_CHIP_P3)
#define AI_DEV_ID_MAX   (5)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_M6) || defined(CONFIG_SIGMASTAR_CHIP_P3))
#define AO_DEV_ID_MAX   (5)
#else
#define AO_DEV_ID_MAX   (2)
#endif

#define AI_DEV_AMIC     (0)
#define AI_DEV_DMIC     (1)
#define AI_DEV_I2S_RX   (2)
#define AI_DEV_LINE_IN  (3)

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E)\
    || defined(CONFIG_SIGMASTAR_CHIP_I6B0))
#define AI_DEV_I2S_RX_AND_SRC   (4)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
#define AI_DEV_AMIC_AND_I2S_RX  (4)
#define AI_DEV_DMIC_AND_I2S_RX  (5)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_M6))
#define AI_DEV_AMIC_2_3     (4)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_P3))
#define AI_DEV_AMIC_2     (4)
#endif

#define AO_DEV_LINE_OUT  (0)
#define AO_DEV_I2S_TX   (1)

#if (defined(CONFIG_SIGMASTAR_CHIP_M6))
#define AO_DEV_DAC0   (2)
#define AO_DEV_DAC1   (3)
#define AO_DEV_HDMI_AND_LINEOUT   (4)
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_P3))
#define AO_DEV_DAC0   (2)
#define AO_DEV_DAC1   (3)
#define AO_DEV_ID_HEADPHONE         (4)
#endif

#define AI_VOLUME_AMIC_MIN      (0)
#define AI_VOLUME_AMIC_MAX      (21)
#define AI_VOLUME_LINEIN_MAX    (7)

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
#define AI_VOLUME_DMIC_MIN      (-60)
#define AI_VOLUME_DMIC_MAX      (30)
#elif (defined(CONFIG_SIGMASTAR_CHIP_I2M) \
        || defined(CONFIG_SIGMASTAR_CHIP_M6) \
        || defined(CONFIG_SIGMASTAR_CHIP_P3))
#define AI_VOLUME_DMIC_MIN      (0)
#define AI_VOLUME_DMIC_MAX      (4)
#endif

#define AO_VOLUME_MIN           (-60)
#define AO_VOLUME_MAX           (30)

#define MI_AUDIO_SAMPLE_PER_FRAME	(1024)

#define DMA_BUF_SIZE_8K     (8000)
#define DMA_BUF_SIZE_16K    (16000)
#define DMA_BUF_SIZE_32K    (32000)
#define DMA_BUF_SIZE_48K    (48000)

#define AI_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define MIU_WORD_BYTE_SIZE	(8)
#define USER_BUF_DEPTH      (8)//4->8
#define TOTAL_BUF_DEPTH		(16)//8->16

typedef enum
{
	E_AI_SOUND_MODE_MONO = 0,
	E_AI_SOUND_MODE_STEREO,
	E_AI_SOUND_MODE_QUEUE,
}  AiSoundMode_e;

#define ExecFunc(func, _ret_) \
do{	\
	MI_S32 s32TmpRet;	\
    printf("[%d] Start test: %s\n", __LINE__, #func);\
    s32TmpRet = func;	\
    if (s32TmpRet != _ret_)\
    {\
        printf("AUDIO_TEST [%d] %s exec function failed, result:0x%x\n",__LINE__, #func, s32TmpRet);\
        return 1;\
    }\
    else\
    {\
        printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);\
    }\
    printf("[%d] End test: %s\n", __LINE__, #func);	\
}while(0);

#define ExecFuncNoExit(func, _ret_, __ret) \
do{	\
	MI_S32 s32TmpRet;	\
    printf("%d Start test: %s\n", __LINE__, #func);\
    s32TmpRet = func;	\
    __ret = s32TmpRet;	\
    if (s32TmpRet != _ret_)\
    {\
        printf("AUDIO_TEST [%d] %s exec function failed, result:0x%x\n",__LINE__, #func, s32TmpRet);\
    }\
    else\
    {\
        printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);\
        printf("%d End test: %s\n", __LINE__, #func);	\
    }\
}while(0);

/*=============================================================*/
// Global Variable definition
/*=============================================================*/
typedef enum
{
    E_SOUND_MODE_MONO = 0,   /* mono */
    E_SOUND_MODE_STEREO = 1, /* stereo */
} SoundMode_e;

typedef enum
{
    E_SAMPLE_RATE_8000 = 8000,   /* 8kHz  sampling rate */
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

typedef struct AiOutFilenName_s
{
    MI_S8 *ps8OutputPath;
    MI_S8 *ps8OutputFile;
    MI_S8 *ps8SrcPcmOutputFile;
    MI_AUDIO_DEV AiDevId;
    MI_AUDIO_Attr_t stAiAttr;
    MI_BOOL bSetVqeWorkingSampleRate;
    MI_AUDIO_SampleRate_e eVqeWorkingSampleRate;
    MI_BOOL bEnableRes;
    MI_AUDIO_SampleRate_e eOutSampleRate;
    MI_BOOL bEnableAenc;
    MI_AI_AencConfig_t stSetAencConfig;
    MI_AI_VqeConfig_t stSetVqeConfig;
    MI_BOOL bEnableBf;
} AiOutFileName_t;

typedef struct AiChnPriv_s
{
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;
    MI_S32 s32Fd;
    MI_U32 u32TotalSize;
    MI_U32 u32SrcPcmTotalSize;
    MI_U32 u32ChnCnt;
    MI_BOOL bEnableAed;
    pthread_t tid;
    MI_S32 s32SrcPcmFd;
} AiChnPriv_t;

/* AO add vriable definitions start */
#define PRINTF_INFO(fmt, args...)   ({printf("[APP INFO]->%s[%d]:", \
    __FUNCTION__, __LINE__);printf(fmt, ##args);})

#if 0   /* Select Debugging Information */
#define PRINTF_DBG(fmt, args...)   ({printf("[APP DBG]->%s[%d]:", \
    __FUNCTION__, __LINE__);printf(fmt, ##args);})
#else
#define PRINTF_DBG(fmt, args...)
#endif

#define SLEEP(n_s)    sleep(n_s)

#define AO_MAX_CHN_NUM          (2)
#define AO_MAX_DEV_NUM          (6)
#define AUDIO_SAMPLE_PER_FRAME	(1024)

typedef enum
{
    E_APP_SUCCEED = 0,
    E_APP_ERR_NO_BUFFER = -2,
} RetType_e;

typedef struct _AUDIO_QueueInfo_s
{
    MI_S32 s32Front;
    MI_S32 s32Rear;
    MI_S32 s32Max;  //Max len of queue
    MI_S32 s32Size; //current size
    MI_U8  *pu8Buff;
} _AUDIO_QueueInfo_t;

typedef struct _AO_ChanInfoUsr_s
{
    MI_U8                   *pu8InputBuff;
    MI_U8                   *pu8OutputBuff;
    MI_BOOL                 bResampleEnable;
    MI_AUDIO_SampleRate_e   eInResampleRate;
    MI_BOOL                 bVqeEnable;
    MI_BOOL                 bVqeAttrSet;
    MI_AO_VqeConfig_t       stAoVqeConfig;
    MI_BOOL                 bAdecAttrSet;
    MI_BOOL                 bAdecEnable;
    MI_AO_AdecConfig_t      stAoAdecConfig;
    //Resample(SRC)
    SRC_HANDLE              hSrcHandle;
    void*                   pSrcWorkingBuf;
    //VQE
    APC_HANDLE              hApcHandle;
    void*                   pApcWorkingBuf;
    //g726
    stG726State_t            *hG726Handle;
    //Queue
    _AUDIO_QueueInfo_t      stAdecInputQueue;
    _AUDIO_QueueInfo_t      stSrcInputQueue;
    _AUDIO_QueueInfo_t      stVqeInputQueue;
    _AUDIO_QueueInfo_t      stChnTempQueue;
} _AO_ChanInfoUsr_t;

typedef struct _AO_DevInfoUsr_s
{
    MI_AUDIO_DEV            AoDevId;
    MI_AUDIO_Attr_t         stDevAttr;
    _AO_ChanInfoUsr_t       astChanInfo[AO_MAX_CHN_NUM];
} _AO_DevInfoUsr_t;

#define AUDIO_CHECK_POINTER(pPtr)  \
do{ \
    if (NULL == pPtr)   \
    {   \
        PRINTF_INFO("This is null pointer.\n"); \
        return -1;  \
    }   \
}while(0);

#define AUDIO_USER_TRANS_EMODE_TO_CHAN(u32Chan, eSoundmode) \
    switch(eSoundmode)  \
    {   \
        case E_MI_AUDIO_SOUND_MODE_MONO:        \
        case E_MI_AUDIO_SOUND_MODE_QUEUE:       \
            u32Chan = 1;    \
            break;      \
        case E_MI_AUDIO_SOUND_MODE_STEREO:   \
            u32Chan = 2;    \
            break;  \
        default:    \
            u32Chan = 0;    \
            PRINTF_INFO("eSoundmode is illegal = %u.\n", eSoundmode); \
            break; \
    }

#define AUDIO_VQE_SAMPLERATE_TRANS_TYPE(eSampleRate, eIaaSampleRate)\
    switch(eSampleRate)\
    {\
        case E_MI_AUDIO_SAMPLE_RATE_8000:\
			 eIaaSampleRate = IAA_APC_SAMPLE_RATE_8000;\
             break;\
        case E_MI_AUDIO_SAMPLE_RATE_16000:\
             eIaaSampleRate = IAA_APC_SAMPLE_RATE_16000;\
             break;\
        case E_MI_AUDIO_SAMPLE_RATE_48000:\
             eIaaSampleRate = IAA_APC_SAMPLE_RATE_48000;\
             break;\
        default:\
             PRINTF_INFO("eIaaSampleRate is illegal: %d\n", eSampleRate);\
             break;\
    }

#define AUDIO_VQE_NR_SPEED_TRANS_TYPE(eNrSpeed, eIaaNrSpeed)\
    switch(eNrSpeed)\
    {\
        case E_MI_AUDIO_NR_SPEED_LOW:\
            eIaaNrSpeed = NR_SPEED_LOW;\
            break;\
        case E_MI_AUDIO_NR_SPEED_MID:\
            eIaaNrSpeed = NR_SPEED_MID;\
            break;\
        case E_MI_AUDIO_NR_SPEED_HIGH:\
            eIaaNrSpeed = NR_SPEED_HIGH;\
            break;\
        default:\
            PRINTF_INFO("eIaaNrSpeed is illegal: %d\n", eNrSpeed);\
            break;\
    }

#define AUDIO_VQE_HPF_TRANS_TYPE(eHpfFreq, eIaaHpfFreq)          \
    switch(eHpfFreq)  \
    {   \
        case E_MI_AUDIO_HPF_FREQ_80:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_80;    \
            break;  \
        case E_MI_AUDIO_HPF_FREQ_120:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_120;    \
            break;  \
         case E_MI_AUDIO_HPF_FREQ_150:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_150; \
            break; \
        default:    \
            eIaaHpfFreq = AUDIO_HPF_FREQ_BUTT; \
            PRINTF_INFO("eHpfFreq is illegal = %d \n", eHpfFreq); \
            break; \
    }

static _AO_DevInfoUsr_t _gastAoDevInfoApp[AO_MAX_DEV_NUM] =
{
    {
        .AoDevId = 0,
    },
    {
        .AoDevId = 1,
    },
    {
        .AoDevId = 2,
    },
    {
        .AoDevId = 3,
    },
    {
        .AoDevId = 4,
    },
    {
        .AoDevId = 5,
    },
};

static const MI_U32 u32AoSrcPointNumber = 256;
/* AO add vriable definitions end */

/* AI add vriable definitions start */
#define AED_THRESHOLD_DB_MIN    (-80)
#define AED_THRESHOLD_DB_MAX    (0)

#define AED_OPERATING_POINT_MIN (-10)
#define AED_OPERATING_POINT_MAX (10)

#define AI_MAX_DEV_NUM          (6)
#define AI_MAX_CHN_NUM          (8)

#define AI_DEV_ID_AMIC          (0)
#define AI_DEV_ID_AMIC_AND_I2S_RX   (4)

#define AI_DEV_ID_I2S_RX            (2)

/* #define AUDIO_VQE_SAMPLES_UNIT   (256) //ver.2020.1019.0000  only support point_number = 256 */
#define AUDIO_VQE_SAMPLES_UNIT   (128)
#define	AUDIO_ALGORITHM_SAMPLES_UNIT (128)
#define AUDIO_BUFFER_SIZE_RATIO (1)
#define AI_BUFFER_SIZE_RATIO (1)

#define AUDIO_VQE_SAMPLERATE_TRANS_TYPE(eSampleRate, eIaaSampleRate)\
    switch(eSampleRate)\
    {\
        case E_MI_AUDIO_SAMPLE_RATE_8000:\
			 eIaaSampleRate = IAA_APC_SAMPLE_RATE_8000;\
             break;\
        case E_MI_AUDIO_SAMPLE_RATE_16000:\
             eIaaSampleRate = IAA_APC_SAMPLE_RATE_16000;\
             break;\
        case E_MI_AUDIO_SAMPLE_RATE_48000:\
             eIaaSampleRate = IAA_APC_SAMPLE_RATE_48000;\
             break;\
        default:\
             PRINTF_INFO("eIaaSampleRate is illegal %d\n", eSampleRate);\
             break;\
    }

#define AUDIO_VQE_NR_SPEED_TRANS_TYPE(eNrSpeed, eIaaNrSpeed)\
    switch(eNrSpeed)\
    {\
        case E_MI_AUDIO_NR_SPEED_LOW:\
            eIaaNrSpeed = NR_SPEED_LOW;\
            break;\
        case E_MI_AUDIO_NR_SPEED_MID:\
            eIaaNrSpeed = NR_SPEED_MID;\
            break;\
        case E_MI_AUDIO_NR_SPEED_HIGH:\
            eIaaNrSpeed = NR_SPEED_HIGH;\
            break;\
        default:\
            PRINTF_INFO("eIaaNrSpeed is illegal %d\n", eNrSpeed);\
            break;\
    }

#define AUDIO_VQE_HPF_TRANS_TYPE(eHpfFreq, eIaaHpfFreq)          \
    switch(eHpfFreq)  \
    {   \
        case E_MI_AUDIO_HPF_FREQ_80:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_80;    \
            break;  \
        case E_MI_AUDIO_HPF_FREQ_120:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_120;    \
            break;  \
         case E_MI_AUDIO_HPF_FREQ_150:   \
            eIaaHpfFreq = AUDIO_HPF_FREQ_150; \
            break; \
        default:    \
            eIaaHpfFreq = AUDIO_HPF_FREQ_BUTT; \
            PRINTF_INFO("eHpfFreq is illegal = %d \n", eHpfFreq); \
            break; \
    }

typedef struct _AI_ChanInfoUsr_s
{
    MI_AUDIO_DEV            AoDevId;
    MI_AO_CHN               AoChn;
    //Resample for Vqe
    SRC_HANDLE              hSrcHandleForVqeAiIn;
    void*                   pSrcWorkingBufForVqeAiIn;
    _AUDIO_QueueInfo_t      stSrcQueueForVqeAiIn;
    SRC_HANDLE              hSrcHandleForVqeAoIn;
    void*                   pSrcWorkingBufForVqeAoIn;
    _AUDIO_QueueInfo_t      stSrcQueueForVqeAoIn;
    MI_BOOL                 bResampleEnableForVqe;
    //Resample(SRC)
    SRC_HANDLE              hSrcHandle;
    void*                   pSrcWorkingBuf;
    MI_BOOL                 bResampleEnable;
    MI_AUDIO_SampleRate_e   eOutResampleRate;
    //VQE
    MI_BOOL                 bVqeEnable;
    MI_BOOL                 bVqeAttrSet;
    MI_AI_VqeConfig_t       stAiVqeConfig;
    APC_HANDLE              hApcHandle;
    void*                   pApcWorkingBuf;
    //AEC
    AEC_HANDLE              hAecHandle;
    void*                   pAecWorkingBuf;
    //Aenc
    MI_BOOL                 bAencAttrSet;
    MI_BOOL                 bAencEnable;
    MI_AI_AencConfig_t      stAiAencConfig;
    //AED
    AedHandle               hAedHandle;
    MI_BOOL                 bAedAttrSet;
    MI_BOOL                 bAedEnable;
    MI_AI_AedConfig_t       stAiAedConfig;
    MI_BOOL                 bAcousticEventDetected;
    MI_BOOL                 bLoudSoundDetected;
    //SSL
    MI_BOOL                 bSslInitAttrSet;
    MI_BOOL                 bSslConfigAttrSet;
    MI_BOOL                 bSslEnable;
    MI_AI_SslInitAttr_t     stSslInitAttr;
    MI_AI_SslConfigAttr_t   stSslConfigAttr;
    SSL_HANDLE              hSslHandle;     //new alg
    void*                   pSslWorkingBuf; //for malloc
    MI_U32                  u32SslFrameIndex;
    MI_S32                  s32SslDoa;
    //BF
    MI_BOOL                 bBfInitAttrSet;
    MI_BOOL                 bBfConfigAttrSet;
    MI_BOOL                 bAiEnableBf;
    MI_AI_BfInitAttr_t      stBfInitAttr;
    MI_AI_BfConfigAttr_t    stBfConfigAttr;
    BF_HANDLE               hBfHandle;
    void*                   pBfWorkingBuf;
    MI_BOOL                 bSetBfDoa;
    MI_S32                  s32BfDoa;
    //Queue
    _AUDIO_QueueInfo_t      stSrcQueue;     // for IaaSrc
    _AUDIO_QueueInfo_t      stAecQueue;     // for IaaAec
    _AUDIO_QueueInfo_t      stAecRefQueue;  // for IaaAec, save ao data
    _AUDIO_QueueInfo_t      stApcQuque;     // for IaaVqe
    _AUDIO_QueueInfo_t      stAencQueue;    // for g711/g726 encoder
    _AUDIO_QueueInfo_t      stAedQueue;     // for baby cry
    _AUDIO_QueueInfo_t      stSslQueue;     // for ssl
    _AUDIO_QueueInfo_t      stBfQueue;      // for bf
    _AUDIO_QueueInfo_t      stChnQueue;     // for channel
    _AUDIO_QueueInfo_t      stGarbageQueue; // for echo data
    _AUDIO_QueueInfo_t      stTempChnQueue;
    //g726
    stG726State_t            *hG726Handle;
    //Tmp buff
    MI_U8                   *pu8InputBuff;
    MI_U8                   *pu8OutputBuff;
} _AI_ChanInfoUsr_t;

typedef struct _AI_DevInfoUsr_s
{
    MI_AUDIO_DEV            AiDevId;
    MI_AUDIO_Attr_t         stDevAttr;
    _AI_ChanInfoUsr_t       astChanInfo[AI_MAX_CHN_NUM];
    MI_U64                  u64PhyBufAddr;   // for DMA HW address
} _AI_DevInfoUsr_t;

#define  AI_INPUT_FILE_FOR_TEST_ALG   (1)
#define _AI_SRC_ENABLE_ 	(1)
#define _AI_APC_ENABLE_	    (1)
#define _AI_BF_ENABLE_		(1)
#define _AI_AEC_ENABLE_	    (1)
#define _AI_SSL_ENABLE_		(1)
#define _AI_AED_ENABLE_		(1)
#define _AI_AENC_ENABLE_	(1)

static _AI_DevInfoUsr_t _gastAiDevInfoApp[AI_MAX_DEV_NUM]=
{
    {
        .AiDevId = 0, //AI_DEV_ID_AMIC = 0
    },
    {
        .AiDevId = 1, //AI_DEV_ID_DMIC = 1
    },
    {
        .AiDevId = 2, //AI_DEV_ID_I2S_RX = 2
    },
    {
        .AiDevId = 3, //AI_DEV_ID_LINE_IN = 3
    },
    {
        .AiDevId = 4, //AI_DEV_ID_AMIC_AND_I2S_RX = 4
    },
    {
        .AiDevId = 5, //AI_DEV_ID_DMIC_AND_I2S_RX = 5
    },
};

static const MI_U32 u32AiSrcPointNumber = 256;

FILE * fpAiTestAlg = NULL;
FILE * fpAiTestAlgForAec = NULL;
static MI_U8*   pu8AiTestAlgPath = NULL;
static MI_U8*   pu8AiTestAlgForAecPath = NULL;

#if (defined(APP_AI_DUMP_AEC_ENABLE))   //AI dump Aec files
FILE * fpAiDumpAecNear = NULL;
FILE * fpAiDumpAecFar = NULL;
FILE * fpAiDumpAecOutput = NULL;

FILE * fpAiDumpMiFile[2] = {NULL, NULL};
#endif  //AI dump Aec files

static MI_BOOL bIsAecRefQueue = FALSE;
/* AI add vriable definitions end */

static MI_BOOL  bEnableAI = FALSE;
static MI_U8*   pu8AiOutputPath = NULL;
static MI_BOOL  bAiEnableVqe = FALSE;
static MI_BOOL  bAiEnableHpf = FALSE;
static MI_BOOL  bAiEnableAgc = FALSE;
static MI_BOOL  bAiEnableNr = FALSE;
static MI_BOOL  bAiEnableEq = FALSE;
static MI_BOOL  bAiEnableAec = FALSE;
static MI_BOOL  bAiEnableResample = FALSE;
static MI_BOOL	bAiEnableSsl = FALSE;
static MI_BOOL	bAiEnableBf = FALSE;
static MI_AUDIO_SampleRate_e eAiOutputResampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_BOOL  bAiEnableAenc = FALSE;
static MI_AUDIO_AencType_e eAiAencType = E_MI_AUDIO_AENC_TYPE_INVALID;
static MI_AUDIO_G726Mode_e eAiAencG726Mode = E_MI_AUDIO_G726_MODE_INVALID;
static SoundMode_e eAiWavSoundMode = E_SOUND_MODE_MONO;
static AencType_e eAiWavAencType = PCM;
static MI_BOOL  bAiEnableAed = FALSE;
static MI_U32   u32AiChnCnt = 0;

static MI_BOOL  bAiEnableResampleForVqe = FALSE;
static MI_BOOL  bAiEnableApc = FALSE;

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
static MI_BOOL  bAiEnableHwAec = FALSE;
#endif

static MI_S32   s32AiVolume = 0;
static MI_BOOL  bAiSetVolume = FALSE;
static MI_AUDIO_DEV AiDevId = 0;  //add
static MI_AUDIO_SampleRate_e eAiWavSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_AUDIO_SampleRate_e eAiSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static AiSoundMode_e eAiSoundMode = E_AI_SOUND_MODE_MONO;
static MI_BOOL 	bAiI2sNormalMode = TRUE;
static MI_U32	u32MicDistance = 0;
static MI_U32 	u32VqeWorkingSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_BOOL  bAiDumpPcmData = FALSE;
static MI_BOOL  bAiSetBfDoa = FALSE;
static MI_S32   s32AiBfDoa = 0;

static MI_BOOL  bEnableAO = FALSE;
static MI_U8*   pu8AoInputPath = NULL;
static MI_BOOL  bAoEnableVqe = FALSE;
static MI_BOOL  bAoEnableHpf = FALSE;
static MI_BOOL  bAoEnableAgc = FALSE;
static MI_BOOL  bAoEnableNr = FALSE;
static MI_BOOL  bAoEnableEq = FALSE;
static MI_BOOL  bAoEnableResample = FALSE;
static MI_BOOL  bAoEnableAdec = FALSE;
static MI_AUDIO_SampleRate_e eAoOutSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_AUDIO_SampleRate_e eAoInSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_S32   s32AoVolume = 0;
static MI_BOOL  bAoSetVolume = FALSE;
static MI_AUDIO_DEV AoDevId = 0;
static MI_U32   u32RunTime = 0;
static MI_BOOL  bAiExit = FALSE;
static MI_BOOL  bAoExit = FALSE;

static MI_S32   AiChnFd[MI_AUDIO_MAX_CHN_NUM] = {[0 ... MI_AUDIO_MAX_CHN_NUM-1] = -1};
static MI_S32   AiChnSrcPcmFd[MI_AUDIO_MAX_CHN_NUM] = {[0 ... MI_AUDIO_MAX_CHN_NUM-1] = -1};

static AiChnPriv_t stAiChnPriv[MI_AUDIO_MAX_CHN_NUM];

static MI_S32   AoReadFd = -1;
static WaveFileHeader_t stWavHeaderInput;
static MI_AO_CHN AoChn = 0;
static MI_S32 s32NeedSize = 0, s32SendFrameSize = 0;
static pthread_t Aotid;

static MI_AI_CHN AiChn = 0;

MI_U8 u8TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 4];  //MI_AUDIO_SAMPLE_PER_FRAME = 1024
MI_U8 u8TempBuffer[MI_AUDIO_SAMPLE_PER_FRAME * 4];

MI_AUDIO_HpfConfig_t stHpfCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .eHpfFreq = E_MI_AUDIO_HPF_FREQ_150,
};

MI_AI_AecConfig_t stAecCfg = {
    .bComfortNoiseEnable = FALSE,
    .s16DelaySample = 0,
    .u32AecSupfreq = {4, 6, 36, 49, 50, 51},
    .u32AecSupIntensity = {5, 4, 4, 5, 10, 10, 10},
};

MI_AUDIO_AnrConfig_t stAnrCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    .u32NrIntensityBand = {20, 40 ,60, 80, 100, 120},
    .u32NrIntensity = {15, 15, 15, 15, 15, 15, 15},
    .u32NrSmoothLevel = 10,
    .eNrSpeed = E_MI_AUDIO_NR_SPEED_MID,
};

MI_AUDIO_AgcConfig_t stAgcCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s32NoiseGateDb = -60,
    .s32DropGainThreshold= -3,
    .stAgcGainInfo = {
        .s32GainInit = 0,
        .s32GainMax = 20,
        .s32GainMin = 0,
    },
    .u32AttackTime = 1,
    .s16Compression_ratio_input = {-80, -60, -40, -25, 0, 0, 0},
    .s16Compression_ratio_output = {-80, -30, -15, -10, -3, 0, 0},
    .u32DropGainMax = 12,
    .u32NoiseGateAttenuationDb = 0,
    .u32ReleaseTime = 3,
};

MI_AUDIO_EqConfig_t stEqCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s16EqGainDb = {[0 ... 128] = 3},
};

MI_AI_AedConfig_t stAedCfg = {
    .bEnableBabyCry = TRUE,
    .stBabyCryConfig = {
        .bEnableNr = TRUE,
        .eSensitivity = E_MI_AUDIO_BABYCRY_SEN_HIGH,
        .s32OperatingPoint = -10,
        .s32VadThresholdDb = -40,
    },
    .bEnableLsd = TRUE,
    .stLsdConfig = {
        .s32LsdThresholdDb = -15,
    },
};

MI_AI_SslInitAttr_t stSslInit = {
	.bBfMode = FALSE,
};

MI_AI_SslConfigAttr_t stSslConfig = {
	.s32Temperature = 25,
	.s32NoiseGateDbfs = -40,
	.s32DirectionFrameNum = 300,  //8k --> 300
};

MI_AI_BfInitAttr_t stBfInit = {
	.u32ChanCnt = 2,
};

MI_AI_BfConfigAttr_t stBfConfig = {
	.s32Temperature = 25,
	.s32NoiseGateDbfs = -40,
	.s32NoiseSupressionMode = 8,
	.s32NoiseEstimation = 1,
	.outputGain = 0.7,
};

MI_U32 u32AiDevHeapSize = 0;
MI_U32 u32AiChnOutputHeapSize = 0;
MI_U32 u32AoDevHeapSize = 0;

MI_BOOL bAllocAiDevPool = FALSE;
MI_BOOL bAllocAiChnOutputPool = FALSE;
MI_BOOL bAllocAoDevPool = FALSE;

/* Queue function start */
MI_S32 _AUDIO_QueueInit(_AUDIO_QueueInfo_t *pstQueue, MI_S32 s32Size)
{
    AUDIO_CHECK_POINTER(pstQueue);
    if (s32Size <= 0)
    {
        PRINTF_INFO("ERR_ILLEGAL_PARAM.\n");
        return -1;
    }
    pstQueue->pu8Buff = (MI_U8 *)malloc(s32Size);
    AUDIO_CHECK_POINTER(pstQueue->pu8Buff);
    pstQueue->s32Front = 0;
    pstQueue->s32Rear = 0;
    pstQueue->s32Size = 0;
    pstQueue->s32Max = s32Size;

    return 0;
}

MI_S32 _AUDIO_QueueDeinit(_AUDIO_QueueInfo_t *pstQueue)
{
    AUDIO_CHECK_POINTER(pstQueue);
    if (NULL == pstQueue->pu8Buff)
    {
        return -1;
    }

    free(pstQueue->pu8Buff);
    pstQueue->pu8Buff = NULL;
    pstQueue->s32Front = 0;
    pstQueue->s32Rear = 0;
    pstQueue->s32Size = 0;
    pstQueue->s32Max = 0;

    return 0;
}

MI_S32 _AUDIO_QueueClear(_AUDIO_QueueInfo_t *pstQueue)
{
    if (NULL != pstQueue)
    {
        pstQueue->s32Front = 0;
        pstQueue->s32Rear = 0;
        pstQueue->s32Size = 0;
    }

    return 0;
}

MI_S32 _AUDIO_QueueInsert(_AUDIO_QueueInfo_t *pstQueue, MI_U8 *pu8InputBuff, MI_S32 s32Size)
{
    MI_S32 s32Tmp = 0;

    AUDIO_CHECK_POINTER(pstQueue);
    AUDIO_CHECK_POINTER(pstQueue->pu8Buff);
    AUDIO_CHECK_POINTER(pu8InputBuff);
    if (s32Size < 0)
    {
        s32Size = 0;
    }

    s32Tmp = pstQueue->s32Size + s32Size;
    if (s32Tmp > pstQueue->s32Max)
    {
        PRINTF_DBG("Queue has not enough space.\n");
        return E_APP_ERR_NO_BUFFER;
    }

    if (pstQueue->s32Front + s32Size > pstQueue->s32Max)
    {
        s32Tmp = pstQueue->s32Max - pstQueue->s32Front;
        memcpy(pstQueue->pu8Buff + pstQueue->s32Front, pu8InputBuff, s32Tmp);
        memcpy(pstQueue->pu8Buff, pu8InputBuff + s32Tmp, s32Size - s32Tmp);
    }
    else
    {
        memcpy(pstQueue->pu8Buff + pstQueue->s32Front, pu8InputBuff, s32Size);
    }

    pstQueue->s32Front = (pstQueue->s32Front + s32Size) % pstQueue->s32Max;
    pstQueue->s32Size += s32Size;

    return 0;
}

MI_S32 _AUDIO_QueueDraw(_AUDIO_QueueInfo_t *pstQueue, MI_U8 *pu8OutputBuff, MI_S32 s32Size)
{
    MI_S32 s32Tmp = 0;

    AUDIO_CHECK_POINTER(pstQueue)
    AUDIO_CHECK_POINTER(pu8OutputBuff)
    if (s32Size < 0)
    {
        s32Size = 0;
    }

    s32Tmp = pstQueue->s32Size - s32Size;
    if (s32Tmp < 0)
    {
        PRINTF_INFO("Queue has not enough data.\n");
        PRINTF_INFO("Input Size:%d, Remain data Size:%d\n", s32Size, pstQueue->s32Size);
        return -1;
    }

    if (pstQueue->s32Rear + s32Size > pstQueue->s32Max)
    {
        s32Tmp = pstQueue->s32Max - pstQueue->s32Rear;
        memcpy(pu8OutputBuff,pstQueue->pu8Buff +  pstQueue->s32Rear, s32Tmp);
        memcpy(pu8OutputBuff + s32Tmp, pstQueue->pu8Buff, s32Size - s32Tmp);
    }
    else
    {
        memcpy(pu8OutputBuff, pstQueue->pu8Buff + pstQueue->s32Rear, s32Size);
    }

    pstQueue->s32Rear = (pstQueue->s32Rear + s32Size) % pstQueue->s32Max;
    pstQueue->s32Size -= s32Size;

    return 0;
}
/* Queue function end */

long _OsCounterGetMs(void)
{
    struct  timeval t1;
    gettimeofday(&t1, NULL);
    long T = ((1000000 * t1.tv_sec) + t1.tv_usec) / 1000;
    return T;
}

void display_help(void)
{
    printf("----- audio all test -----\n");
    printf("-t : AI/AO run time(s)\n");
	printf("AI Device Id: Amic[0] Dmic[1] I2S RX[2] Linein[3]"
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
	"Amic+I2S RX[4] Dmic+I2S RX[5]"
#endif
	"\n");
    printf("AI test option :\n");
    printf("-I : Enable AI\n");
    printf("-o : AI output Path\n");
    printf("-d : AI Device Id\n");
    printf("-m : AI Sound Mode: Mono[0] Stereo[1] Queue[2]\n");
    printf("-c : AI channel count\n");
    printf("-s : AI Sample Rate\n");
    printf("-v : AI Volume\n");
    printf("-h : AI Enable Hpf\n");
    printf("-g : AI Enable Agc\n");
    printf("-e : AI Enable Eq\n");
    printf("-n : AI Enable Nr\n");
    printf("-r : AI Resample Rate\n");
    printf("-A : AI Enable Aed\n");
    printf("-b : AI Enable SW Aec\n");
    printf("-a : AI Aenc type:g711a/g711u/g726_16/g726_24/g726_32/g726_40\n");
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
    printf("-B : AI Enable HW Aec\n");
#endif
    printf("-S : AI Enable Ssl\n");
    printf("-F : AI Enable Beamforming\n");
    printf("-M : AI mic disttance for Ssl/Bf (cm, step 1 cm)\n");
    printf("-C : AI Bf doc (0~180)\n");
    printf("-w : AI Aec Working Sample Rate(Not necessary)\n");
    printf("-W : AI enable dump pcm data\n");
    printf("\n\n");
	printf("AO Device Id: Lineout[0] I2S TX[1]\n");
    printf("AO test option :\n");
    printf("-O : Enable AO\n");
    printf("-i : AO Input Path\n");
    printf("-D : AO Device Id\n");
    printf("-V : AO Volume\n");
    printf("-H : AO Enable Hpf\n");
    printf("-G : AO Enable Agc\n");
    printf("-E : AO Enable Eq\n");
    printf("-N : AO Enable Nr\n");
    printf("-R : AO Resample Rate\n");
    return;
}

void signalHandler(int signo)
{
    switch (signo){
        case SIGALRM:
        case SIGINT:
            PRINTF_INFO("Catch signal !\n");
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

MI_BOOL checkParam(void)
{
    MI_BOOL bCheckPass = FALSE;
    do{
        if ((FALSE == bEnableAI) && (FALSE == bEnableAO))
        {
            PRINTF_INFO("Neither AI nor AO enabled!!!\n");
            break;
        }

        if (bEnableAI)
        {
            if (NULL == pu8AiOutputPath)
            {
                PRINTF_INFO("Ai output path invalid!!!\n");
                break;
            }

            if (AiDevId < 0 || AiDevId > AI_DEV_ID_MAX)
            {
                PRINTF_INFO("Ai device id invalid!!!\n");
                break;
            }

			if ((eAiSoundMode != E_AI_SOUND_MODE_MONO)
				&& (eAiSoundMode != E_AI_SOUND_MODE_STEREO)
				&& (eAiSoundMode != E_AI_SOUND_MODE_QUEUE))
			{
				PRINTF_INFO("Ai sound mode invalid!!!\n");
				break;
			}

            if (u32AiChnCnt <= 0)
            {
                PRINTF_INFO("Ai channel count invalid!!!\n");
                break;
            }

            if (AI_DEV_DMIC == AiDevId)
            {
            	if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
            		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
            	{
                	if ((u32AiChnCnt > AI_DMIC_CHN_MAX)
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
						|| (3 == u32AiChnCnt)
#endif
                	)
                	{
                    	PRINTF_INFO("Ai channel count invalid!!!\n");
                    	break;
                	}
                }
                else
                {
					if (u32AiChnCnt > AI_DMIC_CHN_MAX / 2)
                	{
                    	PRINTF_INFO("Ai channel count invalid!!!\n");
                    	break;
                	}
                }

            }
            else if ((AI_DEV_AMIC == AiDevId)
            			|| (AI_DEV_LINE_IN == AiDevId)

            		)
            {
            	if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
            		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
            	{
	                if (u32AiChnCnt > AI_AMIC_CHN_MAX)
	                {
	                    PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
	                }
                }
                else
                {
					if (u32AiChnCnt > AI_AMIC_CHN_MAX / 2)
	                {
	                    PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
	                }
                }
            }
            else if (AI_DEV_I2S_RX == AiDevId)
            {
				if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
				{
					if ((AI_I2S_NOR_CHN != u32AiChnCnt)
						)
					{
						PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
					}
				}
				else
				{
					if ((AI_I2S_NOR_CHN / 2 != u32AiChnCnt)
						)
					{
						PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
					}
				}
            }

#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
            else if (AI_DEV_AMIC_AND_I2S_RX == AiDevId)
            {
                if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
			    {
                    if (AI_AMIC_AND_I2S_RX_CHN_MAX != u32AiChnCnt)
                    {
                        PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
                    }
			    }
			    else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
			    {
                    if (AI_AMIC_AND_I2S_RX_CHN_MAX / 2 != u32AiChnCnt)
                    {
                        PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
                    }
			    }
            }
            else if (AI_DEV_DMIC_AND_I2S_RX == AiDevId)
            {
                if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
			    {
                    if (AI_DMIC_AND_I2S_RX_CHN_MAX != u32AiChnCnt)
                    {
                        PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
                    }
			    }
			    else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
			    {
                    if (AI_DMIC_AND_I2S_RX_CHN_MAX / 2 != u32AiChnCnt)
                    {
                        PRINTF_INFO("Ai channel count invalid!!!\n");
	                    break;
					}
				}
            }
#endif
            if (
                    (E_MI_AUDIO_SAMPLE_RATE_8000 != eAiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != eAiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_32000 != eAiSampleRate)
                &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != eAiSampleRate)
            )
            {
                PRINTF_INFO("AI sample rate invalid!!!\n");
                break;
            }

            if (bAiEnableAec)
            {
                if (
                	((E_MI_AUDIO_SAMPLE_RATE_8000 != eAiSampleRate) && (E_MI_AUDIO_SAMPLE_RATE_16000 != eAiSampleRate))
                    &&
                    ((E_MI_AUDIO_SAMPLE_RATE_INVALID == u32VqeWorkingSampleRate)
                		|| ((E_MI_AUDIO_SAMPLE_RATE_8000 != u32VqeWorkingSampleRate)
                			&& (E_MI_AUDIO_SAMPLE_RATE_16000 != u32VqeWorkingSampleRate)))
                	)
                {
                    PRINTF_INFO("Aec only support 8K/16K!!!\n");
                    break;
                }

                if (AI_DEV_I2S_RX == AiDevId)
                {
                    PRINTF_INFO("I2S RX not support AEC!!!\n");
                    break;
                }
            }

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
			if (TRUE == bAiEnableHwAec)
			{
				if ((AI_DEV_AMIC != AiDevId) && (AI_DEV_LINE_IN != AiDevId))
				{
					PRINTF_INFO("Hw Aec only support Amic/Linein!!!\n");
					break;
				}
			}

			if (TRUE == bAiEnableHwAec)
			{
				if (((E_AI_SOUND_MODE_MONO != eAiSoundMode) || (2 != u32AiChnCnt)))
				{
					PRINTF_INFO("Hw Aec only support Mono mode 2 Channel!!!\n");
					break;
				}
			}
#endif

            if (bAiEnableVqe)
            {
                if (
                	((E_MI_AUDIO_SAMPLE_RATE_8000 != eAiSampleRate)
                		&& (E_MI_AUDIO_SAMPLE_RATE_16000 != eAiSampleRate)
                    	&& (E_MI_AUDIO_SAMPLE_RATE_48000 != eAiSampleRate))
                    &&
                    ((E_MI_AUDIO_SAMPLE_RATE_INVALID == u32VqeWorkingSampleRate)
                		|| ((E_MI_AUDIO_SAMPLE_RATE_8000 != u32VqeWorkingSampleRate)
                			&& (E_MI_AUDIO_SAMPLE_RATE_16000 != u32VqeWorkingSampleRate)
                			&& (E_MI_AUDIO_SAMPLE_RATE_48000 != u32VqeWorkingSampleRate))))
                {
                    PRINTF_INFO("Vqe only support 8K/16/48K!!!\n");
                    break;
                }

                if (bAiEnableHpf)
                {
                    if (((E_MI_AUDIO_SAMPLE_RATE_48000 == eAiSampleRate)
                    	&& (E_MI_AUDIO_SAMPLE_RATE_INVALID == u32VqeWorkingSampleRate))
                    		|| (E_MI_AUDIO_SAMPLE_RATE_48000 == u32VqeWorkingSampleRate))
                    {

                        PRINTF_INFO("Hpf not support 48K!!!\n");
                        break;
                    }
                }
            }

            if (TRUE == bAiEnableResample)
            {
                if (
                        (E_MI_AUDIO_SAMPLE_RATE_8000 != eAiOutputResampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != eAiOutputResampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_32000 != eAiOutputResampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != eAiOutputResampleRate)
                )
                {
                    PRINTF_INFO("Ai resample rate invalid!!!\n");
                    break;
                }

                if (E_MI_AUDIO_SAMPLE_RATE_INVALID == u32VqeWorkingSampleRate)
                {
                    if (eAiOutputResampleRate == eAiSampleRate)
                    {
                        PRINTF_INFO("eAiSampleRate:%d eAiOutputResampleRate:%d. It does not need to resample.\n",
                                eAiSampleRate, eAiOutputResampleRate);
                        break;
                    }
                }
                else
                {
                    if (eAiOutputResampleRate == u32VqeWorkingSampleRate)
                    {
                        PRINTF_INFO("u32VqeWorkingSampleRate:%d eAiOutputResampleRate:%d. It does not need to resample.\n",
                                u32VqeWorkingSampleRate, eAiOutputResampleRate);
                        break;
                    }
                }
            }

            if (TRUE == bAiEnableAenc)
            {
                if (
                        (E_MI_AUDIO_AENC_TYPE_G711A != eAiAencType)
                    &&  (E_MI_AUDIO_AENC_TYPE_G711U != eAiAencType)
                    &&  (E_MI_AUDIO_AENC_TYPE_G726 != eAiAencType)
                )
                {
                    PRINTF_INFO("Ai aenc type invalid!!!\n");
                    break;
                }

                if (E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType)
                {
                    if (
                            (E_MI_AUDIO_G726_MODE_16 != eAiAencG726Mode)
                        &&  (E_MI_AUDIO_G726_MODE_24 != eAiAencG726Mode)
                        &&  (E_MI_AUDIO_G726_MODE_32 != eAiAencG726Mode)
                        &&  (E_MI_AUDIO_G726_MODE_40 != eAiAencG726Mode)
                    )
                    {
                        PRINTF_INFO("Ai G726 mode invalid!!!\n");
                        break;
                    }
                }

                if (E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType)
                {
                    if (E_AI_SOUND_MODE_STEREO == eAiSoundMode)
                    {
                        PRINTF_INFO("Wav not support stereo g726!!!\n");
                        break;
                    }
                }
            }

            if (bAiEnableSsl || bAiEnableBf)
            {
				if (0 == u32MicDistance)
				{
					PRINTF_INFO("Ai mic distance invalid!!!\n");
					break;
				}

				if (E_AI_SOUND_MODE_STEREO != eAiSoundMode)
				{
					PRINTF_INFO("Ssl/Bf only support stereo mode!!!\n");
					break;
				}

				if (bAiEnableBf && bAiSetBfDoa)
				{
                    if (0 > s32AiBfDoa || s32AiBfDoa > 180)  //bf_change:0 < s32AiBfDoa
                    {
                        PRINTF_INFO("s32AiBfDoa only supports 0~180 currently.\n");
                        break;
                    }
				}
            }

            if (TRUE == bAiSetVolume)
            {
                if (AI_DEV_AMIC == AiDevId
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
					|| AI_DEV_AMIC_AND_I2S_RX == AiDevId
#endif
				)
                {
                    if ((s32AiVolume < AI_VOLUME_AMIC_MIN) || (s32AiVolume > AI_VOLUME_AMIC_MAX))
                    {
                        PRINTF_INFO("Ai volume invalid!!!\n");
                        break;
                    }
                }
                else if (AI_DEV_DMIC == AiDevId
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
					|| AI_DEV_DMIC_AND_I2S_RX == AiDevId
#endif
				)
                {
                    if ((s32AiVolume < AI_VOLUME_DMIC_MIN) || (s32AiVolume > AI_VOLUME_DMIC_MAX))
                    {
                        PRINTF_INFO("Ai volume invalid!!!\n");
                        break;
                    }
                }

                else if (AI_DEV_LINE_IN == AiDevId)
                {
                    if ((s32AiVolume < AI_VOLUME_AMIC_MIN) || (s32AiVolume > AI_VOLUME_LINEIN_MAX))
                    {
                        PRINTF_INFO("Ai volume invalid!!!\n");
                        break;
                    }
                }

                else if (AI_DEV_I2S_RX == AiDevId)
                {
                    PRINTF_INFO("I2S RX is not supported volume setting!!!\n");
                    break;
                }
            }
        }

        if (bEnableAO)
        {
            if (NULL == pu8AoInputPath)
            {
                PRINTF_INFO("AO input path invalid!!!\n");
                break;
            }

            if ((AoDevId < 0) || (AoDevId > AO_DEV_ID_MAX))
            {
                PRINTF_INFO("Ao device id invalid!!!\n");
                break;
            }

            if (TRUE == bAoEnableResample)
            {
                if (
                        (E_MI_AUDIO_SAMPLE_RATE_8000 != eAoOutSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_16000 != eAoOutSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_32000 != eAoOutSampleRate)
                    &&  (E_MI_AUDIO_SAMPLE_RATE_48000 != eAoOutSampleRate)
                )
                {
                    PRINTF_INFO("AO resample rate invalid!!!\n");
                    break;
                }
            }

            if (TRUE == bAoSetVolume)
            {
                if ((s32AoVolume < AO_VOLUME_MIN) || (s32AoVolume > AO_VOLUME_MAX))
                {
                    PRINTF_INFO("AO Volume invalid!!!\n");
                    break;
                }
            }

            if ( NULL == strstr((const char*)pu8AoInputPath, ".wav") )
            {
                PRINTF_INFO("Only support wav file.\n");
                break;
            }
        }
        bCheckPass = TRUE;
    }while(0);
    return bCheckPass;
}

void initParam(void)
{
    eAiWavSampleRate = eAiSampleRate;

    if (E_MI_AUDIO_AENC_TYPE_G711A == eAiAencType)
    {
        eAiWavAencType = E_AENC_TYPE_G711A;
    }
    else if (E_MI_AUDIO_AENC_TYPE_G711U == eAiAencType)
    {
        eAiWavAencType = E_AENC_TYPE_G711U;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_16 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_16;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_24 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_24;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_32 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_32;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_40 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_40;
    }
    else
    {
        eAiWavAencType = PCM;
    }

	if (AI_DEV_I2S_RX == AiDevId)
	{
		if (
			(((E_AI_SOUND_MODE_MONO == eAiSoundMode) || (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)) && (AI_I2S_NOR_CHN == u32AiChnCnt))
			|| ((E_AI_SOUND_MODE_STEREO == eAiSoundMode) && (AI_I2S_NOR_CHN / 2 == u32AiChnCnt)))
		{
			bAiI2sNormalMode = TRUE;
		}
		else
		{
			bAiI2sNormalMode = FALSE;
		}
	}

	if (bAiEnableSsl || bAiEnableBf)
	{
		stSslInit.u32MicDistance = u32MicDistance;
		stBfInit.u32MicDistance = u32MicDistance;
	}

    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);

    setTimer(u32RunTime);
    return;
}

void printParam(void)
{
    if (bEnableAI)
    {
        printf("Ai Param:\n");
        // output path
        printf("AI OutPut Path:%s\n", pu8AiOutputPath);

        // device
        printf("Device:");
        if (AiDevId == AI_DEV_AMIC)
        {
            printf("Amic");
        }
        else if (AiDevId == AI_DEV_DMIC)
        {
            printf("Dmic");
        }
        else if (AiDevId == AI_DEV_I2S_RX)
        {
            printf("I2S_RX");
        }
        else if (AiDevId == AI_DEV_LINE_IN)
        {
            printf("Linein");
        }
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E)\
    || defined(CONFIG_SIGMASTAR_CHIP_I6B0))
        else if (AiDevId == AI_DEV_I2S_RX_AND_SRC)
        {
            printf("I2S_RX + SRC");
        }
#endif

#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
        else if (AiDevId == AI_DEV_AMIC_AND_I2S_RX)
        {
            printf("Amic+I2S RX");
        }

        else if (AiDevId == AI_DEV_DMIC_AND_I2S_RX)
        {
            printf("Dmic+I2S RX");
        }
#endif
        printf("\n");

        // chn cnt
        printf("ChnNum:%d\n", u32AiChnCnt);

        // mode
        printf("Mode:");
        if (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
        {
            printf("Queue");
        }
        else if (E_AI_SOUND_MODE_MONO == eAiSoundMode)
        {
            printf("Mono");
        }
        else if (E_AI_SOUND_MODE_STEREO == eAiSoundMode)
        {
			printf("Stereo");
        }
        printf("\n");

        // aed
        printf("Aed:");
        if (bAiEnableAed)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

		// vqe working sample rate
		printf("Vqe working sample rate:");
		if (E_MI_AUDIO_SAMPLE_RATE_INVALID == u32VqeWorkingSampleRate)
		{
			printf("not specified");
		}
		else
		{
			printf("%dK", u32VqeWorkingSampleRate / 1000);
		}
		printf("\n");

        // aec
        printf("Aec:");
        if (bAiEnableAec)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        if (bAiEnableAec)
        {
            printf("Aec Type:");
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
            if (bAiEnableHwAec)
            {
                printf("HW");
            }
            else
#endif
            {
                printf("SW");
            }
            printf("\n");
        }

        // hpf
        printf("Hpf:");
        if (bAiEnableHpf)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // nr
        printf("Nr:");
        if (bAiEnableNr)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // agc
        printf("Agc:");
        if (bAiEnableAgc)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // eq
        printf("Eq:");
        if (bAiEnableEq)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // ssl
        printf("Ssl:");
        if (bAiEnableSsl)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

		// bf
		printf("Bf:");
        if (bAiEnableBf)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }

        if (TRUE == bAiSetBfDoa)
        {
            printf("   Bf Doa:%d.\n", s32AiBfDoa);
        }
        printf("\n");

        // resample
        printf("Resample:");
        if (bAiEnableResample)
        {
            printf("Enable %dK", eAiOutputResampleRate / 1000);
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // Aenc
        printf("Aenc:");
        if (bAiEnableAenc)
        {
            printf("Enable ");
            if (E_AENC_TYPE_G711A == eAiWavAencType)
            {
                printf("g711a");
            }
            else if (E_AENC_TYPE_G711U == eAiWavAencType)
            {
                printf("g711u");
            }
            else if (E_AENC_TYPE_G726_16 == eAiWavAencType)
            {
                printf("g726_16");
            }
            else if (E_AENC_TYPE_G726_24 == eAiWavAencType)
            {
                printf("g726_24");
            }
            else if (E_AENC_TYPE_G726_32 == eAiWavAencType)
            {
                printf("g726_32");
            }
            else if (E_AENC_TYPE_G726_40 == eAiWavAencType)
            {
                printf("g726_40");
            }
        }
        else
        {
            printf("Disable");
        }
        printf("\n");
    }

    if (bEnableAO)
    {
        printf("Ao Param:\n");

        // input path
        printf("AO InPut Path:%s\n", pu8AoInputPath);

        // device
        printf("Device:");
        if (AoDevId == AO_DEV_LINE_OUT)
        {
            printf("LineOut");
        }
        else if (AoDevId == AO_DEV_I2S_TX)
        {
            printf("I2S_TX");
        }
        printf("\n");

        // hpf
        printf("Hpf:");
        if (bAoEnableHpf)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // nr
        printf("Nr:");
        if (bAoEnableNr)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // agc
        printf("Agc:");
        if (bAoEnableAgc)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // eq
        printf("Eq:");
        if (bAoEnableEq)
        {
            printf("Enable");
        }
        else
        {
            printf("Disable");
        }
        printf("\n");

        // resample
        printf("Resample:");
        if (bAoEnableResample)
        {
            printf("Enable %dK", eAoOutSampleRate / 1000);
        }
        else
        {
            printf("Disable");
        }
        printf("\n");
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

    if(eAencType == E_AENC_TYPE_G711A)
    {
        tWavHead->wave.wFormatTag = 0x06;
    }

    if(eAencType == E_AENC_TYPE_G711U)
    {
        tWavHead->wave.wFormatTag = 0x07;
    }

    if(eAencType == E_AENC_TYPE_G711U || eAencType == E_AENC_TYPE_G711A)
    {
        if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wBitsPerSample = 8;
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.wChannels) / 8;
    }
    else if(eAencType == PCM)
    {
        if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x1;
        tWavHead->wave.wBitsPerSample = 16;
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = 1024;
    }
    else //g726
    {
		if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x45;
        switch(eAencType)
        {
            case E_AENC_TYPE_G726_40:
                tWavHead->wave.wBitsPerSample = 5;
                tWavHead->wave.wBlockAlign =  5;
                break;
            case E_AENC_TYPE_G726_32:
                tWavHead->wave.wBitsPerSample = 4;
                tWavHead->wave.wBlockAlign =  4;
                break;
            case E_AENC_TYPE_G726_24:
                tWavHead->wave.wBitsPerSample = 3;
                tWavHead->wave.wBlockAlign =  3;
                break;
            case E_AENC_TYPE_G726_16:
                tWavHead->wave.wBitsPerSample = 2;
                tWavHead->wave.wBlockAlign =  2;
                break;
            default:
                PRINTF_INFO("eAencType error:%d\n", eAencType);
                return -1;
        }

        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
    }

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(WaveFileHeader_t) - 8;

    return 0;
}

MI_S32 createOutputFileName(AiOutFileName_t *pstAiOutFileName, MI_U32 u32ChnIdx)
{
    char as8Tmp[512] = {0};
    char asAencG726Mode[4][15] = {"_G726_16", "_G726_24", "_G726_32", "_G726_40"};

    //set OutpuFile prefix
    memset(as8Tmp, 0, sizeof(as8Tmp));
    if ('/' == pstAiOutFileName->ps8OutputPath[strlen((char *)pstAiOutFileName->ps8OutputPath) - 1])
    {
        sprintf(as8Tmp, "%s", pstAiOutFileName->ps8OutputPath);
    }
    else
    {
        sprintf(as8Tmp, "%s/", pstAiOutFileName->ps8OutputPath);
    }
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set Chn num
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "Chn%d_", u32ChnIdx);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile Device ID

    memset(as8Tmp, 0, sizeof(as8Tmp));
    if (AI_DEV_AMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic_");
    }
    else if (AI_DEV_DMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Dmic_");
    }
    else if (AI_DEV_I2S_RX == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "I2SRx_" );
    }
    else if (AI_DEV_LINE_IN == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "LineIn_" );
    }

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E)\
    || defined(CONFIG_SIGMASTAR_CHIP_I6B0))
    else if (AI_DEV_I2S_RX_AND_SRC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "I2SRx+Src_" );
    }
#endif
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile SampleRate
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "%dK", pstAiOutFileName->stAiAttr.eSamplerate / 1000);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile BitWidth
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "_%dbit", 16);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile Channel Mode
    memset(as8Tmp, 0, sizeof(as8Tmp));
    if (E_MI_AUDIO_SOUND_MODE_MONO == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_MONO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_STEREO == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_STEREO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_QUEUE");
    }

    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    strcpy((char *)pstAiOutFileName->ps8SrcPcmOutputFile, (char *)pstAiOutFileName->ps8OutputFile);

	// set vqe working sample rate
	if (pstAiOutFileName->bSetVqeWorkingSampleRate)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_%dKVqeWork", pstAiOutFileName->eVqeWorkingSampleRate / 1000);
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    //set  OutpuFile ResampleRate
    if (pstAiOutFileName->bEnableRes)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_%dKRES", pstAiOutFileName->eOutSampleRate / 1000);
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    //************set OutpuFile Veq Mod START******************
	if (TRUE == pstAiOutFileName->stSetVqeConfig.bAecOpen)
	{
		memset(as8Tmp, 0, sizeof(as8Tmp));
		sprintf(as8Tmp, "_Aec");
		strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
	}

    if (TRUE == pstAiOutFileName->bEnableBf)
    {
		memset(as8Tmp, 0, sizeof(as8Tmp));
		sprintf(as8Tmp, "_Bf");
		strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bHpfOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Hpf");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bAnrOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Anr");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bAgcOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Agc");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if(TRUE == pstAiOutFileName->stSetVqeConfig.bEqOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Eq");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }
    //************set OutpuFile Veq Mod END******************

    // set OutpufFile Aenc
    if (TRUE == pstAiOutFileName->bEnableAenc)
    {
        if (E_MI_AUDIO_AENC_TYPE_G711A == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, "_G711a");
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
        else if (E_MI_AUDIO_AENC_TYPE_G711U == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, "_G711u");
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
        else if (E_MI_AUDIO_AENC_TYPE_G726 == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, asAencG726Mode[pstAiOutFileName->stSetAencConfig.stAencG726Cfg.eG726Mode]);
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
    }

    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, ".wav");
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    strcat((char *)pstAiOutFileName->ps8SrcPcmOutputFile, as8Tmp);
    return 0;
}

void* aiGetChnPortBuf(void* data)
{
    AiChnPriv_t* priv = (AiChnPriv_t*)data;
    MI_AUDIO_Frame_t stAiChFrame;
    MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32Ret;
    MI_U32 u32ChnIndex;
    struct timeval tv_before, tv_after;
    MI_S64 before_us, after_us;
	MI_AI_AedResult_t stAedResult;
    WaveFileHeader_t stWavHead;
    MI_S32 s32Doa = 0;

    MI_S32 OutputBuffSize = 0;
    MI_U8  *pu8OutputBuff = NULL;

    if(_gastAiDevInfoApp[priv->AiDevId].astChanInfo[priv->AiChn].pu8OutputBuff != NULL)
    {
        pu8OutputBuff = _gastAiDevInfoApp[priv->AiDevId].astChanInfo[priv->AiChn].pu8OutputBuff;
    }
    else
    {
        PRINTF_INFO("pu8OutputBuff is NULL !\n");
    }

    AUDIO_CHECK_POINTER(pu8OutputBuff);

    memset(&stAiChFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode) || (E_AI_SOUND_MODE_STEREO == eAiSoundMode))
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if (MI_SUCCESS == s32Ret)
            {
                fwrite(stAiChFrame.apVirAddr[0], 1, stAiChFrame.u32Len[0], fpAiDumpMiFile[0]);//dump MI Ai files
                fwrite(stAecFrame.stRefFrame.apVirAddr[0], 1, stAecFrame.stRefFrame.u32Len[0], fpAiDumpMiFile[1]);//dump MI src files
#if (defined(AI_INPUT_FILE_FOR_TEST_ALG))   //AI input file for test alg
                if(NULL != pu8AiTestAlgPath)
                {
                    memset(stAiChFrame.apVirAddr[0], 0x0, stAiChFrame.u32Len[0]);
                    s32Ret = fread(stAiChFrame.apVirAddr[0], 1, stAiChFrame.u32Len[0], fpAiTestAlg);
                    if(s32Ret != stAiChFrame.u32Len[0])
                    {
                        //PRINTF_DBG("files end,continue......!!!\n");

                        return -1;//test alg !

                        fseek(fpAiTestAlg, sizeof(WaveFileHeader_t), SEEK_SET);
                        s32Ret = fread(stAiChFrame.apVirAddr[0], 1, stAiChFrame.u32Len[0], fpAiTestAlg);
                        if (s32Ret < 0)
                        {
                            PRINTF_INFO("Input file does not has enough data!!!\n");
                            return -1;
                        }
                    }
                }
				if((NULL != pu8AiTestAlgForAecPath) && (NULL != pu8AiTestAlgPath))
				{
					memset(stAecFrame.stRefFrame.apVirAddr[0], 0x0, stAecFrame.stRefFrame.u32Len[0]);
					s32Ret = fread(stAecFrame.stRefFrame.apVirAddr[0], 1, stAecFrame.stRefFrame.u32Len[0], fpAiTestAlgForAec);
					if(s32Ret != stAecFrame.stRefFrame.u32Len[0])
					{
						//PRINTF_DBG("files end,continue......!!!\n");
						return -1;//test alg !

						fseek(fpAiTestAlgForAec, sizeof(WaveFileHeader_t), SEEK_SET);
						s32Ret = fread(stAecFrame.stRefFrame.apVirAddr[0], 1, stAecFrame.stRefFrame.u32Len[0], fpAiTestAlgForAec);
						if (s32Ret < 0)
						{
							PRINTF_INFO("Input Aec file does not has enough data!!!\n");
							return -1;
						}
					}
				}
#endif   //AI input files for test alg

                PRINTF_DBG("priv->AiDevId = %d, priv->AiChn = %d\n", priv->AiDevId, priv->AiChn);
                do
                {
                    s32Ret = AI_DoAlg(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, &OutputBuffSize);
                }while(-2 == s32Ret);

                if (priv->bEnableAed)
                {
                    AI_GetAedResult(priv->AiDevId, priv->AiChn, &stAedResult);
                    if(stAedResult.bLoudSoundDetected)
                    {
                        printf("Dev%d Chn%d, loud sound detected!\n", priv->AiDevId, priv->AiChn);
                    }
                    else if(stAedResult.bAcousticEventDetected)
                    {
                        printf("Dev%d Chn%d, Baby crying!\n", priv->AiDevId, priv->AiChn);
                    }
                    else
                    {
                        printf("Dev%d Chn%d, Nothing happened !\n", priv->AiDevId, priv->AiChn);
                    }
                }
				else if ((E_AI_SOUND_MODE_STEREO == eAiSoundMode) && (bAiEnableSsl) &&
				        ((!bAiEnableBf) || (bAiEnableBf && bAiSetBfDoa)))
                {
					AI_GetSslDoa(priv->AiDevId, priv->AiChn, &s32Doa);
					printf("SSL Doa is %d.\n", s32Doa);
                }
                else
                {
	                gettimeofday(&tv_before, NULL);

	                //write(priv->s32Fd, stAiChFrame.apVirAddr[0], stAiChFrame.u32Len[0]);
#if 1
	                write(priv->s32Fd, pu8OutputBuff, OutputBuffSize);
#else
                    write(priv->s32Fd, stAecFrame.stRefFrame.apVirAddr[0], stAecFrame.stRefFrame.u32Len[0]);
#endif
	                gettimeofday(&tv_after, NULL);
	                before_us = tv_before.tv_sec * 1000000 + tv_before.tv_usec;
	                after_us = tv_after.tv_sec * 1000000 + tv_after.tv_usec;
	                if (after_us - before_us > 10 * 1000)
	                {
	                    PRINTF_INFO("Chn:%d, cost time:%lldus = %lldms.\n", priv->AiChn, after_us - before_us, (after_us - before_us) / 1000);
	                }
	                //priv->u32TotalSize += stAiChFrame.u32Len[0];
                    priv->u32TotalSize += OutputBuffSize;

	                if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
	                {
                        if (TRUE == bAiDumpPcmData)
                        {
	                	    write(priv->s32SrcPcmFd, stAiChFrame.apSrcPcmVirAddr[0], stAiChFrame.u32SrcPcmLen[0]);
	                	    priv->u32SrcPcmTotalSize += stAiChFrame.u32SrcPcmLen[0];
	                    }
                    }
                }
                MI_AI_ReleaseFrame(priv->AiDevId, priv->AiChn,  &stAiChFrame,  NULL);
            }
            else
            {
                PRINTF_INFO("Dev%dChn%d get frame failed!!!error:0x%x\n", priv->AiDevId, priv->AiChn, s32Ret);
            }
        }
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, eAiWavAencType, eAiWavSoundMode, eAiWavSampleRate, priv->u32TotalSize);
        lseek(priv->s32Fd, 0, SEEK_SET);
        write(priv->s32Fd, &stWavHead, sizeof(WaveFileHeader_t));
        close(priv->s32Fd);

        if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
        {
            if (TRUE == bAiDumpPcmData)
            {
			    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
                if (TRUE == bAiEnableBf)
                {
                    eAiWavSoundMode = E_SOUND_MODE_STEREO;
                }
	            addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, eAiSampleRate, priv->u32SrcPcmTotalSize);
	            lseek(priv->s32SrcPcmFd, 0, SEEK_SET);
	            write(priv->s32SrcPcmFd, &stWavHead, sizeof(WaveFileHeader_t));
	            close(priv->s32SrcPcmFd);
            }
        }
    }
    else  //queue mode
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if (s32Ret == MI_SUCCESS)
            {
                for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
                {
                    if (priv->bEnableAed)
                    {
                        AI_GetAedResult(priv->AiDevId, u32ChnIndex, &stAedResult);
                        PRINTF_INFO("Dev%d Chn%d, bAcousticEventDetected[%d] bLoudSoundDetected[%d].\n",
                            priv->AiDevId, u32ChnIndex, stAedResult.bAcousticEventDetected, stAedResult.bLoudSoundDetected);
                    }
					//AI_DoAlg(priv->AiDevId, u32ChnIndex, &stAiChFrame, &stAecFrame, &OutputBuffSize);
					write(AiChnFd[u32ChnIndex], stAiChFrame.apVirAddr[u32ChnIndex], stAiChFrame.u32Len[u32ChnIndex]);

                    if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
	                {
                        if (TRUE == bAiDumpPcmData)
                        {
	                	    write(AiChnSrcPcmFd[u32ChnIndex], stAiChFrame.apSrcPcmVirAddr[u32ChnIndex],
	                		    stAiChFrame.u32SrcPcmLen[u32ChnIndex]);
                        }
                    }
                }
                priv->u32TotalSize += stAiChFrame.u32Len[0];

                if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
                {
                    if (TRUE == bAiDumpPcmData)
                    {
					    priv->u32SrcPcmTotalSize += stAiChFrame.u32SrcPcmLen[0];
                    }
                }
                MI_AI_ReleaseFrame(priv->AiDevId, priv->AiChn, &stAiChFrame,  NULL);
            }
        }
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, eAiWavAencType, eAiWavSoundMode, eAiWavSampleRate, priv->u32TotalSize);
        for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
        {
            lseek(AiChnFd[u32ChnIndex], 0, SEEK_SET);
            write(AiChnFd[u32ChnIndex], &stWavHead, sizeof(WaveFileHeader_t));
            close(AiChnFd[u32ChnIndex]);
        }

        if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
        {
            if (TRUE == bAiDumpPcmData)
            {
			    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
                if (TRUE == bAiEnableBf)
                {
                    eAiWavSoundMode = E_SOUND_MODE_STEREO;
                }
	            addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, eAiSampleRate, priv->u32SrcPcmTotalSize);
	            for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
	            {
	                lseek(AiChnSrcPcmFd[u32ChnIndex], 0, SEEK_SET);
	                write(AiChnSrcPcmFd[u32ChnIndex], &stWavHead, sizeof(WaveFileHeader_t));
	                close(AiChnSrcPcmFd[u32ChnIndex]);
	            }
            }
        }
    }  //queue mode

    return NULL;
}

void* aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;
    MI_S32 Time0, Time1;

    while(FALSE == bAoExit)
    {
        memset(u8TempBuf, 0x0, MI_AUDIO_SAMPLE_PER_FRAME * 4);
        s32Ret = read(AoReadFd, u8TempBuf, s32NeedSize);
        if(s32Ret != s32NeedSize)
        {
            lseek(AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            PRINTF_INFO("Playback file end ! restart ...\n");
            s32Ret = read(AoReadFd, u8TempBuf, s32NeedSize);
            if (s32Ret < 0)
            {
                PRINTF_INFO("Input file does not has enough data!!!\n");
                break;
            }
        }

        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        PRINTF_DBG("SendFrame size %d in aoSendFrame()\n", s32Ret);
        stAoSendFrame.apVirAddr[0] = u8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        Time0 = (long)_OsCounterGetMs();
        do{
            s32Ret = AO_DoAlg_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);
        }while(s32Ret == E_APP_ERR_NO_BUFFER);
        Time1 = (long)_OsCounterGetMs();
        PRINTF_DBG("\n[01]cost time:%dms !!!\n", Time1 - Time0);

        if(s32Ret != MI_SUCCESS)
        {
            PRINTF_INFO("[Warning !!! !!!]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
        }
    }

    close(AoReadFd);
    return NULL;
}

MI_S32 allocPrivatePool(void)
{
	MI_SYS_GlobalPrivPoolConfig_t stGlobalPrivPoolConf;
	MI_U32 u32HeapSize = 0;
	MI_U32 u32ChnCnt = 0;
	MI_U32 u32PtNum = 0;
	MI_U32 u32BufSize;
	MI_U32 u32BitWidth = 2;
	MI_U32 u32AlignSize = 4 * 1024;
	MI_U32 u32ChnIdx;

	if (bEnableAI)
	{
		u32ChnCnt = u32AiChnCnt;
		if ((unsigned char)E_MI_AUDIO_SOUND_MODE_STEREO == (unsigned char)eAiSoundMode)
		{
			u32ChnCnt *= 2;
		}

		u32HeapSize = u32BitWidth * u32ChnCnt * 2 * eAiSampleRate;
		u32HeapSize += (MIU_WORD_BYTE_SIZE - (u32HeapSize % MIU_WORD_BYTE_SIZE));

        if (u32HeapSize >= AI_DMA_BUFFER_MAX_SIZE)
        {
			u32HeapSize = AI_DMA_BUFFER_MAX_SIZE;
        }
        else if (u32HeapSize >= AI_DMA_BUFFER_MID_SIZE)
        {
			u32HeapSize = AI_DMA_BUFFER_MID_SIZE;
        }
		else
        {
			u32HeapSize = AI_DMA_BUFFER_MIN_SIZE;
        }

		u32AiDevHeapSize = u32HeapSize;

		memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
		stGlobalPrivPoolConf.bCreate = TRUE;
		stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_DEV_PRIVATE_POOL;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.eModule = E_MI_MODULE_ID_AI;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32Devid = AiDevId;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32PrivateHeapSize = u32HeapSize;
		ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);

		bAllocAiDevPool = TRUE;

		u32PtNum = eAiSampleRate / 16;
		u32BufSize = u32PtNum * u32BitWidth;	// for one date channel

		if ((unsigned char)E_MI_AUDIO_SOUND_MODE_STEREO == (unsigned char)eAiSoundMode)
		{
			u32BufSize *= 2;
		}

		if (AI_DEV_I2S_RX != AiDevId)
		{
			u32BufSize *= 2;
		}

		// 4K alignment
		if (0 != (u32BufSize % u32AlignSize))
		{
			u32BufSize = ((u32BufSize / u32AlignSize) + 1) * u32AlignSize;
		}

		u32HeapSize = u32BufSize * TOTAL_BUF_DEPTH;
		u32AiChnOutputHeapSize = u32HeapSize;

		for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
		{
			memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
			stGlobalPrivPoolConf.bCreate = TRUE;
			stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_CHN_PORT_OUTPUT_POOL;
			stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.eModule = E_MI_MODULE_ID_AI;
			stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Channel = u32ChnIdx;
			stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Devid = AiDevId;
			stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Port = 0;
			stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32PrivateHeapSize = u32HeapSize;
			ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);
		}
		bAllocAiChnOutputPool = TRUE;
	}

	if (bEnableAO)
	{
		// I can not get sampling rate of ao device, so use max sample rate to alloc heap
		u32HeapSize = u32BitWidth * E_MI_AUDIO_SAMPLE_RATE_48000 / 2;
		u32HeapSize += (MIU_WORD_BYTE_SIZE - (u32HeapSize % MIU_WORD_BYTE_SIZE));
		if (u32HeapSize >= AO_DMA_BUFFER_MAX_SIZE)
		{
			u32HeapSize = AO_DMA_BUFFER_MAX_SIZE;
		}
		else if (u32HeapSize >= AO_DMA_BUFFER_MID_SIZE)
		{
			u32HeapSize = AO_DMA_BUFFER_MID_SIZE;
		}
		else
		{
			u32HeapSize = AO_DMA_BUFFER_MIN_SIZE;
		}

		// 2 buf, hw buffer + copy buffer
		u32HeapSize *= 2;
		u32AoDevHeapSize = u32HeapSize;
		memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
		stGlobalPrivPoolConf.bCreate = TRUE;
		stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_DEV_PRIVATE_POOL;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.eModule = E_MI_MODULE_ID_AO;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32Devid = AoDevId;
		stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32PrivateHeapSize = u32HeapSize;
		ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);
		bAllocAoDevPool = TRUE;
	}
	return MI_SUCCESS;
}

MI_S32 freePrivatePool(void)
{
	MI_SYS_GlobalPrivPoolConfig_t stGlobalPrivPoolConf;
	MI_U32 u32ChnIdx;

	if (bEnableAI)
	{
		if (TRUE == bAllocAiDevPool)
		{
			memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
			stGlobalPrivPoolConf.bCreate = FALSE;
			stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_DEV_PRIVATE_POOL;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.eModule = E_MI_MODULE_ID_AI;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32Devid = AiDevId;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32PrivateHeapSize = u32AiDevHeapSize;
			ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);

			bAllocAiDevPool = FALSE;
		}

		if (TRUE == bAllocAiChnOutputPool)
		{
			for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
			{
				memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
				stGlobalPrivPoolConf.bCreate = FALSE;
				stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_CHN_PORT_OUTPUT_POOL;
				stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.eModule = E_MI_MODULE_ID_AI;
				stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Channel = u32ChnIdx;
				stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Devid = AiDevId;
				stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32Port = 0;
				stGlobalPrivPoolConf.uConfig.stPreChnPortOutputPrivPool.u32PrivateHeapSize = u32AiChnOutputHeapSize;
				ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);
			}
			bAllocAiChnOutputPool = FALSE;
		}
	}

	if (bEnableAO)
	{
		if (TRUE == bAllocAoDevPool)
		{
			memset(&stGlobalPrivPoolConf, 0x0, sizeof(stGlobalPrivPoolConf));
			stGlobalPrivPoolConf.bCreate = FALSE;
			stGlobalPrivPoolConf.eConfigType = E_MI_SYS_PER_DEV_PRIVATE_POOL;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.eModule = E_MI_MODULE_ID_AO;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32Devid = AoDevId;
			stGlobalPrivPoolConf.uConfig.stPreDevPrivPoolConfig.u32PrivateHeapSize = u32AoDevHeapSize;
			ExecFunc(MI_SYS_ConfigPrivateMMAPool(&stGlobalPrivPoolConf), MI_SUCCESS);
			bAllocAoDevPool = FALSE;
		}
	}
	return MI_SUCCESS;
}

void freePrivatePoolExit(void)
{
	MI_S32 s32Ret = MI_SUCCESS;
	s32Ret = freePrivatePool();
	if (MI_SUCCESS != s32Ret)
	{
		PRINTF_INFO("Failed to free private pool!!!\n");
	}
	return;
}

MI_S32 initAi(void)
{
    MI_AUDIO_Attr_t     stAiSetAttr;
    MI_AUDIO_Attr_t     stAiGetAttr;
    AiOutFileName_t     stAiFileName;
    MI_AI_AencConfig_t  stAiSetAencConfig, stAiGetAencConfig;
    MI_AI_VqeConfig_t   stAiSetVqeConfig, stAiGetVqeConfig;
    MI_AI_AedConfig_t   stAiSetAedConfig, stAiGetAedConfig;
    MI_U32              u32ChnIdx = 0;
    MI_U32              u32ChnCnt = 0;
    MI_S8               s8OutputFileName[512];
    MI_S8               s8SrcPcmOutputFileName[512];
    MI_S32              s32Ret;
    WaveFileHeader_t    stAiWavHead;
    MI_SYS_ChnPort_t    stAiChnOutputPort0[MI_AUDIO_MAX_CHN_NUM];
#if 0
    MI_AI_ChnParam_t    stAiChnParam;
#endif
    MI_AI_SslInitAttr_t		stAiGetSslInitAttr;
    MI_AI_SslConfigAttr_t	stAiGetSslConfigAttr;
    MI_AI_BfInitAttr_t		stAiGetBfInitAttr;
    MI_AI_BfConfigAttr_t	stAiGetBfConfigAttr;

    memset(&stAiSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    memset(&stAiGetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = eAiSampleRate;
    stAiSetAttr.eSoundmode = eAiSoundMode;
    stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAiSetAttr.u32ChnCnt = u32AiChnCnt;
    stAiSetAttr.u32CodecChnCnt = 0; // useless
    stAiSetAttr.u32FrmNum = 6;  // useless
    stAiSetAttr.u32PtNumPerFrm = stAiSetAttr.eSamplerate / 2;//16->2
    stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
    stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;

    memcpy(&_gastAiDevInfoApp[AiDevId].stDevAttr, &stAiSetAttr, sizeof(MI_AUDIO_Attr_t)); //add

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
	{
		eAiWavSoundMode = E_SOUND_MODE_MONO;
	}
	else
	{
		eAiWavSoundMode = E_SOUND_MODE_STEREO;
	}

	if (bAiEnableBf)
	{
		eAiWavSoundMode = E_SOUND_MODE_MONO;
	}

    memset(&stAiSetVqeConfig, 0x0, sizeof(MI_AI_VqeConfig_t));
    if (bAiEnableVqe)
    {
		if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
			|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
		{
			stAiSetVqeConfig.u32ChnNum = 1;
		}
		else
		{
			stAiSetVqeConfig.u32ChnNum = 2;
		}

		if (bAiEnableBf)
		{
			stAiSetVqeConfig.u32ChnNum = 1;
		}

        stAiSetVqeConfig.bAecOpen = bAiEnableAec;
        stAiSetVqeConfig.bAgcOpen = bAiEnableAgc;
        stAiSetVqeConfig.bAnrOpen = bAiEnableNr;
        stAiSetVqeConfig.bEqOpen = bAiEnableEq;
        stAiSetVqeConfig.bHpfOpen = bAiEnableHpf;
        stAiSetVqeConfig.s32FrameSample = 128;
        if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
        {
        	stAiSetVqeConfig.s32WorkSampleRate = u32VqeWorkingSampleRate;
        }
        else
        {
			stAiSetVqeConfig.s32WorkSampleRate = eAiSampleRate;
        }

        // AEC
        memcpy(&stAiSetVqeConfig.stAecCfg, &stAecCfg, sizeof(MI_AI_AecConfig_t));

        // AGC
        memcpy(&stAiSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));

        // ANR
        memcpy(&stAiSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));

        // EQ
        memcpy(&stAiSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));

        // HPF
        memcpy(&stAiSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));
    }

    memset(&stAiSetAencConfig, 0x0, sizeof(MI_AI_AencConfig_t));
    if (bAiEnableAenc)
    {
        stAiSetAencConfig.eAencType = eAiAencType;
        if (E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType)
        {
            stAiSetAencConfig.stAencG726Cfg.eG726Mode = eAiAencG726Mode;
        }
    }

    memset(&stAiFileName, 0x0, sizeof(AiOutFileName_t));
    stAiFileName.AiDevId = AiDevId;
    stAiFileName.bEnableAenc = bAiEnableAenc;
    stAiFileName.bEnableRes = bAiEnableResample;
    stAiFileName.eOutSampleRate = eAiOutputResampleRate;
    stAiFileName.ps8OutputFile = (MI_S8*)s8OutputFileName;
    stAiFileName.ps8SrcPcmOutputFile = (MI_S8*)s8SrcPcmOutputFileName;
    stAiFileName.ps8OutputPath = (MI_S8*)pu8AiOutputPath;
    memcpy(&stAiFileName.stAiAttr, &stAiSetAttr, sizeof(MI_AUDIO_Attr_t));
    memcpy(&stAiFileName.stSetVqeConfig, &stAiSetVqeConfig, sizeof(MI_AI_VqeConfig_t));
    memcpy(&stAiFileName.stSetAencConfig, &stAiSetAencConfig, sizeof(MI_AI_AencConfig_t));

	if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
    {
		stAiFileName.bSetVqeWorkingSampleRate = TRUE;
		stAiFileName.eVqeWorkingSampleRate = u32VqeWorkingSampleRate;
    }

    if (TRUE == bAiEnableBf)
    {
        stAiFileName.bEnableBf = TRUE;
    }

    memset(&stAiWavHead, 0x0, sizeof(WaveFileHeader_t));

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
    if (bAiEnableHwAec)
    {
        u32ChnCnt = 1;
    }
    else
#endif
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        memset(s8OutputFileName, 0x0, sizeof(s8OutputFileName));
        memset(s8SrcPcmOutputFileName, 0x0, sizeof(s8SrcPcmOutputFileName));
        createOutputFileName(&stAiFileName, u32ChnIdx);
        PRINTF_INFO("OutputName:%s\n", s8OutputFileName);
        AiChnFd[u32ChnIdx] = open((const char *)s8OutputFileName, O_RDWR | O_CREAT, 0777);
        if(AiChnFd[u32ChnIdx] < 0)
        {
            PRINTF_INFO("Open output file path:%s fail \n", s8OutputFileName);
            PRINTF_INFO("error:%s", strerror(errno));
            return -1;
        }

        s32Ret = write(AiChnFd[u32ChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
        if (s32Ret < 0)
        {
            PRINTF_INFO("write wav head failed\n");
            return -1;
        }

		if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
		{
            if (TRUE == bAiDumpPcmData)
            {
	            PRINTF_INFO("OutputName:%s\n", s8SrcPcmOutputFileName);
	            AiChnSrcPcmFd[u32ChnIdx] = open((const char *)s8SrcPcmOutputFileName, O_RDWR | O_CREAT, 0777);
	            if(AiChnSrcPcmFd[u32ChnIdx] < 0)
	            {
	                PRINTF_INFO("Open output file path:%s fail \n", s8SrcPcmOutputFileName);
	                PRINTF_INFO("error:%s", strerror(errno));
	                return -1;
	            }

	            s32Ret = write(AiChnSrcPcmFd[u32ChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
	            if (s32Ret < 0)
	            {
	                PRINTF_INFO("write wav head failed\n");
	                return -1;
	            }
            }
        }

		/* add AI init start */
        PRINTF_DBG("AiDevId = %d, u32ChnIdx = %d.\n", AiDevId, u32ChnIdx);

        s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stTempChnQueue, AUDIO_DEMO_TEMP_SIZE);
        if (0 != s32Ret)
        {
            PRINTF_INFO("Init AI stTempChnQueue failed.\n");
            return -1;
        }
        PRINTF_DBG("Init AI stTempChnQueue succeed.\n");
        _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff = malloc(AUDIO_DEMO_TEMP_SIZE);
        _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff = malloc(AUDIO_DEMO_TEMP_SIZE);

        if ((NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff)
                || (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff))
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff);
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff = NULL;
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff);
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff = NULL;
            PRINTF_INFO("Dev%d ch%d malloc pu8InputBuff or pu8OutputBuff failed.\n", AiDevId, u32ChnIdx);
        }
        PRINTF_DBG("Dev%d ch%d malloc pu8InputBuff and OutputBuff succeed !!!\n", AiDevId, u32ChnIdx);

		/* add AI init end */
    }

    ExecFuncNoExit(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AI_GetPubAttr(AiDevId, &stAiGetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AI_Enable(AiDevId), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    if (bAiSetVolume)
    {
        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
			if ((AI_DEV_AMIC_AND_I2S_RX == AiDevId)
                || ((AI_DEV_DMIC_AND_I2S_RX == AiDevId)))
            {
                if ((E_MI_AUDIO_SOUND_MODE_MONO == eAiSoundMode)
                    || (E_MI_AUDIO_SOUND_MODE_QUEUE == eAiSoundMode))
                {
                    if ((2 == u32ChnIdx) || (3 == u32ChnIdx))
                    {
                        continue;
                    }
                }
                else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
                {
                    if (1 == u32ChnIdx)
                    {
                        continue;
                    }
                }
            }
#endif
            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, s32AiVolume), MI_SUCCESS);
        }
    }

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
    if (bAiEnableHwAec)
    {
        ExecFunc(MI_AI_SetVqeVolume(AiDevId, 1, 1), MI_SUCCESS);
    }
#endif
    memset(&stAiChnPriv, 0x0, sizeof(stAiChnPriv));
    memset(&stAiChnOutputPort0, 0x0, sizeof(stAiChnOutputPort0));

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
	 	|| bAiEnableHwAec
#endif
	)
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;//main
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        stAiChnPriv[u32ChnIdx].AiChn = u32ChnIdx;
        stAiChnPriv[u32ChnIdx].AiDevId = AiDevId;
        stAiChnPriv[u32ChnIdx].s32Fd = AiChnFd[u32ChnIdx];
        stAiChnPriv[u32ChnIdx].s32SrcPcmFd = AiChnSrcPcmFd[u32ChnIdx];
        stAiChnPriv[u32ChnIdx].u32ChnCnt = u32AiChnCnt;
        stAiChnPriv[u32ChnIdx].u32TotalSize = 0;
        stAiChnPriv[u32ChnIdx].u32SrcPcmTotalSize = 0;

        stAiChnOutputPort0[u32ChnIdx].eModId = E_MI_MODULE_ID_AI;
        stAiChnOutputPort0[u32ChnIdx].u32DevId = AiDevId;
        stAiChnOutputPort0[u32ChnIdx].u32ChnId = u32ChnIdx;
        stAiChnOutputPort0[u32ChnIdx].u32PortId = 0;
        ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort0[u32ChnIdx], USER_BUF_DEPTH, TOTAL_BUF_DEPTH), MI_SUCCESS);//(2,8)->(8,16)
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
        if (bAiEnableHwAec)
        {
            ExecFunc(MI_AI_SetExtAecChn(AiDevId, u32ChnIdx, 1), MI_SUCCESS);
        }
#endif
        ExecFuncNoExit(MI_AI_EnableChn(AiDevId, u32ChnIdx), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
			goto DISABLE_DEVICE;
        }
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        // AED
        if (bAiEnableAed)
        {
            memcpy(&stAiSetAedConfig, &stAedCfg, sizeof(MI_AI_AedConfig_t));
            ExecFunc(AI_SetAedAttr(AiDevId, u32ChnIdx, &stAiSetAedConfig), MI_SUCCESS);
            ExecFunc(AI_GetAedAttr(AiDevId, u32ChnIdx, &stAiGetAedConfig), MI_SUCCESS);
            ExecFunc(AI_EnableAed(AiDevId, u32ChnIdx), MI_SUCCESS);

            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAiAedConfig.bEnableBabyCry = TRUE;
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAiAedConfig.bEnableLsd = TRUE;
            PRINTF_DBG("AiDevId = %d, u32ChnIdx = %d.\n", AiDevId, u32ChnIdx);
            stAiChnPriv[u32ChnIdx].bEnableAed = TRUE;
        }

		// SSL
		if (bAiEnableSsl)
		{
			if ((TRUE == bAiEnableBf) && (FALSE == bAiSetBfDoa))
			{
				stSslInit.bBfMode = TRUE;
			}

			if (E_MI_AUDIO_SAMPLE_RATE_8000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 50;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_16000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 100;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_32000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 200;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_48000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 300;
			}

			ExecFunc(AI_SetSslInitAttr(AiDevId, u32ChnIdx, &stSslInit), MI_SUCCESS);
			ExecFunc(AI_GetSslInitAttr(AiDevId, u32ChnIdx, &stAiGetSslInitAttr), MI_SUCCESS);
			ExecFunc(AI_SetSslConfigAttr(AiDevId, u32ChnIdx, &stSslConfig), MI_SUCCESS);
			ExecFunc(AI_GetSslConfigAttr(AiDevId, u32ChnIdx, &stAiGetSslConfigAttr), MI_SUCCESS);
			ExecFunc(AI_EnableSsl(AiDevId, u32ChnIdx), MI_SUCCESS);
		}

		// BF
		if (bAiEnableBf)
		{
			ExecFunc(AI_SetBfInitAttr(AiDevId, u32ChnIdx, &stBfInit), MI_SUCCESS);
			ExecFunc(AI_GetBfInitAttr(AiDevId, u32ChnIdx, &stAiGetBfInitAttr), MI_SUCCESS);
			ExecFunc(AI_SetBfConfigAttr(AiDevId, u32ChnIdx, &stBfConfig), MI_SUCCESS);
			ExecFunc(AI_GetBfConfigAttr(AiDevId, u32ChnIdx, &stAiGetBfConfigAttr), MI_SUCCESS);
			if (bAiSetBfDoa)
			{
			    ExecFunc(AI_SetBfAngle(AiDevId, u32ChnIdx, s32AiBfDoa), MI_SUCCESS);
			}
			ExecFunc(AI_EnableBf(AiDevId, u32ChnIdx), MI_SUCCESS);
		}

        // VQE
        if(bAiEnableVqe)
        {
            ExecFunc(AI_SetVqeAttr(AiDevId, u32ChnIdx, 0, 0, &stAiSetVqeConfig), MI_SUCCESS);
            ExecFunc(AI_GetVqeAttr(AiDevId, u32ChnIdx, &stAiGetVqeConfig), MI_SUCCESS);
            ExecFunc(AI_EnableVqe(AiDevId, u32ChnIdx), MI_SUCCESS);
            if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
            {
            	eAiWavSampleRate = u32VqeWorkingSampleRate;
            }
        }

        // RES
        if(bAiEnableResample)
        {
            ExecFunc(AI_EnableReSmp(AiDevId, u32ChnIdx, eAiOutputResampleRate), MI_SUCCESS);
            eAiWavSampleRate = eAiOutputResampleRate;
        }

        // AENC finish
        if (bAiEnableAenc)
        {
            ExecFunc(AI_SetAencAttr(AiDevId, u32ChnIdx, &stAiSetAencConfig), MI_SUCCESS);
            ExecFunc(AI_EnableAenc(AiDevId, u32ChnIdx), MI_SUCCESS);
        }
        pthread_create(&stAiChnPriv[u32ChnIdx].tid, NULL, aiGetChnPortBuf, &stAiChnPriv[u32ChnIdx]);
    }
    PRINTF_INFO("Create ai thread done.\n");
    return 0;

DISABLE_CHANNEL:
	for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
	{
		ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
	}
DISABLE_DEVICE:
	ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);

ERROR_RETURN:
	return s32Ret;
}

MI_S32 initAo(void)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AO_AdecConfig_t stAoSetAdecConfig, stAoGetAdecConfig;
    MI_AO_VqeConfig_t stAoSetVqeConfig, stAoGetVqeConfig;
    MI_S32 s32AoGetVolume;
#if 0
    MI_AO_ChnParam_t stAoChnParam;
#endif
    MI_U32 u32DmaBufSize;
	MI_BOOL bInitAdec = FALSE, bInitRes = FALSE, bInitVqe = FALSE;

	/* add for AO init start */
    PRINTF_DBG("AoDevId = %d, AoChn = %d.\n", AoDevId, AoChn);
    s32Ret = _AUDIO_QueueInit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stChnTempQueue, AUDIO_DEMO_TEMP_SIZE);
    if (0 != s32Ret)
    {
        PRINTF_INFO("Init stChnTempQueue failed.\n");
        return -1;
    }
    PRINTF_DBG("Init stChnTempQueue succeed.\n");
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff = malloc(AUDIO_DEMO_TEMP_SIZE);
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff = malloc(AUDIO_DEMO_TEMP_SIZE);

    if ((NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff)
            || (NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff))
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff = NULL;
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff = NULL;
        PRINTF_INFO("malloc pu8InputBuff or pu8OutputBuff failed.\n");
    }
    PRINTF_DBG("malloc pu8InputBuff and OutputBuff succeed !!!\n");

	/* add for AO init end */
    AoReadFd = open((const char *)pu8AoInputPath, O_RDONLY, 0666);
    if(AoReadFd <= 0)
    {
        PRINTF_INFO("Open input file failed:%s \n", pu8AoInputPath);
        PRINTF_INFO("error:%s", strerror(errno));
        return -1;
    }

    s32Ret = read(AoReadFd, &stWavHeaderInput, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        PRINTF_INFO("Read wav header failed!!!\n");
        return -1;
    }

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
    stAoSetAttr.u32ChnCnt = stWavHeaderInput.wave.wChannels;

    if (stAoSetAttr.u32ChnCnt == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if(stAoSetAttr.u32ChnCnt == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stAoSetAttr.u32ChnCnt = 1;

    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    eAoInSampleRate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    if (bAoEnableResample)
    {
        stAoSetAttr.eSamplerate = eAoOutSampleRate;
    }

    memset(&stAoSetAdecConfig, 0x0, sizeof(MI_AO_AdecConfig_t));
    if (WAV_G711A == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G711A;
        stAoSetAdecConfig.stAdecG711Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG711Cfg.eSoundmode = stAoSetAttr.eSoundmode;
    }
    else if (WAV_G711U == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G711U;
        stAoSetAdecConfig.stAdecG711Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG711Cfg.eSoundmode = stAoSetAttr.eSoundmode;
    }
    else if (WAV_G726 == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G726;
        stAoSetAdecConfig.stAdecG726Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG726Cfg.eSoundmode = stAoSetAttr.eSoundmode;

        switch(stWavHeaderInput.wave.wBitsPerSample)
        {
            case G726_16:
                stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_16;
                break;
            case G726_24:
                stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_24;
                break;
            case G726_32:
                stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_32;
                break;
            case G726_40:
                stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_40;
                break;
            default:
                PRINTF_INFO("G726 Mode Error:%d\n", stWavHeaderInput.wave.wBitsPerSample);
                return -1;
        }
    }

    stAoSetVqeConfig.bAgcOpen = bAoEnableAgc;
    stAoSetVqeConfig.bAnrOpen = bAoEnableNr;
    stAoSetVqeConfig.bEqOpen = bAoEnableEq;
    stAoSetVqeConfig.bHpfOpen = bAoEnableHpf;
    stAoSetVqeConfig.s32FrameSample = 128;
    stAoSetVqeConfig.s32WorkSampleRate = bAoEnableResample == TRUE ? eAoOutSampleRate : eAoInSampleRate;
    memcpy(&stAoSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));
    memcpy(&stAoSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));
    memcpy(&stAoSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));
    memcpy(&stAoSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));

    memcpy(&_gastAoDevInfoApp[AoDevId].stDevAttr, &stAoSetAttr, sizeof(MI_AUDIO_Attr_t));//save Ao parameter
    PRINTF_DBG("eSamplerate = %d\n", stAoSetAttr.eSamplerate);
    ExecFuncNoExit(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AO_GetPubAttr(AoDevId, &stAoGetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }
    PRINTF_DBG("stAoGetAttr.eSamplerate = %d !!!\n", stAoGetAttr.eSamplerate);

    ExecFuncNoExit(MI_AO_Enable(AoDevId), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AO_EnableChn(AoDevId, AoChn), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto DISABLE_DEVICE;
    }

    if (bAoEnableAdec)
    {
        ExecFuncNoExit(AO_SetAdecAttr(AoDevId, AoChn, &stAoSetAdecConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }

        ExecFuncNoExit(AO_EnableAdec(AoDevId, AoChn), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }

	    bInitAdec = TRUE;
    }

    if(bAoEnableResample)
    {
        PRINTF_DBG("eAoInSampleRate = %d\n", eAoInSampleRate);
        ExecFuncNoExit(AO_EnableReSmp(AoDevId, AoChn, eAoInSampleRate), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }
	    bInitRes = TRUE;
    }

    if(bAoEnableVqe)
    {
        ExecFuncNoExit(AO_SetVqeAttr(AoDevId, AoChn, &stAoSetVqeConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }

        ExecFuncNoExit(AO_GetVqeAttr(AoDevId, AoChn, &stAoGetVqeConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }

        ExecFuncNoExit(AO_EnableVqe(AoDevId, AoChn), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
	    {
			goto DISABLE_ALG;
	    }

	    bInitVqe = TRUE;
    }

    if (bAoSetVolume)
    {
        ExecFunc(MI_AO_SetVolume(AoDevId, 0, s32AoVolume, E_MI_AO_GAIN_FADING_64_SAMPLE), MI_SUCCESS);
        ExecFunc(MI_AO_GetVolume(AoDevId, 0, &s32AoGetVolume), MI_SUCCESS);
    }

    s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * (stAoSetAttr.u32ChnCnt);
    if (E_MI_AUDIO_SAMPLE_RATE_8000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_8K;;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_16000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_16K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_32000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_32K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_48000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_48K;
    }

    if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_STEREO)
    {
        if (s32NeedSize > (u32DmaBufSize / 4))
        {
            s32NeedSize = u32DmaBufSize / 4;
        }
    }
    else if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_MONO)
    {
        if (s32NeedSize > (u32DmaBufSize / 8))
        {
            s32NeedSize = u32DmaBufSize / 8;
        }
    }

    s32SendFrameSize = s32NeedSize;  /* add for send frame */

    if (bAoEnableResample)
    {
        s32NeedSize = s32NeedSize * eAoInSampleRate / eAoOutSampleRate;
        if (s32NeedSize > MI_AUDIO_SAMPLE_PER_FRAME * 4)
        {
            s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 4;
        }
    }

    if (bAoEnableAdec)
    {
        if (stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G711A
            || stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G711U)
        {
            s32NeedSize /= 2;
        }
        else if (stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G726)
        {
            switch (stAoSetAdecConfig.stAdecG726Cfg.eG726Mode)
            {
                case E_MI_AUDIO_G726_MODE_16:
                    s32NeedSize = s32NeedSize * 2 / 16;
                    break;
                case E_MI_AUDIO_G726_MODE_24:
                    s32NeedSize = s32NeedSize * 3 / 16;
                    break;
                case E_MI_AUDIO_G726_MODE_32:
                    s32NeedSize = s32NeedSize * 4 / 16;
                    break;
                case E_MI_AUDIO_G726_MODE_40:
                    s32NeedSize = s32NeedSize * 5 / 16;
                    break;
                default:
                    assert(0);
                    break;
            }
        }
    }

    s32NeedSize = s32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * 1) * (stAoSetAttr.u32ChnCnt * 2 * 1);
    pthread_create(&Aotid, NULL, aoSendFrame, NULL);
    PRINTF_INFO("Create ao thread done.\n");
	return 0;

DISABLE_ALG:
	if (bAoEnableAdec && bInitAdec)
    {
        ExecFunc(AO_DisableAdec(AoDevId, AoChn), MI_SUCCESS);
    }

    if(bAoEnableResample && bInitRes)
    {
        ExecFunc(AO_DisableReSmp(AoDevId, AoChn), MI_SUCCESS);
    }

    if(bAoEnableVqe && bInitVqe)
    {
        ExecFunc(AO_DisableVqe(AoDevId, AoChn), MI_SUCCESS);
    }

DISABLE_CHANNEL:
	ExecFunc(MI_AO_DisableChn(AoDevId, AoChn), MI_SUCCESS);
DISABLE_DEVICE:
	ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);
ERROR_RETURN:
	return s32Ret;
}

MI_S32 deinitAi(void)
{
    MI_U32 u32ChnIdx;
    MI_U32 u32ChnCnt;

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
		|| bAiEnableHwAec
#endif
	 )
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        if(bAiEnableAed)
        {
            ExecFunc(AI_DisableAed(AiDevId, u32ChnIdx), MI_SUCCESS);
        }

        if (bAiEnableAenc)
        {
            ExecFunc(AI_DisableAenc(AiDevId, u32ChnIdx), MI_SUCCESS);
        }

        if(bAiEnableResample)
        {
            ExecFunc(AI_DisableReSmp(AiDevId, u32ChnIdx), MI_SUCCESS);
        }

        if(bAiEnableVqe)
        {
            ExecFunc(AI_DisableVqe(AiDevId, u32ChnIdx), MI_SUCCESS);
        }

		if (bAiEnableSsl)
		{
			ExecFunc(AI_DisableSsl(AiDevId, u32ChnIdx), MI_SUCCESS);
		}

		if (bAiEnableBf)
		{
			ExecFunc(AI_DisableBf(AiDevId, u32ChnIdx), MI_SUCCESS);
		}

        ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);

        _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stTempChnQueue);

        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff);
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8InputBuff = NULL;
        }
        if(NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff);
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].pu8OutputBuff = NULL;
        }
    }
    ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);

#if (defined(AI_INPUT_FILE_FOR_TEST_ALG))   //AI input file for test alg

    if(NULL != fpAiTestAlg)
    {
        fclose(fpAiTestAlg);
        fpAiTestAlg = NULL;
    }
    if(NULL != fpAiTestAlgForAec)
    {
        fclose(fpAiTestAlgForAec);
        fpAiTestAlgForAec = NULL;
    }

#endif  //AI input file for test alg

#if (defined(APP_AI_DUMP_AEC_ENABLE))   //AI dump Aec files

    if(NULL != fpAiDumpAecNear)
    {
        fclose(fpAiDumpAecNear);
        fpAiDumpAecNear = NULL;
    }
    if(NULL != fpAiDumpAecFar)
    {
        fclose(fpAiDumpAecFar);
        fpAiDumpAecFar = NULL;
    }
    if(NULL != fpAiDumpAecOutput)
    {
        fclose(fpAiDumpAecOutput);
        fpAiDumpAecOutput = NULL;
    }

    if(NULL != fpAiDumpMiFile[0])
    {
        fclose(fpAiDumpMiFile[0]);
        fpAiDumpMiFile[0] = NULL;
    }
    if(NULL != fpAiDumpMiFile[1])
    {
        fclose(fpAiDumpMiFile[1]);
        fpAiDumpMiFile[1] = NULL;
    }
#endif  //AI dump Aec files

    return 0;
}

MI_S32 deinitAo(void)
{
    if (bAoEnableAdec)
    {
        ExecFunc(AO_DisableAdec(AoDevId, AoChn), MI_SUCCESS);
    }

    if(bAoEnableResample)
    {
        ExecFunc(AO_DisableReSmp(AoDevId, AoChn), MI_SUCCESS);
    }

    if(bAoEnableVqe)
    {
        ExecFunc(AO_DisableVqe(AoDevId, AoChn), MI_SUCCESS);
    }

    ExecFunc(MI_AO_DisableChn(AoDevId, AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);

    _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stChnTempQueue);

    if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff)
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff = NULL;
    }
    if(NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff)
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff = NULL;
    }

    return 0;
}

#if 1

/*enable ao src start*/
#if 1
static SrcConversionMode _AUDIO_GetSrcConvertMode(MI_AUDIO_SampleRate_e  eInResampleRate, MI_AUDIO_SampleRate_e eOutSamplerate)
{
    switch (eInResampleRate)
    {

        case E_MI_AUDIO_SAMPLE_RATE_8000:
            switch (eOutSamplerate)
            {
                case E_MI_AUDIO_SAMPLE_RATE_16000:
                    return SRC_8k_to_16k;

                case E_MI_AUDIO_SAMPLE_RATE_32000:
                    return SRC_8k_to_32k;

                case E_MI_AUDIO_SAMPLE_RATE_48000:
                    return SRC_8k_to_48k;

                default:
                    PRINTF_INFO("Can't found correct mode for sample rate %d to %d\n", eInResampleRate, eOutSamplerate);
                    break;
            }
            break;

        case E_MI_AUDIO_SAMPLE_RATE_16000:
            switch (eOutSamplerate)
            {
                case E_MI_AUDIO_SAMPLE_RATE_8000:
                    return SRC_16k_to_8k;

                case E_MI_AUDIO_SAMPLE_RATE_32000:
                    return SRC_16k_to_32k;

                case E_MI_AUDIO_SAMPLE_RATE_48000:
                    return SRC_16k_to_48k;

                default:
                    PRINTF_INFO("Can't found correct mode for sample rate %d to %d\n", eInResampleRate, eOutSamplerate);
                    break;
            }
            break;

        case E_MI_AUDIO_SAMPLE_RATE_32000:
            switch (eOutSamplerate)
            {
                case E_MI_AUDIO_SAMPLE_RATE_8000:
                    return SRC_32k_to_8k;

                case E_MI_AUDIO_SAMPLE_RATE_16000:
                    return SRC_32k_to_16k;

                case E_MI_AUDIO_SAMPLE_RATE_48000:
                    return SRC_32k_to_48k;

                default:
                    PRINTF_INFO("Can't found correct mode for sample rate %d to %d\n", eInResampleRate, eOutSamplerate);
                    break;
            }
            break;

        case E_MI_AUDIO_SAMPLE_RATE_48000:
            switch (eOutSamplerate)
            {
                case E_MI_AUDIO_SAMPLE_RATE_8000:
                    return SRC_48k_to_8k;

                case E_MI_AUDIO_SAMPLE_RATE_16000:
                    return SRC_48k_to_16k;

                case E_MI_AUDIO_SAMPLE_RATE_32000:
                    return SRC_48k_to_32k;

                default:
                    PRINTF_INFO("Can't found correct mode for sample rate %d to %d\n", eInResampleRate, eOutSamplerate);
                    break;
            }
            break;

        default:
            PRINTF_INFO("Can't found correct mode for sample rate %d to %d\n", eInResampleRate, eOutSamplerate);
            break;
    }

    return (SrcConversionMode)(-1); // fail case
}

static MI_S32 _AO_ReSmpInit(MI_AUDIO_DEV AoDevId, MI_AI_CHN AoChn, MI_AUDIO_SampleRate_e eInSampleRate)
{
    MI_S32 s32Ret = MI_SUCCESS;

    SRCStructProcess stSrcStruct;
    MI_AUDIO_SampleRate_e eAttrSampleRate;
    MI_U16 u16ChanlNum;
    MI_U32 u32SrcUnit;
    MI_S32 SrcBuffSize;

    /* SRC parameter setting */
    eAttrSampleRate = _gastAoDevInfoApp[AoDevId].stDevAttr.eSamplerate;

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAoDevInfoApp[AoDevId].stDevAttr.eSoundmode);

    if ((1 != u16ChanlNum) && (2 != u16ChanlNum))
    {
        PRINTF_INFO("It can not support %d Channels.\n", u16ChanlNum);
        return -1;
    }

    u32SrcUnit = _gastAoDevInfoApp[AoDevId].stDevAttr.u32PtNumPerFrm;
    stSrcStruct.WaveIn_srate = (SrcInSrate) (eInSampleRate / 1000);
    stSrcStruct.channel = u16ChanlNum;
    stSrcStruct.mode = _AUDIO_GetSrcConvertMode(eInSampleRate, eAttrSampleRate);
    PRINTF_INFO("[SRC]stSrcStruct.mode = %d \n", stSrcStruct.mode);

    u32SrcUnit = u32AoSrcPointNumber;
    stSrcStruct.point_number = u32SrcUnit;

    //(1)IaaSrc_GetBufferSize
    SrcBuffSize = IaaSrc_GetBufferSize(stSrcStruct.mode);
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf = malloc(SrcBuffSize);
    if (NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf)
    {
        PRINTF_INFO("Malloc IaaSrc_GetBufferSize failed.\n");
        return -1;
    }
    PRINTF_INFO("Malloc IaaSrc_GetBufferSize succeed, size = %d\n", SrcBuffSize);

    //(2)IaaSrc_Init
    stSrcStruct.channel = 1;
    stSrcStruct.mode = SRC_8k_to_16k;
    stSrcStruct.point_number = 256;
    stSrcStruct.WaveIn_srate = SRATE_8K;

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle =IaaSrc_Init(
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf, &stSrcStruct);
    if (_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle == NULL)
    {
        PRINTF_INFO("_MI_AO_ReSmpInit Fail !!!!! \n");
        return -1;
    }
    PRINTF_INFO("_MI_AO_ReSmpInit succeed, hSrcHandle = %p, point_number = %d, channel = %d, WaveIn_srate = %d\n", \
                _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle, stSrcStruct.point_number, stSrcStruct.channel, stSrcStruct.WaveIn_srate);
    PRINTF_INFO("[AO SRC]end, Waiting run...\n");

    return s32Ret;
}

MI_S32 AO_EnableReSmp(
                            MI_AUDIO_DEV AoDevId,
                            MI_AO_CHN AoChn,
                            MI_AUDIO_SampleRate_e eInSampleRate)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_BOOL bInitSuccess = TRUE;

    do
    {
        /* SRC parameter setting */
        s32Ret = _AO_ReSmpInit(AoDevId, AoChn, eInSampleRate);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("_AO_ReSmpInit failed.\n");
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("_AO_ReSmpInit succeed.\n");

        s32Ret = _AUDIO_QueueInit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stSrcInputQueue, AUDIO_DEMO_TEMP_SIZE);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("_AUDIO_QueueInit failed.\n");
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("_AUDIO_QueueInit succeed, size = %d\n", AUDIO_DEMO_TEMP_SIZE);
    }
    while(0);

    if (FALSE == bInitSuccess)
    {
        PRINTF_INFO("_AO_ReSmpInit or  _AUDIO_QueueInit failed !!!\n");
        if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle)
        {
            IaaSrc_Release(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle);
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle = NULL;
        }

        if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf)
        {
            free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf);
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf = NULL;
        }
        _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stSrcInputQueue);
        return -1;
    }

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bResampleEnable = TRUE;
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].eInResampleRate = eInSampleRate;

    return 0;
}

static MI_S32 _AO_DoSrc(
                            MI_AUDIO_DEV AoDevId,
                            MI_AO_CHN AoChn,
                            _AUDIO_QueueInfo_t *pstInputQueue,
                            _AUDIO_QueueInfo_t *pstOutputQueue)
{
    PRINTF_DBG("AoDevId = %d, AoChn = %d\n", AoDevId, AoChn);
    MI_S32 s32GetSize = 0;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U8 *pu8InputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff;
    MI_U8 *pu8OutputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff;
    MI_S32 s32BitwidthByte = 2;
    MI_U32 u32ChnCnt;
    SRC_HANDLE hSrcHandle;
    MI_U32 u32SrcUnit;
    MI_S32 s32SrcOutSample;
    ALGO_SRC_RET ret;

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnCnt, _gastAoDevInfoApp[AoDevId].stDevAttr.eSoundmode);

    if ((1 != u32ChnCnt) && (2 != u32ChnCnt))
    {
        PRINTF_INFO("It can not support %d Channels.\n", u32ChnCnt);
        return -1;
    }
    if(pu8InputBuff == NULL || pu8OutputBuff == NULL)
    {
        PRINTF_INFO("pu8InputBuff or pu8OutputBuff is NULL !!!\n");
    }

    hSrcHandle = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle;
    u32SrcUnit = u32AoSrcPointNumber;
    s32GetSize = u32SrcUnit * s32BitwidthByte * u32ChnCnt;  //256*2*1 = 512
    PRINTF_DBG("pstInputQueue->s32Size = %d, s32GetSize = %d\n",pstInputQueue->s32Size, s32GetSize);

    while(pstInputQueue->s32Size >= s32GetSize)
    {
        memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);

        if (MI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
        PRINTF_DBG("QueueDraw succeed, s32GetSize = %d, DATA:%s\n", s32GetSize, pu8InputBuff);
        s32SrcOutSample = s32GetSize;

        if (NULL == hSrcHandle)
        {
            PRINTF_INFO("hSrcHandle is NULL !!!\n");
        }
        else
        {
            PRINTF_DBG("hSrcHandle = %p\n", hSrcHandle);
            ret = IaaSrc_Run(hSrcHandle, (MI_S16 *)pu8InputBuff, (MI_S16 *)pu8OutputBuff, &s32SrcOutSample);
            PRINTF_DBG("IaaSrc_Run ret = %d\n", ret);
        }

        if (s32SrcOutSample * s32BitwidthByte * u32ChnCnt > AUDIO_DEMO_TEMP_SIZE)
        {
            PRINTF_INFO("Src out size is too big:%d\n", s32SrcOutSample * s32BitwidthByte * u32ChnCnt);
            return -1;
        }

        s32Ret =_AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32SrcOutSample * s32BitwidthByte * u32ChnCnt);
        if (E_APP_ERR_NO_BUFFER == s32Ret)
        {
            return s32Ret;
        }
    }

    return MI_SUCCESS;
}

MI_S32 AO_DisableReSmp(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

    _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stSrcInputQueue);
    if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle)
    {
        IaaSrc_Release(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hSrcHandle = NULL;
    }

    if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf)
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pSrcWorkingBuf = NULL;
    }

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bResampleEnable = FALSE;

    return s32Ret;
}

#endif
/*enable ao src end*/

MI_S32 AO_SetAdecAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_AdecConfig_t *pstAdecConfig)
{
    MI_S32 s32Ret = MI_SUCCESS;

    AUDIO_CHECK_POINTER(pstAdecConfig);

    if ((pstAdecConfig->eAdecType != E_MI_AUDIO_ADEC_TYPE_G711A)
            && (pstAdecConfig->eAdecType != E_MI_AUDIO_ADEC_TYPE_G711U)
            && (pstAdecConfig->eAdecType != E_MI_AUDIO_ADEC_TYPE_G726)
       )
    {
        PRINTF_INFO("Invalid AdecType[%d], Dev[%d]Chn[%d]!!!\n", pstAdecConfig->eAdecType, AoDevId, AoChn);
        return -1;
    }

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecAttrSet = TRUE;
    memcpy(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoAdecConfig, pstAdecConfig, sizeof(MI_AO_AdecConfig_t));

    return 0;
}

MI_S32 _AO_G726Init(MI_AUDIO_DEV AoDevId, stG726State_t *pstG726Info, MI_S32 s32BitRate)
{
    AUDIO_CHECK_POINTER(pstG726Info);
    g726_init(pstG726Info, s32BitRate);
    return MI_SUCCESS;
}

MI_S32 _AO_AdecInit(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AO_AdecConfig_t stAoAdecConfig;
    stG726State_t *pstG726Info = NULL;

    stAoAdecConfig = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoAdecConfig;

    if (E_MI_AUDIO_ADEC_TYPE_G726 == stAoAdecConfig.eAdecType)
    {
        pstG726Info = malloc(sizeof(stG726State_t));
        AUDIO_CHECK_POINTER(pstG726Info);
        memset(pstG726Info, 0, sizeof(stG726State_t));

        switch(stAoAdecConfig.stAdecG726Cfg.eG726Mode)
        {
            case E_MI_AUDIO_G726_MODE_16:
                _AO_G726Init(AoDevId, pstG726Info, 8000 * 2);
                PRINTF_INFO("_AO_G726Init 16k\n");
                break;

            case E_MI_AUDIO_G726_MODE_24:
                _AO_G726Init(AoDevId, pstG726Info, 8000 * 3);
                PRINTF_INFO("_AO_G726Init 24k\n");
                break;

            case E_MI_AUDIO_G726_MODE_32:
                _AO_G726Init(AoDevId, pstG726Info, 8000 * 4);
                PRINTF_INFO("_AO_G726Init 32k\n");
                break;

            case E_MI_AUDIO_G726_MODE_40:
                _AO_G726Init(AoDevId, pstG726Info, 8000 * 5);
                PRINTF_INFO("_AO_G726Init 40k\n");
                break;

            default:
                PRINTF_INFO("G726 Mode is not find:%d\n", stAoAdecConfig.stAdecG726Cfg.eG726Mode);
                free(pstG726Info);
                return -1;
        }
    }
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle = pstG726Info;

    return s32Ret;
}

MI_S32 AO_EnableAdec(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_BOOL bInitSuccess = TRUE;

    if (TRUE != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecAttrSet)
    {
        PRINTF_INFO("Dev%d Chn%d Adec Attr is not set\n", AoDevId, AoChn);
        return -1;
    }

    if (TRUE == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecEnable)
    {
        return 0;
    }

    do
    {
        s32Ret = _AO_AdecInit(AoDevId, AoChn);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("_AO_AdecInit failed \n");
            bInitSuccess = FALSE;
        }

        //5, G726 Max 5 byte alignment
        //8, G726_16, 2bit convert 16 bit
        //6, Res 8K->48K

        s32Ret = _AUDIO_QueueInit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAdecInputQueue, AUDIO_DEMO_TEMP_SIZE);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("_AUDIO_QueueInit AO stAdecInputQueue failed.\n");
            bInitSuccess = FALSE;
        }
    }
    while(0);

    if (FALSE == bInitSuccess)
    {
        if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle)
        {
            free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle);
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle = NULL;
        }
        _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAdecInputQueue);
        return -1;
    }
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecEnable = TRUE;

    return 0;
}

MI_S32 AO_DisableAdec(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

    if (TRUE != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecAttrSet)
    {
        PRINTF_INFO("Dev%d Chn%d Aenc Attr is not set\n", AoDevId, AoChn);
        return -1;
    }

    _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAdecInputQueue);
    if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle)
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle = NULL;
    }

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bAdecEnable = FALSE;

    return 0;
}

MI_S32 _AO_G711Decoder(
                            MI_AUDIO_DEV AoDevId,
                            MI_AO_CHN AoChn,
                            _AUDIO_QueueInfo_t *pstInputQueue,
                            _AUDIO_QueueInfo_t *pstOutputQueue,
                            MI_AUDIO_AdecType_e eG711Type)
{
    MI_S32 s32GetSize= 0;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U8 *pu8InputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff;
    MI_U8 *pu8OutputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff;

    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);

    memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);

    //g711 decoder
    if (pstInputQueue->s32Size < AUDIO_DEMO_TEMP_SIZE / 2)
    {
        s32GetSize = pstInputQueue->s32Size;
    }
    else
    {
        s32GetSize =  AUDIO_DEMO_TEMP_SIZE / 2;
    }

    s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
    if (s32Ret != MI_SUCCESS)
    {
        return -1;
    }

    switch(eG711Type)
    {
        case E_MI_AUDIO_ADEC_TYPE_G711A:
            G711Decoder((MI_S16 *)pu8OutputBuff, pu8InputBuff, s32GetSize, AUDIO_AO_G711A);
            break;

        case E_MI_AUDIO_ADEC_TYPE_G711U:
            G711Decoder((MI_S16 *)pu8OutputBuff, pu8InputBuff, s32GetSize, AUDIO_AO_G711U);
            break;

        default:
            PRINTF_INFO("G711 Decoder Type not find:%d\n", eG711Type);
            return -1;
    }

    s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32GetSize * 2);

    return s32Ret;
}

MI_S32 _MI_AO_G726Decoder(
                            MI_AUDIO_DEV AoDevId,
                            MI_AO_CHN AoChn,
                            _AUDIO_QueueInfo_t *pstInputQueue,
                            _AUDIO_QueueInfo_t *pstOutputQueue,
                            MI_AUDIO_G726Mode_e eG726Mode)
{
    MI_S32 s32WriteSize = 0;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32AdecUnit;
    MI_U8 *pu8InputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff;
    MI_U8 *pu8OutputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8OutputBuff;
    stG726State_t *hG726Handle = NULL;

    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);

    //g726_16  1bytes->8bytes  4short
    //g726_24  3bytes->16bytes 8short
    //g726_32  1bytes->4bytes  2short
    //g726_40  5bytes->16bytes 8short

    u32AdecUnit = pstInputQueue->s32Size;

    if (u32AdecUnit < AUDIO_AO_ADEC_G726_UNIT)
    {
        return MI_SUCCESS;
    }
    else if (u32AdecUnit > AUDIO_AO_ADEC_G726_UNIT_MAX)
    {
        u32AdecUnit = AUDIO_AO_ADEC_G726_UNIT_MAX;
    }
    else
    {
        u32AdecUnit = u32AdecUnit - (u32AdecUnit % AUDIO_AO_ADEC_G726_UNIT);
    }

    while(pstInputQueue->s32Size >= u32AdecUnit)
    {
        memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);

        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, u32AdecUnit);
        if (MI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }

        hG726Handle = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hG726Handle;
        if (NULL == hG726Handle)
        {
            PRINTF_INFO("hG726Handle is NULL!!!!!!\n");
        }
        else
        {
            s32WriteSize = g726_decode(hG726Handle, (MI_S16 *)pu8OutputBuff, pu8InputBuff, u32AdecUnit);
        }

        if (s32WriteSize <= 0)
        {
            PRINTF_INFO("s32WriteSize:%d\n", s32WriteSize);
            return -1;
        }

        if (s32WriteSize * 2 > AUDIO_DEMO_TEMP_SIZE)
        {
            PRINTF_INFO("g726 decode size is too big:%d\n", s32WriteSize);
            return -1;
        }
        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32WriteSize * 2);
        if(0 != s32Ret)
        {
            PRINTF_INFO("Queue has not enough space.\n");
            return s32Ret;
        }
    }

    return s32Ret;
}

MI_S32 _AO_DoAdec(
                                MI_AUDIO_DEV AoDevId,
                                MI_AO_CHN AoChn,
                                _AUDIO_QueueInfo_t *pstInputQueue,
                                _AUDIO_QueueInfo_t *pstOutputQueue)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AO_AdecConfig_t *pstAoAdecInfo = NULL;

    pstAoAdecInfo =  &_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoAdecConfig;
    AUDIO_CHECK_POINTER(pstAoAdecInfo);

    switch(pstAoAdecInfo->eAdecType)
    {
        case E_MI_AUDIO_ADEC_TYPE_G711A:
        case E_MI_AUDIO_ADEC_TYPE_G711U:
            s32Ret = _AO_G711Decoder(AoDevId, AoChn, pstInputQueue, pstOutputQueue, pstAoAdecInfo->eAdecType);
            break;

        case E_MI_AUDIO_ADEC_TYPE_G726:
            s32Ret = _MI_AO_G726Decoder(AoDevId, AoChn, pstInputQueue, pstOutputQueue, pstAoAdecInfo->stAdecG726Cfg.eG726Mode);
            break;

        default:
            PRINTF_INFO("AdecType is not find:%d\n", pstAoAdecInfo->eAdecType);
            s32Ret = -1;
            break;
    }
    return s32Ret;
}

/*enable ao vqe start*/
#if 1

MI_S32 _AO_VqeInit(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = 0;
    AudioProcessInit stApstruct;
    AudioApcBufferConfig stApcBufCfg;
    MI_U16 u16ChanlNum;
    MI_AO_VqeConfig_t stAoVqeConfig;
    AudioAnrConfig stAnrInfo;
    AudioEqConfig  stEqInfo;
    AudioHpfConfig stHpfInfo;
    AudioAgcConfig stAgcInfo;
    MI_U32 u32ApcBufSize = 0;

	int intensity_band[6] = {3,24,40,64,80,128};
	int intensity[7] = {30,30,30,30,30,30,30};

    stAoVqeConfig = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoVqeConfig;

    //Apc init arguments
    memset(&stApstruct, 0, sizeof(AudioProcessInit));
    stApstruct.point_number = 128;
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAoDevInfoApp[AoDevId].stDevAttr.eSoundmode);

    if ((1 != u16ChanlNum) && (2 != u16ChanlNum))
    {
        PRINTF_INFO("It can not support %d Channels.\n", u16ChanlNum);
        return -1;
    }
    PRINTF_INFO("stApstruct.channel = %d.\n", u16ChanlNum);

    stApstruct.channel = u16ChanlNum;
	PRINTF_DBG("stApstruct.channel = %d!\n", stApstruct.channel);
    stApstruct.channel = 1;
    AUDIO_VQE_SAMPLERATE_TRANS_TYPE(_gastAoDevInfoApp[AoDevId].stDevAttr.eSamplerate, stApstruct.sample_rate);
    stApstruct.sample_rate = IAA_APC_SAMPLE_RATE_16000;

    /* APC Buffer Config Init*/
    //memset(&stApcBufCfg, 0, sizeof(AudioApcBufferConfig));
    stApcBufCfg.agc_enable = stAoVqeConfig.bAgcOpen;
    stApcBufCfg.anr_enable = stAoVqeConfig.bAnrOpen;
    stApcBufCfg.dr_enable = 0;
    stApcBufCfg.eq_enable = stAoVqeConfig.bEqOpen || stAoVqeConfig.bHpfOpen ;
    stApcBufCfg.vad_enable = 0;

    PRINTF_INFO("Agc:%d, Anr:%d, Eq:%d, Hpf:%d\n", stAoVqeConfig.bAgcOpen, stAoVqeConfig.bAnrOpen, stAoVqeConfig.bEqOpen, stAoVqeConfig.bHpfOpen);
    /* APC init */
    //(1)IaaApc_GetBufferSize
    u32ApcBufSize = IaaApc_GetBufferSize(&stApcBufCfg);
    if (0 == u32ApcBufSize)
    {
        PRINTF_INFO("Apc buffer size is zero!!!\n");
        return -1;
    }
    PRINTF_INFO("[%s:%d] Apc buffer size:%d.\n", __func__, __LINE__, u32ApcBufSize);

    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf = (char *)malloc(u32ApcBufSize);
    if (NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf)
    {
        PRINTF_INFO("Malloc IaaApc_GetBuffer failed\n");
        return -1;
    }
    PRINTF_INFO("Malloc IaaApc_GetBuffer succeed\n");
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle =IaaApc_Init(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf,
            &stApstruct,
            &stApcBufCfg);

    if (NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle)
    {
        PRINTF_INFO("IaaApc_Init FAIL !\n");
        return -1;
    }
    PRINTF_INFO("IaaApc_Init succeed !\n");
    //set Anr
    memset(&stAnrInfo, 0, sizeof(AudioAnrConfig));
    if (TRUE == stAoVqeConfig.bAnrOpen)
    {
        stAnrInfo.anr_enable = (MI_U32)stAoVqeConfig.bAnrOpen;
        stAnrInfo.user_mode = (MI_U32)stAoVqeConfig.stAnrCfg.eMode;
        AUDIO_VQE_NR_SPEED_TRANS_TYPE(stAoVqeConfig.stAnrCfg.eNrSpeed, stAnrInfo.anr_converge_speed);
        memcpy(stAnrInfo.anr_intensity_band,
            stAoVqeConfig.stAnrCfg.u32NrIntensityBand,
            sizeof(stAoVqeConfig.stAnrCfg.u32NrIntensityBand));
        memcpy(stAnrInfo.anr_intensity,
            stAoVqeConfig.stAnrCfg.u32NrIntensity,
            sizeof(stAoVqeConfig.stAnrCfg.u32NrIntensity));
        stAnrInfo.anr_smooth_level = stAoVqeConfig.stAnrCfg.u32NrSmoothLevel;
    }

    //set Eq
    memset(&stEqInfo, 0, sizeof(AudioEqConfig));
    if (TRUE == stAoVqeConfig.bEqOpen)
    {
        stEqInfo.eq_enable = (MI_U32)stAoVqeConfig.bEqOpen;
        stEqInfo.user_mode = (MI_U32)stAoVqeConfig.stEqCfg.eMode;
        memcpy(stEqInfo.eq_gain_db, stAoVqeConfig.stEqCfg.s16EqGainDb, sizeof(stAoVqeConfig.stEqCfg.s16EqGainDb));
    }

    //set Hpf
    memset(&stHpfInfo, 0, sizeof(AudioHpfConfig));
    if (TRUE == stAoVqeConfig.bHpfOpen)
    {
        stHpfInfo.hpf_enable = (MI_U32)stAoVqeConfig.bHpfOpen;
        stHpfInfo.user_mode = (MI_U32)stAoVqeConfig.stHpfCfg.eMode;
        AUDIO_VQE_HPF_TRANS_TYPE(stAoVqeConfig.stHpfCfg.eHpfFreq, stHpfInfo.cutoff_frequency);
    }

    //set Agc
    memset(&stAgcInfo, 0, sizeof(AudioAgcConfig));
    if (TRUE == stAoVqeConfig.bAgcOpen)
    {
        stAgcInfo.agc_enable = (MI_U32)stAoVqeConfig.bAgcOpen;
        stAgcInfo.user_mode = (MI_U32)stAoVqeConfig.stAgcCfg.eMode;
        stAgcInfo.attack_time = stAoVqeConfig.stAgcCfg.u32AttackTime;
        stAgcInfo.release_time = stAoVqeConfig.stAgcCfg.u32ReleaseTime;
        memcpy(stAgcInfo.compression_ratio_input, stAoVqeConfig.stAgcCfg.s16Compression_ratio_input,
            sizeof(stAoVqeConfig.stAgcCfg.s16Compression_ratio_input));
        memcpy(stAgcInfo.compression_ratio_output, stAoVqeConfig.stAgcCfg.s16Compression_ratio_output,
            sizeof(stAoVqeConfig.stAgcCfg.s16Compression_ratio_output));
        stAgcInfo.drop_gain_max = stAoVqeConfig.stAgcCfg.u32DropGainMax;
        stAgcInfo.gain_info.gain_init = stAoVqeConfig.stAgcCfg.stAgcGainInfo.s32GainInit;
        stAgcInfo.gain_info.gain_max = stAoVqeConfig.stAgcCfg.stAgcGainInfo.s32GainMax;
        stAgcInfo.gain_info.gain_min = stAoVqeConfig.stAgcCfg.stAgcGainInfo.s32GainMin;
        stAgcInfo.noise_gate_attenuation_db = stAoVqeConfig.stAgcCfg.u32NoiseGateAttenuationDb;
        stAgcInfo.noise_gate_db = stAoVqeConfig.stAgcCfg.s32NoiseGateDb;
        stAgcInfo.drop_gain_threshold = stAoVqeConfig.stAgcCfg.s32DropGainThreshold;
    }

    if (NULL == _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle)
    {
        PRINTF_INFO("hApcHandle is NULL !\n");
        return -1;
    }
    s32Ret = IaaApc_Config(
                 _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle,
                 &stAnrInfo, &stEqInfo, &stHpfInfo, NULL, NULL, &stAgcInfo);

    if (0 != s32Ret)
    {
        PRINTF_INFO("IaaPac_config FAIL ,s32Ret = %d !\n", s32Ret);
        return -1;
    }
    PRINTF_INFO("IaaApc_Config succeed !\n");

    return s32Ret;
}

MI_S32 AO_SetVqeAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_VqeConfig_t *pstVqeConfig)
{
    MI_S32 s32Ret = 0;

     _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bVqeAttrSet = TRUE;
    memcpy(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoVqeConfig, pstVqeConfig, sizeof(MI_AO_VqeConfig_t));
    PRINTF_INFO("AO_SetVqeAttr done.\n");

    return s32Ret;
}

MI_S32 AO_GetVqeAttr(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_VqeConfig_t *pstVqeConfig)
{
    MI_S32 s32Ret = 0;

    memcpy(pstVqeConfig, &_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stAoVqeConfig, sizeof(MI_AO_VqeConfig_t));
    PRINTF_INFO("AO_GetVqeAttr done.\n");

    return s32Ret;
}

MI_S32 AO_EnableVqe(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = 0;
    MI_BOOL bInitSuccess = TRUE;
    MI_S32 s32Err = 0;
    do
    {
        //VQE init
        s32Ret = _AO_VqeInit(AoDevId, AoChn);
        if (0 != s32Ret)
        {
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("_AO_VqeInit done.\n");

        s32Ret = _AUDIO_QueueInit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stVqeInputQueue, AUDIO_DEMO_TEMP_SIZE);
        if (MI_SUCCESS != s32Ret)
        {
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("_AUDIO_QueueInit done.\n");
    }
    while(0);

    if (FALSE == bInitSuccess)
    {
        if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle)
        {
            IaaApc_Free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle);
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle = NULL;
        }

        if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf)
        {
            free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf);
            _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf = NULL;
        }

        _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stVqeInputQueue);
        goto TO_RETURN;
    }
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bVqeEnable = TRUE;

TO_RETURN:

    return s32Ret;
}

MI_S32 AO_DisableVqe(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    MI_S32 s32Ret = 0;

    if (NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle)
    {
        IaaApc_Free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle = NULL;
    }

    if(NULL != _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf)
    {
        free(_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf);
        _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pApcWorkingBuf = NULL;
    }

    _AUDIO_QueueDeinit(&_gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].stVqeInputQueue);
    _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].bVqeEnable = FALSE;

TO_RETURN:

    return s32Ret;
}

#endif
/*enable ao vqe end*/

/*AO_SendFrame start*/
#if 1

MI_S32 AO_DoAlg_SendFrame(
                            MI_AUDIO_DEV AoDevId,
                            MI_AO_CHN AoChn,
                            MI_AUDIO_Frame_t *pstData,
                            MI_S32 s32MilliSec)
{
    static long T1 = 0;
    long  T0 = 0, T2 = 0, T3 = 0;
    MI_S32  s32Ret = 0;
    MI_U32  u32ChnIdx = 0, u32ChnCnt = 1;

    MI_U32  u32ChnDataLen[AO_MAX_CHN_NUM] = {0, 0};
    MI_BOOL bNoBuff = FALSE;
    MI_U32  u32ChnEstimatedTotalDataSize = 0;
    MI_U32  u32ChnEstimatedChnQueueSize = 0;
    MI_U32  u32NeedDmaFreeSize = 0;
    MI_AUDIO_Frame_t AoSendData;
    MI_AO_AdecConfig_t __attribute__((unused)) *pstChnAoAdecInfo = NULL;
    _AUDIO_QueueInfo_t *pstChnAdecInputQueue = NULL;
    _AUDIO_QueueInfo_t *pstChnSrcInputQueue = NULL;
    _AUDIO_QueueInfo_t *pstChnVqeInputQueue = NULL;
    _AUDIO_QueueInfo_t *pstChnTempQueue = NULL;
    MI_BOOL bChnResampleEnable = FALSE;
    MI_BOOL bChnAdecEnable = FALSE;
    MI_BOOL bChnVqeEnable = FALSE;
    MI_U32  u32ChnEstimatedAdecQueueSize = 0;
    MI_U32  u32ChnEstimatedSrcQueueSize = 0;
    MI_U32  u32ChnEstimatedVqeQueueSize = 0;
    MI_U32  u32AdecUnit = 0;
    MI_U32  u32SrcUnit = 0;
    MI_S32  s32ChnGetSize = 0;
    MI_S32  s32BitwidthByte;

    _AO_ChanInfoUsr_t* pstChanInfo = NULL;
    MI_AUDIO_SoundMode_e eSoundMode;
    eSoundMode = _gastAoDevInfoApp[AoDevId].stDevAttr.eSoundmode;

    /*
        insert queue and do Src/Vqe:
        resample + vqe
        SrcInputQueue->VqeInputQueue->pstChnTempQueue
    */

    /* insert queue  u32ChnIdx == 0 */
    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        pstChanInfo = &_gastAoDevInfoApp[AoDevId].astChanInfo[u32ChnIdx];
        pstChnTempQueue = &(pstChanInfo->stChnTempQueue);
        //PRINTF_DBG("pstChnTempQueue->s32Size = %d.\n", pstChnTempQueue->s32Size);
        bChnResampleEnable = pstChanInfo->bResampleEnable;
        bChnAdecEnable = pstChanInfo->bAdecEnable;
        bChnVqeEnable = pstChanInfo->bVqeEnable;
        //PRINTF_DBG("bChnAdecEnable = %d, bChnResampleEnable = %d, bChnVqeEnable = %d.\n", bChnAdecEnable, bChnResampleEnable, bChnVqeEnable);

        pstChnAdecInputQueue = &(pstChanInfo->stAdecInputQueue);
        pstChnSrcInputQueue = &(pstChanInfo->stSrcInputQueue);
        pstChnVqeInputQueue = &(pstChanInfo->stVqeInputQueue);
        pstChnAoAdecInfo = &(pstChanInfo->stAoAdecConfig);

        if(bChnAdecEnable)
        {
            s32Ret = _AUDIO_QueueInsert(pstChnAdecInputQueue, pstData->apVirAddr[u32ChnIdx], pstData->u32Len[u32ChnIdx]);
            if (E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_DBG("Dev%d Chn%d AO Adec Queue has not enough space.\n", AoDevId, u32ChnIdx);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_DBG("[AO Adec]QueueInsert Size %d in AO_DoAlg_SendFrame().\n", pstData->u32Len[u32ChnIdx]);
            }
        }
        else if (bChnResampleEnable)
        {
            s32Ret = _AUDIO_QueueInsert(pstChnSrcInputQueue, pstData->apVirAddr[u32ChnIdx], pstData->u32Len[u32ChnIdx]);
            if (E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_DBG("Dev%d Chn%d Resample Queue has not enough space.\n", AoDevId, u32ChnIdx);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_DBG("[AO SRC]QueueInsert Size %d in AO_DoAlg_SendFrame().\n", pstData->u32Len[u32ChnIdx]);
            }
        }
        else if (bChnVqeEnable)
        {
            s32Ret = _AUDIO_QueueInsert(pstChnVqeInputQueue, pstData->apVirAddr[u32ChnIdx], pstData->u32Len[u32ChnIdx]);
            if (E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Dev%d Chn%d Vqe Queue has not enough space.\n", AoDevId, u32ChnIdx);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_DBG("QueueInsert Size %d in AO_DoAlg_SendFrame().\n", pstData->u32Len[u32ChnIdx]);
            }
        }
        else
        {
            s32Ret = _AUDIO_QueueInsert(pstChnTempQueue, pstData->apVirAddr[u32ChnIdx], pstData->u32Len[u32ChnIdx]);
            if (E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Dev%d Chn%d Chn Temp Queue has not enough space.\n", AoDevId, u32ChnIdx);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_DBG("QueueInsert Size %d in AO_DoAlg_SendFrame().\n", pstData->u32Len[u32ChnIdx]);
            }
        }
        PRINTF_DBG("bChnResampleEnable = %d, bChnVqeEnable = %d.\n", bChnResampleEnable, bChnVqeEnable);

        if(bChnAdecEnable && bChnResampleEnable)
        {
            s32Ret = _AO_DoAdec(AoDevId, u32ChnIdx, pstChnAdecInputQueue, pstChnSrcInputQueue);

            if (s32Ret != MI_SUCCESS)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoAdec failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }

        }
        else if(bChnAdecEnable && bChnVqeEnable)
        {
            s32Ret = _AO_DoAdec(AoDevId, u32ChnIdx, pstChnAdecInputQueue, pstChnVqeInputQueue);

            if (s32Ret != MI_SUCCESS)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoAdec failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }
        }
        else if(bChnAdecEnable)
        {
            s32Ret = _AO_DoAdec(AoDevId, u32ChnIdx, pstChnAdecInputQueue, pstChnTempQueue);
            if (s32Ret != 0)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoAdec failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }
        }

        if (bChnResampleEnable && bChnVqeEnable)
        {
            s32Ret = _AO_DoSrc(AoDevId, u32ChnIdx, pstChnSrcInputQueue, pstChnVqeInputQueue);

            if (s32Ret != MI_SUCCESS)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoSrc failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }
        }
        else if (bChnResampleEnable)
        {
            s32Ret = _AO_DoSrc(AoDevId, u32ChnIdx, pstChnSrcInputQueue, pstChnTempQueue);
            if (s32Ret != MI_SUCCESS)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoSrc failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }
        }

        if (bChnVqeEnable)
        {
            s32Ret = _AO_DoVqe(AoDevId, u32ChnIdx, pstChnVqeInputQueue, pstChnTempQueue);
            if (s32Ret != 0)
            {
                PRINTF_INFO("Dev%d Chn%d _AO_DoVqe failed.\n", AoDevId, u32ChnIdx);
                goto TO_RETURN;
            }
        }
    }
    PRINTF_DBG("[0]Draw queue start. TempQueue size = %d\n", pstChnTempQueue->s32Size);

    while(pstChnTempQueue->s32Size >= s32SendFrameSize)
    {
        memset(u8TempBuffer, 0, AUDIO_SAMPLE_PER_FRAME * 4);
        s32Ret = _AUDIO_QueueDraw(pstChnTempQueue, u8TempBuffer, s32SendFrameSize);
        if (0 != s32Ret)
        {
            PRINTF_INFO("pstChnTempQueue draw failed.\n");
            return -1;
        }
        memset(&AoSendData, 0x0, sizeof(MI_AUDIO_Frame_t));
        AoSendData.u32Len[0] = s32SendFrameSize;
        AoSendData.apVirAddr[0] = u8TempBuffer;
        AoSendData.apVirAddr[1] = NULL;
        do
        {
            T0 = (long)_OsCounterGetMs();
            PRINTF_DBG("cost time:%dms !!!\n\n", T0 - T1);
            T1 = T0;
            s32Ret = MI_AO_SendFrame(AoDevId, AoChn, &AoSendData, -1);
            T2 = (long)_OsCounterGetMs();
            PRINTF_DBG("Send cost time:%dms !!!\n\n", T2 - T0);
            if(s32Ret == MI_AO_ERR_NOBUF)
            {
                PRINTF_DBG("MI_AO_SendFrame no buffer!!!\n\n");
            }
        }
        while(s32Ret == MI_AO_ERR_NOBUF);
    }

    if(bNoBuff)
    {
        s32Ret = E_APP_ERR_NO_BUFFER;
    }

TO_RETURN:
    PRINTF_DBG("pstChnTempQueue->s32Size = %d.\n", pstChnTempQueue->s32Size);

    return s32Ret;
}

/* Do Vqe */
MI_S32  _AO_DoVqe(MI_AUDIO_DEV AoDevId,
                        MI_AO_CHN AoChn,
                        _AUDIO_QueueInfo_t *pstInputQueue,
                        _AUDIO_QueueInfo_t *pstOutputQueue)
{
    MI_S32 s32Ret = 0;
    MI_S32 s32GetSize;
    MI_S32 s32BitwidthByte = 2;
    MI_U32 u32ChnCnt;
    MI_U8 *pu8InputBuff = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].pu8InputBuff;
    APC_HANDLE hApcHandle;

    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnCnt, _gastAoDevInfoApp[AoDevId].stDevAttr.eSoundmode);

    if ((1 != u32ChnCnt) && (2 != u32ChnCnt))
    {
        PRINTF_INFO("It can not support %d Channels.\n", u32ChnCnt);
        return -1;
    }
	//PRINTF_DBG("u32ChnCnt = %d.\n", u32ChnCnt);
    s32GetSize = AUDIO_VQE_SAMPLES_UNIT * s32BitwidthByte * u32ChnCnt;
    hApcHandle = _gastAoDevInfoApp[AoDevId].astChanInfo[AoChn].hApcHandle;
    if (pstInputQueue->s32Size < s32GetSize)
    {
        return s32Ret;
    }

    while(pstInputQueue->s32Size >= s32GetSize)
    {
        memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
        if (0 != s32Ret)
        {
            return s32Ret;
        }

        if (NULL == hApcHandle)
        {
            PRINTF_INFO("hApcHandle is NULL !\n");
        }
        else
        {
            s32Ret = IaaApc_Run(hApcHandle, (MI_S16*)(pu8InputBuff));
        }

        if (s32Ret != 0)
        {
            PRINTF_INFO("IaaApc_Run failed.\n");
            return -1;
        }
        PRINTF_DBG("IaaApc_Run succeed.\n");
        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8InputBuff, s32GetSize);
    }

    return s32Ret;
}

#endif
/*AO_SendFrame end*/

#endif

#if 1

/*enable ai Aed start*/
#if 1  //aed
MI_S32 _AI_AedInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = 0;
    ALGO_AED_RET ret = ALGO_AED_RET_SUCCESS;
    AedHandle hAedHandle;
    AedProcessStruct stAedparams;
    unsigned int aedBuffSize;
    char *working_buf_ptr = NULL;
    MI_AUDIO_SampleRate_e eSampleRate;
    AedSensitivity eSensitivity = AED_SEN_HIGH;
    MI_AI_AedConfig_t *pstAiAedConfig = NULL;
    pstAiAedConfig = &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAedConfig;
    MI_S32 s32OperatingPoint;
    MI_S32 s32VadThresholdDb;
    MI_S32 s32LsdThresholdDb;

    stAedparams.channel = 1;
    eSampleRate = (AedSampleRate)_gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;

    switch (eSampleRate)
    {
        case E_MI_AUDIO_SAMPLE_RATE_8000:
            stAedparams.point_number = 256;
            break;
        case E_MI_AUDIO_SAMPLE_RATE_16000:
            stAedparams.point_number = 512;
            break;
        case E_MI_AUDIO_SAMPLE_RATE_32000:
            stAedparams.point_number = 1024;
            break;
        case E_MI_AUDIO_SAMPLE_RATE_48000:
            stAedparams.point_number = 1535;
            break;
        default:
            PRINTF_INFO("Unsupported current sample rate !\n");
            return -1;
    }
    //(1)IaaAed_GetBufferSize
    aedBuffSize = IaaAed_GetBufferSize();
    working_buf_ptr = (char *)malloc(aedBuffSize);
    if(NULL == working_buf_ptr)
    {
        PRINTF_INFO("AED malloc workingBuffer failed !\n");
        return -1;
    }
    PRINTF_INFO("AED malloc workingBuffer succeed ,aedBuffSize = %d\n", aedBuffSize);

    //(2)IaaAed_Init
    hAedHandle = IaaAed_Init(working_buf_ptr, &stAedparams);
    if(NULL == hAedHandle)
    {
        PRINTF_INFO("IaaAed_Init faild !\n");
        return -1;
    }
    PRINTF_INFO("IaaAed_Init succeed.   stAedparams.point_number = %d\n", stAedparams.point_number);
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAedHandle = hAedHandle;

    //(3)IaaAed_Config
    ret = IaaAed_Config(hAedHandle);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_Config failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("IaaAed_Config succeed !\n");
    eSensitivity = pstAiAedConfig->stBabyCryConfig.eSensitivity;
    //(4)IaaAed_SetSensitivity
    ret = IaaAed_SetSensitivity(hAedHandle, eSensitivity);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_SetSensitivity failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("succeed ,eSensitivity = %d\n", eSensitivity);
    //(5)IaaAed_SetSampleRate
    ret = IaaAed_SetSampleRate(hAedHandle, eSampleRate);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_SetSampleRate failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("succeed ,eSampleRate = %d\n", eSampleRate);

    s32OperatingPoint = pstAiAedConfig->stBabyCryConfig.s32OperatingPoint;
    //(6)IaaAed_SetOperatingPoint
    ret = IaaAed_SetOperatingPoint(hAedHandle, s32OperatingPoint);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_SetOperatingPoint failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("succeed ,s32OperatingPoint = %d\n", s32OperatingPoint);

    s32LsdThresholdDb = pstAiAedConfig->stLsdConfig.s32LsdThresholdDb;
    //(7)IaaAed_SetLsdThreshold
    ret = IaaAed_SetLsdThreshold(hAedHandle, s32LsdThresholdDb);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_SetLsdThreshold failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("succeed ,s32LsdThresholdDb = %d\n", s32LsdThresholdDb);

    s32VadThresholdDb = pstAiAedConfig->stBabyCryConfig.s32VadThresholdDb;
    //(8)IaaAed_SetVadThreshold
    ret = IaaAed_SetVadThreshold(hAedHandle, s32VadThresholdDb);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        PRINTF_INFO("IaaAed_SetVadThreshold failed !, ret = %d\n", ret);
        return -1;
    }
    PRINTF_INFO("succeed ,s32VadThresholdDb = %d\n", s32VadThresholdDb);
    PRINTF_INFO("end ,Waiting run ...\n");

    return s32Ret;
}

MI_S32 _AI_DoAed(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, _AUDIO_QueueInfo_t *pstInputQueue)
{
#if 1  //_AI_DoAed content
    MI_S32  s32Ret = MI_SUCCESS;
    MI_AUDIO_SampleRate_e eSampleRate;
    MI_AUDIO_SoundMode_e eSoundMode;
    MI_U32  u32ChnNum = 0;
    MI_U8*  pu8InputBuff = NULL;
    AiDevId = 0;
    AiChn = 0;

    MI_U32  u32GetSize = 0;
    MI_U32  u32BitwidthByte = 2;//change
    int lsd_db = 0; //useless param.
    MI_AUDIO_BitWidth_e eBitWidth;
    AedHandle hAedHandle = NULL;
    MI_BOOL bEnableBabyCryDetect, bTempEnableBCDetect;
    MI_BOOL bEnableLoudSoundDetect, bTempEnableLSDetect;

    AUDIO_CHECK_POINTER(pstInputQueue);
    // check sample rate
    eSampleRate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;

    // check sound mode
    eSoundMode = _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode;
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, eSoundMode);

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);

    if (E_MI_AUDIO_SAMPLE_RATE_8000 == eSampleRate)
    {
        u32GetSize = u32BitwidthByte * 256 * u32ChnNum;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_16000 == eSampleRate)
    {
        u32GetSize = u32BitwidthByte * 512 * u32ChnNum;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_32000 == eSampleRate)
    {
        u32GetSize = u32BitwidthByte * 1024 * u32ChnNum;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_48000 == eSampleRate)
    {
        u32GetSize = u32BitwidthByte * 1536 * u32ChnNum;
    }

    bEnableBabyCryDetect = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAedConfig.bEnableBabyCry;
    bEnableLoudSoundDetect = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAedConfig.bEnableLsd;

    hAedHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAedHandle;
    if (NULL == hAedHandle)
    {
        PRINTF_INFO("Aed handle is NULL !!!\n");
        return -1;
    }

    while(pstInputQueue->s32Size >= u32GetSize)
    {
        memset(pu8InputBuff, 0x0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, u32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueDraw!!!\n");
            return s32Ret;
        }
        if (bEnableLoudSoundDetect)
        {
            //PRINTF_DBG("LoudSound is enable!\n");
            s32Ret = IaaAed_RunLsd(hAedHandle, (MI_S16*)pu8InputBuff, &lsd_db);
            if(ALGO_AED_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaAed_RunLsd failed !,s32Ret = %d\n", s32Ret);
                return -1;
            }
            s32Ret = IaaAed_GetLsdResult(hAedHandle, &bTempEnableLSDetect);
            if(ALGO_AED_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaAed_GetLsdResult failed !,s32Ret = %d\n", s32Ret);
                return -1;
            }
            if(bTempEnableLSDetect)
            {
                //printf("LoudSound:%d    \n",bTempEnableLSDetect);
            }
            if(ALGO_AED_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaAed_GetLsdResult failed !,s32Ret = %d\n", s32Ret);
            }
        }

        if (bEnableBabyCryDetect)
        {
            s32Ret = IaaAed_Run(
                hAedHandle, (MI_S16*)pu8InputBuff);
            if(ALGO_AED_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaAed_Run failed !,s32Ret = %d\n", s32Ret);
            }
            s32Ret = IaaAed_GetResult(hAedHandle, &bTempEnableBCDetect);
            if(0 != s32Ret)
            {
                PRINTF_INFO("IaaAed_GetResult failed !,s32Ret = %d\n", s32Ret);
            }
            if(bTempEnableBCDetect)
            {
                //printf("BabyCry:%d    \n",bTempEnableBCDetect);
            }
            if(ALGO_AED_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaAed_GetResult failed !,s32Ret = %d\n", s32Ret);
            }
        }

    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAcousticEventDetected = bTempEnableBCDetect;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bLoudSoundDetected = bTempEnableLSDetect;

    return s32Ret;

#endif   //_AI_DoAed content
}

MI_S32 AI_SetAedAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedConfig_t *pstAedConfig)
{
#if 1 //content

    memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAedConfig,
           pstAedConfig, sizeof(MI_AI_AedConfig_t));

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAedAttrSet = TRUE;

    return 0;
#endif //content
}

MI_S32 AI_GetAedAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedConfig_t *pstAedConfig)
{
    memcpy(pstAedConfig, &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAedConfig, sizeof(MI_AI_AedConfig_t));
    return 0;
}

MI_S32 AI_EnableAed(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1   //content
    MI_S32 s32Ret;
    MI_U32 u32NumPerFrm;
    MI_U32 u32BitWidthByte = 2;
    MI_U16 u16ChanlNum;

    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;
    PRINTF_INFO("[AED]u32NumPerFrm = %d\n", u32NumPerFrm);

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    PRINTF_INFO("[AED]u16ChanlNum = %d\n", u16ChanlNum);
    s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAedQueue,
                              u32NumPerFrm * u32BitWidthByte * u16ChanlNum *8);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d failed to call _AUDIO_QueueInit!!!\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("[AED]_AUDIO_QueueInit succeed, size = %d\n", u32NumPerFrm * u32BitWidthByte * u16ChanlNum *8);

    s32Ret = _AI_AedInit(AiDevId, AiChn);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d failed to call _AI_AedInit!!!\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("[AED]_AI_AedInit succeed !\n");

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAedEnable = TRUE;

    return MI_SUCCESS;

#endif   //content
}

MI_S32 AI_DisableAed(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1  //content
    _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAedQueue);

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAedHandle)
    {

        IaaAed_Release(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAedHandle);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAedHandle = NULL;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAedEnable = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAedAttrSet = FALSE;

    return 0;

#endif  //content
}

MI_S32 AI_GetAedResult(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AedResult_t *pstAedResult)
{
    pstAedResult->bAcousticEventDetected = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAcousticEventDetected;
    pstAedResult->bLoudSoundDetected = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bLoudSoundDetected;

    return 0;
}

#endif  //aed
/*enable ai Aed end*/

/*enable ai ssl start*/
#if 1  //ssl
static MI_S32 _AI_SslInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1  //content
    MI_S32 s32Ret = 0;
    MI_U32 u32BufferSize;
    AudioSslInit stAudioSslInit;
    AudioSslConfig stAudioSslConfig;
    MI_AUDIO_SampleRate_e eSampleRate;

    eSampleRate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;
    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.bBfMode)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeEnable)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnableForVqe))
    {
        eSampleRate = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate;
    }
    PRINTF_INFO("[SSL]eSampleRate = %d\n", eSampleRate);

    //(1)IaaSsl_GetBufferSize
    u32BufferSize = IaaSsl_GetBufferSize();
    if (0 == u32BufferSize)
    {
        PRINTF_INFO("Ssl buffer size is 0 !!!\n");
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf = (void *)malloc(u32BufferSize);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf)
    {
        PRINTF_INFO("Malloc IaaSsl_GetBufferSize failed.\n");
        return -1;
    }
    PRINTF_INFO("Malloc IaaSsl_GetBufferSize succeed, size = %d\n", u32BufferSize);
    memset(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf, 0x0, u32BufferSize);

    memset(&stAudioSslInit, 0x0, sizeof(stAudioSslInit));
    memset(&stAudioSslConfig, 0x0, sizeof(stAudioSslConfig));

    stAudioSslInit.point_number = 128;
    stAudioSslInit.sample_rate = eSampleRate;
    stAudioSslInit.mic_distance = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.u32MicDistance;
    stAudioSslInit.bf_mode = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.bBfMode;

    //(2)IaaSsl_Init
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle = IaaSsl_Init(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf,
            &stAudioSslInit);

    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle)
    {
        PRINTF_INFO("IaaSsl_Init failed.\n");
        return -1;
    }
    PRINTF_INFO("[SSL]IaaSsl_Init succeed.\n        mic_distance = %d, bf_mode = %d\n", stAudioSslInit.mic_distance, stAudioSslInit.bf_mode);

    stAudioSslConfig.temperature = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32Temperature;
    stAudioSslConfig.noise_gate_dbfs = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32NoiseGateDbfs;
    stAudioSslConfig.direction_frame_num = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32DirectionFrameNum;

    //(3)IaaSsl_Config
    s32Ret = IaaSsl_Config(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle,
            &stAudioSslConfig);
    if (ALGO_SSL_RET_SUCCESS != s32Ret)
    {
        PRINTF_INFO("IaaSsl_Config failed.\n");
        return -1;
    }
    PRINTF_INFO("[SSL]IaaSsl_Config succeed.\n        temperature = %d, noise_gate_dbfs = %d, direction_frame_num = %d\n", \
        stAudioSslConfig.temperature, stAudioSslConfig.noise_gate_dbfs, stAudioSslConfig.direction_frame_num);
    PRINTF_INFO("[SSL] end, waiting run...\n");

    return 0;

#endif //content
}

MI_S32 _AI_DoSsl(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue)
{
#if 1  //content
    //stAudioSslInit.bf_mode = 0; not set
    MI_S32  s32Ret = 0;
    SSL_HANDLE hSslHandle = NULL;
    MI_U32  u32GetSize = 0;
    MI_U32  u32BitwidthByte = 2;
    MI_U8*  pu8InputBuff = NULL;
    MI_S32 delay_sample = 0;
    MI_S32  s32doa;

    MI_U32  u32ChnNum = 0;
    AudioSslInit stAudioSslInit;
    AudioSslConfig stAudioSslConfig;

    if (E_MI_AUDIO_SOUND_MODE_STEREO != _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        PRINTF_INFO("[%s,%d]SSL only support stereo mode!!!\n", __func__, __LINE__);
        return -1;
    }

    if (FALSE != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.bBfMode)
    {
        PRINTF_INFO("SSL bBfMode must be FALSE when disable Bf!!!\n");
        return -1;
    }

    memset(&stAudioSslInit, 0x0, sizeof(stAudioSslInit));
    stAudioSslInit.point_number = AUDIO_ALGORITHM_SAMPLES_UNIT;
    stAudioSslInit.sample_rate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;
    stAudioSslInit.mic_distance = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.u32MicDistance;
    memset(&stAudioSslConfig, 0x0, sizeof(stAudioSslConfig));
    stAudioSslConfig.direction_frame_num = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32DirectionFrameNum;
    stAudioSslConfig.noise_gate_dbfs = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32NoiseGateDbfs;
    stAudioSslConfig.temperature = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32Temperature;

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32GetSize = u32BitwidthByte * u32ChnNum * AUDIO_ALGORITHM_SAMPLES_UNIT;

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);

    hSslHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle;
    if (NULL == hSslHandle)
    {
        PRINTF_INFO("SSL handle is NULL!!!\n");
        return -1;
    }

    while (pstInputQueue->s32Size >= u32GetSize)
    {
        memset(pu8InputBuff, 0x0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, u32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueDraw!!!\n");
            return s32Ret;
        }

        //(4)IaaSsl_Run
        s32Ret = IaaSsl_Run(hSslHandle, (MI_S16*)pu8InputBuff, &delay_sample);
        if (ALGO_SSL_RET_SUCCESS == s32Ret)
        {
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].u32SslFrameIndex++;
        }
        else
        {
            PRINTF_INFO("Dev%d Chn%d failed to call IaaSsl_Run!!!\n", AiDevId, AiChn);
            return -1;
        }

        if (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].u32SslFrameIndex
                == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr.s32DirectionFrameNum)
        {
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].u32SslFrameIndex = 0;

            //(5)IaaSsl_Get_Direction
            s32Ret = IaaSsl_Get_Direction(hSslHandle, &s32doa);

            PRINTF_INFO("s32doa:%d!!!\n", s32doa);
            if (-32767 != s32doa)
            {
                if (-112 != s32doa)
                {
                    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].s32SslDoa = s32doa;
                }
            }
            else
            {
                PRINTF_INFO("IaaSsl_Get_Direction failed !!!\n");
                return -1;
            }

            hSslHandle = IaaSsl_Reset(
                    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf,
                    &stAudioSslInit);
            if (NULL == hSslHandle)
            {
                PRINTF_INFO("IaaSsl_Reset failed !!!\n");
                return -1;
            }

            s32Ret = IaaSsl_Config(
                    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle,
                    &stAudioSslConfig);
            if (ALGO_SSL_RET_SUCCESS != s32Ret)
            {
                PRINTF_INFO("IaaSsl_Config failed !!!\n");
                return -1;
            }
        }
    }

    return 0;

#endif
}

MI_S32 AI_SetSslInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslInitAttr_t* pstSslInitAttr)
{
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslInitAttrSet = TRUE;
    memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr, pstSslInitAttr, sizeof(MI_AI_SslInitAttr_t));

    return 0;
}

MI_S32 AI_GetSslInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslInitAttr_t* pstSslInitAttr)
{

    memcpy(pstSslInitAttr, &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr, sizeof(MI_AI_SslInitAttr_t));

    return 0;
}

MI_S32 AI_SetSslConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslConfigAttr_t* pstSslConfigAttr)
{
#if 1

    if ((0 != pstSslConfigAttr->s32DirectionFrameNum % 50) || (0 == pstSslConfigAttr->s32DirectionFrameNum))
    {
        PRINTF_INFO("SSL: illeagl direction frame num!!!\n");
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslConfigAttrSet = TRUE;
    memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr, pstSslConfigAttr, sizeof(MI_AI_SslConfigAttr_t));

    return 0;

#endif
}

MI_S32 AI_GetSslConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_SslConfigAttr_t* pstSslConfigAttr)
{
    memcpy(pstSslConfigAttr, &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslConfigAttr, sizeof(MI_AI_SslConfigAttr_t));

    return 0;
}

MI_S32 AI_EnableSsl(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1
    MI_S32 s32Ret = 0;
    MI_U16 u16ChanlNum;
    MI_U32 u32NumPerFrm;
    MI_BOOL bInitSuccess = TRUE;
    MI_U32 u32BitWidthByte = 2;

    // check status
    if (E_MI_AUDIO_SOUND_MODE_STEREO != _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        PRINTF_INFO("SSL: only support stereo sound mode!!!\n");
        return -1;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;
    PRINTF_INFO("[SSL]u16ChanlNum = %d, u32NumPerFrm = %d\n", u16ChanlNum, u32NumPerFrm);
    do
    {
        s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslQueue,
                                  u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8);
        if (MI_SUCCESS != s32Ret)
        {
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("[SSL]_AUDIO_QueueInit succeed, size = %d\n", u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8);\

        s32Ret = _AI_SslInit(AiDevId, AiChn);
        if (MI_SUCCESS != s32Ret)
        {
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("[SSL]_AI_SslInit succeed !\n");
    }
    while(0);

    if (FALSE == bInitSuccess)
    {
        _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslQueue);
        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle)
        {
            s32Ret = IaaSsl_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle);
            if (0 != s32Ret)
            {
                PRINTF_INFO("IaaSsl_Free failed!!!\n");
                return -1;
            }
            else
            {
                _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle = NULL;
            }
        }

        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf);
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf = NULL;
        }

        return -1;
    }
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslEnable = TRUE;

    return 0;

#endif
}

MI_S32 AI_DisableSsl(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1
    MI_S32 s32Ret = 0;

    _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslQueue);
    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle)
    {
        s32Ret = IaaSsl_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle);
        if (0 != s32Ret)
        {
            PRINTF_INFO("IaaSsl_Free failed!!!\n");
            return -1;
        }
        else
        {
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle = NULL;
        }
    }

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf)
    {
        free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSslWorkingBuf = NULL;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslEnable = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslInitAttrSet = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslConfigAttrSet = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].u32SslFrameIndex = 0;

    return 0;

#endif
}

MI_S32 AI_GetSslDoa(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 *ps32SslDoa)
{
    *ps32SslDoa = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].s32SslDoa;

    return 0;
}

#endif //ssl
/*enable ai ssl end*/

/*enable ai Bf start*/
#if 1  //bf
static MI_S32 _AI_BfInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1
    MI_S32 s32Ret = 0;
    MI_U32 u32BufferSize;
    AudioBfInit stAudioBfInit;
    AudioBfConfig stAudioBfConfig;
    MI_AUDIO_SampleRate_e eSampleRate;
    MI_U32 u32ChnNum;

    eSampleRate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;
    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeEnable)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnableForVqe))
    {
        eSampleRate = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate;
    }
    PRINTF_INFO("[BF]eSampleRate = %d\n", eSampleRate);

    //(1)IaaBf_GetBufferSize
    u32BufferSize = IaaBf_GetBufferSize();
    if (0 == u32BufferSize)
    {
        PRINTF_INFO("Bf buffer size is 0 !!!\n");
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf = (void *)malloc(u32BufferSize);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf)
    {
        PRINTF_INFO("Malloc IaaBf_GetBufferSize failed\n");
        return -1;
    }
    PRINTF_INFO("[BF]Malloc Bf buffer succeed, size = %d.\n", u32BufferSize);

    memset(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf, 0x0, u32BufferSize);

    memset(&stAudioBfInit, 0x0, sizeof(stAudioBfInit));
    memset(&stAudioBfConfig, 0x0, sizeof(stAudioBfConfig));

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    stAudioBfInit.point_number = 128;
    stAudioBfInit.sample_rate = eSampleRate;
    stAudioBfInit.mic_distance = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfInitAttr.u32MicDistance;
    stAudioBfInit.channel = u32ChnNum;

    //(2)IaaBf_Init
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle = IaaBf_Init(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf,
            &stAudioBfInit);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle)
    {
        PRINTF_INFO("IaaBf_Init failed channel = %d err, Continue to IaaBf_Init ...\n", u32ChnNum);
        if(4 != stAudioBfInit.channel)
        {
            stAudioBfInit.channel = 4;  //Test:This library only support 4 Microphones!!
            PRINTF_INFO("Auto modify stAudioBfInit.channel = 4 !!!\n");
        }
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle = IaaBf_Init(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf,
            &stAudioBfInit);
        if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle)
        {
            PRINTF_INFO("IaaBf_Init failed channel = %d err.\n", u32ChnNum);
            return -1;
        }
    }
    PRINTF_INFO("[BF]IaaBf_Init succeed.\n \
        mic_distance = %d, channel = %d\n", stAudioBfInit.mic_distance, stAudioBfInit.channel);

    stAudioBfConfig.noise_estimation = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr.s32NoiseEstimation;
    stAudioBfConfig.noise_gate_dbfs = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr.s32NoiseGateDbfs;
    stAudioBfConfig.noise_supression_mode = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr.s32NoiseSupressionMode;
    stAudioBfConfig.output_gain = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr.outputGain;
    stAudioBfConfig.temperature = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr.s32Temperature;
    stAudioBfConfig.vad_enable = 0;
    //(3)IaaBf_Config
    s32Ret = IaaBf_Config(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle,
            &stAudioBfConfig);
    if (ALGO_BF_RET_SUCCESS != s32Ret)
    {
        PRINTF_INFO("IaaBf_Config failed.\n");
        return -1;
    }
    PRINTF_INFO("[BF]IaaBf_Config succeed.\n \
        noise_estimation = %d, noise_gate_dbfs = %d, noise_supression_mode = %d \n \
        output_gain = %f, temperature = %d\n", stAudioBfConfig.noise_estimation, stAudioBfConfig.noise_gate_dbfs, \
        stAudioBfConfig.noise_supression_mode, stAudioBfConfig.output_gain, stAudioBfConfig.temperature);

    PRINTF_INFO("\n[BF]end ,Waiting run ...\n");
    return 0;

#endif
}

MI_S32 _AI_DoBf(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue)
{
#if 1
    MI_S32  s32Ret = 0;
    MI_U32  u32BfDoaMode;   // 0 -- use Bf doa, 1 -- ssl, 2 -- default
    MI_U32  u32ChnNum = 0;
    MI_U8*  pu8InputBuff = NULL;
    MI_U32  u32GetSize = 0;
    MI_U32  u32BitwidthByte = 2;
    SSL_HANDLE hSslHandle = NULL;
    BF_HANDLE hBfHandle = NULL;
    MI_S32  s32doa;

    if (E_MI_AUDIO_SOUND_MODE_STEREO != _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        PRINTF_INFO("SSL and BF only support stereo mode!!!\n");
        return -1;
    }

    if (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSetBfDoa)
    {
        u32BfDoaMode = 0;
    }
    else if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSslEnable)
        && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSslInitAttr.bBfMode))
    {
        u32BfDoaMode = 1;
    }
    else
    {
        u32BfDoaMode = 2;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);

    u32GetSize = u32BitwidthByte * u32ChnNum * AUDIO_ALGORITHM_SAMPLES_UNIT;

    if (1 == u32BfDoaMode)
    {
        hSslHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSslHandle;
        if (NULL == hSslHandle)
        {
            PRINTF_INFO("SSL handle is NULL!!!\n");
            return -1;
        }
    }

    hBfHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle;
    if (NULL == hBfHandle)
    {
        PRINTF_INFO("BF handle is NULL!!!\n");
        return -1;
    }

    while(pstInputQueue->s32Size >= u32GetSize)
    {
        memset(pu8InputBuff, 0x0, 1024*25);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, u32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueDraw!!!\n");
            return -1;
        }
        if (0 == u32BfDoaMode)
        {
            s32doa = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].s32BfDoa;
        }
        else if (1 == u32BfDoaMode)
        {
            PRINTF_INFO("[%s,%d]Dev%d Chn%d not support ssl s32doa!!!\n [%s,%d]now use BF set s32doa!!!\n",__func__, __LINE__, AiDevId, AiChn,__func__, __LINE__);
        }
        else
        {
            s32doa = 0;
        }

        s32Ret = IaaBf_Run(hBfHandle, (MI_S16*)pu8InputBuff, &s32doa);
        if (0 != s32Ret)
        {
            PRINTF_INFO("IaaBf_Run FAIL !!!!!!!!!!!!!!!\n");
            return -1;
        }

        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8InputBuff, u32GetSize / 2);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueInsert!!!\n");
            return s32Ret;
        }
    }

    return 0;

#endif
}

MI_S32 AI_SetBfInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfInitAttr_t* pstBfInitAttr)
{
    if (0 == pstBfInitAttr->u32MicDistance)
    {
        PRINTF_INFO("BF: Dev%d Chn%d mic distance is illegal!!!\n", AiDevId, AiChn);
        return -1;
    }

    if (2 != pstBfInitAttr->u32ChanCnt)
    {
        PRINTF_INFO("BF: Dev%d Chn%d channel count is illegal!!!\n", AiDevId, AiChn);
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bBfInitAttrSet = TRUE;
    memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfInitAttr, pstBfInitAttr, sizeof(MI_AI_BfInitAttr_t));

    return 0;
}

MI_S32 AI_GetBfInitAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfInitAttr_t* pstBfInitAttr)
{
    memcpy(pstBfInitAttr, &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfInitAttr, sizeof(MI_AI_BfInitAttr_t));

    return 0;
}

MI_S32 AI_SetBfConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfConfigAttr_t* pstBfConfigAttr)
{
    if ((pstBfConfigAttr->s32NoiseSupressionMode < 0) || (pstBfConfigAttr->s32NoiseSupressionMode > 15))
    {
        PRINTF_INFO("BF: Dev%d Chn%d noise supression mode is illegal!!! It must be (0 ~ 15).\n", AiDevId, AiChn);
        return -1;
    }

    if ((0 != pstBfConfigAttr->s32NoiseEstimation) && (1 != pstBfConfigAttr->s32NoiseEstimation))
    {
        PRINTF_INFO("BF: Dev%d Chn%d noise estimation is illegal!!! It must be (0 ~ 1).\n", AiDevId, AiChn);
        return -1;
    }

    if ((pstBfConfigAttr->outputGain < 0.0) || (pstBfConfigAttr->outputGain > 1.0))
    {
        PRINTF_INFO("BF: Dev%d Chn%d output gain is illegal!!! It must be (0.0 ~ 1.0).\n", AiDevId, AiChn);
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bBfConfigAttrSet = TRUE;
    memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr, pstBfConfigAttr, sizeof(MI_AI_BfConfigAttr_t));

    return 0;
}

MI_S32 AI_GetBfConfigAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_BfConfigAttr_t* pstBfConfigAttr)
{
    memcpy(pstBfConfigAttr,&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfConfigAttr,sizeof(MI_AI_BfConfigAttr_t));

    return 0;
}

MI_S32 AI_EnableBf(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = 0;
    MI_U32 u32NumPerFrm;
    MI_U32 u32BitWidthByte = 2;
    MI_U16 u16ChanlNum;
    MI_BOOL bInitSuccess = TRUE;

    if (E_MI_AUDIO_SOUND_MODE_STEREO != _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        PRINTF_INFO("Bf: only support stereo sound mode!!!\n");
        return -1;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;
    PRINTF_INFO("[BF]u16ChanlNum = %d, u32NumPerFrm = %d\n", u16ChanlNum, u32NumPerFrm);
    //return 0;
    do
    {
        s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfQueue,
                                  u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("[BF]_AUDIO_QueueInit failed.\n");
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("[BF]_AUDIO_QueueInit succeed, size = %d\n", u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8);

        s32Ret = _AI_BfInit(AiDevId, AiChn);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("[BF]_AI_BfInit failed.\n");
            bInitSuccess = FALSE;
            break;
        }
        PRINTF_INFO("\n[BF]_AI_BfInit succeed.\n");
    }
    while(0);

    if (FALSE == bInitSuccess)
    {
        _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfQueue);
        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle)
        {
            s32Ret = IaaBf_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle);
            if (0 != s32Ret)
            {
                PRINTF_INFO("IaaBf_Free failed !, s32Ret = %d\n", s32Ret);
                return -1;
            }
            else
            {
                _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle = NULL;
            }
        }

        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf);
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf = NULL;
        }

        return -1;
    }

    return 0;
}

MI_S32 AI_DisableBf(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = 0;

    _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stBfQueue);
    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle)
    {
        s32Ret = IaaBf_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle);
        if (0 != s32Ret)
        {
            PRINTF_INFO("IaaBf_Free failed !, ret = %d\n", s32Ret);
            return -1;
        }
        else
        {
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hBfHandle = NULL;
        }
    }

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf)
    {
        free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pBfWorkingBuf = NULL;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAiEnableBf = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bBfInitAttrSet = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bBfConfigAttrSet = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSetBfDoa = FALSE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].s32BfDoa = 0;

    return 0;
}

MI_S32 AI_SetBfAngle(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 s32BfAngle)
{
    if (-180 > s32BfAngle || s32BfAngle > 180)
    {
        PRINTF_INFO("s32BfDoa[%d] only supports -180~180 currently.\n", s32BfAngle);
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bSetBfDoa = TRUE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].s32BfDoa = s32BfAngle;

    return 0 ;
}

#endif
/*enable ai Bf end*/

/*enable ai vqe start*/
#if 1

static MI_S32 _AI_SrcInitForVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_S32 s32WorkSampleRate)
{

    return 0;
}

static MI_S32 _AI_ApcConfig(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_VqeConfig_t *pstVqeConfig)
{
    MI_S32 s32Ret = MI_SUCCESS;
#if 1
    AudioAnrConfig stAnrInfo, *pstAnrInfo = NULL;
    AudioEqConfig  stEqInfo, *pstEqInfo = NULL;
    AudioHpfConfig stHpfInfo, *pstHpfInfo = NULL;
    AudioAgcConfig stAgcInfo, *pstAgcInfo = NULL;

    //set Anr
    if (TRUE == pstVqeConfig->bAnrOpen)
    {
        memset(&stAnrInfo, 0, sizeof(AudioAnrConfig));

        stAnrInfo.anr_enable = (MI_U32)pstVqeConfig->bAnrOpen;
        stAnrInfo.user_mode = (MI_U32)pstVqeConfig->stAnrCfg.eMode;

        AUDIO_VQE_NR_SPEED_TRANS_TYPE(pstVqeConfig->stAnrCfg.eNrSpeed, stAnrInfo.anr_converge_speed);
        memcpy(stAnrInfo.anr_intensity_band,
            pstVqeConfig->stAnrCfg.u32NrIntensityBand,
            sizeof(pstVqeConfig->stAnrCfg.u32NrIntensityBand));
        memcpy(stAnrInfo.anr_intensity,
            pstVqeConfig->stAnrCfg.u32NrIntensity,
            sizeof(pstVqeConfig->stAnrCfg.u32NrIntensity));
        stAnrInfo.anr_smooth_level = pstVqeConfig->stAnrCfg.u32NrSmoothLevel;
        pstAnrInfo = &stAnrInfo;
    }

    //set Eq
    if (TRUE == pstVqeConfig->bEqOpen)
    {
        memset(&stEqInfo, 0, sizeof(AudioEqConfig));
        stEqInfo.eq_enable = (MI_U32)pstVqeConfig->bEqOpen;
        stEqInfo.user_mode = (MI_U32)pstVqeConfig->stEqCfg.eMode;
        memcpy(stEqInfo.eq_gain_db, pstVqeConfig->stEqCfg.s16EqGainDb, sizeof(pstVqeConfig->stEqCfg.s16EqGainDb));
        pstEqInfo = &stEqInfo;
    }

    //set Hpf
    if (TRUE == pstVqeConfig->bHpfOpen)
    {
        memset(&stHpfInfo, 0, sizeof(AudioHpfConfig));
        stHpfInfo.hpf_enable = (MI_U32)pstVqeConfig->bHpfOpen;
        stHpfInfo.user_mode = (MI_U32)pstVqeConfig->stHpfCfg.eMode;
        AUDIO_VQE_HPF_TRANS_TYPE(pstVqeConfig->stHpfCfg.eHpfFreq, stHpfInfo.cutoff_frequency);
        pstHpfInfo = &stHpfInfo;
    }

    //set Agc
    if (TRUE == pstVqeConfig->bAgcOpen)
    {
        memset(&stAgcInfo, 0, sizeof(AudioAgcConfig));
        stAgcInfo.agc_enable = (MI_U32)pstVqeConfig->bAgcOpen;
        stAgcInfo.user_mode = (MI_U32)pstVqeConfig->stAgcCfg.eMode;
        stAgcInfo.attack_time = pstVqeConfig->stAgcCfg.u32AttackTime;
        stAgcInfo.release_time = pstVqeConfig->stAgcCfg.u32ReleaseTime;
        memcpy(stAgcInfo.compression_ratio_input, pstVqeConfig->stAgcCfg.s16Compression_ratio_input,
               sizeof(pstVqeConfig->stAgcCfg.s16Compression_ratio_input));
        memcpy(stAgcInfo.compression_ratio_output, pstVqeConfig->stAgcCfg.s16Compression_ratio_output,
               sizeof(pstVqeConfig->stAgcCfg.s16Compression_ratio_output));
        stAgcInfo.drop_gain_max = pstVqeConfig->stAgcCfg.u32DropGainMax;
        stAgcInfo.gain_info.gain_init = pstVqeConfig->stAgcCfg.stAgcGainInfo.s32GainInit;
        stAgcInfo.gain_info.gain_max = pstVqeConfig->stAgcCfg.stAgcGainInfo.s32GainMax;
        stAgcInfo.gain_info.gain_min = pstVqeConfig->stAgcCfg.stAgcGainInfo.s32GainMin;
        stAgcInfo.noise_gate_attenuation_db = pstVqeConfig->stAgcCfg.u32NoiseGateAttenuationDb;
        stAgcInfo.noise_gate_db = pstVqeConfig->stAgcCfg.s32NoiseGateDb;
        stAgcInfo.drop_gain_threshold = pstVqeConfig->stAgcCfg.s32DropGainThreshold;
        pstAgcInfo = &stAgcInfo;
    }

    /* APC Config */
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle)
    {
        PRINTF_INFO("Dev%d Chn%d hApcHandle is NULL!!!!!\n", AiDevId, AiChn);
        return -1;
    }

    s32Ret = IaaApc_Config(
                 _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle, pstAnrInfo,
                 pstEqInfo, pstHpfInfo, NULL, NULL, pstAgcInfo);
    if (0 != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d IaaApc_config failed!!!\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("Dev%d Chn%d IaaApc_config succeed!\n", AiDevId, AiChn);
    PRINTF_INFO("APC config end, Waiting run...\n\n");
#endif

    return s32Ret;
}

static MI_S32 _AI_ApcInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_VqeConfig_t *pstVqeConfig)
{
    MI_S32 s32Ret = 0;
    AudioProcessInit stApstruct;
    MI_AUDIO_SampleRate_e eSampleRate;
    AudioApcBufferConfig stApcBufCfg;
    MI_U32 u32BufSize;

    AUDIO_CHECK_POINTER(pstVqeConfig);

    eSampleRate = pstVqeConfig->s32WorkSampleRate;
    if (E_MI_AUDIO_SAMPLE_RATE_32000 == eSampleRate)
    {
        PRINTF_INFO("Dev%d Chn%d Vqe not support 32K!!!\n", AiDevId, AiChn);
        return -1;
    }

    stApstruct.point_number = AUDIO_VQE_SAMPLES_UNIT; //APC demo is 128
    stApstruct.channel = pstVqeConfig->u32ChnNum;
    PRINTF_INFO("[APC]point_number = %d, channel = %d\n", stApstruct.point_number, stApstruct.channel);
    AUDIO_VQE_SAMPLERATE_TRANS_TYPE(eSampleRate, stApstruct.sample_rate);

    /* APC Buffer Config Init*/
    stApcBufCfg.agc_enable = TRUE;
    stApcBufCfg.anr_enable = TRUE;
    stApcBufCfg.dr_enable = 0;
    stApcBufCfg.eq_enable = TRUE;
    stApcBufCfg.vad_enable = 0;

    //(1)IaaApc_GetBufferSize
    u32BufSize = IaaApc_GetBufferSize(&stApcBufCfg);
    if (0 == u32BufSize)
    {
        PRINTF_INFO("Dev%d Chn%d Apc buf size is 0 !!!\n", AiDevId, AiChn);
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf = malloc(u32BufSize);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf)
    {
        PRINTF_INFO("Dev%d Chn%d Malloc IaaApc_GetBuffer failed\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("Dev%d Chn%d Malloc IaaApc_GetBuffer succeed, size = %d\n", AiDevId, AiChn, u32BufSize);

    //(2)IaaApc_Init
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle = IaaApc_Init(
            (char* const)_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf,
            &stApstruct, &stApcBufCfg);

    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle)
    {
        PRINTF_INFO(" Dev%d Chn%d IaaApc_Init failed. !!!\n" , AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO(" Dev%d Chn%d IaaApc_Init succeed, hApcHandle = %p\n" , AiDevId, AiChn, _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle);
    PRINTF_INFO("_AI_ApcInit end,Waiting APC_config...\n\n");

    return 0;
}

static MI_S32 _AI_DoVqe(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue
)
{
#if 1

    MI_S32 s32Ret = 0;
    MI_U32 u32ChnNum;
    MI_U8 *pu8InputBuff = NULL;
    MI_S32 s32GetSize;
    MI_S32 s32BitwidthByte = 2;
    APC_HANDLE hApcHandle = NULL;

    u32ChnNum = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.u32ChnNum;
    s32GetSize = AUDIO_VQE_SAMPLES_UNIT * s32BitwidthByte * u32ChnNum;  //AUDIO_VQE_SAMPLES_UNIT = 128
    if (pstInputQueue->s32Size < s32GetSize)
    {
        PRINTF_INFO("pstInputQueue->s32Size < s32GetSize err.\n");
        return -1;
    }

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);
    hApcHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle;
    AUDIO_CHECK_POINTER(hApcHandle);

    while (pstInputQueue->s32Size >= s32GetSize)
    {
        memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueDraw!!!\n");
            return -1;
        }
        PRINTF_DBG("QueueDraw succeed, Queue size = %d !!!\n", pstInputQueue->s32Size);

        if (NULL == hApcHandle)
        {
            PRINTF_INFO("hApcHandle is NULL!!!.\n");
        }
        else
        {
            s32Ret = IaaApc_Run(hApcHandle, (MI_S16*)(pu8InputBuff));
            if (s32Ret != MI_SUCCESS)
            {
                PRINTF_INFO("IaaApc_Run failed\n");
                return -1;
            }
        }
        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8InputBuff, s32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _AUDIO_QueueInsert!!!\n");
            return -1;
        }
    }

    return 0;

#endif
}

MI_S32 AI_SetVqeAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AI_VqeConfig_t *pstVqeConfig)
{
#if 1

#if 1
    MI_S32 s32Ret = 0;
    MI_U32 u32ChnNum = 0;
    MI_U16 u16ChanlNum;
    MI_U32 u32NumPerFrm;
    MI_U32 u32BitWidthByte = 2;
    MI_U32 u32ChnIdx = 0, u32ChnStart = 0, u32ChnEnd = 0;

    MI_BOOL __attribute__((unused)) bCurApcEnable = FALSE, __attribute__((unused)) bNewApcEnable = FALSE;
    MI_BOOL bApcInitSuccess = TRUE;

    MI_BOOL __attribute__((unused)) bCurAecEnable = FALSE, __attribute__((unused)) bNewAecEnable = FALSE;
    MI_BOOL bAecInitSuccess = TRUE;

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    if ((0 == pstVqeConfig->u32ChnNum) || (pstVqeConfig->u32ChnNum > u32ChnNum))
    {
        PRINTF_INFO("Dev%d Chn%d illegal VQE channel number[%d], sound mode channel number[%d]!!!\n",
                AiDevId, AiDevId, pstVqeConfig->u32ChnNum, u32ChnNum);
        return -1;
    }

    if (E_MI_AUDIO_SAMPLE_RATE_32000 == pstVqeConfig->s32WorkSampleRate)
    {
        PRINTF_INFO("Dev%d Chn%d :Because of Vqe does not support 32K!!!\n", AiDevId, AiChn);
        return -1;
    }

    // check Aec param
    if (TRUE == pstVqeConfig->bAecOpen)
    {
        if (2 == AiDevId)
        {
            PRINTF_INFO("Dev%d Chn%d :Because of I2S(RX) dose not support AEC!!!\n", AiDevId, AiChn);
            return -1;
        }

        if ((E_MI_AUDIO_SAMPLE_RATE_8000 != pstVqeConfig->s32WorkSampleRate)
                && (E_MI_AUDIO_SAMPLE_RATE_16000 != pstVqeConfig->s32WorkSampleRate))
        {
            PRINTF_INFO("Dev%d Chn%d :Because of Aec only supports 8/16K.\n", AiDevId, AiChn);
            return -1;
        }
    }

    if (TRUE == pstVqeConfig->bHpfOpen)
    {
        if ((E_MI_AUDIO_ALGORITHM_MODE_DEFAULT != pstVqeConfig->stHpfCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_USER != pstVqeConfig->stHpfCfg.eMode)           )
        {
            PRINTF_INFO("Dev%d Chn%d Hpf dose not support this mode[%d].\n", AiDevId, AiChn, pstVqeConfig->stHpfCfg.eMode);
            return -1;
        }

        if ((E_MI_AUDIO_HPF_FREQ_80 != pstVqeConfig->stHpfCfg.eHpfFreq)
                && (E_MI_AUDIO_HPF_FREQ_120 != pstVqeConfig->stHpfCfg.eHpfFreq)
                && (E_MI_AUDIO_HPF_FREQ_150 != pstVqeConfig->stHpfCfg.eHpfFreq))
        {
            PRINTF_INFO("Dev%d Chn%d Hpf frequency only supports 80/120/150Hz, current HpfFreq is [%d]Hz.\n",
                    AiDevId, AiChn, pstVqeConfig->stHpfCfg.eHpfFreq);
            return -1;
        }

        if ((E_MI_AUDIO_SAMPLE_RATE_8000 != pstVqeConfig->s32WorkSampleRate)
                && (E_MI_AUDIO_SAMPLE_RATE_16000 != pstVqeConfig->s32WorkSampleRate))
        {
            PRINTF_INFO("Dev%d Chn%d Hpf only supports 8/16K!!!\n", AiDevId, AiChn);
            return -1;
        }
    }

    if (TRUE == pstVqeConfig->bAnrOpen)
    {
        if ((E_MI_AUDIO_ALGORITHM_MODE_DEFAULT != pstVqeConfig->stAnrCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_USER != pstVqeConfig->stAnrCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_MUSIC != pstVqeConfig->stAnrCfg.eMode))
        {
            PRINTF_INFO("Dev%d Chn%d Anr not support this mode[%d].\n", AiDevId, AiChn, pstVqeConfig->stAnrCfg.eMode);
            return -1;
        }

        if ((E_MI_AUDIO_NR_SPEED_LOW != pstVqeConfig->stAnrCfg.eNrSpeed)
                && (E_MI_AUDIO_NR_SPEED_MID != pstVqeConfig->stAnrCfg.eNrSpeed)
                && (E_MI_AUDIO_NR_SPEED_HIGH != pstVqeConfig->stAnrCfg.eNrSpeed))
        {
            PRINTF_INFO("Dev%d Chn%d Anr speed only supports low/mid/high speed, current speed is [%d].\n",
                    AiDevId, AiChn, pstVqeConfig->stAnrCfg.eNrSpeed);
            return -1;
        }
    }

    //check Agc
    if (TRUE == pstVqeConfig->bAgcOpen)
    {
        if ((E_MI_AUDIO_ALGORITHM_MODE_DEFAULT != pstVqeConfig->stAgcCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_USER != pstVqeConfig->stAgcCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_MUSIC != pstVqeConfig->stAgcCfg.eMode))
        {
            PRINTF_INFO("Dev%d Chn%d Agc not support this mode[%d].\n", AiDevId, AiChn, pstVqeConfig->stAgcCfg.eMode);
            return -1;
        }
    }

    //check Eq
    if (TRUE == pstVqeConfig->bEqOpen)
    {
        if ((E_MI_AUDIO_ALGORITHM_MODE_DEFAULT != pstVqeConfig->stEqCfg.eMode)
                && (E_MI_AUDIO_ALGORITHM_MODE_USER != pstVqeConfig->stEqCfg.eMode)
                //&& (E_MI_AUDIO_ALGORITHM_MODE_MUSIC != pstVqeConfig->stEqCfg.eMode)
           )
        {
            PRINTF_INFO("Dev%d Chn%d Eq not support this mode[%d].\n", AiDevId, AiChn, pstVqeConfig->stEqCfg.eMode);
            return -1;
        }
    }

    // only supports frist setting
    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeAttrSet)
            && (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate
                != pstVqeConfig->s32WorkSampleRate))
    {
        PRINTF_INFO("Dev%d Chn%d Vqe does not support to change sample rate.\nYou can reset sample rate after disable \
                Vqe.\n", AiDevId, AiChn);
        return -1;
    }

    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeAttrSet)
            && (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.u32ChnNum
                != pstVqeConfig->u32ChnNum))
    {
        PRINTF_INFO("Dev%d Chn%d Vqe does not support to change channel number.\nYou can reset sample rate after disable \
                Vqe.\n", AiDevId, AiChn);
        return -1;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    // first setting
    if (FALSE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeAttrSet)
    {

#if 0
        // init Vqe Resample
        if (pstVqeConfig->s32WorkSampleRate != _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate)
        {
            for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
            {
                s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stSrcQueueForVqeAiIn,
                                          u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init src queue for Vqe Ai in.\n", AiDevId, u32ChnIdx);
                    goto INIT_SRC_FAIL;
                }

                s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stSrcQueueForVqeAoIn,
                                          u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init src queue for Vqe Ao in.\n", AiDevId, u32ChnIdx);
                    goto INIT_SRC_FAIL;
                }

                s32Ret = _AI_SrcInitForVqe(AiDevId, u32ChnIdx, pstVqeConfig->s32WorkSampleRate);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init src lib for Vqe.\n", AiDevId, u32ChnIdx);
                    goto INIT_SRC_FAIL;
                }
                _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].bResampleEnableForVqe = TRUE;
            }

        }
#endif

        // init Apc lib
        if (pstVqeConfig->bAgcOpen || pstVqeConfig->bAnrOpen || pstVqeConfig->bEqOpen || pstVqeConfig->bHpfOpen)
        {
            for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
            {
                s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stApcQuque,
                                          u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init apc queue.\n", AiDevId, u32ChnIdx);
                    goto INIT_APC_FAIL;
                }
                PRINTF_INFO("Dev%d Chn%d succeed to init apc queue, size = %d\n", AiDevId, u32ChnIdx, u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);

                s32Ret = _AI_ApcInit(AiDevId, u32ChnIdx, pstVqeConfig);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init apc lib.\n", AiDevId, u32ChnIdx);
                    goto INIT_APC_FAIL;
                }
                PRINTF_INFO("APC init end !!!\n");

                s32Ret = _AI_ApcConfig(AiDevId, u32ChnIdx, pstVqeConfig);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to config apc lib.\n", AiDevId, u32ChnIdx);
                    goto INIT_APC_FAIL;
                }
                PRINTF_INFO("APC config end !!!\n");
            }
        }

        // init Aec lib
        if (pstVqeConfig->bAecOpen)
        {
            for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
            {
                s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAecQueue,
                                          u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init aec queue.\n", AiDevId, u32ChnIdx);
                    goto INIT_AEC_FAIL;
                }
                PRINTF_INFO("Dev%d Chn%d succeed to init aec queue, size = %d\n", AiDevId, u32ChnIdx, u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);

                s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAecRefQueue,
                                          u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init aec ref queue.\n", AiDevId, u32ChnIdx);
                    goto INIT_AEC_FAIL;
                }
                PRINTF_INFO("Dev%d Chn%d succeed to init aec ref queue, size = %d\n", AiDevId, u32ChnIdx, u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);

                s32Ret = _AI_AecInit(AiDevId, u32ChnIdx, pstVqeConfig);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to init aec lib.\n", AiDevId, u32ChnIdx);
                    goto INIT_AEC_FAIL;
                }
                PRINTF_INFO("AEC init end\n");

                s32Ret = _AI_AecConfig(AiDevId, u32ChnIdx, pstVqeConfig);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("Dev%d Chn%d failed to config aec lib.\n", AiDevId, u32ChnIdx);
                    goto INIT_AEC_FAIL;
                }
                PRINTF_INFO("AEC config end\n");
            }
        }

        // save vqe attr and set flag
        for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
        {
            memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAiVqeConfig,
                   pstVqeConfig, sizeof(MI_AI_VqeConfig_t));
        }

        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeAttrSet = TRUE;
        return 0;
    }

INIT_AEC_FAIL:
INIT_APC_FAIL:
INIT_SRC_FAIL:

    return -1;

#endif

#endif
}

MI_S32 AI_GetVqeAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn,  MI_AI_VqeConfig_t *pstVqeConfig)
{
    memcpy(pstVqeConfig, &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig, sizeof(MI_AI_VqeConfig_t));

    return 0;
}

MI_S32 AI_EnableVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

    MI_U32 u32ChnIdx = 0, u32ChnStart = 0, u32ChnEnd = 0;

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
    {
        _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].bVqeEnable = TRUE;
    }

    return s32Ret;
}

MI_S32 AI_DisableVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

    MI_U32 u32ChnIdx, u32ChnStart = 0, u32ChnEnd = 0;

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
    {
        if (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate
                != _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate)
        {
#if (defined(_AI_SRC_ENABLE_))
            _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stSrcQueueForVqeAiIn);
            _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stSrcQueueForVqeAoIn);
            _AI_SrcDeinitForVqe(AiDevId, u32ChnIdx);
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnableForVqe = FALSE;
#endif
        }

        if (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.bAgcOpen
                || _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.bAnrOpen
                || _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.bEqOpen
                || _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.bHpfOpen)
        {
#if (defined(_AI_APC_ENABLE_))
            _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stApcQuque);
            _AI_ApcDeinit(AiDevId, u32ChnIdx);
#endif
        }

        if (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.bAecOpen)
        {
#if (defined(_AI_AEC_ENABLE_))
            _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAecQueue);
            _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAecRefQueue);
            _AI_AecDeinit(AiDevId, u32ChnIdx);
#endif
        }
        memset(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAiVqeConfig, 0x0, sizeof(MI_AI_VqeConfig_t));
        _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].bVqeEnable = FALSE;
        _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].bVqeAttrSet = FALSE;
    }

TO_RETURN:

    return s32Ret;
}

MI_S32 _AI_ApcDeinit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
#if 1

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle)
    {

        IaaApc_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hApcHandle = NULL;
    }

    if(NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf)
    {
        free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pApcWorkingBuf = NULL;
    }

#endif

    return s32Ret;
}

MI_S32 _AI_SrcDeinitForVqe(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
#if 0

#endif

    return 0;
}

#endif
/*enable ai vqe end*/

/*enable ai src start*/
#if 1

static MI_S32 _AI_ReSmpInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_SampleRate_e eOutSampleRate)
{
#if 1
    MI_S32 s32Ret = 0;
    MI_AUDIO_SampleRate_e eInSampleRate;
    MI_U32 u32SrcBufSize = 0;

    SRCStructProcess stSrcStruct;
    MI_U16 u16ChanlNum;
    MI_U32 u32SrcUnit;

    eInSampleRate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;
    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeEnable)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnableForVqe))
    {
        eInSampleRate = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);

    if (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAiEnableBf)
    {
        u16ChanlNum = 1;
    }
    u32SrcUnit = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;
    stSrcStruct.WaveIn_srate = (SrcInSrate)(eInSampleRate / 1000);
    stSrcStruct.channel = u16ChanlNum;
    stSrcStruct.mode = _AUDIO_GetSrcConvertMode(eInSampleRate, eOutSampleRate);
    u32SrcUnit = u32AiSrcPointNumber;
    stSrcStruct.point_number = u32SrcUnit;

    //(1)IaaSrc_GetBufferSize
    u32SrcBufSize = IaaSrc_GetBufferSize(stSrcStruct.mode);
    if (0 == u32SrcBufSize)
    {
        PRINTF_INFO("src buffer size is zero!!!\n");
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf = malloc(u32SrcBufSize);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf)
    {
        PRINTF_INFO("Malloc IaaSrc_GetBufferSize failed.\n");
        return -1;
    }
    PRINTF_INFO("Malloc IaaSrc_GetBufferSize succeed, u32SrcBufSize = %d.\n \
                stSrcStruct.mode = %d\n", u32SrcBufSize, stSrcStruct.mode);

    //(2)IaaSrc_Init
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle = IaaSrc_Init(
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf, &stSrcStruct);
    if(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle == NULL)
    {
        PRINTF_INFO("_AI_ReSmpInit Fail hSrcHandle == NULL !!!\n");
        return -1;
    }
    PRINTF_INFO("IaaSrc_Init succeed, WaveIn_srate = %d.\n \
                channel = %d, point_number = %d\n", stSrcStruct.WaveIn_srate, stSrcStruct.channel, stSrcStruct.point_number);

    PRINTF_INFO("[SRC]end,Waiting run...\n\n");
    return 0;

#endif
}

MI_S32 _AI_DoSrc(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue
)
{
#if 1
    MI_S32 s32GetSize = 0;
    MI_S32 s32Ret = 0;
    MI_U8 *pu8InputBuff = NULL;
    MI_U8 *pu8OutputBuff = NULL;
    MI_U32 u32ChnNum;
    MI_S32 s32BitwidthByte = 2;
    SRC_HANDLE hSrcHandle;
    MI_U32 u32SrcUnit;
    MI_S32 s32SrcOutSample;

    AUDIO_CHECK_POINTER(pstInputQueue);
    AUDIO_CHECK_POINTER(pstOutputQueue);
    //PRINTF_DBG("AiDevId:%d  AiChn:%d\n", AiDevId, AiChn);
    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    pu8OutputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8OutputBuff;

    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);

    if ((pstInputQueue == &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueue)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAiEnableBf))
    {
        u32ChnNum = 1;
    }

    if (pstInputQueue == &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueue)
    {
        hSrcHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle;

    }
    else if (pstInputQueue == &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueueForVqeAiIn)
    {
        hSrcHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandleForVqeAiIn;

    }
    else
    {
        hSrcHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandleForVqeAoIn;
    }

    AUDIO_CHECK_POINTER(hSrcHandle);
    u32SrcUnit = u32AiSrcPointNumber;
    s32GetSize = u32SrcUnit * s32BitwidthByte * u32ChnNum;

    while (pstInputQueue->s32Size >= s32GetSize)
    {
        memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Dev%d Chn%d Failed to call _AUDIO_QueueDraw!!!\n", AiDevId, AiChn);
            return -1;
        }
        PRINTF_DBG("QueueDraw succeed, Queue size = %d !!!\n", pstInputQueue->s32Size);

        s32SrcOutSample = s32GetSize;

        if (NULL == hSrcHandle)
        {
            PRINTF_INFO("Dev%d Chn%d hSrcHandle is NULL!!!!.\n", AiDevId, AiChn);
        }
        else
        {
            s32Ret = IaaSrc_Run(
                    hSrcHandle, (MI_S16 *)pu8InputBuff, (MI_S16 *)pu8OutputBuff, &s32SrcOutSample);
            if(0 != s32Ret)
            {
                PRINTF_INFO("IaaSrc_Run failed !!!\n");
            }
            PRINTF_INFO("IaaSrc_Run succeed,  s32GetSize:%d   s32SrcOutSample:%d!!!\n", s32GetSize, s32SrcOutSample);
        }

        if (s32SrcOutSample * s32BitwidthByte * u32ChnNum > 1024*25)
        {
            PRINTF_INFO("Dev%d Chn%d Src out size is too big:%d\n", AiDevId, AiChn, s32SrcOutSample * s32BitwidthByte);
            return -1;
        }

        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32SrcOutSample * s32BitwidthByte * u32ChnNum);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Dev%d Chn%d Failed to call _AUDIO_QueueInsert!!!\n", AiDevId, AiChn);
            return -1;
        }
    }

    return 0;

#endif
}

MI_S32 AI_EnableReSmp(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_SampleRate_e eOutSampleRate)
{
#if 1
    MI_S32 s32Ret = 0;
    MI_AUDIO_SampleRate_e eInSampleRate;
    MI_U16 u16ChanlNum;
    MI_U32 u32NumPerFrm;
    MI_U32 u32BitWidthByte = 2;
    MI_BOOL bInitSuccess = TRUE;

    if ((TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bVqeEnable)
            && (TRUE == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnableForVqe))
    {
        eInSampleRate = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiVqeConfig.s32WorkSampleRate;
    }
    else
    {
        eInSampleRate = _gastAiDevInfoApp[AiDevId].stDevAttr.eSamplerate;
    }

    if (eOutSampleRate == eInSampleRate)
    {
        PRINTF_INFO("Don't need to resample!!!\n");
        return -1;
    }
    PRINTF_INFO("[SRC]eInSampleRate -->eOutSampleRate:%d -->%d\n", eInSampleRate, eOutSampleRate);
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;
    PRINTF_INFO("[SRC]u32NumPerFrm = %d\n", u32NumPerFrm);

    s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueue,
                              u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("_AUDIO_QueueInit err !!!\n");
        bInitSuccess = FALSE;
    }
    PRINTF_INFO("[SRC]_AUDIO_QueueInit succeed, size = %d\n", u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AI_BUFFER_SIZE_RATIO);

    s32Ret = _AI_ReSmpInit(AiDevId, AiChn, eOutSampleRate);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d failed to call _AI_ReSmpInit!!!\n", AiDevId, AiChn);
        bInitSuccess = FALSE;
    }
    PRINTF_INFO("[SRC]_AI_ReSmpInit succeed.\n");

    if (FALSE == bInitSuccess)
    {
        _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueue);
        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle)
        {

            IaaSrc_Release(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle);
            _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle = NULL;
        }

        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf);
            _gastAiDevInfoApp[AiDevId].astChanInfo[0].pSrcWorkingBuf = NULL;
        }

        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnable = TRUE;
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].eOutResampleRate = eOutSampleRate;

    return 0;

#endif
}

MI_S32 AI_DisableReSmp(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
#if 1
    _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stSrcQueue);
    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle)
    {

        IaaSrc_Release(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hSrcHandle = NULL;
    }

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf)
    {
        free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pSrcWorkingBuf = NULL;
    }
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bResampleEnable = FALSE;
    PRINTF_INFO("succeed !\n");

    return 0;

#endif
}

#endif
/*enable ai src end*/

MI_S32 AI_SetAencAttr(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_AencConfig_t *pstAencConfig)
{
    MI_S32 s32Ret = MI_SUCCESS;

#if (defined(_AI_AENC_ENABLE_))
    MI_U32 u32ChnIdx, u32ChnStart = 0, u32ChnEnd = 0;

    AUDIO_CHECK_POINTER(pstAencConfig);

    if ((pstAencConfig->eAencType != E_MI_AUDIO_AENC_TYPE_G711A)
            && (pstAencConfig->eAencType != E_MI_AUDIO_AENC_TYPE_G711U)
            && (pstAencConfig->eAencType != E_MI_AUDIO_AENC_TYPE_G726)
       )
    {
        PRINTF_INFO("Invalid AencType[%d], Dev[%d]Chn[%d]!!!\n", pstAencConfig->eAencType, AiDevId, AiChn);
        return -1;
    }

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
    {
        memcpy(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAiAencConfig,
                pstAencConfig, sizeof(MI_AI_AencConfig_t));
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAencAttrSet = TRUE;

#endif
    return 0;
}

MI_S32 _AI_G726Init(MI_AUDIO_DEV AiDevId, stG726State_t *pstG726Info, MI_S32 s32BitRate)
{
    AUDIO_CHECK_POINTER(pstG726Info);
    g726_init(pstG726Info, s32BitRate);
    return 0;
}

MI_S32 _AI_AencInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AI_AencConfig_t stAiAencConfig;
    stG726State_t *pstG726Info = NULL;
    stAiAencConfig = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAencConfig;

    if(E_MI_AUDIO_AENC_TYPE_G726 == stAiAencConfig.eAencType)
    {
        pstG726Info = malloc(sizeof(stG726State_t));
        AUDIO_CHECK_POINTER(pstG726Info);
        memset(pstG726Info, 0, sizeof(stG726State_t));

        switch(stAiAencConfig.stAencG726Cfg.eG726Mode)
        {

            case E_MI_AUDIO_G726_MODE_16:
                _AI_G726Init(AiDevId, pstG726Info, 8000 * 2);
                PRINTF_INFO("_AI_G726Init 16k\n");
                break;
            case E_MI_AUDIO_G726_MODE_24:
                _AI_G726Init(AiDevId, pstG726Info, 8000 * 3);
                PRINTF_INFO("_AI_G726Init 24k\n");
                break;
            case E_MI_AUDIO_G726_MODE_32:
                _AI_G726Init(AiDevId, pstG726Info, 8000 * 4);
                PRINTF_INFO("_AI_G726Init 32k\n");
                break;
            case E_MI_AUDIO_G726_MODE_40:
                _AI_G726Init(AiDevId, pstG726Info, 8000 * 5);
                PRINTF_INFO("_AI_G726Init 40k\n");
                break;
            default:
                PRINTF_INFO("G726 Mode is not find:%d\n", stAiAencConfig.stAencG726Cfg.eG726Mode);
                free(pstG726Info);
                return -1;
        }
    }
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hG726Handle = pstG726Info;

    return 0;
}

MI_S32 AI_EnableAenc(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U16 u16ChanlNum;
    MI_U32 u32NumPerFrm;
    MI_U32 u32BitWidthByte = 2;
    MI_U32 u32ChnIdx, u32ChnStart = 0, u32ChnEnd = 0;
    MI_BOOL bInitSuccess = TRUE;

    if (TRUE != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAencAttrSet)
    {
        PRINTF_INFO("Dev%d Chn%d Aenc Attr is not set\n", AiDevId, AiChn);
        return -1;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u16ChanlNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    u32NumPerFrm = _gastAiDevInfoApp[AiDevId].stDevAttr.u32PtNumPerFrm;

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
    {
        s32Ret = _AUDIO_QueueInit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAencQueue,
                                    u32NumPerFrm * u32BitWidthByte * u16ChanlNum * 8 * AUDIO_BUFFER_SIZE_RATIO);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("_AUDIO_QueueInit stAencQueue failed.\n");
            bInitSuccess = FALSE;
        }
        else
        {
            PRINTF_DBG("_AUDIO_QueueInit stAencQueue succeed.\n");
        }

        s32Ret = _AI_AencInit(AiDevId, u32ChnIdx);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Dev%d Chn%d failed to call _AI_AencInit!!!\n", AiDevId, AiChn);
            bInitSuccess = FALSE;
        }
        else
        {
            PRINTF_DBG("Dev%d Chn%d succeed to call _AI_AencInit!!!\n", AiDevId, AiChn);
        }

        if (FALSE == bInitSuccess)
        {
            for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
            {
                _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAencQueue);
                if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle)
                {
                    free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle);
                    _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle = NULL;
                }
            }
            return -1;
        }

        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAencEnable = TRUE;
    }
    return 0;
}

MI_S32 AI_DisableAenc(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32ChnIdx, u32ChnStart = 0, u32ChnEnd = 0;

    if (TRUE != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAencAttrSet)
    {
        PRINTF_INFO("Dev%d Chn%d Aenc Attr is not set\n", AiDevId, AiChn);
        return -1;
    }

    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIdx = u32ChnStart; u32ChnIdx < u32ChnEnd; u32ChnIdx++)
    {
        _AUDIO_QueueDeinit(&_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].stAencQueue);
        if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle)
        {
            free(_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle);
            _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIdx].hG726Handle = NULL;
        }
    }
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].bAencEnable = FALSE;

    return 0;
}


MI_S32 _AI_G711Encoder(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue,
    MI_AUDIO_AencType_e eG711Type
)
{
    MI_S32 s32GetSize= 0;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32ChnIndex = 0;
    MI_U8 *pu8InputBuff = NULL;
    MI_U8 *pu8OutputBuff = NULL;

    AUDIO_CHECK_POINTER(pstInputQueue);
    AUDIO_CHECK_POINTER(pstOutputQueue);
    if ((E_MI_AUDIO_AENC_TYPE_G711A != eG711Type) && (E_MI_AUDIO_AENC_TYPE_G711U != eG711Type))
    {
        PRINTF_INFO("Invalid G711 type!!!\n");
        return -1;
    }

    if(E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        for (u32ChnIndex = 0; u32ChnIndex < _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt; u32ChnIndex++)
        {
            if (pstInputQueue == &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAencQueue)
            {
                break;
            }
        }

        if (u32ChnIndex == _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt)
        {
            PRINTF_INFO("Can't find Aenc Channel.\n");
            return -1;
        }
    }
    else if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
             ||  E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnIndex = AiChn;
    }

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].pu8InputBuff;
    pu8OutputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].pu8OutputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);

    memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    if (0 == pstInputQueue->s32Size)
    {
        return 0;
    }

    if ((pstInputQueue->s32Size > 0) && (pstInputQueue->s32Size <= AUDIO_DEMO_TEMP_SIZE))
    {
        s32GetSize = pstInputQueue->s32Size;
        s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
        if (s32Ret != MI_SUCCESS)
        {
            PRINTF_INFO("Failed to call _MI_AI_QueueDraw!!!\n");
            return -1;
        }

        switch(eG711Type)
        {
            case E_MI_AUDIO_AENC_TYPE_G711A:
                G711Encoder(
                    (MI_S16 *)pu8InputBuff, pu8OutputBuff, s32GetSize >> 1, AUDIO_AI_G711A);
                break;

            case E_MI_AUDIO_AENC_TYPE_G711U:
                G711Encoder(
                    (MI_S16 *)pu8InputBuff, pu8OutputBuff, s32GetSize >> 1, AUDIO_AI_G711U);
                break;

            default:
                PRINTF_INFO("G711 Enocder Type not find:%d\n", eG711Type);
                return -1;
        }

        s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32GetSize >> 1);
        if (MI_SUCCESS != s32Ret)
        {
            PRINTF_INFO("Failed to call _MI_AI_QueueInsert!!!\n");
            return s32Ret;
        }
    }
    else
    {
        PRINTF_INFO("Input queue size[%d] is out of range.\n", pstInputQueue->s32Size);
        return -1;
    }

    return 0;
}

static MI_S32 _AI_G726Encoder(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue,
    MI_AUDIO_G726Mode_e eG726Mode
)
{
    MI_S32 s32WriteSize = 0;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 s32GetSize = 0;
    MI_U32 u32ChnIndex = 0;
    MI_U8 *pu8InputBuff = NULL;
    MI_U8 *pu8OutputBuff = NULL;
    stG726State_t *hG726Handle = NULL;

    //g726_16   8bytes ->1bytes
    //g726_24   16bytes->3bytes
    //g726_32   4bytes ->1bytes
    //g726_40   16bytes->5bytes

    AUDIO_CHECK_POINTER(pstInputQueue);
    AUDIO_CHECK_POINTER(pstOutputQueue);
    if (
        (E_MI_AUDIO_G726_MODE_16 != eG726Mode)
        &&  (E_MI_AUDIO_G726_MODE_24 != eG726Mode)
        &&  (E_MI_AUDIO_G726_MODE_32 != eG726Mode)
        &&  (E_MI_AUDIO_G726_MODE_40 != eG726Mode)
    )
    {
        PRINTF_INFO("Invalid G726 Type!!!\n");
        return -1;
    }

    if(E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        for (u32ChnIndex = 0; u32ChnIndex < _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt; u32ChnIndex++)
        {
            if (pstInputQueue == &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAencQueue)
            {
                break;
            }
        }

        if (u32ChnIndex == _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt)
        {
            PRINTF_INFO("Can't find Aenc Channel.\n");
            return -1;
        }
    }
    else if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
             ||  E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnIndex = AiChn;
    }

    if (pstInputQueue->s32Size < AUDIO_AI_AENC_G726_UNIT)
    {
        return 0;
    }

    s32GetSize = pstInputQueue->s32Size;
    s32GetSize -= (s32GetSize % AUDIO_AI_AENC_G726_UNIT);

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].pu8InputBuff;
    pu8OutputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].pu8OutputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);

    memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Failed to call _MI_AI_QueueDraw!!!\n");
        return -1;
    }

    hG726Handle = _gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].hG726Handle;
    AUDIO_CHECK_POINTER(hG726Handle);

    if (NULL == hG726Handle)
    {
        PRINTF_INFO("hG726Handle is NULL!!!!!.\n");
    }
    else
    {
        s32WriteSize = g726_encode(hG726Handle, pu8OutputBuff, (MI_S16*) pu8InputBuff, s32GetSize >> 1);
    }

    if (s32WriteSize <= 0)
    {
        PRINTF_INFO("s32WriteSize:%d\n", s32WriteSize);
        return -1;
    }

    s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8OutputBuff, s32WriteSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Failed to call _MI_AI_QueueInsert!!!\n");
        return -1;
    }

    return 0;
}

MI_S32 _AI_DoAenc(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue
)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AI_AencConfig_t *pstAiAencInfo = NULL;

    AUDIO_CHECK_POINTER(pstInputQueue);
    AUDIO_CHECK_POINTER(pstOutputQueue);

    pstAiAencInfo = &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAiAencConfig;
    AUDIO_CHECK_POINTER(pstAiAencInfo);

    switch(pstAiAencInfo->eAencType)
    {
        case E_MI_AUDIO_AENC_TYPE_G711A:
        case E_MI_AUDIO_AENC_TYPE_G711U:
            s32Ret = _AI_G711Encoder(AiDevId, AiChn, pstInputQueue, pstOutputQueue, pstAiAencInfo->eAencType);
            if (MI_SUCCESS != s32Ret)
            {
                PRINTF_INFO("Failed to call _AI_G711Encoder!!!\n");
                return s32Ret;
            }
            break;

        case E_MI_AUDIO_AENC_TYPE_G726:
            s32Ret = _AI_G726Encoder(AiDevId, AiChn, pstInputQueue, pstOutputQueue, pstAiAencInfo->stAencG726Cfg.eG726Mode);
            if (MI_SUCCESS != s32Ret)
            {
                PRINTF_INFO("Failed to call _AI_G726Encoder!!!\n");
                return s32Ret;
            }
            break;

        default:
            PRINTF_INFO("AencType is not find:%d\n", pstAiAencInfo->eAencType);
            s32Ret = MI_AI_ERR_NOT_PERM;
            break;
    }
    return s32Ret;

}


/*enable ai Aec start*/
#if 1
MI_S32 _AI_AecInit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_VqeConfig_t *pstVqeConfig)
{
#if 1
    MI_S32 s32Ret = 0;
    MI_U32 u32AecBufSize = 0;
    AudioAecInit  stAecInit;
    MI_AUDIO_SampleRate_e eSampleRate;
    MI_U32 u32ChnNum = 0;

    AUDIO_CHECK_POINTER(pstVqeConfig);

    eSampleRate = pstVqeConfig->s32WorkSampleRate;
    if ((E_MI_AUDIO_SAMPLE_RATE_8000 != eSampleRate) && (E_MI_AUDIO_SAMPLE_RATE_16000 != eSampleRate))
    {
        PRINTF_INFO("Dev%d Chn%d Aec only support 8/16K!!!\n", AiDevId, AiChn);
        return -1;
    }

    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode);
    // Aec init param setting
    stAecInit.point_number = AUDIO_VQE_SAMPLES_UNIT;  //128
    stAecInit.farend_channel = u32ChnNum;
    stAecInit.nearend_channel = u32ChnNum;
	PRINTF_DBG("\nfarend_channel = nearend_channel = %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", u32ChnNum);
    AUDIO_VQE_SAMPLERATE_TRANS_TYPE(eSampleRate, stAecInit.sample_rate);
	PRINTF_DBG("\neSampleRate = %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", eSampleRate);

    //(1)IaaAec_GetBufferSize
    u32AecBufSize = IaaAec_GetBufferSize();
    if (u32AecBufSize == 0)
    {
        PRINTF_INFO("Dev%d Chn%d Aec buf size is 0!!!\n", AiDevId, AiChn);
        return -1;
    }

    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf = (void *)malloc(u32AecBufSize);
    if (NULL == _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf)
    {
        PRINTF_INFO("Dev%d Chn%d Malloc IaaAec_GetBuffer failed.\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("Dev%d Chn%d Aec buffer malloc succeed, size = %d.\n", AiDevId, AiChn, u32AecBufSize);

    //(2)IaaAec_Init
    _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle = IaaAec_Init(
            (char*)_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf, &stAecInit);
    PRINTF_INFO("Init AEC end, Waiting AEC config...\n\n");

    return s32Ret;

#endif
}

MI_S32 _AI_New_DoAEC(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    MI_SYS_BufInfo_t stAiBufInfo,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue
)
{
    MI_S32 s32Ret = 0;

    AUDIO_CHECK_POINTER(pstInputQueue);
    AUDIO_CHECK_POINTER(pstOutputQueue);

    s32Ret = _AI_DoAecAlgorithm(AiDevId, AiChn, &stAiBufInfo, pstInputQueue, pstOutputQueue);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d failed to call _AI_DoAecAlgorithm!!!\n", AiDevId, AiChn);
        return s32Ret;
    }

    return s32Ret;
}

MI_S32 _AI_DoAecAlgorithm(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    MI_SYS_BufInfo_t *pstAiBufInfo,
    _AUDIO_QueueInfo_t *pstInputQueue,
    _AUDIO_QueueInfo_t *pstOutputQueue
)
{
#if 1

#if 1
    MI_S32 s32Ret = 0;
    AEC_HANDLE hAecHandle;
    MI_U32 u32BitwidthByte = 2;
    MI_U32 u32AECWrPtr;
    MI_U32 u32AECSampleUnitByte;
    MI_U32 u32AECOutBytes;
    MI_S32 s32GetSize = 0;
    MI_U8 *pu8InputBuff = NULL;
    MI_U8 *pu8OutputBuff = NULL;
    _AUDIO_QueueInfo_t * pstAecRefQueue = NULL;
    MI_U32 u32ChnNum = 0;
    MI_AUDIO_SoundMode_e eSoundMode;
    MI_AUDIO_BitWidth_e eBitWidth;

    u32AECWrPtr = 0;
    u32AECSampleUnitByte = 0;
    u32AECOutBytes = 0;
    eBitWidth = _gastAiDevInfoApp[AiDevId].stDevAttr.eBitwidth;

    eSoundMode = _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode;
    AUDIO_USER_TRANS_EMODE_TO_CHAN(u32ChnNum, eSoundMode);

    s32GetSize = AUDIO_VQE_SAMPLES_UNIT * u32BitwidthByte * u32ChnNum;

    pu8InputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8InputBuff;
    pu8OutputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pu8OutputBuff;
    AUDIO_CHECK_POINTER(pu8InputBuff);
    AUDIO_CHECK_POINTER(pu8OutputBuff);

    hAecHandle = _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle;
    AUDIO_CHECK_POINTER(hAecHandle);
    pstAecRefQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stAecRefQueue;
    AUDIO_CHECK_POINTER(pstAecRefQueue);

    if(pstAecRefQueue->s32Size != pstInputQueue->s32Size)
    {
        PRINTF_INFO("pstAecRefQueue->s32Size != pstInputQueue->s32Size err.\n");
        //return -1;
    }

    memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);

#if 0
    s32Ret = _AUDIO_QueueDraw(pstAecRefQueue, pu8OutputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("AEC Ref Queue have not enough data!!!!\n");
        return 0;
    }

    s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("AEC Queue have not enough data!!!!\n");
        return 0;
    }
#endif

    u32AECSampleUnitByte = AUDIO_VQE_SAMPLES_UNIT * u32BitwidthByte * u32ChnNum;
    u32AECOutBytes = s32GetSize;
    u32AECWrPtr = 0;
    if (u32AECOutBytes % u32AECSampleUnitByte)
    {
        PRINTF_INFO("u32AECOutBytes[%d] size is not aligned!!!!!\n", u32AECOutBytes);
    }

    while((pstAecRefQueue->s32Size >= s32GetSize) && (pstInputQueue->s32Size >= s32GetSize))
    {
		memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
		s32Ret = _AUDIO_QueueDraw(pstAecRefQueue, pu8OutputBuff, s32GetSize);
		if (MI_SUCCESS != s32Ret)
		{
			PRINTF_INFO("AEC Ref Queue have not enough data!!!!\n");
			return -1;
		}

		memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
		s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
		if (MI_SUCCESS != s32Ret)
		{
			PRINTF_INFO("AEC Queue have not enough data!!!!\n");
			return -1;
		}

#if (defined(APP_AI_DUMP_AEC_ENABLE))   //AI dump Aec files
        fwrite(pu8OutputBuff, 1, s32GetSize, fpAiDumpAecFar);
        fwrite(pu8InputBuff, 1, s32GetSize, fpAiDumpAecNear);
#endif  //AI dump Aec files

        if (NULL == hAecHandle)
        {
            PRINTF_INFO("hAecHandle is NULL!!!!!!.\n");
        }
        else
        {
            IaaAec_Run(hAecHandle, (MI_S16*)pu8InputBuff, (MI_S16*)pu8OutputBuff);
        }

#if (defined(APP_AI_DUMP_AEC_ENABLE))   //AI dump Aec files
        fwrite(pu8InputBuff, 1, s32GetSize, fpAiDumpAecOutput);
#endif  //AI dump Aec files

	    s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8InputBuff, s32GetSize);
	    if (MI_SUCCESS != s32Ret)
	    {
	        PRINTF_INFO("Failed to call _AUDIO_QueueInsert!!!\n");
	        return -1;
	    }
    }
    return 0;

	/*
	......
    if(pstAecRefQueue->s32Size != pstInputQueue->s32Size)
    {
        PRINTF_INFO("pstAecRefQueue->s32Size != pstInputQueue->s32Size err.\n");
        return -1;
    }

    memset(pu8OutputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    s32Ret = _AUDIO_QueueDraw(pstAecRefQueue, pu8OutputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("AEC Ref Queue have not enough data!!!!\n");
        return 0;
    }

    memset(pu8InputBuff, 0, AUDIO_DEMO_TEMP_SIZE);
    s32Ret = _AUDIO_QueueDraw(pstInputQueue, pu8InputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("AEC Queue have not enough data!!!!\n");
        return 0;
    }

    u32AECSampleUnitByte = AUDIO_VQE_SAMPLES_UNIT * u32BitwidthByte * u32ChnNum;
    u32AECOutBytes = s32GetSize;
    u32AECWrPtr = 0;
    if (u32AECOutBytes % u32AECSampleUnitByte)
    {
        PRINTF_INFO("u32AECOutBytes[%d] size is not aligned!!!!!\n", u32AECOutBytes);
    }

    while(u32AECOutBytes >= u32AECSampleUnitByte)
    {
        if (AUDIO_DEMO_TEMP_SIZE <= u32AECWrPtr)
        {
            PRINTF_INFO("AEC write pointer is out of buffer.\n");
            return -1;
        }

        if (NULL == hAecHandle)
        {
            PRINTF_INFO("hAecHandle is NULL!!!!!!.\n");
        }
        else
        {
            IaaAec_Run(hAecHandle, (MI_S16*)(pu8InputBuff + u32AECWrPtr), (MI_S16*)(pu8OutputBuff+ u32AECWrPtr));
        }
        u32AECOutBytes -= u32AECSampleUnitByte;
        u32AECWrPtr += u32AECSampleUnitByte;
    }

    if (u32AECWrPtr != s32GetSize)
    {
        PRINTF_INFO("u32AECWrPtr[%d] != s32GetSize[%d].\n", u32AECWrPtr, s32GetSize);
    }

    s32Ret = _AUDIO_QueueInsert(pstOutputQueue, pu8InputBuff, s32GetSize);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Failed to call _AUDIO_QueueInsert!!!\n");
        return -1;
    }

    return 0;
	*/
#endif

#endif
}

MI_S32 _AI_AecConfig(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_VqeConfig_t *pstVqeConfig)
{
    MI_S32 s32Ret = MI_SUCCESS;
#if 1
    AudioAecConfig stAecConfig;
	memset(&stAecConfig, 0, sizeof(AudioAecConfig));

    stAecConfig.comfort_noise_enable =  (IAA_AEC_BOOL)pstVqeConfig->stAecCfg.bComfortNoiseEnable;
    stAecConfig.delay_sample = pstVqeConfig->stAecCfg.s16DelaySample;
    memcpy(stAecConfig.suppression_mode_freq,
           pstVqeConfig->stAecCfg.u32AecSupfreq,
           sizeof(pstVqeConfig->stAecCfg.u32AecSupfreq));
    memcpy(stAecConfig.suppression_mode_intensity,
           pstVqeConfig->stAecCfg.u32AecSupIntensity,
           sizeof(pstVqeConfig->stAecCfg.u32AecSupIntensity));

    // AEC config
    if (_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle == NULL)
    {
        PRINTF_INFO("Dev%d Chn%d IaaAec_Init failed !!!\n", AiDevId, AiChn);
        return -1;
    }
    s32Ret = IaaAec_Config(
                 _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle, &stAecConfig);
    if (MI_SUCCESS != s32Ret)
    {
        PRINTF_INFO("Dev%d Chn%d IaaAec_Config failed !!!\n", AiDevId, AiChn);
        return -1;
    }
    PRINTF_INFO("AEC config end, Waiting run...\n\n");
#endif

    return s32Ret;
}

MI_S32 _AI_AecDeinit(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

#if 1

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle)
    {
        IaaAec_Free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].hAecHandle = NULL;
    }

    if (NULL != _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf)
    {
        free(_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf);
        _gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].pAecWorkingBuf = NULL;
    }
#endif

    return s32Ret;
}

#endif
/*enable ai Aec end*/

#endif

/*AI_DoAlg_GetFrame start*/
#if 1
MI_S32 AI_DoAlg(
    MI_AUDIO_DEV AiDevId,
    MI_AI_CHN AiChn,
    MI_AUDIO_Frame_t *pstFrm,
    MI_AUDIO_AecFrame_t *pstAecFrm, MI_S32 *OutputBuffSize)
{
    MI_S32 s32Ret;
    MI_S32 s32GetSize;
    MI_U32 u32ChnStart = 0;
    MI_U32 u32ChnEnd = 0;
    MI_U32 u32ChnIndex = 0;
    MI_BOOL bNoBuff = FALSE;

    _AUDIO_QueueInfo_t *pstTempChnQueue = NULL;

    _AUDIO_QueueInfo_t *pstSrcQueue = NULL;
    _AUDIO_QueueInfo_t *pstAecQueue = NULL;
    _AUDIO_QueueInfo_t *pstAecRefQueue = NULL;
    _AUDIO_QueueInfo_t *pstApcQuque = NULL;
    _AUDIO_QueueInfo_t *pstAedQueue = NULL;
    _AUDIO_QueueInfo_t *pstSslQueue = NULL;
    _AUDIO_QueueInfo_t *pstChnQueue = NULL;
    _AUDIO_QueueInfo_t *pstSrcQueueForVqeAiIn = NULL;
    _AUDIO_QueueInfo_t *pstSrcQueueForVqeAoIn = NULL;
    _AUDIO_QueueInfo_t __attribute__((unused)) *pstGarbageQueue = NULL;
    _AUDIO_QueueInfo_t *pstBfQueue = NULL;
    _AUDIO_QueueInfo_t *pstAencQueue = NULL;

    MI_SYS_BufInfo_t stAiBufInfo;//add
    MI_U8    *pu8OutputBuff = NULL;

    pstTempChnQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[AiChn].stTempChnQueue;
    pu8OutputBuff = _gastAiDevInfoApp[AiDevId].astChanInfo[AoChn].pu8OutputBuff;
    if(NULL == pu8OutputBuff)
    {
        PRINTF_INFO("pu8OutputBuff is NULL !\n");
        return -1;
    }
    memset(pu8OutputBuff, 0x0, AUDIO_DEMO_TEMP_SIZE);
    //get data
    if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
            || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = AiChn;
        u32ChnEnd = AiChn + 1;
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
    {
        u32ChnStart = 0;
        u32ChnEnd = _gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt;
    }

    for (u32ChnIndex = u32ChnStart; u32ChnIndex < u32ChnEnd; u32ChnIndex++)
    {
        pstApcQuque = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stApcQuque;
        pstChnQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stChnQueue;
        pstAedQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAedQueue;
        pstSslQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stSslQueue;
        pstSrcQueueForVqeAiIn = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stSrcQueueForVqeAiIn;
        pstSrcQueueForVqeAoIn = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stSrcQueueForVqeAoIn;
        pstGarbageQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stGarbageQueue;
        pstBfQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stBfQueue;
        pstSrcQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stSrcQueue;
        pstAecQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAecQueue;
		pstAecRefQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAecRefQueue;
        pstAencQueue = &_gastAiDevInfoApp[AiDevId].astChanInfo[u32ChnIndex].stAencQueue;
    }

    if(bAiEnableAed)
    {
        s32Ret = _AUDIO_QueueInsert(pstAedQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
        if (MI_SUCCESS != s32Ret)
        {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Aed Queue has not enough size!!!current size[%d] max size[%d].\n",
                        pstAedQueue->s32Size, pstAedQueue->s32Max);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
        }
    }
    else if(bAiEnableSsl)
    {
        s32Ret = _AUDIO_QueueInsert(pstSslQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
        if (MI_SUCCESS != s32Ret)
        {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!current size[%d] max size[%d].\n",
                        pstSslQueue->s32Size, pstSslQueue->s32Max);
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
        }
    }
    else if(bAiEnableAec)
    {
        if(TRUE == bIsAecRefQueue)
        {
            s32Ret = MI_SUCCESS;
        }
        else
        {
            s32Ret = _AUDIO_QueueInsert(pstAecQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
        }

        if (MI_SUCCESS != s32Ret)
        {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
        }
        else if (AI_DEV_ID_I2S_RX != AiDevId)
        {
            if (E_MI_AUDIO_SOUND_MODE_MONO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode
                    || E_MI_AUDIO_SOUND_MODE_STEREO == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
            {
                s32Ret = _AUDIO_QueueInsert(
                             pstAecRefQueue,
                             pstAecFrm->stRefFrame.apVirAddr[0], pstAecFrm->stRefFrame.u32Len[0]);
				if (MI_SUCCESS != s32Ret)
				{
		            if(E_APP_ERR_NO_BUFFER == s32Ret)
		            {
		                PRINTF_INFO("Queue has not enough size!!!\n");
		                bNoBuff = TRUE;
                        bIsAecRefQueue = TRUE;
		            }
		            else
		            {
		                PRINTF_INFO("_AUDIO_QueueInsert failed.\n");
		                return -1;
		            }
				}
                else
                {
                    bIsAecRefQueue = FALSE;
                }
            }
            else if (E_MI_AUDIO_SOUND_MODE_QUEUE == _gastAiDevInfoApp[AiDevId].stDevAttr.eSoundmode)
            {
            /*
                s32Ret = _AUDIO_QueueInsert(
                            pstAecRefQueue,
                            ((MI_U8 *)stAiBufInfo.stRawData.pVirAddr +
                                u32CopySize * (_gastAiDevInfoApp[AiDevId].stDevAttr.u32ChnCnt + (u32ChnIndex % u32SrcChnCnt))),
                            u32CopySize);
            */
            }
       }
   }
   else if(bAiEnableBf)
   {
       s32Ret = _AUDIO_QueueInsert(pstBfQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
       if (MI_SUCCESS != s32Ret)
       {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
       }
   }
   else if(bAiEnableVqe)
   {
       s32Ret = _AUDIO_QueueInsert(pstApcQuque, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
       if (MI_SUCCESS != s32Ret)
       {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
       }
   }
   else if(bAiEnableResample)
   {
       s32Ret = _AUDIO_QueueInsert(pstSrcQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
       if (MI_SUCCESS != s32Ret)
       {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
       }
   }

    /* encoder */
   else if(bAiEnableAenc)
   {
       PRINTF_INFO("encoder.\n");
       s32Ret = _AUDIO_QueueInsert(pstAencQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
       if (MI_SUCCESS != s32Ret)
       {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
       }
   }
   else
   {
       s32Ret = _AUDIO_QueueInsert(pstTempChnQueue, pstFrm->apVirAddr[0], pstFrm->u32Len[0]);
       if (MI_SUCCESS != s32Ret)
       {
            if(E_APP_ERR_NO_BUFFER == s32Ret)
            {
                PRINTF_INFO("Queue has not enough size!!!\n");
                bNoBuff = TRUE;
            }
            else
            {
                PRINTF_INFO("_AUDIO_QueueInsert failed\n");
                return -1;
            }
       }
   }

/* Do alg start */

#if (defined(_AI_AED_ENABLE_))
            //do Aed
            if (bAiEnableAed)
            {
                s32Ret = _AI_DoAed(AiDevId, AiChn, pstAedQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoAed failed.\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_SSL_ENABLE_))
            // do ssl
            if (bAiEnableSsl
                && ((FALSE == bAiEnableBf) || ((TRUE == bAiEnableBf) && (TRUE == bAiSetBfDoa))))
            {
                s32Ret = _AI_DoSsl(AiDevId, AiChn, pstSslQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSsl failed\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_SRC_ENABLE_))
            //do resample for vqe
            if (bAiEnableResampleForVqe && bAiEnableAec)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAiIn, pstAecQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }

                if (AI_DEV_ID_I2S_RX != AiDevId)
                {
                    s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAoIn, pstAecRefQueue);
                    if (MI_SUCCESS != s32Ret)
                    {
                        PRINTF_INFO("_AI_DoSrc failed.\n");
                        return -1;
                    }
                }
            }
            else if (bAiEnableResampleForVqe && bAiEnableBf)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAiIn, pstBfQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }

                if (AI_DEV_ID_I2S_RX != AiDevId)
                {
                    s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAoIn, pstGarbageQueue);
                    if (MI_SUCCESS != s32Ret)
                    {
                        PRINTF_INFO("_AI_DoSrc failed.\n");
                        return -1;
                    }
                    _AUDIO_QueueClear(pstGarbageQueue);
                }
            }
            else if (bAiEnableResampleForVqe && bAiEnableApc)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAiIn, pstApcQuque);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }

                if (AI_DEV_ID_I2S_RX != AiDevId)
                {
                    s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAoIn, pstGarbageQueue);
                    if (MI_SUCCESS != s32Ret)
                    {
                        PRINTF_INFO("_AI_DoSrc failed.\n");
                        return -1;
                    }
                    _AUDIO_QueueClear(pstGarbageQueue);
                }
            }
            else if (bAiEnableResampleForVqe && bAiEnableResample)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAiIn, pstSrcQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }

                if (AI_DEV_ID_I2S_RX != AiDevId)
                {
                    s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAoIn, pstGarbageQueue);
                    if (MI_SUCCESS != s32Ret)
                    {
                        PRINTF_INFO("_AI_DoSrc failed\n");
                        return -1;
                    }
                    _AUDIO_QueueClear(pstGarbageQueue);
                }
            }
            else if (bAiEnableResampleForVqe)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAiIn, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }

                if (AI_DEV_ID_I2S_RX != AiDevId)
                {
                    s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueueForVqeAoIn, pstGarbageQueue);
                    if (MI_SUCCESS != s32Ret)
                    {
                        PRINTF_INFO("_AI_DoSrc failed.\n");
                        return -1;
                    }
                    _AUDIO_QueueClear(pstGarbageQueue);
                }
            }
#endif

#if (defined(_AI_AEC_ENABLE_))
            //do Aec
            if (bAiEnableAec && bAiEnableBf)
            {
                s32Ret = _AI_New_DoAEC(AiDevId, AiChn, stAiBufInfo, pstAecQueue, pstBfQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoAEC failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableAec && bAiEnableApc)
            {
                s32Ret = _AI_New_DoAEC(AiDevId, AiChn, stAiBufInfo, pstAecQueue, pstApcQuque);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoAEC failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableAec && bAiEnableResample)
            {
                s32Ret = _AI_New_DoAEC(AiDevId, AiChn, stAiBufInfo, pstAecQueue, pstSrcQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoAEC failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableAec && bAiEnableAenc)
            {
                s32Ret = _AI_New_DoAEC(AiDevId, AiChn, stAiBufInfo, pstAecQueue, pstAencQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoAEC failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableAec)
            {
                s32Ret = _AI_New_DoAEC(AiDevId, AiChn, stAiBufInfo, pstAecQueue, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoAEC failed.\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_BF_ENABLE_))
            // do bf
            if (bAiEnableBf && bAiEnableApc)
            {
                s32Ret = _AI_DoBf(AiDevId, AiChn, pstBfQueue, pstApcQuque);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoSslAndBf failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableBf && bAiEnableResample)
            {
                s32Ret = _AI_DoBf(AiDevId, AiChn, pstBfQueue, pstSrcQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoSslAndBf failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableBf && bAiEnableAenc)
            {
                s32Ret = _AI_DoBf(AiDevId, AiChn, pstBfQueue, pstAencQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoSslAndBf failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableBf)
            {
                s32Ret = _AI_DoBf(AiDevId, AiChn, pstBfQueue, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_MI_AI_DoSslAndBf failed.\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_APC_ENABLE_))
            //do Vqe
            if (bAiEnableApc && bAiEnableResample)
            {
                s32Ret = _AI_DoVqe(AiDevId, AiChn, pstApcQuque, pstSrcQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoVqe failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableApc && bAiEnableAenc)
            {
                s32Ret = _AI_DoVqe(AiDevId, AiChn, pstApcQuque, pstAencQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoVqe failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableApc)
            {
                s32Ret = _AI_DoVqe(AiDevId, AiChn, pstApcQuque, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoVqe failed.\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_SRC_ENABLE_))
            //do Resample
            if (bAiEnableResample && bAiEnableAenc)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueue, pstAencQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }
            }
            else if (bAiEnableResample)
            {
                s32Ret = _AI_DoSrc(AiDevId, AiChn, pstSrcQueue, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoSrc failed.\n");
                    return -1;
                }
            }
#endif

#if (defined(_AI_AENC_ENABLE_))
            //do AI AENC
            if (bAiEnableAenc)
            {
                PRINTF_INFO("You need add others case !!!!!!.\n");
                s32Ret = _AI_DoAenc(AiDevId, AiChn, pstAencQueue, pstTempChnQueue);
                if (MI_SUCCESS != s32Ret)
                {
                    PRINTF_INFO("_AI_DoAenc failed.\n");
                    return -1;
                }
            }
#endif
/* Do alg end */
   if ((pstTempChnQueue->s32Size > 0) && (pstTempChnQueue->s32Size <= AUDIO_DEMO_TEMP_SIZE))
   {
       s32GetSize = pstTempChnQueue->s32Size;
       s32Ret = _AUDIO_QueueDraw(pstTempChnQueue, pu8OutputBuff, s32GetSize);
       if (s32Ret != MI_SUCCESS)
       {
           PRINTF_INFO("_AUDIO_QueueDraw failed.\n");
           return -1;
       }
       PRINTF_DBG("Draw Queue size = %d !!!\n", s32GetSize);

       *OutputBuffSize = s32GetSize;
   }
   else
   {
       PRINTF_INFO("pstTempChnQueue size == 0 or is too big !\n");
   }

   if(bNoBuff)
   {
        return -2;
   }
   return 0;
}

#endif
/*AI_DoAlg_GetFrame end*/

int main (int argc, char *argv[])
{
    MI_S32      s32Opt = 0;
    MI_BOOL     bCheckPass = FALSE;
    MI_U32      u32ChnIdx;
    MI_U32      u32ChnCnt;

    // parsing command line
    while ((s32Opt = getopt(argc, argv, "t:I::o:h::g::e::n::r:a:A::c:b::B::v:d:s:S::F::C:m:M:O::i:H::G::E::N::R:V:D:w:W::j:J:")) != -1)
    {
        switch(s32Opt)
        {
            // run time
            case 't':
                {
                    u32RunTime = atoi(optarg);
                }
                break;

            // enable Ai
            case 'I':
                {
                    bEnableAI = TRUE;
                }
                break;

            // set Ai output path
            case 'o':
                {
                    pu8AiOutputPath = (MI_U8*)optarg;
                }
                break;

            // enable Ai hpf
            case 'h':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableApc = TRUE; //add
                    bAiEnableHpf = TRUE;
                }
                break;

            // enable Ai nr
            case'n':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableApc = TRUE; //add
                    bAiEnableNr = TRUE;
                }
                break;

            // enable Ai agc
            case'g':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableApc = TRUE; //add
                    bAiEnableAgc = TRUE;
                }
                break;

            // enable Ai Eq
            case 'e':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableApc = TRUE; //add
                    bAiEnableEq = TRUE;
                }
                break;

            // set Ai Resample rate
            case 'r':
                {
                    bAiEnableResample = TRUE;
                    eAiOutputResampleRate = (MI_AUDIO_SampleRate_e)atoi(optarg);
                }
                break;

            // set Ai Aenc Type,This demo can't support AENC and DENC!!!
            case 'a':
                {
                    //bAiEnableAenc = FALSE;
                    //printf("This demo can't support AENC and DENC!!!\n");
                    bAiEnableAenc = TRUE;
                    if (!strncmp(optarg, "g711a", 5))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G711A;
                    }
                    else if (!strncmp(optarg, "g711u", 5))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G711U;
                    }
                    else if (!strncmp(optarg, "g726_16", 7))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G726;
                        eAiAencG726Mode = E_MI_AUDIO_G726_MODE_16;
                    }
                    else if (!strncmp(optarg, "g726_24", 7))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G726;
                        eAiAencG726Mode = E_MI_AUDIO_G726_MODE_24;
                    }
                    else if (!strncmp(optarg, "g726_32", 7))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G726;
                        eAiAencG726Mode = E_MI_AUDIO_G726_MODE_32;
                    }
                    else if (!strncmp(optarg, "g726_40", 7))
                    {
                        eAiAencType = E_MI_AUDIO_AENC_TYPE_G726;
                        eAiAencG726Mode = E_MI_AUDIO_G726_MODE_40;
                    }
                    else
                    {
                        printf("Aenc Type Error\n");
                        return -1;
                    }
                }
                break;

            // enable Ai Aed
            case 'A':
                {
                    bAiEnableAed = TRUE;
                }
                break;

            // set Ai chn num
            case 'c':
                {
                    u32AiChnCnt = atoi(optarg);
                }
                break;

            // enable Ai sw AEC
            case 'b':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableAec = TRUE;
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
                    bAiEnableHwAec = FALSE;
#endif
                }
                break;

#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
            // enable Ai Hw AEC
            case 'B':
                {
                    bAiEnableVqe = TRUE;
                    bAiEnableAec = TRUE;
                    bAiEnableHwAec = TRUE;
                }
                break;
#endif
            // set Ai volume
            case 'v':
                {
                    s32AiVolume = (MI_S32)atoi(optarg);
                    bAiSetVolume = TRUE;
                }
                break;

            // set Ai device ID
            case 'd':
                {
                    AiDevId = atoi(optarg);
                }
                break;

            // set Ai sample rate
            case 's':
                {
                    eAiSampleRate = (MI_AUDIO_SampleRate_e)atoi(optarg);
                }
                break;

            // set Ai Sound Mode
            case 'm':
                {
                    eAiSoundMode = (MI_U32)atoi(optarg);
                    if (eAiSoundMode < E_AI_SOUND_MODE_MONO || eAiSoundMode > E_AI_SOUND_MODE_QUEUE)
                    {
						printf("Illegal sound mode!!!\n");
						return -1;
                    }
                }
                break;

            case 'M':
            	{
					u32MicDistance = (MI_U32)atoi(optarg);
            	}
            	break;

            case 'S':
            	{
					bAiEnableSsl = TRUE;
            	}
            	break;

           	case 'F':
           		{
					bAiEnableBf = TRUE;
           		}
           		break;

            case 'C':
                {
                    bAiSetBfDoa = TRUE;
                    s32AiBfDoa = (MI_S32)atoi(optarg);
                }
                break;

           	case 'w':
           		{
					u32VqeWorkingSampleRate = (MI_U32)atoi(optarg);
					if ((E_MI_AUDIO_SAMPLE_RATE_8000 != u32VqeWorkingSampleRate)
						&& (E_MI_AUDIO_SAMPLE_RATE_16000 != u32VqeWorkingSampleRate)
						&& (E_MI_AUDIO_SAMPLE_RATE_32000 != u32VqeWorkingSampleRate)
						&& (E_MI_AUDIO_SAMPLE_RATE_48000 != u32VqeWorkingSampleRate))
					{
						printf("Illegal Ai vqe working sample rate!!!\n");
						return -1;
					}
				}
				break;

            case 'W':
                {
                    bAiDumpPcmData = TRUE;
                }
                break;

            // enable AO
            case 'O':
                {
                    bEnableAO = TRUE;
                }
                break;

            // set Ao input path
            case 'i':
                {
                    pu8AoInputPath = (MI_U8*)optarg;
                }
                break;

            // enable ao hpf
            case 'H':
                {
                    bAoEnableVqe = TRUE;
                    bAoEnableHpf = TRUE;
                }
                break;

            // enable ao agc
            case 'G':
                {
                    bAoEnableVqe = TRUE;
                    bAoEnableAgc = TRUE;
                }
                break;

            // enable ao eq
            case 'E':
                {
                    bAoEnableVqe = TRUE;
                    bAoEnableEq = TRUE;
                }
                break;

            // enable ao nr
            case 'N':
                {
                    bAoEnableVqe = TRUE;
                    bAoEnableNr = TRUE;
                }
                break;

            // set Ao Resample rate
            case 'R':
                {
                    bAoEnableResample = TRUE;
                    eAoOutSampleRate = (MI_AUDIO_SampleRate_e)atoi(optarg);
                }
                break;

            // set Ao volume
            case 'V':
                {
                    s32AoVolume = (MI_S32)atoi(optarg);
                    bAoSetVolume = TRUE;
                }
                break;

            case 'D':
                {
                    AoDevId = atoi(optarg);
                }
                break;
			// AI input file for test algorithm
            case 'j':
                {
                    pu8AiTestAlgPath = (MI_U8*)optarg;
                }
                break;
			// AI input file for test Aec algorithm
            case 'J':
                {
                    pu8AiTestAlgForAecPath = (MI_U8*)optarg;
                }
                break;

            case '?':
                if(optopt == 'o')
                {
                    printf("Missing Ai output file path, please -o 'output file path' \n");
                }
                else if(optopt == 'r')
                {
                    printf("Missing Ai output sample of resample, please -r 'output sample rate' \n");
                }
                else if(optopt == 'a')
                {
                    printf("Missing Ai encoder type, please -a 'encoder type' \n");
                    //printf("This demo can't support Aenc and Denc!!!\n");
                }
                else if(optopt == 'c')
                {
                    printf("Missing Ai channel num, please -c 'channel num' \n");
                }
                else if(optopt == 'v')
                {
                    printf("Missing Volume, pleae -v 'volume' \n");
                }
                else if(optopt == 'd')
                {
                    printf("Missing Ai device ID, please -d 'Ai device id' \n");
                }
                else if (optopt == 's')
                {
                    printf("Missing Ai input sample rate, please -s 'input sample rate' \n");
                }
                else if (optopt == 'm')
                {
					printf("Missing Ai sound mode, please -m 'Ai sound mode' \n");
                }
                else if (optopt == 'M')
                {
					printf("Missing Ai mic distance, please -M 'Ai mic distance' \n");
                }
                else if (optopt == 'C')
                {
					printf("Missing Ai Bf doa, please -C 'Ai Bf doa' \n");
                }
                else if (optopt == 'w')
                {
					printf("Missing Ai vqe working sample rate, please -M 'Ai vqe working sample rate' \n");
                }
                else if(optopt == 'i')
                {
                    printf("Missing Ao input file path, please -i 'input file path' \n");
                }
                else if (optopt == 'R')
                {
                    printf("Missing Ao input sample of resample, please -r 'input sample rate'  \n");
                }
                else if(optopt == 'V')
                {
                    printf("Missing Ao volume, pleae -V 'volume' \n");
                }
                else if(optopt == 'D')
                {
                    printf("Missing Ao device ID, please -d 'Ao device id' \n");
                }
                else
                {
                    printf("Invalid option received \n");
                }

            default:
                display_help();
                return -1;
        }
    }

    bCheckPass = checkParam();
    if (FALSE == bCheckPass)
    {
        PRINTF_INFO("Fail to check param.\n");
        display_help();
        return -1;
    }
    else
    {
        initParam();
        printParam();
    }

#if (defined(AI_INPUT_FILE_FOR_TEST_ALG))   //AI input file for test alg
    if(NULL != pu8AiTestAlgPath)
    {
        fpAiTestAlg = fopen(pu8AiTestAlgPath, "rb");
        if(NULL == fpAiTestAlg)
        {
            PRINTF_INFO("fopen AI input file for test alg failed !\n");
            return -1;
        }
        PRINTF_INFO("fopen AI input file for test alg succeed !\n");
        fseek(fpAiTestAlg, sizeof(WaveFileHeader_t), SEEK_SET);

        if(bAiEnableAec && (NULL != pu8AiTestAlgForAecPath))
        {
            fpAiTestAlgForAec = fopen(pu8AiTestAlgForAecPath, "rb");
            if(NULL == fpAiTestAlgForAec)
            {
                PRINTF_INFO("fopen AI input file for test AEC alg failed !\n");
                return -1;
            }
            PRINTF_INFO("fopen AI input file for test AEC alg succeed !\n");
			fseek(fpAiTestAlgForAec, sizeof(WaveFileHeader_t), SEEK_SET);
        }
    }
#endif   //AI input file for test alg

#if (defined(APP_AI_DUMP_AEC_ENABLE))   //AI dump Aec files
    fpAiDumpAecFar = fopen("./DumpAecFar.pcm", "wb+");
    if(NULL == fpAiDumpAecFar)
    {
        PRINTF_INFO("fopen ./DumpAecFar.pcm failed !\n");
        return -1;
    }
    PRINTF_DBG("fopen ./DumpAecFar.pcm succeed !\n");

    fpAiDumpAecNear = fopen("./DumpAecNear.pcm", "wb+");
    if(NULL == fpAiDumpAecNear)
    {
        PRINTF_INFO("fopen ./DumpAecNear.pcm failed !\n");
        return -1;
    }
    PRINTF_DBG("fopen ./DumpAecNear.pcm succeed !\n");

    fpAiDumpAecOutput = fopen("./DumpAecOutput.pcm", "wb+");
    if(NULL == fpAiDumpAecOutput)
    {
        PRINTF_INFO("fopen ./DumpAecOutput.pcm failed !\n");
        return -1;
    }
    PRINTF_DBG("fopen ./DumpAecOutput.pcm succeed !\n");

    fpAiDumpMiFile[0] = fopen("./fpAiDumpMiFile0.pcm", "wb+");
    if(NULL == fpAiDumpMiFile[0])
    {
        PRINTF_INFO("fopen ./fpAiDumpMiFile0.pcm failed !\n");
        return -1;
    }
    PRINTF_DBG("fopen ./fpAiDumpMiFile0.pcm succeed !\n");
    fpAiDumpMiFile[1] = fopen("./fpAiDumpMiFile1.pcm", "wb+");
    if(NULL == fpAiDumpMiFile[1])
    {
        PRINTF_INFO("fopen ./fpAiDumpMiFile1.pcm failed !\n");
        return -1;
    }
    PRINTF_DBG("fopen ./fpAiDumpMiFile1.pcm succeed !\n");
#endif  //AI dump Aec files

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6))
		|| bAiEnableHwAec
#endif
	)
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    ExecFunc(MI_SYS_Init(), MI_SUCCESS);

   	//atexit(freePrivatePoolExit);
    //ExecFunc(allocPrivatePool(), MI_SUCCESS);

    // enable ai
    if (bEnableAI)
    {
        ExecFunc(initAi(), MI_SUCCESS);
        PRINTF_INFO("initAi() done.\n");
    }

    // enable ao
    if (bEnableAO)
    {
        ExecFunc(initAo(), MI_SUCCESS);
        PRINTF_INFO("initAo() done.\n");
    }
    // join ai
    if (bEnableAI)
    {
        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            pthread_join(stAiChnPriv[u32ChnIdx].tid, NULL);
        }
        PRINTF_INFO("join Ai thread done.\n");
    }

    // join ao
    if (bEnableAO)
    {
        pthread_join(Aotid, NULL);
        PRINTF_INFO("join Ao thread done.\n");
    }

    // disable ai
    if (bEnableAI)
    {
        ExecFunc(deinitAi(), MI_SUCCESS);
        PRINTF_INFO("deinitAi() done.\n");
    }

    // disable ao
    if (bEnableAO)
    {
        ExecFunc(deinitAo(), MI_SUCCESS);
        PRINTF_INFO("deinitAo() done.\n");
    }

	//ExecFunc(freePrivatePool(), MI_SUCCESS);
    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
    return 0;
}


