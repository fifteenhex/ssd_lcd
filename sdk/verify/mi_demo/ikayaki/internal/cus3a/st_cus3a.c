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
#include "mi_isp.h"
#include "isp_cus3a_if.h"

#define log_info 1

int ae_init(void* pdata, ISP_AE_INIT_PARAM *init_state)
{
    printf("****** ae_init ,shutter=%d,shutter_step=%d,sensor_gain_min=%d,sensor_gain_max=%d *******\n",
            init_state->shutter,
            init_state->shutter_step,
            init_state->sensor_gain,
            init_state->sensor_gain_max
          );
    return 0;
}

void ae_release(void* pdata)
{
    printf("************* ae_release *************\n");
}

void ae_run(void* pdata, const ISP_AE_INFO *info, ISP_AE_RESULT *result)
{
    unsigned int max = info->AvgBlkY*info->AvgBlkX;
    unsigned int avg=0;
    unsigned int n;

    result->Change              = 0;
    result->u4BVx16384          = 16384;
    result->HdrRatio            = 10;
    result->IspGain             = 1024;
    result->SensorGain          = 4096;
    result->Shutter             = 20000;
    result->IspGainHdrShort     = 1024;
    result->SensorGainHdrShort  = 1024;
    result->ShutterHdrShort     = 1000;
    //result->Size         = sizeof(CusAEResult_t);

    for(n=0;n<max;++n)
    {
        avg += info->avgs[n].y;
    }
    avg /= max;

    result->AvgY         = avg;

    unsigned int y_lower = 0x48;
    unsigned int y_upper = 0x58;
    unsigned int change_ratio = 6; // percentage
    unsigned int Gain_Min = 1024*2;
    unsigned int Gain_Max = 1024*1000;
    unsigned int Shutter_Min = 150;
    unsigned int Shutter_Max = 33333;

    result->SensorGain = info->SensorGain;
    result->Shutter = info->Shutter;

    if(avg<y_lower){
        if (info->Shutter<Shutter_Max){
            result->Shutter = info->Shutter + (info->Shutter*change_ratio/100);
            if (result->Shutter > Shutter_Max) result->Shutter = Shutter_Max;
        }else{
            result->SensorGain = info->SensorGain + (info->SensorGain*change_ratio/100);
            if (result->SensorGain > Gain_Max) result->SensorGain = Gain_Max;
        }
        result->Change = 1;
    }else if(avg>y_upper){
        if (info->SensorGain>Gain_Min){
            result->SensorGain = info->SensorGain - (info->SensorGain*change_ratio/100);
            if (result->SensorGain < Gain_Min) result->SensorGain = Gain_Min;
        }else{
            result->Shutter = info->Shutter - (info->Shutter*change_ratio/100);
            if (result->Shutter < Shutter_Min) result->Shutter = Shutter_Min;
        }
        result->Change = 1;
    }
    #if log_info
        printf("Image avg = 0x%X \n", avg);
        printf("SensorGain: %d -> %d \n", info->SensorGain, result->SensorGain);
        printf("Shutter: %d -> %d \n", info->Shutter, result->Shutter);
    #endif
}

int awb_init(void *pdata)
{
    printf("************ awb_init **********\n");
    return 0;
}

void awb_run(void* pdata, const ISP_AWB_INFO *info, ISP_AWB_RESULT *result)
{
    static MI_U32 count = 0;
    int avg_r = 0;
    int avg_g = 0;
    int avg_b = 0;
    int tar_rgain = 1024;
    int tar_bgain = 1024;
    int x = 0;
    int y = 0;

    result->R_gain = info->CurRGain;
    result->G_gain = info->CurGGain;
    result->B_gain = info->CurBGain;
    result->Change = 0;
    result->ColorTmp = 6000;

    if (++count % 4 == 0)
    {
        //center area YR/G/B avg
        for (y = 30; y<60; ++y)
        {
            for (x = 32; x<96; ++x)
            {
                avg_r += info->avgs[info->AvgBlkX*y + x].r;
                avg_g += info->avgs[info->AvgBlkX*y + x].g;
                avg_b += info->avgs[info->AvgBlkX*y + x].b;
            }
        }
        avg_r /= 30 * 64;
        avg_g /= 30 * 64;
        avg_b /= 30 * 64;

        if (avg_r <1)
            avg_r = 1;
        if (avg_g <1)
            avg_g = 1;
        if (avg_b <1)
            avg_b = 1;

#if log_info
        printf("AVG R / G / B = %d, %d, %d \n", avg_r, avg_g, avg_b);
#endif

        // calculate Rgain, Bgain
        tar_rgain = avg_g * 1024 / avg_r;
        tar_bgain = avg_g * 1024 / avg_b;

        if (tar_rgain > info->CurRGain) {
            if (tar_rgain - info->CurRGain < 384)
                result->R_gain = tar_rgain;
            else
                result->R_gain = info->CurRGain + (tar_rgain - info->CurRGain)/10;
        }else{
            if (info->CurRGain - tar_rgain < 384)
                result->R_gain = tar_rgain;
            else
                result->R_gain = info->CurRGain - (info->CurRGain - tar_rgain)/10;
        }

        if (tar_bgain > info->CurBGain) {
            if (tar_bgain - info->CurBGain < 384)
                result->B_gain = tar_bgain;
            else
                result->B_gain = info->CurBGain + (tar_bgain - info->CurBGain)/10;
        }else{
            if (info->CurBGain - tar_bgain < 384)
                result->B_gain = tar_bgain;
            else
                result->B_gain = info->CurBGain - (info->CurBGain - tar_bgain)/10;
        }

        result->Change = 1;
        result->G_gain = 1024;

#if log_info
        printf("[current] r=%d, g=%d, b=%d \n", info->CurRGain, info->CurGGain, info->CurBGain);
        printf("[result] r=%d, g=%d, b=%d \n", result->R_gain, result->G_gain, result->B_gain);
#endif
    }
}

