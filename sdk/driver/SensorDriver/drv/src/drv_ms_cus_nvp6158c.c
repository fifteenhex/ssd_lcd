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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(NVP6158C_BT656);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
//#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE            CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL

//============================================
#define ENABLE            1
#define DISABLE           0
#undef SENSOR_DBG
#define SENSOR_DBG        0

#define DEBUG_INFO        0

#define _SDR_MODE_1MUX_ 1
#define _SDR_MODE_2MUX_ 0
#define _SDR_MODE_4MUX_ 0

#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) LOGD(args)
//#define SENSOR_DMSG(args...) LOGE(args)
//#define SENSOR_DMSG(args...) printf(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
///////////////////////////////////////////////////////////////
//          @@@                                              //
//         @ @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//          @@@@                                             //
//                                                           //
//      Start Step 1 --  show preview on LCM                 //
//                                                           //
//  Fill these #define value and table with correct settings //
//      camera can work and show preview on LCM              //
//                                                           //
///////////////////////////////////////////////////////////////

////////////////////////////////////
// Sensor-If Info                 //
////////////////////////////////////
//MIPI config begin.
#define SENSOR_BT656_CH_TOTAL_NUM   (1)
#define SENSOR_BT656_CH_DET_ENABLE  CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_CH_DET_SEL     CUS_VIF_BT656_EAV_DETECT
#define SENSOR_BT656_BIT_SWAP       CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_8BIT_MODE      CUS_SENSOR_FUNC_ENABLE
#define SENSOR_BT656_VSYNC_DELAY    CUS_VIF_BT656_VSYNC_DELAY_AUTO
#define SENSOR_HSYCN_INVERT         CUS_SENSOR_FUNC_DISABLE
#define SENSOR_VSYCN_INVERT         CUS_SENSOR_FUNC_DISABLE
#define SENSOR_CLAMP_ENABLE         CUS_SENSOR_FUNC_DISABLE

#define SENSOR_ISP_TYPE             ISP_EXT             //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER    //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_BT656  //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9098  //CFG
#define SENSOR_BAYERID              CUS_BAYER_RG         //0h: CUS_BAYER_RG, 1h: CUS_BAYER_GR, 2h: CUS_BAYER_BG, 3h: CUS_BAYER_GB
#define SENSOR_RGBIRID              CUS_RGBIR_NONE
#define SENSOR_ORIT                 CUS_ORIT_M0F0        //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER              CUS_SEN_YCODR_YC     //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define long_packet_type_enable     0x00 //UD1~UD8 (user define)

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_27MHZ    //CUS_CMU_CLK_24MHZ //CUS_CMU_CLK_37P125MHZ//CUS_CMU_CLK_27MHZ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x60                //I2C slave address
#define SENSOR_I2C_SPEED             300000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL           1                 //I2C Channel
//#define SENSOR_I2C_PAD_MODE          2                 //Pad/Mode Number
#define SENSOR_I2C_LEGACY            I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT               I2C_FMT_A8D8       //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

////////////////////////////////////
// Sensor Signal                  //
////////////////////////////////////
#define SENSOR_PWDN_POL              CUS_CLK_POL_NEG     // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL               CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG
                                                         // VSYNC/HSYNC POL can be found in data sheet timing diagram,
                                                         // Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.
#define SENSOR_VSYNC_POL             CUS_CLK_POL_NEG     // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL             CUS_CLK_POL_NEG     // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL              CUS_CLK_POL_NEG     // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

////////////////////////////////////
// Sensor ID                      //
////////////////////////////////////
//define SENSOR_ID

#undef SENSOR_NAME
#define SENSOR_NAME     NVP6158C_BT656

