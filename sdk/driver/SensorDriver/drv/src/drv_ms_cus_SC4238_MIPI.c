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

#if 0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <cam_os_wrapper.h>
#include <linux/kernel.h>
#include <drv_sensor_common.h>
#include <sensor_i2c_api.h>
#include <linux/delay.h>
#include <drv_ms_cus_sensor.h>
#include <drv_sensor.h>
#else
#include <drv_sensor_common.h>
#include <sensor_i2c_api.h>
#include <drv_sensor.h>
#include <drv_sensor_init_table.h> //TODO: move this header to drv_sensor_common.h
//#include <SC4238_init_table.h>
#endif

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC4238_HDR);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
#define SENSOR_MIPI_LANE_NUM_DOL (4)
#define SENSOR_MIPI_HDR_MODE (2) //0: Non-HDR mode. 1:Sony DOL mode. 2：CUS_HDR_MODE_DCG
//MIPI config end.
//============================================

#define R_GAIN_REG 1
#define G_GAIN_REG 2
#define B_GAIN_REG 3


//#undef SENSOR_DBG
//#define SENSOR_DBG 0

///////////////////////////////////////////////////////////////
//          @@@                                                                                       //
//       @   @@      ==  S t a r t * H e r e ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                           //
//         @@@@                                                                                  //
//                                                                                                     //
//      Start Step 1 --  show preview on LCM                                         //
//                                                                                                    �@//
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_HDR_DOL     CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_MAXGAIN      (15875*315)/10000   /////sensor again 15.875 dgain=31.5
//#define SENSOR_MAXGAIN      ((158*31)/10)
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_BG//CUS_BAYER_GR
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define lane_number 2
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_27MHZ

//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
//#define Preview_line_period                //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define vts_30fps
u32 Preview_line_period;
u32 vts_30fps = 1562;
#define Preview_line_period_HDR_DOL 10670
#define vts_30fps_HDR_DOL 3124
#define Preview_WIDTH       2688                    //resolution Width when preview
#define Preview_HEIGHT      1520                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x60                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;
#define sample 1
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
static int g_sensor_ae_min_gain = 1024;

CUS_MCLK_FREQ UseParaMclk(void);

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
        float sclk;
        u32 hts;
        u32 vts;
        u32 ho;
        u32 xinc;
        u32 line_freq;
        u32 us_per_line;
        u32 final_us;
        u32 final_gain;
        u32 back_pv_us;
        u32 half_lines;
        u32 half_line;
        u32 fps;
        u32 preview_fps;
        u32 max_short_exp;
        u32 line;
    } expo;
    struct {
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_mf;
    bool reg_dirty;
    bool ori_dirty;
    CUS_CAMSENSOR_ORIT cur_orien;
} sc4238_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x42},
    {0x3108, 0x35},
};

I2C_ARRAY Sensor_init_table_4M30fps_2lane[] =
{
        {0x0103,0x01},
        {0x0100,0x00},
        {0x36e9,0xa0},
        {0x36f9,0xd7},
        {0x3638,0x2a},
        {0x363b,0x03},
        {0x3352,0x02},
        {0x3356,0x1f},
        {0x336d,0x01},
        {0x337f,0x2d},
        {0x3320,0x09},
        {0x3e14,0xb1},
        {0x3253,0x06},
        {0x391b,0x80},
        {0x391c,0x0f},
        {0x3905,0xd8},
        {0x3399,0xff},
        {0x4501,0xb4},
        {0x3106,0x81},
        {0x3634,0x34},
        {0x3630,0xa8},
        {0x3631,0x80},
        {0x4509,0x20},
        {0x320c,0x05},
        {0x320d,0x46},
        {0x320e,0x05},
        {0x320f,0xdc},
        {0x36ea,0x37},
        {0x36eb,0x0c},
        {0x36ec,0x05},
        {0x36ed,0x24},
        {0x4837,0x20},
        {0x36fa,0x25},
        {0x36fb,0x18},
        {0x36fc,0x01},
        {0x36fd,0x04},
        {0x3308,0x10},
        {0x3309,0x40},
        {0x331f,0x39},
        {0x3018,0x33},
        {0x3031,0x0a},
        {0x3037,0x20},
        {0x3038,0x22},
        {0x3366,0x92},
        {0x337a,0x08},
        {0x337b,0x10},
        {0x33a3,0x0c},
        {0x3314,0x94},
        {0x330e,0x14},
        {0x334c,0x10},
        {0x3363,0x00},
        {0x3273,0x01},
        {0x3933,0x28},
        {0x3934,0x20},
        {0x3942,0x08},
        {0x3943,0x28},
        {0x3980,0x00},
        {0x3981,0x00},
        {0x3982,0x00},
        {0x3983,0x00},
        {0x3984,0x00},
        {0x3985,0x00},
        {0x3986,0x00},
        {0x3987,0x00},
        {0x3988,0x00},
        {0x3989,0x00},
        {0x398a,0x00},
        {0x398b,0x04},
        {0x398c,0x00},
        {0x398d,0x04},
        {0x398e,0x00},
        {0x398f,0x08},
        {0x3990,0x00},
        {0x3991,0x10},
        {0x3992,0x03},
        {0x3993,0xd8},
        {0x3994,0x03},
        {0x3995,0xe0},
        {0x3996,0x03},
        {0x3997,0xf0},
        {0x3998,0x03},
        {0x3999,0xf8},
        {0x399a,0x00},
        {0x399b,0x00},
        {0x399c,0x00},
        {0x399d,0x08},
        {0x399e,0x00},
        {0x399f,0x10},
        {0x39a0,0x00},
        {0x39a1,0x18},
        {0x39a2,0x00},
        {0x39a3,0x28},
        {0x39af,0x58},
        {0x39b5,0x30},
        {0x39b6,0x00},
        {0x39b7,0x34},
        {0x39b8,0x00},
        {0x39bc,0x00},
        {0x39bd,0x00},
        {0x39be,0x00},
        {0x39db,0x20},
        {0x39dc,0x00},
        {0x39de,0x20},
        {0x39df,0x00},
        {0x39e0,0x00},
        {0x39e1,0x00},
        {0x39e2,0x00},
        {0x39e3,0x00},
        {0x3940,0x6c},
        {0x3200,0x00},
        {0x3201,0x40},
        {0x3202,0x00},
        {0x3203,0x2c},
        {0x3204,0x0a},
        {0x3205,0x47},
        {0x3206,0x05},
        {0x3207,0xd3},
        {0x3208,0x0a},
        {0x3209,0x00},
        {0x320a,0x05},
        {0x320b,0xa0},
        {0x3211,0x04},
        {0x3213,0x04},
        {0x3364,0x1e},
        {0x3301,0x30},
        {0x3393,0x30},
        {0x3394,0x30},
        {0x3395,0x30},
        {0x3390,0x08},
        {0x3391,0x08},
        {0x3392,0x08},
        {0x3670,0x48},
        {0x366e,0x04},
        {0x3690,0x43},
        {0x3691,0x43},
        {0x3692,0x43},
        {0x369c,0x08},
        {0x369d,0x08},
        {0x3699,0x80},
        {0x369a,0x9f},
        {0x369b,0x9f},
        {0x36a2,0x08},
        {0x36a3,0x08},
        {0x360f,0x05},
        {0x3671,0xee},
        {0x3672,0x0e},
        {0x3673,0x0e},
        {0x367a,0x08},
        {0x367b,0x08},
        {0x5784,0x10},
        {0x5785,0x08},
        {0x5787,0x06},
        {0x5788,0x06},
        {0x5789,0x00},
        {0x578a,0x06},
        {0x578b,0x06},
        {0x578c,0x00},
        {0x5790,0x10},
        {0x5791,0x10},
        {0x5792,0x00},
        {0x5793,0x10},
        {0x5794,0x10},
        {0x5795,0x00},
        {0x57c4,0x10},
        {0x57c5,0x08},
        {0x57c7,0x06},
        {0x57c8,0x06},
        {0x57c9,0x00},
        {0x57ca,0x06},
        {0x57cb,0x06},
        {0x57cc,0x00},
        {0x57d0,0x10},
        {0x57d1,0x10},
        {0x57d2,0x00},
        {0x57d3,0x10},
        {0x57d4,0x10},
        {0x57d5,0x00},
        {0x33e0,0xa0},
        {0x33e1,0x08},
        {0x33e2,0x00},
        {0x33e3,0x10},
        {0x33e4,0x10},
        {0x33e5,0x00},
        {0x33e6,0x10},
        {0x33e7,0x10},
        {0x33e8,0x00},
        {0x33e9,0x10},
        {0x33ea,0x16},
        {0x33eb,0x00},
        {0x33ec,0x10},
        {0x33ed,0x18},
        {0x33ee,0xa0},
        {0x33ef,0x08},
        {0x33f4,0x00},
        {0x33f5,0x10},
        {0x33f6,0x10},
        {0x33f7,0x00},
        {0x33f8,0x10},
        {0x33f9,0x10},
        {0x33fa,0x00},
        {0x33fb,0x10},
        {0x33fc,0x16},
        {0x33fd,0x00},
        {0x33fe,0x10},
        {0x33ff,0x18},
        {0x3636,0x20},
        {0x3637,0x11},
        {0x3e25,0x03},
        {0x3e26,0x40},
        {0x331e,0x19},
        {0x3304,0x20},
        {0x330b,0xe8},
        {0x3306,0x68},
        {0x3907,0x01},
        {0x3908,0x11},
        {0x39c5,0x21},
        {0x39c8,0x00},
        {0x3635,0x60},
        {0x3902,0xc5},
        {0x301f,0x12},
        {0x3e00,0x00},
        {0x3e01,0xba},
        {0x3e02,0xe0},
        {0x3e03,0x0b},
        {0x3e06,0x00},
        {0x3e07,0x80},
        {0x3e08,0x03},
        {0x3e09,0x40},
        {0x3633,0x43},
        {0x3622,0xee},
        {0x363a,0x80},
        {0x36e9,0x20},
        {0x36f9,0x57},
        {0x0100,0x01},

};

