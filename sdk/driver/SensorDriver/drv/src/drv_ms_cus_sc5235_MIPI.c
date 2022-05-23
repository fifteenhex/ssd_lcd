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
#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC5235_HDR);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (2)
#define SENSOR_MIPI_HDR_MODE (2) //0: Non-HDR mode. 1:Sony DOL mode. 2：CUS_HDR_MODE_DCG
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
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_HDR_DOL     CUS_DATAPRECISION_10

#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_MAXGAIN      ((1575*31)/100)
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_BG//CUS_BAYER_GR

#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     80                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define lane_number 2
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_27MHZ

u32 Preview_line_period;
u32 vts_30fps;
#define Preview_line_period_HDR_DOL 25253
#define vts_10fps_HDR_DOL 3960

//#define Preview_line_period 22222//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define vts_30fps  1500//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 1944                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       2560                    //resolution Width when preview
#define Preview_HEIGHT      1920                    //resolution Height when preview
#define Preview_MAX_FPS     20                     //fastest preview FPS
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

#define SENSOR_VSYNC_POL    CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEGain_2048x1536(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEGain_2592x1520(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEGain_2592x1944(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int g_sensor_ae_min_gain = 1024;
#define ENABLE_NR 1

CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};

typedef struct {
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
        u32 line;
        u32 max_short_exp;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_mf;
    bool reg_dirty;
} sc5235_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x52},
    {0x3108, 0x35},
};

I2C_ARRAY Sensor_init_table_2592x1944_20fps[] =
{
{0x0103, 0x01}, //soft reset
{0x0100, 0x00}, //stream stop

{0x3039, 0xa6},//bypass pll1
{0x3029, 0xa7},//bypass pll2

{0x3635, 0x4c},
{0x4500, 0x5d},
{0x3e01, 0xf9},
{0x3e02, 0xe0},
{0x3625, 0x0e},
{0x4509, 0x04},
{0x391d, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x1f},
{0x3e09, 0x1f},
{0x3334, 0x40},
{0x3621, 0xa8},

{0x3306, 0x30},
{0x3633, 0x65},
{0x3638, 0x84},
{0x3315, 0x02}, //rblue reverse

{0x3308, 0x10},
{0x330a, 0x01},
{0x330b, 0x20},

{0x3634, 0x86},
{0x3303, 0x10},
{0x331e, 0x0b},
{0x3320, 0x07},
{0x3309, 0x52},
{0x331f, 0x4d},
{0x3321, 0x07}, //new offset timing
{0x3367, 0x06}, //auto precharge

{0x336c, 0x01}, //tx even/odd exchange
{0x3907, 0x00},
{0x3908, 0xf9},
{0x4509, 0x10},

//fullwell
{0x3637, 0x0a},
{0x337f, 0x03}, //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x04}, //auto precharge
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x06},
{0x330e, 0x30},

{0x3366, 0x50}, // div_rst gap

{0x3633, 0x76},

{0x320c, 0x07},
{0x320d, 0x08},
{0x3e08, 0x03},
{0x3e09, 0x10},

{0x390a, 0x2c},
{0x3637, 0x12},

{0x3639, 0x08},
{0x3306, 0x40},

{0x3633, 0x86},
{0x3301, 0x20},

{0x3364, 0x15}, //[4] 1: open adit    real value in 3373
{0x336d, 0x40}, //[6] 1: az mode

{0x337f, 0x03}, //[1:0] 11:new auto precharge  330e in 3372    [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x03},
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x08},
{0x330e, 0x30},
{0x3366, 0x7c}, // div_rst gap

//test
{0x33b6, 0x07},  //gain0
{0x33b7, 0x17},  //gain1
{0x33b8, 0x20}, // <=1f
{0x33b9, 0x20}, //<=0x011f
{0x33ba, 0x44}, //>=0x1110
{0x3670, 0x08}, //[3]:3633 logic ctrl  real value in 3682
{0x367e, 0x08},  //gain0
{0x367f, 0x18},  //gain1
{0x3677, 0x86},  //<gain0
{0x3678, 0x86},  //>=gain0
{0x3679, 0x88},  //>=gain1
//fullwell
{0x3625, 0x0f},
{0x3636, 0x21},
{0x3638, 0x04},

//timing
{0x3320, 0x04},
{0x3321, 0x04},
{0x331e, 0x0d}, //should be target value - 1
{0x331f, 0x4d},
{0x3303, 0x10},
{0x3309, 0x50},

{0x3625, 0x0b},
{0x5000, 0x20},

//fullwell
{0x3637, 0x13},
{0x3908, 0xf9},

//fpn
{0x3635, 0x41},
{0x363b, 0x02},
{0x363c, 0x06},
{0x363d, 0x06},
{0x3038, 0x88},

//1015
{0x3679, 0xa8},  //cmp current
{0x363c, 0x07},
{0x363d, 0x07},

{0x3e02, 0xc0},

//1109
{0x3623, 0x10}, // ph1/ph2 delay

//1111
{0x3631, 0x88},
{0x3622, 0xf6},

{0x3301, 0x28},
{0x3630, 0x10},

//2592x4944 25fps   2lane 684Mbps
{0x320c, 0x05},     //hts=2736
{0x320d, 0x58},
{0x320e, 0x07},     //vts=2000
{0x320f, 0xd0},

{0x3039, 0xc4}, //684M MIPI speed 27/3*2*38
{0x303a, 0x1a},

{0x3029, 0xc4}, //342M cntclk  27/3*2*38/2
{0x302a, 0x1a},
{0x302b, 0x10},
{0x330a, 0x00},
{0x3306, 0x48},
{0x330b, 0xa0},

//1019
{0x4837, 0x1d}, //mipi prepare floor(2 x Tmipi_pclk or bitres x 2000/ mipi lane speed bps )  1114

{0x3f08, 0x04},

{0x3364, 0x05}, //[4] 1: open adit    real value in 3373
{0x3670, 0x00}, //[3]:3633 logic ctrl  real value in 3682

{0x3e01, 0x20},

{0x3303, 0x10},
{0x3304, 0x10},
{0x331e, 0x09},
{0x3320, 0x01},
{0x3326, 0x00},
{0x3309, 0x70},
{0x331f, 0x69},  //new offset timing

{0x3366, 0x78},

{0x3638, 0x18},
{0x3639, 0x09},
{0x3637, 0x10},

{0x3635, 0x47},

{0x3e01, 0xf9},
{0x3e02, 0xc0},


//1113
{0x3620, 0x28},

//1114
{0x330b, 0xaa},

//1114B
{0x3635, 0x4c},

//1115
{0x3235, 0x0f}, //group hold position 1115
{0x3236, 0x9C},

//sram write
{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0xa8},


{0x3301, 0x34},  //[2d,37]
{0x3630, 0x20},  //1114
{0x3633, 0x22},
{0x3622, 0xf6},
{0x363a, 0x83},

//4M 50fps 2lane 999Mpfs
{0x3039, 0xc3},  //99.9M sysclk 499.5M cntclk
{0x303a, 0x1b},
{0x3029, 0xc3},
{0x302a, 0x1b},
{0x302b, 0x10},
{0x4837, 0x14},
{0x5002, 0x00},

{0x4502, 0x10}, // [2] 1: rblue @ write falling edge
{0x3f02, 0x05}, //write to read gap
{0x3f06, 0x21}, //[4:0]read rise to valid rise gap


{0x330b, 0xd0},
{0x3306, 0x58},
{0x3309, 0x60},
{0x331f, 0x59},

{0x3208, 0x0a},  //2560x1440
{0x3209, 0x00},
{0x320a, 0x05},
{0x320b, 0xa0},

{0x3202, 0x00}, //addr xstart 240
{0x3203, 0xf0},
{0x3206, 0x06}, //addr yend  1687
{0x3207, 0x98},

{0x320c, 0x05},  //2700x1480
{0x320d, 0x46},
{0x320e, 0x05},
{0x320f, 0xc8},

{0x3e01, 0xb8},
{0x3e02, 0xc0},

{0x3235, 0x0b}, //group hold position 1115
{0x3236, 0x8C},

//sram write
{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0x9f},

//1226
{0x320d, 0x46},
{0x3303, 0x20},
{0x331e, 0x19},
{0x3614, 0x0c},
{0x3306, 0x70},
{0x330b, 0xb4},

{0x3638, 0x14},
{0x3306, 0x50},
{0x3309, 0x60},
{0x331f, 0x59},

{0x3630, 0x0a},

{0x331b, 0x83},
{0x3333, 0x20},

{0x3638, 0x18},
{0x3306, 0x58},
{0x330b, 0xbc},


//samp timing
{0x3359, 0x22},
{0x335c, 0x22},
{0x33af, 0x22},
{0x33b0, 0x22},
{0x3350, 0x22},
{0x33ae, 0x22},
{0x3332, 0x22},
{0x33b4, 0x22},

{0x330d, 0x30},

//20180209 dpc
{0x5000, 0x26},
{0x5002, 0x06},
{0x578b, 0x17},

//20180228
{0x5000, 0x00},

//20180305 dpc
{0x6000, 0x20},
{0x6002, 0x00},

//20180303
{0x5000, 0x20},
{0x5002, 0x00},
{0x3f05, 0x99},

//20180308   high gain FPN col 1.7V
{0x3624, 0x80},

//20180312
{0x3366, 0xc8},
{0x3621, 0xac},

//20180316
{0x3623, 0x08},

//20180319 BLC
{0x390a, 0x00},

//20180326
{0x3624, 0x00},

//20180327
{0x3624, 0x73},  //tt corner

//20180327
{0x3f04, 0x02}, //hts/2 - 3f08 - 0x04
{0x3f05, 0x9b},

{0x3306, 0x50},//[42,58]
{0x330b, 0xc0},//[b4,e0]

//20180410
{0x363d, 0x03},
{0x3635, 0x4d},
{0x363b, 0x02},

//20180419
{0x320c, 0x05},//hts=2700
{0x320d, 0x46},
{0x3f04, 0x02},
{0x3f05, 0x9b},  // hts / 2 - 3f08 - 0x04

{0x320e, 0x05}, //vts=1500
{0x320f, 0xdc},
{0x3235, 0x0b},
{0x3236, 0xb6}, // vts x 2 - 2
{0x3e01, 0xbb},
{0x3e02, 0x40},

{0x3039, 0xa6},//121.5MPCLK
{0x303a, 0x37},//607.5MVCO
{0x3029, 0x84},//486MCntCLK
{0x302a, 0x37},
{0x302b, 0x00},
{0x4837, 0x20},

//20180420
{0x5000, 0x06}, //dpc enable
{0x578b, 0x07}, //4 channel
{0x5780, 0x7f}, //auto blc setting
{0x57a0, 0x00}, //gain0 = 2x 0x0720 to 0x073f
{0x57a1, 0x72},
{0x57a2, 0x01}, //gain1 = 8x 0x1f20 to 0x1f3f
{0x57a3, 0xf2},
{0x5781, 0x06}, //white 1x
{0x5782, 0x04}, //2x
{0x5783, 0x02}, //8x
{0x5784, 0x01}, //128x
{0x5785, 0x16}, //black 1x
{0x5786, 0x12}, //2x
{0x5787, 0x08}, //8x
{0x5788, 0x02}, //128x

{0x3306, 0x40},//[26,70]20180419
{0x330b, 0xc0},//[80,100]20180419

//20180507  2568x1440
{0x3200, 0x00},
{0x3201, 0x0c},
{0x3202, 0x00},
{0x3203, 0xfc},
{0x3204, 0x0a},
{0x3205, 0x23},
{0x3206, 0x06},
{0x3207, 0xab},
{0x3208, 0x0a},
{0x3209, 0x08},
{0x320a, 0x05},
{0x320b, 0xa0},

