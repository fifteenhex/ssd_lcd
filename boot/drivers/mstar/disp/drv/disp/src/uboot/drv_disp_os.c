/*
* drv_disp_os.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/

#define _DRV_DISP_OS_C_
#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

#include "drv_disp_os.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
void * DrvDispOsMemAlloc(u32 u32Size)
{
    return malloc(u32Size);
}

void DrvDispOsMemRelease(void *pPtr)
{
    free(pPtr);
}

void DrvDispOsMsSleep(u32 u32Msec)
{
    mdelay(u32Msec);
}

void DrvDispOsUsSleep(u32 u32Usec)
{
    udelay(u32Usec);
}

void DrvDispOsMsDelay(u32 u32Msec)
{
    mdelay(u32Msec);
}

void DrvDispOsUsDelay(u32 u32Usec)
{
    udelay(u32Usec);
}

s32 DrvDispOsImiHeapAlloc(u32 u32Size, void **ppAddr)
{
    return 0;
}

void DrvDispOsImiHeapFree(void *pAddr)
{

}

bool DrvDispOsPadMuxActive(void)
{
    return 0;
}

bool DrvDispOsCreateTask(DrvDispOsTaskConfig_t *pstTaskCfg, TaskEntryCb pTaskEntry, void *pDataPtr, u8 *pTaskName, bool bAuotStart)
{
    return 0;
}

bool DrvDispOsDestroyTask(DrvDispOsTaskConfig_t *pstTaskCfg)
{
    return 0;
}

bool DrvDispOsSetDeviceNode(void *pPlatFormDev)
{
    return 1;
}

bool DrvDispOsSetClkOn(void *pClkRate, u32 u32ClkRateSize)
{
    return 0;
}

bool DrvDispOsSetClkOff(void *pClkRate, u32 u32ClkRateSize)
{
    return 0;
}

u32 DrvDispOsGetSystemTime (void)
{
    return 0;
}

u64 DrvDispOsGetSystemTimeStamp (void)
{
    return 0;
}

s32 DrvDispOsSplit(u8 **arr, u8 *str,  u8* del)
{
    return 0;
}

u32 DrvDispOsGetFileSize(u8 *path)
{
    return 0;
}

bool DrvDispOsOpenFile(u8 *path,s32 flag,s32 mode, DrvDispOsFileConfig_t *pstFileCfg)
{
    return 0;
}

s32 DrvDispOsWriteFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf,s32 writelen)
{
    return 0;
}

s32 DrvDispOsReadFile(DrvDispOsFileConfig_t *pFileCfg,u8 *buf,s32 readlen)
{
    return 0;
}


s32 DrvDispOsCloseFile(DrvDispOsFileConfig_t *pFileCfg)
{
    return 0;
}

void DrvDispOsSplitByToken(DrvDispOsStrConfig_t *pstStrCfg, u8 *pBuf, u8 *token)
{
}

void DrvDispOsParsingTxt(u8 *pFileName, DrvDispOsTextConfig_t *pstTxtCfg)
{
}

void DrvDispOsStrToL(u8 *pStr, u16 u16Base, u16 *pu16Val)
{
}

bool DrvDispOsStrCmp(u8 *pu8Str, u8 *pu8Pat)
{
    return 0;
}

bool DrvDispOsCreateMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

bool DrvDispOsDestroyMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

bool DrvDispOsObtainMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

bool DrvDispOsReleaseMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