I2C_ARRAY Sensor_init_table_4M30fps[] = //4 Lane
{
#if 0
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0xd0}, //bypass pll
    {0x36f9,0xa0},  //bypass pll
    {0x3638,0x2a},
    {0x3304,0x30},
    {0x331e,0x29},
    {0x363b,0x03},
    {0x3352,0x02},
    {0x3356,0x1f},
    {0x336d,0x01},
    {0x337f,0x2d},
    {0x3320,0x09},
    {0x3e14,0xb1},
    {0x3253,0x06},
    {0x391b,0x80},
    {0x391c,0x0f},
    {0x3905,0xd8},
    {0x3399,0xff},
    {0x3908,0x11},
    {0x4501,0xb4},
    {0x3106,0x81},
    {0x3634,0x34},
    {0x3630,0xa8},
    {0x3631,0x80},
    {0x4509,0x20},
    {0x3200,0x00},
    {0x3201,0x00},
    {0x3204,0x0a},
    {0x3205,0x87},
    {0x3202,0x00},
    {0x3203,0x00},
    {0x3206,0x05},
    {0x3207,0xf7},
    {0x3208,0x0a},
    {0x3209,0x80},
    {0x320a,0x05},
    {0x320b,0xf0},
    {0x3211,0x04},
    {0x3213,0x04},
    {0x320c,0x05},
    {0x320d,0xa0},
    {0x320e,0x06},
    {0x320f,0x1a},
    {0x36ea,0x27},
    {0x36eb,0x14},
    {0x36ec,0x05},
    {0x36ed,0x14},
    {0x4837,0x3b},
    {0x36fa,0x27},
    {0x36fb,0x09},
    {0x36fc,0x01},
    {0x36fd,0x14},
    {0x3308,0x10},
    {0x3306,0x70},
    {0x3309,0x50},
    {0x331f,0x49},
    {0x3018,0x73},
    {0x3031,0x0a},
    {0x3037,0x20},
    {0x3038,0x22},
    {0x3366,0x92},
    {0x337a,0x08},
    {0x337b,0x10},
    {0x33a3,0x0c},
    {0x3314,0x94},
    {0x330e,0x14},
    {0x334c,0x10},
    {0x3363,0x00},
    {0x3273,0x01},
    {0x3933,0x28},
    {0x3934,0x20},
    {0x3942,0x08},
    {0x3943,0x28},
    {0x3980,0x00},
    {0x3981,0x00},
    {0x3982,0x00},
    {0x3983,0x00},
    {0x3984,0x00},
    {0x3985,0x00},
    {0x3986,0x00},
    {0x3987,0x00},
    {0x3988,0x00},
    {0x3989,0x00},
    {0x398a,0x00},
    {0x398b,0x04},
    {0x398c,0x00},
    {0x398d,0x04},
    {0x398e,0x00},
    {0x398f,0x08},
    {0x3990,0x00},
    {0x3991,0x10},
    {0x3992,0x03},
    {0x3993,0xd8},
    {0x3994,0x03},
    {0x3995,0xe0},
    {0x3996,0x03},
    {0x3997,0xf0},
    {0x3998,0x03},
    {0x3999,0xf8},
    {0x399a,0x00},
    {0x399b,0x00},
    {0x399c,0x00},
    {0x399d,0x08},
    {0x399e,0x00},
    {0x399f,0x10},
    {0x39a0,0x00},
    {0x39a1,0x18},
    {0x39a2,0x00},
    {0x39a3,0x28},
    {0x39af,0x58},
    {0x39b5,0x30},
    {0x39b6,0x00},
    {0x39b7,0x34},
    {0x39b8,0x00},
    {0x39bc,0x00},
    {0x39bd,0x00},
    {0x39be,0x00},
    {0x39db,0x20},
    {0x39dc,0x00},
    {0x39de,0x20},
    {0x39df,0x00},
    {0x39e0,0x00},
    {0x39e1,0x00},
    {0x39e2,0x00},
    {0x39e3,0x00},
    {0x3940,0x6c},
    {0x3364,0x1e},
    {0x3301,0x30},
    {0x3393,0x30},
    {0x3394,0x30},
    {0x3395,0x30},
    {0x3390,0x08},
    {0x3391,0x08},
    {0x3392,0x08},
    {0x3670,0x48},
    {0x366e,0x04},
    {0x3690,0x43},
    {0x3691,0x43},
    {0x3692,0x43},
    {0x369c,0x08},
    {0x369d,0x08},
    {0x3699,0x80},
    {0x369a,0x9f},
    {0x369b,0x9f},
    {0x36a2,0x08},
    {0x36a3,0x08},
    {0x360f,0x05},
    {0x3671,0xee},
    {0x3672,0x0e},
    {0x3673,0x0e},
    {0x367a,0x08},
    {0x367b,0x08},
    {0x5784,0x10},
    {0x5785,0x08},
    {0x5787,0x06},
    {0x5788,0x06},
    {0x5789,0x00},
    {0x578a,0x06},
    {0x578b,0x06},
    {0x578c,0x00},
    {0x5790,0x10},
    {0x5791,0x10},
    {0x5792,0x00},
    {0x5793,0x10},
    {0x5794,0x10},
    {0x5795,0x00},
    {0x57c4,0x10},
    {0x57c5,0x08},
    {0x57c7,0x06},
    {0x57c8,0x06},
    {0x57c9,0x00},
    {0x57ca,0x06},
    {0x57cb,0x06},
    {0x57cc,0x00},
    {0x57d0,0x10},
    {0x57d1,0x10},
    {0x57d2,0x00},
    {0x57d3,0x10},
    {0x57d4,0x10},
    {0x57d5,0x00},
    {0x33e0,0xa0},
    {0x33e1,0x08},
    {0x33e2,0x00},
    {0x33e3,0x10},
    {0x33e4,0x10},
    {0x33e5,0x00},
    {0x33e6,0x10},
    {0x33e7,0x10},
    {0x33e8,0x00},
    {0x33e9,0x10},
    {0x33ea,0x16},
    {0x33eb,0x00},
    {0x33ec,0x10},
    {0x33ed,0x18},
    {0x33ee,0xa0},
    {0x33ef,0x08},
    {0x33f4,0x00},
    {0x33f5,0x10},
    {0x33f6,0x10},
    {0x33f7,0x00},
    {0x33f8,0x10},
    {0x33f9,0x10},
    {0x33fa,0x00},
    {0x33fb,0x10},
    {0x33fc,0x16},
    {0x33fd,0x00},
    {0x33fe,0x10},
    {0x33ff,0x18},
    {0x3636,0x20},
    {0x3637,0x11},
    {0x3e25,0x03},
    {0x3e26,0x40},
    {0x39c8,0x00},
    {0x3635,0x60},
    {0x301f,0x04},
    {0x39c5,0x21},
    {0x3625,0x0a},
    {0x3902,0xc5},
    {0x330b,0xe8},
    {0x3e00,0x00},
    {0x3e01,0xc2},
    {0x3e02,0xa0},
    {0x3e03,0x0b},
    {0x3e06,0x00},
    {0x3e07,0x80},
    {0x3e08,0x03},
    {0x3e09,0x40},
    {0x3633,0x43},
    {0x3622,0xee},
    {0x363a,0x80},
    {0x36e9,0x50},
    {0x36f9,0x20},
    {0x0100,0x01},
    {0xffff,0x0a},
