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
#include <PS5250_MIPI_init_table.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(PS5250);

//#define _ENABLE_FULL_RES_

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define _DEBUG_
//c11 extern int usleep(u32 usec);
//int usleep(u32 usec);

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
//                                                                                                    ���?//
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     32                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
//#define Preview_line_period 28947//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
#define Preview_line_period 29629 //Line per frame = Lpf+1 , line period = (1/30)/1125

//#define Line_per_second     32727
#define vts_30fps  1124//1266//1150//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 2200                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1920                    //resolution Width when preview
#define Preview_HEIGHT      1080                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     4                      //CROP_START_X
#define Preview_CROP_START_Y     4                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x90                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;
#define ENABLE_ver 1
#define SGHD_LS_FOR_TBL (164282)        // 65536*0.068*(32*6)^2/1000
#define SGHD_HS_FOR_TBL (362388)    // 65536*0.15*(32*6)^2/1000
unsigned int u32Temp_Manual = 0, u32GainReport = 1024, u32LRUB = 3; // Davis 20181029
u16 gu16DgainUB = 0, gu16DgainLB = 0;
u16 gu16DgainUBH = 0, gu16DgainUBL = 0;
u16 gu16DgainLBH = 0, gu16DgainLBL = 0, u16GainMin = 0;
//static int  drv_Fnumber = 22;
static volatile long long framecount=0;
static volatile int FirstTime=1;
static volatile int fps_delay=5;
static volatile int ver=1;
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
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
        u32 sclk;
        u32 hts;
        u32 vts;    //target vts
        u32 ho;
        u32 xinc;
        u32 line_freq;
        u32 us_per_line;
        u32 final_us;
        u32 final_gain;
        u32 back_pv_us;
        u32 cur_fps;
        u32 target_fps;
        u32 line;
        u16 sens;
        u32 cur_vts;    //actual vts
        u32 new_fps;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
    u32 cur_shutter;
} ps5250_params;
// set sensor ID address and data,

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static I2C_ARRAY Sensor_id_table[] =
{
{0x00, 0x52},      // {address of ID, ID },
{0x01, 0x50},
};

