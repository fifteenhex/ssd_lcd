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

#ifndef _IMX323_INIT_TABLE_H_

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    //CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_PDWN(0),                   //pdwn low
    CMDQ_DELAY_10US(10),           //T1, delay 100us
    SNR_RST(0),                    //reset low
    CMDQ_DELAY_MS(1),              //T3, delay 1ms
    SNR_RST(1),                    //reset high
    CMDQ_DELAY_10US(10),           //T2, delay 100us
    SNR_MCLK_EN(0x7),              //MCLK 27MhZ
    CMDQ_DELAY_MS(3),              //t4, delay 3ms

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
    I2CM_2A1D_W(0x34, 0x0100, 0x00),
    I2CM_2A1D_W(0x34, 0x0008, 0x00),
    I2CM_2A1D_W(0x34, 0x0009, 0xF0),
    I2CM_2A1D_W(0x34, 0x0101, 0x00),
    I2CM_2A1D_W(0x34, 0x0104, 0x00),
    I2CM_2A1D_W(0x34, 0x0112, 0x0C),
    I2CM_2A1D_W(0x34, 0x0113, 0x0C),
    I2CM_2A1D_W(0x34, 0x0202, 0x00),
    I2CM_2A1D_W(0x34, 0x0203, 0x00),
    I2CM_2A1D_W(0x34, 0x0340, 0x04),
    I2CM_2A1D_W(0x34, 0x0341, 0x65),
    I2CM_2A1D_W(0x34, 0x0342, 0x08),
    I2CM_2A1D_W(0x34, 0x0343, 0x98),
    I2CM_2A1D_W(0x34, 0x0003, 0x08),
    I2CM_2A1D_W(0x34, 0x0004, 0xC8),
    I2CM_2A1D_W(0x34, 0x0005, 0x04),
    I2CM_2A1D_W(0x34, 0x0006, 0x65),
    I2CM_2A1D_W(0x34, 0x0011, 0x01),
    I2CM_2A1D_W(0x34, 0x009A, 0x44),
    I2CM_2A1D_W(0x34, 0x009B, 0x0C),
    I2CM_2A1D_W(0x34, 0x0344, 0x00),
    I2CM_2A1D_W(0x34, 0x0345, 0x00),
    I2CM_2A1D_W(0x34, 0x0346, 0x00),
    I2CM_2A1D_W(0x34, 0x0347, 0x00),
    I2CM_2A1D_W(0x34, 0x034C, 0x07),
    I2CM_2A1D_W(0x34, 0x034D, 0xC0),
    I2CM_2A1D_W(0x34, 0x034E, 0x04),
    I2CM_2A1D_W(0x34, 0x034F, 0xC9),
    I2CM_2A1D_W(0x34, 0x3000, 0x31),
    I2CM_2A1D_W(0x34, 0x3001, 0x00),
    I2CM_2A1D_W(0x34, 0x3002, 0x0F),
    I2CM_2A1D_W(0x34, 0x300D, 0x00),
    I2CM_2A1D_W(0x34, 0x300E, 0x00),
    I2CM_2A1D_W(0x34, 0x300F, 0x00),
    I2CM_2A1D_W(0x34, 0x3010, 0x00),
    I2CM_2A1D_W(0x34, 0x3011, 0x01),
    I2CM_2A1D_W(0x34, 0x3012, 0x82),
    I2CM_2A1D_W(0x34, 0x3016, 0x3C),
    I2CM_2A1D_W(0x34, 0x3017, 0x00),
    I2CM_2A1D_W(0x34, 0x301E, 0x00),
    I2CM_2A1D_W(0x34, 0x301F, 0x73),
    I2CM_2A1D_W(0x34, 0x3020, 0x3C),
    I2CM_2A1D_W(0x34, 0x3021, 0x20),
    I2CM_2A1D_W(0x34, 0x3022, 0x00),
    I2CM_2A1D_W(0x34, 0x3027, 0x20),
    I2CM_2A1D_W(0x34, 0x302C, 0x01),
    I2CM_2A1D_W(0x34, 0x303B, 0xE1),
    I2CM_2A1D_W(0x34, 0x307A, 0x00),
    I2CM_2A1D_W(0x34, 0x307B, 0x00),
    I2CM_2A1D_W(0x34, 0x3098, 0x26),
    I2CM_2A1D_W(0x34, 0x3099, 0x02),
    I2CM_2A1D_W(0x34, 0x309A, 0x26),
    I2CM_2A1D_W(0x34, 0x309B, 0x02),
    I2CM_2A1D_W(0x34, 0x30CE, 0x16),
    I2CM_2A1D_W(0x34, 0x30CF, 0x82),
    I2CM_2A1D_W(0x34, 0x30D0, 0x00),
    I2CM_2A1D_W(0x34, 0x3117, 0x0D),
    I2CM_2A1D_W(0x34, 0x302C, 0x00),
    I2CM_2A1D_W(0x34, 0x0100, 0x01),

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
