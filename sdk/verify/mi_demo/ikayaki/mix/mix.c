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

#define MI_AUDIO_SAMPLE_PER_FRAME	(1024)

#define USER_BUF_DEPTH      (2)
#define TOTAL_BUF_DEPTH		(8)

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

static MI_AUDIO_DEV AiDevId = 4;// ADC2
static MI_AUDIO_DEV AoDevId = 5;// HEADPHONE + Lineout

static MI_BOOL  bAiExit = FALSE;
static MI_BOOL  bAoExit = FALSE;

static MI_S32   AoReadFd = -1;
static WaveFileHeader_t stWavHeaderInput;
static MI_AO_CHN AoChn = 0;
static MI_S32 s32NeedSize = 0;
static pthread_t Aotid;
static MI_AUDIO_SampleRate_e eSampleRate;

MI_S16 s16TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 2];
MI_S16 s16TempBufL[MI_AUDIO_SAMPLE_PER_FRAME];
MI_S16 s16TempBufR[MI_AUDIO_SAMPLE_PER_FRAME];

MI_U32 u32AiDevHeapSize = 0;
MI_U32 u32AiChnOutputHeapSize = 0;
MI_U32 u32AoDevHeapSize = 0;

MI_BOOL bAllocAiDevPool = FALSE;
MI_BOOL bAllocAiChnOutputPool = FALSE;
MI_BOOL bAllocAoDevPool = FALSE;

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


void initParam(void)
{
    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);
    return;
}

void* aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32SampleIndex;
    MI_AUDIO_Frame_t stAoSendFrame;
    MI_U32 u32ChnCnt = 2, u32SampleByte = 2;
    MI_S16 *pTmpBuf = NULL;

    while (FALSE == bAoExit)
    {
        memset(s16TempBuf, 0, sizeof(s16TempBuf));
        memset(s16TempBufL, 0, sizeof(s16TempBufL));
        memset(s16TempBufR, 0, sizeof(s16TempBufR));

        pTmpBuf = s16TempBuf;

        s32Ret = read(AoReadFd, &s16TempBuf, s32NeedSize);
        if (s32Ret != s32NeedSize)
        {
            lseek(AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(AoReadFd, &s16TempBuf, s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }

        for (u32SampleIndex = 0; u32SampleIndex < s32NeedSize / u32ChnCnt / u32SampleByte; u32SampleIndex++)
        {
            s16TempBufL[u32SampleIndex] = *pTmpBuf++;
            s16TempBufR[u32SampleIndex] = *pTmpBuf++;
        }

        memset(s16TempBufL, 0, sizeof(s16TempBufL));
        memset(s16TempBufR, 0, sizeof(s16TempBufR));

        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32NeedSize / u32ChnCnt;
        stAoSendFrame.u32Len[1] = s32NeedSize / u32ChnCnt;
        stAoSendFrame.apVirAddr[0] = s16TempBufL;
        stAoSendFrame.apVirAddr[1] = s16TempBufR;

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

MI_S32 initAi(void)
{
    MI_AUDIO_Attr_t     stAiSetAttr;
    MI_SYS_ChnPort_t    stAiChnOutputPort;

    memset(&stAiSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = eSampleRate;
    stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    //stAiSetAttr.u32ChnCnt = 2;
    stAiSetAttr.u32ChnCnt = 1;
    stAiSetAttr.u32PtNumPerFrm = E_MI_AUDIO_SAMPLE_RATE_48000 / 16;

    ExecFunc(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS);
    ExecFunc(MI_AI_Enable(AiDevId), MI_SUCCESS);
    ExecFunc(MI_AI_SetVqeVolume(AiDevId, 0, 12), MI_SUCCESS);
    //ExecFunc(MI_AI_SetVqeVolume(AiDevId, 1, 6), MI_SUCCESS);

    memset(&stAiChnOutputPort, 0x0, sizeof(stAiChnOutputPort));
    stAiChnOutputPort.eModId = E_MI_MODULE_ID_AI;
    stAiChnOutputPort.u32DevId = AiDevId;
    stAiChnOutputPort.u32ChnId = 0;
    stAiChnOutputPort.u32PortId = 0;
    ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort, USER_BUF_DEPTH, TOTAL_BUF_DEPTH), MI_SUCCESS);

    memset(&stAiChnOutputPort, 0x0, sizeof(stAiChnOutputPort));
    stAiChnOutputPort.eModId = E_MI_MODULE_ID_AI;
    stAiChnOutputPort.u32DevId = AiDevId;
    stAiChnOutputPort.u32ChnId = 1;
    stAiChnOutputPort.u32PortId = 0;
    //ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort, USER_BUF_DEPTH, TOTAL_BUF_DEPTH), MI_SUCCESS);
    ExecFunc(MI_AI_EnableChn(AiDevId, 0), MI_SUCCESS);
    //ExecFunc(MI_AI_EnableChn(AiDevId, 1), MI_SUCCESS);

    return 0;

}

MI_S32 initAo(void)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr;

    AoReadFd = open((const char *)"mix.wav", O_RDONLY, 0666);
    if (AoReadFd <= 0)
    {
        printf("Open input file failed.\n");
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
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
    stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    stAoSetAttr.u32ChnCnt = stWavHeaderInput.wave.wChannels;
    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    eSampleRate = stAoSetAttr.eSamplerate;
    if ((eSampleRate != 8000) && (eSampleRate != 16000) && (eSampleRate != 32000) && (eSampleRate != 48000))
    {
        printf("The demo only supports 8/16/32/48KHz sample rate.\n");
        exit(-1);
    }

    ExecFunc(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS);
    ExecFunc(MI_AO_Enable(AoDevId), MI_SUCCESS);
    ExecFunc(MI_AO_EnableChn(AoDevId, 0), MI_SUCCESS);
    ExecFunc(MI_AO_EnableChn(AoDevId, 1), MI_SUCCESS);
    ExecFunc(MI_AO_SetVolume(AoDevId, 0, 0, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
    ExecFunc(MI_AO_SetVolume(AoDevId, 1, 0, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);

    s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * stAoSetAttr.u32ChnCnt;
    s32NeedSize = s32NeedSize / (stAoSetAttr.u32ChnCnt * 2) * (stAoSetAttr.u32ChnCnt * 2);
    pthread_create(&Aotid, NULL, aoSendFrame, NULL);
    printf("create ao thread.\n");
	return 0;
}

MI_S32 deinitAi(void)
{
    ExecFunc(MI_AI_DisableChn(AiDevId, 0), MI_SUCCESS);
    //ExecFunc(MI_AI_DisableChn(AiDevId, 1), MI_SUCCESS);
    ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);
    return 0;
}

MI_S32 deinitAo(void)
{
    ExecFunc(MI_AO_DisableChn(AoDevId, 0), MI_SUCCESS);
    ExecFunc(MI_AO_DisableChn(AoDevId, 1), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);

    return 0;
}

int main (int argc, char *argv[])
{
    initParam();
    ExecFunc(MI_SYS_Init(), MI_SUCCESS);

    // enable ao
    ExecFunc(initAo(), MI_SUCCESS);

    // enable ai
    ExecFunc(initAi(), MI_SUCCESS);

    // join ao
    pthread_join(Aotid, NULL);
    printf("join Ao thread done.\n");

    // disable ao
    ExecFunc(deinitAo(), MI_SUCCESS);

    // disable ai
    ExecFunc(deinitAi(), MI_SUCCESS);

    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
    return 0;
}