void awb_release(void *pdata)
{
    printf("************ awb_release **********\n");
}

int af_init(void *pdata, ISP_AF_INIT_PARAM *param)
{
    MI_U32 u32ch = 0;
    MI_U8 u8win_idx = 16;
    CusAFRoiMode_t taf_roimode;

    printf("************ af_init **********\n");

#if 1
    //Init Normal mode setting
    taf_roimode.mode = AF_ROI_MODE_NORMAL;
    taf_roimode.u32_vertical_block_number = 1;
    MI_ISP_CUS3A_SetAFRoiMode(u32ch, &taf_roimode);

    static CusAFWin_t afwin[16] =
    {
        // x_start need to start from 16

        //{ 0, {   16,    0,  1023,  1023}},  //for full image
        { 0, {  16,    0,  255,  255}},
        { 1, { 256,    0,  511,  255}},
        { 2, { 512,    0,  767,  255}},
        { 3, { 768,    0, 1023,  255}},
        { 4, {  16,  256,  255,  511}},
        { 5, { 256,  256,  511,  511}},
        { 6, { 512,  256,  767,  511}},
        { 7, { 768,  256, 1023,  511}},
        { 8, {  16,  512,  255,  767}},
        { 9, { 256,  512,  511,  767}},
        {10, { 512,  512,  767,  767}},
        {11, { 768,  512, 1023,  767}},
        {12, {  16,  768,  255, 1023}},
        {13, { 256,  768,  511, 1023}},
        {14, { 512,  768,  767, 1023}},
        {15, { 768,  768, 1023, 1023}}
    };
    for(u8win_idx = 0; u8win_idx < 16; ++u8win_idx)
    {
        MI_ISP_CUS3A_SetAFWindow(u32ch, &afwin[u8win_idx]);
    }
#endif

#if 1 //TARGET_CHIP_I6E
    //set AF Filter
    static CusAFFilter_t affilter =
    {
        //filter setting with sign value
        //{s9, s10, s9, s7, s7}
        //high[0.1~0.6]  : 37, 0, -37, -107, 49, 37, 0, -37, 25, 28, 32, 0, -32, -41, 0
        //low[0.03~0.25]: 19, 0, -19, -122, 59, 19, 0, -19, -73, 36, 17, 0, -17, -91, 30

        //convert to hw format (sign bit with msb)
        37, 0, 37+512, 107+128, 49, 0, 1023, 0, 1023,   //low
        19, 0, 19+512, 122+128, 59, 0, 1023, 0, 1023,   //high
        1, 37, 0, 37+512,      25, 28, 1, 32, 0, 32+512,  41+128, 0,
        1, 19, 0, 19+512,  73+128, 36, 1, 17, 0, 17+512,  91+128, 30,
    };
#endif

    MI_ISP_CUS3A_SetAFFilter(0, &affilter);

    //set AF Sq
    CusAFFilterSq_t sq = {
        .bSobelYSatEn = 0,
        .u16SobelYThd = 1023,
        .bIIRSquareAccEn = 1,
        .bSobelSquareAccEn = 0,
        .u16IIR1Thd = 0,
        .u16IIR2Thd = 0,
        .u16SobelHThd = 0,
        .u16SobelVThd = 0,
        .u8AFTblX = {6,7,7,6,6,6,7,6,6,7,6,6,},
        .u16AFTblY = {0,32,288,800,1152,1568,2048,3200,3872,4607,6271,7199,8191},
    };


    MI_ISP_CUS3A_SetAFFilterSq(0, &sq);
    printf("****[%s] af_init done ****\n", __FUNCTION__);

    return 0;
}

void af_release(void *pdata)
{
    printf("****[%s] af_release ****\n", __FUNCTION__);
}

/*
    MI_U8 iir_1[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 iir_2[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 luma[4*16];   //[4]: luma 32bit, use 4*u8 datatype, [16]: 16wins
    MI_U8 fir_v[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 fir_h[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 ysat[3*16];   //[3]: ysat 22bit, use 3*u8 datatype,  [16]: 16wins
*/

