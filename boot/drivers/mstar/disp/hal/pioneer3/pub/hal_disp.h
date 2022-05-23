/*
* hal_disp.h- Sigmastar
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

#ifndef _HAL_DISP_H_
#define _HAL_DISP_H_

#include "hal_disp_op2.h"
#include "hal_disp_mace.h"
#include "hal_disp_color.h"
#include "hal_disp_mop.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef struct
{
    bool bTgenUpdate[HAL_DISP_DEVICE_MAX];
    bool bInitReset[HAL_DISP_DEVICE_MAX];
    void *pvCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM];
}HalDispHwContain_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef __HAL_DISP_C__
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE bool HalDispGetCmdqByCtx(void *pCtx, void **pCmdqCtx);

#undef INTERFACE

#endif
