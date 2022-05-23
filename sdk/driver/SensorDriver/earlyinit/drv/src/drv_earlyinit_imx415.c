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
#include "IMX415_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(IMX415)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gIMX415Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_GB,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_10,
    .u32FpsX1000    = 30000,
    .u32Width       = 3840,
    .u32Height      = 2160,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 7,
    .u8HdrMode         = 0,
};


/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int IMX415EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define Preview_line_period 14814                           // hts=33.333/1125=25629
#define vts_30fps  2250//1090                              //for 29.091fps @ MCLK=36MHz
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    I2cCfg_t expo_reg[] =
    {
        /*exposure*/
        {0x3052, 0x00},
        {0x3051, 0x00},
        {0x3050, 0x08},
        /*vts*/
        {0x3026, 0x00},
        {0x3025, 0x08},
        {0x3024, 0xCA},
    };

    /*VTS*/
    vts =  (vts_30fps*30000)/nFpsX1000;

    if(nFpsX1000<1000)    //for old method
        vts = (vts_30fps*30)/nFpsX1000;

    /*Exposure time*/
    lines = (1000*nShutterUs)/Preview_line_period;

    if(lines>vts-4)
        vts = lines +4;

    lines = vts-lines-1;

    expo_reg[0].u16Data = (lines>>16) & 0x0003;
    expo_reg[1].u16Data = (lines>>8) & 0x00ff;
    expo_reg[2].u16Data = (lines>>0) & 0x00ff;

    expo_reg[3].u16Data = (vts >> 16) & 0x0003;
    expo_reg[4].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[5].u16Data = (vts >> 0) & 0x00ff;

    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gIMX415Info.u32ShutterUs = nShutterUs;
    _gIMX415Info.u32FpsX1000 = nFpsX1000;

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
static unsigned int IMX415EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define SENSOR_MIN_GAIN      (1 * 1024)
#define SENSOR_MAX_GAIN     (3981 * 1024)                  // max sensor again, a-gain * conversion-gain*d-gain
    /*TODO: Parsing gain to sensor i2c setting*/
    unsigned char n;
    _u64 nGainDouble;
    int nNumRegs = 0;
    I2cCfg_t gain_reg[] = {
        {0x3090, 0x2A},//low bit
        {0x3091, 0x00},//hcg mode,bit 4
    };

    _gIMX415Info.u32GainX1024 = u32GainX1024;

    if(u32GainX1024 < SENSOR_MIN_GAIN)
        u32GainX1024 = SENSOR_MIN_GAIN;
    else if(u32GainX1024 >= SENSOR_MAX_GAIN)
        u32GainX1024 = SENSOR_MAX_GAIN;

    nGainDouble = 20*(intlog10(u32GainX1024)-intlog10(1024));

    gain_reg[0].u16Data = (_u16)(((nGainDouble*10)>> 24)/3);

    /*copy result*/
    for(n=0;n<sizeof(gain_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)gain_reg)[n];
    }
    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);
    return nNumRegs;
}

static unsigned int IMX415EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_GB;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_10;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_MIPI;
        pSnrInfo->u32FpsX1000   = _gIMX415Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gIMX415Info.u32Width;
        pSnrInfo->u32Height     = _gIMX415Info.u32Height;
        pSnrInfo->u32GainX1024  = _gIMX415Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gIMX415Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gIMX415Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gIMX415Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gIMX415Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gIMX415Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( IMX415,
                                 Sensor_init_table,
                                 IMX415EarlyInitShutterAndFps,
                                 IMX415EarlyInitGain,
                                 IMX415EarlyInitGetSensorInfo
                                );