#else
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x3018,0x72},
    {0x301f,0x29},
    {0x3031,0x0a},
    {0x3037,0x20},
    {0x3038,0x22},
    {0x3106,0x81},
    {0x3200,0x00},
    {0x3201,0x00},
    {0x3202,0x00},
    {0x3203,0x00},
    {0x3204,0x0a},
    {0x3205,0x87},
    {0x3206,0x05},
    {0x3207,0xf7},
    {0x3208,0x0a},
    {0x3209,0x80},
    {0x320a,0x05},
    {0x320b,0xf0},
    {0x320c,0x05},
    {0x320d,0xa0},
    {0x320e,0x06},
    {0x320f,0x1a},
    {0x3211,0x04},
    {0x3213,0x04},
    {0x3253,0x06},
    {0x3273,0x01},
    {0x3301,0x30},
    {0x3304,0x30},
    {0x3306,0x70},
    {0x3308,0x10},
    {0x3309,0x50},
    {0x330b,0xe8},
    {0x330e,0x14},
    {0x3314,0x94},
    {0x331e,0x29},
    {0x331f,0x49},
    {0x3320,0x09},
    {0x334c,0x10},
    {0x3352,0x02},
    {0x3356,0x1f},
    {0x3363,0x00},
    {0x3364,0x1e},
    {0x3366,0x92},
    {0x336d,0x01},
    {0x337a,0x08},
    {0x337b,0x10},
    {0x337f,0x2d},
    {0x3390,0x08},
    {0x3391,0x08},
    {0x3392,0x08},
    {0x3393,0x30},
    {0x3394,0x30},
    {0x3395,0x30},
    {0x3399,0xff},
    {0x33a3,0x0c},
    {0x33e0,0xa0},
    {0x33e1,0x08},
    {0x33e2,0x00},
    {0x33e3,0x10},
    {0x33e4,0x10},
    {0x33e5,0x00},
    {0x33e6,0x10},
    {0x33e7,0x10},
    {0x33e8,0x00},
    {0x33e9,0x10},
    {0x33ea,0x16},
    {0x33eb,0x00},
    {0x33ec,0x10},
    {0x33ed,0x18},
    {0x33ee,0xa0},
    {0x33ef,0x08},
    {0x33f4,0x00},
    {0x33f5,0x10},
    {0x33f6,0x10},
    {0x33f7,0x00},
    {0x33f8,0x10},
    {0x33f9,0x10},
    {0x33fa,0x00},
    {0x33fb,0x10},
    {0x33fc,0x16},
    {0x33fd,0x00},
    {0x33fe,0x10},
    {0x33ff,0x18},
    {0x360f,0x05},
    {0x3622,0xee},
    {0x3625,0x0a},
    {0x3630,0xa8},
    {0x3631,0x80},
    {0x3633,0x43},
    {0x3634,0x34},
    {0x3635,0x60},
    {0x3636,0x20},
    {0x3637,0x11},
    {0x3638,0x2a},
    {0x363a,0x80},
    {0x363b,0x03},
    {0x3641,0x00},
    {0x366e,0x04},
    {0x3670,0x48},
    {0x3671,0xee},
    {0x3672,0x0e},
    {0x3673,0x0e},
    {0x367a,0x08},
    {0x367b,0x08},
    {0x3690,0x43},
    {0x3691,0x43},
    {0x3692,0x43},
    {0x3699,0x80},
    {0x369a,0x9f},
    {0x369b,0x9f},
    {0x369c,0x08},
    {0x369d,0x08},
    {0x36a2,0x08},
    {0x36a3,0x08},
    {0x36ea,0x36},
    {0x36eb,0x14},
    {0x36ec,0x05},
    {0x36ed,0x24},
    {0x36fa,0x36},
    {0x36fb,0x09},
    {0x36fc,0x00},
    {0x36fd,0x24},
    {0x3902,0xc5},
    {0x3905,0xd8},
    {0x3908,0x11},
    {0x391b,0x80},
    {0x391c,0x0f},
    {0x3933,0x28},
    {0x3934,0x20},
    {0x3940,0x6c},
    {0x3942,0x08},
    {0x3943,0x28},
    {0x3980,0x00},
    {0x3981,0x00},
    {0x3982,0x00},
    {0x3983,0x00},
    {0x3984,0x00},
    {0x3985,0x00},
    {0x3986,0x00},
    {0x3987,0x00},
    {0x3988,0x00},
    {0x3989,0x00},
    {0x398a,0x00},
    {0x398b,0x04},
    {0x398c,0x00},
    {0x398d,0x04},
    {0x398e,0x00},
    {0x398f,0x08},
    {0x3990,0x00},
    {0x3991,0x10},
    {0x3992,0x03},
    {0x3993,0xd8},
    {0x3994,0x03},
    {0x3995,0xe0},
    {0x3996,0x03},
    {0x3997,0xf0},
    {0x3998,0x03},
    {0x3999,0xf8},
    {0x399a,0x00},
    {0x399b,0x00},
    {0x399c,0x00},
    {0x399d,0x08},
    {0x399e,0x00},
    {0x399f,0x10},
    {0x39a0,0x00},
    {0x39a1,0x18},
    {0x39a2,0x00},
    {0x39a3,0x28},
    {0x39af,0x58},
    {0x39b5,0x30},
    {0x39b6,0x00},
    {0x39b7,0x34},
    {0x39b8,0x00},
    {0x39b9,0x00},
    {0x39ba,0x34},
    {0x39bb,0x00},
    {0x39bc,0x00},
    {0x39bd,0x00},
    {0x39be,0x00},
    {0x39bf,0x00},
    {0x39c0,0x00},
    {0x39c1,0x00},
    {0x39c5,0x21},
    {0x39c8,0x00},
    {0x39db,0x20},
    {0x39dc,0x00},
    {0x39de,0x20},
    {0x39df,0x00},
    {0x39e0,0x00},
    {0x39e1,0x00},
    {0x39e2,0x00},
    {0x39e3,0x00},
    {0x3e00,0x00},
    {0x3e01,0xc2},
    {0x3e02,0xa0},
    {0x3e03,0x0b},
    {0x3e06,0x00},
    {0x3e07,0x80},
    {0x3e08,0x03},
    {0x3e09,0x40},
    {0x3e14,0xb1},
    {0x3e25,0x03},
    {0x3e26,0x40},
    {0x4501,0xb4},
    {0x4509,0x20},
    {0x4800,0x64},
    {0x4818,0x00},
    {0x4819,0x30},
    {0x481a,0x00},
    {0x481b,0x0b},
    {0x481c,0x00},
    {0x481d,0xc8},
    {0x4821,0x02},
    {0x4822,0x00},
    {0x4823,0x03},
    {0x4828,0x00},
    {0x4829,0x02},
    {0x4837,0x3b},
    {0x5784,0x10},
    {0x5785,0x08},
    {0x5787,0x06},
    {0x5788,0x06},
    {0x5789,0x00},
    {0x578a,0x06},
    {0x578b,0x06},
    {0x578c,0x00},
    {0x5790,0x10},
    {0x5791,0x10},
    {0x5792,0x00},
    {0x5793,0x10},
    {0x5794,0x10},
    {0x5795,0x00},
    {0x57c4,0x10},
    {0x57c5,0x08},
    {0x57c7,0x06},
    {0x57c8,0x06},
    {0x57c9,0x00},
    {0x57ca,0x06},
    {0x57cb,0x06},
    {0x57cc,0x00},
    {0x57d0,0x10},
    {0x57d1,0x10},
    {0x57d2,0x00},
    {0x57d3,0x10},
    {0x57d4,0x10},
    {0x57d5,0x00},
    {0x5988,0x86},
    {0x598e,0x05},
    {0x598f,0x6c},
    {0x36e9,0x54},
    {0x36f9,0x54},
    {0x0100,0x01},
    {0xffff,0x0a},
