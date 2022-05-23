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
#include <drv_sensor_init_table.h> //TODO: move this header to drv_sensor_common.h
#include <PS5270_MIPI_init_table.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(PS5270);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif


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

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_14    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_MAXGAIN      128
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     128                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
#define Preview_line_period 24535                  //
//#define Line_per_second     32727
#define vts_30fps  1630//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 1626                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1536                   //resolution Width when preview
#define Preview_HEIGHT      1536                   //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR     0x90                    //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;
//static volatile long long framecount=0;
static volatile int fps_delay=5;
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
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
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;
    struct {
        int sclk;
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
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[3];
    I2C_ARRAY tExpo_reg[7];
    I2C_ARRAY tExpo_reg_prev[7]; //keep previous to reduce I2C data
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
    bool first_use;
    bool sghd_flag;
    u32 sns_const1;
    bool ne_dirty;
} ps5270_params;
// set sensor ID address and data

typedef struct {
    u32 total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

const I2C_ARRAY Sensor_id_table[] =
{
{0x00, 0x52},      // {address of ID, ID },
{0x01, 0x70},
};

#define BANK1_0X0E (0x01)
#define BANK1_0X0F (0xF4)
#define BANK1_0X5F (0x2C)
#define BANK1_0X60 (0xC2)
const I2C_ARRAY Sensor_init_table_HDR[] =
{
    {0xEF, 0x05}, //bank5
    {0x0F, 0x00},
    {0x42, 0x00},
#if 0
    {0x43, 0x06},
#else
    {0x43, 0x0E},//mipi improvement
#endif
    {0xED, 0x01},
    {0xEF, 0x01}, //bank1
    {0xF5, 0x01},
    {0x09, 0x01},
    {0xEF, 0x00},
    {0x10, 0x20},
    {0x11, 0x80},
    {0x3A, 0x02},
    {0x3C, 0x07},
    {0x5F, BANK1_0X5F},
    {0x60, BANK1_0X60},
    {0x61, 0xFD},
    {0x69, 0x40},
    {0x6A, 0x80},
    {0x6E, 0x80},
    {0x6F, 0x5A},
    {0x71, 0x2D},
    {0x7F, 0x28},
    {0x85, 0x1E},
    {0x87, 0x27},
    {0x90, 0x02},
    {0x9B, 0x0B},
    {0x9E, 0x42},
    {0xA0, 0x05},
    {0xA1, 0x00},
    {0xA2, 0x0A},
    {0xA3, 0x04},
    {0xA4, 0xFE},
    {0xBE, 0x05},
    {0xE1, 0x03},
    {0xE2, 0x02},
    {0xE5, 0x03},
    {0xE6, 0x02},
    {0xED, 0x01},
    {0xEF, 0x01}, //bank1
    {0x05, 0x0B},
    {0x0A, 0x06},
    {0x0B, 0x5D},
    {0x0C, 0x00},
    {0x0D, 0x04},
    {0x0E, BANK1_0X0E},
    {0x0F, BANK1_0X0F},
    {0x10, 0xB0},
    {0x11, 0x4A},
    {0x19, 0x3F},
    {0x1E, 0x04},
    {0x20, 0x06},
    {0x27, 0x10},
    {0x28, 0x7C},
    {0x29, 0x0A},
    {0x2A, 0x0A},
    {0x2F, 0x02},
    {0x37, 0x18},
    {0x38, 0x03},
    {0x39, 0x22},
    {0x3B, 0x70},
    {0x3E, 0x10},
    {0x3F, 0x7C},
    {0x41, 0x13},
    {0x42, 0xF4},
    {0x44, 0x00},
    {0x56, 0x02},
    {0x60, 0x64},
    {0x67, 0x11},
    {0x68, 0x4A},
    {0x69, 0x4A},
    {0x74, 0x0E},
    {0x75, 0x4C},
    {0x7A, 0x03},
    {0x7B, 0xB1},
    {0x7C, 0x06},
    {0x7D, 0x77},
    {0x8F, 0x07},
    {0x92, 0x00},
    {0x96, 0x80},
    {0x9B, 0x0A},
    {0x9F, 0x01},
    {0xA1, 0x40},
    {0xA2, 0x40},
    {0xA4, 0x0C},
    {0xA5, 0x06},
    {0xA6, 0x00},
    {0xA8, 0x06},
    {0xA9, 0x06},
    {0xAA, 0x00},
    {0xAB, 0x01},
    {0xB0, 0x00},
    {0xB4, 0x00},
    {0xBC, 0x01},
    {0xBD, 0x09},
    {0xD7, 0x0E},
    {0xE2, 0x4F},
    {0xE3, 0x01},
    {0xE4, 0x00},
    {0xE6, 0x01},
    {0xEA, 0xBB},
    {0xF5, 0x00},
    {0xF0, 0x03},
    {0xF1, 0x19},
    {0xF2, 0x29},
    {0xF5, 0x10},
    {0xF8, 0x08},
    {0xFA, 0x75},
    {0xFC, 0x04},
    {0xFD, 0x20},
    {0x09, 0x01},
    {0xEF, 0x02},  //bank2
    //{0x33, 0x85},
    {0x33, 0x81},   //blc
    {0x36, 0x00},   //blc
    {0x4E, 0x02},
    {0x4F, 0x05},
    {0xED, 0x01},
    {0xEF, 0x05},  // bank5
    {0x06, 0x06/*0x05*/},  //0x05=RAW12, 0x06=RAW14
    {0x09, 0x09},
    {0x0A, 0x05},
    {0x0C, 0x04},
    {0x0D, 0x5E},
    {0x0E, 0x01},
    {0x0F, 0x00},
    {0x10, 0x02},
    {0x11, 0x01},
    {0x15, 0x07},
    {0x17, 0x05},
    {0x18, 0x02},
    {0x1B, 0x03},
    {0x1C, 0x04},
    {0x25, 0x01},
    {0x38, 0x0E},
    {0x3B, 0x01},
    {0x42, 0x00},
    {0x3F, 0x00},
    {0x40, 0x19},
#if 0
    {0x41, 0x1C},
    {0x43, 0x06},
#else
    {0x41, 0x3A},//mipi improvement
    {0x43, 0x0E},//mipi improvement
#endif

    {0x47, 0x05},
#if 0
    {0x54, 0x0A},
    {0x56, 0x0A},
#else
    {0x54, 0x0F},//mipi improvement
    {0x56, 0x0F},//mipi improvement
#endif
    {0x58, 0x01},
    {0x42, 0x01},
    //{0x5B, 0x00},
	{0x5B, 0x10}, //james.yang , 05/15/2019 primesensor
    {0x5C, 0x00},
#if 0
    {0x94, 0x04},
#else
    {0x94, 0x00},//mipi improvement
#endif
    {0xB0, 0x01},
    {0xB1, 0x00},
    {0xED, 0x01},
    {0xEF, 0x06}, //bank6
    {0x00, 0x0C},
    {0x02, 0x13},
    {0x03, 0x8D},
    {0x04, 0x05},
    {0x05, 0x01},
    {0x07, 0x02},
    {0x08, 0x02},
    {0x09, 0x01},
    {0x0A, 0x01},
    {0x0B, 0x82},
    {0x0C, 0xFA},
    {0x0D, 0xDB},
    {0x0F, 0x02},
    {0x10, 0x58},
    {0x11, 0x02},
    {0x12, 0x58},
    {0x17, 0x02},
    {0x18, 0x58},
    {0x19, 0x02},
    {0x1A, 0x58},
    {0x28, 0x02},
    {0x2A, 0xF0},
    {0x2B, 0xB2},
    {0x5E, 0x90},
    {0xBF, 0xC8},
    {0xED, 0x01},
    {0xEF, 0x03}, //bank 3
    {0x00, 0xEF},
    {0x01, 0x01},
    {0x02, 0x31},
    {0x03, 0x00},
    {0x04, 0x32},
    {0x05, 0x00},
    {0x06, 0x33},
    {0x07, 0x00},
    {0x08, 0x3B},
    {0x09, 0x4C},
    {0x0A, 0x3E},
    {0x0B, 0x30},
    {0x0C, 0x3F},
    {0x0D, 0xFF},
    {0x0E, 0x42},
    {0x0F, 0xFF},
    {0x10, 0x43},
    {0x11, 0xFF},
    {0x12, 0x44},
    {0x13, 0xFF},
    {0x14, 0x47},
    {0x15, 0x08},
    {0x16, 0x4B},
    {0x17, 0x08},
    {0x18, 0x4E},
    {0x19, 0x00},
    {0x1a, 0x52},
    {0x1b, 0x00},
    {0x1c, 0x53},
    {0x1d, 0x00},
    {0x1e, 0x58},
    {0x1f, 0x00},
    {0x20, 0x59},
    {0x21, 0x00},
    {0x22, 0x5A},
    {0x23, 0x16},
    {0x24, 0x64},
    {0x25, 0x00},
    {0x26, 0x65},
    {0x27, 0x00},
    {0x28, 0x66},
    {0x29, 0x00},
    {0x2a, 0x67},
    {0x2b, 0x00},
    {0x2c, 0x68},
    {0x2d, 0x00},
    {0x2e, 0x69},
    {0x2f, 0x00},
    {0x30, 0x74},
    {0x31, 0x0B},
    {0x32, 0x75},
    {0x33, 0xB8},
    {0x34, 0x7A},
    {0x35, 0x00},
    {0x36, 0x7B},
    {0x37, 0x96},
    {0x38, 0x7C},
    {0x39, 0x0B},
    {0x3a, 0x7D},
    {0x3b, 0xB8},
    {0x3c, 0xBC},
    {0x3d, 0x81},
    {0x3e, 0x38},
    {0x3f, 0x04},
    {0xef, 0x01}, //bank1
    {0x9f, 0x03},
    {0xef, 0x03}, //bank3
    {0x00, 0xef},
    {0x01, 0x01},
    {0x02, 0x31},
    {0x03, 0x03},
    {0x04, 0x32},
    {0x05, 0xFF},
    {0x06, 0x33},
    {0x07, 0x70},
    {0x08, 0x3B},
    {0x09, 0x70},
    {0x0A, 0x3E},
    {0x0B, 0x10},
    {0x0C, 0x3F},
    {0x0D, 0x7C},
    {0x0E, 0x42},
    {0x0F, 0xF4},
    {0x10, 0x43},
    {0x11, 0x70},
    {0x12, 0x44},
    {0x13, 0x00},
    {0x14, 0x47},
    {0x15, 0x00},
    {0x16, 0x4B},
    {0x17, 0x00},
    {0x18, 0x4E},
    {0x19, 0x08},
    {0x1a, 0x52},
    {0x1b, 0x02},
    {0x1c, 0x53},
    {0x1d, 0x08},
    {0x1e, 0x58},
    {0x1f, 0x02},
    {0x20, 0x59},
    {0x21, 0x08},
    {0x22, 0x5A},
    {0x23, 0x02},
    {0x24, 0x64},
    {0x25, 0x03},
    {0x26, 0x65},
    {0x27, 0xFF},
    {0x28, 0x66},
    {0x29, 0x70},
    {0x2a, 0x67},
    {0x2b, 0x11},
    {0x2c, 0x68},
    {0x2d, 0x4A},
    {0x2e, 0x69},
    {0x2f, 0x4A},
    {0x30, 0x74},
    {0x31, 0x0E},
    {0x32, 0x75},
    {0x33, 0x4C},
    {0x34, 0x7A},
    {0x35, 0x03},
    {0x36, 0x7B},
    {0x37, 0xB1},
    {0x38, 0x7C},
    {0x39, 0x06},
    {0x3a, 0x7D},
    {0x3b, 0x77},
    {0x3c, 0xBC},
    {0x3d, 0x01},
    {0x3e, 0x38},
    {0x3f, 0x03},
    {0xEF, 0x01}, //bank1
    {0x9f, 0x00},
    {0xEF, 0x00},
    {0x11, 0x00},
    {0xEF, 0x05}, //bank5
    {0x3B, 0x00},
    {0xED, 0x01},
    {0xEF, 0x01}, //bank1
    {0x02, 0x73},
    {0x09, 0x01},
    {0xEF, 0x05}, //bank5
    {0x0F, 0x01},
    {0xED, 0x01},
    {0xFF, 0x78}, //{0xDL, 0x50}, //delay 3 frames, 120ms
    {0xEF, 0x05}, //bank5
    {0x25, 0x00},
    {0xED, 0x01},


};

#if 0
const I2C_ARRAY Sensor_init_table[] =
{
    {0xEF, 0x05}, //bank5
    {0x0F, 0x00},
    {0x42, 0x00},
#if 0
    {0x43, 0x06},
#else
    {0x43, 0x0E},//mipi improvement
#endif
    {0xED, 0x01},
    {0xEF, 0x01}, //bank1
    {0xF5, 0x01},
    {0x09, 0x01},
    {0xEF, 0x00}, //bank0
    {0x10, 0x20},
    {0x11, 0x80},
    {0x3A, 0x02},
    {0x3C, 0x07},
    {0x5F, BANK1_0X5F},
    {0x60, BANK1_0X60},
    {0x61, 0xFD},
    {0x69, 0x40},
    {0x6A, 0x80},
    {0x6E, 0x80},
    {0x6F, 0x5A},
    {0x71, 0x2D},
    {0x7F, 0x28},
    {0x85, 0x1E},
    {0x87, 0x27},
    {0x90, 0x02},
    {0x9B, 0x0B},
    {0x9E, 0x42},
    {0xA0, 0x05},
    {0xA1, 0x00},
    {0xA2, 0x0A},
    {0xA3, 0x04},
    {0xA4, 0xFE},
    {0xBE, 0x05},
    {0xE1, 0x03},
    {0xE2, 0x02},
    {0xE5, 0x03},
    {0xE6, 0x02},
    {0xED, 0x01},
    {0xEF, 0x01},
    {0x05, 0x0B},
    {0x0A, 0x06},
    {0x0B, 0x5D},
    {0x0C, 0x00},
    {0x0D, 0x04},
    {0x0E, BANK1_0X0E},
    {0x0F, BANK1_0X0F},
    {0x10, 0xB0},
    {0x11, 0x4A},
    {0x19, 0x3F},
    {0x1E, 0x04},
    {0x20, 0x06},
    {0x27, 0x10},
    {0x28, 0x7C},
    {0x29, 0x0A},
    {0x2A, 0x0A},
    {0x2F, 0x02},
    {0x37, 0x18},
    {0x38, 0x03},
    {0x39, 0x22},
    {0x3B, 0x70},
    {0x3E, 0x10},
    {0x3F, 0x7C},
    {0x41, 0x13},
    {0x42, 0xF4},
    {0x44, 0x00},
    {0x56, 0x02},
    {0x60, 0x64},
    {0x67, 0x11},
    {0x68, 0x4A},
    {0x69, 0x4A},
    {0x74, 0x0E},
    {0x75, 0x4C},
    {0x7A, 0x03},
    {0x7B, 0xB1},
    {0x7C, 0x06},
    {0x7D, 0x77},
    {0x8F, 0x02}, //0x02:non-HDR  0x07:HDR
    {0x92, 0x00},
    {0x96, 0x80},
    {0x9B, 0x0A},
    {0x9F, 0x01},
    {0xA1, 0x40},
    {0xA2, 0x40},
    {0xA4, 0x0C},
    {0xA5, 0x06},
    {0xA6, 0x00},
    {0xA8, 0x06},
    {0xA9, 0x06},
    {0xAA, 0x00},
    {0xAB, 0x01},
    {0xB0, 0x00},
    {0xB4, 0x00},
    {0xBC, 0x01},
    {0xBD, 0x09},
    {0xD7, 0x0E},
    {0xE2, 0x4F},
    {0xE3, 0x01},
    {0xE4, 0x00},
    {0xE6, 0x01},
    {0xEA, 0xBB},
    {0xF5, 0x00},
    {0xF0, 0x03},
    {0xF1, 0x19},
    {0xF2, 0x29},
    {0xF5, 0x10},
    {0xF8, 0x08},
    {0xFA, 0x75},
    {0xFC, 0x04},
    {0xFD, 0x20},
    {0x09, 0x01},
    {0xEF, 0x02},  //bank2
    //{0x33, 0x85},
    {0x33, 0x81},  //blc
    {0x36, 0x00},  //blc
    {0x4E, 0x02},
    {0x4F, 0x05},
    {0xED, 0x01},
    {0xEF, 0x05},  // bank5
    {0x06, 0x06/*0x05*/},  //0x05=RAW12, 0x06=RAW14
    {0x09, 0x09},
    {0x0A, 0x05},
    {0x0C, 0x04},
    {0x0D, 0x5E},
    {0x0E, 0x01},
    {0x0F, 0x00},
    {0x10, 0x02},
    {0x11, 0x01},
    {0x15, 0x07},
    {0x17, 0x05},
    {0x18, 0x02},
    {0x1B, 0x03},
    {0x1C, 0x04},
    {0x25, 0x01},
    {0x38, 0x0E},
    {0x3B, 0x01},
    {0x42, 0x00},
    {0x3F, 0x00},
    {0x40, 0x19},
#if 0
    {0x41, 0x1C},
    {0x43, 0x06},
#else
    {0x41, 0x3A},//mipi improvement
    {0x43, 0x0E},//mipi improvement
#endif
    {0x47, 0x05},
#if 0
    {0x54, 0x0A},
    {0x56, 0x0A},
#else
    {0x54, 0x0F},//mipi improvement
    {0x56, 0x0F},//mipi improvement
#endif
    {0x58, 0x01},
    {0x42, 0x01},
    //{0x5B, 0x00},
	{0x5B, 0x10}, //james.yang , 05/15/2019 primesensor
    {0x5C, 0x00},
#if 0
    {0x94, 0x04},
#else
    {0x94, 0x00},//mipi improvement
#endif
    {0xB0, 0x01},
    {0xB1, 0x00},
    {0xED, 0x01},
    {0xEF, 0x06}, //bank6
    {0x00, 0x0C},
    {0x02, 0x13},
    {0x03, 0x8D},
    {0x04, 0x05},
    {0x05, 0x01},
    {0x07, 0x02},
    {0x08, 0x02},
    {0x09, 0x01},
    {0x0A, 0x01},
    {0x0B, 0x82},
    {0x0C, 0xFA},
    {0x0D, 0xDB},
    {0x0F, 0x02},
    {0x10, 0x58},
    {0x11, 0x02},
    {0x12, 0x58},
    {0x17, 0x02},
    {0x18, 0x58},
    {0x19, 0x02},
    {0x1A, 0x58},
    {0x28, 0x02},
    {0x2A, 0xF0},
    {0x2B, 0xB2},
    {0x5E, 0x90},
    {0xBF, 0xC8},
    {0xED, 0x01},
    {0xEF, 0x03}, //bank 3
    {0x00, 0xEF},
    {0x01, 0x01},
    {0x02, 0x31},
    {0x03, 0x00},
    {0x04, 0x32},
    {0x05, 0x00},
    {0x06, 0x33},
    {0x07, 0x00},
    {0x08, 0x3B},
    {0x09, 0x4C},
    {0x0A, 0x3E},
    {0x0B, 0x30},
    {0x0C, 0x3F},
    {0x0D, 0xFF},
    {0x0E, 0x42},
    {0x0F, 0xFF},
    {0x10, 0x43},
    {0x11, 0xFF},
    {0x12, 0x44},
    {0x13, 0xFF},
    {0x14, 0x47},
    {0x15, 0x08},
    {0x16, 0x4B},
    {0x17, 0x08},
    {0x18, 0x4E},
    {0x19, 0x00},
    {0x1a, 0x52},
    {0x1b, 0x00},
    {0x1c, 0x53},
    {0x1d, 0x00},
    {0x1e, 0x58},
    {0x1f, 0x00},
    {0x20, 0x59},
    {0x21, 0x00},
    {0x22, 0x5A},
    {0x23, 0x16},
    {0x24, 0x64},
    {0x25, 0x00},
    {0x26, 0x65},
    {0x27, 0x00},
    {0x28, 0x66},
    {0x29, 0x00},
    {0x2a, 0x67},
    {0x2b, 0x00},
    {0x2c, 0x68},
    {0x2d, 0x00},
    {0x2e, 0x69},
    {0x2f, 0x00},
    {0x30, 0x74},
    {0x31, 0x0B},
    {0x32, 0x75},
    {0x33, 0xB8},
    {0x34, 0x7A},
    {0x35, 0x00},
    {0x36, 0x7B},
    {0x37, 0x96},
    {0x38, 0x7C},
    {0x39, 0x0B},
    {0x3a, 0x7D},
    {0x3b, 0xB8},
    {0x3c, 0xBC},
    {0x3d, 0x81},
    {0x3e, 0x38},
    {0x3f, 0x04},
    {0xef, 0x01}, //bank1
    {0x9f, 0x03},
    {0xef, 0x03}, //bank3
    {0x00, 0xef},
    {0x01, 0x01},
    {0x02, 0x31},
    {0x03, 0x03},
    {0x04, 0x32},
    {0x05, 0xFF},
    {0x06, 0x33},
    {0x07, 0x70},
    {0x08, 0x3B},
    {0x09, 0x70},
    {0x0A, 0x3E},
    {0x0B, 0x10},
    {0x0C, 0x3F},
    {0x0D, 0x7C},
    {0x0E, 0x42},
    {0x0F, 0xF4},
    {0x10, 0x43},
    {0x11, 0x70},
    {0x12, 0x44},
    {0x13, 0x00},
    {0x14, 0x47},
    {0x15, 0x00},
    {0x16, 0x4B},
    {0x17, 0x00},
    {0x18, 0x4E},
    {0x19, 0x08},
    {0x1a, 0x52},
    {0x1b, 0x02},
    {0x1c, 0x53},
    {0x1d, 0x08},
    {0x1e, 0x58},
    {0x1f, 0x02},
    {0x20, 0x59},
    {0x21, 0x08},
    {0x22, 0x5A},
    {0x23, 0x02},
    {0x24, 0x64},
    {0x25, 0x03},
    {0x26, 0x65},
    {0x27, 0xFF},
    {0x28, 0x66},
    {0x29, 0x70},
    {0x2a, 0x67},
    {0x2b, 0x11},
    {0x2c, 0x68},
    {0x2d, 0x4A},
    {0x2e, 0x69},
    {0x2f, 0x4A},
    {0x30, 0x74},
    {0x31, 0x0E},
    {0x32, 0x75},
    {0x33, 0x4C},
    {0x34, 0x7A},
    {0x35, 0x03},
    {0x36, 0x7B},
    {0x37, 0xB1},
    {0x38, 0x7C},
    {0x39, 0x06},
    {0x3a, 0x7D},
    {0x3b, 0x77},
    {0x3c, 0xBC},
    {0x3d, 0x01},
    {0x3e, 0x38},
    {0x3f, 0x03},
    {0xEF, 0x01}, //bank1
    {0x9f, 0x00},
    {0xEF, 0x00},
    {0x11, 0x00},
    {0xEF, 0x05}, //bank5
    {0x3B, 0x00},
    {0xED, 0x01},
    {0xEF, 0x01}, //bank1
    {0x02, 0x73},
    {0x09, 0x01},
    {0xEF, 0x05}, //bank5
    {0x0F, 0x01},
    {0xED, 0x01},
    {0xFF, 0x78}, //{0xDL, 0x50}, //delay 3 frames, 120ms
    {0xEF, 0x05}, //bank5
    {0x25, 0x00},
    {0xED, 0x01},

};
#endif

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};
#if 0
I2C_ARRAY PatternTbl[] = {
    //pattern mode
};
#endif
I2C_ARRAY Current_Mirror_Flip_Tbl[] = {

    {0xEF, 0x01},//M0F0
    {0x1b, 0x00},//bit7,Hflip
    //{0x90, 0x04},//bit[7:4] Cmd_ADC_Latency
    {0x1d, 0x00},//bit7,Vflip

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

    {0xEF, 0x01},//M0F0
    {0x1b, 0x00},//bit7,Hflip
   // {0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x00},//bit7,Vflip

    {0xEF, 0x01},//M1F0
    {0x1b, 0x80},//bit7,Hflip
   // {0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x00},//bit7,Vflip

    {0xEF, 0x01},//M0F1
    {0x1b, 0x00},//bit7,Hflip
    //{0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x80},//bit7,Vflip

    {0xEF, 0x01},//M1F1
    {0x1b, 0x80},//bit7,Hflip
   // {0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x80},//bit7,Vflip

};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const static I2C_ARRAY gain_reg[] = {
    {0xEF, 0x01},
    {0x83, 0x04},//analog gain suggest from 1.25X to 32X
    {0x18, 0x00},
    //{0x97, 0x01},
    //{0x09, 0x01},
};