////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {     // LINEAR
    // Modify it based on number of support resolution
    enum {BT656_RES_1 = 0, BT656_RES_2, BT656_RES_3, BT656_RES_4, BT656_RES_5, BT656_RES_6, BT656_RES_7, BT656_RES_8, BT656_RES_9, BT656_RES_10, BT656_RES_11, BT656_RES_12, BT656_RES_13, BT656_RES_END}mode;
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
}nvp6158c_bt656_linear[] = {
    {BT656_RES_1, {1280,  720, 3, 30}, {0, 0, 1280,  720}, {"1280x720@30fps_1Mux"}},
    {BT656_RES_2, {1280,  720, 3, 25}, {0, 0, 1280,  720}, {"1280x720@25fps_1Mux"}},
    {BT656_RES_3, {1280,  720, 3, 30}, {0, 0, 1280,  720}, {"1280x720@30fps_2Mux"}},
    {BT656_RES_4, {1280,  720, 3, 25}, {0, 0, 1280,  720}, {"1280x720@25fps_2Mux"}},
    {BT656_RES_5, {1280,  720, 3, 30}, {0, 0, 1280,  720}, {"1280x720@30fps_4Mux"}},
    {BT656_RES_6, {1280,  720, 3, 25}, {0, 0, 1280,  720}, {"1280x720@25fps_4Mux"}},
    {BT656_RES_7, {1920, 1080, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps_1Mux"}},
    {BT656_RES_8, {1920, 1080, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps_2Mux"}},
    {BT656_RES_9, {960, 1080, 3, 30}, {0, 0, 960, 1080}, {"960x1080@30fps_4Mux"}},
    {BT656_RES_10, {1920, 1080, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps_1Mux_bt1120"}},
    {BT656_RES_11, {1920, 1080, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps_2Mux_bt1120"}},
    {BT656_RES_12, {960, 240, 3, 60}, {0, 0, 960, 240}, {"960x240@60i_1Mux"}},
    {BT656_RES_13, {960, 290, 3, 60}, {0, 0, 960, 290}, {"960x290@50i_1Mux"}},
};


////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (0)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (0)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (0)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (0)
#define SENSOR_GAIN_DELAY_FRAME_COUNT_HDR_DOL       (0)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL    (0)

////////////////////////////////////
// Mirror-Flip Info               //
////////////////////////////////////
#define REG_MIRROR                                  0x0
#define REG_FLIP                                    0x0
#define SENSOR_NOR                                  0x0
#define MIRROR_EN                                   0x1 << (0)
#define FLIP_EN                                     0x1 << (0)

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

//static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
//static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
//static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
//static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);

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
        u32 expo_lines;
        u32 expo_lef_us;
        u32 expo_sef_us;
    } expo;
    u32 max_rhs1;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orien_dirty;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tExpo_shr_dol1_reg[3];
    I2C_ARRAY tExpo_rhs1_reg[3];
    I2C_ARRAY tGain_hdr_dol_lef_reg[2];
    I2C_ARRAY tGain_hdr_dol_sef_reg[2];
} nvp6158c_params;
// set sensor ID address and data,

const static I2C_ARRAY Sensor_id_table[] =
{
    {0xFF, 0x00},      // {address of ID, ID },
    {0xF4, 0xA0}
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x06},
    {0x82, 0x06},
    {0x83, 0x06},
    {0x84, 0x06},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x00},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x00}, //port1 outputs ch1
    {0xc4, 0x11}, //port2 outputs ch2
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},
#if  _SDR_MODE_1MUX_
    {0xcd, 0x08}, //74.25m
    {0xce, 0x08}, //74.25m
#else  // _DDR_MODE
    {0xcd, 0x88}, //37.125m
    {0xce, 0x88}, //37.125m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x07},
    {0x82, 0x07},
    {0x83, 0x07},
    {0x84, 0x07},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x5f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x00},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x00}, //port1 outputs ch1
    {0xc4, 0x11}, //port2 outputs ch2
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},
#if _SDR_MODE_1MUX_
    {0xcd, 0x08}, //74.25m
    {0xce, 0x08}, //74.25m
#else
    {0xcd, 0x88}, //37.125m
    {0xce, 0x88}, //37.125m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x06},
    {0x82, 0x06},
    {0x83, 0x06},
    {0x84, 0x06},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x20},
    {0xa2, 0x20},
    {0xc2, 0x10}, //port1
    {0xc4, 0x32}, //port2
    {0xc8, 0x20}, //2mux
    {0xc9, 0x02},
#if _SDR_MODE_2MUX_
    {0xcd, 0x58}, //148.5m
    {0xce, 0x58}, //148.5m
#else //_SDR_MODE_
    {0xcd, 0x08}, //74.25m
    {0xce, 0x08}, //74.25m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x07},
    {0x82, 0x07},
    {0x83, 0x07},
    {0x84, 0x07},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x20},
    {0xa2, 0x20},
    {0xc2, 0x10}, //port1
    {0xc4, 0x32}, //port2
    {0xc8, 0x20}, //2mux
    {0xc9, 0x02},
#if _SDR_MODE_2MUX_
    {0xcd, 0x58}, //148.5m
    {0xce, 0x58}, //148.5m
#else
    {0xcd, 0x0f}, //74.25m
    {0xce, 0x0f}, //74.25m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x06},
    {0x82, 0x06},
    {0x83, 0x06},
    {0x84, 0x06},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x32},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x10}, //port1
    {0xc3, 0x32}, //port1
    {0xc4, 0x10}, //port2
    {0xc5, 0x32}, //port2
    {0xc8, 0x80}, //4mux
    {0xc9, 0x08},
