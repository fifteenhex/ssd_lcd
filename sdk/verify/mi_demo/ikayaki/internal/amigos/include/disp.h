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
#ifndef __DISP_H__
#define __DISP_H__

#include "mi_common_datatype.h"
#include "sys.h"





#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE              MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED                MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN              MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE               MAKE_YUYV_VALUE(29,225,107)
#define MAX(a,b)                    ((a) > (b) ? (a) : (b))


typedef struct stDispInfo_s
{
    int intDisplayWidth;
    int intDisplayHeight;
    int intLinkType;
    int intBackGroundColor;
}stDispInfo_t;

class Disp: public Sys
{
    public:
        Disp();
        virtual ~Disp();
    private:
        virtual void Init();
        virtual void Deinit();
        stDispInfo_t stDispInfo;
};
#endif

