/*
* uart_padmux.c- Sigmastar
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
/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <common.h>
#include <command.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_serial.h"
#include <linux/compiler.h>
#include <serial.h>

#define MS_BASE_REG_UART2_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110A00)
#define MS_BASE_REG_UART1_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110900)
#define MS_BASE_REG_FUART_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110200)

#if 1
#define REG_FUART_SEL                  0x1F203D4C /*0x101E, h53*/
#define REG_UART1_SEL                  0x1F203D4C /*0x101E, h53*/
#define REG_UART2_SEL                  0x1F203D50 /*0x101E, h54*/

#define UART_PIU_FUART                 1 /*select controller for fuart, 0x101E, h53 BIT[7:4]*/
#define UART_PIU_UART1                 3 /*select controller for fuart, 0x101E, h53 BIT[15:12]*/
#define UART_PIU_UART2                 4 /*select controller for fuart, 0x101E, h54 BIT[3:0]*/

#define REG_FUART_CLK                  0x1F2070D0 /*0x1038, h34 BIT[3:0]*/
#define REG_UART1_CLK                  0x1F2070C4 /*0x1038, h31 BIT[11:8]*/
#define REG_UART2_CLK                  0x1F2070C4 /*0x1038, h31 BIT[15:12]*/

#define REG_FUART_PADMUX               0x1F2079B8 /*0x103C,offset:0x6e BIT[11:8]*/
#define REG_UART1_PADMUX               0x1F2079B4 /*0x103C,offset:0x6d BIT[7:4]*/
#define REG_UART2_PADMUX               0x1F2079B4 /*0x103C,offset:0x6d BIT[11:8]*/

//select uart mode
#define CONFIG_FUART_PAD_MODE          6 /*PINMUX_FOR_FUART_MODE_6*/
#define CONFIG_UART1_PAD_MODE          8 /*PINMUX_FOR_UART1_MODE_8*/
#define CONFIG_UART2_PAD_MODE          6 /*PINMUX_FOR_UART2_MODE_6*/
#endif

extern U32 uart_multi_base;

U32 ms_uart_padmux(U8 u8_Port)
{
    U32 uartClk = 0;

    printf("TODO: uart padmux\r\n");
    switch(u8_Port)
    {
        case 1:
            uart_multi_base = MS_BASE_REG_UART1_PA;
            /*clk*/
            uartClk = 172800000;
            /*padmux*/
            OUTREGMSK16(REG_UART1_CLK, 0x00 << 8, 0xF << 8);    //reg_uart_sel3
            /*padmux*/
            OUTREGMSK16(REG_UART1_PADMUX, CONFIG_UART1_PAD_MODE << 4, 0xF << 4);
            OUTREGMSK16(REG_UART1_SEL, UART_PIU_UART1 << 12, 0xF << 12);
            printf("reg[%x] = %x\n", REG_UART1_PADMUX, INREG16(REG_UART1_PADMUX));
            break;
        case 2:
            uart_multi_base = MS_BASE_REG_UART2_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_UART2_CLK, 0x00 << 12, 0xF << 12);    //reg_uart_sel4
            /*padmux*/
            OUTREGMSK16(REG_UART2_PADMUX, CONFIG_UART2_PAD_MODE << 8, 0x7 << 8);
            OUTREGMSK16(REG_UART2_SEL, UART_PIU_UART2 << 0, 0xF << 0);
            printf("reg[%x] = %x\n", REG_UART2_PADMUX, INREG16(REG_UART2_PADMUX));
            break;
        case 3:
            uart_multi_base = MS_BASE_REG_FUART_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_FUART_CLK, 0x00 << 0, 0xF << 0);    //reg_uart_sel1
            /*padmux*/
            OUTREGMSK16(REG_FUART_PADMUX, CONFIG_FUART_PAD_MODE << 8, 0xF << 8);
            OUTREGMSK16(REG_FUART_SEL, UART_PIU_FUART << 4, 0xF << 4);
            printf("reg[%x] = %x\n", REG_FUART_PADMUX, INREG16(REG_FUART_PADMUX));
            break;
        default:
            uart_multi_base = MS_BASE_REG_UART1_PA;
            /*clk*/
            uartClk = 172800000;
            /*padmux*/
            OUTREGMSK16(REG_UART1_CLK, 0x00 << 8, 0xF << 8);    //reg_uart_sel3
            /*padmux*/
            OUTREGMSK16(REG_UART1_PADMUX, CONFIG_UART1_PAD_MODE << 4, 0xF << 4);
            OUTREGMSK16(REG_UART1_SEL, UART_PIU_UART1 << 12, 0xF << 12);
            printf("invalid port, use default uart1\n");
            break;
    }
    return uartClk;
}
