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
#include <string.h>

#include "AudioSslProcess.h"

/*0:Fixed input file  1:User input file*/
#define IN_PARAMETER 1

int main(int argc, char *argv[])
{
    AudioSslInit ssl_init;
    AudioSslConfig ssl_config;
    memset(&ssl_init, 0, sizeof(AudioSslInit));
    memset(&ssl_config, 0, sizeof(AudioSslConfig));
    /**************User change section start**********************/
    /* The user modifies as needed,for example:5, 6(Unit: cm) */
    ssl_init.mic_distance = 5;
    /* The user modifies as needed,for example:128, ... */
    ssl_init.point_number = 128;
    /* The user modifies as needed */
    ssl_init.sample_rate = 48000;
    /* The user modifies as needed */
    ssl_init.bf_mode = 0;
    /* The user modifies as needed */
    ssl_config.temperature = 25;
    /* The user modifies as needed */
    ssl_config.noise_gate_dbfs = -40;
    /* The user modifies as needed */
    ssl_config.direction_frame_num = 100;
    /**************User change section end**********************/
    unsigned int workingBufferSize;
    SSL_HANDLE handle;
    ALGO_SSL_RET ret = ALGO_SSL_RET_SUCCESS;
    int delay_sample = 0;
    int direction;
    int counter = 0;
    int frame_number = ssl_config.direction_frame_num;
    short input[1024];
    char *workingBufferAddress = NULL;
    FILE* fpIn;  //input file
    char src_file[128] = {0};

#if IN_PARAMETER
    if(argc < 2)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", src_file);
#else
    sprintf(src_file, "%s", "./mic34_gain1_ssong_continue_48k_stereo.wav");
#endif

    fpIn = fopen(src_file, "rb");
    if(NULL == fpIn)
    {
        printf("fopen in_file failed !\n");
        return -1;
    }
    printf("\nfopen in_file success !\n");
    //(1)IaaSsl_GetBufferSize
    workingBufferSize = IaaSsl_GetBufferSize();
    workingBufferAddress = (char *)malloc(sizeof(char) * workingBufferSize);
    if(NULL == workingBufferAddress)
    {
        printf("malloc SSL workingBuffer failed !\n");
        return -1;
    }
    printf("malloc SSL workingBuffer success !\n");
    //(2)IaaSsl_Init
    handle = IaaSsl_Init(workingBufferAddress, &ssl_init);
    if(NULL == handle)
    {
        printf("SSL:IaaSsl_Init  failed !\n");
        return -1;
    }
    printf("SSL:IaaSsl_Init  success !\n");
    //(3)IaaSsl_Config
    ret = IaaSsl_Config(handle, &ssl_config);
    if(ALGO_SSL_RET_SUCCESS != ret)
    {
        printf("SSL:IaaSsl_Config  failed !, ret = %d\n", ret);
        return -1;
    }
    printf("SSL:IaaSsl_Config  success !\n");
    ret = IaaSsl_Get_Config(handle, &ssl_config);
    if(ALGO_SSL_RET_SUCCESS != ret)
    {
        printf("IaaSsl_Get_Config failed !, ret = %d\n", ret);
    }
    printf("IaaSsl_Get_Config succeed !\n");
    printf("ssl_config.temperature = %u\n", ssl_config.temperature);
    printf("ssl_config.noise_gate_dbfs = %d\n", ssl_config.noise_gate_dbfs);
    printf("ssl_config.direction_frame_num = %d\n", ssl_config.direction_frame_num);
#if 1
    /*Consider whether the input file has a header*/
    fread(input, sizeof(char), 44, fpIn); // Remove the 44 bytes header
#endif
    while(1)
    {
        /*2 is the number of channels*/
        ret = fread(input, sizeof(short), ssl_init.point_number * 2, fpIn);
        if(ret != ssl_init.point_number * 2)
        {
            printf("break:endReadSize = The remaining size = %d.\n", ret);
            break;
        }
        //(4)IaaSsl_Run
        ret = IaaSsl_Run(handle, input, &delay_sample);
        if(ALGO_SSL_RET_SUCCESS != ret)
        {
            printf("SSL:IaaSsl_Run  failed !, ret = %d\n", ret);
        }
        counter ++;
        if(counter == frame_number && ssl_init.bf_mode == 0)
        {
            //(5)IaaSsl_Get_Direction
            ret = IaaSsl_Get_Direction(handle, &direction);
            if(ALGO_SSL_RET_SUCCESS != ret)
            {
                printf("SSL:IaaSsl_Get_Direction  failed !, ret = %d\n", ret);
            }
            printf("The current direction:%d \n", direction);
            handle = IaaSsl_Reset(handle, &ssl_init);
            if(NULL == handle)
            {
                printf("SSL:IaaSsl_Reset failed !\n");
                return -1;
            }
            ret = IaaSsl_Config(handle, &ssl_config);
            if(ALGO_SSL_RET_SUCCESS != ret)
            {
                printf("SSL:IaaSsl_Config failed !, ret = %d\n", ret);
                return -1;
            }
            counter = 0;
        }
    }
    printf("Explain:If the current direction is not \
    in the -90~90 range, consult the API documentation.\n");
    //(6)IaaSsl_Free
    ret = IaaSsl_Free(handle);
    if(ALGO_SSL_RET_SUCCESS != ret)
    {
        printf("IaaSsl_Free failed !, ret = %d\n", ret);
    }
    free(workingBufferAddress);
    fclose(fpIn);
    printf("SSL end !\n");
    return 0;
}

