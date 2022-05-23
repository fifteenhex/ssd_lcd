/*
* drv_disp_irq.h- Sigmastar
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

#ifndef _DRV_DISP_IRQ_H_
#define _DRV_DISP_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------


#ifdef _DRV_DISP_IRQ_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
INTERFACE void DrvDispIrqSetIsrNum(u32 u32DevId, u32 u32IsrNum);
INTERFACE bool DrvDispIrqGetIsrNum(void *pDevCtx, u32 *pu32IsrNum);
INTERFACE bool DrvDispIrqGetIsrNumByDevId(u32 u32DevId, u32 *pu32IsrNum);
INTERFACE bool DrvDispIrqEnable(void *pDevCtx, u32 u32DevIrq, bool bEnable);
INTERFACE bool DrvDispIrqGetFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);
INTERFACE bool DrvDispIrqClear(void *pDevCtx, void* pData);

INTERFACE bool DrvDispIrqCreateInternalIsr(void *pDispCtx);
INTERFACE bool DrvDispIrqDestroyInternalIsr(void *pDispCtx);
INTERFACE u16  DrvDispIrqGetIrqCount(void);

#undef INTERFACE
#endif