#if _SDR_MODE_4MUX_
    {0xcd, 0x78}, //297m
    {0xce, 0x78}, //297m
#else //_SDR_MODE_
    {0xcd, 0x5f}, //148.5m
    {0xce, 0x5f}, //148.5m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x07},
    {0x82, 0x07},
    {0x83, 0x07},
    {0x84, 0x07},
    {0x8e, 0x03},
    {0x8f, 0x03},
    {0x90, 0x03},
    {0x91, 0x03},
    //{0x78, 0x10},
    //{0x79, 0x32},
    {0x30, 0x00},
    {0x31, 0x00},
    {0x32, 0x00},
    {0x33, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x12},
    {0x6d, 0xee},
    {0x6e, 0x10},
    {0x6f, 0x1e},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x32},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x10}, //port1
    {0xc3, 0x32}, //port1
    {0xc4, 0x10}, //port2
    {0xc5, 0x32}, //port2
    {0xc8, 0x80}, //4mux
    {0xc9, 0x08},
#if _SDR_MODE_4MUX_
    {0xcd, 0x73}, //297m
    {0xce, 0x73}, //297m
#else
    {0xcd, 0x53}, //148.5m
    {0xce, 0x53}, //148.5m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //{0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x02},
    {0x82, 0x02},
    {0x83, 0x02},
    {0x84, 0x02},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x00},
    {0x6d, 0x00},
    {0x6e, 0x00},
    {0x6f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x00},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x00}, //port1 outputs ch1
    {0xc4, 0x11}, //port2 outputs ch2
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},
#if _SDR_MODE_1MUX_
    {0xcd, 0x48}, //148.5m
    {0xce, 0x48}, //148.5m
#else //_DDR_MODE_
    {0xcd, 0x08}, //74.25m
    {0xce, 0x08}, //74.25m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x02},
    {0x82, 0x02},
    {0x83, 0x02},
    {0x84, 0x02},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x00},
    {0x6d, 0x00},
    {0x6e, 0x00},
    {0x6f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x10}, //port1
    {0xc4, 0x32}, //port2
    {0xc8, 0x20}, //2mux
    {0xc9, 0x02},
#if _SDR_MODE_2MUX_
    {0xcd, 0x63}, //297m
    {0xce, 0x63}, //297m
#else //_DDR_MODE_
    {0xcd, 0x53}, //148.5m
    {0xce, 0x53}, //148.5m
#endif
    {0xca, 0x66}, //port1/2_enable
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
};

const static I2C_ARRAY Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[] =
{
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x02},
    {0x82, 0x02},
    {0x83, 0x02},
    {0x84, 0x02},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x00},
    {0x6d, 0x00},
    {0x6e, 0x00},
    {0x6f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x98}, //port1
    {0xc3, 0xba}, //port1
    {0xc4, 0x98}, //port2
    {0xc5, 0xba}, //port2
    {0xc8, 0x80}, //4mux
    {0xc9, 0x08},
#if _SDR_MODE_4MUX_
    {0xcd, 0x68}, //297m
    {0xce, 0x68}, //297m
#else //_DDR_MODE_
    {0xcd, 0x58}, //148.5m
    {0xce, 0x58}, //148.5m
#endif
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[]  = {
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x02},
    {0x82, 0x02},
    {0x83, 0x02},
    {0x84, 0x02},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x6c, 0x00},
    {0x6d, 0x00},
    {0x6e, 0x00},
    {0x6f, 0x00},
    {0x5f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x00},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x44}, //port1 outputs ch1 Y
    {0xc3, 0x44}, //port1 outputs ch1 Y
    {0xc4, 0xcc}, //port2 outputs ch2 C
    {0xc5, 0xcc}, //port2 outputs ch2 C
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},
#if _SDR_MODE_1MUX_
    {0xcd, 0x03}, //74.25m
    {0xce, 0x03}, //74.25m
#else
    {0xcd, 0x83}, //37.125m
    {0xce, 0x83}, //37.125m
