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

#ifndef _PS5250_MIPI_INIT_TABLE_H_

#if 1

#if 0 //begin compact setting , 2019/08
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
        /*set cmdq0 busy*/
        CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
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

        /*sensor initial table*/
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x05 ,0x03),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x0F ,0x00),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x42 ,0x00),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x43 ,0x06),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xF5 ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x00), //bank0
        I2CM_1A1D_W( 0x90, 0x10 ,0x80), //Cmd_BYTECLK_InvSel=1
        I2CM_1A1D_W( 0x90, 0x81 ,0x80),//default=62 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0x9E ,0x08),
        I2CM_1A1D_W( 0x90, 0xA2 ,0x30),
        I2CM_1A1D_W( 0x90, 0xA3 ,0x03),
        I2CM_1A1D_W( 0x90, 0xD9 ,0x64),
        I2CM_1A1D_W( 0x90, 0xDA ,0x12),
        I2CM_1A1D_W( 0x90, 0xDB ,0x84),
        I2CM_1A1D_W( 0x90, 0xDC ,0x31),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x01), //bank1
        I2CM_1A1D_W( 0x90, 0x04 ,0x10),
        I2CM_1A1D_W( 0x90, 0x05 ,0x0B),
        I2CM_1A1D_W( 0x90, 0x0A ,0x04),
        I2CM_1A1D_W( 0x90, 0x0B ,0x64),
        I2CM_1A1D_W( 0x90, 0x0D ,0x02),
        I2CM_1A1D_W( 0x90, 0x1E ,0x46), //default=43 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x27 ,0x08),
        I2CM_1A1D_W( 0x90, 0x28 ,0xCC),
        I2CM_1A1D_W( 0x90, 0x29 ,0x02), //default=00 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x2B ,0x93), //default=C8 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x2E ,0x78), //default=5A 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0x41 ,0x0F), //default=1F 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x42 ,0xC8), //default=68 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x4A ,0x21), //default=22 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x4C ,0xBE), //default=5F 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x52 ,0xE8), //default=B8 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x56 ,0x0A), //default=64 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x60 ,0x78), //default=5A 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0x7C ,0x38), //default=0E 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0x8B ,0x46),
        I2CM_1A1D_W( 0x90, 0xA6 ,0x40),
        I2CM_1A1D_W( 0x90, 0xA8 ,0x00),
        I2CM_1A1D_W( 0x90, 0xAA ,0x88),
        I2CM_1A1D_W( 0x90, 0xD0 ,0xC2),
        I2CM_1A1D_W( 0x90, 0xD1 ,0x04), //default=44 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0xD3 ,0x17), //default=14 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0xD8 ,0x5E), //default=4E 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0xD9 ,0x66), //default=NC 20170608 JAMES
        //I2CM_1A1D_W( 0x90, 0xDD ,0x72), //default=62 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xDD ,0x52), //2018/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xDE ,0x43),
        I2CM_1A1D_W( 0x90, 0xE2 ,0x6D), //default=1D 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xE3 ,0x21),
        I2CM_1A1D_W( 0x90, 0xEA ,0x7A),
        I2CM_1A1D_W( 0x90, 0xF5 ,0x01),
        I2CM_1A1D_W( 0x90, 0xF6 ,0xC8),
        I2CM_1A1D_W( 0x90, 0xF7 ,0x02),
        I2CM_1A1D_W( 0x90, 0xF0 ,0x03),
        I2CM_1A1D_W( 0x90, 0xF4 ,0x06),
        I2CM_1A1D_W( 0x90, 0xF2 ,0x11),
        //I2CM_1A1D_W( 0x90, 0xF1 ,0x0A),
        I2CM_1A1D_W( 0x90, 0xF1 ,0x16), //2018/6/12 reduce light flare in corner
        //I2CM_1A1D_W( 0x90, 0xF5 ,0x11),
        I2CM_1A1D_W( 0x90, 0xF5 ,0x10), //2018/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xF8 ,0x00),//default=40 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xF9 ,0x15),
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),

#if defined(SENSOR_INIT_CMDQ_MODE)
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),         //bank 1
        /*Sensor runtime parameter*/
        SNR_SHUTTER_FPS( 0x90, 10000, 30000),   //shutter 10ms , fps 30
        SNR_GAIN( 0x90, 1024),                  //gain = 1x
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),         //trigger update
#endif

        I2CM_1A1D_W( 0x90, 0xEF ,0x02),//bank2
#if 0   //low sensitivity OBC
        I2CM_1A1D_W( 0x90, 0x21 ,0x18),
        I2CM_1A1D_W( 0x90, 0x22 ,0x0C),
        I2CM_1A1D_W( 0x90, 0x23 ,0x08),
        I2CM_1A1D_W( 0x90, 0x24 ,0x05),
        I2CM_1A1D_W( 0x90, 0x25 ,0x03),
        I2CM_1A1D_W( 0x90, 0x26 ,0x02),
        I2CM_1A1D_W( 0x90, 0x33 ,0x85),
#else //high sensitivity OBC
        I2CM_1A1D_W( 0x90, 0x21 ,0x0C),   //d1, reg 0x21, 12X
        I2CM_1A1D_W( 0x90, 0x22 ,0x08),   //d2, reg 0x22, 8X
        I2CM_1A1D_W( 0x90, 0x23 ,0x04),   //d3, reg 0x23, 4X
        I2CM_1A1D_W( 0x90, 0x24 ,0x06),   //d4, reg 0x24, UB=06,12x~32x
        I2CM_1A1D_W( 0x90, 0x25 ,0x05),   //d5, reg 0x25, UB=05,8x~12x
        I2CM_1A1D_W( 0x90, 0x26 ,0x04),   //d6, reg 0x26, UB=04,4x~8x
        I2CM_1A1D_W( 0x90, 0x33 ,0x81),//reg 0x33, UB=03,1x~4x
