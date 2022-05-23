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

#ifndef DRV_VCM_H
#define DRV_VCM_H

#include <cam_os_wrapper.h>

#define VCM_DMSG(args...) do{}while(0)
#define VCM_EMSG(args...) CamOsPrintf(args)
#define VCM_IMSG(args...) CamOsPrintf(args)

extern s32 DrvRegisterVcmDriver(u32 nSNRPadID, VcmInitHandle pfnVcmInitHandle);
extern s32 DrvReleaseVcmDriver(u32 nCamID);

#define SENSOR_USLEEP(us) CamOsUsSleep(us)
#define SENSOR_MSLEEP(ms) CamOsMsSleep(ms)
#define SENSOR_UDELAY(us) CamOsUsDelay(us)
#define SENSOR_MDELAY(ms) CamOsMsDelay(ms)
#endif
