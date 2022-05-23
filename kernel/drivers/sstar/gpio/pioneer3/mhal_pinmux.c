/*
* mhal_pinmux.c- Sigmastar
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
#include <linux/mm.h>
#include "ms_platform.h"
#include "mdrv_types.h"
#include "mhal_gpio.h"
#include "padmux.h"
#include "gpio.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define BASE_RIU_PA                         0xFD000000
#define PMSLEEP_BANK                        0x000E00
#define PM_SAR_BANK                         0x001400
#define ALBANY1_BANK                        0x003200
#define ALBANY2_BANK                        0x003300
#define CHIPTOP_BANK                        0x101E00
#define PADTOP_BANK                         0x103C00
#define PM_PADTOP_BANK                      0x003F00
#define UTMI0_BANK                          0x142100
#define ETH_BANK                            0x003300
#define ARBPHY_BANK                         0x003200


#define _GPIO_W_WORD(addr,val)              {(*(volatile u16*)(addr)) = (u16)(val);}
#define _GPIO_W_WORD_MASK(addr,val,mask)    {(*(volatile u16*)(addr)) = ((*(volatile u16*)(addr)) & ~(mask)) | ((u16)(val) & (mask));}
#define _GPIO_R_BYTE(addr)                  (*(volatile u8*)(addr))
#define _GPIO_R_WORD_MASK(addr,mask)        ((*(volatile u16*)(addr)) & (mask))

#define GET_BASE_ADDR_BY_BANK(x, y)         ((x) + ((y) << 1))
#define _RIUA_8BIT(bank , offset)           GET_BASE_ADDR_BY_BANK(BASE_RIU_PA, bank) + (((offset) & ~1)<<1) + ((offset) & 1)
#define _RIUA_16BIT(bank , offset)          GET_BASE_ADDR_BY_BANK(BASE_RIU_PA, bank) + ((offset)<<2)

/* SAR : SAR_BANK, R/W 8-bits */
#define REG_SAR_AISEL_8BIT          0x11
    #define REG_SAR_CH0_AISEL       BIT0
    #define REG_SAR_CH1_AISEL       BIT1
    #define REG_SAR_CH2_AISEL       BIT2
    #define REG_SAR_CH3_AISEL       BIT3
/* EMAC : ALBANY1_BANK */
#define REG_ATOP_RX_INOFF       0x69
    #define REG_ATOP_RX_INOFF_MASK  BIT15|BIT14

/* EMAC : ALBANY2_BANK */
#define REG_ETH_GPIO_EN         0x71
    #define REG_ETH_GPIO_EN_MASK    BIT3|BIT2|BIT1|BIT0
//#define REG_AUTO_GPIO_EN                        0x9
//    #define REG_AUTO_GPIO_EN_MASK                   BIT0|BIT1|BIT2
#define REG_ALL_PAD_IN                          0x28
    #define REG_ALL_PAD_IN_MASK                     BIT15
#define REG_TEST_IN_MODE                        0x12
    #define REG_TEST_IN_MODE_MASK                   BIT0|BIT1
#define REG_TEST_OUT_MODE                       0x12
    #define REG_TEST_OUT_MODE_MASK                  BIT4|BIT5
#define REG_I2C0_MODE                           0x6f
    #define REG_I2C0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_I2C1_MODE                           0x6f
    #define REG_I2C1_MODE_MASK                      BIT4|BIT5|BIT6|BIT7
#define REG_SR0_MIPI_MODE                       0x74
    #define REG_SR0_MIPI_MODE_MASK                  BIT0
#define REG_SR1_MIPI_MODE                       0x74
    #define REG_SR1_MIPI_MODE_MASK                  BIT4|BIT5
#define REG_ISP_IR_MODE                         0x73
    #define REG_ISP_IR_MODE_MASK                    BIT0|BIT1
#define REG_SR0_CTRL_MODE                       0x74
    #define REG_SR0_CTRL_MODE_MASK                  BIT8|BIT9
#define REG_SR0_MCLK_MODE                       0x76
    #define REG_SR0_MCLK_MODE_MASK                  BIT0|BIT1
#define REG_SR0_PDN_MODE                        0x76
    #define REG_SR0_PDN_MODE_MASK                   BIT8
#define REG_SR0_RST_MODE                        0x76
    #define REG_SR0_RST_MODE_MASK                   BIT4|BIT5
#define REG_SR1_CTRL_MODE                       0x74
    #define REG_SR1_CTRL_MODE_MASK                  BIT12|BIT13
#define REG_SR1_MCLK_MODE                       0x77
    #define REG_SR1_MCLK_MODE_MASK                  BIT0|BIT1
#define REG_SR1_PDN_MODE                        0x77
    #define REG_SR1_PDN_MODE_MASK                   BIT8|BIT9
#define REG_SR1_RST_MODE                        0x77
    #define REG_SR1_RST_MODE_MASK                   BIT4|BIT5
#define REG_SR0_BT601_MODE                      0x75
    #define REG_SR0_BT601_MODE_MASK                 BIT0|BIT1
#define REG_SR0_BT656_MODE                      0x75
    #define REG_SR0_BT656_MODE_MASK                 BIT4|BIT5|BIT6
#define REG_ETH0_MODE                           0x6e
    #define REG_ETH0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_TTL16_MODE                          0x6c
    #define REG_TTL16_MODE_MASK                     BIT0|BIT1|BIT2
#define REG_TTL18_MODE                          0x6c
    #define REG_TTL18_MODE_MASK                     BIT4|BIT5
#define REG_TTL24_MODE                          0x6c
    #define REG_TTL24_MODE_MASK                     BIT8|BIT9|BIT10
#define REG_RGB8_MODE                           0x6b
    #define REG_RGB8_MODE_MASK                      BIT0|BIT1
#define REG_BT656_OUT_MODE                      0x6c
    #define REG_BT656_OUT_MODE_MASK                 BIT12|BIT13
#define REG_PSPI0_SR_MODE                       0x69
    #define REG_PSPI0_SR_MODE_MASK                  BIT0|BIT1|BIT2
#define REG_PSPI0_G_MODE                        0x69
    #define REG_PSPI0_G_MODE_MASK                   BIT4|BIT5
#define REG_SPI0_MODE                           0x68
    #define REG_SPI0_MODE_MASK                      BIT0|BIT1|BIT2
#define REG_PSPI1_PL_MODE                       0x6a
    #define REG_PSPI1_PL_MODE_MASK                  BIT0|BIT1|BIT2|BIT3
#define REG_PSPI1_CS2_MODE                      0x6a
    #define REG_PSPI1_CS2_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_PSPI1_TE_MODE                       0x6a
    #define REG_PSPI1_TE_MODE_MASK                  BIT8|BIT9|BIT10|BIT11
#define REG_PSPI1_G_MODE                        0x6a
    #define REG_PSPI1_G_MODE_MASK                   BIT12|BIT13
#define REG_SPI1_MODE                           0x68
    #define REG_SPI1_MODE_MASK                      BIT8|BIT9|BIT10|BIT11
#define REG_SPI1_CS2_MODE                       0x68
    #define REG_SPI1_CS2_MODE_MASK                  BIT12|BIT13|BIT14
#define REG_SDIO_MODE                           0x67
    #define REG_SDIO_MODE_MASK                      BIT8|BIT9
#define REG_SD_CDZ_MODE                         0x67
    #define REG_SD_CDZ_MODE_MASK                    BIT0|BIT1
#define REG_KEY_READ0_MODE                      0x78
    #define REG_KEY_READ0_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_READ1_MODE                      0x78
    #define REG_KEY_READ1_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_READ2_MODE                      0x78
    #define REG_KEY_READ2_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_READ3_MODE                      0x78
    #define REG_KEY_READ3_MODE_MASK                 BIT12|BIT13|BIT14|BIT15
#define REG_KEY_READ4_MODE                      0x79
    #define REG_KEY_READ4_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_READ5_MODE                      0x79
    #define REG_KEY_READ5_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_READ6_MODE                      0x79
    #define REG_KEY_READ6_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_SCAN0_MODE                      0x7a
    #define REG_KEY_SCAN0_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_SCAN1_MODE                      0x7a
    #define REG_KEY_SCAN1_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_SCAN2_MODE                      0x7a
    #define REG_KEY_SCAN2_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_SCAN3_MODE                      0x7a
    #define REG_KEY_SCAN3_MODE_MASK                 BIT12|BIT13|BIT14|BIT15
#define REG_KEY_SCAN4_MODE                      0x7b
    #define REG_KEY_SCAN4_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_SCAN5_MODE                      0x7b
    #define REG_KEY_SCAN5_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_SCAN6_MODE                      0x7b
    #define REG_KEY_SCAN6_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_FIX_MODE                        0x79
    #define REG_KEY_FIX_MODE_MASK                   BIT12
#define REG_FUART_MODE                          0x6e
    #define REG_FUART_MODE_MASK                     BIT8|BIT9|BIT10|BIT11
#define REG_UART0_MODE                          0x6d
    #define REG_UART0_MODE_MASK                     BIT0|BIT1|BIT2
#define REG_UART1_MODE                          0x6d
    #define REG_UART1_MODE_MASK                     BIT4|BIT5|BIT6|BIT7
#define REG_UART2_MODE                          0x6d
    #define REG_UART2_MODE_MASK                     BIT8|BIT9|BIT10
#define REG_PWM0_MODE                           0x65
    #define REG_PWM0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_PWM1_MODE                           0x65
    #define REG_PWM1_MODE_MASK                      BIT4|BIT5|BIT6|BIT7
#define REG_PWM2_MODE                           0x65
    #define REG_PWM2_MODE_MASK                      BIT8|BIT9|BIT10|BIT11
#define REG_PWM3_MODE                           0x65
    #define REG_PWM3_MODE_MASK                      BIT12|BIT13|BIT14|BIT15
#define REG_DMIC_MODE                           0x60
    #define REG_DMIC_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_I2S_MCK_MODE                        0x62
    #define REG_I2S_MCK_MODE_MASK                   BIT0|BIT1|BIT2
#define REG_I2S_RX_MODE                         0x62
    #define REG_I2S_RX_MODE_MASK                    BIT4|BIT5|BIT6
#define REG_I2S_TX_MODE                         0x62
    #define REG_I2S_TX_MODE_MASK                    BIT8|BIT9|BIT10
#define REG_I2S_RXTX_MODE                       0x62
    #define REG_I2S_RXTX_MODE_MASK                  BIT12|BIT13|BIT14
#define REG_BT1120_MODE                         0x72
    #define REG_BT1120_MODE_MASK                    BIT0|BIT1

#define REG_PM_SPI_GPIO                         0x35
    #define REG_PM_SPI_GPIO_MASK                    BIT0
#define REG_PM_SPIWPN_GPIO                      0x35
    #define REG_PM_SPIWPN_GPIO_MASK                 BIT4
#define REG_PM_SPIHOLDN_MODE                    0x35
    #define REG_PM_SPIHOLDN_MODE_MASK               BIT6|BIT7
#define REG_PM_SPICSZ1_GPIO                     0x35
    #define REG_PM_SPICSZ1_GPIO_MASK                BIT2
#define REG_PM_SPICSZ2_GPIO                     0x35
    #define REG_PM_SPICSZ2_GPIO_MASK                BIT3
#define REG_PM_PWM0_MODE                        0x28
    #define REG_PM_PWM0_MODE_MASK                   BIT0|BIT1
#define REG_PM_PWM1_MODE                        0x28
    #define REG_PM_PWM1_MODE_MASK                   BIT2|BIT3
#define REG_PM_PWM2_MODE                        0x28
    #define REG_PM_PWM2_MODE_MASK                   BIT6|BIT7
#define REG_PM_PWM3_MODE                        0x28
    #define REG_PM_PWM3_MODE_MASK                   BIT8|BIT9
#define REG_PM_UART1_MODE                       0x27
    #define REG_PM_UART1_MODE_MASK                  BIT8
#define REG_PM_VID_MODE                         0x28
    #define REG_PM_VID_MODE_MASK                    BIT12|BIT13
#define REG_PM_SD_CDZ_MODE                      0x28
    #define REG_PM_SD_CDZ_MODE_MASK                 BIT14
#define REG_PM_LED_MODE                         0x28
    #define REG_PM_LED_MODE_MASK                    BIT4|BIT5
#define REG_PM_PAD_EXT_MODE_0                   0x38
    #define REG_PM_PAD_EXT_MODE_0_MASK              BIT0
#define REG_PM_PAD_EXT_MODE_1                   0x38
    #define REG_PM_PAD_EXT_MODE_1_MASK              BIT1
#define REG_PM_PAD_EXT_MODE_2                   0x38
    #define REG_PM_PAD_EXT_MODE_2_MASK              BIT2
#define REG_PM_PAD_EXT_MODE_3                   0x38
    #define REG_PM_PAD_EXT_MODE_3_MASK              BIT3
#define REG_PM_PAD_EXT_MODE_4                   0x38
    #define REG_PM_PAD_EXT_MODE_4_MASK              BIT4
#define REG_PM_PAD_EXT_MODE_5                   0x38
    #define REG_PM_PAD_EXT_MODE_5_MASK              BIT5
#define REG_PM_PAD_EXT_MODE_6                   0x38
    #define REG_PM_PAD_EXT_MODE_6_MASK              BIT6
#define REG_PM_PAD_EXT_MODE_7                   0x38
    #define REG_PM_PAD_EXT_MODE_7_MASK              BIT7
#define REG_PM_PAD_EXT_MODE_8                   0x38
    #define REG_PM_PAD_EXT_MODE_8_MASK              BIT8
#define REG_PM_PAD_EXT_MODE_9                   0x38
    #define REG_PM_PAD_EXT_MODE_9_MASK              BIT9
#define REG_PM_PAD_EXT_MODE_10                  0x38
    #define REG_PM_PAD_EXT_MODE_10_MASK             BIT10
// PADMUX MASK MARCO END



#define PM_PADTOP_BANK        0x003F00
#define CHIPTOP_BANK          0x101E00
#define PADTOP_BANK           0x103C00
#define PADGPIO_BANK          0x103E00
#define PM_SAR_BANK           0x001400
#define PMSLEEP_BANK          0x000E00
#define PM_GPIO_BANK          0x000F00

#define REG_SR_IO00_GPIO_MODE                   0x0
    #define REG_SR_IO00_GPIO_MODE_MASK              BIT3
#define REG_SR_IO01_GPIO_MODE                   0x1
    #define REG_SR_IO01_GPIO_MODE_MASK              BIT3
#define REG_SR_IO02_GPIO_MODE                   0x2
    #define REG_SR_IO02_GPIO_MODE_MASK              BIT3
#define REG_SR_IO03_GPIO_MODE                   0x3
    #define REG_SR_IO03_GPIO_MODE_MASK              BIT3
#define REG_SR_IO04_GPIO_MODE                   0x4
    #define REG_SR_IO04_GPIO_MODE_MASK              BIT3
#define REG_SR_IO05_GPIO_MODE                   0x5
    #define REG_SR_IO05_GPIO_MODE_MASK              BIT3
#define REG_SR_IO06_GPIO_MODE                   0x6
    #define REG_SR_IO06_GPIO_MODE_MASK              BIT3
#define REG_SR_IO07_GPIO_MODE                   0x7
    #define REG_SR_IO07_GPIO_MODE_MASK              BIT3
#define REG_SR_IO08_GPIO_MODE                   0x8
    #define REG_SR_IO08_GPIO_MODE_MASK              BIT3
#define REG_SR_IO09_GPIO_MODE                   0x9
    #define REG_SR_IO09_GPIO_MODE_MASK              BIT3
#define REG_SR_IO10_GPIO_MODE                   0xA
    #define REG_SR_IO10_GPIO_MODE_MASK              BIT3
#define REG_SR_IO11_GPIO_MODE                   0xB
    #define REG_SR_IO11_GPIO_MODE_MASK              BIT3
#define REG_SR_IO12_GPIO_MODE                   0xC
    #define REG_SR_IO12_GPIO_MODE_MASK              BIT3
#define REG_SR_IO13_GPIO_MODE                   0xD
    #define REG_SR_IO13_GPIO_MODE_MASK              BIT3
#define REG_SR_IO14_GPIO_MODE                   0xE
    #define REG_SR_IO14_GPIO_MODE_MASK              BIT3
#define REG_SR_IO15_GPIO_MODE                   0xF
    #define REG_SR_IO15_GPIO_MODE_MASK              BIT3
#define REG_SR_IO16_GPIO_MODE                   0x10
    #define REG_SR_IO16_GPIO_MODE_MASK              BIT3
#define REG_TTL0_GPIO_MODE                      0x11
    #define REG_TTL0_GPIO_MODE_MASK                 BIT3
#define REG_TTL1_GPIO_MODE                      0x12
    #define REG_TTL1_GPIO_MODE_MASK                 BIT3
#define REG_TTL2_GPIO_MODE                      0x13
    #define REG_TTL2_GPIO_MODE_MASK                 BIT3
#define REG_TTL3_GPIO_MODE                      0x14
    #define REG_TTL3_GPIO_MODE_MASK                 BIT3
#define REG_TTL4_GPIO_MODE                      0x15
    #define REG_TTL4_GPIO_MODE_MASK                 BIT3
#define REG_TTL5_GPIO_MODE                      0x16
    #define REG_TTL5_GPIO_MODE_MASK                 BIT3
#define REG_TTL6_GPIO_MODE                      0x17
    #define REG_TTL6_GPIO_MODE_MASK                 BIT3
#define REG_TTL7_GPIO_MODE                      0x18
    #define REG_TTL7_GPIO_MODE_MASK                 BIT3
#define REG_TTL8_GPIO_MODE                      0x19
    #define REG_TTL8_GPIO_MODE_MASK                 BIT3
#define REG_TTL9_GPIO_MODE                      0x1A
    #define REG_TTL9_GPIO_MODE_MASK                 BIT3
#define REG_TTL10_GPIO_MODE                     0x1B
    #define REG_TTL10_GPIO_MODE_MASK                BIT3
#define REG_TTL11_GPIO_MODE                     0x1C
    #define REG_TTL11_GPIO_MODE_MASK                BIT3
#define REG_TTL12_GPIO_MODE                     0x1D
    #define REG_TTL12_GPIO_MODE_MASK                BIT3
#define REG_TTL13_GPIO_MODE                     0x1E
    #define REG_TTL13_GPIO_MODE_MASK                BIT3
#define REG_TTL14_GPIO_MODE                     0x1F
    #define REG_TTL14_GPIO_MODE_MASK                BIT3
#define REG_TTL15_GPIO_MODE                     0x20
    #define REG_TTL15_GPIO_MODE_MASK                BIT3
#define REG_TTL16_GPIO_MODE                     0x21
    #define REG_TTL16_GPIO_MODE_MASK                BIT3
#define REG_TTL17_GPIO_MODE                     0x22
    #define REG_TTL17_GPIO_MODE_MASK                BIT3
#define REG_TTL18_GPIO_MODE                     0x23
    #define REG_TTL18_GPIO_MODE_MASK                BIT3
#define REG_TTL19_GPIO_MODE                     0x24
    #define REG_TTL19_GPIO_MODE_MASK                BIT3
#define REG_TTL20_GPIO_MODE                     0x25
    #define REG_TTL20_GPIO_MODE_MASK                BIT3
#define REG_TTL21_GPIO_MODE                     0x26
    #define REG_TTL21_GPIO_MODE_MASK                BIT3
#define REG_KEY0_GPIO_MODE                      0x27
    #define REG_KEY0_GPIO_MODE_MASK                 BIT3
#define REG_KEY1_GPIO_MODE                      0x28
    #define REG_KEY1_GPIO_MODE_MASK                 BIT3
#define REG_KEY2_GPIO_MODE                      0x29
    #define REG_KEY2_GPIO_MODE_MASK                 BIT3
#define REG_KEY3_GPIO_MODE                      0x2A
    #define REG_KEY3_GPIO_MODE_MASK                 BIT3
#define REG_KEY4_GPIO_MODE                      0x2B
    #define REG_KEY4_GPIO_MODE_MASK                 BIT3
#define REG_KEY5_GPIO_MODE                      0x2C
    #define REG_KEY5_GPIO_MODE_MASK                 BIT3
#define REG_KEY6_GPIO_MODE                      0x2D
    #define REG_KEY6_GPIO_MODE_MASK                 BIT3
#define REG_KEY7_GPIO_MODE                      0x2E
    #define REG_KEY7_GPIO_MODE_MASK                 BIT3
#define REG_KEY8_GPIO_MODE                      0x2F
    #define REG_KEY8_GPIO_MODE_MASK                 BIT3
#define REG_KEY9_GPIO_MODE                      0x30
    #define REG_KEY9_GPIO_MODE_MASK                 BIT3
#define REG_KEY10_GPIO_MODE                      0x31
    #define REG_KEY10_GPIO_MODE_MASK                 BIT3
#define REG_KEY11_GPIO_MODE                      0x32
    #define REG_KEY11_GPIO_MODE_MASK                 BIT3
#define REG_KEY12_GPIO_MODE                      0x33
    #define REG_KEY12_GPIO_MODE_MASK                 BIT3
 #define REG_KEY13_GPIO_MODE                      0x34
    #define REG_KEY13_GPIO_MODE_MASK                 BIT3
#define REG_SD_D1_GPIO_MODE                     0x35
    #define REG_SD_D1_GPIO_MODE_MASK                BIT3
#define REG_SD_D0_GPIO_MODE                     0x36
    #define REG_SD_D0_GPIO_MODE_MASK                BIT3
#define REG_SD_CLK_GPIO_MODE                    0x37
    #define REG_SD_CLK_GPIO_MODE_MASK               BIT3
#define REG_SD_CMD_GPIO_MODE                    0x38
    #define REG_SD_CMD_GPIO_MODE_MASK               BIT3
#define REG_SD_D3_GPIO_MODE                     0x39
    #define REG_SD_D3_GPIO_MODE_MASK                BIT3
