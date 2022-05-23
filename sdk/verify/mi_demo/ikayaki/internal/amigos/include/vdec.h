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
#ifndef __VDEC_H__
#define __VDEC_H__

#include "mi_common_datatype.h"
#include "sys.h"


typedef struct stVdecInfo_s
{
    int picWidth;
    int picHeight;
    int decodeType;
    int dpBufMode;
    int refFrameNum;
    unsigned int bitstreamSize;
}stVdecInfo_t;

typedef struct stDecOutInfo_s
{
    int intPortId;
    int intDecOutWidth;
    int intDecOutHeight;
}stDecOutInfo_t;

class Vdec: public Sys
{
public:
    Vdec();
    virtual ~Vdec();

private:
    virtual void Init();
    virtual void Deinit();
    stVdecInfo_t stVdecInfo;
    std::map<std::string, int> mapInputChnId;
};
#endif

