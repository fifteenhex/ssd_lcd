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

#ifndef _GC1054_INIT_TABLE_H_
#define _GC1054_INIT_TABLE_H_

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_MCLK_EN(0x7),               //0x7=MCLK 24MhZ, 0xB=37.125MHz
    SNR_RST(0x0),       //reset off
    CMDQ_DELAY_MS(5),                //T1, delay 5ms
    SNR_PDWN(0x1),     //power off
    CMDQ_DELAY_MS(5),                 //T2, delay 5ms

    SNR_PDWN(0x0),     //power on
    CMDQ_DELAY_MS(5),                  //T3, delay 5ms
    SNR_RST(0x1),       //reset on
    CMDQ_DELAY_MS(5), 
    SNR_PDWN(0x0),     //power on
    CMDQ_DELAY_MS(5),                 //T4, delay 5ms
    /*I2C timing*/
    I2CM_CLK_EN(0x2),  //24MHz
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

    I2CM_1A1D_W(0x42,0xfe,0x00),
    I2CM_1A1D_W(0x42,0xf2,0x00),
    I2CM_1A1D_W(0x42,0xf6,0x00),
    I2CM_1A1D_W(0x42,0xfc,0x04),
    I2CM_1A1D_W(0x42,0xf7,0x01),
    I2CM_1A1D_W(0x42,0xf8,0x0d),
    I2CM_1A1D_W(0x42,0xf9,0x06),
    I2CM_1A1D_W(0x42,0xfa,0x80),
    I2CM_1A1D_W(0x42,0xfc,0x0e),
    I2CM_1A1D_W(0x42,0xfe,0x00),
    I2CM_1A1D_W(0x42,0x03,0x02),
    I2CM_1A1D_W(0x42,0x04,0xa6),
    I2CM_1A1D_W(0x42,0x05,0x02), //HB
    I2CM_1A1D_W(0x42,0x06,0x06),
    I2CM_1A1D_W(0x42,0x07,0x00), //VB
    I2CM_1A1D_W(0x42,0x08,0x48),
    //I2CM_1A1D_W(0x42,0x08,0x3c),
    //I2CM_1A1D_W(0x42,0x08,0xe6),
    I2CM_1A1D_W(0x42,0x09,0x00),
    I2CM_1A1D_W(0x42,0x0a,0x04), //row start
    I2CM_1A1D_W(0x42,0x0b,0x00),
    I2CM_1A1D_W(0x42,0x0c,0x00), //col start
    I2CM_1A1D_W(0x42,0x0d,0x02), 
    I2CM_1A1D_W(0x42,0x0e,0xd4), //height 724
    I2CM_1A1D_W(0x42,0x0f,0x05), 
    I2CM_1A1D_W(0x42,0x10,0x08), //width 1288
    I2CM_1A1D_W(0x42,0x17,0xc0),
    I2CM_1A1D_W(0x42,0x18,0x02),
    I2CM_1A1D_W(0x42,0x19,0x08),
    I2CM_1A1D_W(0x42,0x1a,0x18),
    I2CM_1A1D_W(0x42,0x1d,0x12),
    I2CM_1A1D_W(0x42,0x1e,0x50),
    I2CM_1A1D_W(0x42,0x1f,0x80),
    I2CM_1A1D_W(0x42,0x21,0x30),
    I2CM_1A1D_W(0x42,0x23,0xf8),
    I2CM_1A1D_W(0x42,0x25,0x10),
    I2CM_1A1D_W(0x42,0x28,0x20),
    I2CM_1A1D_W(0x42,0x34,0x08), //data low
    I2CM_1A1D_W(0x42,0x3c,0x10),
    I2CM_1A1D_W(0x42,0x3d,0x0e),
    I2CM_1A1D_W(0x42,0xcc,0x8e),
    I2CM_1A1D_W(0x42,0xcd,0x9a),
    I2CM_1A1D_W(0x42,0xcf,0x70),
    I2CM_1A1D_W(0x42,0xd0,0xa9),
    I2CM_1A1D_W(0x42,0xd1,0xc5),
    I2CM_1A1D_W(0x42,0xd2,0xed), //data high
    I2CM_1A1D_W(0x42,0xd8,0x3c), //dacin offset
    I2CM_1A1D_W(0x42,0xd9,0x7a),
    I2CM_1A1D_W(0x42,0xda,0x12),
    I2CM_1A1D_W(0x42,0xdb,0x50),
    I2CM_1A1D_W(0x42,0xde,0x0c),
    I2CM_1A1D_W(0x42,0xe3,0x60),
    I2CM_1A1D_W(0x42,0xe4,0x78),
    I2CM_1A1D_W(0x42,0xfe,0x01),
    I2CM_1A1D_W(0x42,0xe3,0x01),
    I2CM_1A1D_W(0x42,0xe6,0x10), //ramps offset
    I2CM_1A1D_W(0x42,0xfe,0x01),
    I2CM_1A1D_W(0x42,0x80,0x50),
    I2CM_1A1D_W(0x42,0x88,0x73),
    I2CM_1A1D_W(0x42,0x89,0x03),
    I2CM_1A1D_W(0x42,0x90,0x01), 
    I2CM_1A1D_W(0x42,0x92,0x02), //crop win 2<=y<=4
    I2CM_1A1D_W(0x42,0x94,0x03), //crop win 2<=x<=5
    I2CM_1A1D_W(0x42,0x95,0x02), //crop win height
    I2CM_1A1D_W(0x42,0x96,0xd0),
    I2CM_1A1D_W(0x42,0x97,0x05), //crop win width
    I2CM_1A1D_W(0x42,0x98,0x00),
    I2CM_1A1D_W(0x42,0xfe,0x01),
    I2CM_1A1D_W(0x42,0x40,0x22),
    I2CM_1A1D_W(0x42,0x43,0x03),
    I2CM_1A1D_W(0x42,0x4e,0x3c),
    I2CM_1A1D_W(0x42,0x4f,0x00),
    I2CM_1A1D_W(0x42,0x60,0x00),
    I2CM_1A1D_W(0x42,0x61,0x80),
    I2CM_1A1D_W(0x42,0xfe,0x01),
    I2CM_1A1D_W(0x42,0xb0,0x48),
    I2CM_1A1D_W(0x42,0xb1,0x01), 
    I2CM_1A1D_W(0x42,0xb2,0x00), 
    I2CM_1A1D_W(0x42,0xb6,0x00), 
    I2CM_1A1D_W(0x42,0xfe,0x02),
    I2CM_1A1D_W(0x42,0x01,0x00),
    I2CM_1A1D_W(0x42,0x02,0x01),
    I2CM_1A1D_W(0x42,0x03,0x02),
    I2CM_1A1D_W(0x42,0x04,0x03),
    I2CM_1A1D_W(0x42,0x05,0x04),
    I2CM_1A1D_W(0x42,0x06,0x05),
    I2CM_1A1D_W(0x42,0x07,0x06),
    I2CM_1A1D_W(0x42,0x08,0x0e),
    I2CM_1A1D_W(0x42,0x09,0x16),
    I2CM_1A1D_W(0x42,0x0a,0x1e),
    I2CM_1A1D_W(0x42,0x0b,0x36),
    I2CM_1A1D_W(0x42,0x0c,0x3e),
    I2CM_1A1D_W(0x42,0x0d,0x56),
    I2CM_1A1D_W(0x42,0xfe,0x02),
    I2CM_1A1D_W(0x42,0xb0,0x00), //col_gain[11:8]
    I2CM_1A1D_W(0x42,0xb1,0x00),
    I2CM_1A1D_W(0x42,0xb2,0x00),
    I2CM_1A1D_W(0x42,0xb3,0x11),
    I2CM_1A1D_W(0x42,0xb4,0x22),
    I2CM_1A1D_W(0x42,0xb5,0x54),
    I2CM_1A1D_W(0x42,0xb6,0xb8),
    I2CM_1A1D_W(0x42,0xb7,0x60),
    I2CM_1A1D_W(0x42,0xb9,0x00), //col_gain[12]
    I2CM_1A1D_W(0x42,0xba,0xc0),
    I2CM_1A1D_W(0x42,0xc0,0x20), //col_gain[7:0]
    I2CM_1A1D_W(0x42,0xc1,0x2d),
    I2CM_1A1D_W(0x42,0xc2,0x40),
    I2CM_1A1D_W(0x42,0xc3,0x5b),
    I2CM_1A1D_W(0x42,0xc4,0x80),
    I2CM_1A1D_W(0x42,0xc5,0xb5),
    I2CM_1A1D_W(0x42,0xc6,0x00),
    I2CM_1A1D_W(0x42,0xc7,0x6a),
    I2CM_1A1D_W(0x42,0xc8,0x00),
    I2CM_1A1D_W(0x42,0xc9,0xd4),
    I2CM_1A1D_W(0x42,0xca,0x00),
    I2CM_1A1D_W(0x42,0xcb,0xa8),
    I2CM_1A1D_W(0x42,0xcc,0x00),
    I2CM_1A1D_W(0x42,0xcd,0x50),
    I2CM_1A1D_W(0x42,0xce,0x00),
    I2CM_1A1D_W(0x42,0xcf,0xa1),
    I2CM_1A1D_W(0x42,0xfe,0x02),
    I2CM_1A1D_W(0x42,0x54,0xf7),
    I2CM_1A1D_W(0x42,0x55,0xf0),
    I2CM_1A1D_W(0x42,0x56,0x00),
    I2CM_1A1D_W(0x42,0x57,0x00),
    I2CM_1A1D_W(0x42,0x58,0x00),
    I2CM_1A1D_W(0x42,0x5a,0x04),
    I2CM_1A1D_W(0x42,0xfe,0x04),
    I2CM_1A1D_W(0x42,0x81,0x8a),
    I2CM_1A1D_W(0x42,0xfe,0x03),
    I2CM_1A1D_W(0x42,0x01,0x03),
    I2CM_1A1D_W(0x42,0x02,0x11),
    I2CM_1A1D_W(0x42,0x03,0x90),
    I2CM_1A1D_W(0x42,0x10,0x90),
    I2CM_1A1D_W(0x42,0x11,0x2b),
    I2CM_1A1D_W(0x42,0x12,0x40), //lwc 1280*5/4
    I2CM_1A1D_W(0x42,0x13,0x06),
    I2CM_1A1D_W(0x42,0x15,0x06),
    I2CM_1A1D_W(0x42,0x21,0x02),
    I2CM_1A1D_W(0x42,0x22,0x02),
    I2CM_1A1D_W(0x42,0x23,0x08),
    I2CM_1A1D_W(0x42,0x24,0x02),
    I2CM_1A1D_W(0x42,0x25,0x10),
    I2CM_1A1D_W(0x42,0x26,0x04),
    I2CM_1A1D_W(0x42,0x29,0x02),
    I2CM_1A1D_W(0x42,0x2a,0x02),
    I2CM_1A1D_W(0x42,0x2b,0x04),
    I2CM_1A1D_W(0x42,0xfe,0x00),

#if defined(SENSOR_INIT_CMDQ_MODE)
    /*Sensor runtime parameter*/
    SNR_SHUTTER_FPS_2A1D(0x42, 10000, 30000),   //shutter 10ms , fps 30
    SNR_GAIN_2A1D(0x42, 1024),
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