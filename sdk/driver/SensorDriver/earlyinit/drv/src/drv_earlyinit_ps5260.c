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
#include "PS5260_MIPI_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(PS5260)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gPS5260Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_14,
    .u32FpsX1000    = 30000,
    .u32Width       = 1920,
    .u32Height      = 1080,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 2,
    .u8HdrMode         = 0,
};
#if 1
// Davis 20181101
const static Gain_ARRAY gain_table[]={
    {1024 ,0   },
    {1088 ,1   },
    {1152 ,2   },
    {1216 ,3   },
    {1280 ,4   },
    {1344 ,5   },
    {1408 ,6   },
    {1472 ,7   },
    {1536 ,8   },
    {1600 ,9   },
    {1664 ,10  },
    {1728 ,11  },
    {1792 ,12  },
    {1856 ,13  },
    {1920 ,14  },
    {1984 ,15  },
    {2048 ,16  },
    {2176 ,17  },
    {2304 ,18  },
    {2432 ,19  },
    {2560 ,20  },
    {2720 ,21  },
    {2880 ,22  },
    {3040 ,23  },
    {3200 ,24  },
    {3360 ,25  },
    {3520 ,26  },
    {3680 ,27  },
    {3840 ,28  },
    {4000 ,29  },
    {4160 ,30  },
    {4320 ,31  },
    {4480 ,32  },
    {4640 ,33  },
    {4800 ,34  },
    {4960 ,35  },
    {5120 ,36  },
    {5440 ,37  },
    {5760 ,38  },
    {6080 ,39  },
    {6400 ,40  },
    {6720 ,41  },
    {7040 ,42  },
    {7360 ,43  },
    {7680 ,44  },
    {8000 ,45  },
    {8320 ,46  },
    {8640 ,47  },
    {8960 ,48  },
    {9280 ,49  },
    {9600 ,50  },
    {9920 ,51  },
    {10240, 52 },
    {10880, 53 },
    {11520, 54 },
    {12160, 55 },
    {12800, 56 },
    {13440, 57 },
    {14080, 58 },
    {14720, 59 },
    {15360, 60 },
    {16000, 61 },
    {16640, 62 },
    {17280, 63 },
    {17920, 64 },
    {18560, 65 },
    {19200, 66 },
    {19840, 67 },
    {20480, 68 },   //20x
    {21504, 69},
    {22528, 70},
    {23552, 71},
    {24576, 72},
    {25600, 73},
    {26624, 74},
    {27648, 75},
    {28672, 76},
    {29696, 77},
    {30720, 78},
    {31744, 79},
    {32768, 80},
    {34816, 81},
    {36864, 82},
    {38912, 83},
    {40960, 84},
    {43008, 85},
    {45056, 86},
    {47104, 87},
    {49152, 88},
    {51200, 89},
    {53248, 90},
    {55296, 91},
    {57344, 92},
    {59392, 93},
    {61440, 94},
    {63488, 95},
    {65536, 96},
};
#endif

#define PREVIEW_LINE_PERIOD 29630 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define VTS_30FPS  1125

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int PS5260EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
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

    _gPS5250Info.u32ShutterUs = nShutterUs;
    _gPS5250Info.u32FpsX1000 = nFpsX1000;

    return nNumRegs; //Return number of sensor registers to write
