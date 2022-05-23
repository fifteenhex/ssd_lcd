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

#ifndef _IMX307_INIT_TABLE_H_

#define _IMX307_PDWN_ON_ 0
#define _IMX307_RST_ON_ 0

//static CMDQ_CMDS gTable[] __attribute__((aligned(8))) =
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),

    /*sensor reset*/
    SNR_PDWN(~_IMX307_PDWN_ON_),     //power on
    CMDQ_DELAY_10US(10),             //T1, delay 100us
    SNR_RST(~_IMX307_RST_ON_),       //reset off
    CMDQ_DELAY_MS(1),                //T3, delay 1ms

    SNR_RST(_IMX307_RST_ON_),       //reset off
    SNR_MCLK_EN(0xB),               //0x7=MCLK 27MhZ, 0xB=37.125MHz
    CMDQ_DELAY_10US(2),             //T2, delay 20us

    SNR_PDWN(_IMX307_PDWN_ON_),     //power off
    CMDQ_DELAY_10US(2),             //T1, delay 20us
    SNR_PDWN(~_IMX307_PDWN_ON_),    //power on
    CMDQ_DELAY_10US(2),             //T1, delay 20us
    SNR_RST(~_IMX307_RST_ON_),      //reset off
    CMDQ_DELAY_10US(2),             //T1, delay 20us

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
    I2CM_2A1D_W(0x34,0x3002, 0x01),   //Master mode stop
    CMDQ_DELAY_MS(2),
    I2CM_2A1D_W(0x34,0x3000, 0x01),   // standby
    CMDQ_DELAY_MS(2),
    I2CM_2A1D_W(0x34,0x3005, 0x01),
    I2CM_2A1D_W(0x34,0x3009, 0x02),//FRSEL
    I2CM_2A1D_W(0x34,0x3011, 0x0A),
    I2CM_2A1D_W(0x34,0x3016, 0x08),//yc modify
    I2CM_2A1D_W(0x34,0x3018, 0x65),//VMAX
    I2CM_2A1D_W(0x34,0x3019, 0x04),
    I2CM_2A1D_W(0x34,0x301c, 0x30),//0x1167 HMAX,for 25fps
    I2CM_2A1D_W(0x34,0x301d, 0x11),
    I2CM_2A1D_W(0x34,0x305C, 0x18),//INCK
    I2CM_2A1D_W(0x34,0x305D, 0x03),
    I2CM_2A1D_W(0x34,0x305E, 0x20),
    I2CM_2A1D_W(0x34,0x309E, 0x4A),
    I2CM_2A1D_W(0x34,0x309F, 0x4A),
    I2CM_2A1D_W(0x34,0x311C, 0x0E),
    I2CM_2A1D_W(0x34,0x3128, 0x04),
    I2CM_2A1D_W(0x34,0x3129, 0x00),
    I2CM_2A1D_W(0x34,0x313B, 0x41),
    I2CM_2A1D_W(0x34,0x315E, 0x1A),//INCKSEL5
    I2CM_2A1D_W(0x34,0x3164, 0x1A),//INCKSEL6
    I2CM_2A1D_W(0x34,0x317C, 0x00),//ADBIT2
    I2CM_2A1D_W(0x34,0x317E, 0x00),
    I2CM_2A1D_W(0x34,0x31EC, 0x0E),
    I2CM_2A1D_W(0x34,0x3404, 0x01),
    I2CM_2A1D_W(0x34,0x3405, 0x10),
    I2CM_2A1D_W(0x34,0x3407, 0x01),  // 2 lane for phy
    I2CM_2A1D_W(0x34,0x3418, 0x49),//Y-out
    I2CM_2A1D_W(0x34,0x3419, 0x04),
    I2CM_2A1D_W(0x34,0x3443, 0x01),  // 2 lane
    I2CM_2A1D_W(0x34,0x3444, 0x20),
    I2CM_2A1D_W(0x34,0x3445, 0x25),
    I2CM_2A1D_W(0x34,0x3446, 0x57),
    I2CM_2A1D_W(0x34,0x3448, 0x37),
    I2CM_2A1D_W(0x34,0x344A, 0x1F),
    I2CM_2A1D_W(0x34,0x344C, 0x1F),
    I2CM_2A1D_W(0x34,0x344E, 0x1f),
    I2CM_2A1D_W(0x34,0x3450, 0x77),
    I2CM_2A1D_W(0x34,0x3452, 0x1F),
    I2CM_2A1D_W(0x34,0x3454, 0x17),
    I2CM_2A1D_W(0x34,0x3480, 0x49),//0x9c
    I2CM_2A1D_W(0x34,0x3000, 0x00),   // operating

#if defined(SENSOR_INIT_CMDQ_MODE)
    /*Sensor runtime parameter*/
    I2CM_2A1D_W(0x34,0x3001, 0x01),//group hold begin
    SNR_SHUTTER_FPS_2A1D(0x34, 10000, 30000),   //shutter 10ms , fps 30
    SNR_GAIN_2A1D(0x34, 1024),
    I2CM_2A1D_W(0x34,0x3001, 0x00),//group hold end
#endif

    CMDQ_DELAY_MS(10),
    I2CM_2A1D_W(0x34,0x308c, 0x20),  //pattern mode config
    I2CM_2A1D_W(0x34,0x3002, 0x00),  //Trigger master mode start

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