const static Gain_ARRAY gain_table[]={
    {10000 , 0   },
    {10220 , 1   },
    {10444 , 2   },
    {10672 , 3   },
    {10905 , 4   },
    {11143 , 5   },
    {11387 , 6   },
    {11636 , 7   },
    {11893 , 8   },
    {12151 , 9   },
    {12420 , 10  },
    {12689 , 11  },
    {12970 , 12  },
    {13251 , 13  },
    {13540 , 14  },
    {13838 , 15  },
    {14144 , 16  },
    {14453 , 17  },
    {14766 , 18  },
    {15092 , 19  },
    {15422 , 20  },
    {15760 , 21  },
    {16107 , 22  },
    {16456 , 23  },
    {16821 , 24  },
    {17188 , 25  },
    {17564 , 26  },
    {17949 , 27  },
    {18343 , 28  },
    {18737 , 29  },
    {19149 , 30  },
    {19570 , 31  },
    {20000 , 32  },
    {20439 , 33  },
    {20887 , 34  },
    {21344 , 35  },
    {21810 , 36  },
    {22285 , 37  },
    {22781 , 38  },
    {23273 , 39  },
    {23786 , 40  },
    {24309 , 41  },
    {24839 , 42  },
    {25378 , 43  },
    {25940 , 44  },
    {26511 , 45  },
    {27090 , 46  },
    {27676 , 47  },
    {28287 , 48  },
    {28906 , 49  },
    {29531 , 50  },
    {30184 , 51  },
    {30843 , 52  },
    {31508 , 53  },
    {32201 , 54  },
    {32926 , 55  },
    {33629 , 56  },
    {34362 , 57  },
    {35129 , 58  },
    {35898 , 59  },
    {36670 , 60  },
    {37475 , 61  },
    {38316 , 62  },
    {39159 , 63  },
    {40000 , 64  },
    {40878 , 65  },
    {41775 , 66  },
    {42689 , 67  },
    {43621 , 68  },
    {44570 , 69  },
    {45549 , 70  },
    {46545 , 71  },
    {47573 , 72  },
    {48603 , 73  },
    {49679 , 74  },
    {50756 , 75  },
    {51881 , 76  },
    {53005 , 77  },
    {54162 , 78  },
    {55351 , 79  },
    {56575 , 80  },
    {57812 , 81  },
    {59063 , 82  },
    {60368 , 83  },
    {61687 , 84  },
    {63040 , 85  },
    {64428 , 86  },
    {65826 , 87  },
    {67285 , 88  },
    {68754 , 89  },
    {70257 , 90  },
    {71797 , 91  },
    {73372 , 92  },
    {74950 , 93  },
    {76597 , 94  },
    {78280 , 95  },
    {80000 , 96  },
    {81756 , 97  },
    {83549 , 98  },
    {85378 , 99  },
    {87242 , 100 },
    {89140 , 101 },
    {91123 , 102 },
    {93091 , 103 },
    {95145 , 104 },
    {97234 , 105 },
    {99357 , 106 },
    {101512  ,107},
    {103762  ,108},
    {106045  ,109},
    {108360  ,110},
    {110703  ,111},
    {113149  ,112},
    {115625  ,113},
    {118125  ,114},
    {120737  ,115},
    {123373  ,116},
    {126031  ,117},
    {128805  ,118},
    {131704  ,119},
    {134516  ,120},
    {137450  ,121},
    {140515  ,122},
    {143593  ,123},
    {146679  ,124},
    {149899  ,125},
    {153265  ,126},
    {156635  ,127},
    {160000  ,128},
    {163513  ,129},
    {167013  ,130},
    {170667  ,131},
    {174483  ,132},
    {178281  ,133},
    {182247  ,134},
    {186182  ,135},
    {190290  ,136},
    {194353  ,137},
    {198594  ,138},
    {203024  ,139},
    {207392  ,140},
    {211953  ,141},
    {216720  ,142},
    {221405  ,143},
    {226298  ,144},
    {231086  ,145},
    {236421  ,146},
    {241296  ,147},
    {246747  ,148},
    {252062  ,149},
    {257610  ,150},
    {263408  ,151},
    {269031  ,152},
    {274899  ,153},
    {281029  ,154},
    {286935  ,155},
    {293620  ,156},
    {300073  ,157},
    {306243  ,158},
    {313270  ,159},
    {320000  ,160},
    {327026  ,161},
    {334367  ,162},
    {341333  ,163},
    {348596  ,164},
    {359298  ,165},
    {364089  ,166},
    {372364  ,167},
    {380139  ,168},
    {389169  ,169},
    {397670  ,170},
    {406551  ,171},
    {414785  ,172},
    {424456  ,173},
    {433439  ,174},
    {442811  ,175},
    {452597  ,176},
    {462825  ,177},
    {472161  ,178},
    {483304  ,179},
    {493494  ,180},
    {504123  ,181},
    {515220  ,182},
    {526817  ,183},
    {538947  ,184},
    {549799  ,185},
    {561096  ,186},
    {574877  ,187},
    {587240  ,188},
    {600147  ,189},
    {613633  ,190},
    {625344  ,191},
    {640000  ,192},
    {652749  ,193},
    {668735  ,194},
    {682667  ,195},
    {697191  ,196},
    {712348  ,197},
    {728178  ,198},
    {744727  ,199},
    {762047  ,200},
    {776493  ,201},
    {795340  ,202},
    {811089  ,203},
    {831675  ,204},
    {848912  ,205},
    {866878  ,206},
    {885622  ,207},
    {905193  ,208},
    {925650  ,209},
    {947052  ,210},
    {963765  ,211},
    {986988  ,212},
    {1011358 ,213},
    {1030440 ,214},
    {1050256 ,215},
    {1077895 ,216},
    {1099597 ,217},
    {1122192 ,218},
    {1145734 ,219},
    {1170286 ,220},
    {1195912 ,221},
    {1222687 ,222},
    {1250687 ,223},
    {1280000 ,224},
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
    {0xEF, 0x01},
    {0x0C, 0x00},
    {0x0D, 0x02},
    {0x0E, BANK1_0X0E},	//[3] Cmd_OffNe1[11:8]
    {0x0F, BANK1_0X0F},	//[4] Cmd_OffNe1[7:0]
    {0x5F, BANK1_0X5F},	//[5]
    {0x60, BANK1_0X60},	//[6]
    //{0x09, 0x01},
};