{0x3301, 0x20},  //[e,30] 20180419
{0x3630, 0x20},  //1114
{0x3633, 0x23},//20180419
{0x3622, 0xf6},
{0x363a, 0x83},

//20180511
{0x3614, 0x00},

//20180523
{0x3624, 0x47},
{0x3621, 0xac},

//0627
{0x338f, 0x40},
{0x5002, 0x06},
{0x3907, 0x00},
{0x3908, 0xf9},

//20180629
{0x391c, 0x9f},

//20180703
{0x3632, 0x18},  //pump driver current

//20180719
{0x3802, 0x01},

//20180728
{0x6000, 0x20},
{0x6002, 0x00},

//20180731
{0x3209, 0x00},

//20180801  2560x1440
{0x3201, 0x10},
{0x3205, 0x1f},
{0x3314, 0x14},
{0x3211, 0x08},

//20180810
{0x3637, 0x20},

//20180810
{0x3306, 0x50},//20180810[33,71]    330b=b8     20180816[2d,69]
{0x330b, 0xb8},//20180810[90,d9]    3306=50     20180816[90,d1]

//20180813 row fpn fix -- Gene
{0x3638, 0x1a},//ramp offset, increase 0x3303 & 0x3309 margin
{0x3029, 0xa7},
{0x302a, 0x34},
{0x302b, 0x10},
{0x302d, 0x00},
{0x3303, 0x28},//[,32]   -->    0x3309[,6a]
{0x331f, 0x51},

//20180816 digital blc
{0x3e26, 0x20},
{0x391e, 0x01},
{0x391f, 0xc0},

//20180816 increase preprecharge margin
{0x3318, 0x42},
{0x3319, 0x82},
{0x3307, 0x14},
{0x3339, 0x44},
{0x330f, 0x04},
{0x3310, 0x51},
{0x330e, 0x20},

//20180823
{0x3038, 0x44},//ppump & npump div

//20180824
{0x4809, 0x01},//mipi first frame, lp status

//20180829
{0x5783, 0x00}, //8x
{0x5784, 0x00}, //128x

//20181120 2592x1944
{0x3200, 0x00},
{0x3201, 0x04},
{0x3202, 0x00},
{0x3203, 0x00},
{0x3204, 0x0a},
{0x3205, 0x2b},
{0x3206, 0x07},
{0x3207, 0x9f},
{0x3208, 0x0a},
{0x3209, 0x20},
{0x320a, 0x07},
{0x320b, 0x98},
{0x3211, 0x04},
{0x3213, 0x04},

{0x320c, 0x05},//hts=1500*2
{0x320d, 0xdc},
{0x320e, 0x07}, //vts=2025
{0x320f, 0xe9},
{0x3f04, 0x02},
{0x3f05, 0xe6},  // hts / 2 - 3f08 - 0x04
{0x3235, 0x0f},
{0x3236, 0xd0}, // vts x 2 - 2

//init
{0x3e00, 0x00},//max exposure = vts*2-8; min exposure = 3;
{0x3e01, 0xfc},
{0x3e02, 0xa0},
{0x3e03, 0x0b},
{0x3e06, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x03},
{0x3e09, 0x20},
{0x3301, 0x1c},  //[12,1c]  20180727[14,20]
{0x3630, 0x30},  //20180706
{0x3633, 0x23},//20180419
{0x3622, 0xf6},
{0x363a, 0x83},

//20180816 when isp not wdr mode need open, blc
{0x3905, 0x98},
{0x3e1e, 0x30},

{0x3039, 0x26},//enable pll1
{0x3029, 0x27},//enable pll2

{0x0100, 0x01}, //stream on
};

I2C_ARRAY Sensor_init_table_2560x1920_20fps[] =
{
{0x0103, 0x01}, //soft reset
{0x0100, 0x00}, //stream stop

{0x3039, 0x84},//bypass pll1
{0x3029, 0xb7},//bypass pll2
{0x3635, 0x4c},
{0x4500, 0x5d},
{0x3e01, 0xf9},
{0x3e02, 0xe0},
{0x3625, 0x0e},
{0x4509, 0x04},
{0x391d, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x1f},
{0x3e09, 0x1f},

{0x3334, 0x40}, //cmp leakage need debug
{0x3621, 0xa8},
{0x3306, 0x30},
{0x3633, 0x65},
{0x3638, 0x84},
{0x3315, 0x02}, //rblue reverse
{0x3308, 0x10},
{0x330a, 0x01},
{0x330b, 0x20},
{0x3634, 0x86},
{0x3303, 0x10},
{0x331e, 0x0b},
{0x3320, 0x07},
{0x3309, 0x52},
{0x331f, 0x4d},
{0x3321, 0x07}, //new offset timing
{0x3367, 0x06}, //auto precharge
{0x336c, 0x01}, //tx even/odd exchange
{0x3907, 0x00},
{0x3908, 0xf9},
{0x4509, 0x10},
{0x3637, 0x0a},
{0x337f, 0x03}, //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x04}, //auto precharge
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x06},
{0x330e, 0x30},
{0x3366, 0x50}, // div_rst gap
{0x3633, 0x76},
{0x320c, 0x07},
{0x320d, 0x08},
{0x3e08, 0x03},
{0x3e09, 0x10},
{0x390a, 0x2c},
{0x3637, 0x12},
{0x3639, 0x08},
{0x3306, 0x40},
{0x3633, 0x86},
{0x3301, 0x20},
{0x3364, 0x15}, //[4] 1: open adit    real value in 3373
{0x336d, 0x40}, //[6] 1: az mode
{0x337f, 0x03}, //[1:0] 11:new auto precharge  330e in 3372    [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x03},
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x08},
{0x330e, 0x30},
{0x3366, 0x7c}, // div_rst gap

{0x33b6, 0x07},  //gain0
{0x33b7, 0x17},  //gain1
{0x33b8, 0x20},  // <=1f
{0x33b9, 0x20},  //<=0x011f
{0x33ba, 0x44},  //>=0x1110

{0x3670, 0x08},  //[3]:3633 logic ctrl  real value in 3682
{0x367e, 0x08},  //gain0
{0x367f, 0x18},  //gain1
{0x3677, 0x86},  //<gain0
{0x3678, 0x86},  //>=gain0
{0x3679, 0x88},  //>=gain1
{0x3625, 0x0f},
{0x3636, 0x21},
{0x3638, 0x04},
{0x3320, 0x04},
{0x3321, 0x04},
{0x331e, 0x0d}, //should be target value - 1
{0x331f, 0x4d},
{0x3303, 0x10},
{0x3309, 0x50},

{0x3625, 0x0b},
{0x5000, 0x20},
{0x3637, 0x13},
{0x3908, 0xf9},
{0x3635, 0x41},
{0x363b, 0x02},
{0x363c, 0x06},
{0x363d, 0x06},
{0x3038, 0x88},
{0x3679, 0xa8},  //cmp current
{0x363c, 0x07},
{0x363d, 0x07},

{0x3e02, 0xc0},
{0x3623, 0x10}, // ph1/ph2 delay
{0x3631, 0x88},
{0x3622, 0xf6},
{0x3301, 0x28},
{0x3630, 0x10},
{0x320c, 0x05},     //hts=2700
{0x320d, 0x58},
{0x320e, 0x07},     //vts=2000
{0x320f, 0xd0},
{0x3039, 0xc4}, //684M MIPI speed 27/3*2*38
{0x303a, 0x1a},
{0x3029, 0xc4}, //342M cntclk  27/3*2*38/2
{0x302a, 0x1a},
{0x302b, 0x10},
{0x330a, 0x00},
{0x3306, 0x48},
{0x330b, 0xa0},
{0x4837, 0x21}, //mipi prepare floor(2 x Tmipi_pclk or bitres x 2000/ mipi lane speed bps )
{0x3f08, 0x04},
{0x3364, 0x05}, //[4] 1: open adit    real value in 3373
{0x3670, 0x00}, //[3]:3633 logic ctrl  real value in 3682
{0x3e01, 0x20},
{0x3303, 0x10},
{0x3304, 0x10},
{0x331e, 0x09},
{0x3320, 0x01},
{0x3326, 0x00},
{0x3309, 0x70},
{0x331f, 0x69},  //new offset timing
{0x3366, 0x78},
{0x3638, 0x18},
{0x3639, 0x09},
{0x3637, 0x10},
{0x3635, 0x47},
{0x3e01, 0xf7},
{0x3e02, 0x40},
{0x3620, 0x28},
{0x3039, 0x84},
{0x303a, 0x35},
{0x3029, 0xb3},
{0x302a, 0x35},
{0x302b, 0x00},
{0x330b, 0x90},
{0x3306, 0x40},
{0x3208, 0x0a}, //2560
{0x3209, 0x00},
{0x320c, 0x05}, //hts=3000
{0x320d, 0xdc},
{0x320e, 0x07}, //vts=1980
{0x320f, 0xbc},
{0x330b, 0x98},
{0x3635, 0x4c},
{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0xea},
{0x3802, 0x01},
{0x5000, 0x06},
{0x5002, 0x06},
{0x578b, 0x07},
{0x331b, 0x83},
{0x3333, 0x20},
{0x3621, 0xac},
{0x390a, 0x00},
{0x3624, 0x07},
{0x3f04, 0x02},//hts/2 - 3f08 - 0x04
{0x3f05, 0xe6},
{0x363d, 0x03},
{0x3213, 0x02},
{0x3635, 0x4d},
{0x363b, 0x02},
{0x3208, 0x0a},//2592
{0x3209, 0x20},
{0x3200, 0x00},//xstar 4
{0x3201, 0x04},
{0x3204, 0x0a},//xend
{0x3205, 0x27},
{0x3211, 0x04}, // x shift 4
{0x5000, 0x06}, //dpc enable
{0x578b, 0x07}, //4 channel
{0x5780, 0x7f}, //auto blc setting
{0x57a0, 0x00}, //gain0 = 2x 0x0720 to 0x073f
{0x57a1, 0x72},
{0x57a2, 0x01}, //gain1 = 8x 0x1f20 to 0x1f3f
{0x57a3, 0xf2},
{0x5781, 0x06}, //white 1x
{0x5782, 0x04}, //2x
{0x5783, 0x02}, //8x
{0x5784, 0x01}, //128x
{0x5785, 0x16}, //black 1x
{0x5786, 0x12}, //2x
{0x5787, 0x08}, //8x
{0x5788, 0x02}, //128x
{0x3624, 0x47},
{0x3621, 0xac},
{0x338f, 0x40},
{0x3306, 0x50},//20180816[36,62]    0x330b=0xb8
{0x330b, 0xb8},//20180816[a6,dc]    0x3306=0x50
{0x3907, 0x00},
{0x3908, 0xf9},
{0x391c, 0x9f},
{0x3632, 0x18},  //pump driver current
{0x6000, 0x20},
{0x6002, 0x00},
{0x3314, 0x04},
{0x3205, 0x2b},
{0x3200, 0x00},
{0x3201, 0x10},
{0x3202, 0x00},
{0x3203, 0x0c},
{0x3204, 0x0a},
{0x3205, 0x1f},
{0x3206, 0x07},
{0x3207, 0x9b},
{0x3208, 0x0a},
{0x3209, 0x00},
{0x320a, 0x07},
{0x320b, 0x80},
{0x3211, 0x08},
{0x3213, 0x08},
{0x3314, 0x14},
{0x330e, 0x20},
{0x3637, 0x20},
{0x3303, 0x28}, //[,34] 20180816
{0x331e, 0x19},
{0x331f, 0x61},
{0x3309, 0x70}, //[,7c] 20180816
{0x3029, 0xb7},
{0x302b, 0x10},
{0x302d, 0x00},
{0x3e26, 0x20},
{0x391e, 0x01},
{0x391f, 0xc0},
{0x3038, 0x44},//ppump & npump div
{0x4809, 0x01},//mipi first frame, lp status
{0x5783, 0x00}, //8x
{0x5784, 0x00}, //128x