#endif
        I2CM_1A1D_W( 0x90, 0x47 ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x49 ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x4B ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x4D ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05), //bank5
        I2CM_1A1D_W( 0x90, 0x06 ,0x04),
        I2CM_1A1D_W( 0x90, 0x0A ,0x05),
        I2CM_1A1D_W( 0x90, 0x0C ,0x04),
        I2CM_1A1D_W( 0x90, 0x0F ,0x00),
        I2CM_1A1D_W( 0x90, 0x10 ,0x02),
        I2CM_1A1D_W( 0x90, 0x11 ,0x01),
        I2CM_1A1D_W( 0x90, 0x15 ,0x07),
        I2CM_1A1D_W( 0x90, 0x17 ,0x03),
        I2CM_1A1D_W( 0x90, 0x18 ,0x01),
        I2CM_1A1D_W( 0x90, 0x1B ,0x03),
        I2CM_1A1D_W( 0x90, 0x1C ,0x04),
        I2CM_1A1D_W( 0x90, 0x25 ,0x01),//Cmd_CSI2_Stall=1, Add, 20171027
        I2CM_1A1D_W( 0x90, 0x3F ,0x00),
        I2CM_1A1D_W( 0x90, 0x42 ,0x00),
        I2CM_1A1D_W( 0x90, 0x40 ,0x16), //24M=16 27M=19
        I2CM_1A1D_W( 0x90, 0x41 ,0x12),
        I2CM_1A1D_W( 0x90, 0x47 ,0x05),
        I2CM_1A1D_W( 0x90, 0x54 ,0x0A),
        I2CM_1A1D_W( 0x90, 0x56 ,0x0A),
        I2CM_1A1D_W( 0x90, 0x58 ,0x01),
        I2CM_1A1D_W( 0x90, 0x42 ,0x01),//MIPI PLL , PARALLEL set 0
        I2CM_1A1D_W( 0x90, 0x43 ,0x06),//T_MIPI_sel[0], Bank5_67[1]=1
        I2CM_1A1D_W( 0x90, 0x5B ,0x10),//R_MIPI_FrameReset_by_Vsync_En=1, Modify, 20171027
        I2CM_1A1D_W( 0x90, 0x5C ,0x00),
        I2CM_1A1D_W( 0x90, 0x94 ,0x04),
        I2CM_1A1D_W( 0x90, 0xB1 ,0x00),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x00),

        I2CM_1A1D_W( 0x90, 0x11 ,0x00),//Gate all clock

        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x3B ,0x00),//R_Cmd_Gated_MIPI_Clk=0, Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x02 ,0x73),//Reset MIPI & ISP & TG, Modify, 20171027
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),
        I2CM_1A1D_W( 0x90, 0x0F ,0x01),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        //I2CM_1A1D_W( 0x90, 0xFFFF,  2),//delay 2ms SENSOR_DELAY_REG ,2,//Add, 20171027
        CMDQ_DELAY_MS(2),              //delay 2 ms
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x25 ,0x00),//Cmd_CSI2_Stall=0, Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027

        /*Register update flag*/
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//switch bank 1
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),

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
#endif //end compact setting , 2019/08

