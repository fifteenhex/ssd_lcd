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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(OS08A10_HDR);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
#define SENSOR_MIPI_LANE_NUM_HDR (4)
#define SENSOR_MIPI_HDR_MODE (2) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

#define R_GAIN_REG 1
#define G_GAIN_REG 2
#define B_GAIN_REG 3

//#undef SENSOR_DBG
#define SENSOR_DBG 0

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
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_HDR  CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR  CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,

#define SENSOR_MAX_GAIN     246*1024//(15.5*15.9)                  // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN     1024//(15.5*15.9)                  // max sensor again, a-gain * conversion-gain*d-gain

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 16801//17814                           // MCLK=21.6 HTS/PCLK=3080 pixels/97.2MHZ=31.687us                              // 3126 for 25fps
//#define Preview_line_period_DCG 28810//17814                     // MCLK=21.6 HTS/PCLK=3080 pixels/97.2MHZ=31.687us                              // 3126 for 25fps
//#define vts_30fps           1984                               // VTS for 20fps
//#define vts_30fps_DCG       2314                     // VTS for 20fps

u32 vts_30fps;
u32 vts_30fps_DCG;
u32 Preview_line_period;
u32 Preview_line_period_DCG;

#define Preview_WIDTH       3840//2688                    //resolution Width when preview
#define Preview_HEIGHT      2160//1520                    //resolution Height when preview
#define Preview_MAX_FPS     20                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS

#define SENSOR_I2C_ADDR    0x6c                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif
static int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int OS08A10_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int OS08A10_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int OS08A10_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
static int OS08A10_SetAEUSecs_HDR_lef(ms_cus_sensor *handle, u32 us);
static int OS08A10_SetAEUSecs_HDR_sef(ms_cus_sensor *handle, u32 us);

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
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;
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
        u32 fps;
        u32 preview_fps;
        u32 lines;
        u32 max_short;
    } expo;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool mirror_dirty;
    bool dirty;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tMirror_reg[2];
    I2C_ARRAY tMirror_reg_DCG[2];
    I2C_ARRAY tGain_vc1_reg[4];
    I2C_ARRAY tExpo_vc0_reg[2];
    I2C_ARRAY tExpo_vc1_reg[2];
    CUS_CAMSENSOR_ORIT cur_orien;
} os08A10_params;

// set sensor ID address and data,
const static I2C_ARRAY Sensor_id_table[] =
{
    {0x300a, 0x53},      // {address of ID, ID },
    {0x300b, 0x08},      // {address of ID, ID },
    //{0x300c, 0x41},      // {address of ID, ID },
    // max 8 sets in this table
};

/*
handle->video_res_supported.num_res = res+1;
handle->video_res_supported.res[res].width         = imx415_mipi_linear[res].senif.preview_w;
handle->video_res_supported.res[res].height        = imx415_mipi_linear[res].senif.preview_h;
handle->video_res_supported.res[res].max_fps       = imx415_mipi_linear[res].senout.max_fps;
handle->video_res_supported.res[res].min_fps       = imx415_mipi_linear[res].senout.min_fps;
handle->video_res_supported.res[res].crop_start_x  = imx415_mipi_linear[res].senif.crop_start_X;
handle->video_res_supported.res[res].crop_start_y  = imx415_mipi_linear[res].senif.crop_start_y;
handle->video_res_supported.res[res].nOutputWidth  = imx415_mipi_linear[res].senout.width;
handle->video_res_supported.res[res].nOutputHeight = imx415_mipi_linear[res].senout.height;
sprintf(handle->video_res_supported.res[res].strResDesc, imx415_mipi_linear[res].senstr.strResDesc);
*/
////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {     // LINEAR
    // Modify it based on number of support resolution
    enum {LINEAR_RES_1 = 0, LINEAR_RES_2, LINEAR_RES_3, LINEAR_RES_4, LINEAR_RES_5, LINEAR_RES_END}mode;
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
}os08a10_mipi_linear[] = {
    {LINEAR_RES_1, {3840, 2160, 3, 30}, {0, 0, 3840, 2160}, {"3840x2160@30fps"}},
    {LINEAR_RES_2, {2592, 1944, 3, 30}, {0, 0, 2592, 1944}, {"2592x1944@30fps"}}, // Modify it
    {LINEAR_RES_3, {2944, 1656, 3, 30}, {0, 0, 2944, 1656}, {"2944x1656@30fps"}}, // Modify it
    {LINEAR_RES_4, {1920, 1080, 3, 60}, {0, 0, 1920, 1080}, {"1920x1080@60fps"}}, // Modify it
    {LINEAR_RES_5, {3840, 2160, 3, 20}, {0, 0, 3840, 2160}, {"3840x2160@20fps"}}, // Modify it
};

static struct {     // HDR
    // Modify it based on number of support resolution
    enum {HDR_RES_1 = 0, HDR_RES_2, HDR_RES_3, HDR_RES_4 , HDR_RES_5, HDR_RES_END}mode;
    // Sensor Output Image info
    struct _hsenout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _hsensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _hsenstr{
        const char* strResDesc;
    }senstr;
}os08a10_mipi_hdr[] = {
   {HDR_RES_1, {3840, 2160, 3, 30}, {0, 0, 3840, 2160}, {"3840x2160@30fps_HDR"}}, // Modify it
   {HDR_RES_2, {2592, 1944, 3, 30}, {0, 0, 2592, 1944}, {"2592x1944@30fps_HDR"}}, // Modify it
   {HDR_RES_3, {2944, 1656, 3, 30}, {0, 0, 2944, 1656}, {"2944x1656@30fps_HDR"}}, // Modify it
   {HDR_RES_4, {1920, 1080, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps_HDR"}}, // Modify it
   {HDR_RES_5, {3840, 2160, 3, 20}, {0, 0, 3840, 2160}, {"3840x2160@20fps_HDR"}}, // Modify it
};

const static I2C_ARRAY Sensor_init_table_DCG_8m20[] =
{
//Res 3840X2160 4lane MIPI960Mbps HDRVC10 20fps MCLK24M
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x3c},
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x09},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xb5},
    {0x3610, 0x39},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0x42},
    {0x3661, 0x07},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x39},
    {0x370a, 0x00},
    {0x370b, 0x98},
    {0x3709, 0x49},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},
    {0x3741, 0x04},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3762, 0x11},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0f},
    {0x3809, 0x00},
    {0x380a, 0x08},
    {0x380b, 0x70},
    {0x380c, 0x06},
    {0x380d, 0x12},
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x08},
    {0x3820, 0x00},
    {0x3821, 0x24},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x01},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x98},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x10},
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x07},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x08},
    {0x3502, 0xe1},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
	{0x0100, 0x01},
};

