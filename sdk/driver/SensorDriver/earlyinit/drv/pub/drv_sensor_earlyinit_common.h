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

#ifndef _DRV_SENSOR_EARLYINIY_COMMON_H_
#define _DRV_SENSOR_EARLYINIY_COMMON_H_

#include <drv_sensor_earlyinit_method_1.h>
#include <drv_sensor_earlyinit_datatype.h>

typedef unsigned int (*ShutterFpsParser_fp)( unsigned int, unsigned int, I2cCfg_t *, unsigned int);
typedef unsigned int (*GainParser_fp)( unsigned int, I2cCfg_t *, unsigned int);
typedef unsigned int (*GetSnrInfo_fp)( EarlyInitSensorInfo_t* pSnrInfo);

typedef struct
{
    void *pInitTable;            //sensor EarlyInit data
    unsigned int nInitTableSize; //sensor EarlyInit data size
    ShutterFpsParser_fp fpShutterFpsParser;
    GainParser_fp fpGainParser;
    GetSnrInfo_fp fpGetSnrInfo;
    EarlyInitHwRes_t tHwRes; //Hardware resource
}EarlyInitEntry_t;

#define SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(Name)

#define SENSOR_EARLYINIY_ENTRY_IMPL_END(Name, EarlyInitTable, ShutterFpsParser, GainParser, GetSnrInfo) \
void Name##_EarlyInitReg(unsigned int nChMap)\
{\
    int n;\
    for(n=0;n<4;++n)\
    {\
        if( nChMap&(0x1<<n) )\
        {\
            DrvEarlyInitSetInitParam( n,\
                                      &EarlyInitTable[0],\
                                      sizeof(EarlyInitTable),\
                                      ShutterFpsParser,\
                                      GainParser,\
                                      GetSnrInfo\
            );\
        }\
    }\
}

void DrvEarlyInitSetInitParam(EARLY_INIT_SN_PAD_e eSensorPad,
                              void* pTable, unsigned int nTableSize,
                              ShutterFpsParser_fp fpShutterFpsParser,
                              GainParser_fp fpGainParser,
                              GetSnrInfo_fp fpGetSnrInfo
                              );

#endif //end of _DRV_SENSOR_EARLYINIY_COMMON_H_
