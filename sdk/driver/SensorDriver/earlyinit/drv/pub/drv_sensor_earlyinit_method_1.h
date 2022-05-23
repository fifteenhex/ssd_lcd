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

#ifndef _DRV_SENSOR_DRIVER_EARLY_INIT_MACRO_H_
#define _DRV_SENSOR_DRIVER_EARLY_INIT_MACRO_H_

#include <drv_sensor_earlyinit_metadata.h>

#define SENSOR_INIT_CMDQ_MODE               /* Setting for EarlyInit driver,Do not remove it */
typedef unsigned char SENSOR_INIT_TABLE;    /* Setting for EarlyInit driver,Do not remove it */

/*For CPU read*/
#define CMDQ_STATUS_READY 0x02
#define CMDQ_STATUS_BUSY 0x01

/*I2C 1A1D write*/
#define I2CM_1A1D_W(slave_addr,reg,data) \
        sizeof(EarlyInitCmdI2c1A1D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A1D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (data)                                              /*u8Data[0]*/

/*I2C 1A2D write*/
#define I2CM_1A1D_W_BURST2(slave_addr,reg,d0,d1) \
        sizeof(EarlyInitCmdI2c1A2D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A2D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1)                                                /*u8Data[1]*/

/*I2C 1A3D write*/
#define I2CM_1A1D_W_BURST3(slave_addr,reg,d0,d1,d2) \
        sizeof(EarlyInitCmdI2c1A3D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A3D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1),                                               /*u8Data[1]*/      \
        (d2)                                                /*u8Data[2]*/      \

/*I2C 1A4D write*/
#define I2CM_1A1D_W_BURST4(slave_addr,reg,d0,d1,d2,d3) \
        sizeof(EarlyInitCmdI2c1A4D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A4D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1),                                               /*u8Data[1]*/       \
        (d2),                                               /*u8Data[2]*/       \
        (d3)                                                /*u8Data[3]*/       \

/*I2C 1A5D write*/
#define I2CM_1A1D_W_BURST5(slave_addr,reg,d0,d1,d2,d3,d4) \
        sizeof(EarlyInitCmdI2c1A5D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A5D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1),                                               /*u8Data[1]*/       \
        (d2),                                               /*u8Data[2]*/       \
        (d3),                                               /*u8Data[3]*/       \
        (d4)                                                /*u8Data[4]*/       \

/*I2C 1A6D write*/
#define I2CM_1A1D_W_BURST6(slave_addr,reg,d0,d1,d2,d3,d4,d5) \
        sizeof(EarlyInitCmdI2c1A6D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A6D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1),                                               /*u8Data[1]*/       \
        (d2),                                               /*u8Data[2]*/       \
        (d3),                                               /*u8Data[3]*/       \
        (d4),                                               /*u8Data[4]*/       \
        (d5)                                                /*u8Data[5]*/       \

/*I2C 1A7D write*/
#define I2CM_1A1D_W_BURST7(slave_addr,reg,d0,d1,d2,d3,d4,d5,d6) \
        sizeof(EarlyInitCmdI2c1A7D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A7D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (reg),                                              /*u8Reg*/           \
        (d0),                                               /*u8Data[0]*/       \
        (d1),                                               /*u8Data[1]*/       \
        (d2),                                               /*u8Data[2]*/       \
        (d3),                                               /*u8Data[3]*/       \
        (d4),                                               /*u8Data[4]*/       \
        (d5),                                               /*u8Data[5]*/       \
        (d6)                                                /*u8Data[6]*/

/*I2C 2A1D write*/
#define I2CM_2A1D_W(slave_addr,reg,data) \
        sizeof(EarlyInitCmdI2c1A2D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A2D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        ((reg>>8)&0xFF),                                    /*u8Data[0]*/       \
        ((reg)&0xFF),                                       /*u8Reg*/           \
        (data)                                              /*u8Data[1]*/

/*I2C 2A1D write*/
#define I2CM_2A2D_W(slave_addr,reg,data) \
        sizeof(EarlyInitCmdI2c1A3D_t),                      /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_1A2D_W,                        /*u8CmdID*/         \
        (slave_addr),                                       /*u8SlaveAddr*/     \
        (((reg)>>8)&0xFF),                                  /*u8Data[0]*/       \
        ((reg)&0xFF),                                       /*u8Reg*/           \
        (((data)>>8)&0xFF),                                  /*u8Data[2]*/  \
        ((data)&0xFF)                                      /*u8Data[1]*/

#define VIF_REG_DUMMY(a) \
        sizeof(EarlyInitCmdSetDummyReg_t),          /*u8Size*/          \
        E_EARLY_INIT_CMD_SET_VIF_DUMMY_REG,         /*u8CmdID*/         \
        (a)&0xFF,                                   /*a [7:0]*/         \
        ((a)>>8)&0xFF                                 /*a [15:8]*/        \

#define CMDQ_REG_DUMMY(a) \
        sizeof(EarlyInitCmdSetDummyReg_t),          /*u8Size*/          \
        E_EARLY_INIT_CMD_SET_CMDQ_DUMMY_REG,         /*u8CmdID*/         \
        (a)&0xFF,                                   /*a [7:0]*/         \
        ((a)>>8)&0xFF                                 /*a [15:8]*/        \

/***** Delay MS *****/
#define CMDQ_DELAY_MS(t) \
        sizeof(EarlyInitCmdDelay_t),                /*u8Size*/          \
        E_EARLY_INIT_CMD_DELAY_MS,                  /*u8CmdID*/         \
        (t)                                         /*u8DelayT*/        \

/***** Delay 10us *****/
#define CMDQ_DELAY_10US(t) \
        sizeof(EarlyInitCmdDelay_t),                /*u8Size*/          \
        E_EARLY_INIT_CMD_DELAY_10US,                /*u8CmdID*/         \
        (t)                                         /*u8DelayT*/        \

/* Sensor power down pin */
#define SNR_PDWN(val) \
        sizeof(EarlyInitCmdSnrPin_t),                /*u8Size*/          \
        E_EARLY_INIT_CMD_SET_PDWN_PIN,               /*u8CmdID*/         \
        (val)                                        /*u8Val*/           \

/* Sensor reset pin */
#define SNR_RST(val) \
        sizeof(EarlyInitCmdSnrPin_t),                /*u8Size*/          \
        E_EARLY_INIT_CMD_SET_RST_PIN,               /*u8CmdID*/          \
        (val)                                        /*u8Val*/           \

/* Sensor MCLK */
#define SNR_MCLK_EN(sel) \
        sizeof(EarlyInitCmdSnrMclk_t),               /*u8Size*/          \
        E_EARLY_INIT_CMD_MCLK,                      /*u8CmdID*/          \
        (sel)                                        /*u8Sel*/           \

/* I2C CLOCK */
#define I2CM_CLK_EN(sel) \
        sizeof(EarlyInitCmdI2cClk_t),               /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_CLK,                  /*u8CmdID*/          \
        (sel)

/* I2C Register */
#define I2CM_REGW(reg,val)\
        sizeof(EarlyInitCmdSetI2cReg_t),        /*u8Size*/          \
        E_EARLY_INIT_CMD_SET_I2C_REG,           /*u8CmdID*/         \
        (reg),                                  /*u8Reg*/           \
        (val)&0xFF,                             /*u16Val [7:0]*/    \
        ((val)>>8)&0xFF                         /*u16Val [15:7]*/

/* TAG */
#define CMDQ_TAG(tag)\
        sizeof(EarlyInitCmdTag_t),              /*u8Size*/          \
        E_EARLY_INIT_CMD_TAG,                   /*u8CmdID*/         \
        (tag)&0xFF,                             /*u32Tag [7:0]*/    \
        ((tag)>>8)&0xFF,                        /*u32Tag [15:7]*/   \
        ((tag)>>16)&0xFF,                       /*u32Tag [23:16]*/  \
        ((tag)>>24)&0xFF                        /*u32Tag [31:24]*/

#define I2CM_1A1D_W_TAG(slave_addr,reg,val,tag) \
        CMDQ_TAG(tag),                \
        I2CM_1A1D_W(slave_addr,reg,val)

/* I2C parameter */
#define U16_TO_U8_ARY(a)\
        (a)&0xFF,                             /*[7:0]*/    \
        ((a)>>8)&0xFF                        /*[15:7]*/    \

#define I2CM_PARAM(lcnt,hcnt,start_cnt,stop_cnt,data_lat_cnt,sda_cnt) \
        sizeof(EarlyInitCmdI2cParam_t),              /*u8Size*/          \
        E_EARLY_INIT_CMD_I2C_PARAM,                   /*u8CmdID*/         \
        U16_TO_U8_ARY(lcnt),\
        U16_TO_U8_ARY(hcnt),\
        U16_TO_U8_ARY(start_cnt),\
        U16_TO_U8_ARY(stop_cnt),\
        U16_TO_U8_ARY(data_lat_cnt),\
        U16_TO_U8_ARY(sda_cnt)

/* NULL cmd */
#define CMDQ_NULL() \
        sizeof(EarlyInitCmdNull_t), \
        E_EARLY_INIT_CMD_NULL

#define SNR_SHUTTER_FPS_1A1D(slava_ddr,shutter,fps)\
        sizeof(EarlyInitShutterFps_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_SHUTTER_FPS,   /*u8CmdID*/\
        (shutter)&0xFF,\
        ((shutter)>>8)&0xFF,\
        ((shutter)>>16)&0xFF,\
        ((shutter)>>24)&0xFF,\
        (fps)&0xFF,\
        ((fps)>>8)&0xFF,\
        ((fps)>>16)&0xFF,\
        ((fps)>>24)&0xFF,\
        slava_ddr,\
        E_EARLY_INIT_CMD_I2C_1A1D_W

#define SNR_SHUTTER_FPS_2A1D(slava_ddr,shutter,fps)\
        sizeof(EarlyInitShutterFps_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_SHUTTER_FPS,   /*u8CmdID*/\
        (shutter)&0xFF,\
        ((shutter)>>8)&0xFF,\
        ((shutter)>>16)&0xFF,\
        ((shutter)>>24)&0xFF,\
        (fps)&0xFF,\
        ((fps)>>8)&0xFF,\
        ((fps)>>16)&0xFF,\
        ((fps)>>24)&0xFF,\
        slava_ddr,\
        E_EARLY_INIT_CMD_I2C_2A1D_W

#define SNR_SHUTTER_FPS_2A2D(slava_ddr,shutter,fps)\
        sizeof(EarlyInitShutterFps_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_SHUTTER_FPS,   /*u8CmdID*/\
        (shutter)&0xFF,\
        ((shutter)>>8)&0xFF,\
        ((shutter)>>16)&0xFF,\
        ((shutter)>>24)&0xFF,\
        (fps)&0xFF,\
        ((fps)>>8)&0xFF,\
        ((fps)>>16)&0xFF,\
        ((fps)>>24)&0xFF,\
        slava_ddr,\
        E_EARLY_INIT_CMD_I2C_2A2D_W

#define SNR_GAIN_1A1D(slave_addr,gain)\
        sizeof(EarlyInitGain_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_GAIN,          /*u8CmdID*/\
        (gain)&0xFF,\
        ((gain)>>8)&0xFF,\
        ((gain)>>16)&0xFF,\
        ((gain)>>24)&0xFF,\
        slave_addr,\
        E_EARLY_INIT_CMD_I2C_1A1D_W

#define SNR_GAIN_2A1D(slave_addr,gain)\
        sizeof(EarlyInitGain_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_GAIN,          /*u8CmdID*/\
        (gain)&0xFF,\
        ((gain)>>8)&0xFF,\
        ((gain)>>16)&0xFF,\
        ((gain)>>24)&0xFF,\
        slave_addr,\
        E_EARLY_INIT_CMD_I2C_2A1D_W

#define SNR_GAIN_2A2D(slave_addr,gain)\
        sizeof(EarlyInitGain_t),  /*u8Size*/\
        E_EARLY_INIT_CMD_GAIN,          /*u8CmdID*/\
        (gain)&0xFF,\
        ((gain)>>8)&0xFF,\
        ((gain)>>16)&0xFF,\
        ((gain)>>24)&0xFF,\
        slave_addr,\
        E_EARLY_INIT_CMD_I2C_2A2D_W

#endif
