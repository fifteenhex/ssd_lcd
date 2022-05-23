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

#ifndef _IMX415_INIT_TABLE_H_

#define _IMX415_PDWN_ON_ 0
#define _IMX415_RST_ON_ 0

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_PDWN(_IMX415_PDWN_ON_),     //power off
    SNR_RST(_IMX415_RST_ON_),       //reset  off
    SNR_PDWN(~_IMX415_PDWN_ON_),    //power on
    CMDQ_DELAY_MS(40),             // delay 31ms
    SNR_RST(~_IMX415_RST_ON_),      //reset on
    CMDQ_DELAY_10US(1),             //delay 10us
    SNR_MCLK_EN(0x0),               //0x0=MCLK 27MhZ, 0xB=37.125MHz


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

    I2CM_2A1D_W(0x34,0x3000, 0x01),   // standby
    I2CM_2A1D_W(0x34,0x3002, 0x01),   //Master mode stop
    I2CM_2A1D_W(0x34,0x3008, 0x5D),   // BCWAIT_TIME[9:0]
    I2CM_2A1D_W(0x34,0x300A, 0x42),    // CPWAIT_TIME[9:0]
    I2CM_2A1D_W(0x34,0x3024, 0xCA),    // VMAX[19:0]
    I2CM_2A1D_W(0x34,0x3025, 0x08),
    I2CM_2A1D_W(0x34,0x3028, 0x4C),    //HMAX
    I2CM_2A1D_W(0x34,0x3029, 0x04),
    I2CM_2A1D_W(0x34,0x3031, 0x00),    // ADBIT[1:0]
    I2CM_2A1D_W(0x34,0x3032, 0x00),    // MDBIT
    I2CM_2A1D_W(0x34,0x3033, 0x05),    // SYS_MODE[3:0]
    I2CM_2A1D_W(0x34,0x3050, 0x08),    // SHR0[19:0]
    I2CM_2A1D_W(0x34,0x30C1, 0x00),    // XVS_DRV[1:0]
    I2CM_2A1D_W(0x34,0x3116, 0x23),    // INCKSEL2[7:0]
    I2CM_2A1D_W(0x34,0x3118, 0xC6),    // INCKSEL3[10:0]
    I2CM_2A1D_W(0x34,0x311A, 0xE7),    // INCKSEL4[10:0]
    I2CM_2A1D_W(0x34,0x311E, 0x23),    // INCKSEL5[7:0]
    I2CM_2A1D_W(0x34,0x32D4, 0x21),//
    I2CM_2A1D_W(0x34,0x32EC, 0xA1),//
    I2CM_2A1D_W(0x34,0x3452, 0x7F),//
    I2CM_2A1D_W(0x34,0x3453, 0x03),
    I2CM_2A1D_W(0x34,0x358A, 0x04),
    I2CM_2A1D_W(0x34,0x35A1, 0x02),
    I2CM_2A1D_W(0x34,0x36BC, 0x0C),
    I2CM_2A1D_W(0x34,0x36CC, 0x53),  // 2 lane for phy
    I2CM_2A1D_W(0x34,0x36CD, 0x00),//Y-out
    I2CM_2A1D_W(0x34,0x36CE, 0x3C),
    I2CM_2A1D_W(0x34,0x36D0, 0x8C),  // 2 lane
    I2CM_2A1D_W(0x34,0x36D1, 0x00),
    I2CM_2A1D_W(0x34,0x36D2, 0x71),
    I2CM_2A1D_W(0x34,0x36D4, 0x3C),
    I2CM_2A1D_W(0x34,0x36D6, 0x53),
    I2CM_2A1D_W(0x34,0x36D7, 0x00),
    I2CM_2A1D_W(0x34,0x36D8, 0x71),
    I2CM_2A1D_W(0x34,0x36DA, 0x8C),
    I2CM_2A1D_W(0x34,0x36DB, 0x00),
    I2CM_2A1D_W(0x34,0x3701, 0x00),
    I2CM_2A1D_W(0x34,0x3724, 0x02),
    I2CM_2A1D_W(0x34,0x3726, 0x02),//0x9c
    I2CM_2A1D_W(0x34,0x3732, 0x02),   // operating
    I2CM_2A1D_W(0x34,0x3734, 0x03),//
    I2CM_2A1D_W(0x34,0x3736, 0x03),//
    I2CM_2A1D_W(0x34,0x3742, 0x03),//
    I2CM_2A1D_W(0x34,0x3862, 0xE0),
    I2CM_2A1D_W(0x34,0x38CC, 0x30),
    I2CM_2A1D_W(0x34,0x38CD, 0x2F),
    I2CM_2A1D_W(0x34,0x395C, 0x0C),//
    I2CM_2A1D_W(0x34,0x3A42, 0xD1),//
    I2CM_2A1D_W(0x34,0x3A4C, 0x77),//
    I2CM_2A1D_W(0x34,0x3AE0, 0x02),
    I2CM_2A1D_W(0x34,0x3AEC, 0x0C),
    I2CM_2A1D_W(0x34,0x3B00, 0x2E),
    I2CM_2A1D_W(0x34,0x3B06, 0x29),//
    I2CM_2A1D_W(0x34,0x3B98, 0x25),//
    I2CM_2A1D_W(0x34,0x3B99, 0x21),//
    I2CM_2A1D_W(0x34,0x3B9B, 0x13),
    I2CM_2A1D_W(0x34,0x3B9C, 0x13),
    I2CM_2A1D_W(0x34,0x3B9D, 0x13),
    I2CM_2A1D_W(0x34,0x3B9E, 0x13),//
    I2CM_2A1D_W(0x34,0x3BA1, 0x00),//
    I2CM_2A1D_W(0x34,0x3BA2, 0x06),//
    I2CM_2A1D_W(0x34,0x3BA3, 0x0B),
    I2CM_2A1D_W(0x34,0x3BA4, 0x10),
    I2CM_2A1D_W(0x34,0x3BA5, 0x14),
    I2CM_2A1D_W(0x34,0x3BA6, 0x18),//
    I2CM_2A1D_W(0x34,0x3BA7, 0x1A),//
    I2CM_2A1D_W(0x34,0x3BA8, 0x1A),//
    I2CM_2A1D_W(0x34,0x3BA9, 0x1A),
    I2CM_2A1D_W(0x34,0x3BAC, 0xED),
    I2CM_2A1D_W(0x34,0x3BAD, 0x01),
    I2CM_2A1D_W(0x34,0x3BAE, 0xF6),//
    I2CM_2A1D_W(0x34,0x3BAF, 0x02),//
    I2CM_2A1D_W(0x34,0x3BB0, 0xA2),//
    I2CM_2A1D_W(0x34,0x3BB1, 0x03),
    I2CM_2A1D_W(0x34,0x3BB2, 0xE0),
    I2CM_2A1D_W(0x34,0x3BB3, 0x03),
    I2CM_2A1D_W(0x34,0x3BB4, 0xE0),//
    I2CM_2A1D_W(0x34,0x3BB5, 0x03),//
    I2CM_2A1D_W(0x34,0x3BB6, 0xE0),//
    I2CM_2A1D_W(0x34,0x3BB7, 0x03),
    I2CM_2A1D_W(0x34,0x3BB8, 0xE0),
    I2CM_2A1D_W(0x34,0x3BBA, 0xE0),
    I2CM_2A1D_W(0x34,0x3BBC, 0xDA),//
    I2CM_2A1D_W(0x34,0x3BBE, 0x88),//
    I2CM_2A1D_W(0x34,0x3BC0, 0x44),//
    I2CM_2A1D_W(0x34,0x3BC2, 0x7B),
    I2CM_2A1D_W(0x34,0x3BC4, 0xA2),
    I2CM_2A1D_W(0x34,0x3BC8, 0xBD),
    I2CM_2A1D_W(0x34,0x3BCA, 0xBD),//
    I2CM_2A1D_W(0x34,0x4000, 0x11),//mipi clock non-continuous mode enble
    I2CM_2A1D_W(0x34,0x4004, 0xC0),//
    I2CM_2A1D_W(0x34,0x4005, 0x06),//
    I2CM_2A1D_W(0x34,0x400C, 0x00),
    I2CM_2A1D_W(0x34,0x4018, 0x7F),
    I2CM_2A1D_W(0x34,0x401A, 0x37),
    I2CM_2A1D_W(0x34,0x401C, 0x37),//
    I2CM_2A1D_W(0x34,0x401E, 0xF7),//
    I2CM_2A1D_W(0x34,0x401F, 0x00),//
    I2CM_2A1D_W(0x34,0x4020, 0x3F),
    I2CM_2A1D_W(0x34,0x4022, 0x6F),
    I2CM_2A1D_W(0x34,0x4024, 0x3F),
    I2CM_2A1D_W(0x34,0x4026, 0x5F),//
    I2CM_2A1D_W(0x34,0x4028, 0x2F),//
    I2CM_2A1D_W(0x34,0x4074, 0x01),//
    CMDQ_DELAY_MS(2),
    I2CM_2A1D_W(0x34,0x3002, 0x00),
    CMDQ_DELAY_MS(2),
    I2CM_2A1D_W(0x34,0x3000, 0x00),//


#if defined(SENSOR_INIT_CMDQ_MODE)
    /*Sensor runtime parameter*/
    I2CM_2A1D_W(0x34,0x3001, 0x01),//group hold begin
    SNR_SHUTTER_FPS_2A1D(0x34, 10000, 30000),   //shutter 10ms , fps 30
    SNR_GAIN_2A1D(0x34, 1024),
    I2CM_2A1D_W(0x34,0x3001, 0x00),//group hold end
#endif

    CMDQ_DELAY_MS(10),
    I2CM_2A1D_W(0x34,0x308c, 0x21),  //pattern mode config

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