#if 1 //begin of compact setting 2019/12/03
static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
        /*set cmdq0 busy*/
        CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
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

        I2CM_1A1D_W( 0x90, 0xEF ,0x00),
        I2CM_1A1D_W( 0x90, 0x10 ,0x80),//Cmd_BYTECLK_InvSel=1
                                       //0x11, 0x00, //Dig Cmd_GatedAllClk latch (default keep 1)
        I2CM_1A1D_W( 0x90, 0x81 ,0x80),//62, 0811 setting, Cmd_tpxoicut=128
        I2CM_1A1D_W( 0x90, 0x9E ,0x08),//0A, 0602 setting
        I2CM_1A1D_W( 0x90, 0xA2 ,0x30),
        I2CM_1A1D_W( 0x90, 0xA3 ,0x03),
        I2CM_1A1D_W( 0x90, 0xBE ,0x05),
        I2CM_1A1D_W( 0x90, 0xD9 ,0x64),
        I2CM_1A1D_W( 0x90, 0xDA ,0x12),
        I2CM_1A1D_W( 0x90, 0xDB ,0x84),
        I2CM_1A1D_W( 0x90, 0xDC ,0x31),//1B, 0602 setting
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x04 ,0x10),
        I2CM_1A1D_W( 0x90, 0x05 ,0x0B), //IO TriState latch Cmd_8_TriState[0] & Cmd_Sw_TriState[0] (keep 1)
        I2CM_1A1D_W( 0x90, 0x0A ,0x04),
        I2CM_1A1D_W( 0x90, 0x0B ,0x64),
        I2CM_1A1D_W( 0x90, 0x0C ,0x01),
        I2CM_1A1D_W( 0x90, 0x0D ,0xFA),
        I2CM_1A1D_W( 0x90, 0x0E ,0x01),
        I2CM_1A1D_W( 0x90, 0x0F ,0x2C),
        I2CM_1A1D_W( 0x90, 0x1D ,0x04),
        I2CM_1A1D_W( 0x90, 0x1E ,0x46),
        I2CM_1A1D_W( 0x90, 0x1F ,0x00),
        I2CM_1A1D_W( 0x90, 0x20 ,0x02),
        I2CM_1A1D_W( 0x90, 0x27 ,0x08),
        I2CM_1A1D_W( 0x90, 0x28 ,0xCC),
        I2CM_1A1D_W( 0x90, 0x29 ,0x02),//0811 setting, Cmd_Treset=2
        I2CM_1A1D_W( 0x90, 0x2E ,0x78),//5A, 0811 setting, Cmd_Tltgon=120
        I2CM_1A1D_W( 0x90, 0x41 ,0x0F),
        I2CM_1A1D_W( 0x90, 0x42 ,0xC8),
        I2CM_1A1D_W( 0x90, 0x4A ,0x21),
        I2CM_1A1D_W( 0x90, 0x4C ,0xBE),
        I2CM_1A1D_W( 0x90, 0x52 ,0xE8),
        I2CM_1A1D_W( 0x90, 0x56 ,0x0A),
        I2CM_1A1D_W( 0x90, 0x60 ,0x78),//5A, 0811 setting, Cmd_Tlnepls=120
        I2CM_1A1D_W( 0x90, 0x7C ,0x38),//0E, 0602 setting
        I2CM_1A1D_W( 0x90, 0x8B ,0x46),
        I2CM_1A1D_W( 0x90, 0xA3 ,0x00),
        I2CM_1A1D_W( 0x90, 0xA4 ,0x0D),
        I2CM_1A1D_W( 0x90, 0xA5 ,0x04),
        I2CM_1A1D_W( 0x90, 0xA6 ,0x40),
        I2CM_1A1D_W( 0x90, 0xA7 ,0x00),
        I2CM_1A1D_W( 0x90, 0xA8 ,0x00),
        I2CM_1A1D_W( 0x90, 0xA9 ,0x07),
        I2CM_1A1D_W( 0x90, 0xAA ,0x88),
        I2CM_1A1D_W( 0x90, 0xAB ,0x01),
        I2CM_1A1D_W( 0x90, 0xD1 ,0x04), //44, 0602 setting
        I2CM_1A1D_W( 0x90, 0xD3 ,0x17), //14, 0602 setting
        I2CM_1A1D_W( 0x90, 0xD8 ,0x5E),
        I2CM_1A1D_W( 0x90, 0xDD ,0x52),//2019/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xE2 ,0x6D),//1D, 0811 setting, T_pos_pump2_lvl=6
        I2CM_1A1D_W( 0x90, 0xE3 ,0x21),
        I2CM_1A1D_W( 0x90, 0xEA ,0x7A),
        I2CM_1A1D_W( 0x90, 0xF5 ,0x01), //pll off
        I2CM_1A1D_W( 0x90, 0xF6 ,0xC8),
        I2CM_1A1D_W( 0x90, 0xF2 ,0x11),
        I2CM_1A1D_W( 0x90, 0xF1 ,0x16),
                                       //{0xF5 ,0x11), //pll enable
        I2CM_1A1D_W( 0x90, 0xF5 ,0x10),//2019/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xF8 ,0x00),//40, 0811 setting, T_pos_pump2_lvl=6
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),
#if defined(SENSOR_INIT_CMDQ_MODE)
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),         //bank 1
        /*Sensor runtime parameter*/
        SNR_SHUTTER_FPS_1A1D( 0x90, 10000, 30000),   //shutter 10ms , fps 30
        SNR_GAIN_1A1D( 0x90, 1024),                  //gain = 1x
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),         //trigger update
#endif
        I2CM_1A1D_W( 0x90, 0xEF ,0x02),
#if 0   //low sensitivity OBC
        I2CM_1A1D_W( 0x90, 0x21 ,0x18),
        I2CM_1A1D_W( 0x90, 0x22 ,0x0C),
        I2CM_1A1D_W( 0x90, 0x23 ,0x08),
        I2CM_1A1D_W( 0x90, 0x24 ,0x05),
        I2CM_1A1D_W( 0x90, 0x25 ,0x03),
        I2CM_1A1D_W( 0x90, 0x26 ,0x02),
        I2CM_1A1D_W( 0x90, 0x33 ,0x85),
#else //high sensitivity OBC
        I2CM_1A1D_W( 0x90, 0x21 ,0x0C),   //d1, reg 0x21, 12X
        I2CM_1A1D_W( 0x90, 0x22 ,0x08),   //d2, reg 0x22, 8X
        I2CM_1A1D_W( 0x90, 0x23 ,0x04),   //d3, reg 0x23, 4X
        I2CM_1A1D_W( 0x90, 0x24 ,0x06),   //d4, reg 0x24, UB=06,12x~32x
        I2CM_1A1D_W( 0x90, 0x25 ,0x05),   //d5, reg 0x25, UB=05,8x~12x
        I2CM_1A1D_W( 0x90, 0x26 ,0x04),   //d6, reg 0x26, UB=04,4x~8x
        I2CM_1A1D_W( 0x90, 0x33 ,0x81),//reg 0x33, UB=03,1x~4x
