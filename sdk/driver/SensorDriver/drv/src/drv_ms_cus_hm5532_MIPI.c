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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(HM5532);

#define SENSOR_MIPI_LANE_NUM (4)
#define SENSOR_MIPI_HSYNC_MODE PACKET_FOOTER_EDGE
#define SENSOR_MODEL_ID     "HM5532_MIPI"
#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_DELAY   0x1212                  //CFG
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000
#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
//HDR
#define SENSOR_CHANNEL_MODE_HDR CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR
//#define SENSOR_MIPI_HSYNC_MODE PACKET_FOOTER_EDGE//PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR PACKET_FOOTER_EDGE
#define SENSOR_BAYERID_HDR      CUS_BAYER_BG//CUS_BAYER_GR
#define SENSOR_DATAPREC_HDR            CUS_DATAPRECISION_10


#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     32563//(31.8*1024)                  // max sensor gain, again 15.5x, dgain 4x
#define SENSOR_MIN_GAIN      (1 * 1024)


#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ         //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_27M

#define Preview_line_period 15686                   //1280*2*1125
#define vts_30fps           (2125)                    //for 30fps

#define Preview_WIDTH       2720                    //resolution Width when preview
#define Preview_HEIGHT      2080                   //resolution Height when preview
//HDR
#define Preview_MAX_FPS_HDR     25                      //fastest preview FPS
#define Preview_MIN_FPS_HDR     8                       //slowest preview FPS

#define Preview_WIDTH_HDR  2560                  //resolution Width when preview
#define Preview_HEIGHT_HDR      1920                   //resolution Height when preview
#define vts_25fps_HDR_4lane        (1152)//(1152)//3124
#define vts_30fps_HDR_2lane        (1152)//(1152)//3124

#define SENSOR_MAX_GAIN_HDR     (64*1024)                  // max sensor gain, again 15.5x, dgain 4x
#define SENSOR_MIN_GAIN_HDR      (1 * 1024)

//#define Preview_HEIGHT_HDR_DOL        2080

#define Preview_MAX_FPS     30                      //fastest preview FPS
#define Preview_MIN_FPS     8                       //slowest preview FPS

#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR     0x48
#define SENSOR_I2C_FMT      I2C_FMT_A16D8
#define SENSOR_I2C_SPEED    200000
#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG
#define SENSOR_RST_POL      CUS_CLK_POL_NEG
#define SENSOR_PCLK_POL     CUS_CLK_POL_NEG        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

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
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty;
    bool orien_dirty;
    I2C_ARRAY tVts_reg[4];
    I2C_ARRAY tGain_reg[5];
    I2C_ARRAY tExpo_reg[4];
    I2C_ARRAY tGain_hdr_lef_reg[5];
    I2C_ARRAY tGain_hdr_sef_reg[5];
    I2C_ARRAY tExpo_hdr_lef_reg[4];
    I2C_ARRAY tExpo_hdr_sef_reg[4];


} hm5532_params;
typedef struct {
    float total_gain;
    unsigned short reg_val;
} Gain_ARRAY;


const I2C_ARRAY Sensor_id_table[] =
{
    {0x0000, 0x55},                 // HM2140 16b RO ID,
    {0x0001, 0x30},
};

I2C_ARRAY mirror_reg[] = {
    {0x0101, 0x00},    // HV mirror/flip
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};
#if 0
static Gain_ARRAY gain_table[]={
    {1.0000  ,0x0},
    {1.0625  ,0x1},
    {1.1250  ,0x2},
    {1.1875  ,0x3},
    {1.2500  ,0x4},
    {1.3125  ,0x5},
    {1.3750  ,0x6},
    {1.4375  ,0x7},
    {1.5000  ,0x8},
    {1.5625  ,0x9},
    {1.6250  ,0xa},
    {1.6875  ,0xb},
    {1.7500  ,0xc},
    {1.8125  ,0xd},
    {1.8750  ,0xe},
    {1.9375  ,0xf},
    {2.0000  ,0x10},
    {2.1250  ,0x12},
    {2.2500  ,0x14},
    {2.3750  ,0x16},
    {2.5000  ,0x18},
    {2.6250  ,0x1a},
    {2.7500  ,0x1c},
    {2.8750  ,0x1e},
    {3.0000  ,0x20},
    {3.1250  ,0x22},
    {3.2500  ,0x24},
    {3.3750  ,0x26},
    {3.5000  ,0x28},
    {3.6250  ,0x2a},
    {3.7500  ,0x2c},
    {3.8750  ,0x2e},
    {4.0000  ,0x30},
    {4.2500  ,0x34},
    {4.5000  ,0x38},
    {4.7500  ,0x3c},
    {5.0000  ,0x40},
    {5.2500  ,0x44},
    {5.5000  ,0x48},
    {5.7500  ,0x4c},
    {6.0000  ,0x50},
    {6.2500  ,0x54},
    {6.5000  ,0x58},
    {6.7500  ,0x5c},
    {7.0000  ,0x60},
    {7.2500  ,0x64},
    {7.5000  ,0x68},
    {7.7500  ,0x6c},
    {8.0000  ,0x70},
    {8.5000  ,0x78},
    {9.0000  ,0x80},
    {9.5000  ,0x88},
    {10.0000 ,0x90},
    {10.5000 ,0x98},
    {11.0000 ,0xa0},
    {11.5000 ,0xa8},
    {12.0000 ,0xb0},
    {12.5000 ,0xb8},
    {13.0000 ,0xc0},
    {13.5000 ,0xc8},
    {14.0000 ,0xd0},
    {14.5000 ,0xd8},
    {15.0000 ,0xe0},
    {15.5000 ,0xe8},
};
#endif

