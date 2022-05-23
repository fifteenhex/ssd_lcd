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

#ifdef __cplusplus
extern "C"
{
#endif

#include <drv_sensor_common.h>
#include <sensor_i2c_api.h>
#include <drv_sensor.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC132gs);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (2)
//#define SENSOR_MIPI_HDR_MODE (1) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

#define R_GAIN_REG 1
#define G_GAIN_REG 2
#define B_GAIN_REG 3


//#undef SENSOR_DBG
#define SENSOR_DBG 0

#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI     //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000
#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAXGAIN      (15875*31750)/1000000   //max sensor gain, a-gain*conversion-gain*d-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
u32 Preview_line_period;
u32 vts_30fps;

#define Preview_WIDTH       1080                   //resolution Width when preview
#define Preview_HEIGHT      1280                  //resolution Height when preview
#define Preview_MAX_FPS     30  //25                     //fastest preview FPS
#define Preview_MIN_FPS     5                       //slowest preview FPS

#define SENSOR_I2C_ADDR     0x60                   //I2C slave address
#define SENSOR_I2C_SPEED    240000                  //I2C speed,60000~320000

#define SENSOR_I2C_LEGACY   I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT      I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG            // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG


#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif
static int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
//#define ABS(a)   ((a)>(0) ? (a) : (-(a)))
static int g_sensor_ae_min_gain = 1024;
#define ENABLE_NR 1

CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};

typedef struct {
    struct {
        u16 pre_div0;
        u16 div124;
        u16 div_cnt7b;
        u16 sdiv0;
        u16 mipi_div0;
        u16 r_divp;
        u16 sdiv1;
        u16 r_seld5;
        u16 r_sclk_dac;
        u16 sys_sel;
        u16 pdac_sel;
        u16 adac_sel;
        u16 pre_div_sp;
        u16 r_div_sp;
        u16 div_cnt5b;
        u16 sdiv_sp;
        u16 div12_sp;
        u16 mipi_lane_sel;
        u16 div_dac;
    } clk_tree;
    struct {
        u32 sclk;
        u32 hts;
        u32 vts;
        u32 ho;
        u32 xinc;
        u32 line_freq;
        u32 us_per_line;
        u32 final_us;
        u32 final_gain;
        u32 back_pv_us;
        u32 fps;
        u32 preview_fps;
        u32 line;
    } expo;
    struct {
        bool bVideoMode;
        u16 res_idx;
        CUS_CAMSENSOR_ORIT  orit;
    } res;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tMirror_reg[1];
    I2C_ARRAY tNr_reg[2];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool orient_dirty;
    bool reg_dirty;
    bool nr_dirty;
    CUS_CAMSENSOR_ORIT cur_orien;
} sc132gs_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0xcc},
    {0x3108, 0x1a},
};

////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {     // LINEAR
    // Modify it based on number of support resolution
    enum {LINEAR_RES_1 = 0, LINEAR_RES_2 = 1, LINEAR_RES_END}mode;
    // Sensor Output Image info
    struct _senout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _sensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _senstr{
        const char* strResDesc;
    }senstr;
}sc132gs_mipi_linear[] = {
    {LINEAR_RES_1, {1080, 1280, 3, 30}, {0, 0, 1080, 1280}, {"1080x1280@30fps"}},
    {LINEAR_RES_2, {1080, 1280, 3, 60}, {0, 0, 1080, 1280}, {"1080x1280@60fps"}},
    {LINEAR_RES_2, {1080, 1280, 3,120}, {0, 0, 1080, 1280}, {"1080x1280@120fps"}},
};

