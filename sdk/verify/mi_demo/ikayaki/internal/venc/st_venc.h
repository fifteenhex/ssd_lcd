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
#ifndef _ST_VENC_H_
#define _ST_VENC_H_

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mi_sys.h"
#include "mi_venc.h"

MI_S32 ST_Venc_CreateChannel(MI_VENC_CHN VencChn, MI_VENC_ChnAttr_t *pstAttr);
MI_S32 ST_Venc_DestoryChannel(MI_VENC_CHN VencChn);

MI_S32 ST_Venc_StartChannel(MI_VENC_CHN VencChn);
MI_S32 ST_Venc_StopChannel(MI_VENC_CHN VencChn);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif //_ST_VENC_H_