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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(OS05A10);

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
//#define SENSOR_MIPI_HDR_MODE (1) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

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
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,

#define SENSOR_MAX_GAIN     246//(15.5*15.9)                  // max sensor again, a-gain * conversion-gain*d-gain

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 16801//17814                           // MCLK=21.6 HTS/PCLK=3080 pixels/97.2MHZ=31.687us                              // 3126 for 25fps
#define vts_23fps  1984                                     // VTS for 20fps

#define Preview_WIDTH       2592//2688                    //resolution Width when preview
#define Preview_HEIGHT      1944//1520                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     8                      //slowest preview FPS

#define Cap_Max_line_number 1944//1520                   //maximum exposure line munber of sensor when capture

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

static int OS05A10_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int OS05A10_SetAEUSecs(ms_cus_sensor *handle, u32 us);

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
    } expo;
    struct {
        CUS_CAMSENSOR_ORIT new_setting;//the mirror/flip status set from user
        CUS_CAMSENSOR_ORIT cur; //current sensor setting
    }mirror_flip;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool mirror_dirty;
    bool dirty;
    I2C_ARRAY tVts_reg[4];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[4];
} os05a10_params;

// set sensor ID address and data,
const I2C_ARRAY Sensor_id_table[] =
{
    {0x300a, 0x53},      // {address of ID, ID },
    {0x300b, 0x05},      // {address of ID, ID },
    //{0x300c, 0x41},      // {address of ID, ID },
    // max 8 sets in this table
};

const I2C_ARRAY Sensor_init_table[] =
{
//@@ Res 2592X1944 4lane MIPI0512Mbps Linear12 30fps MCLK24M VTS1984
//;version = OS05A10_R1A_AM14
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x27},
    {0x0306, 0x00},
    {0x0307, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x031e, 0x0a},
    {0x0325, 0x48},
    {0x0328, 0x07},
    {0x300d, 0x11},
    {0x300e, 0x11},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x3018, 0xf0},
    {0x3028, 0xf0},
    {0x301e, 0x98},
    {0x3010, 0x04},
    {0x3011, 0x06},
    {0x3031, 0xa9},
    {0x3103, 0x48},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3501, 0x09},
    {0x3502, 0x2c},
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
    {0x3626, 0xff},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x3629, 0x6a},
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
    {0x3621, 0x81},
    {0x3634, 0x31},
    {0x3620, 0x00},
    {0x3622, 0x00},
    {0x362a, 0xd0},
    {0x362e, 0x8c},
    {0x362f, 0x98},
    {0x3630, 0xb0},
    {0x3631, 0xd7},
    {0x3701, 0x0f},
    {0x3737, 0x02},
    {0x3740, 0x18},
    {0x3741, 0x04},
    {0x373c, 0x0f},
    {0x373b, 0x02},
    {0x3705, 0x00},
    {0x3706, 0xa0},
    {0x370a, 0x01},
    {0x370b, 0xc8},
    {0x3709, 0x4a},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0e},
    {0x3760, 0x13},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x84},
    {0x3798, 0x01},
    {0x3799, 0x00},
    {0x3761, 0x02},
    {0x3762, 0x0d},
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
    {0x380c, 0x03},
    {0x380d, 0xf0},
    {0x380e, 0x09},
    {0x380f, 0x4c},
    {0x3813, 0x04},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x18},
    {0x3826, 0x00},
    {0x3827, 0x01},
    {0x3833, 0x00},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3843, 0x20},
    {0x382d, 0x08},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0x00},
    {0x4000, 0x78},
    {0x4001, 0x2b},
    {0x4004, 0x01},
    {0x4005, 0x00},
    {0x4028, 0x2f},
    {0x400a, 0x01},
    {0x4010, 0x12},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
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
    {0x4601, 0x10},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x480e, 0x00},
    {0x4813, 0x00},
    {0x4815, 0x2b},
    {0x486e, 0x36},
    {0x486f, 0x84},
    {0x4860, 0x00},
    {0x4861, 0xa0},
    {0x484b, 0x05},
    {0x4850, 0x00},
    {0x4851, 0xaa},
    {0x4852, 0xff},
    {0x4853, 0x8a},
    {0x4854, 0x08},
    {0x4855, 0x30},
    {0x4800, 0x60},
    {0x4837, 0x19},
    {0x484a, 0x3f},
    {0x5000, 0xc9},
    {0x5001, 0x43},
    {0x5002, 0x00},
    {0x5211, 0x03},
    {0x5291, 0x03},
    {0x520d, 0x0f},
    {0x520e, 0xfd},
    {0x520f, 0xa5},
    {0x5210, 0xa5},
    {0x528d, 0x0f},
    {0x528e, 0xfd},
    {0x528f, 0xa5},
    {0x5290, 0xa5},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x5182, 0x0f},
    {0x5183, 0xff},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xe9},
    {0x4d02, 0xba},
    {0x4d03, 0x66},
    {0x4d04, 0x46},
    {0x4d05, 0xa5},
    {0x3603, 0x3c},
    {0x3703, 0x26},
    {0x3709, 0x49},
    {0x3708, 0x2d},
    {0x3719, 0x1c},
    {0x371a, 0x06},
    {0x4000, 0x79},
    {0x0305, 0x20},
    {0x4837, 0x1f},
    {0x380c, 0x07},
    {0x380d, 0x17},
    {0x380e, 0x07},
    {0x380f, 0xc0},
    {0x3501, 0x05},
    {0x3502, 0xc0},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01}
