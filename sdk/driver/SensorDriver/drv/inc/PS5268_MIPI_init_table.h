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

#ifndef _PS5268_MIPI_INIT_TABLE_H_

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
    I2CM_2A1D_W(0x90, 0x0110, 0x00), // Cmd_HLGain_Mode=0
    I2CM_2A1D_W(0x90, 0x010B, 0x07), // Cmd_Sw_TriState[0]=1
    I2CM_2A1D_W(0x90, 0x0114, 0x08), // Cmd_LineTime=2200
    I2CM_2A1D_W(0x90, 0x0115, 0x98), // Cmd_LineTime=2200
    I2CM_2A1D_W(0x90, 0x0118, 0x01), // Cmd_LineTime=2200
    I2CM_2A1D_W(0x90, 0x0119, 0xFA), // Cmd_LineTime=2200
    I2CM_2A1D_W(0x90, 0x0162, 0x02), // Cmd_Np[3:0]=2
    I2CM_2A1D_W(0x90, 0x0178, 0xA0), // A02A: Version
    I2CM_2A1D_W(0x90, 0x0179, 0x2A), // A02A: Version
    I2CM_2A1D_W(0x90, 0x022D, 0x01), // T_spll_enh[0]=1
    I2CM_2A1D_W(0x90, 0x0225, 0xEA), // T_pos_pump2_lvl[3:0]=9->14
    I2CM_2A1D_W(0x90, 0x0226, 0xB1), // rsel: vdday -> vddma
    I2CM_2A1D_W(0x90, 0x0227, 0x39), // T_ldotg_lvl[3:0]=7->3
    I2CM_2A1D_W(0x90, 0x021C, 0x00), // T_FAE_CLK_SEL[0]=0
    I2CM_2A1D_W(0x90, 0x0233, 0x70), // T_iref_enl=1, T_vdda_enl=1
    I2CM_2A1D_W(0x90, 0x023C, 0x57), // T_pxoi_fast[2:0]=7
    I2CM_2A1D_W(0x90, 0x024B, 0x05), // T_mipi_bitclk_phase[0]=1
    I2CM_2A1D_W(0x90, 0x024D, 0x11), // T_MIPI_EnL[0]=0, T_MIPI_sel[0]=1
    I2CM_2A1D_W(0x90, 0x0252, 0x19), // T_pll_predivider[5:0]=25
    I2CM_2A1D_W(0x90, 0x0253, 0x26), // T_pll_postdivider[5:0]=38
    I2CM_2A1D_W(0x90, 0x0254, 0x61), // T_pll_enh[0]=1, T_icp_p[2:0]=1->6
    I2CM_2A1D_W(0x90, 0x0255, 0x11), // 11//T_pll_div2_EnH=1
    I2CM_2A1D_W(0x90, 0x0624, 0x05), // R_tg=1293
    I2CM_2A1D_W(0x90, 0x0625, 0x0D), // R_tg=1293
    I2CM_2A1D_W(0x90, 0x0B01, 0x00), // Cmd_Adc_sample_posedge=0 for work-around
    I2CM_2A1D_W(0x90, 0x0B02, 0x02), // Cmd_RClkDly_Sel=7
    I2CM_2A1D_W(0x90, 0x0B0C, 0x00), // Cmd_MIPI_Clk_Gated[0]=0
    I2CM_2A1D_W(0x90, 0x0C00, 0x02), // Cmd_DG_EnH=1
    I2CM_2A1D_W(0x90, 0x0D00, 0x00), // R_ImgSyn_SGHD_EnH=0
    I2CM_2A1D_W(0x90, 0x1409, 0x1A), // R_tx_data_settle_prd[7:0]=26
    I2CM_2A1D_W(0x90, 0x140A, 0x15), // R_tx_data_sp_blank_prd[7:0]=21
    I2CM_2A1D_W(0x90, 0x140C, 0x13), // FF//R_phyclk_lps_prd[7:0]=19 for discontinue clock, defualt value is wrong
    I2CM_2A1D_W(0x90, 0x1411, 0x00), // 01//R_phyclk_cont_mode[0]=1
    I2CM_2A1D_W(0x90, 0x1415, 0x04), // R_LPX_prd[7:0]=7->4
    I2CM_2A1D_W(0x90, 0x1417, 0x03), // R_HsPrep_prd[7:0]=6->3
    I2CM_2A1D_W(0x90, 0x1418, 0x02), // R_HsEoT_prd[7:0]=5->2
    I2CM_2A1D_W(0x90, 0x1406, 0x04), // RAW10
    I2CM_2A1D_W(0x90, 0x1410, 0x02), // 2Lane
    I2CM_2A1D_W(0x90, 0x140F, 0x01), // R_CSI2_enable=1
    I2CM_2A1D_W(0x90, 0x0111, 0x01), // UpdateFlag
    I2CM_2A1D_W(0x90, 0x010F, 0x01),  // Sensor_EnH[0]=1

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