#else
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;
    unsigned int ne_patch = 0, g_sns_const = 0, ne=0;

    I2cCfg_t expo_reg[] = {
        {0xEF, 0x01},//shutter
        {0x0C, 0x00},
        {0x0D, 0x03},
        {0x0E, 0x10},
        {0x0F, 0xCC},
        {0x10, 0x00},
        {0x12, 0x50},
        //{0x09, 0x01},
        //{0xEF, 0x01},//VTS
        {0x0A, 0x04},//VTS
        {0x0B, 0x64},
        //{0x09, 0x01},
        {0x09, 0x01},
    };

    lines=(1000*nShutterUs)/PREVIEW_LINE_PERIOD;
    if(lines < 2)
        lines = 2;

    if(nFpsX1000<1000)    //for old method
        vts = (VTS_30FPS*30)/nFpsX1000;
    else    //new method, fps is 1000 based
        vts = (VTS_30FPS*30*1000)/nFpsX1000;

    if ( lines > (vts-2) ) //if shutter > frame interval
    {
        vts = lines +2;
    }

    lines = vts - lines;

    g_sns_const=0x111c;
    ne_patch=16+((38*lines)>>8);
    if (ne_patch > 200){
        ne_patch = 200;
    }
    ne = g_sns_const - ne_patch;

    expo_reg[1].u16Data =(unsigned short)( (lines>>8) & 0x00ff);
    expo_reg[2].u16Data =(unsigned short)( (lines>>0) & 0x00ff);
    expo_reg[3].u16Data = (unsigned short)((ne >>8) & 0x001f);
    expo_reg[4].u16Data = (unsigned short)((ne >>0) & 0x00ff);
    expo_reg[5].u16Data = (unsigned short)(((ne_patch & 0x0100) >>8)<<2);
    expo_reg[6].u16Data = (unsigned short)((ne_patch >>0) & 0x00ff);

    expo_reg[7].u16Data = (unsigned short)((vts >> 8) & 0x00ff);
    expo_reg[8].u16Data = (unsigned short)((vts >> 0) & 0x00ff);

    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    return nNumRegs;
#endif
}

/** @brief Convert gain to sensor register setting
@param[in] nGainX1024 target sensor gain x 1024
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int PS5260EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#if 0
    /*TODO: Parsing gain to sensor i2c setting*/
    unsigned char n;
    unsigned int u32Gain;
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

    u32Gain=((u32GainX1024)*10000)/1024;
    for(nId=1;nId<nGainTableSize;nId++)
    {
        if(gain_table[nId].total_gain>u32Gain)
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

    _gPS5250Info.u32GainX1024 = u32GainX1024;
    return nNumRegs;
#else

#define AE_MIN_GAIN     1152
#define MIN_GAIN_ID 4
#define SENSOR_MAX_GAIN (1024*80)
    int n = 0, sens = 0, gain = 1024;
    int nNumRegs = 0;
    int nId = 0;
    I2cCfg_t gain_reg[] = {
        {0xEF, 0x01},
        {0x83, 0x04},//analog gain suggest from 1.25X to 32X
        {0x18, 0x00},
        {0x80, 0x00},
        {0x09, 0x01},
    };

    if(u32GainX1024<AE_MIN_GAIN)
        u32GainX1024=AE_MIN_GAIN;
    else if(u32GainX1024>=SENSOR_MAX_GAIN)
        u32GainX1024=SENSOR_MAX_GAIN;

    if(u32GainX1024 < 4*1024){
        sens = 1;     // LS
    }else if(u32GainX1024 > 6*1024){
        sens = 0;     // HS
    }

    for(nId = MIN_GAIN_ID;nId < sizeof(gain_table)/sizeof(gain_table[0]);nId++)
    {
        if(gain < gain_table[nId].total_gain)    break;
    }

    gain_reg[1].u16Data = sens ?nId:(nId-16);    // Davis 20181101 -- 20200513 for normal usage
    gain_reg[2].u16Data = sens;

    /*copy result*/
    for(n=0;n<sizeof(gain_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)gain_reg)[n];
    }
    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);

    return nNumRegs;
#endif
}

static unsigned int PS5260EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_14;
        pSnrInfo->u32FpsX1000   = _gPS5260Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gPS5260Info.u32Width;
        pSnrInfo->u32Height     = _gPS5260Info.u32Height;
        pSnrInfo->u32GainX1024  = _gPS5260Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gPS5260Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gPS5260Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gPS5260Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gPS5260Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gPS5260Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
        //CamOsPrintf("PS5250 , FPS=%u, Shutter=%u, Gain=%u \n", pSnrInfo->u32FpsX1000, pSnrInfo->u32ShutterUs, pSnrInfo->u32GainX1024);
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( PS5260,
                                 Sensor_init_table,
                                 PS5260EarlyInitShutterAndFps,
                                 PS5260EarlyInitGain,
                                 PS5260EarlyInitGetSensorInfo
                                );
