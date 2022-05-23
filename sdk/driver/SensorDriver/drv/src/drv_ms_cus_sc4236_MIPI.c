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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC4236);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

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

#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI     //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000
#define SENSOR_MAXGAIN      480
#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
u32 Preview_line_period;
u32 vts_30fps;
#define Preview_WIDTH       2304                   //resolution Width when preview
#define Preview_HEIGHT      1296                  //resolution Height when preview
#define Preview_MAX_FPS     30  //25                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x60                   //I2C slave address
#define SENSOR_I2C_SPEED    240000                  //I2C speed,60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG            // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG


static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
//#define ABS(a)     ((a)>(0) ? (a) : (-(a)))
static int g_sensor_ae_min_gain = 1024;
#define ENABLE_NR 1
#define ENABLE_DPC_MANUAL 1

CUS_MCLK_FREQ UseParaMclk(void);

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
        u32 lines;
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
    I2C_ARRAY tPpcharge_reg[2];
    I2C_ARRAY tNr_reg[4];
    I2C_ARRAY tDPC_reg[2];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_mf;
    bool reg_dirty;
    CUS_CAMSENSOR_ORIT cur_orien;
} sc4236_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x32},
    {0x3108, 0x35},
};

const I2C_ARRAY Sensor_init_table_3p6m_30fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},

    {0x36e9,0xc4},

    //close mipi
    //0x3018,0x1f,
    //0x3019,0xff,
    //0x301c,0xb4,

    //close DVP
    {0x301c,0x78}, // [7]:0  [3]:1 to

    {0x3333,0x00},

    {0x3e01,0xc7},
    {0x3e02,0xe0},
    {0x3640,0x02},
    {0x3641,0x02},

    {0x3908,0x11},

    //row noise
    {0x3e06,0x03},
    {0x3e08,0x1f},
    {0x3e09,0x1f},

    //9.29  mipi
    //mipi
    {0x3018,0x33},//[7:5] lane_num-1
    {0x3031,0x0a},//[3:0] bitmode
    {0x3037,0x20},//[6:5] bitsel
    {0x3001,0xFE},//[0] c_y

    {0x4603,0x00},//[0] data_fifo mipi mode
    {0x4837,0x35},//[7:0] pclk period * 2
    //0x4827,0x88,//[7:0] hs_prepare_time[7:0]  20180108

    {0x303f,0x01}, //[7] 1: pll_sclk  0: pll_pclk

    {0x5784,0x10}, //1114 0x04
    {0x5788,0x10}, //1114 0x04


    {0x337f,0x03},
    {0x3368,0x04},
    {0x3369,0x00},
    {0x336a,0x00},
    {0x336b,0x00},
    {0x3367,0x08},
    {0x330e,0x30},

    {0x3320,0x06}, // New ramp offset timing
    //0x3321,0x06,
    {0x3326,0x00},
    {0x331e,0x2f},
    {0x331f,0x2f},
    {0x3308,0x18},
    {0x3303,0x40},
    {0x3309,0x40},

    {0x3306,0x40},

    {0x3301,0x50},
    {0x3638,0x88},

    {0x3307,0x18}, //[3:0]nedd >=7

    {0x3366,0x7c}, // div_rst gap


    //read noise
    {0x3622,0x22},
    {0x3633,0x88},

    {0x3635,0xc2},

    //fwc&Noise FWC4K
    {0x3632,0x18},
    {0x3038,0xcc},
    {0x363c,0x05},
    {0x363d,0x05},
    {0x3637,0x62},
    {0x3638,0x8c},
    {0x330b,0xc8},

    //10.16
    {0x3639,0x09},
    {0x363a,0x1f},

    {0x3638,0x98},
    {0x3306,0x60},

    {0x363b,0x0c},
    //0x3e03,0x03,

    {0x3908,0x15},

    //low power
    {0x3620,0x28},

    //PLL
    {0x36e9,0x24},

    //25fps
    {0x320d,0x8c},

    {0x330b,0xd8},

    //blksun
    {0x3622,0x26},
    {0x3630,0xb8},
    {0x3631,0x88}, //margin 6 levels


    {0x36e9,0x46},
    {0x36ea,0x33},
    {0x36f9,0x06},
    {0x36fa,0xca},

    {0x320c,0x0a}, //2600x1500, 0x0a
    {0x320d,0x28},
    {0x320e,0x05},
    {0x320f,0xdc},

    {0x320a,0x05}, //1440
    {0x320b,0xa0},

    {0x3202,0x00},
    {0x3203,0x30},
    {0x3206,0x05}, //1448 rows seleted
    {0x3207,0xd7},

    {0x3e01,0xbb},
    {0x3e02,0x00},
    {0x330b,0xec},

    //
    {0x3636,0x24},
    {0x3637,0x64},
    {0x3638,0x18},
    {0x3625,0x03},

    //1019
    {0x4837,0x20}, //mipi prepare

    {0x3333,0x20}, //pix_samp all high
    //0x331b,0x83, //snow nosie cancel

    {0x3e06,0x00},
    {0x3e08,0x03},
    {0x3e09,0x10},

    {0x3622,0x06},

    {0x3306,0x50},
    {0x330a,0x01},
    {0x330b,0x10},

    {0x366e,0x08},  // ofs auto en [3]
    {0x366f,0x2f},  // ofs+finegain  real ofs in 0x3687[4:0]



    {0x3235,0x0b}, //group hold position 1115C
    {0x3236,0xb0},

    //for 3.7k
    {0x3637,0x61},
    {0x3306,0x60},
    {0x330b,0x10},


    {0x3633,0x83},
    {0x3301,0x50},

    {0x3630,0xc8},

    {0x330e,0x80}, //for nir


    //pll test
    {0x36e9,0x44},
    {0x36eb,0x0e},
    {0x36ec,0x1e},
    {0x36ed,0x23},

    //sram write
    {0x3f00,0x0f}, //[2]
    {0x3f04,0x05},
    {0x3f05,0x00},

    {0x3962,0x04}, //[0] 1  high temp cal_en

    //1114
    {0x3622,0x16},


    {0x3320,0x06}, // New ramp offset timing
    //0x3321,0x06,
    {0x3326,0x00},
    {0x331e,0x21},
    {0x331f,0x71},
    {0x3308,0x18},
    {0x3303,0x30},
    {0x3309,0x80},


    /////////////////////////////////////high temp/////////////////////////////////////
    //blc max
    {0x3933,0x0a},
    {0x3934,0x08},
    {0x3942,0x02},
    {0x3943,0x0d},
    {0x3940,0x19},
    {0x3941,0x14},
    //blc temp
    {0x3946,0x20}, //kh0
    {0x3947,0x18},//kh1
    {0x3948,0x06}, //kh2
    {0x3949,0x06}, //kh3
    {0x394a,0x18}, //kh4
    {0x394b,0x2c}, //kh5
    {0x394c,0x08},
    {0x394d,0x14},
    {0x394e,0x24},
    {0x394f,0x34},
    {0x3950,0x14},
    {0x3951,0x08},

    {0x3952, 0x78}, //kv0 20171211
    {0x3953, 0x48}, //kv1
    {0x3954, 0x18}, //kv2
    {0x3955, 0x18}, //kv3
    {0x3956, 0x48}, //kv4
    {0x3957, 0x80}, //kv5
    {0x3958, 0x10}, //posv0
    {0x3959, 0x20}, //posv1
    {0x395a, 0x38}, //posv2
    {0x395b, 0x38}, //posv3
    {0x395c, 0x20}, //posv4
    {0x395d, 0x10}, //posv5

    {0x395e,0x24}, // alpha threshold 1115B
    {0x395f,0x00},
    {0x3960,0xc4},
    {0x3961,0xb1},
    {0x3962,0x0d},
    {0x3963,0x44}, //0x80

    //////////////////////////////////////////////////////////////////////////////////////////

    //1115
    {0x3637,0x63}, //add fullwell
    {0x3802,0x01},

    //1123
    {0x3366,0x78}, //row noise optimize

    //1211
    {0x33aa,0x00},//save power

    //20171213
    {0x3208,0x09},//2304
    {0x3209,0x00},
    {0x320a,0x05},//1296
    {0x320b,0x10},

    {0x3200,0x00},//xstar 4
    {0x3201,0x04},
    {0x3202,0x00},//ystar 124
    {0x3203,0x7c},
    {0x3211,0x04}, // x shift 4
    {0x3213,0x04}, // y shift 4

    {0x3204,0x09},//xend 4+8+2304-1=2315
    {0x3205,0x0b},
    {0x3206,0x05},//yend 124+8+1296-1=1427
    {0x3207,0x93},

    //20171225
    {0x3222,0x29},
    {0x3901,0x02},

    //20171226  bypass txvdd
    {0x3635,0xe2},
    //20171226B
    {0x3963,0x80},//edges brighting when high temp
    {0x3e1e,0x34},// digital finegain enable


    ////////////20180201
    {0x330f,0x04},
    {0x3310,0x20},//0201B
    {0x3314,0x04},
    {0x330e,0x50},//0201B
    ///////////////////////////

    //20180208
    {0x4827,0x46},// [40ns+4/585M,85ns]
    {0x3650,0x42},

    //20180508
    {0x5000,0x06},  //dpc enable
    {0x5780,0x7f},  //auto dpc
    {0x5781,0x04},  //white 1x
    {0x5782,0x03},  //      4x
    {0x5783,0x02},  //      8x
    {0x5784,0x01},  //      128x
    {0x5785,0x18},  //black 1x
    {0x5786,0x10},  //      4x
    {0x5787,0x08},  //      8x
    {0x5788,0x02},  //      128x
    {0x5789,0x20},  //Gain_thre1=4x
    {0x578a,0x30},  //Gain_thre2=8x

    //20180517  high temperature logic
    {0x3962,0x09},
    {0x3940,0x17},
    {0x3946,0x48},
    {0x3947,0x20},
    {0x3948,0x0A},
    {0x3949,0x10},
    {0x394A,0x28},
    {0x394B,0x48},
    {0x394E,0x28},
    {0x394F,0x50},
    {0x3950,0x20},
    {0x3951,0x10},
    {0x3952,0x70},
    {0x3953,0x40},
    {0x3956,0x40},

    //20180529 20180530
    {0x3940,0x15},
    {0x3934,0x16},
    {0x3943,0x20},
    {0x3952,0x68},
    {0x3953,0x38},
    {0x3956,0x38},
    {0x3957,0x78},
    {0x394F,0x40},

    {0x3301,0x1e},  //[a,21]
    {0x3633,0x23},
    {0x3630,0x80},
    {0x3622,0xf6},
    {0x3651,0x05},
    {0x0100,0x01},
};