{0x3e00, 0x00},//max exposure = vts*2-8;    min exposure = 3;
{0x3e01, 0xf7},
{0x3e02, 0x00},
{0x3e03, 0x0b},
{0x3e06, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x03},
{0x3e09, 0x20},
{0x3301, 0x1c},  //[12,1c] 20180727[18,20]
{0x3630, 0x30},  //20180703
{0x3633, 0x23},//20180703
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3905, 0x98},
{0x3e1e, 0x30},
{0x3039, 0x04},//enable pll1
{0x3029, 0x37},//enable pll2

{0x0100, 0x01}, //stream on
};

I2C_ARRAY Sensor_init_table_2592x1520_25fps[] =
{
{0x0103,0x01}, //soft reset
{0x0100,0x00}, //stream stop

{0x3039,0xa6},//bypass pll1
{0x3029,0xa7},//bypass pll2
{0x3635,0x4c},
{0x4500,0x5d},
{0x3e01,0xf9},
{0x3e02,0xe0},
{0x3625,0x0e},
{0x4509,0x04},
{0x391d,0x00},
{0x3e07,0x80},
{0x3e08,0x1f},
{0x3e09,0x1f},
{0x3334,0x40}, //cmp leakage need debug
{0x3621,0xa8},
{0x3306,0x30},
{0x3633,0x65},
{0x3638,0x84},
{0x3315,0x02}, //rblue reverse
{0x3308,0x10},
{0x330a,0x01},
{0x330b,0x20},
{0x3634,0x86},
{0x3303,0x10},
{0x331e,0x0b},
{0x3320,0x07},
{0x3309,0x52},
{0x331f,0x4d},
{0x3321,0x07}, //new offset timing
{0x3367,0x06}, //auto precharge
{0x336c,0x01}, //tx even/odd exchange

{0x3907,0x00},
{0x3908,0xf9},
{0x4509,0x10},

{0x3637,0x0a},
{0x337f,0x03},
{0x3368,0x04}, //auto precharge
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x06},
{0x330e,0x30},
{0x3366,0x50}, // div_rst gap
{0x3633,0x76},
{0x320c,0x07},
{0x320d,0x08},
{0x3e08,0x03},
{0x3e09,0x10},

{0x390a,0x2c},
{0x3637,0x12},
{0x3639,0x08},
{0x3306,0x40},

{0x3633,0x86},
{0x3301,0x20},

{0x3364,0x15}, //[4] 1: open adit    real value in 3373
{0x336d,0x40}, //[6] 1: az mode

{0x337f,0x03}, //[1:0] 11:new auto precharge  330e in 3372    [7:6] 11: close div_rst 00:open div_rst
{0x3368,0x03},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x30},
{0x3366,0x7c}, // div_rst gap
{0x33b6,0x07},  //gain0
{0x33b7,0x17},    //gain1
{0x33b8,0x20}, // <=1f
{0x33b9,0x20}, //<=0x011f
{0x33ba,0x44}, //>=0x1110

{0x3670,0x08} , //[3]:3633 logic ctrl  real value in 3682
{0x367e,0x08},  //gain0
{0x367f,0x18},  //gain1
{0x3677,0x86},  //<gain0
{0x3678,0x86},  //>=gain0
{0x3679,0x88},  //>=gain1

{0x3625,0x0f},
{0x3636,0x21},
{0x3638,0x04},

{0x3320,0x04},
{0x3321,0x04},
{0x331e,0x0d}, //should be target value - 1
{0x331f,0x4d},
{0x3303,0x10},
{0x3309,0x50},
{0x3625,0x0b},

{0x3637,0x13},
{0x3908,0xf9},

{0x3635,0x41},
{0x363b,0x02},
{0x363c,0x06},
{0x363d,0x06},
{0x3038,0x88},

{0x3679,0xa8},  //cmp current
{0x363c,0x07},
{0x363d,0x07},
{0x3e02,0xc0},
{0x3623,0x10}, // ph1/ph2 delay
{0x3631,0x88},
{0x3622,0xf6},
{0x3301,0x28},
{0x3630,0x10},

{0x320c,0x05},     //hts=2736
{0x320d,0x58},
{0x320e,0x07},     //vts=2000
{0x320f,0xd0},
{0x3039,0xc4}, //684M MIPI speed 27/3*2*38
{0x303a,0x1a},

{0x3029,0xc4}, //342M cntclk  27/3*2*38/2
{0x302a,0x1a},
{0x302b,0x10},
{0x330a,0x00},
{0x3306,0x48},
{0x330b,0xa0},

{0x4837,0x1d},
{0x3f08,0x04},
{0x3364,0x05}, //[4] 1: open adit    real value in 3373
{0x3670,0x00} , //[3]:3633 logic ctrl  real value in 3682
{0x3e01,0x20},
{0x3303,0x10},
{0x3304,0x10},
{0x331e,0x09},
{0x3320,0x01},
{0x3326,0x00},
{0x3309,0x70},
{0x331f,0x69},  //new offset timing
{0x3366,0x78},
{0x3638,0x18},
{0x3639,0x09},
{0x3637,0x10},
{0x3635,0x47},
{0x3e01,0xf9},
{0x3e02,0xc0},
{0x3620,0x28},
{0x330b,0xaa},
{0x3635,0x4c},
{0x3235,0x0f}, //group hold position 1115
{0x3236,0x9C},

{0x3f00,0x0d}, //[2]   hts/2-4
{0x3f04,0x02},
{0x3f05,0xa8},
{0x3301,0x34},  //[2d,37]
{0x3630,0x20},  //1114
{0x3633,0x22},
{0x3622,0xf6},
{0x363a,0x83},

{0x3039,0xc3},
{0x303a,0x1b},
{0x3029,0xc3},
{0x302a,0x1b},
{0x302b,0x10},
{0x4837,0x14},
{0x5002,0x00},
{0x4502,0x10},
{0x3f02,0x05},
{0x3f06,0x21},
{0x330b,0xd0},
{0x3306,0x58},
{0x3309,0x60},
{0x331f,0x59},
{0x3208,0x0a},
{0x3209,0x00},
{0x320a,0x05},
{0x320b,0xa0},
{0x3202,0x00},
{0x3203,0xf0},
{0x3206,0x06}, //addr yend  1687
{0x3207,0x98},
{0x320c,0x05},  //2700x1480
{0x320d,0x46},
{0x320e,0x05},
{0x320f,0xc8},
{0x3e01,0xb8},
{0x3e02,0xc0},
{0x3235,0x0b}, //group hold position 1115
{0x3236,0x8C},

{0x3f00,0x0d}, //[2]   hts/2-4
{0x3f04,0x02},
{0x3f05,0x9f},

{0x320d,0x46},
{0x3303,0x20},
{0x331e,0x19},
{0x3614,0x0c},
{0x3306,0x70},
{0x330b,0xb4},
{0x3638,0x14},
{0x3306,0x50},
{0x3309,0x60},
{0x331f,0x59},

{0x3630,0x0a},
{0x331b,0x83},
{0x3333,0x20},
{0x3638,0x18},
{0x3306,0x58},
{0x330b,0xbc},

{0x3359,0x22},
{0x335c,0x22},
{0x33af,0x22},
{0x33b0,0x22},
{0x3350,0x22},
{0x33ae,0x22},
{0x3332,0x22},
{0x33b4,0x22},
{0x330d,0x30},

{0x5000,0x26},
{0x5002,0x06},
{0x578b,0x17},
{0x5000,0x00},
{0x6000,0x20},
{0x6002,0x00},
{0x5000,0x20},
{0x5002,0x00},
{0x3f05,0x99},

{0x3624,0x80},
{0x3366,0xc8},
{0x3621,0xac},

{0x3623,0x08},
{0x390a,0x00},
{0x3624,0x00},
{0x3624,0x73},  //tt corner
{0x3f04,0x02}, //hts/2 - 3f08 - 0x04
{0x3f05,0x9b},
{0x3306,0x50},//[42,58]
{0x330b,0xc0},//[b4,e0]
{0x363d,0x03},
{0x3635,0x4d},
{0x363b,0x02},

{0x320c,0x05},//hts=2700
{0x320d,0x46},
{0x3f04,0x02},
{0x3f05,0x9b},  // hts / 2 - 3f08 - 0x04

{0x320e,0x05}, //vts=1500
{0x320f,0xdc},
{0x3235,0x0b},
{0x3236,0xb6}, // vts x 2 - 2
{0x3e01,0xbb},
{0x3e02,0x40},

{0x3039,0xa6},//121.5MPCLK
{0x303a,0x37},//607.5MVCO
{0x3029,0x84},//486MCntCLK
{0x302a,0x37},
{0x302b,0x00},
{0x4837,0x20},

{0x5000,0x06}, //dpc enable
{0x578b,0x07}, //4 channel
{0x5780,0x7f}, //auto blc setting
{0x57a0,0x00}, //gain0 = 2x 0x0720 to 0x073f
{0x57a1,0x72},
{0x57a2,0x01}, //gain1 = 8x 0x1f20 to 0x1f3f
{0x57a3,0xf2},
{0x5781,0x06}, //white 1x
{0x5782,0x04}, //2x
{0x5783,0x02}, //8x
{0x5784,0x01}, //128x
{0x5785,0x16}, //black 1x
{0x5786,0x12}, //2x
{0x5787,0x08}, //8x
{0x5788,0x02}, //128x

{0x3306,0x40},//[26,70]20180419
{0x330b,0xc0},//[80,100]20180419

{0x3200,0x00},
{0x3201,0x0c},
{0x3202,0x00},
{0x3203,0xfc},
{0x3204,0x0a},
{0x3205,0x23},
{0x3206,0x06},
{0x3207,0xab},
{0x3208,0x0a},
{0x3209,0x08},
{0x320a,0x05},
{0x320b,0xa0},
{0x3301,0x20},
{0x3630,0x20},
{0x3633,0x23},
{0x3622,0xf6},
{0x363a,0x83},
{0x3614,0x00},
{0x3624,0x47},
{0x3621,0xac},
{0x338f,0x40},
{0x5002,0x06},
{0x3907,0x00},
{0x3908,0xf9},
{0x391c,0x9f},
{0x3632,0x18},  //pump driver current
{0x3802,0x01},
{0x6000,0x20},
{0x6002,0x00},
{0x3209,0x00},
{0x3201,0x10},
{0x3205,0x1f},
{0x3314,0x14},
{0x3211,0x08},
{0x3637,0x20},
{0x3306,0x50},
{0x330b,0xb8},

{0x3638,0x1a},
{0x3029,0xa7},
{0x302a,0x34},
{0x302b,0x10},
{0x302d,0x00},
{0x3303,0x28},
{0x331f,0x51},

{0x3e26,0x20},
{0x391e,0x01},
{0x391f,0xc0},

{0x3318,0x42},
{0x3319,0x82},
{0x3307,0x14},
{0x3339,0x44},
{0x330f,0x04},
{0x3310,0x51},
{0x330e,0x20},
{0x3038,0x44},//ppump & npump div
{0x4809,0x01},//mipi first frame, lp status
{0x5783,0x00}, //8x
{0x5784,0x00}, //128x
{0x3200,0x00},
{0x3201,0x04},
{0x3202,0x00},
{0x3203,0xd8},
{0x3204,0x0a},
{0x3205,0x2b},
{0x3206,0x06},
{0x3207,0xcf},
{0x3208,0x0a},
{0x3209,0x20},
{0x320a,0x05},
{0x320b,0xf0},
{0x320c,0x05},//3000
{0x320d,0xdc},
{0x320e,0x06},//1620
{0x320f,0x54},
{0x3211,0x04},
{0x3213,0x04},
{0x3e00,0x00},//max exposure = vts*2-8; min exposure = 3;
{0x3e01,0xca},
{0x3e02,0x00},
{0x3e03,0x0b},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x20},
{0x3301,0x1c},
{0x3630,0x30},
{0x3633,0x23},
{0x3622,0xf6},
{0x363a,0x83},

