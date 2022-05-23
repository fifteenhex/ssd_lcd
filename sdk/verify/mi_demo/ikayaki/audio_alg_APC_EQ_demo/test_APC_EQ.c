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
    EQ_HANDLE handle;
    AudioProcessInit eq_init, eq_get_init;
    AudioHpfConfig hpf_config, hpf_get_config;
    AudioEqConfig  eq_config, eq_get_config;
    ALGO_APC_RET ret;
    int tempSize;

    FILE* fpIn;  //input file
    FILE* fpOut; //output file
    char src_file[128] = {0};
    char dst_file[128] = {0};
    /*********************User change section start*******************/
    short eq_table[129];
    memset(eq_table, 0, sizeof(eq_table));
    memset(&eq_init, 0, sizeof(AudioProcessInit));
    eq_init.point_number = 128;
    eq_init.channel = 1;
    eq_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;

    memset(&hpf_config, 0, sizeof(AudioHpfConfig));
    hpf_config.hpf_enable = 1;
    hpf_config.user_mode = 1;
    hpf_config.cutoff_frequency = AUDIO_HPF_FREQ_150;

    memset(&eq_config, 0, sizeof(AudioEqConfig));
    eq_config.eq_enable = 1;
    eq_config.user_mode = 1;
    /*********************User change section end*******************/
    memcpy(eq_config.eq_gain_db, eq_table, sizeof(eq_table));
    //(1)IaaEq_GetBufferSize
    workingBufferSize = IaaEq_GetBufferSize();
    workingBuffer = (char *)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("malloc workingBuffer failed !\n");
        return -1;
    }
    printf("malloc workingBuffer succeed !\n");
    //(2)IaaEq_Init
    handle = IaaEq_Init(workingBuffer, &eq_init);
    if(NULL == handle)
    {
        printf("IaaEq_Init failed !\n");
        return -1;
    }
    printf("IaaEq_Init succeed !\n");
    //(3)IaaEq_Config
    ret = IaaEq_Config(handle, &hpf_config, &eq_config);
    if(ret)
    {
        printf("IaaEq_Config failed !\n");
        return -1;
    }
    printf("IaaEq_Config succeed !\n");
    //(4)IaaEq_GetConfig
    memset(&eq_get_init, 0, sizeof(AudioProcessInit));
    memset(&hpf_get_config, 0, sizeof(AudioHpfConfig));
    memset(&eq_get_config, 0, sizeof(AudioEqConfig));
    ret = IaaEq_GetConfig(handle, &eq_get_init, &hpf_get_config, &eq_get_config);
    if(ret)
    {
        printf("IaaEq_GetConfig failed !\n");
        return -1;
    }
    printf("IaaEq_GetConfig succeed !\n");
    printf("eq_get_config.user_mode = %d, ...\n", eq_get_config.user_mode);

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
    tempSize = eq_init.point_number * eq_init.channel;
    while(tempSize == fread(in_output, sizeof(short), tempSize, fpIn))
    {
        //(5)IaaEq_Run
        ret = IaaEq_Run(handle, in_output);
        if(ret)
        {
            printf("IaaEq_Run failed !\n");
            return -1;
        }
        fwrite(in_output, sizeof(short), tempSize, fpOut);
    }
    printf("Break:needBytes =%d \t nBytesRead = %d\n", eq_init.point_number * eq_init.channel, tempSize);
    //(6)IaaEq_Free
    IaaEq_Free(handle);
    free(workingBuffer);
    fclose(fpIn);
    fclose(fpOut);
    printf("APC_EQ end !\n");

    return 0;
}