const static I2C_ARRAY Sensor_init_table_DCG_8m30[] =
{
//Res 3840X2160 4lane MIPI1440Mbps HDRVC10 30fps MCLK24M
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x5a},//4c
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
	//{0x0325, 0x48},//0702 add
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x09},
    {0x3501, 0x04},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xb5},
    {0x3610, 0x39},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0x42},
    {0x3661, 0x07},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x39},
    {0x370a, 0x00},
    {0x370b, 0x98},
    {0x3709, 0x49},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},
    {0x3741, 0x04},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3762, 0x11},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0f},
    {0x3809, 0x00},
    {0x380a, 0x08},
    {0x380b, 0x70},
    {0x380c, 0x04},//04
    {0x380d, 0x0c},//0c
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x08},
    {0x3820, 0x00},
    {0x3821, 0x24},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x01},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x98},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x0b},//0d
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x07},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x04},
    {0x3502, 0xe1},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
	{0x0100, 0x01},
};

const static I2C_ARRAY Sensor_init_table_DCG_5m30[] =
{
//Res 2592X1944 4lane MIPI960Mbps HDRVC10 30fps MCLK24M
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x3c},
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x09},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xb5},
    {0x3610, 0x39},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0x42},
    {0x3661, 0x07},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x39},
    {0x370a, 0x00},
    {0x370b, 0x98},
    {0x3709, 0x49},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},
    {0x3741, 0x04},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3762, 0x11},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0a},
    {0x3809, 0x20},
    {0x380a, 0x07},
    {0x380b, 0x98},
    {0x380c, 0x04},//4
    {0x380d, 0x7a},//7a
    {0x380e, 0x08},
    {0x380f, 0x2e},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x08},
    {0x3820, 0x00},
    {0x3821, 0x24},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x01},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x98},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x10},
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x07},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x08},
    {0x3502, 0xe1},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
	{0x0100, 0x01},
};

const static I2C_ARRAY Sensor_init_table_DCG_4p8m30[] =
{
//@@ 2 2001 Res 2944X1656 4lane MIPI960Mbps HDRVC10 30fps MCLK24M
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x3c},
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x09},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xb5},
    {0x3610, 0x39},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0x42},
    {0x3661, 0x07},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x39},
    {0x370a, 0x00},
    {0x370b, 0x98},
    {0x3709, 0x49},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},
    {0x3741, 0x04},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3762, 0x11},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0b},
    {0x3809, 0x80},
    {0x380a, 0x06},
    {0x380b, 0x78},
    {0x380c, 0x05},//05
    {0x380d, 0x30},//30
    {0x380e, 0x07},
    {0x380f, 0x0e},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x08},
    {0x3820, 0x00},
    {0x3821, 0x24},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x01},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x98},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x10},
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x07},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x08},
    {0x3502, 0xe1},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
	{0x0100, 0x01},
};

const static I2C_ARRAY Sensor_init_table_DCG_2m30[] =
{
// Res 1920X1080 BW BINAVE 4lane MIPI0800Mbps HDRVC10 60fps MCLK24M Tline=26.201us
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x32},
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x09},
    {0x3501, 0x04},
    {0x3502, 0x62},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x09},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xb5},
    {0x3610, 0x39},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0x42},
    {0x3661, 0x07},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x39},
    {0x370a, 0x00},
    {0x370b, 0x98},
    {0x3709, 0x49},
    {0x3714, 0x22},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},
    {0x3741, 0x04},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3762, 0x11},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x07},
    {0x3809, 0x80},
    {0x380a, 0x04},
    {0x380b, 0x38},
    {0x380c, 0x08},
    {0x380d, 0x18},
    {0x380e, 0x04},
    {0x380f, 0x86},
    {0x3813, 0x08},
    {0x3814, 0x03},
    {0x3815, 0x01},
    {0x3816, 0x03},
    {0x3817, 0x01},
    {0x381c, 0x08},
    {0x3820, 0x01},
    {0x3821, 0x25},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x01},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x05},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x98},
    {0x4502, 0x00},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x98},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x14},
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x07},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x04},
    {0x3502, 0x5d},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
	{0x0100, 0x01},
};

const static I2C_ARRAY Sensor_8m20_init_table[] =
{
    //@@ Res 3840X2160 4lane 768Mbps Linear12 20fps MCLK24M
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x30},//;5a   for 768Mbps
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x51},
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x0a},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0xd3},
    {0x3661, 0x06},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x72},
    {0x370a, 0x01},
    {0x370b, 0x30},
    {0x3709, 0x48},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0f},
    {0x3809, 0x00},
    {0x380a, 0x08},
    {0x380b, 0x70},
    {0x380c, 0x0c},//;10;08  for 20fps
    {0x380d, 0x24},//;30;18  for 20fps
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4600, 0x00},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x90},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x14},//;0b
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x03},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},

    {0x4700, 0x2b},
    {0x4e00, 0x2b},

    {0x3501, 0x09},
    {0x3502, 0x01},

    {0x0100, 0x01},
};

const static I2C_ARRAY Sensor_8m30_init_table[] =
{
//@@ Res 3840X2160 4lane 832Mbps Linear12 30fps MCLK24M
//;version = OS08A10_AM13_EC
//100 99 3840 2160
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x34},//;30;5a   for 832Mbps
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x51},
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x0a},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0xd3},
    {0x3661, 0x06},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x72},
    {0x370a, 0x01},
    {0x370b, 0x30},
    {0x3709, 0x48},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0f},
    {0x3809, 0x00},
    {0x380a, 0x08},
    {0x380b, 0x70},
    {0x380c, 0x08},//;  for 30fps
    {0x380d, 0x18},//;  for 30fps
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4600, 0x00},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x90},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x13},//;14;0b
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x03},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},

    {0x4700, 0x2b},
    {0x4e00, 0x2b},

    {0x3501, 0x09},
    {0x3502, 0x01},

    {0x0100, 0x01},
};

const static I2C_ARRAY Sensor_5m30_init_table[] =
{
    //@@ Res 2592X1944 4lane 832Mbps Linear12 30fps MCLK24M
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x34},//;30;5a   for 832Mbps
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x51},
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x0a},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0xd3},
    {0x3661, 0x06},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x72},
    {0x370a, 0x01},
    {0x370b, 0x30},
    {0x3709, 0x48},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0a},//;0f
    {0x3809, 0x20},//;00
    {0x380a, 0x07},//;08
    {0x380b, 0x98},//;70
    {0x380c, 0x08},//;  for 30fps
    {0x380d, 0x18},//;  for 30fps
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4600, 0x00},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x90},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x13},//;14;0b
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x03},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x3501, 0x09},
    {0x3502, 0x01},
    {0x0100, 0x01}
};

