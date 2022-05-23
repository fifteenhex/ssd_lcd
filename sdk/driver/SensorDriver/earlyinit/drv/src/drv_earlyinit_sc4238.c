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
#include "SC4238_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(SC4238)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gSC4238Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_10,
    .u32FpsX1000    = 30000,
    .u32Width       = 2560,
    .u32Height      = 1440,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 0,
    .u8HdrMode         = 0,
};

typedef struct {
    unsigned int gain;
    unsigned short fine_gain_reg;
} FINE_GAIN;

FINE_GAIN _gSC4238fine_again[] = {
//gain map update for 1/64 precision
    {1024, 0x40},
    {1040, 0x41},
    {1056, 0x42},
    {1072, 0x43},
    {1088, 0x44},
    {1104, 0x45},
    {1120, 0x46},
    {1136, 0x47},
    {1152, 0x48},
    {1168, 0x49},
    {1184, 0x4a},
    {1200, 0x4b},
    {1216, 0x4c},
    {1232, 0x4d},
    {1248, 0x4e},
    {1264, 0x4f},
    {1280, 0x50},
    {1296, 0x51},
    {1312, 0x52},
    {1328, 0x53},
    {1344, 0x54},
    {1360, 0x55},
    {1376, 0x56},
    {1392, 0x57},
    {1408, 0x58},
    {1424, 0x59},
    {1440, 0x5a},
    {1456, 0x5b},
    {1472, 0x5c},
    {1488, 0x5d},
    {1504, 0x5e},
    {1520, 0x5f},
    {1536, 0x60},
    {1552, 0x61},
    {1568, 0x62},
    {1584, 0x63},
    {1600, 0x64},
    {1616, 0x65},
    {1632, 0x66},
    {1648, 0x67},
    {1664, 0x68},
    {1680, 0x69},
    {1696, 0x6a},
    {1712, 0x6b},
    {1728, 0x6c},
    {1744, 0x6d},
    {1760, 0x6e},
    {1776, 0x6f},
    {1792, 0x70},
    {1808, 0x71},
    {1824, 0x72},
    {1840, 0x73},
    {1856, 0x74},
    {1872, 0x75},
    {1888, 0x76},
    {1904, 0x77},
    {1920, 0x78},
    {1936, 0x79},
    {1952, 0x7a},
    {1968, 0x7b},
    {1984, 0x7c},
    {2000, 0x7d},
    {2016, 0x7e},
    {2032, 0x7f},
};
#define PREVIEW_LINE_PERIOD 29629 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define VTS_30FPS  1124

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int SC4238EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define Preview_line_period 22222                           // hts=33.333/1125=25629
#define vts_30fps  1562//1090                              //for 29.091fps @ MCLK=36MHz
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    I2cCfg_t expo_reg[] =
    {
        /*exposure*/
        {0x3e00, 0x00},//expo [20:17]
        {0x3e01, 0x30}, // expo[16:8]
        {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
        /*vts*/
        {0x320e, 0x06},
        {0x320f, 0x1a},
    };

    /*VTS*/
    vts =  (vts_30fps*30000)/nFpsX1000;

    if(nFpsX1000<=30)    //for old method
        vts = (vts_30fps*30)/nFpsX1000;

    lines = (1000*nShutterUs*2)/Preview_line_period; // Preview_line_period in ns
    if(lines<3) lines=3;
    if (lines >  2 * vts-10) {
        vts = (lines+11)/2;
    }
    //else
   //     vts=params->expo.vts;

    //params->expo.line = half_lines;
    //CamOsPrintf("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, nShutterUs, lines, vts);

    lines = lines<<4;

    expo_reg[0].u16Data = (lines>>16) & 0x0f;
    expo_reg[1].u16Data =  (lines>>8) & 0xff;
    expo_reg[2].u16Data = (lines>>0) & 0xf0;
    expo_reg[3].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[4].u16Data = (vts >> 0) & 0x00ff;

    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gSC4238Info.u32ShutterUs = nShutterUs;
    _gSC4238Info.u32FpsX1000 = nFpsX1000;

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
extern _u64 EXT_log_2(_u32 value);

static unsigned int SC4238EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define SENSOR_MAXGAIN      (15875*315)/10000   /////sensor again 15.875 dgain=31.5
    _u16 i=0 ,Dgain = 1,  Coarse_gain = 1;
    _u64 Fine_again = 1024,Fine_dgain = 10000;
    _u16 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;
    int nNumRegs = 0;

    I2cCfg_t gain_reg[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x00},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x10},
    };

    _gSC4238Info.u32GainX1024 = u32GainX1024;

    if (u32GainX1024 < 1024) {
        u32GainX1024 = 1024;
    } else if (u32GainX1024 > SENSOR_MAXGAIN*1024) {
        u32GainX1024 = SENSOR_MAXGAIN*1024;
    }
    if (u32GainX1024 < 2 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (u32GainX1024 <  4 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (u32GainX1024 <  8 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (u32GainX1024 <=  16256)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (u32GainX1024 <  32512)
    {
        Dgain = 1;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
    else if (u32GainX1024 <  65024)
    {
        Dgain = 2;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
     else if (u32GainX1024 < 127 * 1024)
    {
        Dgain = 4;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (u32GainX1024 < 254 * 1024)
    {
        Dgain = 8;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (u32GainX1024 < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }

    if (u32GainX1024 <= 16256)
    {
        Fine_again = u32GainX1024*10000 / (Dgain * Coarse_gain * Fine_dgain);
        for(i = 1; i< sizeof(_gSC4238fine_again)/sizeof(FINE_GAIN);i++)
        {
            if(Fine_again >= _gSC4238fine_again[i-1].gain && Fine_again <= _gSC4238fine_again[i].gain)
            {
                Fine_again_reg = (_gSC4238fine_again[i].gain - Fine_again) > (Fine_again - _gSC4238fine_again[i-1].gain) ? _gSC4238fine_again[i-1].fine_gain_reg:_gSC4238fine_again[i].fine_gain_reg;
                break;
            }
            else if(Fine_again > _gSC4238fine_again[(sizeof(_gSC4238fine_again)/sizeof(FINE_GAIN)) - 1].gain)
            {
                Fine_again_reg = 0x7f;
                break;
            }
        }
    }
    else
    {
        Fine_dgain_reg = (u32GainX1024 * 128 / (Dgain * Coarse_gain * Fine_again));
    }

    gain_reg[3].u16Data = Fine_again_reg;
    gain_reg[2].u16Data = Coarse_gain_reg;
    gain_reg[1].u16Data = Fine_dgain_reg;
    gain_reg[0].u16Data = Dgain_reg & 0xF;

    for (i = 0; i < sizeof(gain_reg)/sizeof(gain_reg[0]); i++)
    {
        ((unsigned char*)pRegs)[i] = ((unsigned char*)gain_reg)[i];
    }

    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);
    return nNumRegs;
}

static unsigned int SC4238EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_10;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_MIPI;
        pSnrInfo->u32FpsX1000   = _gSC4238Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gSC4238Info.u32Width;
        pSnrInfo->u32Height     = _gSC4238Info.u32Height;
        pSnrInfo->u32GainX1024  = _gSC4238Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gSC4238Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gSC4238Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gSC4238Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gSC4238Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gSC4238Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( SC4238,
                                 Sensor_init_table,
                                 SC4238EarlyInitShutterAndFps,
                                 SC4238EarlyInitGain,
                                 SC4238EarlyInitGetSensorInfo
                                );