#endif
    {0xcb, 0x06}, //port1/2 data order inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[]  = {
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x02},
    {0x82, 0x02},
    {0x83, 0x02},
    {0x84, 0x02},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x02},
    {0x85, 0x02},
    {0x86, 0x02},
    {0x87, 0x02},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x40},
    {0x8f, 0x40},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xdc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x6c, 0x00},
    {0x6d, 0x00},
    {0x6e, 0x00},
    {0x6f, 0x00},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x00},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x01},
    {0x56, 0x10},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x20},
    {0xa2, 0x20},
    {0xc2, 0x54}, //port1 ch1/2 Y
    {0xc3, 0x54}, //port1 ch1/2 Y
    {0xc4, 0xdc}, //port2 ch1/2 C
    {0xc5, 0xdc}, //port2 ch1/2 C
    {0xc8, 0x20}, //2mux
    {0xc9, 0x02},
#if _SDR_MODE_2MUX_
    {0xcd, 0x58}, //148.5m
    {0xce, 0x58}, //148.5m
#else
    {0xcd, 0x03}, //74.25m
    {0xce, 0x03}, //74.25m
#endif
    {0xcb, 0x06}, //port1/2 data order inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[]  = {
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10}, 
    {0x02, 0x10}, 
    {0x03, 0x10},
    {0x08, 0xa0},
    {0x09, 0xa0}, 
    {0x0a, 0xa0}, 
    {0x0b, 0xa0},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b}, 
    {0x23, 0x41},
    {0x26, 0x0b}, 
    {0x27, 0x41},
    {0x2a, 0x0b}, 
    {0x2b, 0x41},
    {0x2e, 0x0b}, 
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x00},
    {0x82, 0x00},
    {0x83, 0x00},
    {0x84, 0x00},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x06},
    {0x85, 0x06}, 
    {0x86, 0x06}, 
    {0x87, 0x06},
    {0x8c, 0xa6},
    {0x8d, 0xa6}, 
    {0x8e, 0xa6}, 
    {0x8f, 0xa6},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xcc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x69, 0x01},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x80},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},   
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},   
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},   
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0xf0},
    {0x56, 0x10},
    {0x78, 0x11},
    {0x79, 0x11},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00}, 
    {0xa2, 0x00}, 
    {0xc2, 0x00}, //port1
    {0xc4, 0x11}, //port2
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},

    {0xcd, 0x88}, //37.125m
    {0xce, 0x88}, //37.125m
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};

const static I2C_ARRAY Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[]  = {
    {0xff, 0x00},
    {0x80, 0x00}, //config all
    {0x00, 0x10},
    {0x01, 0x10},
    {0x02, 0x10},
    {0x03, 0x10},
    {0x08, 0xdd},
    {0x09, 0xdd},
    {0x0a, 0xdd},
    {0x0b, 0xdd},
    {0x18, 0x1f},
    {0x19, 0x1f},
    {0x1a, 0x1f},
    {0x1b, 0x1f},
    {0x22, 0x0b},
    {0x23, 0x41},
    {0x26, 0x0b},
    {0x27, 0x41},
    {0x2a, 0x0b},
    {0x2b, 0x41},
    {0x2e, 0x0b},
    {0x2f, 0x41},
    {0x64, 0x21},
    {0x65, 0x21},
    {0x66, 0x21},
    {0x67, 0x21},
    {0x81, 0x10},
    {0x82, 0x10},
    {0x83, 0x10},
    {0x84, 0x10},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},

    {0xff, 0x01},
    {0x7A, 0x0f},
    {0x84, 0x06},
    {0x85, 0x06},
    {0x86, 0x06},
    {0x87, 0x06},
    {0x8c, 0xa6},
    {0x8d, 0xa6},
    {0x8e, 0xa6},
    {0x8f, 0xa6},

    {0xff, 0x05},
    {0x00, 0xf0},
    {0x01, 0x22},
    {0x25, 0xcc},
    {0x28, 0x80},
    {0x47, 0x04},
    {0x50, 0x84},
    {0x58, 0x77},
    {0x5c, 0x78},
    {0x5f, 0x00},
    {0x69, 0x01},
    {0x76, 0x00},
    {0x78, 0x00},
    {0x7a, 0x00},
    {0xa9, 0x80},
    {0xb8, 0xb9},
    {0xb9, 0x72},
    {0xd5, 0x80},

    {0xff, 0x09},
    {0x50, 0x30},
    {0x51, 0x6f},
    {0x52, 0x67},
    {0x53, 0x48},
    {0x54, 0x30},
    {0x55, 0x6f},
    {0x56, 0x67},
    {0x57, 0x48},
    {0x58, 0x30},
    {0x59, 0x6f},
    {0x5a, 0x67},
    {0x5b, 0x48},
    {0x5c, 0x30},
    {0x5d, 0x6f},
    {0x5e, 0x67},
    {0x5f, 0x48},
    {0x96, 0x00},
    {0x9e, 0x00},
    {0xb6, 0x00},
    {0xbe, 0x00},
    {0xd6, 0x00},
    {0xde, 0x00},
    {0xf6, 0x00},
    {0xfe, 0x00},

    {0xff, 0x0a},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},
    {0xff, 0x0b},
    {0x25, 0x10},
    {0x27, 0x1e},
    {0x30, 0xac},
    {0x31, 0x78},
    {0x32, 0x17},
    {0x33, 0xc1},
    {0x34, 0x40},
    {0x36, 0xc3},
    {0x37, 0x0a},
    {0x39, 0x02},
    {0x3b, 0xb2},
    {0xa5, 0x10},
    {0xa7, 0x1e},
    {0xb0, 0xac},
    {0xb1, 0x78},
    {0xb2, 0x17},
    {0xb3, 0xc1},
    {0xb4, 0x40},
    {0xb6, 0xc3},
    {0xb7, 0x0a},
    {0xb9, 0x02},
    {0xbb, 0xb2},

    {0xff, 0x00},
    {0x54, 0x00},
    {0x56, 0x10},
    {0x78, 0x11},
    {0x79, 0x11},
    {0xff, 0x01},
    {0x97, 0x00},
    {0x97, 0x0f}, //ch reset
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xc2, 0x00}, //port1 outputs ch1
    {0xc4, 0x11}, //port2 outputs ch2
    {0xc8, 0x00}, //1mux
    {0xc9, 0x00},
    {0xcd, 0x88}, //37.125m
    {0xce, 0x88}, //37.125m
    {0xcb, 0x06}, //nvp6158c evb data pin inverse
    {0xca, 0x66}, //port1/2_enable
};