#endif
        I2CM_1A1D_W( 0x90, 0x33 ,0x85),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),
        I2CM_1A1D_W( 0x90, 0x5B ,0x10),
        I2CM_1A1D_W( 0x90, 0x06 ,0x04),
        I2CM_1A1D_W( 0x90, 0x09 ,0x09),
        I2CM_1A1D_W( 0x90, 0x0A ,0x05),
        I2CM_1A1D_W( 0x90, 0x0C ,0x04),
        I2CM_1A1D_W( 0x90, 0x0D ,0xA1),
        I2CM_1A1D_W( 0x90, 0x0E ,0x00),
        I2CM_1A1D_W( 0x90, 0x0F ,0x00),//MIPI CSI disable
        I2CM_1A1D_W( 0x90, 0x10 ,0x02),
        I2CM_1A1D_W( 0x90, 0x11 ,0x01),
        I2CM_1A1D_W( 0x90, 0x15 ,0x07),
        I2CM_1A1D_W( 0x90, 0x17 ,0x03),
        I2CM_1A1D_W( 0x90, 0x18 ,0x01),
        I2CM_1A1D_W( 0x90, 0x1B ,0x03),
        I2CM_1A1D_W( 0x90, 0x1C ,0x04),
        I2CM_1A1D_W( 0x90, 0x25 ,0x01),
        I2CM_1A1D_W( 0x90, 0x38 ,0x0E),
        I2CM_1A1D_W( 0x90, 0x3B ,0x00),
        I2CM_1A1D_W( 0x90, 0x3F ,0x00),
        I2CM_1A1D_W( 0x90, 0x42 ,0x00),//MIPI PLL disable
        I2CM_1A1D_W( 0x90, 0x40 ,0x16),
        I2CM_1A1D_W( 0x90, 0x41 ,0x12),
        I2CM_1A1D_W( 0x90, 0x43 ,0x04),//T_MIPI_sel[0], Bank5_67[1]=1
        I2CM_1A1D_W( 0x90, 0x47 ,0x05),
        I2CM_1A1D_W( 0x90, 0x54 ,0x0A),
        I2CM_1A1D_W( 0x90, 0x56 ,0x0A),
        I2CM_1A1D_W( 0x90, 0x58 ,0x01),
        I2CM_1A1D_W( 0x90, 0x42 ,0x01),
        I2CM_1A1D_W( 0x90, 0x43 ,0x06),//T_MIPI_sel[0], Bank5_67[1]=1
        I2CM_1A1D_W( 0x90, 0x5C ,0x00),
        I2CM_1A1D_W( 0x90, 0x94 ,0x04),
        I2CM_1A1D_W( 0x90, 0xB1 ,0x00),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x00),
        I2CM_1A1D_W( 0x90, 0x11 ,0x00), //Dig Cmd_GatedAllClk active
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x02 ,0xFB), //RstN TG
        I2CM_1A1D_W( 0x90, 0x05 ,0x3B),//Vsync delay=3
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),
        I2CM_1A1D_W( 0x90, 0x0F ,0x01),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        CMDQ_DELAY_MS(2),              //delay 2 ms//delay 2ms SENSOR_DELAY_REG ,2,//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x25 ,0x00),//Cmd_CSI2_Stall=0, Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027

        /*Register update flag*/
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//switch bank 1
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),


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
#endif //end of compact setting 2019/12/03


