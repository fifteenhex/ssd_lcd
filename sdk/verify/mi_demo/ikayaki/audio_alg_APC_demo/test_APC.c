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
    unsigned int T = ((1000000 * t1.tv_sec) + t1.tv_usec) / 1000;
    return T;
}

int main(int argc, char *argv[])
{
    short in_output[1024];
    unsigned int T0, T1;
    float avg = 0;
    char src_file[128];
    char dst_file[128];
    int counter = 0;
    unsigned int workingBufferSize;
    char *workingBuffer = NULL;
    AudioApcBufferConfig apc_switch;
    FILE *fpIn, * fpOut;
    ALGO_APC_RET ret;
    AudioProcessInit apc_init;
    AudioAnrConfig anr_config;
    AudioEqConfig eq_config;
    AudioHpfConfig hpf_config;
    AudioAgcConfig agc_config;
    APC_HANDLE handle;
    /****************************User change section start**********************************/
    int intensity_band[6] = {3,24,40,64,80,128};
    int intensity[7] = {30,30,30,30,30,30,30};
    short eq_table[129];
    memset(eq_table, 0, sizeof(eq_table));
    short compression_ratio_input[7] = {-65,-55,-48,-25,-18,-12,0};
    short compression_ratio_output[7] = {-65,-50,-27,-12,-1,-1,-1};

    memset(&apc_switch, 0, sizeof(AudioApcBufferConfig));
    apc_switch.anr_enable = 1;
    apc_switch.eq_enable = 1;
    apc_switch.agc_enable = 1;

    memset(&apc_init, 0, sizeof(AudioProcessInit));
    apc_init.point_number = 128;
    apc_init.channel = 1;
    apc_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;

    /******ANR Config*******/
    memset(&anr_config, 0, sizeof(AudioAnrConfig));
    anr_config.anr_enable = apc_switch.anr_enable;
    anr_config.user_mode = 2;
    memcpy(anr_config.anr_intensity_band, intensity_band, sizeof(intensity_band));
    memcpy(anr_config.anr_intensity, intensity, sizeof(intensity));
    anr_config.anr_smooth_level = 10;
    anr_config.anr_converge_speed = 0;
    /******EQ Config********/
    memset(&eq_config, 0, sizeof(AudioEqConfig));
    eq_config.eq_enable = apc_switch.eq_enable;
    eq_config.user_mode = 1;
    memcpy(eq_config.eq_gain_db, eq_table, sizeof(eq_table));
    /******HPF Config********/
    memset(&hpf_config, 0, sizeof(AudioHpfConfig));
    hpf_config.hpf_enable = apc_switch.eq_enable;
    hpf_config.user_mode = 1;
    hpf_config.cutoff_frequency = AUDIO_HPF_FREQ_150;
    /******AGC Config********/
    memset(&agc_config, 0, sizeof(AudioAgcConfig));
    agc_config.agc_enable = apc_switch.agc_enable;
    agc_config.user_mode = 1;
    agc_config.gain_info.gain_max  = 40;
    agc_config.gain_info.gain_min  = -10;
    agc_config.gain_info.gain_init = 12;
    agc_config.drop_gain_max = 36;
    agc_config.attack_time = 1;
    agc_config.release_time = 1;
    agc_config.noise_gate_db = -80;
	agc_config.gain_step = 1;
    memcpy(agc_config.compression_ratio_input, compression_ratio_input, sizeof(compression_ratio_input));
    memcpy(agc_config.compression_ratio_output, compression_ratio_output, sizeof(compression_ratio_output));
    agc_config.noise_gate_attenuation_db = 0;
    agc_config.drop_gain_threshold = -5;
    /****************************User change section end***********************************/
    //(1)IaaApc_GetBufferSize
    workingBufferSize = IaaApc_GetBufferSize(&apc_switch);
    workingBuffer = (char*)malloc(workingBufferSize);
    if(NULL == workingBuffer)
    {
        printf("malloc workingBuffer failed !\n");
        return -1;
    }
    printf("malloc workingBuffer succeed !\n");
    //(2)IaaApc_Init
    handle = IaaApc_Init(workingBuffer, &apc_init, &apc_switch);
    if(NULL == handle)
    {
        printf("IaaApc_Init failed !\n");
        return -1;
    }
    printf("IaaApc_Init succeed !\n");
    //(3)IaaApc_Config
    if(IaaApc_Config(handle, &anr_config, &eq_config, &hpf_config, NULL, NULL, &agc_config))
    {
        printf("IaaApc_Config failed !\n");
        return -1;
    }
    printf("IaaApc_Config succeed !\n");

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
        printf("src_file open failed !\n");
        return -1;
    }
    printf("src_file open succeed !\n");
    fpOut = fopen(dst_file, "wb");
    if(NULL == fpOut)
    {
        printf("dst_file open failed !\n");
        return -1;
    }
    printf("dst_file open succeed !\n");
#if 1
    fread(in_output, sizeof(char), 44, fpIn);
    fwrite(in_output, sizeof(char), 44, fpOut);
#endif
    while(fread(in_output, sizeof(short), apc_init.point_number * apc_init.channel, fpIn))
    {
        counter++;
        T0  = (long)_OsCounterGetMs();
        ret = IaaApc_Run(handle, in_output);
        T1  = (long)_OsCounterGetMs();
        avg += (T1 - T0);

        if(counter%1000 == 999)
        {
            printf("counter = %d\n", counter);
            printf("current time = %f\n", (float)counter * apc_init.point_number / apc_init.sample_rate);
            printf("process time = %lu(ms)\t", (long)(T1 - T0));
        }
        if(ret)
        {
            printf("Error occured in NoiseReduct\n");
            break;
        }
        fwrite(in_output, sizeof(short), apc_init.point_number * apc_init.channel, fpOut);
    }
    avg /= counter;
    printf("AVG is %.2f ms\n", avg);
    IaaApc_Free(handle);
    free(workingBuffer);
    fclose(fpIn);
    fclose(fpOut);
    printf("APC end !\n");

    return 0;
}