#if 0
static I2C_ARRAY Sensor_init_table[] =
{
#if 0//defined(__KERNEL__)
        //for MCLK 27MHz
        {0xEF, 0x01},
        {0x05, 0x03},
        {0xEF, 0x00}, //bank0
        {0x06, 0x02},
        {0x0B, 0x00},
        {0x0C, 0xA0},
        {0x10, 0x80}, //Cmd_BYTECLK_InvSel=1
        {0x11, 0x00}, //??Cmd_GatedAllClk???(???keep 1)
        {0x12, 0x80},
        {0x13, 0x00},
        {0x14, 0xBF},
        {0x15, 0x07},
        {0x16, 0xBF},
        {0x17, 0xBF},
        {0x18, 0xBF},
        {0x19, 0x64},
        {0x1B, 0x64},
        {0x1C, 0x64},
        {0x1D, 0x64},
        {0x1E, 0x64},
        {0x1F, 0x64},
        {0x20, 0x64},
        {0x21, 0x00},
        {0x23, 0x00},
        {0x24, 0x00},
        {0x25, 0x00},
        {0x26, 0x00},
        {0x27, 0x00},
        {0x28, 0x00},
        {0x29, 0x64},
        {0x2B, 0x64},
        {0x2C, 0x64},
        {0x2D, 0x64},
        {0x2E, 0x64},
        {0x2F, 0x64},
        {0x30, 0x64},
        {0x31, 0x0F},
        {0x32, 0x00},
        {0x33, 0x64},
        {0x34, 0x64},
        {0x54, 0xC3},
        {0x55, 0x21},
        {0x56, 0x1D},
        {0x57, 0x2D},
        {0x58, 0x3D},
        {0x5A, 0x00},
        {0x5B, 0x00},
        {0x5C, 0x00},
        {0x5E, 0x01},
        {0x5F, 0x90},
        {0x60, 0x9A},
        {0x61, 0xFF},
        {0x62, 0x07},
        {0x63, 0x46},
        {0x64, 0x05},
        {0x65, 0x82},
        {0x66, 0x05},
        {0x67, 0xBE},
        {0x68, 0x05},
        {0x69, 0x05},
        {0x6A, 0x36},
        {0x6B, 0x00},
        {0x6C, 0x00},
        {0x6D, 0x00},
        {0x6E, 0x00},
        {0x6F, 0x20},
        {0x70, 0x00},
        {0x71, 0x1C},
        {0x72, 0x00},
        {0x73, 0x96},
        {0x74, 0x96},
        {0x75, 0x00},
        {0x76, 0xFD}, //default=2F 20170608 JAMES
        {0x77, 0xFD}, //default=2F 20170608 JAMES
        {0x78, 0x77}, //default=88 20170608 JAMES
        {0x79, 0x34},
        {0x7A, 0x00},
        {0x7B, 0x03},
        {0x7C, 0x96},
        {0x7D, 0xF8}, //default=2A 20170608 JAMES
        {0x7E, 0x00},
        {0x7F, 0xA0},
        {0x80, 0x0E},
        {0x81, 0x80}, //default=62 20170911 JAMES
        {0x81, 0x62}, //default=26 20170608 JAMES
        {0x82, 0x71}, //default=81 20170608 JAMES
        {0x83, 0x01},
        {0x84, 0x00},
        {0x85, 0x96},
        {0x86, 0x00},
        {0x87, 0xC8},
        {0x88, 0x09},
        {0x89, 0x01},
        {0x8B, 0x00},
        {0x8C, 0x00},
        {0x8D, 0x00},
        {0x8E, 0x00},
        {0x8F, 0x00},
        {0x90, 0x00},
        {0x91, 0x00},
        {0x92, 0x11},
        {0x93, 0x00},
        {0x94, 0x00},
        {0x95, 0x00},
        {0x96, 0x00},
        {0x97, 0x00},
        {0x99, 0x00},
        {0x9A, 0x00},
        {0x9B, 0x09},
        {0x9C, 0x00},
        {0x9D, 0x00},
        {0x9E, 0x08}, //default=0A 20170711 JAMES
        {0x9F, 0x03},
        {0xA0, 0xE0},
        {0xA1, 0x40},
        {0xA2, 0x30}, //default=10 20170608 JAMES
        {0xA3, 0x03}, //default=01 20170608 JAMES
        {0xA4, 0xFF},
        {0xA6, 0x00},
        {0xA8, 0x00},
        {0xA9, 0x11},
        {0xAA, 0x65},
        {0xAB, 0x65},
        {0xAD, 0x00},
        {0xAE, 0x00},
        {0xAF, 0x00},
        {0xB0, 0x00},
        {0xB1, 0x00},
        {0xBE, 0x05},
        {0xBF, 0x00},
        {0xC0, 0x10},
        {0xC1, 0x01},
        {0xC7, 0x10},
        {0xC8, 0x01},
        {0xC9, 0x00},
        {0xCA, 0x55},
        {0xCB, 0x06},
        {0xCC, 0x09},
        {0xCD, 0x00},
        {0xCE, 0xA2},
        {0xCF, 0x00},
        {0xD0, 0x02},
        {0xD1, 0x10},
        {0xD2, 0x1E},
        {0xD3, 0x19},
        {0xD4, 0x04},
        {0xD5, 0x18},
        {0xD6, 0xC8},
        {0xD9, 0x64}, //default=71 20170608 JAMES
        {0xDA, 0x12}, //default=0E 20170608 JAMES
        {0xDB, 0x84}, //default=00 20170608 JAMES
        {0xDC, 0x31}, //default=1B 20170608 JAMES
        {0xF0, 0x00},
        {0xF1, 0x00},
        {0xF2, 0x00},
        {0xF3, 0x00},
        {0xF4, 0x00},
        {0xF5, 0x40},
        {0xF6, 0x00},
        {0xF7, 0x00},
        {0xF8, 0x00},
        {0xED, 0x01},
        {0xEF, 0x01}, //bank1
        {0x02, 0xFF},
        {0x03, 0x03},
        {0x04, 0x10},
        {0x05, 0x03},
        {0x06, 0xFF},
        {0x07, 0x04},
        {0x08, 0x00},
        {0x09, 0x00},
        {0x0A, 0x04},
        {0x0B, 0xF2},
        {0x0C, 0x00},
        {0x0D, 0x02},
        {0x0E, 0x01},
        {0x0F, 0x2C},
        {0x10, 0x00},
        {0x11, 0x00},
        {0x12, 0x00},
        {0x13, 0x00},
        {0x14, 0x01},
        {0x15, 0x00},
        {0x16, 0x00},
        {0x17, 0x00},
        {0x1A, 0x00},
        {0x1B, 0x07},
        {0x1C, 0x90},
        {0x1D, 0x04},
        {0x1E, 0x46}, //default=43 20170608 JAMES
        {0x1F, 0x00},
        {0x20, 0x02},
        {0x21, 0x00},
        {0x22, 0xD4},
        {0x23, 0x10},
        {0x24, 0xA0},
        {0x25, 0x00},
        {0x26, 0x08},
        {0x27, 0x08},
        {0x28, 0xCC},
        {0x29, 0x02}, //default=00 20170608 JAMES
        {0x2A, 0x08},
        {0x2B, 0x93}, //default=C8 20170608 JAMES
        {0x2C, 0x10},
        {0x2D, 0x12},
        {0x2E, 0x78}, //default=5A 20170911 JAMES
        {0x2E, 0x5A}, //default=1E 20170608 JAMES
        {0x2F, 0x10},
        {0x30, 0x2E},
        {0x31, 0x84},
        {0x32, 0x10},
        {0x33, 0x2E},
        {0x34, 0x84},
        {0x35, 0x01},
        {0x36, 0x00},
        {0x37, 0x90},
        {0x38, 0x44},
        {0x39, 0xC2},
        {0x3A, 0xFF},
        {0x3B, 0x0A},
        {0x3C, 0x08},
        {0x3D, 0x04},
        {0x3E, 0x20},
        {0x3F, 0x22},
        {0x40, 0xFF},
        {0x41, 0x0F}, //default=1F 20170608 JAMES
        {0x42, 0xC8}, //default=68 20170608 JAMES
        {0x43, 0xFF},
        {0x44, 0x04},
        {0x47, 0x00},
        {0x48, 0x73},
        {0x49, 0x0A},
        {0x4A, 0x21}, //default=22 20170608 JAMES
        {0x4B, 0x1E},
        {0x4C, 0xBE}, //default=5F 20170608 JAMES
        {0x4D, 0x08},
        {0x4E, 0x41},
        {0x4F, 0x01},
        {0x50, 0x08},
        {0x51, 0x03},
        {0x52, 0xE8}, //default=B8 20170608 JAMES
        {0x53, 0x08},
        {0x54, 0x00},
        {0x55, 0x00},
        {0x56, 0x0A}, //default=64 20170608 JAMES
        {0x57, 0x01},
        {0x58, 0x2C},
        {0x59, 0x00},
        {0x5A, 0xD6},
        {0x5B, 0x00},
        {0x5C, 0xC8},
        {0x5D, 0x01},
        {0x5E, 0x9E},
        {0x5F, 0x00},
        {0x60, 0x78}, //default=5A 20170911 JAMES
        {0x60, 0x5A}, //default=1E 20170608 JAMES
        {0x61, 0x09},
        {0x62, 0x4A},
        {0x63, 0x18},
        {0x64, 0x02},
        {0x65, 0x01},
        {0x66, 0x27},
        {0x69, 0x07},
        {0x6A, 0x02},
        {0x6B, 0x9E},
        {0x6C, 0x10},
        {0x6D, 0xC8},
        {0x6E, 0x04},
        {0x6F, 0x06},
        {0x70, 0x00},
        {0x71, 0xC8},
        {0x72, 0x09},
        {0x73, 0x56},
        {0x74, 0x00},
        {0x75, 0x00},
        {0x76, 0x00},
        {0x77, 0x00},
        {0x78, 0x00},
        {0x79, 0x0F},
        {0x7A, 0x00},
        {0x7B, 0x00},
        {0x7C, 0x38}, //default=0E 20170608 JAMES
        {0x7D, 0x00},
        {0x7E, 0x04},
        {0x7F, 0x00},
        {0x80, 0x00},
        {0x83, 0x00},
        {0x87, 0x00},
        {0x88, 0x0B},
        {0x89, 0x00},
        {0x8A, 0x04},
        {0x8B, 0x46},
        {0x8C, 0x00},
        {0x8D, 0x00},
        {0x8E, 0x8A},
        {0x8F, 0x00},
        {0x90, 0x21},
        {0x91, 0xBD},
        {0x92, 0x80},
        {0x93, 0x00},
        {0x94, 0xFF},
        {0x95, 0x00},
        {0x96, 0x00},
        {0x97, 0x01},
        {0x98, 0x02},
        {0x99, 0x02},
        {0x9A, 0x50},
        {0x9B, 0x03},
        {0x9C, 0x54},
        {0x9D, 0x07},
        {0x9E, 0x30},
        {0x9F, 0x00},
        {0xA0, 0x00},
        {0xA1, 0x00},
        {0xA2, 0x00},
        {0xA3, 0x00},
        {0xA4, 0x0D},
        {0xA5, 0x04},
        {0xA6, 0x40},
        {0xA7, 0x00},
        {0xA8, 0x00},
        {0xA9, 0x07},
        {0xAA, 0x88},
        {0xAB, 0x01},
        {0xAD, 0x00},
        {0xAE, 0x00},
        {0xAF, 0x00},
        {0xB0, 0x50},
        {0xB1, 0x00},
        {0xB2, 0x00},
        {0xB3, 0x00},
        {0xB4, 0x50},
        {0xB5, 0x07},
        {0xB6, 0x80},
        {0xB7, 0x82},
        {0xB8, 0x0A},
        {0xB9, 0x9E},
        {0xBA, 0x48},
        {0xBB, 0xCC},
        {0xBC, 0x48},
        {0xBD, 0xC2},
        {0xBE, 0x48},
        {0xBF, 0xC2},
        {0xC0, 0x54},
        {0xC1, 0xCC},
        {0xC2, 0x54},
        {0xC3, 0x27},
        {0xC4, 0x12},
        {0xC5, 0x02},
        {0xC6, 0x00},
        {0xC7, 0x00},
        {0xC8, 0x50},
        {0xC9, 0xE5},
        {0xCA, 0x00},
        {0xCB, 0x00},
        {0xCC, 0x3C},
        {0xCE, 0xF0},
        {0xCF, 0x80},
        {0xD0, 0xC2},
        {0xD1, 0x04}, //default=44 20170608 JAMES
        {0xD2, 0x54},
        {0xD3, 0x17}, //default=14 20170608 JAMES
        {0xD4, 0x00},
        {0xD5, 0x01},
        {0xD6, 0x00},
        {0xD7, 0x06},
        {0xD8, 0x5E}, //default=4E 20170608 JAMES
        {0xD9, 0x66}, //default=NC 20170608 JAMES
        {0xDA, 0x70},
        {0xDB, 0x70},
        {0xDC, 0x10},
        {0xDD, 0x72}, //default=62 20170911 JAMES
        {0xDD, 0x62}, //default=52 20170608 JAMES
        {0xDE, 0x43},
        {0xDF, 0x40},
        {0xE0, 0x42},
        {0xE1, 0x11},
        {0xE2, 0x6D}, ////default=1D 20170911 JAMES
        {0xE2, 0x1D}, //default=4D 20170711 JAMES
        {0xE3, 0x21},
        {0xE4, 0x60},
        {0xE6, 0x00},
        {0xE7, 0x00},
        {0xEA, 0x7A},
        {0xF5, 0x01},
        {0xF6, 0xC8},
        {0xF7, 0x02},
        {0xF0, 0x03},
        {0xF4, 0x06},
        {0xF2, 0x11},
        {0xF1, 0x0A},
        {0xF5, 0x11},
        {0xF1, 0x0A},
        {0xF2, 0x11},
        {0xF4, 0x06},
        {0xF5, 0x11},
        {0xF6, 0xC8},
        {0xF7, 0x02},
        {0xF8, 0x00}, //default=40 20170911 JAMES
        {0xF8, 0x40}, //default=00 20170608 JAMES
        {0xF9, 0x15},
        {0xFA, 0x3D},
        {0xFB, 0x02},
        {0xFC, 0x28},
        {0xFD, 0x32},
        {0x09, 0x01},
        {0xEF, 0x02}, //bank2
        {0x10, 0x00},
        {0x20, 0x01},
        {0x21, 0x18},
        {0x22, 0x0C},
        {0x23, 0x08},
        {0x24, 0x05},
        {0x25, 0x03},
        {0x26, 0x02},
        {0x27, 0x02},
        {0x28, 0x07},
        {0x29, 0x08},
        {0x2E, 0x00},
        {0x30, 0xBF},
        {0x31, 0x06},
        {0x32, 0x07},
        {0x33, 0x81}, //default=80 20170608 JAMES
        {0x34, 0x00},
        {0x35, 0x00},
        {0x36, 0x01},
        {0x37, 0x00},
        {0x38, 0x00},
        {0x39, 0x00},
        {0x3A, 0xCE},
        {0x3B, 0x17},
        {0x3C, 0x64},
        {0x3D, 0x04},
        {0x3E, 0x00},
        {0x3F, 0x0A},
        {0x40, 0x0A},
        {0x41, 0x09},
        {0x42, 0x0A},
        {0x43, 0x09},
        {0x45, 0x00},
        {0x46, 0x00},
        {0x47, 0x00},
        {0x48, 0x00},
        {0x49, 0x00},
        {0x4A, 0x00},
        {0x4B, 0x00},
        {0x4C, 0x00},
        {0x4D, 0x00},
        {0x88, 0x01},
        {0xA0, 0x00},
        {0xA1, 0x00},
        {0xA2, 0x00},
        {0xA3, 0x00},
        {0xA4, 0x00},
        {0xA5, 0x00},
        {0xA6, 0x00},
        {0xA7, 0x00},
        {0xA9, 0x00},
        {0xAA, 0x00},
        {0xAB, 0x00},
        {0xB2, 0x01},
        {0xB3, 0x00},
        {0xB4, 0x03},
        {0xB5, 0x01},
        {0xB6, 0x04},
        {0xB7, 0x01},
        {0xB8, 0x04},
        {0xB9, 0x00},
        {0xBA, 0x00},
        {0xBB, 0x00},
        {0xBC, 0x00},
        {0xBD, 0x00},
        {0xBE, 0x00},
        {0xCB, 0xBD},
        {0xCC, 0x00},
        {0xCD, 0x00},
        {0xCE, 0x00},
        {0xCF, 0x00},
        {0xD0, 0x00},
        {0xD1, 0x00},
        {0xD2, 0x00},
        {0xD3, 0x00},
        {0xD4, 0x01},
        {0xD5, 0x00},
        {0xD6, 0x00},
        {0xD7, 0x00},
        {0xD8, 0x00},
        {0xD9, 0x00},
        {0xDA, 0x00},
        {0xDB, 0x00},
        {0xDC, 0x00},
        {0xDD, 0xF0},
        {0xDE, 0x04},
        {0xDF, 0x97},
        {0xE0, 0x50},
        {0xE1, 0x50},
        {0xE2, 0x14},
        {0xE3, 0x1B},
        {0xE4, 0x3F},
        {0xE5, 0xFF},
        {0xE6, 0x00},
        {0xF0, 0x00},
        {0xF1, 0x00},
        {0xF2, 0x00},
        {0xF3, 0x00},
        {0xF6, 0x00},
        {0xF7, 0x00},
        {0xFD, 0x18},
        {0xFE, 0x9E},
        {0xED, 0x01},
        {0xEF, 0x05}, //bank5
        {0x03, 0x10},
        {0x04, 0xE0},
        {0x05, 0x01},
        {0x06, 0x04},
        {0x07, 0x80},
        {0x08, 0x02},
        {0x09, 0x09},
        {0x0A, 0x05},
        {0x0B, 0x06},
        {0x0C, 0x04},
        {0x0D, 0xA1},
        {0x0E, 0x00},
        {0x0F, 0x00},
        {0x10, 0x02},
        {0x11, 0x01},
        {0x12, 0x00},
        {0x13, 0x00},
        {0x14, 0xB8},
        {0x15, 0x07},
        {0x16, 0x06},
        {0x17, 0x03},
        {0x18, 0x01},
        {0x19, 0x04},
        {0x1A, 0x06},
        {0x1B, 0x03},
        {0x1C, 0x04},
        {0x1D, 0x08},
        {0x1E, 0x1A},
        {0x1F, 0x00},
        {0x20, 0x00},
        {0x21, 0x1E},
        {0x22, 0x1E},
        {0x23, 0x01},
        {0x24, 0x04},
        {0x27, 0x00},
        {0x28, 0x00},
        {0x2A, 0x08},
        {0x2B, 0x02},
        {0x2C, 0xA4},
        {0x2D, 0x06},
        {0x2E, 0x00},
        {0x2F, 0x05},
        {0x30, 0xE0},
        {0x31, 0x01},
        {0x32, 0x00},
        {0x33, 0x00},
        {0x34, 0x00},
        {0x35, 0x00},
        {0x36, 0x00},
        {0x37, 0x00},
        {0x38, 0x0E},
        {0x39, 0x01},
        {0x3A, 0x02},
        {0x3B, 0x00},
        {0x3C, 0x00},
        {0x3D, 0x00},
        {0x3E, 0x00},
        {0x3F, 0x00},
        {0x42, 0x00},
        {0x40, 0x16}, //24M=16 27M=19
        {0x41, 0x12},
        {0x47, 0x05},
        {0x48, 0x00},
        {0x49, 0x01},
        {0x4D, 0x02},
        {0x4F, 0x00},
        {0x54, 0x0A},
        {0x55, 0x01},
        {0x56, 0x0A},
        {0x57, 0x01},
        {0x58, 0x01},
        {0x59, 0x01},
        {0x42, 0x01}, //MIPI PLL , PARALLEL set 0
        {0x43, 0x06}, //default=04 20170711 JAMES
        {0x5B, 0x00},
        {0x5C, 0x00},
        {0x5D, 0x00},
        {0x5E, 0x07},
        {0x5F, 0x08},
        {0x60, 0x00},
        {0x61, 0x00},
        {0x62, 0x00},
        {0x63, 0x28},
        {0x64, 0x30},
        {0x65, 0x9E},
        {0x66, 0xB9},
        {0x67, 0x52},
        {0x68, 0x70},
        {0x69, 0x4E},
        {0x70, 0x00},
        {0x71, 0x00},
        {0x72, 0x00},
        {0x90, 0x04},
        {0x91, 0x01},
        {0x92, 0x00},
        {0x93, 0x00},
        {0x94, 0x04},
        {0x96, 0x00},
        {0x97, 0x01},
        {0x98, 0x01},
        {0xA0, 0x00},
        {0xA1, 0x01},
        {0xA2, 0x00},
        {0xA3, 0x01},
        {0xA4, 0x00},
        {0xA5, 0x01},
        {0xA6, 0x00},
        {0xA7, 0x00},
        {0xAA, 0x00},
        {0xAB, 0x0F},
        {0xAC, 0x08},
        {0xAD, 0x09},
        {0xAE, 0x0A},
        {0xAF, 0x0B},
        {0xB0, 0x00},
        {0xB1, 0x00},
        {0xB2, 0x01},
        {0xB3, 0x00},
        {0xB4, 0x00},
        {0xB5, 0x0A},
        {0xB6, 0x0A},
        {0xB7, 0x0A},
        {0xB8, 0x0A},
        {0xB9, 0x00},
        {0xBA, 0x00},
        {0xBB, 0x00},
        {0xBC, 0x00},
        {0xBD, 0x00},
        {0xBE, 0x00},
        {0xBF, 0x00},
        {0xC0, 0x00},
        {0xC1, 0x00},
        {0xC2, 0x00},
        {0xC3, 0x00},
        {0xC4, 0x00},
        {0xC5, 0x00},
        {0xC6, 0x00},
        {0xC7, 0x00},
        {0xC8, 0x00},
        {0xD3, 0x80},
        {0xD4, 0x00},
        {0xD5, 0x00},
        {0xD6, 0x03},
        {0xD7, 0x77},
        {0xD8, 0x00},
        {0xED, 0x01},
        {0xff, 0x02}, //DELAY 2MS
        {0xEF, 0x00},
        {0x11, 0x00}, //?Cmd_GatedAllClk
        {0xEF, 0x01},
        {0x02, 0xFB}, //RstN TG
        {0x05, 0x3B}, //Vsync delay=3
        {0x09, 0x01},
        {0xEF, 0x05},
        {0x0F, 0x01},
        {0xED, 0x01},
#else /* Fast init for RTK*/
        /*For MCLK 24MHz*/
        {0xEF ,0x01},
        {0x05 ,0x03},
        {0xEF ,0x05},//Add, 20171027
        {0x0F ,0x00},//Add, 20171027
        {0x42 ,0x00},//Add, 20171027
        {0x43 ,0x06},//Add, 20171027
        {0xED ,0x01},//Add, 20171027
        {0xEF ,0x01},//Add, 20171027
        {0xF5 ,0x01},//Add, 20171027
        {0x09 ,0x01},//Add, 20171027
        {0xEF ,0x00}, //bank0
        {0x10 ,0x80}, //Cmd_BYTECLK_InvSel=1
        {0x54 ,0xC3},
        {0x55 ,0x21},
        {0x56 ,0x1D},
        {0x57 ,0x2D},
        {0x58 ,0x3D},
        {0x5F ,0x90},
        {0x60 ,0x9A},
        {0x61 ,0xFF},
        {0x63 ,0x46},
        {0x69 ,0x05},
        {0x6A ,0x36},
        {0x73 ,0x96},
        {0x74 ,0x96},
        {0x75 ,0x00},
        {0x76 ,0xFD},
        {0x77 ,0xFD},
        {0x78 ,0x77},
        {0x7A ,0x00},
        {0x7B ,0x03},
        {0x7C ,0x96},
        {0x7D ,0xF8},
        {0x7F ,0xA0},
        {0x80 ,0x0E},
        {0x81 ,0x80},//default=62 20170911 JAMES
        {0x82 ,0x71},
        {0x83 ,0x01},
        {0x85 ,0x96},
        {0x87 ,0xC8},
        {0x9E ,0x08},
        {0xA2 ,0x30},
        {0xA3 ,0x03},
        {0xD9 ,0x64},
        {0xDA ,0x12},
        {0xDB ,0x84},
        {0xDC ,0x31},
        {0xED ,0x01},
        {0xEF ,0x01}, //bank1
        {0x04 ,0x10},
        {0x05, 0x0B},
        {0x09 ,0x00},
        {0x0A ,0x04},
        {0x0B ,0x64},
        {0x0D ,0x02},
        {0x1E ,0x46}, //default=43 20170608 JAMES
        {0x20 ,0x02},
        {0x27 ,0x08},
        {0x28 ,0xCC},
        {0x29 ,0x02}, //default=00 20170608 JAMES
        {0x2A ,0x08},
        {0x2B ,0x93}, //default=C8 20170608 JAMES
        {0x2C ,0x10},
        {0x2D ,0x12},
        {0x2E ,0x78}, //default=5A 20170911 JAMES
        {0x37 ,0x90},
        {0x38 ,0x44},
        {0x39 ,0xC2},
        {0x3A ,0xFF},
        {0x3B ,0x0A},
        {0x3E ,0x20},
        {0x3F ,0x22},
        {0x40 ,0xFF},
        {0x41 ,0x0F}, //default=1F 20170608 JAMES
        {0x42 ,0xC8}, //default=68 20170608 JAMES
        {0x4A ,0x21}, //default=22 20170608 JAMES
        {0x4B ,0x1E},
        {0x4C ,0xBE}, //default=5F 20170608 JAMES
        {0x4F ,0x01},
        {0x51 ,0x03},
        {0x52 ,0xE8}, //default=B8 20170608 JAMES
        {0x56 ,0x0A}, //default=64 20170608 JAMES
        {0x60 ,0x78}, //default=5A 20170911 JAMES
        {0x6F ,0x06},
        {0x7C ,0x38}, //default=0E 20170711 JAMES
        {0x8B ,0x46},
        {0x99 ,0x02},
        {0xA6 ,0x40},
        {0xA8 ,0x00},
        {0xAA ,0x88},
        {0xB0 ,0x50},
        {0xB4 ,0x50},
        {0xC6 ,0x00},
        {0xC7 ,0x00},
        {0xCA ,0x00},
        {0xCB ,0x00},
        {0xD0 ,0xC2},
        {0xD1 ,0x04}, //default=44 20170711 JAMES
        {0xD3 ,0x17}, //default=14 20170711 JAMES
        {0xD8 ,0x5E}, //default=4E 20170608 JAMES
        {0xD9 ,0x66}, //default=NC 20170608 JAMES
        //{0xDD ,0x72}, //default=62 20170911 JAMES
        {0xDD ,0x52},//2019/6/12 reduce light flare in corner
        {0xDE ,0x43},
        {0xE2 ,0x6D}, //default=1D 20170911 JAMES
        {0xE3 ,0x21},
        {0xEA ,0x7A},
        {0xF5 ,0x01},
        {0xF6 ,0xC8},
        {0xF7 ,0x02},
        {0xF0 ,0x03},
        {0xF4 ,0x06},
        {0xF2 ,0x11},
        //{0xF1 ,0x0A},
        //{0xF5 ,0x11},
        {0xF1 ,0x16},//2019/6/12 reduce light flare in corner
        {0xF5 ,0x10},//2019/6/12 reduce light flare in corner
        {0xF8 ,0x00},//default=40 20170911 JAMES
        {0xF9 ,0x15},
        {0x09 ,0x01},
        {0xEF ,0x02},//bank2
        {0x20 ,0x01},
        {0x21 ,0x18},
        {0x22 ,0x0C},
        {0x23 ,0x08},
        {0x24 ,0x05},
        {0x25 ,0x03},
        {0x26 ,0x02},
        {0x27 ,0x02},
        {0x28 ,0x07},
        {0x29 ,0x08},
        {0x33 ,0x81},
        {0x47 ,0x10}, //black level
        {0x49 ,0x10}, //black level
        {0x4B ,0x10}, //black level
        {0x4D ,0x10}, //black level
        {0xCB ,0xBD},
        {0xE3 ,0x1B},
        {0xFD ,0x18},
        {0xFE ,0x9E},
        {0xED ,0x01},
        {0xEF ,0x05}, //bank5
        {0x03 ,0x10},
        {0x04 ,0xE0},
        {0x05 ,0x01},
        {0x06 ,0x04},
        {0x07 ,0x80},
        {0x08 ,0x02},
        {0x09 ,0x09},
        {0x0A ,0x05},
        {0x0B ,0x06},
        {0x0C ,0x04},
        {0x0D ,0xA1},
        {0x0E ,0x00},
        {0x0F ,0x00},
        {0x10 ,0x02},
        {0x11 ,0x01},
        {0x12 ,0x00},
        {0x13 ,0x00},
        {0x14 ,0xB8},
        {0x15 ,0x07},
        {0x16 ,0x06},
        {0x17 ,0x03},
        {0x18 ,0x01},
        {0x19 ,0x04},
        {0x1A ,0x06},
        {0x1B ,0x03},
        {0x1C ,0x04},
        {0x1D ,0x08},
        {0x1E ,0x1A},
        {0x1F ,0x00},
        {0x20 ,0x00},
        {0x21 ,0x1E},
        {0x22 ,0x1E},
        {0x23 ,0x01},
        {0x24 ,0x04},
        {0x25 ,0x01},//Cmd_CSI2_Stall=1, Add, 20171027
        {0x27 ,0x00},
        {0x28 ,0x00},
        {0x2A ,0x08},
        {0x2B ,0x02},
        {0x2C ,0xA4},
        {0x2D ,0x06},
        {0x2E ,0x00},
        {0x2F ,0x05},
        {0x30 ,0xE0},
        {0x31 ,0x01},
        {0x32 ,0x00},
        {0x33 ,0x00},
        {0x34 ,0x00},
        {0x35 ,0x00},
        {0x36 ,0x00},
        {0x37 ,0x00},
        {0x38 ,0x0E},
        {0x39 ,0x01},
        {0x3A ,0x02},
        {0x3B ,0x01},//R_Cmd_Gated_MIPI_Clk=1, Modify, 20171027
        {0x3C ,0x00},
        {0x3D ,0x00},
        {0x3E ,0x00},
        {0x3F ,0x00},
        {0x42 ,0x00},
        {0x40 ,0x16}, //24M=16 27M=19
        {0x41 ,0x12},
        {0x47 ,0x05},
        {0x48 ,0x00},
        {0x49 ,0x01},
        {0x4D ,0x02},
        {0x4F ,0x00},
        {0x54 ,0x0A},
        {0x55 ,0x01},
        {0x56 ,0x0A},
        {0x57 ,0x01},
        {0x58 ,0x01},
        {0x59 ,0x01},
        {0x42 ,0x01},//MIPI PLL , PARALLEL set 0
        {0x43 ,0x06},//T_MIPI_sel[0], Bank5_67[1]=1
        {0x5B ,0x10},//R_MIPI_FrameReset_by_Vsync_En=1, Modify, 20171027
        {0x5C ,0x00},
        {0x5D ,0x00},
        {0x5E ,0x07},
        {0x5F ,0x08},
        {0x60 ,0x00},
        {0x61 ,0x00},
        {0x62 ,0x00},
        {0x63 ,0x28},
        {0x64 ,0x30},
        {0x65 ,0x9E},
        {0x66 ,0xB9},
        {0x67 ,0x52},
        {0x68 ,0x70},
        {0x69 ,0x4E},
        {0x70 ,0x00},
        {0x71 ,0x00},
        {0x72 ,0x00},
        {0x90 ,0x04},
        {0x91 ,0x01},
        {0x92 ,0x00},
        {0x93 ,0x00},
        {0x94 ,0x04},
        {0x96 ,0x00},
        {0x97 ,0x01},
        {0x98 ,0x01},
        {0xA0 ,0x00},
        {0xA1 ,0x01},
        {0xA2 ,0x00},
        {0xA3 ,0x01},
        {0xA4 ,0x00},
        {0xA5 ,0x01},
        {0xA6 ,0x00},
        {0xA7 ,0x00},
        {0xAA ,0x00},
        {0xAB ,0x0F},
        {0xAC ,0x08},
        {0xAD ,0x09},
        {0xAE ,0x0A},
        {0xAF ,0x0B},
        {0xB0 ,0x00},
        {0xB1 ,0x00},
        {0xB2 ,0x01},
        {0xB3 ,0x00},
        {0xB4 ,0x00},
        {0xB5 ,0x0A},
        {0xB6 ,0x0A},
        {0xB7 ,0x0A},
        {0xB8 ,0x0A},
        {0xB9 ,0x00},
        {0xBA ,0x00},
        {0xBB ,0x00},
        {0xBC ,0x00},
        {0xBD ,0x00},
        {0xBE ,0x00},
        {0xBF ,0x00},
        {0xC0 ,0x00},
        {0xC1 ,0x00},
        {0xC2 ,0x00},
        {0xC3 ,0x00},
        {0xC4 ,0x00},
        {0xC5 ,0x00},
        {0xC6 ,0x00},
        {0xC7 ,0x00},
        {0xC8 ,0x00},
        {0xD3 ,0x80},
        {0xD4 ,0x00},
        {0xD5 ,0x00},
        {0xD6 ,0x03},
        {0xD7 ,0x77},
        {0xD8 ,0x00},
        {0xED ,0x01},
        {0xEF ,0x00},
        {0x11 ,0x00}, //Cmd_GatedAllClk
        {0xEF ,0x05},//Add, 20171027
        {0x3B ,0x00},//R_Cmd_Gated_MIPI_Clk=0, Add, 20171027
        {0xED ,0x01},//Add, 20171027
        {0xEF ,0x01},
        {0x02 ,0x73},//Reset MIPI & ISP & TG, Modify, 20171027
        {0x09 ,0x01},
        {0xEF ,0x05},
        {0x0F ,0x01},
        {0xED ,0x01},
        {0xFFFF,  2},//delay 2ms SENSOR_DELAY_REG ,2,//Add, 20171027
        {0xEF ,0x05},//Add, 20171027
        {0x25 ,0x00},//Cmd_CSI2_Stall=0, Add, 20171027
        {0xED ,0x01},//Add, 20171027
#endif

};
#endif

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
    {0xEF, 0x01},//M0F0
    {0x1B, 0x87},//bit[7],  Hflip
    {0x1D, 0x84},//bit[7]   Vflip
    {0x90, 0x31},//bit[7:4] Cmd_ADC_Latency
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


