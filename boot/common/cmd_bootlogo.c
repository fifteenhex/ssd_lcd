/*
* cmd_bootlogo.c- Sigmastar
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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdlib.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#include <ubi_uboot.h>
#include <cmd_osd.h>

#if defined(CONFIG_SSTAR_DISP)
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"
#endif

#if defined(CONFIG_SSTAR_PNL)
#include "mhal_pnl_datatype.h"
#include "mhal_pnl.h"
#include <../drivers/mstar/panel/PnlTbl.h>
void spi_cmd_init(void);
#endif

#if defined(CONFIG_SSTAR_HDMITX)
#include "mhal_hdmitx_datatype.h"
#include "mhal_hdmitx.h"
#endif

#if defined(CONFIG_SSTAR_JPD)
#include "jinclude.h"
#include "jpeglib.h"
#endif

#if defined(CONFIG_SSTAR_RGN)
#include "mhal_rgn_datatype.h"
#include "mhal_rgn.h"
#endif

#if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
#define blit_pixel unsigned char
#endif
#include "blit32.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PNL_TEST_MD_EN             0

#define BOOTLOGO_DBG_LEVEL_ERR     0x01
#define BOOTLOGO_DBG_LEVEL_INFO    0x02
#define BOOTLOGO_DBG_LEVEL_JPD     0x04

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER


#define REGFLAG_DELAY                                         0xFFFE
#define REGFLAG_END_OF_TABLE                                  0xFFFF   // END OF REGISTERS MARKER

#define BOOTLOGO_DBG_LEVEL          0 // BOOTLOGO_DBG_LEVEL_INFO

#define BOOTLOGO_DBG(dbglv, _fmt, _args...)    \
    do                                          \
    if(dbglv & u32BootlogDvgLevel)              \
    {                                           \
            printf(_fmt, ## _args);             \
    }while(0)



#define DISP_DEVICE_NULL     0
#define DISP_DEVICE_HDMI     1
#define DISP_DEVICE_VGA      2
#define DISP_DEVICE_LCD      4

#define BOOTLOGO_TIMING_NUM  14


#define BOOTLOGO_NOT_ZOOM     0

#define BOOTLOGO_VIRTUAL_ADDRESS_OFFSET 0x20000000

#define MEASURE_BOOT_LOGO_TIME 0
//-------------------------------------------------------------------------------------------------
//  structure & Enu
//-------------------------------------------------------------------------------------------------
/*Base*/
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

/*Disp*/
typedef enum
{
    EN_DISPLAY_DEVICE_NULL,
    EN_DISPLAY_DEVICE_LCD,
    EN_DISPLAY_DEVICE_HDMI,
    EN_DISPLAY_DEVICE_VGA
}SS_SHEADER_DisplayDevice_e;

typedef struct
{
    SS_SHEADER_DataInfo_t stDataInfo;
    u32 u32FirstUseOffset;
    u32 u32DispBufSize;
    u32 u32DispBufStart;
}SS_SHEADER_DispInfo_t;

/*HDMI & VGA*/
typedef struct
{
    SS_SHEADER_DisplayDevice_e enDevice;
    u8 au8ResName[32];
    u32 u32Width;
    u32 u32Height;
    u32 u32Clock;
}SS_SHEADER_DispConfig_t;

#if defined(CONFIG_SSTAR_PNL)
/*Panel*/
typedef struct
{
    SS_SHEADER_DisplayDevice_e enDevice;
    u8 au8PanelName[32];
    MhalPnlParamConfig_t stPnlParaCfg;
    MhalPnlMipiDsiConfig_t stMipiDsiCfg;
}SS_SHEADER_PnlPara_t;
#endif

typedef union
{
    SS_SHEADER_DispConfig_t stDispOut;
#if defined(CONFIG_SSTAR_PNL)
    SS_SHEADER_PnlPara_t stPnlPara;
#endif
}SS_SHEADER_DispPnl_u;

typedef struct
{
    SS_SHEADER_DataInfo_t stDataInfo;
}SS_SHEADER_PictureDataInfo_t;


typedef enum
{
    EN_ASPECT_RATIO_ZOOM,
    EN_ASPECT_RATIO_CENTER,
    EN_ASPECT_RATIO_USER
}DisplayOutAspectRatio_e;

typedef enum
{
    EN_DISPLAY_OUT_DEVICE_NULL,
    EN_DISPLAY_OUT_DEVICE_LCD,
    EN_DISPLAY_OUT_DEVICE_HDMI,
    EN_DISPLAY_OUT_DEVICE_VGA
}DisplayOutDevice_e;
typedef struct
{
    DisplayOutAspectRatio_e enAspectRatio;
    u32 u32DstX;
    u32 u32DstY;
}DispOutAspectRatio;

typedef struct DispMiscConfig_s
{
    u32 bColorMetrixIdValid;
    u32 u32ColorMetrixId;
    u32 bGopDstIdValid;
    u32 u32GopDstId;
} DispMiscConfig_t;

typedef struct
{
    DisplayOutDevice_e enDev;
    u64 phyAddr;
    u32 u32Size;
    u32 u32Width;
    u32 u32Height;
    u32 u32Clock;
    u32 u32ImgWidth;
    u32 u32ImgHeight;
#if defined(CONFIG_SSTAR_PNL)
    MhalPnlParamConfig_t *pstPnlParaCfg;
    MhalPnlMipiDsiConfig_t *pstMipiDsiCfg;
    MhalPnlUnifiedParamConfig_t *pstUniPnlParaCfg;
#endif
    MHAL_DISP_PqConfig_t stDispPqCfg;
    DispMiscConfig_t stDispMiscCfg;
}DispOutCfg;

typedef enum
{
    EN_LOGO_ROTATE_NONE,
    EN_LOGO_ROTATE_90,
    EN_LOGO_ROTATE_180,
    EN_LOGO_ROTATE_270
}LogoRotation_e;

#if defined(CONFIG_SSTAR_DISP)
typedef struct
{
    MHAL_DISP_DeviceTiming_e enTiminId;
    u16 u16HsyncWidht;
    u16 u16HsyncBacPorch;

    u16 u16VsyncWidht;
    u16 u16VsyncBacPorch;

    u16 u16Hstart;
    u16 u16Vstart;
    u16 u16Hactive;
    u16 u16Vactive;

    u16 u16Htotal;
    u16 u16Vtotal;
    u16 u16DclkMhz;
}DisplayLogoTimingConfig_t;
#endif

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 u32BootlogDvgLevel = BOOTLOGO_DBG_LEVEL_ERR;// | BOOTLOGO_DBG_LEVEL_INFO | BOOTLOGO_DBG_LEVEL_JPD;

#if defined(CONFIG_SSTAR_DISP)
DisplayLogoTimingConfig_t stTimingTable[BOOTLOGO_TIMING_NUM] =
{
    {   E_MHAL_DISP_OUTPUT_1080P60,
        44, 148,  5,  36, 192, 41, 1920, 1080, 2200, 1125, 148 },

    {   E_MHAL_DISP_OUTPUT_1080P50,
        44,  148, 5, 36, 192, 41, 1920, 1080, 2640, 1125, 148 },

    {   E_MHAL_DISP_OUTPUT_720P50,
        40, 220, 5, 20, 260, 25, 1280, 720, 1980, 750, 74},

    {   E_MHAL_DISP_OUTPUT_720P60,
        40, 220,5,20, 260, 25, 1280, 720, 1650, 750, 74},

    {   E_MHAL_DISP_OUTPUT_480P60,
        62, 60, 6, 30, 122, 36, 720, 480, 858, 525, 27},

    {   E_MHAL_DISP_OUTPUT_576P50,
        64, 68, 4, 39, 132, 44, 720, 5760, 864, 625, 27},

    {   E_MHAL_DISP_OUTPUT_1024x768_60,
        136, 160, 6, 29, 296, 35, 1024, 768, 1344, 806, 65},

    {   E_MHAL_DISP_OUTPUT_1366x768_60,
        143, 215, 3, 24, 358, 27, 1366, 768, 1792, 798, 86},

    {   E_MHAL_DISP_OUTPUT_1440x900_60,
        152, 232, 6, 25, 384, 31, 1440, 900, 1904, 934, 106},

    {   E_MHAL_DISP_OUTPUT_1280x800_60,
        128, 200, 6, 22, 328, 28, 1280, 800, 1680, 831, 84},

    {   E_MHAL_DISP_OUTPUT_1280x1024_60,
        112, 248, 3, 38, 360, 41, 1280, 1024, 1688, 1066, 108},

    {   E_MHAL_DISP_OUTPUT_1680x1050_60,
        176, 280, 6, 30, 456, 36, 1680, 1050, 2240, 1089, 146},

    {   E_MHAL_DISP_OUTPUT_1600x1200_60,
        192, 304, 3, 46, 496, 49, 1600, 1200, 2160, 1250, 162},

    {   E_MHAL_DISP_OUTPUT_USER,
        48, 46,  4,  23, 98, 27, 800, 480, 928, 525, 43},
};
#endif

#if defined(CONFIG_CMD_MTDPARTS)
#include <jffs2/jffs2.h>
/* partition handling routines */
int mtdparts_init(void);
int find_dev_and_part(const char *id, struct mtd_device **dev,
        u8 *part_num, struct part_info **part);
#endif

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_SSTAR_DISP)
MS_S32 BootLogoMemAlloc(MS_U8 *pu8Name, MS_U32 size, unsigned long long *pu64PhyAddr)
{
    return 0;
}

MS_S32 BootLogoMemRelease(unsigned long long u64PhyAddr)
{
    return 0;
}