{0x3905,0x98},
{0x3e1e,0x30},
{0x3039,0x26},//enable pll1
{0x3029,0x27},//enable pll2

{0x0100,0x01}, //stream on
};

I2C_ARRAY Sensor_init_table_2560x1440_30fps[] =
{
{0x0103,0x01},
{0x0100,0x00},
{0x4500,0x5d},
{0x391d,0x00},
{0x3334,0x40},
{0x3315,0x02},
{0x3308,0x10},
{0x3634,0x86},
{0x336c,0x01},
{0x4509,0x10},
{0x336d,0x40},
{0x337f,0x03},
{0x3368,0x03},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x33b6,0x07},
{0x33b7,0x17},
{0x33b8,0x20},
{0x33b9,0x20},
{0x33ba,0x44},
{0x367e,0x08},
{0x367f,0x18},
{0x3677,0x86},
{0x3678,0x86},
{0x3636,0x21},
{0x3321,0x04},
{0x3625,0x0b},
{0x3679,0xa8},
{0x363c,0x07},
{0x3631,0x88},
{0x330a,0x00},
{0x3f08,0x04},
{0x3364,0x05},
{0x3670,0x00},
{0x3304,0x10},
{0x3320,0x01},
{0x3326,0x00},
{0x3639,0x09},
{0x3620,0x28},
{0x4502,0x10},
{0x3f02,0x05},
{0x3f06,0x21},
{0x3f00,0x0d},
{0x331e,0x19},
{0x3309,0x60},
{0x331b,0x83},
{0x3333,0x20},
{0x3359,0x22},
{0x335c,0x22},
{0x33af,0x22},
{0x33b0,0x22},
{0x3350,0x22},
{0x33ae,0x22},
{0x3332,0x22},
{0x33b4,0x22},
{0x330d,0x30},
{0x3366,0xc8},
{0x3623,0x08},
{0x390a,0x00},
{0x363d,0x03},
{0x3635,0x4d},
{0x363b,0x02},
{0x320c,0x05},
{0x320d,0x46},
{0x3f04,0x02},
{0x3f05,0x9b},
{0x320e,0x05},
{0x320f,0xdc},
{0x3235,0x0b},
{0x3236,0xb6},
{0x3039,0x26},
{0x303a,0x37},
{0x4837,0x20},
{0x5000,0x06},
{0x578b,0x07},
{0x5780,0x7f},
{0x57a0,0x00},
{0x57a1,0x72},
{0x57a2,0x01},
{0x57a3,0xf2},
{0x5781,0x06},
{0x5782,0x04},
{0x5785,0x16},
{0x5786,0x12},
{0x5787,0x08},
{0x5788,0x02},
{0x3200,0x00},
{0x3202,0x00},
{0x3203,0xfc},
{0x3204,0x0a},
{0x3206,0x06},
{0x3207,0xab},
{0x3208,0x0a},
{0x320a,0x05},
{0x320b,0xa0},
{0x3614,0x00},
{0x3624,0x47},
{0x3621,0xac},
{0x338f,0x40},
{0x5002,0x06},
{0x3907,0x00},
{0x3908,0xf9},
{0x391c,0x9f},
{0x3632,0x18},
{0x3802,0x01},
{0x6000,0x20},
{0x6002,0x00},
{0x3209,0x00},
{0x3201,0x10},
{0x3205,0x1f},
{0x3314,0x14},
{0x3211,0x08},
{0x3637,0x20},
{0x3306,0x50},
{0x330b,0xb8},
{0x3638,0x1a},
{0x3029,0x27},
{0x302a,0x34},
{0x302b,0x10},
{0x302d,0x00},
{0x3303,0x28},
{0x331f,0x51},
{0x3e26,0x20},
{0x391e,0x01},
{0x391f,0xc0},
{0x3318,0x42},
{0x3319,0x82},
{0x3307,0x14},
{0x3339,0x44},
{0x330f,0x04},
{0x3310,0x51},
{0x330e,0x20},
{0x3038,0x44},
{0x4809,0x01},
{0x5783,0x00},
{0x5784,0x00},
{0x3e00,0x00},
{0x3e01,0xbb},
{0x3e02,0x00},
{0x3e03,0x0b},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x20},
{0x3301,0x1c},
{0x3630,0x30},
{0x3633,0x23},
{0x3622,0xf6},
{0x363a,0x83},
{0x3905,0x98},
{0x3e1e,0x30},
{0x0100,0x01},
};

I2C_ARRAY Sensor_init_table_2048x1536_30fps[] =
{
{0x0103, 0x01}, //soft reset
{0x0100, 0x00}, //stream stop

{0x3039, 0xc6},//bypass pll1
{0x3029, 0xc0},//bypass pll2
{0x3635, 0x4c},
{0x4500, 0x5d},
{0x3e01, 0xf9},
{0x3e02, 0xe0},
{0x3625, 0x0e},

{0x4509, 0x04},
{0x391d, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x1f},
{0x3e09, 0x1f},

{0x3334, 0x40},
{0x3621, 0xa8},

{0x3306, 0x30},
{0x3633, 0x65},
{0x3638, 0x84},
{0x3315, 0x02},
{0x3308, 0x10},
{0x330a, 0x01},
{0x330b, 0x20},
{0x3634, 0x86},
{0x3303, 0x10},
{0x331e, 0x0b},
{0x3320, 0x07},
{0x3309, 0x52},
{0x331f, 0x4d},
{0x3321, 0x07},
{0x3367, 0x06},
{0x336c, 0x01},
{0x3907, 0x00},
{0x3908, 0x40},
{0x4509, 0x10},

{0x3637, 0x0a},
{0x337f, 0x03},
{0x3368, 0x04},
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x06},
{0x330e, 0x30},

{0x3366, 0x50},

{0x3633, 0x76},
{0x320c, 0x07},
{0x320d, 0x08},
{0x3e08, 0x03},
{0x3e09, 0x10},
{0x390a, 0x2c},
{0x3637, 0x12},
{0x3639, 0x08},
{0x3306, 0x40},
{0x3633, 0x86},
{0x3301, 0x20},
{0x3364, 0x15}, //[4] 1: open adit    real value in 3373
{0x336d, 0x40}, //[6] 1: az mode
{0x337f, 0x03}, //[1:0] 11:new auto precharge  330e in 3372    [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x03},
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x08},
{0x330e, 0x30},
{0x3366, 0x7c}, // div_rst gap
{0x33b6, 0x07},
{0x33b7, 0x17},
{0x33b8, 0x20},
{0x33b9, 0x20},
{0x33ba, 0x44},

{0x3670, 0x08},
{0x367e, 0x08},
{0x367f, 0x18},
{0x3677, 0x86},
{0x3678, 0x86},
{0x3679, 0x88},
{0x3625, 0x0f},
{0x3636, 0x21},
{0x3638, 0x04},
{0x3320, 0x04},
{0x3321, 0x04},
{0x331e, 0x0d},
{0x331f, 0x4d},
{0x3303, 0x10},
{0x3309, 0x50},
{0x3625, 0x0b},
{0x5000, 0x20},
{0x3637, 0x13},
{0x3908, 0x41},
{0x3635, 0x41},
{0x363b, 0x02},
{0x363c, 0x06},
{0x363d, 0x06},
{0x3038, 0x88},
{0x3679, 0xa8},
{0x363c, 0x07},
{0x363d, 0x07},
{0x3e02, 0xc0},
{0x3623, 0x10},
{0x3631, 0x88},
{0x3622, 0xf6},
{0x3301, 0x28},
{0x3630, 0x10},
{0x320c, 0x05},
{0x320d, 0x58},
{0x320e, 0x07},
{0x320f, 0xd0},
{0x3039, 0xc4},
{0x303a, 0x1a},
{0x3029, 0xc4},
{0x302a, 0x1a},
{0x302b, 0x10},
{0x330a, 0x00},
{0x3306, 0x48},
{0x330b, 0xa0},
{0x4837, 0x1d},
{0x3f08, 0x04},
{0x3364, 0x05},
{0x3670, 0x00},
{0x3e01, 0x20},
{0x3303, 0x10},
{0x3304, 0x10},
{0x331e, 0x09},
{0x3320, 0x01},
{0x3326, 0x00},
{0x3309, 0x70},
{0x331f, 0x69},
{0x3366, 0x78},
{0x3638, 0x18},
{0x3639, 0x09},
{0x3637, 0x10},
{0x3635, 0x47},
{0x3e01, 0xf9},
{0x3e02, 0xc0},
{0x3620, 0x28},
{0x330b, 0xaa},
{0x3635, 0x4c},
{0x3235, 0x0f}, //group hold position 1115
{0x3236, 0x9C},
{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0xa8},
{0x3301, 0x34},  //[2d,37]
{0x3630, 0x20},  //1114
{0x3633, 0x22},
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3039, 0xc3},
{0x303a, 0x1b},
{0x3029, 0xc3},
{0x302a, 0x1b},
{0x302b, 0x10},
{0x4837, 0x14},
{0x5002, 0x00},
{0x4502, 0x10},
{0x3f02, 0x05},
{0x3f06, 0x21},
{0x330b, 0xd0},
{0x3306, 0x58},
{0x3309, 0x60},
{0x331f, 0x59},
{0x3208, 0x0a},
{0x3209, 0x00},
{0x320a, 0x05},
{0x320b, 0xa0},

{0x3202, 0x00},
{0x3203, 0xf0},
{0x3206, 0x06},
{0x3207, 0x98},
{0x320c, 0x05},
{0x320d, 0x46},
{0x320e, 0x05},
{0x320f, 0xc8},
{0x3e01, 0xb8},
{0x3e02, 0xc0},
{0x3235, 0x0b}, //group hold position 1115
{0x3236, 0x8C},
{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0x9f},
{0x320d, 0x46},
{0x3303, 0x20},
{0x331e, 0x19},
{0x3614, 0x0c},
{0x3306, 0x70},
{0x330b, 0xb4},

{0x3638, 0x14},
{0x3306, 0x50},
{0x3309, 0x60},
{0x331f, 0x59},
{0x3630, 0x0a},
{0x331b, 0x83},
{0x3333, 0x20},
{0x3638, 0x18},
{0x3306, 0x58},
{0x330b, 0xbc},
{0x3359, 0x22},
{0x335c, 0x22},
{0x33af, 0x22},
{0x33b0, 0x22},
{0x3350, 0x22},
{0x33ae, 0x22},
{0x3332, 0x22},
{0x33b4, 0x22},

{0x330d, 0x30},
{0x5000, 0x26},
{0x5002, 0x06},
{0x578b, 0x17},
{0x5000, 0x00},
{0x6000, 0x20},
{0x6002, 0x00},
{0x5000, 0x20},
{0x5002, 0x00},
{0x3f05, 0x99},
{0x3624, 0x80},

{0x3366, 0xc8},
{0x3621, 0xac},
{0x3623, 0x08},
{0x390a, 0x00},
{0x3624, 0x00},
{0x3624, 0x73},  //tt corner
{0x3f04, 0x02}, //hts/2 - 3f08 - 0x04
{0x3f05, 0x9b},
{0x3306, 0x50},
{0x330b, 0xc0},
{0x363d, 0x03},
{0x3635, 0x4d},
{0x363b, 0x02},
{0x320c, 0x05},
{0x320d, 0x46},
{0x3f04, 0x02},
{0x3f05, 0x9b},

{0x320e, 0x05},
{0x320f, 0xdc},
{0x3235, 0x0b},
{0x3236, 0xb6},
{0x3e01, 0xbb},
{0x3e02, 0x40},