#if 0
//@@ 2 2001 Res 2592x1944 4lane MIPI0624Mbps Linear12 30fps
    {0x0103, 0x01},
    {0x0303, 0x01},
    {0x0305, 0x27},
    {0x0306, 0x00},
    {0x0307, 0x00},
    {0x0308, 0x03},
    {0x0309, 0x04},
    {0x032a, 0x00},
    {0x031e, 0x0a},
    {0x0325, 0x48},
    {0x0328, 0x07},
    {0x300d, 0x11},
    {0x300e, 0x11},
    {0x300f, 0x11},
    {0x3010, 0x01},
    {0x3012, 0x41},
    {0x3016, 0xf0},
    {0x3018, 0xf0},
    {0x3028, 0xf0},
    {0x301e, 0x98},
    {0x3010, 0x04},
    {0x3011, 0x06},
    {0x3031, 0xa9},
    {0x3103, 0x48},
    {0x3104, 0x01},
    {0x3106, 0x10},
    {0x3400, 0x04},
    {0x3025, 0x03},
    {0x3425, 0x51},
    {0x3428, 0x01},
    {0x3406, 0x08},
    {0x3408, 0x03},
    {0x3501, 0x09},
    {0x3502, 0x2c},
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
    {0x3626, 0xff},
    {0x3605, 0x50},
    {0x3609, 0xdb},
    {0x3610, 0x69},
    {0x360c, 0x01},
    {0x3628, 0xa4},
    {0x3629, 0x6a},
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
    {0x3621, 0x81},
    {0x3634, 0x31},
    {0x3620, 0x00},
    {0x3622, 0x00},
    {0x362a, 0xd0},
    {0x362e, 0x8c},
    {0x362f, 0x98},
    {0x3630, 0xb0},
    {0x3631, 0xd7},
    {0x3701, 0x0f},
    {0x3737, 0x02},
    {0x3741, 0x04},
    {0x373c, 0x0f},
    {0x373b, 0x02},
    {0x3705, 0x00},
    {0x3706, 0xa0},
    {0x370a, 0x01},
    {0x370b, 0xc8},
    {0x3709, 0x4a},
    {0x3714, 0x21},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x375e, 0x0e},
    {0x3760, 0x13},
    {0x3776, 0x10},
    {0x3781, 0x02},
    {0x3782, 0x04},
    {0x3783, 0x02},
    {0x3784, 0x08},
    {0x3785, 0x08},
    {0x3788, 0x01},
    {0x3789, 0x01},
    {0x3797, 0x84},
    {0x3798, 0x01},
    {0x3799, 0x00},
    {0x3761, 0x02},
    {0x3762, 0x0d},
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
    {0x380c, 0x03},
    {0x380d, 0xf0},
    {0x380e, 0x09},
    {0x380f, 0x4c},
    {0x3813, 0x04},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x00},
    {0x3821, 0x04},
    {0x3823, 0x18},
    {0x3826, 0x00},
    {0x3827, 0x01},
    {0x3832, 0x02},
    {0x383c, 0x48},
    {0x383d, 0xff},
    {0x3843, 0x20},
    {0x382d, 0x08},
    {0x3d85, 0x0b},
    {0x3d84, 0x40},
    {0x3d8c, 0x63},
    {0x3d8d, 0x00},
    {0x4000, 0x78},
    {0x4001, 0x2b},
    {0x4005, 0x40},
    {0x4028, 0x2f},
    {0x400a, 0x01},
    {0x4010, 0x12},
    {0x4008, 0x02},
    {0x4009, 0x0d},
    {0x401a, 0x58},
    {0x4050, 0x00},
    {0x4051, 0x01},
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
    {0x4601, 0x10},
    {0x4603, 0x01},
    {0x4643, 0x00},
    {0x4640, 0x01},
    {0x4641, 0x04},
    {0x480e, 0x00},
    {0x4813, 0x00},
    {0x4815, 0x2b},
    {0x486e, 0x36},
    {0x486f, 0x84},
    {0x4860, 0x00},
    {0x4861, 0xa0},
    {0x484b, 0x05},
    {0x4850, 0x00},
    {0x4851, 0xaa},
    {0x4852, 0xff},
    {0x4853, 0x8a},
    {0x4854, 0x08},
    {0x4855, 0x30},
    {0x4800, 0x00},
    {0x4837, 0x19},
    {0x484a, 0x3f},
    {0x5000, 0xc9},
    {0x5001, 0x43},
    {0x5002, 0x00},
    {0x5211, 0x03},
    {0x5291, 0x03},
    {0x520d, 0x0f},
    {0x520e, 0xfd},
    {0x520f, 0xa5},
    {0x5210, 0xa5},
    {0x528d, 0x0f},
    {0x528e, 0xfd},
    {0x528f, 0xa5},
    {0x5290, 0xa5},
    {0x5004, 0x40},
    {0x5005, 0x00},
    {0x5180, 0x00},
    {0x5181, 0x10},
    {0x5182, 0x0f},
    {0x5183, 0xff},
    {0x580b, 0x03},
    {0x4d00, 0x03},
    {0x4d01, 0xe9},
    {0x4d02, 0xba},
    {0x4d03, 0x66},
    {0x4d04, 0x46},
    {0x4d05, 0xa5},
    {0x3603, 0x3c},
    {0x3703, 0x26},
    {0x3709, 0x49},
    {0x3708, 0x2d},
    {0x3719, 0x1c},
    {0x371a, 0x06},
    {0x4000, 0x79},
    {0x380c, 0x06},
    {0x380d, 0x04},
    {0x380e, 0x09},
    {0x380f, 0x21},
    {0x3501, 0x09},
    {0x3502, 0x19},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01},
    {0x0100, 0x01}