MHAL_DISP_DeviceTiming_e _BootLogoGetTiminId(u16 u16Width, u16 u16Height, u8 u8Rate)
{
    MHAL_DISP_DeviceTiming_e enTiming;
    enTiming =  ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 24) ? E_MHAL_DISP_OUTPUT_1080P24 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 25) ? E_MHAL_DISP_OUTPUT_1080P25 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 30) ? E_MHAL_DISP_OUTPUT_1080P30 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 50) ? E_MHAL_DISP_OUTPUT_1080P50 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1080P60 :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 50) ? E_MHAL_DISP_OUTPUT_720P50  :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_720P60  :
                ((u16Width) == 720  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_480P60  :
                ((u16Width) == 720  && (u16Height) == 576  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_576P50  :
                ((u16Width) == 640  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_640x480_60   :
                ((u16Width) == 800  && (u16Height) == 600  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_800x600_60   :
                ((u16Width) == 1280 && (u16Height) == 1024 && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1280x1024_60 :
                ((u16Width) == 1366 && (u16Height) == 768  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1366x768_60  :
                ((u16Width) == 1440 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1440x900_60  :
                ((u16Width) == 1280 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1280x800_60  :
                ((u16Width) == 3840 && (u16Height) == 2160 && (u8Rate) == 30) ? E_MHAL_DISP_OUTPUT_3840x2160_30 :
                                                                                E_MHAL_DISP_OUTPUT_MAX;
    return enTiming;
}
#endif

#if defined(CONFIG_SSTAR_HDMITX)
MhaHdmitxTimingResType_e _BootLogoGetHdmitxTimingId(u16 u16Width, u16 u16Height, u8 u8Rate)
{
    MhaHdmitxTimingResType_e enTiming;
    enTiming =  ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 24) ? E_MHAL_HDMITX_RES_1920X1080P_24HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 25) ? E_MHAL_HDMITX_RES_1920X1080P_25HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 30) ? E_MHAL_HDMITX_RES_1920X1080P_30HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 50) ? E_MHAL_HDMITX_RES_1920X1080P_50HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1920X1080P_60HZ :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 50) ? E_MHAL_HDMITX_RES_1280X720P_50HZ  :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X720P_60HZ  :
                ((u16Width) == 720  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_720X480P_60HZ  :
                ((u16Width) == 720  && (u16Height) == 576  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_720X576P_50HZ  :
                ((u16Width) == 640  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_MAX   :
                ((u16Width) == 800  && (u16Height) == 600  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_MAX   :
                ((u16Width) == 1280 && (u16Height) == 1024 && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X1024P_60HZ :
                ((u16Width) == 1366 && (u16Height) == 768  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1366X768P_60HZ  :
                ((u16Width) == 1440 && (u16Height) == 900  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1440X900P_60HZ  :
                ((u16Width) == 1280 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X800P_60HZ  :
                ((u16Width) == 3840 && (u16Height) == 2160 && (u8Rate) == 30) ? E_MHAL_HDMITX_RES_MAX :
                                                                                E_MHAL_HDMITX_RES_MAX;
    return enTiming;
}
#endif

void _BootLogoDispPnlInit(void)
{
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_PanelConfig_t stPnlCfg[BOOTLOGO_TIMING_NUM];
    u16 i;


    if( sizeof(stTimingTable)/sizeof(DisplayLogoTimingConfig_t) > BOOTLOGO_TIMING_NUM)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Timing Talbe is bigger than %d\n",
            __FUNCTION__, __LINE__, BOOTLOGO_TIMING_NUM);
        return;
    }
    memset(stPnlCfg, 0, sizeof(MHAL_DISP_PanelConfig_t)*BOOTLOGO_TIMING_NUM);
    for(i=0; i<BOOTLOGO_TIMING_NUM; i++)
    {
        stPnlCfg[i].bValid = 1;
        stPnlCfg[i].eTiming = stTimingTable[i].enTiminId;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncWidth     = stTimingTable[i].u16HsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncBackPorch = stTimingTable[i].u16HsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_ucPanelVSyncWidth     = stTimingTable[i].u16VsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelVBackPorch     = stTimingTable[i].u16VsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_wPanelHStart          = stTimingTable[i].u16Hstart;
        stPnlCfg[i].stPanelAttr.m_wPanelVStart          = stTimingTable[i].u16Vstart;
        stPnlCfg[i].stPanelAttr.m_wPanelWidth           = stTimingTable[i].u16Hactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHeight          = stTimingTable[i].u16Vactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHTotal          = stTimingTable[i].u16Htotal;
        stPnlCfg[i].stPanelAttr.m_wPanelVTotal          = stTimingTable[i].u16Vtotal;
        stPnlCfg[i].stPanelAttr.m_dwPanelDCLK           = stTimingTable[i].u16DclkMhz;
    }


    MHAL_DISP_InitPanelConfig(stPnlCfg, BOOTLOGO_TIMING_NUM);
#endif
}
#define YUV444_TO_YUV420_PIXEL_MAPPING(y_dst_addr, uv_dst_addr, dst_x, dst_y, dst_stride, src_addr, src_x, src_y, src_w, src_h) do {   \
        for (src_y = 0; src_y < src_h; src_y++) \
        {   \
            for (src_x = 0; src_x < src_w; src_x++) \
            {   \
                *((char *)((char *)(y_dst_addr) + (dst_y) * (dst_stride) + (dst_x)))    \
                    = *((char *)((char *)(src_addr) + (src_y) * (src_w * 3) + (src_x * 3)));  \
                if ((src_y & 0x01) && (src_x & 0x01))   \
                {   \
                    *((short *)((char *)(uv_dst_addr) + ((dst_y - 1) >> 1) * (dst_stride) + (dst_x - 1)))    \
                        = *((short *)((char *)(src_addr) + (src_y) * (src_w * 3) + (src_x * 3) + 1));  \
                }   \
            }   \
        }   \
    }while(0)

void _BootLogoYuv444ToYuv420(u8 *pu8InBuf, u8 *pu8OutBuf, u16 *pu16Width, u16 *pu16Height, LogoRotation_e eRot)
{
    u16 x, y;

    u8 *pu8DesY = NULL, *pu8DesUV = NULL;;
    u8 *pu8SrcYUV = NULL;

    pu8SrcYUV = pu8InBuf;

    pu8DesY = pu8OutBuf;
    pu8DesUV = pu8DesY + (*pu16Width) * (*pu16Height);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: 444 To 422, In:%x, Out:%x, Width:%d, Height:%d\n",
        __FUNCTION__, __LINE__,
        (u32)pu8InBuf, (u32)pu8OutBuf, *pu16Width, *pu16Height);

    switch (eRot)
    {
        case EN_LOGO_ROTATE_NONE:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, x, y, *pu16Width, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_90:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, *pu16Height - y, x, *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        case EN_LOGO_ROTATE_180:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, (*pu16Width - x), (*pu16Height - y - 1), *pu16Width, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_270:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, y, (*pu16Width - x - 1), *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        default:
            return;
    }
}

#if defined(CONFIG_SSTAR_RGN)
#define RGB_PIXEL_MAPPING(dst_addr, dst_x, dst_y, dst_stride, src_addr, src_x, src_y, src_stride, src_w, src_h, type) do { \
        for (src_y = 0; src_y < src_h; src_y++) \
        {   \
            for (src_x = 0; src_x < src_w; src_x++) \
            {   \
                *((type *)((char *)(dst_addr) + (dst_y) * (dst_stride) + (dst_x) * sizeof(type)))    \
                    = *((type *)((char *)(src_addr) + (src_y) * (src_stride) + (src_x) * sizeof(type)));  \
            }   \
        }   \
    }while(0)

static void _BootLogoRgbRotate(u8 *pDstBuf, u8 *pSrcBuf, u16 u16Width, u16 u16Height, LogoRotation_e eRot, u8 u8BytePerPixel)
{
    u16 x = 0, y = 0;
    switch (eRot)
    {
        case EN_LOGO_ROTATE_90:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, u16Height - y - 1, x, u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, u16Height - y - 1, x, u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        case EN_LOGO_ROTATE_180:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, (u16Width - x - 1), (u16Height - y - 1), u8BytePerPixel * u16Width, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, (u16Width - x - 1), (u16Height - y - 1), u8BytePerPixel * u16Width, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        case EN_LOGO_ROTATE_270:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, y, (u16Width - x - 1), u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, y, (u16Width - x - 1), u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        default:
            return;
    }
}
static void _BootJpdArgbCtrl(u32 u32InBufSzie, u32 u32InBuf, u32 u32OutBufSize, u32 u32OutBuf, u16 *pu16OutWidth, u16 *pu16OutHeight, LogoRotation_e eRot)
{
#if defined(CONFIG_SSTAR_JPD)
    u32 u32JpgSize;
    u8 *pu8JpgBuffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    u8* dstbuffer = NULL;
    u8 *framebuffer;
    u8* linebuffer;
    u8* optbuffer;
    u16 u16RowStride, u16Width, u16Height, u16PixelSize, u16FbPixleSize;
    int rc; //, i, j;

    u32JpgSize = u32OutBufSize;
    pu8JpgBuffer = (unsigned char *)(u32InBuf);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, pu8JpgBuffer, u32JpgSize);
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1)
    {
        return;
    }

    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);

    u16Width = cinfo.output_width;
    u16Height = cinfo.output_height;
    u16PixelSize = cinfo.output_components;
    *pu16OutWidth = u16Width;
    *pu16OutHeight = u16Height;

    framebuffer = (unsigned char *)(u32OutBuf + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET);

    u16RowStride = u16Width * u16PixelSize;
    linebuffer = malloc(u16RowStride);
    if(!linebuffer)
        return;


#if 1 //ARGB8888
    u16FbPixleSize = 4;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((int*)optbuffer)+i) = 0xFF<<24|(*(pixel))<<16|(*(pixel+1))<<8|(*(pixel+2));
        }
        optbuffer+=u16Width*4;
    }
#endif
#if 0 //ARGB1555
    u16FbPixleSize = 2;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((u16*)optbuffer)+i) = 0x1<<15|(*(pixel)&0xF8)<<7|(*(pixel+1)&0xF8)<<2|(*(pixel+2)&0xF8)>>3;
        }
        optbuffer+=u16Width*2;
    }
#endif
#if 0 //ARGB4444
    u16FbPixleSize = 2;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((u16*)optbuffer)+i) = 0xF<<12|(*(pixel)&0xF0)<<4|(*(pixel+1)&0xF0)|(*(pixel+2)&0xF0)>>4;
        }
        optbuffer+=u16Width*2;
    }
#endif

    if (dstbuffer != NULL && eRot != EN_LOGO_ROTATE_NONE)
    {
        _BootLogoRgbRotate(framebuffer, dstbuffer, u16Width, u16Height, eRot, u16FbPixleSize);
        free(dstbuffer);
        if (eRot == EN_LOGO_ROTATE_90 || eRot == EN_LOGO_ROTATE_270)
        {
            *pu16OutWidth = u16Height;
            *pu16OutHeight = u16Width;
        }
    }
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);
    free(linebuffer);
#endif
}
#else
static void _BootJpdYuvCtrl(u32 u32InBufSzie, u32 u32InBuf, u32 u32OutBufSize, u32 u32OutBuf, u16 *pu16OutWidth, u16 *pu16OutHeight, LogoRotation_e eRot)
{
#if defined(CONFIG_SSTAR_JPD)
    // Variables for the source jpg
    u32 u32JpgSize;
    u8 *pu8JpgBuffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // Variables for the output buffer, and how long each row is
    u32 u32BmpSize;
    u8 *pu8BmpBuffer;

    u32 u32Yuv420Size;
    u8 *pu8Yuv420Buffer;

    u16 u16RowStride, u16Width, u16Height, u16PixelSize;

    int rc; //, i, j;

    u32JpgSize = u32InBufSzie;
    pu8JpgBuffer = (unsigned char *)u32InBuf;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d::  Create Decompress struct\n", __FUNCTION__, __LINE__);
    // Allocate a new decompress struct, with the default error handler.
    // The default error handler will exit() on pretty much any issue,
    // so it's likely you'll want to replace it or supplement it with
    // your own.
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d::  Set memory buffer as source\n", __FUNCTION__, __LINE__);
    // Configure this decompressor to read its data from a memory
    // buffer starting at unsigned char *pu8JpgBuffer, which is u32JpgSize
    // long, and which must contain a complete jpg already.
    //
    // If you need something fancier than this, you must write your
    // own data source manager, which shouldn't be too hard if you know
    // what it is you need it to do. See jpeg-8d/jdatasrc.c for the
    // implementation of the standard jpeg_mem_src and jpeg_stdio_src
    // managers as examples to work from.
    jpeg_mem_src(&cinfo, pu8JpgBuffer, u32JpgSize);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Read the JPEG header\n", __FUNCTION__, __LINE__);
    // Have the decompressor scan the jpeg header. This won't populate
    // the cinfo struct output fields, but will indicate if the
    // jpeg is valid.
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: File does not seem to be a normal JPEG\n", __FUNCTION__, __LINE__);
        return;
    }


    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d::  Initiate JPEG decompression\n", __FUNCTION__, __LINE__);

    // output color space is yuv444 packet
    cinfo.out_color_space = JCS_YCbCr;

    jpeg_start_decompress(&cinfo);

    u16Width = cinfo.output_width;
    u16Height = cinfo.output_height;
    u16PixelSize = cinfo.output_components;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Image is %d by %d with %d components\n",
        __FUNCTION__, __LINE__, u16Width, u16Height, u16PixelSize);


    u32BmpSize = u16Width * u16Height * u16PixelSize;
    pu8BmpBuffer =(u8 *) malloc(u32BmpSize);

    if(pu8BmpBuffer == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: malloc fail\n", __FUNCTION__, __LINE__);
        return;
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: BmpBuffer: 0x%x\n", __FUNCTION__, __LINE__, (u32)pu8BmpBuffer);
    u32Yuv420Size = u16Width * u16Height * 3 / 2;
    pu8Yuv420Buffer = (unsigned char *)(u32OutBuf + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET);

    if( u32Yuv420Size > u32OutBufSize)
    {
        free(pu8BmpBuffer);
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR,"%s %d:: Output buffer is too big, %d\n",
            __FUNCTION__, __LINE__, u16Width * u16Height * u16PixelSize);
        return;
    }

    u16RowStride = u16Width * u16PixelSize;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: Start reading scanlines\n", __FUNCTION__, __LINE__);
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = pu8BmpBuffer + \
                           (cinfo.output_scanline) * u16RowStride;

        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: Done reading scanlines\n", __FUNCTION__, __LINE__);
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: End of decompression\n", __FUNCTION__, __LINE__);
    _BootLogoYuv444ToYuv420(pu8BmpBuffer, pu8Yuv420Buffer, &u16Width, &u16Height, eRot);
    *pu16OutWidth = u16Width;
    *pu16OutHeight = u16Height;

    free(pu8BmpBuffer);