const I2C_ARRAY Sensor_init_table_1920x1536m_25fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0xc4},
    {0x3001,0xfe},
    {0x3018,0x33},
    {0x301c,0x78},
    {0x3031,0x0a},
    {0x3037,0x20},
    {0x3038,0xcc},
    {0x303f,0x01},
    {0x3200,0x00},
    {0x3201,0xc4},
    {0x3202,0x00},
    {0x3203,0x04},
    {0x3204,0x08},
    {0x3205,0x4b},
    {0x3206,0x06},
    {0x3207,0x0b},
    {0x3208,0x07},
    {0x3209,0x80},
    {0x320a,0x06},
    {0x320b,0x00},
    {0x320c,0x0b},
    {0x320d,0x40},
    {0x320e,0x06},
    {0x320f,0x59},
    {0x3211,0x04},
    {0x3213,0x04},
    {0x3222,0x29},
    {0x3235,0x0c},
    {0x3236,0xb0},
    {0x3301,0x1e},
    {0x3303,0x30},
    {0x3306,0x60},
    {0x3307,0x18},
    {0x3308,0x18},
    {0x3309,0x80},
    {0x330a,0x01},
    {0x330b,0x10},
    {0x330e,0x50},
    {0x330f,0x04},
    {0x3310,0x20},
    {0x3314,0x04},
    {0x331e,0x21},
    {0x331f,0x71},
    {0x3320,0x06},
    {0x3326,0x00},
    {0x3333,0x20},
    {0x3366,0x78},
    {0x3367,0x08},
    {0x3368,0x04},
    {0x3369,0x00},
    {0x336a,0x00},
    {0x336b,0x00},
    {0x337f,0x03},
    {0x33aa,0x00},
    {0x3620,0x28},
    {0x3622,0xf6},
    {0x3625,0x03},
    {0x3630,0x80},
    {0x3631,0x88},
    {0x3632,0x18},
    {0x3633,0x23},
    {0x3635,0xe2},
    {0x3636,0x24},
    {0x3637,0x63},
    {0x3638,0x18},
    {0x3639,0x09},
    {0x363a,0x1f},
    {0x363b,0x0c},
    {0x363c,0x05},
    {0x363d,0x05},
    {0x3640,0x02},
    {0x3641,0x02},
    {0x3650,0x42},
    {0x366e,0x08},
    {0x366f,0x2f},
    {0x36ea,0x33},
    {0x36eb,0x0e},
    {0x36ec,0x1e},
    {0x36ed,0x23},
    {0x36f9,0x06},
    {0x36fa,0xca},
    {0x3802,0x01},
    {0x3901,0x02},
    {0x3908,0x15},
    {0x3933,0x0a},
    {0x3934,0x16},
    {0x3940,0x15},
    {0x3941,0x14},
    {0x3942,0x02},
    {0x3943,0x20},
    {0x3946,0x48},
    {0x3947,0x20},
    {0x3948,0x0a},
    {0x3949,0x10},
    {0x394a,0x28},
    {0x394b,0x48},
    {0x394c,0x08},
    {0x394d,0x14},
    {0x394e,0x28},
    {0x394f,0x40},
    {0x3950,0x20},
    {0x3951,0x10},
    {0x3952,0x68},
    {0x3953,0x38},
    {0x3954,0x18},
    {0x3955,0x18},
    {0x3956,0x38},
    {0x3957,0x78},
    {0x3958,0x10},
    {0x3959,0x20},
    {0x395a,0x38},
    {0x395b,0x38},
    {0x395c,0x20},
    {0x395d,0x10},
    {0x395e,0x24},
    {0x395f,0x00},
    {0x3960,0xc4},
    {0x3961,0xb1},
    {0x3962,0x09},
    {0x3963,0x80},
    {0x3e00,0x00},
    {0x3e01,0xca},
    {0x3e02,0x00},
    {0x3e06,0x00},
    {0x3e07,0x80},
    {0x3e08,0x03},
    {0x3e09,0x10},
    {0x3e1e,0x34},
    {0x3f00,0x0f},
    {0x3f04,0x05},
    {0x3f05,0x7c},
    {0x4603,0x00},
    {0x4827,0x46},
    {0x4837,0x22},
    {0x5000,0x06},
    {0x5780,0x7f},
    {0x5781,0x04},
    {0x5782,0x03},
    {0x5783,0x02},
    {0x5784,0x01},
    {0x5785,0x18},
    {0x5786,0x10},
    {0x5787,0x08},
    {0x5788,0x02},
    {0x5789,0x20},
    {0x578a,0x30},
    {0x36e9,0x44},
    {0x0100,0x01},
};