#endif
};


I2C_ARRAY TriggerStartTbl[] = {
    {0x0100,0x01},//normal mode
};

I2C_ARRAY PatternTbl[] = {
    {0x5081,0x00}, //colorbar pattern , bit 7 to enable
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
    {0x3820, 0x00},//M0F0
    {0x3821, 0x00},
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

const I2C_ARRAY mirr_flip_table[] =
{
    {0x3820, 0x00},//M0F0
    {0x3821, 0x00},

    {0x3820, 0x00},//M1F0
    {0x3821, 0x04},

    {0x3820, 0x24},//M0F1
    {0x3821, 0x00},

    {0x3820, 0x24},//M1F1
    {0x3821, 0x04},

};


typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const static I2C_ARRAY gain_reg[] = {
    {0x3508, 0x00},//long a-gain[13:8]
    {0x3509, 0x70},//long a-gain[7:0]
    {0x350A, 0x00},// d-gain[13:8]
    {0x350B, 0x00},// d-gain[7:0]
};

static int g_sensor_ae_min_gain = 1024;
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
    {0x3208, 0x00},//Group 0 hold start
    {0x3500, 0x00},//long exp[19,16]
    {0x3501, 0x02},//long exp[15,8]
    {0x3502, 0x00},//long exp[7,0]
};