const I2C_ARRAY vts_reg[] = {
    {0xEF, 0x01},
    {0x0A, 0x04},
    {0x0B, 0x64},
    //{0x09, 0x01},
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
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME ps5270


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

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x3C00, 0);

    /*Power ON*/
    res = sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    if(res>=0)//if success
        SENSOR_UDELAY(100);     //T1 = 100us

    /*Reset PIN*/
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    res = sensor_if->Reset(idx, handle->reset_POLARITY);
    if(res>=0)//if success
        SENSOR_UDELAY(1000);    //T3 = 1ms

    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    res = sensor_if->Reset(idx, !handle->reset_POLARITY);
    if(res>=0)//if success
        SENSOR_UDELAY(100);    //T2 = 1us

    /*MCLK ON*/
    res = sensor_if->MCLK(idx, 1, handle->mclk);
    if(res>=0)//if success
        SENSOR_USLEEP(3000);     //T4 = 3ms

    //CamOsPrintf("pCus_poweron = %d us \n",timeGetTimeU()-TStart);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_UDELAY(100);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
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

	SensorReg_Write(0xef,0x00);

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
    SENSOR_DMSG("[%s]ps5270 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ps5270_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    ps5270_params *params = (ps5270_params *)handle->private_data;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
    {
        if(Sensor_init_table[i].reg==0xff)
        {
            SENSOR_MSLEEP(Sensor_init_table[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

#if 0
	    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
        {
            if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            {
                //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                return FAIL;
            }
        }
#endif

    memset(params->tExpo_reg_prev, 0, sizeof(expo_reg));

    params->tVts_reg[1].data = (u16)((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((params->expo.vts >> 0) & 0x00ff);
    params->sghd_flag = 1;  // 1x
    params->sns_const1 = ( (expo_reg[3].data<<8)|expo_reg[4].data ) + ( (expo_reg[5].data<<8)|expo_reg[6].data );

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 15000);
    //pCus_SetFPS(handle, 24000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    return SUCCESS;
}

static int pCus_init_HDR(ms_cus_sensor *handle)
{
    int i,cnt=0;
    ps5270_params *params = (ps5270_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR);i++)
    {
        if(Sensor_init_table_HDR[i].reg==0xff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR[i].reg,Sensor_init_table_HDR[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

#if 0
        for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
        {
            if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            {
                //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                return FAIL;
            }
        }
#endif

    params->tVts_reg[1].data = (u16)((params->expo.vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((params->expo.vts >> 0) & 0x00ff);
    params->sghd_flag = 1;  // 1x
    params->sns_const1 = ( (expo_reg[3].data<<8)|expo_reg[4].data ) + ( (expo_reg[5].data<<8)|expo_reg[6].data );

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 15000);
    //pCus_SetFPS(handle, 24000);
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
    if (res_idx >= num_res) {
        return FAIL;
    }


    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            break;
        case 1:
             handle->video_res_supported.ulcur_res = 1;
             handle->pCus_sensor_init = pCus_init_HDR;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    u16 HFlip,VFlip;

    SensorReg_Write(0xef,0x01);//page 1
    SensorReg_Read(0x1b, &HFlip);
    SensorReg_Read(0x1d, &VFlip);

    if(((HFlip&0x80)==0) &&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M0F0;
    else if(((HFlip&0x80)!=0) &&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M1F0;
    else if(((HFlip&0x80)==0) &&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M0F1;
    else if(((HFlip&0x80)!=0) &&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M1F1;

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
	//ISensorIfAPI *sensor_if = handle->sensor_if_api;
    ps5270_params *params = (ps5270_params *)handle->private_data;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;
    I2C_ARRAY mirrir_flip_table_tmp;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    //return SUCCESS;
    //sensor_if->SetSkipFrame(2); //skip 2 frame to avoid bad frame after mirror/flip

    switch(orit)
    {
        case CUS_ORIT_M0F0:

            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Read(mirr_flip_table[i].reg,&(mirrir_flip_table_tmp.data));
                SENSOR_EMSG("origin:%x\n",mirrir_flip_table_tmp.data);
                mirrir_flip_table_tmp.data = (mirrir_flip_table_tmp.data & 0x7f);
                //SensorReg_Write(mirr_flip_table[i].reg, (mirr_flip_table[i].data | mirrir_flip_table_tmp.data));
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = (mirr_flip_table[i].data | mirrir_flip_table_tmp.data);
                SENSOR_EMSG("after:%x\n",Current_Mirror_Flip_Tbl[j].data);
            }
         //  handle->bayer_id=  CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                SensorReg_Read(mirr_flip_table[i].reg,&(mirrir_flip_table_tmp.data));
                SENSOR_EMSG("origin:%x\n",mirrir_flip_table_tmp.data);
                mirrir_flip_table_tmp.data = (mirrir_flip_table_tmp.data & 0x7f);
                //SensorReg_Write(mirr_flip_table[i].reg, (mirr_flip_table[i].data | mirrir_flip_table_tmp.data));
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data =  (mirr_flip_table[i].data | mirrir_flip_table_tmp.data);
                SENSOR_EMSG("after:%x\n",Current_Mirror_Flip_Tbl[j].data);
            }
    //  handle->bayer_id= CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Read(mirr_flip_table[i].reg,&(mirrir_flip_table_tmp.data));
                SENSOR_EMSG("origin:%x\n",mirrir_flip_table_tmp.data);
                mirrir_flip_table_tmp.data = (mirrir_flip_table_tmp.data & 0x7f);
                //SensorReg_Write(mirr_flip_table[i].reg, (mirr_flip_table[i].data | mirrir_flip_table_tmp.data));
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data =  (mirr_flip_table[i].data | mirrir_flip_table_tmp.data);
                SENSOR_EMSG("after:%x\n",Current_Mirror_Flip_Tbl[j].data);
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                SensorReg_Read(mirr_flip_table[i].reg,&(mirrir_flip_table_tmp.data));
                SENSOR_EMSG("origin:%x\n",mirrir_flip_table_tmp.data);
                mirrir_flip_table_tmp.data = (mirrir_flip_table_tmp.data & 0x7f);
                //SensorReg_Write(mirr_flip_table[i].reg, (mirr_flip_table[i].data | mirrir_flip_table_tmp.data));
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data =  (mirr_flip_table[i].data | mirrir_flip_table_tmp.data);
                SENSOR_EMSG("after:%x\n",Current_Mirror_Flip_Tbl[j].data);
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Read(mirr_flip_table[i].reg,&(mirrir_flip_table_tmp.data));
                SENSOR_EMSG("origin:%x\n",mirrir_flip_table_tmp.data);
                mirrir_flip_table_tmp.data = (mirrir_flip_table_tmp.data & 0x7f);
                //SensorReg_Write(mirr_flip_table[i].reg, (mirr_flip_table[i].data | mirrir_flip_table_tmp.data));
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data =  (mirr_flip_table[i].data | mirrir_flip_table_tmp.data);
                SENSOR_EMSG("after:%x\n",Current_Mirror_Flip_Tbl[j].data);
            }
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
    }
    //SensorReg_Write(0xef,0x01);
    //SensorReg_Write(0x09,1);

    params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ps5270_params *params = (ps5270_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    ps5270_params *params = (ps5270_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);

    if(fps>=5 && fps <= 24){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*25+(fps>>1))/fps)-1;
        params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true;
        return SUCCESS;
    }else if(fps>=5000 && fps <= 24000){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*25010+(fps>>1))/fps)-1;
        params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true;
        return SUCCESS;
    }else{
      //params->expo.vts=vts_25fps;
      //params->expo.fps=25;
      //SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
      return FAIL;
    }


    //params->dirty = true;
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

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    ps5270_params *params = (ps5270_params *)handle->private_data;
    int seg_length=ARRAY_SIZE(mirr_flip_table)/4;
    int i=0;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        {
            if(params->orient_dirty)
            {
                for(i=0;i<seg_length;i++)
                {
                    SensorReg_Write(Current_Mirror_Flip_Tbl[i].reg, Current_Mirror_Flip_Tbl[i].data);
                }
                SensorReg_Write(0xef,0x01);
                SensorReg_Write(0x09,1); //trigger update
                params->orient_dirty =false;
            }


            /*Update NE at frame blanking*/
            if(params->ne_dirty)
            {
                SensorReg_Write(params->tExpo_reg[0].reg, params->tExpo_reg[0].data);
                for(i=3;i<ARRAY_SIZE(expo_reg);++i)    //update Ne only
                {
                    if(params->tExpo_reg_prev[i].data != params->tExpo_reg[i].data)
                    {
                        SensorReg_Write(params->tExpo_reg[i].reg, params->tExpo_reg[i].data);
                        params->tExpo_reg_prev[i].data = params->tExpo_reg[i].data;
                    }
                }
                //NE has no frame sync
                //SensorReg_Write(0x09,1); //trigger update
                params->ne_dirty = false;
            }
        }
        break;

        case CUS_FRAME_ACTIVE:
        {
#if 0
            if(params->orient_dirty)
            {
                if(params->first_use == false)
                {
                    for(i=0;i<seg_length;i++)
                    {
                        SensorReg_Write(Current_Mirror_Flip_Tbl[i].reg, Current_Mirror_Flip_Tbl[i].data);
                    }
                    params->orient_dirty =false;
                }
                else
                    params->first_use = false;
            }
#endif
            if(params->dirty)
            {
                /*Write I2C data if new setting are different to current HW setting*/
                SensorReg_Write(params->tExpo_reg[0].reg, params->tExpo_reg[0].data);
                //for(i=1;i<ARRAY_SIZE(expo_reg);++i)
                for(i=1;i<3;++i)    //update Ny only
                {
                    if(params->tExpo_reg_prev[i].data != params->tExpo_reg[i].data)
                    {
                        SensorReg_Write(params->tExpo_reg[i].reg, params->tExpo_reg[i].data);
                        params->tExpo_reg_prev[i].data = params->tExpo_reg[i].data;
                        params->ne_dirty = true;
                    }
                }
                //SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));

                SensorRegArrayW((I2C_ARRAY*)&params->tGain_reg[1], ARRAY_SIZE(gain_reg)-1);
                SensorRegArrayW((I2C_ARRAY*)&params->tVts_reg[1], ARRAY_SIZE(vts_reg)-1);
                //SensorReg_Write(0xef,0x01);
                SensorReg_Write(0x09,1); //trigger update
                params->dirty = false;

                memcpy(params->tExpo_reg_prev, params->tExpo_reg, sizeof(expo_reg));
            }
        }
        break;

        default:
        break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    ps5270_params *params = (ps5270_params *)handle->private_data;
    u32 lines = 0;

    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xff)<<0;

    lines=params->expo.vts-lines;

    *us = lines*Preview_line_period/1000;
    //printf("====================================================\n");
    //printf("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);
    //printf("====================================================\n");

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

/*05/15/2019 from primesensor james.yang*/
#define NE_PATCH_LB	(30)
#define NE_PATCH_UB	(300)
#define NE_PATCH_SCALE	(40)
/*****/
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
  /*05/15/2019 from primesensor james.yang*/
  u32 ny, ne, ne_patch, line_fix;
  u64 lines = 0;
  u32 vts = 0;
  /******/
  ps5270_params *params = (ps5270_params *)handle->private_data;


  lines=(1000*us)/Preview_line_period;

  if (lines > params->expo.vts-2) {
      vts = lines +3;
  }
  else
    vts=params->expo.vts;

  SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
              us,
              lines,
              params->expo.vts
              );

  //lines=vts+1-lines;

  // g_total_lines = vts, lines = exposure lines
  //g_lines = ((g_total_lines - 4) > lines)?((3 < lines)?lines:3):(g_total_lines - 4);  // 4 <= Ny <= LPF-3
  //ny = g_total_lines - g_lines;

  /*patch 05/15/2019 james.yang primesensor*/
  line_fix = ((vts - 4) > lines)?((3 < lines)?lines:3):(vts - 4);	// 4 <= Ny <= LPF-3
  ny = vts - line_fix;

	// ne_patch update.
  ne_patch = NE_PATCH_LB + ((ny*NE_PATCH_SCALE)>>8);
  if (ne_patch > NE_PATCH_UB)
	  ne_patch = NE_PATCH_UB;

  //(sensor_i2c_read(0xE)<<8|sensor_i2c_read(0xF)) + (sensor_i2c_read(0x5F)<<8|sensor_i2c_read(0x60));
  //sns_const1 = ( (expo_reg[3].data<<8)|expo_reg[4].data ) + ( (expo_reg[5].data<<8)|expo_reg[6].data );
  ne = params->sns_const1 - ne_patch;
  /*******/

  //expo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
  //expo_reg[2].data =(u16)( (lines>>0) & 0x00ff);
  params->tExpo_reg[1].data =(u16)( (ny>>8) & 0x00ff);
  params->tExpo_reg[2].data =(u16)( (ny>>0) & 0x00ff);
  params->tExpo_reg[3].data =(u16)( (ne>>8) & 0x00ff);
  params->tExpo_reg[4].data =(u16)( (ne>>0) & 0x00ff);
  params->tExpo_reg[5].data =(u16)( (ne_patch>>8) & 0x00ff);
  params->tExpo_reg[6].data =(u16)( (ne_patch>>0) & 0x00ff);

  params->tVts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
  params->tVts_reg[2].data = (u16)((vts >> 0) & 0x00ff);

  //SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
  //SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
  //SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
  params->dirty = true;
  return SUCCESS;
}


// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
  ps5270_params *params = (ps5270_params *)handle->private_data;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);
   // printf("set gain/reg=%d/0x%x\n", gain,params->tGain_reg[1].data);

   return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

  ps5270_params *params = (ps5270_params *)handle->private_data;
  u32 i;
  u64 gain_double,total_gain_double;

  params->expo.final_gain = gain;
  if(gain<handle->sat_mingain)
      gain=handle->sat_mingain;
  else if(gain>=SENSOR_MAX_GAIN*1024)
      gain=SENSOR_MAX_GAIN*1024;

   gain_double=(u64)gain;
   total_gain_double=(gain_double)/1024;

  for(i=0;i<ARRAY_SIZE(gain_table);i++)
  {
      if(total_gain_double < gain_table[i].total_gain)    break;
  }
  params->tGain_reg[1].data = (params->tGain_reg[2].data)?i:(i-64);
  SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);
  return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
  ps5270_params *params = (ps5270_params *)handle->private_data;
  u32 i;
  u32 gain_idx;
  //u8 sens_ori;
  //static u8 sghd_flag = 0x0;
  u64 gain_double,total_gain_double;
  CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
  //u8 sens = 1;

  params->expo.final_gain = gain;
  if(gain<handle->sat_mingain)
      gain=handle->sat_mingain;
  else if(gain>=SENSOR_MAX_GAIN*1024)
      gain=SENSOR_MAX_GAIN*1024;

  Sensor_Gain_Linearity = gain_gap_compensate;

  for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){
      //LOGD("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

      if (Sensor_Gain_Linearity[i].gain == 0)
          break;
      if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
            gain=Sensor_Gain_Linearity[i].gain;
            break;
      }
  }

  gain_double=(u64)gain;
  total_gain_double=(gain_double)*10000/1024;
  total_gain_double = (total_gain_double > SENSOR_MAXGAIN*10000) ? SENSOR_MAXGAIN*10000:total_gain_double; //limit max gain at 128X

  for(gain_idx=6; gain_idx<ARRAY_SIZE(gain_table); gain_idx++)
  {
      if(total_gain_double <= gain_table[gain_idx].total_gain)    break;
  }

  if (gain_idx == ARRAY_SIZE(gain_table)) {
    return FAIL;
  }

