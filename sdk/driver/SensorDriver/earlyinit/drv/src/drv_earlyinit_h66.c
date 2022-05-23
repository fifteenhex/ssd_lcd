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
#include "H66_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(H66)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gH66Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_10,
    .u32FpsX1000    = 30000,
    .u32Width       = 1280,
    .u32Height      = 960,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 0,
    .u8HdrMode         = 0,
};

#define PREVIEW_LINE_PERIOD 33333 //Line per frame = Lpf+1 , line period = (1/30)/1000
#define VTS_30FPS  1000

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int H66EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define Preview_line_period 33333                           // hts=33.333/10000=33333
#define vts_30fps  1000//1090                              //for 30fps @ MCLK=27MHz
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    I2cCfg_t expo_reg[] =
    {
        /*exposure*/
        {0x02, 0x00},
        {0x01, 0x00},
        /*vts*/
        {0x23, 0x03},
        {0x22, 0xe8},
    };

    /*VTS*/
    vts =  (vts_30fps*30000)/nFpsX1000;

    if(nFpsX1000<1000)    //for old method
        vts = (vts_30fps*30)/nFpsX1000;

    /*Exposure time*/
    lines = (1000*nShutterUs)/Preview_line_period;

    if(lines>vts-2)
        vts = lines +2;

    lines = vts-lines-1;

    expo_reg[0].u16Data = (lines>>8) & 0x00ff;
    expo_reg[1].u16Data = (lines>>0) & 0x00ff;

    expo_reg[2].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[3].u16Data = (vts >> 0) & 0x00ff;

    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gH66Info.u32ShutterUs = nShutterUs;
    _gH66Info.u32FpsX1000 = nFpsX1000;

    return nNumRegs; //Return number of sensor registers to write
}

/** @brief Convert gain to sensor register setting
@param[in] nGainX1024 target sensor gain x 1024
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
typedef unsigned long long _u64;
typedef unsigned long _u32;
typedef unsigned short _u16;
extern _u64 intlog10(_u32 value);
extern _u64 EXT_log_2(_u32 value);
static unsigned int H66EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define SENSOR_MIN_GAIN      (1 * 1024)
#define SENSOR_MAX_GAIN     (15872)                  // max sensor again, a-gain * conversion-gain*d-gain
    /*TODO: Parsing gain to sensor i2c setting*/
    unsigned char n;
    _u32 gain2_4, gain4_8, gain8_16;
    int nNumRegs = 0;
    I2cCfg_t gain_reg[] = {
        {0x00, 0x00},   //again:2^PGA[6:4]*(1+PGA[3:0]/16)
    };

    _gH66Info.u32GainX1024 = u32GainX1024;

    if(u32GainX1024 < SENSOR_MIN_GAIN)
        u32GainX1024 = SENSOR_MIN_GAIN;
    else if(u32GainX1024 >= SENSOR_MAX_GAIN)
        u32GainX1024 = SENSOR_MAX_GAIN;

    if(u32GainX1024<2048) {
      gain_reg[0].u16Data = (((u32GainX1024-1024)>>6)) & 0x000f;//<X2
    }
    else if((u32GainX1024>=2048 )&&(u32GainX1024<4096))//X2~X4
    {

      gain2_4=u32GainX1024-2048;
      gain_reg[0].u16Data = ((gain2_4>>7) & 0x000f)|0x10;
    }
    else if((u32GainX1024>=4096 )&&(u32GainX1024<8192))//X4~X8
    {

      gain4_8=u32GainX1024-4096;
      gain_reg[0].u16Data =( (gain4_8>>8) & 0x000f)|0x20;
    }
    else if((u32GainX1024>=8192 )&&(u32GainX1024<=15872))//X8~X15.5
    {

      gain8_16=u32GainX1024-8192;
      gain_reg[0].u16Data =( (gain8_16>>9) & 0x000f)|0x30;
    }

    /*copy result*/
    for(n=0;n<sizeof(gain_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)gain_reg)[n];
    }
    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);
    return nNumRegs;
}

static unsigned int H66EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_10;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_MIPI;
        pSnrInfo->u32FpsX1000   = _gH66Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gH66Info.u32Width;
        pSnrInfo->u32Height     = _gH66Info.u32Height;
        pSnrInfo->u32GainX1024  = _gH66Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gH66Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gH66Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gH66Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gH66Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gH66Info.u8HdrMode;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( H66,
                                 Sensor_init_table,
                                 H66EarlyInitShutterAndFps,
                                 H66EarlyInitGain,
                                 H66EarlyInitGetSensorInfo
                                );