#endif
}
#endif
#if 1//defined(CONFIG_SSTAR_RGN)
static u32 gu32FrameBuffer = 0;
static u32 gu32DispWidth = 0;
static u32 gu32DispHeight = 0;
#endif
static bool _BootLogoSetPq(void* pDispDevCtx, MHAL_DISP_PqConfig_t *pstHalPqCfg, MS_U32 u32PqFlag);
void _BootDispCtrl(DispOutCfg *pstDispOut, DispOutAspectRatio *pstAspectRatio, u32 u32Shift)
{
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_AllocPhyMem_t stPhyMem;
    MHAL_DISP_DeviceTimingInfo_t stTimingInfo;
    void *pDevCtx = NULL;
    void *pVidLayerCtx = NULL;
    void *pInputPortCtx = NULL;
    u32 u32Interface = 0;
    u32 u32DispDbgLevel;
    u16 u16DispOutWidht = 0;
    u16 u16DispOutHeight = 0;
    u8 bRet = 0;
    MHAL_DISP_DeviceConfig_t stDevCfg;
    memset(&stDevCfg, 0, sizeof(stDevCfg));

    stPhyMem.alloc = BootLogoMemAlloc;
    stPhyMem.free  = BootLogoMemRelease;

    u32DispDbgLevel = 0;//0x1F;
    MHAL_DISP_DbgLevel(&u32DispDbgLevel);

    //Inint Pnl Tbl
    _BootLogoDispPnlInit();

    if(pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_LCD)
    {
        u32Interface = MHAL_DISP_INTF_LCD;
    }
    else
    {
        if (pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_HDMI)
            u32Interface = MHAL_DISP_INTF_HDMI;
        else if (pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_VGA)
            u32Interface = MHAL_DISP_INTF_VGA;
    }
#if defined(CONFIG_SSTAR_PNL)
        spi_cmd_init();
#endif

    //----------disp misc config----------
    memset(&stDevCfg, 0, sizeof(stDevCfg));
    if (pstDispOut->stDispMiscCfg.bColorMetrixIdValid)
    {
        stDevCfg.eType = E_MHAL_DISP_DEV_CFG_COLORID;
        stDevCfg.u8ColorId = pstDispOut->stDispMiscCfg.u32ColorMetrixId;
    }
    if (pstDispOut->stDispMiscCfg.bGopDstIdValid)
    {
        stDevCfg.eType |= E_MHAL_DISP_DEV_CFG_GOPBLENDID;
        stDevCfg.u8GopBlendId = pstDispOut->stDispMiscCfg.u32GopDstId;
    }
    if (stDevCfg.eType != E_MHAL_DISP_DEV_CFG_NONE)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, eType: %d\n", __FUNCTION__, __LINE__, stDevCfg.eType);
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, u8ColorId: %d\n", __FUNCTION__, __LINE__, stDevCfg.u8ColorId);
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, u8GopBlendId: %d\n", __FUNCTION__, __LINE__, stDevCfg.u8GopBlendId);
        bRet = MHAL_DISP_DeviceSetConfig(0, &stDevCfg);
        if (!bRet)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, MHAL_DISP_DeviceSetConfig error\n", __FUNCTION__, __LINE__);
        }
    }

    if(pDevCtx == NULL)
    {
        if(MHAL_DISP_DeviceCreateInstance(&stPhyMem, 0, &pDevCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreateDevice Fail\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    if(pVidLayerCtx == NULL)
    {
        if(MHAL_DISP_VideoLayerCreateInstance(&stPhyMem, 0, &pVidLayerCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreateVideoLayer Fail\n", __FUNCTION__, __LINE__);
            return;

        }
        MHAL_DISP_VideoLayerBind(pVidLayerCtx, pDevCtx);
    }

    if(pInputPortCtx == NULL)
    {
        if(MHAL_DISP_InputPortCreateInstance(&stPhyMem, pVidLayerCtx, 0, &pInputPortCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreaetInputPort Fail\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    MHAL_DISP_DeviceSetBackGroundColor(pDevCtx, 0x800080);
    MHAL_DISP_DeviceEnable(pDevCtx, 0);
#if defined(CONFIG_SSTAR_PNL)
    if (pstDispOut->pstPnlParaCfg)
    {
        u32 u32DispInterface = u32Interface;
        if((pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL)||
            (pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF))
            u32DispInterface = MHAL_DISP_INTF_TTL;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            u32DispInterface = MHAL_DISP_INTF_MIPIDSI;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_SRGB)
            u32DispInterface = MHAL_DISP_INTF_SRGB;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MCU_TYPE)
            u32DispInterface = MHAL_DISP_INTF_MCU;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT1120)
            u32DispInterface = MHAL_DISP_INTF_BT1120;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT656)
            u32DispInterface = MHAL_DISP_INTF_BT656;
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32DispInterface);
    }
    else if (pstDispOut->pstUniPnlParaCfg)
    {
        u32 u32DispInterface = u32Interface;
        if((pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL)||
            (pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF))
            u32DispInterface = MHAL_DISP_INTF_TTL;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            u32DispInterface = MHAL_DISP_INTF_MIPIDSI;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_SRGB)
            u32DispInterface = MHAL_DISP_INTF_SRGB;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MCU_TYPE)
            u32DispInterface = MHAL_DISP_INTF_MCU;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT1120)
            u32DispInterface = MHAL_DISP_INTF_BT1120;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT656)
            u32DispInterface = MHAL_DISP_INTF_BT656;
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32DispInterface);
    }
    else
#endif
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32Interface);
#if defined(CONFIG_SSTAR_PNL)
    if(u32Interface == MHAL_DISP_INTF_LCD)
    {
        MHAL_DISP_SyncInfo_t stSyncInfo;
        memset(&stSyncInfo, 0, sizeof(stSyncInfo));
        if (pstDispOut->pstPnlParaCfg)
        {
            stSyncInfo.u16Vact = pstDispOut->pstPnlParaCfg->u16Height;
            stSyncInfo.u16Vbb  = pstDispOut->pstPnlParaCfg->u16VSyncBackPorch;
            stSyncInfo.u16Vpw  = pstDispOut->pstPnlParaCfg->u16VSyncWidth;
            stSyncInfo.u16Vfb  = pstDispOut->pstPnlParaCfg->u16VTotal - stSyncInfo.u16Vact - stSyncInfo.u16Vbb - stSyncInfo.u16Vpw;
            stSyncInfo.u16Hact = pstDispOut->pstPnlParaCfg->u16Width;
            stSyncInfo.u16Hbb  = pstDispOut->pstPnlParaCfg->u16HSyncBackPorch;
            stSyncInfo.u16Hpw  = pstDispOut->pstPnlParaCfg->u16HSyncWidth;
            stSyncInfo.u16Hfb  = pstDispOut->pstPnlParaCfg->u16HTotal - stSyncInfo.u16Hact - stSyncInfo.u16Hbb - stSyncInfo.u16Hpw;
            u16DispOutWidht = pstDispOut->pstPnlParaCfg->u16Width;
            u16DispOutHeight = pstDispOut->pstPnlParaCfg->u16Height;
        }
        else if (pstDispOut->pstUniPnlParaCfg)
        {
            if (pstDispOut->pstUniPnlParaCfg->u8TgnTimingFlag)
            {
                stSyncInfo.u16Vact = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VActive;
                stSyncInfo.u16Vbb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VSyncBackPorch;
                stSyncInfo.u16Vpw  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VSyncWidth;
                stSyncInfo.u16Vfb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VTotal - stSyncInfo.u16Vact - stSyncInfo.u16Vbb - stSyncInfo.u16Vpw;
                stSyncInfo.u16Hact = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HActive;
                stSyncInfo.u16Hbb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HSyncBackPorch;
                stSyncInfo.u16Hpw  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HSyncWidth;
                stSyncInfo.u16Hfb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HTotal - stSyncInfo.u16Hact - stSyncInfo.u16Hbb - stSyncInfo.u16Hpw;
                u16DispOutWidht = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HActive;
                u16DispOutHeight = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VActive;
                stSyncInfo.u32FrameRate = pstDispOut->pstUniPnlParaCfg->stMpdInfo.u16Fps;

            }
            else
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "PNL PARA u8TgnTimingFlag 0\n");

                return;
            }
        }
        else
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "PNL PARA is NULL\n");

            return;
        }
        if (pstDispOut->pstMipiDsiCfg)
        {
            stSyncInfo.u32FrameRate = pstDispOut->pstMipiDsiCfg->u16Fps;
        }
        else
        {
            //TTL panel has no mipi para.
        }
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, H(%d %d %d %d) V(%d %d %d %d) Fps:%d\n",
            __FUNCTION__, __LINE__,
            stSyncInfo.u16Hfb, stSyncInfo.u16Hpw, stSyncInfo.u16Hbb, stSyncInfo.u16Hact,
            stSyncInfo.u16Vfb, stSyncInfo.u16Vpw, stSyncInfo.u16Vbb, stSyncInfo.u16Vact,
            stSyncInfo.u32FrameRate);

        stTimingInfo.eTimeType = E_MHAL_DISP_OUTPUT_USER;
        stTimingInfo.pstSyncInfo = &stSyncInfo;
        MHAL_DISP_DeviceSetOutputTiming(pDevCtx, u32Interface, &stTimingInfo);
    }
    else
#endif
    {
        stTimingInfo.eTimeType = _BootLogoGetTiminId(pstDispOut->u32Width,
                                                     pstDispOut->u32Height,
                                                     pstDispOut->u32Clock);
        stTimingInfo.pstSyncInfo = NULL;
        MHAL_DISP_DeviceSetOutputTiming(pDevCtx, u32Interface, &stTimingInfo);
        u16DispOutWidht = pstDispOut->u32Width;
        u16DispOutHeight = pstDispOut->u32Height;
    }
    _BootLogoSetPq(pDevCtx, &pstDispOut->stDispPqCfg, 0);
    MHAL_DISP_DeviceEnable(pDevCtx, 1);
#if !defined(CONFIG_SSTAR_RGN)
    MHAL_DISP_VideoFrameData_t stVideoFrameBuffer;
    MHAL_DISP_InputPortAttr_t stInputAttr;

    memset(&stInputAttr, 0, sizeof(MHAL_DISP_InputPortAttr_t));
    memset(&stVideoFrameBuffer, 0, sizeof(MHAL_DISP_VideoFrameData_t));
    stInputAttr.u16SrcWidth = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
    stInputAttr.u16SrcHeight = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
    switch (pstAspectRatio->enAspectRatio)
    {
        case EN_ASPECT_RATIO_ZOOM:
        {
            stInputAttr.stDispWin.u16X = 0;
            stInputAttr.stDispWin.u16Y = 0;
            stInputAttr.stDispWin.u16Width = u16DispOutWidht;
            stInputAttr.stDispWin.u16Height = u16DispOutHeight;
        }
        break;
        case EN_ASPECT_RATIO_CENTER:
        {
            stInputAttr.stDispWin.u16X = (u16DispOutWidht - stInputAttr.u16SrcWidth) / 2;
            stInputAttr.stDispWin.u16Y = (u16DispOutHeight - stInputAttr.u16SrcHeight) / 2;
            stInputAttr.stDispWin.u16Width = stInputAttr.u16SrcWidth;
            stInputAttr.stDispWin.u16Height = stInputAttr.u16SrcHeight;
        }
        break;
        case EN_ASPECT_RATIO_USER:
        {
            stInputAttr.stDispWin.u16X = pstAspectRatio->u32DstX;
            stInputAttr.stDispWin.u16Y = pstAspectRatio->u32DstY;
            stInputAttr.stDispWin.u16Width = stInputAttr.u16SrcWidth;
            stInputAttr.stDispWin.u16Height = stInputAttr.u16SrcHeight;
        }
        break;
        default:
            return;
    }
    stVideoFrameBuffer.ePixelFormat = E_MHAL_PIXEL_FRAME_YUV_MST_420;
    stVideoFrameBuffer.aPhyAddr[0] = (MS_PHYADDR)(pstDispOut->phyAddr + (u64)u32Shift);
    stVideoFrameBuffer.aPhyAddr[1] = (MS_PHYADDR)(pstDispOut->phyAddr + (u64)(u32Shift + pstDispOut->u32ImgWidth * pstDispOut->u32ImgHeight));
    stVideoFrameBuffer.au32Stride[0] = pstDispOut->u32ImgWidth;
    MHAL_DISP_InputPortSetAttr(pInputPortCtx, &stInputAttr);
    MHAL_DISP_InputPortFlip(pInputPortCtx, &stVideoFrameBuffer);
    MHAL_DISP_InputPortEnable(pInputPortCtx, TRUE);

    MHAL_DISP_RegFlipConfig_t stRegFlipCfg;

    stRegFlipCfg.bEnable = 1;
    stRegFlipCfg.pCmdqInf = NULL;

    MHAL_DISP_SetRegFlipConfig(pDevCtx, &stRegFlipCfg);
