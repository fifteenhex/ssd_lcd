/*
* hal_pwm.c- Sigmastar
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
#include "MsTypes.h"
#include <common.h>
#include "hal_pwm.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "../drivers/mstar/gpio/pioneer3/gpio.h"
#include "../drivers/mstar/gpio/pioneer3/padmux.h"
extern S32 HalPadSetVal(U32 u32PadID, U32 u32Mode);

static pwmPadTbl_t padTbl_0[] =
{
    {PAD_SR_IO00,     PINMUX_FOR_PWM0_MODE_1},
    {PAD_SR_IO04,     PINMUX_FOR_PWM0_MODE_2},
    {PAD_SR_IO08,     PINMUX_FOR_PWM0_MODE_3},
    {PAD_TTL4,        PINMUX_FOR_PWM0_MODE_4},
    {PAD_TTL14,       PINMUX_FOR_PWM0_MODE_5},
    {PAD_TTL18,       PINMUX_FOR_PWM0_MODE_6},
    {PAD_KEY0,      PINMUX_FOR_PWM0_MODE_7},
    {PAD_KEY10,     PINMUX_FOR_PWM0_MODE_8},
    {PAD_SD_CMD,    PINMUX_FOR_PWM0_MODE_9},
    {PAD_GPIO1,     PINMUX_FOR_PWM0_MODE_10},
    {PAD_UNKNOWN, BASE_REG_NULL},
    {PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_1[] =
{
    {PAD_SR_IO01,     PINMUX_FOR_PWM1_MODE_1},
    {PAD_SR_IO05,     PINMUX_FOR_PWM1_MODE_2},
    {PAD_SR_IO09,     PINMUX_FOR_PWM1_MODE_3},
    {PAD_TTL5,        PINMUX_FOR_PWM1_MODE_4},
    {PAD_TTL15,       PINMUX_FOR_PWM1_MODE_5},
    {PAD_TTL19,     PINMUX_FOR_PWM1_MODE_6},
    {PAD_KEY1,      PINMUX_FOR_PWM1_MODE_7},
    {PAD_KEY11,     PINMUX_FOR_PWM1_MODE_8},
    {PAD_SD_D3,     PINMUX_FOR_PWM1_MODE_9},
    {PAD_GPIO2,     PINMUX_FOR_PWM1_MODE_10},
    {PAD_UNKNOWN, BASE_REG_NULL},
    {PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_2[] =
{
    {PAD_SR_IO02,     PINMUX_FOR_PWM2_MODE_1},
    {PAD_SR_IO06,     PINMUX_FOR_PWM2_MODE_2},
    {PAD_SR_IO10,     PINMUX_FOR_PWM2_MODE_3},
    {PAD_TTL6,        PINMUX_FOR_PWM2_MODE_4},
    {PAD_TTL16,       PINMUX_FOR_PWM2_MODE_5},
    {PAD_TTL20,     PINMUX_FOR_PWM2_MODE_6},
    {PAD_KEY2,      PINMUX_FOR_PWM2_MODE_7},
    {PAD_KEY12,     PINMUX_FOR_PWM2_MODE_8},
    {PAD_SD_D2,     PINMUX_FOR_PWM2_MODE_9},
    {PAD_GPIO3,     PINMUX_FOR_PWM2_MODE_10},
    {PAD_UNKNOWN, BASE_REG_NULL},
    {PAD_UNKNOWN, BASE_REG_NULL},

};

static pwmPadTbl_t padTbl_3[] =
{
    {PAD_SR_IO03,     PINMUX_FOR_PWM3_MODE_1},
    {PAD_SR_IO07,     PINMUX_FOR_PWM3_MODE_2},
    {PAD_SR_IO11,     PINMUX_FOR_PWM3_MODE_3},
    {PAD_TTL7,        PINMUX_FOR_PWM3_MODE_4},
    {PAD_TTL17,       PINMUX_FOR_PWM3_MODE_5},
    {PAD_TTL21,     PINMUX_FOR_PWM3_MODE_6},
    {PAD_KEY3,      PINMUX_FOR_PWM3_MODE_7},
    {PAD_KEY13,     PINMUX_FOR_PWM3_MODE_8},
    {PAD_SD_GPIO0,  PINMUX_FOR_PWM3_MODE_9},
    {PAD_GPIO4,    PINMUX_FOR_PWM3_MODE_10},
    {PAD_UNKNOWN, BASE_REG_NULL},
    {PAD_UNKNOWN, BASE_REG_NULL},
};
static pwmPadTbl_t* padTbl[] =
{
    padTbl_0,
    padTbl_1,
    padTbl_2,
    padTbl_3,
};
void halPWMPadSet(U8 u8Id, U32 u32Val)
{
    pwmPadTbl_t* pTbl = NULL;

    if (PWM_NUM <= u8Id)
    {
        return;
    }

    printf("[%s][%d] (pwmId, padId) = (%d, %d)\n", __FUNCTION__, __LINE__, u8Id, u32Val);

    pTbl = padTbl[u8Id];
    while (1)
    {
        if (u32Val == pTbl->u32PadId)
        {
            if (BASE_REG_NULL != pTbl->u32Mode)
            {
                HalPadSetVal(u32Val, pTbl->u32Mode);
            }
            break;
        }
        if (PAD_UNKNOWN == pTbl->u32PadId)
        {
            printf("[%s][%d] void DrvPWMEnable error!!!! (%x, %x)\r\n", __FUNCTION__, __LINE__, u8Id, u32Val);
            break;
        }
        pTbl++;
    }
}

int halGroupEnable(U8 u8GroupId, U8 u8Val)
{
    U16 u32JoinMask = 0X0000;
    if (PWM_GROUP_NUM <= u8GroupId)
        return 0;
    u32JoinMask |= 1 << (u8GroupId + PWM_NUM);
    
    if (u8Val)
    {
        SETREG16(BASE_REG_PWM_PA+ u16REG_GROUP_ENABLE, (1 << (u8GroupId + u16REG_GROUP_ENABLE_SHFT)));
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, u32JoinMask);
//        MDEV_PWM_SetSyncFlag(1); //dont need to sync until new data in
    }
    else
    {
        CLRREG16(BASE_REG_PWM_PA + u16REG_GROUP_ENABLE, (1 << (u8GroupId + u16REG_GROUP_ENABLE_SHFT)));
        SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, u32JoinMask);
    }
    return 1;
}
int halPWMGroupJoin(U8 u8PWMId, U8 u8Val)
{
    if (PWM_NUM <= u8PWMId)
        return 0;
    if(u8Val)
        SETREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    else
        CLRREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    return 1;
}
void halPWMAllGrpEnable(void)
{
    //dummy
}
void halPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    //dummy
}

