/*
* hal_disp_chip.h- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/

#ifndef _HAL_DISP_CHIP_H_
#define _HAL_DISP_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctx
#define HAL_DISP_CTX_MAX_INST   1

//------------------------------------------------------------------------------
// Device Ctx
#define HAL_DISP_DEVICE_ID_0    0
#define HAL_DISP_DEVICE_ID_1    1 //unused
#define HAL_DISP_DEVICE_MAX     1
//------------------------------------------------------------------------------
// VideoLayer
#define HAL_DISP_VIDLAYER_MAX   1 // MOPG & MOPS, only MOPS here

#define HAL_DISP_MOPG_GWIN_NUM  0 //MOPG 16 Gwins
#define HAL_DISP_MOPS_GWIN_NUM  1  //MOPS 1 Gwin
//------------------------------------------------------------------------------
// InputPort
#define HAL_DISP_INPUTPORT_NUM  (HAL_DISP_MOPG_GWIN_NUM+HAL_DISP_MOPS_GWIN_NUM) // MOP: 16Gwin_MOPG + 1Gwin_MOPS
#define HAL_DISP_INPUTPORT_MAX  (1  * HAL_DISP_INPUTPORT_NUM)
//------------------------------------------------------------------------------
// DMA  Ctx
#define HAL_DISP_DMA_ID_0                   0
#define HAL_DISP_DMA_MAX                    1
#define HAL_DISP_DMA_PIX_FMT_PLANE_MAX      3
//------------------------------------------------------------------------------
// IRQ CTX
#define HAL_DISP_IRQ_ID_DEVICE_0        0 //lcd int
#define HAL_DISP_IRQ_ID_TIMEZONE_0      1 //sc int
#define HAL_DISP_IRQ_ID_MAX             2
//------------------------------------------------------------------------------
// Use IMI
#define HAL_DISP_IMI_USE   0


#define HAL_DISP_IRQ_CFG \
{ \
    {0, 0}, \
    {0, 0}, \
}
//------------------------------------------------------------------------------
// TimeZone
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_LINUX     1
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_UBOOT     0


#define E_HAL_DISP_IRQ_TYPE_TIMEZONE            (E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)

//------------------------------------------------------------------------------
// Vga HPD Isr
#define HAL_DISP_VGA_HPD_ISR_SUPPORT            0
#define HAL_DISP_DEVICE_IRQ_VGA_HPD_ISR_IDX     4
#define E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF      (E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON | E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
//------------------------------------------------------------------------------
// CLK
#define CLK_MHZ(x)                  (x*1000000)
#define HAL_DISP_CLK_MOP_RATE       CLK_MHZ(320)
#define HAL_DISP_CLK_HDMI_RATE      0
#define HAL_DISP_CLK_DAC_RATE       0
#define HAL_DISP_CLK_DISP_432_RATE  CLK_MHZ(432)
#define HAL_DISP_CLK_DISP_216_RATE  CLK_MHZ(216)
#define HAL_DISP_CLK_SC_PIXEL_RATE  6


#define HAL_DISP_CLK_NUM            4

#define HAL_DISP_CLK_ON_SETTING \
{ \
    1, 1, 1, 1, \
}

#define HAL_DISP_CLK_OFF_SETTING \
{ \
    0, 0, 0, 0,\
}


#define HAL_DISP_CLK_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
    HAL_DISP_CLK_SC_PIXEL_RATE, \
}

#define HAL_DISP_CLK_OFF_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
    HAL_DISP_CLK_SC_PIXEL_RATE, \
}


#define HAL_DISP_CLK_MUX_ATTR \
{ \
    0, 0, 0, 1, \
}

#define HAL_DISP_CLK_NAME \
{   \
    "mop", \
    "disp432", \
    "disp216", \
    "sc_pixel", \
}


//------------------------------------------------------------------------------
// reg_ckg_odclk0 / reg_ckg_odclk1
#define HAL_DISP_ODCLK_SEL_HDMI                 0x00
#define HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK        0x01

// reg_clk_lcd
#define HAL_DISP_CLK_LCD_SEL_ODCLK0             0x00

// reg_clk_lcd_src
#define HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0         0x00
#define HAL_DISP_CLK_LCD_SRC_SEL_ODCLK1         0x01

// reg_disp_to_miu_mux
#define HAL_DISP_DMA_MIU_MUX_FRONT_DISP0        0x0000
#define HAL_DISP_DMA_MIU_MUX_BACK_DISP0         0x0001
#define HAL_DISP_DMA_MIU_MUX_FRONT_DISP1        0x0002
#define HAL_DISP_DMA_MIU_MUX_BACK_DISP1         0x0003

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif

