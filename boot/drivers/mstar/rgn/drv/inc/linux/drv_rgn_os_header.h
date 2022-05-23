/*
* drv_rgn_os_header.h- Sigmastar
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

#ifndef __DRV_RGN_OS_HEADER_H__
#define __DRV_RGN_OS_HEADER_H__

#include "cam_os_wrapper.h"
//=============================================================================
//  Defines & Macro
//=============================================================================
#define RGN_CMDQ_ONESHOTINIT(x)   CamOsTsemInit(&x.stSem,1)
#define RGN_CMDQ_ONESHOTDEINIT(x) CamOsTsemDeinit(&x.stSem)
#define RGN_CMDQ_ONESHOTLOCK(x)   CamOsTsemDown(&x.stSem)
#define RGN_CMDQ_ONESHOTUNLOCK(x) CamOsTsemUp(&x.stSem)

//=============================================================================
//  Structure & Emu
//=============================================================================
typedef struct
{
    CamOsTsem_t stSem;
}DrvRgnOsSemConfig_t;
#endif