{0x3039, 0xa6},
{0x303a, 0x37},
{0x3029, 0x84},
{0x302a, 0x37},
{0x302b, 0x00},
{0x4837, 0x20},
{0x5000, 0x06},
{0x5002, 0x06},
{0x578b, 0x07},
{0x5780, 0x7f},
{0x57a0, 0x00},
{0x57a1, 0x72},
{0x57a2, 0x01},
{0x57a3, 0xf2},
{0x5781, 0x06},
{0x5782, 0x04},
{0x5783, 0x02},
{0x5784, 0x01},
{0x5785, 0x16},
{0x5786, 0x12},
{0x5787, 0x08},
{0x5788, 0x02},

{0x3306, 0x40},
{0x330b, 0xc0},
{0x3200, 0x00},
{0x3201, 0x0c},
{0x3202, 0x00},
{0x3203, 0xfc},
{0x3204, 0x0a},
{0x3205, 0x23},
{0x3206, 0x06},
{0x3207, 0xab},
{0x3208, 0x0a},
{0x3209, 0x08},
{0x320a, 0x05},
{0x320b, 0xa0},

{0x3301, 0x20},
{0x3630, 0x20},
{0x3633, 0x23},
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3614, 0x00},
{0x3624, 0x47},
{0x3621, 0xac},
{0x338f, 0x40},
{0x5002, 0x06},
{0x3907, 0x00},
{0x3908, 0xf9},
{0x391c, 0x9f},
{0x3632, 0x18},
{0x3802, 0x01},
{0x6000, 0x20},
{0x6002, 0x00},
{0x3200, 0x00},
{0x3201, 0x10},
{0x3202, 0x01},
{0x3203, 0x00},
{0x3204, 0x0a},
{0x3205, 0x23},//last 4 col
{0x3206, 0x06},
{0x3207, 0xa7},
{0x3208, 0x0a},
{0x3209, 0x08},
{0x320a, 0x05},
{0x320b, 0xa0},
{0x3211, 0x04},
{0x3213, 0x04},
{0x3314, 0x14},
{0x3637, 0x20},
{0x3306, 0x50},
{0x330b, 0xb0},
{0x3638, 0x1a},
{0x3029, 0xa7},
{0x302a, 0x34},
{0x302b, 0x10},
{0x302d, 0x00},
{0x3303, 0x28},
{0x331f, 0x51},

{0x3e26, 0x20},
{0x391e, 0x01},
{0x391f, 0xc0},
{0x3318, 0x42},
{0x3319, 0x82},
{0x3307, 0x14},
{0x3339, 0x44},
{0x330f, 0x04},
{0x3310, 0x51},
{0x330e, 0x20},
{0x3038, 0x44},//ppump & npump div

{0x4809, 0x01},//mipi first frame, lp status
{0x5783, 0x00}, //8x
{0x5784, 0x00}, //128x
{0x3039, 0xc6},
{0x303a, 0x33},
{0x3029, 0xc0},
{0x302a, 0x26},
{0x302c, 0x01},
{0x320c, 0x04},
{0x320d, 0xe2},
{0x320e, 0x06},
{0x320f, 0x18},
{0x3f04, 0x02},
{0x3f05, 0x69},  // hts / 2 - 3f08 - 0x04
{0x3235, 0x0c},
{0x3236, 0x2e}, // vts x 2 - 2
{0x3200, 0x01},
{0x3201, 0x14},
{0x3202, 0x00},
{0x3203, 0xd2},
{0x3204, 0x09},
{0x3205, 0x1b},
{0x3206, 0x06},
{0x3207, 0xd5},
{0x3208, 0x08},
{0x3209, 0x00},
{0x320a, 0x06},
{0x320b, 0x00},
{0x3211, 0x04},
{0x3213, 0x02},

{0x3e00, 0x00},//max exposure = vts*2-8; min exposure = 3;
{0x3e01, 0xc2},
{0x3e02, 0x80},
{0x3e03, 0x0b},
{0x3e06, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x03},
{0x3e09, 0x20},
{0x3301, 0x18},
{0x3630, 0x30},
{0x3633, 0x23},
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3905, 0x98},
{0x3e1e, 0x30},
{0x3039, 0x46},//enable pll1
{0x3029, 0x40},//enable pll2

{0x0100, 0x01}, //stream on
};

I2C_ARRAY Sensor_init_table_2304x1296_30fps[] =
{
{0x0103, 0x01}, //soft reset
{0x0100, 0x00}, //stream stop

{0x3039, 0xa6},//bypass pll1
{0x3029, 0xa7},//bypass pll2
{0x3635, 0x4c},
{0x4500, 0x5d},
{0x3e01, 0xf9},
{0x3e02, 0xe0},
{0x3625, 0x0e},
{0x4509, 0x04},
{0x391d, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x1f},
{0x3e09, 0x1f},
{0x3334, 0x40}, //cmp leakage need debug

{0x3621, 0xa8},

{0x3306, 0x30},
{0x3633, 0x65},
{0x3638, 0x84},
{0x3315, 0x02}, //rblue reverse

{0x3308, 0x10},
{0x330a, 0x01},
{0x330b, 0x20},

{0x3634, 0x86},
{0x3303, 0x10},
{0x331e, 0x0b},
{0x3320, 0x07},
{0x3309, 0x52},
{0x331f, 0x4d},
{0x3321, 0x07}, //new offset timing
{0x3367, 0x06}, //auto precharge
{0x336c, 0x01}, //tx even/odd exchange
{0x3907, 0x00},
{0x3908, 0xf9},
{0x4509, 0x10},
{0x3637, 0x0a},
{0x337f, 0x03}, //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x04}, //auto precharge
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x06},
{0x330e, 0x30},
{0x3366, 0x50}, // div_rst gap
{0x3633, 0x76},
{0x320c, 0x07},
{0x320d, 0x08},
{0x3e08, 0x03},
{0x3e09, 0x10},
{0x390a, 0x2c},
{0x3637, 0x12},

{0x3639, 0x08},
{0x3306, 0x40},

{0x3633, 0x86},
{0x3301, 0x20},
{0x3364, 0x15}, //[4] 1: open adit    real value in 3373
{0x336d, 0x40}, //[6] 1: az mode
{0x337f, 0x03}, //[1:0] 11:new auto precharge  330e in 3372    [7:6] 11: close div_rst 00:open div_rst
{0x3368, 0x03},
{0x3369, 0x00},
{0x336a, 0x00},
{0x336b, 0x00},
{0x3367, 0x08},
{0x330e, 0x30},
{0x3366, 0x7c},

{0x33b6, 0x07},
{0x33b7, 0x17},
{0x33b8, 0x20},
{0x33b9, 0x20},
{0x33ba, 0x44},

{0x3670, 0x08},
{0x367e, 0x08},
{0x367f, 0x18},
{0x3677, 0x86},
{0x3678, 0x86},
{0x3679, 0x88},
{0x3625, 0x0f},
{0x3636, 0x21},
{0x3638, 0x04},
{0x3320, 0x04},
{0x3321, 0x04},
{0x331e, 0x0d},
{0x331f, 0x4d},
{0x3303, 0x10},
{0x3309, 0x50},
{0x3625, 0x0b},
{0x5000, 0x20},
{0x3637, 0x13},
{0x3908, 0xf9},
{0x3635, 0x41},
{0x363b, 0x02},
{0x363c, 0x06},
{0x363d, 0x06},
{0x3038, 0x88},
{0x3679, 0xa8},  //cmp current
{0x363c, 0x07},
{0x363d, 0x07},
{0x3e02, 0xc0},

{0x3623, 0x10}, // ph1/ph2 delay
{0x3631, 0x88},
{0x3622, 0xf6},
{0x3301, 0x28},
{0x3630, 0x10},
{0x320c, 0x05},     //hts=2736
{0x320d, 0x58},
{0x320e, 0x07},     //vts=2000
{0x320f, 0xd0},
{0x3039, 0xc4}, //684M MIPI speed 27/3*2*38
{0x303a, 0x1a},
{0x3029, 0xc4}, //342M cntclk  27/3*2*38/2
{0x302a, 0x1a},
{0x302b, 0x10},
{0x330a, 0x00},
{0x3306, 0x48},
{0x330b, 0xa0},
{0x4837, 0x1d},
{0x3f08, 0x04},
{0x3364, 0x05}, //[4] 1: open adit    real value in 3373
{0x3670, 0x00 }, //[3]:3633 logic ctrl  real value in 3682
{0x3e01, 0x20},

{0x3303, 0x10},
{0x3304, 0x10},
{0x331e, 0x09},
{0x3320, 0x01},
{0x3326, 0x00},
{0x3309, 0x70},
{0x331f, 0x69},  //new offset timing
{0x3366, 0x78},
{0x3638, 0x18},
{0x3639, 0x09},
{0x3637, 0x10},
{0x3635, 0x47},
{0x3e01, 0xf9},
{0x3e02, 0xc0},
{0x3620, 0x28},
{0x330b, 0xaa},
{0x3635, 0x4c},
{0x3235, 0x0f}, //group hold position 1115
{0x3236, 0x9C},

{0x3f00, 0x0d}, //[2]   hts/2-4
{0x3f04, 0x02},
{0x3f05, 0xa8},
{0x3301, 0x34},  //[2d,37]
{0x3630, 0x20},  //1114
{0x3633, 0x22},
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3039, 0xc3},  //99.9M sysclk 499.5M cntclk
{0x303a, 0x1b},
{0x3029, 0xc3},
{0x302a, 0x1b},
{0x302b, 0x10},
{0x4837, 0x14},
{0x5002, 0x00},
{0x4502, 0x10},
{0x3f02, 0x05},
{0x3f06, 0x21},
{0x330b, 0xd0},
{0x3306, 0x58},
{0x3309, 0x60},
{0x331f, 0x59},
{0x3208, 0x0a},
{0x3209, 0x00},
{0x320a, 0x05},
{0x320b, 0xa0},
{0x3202, 0x00},
{0x3203, 0xf0},
{0x3206, 0x06},
{0x3207, 0x98},
{0x320c, 0x05},
{0x320d, 0x46},
{0x320e, 0x05},
{0x320f, 0xc8},
{0x3e01, 0xb8},
{0x3e02, 0xc0},
{0x3235, 0x0b},
{0x3236, 0x8C},
{0x3f00, 0x0d},
{0x3f04, 0x02},
{0x3f05, 0x9f},
{0x320d, 0x46},
{0x3303, 0x20},
{0x331e, 0x19},
{0x3614, 0x0c},
{0x3306, 0x70},
{0x330b, 0xb4},
{0x3638, 0x14},
{0x3306, 0x50},
{0x3309, 0x60},
{0x331f, 0x59},
{0x3630, 0x0a},
{0x331b, 0x83},
{0x3333, 0x20},
{0x3638, 0x18},
{0x3306, 0x58},
{0x330b, 0xbc},
{0x3359, 0x22},
{0x335c, 0x22},
{0x33af, 0x22},
{0x33b0, 0x22},
{0x3350, 0x22},
{0x33ae, 0x22},
{0x3332, 0x22},
{0x33b4, 0x22},

{0x330d, 0x30},
{0x5000, 0x26},
{0x5002, 0x06},
{0x578b, 0x17},
{0x5000, 0x00},
{0x6000, 0x20},
{0x6002, 0x00},
{0x5000, 0x20},
{0x5002, 0x00},
{0x3f05, 0x99},
{0x3624, 0x80},

{0x3366, 0xc8},
{0x3621, 0xac},
{0x3623, 0x08},
{0x390a, 0x00},
{0x3624, 0x00},
{0x3624, 0x73},  //tt corner

