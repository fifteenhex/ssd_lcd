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

#ifndef _SENSOR_OS_WRAPPER_H_
#define _SENSOR_OS_WRAPPER_H_

#if defined(__KERNEL__)
#define CAM_OS_LINUX_KERNEL
#endif

#include <cam_os_wrapper.h>
#include <drv_ms_cus_vcm.h>
#include <drv_vcm.h>

/******************************************************** RTK ***********************************************************/
#if defined(CAM_OS_RTK)
typedef unsigned char bool;
#ifndef true
#define true                        1
#define false                       0
#endif

#define VCM_DRV_ENTRY_IMPL_BEGIN(NAME)

#define VCM_DRV_ENTRY_IMPL_END(Name,LinearEntry)\
int Name##_init_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    /*To avoid compile warning*/\
    void* p0 = LinearEntry;\
    for(nCamID=0;nCamID<4;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){ DrvRegisterVcmDriver(nCamID, LinearEntry);\
                VCM_DMSG("Connect %s vcm to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
        }\
    }\
    return 0;\
}

#define SENSOR_DRV_PARAM_MCLK() "NONE"
#define SENSOR_USLEEP_(us) {CamOsUsDelay(us);}
#define SENSOR_MSLEEP_(ms) {CamOsMsDelay(ms);}
#if 0
/*Extension version*/
#define VCM_DRV_ENTRY_IMPL_BEGIN_EX(NAME) VCM_DRV_ENTRY_IMPL_BEGIN(NAME)

#define MAX_CAMID_LEN 4
#define VCM_DRV_ENTRY_IMPL_END_EX(Name,LinearEntry,PrivateDataType)\
static PrivateDataType* g_pData[MAX_CAMID_LEN] = {0,0,0,0};\
static PrivateDataType g_PrivData[MAX_CAMID_LEN];\
int Name##_init_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    /*To avoid compile warning*/\
    void* p0 = LinearEntry;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){\
                /*void* pData = CamOsMemAlloc(sizeof(PrivateDataType));*/\
                void* pData = &g_PrivData[nCamID];/*Change private data to static var*/\
                memset(pData,0,sizeof(PrivateDataType));\
                DrvRegisterVcmDriverEx(nCamID, LinearEntry,pData);\
                SENSOR_DMSG("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
                g_pData[nCamID] = pData;\
            }\
        }\
    }\
    return 0;\
}\
\
int Name##_deinit_driver(unsigned char chmap)\
{\
    int nCamID=0;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(g_pData[nCamID])\
            {\
                DrvUnregisterVcmDriverEx(nCamID);\
                if(g_pData[nCamID]){\
                    /*CamOsMemRelease((void*)g_pData[0][nCamID]);*/\
                    g_pData[nCamID] = 0;\
                }\
            }\
        }\
    }\
    return 0;\
}
#endif
/******************************************************** LINUX ***********************************************************/
#elif defined(CAM_OS_LINUX_KERNEL)
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#define VCM_DRV_ENTRY_IMPL_BEGIN(Name) \
int chmap = 0;\
module_param(chmap, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);\
MODULE_PARM_DESC(chmap, "VCM mapping");


#define VCM_DRV_ENTRY_IMPL_END(Name,LinearEntry) \
static int __init Name##_init_driver(void)\
{\
    int nCamID=0;\
    void* p0 = LinearEntry;\
    for(nCamID=0;nCamID<4;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){ DrvRegisterVcmDriver(nCamID, LinearEntry);\
            pr_info("Connect %s vcm to sensor pad %d\n",__FUNCTION__, nCamID);\
            }\
        }\
    }\
    return 0;\
}\
static void __exit Name##_exit_driver(void)\
{\
    pr_info("vcm drv exit");\
}\
subsys_initcall(Name##_init_driver);\
module_exit(Name##_exit_driver);\
MODULE_DESCRIPTION("Vcm_"#Name);\
MODULE_AUTHOR("SigmaStar");\
MODULE_LICENSE("Proprietary");

#define SENSOR_USLEEP_(us) CamOsUsSleep(us)
#define SENSOR_MSLEEP_(ms) CamOsMsSleep(ms)

#if 0
/*Extension version*/
#define SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(Name) SENSOR_DRV_ENTRY_IMPL_BEGIN(Name)

#define MAX_CAMID_LEN 4
#define SENSOR_DRV_ENTRY_IMPL_END_EX(Name,LinearEntry,HdrSefEntry,HdrLefEntry,PrivateDataType) \
static PrivateDataType* g_pData[2][MAX_CAMID_LEN] = {{0,0,0,0}, {0,0,0,0}};\
static int __init Name##_init_driver(void)\
{\
    int nCamID=0;\
    void* p0 = LinearEntry;\
    void* p1 = HdrSefEntry;\
    void* p2 = HdrLefEntry;\
    if(DrvSensorHandleVer(CUS_CAMSENSOR_HANDLE_MAJ_VER, CUS_CAMSENSOR_HANDLE_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorIFVer(CUS_CAMSENSORIF_MAJ_VER, CUS_CAMSENSORIF_MIN_VER)==FAIL)\
    return FAIL;\
    if(DrvSensorI2CVer(CUS_CAMSENSOR_I2C_MAJ_VER, CUS_CAMSENSOR_I2C_MIN_VER)==FAIL)\
    return FAIL;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(p0){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                DrvRegisterSensorDriverEx(nCamID, LinearEntry,pData);\
                DrvRegisterSensorI2CSlaveID(nCamID, (i2c_slave_id>>(nCamID*8))&0xFF);\
                pr_info("Connect %s linear to sensor pad %d\n",__FUNCTION__, nCamID);\
                g_pData[0][nCamID] = pData;\
            }\
            if(p1||p2){\
                void* pData = CamOsMemAlloc(sizeof(PrivateDataType));\
                memset(pData,0,sizeof(PrivateDataType));\
                if(p1){\
                    DrvRegisterPlaneDriverEx(nCamID, 1, HdrSefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 1, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s SEF to vif sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                if(p2){\
                    DrvRegisterPlaneDriverEx(nCamID, 0, HdrLefEntry,pData);\
                    DrvRegisterPlaneI2CSlaveID(nCamID, 0, (i2c_slave_id>>(nCamID*8))&0xFF);\
                    pr_info("Connect %s LEF to sensor pad %d\n",__FUNCTION__, nCamID);\
                }\
                g_pData[1][nCamID] = pData;\
            }\
        }\
    }\
    return 0;\
}\
static void __exit Name##_exit_driver(void)\
{\
    int nCamID=0;\
    for(nCamID=0;nCamID<MAX_CAMID_LEN;++nCamID)\
    {\
        if((chmap>>nCamID)&0x1)\
        {\
            if(g_pData[0][nCamID] || g_pData[1][nCamID])\
            {\
                DrvUnregisterSensorDriverEx(nCamID);\
                if(g_pData[0][nCamID]){\
                    CamOsMemRelease((void*)g_pData[0][nCamID]);\
                    g_pData[0][nCamID] = 0;\
                }\
                if(g_pData[1][nCamID]){\
                    CamOsMemRelease((void*)g_pData[1][nCamID]);\
                    g_pData[1][nCamID] = 0;\
                }\
            }\
        }\
    }\
}\
subsys_initcall(Name##_init_driver);\
module_exit(Name##_exit_driver);\
MODULE_DESCRIPTION("Sensor_"#Name);\
MODULE_AUTHOR("SigmaStar");\
MODULE_LICENSE("Proprietary");
#endif
#endif //end of CAM_OS_LINUX_KERNEL


#endif //endif _SENSOR_OS_WRAPPER_H_