const static I2C_ARRAY Sensor_init_table_2lane_30fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x3018,0x32},
    {0x3019,0x0c},
    {0x301a,0xb4},
    {0x3031,0x0a},
    {0x3032,0x60},
    {0x3038,0x44},
    {0x3207,0x17},
    {0x320c,0x05}, // 1500
    {0x320d,0xdc},
    {0x320e,0x09}, // 2400
    {0x320f,0x60},
    {0x3250,0xcc},
    {0x3251,0x02},
    {0x3252,0x09},
    {0x3253,0x5b},
    {0x3254,0x05},
    {0x3255,0x3b},
    {0x3306,0x78},
    {0x330a,0x00},
    {0x330b,0xc8},
    {0x330f,0x24},
    {0x3314,0x80},
    {0x3315,0x40},
    {0x3317,0xf0},
    {0x331f,0x12},
    {0x3364,0x00},
    {0x3385,0x41},
    {0x3387,0x41},
    {0x3389,0x09},
    {0x33ab,0x00},
    {0x33ac,0x00},
    {0x33b1,0x03},
    {0x33b2,0x12},
    {0x33f8,0x02},
    {0x33fa,0x01},
    {0x3409,0x08},
    {0x34f0,0xc0},
    {0x34f1,0x20},
    {0x34f2,0x03},
    {0x3622,0xf5},
    {0x3630,0x5c},
    {0x3631,0x80},
    {0x3632,0xc8},
    {0x3633,0x32},
    {0x3638,0x2a},
    {0x3639,0x07},
    {0x363b,0x48},
    {0x363c,0x83},
    {0x363d,0x10},
    {0x36ea,0x38},
    {0x36fa,0x25},
    {0x36fb,0x05},
    {0x36fd,0x04},
    {0x3900,0x11},
    {0x3901,0x05},
    {0x3902,0xc5},
    {0x3904,0x04},
    {0x3908,0x91},
    {0x391e,0x00},
    {0x3e01,0x53},
    {0x3e02,0xe0},
    {0x3e09,0x20},
    {0x3e0e,0xd2},
    {0x3e14,0xb0},
    {0x3e1e,0x7c},
    {0x3e26,0x20},
    {0x4418,0x38},
    {0x4503,0x10},
    {0x4837,0x21},
    {0x5000,0x0e},
    {0x540c,0x51},
    {0x550f,0x38},
    {0x5780,0x67},
    {0x5784,0x10},
    {0x5785,0x06},
    {0x5787,0x02},
    {0x5788,0x00},
    {0x5789,0x00},
    {0x578a,0x02},
    {0x578b,0x00},
    {0x578c,0x00},
    {0x5790,0x00},
    {0x5791,0x00},
    {0x5792,0x00},
    {0x5793,0x00},
    {0x5794,0x00},
    {0x5795,0x00},
    {0x5799,0x04},
    {0x300a,0x64},
    {0x3032,0xa0},
    {0x3217,0x09},
    {0x3218,0x5a},
    {0x36e9,0x20},
    {0x36f9,0x24},
    {0x0100,0x01}, 
    {0xffff,0x0a},
};