{0x3f04, 0x02}, //hts/2 - 3f08 - 0x04
{0x3f05, 0x9b},
{0x3306, 0x50},//[42,58]
{0x330b, 0xc0},//[b4,e0]
{0x363d, 0x03},
{0x3635, 0x4d},
{0x363b, 0x02},
{0x320c, 0x05},//hts=2700
{0x320d, 0x46},
{0x3f04, 0x02},
{0x3f05, 0x9b},  // hts / 2 - 3f08 - 0x04
{0x320e, 0x05}, //vts=1500
{0x320f, 0xdc},
{0x3235, 0x0b},
{0x3236, 0xb6}, // vts x 2 - 2
{0x3e01, 0xbb},
{0x3e02, 0x40},
{0x3039, 0xa6},//121.5MPCLK
{0x303a, 0x37},//607.5MVCO
{0x3029, 0x84},//486MCntCLK
{0x302a, 0x37},
{0x302b, 0x00},
{0x4837, 0x20},
{0x5000, 0x06}, //dpc enable
{0x5002, 0x06},
{0x578b, 0x07}, //4 channel
{0x5780, 0x7f}, //auto blc setting
{0x57a0, 0x00}, //gain0 = 2x 0x0720 to 0x073f
{0x57a1, 0x72},
{0x57a2, 0x01}, //gain1 = 8x 0x1f20 to 0x1f3f
{0x57a3, 0xf2},
{0x5781, 0x06}, //white 1x
{0x5782, 0x04}, //2x
{0x5783, 0x02}, //8x
{0x5784, 0x01}, //128x
{0x5785, 0x16}, //black 1x
{0x5786, 0x12}, //2x
{0x5787, 0x08}, //8x
{0x5788, 0x02}, //128x

{0x3306, 0x40},//[26,70]20180419
{0x330b, 0xc0},//[80,100]20180419
{0x3200, 0x00},
{0x3201, 0x0c},
{0x3202, 0x00},
{0x3203, 0xfc},
{0x3204, 0x0a},
{0x3205, 0x23},
{0x3206, 0x06},
{0x3207, 0xab},
{0x3208, 0x0a},
{0x3209, 0x08},
{0x320a, 0x05},
{0x320b, 0xa0},
{0x3301, 0x20},  //[e,30] 20180419
{0x3630, 0x20},  //1114
{0x3633, 0x23},//20180419
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3614, 0x00},
{0x3624, 0x47},
{0x3621, 0xac},
{0x338f, 0x40},
{0x5002, 0x06},
{0x3907, 0x00},
{0x3908, 0xf9},
{0x391c, 0x9f},

{0x3632, 0x18},  //pump driver current

{0x3802, 0x01},
{0x6000, 0x20},
{0x6002, 0x00},
{0x3200, 0x00},
{0x3201, 0x10},
{0x3202, 0x01},
{0x3203, 0x00},
{0x3204, 0x0a},
{0x3205, 0x23},//last 4 col
{0x3206, 0x06},
{0x3207, 0xa7},
{0x3208, 0x0a},
{0x3209, 0x08},
{0x320a, 0x05},
{0x320b, 0xa0},
{0x3211, 0x04},
{0x3213, 0x04},
{0x3314, 0x14},
{0x3637, 0x20},

{0x3306, 0x50},
{0x330b, 0xb8},

{0x3638, 0x1a},
{0x3029, 0xa7},
{0x302a, 0x34},
{0x302b, 0x10},
{0x302d, 0x00},
{0x3303, 0x28},
{0x331f, 0x51},
{0x3e26, 0x20},
{0x391e, 0x01},
{0x391f, 0xc0},
{0x3318, 0x42},
{0x3319, 0x82},
{0x3307, 0x14},
{0x3339, 0x44},
{0x330f, 0x04},
{0x3310, 0x51},
{0x330e, 0x20},
{0x3038, 0x44},//ppump & npump div
{0x4809, 0x01},//mipi first frame, lp status
{0x5783, 0x00}, //8x
{0x5784, 0x00}, //128x
{0x3200, 0x00},
{0x3201, 0x94},
{0x3202, 0x01},
{0x3203, 0x48},
{0x3204, 0x09},
{0x3205, 0x9b},
{0x3206, 0x06},
{0x3207, 0x5f},
{0x3208, 0x09},
{0x3209, 0x00},
{0x320a, 0x05},
{0x320b, 0x10},
{0x3211, 0x04},
{0x3213, 0x04},
{0x3e00, 0x00},//max exposure = vts*2-8; min exposure = 3;
{0x3e01, 0xbb},
{0x3e02, 0x00},
{0x3e03, 0x0b},
{0x3e06, 0x00},
{0x3e07, 0x80},
{0x3e08, 0x03},
{0x3e09, 0x20},
{0x3301, 0x1c},  //[12,1c]  20180727[14,20]
{0x3630, 0x30},  //20180706
{0x3633, 0x23},//20180419
{0x3622, 0xf6},
{0x363a, 0x83},
{0x3905, 0x98},
{0x3e1e, 0x30},
{0x3039, 0x26},//enable pll1
{0x3029, 0x27},//enable pll2

{0x0100, 0x01}, //stream on
};

I2C_ARRAY Sensor_init_table_HDR_DOL_2lane[] =
{
//new_SC5235_27Minput_2lane_2592x1944p_594Mbps_VC_HDR_10fps.ini
{0x0103,0x01},
{0x0100,0x00},
{0x3039,0x84},//bypass pll1
{0x3029,0xb7},//bypass pll2
{0x4500,0x5d},
{0x391d,0x00},
{0x3334,0x40},
{0x3315,0x02},
{0x3308,0x10},
{0x3634,0x86},
{0x336c,0x01},
{0x4509,0x10},
{0x336d,0x40},
{0x337f,0x03},
{0x3368,0x03},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x33b6,0x07},
{0x33b7,0x17},
{0x33b8,0x20},
{0x33b9,0x20},
{0x33ba,0x44},
{0x367e,0x08},
{0x367f,0x18},
{0x3677,0x86},
{0x3678,0x86},
{0x3636,0x21},
{0x3321,0x04},
{0x3625,0x0b},
{0x3679,0xa8},
{0x363c,0x07},
{0x3623,0x10},
{0x3631,0x88},
{0x330a,0x00},
{0x4837,0x21},
{0x3f08,0x04},
{0x3364,0x05},
{0x3670,0x00},
{0x3304,0x10},
{0x3320,0x01},
{0x3326,0x00},
{0x3366,0x78},
{0x3638,0x18},
{0x3639,0x09},
{0x3620,0x28},
{0x303a,0x35},
{0x302a,0x35},
{0x320c,0x05},
{0x320d,0xdc},
{0x3f00,0x0d},
{0x3802,0x01},
{0x5002,0x06},
{0x331b,0x83},
{0x3333,0x20},
{0x390a,0x00},
{0x3f04,0x02},
{0x3f05,0xe6},
{0x363d,0x03},
{0x3635,0x4d},
{0x363b,0x02},
{0x3208,0x0a},
{0x3209,0x20},
{0x3200,0x00},
{0x3201,0x04},
{0x3204,0x0a},
{0x3211,0x04},
{0x5000,0x06},
{0x578b,0x07},
{0x5780,0x7f},
{0x57a0,0x00},
{0x57a1,0x72},
{0x57a2,0x01},
{0x57a3,0xf2},
{0x5781,0x06},
{0x5782,0x04},
{0x5785,0x16},
{0x5786,0x12},
{0x5787,0x08},
{0x5788,0x02},
{0x3624,0x47},
{0x3621,0xac},
{0x338f,0x40},
{0x3306,0x50},
{0x330b,0xb8},
{0x3907,0x00},
{0x3908,0xf9},
{0x391c,0x9f},
{0x3632,0x18},
{0x6000,0x20},
{0x6002,0x00},
{0x320a,0x07},
{0x320b,0x98},
{0x3205,0x2b},
{0x3213,0x04},
{0x3207,0x9f},
{0x3314,0x14},
{0x330e,0x20},
{0x3637,0x20},
{0x3303,0x28},
{0x331e,0x19},
{0x331f,0x61},
{0x3309,0x70},
{0x302b,0x10},
{0x302d,0x00},
{0x3e26,0x20},
{0x391e,0x01},
{0x391f,0xc0},
{0x3038,0x44},
{0x4809,0x01},
{0x5783,0x00},
{0x5784,0x00},
{0x3220,0x50},
{0x3e23,0x00},
{0x3e24,0xf0},
{0x320e,0x0f},
{0x320f,0x78},
{0x3235,0x1e},
{0x3236,0xee},
{0x3e00,0x01},
{0x3e01,0xd0},
{0x3e02,0x20},
{0x3e04,0x1d},
{0x3e05,0x40},
{0x4816,0x51},
{0x4602,0x0f},
{0x3988,0x11},
{0x3e03,0x0b},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x20},
{0x3301,0x1c},
{0x3630,0x30},
{0x3633,0x23},
{0x3622,0xf6},
{0x363a,0x83},
{0x3905,0x98},
{0x3e1e,0x30},
{0x3039,0x04},
{0x3029,0x37},
{0x0100,0x01},
};


FINE_GAIN fine_again[] = {
//gain map update for 1/32 precision
    {100000, 0x20},
    {103125, 0x21},
    {106250, 0x22},
    {109375, 0x23},
    {112500, 0x24},
    {115625, 0x25},
    {118750, 0x26},
    {121875, 0x27},
    {125000, 0x28},
    {128125, 0x29},
    {131250, 0x2a},
    {134375, 0x2b},
    {137500, 0x2c},
    {140625, 0x2d},
    {143750, 0x2e},
    {146875, 0x2f},
    {150000, 0x30},
    {153125, 0x31},
    {156250, 0x32},
    {159375, 0x33},
    {162500, 0x34},
    {165625, 0x35},
    {168750, 0x36},
    {171875, 0x37},
    {175000, 0x38},
    {178125, 0x39},
    {181250, 0x3a},
    {184375, 0x3b},
    {187500, 0x3c},
    {190625, 0x3d},
    {193750, 0x3e},
    {196875, 0x3f},
};


I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    //{0x3213, 0x04}, // crop for bayer
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


I2C_ARRAY mirr_flip_table[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    //{0x3213, 0x04}, // crop for bayer
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
    {0x3e10, 0x00},
    {0x3e11, 0x00},
    {0x3e12, 0x00|0x03},
    {0x3e13, 0x10}, //low bit, 0x40 - 0x7f, step 1/64
    };

I2C_ARRAY expo_reg[] = {

    {0x3e00, 0x00}, //expo [20:17]
    {0x3e01, 0x30}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
};
I2C_ARRAY expo_reg_HDR_DOL_SEF[] = {
    {0x3e04, 0x21}, // expo[7:0]
    {0x3e05, 0x00}, // expo[7:4]
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x07},
    {0x320f, 0xbc}
};