#endif
};

I2C_ARRAY Sensor_init_table_HDR_DOL_4lane[] =
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0xd0},
    {0x36f9,0xa0},
    {0x3638,0x2a},
    {0x3304,0x30},
    {0x331e,0x29},
    {0x363b,0x03},
    {0x3352,0x02},
    {0x3356,0x1f},
    {0x336d,0x01},
    {0x337f,0x2d},
    {0x3320,0x09},
    {0x3e14,0xb1},
    {0x3253,0x06},
    {0x391b,0x80},
    {0x391c,0x0f},
    {0x3399,0xff},
    {0x3908,0x11},
    {0x3106,0x81},
    {0x3634,0x34},
    {0x3630,0xa8},
    {0x3631,0x80},
    {0x3200,0x00},
    {0x3201,0x00},
    {0x3204,0x0a},
    {0x3205,0x87},
    {0x3202,0x00},
    {0x3203,0x00},
    {0x3206,0x05},
    {0x3207,0xf7},
    {0x3208,0x0a},
    {0x3209,0x80},
    {0x320a,0x05},
    {0x320b,0xf0},
    {0x3211,0x04},
    {0x3213,0x04},
    {0x320c,0x05},
    {0x320d,0xa0},
    {0x36ea,0x27},
    {0x36eb,0x04},
    {0x36ec,0x05},
    {0x36ed,0x14},
    {0x36fa,0x27},
    {0x36fb,0x09},
    {0x36fc,0x01},
    {0x36fd,0x14},
    {0x3308,0x10},
    {0x3306,0x70},
    {0x3309,0x60},
    {0x331f,0x59},
    {0x4501,0xa4},
    {0x4509,0x10},
    {0x3018,0x73},
    {0x3031,0x0a},
    {0x3037,0x20},
    {0x3038,0x22},
    {0x3366,0x92},
    {0x337a,0x08},
    {0x337b,0x10},
    {0x33a3,0x0c},
    {0x3314,0x94},
    {0x330e,0x1a},
    {0x334c,0x10},
    {0x3363,0x00},
    {0x3273,0x01},
    {0x3933,0x28},
    {0x3934,0x20},
    {0x3942,0x08},
    {0x3943,0x28},
    {0x3980,0x00},
    {0x3981,0x00},
    {0x3982,0x00},
    {0x3983,0x00},
    {0x3984,0x00},
    {0x3985,0x00},
    {0x3986,0x00},
    {0x3987,0x00},
    {0x3988,0x00},
    {0x3989,0x00},
    {0x3992,0x03},
    {0x3993,0xd8},
    {0x3994,0x03},
    {0x3995,0xe0},
    {0x3996,0x03},
    {0x3997,0xf0},
    {0x3998,0x03},
    {0x3999,0xf8},
    {0x399a,0x00},
    {0x399b,0x00},
    {0x399c,0x00},
    {0x399d,0x08},
    {0x399e,0x00},
    {0x399f,0x10},
    {0x39a0,0x00},
    {0x39a1,0x18},
    {0x39a2,0x00},
    {0x39a3,0x28},
    {0x39af,0x58},
    {0x39b5,0x30},
    {0x39b6,0x00},
    {0x39b7,0x34},
    {0x39b8,0x00},
    {0x39bc,0x00},
    {0x39bd,0x00},
    {0x39be,0x00},
    {0x39db,0x20},
    {0x39dc,0x00},
    {0x39de,0x20},
    {0x39df,0x00},
    {0x39e0,0x00},
    {0x39e1,0x00},
    {0x39e2,0x00},
    {0x39e3,0x00},
    {0x3940,0x68},
    {0x398a,0x00},
    {0x398b,0x08},
    {0x398c,0x00},
    {0x398d,0x10},
    {0x398e,0x00},
    {0x398f,0x18},
    {0x3990,0x00},
    {0x3991,0x20},
    {0x3364,0x1e},
    {0x3301,0x30},
    {0x3393,0x30},
    {0x3394,0x30},
    {0x3395,0x30},
    {0x3390,0x08},
    {0x3391,0x08},
    {0x3392,0x08},
    {0x3670,0x48},
    {0x366e,0x04},
    {0x3690,0x43},
    {0x3691,0x44},
    {0x3692,0x44},
    {0x369c,0x08},
    {0x369d,0x08},
    {0x3699,0x9f},
    {0x369a,0x9f},
    {0x369b,0x9f},
    {0x36a2,0x08},
    {0x36a3,0x08},
    {0x360f,0x05},
    {0x3671,0xee},
    {0x3672,0x6e},
    {0x3673,0x6e},
    {0x367a,0x08},
    {0x367b,0x08},
    {0x5784,0x10},
    {0x5785,0x08},
    {0x5787,0x06},
    {0x5788,0x06},
    {0x5789,0x00},
    {0x578a,0x06},
    {0x578b,0x06},
    {0x578c,0x00},
    {0x5790,0x10},
    {0x5791,0x10},
    {0x5792,0x00},
    {0x5793,0x10},
    {0x5794,0x10},
    {0x5795,0x00},
    {0x57c4,0x10},
    {0x57c5,0x08},
    {0x57c7,0x06},
    {0x57c8,0x06},
    {0x57c9,0x00},
    {0x57ca,0x06},
    {0x57cb,0x06},
    {0x57cc,0x00},
    {0x57d0,0x10},
    {0x57d1,0x10},
    {0x57d2,0x00},
    {0x57d3,0x10},
    {0x57d4,0x10},
    {0x57d5,0x00},
    {0x33e0,0xa0},
    {0x33e1,0x08},
    {0x33e2,0x00},
    {0x33e3,0x10},
    {0x33e4,0x10},
    {0x33e5,0x00},
    {0x33e6,0x10},
    {0x33e7,0x10},
    {0x33e8,0x00},
    {0x33e9,0x10},
    {0x33ea,0x16},
    {0x33eb,0x00},
    {0x33ec,0x10},
    {0x33ed,0x18},
    {0x33ee,0xa0},
    {0x33ef,0x08},
    {0x33f4,0x00},
    {0x33f5,0x10},
    {0x33f6,0x10},
    {0x33f7,0x00},
    {0x33f8,0x10},
    {0x33f9,0x10},
    {0x33fa,0x00},
    {0x33fb,0x10},
    {0x33fc,0x16},
    {0x33fd,0x00},
    {0x33fe,0x10},
    {0x33ff,0x18},
    {0x3636,0x20},
    {0x3637,0x22},
    {0x3e25,0x03},
    {0x3e26,0x40},
    {0x330b,0xe0},
    {0x39c5,0x21},
    {0x39c8,0x00},
    {0x4837,0x1d},
    {0x3635,0x60},
    {0x3633,0x43},
    {0x3622,0xee},
    {0x363a,0x80},
    {0x3220,0x53},
    {0x4816,0x51},
    {0x320e,0x0c},
    {0x320f,0x34},
    {0x3250,0x3f},
    {0x3e23,0x00},
    {0x3e24,0xb6},
    {0x4500,0x08},
    {0x3225,0x02},
    {0x3235,0x18},
    {0x3236,0x67},
    {0x3237,0x02},
    {0x3238,0x90},
    {0x3905,0x98},
    {0x3902,0xc5},
    {0x39e8,0x03},
    {0x301f,0x03},
    {0x3e03,0x0b},
    {0x3e00,0x01},
    {0x3e01,0x6e},
    {0x3e02,0xe0},
    {0x3e04,0x16},
    {0x3e05,0x00},
    {0x3e06,0x00},
    {0x3e07,0x80},
    {0x3e08,0x03},
    {0x3e09,0x40},
    {0x3e10,0x00},
    {0x3e11,0x80},
    {0x3e12,0x03},
    {0x3e13,0x40},
    {0x36e9,0x50},
    {0x36f9,0x20},
    {0x0100,0x01},
};

