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

#ifndef _F32_INIT_TABLE_H_

#define _F32_PDWN_ON_ 0
#define _F32_RST_ON_ 0

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_PDWN(_F32_PDWN_ON_),            //power low
    SNR_RST(~_F32_RST_ON_),             //reset high
    CMDQ_DELAY_MS(2),       //T1, delay 2ms

    SNR_PDWN(~_F32_PDWN_ON_),            //power high
    SNR_MCLK_EN(0x7),               //0x7=MCLK 27MhZ, 0xB=37.125MHz
    CMDQ_DELAY_MS(2),             //T2, delay 2ms

    SNR_RST(_F32_RST_ON_),       //reset low
    CMDQ_DELAY_MS(5),             //T3, delay 5ms

    /*I2C timing*/
    I2CM_CLK_EN(0x2),  //12MHz
    /*
    I2CM_REGW(reg_lcnt,16),
    I2CM_REGW(reg_hcnt,11),
    I2CM_REGW(reg_start_cnt,50),
    I2CM_REGW(reg_stop_cnt,50),
    I2CM_REGW(reg_data_lat_cnt,0),
    I2CM_REGW(reg_sda_cnt,11),
    */
    I2CM_PARAM(16,11,50,50,0,11),
#endif

    /*Linear, 2 lane */
    I2CM_1A1D_W(0x80, 0x12, 0x40),
    I2CM_1A1D_W(0x80, 0x48, 0x8A),
    I2CM_1A1D_W(0x80, 0x48, 0x0A),
    I2CM_1A1D_W(0x80, 0x0E, 0x11),
    I2CM_1A1D_W(0x80, 0x0F, 0x14),
    I2CM_1A1D_W(0x80, 0x10, 0x20),
    I2CM_1A1D_W(0x80, 0x11, 0x80),
    I2CM_1A1D_W(0x80, 0x0D, 0xF0),
    I2CM_1A1D_W(0x80, 0x5F, 0x42),
    I2CM_1A1D_W(0x80, 0x60, 0x2B),
    I2CM_1A1D_W(0x80, 0x58, 0x18),
    I2CM_1A1D_W(0x80, 0x57, 0x60),
    I2CM_1A1D_W(0x80, 0x64, 0xE0),
    I2CM_1A1D_W(0x80, 0x20, 0x00),
    I2CM_1A1D_W(0x80, 0x21, 0x05),
    I2CM_1A1D_W(0x80, 0x22, 0x65),
    I2CM_1A1D_W(0x80, 0x23, 0x04),
    I2CM_1A1D_W(0x80, 0x24, 0xC0),
    I2CM_1A1D_W(0x80, 0x25, 0x38),
    I2CM_1A1D_W(0x80, 0x26, 0x43),
    I2CM_1A1D_W(0x80, 0x27, 0x0C),
    I2CM_1A1D_W(0x80, 0x28, 0x15),
    I2CM_1A1D_W(0x80, 0x29, 0x02),
    I2CM_1A1D_W(0x80, 0x2A, 0x00),
    I2CM_1A1D_W(0x80, 0x2B, 0x12),
    I2CM_1A1D_W(0x80, 0x2C, 0x00),
    I2CM_1A1D_W(0x80, 0x2D, 0x00),
    I2CM_1A1D_W(0x80, 0x2E, 0x14),
    I2CM_1A1D_W(0x80, 0x2F, 0x44),
    I2CM_1A1D_W(0x80, 0x41, 0xC4),
    I2CM_1A1D_W(0x80, 0x42, 0x13),
    I2CM_1A1D_W(0x80, 0x46, 0x01),
    I2CM_1A1D_W(0x80, 0x76, 0x60),
    I2CM_1A1D_W(0x80, 0x77, 0x09),
    I2CM_1A1D_W(0x80, 0x80, 0x04),
    I2CM_1A1D_W(0x80, 0x1D, 0x00),
    I2CM_1A1D_W(0x80, 0x1E, 0x04),
    I2CM_1A1D_W(0x80, 0x6C, 0x40),
    I2CM_1A1D_W(0x80, 0x68, 0x00),
    I2CM_1A1D_W(0x80, 0x70, 0x6D),
    I2CM_1A1D_W(0x80, 0x71, 0x6D),
    I2CM_1A1D_W(0x80, 0x72, 0x6A),
    I2CM_1A1D_W(0x80, 0x73, 0x36),
    I2CM_1A1D_W(0x80, 0x74, 0x02),
    I2CM_1A1D_W(0x80, 0x78, 0x9E),
    I2CM_1A1D_W(0x80, 0x89, 0x81),
    I2CM_1A1D_W(0x80, 0x6E, 0x2C),
    I2CM_1A1D_W(0x80, 0x32, 0x4F),
    I2CM_1A1D_W(0x80, 0x33, 0x58),
    I2CM_1A1D_W(0x80, 0x34, 0x5F),
    I2CM_1A1D_W(0x80, 0x35, 0x5F),
    I2CM_1A1D_W(0x80, 0x3A, 0xAF),
    I2CM_1A1D_W(0x80, 0x56, 0x32),
    I2CM_1A1D_W(0x80, 0x59, 0x67),
    I2CM_1A1D_W(0x80, 0x85, 0x3C),
    I2CM_1A1D_W(0x80, 0x8A, 0x04),
    I2CM_1A1D_W(0x80, 0x91, 0x10),
    I2CM_1A1D_W(0x80, 0x5A, 0x09),
    I2CM_1A1D_W(0x80, 0x5C, 0x4C),
    I2CM_1A1D_W(0x80, 0x5D, 0xF4),
    I2CM_1A1D_W(0x80, 0x5E, 0x1E),
    I2CM_1A1D_W(0x80, 0x62, 0x04),
    I2CM_1A1D_W(0x80, 0x63, 0x0F),
    I2CM_1A1D_W(0x80, 0x66, 0x04),
    I2CM_1A1D_W(0x80, 0x67, 0x50),
    I2CM_1A1D_W(0x80, 0x6A, 0x15),
    I2CM_1A1D_W(0x80, 0x7A, 0xA0),
    I2CM_1A1D_W(0x80, 0x9D, 0x10),
    I2CM_1A1D_W(0x80, 0x4A, 0x05),
    I2CM_1A1D_W(0x80, 0x7E, 0xCD),
    I2CM_1A1D_W(0x80, 0x50, 0x02),
    I2CM_1A1D_W(0x80, 0x49, 0x10),
    I2CM_1A1D_W(0x80, 0x47, 0x02),
    I2CM_1A1D_W(0x80, 0x7B, 0x4A),
    I2CM_1A1D_W(0x80, 0x7C, 0x0C),
    I2CM_1A1D_W(0x80, 0x7F, 0x57),
    I2CM_1A1D_W(0x80, 0x8F, 0x80),
    I2CM_1A1D_W(0x80, 0x90, 0x00),
    I2CM_1A1D_W(0x80, 0x8C, 0xFF),
    I2CM_1A1D_W(0x80, 0x8D, 0xC7),
    I2CM_1A1D_W(0x80, 0x8E, 0x00),
    I2CM_1A1D_W(0x80, 0x8B, 0x01),
    I2CM_1A1D_W(0x80, 0x0C, 0x00),
    I2CM_1A1D_W(0x80, 0x69, 0x7C),
    I2CM_1A1D_W(0x80, 0x65, 0x04),
    I2CM_1A1D_W(0x80, 0x19, 0x20),
    I2CM_1A1D_W(0x80, 0x12, 0x00),

#if defined(SENSOR_INIT_CMDQ_MODE)
    /*Sensor runtime parameter*/
    SNR_SHUTTER_FPS_1A1D(0x80, 10000, 30000),   //shutter 10ms , fps 30
    SNR_GAIN_1A1D(0x80, 1024),
#endif

#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 idle*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_READY),
    VIF_REG_DUMMY(CMDQ_STATUS_READY),

    /*append dummy*/
    //CMDQ_TAG('EOT'),    //end of table
    CMDQ_NULL(),
    CMDQ_NULL(),
    CMDQ_NULL(),
    CMDQ_NULL(),
#endif
};

#endif
