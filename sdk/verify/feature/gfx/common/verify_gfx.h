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
#include <mi_gfx.h>
#include <mi_gfx_datatype.h>

#include <mi_sys.h>


// get bytesPerPixel each format
unsigned int getBpp(MI_GFX_ColorFmt_e eFmt);
int _gfx_alloc_surface(MI_GFX_Surface_t *pSurf, char **data,char  *surfName);
void _gfx_free_surface(MI_GFX_Surface_t *pSurf, char *data);
void _gfx_sink_surface(MI_GFX_Surface_t *pSurf, const char *data,const char  *name);
