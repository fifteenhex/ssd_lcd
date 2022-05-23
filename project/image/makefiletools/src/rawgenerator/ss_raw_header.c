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


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ss_raw_header.h"

FILE *SS_HEADER_Create(const s8 *pPath, SS_HEADER_Desc_t *pstHeader)
{
    FILE *fp = NULL;

    fp = fopen(pPath, "w+");
    if(fp == NULL)
    {
        perror("fopen");
        return NULL;
    }
    memcpy(pstHeader->au8Tittle, "SSTAR", 5);
    fwrite(pstHeader, 1, sizeof(SS_HEADER_Desc_t), fp);

    return fp;
}
FILE *SS_HEADER_OpenAppend(const s8 *pPath, SS_HEADER_Desc_t *pstHeader)
{
    FILE *fp = NULL;

    fp = fopen(pPath, "r+");
    if(fp == NULL)
    {
        perror("fopen");
        return NULL;
    }    
    fread(pstHeader, 1, sizeof(SS_HEADER_Desc_t), fp);
    fseek(fp, 0, SEEK_END);

    return fp;
}
s32 SS_HEADER_Close(FILE *fp)
{
    return fclose(fp);
}
s32 SS_HEADER_Update(FILE *fp, SS_HEADER_Desc_t *pstHeader)
{
    s32 s32CurPos = 0;

    s32CurPos = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fwrite(pstHeader, 1, sizeof(SS_HEADER_Desc_t), fp);
    fseek(fp, s32CurPos, SEEK_SET);

    return 0;
}