#if ENABLE_NR
I2C_ARRAY nr_reg[] = {
    {0x3301,0x1c},
    {0x3630,0x30},
    {0x3633,0x23},
    {0x3622,0xf6},
    {0x363a,0x83},
};
#endif

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
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME sc5235


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
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    }

    //ISP_config_io(handle);
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    //sensor_if->Set3ATaskOrder(handle, def_order);
    // pure power on
    //ISP_config_io(handle);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
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
    SENSOR_DMSG("[%s]sc5235 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int sc5235_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init_5M20fps(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2560x1920_20fps);i++)
    {
        if(Sensor_init_table_2560x1920_20fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2560x1920_20fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2560x1920_20fps[i].reg,Sensor_init_table_2560x1920_20fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2560x1920_20fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_5M20fps_2(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2592x1944_20fps);i++)
    {
        if(Sensor_init_table_2592x1944_20fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2592x1944_20fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2592x1944_20fps[i].reg,Sensor_init_table_2592x1944_20fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2592x1944_20fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_2592x1520_25fps(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2592x1520_25fps);i++)
    {
        if(Sensor_init_table_2592x1520_25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2592x1520_25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2592x1520_25fps[i].reg,Sensor_init_table_2592x1520_25fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2592x1520_25fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_2560x1440_30fps(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2560x1440_30fps);i++)
    {
        if(Sensor_init_table_2560x1440_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2560x1440_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2560x1440_30fps[i].reg,Sensor_init_table_2560x1440_30fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2560x1440_30fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_2304x1296_30fps(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2304x1296_30fps);i++)
    {
        if(Sensor_init_table_2304x1296_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2304x1296_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2304x1296_30fps[i].reg,Sensor_init_table_2304x1296_30fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2304x1296_30fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_2048x1536_30fps(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2048x1536_30fps);i++)
    {
        if(Sensor_init_table_2048x1536_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2048x1536_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2048x1536_30fps[i].reg,Sensor_init_table_2048x1536_30fps[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2048x1536_30fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}
static int pCus_init_5M10fps_HDR_DOL(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_2lane);i++)
    {
        if(Sensor_init_table_HDR_DOL_2lane[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR_DOL_2lane[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_2lane[i].reg,Sensor_init_table_HDR_DOL_2lane[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_2048x1536_30fps -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

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
    u32 num_res = handle->video_res_supported.num_res;
    sc5235_params *params = (sc5235_params *)handle->private_data;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
         case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_5M20fps;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain;
            vts_30fps = 1980;
            params->expo.vts = vts_30fps;
            params->expo.fps=20;
            Preview_line_period = 25253;
            break;
         case 1:
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_5M20fps_2;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain_2592x1944;
            vts_30fps = 2025;
            params->expo.vts = vts_30fps;
            params->expo.fps=20;
            Preview_line_period = 24692;
            break;
        case 2:
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init_2592x1520_25fps;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain_2592x1520;
            vts_30fps = 1620;
            params->expo.vts = vts_30fps;
            params->expo.fps=25;
            Preview_line_period = 24691;
            break;
        case 3:
            handle->video_res_supported.ulcur_res = 3;
            handle->pCus_sensor_init = pCus_init_2560x1440_30fps;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain_2592x1520;
            vts_30fps = 1500;
            params->expo.vts = vts_30fps;
            params->expo.fps=30;
            Preview_line_period = 22222;
            break;
        case 4:
            handle->video_res_supported.ulcur_res = 4;
            handle->pCus_sensor_init = pCus_init_2304x1296_30fps;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain_2592x1520;
            vts_30fps = 1500;
            params->expo.vts = vts_30fps;
            params->expo.fps=30;
            Preview_line_period = 22222;
            break;
        case 5:
            handle->video_res_supported.ulcur_res = 5;
            handle->pCus_sensor_init = pCus_init_2048x1536_30fps;
            handle->pCus_sensor_SetAEGain = pCus_SetAEGain_2048x1536;
            vts_30fps = 1594;
            params->expo.vts = vts_30fps;
            params->expo.fps=30;
            Preview_line_period = 21368;
            break;
        default:
            break;
    }

    return SUCCESS;
}
static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    sc5235_params *params = (sc5235_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_5M10fps_HDR_DOL;
            params->expo.vts = vts_10fps_HDR_DOL;
            params->expo.fps = 10;
            params->expo.max_short_exp=240;
            break;
        default:
            break;
    }
    return SUCCESS;
}


static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sen_data = mirr_flip_table[0].data;
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

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
   sc5235_params *params = (sc5235_params *)handle->private_data;
   switch(orit) {
     case CUS_ORIT_M0F0:
       if (mirr_flip_table[0].data) {
           mirr_flip_table[0].data = 0;
           params->reg_mf = true;
       }
       break;
     case CUS_ORIT_M1F0:
       if (mirr_flip_table[0].data!=6) {
           mirr_flip_table[0].data = 6;
           params->reg_mf = true;
       }
       break;
     case CUS_ORIT_M0F1:
       if (mirr_flip_table[0].data!=0x60) {
           mirr_flip_table[0].data = 0x60;
           params->reg_mf = true;
       }
       break;
     case CUS_ORIT_M1F1:
       if (mirr_flip_table[0].data!=0x66) {
           mirr_flip_table[0].data = 0x66;
           params->reg_mf = true;
       }
       break;
   }
     return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
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
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps*1000  + fps * 500)/(fps*1000);
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000) + (fps>>1))/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > 2* (params->expo.vts) -8){
        vts = (params->expo.line + 9)/2;
    }else{
        vts = params->expo.vts;
    }
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS_HDR_SEF(ms_cus_sensor *handle)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_10fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_10fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF(ms_cus_sensor *handle, u32 fps)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    //pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_10fps_HDR_DOL*max_fps)/fps;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_10fps_HDR_DOL*(max_fps*1000))/fps;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_sef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else{
        pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

}
static int pCus_SetFPS_hdr_dol_lef(ms_cus_sensor *handle, u32 fps)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
//  pr_info("[%s]  max_min_fps : %d ,%d\n\n", __FUNCTION__,max_fps,min_fps);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_10fps_HDR_DOL*max_fps)/fps;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_lef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_10fps_HDR_DOL*(max_fps*1000))/fps;
        vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        pr_info("[%s]  vts_reg_lef : %x , %x\n\n", __FUNCTION__,vts_reg[0].data,vts_reg[1].data);
        return SUCCESS;
    }else{
        pr_info("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
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

    sc5235_params *params = (sc5235_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->reg_mf)
        {
            //sensor_if->SetSkipFrame(handle,2);  //to do skip frame method
            SensorRegArrayW((I2C_ARRAY*)mirr_flip_table, sizeof(mirr_flip_table)/sizeof(I2C_ARRAY));
            params->reg_mf = false;
        }
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
            SensorReg_Write(0x3812,0x00);
#if ENABLE_NR
            SensorRegArrayW((I2C_ARRAY*)nr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
#endif
            SensorReg_Write(0x3812,0x30);
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
        sc5235_params *params = (sc5235_params *)handle->private_data;

        switch(status)
        {
            case CUS_FRAME_INACTIVE:

            break;
            case CUS_FRAME_ACTIVE:
            if(params->reg_mf)
            {
                //sensor_if->SetSkipFrame(handle,2);  //to do skip frame method
                SensorRegArrayW((I2C_ARRAY*)mirr_flip_table, sizeof(mirr_flip_table)/sizeof(I2C_ARRAY));
                params->reg_mf = false;
            }
            if(params->reg_dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
                SensorReg_Write(0x3812,0x00);
#if ENABLE_NR
                SensorRegArrayW((I2C_ARRAY*)nr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
#endif
                SensorReg_Write(0x3812,0x30);
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
            sc5235_params *params = (sc5235_params *)handle->private_data;

            switch(status)
            {
                case CUS_FRAME_INACTIVE:

                break;
                case CUS_FRAME_ACTIVE:
                if(params->reg_mf)
                {
                    //sensor_if->SetSkipFrame(handle,2);  //to do skip frame method
                    SensorRegArrayW((I2C_ARRAY*)mirr_flip_table, sizeof(mirr_flip_table)/sizeof(I2C_ARRAY));
                    params->reg_mf = false;
                }
                if(params->reg_dirty)
                {
                    SensorRegArrayW((I2C_ARRAY*)expo_reg_HDR_DOL_SEF, sizeof(expo_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY));
                    SensorRegArrayW((I2C_ARRAY*)gain_reg_HDR_DOL_SEF, sizeof(gain_reg_HDR_DOL_SEF)/sizeof(I2C_ARRAY));
                    SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
                    SensorReg_Write(0x3812,0x00);
#if ENABLE_NR
                    SensorRegArrayW((I2C_ARRAY*)nr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
#endif
                    SensorReg_Write(0x3812,0x30);
                    params->reg_dirty = false;
               }
                break;
                default :
                break;
            }
            return SUCCESS;

}
static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0,dou_lines = 0,vts = 0;
    sc5235_params *params = (sc5235_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));
    dou_lines = (1000*us)/(Preview_line_period_HDR_DOL*2); // Preview_line_period in ns
    half_lines = 4*dou_lines;
    if(half_lines<5) half_lines=5;
    if (half_lines >  2 * (params->expo.vts-params->expo.max_short_exp-7)) {
        half_lines = 2 * (params->expo.vts-params->expo.max_short_exp-7);
    }
    else
     vts=params->expo.vts;
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

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0,dou_lines = 0,vts = 0;
    sc5235_params *params = (sc5235_params *)handle->private_data;
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
     vts=params->expo.vts;
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
    sc5235_params *params = (sc5235_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-8) {
        vts = (half_lines+9)/2;
    }
    else
        vts=params->expo.vts;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    params->expo.line = half_lines;
    half_lines = half_lines<<4;
//  printf("===================================================================\n");
//  printf("us = %d  half_lines = %x params->expo.vts = %x\n",us, half_lines, params->expo.vts);
//  printf("===================================================================\n");
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
    u8 Dgain = 1,  Coarse_gain = 1;

    Coarse_gain = ((gain_reg[2].data&0x1C)>>2) +1;
    Dgain = ((gain_reg[0].data&0x0f) + 1);

    *gain = (Coarse_gain*Dgain*(gain_reg[1].data)*(gain_reg[3].data))/4;
    //SENSOR_DMSG("[%s] gain/reg = %d, 0x%x,  0x%x  0x%x 0x%x\n", __FUNCTION__, *gain,gain_reg[0].data,gain_reg[1].data,gain_reg[2].data,gain_reg[3].data);
    //SENSOR_DMSG("[%s] gain/reg = %d, %f ,%d, %d %f %f\n", __FUNCTION__, *gain,temp_gain,Coarse_gain,Dgain,Fine_again,Fine_dgain);
    return rc;
}
static int pCus_GetAEGainHDR_DOL_SEF(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;
    u8 Dgain = 1,  Coarse_gain = 1;

    Coarse_gain = ((gain_reg_HDR_DOL_SEF[2].data&0x1C)>>2) +1;
    Dgain = ((gain_reg_HDR_DOL_SEF[0].data&0x0f) + 1);

    *gain = (Coarse_gain*Dgain*(gain_reg_HDR_DOL_SEF[1].data)*(gain_reg_HDR_DOL_SEF[3].data))/2;


    return rc;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        nr_reg[0].data = 0x1c;
        nr_reg[1].data = 0x30;
        nr_reg[2].data = 0x23;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x83;
    } else if (gain < 4*1024) {
        nr_reg[0].data = 0x26;
        nr_reg[1].data = 0x23;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x87;
    } else if (gain < 8*1024) {
        nr_reg[0].data = 0x2c;
        nr_reg[1].data = 0x24;
        nr_reg[2].data = 0x43;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain < 16128) {
        nr_reg[0].data = 0x38;
        nr_reg[1].data = 0x28;
        nr_reg[2].data = 0x43;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain >= 16128){
        nr_reg[0].data = 0x44;
        nr_reg[1].data = 0x19;
        nr_reg[2].data = 0x55;
        nr_reg[3].data = 0x16;
        nr_reg[4].data = 0x9f;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  63 * 1024)
    {
        Dgain = 2;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 126 * 1024)
    {
        Dgain = 4;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain < 252 * 1024)
    {
        Dgain = 8;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16128)
    {
        Fine_againx32 = (gain*4) / (Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);
        Fine_again_reg = Fine_againx32;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);;
        Fine_dgain_reg = Fine_dgainx128;
    }

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

static int pCus_SetAEGain_2592x1520(ms_cus_sensor *handle, u32 gain) {
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        nr_reg[0].data = 0x1c;
        nr_reg[1].data = 0x30;
        nr_reg[2].data = 0x23;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x83;
    } else if (gain < 4*1024) {
        nr_reg[0].data = 0x20;
        nr_reg[1].data = 0x23;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x87;
    } else if (gain < 8*1024) {
        nr_reg[0].data = 0x24;
        nr_reg[1].data = 0x24;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain < 16128) {
        nr_reg[0].data = 0x30;
        nr_reg[1].data = 0x16;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain >= 16128){
        nr_reg[0].data = 0x44;
        nr_reg[1].data = 0x19;
        nr_reg[2].data = 0x45;
        nr_reg[3].data = 0x16;
        nr_reg[4].data = 0x9f;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  63 * 1024)
    {
        Dgain = 2;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 126 * 1024)
    {
        Dgain = 4;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain < 252 * 1024)
    {
        Dgain = 8;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16128)
    {
        Fine_againx32 = (gain*4) / (Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);
        Fine_again_reg = Fine_againx32;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);;
        Fine_dgain_reg = Fine_dgainx128;
    }

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

static int pCus_SetAEGain_2048x1536(ms_cus_sensor *handle, u32 gain) {
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        nr_reg[0].data = 0x18;
        nr_reg[1].data = 0x30;
        nr_reg[2].data = 0x23;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x83;
    } else if (gain < 4*1024) {
        nr_reg[0].data = 0x1a;
        nr_reg[1].data = 0x23;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x87;
    } else if (gain < 8*1024) {
        nr_reg[0].data = 0x22;
        nr_reg[1].data = 0x24;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain < 16128) {
        nr_reg[0].data = 0x30;
        nr_reg[1].data = 0x16;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain >= 16128){
        nr_reg[0].data = 0x30;
        nr_reg[1].data = 0x19;
        nr_reg[2].data = 0x45;
        nr_reg[3].data = 0x16;
        nr_reg[4].data = 0x9f;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  63 * 1024)
    {
        Dgain = 2;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 126 * 1024)
    {
        Dgain = 4;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain < 252 * 1024)
    {
        Dgain = 8;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16128)
    {
        Fine_againx32 = (gain*4) / (Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);
        Fine_again_reg = Fine_againx32;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);;
        Fine_dgain_reg = Fine_dgainx128;
    }

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

static int pCus_SetAEGain_2592x1944(ms_cus_sensor *handle, u32 gain) {
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        nr_reg[0].data = 0x1c;
        nr_reg[1].data = 0x30;
        nr_reg[2].data = 0x23;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x83;
    } else if (gain < 4*1024) {
        nr_reg[0].data = 0x26;
        nr_reg[1].data = 0x23;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x87;
    } else if (gain < 8*1024) {
        nr_reg[0].data = 0x24;
        nr_reg[1].data = 0x24;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain < 16128) {
        nr_reg[0].data = 0x30;
        nr_reg[1].data = 0x16;
        nr_reg[2].data = 0x33;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain >= 16128){
        nr_reg[0].data = 0x44;
        nr_reg[1].data = 0x19;
        nr_reg[2].data = 0x45;
        nr_reg[3].data = 0x16;
        nr_reg[4].data = 0x9f;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  63 * 1024)
    {
        Dgain = 2;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 126 * 1024)
    {
        Dgain = 4;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain < 252 * 1024)
    {
        Dgain = 8;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16128)
    {
        Fine_againx32 = (gain*4) / (Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);
        Fine_again_reg = Fine_againx32;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);;
        Fine_dgain_reg = Fine_dgainx128;
    }

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
     sc5235_params *params = (sc5235_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e10, 0x00},
        {0x3e11, 0x00},
        {0x3e12, (0x00|0x03)},
        {0x3e13, 0x10},
    };

    memcpy(gain_reg_temp, gain_reg_HDR_DOL_SEF, sizeof(gain_reg_HDR_DOL_SEF));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        nr_reg[0].data = 0x20;
        nr_reg[1].data = 0x30;
        nr_reg[2].data = 0x34;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x83;
    } else if (gain < 4*1024) {
        nr_reg[0].data = 0x28;
        nr_reg[1].data = 0x34;
        nr_reg[2].data = 0x35;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x87;
    } else if (gain < 8*1024) {
        nr_reg[0].data = 0x28;
        nr_reg[1].data = 0x24;
        nr_reg[2].data = 0x35;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain < 16128) {
        nr_reg[0].data = 0x48;
        nr_reg[1].data = 0x16;
        nr_reg[2].data = 0x45;
        nr_reg[3].data = 0xf6;
        nr_reg[4].data = 0x9f;
    } else if (gain >= 16128){
        nr_reg[0].data = 0x48;
        nr_reg[1].data = 0x09;
        nr_reg[2].data = 0x46;
        nr_reg[3].data = 0x16;
        nr_reg[4].data = 0x9f;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x07;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x0f;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  63 * 1024)
    {
        Dgain = 2;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 126 * 1024)
    {
        Dgain = 4;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain < 252 * 1024)
    {
        Dgain = 8;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain < SENSOR_MAXGAIN * 1024)
    {
        Dgain = 16;      Fine_againx32 = 63;          Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <= 16128)
    {
        Fine_againx32 = (gain*4) / (Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);
        Fine_again_reg = Fine_againx32;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        Fine_dgainx128 = (gain*4) / (Dgain * Coarse_gain * Fine_againx32);;
        Fine_dgain_reg = Fine_dgainx128;
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

static int sc5235_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period * 3) / 2;
    info->step = Preview_line_period;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period_HDR_DOL * 5);
    info->step = Preview_line_period_HDR_DOL*4;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
    info->max = Preview_line_period_HDR_DOL*params->expo.max_short_exp;
    info->min = (Preview_line_period_HDR_DOL * 5);
    info->step = Preview_line_period_HDR_DOL*4;
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
static int pCus_GetOrien_hdr_dol_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    *orit = CUS_ORIT_M0F0;
    return SUCCESS;
}

