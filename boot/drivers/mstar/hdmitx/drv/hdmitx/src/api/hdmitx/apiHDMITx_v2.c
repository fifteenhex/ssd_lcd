/*
* apiHDMITx_v2.c- Sigmastar
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

#include "mhal_common.h"
#include "apiHDMITx.h"
#include "drvHDMITx.h"
#include "HDMITX_private.h"


/*********************************************************************/
/*                                                                                                                     */
/*                                                      Defines                                                    */
/*                                                                                                                     */
/*********************************************************************/
#define EN_HDMITX_V2_DBG                (0)

#if (EN_HDMITX_V2_DBG == 1)
#define DBG_HDMITX_V2(_f)                  _f
#else
#define DBG_HDMITX_V2(_f)
#endif