static I2C_ARRAY PatternTbl[] = {
    {0, 0},
};

const static I2C_ARRAY mirr_flip_table[] = {
    {0, 0},
};

const static I2C_ARRAY gain_reg[] = {
    {0, 0},
};

const static I2C_ARRAY expo_reg[] = {
    {0, 0},
};

const static I2C_ARRAY vts_reg[] = {
    {0, 0},
};

#if 0
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
#endif
/////////////////////////////////////////////////////////////////
//       @@@@@@@                                               //
//           @@                                                //
//          @@                                                 //
//          @@@                                                //
//       @   @@                                                //
//        @@@@                                                 //
//                                                             //
//      Step 3 --  complete camera features                    //
//                                                             //
//  camera set EV, MWB, orientation, contrast, sharpness       //
//   , saturation, and Denoise can work correctly.             //
//                                                             //
/////////////////////////////////////////////////////////////////

/////////// function definition ///////////////////
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

/*******I5/I6 Support MCLK List*******
 *    CUS_CMU_CLK_27MHZ,
 *    CUS_CMU_CLK_21P6MHZ,
 *    CUS_CMU_CLK_12MHZ,
 *    CUS_CMU_CLK_5P4MHZ,
 *    CUS_CMU_CLK_36MHZ,
 *    CUS_CMU_CLK_54MHZ,
 *    CUS_CMU_CLK_43P2MHZ,
 *    CUS_CMU_CLK_61P7MHZ,
 *    CUS_CMU_CLK_72MHZ,
 *    CUS_CMU_CLK_48MHZ,
 *    CUS_CMU_CLK_24MHZ,
 *    CUS_CMU_CLK_37P125MHZ,
 ******End of Support MCLK List*******/
 #if 0
static CUS_MCLK_FREQ UseParaMclk(const char *mclk)
{
    if (strcmp(mclk, "27M") == 0) {
        return CUS_CMU_CLK_27MHZ;
    } else if (strcmp(mclk, "12M") == 0) {
        return CUS_CMU_CLK_12MHZ;
    } else if (strcmp(mclk, "36M") == 0) {
        return CUS_CMU_CLK_36MHZ;
    } else if (strcmp(mclk, "48M") == 0) {
        return CUS_CMU_CLK_48MHZ;
    } else if (strcmp(mclk, "54M") == 0) {
        return CUS_CMU_CLK_54MHZ;
    } else if (strcmp(mclk, "24M") == 0) {
        return CUS_CMU_CLK_24MHZ;
    } else if (strcmp(mclk, "37.125M") == 0) {
        return CUS_CMU_CLK_37P125MHZ;
    }
    return Preview_MCLK_SPEED;
}
#endif

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    //Sensor power on sequence
    sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);		//Set_csi_if(0, 0); //disable MIPI
    sensor_if->MCLK(idx, 1, handle->mclk);

    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(10);
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(10);
    //SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    //sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //SENSOR_MSLEEP(1);

    // power -> high, reset -> high
    //SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    //sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    //SENSOR_MSLEEP(1);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(100);

    //sensor_if->Set3ATaskOrder( def_order);
    // pure power on
    //sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);

    printk("Sensor Power On finished\n");
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    handle->orient = SENSOR_ORIT;

    return SUCCESS;
}

