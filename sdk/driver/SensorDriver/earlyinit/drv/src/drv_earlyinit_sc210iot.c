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
#include "SC210iot_MIPI_init_table.h"         /* Sensor initial table */
#include <drv_sensor_common.h>
#include <sensor_i2c_api.h>
#include <drv_sensor.h>

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(SC210iot)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gSC2335Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_10,
    .u32FpsX1000    = 30000,
    .u32Width       = 1920,
    .u32Height      = 1080,
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


//#define PREVIEW_LINE_PERIOD 29630 //Line per frame = Lpf+1 , line period = (1/30)/1125
//#define VTS_30FPS  1125

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/


static unsigned int SC2335EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define Preview_line_period 29630                           // hts=33.333/1125=25629
#define vts_30fps  1125//1090                              //for 29.091fps @ MCLK=36MHz
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    int nNumRegs = 0;

    I2cCfg_t expo_reg[] =
    {
        /*exposure*/
        {0x3e00, 0x00},//expo [20:17]
        {0x3e01, 0x8c}, // expo[16:8]
        {0x3e02, 0x60}, // expo[7:0], [3:0] fraction of line
        /*vts*/
        {0x320e, 0x04},
        {0x320f, 0x65},
    };
    /*VTS*/
    vts =  (vts_30fps*30000)/nFpsX1000;

    if(nFpsX1000<1000)    //for old method
        vts = (vts_30fps*30)/nFpsX1000;

    /*Exposure time*/
    lines = (1000*nShutterUs*2)/Preview_line_period;

    if(lines<1)
        lines=1;

    if (lines >  2 * (vts)-8) {
        vts = (lines+9)/2;
    };
    lines = lines<<4;

    expo_reg[0].u16Data = (lines>>16) & 0x0f;
    expo_reg[1].u16Data = (lines>>8) & 0xff;
    expo_reg[2].u16Data = (lines>>0) & 0xf0;
    //vts
    expo_reg[3].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[4].u16Data = (vts >> 0) & 0x00ff;


    /*copy result*/
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gSC2335Info.u32ShutterUs = nShutterUs;
    _gSC2335Info.u32FpsX1000 = nFpsX1000;

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

static unsigned int SC2335EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
#define SENSOR_MAXGAIN      (15875*31)/1000   /////sensor again 15.875 dgain=31.5
    _u16 i=0 ,Dgain = 1,  Ana_gain = 1;
    _u64 Fine_againx64 = 32,Fine_dgainx128 = 128;
    _u16 Dgain_reg = 0, Ana_gain_reg = 0, Fine_again_reg= 0x20,Fine_dgain_reg= 0x80;
    int nNumRegs = 0;
	//static u32 reg_0x397475=0/*, BLC_value=0, BLC_ratio=1*/;
	//u16 reg3974, reg3974_2, reg3975;
	//static u8 frmcount=0;

    I2cCfg_t gain_reg[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, 0x03},
        {0x3e09, 0x20},
    };
    _gSC2335Info.u32GainX1024 = u32GainX1024;
#if 0
	I2C_ARRAY nr_reg_temp[] ={
		{0x363c, 0x0e},
	};
	I2C_ARRAY temperature_reg_1_temp[] ={
		{0x5787, 0x00},
		{0x5788, 0x00},
		{0x5790, 0x00},
		{0x5791, 0x00},
		{0x5799, 0x07},
	};
	// I2C_ARRAY temperature_reg_2_temp[] ={
		// {0x3637, 0x20},
		// {0x391f, 0x18},
		// {0x3908, 0x82},
	// };
#endif
    //memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));
	//memcpy(nr_reg_temp, nr_reg, sizeof(nr_reg));
	//memcpy(temperature_reg_1_temp, temperature_reg_1, sizeof(temperature_reg_1));
	//memcpy(temperature_reg_2_temp, temperature_reg_2, sizeof(temperature_reg_2));

    if (u32GainX1024<1024){
        u32GainX1024=1024;
    }else if (u32GainX1024>=SENSOR_MAXGAIN*1024){
        u32GainX1024=SENSOR_MAXGAIN*1024;
    }

    if (u32GainX1024 < 2 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 1;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x03;
    } else if (u32GainX1024 < 4 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 2;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x07;
    } else if (u32GainX1024 < 8 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 4;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x0f;
    } else if (u32GainX1024 <=  16128) {// 16256
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x1f;
    }
#if 1
	else if (u32GainX1024 <  16128 * 2) {
        Dgain = 1;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (u32GainX1024 <  16128 * 4) {
        Dgain = 2;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x01;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (u32GainX1024 < 16128 * 8) {
        Dgain = 4;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x03;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (u32GainX1024 < 16128 * 16) {
        Dgain = 8;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x07;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (u32GainX1024 <= 508032) { // 16128 * 31.5  ---   SENSOR_MAXGAIN * 1024
        Dgain = 16;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x0f;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    }
#endif

    if (u32GainX1024 <= 16128) {
        Fine_againx64 = 4 * u32GainX1024/ (Ana_gain * Fine_dgainx128);
        //Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_again_reg = Fine_againx64;
        //Fine_dgain_reg = Fine_dgainx128;
    } else {
        Fine_dgainx128 = 4 * u32GainX1024/ (Dgain * Ana_gain * Fine_againx64);
        Fine_dgain_reg = Fine_dgainx128;
    }
	// printk("[%s]  gain : %d,%lld,%lld,%d, %d\n", __FUNCTION__,gain,Fine_againx64,Fine_dgainx128,Dgain,Ana_gain);
    // printk("[%s]  gain:%d. gain_reg : %x ,%x ,%x , %x\n", __FUNCTION__,gain,Fine_again_reg,Ana_gain_reg,Fine_dgain_reg,Dgain_reg);

    gain_reg[3].u16Data = Fine_again_reg;      // 3e09
    gain_reg[2].u16Data = Ana_gain_reg;		// 3e08
    gain_reg[1].u16Data = Fine_dgain_reg;		// 3e07
    gain_reg[0].u16Data = Dgain_reg & 0x0f;	// 3e06

	for (i = 0; i < sizeof(gain_reg)/sizeof(gain_reg[0]); i++)
        {
             ((unsigned char*)pRegs)[i] = ((unsigned char*)gain_reg)[i];
        }
            nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);
        return nNumRegs;
}

static unsigned int SC2335EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_10;
        pSnrInfo->eIfBusType    = EARLYINIT_BUS_TYPE_MIPI;
        pSnrInfo->u32FpsX1000   = _gSC2335Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gSC2335Info.u32Width;
        pSnrInfo->u32Height     = _gSC2335Info.u32Height;
        pSnrInfo->u32GainX1024  = _gSC2335Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gSC2335Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gSC2335Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gSC2335Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gSC2335Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gSC2335Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 200;
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( SC210iot,
                                 Sensor_init_table,
                                 SC2335EarlyInitShutterAndFps,
                                 SC2335EarlyInitGain,
                                 SC2335EarlyInitGetSensorInfo
                                );