const I2C_ARRAY Sensor_init_table_1344x1344m_25fps[] =
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0xc4},
    {0x3001,0xfe},
    {0x3018,0x33},
    {0x301c,0x78},
    {0x3031,0x0a},
    {0x3037,0x20},
    {0x3038,0xcc},
    {0x303f,0x01},
    {0x3200,0x01},
    {0x3201,0xe4},
    {0x3202,0x00},
    {0x3203,0x64},
    {0x3204,0x07},
    {0x3205,0x2b},
    {0x3206,0x05},
    {0x3207,0xab},
    {0x3208,0x05},
    {0x3209,0x40},
    {0x320a,0x05},
    {0x320b,0x40},
    {0x320c,0x0b},
    {0x320d,0x40},
    {0x320e,0x06},
    {0x320f,0x59},
    {0x3211,0x04},
    {0x3213,0x04},
    {0x3222,0x29},
    {0x3235,0x0c},
    {0x3236,0xb0},
    {0x3301,0x1e},
    {0x3303,0x30},
    {0x3306,0x60},
    {0x3307,0x18},
    {0x3308,0x18},
    {0x3309,0x80},
    {0x330a,0x01},
    {0x330b,0x10},
    {0x330e,0x50},
    {0x330f,0x04},
    {0x3310,0x20},
    {0x3314,0x04},
    {0x331e,0x21},
    {0x331f,0x71},
    {0x3320,0x06},
    {0x3326,0x00},
    {0x3333,0x20},
    {0x3366,0x78},
    {0x3367,0x08},
    {0x3368,0x04},
    {0x3369,0x00},
    {0x336a,0x00},
    {0x336b,0x00},
    {0x337f,0x03},
    {0x33aa,0x00},
    {0x3620,0x28},
    {0x3622,0xf6},
    {0x3625,0x03},
    {0x3630,0x80},
    {0x3631,0x88},
    {0x3632,0x18},
    {0x3633,0x23},
    {0x3635,0xe2},
    {0x3636,0x24},
    {0x3637,0x63},
    {0x3638,0x18},
    {0x3639,0x09},
    {0x363a,0x1f},
    {0x363b,0x0c},
    {0x363c,0x05},
    {0x363d,0x05},
    {0x3640,0x02},
    {0x3641,0x02},
    {0x3650,0x42},
    {0x366e,0x08},
    {0x366f,0x2f},
    {0x36ea,0x33},
    {0x36eb,0x0e},
    {0x36ec,0x1e},
    {0x36ed,0x23},
    {0x36f9,0x06},
    {0x36fa,0xca},
    {0x3802,0x01},
    {0x3901,0x02},
    {0x3908,0x15},
    {0x3933,0x0a},
    {0x3934,0x16},
    {0x3940,0x15},
    {0x3941,0x14},
    {0x3942,0x02},
    {0x3943,0x20},
    {0x3946,0x48},
    {0x3947,0x20},
    {0x3948,0x0a},
    {0x3949,0x10},
    {0x394a,0x28},
    {0x394b,0x48},
    {0x394c,0x08},
    {0x394d,0x14},
    {0x394e,0x28},
    {0x394f,0x40},
    {0x3950,0x20},
    {0x3951,0x10},
    {0x3952,0x68},
    {0x3953,0x38},
    {0x3954,0x18},
    {0x3955,0x18},
    {0x3956,0x38},
    {0x3957,0x78},
    {0x3958,0x10},
    {0x3959,0x20},
    {0x395a,0x38},
    {0x395b,0x38},
    {0x395c,0x20},
    {0x395d,0x10},
    {0x395e,0x24},
    {0x395f,0x00},
    {0x3960,0xc4},
    {0x3961,0xb1},
    {0x3962,0x09},
    {0x3963,0x80},
    {0x3e01,0xca},
    {0x3e02,0x00},
    {0x3e06,0x00},
    {0x3e08,0x03},
    {0x3e09,0x10},
    {0x3e1e,0x34},
    {0x3f00,0x0f},
    {0x3f04,0x05},
    {0x3f05,0x7c},
    {0x4603,0x00},
    {0x4827,0x46},
    {0x4837,0x22},
    {0x5000,0x06},
    {0x5780,0x7f},
    {0x5781,0x04},
    {0x5782,0x03},
    {0x5783,0x02},
    {0x5784,0x01},
    {0x5785,0x18},
    {0x5786,0x10},
    {0x5787,0x08},
    {0x5788,0x02},
    {0x5789,0x20},
    {0x578a,0x30},
    {0x36e9,0x44},
    {0x0100,0x01},
};