/////////////////// Check Sensor Product ID /////////////////////////
//Get and check sensor ID
static int pCus_CheckProductID(ms_cus_sensor *handle)
{
    u16 sen_data;

    // switch bank to bank0
    SensorReg_Write(Sensor_id_table[0].reg, Sensor_id_table[0].data);

    // read reg 0xF4
    SensorReg_Read(Sensor_id_table[1].reg, &sen_data);
    if(sen_data == Sensor_id_table[1].data){
        printk("[%s]This is RGBIR sensor and right driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return SUCCESS;
     }
    else{
        //SENSOR_DMSG("[%s]This is Bayer sensor and wrong driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        printk("[%s]This is Bayer sensor and wrong driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return FAIL;
    }

}

//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    u16 sen_data;

    // switch bank to bank0
    SensorReg_Write(Sensor_id_table[0].reg, Sensor_id_table[0].data);

    // read reg 0x34
    SensorReg_Read(Sensor_id_table[1].reg, &sen_data);

    if(sen_data == Sensor_id_table[1].data){
        *id = sen_data;
        SENSOR_DMSG("[%s]This is RGBIR sensor and right driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return SUCCESS;
     }
    else{
        *id = 0;
        SENSOR_DMSG("[%s]This is Bayer sensor and wrong driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return FAIL;
    }
}

static int nvp6158c_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;
    switch(mode)
    {
    case 1:
        PatternTbl[0].data = 0x21; //enable
        break;
    case 0:
        PatternTbl[0].data &= 0xFE; //disable
        break;
    default:
        PatternTbl[0].data &= 0xFE; //disable
        break;
      }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_720P30_8bitBT656_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //u16 sen_data, sen_bank;
    printk("\n[%s]", __FUNCTION__);

    if (pCus_CheckProductID(handle) == FAIL) {
        return FAIL;
    }

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(0xFF,  &sen_bank);
            SensorReg_Read(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg,  &sen_data);
            //if(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data != sen_data)
            //    printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg);
            printk("=>Bank %d reg: 0x%x, data: 0x%x, read: 0x%x.\n", sen_bank, Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
#if 0
    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg==0xff)
        {
            SensorReg_Write(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data);
            printk("Switch Bank 0x%x, 0x%x.\n",Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data);
        }
        else {
            SensorReg_Read(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg,  &sen_data);
            printk("=> NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data, sen_data);
        }
    }
#endif
    return SUCCESS;
}


static int pCus_init_NVP6158C_720P25_8bitBT656_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //u16 sen_data, sen_bank;
    printk("\n[%s]", __FUNCTION__);

    if (pCus_CheckProductID(handle) == FAIL) {
        return FAIL;
    }

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(0xFF,  &sen_bank);
            SensorReg_Read(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg,  &sen_data);
            //if(Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].data != sen_data)
            //    printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P30_8bitBT656_1MUX_TAB[i].reg);
            printk("=>Bank %d reg: 0x%x, data: 0x%x, read: 0x%x.\n", sen_bank, Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
#if 0
    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg==0xff)
        {
            SensorReg_Write(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data);
            printk("Switch Bank 0x%x, 0x%x.\n",Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data);
        }
        else {
            SensorReg_Read(Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg,  &sen_data);
            printk("=> NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_1MUX_TAB[i].data, sen_data);
        }
    }
#endif
    return SUCCESS;
}

static int pCus_init_NVP6158C_720P30_8bitBT656_2MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg,Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_2MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_720P25_8bitBT656_2MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg,Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_2MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

#if 1
static int pCus_init_NVP6158C_720P25_8bitBT656_4MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].reg,Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_720P25_8bitBT656_4MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}
#endif

static int pCus_init_NVP6158C_720P30_8bitBT656_4MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg,Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_720P30_8bitBT656_4MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_1080P30_8bitBT656_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_1080P30_8bitBT656_2MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg,Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_2MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_1080P30_8bitBT656_4MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg,Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg);
                //printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_8bitBT656_4MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_1080P30_16bitBT1120_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg,Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg);
                printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_16bitBT1120_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}


static int pCus_init_NVP6158C_1080P30_16bitBT1120_2MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg,Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg);
                printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].reg, Sensor_init_NVP6158C_1080P30_16bitBT1120_2MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}


