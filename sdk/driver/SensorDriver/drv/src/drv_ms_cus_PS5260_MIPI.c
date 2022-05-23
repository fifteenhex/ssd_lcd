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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(PS5260);

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
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     80                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
#define Preview_line_period 29630//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define Line_per_second     32727
#define vts_30fps  1125//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 1080                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1920                    //resolution Width when preview
#define Preview_HEIGHT      1080                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x90                   //I2C slave address
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
#define ENABLE_ver 1
#define SGHD_LS_FOR_TBL (164282)        // 65536*0.068*(32*6)^2/1000
#define SGHD_HS_FOR_TBL (362388)    // 65536*0.15*(32*6)^2/1000
unsigned int u32Temp_Manual = 0, u32GainReport = 1024, u32LRUB = 3; // Davis 20181029
u16 gu16DgainUB = 0, gu16DgainLB = 0;
u16 gu16DgainUBH = 0, gu16DgainUBL = 0;
u16 gu16DgainLBH = 0, gu16DgainLBL = 0, u16GainMin = 4;
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
        u16 sens;
    } expo;
    I2C_ARRAY tVts_reg[4];
    I2C_ARRAY tGain_reg[5];
    I2C_ARRAY tExpo_reg[8];
#if ENABLE_ver
    I2C_ARRAY tNr_reg[6];
#endif
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
    const I2C_ARRAY *pInitTable;
    int nInitTableSize;
    int nCompanding; //HDR companding mode.
} ps5260_params;
// set sensor ID address and data,

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

const I2C_ARRAY Sensor_id_table[] =
{
{0x00, 0x52},      // {address of ID, ID },
{0x01, 0x60},
};

const I2C_ARRAY Sensor_init_table_0[] =
{
    // PNS310_MCLK27MHz_Imgsyn_1x8x_Synout_HQ_LCG_20180531_RAW14_560Mbps_A06a=>raw12
    //Linear 24MCLK
    {0xEF, 0x05},
    {0x0F, 0x00},
    {0x44, 0x00},//T_pll_enh=0
    {0x43, 0x02},//T_MIPI_sel=1
    {0xED, 0x01},
    {0xEF, 0x01},
    {0xF5, 0x01},
    {0x09, 0x01},
    {0xEF, 0x00},
    {0x10, 0x80},//Cmd_BYTECLK_InvSel=0 for PS308
    {0x11, 0x80},//GatedAllClk enable
    {0x13, 0x01},
    {0x16, 0xBC},
    {0x38, 0x28},
    {0x3C, 0x02},
    {0x5F, 0x64},
    {0x61, 0xDA},
    {0x62, 0x14},
    {0x69, 0x10},
    {0x75, 0x0B},
    {0x77, 0x06},
    {0x7E, 0x19},
    {0x85, 0x88},
    {0x9E, 0x00},
    {0xA0, 0x01},
    {0xA2, 0x09},
    {0xBE, 0x05},
    {0xBF, 0x80},
    {0xDF, 0x1E},
    {0xE1, 0x05},
    {0xE2, 0x03},
    {0xE3, 0x20},
    {0xE4, 0x0F},
    {0xE5, 0x07},
    {0xE6, 0x05},
    {0xF3, 0xB1},//B16A: Version
    {0xF8, 0x6A},//B16A: Version
    {0xED, 0x01},
    {0xEF, 0x01},
    {0x05, 0x0B},
    {0x07, 0x01},
    {0x08, 0x46},
    {0x0A, 0x04},
    {0x0B, 0x64},
    {0x0C, 0x00},
    {0x0D, 0x03},
    {0x0E, 0x08},
    {0x0F, 0x68},
    {0x18, 0x01},
    {0x19, 0x23},
    {0x27, 0x08},
    {0x28, 0xCC},
    {0x2A, 0x3C},
    {0x37, 0x90},
    {0x3A, 0xF0},
    {0x3B, 0x20},
    {0x40, 0xF0},
    {0x42, 0xD6},
    {0x43, 0x20},
    {0x5C, 0x1E},
    {0x5D, 0x0A},
    {0x5E, 0x32},
    {0x67, 0x11},
    {0x68, 0xF4},
    {0x69, 0xC2},
    {0x75, 0x3C},
    {0x7A, 0x00},
    {0x7B, 0x98},
    {0x7C, 0x07},
    {0x7D, 0xA0},
    {0x83, 0x02},
    {0x8F, 0x00},
    {0x90, 0x01},
    {0x92, 0x80},
    {0xA3, 0x00},
    {0xA4, 0x0E},
    {0xA5, 0x04},
    {0xA6, 0x40},
    {0xA7, 0x00},
    {0xA8, 0x00},
    {0xA9, 0x07},
    {0xAA, 0x88},
    {0xAB, 0x02},
    {0xAE, 0x28},
    {0xB0, 0x28},
    {0xB3, 0x0A},
    {0xB6, 0x08},
    {0xB7, 0x68},
    {0xB8, 0x04},
    {0xB9, 0xE4},
    {0xBE, 0x00},
    {0xBF, 0x00},
    {0xC0, 0x00},
    {0xC1, 0x00},
    {0xC4, 0x60},
    {0xC6, 0x60},
    {0xC7, 0x0A},
    {0xC8, 0xC8},
    {0xC9, 0x35},
    {0xCE, 0x6C},
    {0xCF, 0x82},
    {0xD0, 0x02},
    {0xD1, 0x60},
    {0xD5, 0x49},
    {0xD7, 0x0A},
    {0xD8, 0xE8},
    {0xDA, 0xC0},
    {0xDD, 0x42},
    {0xDE, 0x43},
    {0xE2, 0x9A},
    {0xE4, 0x00},
    {0xF0, 0x7C},
    {0xF1, 0x16},
    {0xF2, 0x24},
    {0xF3, 0x0C},
    {0xF4, 0x01},
    {0xF5, 0x19},
    {0xF6, 0x16},
    {0xF7, 0x00},
    {0xF8, 0x48},
    {0xF9, 0x05},
    {0xFA, 0x55},
    {0x09, 0x01},
    {0xEF, 0x02},
    {0x2E, 0x0C},
    {0x33, 0x8C},
    {0x3C, 0xC8},
    {0x4E, 0x02},
    {0xB0, 0x81},
    {0xED, 0x01},
    {0xEF, 0x05},
    {0x06, 0x05},
    {0x09, 0x09},
    {0x0A, 0x05},
    {0x0B, 0x06},
    {0x0C, 0x04},
    {0x0D, 0x5E},
    {0x0E, 0x01},
    {0x0F, 0x00},//MIPI CSI disable
    {0x10, 0x02},
    {0x11, 0x01},
    {0x15, 0x07},
    {0x17, 0x05},
    {0x18, 0x03},
    {0x1B, 0x03},
    {0x1C, 0x04},
    {0x40, 0x16},
    {0x41, 0x1A},
    {0x43, 0x02},//T_MIPI_sel[0], Bank5_67[1]=1
    {0x44, 0x01},//MIPI PLL enable
    {0x4A, 0x02},
    {0x4F, 0x01},
    {0x5B, 0x10},//clock non-continuous mode
    {0x94, 0x04},
    {0xB1, 0x00},
    {0xED, 0x01},
    {0xEF, 0x06},
    {0x00, 0x0C},
    {0x02, 0x13},
    {0x06, 0x02},
    {0x07, 0x02},
    {0x08, 0x02},
    {0x09, 0x02},
    {0x0F, 0x12},
    {0x10, 0x6C},
    {0x11, 0x12},
    {0x12, 0x6C},
    {0x18, 0x26},
    {0x1A, 0x26},
    {0x28, 0x00},
    {0x2D, 0x00},
    {0x2E, 0x6C},
    {0x2F, 0x6C},
    {0x4A, 0x26},
    {0x4B, 0x26},
    {0x9E, 0x80},
    {0xDF, 0x04},
    {0xED, 0x01},
    {0xEF, 0x00},
    {0x11, 0x00},//GatedAllClk disable
    {0xEF, 0x05},
    {0x3B, 0x00},//R_Cmd_Gated_MIPI_Clk=0
    {0xED, 0x01},
    {0xEF, 0x01},
    {0x02, 0xFB},//ResetTG
    {0x09, 0x01},
    {0xEF, 0x05},
    {0x0F, 0x01},//MIPI CSI enable
    {0xED, 0x01},
};

