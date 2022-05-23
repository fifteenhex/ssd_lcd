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
#include <linux/delay.h>
#include <drv_ms_cus_sensor.h>
#include <drv_sensor.h>
#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(PS5280);

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
#define Preview_line_period 29630                  //
//#define Line_per_second     32727
#define vts_30fps  1125//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 1080                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1920                   //resolution Width when preview
#define Preview_HEIGHT      1080                   //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR     0x90                    //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
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
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
} ps5280_params;
// set sensor ID address and data

typedef struct {
    u64 total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

I2C_ARRAY Sensor_id_table[] =
{
{0x00, 0x52},      // {address of ID, ID },
{0x01, 0x80},
};

I2C_ARRAY Sensor_init_table[] =
{
//PNS309_1936x1096_sys27_pix74p25_ImgSyn_20180503_C01_asc_400Mbps_C03b.asc
{0xEF, 0x05},
{0x0F, 0x00},
{0x42, 0x00},
{0x43, 0x02},
{0xED, 0x01},
{0xEF, 0x01},
{0xF5, 0x01},
{0x09, 0x01},
{0xEF, 0x00},
{0x0E, 0x45},
{0x10, 0x80},
{0x11, 0x80},
{0x16, 0xBC},
{0x36, 0x07},
{0x37, 0x07},
{0x38, 0x10},
{0x63, 0x01},
{0x64, 0x90},
{0x67, 0x04},
{0x68, 0xA0},
{0x69, 0x10},
{0x81, 0xC4},
{0x85, 0x78},
{0x9E, 0x40},
{0xA3, 0x04},
{0xA4, 0x7F},
{0xBE, 0x05},
{0xDE, 0x03},
{0xDF, 0x34},
{0xE0, 0x10},
{0xE1, 0x0A},
{0xE2, 0x09},
{0xE3, 0x34},
{0xE4, 0x10},
{0xE5, 0x0A},
{0xE6, 0x09},
{0xED, 0x01},
{0xEF, 0x01},
{0x04, 0x10},
{0x05, 0x0B},
{0x0A, 0x04},
{0x0B, 0x64},
{0x0C, 0x00},
{0x0D, 0x04},
{0x0E, 0x0F},
{0x0F, 0xA0},
{0x10, 0xC0},
{0x11, 0x3B},
{0x19, 0x40},
{0x2E, 0xB4},
{0x32, 0x1E},
{0x33, 0x62},
{0x37, 0x80},
{0x39, 0x88},
{0x3A, 0x1E},
{0x3B, 0x6A},
{0x3E, 0x11},
{0x3F, 0xC8},
{0x40, 0x1E},
{0x42, 0xE8},
{0x43, 0x62},
{0x5D, 0x08},
{0x5F, 0x32},
{0x65, 0x1E},
{0x66, 0x62},
{0x78, 0x00},
//{0x8F, 0x05},
{0x8F, 0x00}, //hdr off
{0x90, 0x00},
{0x96, 0x80},
{0xA4, 0x0A},
{0xA5, 0x04},
{0xA6, 0x48},
{0xA8, 0x00},
{0xA9, 0x07},
{0xAA, 0x90},
{0xAB, 0x01},
{0xB6, 0x0F},
{0xB7, 0xA0},
{0xB8, 0x00},
{0xB9, 0x01},
{0xBD, 0x02},
{0xC9, 0x54},
{0xCF, 0xBB},
{0xD2, 0x22},
{0xD3, 0x3E},
{0xD4, 0xA4},
{0xD6, 0x07},
{0xD7, 0x0A},
{0xD8, 0x07},
{0xDC, 0x30},
{0xDD, 0x52},
{0xE0, 0x42},
{0xE2, 0xE4},
{0xE4, 0x00},
{0xF0, 0xDC},
{0xF1, 0x16},
{0xF2, 0x1F},
{0xF3, 0x0F},
{0xF5, 0x95},
{0xF6, 0x05},
{0xF7, 0x00},
{0xF8, 0x48},
{0xFA, 0x25},
{0x09, 0x01},
{0xEF, 0x02},
{0x2E, 0x0a},//1f
{0x33, 0x8a},//9f
{0x36, 0x00},
{0x4e, 0x02},
{0x4F, 0x0A},
{0x50, 0x0A},
{0xCB, 0x9B},
{0xD4, 0x00},
{0xED, 0x01},
{0xEF, 0x05},
{0x06, 0x05},//raw12
{0x09, 0x09},
{0x0A, 0x05},
{0x0F, 0x00},
{0x10, 0x02},
{0x11, 0x01},
{0x15, 0x07},
{0x17, 0x03},
{0x18, 0x01},
{0x3B, 0x01},
{0x40, 0x19},
{0x41, 0x15},
{0x43, 0x02},
{0x44, 0x01},
{0x4A, 0x02},
{0x4F, 0x01},
{0x94, 0x04},
{0x99, 0x01},
{0x9A, 0x01},
{0xB1, 0x00},
{0xED, 0x01},
{0xEF, 0x06},
{0x00, 0x08},
{0x02, 0x93},
{0x08, 0x20},
{0x09, 0xB0},
{0x0A, 0x78},
{0x0B, 0xD0},
{0x0C, 0x60},
{0x0D, 0x21},
{0x0E, 0x30},
{0x0F, 0x00},
{0x10, 0xFA},
{0x11, 0x00},
{0x12, 0xFA},
{0x17, 0x01},
{0x18, 0x2C},
{0x19, 0x01},
{0x1A, 0x2C},
{0x2B, 0x02},
{0x2D, 0x04},
{0x4A, 0x54},
{0x4B, 0x97},
{0x98, 0x40},
{0x99, 0x06},
{0x9A, 0x60},
{0x9B, 0x09},
{0x9C, 0xF0},
{0x9D, 0x0A},
{0x9F, 0x19},
//{0xA1, 0x02},   //16bit HDR, mode 0
{0xA1, 0x04},
{0xD7, 0x32},
{0xD8, 0x02},
{0xDA, 0x02},
{0xDC, 0x02},
{0xDE, 0x02},
{0xE4, 0x02},
{0xE6, 0x06},
{0xE9, 0x10},
{0xEB, 0x04},
{0xED, 0x01},
{0xEF, 0x07},
{0x01, 0x33},
{0x02, 0x33},
{0x03, 0x33},
{0x04, 0x44},
{0x05, 0x55},
{0x06, 0x05},
{0x10, 0xC9},
{0x12, 0x85},
{0x15, 0x10},
{0x65, 0x0C},
{0x67, 0x85},
{0x70, 0x12},
{0xED, 0x01},
{0xEF, 0x00},
{0x11, 0x00},
{0xEF, 0x05},
{0x3B, 0x00},
{0xED, 0x01},
{0xEF, 0x01},
{0x02, 0xFB},
{0x09, 0x01},
{0xEF, 0x05},
{0x0F, 0x01},
{0xED, 0x01},


};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
{0xEF, 0x01},//M0F0
{0x1b, 0x07},//bit7,Hflip
//{0x90, 0x04},//bit[7:4] Cmd_ADC_Latency
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


I2C_ARRAY mirr_flip_table[] =
{
    {0xEF, 0x01},//M0F0
    {0x1b, 0x07},//bit7,Hflip
    //{0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x04},//bit7,Vflip

    {0xEF, 0x01},//M1F0
    {0x1b, 0x87},//bit7,Hflip
    //{0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x04},//bit7,Vflip

    {0xEF, 0x01},//M0F1
    {0x1b, 0x07},//bit7,Hflip
    //{0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x84},//bit7,Vflip

    {0xEF, 0x01},//M1F1
    {0x1b, 0x87},//bit7,Hflip
    //{0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x84},//bit7,Vflip
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static I2C_ARRAY gain_reg[] = {
    {0xEF, 0x01},
    {0x83, 0x04},//analog gain suggest from 1.25X to 32X
    {0x18, 0x00},
    {0x09, 0x01},
};

static Gain_ARRAY gain_table[]={
    {1024  ,0},
    {1088  ,1},
    {1152  ,2},
    {1216  ,3},
    {1280  ,4},
    {1344  ,5},
    {1408  ,6},
    {1472  ,7},
    {1536  ,8},
    {1600  ,9},
    {1664  ,10},
    {1728  ,11},
    {1792  ,12},
    {1856  ,13},
    {1920  ,14},
    {1982  ,15},
    {2048  ,16},
    {2176  ,17},
    {2304  ,18},
    {2432  ,19},
    {2560  ,20},
    {2688  ,21},
    {2816  ,22},
    {2944  ,23},
    {3072  ,24},
    {3200  ,25},
    {3328  ,26},
    {3456  ,27},
    {3584  ,28},
    {3712  ,29},
    {3840  ,30},
    {3968  ,31},
    {4096  ,32},
    {4352  ,33},
    {4608  ,34},
    {4864  ,35},
    {5120  ,36},
    {5376  ,37},
    {5623  ,38},
    {5888  ,39},
    {6144  ,40},
    {6400  ,41},
    {6656  ,42},
    {6912  ,43},
    {7168  ,44},
    {7424  ,45},
    {7680  ,46},
    {7936  ,47},
    {8192  ,48},
    {8704  ,49},
    {9216  ,50},
    {9728  ,51},
    {10240 ,52},
    {10752 ,53},
    {11264 ,54},
    {11776 ,55},
    {12288 ,56},
    {12800 ,57},
    {13312 ,58},
    {13824 ,59},
    {14336 ,60},
    {14848 ,61},
    {15360 ,62},
    {15872 ,63},
    {16384 ,64},
    {17408 ,65},
    {18432 ,66},
    {19456 ,67},
    {20480 ,68},
    {21504 ,69},
    {22528 ,70},
    {23552 ,71},
    {24576 ,72},
    {25600 ,73},
    {26624 ,74},
    {27648 ,75},
    {28672 ,76},
    {29696 ,77},
    {30720 ,78},
    {31744 ,79},
    {32768 ,80},
    {34816 ,81},
    {36864 ,82},
    {38912 ,83},
    {40960 ,84},
    {43008 ,85},
    {45056 ,86},
    {47104 ,87},
    {49152 ,88},
    {51200 ,89},
    {53248 ,90},
    {55286 ,91},
    {57344 ,92},
    {59392 ,93},
    {61440 ,94},
    {63488 ,95},
    {65536 ,96},
    {69632 ,97},
    {73728 ,98},
    {77824 ,99},
    {81920 ,100},
    {86016 ,101},
    {90112 ,102},
    {94208 ,103},
    {98304 ,104},
    {102400 ,105},
    {106496 ,106},
    {110592 ,107},
    {114688 ,108},
    {118784 ,109},
    {122880 ,110},
    {126976 ,111},
    {131072 ,112},
};

I2C_ARRAY expo_reg[] = {

    {0xEF, 0x01},
    {0x0C, 0x00},
    {0x0D, 0x02},
    {0x0E, 0x00},
    {0x0F, 0x00},
    {0x5e, 0x00},
    {0x5f, 0x00},
    {0x09, 0x01},
};

I2C_ARRAY vts_reg[] = {
    {0xEF, 0x01},
    {0x0A, 0x04},
    {0x0B, 0x64},
    {0x09, 0x01},
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
#define SENSOR_NAME ps5280


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
    SENSOR_DMSG("[%s]ps5280 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ps5280_SetPatternMode(ms_cus_sensor *handle,u32 mode)
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
        SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

        for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
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
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
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

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //return SUCCESS;
    //sensor_if->SetSkipFrame(2); //skip 2 frame to avoid bad frame after mirror/flip

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
         //  handle->bayer_id=  CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
    //  handle->bayer_id= CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
     // handle->bayer_id= CUS_BAYER_GR;
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
    }
    //SensorReg_Write(0xef,0x01);
    SensorReg_Write(0x09,1);

   // params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ps5280_params *params = (ps5280_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[1].data << 8) | (vts_reg[2].data << 0);

    if (params->expo.fps >= 3000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0,lines=0,ne =0, ne_patch = 0, g_sns_const = 0;
/*     u16 a=0,b=0;
    u32 line_read =0; */
    ps5280_params *params = (ps5280_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);
    if(fps>=3 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30+(fps>>1))/fps)-1;
    }else if(fps>=3000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30000+(fps>>1))/fps)-1;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > params->expo.vts -4){
        vts = params->expo.line + 4;
    }else{
        vts = params->expo.vts;
    }
    lines = vts - params->expo.line;
    g_sns_const=0xfd2;
    ne_patch=50+((38*lines)>>8);
    if (ne_patch > 250){
        ne_patch = 250;
    }
    ne = g_sns_const - ne_patch;

    vts_reg[1].data = (vts >> 8) & 0x00ff;
    vts_reg[2].data = (vts >> 0) & 0x00ff;
    expo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
    expo_reg[2].data =(u16)( (lines>>0) & 0x00ff);
    expo_reg[3].data = (u16)((ne >>8) & 0x00ff);
    expo_reg[4].data = (u16)((ne >>0) & 0x00ff);
    expo_reg[5].data = (u16)((ne_patch >>8) & 0x00ff);
    expo_reg[6].data = (u16)((ne_patch >>0) & 0x00ff);

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
     ps5280_params *params = (ps5280_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    u16 a=0,b=0;
    u32 line_read =0,line_write =0;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
            ++fps_delay;
            //printf("[%s] frame %ld\n", __FUNCTION__,framecount);
            SensorReg_Read(0xc,&a);
            SensorReg_Read(0xd,&b);
            line_read |= (u32)(a&0xff)<<8;
            line_read |= (u32)(b&0xff)<<0;
            line_write|= (u32)(expo_reg[1].data&0xff)<<8;
            line_write|= (u32)(expo_reg[2].data&0xff)<<0;
            if(line_read > params->expo.vts && line_read != line_write){
                fps_delay=0;
            }

            if(params->dirty && fps_delay > 1)
            {
                SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
                params->dirty = false;
            }else if(params->dirty && fps_delay <1)
            {
                SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
            }else if(params->dirty && fps_delay == 1)
            {
                SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
                SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
                params->dirty = false;
            }

            break;
            default :
            break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    ps5280_params *params = (ps5280_params *)handle->private_data;
    u32 lines = 0;

    lines |= (u32)(expo_reg[1].data&0xff)<<8;
    lines |= (u32)(expo_reg[2].data&0xff)<<0;

    lines=params->expo.vts-lines;

    *us = lines*Preview_line_period/1000;
    //printf("====================================================\n");
    //printf("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);
    //printf("====================================================\n");

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0, ne =0, ne_patch = 0, g_sns_const = 0;
    ps5280_params *params = (ps5280_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if(lines < 2) lines = 2;
    if (lines > params->expo.vts-4) {
        vts = lines +4;
    }
    else
      vts=params->expo.vts;

    params->expo.line = lines;

    //printf("====================================================\n");
    //printf("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,us,lines, params->expo.vts);
    //printf("====================================================\n");
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    lines=vts-lines;

    g_sns_const=0xfd2;
    ne_patch=50+((38*lines)>>8);
    if (ne_patch > 250){
        ne_patch = 250;
    }
    ne = g_sns_const - ne_patch;

    expo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
    expo_reg[2].data =(u16)( (lines>>0) & 0x00ff);
    expo_reg[3].data = (u16)((ne >>8) & 0x00ff);
    expo_reg[4].data = (u16)((ne >>0) & 0x00ff);
    expo_reg[5].data = (u16)((ne_patch >>8) & 0x00ff);
    expo_reg[6].data = (u16)((ne_patch >>0) & 0x00ff);

    vts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
    vts_reg[2].data = (u16)((vts >> 0) & 0x00ff);

    //SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
    //SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
    //SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
  ps5280_params *params = (ps5280_params *)handle->private_data;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,gain_reg[1].data);
   // printf("set gain/reg=%d/0x%x\n", gain,gain_reg[1].data);

   return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    ps5280_params *params = (ps5280_params *)handle->private_data;
    u32 i;
    u8 sens = 1;

    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    //TODO : sens=0 has 4X gain
    //printf("[%s] set total_gain=%d\n", __FUNCTION__,gain);
    if(gain < 5*1024)
    {
      sens = 1;     // LS
    }
    //else if(gain > 6*1024)
    else
    {
      sens = 0;     // HS
    }

    for(i = 0;i < ARRAY_SIZE(gain_table);i++)
    {
        if(gain < gain_table[i].total_gain)    break;
    }

    gain_reg[1].data = sens ?i:(i-32);
    gain_reg[2].data = sens;
    //SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
    //printf("[%s] gain  gain_reg/reg=%d/0x%x\n", __FUNCTION__, gain,gain_reg[1].data);
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[0].data,gain_reg[1].data);

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

static int ps5280_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 1;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    ps5280_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(ps5280_params));
    params = (ps5280_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ps5280_MIPI");

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
    strcpy(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

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
    handle->pCus_sensor_SetPatternMode = ps5280_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = ps5280_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.line=1000;
    params->expo.fps = 30;
    params->dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  PS5280,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ps5280_params
                         );

