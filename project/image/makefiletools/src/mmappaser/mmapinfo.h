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


#ifndef _MMAP_INFO_H_
#define _MMAP_INFO_H_
#include "mmapdatatype.h"

typedef struct {
    MMInfo_t *header;
    char (*name)[MMAP_MAX_NAME_LEN];
    MMapInfo_t *context;
} MMAPInfo;

int ParserMmap(const char *, MMAPInfo *);

#endif // _MMAP_INFO_H_
