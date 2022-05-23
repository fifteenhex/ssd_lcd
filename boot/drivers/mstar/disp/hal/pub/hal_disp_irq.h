/*
* hal_disp_irq.h- Sigmastar
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

#ifndef _HAL_DISP_IRQ_H_
#define _HAL_DISP_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_IRQ_IOCTL_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_GET_ID,

    E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID,

    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS,

    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_STATUS,
    E_HAL_DISP_IRQ_IOCTL_NUM,
}HalDispIrqIoCtlType_e;

typedef enum
{
    E_HAL_DISP_IRQ_TYPE_NONE                    = 0x00000000,
    E_HAL_DISP_IRQ_TYPE_VSYNC                   = 0x00000001,
    E_HAL_DISP_IRQ_TYPE_VDE                     = 0x00000002,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE   = 0x00000100,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE   = 0x00000200,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE = 0x00000400,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE = 0x00000800,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON              = 0x00001000,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF             = 0x00002000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON           = 0x00004000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF          = 0x00008000,
}HalDispIrqType_e;

typedef enum
{
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE = 0x00,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_NEGATIVE = 0x01,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE   = 0x02,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE   = 0x03,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM            = 0x04,
}HalDispIrqTimeStampType_e;

typedef struct
{
    u32 u32IrqFlags;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM];
}HalDispIrqTimeZoneStatus_t;

typedef struct
{
    HalDispIrqIoCtlType_e enIoctlType;
    HalDispIrqType_e enIrqType;
    void *pDispCtx;
    void *pParam;
}HalDispIrqIoctlConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifdef _HAL_DISP_IRQ_C_
#define INTERFACE

#else
#define INTERFACE extern
#endif
INTERFACE bool HalDispIrqIoCtl(HalDispIrqIoctlConfig_t *pCfg);
INTERFACE void HalDispIrqSetDacEn(u8 u8Val);

#undef INTERFACE

#endif