const I2C_ARRAY Sensor_init_table_2lane_60fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x0100,0x00},
    {0x3018,0x32},
    {0x3019,0x0c},
    {0x301a,0xb4},
    {0x3031,0x0a},
    {0x3032,0x60},
    {0x3038,0x44},
    {0x3207,0x17},
    {0x320c,0x05},
    {0x320d,0x35},
    {0x320e,0x05},
    {0x320f,0x46},
    {0x3250,0xcc},
    {0x3251,0x02},
    {0x3252,0x05},
    {0x3253,0x41},
    {0x3254,0x05},
    {0x3255,0x3b},
    {0x3306,0x78},
    {0x330a,0x00},
    {0x330b,0xc8},
    {0x330f,0x24},
    {0x3314,0x80},
    {0x3315,0x40},
    {0x3317,0xf0},
    {0x331f,0x12},
    {0x3364,0x00},
    {0x3385,0x41},
    {0x3387,0x41},
    {0x3389,0x09},
    {0x33ab,0x00},
    {0x33ac,0x00},
    {0x33b1,0x03},
    {0x33b2,0x12},
    {0x33f8,0x02},
    {0x33fa,0x01},
    {0x3409,0x08},
    {0x34f0,0xc0},
    {0x34f1,0x20},
    {0x34f2,0x03},
    {0x3622,0xf5},
    {0x3630,0x5c},
    {0x3631,0x80},
    {0x3632,0xc8},
    {0x3633,0x32},
    {0x3638,0x2a},
    {0x3639,0x07},
    {0x363b,0x48},
    {0x363c,0x83},
    {0x363d,0x10},
    {0x36ea,0x38},
    {0x36fa,0x25},
    {0x36fb,0x05},
    {0x36fd,0x04},
    {0x3900,0x11},
    {0x3901,0x05},
    {0x3902,0xc5},
    {0x3904,0x04},
    {0x3908,0x91},
    {0x391e,0x00},
    {0x3e01,0x53},
    {0x3e02,0xe0},
    {0x3e09,0x20},
    {0x3e0e,0xd2},
    {0x3e14,0xb0},
    {0x3e1e,0x7c},
    {0x3e26,0x20},
    {0x4418,0x38},
    {0x4503,0x10},
    {0x4837,0x21},
    {0x5000,0x0e},
    {0x540c,0x51},
    {0x550f,0x38},
    {0x5780,0x67},
    {0x5784,0x10},
    {0x5785,0x06},
    {0x5787,0x02},
    {0x5788,0x00},
    {0x5789,0x00},
    {0x578a,0x02},
    {0x578b,0x00},
    {0x578c,0x00},
    {0x5790,0x00},
    {0x5791,0x00},
    {0x5792,0x00},
    {0x5793,0x00},
    {0x5794,0x00},
    {0x5795,0x00},
    {0x5799,0x04},
    {0x36e9,0x20},
    {0x36f9,0x24},
    {0x0100,0x01},
    {0xffff,0x0a},
};

const I2C_ARRAY Sensor_init_table_4lane_120fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},

//PLL bypass
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x3389,0x01},
    {0x3e01,0x97},
    {0x3e02,0xe0},
    {0x363c,0x87},
    {0x3630,0x88},
    {0x4418,0x1c},
    {0x3631,0xc8},
    {0x3622,0xf5},
    {0x3633,0x32},
    {0x363b,0x48},
    {0x363c,0x83},
    {0x3638,0x2f},
    {0x3306,0x78},
    {0x36ea,0x37},
    {0x4837,0x1a},
    {0x36fb,0x05},
    {0x36fd,0x04},
    {0x36fa,0x25},
    {0x3e01,0x53},
    {0x330a,0x00},
    {0x330b,0xb0},
    {0x3315,0x40},
    {0x320c,0x02},
    {0x320d,0xee},
    {0x33b2,0x12},
    {0x33b1,0x03},
    {0x3631,0xb8},
    {0x4418,0x1c},
    {0x3385,0x41},
    {0x3387,0x41},
    {0x3317,0x90},
    {0x3630,0x8c},
    {0x34f2,0x03},
    {0x34f0,0xc0},
    {0x34f1,0x20},
    {0x3032,0x60},
    {0x5780,0x67},
    {0x5784,0x10},
    {0x5785,0x06},
    {0x5787,0x02},
    {0x5788,0x00},
    {0x5789,0x00},
    {0x578a,0x02},
    {0x578b,0x00},
    {0x578c,0x00},
    {0x5790,0x00},
    {0x5791,0x00},
    {0x5792,0x00},
    {0x5793,0x00},
    {0x5794,0x00},
    {0x5795,0x00},
    {0x5799,0x04},
    {0x3e0e,0xd2},
    {0x3e14,0xb0},
    {0x3e1e,0x7c},
    {0x301a,0xb4},
    {0x391e,0x00},
    {0x3908,0x91},
    {0x4503,0x10},
    {0x3901,0x05},
    {0x3904,0x04},
    {0x3207,0x17},
    {0x3254,0x05},
    {0x3255,0x3b},
    {0x3252,0x05},
    {0x3253,0x41},
    {0x3250,0xcc},
    {0x3251,0x02},
    {0x363d,0x10},
    {0x3639,0x0a},
    {0x3631,0xa4},
    {0x3317,0x88},
    {0x4418,0x38},
    {0x3e09,0x20},
    {0x3409,0x08},
    {0x3389,0x09},
    {0x331f,0x12},
    {0x33ab,0x00},
    {0x33ac,0x00},
    {0x3364,0x00},
    {0x3639,0x07},
    {0x3631,0x80},
    {0x3630,0x5c},
    {0x3638,0x2a},
    {0x330b,0xc8},
    {0x330f,0x24},
    {0x33f8,0x02},
    {0x3314,0x80},
    {0x33fa,0x01},
    {0x3317,0xf0},
    {0x5000,0x0e},
    {0x550f,0x38},
    {0x540c,0x51},
    {0x3e26,0x20},
    {0x3900,0x11},
    {0x3902,0xc5},
    {0x3038,0x44},
    {0x3632,0xc8},
    {0x3e01,0x53},
    {0x3e02,0xe0},