FINE_GAIN fine_again[] = {
//gain map update for 1/64 precision
    {1024, 0x40},
    {1040, 0x41},
    {1056, 0x42},
    {1072, 0x43},
    {1088, 0x44},
    {1104, 0x45},
    {1120, 0x46},
    {1136, 0x47},
    {1152, 0x48},
    {1168, 0x49},
    {1184, 0x4a},
    {1200, 0x4b},
    {1216, 0x4c},
    {1232, 0x4d},
    {1248, 0x4e},
    {1264, 0x4f},
    {1280, 0x50},
    {1296, 0x51},
    {1312, 0x52},
    {1328, 0x53},
    {1344, 0x54},
    {1360, 0x55},
    {1376, 0x56},
    {1392, 0x57},
    {1408, 0x58},
    {1424, 0x59},
    {1440, 0x5a},
    {1456, 0x5b},
    {1472, 0x5c},
    {1488, 0x5d},
    {1504, 0x5e},
    {1520, 0x5f},
    {1536, 0x60},
    {1552, 0x61},
    {1568, 0x62},
    {1584, 0x63},
    {1600, 0x64},
    {1616, 0x65},
    {1632, 0x66},
    {1648, 0x67},
    {1664, 0x68},
    {1680, 0x69},
    {1696, 0x6a},
    {1712, 0x6b},
    {1728, 0x6c},
    {1744, 0x6d},
    {1760, 0x6e},
    {1776, 0x6f},
    {1792, 0x70},
    {1808, 0x71},
    {1824, 0x72},
    {1840, 0x73},
    {1856, 0x74},
    {1872, 0x75},
    {1888, 0x76},
    {1904, 0x77},
    {1920, 0x78},
    {1936, 0x79},
    {1952, 0x7a},
    {1968, 0x7b},
    {1984, 0x7c},
    {2000, 0x7d},
    {2016, 0x7e},
    {2032, 0x7f},
};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY mirror_table[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]

};

I2C_ARRAY mirr_flip_table[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    {0x3221, 0x06}, // mirror[2:1], flip[6:5]
    {0x3221, 0x60}, // mirror[2:1], flip[6:5]
    {0x3221, 0x66}, // mirror[2:1], flip[6:5]
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static I2C_ARRAY gain_reg[] = {
    {0x3e06,0x00},
    {0x3e07, 0x00},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x10}, //low bit, 0x10 - 0x3e0, step 1/16
};

static I2C_ARRAY gain_reg_HDR_DOL_SEF[] = {
    {0x3e10,0x00},
    {0x3e11, 0x00},
    {0x3e12, 0x00|0x03},
    {0x3e13, 0x10}, //low bit, 0x10 - 0x3e0, step 1/16
    };

I2C_ARRAY expo_reg[] = {  // max expo line vts*2-6!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x30}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
};

I2C_ARRAY expo_reg_HDR_DOL_SEF[] = {
    {0x3e04, 0x21}, // expo[7:0]
    {0x3e05, 0x00}, // expo[7:4]
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x06},
    {0x320f, 0x1a},
};

I2C_ARRAY vts_reg_HDR[] = {
    {0x320e, 0x0c},
    {0x320f, 0x34},
    {0x3235, 0x18},
    {0x3236, 0x67},
};

I2C_ARRAY expo_maxshr_reg[] = {
    {0x3e23, 0x00},
    {0x3e24, 0x87}
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
#define SENSOR_NAME sc4238
*/
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

int cus_camsensor_release_handle(ms_cus_sensor *handle);

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
    int res = 0;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    //sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    //sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    //sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

/*     if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    } */

    //ISP_config_io(handle);
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    res = sensor_if->Reset(idx, handle->reset_POLARITY);
    if(res>0)
    {
        SENSOR_USLEEP(1000);
        SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    }

    res = sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    if(res>0)
    {
        SENSOR_USLEEP(1000);
        SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    }

    // power -> high, reset -> high
    res = sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    if(res>0)
    {
        SENSOR_USLEEP(1000);
        SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    }

    res = sensor_if->Reset(idx, !handle->reset_POLARITY);
    if(res>0)
    {
        SENSOR_USLEEP(1000);
    }

    res = sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    if(res>0)
    {
        SENSOR_USLEEP(5000);
    }

    return SUCCESS;
}
static int pCus_post_init(ms_cus_sensor *handle, u32 idx)
{
	ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);
	
	sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
	return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_USLEEP(1000);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
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

    for(n=0;n<table_length;++n)
    {
      id_from_sensor[n].reg = Sensor_id_table[n].reg;
      id_from_sensor[n].data = 0;
    }

    *id =0;
    if(table_length>8) table_length=8;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(n=0;n<4;++n) //retry , until I2C success
    {
      if(n>2) return FAIL;

      if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
          break;
      else
          continue;
    }

    //convert sensor id to u32 format
    for(i=0;i<table_length;++i)
    {
      if( id_from_sensor[i].data != Sensor_id_table[i].data )
        return FAIL;
      //*id = id_from_sensor[i].data;
      *id = ((*id)+ id_from_sensor[i].data)<<8;
    }

    *id >>= 8;
    SENSOR_DMSG("[%s]sc4238 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int sc4238_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);

static int pCus_init_4M30fps(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4M30fps);i++)
    {
        //if(Sensor_init_table_4M30fps[i].reg==0x0100 && 0x01 == Sensor_init_table_4M30fps[i].data)
        //{
        //    sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
        //    sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
        //    sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
        //    SENSOR_MSLEEP(50);
        //}

        if(Sensor_init_table_4M30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4M30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4M30fps[i].reg,Sensor_init_table_4M30fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
   }

    pCus_SetOrien(handle, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_4M30fps_2lane(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4M30fps_2lane);i++)
    {
        //if(Sensor_init_table_4M30fps_2lane[i].reg==0x0100 && 0x01 == Sensor_init_table_4M30fps_2lane[i].data)
        //{
        //    sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
        //    sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, 2/*handle->interface_attr.attr_mipi.mipi_lane_num*/, 1);
        //    sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
        //    SENSOR_MSLEEP(50);
        //}

        if(Sensor_init_table_4M30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4M30fps_2lane[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4M30fps_2lane[i].reg,Sensor_init_table_4M30fps_2lane[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
   }

    pCus_SetOrien(handle, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_HDR_DOL_4lane(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    int i,cnt=0;
/*     sensor_if->SetCSI_Clk(0, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(0, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(0, 0, 0x1C00, 0);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(0, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    } */

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane);i++)
    {
        //if(Sensor_init_table_HDR_DOL_4lane[i].reg==0x0100 && 0x01 == Sensor_init_table_HDR_DOL_4lane[i].data)
        //{
        //    sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
        //    sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
        //    sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
        //    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        //    sensor_if->SetCSI_hdr_mode(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
        //    }
        //    SENSOR_MSLEEP(50);
        //}

        if(Sensor_init_table_HDR_DOL_4lane[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_4lane[i].data);
        }
        else
        {
            cnt = 0;
                while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane[i].reg,Sensor_init_table_HDR_DOL_4lane[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
        }
    }

     pCus_SetOrien(handle, params->cur_orien);
    // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
     vts_reg_HDR[0].data = (params->expo.vts >> 8) & 0x00ff;
     vts_reg_HDR[1].data = (params->expo.vts >> 0) & 0x00ff;

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
    u32 num_res = handle->video_res_supported.num_res;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0: //"2560x1440@30fps"
            handle->video_res_supported.ulcur_res = 0;
            if(lane_num == 4)
            {
                handle->pCus_sensor_init = pCus_init_4M30fps;
                vts_30fps  = 1562;//1500;
                params->expo.fps = 30;
                Preview_line_period  = 21340;
            }else if (lane_num == 2)
            {
                handle->pCus_sensor_init = pCus_init_4M30fps_2lane;
                vts_30fps  = 1500;//1500;
                params->expo.fps = 30;
                Preview_line_period  = 22222;
            }else
            {
                handle->pCus_sensor_init = pCus_init_4M30fps;
                vts_30fps  = 1562;//1500;
                params->expo.fps = 30;
                Preview_line_period  = 21340;
            }
            params->expo.vts = vts_30fps;
            break;
        case 1: //"2688x1520@30fps"
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_4M30fps;
            vts_30fps  = 1562;//1500;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 21340;
            break;
        default:
            break;
    }

    return SUCCESS;
}
static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    sc4238_params *params = (sc4238_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
            handle->pCus_sensor_init = pCus_init_HDR_DOL_4lane;
            }
            params->expo.vts = vts_30fps_HDR_DOL;
            params->expo.fps = 30;
            params->expo.max_short_exp=182;
            break;
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
            handle->pCus_sensor_init = pCus_init_HDR_DOL_4lane;
            }
            params->expo.vts = vts_30fps_HDR_DOL;
            params->expo.fps = 30;
            params->expo.max_short_exp=182;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sen_data = mirror_table[0].data;
    SENSOR_DMSG("mirror:%x\r\n", sen_data);
    switch(sen_data) {
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
   sc4238_params *params = (sc4238_params *)handle->private_data;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;
   // pr_info("Connect %s table_length %d\n",__FUNCTION__, table_length);
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    switch(orit)
    {
        case CUS_ORIT_M0F0:
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                mirror_table[0].data = 0;
                params->cur_orien = CUS_ORIT_M0F0;
            }
            //SensorReg_Write(0x3213,0x04);    //{0x3213, 0x04}, // crop for bayer
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
#if sample
                SensorReg_Write(0x3812,0x00);
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                SensorReg_Write(0x3005,0xf1);
                SensorReg_Write(0x3005,0xf0);
                SensorReg_Write(0x3812,0x30);
#endif
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                mirror_table[0].data = 0x06;
                params->cur_orien = CUS_ORIT_M1F0;
            }
            //SensorReg_Write(0x3213,0x04);    //{0x3213, 0x04}, // crop for bayer
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                mirror_table[0].data = 0x60;
                params->cur_orien = CUS_ORIT_M0F1;
            }
            //SensorReg_Write(0x3213,0x04);    //{0x3213, 0x04}, // crop for bayer
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
#if sample
                SensorReg_Write(0x3812,0x00);
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                SensorReg_Write(0x3005,0xf1);
                SensorReg_Write(0x3005,0xf0);
                SensorReg_Write(0x3812,0x30);
#endif
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                mirror_table[0].data = 0x66;
                params->cur_orien = CUS_ORIT_M1F1;
            }
            //SensorReg_Write(0x3213,0x04);    //{0x3213, 0x04}, // crop for bayer
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M0F0;
            }
            //SensorReg_Write(0x3213,0x04);    //{0x3213, 0x04}, // crop for bayer
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
  //  pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);
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

    if(params->expo.line > 2* (params->expo.vts) -10){
        vts = (params->expo.line + 11)/2;
    }else{
        vts = params->expo.vts;
    }
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;

}

