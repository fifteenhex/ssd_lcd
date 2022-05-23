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

#ifndef _SENSOR_DRIVER_EARLYINIT_DATATYPE_H_
#define _SENSOR_DRIVER_EARLYINIT_DATATYPE_H_

//#include "drv_earlyinit_i2c.h"

/* Hardware resource */
typedef enum
{
    E_EARLY_INIT_I2CM0=0,
    E_EARLY_INIT_I2CM1=1,
    E_EARLY_INIT_I2CM2=2,
}EARLY_INIT_I2CM_e;

typedef enum
{
    E_EARLY_INIT_CMDQ0=0,
    E_EARLY_INIT_CMDQ1=1,
    E_EARLY_INIT_CMDQ2=2,
}EARLY_INIT_CMDQ_e;

typedef enum
{
    E_EARLY_INIT_SN_PAD0=0,
    E_EARLY_INIT_SN_PAD1=1,
    E_EARLY_INIT_SN_PAD2=2,
    E_EARLY_INIT_SN_PAD3=3,
    E_EARLY_INIT_SN_PAD_NUM
}EARLY_INIT_SN_PAD_e;

typedef enum
{
    E_EARLY_INIT_MCLK0=0,
    E_EARLY_INIT_MCLK1=1,
    E_EARLY_INIT_MCLK2=2,
}EARLY_INIT_MCLK_SRC_e;

typedef enum
{
    E_EARLY_INIT_MCLK_RATE_DEFAULT=0,
    E_EARLY_INIT_27M=   1,
    E_EARLY_INIT_72M=   2,
    E_EARLY_INIT_61P7M= 3,
    E_EARLY_INIT_54M=   4,
    E_EARLY_INIT_48M=   5,
    E_EARLY_INIT_43P2M= 6,
    E_EARLY_INIT_36M=   7,
    E_EARLY_INIT_24M=   8,
    E_EARLY_INIT_21P6M= 9,
    E_EARLY_INIT_12M=   10,
    E_EARLY_INIT_5P4M=  11,
    E_EARLY_INIT_100M=  12,
    E_EARLY_INIT_50M=   13,
    E_EARLY_INIT_25M=   14,
    E_EARLY_INIT_12P5M= 15,
    E_EARLY_INIT_37P5M= 16
}EARLY_INIT_MCLK_RATE_e;

typedef struct
{
    unsigned long nShutter;     //Shutter in us
    unsigned long nSensorGain;  //Gain 1024 based
    unsigned short nFps;        //FPS x1000
    unsigned long nBufVirtAddr;  //buffer for CMDQ, if null use module internal memory
    unsigned long nBufMiuAddr;  //miu buffer for CMDQ, if null use module internal memory
    unsigned long nBufLen;  //miu buffer for CMDQ, if null use module internal memory
}EarlyInitParam_t;

#define EARLY_INIT_SN_PIN_DEFAULT (-1)
typedef struct
{
    EARLY_INIT_I2CM_e eI2cPort;    //Sensor I2C port
    EARLY_INIT_CMDQ_e eCmdqID;     //
    EARLY_INIT_MCLK_SRC_e eMclkSrc;
    EARLY_INIT_MCLK_RATE_e eMclkRate; //MCLK rate in Hz
    unsigned long nSlaveID; //I2C slave ID
    int nRstPin; //reset pin to remap
    int nPwnPin; //power down pin remap
}EarlyInitHwRes_t;

typedef enum
{
    E_EARLYINIT_NOT_SUPPORT = -1,
    E_EARLYINIT_RUNNING = 0,
    E_EARLYINIT_DONE = 1,
}EarlyInitStatus_e;

/*! @brief Sensor bayer raw pixel order */
typedef enum {
    E_EARLYINIT_SNR_BAYER_RG = 0,        /**< bayer data start with R channel */
    E_EARLYINIT_SNR_BAYER_GR,             /**<  bayer data start with Gr channel */
    E_EARLYINIT_SNR_BAYER_BG,               /**<  bayer data start with B channel */
    E_EARLYINIT_SNR_BAYER_GB             /**<  bayer data start with Gb channel */
} EarlyInitSensorBayerId_e;

/*! @brief Sensor input raw data precision */
typedef enum {
    EARLYINIT_DATAPRECISION_8 = 0,    /**< raw data precision is 8bits */
    EARLYINIT_DATAPRECISION_10 = 1,   /**< raw data precision is 10bits */
    EARLYINIT_DATAPRECISION_16 = 2,    /**< raw data precision is 16bits */
    EARLYINIT_DATAPRECISION_12 = 3,   /**< raw data precision is 12bits */
    EARLYINIT_DATAPRECISION_14 = 4,   /**< raw data precision is 14bits */
} EarlyInitSensorDataPrecision_e;

typedef enum {
    EARLYINIT_BUS_TYPE_PARL = 0,
    EARLYINIT_BUS_TYPE_MIPI,
    EARLYINIT_BUS_TYPE_BT601,
    EARLYINIT_BUS_TYPE_BT656,
    EARLYINIT_BUS_TYPE_BT1120,
} EarlyInitSensorBusType_e;

typedef struct
{
    EarlyInitSensorBayerId_e eBayerID;
    EarlyInitSensorDataPrecision_e ePixelDepth;
    EarlyInitSensorBusType_e eIfBusType;
    unsigned int u32Width;
    unsigned int u32Height;
    unsigned int u32FpsX1000;
    unsigned int u32ShutterUs;
    unsigned int u32GainX1024;
    unsigned int u32ShutterShortUs;  /**< HDR short shutter*/
    unsigned int u32GainShortX1024;  /**< HDR short gain*/
    unsigned char  u8ResId;  /**< Sensor driver resolution ID*/
    unsigned char  u8HdrMode; /**< 0: linear mode , 1: two frame HDR enabled*/
    unsigned int  u32TimeoutMs; /**< Sensor init timeout in ms*/
}EarlyInitSensorInfo_t;

typedef struct
{
    unsigned short u16Reg;
    unsigned short u16Data;
}I2cCfg_t;

#endif  //end of _SENSOR_DRIVER_EARLY_INIT_H_