const I2C_ARRAY Sensor_init_table_1[] =
{
    {0xEF, 0x05},
    {0x0F, 0x00},
    {0x44, 0x00},//T_pll_enh=0
    {0x43, 0x02},//T_MIPI_sel=1
    {0xED, 0x01},
    {0xEF, 0x01},
    {0xF5, 0x01},
    {0x09, 0x01},
    {0xEF, 0x00},
    {0x10, 0x80},//Cmd_BYTECLK_InvSel=0 for PS308
    {0x11, 0x80},//GatedAllClk enable
    {0x13, 0x01},
    {0x16, 0xBC},
    {0x38, 0x28},
    {0x3C, 0x02},
    {0x5F, 0x64},
    {0x61, 0xDA},
    {0x62, 0x14},
    {0x69, 0x10},
    {0x75, 0x0B},
    {0x77, 0x06},
    {0x7E, 0x19},
    {0x85, 0x88},
    {0x9E, 0x00},
    {0xA0, 0x01},
    {0xA2, 0x09},
    {0xBE, 0x05},
    {0xBF, 0x80},
    {0xDF, 0x1E},
    {0xE1, 0x05},
    {0xE2, 0x03},
    {0xE3, 0x20},
    {0xE4, 0x0F},
    {0xE5, 0x07},
    {0xE6, 0x05},
    {0xF3, 0xB1},//B16A: Version
    {0xF8, 0x6A},//B16A: Version
    {0xED, 0x01},
    {0xEF, 0x01},
    {0x05, 0x0B},
    {0x07, 0x01},
    {0x08, 0x46},
    {0x0A, 0x04},
    {0x0B, 0x64},
    {0x0C, 0x00},
    {0x0D, 0x03},
    {0x0E, 0x11},
    {0x0F, 0x34},
    {0x18, 0x01},
    {0x19, 0x23},
    {0x27, 0x11},
    {0x28, 0x98},
    {0x2A, 0x3C},
    {0x37, 0x90},
    {0x3A, 0xF0},
    {0x3B, 0x20},
    {0x40, 0xF0},
    {0x42, 0xD6},
    {0x43, 0x20},
    {0x5C, 0x1E},
    {0x5D, 0x0A},
    {0x5E, 0x32},
    {0x67, 0x11},
    {0x68, 0xF4},
    {0x69, 0xC2},
    {0x75, 0x3C},
    {0x7B, 0x08},
    {0x7C, 0x08},
    {0x7D, 0x02},
    {0x83, 0x02},
    {0x8F, 0x07},
    {0x90, 0x01},
    {0x92, 0x80},
    {0xA3, 0x00},
    {0xA4, 0x0E},
    {0xA5, 0x04},
    {0xA6, 0x40},
    {0xA7, 0x00},
    {0xA8, 0x00},
    {0xA9, 0x07},
    {0xAA, 0x88},
    {0xAB, 0x01},
    {0xAE, 0x28},
    {0xB0, 0x28},
    {0xB3, 0x0A},
    {0xB6, 0x11},
    {0xB7, 0x34},
    {0xBE, 0x00},
    {0xBF, 0x00},
    {0xC0, 0x00},
    {0xC1, 0x00},
    {0xC4, 0x60},
    {0xC6, 0x60},
    {0xC7, 0x0A},
    {0xC8, 0xC8},
    {0xC9, 0x35},
    {0xCE, 0x6C},
    {0xCF, 0x82},
    {0xD0, 0x02},
    {0xD1, 0x60},
    {0xD5, 0x49},
    {0xD7, 0x0A},
    {0xD8, 0xC8},
    {0xDA, 0xC0},
    {0xDD, 0x42},
    {0xDE, 0x43},
    {0xE2, 0x9A},
    {0xE4, 0x00},
    {0xF0, 0x7C},
    {0xF1, 0x16},
    {0xF2, 0x24},
    {0xF3, 0x0C},
    {0xF4, 0x01},
    {0xF5, 0x19},
    {0xF6, 0x16},
    {0xF7, 0x00},
    {0xF8, 0x48},
    {0xF9, 0x05},
    {0xFA, 0x55},
    {0x09, 0x01},
    {0xEF, 0x02},
    {0x2E, 0x0C},
    {0x33, 0x8C},
    {0x3C, 0xC8},
    {0x4E, 0x02},
    {0xB0, 0x81},
    {0xED, 0x01},
    {0xEF, 0x05},
    {0x06, 0x05}, //12bits
    {0x09, 0x09},
    {0x0A, 0x05},
    {0x0B, 0x06},
    {0x0C, 0x04},
    {0x0D, 0x5E},
    {0x0E, 0x01},
    {0x0F, 0x00},//MIPI CSI disable
    {0x10, 0x02},
    {0x11, 0x01},
    {0x15, 0x07},
    {0x17, 0x05},
    {0x18, 0x03},
    {0x1B, 0x03},
    {0x1C, 0x04},
    {0x40, 0x16},
    {0x41, 0x1A},
    {0x43, 0x02},//T_MIPI_sel[0], Bank5_67[1]=1
    {0x44, 0x01},//MIPI PLL enable
    {0x4A, 0x02},
    {0x4F, 0x01},
    {0x5B, 0x10},//clock non-continuous mode
    {0x94, 0x04},
    {0xB1, 0x00},
    {0xED, 0x01},
    {0xEF, 0x06},
    {0x00, 0x0C},
    {0x02, 0x13},
    {0x06, 0x02},
    {0x07, 0x02},
    {0x08, 0x02},
    {0x09, 0x02},
    {0x0F, 0x12},
    {0x10, 0x6C},
    {0x11, 0x12},
    {0x12, 0x6C},
    {0x18, 0x26},
    {0x1A, 0x26},
    {0x28, 0x00},
    {0x2D, 0x00},
    {0x2E, 0x6C},
    {0x2F, 0x6C},
    {0x4A, 0x26},
    {0x4B, 0x26},
    {0x9E, 0x01}, //comp on
    {0xDF, 0x04},
    {0xED, 0x01},
    {0xEF, 0x00},
    {0x11, 0x00},//GatedAllClk disable
    {0xEF, 0x05},
    {0x3B, 0x00},//R_Cmd_Gated_MIPI_Clk=0
    {0xED, 0x01},
    {0xEF, 0x01},
    {0x02, 0xFB},//ResetTG
    {0x09, 0x01},
    {0xEF, 0x05},
    {0x0F, 0x01},//MIPI CSI enable
    {0xED, 0x01}
};