#endif
#if defined(CONFIG_SSTAR_PNL)
    static void *pPnlDev = NULL;
    u32 u32DbgLevel;

    if(u32Interface == MHAL_DISP_INTF_LCD)
    {
        if (pPnlDev == NULL)
        {
            u32DbgLevel = 0; //0x0F;
            MhalPnlSetDebugLevel((void *)&u32DbgLevel);
            if (pstDispOut->pstPnlParaCfg)
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, PnlLink:%d\n",
                         __FUNCTION__, __LINE__, pstDispOut->pstPnlParaCfg->eLinkType);
                if(MhalPnlCreateInstance(&pPnlDev, pstDispOut->pstPnlParaCfg->eLinkType) == FALSE)
                {
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, PnlCreateInstance Fail\n", __FUNCTION__, __LINE__);
                    return;
                }
                MhalPnlSetParamConfig(pPnlDev,pstDispOut->pstPnlParaCfg);
                if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
                {
                    MhalPnlSetMipiDsiConfig(pPnlDev, pstDispOut->pstMipiDsiCfg);
                }
            }
            else if (pstDispOut->pstUniPnlParaCfg)
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, PnlLink:%d\n",
                         __FUNCTION__, __LINE__, pstDispOut->pstUniPnlParaCfg->eLinkType);
                if(MhalPnlCreateInstance(&pPnlDev, pstDispOut->pstUniPnlParaCfg->eLinkType) == FALSE)
                {
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, PnlCreateInstance Fail\n", __FUNCTION__, __LINE__);
                    return;
                }
                MhalPnlSetUnifiedParamConfig(pPnlDev,pstDispOut->pstUniPnlParaCfg);
            }
            else
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "PNL PARA is NULL");
                return;
            }

        }
    }
#elif defined(CONFIG_SSTAR_HDMITX)
    static void *pHdmitxCtx = NULL;
    MhalHdmitxAttrConfig_t stAttrCfg;
    MhalHdmitxSignalConfig_t stSignalCfg;
    MhalHdmitxMuteConfig_t stMuteCfg;
    MhalHdmitxHpdConfig_t stHpdCfg;

    if(u32Interface == MHAL_DISP_INTF_HDMI)
    {
        if(pHdmitxCtx == NULL)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO,  "interface %d w %d h %d clock %d\n", u32Interface, pstDispOut->u32Width, pstDispOut->u32Height, pstDispOut->u32Clock);
            if(MhalHdmitxCreateInstance(&pHdmitxCtx, 0) != E_MHAL_HDMITX_RET_SUCCESS)
            {
                printf("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
                return;
            }
            //MhalHdmitxSetDebugLevel(pHdmitxCtx, 0x3F);

            stHpdCfg.u8GpioNum = 26;
            MhalHdmitxSetHpdConfig(pHdmitxCtx, &stHpdCfg);

            stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
            stMuteCfg.bMute = 1;
            MhalHdmitxSetMute(pHdmitxCtx, &stMuteCfg);

            stSignalCfg.bEn = 0;
            MhalHdmitxSetSignal(pHdmitxCtx, &stSignalCfg);

            stAttrCfg.bVideoEn = 1;
            stAttrCfg.enInColor    = E_MHAL_HDMITX_COLOR_RGB444;
            stAttrCfg.enOutColor   = E_MHAL_HDMITX_COLOR_RGB444;
            stAttrCfg.enOutputMode = E_MHAL_HDMITX_OUTPUT_MODE_HDMI;
            stAttrCfg.enColorDepth = E_MHAL_HDMITX_CD_24_BITS;
            stAttrCfg.enTiming     = _BootLogoGetHdmitxTimingId(pstDispOut->u32Width, pstDispOut->u32Height, pstDispOut->u32Clock);

            stAttrCfg.bAudioEn = 1;
            stAttrCfg.enAudioFreq = E_MHAL_HDMITX_AUDIO_FREQ_48K;
            stAttrCfg.enAudioCh   = E_MHAL_HDMITX_AUDIO_CH_2;
            stAttrCfg.enAudioFmt  = E_MHAL_HDMITX_AUDIO_FORMAT_PCM;
            stAttrCfg.enAudioCode = E_MHAL_HDMITX_AUDIO_CODING_PCM;
            MhalHdmitxSetAttr(pHdmitxCtx, &stAttrCfg);

            stSignalCfg.bEn = 1;
            MhalHdmitxSetSignal(pHdmitxCtx, &stSignalCfg);

            stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
            stMuteCfg.bMute = 0;
            MhalHdmitxSetMute(pHdmitxCtx, &stMuteCfg);
        }
    }
#endif
#if defined(CONFIG_SSTAR_RGN)
    MHAL_RGN_GopType_e eGopId = E_MHAL_GOP_DISP0_UI;
    MHAL_RGN_GopGwinId_e eGwinId = E_MHAL_GOP_GWIN_ID_0;
    MHAL_RGN_GopWindowConfig_t stSrcWinCfg;
    MHAL_RGN_GopWindowConfig_t stDstWinCfg;
    MHAL_RGN_OutFmtType_e eOutFmt;
    u8 bInitRgn = 0;

    switch (pstAspectRatio->enAspectRatio)
    {
        case EN_ASPECT_RATIO_ZOOM:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = 0;
            stDstWinCfg.u32Y = 0;
            stDstWinCfg.u32Width  = u16DispOutWidht;
            stDstWinCfg.u32Height = u16DispOutHeight;
        }
        break;
        case EN_ASPECT_RATIO_CENTER:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = (u16DispOutWidht - stSrcWinCfg.u32Width) / 2;
            stDstWinCfg.u32Y = (u16DispOutHeight - stSrcWinCfg.u32Height) / 2;
            stDstWinCfg.u32Width = stSrcWinCfg.u32Width;
            stDstWinCfg.u32Height = stSrcWinCfg.u32Height;
        }
        break;
        case EN_ASPECT_RATIO_USER:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = pstAspectRatio->u32DstX;
            stDstWinCfg.u32Y = pstAspectRatio->u32DstY;
            stDstWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stDstWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
        }
        break;
        default:
            return;
    }
    if (!bInitRgn)
    {
        MHAL_RGN_GopInit();
        bInitRgn = 1;
    }
    eOutFmt = stDevCfg.u8ColorId >= stDevCfg.u8GopBlendId ? E_MHAL_GOP_OUT_FMT_RGB : E_MHAL_GOP_OUT_FMT_YUV;
    MHAL_RGN_GopSetOutputFormat(eGopId, eOutFmt);

    MHAL_RGN_GopSetBaseWindow(eGopId, &stSrcWinCfg, &stDstWinCfg);

    MHAL_RGN_GopGwinSetPixelFormat(eGopId, eGwinId, E_MHAL_RGN_PIXEL_FORMAT_ARGB8888);

    MHAL_RGN_GopGwinSetWindow(eGopId, eGwinId, min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth), min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight), min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth) * 4, 0, 0);

    MHAL_RGN_GopGwinSetBuffer(eGopId, eGwinId, (MS_PHYADDR)pstDispOut->phyAddr);

    MHAL_RGN_GopSetAlphaZeroOpaque(eGopId, FALSE, FALSE, E_MHAL_RGN_PIXEL_FORMAT_ARGB8888);

    MHAL_RGN_GopSetAlphaType(eGopId, eGwinId, E_MHAL_GOP_GWIN_ALPHA_PIXEL, 0xFF);

    MHAL_RGN_GopGwinEnable(eGopId,eGwinId);
#endif
#if 1 //defined(CONFIG_SSTAR_RGN)
    gu32FrameBuffer = (u32)pstDispOut->phyAddr + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET;
    gu32DispWidth = pstDispOut->u32ImgWidth;
    gu32DispHeight = pstDispOut->u32ImgHeight;
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Framebuffer addr 0x%x width %d height %d\n", gu32FrameBuffer, gu32DispWidth, gu32DispHeight);
#endif

#endif
}
#ifndef ALIGN_UP
#define ALIGN_UP(val, alignment) ((( (val)+(alignment)-1)/(alignment))*(alignment))
#endif
SS_SHEADER_DispPnl_u *_BootDbTable(SS_SHEADER_DispInfo_t *pHeadInfo, SS_SHEADER_DispPnl_u *pDispPnl, u8 *pbNeedRestorePartition)
{
    char *pDispTable = NULL;
    SS_SHEADER_DisplayDevice_e enDevice = EN_DISPLAY_DEVICE_NULL;
    SS_SHEADER_DispPnl_u *puDispPnlLoop = NULL;
    u32 u32Idx = 0;
    char tmp[64];

    pDispTable = getenv("dispout");
    enDevice = *(SS_SHEADER_DisplayDevice_e *)pDispPnl;
    *pbNeedRestorePartition = 0;
    switch (enDevice)
    {
#if defined(CONFIG_SSTAR_PNL)
        case EN_DISPLAY_DEVICE_LCD:
        {
            if (!pDispTable)
            {
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache off");
                run_command(tmp, 0);
                setenv("dispout", (const char *)pDispPnl->stPnlPara.au8PanelName);
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "dispout is empty, set %s as default.\n", pDispPnl->stPnlPara.au8PanelName);
                saveenv();
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache on");
                run_command(tmp, 0);

                return pDispPnl;
            }
            if (!strcmp((const char *)pDispPnl->stPnlPara.au8PanelName, pDispTable))
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "DB Table and setting match.\n");

                return pDispPnl;
            }
            else
            {
                for (u32Idx = 0, puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)pHeadInfo + pHeadInfo->stDataInfo.u32SubHeadSize);
                      u32Idx < pHeadInfo->stDataInfo.u32SubNodeCount; u32Idx++)
                {
                    if (!strcmp((const char *)puDispPnlLoop->stPnlPara.au8PanelName, pDispTable))
                    {
                        pHeadInfo->u32FirstUseOffset = (u32)puDispPnlLoop - (u32)pHeadInfo + sizeof(SS_HEADER_Desc_t);
                        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Find dispout: %s, first offset 0x%x\n", pDispTable, pHeadInfo->u32FirstUseOffset);
                        *pbNeedRestorePartition = 1;

                        return puDispPnlLoop;
                    }
                    puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)puDispPnlLoop + ALIGN_UP(sizeof(SS_SHEADER_PnlPara_t) + puDispPnlLoop->stPnlPara.stMipiDsiCfg.u32CmdBufSize, 4));
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Disp header size (%d) mhal pnl para size(%d) spi cmd size(%d) offset 0x%x\n", sizeof(SS_SHEADER_PnlPara_t), sizeof(MhalPnlParamConfig_t), puDispPnlLoop->stPnlPara.stMipiDsiCfg.u32CmdBufSize, (u32)puDispPnlLoop - (u32)pHeadInfo + (u32)sizeof(SS_HEADER_Desc_t));

                }
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found dispout: %s\n", pDispTable);

                return NULL;
            }
        }
        break;
#endif
        case EN_DISPLAY_DEVICE_HDMI:
        case EN_DISPLAY_DEVICE_VGA:
        {
            if (!pDispTable)
            {
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache off");
                run_command(tmp, 0);
                setenv("dispout", (const char *)pDispPnl->stDispOut.au8ResName);
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "dispout is empty, set %s as default.\n", pDispPnl->stDispOut.au8ResName);
                saveenv();
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache on");
                run_command(tmp, 0);

                return pDispPnl;
            }
            if (!strcmp((const char *)pDispPnl->stDispOut.au8ResName, pDispTable))
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "DB Table and setting match.\n");

                return pDispPnl;
            }
            else
            {
                for (u32Idx = 0, puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)pHeadInfo + pHeadInfo->stDataInfo.u32SubHeadSize);
                      u32Idx < pHeadInfo->stDataInfo.u32SubNodeCount; u32Idx++)
                {
                    if (!strcmp((const char *)puDispPnlLoop->stDispOut.au8ResName, pDispTable))
                    {
                        pHeadInfo->u32FirstUseOffset = (u32)puDispPnlLoop - (u32)pHeadInfo + sizeof(SS_HEADER_Desc_t);
                        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Find dispout: %s, first offset 0x%x\n", pDispTable, pHeadInfo->u32FirstUseOffset);
                        *pbNeedRestorePartition = 1;

                        return puDispPnlLoop;
                    }
                    puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)puDispPnlLoop + sizeof(SS_SHEADER_DispConfig_t));
                }
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found dispout: %s\n", pDispTable);

                return NULL;
            }
        }
        break;
        default:
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Device fail\n");

            return NULL;
        }
    }
}
#if (MEASURE_BOOT_LOGO_TIME == 1)
static u64 _gettime(void)
{
    u64 cval;

    asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
    return cval;
}
#endif
#if defined(CONFIG_SSTAR_INI_PARSER)
#include "vsprintf.h"
#include "iniparser.h"

