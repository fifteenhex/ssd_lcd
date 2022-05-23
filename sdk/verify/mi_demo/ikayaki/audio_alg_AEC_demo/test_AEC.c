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

#include "AudioAecProcess.h"

/*0:Fixed input file  1:User input file*/
#define IN_PARAMETER 1

float AVERAGE_RUN(int a)
{
    static unsigned int num = 0;
    static float avg = 0;
    if(0 == num)
        avg = 0;
    num++;
    avg = avg + ((float)a - avg) / ((float)num);
    return avg;
}

unsigned int _OsCounterGetMs(void)
{
    struct  timeval t1;
    gettimeofday(&t1, NULL);
    unsigned int T = ((1000000 * t1.tv_sec)+ t1.tv_usec) / 1000;
    return T;
}

int main(int argc, char *argv[])
{
    short in_output[1024];
    short input_far[1024];
    char src_file1[128] = {0};
    char src_file2[128] = {0};
    char dst_file[128] = {0};
    unsigned int workingBufferSize;
    char *workingBuffer = NULL;
    int counter = 0;
    int ret;
    int tempSize;
    unsigned int T0, T1;
    float avg;
    FILE *fpInFar, *fpOut, *fpInNear;
    AudioAecInit aec_init;
    AudioAecConfig aec_config;
    AEC_HANDLE handle;
    /*********************User change section start*******************/
    unsigned int supMode_band[6] = {20,40,60,80,100,120};
    unsigned int supMode[7] = {4,4,4,4,4,4,4};
    memset(&aec_init, 0, sizeof(AudioAecInit));
    aec_init.point_number = 128;
    aec_init.nearend_channel = 1;
    aec_init.farend_channel = 1;
    aec_init.sample_rate = IAA_AEC_SAMPLE_RATE_16000;
    memset(&aec_config, 0, sizeof(AudioAecConfig));
    aec_config.delay_sample = 0;
    aec_config.comfort_noise_enable = IAA_AEC_FALSE;
    /*********************User change section end*******************/
    memcpy(&(aec_config.suppression_mode_freq[0]), supMode_band, sizeof(supMode_band));
    memcpy(&(aec_config.suppression_mode_intensity[0]), supMode, sizeof(supMode));
    //(1)IaaAec_GetBufferSize
    workingBufferSize = IaaAec_GetBufferSize();
    workingBuffer = (char*)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("workingBuffer malloc failed !\n");
        return -1;
    }
    printf("workingBuffer malloc success !\n");
    //(2)IaaAec_Init
    handle = IaaAec_Init(workingBuffer, &aec_init);
    if (NULL == handle)
    {
        printf("AEC init failed !\r\n");
        return -1;
    }
    printf("AEC init success !\r\n");
    //(3)IaaAec_Config
    ret = IaaAec_Config(handle, &aec_config);
    if(ret)
    {
        printf("IaaAec_Config failed !\n");
        return -1;
    }
    printf("IaaAec_Config succeed !\n");

#if IN_PARAMETER
    if(argc < 4)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", src_file1);
    sscanf(argv[2], "%s", src_file2);
    sscanf(argv[3], "%s", dst_file);
#else
    sprintf(src_file1, "%s", "./farend_16K_MONO.wav");
    sprintf(src_file2, "%s", "./nearend_16K_MONO.wav");
    if(argc < 2)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", dst_file);
#endif

    fpInFar = fopen(src_file1, "rb");
    if(NULL == fpInFar)
    {
        printf("src_file1 open failed !\n");
        return -1;
    }
    printf("src_file1 open succeed !\n");
    fpInNear = fopen(src_file2, "rb");
    if(NULL == fpInNear)
    {
        printf("src_file2 open failed !\n");
        return -1;
    }
    printf("src_file2 open succeed !\n");
    fpOut = fopen(dst_file, "wb");
    if(NULL == fpOut)
    {
        printf("dst_file open failed !\n");
        return -1;
    }
    printf("dst_file open succeed !\n");
#if 1
    fread(in_output, sizeof(char), 44, fpInNear);  // Remove the 44 bytes header
    fwrite(in_output, sizeof(char), 44, fpOut);  //New file add header
    fread(input_far, sizeof(char), 44, fpInFar); // Remove the 44 bytes header
#endif
    tempSize = aec_init.point_number * aec_init.nearend_channel;
    while(fread(in_output, sizeof(short), tempSize, fpInNear))
    {
        tempSize = aec_init.point_number * aec_init.farend_channel;
        fread(input_far, sizeof(short), tempSize, fpInFar);
        counter++;
        T0  = (long)_OsCounterGetMs();
        //(4)IaaAec_Run
        ret = IaaAec_Run(handle, in_output, input_far);
        T1  = (long)_OsCounterGetMs();
        avg = AVERAGE_RUN(T1 - T0);
        if(0 == counter % 100)
        {
            printf("counter = %d\n", counter);
            printf("current time = %f\n", (float)counter * aec_init.point_number / aec_init.sample_rate);
            printf("process time = %lu(ms)\t", (long)(T1 - T0));
            printf("AVG is %.2f ms\n", avg);
        }
        if(ret < 0)
        {
            printf("IaaAec_Run failed !\n");
            break;
        }
        tempSize = aec_init.point_number * aec_init.nearend_channel;
        fwrite(in_output, sizeof(short), tempSize, fpOut);
    }
    //(5)IaaAec_Free
    IaaAec_Free(handle);
    fclose(fpInFar);
    fclose(fpInNear);
    fclose(fpOut);
    free(workingBuffer);
    printf("AEC end !\n");

    return 0;
}
