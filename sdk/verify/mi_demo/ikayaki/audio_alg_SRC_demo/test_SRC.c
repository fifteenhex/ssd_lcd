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

#include "AudioSRCProcess.h"

/* 0:Fixed input file  1:User input file */
#define IN_PARAMETER 1

int main(int argc, char *argv[])
{
    SRCStructProcess src_struct;
    memset(&src_struct, 0, sizeof(SRCStructProcess));
    /*********************User change section start*******************/
    /*
    *  Audio file bit wide:The user modifies as needed,
    *  for example:16, 8
    */
    int srcFileBitWide = 16;
    /*
    *  The user modifies as needed,for example:1, 2
    */
    src_struct.channel = 1;
    /*
    *  The user modifies as needed,for example:
    *  SRATE_8K, SRATE_16K, SRATE_32K, SRATE_48K
    */
    src_struct.WaveIn_srate = SRATE_8K;
    /*
    *  The user modifies as needed,for example:
    *  SRC_8k_to_16k, SRC_8k_to_32k, SRC_48k_to_8k ...
    */
    src_struct.mode = SRC_8k_to_48k;
    /*
    *  The user modifies as needed,for example:
    *  256, 512, 1024 and 1536 (Please select among these values)
    */
    src_struct.point_number = 1536;
    /*********************User change section end*******************/
    SrcConversionMode mode = src_struct.mode;
    SRC_HANDLE handle;
    int ret;
    int output_size;
    unsigned int workingBufferSize;
    char *workingBufferAddress = NULL;
    int freadBufferSize;
    char *freadBuffer = NULL;
    int fwriteBufferSize;
    char *fwriteBuffer = NULL;
    int nBytesRead;
    int nPoints = src_struct.point_number;
    int MaxFactor = SRATE_48K / SRATE_8K;//The biggest factor
    FILE* fpIn;  //input file
    FILE* fpOut; //output file
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
    sprintf(src_file, "%s", "./8K_16bit_MONO_30s.wav");
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
    //(1)IaaSrc_GetBufferSize
    workingBufferSize =  IaaSrc_GetBufferSize(mode);
    workingBufferAddress = (char *)malloc(sizeof(char) * workingBufferSize);
    if(NULL == workingBufferAddress)
    {
        printf("malloc SRC workingBuffer failed !\n");
        return -1;
    }
    printf("malloc SRC workingBuffer success !\n");
    //(2)IaaSrc_Init
    handle = IaaSrc_Init(workingBufferAddress, &src_struct);
    if(NULL == handle)
    {
        printf("SRC:IaaSrc_Init  failed !\n");
        return -1;
    }
    printf("SRC:IaaSrc_Init  success !\n");
    freadBufferSize = src_struct.channel * nPoints * srcFileBitWide / 8;
    freadBuffer = (char *)malloc(freadBufferSize);
    if(NULL == freadBuffer)
    {
        printf("malloc freadBuffer failed !\n");
        return -1;
    }
    printf("malloc freadBuffer success !\n");
    fwriteBufferSize = MaxFactor * freadBufferSize;
    fwriteBuffer = (char *)malloc(fwriteBufferSize);
    if(NULL == fwriteBuffer)
    {
        printf("malloc fwriteBuffer failed !\n");
        return -1;
    }
    printf("malloc fwriteBuffer success !\n");
#if 0
    /*Consider whether the input file has a header*/
    fread(freadBuffer, sizeof(char), 44, fpIn); //Remove the 44 bytes header
#endif
    while(1)
    {
        nBytesRead = fread(freadBuffer, 1, freadBufferSize, fpIn);
        if(nBytesRead != freadBufferSize)
        {
            printf("needBytes =%d      nBytesRead = %d\n", freadBufferSize, nBytesRead);
            break;
        }
        //(3)IaaSrc_Run
        ret = IaaSrc_Run(handle, (short *)freadBuffer, (short *)fwriteBuffer, &output_size);
        if(ret < 0)
        {
            printf("SRC:IaaSrc_Run  failed !\n");
            return -1;
        }
        //printf("ret = %d\n", ret);
        fwriteBufferSize = src_struct.channel * output_size * srcFileBitWide / 8;
        fwrite(fwriteBuffer, 1, fwriteBufferSize, fpOut);
    }
    fclose(fpIn);
    fclose(fpOut);
    //(4)IaaSrc_Release
    ret = IaaSrc_Release(handle);
    if(ret)
    {
        printf("IaaSrc_Release failed !\n");
        return -1;
    }
    printf("IaaSrc_Release success !\n");
    free(workingBufferAddress);
    free(freadBuffer);
    free(fwriteBuffer);

    printf("SRC end!\n");

    return 0;
}