const I2C_ARRAY vts_reg[] = {
    {0x380E, 0x7F&(vts_23fps>>8)},
    {0x380F, 0xFF&(vts_23fps)},
    {0x3208, 0x10},//Group 0 hold end
    // {0x320B, 0x00},//manual launch
    {0x3208, 0xa0},// Group delay launch
    //   {0x3208, 0x00},//quick launch group 0
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

/////////// function definition ///////////////////
#define SENSOR_NAME os05a10

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int OS05A10_SetFPS(ms_cus_sensor *handle, u32 fps);

/////////////////// sensor hardware dependent //////////////

static int OS05A10_poweron(ms_cus_sensor *handle, u32 idx)
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

static int OS05A10_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_UDELAY(30);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_UDELAY(30);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    Current_Mirror_Flip_Tbl[0].data = 0x00;
    Current_Mirror_Flip_Tbl[1].data = 0x00;
    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int OS05A10_GetSensorID(ms_cus_sensor *handle, u32 *id)
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
    SENSOR_DMSG("[%s]OS05A10 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
   // printf("OS05A10 Read sensor id, get 0x%x Success\n", (int)*id);

}
    return SUCCESS;
}

static int OS05A10_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;
    switch(mode)
    {
    case 1:
        PatternTbl[0].data = 0xA0; //enable
    break;
    case 0:
        PatternTbl[0].data &= 0x7F; //disable
    break;
    default:
        PatternTbl[0].data &= 0x7F; //disable
    break;
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }
    return SUCCESS;
}

static int OS05A10_init(ms_cus_sensor *handle)
{
	int i;
    os05a10_params *params = (os05a10_params *)handle->private_data;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
    {

        if(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS)
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

    for(i=0;i< ARRAY_SIZE(Current_Mirror_Flip_Tbl); i++)
    {
        if(SensorReg_Write(Current_Mirror_Flip_Tbl[i].reg,Current_Mirror_Flip_Tbl[i].data) != SUCCESS)
        {
            return FAIL;
        }

    }

    params->tVts_reg[0].data = ((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = ((params->expo.vts >> 0) & 0x00ff);

    return SUCCESS;
}

static int OS05A10_GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}

static int OS05A10_GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}
static int OS05A10_GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}
static int OS05A10_SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    os05a10_params *params = (os05a10_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = OS05A10_init;
            params->expo.vts = vts_23fps;
            params->expo.fps = 30;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int OS05A10_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    os05a10_params *params = (os05a10_params *)handle->private_data;
    return params->mirror_flip.cur;
}

static int DoMirror(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;

    os05a10_params *params = (os05a10_params *)handle->private_data;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    //SensorReg_Write(0x0100,0x00);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
	       //  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
		//  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
		 // handle->bayer_id=	CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
		 // handle->bayer_id=	CUS_BAYER_GR;
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
		//  handle->bayer_id=	CUS_BAYER_BG;
            break;
    }

    //SensorReg_Write(0x0100,0x01);
    params->mirror_flip.cur = orit;
    params->mirror_dirty = true;
    return SUCCESS;
}

static int OS05A10_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
    params->mirror_flip.new_setting = orit;
    DoMirror(handle,params->mirror_flip.new_setting);
    return SUCCESS;
}

static int OS05A10_GetFPS(ms_cus_sensor *handle)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_23fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_23fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int OS05A10_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=5 && fps <= 30){
        params->expo.fps = fps;
        params->expo.vts=  (vts_23fps*30)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true;
    }else if(fps>=5000 && fps <= 30000){
        params->expo.fps = fps;
        params->expo.vts=  (vts_23fps*30000)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    return SUCCESS;
}

static int OS05A10_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
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
static int OS05A10_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        if(params->mirror_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)Current_Mirror_Flip_Tbl, ARRAY_SIZE(Current_Mirror_Flip_Tbl));
            params->mirror_dirty = false;
        }
        break;
        case CUS_FRAME_ACTIVE:
        if(params->dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
            params->dirty = false;
        }
        break;
        default :
        break;
  }


  return SUCCESS;
}

static int OS05A10_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc = SUCCESS;
    u32 lines = 0;
    //rc = SensorRegArrayR((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
    os05a10_params *params = (os05a10_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<16;
    lines |= (u32)(params->tExpo_reg[2].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[3].data&0xff)<<0;
    lines >>= 4;

    *us = (lines*Preview_line_period);

    return rc;
}
#define MAX_A_GAIN 15872//(15.5*1024)
static int OS05A10_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u32 i,input_gain = 0;
    u16 gain16;

    if (gain < 1024) gain = 1024;
    else if (gain >= SENSOR_MAX_GAIN * 1024) gain = SENSOR_MAX_GAIN * 1024;

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

    params->dirty = true;
    //pr_info("[%s] set input gain/gain/AregH/AregL/DregH/DregL=%d/%d/0x%x/0x%x/0x%x/0x%x\n", __FUNCTION__, input_gain,gain,params->tGain_reg[0].data,params->tGain_reg[1].data,params->tGain_reg[2].data,params->tGain_reg[3].data);

    return SUCCESS;

}