#if defined(CONFIG_FS_LITTLEFS)
#include "littlefs.h"
#elif defined(CONFIG_FS_FIRMWAREFS)
#include "firmwarefs.h"
#else
#error "CONFIG_FS_LITTLEFS or CONFIG_FS_FIRMWAREFS must be defined"
#endif

static S32 inifs_mount(char *partition, char *mnt_path)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_mount(partition, mnt_path);
#else
    return firmwarefs_mount(partition, mnt_path);
#endif
}

static void inifs_unmount(void)
{
#if defined(CONFIG_FS_LITTLEFS)
    littlefs_unmount();
#else
    firmwarefs_unmount();
#endif
}

static void *inifs_open(char *filename, U32 flags, U32 mode)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_open(filename, flags, mode);
#else
    return firmwarefs_open(filename, flags, mode);
#endif
}

static S32 inifs_close(void *fd)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_close(fd);
#else
    return firmwarefs_close(fd);
#endif
}

static S32 inifs_read(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_read(fd, buf, count);
#else
    return firmwarefs_read(fd, buf, count);
#endif
}

static S32 inifs_write(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_write(fd, buf, count);
#else
    return firmwarefs_write(fd, buf, count);
#endif
}

static S32 inifs_lseek(void *fd, S32 offset, S32 whence)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_lseek(fd, offset, whence);
#else
    return firmwarefs_lseek(fd, offset, whence);
#endif
}

static int _GetLogoReservedAddr(u32 *pu32Addr, u32 *pu32Size)
{
    char *pEnv = NULL;
    char *pHandle = NULL;
    int miu = 0;
    char sz[128];
    char start[128];
    char end[128];

    *pu32Addr = 0;
    *pu32Size = 0;
    pEnv = getenv("bootargs");
    if (!pEnv)
        return -1;
    pHandle = strstr(pEnv, "mmap_reserved=fb");
    if (!pHandle)
        return -1;
    sscanf(pHandle, "mmap_reserved=fb,miu=%d,sz=%[^,],max_start_off=%[^,],max_end_off=%[^ ]", &miu, sz, start, end);
    *pu32Size = simple_strtoul(sz, NULL, 16);
    *pu32Addr = simple_strtoul(start, NULL, 16);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Parsing bootargs size 0x%x addr 0x%x\n", *pu32Size, *pu32Addr);

    return 0;
}

#define INI_GET_INT(dict, ret, key, bExist) do{  \
            ret = (typeof(ret))iniparser_getint(dict, key, -1); \
            if (ret == -1)  \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_INT: Get %s error\n", key);   \
                bExist = 0;\
                break;  \
            }   \
            bExist = 1;\
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%d\n", key, ret);   \
        }while(0);
#define INI_GET_STR(dict, ret, key, bExist) do{  \
            ret = (typeof(ret))iniparser_getstring(dict, key, NULL); \
            if (ret == NULL)  \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_STR: Get %s error\n", key);   \
                bExist = 0;\
                break; \
            }   \
            bExist = 1;\
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%s\n", key, ret);   \
        }while(0);
#define INI_GET_PNL_INT(dict, ret, section, key, bExist) do{  \
            int value = 0;  \
            char section_key[128];  \
            snprintf(section_key, 128, "%s:%s", section, key); \
            value = (typeof(value))iniparser_getint(dict, section_key, -1); \
            if (value == -1)  \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "INI_GET_PNL_INT: Get %s error\n", section_key);   \
                ret = -1;   \
                bExist = 0; \
                break; \
            }   \
            else \
            { \
                bExist = 1;  \
                ret = value; \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%d\n", section_key, ret);   \
            } \
        }while(0);
#define INI_GET_PNL_STR(dict, ret, section, key, bExist) do{  \
            char section_key[128];  \
            snprintf(section_key, 128, "%s:%s", section, key); \
            ret = (typeof(ret))iniparser_getstring(dict, section_key, NULL); \
            if (ret == NULL)  \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_STR: Get %s error\n", section_key);   \
                bExist = 0;\
                break;  \
            }   \
            bExist = 1;\
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%s\n", section_key, ret);   \
        }while(0);
#define INI_GET_PNL_ARRAY(dict, ret, cnt, section, key, bExist) do{  \
            char section_key[128];  \
            char *holder = NULL;    \
            char *array_str = NULL; \
            char **array_str_holder = NULL; \
            int array_count;    \
            int array_loop_idx = 0; \
            snprintf(section_key, 128, "%s:%s", section, key); \
            array_str= iniparser_getstring(dict, section_key, NULL); \
            if (array_str == NULL)  \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: Get %s error\n", section_key);   \
                bExist = 0;\
                break;  \
            }   \
            holder = iniparser_parser_string_to_array(array_str, &array_str_holder, &array_count); \
            if (holder == NULL) \
            {   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: Ret %s array error\n", section_key);   \
                bExist = 0;\
                break;  \
            }   \
            ret = (typeof(ret))malloc(sizeof(typeof(*ret)) * array_count);  \
            if (ret == NULL)    \
            {   \
                free(array_str_holder); \
                free(holder);   \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: malloc %s error\n", section_key);   \
                bExist = 0;\
                break;  \
            }   \
            cnt = array_count;  \
            for (array_loop_idx = 0; array_loop_idx < array_count; array_loop_idx++)    \
            {   \
                ret[array_loop_idx] = (typeof(*ret))simple_strtoul(array_str_holder[array_loop_idx], NULL, -1); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s[%d]=%x\n", section_key, array_loop_idx, ret[array_loop_idx]);   \
            }   \
            free(array_str_holder); \
            free(holder);   \
            bExist = 1;\
        }while(0);

static INI_FILE *ini_file_open(const char *path, const char *opt);
static int ini_file_close(INI_FILE *pfile_desc);
static char *ini_file_fgets(char *str_buf, int size, INI_FILE *file_desc);
static bool _BootLoadFile(dictionary* dict, char* pSection, char *pFileName, u8 ** pu8FileBuff, u32 * pu32BuffSize)
{
    bool bRet = true;
    char section_fileName[50] = {0};
    char * pFilePathName = NULL;
    bool bExist = true;
    void * fileFd = NULL;

    if(!dict || !pSection || !pu8FileBuff || !pu32BuffSize)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "illegal param in %s:%d\n", __FUNCTION__, __LINE__);
        bRet = false;
        goto EXIT;
    }
    memset(&section_fileName, 0, sizeof(section_fileName));
    snprintf(section_fileName, sizeof(section_fileName), "%s:%s",pSection, pFileName);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "to load %s\n", section_fileName);
    INI_GET_STR(dict, pFilePathName, section_fileName, bExist);
    if(bExist)
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "get %s\n", pFilePathName);
    else
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "can't get %s\n", section_fileName);
        bRet = false;
        goto EXIT;
    }

    fileFd = inifs_open(pFilePathName, O_RDONLY, 0);
    if(!fileFd)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "can't open %s !\n", pFilePathName);
        bRet = false;
        goto OPEN_FILE_FAILED;
    }
    *pu32BuffSize = inifs_lseek(fileFd, 0, SEEK_END);
    if(*pu32BuffSize == 0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s size is 0!\n", pFilePathName);
        bRet = false;
        goto LSEEK_FAILED;
    }
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "File size %d!\n", *pu32BuffSize);
    *pu8FileBuff = (void *)malloc(*pu32BuffSize);
    if(!*pu8FileBuff)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "malloc memory failed for PQ!\n");
        bRet = false;
        goto MEM_ALLOC_FAILED;
    }
    *pu32BuffSize = inifs_read(fileFd, *pu8FileBuff, *pu32BuffSize);
    if(*pu32BuffSize <= 0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "read %s failed!\n", pFilePathName);
        bRet = false;
        goto READ_FAILED;
    }
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Read size %d!\n", *pu32BuffSize);

    inifs_close(fileFd);
    bRet = true;
    return bRet;

READ_FAILED:
    free(*pu8FileBuff);
    *pu8FileBuff = NULL;
MEM_ALLOC_FAILED:
LSEEK_FAILED:
    inifs_close(fileFd);
