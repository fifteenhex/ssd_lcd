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

#ifndef __VIF_H__
#define __VIF_H__

#include "sys.h"

typedef struct stVifInfo_s
{
    int intSensorId;
    int intSensorRes;
    int intHdrType;
}stVifInfo_t;

class Vif: public Sys
{
    public:
        Vif();
        virtual ~Vif();
    private:
        virtual void Init();
        virtual void Deinit();
        stVifInfo_t stVifInfo;
};
#endif


