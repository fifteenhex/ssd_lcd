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
 
#include <cam_os_wrapper.h>

#if defined(_VCM_DW9714_)
extern int  DW9714_init_driver(unsigned char chmap);
#endif

void rtk_vcm_module_init(void)
{
#if defined(_VCM_DW9714_)
    #if (_VCM_DW9714_ == 0)
        DW9714_init_driver(_VCM0_CHMAP_);
    #elif (_VCM_DW9714_ == 1)
        DW9714_init_driver(_VCM1_CHMAP_);
    #endif
#endif
}