OPEN_FILE_FAILED:
EXIT:
    return bRet;
}
static bool _BootLogoLoadPQ(dictionary *dict, MHAL_DISP_PqConfig_t *pstHalPqCfg, char * pSection, char *pPqName)
{
    if(!dict || !pstHalPqCfg || !pSection || !pPqName)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "illegal param in %s:%d\n", __FUNCTION__, __LINE__);
        return false;
    }
    return _BootLoadFile(dict, pSection, pPqName, (u8**)&pstHalPqCfg->pData, &pstHalPqCfg->u32DataSize);
}
static bool _BootLogoSetPq(void* pDispDevCtx, MHAL_DISP_PqConfig_t *pstHalPqCfg, MS_U32 u32PqFlag)
{
    bool bRet = true;
    if(pstHalPqCfg->pData == NULL || pstHalPqCfg->u32DataSize == 0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "illegal param in %s:%d\n", __FUNCTION__, __LINE__);
        bRet = false;
        return bRet;
    }
    pstHalPqCfg->u32PqFlags = u32PqFlag;
    printf("%s %d, pData: %p u32DataSize: %d\n",
    __FUNCTION__, __LINE__, pstHalPqCfg->pData, pstHalPqCfg->u32DataSize);
    return MHAL_DISP_DeviceSetPqConfig(pDispDevCtx, pstHalPqCfg);
}
static bool _BootLogoUnloadPq(MHAL_DISP_PqConfig_t *pstHalPqCfg)
{
    bool bRet = true;
    if(pstHalPqCfg == NULL)
    {
        bRet = false;
        return bRet;
    }
    if(pstHalPqCfg->pData)
        free(pstHalPqCfg->pData);
    pstHalPqCfg->pData = NULL;
    pstHalPqCfg->u32DataSize = 0;
    return bRet;
}
static bool _LogoloadConfigIni(dictionary *dict, char * pstSectionName, DispOutCfg * pstDispOut)
{
    bool bRet = true;
    char strLogoName[50] = {0};
    bool bExist = 0;
    u32 u32Addr = 0;
    u32 u32Size = 0;
    s32 s32TtlCusInitTemp = -1;
#if defined(CONFIG_SSTAR_PNL)
    MhalPnlUnifiedParamConfig_t *stPnlPara;
    char *pTargetPnlName = NULL;
#endif

    if(!dict || !pstSectionName || !pstDispOut)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "illegal param in %s:%d\n", __FUNCTION__, __LINE__);
        bRet = false;
        goto EXIT;
    }
    memset(pstDispOut, 0, sizeof(DispOutCfg));
    memset(&strLogoName, 0, sizeof(strLogoName));
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_eDeviceType", pstSectionName);
    INI_GET_INT(dict, pstDispOut->enDev, strLogoName, bExist);
    _GetLogoReservedAddr(&u32Addr, &u32Size);
    pstDispOut->phyAddr = (u64)u32Addr;
    pstDispOut->u32Size = u32Size;
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispWidth", pstSectionName);
    INI_GET_INT(dict, pstDispOut->u32Width, strLogoName, bExist);
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispHeight", pstSectionName);
    INI_GET_INT(dict, pstDispOut->u32Height, strLogoName, bExist);
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispFps", pstSectionName);
    INI_GET_INT(dict, pstDispOut->u32Clock, strLogoName, bExist);
    switch (pstDispOut->enDev)
    {
        case EN_DISPLAY_DEVICE_NULL:
            pstDispOut->enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
        case EN_DISPLAY_DEVICE_LCD:
            pstDispOut->enDev = EN_DISPLAY_OUT_DEVICE_LCD;
#if defined(CONFIG_SSTAR_PNL)
            snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispWidth", pstSectionName);
            INI_GET_INT(dict, pstDispOut->u32Width, strLogoName, bExist);
            snprintf(strLogoName, sizeof(strLogoName), "%s:m_sParaTarget", pstSectionName);
            INI_GET_STR(dict, pTargetPnlName, strLogoName, bExist);
            stPnlPara = (MhalPnlUnifiedParamConfig_t *)malloc(sizeof(MhalPnlUnifiedParamConfig_t));
            if(stPnlPara == NULL)
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "malloc memory failed %s:%d\n", __FUNCTION__, __LINE__);
                bRet = false;
                goto EXIT;
            }
            memset(stPnlPara, 0, sizeof(MhalPnlUnifiedParamConfig_t));
            INI_GET_PNL_STR(dict, stPnlPara->pPanelName, pTargetPnlName, "m_pPanelName", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->eLinkType, pTargetPnlName, "m_ePanelIntfType", bExist);

            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16HSyncWidth, pTargetPnlName, "m_wPanelHSyncWidth", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16HSyncBackPorch, pTargetPnlName, "m_wPanelHSyncBackPorch", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16VSyncWidth, pTargetPnlName, "m_wPanelVSyncWidth", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16VSyncBackPorch, pTargetPnlName, "m_wPanelVBackPorch", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16HStart, pTargetPnlName, "m_wPanelHStart", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16VStart, pTargetPnlName, "m_wPanelVStart", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16HActive, pTargetPnlName, "m_wPanelWidth", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16VActive, pTargetPnlName, "m_wPanelHeight", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16HTotal, pTargetPnlName, "m_wPanelHTotal", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u16VTotal, pTargetPnlName, "m_wPanelVTotal", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnTimingInfo.u32Dclk, pTargetPnlName, "m_wPanelDCLK", bExist);
            stPnlPara->u8TgnTimingFlag |= bExist?1:0;

            INI_GET_PNL_INT(dict, stPnlPara->eRgbDtype, pTargetPnlName, "m_ePanelRgbDataType", bExist);
            stPnlPara->u8RgbDtypeFlag |= (bExist) ? 1 : 0;

            INI_GET_PNL_INT(dict, stPnlPara->stRgbDeltaInfo.eEvenLine, pTargetPnlName, "m_ePanelRgbDeltaEvenMode", bExist);
            stPnlPara->u8RgbDeltaMdFlag |= (bExist) ? 1 : 0;
            INI_GET_PNL_INT(dict, stPnlPara->stRgbDeltaInfo.eOddLine, pTargetPnlName, "m_ePanelRgbDeltaOddMode", bExist);
            stPnlPara->u8RgbDeltaMdFlag |= (bExist) ? 1 : 0;

            INI_GET_PNL_INT(dict, stPnlPara->stTgnPolarityInfo.u8InvDCLK, pTargetPnlName, "m_bPanelInvDCLK", bExist);
            stPnlPara->u8TgnPolarityFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnPolarityInfo.u8InvDE, pTargetPnlName, "m_bPanelInvDE", bExist);
            stPnlPara->u8TgnPolarityFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnPolarityInfo.u8InvHSync, pTargetPnlName, "m_bPanelInvHSync", bExist);
            stPnlPara->u8TgnPolarityFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnPolarityInfo.u8InvVSync, pTargetPnlName, "m_bPanelInvVSync", bExist);
            stPnlPara->u8TgnPolarityFlag |= bExist?1:0;

            INI_GET_PNL_INT(dict, stPnlPara->stTgnRgbSwapInfo.u8SwapChnR, pTargetPnlName, "m_ucPanelSwapChnR", bExist);
            stPnlPara->u8TgnRgbSwapFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnRgbSwapInfo.u8SwapChnG, pTargetPnlName, "m_ucPanelSwapChnG", bExist);
            stPnlPara->u8TgnRgbSwapFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnRgbSwapInfo.u8SwapChnB, pTargetPnlName, "m_ucPanelSwapChnB", bExist);
            stPnlPara->u8TgnRgbSwapFlag |= bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnRgbSwapInfo.u8SwapRgbML, pTargetPnlName, "m_ucPanelSwapRgbML", bExist);
            stPnlPara->u8TgnRgbSwapFlag |= bExist?1:0;
            stPnlPara->stTgnRgbSwapInfo.u8SwapChnR =
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_B;
            stPnlPara->stTgnRgbSwapInfo.u8SwapChnG =
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_G;
            stPnlPara->stTgnRgbSwapInfo.u8SwapChnB =
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara->stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_R;
            INI_GET_PNL_INT(dict, stPnlPara->eOutputFormatBitMode, pTargetPnlName, "m_eOutputFormatBitMode", bExist);
            stPnlPara->u8TgnOutputBitMdFlag = bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->u16PadMux, pTargetPnlName, "m_wPadmux", bExist);
            stPnlPara->u8TgnPadMuxFlag = bExist?1:0;

            INI_GET_PNL_INT(dict, stPnlPara->u16FDclk, pTargetPnlName, "m_wPanelFixedDCLK", bExist);
            stPnlPara->u8TgnFixedDclkFlag |= (bExist) ? 1 : 0;

            INI_GET_PNL_INT(dict, stPnlPara->stTgnSscInfo.u16SpreadSpectrumStep, pTargetPnlName, "m_wSpreadSpectrumFreq", bExist);
            stPnlPara->u8TgnSscFlag = bExist?1:0;
            INI_GET_PNL_INT(dict, stPnlPara->stTgnSscInfo.u16SpreadSpectrumSpan, pTargetPnlName, "m_wSpreadSpectrumRatio", bExist);
            stPnlPara->u8TgnSscFlag = bExist?1:0;
            INI_GET_PNL_INT(dict, s32TtlCusInitTemp, pTargetPnlName, "m_ePanelPwBlCtrl", bExist);
            stPnlPara->u8TtlCstInitFlag |= (bExist && s32TtlCusInitTemp > 0) ? 1 : 0;

            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u16PwGpioNum, pTargetPnlName, "m_wPanelPwGpioNum", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u32DlyTmData2Bl, pTargetPnlName, "m_wPanelDlyTmData2Bl", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u32DlyTmPw2Data, pTargetPnlName, "m_wPanelDlyTm2Data", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u32DutyVal, pTargetPnlName, "m_wPanelDutyVal", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u32PeriodVal, pTargetPnlName, "m_wPanelPeriodVal", bExist);
            INI_GET_PNL_INT(dict, stPnlPara->stTtlCstmInitInfo.u8PwmNum, pTargetPnlName, "m_wPanelPwmNum", bExist);
            if (stPnlPara->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            {
                stPnlPara->u8MpdFlag = 1;
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8HsTrail, pTargetPnlName, "m_wHsTrail", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8HsPrpr, pTargetPnlName, "m_wHsPrpr", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8HsZero, pTargetPnlName, "m_wHsZero", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ClkHsPrpr, pTargetPnlName, "m_wClkHsPrpr", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ClkHsExit, pTargetPnlName, "m_wClkHsExit", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ClkTrail, pTargetPnlName, "m_wClkTrail", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ClkZero, pTargetPnlName, "m_wClkZero", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ClkHsPost, pTargetPnlName, "m_wClkHsPost", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8DaHsExit, pTargetPnlName, "m_wDaHsExit", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8ContDet, pTargetPnlName, "m_wContDet", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8Lpx, pTargetPnlName, "m_wLpx", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8TaGet, pTargetPnlName, "m_wTaGet", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8TaSure, pTargetPnlName, "m_wTaSure", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8TaGo, pTargetPnlName, "m_wTaGo", bExist);

                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Hactive, pTargetPnlName, "m_wHactive", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Hpw, pTargetPnlName, "m_wHpw", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Hbp, pTargetPnlName, "m_wHbp", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Hfp, pTargetPnlName, "m_wHfp", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Vactive, pTargetPnlName, "m_wVactive", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Vpw, pTargetPnlName, "m_wVpw", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Vbp, pTargetPnlName, "m_wVbp", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Vfp, pTargetPnlName, "m_wVfp", bExist);

                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Bllp, pTargetPnlName, "m_wBllp", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u16Fps, pTargetPnlName, "m_wFps", bExist);

                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.enLaneNum, pTargetPnlName, "m_eLaneNum", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.enFormat, pTargetPnlName, "m_eFormat", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.enCtrl, pTargetPnlName, "m_eCtrlMode", bExist);

                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8PolCh0, pTargetPnlName, "m_ucPolCh0", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8PolCh1, pTargetPnlName, "m_ucPolCh1", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8PolCh2, pTargetPnlName, "m_ucPolCh2", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8PolCh3, pTargetPnlName, "m_ucPolCh3", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8PolCh4, pTargetPnlName, "m_ucPolCh4", bExist);

                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8SwapCh0, pTargetPnlName, "m_ucClkLane", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8SwapCh1, pTargetPnlName, "m_ucDataLane0", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8SwapCh2, pTargetPnlName, "m_ucDataLane1", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8SwapCh3, pTargetPnlName, "m_ucDataLane2", bExist);
                INI_GET_PNL_INT(dict, stPnlPara->stMpdInfo.u8SwapCh4, pTargetPnlName, "m_ucDataLane3", bExist);
                INI_GET_PNL_ARRAY(dict, stPnlPara->stMpdInfo.pu8CmdBuf, stPnlPara->stMpdInfo.u32CmdBufSize, pTargetPnlName, "m_pCmdBuff", bExist);
            }
            pstDispOut->pstUniPnlParaCfg = stPnlPara;
#endif
            break;
        case EN_DISPLAY_DEVICE_HDMI:
            pstDispOut->enDev = EN_DISPLAY_OUT_DEVICE_HDMI;
            pstDispOut->u32Width = pstDispOut->u32Width;
            pstDispOut->u32Height = pstDispOut->u32Height;
            pstDispOut->u32Clock = pstDispOut->u32Clock;
            break;
        case EN_DISPLAY_DEVICE_VGA:
            pstDispOut->enDev = EN_DISPLAY_OUT_DEVICE_VGA;
            pstDispOut->u32Width = pstDispOut->u32Width;
            pstDispOut->u32Height = pstDispOut->u32Height;
            pstDispOut->u32Clock = pstDispOut->u32Clock;
            break;
        default:
            pstDispOut->enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
    }

    //----------disp misc config----------

    INI_GET_PNL_INT(dict, pstDispOut->stDispMiscCfg.u32ColorMetrixId, "DISP0_MISC", "m_wColorMetrixId", bExist);
    pstDispOut->stDispMiscCfg.bColorMetrixIdValid = bExist;
    INI_GET_PNL_INT(dict, pstDispOut->stDispMiscCfg.u32GopDstId, "DISP0_MISC", "m_wGopDstId", bExist);
    pstDispOut->stDispMiscCfg.bGopDstIdValid = bExist;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "[%s %d] bColorMetrixIdValid: %d\n", __func__, __LINE__,
        pstDispOut ->stDispMiscCfg.bColorMetrixIdValid);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "[%s %d] u32ColorMetrixId: %d\n", __func__, __LINE__,
        pstDispOut->stDispMiscCfg.u32ColorMetrixId);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "[%s %d] bGopDstIdValid: %d\n", __func__, __LINE__,
        pstDispOut->stDispMiscCfg.bGopDstIdValid);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "[%s %d] u32GopDstId: %d\n", __func__, __LINE__,
        pstDispOut->stDispMiscCfg.u32GopDstId);

EXIT:
    return bRet;
}