static I2C_ARRAY mirr_flip_table[] =
{
    {0xEF, 0x01},//M0F0
    {0x1B, 0x07},//bit[7],  Hflip
    {0x1D, 0x04},//bit[7]   Vflip
    {0x90, 0x21},//bit[7:4] Cmd_ADC_Latency

    {0xEF, 0x01},//M1F0
    {0x1B, 0x87},//bit[7],  Hflip
    {0x1D, 0x04},//bit[7]   Vflip
    {0x90, 0x31},//bit[7:4] Cmd_ADC_Latency

    {0xEF, 0x01},//M0F1
    {0x1B, 0x07},//bit[7]   Hflip
    {0x1D, 0x84},//bit[7:4] Cmd_ADC_Latency
    {0x90, 0x21},//bit[7]   Vflip

    {0xEF, 0x01},//M0F1
    {0x1B, 0x87},//bit[7]   Hflip
    {0x1D, 0x84},//bit[7:4] Cmd_ADC_Latency
    {0x90, 0x31},//bit[7]   Vflip
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;



static I2C_ARRAY gain_reg[] = {
    {0xEF, 0x01},
    {0x78, 0x00},//analog gain[12:8]
    {0x79, 0x00},//analog gain[0:7]
    {0x83, 0x00},//gain ID
};

// Davis 20181101
static Gain_ARRAY gain_table[]={
        {10000  ,4096},
        {10625  ,3855},
        {11250  ,3641},
        {11875  ,3449},
        {12500  ,3277},
        {13125  ,3121},
        {13750  ,2979},
        {14375  ,2849},
        {15000  ,2731},
        {15625  ,2621},
        {16250  ,2521},
        {16875  ,2427},
        {17500  ,2341},
        {18125  ,2260},
        {18750  ,2185},
        {19375  ,2114},
        {20000  ,2048},
        {21250  ,1928},
        {22500  ,1820},
        {23750  ,1725},
        {25000  ,1638},
        {26250  ,1560},
        {27500  ,1489},
        {28750  ,1425},
        {30000  ,1365},
        {31250  ,1311},
        {32500  ,1260},
        {33750  ,1214},
        {35000  ,1170},
        {36250  ,1130},
        {37500  ,1092},
        {38750  ,1057},
        {40000  ,1024},
        {42500  ,964 },
        {45000  ,910 },
        {47500  ,862 },
        {50000  ,819 },
        {52500  ,780 },
        {55000  ,745 },
        {57500  ,712 },
        {60000  ,683 },
        {62500  ,655 },
        {65000  ,630 },
        {67500  ,607 },
        {70000  ,585 },
        {72500  ,565 },
        {75000  ,546 },
        {77500  ,529 },
        {80000  ,512 },
        {85000  ,482 },
        {90000  ,455 },
        {95000  ,431 },
        {100000 ,410 },
        {105000 ,390 },
        {110000 ,372 },
        {115000 ,356 },
        {120000 ,341 },
        {125000 ,328 },
        {130000 ,315 },
        {135000 ,303 },
        {140000 ,293 },
        {145000 ,282 },
        {150000 ,273 },
        {155000 ,264 },
        {160000 ,256 },
        {169959 ,241 },
        {180441 ,227 },
        {190512 ,215 },
        {199805 ,205 },
        {210051 ,195 },
        {220215 ,186 },
        {230112 ,178 },
        {239532 ,171 },
        {249756 ,164 },
        {259241 ,158 },
        {269474 ,152 },
        {280548 ,146 },
        {290496 ,141 },
        {298948 ,137 },
        {310303 ,132 },
        {320000 ,128 },
};

static int g_sensor_ae_min_gain = 1280;
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

static I2C_ARRAY expo_reg[] = {
    {0xEF, 0x01},
    {0x0C, 0x00},
    {0x0D, 0x02},
};

static I2C_ARRAY vts_reg[] = {
    {0xEF, 0x01},
    {0x0A, 0x04},
    {0x0B, 0xF2},

};

#if 0
static CUS_INT_TASK_ORDER def_order = {
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
#define SENSOR_NAME ps5250


#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

static int cus_camsensor_release_handle(ms_cus_sensor *handle);

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

//static u32 timeGetTimeU(void)
//{
//    CamOsTimespec_t tRes;
//    CamOsGetMonotonicTime(&tRes);
//    return (tRes.nSec * 1000000)+(tRes.nNanoSec/1000);
//}
//static u32 TStart = 0;

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    int res = 0;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //TStart = timeGetTimeU();
    /*PAD and CSI*/
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

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
        SENSOR_MSLEEP(3);     //T4 = 3ms

    //CamOsPrintf("pCus_poweron = %d us \n",timeGetTimeU()-TStart);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ps5250_params *params = (ps5250_params *)handle->private_data;

    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_UDELAY(100);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    params->dirty = false;
    params->orient_dirty = false;

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
    SENSOR_DMSG("[%s]ps5250 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ps5250_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    unsigned int i,cnt=0;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);
    //TStart = timeGetTimeU();

    for(i=0;(sizeof(Sensor_init_table)&&(i< ARRAY_SIZE(Sensor_init_table)));i++)
    {
        if(Sensor_init_table[i].reg==0xffff)
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
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }

    for(i=0;(sizeof(PatternTbl)&&(i<ARRAY_SIZE(PatternTbl)));i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    FirstTime=1;

    //pCus_SetAEGain(handle,1536); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 15000);
    //pCus_SetFPS(handle,30000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    //CamOsPrintf("pCus_init = %d us \n",timeGetTimeU()-TStart);

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
#ifdef _ENABLE_FULL_RES_
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init;
            break;
#endif
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
#if 0
    short HFlip,VFlip;

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
#endif
    *orit = handle->orient;
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    ps5250_params *params = (ps5250_params *)handle->private_data;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2; //to do (skip method)
    int i,j;

    SENSOR_DMSG("\n\n[%s] orit=%d", __FUNCTION__, orit);

    //sensor_if1->SetSkipFrame(handle,2); //skip 2 frame to avoid bad frame after mirror/flip

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = orit;
            for(i=0,j=0;i<seg_length;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
         //  handle->bayer_id=  CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            handle->orient = orit;
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
    //  handle->bayer_id= CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            handle->orient = orit;
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            handle->orient = orit;
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        default :
            handle->orient = CUS_ORIT_M0F0;
            for(i=0,j=0;i<seg_length;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
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
    ps5250_params *params = (ps5250_params *)handle->private_data;
    return (vts_30fps*30000)/params->expo.cur_vts;
}

static int _SetFps(ms_cus_sensor *handle, u32 fps)
{
    ps5250_params *params = (ps5250_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //CamOsPrintf("SetFPS %d, target=%d, cur=%d, new=%d\n", fps, params->expo.target_fps, params->expo.cur_fps, params->expo.new_fps);
    //return SUCCESS;
    /*
    if(fps>=5 && fps <= 30)
    {
        params->expo.target_fps = fps*1000;
        params->expo.vts=  (vts_30fps*30)/fps;
        vts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        pCus_SetAEUSecs(handle,params->cur_shutter);
        return SUCCESS;
    }
    else  */
    if(fps>=5000 && fps <= 30000)
    {
        //params->expo.target_fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
        vts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        pCus_SetAEUSecs(handle,params->cur_shutter);
        return SUCCESS;
    }else{
      return FAIL;
    }
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    ps5250_params *params = (ps5250_params *)handle->private_data;
    SENSOR_DMSG("[%s] Fps=%d", __FUNCTION__, fps);

    if(fps>=5 && fps <= 30)
    {
      params->expo.target_fps = fps*1000;
    }
    else if(fps>=5000 && fps <= 30000)
    {
        params->expo.target_fps = fps;
    }else
    {
      return FAIL;
    }

    if( abs((int)params->expo.target_fps-(int)params->expo.cur_fps) >= params->expo.cur_fps )
    {
        int diff = params->expo.target_fps - params->expo.cur_fps;
        if(diff>0)
            params->expo.new_fps = params->expo.cur_fps + (params->expo.cur_fps/2);
        else
            params->expo.new_fps = params->expo.cur_fps - (params->expo.cur_fps/2);

        _SetFps( handle, params->expo.new_fps);
    }
    else
    {
        params->expo.new_fps = params->expo.target_fps;
        _SetFps( handle, params->expo.target_fps);
    }

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

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status){
    ps5250_params *params = (ps5250_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //mirror flip
            if(params->orient_dirty)
            {
                /*Drop MIPI frame begin*/
                //SensorReg_Write(0xEF,5); //bank 5
                //SensorReg_Write(0x25,1); //CSI stall
                handle->sensor_if_api->SetSkipFrame(handle->snr_pad_group, params->expo.cur_fps, 1);

                SensorRegArrayW( (I2C_ARRAY*)Current_Mirror_Flip_Tbl, ARRAY_SIZE(Current_Mirror_Flip_Tbl));
                SensorReg_Write(0x09,1);

                /*Drop MIPI frame end*/
                //SENSOR_MSLEEP( (1000000/pCus_GetFPS(handle))+5 ); //delay 1 frame time + 5ms
                //SensorReg_Write(0xEF,5); //bank 5
                //SensorReg_Write(0x25,0); //CSI stall

                params->orient_dirty = false;
            }
             break;
        case CUS_FRAME_ACTIVE:
            //shutter, fps , gain
            if(params->dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)&gain_reg[1], ARRAY_SIZE(gain_reg)-1);  //reduce I2C data size
                SensorRegArrayW((I2C_ARRAY*)&vts_reg[1], ARRAY_SIZE(vts_reg)-1);
                SensorReg_Write(0x09,1);
                params->expo.cur_fps = params->expo.new_fps;
                params->dirty = false;
            }
            if( params->expo.target_fps != params->expo.cur_fps ) //smooth fps
            {
                pCus_SetFPS(handle, params->expo.target_fps);
            }
            break;
        default :
             break;
    }

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    ps5250_params *params = (ps5250_params *)handle->private_data;
    //u32 lines = 0;

    //lines |= (u32)(expo_reg[1].data&0xff)<<8;
    //lines |= (u32)(expo_reg[2].data&0xff)<<0;

    //lines=params->expo.vts-lines;

    //*us = lines*Preview_line_period/1000;
    //SENSOR_DMSG("====================================================\n");
    //SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);
    //SENSOR_DMSG("====================================================\n");

    *us = params->expo.line*Preview_line_period/1000;
    //SENSOR_DMSG("[%s] sensor expo lines/us %ld us\n", __FUNCTION__, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    ps5250_params *params = (ps5250_params *)handle->private_data;
    params->cur_shutter = us;

    lines=(1000*us)/Preview_line_period;

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
    params->expo.cur_vts = vts;
    params->expo.line = lines;
    lines=vts-lines;
    expo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
    expo_reg[2].data =(u16)( (lines>>0) & 0x00ff);


    vts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
    vts_reg[2].data = (u16)((vts >> 0) & 0x00ff);

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    ps5250_params *params = (ps5250_params *)handle->private_data;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,gain_reg[1].data);
    // SENSOR_DMSG("set gain/reg=%d/0x%x\n", gain,gain_reg[1].data);

    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {
    ps5250_params *params = (ps5250_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;

    u16 gain16=1024;

#if 1
    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;


    gain_double=(double)gain;
    total_gain_double=((gain_double)*10000)/1024;
    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            gain16=gain_table[i-1].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            gain16=gain_table[i].reg_val;
            break;
        }
    }

    gain_reg[1].data=(gain16>>8)&0x01f;
    gain_reg[2].data=gain16&0xff;//low byte,LSB
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[0].data,gain_reg[1].data);
    #endif
   // printf("set gain/reg=%d/0x%x\n", gain,gain_reg[1].data);
  //  params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    ps5250_params *params = (ps5250_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u16 gain16 = 0;
    #if 1
    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < ARRAY_SIZE(gain_gap_compensate); i++)
    {
        //LOGD("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double=(u32)gain;
    total_gain_double=((gain_double)*10000)/1024;
    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            gain16=gain_table[i-1].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            gain16=gain_table[i].reg_val;
            break;
        }
    }

    gain_reg[1].data=(gain16>>8)&0x01f;
    gain_reg[2].data=gain16&0xff;//low byte,LSB
    gain_reg[3].data=i; //set gain ID

#if 0
//for dark current
        SensorReg_Write(0xef,0x02);
        SensorReg_Read(0x1B, &Normalized_DigDacH);
        SensorReg_Read(0x1C, &Normalized_DigDacL);
        Normalized_DigDac=((Normalized_DigDacH&0x07)<<8)|(Normalized_DigDacL&0xff);
        SensorReg_Write(0xef,0x01);
        SensorReg_Read(0xE1, &T_analog_control_enh);
        SensorReg_Read(0xE2, &T_analog_control_value);
#if 0 //Original from ps5220
        if((Normalized_DigDac>0x001C)||(gain16<0x0400)){
                if((T_analog_control_enh&0x10)!=0x10){
                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value&0xf0|0x09);
                  }
                 else{
                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value&0xf0|0x0d);
                   SensorReg_Write(0xE1,T_analog_control_enh|0x10);
                }
          }
        else  if((Normalized_DigDac<0x0016)||(gain16>0x0555)){
                 if((T_analog_control_value&0x05)!=0x05){
                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value&0xf0|0x09);
                   SensorReg_Write(0xE1,T_analog_control_enh|0x10);
                  }
                 else{

                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value&0xf7);
                   SensorReg_Write(0xE1,T_analog_control_enh&0xef);
                 }
          }
