/*
* drv_hdmitx_os_header.h- Sigmastar
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

#ifndef _DRV_HDMITX_OS_HEADER_H_
#define _DRV_HDMITX_OS_HEADER_H_

#include <linux/time.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/string.h>


#include "cam_os_wrapper.h"


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CAM_OS_EVENTGROUP            0


#define printf(fmt, args...) CamOsPrintf( fmt, ## args)


#define HDMITX_SPRINTF_STRCAT(str, _fmt, _args...) \
    do {                                   \
        char tmpstr[1024];                 \
        sprintf(tmpstr, _fmt, ## _args);   \
        strcat(str, tmpstr);               \
    }while(0)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

// Timer
typedef struct
{
    MS_S32  s32Id;
    spinlock_t sem_lock;
    CamOsTimer_t stTimerCfg;
    bool bCycle;
    bool bStop;
    MS_U32 u32TimerOut;
    void (*pfnFunc)(MS_U32 stTimer, MS_U32 u32Data);
}DrvHdmitxOsTimerConfig_t;


// Mutex

typedef struct
{
    MS_S32 s32Id;
    CamOsMutex_t stMutxCfg;
}DrvHdmitxOsMutexConfig_t;


// Semaphore

typedef struct
{
    MS_S32 s32Id;
    CamOsTsem_t stSemCfg;
}DrvHdmitxOsSemConfig_t;


// Event
typedef enum
{
    E_DRV_HDMITX_OS_EVENT_MD_AND,                      ///< Specify all of the requested events are require.
    E_DRV_HDMITX_OS_EVENT_MD_OR,                       ///< Specify any of the requested events are require.
    E_DRV_HDMITX_OS_EVENT_MD_AND_CLEAR,                ///< Specify all of the requested events are require. If the request are successful, clear the event.
    E_DRV_HDMITX_OS_EVENT_MD_OR_CLEAR,                 ///< Specify any of the requested events are require. If the request are successful, clear the event.
} DrvHdmitxOsEventWaitMoodeType_e;

#if CAM_OS_EVENTGROUP
typedef struct
{
    bool          bUsed;
    u32           u32EventGroup;
    CamOsTsem_t   stMutexEvent;
    CamOsTcond_t  stSemaphore;
} DrvHdmitxOsEventGroupInfoConfig_t;
#else

struct work_data
{
    struct work_struct         stWork;
    MS_U32                     u32EventFlag;
};


typedef struct
{
    MS_BOOL                     bUsed;
    MS_U32                      u32Waiting;
    MS_U32                      u32EventGroup;
    spinlock_t                  stMutexEvent;
    wait_queue_head_t           stEventWaitQueue;
    struct work_data            stWorkData;
    // pthread_cond_t              stSemaphore; // ?????????????
} DrvHdmitxOsEventGroupInfoConfig_t;
#endif

// Interrupt

typedef void ( *InterruptCallBack) (MS_U32 u32IntNum);

typedef irqreturn_t (*InterruptCb)(int irq, void *dev_instance);

// Task

typedef void* (*TaskEntryCb)(void *argv);


typedef struct
{
    MS_S32 s32Id;
    CamOsThread stThreadCfg;
} DrvHdmitxOsTaskConfig_t;


// MMIO
typedef enum
{
    E_HDMITX_OS_MMIO_PM,
    E_HDMITX_OS_MMIO_NONEPM,
} DrvHdmitxOsMmioType_e;

#endif