static bool _LogoUnloadConfigIni(DispOutCfg * pstDispOut)
{
    if(pstDispOut == NULL)
        return false;
#if defined(CONFIG_SSTAR_PNL)
    if(pstDispOut->pstUniPnlParaCfg->stMpdInfo.pu8CmdBuf
        && pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
    {
        free(pstDispOut->pstUniPnlParaCfg->stMpdInfo.pu8CmdBuf);
        pstDispOut->pstUniPnlParaCfg->stMpdInfo.pu8CmdBuf = NULL;
    }
    if(pstDispOut->pstUniPnlParaCfg)
        free(pstDispOut->pstUniPnlParaCfg);
    pstDispOut->pstUniPnlParaCfg = NULL;
#endif
    return true;
}
static int _BootLoadLogo(dictionary *dir, char * pstSection, u32 u32LogoId, u8** pu8LogoBuff, u32 * pu32LogoBuffSize)
{
    char strLogoName[50] = {0};
    bool bRet = true;

    if(!dir || !pstSection ||!pu8LogoBuff || !pu32LogoBuffSize)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "illegal param in %s:%d\n", __FUNCTION__, __LINE__);
        bRet = false;
        goto EXIT;
    }
    memset(&strLogoName, 0, sizeof(strLogoName));
    snprintf(strLogoName, sizeof(strLogoName), "m_sLogoFile%d",u32LogoId);
    return _BootLoadFile(dir, pstSection, strLogoName, pu8LogoBuff, pu32LogoBuffSize);
EXIT:
    return bRet;
}
static bool _BootUnloadLogo(void * pLogoBuff)
{
    if(pLogoBuff)
    {
        free(pLogoBuff);
    }
    return true;
}
static int _GetPartiton(char * strENVName)
{
#if !(defined(CONFIG_FS_FIRMWAREFS) && defined(CONFIG_FIRMWAREFS_ON_MMC))
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;
    int ret;

    ret = mtdparts_init();
    if (ret)
        return -1;

    ret = find_dev_and_part(strENVName, &dev, &pnum, &part);
    if (ret)
    {
        return -1;
    }


#else
    return -1;
#endif
#endif
    return 0;
}

static int _MiscPartiton(u32 u32LogoId, DispOutAspectRatio *pstAspectRatio, LogoRotation_e eRot)
{
    char strLogoName[50] = {0};
    char strENVName[] = "MISC";
    iniparser_file_opt ini_opt;
    char  strSectionName[64] = "LOGO";
    char  strPQName[50] = "m_sPQFile0";
    MHAL_DISP_PqConfig_t stDispPQConfig = {0};
    dictionary *dict;
    u32 u32Shift = 0;
    DispOutCfg stDispOut;
    u32 u32LogoBuffSize = 0;
    u8 *pu8LogoBuf = NULL;
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64 u64Time0 = 0;
    u64 u64Time1 = 0;
    u32 u32TimeDiff = 0;
    u64Time0 = _gettime();
#endif

    memset(strLogoName, 0, sizeof(strLogoName));
    memset(&stDispOut, 9, sizeof(DispOutCfg));
    memset(&stDispPQConfig, 0, sizeof(stDispPQConfig));
    memset(&ini_opt, 0, sizeof(ini_opt));
    if(_GetPartiton(strENVName)<0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "get Partiton failed!\n");
        return -1;
    }
    if (inifs_mount(strENVName, NULL) < 0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "inifs_mount %s failed!\n", strENVName);
        return -1;
    }
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    dict = iniparser_load("config.ini", &ini_opt);
    if(dict == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "iniparser_load config.ini failed!\n");
        return -1;
    }
    if(!_LogoloadConfigIni(dict, strSectionName, &stDispOut))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "load config ini failed!\n");
        return -1;
    }
    if(!_BootLoadLogo(dict, strSectionName, u32LogoId, &pu8LogoBuf, &u32LogoBuffSize))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "load logo:%d failed!\n",u32LogoId);
        return -1;
    }
    if(!_BootLogoLoadPQ(dict, &stDispOut.stDispPqCfg, strSectionName, strPQName))
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "load PQ file failed!\n");
#if defined(CONFIG_SSTAR_RGN)
    _BootJpdArgbCtrl(u32LogoBuffSize, (u32)pu8LogoBuf, stDispOut.u32Size-u32Shift, (u32)stDispOut.phyAddr+u32Shift, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#else
    _BootJpdYuvCtrl(u32LogoBuffSize, (u32)pu8LogoBuf, stDispOut.u32Size-u32Shift, (u32)stDispOut.phyAddr+u32Shift, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#endif
    flush_dcache_all();
    _BootUnloadLogo(pu8LogoBuf);
    _BootDispCtrl(&stDispOut, pstAspectRatio, u32Shift);
    _LogoUnloadConfigIni(&stDispOut);
    _BootLogoUnloadPq(&stDispOut.stDispPqCfg);
    iniparser_freedict(dict);
    inifs_unmount();
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time1 = _gettime();
    u32TimeDiff = ((u32)(u64Time1 - u64Time0)) / 6;
    printf("Display Time diff %d us\n", u32TimeDiff);
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "bootlogo exit. %s:%d\n",__FUNCTION__, __LINE__);
    return 0;
}
#endif

int do_display (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    DispOutAspectRatio stAspectRatio;
    LogoRotation_e enRot = EN_LOGO_ROTATE_NONE;
    char *pRot = NULL;

    u32 u32LogoId = 0;

    if (argc != 5)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "usage: bootlogo [logo_id] [aspect ratio '0: zoom' '1: center' '2: usr'] [x] [y]\n");
        return 0;
    }
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "do_display++");
    u32LogoId = simple_strtoul(argv[1], NULL, 0);
    memset(&stAspectRatio, 0, sizeof(DispOutAspectRatio));
    pRot =  getenv("logo_rot");
    if (pRot)
        enRot = (LogoRotation_e)simple_strtoul(pRot, NULL, 0);
    stAspectRatio.enAspectRatio = simple_strtoul(argv[2], NULL, 0);
    stAspectRatio.u32DstX = simple_strtoul(argv[3], NULL, 0);
    stAspectRatio.u32DstY = simple_strtoul(argv[4], NULL, 0);

#if defined(CONFIG_SSTAR_INI_PARSER)
    if (_MiscPartiton(u32LogoId, &stAspectRatio, enRot) == -1)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "show bootlogo failed");
        return -1;
    }
#else
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "you not set CONFIG_SSTAR_INI_PARSER!");
#endif
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "do_display--");

    return 0;
}
/**
 * draw Rectangle. the colormat of Framebuffer is ARGB8888
 */
void drawRect_rgb32 (int x0, int y0, int width, int height, int stride,
    int color,unsigned char* frameBuffer )
{
    int *dest = (int *)((char *) (frameBuffer)
        + y0 * stride + x0 * 4);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color;
        }
        dest = (int *)((char *)dest + stride);
    }
}
void  drawRect_rgb12(int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel =2;
    const int red = (color & 0xff0000) >> (16 + 4);
    const int green = (color & 0xff00) >> (8 + 4);
    const int blue = (color & 0xff) >> 4;
    const short color16 = blue | (green << 4) | (red << (4+4)) |0xf000;
    short *dest = NULL;
    int x, y;

    dest = (short *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            dest[x] = color16;
        }
        dest = (short *)((char *)dest + stride);
    }
}

void  drawRect_Yuv_Y(int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel =1;

    const char color8 = color & 0xff;
    char *dest = NULL;
    int x, y;

    dest = (char *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            dest[x] = color8;
        }
        dest = (char *)((char *)dest + stride);
    }
}

void drawRect_rgb15 (int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel = 2;
    const int red = (color & 0xff0000) >> (16 + 3);
    const int green = (color & 0xff00) >> (8 + 3);
    const int blue = (color & 0xff) >> 3;
    const short color15 = blue | (green << 5) | (red << (5 + 5)) | 0x8000;
    short *dest = NULL;
    int x = 0, y = 0;

    dest = (short *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color15;
        }
        dest = (short *)((char *)dest + stride);
    }
}

#if defined(CONFIG_SSTAR_RGN)
#define BGCOLOR 0xFFFFFFFF
#define BARCOLOR 0xFF00FF00
#define BARCOLOR_DARK 0xFF00AEEF
#else
#define BGCOLOR 0x000000FF
#define BARCOLOR 0x000000A0
#define BARCOLOR_DARK 0x00000000
#endif

void do_bootfb_bar(u8 progress,char* message, u32 width, u32 height, u32 u32Framebuffer)
{

    int total_bar_width = 0;
    int bar_x0 = 0;
    int bar_y0 = 0;
    int bar_width = 0;
    int bar_height = 0;
    #if defined(CONFIG_SSTAR_RGN)
    int stride = width * 4;
    #else
    int stride = width;//YUV, y only
    #endif

    total_bar_width = width * 618 / 1000;
    if (progress > 0 && progress < 100)
    {
        bar_x0 = (width - total_bar_width) / 2;
        bar_y0 = height * 750 / 1000;
        bar_width = total_bar_width * progress / 100;
        bar_height = 8;
        #if defined(CONFIG_SSTAR_RGN)
        drawRect_rgb32(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR, (unsigned char *)u32Framebuffer);
        #else
        drawRect_Yuv_Y(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR, (unsigned char *)u32Framebuffer);
        #endif
        //printf("w %d h %d x %d y %d\n", bar_width, bar_height, bar_x0, bar_y0);
    }
    if (progress < 100)
    {
        bar_x0 = (width - total_bar_width) / 2 + bar_width;
        bar_y0 = height * 750 / 1000;
        bar_width = total_bar_width - bar_width;
        bar_height = 8;
        #if defined(CONFIG_SSTAR_RGN)
        drawRect_rgb32(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR_DARK, (unsigned char *)u32Framebuffer);
        #else
        drawRect_Yuv_Y(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR_DARK, (unsigned char *)u32Framebuffer);
        #endif
        //printf("w %d h %d x %d y %d\n", bar_width, bar_height, bar_x0, bar_y0);
    }
    flush_dcache_all();
}