static int pCus_GetFPS_HDR_DOL_SEF(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg_HDR[0].data << 8) | (vts_reg_HDR[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF(ms_cus_sensor *handle, u32 fps)
{
    //u32 max_short_exp=0;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    //pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*max_fps)/fps;
        params->expo.max_short_exp = (((params->expo.vts)/17 - 1)>>1) << 1;

        vts_reg_HDR[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg_HDR[1].data = (params->expo.vts >> 0) & 0x00ff;

        vts_reg_HDR[2].data = ((2*(params->expo.vts) - 1) >> 8) & 0x00ff;
        vts_reg_HDR[3].data = ((2*(params->expo.vts) - 1) >> 0) & 0x00ff;

        expo_maxshr_reg[0].data = (params->expo.max_short_exp >> 8) & 0x00ff;
        expo_maxshr_reg[1].data = (params->expo.max_short_exp >> 0) & 0x00ff;
        params->reg_dirty = true;
        //pr_info("[%s]  vts_reg : %x , %x\n\n", __FUNCTION__,vts_reg_HDR[0].data,vts_reg_HDR[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*(max_fps*1000))/fps;
        params->expo.max_short_exp = (((params->expo.vts)/17 - 1)>>1) << 1;

        vts_reg_HDR[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg_HDR[1].data = (params->expo.vts >> 0) & 0x00ff;

        vts_reg_HDR[2].data = ((2*(params->expo.vts) - 1) >> 8) & 0x00ff;
        vts_reg_HDR[3].data = ((2*(params->expo.vts) - 1) >> 0) & 0x00ff;

        expo_maxshr_reg[0].data = (params->expo.max_short_exp >> 8) & 0x00ff;
        expo_maxshr_reg[1].data = (params->expo.max_short_exp >> 0) & 0x00ff;
        params->reg_dirty = true;
        //pr_info("[%s]  vts_reg : %x , %x\n\n", __FUNCTION__,vts_reg_HDR[0].data,vts_reg_HDR[1].data);
        return SUCCESS;
    }else{
        //pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

}
static int pCus_SetFPS_hdr_dol_lef(ms_cus_sensor *handle, u32 fps)
{
    //u32 max_short_exp=0;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
//  pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*max_fps)/fps;
        params->expo.max_short_exp = (((params->expo.vts)/17 - 1)>>1) << 1;
        vts_reg_HDR[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg_HDR[1].data = (params->expo.vts >> 0) & 0x00ff;
        vts_reg_HDR[2].data = ((2*(params->expo.vts) - 1) >> 8) & 0x00ff;
        vts_reg_HDR[3].data = ((2*(params->expo.vts) - 1) >> 0) & 0x00ff;
        expo_maxshr_reg[0].data = (params->expo.max_short_exp >> 8) & 0x00ff;
        expo_maxshr_reg[1].data = (params->expo.max_short_exp >> 0) & 0x00ff;
        params->reg_dirty = true;
        //pr_info("[%s]  vts_reg : %x , %x\n\n", __FUNCTION__,vts_reg_HDR[0].data,vts_reg_HDR[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR_DOL*(max_fps*1000))/fps;
        params->expo.max_short_exp = (((params->expo.vts)/17 - 1)>>1) << 1;
        vts_reg_HDR[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg_HDR[1].data = (params->expo.vts >> 0) & 0x00ff;
        vts_reg_HDR[2].data = ((2*(params->expo.vts) - 1) >> 8) & 0x00ff;
        vts_reg_HDR[3].data = ((2*(params->expo.vts) - 1) >> 0) & 0x00ff;
        expo_maxshr_reg[0].data = (params->expo.max_short_exp >> 8) & 0x00ff;
        expo_maxshr_reg[1].data = (params->expo.max_short_exp >> 0) & 0x00ff;
        params->reg_dirty = true;
        //pr_info("[%s]  vts_reg : %x , %x\n\n", __FUNCTION__,vts_reg_HDR[0].data,vts_reg_HDR[1].data);
        return SUCCESS;
    }else{
        //pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
}
#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap) {
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else     return FAIL;
    return SUCCESS;
}
#endif
///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status){
   sc4238_params *params = (sc4238_params *)handle->private_data;
   switch(status)
   {
       case CUS_FRAME_INACTIVE:
       break;
       case CUS_FRAME_ACTIVE:
       if(params->reg_dirty)
       {
          SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
          SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
          SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
          params->reg_dirty = false;
       }
       break;
       default :
       break;
   }
   return SUCCESS;
}
static int pCus_AEStatusNotifyHDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
        if(params->reg_dirty)
        {
           SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)vts_reg_HDR, sizeof(vts_reg_HDR)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)expo_maxshr_reg, sizeof(expo_maxshr_reg)/sizeof(I2C_ARRAY));
           params->reg_dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;

}

static int pCus_AEStatusNotifyHDR_DOL_SEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
   sc4238_params *params = (sc4238_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             break;
        case CUS_FRAME_ACTIVE:
       if(params->reg_dirty)
        {
           SensorRegArrayW((I2C_ARRAY*)expo_reg_HDR_DOL_SEF, sizeof(expo_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)gain_reg_HDR_DOL_SEF, sizeof(gain_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)vts_reg_HDR, sizeof(vts_reg_HDR)/sizeof(I2C_ARRAY));
           SensorRegArrayW((I2C_ARRAY*)expo_maxshr_reg, sizeof(expo_maxshr_reg)/sizeof(I2C_ARRAY));
           params->reg_dirty = false;
        }
        break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 *us) {
  int rc=0;
  u32 lines = 0;
  lines |= (u32)(expo_reg_HDR_DOL_SEF[0].data&0x0f)<<8;
  lines |= (u32)(expo_reg_HDR_DOL_SEF[1].data&0xff)<<0;
  lines >>= 4;
  *us = (lines*Preview_line_period_HDR_DOL)/1000/2; //return us

  SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
  return rc;
}

static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    //u16 data0 = 0,data1 = 0;
    u32 half_lines = 0,dou_lines =0;//vts = 0;

    sc4238_params *params = (sc4238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

/*  SensorReg_Read(0x3e23,&data0);
    SensorReg_Read(0x3e24,&data1);
    params->expo.max_short_exp = (data0 << 8) | (data1 << 0); */

    dou_lines = (1000*us)/(Preview_line_period_HDR_DOL*2); // Preview_line_period in ns
    half_lines = 4*dou_lines;
    if(half_lines<5) half_lines=5;
    if (half_lines >  2 * ((params->expo.vts)-(params->expo.max_short_exp)-7)) {
        half_lines = 2 * ((params->expo.vts)-(params->expo.max_short_exp)-7);
    }
    else
        //vts=params->expo.vts;

//  SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg[0].data = (half_lines>>16) & 0x0f;
    expo_reg[1].data =  (half_lines>>8) & 0xff;
    expo_reg[2].data = (half_lines>>0) & 0xf0;
 //   pr_info("[%s]  expo_reg : %x ,%x , %x\n\n", __FUNCTION__,expo_reg[0].data,expo_reg[1].data,expo_reg[2].data);
    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (expo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
     }
    return SUCCESS;
}

static int pCus_GetAEGain_HDR_DOL_LEF(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;

    return rc;
}

static int pCus_GetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 *us) {
  int rc=0;
  u32 lines = 0;
  lines |= (u32)(expo_reg_HDR_DOL_SEF[0].data&0x0f)<<8;
  lines |= (u32)(expo_reg_HDR_DOL_SEF[1].data&0xff)<<0;
  lines >>= 4;
  *us = (lines*Preview_line_period_HDR_DOL)/1000/2; //return us

  SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
  return rc;
}

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0,dou_lines = 0;//vts = 0;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {
        {0x3e04, 0x21}, // expo[7:0]
        {0x3e05, 0x00}, // expo[7:4]
    };
    memcpy(expo_reg_temp, expo_reg_HDR_DOL_SEF, sizeof(expo_reg_HDR_DOL_SEF));

    dou_lines = (1000*us)/(Preview_line_period_HDR_DOL*2); // Preview_line_period in ns
    half_lines = 4*dou_lines;
    if(half_lines<5) half_lines=5;
    if (half_lines >  2 * (params->expo.max_short_exp-6)) {
        half_lines = 2 * (params->expo.max_short_exp-6);
    }
    else
     //vts=params->expo.vts;