const I2C_ARRAY mirror_reg[] =
{
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
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

const static I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x80},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x10}, //low bit, 0x10 - 0x3e0, step 1/16
};

const I2C_ARRAY expo_reg[] = {
    {0x3e00, 0x00}, //expo [20:17]
    {0x3e01, 0x30}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
};

const I2C_ARRAY vts_reg[] = {
    {0x320e, 0x05},
    {0x320f, 0xdc},
};

const I2C_ARRAY ppcharge_reg[] = {
    {0x3314,0x04},
    {0x330e,0x50},
};

#if ENABLE_NR
const I2C_ARRAY nr_reg[] = {
    {0x3301,0x1e},  //[a,21]
    {0x3633,0x23},
    {0x3630,0x80},
    {0x3622,0xf6},
};
#endif

#if ENABLE_DPC_MANUAL
const I2C_ARRAY DPC_reg[] = {
    {0x5781,0x04},
    {0x5785,0x18},
};
#endif

const I2C_ARRAY PatternTbl[] = {
    {0x4501,0xc0}, //colorbar pattern , bit 7 to enable
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
#define SENSOR_NAME sc4236
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
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);////pwd always high
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    //sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    //sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    //sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    SENSOR_USLEEP(2000);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    //sensor_if->Set3ATaskOrder(handle, def_order);
    // pure power on
    //ISP_config_io(handle);
   // sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
   // SENSOR_USLEEP(5000);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc4236_params *params = (sc4236_params *)handle->private_data;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
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