#define AG_HS_NODE  (6*10000)    // 6.0x (U.9)
#define AG_LS_NODE  (5*10000)    // 5.0x (U.9)

  if(gain_table[gain_idx].total_gain >= AG_HS_NODE)
  {
      params->sghd_flag = 0;    //HS mode , 4X
  }
  else if(gain_table[gain_idx].total_gain <= AG_LS_NODE)
  {
      params->sghd_flag = 1;    //LS mode , 1x
  }

  if (params->sghd_flag == 0) gain_idx -= 64; //HS mode, gain_table[gain_idx].total_gain /= 4

  params->tGain_reg[1].data = gain_idx&0xFF;
  params->tGain_reg[2].data = params->sghd_flag;

  SENSOR_DMSG("[%s] set gain/regHS/regIDX=%d/0x%x/0x%x\n", __FUNCTION__, gain, params->tGain_reg[2].data, params->tGain_reg[1].data);

  params->dirty = true;
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

static int ps5270_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 4;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
  u32 i, j;

  for(i=0,j=0;i< num ;i++,j+=2){
      gain_gap_compensate[i].gain=pArray[i].gain;
      gain_gap_compensate[i].offset=pArray[i].offset;
  }

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
    ms_cus_sensor *handle = drv_handle;
    ps5270_params *params;
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
    params = (ps5270_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memset(params->tExpo_reg_prev, 0, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ps5270_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth= 0;
    handle->video_res_supported.res[0].nOutputHeight= 0;
    strcpy(handle->video_res_supported.res[0].strResDesc, "1536x1536@25fps");


    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 0;
    handle->video_res_supported.res[1].nOutputHeight= 0;
    strcpy(handle->video_res_supported.res[1].strResDesc, "1536x1536HDR@25fps");

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
    handle->pCus_sensor_init        = pCus_init    ;

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
    handle->pCus_sensor_SetPatternMode = ps5270_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = ps5270_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 25;
    params->dirty = false;
    params->orient_dirty = false;
    params->first_use = true;
    params->ne_dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  PS5270,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ps5270_params
                         );