const static I2C_ARRAY Sensor_4p8m30_init_table[] =
{
//@@ Res 2944x1656 4lane 768Mbps Linear12 30fps MCLK24M
//;version = OS08A10_AM13_EC
//100 99 2688 1520
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x30},//;5a   for 768Mbps
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x51},
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x0a},
    {0x3501, 0x08},
    {0x3502, 0xe5},
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x00},
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0xd3},
    {0x3661, 0x06},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x72},
    {0x370a, 0x01},
    {0x370b, 0x30},
    {0x3709, 0x48},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x0b},//;0f
    {0x3809, 0x80},//;00
    {0x380a, 0x06},//;08
    {0x380b, 0x78},//;70
    {0x380c, 0x08},//;  for 30fps
    {0x380d, 0x18},//;  for 30fps
    {0x380e, 0x09},
    {0x380f, 0x0a},
    {0x3813, 0x10},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x18},
    {0x4502, 0x00},
    {0x4600, 0x00},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},
    {0x4809, 0x2b},
    {0x4813, 0x90},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x14},//;0b
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x03},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},

    {0x4700, 0x2b},
    {0x4e00, 0x2b},

    {0x3501, 0x09},
    {0x3502, 0x01},

    {0x0100, 0x01},
};

const static I2C_ARRAY Sensor_2m60_init_table[] =
{
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x32},
    {0x0306, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3011, 0x04},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x301e, 0x98},
    {0x3031, 0xa9},
    {0x3103, 0x92},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x01},//51
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x340c, 0xff},
    {0x340d, 0xff},
    {0x031e, 0x0a},
    {0x3501, 0x04},//08
    {0x3502, 0x62},//e5
    {0x3505, 0x83},
    {0x3508, 0x00},
    {0x3509, 0x80},
    {0x350a, 0x04},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x350d, 0x80},
    {0x350e, 0x04},
    {0x350f, 0x00},
    {0x3600, 0x09},//00
    {0x3603, 0x2c},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x39},//69
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x362d, 0x10},
    {0x3660, 0xd3},
    {0x3661, 0x06},
    {0x3662, 0x00},
    {0x3663, 0x28},
    {0x3664, 0x0d},
    {0x366a, 0x38},
    {0x366b, 0xa0},
    {0x366d, 0x00},
    {0x366e, 0x00},
    {0x3680, 0x00},
    {0x36c0, 0x00},
    {0x3701, 0x02},
    {0x373b, 0x02},
    {0x373c, 0x02},
    {0x3736, 0x02},
    {0x3737, 0x02},
    {0x3705, 0x00},
    {0x3706, 0x72},
    {0x370a, 0x01},
    {0x370b, 0x30},
    {0x3709, 0x48},
    {0x3714, 0x22},//21
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3740, 0x1b},//0708
    {0x3741, 0x04},//0708
    {0x375e, 0x0b},
    {0x3760, 0x10},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x04},
    {0x3798, 0x00},//0708
    {0x3799, 0x00},//0708
    {0x3762, 0x11},//0708
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x0c},
    {0x3804, 0x0e},
    {0x3805, 0xff},
    {0x3806, 0x08},
    {0x3807, 0x6f},
    {0x3808, 0x07},
    {0x3809, 0x80},
    {0x380a, 0x04},
    {0x380b, 0x38},
    {0x380c, 0x04},//08
    {0x380d, 0x0c},//18
    {0x380e, 0x04},
    {0x380f, 0x86},
    {0x3813, 0x08},
    {0x3814, 0x03},
    {0x3815, 0x01},
    {0x3816, 0x03},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x01},
    {0x3821, 0x05},
    {0x3822, 0x54},//0708
    {0x3823, 0x08},
    {0x3826, 0x00},
    {0x3827, 0x08},
    {0x382d, 0x08},
    {0x3832, 0x02},
    {0x3833, 0x00},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0xd7},
    {0x4000, 0xf8},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x400a, 0x01},
    {0x400f, 0xa0},
    {0x4010, 0x12},
    {0x4018, 0x00},
    {0x4008, 0x02},
    {0x4009, 0x05},//0d
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
    {0x4028, 0x2f},
    {0x4052, 0x00},
    {0x4053, 0x80},
    {0x4054, 0x00},
    {0x4055, 0x80},
    {0x4056, 0x00},
    {0x4057, 0x80},
    {0x4058, 0x00},
    {0x4059, 0x80},
    {0x430b, 0xff},
    {0x430c, 0xff},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4501, 0x98},
    {0x4502, 0x00},
    {0x4600, 0x00},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x4800, 0x64},//04
    {0x4809, 0x2b},
    {0x4813, 0x90},
    {0x4817, 0x04},
    {0x4833, 0x18},
    {0x4837, 0x14},
    {0x483b, 0x00},
    {0x484b, 0x03},
    {0x4850, 0x7c},
    {0x4852, 0x06},
    {0x4856, 0x58},
    {0x4857, 0xaa},
    {0x4862, 0x0a},
    {0x4869, 0x18},
    {0x486a, 0xaa},
    {0x486e, 0x03},
    {0x486f, 0x55},
    {0x4875, 0xf0},
    {0x5000, 0x89},
    {0x5001, 0x42},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xc9},
    {0x4d02, 0xbc},
    {0x4d03, 0xc6},
    {0x4d04, 0x4a},
    {0x4d05, 0x25},
    {0x4700, 0x2b},
    {0x4e00, 0x2b},
    {0x4601, 0x2c},
    {0x4602, 0x00},
    {0x380c, 0x05},
    {0x380d, 0x40},
    {0x380e, 0x06},
    {0x380f, 0xfa},
    {0x3501, 0x06},
    {0x3502, 0x25},
    {0x3511, 0x00},
    {0x3512, 0x20},
    {0x3833, 0x01},

    {0x0100, 0x01},


};

I2C_ARRAY TriggerStartTbl[] = {
    {0x0100,0x01},//normal mode
};

const I2C_ARRAY PatternTbl[] = {
    {0x5081,0x00}, //colorbar pattern , bit 7 to enable
};

/////////////////////////////////////////////////////////////////
//       @@@@@@                                                                                    //
//                 @@                                                                                    //
//             @@@                                                                                      //
//       @       @@                                                                                    //
//         @@@@                                                                                        //
//                                                                                                          //
//      Step 3 --  complete camera features                                              //
//                                                                                                         //
//                                                                                                         //
//  camera set EV, MWB, orientation, contrast, sharpness                          //
//   , saturation, and Denoise can work correctly.                                     //
//                                                                                                          //
/////////////////////////////////////////////////////////////////

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const I2C_ARRAY mirror_reg[] = {
    {0x3820, 0x00},//Flip
    {0x3821, 0x04},//mirror
};