static int pCus_SetOrien_hdr_dol_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    return SUCCESS;
}

static int pCus_GetFPS_hdr_dol_lef(ms_cus_sensor *handle)
{
    sc5235_params *params = (sc5235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_10fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_10fps_HDR_DOL*max_fps)/tVts;

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

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc5235_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(sc5235_params));
    params = (sc5235_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    SENSOR_DMSG(handle->model_id,"sc5235_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth= 2560;
    handle->video_res_supported.res[0].nOutputHeight= 1920;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1920@20fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = 2592;
    handle->video_res_supported.res[1].height = 1944;
    handle->video_res_supported.res[1].max_fps= 20;//29.98
    handle->video_res_supported.res[1].min_fps= 3;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 2592;
    handle->video_res_supported.res[1].nOutputHeight= 1944;
    sprintf(handle->video_res_supported.res[1].strResDesc, "2592x1944@20fps");

    handle->video_res_supported.num_res = 3;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[2].width = 2592;
    handle->video_res_supported.res[2].height = 1520;
    handle->video_res_supported.res[2].max_fps= 25;
    handle->video_res_supported.res[2].min_fps= 3;
    handle->video_res_supported.res[2].crop_start_x= 0;
    handle->video_res_supported.res[2].crop_start_y= 0;
    handle->video_res_supported.res[2].nOutputWidth= 2592;
    handle->video_res_supported.res[2].nOutputHeight= 1520;
    sprintf(handle->video_res_supported.res[2].strResDesc, "2592x1520@25fps");

    handle->video_res_supported.num_res = 4;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[3].width = 2560;
    handle->video_res_supported.res[3].height = 1440;
    handle->video_res_supported.res[3].max_fps= 30;
    handle->video_res_supported.res[3].min_fps= 3;
    handle->video_res_supported.res[3].crop_start_x= 0;
    handle->video_res_supported.res[3].crop_start_y= 0;
    handle->video_res_supported.res[3].nOutputWidth= 2560;
    handle->video_res_supported.res[3].nOutputHeight= 1440;
    sprintf(handle->video_res_supported.res[3].strResDesc, "2560x1440@30fps");

    handle->video_res_supported.num_res = 5;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[4].width = 2304;
    handle->video_res_supported.res[4].height = 1296;
    handle->video_res_supported.res[4].max_fps= 30;
    handle->video_res_supported.res[4].min_fps= 3;
    handle->video_res_supported.res[4].crop_start_x= 0;
    handle->video_res_supported.res[4].crop_start_y= 0;
    handle->video_res_supported.res[4].nOutputWidth= 2304;
    handle->video_res_supported.res[4].nOutputHeight= 1296;
    sprintf(handle->video_res_supported.res[4].strResDesc, "2304x1296@30fps");

    handle->video_res_supported.num_res = 6;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[5].width = 2048;
    handle->video_res_supported.res[5].height = 1536;
    handle->video_res_supported.res[5].max_fps= 30;//29.98
    handle->video_res_supported.res[5].min_fps= 3;
    handle->video_res_supported.res[5].crop_start_x= 0;
    handle->video_res_supported.res[5].crop_start_y= 0;
    handle->video_res_supported.res[5].nOutputWidth= 2048;
    handle->video_res_supported.res[5].nOutputHeight= 1536;
    sprintf(handle->video_res_supported.res[5].strResDesc, "2048x1536@30fps");


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
    handle->pCus_sensor_init        = pCus_init_5M20fps;//pCus_init_5M20fps    ;

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
    handle->pCus_sensor_SetPatternMode = sc5235_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = sc5235_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 20;
    params->expo.line= 1500;
    params->reg_dirty = false;
    params->reg_mf = false;
    return SUCCESS;
}
int cus_camsensor_init_handle_hdr_dol_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc5235_params *params = NULL;

    cus_camsensor_init_handle(drv_handle);
    params = (sc5235_params *)handle->private_data;

    sprintf(handle->model_id,"sc5235_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;
    handle->RGBIR_id    = SENSOR_RGBIRID;

    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = 2592;
    handle->video_res_supported.res[0].height = 1944;
    handle->video_res_supported.res[0].max_fps= 10;
    handle->video_res_supported.res[0].min_fps= 3;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 2592;
    handle->video_res_supported.res[0].nOutputHeight= 1944;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2592x1944@10fps");


    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

    handle->pCus_sensor_init        = pCus_init_5M10fps_HDR_DOL;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_SEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_SEF; //TBD

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGainHDR_DOL_SEF;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGainHDR_DOL_SEF;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_sef;
    params->expo.vts = vts_10fps_HDR_DOL;
    params->expo.fps = 10;
    params->expo.max_short_exp=240;
    handle->data_prec   = SENSOR_DATAPREC_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc5235_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(sc5235_params));
    params = (sc5235_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc5235_MIPI_HDR_LEF");

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
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
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
    handle->video_res_supported.res[0].width = 2592;
    handle->video_res_supported.res[0].height = 1944;
    handle->video_res_supported.res[0].max_fps= 10;
    handle->video_res_supported.res[0].min_fps= 3;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 2592;
    handle->video_res_supported.res[0].nOutputHeight= 1944;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2592x1944@10fps");

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
    handle->pCus_sensor_GetVideoResNum = NULL;
    handle->pCus_sensor_GetVideoRes       = NULL;
    handle->pCus_sensor_GetCurVideoRes  = NULL;
    handle->pCus_sensor_SetVideoRes       = NULL;
    handle->pCus_sensor_GetOrien          = pCus_GetOrien_hdr_dol_lef;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien_hdr_dol_lef;
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
    //handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_hdr_dol_lef;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_LEF;
    //handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_hdr_dol_lef;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_dol_lef;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    //handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_hdr_dol_lef;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_dol_lef;
    handle->pCus_sensor_setCaliData_gain_linearity= pCus_setCaliData_gain_linearity_hdr_dol_lef;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_lef;

    params->expo.vts = vts_10fps_HDR_DOL;
    params->expo.fps = 10;
    params->reg_dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  SC5235_HDR,
                            cus_camsensor_init_handle,
                            cus_camsensor_init_handle_hdr_dol_sef,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            sc5235_params
                         );