static int pCus_init_NVP6158C_960h60i_8bitBT656_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //u16 sen_data;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg);
            printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_960h60i_8bitBT656_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}

static int pCus_init_NVP6158C_960h50i_8bitBT656_1MUX(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //u16 sen_data;

    printk("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB);i++)
    {
        if(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg,Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].data);
#if 0
            SensorReg_Read(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg, &sen_data );
            if(Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg);
            printk("NVP6158C reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].reg, Sensor_init_NVP6158C_960h50i_8bitBT656_1MUX_TAB[i].data, sen_data);
#endif
        }
    }
    return SUCCESS;
}


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
    //nvp6158c_params *params = (nvp6158c_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    handle->video_res_supported.ulcur_res = res_idx;

    switch (res_idx) {
        case BT656_RES_1:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P30_8bitBT656_1MUX;
#if 0
#if _SDR_MODE_1MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif
            break;
        case BT656_RES_2:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P25_8bitBT656_1MUX;
#if 0
#if _SDR_MODE_1MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif
            break;
        case BT656_RES_3:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P30_8bitBT656_2MUX;
#if 0
#if _SDR_MODE_2MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif
            break;
        case BT656_RES_4:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P25_8bitBT656_2MUX;
#if 0
#if _SDR_MODE_2MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_5:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P30_8bitBT656_4MUX;
#if 0
#if _SDR_MODE_4MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif 

            break;
        case BT656_RES_6:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_720P25_8bitBT656_4MUX;
#if 0
#if _SDR_MODE_4MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_7:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_1080P30_8bitBT656_1MUX;
#if 0
#if _SDR_MODE_1MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_8:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_1080P30_8bitBT656_2MUX;
#if 0
#if _SDR_MODE_2MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_9:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_1080P30_8bitBT656_4MUX;
#if 0
#if _SDR_MODE_4MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_10:
            handle->sif_bus = CUS_SENIF_BUS_BT1120;
            handle->pCus_sensor_init = pCus_init_NVP6158C_1080P30_16bitBT1120_1MUX;
#if 0
#if _SDR_MODE_1MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif

            break;
        case BT656_RES_11:
            handle->sif_bus = CUS_SENIF_BUS_BT1120;
            handle->pCus_sensor_init = pCus_init_NVP6158C_1080P30_16bitBT1120_2MUX;
#if 0
#if _SDR_MODE_2MUX_
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_SINGLE_UP;
#else
            handle->interface_attr.attr_bt656.bt656_clkedge = CUS_BT656_CLK_EDGE_DOUBLE;
#endif
#endif
            break;
        case BT656_RES_12:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_960h60i_8bitBT656_1MUX;

            break;
        case BT656_RES_13:
            handle->sif_bus = CUS_SENIF_BUS_BT656;
            handle->pCus_sensor_init = pCus_init_NVP6158C_960h50i_8bitBT656_1MUX;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
#if 0
    short Horiz_Inv = 0;
    short Verti_Inv = 0;
    short Orien_Mode = 0;

    Horiz_Inv &= 0x01;
    Verti_Inv &= 0X01;
    Orien_Mode = Horiz_Inv |(Verti_Inv << 2);
    switch(Orien_Mode)
    {
        case 0x00:
            *orit = CUS_ORIT_M0F0;
        break;
        case 0x01:
            *orit = CUS_ORIT_M1F0;
        break;
        case 0x02:
            *orit = CUS_ORIT_M0F1;
        break;
        case 0x03:
            *orit = CUS_ORIT_M1F1;
        break;
    }
#endif
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    nvp6158c_params *params = (nvp6158c_params *)handle->private_data;

    handle->orient = orit;
    params->orien_dirty = true;

    return SUCCESS;
}

#if 0

static int DoOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length = table_length/4;
    int i,j;

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = CUS_ORIT_M0F0;
            for(i = 0, j = 0; i < seg_length; i++, j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x00);
                //SensorReg_Write(0x304f,0x00);
            }
            break;
        case CUS_ORIT_M1F0:
            for(i = seg_length, j = 0; i < seg_length*2; i++, j++){
                handle->orient = CUS_ORIT_M1F0;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x01);
                //SensorReg_Write(0x304f,0x00);
            }
            break;
        case CUS_ORIT_M0F1:
            for(i = seg_length * 2, j = 0; i < seg_length*3; i++, j++){
                handle->orient = CUS_ORIT_M0F1;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x00);
                //SensorReg_Write(0x304f,0x01);
            }
            break;
        case CUS_ORIT_M1F1:
            for(i = seg_length * 3, j = 0; i < seg_length*4; i++, j++){
                handle->orient = CUS_ORIT_M1F1;
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                //SensorReg_Write(0x304e,0x01);
                //SensorReg_Write(0x304e,0x01);
            }
            break;
        default :
            handle->orient = CUS_ORIT_M0F0;
            break;
    }
    return SUCCESS;
}
#endif

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    return SUCCESS;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    //nvp6158c_params *params = (nvp6158c_params *)handle->private_data;

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

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
#if 0
    *min = 1;
    *max = 1000000/nvp6158c_bt656_linear[0].senout.min_fps;
