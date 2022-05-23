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

#ifndef _DRV_SENSOR_EARLYINIT_METADATA_H_
#define _DRV_SENSOR_EARLYINIT_METADATA_H_

typedef enum
{
    E_EARLY_INIT_CMD_I2C_1A1D_W = 0,
    E_EARLY_INIT_CMD_I2C_1A2D_W = 1,
    E_EARLY_INIT_CMD_I2C_1A3D_W = 2,
    E_EARLY_INIT_CMD_I2C_1A4D_W = 3,
    E_EARLY_INIT_CMD_I2C_1A5D_W = 4,
    E_EARLY_INIT_CMD_I2C_1A6D_W = 5,
    E_EARLY_INIT_CMD_I2C_1A7D_W = 6,
    E_EARLY_INIT_CMD_SET_CMDQ_DUMMY_REG = 7,
    E_EARLY_INIT_CMD_DELAY_MS   =   8,
    E_EARLY_INIT_CMD_DELAY_10US = 9,
    E_EARLY_INIT_CMD_SET_PDWN_PIN   = 10,
    E_EARLY_INIT_CMD_SET_RST_PIN    = 11,
    E_EARLY_INIT_CMD_MCLK           = 12,
    E_EARLY_INIT_CMD_I2C_CLK        = 13,
    E_EARLY_INIT_CMD_SET_I2C_REG    = 14,
    E_EARLY_INIT_CMD_SET_VIF_DUMMY_REG = 15,
    E_EARLY_INIT_CMD_TAG            = 16,
    E_EARLY_INIT_CMD_I2C_PARAM      = 17,
    E_EARLY_INIT_CMD_NULL           = 18,
    E_EARLY_INIT_CMD_I2C_2A1D_W     = 19,
    E_EARLY_INIT_CMD_I2C_2A2D_W     = 20,
    E_EARLY_INIT_CMD_SHUTTER_FPS    = 21,
    E_EARLY_INIT_CMD_GAIN           = 22,
}EarlyInitCmdID_e;

/*Meta data cmd header*/
typedef struct __attribute__ ((packed))
{
    unsigned char u8Size;
    unsigned char u8CmdID;
}EarlyInitCmdHead_t;

/*I2C 1A1D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[2];
}EarlyInitCmdI2c1A1D_t;

/*I2C 1A2D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[3];
}EarlyInitCmdI2c1A2D_t;

/*I2C 1A3D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[4];
}EarlyInitCmdI2c1A3D_t;

/*I2C 1A4D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[5];
}EarlyInitCmdI2c1A4D_t;

/*I2C 1A5D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[6];
}EarlyInitCmdI2c1A5D_t;

/*I2C 1A6D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[7];
}EarlyInitCmdI2c1A6D_t;

/*I2C 1A7D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    //unsigned char u8Reg;
    unsigned char u8Data[8];
}EarlyInitCmdI2c1A7D_t;

/*I2C 2A1D write*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    unsigned char u8Reg[2];
    unsigned char u8Data[1];
}EarlyInitCmdI2c2A1D_t;

typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8SlaveAddr;
    unsigned char u8Reg[2];
    unsigned char u8Data[2];
}EarlyInitCmdI2c2A2D_t;


/****** Set CMDQ dummy register *****/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned short u16Dummy;
}EarlyInitCmdSetDummyReg_t;

/***** Delay MS *****/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8DelayT;
}EarlyInitCmdDelay_t;

/* Sensor power down pin */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8Val;
}EarlyInitCmdSnrPin_t;

/* Sensor MCLK */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8Sel;
}EarlyInitCmdSnrMclk_t;

/* I2C CLOCK */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8Sel;
}EarlyInitCmdI2cClk_t;

/* I2C Register */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned char u8Reg;
    unsigned short u16Val;
}EarlyInitCmdSetI2cReg_t;

/* TAG */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned int u32Tag;
}EarlyInitCmdTag_t;

/*I2CM parameter*/
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned short reg_lcnt;
    unsigned short reg_hcnt;
    unsigned short reg_start_cnt;
    unsigned short reg_stop_cnt;
    unsigned short reg_data_lat_cnt;
    unsigned short reg_sda_cnt;
}EarlyInitCmdI2cParam_t;

typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned int nShutterUs;
    unsigned int nFpsX1000;
    unsigned char nSlaveAddr;
    unsigned char nI2cFmt;
}EarlyInitShutterFps_t;

typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
    unsigned int nGainX1024;
    unsigned char nSlaveAddr;
    unsigned char nI2cFmt;
}EarlyInitGain_t;

/* NULL cmd */
typedef struct __attribute__ ((packed))
{
    EarlyInitCmdHead_t tHead;
}EarlyInitCmdNull_t;

#endif
