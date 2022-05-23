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
#include "mi_gfx_datatype.h"

#define RECTW(r) (r.right-r.left)
#define RECTH(r) (r.bottom-r.top)

typedef struct RECT
{
    int top;
    int bottom;
    int left;
    int right;
}RECT;

typedef struct Rect
{
    int x;
    int y;
    int w;
    int h;
}Rect;

typedef struct Surface
{
    int w;
    int h;
    int pitch;
    int BytesPerPixel;
    MI_GFX_ColorFmt_e eGFXcolorFmt;
    unsigned long long phy_addr;
}Surface;


void SstarBlitCCW(Surface * pSrcSurface, Surface *pDstSurface, RECT* pRect);
void SstarBlitCW(Surface * pSrcSurface, Surface *pDstSurface, RECT* pRect);
void SstarBlitHVFlip(Surface * pSrcSurface, Surface *pDstSurface, RECT* pRect);
void SstarBlitNormal(Surface * pSrcSurface, Surface *pDstSurface, RECT* pRect);