#if !defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
void do_bootfb_progress(u8 progress,char* message, u16 width, u16 height, u32 u32Framebuffer)
{
    static u8 bInited = 0;
    static u8 u8Completed = 0;
    static u8 u8PreMsgLen = 0;
    blit_props props;
    int completed = progress/10;
    unsigned char* framebuffer = (unsigned char*)(u32Framebuffer);
    int stride = width * 4;
    int i = 0;

    props.Buffer = (blit_pixel *)(framebuffer);
    props.BufHeight = height;
    props.BufWidth= width;
    props.Value = 0xFF000000;
    props.Wrap = 0;
    props.Scale = 4;


    int offset = 10+blit32_ADVANCE*props.Scale*strlen("[progress][100%]");
    int w = (width-offset)/20;
    if(!bInited)
    {
        drawRect_rgb32(0,0,width,height,stride,BGCOLOR,(unsigned char *)framebuffer);
        char str[]="[progess]";
        blit32_TextProps(props,10,height/3,str);

        for(i=0;i<completed;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
        for(i=completed;i<10;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR,framebuffer);
        }
        u8Completed = completed;
        bInited = 1;
    }
    else
    {
        for(i=u8Completed;i<completed;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
    }

    char str[] = "[   %]";
    str[3] = '0'+progress%10;
    if(progress>=10)
    {
        str[2] = '0'+(progress%100)/10;
    }
    if(progress>=100)
    {
        str[1] = '0'+progress/100;
    }
    offset = 10+blit32_ADVANCE*props.Scale*strlen("[progress]");
    drawRect_rgb32(offset,height/3,blit32_ADVANCE*props.Scale*strlen(str),blit32_ADVANCE*props.Scale,stride,BGCOLOR,framebuffer);
    blit32_TextProps(props,offset,height/3,str);
    if(message!=NULL)
    {
        props.Scale = 8;
        if(u8PreMsgLen>0)
        {
            offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
            drawRect_rgb32(offset,height*2/3,blit32_ADVANCE*props.Scale*u8PreMsgLen,(blit32_ADVANCE+1)*props.Scale,stride,BGCOLOR,framebuffer);
        }
        u8PreMsgLen  = strlen(message);
        offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
        blit32_TextProps(props,offset,height*2/3,message);
    }
    flush_dcache_all();
}
#else
void do_bootfb_progress_Yonly(u8 progress,char* message, u16 width, u16 height, u32 u32Framebuffer)
{
    static u8 bInited = 0;
    static u8 u8Completed = 0;
    static u8 u8PreMsgLen = 0;
    blit_props props;
    int completed = progress/10;
    unsigned char* framebuffer = (unsigned char*)(u32Framebuffer);
    int stride = width;
    int i = 0;

    props.Buffer = (blit_pixel *)(framebuffer);
    props.BufHeight = height;
    props.BufWidth= width;
    props.Value = 0x80;
    props.Wrap = 0;
    props.Scale = 4;

    int offset = 70+blit32_ADVANCE*props.Scale*strlen("[prog][100%]");
    int w = 18;

    if(!bInited)
    {
        //drawRect_Yuv_Y(0,0,width,height,stride,BGCOLOR,(unsigned char *)framebuffer);
        char str[]="[prog]";
        blit32_TextProps(props,60,height*2/3,str);

        for(i=0;i<completed;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
        for(i=completed;i<10;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR,framebuffer);
        }
        u8Completed = completed;
        bInited = 1;
    }
    else
    {
        for(i=u8Completed;i<completed;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
    }

    char str[] = "[   %]";
    str[3] = '0'+progress%10;
    if(progress>=10)
    {
        str[2] = '0'+(progress%100)/10;
    }
    if(progress>=100)
    {
        str[1] = '0'+progress/100;
    }
    offset = 60+blit32_ADVANCE*props.Scale*strlen("[prog]");
    drawRect_Yuv_Y(offset,height*2/3,blit32_ADVANCE*props.Scale*strlen(str),blit32_ADVANCE*props.Scale,stride,BGCOLOR,framebuffer);
    blit32_TextProps(props,offset,height*2/3,str);

    if(message!=NULL)
    {
        props.Scale = 4;
        if(u8PreMsgLen>0)
        {
            offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
            drawRect_Yuv_Y(offset,height*4/5,blit32_ADVANCE*props.Scale*u8PreMsgLen,(blit32_ADVANCE+1)*props.Scale,stride,BGCOLOR,framebuffer);
        }
        u8PreMsgLen  = strlen(message);
        offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
        blit32_TextProps(props,offset,height*4/5,message);
    }
    flush_dcache_all();
}
#endif

void do_bootfb_blank(u16 width, u16 height, u32 u32Framebuffer)
{
    #if defined(CONFIG_SSTAR_RGN)
    memset((unsigned char*)(u32Framebuffer), 0, width * height * 4);
    #else
    memset((unsigned char*)(u32Framebuffer), 0x00, (width * height));
    memset((unsigned char*)(u32Framebuffer + (width * height)), 0x80, (width * height)/2);
    #endif
}

int do_bootfb (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 1//defined(CONFIG_SSTAR_RGN)
    if(argc < 2)
    {
        printf("usage\n");
        printf("1.bootframebuffer progressbar <percentage> [message]\n");
        printf("2.bootframebuffer blank\n");
    }
    else
    {
        if(strcmp("progressbar",argv[1])==0)
        {
            u8 progress = simple_strtoul(argv[2], NULL, 10);

            if(argc>=4)
            {
                #if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
                do_bootfb_progress_Yonly(progress,argv[3], gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #else
                do_bootfb_progress(progress,argv[3], gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #endif
            }
            else
            {
                #if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
                do_bootfb_progress_Yonly(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #else
                do_bootfb_progress(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #endif
            }
        }
        else if(strcmp("bar",argv[1])==0)
        {
            u8 progress = simple_strtoul(argv[2], NULL, 10);

            do_bootfb_bar(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
        }
        else if(strcmp("blank",argv[1])==0)
        {
            do_bootfb_blank(gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
        }
    }
#endif
    return 0;
}

U_BOOT_CMD(
    bootlogo, CONFIG_SYS_MAXARGS, 1,    do_display,
    "show bootlogo",
    NULL
);

U_BOOT_CMD(
    bootframebuffer, CONFIG_SYS_MAXARGS, 1,    do_bootfb,
    "boot framebuffer \n" \
    "                 1.bootframebuffer progressbar <percentage> [message]\n" \
    "                 2.bootframebuffer blank\n",
    NULL
);
#if defined(CONFIG_SSTAR_INI_PARSER)
static INI_FILE *ini_file_open(const char *path, const char *opt)
{
    U32 flags;

    if (!strcmp("r", opt))
        flags = O_RDONLY;
    else if (!strcmp("rw", opt))
        flags = O_RDWR;
    else if (!strcmp("w", opt))
        flags = O_WRONLY;
    else
        flags = O_RDONLY;

    return (INI_FILE *)inifs_open((char *)path, flags, 0);
}

static char *ini_file_fgets(char *str_buf, int size, INI_FILE *file_desc)
{
    char c;
    int read_cnt = 0;

    if (!size)
        return NULL;

    while(inifs_read((void *)file_desc, &c, 1))
    {
        str_buf[read_cnt] = c;
        read_cnt++;
        if (read_cnt > size)
            break;
        if (c == '\n')
        {
            if (read_cnt >= 1 && str_buf[read_cnt - 1] == '\r')
            {
                /*Windows format a line end with '\r' '\n'*/
                read_cnt--;
            }
            break;
        }
    }
    if (read_cnt)
    {
        str_buf[read_cnt] = 0;
    }
    else
    {
        return NULL;
    }

    return str_buf;
}

static int ini_file_close(INI_FILE *pfile_desc)
{
    return inifs_close((void *)pfile_desc);
}

static int ini_get_int(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    int val = 0;
    char parse[128];

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }
    memset(parse, 0, 128);
    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getint(d, parse, -1);
    printf("Partition %s file %s, section %s, key %s=%d\n", argv[1], argv[2], argv[3], argv[4], val);
    iniparser_freedict(d);
    inifs_unmount();
    return 0;
}

U_BOOT_CMD(
    ini_get_int, CONFIG_SYS_MAXARGS, 1,    ini_get_int,
    "ini_test\n",
    NULL
);
static int ini_get_str(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    char *val = 0;
    char parse[128];

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getstring(d, parse, "");
    printf("Partition %s file %s, section %s, key %s=%s\n", argv[1], argv[2], argv[3], argv[4], val);
    iniparser_freedict(d);
    inifs_unmount();

    return 0;
}

U_BOOT_CMD(
    ini_get_str, CONFIG_SYS_MAXARGS, 1,    ini_get_str,
    "ini_test\n",
    NULL
);
static int ini_get_str_array(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    char parse[128];
    char *val = NULL;
    char *ret = NULL;
    char **str_array = NULL;
    int str_array_count = 0;
    int i = 0;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }
    memset(parse, 0, 128);
    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getstring(d, parse, "");
    ret = iniparser_parser_string_to_array((const char *)val, &str_array, &str_array_count);
    printf("Partition %s file %s, section %s count %d\n", argv[1], argv[2], argv[3], str_array_count);
    for (i = 0; i < str_array_count; i++)
    {
        printf("idx[%d]=%s(%d)\n", i, str_array[i], (int)simple_strtoul(str_array[i], NULL, 16));
    }
    if (str_array)
    {
        free(str_array);
    }
    if (ret)
    {
        free(ret);
    }
    iniparser_freedict(d);
    inifs_unmount();
    return 0;
}

U_BOOT_CMD(
    ini_get_str_array, CONFIG_SYS_MAXARGS, 1,    ini_get_str_array,
    "ini_test\n",
    NULL
);

static int atoi(char *str)
{
    int value = 0;

    while(*str >= '0' && *str <= '9')
    {
        value *= 10;
        value += *str - '0';
        str++;
    }

    return value;
}

static int ini_get_filesize(cmd_tbl_t *cmdtp, int flag, int argc,
                            char * const argv[])
{
    void *fd = NULL;
    int ret;

    if (argc != 3)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDONLY, 0);
    if (fd != NULL)
    {
        int nFileLen = inifs_lseek(fd, 0, SEEK_END);

        printf("file len is %d\n", nFileLen);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_get_filesize, CONFIG_SYS_MAXARGS, 1,    ini_get_filesize,
    "ini_get_filesize\n",
    NULL
);

static int ini_get_buf(cmd_tbl_t *cmdtp, int flag, int argc,
                       char * const argv[])
{
    char value[32];
    void *fd = NULL;
    int ret;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDONLY, 0);
    if (fd != NULL)
    {
        int pos = atoi(argv[3]);
        int size = atoi(argv[4]);

        if (size > sizeof(value))
            size = sizeof(value);

        inifs_lseek(fd, pos, SEEK_SET);
        printf("seek is %d\n", pos);

        inifs_read(fd, value, size);
        printf("value is:");
        for(int i = 0; i < size; i++)
        {
            printf("0x%.2x,", value[i]);
        }
        printf("\n");
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_get_buf, CONFIG_SYS_MAXARGS, 1,    ini_get_buf,
    "ini_get_buf\n",
    NULL
);

static int ini_put_buf(cmd_tbl_t *cmdtp, int flag, int argc,
                       char * const argv[])
{
    char value[32];
    void *fd = NULL;
    int ret;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDWR, 0);
    if (fd != NULL)
    {
        int pos = atoi(argv[3]);
        int size = strlen(argv[4]);
        char *content = argv[4];
        unsigned char lo, hi;

        if ((size % 2) != 0)
        {
            fprintf(stderr, "ini_put_buf:invalid length %d ((%d %% 2) != 0)\n",
                    size, size);
            inifs_close(fd);
            inifs_unmount();
            return 0;
        }

        printf("ini_put_buf: put %s at %d, size=%d\n",
               content, pos, size);

        printf("fd=%p\n", fd);
        if (size > (sizeof(value)*2))
            size = sizeof(value)*2;

        int j;
        int i;
        for (i = 0, j = 0; i < size; i += 2, j++)
        {
            if (content[i] >= 'A' && content[i] <= 'Z')
            {
                hi = content[i] - 'A' + 10;
            } else if (content[i] >= 'a' && content[i] <= 'z')
            {
                hi = content[i] - 'a' + 10;
            } else if (content[i] >= '0' && content[i] <= '9')
            {
                hi = content[i] - '0';
            } else
            {
                fprintf(stderr, "ini_put_buf:invalid value 0x%x at %d\n",
                        (u32)content[i], i);
                inifs_close(fd);
                inifs_unmount();
                return 0;
            }

            if (content[i + 1] >= 'A' && content[i + 1] <= 'Z')
            {
                lo = content[i + 1] - 'A' + 10;
            } else if (content[i + 1] >= 'a' && content[i + 1] <= 'z')
            {
                lo = content[i + 1] - 'a' + 10;
            } else if (content[i + 1] >= '0' && content[i + 1] <= '9')
            {
                lo = content[i + 1] - '0';
            } else
            {
                fprintf(stderr, "ini_put_buf:invalid value 0x%x at %d\n",
                        (u32)content[i + 1], i + 1);
                inifs_close(fd);
                inifs_unmount();
                return 0;
            }
            value[j] = (unsigned char)(hi << 4 | lo);
        }
        printf("fd=%p,%d,%d\n", fd, j, size);

        inifs_lseek(fd, pos, SEEK_SET);
        printf("seek is %d\n", pos);

        ret = inifs_write(fd, value, j);
        printf("write:%d\n", ret);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_put_buf, CONFIG_SYS_MAXARGS, 1,    ini_put_buf,
    "ini_put_buf\n",
    NULL
);

static int ini_create_file(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int len;
    void *fd = NULL;
    int ret;

    if (argc != 4)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_CREAT|O_RDWR, 0);
    if (fd != NULL)
    {
        len = inifs_write(fd, argv[3], strlen(argv[3]));
        printf("write len is %d!", len);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_create_file, CONFIG_SYS_MAXARGS, 1,    ini_create_file,
    "ini_create_file\n",
    NULL
);

static int ini_write_file(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    void *fd = NULL;
    int ret;
    int flags;
    int offset;

    if (argc != 6)
    {
        printf("arg error!\n");
        return 0;
    }
    /**
     * argv[3] is mode
     * O_RDONLY = 00000000 (Open a file as read only)
     * O_WRONLY = 00000001 (Open a file as write only)
     * O_RDWR   = 00000002 (Open a file as read and write)
     * O_CREAT  = 00000100 (Create a file if it does not exist)
     * O_TRUNC  = 00001000 (Truncate the existing file to zero size)
     * O_APPEND = 00002000 (Move to end of file before every write)
     *
     * append + write is 1025
     * create + write is 65
     */
    flags = atoi(argv[3]);
    offset = atoi(argv[5]);

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], flags, 0);
    if (fd != NULL)
    {
        if (offset > 0)
            inifs_lseek(fd, offset, SEEK_SET);
        ret = inifs_write(fd, argv[4], strlen(argv[4]));
        printf("write return %d\n", ret);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_write_file, CONFIG_SYS_MAXARGS, 1,    ini_write_file,
    "ini_write_file\n",
    NULL
);

#endif