//  SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg_HDR_DOL_SEF[0].data =  (half_lines>>8) & 0xff;
    expo_reg_HDR_DOL_SEF[1].data = (half_lines>>0) & 0xf0;
 //   pr_info("[%s]  expo_reg_HDR_DOL_SEF : %x , %x\n\n", __FUNCTION__,expo_reg_HDR_DOL_SEF[0].data,expo_reg_HDR_DOL_SEF[1].data);
    for (i = 0; i < sizeof(expo_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY); i++)
    {
      if (expo_reg_HDR_DOL_SEF[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
     }
    return SUCCESS;

}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
  int rc=0;
  u32 lines = 0;
  lines |= (u32)(expo_reg[0].data&0x0f)<<16;
  lines |= (u32)(expo_reg[1].data&0xff)<<8;
  lines |= (u32)(expo_reg[2].data&0xf0)<<0;
  lines >>= 4;
  *us = (lines*Preview_line_period)/1000/2; //return us

  SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
  return rc;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    int i;
    u32 half_lines = 0,vts = 0;
    sc4238_params *params = (sc4238_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-10) {
        vts = (half_lines+11)/2;
    }
    else
        vts=params->expo.vts;
    params->expo.line = half_lines;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;

    expo_reg[0].data = (half_lines>>16) & 0x0f;
    expo_reg[1].data =  (half_lines>>8) & 0xff;
    expo_reg[2].data = (half_lines>>0) & 0xf0;
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;

    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (expo_reg[i].data != expo_reg_temp[i].data)
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
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u64 Fine_again = 1024,Fine_dgain = 10000;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x00},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x10},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

    //printk("[%s]  gain : %d \n", __FUNCTION__,gain);
    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16256)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32512)
    {
        Dgain = 1;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  65024)
    {
        Dgain = 2;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 127 * 1024)
    {
        Dgain = 4;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < 254 * 1024)
    {
        Dgain = 8;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16256)
    {
        Fine_again = gain*10000 / (Dgain * Coarse_gain * Fine_dgain);
        for(i = 1; i< sizeof(fine_again)/sizeof(FINE_GAIN);i++)
        {
            if(Fine_again >= fine_again[i-1].gain && Fine_again <= fine_again[i].gain)
            {
                Fine_again_reg = (fine_again[i].gain - Fine_again) > (Fine_again - fine_again[i-1].gain) ? fine_again[i-1].fine_gain_reg:fine_again[i].fine_gain_reg;
                break;
            }
            else if(Fine_again > fine_again[(sizeof(fine_again)/sizeof(FINE_GAIN)) - 1].gain)
            {
                Fine_again_reg = 0x7f;
                break;
            }
        }
    }
    else
    {
        //Fine_dgain =gain / (Dgain * Coarse_gain * Fine_again);
        Fine_dgain_reg = abs(gain * 128 / (Dgain * Coarse_gain * Fine_again));
    }

    //printk("[%s]  gain : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Fine_dgain_reg,Dgain_reg,Coarse_gain_reg);
    SENSOR_DMSG("[%s]  gain : %f ,%f ,%d , %d\n\n", __FUNCTION__,Fine_again,Fine_dgain,Dgain,Coarse_gain);
    SENSOR_DMSG("[%s]  gain : %x ,%x ,%x , %x\n\n", __FUNCTION__,Dgain_reg,Fine_dgain_reg,Fine_again_reg,Coarse_gain_reg);
    gain_reg[3].data = Fine_again_reg;
    gain_reg[2].data = Coarse_gain_reg;
    gain_reg[1].data = Fine_dgain_reg;
    gain_reg[0].data = Dgain_reg & 0xF;

    for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (gain_reg[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}

static int pCus_SetAEGainHDR_DOL_SEF(ms_cus_sensor *handle, u32 gain) {
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u64 Fine_again = 1024,Fine_dgain = 10000000;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e10, 0x00},
        {0x3e11, 0x00},
        {0x3e12, 0x00|0x03},
        {0x3e13, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg_HDR_DOL_SEF, sizeof(gain_reg_HDR_DOL_SEF));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16256)
    {
        Dgain = 1;      Fine_dgain = 10000;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32512)
    {
        Dgain = 1;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  65024)
    {
        Dgain = 2;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 127 * 1024)
    {
        Dgain = 4;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < 254 * 1024)
    {
        Dgain = 8;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_again = 2032;    Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x7f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16256)
    {
        Fine_again = gain*10000 / (Dgain * Coarse_gain * Fine_dgain);
        for(i = 1; i< sizeof(fine_again)/sizeof(FINE_GAIN);i++)
        {
            if(Fine_again >= fine_again[i-1].gain && Fine_again <= fine_again[i].gain)
            {
                Fine_again_reg = (fine_again[i].gain - Fine_again) > (Fine_again - fine_again[i-1].gain) ? fine_again[i-1].fine_gain_reg:fine_again[i].fine_gain_reg;
                break;
            }
            else if(Fine_again > fine_again[(sizeof(fine_again)/sizeof(FINE_GAIN)) - 1].gain)
            {
                Fine_again_reg = 0x7f;
                break;
            }
        }
    }
    else
    {
        //Fine_dgain =gain*10000 / (Dgain * Coarse_gain * Fine_again);
        Fine_dgain_reg = abs(gain * 128 / (Dgain * Coarse_gain * Fine_again));
    }

    SENSOR_DMSG("[%s]  gain : %f ,%f ,%d , %d\n\n", __FUNCTION__,Fine_again,Fine_dgain,Dgain,Coarse_gain);
    SENSOR_DMSG("[%s]  gain : %x ,%x ,%x , %x\n\n", __FUNCTION__,Dgain_reg,Fine_dgain_reg,Fine_again_reg,Coarse_gain_reg);
    gain_reg_HDR_DOL_SEF[3].data = Fine_again_reg;
    gain_reg_HDR_DOL_SEF[2].data = Coarse_gain_reg;
    gain_reg_HDR_DOL_SEF[1].data = Fine_dgain_reg;
    gain_reg_HDR_DOL_SEF[0].data = Dgain_reg & 0xF;

    for (i = 0; i < sizeof(gain_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY); i++)
    {
      if (gain_reg_HDR_DOL_SEF[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}


static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 30;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1024;
  *max = SENSOR_MAXGAIN*1024;
  return SUCCESS;
}

static int sc4238_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 3;
    info->step = Preview_line_period;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period_HDR_DOL * 3);
    info->step = Preview_line_period_HDR_DOL*2;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = Preview_line_period_HDR_DOL * 270;
    info->min = (Preview_line_period_HDR_DOL * 3);
    info->step = Preview_line_period_HDR_DOL*2;
    return SUCCESS;
}
static int pCus_poweron_hdr_dol_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int pCus_poweroff_hdr_dol_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}
static int pCus_GetSensorID_hdr_dol_lef(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}
static int pCus_init_hdr_dol_lef(ms_cus_sensor *handle)
{
    return SUCCESS;
}
#if 0
static int pCus_GetVideoRes_hdr_dol_lef( ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res )
{
    *res = &handle->video_res_supported.res[res_idx];
    return SUCCESS;
}