#define REG_SD_D2_GPIO_MODE                     0x3A
    #define REG_SD_D2_GPIO_MODE_MASK                BIT3
#define REG_SD_GPIO0_GPIO_MODE                  0x3B
    #define REG_SD_GPIO0_GPIO_MODE_MASK             BIT3
#define REG_SD_GPIO1_GPIO_MODE                  0x3C
    #define REG_SD_GPIO1_GPIO_MODE_MASK             BIT3
#define REG_GPIO0_GPIO_MODE                     0x3D
    #define REG_GPIO0_GPIO_MODE_MASK                BIT3
#define REG_GPIO1_GPIO_MODE                     0x3E
    #define REG_GPIO1_GPIO_MODE_MASK                BIT3
#define REG_GPIO2_GPIO_MODE                     0x3F
    #define REG_GPIO2_GPIO_MODE_MASK                BIT3
#define REG_GPIO3_GPIO_MODE                     0x40
    #define REG_GPIO3_GPIO_MODE_MASK                BIT3
#define REG_GPIO4_GPIO_MODE                     0x41
    #define REG_GPIO4_GPIO_MODE_MASK                BIT3
#define REG_GPIO5_GPIO_MODE                     0x42
    #define REG_GPIO5_GPIO_MODE_MASK                BIT3
#define REG_GPIO6_GPIO_MODE                     0x43
    #define REG_GPIO6_GPIO_MODE_MASK                BIT3
#define REG_GPIO7_GPIO_MODE                     0x44
    #define REG_GPIO7_GPIO_MODE_MASK                BIT3
#define REG_GPIO8_GPIO_MODE                     0x45
    #define REG_GPIO8_GPIO_MODE_MASK                BIT3
#define REG_GPIO9_GPIO_MODE                     0x46
    #define REG_GPIO9_GPIO_MODE_MASK                BIT3

#define REG_ETH_GPIO_EN_MODE                    0x71
    #define REG_ETH_GPIO_EN_MODE_MASK               BIT0
#define REG_MISC_ETH_0_MODE                     0x69
    #define REG_MISC_ETH_0_MODE_MASK                BIT14|BIT15
#define REG_MISC_ETH_1_MODE                     0x69
    #define REG_MISC_ETH_1_MODE_MASK                BIT10|BIT11
#define REG_MISC_ETH_2_MODE                     0x44
    #define REG_MISC_ETH_2_MODE_MASK                BIT4




#define REG_SAR_GPIO0_GPIO_MODE         0x11
    #define REG_SAR_GPIO0_GPIO_MODE_MASK    BIT0
#define REG_SAR_GPIO1_GPIO_MODE         0x11
    #define REG_SAR_GPIO1_GPIO_MODE_MASK    BIT1
#define REG_SAR_GPIO2_GPIO_MODE         0x11
    #define REG_SAR_GPIO2_GPIO_MODE_MASK    BIT2
#define REG_SAR_GPIO3_GPIO_MODE         0x11
    #define REG_SAR_GPIO3_GPIO_MODE_MASK    BIT3

//-------------------- configuration -----------------
#define ENABLE_CHECK_ALL_PAD_CONFLICT       1

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

typedef struct stPadMux
{
    U16 padID;
    U32 base;
    U16 offset;
    U16 mask;
    U16 val;
    U16 mode;
} ST_PadMuxInfo;

typedef struct stPadMode
{
    U8  u8PadName[32];
    U32 u32ModeRIU;
    U16 u16ModeMask;
    U16 u16ModeVal;
} ST_PadModeInfo;

typedef struct stPadCheck
{
    U16 base;
    U16 offset;
    U16 mask;
    U16 val;
    U16 regval;
}ST_PadCheckInfo;

typedef struct stPadCheckVal
{
    U8  infocount;
    struct stPadCheck infos[64];
}ST_PadCheckVal;

typedef struct stPadMuxEntry{
    U32 size;
    const ST_PadMuxInfo* padmux;
} ST_PadMuxEntry;

ST_PadCheckVal m_stPadCheckVal;

