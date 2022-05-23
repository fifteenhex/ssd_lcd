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

#ifndef _PS5258_MIPI_INIT_TABLE_H_

static SENSOR_INIT_TABLE Sensor_init_table[] __attribute__((aligned(8)))=
{
#if defined(SENSOR_INIT_CMDQ_MODE)
    /*set cmdq0 busy*/
    //CMDQ_REG_DUMMY(CMDQ_STATUS_BUSY),
    VIF_REG_DUMMY(CMDQ_STATUS_BUSY),
    //SNR_RST(1),                    //reset high
    /*Sensor MCLK*/
    SNR_MCLK_EN(0x0),              //MCLK 27MhZ
    CMDQ_DELAY_10US(1),

    /*I2C timing*/
    I2CM_CLK_EN(0x2),  //12MHz
    I2CM_PARAM(16,11,50,50,0,11),
#endif
    /*sensor initial table*/
    I2CM_2A1D_W(0x90, 0x0114, 0x09), 
    I2CM_2A1D_W(0x90, 0x0115, 0x60), 
    I2CM_2A1D_W(0x90, 0x0178, 0xA0), 
    I2CM_2A1D_W(0x90, 0x0179, 0x1A), 
    I2CM_2A1D_W(0x90, 0x022E, 0x0E), 
    I2CM_2A1D_W(0x90, 0x022F, 0x19), 
    I2CM_2A1D_W(0x90, 0x022D, 0x01), 
    I2CM_2A1D_W(0x90, 0x021C, 0x00), 
    I2CM_2A1D_W(0x90, 0x0252, 0x16), 
    I2CM_2A1D_W(0x90, 0x0254, 0x61), 
    I2CM_2A1D_W(0x90, 0x0B0C, 0x00),
    I2CM_2A1D_W(0x90, 0x140C, 0x19),
    I2CM_2A1D_W(0x90, 0x1411, 0x00),
    I2CM_2A1D_W(0x90, 0x140F, 0x01), 
    I2CM_2A1D_W(0x90, 0x0111, 0x01), 
    I2CM_2A1D_W(0x90, 0x010F, 0x01), 

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