static int pCus_SetVideoRes_hdr_dol_lef( ms_cus_sensor *handle, u32 res )
{
    handle->video_res_supported.ulcur_res = 0; //TBD
    return SUCCESS;
}
#endif

static int pCus_GetFPS_hdr_dol_lef(ms_cus_sensor *handle)
{
    sc4238_params *params = (sc4238_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg_HDR[0].data << 8) | (vts_reg_HDR[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_setCaliData_gain_linearity_hdr_dol_lef(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}
static int pCus_SetAEGain_cal_hdr_dol_lef(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}
static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc4238_params *params;
#if defined(CAM_OS_RTK)
    lane_num = SENSOR_MIPI_LANE_NUM;
#endif
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(sc4238_params));
    params = (sc4238_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    SENSOR_DMSG(handle->model_id,"sc4238_MIPI");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    //handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_num;//SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
if (lane_num == 4)
{
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = 2560;
    handle->video_res_supported.res[0].height = 1440;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 64;
    handle->video_res_supported.res[0].crop_start_y= 40;
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1440@30fps");

     handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 2688;
    handle->video_res_supported.res[1].nOutputHeight= 1520;
    sprintf(handle->video_res_supported.res[1].strResDesc, "2688x1520@30fps");
}else if (lane_num == 2)
{
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = 2560;
    handle->video_res_supported.res[0].height = 1440;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1440@30fps");
}else
{
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = 2560;
    handle->video_res_supported.res[0].height = 1440;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 64;
    handle->video_res_supported.res[0].crop_start_y= 40;
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1440@30fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 2688;
    handle->video_res_supported.res[1].nOutputHeight= 1520;
    sprintf(handle->video_res_supported.res[1].strResDesc, "2688x1520@30fps");
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
    if(lane_num == 4)
        handle->pCus_sensor_init        = pCus_init_4M30fps    ;
    else if(lane_num == 2)
        handle->pCus_sensor_init        = pCus_init_4M30fps_2lane    ;
    else
        handle->pCus_sensor_init        = pCus_init_4M30fps    ;

    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;
    handle->pCus_sensor_post_init   = pCus_post_init;
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
    handle->pCus_sensor_SetPatternMode = sc4238_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = sc4238_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line= 1500;
    params->reg_dirty = false;
    params->reg_mf = false;

    return SUCCESS;
}
int cus_camsensor_init_handle_hdr_dol_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc4238_params *params = NULL;

    cus_camsensor_init_handle_linear(drv_handle);
    params = (sc4238_params *)handle->private_data;

    sprintf(handle->model_id,"sc4238_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;
    handle->RGBIR_id    = SENSOR_RGBIRID;

    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_DOL;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = 2560;
    handle->video_res_supported.res[0].height = 1440;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 64;
    handle->video_res_supported.res[0].crop_start_y= 40;
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560X1440@30fps_HDR");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 2688;
    handle->video_res_supported.res[1].nOutputHeight= 1520;
    sprintf(handle->video_res_supported.res[1].strResDesc, "2688X1520@30fps_HDR");


    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

    handle->pCus_sensor_init        = pCus_init_HDR_DOL_4lane;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_DOL_SEF; //TBD
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_SEF; //TBD

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGainHDR_DOL_SEF;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_sef;
    params->expo.vts = vts_30fps_HDR_DOL;
    params->expo.fps = 30;
    params->expo.max_short_exp=182;
    handle->data_prec   = SENSOR_DATAPREC_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc4238_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(sc4238_params));
    params = (sc4238_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"SC4238_MIPI_HDR_LEF");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC_HDR_DOL;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_DOL;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num =  0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = 2560;
    handle->video_res_supported.res[0].height = 1440;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 64;
    handle->video_res_supported.res[0].crop_start_y= 40;
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560X1440@30fps_HDR");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 2688;
    handle->video_res_supported.res[1].nOutputHeight= 1520;
    sprintf(handle->video_res_supported.res[1].strResDesc, "2688X1520@30fps_HDR");


    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

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
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_hdr_dol_lef;
    //handle->pCus_sensor_powerupseq  = pCus_powerupseq   ;
    handle->pCus_sensor_poweron     = pCus_poweron_hdr_dol_lef;
    handle->pCus_sensor_poweroff    = pCus_poweroff_hdr_dol_lef;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID_hdr_dol_lef;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_hdr_dol_lef;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_hdr_dol_lef;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap_hdr_dol_lef;
    //handle->pCus_sensor_SetPatternMode = pCus_SetPatternMode_hdr_dol_lef;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_LEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecsHDR_DOL_LEF;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_LEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_HDR_DOL_LEF;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    //handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_hdr_dol_lef;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_dol_lef;
    handle->pCus_sensor_setCaliData_gain_linearity= pCus_setCaliData_gain_linearity_hdr_dol_lef;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_lef;

    params->expo.vts = vts_30fps_HDR_DOL;
    params->expo.fps = 30;
    params->reg_dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  SC4238_HDR,
                            cus_camsensor_init_handle_linear,
                            cus_camsensor_init_handle_hdr_dol_sef,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            sc4238_params
                         );