#else //compact == 0
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

        /*sensor initial table*/
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x05 ,0x03),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x0F ,0x00),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x42 ,0x00),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x43 ,0x06),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xF5 ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x00), //bank0
        I2CM_1A1D_W( 0x90, 0x10 ,0x80), //Cmd_BYTECLK_InvSel=1
        //I2CM_1A1D_W( 0x90, 0x54 ,0xC3),
        //I2CM_1A1D_W( 0x90, 0x55 ,0x21),
        //I2CM_1A1D_W( 0x90, 0x56 ,0x1D),
        //I2CM_1A1D_W( 0x90, 0x57 ,0x2D),
        //I2CM_1A1D_W( 0x90, 0x58 ,0x3D),
        I2CM_1A1D_W_BURST5( 0x90, 0x54 ,
                            0xC3,   //d0
                            0x21,   //d1
                            0x1D,   //d2
                            0x2D,   //d3
                            0x3D    //d4
                            ),
        I2CM_1A1D_W( 0x90, 0x5F ,0x90),
        I2CM_1A1D_W( 0x90, 0x60 ,0x9A),
        I2CM_1A1D_W( 0x90, 0x61 ,0xFF),
        I2CM_1A1D_W( 0x90, 0x63 ,0x46),
        I2CM_1A1D_W( 0x90, 0x69 ,0x05),
        I2CM_1A1D_W( 0x90, 0x6A ,0x36),
        //I2CM_1A1D_W( 0x90, 0x73 ,0x96),
        //I2CM_1A1D_W( 0x90, 0x74 ,0x96),
        //I2CM_1A1D_W( 0x90, 0x75 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x76 ,0xFD),
        //I2CM_1A1D_W( 0x90, 0x77 ,0xFD),
        //I2CM_1A1D_W( 0x90, 0x78 ,0x77),
        I2CM_1A1D_W_BURST6( 0x90, 0x73 ,
                            0x96,   //d0
                            0x96,   //d1
                            0x00,   //d2
                            0xFD,   //d3
                            0xFD,   //d4
                            0x77    //d5
                            ),
        //I2CM_1A1D_W( 0x90, 0x7A ,0x00),
        //I2CM_1A1D_W( 0x90, 0x7B ,0x03),
        //I2CM_1A1D_W( 0x90, 0x7C ,0x96),
        //I2CM_1A1D_W( 0x90, 0x7D ,0xF8),
        I2CM_1A1D_W_BURST4( 0x90, 0x7A ,
                            0x00,   //d0
                            0x03,   //d1
                            0x96,   //d2
                            0xF8    //d3
                            ),
        //I2CM_1A1D_W( 0x90, 0x7F ,0xA0),
        //I2CM_1A1D_W( 0x90, 0x80 ,0x0E),
        //I2CM_1A1D_W( 0x90, 0x81 ,0x80),//default=62 20170911 JAMES
        //I2CM_1A1D_W( 0x90, 0x82 ,0x71),
        //I2CM_1A1D_W( 0x90, 0x83 ,0x01),
        I2CM_1A1D_W_BURST5( 0x90, 0x7F ,
                            0xA0,   //d0
                            0x0E,   //d1
                            0x80,   //d2
                            0x71,   //d3
                            0x01    //d4
                            ),
        I2CM_1A1D_W( 0x90, 0x85 ,0x96),
        I2CM_1A1D_W( 0x90, 0x87 ,0xC8),
        I2CM_1A1D_W( 0x90, 0x9E ,0x08),
        I2CM_1A1D_W( 0x90, 0xA2 ,0x30),
        I2CM_1A1D_W( 0x90, 0xA3 ,0x03),
        //I2CM_1A1D_W( 0x90, 0xD9 ,0x64),
        //I2CM_1A1D_W( 0x90, 0xDA ,0x12),
        //I2CM_1A1D_W( 0x90, 0xDB ,0x84),
        //I2CM_1A1D_W( 0x90, 0xDC ,0x31),
        I2CM_1A1D_W_BURST4( 0x90, 0xD9 ,
                            0x64,   //d0
                            0x12,   //d1
                            0x84,   //d2
                            0x31    //d3
                            ),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x01), //bank1
        I2CM_1A1D_W( 0x90, 0x04 ,0x10),
        I2CM_1A1D_W( 0x90, 0x05, 0x0B),
        I2CM_1A1D_W( 0x90, 0x09 ,0x00),
        I2CM_1A1D_W( 0x90, 0x0A ,0x04),
        I2CM_1A1D_W( 0x90, 0x0B ,0x64),
        I2CM_1A1D_W( 0x90, 0x0D ,0x02),
        I2CM_1A1D_W( 0x90, 0x1E ,0x46), //default=43 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x20 ,0x02),
        //I2CM_1A1D_W( 0x90, 0x27 ,0x08),
        //I2CM_1A1D_W( 0x90, 0x28 ,0xCC),
        //I2CM_1A1D_W( 0x90, 0x29 ,0x02), //default=00 20170608 JAMES
        //I2CM_1A1D_W( 0x90, 0x2A ,0x08),
        //I2CM_1A1D_W( 0x90, 0x2B ,0x93), //default=C8 20170608 JAMES
        //I2CM_1A1D_W( 0x90, 0x2C ,0x10),
        //I2CM_1A1D_W( 0x90, 0x2D ,0x12),
        I2CM_1A1D_W_BURST7( 0x90, 0x27 ,
                            0x08,   //d0
                            0xcc,   //d1
                            0x02,   //d2
                            0x08,   //d3
                            0x93,   //d4
                            0x10,   //d5
                            0x12    //d6
                            ),
        I2CM_1A1D_W( 0x90, 0x2E ,0x78), //default=5A 20170911 JAMES
        //I2CM_1A1D_W( 0x90, 0x37 ,0x90),
        //I2CM_1A1D_W( 0x90, 0x38 ,0x44),
        //I2CM_1A1D_W( 0x90, 0x39 ,0xC2),
        //I2CM_1A1D_W( 0x90, 0x3A ,0xFF),
        //I2CM_1A1D_W( 0x90, 0x3B ,0x0A),
        I2CM_1A1D_W_BURST5( 0x90, 0x37 ,
                            0x90,   //d0
                            0x44,   //d1
                            0xC2,   //d2
                            0xFF,   //d3
                            0x0A    //d4
                            ),
        //I2CM_1A1D_W( 0x90, 0x3E ,0x20),
        //I2CM_1A1D_W( 0x90, 0x3F ,0x22),
        //I2CM_1A1D_W( 0x90, 0x40 ,0xFF),
        //I2CM_1A1D_W( 0x90, 0x41 ,0x0F), //default=1F 20170608 JAMES
        I2CM_1A1D_W_BURST4( 0x90, 0x3E ,
                            0x20,   //d0
                            0x22,   //d1
                            0xFF,   //d2
                            0x0F     //d3
                            ),
        I2CM_1A1D_W( 0x90, 0x42 ,0xC8), //default=68 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x4A ,0x21), //default=22 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x4B ,0x1E),
        I2CM_1A1D_W( 0x90, 0x4C ,0xBE), //default=5F 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x4F ,0x01),
        I2CM_1A1D_W( 0x90, 0x51 ,0x03),
        I2CM_1A1D_W( 0x90, 0x52 ,0xE8), //default=B8 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x56 ,0x0A), //default=64 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0x60 ,0x78), //default=5A 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0x6F ,0x06),
        I2CM_1A1D_W( 0x90, 0x7C ,0x38), //default=0E 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0x8B ,0x46),
        I2CM_1A1D_W( 0x90, 0x99 ,0x02),
        I2CM_1A1D_W( 0x90, 0xA6 ,0x40),
        I2CM_1A1D_W( 0x90, 0xA8 ,0x00),
        I2CM_1A1D_W( 0x90, 0xAA ,0x88),
        I2CM_1A1D_W( 0x90, 0xB0 ,0x50),
        I2CM_1A1D_W( 0x90, 0xB4 ,0x50),
        I2CM_1A1D_W( 0x90, 0xC6 ,0x00),
        I2CM_1A1D_W( 0x90, 0xC7 ,0x00),
        I2CM_1A1D_W( 0x90, 0xCA ,0x00),
        I2CM_1A1D_W( 0x90, 0xCB ,0x00),
        I2CM_1A1D_W( 0x90, 0xD0 ,0xC2),
        I2CM_1A1D_W( 0x90, 0xD1 ,0x04), //default=44 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0xD3 ,0x17), //default=14 20170711 JAMES
        I2CM_1A1D_W( 0x90, 0xD8 ,0x5E), //default=4E 20170608 JAMES
        I2CM_1A1D_W( 0x90, 0xD9 ,0x66), //default=NC 20170608 JAMES
        //I2CM_1A1D_W( 0x90, 0xDD ,0x72), //default=62 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xDD ,0x52), //2018/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xDE ,0x43),
        I2CM_1A1D_W( 0x90, 0xE2 ,0x6D), //default=1D 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xE3 ,0x21),
        I2CM_1A1D_W( 0x90, 0xEA ,0x7A),
        I2CM_1A1D_W( 0x90, 0xF5 ,0x01),
        I2CM_1A1D_W( 0x90, 0xF6 ,0xC8),
        I2CM_1A1D_W( 0x90, 0xF7 ,0x02),
        I2CM_1A1D_W( 0x90, 0xF0 ,0x03),
        I2CM_1A1D_W( 0x90, 0xF4 ,0x06),
        I2CM_1A1D_W( 0x90, 0xF2 ,0x11),
        //I2CM_1A1D_W( 0x90, 0xF1 ,0x0A),
        I2CM_1A1D_W( 0x90, 0xF1 ,0x16), //2018/6/12 reduce light flare in corner
        //I2CM_1A1D_W( 0x90, 0xF5 ,0x11),
        I2CM_1A1D_W( 0x90, 0xF5 ,0x10), //2018/6/12 reduce light flare in corner
        I2CM_1A1D_W( 0x90, 0xF8 ,0x00),//default=40 20170911 JAMES
        I2CM_1A1D_W( 0x90, 0xF9 ,0x15),
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),

        I2CM_1A1D_W( 0x90, 0xEF ,0x02),//bank2
        //I2CM_1A1D_W( 0x90, 0x20 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x21 ,0x18),
        //I2CM_1A1D_W( 0x90, 0x22 ,0x0C),
        //I2CM_1A1D_W( 0x90, 0x23 ,0x08),
        //I2CM_1A1D_W( 0x90, 0x24 ,0x05),
        //I2CM_1A1D_W( 0x90, 0x25 ,0x03),
        //I2CM_1A1D_W( 0x90, 0x26 ,0x02),
#if 0   //low sensitivity OBC
        I2CM_1A1D_W_BURST7( 0x90, 0x20 ,
                            0x01,   //d0
                            0x18,   //d1
                            0x0C,   //d2
                            0x08,   //d3
                            0x05,   //d4
                            0x03,   //d5
                            0x02    //d6
                            ),  //n data
        I2CM_1A1D_W( 0x90, 0x27 ,0x02),
        I2CM_1A1D_W( 0x90, 0x28 ,0x07),
        I2CM_1A1D_W( 0x90, 0x29 ,0x08),
        I2CM_1A1D_W( 0x90, 0x33 ,0x85),
#else //high sensitivity OBC
        I2CM_1A1D_W_BURST7( 0x90, 0x20 ,
                            0x01,   //d0, reg 0x20, chg ABC Mode
                            0x0C,   //d1, reg 0x21, 12X
                            0x08,   //d2, reg 0x22, 8X
                            0x04,   //d3, reg 0x23, 4X
                            0x06,   //d4, reg 0x24, UB=06,12x~32x
                            0x05,   //d5, reg 0x25, UB=05,8x~12x
                            0x04    //d6, reg 0x26, UB=04,4x~8x
                            ),  //n data
        I2CM_1A1D_W( 0x90, 0x27 ,0x02),
        I2CM_1A1D_W( 0x90, 0x28 ,0x07),
        I2CM_1A1D_W( 0x90, 0x29 ,0x08),
        I2CM_1A1D_W( 0x90, 0x33 ,0x81),//reg 0x33, UB=03,1x~4x