static int sc4236_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init_linear_3p6m_30fps(ms_cus_sensor *handle)
{
    sc4236_params *params = (sc4236_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_3p6m_30fps);i++)
    {
        if(Sensor_init_table_3p6m_30fps[i].reg==0x0100 && 0x01 == Sensor_init_table_3p6m_30fps[i].data)
        {
            sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
            sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
            sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
            SENSOR_MSLEEP(50);
        }

        if(Sensor_init_table_3p6m_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_3p6m_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_3p6m_30fps[i].reg, Sensor_init_table_3p6m_30fps[i].data) != SUCCESS)
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

    //DPC_MODE  default mode is AUTO dpc
#if ENABLE_DPC_MANUAL
    SensorReg_Write(0x5780,0xff);
#endif

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_linear_1920x1536m_25fps(ms_cus_sensor *handle)
{
    sc4236_params *params = (sc4236_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_1920x1536m_25fps);i++)
    {
        if(Sensor_init_table_1920x1536m_25fps[i].reg==0x0100 && 0x01 == Sensor_init_table_1920x1536m_25fps[i].data)
        {
            sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
            sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
            sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
            SENSOR_MSLEEP(50);
        }

        if(Sensor_init_table_1920x1536m_25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_1920x1536m_25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_1920x1536m_25fps[i].reg, Sensor_init_table_1920x1536m_25fps[i].data) != SUCCESS)
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

    //DPC_MODE  default mode is AUTO dpc