const ST_PadMuxInfo sr_io00_tbl[] =
{
    {PAD_SR_IO00,      PADGPIO_BANK,     REG_SR_IO00_GPIO_MODE,          REG_SR_IO00_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO00,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO00,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO00,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_I2C0_MODE_3},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT5,                   PINMUX_FOR_I2C1_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT4,                   PINMUX_FOR_SR1_MIPI_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_3},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_5},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT4,                   PINMUX_FOR_PSPI0_G_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1,                   PINMUX_FOR_SPI0_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI1_PL_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT8,                   PINMUX_FOR_FUART_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9|BIT8,        PINMUX_FOR_FUART_MODE_7},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT4,                   PINMUX_FOR_UART1_MODE_1},
    {PAD_SR_IO00,      PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT0,                   PINMUX_FOR_PWM0_MODE_1},
};
const ST_PadMuxInfo sr_io01_tbl[] =
{
    {PAD_SR_IO01,      PADGPIO_BANK,     REG_SR_IO01_GPIO_MODE,          REG_SR_IO01_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO01,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO01,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO01,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_I2C0_MODE_3},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT5,                   PINMUX_FOR_I2C1_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT4,                   PINMUX_FOR_SR1_MIPI_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI0_SR_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_3},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI0_SR_MODE_4},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_5},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI0_SR_MODE_6},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT4,                   PINMUX_FOR_PSPI0_G_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1,                   PINMUX_FOR_SPI0_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI1_PL_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT8,                   PINMUX_FOR_FUART_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9|BIT8,        PINMUX_FOR_FUART_MODE_7},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT4,                   PINMUX_FOR_UART1_MODE_1},
    {PAD_SR_IO01,      PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT4,                   PINMUX_FOR_PWM1_MODE_1},
};
const ST_PadMuxInfo sr_io02_tbl[] =
{
    {PAD_SR_IO02,      PADGPIO_BANK,     REG_SR_IO02_GPIO_MODE,          REG_SR_IO02_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO02,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO02,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO02,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT4,                   PINMUX_FOR_SR1_MIPI_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI0_SR_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_3},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI0_SR_MODE_4},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_5},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI0_SR_MODE_6},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT4,                   PINMUX_FOR_PSPI0_G_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1,                   PINMUX_FOR_SPI0_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI1_PL_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT8,                   PINMUX_FOR_FUART_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT5,                   PINMUX_FOR_UART1_MODE_2},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT8,                   PINMUX_FOR_UART2_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT8,                   PINMUX_FOR_PWM2_MODE_1},
    {PAD_SR_IO02,      PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT0,                   PINMUX_FOR_I2S_MCK_MODE_1},
};
const ST_PadMuxInfo sr_io03_tbl[] =
{
    {PAD_SR_IO03,      PADGPIO_BANK,     REG_SR_IO03_GPIO_MODE,          REG_SR_IO03_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO03,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO03,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO03,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT4,                   PINMUX_FOR_SR1_MIPI_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI0_SR_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI0_SR_MODE_3},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI0_SR_MODE_4},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT4,                   PINMUX_FOR_PSPI0_G_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1,                   PINMUX_FOR_SPI0_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI1_PL_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT8,                   PINMUX_FOR_FUART_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT5,                   PINMUX_FOR_UART1_MODE_2},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT8,                   PINMUX_FOR_UART2_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT12,                  PINMUX_FOR_PWM3_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT4,                   PINMUX_FOR_I2S_RX_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT8,                   PINMUX_FOR_I2S_TX_MODE_1},
    {PAD_SR_IO03,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT12,                  PINMUX_FOR_I2S_RXTX_MODE_1},
};
const ST_PadMuxInfo sr_io04_tbl[] =
{
    {PAD_SR_IO04,      PADGPIO_BANK,     REG_SR_IO04_GPIO_MODE,          REG_SR_IO04_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO04,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO04,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO04,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2,                   PINMUX_FOR_I2C0_MODE_4},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT5|BIT4,              PINMUX_FOR_I2C1_MODE_3},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_MIPI_MODE,              REG_SR0_MIPI_MODE_MASK,             BIT0,                   PINMUX_FOR_SR0_MIPI_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI0_SR_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT5,                   PINMUX_FOR_PSPI1_CS2_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT9,                   PINMUX_FOR_PSPI1_TE_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT1,                   PINMUX_FOR_PWM0_MODE_2},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT0,                   PINMUX_FOR_DMIC_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_DMIC_MODE_7},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT4,                   PINMUX_FOR_I2S_RX_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT8,                   PINMUX_FOR_I2S_TX_MODE_1},
    {PAD_SR_IO04,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT12,                  PINMUX_FOR_I2S_RXTX_MODE_1},
};
const ST_PadMuxInfo sr_io05_tbl[] =
{
    {PAD_SR_IO05,      PADGPIO_BANK,     REG_SR_IO05_GPIO_MODE,          REG_SR_IO05_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO05,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO05,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO05,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2,                   PINMUX_FOR_I2C0_MODE_4},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT5|BIT4,              PINMUX_FOR_I2C1_MODE_3},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_MIPI_MODE,              REG_SR0_MIPI_MODE_MASK,             BIT0,                   PINMUX_FOR_SR0_MIPI_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR1_MIPI_MODE,              REG_SR1_MIPI_MODE_MASK,             BIT5,                   PINMUX_FOR_SR1_MIPI_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT0,                   PINMUX_FOR_PSPI0_SR_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_PSPI0_SR_MODE,              REG_PSPI0_SR_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI0_SR_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT5,                   PINMUX_FOR_PWM1_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT0,                   PINMUX_FOR_DMIC_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_DMIC_MODE_7},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT4,                   PINMUX_FOR_I2S_RX_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5,                   PINMUX_FOR_I2S_RX_MODE_2},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT12,                  PINMUX_FOR_I2S_RXTX_MODE_1},
    {PAD_SR_IO05,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13,                  PINMUX_FOR_I2S_RXTX_MODE_2},
};
const ST_PadMuxInfo sr_io06_tbl[] =
{
    {PAD_SR_IO06,      PADGPIO_BANK,     REG_SR_IO06_GPIO_MODE,          REG_SR_IO06_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO06,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO06,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO06,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_I2C0_MODE_5},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6,                   PINMUX_FOR_I2C1_MODE_4},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_MIPI_MODE,              REG_SR0_MIPI_MODE_MASK,             BIT0,                   PINMUX_FOR_SR0_MIPI_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_PSPI1_CS2_MODE_3},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT9|BIT8,              PINMUX_FOR_PSPI1_TE_MODE_3},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9,                   PINMUX_FOR_FUART_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11,                  PINMUX_FOR_FUART_MODE_8},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT9,                   PINMUX_FOR_PWM2_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT0,                   PINMUX_FOR_DMIC_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT8,                   PINMUX_FOR_I2S_TX_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9,                   PINMUX_FOR_I2S_TX_MODE_2},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT12,                  PINMUX_FOR_I2S_RXTX_MODE_1},
    {PAD_SR_IO06,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13,                  PINMUX_FOR_I2S_RXTX_MODE_2},
};
const ST_PadMuxInfo sr_io07_tbl[] =
{
    {PAD_SR_IO07,      PADGPIO_BANK,     REG_SR_IO07_GPIO_MODE,          REG_SR_IO07_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO07,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO07,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_SR_IO07,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_I2C0_MODE_5},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6,                   PINMUX_FOR_I2C1_MODE_4},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_MIPI_MODE,              REG_SR0_MIPI_MODE_MASK,             BIT0,                   PINMUX_FOR_SR0_MIPI_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI1_PL_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT12,                  PINMUX_FOR_PSPI1_G_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT8,                   PINMUX_FOR_SPI1_MODE_1},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9,                   PINMUX_FOR_FUART_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11,                  PINMUX_FOR_FUART_MODE_8},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT13,                  PINMUX_FOR_PWM3_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3,                   PINMUX_FOR_DMIC_MODE_8},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT1,                   PINMUX_FOR_I2S_MCK_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5,                   PINMUX_FOR_I2S_RX_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9,                   PINMUX_FOR_I2S_TX_MODE_2},
    {PAD_SR_IO07,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13,                  PINMUX_FOR_I2S_RXTX_MODE_2},
};
const ST_PadMuxInfo sr_io08_tbl[] =
{
    {PAD_SR_IO08,      PADGPIO_BANK,     REG_SR_IO08_GPIO_MODE,          REG_SR_IO08_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO08,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_I2C0_MODE_6},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT4,              PINMUX_FOR_I2C1_MODE_5},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_11},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_ISP_IR_MODE,                REG_ISP_IR_MODE_MASK,               BIT0,                   PINMUX_FOR_ISP_IR_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT12,                  PINMUX_FOR_SR1_CTRL_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR1_PDN_MODE,               REG_SR1_PDN_MODE_MASK,              BIT8,                   PINMUX_FOR_SR1_PDN_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT0,                   PINMUX_FOR_ETH0_MODE_1},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_ETH0_MODE_3},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9,                   PINMUX_FOR_FUART_MODE_2},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_UART1_MODE_3},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_PWM0_MODE_3},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3,                   PINMUX_FOR_DMIC_MODE_8},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5,                   PINMUX_FOR_I2S_RX_MODE_2},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9,                   PINMUX_FOR_I2S_TX_MODE_2},
    {PAD_SR_IO08,      PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13,                  PINMUX_FOR_I2S_RXTX_MODE_2},
};
const ST_PadMuxInfo sr_io09_tbl[] =
{
    {PAD_SR_IO09,      PADGPIO_BANK,     REG_SR_IO09_GPIO_MODE,          REG_SR_IO09_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO09,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO09,      CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_SR_IO09,      CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT9,                   PINMUX_FOR_SR0_CTRL_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_MCLK_MODE,              REG_SR0_MCLK_MODE_MASK,             BIT1,                   PINMUX_FOR_SR0_MCLK_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT12,                  PINMUX_FOR_SR1_CTRL_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT13,                  PINMUX_FOR_SR1_CTRL_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR1_MCLK_MODE,              REG_SR1_MCLK_MODE_MASK,             BIT1,                   PINMUX_FOR_SR1_MCLK_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR1_RST_MODE,               REG_SR1_RST_MODE_MASK,              BIT4,                   PINMUX_FOR_SR1_RST_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT1,                   PINMUX_FOR_ETH0_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2,                   PINMUX_FOR_ETH0_MODE_4},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI1_PL_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT12,                  PINMUX_FOR_PSPI1_G_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT8,                   PINMUX_FOR_SPI1_MODE_1},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9,                   PINMUX_FOR_FUART_MODE_2},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_UART1_MODE_3},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT5|BIT4,              PINMUX_FOR_PWM1_MODE_3},
    {PAD_SR_IO09,      PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_I2S_MCK_MODE_3},
};
const ST_PadMuxInfo sr_io10_tbl[] =
{
    {PAD_SR_IO10,      PADGPIO_BANK,     REG_SR_IO10_GPIO_MODE,          REG_SR_IO10_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO10,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT9,                   PINMUX_FOR_SR0_CTRL_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_RST_MODE,               REG_SR0_RST_MODE_MASK,              BIT5,                   PINMUX_FOR_SR0_RST_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT12,                  PINMUX_FOR_SR1_CTRL_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT13,                  PINMUX_FOR_SR1_CTRL_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR1_MCLK_MODE,              REG_SR1_MCLK_MODE_MASK,             BIT0,                   PINMUX_FOR_SR1_MCLK_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR1_RST_MODE,               REG_SR1_RST_MODE_MASK,              BIT5,                   PINMUX_FOR_SR1_RST_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_PSPI0_G_MODE,               REG_PSPI0_G_MODE_MASK,              BIT5,                   PINMUX_FOR_PSPI0_G_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI1_PL_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT12,                  PINMUX_FOR_PSPI1_G_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT13,                  PINMUX_FOR_PSPI1_G_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT8,                   PINMUX_FOR_SPI1_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT0,                   PINMUX_FOR_UART0_MODE_1},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT9,                   PINMUX_FOR_UART2_MODE_2},
    {PAD_SR_IO10,      PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT9|BIT8,              PINMUX_FOR_PWM2_MODE_3},
};
const ST_PadMuxInfo sr_io11_tbl[] =
{
    {PAD_SR_IO11,      PADGPIO_BANK,     REG_SR_IO11_GPIO_MODE,          REG_SR_IO11_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO11,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_I2C0_MODE_6},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT4,              PINMUX_FOR_I2C1_MODE_5},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_11},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_ISP_IR_MODE,                REG_ISP_IR_MODE_MASK,               BIT1,                   PINMUX_FOR_ISP_IR_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT8,                   PINMUX_FOR_SR0_CTRL_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT9,                   PINMUX_FOR_SR0_CTRL_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_PDN_MODE,               REG_SR0_PDN_MODE_MASK,              BIT8,                   PINMUX_FOR_SR0_PDN_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR1_CTRL_MODE,              REG_SR1_CTRL_MODE_MASK,             BIT13,                  PINMUX_FOR_SR1_CTRL_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR1_PDN_MODE,               REG_SR1_PDN_MODE_MASK,              BIT9,                   PINMUX_FOR_SR1_PDN_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1,                   PINMUX_FOR_SR0_BT601_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5,                   PINMUX_FOR_SR0_BT656_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT6,                   PINMUX_FOR_SR0_BT656_MODE_4},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13,                  PINMUX_FOR_BT656_OUT_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1,                   PINMUX_FOR_PSPI1_PL_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT6,                   PINMUX_FOR_PSPI1_CS2_MODE_4},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT10,                  PINMUX_FOR_PSPI1_TE_MODE_4},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_PSPI1_G_MODE,               REG_PSPI1_G_MODE_MASK,              BIT12,                  PINMUX_FOR_PSPI1_G_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT8,                   PINMUX_FOR_SPI1_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT0,                   PINMUX_FOR_UART0_MODE_1},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT9,                   PINMUX_FOR_UART2_MODE_2},
    {PAD_SR_IO11,      PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT13|BIT12,            PINMUX_FOR_PWM3_MODE_3},
};
const ST_PadMuxInfo sr_io12_tbl[] =
{
    {PAD_SR_IO12,      PADGPIO_BANK,     REG_SR_IO12_GPIO_MODE,          REG_SR_IO12_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO12,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT8,                   PINMUX_FOR_SR0_CTRL_MODE_1},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SR0_RST_MODE,               REG_SR0_RST_MODE_MASK,              BIT4,                   PINMUX_FOR_SR0_RST_MODE_1},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_3},
    {PAD_SR_IO12,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9,                   PINMUX_FOR_SPI1_MODE_2},
};
const ST_PadMuxInfo sr_io13_tbl[] =
{
    {PAD_SR_IO13,      PADGPIO_BANK,     REG_SR_IO13_GPIO_MODE,          REG_SR_IO13_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO13,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SR0_CTRL_MODE,              REG_SR0_CTRL_MODE_MASK,             BIT8,                   PINMUX_FOR_SR0_CTRL_MODE_1},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SR0_MCLK_MODE,              REG_SR0_MCLK_MODE_MASK,             BIT0,                   PINMUX_FOR_SR0_MCLK_MODE_1},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT4,                   PINMUX_FOR_SR0_BT656_MODE_1},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SR0_BT656_MODE,             REG_SR0_BT656_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_SR0_BT656_MODE_3},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_3},
    {PAD_SR_IO13,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9,                   PINMUX_FOR_SPI1_MODE_2},
};
const ST_PadMuxInfo sr_io14_tbl[] =
{
    {PAD_SR_IO14,      PADGPIO_BANK,     REG_SR_IO14_GPIO_MODE,          REG_SR_IO14_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO14,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_I2C0_MODE_7},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT5,              PINMUX_FOR_I2C1_MODE_6},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_11},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_3},
    {PAD_SR_IO14,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9,                   PINMUX_FOR_SPI1_MODE_2},
};
const ST_PadMuxInfo sr_io15_tbl[] =
{
    {PAD_SR_IO15,      PADGPIO_BANK,     REG_SR_IO15_GPIO_MODE,          REG_SR_IO15_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO15,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_I2C0_MODE_7},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT5,              PINMUX_FOR_I2C1_MODE_6},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_11},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT0,                   PINMUX_FOR_SR0_BT601_MODE_1},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT1|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_3},
    {PAD_SR_IO15,      PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9,                   PINMUX_FOR_SPI1_MODE_2},
};
const ST_PadMuxInfo sr_io16_tbl[] =
{
    {PAD_SR_IO16,      PADGPIO_BANK,     REG_SR_IO16_GPIO_MODE,          REG_SR_IO16_GPIO_MODE_MASK,         BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SR_IO16,      CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SR_IO16,      PADTOP_BANK,      REG_ISP_IR_MODE,                REG_ISP_IR_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_ISP_IR_MODE_3},
    {PAD_SR_IO16,      PADTOP_BANK,      REG_SR0_BT601_MODE,             REG_SR0_BT601_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_SR0_BT601_MODE_3},
    {PAD_SR_IO16,      PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT4,                   PINMUX_FOR_PSPI1_CS2_MODE_1},
    {PAD_SR_IO16,      PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT8,                   PINMUX_FOR_PSPI1_TE_MODE_1},
    {PAD_SR_IO16,      PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT11|BIT9|BIT8,        PINMUX_FOR_PWM2_MODE_11},
};
const ST_PadMuxInfo ttl0_tbl[] =
{
    {PAD_TTL0,         PADGPIO_BANK,     REG_TTL0_GPIO_MODE,             REG_TTL0_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL0,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL0,         PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT0,                   PINMUX_FOR_I2C0_MODE_1},
    {PAD_TTL0,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL0,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL0,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL0,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL0,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL0,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL0,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL0,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI1_PL_MODE_4},
    {PAD_TTL0,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9|BIT8,              PINMUX_FOR_SPI1_MODE_3},
    {PAD_TTL0,         PADTOP_BANK,      REG_SD_CDZ_MODE,                REG_SD_CDZ_MODE_MASK,               BIT1,                   PINMUX_FOR_SD_CDZ_MODE_2},
};
const ST_PadMuxInfo ttl1_tbl[] =
{
    {PAD_TTL1,         PADGPIO_BANK,     REG_TTL1_GPIO_MODE,             REG_TTL1_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL1,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT0,                   PINMUX_FOR_I2C0_MODE_1},
    {PAD_TTL1,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL1,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL1,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI1_PL_MODE_4},
    {PAD_TTL1,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9|BIT8,              PINMUX_FOR_SPI1_MODE_3},
    {PAD_TTL1,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
};
const ST_PadMuxInfo ttl2_tbl[] =
{
    {PAD_TTL2,         PADGPIO_BANK,     REG_TTL2_GPIO_MODE,             REG_TTL2_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL2,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL2,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL2,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI1_PL_MODE_4},
    {PAD_TTL2,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9|BIT8,              PINMUX_FOR_SPI1_MODE_3},
    {PAD_TTL2,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
};
const ST_PadMuxInfo ttl3_tbl[] =
{
    {PAD_TTL3,         PADGPIO_BANK,     REG_TTL3_GPIO_MODE,             REG_TTL3_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL3,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL3,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL3,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2,                   PINMUX_FOR_PSPI1_PL_MODE_4},
    {PAD_TTL3,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT9|BIT8,              PINMUX_FOR_SPI1_MODE_3},
    {PAD_TTL3,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
};
const ST_PadMuxInfo ttl4_tbl[] =
{
    {PAD_TTL4,         PADGPIO_BANK,     REG_TTL4_GPIO_MODE,             REG_TTL4_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL4,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL4,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL4,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL4,         PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_SPI0_MODE_5},
    {PAD_TTL4,         PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT6|BIT4,              PINMUX_FOR_PSPI1_CS2_MODE_5},
    {PAD_TTL4,         PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT10|BIT8,             PINMUX_FOR_PSPI1_TE_MODE_5},
    {PAD_TTL4,         PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT12,                  PINMUX_FOR_SPI1_CS2_MODE_1},
    {PAD_TTL4,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
    {PAD_TTL4,         PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT2,                   PINMUX_FOR_PWM0_MODE_4},
};
const ST_PadMuxInfo ttl5_tbl[] =
{
    {PAD_TTL5,         PADGPIO_BANK,     REG_TTL5_GPIO_MODE,             REG_TTL5_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL5,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL5,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL5,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL5,         PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_SPI0_MODE_5},
    {PAD_TTL5,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
    {PAD_TTL5,         PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT6,                   PINMUX_FOR_PWM1_MODE_4},
};
const ST_PadMuxInfo ttl6_tbl[] =
{
    {PAD_TTL6,         PADGPIO_BANK,     REG_TTL6_GPIO_MODE,             REG_TTL6_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL6,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL6,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL6,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL6,         PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_SPI0_MODE_5},
    {PAD_TTL6,         PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT9,                   PINMUX_FOR_SDIO_MODE_2},
    {PAD_TTL6,         PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6,                   PINMUX_FOR_UART1_MODE_4},
    {PAD_TTL6,         PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT10,                  PINMUX_FOR_PWM2_MODE_4},
};
const ST_PadMuxInfo ttl7_tbl[] =
{
    {PAD_TTL7,         PADGPIO_BANK,     REG_TTL7_GPIO_MODE,             REG_TTL7_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL7,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL7,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL7,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL7,         PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_SPI0_MODE_5},
    {PAD_TTL7,         PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6,                   PINMUX_FOR_UART1_MODE_4},
    {PAD_TTL7,         PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT14,                  PINMUX_FOR_PWM3_MODE_4},
};
const ST_PadMuxInfo ttl8_tbl[] =
{
    {PAD_TTL8,         PADGPIO_BANK,     REG_TTL8_GPIO_MODE,             REG_TTL8_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL8,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3,                   PINMUX_FOR_I2C0_MODE_8},
    {PAD_TTL8,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_ETH0_MODE_5},
    {PAD_TTL8,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_ETH0_MODE_6},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL8,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL8,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL8,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_FUART_MODE_3},
    {PAD_TTL8,         PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT8,             PINMUX_FOR_FUART_MODE_9},
    {PAD_TTL8,         PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT4,              PINMUX_FOR_UART1_MODE_5},
};
const ST_PadMuxInfo ttl9_tbl[] =
{
    {PAD_TTL9,         PADGPIO_BANK,     REG_TTL9_GPIO_MODE,             REG_TTL9_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL9,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL9,         CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL9,         CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL9,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL9,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL9,         PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL9,         PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
};
const ST_PadMuxInfo ttl10_tbl[] =
{
    {PAD_TTL10,        PADGPIO_BANK,     REG_TTL10_GPIO_MODE,            REG_TTL10_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL10,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL10,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL10,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL10,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL10,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL10,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL10,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
};
const ST_PadMuxInfo ttl11_tbl[] =
{
    {PAD_TTL11,        PADGPIO_BANK,     REG_TTL11_GPIO_MODE,            REG_TTL11_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL11,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3,                   PINMUX_FOR_I2C0_MODE_8},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL11,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL11,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL11,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL11,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_FUART_MODE_3},
    {PAD_TTL11,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT8,             PINMUX_FOR_FUART_MODE_9},
    {PAD_TTL11,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT4,              PINMUX_FOR_UART1_MODE_5},
};
const ST_PadMuxInfo ttl12_tbl[] =
{
    {PAD_TTL12,        PADGPIO_BANK,     REG_TTL12_GPIO_MODE,            REG_TTL12_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL12,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL12,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL12,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL12,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL12,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL12,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_RGB8_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL12,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_BT656_OUT_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_FUART_MODE_3},
    {PAD_TTL12,        PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_UART2_MODE_3},
};
const ST_PadMuxInfo ttl13_tbl[] =
{
    {PAD_TTL13,        PADGPIO_BANK,     REG_TTL13_GPIO_MODE,            REG_TTL13_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL13,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL13,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL13,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL13,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL13,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL13,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL13,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_FUART_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_UART2_MODE_3},
    {PAD_TTL13,        PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT2,                   PINMUX_FOR_I2S_MCK_MODE_4},
};
const ST_PadMuxInfo ttl14_tbl[] =
{
    {PAD_TTL14,        PADGPIO_BANK,     REG_TTL14_GPIO_MODE,            REG_TTL14_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL14,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL14,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL14,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL14,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL14,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL14,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL14,        PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_PWM0_MODE_5},
    {PAD_TTL14,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5|BIT4,              PINMUX_FOR_I2S_RX_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9|BIT8,              PINMUX_FOR_I2S_TX_MODE_3},
    {PAD_TTL14,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_3},
};
const ST_PadMuxInfo ttl15_tbl[] =
{
    {PAD_TTL15,        PADGPIO_BANK,     REG_TTL15_GPIO_MODE,            REG_TTL15_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL15,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL15,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL15,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL15,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL15,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL15,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL15,        PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT6|BIT4,              PINMUX_FOR_PWM1_MODE_5},
    {PAD_TTL15,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1,                   PINMUX_FOR_DMIC_MODE_2},
    {PAD_TTL15,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5|BIT4,              PINMUX_FOR_I2S_RX_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9|BIT8,              PINMUX_FOR_I2S_TX_MODE_3},
    {PAD_TTL15,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_3},
};
const ST_PadMuxInfo ttl16_tbl[] =
{
    {PAD_TTL16,        PADGPIO_BANK,     REG_TTL16_GPIO_MODE,            REG_TTL16_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL16,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL16,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL16,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL16,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL16,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL16,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL16,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL16,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_5},
    {PAD_TTL16,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10,                  PINMUX_FOR_SPI1_MODE_4},
    {PAD_TTL16,        PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT10|BIT8,             PINMUX_FOR_PWM2_MODE_5},
    {PAD_TTL16,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1,                   PINMUX_FOR_DMIC_MODE_2},
    {PAD_TTL16,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_DMIC_MODE_9},
    {PAD_TTL16,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT5|BIT4,              PINMUX_FOR_I2S_RX_MODE_3},
    {PAD_TTL16,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_3},
};
const ST_PadMuxInfo ttl17_tbl[] =
{
    {PAD_TTL17,        PADGPIO_BANK,     REG_TTL17_GPIO_MODE,            REG_TTL17_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT0,                   PINMUX_FOR_TEST_IN_MODE_1},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT4,                   PINMUX_FOR_TEST_OUT_MODE_1},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL17,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL17,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL17,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL17,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL17,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL17,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL17,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL17,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_5},
    {PAD_TTL17,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10,                  PINMUX_FOR_SPI1_MODE_4},
    {PAD_TTL17,        PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT14|BIT12,            PINMUX_FOR_PWM3_MODE_5},
    {PAD_TTL17,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1,                   PINMUX_FOR_DMIC_MODE_2},
    {PAD_TTL17,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_DMIC_MODE_9},
    {PAD_TTL17,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT9|BIT8,              PINMUX_FOR_I2S_TX_MODE_3},
    {PAD_TTL17,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT13|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_3},
};
const ST_PadMuxInfo ttl18_tbl[] =
{
    {PAD_TTL18,        PADGPIO_BANK,     REG_TTL18_GPIO_MODE,            REG_TTL18_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL18,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL18,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL18,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL18,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL18,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL18,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_I2C0_MODE_9},
    {PAD_TTL18,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL18,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL18,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL18,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL18,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL18,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL18,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_SPI0_MODE_3},
    {PAD_TTL18,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_5},
    {PAD_TTL18,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10,                  PINMUX_FOR_SPI1_MODE_4},
    {PAD_TTL18,        PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_PWM0_MODE_6},
};
const ST_PadMuxInfo ttl19_tbl[] =
{
    {PAD_TTL19,        PADGPIO_BANK,     REG_TTL19_GPIO_MODE,            REG_TTL19_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL19,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL19,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_TTL19,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_TTL19,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_TTL19,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_TTL19,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_I2C0_MODE_9},
    {PAD_TTL19,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL19,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL19,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL19,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL19,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL19,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL19,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_SPI0_MODE_3},
    {PAD_TTL19,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_5},
    {PAD_TTL19,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10,                  PINMUX_FOR_SPI1_MODE_4},
    {PAD_TTL19,        PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT6|BIT5,              PINMUX_FOR_PWM1_MODE_6},
};
const ST_PadMuxInfo ttl20_tbl[] =
{
    {PAD_TTL20,        PADGPIO_BANK,     REG_TTL20_GPIO_MODE,            REG_TTL20_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL20,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_7},
    {PAD_TTL20,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_7},
    {PAD_TTL20,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3,                   PINMUX_FOR_ETH0_MODE_8},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT0,                   PINMUX_FOR_TTL16_MODE_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1,                   PINMUX_FOR_TTL16_MODE_2},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9|BIT8,              PINMUX_FOR_TTL24_MODE_3},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_TTL20,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_TTL20,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT0,                   PINMUX_FOR_RGB8_MODE_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL20,        PADTOP_BANK,      REG_BT656_OUT_MODE,             REG_BT656_OUT_MODE_MASK,            BIT12,                  PINMUX_FOR_BT656_OUT_MODE_1},
    {PAD_TTL20,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_SPI0_MODE_3},
    {PAD_TTL20,        PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT6|BIT5,              PINMUX_FOR_PSPI1_CS2_MODE_6},
    {PAD_TTL20,        PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT10|BIT9,             PINMUX_FOR_PSPI1_TE_MODE_6},
    {PAD_TTL20,        PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT13,                  PINMUX_FOR_SPI1_CS2_MODE_2},
    {PAD_TTL20,        PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT10|BIT9,             PINMUX_FOR_PWM2_MODE_6},
};
const ST_PadMuxInfo ttl21_tbl[] =
{
    {PAD_TTL21,        PADGPIO_BANK,     REG_TTL21_GPIO_MODE,            REG_TTL21_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_TTL21,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_TTL21,        PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT6|BIT5|BIT4,         PINMUX_FOR_I2C1_MODE_7},
    {PAD_TTL21,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_TTL16_MODE_3},
    {PAD_TTL21,        PADTOP_BANK,      REG_TTL16_MODE,                 REG_TTL16_MODE_MASK,                BIT2,                   PINMUX_FOR_TTL16_MODE_4},
    {PAD_TTL21,        PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5|BIT4,              PINMUX_FOR_TTL18_MODE_3},
    {PAD_TTL21,        PADTOP_BANK,      REG_RGB8_MODE,                  REG_RGB8_MODE_MASK,                 BIT1,                   PINMUX_FOR_RGB8_MODE_2},
    {PAD_TTL21,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_SPI0_MODE_3},
    {PAD_TTL21,        PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT14|BIT13,            PINMUX_FOR_PWM3_MODE_6},
};
const ST_PadMuxInfo key0_tbl[] =
{
    {PAD_KEY0,         PADGPIO_BANK,     REG_KEY0_GPIO_MODE,             REG_KEY0_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY0,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT5,                   PINMUX_FOR_TTL18_MODE_2},
    {PAD_KEY0,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY0,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY0,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI1_PL_MODE_6},
    {PAD_KEY0,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT8,             PINMUX_FOR_SPI1_MODE_5},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT0,                   PINMUX_FOR_KEY_READ0_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT4,                   PINMUX_FOR_KEY_READ1_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT8,                   PINMUX_FOR_KEY_READ2_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT12,                  PINMUX_FOR_KEY_READ3_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT0,                   PINMUX_FOR_KEY_READ4_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT4,                   PINMUX_FOR_KEY_READ5_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT8,                   PINMUX_FOR_KEY_READ6_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT0,                   PINMUX_FOR_KEY_SCAN0_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT4,                   PINMUX_FOR_KEY_SCAN1_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT8,                   PINMUX_FOR_KEY_SCAN2_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT12,                  PINMUX_FOR_KEY_SCAN3_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT0,                   PINMUX_FOR_KEY_SCAN4_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT4,                   PINMUX_FOR_KEY_SCAN5_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT8,                   PINMUX_FOR_KEY_SCAN6_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT2|BIT1|BIT0,         PINMUX_FOR_PWM0_MODE_7},
    {PAD_KEY0,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY0,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key1_tbl[] =
{
    {PAD_KEY1,         PADGPIO_BANK,     REG_KEY1_GPIO_MODE,             REG_KEY1_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY1,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY1,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY1,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY1,         PADTOP_BANK,      REG_TTL18_MODE,                 REG_TTL18_MODE_MASK,                BIT4,                   PINMUX_FOR_TTL18_MODE_1},
    {PAD_KEY1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY1,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY1,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI1_PL_MODE_6},
    {PAD_KEY1,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT8,             PINMUX_FOR_SPI1_MODE_5},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT1,                   PINMUX_FOR_KEY_READ0_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT5,                   PINMUX_FOR_KEY_READ1_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT9,                   PINMUX_FOR_KEY_READ2_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT13,                  PINMUX_FOR_KEY_READ3_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT1,                   PINMUX_FOR_KEY_READ4_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT5,                   PINMUX_FOR_KEY_READ5_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT9,                   PINMUX_FOR_KEY_READ6_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT1,                   PINMUX_FOR_KEY_SCAN0_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT5,                   PINMUX_FOR_KEY_SCAN1_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT9,                   PINMUX_FOR_KEY_SCAN2_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT13,                  PINMUX_FOR_KEY_SCAN3_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT1,                   PINMUX_FOR_KEY_SCAN4_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT5,                   PINMUX_FOR_KEY_SCAN5_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT9,                   PINMUX_FOR_KEY_SCAN6_MODE_2},
    {PAD_KEY1,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY1,         PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT6|BIT5|BIT4,         PINMUX_FOR_PWM1_MODE_7},
    {PAD_KEY1,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY1,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key2_tbl[] =
{
    {PAD_KEY2,         PADGPIO_BANK,     REG_KEY2_GPIO_MODE,             REG_KEY2_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY2,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY2,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY2,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY2,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY2,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI1_PL_MODE_6},
    {PAD_KEY2,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT8,             PINMUX_FOR_SPI1_MODE_5},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_KEY_READ0_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_KEY_READ1_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT9|BIT8,              PINMUX_FOR_KEY_READ2_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_KEY_READ3_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_KEY_READ4_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_KEY_READ5_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT9|BIT8,              PINMUX_FOR_KEY_READ6_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_KEY_SCAN0_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_KEY_SCAN1_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT9|BIT8,              PINMUX_FOR_KEY_SCAN2_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT13|BIT12,            PINMUX_FOR_KEY_SCAN3_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT1|BIT0,              PINMUX_FOR_KEY_SCAN4_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT5|BIT4,              PINMUX_FOR_KEY_SCAN5_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT9|BIT8,              PINMUX_FOR_KEY_SCAN6_MODE_3},
    {PAD_KEY2,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY2,         PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT10|BIT9|BIT8,        PINMUX_FOR_PWM2_MODE_7},
    {PAD_KEY2,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY2,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key3_tbl[] =
{
    {PAD_KEY3,         PADGPIO_BANK,     REG_KEY3_GPIO_MODE,             REG_KEY3_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY3,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY3,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY3,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY3,         PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1,              PINMUX_FOR_PSPI1_PL_MODE_6},
    {PAD_KEY3,         PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT8,             PINMUX_FOR_SPI1_MODE_5},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT2,                   PINMUX_FOR_KEY_READ0_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT6,                   PINMUX_FOR_KEY_READ1_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT10,                  PINMUX_FOR_KEY_READ2_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT14,                  PINMUX_FOR_KEY_READ3_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT2,                   PINMUX_FOR_KEY_READ4_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT6,                   PINMUX_FOR_KEY_READ5_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT10,                  PINMUX_FOR_KEY_READ6_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT2,                   PINMUX_FOR_KEY_SCAN0_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT6,                   PINMUX_FOR_KEY_SCAN1_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT10,                  PINMUX_FOR_KEY_SCAN2_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT14,                  PINMUX_FOR_KEY_SCAN3_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT2,                   PINMUX_FOR_KEY_SCAN4_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT6,                   PINMUX_FOR_KEY_SCAN5_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT10,                  PINMUX_FOR_KEY_SCAN6_MODE_4},
    {PAD_KEY3,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY3,         PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT14|BIT13|BIT12,      PINMUX_FOR_PWM3_MODE_7},
    {PAD_KEY3,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY3,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key4_tbl[] =
{
    {PAD_KEY4,         PADGPIO_BANK,     REG_KEY4_GPIO_MODE,             REG_KEY4_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY4,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY4,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY4,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY4,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT6|BIT5|BIT4,         PINMUX_FOR_PSPI1_CS2_MODE_7},
    {PAD_KEY4,         PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT10|BIT9|BIT8,        PINMUX_FOR_PSPI1_TE_MODE_7},
    {PAD_KEY4,         PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT13|BIT12,            PINMUX_FOR_SPI1_CS2_MODE_3},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT2|BIT0,              PINMUX_FOR_KEY_READ0_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT6|BIT4,              PINMUX_FOR_KEY_READ1_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT10|BIT8,             PINMUX_FOR_KEY_READ2_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT14|BIT12,            PINMUX_FOR_KEY_READ3_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT2|BIT0,              PINMUX_FOR_KEY_READ4_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT6|BIT4,              PINMUX_FOR_KEY_READ5_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT10|BIT8,             PINMUX_FOR_KEY_READ6_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT2|BIT0,              PINMUX_FOR_KEY_SCAN0_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT6|BIT4,              PINMUX_FOR_KEY_SCAN1_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT10|BIT8,             PINMUX_FOR_KEY_SCAN2_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT14|BIT12,            PINMUX_FOR_KEY_SCAN3_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT2|BIT0,              PINMUX_FOR_KEY_SCAN4_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT6|BIT4,              PINMUX_FOR_KEY_SCAN5_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT10|BIT8,             PINMUX_FOR_KEY_SCAN6_MODE_5},
    {PAD_KEY4,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY4,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY4,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key5_tbl[] =
{
    {PAD_KEY5,         PADGPIO_BANK,     REG_KEY5_GPIO_MODE,             REG_KEY5_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY5,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY5,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY5,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY5,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT2|BIT1,              PINMUX_FOR_KEY_READ0_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT6|BIT5,              PINMUX_FOR_KEY_READ1_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT10|BIT9,             PINMUX_FOR_KEY_READ2_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT14|BIT13,            PINMUX_FOR_KEY_READ3_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT2|BIT1,              PINMUX_FOR_KEY_READ4_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT6|BIT5,              PINMUX_FOR_KEY_READ5_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT10|BIT9,             PINMUX_FOR_KEY_READ6_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT2|BIT1,              PINMUX_FOR_KEY_SCAN0_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT6|BIT5,              PINMUX_FOR_KEY_SCAN1_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT10|BIT9,             PINMUX_FOR_KEY_SCAN2_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT14|BIT13,            PINMUX_FOR_KEY_SCAN3_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT2|BIT1,              PINMUX_FOR_KEY_SCAN4_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT6|BIT5,              PINMUX_FOR_KEY_SCAN5_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT10|BIT9,             PINMUX_FOR_KEY_SCAN6_MODE_6},
    {PAD_KEY5,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY5,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY5,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key6_tbl[] =
{
    {PAD_KEY6,         PADGPIO_BANK,     REG_KEY6_GPIO_MODE,             REG_KEY6_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY6,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY6,         PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_I2C0_MODE_10},
    {PAD_KEY6,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY6,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY6,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT2|BIT1|BIT0,         PINMUX_FOR_KEY_READ0_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT6|BIT5|BIT4,         PINMUX_FOR_KEY_READ1_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT10|BIT9|BIT8,        PINMUX_FOR_KEY_READ2_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT14|BIT13|BIT12,      PINMUX_FOR_KEY_READ3_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT2|BIT1|BIT0,         PINMUX_FOR_KEY_READ4_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT6|BIT5|BIT4,         PINMUX_FOR_KEY_READ5_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT10|BIT9|BIT8,        PINMUX_FOR_KEY_READ6_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT2|BIT1|BIT0,         PINMUX_FOR_KEY_SCAN0_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT6|BIT5|BIT4,         PINMUX_FOR_KEY_SCAN1_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT10|BIT9|BIT8,        PINMUX_FOR_KEY_SCAN2_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT14|BIT13|BIT12,      PINMUX_FOR_KEY_SCAN3_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT2|BIT1|BIT0,         PINMUX_FOR_KEY_SCAN4_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT6|BIT5|BIT4,         PINMUX_FOR_KEY_SCAN5_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT10|BIT9|BIT8,        PINMUX_FOR_KEY_SCAN6_MODE_7},
    {PAD_KEY6,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY6,         PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT1,                   PINMUX_FOR_UART0_MODE_2},
    {PAD_KEY6,         PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT5,              PINMUX_FOR_UART1_MODE_6},
    {PAD_KEY6,         PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_DMIC_MODE_3},
    {PAD_KEY6,         PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6,                   PINMUX_FOR_I2S_RX_MODE_4},
    {PAD_KEY6,         PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14,                  PINMUX_FOR_I2S_RXTX_MODE_4},
    {PAD_KEY6,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY6,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key7_tbl[] =
{
    {PAD_KEY7,         PADGPIO_BANK,     REG_KEY7_GPIO_MODE,             REG_KEY7_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY7,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY7,         PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_I2C0_MODE_10},
    {PAD_KEY7,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY7,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT8,                   PINMUX_FOR_TTL24_MODE_1},
    {PAD_KEY7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10,                  PINMUX_FOR_TTL24_MODE_4},
    {PAD_KEY7,         PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_TTL24_MODE_5},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3,                   PINMUX_FOR_KEY_READ0_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7,                   PINMUX_FOR_KEY_READ1_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11,                  PINMUX_FOR_KEY_READ2_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15,                  PINMUX_FOR_KEY_READ3_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3,                   PINMUX_FOR_KEY_READ4_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7,                   PINMUX_FOR_KEY_READ5_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11,                  PINMUX_FOR_KEY_READ6_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3,                   PINMUX_FOR_KEY_SCAN0_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7,                   PINMUX_FOR_KEY_SCAN1_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11,                  PINMUX_FOR_KEY_SCAN2_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15,                  PINMUX_FOR_KEY_SCAN3_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3,                   PINMUX_FOR_KEY_SCAN4_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7,                   PINMUX_FOR_KEY_SCAN5_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11,                  PINMUX_FOR_KEY_SCAN6_MODE_8},
    {PAD_KEY7,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY7,         PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT1,                   PINMUX_FOR_UART0_MODE_2},
    {PAD_KEY7,         PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT5,              PINMUX_FOR_UART1_MODE_6},
    {PAD_KEY7,         PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_DMIC_MODE_3},
    {PAD_KEY7,         PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_DMIC_MODE_10},
    {PAD_KEY7,         PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6,                   PINMUX_FOR_I2S_RX_MODE_4},
    {PAD_KEY7,         PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14,                  PINMUX_FOR_I2S_RXTX_MODE_4},
    {PAD_KEY7,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY7,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1,                   PINMUX_FOR_BT1120_MODE_2},
};
const ST_PadMuxInfo key8_tbl[] =
{
    {PAD_KEY8,         PADGPIO_BANK,     REG_KEY8_GPIO_MODE,             REG_KEY8_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY8,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY8,         PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7,                   PINMUX_FOR_I2C1_MODE_8},
    {PAD_KEY8,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT0,              PINMUX_FOR_KEY_READ0_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT4,              PINMUX_FOR_KEY_READ1_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT8,             PINMUX_FOR_KEY_READ2_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT12,            PINMUX_FOR_KEY_READ3_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT0,              PINMUX_FOR_KEY_READ4_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT4,              PINMUX_FOR_KEY_READ5_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT8,             PINMUX_FOR_KEY_READ6_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT0,              PINMUX_FOR_KEY_SCAN0_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT4,              PINMUX_FOR_KEY_SCAN1_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT8,             PINMUX_FOR_KEY_SCAN2_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT12,            PINMUX_FOR_KEY_SCAN3_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT0,              PINMUX_FOR_KEY_SCAN4_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT4,              PINMUX_FOR_KEY_SCAN5_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT8,             PINMUX_FOR_KEY_SCAN6_MODE_9},
    {PAD_KEY8,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY8,         PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10,                  PINMUX_FOR_UART2_MODE_4},
    {PAD_KEY8,         PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT1|BIT0,              PINMUX_FOR_DMIC_MODE_3},
    {PAD_KEY8,         PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_DMIC_MODE_10},
    {PAD_KEY8,         PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6,                   PINMUX_FOR_I2S_RX_MODE_4},
    {PAD_KEY8,         PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14,                  PINMUX_FOR_I2S_RXTX_MODE_4},
    {PAD_KEY8,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY8,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo key9_tbl[] =
{
    {PAD_KEY9,         PADGPIO_BANK,     REG_KEY9_GPIO_MODE,             REG_KEY9_GPIO_MODE_MASK,            BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY9,         CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY9,         PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7,                   PINMUX_FOR_I2C1_MODE_8},
    {PAD_KEY9,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_ETH0_MODE_9},
    {PAD_KEY9,         PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_ETH0_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT7,                   PINMUX_FOR_PSPI1_CS2_MODE_8},
    {PAD_KEY9,         PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT11,                  PINMUX_FOR_PSPI1_TE_MODE_8},
    {PAD_KEY9,         PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT14,                  PINMUX_FOR_SPI1_CS2_MODE_4},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT1,              PINMUX_FOR_KEY_READ0_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT5,              PINMUX_FOR_KEY_READ1_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT9,             PINMUX_FOR_KEY_READ2_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT13,            PINMUX_FOR_KEY_READ3_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT1,              PINMUX_FOR_KEY_READ4_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT5,              PINMUX_FOR_KEY_READ5_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT9,             PINMUX_FOR_KEY_READ6_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT1,              PINMUX_FOR_KEY_SCAN0_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT5,              PINMUX_FOR_KEY_SCAN1_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT9,             PINMUX_FOR_KEY_SCAN2_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT13,            PINMUX_FOR_KEY_SCAN3_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT1,              PINMUX_FOR_KEY_SCAN4_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT5,              PINMUX_FOR_KEY_SCAN5_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT9,             PINMUX_FOR_KEY_SCAN6_MODE_10},
    {PAD_KEY9,         PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY9,         PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10,                  PINMUX_FOR_UART2_MODE_4},
    {PAD_KEY9,         PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10,                  PINMUX_FOR_I2S_TX_MODE_4},
    {PAD_KEY9,         PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14,                  PINMUX_FOR_I2S_RXTX_MODE_4},
    {PAD_KEY9,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY9,         PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo key10_tbl[] =
{
    {PAD_KEY10,        PADGPIO_BANK,     REG_KEY10_GPIO_MODE,            REG_KEY10_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY10,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY10,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2,                   PINMUX_FOR_SPI0_MODE_4},
    {PAD_KEY10,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1|BIT0,         PINMUX_FOR_PSPI1_PL_MODE_7},
    {PAD_KEY10,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9,             PINMUX_FOR_SPI1_MODE_6},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT1|BIT0,         PINMUX_FOR_KEY_READ0_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT5|BIT4,         PINMUX_FOR_KEY_READ1_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT9|BIT8,        PINMUX_FOR_KEY_READ2_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT13|BIT12,      PINMUX_FOR_KEY_READ3_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT1|BIT0,         PINMUX_FOR_KEY_READ4_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT5|BIT4,         PINMUX_FOR_KEY_READ5_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT9|BIT8,        PINMUX_FOR_KEY_READ6_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT1|BIT0,         PINMUX_FOR_KEY_SCAN0_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT5|BIT4,         PINMUX_FOR_KEY_SCAN1_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT9|BIT8,        PINMUX_FOR_KEY_SCAN2_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT13|BIT12,      PINMUX_FOR_KEY_SCAN3_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT1|BIT0,         PINMUX_FOR_KEY_SCAN4_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT5|BIT4,         PINMUX_FOR_KEY_SCAN5_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT9|BIT8,        PINMUX_FOR_KEY_SCAN6_MODE_11},
    {PAD_KEY10,        PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY10,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10,                  PINMUX_FOR_FUART_MODE_4},
    {PAD_KEY10,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT9,             PINMUX_FOR_FUART_MODE_10},
    {PAD_KEY10,        PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT3,                   PINMUX_FOR_PWM0_MODE_8},
    {PAD_KEY10,        PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT2|BIT0,              PINMUX_FOR_I2S_MCK_MODE_5},
    {PAD_KEY10,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY10,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo key11_tbl[] =
{
    {PAD_KEY11,        PADGPIO_BANK,     REG_KEY11_GPIO_MODE,            REG_KEY11_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY11,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY11,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2,                   PINMUX_FOR_SPI0_MODE_4},
    {PAD_KEY11,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1|BIT0,         PINMUX_FOR_PSPI1_PL_MODE_7},
    {PAD_KEY11,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9,             PINMUX_FOR_SPI1_MODE_6},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT2,              PINMUX_FOR_KEY_READ0_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT6,              PINMUX_FOR_KEY_READ1_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT10,            PINMUX_FOR_KEY_READ2_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT14,            PINMUX_FOR_KEY_READ3_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT2,              PINMUX_FOR_KEY_READ4_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT6,              PINMUX_FOR_KEY_READ5_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT10,            PINMUX_FOR_KEY_READ6_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT2,              PINMUX_FOR_KEY_SCAN0_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT6,              PINMUX_FOR_KEY_SCAN1_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT10,            PINMUX_FOR_KEY_SCAN2_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT14,            PINMUX_FOR_KEY_SCAN3_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT2,              PINMUX_FOR_KEY_SCAN4_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT6,              PINMUX_FOR_KEY_SCAN5_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT10,            PINMUX_FOR_KEY_SCAN6_MODE_12},
    {PAD_KEY11,        PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY11,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10,                  PINMUX_FOR_FUART_MODE_4},
    {PAD_KEY11,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT9,             PINMUX_FOR_FUART_MODE_10},
    {PAD_KEY11,        PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT7,                   PINMUX_FOR_PWM1_MODE_8},
    {PAD_KEY11,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10,                  PINMUX_FOR_I2S_TX_MODE_4},
    {PAD_KEY11,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY11,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo key12_tbl[] =
{
    {PAD_KEY12,        PADGPIO_BANK,     REG_KEY12_GPIO_MODE,            REG_KEY12_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY12,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY12,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_I2C0_MODE_11},
    {PAD_KEY12,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2,                   PINMUX_FOR_SPI0_MODE_4},
    {PAD_KEY12,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1|BIT0,         PINMUX_FOR_PSPI1_PL_MODE_7},
    {PAD_KEY12,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9,             PINMUX_FOR_SPI1_MODE_6},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT2|BIT0,         PINMUX_FOR_KEY_READ0_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT6|BIT4,         PINMUX_FOR_KEY_READ1_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT10|BIT8,       PINMUX_FOR_KEY_READ2_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT14|BIT12,      PINMUX_FOR_KEY_READ3_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT2|BIT0,         PINMUX_FOR_KEY_READ4_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT6|BIT4,         PINMUX_FOR_KEY_READ5_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT10|BIT8,       PINMUX_FOR_KEY_READ6_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT2|BIT0,         PINMUX_FOR_KEY_SCAN0_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT6|BIT4,         PINMUX_FOR_KEY_SCAN1_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT10|BIT8,       PINMUX_FOR_KEY_SCAN2_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT14|BIT12,      PINMUX_FOR_KEY_SCAN3_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT2|BIT0,         PINMUX_FOR_KEY_SCAN4_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT6|BIT4,         PINMUX_FOR_KEY_SCAN5_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT10|BIT8,       PINMUX_FOR_KEY_SCAN6_MODE_13},
    {PAD_KEY12,        PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY12,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10,                  PINMUX_FOR_FUART_MODE_4},
    {PAD_KEY12,        PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT11,                  PINMUX_FOR_PWM2_MODE_8},
    {PAD_KEY12,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10,                  PINMUX_FOR_I2S_TX_MODE_4},
    {PAD_KEY12,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY12,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo key13_tbl[] =
{
    {PAD_KEY13,        PADGPIO_BANK,     REG_KEY13_GPIO_MODE,            REG_KEY13_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_KEY13,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_KEY13,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_I2C0_MODE_11},
    {PAD_KEY13,        PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT2,                   PINMUX_FOR_SPI0_MODE_4},
    {PAD_KEY13,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT2|BIT1|BIT0,         PINMUX_FOR_PSPI1_PL_MODE_7},
    {PAD_KEY13,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9,             PINMUX_FOR_SPI1_MODE_6},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ0_MODE,             REG_KEY_READ0_MODE_MASK,            BIT3|BIT2|BIT1,         PINMUX_FOR_KEY_READ0_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ1_MODE,             REG_KEY_READ1_MODE_MASK,            BIT7|BIT6|BIT5,         PINMUX_FOR_KEY_READ1_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ2_MODE,             REG_KEY_READ2_MODE_MASK,            BIT11|BIT10|BIT9,       PINMUX_FOR_KEY_READ2_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ3_MODE,             REG_KEY_READ3_MODE_MASK,            BIT15|BIT14|BIT13,      PINMUX_FOR_KEY_READ3_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ4_MODE,             REG_KEY_READ4_MODE_MASK,            BIT3|BIT2|BIT1,         PINMUX_FOR_KEY_READ4_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ5_MODE,             REG_KEY_READ5_MODE_MASK,            BIT7|BIT6|BIT5,         PINMUX_FOR_KEY_READ5_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_READ6_MODE,             REG_KEY_READ6_MODE_MASK,            BIT11|BIT10|BIT9,       PINMUX_FOR_KEY_READ6_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN0_MODE,             REG_KEY_SCAN0_MODE_MASK,            BIT3|BIT2|BIT1,         PINMUX_FOR_KEY_SCAN0_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN1_MODE,             REG_KEY_SCAN1_MODE_MASK,            BIT7|BIT6|BIT5,         PINMUX_FOR_KEY_SCAN1_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN2_MODE,             REG_KEY_SCAN2_MODE_MASK,            BIT11|BIT10|BIT9,       PINMUX_FOR_KEY_SCAN2_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN3_MODE,             REG_KEY_SCAN3_MODE_MASK,            BIT15|BIT14|BIT13,      PINMUX_FOR_KEY_SCAN3_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN4_MODE,             REG_KEY_SCAN4_MODE_MASK,            BIT3|BIT2|BIT1,         PINMUX_FOR_KEY_SCAN4_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN5_MODE,             REG_KEY_SCAN5_MODE_MASK,            BIT7|BIT6|BIT5,         PINMUX_FOR_KEY_SCAN5_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_SCAN6_MODE,             REG_KEY_SCAN6_MODE_MASK,            BIT11|BIT10|BIT9,       PINMUX_FOR_KEY_SCAN6_MODE_14},
    {PAD_KEY13,        PADTOP_BANK,      REG_KEY_FIX_MODE,               REG_KEY_FIX_MODE_MASK,              BIT12,                  PINMUX_FOR_KEY_FIX_MODE_1},
    {PAD_KEY13,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10,                  PINMUX_FOR_FUART_MODE_4},
    {PAD_KEY13,        PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT15,                  PINMUX_FOR_PWM3_MODE_8},
    {PAD_KEY13,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT0,                   PINMUX_FOR_BT1120_MODE_1},
    {PAD_KEY13,        PADTOP_BANK,      REG_BT1120_MODE,                REG_BT1120_MODE_MASK,               BIT1|BIT0,              PINMUX_FOR_BT1120_MODE_3},
};
const ST_PadMuxInfo sd_d1_tbl[] =
{
    {PAD_SD_D1,        PADGPIO_BANK,     REG_SD_D1_GPIO_MODE,            REG_SD_D1_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_D1,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_D1,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_D1,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3,                   PINMUX_FOR_PSPI1_PL_MODE_8},
    {PAD_SD_D1,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9|BIT8,        PINMUX_FOR_SPI1_MODE_7},
    {PAD_SD_D1,        PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_D1,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_FUART_MODE_5},
    {PAD_SD_D1,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT9|BIT8,        PINMUX_FOR_FUART_MODE_11},
};
const ST_PadMuxInfo sd_d0_tbl[] =
{
    {PAD_SD_D0,        PADGPIO_BANK,     REG_SD_D0_GPIO_MODE,            REG_SD_D0_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_D0,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_D0,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_D0,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3,                   PINMUX_FOR_PSPI1_PL_MODE_8},
    {PAD_SD_D0,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9|BIT8,        PINMUX_FOR_SPI1_MODE_7},
    {PAD_SD_D0,        PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_D0,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_FUART_MODE_5},
    {PAD_SD_D0,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT9|BIT8,        PINMUX_FOR_FUART_MODE_11},
};
const ST_PadMuxInfo sd_clk_tbl[] =
{
    {PAD_SD_CLK,       PADGPIO_BANK,     REG_SD_CLK_GPIO_MODE,           REG_SD_CLK_GPIO_MODE_MASK,          BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_CLK,       CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_CLK,       PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_CLK,       PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3,                   PINMUX_FOR_PSPI1_PL_MODE_8},
    {PAD_SD_CLK,       PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9|BIT8,        PINMUX_FOR_SPI1_MODE_7},
    {PAD_SD_CLK,       PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_CLK,       PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_FUART_MODE_5},
};
const ST_PadMuxInfo sd_cmd_tbl[] =
{
    {PAD_SD_CMD,       PADGPIO_BANK,     REG_SD_CMD_GPIO_MODE,           REG_SD_CMD_GPIO_MODE_MASK,          BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_CMD,       CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3,                   PINMUX_FOR_PSPI1_PL_MODE_8},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT10|BIT9|BIT8,        PINMUX_FOR_SPI1_MODE_7},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_FUART_MODE_5},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT3|BIT0,              PINMUX_FOR_PWM0_MODE_9},
    {PAD_SD_CMD,       PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT2|BIT1,              PINMUX_FOR_I2S_MCK_MODE_6},
};
const ST_PadMuxInfo sd_d3_tbl[] =
{
    {PAD_SD_D3,        PADGPIO_BANK,     REG_SD_D3_GPIO_MODE,            REG_SD_D3_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_D3,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_D3,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_D3,        PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT7|BIT4,              PINMUX_FOR_PSPI1_CS2_MODE_9},
    {PAD_SD_D3,        PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT11|BIT8,             PINMUX_FOR_PSPI1_TE_MODE_9},
    {PAD_SD_D3,        PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT14|BIT12,            PINMUX_FOR_SPI1_CS2_MODE_5},
    {PAD_SD_D3,        PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_D3,        PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_UART0_MODE_3},
    {PAD_SD_D3,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT5|BIT4,         PINMUX_FOR_UART1_MODE_7},
    {PAD_SD_D3,        PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT7|BIT4,              PINMUX_FOR_PWM1_MODE_9},
    {PAD_SD_D3,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT4,              PINMUX_FOR_I2S_RX_MODE_5},
    {PAD_SD_D3,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT8,             PINMUX_FOR_I2S_TX_MODE_5},
    {PAD_SD_D3,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_5},
};
const ST_PadMuxInfo sd_d2_tbl[] =
{
    {PAD_SD_D2,        PADGPIO_BANK,     REG_SD_D2_GPIO_MODE,            REG_SD_D2_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_D2,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_SD_D2,        PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_D2,        PADTOP_BANK,      REG_SDIO_MODE,                  REG_SDIO_MODE_MASK,                 BIT8,                   PINMUX_FOR_SDIO_MODE_1},
    {PAD_SD_D2,        PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT1|BIT0,              PINMUX_FOR_UART0_MODE_3},
    {PAD_SD_D2,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT6|BIT5|BIT4,         PINMUX_FOR_UART1_MODE_7},
    {PAD_SD_D2,        PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT11|BIT8,             PINMUX_FOR_PWM2_MODE_9},
    {PAD_SD_D2,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2,                   PINMUX_FOR_DMIC_MODE_4},
    {PAD_SD_D2,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT4,              PINMUX_FOR_I2S_RX_MODE_5},
    {PAD_SD_D2,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT8,             PINMUX_FOR_I2S_TX_MODE_5},
    {PAD_SD_D2,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_5},
};
const ST_PadMuxInfo sd_gpio0_tbl[] =
{
    {PAD_SD_GPIO0,     PADGPIO_BANK,     REG_SD_GPIO0_GPIO_MODE,         REG_SD_GPIO0_GPIO_MODE_MASK,        BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_UART2_MODE_5},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT15|BIT12,            PINMUX_FOR_PWM3_MODE_9},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2,                   PINMUX_FOR_DMIC_MODE_4},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_DMIC_MODE_11},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT4,              PINMUX_FOR_I2S_RX_MODE_5},
    {PAD_SD_GPIO0,     PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_5},
};
const ST_PadMuxInfo sd_gpio1_tbl[] =
{
    {PAD_SD_GPIO1,     PADGPIO_BANK,     REG_SD_GPIO1_GPIO_MODE,         REG_SD_GPIO1_GPIO_MODE_MASK,        BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_TTL24_MODE,                 REG_TTL24_MODE_MASK,                BIT9,                   PINMUX_FOR_TTL24_MODE_2},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT8,             PINMUX_FOR_UART2_MODE_5},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2,                   PINMUX_FOR_DMIC_MODE_4},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_DMIC_MODE_11},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT8,             PINMUX_FOR_I2S_TX_MODE_5},
    {PAD_SD_GPIO1,     PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT12,            PINMUX_FOR_I2S_RXTX_MODE_5},
};
const ST_PadMuxInfo gpio0_tbl[] =
{
    {PAD_GPIO0,        PADGPIO_BANK,     REG_GPIO0_GPIO_MODE,            REG_GPIO0_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO0,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO0,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_GPIO0,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_GPIO0,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO0,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO0,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_DMIC_MODE_5},
    {PAD_GPIO0,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_DMIC_MODE_12},
};
const ST_PadMuxInfo gpio1_tbl[] =
{
    {PAD_GPIO1,        PADGPIO_BANK,     REG_GPIO1_GPIO_MODE,            REG_GPIO1_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO1,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO1,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_GPIO1,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_GPIO1,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_I2C0_MODE_12},
    {PAD_GPIO1,        PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT4,              PINMUX_FOR_I2C1_MODE_9},
    {PAD_GPIO1,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO1,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO1,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT7,                   PINMUX_FOR_UART1_MODE_8},
    {PAD_GPIO1,        PADTOP_BANK,      REG_PWM0_MODE,                  REG_PWM0_MODE_MASK,                 BIT3|BIT1,              PINMUX_FOR_PWM0_MODE_10},
    {PAD_GPIO1,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_DMIC_MODE_5},
    {PAD_GPIO1,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_DMIC_MODE_12},
};
const ST_PadMuxInfo gpio2_tbl[] =
{
    {PAD_GPIO2,        PADGPIO_BANK,     REG_GPIO2_GPIO_MODE,            REG_GPIO2_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO2,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO2,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_GPIO2,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_GPIO2,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_I2C0_MODE_12},
    {PAD_GPIO2,        PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT4,              PINMUX_FOR_I2C1_MODE_9},
    {PAD_GPIO2,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO2,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO2,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT7,                   PINMUX_FOR_UART1_MODE_8},
    {PAD_GPIO2,        PADTOP_BANK,      REG_PWM1_MODE,                  REG_PWM1_MODE_MASK,                 BIT7|BIT5,              PINMUX_FOR_PWM1_MODE_10},
    {PAD_GPIO2,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT0,              PINMUX_FOR_DMIC_MODE_5},
    {PAD_GPIO2,        PADTOP_BANK,      REG_I2S_MCK_MODE,               REG_I2S_MCK_MODE_MASK,              BIT2|BIT1|BIT0,         PINMUX_FOR_I2S_MCK_MODE_7},
};
const ST_PadMuxInfo gpio3_tbl[] =
{
    {PAD_GPIO3,        PADGPIO_BANK,     REG_GPIO3_GPIO_MODE,            REG_GPIO3_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO3,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO3,        CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1|BIT0,              PINMUX_FOR_TEST_IN_MODE_3},
    {PAD_GPIO3,        CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5|BIT4,              PINMUX_FOR_TEST_OUT_MODE_3},
    {PAD_GPIO3,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2|BIT0,         PINMUX_FOR_I2C0_MODE_13},
    {PAD_GPIO3,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO3,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO3,        PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_UART2_MODE_6},
    {PAD_GPIO3,        PADTOP_BANK,      REG_PWM2_MODE,                  REG_PWM2_MODE_MASK,                 BIT11|BIT9,             PINMUX_FOR_PWM2_MODE_10},
    {PAD_GPIO3,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT5,              PINMUX_FOR_I2S_RX_MODE_6},
    {PAD_GPIO3,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT13,            PINMUX_FOR_I2S_RXTX_MODE_6},
};
const ST_PadMuxInfo gpio4_tbl[] =
{
    {PAD_GPIO4,        PADGPIO_BANK,     REG_GPIO4_GPIO_MODE,            REG_GPIO4_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO4,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO4,        PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2|BIT0,         PINMUX_FOR_I2C0_MODE_13},
    {PAD_GPIO4,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO4,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO4,        PADTOP_BANK,      REG_PSPI1_CS2_MODE,             REG_PSPI1_CS2_MODE_MASK,            BIT7|BIT5,              PINMUX_FOR_PSPI1_CS2_MODE_10},
    {PAD_GPIO4,        PADTOP_BANK,      REG_PSPI1_TE_MODE,              REG_PSPI1_TE_MODE_MASK,             BIT11|BIT9,             PINMUX_FOR_PSPI1_TE_MODE_10},
    {PAD_GPIO4,        PADTOP_BANK,      REG_SPI1_CS2_MODE,              REG_SPI1_CS2_MODE_MASK,             BIT14|BIT13,            PINMUX_FOR_SPI1_CS2_MODE_6},
    {PAD_GPIO4,        PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_UART2_MODE_6},
    {PAD_GPIO4,        PADTOP_BANK,      REG_PWM3_MODE,                  REG_PWM3_MODE_MASK,                 BIT15|BIT13,            PINMUX_FOR_PWM3_MODE_10},
    {PAD_GPIO4,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_DMIC_MODE_6},
    {PAD_GPIO4,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT2|BIT0,         PINMUX_FOR_DMIC_MODE_13},
    {PAD_GPIO4,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT5,              PINMUX_FOR_I2S_RX_MODE_6},
    {PAD_GPIO4,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT13,            PINMUX_FOR_I2S_RXTX_MODE_6},
};
const ST_PadMuxInfo gpio5_tbl[] =
{
    {PAD_GPIO5,        PADGPIO_BANK,     REG_GPIO5_GPIO_MODE,            REG_GPIO5_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO5,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO5,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO5,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO5,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_9},
    {PAD_GPIO5,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT11,                  PINMUX_FOR_SPI1_MODE_8},
    {PAD_GPIO5,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_FUART_MODE_6},
    {PAD_GPIO5,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT10,            PINMUX_FOR_FUART_MODE_12},
    {PAD_GPIO5,        PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT2,                   PINMUX_FOR_UART0_MODE_4},
    {PAD_GPIO5,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_DMIC_MODE_6},
    {PAD_GPIO5,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT3|BIT2|BIT0,         PINMUX_FOR_DMIC_MODE_13},
    {PAD_GPIO5,        PADTOP_BANK,      REG_I2S_RX_MODE,                REG_I2S_RX_MODE_MASK,               BIT6|BIT5,              PINMUX_FOR_I2S_RX_MODE_6},
    {PAD_GPIO5,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT13,            PINMUX_FOR_I2S_RXTX_MODE_6},
};
const ST_PadMuxInfo gpio6_tbl[] =
{
    {PAD_GPIO6,        PADGPIO_BANK,     REG_GPIO6_GPIO_MODE,            REG_GPIO6_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO6,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO6,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO6,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO6,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_9},
    {PAD_GPIO6,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT11,                  PINMUX_FOR_SPI1_MODE_8},
    {PAD_GPIO6,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_FUART_MODE_6},
    {PAD_GPIO6,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT11|BIT10,            PINMUX_FOR_FUART_MODE_12},
    {PAD_GPIO6,        PADTOP_BANK,      REG_UART0_MODE,                 REG_UART0_MODE_MASK,                BIT2,                   PINMUX_FOR_UART0_MODE_4},
    {PAD_GPIO6,        PADTOP_BANK,      REG_DMIC_MODE,                  REG_DMIC_MODE_MASK,                 BIT2|BIT1,              PINMUX_FOR_DMIC_MODE_6},
    {PAD_GPIO6,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT9,             PINMUX_FOR_I2S_TX_MODE_6},
    {PAD_GPIO6,        PADTOP_BANK,      REG_I2S_RXTX_MODE,              REG_I2S_RXTX_MODE_MASK,             BIT14|BIT13,            PINMUX_FOR_I2S_RXTX_MODE_6},
};
const ST_PadMuxInfo gpio7_tbl[] =
{
    {PAD_GPIO7,        PADGPIO_BANK,     REG_GPIO7_GPIO_MODE,            REG_GPIO7_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO7,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO7,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO7,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO7,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_9},
    {PAD_GPIO7,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT11,                  PINMUX_FOR_SPI1_MODE_8},
    {PAD_GPIO7,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_FUART_MODE_6},
    {PAD_GPIO7,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT7|BIT4,              PINMUX_FOR_UART1_MODE_9},
    {PAD_GPIO7,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT9,             PINMUX_FOR_I2S_TX_MODE_6},
};
const ST_PadMuxInfo gpio8_tbl[] =
{
    {PAD_GPIO8,        PADGPIO_BANK,     REG_GPIO8_GPIO_MODE,            REG_GPIO8_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO8,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
    {PAD_GPIO8,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT1|BIT0,         PINMUX_FOR_ETH0_MODE_11},
    {PAD_GPIO8,        PADTOP_BANK,      REG_ETH0_MODE,                  REG_ETH0_MODE_MASK,                 BIT3|BIT2,              PINMUX_FOR_ETH0_MODE_12},
    {PAD_GPIO8,        PADTOP_BANK,      REG_PSPI1_PL_MODE,              REG_PSPI1_PL_MODE_MASK,             BIT3|BIT0,              PINMUX_FOR_PSPI1_PL_MODE_9},
    {PAD_GPIO8,        PADTOP_BANK,      REG_SPI1_MODE,                  REG_SPI1_MODE_MASK,                 BIT11,                  PINMUX_FOR_SPI1_MODE_8},
    {PAD_GPIO8,        PADTOP_BANK,      REG_FUART_MODE,                 REG_FUART_MODE_MASK,                BIT10|BIT9,             PINMUX_FOR_FUART_MODE_6},
    {PAD_GPIO8,        PADTOP_BANK,      REG_UART1_MODE,                 REG_UART1_MODE_MASK,                BIT7|BIT4,              PINMUX_FOR_UART1_MODE_9},
    {PAD_GPIO8,        PADTOP_BANK,      REG_I2S_TX_MODE,                REG_I2S_TX_MODE_MASK,               BIT10|BIT9,             PINMUX_FOR_I2S_TX_MODE_6},
};
const ST_PadMuxInfo gpio9_tbl[] =
{
    {PAD_GPIO9,        PADGPIO_BANK,     REG_GPIO9_GPIO_MODE,            REG_GPIO9_GPIO_MODE_MASK,           BIT3,                   PINMUX_FOR_GPIO_MODE},
    {PAD_GPIO9,        CHIPTOP_BANK,     REG_ALL_PAD_IN,                 REG_ALL_PAD_IN_MASK,                BIT15,                  PINMUX_FOR_ALL_PAD_IN_1},
};
const ST_PadMuxInfo pm_sd_cdz_tbl[] =
{
    {PAD_PM_SD_CDZ,    PADTOP_BANK,      REG_SD_CDZ_MODE,                REG_SD_CDZ_MODE_MASK,               BIT0,                   PINMUX_FOR_SD_CDZ_MODE_1},
    {PAD_PM_SD_CDZ,    PMSLEEP_BANK,     REG_PM_PWM3_MODE,               REG_PM_PWM3_MODE_MASK,              BIT8,                   PINMUX_FOR_PM_PWM3_MODE_1},
    {PAD_PM_SD_CDZ,    PMSLEEP_BANK,     REG_PM_SD_CDZ_MODE,             REG_PM_SD_CDZ_MODE_MASK,            BIT14,                  PINMUX_FOR_PM_SD_CDZ_MODE_1},
    {PAD_PM_SD_CDZ,    PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_0,          REG_PM_PAD_EXT_MODE_0_MASK,         BIT0,                   PINMUX_FOR_PM_PAD_EXT_MODE_0_1},
};
const ST_PadMuxInfo pm_spi_cz_tbl[] =
{
    {PAD_PM_SPI_CZ,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_PM_SPI_CZ,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_PM_SPI_CZ,    PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT0,                   PINMUX_FOR_SPI0_MODE_1},
    {PAD_PM_SPI_CZ,    PMSLEEP_BANK,     REG_PM_SPICSZ1_GPIO,            REG_PM_SPICSZ1_GPIO_MASK,           0,                      PINMUX_FOR_PM_SPICSZ1_GPIO_0},
    {PAD_PM_SPI_CZ,    PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_3,          REG_PM_PAD_EXT_MODE_3_MASK,         BIT3,                   PINMUX_FOR_PM_PAD_EXT_MODE_3_1},
};
const ST_PadMuxInfo pm_spi_ck_tbl[] =
{
    {PAD_PM_SPI_CK,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_PM_SPI_CK,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_PM_SPI_CK,    PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT0,                   PINMUX_FOR_SPI0_MODE_1},
    {PAD_PM_SPI_CK,    PMSLEEP_BANK,     REG_PM_SPI_GPIO,                REG_PM_SPI_GPIO_MASK,               0,                      PINMUX_FOR_PM_SPI_GPIO_0},
    {PAD_PM_SPI_CK,    PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_7,          REG_PM_PAD_EXT_MODE_7_MASK,         BIT7,                   PINMUX_FOR_PM_PAD_EXT_MODE_7_1},
};
const ST_PadMuxInfo pm_spi_di_tbl[] =
{
    {PAD_PM_SPI_DI,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_PM_SPI_DI,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_PM_SPI_DI,    PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT0,                   PINMUX_FOR_SPI0_MODE_1},
    {PAD_PM_SPI_DI,    PMSLEEP_BANK,     REG_PM_SPI_GPIO,                REG_PM_SPI_GPIO_MASK,               0,                      PINMUX_FOR_PM_SPI_GPIO_0},
    {PAD_PM_SPI_DI,    PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_4,          REG_PM_PAD_EXT_MODE_4_MASK,         BIT4,                   PINMUX_FOR_PM_PAD_EXT_MODE_4_1},
};
const ST_PadMuxInfo pm_spi_do_tbl[] =
{
    {PAD_PM_SPI_DO,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_PM_SPI_DO,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_PM_SPI_DO,    PADTOP_BANK,      REG_SPI0_MODE,                  REG_SPI0_MODE_MASK,                 BIT0,                   PINMUX_FOR_SPI0_MODE_1},
    {PAD_PM_SPI_DO,    PMSLEEP_BANK,     REG_PM_SPI_GPIO,                REG_PM_SPI_GPIO_MASK,               0,                      PINMUX_FOR_PM_SPI_GPIO_0},
    {PAD_PM_SPI_DO,    PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_6,          REG_PM_PAD_EXT_MODE_6_MASK,         BIT6,                   PINMUX_FOR_PM_PAD_EXT_MODE_6_1},
};
const ST_PadMuxInfo pm_spi_wpz_tbl[] =
{
    {PAD_PM_SPI_WPZ,   CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              BIT1,                   PINMUX_FOR_TEST_IN_MODE_2},
    {PAD_PM_SPI_WPZ,   CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             BIT5,                   PINMUX_FOR_TEST_OUT_MODE_2},
    {PAD_PM_SPI_WPZ,   PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT1,                   PINMUX_FOR_I2C0_MODE_2},
    {PAD_PM_SPI_WPZ,   PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT4,                   PINMUX_FOR_I2C1_MODE_1},
    {PAD_PM_SPI_WPZ,   PMSLEEP_BANK,     REG_PM_SPIWPN_GPIO,             REG_PM_SPIWPN_GPIO_MASK,            0,                      PINMUX_FOR_PM_SPIWPN_GPIO_0},
    {PAD_PM_SPI_WPZ,   PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_5,          REG_PM_PAD_EXT_MODE_5_MASK,         BIT5,                   PINMUX_FOR_PM_PAD_EXT_MODE_5_1},
};
const ST_PadMuxInfo pm_spi_hld_tbl[] =
{
    {PAD_PM_SPI_HLD,   PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT1,                   PINMUX_FOR_I2C0_MODE_2},
    {PAD_PM_SPI_HLD,   PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT4,                   PINMUX_FOR_I2C1_MODE_1},
    {PAD_PM_SPI_HLD,   PMSLEEP_BANK,     REG_PM_SPIHOLDN_MODE,           REG_PM_SPIHOLDN_MODE_MASK,          0,                      PINMUX_FOR_PM_SPIHOLDN_MODE_0},
    {PAD_PM_SPI_HLD,   PMSLEEP_BANK,     REG_PM_PWM2_MODE,               REG_PM_PWM2_MODE_MASK,              BIT6,                   PINMUX_FOR_PM_PWM2_MODE_1},
    {PAD_PM_SPI_HLD,   PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_8,          REG_PM_PAD_EXT_MODE_8_MASK,         BIT8,                   PINMUX_FOR_PM_PAD_EXT_MODE_8_1},
};
const ST_PadMuxInfo pm_led0_tbl[] =
{
    {PAD_PM_LED0,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2|BIT1,         PINMUX_FOR_I2C0_MODE_14},
    {PAD_PM_LED0,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5,              PINMUX_FOR_I2C1_MODE_10},
    {PAD_PM_LED0,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT9|BIT8,        PINMUX_FOR_UART2_MODE_7},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_SPICSZ2_GPIO,            REG_PM_SPICSZ2_GPIO_MASK,           0,                      PINMUX_FOR_PM_SPICSZ2_GPIO_0},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_PWM0_MODE,               REG_PM_PWM0_MODE_MASK,              BIT0,                   PINMUX_FOR_PM_PWM0_MODE_1},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_UART1_MODE,              REG_PM_UART1_MODE_MASK,             BIT8,                   PINMUX_FOR_PM_UART1_MODE_1},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_VID_MODE,                REG_PM_VID_MODE_MASK,               BIT12,                  PINMUX_FOR_PM_VID_MODE_1},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_VID_MODE,                REG_PM_VID_MODE_MASK,               BIT13,                  PINMUX_FOR_PM_VID_MODE_2},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_LED_MODE,                REG_PM_LED_MODE_MASK,               BIT4,                   PINMUX_FOR_PM_LED_MODE_1},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_LED_MODE,                REG_PM_LED_MODE_MASK,               BIT5,                   PINMUX_FOR_PM_LED_MODE_2},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_LED_MODE,                REG_PM_LED_MODE_MASK,               BIT5|BIT4,              PINMUX_FOR_PM_LED_MODE_3},
    {PAD_PM_LED0,      PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_9,          REG_PM_PAD_EXT_MODE_9_MASK,         BIT9,                   PINMUX_FOR_PM_PAD_EXT_MODE_9_1},
};
const ST_PadMuxInfo pm_led1_tbl[] =
{
    {PAD_PM_LED1,      PADTOP_BANK,      REG_I2C0_MODE,                  REG_I2C0_MODE_MASK,                 BIT3|BIT2|BIT1,         PINMUX_FOR_I2C0_MODE_14},
    {PAD_PM_LED1,      PADTOP_BANK,      REG_I2C1_MODE,                  REG_I2C1_MODE_MASK,                 BIT7|BIT5,              PINMUX_FOR_I2C1_MODE_10},
    {PAD_PM_LED1,      PADTOP_BANK,      REG_UART2_MODE,                 REG_UART2_MODE_MASK,                BIT10|BIT9|BIT8,        PINMUX_FOR_UART2_MODE_7},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_PWM1_MODE,               REG_PM_PWM1_MODE_MASK,              BIT2,                   PINMUX_FOR_PM_PWM1_MODE_1},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_UART1_MODE,              REG_PM_UART1_MODE_MASK,             BIT8,                   PINMUX_FOR_PM_UART1_MODE_1},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_VID_MODE,                REG_PM_VID_MODE_MASK,               BIT12,                  PINMUX_FOR_PM_VID_MODE_1},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_VID_MODE,                REG_PM_VID_MODE_MASK,               BIT13|BIT12,            PINMUX_FOR_PM_VID_MODE_3},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_LED_MODE,                REG_PM_LED_MODE_MASK,               BIT4,                   PINMUX_FOR_PM_LED_MODE_1},
    {PAD_PM_LED1,      PMSLEEP_BANK,     REG_PM_PAD_EXT_MODE_10,         REG_PM_PAD_EXT_MODE_10_MASK,        BIT10,                  PINMUX_FOR_PM_PAD_EXT_MODE_10_1},
};
const ST_PadMuxInfo eth_rn_tbl[] =
{
    {PAD_ETH_RN,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           BIT14|BIT15,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           BIT10|BIT11,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           BIT4,                   PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             BIT14,                  PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RN,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               BIT0,                   PINMUX_FOR_GPIO_MODE},

    {PAD_ETH_RN,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RN,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
};
const ST_PadMuxInfo eth_rp_tbl[] =
{
    {PAD_ETH_RP,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           BIT14|BIT15,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           BIT10|BIT11,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           BIT4,                   PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             BIT14,                  PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_RP,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               BIT1,                   PINMUX_FOR_GPIO_MODE},

    {PAD_ETH_RP,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_RP,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
};
const ST_PadMuxInfo eth_tn_tbl[] =
{
    {PAD_ETH_TN,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           BIT14|BIT15,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           BIT10|BIT11,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           BIT4,                   PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             BIT15,                  PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TN,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               BIT2,                   PINMUX_FOR_GPIO_MODE},

    {PAD_ETH_TN,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TN,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
};
const ST_PadMuxInfo eth_tp_tbl[] =
{
    {PAD_ETH_TP,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           BIT14|BIT15,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           BIT10|BIT11,            PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           BIT4,                   PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             BIT15,                  PINMUX_FOR_GPIO_MODE},
    {PAD_ETH_TP,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               BIT3,                   PINMUX_FOR_GPIO_MODE},

    {PAD_ETH_TP,       ARBPHY_BANK,      REG_MISC_ETH_0_MODE,            REG_MISC_ETH_0_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       ARBPHY_BANK,      REG_MISC_ETH_1_MODE,            REG_MISC_ETH_1_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       ETH_BANK,         REG_MISC_ETH_2_MODE,            REG_MISC_ETH_2_MODE_MASK,           0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       ALBANY1_BANK,     REG_ATOP_RX_INOFF,              REG_ATOP_RX_INOFF_MASK,             0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
    {PAD_ETH_TP,       ALBANY2_BANK,     REG_ETH_GPIO_EN,                REG_ETH_GPIO_EN_MASK,               0,                      PINMUX_FOR_ETH_GPIO_EN_MODE},
};
const ST_PadMuxInfo sar_gpio0_tbl[] =
{
    {PAD_SAR_GPIO0,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO0,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO0,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH0_AISEL,                  0,                      PINMUX_FOR_GPIO_MODE},

    {PAD_SAR_GPIO0,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO0,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO0,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH0_AISEL,                  BIT0,                   PINMUX_FOR_SAR_MODE},
};
const ST_PadMuxInfo sar_gpio1_tbl[] =
{
    {PAD_SAR_GPIO1,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO1,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO1,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH1_AISEL,                  0,                      PINMUX_FOR_GPIO_MODE},

    {PAD_SAR_GPIO1,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO1,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO1,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH1_AISEL,                  BIT1,                   PINMUX_FOR_SAR_MODE},
};
const ST_PadMuxInfo sar_gpio2_tbl[] =
{
    {PAD_SAR_GPIO2,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO2,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO2,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH2_AISEL,                  0,                      PINMUX_FOR_GPIO_MODE},

    {PAD_SAR_GPIO2,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO2,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO2,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH2_AISEL,                  BIT2,                   PINMUX_FOR_SAR_MODE},
};
const ST_PadMuxInfo sar_gpio3_tbl[] =
{
    {PAD_SAR_GPIO3,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO3,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_GPIO_MODE},
    {PAD_SAR_GPIO3,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH3_AISEL,                  0,                      PINMUX_FOR_GPIO_MODE},

    {PAD_SAR_GPIO3,    CHIPTOP_BANK,     REG_TEST_IN_MODE,               REG_TEST_IN_MODE_MASK,              0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO3,    CHIPTOP_BANK,     REG_TEST_OUT_MODE,              REG_TEST_OUT_MODE_MASK,             0,                      PINMUX_FOR_SAR_MODE},
    {PAD_SAR_GPIO3,    PM_SAR_BANK,      REG_SAR_AISEL_8BIT,             REG_SAR_CH3_AISEL,                  BIT3,                   PINMUX_FOR_SAR_MODE},
};

const ST_PadMuxEntry m_stPadMuxEntry[] =
{
    {(sizeof(sr_io00_tbl)/sizeof(ST_PadMuxInfo)),             sr_io00_tbl},
    {(sizeof(sr_io01_tbl)/sizeof(ST_PadMuxInfo)),             sr_io01_tbl},
    {(sizeof(sr_io02_tbl)/sizeof(ST_PadMuxInfo)),             sr_io02_tbl},
    {(sizeof(sr_io03_tbl)/sizeof(ST_PadMuxInfo)),             sr_io03_tbl},
    {(sizeof(sr_io04_tbl)/sizeof(ST_PadMuxInfo)),             sr_io04_tbl},
    {(sizeof(sr_io05_tbl)/sizeof(ST_PadMuxInfo)),             sr_io05_tbl},
    {(sizeof(sr_io06_tbl)/sizeof(ST_PadMuxInfo)),             sr_io06_tbl},
    {(sizeof(sr_io07_tbl)/sizeof(ST_PadMuxInfo)),             sr_io07_tbl},
    {(sizeof(sr_io08_tbl)/sizeof(ST_PadMuxInfo)),             sr_io08_tbl},
    {(sizeof(sr_io09_tbl)/sizeof(ST_PadMuxInfo)),             sr_io09_tbl},
    {(sizeof(sr_io10_tbl)/sizeof(ST_PadMuxInfo)),             sr_io10_tbl},
    {(sizeof(sr_io11_tbl)/sizeof(ST_PadMuxInfo)),             sr_io11_tbl},
    {(sizeof(sr_io12_tbl)/sizeof(ST_PadMuxInfo)),             sr_io12_tbl},
    {(sizeof(sr_io13_tbl)/sizeof(ST_PadMuxInfo)),             sr_io13_tbl},
    {(sizeof(sr_io14_tbl)/sizeof(ST_PadMuxInfo)),             sr_io14_tbl},
    {(sizeof(sr_io15_tbl)/sizeof(ST_PadMuxInfo)),             sr_io15_tbl},
    {(sizeof(sr_io16_tbl)/sizeof(ST_PadMuxInfo)),             sr_io16_tbl},
    {(sizeof(ttl0_tbl)/sizeof(ST_PadMuxInfo)),                ttl0_tbl},
    {(sizeof(ttl1_tbl)/sizeof(ST_PadMuxInfo)),                ttl1_tbl},
    {(sizeof(ttl2_tbl)/sizeof(ST_PadMuxInfo)),                ttl2_tbl},
    {(sizeof(ttl3_tbl)/sizeof(ST_PadMuxInfo)),                ttl3_tbl},
    {(sizeof(ttl4_tbl)/sizeof(ST_PadMuxInfo)),                ttl4_tbl},
    {(sizeof(ttl5_tbl)/sizeof(ST_PadMuxInfo)),                ttl5_tbl},
    {(sizeof(ttl6_tbl)/sizeof(ST_PadMuxInfo)),                ttl6_tbl},
    {(sizeof(ttl7_tbl)/sizeof(ST_PadMuxInfo)),                ttl7_tbl},
    {(sizeof(ttl8_tbl)/sizeof(ST_PadMuxInfo)),                ttl8_tbl},
    {(sizeof(ttl9_tbl)/sizeof(ST_PadMuxInfo)),                ttl9_tbl},
    {(sizeof(ttl10_tbl)/sizeof(ST_PadMuxInfo)),               ttl10_tbl},
    {(sizeof(ttl11_tbl)/sizeof(ST_PadMuxInfo)),               ttl11_tbl},
    {(sizeof(ttl12_tbl)/sizeof(ST_PadMuxInfo)),               ttl12_tbl},
    {(sizeof(ttl13_tbl)/sizeof(ST_PadMuxInfo)),               ttl13_tbl},
    {(sizeof(ttl14_tbl)/sizeof(ST_PadMuxInfo)),               ttl14_tbl},
    {(sizeof(ttl15_tbl)/sizeof(ST_PadMuxInfo)),               ttl15_tbl},
    {(sizeof(ttl16_tbl)/sizeof(ST_PadMuxInfo)),               ttl16_tbl},
    {(sizeof(ttl17_tbl)/sizeof(ST_PadMuxInfo)),               ttl17_tbl},
    {(sizeof(ttl18_tbl)/sizeof(ST_PadMuxInfo)),               ttl18_tbl},
    {(sizeof(ttl19_tbl)/sizeof(ST_PadMuxInfo)),               ttl19_tbl},
    {(sizeof(ttl20_tbl)/sizeof(ST_PadMuxInfo)),               ttl20_tbl},
    {(sizeof(ttl21_tbl)/sizeof(ST_PadMuxInfo)),               ttl21_tbl},
    {(sizeof(key0_tbl)/sizeof(ST_PadMuxInfo)),                key0_tbl},
    {(sizeof(key1_tbl)/sizeof(ST_PadMuxInfo)),                key1_tbl},
    {(sizeof(key2_tbl)/sizeof(ST_PadMuxInfo)),                key2_tbl},
    {(sizeof(key3_tbl)/sizeof(ST_PadMuxInfo)),                key3_tbl},
    {(sizeof(key4_tbl)/sizeof(ST_PadMuxInfo)),                key4_tbl},
    {(sizeof(key5_tbl)/sizeof(ST_PadMuxInfo)),                key5_tbl},
    {(sizeof(key6_tbl)/sizeof(ST_PadMuxInfo)),                key6_tbl},
    {(sizeof(key7_tbl)/sizeof(ST_PadMuxInfo)),                key7_tbl},
    {(sizeof(key8_tbl)/sizeof(ST_PadMuxInfo)),                key8_tbl},
    {(sizeof(key9_tbl)/sizeof(ST_PadMuxInfo)),                key9_tbl},
    {(sizeof(key10_tbl)/sizeof(ST_PadMuxInfo)),               key10_tbl},
    {(sizeof(key11_tbl)/sizeof(ST_PadMuxInfo)),               key11_tbl},
    {(sizeof(key12_tbl)/sizeof(ST_PadMuxInfo)),               key12_tbl},
    {(sizeof(key13_tbl)/sizeof(ST_PadMuxInfo)),               key13_tbl},
    {(sizeof(sd_d1_tbl)/sizeof(ST_PadMuxInfo)),               sd_d1_tbl},
    {(sizeof(sd_d0_tbl)/sizeof(ST_PadMuxInfo)),               sd_d0_tbl},
    {(sizeof(sd_clk_tbl)/sizeof(ST_PadMuxInfo)),              sd_clk_tbl},
    {(sizeof(sd_cmd_tbl)/sizeof(ST_PadMuxInfo)),              sd_cmd_tbl},
    {(sizeof(sd_d3_tbl)/sizeof(ST_PadMuxInfo)),               sd_d3_tbl},
    {(sizeof(sd_d2_tbl)/sizeof(ST_PadMuxInfo)),               sd_d2_tbl},
    {(sizeof(sd_gpio0_tbl)/sizeof(ST_PadMuxInfo)),            sd_gpio0_tbl},
    {(sizeof(sd_gpio1_tbl)/sizeof(ST_PadMuxInfo)),            sd_gpio1_tbl},
    {(sizeof(gpio0_tbl)/sizeof(ST_PadMuxInfo)),               gpio0_tbl},
    {(sizeof(gpio1_tbl)/sizeof(ST_PadMuxInfo)),               gpio1_tbl},
    {(sizeof(gpio2_tbl)/sizeof(ST_PadMuxInfo)),               gpio2_tbl},
    {(sizeof(gpio3_tbl)/sizeof(ST_PadMuxInfo)),               gpio3_tbl},
    {(sizeof(gpio4_tbl)/sizeof(ST_PadMuxInfo)),               gpio4_tbl},
    {(sizeof(gpio5_tbl)/sizeof(ST_PadMuxInfo)),               gpio5_tbl},
    {(sizeof(gpio6_tbl)/sizeof(ST_PadMuxInfo)),               gpio6_tbl},
    {(sizeof(gpio7_tbl)/sizeof(ST_PadMuxInfo)),               gpio7_tbl},
    {(sizeof(gpio8_tbl)/sizeof(ST_PadMuxInfo)),               gpio8_tbl},
    {(sizeof(gpio9_tbl)/sizeof(ST_PadMuxInfo)),               gpio9_tbl},
    {(sizeof(pm_sd_cdz_tbl)/sizeof(ST_PadMuxInfo)),           pm_sd_cdz_tbl},
    {(sizeof(pm_spi_cz_tbl)/sizeof(ST_PadMuxInfo)),           pm_spi_cz_tbl},
    {(sizeof(pm_spi_ck_tbl)/sizeof(ST_PadMuxInfo)),           pm_spi_ck_tbl},
    {(sizeof(pm_spi_di_tbl)/sizeof(ST_PadMuxInfo)),           pm_spi_di_tbl},
    {(sizeof(pm_spi_do_tbl)/sizeof(ST_PadMuxInfo)),           pm_spi_do_tbl},
    {(sizeof(pm_spi_wpz_tbl)/sizeof(ST_PadMuxInfo)),          pm_spi_wpz_tbl},
    {(sizeof(pm_spi_hld_tbl)/sizeof(ST_PadMuxInfo)),          pm_spi_hld_tbl},
    {(sizeof(pm_led0_tbl)/sizeof(ST_PadMuxInfo)),             pm_led0_tbl},
    {(sizeof(pm_led1_tbl)/sizeof(ST_PadMuxInfo)),             pm_led1_tbl},
    {(sizeof(eth_rn_tbl)/sizeof(ST_PadMuxInfo)),             eth_rn_tbl},
    {(sizeof(eth_rp_tbl)/sizeof(ST_PadMuxInfo)),             eth_rp_tbl},
    {(sizeof(eth_tn_tbl)/sizeof(ST_PadMuxInfo)),             eth_tn_tbl},
    {(sizeof(eth_tp_tbl)/sizeof(ST_PadMuxInfo)),             eth_tp_tbl},
    {(sizeof(sar_gpio0_tbl)/sizeof(ST_PadMuxInfo)),             sar_gpio0_tbl},
    {(sizeof(sar_gpio1_tbl)/sizeof(ST_PadMuxInfo)),             sar_gpio1_tbl},
    {(sizeof(sar_gpio2_tbl)/sizeof(ST_PadMuxInfo)),             sar_gpio2_tbl},
    {(sizeof(sar_gpio3_tbl)/sizeof(ST_PadMuxInfo)),             sar_gpio3_tbl},
};

static const ST_PadModeInfo m_stPadModeInfoTbl[] =
{
    {"GPIO_MODE",    0,    0,    0},
    {"ALL_PAD_IN_1",         _RIUA_16BIT(CHIPTOP_BANK,REG_ALL_PAD_IN),                  REG_ALL_PAD_IN_MASK,            BIT15},
    {"TEST_IN_MODE_1",       _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_IN_MODE),                REG_TEST_IN_MODE_MASK,          BIT0},
    {"TEST_IN_MODE_2",       _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_IN_MODE),                REG_TEST_IN_MODE_MASK,          BIT1},
    {"TEST_IN_MODE_3",       _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_IN_MODE),                REG_TEST_IN_MODE_MASK,          BIT1|BIT0},
    {"TEST_OUT_MODE_1",      _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_OUT_MODE),               REG_TEST_OUT_MODE_MASK,         BIT4},
    {"TEST_OUT_MODE_2",      _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_OUT_MODE),               REG_TEST_OUT_MODE_MASK,         BIT5},
    {"TEST_OUT_MODE_3",      _RIUA_16BIT(CHIPTOP_BANK,REG_TEST_OUT_MODE),               REG_TEST_OUT_MODE_MASK,         BIT5|BIT4},
    {"I2C0_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT0},
    {"I2C0_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT1},
    {"I2C0_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT1|BIT0},
    {"I2C0_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT2},
    {"I2C0_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT2|BIT0},
    {"I2C0_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT2|BIT1},
    {"I2C0_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT2|BIT1|BIT0},
    {"I2C0_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3},
    {"I2C0_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT0},
    {"I2C0_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT1},
    {"I2C0_MODE_11",         _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT1|BIT0},
    {"I2C0_MODE_12",         _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT2},
    {"I2C0_MODE_13",         _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT2|BIT0},
    {"I2C0_MODE_14",         _RIUA_16BIT(PADTOP_BANK,REG_I2C0_MODE),                    REG_I2C0_MODE_MASK,             BIT3|BIT2|BIT1},
    {"I2C1_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT4},
    {"I2C1_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT5},
    {"I2C1_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT5|BIT4},
    {"I2C1_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT6},
    {"I2C1_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT6|BIT4},
    {"I2C1_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT6|BIT5},
    {"I2C1_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT6|BIT5|BIT4},
    {"I2C1_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT7},
    {"I2C1_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT7|BIT4},
    {"I2C1_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT7|BIT5},
    {"I2C1_MODE_11",         _RIUA_16BIT(PADTOP_BANK,REG_I2C1_MODE),                    REG_I2C1_MODE_MASK,             BIT7|BIT5|BIT4},
    {"SR0_MIPI_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR0_MIPI_MODE),                REG_SR0_MIPI_MODE_MASK,         BIT0},
    {"SR1_MIPI_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_MIPI_MODE),                REG_SR1_MIPI_MODE_MASK,         BIT4},
    {"SR1_MIPI_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_MIPI_MODE),                REG_SR1_MIPI_MODE_MASK,         BIT5},
    {"ISP_IR_MODE_1",        _RIUA_16BIT(PADTOP_BANK,REG_ISP_IR_MODE),                  REG_ISP_IR_MODE_MASK,           BIT0},
    {"ISP_IR_MODE_2",        _RIUA_16BIT(PADTOP_BANK,REG_ISP_IR_MODE),                  REG_ISP_IR_MODE_MASK,           BIT1},
    {"ISP_IR_MODE_3",        _RIUA_16BIT(PADTOP_BANK,REG_ISP_IR_MODE),                  REG_ISP_IR_MODE_MASK,           BIT1|BIT0},
    {"SR0_CTRL_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR0_CTRL_MODE),                REG_SR0_CTRL_MODE_MASK,         BIT8},
    {"SR0_CTRL_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SR0_CTRL_MODE),                REG_SR0_CTRL_MODE_MASK,         BIT9},
    {"SR0_MCLK_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR0_MCLK_MODE),                REG_SR0_MCLK_MODE_MASK,         BIT0},
    {"SR0_MCLK_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SR0_MCLK_MODE),                REG_SR0_MCLK_MODE_MASK,         BIT1},
    {"SR0_PDN_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_SR0_PDN_MODE),                 REG_SR0_PDN_MODE_MASK,          BIT8},
    {"SR0_RST_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_SR0_RST_MODE),                 REG_SR0_RST_MODE_MASK,          BIT4},
    {"SR0_RST_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_SR0_RST_MODE),                 REG_SR0_RST_MODE_MASK,          BIT5},
    {"SR1_CTRL_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_CTRL_MODE),                REG_SR1_CTRL_MODE_MASK,         BIT12},
    {"SR1_CTRL_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_CTRL_MODE),                REG_SR1_CTRL_MODE_MASK,         BIT13},
    {"SR1_MCLK_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_MCLK_MODE),                REG_SR1_MCLK_MODE_MASK,         BIT0},
    {"SR1_MCLK_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SR1_MCLK_MODE),                REG_SR1_MCLK_MODE_MASK,         BIT1},
    {"SR1_PDN_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_SR1_PDN_MODE),                 REG_SR1_PDN_MODE_MASK,          BIT8},
    {"SR1_PDN_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_SR1_PDN_MODE),                 REG_SR1_PDN_MODE_MASK,          BIT9},
    {"SR1_RST_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_SR1_RST_MODE),                 REG_SR1_RST_MODE_MASK,          BIT4},
    {"SR1_RST_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_SR1_RST_MODE),                 REG_SR1_RST_MODE_MASK,          BIT5},
    {"SR0_BT601_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT601_MODE),               REG_SR0_BT601_MODE_MASK,        BIT0},
    {"SR0_BT601_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT601_MODE),               REG_SR0_BT601_MODE_MASK,        BIT1},
    {"SR0_BT601_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT601_MODE),               REG_SR0_BT601_MODE_MASK,        BIT1|BIT0},
    {"SR0_BT656_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT656_MODE),               REG_SR0_BT656_MODE_MASK,        BIT4},
    {"SR0_BT656_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT656_MODE),               REG_SR0_BT656_MODE_MASK,        BIT5},
    {"SR0_BT656_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT656_MODE),               REG_SR0_BT656_MODE_MASK,        BIT5|BIT4},
    {"SR0_BT656_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_SR0_BT656_MODE),               REG_SR0_BT656_MODE_MASK,        BIT6},
    {"ETH0_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT0},
    {"ETH0_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT1},
    {"ETH0_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT1|BIT0},
    {"ETH0_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT2},
    {"ETH0_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT2|BIT0},
    {"ETH0_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT2|BIT1},
    {"ETH0_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT2|BIT1|BIT0},
    {"ETH0_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT3},
    {"ETH0_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT3|BIT0},
    {"ETH0_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT3|BIT1},
    {"ETH0_MODE_11",         _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT3|BIT1|BIT0},
    {"ETH0_MODE_12",         _RIUA_16BIT(PADTOP_BANK,REG_ETH0_MODE),                    REG_ETH0_MODE_MASK,             BIT3|BIT2},
    {"TTL16_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_TTL16_MODE),                   REG_TTL16_MODE_MASK,            BIT0},
    {"TTL16_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_TTL16_MODE),                   REG_TTL16_MODE_MASK,            BIT1},
    {"TTL16_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_TTL16_MODE),                   REG_TTL16_MODE_MASK,            BIT1|BIT0},
    {"TTL16_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_TTL16_MODE),                   REG_TTL16_MODE_MASK,            BIT2},
    {"TTL18_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_TTL18_MODE),                   REG_TTL18_MODE_MASK,            BIT4},
    {"TTL18_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_TTL18_MODE),                   REG_TTL18_MODE_MASK,            BIT5},
    {"TTL18_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_TTL18_MODE),                   REG_TTL18_MODE_MASK,            BIT5|BIT4},
    {"TTL24_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_TTL24_MODE),                   REG_TTL24_MODE_MASK,            BIT8},
    {"TTL24_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_TTL24_MODE),                   REG_TTL24_MODE_MASK,            BIT9},
    {"TTL24_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_TTL24_MODE),                   REG_TTL24_MODE_MASK,            BIT9|BIT8},
    {"TTL24_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_TTL24_MODE),                   REG_TTL24_MODE_MASK,            BIT10},
    {"TTL24_MODE_5",         _RIUA_16BIT(PADTOP_BANK,REG_TTL24_MODE),                   REG_TTL24_MODE_MASK,            BIT10|BIT8},
    {"RGB8_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_RGB8_MODE),                    REG_RGB8_MODE_MASK,             BIT0},
    {"RGB8_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_RGB8_MODE),                    REG_RGB8_MODE_MASK,             BIT1},
    {"RGB8_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_RGB8_MODE),                    REG_RGB8_MODE_MASK,             BIT1|BIT0},
    {"BT656_OUT_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_BT656_OUT_MODE),               REG_BT656_OUT_MODE_MASK,        BIT12},
    {"BT656_OUT_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_BT656_OUT_MODE),               REG_BT656_OUT_MODE_MASK,        BIT13},
    {"BT656_OUT_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_BT656_OUT_MODE),               REG_BT656_OUT_MODE_MASK,        BIT13|BIT12},
    {"PSPI0_SR_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT0},
    {"PSPI0_SR_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT1},
    {"PSPI0_SR_MODE_3",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT1|BIT0},
    {"PSPI0_SR_MODE_4",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT2},
    {"PSPI0_SR_MODE_5",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT2|BIT0},
    {"PSPI0_SR_MODE_6",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_SR_MODE),                REG_PSPI0_SR_MODE_MASK,         BIT2|BIT1},
    {"PSPI0_G_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_G_MODE),                 REG_PSPI0_G_MODE_MASK,          BIT4},
    {"PSPI0_G_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_PSPI0_G_MODE),                 REG_PSPI0_G_MODE_MASK,          BIT5},
    {"SPI0_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_SPI0_MODE),                    REG_SPI0_MODE_MASK,             BIT0},
    {"SPI0_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_SPI0_MODE),                    REG_SPI0_MODE_MASK,             BIT1},
    {"SPI0_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_SPI0_MODE),                    REG_SPI0_MODE_MASK,             BIT1|BIT0},
    {"SPI0_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_SPI0_MODE),                    REG_SPI0_MODE_MASK,             BIT2},
    {"SPI0_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_SPI0_MODE),                    REG_SPI0_MODE_MASK,             BIT2|BIT0},
    {"PSPI1_PL_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT0},
    {"PSPI1_PL_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT1},
    {"PSPI1_PL_MODE_3",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT1|BIT0},
    {"PSPI1_PL_MODE_4",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT2},
    {"PSPI1_PL_MODE_5",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT2|BIT0},
    {"PSPI1_PL_MODE_6",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT2|BIT1},
    {"PSPI1_PL_MODE_7",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT2|BIT1|BIT0},
    {"PSPI1_PL_MODE_8",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT3},
    {"PSPI1_PL_MODE_9",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_PL_MODE),                REG_PSPI1_PL_MODE_MASK,         BIT3|BIT0},
    {"PSPI1_CS2_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT4},
    {"PSPI1_CS2_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT5},
    {"PSPI1_CS2_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT5|BIT4},
    {"PSPI1_CS2_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT6},
    {"PSPI1_CS2_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT6|BIT4},
    {"PSPI1_CS2_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT6|BIT5},
    {"PSPI1_CS2_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT6|BIT5|BIT4},
    {"PSPI1_CS2_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT7},
    {"PSPI1_CS2_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT7|BIT4},
    {"PSPI1_CS2_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_CS2_MODE),               REG_PSPI1_CS2_MODE_MASK,        BIT7|BIT5},
    {"PSPI1_TE_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT8},
    {"PSPI1_TE_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT9},
    {"PSPI1_TE_MODE_3",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT9|BIT8},
    {"PSPI1_TE_MODE_4",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT10},
    {"PSPI1_TE_MODE_5",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT10|BIT8},
    {"PSPI1_TE_MODE_6",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT10|BIT9},
    {"PSPI1_TE_MODE_7",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT10|BIT9|BIT8},
    {"PSPI1_TE_MODE_8",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT11},
    {"PSPI1_TE_MODE_9",      _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT11|BIT8},
    {"PSPI1_TE_MODE_10",     _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_TE_MODE),                REG_PSPI1_TE_MODE_MASK,         BIT11|BIT9},
    {"PSPI1_G_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_G_MODE),                 REG_PSPI1_G_MODE_MASK,          BIT12},
    {"PSPI1_G_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_PSPI1_G_MODE),                 REG_PSPI1_G_MODE_MASK,          BIT13},
    {"SPI1_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT8},
    {"SPI1_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT9},
    {"SPI1_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT9|BIT8},
    {"SPI1_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT10},
    {"SPI1_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT10|BIT8},
    {"SPI1_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT10|BIT9},
    {"SPI1_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT10|BIT9|BIT8},
    {"SPI1_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_SPI1_MODE),                    REG_SPI1_MODE_MASK,             BIT11},
    {"SPI1_CS2_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT12},
    {"SPI1_CS2_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT13},
    {"SPI1_CS2_MODE_3",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT13|BIT12},
    {"SPI1_CS2_MODE_4",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT14},
    {"SPI1_CS2_MODE_5",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT14|BIT12},
    {"SPI1_CS2_MODE_6",      _RIUA_16BIT(PADTOP_BANK,REG_SPI1_CS2_MODE),                REG_SPI1_CS2_MODE_MASK,         BIT14|BIT13},
    {"SDIO_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_SDIO_MODE),                    REG_SDIO_MODE_MASK,             BIT8},
    {"SDIO_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_SDIO_MODE),                    REG_SDIO_MODE_MASK,             BIT9},
    {"SD_CDZ_MODE_1",        _RIUA_16BIT(PADTOP_BANK,REG_SD_CDZ_MODE),                  REG_SD_CDZ_MODE_MASK,           BIT0},
    {"SD_CDZ_MODE_2",        _RIUA_16BIT(PADTOP_BANK,REG_SD_CDZ_MODE),                  REG_SD_CDZ_MODE_MASK,           BIT1},
    {"KEY_READ0_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT0},
    {"KEY_READ0_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT1},
    {"KEY_READ0_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT1|BIT0},
    {"KEY_READ0_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT2},
    {"KEY_READ0_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT2|BIT0},
    {"KEY_READ0_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT2|BIT1},
    {"KEY_READ0_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT2|BIT1|BIT0},
    {"KEY_READ0_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3},
    {"KEY_READ0_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT0},
    {"KEY_READ0_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT1},
    {"KEY_READ0_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT1|BIT0},
    {"KEY_READ0_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT2},
    {"KEY_READ0_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT2|BIT0},
    {"KEY_READ0_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ0_MODE),               REG_KEY_READ0_MODE_MASK,        BIT3|BIT2|BIT1},
    {"KEY_READ1_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT4},
    {"KEY_READ1_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT5},
    {"KEY_READ1_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT5|BIT4},
    {"KEY_READ1_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT6},
    {"KEY_READ1_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT6|BIT4},
    {"KEY_READ1_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT6|BIT5},
    {"KEY_READ1_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT6|BIT5|BIT4},
    {"KEY_READ1_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7},
    {"KEY_READ1_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT4},
    {"KEY_READ1_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT5},
    {"KEY_READ1_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT5|BIT4},
    {"KEY_READ1_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT6},
    {"KEY_READ1_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT6|BIT4},
    {"KEY_READ1_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ1_MODE),               REG_KEY_READ1_MODE_MASK,        BIT7|BIT6|BIT5},
    {"KEY_READ2_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT8},
    {"KEY_READ2_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT9},
    {"KEY_READ2_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT9|BIT8},
    {"KEY_READ2_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT10},
    {"KEY_READ2_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT10|BIT8},
    {"KEY_READ2_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT10|BIT9},
    {"KEY_READ2_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT10|BIT9|BIT8},
    {"KEY_READ2_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11},
    {"KEY_READ2_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT8},
    {"KEY_READ2_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT9},
    {"KEY_READ2_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT9|BIT8},
    {"KEY_READ2_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT10},
    {"KEY_READ2_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT10|BIT8},
    {"KEY_READ2_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ2_MODE),               REG_KEY_READ2_MODE_MASK,        BIT11|BIT10|BIT9},
    {"KEY_READ3_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT12},
    {"KEY_READ3_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT13},
    {"KEY_READ3_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT13|BIT12},
    {"KEY_READ3_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT14},
    {"KEY_READ3_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT14|BIT12},
    {"KEY_READ3_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT14|BIT13},
    {"KEY_READ3_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT14|BIT13|BIT12},
    {"KEY_READ3_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15},
    {"KEY_READ3_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT12},
    {"KEY_READ3_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT13},
    {"KEY_READ3_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT13|BIT12},
    {"KEY_READ3_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT14},
    {"KEY_READ3_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT14|BIT12},
    {"KEY_READ3_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ3_MODE),               REG_KEY_READ3_MODE_MASK,        BIT15|BIT14|BIT13},
    {"KEY_READ4_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT0},
    {"KEY_READ4_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT1},
    {"KEY_READ4_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT1|BIT0},
    {"KEY_READ4_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT2},
    {"KEY_READ4_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT2|BIT0},
    {"KEY_READ4_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT2|BIT1},
    {"KEY_READ4_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT2|BIT1|BIT0},
    {"KEY_READ4_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3},
    {"KEY_READ4_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT0},
    {"KEY_READ4_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT1},
    {"KEY_READ4_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT1|BIT0},
    {"KEY_READ4_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT2},
    {"KEY_READ4_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT2|BIT0},
    {"KEY_READ4_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ4_MODE),               REG_KEY_READ4_MODE_MASK,        BIT3|BIT2|BIT1},
    {"KEY_READ5_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT4},
    {"KEY_READ5_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT5},
    {"KEY_READ5_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT5|BIT4},
    {"KEY_READ5_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT6},
    {"KEY_READ5_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT6|BIT4},
    {"KEY_READ5_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT6|BIT5},
    {"KEY_READ5_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT6|BIT5|BIT4},
    {"KEY_READ5_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7},
    {"KEY_READ5_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT4},
    {"KEY_READ5_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT5},
    {"KEY_READ5_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT5|BIT4},
    {"KEY_READ5_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT6},
    {"KEY_READ5_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT6|BIT4},
    {"KEY_READ5_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ5_MODE),               REG_KEY_READ5_MODE_MASK,        BIT7|BIT6|BIT5},
    {"KEY_READ6_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT8},
    {"KEY_READ6_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT9},
    {"KEY_READ6_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT9|BIT8},
    {"KEY_READ6_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT10},
    {"KEY_READ6_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT10|BIT8},
    {"KEY_READ6_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT10|BIT9},
    {"KEY_READ6_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT10|BIT9|BIT8},
    {"KEY_READ6_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11},
    {"KEY_READ6_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT8},
    {"KEY_READ6_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT9},
    {"KEY_READ6_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT9|BIT8},
    {"KEY_READ6_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT10},
    {"KEY_READ6_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT10|BIT8},
    {"KEY_READ6_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_READ6_MODE),               REG_KEY_READ6_MODE_MASK,        BIT11|BIT10|BIT9},
    {"KEY_SCAN0_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT0},
    {"KEY_SCAN0_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT1},
    {"KEY_SCAN0_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT1|BIT0},
    {"KEY_SCAN0_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT2},
    {"KEY_SCAN0_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT2|BIT0},
    {"KEY_SCAN0_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT2|BIT1},
    {"KEY_SCAN0_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT2|BIT1|BIT0},
    {"KEY_SCAN0_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3},
    {"KEY_SCAN0_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT0},
    {"KEY_SCAN0_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT1},
    {"KEY_SCAN0_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT1|BIT0},
    {"KEY_SCAN0_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT2},
    {"KEY_SCAN0_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT2|BIT0},
    {"KEY_SCAN0_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN0_MODE),               REG_KEY_SCAN0_MODE_MASK,        BIT3|BIT2|BIT1},
    {"KEY_SCAN1_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT4},
    {"KEY_SCAN1_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT5},
    {"KEY_SCAN1_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT5|BIT4},
    {"KEY_SCAN1_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT6},
    {"KEY_SCAN1_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT6|BIT4},
    {"KEY_SCAN1_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT6|BIT5},
    {"KEY_SCAN1_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT6|BIT5|BIT4},
    {"KEY_SCAN1_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7},
    {"KEY_SCAN1_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT4},
    {"KEY_SCAN1_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT5},
    {"KEY_SCAN1_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT5|BIT4},
    {"KEY_SCAN1_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT6},
    {"KEY_SCAN1_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT6|BIT4},
    {"KEY_SCAN1_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN1_MODE),               REG_KEY_SCAN1_MODE_MASK,        BIT7|BIT6|BIT5},
    {"KEY_SCAN2_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT8},
    {"KEY_SCAN2_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT9},
    {"KEY_SCAN2_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT9|BIT8},
    {"KEY_SCAN2_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT10},
    {"KEY_SCAN2_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT10|BIT8},
    {"KEY_SCAN2_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT10|BIT9},
    {"KEY_SCAN2_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT10|BIT9|BIT8},
    {"KEY_SCAN2_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11},
    {"KEY_SCAN2_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT8},
    {"KEY_SCAN2_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT9},
    {"KEY_SCAN2_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT9|BIT8},
    {"KEY_SCAN2_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT10},
    {"KEY_SCAN2_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT10|BIT8},
    {"KEY_SCAN2_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN2_MODE),               REG_KEY_SCAN2_MODE_MASK,        BIT11|BIT10|BIT9},
    {"KEY_SCAN3_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT12},
    {"KEY_SCAN3_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT13},
    {"KEY_SCAN3_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT13|BIT12},
    {"KEY_SCAN3_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT14},
    {"KEY_SCAN3_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT14|BIT12},
    {"KEY_SCAN3_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT14|BIT13},
    {"KEY_SCAN3_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT14|BIT13|BIT12},
    {"KEY_SCAN3_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15},
    {"KEY_SCAN3_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT12},
    {"KEY_SCAN3_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT13},
    {"KEY_SCAN3_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT13|BIT12},
    {"KEY_SCAN3_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT14},
    {"KEY_SCAN3_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT14|BIT12},
    {"KEY_SCAN3_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN3_MODE),               REG_KEY_SCAN3_MODE_MASK,        BIT15|BIT14|BIT13},
    {"KEY_SCAN4_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT0},
    {"KEY_SCAN4_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT1},
    {"KEY_SCAN4_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT1|BIT0},
    {"KEY_SCAN4_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT2},
    {"KEY_SCAN4_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT2|BIT0},
    {"KEY_SCAN4_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT2|BIT1},
    {"KEY_SCAN4_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT2|BIT1|BIT0},
    {"KEY_SCAN4_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3},
    {"KEY_SCAN4_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT0},
    {"KEY_SCAN4_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT1},
    {"KEY_SCAN4_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT1|BIT0},
    {"KEY_SCAN4_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT2},
    {"KEY_SCAN4_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT2|BIT0},
    {"KEY_SCAN4_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN4_MODE),               REG_KEY_SCAN4_MODE_MASK,        BIT3|BIT2|BIT1},
    {"KEY_SCAN5_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT4},
    {"KEY_SCAN5_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT5},
    {"KEY_SCAN5_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT5|BIT4},
    {"KEY_SCAN5_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT6},
    {"KEY_SCAN5_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT6|BIT4},
    {"KEY_SCAN5_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT6|BIT5},
    {"KEY_SCAN5_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT6|BIT5|BIT4},
    {"KEY_SCAN5_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7},
    {"KEY_SCAN5_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT4},
    {"KEY_SCAN5_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT5},
    {"KEY_SCAN5_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT5|BIT4},
    {"KEY_SCAN5_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT6},
    {"KEY_SCAN5_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT6|BIT4},
    {"KEY_SCAN5_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN5_MODE),               REG_KEY_SCAN5_MODE_MASK,        BIT7|BIT6|BIT5},
    {"KEY_SCAN6_MODE_1",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT8},
    {"KEY_SCAN6_MODE_2",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT9},
    {"KEY_SCAN6_MODE_3",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT9|BIT8},
    {"KEY_SCAN6_MODE_4",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT10},
    {"KEY_SCAN6_MODE_5",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT10|BIT8},
    {"KEY_SCAN6_MODE_6",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT10|BIT9},
    {"KEY_SCAN6_MODE_7",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT10|BIT9|BIT8},
    {"KEY_SCAN6_MODE_8",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11},
    {"KEY_SCAN6_MODE_9",     _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT8},
    {"KEY_SCAN6_MODE_10",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT9},
    {"KEY_SCAN6_MODE_11",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT9|BIT8},
    {"KEY_SCAN6_MODE_12",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT10},
    {"KEY_SCAN6_MODE_13",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT10|BIT8},
    {"KEY_SCAN6_MODE_14",    _RIUA_16BIT(PADTOP_BANK,REG_KEY_SCAN6_MODE),               REG_KEY_SCAN6_MODE_MASK,        BIT11|BIT10|BIT9},
    {"KEY_FIX_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_KEY_FIX_MODE),                 REG_KEY_FIX_MODE_MASK,          BIT12},
    {"FUART_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT8},
    {"FUART_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT9},
    {"FUART_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT9|BIT8},
    {"FUART_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT10},
    {"FUART_MODE_5",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT10|BIT8},
    {"FUART_MODE_6",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT10|BIT9},
    {"FUART_MODE_7",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT10|BIT9|BIT8},
    {"FUART_MODE_8",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT11},
    {"FUART_MODE_9",         _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT11|BIT8},
    {"FUART_MODE_10",        _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT11|BIT9},
    {"FUART_MODE_11",        _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT11|BIT9|BIT8},
    {"FUART_MODE_12",        _RIUA_16BIT(PADTOP_BANK,REG_FUART_MODE),                   REG_FUART_MODE_MASK,            BIT11|BIT10},
    {"UART0_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_UART0_MODE),                   REG_UART0_MODE_MASK,            BIT0},
    {"UART0_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_UART0_MODE),                   REG_UART0_MODE_MASK,            BIT1},
    {"UART0_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_UART0_MODE),                   REG_UART0_MODE_MASK,            BIT1|BIT0},
    {"UART0_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_UART0_MODE),                   REG_UART0_MODE_MASK,            BIT2},
    {"UART0_MODE_5",         _RIUA_16BIT(PADTOP_BANK,REG_UART0_MODE),                   REG_UART0_MODE_MASK,            BIT2|BIT0},
    {"UART1_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT4},
    {"UART1_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT5},
    {"UART1_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT5|BIT4},
    {"UART1_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT6},
    {"UART1_MODE_5",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT6|BIT4},
    {"UART1_MODE_6",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT6|BIT5},
    {"UART1_MODE_7",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT6|BIT5|BIT4},
    {"UART1_MODE_8",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT7},
    {"UART1_MODE_9",         _RIUA_16BIT(PADTOP_BANK,REG_UART1_MODE),                   REG_UART1_MODE_MASK,            BIT7|BIT4},
    {"UART2_MODE_1",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT8},
    {"UART2_MODE_2",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT9},
    {"UART2_MODE_3",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT9|BIT8},
    {"UART2_MODE_4",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT10},
    {"UART2_MODE_5",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT10|BIT8},
    {"UART2_MODE_6",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT10|BIT9},
    {"UART2_MODE_7",         _RIUA_16BIT(PADTOP_BANK,REG_UART2_MODE),                   REG_UART2_MODE_MASK,            BIT10|BIT9|BIT8},
    {"PWM0_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT0},
    {"PWM0_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT1},
    {"PWM0_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT1|BIT0},
    {"PWM0_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT2},
    {"PWM0_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT2|BIT0},
    {"PWM0_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT2|BIT1},
    {"PWM0_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT2|BIT1|BIT0},
    {"PWM0_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT3},
    {"PWM0_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT3|BIT0},
    {"PWM0_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_PWM0_MODE),                    REG_PWM0_MODE_MASK,             BIT3|BIT1},
    {"PWM1_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT4},
    {"PWM1_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT5},
    {"PWM1_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT5|BIT4},
    {"PWM1_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT6},
    {"PWM1_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT6|BIT4},
    {"PWM1_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT6|BIT5},
    {"PWM1_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT6|BIT5|BIT4},
    {"PWM1_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT7},
    {"PWM1_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT7|BIT4},
    {"PWM1_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_PWM1_MODE),                    REG_PWM1_MODE_MASK,             BIT7|BIT5},
    {"PWM2_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT8},
    {"PWM2_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT9},
    {"PWM2_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT9|BIT8},
    {"PWM2_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT10},
    {"PWM2_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT10|BIT8},
    {"PWM2_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT10|BIT9},
    {"PWM2_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT10|BIT9|BIT8},
    {"PWM2_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT11},
    {"PWM2_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT11|BIT8},
    {"PWM2_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT11|BIT9},
    {"PWM2_MODE_11",         _RIUA_16BIT(PADTOP_BANK,REG_PWM2_MODE),                    REG_PWM2_MODE_MASK,             BIT11|BIT9|BIT8},
    {"PWM3_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT12},
    {"PWM3_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT13},
    {"PWM3_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT13|BIT12},
    {"PWM3_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT14},
    {"PWM3_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT14|BIT12},
    {"PWM3_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT14|BIT13},
    {"PWM3_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT14|BIT13|BIT12},
    {"PWM3_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT15},
    {"PWM3_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT15|BIT12},
    {"PWM3_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_PWM3_MODE),                    REG_PWM3_MODE_MASK,             BIT15|BIT13},
    {"DMIC_MODE_1",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT0},
    {"DMIC_MODE_2",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT1},
    {"DMIC_MODE_3",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT1|BIT0},
    {"DMIC_MODE_4",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT2},
    {"DMIC_MODE_5",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT2|BIT0},
    {"DMIC_MODE_6",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT2|BIT1},
    {"DMIC_MODE_7",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT2|BIT1|BIT0},
    {"DMIC_MODE_8",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3},
    {"DMIC_MODE_9",          _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3|BIT0},
    {"DMIC_MODE_10",         _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3|BIT1},
    {"DMIC_MODE_11",         _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3|BIT1|BIT0},
    {"DMIC_MODE_12",         _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3|BIT2},
    {"DMIC_MODE_13",         _RIUA_16BIT(PADTOP_BANK,REG_DMIC_MODE),                    REG_DMIC_MODE_MASK,             BIT3|BIT2|BIT0},
    {"I2S_MCK_MODE_1",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT0},
    {"I2S_MCK_MODE_2",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT1},
    {"I2S_MCK_MODE_3",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT1|BIT0},
    {"I2S_MCK_MODE_4",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT2},
    {"I2S_MCK_MODE_5",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT2|BIT0},
    {"I2S_MCK_MODE_6",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT2|BIT1},
    {"I2S_MCK_MODE_7",       _RIUA_16BIT(PADTOP_BANK,REG_I2S_MCK_MODE),                 REG_I2S_MCK_MODE_MASK,          BIT2|BIT1|BIT0},
    {"I2S_RX_MODE_1",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT4},
    {"I2S_RX_MODE_2",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT5},
    {"I2S_RX_MODE_3",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT5|BIT4},
    {"I2S_RX_MODE_4",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT6},
    {"I2S_RX_MODE_5",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT6|BIT4},
    {"I2S_RX_MODE_6",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_RX_MODE),                  REG_I2S_RX_MODE_MASK,           BIT6|BIT5},
    {"I2S_TX_MODE_1",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT8},
    {"I2S_TX_MODE_2",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT9},
    {"I2S_TX_MODE_3",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT9|BIT8},
    {"I2S_TX_MODE_4",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT10},
    {"I2S_TX_MODE_5",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT10|BIT8},
    {"I2S_TX_MODE_6",        _RIUA_16BIT(PADTOP_BANK,REG_I2S_TX_MODE),                  REG_I2S_TX_MODE_MASK,           BIT10|BIT9},
    {"I2S_RXTX_MODE_1",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT12},
    {"I2S_RXTX_MODE_2",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT13},
    {"I2S_RXTX_MODE_3",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT13|BIT12},
    {"I2S_RXTX_MODE_4",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT14},
    {"I2S_RXTX_MODE_5",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT14|BIT12},
    {"I2S_RXTX_MODE_6",      _RIUA_16BIT(PADTOP_BANK,REG_I2S_RXTX_MODE),                REG_I2S_RXTX_MODE_MASK,         BIT14|BIT13},
    {"BT1120_MODE_1",        _RIUA_16BIT(PADTOP_BANK,REG_BT1120_MODE),                  REG_BT1120_MODE_MASK,           BIT0},
    {"BT1120_MODE_2",        _RIUA_16BIT(PADTOP_BANK,REG_BT1120_MODE),                  REG_BT1120_MODE_MASK,           BIT1},
    {"BT1120_MODE_3",        _RIUA_16BIT(PADTOP_BANK,REG_BT1120_MODE),                  REG_BT1120_MODE_MASK,           BIT1|BIT0},
    {"PM_SPI_GPIO_0",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SPI_GPIO),                 REG_PM_SPI_GPIO_MASK,           0},
    {"PM_SPIWPN_GPIO_0",     _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SPIWPN_GPIO),              REG_PM_SPIWPN_GPIO_MASK,        0},
    {"PM_SPIHOLDN_MODE_0",   _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SPIHOLDN_MODE),            REG_PM_SPIHOLDN_MODE_MASK,      0},
    {"PM_SPICSZ1_GPIO_0",    _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SPICSZ1_GPIO),             REG_PM_SPICSZ1_GPIO_MASK,       0},
    {"PM_SPICSZ2_GPIO_0",    _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SPICSZ2_GPIO),             REG_PM_SPICSZ2_GPIO_MASK,       0},
    {"PM_PWM0_MODE_1",       _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PWM0_MODE),                REG_PM_PWM0_MODE_MASK,          BIT0},
    {"PM_PWM1_MODE_1",       _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PWM1_MODE),                REG_PM_PWM1_MODE_MASK,          BIT2},
    {"PM_PWM2_MODE_1",       _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PWM2_MODE),                REG_PM_PWM2_MODE_MASK,          BIT6},
    {"PM_PWM3_MODE_1",       _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PWM3_MODE),                REG_PM_PWM3_MODE_MASK,          BIT0},
    {"PM_UART1_MODE_1",      _RIUA_16BIT(PMSLEEP_BANK,REG_PM_UART1_MODE),               REG_PM_UART1_MODE_MASK,         BIT0},
    {"PM_VID_MODE_1",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_VID_MODE),                 REG_PM_VID_MODE_MASK,           BIT4},
    {"PM_VID_MODE_2",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_VID_MODE),                 REG_PM_VID_MODE_MASK,           BIT5},
    {"PM_VID_MODE_3",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_VID_MODE),                 REG_PM_VID_MODE_MASK,           BIT4|BIT5},
    {"PM_SD_CDZ_MODE_1",     _RIUA_16BIT(PMSLEEP_BANK,REG_PM_SD_CDZ_MODE),              REG_PM_SD_CDZ_MODE_MASK,        BIT6},
    {"PM_LED_MODE_1",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_LED_MODE),                 REG_PM_LED_MODE_MASK,           BIT4},
    {"PM_LED_MODE_2",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_LED_MODE),                 REG_PM_LED_MODE_MASK,           BIT5},
    {"PM_LED_MODE_3",        _RIUA_16BIT(PMSLEEP_BANK,REG_PM_LED_MODE),                 REG_PM_LED_MODE_MASK,           BIT4|BIT5},
    {"PM_PAD_EXT_MODE_0_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_0),           REG_PM_PAD_EXT_MODE_0_MASK,     BIT0},
    {"PM_PAD_EXT_MODE_1_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_1),           REG_PM_PAD_EXT_MODE_1_MASK,     BIT1},
    {"PM_PAD_EXT_MODE_2_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_2),           REG_PM_PAD_EXT_MODE_2_MASK,     BIT2},
    {"PM_PAD_EXT_MODE_3_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_3),           REG_PM_PAD_EXT_MODE_3_MASK,     BIT3},
    {"PM_PAD_EXT_MODE_4_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_4),           REG_PM_PAD_EXT_MODE_4_MASK,     BIT4},
    {"PM_PAD_EXT_MODE_5_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_5),           REG_PM_PAD_EXT_MODE_5_MASK,     BIT5},
    {"PM_PAD_EXT_MODE_6_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_6),           REG_PM_PAD_EXT_MODE_6_MASK,     BIT6},
    {"PM_PAD_EXT_MODE_7_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_7),           REG_PM_PAD_EXT_MODE_7_MASK,     BIT7},
    {"PM_PAD_EXT_MODE_8_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_8),           REG_PM_PAD_EXT_MODE_8_MASK,     BIT8},
    {"PM_PAD_EXT_MODE_9_1",  _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_9),           REG_PM_PAD_EXT_MODE_9_MASK,     BIT9},
    {"PM_PAD_EXT_MODE_10_1", _RIUA_16BIT(PMSLEEP_BANK,REG_PM_PAD_EXT_MODE_10),          REG_PM_PAD_EXT_MODE_10_MASK,    BIT10},
};


//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : _HalCheckPin
//  Description :
//------------------------------------------------------------------------------
static S32 _HalCheckPin(U32 padID)
{
    if (GPIO_NR <= padID) {
        return FALSE;
    }
    return TRUE;
}

#if 0
static void _HalSARGPIOWriteRegBit(u32 u32RegOffset, bool bEnable, U8 u8BitMsk)
{
    if (bEnable)
        _GPIO_R_BYTE(_RIUA_8BIT(PM_SAR_BANK, u32RegOffset)) |= u8BitMsk;
    else
        _GPIO_R_BYTE(_RIUA_8BIT(PM_SAR_BANK, u32RegOffset)) &= (~u8BitMsk);
}
#endif

void _HalPadDisablePadMux(U32 u32PadModeID)
{
    if (_GPIO_R_WORD_MASK(m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU, m_stPadModeInfoTbl[u32PadModeID].u16ModeMask)) {
        _GPIO_W_WORD_MASK(m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU, 0, m_stPadModeInfoTbl[u32PadModeID].u16ModeMask);
    }
}

void _HalPadEnablePadMux(U32 u32PadModeID)
{
    _GPIO_W_WORD_MASK(m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU, m_stPadModeInfoTbl[u32PadModeID].u16ModeVal, m_stPadModeInfoTbl[u32PadModeID].u16ModeMask);
}

static S32 HalPadSetMode_General(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U16 i = 0;

    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= ~(m_stPadMuxEntry[u32PadID].padmux[i].mask);
            u16RegVal |= m_stPadMuxEntry[u32PadID].padmux[i].val; // CHECK Multi-Pad Mode
            _GPIO_W_WORD_MASK(u32RegAddr, u16RegVal, 0xFFFF);
            u8ModeIsFind = 1;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
                break;
#endif
            }
        else
        {   //Clear high priority setting
            if(u32Mode == PINMUX_FOR_GPIO_MODE)
                continue;
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask);
            if (u16RegVal == m_stPadMuxEntry[u32PadID].padmux[i].val)
            {
                printk(KERN_INFO"[Padmux]reset PAD%d(reg 0x%x:%x; mask0x%x) t0 %s (org: %s)\n",
                       u32PadID,
                       m_stPadMuxEntry[u32PadID].padmux[i].base,
                       m_stPadMuxEntry[u32PadID].padmux[i].offset,
                       m_stPadMuxEntry[u32PadID].padmux[i].mask,
                       m_stPadModeInfoTbl[u32Mode].u8PadName,
                       m_stPadModeInfoTbl[m_stPadMuxEntry[u32PadID].padmux[i].mode].u8PadName);
                if (m_stPadMuxEntry[u32PadID].padmux[i].val != 0)
                {
                    _GPIO_W_WORD_MASK(u32RegAddr, 0, m_stPadMuxEntry[u32PadID].padmux[i].mask);
                }
                else
                {
                    _GPIO_W_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask, m_stPadMuxEntry[u32PadID].padmux[i].mask);
                }
            }
        }
    }

    return (u8ModeIsFind) ? 0 : -1;
}


static S32 HalPadSetMode_GeneralPMPad(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U16 i = 0, ExtItemID = 0;
    U32 SetBank = 0;

    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= ~(m_stPadMuxEntry[u32PadID].padmux[i].mask);
            u16RegVal |= m_stPadMuxEntry[u32PadID].padmux[i].val; // CHECK Multi-Pad Mode
            _GPIO_W_WORD_MASK(u32RegAddr, u16RegVal, 0xFFFF);
            u8ModeIsFind = 1;
            SetBank = m_stPadMuxEntry[u32PadID].padmux[i].base;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
                break;
#endif
            }
            else if ((m_stPadMuxEntry[u32PadID].padmux[i].mode >= PINMUX_FOR_PM_PAD_EXT_MODE_0_1) &&
                    (m_stPadMuxEntry[u32PadID].padmux[i].mode <= PINMUX_FOR_PM_PAD_EXT_MODE_10_1)) {
                ExtItemID = i;
            }
            else
            {
                if(u32Mode == PINMUX_FOR_GPIO_MODE)
                    continue;
                u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask);
                if (u16RegVal == m_stPadMuxEntry[u32PadID].padmux[i].val)
                {
                    printk(KERN_INFO"[Padmux]reset PAD%d(reg 0x%x:%x; mask0x%x) t0 %s (org: %s)\n",
                           u32PadID,
                           m_stPadMuxEntry[u32PadID].padmux[i].base,
                           m_stPadMuxEntry[u32PadID].padmux[i].offset,
                           m_stPadMuxEntry[u32PadID].padmux[i].mask,
                           m_stPadModeInfoTbl[u32Mode].u8PadName,
                           m_stPadModeInfoTbl[m_stPadMuxEntry[u32PadID].padmux[i].mode].u8PadName);
                    if (m_stPadMuxEntry[u32PadID].padmux[i].val != 0)
                    {
                        _GPIO_W_WORD_MASK(u32RegAddr, 0, m_stPadMuxEntry[u32PadID].padmux[i].mask);
                    }
                    else
                    {
                        _GPIO_W_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask, m_stPadMuxEntry[u32PadID].padmux[i].mask);
                    }
                }
            }
    }

    if (u8ModeIsFind && ((SetBank >> 8) > 0x1000)) {
        // set external data mode
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[ExtItemID].base, m_stPadMuxEntry[u32PadID].padmux[ExtItemID].offset);
        u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
        u16RegVal &= ~(m_stPadMuxEntry[u32PadID].padmux[ExtItemID].mask);
        u16RegVal |= m_stPadMuxEntry[u32PadID].padmux[ExtItemID].val; // CHECK Multi-Pad Mode
        _GPIO_W_WORD_MASK(u32RegAddr, u16RegVal, 0xFFFF);
    }

    return (u8ModeIsFind) ? 0 : -1;
}

static S32 HalPadSetMode_MISC(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U16 i = 0;

    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= ~(m_stPadMuxEntry[u32PadID].padmux[i].mask);
            u16RegVal |= m_stPadMuxEntry[u32PadID].padmux[i].val; // CHECK Multi-Pad Mode
            _GPIO_W_WORD_MASK(u32RegAddr, u16RegVal, 0xFFFF);
            u8ModeIsFind = 1;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
            break;
#endif
        }
        else
        {
            continue;
        }
    }

    return (u8ModeIsFind) ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : HalPadSetVal
//  Description :
//------------------------------------------------------------------------------
S32 HalPadSetVal(U32 u32PadID, U32 u32Mode)
{
    if (FALSE == _HalCheckPin(u32PadID)) {
        return FALSE;
    }
    if(u32PadID >= PAD_ETH_RN && u32PadID <= PAD_SAR_GPIO3)
	{
        return HalPadSetMode_MISC(u32PadID, u32Mode);
	}
    if(u32PadID >= PAD_PM_SD_CDZ && u32PadID <= PAD_PM_LED1) {
        return HalPadSetMode_GeneralPMPad(u32PadID, u32Mode);
	}
	else
	{
    return HalPadSetMode_General(u32PadID, u32Mode);
	}
}

//------------------------------------------------------------------------------
//  Function    : HalPadSet
//  Description :
//------------------------------------------------------------------------------
S32 HalPadSetMode(U32 u32Mode)
{
    U32 u32PadID;
    U16 k = 0;
    U16 i = 0;
    for (k = 0; k < sizeof(m_stPadMuxEntry) / sizeof(struct stPadMuxEntry); k++)
    {
        for (i = 0; i < m_stPadMuxEntry[k].size; i++)
        {
            if (u32Mode == m_stPadMuxEntry[k].padmux[i].mode)
            {
                u32PadID = m_stPadMuxEntry[k].padmux[i].padID;
                if(u32PadID >= PAD_ETH_RN && u32PadID <= PAD_SAR_GPIO3)
                {
                    if (HalPadSetMode_MISC(u32PadID, u32Mode) < 0)
                    {
                        return -1;
                    }
                }
                else if(u32PadID >= PAD_PM_SD_CDZ && u32PadID <= PAD_PM_LED1) {
                    if (HalPadSetMode_GeneralPMPad(u32PadID, u32Mode) < 0)
                    {
                        return -1;
                    }
                }
                else
                {
                    if (HalPadSetMode_General(u32PadID, u32Mode) < 0)
                    {
                        return -1;
                    }
                }

            }
        }
    }
    return 0;
}

S32 _HalPadCheckDisablePadMux(U32 u32PadModeID)
{
    U16 u16RegVal = 0;

    u16RegVal = _GPIO_R_WORD_MASK(m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU, m_stPadModeInfoTbl[u32PadModeID].u16ModeMask);
    if(u16RegVal != 0)
    {
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)((m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU & 0xFFFF00) >> 9);
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = (U16)((m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU & 0x1FF) >> 2);
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadModeInfoTbl[u32PadModeID].u16ModeMask;
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = 0;
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
        m_stPadCheckVal.infocount ++;
        return -1;
    }
    return 0;
}

S32 _HalPadCheckEnablePadMux(U32 u32PadModeID)
{
    U16 u16RegVal = 0;

    u16RegVal = _GPIO_R_WORD_MASK(m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU, m_stPadModeInfoTbl[u32PadModeID].u16ModeMask);
    if(u16RegVal != m_stPadModeInfoTbl[u32PadModeID].u16ModeVal)
    {
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)((m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU & 0xFFFF00) >> 9);
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = (U16)((m_stPadModeInfoTbl[u32PadModeID].u32ModeRIU & 0x1FF) >> 2);
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadModeInfoTbl[u32PadModeID].u16ModeMask;
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadModeInfoTbl[u32PadModeID].u16ModeVal;
        m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
        m_stPadCheckVal.infocount ++;
        return -1;
    }
    return 0;
}

static S32 HalPadCheckMode_General(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U8  u8ModeIsErr  = 0;
    U16 i = 0;
    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= (m_stPadMuxEntry[u32PadID].padmux[i].mask);

            if (u16RegVal != m_stPadMuxEntry[u32PadID].padmux[i].val) // CHECK Multi-Pad Mode
            {
                u8ModeIsErr ++;

                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[i].base >> 8);
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[i].offset;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[i].mask;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[i].val;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
                m_stPadCheckVal.infocount ++;
            }
            u8ModeIsFind = 1;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
            break;
#endif
        }
        else
        {
            if(u32Mode == PINMUX_FOR_GPIO_MODE)
                continue;
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask);
            if (u16RegVal == m_stPadMuxEntry[u32PadID].padmux[i].val)
            {
                u8ModeIsErr ++;

                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[i].base >> 8);
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[i].offset;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[i].mask;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[i].val;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
                m_stPadCheckVal.infocount ++;
            }
        }
    }

    return (u8ModeIsFind && !u8ModeIsErr) ? 0 : -1;
}

static S32 HalPadCheckMode_GeneralPMPad(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U8  u8ModeIsErr  = 0;
    U16 i = 0, ExtItemID = 0;
    U32 SetBank = 0;

#ifdef PAD_PM_IRIN_PATCH
    if((u32PadID == PAD_PM_IRIN) && (u32Mode == PINMUX_FOR_GPIO_MODE))
    {
        if(_HalPadCheckEnablePadMux(PINMUX_FOR_PM_IR_IN_MODE_1))
            u8ModeIsErr ++;
    }
#endif

    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= (m_stPadMuxEntry[u32PadID].padmux[i].mask);
            if(u16RegVal != m_stPadMuxEntry[u32PadID].padmux[i].val)
            {
                u8ModeIsErr ++;

                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[i].base >> 8);
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[i].offset;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[i].mask;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[i].val;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
                m_stPadCheckVal.infocount ++;
            }
            u8ModeIsFind = 1;
            SetBank = m_stPadMuxEntry[u32PadID].padmux[i].base;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
            break;
#endif
        }
        else if ((m_stPadMuxEntry[u32PadID].padmux[i].mode >= PINMUX_FOR_PM_PAD_EXT_MODE_0_1) &&
                    (m_stPadMuxEntry[u32PadID].padmux[i].mode <= PINMUX_FOR_PM_PAD_EXT_MODE_10_1)) {
                ExtItemID = i;
            }
        else
        {
            if(u32Mode == PINMUX_FOR_GPIO_MODE)
                continue;
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, m_stPadMuxEntry[u32PadID].padmux[i].mask);
            if (u16RegVal == m_stPadMuxEntry[u32PadID].padmux[i].val)
            {
                u8ModeIsErr ++;

                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[i].base >> 8);
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[i].offset;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[i].mask;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[i].val;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
                m_stPadCheckVal.infocount ++;
            }
        }
    }

    if (u8ModeIsFind && ((SetBank >> 8) > 0x1000)) {
        // set external data mode
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[ExtItemID].base, m_stPadMuxEntry[u32PadID].padmux[ExtItemID].offset);
        u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
        u16RegVal &= (m_stPadMuxEntry[u32PadID].padmux[ExtItemID].mask);
        if(u16RegVal != m_stPadMuxEntry[u32PadID].padmux[ExtItemID].val)
        {
            u8ModeIsErr ++;

            m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[ExtItemID].base >> 8);
            m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[ExtItemID].offset;
            m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[ExtItemID].mask;
            m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[ExtItemID].val;
            m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
            m_stPadCheckVal.infocount ++;
        }
    }

    return (u8ModeIsFind && !u8ModeIsErr) ? 0 : -1;
}

static S32 HalPadCheckMode_MISC(U32 u32PadID, U32 u32Mode)
{
    U32 u32RegAddr = 0;
    U16 u16RegVal  = 0;
    U8  u8ModeIsFind = 0;
    U8  u8ModeIsErr  = 0;
    U16 i = 0;

    for (i = 0; i < m_stPadMuxEntry[u32PadID].size; i++)
    {
        u32RegAddr = _RIUA_16BIT(m_stPadMuxEntry[u32PadID].padmux[i].base, m_stPadMuxEntry[u32PadID].padmux[i].offset);
        if (u32Mode == m_stPadMuxEntry[u32PadID].padmux[i].mode)
        {
            u16RegVal = _GPIO_R_WORD_MASK(u32RegAddr, 0xFFFF);
            u16RegVal &= (m_stPadMuxEntry[u32PadID].padmux[i].mask);
            if(u16RegVal != m_stPadMuxEntry[u32PadID].padmux[i].val)
            {
                u8ModeIsErr ++;

                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].base   = (U16)(m_stPadMuxEntry[u32PadID].padmux[i].base >> 8);
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].offset = m_stPadMuxEntry[u32PadID].padmux[i].offset;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].mask   = m_stPadMuxEntry[u32PadID].padmux[i].mask;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].val    = m_stPadMuxEntry[u32PadID].padmux[i].val;
                m_stPadCheckVal.infos[m_stPadCheckVal.infocount].regval = u16RegVal;
                m_stPadCheckVal.infocount ++;
            }
            u8ModeIsFind = 1;
#if (ENABLE_CHECK_ALL_PAD_CONFLICT == 0)
                break;
#endif
        }
        else
        {
            continue;
        }
    }
    return (u8ModeIsFind && !u8ModeIsErr) ? 0 : -1;
}

S32 HalPadCheckVal(U32 u32PadID, U32 u32Mode)
{
    if (FALSE == _HalCheckPin(u32PadID)) {
        return FALSE;
    }

    memset(&m_stPadCheckVal, 0, sizeof(m_stPadCheckVal));

    if(u32PadID >= PAD_ETH_RN && u32PadID <= PAD_SAR_GPIO3)
    {
        return HalPadCheckMode_MISC(u32PadID, u32Mode);
    }
    if (u32PadID >= PAD_PM_SD_CDZ && u32PadID <= PAD_PM_LED1) {
        return HalPadCheckMode_GeneralPMPad(u32PadID, u32Mode);
    }
    else
    {
        return HalPadCheckMode_General(u32PadID, u32Mode);
    }

}

U8 HalPadCheckInfoCount(void)
{
    return m_stPadCheckVal.infocount;
}

void* HalPadCheckInfoGet(U8 u8Index)
{
    return (void*)&m_stPadCheckVal.infos[u8Index];
}

int HalPadModeToVal(U8 *pu8Mode)
{
    U16 index;
    for (index = 0; index < (sizeof(m_stPadModeInfoTbl)/sizeof(m_stPadModeInfoTbl[0])); index++)
    {
        if(!strcmp(m_stPadModeInfoTbl[index].u8PadName, pu8Mode))
        {
            return index;
        }
    }
    return -1;
}