I2C_ARRAY Sensor_init_table_2[] = { //res_id=2 , 1920x1080 14bit Sensor HDR mode to Linear output
{0xEF, 0x05},
{0x0F, 0x00},
{0x44, 0x00},//T_pll_enh=0
{0x43, 0x02},//T_MIPI_sel=1
{0xED, 0x01},
{0xEF, 0x01},
{0xF5, 0x01},
{0x09, 0x01},
{0xEF, 0x00},
{0x10, 0x80},//Cmd_BYTECLK_InvSel=0 for PS308
{0x11, 0x80},//GatedAllClk enable
{0x13, 0x01},
{0x16, 0xBC},
{0x38, 0x28},
{0x3C, 0x02},
{0x5F, 0x64},
{0x61, 0xDA},
{0x62, 0x14},
{0x69, 0x10},
{0x75, 0x0B},
{0x77, 0x06},
{0x7E, 0x19},
{0x85, 0x88},
{0x9E, 0x00},
{0xA0, 0x01},
{0xA2, 0x09},
{0xBE, 0x05},
{0xBF, 0x80},
{0xDF, 0x1E},
{0xE1, 0x05},
{0xE2, 0x03},
{0xE3, 0x20},
{0xE4, 0x0F},
{0xE5, 0x07},
{0xE6, 0x05},
{0xF3, 0xB1},//B17A: Version
{0xF8, 0x7A},//B17A: Version
{0xED, 0x01},
{0xEF, 0x01},
{0x05, 0x0B},
{0x07, 0x01},
{0x08, 0x46},
{0x0A, 0x04},
{0x0B, 0x64},
{0x0C, 0x00},
{0x0D, 0x03},
{0x0E, 0x11},
{0x0F, 0x34},
{0x18, 0x01},
{0x19, 0x23},
{0x27, 0x11},
{0x28, 0x98},
{0x2A, 0x3C},
{0x37, 0x90},
{0x3A, 0xF0},
{0x3B, 0x20},
{0x40, 0xF0},
{0x42, 0xD6},
{0x43, 0x20},
{0x5C, 0x1E},
{0x5D, 0x0A},
{0x5E, 0x32},
{0x67, 0x11},
{0x68, 0xF4},
{0x69, 0xC2},
{0x75, 0x3C},
{0x7B, 0x08},
{0x7C, 0x08},
{0x7D, 0x02},
{0x83, 0x02},
{0x8F, 0x07},
{0x90, 0x01},
{0x92, 0x80},
{0xA3, 0x00},
{0xA4, 0x0E},
{0xA5, 0x04},
{0xA6, 0x40},
{0xA7, 0x00},
{0xA8, 0x00},
{0xA9, 0x07},
{0xAA, 0x88},
{0xAB, 0x01},
{0xAE, 0x28},
{0xB0, 0x28},
{0xB3, 0x0A},
{0xB6, 0x11},
{0xB7, 0x34},
{0xBE, 0x00},
{0xBF, 0x00},
{0xC0, 0x00},
{0xC1, 0x00},
{0xC4, 0x60},
{0xC6, 0x60},
{0xC7, 0x0A},
{0xC8, 0xC8},
{0xC9, 0x35},
{0xCE, 0x6C},
{0xCF, 0x82},
{0xD0, 0x02},
{0xD1, 0x60},
{0xD5, 0x49},
{0xD7, 0x0A},
{0xD8, 0xC8},
{0xDA, 0xC0},
{0xDD, 0x42},
{0xDE, 0x43},
{0xE2, 0x9A},
{0xE4, 0x00},
{0xF0, 0x7C},
{0xF1, 0x16},
{0xF2, 0x24},
{0xF3, 0x0C},
{0xF4, 0x01},
{0xF5, 0x19},
{0xF6, 0x16},
{0xF7, 0x00},
{0xF8, 0x48},
{0xF9, 0x05},
{0xFA, 0x55},
{0x09, 0x01},
{0xEF, 0x02},
{0x2E, 0x0C},
{0x33, 0x8C},
{0x3C, 0xC8},
{0x4E, 0x02},
{0xB0, 0x81},
{0xED, 0x01},
{0xEF, 0x05},
{0x06, 0x06},
{0x09, 0x09},
{0x0A, 0x05},
{0x0B, 0x06},
{0x0C, 0x04},
{0x0D, 0x5E},
{0x0E, 0x01},
{0x0F, 0x00},//MIPI CSI disable
{0x10, 0x02},
{0x11, 0x01},
{0x15, 0x07},
{0x17, 0x05},
{0x18, 0x03},
{0x1B, 0x03},
{0x1C, 0x04},
{0x25, 0x01}, //B17A
{0x40, 0x16},
{0x41, 0x1A},
{0x43, 0x02},//T_MIPI_sel[0], Bank5_67[1]=1
{0x44, 0x01},//MIPI PLL enable
{0x4A, 0x02},
{0x4F, 0x01},
{0x5B, 0x10},
{0x94, 0x04},
{0xB1, 0x00},
{0xED, 0x01},
{0xEF, 0x06},
{0x00, 0x0C},
{0x02, 0x13},
{0x06, 0x02},
{0x07, 0x02},
{0x08, 0x02},
{0x09, 0x02},
{0x0F, 0x12},
{0x10, 0x6C},
{0x11, 0x12},
{0x12, 0x6C},
{0x18, 0x26},
{0x1A, 0x26},
{0x28, 0x00},
{0x2D, 0x00},
{0x2E, 0x6C},
{0x2F, 0x6C},
{0x4A, 0x26},
{0x4B, 0x26},
{0x9E, 0x80},
{0xDF, 0x04},
{0xED, 0x01},
{0xEF, 0x00},
{0x11, 0x00},//GatedAllClk disable
{0xEF, 0x05},
{0x3B, 0x00},//R_Cmd_Gated_MIPI_Clk=0
{0xED, 0x01},
{0xEF, 0x01},
{0x02, 0xFB},//ResetTG
{0x09, 0x01},
{0xEF, 0x05},
{0x0F, 0x01},//MIPI CSI enable
{0xED, 0x01},
{0xDL, 0x10},//B17A, Delay time need bigger than Vblanking interval
{0x25, 0x00},//B17A
};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};


