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
#ifdef __cplusplus
extern "C"
{
#endif
#include <drv_vcm_common.h>
#ifdef __cplusplus
}
#endif

VCM_DRV_ENTRY_IMPL_BEGIN(AD5820);

// ============================================================================

#define VcmReg_Read(_reg, _data)    (pVcmCusHandle->i2c_bus->i2c_rx(pVcmCusHandle->i2c_bus, &(pVcmCusHandle->i2c_cfg), _reg, _data))
#define VcmReg_Write(_reg, _data)   (pVcmCusHandle->i2c_bus->i2c_tx(pVcmCusHandle->i2c_bus, &(pVcmCusHandle->i2c_cfg), _reg, _data))
#define VcmRegArrayR(_reg, _len)    (pVcmCusHandle->i2c_bus->i2c_array_rx(pVcmCusHandle->i2c_bus, &(pVcmCusHandle->i2c_cfg), (_reg), (_len)))
#define VcmRegArrayW(_reg, _len)    (pVcmCusHandle->i2c_bus->i2c_array_tx(pVcmCusHandle->i2c_bus, &(pVcmCusHandle->i2c_cfg), (_reg), (_len)))

#define VCM_I2C_LEGACY  I2C_NORMAL_MODE
#define VCM_I2C_FMT     I2C_FMT_A8D8
#define VCM_I2C_SPEED   200000
#define VCM_I2C_ADDR    0x18

#define MAX_POS 1023
#define MIN_POS 0

u32 g_cur_pos = 0;

static int pCus_vcm_SetPos(ms_cus_vcm *pVcmCusHandle, u32 pos);
// ============================================================================

static int pCus_vcm_PowerOn(ms_cus_vcm *pVcmCusHandle, u32 idx)
{
    return SUCCESS;
}

static int pCus_vcm_PowerOff(ms_cus_vcm *pVcmCusHandle, u32 idx)
{
    return SUCCESS;
}

static int pCus_vcm_Init(ms_cus_vcm *pVcmCusHandle)
{
    //Reset VCM position
    pCus_vcm_SetPos(pVcmCusHandle, 0);
    return SUCCESS;
}

static int pCus_vcm_SetPos(ms_cus_vcm *pVcmCusHandle, u32 pos)
{
    u8 byte1 = (pos>>4) & 0x3F;
    u8 byte2 = (pos<<4) & 0xF0;

    if (pos > MAX_POS)
        return FAIL;

    VcmReg_Write(byte1, byte2);

    g_cur_pos = pos;

    return SUCCESS;
}

static int pCus_vcm_GetCurPos(ms_cus_vcm *pVcmCusHandle, u32 *cur_pos)
{
    *cur_pos = g_cur_pos;
    return SUCCESS;
}

static int pCus_vcm_GetMinMaxPos(ms_cus_vcm *pVcmCusHandle, u32 *min_pos, u32 *max_pos)
{
    *min_pos = MIN_POS;
    *max_pos = MAX_POS;
    return SUCCESS;
}

int cus_vcm_init_handle(ms_cus_vcm* drv_handle)
{
    ms_cus_vcm *pVcmCusHandle = drv_handle;

    if (!pVcmCusHandle) {
        VCM_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    VCM_DMSG("[%s]", __FUNCTION__);

    // i2c
    pVcmCusHandle->i2c_cfg.mode = VCM_I2C_LEGACY;
    pVcmCusHandle->i2c_cfg.fmt = VCM_I2C_FMT;
    pVcmCusHandle->i2c_cfg.speed = VCM_I2C_SPEED;
    pVcmCusHandle->i2c_cfg.address = VCM_I2C_ADDR;

    pVcmCusHandle->pCus_vcm_PowerOn = pCus_vcm_PowerOn;
    pVcmCusHandle->pCus_vcm_PowerOff = pCus_vcm_PowerOff;
    pVcmCusHandle->pCus_vcm_Init = pCus_vcm_Init;
    pVcmCusHandle->pCus_vcm_SetPos = pCus_vcm_SetPos;
    pVcmCusHandle->pCus_vcm_GetCurPos = pCus_vcm_GetCurPos;
    pVcmCusHandle->pCus_vcm_GetMinMaxPos = pCus_vcm_GetMinMaxPos;

    return SUCCESS;
}

VCM_DRV_ENTRY_IMPL_END(AD5820, cus_vcm_init_handle);
