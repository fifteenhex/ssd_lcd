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

#if defined(_SENSOR_OS02G10_)
extern int OS02G10_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_IMX307_)
extern int IMX307_HDR_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5250_)
extern int PS5250_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PLECO_)
extern int pleco_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5520_)
extern int PS5520_init_driver(unsigned char chmap);
#endif
#if defined(_SENSOR_GC2053_)
extern int gc2053_init_driver(unsigned char chmap);
#endif
#if defined(_SENSOR_GC2053_1LANE_)
extern int gc2053_1lane_init_driver(unsigned char chmap);
#endif


#if defined(_SENSOR_GC1054_)
extern int gc1054_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_GC1054_DUAL_)
extern int gc1054_dual_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5268_)
extern int PS5268_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_IMX291_)
extern int IMX291_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_IMX323_)
extern int IMX323_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_IMX327_)
extern int IMX327_HDR_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_NVP6124B_)
extern int NVP6124B_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5270_)
extern int PS5270_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_SC4238_)
extern int SC4238_HDR_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_IMX415_)
extern int IMX415_HDR_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_F32_)
extern int F32_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5258_)
extern int PS5258_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_PS5260_)
extern int PS5260_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_SC210IOT_)
extern int  SC210iot_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_H66_)
extern int  H66_init_driver(unsigned char chmap);
#endif

#if defined(_SENSOR_EARLYINIT_)
void rtk_earlyinit_prepare(void)
{
#if defined(_SENSOR_IMX307_)
extern void IMX307_EarlyInitReg(unsigned int);
    #if (_SENSOR_IMX307_ == 0)
        IMX307_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX307_ == 1)
        IMX307_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5250_)
extern void PS5250_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5250_ == 0)
        PS5250_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5250_ == 1)
        PS5250_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5268_)
extern void PS5268_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5268_ == 0)
        PS5268_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5268_ == 1)
        PS5268_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5520_)
extern void PS5520_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5520_ == 0)
        PS5520_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5520_ == 1)
        PS5520_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5270_)
extern void PS5270_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5270_ == 0)
        PS5270_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5270_ == 1)
        PS5270_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_SC4238_)
extern void SC4238_EarlyInitReg(unsigned int);
    #if (_SENSOR_SC4238_ == 0)
        SC4238_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_SC4238_ == 1)
        SC4238_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_IMX415_)
extern void IMX415_EarlyInitReg(unsigned int);
    #if (_SENSOR_IMX415_ == 0)
        IMX415_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX415_ == 1)
        IMX415_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_F32_)
extern void F32_EarlyInitReg(unsigned int);
    #if (_SENSOR_F32_ == 0)
        F32_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_F32_ == 1)
        F32_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5258_)
extern void PS5258_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5258_ == 0)
        PS5258_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5258_ == 1)
        PS5258_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5260_)
extern void PS5260_EarlyInitReg(unsigned int);
    #if (_SENSOR_PS5260_ == 0)
        PS5260_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5260_ == 1)
        PS5260_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_SC210IOT_)
extern void SC210iot_EarlyInitReg(unsigned int);
    #if (_SENSOR_SC210IOT_ == 0)
        SC210iot_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_SC210IOT_ == 1)
        SC210iot_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_H66_)
extern void H66_EarlyInitReg(unsigned int);
    #if (_SENSOR_H66_ == 0)
        H66_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_H66_ == 1)
        H66_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_GC1054_)
extern int gc1054_EarlyInitReg(unsigned char chmap);
    #if (_SENSOR_GC1054_ == 0)
            gc1054_EarlyInitReg(_SENSOR0_CHMAP_);
    #elif (_SENSOR_GC1054_ == 1)
            gc1054_EarlyInitReg(_SENSOR1_CHMAP_);
    #endif
#endif


}
#endif

void rtk_sensor_module_init(void)
{

#if defined(_SENSOR_OS02G10_)
    #if (_SENSOR_OS02G10_ == 0)
        OS02G10_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_OS02G10_ == 1)
        OS02G10_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif


#if defined(_SENSOR_IMX307_)
    #if (_SENSOR_IMX307_ == 0)
        IMX307_HDR_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX307_ == 1)
        IMX307_HDR_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5250_)
    #if (_SENSOR_PS5250_ == 0)
        PS5250_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5250_ == 1)
        PS5250_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5520_)
    #if (_SENSOR_PS5520_ == 0)
        PS5520_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5520_ == 1)
        PS5520_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5268_)
    #if (_SENSOR_PS5268_ == 0)
        PS5268_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5268_ == 1)
        PS5268_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_IMX291_)
    #if (_SENSOR_IMX291_ == 0)
        IMX291_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX291_ == 1)
        IMX291_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_IMX323_)
    #if (_SENSOR_IMX323_ == 0)
        IMX323_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX323_ == 1)
        IMX323_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_IMX327_)
    #if (_SENSOR_IMX327_ == 0)
        IMX327_HDR_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX327_ == 1)
        IMX327_HDR_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_NVP6124B_)
    #if (_SENSOR_NVP6124B_ == 0)
        NVP6124B_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_NVP6124B_ == 1)
        NVP6124B_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5270_)
    #if (_SENSOR_PS5270_ == 0)
        PS5270_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5270_ == 1)
        PS5270_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_SC4238_)
    #if (_SENSOR_SC4238_ == 0)
        SC4238_HDR_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_SC4238_ == 1)
        SC4238_HDR_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_IMX415_)
    #if (_SENSOR_IMX415_ == 0)
        IMX415_HDR_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_IMX415_ == 1)
        IMX415_HDR_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_F32_)
    #if (_SENSOR_F32_ == 0)
        F32_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_F32_ == 1)
        F32_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_SC210IOT_)
    #if (_SENSOR_SC210IOT_ == 0)
          SC210iot_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_SC210IOT_ == 1)
          SC210iot_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_H66_)
    #if (_SENSOR_H66_ == 0)
          H66_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_H66_ == 1)
          H66_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5258_)
    #if (_SENSOR_PS5258_ == 0)
        PS5258_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5258_ == 1)
        PS5258_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_PS5260_)
    #if (_SENSOR_PS5260_ == 0)
        PS5260_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PS5260_ == 1)
        PS5260_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_GC1054_)
    #if (_SENSOR_GC1054_ == 0)
        gc1054_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_GC1054_ == 1)
        gc1054_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif

#if defined(_SENSOR_GC1054_DUAL_)
    #if (_SENSOR_GC1054_DUAL_ == 0)
        gc1054_dual_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_GC1054_DUAL_ == 1)
        gc1054_dual_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif
#if defined(_SENSOR_GC2053_)
    #if (_SENSOR_GC2053_ == 0)
        gc2053_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_GC2053_ == 1)
        gc2053_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif
#if defined(_SENSOR_GC2053_1LANE_)
    #if (_SENSOR_GC2053_1LANE_ == 0)
        gc2053_1lane_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_GC2053_1LANE_ == 1)
        gc2053_1lane_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif
#if defined(_SENSOR_PLECO_)
    #if (_SENSOR_PLECO_ == 0)
        pleco_init_driver(_SENSOR0_CHMAP_);
    #elif (_SENSOR_PLECO_ == 1)
        pleco_init_driver(_SENSOR1_CHMAP_);
    #endif
#endif
}
