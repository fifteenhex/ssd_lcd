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


#ifndef _MMAP_PARSER_H_
#define _MMAP_PARSER_H_
#include "mmapdatatype.h"

#ifdef __cplusplus
extern "C"
{
#endif


MMAP_S32 MMAPFileParser(const char*,MMAPParserHandle*,char*);
MMAP_BOOL Is_StrFullCmp(const char *, const char *);
MMAP_S32 Parser(MMAPParserHandle*,const char *);

#ifdef __cplusplus
}
#endif

#endif
