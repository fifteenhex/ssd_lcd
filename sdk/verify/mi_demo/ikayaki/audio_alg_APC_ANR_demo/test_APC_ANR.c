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
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "AudioProcess.h"

/*0:Fixed input file  1:User input file*/
#define IN_PARAMETER 1

int main(int argc, char *argv[])
{
    short in_output[1024];
    unsigned int workingBufferSize;
    char *workingBuffer = NULL;
    ANR_HANDLE handle;
    AudioProcessInit anr_init, anr_get_init;
    AudioAnrConfig anr_config, anr_get_config;
    ALGO_APC_RET ret;
    int tempSize;
    FILE* fpIn;  //input file
    FILE* fpOut; //output file
    char src_file[128] = {0};
    char dst_file[128] = {0};
    /*********************User change section start*******************/
    int intensity_band[6] = {3,24,40,64,80,128};
    int intensity[7] = {30,30,30,30,30,30,30};
    memset(&anr_init, 0, sizeof(AudioProcessInit));
    anr_init.point_number = 128;
    anr_init.channel = 1;
    anr_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;

    memset(&anr_config, 0, sizeof(AudioAnrConfig));
    anr_config.anr_enable = 1;
    anr_config.user_mode = 2;
    anr_config.anr_smooth_level = 10;
    anr_config.anr_converge_speed = 0;
    /*********************User change section end*******************/
    memcpy(anr_config.anr_intensity_band, intensity_band, sizeof(intensity_band));
    memcpy(anr_config.anr_intensity, intensity, sizeof(intensity));
    //(1)IaaAnr_GetBufferSize
    workingBufferSize = IaaAnr_GetBufferSize();
    workingBuffer = (char *)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("malloc workingBuffer failed !\n");
        return -1;
    }
    printf("malloc workingBuffer succeed !\n");
    //(2)IaaAnr_Init
    handle = IaaAnr_Init(workingBuffer, &anr_init);
    if(NULL == handle)
    {
        printf("IaaAnr_Init failed !\n");
        return -1;
    }
    printf("IaaAnr_Init succeed !\n");
    //(3)IaaAnr_Config
    ret = IaaAnr_Config(handle, &anr_config);
    if(ret)
    {
        printf("IaaAnr_Config failed !\n");
        return -1;
    }
    printf("IaaAnr_Config succeed !\n");
    //(4)IaaAnr_GetConfig
    memset(&anr_get_init, 0, sizeof(AudioProcessInit));
    memset(&anr_get_config, 0, sizeof(AudioAnrConfig));
    ret = IaaAnr_GetConfig(handle, &anr_get_init, &anr_get_config);
    if(ret)
    {
        printf("IaaAnr_GetConfig failed !\n");
        return -1;
    }
    printf("IaaAnr_GetConfig succeed !\n");
    printf("anr_get_config.user_mode = %d, ...\n", anr_get_config.user_mode);

#if IN_PARAMETER
    if(argc < 3)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", src_file);
    sscanf(argv[2], "%s", dst_file);
#else
    sprintf(src_file, "%s", "./APC_AFE_16K_MONO.wav");
    if(argc < 2)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", dst_file);
#endif

    fpIn = fopen(src_file, "rb");
    if(NULL == fpIn)
    {
        printf("fopen in_file failed !\n");
        return -1;
    }
    printf("fopen in_file success !\n");
    fpOut = fopen(dst_file, "wb");
    if(NULL == fpOut)
    {
        printf("fopen out_file failed !\n");
        return -1;
    }
    printf("fopen out_file success !\n");
#if 1
    fread(in_output, sizeof(char), 44, fpIn);
    fwrite(in_output, sizeof(char), 44, fpOut);
#endif
    tempSize = anr_init.point_number * anr_init.channel;
    while(tempSize == fread(in_output, sizeof(short), tempSize, fpIn))
    {
        //(5)IaaAnr_Run
        ret = IaaAnr_Run(handle, in_output);
        if(ret)
        {
            printf("IaaAnr_Run failed !\n");
            return -1;
        }
        fwrite(in_output, sizeof(short), tempSize, fpOut);
    }
    printf("Break:needBytes =%d \t nBytesRead = %d\n", anr_init.point_number * anr_init.channel, tempSize);
    //(6)IaaAnr_Free
    IaaAnr_Free(handle);
    free(workingBuffer);
    fclose(fpIn);
    fclose(fpOut);
    printf("APC_ANR end !\n");

    return 0;
}