const I2C_ARRAY gain_reg[] = {
    {0x3508, 0x00},//long a-gain[13:8]
    {0x3509, 0x70},//long a-gain[7:0]
    {0x350A, 0x00},// d-gain[13:8]
    {0x350B, 0x00},// d-gain[7:0]
};

static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] = {  //compensate  gain gap
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}
};

const I2C_ARRAY expo_reg[] = {
    //{0x3208, 0x00},//Group 0 hold start
    {0x3500, 0x00},//long exp[19,16]
    {0x3501, 0x02},//long exp[15,8]
    {0x3502, 0x00},//long exp[7,0]
};

const I2C_ARRAY vts_reg[] = {
    {0x380e, 0x09},
    {0x380f, 0x0a},
//    {0x3208, 0x10},//Group 0 hold end
//    {0x3208, 0xa0},// Group delay launch
};

const I2C_ARRAY mirror_reg_DCG[] = {
    {0x3820, 0x00},//Flip
    {0x3821, 0x24},//mirror
};

const static I2C_ARRAY gain_vc1_reg[] = {
    {0x350C, 0x00},//short
    {0x350D, 0x80},
    {0x350E, 0x80},//Dgain
    {0x350F, 0x80},
};

const I2C_ARRAY expo_vc0_reg[] = {
    {0x3501, 0x00},//long
    {0x3502, 0x40},
};

const I2C_ARRAY expo_vc1_reg[] = {
    {0x3511, 0x00},//short
    {0x3512, 0x20},
};

#if 0
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
#endif

/////////// function definition ///////////////////
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME os08a10

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////

static int OS08A10_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    }
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_UDELAY(20);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_UDELAY(20);
    ///////////////////

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_UDELAY(20);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(6000);

    //sensor_if->Set3ATaskOrder(handle, def_order);
    // pure power on
    //ISP_config_io(handle);
    //sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    //SENSOR_USLEEP(5000);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;
}

static int OS08A10_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_UDELAY(30);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_UDELAY(30);
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }


    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int OS08A10_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i,n;
    //u16 sen_data1,sen_data2;
    int table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

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
      *id = ((*id)+ id_from_sensor[i].data)<<8;

    *id >>= 8;
    SENSOR_DMSG("[%s]OS08A10 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
   // printf("OS08A10 Read sensor id, get 0x%x Success\n", (int)*id);

}
    return SUCCESS;
}

static int OS08A10_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    return SUCCESS;
}