#if ENABLE_DPC_MANUAL
    SensorReg_Write(0x5780,0xff);
#endif

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_linear_1344x1344m_25fps(ms_cus_sensor *handle)
{
    sc4236_params *params = (sc4236_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    int i,cnt;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_1344x1344m_25fps);i++)
    {
        if(Sensor_init_table_1344x1344m_25fps[i].reg==0x0100 && 0x01 == Sensor_init_table_1344x1344m_25fps[i].data)
        {
            sensor_if->SetCSI_Clk(handle->i2c_bus->nSensorID, CUS_CSI_CLK_216M);
            sensor_if->SetCSI_Lane(handle->i2c_bus->nSensorID, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
            sensor_if->SetCSI_LongPacketType(handle->i2c_bus->nSensorID, 0, 0x1C00, 0);
            SENSOR_MSLEEP(50);
        }

        if(Sensor_init_table_1344x1344m_25fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_1344x1344m_25fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_1344x1344m_25fps[i].reg, Sensor_init_table_1344x1344m_25fps[i].data) != SUCCESS)
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

    //DPC_MODE  default mode is AUTO dpc
#if ENABLE_DPC_MANUAL
    SensorReg_Write(0x5780,0xff);
#endif

    pCus_SetOrien(handle, params->cur_orien);
   // pr_info("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
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
    sc4236_params *params = (sc4236_params *)handle->private_data;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0: //"2304x1536@30fps"
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_linear_3p6m_30fps;
            vts_30fps=1500;
            params->expo.vts = vts_30fps;
            params->expo.fps = 30;
            Preview_line_period  = 22222;
            break;
        case 1: //"1920x1536@25fps"
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init_linear_1920x1536m_25fps;
            vts_30fps=1625;
            params->expo.vts = vts_30fps;
            params->expo.fps = 25;
            Preview_line_period  = 24615;
            break;
        case 2: //"1344x1344@25fps"
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init_linear_1344x1344m_25fps;
            vts_30fps=1625;
            params->expo.vts = vts_30fps;
            params->expo.fps = 25;
            Preview_line_period  = 24615;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sc4236_params *params = (sc4236_params *)handle->private_data;
    sen_data = params->tMirror_reg[0].data;
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
   sc4236_params *params = (sc4236_params *)handle->private_data;

  switch(orit) {
    case CUS_ORIT_M0F0:
    if (params->tMirror_reg[0].data) {
        params->tMirror_reg[0].data = 0;
        params->reg_mf = true;
    }
    break;
    case CUS_ORIT_M1F0:
    if (params->tMirror_reg[0].data!=6) {
        params->tMirror_reg[0].data = 6;
        params->reg_mf = true;
    }
    break;
    case CUS_ORIT_M0F1:
    if (params->tMirror_reg[0].data!=0x60) {
        params->tMirror_reg[0].data = 0x60;
        params->reg_mf = true;
    }
    break;
    case CUS_ORIT_M1F1:
    if (params->tMirror_reg[0].data!=0x66) {
        params->tMirror_reg[0].data = 0x66;
        params->reg_mf = true;
    }
    break;
  }

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", orit);

  return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc4236_params *params = (sc4236_params *)handle->private_data;
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
    sc4236_params *params = (sc4236_params *)handle->private_data;
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

    if ((params->expo.lines) >  2 * (params->expo.vts)-4) {
        vts = ((params->expo.lines)+5)/2;
    }
    else
        vts = params->expo.vts;

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
    sc4236_params *params = (sc4236_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
		if(params->reg_mf)
		{
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
			params->reg_mf = false;
		}
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
#if ENABLE_NR
            SensorReg_Write(0x3812,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tPpcharge_reg, sizeof(ppcharge_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tDPC_reg, sizeof(DPC_reg)/sizeof(I2C_ARRAY));
            SensorReg_Write(0x3812,0x30);
#endif

            params->reg_dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    int rc=0;
    u32 lines = 0;
    sc4236_params *params = (sc4236_params *)handle->private_data;
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
    sc4236_params *params = (sc4236_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, params->tExpo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-4) {
        vts = (half_lines+5)/2;
    }
    else
        vts=params->expo.vts;
    params->expo.lines = half_lines;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;
//  printf("===================================================================\n");
//  printf("us = %d  half_lines = %x params->expo.vts = %x\n",us, half_lines, params->expo.vts);
//  printf("===================================================================\n");
    params->tExpo_reg[0].data = (half_lines>>16) & 0x0f;
    params->tExpo_reg[1].data =  (half_lines>>8) & 0xff;
    params->tExpo_reg[2].data = (half_lines>>0) & 0xf0;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    if (((half_lines >>8) & 0x0fff) < 5)
    {
        params->tPpcharge_reg[0].data = 0x14;
        params->tPpcharge_reg[1].data = 0x40;
    }else if (((half_lines >>8) & 0x0fff) > 0xa)
    {
        params->tPpcharge_reg[0].data = 0x04;
        params->tPpcharge_reg[1].data = 0x50;
    }
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
/*     u64 temp_gain;
    u8 Dgain = 1,  Coarse_gain = 1;
    u64 Fine_again = 1000,Fine_dgain = 1000;

    Fine_again =  (params->tGain_reg[3].data)>>4;
    Fine_dgain = (params->tGain_reg[1].data)>>9;
    Coarse_gain = ((params->tGain_reg[2].data&0x1C)>>2) +1;
    Dgain = ((params->tGain_reg[0].data&0x0f) + 1);

    temp_gain = Fine_again * 1024;
    *gain = (temp_gain * Coarse_gain * Dgain * Fine_dgain);
 */
    return rc;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    sc4236_params *params = (sc4236_params *)handle->private_data;

    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u64 Fine_againx16 = 16,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x00},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x10},
    };
    memcpy(gain_reg_temp, params->tGain_reg, sizeof(gain_reg));

    if (gain < 1024) {
        gain = 1024;
    } else if (gain > SENSOR_MAXGAIN*1024) {
        gain = SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        params->tNr_reg[0].data = 0x1e;
        params->tNr_reg[1].data = 0x23;
        params->tNr_reg[2].data = 0x80;
        params->tNr_reg[3].data = 0xf6;
    } else if (gain < 4*1024) {
        params->tNr_reg[0].data = 0x50;
        params->tNr_reg[1].data = 0x23;
        params->tNr_reg[2].data = 0x80;
        params->tNr_reg[3].data = 0xf6;
    } else if (gain <= 15872) {
        params->tNr_reg[0].data = 0x50;
        params->tNr_reg[1].data = 0x23;
        params->tNr_reg[2].data = 0x80;
        params->tNr_reg[3].data = 0xf6;
    } else {
        params->tNr_reg[0].data = 0xb8;
        params->tNr_reg[1].data = 0x43;
        params->tNr_reg[2].data = 0x82;
        params->tNr_reg[3].data = 0x16;
    }