#endif
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
#if 0
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;//3980*1024;
#endif
    return SUCCESS;
}

static int NVP6158C_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
#if 0
    info->max = 1000000000/nvp6158c_bt656_linear[0].senout.min_fps;
    info->min = (Preview_line_period * 9);
    info->step = Preview_line_period;
#endif
    return SUCCESS;
}

int nvp6158c_cus_camsensor_init_bt656(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    nvp6158c_params *params;
    int res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (nvp6158c_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"NVP6158C_BT656");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D8;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x34;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //300000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                    = Preview_MCLK_SPEED;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type              = SENSOR_ISP_TYPE;
    //handle->data_fmt              = SENSOR_DATAFMT;
    handle->sif_bus               = SENSOR_IFBUS_TYPE;
    handle->data_prec             = SENSOR_DATAPREC;
    handle->data_mode             = SENSOR_DATAMODE;
    handle->bayer_id              = SENSOR_BAYERID;
    handle->RGBIR_id              = SENSOR_RGBIRID;
    handle->orient                = SENSOR_ORIT;
    //handle->YC_ODER               = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;

    ////////////////////////////////////
    //    BT656 Config Begin          //
    ////////////////////////////////////

    handle->interface_attr.attr_bt656.bt656_total_ch       = SENSOR_BT656_CH_TOTAL_NUM;
    handle->interface_attr.attr_bt656.bt656_cur_ch         = 0;
    handle->interface_attr.attr_bt656.bt656_ch_det_en      = SENSOR_BT656_CH_DET_ENABLE;
    handle->interface_attr.attr_bt656.bt656_ch_det_sel     = SENSOR_BT656_CH_DET_SEL;
    handle->interface_attr.attr_bt656.bt656_bit_swap       = SENSOR_BT656_BIT_SWAP;
    handle->interface_attr.attr_bt656.bt656_8bit_mode      = SENSOR_BT656_8BIT_MODE;
    handle->interface_attr.attr_bt656.bt656_vsync_delay    = SENSOR_BT656_VSYNC_DELAY;
    handle->interface_attr.attr_bt656.bt656_hsync_inv      = SENSOR_HSYCN_INVERT;
    handle->interface_attr.attr_bt656.bt656_vsync_inv      = SENSOR_VSYCN_INVERT;
    handle->interface_attr.attr_bt656.bt656_clamp_en       = SENSOR_CLAMP_ENABLE;
    //handle->interface_attr.attr_bt656.bt656_format         = SENSOR_DATAFMT;
    //handle->interface_attr.attr_bt656.bt656_clkedge        = SENSOR_CLK_EDGE;
    //handle->interface_attr.attr_bt656.pCus_sensor_bt656_get_src_type = pCus_BT656_GetSrcType;

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
   //handle->video_res_supported.num_res = BT656_RES_END;
    for (res = 0; res < BT656_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = nvp6158c_bt656_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = nvp6158c_bt656_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = nvp6158c_bt656_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = nvp6158c_bt656_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = nvp6158c_bt656_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = nvp6158c_bt656_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = nvp6158c_bt656_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = nvp6158c_bt656_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, nvp6158c_bt656_linear[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init_NVP6158C_720P30_8bitBT656_1MUX;
    //handle->pCus_sensor_powerupseq      = pCus_powerupseq;
    handle->pCus_sensor_poweron         = pCus_poweron;
    handle->pCus_sensor_poweroff        = pCus_poweroff;
    handle->pCus_sensor_GetSensorID     = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum  = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes     = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes     = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien        = pCus_GetOrien;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode  = nvp6158c_SetPatternMode;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT;
    handle->ae_gain_ctrl_num           = 1;
    handle->ae_shutter_ctrl_num        = 1;
    handle->sat_mingain                = SENSOR_MIN_GAIN;  //calibration
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;
    handle->pCus_sensor_GetShutterInfo  = NVP6158C_GetShutterInfo;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(NVP6158C_BT656,
                            nvp6158c_cus_camsensor_init_bt656,
                            NULL,
                            NULL,
                            nvp6158c_params
                         );