I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
{0xEF, 0x01},//M0F0
{0x1b, 0x07},//bit7,Hflip
//{0x90, 0x04},//bit[7:4] Cmd_ADC_Latency, no used
{0x1d, 0x04},//bit7,Vflip
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
    {0x1b, 0x07},//bit7,Hflip
//    {0x90, 0x00},//bit2 Cmd_ADC_Latency, no used
    {0x1d, 0x04},//bit7,Vflip

    {0xEF, 0x01},//M1F0
    {0x1b, 0x87},//bit7,Hflip
//    {0x90, 0x04},//bit2 Cmd_ADC_Latency, no used
    {0x1d, 0x04},//bit7,Vflip

    {0xEF, 0x01},//M0F1
    {0x1b, 0x07},//bit7,Hflip
//    {0x90, 0x00},//bit2 Cmd_ADC_Latency, no used
    {0x1d, 0x84},//bit7,Vflip

    {0xEF, 0x01},//M1F1
    {0x1b, 0x87},//bit7,Hflip
//    {0x90, 0x04},//bit2 Cmd_ADC_Latency, no used
    {0x1d, 0x84},//bit7,Vflip

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
    {0x80, 0x00},
    {0x09, 0x01},
};

// Davis 20181101
const static Gain_ARRAY gain_table[]={
    {1024 ,0   },
    {1088 ,1   },
    {1152 ,2   },
    {1216 ,3   },
    {1280 ,4   },
    {1344 ,5   },
    {1408 ,6   },
    {1472 ,7   },
    {1536 ,8   },
    {1600 ,9   },
    {1664 ,10  },
    {1728 ,11  },
    {1792 ,12  },
    {1856 ,13  },
    {1920 ,14  },
    {1984 ,15  },
    {2048 ,16  },
    {2176 ,17  },
    {2304 ,18  },
    {2432 ,19  },
    {2560 ,20  },
    {2720 ,21  },
    {2880 ,22  },
    {3040 ,23  },
    {3200 ,24  },
    {3360 ,25  },
    {3520 ,26  },
    {3680 ,27  },
    {3840 ,28  },
    {4000 ,29  },
    {4160 ,30  },
    {4320 ,31  },
    {4480 ,32  },
    {4640 ,33  },
    {4800 ,34  },
    {4960 ,35  },
    {5120 ,36  },
    {5440 ,37  },
    {5760 ,38  },
    {6080 ,39  },
    {6400 ,40  },
    {6720 ,41  },
    {7040 ,42  },
    {7360 ,43  },
    {7680 ,44  },
    {8000 ,45  },
    {8320 ,46  },
    {8640 ,47  },
    {8960 ,48  },
    {9280 ,49  },
    {9600 ,50  },
    {9920 ,51  },
    {10240, 52 },
    {10880, 53 },
    {11520, 54 },
    {12160, 55 },
    {12800, 56 },
    {13440, 57 },
    {14080, 58 },
    {14720, 59 },
    {15360, 60 },
    {16000, 61 },
    {16640, 62 },
    {17280, 63 },
    {17920, 64 },
    {18560, 65 },
    {19200, 66 },
    {19840, 67 },
    {20480, 68 },	//20x
#if 0 	
    {21760, 69 },
    {23040, 70 },
    {24320, 71 },
    {25600, 72 },
    {26880, 73 },
    {28160, 74 },
    {29440, 75 },
    {30720, 76 },
    {32000, 77 },
    {33280, 78 },
    {34560, 79 },
    {35840, 80 },
    {37120, 81 },
    {38400, 82 },
    {39680, 83 },
    {40960, 84 },
    {43520, 85 },
    {46080, 86 },
    {48640, 87 },
    {51200, 88 },
    {53760, 89 },
    {56320, 90 },
    {58880, 91 },
    {61440, 92 },
    {64000, 93 },
    {66560, 94 },
    {69120, 95 },
    {71680, 96 },
    {74240, 97 },
    {76800, 98 },
    {79360, 99 },
    {81920, 100},
#else	// 20200513 for normal usage
	{21504, 69},
	{22528, 70},
	{23552, 71},
	{24576, 72},
	{25600, 73},
	{26624, 74},
	{27648, 75},
	{28672, 76},
	{29696, 77},
	{30720, 78},
	{31744, 79},
	{32768, 80},
	{34816, 81},
	{36864, 82},
	{38912, 83},
	{40960, 84},
	{43008, 85},
	{45056, 86},
	{47104, 87},
	{49152, 88},
	{51200, 89},
	{53248, 90},
	{55296, 91},
	{57344, 92},
	{59392, 93},
	{61440, 94},
	{63488, 95},
	{65536, 96},	
#endif
};