//PLL set
    {0x36e9,0x20},
    {0x36f9,0x24},
    {0x0100,0x01},
    {0xffff,0x0a},
};

I2C_ARRAY mirror_reg[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x80},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x40}, //low bit, 0x40 - 0x7f, step 1/64
};

I2C_ARRAY expo_reg[] = {
    {0x3e00, 0x00}, //expo [20:17]
    {0x3e01, 0x30}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x05},
    {0x320f, 0x46},
};

#if ENABLE_NR
I2C_ARRAY nr_reg[] = {
    {0x33fa,0x01},
    {0x3317,0xf0},
};
#endif

I2C_ARRAY PatternTbl[] = {
    {0x4501,0xc8}, //testpattern , bit 3 to enable
};

CUS_INT_TASK_ORDER def_order = {
        .RunLength = 9,
        .Orders = {
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AE|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_AWB|CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
                CUS_INT_TASK_VDOS|CUS_INT_TASK_AF,
        },
};
/*
/////////// function definition ///////////////////
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) LOGD(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) printf(args)
#elif SENSOR_DBG == 0
#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME sc132gs
*/
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

//int cus_camsensor_release_handle(ms_cus_sensor *handle);

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s]", __FUNCTION__);

    sensor_if->HsyncPol(handle, handle->HSYNC_POLARITY);
    sensor_if->VsyncPol(handle, handle->VSYNC_POLARITY);
    sensor_if->ClkPol(handle, handle->PCLK_POLARITY);
    sensor_if->BayerFmt(handle, handle->bayer_id);
    sensor_if->DataBus(handle, handle->sif_bus);

    sensor_if->DataPrecision(handle, handle->data_prec);
    sensor_if->FmtConv(handle,  handle->data_mode);
    return SUCCESS;
}
#endif
static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);////pwd low
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    SENSOR_USLEEP(2000);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    SENSOR_DMSG("[%s] pwd high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_USLEEP(1000);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    params->cur_orien = CUS_ORIT_M0F0;

    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
  int i,n;
  int table_length= ARRAY_SIZE(Sensor_id_table);
  I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

  SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
  for(n=0;n<table_length;++n) {
    id_from_sensor[n].reg = Sensor_id_table[n].reg;
    id_from_sensor[n].data = 0;
  }

  *id =0;
  if(table_length>8) table_length=8;
  for(n=0; n<4; ++n) {              //retry , until I2C success
    if(n>2) return FAIL;
    if(SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
        break;
    else
        SENSOR_USLEEP(1000);
  }

    for(i=0; i<table_length; ++i) {
    if (id_from_sensor[i].data != Sensor_id_table[i].data)
            return FAIL;
    *id = ((*id)+ id_from_sensor[i].data)<<8;
  }
  *id >>= 8;
    SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int sc132gs_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
  int i;
  SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

  switch(mode) {
  case 1:
    PatternTbl[0].data = 0xc8; //enable
    break;
  case 0:
    PatternTbl[0].data = 0xc0; //disable
    break;
  default:
    PatternTbl[0].data = 0xc0; //disable
    break;
  }
  for(i=0; i< ARRAY_SIZE(PatternTbl); i++) {
      if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            return FAIL;
  }

  return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init_linear_2lane_30fps(ms_cus_sensor *handle)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2lane_30fps);i++)
    {
        if(Sensor_init_table_2lane_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2lane_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2lane_30fps[i].reg, Sensor_init_table_2lane_30fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    return SUCCESS;
}

static int pCus_init_linear_2lane_60fps(ms_cus_sensor *handle)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2lane_60fps);i++)
    {
        if(Sensor_init_table_2lane_60fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2lane_60fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2lane_60fps[i].reg, Sensor_init_table_2lane_60fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    return SUCCESS;
}

static int pCus_init_linear_4lane_120fps(ms_cus_sensor *handle)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_120fps);i++)
    {
        if(Sensor_init_table_4lane_120fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_120fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_120fps[i].reg, Sensor_init_table_4lane_120fps[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    return SUCCESS;
}

/*
int pCus_release(ms_cus_sensor *handle)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sensor_if->PCLK(NULL,CUS_PCLK_OFF);
    return SUCCESS;
}
*/

static int pCus_GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}

static int pCus_GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int pCus_GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int pCus_SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0: //"1080x1280@30fps"
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_linear_2lane_30fps;
            vts_30fps= 2400;//1500
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 13888;
            handle->mclk = CUS_CMU_CLK_24MHZ;
            handle->interface_attr.attr_mipi.mipi_lane_num = 2;
            break;
        case 1: //"1080x1280@60fps"
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_linear_2lane_60fps;
            vts_30fps= 1350;//1500
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 13888;
            handle->mclk = CUS_CMU_CLK_24MHZ;
            handle->interface_attr.attr_mipi.mipi_lane_num = 2;
            break;
        case 2: //"1080x1280@120fps"
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init_linear_4lane_120fps;
            vts_30fps= 1350;//1500
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 13888;
            handle->mclk = CUS_CMU_CLK_27MHZ;
            handle->interface_attr.attr_mipi.mipi_lane_num = 4;
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    sen_data = params->tMirror_reg[0].data;
    SENSOR_DMSG("[%s] mirror:%x\r\n", __FUNCTION__, sen_data & 0x66);
    switch(sen_data & 0x66)
    {
        case 0x00:
            *orit = CUS_ORIT_M0F0;
            break;
        case 0x06:
            *orit = CUS_ORIT_M1F0;
            break;
        case 0x60:
            *orit = CUS_ORIT_M0F1;
            break;
        case 0x66:
            *orit = CUS_ORIT_M1F1;
            break;
    }

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{

    sc132gs_params *params = (sc132gs_params *)handle->private_data;

    switch(orit) {
    case CUS_ORIT_M0F0:
    {
        params->tMirror_reg[0].data = 0;
        //params->tMirror_reg[1].data = 8;
        params->orient_dirty = true;
    }
    break;
    case CUS_ORIT_M1F0:
    {
        params->tMirror_reg[0].data = 6;
        //params->tMirror_reg[1].data = 8;
        params->orient_dirty = true;
    }
    break;
    case CUS_ORIT_M0F1:
    {
        params->tMirror_reg[0].data = 0x60;
        //params->tMirror_reg[1].data = 8;
        params->orient_dirty = true;
    }
    break;
    case CUS_ORIT_M1F1:
    {
        params->tMirror_reg[0].data = 0x66;
        //params->tMirror_reg[1].data = 8;
        params->orient_dirty = true;
    }
    break;
}

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", orit);

  return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}


static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0;
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > 2 * (params->expo.vts) -10){
        vts = (params->expo.line + 9)/2;
    }else{
        vts = params->expo.vts;
    }
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;
}

