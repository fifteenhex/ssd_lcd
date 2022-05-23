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
#ifndef _SC210IOT_INIT_TABLE_H_

#define _SC210IOT_PDWN_ON_ 0
#define _SC210IOT_RST_ON_ 0

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_PDWN(_SC210IOT_PDWN_ON_),     //power off
    SNR_RST(_SC210IOT_RST_ON_),       //reset  off
     CMDQ_DELAY_MS(1),             //T1 delay 1ms
     SNR_MCLK_EN(0x0), //MCLK //0x0=MCLK 27MhZ, 0xB=37.125MHz
     CMDQ_DELAY_MS(2),   //T2 delay 2ms
     SNR_RST(~_SC210IOT_RST_ON_),       //reset  on
     CMDQ_DELAY_MS(1),  //T3 delay 1ms

    SNR_PDWN(~_SC210IOT_PDWN_ON_),    //power on
    CMDQ_DELAY_MS(2),     //T4 delay 2ms



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


    I2CM_2A1D_W(0x64,0x0103, 0x01),   // standby
    I2CM_2A1D_W(0x64,0xffff,  50),   // BCWAIT_TIME[9:0]
    I2CM_2A1D_W(0x64,0x0100,0x00),   // BCWAIT_TIME[9:0]
    I2CM_2A1D_W(0x64,0x36e9,0x80),    // CPWAIT_TIME[9:0]
    I2CM_2A1D_W(0x64,0x36f9,0x80),    // VMAX[19:0]
    I2CM_2A1D_W(0x64,0x3622,0x06),
    I2CM_2A1D_W(0x64,0x3630,0xc8),    //HMAX
    I2CM_2A1D_W(0x64,0x3637,0x15),
    I2CM_2A1D_W(0x64,0x3638,0x46),    // ADBIT[1:0]
    I2CM_2A1D_W(0x64,0x3253,0x0c),    // MDBIT
    I2CM_2A1D_W(0x64,0x3333,0x10),    // SYS_MODE[3:0]
    I2CM_2A1D_W(0x64,0x3304,0x68),    // SHR0[19:0]
    I2CM_2A1D_W(0x64,0x331e,0x61),    // INCKSEL2[7:0]
    I2CM_2A1D_W(0x64,0x3306,0x40),    // XVS_DRV[1:0]
    I2CM_2A1D_W(0x64,0x330b,0xc6),    // INCKSEL3[10:0]
    I2CM_2A1D_W(0x64,0x3633,0x43),    // INCKSEL4[10:0]
    I2CM_2A1D_W(0x64,0x3634,0x54),    // INCKSEL5[7:0]
    I2CM_2A1D_W(0x64,0x3631,0x84),//
    I2CM_2A1D_W(0x64,0x3301,0x06),//
    I2CM_2A1D_W(0x64,0x363a,0x1f),//
    I2CM_2A1D_W(0x64,0x36f9,0xa4),
    I2CM_2A1D_W(0x64,0x36fa,0x35),
    I2CM_2A1D_W(0x64,0x36fc,0x11),
    I2CM_2A1D_W(0x64,0x36fd,0x14),
    I2CM_2A1D_W(0x64,0x36f9,0x24),  // 2 lane for phy
    I2CM_2A1D_W(0x64,0x3364,0x17),//Y-out
    I2CM_2A1D_W(0x64,0x3390,0x08),
    I2CM_2A1D_W(0x64,0x3391,0x18),  // 2 lane
    I2CM_2A1D_W(0x64,0x3392,0x38),
    I2CM_2A1D_W(0x64,0x3301,0x06),
    I2CM_2A1D_W(0x64,0x3393,0x08),
    I2CM_2A1D_W(0x64,0x3394,0x0a),
    I2CM_2A1D_W(0x64,0x3395,0x50),
    I2CM_2A1D_W(0x64,0x3670,0x08),
    I2CM_2A1D_W(0x64,0x369c,0x08),
    I2CM_2A1D_W(0x64,0x369d,0x38),
    I2CM_2A1D_W(0x64,0x3690,0x53),
    I2CM_2A1D_W(0x64,0x3691,0x63),
    I2CM_2A1D_W(0x64,0x3692,0x63),//0x9c
    I2CM_2A1D_W(0x64,0x3670,0x0c),   // operating
    I2CM_2A1D_W(0x64,0x367e,0x08),//
    I2CM_2A1D_W(0x64,0x367f,0x38),//
    I2CM_2A1D_W(0x64,0x3677,0x84),//
    I2CM_2A1D_W(0x64,0x3678,0x84),
    I2CM_2A1D_W(0x64,0x3679,0x88),
    I2CM_2A1D_W(0x64,0x36e9,0x20),
    I2CM_2A1D_W(0x64,0x36f9,0x24),//
    I2CM_2A1D_W(0x64,0x0100,0x01),//

	//CMDQ_DELAY_MS(50),
    I2CM_2A1D_W(0x64,0x3802, 0x00),  //sync

#if defined(SENSOR_INIT_CMDQ_MODE)
    /*Sensor runtime parameter*/
   // I2CM_2A1D_W(0x64,0x3001, 0x01),//group hold begin
    //SNR_SHUTTER_FPS_2A1D(0x64, 10000, 30000),   //shutter 10ms , fps 30
    //SNR_GAIN_2A1D(0x64, 1024),
    //I2CM_2A1D_W(0x64,0x3001, 0x00),//group hold end
#endif

    CMDQ_DELAY_MS(2),
    //I2CM_2A1D_W(0x64,0x308c, 0x21),  //pattern mode config

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
