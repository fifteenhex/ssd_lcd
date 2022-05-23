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

#ifndef DRV_MS_CUS_VCM_H_
#define DRV_MS_CUS_VCM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sensor_i2c_api.h>

#ifndef SUCCESS
#define FAIL        (-1)
#define SUCCESS     0
#endif

typedef enum {
    CUS_VCM_POL_POS = 0,    /**< High active */
    CUS_VCM_POL_NEG         /**< Low active */
} CUS_VCM_POL;

typedef struct __VcmIfAPI
{

    /** @brief Set vcm power down pin.
    @param[in] idx Sensor pad ID.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*PowerOff)(u32 idx, CUS_VCM_POL pol);

    /** @brief Set vcm power reset pin.
    @param[in] idx Sensor pad ID.
    @param[in] pol pin polarity.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*Reset)(u32 idx, CUS_VCM_POL pol);

}VcmIfAPI;

typedef struct __ms_cus_vcm{

    char model_id[32];      /**< Please fill the vcm model id string.*/
    void *private_data;     /**< vcm driver dependent variables should store in private_data and free when release */

    app_i2c_cfg i2c_cfg;           /**< vcm i2c setting */
    i2c_handle_t *i2c_bus;         /**< Handle to vcm i2c API. */
    VcmIfAPI *vcm_if_api;         /**< Handle to vcm interface API. */

    /** @brief vcm power on
    @param[in] handle: Handle to vcm driver.
    @param[in] idx: Sensor pad ID.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*pCus_vcm_PowerOn)(struct __ms_cus_vcm* handle, u32 idx);

    /** @brief vcm power off
    @param[in] handle: Handle to vcm driver.
    @param[in] idx: Sensor pad ID.
    @retval SUCCESS or FAIL if error occurs.
    */
    int (*pCus_vcm_PowerOff)(struct __ms_cus_vcm* handle, u32 idx);

    /** @brief vcm initialization
    @param[in] handle: Handle to vcm driver.
    @retval: SUCCESS or FAIL if error occurs.
    @remark: Fill vcm initial table here.
    */
    int (*pCus_vcm_Init)(struct __ms_cus_vcm* handle);

    /** @brief Set a vcm position
    @param[in] handle: Handle to vcm driver.
    @param[in] pos: Set position.
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_vcm_SetPos)(struct __ms_cus_vcm* handle, u32 pos);

    /** @brief Get vcm current position
    @param[in] handle: Handle to vcm driver.
    @param[out] cur_pos: current position.
    @retval Return SUCCESS or FAIL if error occurs.
    */
    int (*pCus_vcm_GetCurPos)(struct __ms_cus_vcm* handle, u32 *cur_pos);

    /** @brief Get supported vcm position range
    @param[in] handle: Handle to vcm driver.
    @param[out] min_pos: Receive minimum position which vcm can supported
    @param[out] min_pos: Receive maxiimum position which vcm can supported
    @retval Return SUCCESS or FAIL if error occurs.
    @remark position 0~2^n-1
    */
    int (*pCus_vcm_GetMinMaxPos)(struct __ms_cus_vcm* handle, u32 *min_pos, u32 *max_pos);

} ms_cus_vcm;

typedef int (*VcmInitHandle)(ms_cus_vcm* handle);

#ifdef __cplusplus
}
#endif

#endif /* DRV_MS_CUS_VCM_H_ */