static int OS05A10_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    os05a10_params *params = (os05a10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;
    if (lines >params->expo.vts-8)
        vts = lines +8;
    else
        vts=params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );
    // lines <<= 4;
    params->tExpo_reg[1].data = (lines>>16) & 0x000f;
    params->tExpo_reg[2].data = (lines>>8) & 0x00ff;
    params->tExpo_reg[3].data = (lines>>0) & 0x00ff;

    params->tVts_reg[0].data = (vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->dirty = true;

    return SUCCESS;
}

// Gain: 1x = 1024
static int OS05A10_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;

    if(params->tGain_reg[0].data==0)
	*gain=(u32)((params->tGain_reg[1].data/128)<<10);
    else if(params->tGain_reg[0].data==1)
       *gain=(u32)(((params->tGain_reg[1].data+8)/64)<<10);
    else if(params->tGain_reg[0].data==3)
       *gain=(u32)(((params->tGain_reg[1].data+12)/32)<<10);
    else if(params->tGain_reg[0].data==7)
       *gain=(u32)(((params->tGain_reg[1].data+8)/16)<<10);


    SENSOR_DMSG("[%s] get gain/reg0/reg1 (1024=1X)= %d/0x%x/0x%x\n", __FUNCTION__, *gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
    return SUCCESS;
}
static int OS05A10_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    os05a10_params *params = (os05a10_params *)handle->private_data;
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

static int OS05A10_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000/12;
    return SUCCESS;
}

static int OS05A10_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {

  *min = 1024;//1024*1.52;
  *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}
#if 0
static int OS05A10_setCaliData_mingain(ms_cus_sensor* handle,  u32 gain) {
    handle->sat_mingain=gain;
    return SUCCESS;
}
#endif
static int OS05A10_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }

    return SUCCESS;
}

static int OS05A10_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period*2;
    info->step = Preview_line_period;
    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    os05a10_params *params;
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
    params = (os05a10_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"OS05A10_MIPI");

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
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 0xA20;
    handle->video_res_supported.res[0].nOutputHeight= 0x798;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2592x1944@15fps");

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

    //Mirror / Flip
    params->mirror_flip.new_setting = SENSOR_ORIT;
    params->mirror_flip.cur = SENSOR_ORIT;

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = OS05A10_init    ;
    handle->pCus_sensor_poweron     = OS05A10_poweron ;
    handle->pCus_sensor_poweroff    = OS05A10_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = OS05A10_GetSensorID   ;
    handle->pCus_sensor_GetVideoResNum = OS05A10_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = OS05A10_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = OS05A10_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = OS05A10_SetVideoRes;
    handle->pCus_sensor_GetOrien          = OS05A10_GetOrien      ;
    handle->pCus_sensor_SetOrien          = OS05A10_SetOrien      ;
    handle->pCus_sensor_GetFPS          = OS05A10_GetFPS      ;
    handle->pCus_sensor_SetFPS          = OS05A10_SetFPS      ;
    handle->pCus_sensor_GetSensorCap    = OS05A10_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = OS05A10_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify = OS05A10_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = OS05A10_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = OS05A10_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = OS05A10_GetAEGain;
    handle->pCus_sensor_SetAEGain       = OS05A10_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = OS05A10_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= OS05A10_GetAEMinMaxUSecs;

    handle->pCus_sensor_GetShutterInfo = OS05A10_GetShutterInfo;


     //sensor calibration
    //handle->pCus_sensor_setCaliData_mingain=OS05A10_setCaliData_mingain;
    handle->pCus_sensor_SetAEGain_cal   = OS05A10_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=OS05A10_setCaliData_gain_linearity;

    params->expo.vts = vts_23fps;
    params->expo.fps = 30;
    params->mirror_dirty = false;
    params->dirty = false;
    return SUCCESS;
}


int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  OS05A10,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            os05a10_params
                         );