#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap) {
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else     return FAIL;
    return SUCCESS;
}
#endif

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
        if(params->orient_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            params->orient_dirty = false;
        }
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
            params->reg_dirty = false;
        }
#if ENABLE_NR   
        if(params->nr_dirty)
        {
            SensorReg_Write(0x3812,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
            params->nr_dirty = false;   
            //printk("nr_reg update --------------------- \n");
            SensorReg_Write(0x3812,0x30);
        }
#endif   
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    int rc=0;
    u32 lines = 0;
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    lines |= (u32)(params->tExpo_reg[0].data&0x0f)<<16;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xf0)<<0;
    lines >>= 4;
    *us = (lines*Preview_line_period)/1000/2; //return us

  SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
  return rc;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    int i;
    u32 half_lines = 0,vts = 0;
    sc132gs_params *params = (sc132gs_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, params->tExpo_reg, sizeof(expo_reg));

    // half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    half_lines = (1000*us)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<=1) half_lines=1;
    if (half_lines >  (params->expo.vts) - 8) {
        vts = (half_lines + 7)/2;
    }
    else
        vts=params->expo.vts;
    params->expo.line = half_lines;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;
    params->tExpo_reg[0].data = (half_lines>>16) & 0x0f;
    params->tExpo_reg[1].data = (half_lines>>8) & 0xff;
    params->tExpo_reg[2].data = (half_lines>>0) & 0xf0;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tExpo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;

    return rc;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    sc132gs_params *params = (sc132gs_params *)handle->private_data;

    u8 i=0 , Coarse_gain = 1,DIG_gain=1;
   //  u32 Fine_againx64 = 64,Fine_dgainx128 = 128;
    // u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;
    u32 Dcg_gainx100 = 1, ANA_Fine_gainx64 = 1,DIG_Fine_gainx1000 =1;
    u8 Dcg_gain_reg = 0, Coarse_gain_reg = 0,DIG_gain_reg=0, ANA_Fine_gain_reg= 0x20,DIG_Fine_gain_reg=0x80;


    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x40},
    };
    I2C_ARRAY nr_reg_temp[] = {
        {0x33fa,0x01},
        {0x3317,0xf0},
        // {0x5799,0x07},
    };
    
    memcpy(gain_reg_temp, params->tGain_reg, sizeof(gain_reg));
    memcpy(nr_reg_temp, params->tNr_reg, sizeof(nr_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

    if (gain < 1856) // start again  1.813 * 1024
    {
        Dcg_gainx100 = 1000;      Coarse_gain = 1;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 0;  Coarse_gain_reg = 0x03; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain < 3712) // 3.625 * 1024 agc
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 1;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x23; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;
    }
    else if (gain < 7424) // 7.250 * 1024
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 2;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x27; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;

    }
    else if (gain < 14848)// 14.500 * 1024
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 4;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x2f; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;

    }
    else if (gain < 29232)// end again 28.547 * 1024
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 8;     DIG_gain=1;       DIG_Fine_gainx1000=1000;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x0;  DIG_Fine_gain_reg=0x80;

    }
    else if (gain < 29232 * 2) // start dgain
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 8;     DIG_gain=1;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x0;  ANA_Fine_gain_reg=0x3f;

    }
    else if (gain < 29232 * 4)
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 8;     DIG_gain=2;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x1;  ANA_Fine_gain_reg=0x3f;
    }
    else if (gain < 29232 * 8)
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 8;     DIG_gain=4;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x3;  ANA_Fine_gain_reg=0x3f;
    }
   else if (gain < 29232 * 16)
    {
       Dcg_gainx100 = 1813;      Coarse_gain = 8;      DIG_gain=8;        ANA_Fine_gainx64=127;
       Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0x7;  ANA_Fine_gain_reg=0x3f;
    }
    else if (gain <= 920812)
    {
        Dcg_gainx100 = 1813;      Coarse_gain = 8;     DIG_gain=16;       ANA_Fine_gainx64=127;
        Dcg_gain_reg = 1;  Coarse_gain_reg = 0x3f; DIG_gain_reg=0xF;  ANA_Fine_gain_reg=0x3f;
    }

    if(gain < 1856)
    {
        //ANA_Fine_gainx64 = 1000 * 100 * gain / (Dcg_gainx100 * Coarse_gain) / 1024;
        ANA_Fine_gain_reg = abs(1000 * gain / (Dcg_gainx100 * Coarse_gain) / 32);
        // SENSOR_DMSG("[%s]  gain : %d ,%d ,%d\n", __FUNCTION__,Dcg_gainx100,Coarse_gain,ANA_Fine_gainx64);
        // SENSOR_DMSG("[%s]  reg : 0x%x ,0x%x ,0x%x\n", __FUNCTION__,Dcg_gain_reg,Coarse_gain_reg,ANA_Fine_gain_reg);
    }else if(gain == 1856) // || gain == 1552)
    {
        ANA_Fine_gain_reg = 0x39;
    }
    else if(gain < 29232)
    {
        //ANA_Fine_gainx64 = 1000 * 100 * gain / (Dcg_gainx100 * Coarse_gain) / 1024;
        ANA_Fine_gain_reg = abs(1000 * gain / (Dcg_gainx100 * Coarse_gain) / 32);
        // SENSOR_DMSG("[%s]  gain : %d ,%d ,%d\n", __FUNCTION__,Dcg_gainx100,Coarse_gain, ANA_Fine_gainx64);
        // SENSOR_DMSG("[%s]  reg : 0x%x ,0x%x ,0x%x\n", __FUNCTION__,Dcg_gain_reg,Coarse_gain_reg,ANA_Fine_gain_reg);
    }else{
        DIG_Fine_gain_reg = abs(8000 * gain /(Dcg_gainx100 * Coarse_gain * DIG_gain) / ANA_Fine_gainx64);
        // SENSOR_DMSG("[%s]  gain : %d,%d\n", __FUNCTION__,DIG_gain,DIG_Fine_gainx1000);
        // SENSOR_DMSG("[%s]  reg : 0x%x ,0x%x\n", __FUNCTION__,DIG_gain_reg,DIG_Fine_gain_reg);
    }

    params->tGain_reg[3].data = ANA_Fine_gain_reg;
    params->tGain_reg[2].data = Coarse_gain_reg;
    params->tGain_reg[1].data = DIG_Fine_gain_reg;
    params->tGain_reg[0].data = DIG_gain_reg & 0xF;
    

    for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tGain_reg[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    
#if ENABLE_NR
    // logic
    if (gain_reg_temp[2].data < 0x07) {         // gain < x2
        params->tNr_reg[0].data = 0x01;
        params->tNr_reg[1].data = 0xf0;
    } 
    else // if (gain_reg_temp[2].data < 0x0F) {     // gain >= x2
    {
        params->tNr_reg[0].data = 0x02;
        params->tNr_reg[1].data = 0x0a;
    }
    // highTemp dpc
    /* if (gain >= 6144) // x16, x6
    {
        params->tNr_reg[2].data = 0x07; 
    }
    else if (gain <= 4096) // x10, x4
    {
        params->tNr_reg[2].data = 0x00; 
    } */
    for (i = 0; i < sizeof(nr_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tNr_reg[i].data != nr_reg_temp[i].data) 
      {
        params->nr_dirty = true;
        break;
      }
    }
#endif

    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1; // 30;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1024;
  *max = SENSOR_MAXGAIN*1024;
  return SUCCESS;
}

static int sc132gs_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period; // (Preview_line_period * 3) / 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc132gs_params *params;
    int res;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (sc132gs_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"sc132gs_MIPI");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0;
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = sc132gs_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = sc132gs_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = sc132gs_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = sc132gs_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = sc132gs_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = sc132gs_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = sc132gs_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = sc132gs_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, sc132gs_mipi_linear[res].senstr.strResDesc);
    }

    // i2c

    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED;

    //polarity
    /////////////////////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;


    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_linear_2lane_30fps;

    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID   ;

    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien      ;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien      ;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS      ;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS      ;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = sc132gs_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;

    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = sc132gs_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line= 1000;
    params->reg_dirty = false;
    params->nr_dirty = false;
    
    params->orient_dirty = false;

    //handle->snr_pad_group = SENSOR_PAD_GROUP_SET;

    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}
SENSOR_DRV_ENTRY_IMPL_END_EX( SC132gs,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            sc132gs_params
                         );

