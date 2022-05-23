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
#ifndef _ST_CUS3A_H_
#define _ST_CUS3A_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
int ae_init(void* pdata, ISP_AE_INIT_PARAM *init_state);
void ae_release(void* pdata);
void ae_run(void* pdata, const ISP_AE_INFO *info, ISP_AE_RESULT *result);
int awb_init(void *pdata);
void awb_run(void* pdata, const ISP_AWB_INFO *info, ISP_AWB_RESULT *result);
void awb_release(void *pdata);
int af_init(void *pdata, ISP_AF_INIT_PARAM *param);
void af_release(void *pdata);
void af_run(void *pdata, const ISP_AF_INFO *af_info, ISP_AF_RESULT *result);
int af_ctrl(void *pdata, ISP_AF_CTRL_CMD cmd, void* param);
*/
void ST_EnableCustomize3A(void);
void ST_ReleaseCUS3A(void);

#ifdef __cplusplus
}
#endif

#endif
