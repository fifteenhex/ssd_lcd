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

#include "AudioAedProcess.h"

/*0:Fixed input file  1:User input file*/
#define IN_PARAMETER 1

int main(int argc, char *argv[])
{
    AedHandle AED_HANDLE;
    AedProcessStruct aed_params;
    unsigned int aedBuffSize;
    char *working_buf_ptr = NULL;
    /************************User change section start**************************/
    int vad_threshold_db = -40;
    int lsd_threshold_db = -15;
    int operating_point = -10;
    AedSensitivity sensitivity = AED_SEN_HIGH;
    AedSampleRate sample_rate = AED_SRATE_8K;
    memset(&aed_params, 0, sizeof(AedProcessStruct));
    aed_params.channel = 1;
    /************************User change section end***************************/
    short input[1024];
    char src_file[128] = {0};
    FILE * fpIn;
    ALGO_AED_RET ret = ALGO_AED_RET_SUCCESS;
    int bcd_result;
    int lsd_result;
    int frm_cnt = 0;
    int lsd_db = 0; //useless param.

#if IN_PARAMETER
    if(argc < 2)
    {
        printf("Please enter the correct parameters!\n");
        return -1;
    }
    sscanf(argv[1], "%s", src_file);
#else
    sprintf(src_file, "%s", "./AFE_8K_MONO.wav");
#endif

    switch (sample_rate)
    {
        case AED_SRATE_8K:
            aed_params.point_number = 256;
            break;
        case AED_SRATE_16K:
            aed_params.point_number = 512;
            break;
        case AED_SRATE_32K:
            aed_params.point_number = 1024;
            break;
        case AED_SRATE_48K:
            aed_params.point_number = 1535;
            break;
        default:
            printf("Unsupported current sample rate !\n");
            return -1;
    }
    aedBuffSize = IaaAed_GetBufferSize();
    working_buf_ptr = (char *)malloc(aedBuffSize);
    if(NULL == working_buf_ptr)
    {
        printf("malloc workingBuffer failed !\n");
        return -1;
    }
    printf("malloc workingBuffer succeed !\n");
    AED_HANDLE = IaaAed_Init(working_buf_ptr, &aed_params);
    if(NULL == AED_HANDLE)
    {
        printf("IaaAed_Init faild !\n");
        return -1;
    }
    printf("IaaAed_Init succeed !\n");
    ret = IaaAed_Config(AED_HANDLE);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_Config failed !, ret = %d\n", ret);
        return -1;
    }
    ret = IaaAed_SetSensitivity(AED_HANDLE, sensitivity);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_SetSensitivity failed !, ret = %d\n", ret);
        return -1;
    }
    ret = IaaAed_SetSampleRate(AED_HANDLE, sample_rate);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_SetSampleRate failed !, ret = %d\n", ret);
        return -1;
    }
    ret = IaaAed_SetOperatingPoint(AED_HANDLE, operating_point);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_SetOperatingPoint failed !, ret = %d\n", ret);
        return -1;
    }
    ret = IaaAed_SetLsdThreshold(AED_HANDLE, lsd_threshold_db);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_SetLsdThreshold failed !, ret = %d\n", ret);
        return -1;
    }
    ret = IaaAed_SetVadThreshold(AED_HANDLE, vad_threshold_db);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_SetVadThreshold failed !, ret = %d\n", ret);
        return -1;
    }

    fpIn = fopen(src_file, "rb");
    if(NULL == fpIn)
    {
        printf("fopen in_file failed !\n");
        return -1;
    }
    printf("fopen in_file success !\n");
    memset(input, 0, sizeof(short) * 1024);
#if 1
    /*Consider whether the input file has a header*/
    fread(input, sizeof(char), 44, fpIn); // Remove the 44 bytes header
#endif
    while(fread(input, sizeof(short), aed_params.point_number * aed_params.channel, fpIn))
    {
        frm_cnt++;
        /* Run LSD process */
        ret = IaaAed_RunLsd(AED_HANDLE, input, &lsd_db);
        if (ALGO_AED_RET_SUCCESS != ret)
        {
            printf("MI_AED_RunLsd failed !, ret = %d\n", ret);
            break;
        }
        ret = IaaAed_GetLsdResult(AED_HANDLE, &lsd_result);
        if(ALGO_AED_RET_SUCCESS != ret)
        {
            printf("IaaAed_GetLsdResult failed !, ret = %d\n", ret);
        }
        if (lsd_result)
        {
            printf("current time = %f, loud sound detected!\n", \
            frm_cnt * ((float)aed_params.point_number / sample_rate));
        }
        /* Run AED process */
        ret = IaaAed_Run(AED_HANDLE, input);
        if(ALGO_AED_RET_SUCCESS != ret)
        {
            printf("MI_AED_Run failed !,ret = %d\n", ret);
            break;
        }
        ret = IaaAed_GetResult(AED_HANDLE, &bcd_result);
        if(ALGO_AED_RET_SUCCESS != ret)
        {
            printf("IaaAed_GetResult failed !, ret = %d\n", ret);
        }
        if(bcd_result)
        {
            printf("Baby cried at %.3f.\n", \
            frm_cnt * ((float)aed_params.point_number / sample_rate));
        }
    }
    printf("AED end !\n");
    ret = IaaAed_Release(AED_HANDLE);
    if(ALGO_AED_RET_SUCCESS != ret)
    {
        printf("IaaAed_Release failed !, ret = %d\n", ret);
    }
    fclose(fpIn);
    return 0;
}

