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

#include <drv_sensor_earlyinit_common.h>
#include <drv_sensor_earlyinit_method_1.h>
#include "GC1054_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(GC1054)

static EarlyInitSensorInfo_t _gGC1054Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_RG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_10,
    .u32FpsX1000    = 30000,
    .u32Width       = 1280,
    .u32Height      = 720,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 0,
    .u8HdrMode         = 0,
};

static unsigned int gain_table[]=
{
    64, 
    91, 
    127, 
    182, 
    258, 
    369, 
    516, 
    738, 
    1032, 
    1491, 
    2084
};

static I2cCfg_t gain_reg[] = { //need to fix
    {0xfe, 0x01}, //sw page 1
    {0xb6, 0x00}, //again
    {0xb1, 0x00}, //dgain0
    {0xb2, 0x00},  //dgain1
    {0xfe, 0x00}, //sw page 0
};

#define PREVIEW_LINE_PERIOD 44959 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define VTS_30FPS  816
#define SENSOR_MAXGAIN     128

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int GC1054EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{                     
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    unsigned int u32WinHeight, u32vb;
    I2cCfg_t expo_reg[] =
    {
        {0xfe, 0x00},
        /*exposure*/
        {0x03, 0x02},
        {0x04, 0xa6},
        /*vts*/
        {0x0d, 0x02}, //win height [9:8]
        {0x0e, 0xd4}, //win height [7:0]
        {0x07, 0x00}, //vb [12:8]
        {0x08, 0x48}, //vb [7:0]
    };
        		
    /*VTS*/
    vts =  (VTS_30FPS*30*1000)/nFpsX1000;

    if(nFpsX1000<=30)    //for old method
        vts = (VTS_30FPS*30)/nFpsX1000;
    	
    lines=(1000*nShutterUs)/PREVIEW_LINE_PERIOD;
    if(lines<1) lines=1;
    if (lines > vts) {
        vts = lines;
    }else{
        vts = vts;
    }

    expo_reg[2].u16Data = (lines) & 0x00ff;
    expo_reg[1].u16Data = (lines>>8) & 0x001f;

    u32WinHeight = ((expo_reg[2].u16Data & 0x07) << 8 | expo_reg[3].u16Data);
    u32vb = vts - u32WinHeight - 20;
    expo_reg[5].u16Data = (u32vb >> 8) & 0x001f;  
    expo_reg[6].u16Data = (u32vb >> 0) & 0x00ff;

    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gGC1054Info.u32ShutterUs = nShutterUs;
    _gGC1054Info.u32FpsX1000 = nFpsX1000;

    return nNumRegs; //Return number of sensor registers to write
}

/** @brief Convert gain to sensor register setting
@param[in] nGainX1024 target sensor gain x 1024
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int GC1054EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{    
    int nNumRegs = 0,i = 0;
    int u32AgainIdx;
    unsigned int u32Total_Dgain = 0;
    unsigned int u32SnrGain;

    _gGC1054Info.u32GainX1024 = u32GainX1024;

    if (u32GainX1024 < 1024) {
        u32GainX1024 = 1024;
    } else if (u32GainX1024 > SENSOR_MAXGAIN*1024) {
        u32GainX1024 = SENSOR_MAXGAIN*1024;
    }
    u32SnrGain = u32GainX1024 / 16;

    for(u32AgainIdx = 0; u32AgainIdx < sizeof(gain_table)/sizeof(unsigned int); u32AgainIdx++)
    {
        if((gain_table[u32AgainIdx] <= u32SnrGain)&&(u32SnrGain < gain_table[u32AgainIdx+1]))
            break;
    }
    if(u32AgainIdx > sizeof(gain_table)/sizeof(unsigned int) - 1){
        u32AgainIdx = sizeof(gain_table)/sizeof(unsigned int) - 1;
    }
    u32Total_Dgain = u32SnrGain * 64 / gain_table[u32AgainIdx];

    gain_reg[1].u16Data = u32AgainIdx;
    gain_reg[2].u16Data = u32Total_Dgain >> 6;
    gain_reg[3].u16Data = (u32Total_Dgain & 0x3f) << 2;

    for (i = 0; i < sizeof(gain_reg)/sizeof(gain_reg[0]); i++)
    {
        ((unsigned char*)pRegs)[i] = ((unsigned char*)gain_reg)[i];
    }

    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);
    return nNumRegs;
}

static unsigned int GC1054EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_RG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_10;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_MIPI;
        pSnrInfo->u32FpsX1000   = _gGC1054Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gGC1054Info.u32Width;
        pSnrInfo->u32Height     = _gGC1054Info.u32Height;
        pSnrInfo->u32GainX1024  = _gGC1054Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gGC1054Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gGC1054Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gGC1054Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gGC1054Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gGC1054Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( gc1054,
                                 Sensor_init_table,
                                 GC1054EarlyInitShutterAndFps,
                                 GC1054EarlyInitGain,
                                 GC1054EarlyInitGetSensorInfo
                                );
