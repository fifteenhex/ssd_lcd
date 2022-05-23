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
#include "IMX323_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(IM323)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gIMX323Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_12,
    .u32FpsX1000    = 30,
    .u32Width       = 1920,
    .u32Height      = 1080,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 0,
    .u8HdrMode         = 0,
};
#if 0
// Davis 20181101
static Gain_ARRAY gain_table[]={
        {10000  ,4096},
        {10625  ,3855},
        {11250  ,3641},
        {11875  ,3449},
        {12500  ,3277},
        {13125  ,3121},
        {13750  ,2979},
        {14375  ,2849},
        {15000  ,2731},
        {15625  ,2621},
        {16250  ,2521},
        {16875  ,2427},
        {17500  ,2341},
        {18125  ,2260},
        {18750  ,2185},
        {19375  ,2114},
        {20000  ,2048},
        {21250  ,1928},
        {22500  ,1820},
        {23750  ,1725},
        {25000  ,1638},
        {26250  ,1560},
        {27500  ,1489},
        {28750  ,1425},
        {30000  ,1365},
        {31250  ,1311},
        {32500  ,1260},
        {33750  ,1214},
        {35000  ,1170},
        {36250  ,1130},
        {37500  ,1092},
        {38750  ,1057},
        {40000  ,1024},
        {42500  ,964 },
        {45000  ,910 },
        {47500  ,862 },
        {50000  ,819 },
        {52500  ,780 },
        {55000  ,745 },
        {57500  ,712 },
        {60000  ,683 },
        {62500  ,655 },
        {65000  ,630 },
        {67500  ,607 },
        {70000  ,585 },
        {72500  ,565 },
        {75000  ,546 },
        {77500  ,529 },
        {80000  ,512 },
        {85000  ,482 },
        {90000  ,455 },
        {95000  ,431 },
        {100000 ,410 },
        {105000 ,390 },
        {110000 ,372 },
        {115000 ,356 },
        {120000 ,341 },
        {125000 ,328 },
        {130000 ,315 },
        {135000 ,303 },
        {140000 ,293 },
        {145000 ,282 },
        {150000 ,273 },
        {155000 ,264 },
        {160000 ,256 },
        {169959 ,241 },
        {180441 ,227 },
        {190512 ,215 },
        {199805 ,205 },
        {210051 ,195 },
        {220215 ,186 },
        {230112 ,178 },
        {239532 ,171 },
        {249756 ,164 },
        {259241 ,158 },
        {269474 ,152 },
        {280548 ,146 },
        {290496 ,141 },
        {298948 ,137 },
        {310303 ,132 },
        {320000 ,128 },
};
#endif
#define PREVIEW_LINE_PERIOD 29629 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define VTS_30FPS  1124

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int IM323EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#if 0
    /*ToDO: Parsing shutter to sensor i2c setting*/
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    I2cCfg_t expo_reg[] = {
        {0xEF, 0x01},
        {0x0C, 0x00},   //shutter [8:15]
        {0x0D, 0x02},   //shutter [0:7]
        {0x0A, 0x04},   //vts [8:15]
        {0x0B, 0xF2},   //vts [0:7]
    };

    lines=(1000*nShutterUs)/PREVIEW_LINE_PERIOD;

    if(nFpsX1000<1000)    //for old method
        vts = (VTS_30FPS*30)/nFpsX1000;
    else    //new method, fps is 1000 based
        vts = (VTS_30FPS*30*1000)/nFpsX1000;

    if ( lines > (vts-2) ) //if shutter > frame interval
    {
        vts = lines +2;
    }

    lines = vts - lines;

    expo_reg[1].u16Data = (lines>>8) & 0xff;
    expo_reg[2].u16Data = (lines>>0) & 0xff;
    expo_reg[3].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[4].u16Data = (vts >> 0) & 0x00ff;

    /*copy result*/
    //memcpy(pRegs,expo_reg,sizeof(expo_reg));
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gIMX323Info.u32ShutterUs = nShutterUs;
    _gIMX323Info.u32FpsX1000 = nFpsX1000;

    return nNumRegs; //Return number of sensor registers to write
#endif
    return 0;
}

/** @brief Convert gain to sensor register setting
@param[in] nGainX1024 target sensor gain x 1024
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int IM323EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#if 0
    /*TODO: Parsing gain to sensor i2c setting*/
    unsigned char n;
    int nNumRegs = 0;
    int nId = 0;
    unsigned short u16Gain = gain_table[0].reg_val;
    int nGainTableSize= sizeof(gain_table)/sizeof(gain_table[0]);
    I2cCfg_t gain_reg[] = {
        {0xEF, 0x01},
        {0x78, 0x00},//analog gain[12:8]
        {0x79, 0x00},//analog gain[0:7]
        {0x83, 0x00},//sensor gain id
    };

    u32GainX1024=((u32GainX1024)*10000)/1024;
    for(nId=1;nId<nGainTableSize;nId++)
    {
        if(gain_table[nId].total_gain>u32GainX1024)
        {
            u16Gain=gain_table[nId-1].reg_val;
            break;
        }
        else if(nId==nGainTableSize-1)
        {
            u16Gain=gain_table[nId].reg_val;
            break;
        }
    }
    gain_reg[1].u16Data =(u16Gain>>8)&0x01f;
    gain_reg[2].u16Data =u16Gain&0xff;//low byte,LSB
    gain_reg[3].u16Data =nId-1;
    /*copy result*/
    //memcpy(pRegs,gain_reg,sizeof(gain_reg));
    for(n=0;n<sizeof(gain_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)gain_reg)[n];
    }
    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);

    _gIMX323Info.u32GainX1024 = u32GainX1024;
    return nNumRegs;
#endif
    return 0;
}

static unsigned int IM323EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_12;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_PARL;
        pSnrInfo->u32FpsX1000   = _gIMX323Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gIMX323Info.u32Width;
        pSnrInfo->u32Height     = _gIMX323Info.u32Height;
        pSnrInfo->u32GainX1024  = _gIMX323Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gIMX323Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gIMX323Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gIMX323Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gIMX323Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gIMX323Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( IM323,
                                 Sensor_init_table,
                                 IM323EarlyInitShutterAndFps,
                                 IM323EarlyInitGain,
                                 IM323EarlyInitGetSensorInfo
                                );