#else

        if((Normalized_DigDac>0x0020)||(gain16<0x0200)){
                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value |0x08);
                   SensorReg_Write(0xE1,T_analog_control_enh|0x10);
                   //printf("#2#\n");
          }
        else  if((Normalized_DigDac<0x001C)||(gain16>0x02AB)){
                   SensorReg_Write(0xef,0x01);
                   SensorReg_Write(0xE2,T_analog_control_value&0xf7);
                   SensorReg_Write(0xE1,T_analog_control_enh&0xef);
                   //printf("#3#\n");
          }

#endif

        SensorReg_Write(0x09,1);//update flag
#endif

#endif
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[0].data,gain_reg[1].data);
   // printf("set gain/reg=%d/0x%x\n", gain,gain_reg[1].data);
    params->dirty = true;
    return SUCCESS;
}

static u32 pCus_TryAEGain(ms_cus_sensor *handle, u32 gain) {
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    ps5250_params *params = (ps5250_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u16 actual_gain = gain;

    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < ARRAY_SIZE(gain_gap_compensate); i++)
    {
        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double=(u32)gain;
    total_gain_double=((gain_double)*10000)/1024;
    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            actual_gain = (gain_table[i-1].total_gain * 1024)/10000;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            actual_gain = (gain_table[i].total_gain * 1024)/10000;
            break;
        }

    }
    return actual_gain;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000/30;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int ps5250_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min =  Preview_line_period*3;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    //SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}