#endif
        I2CM_1A1D_W( 0x90, 0x47 ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x49 ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x4B ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0x4D ,0x10), //black level
        I2CM_1A1D_W( 0x90, 0xCB ,0xBD),
        I2CM_1A1D_W( 0x90, 0xE3 ,0x1B),
        I2CM_1A1D_W( 0x90, 0xFD ,0x18),
        I2CM_1A1D_W( 0x90, 0xFE ,0x9E),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05), //bank5
        //I2CM_1A1D_W( 0x90, 0x03 ,0x10),
        //I2CM_1A1D_W( 0x90, 0x04 ,0xE0),
        //I2CM_1A1D_W( 0x90, 0x05 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x06 ,0x04),
        //I2CM_1A1D_W( 0x90, 0x07 ,0x80),
        //I2CM_1A1D_W( 0x90, 0x08 ,0x02),
        //I2CM_1A1D_W( 0x90, 0x09 ,0x09),
        I2CM_1A1D_W_BURST7( 0x90, 0x03 ,
                            0x10,   //d0
                            0xE0,   //d1
                            0x01,   //d2
                            0x04,   //d3
                            0x80,   //d4
                            0x02,   //d5
                            0x09    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x0A ,0x05),
        //I2CM_1A1D_W( 0x90, 0x0B ,0x06),
        //I2CM_1A1D_W( 0x90, 0x0C ,0x04),
        //I2CM_1A1D_W( 0x90, 0x0D ,0xA1),
        //I2CM_1A1D_W( 0x90, 0x0E ,0x00),
        //I2CM_1A1D_W( 0x90, 0x0F ,0x00),
        //I2CM_1A1D_W( 0x90, 0x10 ,0x02),
        I2CM_1A1D_W_BURST7( 0x90, 0x0A ,
                            0x05,   //d0
                            0x06,   //d1
                            0x04,   //d2
                            0xA1,   //d3
                            0x00,   //d4
                            0x00,   //d5
                            0x02    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x11 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x12 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x13 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x14 ,0xB8),
        //I2CM_1A1D_W( 0x90, 0x15 ,0x07),
        //I2CM_1A1D_W( 0x90, 0x16 ,0x06),
        //I2CM_1A1D_W( 0x90, 0x17 ,0x03),
        I2CM_1A1D_W_BURST7( 0x90, 0x11 ,
                            0x01,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0xB8,   //d3
                            0x07,   //d4
                            0x06,   //d5
                            0x03    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x18 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x19 ,0x04),
        //I2CM_1A1D_W( 0x90, 0x1A ,0x06),
        //I2CM_1A1D_W( 0x90, 0x1B ,0x03),
        //I2CM_1A1D_W( 0x90, 0x1C ,0x04),
        //I2CM_1A1D_W( 0x90, 0x1D ,0x08),
        //I2CM_1A1D_W( 0x90, 0x1E ,0x1A),
        I2CM_1A1D_W_BURST7( 0x90, 0x18 ,
                            0x01,   //d0
                            0x04,   //d1
                            0x06,   //d2
                            0x03,   //d3
                            0x04,   //d4
                            0x08,   //d5
                            0x1A    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x1F ,0x00),
        //I2CM_1A1D_W( 0x90, 0x20 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x21 ,0x1E),
        //I2CM_1A1D_W( 0x90, 0x22 ,0x1E),
        //I2CM_1A1D_W( 0x90, 0x23 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x24 ,0x04),
        //I2CM_1A1D_W( 0x90, 0x25 ,0x01),//Cmd_CSI2_Stall=1, Add, 20171027
        I2CM_1A1D_W_BURST7( 0x90, 0x1F ,
                            0x00,   //d0
                            0x00,   //d1
                            0x1E,   //d2
                            0x1E,   //d3
                            0x01,   //d4
                            0x04,   //d5
                            0x01    //d6
                            ),
        I2CM_1A1D_W( 0x90, 0x27 ,0x00),
        I2CM_1A1D_W( 0x90, 0x28 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x2A ,0x08),
        //I2CM_1A1D_W( 0x90, 0x2B ,0x02),
        //I2CM_1A1D_W( 0x90, 0x2C ,0xA4),
        //I2CM_1A1D_W( 0x90, 0x2D ,0x06),
        //I2CM_1A1D_W( 0x90, 0x2E ,0x00),
        //I2CM_1A1D_W( 0x90, 0x2F ,0x05),
        //I2CM_1A1D_W( 0x90, 0x30 ,0xE0),
        I2CM_1A1D_W_BURST7( 0x90, 0x2A ,
                            0x08,   //d0
                            0x02,   //d1
                            0xA4,   //d2
                            0x06,   //d3
                            0x00,   //d4
                            0x05,   //d5
                            0xE0    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x31 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x32 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x33 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x34 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x35 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x36 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x37 ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0x31 ,
                            0x01,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x00,   //d3
                            0x00,   //d4
                            0x00,   //d5
                            0x00    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x38 ,0x0E),
        //I2CM_1A1D_W( 0x90, 0x39 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x3A ,0x02),
        //I2CM_1A1D_W( 0x90, 0x3B ,0x01),//R_Cmd_Gated_MIPI_Clk=1, Modify, 20171027
        //I2CM_1A1D_W( 0x90, 0x3C ,0x00),
        //I2CM_1A1D_W( 0x90, 0x3D ,0x00),
        //I2CM_1A1D_W( 0x90, 0x3E ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0x38 ,
                            0x0E,   //d0
                            0x01,   //d1
                            0x02,   //d2
                            0x01,   //d3
                            0x00,   //d4
                            0x00,   //d5
                            0x00    //d6
                            ),
        I2CM_1A1D_W( 0x90, 0x3F ,0x00),
        I2CM_1A1D_W( 0x90, 0x42 ,0x00),
        I2CM_1A1D_W( 0x90, 0x40 ,0x16), //24M=16 27M=19
        I2CM_1A1D_W( 0x90, 0x41 ,0x12),
        I2CM_1A1D_W( 0x90, 0x47 ,0x05),
        I2CM_1A1D_W( 0x90, 0x48 ,0x00),
        I2CM_1A1D_W( 0x90, 0x49 ,0x01),
        I2CM_1A1D_W( 0x90, 0x4D ,0x02),
        I2CM_1A1D_W( 0x90, 0x4F ,0x00),
        //I2CM_1A1D_W( 0x90, 0x54 ,0x0A),
        //I2CM_1A1D_W( 0x90, 0x55 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x56 ,0x0A),
        //I2CM_1A1D_W( 0x90, 0x57 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x58 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x59 ,0x01),
        I2CM_1A1D_W_BURST6( 0x90, 0x54 ,
                            0x0A,   //d0
                            0x01,   //d1
                            0x0A,   //d2
                            0x01,   //d3
                            0x01,   //d4
                            0x01    //d5
                            ),
        I2CM_1A1D_W( 0x90, 0x42 ,0x01),//MIPI PLL , PARALLEL set 0
        I2CM_1A1D_W( 0x90, 0x43 ,0x06),//T_MIPI_sel[0], Bank5_67[1]=1
        //I2CM_1A1D_W( 0x90, 0x5B ,0x10),//R_MIPI_FrameReset_by_Vsync_En=1, Modify, 20171027
        //I2CM_1A1D_W( 0x90, 0x5C ,0x00),
        //I2CM_1A1D_W( 0x90, 0x5D ,0x00),
        //I2CM_1A1D_W( 0x90, 0x5E ,0x07),
        //I2CM_1A1D_W( 0x90, 0x5F ,0x08),
        //I2CM_1A1D_W( 0x90, 0x60 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x61 ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0x5B ,
                            0x10,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x07,   //d3
                            0x08,   //d4
                            0x00,   //d5
                            0x00    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x62 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x63 ,0x28),
        //I2CM_1A1D_W( 0x90, 0x64 ,0x30),
        //I2CM_1A1D_W( 0x90, 0x65 ,0x9E),
        //I2CM_1A1D_W( 0x90, 0x66 ,0xB9),
        //I2CM_1A1D_W( 0x90, 0x67 ,0x52),
        //I2CM_1A1D_W( 0x90, 0x68 ,0x70),
        I2CM_1A1D_W_BURST7( 0x90, 0x62 ,
                            0x00,   //d0
                            0x28,   //d1
                            0x30,   //d2
                            0x9E,   //d3
                            0xB9,   //d4
                            0x52,   //d5
                            0x70    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0x69 ,0x4E),
        //I2CM_1A1D_W( 0x90, 0x70 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x71 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x72 ,0x00),
        I2CM_1A1D_W_BURST4( 0x90, 0x69 ,
                            0x4E,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x00    //d3
                            ),
        //I2CM_1A1D_W( 0x90, 0x90 ,0x04),
        //I2CM_1A1D_W( 0x90, 0x91 ,0x01),
        //I2CM_1A1D_W( 0x90, 0x92 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x93 ,0x00),
        //I2CM_1A1D_W( 0x90, 0x94 ,0x04),
        I2CM_1A1D_W_BURST5( 0x90, 0x90 ,
                            0x04,   //d0
                            0x01,   //d1
                            0x00,   //d2
                            0x00,   //d3
                            0x04    //d4
                            ),
        I2CM_1A1D_W( 0x90, 0x96 ,0x00),
        I2CM_1A1D_W( 0x90, 0x97 ,0x01),
        I2CM_1A1D_W( 0x90, 0x98 ,0x01),
        //I2CM_1A1D_W( 0x90, 0xA0 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xA1 ,0x01),
        //I2CM_1A1D_W( 0x90, 0xA2 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xA3 ,0x01),
        //I2CM_1A1D_W( 0x90, 0xA4 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xA5 ,0x01),
        //I2CM_1A1D_W( 0x90, 0xA6 ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0xA0 ,
                            0x00,   //d0
                            0x01,   //d1
                            0x00,   //d2
                            0x01,   //d3
                            0x00,   //d4
                            0x01,   //d5
                            0x00    //d6
                            ),
        I2CM_1A1D_W( 0x90, 0xA7 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xAA ,0x00),
        //I2CM_1A1D_W( 0x90, 0xAB ,0x0F),
        //I2CM_1A1D_W( 0x90, 0xAC ,0x08),
        //I2CM_1A1D_W( 0x90, 0xAD ,0x09),
        //I2CM_1A1D_W( 0x90, 0xAE ,0x0A),
        //I2CM_1A1D_W( 0x90, 0xAF ,0x0B),
        //I2CM_1A1D_W( 0x90, 0xB0 ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0xAA ,
                            0x00,   //d0
                            0x0F,   //d1
                            0x08,   //d2
                            0x09,   //d3
                            0x0A,   //d4
                            0x0B,   //d5
                            0x00    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0xB1 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xB2 ,0x01),
        //I2CM_1A1D_W( 0x90, 0xB3 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xB4 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xB5 ,0x0A),
        //I2CM_1A1D_W( 0x90, 0xB6 ,0x0A),
        //I2CM_1A1D_W( 0x90, 0xB7 ,0x0A),
        I2CM_1A1D_W_BURST7( 0x90, 0xB1 ,
                            0x00,   //d0
                            0x01,   //d1
                            0x00,   //d2
                            0x00,   //d3
                            0x0A,   //d4
                            0x0A,   //d5
                            0x0A    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0xB8 ,0x0A),
        //I2CM_1A1D_W( 0x90, 0xB9 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xBA ,0x00),
        //I2CM_1A1D_W( 0x90, 0xBB ,0x00),
        //I2CM_1A1D_W( 0x90, 0xBC ,0x00),
        //I2CM_1A1D_W( 0x90, 0xBD ,0x00),
        //I2CM_1A1D_W( 0x90, 0xBE ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0xB8 ,
                            0x0A,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x00,   //d3
                            0x00,   //d4
                            0x00,   //d5
                            0x00    //d6
                            ),
        //I2CM_1A1D_W( 0x90, 0xBF ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC0 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC1 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC2 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC3 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC4 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xC5 ,0x00),
        I2CM_1A1D_W_BURST7( 0x90, 0xBF ,
                            0x00,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x00,   //d3
                            0x00,   //d4
                            0x00,   //d5
                            0x00    //d6
                            ),
        I2CM_1A1D_W( 0x90, 0xC6 ,0x00),
        I2CM_1A1D_W( 0x90, 0xC7 ,0x00),
        I2CM_1A1D_W( 0x90, 0xC8 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xD3 ,0x80),
        //I2CM_1A1D_W( 0x90, 0xD4 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xD5 ,0x00),
        //I2CM_1A1D_W( 0x90, 0xD6 ,0x03),
        //I2CM_1A1D_W( 0x90, 0xD7 ,0x77),
        //I2CM_1A1D_W( 0x90, 0xD8 ,0x00),
        I2CM_1A1D_W_BURST6( 0x90, 0xD3 ,
                            0x80,   //d0
                            0x00,   //d1
                            0x00,   //d2
                            0x03,   //d3
                            0x77,   //d4
                            0x00    //d5
                            ),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x00),

        I2CM_1A1D_W( 0x90, 0x11 ,0x00),//Gate all clock

        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x3B ,0x00),//R_Cmd_Gated_MIPI_Clk=0, Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),
        I2CM_1A1D_W( 0x90, 0x02 ,0x73),//Reset MIPI & ISP & TG, Modify, 20171027
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),
        I2CM_1A1D_W( 0x90, 0x0F ,0x01),
        I2CM_1A1D_W( 0x90, 0xED ,0x01),
        //I2CM_1A1D_W( 0x90, 0xFFFF,  2),//delay 2ms SENSOR_DELAY_REG ,2,//Add, 20171027
        CMDQ_DELAY_MS(2),              //delay 2 ms
        I2CM_1A1D_W( 0x90, 0xEF ,0x05),//Add, 20171027
        I2CM_1A1D_W( 0x90, 0x25 ,0x00),//Cmd_CSI2_Stall=0, Add, 20171027
        I2CM_1A1D_W( 0x90, 0xED ,0x01),//Add, 20171027

#if defined(SENSOR_INIT_CMDQ_MODE)
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//bank 1
        /*Sensor runtime parameter*/
        SNR_SHUTTER_FPS( 0x90, 10000, 30000), //shutter 10ms , fps 30
        SNR_GAIN( 0x90, 1024), //gain = 1x
#endif

        /*Register update flag*/
        I2CM_1A1D_W( 0x90, 0xEF ,0x01),//switch bank 1
        I2CM_1A1D_W( 0x90, 0x09 ,0x01),

#if defined(SENSOR_INIT_CMDQ_MODE)
        /*set cmdq0 idle*/
        //CMDQ_REG_DUMMY(CMDQ_STATUS_READY),
        VIF_REG_DUMMY(CMDQ_STATUS_READY),

        /*append dummy*/
        //CMDQ_TAG('EOT'),    //end of table
        CMDQ_NULL(),
        CMDQ_NULL(),
        CMDQ_NULL(),
        CMDQ_NULL(),
#endif
};
#endif//end of compact setting

#endif
