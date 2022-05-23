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
#include <sys/time.h>
#include <stdlib.h>

#include "AudioBfProcess.h"

/*0:Fixed input file  1:User input file*/
#define IN_PARAMETER 1

int main(int argc, char *argv[])
{
    short in_output[1024];
    char *workingBuffer = NULL;
    unsigned int workingBufferSize;
    ALGO_BF_RET ret = ALGO_BF_RET_SUCCESS;
    int tempSize;
    int delay_sample = 0;
    AudioBfInit bf_init;
    AudioBfConfig bf_config, bf_get_config;
    BF_HANDLE handle;
    FILE* fpIn;
    FILE* fpOut;
    char src_file[128] = {0};
    char dst_file[128] = {0};

#if IN_PARAMETER
    if(argc < 3)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", src_file);
    sscanf(argv[2], "%s", dst_file);
#else
    sprintf(src_file, "%s", "./12cm_2mic_8k_0degree.wav");
    if(argc < 2)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", dst_file);
#endif

    /****User change section start****/
    memset(&bf_init, 0, sizeof(AudioBfInit));
    bf_init.mic_distance = 12;
    bf_init.point_number = 128;
    bf_init.sample_rate = 8000;
    bf_init.channel = 2;

    memset(&bf_config, 0, sizeof(AudioBfConfig));
    bf_config.noise_gate_dbfs = -40;
    bf_config.temperature = 25;
    bf_config.noise_supression_mode = 8;
    bf_config.noise_estimation = 1;
    bf_config.output_gain = 0.7;
    bf_config.vad_enable = 0;

    /****User change section end ****/
    //(1)IaaBf_GetBufferSize
    workingBufferSize = IaaBf_GetBufferSize();
    workingBuffer = (char*)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("workingBuffer malloc failed !\n");
        return -1;
    }
    //(2)IaaBf_Init
    handle = IaaBf_Init(workingBuffer, &bf_init);
    if (NULL == handle)
    {
        printf("IaaBf_Init failed !\n");
        return -1;
    }
    printf("IaaBf_Init succeed !\n");
    //(3)IaaBf_Config
    ret = IaaBf_Config(handle, &bf_config);
    if (ALGO_BF_RET_SUCCESS != ret)
    {
        printf("IaaBf_Config failed !, ret = %d\n", ret);
        return -1;
    }
    printf("IaaBf_Config succeed !\n");
    //(4)IaaBf_Get_Config
    memset(&bf_get_config, 0, sizeof(AudioBfConfig));
    ret = IaaBf_Get_Config(handle, &bf_get_config);
    if(ALGO_BF_RET_SUCCESS != ret)
    {
        printf("IaaBf_Get_Config dailed !, ret = %d\n", ret);
        return -1;
    }
    printf("IaaBf_Get_Config succeed !\n");
    printf("bf_get_config.noise_gate_dbfs = %d\n", bf_get_config.noise_gate_dbfs);
    printf("bf_get_config.temperature = %u\n", bf_get_config.temperature);
    printf("bf_get_config.noise_supression_mode = %d\n", bf_get_config.noise_supression_mode);
    printf("bf_get_config.noise_estimation = %d\n", bf_get_config.noise_estimation);
    printf("bf_get_config.output_gain = %f\n", bf_get_config.output_gain);
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

    fread(in_output, sizeof(char), 44, fpIn);
#if 0
    fwrite(in_output, sizeof(char), 44, fpOut);
#endif
    tempSize = bf_init.point_number * bf_init.channel;
    while(tempSize == fread(in_output, sizeof(short), tempSize, fpIn))
    {
        //(5)IaaBf_Run
        ret = IaaBf_Run(handle, in_output, &delay_sample);
        if(ALGO_BF_RET_SUCCESS != ret)
        {
            printf("IaaBf_Run failed !\n");
            return -1;
        }
        fwrite(in_output, sizeof(short), tempSize/bf_init.channel, fpOut);
    }
    //(6)IaaBf_Free
    ret = IaaBf_Free(handle);
    if(ALGO_BF_RET_SUCCESS != ret)
    {
        printf("IaaBf_Free failed !, ret = %d\n", ret);
        return -1;
    }
    printf("IaaBf_Free succeed !\n");
    fclose(fpIn);
    fclose(fpOut);
    free(workingBuffer);
    printf("BF end !\n");

    return 0;
}
