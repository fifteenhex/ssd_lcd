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

#define WAV_PCM  	(0x1)

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
#define AI_DEV_ID_MAX   (7)
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
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
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
#define AI_DEV_DMIC_2CHN     (6)
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
#define AI_VOLUME_2CHN_DMIC_MIN      (-60)
#define AI_VOLUME_2CHN_DMIC_MAX      (30)
#endif

#define AO_VOLUME_MIN           (-60)
#define AO_VOLUME_MAX           (30)

#define MI_AUDIO_SAMPLE_PER_FRAME	(1024)

#define AI_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define MIU_WORD_BYTE_SIZE	(8)
#define USER_BUF_DEPTH      (4)
#define TOTAL_BUF_DEPTH		(8)


typedef enum
{
	E_AI_SOUND_MODE_MONO = 0,
	E_AI_SOUND_MODE_STEREO,
	E_AI_SOUND_MODE_QUEUE,
} AiSoundMode_e;

#define ExecFunc(func, _ret_) \
do{	\
	MI_S32 s32TmpRet;	\
    printf("%d Start test: %s\n", __LINE__, #func);\
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
    printf("%d End test: %s\n", __LINE__, #func);	\
}while(0);

/*=============================================================*/
// Global Variable definition
/*=============================================================*/
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

typedef struct AiOutFilenName_s
{
    MI_AUDIO_DEV AiDevId;
    MI_AUDIO_Attr_t stAiAttr;
} AiOutFileName_t;

typedef struct AiChnPriv_s
{
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;
    MI_S32 s32Fd;
    MI_U32 u32TotalSize;
    MI_U32 u32ChnCnt;
    pthread_t tid;
} AiChnPriv_t;

static MI_BOOL  bEnableAI = FALSE;
static MI_S8*   ps8AiOutputPath = NULL;
static SoundMode_e eAiWavSoundMode = E_SOUND_MODE_MONO;
static AencType_e eAiWavAencType = PCM;
static MI_U32   u32AiChnCnt = 0;
static MI_S32   s32AiVolume = 0;
static MI_BOOL  bAiSetVolume = FALSE;
static MI_AUDIO_DEV AiDevId = -1;
static MI_AUDIO_SampleRate_e eAiWavSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_AUDIO_SampleRate_e eAiSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static AiSoundMode_e eAiSoundMode = E_AI_SOUND_MODE_MONO;

static MI_BOOL  bEnableAO = FALSE;
static MI_U8*   pu8AoInputPath = NULL;
static MI_AUDIO_SampleRate_e eAoInSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_S32   s32AoVolume = 0;
static MI_BOOL  bAoSetVolume = FALSE;
static MI_AUDIO_DEV AoDevId = -1;

static MI_U32   u32RunTime = 0;
static MI_BOOL  bAiExit = FALSE;
static MI_BOOL  bAoExit = FALSE;

static MI_S32   AiChnFd[MI_AUDIO_MAX_CHN_NUM] = {[0 ... MI_AUDIO_MAX_CHN_NUM-1] = -1};

static AiChnPriv_t stAiChnPriv[MI_AUDIO_MAX_CHN_NUM];

static MI_S32   AoReadFd = -1;
static WaveFileHeader_t stWavHeaderInput;
static MI_AO_CHN AoChn = 0;
static MI_S32 s32NeedSize = 0;
static pthread_t Aotid;

MI_U8 u8TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 4];

MI_U32 u32AiDevHeapSize = 0;
MI_U32 u32AiChnOutputHeapSize = 0;
MI_U32 u32AoDevHeapSize = 0;

MI_BOOL bAllocAiDevPool = FALSE;
MI_BOOL bAllocAiChnOutputPool = FALSE;
MI_BOOL bAllocAoDevPool = FALSE;

void display_help(void)
{
    printf("----- audio all test -----\n");
    printf("-t : AI/AO run time(s)\n");
	printf("AI Device Id: Amic[0] Dmic[1] I2S RX[2] Linein[3] "
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
	"Amic+I2S RX[4] Dmic+I2S RX[5]"
#elif (defined(CONFIG_SIGMASTAR_CHIP_M6))
    "ADC2/3[4]"
#elif (defined(CONFIG_SIGMASTAR_CHIP_P3))
    "ADC2[4]"
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
    printf("\n\n");
	printf("AO Device Id: Lineout[0] I2S TX[1]"
#if (defined(CONFIG_SIGMASTAR_CHIP_M6))
        " Dac0[2] Dac1[3] Hdmi+Lineout[4]"
#elif (defined(CONFIG_SIGMASTAR_CHIP_P3))
        " Dac0[2] Dac1[3] HeadPhone[4]"
#endif
    "\n");

    printf("AO test option :\n");
    printf("-O : Enable AO\n");
    printf("-i : AO Input Path\n");
    printf("-D : AO Device Id\n");
    printf("-V : AO Volume\n");
    return;
}

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

MI_BOOL checkParam(void)
{
    MI_BOOL bCheckPass = FALSE;
    do{
        if ((FALSE == bEnableAI) && (FALSE == bEnableAO))
        {
            printf("Neither AI nor AO enabled!!!\n");
            break;
        }

        if (bEnableAI)
        {
            if (NULL == ps8AiOutputPath)
            {
                printf("Ai output path invalid!!!\n");
                break;
            }

            if (AiDevId < 0 || AiDevId > AI_DEV_ID_MAX)
            {
                printf("Ai device id invalid!!!\n");
                break;
            }

			if ((eAiSoundMode != E_AI_SOUND_MODE_MONO)
				&& (eAiSoundMode != E_AI_SOUND_MODE_STEREO)
				&& (eAiSoundMode != E_AI_SOUND_MODE_QUEUE))
			{
				printf("Ai sound mode invalid!!!\n");
				break;
			}

            if (u32AiChnCnt <= 0)
            {
                printf("Ai channel count invalid!!!\n");
                break;
            }

            if (AI_DEV_DMIC == AiDevId)
            {
            	if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
            		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
            	{
                	if ((u32AiChnCnt > AI_DMIC_CHN_MAX)
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M) || defined(CONFIG_SIGMASTAR_CHIP_M6) || defined(CONFIG_SIGMASTAR_CHIP_P3))
						|| (3 == u32AiChnCnt)
#endif
                	)
                	{
                    	printf("Ai channel count invalid!!!\n");
                    	break;
                	}
                }
                else
                {
					if (u32AiChnCnt > AI_DMIC_CHN_MAX / 2)
                	{
                    	printf("Ai channel count invalid!!!\n");
                    	break;
                	}
                }
            }
            else if ((AI_DEV_AMIC == AiDevId)
            			|| (AI_DEV_LINE_IN == AiDevId)
#if (defined(CONFIG_SIGMASTAR_CHIP_M6))
                        || (AI_DEV_AMIC_2_3 == AiDevId)
#elif (defined(CONFIG_SIGMASTAR_CHIP_P3))
                        || (AI_DEV_AMIC_2 == AiDevId)
#endif
            )
            {
            	if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
            		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
            	{
	                if (u32AiChnCnt > AI_AMIC_CHN_MAX)
	                {
	                    printf("Ai channel count invalid!!!\n");
	                    break;
	                }
                }
                else
                {
					if (u32AiChnCnt > AI_AMIC_CHN_MAX / 2)
	                {
	                    printf("Ai channel count invalid!!!\n");
	                    break;
	                }
                }
            }
#if 0
            else if (AI_DEV_I2S_RX == AiDevId)
            {
				if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
				{
					if (AI_I2S_NOR_CHN != u32AiChnCnt)
					{
						printf("Ai channel count invalid!!!\n");
	                    break;
					}
				}
				else
				{
					if (AI_I2S_NOR_CHN / 2 != u32AiChnCnt)
					{
						printf("Ai channel count invalid!!!\n");
	                    break;
					}
				}
            }
#endif
#if (defined(CONFIG_SIGMASTAR_CHIP_I2M))
            else if (AI_DEV_AMIC_AND_I2S_RX == AiDevId)
            {
                if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
			    {
                    if (AI_AMIC_AND_I2S_RX_CHN_MAX != u32AiChnCnt)
                    {
                        printf("Ai channel count invalid!!!\n");
	                    break;
                    }
			    }
			    else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
			    {
                    if (AI_AMIC_AND_I2S_RX_CHN_MAX / 2 != u32AiChnCnt)
                    {
                        printf("Ai channel count invalid!!!\n");
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
                        printf("Ai channel count invalid!!!\n");
	                    break;
                    }
			    }
			    else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
			    {
                    if (AI_DMIC_AND_I2S_RX_CHN_MAX / 2 != u32AiChnCnt)
                    {
                        printf("Ai channel count invalid!!!\n");
	                    break;
					}
				}
            }
#endif
#if (defined(CONFIG_SIGMASTAR_CHIP_P3))
            else if(AI_DEV_DMIC_2CHN == AiDevId)
            {
                if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
					|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
			    {
                    if ((1 != u32AiChnCnt) && (2 != u32AiChnCnt))
                    {
                        printf("Ai channel count invalid!!!\n");
                        printf("P3 Dev%d MONO/QUEUE only support 1/2 chn !\n", AiDevId);
	                    break;
                    }
			    }
			    else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
			    {
                    if (1 != u32AiChnCnt)
                    {
                        printf("Ai channel count invalid!!!\n");
                        printf("P3 Dev%d STEREO only support 1 chn !\n", AiDevId);
	                    break;
                    }
			    }

                if(E_MI_AUDIO_SAMPLE_RATE_48000 == eAiSampleRate)
                {
                    printf("Ai 2Chn Dmic can not support sample rate 48000 !!!\n");
                    break;
                }

                if (TRUE == bAiSetVolume)
                {
                    if ((s32AiVolume < AI_VOLUME_2CHN_DMIC_MIN) || (s32AiVolume > AI_VOLUME_2CHN_DMIC_MAX))
                    {
                        printf("Dev%d Ai volume invalid!!!\n", AiDevId);
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
                printf("AI sample rate invalid!!!\n");
                break;
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
                        printf("Ai volume invalid!!!\n");
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
                        printf("Ai volume invalid!!!\n");
                        break;
                    }
                }

                else if (AI_DEV_LINE_IN == AiDevId)
                {
                    if ((s32AiVolume < AI_VOLUME_AMIC_MIN) || (s32AiVolume > AI_VOLUME_LINEIN_MAX))
                    {
                        printf("Ai volume invalid!!!\n");
                        break;
                    }
                }

                else if (AI_DEV_I2S_RX == AiDevId)
                {
                    printf("I2S RX is not supported volume setting!!!\n");
                    break;
                }
            }
        }

        if (bEnableAO)
        {
            if (NULL == pu8AoInputPath)
            {
                printf("AO input path invalid!!!\n");
                break;
            }

            if ((AoDevId < 0) || (AoDevId > AO_DEV_ID_MAX))
            {
                printf("Ao device id invalid!!!\n");
                break;
            }

            if (TRUE == bAoSetVolume)
            {
                if ((s32AoVolume < AO_VOLUME_MIN) || (s32AoVolume > AO_VOLUME_MAX))
                {
                    printf("AO Volume invalid!!!\n");
                    break;
                }
            }

            if ( NULL == strstr((const char*)pu8AoInputPath, ".wav") )
            {
                printf("Only support wav file.\n");
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
    eAiWavAencType = PCM;

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
        printf("AI OutPut Path:%s\n", ps8AiOutputPath);

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
#if defined(CONFIG_SIGMASTAR_CHIP_P3)
        else if(AiDevId == AI_DEV_DMIC_2CHN)
        {
            printf("2Chn Dmic");
        }
#endif
        else if (AiDevId == AI_DEV_I2S_RX)
        {
            printf("I2S_RX");
        }
        else if (AiDevId == AI_DEV_LINE_IN)
        {
            printf("Linein");
        }

#if (defined(CONFIG_SIGMASTAR_CHIP_I6B0) || defined(CONFIG_SIGMASTAR_CHIP_I6E))
        else if (AiDevId == AI_DEV_I2S_RX_AND_SRC)
        {
            printf("I2S_RX + SRC");
        }
#elif (defined(CONFIG_SIGMASTAR_CHIP_I2M))
        else if (AiDevId == AI_DEV_AMIC_AND_I2S_RX)
        {
            printf("Amic+I2S RX");
        }

        else if (AiDevId == AI_DEV_DMIC_AND_I2S_RX)
        {
            printf("Dmic+I2S RX");
        }
#elif (defined(CONFIG_SIGMASTAR_CHIP_M6))
        else if (AiDevId == AI_DEV_AMIC_2_3)
        {
            printf("Amic2_3");
        }
#elif (defined(CONFIG_SIGMASTAR_CHIP_P3))
        else if (AiDevId == AI_DEV_AMIC_2)
        {
            printf("Amic2");
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
#if (defined(CONFIG_SIGMASTAR_CHIP_M6) || defined(CONFIG_SIGMASTAR_CHIP_P3))
        else if (AoDevId == AO_DEV_DAC0)
        {
            printf("Dac0");
        }
        else if (AoDevId == AO_DEV_DAC1)
        {
            printf("Dac1");
        }
#endif
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

MI_S32 createOutputFileName(AiOutFileName_t *pstAiOutFileName, MI_U32 u32ChnIdx, MI_S8* ps8OutputPath, MI_S8* ps8OutputFileName)
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
    sprintf(as8Tmp, "Chn%d_", u32ChnIdx);
    strcat((char *)ps8OutputFileName, as8Tmp);

    memset(as8Tmp, 0, sizeof(as8Tmp));
    if (AI_DEV_AMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic_");
    }
    else if (AI_DEV_DMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Dmic_");
    }
#if  defined(CONFIG_SIGMASTAR_CHIP_P3)
    else if(AI_DEV_DMIC_2CHN == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Dev%d_Dmic_", pstAiOutFileName->AiDevId);
    }
#endif
    else if (AI_DEV_I2S_RX == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "I2SRx_" );
    }
    else if (AI_DEV_LINE_IN == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "LineIn_" );
    }
#if (defined(CONFIG_SIGMASTAR_CHIP_I6E) \
	|| defined(CONFIG_SIGMASTAR_CHIP_I6B0))
    else if (AI_DEV_I2S_RX_AND_SRC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "I2SRx+Src_" );
    }
#elif (defined(CONFIG_SIGMASTAR_CHIP_I2M))
    else if (AI_DEV_AMIC_AND_I2S_RX == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic+I2SRx_" );
    }
    else if (AI_DEV_DMIC_AND_I2S_RX == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Dmic+I2SRx_" );
    }
#elif (defined(CONFIG_SIGMASTAR_CHIP_M6))
    else if (AI_DEV_AMIC_2_3 == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic2_3_" );
    }
#elif (defined(CONFIG_SIGMASTAR_CHIP_P3))
    else if (AI_DEV_AMIC_2 == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic2_" );
    }
#endif

    strcat((char *)ps8OutputFileName, as8Tmp);

    //set OutpuFile SampleRate
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "%dK", pstAiOutFileName->stAiAttr.eSamplerate / 1000);
    strcat((char *)ps8OutputFileName, as8Tmp);

    //set OutpuFile BitWidth
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "_%dbit", 16);
    strcat((char *)ps8OutputFileName, as8Tmp);

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

    strcat((char *)ps8OutputFileName, as8Tmp);
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, ".wav");
    strcat((char *)ps8OutputFileName, as8Tmp);
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
    WaveFileHeader_t stWavHead;

    memset(&stAiChFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode) || (E_AI_SOUND_MODE_STEREO == eAiSoundMode))
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if (MI_SUCCESS == s32Ret)
            {
                {
	                gettimeofday(&tv_before, NULL);
	                write(priv->s32Fd, stAiChFrame.apVirAddr[0], stAiChFrame.u32Len[0]);
	                gettimeofday(&tv_after, NULL);
	                before_us = tv_before.tv_sec * 1000000 + tv_before.tv_usec;
	                after_us = tv_after.tv_sec * 1000000 + tv_after.tv_usec;
	                if (after_us - before_us > 10 * 1000)
	                {
	                    printf("Chn:%d, cost time:%lldus = %lldms.\n", priv->AiChn, after_us - before_us, (after_us - before_us) / 1000);
	                }
	                priv->u32TotalSize += stAiChFrame.u32Len[0];
                }
                MI_AI_ReleaseFrame(priv->AiDevId, priv->AiChn,  &stAiChFrame,  NULL);
            }
            else
            {
                printf("Dev%dChn%d get frame failed!!!error:0x%x\n", priv->AiDevId, priv->AiChn, s32Ret);
            }
        }
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, eAiWavAencType, eAiWavSoundMode, eAiWavSampleRate, priv->u32TotalSize);
        lseek(priv->s32Fd, 0, SEEK_SET);
        write(priv->s32Fd, &stWavHead, sizeof(WaveFileHeader_t));
        close(priv->s32Fd);
    }
    else
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if (s32Ret == MI_SUCCESS)
            {
                for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
                {
                    write(AiChnFd[u32ChnIndex], stAiChFrame.apVirAddr[u32ChnIndex], stAiChFrame.u32Len[u32ChnIndex]);
                }
                priv->u32TotalSize += stAiChFrame.u32Len[0];
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
    }

    return NULL;
}

void* aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;

    while (FALSE == bAoExit)
    {
        s32Ret = read(AoReadFd, &u8TempBuf, s32NeedSize);
        if (s32Ret != s32NeedSize)
        {
            lseek(AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(AoReadFd, &u8TempBuf, s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }
        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        stAoSendFrame.apVirAddr[0] = u8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do{
            s32Ret = MI_AO_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);
        }while(s32Ret == MI_AO_ERR_NOBUF);

        if (s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
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
		printf("Failed to free private pool!!!\n");
	}
	return;
}


MI_S32 initAi(void)
{
    MI_AUDIO_Attr_t     stAiSetAttr;
    MI_AUDIO_Attr_t     stAiGetAttr;
    AiOutFileName_t     stAiFileName;
    MI_U32              u32ChnIdx;
    MI_U32              u32ChnCnt;
    MI_S8               s8OutputFileName[512];
    MI_S32              s32Ret;
    WaveFileHeader_t    stAiWavHead;
    MI_SYS_ChnPort_t    stAiChnOutputPort0[MI_AUDIO_MAX_CHN_NUM];
#if 0
    MI_AI_ChnParam_t    stAiChnParam;
#endif

    memset(&stAiSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    memset(&stAiGetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = eAiSampleRate;
    stAiSetAttr.eSoundmode = eAiSoundMode;
    stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAiSetAttr.u32ChnCnt = u32AiChnCnt;
    stAiSetAttr.u32PtNumPerFrm = stAiSetAttr.eSamplerate / 16; // for aec
    stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
    stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAiSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 4;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
	{
		eAiWavSoundMode = E_SOUND_MODE_MONO;
	}
	else
	{
		eAiWavSoundMode = E_SOUND_MODE_STEREO;
	}

    memset(&stAiFileName, 0x0, sizeof(AiOutFileName_t));
    stAiFileName.AiDevId = AiDevId;
    memcpy(&stAiFileName.stAiAttr, &stAiSetAttr, sizeof(MI_AUDIO_Attr_t));

    memset(&stAiWavHead, 0x0, sizeof(WaveFileHeader_t));
    u32ChnCnt = u32AiChnCnt;

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        memset(s8OutputFileName, 0x0, sizeof(s8OutputFileName));
        createOutputFileName(&stAiFileName, u32ChnIdx, ps8AiOutputPath, s8OutputFileName);
        printf("OutputName:%s\n", s8OutputFileName);
        AiChnFd[u32ChnIdx] = open((const char *)s8OutputFileName, O_RDWR | O_CREAT, 0777);
        if (AiChnFd[u32ChnIdx] < 0)
        {
            printf("Open output file path:%s fail \n", s8OutputFileName);
            printf("error:%s", strerror(errno));
            return -1;
        }

        s32Ret = write(AiChnFd[u32ChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
        if (s32Ret < 0)
        {
            printf("write wav head failed\n");
            return -1;
        }
    }

    ExecFunc(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS);
    ExecFunc(MI_AI_GetPubAttr(AiDevId, &stAiGetAttr), MI_SUCCESS);
    ExecFunc(MI_AI_Enable(AiDevId), MI_SUCCESS);

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

#if 0
    // for test new API
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    stAiChnParam.stChnGain.bEnableGainSet = TRUE;
    stAiChnParam.stChnGain.s16FrontGain = 0;
    stAiChnParam.stChnGain.s16RearGain = 15;
    ExecFunc(MI_AI_SetChnParam(AiDevId, 0, &stAiChnParam), MI_SUCCESS);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    ExecFunc(MI_AI_GetChnParam(AiDevId, 0, &stAiChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableGainSetting:%d Front Gain:%d, Rear Gain:%d.\n",
            AoDevId, 0, stAiChnParam.stChnGain.bEnableGainSet, stAiChnParam.stChnGain.s16FrontGain,
            stAiChnParam.stChnGain.s16RearGain);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    stAiChnParam.stChnGain.bEnableGainSet = TRUE;
    stAiChnParam.stChnGain.s16FrontGain = 2;
    stAiChnParam.stChnGain.s16RearGain = 30;
    ExecFunc(MI_AI_SetChnParam(AiDevId, 1, &stAiChnParam), MI_SUCCESS);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    ExecFunc(MI_AI_GetChnParam(AiDevId, 1, &stAiChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableGainSetting:%d Front Gain:%d, Rear Gain:%d.\n",
            AoDevId, 1, stAiChnParam.stChnGain.bEnableGainSet, stAiChnParam.stChnGain.s16FrontGain,
            stAiChnParam.stChnGain.s16RearGain);
#endif
    memset(&stAiChnPriv, 0x0, sizeof(stAiChnPriv));
    memset(&stAiChnOutputPort0, 0x0, sizeof(stAiChnOutputPort0));

    if (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        stAiChnPriv[u32ChnIdx].AiChn = u32ChnIdx;
        stAiChnPriv[u32ChnIdx].AiDevId = AiDevId;
        stAiChnPriv[u32ChnIdx].s32Fd = AiChnFd[u32ChnIdx];
        stAiChnPriv[u32ChnIdx].u32ChnCnt = u32AiChnCnt;
        stAiChnPriv[u32ChnIdx].u32TotalSize = 0;

        stAiChnOutputPort0[u32ChnIdx].eModId = E_MI_MODULE_ID_AI;
        stAiChnOutputPort0[u32ChnIdx].u32DevId = AiDevId;
        stAiChnOutputPort0[u32ChnIdx].u32ChnId = u32ChnIdx;
        stAiChnOutputPort0[u32ChnIdx].u32PortId = 0;
        ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort0[u32ChnIdx], USER_BUF_DEPTH, TOTAL_BUF_DEPTH), MI_SUCCESS);
        ExecFunc(MI_AI_EnableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
        pthread_create(&stAiChnPriv[u32ChnIdx].tid, NULL, aiGetChnPortBuf, &stAiChnPriv[u32ChnIdx]);
    }
    printf("create ai thread.\n");
    return 0;

	return s32Ret;
}

MI_S32 initAo(void)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_S32 s32AoGetVolume;
    MI_U32 u32PhyChnNum;
    MI_S32 s32LeftVolumeDb, s32RightVolumeDb;
#if 0
    MI_AO_ChnParam_t stAoChnParam;
#endif

    AoReadFd = open((const char *)pu8AoInputPath, O_RDONLY, 0666);
    if (AoReadFd <= 0)
    {
        printf("Open input file failed:%s \n", pu8AoInputPath);
        printf("error:%s", strerror(errno));
        return -1;
    }

    s32Ret = read(AoReadFd, &stWavHeaderInput, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        printf("Read wav header failed!!!\n");
        return -1;
    }

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 4;
	stAoSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;

    if (stWavHeaderInput.wave.wChannels == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if(stWavHeaderInput.wave.wChannels == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stAoSetAttr.u32ChnCnt = 1;

    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    eAoInSampleRate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;

    ExecFunc(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS);

    ExecFunc(MI_AO_GetPubAttr(AoDevId, &stAoGetAttr), MI_SUCCESS);

    ExecFunc(MI_AO_Enable(AoDevId), MI_SUCCESS);

    ExecFunc(MI_AO_EnableChn(AoDevId, AoChn), MI_SUCCESS);

    if (bAoSetVolume)
    {
#if 1
        ExecFunc(MI_AO_SetVolume(AoDevId, 0, s32AoVolume, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
        ExecFunc(MI_AO_GetVolume(AoDevId, 0, &s32AoGetVolume), MI_SUCCESS);
#else
        s32LeftVolumeDb = s32AoVolume;
        s32RightVolumeDb = s32AoVolume;

        /* add volume left/right channel set API */
        ExecFunc(MI_AO_SetLRVolume(AoDevId, 0, s32LeftVolumeDb, s32RightVolumeDb, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
        ExecFunc(MI_AO_GetLRVolume(AoDevId, 0, &s32LeftVolumeDb, &s32RightVolumeDb), MI_SUCCESS);

        printf("Ao get volume info:Left=%d  Right=%d\n", s32LeftVolumeDb, s32RightVolumeDb);
#endif
    }

#if 0
    // for test new API
    memset(&stAoChnParam, 0x0, sizeof(MI_AO_ChnParam_t));
    stAoChnParam.stChnGain.bEnableGainSet = TRUE;
    stAoChnParam.stChnGain.s16Gain = -10;
    ExecFunc(MI_AO_SetChnParam(AoDevId, 0, &stAoChnParam), MI_SUCCESS);
    memset(&stAoChnParam, 0x0, sizeof(MI_AO_ChnParam_t));
    ExecFunc(MI_AO_GetChnParam(AoDevId, 0, &stAoChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableSetting:%d gain:%d.\n",
            AoDevId, 0, stAoChnParam.stChnGain.bEnableGainSet,
            stAoChnParam.stChnGain.s16Gain);

    memset(&stAoChnParam, 0x0, sizeof(MI_AO_ChnParam_t));
    stAoChnParam.stChnGain.bEnableGainSet = TRUE;
    stAoChnParam.stChnGain.s16Gain = 5;
    ExecFunc(MI_AO_SetChnParam(AoDevId, 1, &stAoChnParam), MI_SUCCESS);
    memset(&stAoChnParam, 0x0, sizeof(MI_AO_ChnParam_t));
    ExecFunc(MI_AO_GetChnParam(AoDevId, 1, &stAoChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableSetting:%d gain:%d.\n",
            AoDevId, 1, stAoChnParam.stChnGain.bEnableGainSet,
            stAoChnParam.stChnGain.s16Gain);
#endif
    switch (eAiSoundMode)
    {
        case E_MI_AUDIO_SOUND_MODE_MONO:
            u32PhyChnNum = 1;
            break;

        case E_MI_AUDIO_SOUND_MODE_STEREO:
            u32PhyChnNum = 2;
            break;

        default:
            break;
    }
    s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * u32PhyChnNum * stAoSetAttr.u32ChnCnt;
    s32NeedSize = s32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * u32PhyChnNum) * (stAoSetAttr.u32ChnCnt * 2 * u32PhyChnNum);
    pthread_create(&Aotid, NULL, aoSendFrame, NULL);
    printf("create ao thread.\n");
	return 0;

	return s32Ret;
}

MI_S32 deinitAi(void)
{
    MI_U32 u32ChnIdx;
    MI_U32 u32ChnCnt;

    if (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
    }
    ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);
    return 0;
}

MI_S32 deinitAo(void)
{
    ExecFunc(MI_AO_DisableChn(AoDevId, AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);

    return 0;
}

int main (int argc, char *argv[])
{
    //MI_S32      s32Ret = MI_SUCCESS;
    MI_S32      s32Opt = 0;
    MI_BOOL     bCheckPass = FALSE;
    MI_U32      u32ChnIdx;
    MI_U32      u32ChnCnt;

    // parsing command line
    while ((s32Opt = getopt(argc, argv, "t:I::o:c:v:d:s:m:O::i:V:D:")) != -1)
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
                    ps8AiOutputPath = (MI_S8*)optarg;
                }
                break;

            // set Ai chn num
            case 'c':
                {
                    u32AiChnCnt = atoi(optarg);
                }
                break;

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

            case '?':
                if(optopt == 't')
                {
                    printf("Missing run time, please -t 'run time second' \n");
                }
                else if(optopt == 'o')
                {
                    printf("Missing Ai output file path, please -o 'output file path' \n");
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
                else if(optopt == 'i')
                {
                    printf("Missing Ao input file path, please -i 'input file path' \n");
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
        printf("Fail to check param.\n");
        display_help();
        return -1;
    }
    else
    {
        initParam();
        printParam();
    }

    if (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)
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
    }

    // enable ao
    if (bEnableAO)
    {
        ExecFunc(initAo(), MI_SUCCESS);
    }

    // join ai
    if (bEnableAI)
    {
        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            pthread_join(stAiChnPriv[u32ChnIdx].tid, NULL);
        }
        printf("join Ai thread done.\n");
    }

    // join ao
    if (bEnableAO)
    {
        pthread_join(Aotid, NULL);
        printf("join Ao thread done.\n");
    }

    // disable ai
    if (bEnableAI)
    {
        ExecFunc(deinitAi(), MI_SUCCESS);
    }

    // disable ao
    if (bEnableAO)
    {
        ExecFunc(deinitAo(), MI_SUCCESS);
    }

	//ExecFunc(freePrivatePool(), MI_SUCCESS);
    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
    return 0;
}