static int g_sensor_ae_min_gain = 1152;
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
    {0x0D, 0x03},
    {0x0E, 0x10},
    {0x0F, 0xCC},
    {0x10, 0x00},
    {0x12, 0x50},
    {0x09, 0x01},
};

const I2C_ARRAY vts_reg[] = {
    {0xEF, 0x01},
    {0x0A, 0x04},
    {0x0B, 0x64},
    {0x09, 0x01},

};
#if ENABLE_ver
const I2C_ARRAY nr_reg[] = {
    {0xef, 0x00},
    {0x74, 0x00},
    {0x75, 0x0b},
    {0x76, 0x00},
    {0x77, 0x06},
    {0xed, 0x01},

};
#endif

#if 0
u16 u32TblABC_LR[][2] =
{
    {0,                                                 0},
    {((1<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,   ((1<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((4<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,   ((4<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((9<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,   ((9<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((16<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((16<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((25<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((25<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((36<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((36<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((49<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((49<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((64<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((64<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((81<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL,  ((81<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((100<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((100<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((121<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((121<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((144<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((144<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((169<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((169<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((196<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((196<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((225<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((225<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((256<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((256<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((289<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((289<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((324<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((324<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((361<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((361<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((400<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((400<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((441<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((441<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((484<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((484<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((529<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((529<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((576<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((576<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((625<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((625<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((676<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((676<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((729<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((729<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((784<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((784<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((841<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((841<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((900<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((900<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
    {((961<<16)-(SGHD_HS_FOR_TBL>>12))/SGHD_HS_FOR_TBL, ((961<<16)-(SGHD_LS_FOR_TBL>>12))/SGHD_LS_FOR_TBL},
};
const u16 u32TblABC_LRSize = (sizeof(u32TblABC_LR)/(sizeof(u16)*2));
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
#define SENSOR_NAME ps5260


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
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x7C00, 0);

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

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_USLEEP(1000);
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
    SENSOR_DMSG("[%s]ps5260 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ps5260_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    ps5260_params *params = (ps5260_params *)handle->private_data;
    int i,cnt=0;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(i=0;i<params->nInitTableSize;i++)
    {
        if(params->pInitTable[i].reg==0xff)
        {
            SENSOR_MSLEEP(params->pInitTable[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(params->pInitTable[i].reg,params->pInitTable[i].data) != SUCCESS)
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

    if(params->nCompanding)
    {
        SensorReg_Write(0xEF,0x06);
        SensorReg_Write(0xDF,0x00);
		SensorReg_Write(0xed,0x01);	// Updated.
    }
    else
    {
        SensorReg_Write(0xEF,0x00);
        SensorReg_Read(0x74,&gu16DgainUBH);
        SensorReg_Read(0x75,&gu16DgainUBL);
        gu16DgainUB = (gu16DgainUBH<<8) + gu16DgainUBL;
        SensorReg_Read(0x76,&gu16DgainLBH);
        SensorReg_Read(0x77,&gu16DgainLBL);
        gu16DgainLB = (gu16DgainLBH<<8) + gu16DgainLBL;
    }
    SensorReg_Write(0xEF,0x01);    // return to bank1 for AE

    FirstTime=1;

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
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
    ps5260_params *params = (ps5260_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0: //linear 12 bit
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            params->pInitTable = Sensor_init_table_0;
            params->nInitTableSize = ARRAY_SIZE(Sensor_init_table_0);
            params->nCompanding = 0;
            handle->data_prec = CUS_DATAPRECISION_12;
            handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
            break;
        case 1: //HDR Companding 14 to 12
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init;
            params->pInitTable = Sensor_init_table_1;
            params->nInitTableSize = ARRAY_SIZE(Sensor_init_table_1);
            params->nCompanding = 1;
            handle->data_prec = CUS_DATAPRECISION_12;
            handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_EMBEDDED_RAW12;
            break;
        case 2: //14BIT mode , HDR no companding
            handle->video_res_supported.ulcur_res = 2;
            handle->pCus_sensor_init = pCus_init;
            params->pInitTable = Sensor_init_table_2;
            params->nInitTableSize = ARRAY_SIZE(Sensor_init_table_2);
            params->nCompanding = 0;
            handle->data_prec = CUS_DATAPRECISION_14;
            handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
#if 0
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
#else
    *orit = handle->orient;
#endif
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    ps5260_params *params = (ps5260_params *)handle->private_data;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = orit;
            for(i=0,j=0;i<seg_length;i++,j++){
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M1F0:
            handle->orient = orit;
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M0F1:
            handle->orient = orit;
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M1F1:
            handle->orient = orit;
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            break;

        default :
            handle->orient = CUS_ORIT_M0F0;
            for(i=0,j=0;i<seg_length;i++,j++){
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            break;
    }
    params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ps5260_params *params = (ps5260_params *)handle->private_data;
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
    ps5260_params *params = (ps5260_params *)handle->private_data;
	//u16 const1,const2,const3,const4;
    u16 u16UB,u16LB;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=5 && fps <= 30){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30+(fps>>1))/fps)-1;
    }else if(fps>=5000 && fps <= 30000){
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30000+(fps>>1))/fps)-1;
    }else{
        //params->expo.vts=vts_25fps;
        //params->expo.fps=25;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
#if ENABLE_ver

    if(!params->nCompanding)
    {
        //temp compensate
        u16UB = (gu16DgainUB&0xf800)|((gu16DgainUB&0x07ff)*(params->expo.vts +1)/vts_30fps);
        u16LB = (gu16DgainLB&0xf800)|((gu16DgainLB&0x07ff)*(params->expo.vts +1)/vts_30fps);
        //SENSOR_DMSG("[%s]u16UB = %d, u16LB = %d\n", __FUNCTION__, u16UB, u16LB);

        params->tNr_reg[1].data = (u16UB >> 8)& 0x00ff;
        params->tNr_reg[2].data = u16UB&0x00ff;
        params->tNr_reg[3].data = (u16LB >> 8)& 0x00ff;
        params->tNr_reg[4].data = u16LB&0x00ff;
        FirstTime = 1;
		//SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, ARRAY_SIZE(nr_reg));	// No need to set in blanking...
    }

#endif

    u32Temp_Manual = (u32Temp_Manual*params->expo.vts)/((params->tVts_reg[1].data<<8)+params->tVts_reg[2].data);

    params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;

    params->dirty = true;
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
    ps5260_params *params = (ps5260_params *)handle->private_data;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;  //to do (skip method)
    //static u16 u16PreTempManual=0xFFFF, u16TempManual =0;
    //u32 LPF, IntTime;
    //u16 cnt, FDC, FDC_ABS, bSigned, TempNew;
    //u16 data1, data2, data3, data4, BLC1, BLC2;
    u16 data1, data2, TempNew;
    u16 a=0,b=0;
    u32 line_read =0,line_write =0;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            if(params->orient_dirty)
            {
                /*Drop MIPI frame begin*/
                //handle->sensor_if_api->SetSkipFrame(handle->snr_pad_group, params->expo.cur_fps, 1);
                SensorRegArrayW( (I2C_ARRAY*)Current_Mirror_Flip_Tbl, ARRAY_SIZE(Current_Mirror_Flip_Tbl));
                SensorReg_Write(0x09,1);
                params->orient_dirty = false;
            }
        break;
        case CUS_FRAME_ACTIVE:
            ++framecount;
            ++fps_delay;
            //SENSOR_DMSG("[%s] frame %lld\n", __FUNCTION__,framecount);
            SensorReg_Write(0xef,0x01);
            SensorReg_Read(0xc,&a);
            SensorReg_Read(0xd,&b);
            line_read |= (u32)(a&0xff)<<8;
            line_read |= (u32)(b&0xff)<<0;
            line_write|= (u32)(params->tExpo_reg[1].data&0xff)<<8;
            line_write|= (u32)(params->tExpo_reg[2].data&0xff)<<0;
            if(line_read > params->expo.vts && line_read != line_write){
                fps_delay=0;
            }
            if(fps_delay == 4){
                //sensor_if1->SetSkipFrame(handle,3); //to do (skip method)
            }

            if(params->dirty)
            {
				SensorReg_Write(0xef,0x01);
                SensorRegArrayW((I2C_ARRAY*)&params->tExpo_reg[1], ARRAY_SIZE(expo_reg)-2);
                SensorRegArrayW((I2C_ARRAY*)&params->tGain_reg[1], ARRAY_SIZE(gain_reg)-2);
                SensorRegArrayW((I2C_ARRAY*)&params->tVts_reg[1], ARRAY_SIZE(vts_reg)-2);
				SensorReg_Write(0x09,0x01);
//#if ENABLE_ver
//                SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, ARRAY_SIZE(nr_reg));
//#endif
             //   SensorReg_Write(0xef,0x01);
             //   SensorReg_Write(0x09,1);
                params->dirty = false;
            }

#if 0
            if(framecount > ((params->expo.fps)/1000)/4)	// run at every 0.25s
            {
                framecount = 0;
                SensorReg_Write(0xef,0x00);
                SensorReg_Read(0xb7,&data1);
                SensorReg_Read(0xb8,&data2);
                SensorReg_Read(0xbb,&data3);
                SensorReg_Read(0xbc,&data4);
                LPF = params->expo.vts;
                IntTime = params->expo.line;
                FDC = ((data1&0x0f)<<8) + data2;
                FDC_ABS = ((data3&0x30)<<4) + data4;
                bSigned = (data3&0x40)>>6;
                TempNew = bSigned?(FDC + FDC_ABS):((FDC > FDC_ABS)?(FDC-FDC_ABS):0);
                TempNew = (TempNew * LPF * (params->expo.sens?0x23:0x10))/(IntTime<<4);
                if(FirstTime == 1)
                {
                    //u32Temp_Manual = TempNew<<8;
                }else{
                    u32Temp_Manual = TempNew + u32Temp_Manual - u16TempManual;
                }
                u16TempManual = (u32Temp_Manual+8)>>4;
                //SENSOR_DMSG("[%s] u16TempManual 0x%x\n", __FUNCTION__,u16TempManual);

                BLC1 = 0xc0|((u16TempManual>>8)&0x07);
                BLC2 = u16TempManual&0xff;
                //SENSOR_DMSG("[%s] 0x45:0x%x , 0x46:0x%x\n", __FUNCTION__,BLC1,BLC2);
                SensorReg_Write(0xef,0x06);
                SensorReg_Write(0x45,BLC1);
                SensorReg_Write(0x46,BLC2);
            }
            else if(u16PreTempManual != u16TempManual)
            {
                if(FirstTime == 1)
                {
                    u32LRUB = 5;
                    FirstTime = 0;
                }
                else
                {
                    for(cnt =3; cnt < u32TblABC_LRSize; cnt++)
                    {
                        if(u16TempManual < u32TblABC_LR[cnt][params->expo.sens])    break;
                    }
                    // Davis 20181029
                    //cnt = ((cnt < 3)?3:((cnt > 31)?31:cnt));
                    //if(cnt < 3) cnt = 3;
                    cnt = (cnt*((u32GainReport>>10)+31)+16)>>5; // 3.0x by Gain
                    if(cnt > 31) cnt =31;

                    if(u32LRUB > cnt)   u32LRUB--;
                    else if(u32LRUB < cnt)  u32LRUB++;
                }

                //SENSOR_DMSG("[%s] u32LRUB = %d, cnt = %d, u32GainReport = %d, u16TempManual = %d\n", __FUNCTION__, u32LRUB, cnt, u32GainReport, u16TempManual);

                u16PreTempManual = u16TempManual;
                SensorReg_Write(0xef,0x02);
                SensorReg_Write(0x2e,u32LRUB);
                SensorReg_Write(0x33,0x80|u32LRUB);
                SensorReg_Write(0xed,0x01);

                // patch // Davis 20181101
                /*{
                    u16 u16UB, u16LB;

                    u16UB = ((params->tNr_reg[1].data&0x7)<<8)|params->tNr_reg[2].data;
                    u16LB = ((params->tNr_reg[3].data&0x7)<<8)|params->tNr_reg[4].data;
                    //SENSOR_DMSG("[%s] u16UB:0x%x \n", __FUNCTION__,u16UB);
                    //SENSOR_DMSG("[%s] u16LB:0x%x \n", __FUNCTION__,u16LB);

                    if(u16TempManual > u16UB)   u16GainMin++;
                    else if((u16GainMin > 0)&&(u16TempManual < u16LB))  u16GainMin--;

                    u16GainMin = (u16GainMin > 4)?4:u16GainMin;

                    SENSOR_DMSG("[%s]u8Frac = %d\n", __FUNCTION__, u8Frac);

                    //SensorReg_Write(0xef,0x06);
                    //SensorReg_Write(0xDF,0);
                    //SensorReg_Write(0xed,0x01);
                }*/

            }
#else
            if((params->nCompanding)&&(framecount > ((params->expo.fps)/1000)/4))	// run at every 0.25s
			{
				u16 u16UB, u16LB;
				
				// Read Temp
                framecount = 0;
                SensorReg_Write(0xef,0x02);
                SensorReg_Read(0xC0,&data1);
                SensorReg_Read(0xC1,&data2);
				TempNew = ((data1&0x07)<<8) + data2;
				// Calculate Again_Min
				u16UB = ((params->tNr_reg[1].data&0x7)<<8)|params->tNr_reg[2].data;
				u16LB = ((params->tNr_reg[3].data&0x7)<<8)|params->tNr_reg[4].data;

				if(TempNew > u16UB)   u16GainMin++;
				else if((u16GainMin > 0)&&(TempNew < u16LB))  u16GainMin--;

				u16GainMin = (u16GainMin > 4)?4:u16GainMin;
				if((params->tGain_reg[2].data)&&(params->tGain_reg[1].data < u16GainMin))
				{
					params->tGain_reg[1].data = u16GainMin;
					params->dirty = true;
				}
			}
#endif

            break;
            default :
            break;
    }
            /*short aa,bb,cc;
            SensorReg_Write(0xef,0x02);
            SensorReg_Read(0x2e,&aa);
            //SensorReg_Write(0xef,0x00);
            SensorReg_Read(0x33,&bb);
            //SensorReg_Read(0x18,&bb);
            //SensorReg_Read(0x80,&cc);
            SENSOR_DMSG("[%s] 0x2e:0x%x \n", __FUNCTION__,aa);
            SENSOR_DMSG("[%s] 0x33:0x%x \n", __FUNCTION__,bb);
            //SENSOR_DMSG("[%s] 0x80:0x%x \n", __FUNCTION__,cc);*/
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    ps5260_params *params = (ps5260_params *)handle->private_data;
    u32 lines = 0;

    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xff)<<0;

    lines=params->expo.vts-lines;

    *us = lines*Preview_line_period/1000;
    //SENSOR_DMSG("====================================================\n");
    //SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);
    //SENSOR_DMSG("====================================================\n");

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0, ne =0, ne_patch = 0, g_sns_const = 0;
    ps5260_params *params = (ps5260_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if(lines < 2) lines = 2;
    if (lines > params->expo.vts-2) {
        vts = lines +2;
    }
    else
      vts=params->expo.vts;

    //SENSOR_DMSG("====================================================\n");
    //SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,us,lines, params->expo.vts);
    //SENSOR_DMSG("====================================================\n");
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    params->expo.line = lines;
    params->expo.vts=vts;
    lines=vts-lines;

#if 1//PS5260 needn't SW ne_patch
    g_sns_const=0x111c;
    ne_patch=16+((38*lines)>>8);
    if (ne_patch > 200){
        ne_patch = 200;
    }
    ne = g_sns_const - ne_patch;
#endif

    params->tExpo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[2].data =(u16)( (lines>>0) & 0x00ff);
#if 1//PS5260 needn't SW ne_patch
    params->tExpo_reg[3].data = (u16)((ne >>8) & 0x001f);
    params->tExpo_reg[4].data = (u16)((ne >>0) & 0x00ff);
    params->tExpo_reg[5].data = (u16)(((ne_patch & 0x0100) >>8)<<2);
    params->tExpo_reg[6].data = (u16)((ne_patch >>0) & 0x00ff);
#endif

    u32Temp_Manual = (u32Temp_Manual*vts)/((params->tVts_reg[1].data<<8)+params->tVts_reg[2].data);

    params->tVts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((vts >> 0) & 0x00ff);


    //SENSOR_DMSG("[%s] lines = %d, ne = %d, ne_patch = %d, vts = %d\n", __FUNCTION__, lines, ne, ne_patch, vts);
    //SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
    //SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
    //SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
  ps5260_params *params = (ps5260_params *)handle->private_data;
    //u16  temp_gain;
    //double  temp_gain_double;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);
   // SENSOR_DMSG("set gain/reg=%d/0x%x\n", gain,params->tGain_reg[1].data);

    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {
    ps5260_params *params = (ps5260_params *)handle->private_data;
    u32 i;
    //CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;

    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    for(i=0;i<96;i++)
    {
        if(gain < gain_table[i].total_gain)    break;
    }
    params->tGain_reg[1].data = (params->tGain_reg[2].data)?i:(i-16);
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    ps5260_params *params = (ps5260_params *)handle->private_data;
    u32 i;
    static u8 sens = 0;//, ratio = 4;  // Davis 20181101
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;

    params->expo.final_gain = gain;
    u32GainReport = gain;   // Davis 20181029
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){
        //SENSOR_DMSG("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    //SENSOR_DMSG("[%s] set total_gain=%d\n", __FUNCTION__,gain);
    if(gain < 4*1024){
      sens = 1;     // LS
      //ratio = 0;
    }else if(gain > 6*1024){
      sens = 0;     // HS
      // Davis 20181101 -- 20200513 remark for normal usage
      //if(gain < 20*1024)    ratio = 0;
      //else if(gain > 30*1024)   ratio = 4;
    }

    //for(i = 0;i < 100;i++)
    for(i = u16GainMin;i < 96;i++)
    {
        if(gain < gain_table[i].total_gain)    break;
    }

    //params->tGain_reg[1].data = sens ?i:(i-(ratio?20:16));    // Davis 20181101
    params->tGain_reg[1].data = sens ?i:(i-16);    // Davis 20181101 -- 20200513 for normal usage
    params->tGain_reg[2].data = sens;
    //params->tGain_reg[3].data = ratio;    // Davis 20181101 -- 20200513 remark for normal usage

    //SENSOR_DMSG("[%s] in_gain = %d, sens = %d, gain = %d, ratio = %d\n", __FUNCTION__, gain, sens, params->tGain_reg[1].data, ratio);
    //SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
    SENSOR_DMSG("[%s] set gain/regH/regL/ratio=%d/0x%x/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data,params->tGain_reg[2].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 30;
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int ps5260_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 1;
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

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    ps5260_params *params;
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
    params = (ps5260_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
#if ENABLE_ver
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
#endif

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ps5260_MIPI");

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

    handle->video_res_supported.num_res = 3;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 0;
    handle->video_res_supported.res[0].nOutputHeight= 0;
    strcpy(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= 0;
    handle->video_res_supported.res[1].crop_start_y= 0;
    handle->video_res_supported.res[1].nOutputWidth= 0;
    handle->video_res_supported.res[1].nOutputHeight= 0;
    strcpy(handle->video_res_supported.res[1].strResDesc, "HDR 1920x1080@30fps");

    handle->video_res_supported.res[2].width = Preview_WIDTH;
    handle->video_res_supported.res[2].height = Preview_HEIGHT;
    handle->video_res_supported.res[2].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[2].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[2].crop_start_x= 0;
    handle->video_res_supported.res[2].crop_start_y= 0;
    handle->video_res_supported.res[2].nOutputWidth= 0;
    handle->video_res_supported.res[2].nOutputHeight= 0;
    strcpy(handle->video_res_supported.res[2].strResDesc, "14BIT 1920x1080@30fps");

    params->pInitTable = &Sensor_init_table_0[0];
    params->nInitTableSize = ARRAY_SIZE(Sensor_init_table_0);

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
    handle->pCus_sensor_SetPatternMode = ps5260_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = ps5260_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line = 100;
    params->expo.sens = 1;
    params->dirty = false;
    params->orient_dirty = false;
    params->nCompanding = 0;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  PS5260,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ps5260_params
                         );