#endif

#if ENABLE_DPC_MANUAL
    if (gain < 10*1024) {
        params->tDPC_reg[0].data = 0x04;
        params->tDPC_reg[1].data = 0x18;
    } else {
        params->tDPC_reg[0].data = 0x02;
        params->tDPC_reg[1].data = 0x08;
    }
#endif

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 3;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 7;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0xf;
    }
    else if (gain <=  15872)
    {
        Dgain = 1;      Fine_dgainx128 = 128;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  31 * 1024)
    {
        Dgain = 1;      Fine_againx16 = 31;    Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  62 * 1024)
    {
        Dgain = 2;      Fine_againx16 = 31;    Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain < 124 * 1024)
    {
        Dgain = 4;      Fine_againx16 = 31;    Coarse_gain = 8;
        Dgain_reg = 3;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
    }
        else if (gain <248 * 1024)
    {
        Dgain = 8;      Fine_againx16 = 31;    Coarse_gain = 8;
        Dgain_reg = 7;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
    }
     else if (gain <=(SENSOR_MAXGAIN * 1024))
    {
        Dgain = 16;      Fine_againx16 = 31;    Coarse_gain = 8;
        Dgain_reg = 0xf;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
    }

    if (gain <=  15872)
    {
        Fine_againx16 =(gain * 2)/(Dgain * Coarse_gain * Fine_dgainx128);
        Fine_dgainx128 =(gain * 2)/(Dgain * Coarse_gain * Fine_againx16);
        Fine_again_reg = Fine_againx16;
        Fine_dgain_reg = Fine_dgainx128;
    }
    else
    {
        //Fine_dgainx128 =gain*1000 /((Dgain * Coarse_gain * Fine_againx16*1024)/ 1000);
        Fine_dgain_reg = abs((gain*2)/(Dgain * Coarse_gain * Fine_againx16));
    }

  //   pr_info("[%s]  gain : %d,%lld,%lld,%d, %d\n\n", __FUNCTION__,gain,Fine_againx16,Fine_dgainx128,Dgain,Coarse_gain);
  //   pr_info("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Coarse_gain_reg,Fine_dgain_reg,Dgain_reg);

    params->tGain_reg[3].data = Fine_again_reg;
    params->tGain_reg[2].data = Coarse_gain_reg;
    params->tGain_reg[1].data = Fine_dgain_reg;
    params->tGain_reg[0].data = Dgain_reg & 0xF;

    //printk("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Coarse_gain_reg,Fine_dgain_reg,Dgain_reg);
    for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tGain_reg[i].data != gain_reg_temp[i].data)
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

