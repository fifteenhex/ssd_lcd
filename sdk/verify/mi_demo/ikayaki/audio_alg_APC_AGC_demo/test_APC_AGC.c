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
    AGC_HANDLE handle;
    AudioProcessInit agc_init, agc_get_init;
    AudioAgcConfig agc_config, agc_get_config;
    ALGO_APC_RET ret;
    int tempSize;
    FILE* fpIn;  //input file
    FILE* fpOut; //output file
    char src_file[128] = {0};
    char dst_file[128] = {0};
    /*********************User change section start*******************/
    short compression_ratio_input[7] = {-65,-55,-48,-25,-18,-12,0};
    short compression_ratio_output[7] = {-65,-50,-27,-12,-1,-1,-1};
    memset(&agc_init, 0, sizeof(AudioProcessInit));
    agc_init.point_number = 128;
    agc_init.channel = 1;
    agc_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;

    memset(&agc_config, 0, sizeof(AudioAgcConfig));
    agc_config.agc_enable = 1;
    agc_config.user_mode = 1;
    agc_config.gain_info.gain_max  = 40;
    agc_config.gain_info.gain_min  = -10;
    agc_config.gain_info.gain_init = 12;
    agc_config.drop_gain_max = 36;
    agc_config.attack_time = 1;
    agc_config.release_time = 1;
    agc_config.noise_gate_db = -80;
    agc_config.noise_gate_attenuation_db = 0;
    agc_config.drop_gain_threshold = -5;
	agc_config.gain_step = 1;
    /*********************User change section end*******************/
    memcpy(agc_config.compression_ratio_input, compression_ratio_input, sizeof(compression_ratio_input));
    memcpy(agc_config.compression_ratio_output, compression_ratio_output, sizeof(compression_ratio_output));
    //(1)IaaAgc_GetBufferSize
    workingBufferSize = IaaAgc_GetBufferSize();
    workingBuffer = (char *)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("malloc workingBuffer failed !\n");
        return -1;
    }
    printf("malloc workingBuffer succeed !\n");
    //(2)IaaAgc_Init
    handle = IaaAgc_Init(workingBuffer, &agc_init);
    if(NULL == handle)
    {
        printf("IaaAgc_Init failed !\n");
        return -1;
    }
    printf("IaaAgc_Init succeed !\n");
    //(3)IaaAgc_Config
    ret = IaaAgc_Config(handle, &agc_config);
    if(ret)
    {
        printf("IaaAgc_Config failed !\n");
        return -1;
    }
    printf("IaaAgc_Config succeed !\n");
    //(4)IaaAgc_GetConfig
    memset(&agc_get_init, 0, sizeof(AudioProcessInit));
    memset(&agc_get_config, 0, sizeof(AudioAgcConfig));
    ret = IaaAgc_GetConfig(handle, &agc_get_init, &agc_get_config);
    if(ret)
    {
        printf("IaaAgc_GetConfig failed !\n");
        return -1;
    }
    printf("IaaAgc_GetConfig succeed !\n");
    printf("agc_get_config.user_mode = %d, ...\n", agc_get_config.user_mode);

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
    tempSize = agc_init.point_number * agc_init.channel;
    while(tempSize == fread(in_output, sizeof(short), tempSize, fpIn))
    {
        //(5)IaaAgc_Run
        ret = IaaAgc_Run(handle, in_output);
        if(ret)
        {
            printf("IaaAnr_Run failed !\n");
            return -1;
        }
        fwrite(in_output, sizeof(short), tempSize, fpOut);
    }
    printf("Break:needBytes =%d \t nBytesRead = %d\n", agc_init.point_number * agc_init.channel, tempSize);
    //(6)IaaAgc_Free
    IaaAgc_Free(handle);
    free(workingBuffer);
    fclose(fpIn);
    fclose(fpOut);
    printf("APC_AGC end !\n");

    return 0;
}