static int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    ps5250_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    //handle->private_data = CamOsMemCalloc(1, sizeof(ps5250_params));
    params = (ps5250_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ps5250_MIPI");

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
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1928;
    handle->video_res_supported.res[0].nOutputHeight= 1088;
    strcpy(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");
#ifdef _ENABLE_FULL_RES_
    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = 1928;
    handle->video_res_supported.res[1].height = 1088;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 1928;
    handle->video_res_supported.res[1].nOutputHeight= 1088;
    strcpy(handle->video_res_supported.res[1].strResDesc, "1928x1088@30fps");
#endif
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
    handle->pCus_sensor_SetPatternMode = ps5250_SetPatternMode;
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
    handle->pCus_sensor_TryAEGain       = pCus_TryAEGain;

    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = ps5250_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.cur_fps = 30000; //fps x 1000
    params->expo.target_fps = 30000;
    params->expo.new_fps = 30000;
    params->expo.line = 100;
    params->expo.sens = 1;
    params->dirty = false;
    params->orient_dirty = false;
    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_OFF);
    //sensor_if->SetCSI_Clk(handle,CUS_CSI_CLK_DISABLE);
    if (handle && handle->private_data) {
        SENSOR_DMSG("[%s] release handle, handle %x, private data %x",
                __FUNCTION__,
                (int)handle,
                (int)handle->private_data);
        //CamOsMemRelease(handle->private_data);
        handle->private_data = NULL;
    }
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  PS5250,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ps5250_params
                         );