static int sc4236_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period * 3) / 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc4236_params *params;
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
    params = (sc4236_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
    memcpy(params->tPpcharge_reg, ppcharge_reg, sizeof(ppcharge_reg));
    memcpy(params->tDPC_reg, DPC_reg, sizeof(DPC_reg));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"sc4236_MIPI");

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
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 0;
    handle->video_res_supported.res[0].nOutputHeight= 0;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2304x1536@30fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = 1920;
    handle->video_res_supported.res[1].height = 1536;
    handle->video_res_supported.res[1].max_fps= 25;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 0;
    handle->video_res_supported.res[1].nOutputHeight= 0;
    sprintf(handle->video_res_supported.res[1].strResDesc, "1920x1536@25fps");

    handle->video_res_supported.num_res = 3;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[2].width = 1344;
    handle->video_res_supported.res[2].height = 1344;
    handle->video_res_supported.res[2].max_fps= 25;
    handle->video_res_supported.res[2].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[2].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[2].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[2].nOutputWidth= 0;
    handle->video_res_supported.res[2].nOutputHeight= 0;
    sprintf(handle->video_res_supported.res[2].strResDesc, "1344x1344@25fps");

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
    handle->pCus_sensor_init        = pCus_init_linear_3p6m_30fps;

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
    handle->pCus_sensor_SetPatternMode = sc4236_SetPatternMode;
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
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = sc4236_GetShutterInfo;
    //params->expo.vts=vts_30fps;
    //params->expo.fps = 30;
    params->expo.lines = 1000;
    params->reg_dirty = false;
    params->reg_mf = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  SC4236,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            sc4236_params
                         );

