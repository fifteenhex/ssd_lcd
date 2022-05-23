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


#ifndef __SS_RAW_HEADER__
typedef char s8;
typedef short s16;
typedef int s32;
typedef unsigned char u8;
typedef unsigned char bool;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct
{
    u8 au8Tittle[8];
    u32 u32DataInfoCnt;
}SS_HEADER_Desc_t;
typedef struct
{
    u8 au8DataInfoName[32];
    u32 u32DataTotalSize;
    u32 u32SubHeadSize;
    u32 u32SubNodeCount;
}SS_SHEADER_DataInfo_t;

FILE *SS_HEADER_Create(const s8 *pPath, SS_HEADER_Desc_t *pstHeader);
FILE *SS_HEADER_OpenAppend(const s8 *pPath, SS_HEADER_Desc_t *pstHeader);
s32 SS_HEADER_Close(FILE *fp);
s32 SS_HEADER_Update(FILE *fp, SS_HEADER_Desc_t *pstHeader);
#endif