void af_run(void *pdata, const ISP_AF_INFO *af_info, ISP_AF_RESULT *result)
{
#if log_info
    int i = 0, x = 0;

    //printf("\n\n");

    //print row0 16wins
    x = 0;
    for (i = 0; i < 16; i++)
    {
        printf("\n[AF]win%d-%d iir0:0x%02x%02x%02x%02x%02x, iir1:0x%02x%02x%02x%02x%02x, luma:0x%02x%02x%02x%02x, sobelh:0x%02x%02x%02x%02x%02x, sobelv:0x%02x%02x%02x%02x%02x ysat:0x%02x%02x%02x",
               x, i,
               af_info->pStats->stParaAPI[x].high_iir[4 + i * 5], af_info->pStats->stParaAPI[x].high_iir[3 + i * 5], af_info->pStats->stParaAPI[x].high_iir[2 + i * 5], af_info->pStats->stParaAPI[x].high_iir[1 + i * 5], af_info->pStats->stParaAPI[x].high_iir[0 + i * 5],
               af_info->pStats->stParaAPI[x].low_iir[4 + i * 5], af_info->pStats->stParaAPI[x].low_iir[3 + i * 5], af_info->pStats->stParaAPI[x].low_iir[2 + i * 5], af_info->pStats->stParaAPI[x].low_iir[1 + i * 5], af_info->pStats->stParaAPI[x].low_iir[0 + i * 5],
               af_info->pStats->stParaAPI[x].luma[3 + i * 4], af_info->pStats->stParaAPI[x].luma[2 + i * 4], af_info->pStats->stParaAPI[x].luma[1 + i * 4], af_info->pStats->stParaAPI[x].luma[0 + i * 4],
               af_info->pStats->stParaAPI[x].sobel_h[4 + i * 5], af_info->pStats->stParaAPI[x].sobel_h[3 + i * 5], af_info->pStats->stParaAPI[x].sobel_h[2 + i * 5], af_info->pStats->stParaAPI[x].sobel_h[1 + i * 5], af_info->pStats->stParaAPI[x].sobel_h[0 + i * 5],
               af_info->pStats->stParaAPI[x].sobel_v[4 + i * 5], af_info->pStats->stParaAPI[x].sobel_v[3 + i * 5], af_info->pStats->stParaAPI[x].sobel_v[2 + i * 5], af_info->pStats->stParaAPI[x].sobel_v[1 + i * 5], af_info->pStats->stParaAPI[x].sobel_v[0 + i * 5],
               af_info->pStats->stParaAPI[x].ysat[2 + i * 3], af_info->pStats->stParaAPI[x].ysat[1 + i * 3], af_info->pStats->stParaAPI[x].ysat[0 + i * 3]
              );
    }
#endif
}

int af_ctrl(void *pdata, ISP_AF_CTRL_CMD cmd, void* param)
{
    return 0;
}

int g_Cus3AInited = 0;
void ST_EnableCustomize3A(void)
{
    //if(bEn)
    {
        MI_U32 u32Select = 0;
        ISP_AE_INTERFACE tAeIf;
        ISP_AWB_INTERFACE tAwbIf;
        ISP_AF_INTERFACE tAfIf;

        ISP_AE_INTERFACE *pAeIf = NULL;
        ISP_AWB_INTERFACE *pAwbIf = NULL;
        ISP_AF_INTERFACE *pAfIf = NULL;

        if(!g_Cus3AInited)
        {
            CUS3A_Init();
            g_Cus3AInited = 1;
        }

        /*AE*/
        printf("Enable AE? 1:Enable, 0:Disable\n");
        scanf("%d", &u32Select);
        if(u32Select==1)
        {
            tAeIf.ctrl = NULL;
            tAeIf.pdata = NULL;
            tAeIf.init = ae_init;
            tAeIf.release = ae_release;
            tAeIf.run = ae_run;
            pAeIf = &tAeIf;
        }
        else
        {
            pAeIf = NULL;
        }

        /*AWB*/
        printf("Enable AWB? 1:Enable, 0:Disable\n");
        scanf("%d", &u32Select);
        if(u32Select==1)
        {
            tAwbIf.ctrl = NULL;
            tAwbIf.pdata = NULL;
            tAwbIf.init = awb_init;
            tAwbIf.release = awb_release;
            tAwbIf.run = awb_run;
            pAwbIf = &tAwbIf;
        }
        else
        {
            pAwbIf = NULL;
        }

        /*AF*/
        printf("Enable AF? 1:Enable, 0:Disable\n");
        scanf("%d", &u32Select);
        if(u32Select==1)
        {
            tAfIf.ctrl = af_ctrl;
            tAfIf.pdata = NULL;
            tAfIf.init = af_init;
            tAfIf.release = af_release;
            tAfIf.run = af_run;
            pAfIf = &tAfIf;
        }
        else
        {
            pAfIf = NULL;
        }

        CUS3A_RegInterface(0,pAeIf, pAwbIf, pAfIf);
        if( (pAeIf==NULL) && (pAwbIf==NULL) && (pAfIf==NULL))
        {
            CUS3A_Release();
            g_Cus3AInited = 0;
        }
    }
}

void ST_ReleaseCUS3A(void)
{
    /*Release CUS3A*/
    if(g_Cus3AInited)
    {
        CUS3A_Release();
        g_Cus3AInited = 0;
    }
}