static int OS08A10_SetFPS(ms_cus_sensor *handle, u32 fps);
static int OS08A10_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int OS08A10_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int OS08A10_init_DCG_8m20(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_DCG_8m20);i++)
    {
        if(Sensor_init_table_DCG_8m20[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_DCG_8m20[i].data);
        }
        if(SensorReg_Write(Sensor_init_table_DCG_8m20[i].reg,Sensor_init_table_DCG_8m20[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table_DCG_8m20[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg_DCG); i++)
    {
        if(SensorReg_Write(mirror_reg_DCG[i].reg,mirror_reg_DCG[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_init_DCG_8m30(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_DCG_8m30);i++)
    {
        if(Sensor_init_table_DCG_8m30[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_DCG_8m30[i].data);
        }
        if(SensorReg_Write(Sensor_init_table_DCG_8m30[i].reg,Sensor_init_table_DCG_8m30[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table_DCG_8m30[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg_DCG); i++)
    {
        if(SensorReg_Write(mirror_reg_DCG[i].reg,mirror_reg_DCG[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_init_DCG_5m30(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_DCG_5m30);i++)
    {
        if(Sensor_init_table_DCG_5m30[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_DCG_5m30[i].data);
        }
        if(SensorReg_Write(Sensor_init_table_DCG_5m30[i].reg,Sensor_init_table_DCG_5m30[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table_DCG_5m30[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg_DCG); i++)
    {
        if(SensorReg_Write(mirror_reg_DCG[i].reg,mirror_reg_DCG[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_init_DCG_4p8m30(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_DCG_4p8m30);i++)
    {
        if(Sensor_init_table_DCG_4p8m30[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_DCG_4p8m30[i].data);
        }
        if(SensorReg_Write(Sensor_init_table_DCG_4p8m30[i].reg,Sensor_init_table_DCG_4p8m30[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table_DCG_4p8m30[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg_DCG); i++)
    {
        if(SensorReg_Write(mirror_reg_DCG[i].reg,mirror_reg_DCG[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_init_DCG_2m30(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_DCG_2m30);i++)
    {
        if(Sensor_init_table_DCG_2m30[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_DCG_2m30[i].data);
        }
        if(SensorReg_Write(Sensor_init_table_DCG_2m30[i].reg,Sensor_init_table_DCG_2m30[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table_DCG_2m30[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg_DCG); i++)
    {
        if(SensorReg_Write(mirror_reg_DCG[i].reg,mirror_reg_DCG[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_8m30_init(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_8m30_init_table);i++)
    {
        if(Sensor_8m30_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_8m30_init_table[i].data);
        }
        if(SensorReg_Write(Sensor_8m30_init_table[i].reg,Sensor_8m30_init_table[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
    {
        if(SensorReg_Write(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
    {
        if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_8m20_init(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_8m20_init_table);i++)
    {
        if(Sensor_8m20_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_8m20_init_table[i].data);
        }
        if(SensorReg_Write(Sensor_8m20_init_table[i].reg,Sensor_8m20_init_table[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
    {
        if(SensorReg_Write(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
    {
        if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_4p8m30_init(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_4p8m30_init_table);i++)
    {
        if(Sensor_4p8m30_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_4p8m30_init_table[i].data);
        }
        if(SensorReg_Write(Sensor_4p8m30_init_table[i].reg,Sensor_4p8m30_init_table[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
     // SensorReg_Read(Sensor_init_table[i].reg, &sen_data);
      // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
    {
        if(SensorReg_Write(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
    {
        if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_5m30_init(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_5m30_init_table);i++)
    {
        if(Sensor_5m30_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_5m30_init_table[i].data);
        }
        if(SensorReg_Write(Sensor_5m30_init_table[i].reg,Sensor_5m30_init_table[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
        // SensorReg_Read(Sensor_init_table[i].reg, &sen_data);
        // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
    {
        if(SensorReg_Write(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
    {
        if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_2m60_init(ms_cus_sensor *handle)
{
    int i;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_2m60_init_table);i++)
    {
        if(Sensor_2m60_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_2m60_init_table[i].data);
        }
        if(SensorReg_Write(Sensor_2m60_init_table[i].reg,Sensor_2m60_init_table[i].data) != SUCCESS)
        {
           SENSOR_DMSG("[%s] I2C write fail\n", __FUNCTION__);
            return FAIL;
        }
        // SensorReg_Read(Sensor_init_table[i].reg, &sen_data);
        // printf("[%s] i=0x%x,sen_data=0x%x\n", __FUNCTION__,i,sen_data);
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
    {
        if(SensorReg_Write(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
    {
        if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS08A10_GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}

static int OS08A10_GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int OS08A10_GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int OS08A10_SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0://3840x2160@30fps
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = OS08A10_8m30_init;
            vts_30fps = 2314;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period = 14405;
            break;
        case 1://2592x1944@30fps
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = OS08A10_5m30_init;
            vts_30fps = 2314;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period = 14405;
            break;
        case 2://2944x1656@30fps
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = OS08A10_4p8m30_init;
            vts_30fps = 2314;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period = 14405;
            break;
        case 3://1920x1080@60fps
            handle->video_res_supported.ulcur_res = 3;
            handle->pCus_sensor_init = OS08A10_2m60_init;
            vts_30fps = 1786;//1158;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period = 9332;//14392;
            break;
        case 4://3840x2160@30fps
            handle->video_res_supported.ulcur_res = 4;
            handle->pCus_sensor_init = OS08A10_8m20_init;
            vts_30fps = 2314;
            params->expo.vts = vts_30fps;
            params->expo.fps = 20;
            Preview_line_period = 21607;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int OS08A10_SetVideoRes_HDR_DCG(ms_cus_sensor *handle, u32 res_idx)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0://3840x2160@30fps
            handle->video_res_supported.ulcur_res = 0;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
                handle->pCus_sensor_init = OS08A10_init_DCG_8m30;
            }
            vts_30fps_DCG = 2314;
            params->expo.vts = vts_30fps_DCG;
            Preview_line_period_DCG = 14405;
            params->expo.fps = 20;
            params->expo.max_short=136;
            break;
        case 1://2592x1944@30fps
            handle->video_res_supported.ulcur_res = 1;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
                handle->pCus_sensor_init = OS08A10_init_DCG_5m30;
            }
            vts_30fps_DCG = 2094;
            params->expo.vts = vts_30fps_DCG;
            Preview_line_period_DCG = 15918;
            params->expo.fps = 20;
            params->expo.max_short=122;
            break;
        case 2://2944x1656@30fps
            handle->video_res_supported.ulcur_res = 2;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
                handle->pCus_sensor_init = OS08A10_init_DCG_4p8m30;
            }
            vts_30fps_DCG = 1806;
            params->expo.vts = vts_30fps_DCG;
            Preview_line_period_DCG = 18457;
            params->expo.fps = 20;
			params->expo.max_short=106;
            break;
        case 3:
            handle->video_res_supported.ulcur_res = 3;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
                handle->pCus_sensor_init = OS08A10_init_DCG_2m30;
            }
            vts_30fps_DCG = 1158;
            params->expo.vts = vts_30fps_DCG;
            Preview_line_period_DCG = 28785;
            params->expo.fps = 20;
			params->expo.max_short=68;
            break;
        case 4:
            handle->video_res_supported.ulcur_res = 4;
            if (handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num == 1) {
                handle->pCus_sensor_init = OS08A10_init_DCG_8m20;
            }
            vts_30fps_DCG = 2314;
            params->expo.vts = vts_30fps_DCG;
            Preview_line_period_DCG = 21607;
			params->expo.max_short=136;
            params->expo.fps = 20;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int OS08A10_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    os08A10_params *params = (os08A10_params *)handle->private_data;
    return params->cur_orien;
}

static int OS08A10_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    switch(orit)
    {
        case CUS_ORIT_M0F0:
            params->tMirror_reg[0].data = 0;
            params->tMirror_reg[1].data = 0x04;
            params->mirror_dirty = true;
        break;
        case CUS_ORIT_M1F0:
            params->tMirror_reg[0].data = 0;
            params->tMirror_reg[1].data = 0;
            params->mirror_dirty = true;
        break;
        case CUS_ORIT_M0F1:
            params->tMirror_reg[0].data = 0x04;
            params->tMirror_reg[1].data = 0x04;
            params->mirror_dirty = true;
            break;
        case CUS_ORIT_M1F1:
            params->tMirror_reg[0].data = 0x04;
            params->tMirror_reg[1].data = 0;
            params->mirror_dirty = true;
            break;
        default :
            break;
    }

    //SensorReg_Write(0x0100,0x01);
    params->cur_orien = orit;
    return SUCCESS;
}

static int OS08A10_SetOrien_HDR(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{

    os08A10_params *params = (os08A10_params *)handle->private_data;

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            params->tMirror_reg_DCG[0].data = 0;
            params->tMirror_reg_DCG[1].data = 0x24;
            params->mirror_dirty = true;
        break;
        case CUS_ORIT_M1F0:
            params->tMirror_reg_DCG[0].data = 0;
            params->tMirror_reg_DCG[1].data = 0x20;
            params->mirror_dirty = true;
        break;
        case CUS_ORIT_M0F1:
            params->tMirror_reg_DCG[0].data = 0x24;
            params->tMirror_reg_DCG[1].data = 0x24;
            params->mirror_dirty = true;
            break;
        case CUS_ORIT_M1F1:
            params->tMirror_reg_DCG[0].data = 0x24;
            params->tMirror_reg_DCG[1].data = 0x20;
            params->mirror_dirty = true;
            break;
        default :
            break;
}

    //SensorReg_Write(0x0100,0x01);
    params->cur_orien = orit;
    return SUCCESS;
}

static int OS08A10_GetFPS(ms_cus_sensor *handle)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int OS08A10_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts = 0;
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps)/fps;
    }else if(fps>=(min_fps*1000) && fps <= (max_fps*1000)){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if ((params->expo.lines) > (params->expo.vts - 8))
        vts = params->expo.lines +8;
    else
        vts = params->expo.vts;
    params->tVts_reg[0].data = (vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
    params->dirty = true;
    return SUCCESS;
}

static int OS08A10_GetFPS_HDR_lef(ms_cus_sensor *handle)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_DCG*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_DCG*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int OS08A10_SetFPS_HDR_lef(ms_cus_sensor *handle, u32 fps)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_DCG*max_fps)/fps;
    }else if(fps>=(min_fps*1000) && fps <= (max_fps*1000)){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_DCG*(max_fps*1000))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    //pr_info("[%s] %d  %d \n\n", __FUNCTION__, params->expo.vts, fps);
    params->expo.max_short = (((params->expo.vts)/17 - 1)>>1) << 1;
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    params->dirty = true;
    return SUCCESS;
}

static int OS08A10_GetFPS_HDR_sef(ms_cus_sensor *handle)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_DCG*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_DCG*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int OS08A10_SetFPS_HDR_sef(ms_cus_sensor *handle, u32 fps)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_DCG*max_fps)/fps;
    }else if(fps>=(min_fps*1000) && fps <= (max_fps*1000)){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_DCG*(max_fps*1000))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    //pr_info("[%s] %d  %d \n\n", __FUNCTION__, params->expo.vts, fps);
    params->expo.max_short = (((params->expo.vts)/17 - 1)>>1) << 1;
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    params->dirty = true;
    return SUCCESS;
}

static int OS08A10_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else
        return FAIL;

    return SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int OS08A10_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->mirror_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, ARRAY_SIZE(mirror_reg));
            params->mirror_dirty = false;
        }
        if(params->dirty)
        {
            SensorReg_Write(0x3208, 0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
            SensorReg_Write(0x3208, 0x10);
            SensorReg_Write(0x3208, 0xa0);
            params->dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int OS08A10_AEStatusNotify_HDR_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    //os08A10_params *params = (os08A10_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:

        break;
        default :
        break;
    }
    return SUCCESS;
}

static int OS08A10_AEStatusNotify_HDR_sef(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->mirror_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg_DCG, ARRAY_SIZE(mirror_reg_DCG));
            params->mirror_dirty = false;
        }
        if(params->dirty)
        {
//            SensorReg_Write(0x3208, 0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_vc0_reg, ARRAY_SIZE(expo_vc0_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_vc1_reg, ARRAY_SIZE(expo_vc1_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_vc1_reg, ARRAY_SIZE(gain_vc1_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
//            SensorReg_Write(0x3208, 0x10);
            params->dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int OS08A10_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc = SUCCESS;
    u32 lines = 0;
    //rc = SensorRegArrayR((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<16;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xff)<<0;

    *us = (lines*Preview_line_period);

    return rc;
}

static int OS08A10_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;
    if (lines >params->expo.vts-8)
        vts = lines +8;
    else
        vts=params->expo.vts;
    params->expo.lines = lines;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );
    // lines <<= 4;
    params->tExpo_reg[0].data = (lines>>16) & 0x000f;
    params->tExpo_reg[1].data = (lines>>8) & 0x00ff;
    params->tExpo_reg[2].data = (lines>>0) & 0x00ff;

    params->tVts_reg[0].data = (vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->dirty = true;

    return SUCCESS;
}

static int OS08A10_GetAEUSecs_HDR_lef(ms_cus_sensor *handle, u32 *us)
{
    int rc = SUCCESS;
    u32 lines = 0;
    //rc = SensorRegArrayR((I2C_ARRAY*)params->tExpo_vc0_reg, ARRAY_SIZE(expo_vc0_reg));
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines |= (u32)(params->tExpo_vc0_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_vc0_reg[1].data&0xff)<<0;

    *us = (lines*Preview_line_period_DCG);

    return rc;
}

static int OS08A10_SetAEUSecs_HDR_lef(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period_DCG;
    if (lines > ((params->expo.vts) - (params->expo.max_short) - 4))
        lines = (params->expo.vts) - (params->expo.max_short) - 4;
    else
        vts=params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );
    // lines <<= 4;
    params->tExpo_vc0_reg[0].data = (lines>>8) & 0x00ff;
    params->tExpo_vc0_reg[1].data = (lines>>0) & 0x00ff;

    //pr_info("[%s] shutter %d  0x3e01 0x%x  0x3e02 0x%x\n", __FUNCTION__, us, params->tExpo_vc0_reg[0].data,params->tExpo_vc0_reg[1].data);
    params->dirty = true;

    return SUCCESS;
}

static int OS08A10_GetAEUSecs_HDR_sef(ms_cus_sensor *handle, u32 *us)
{
    int rc = SUCCESS;
    u32 lines = 0;
    //rc = SensorRegArrayR((I2C_ARRAY*)params->tExpo_vc1_reg, ARRAY_SIZE(expo_vc1_reg));
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines |= (u32)(params->tExpo_vc1_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_vc1_reg[1].data&0xff)<<0;

    *us = (lines*Preview_line_period_DCG);

    return rc;
}

static int OS08A10_SetAEUSecs_HDR_sef(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    os08A10_params *params = (os08A10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period_DCG;
    if (lines > ((params->expo.max_short) - 4))
        lines = (params->expo.max_short) - 4;
    else
        vts=params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );
    // lines <<= 4;
    params->tExpo_vc1_reg[0].data = (lines>>8) & 0x00ff;
    params->tExpo_vc1_reg[1].data = (lines>>0) & 0x00ff;
    //pr_info("[%s] shutter %d  0x3e11 0x%x  0x3e12 0x%x \n", __FUNCTION__, us, params->tExpo_vc1_reg[0].data,params->tExpo_vc1_reg[1].data);
    params->dirty = true;

    return SUCCESS;
}

// Gain: 1x = 1024
static int OS08A10_GetAEGain(ms_cus_sensor *handle, u32* gain)
{

    //SENSOR_DMSG("[%s] get gain/reg0/reg1 (1024=1X)= %d/0x%x/0x%x\n", __FUNCTION__, *gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
    return SUCCESS;
}

#define MAX_A_GAIN 15872//(15.5*1024)
static int OS08A10_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u32 i,input_gain = 0;
    u16 gain16 = 0;

    if (gain < 1024) gain = 1024;
    else if (gain >= SENSOR_MAX_GAIN) gain = SENSOR_MAX_GAIN;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;
    if(gain<1024)
        gain=1024;
    else if(gain>=MAX_A_GAIN)
        gain=MAX_A_GAIN;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    /* A Gain */
    if (gain < 1024) {
        gain=1024;
    } else if ((gain >=1024) && (gain < 2048)) {
        gain = (gain>>6)<<6;
    } else if ((gain >=2048) && (gain < 4096)) {
        gain = (gain>>7)<<7;
    } else if ((gain >= 4096) && (gain < 8192)) {
        gain = (gain>>8)<<8;
    } else if ((gain >= 8192) && (gain < MAX_A_GAIN)) {
        gain = (gain>>9)<<9;
    } else {
        gain = MAX_A_GAIN;
    }

    gain16=(u16)(gain>>3);
    params->tGain_reg[0].data = (gain16>>8)&0x3f;//high bit
    params->tGain_reg[1].data = gain16&0xff; //low byte

    if(input_gain > MAX_A_GAIN){
        params->tGain_reg[2].data=(u16)((input_gain*4)/MAX_A_GAIN)&0x3F;
        params->tGain_reg[3].data=(u16)((input_gain*1024)/MAX_A_GAIN)&0xFF;
    }
    else{
        u16 tmp_dgain = ((input_gain*1024)/gain);
        params->tGain_reg[2].data=(u16)((tmp_dgain >> 8) & 0x3F);
        params->tGain_reg[3].data=(u16)(tmp_dgain & 0xFF);
    }

    //pr_info("[%s] gain %d  0x3508 0x%x  0x3509 0x%x\n", __FUNCTION__, input_gain, params->tGain_reg[0].data, params->tGain_reg[1].data);
    params->dirty = true;
    //pr_info("[%s] set input gain/gain/AregH/AregL/DregH/DregL=%d/%d/0x%x/0x%x/0x%x/0x%x\n", __FUNCTION__, input_gain,gain,params->tGain_reg[0].data,params->tGain_reg[1].data,params->tGain_reg[2].data,params->tGain_reg[3].data);

    return SUCCESS;
}

static int OS08A10_SetAEGain_HDR_sef(ms_cus_sensor *handle, u32 gain)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u32 i,input_gain = 0;
    u16 gain16 = 0;

    if (gain < 1024) gain = 1024;
    else if (gain >= SENSOR_MAX_GAIN) gain = SENSOR_MAX_GAIN;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;
    if(gain<1024)
        gain=1024;
    else if(gain>=MAX_A_GAIN)
        gain=MAX_A_GAIN;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    /* A Gain */
    if (gain < 1024) {
        gain=1024;
    } else if ((gain >=1024) && (gain < 2048)) {
        gain = (gain>>6)<<6;
    } else if ((gain >=2048) && (gain < 4096)) {
        gain = (gain>>7)<<7;
    } else if ((gain >= 4096) && (gain < 8192)) {
        gain = (gain>>8)<<8;
    } else if ((gain >= 8192) && (gain < MAX_A_GAIN)) {
        gain = (gain>>9)<<9;
    } else {
        gain = MAX_A_GAIN;
    }

    gain16=(u16)(gain>>3);
    params->tGain_vc1_reg[0].data = (gain16>>8)&0x3f;//high bit
    params->tGain_vc1_reg[1].data = gain16&0xff; //low byte

    if(input_gain > MAX_A_GAIN){
        params->tGain_vc1_reg[2].data=(u16)((input_gain*4)/MAX_A_GAIN)&0x3F;
        params->tGain_vc1_reg[3].data=(u16)((input_gain*1024)/MAX_A_GAIN)&0xFF;
    }
    else{
        u16 tmp_dgain = ((input_gain*1024)/gain);
        params->tGain_vc1_reg[2].data=(u16)((tmp_dgain >> 8) & 0x3F);
        params->tGain_vc1_reg[3].data=(u16)(tmp_dgain & 0xFF);
    }

    //pr_info("[%s] gain %d  0x350c 0x%x  0x350d 0x%x\n", __FUNCTION__, input_gain, params->tGain_vc1_reg[0].data, params->tGain_vc1_reg[1].data);

    params->dirty = true;
    //pr_info("[%s] set input gain/gain/AregH/AregL/DregH/DregL=%d/%d/0x%x/0x%x/0x%x/0x%x\n", __FUNCTION__, input_gain,gain,params->tGain_reg[0].data,params->tGain_reg[1].data,params->tGain_reg[2].data,params->tGain_reg[3].data);

    return SUCCESS;
}

static int OS08A10_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    //CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u32 input_gain = 0;
    u16 gain16;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;

    if(gain<1024)
        gain=1024;
    else if(gain>=MAX_A_GAIN)
        gain=MAX_A_GAIN;

    gain16=(u16)(gain>>3);
    params->tGain_reg[0].data = (gain16>>8)&0x3f;//high bit
    params->tGain_reg[1].data = gain16&0xff; //low byte

    if(input_gain > MAX_A_GAIN){
        params->tGain_reg[2].data=(u16)((input_gain*4)/MAX_A_GAIN)&0x3F;
        params->tGain_reg[3].data=(u16)((input_gain*1024)/MAX_A_GAIN)&0xFF;
    }
    else{
        params->tGain_reg[2].data=0x04;
        params->tGain_reg[3].data=0;
    }

    SENSOR_DMSG("[%s] set input gain/gain/regH/regL=%d/%d/0x%x/0x%x\n", __FUNCTION__, input_gain,gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
    return SUCCESS;
}

static int OS08A10_SetAEGain_cal_lef(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int OS08A10_setCaliData_gain_linearity_hdr_lef(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}

static int OS08A10_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int OS08A10_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {

  *min = SENSOR_MIN_GAIN;//1024*1.52;
  *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int OS08A10_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }

    return SUCCESS;
}

static int OS08A10_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period*2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int OS08A10_GetShutterInfo_HDR_SEF(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    os08A10_params *params = (os08A10_params *)handle->private_data;
    info->max = Preview_line_period_DCG*params->expo.max_short;
    info->min = Preview_line_period_DCG*3;
    info->step = Preview_line_period_DCG;
    return SUCCESS;
}

static int OS08A10_GetShutterInfo_HDR_LEF(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period_DCG*3;
    info->step = Preview_line_period_DCG;
    return SUCCESS;
}

static int OS08A10_SetPatternMode_hdr_lef(ms_cus_sensor *handle,u32 mode)
{
    return SUCCESS;
}

static int OS08A10_poweron_hdr_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int OS08A10_poweroff_hdr_lef(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int OS08A10_GetSensorID_hdr_lef(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}

static int OS08A10_init_hdr_lef(ms_cus_sensor *handle)
{
    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    os08A10_params *params;
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
    params = (os08A10_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tMirror_reg_DCG, mirror_reg_DCG, sizeof(mirror_reg_DCG));
    memcpy(params->tGain_vc1_reg, gain_vc1_reg, sizeof(gain_vc1_reg));
    memcpy(params->tExpo_vc0_reg, expo_vc0_reg, sizeof(expo_vc0_reg));
    memcpy(params->tExpo_vc1_reg, expo_vc1_reg, sizeof(expo_vc1_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"OS08A10_MIPI");

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
        handle->video_res_supported.res[res].width         = os08a10_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = os08a10_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = os08a10_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = os08a10_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = os08a10_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = os08a10_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = os08a10_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = os08a10_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, os08a10_mipi_linear[res].senstr.strResDesc);
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
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////

    //Mirror / Flip
    params->cur_orien = SENSOR_ORIT;

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=SENSOR_MIN_GAIN;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = OS08A10_8m30_init    ;
    handle->pCus_sensor_poweron     = OS08A10_poweron ;
    handle->pCus_sensor_poweroff    = OS08A10_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = OS08A10_GetSensorID   ;
    handle->pCus_sensor_GetVideoResNum = OS08A10_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = OS08A10_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = OS08A10_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = OS08A10_SetVideoRes;
    handle->pCus_sensor_GetOrien          = OS08A10_GetOrien      ;
    handle->pCus_sensor_SetOrien          = OS08A10_SetOrien      ;
    handle->pCus_sensor_GetFPS          = OS08A10_GetFPS      ;
    handle->pCus_sensor_SetFPS          = OS08A10_SetFPS      ;
    //handle->pCus_sensor_GetSensorCap    = OS08A10_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = OS08A10_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify = OS08A10_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = OS08A10_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = OS08A10_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = OS08A10_GetAEGain;
    handle->pCus_sensor_SetAEGain       = OS08A10_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = OS08A10_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= OS08A10_GetAEMinMaxUSecs;

    handle->pCus_sensor_GetShutterInfo = OS08A10_GetShutterInfo;


     //sensor calibration
    //handle->pCus_sensor_setCaliData_mingain=OS08A10_setCaliData_mingain;
    handle->pCus_sensor_SetAEGain_cal   = OS08A10_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=OS08A10_setCaliData_gain_linearity;

    params->expo.vts=vts_30fps;
    params->expo.fps = 20;
    params->expo.lines = 1000;
    params->mirror_dirty = false;
    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dcg_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    os08A10_params *params = NULL;
    int res;

    cus_camsensor_init_handle(drv_handle);
    params = (os08A10_params *)handle->private_data;

    sprintf(handle->model_id,"OS08A10_MIPI_HDR_SEF");

    handle->data_prec   = SENSOR_DATAPREC_HDR;  //CUS_DATAPRECISION_8;
    handle->bayer_id    = SENSOR_BAYERID_HDR;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_HDR;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0;
    for (res = 0; res < HDR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = os08a10_mipi_hdr[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = os08a10_mipi_hdr[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = os08a10_mipi_hdr[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = os08a10_mipi_hdr[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = os08a10_mipi_hdr[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = os08a10_mipi_hdr[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = os08a10_mipi_hdr[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = os08a10_mipi_hdr[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, os08a10_mipi_hdr[res].senstr.strResDesc);
    }

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED_HDR;

    //Mirror / Flip
    params->cur_orien = SENSOR_ORIT;

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;
    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=SENSOR_MIN_GAIN;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_init        = OS08A10_init_DCG_8m30;

    // Normal
    handle->pCus_sensor_SetVideoRes       = OS08A10_SetVideoRes_HDR_DCG;
    handle->pCus_sensor_GetFPS          = OS08A10_GetFPS_HDR_sef;
    handle->pCus_sensor_SetFPS          = OS08A10_SetFPS_HDR_sef;

    handle->pCus_sensor_AEStatusNotify = OS08A10_AEStatusNotify_HDR_sef;
    handle->pCus_sensor_GetAEUSecs      = OS08A10_GetAEUSecs_HDR_sef;
    handle->pCus_sensor_SetAEUSecs      = OS08A10_SetAEUSecs_HDR_sef;
    handle->pCus_sensor_GetAEGain       = OS08A10_GetAEGain;
    handle->pCus_sensor_SetAEGain       = OS08A10_SetAEGain_HDR_sef;

    handle->pCus_sensor_GetShutterInfo = OS08A10_GetShutterInfo_HDR_SEF;

    params->expo.vts=vts_30fps_DCG;
    params->expo.fps = 20;
    params->expo.max_short=70;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dcg_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    os08A10_params *params;
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
    params = (os08A10_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tMirror_reg_DCG, mirror_reg_DCG, sizeof(mirror_reg_DCG));
    memcpy(params->tGain_vc1_reg, gain_vc1_reg, sizeof(gain_vc1_reg));
    memcpy(params->tExpo_vc0_reg, expo_vc0_reg, sizeof(expo_vc0_reg));
    memcpy(params->tExpo_vc1_reg, expo_vc1_reg, sizeof(expo_vc1_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"OS08A10_MIPI_HDR_LEF");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC_HDR;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID_HDR;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_HDR;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0;
    for (res = 0; res < HDR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = os08a10_mipi_hdr[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = os08a10_mipi_hdr[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = os08a10_mipi_hdr[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = os08a10_mipi_hdr[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = os08a10_mipi_hdr[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = os08a10_mipi_hdr[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = os08a10_mipi_hdr[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = os08a10_mipi_hdr[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, os08a10_mipi_hdr[res].senstr.strResDesc);
    }

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED_HDR;

    //polarity
    /////////////////////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    //handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////

    //Mirror / Flip
    params->cur_orien = SENSOR_ORIT;

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=SENSOR_MIN_GAIN;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = OS08A10_init_hdr_lef    ;
    handle->pCus_sensor_poweron     = OS08A10_poweron_hdr_lef ;
    handle->pCus_sensor_poweroff    = OS08A10_poweroff_hdr_lef;

    // Normal
    handle->pCus_sensor_GetSensorID       = OS08A10_GetSensorID_hdr_lef;
    handle->pCus_sensor_GetVideoResNum = OS08A10_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = OS08A10_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = OS08A10_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = OS08A10_SetVideoRes_HDR_DCG;
    handle->pCus_sensor_GetOrien          = OS08A10_GetOrien      ;
    handle->pCus_sensor_SetOrien          = OS08A10_SetOrien_HDR   ;
    handle->pCus_sensor_GetFPS          = OS08A10_GetFPS_HDR_lef;
    handle->pCus_sensor_SetFPS          = OS08A10_SetFPS_HDR_lef;
    handle->pCus_sensor_GetSensorCap    = OS08A10_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = OS08A10_SetPatternMode_hdr_lef;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify = OS08A10_AEStatusNotify_HDR_lef;
    handle->pCus_sensor_GetAEUSecs      = OS08A10_GetAEUSecs_HDR_lef;
    handle->pCus_sensor_SetAEUSecs      = OS08A10_SetAEUSecs_HDR_lef;
    handle->pCus_sensor_GetAEGain       = OS08A10_GetAEGain;
    handle->pCus_sensor_SetAEGain       = OS08A10_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = OS08A10_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= OS08A10_GetAEMinMaxUSecs;

    handle->pCus_sensor_GetShutterInfo = OS08A10_GetShutterInfo_HDR_LEF;


     //sensor calibration
    //handle->pCus_sensor_setCaliData_mingain=OS08A10_setCaliData_mingain;
    handle->pCus_sensor_SetAEGain_cal   = OS08A10_SetAEGain_cal_lef;
    handle->pCus_sensor_setCaliData_gain_linearity=OS08A10_setCaliData_gain_linearity_hdr_lef;

    params->expo.vts=vts_30fps_DCG;
    params->expo.fps = 20;
    params->expo.lines = 1000;
    params->mirror_dirty = false;
    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  OS08A10_HDR,
                            cus_camsensor_init_handle,
                            cus_camsensor_init_handle_hdr_dcg_sef,
                            cus_camsensor_init_handle_hdr_dcg_lef,
                            os08A10_params
                         );