const I2C_ARRAY gain_reg[] = {
    {0x0205, 0x00},    // again,   1-15.5x
    {0x020e, 0x01},    // dgain_h, 1-3x
    {0x020f, 0x00},    // dgain_l,
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const I2C_ARRAY expo_reg[] = {
    {0x0202, 0x01},    // shutter[15:8]
    {0x0203, 0xf6},    // shutter[7:0]
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const I2C_ARRAY vts_reg[] = {
    {0x0340, 0x08},    // Frame_Line[15:8]
    {0x0341, 0x4D},    // Frame_Line[7:0]
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const I2C_ARRAY Sensor_init_table_2lane[] =
{
    {0x0103, 0x00},
    {0x0100, 0x02},
    {0xffff,  100},
    {0x0303, 0x02},
    {0x0305, 0x0C},
    {0x0307, 0x4A},
    {0x0309, 0x00},
    {0x030D, 0x0C},
    {0x030F, 0x71},
    {0x0304, 0xA9},
    {0x231C, 0x70},
    {0x2300, 0x80},
    {0x2301, 0x23},
    {0x2302, 0x30},
    {0x2303, 0x01},
    {0x2305, 0xB0},
    {0x2306, 0x08},
    {0x2308, 0x70},
    {0x2309, 0x36},
    {0x230A, 0x05},
    {0x230B, 0x37},
    {0x230C, 0x11},
    {0x230D, 0x37},
    {0x230F, 0x01},
    {0x2310, 0x01},
    {0x2313, 0x3B},
    {0x2320, 0x04},
    {0x2321, 0x17},
    {0x2322, 0x27},
    {0x2323, 0x27},
    {0x2324, 0x37},
    {0x2325, 0x47},
    {0x2326, 0x57},
    {0x2327, 0x08},
    {0x2328, 0x08},
    {0x2329, 0x08},
    {0x232A, 0x09},
    {0x232B, 0x05},
    {0x232C, 0x05},
    {0x232D, 0x05},
    {0x2330, 0x14},
    {0x2331, 0x27},
    {0x2332, 0x27},
    {0x2333, 0x37},
    {0x2334, 0x47},
    {0x2335, 0x57},
    {0x2336, 0x57},
    {0x2337, 0x08},
    {0x2338, 0x08},
    {0x2339, 0x09},
    {0x233A, 0x09},
    {0x233B, 0x05},
    {0x233C, 0x05},
    {0x233D, 0x06},
    {0x2340, 0xF0},
    {0x2341, 0x03},
    {0x2342, 0x19},
    {0x2344, 0xF0},
    {0x2345, 0x0F},
    {0x2346, 0x15},
    {0x2348, 0xF0},
    {0x2349, 0x0F},
    {0x234A, 0x15},
    {0x2350, 0xF0},
    {0x2351, 0x0F},
    {0x2352, 0x15},
    {0x2354, 0xF7},
    {0x2355, 0x0F},
    {0x2356, 0x15},
    {0x2358, 0xF7},
    {0x2359, 0x0F},
    {0x235A, 0x11},
    {0x238E, 0x01},
    {0x23A0, 0x02},
    {0x23A1, 0x01},
    {0x23A3, 0xB0},
    {0x23A4, 0x00},
    {0x226A, 0x02},
    {0x23A5, 0x3F},
    {0x23A6, 0xB2},
    {0x23A7, 0x0B},
    {0x23A8, 0x1F},
    {0x23A9, 0x00},
    {0x23AB, 0x20},
    {0x23AC, 0x0F},
    {0x23AD, 0x0F},
    {0x23AF, 0xF0},
    {0x23B0, 0x02},
    {0x23B1, 0x84},
    {0x23B2, 0x40},
    {0x23B6, 0x05},
    {0x23B8, 0x07},
    {0x23BD, 0x02},
    {0x23BF, 0x09},
    {0x23C0, 0x08},
    {0x23C1, 0x11},
    {0x23C2, 0x00},
    {0x23C3, 0x08},
    {0x23C6, 0x55},
    {0x23C7, 0x55},
    {0x23C8, 0x05},
    {0x23CA, 0x3F},
    {0x0202, 0x03},
    {0x0203, 0x08},
    {0x0205, 0x00},
    {0x0206, 0x01},
    {0x0207, 0x00},
    {0x0208, 0x01},
    {0x0209, 0x00},
    {0x020A, 0x01},
    {0x020B, 0x00},
    {0x020C, 0x01},
    {0x020D, 0x00},
    {0x020E, 0x01},
    {0x020F, 0x00},
    {0x0350, 0xFF},
    {0x2200, 0x00},
    {0x2201, 0x00},
    {0x2202, 0x40},
    {0x2204, 0x00},
    {0x2205, 0x20},
    {0x2206, 0x00},
    {0x2207, 0x10},
    {0x2208, 0x10},
    {0x2209, 0x20},
    {0x220A, 0x02},
    {0x220B, 0x00},
    {0x220C, 0x01},
    {0x220D, 0x00},
    {0x2210, 0x00},
    {0x2220, 0x00},
    {0x2221, 0x00},
    {0x2222, 0x40},
    {0x2230, 0x00},
    {0x2400, 0x20},
    {0x2401, 0x30},
    {0x2402, 0x20},
    {0x2403, 0x08},
    {0x2404, 0x40},
    {0x2405, 0x08},
    {0x2406, 0x30},
    {0x2407, 0x43},
    {0x2408, 0x50},
    {0x240B, 0x10},
    {0x2412, 0x02},
    {0x2413, 0x0F},
    {0x2460, 0x00},
    {0x2468, 0x03},
    {0x2443, 0x01},
    {0x2444, 0x30},
    {0x2445, 0x20},
    {0x2446, 0x20},
    {0x2481, 0x1B},
    {0x2482, 0x20},
    {0x2483, 0x20},
    {0x2484, 0x0F},
    {0x2480, 0x00},
    {0x2485, 0x14},
    {0x2486, 0x0A},
    {0x2487, 0x16},
    {0x2488, 0x2E},
    {0x2489, 0x3F},
    {0x247C, 0x1F},
    {0x247D, 0x04},
    {0x2490, 0x00},
    {0x2880, 0x62},
    {0x2881, 0x10},
    {0x2882, 0x09},
    {0x2883, 0x00},
    {0x2884, 0x1F},
    {0x2885, 0x0F},
    {0x2886, 0x03},
    {0x2882, 0x39},
    {0x2883, 0x02},
    {0x0340, 0x08},
    {0x0341, 0x4D},
    {0x0342, 0x0C},
    {0x0343, 0x10},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0348, 0x0A},
    {0x0349, 0x9F},
    {0x0346, 0x00},
    {0x0347, 0x00},
    {0x034A, 0x08},
    {0x034B, 0x1F},
    {0x0351, 0x0A},
    {0x0352, 0xA0},
    {0x0353, 0x08},
    {0x0354, 0x20},
    {0x0355, 0x00},
    {0x0356, 0x00},
    {0x0357, 0x00},
    {0x0358, 0x00},
    {0x0370, 0x00},
    {0x0371, 0x00},
    {0x0383, 0x01},
    {0x0387, 0x01},
    {0x0390, 0x00},
    {0x2800, 0x01},
    {0x2810, 0x07},
    {0x2811, 0x07},
    {0x2812, 0x06},
    {0x2813, 0x0D},
    {0x2814, 0x08},
    {0x2815, 0x1E},
    {0x2816, 0x07},
    {0x281B, 0x0A},
    {0x281C, 0x0A},
    {0x281D, 0x00},
    {0x281E, 0x0D},
    {0x2828, 0x00},
    {0x2829, 0x10},
    {0x2831, 0xCE},
    {0x2832, 0x2B},
    {0x2833, 0x01},
    {0x2805, 0x04},
    {0x23BB, 0x01},
    {0x23C5, 0x00},
    {0x23AE, 0x80},
    {0x2251, 0x03},
    {0x0104, 0x01},
    {0x0100, 0x01},
};
const I2C_ARRAY Sensor_init_table_4lane[] =
{
    {0x0103, 0x00},
    {0x0100, 0x02},
    {0xffff, 100 },
    {0x0303, 0x02},
    {0x0305, 0x0C},
    {0x0307, 0x4A},
    {0x0309, 0x00},
    {0x030D, 0x0C},
    {0x030F, 0x3C},
    {0x0304, 0xA9},
    {0x231C, 0x70},
    {0x2300, 0x80},
    {0x2301, 0x23},
    {0x2302, 0x30},
    {0x2303, 0x01},
    {0x2305, 0xB0},
    {0x2306, 0x08},
    {0x2308, 0x70},
    {0x2309, 0x36},
    {0x230A, 0x05},
    {0x230B, 0x37},
    {0x230C, 0x11},
    {0x230D, 0x37},
    {0x230F, 0x01},
    {0x2310, 0x01},
    {0x2313, 0x3B},
    {0x2320, 0x04},
    {0x2321, 0x17},
    {0x2322, 0x27},
    {0x2323, 0x27},
    {0x2324, 0x37},
    {0x2325, 0x47},
    {0x2326, 0x57},
    {0x2327, 0x08},
    {0x2328, 0x08},
    {0x2329, 0x08},
    {0x232A, 0x09},
    {0x232B, 0x05},
    {0x232C, 0x05},
    {0x232D, 0x05},
    {0x2330, 0x14},
    {0x2331, 0x27},
    {0x2332, 0x27},
    {0x2333, 0x37},
    {0x2334, 0x47},
    {0x2335, 0x57},
    {0x2336, 0x57},
    {0x2337, 0x08},
    {0x2338, 0x08},
    {0x2339, 0x09},
    {0x233A, 0x09},
    {0x233B, 0x05},
    {0x233C, 0x05},
    {0x233D, 0x06},
    {0x2340, 0xF0},
    {0x2341, 0x03},
    {0x2342, 0x19},
    {0x2344, 0xF0},
    {0x2345, 0x0F},
    {0x2346, 0x15},
    {0x2348, 0xF0},
    {0x2349, 0x0F},
    {0x234A, 0x15},
    {0x2350, 0xF0},
    {0x2351, 0x0F},
    {0x2352, 0x15},
    {0x2354, 0xF7},
    {0x2355, 0x0F},
    {0x2356, 0x15},
    {0x2358, 0xF7},
    {0x2359, 0x0F},
    {0x235A, 0x11},
    {0x238E, 0x01},
    {0x23A0, 0x02},
    {0x23A1, 0x01},
    {0x23A3, 0xB0},
    {0x23A4, 0x00},
    {0x226A, 0x02},
    {0x23A5, 0x3F},
    {0x23A6, 0xB2},
    {0x23A7, 0x0B},
    {0x23A8, 0x1F},
    {0x23A9, 0x00},
    {0x23AB, 0x20},
    {0x23AC, 0x0F},
    {0x23AD, 0x0F},
    {0x23AF, 0xF0},
    {0x23B0, 0x02},
    {0x23B1, 0x84},
    {0x23B2, 0x40},
    {0x23B6, 0x05},
    {0x23B8, 0x07},
    {0x23BD, 0x02},
    {0x23BF, 0x09},
    {0x23C0, 0x08},
    {0x23C1, 0x11},
    {0x23C2, 0x00},
    {0x23C3, 0x08},
    {0x23C6, 0x55},
    {0x23C7, 0x55},
    {0x23C8, 0x05},
    {0x23CA, 0x3F},
    {0x0202, 0x03},
    {0x0203, 0x08},
    {0x0205, 0x00},
    {0x0206, 0x01},
    {0x0207, 0x00},
    {0x0208, 0x01},
    {0x0209, 0x00},
    {0x020A, 0x01},
    {0x020B, 0x00},
    {0x020C, 0x01},
    {0x020D, 0x00},
    {0x020E, 0x01},
    {0x020F, 0x00},
    {0x0350, 0xFF},
    {0x2200, 0x00},
    {0x2201, 0x00},
    {0x2202, 0x40},
    {0x2204, 0x00},
    {0x2205, 0x20},
    {0x2206, 0x00},
    {0x2207, 0x10},
    {0x2208, 0x10},
    {0x2209, 0x20},
    {0x220A, 0x02},
    {0x220B, 0x00},
    {0x220C, 0x01},
    {0x220D, 0x00},
    {0x2210, 0x00},
    {0x2220, 0x00},
    {0x2221, 0x00},
    {0x2222, 0x40},
    {0x2230, 0x00},
    {0x2400, 0x20},
    {0x2401, 0x30},
    {0x2402, 0x20},
    {0x2403, 0x08},
    {0x2404, 0x40},
    {0x2405, 0x08},
    {0x2406, 0x30},
    {0x2407, 0x43},
    {0x2408, 0x50},
    {0x240B, 0x10},
    {0x2412, 0x02},
    {0x2413, 0x0F},
    {0x2460, 0x00},
    {0x2468, 0x03},
    {0x2443, 0x01},
    {0x2444, 0x30},
    {0x2445, 0x20},
    {0x2446, 0x20},
    {0x2481, 0x1B},
    {0x2482, 0x20},
    {0x2483, 0x20},
    {0x2484, 0x0F},
    {0x2480, 0x00},
    {0x2485, 0x14},
    {0x2486, 0x0A},
    {0x2487, 0x16},
    {0x2488, 0x2E},
    {0x2489, 0x3F},
    {0x247C, 0x1F},
    {0x247D, 0x04},
    {0x2490, 0x00},
    {0x2880, 0x62},
    {0x2881, 0x10},
    {0x2882, 0x09},
    {0x2883, 0x00},
    {0x2884, 0x1F},
    {0x2885, 0x0F},
    {0x2886, 0x03},
    {0x0340, 0x08},
    {0x0341, 0x4D},
    {0x0342, 0x0C},
    {0x0343, 0x10},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0348, 0x0A},
    {0x0349, 0x9F},
    {0x0346, 0x00},
    {0x0347, 0x00},
    {0x034A, 0x08},
    {0x034B, 0x1F},
    {0x0351, 0x0A},
    {0x0352, 0xA0},
    {0x0353, 0x08},
    {0x0354, 0x20},
    {0x0355, 0x00},
    {0x0356, 0x00},
    {0x0357, 0x00},
    {0x0358, 0x00},
    {0x0370, 0x00},
    {0x0371, 0x00},
    {0x0383, 0x01},
    {0x0387, 0x01},
    {0x0390, 0x00},
    {0x2800, 0x01},
    {0x2810, 0x04},
    {0x2811, 0x04},
    {0x2812, 0x03},
    {0x2813, 0x08},
    {0x2814, 0x05},
    {0x2815, 0x10},
    {0x2816, 0x04},
    {0x281B, 0x04},
    {0x281C, 0x04},
    {0x281D, 0x00},
    {0x281E, 0x0A},
    {0x2828, 0x00},
    {0x2829, 0x10},
    {0x2831, 0xCE},
    {0x2832, 0x2B},
    {0x2833, 0x03},
    {0x2805, 0x00},
    {0x23BB, 0x01},
    {0x23C5, 0x00},
    {0x23AE, 0x80},
    {0x2251, 0x03},
    {0x0104, 0x01},
    {0x0100, 0x01},
};

const I2C_ARRAY Sensor_init_table_HDR_4lane[] =
{
    {0x0103,0x00},
    {0x0100,0x02},
    //delay 100ms
    {0x0303,0x02},
    {0x0305,0x0C},
    {0x0307,0x72},
    {0x0309,0x00},
    {0x030D,0x0C},
    {0x030F,0x58},
    {0x0304,0xA9},
    {0x231C,0x70},
    {0x240B,0x10},
    {0x240C,0x10},
    {0x240D,0x10},
    {0x2300,0x90},
    {0x2301,0x23},
    {0x2302,0x30},
    {0x2303,0x01},
    {0x2305,0x60},
    {0x2306,0x2D},
    {0x2308,0x80},
    {0x2309,0x36},
    {0x230A,0x05},
    {0x230B,0x38},
    {0x230C,0x11},
    {0x230D,0x37},
    {0x230F,0x01},
    {0x2310,0x01},
    {0x2313,0x00},
    {0x2320,0x04},
    {0x2321,0x17},
    {0x2322,0x27},
    {0x2323,0x27},
    {0x2324,0x37},
    {0x2325,0x47},
    {0x2326,0x57},
    {0x2327,0x08},
    {0x2328,0x08},
    {0x2329,0x08},
    {0x232A,0x09},
    {0x232B,0x05},
    {0x232C,0x05},
    {0x232D,0x05},
    {0x2330,0x14},
    {0x2331,0x27},
    {0x2332,0x27},
    {0x2333,0x37},
    {0x2334,0x47},
    {0x2335,0x57},
    {0x2336,0x57},
    {0x2337,0x08},
    {0x2338,0x08},
    {0x2339,0x09},
    {0x233A,0x09},
    {0x233B,0x05},
    {0x233C,0x05},
    {0x233D,0x06},
    {0x2340,0xF0},
    {0x2341,0x03},
    {0x2342,0x25},
    {0x2344,0xF0},
    {0x2345,0x0F},
    {0x2346,0x1C},
    {0x2348,0xF0},
    {0x2349,0x0F},
    {0x234A,0x19},
    {0x2350,0xF0},
    {0x2351,0x0F},
    {0x2352,0x19},
    {0x2354,0xF7},
    {0x2355,0x0F},
    {0x2356,0x19},
    {0x2358,0xF7},
    {0x2359,0x0F},
    {0x235A,0x19},
    {0x238E,0x01},
    {0x23A0,0x02},
    {0x23A1,0x01},
    {0x23A3,0xB0},
    {0x23A4,0x00},
    {0x226A,0x02},
    {0x23A5,0x3F},
    {0x23A6,0xB2},
    {0x23A7,0x0B},
    {0x23A8,0x1F},
    {0x23A9,0x00},
    {0x23AB,0x20},
    {0x23AC,0x0F},
    {0x23AD,0x0F},
    {0x23AF,0xF0},
    {0x23B0,0x02},
    {0x23B1,0x84},
    {0x23B2,0x40},
    {0x23B6,0x05},
    {0x23B8,0x07},
    {0x23BD,0x02},
    {0x23BF,0x09},
    {0x23C0,0x08},
    {0x23C1,0x11},
    {0x23C2,0x00},
    {0x23C3,0x08},
    {0x23C6,0x55},
    {0x23C7,0x55},
    {0x23C8,0x05},
    {0x23CA,0x3F},
    {0x2308,0x70},
    {0x0202,0x03},
    {0x0203,0x08},
    {0x0205,0x00},
    {0x0206,0x01},
    {0x0207,0x00},
    {0x0208,0x01},
    {0x0209,0x00},
    {0x020A,0x01},
    {0x020B,0x00},
    {0x020C,0x01},
    {0x020D,0x00},
    {0x020E,0x01},
    {0x020F,0x00},
    {0x0350,0xFF},
    {0x2200,0x01},//enable stagger
    {0x2201,0x00},//ttt
    {0x2202,0x40},
    {0x2204,0x00},//ttt
    {0x2205,0x70},
    {0x2206,0x00},
    {0x2207,0x20},//ttt
    {0x2208,0x10},
    {0x2209,0x20},
    {0x220A,0x02},
    {0x220B,0x00},
    {0x220C,0x01},
    {0x220D,0x00},
    {0x2210,0x00},
    {0x2220,0x00},
    {0x2221,0x00},
    {0x2222,0x40},
    {0x2230,0x00},
    {0x2400,0x20},
    {0x2401,0x30},
    {0x2402,0x20},
    {0x2403,0x08},
    {0x2404,0x40},
    {0x2405,0x08},
    {0x2406,0x30},
    {0x2407,0x43},
    {0x2408,0x50},
    {0x240B,0x10},
    {0x2412,0x02},
    {0x2413,0x0F},
    {0x2460,0x00},
    {0x2468,0x03},
    {0x2443,0x01},
    {0x2444,0x30},
    {0x2445,0x20},
    {0x2446,0x20},
    {0x2481,0x1B},
    {0x2482,0x20},
    {0x2483,0x20},
    {0x2484,0x0F},
    {0x2480,0x00},
    {0x2485,0x14},
    {0x2486,0x0A},
    {0x2487,0x16},
    {0x2488,0x2E},
    {0x2489,0x3F},
    {0x247C,0x1F},
    {0x247D,0x04},
    {0x2490,0x00},
    {0x2880,0x62},
    {0x2881,0x10},
    {0x2882,0x09},
    {0x2883,0x00},
    {0x2884,0x1F},
    {0x2885,0x0F},
    {0x2886,0x03},
    {0x0340,0x07},//1942
    {0x0341,0x96},
    {0x0342,0x18},//6256
    {0x0343,0x70},
    {0x0344,0x00},
    {0x0345,0x00},
    {0x0348,0x0A},
    {0x0349,0x9F},
    {0x0346,0x00},
    {0x0347,0x50},
    {0x034A,0x07},
    {0x034B,0xCF},
    {0x0351,0x0A},
    {0x0352,0x00},
    {0x0353,0x07},
    {0x0354,0x80},
    {0x0355,0x00},
    {0x0356,0x50},
    {0x0357,0x00},
    {0x0358,0x00},
    {0x0370,0x00},
    {0x0371,0x00},
    {0x0383,0x01},
    {0x0387,0x01},
    {0x0390,0x00},
    {0x2800,0x01},
    {0x2810,0x05},
    {0x2811,0x06},
    {0x2812,0x05},
    {0x2813,0x0B},
    {0x2814,0x07},
    {0x2815,0x18},
    {0x2816,0x06},
    {0x281B,0x07},
    {0x281C,0x07},
    {0x281D,0x00},
    {0x281E,0x0C},
    {0x2828,0x00},
    {0x2829,0x10},
    {0x2831,0xCE},
    {0x2832,0x2B},
    {0x2833,0x03},
    {0x2805,0x04},
    {0x2200,0x01},
    {0x23BB,0x01},
    {0x23C5,0x00},
    {0x23AE,0x80},
    {0x2251,0x03},
    {0x0104,0x01},
    {0x0100,0x01},

    //{0x0601,0x87},//test pattern
    //{0x0104,0x01},
    //{0x0100,0x01},


};

//HDR
const static I2C_ARRAY gain_HDR_LEF_reg[] =
{
    {0x2209, 0x20},       //again
    {0x220A, 0x01},    //dgain_h,
    {0x220B, 0x00},    //dgain_l,
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const static I2C_ARRAY gain_HDR_SEF1_reg[] =
{
    {0x2208, 0x10},       //again
    {0x220C, 0x01},    //dgain_h, 1-3x
    {0x220D, 0x00},    //dgain_l,
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};


const I2C_ARRAY gain_reg_hdr[] = {
    {0x0205, 0x00},    // again,   1-15.5x
    {0x020e, 0x01},    // dgain_h, 1-3x
    {0x020f, 0x00},    // dgain_l,
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const I2C_ARRAY expo_HDR_LEF_reg[] = {
    {0x2204, 0x00},    // shutter[15:8]
    {0x2205, 0x70},    // shutter[7:0]
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

const I2C_ARRAY expo_HDR_SEF_reg[] = {
    {0x2206, 0x00},    // shutter[15:8]
    {0x2207, 0x20},    // shutter[7:0]
    {0x0104, 0x01},    //
    {0x0104, 0x00},    //
};

#if 0
const I2C_ARRAY Sensor_init_table[] =     // 1920*1080_30fps_27MCLK_1280*2*1125*30
{
//---------------------------------------------------
// Initial
//---------------------------------------------------
{0103,0x00},// ; software reset-> was 0x22
{0100,0x02},//  power up
{0xffff,100 }, //delay 100msec
//---------------------------------------------------
// PLL (PCLK = 40.5 MHz, PKTCLK = 94.5 MHz)
//---------------------------------------------------
// PCLK (RAW)
{0x0303,0x02},// ; vt_sys_div[7:0]        CLK_DIV, [2:0] -> 0:divide by 4, 1:divide by 5, 2:divide by 6, 3:divide by 7, 4:divide by 8   [5:4] -> 0:divide by 2, 1:divide by 3, 2:divide by 4
{0x0305,0x0C},// ; pll_pre_div_d [4:0]      PLL N,   N = 12, while mclk 24mhz
{0x0307,0x36},// ; pll_multiplier_d         PLL M,   M = 52, pclk_raw= (2*mclk*M/N)/CLK_DIV[2:0]

// lv_pclk = (2*mclk*M/N)/CLK_DIV[5:4]

// PKTCLK (MIPI)
{0x0309,0x00},// ; op_pix_div             CLK_DIV, 0,1:divide by 1, 2,3:divide by 2
{0x030D,0x0C},// ; mipi_pll_clk_div_d [4:0]    PLL N,   N = 12, while mclk 24mhz
{0x030F,0x54},// ; pll_mipi_multiplier_d    PLL M,   M = 124, pclk_raw= (2*mclk*M/N)/CLK_DIV[2:0]/4
//PLL enable
{0x0304,0xA9},// ; [0] enable PLL after PLL config done
//---------------------------------------------------
// Analog
//---------------------------------------------------
//L common\HM5530_analog_setting.txt
{0x231C,0x70},// ;
{0x2300,0x80},// ; CDS timing
{0x2301,0x23},// ; CDS timing
{0x2302,0x30},// ; CDS timing
{0x2303,0x01},// ; CDS timing
{0x2305,0xB0},// ; CDS timing
{0x2306,0x26},// ; CDS timing
{0x2308,0x70},// ; CDS timing
{0x2309,0x36},// ; CDS timing
{0x230A,0x05},// ; CDS timing
{0x230B,0x37},// ; CDS timing
{0x230C,0x11},// ; CDS timing
{0x230D,0x37},// ; CDS timing
{0x230F,0x01},// ; CDS timing
{0x2310,0x01},// ; CDS timing
{0x2313,0x3B},// ; CDS timing
{0x2325,0x48},// ; 32x gain (normal)
{0x2326,0x5B},// ; 64x gain (normal)
{0x2327,0x08},// ; 1x gain (normal)
{0x2328,0x08},// ; 2x gain (normal)
{0x2329,0x08},// ; 4x gain (normal)
{0x232A,0x08},// ; 8x gain (normal)
{0x232B,0x04},// ; 16x gain (normal)
{0x232C,0x04},// ; 32x gain (normal)
{0x232D,0x04},// ; 64x gain (normal)
{0x2335,0x58},// ; 32x gain (summing)
{0x2336,0x53},// ; 64x gain (summing)
{0x2337,0x08},// ; 1x gain (summing)
{0x2338,0x08},// ; 2x gain (summing)
{0x2339,0x08},// ; 4x gain (summing)
{0x233A,0x08},// ; 8x gain (summing)
{0x233B,0x04},// ; 16x gain (summing)
{0x233C,0x04},// ; 32x gain (summing)
{0x233D,0x04},// ; 64x gain (summing)

// 1x gain
{0x2340,0xF0},// ;
{0x2341,0x03},// ;
{0x2342,0x1E},// ; [5:2] ramp_offset[4:1]
// 2x gain
{0x2344,0xF0},// ;
{0x2345,0x3F},// ;
{0x2346,0x1D},// ; [5:2] ramp_offset[4:1]
// 4x gain
{0x2348,0xF0},// ;
{0x2349,0x3F},// ;
{0x234A,0x1D},// ; [5:2] ramp_offset[4:1]
// 8x gain
{0x2350,0xF0},//;
{0x2351,0x3F},//;
{0x2352,0x1D},//; [5:2] ramp_offset[4:1]
// 16x gain
{0x2354,0xF4},
{0x2355,0x3F},
{0x2356,0x25}, //[5:2] ramp_offset[4:1]
// 32x gain
{0x2358,0xF5},//;
{0x2359,0x3F},//;
{0x235A,0x25},//; [5:2] ramp_offset[4:1]
{0x238E,0x01},//;
{0x23A0,0x06},//;
{0x23A1,0x01},//;
{0x23A3,0xB0},//;
{0x23A4,0x00},//;
{0x226A,0x02},//;
{0x23A5,0x3F},//;
{0x23A6,0x92},//;[3:1]ca_bias_sel_d
{0x23A7,0x09},//;
{0x23A8,0x1F},//;
{0x23A9,0x00},//;
{0x23AB,0x20},//;
{0x23AC,0x01},//;
{0x23AD,0x0F},//;
{0x23AF,0xF0},//;
{0x23B0,0x02},//;
{0x23B1,0x84},//; SF signal clamp enable[2]
{0x23B2,0x40},//;
{0x23B6,0x05},//;
{0x23B8,0x07},//;
{0x23BD,0x02},//;[1]ca_gain_float_en_d
{0x23BF,0x09},//; CA reset clamp enable[3]
{0x23C0,0x08},//;
{0x23C1,0x11},//;SF reset clamp enable[4]
{0x23C2,0x00},//;
{0x23C3,0x08},//;
{0x23C6,0x55},//;
{0x23C7,0x55},//;
{0x23C8,0x05},//;
{0x23CA,0x3F},//;
//---------------------------------------------------
// Digital function
//---------------------------------------------------

//L common\HM5530_digital_setting.txt

{0x0202,0x03},// ; INTG H
{0x0203,0x08},// ; INTG L
{0x0205,0x00},// ; Again
{0x0206,0x01},// ; GR Channel Gain
{0x0207,0x00},// ;
{0x0208,0x01},// ; R Channel Gain
{0x0209,0x00},// ;
{0x020A,0x01},// ; B Channel Gain
{0x020B,0x00},// ;
{0x020C,0x01},// ; GB Channel Gain
{0x020D,0x00},// ;
{0x020E,0x01},// ; AE gain
{0x020F,0x00},// ;
{0x0350,0xFF},// ; Turn it off dithering on Dgain
// Stagger HDR Function
{0x2200,0x00},// ;  [0]: Stagger HDR Enable
{0x2201,0x00},// ;  Long & Short Frame Distance H
{0x2202,0x40},// ;  Long & Short Frame Distance L (Precision: Full Row in 30 fps)
{0x2204,0x00},// ;  EDR Long INTG H
{0x2205,0x20},// ;    EDR Long INTG L
{0x2206,0x00},// ;  EDR Short INTG H
{0x2207,0x10},// ;     EDR Short INTG L
{0x2208,0x10},// ;  EDR Short Again
{0x2209,0x20},// ;  EDR Long Again
{0x220A,0x02},// ;  EDR Long Dgain
{0x220B,0x00},// ;    EDR Long Dgain
{0x220C,0x01},// ;    EDR Short Dgain
{0x220D,0x00},// ;     EDR Short Dgain

// 3DSYNC
{0x2210,0x00},// ;     [0] - 3DSYNC Master Enable,  [1] - 3DSYNC Slave Enable

// FSIN
{0x2220,0x00},//;  [0] - Exposure Sync Enable,  [1] - Read Sync Enable
{0x2221,0x00},//;    TD for Exposure Sync H
{0x2222,0x40},//;    TD for Exposure Sync L

// Temperature Sensor
{0x2230,0x00},//;  [0] - Temp Enable

// TestPattern
//;W 24 0601 00 2 1 ; [7]: Test pattern Enable
//                  ; [3:0]: Test pattern mode

// BLC
{0x2400,0x20},//; Dark-level Targert
{0x2401,0x30},//; Dark-level Targert (HDR-L)
{0x2402,0x20},//; Dark-level Targert (HDR-S)
{0x2403,0x08},//; [3]: signbit, [2:0]: BLCManualOffset[10:8] (Normal/HDR-L)
{0x2404,0x40},//; BLCManualOffset[7:0] (Normal/HDR-L)
{0x2405,0x08},//; [3]: signbit, [2:0]: BLCManualOffset[10:8] (HDR-S)
{0x2406,0x30},//; BLCManualOffset[7:0] (HDR-S)
{0x2407,0x43},//; BLC CFG ,
               //  ; [0]:BLC en, [1]:IIR en,
               //  ; [2]:Output mode (0->divide by 2, 1->truncate 1023)
               //  ; [3]:BLC manual mode
               //  ; [4]:BLC recompute (manual)
               //  ; [6:5]:IIR Filter weight. (00->1/8, 01->1/16, 1x->1/32)
{0x2408,0x50},// ; BLC MODE,
                // ; [2:0]:Hyst. threshold
                // ; [3]:Hyst. Enable
                // ; [4]:BLC adjust type. (0:4-ch, 1:1-ch)
                // ; [6]:BLC dither enable

{0x240B,0x10},//; Dark row DPC threshold level,

// BLE
//;W 24 2410 00 2 1 ; [3]: BLE manual neg_offset_en (Normal/HDR-L)
//;                 ; [2:0]:BLE manual offset_value (Normal/HDR-L)
//;W 24 2411 00 2 1 ; [3]: BLE manual neg_offset_en (HDR-S)
//;                 ; [2:0]:BLE manual offset_value (HDR-S)
{0x2412,0x02},// ; BLE CFG
                // ; [0]:BLE en, [1]:IIR en,
                // ; [3]:BLE manual mode
               //  ; [6:5]:IIR Filter weight. (00->1/8, 01->1/16, 1x->1/32)
{0x2413,0x0F},//; BLE MODE
               //  ; [2:0]:Hyst. threshold
               //  ; [3]:Hyst. Enable
               //  ; [6]:BLE dither enable

// LSC
{0x2460,0x00},//; [0] : LSC en

// Dgain
{0x2468,0x03},//; [0] : cgc_gain_en

// BLI/Reorder
{0x2443,0x01},//; [0]: BLI enable, [7]:reorder_mem_bypass_en
{0x2444,0x30},//; BLI target (HDR-L)
{0x2445,0x20},//; BLI target (HDR-S)
{0x2446,0x20},//; BLI target (Normal)

// DPC (bayer setting)
{0x2481,0x1B},//; DPC control: [0]:DPC enable, [1]:Static correction, [2]: Dynamic correction, [3]: Queue update(debugging)
//;             ; [4]: 0 mono/ 1 bayer, [5]: Dynamic margin enable
{0x2482,0x20},//; Dynamic hot pixel threshold
{0x2483,0x20},//; Dynamic cold pixel threshold
{0x2484,0x0F},//; Dynamic Margin threshold

// Color Denoise, updated setting based on 013017 ver analog setting
{0x2480,0x00},// ; [0] : color denoise en
{0x2485,0x14},// ;
{0x2486,0x0A},// ;
{0x2487,0x16},// ;
{0x2488,0x2E},// ;
{0x2489,0x3F},// ;

// LVCMOS CFG
{0x247C,0x1F},// ; Vsync Pulse Start Position
{0x247D,0x04},// ; Vsync Pulse Width
{0x2490,0x00},// ; [0]: Vsync Output Format, 0-> Conventional, 1->Pulse-Type

// OTP
//;W 24 2606 01 2 1 ; [0]: LSC Auto-load enable
//;W 24 2690 07 2 1 ; [2:0]: BPC OTP enable

//MIPI
{0x2880,0x62},//;
{0x2881,0x10},//;
{0x2882,0x09},//;
{0x2883,0x00},//;
{0x2884,0x1F},//;
{0x2885,0x0F},//;
{0x2886,0x03},//;
//---------------------------------------------------
// Resolution :
//---------------------------------------------------
// Frame Length
{0x0340,0x0A},//
{0x0341,0x24},//

// Line Length        (Can be different after Analog tuning)

{0x0342,0x0C},//
{0x0343,0x30},//


//---------------------------------------------------
// Resolution :
//---------------------------------------------------
// Cropping Size
{0x0344,0x00},//; x start H
{0x0345,0x00},//; x start L
{0x0348,0x0A},//; x end H
{0x0349,0x9F},//; x end L
{0x0346,0x00},//; y start H
{0x0347,0x00},//; y start L
{0x034A,0x08},//; y end H -> was 03
{0x034B,0x1F},//; y end L -> was CF

// Digital Window Size & Offset
{0x0351,0x0A},//; D-win, x size hb [2:0]
{0x0352,0x20},//; D-win, x size lb
{0x0353,0x07},//; D-win, y size hb [2:0]
{0x0354,0x98},//; D-win, y size lb
{0x0355,0x00},//; D-win, x offset hb [2:0]
{0x0356,0x40},//; D-win, x offset lb
{0x0357,0x00},//; D-win, y offset hb [2:0]
{0x0358,0x44},//; D-win, y offset lb

// Mono mode
{0x0370,0x00},//; [0] - Mono mode enable
{0x0371,0x00},//;

// Sub2 Timing & Binning mode
{0x0383,0x01},//; X_odd_increment
{0x0387,0x01},//; Y_odd_increment
{0x0390,0x00},//; Binning mode enable, [0] - Vertical, [1] - Horizontal

//---------------------------------------------------
// MIPI TX
//---------------------------------------------------
{0x2800,0x01},//; MIPI en
{0x2810,0x06},//; THS_PREPARE
{0x2811,0x06},//; TCLK_PREPARE
{0x2812,0x05},//; TLPX
{0x2813,0x0B},//; THS_ZERO
{0x2814,0x07},//; THS_TRAIL
{0x2815,0x19},//; TCLK_ZERO
{0x2816,0x06},//; TCLK_TRAIL
{0x281B,0x08},//; TCLK_EXIT
{0x281C,0x08},//; TCLK_PRE
{0x281D,0x00},//; TCLK_PRE
{0x281E,0x0C},//; TCLK_POST
{0x2828,0x00},//; full trigger H
{0x2829,0x10},//; full trigger L

{0x2831,0xDE},//; CC0 : Clock lane on always.              (9E) / woLSLE (8E)
              //   ; GC2 : Clock lane on while frame active.  (DE) / woLSLE (CE)
              //   ; GC1 : Clock lane on while packet issued. (BE) / woLSLE (AE)
              //   ; GC3 : Clock lane on while line  active.  (FE) / woLSLE (EE)

{0x2832,0x2B},// ; color format 2B: Raw10, other : Raw8

{0x2833,0x01},//; 1:2 lane  3:4 lane
{0x2805,0x04},//;

//---------------------------------------------------
// MIPI RX (HII/ST-RX)
//---------------------------------------------------
// HITC01, 2/4-lane
//L common\HIIRX\HITC01_common_configuration.txt


//---------------------------------------------------
// Output Format Selection
//---------------------------------------------------
{0x23BB,0x01},////; [0] : enable MIPI D_PHY
{0x23C5,0x00},////; [5] : lv_pad_oe   [6] : pad_oe
{0x23AE,0x80},////; [5] : MSB first   [6] : invert pclk_lv clock that is fed to digital   [7] : invert pclko (output of LVCMOS)

{0x2251,0x03},//; [7]EARLY_GAIN_EN

//---------------------------------------------------
// CMU update
//---------------------------------------------------
//W 24 4800 00 2 1 ; was CMU 0000
{0x0104,0x01},//; was 0100
//W 24 0104 00 2 1 ; was 0100, need an high to low edge now! (CMU_AE)
//W 24 4801 00 2 1 ; was 0106
//W 24 0000 00 2 1 ; was 0000


//---------------------------------------------------
// Turn on rolling shutter
//---------------------------------------------------
{0x0100,0x01},//was 0005 ; mode_select

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

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////
static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence: XSD_L&MCLK -> XSD H -> I2C
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }

    sensor_if->Reset(idx, handle->reset_POLARITY );
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_USLEEP(1000);

    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    sensor_if->Reset(idx, !handle->reset_POLARITY );
    SENSOR_USLEEP(1000);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    return SUCCESS;
}

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

    for(n=0; n<16; ++n) {              //retry , until I2C success
        if(n>10) return FAIL;

        if(SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
            break;
        else
            SENSOR_MSLEEP(10);
    }

    for(i=0; i<table_length; ++i) {
        if( id_from_sensor[i].data != Sensor_id_table[i].data ) {
            SENSOR_EMSG("[%s]Read hm5532 id: 0x%x 0x%x\n", __FUNCTION__, id_from_sensor[0].data, id_from_sensor[1].data);
            return FAIL;
        }
        *id = id_from_sensor[i].data;

        SENSOR_IMSG("[%s]Read hm5532 id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    }
    return SUCCESS;
}

static int pCus_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}

static int pCus_init(ms_cus_sensor *handle)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    int i,cnt = 0;
    u32 size = 0;
    const I2C_ARRAY *sensor_init_table = NULL;

    if (lane_num == 4) {
        size = ARRAY_SIZE(Sensor_init_table_4lane);
        sensor_init_table = Sensor_init_table_4lane;
        SENSOR_DMSG("HM5532 4lane\n");
    } else if (lane_num == 2) {
        size = ARRAY_SIZE(Sensor_init_table_2lane);
        sensor_init_table = Sensor_init_table_2lane;
        SENSOR_DMSG("HM5532 2lane\n");
    } else {
        return FAIL;
    }
    //UartSendTrace("HM5532 Sensor_init_table_4lane\n");
    for(i=0; i< size; i++)
    {
        if(sensor_init_table[i].reg == 0xffff)
        {
            SENSOR_MSLEEP(sensor_init_table[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(sensor_init_table[i].reg, sensor_init_table[i].data) != SUCCESS)
            {
                cnt++;
                //printf("Sensor_init_table_4lane -> Retry %d...\n",cnt);
                if(cnt >= 10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //SensorReg_Read( Sensor_init_table_4lane[i].reg, &sen_data );
            //UartSendTrace("HM5532 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_4lane[i].reg, Sensor_init_table_4lane[i].data, sen_data);
        }
    }

    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;

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
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            if (lane_num == 4) {
                handle->interface_attr.attr_mipi.mipi_lane_num = 4;
            } else if (lane_num == 2) {
                handle->interface_attr.attr_mipi.mipi_lane_num = 2;
            } else {
                return FAIL;
            }
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    char sen_data;

    sen_data = mirror_reg[0].data;
    SENSOR_DMSG("mirror:%x\r\n", sen_data & 0x03);
    switch(sen_data & 0x03)
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

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;

    switch(orit) {
        case CUS_ORIT_M0F0:
            mirror_reg[0].data = 0;
            handle->bayer_id=    CUS_BAYER_BG;
        break;
        case CUS_ORIT_M1F0:
            mirror_reg[0].data = 1;
            handle->bayer_id=    CUS_BAYER_GB;
        break;
        case CUS_ORIT_M0F1:
            mirror_reg[0].data = 2;
            handle->bayer_id=    CUS_BAYER_GR;
        break;
        case CUS_ORIT_M1F1:
            mirror_reg[0].data = 3;
            handle->bayer_id=    CUS_BAYER_RG;
        break;
    }

    params->orien_dirty = true;

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | params->tVts_reg[1].data;

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return  params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;

    if(fps>=5 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0xff;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0xff;
        params->reg_dirty = true;
        SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
        return SUCCESS;
    }else if(fps>=5000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0xff;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0xff;
        params->reg_dirty = true;
        SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
        return SUCCESS;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
}

static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
  if (cap)
    memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
  else
    return FAIL;
  return SUCCESS;
}

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;

    switch(status) {
        case CUS_FRAME_ACTIVE:
            if(params->reg_dirty) {
            //  SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
            params->reg_dirty = false;
            }
        break;
        case CUS_FRAME_INACTIVE:
            if(params->orien_dirty){
             //sensor_if1->SetSkipFrame(handle,5); //skip 2 frame to avoid bad frame after mirror/flip
             SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
             //sensor_if->BayerFmt(handle, handle->bayer_id);
             params->orien_dirty = false;
            }
        default :
        break;
    }

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines  = (u32)(params->tExpo_reg[1].data);
    lines |= (u32)(params->tExpo_reg[0].data)<<8;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines = (1000*us)/Preview_line_period;

    if (lines >params->expo.vts-2) {
        vts = lines +2;
    }
    else
        vts=params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    params->tExpo_reg[0].data = (lines>>8) & 0xff;
    params->tExpo_reg[1].data = (lines)    & 0xff;

    params->tVts_reg[0].data = (vts >> 8) & 0xff;
    params->tVts_reg[1].data = (vts >> 0) & 0xff;

    params->reg_dirty = true;
  return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int rc = SUCCESS;
    int again, dgain;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    again  = 1 << ((params->tGain_reg[0].data&0xf0)>>4);
    again *= (16+(params->tGain_reg[0].data&0x0f)) * 64;

    dgain  = (params->tGain_reg[1].data&0x03)*1024;
    dgain += (params->tGain_reg[2].data)*4;

    *gain = (again*dgain) / 1024;

    SENSOR_DMSG("[%s] again/gain = 0x%x,%x/0x%x\n", __FUNCTION__, params->tGain_reg[0].data,again, *gain);
  return rc;
}

// HII: 2^(gain[7:4])*(1+gain[3:0]/16)
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    //int dgain, again_h, again_l;
    //u16 i;
    //double gain_double,total_gain_double,DGain_double;
    //u16 gain16;
    u32 gain2_4,gain4_8,gain8_16,gain16_32;
    hm5532_params *params = (hm5532_params *)handle->private_data;
    params->expo.final_gain = gain;
/*
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN)
        gain=SENSOR_MAX_GAIN;


     gain_double=(double)gain;
        total_gain_double=(gain_double)/1024;
             for(i=1;i<64;i++){
                   if(gain_table[i].total_gain>total_gain_double){
                      gain16=gain_table[i-1].reg_val;
                      break;
                      }
                   else if(i==63){
                      gain16=gain_table[i].reg_val;
                      break;
                    }
                  }

      DGain_double=(total_gain_double/gain_table[i-1].total_gain);
      if(DGain_double<1.00)
         DGain_double=1.00;
      else if(DGain_double>4)
         DGain_double=4;

      dgain = (u16)(DGain_double*64);
      params->tGain_reg[1].data = dgain >> 8;
      params->tGain_reg[2].data = dgain & 0xff;
      params->tGain_reg[0].data =gain16;
*/

#if 0
  if(gain<1024)
    gain=1024;
  else if(gain>SENSOR_MAX_GAIN)
    gain=SENSOR_MAX_GAIN;

  if (gain > 15.5*1024) {
      dgain = (gain*256) / (15.5*1024);
      params->tGain_reg[1].data = dgain >> 8;
      params->tGain_reg[2].data = dgain & 0xff;
      gain = 15.5*1024;
  }

  if (gain >= 8*1024) {
    again_h = 0x30;
    again_l = (gain/64)/8 - 16;
  } else if (gain >= 4*1024) {
    again_h = 0x20;
    again_l = (gain/64)/4 - 16;
  } else if (gain >= 2*1024) {
    again_h = 0x10;
    again_l = (gain/64)/2 - 16;
  } else {
    again_h = 0x00;
    again_l = (gain/64)/1 - 16;
  }
  params->tGain_reg[0].data = again_h | again_l;
#endif
#if 1
    if(gain<2048) {//<X2
        params->tGain_reg[0].data = (((gain-1024)>>6)) & 0x000f;
    } else if ((gain>=2048 )&&(gain<4096)) { //X2~X4
        gain2_4=gain-2048;
        params->tGain_reg[0].data = ((gain2_4>>7) & 0x000f)|0x10;
    } else if((gain>=4096 )&&(gain<8192)) {//X4~X8
        gain4_8=gain-4096;
        params->tGain_reg[0].data =( (gain4_8>>8) & 0x000f)|0x20;
    } else if((gain>=8192 )&&(gain<16384)) {//X8~X16
        gain8_16=gain-8192;
        params->tGain_reg[0].data =((gain8_16>>9) & 0x000f)|0x30;
    } else if((gain>=16384 )&&(gain<32768)) {//X16~X32
        gain16_32=gain-16384;
        params->tGain_reg[0].data =( (gain16_32>>10) & 0x000f)|0x40;
    }
#endif

  SENSOR_DMSG("[%s] set gain %d, reg=0x%x/0x%x.%x\n", __FUNCTION__, gain, params->tGain_reg[0].data, params->tGain_reg[1].data, params->tGain_reg[2].data);

  params->reg_dirty = true;
  return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 1;
  *max = 1000000/30;

  return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 1024;
  *max = SENSOR_MAX_GAIN;

  return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
  //u32 i, j;

  SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

  return SUCCESS;
}

static int pCus_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

//HDR
int pCus_init_mipi4lane_hdr(ms_cus_sensor *handle)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    int i,cnt = 0;
    u32 size = 0;
    const I2C_ARRAY *sensor_init_table = NULL;

    if (lane_num == 4) {
        size = ARRAY_SIZE(Sensor_init_table_HDR_4lane);
        sensor_init_table = Sensor_init_table_HDR_4lane;
        SENSOR_DMSG("HM5532 4lane HDR\n");
    } else if (lane_num == 2) {
        size = ARRAY_SIZE(Sensor_init_table_2lane);
        sensor_init_table = Sensor_init_table_2lane;
        SENSOR_DMSG("HM5532 2lane HDR\n");
    } else {
        return FAIL;
    }

    for(i=0; i< size; i++)
    {
        if(sensor_init_table[i].reg == 0xffff)
        {
            SENSOR_MSLEEP(sensor_init_table[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(sensor_init_table[i].reg, sensor_init_table[i].data) != SUCCESS)
            {
                cnt++;
                //printf("Sensor_init_table_4lane -> Retry %d...\n",cnt);
                if(cnt >= 10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //SensorReg_Read( Sensor_init_table_4lane[i].reg, &sen_data );
            //UartSendTrace("HM5532 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_4lane[i].reg, Sensor_init_table_4lane[i].data, sen_data);
        }
    }

    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;

        return SUCCESS;
}


int pCus_init_mipi2lane_hdr(ms_cus_sensor *handle)
{
    return SUCCESS;
}

static int pCus_SetVideoRes_hdr(ms_cus_sensor *handle, u32 res_idx)
{
    SENSOR_DMSG("\n\npCus_SetVideoRes_HDR_DOL\n");
    return SUCCESS;
}

static int pCus_GetFPS_hdr_sef(ms_cus_sensor *handle)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | params->tVts_reg[1].data;

    if (lane_num == 4)
    {
        if (params->expo.fps >= 5000)
        	params->expo.preview_fps = vts_25fps_HDR_4lane*max_fps*1000/tVts;
        else
        	params->expo.preview_fps = vts_25fps_HDR_4lane*max_fps/tVts;
    }
    else if (lane_num == 2)
    {
        if (params->expo.fps >= 5000)
        	params->expo.preview_fps = vts_30fps_HDR_2lane*max_fps*1000/tVts;
        else
        	params->expo.preview_fps = vts_30fps_HDR_2lane*max_fps/tVts;
    }
    return  params->expo.preview_fps;
}

static int pCus_SetFPS_hdr_sef(ms_cus_sensor *handle, u32 fps)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;

     if(fps>=5 && fps <= 30) {
         params->expo.fps = fps;
         params->expo.vts=    (vts_25fps_HDR_4lane*25)/fps;
         params->tVts_reg[0].data = (params->expo.vts >> 8) & 0xff;
         params->tVts_reg[1].data = (params->expo.vts >> 0) & 0xff;
         params->reg_dirty = true;
         SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
         return SUCCESS;
     }else if(fps>=5000 && fps <= 30000) {
         params->expo.fps = fps;
         params->expo.vts=    (vts_25fps_HDR_4lane*25000)/fps;
         params->tVts_reg[0].data = (params->expo.vts >> 8) & 0xff;
         params->tVts_reg[1].data = (params->expo.vts >> 0) & 0xff;
         params->reg_dirty = true;
         SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
         return SUCCESS;
     }else{
         SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
         return FAIL;
     }

    return SUCCESS;
}

static int pCus_GetAEUSecs_hdr_sef(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines  = (u32)(params->tExpo_hdr_sef_reg[1].data);
    lines |= (u32)(params->tExpo_hdr_sef_reg[0].data)<<8;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;

}


static int pCus_SetAEUSecs_hdr_sef(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines = (1000*us)/Preview_line_period;

    if (lines >params->expo.vts-2) {
        vts = lines +2;
    }
    else
        vts=params->expo.vts;

//    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
//                us,
//                lines,
//                params->expo.vts
//                );

    params->tExpo_hdr_sef_reg[0].data = (lines>>8) & 0xff;
    params->tExpo_hdr_sef_reg[1].data = (lines)    & 0xff;

    params->tVts_reg[0].data = (vts >> 8) & 0xff;
    params->tVts_reg[1].data = (vts >> 0) & 0xff;

    params->reg_dirty = true;
  return SUCCESS;
}

static int pCus_GetAEGain_hdr_sef(ms_cus_sensor *handle, u32* gain)
{
    int rc = SUCCESS;
    int again, dgain;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    again  = 1 << ((params->tGain_hdr_lef_reg[0].data&0xf0)>>4);
//    again *= (16+(params->tGain_hdr_lef_reg[0].data&0x0f)) * 64;

    dgain  = (params->tGain_hdr_sef_reg[1].data&0x03)*1024;
    dgain += (params->tGain_hdr_sef_reg[2].data)*4;

    *gain = (again*dgain) / 1024;

    SENSOR_DMSG("[%s] again/gain = 0x%x,%x/0x%x\n", __FUNCTION__, params->tGain_reg[0].data,again, *gain);
    return rc;
}


static int pCus_SetAEGain_hdr_sef(ms_cus_sensor *handle, u32 gain)
{ 
    hm5532_params *params = (hm5532_params *)handle->private_data;
    u32 g_again0 = 0;
    u32 g_again1 = 0;

    u32 g_againx = 0;
    u32 g_dgain = 0;

    g_againx = gain/1024;

    if (g_againx > 32)
        g_againx = 32;

    if (g_againx >= 32) {
        params->tGain_hdr_sef_reg[0].data = 0x50;
        g_againx = 32;
    }
    else if ((g_againx >= 16) && (g_againx < 32)) {
        params->tGain_hdr_sef_reg[0].data = 0x40;
        g_againx = 16;
    }
    else if ((g_againx >= 8) && (g_againx < 16)) {
        params->tGain_hdr_sef_reg[0].data = 0x30;
        g_againx = 8;
    }
    else if ((g_againx >= 4) && (g_againx < 8)) {
        params->tGain_hdr_sef_reg[0].data = 0x20;
        g_againx = 4;
    }
    else if ((g_againx >= 2) && (g_againx < 4)) {
        params->tGain_hdr_sef_reg[0].data = 0x10;
        g_againx = 2;
    }
    else {
        params->tGain_hdr_sef_reg[0].data = 0x00;
        g_againx = 1;
    }

    //if gain=24x -> 16x + 8x; 8x need set dgain to 1.5 -> 16 x 1.5 = 24x
    //ex0: 31x ex1:4884
    params->tGain_hdr_sef_reg[2].data = 0x00;
    g_again0 = g_againx*1024;//16*1024// 16384        //2048

    g_dgain = (gain - g_again0);//15x 31744 - 16384 = 15360     4884-4096=788

    g_again1 = g_again0/2;//8x //2048
    if (g_dgain >= g_again1) {//788 > 2048 -->0
        params->tGain_hdr_sef_reg[2].data |= (1 << 7);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//1024
    if (g_dgain >= g_again1) {//788 > 1024 -->0
        params->tGain_hdr_sef_reg[2].data |= (1 << 6);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//2x//512
    if (g_dgain >= g_again1) {//788 > 512 -->1 788-512 = 276
        params->tGain_hdr_sef_reg[2].data |= (1 << 5);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//1x//256
    if (g_dgain >= g_again1) {//276 > 256 --> 1 276-256=20
        params->tGain_hdr_sef_reg[2].data |= (1 << 4);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//0.5x//128
        if (g_dgain >= g_again1) {//20 > 128 --> 0
        params->tGain_hdr_sef_reg[2].data |= (1 << 3);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//0.25x//64
    if (g_dgain > g_again1)
        params->tGain_hdr_sef_reg[2].data |= (1 << 2);

//    SENSOR_DMSG("[%s] !set gain/reg=%u, reg=%02x,%02x,%02x\n", __FUNCTION__, gain, params->tGain_hdr_lef_reg[0].data,params->tGain_hdr_lef_reg[1].data,params->tGain_hdr_lef_reg[2].data);

    params->reg_dirty = true;
    return SUCCESS;
}

static int pCus_GetShutterInfo_hdr_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_AEStatusNotify_hdr_sef(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            if(params->reg_dirty)
            {
                //SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
                //SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_hdr_sef_reg, ARRAY_SIZE(gain_HDR_SEF1_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_hdr_lef_reg, ARRAY_SIZE(gain_HDR_LEF_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_hdr_sef_reg, ARRAY_SIZE(expo_HDR_SEF_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_hdr_lef_reg, ARRAY_SIZE(expo_HDR_LEF_reg));
                params->reg_dirty=false;
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}


static int pCus_init_hdr_lef(ms_cus_sensor *handle)
{
    return SUCCESS;
}

static int pCus_poweron_hdr_lef(ms_cus_sensor *handle, u32 idx)
{
#if 0
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG( "[%s] ", __FUNCTION__ );

    sensor_if->PowerOff(CUS_SENSOR_PAD_GROUP_B/*TBD?*/, !handle->pwdn_POLARITY );
    MsSleep(RTK_MS_TO_TICK(10));//usleep( 500 );

    //sensor_if->Reset(CUS_SENSOR_PAD_GROUP_B, !handle->reset_POLARITY );
    //MsSleep(RTK_MS_TO_TICK(5));//usleep( 500 );

    // pure power on
    sensor_if->PowerOff(CUS_SENSOR_PAD_GROUP_B/*TBD?*/, !handle->pwdn_POLARITY );
#endif
    return SUCCESS;
}

static int pCus_poweroff_hdr_lef(ms_cus_sensor *handle, u32 idx)
{
#if 0
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG( "[%s] power low\n", __FUNCTION__ );

    sensor_if->PowerOff(CUS_SENSOR_PAD_GROUP_B/*TBD?*/, handle->pwdn_POLARITY );
    MsSleep(RTK_MS_TO_TICK(5));//usleep( 1000 );
#endif
    return SUCCESS;
}


static int pCus_GetSensorID_hdr_lef(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}

static int pCus_GetOrien_hdr_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    *orit = CUS_ORIT_M0F0;
    return SUCCESS;
}

static int pCus_SetOrien_hdr_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    return SUCCESS;
}

static int pCus_GetFPS_hdr_lef(ms_cus_sensor *handle)
{
    hm5532_params *params = (hm5532_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | params->tVts_reg[1].data;

    if (lane_num == 4)
    {
        if (params->expo.fps >= 5000)
        	params->expo.preview_fps = vts_25fps_HDR_4lane*max_fps*1000/tVts;
        else
        	params->expo.preview_fps = vts_25fps_HDR_4lane*max_fps/tVts;
    }
    else if (lane_num == 2)
    {
        if (params->expo.fps >= 5000)
        	params->expo.preview_fps = vts_30fps_HDR_2lane*max_fps*1000/tVts;
        else
        	params->expo.preview_fps = vts_30fps_HDR_2lane*max_fps/tVts;
    }
    return  params->expo.preview_fps;
}

static int pCus_SetFPS_hdr_lef(ms_cus_sensor *handle, u32 fps)
{
    return SUCCESS;
}

static int pCus_AEStatusNotify_hdr_lef(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
//    hm5532_params *params = (hm5532_params *)handle->private_data;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             //SENSOR_DMSG("[%s] 444!!\n", __FUNCTION__);
             break;
        case CUS_FRAME_ACTIVE:
            break;
        default :
             break;
    }
    return SUCCESS;
}


static int pCus_GetAEUSecs_hdr_lef(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines  = (u32)(params->tExpo_hdr_lef_reg[1].data);
    lines |= (u32)(params->tExpo_hdr_lef_reg[0].data)<<8;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;

}

static int pCus_SetAEUSecs_hdr_lef(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    lines = (1000*us)/Preview_line_period;

    if (lines >params->expo.vts-2) {
        vts = lines +2;
    }
    else
        vts=params->expo.vts;

//    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
//                us,
//                lines,
//                params->expo.vts
//                );

    params->tExpo_hdr_lef_reg[0].data = (lines>>8) & 0xff;
    params->tExpo_hdr_lef_reg[1].data = (lines)    & 0xff;

    params->tVts_reg[0].data = (vts >> 8) & 0xff;
    params->tVts_reg[1].data = (vts >> 0) & 0xff;

    params->reg_dirty = true;
  return SUCCESS;
}


static int pCus_GetAEGain_hdr_lef(ms_cus_sensor *handle, u32* gain)
{
    int rc = SUCCESS;
    int again, dgain;
    hm5532_params *params = (hm5532_params *)handle->private_data;

    again  = 1 << ((params->tGain_hdr_lef_reg[0].data&0xf0)>>4);
//    again *= (16+(params->tGain_hdr_lef_reg[0].data&0x0f)) * 64;

    dgain  = (params->tGain_hdr_lef_reg[1].data&0x03)*1024;
    dgain += (params->tGain_hdr_lef_reg[2].data)*4;

    *gain = (again*dgain) / 1024;

    SENSOR_DMSG("[%s] again/gain = 0x%x,%x/0x%x\n", __FUNCTION__, params->tGain_reg[0].data,again, *gain);
    return rc;
}


static int pCus_SetAEGain_hdr_lef(ms_cus_sensor *handle, u32 gain)
{ 
    hm5532_params *params = (hm5532_params *)handle->private_data;
    u32 g_again0 = 0;
    u32 g_again1 = 0;

    u32 g_againx = 0;
    u32 g_dgain = 0;

    g_againx = gain/1024;
    if (g_againx > 32)
        g_againx = 32;

    if (g_againx >= 32) {
        params->tGain_hdr_lef_reg[0].data = 0x50;
        g_againx = 32;
    }
    else if ((g_againx >= 16) && (g_againx < 32)) {
        params->tGain_hdr_lef_reg[0].data = 0x40;
        g_againx = 16;
    }
    else if ((g_againx >= 8) && (g_againx < 16)) {
        params->tGain_hdr_lef_reg[0].data = 0x30;
        g_againx = 8;
    }
    else if ((g_againx >= 4) && (g_againx < 8)) {
        params->tGain_hdr_lef_reg[0].data = 0x20;
        g_againx = 4;
    }
    else if ((g_againx >= 2) && (g_againx < 4)) {
        params->tGain_hdr_lef_reg[0].data = 0x10;
        g_againx = 2;
    }
    else {
        params->tGain_hdr_lef_reg[0].data = 0x00;
        g_againx = 1;
    }

    //if gain=24x -> 16x + 8x; 8x need set dgain to 1.5 -> 16 x 1.5 = 24x
    //ex0: 31x ex1:4884
    params->tGain_hdr_lef_reg[2].data = 0x00;
    g_again0 = g_againx*1024;//16*1024// 16384        //2048

    g_dgain = (gain - g_again0);//15x 31744 - 16384 = 15360        4884-4096=788

    g_again1 = g_again0/2;//8x //2048
    if (g_dgain >= g_again1) {//788 > 2048 -->0
        params->tGain_hdr_lef_reg[2].data |= (1 << 7);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//1024
    if (g_dgain >= g_again1) {//788 > 1024 -->0
        params->tGain_hdr_lef_reg[2].data |= (1 << 6);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//2x//512
    if (g_dgain >= g_again1) {//788 > 512 -->1 788-512 = 276
        params->tGain_hdr_lef_reg[2].data |= (1 << 5);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//1x//256
    if (g_dgain >= g_again1) {//276 > 256 --> 1    276-256=20
        params->tGain_hdr_lef_reg[2].data |= (1 << 4);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//0.5x//128
        if (g_dgain >= g_again1) {//20 > 128 --> 0
        params->tGain_hdr_lef_reg[2].data |= (1 << 3);
        g_dgain -= g_again1;
    }

    g_again1 = g_again1/2;//0.25x//64
    if (g_dgain > g_again1)
        params->tGain_hdr_lef_reg[2].data |= (1 << 2);

//    SENSOR_DMSG("[%s] !set gain/reg=%u, reg=%02x,%02x,%02x\n", __FUNCTION__, gain, params->tGain_hdr_lef_reg[0].data,params->tGain_hdr_lef_reg[1].data,params->tGain_hdr_lef_reg[2].data);

    params->reg_dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain_hdr(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN_HDR;
    *max = SENSOR_MAX_GAIN_HDR;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs_hdr_lef(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetShutterInfo_hdr_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_SetAEGain_cal_hdr_lef(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity_hdr_lef(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    hm5532_params *params;

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
    params = (hm5532_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"HM5532_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth = 0xAA0;
    handle->video_res_supported.res[0].nOutputHeight = 0x820;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2720x2080@30fps");
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

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    handle->sat_mingain = 1024;
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init;
    handle->pCus_sensor_poweron     = pCus_poweron;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien;
    handle->pCus_sensor_GetFPS            = pCus_GetFPS;
    handle->pCus_sensor_SetFPS            = pCus_SetFPS;
    handle->pCus_sensor_GetSensorCap      = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode    = pCus_SetPatternMode;

    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;
    handle->pCus_sensor_GetShutterInfo  = pCus_GetShutterInfo;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;

    params->expo.vts = vts_30fps;
    params->expo.fps = 30;
    params->reg_dirty = false;
    params->orien_dirty = false;
    //SENSOR_DMSG("[%s] VTS=%d!\n", __FUNCTION__, params->expo.vts);
    return SUCCESS;
}

static int cus_camsensor_init_handle_hdr_sef(ms_cus_sensor* drv_handle)
{

    ms_cus_sensor *handle = drv_handle;
    hm5532_params *params = NULL;

    cus_camsensor_init_handle(drv_handle);
    params = (hm5532_params *)handle->private_data;

    //SENSOR_DMSG("[%s] HDR SEF INIT!\n", __FUNCTION__);
    sprintf(handle->model_id,"HM5533_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->data_prec   = SENSOR_DATAPREC_HDR;
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_num;//hdr_lane_num;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH_HDR;
    handle->video_res_supported.res[0].height = Preview_HEIGHT_HDR; //TBD. Workaround for Sony DOL HDR mode
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS_HDR;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS_HDR;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth = 0xA00;//0x79C;
    handle->video_res_supported.res[0].nOutputHeight = 0x780;//0x449;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1920@25fps_HDR");

    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_hdr;

    if (lane_num == 4)
        handle->pCus_sensor_init        = pCus_init_mipi4lane_hdr;
    else if (lane_num == 2)
        handle->pCus_sensor_init        = pCus_init_mipi2lane_hdr;
    else
      handle->pCus_sensor_init        = pCus_init_mipi4lane_hdr;

    handle->pCus_sensor_GetFPS          = pCus_GetFPS_hdr_sef;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_hdr_sef;

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify_hdr_sef;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_hdr_sef;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs_hdr_sef;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_hdr_sef;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_sef;

    handle->pCus_sensor_GetShutterInfo  = pCus_GetShutterInfo_hdr_sef;

    if (lane_num == 4)
        params->expo.vts = vts_25fps_HDR_4lane;
    else if (lane_num == 2)
        params->expo.vts = vts_30fps_HDR_2lane;
    else
        params->expo.vts = vts_25fps_HDR_4lane;


//HDR    params->expo.expo_lines = 673;

#if 1

    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_delay       = 2;//SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay    = 2;//SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL;

    handle->ae_gain_ctrl_num = 2;
    handle->ae_shutter_ctrl_num = 2;
#endif
    return SUCCESS;
}

static int cus_camsensor_init_handle_hdr_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    hm5532_params *params;

    //SENSOR_DMSG("[%s] HDR LEF INIT!\n", __FUNCTION__);
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
    params = (hm5532_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tExpo_hdr_lef_reg, expo_HDR_LEF_reg, sizeof(expo_HDR_LEF_reg));
    memcpy(params->tExpo_hdr_sef_reg, expo_HDR_SEF_reg, sizeof(expo_HDR_SEF_reg));
    memcpy(params->tGain_hdr_lef_reg, gain_HDR_LEF_reg, sizeof(gain_HDR_LEF_reg));
    memcpy(params->tGain_hdr_sef_reg, gain_HDR_SEF1_reg, sizeof(gain_HDR_SEF1_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"HM5533_MIPI_HDR_LEF");

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
    handle->interface_attr.attr_mipi.mipi_lane_num = hdr_lane_num;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num =  0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH_HDR;
    handle->video_res_supported.res[0].height = Preview_HEIGHT_HDR; //TBD. Workaround for Sony DOL HDR mode
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS_HDR;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS_HDR;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth = 0xA00;
    handle->video_res_supported.res[0].nOutputHeight = 0x780;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2560x1920@25fps_HDR");

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED;//ParaMclk(SENSOR_DRV_PARAM_MCLK());

    //polarity
    /////////////////////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    //handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////



    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay       = 2;//SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay    = 2;//SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL;

    handle->ae_gain_ctrl_num = 2;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain = SENSOR_MIN_GAIN;//g_sensor_ae_min_gain;
    //handle->dgain_remainder = 0;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_hdr_lef;
    //handle->pCus_sensor_powerupseq  = pCus_powerupseq   ;
    handle->pCus_sensor_poweron     = pCus_poweron_hdr_lef;
    handle->pCus_sensor_poweroff    = pCus_poweroff_hdr_lef;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID_hdr_lef;

    handle->pCus_sensor_GetVideoResNum = NULL;
    handle->pCus_sensor_GetVideoRes       = NULL;
    handle->pCus_sensor_GetCurVideoRes  = NULL;
    handle->pCus_sensor_SetVideoRes       = NULL;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien_hdr_lef;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien_hdr_lef;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_hdr_lef;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_hdr_lef;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap_hdr_dol_lef;
    handle->pCus_sensor_SetPatternMode = pCus_SetPatternMode;//imx307_SetPatternMode_hdr_dol_lef;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify_hdr_lef;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_hdr_lef;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs_hdr_lef;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_hdr_lef;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_dol_lef;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_lef;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain_hdr;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_hdr_lef;
    handle->pCus_sensor_GetShutterInfo  = pCus_GetShutterInfo_hdr_lef;
    //handle->pCus_sensor_GetDGainRemainder = pCus_GetDGainRemainder;

    //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_lef;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity_hdr_lef;

    params->expo.vts = vts_25fps_HDR_4lane;
//  params->expo.expo_lines = 673;
//  if (CUS_CMU_CLK_36MHZ == handle->mclk)
//      params->expo.fps = 29;
//  else
//      params->expo.fps = 30;

//  params->dirty = false;


    return SUCCESS;
}



SENSOR_DRV_ENTRY_IMPL_END_EX(  HM5532,
                            cus_camsensor_init_handle,
                            cus_camsensor_init_handle_hdr_sef,
                            cus_camsensor_init_handle_hdr_lef,
                            hm5532_params
                         );
