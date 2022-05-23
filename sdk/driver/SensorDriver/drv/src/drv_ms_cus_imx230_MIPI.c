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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(IMX230_HDR);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
//#define SENSOR_MIPI_HDR_MODE (1) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

//#undef SENSOR_DBG
//#define SENSOR_DBG 0

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
//#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9098     //CFG
#define SENSOR_BAYERID      CUS_BAYER_RG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_RG//CUS_BAYER_GR
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define SENSOR_MAX_GAIN     350                  // max sensor again, a-gain * conversion-gain*d-gain//51db
//#define SENSOR_MAX_GAIN     22.5                 // max sensor again, a-gain * conversion-gain*d-gain*expand gain//63db
#define SENSOR_MAX_GAIN     (177 * 1024)
#define SENSOR_MIN_GAIN      (1 * 1024)
//#define lane_number 2
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_27MHZ
#define Preview_line_period  7222//13341                  ////HTS/PCLK=1600 pixels/78MHZ=20.513usec
#define Preview_line_period_HDR_DOL 14814

//#define Prv_Max_line_number 4620                    //maximum exposure line munber of sensor when preview
#define vts_30fps 4620//                      //for 25 fps
#define vts_30fps_HDR_DOL   (2250)
#define Preview_WIDTH       3840//3072                    //resolution Width when preview
#define Preview_HEIGHT      2160//2160                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS

/*
 * 5344x3016 SME-HDR 30fps
 * Scaling factor = N/M (N=16 fixed value)
 * if M = 17 --> 5029x2838 (W>4672) NG
 * if M = 18 --> 4750x2680 (W>4672) NG
 * if M = 19 --> 4500x2538 11.4 M
 * if M = 20 --> 4276x2412 10.3 M
 * if M = 21 --> 4072x2296  9.3 M
 * if M = 22 --> 3844x2192  8.5 M
 * if M = 23 --> 3716x2090  7.7 M
 */
#define SNR_SCALE_16M_2_11D4M		(0) // 4500x2538
#define SNR_SCALE_16M_2_10D3M		(0) // 4276x2412
#define SNR_SCALE_16M_2_9D3M		(0) // 4072x2296
#define SNR_SCALE_16M_2_8D5M		(1) // 3884x2192

#if SNR_SCALE_16M_2_11D4M
#define VIF_WIDTH     (4500)
#define VIF_HEIGHT    (2538)
#elif SNR_SCALE_16M_2_10D3M
#define VIF_WIDTH     (4276)
#define VIF_HEIGHT    (2412)
#elif SNR_SCALE_16M_2_9D3M
#define VIF_WIDTH     (4072)
#define VIF_HEIGHT    (2296)
#elif SNR_SCALE_16M_2_8D5M
#define VIF_WIDTH     (3840) //(3884)
#define VIF_HEIGHT    (2160) //(2192)
#else
#define VIF_WIDTH     (3840) //(4276)
#define VIF_HEIGHT    (2160) //(2412)
#endif

#define SENSOR_I2C_ADDR    0x20 //0x34                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG// if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_NEG        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;

#define Preview_CROP_START_X    0                       //CROP_START_X
#define Preview_CROP_START_Y    0                       //CROP_START_Y

#define RTNZERO    0

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
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

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    int max_rhs1;
    bool dirty;
    I2C_ARRAY tGain_reg[3];
    CUS_GAIN_GAP_ARRAY tGain_gap_compensate[16];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tExpo_shr_dol1_reg[2];
    I2C_ARRAY tExpo_rhs1_reg[2];
    I2C_ARRAY tExpo_shr_dol2_reg[2];
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tPatternTbl[2];
    //I2C_ARRAY *tCurrent_Mirror_Flip_Tbl;
    CUS_CAMSENSOR_ORIT cur_orien;

} imx230_params;
// set sensor ID address and data,

const I2C_ARRAY Sensor_id_table[] =
{
    // TBD
};

const I2C_ARRAY Sensor_init_table_HDR_DOL_4lane[] =
{
    { 0xFFFF , 0x01 },
};

const I2C_ARRAY Sensor_init_table_4lane[] =
{
    // Data rate: 1400Mbps/lane
    // 5344x3016 SME-HDR 30fps

    // External Clock Setting
    { 0x0136 , 0x18 },  //  EXCK_FREQ
    { 0x0137 , 0x00 },  //  EXCK_FREQ

    // Global Setting
    { 0x4800 , 0x0E },
    { 0x4890 , 0x01 },
    { 0x4D1E , 0x01 },
    { 0x4D1F , 0xFF },
    { 0x4FA0 , 0x00 },
    { 0x4FA1 , 0x00 },
    { 0x4FA2 , 0x00 },
    { 0x4FA3 , 0x83 },
    { 0x6153 , 0x01 },
    { 0x6156 , 0x01 },
    { 0x69BB , 0x01 },
    { 0x69BC , 0x05 },
    { 0x69BD , 0x05 },
    { 0x69C1 , 0x00 },
    { 0x69C4 , 0x01 },
    { 0x69C6 , 0x01 },
    { 0x7300 , 0x00 },
    { 0x9009 , 0x1A },
    { 0xB040 , 0x90 },
    { 0xB041 , 0x14 },
    { 0xB042 , 0x6B },
    { 0xB043 , 0x43 },
    { 0xB044 , 0x63 },
    { 0xB045 , 0x2A },
    { 0xB046 , 0x68 },
    { 0xB047 , 0x06 },
    { 0xB048 , 0x68 },
    { 0xB049 , 0x07 },
    { 0xB04A , 0x68 },
    { 0xB04B , 0x04 },
    { 0xB04C , 0x68 },
    { 0xB04D , 0x05 },
    { 0xB04E , 0x68 },
    { 0xB04F , 0x16 },
    { 0xB050 , 0x68 },
    { 0xB051 , 0x17 },
    { 0xB052 , 0x68 },
    { 0xB053 , 0x74 },
    { 0xB054 , 0x68 },
    { 0xB055 , 0x75 },
    { 0xB056 , 0x68 },
    { 0xB057 , 0x76 },
    { 0xB058 , 0x68 },
    { 0xB059 , 0x77 },
    { 0xB05A , 0x68 },
    { 0xB05B , 0x7A },
    { 0xB05C , 0x68 },
    { 0xB05D , 0x7B },
    { 0xB05E , 0x68 },
    { 0xB05F , 0x0A },
    { 0xB060 , 0x68 },
    { 0xB061 , 0x0B },
    { 0xB062 , 0x68 },
    { 0xB063 , 0x08 },
    { 0xB064 , 0x68 },
    { 0xB065 , 0x09 },
    { 0xB066 , 0x68 },
    { 0xB067 , 0x0E },
    { 0xB068 , 0x68 },
    { 0xB069 , 0x0F },
    { 0xB06A , 0x68 },
    { 0xB06B , 0x0C },
    { 0xB06C , 0x68 },
    { 0xB06D , 0x0D },
    { 0xB06E , 0x68 },
    { 0xB06F , 0x13 },
    { 0xB070 , 0x68 },
    { 0xB071 , 0x12 },
    { 0xB072 , 0x90 },
    { 0xB073 , 0x0E },
    { 0xD000 , 0xDA },
    { 0xD001 , 0xDA },
    { 0xD002 , 0xAF },
    { 0xD003 , 0xE1 },
    { 0xD004 , 0x55 },
    { 0xD005 , 0x34 },
    { 0xD006 , 0x21 },
    { 0xD007 , 0x00 },
    { 0xD008 , 0x1C },
    { 0xD009 , 0x80 },
    { 0xD00A , 0xFE },
    { 0xD00B , 0xC5 },
    { 0xD00C , 0x55 },
    { 0xD00D , 0xDC },
    { 0xD00E , 0xB6 },
    { 0xD00F , 0x00 },
    { 0xD010 , 0x31 },
    { 0xD011 , 0x02 },
    { 0xD012 , 0x4A },
    { 0xD013 , 0x0E },
    { 0xD014 , 0x55 },
    { 0xD015 , 0xF0 },
    { 0xD016 , 0x1B },
    { 0xD017 , 0x00 },
    { 0xD018 , 0xFA },
    { 0xD019 , 0x2C },
    { 0xD01A , 0xF1 },
    { 0xD01B , 0x7E },
    { 0xD01C , 0x55 },
    { 0xD01D , 0x1C },
    { 0xD01E , 0xD8 },
    { 0xD01F , 0x00 },
    { 0xD020 , 0x76 },
    { 0xD021 , 0xC1 },
    { 0xD022 , 0xBF },
    { 0xD044 , 0x40 },
    { 0xD045 , 0xBA },
    { 0xD046 , 0x70 },
    { 0xD047 , 0x47 },
    { 0xD048 , 0xC0 },
    { 0xD049 , 0xBA },
    { 0xD04A , 0x70 },
    { 0xD04B , 0x47 },
    { 0xD04C , 0x82 },
    { 0xD04D , 0xF6 },
    { 0xD04E , 0xDA },
    { 0xD04F , 0xFA },
    { 0xD050 , 0x00 },
    { 0xD051 , 0xF0 },
    { 0xD052 , 0x02 },
    { 0xD053 , 0xF8 },
    { 0xD054 , 0x81 },
    { 0xD055 , 0xF6 },
    { 0xD056 , 0xCE },
    { 0xD057 , 0xFD },
    { 0xD058 , 0x10 },
    { 0xD059 , 0xB5 },
    { 0xD05A , 0x0D },
    { 0xD05B , 0x48 },
    { 0xD05C , 0x40 },
    { 0xD05D , 0x7A },
    { 0xD05E , 0x01 },
    { 0xD05F , 0x28 },
    { 0xD060 , 0x15 },
    { 0xD061 , 0xD1 },
    { 0xD062 , 0x0C },
    { 0xD063 , 0x49 },
    { 0xD064 , 0x0C },
    { 0xD065 , 0x46 },
    { 0xD066 , 0x40 },
    { 0xD067 , 0x3C },
    { 0xD068 , 0x48 },
    { 0xD069 , 0x8A },
    { 0xD06A , 0x62 },
    { 0xD06B , 0x8A },
    { 0xD06C , 0x80 },
    { 0xD06D , 0x1A },
    { 0xD06E , 0x8A },
    { 0xD06F , 0x89 },
    { 0xD070 , 0x00 },
    { 0xD071 , 0xB2 },
    { 0xD072 , 0x10 },
    { 0xD073 , 0x18 },
    { 0xD074 , 0x0A },
    { 0xD075 , 0x46 },
    { 0xD076 , 0x20 },
    { 0xD077 , 0x32 },
    { 0xD078 , 0x12 },
    { 0xD079 , 0x88 },
    { 0xD07A , 0x90 },
    { 0xD07B , 0x42 },
    { 0xD07C , 0x00 },
    { 0xD07D , 0xDA },
    { 0xD07E , 0x10 },
    { 0xD07F , 0x46 },
    { 0xD080 , 0x80 },
    { 0xD081 , 0xB2 },
    { 0xD082 , 0x88 },
    { 0xD083 , 0x81 },
    { 0xD084 , 0x84 },
    { 0xD085 , 0xF6 },
    { 0xD086 , 0x06 },
    { 0xD087 , 0xF8 },
    { 0xD088 , 0xE0 },
    { 0xD089 , 0x67 },
    { 0xD08A , 0x85 },
    { 0xD08B , 0xF6 },
    { 0xD08C , 0x4B },
    { 0xD08D , 0xFC },
    { 0xD08E , 0x10 },
    { 0xD08F , 0xBD },
    { 0xD090 , 0x00 },
    { 0xD091 , 0x18 },
    { 0xD092 , 0x1E },
    { 0xD093 , 0x78 },
    { 0xD094 , 0x00 },
    { 0xD095 , 0x18 },
    { 0xD096 , 0x17 },
    { 0xD097 , 0x98 },

    // Load Setting
    { 0x5869 , 0x01 },  //  Please set in the last of Global Setting.

    // 5344x3016 SME-HDR 30fps
    // Mode Setting
    { 0x0114 , 0x03 },  //  CSI_LANE_MODE
    { 0x0220 , 0x03 },  //  HDR_MODE
    { 0x0221 , 0x11 },  //  [7:4]HDR_RESO_REDU_H[3:0]HDR_RESO_REDU_V
    { 0x0222 , 0x10 },  //  EXPO_RATIO
    { 0x0340 , 0x0C },  //  FRM_LENGTH_LINES
    { 0x0341 , 0x34 },  //  FRM_LENGTH_LINES
    { 0x0342 , 0x19 },  //  LINE_LENGTH_PCK
    { 0x0343 , 0x00 },  //  LINE_LENGTH_PCK
    { 0x0344 , 0x00 },  //  X_ADD_STA
    { 0x0345 , 0x00 },  //  X_ADD_STA
    { 0x0346 , 0x01 },  //  Y_ADD_STA
    { 0x0347 , 0xF4 },  //  Y_ADD_STA
    { 0x0348 , 0x14 },  //  X_ADD_END 0xEFF = 3839 //0x10B3 = 4275 //0x14DF = 5343
    { 0x0349 , 0xDF },  //  X_ADD_END
    { 0x034A , 0x0D },  //  Y_ADD_END 0x86F = 2159 //0x096B = 2411 //0xDBB = 3515
    { 0x034B , 0xBB },  //  Y_ADD_END
    { 0x0381 , 0x01 },  //  X_EVN_INC
    { 0x0383 , 0x01 },  //  X_ODD_INC
    { 0x0385 , 0x01 },  //  Y_EVN_INC
    { 0x0387 , 0x01 },  //  Y_ODD_INC
    { 0x0900 , 0x00 },  //  BINNING_MODE
    { 0x0901 , 0x11 },  //  [7:4]BINNING_TYPE_H [3:0]BINNING_TYPE_V
    { 0x0902 , 0x00 },  //  BINNING_WEIGHTING
    { 0x3000 , 0x75 },  //  HDR_OUTPUT_CTRL
    { 0x3001 , 0x00 },  //  HDR_OUTPUT_CTRL2
    { 0x305C , 0x11 },

    // Output Size Setting
    { 0x0112 , 0x0A },  //  CSI_DT_FMT_H
    { 0x0113 , 0x0A },  //  CSI_DT_FMT_L

#if SNR_SCALE_16M_2_11D4M
    { 0x034C , 0x11 }, // X_OUT_SIZE 0x1194 = 4500
    { 0x034D , 0x94 }, // X_OUT_SIZE
    { 0x034E , 0x09 }, // Y_OUT_SIZE 0x9EA = 2538
    { 0x034F , 0xEA }, // Y_OUT_SIZE
    { 0x0401 , 0x02 }, // SCALE_MODE
    { 0x0404 , 0x00 }, // SCALE_M 0x13 = 19
    { 0x0405 , 0x13 }, // SCALE_M
#elif SNR_SCALE_16M_2_10D3M
    { 0x034C , 0x10 }, // X_OUT_SIZE 0x10B4 = 4276
    { 0x034D , 0xB4 }, // X_OUT_SIZE
    { 0x034E , 0x09 }, // Y_OUT_SIZE 0x96c = 2412
    { 0x034F , 0x6C }, // Y_OUT_SIZE
    { 0x0401 , 0x02 }, // SCALE_MODE
    { 0x0404 , 0x00 }, // SCALE_M 0x13 = 19
    { 0x0405 , 0x14 }, // SCALE_M
#elif SNR_SCALE_16M_2_9D3M
    { 0x034C , 0x0F }, // X_OUT_SIZE 0xFE8 = 4072
    { 0x034D , 0xE8 }, // X_OUT_SIZE
    { 0x034E , 0x08 }, // Y_OUT_SIZE 0x8F8 = 2296
    { 0x034F , 0xF8 }, // Y_OUT_SIZE
    { 0x0401 , 0x02 }, // SCALE_MODE
    { 0x0404 , 0x00 }, // SCALE_M 0x13 = 19
    { 0x0405 , 0x15 }, // SCALE_M
#elif SNR_SCALE_16M_2_8D5M
    { 0x034C , 0x0F }, // X_OUT_SIZE 0xF2C = 3884
    { 0x034D , 0x2C }, // X_OUT_SIZE
    { 0x034E , 0x08 }, // Y_OUT_SIZE 0x890 = 2192
    { 0x034F , 0x90 }, // Y_OUT_SIZE
    { 0x0401 , 0x02 }, // SCALE_MODE
    { 0x0404 , 0x00 }, // SCALE_M 0x16 = 22
    { 0x0405 , 0x16 }, // SCALE_M
#else
    { 0x034C , 0x14 },  //  X_OUT_SIZE 0xF00 = 3840 //0x10B4 = 4276 //0x14E0 = 5344
    { 0x034D , 0xE0 },  //  X_OUT_SIZE
    { 0x034E , 0x0B },  //  Y_OUT_SIZE 0x870 = 2160 //0x096C = 2412 //0xBC8 = 3016
    { 0x034F , 0xC8 },  //  Y_OUT_SIZE
    { 0x0401 , 0x00 },  //  SCALE_MODE
    { 0x0404 , 0x00 },  //  SCALE_M
    { 0x0405 , 0x10 },  //  SCALE_M
#endif

    { 0x0408 , 0x00 },  //  DIG_CROP_X_OFFSET
    { 0x0409 , 0x00 },  //  DIG_CROP_X_OFFSET
    { 0x040A , 0x00 },  //  DIG_CROP_Y_OFFSET
    { 0x040B , 0x00 },  //  DIG_CROP_Y_OFFSET
    { 0x040C , 0x14 },  //  DIG_CROP_IMAGE_WIDTH  0xF00 = 3840 //0x10B4 = 4276 //0x14E0 = 5344
    { 0x040D , 0xE0 },  //  DIG_CROP_IMAGE_WIDTH
    { 0x040E , 0x0B },  //  DIG_CROP_IMAGE_HEIGHT 0x870 = 2160 //0x096C = 2412 //0xBC8 = 3016
    { 0x040F , 0xC8 },  //  DIG_CROP_IMAGE_HEIGHT

    // Clock Setting
    { 0x0301 , 0x04 },  //  VTPXCK_DIV
    { 0x0303 , 0x02 },  //  VTSYCK_DIV
    { 0x0305 , 0x04 },  //  PREPLLCK_VT_DIV
    { 0x0306 , 0x00 },  //  PLL_VT_MPY
    { 0x0307 , 0x90/*0xC8*/ },  //  PLL_VT_MPY
    { 0x0309 , 0x0A },  //  OPPXCK_DIV
    { 0x030B , 0x01 },  //  OPSYCK_DIV
    { 0x030D , 0x0F },  //  PREPLLCK_OP_DIV
    { 0x030E , 0x03 },  //  PLL_OP_MPY
    { 0x030F , 0x6B },  //  PLL_OP_MPY
    { 0x0310 , 0x01 },  //  PLL_MULT_DRIV

    // Data Rate Setting
    { 0x0820 , 0x15 },  //  REQ_LINK_BIT_RATE_MBPS[31:24]
    { 0x0821 , 0xE0 },  //  REQ_LINK_BIT_RATE_MBPS[23:16]
    { 0x0822 , 0x00 },  //  REQ_LINK_BIT_RATE_MBPS[15:8]
    { 0x0823 , 0x00 },  //  REQ_LINK_BIT_RATE_MBPS[7:0]

    // Integration Time Setting
    { 0x0202 , 0x0C },  //  COARSE_INTEG_TIME
    { 0x0203 , 0x2A },  //  COARSE_INTEG_TIME
    { 0x0224 , 0x00 },  //  ST_COARSE_INTEG_TIME
    { 0x0225 , 0xC2 },  //  ST_COARSE_INTEG_TIME

    // Gain Setting
    { 0x0204 , 0x00 },  //  ANA_GAIN_GLOBAL
    { 0x0205 , 0x00 },  //  ANA_GAIN_GLOBAL
    { 0x0216 , 0x00 },  //  ST_ANA_GAIN_GLOBAL
    { 0x0217 , 0x00 },  //  ST_ANA_GAIN_GLOBAL
    { 0x020E , 0x01 },  //  DIG_GAIN_GR
    { 0x020F , 0x00 },  //  DIG_GAIN_GR
    { 0x0210 , 0x01 },  //  DIG_GAIN_R
    { 0x0211 , 0x00 },  //  DIG_GAIN_R
    { 0x0212 , 0x01 },  //  DIG_GAIN_B
    { 0x0213 , 0x00 },  //  DIG_GAIN_B
    { 0x0214 , 0x01 },  //  DIG_GAIN_GB
    { 0x0215 , 0x00 },  //  DIG_GAIN_GB

    // HDR Setting
    { 0x3006 , 0x01 },
    { 0x3007 , 0x01 },
    { 0x31E0 , 0x3F },
    { 0x31E1 , 0xFF },
    { 0x31E4 , 0x00 },

    // DPC2D Setting
    { 0x3A22 , 0x00 },
    { 0x3A23 , 0x14 },
    { 0x3A24 , 0xE0 },
    { 0x3A25 , 0x0B },
    { 0x3A26 , 0xC8 },
    { 0x3A2F , 0x00 },
    { 0x3A30 , 0x00 },
    { 0x3A31 , 0x01 },
    { 0x3A32 , 0xF4 },
    { 0x3A33 , 0x14 },
    { 0x3A34 , 0xDF },
    { 0x3A35 , 0x0D },
    { 0x3A36 , 0xBB },
    { 0x3A37 , 0x00 },
    { 0x3A38 , 0x00 },
    { 0x3A39 , 0x00 },

    // LSC Setting
    { 0x3A21 , 0x02 },

    // Stats Setting
    { 0x3011 , 0x00 },  //  NML_STATS_CAL_EN
    { 0x3013 , 0x01 },  //  STATS_OUT_EN

    // After XCLR release, please set to 0x0100=0x01 after waiting for 10.6ms
    // { 0xFFFF , 0x0B },

    // Streaming setting
    { 0x0100 , 0x01 },

    // Software Standby settings
    // { 0x0100 , 0x00 },
    // { 0x0100 , 0x01 },

};

const I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
    {0x301a, 0x00},//M0F0
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
    {0x0101, 0x00},//M0F0
    {0x0101, 0x01},//M1F0
    {0x0101, 0x02},//M0F1
    {0x0101, 0x03},//M1F1
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const I2C_ARRAY gain_reg[] = {
    {0x300b, 0x00},//high 10~8bit, analog gain
    {0x300a, 0x10},//low byte
    {0x3012, 0x10},//low bit 0~2
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
    {0x300d, 0x00},//MSB
    {0x300c, 0x00},
};

const I2C_ARRAY expo_shr_dol1_reg[] =
{ //SEL
    {0x302F, 0x00},//MSB
    {0x302E, 0x04},
};

const I2C_ARRAY expo_rhs1_reg[] =
{ //SEL
    {0x3033, 0x00},//MSB
    {0x3032, 0x08},
};

const I2C_ARRAY expo_shr_dol2_reg[] =
{ //LEF
    {0x3031, 0x05},//MSB
    {0x3030, 0x43},
};

const I2C_ARRAY vts_reg[] = {
    {0x30fa, 0x00},//bit0-3-->MSB
    {0x30f9, 0x15},
    {0x30f8, 0xf9},
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
    {0x303c, 0x11},//test pattern
    {0x303d, 0x0a},
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
#define SENSOR_NAME imx230
*/
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

int cus_camsensor_release_handle(ms_cus_sensor *handle);
int cus_camsensor_release_handle_dol_lef(ms_cus_sensor *handle);

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

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(500);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(500);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(500);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(1000);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(500);

    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(500);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    sensor_if->MCLK(idx, 0, handle->mclk);

    return SUCCESS;
}

/////////////////// image function /////////////////////////
static int pCus_CheckSonyProductID(ms_cus_sensor *handle)
{
    u16 sen_data0;
    u16 sen_data1;

    /* Read Product ID */
    if (SensorReg_Read(0x3912, &sen_data0)) {
        return FAIL;
    }
    if (SensorReg_Read(0x3913, &sen_data1)) {
        return FAIL;
    }

    if (((sen_data0 & 0xFF) != 0x17) || ((sen_data1 & 0xF) != 0x3)) {
        pr_err("[***ERROR***]Check Product ID Fail: 0x%x, 0x%x\n", sen_data0, sen_data1);
        return FAIL;
    }

    return SUCCESS;
}

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
      *id = id_from_sensor[i].data;
    }


    SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int imx230_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;

    imx230_params *params = (imx230_params *)handle->private_data;


  switch(mode)
  {
  case 1:
	  params->tPatternTbl[0].data |= 0x11; //enable
  break;
  case 0:
	  params->tPatternTbl[0].data &= 0x00; //disable
  break;
  default:
	  params->tPatternTbl[0].data &= 0x00; //disable
  break;
  }

  for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
  {
    if(SensorReg_Write(params->tPatternTbl[i].reg,params->tPatternTbl[i].data) != SUCCESS)
    {
      //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
      return FAIL;
    }
  }

  return SUCCESS;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);

static int pCus_init_mipi4lane_linear(ms_cus_sensor *handle){
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    imx230_params *params = (imx230_params *)handle->private_data;
    int i,cnt;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane);i++)
    {
        if(Sensor_init_table_4lane[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane[i].reg,Sensor_init_table_4lane[i].data) != SUCCESS)
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

    pCus_SetOrien(handle, params->cur_orien);

    if (pCus_CheckSonyProductID(handle)) {
        return FAIL;
    }
    params->tVts_reg[0].data = (params->expo.vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;

    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}

static int pCus_init_mipi4lane_HDR_DOL(ms_cus_sensor *handle)
{
    imx230_params *params = (imx230_params *)handle->private_data;
    int i,cnt=0;

    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR_DOL_4lane);i++)
    {
        if(Sensor_init_table_HDR_DOL_4lane[i].reg==0xffff)
        {
            //MsSleep(RTK_MS_TO_TICK(1));//usleep(1000*Sensor_init_table_HDR_DOL_4lane[i].data);
            SENSOR_MSLEEP(Sensor_init_table_4lane[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("reg =  %x, data = %x\n", Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data);
            while(SensorReg_Write(Sensor_init_table_HDR_DOL_4lane[i].reg,Sensor_init_table_HDR_DOL_4lane[i].data) != SUCCESS)
            {
                cnt++;
                 SENSOR_DMSG("Sensor_init_table_HDR_DOL_4lane -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //printf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //SensorReg_Read( Sensor_init_table_HDR_DOL_4lane[i].reg, &sen_data );
            //UartSendTrace("IMX230 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data, sen_data);
        }
    }

    params->tVts_reg[0].data = (params->expo.vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;

    //MSG("[%s:%d]Sensor init success!!\n", __FUNCTION__, __LINE__);
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
        case 0: //"3840x2160@15fps"
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_mipi4lane_linear;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_mipi4lane_HDR_DOL;
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    short sen_data = 0;

    SensorReg_Read(0x301a, &sen_data);//always success now

    SENSOR_DMSG("mirror:%x\r\n", sen_data & 0x01);
    switch(sen_data & 0x01)
    {
        case 0x00:
            *orit = CUS_ORIT_M0F0;
        break;
        case 0x01:
            *orit = CUS_ORIT_M0F1;
        break;
        default:
            *orit = CUS_ORIT_M0F0;
        break;
    }
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length=table_length/4;
    int i,j;

    imx230_params *params = (imx230_params *)handle->private_data;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M0F0;
            }
	       //  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M1F0;
            }
		//  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M0F1;
            }
		 // handle->bayer_id=	CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M1F1;
            }
		 // handle->bayer_id=	CUS_BAYER_GR;
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->cur_orien = CUS_ORIT_M0F0;
            }
		//  handle->bayer_id=	CUS_BAYER_BG;
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    imx230_params *params = (imx230_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    //imx230_params *params = (imx230_params *)handle->private_data;
    //u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    //u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    // FIXME
    return RTNZERO;
#if 0
    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps)/fps;
        vts_reg[0].data = (params->expo.vts>> 16) & 0x000f;
        vts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true; //reg need to update = true;
        return SUCCESS;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000))/fps;
        vts_reg[0].data = (params->expo.vts>> 16) & 0x000f;
        vts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        vts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->dirty = true; //reg need to update = true;
        return SUCCESS;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        //SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
#endif
}

static int pCus_GetFPS_HDR_DOL_SEF(ms_cus_sensor *handle)
{
    imx230_params *params = (imx230_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 10000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF(ms_cus_sensor *handle, u32 fps)
{
    //imx230_params *params = (imx230_params *)handle->private_data;
    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=3 && fps <= 30){
      return SUCCESS;
    }else if(fps>=3000 && fps <= 30000){
        return SUCCESS;
    }else{
      //params->expo.vts=vts_30fps;
      //params->expo.fps=30;
      SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
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
    imx230_params *params = (imx230_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty)
            {
                SensorReg_Write(0x302d,1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                SensorReg_Write(0x302d,0);
                params->dirty = false;
                //SENSOR_DMSG("**** notify : (0x%x,%x),(0x%x,%x)****\n",params->tGain_reg[0].reg,params->tGain_reg[0].data,params->tGain_reg[1].reg,params->tGain_reg[1].data);
            }else{

                //SENSOR_DMSG("**** notify : (0x%x,%x),(0x%x,%x)****\n",params->tGain_reg[0].reg,params->tGain_reg[0].data,params->tGain_reg[1].reg,params->tGain_reg[1].data);
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}


static int pCus_AEStatusNotifyHDR_DOL_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    imx230_params *params = (imx230_params *)handle->private_data;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty)
            {
                SensorReg_Write(0x302d, 1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_shr_dol2_reg, ARRAY_SIZE(expo_shr_dol2_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_shr_dol1_reg, ARRAY_SIZE(expo_shr_dol1_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_rhs1_reg, ARRAY_SIZE(expo_rhs1_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorReg_Write(0x302d, 0);
                params->dirty = false;
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_AEStatusNotifyHDR_DOL_SEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
   // imx230_params *params = (imx230_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x3001,0);
             break;
        case CUS_FRAME_ACTIVE:
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_SetAEUSecsHDR_DOL_LEF(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0, shr_dol2 = 0;
    imx230_params *params = (imx230_params *)handle->private_data;

    // us = 1000000/30;
    //lines = us/Preview_line_period_HDR_DOL;
    lines = (1000 * us) / Preview_line_period_HDR_DOL;
    if (lines > params->expo.vts - 4) {
        vts = lines + 4;
    }
    else{
      vts = params->expo.vts;
    }

   // lines=us/Preview_line_period_HDR_DOL;
    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    shr_dol2 = vts - lines;

    params->tExpo_shr_dol2_reg[0].data = (shr_dol2 >> 8) & 0x00ff;
    params->tExpo_shr_dol2_reg[1].data = (shr_dol2 >> 0) & 0x00ff;

    params->tVts_reg[0].data = (vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (vts >> 0) & 0x00ff;

    //SensorReg_Write(0x3001,1);
    //SensorRegArrayW((I2C_ARRAY*)expo_SHS2_reg, ARRAY_SIZE(expo_SHS2_reg));
    //SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
    //SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
    //UartSendTrace("[LEF, us:%d, lines:%d, ", us, lines);
    //UartSendTrace("vts:%d, shs2:%d]\n", vts, shs2);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    u32 rhs1 = 0, shr_dol1 = 0, shr_dol2 = 0;
    imx230_params *params = (imx230_params *)handle->private_data;

    // us = 1000000/30;
    //lines = us/Preview_line_period_HDR_DOL;
    lines = (1000 * us) / Preview_line_period_HDR_DOL;
    vts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    shr_dol2 = ((params->tExpo_shr_dol2_reg[0].data << 8) | (params->tExpo_shr_dol2_reg[1].data << 0));

    params->max_rhs1 = vts - 2198;

    //rhs1 = ((expo_rhs1_reg[0].data << 8) | (expo_rhs1_reg[1].data << 0));
    rhs1 = params->max_rhs1;

    if((rhs1 - 6) < lines){
        shr_dol1 = 6;
    }
    else if((rhs1 <= params->max_rhs1) && (rhs1 <= shr_dol2 - 6)){
        shr_dol1 = rhs1 - lines;
        if((shr_dol1 < 6) || (shr_dol1 > (rhs1 - 2))){ //Check boundary
            //shs1 = 0;
            //UartSendTrace("\n\n[SEF NG1]\n");
        }
    }
    else{
        //UartSendTrace("\n\n[SEF NG2]\n");
    }

    //UartSendTrace("[SEF, us:%d, lines:%d, ", us, lines);
    //UartSendTrace("vts:%d, shs2:%d, ", vts, shs2);
    //UartSendTrace("rhs1:%d, shs1:%d]\n", rhs1, shs1);

    params->tExpo_shr_dol1_reg[0].data = (shr_dol1 >> 8) & 0x00ff;
    params->tExpo_shr_dol1_reg[1].data = (shr_dol1 >> 0) & 0x00ff;

    params->tExpo_rhs1_reg[0].data = (rhs1 >> 8) & 0x00ff;
    params->tExpo_rhs1_reg[1].data = (rhs1 >> 0) & 0x00ff;

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
#if 0
    u32 lines = 0;
    //int rc = SensorRegArrayR((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
    imx230_params *params = (imx230_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<0;
   // lines >>= 4;
   // *us = (lines+dummy) * params->expo.us_per_line;
    //*us = lines;//(lines*Preview_line_period);
    *us = lines*Preview_line_period/1000;
    SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, lines, *us);
    //return rc;
    return SUCCESS;
#else
    // FIXME
    return RTNZERO;
#endif
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
#if 0
    imx230_params *params = (imx230_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if (lines >params->expo.vts-12) {
        vts = lines +12;
    }
    else
        vts=params->expo.vts;

#if 0
    // lines=us/Preview_line_period;
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
    us,
    lines,
    params->expo.vts
    );
#endif
    lines=vts-lines;

    params->tExpo_reg[0].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[1].data =(u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[0].data =(u16)( (vts >> 16) & 0x000f);
    params->tVts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((vts >> 0) & 0x00ff);

    //SensorReg_Write(0x302d,1);
    //SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
    //SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
    //SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
    params->dirty = true;
    return SUCCESS;
#endif
    // FIXME
    return RTNZERO;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
 imx230_params *params = (imx230_params *)handle->private_data;
    //u16  temp_gain;
    //double  temp_gain_double;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] get gain/regL/regH (1024=1X)= %d/0x%x/0x%x\n", __FUNCTION__, *gain,gain_reg[0].data,gain_reg[1].data);

    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {
    imx230_params *params = (imx230_params *)handle->private_data;
    u16 gain16;
    //double gain_double;

    params->expo.final_gain = gain;
    if(gain<SENSOR_MIN_GAIN)
        gain=SENSOR_MIN_GAIN;
    else if(gain>=SENSOR_MAX_GAIN)
        gain=SENSOR_MAX_GAIN;

    gain16=(u16)(2048-((2048*1024)/gain));
    //gain16=(u16)(gain_double*10);

    params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
    params->tGain_reg[1].data=gain16&0xff;//LSB

    SENSOR_DMSG("[%s] set gain/reg0/reg1=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
    return SUCCESS;

}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    imx230_params *params = (imx230_params *)handle->private_data;
    u32 i;
    //double gain_double;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u16 gain16;

    params->expo.final_gain = gain;
    if(gain<SENSOR_MIN_GAIN)
        gain=SENSOR_MIN_GAIN;
    else if(gain>=SENSOR_MAX_GAIN)
        gain=SENSOR_MAX_GAIN;

    //gain_double=(double)gain;
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
#if 0
    gain16=(u16)(2048-((2048*1024)/gain));
    //gain16=(u16)(gain_double*10);
    gain_reg[0].data=(gain16>>8)&0x07;//MSB
    gain_reg[1].data=gain16&0xff;//LSB
#else

    if(gain<=23040){
        gain16=(u16)(2048-((2048*1024)/gain));
        params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
        params->tGain_reg[1].data=gain16&0xff;//LSB
        params->tGain_reg[2].data=0x00;
    }   else if((gain>23040)&&(gain<=46080)){
        gain=gain>>1;
        gain16=(u16)(2048-((2048*1024)/gain));
        params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
        params->tGain_reg[1].data=gain16&0xff;//LSB
        params->tGain_reg[2].data=0x01;
    }    else if((gain>46080)&&(gain<=92160)){
        gain=gain>>2;
        gain16=(u16)(2048-((2048*1024)/gain));
        params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
        params->tGain_reg[1].data=gain16&0xff;//LSB
        params->tGain_reg[2].data=0x02;
    }    else {//if((gain>22.5*1024*4)&&(gain<=22.5*1024*8)){
        gain=gain>>3;
        gain16=(u16)(2048-((2048*1024)/gain));
        params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
        params->tGain_reg[1].data=gain16&0xff;//LSB
        params->tGain_reg[2].data=0x03;
    }
    params->dirty = true;
#endif

    SENSOR_DMSG("[%s] set gain/reg0/reg1=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
   // printf("set gain/reg0/reg1=%d/0x%x/0x%x\n", gain,gain_reg[0].data,gain_reg[1].data);
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000/30;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int pCus_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
#if 0
    info->max = 1000000000/12;
    info->min =  Preview_line_period*4;
    info->step = Preview_line_period;
    return SUCCESS;
#else
    // FIXME
     return RTNZERO;
#endif
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
    imx230_params *params = (imx230_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 10000)
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR_DOL*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_hdr_dol_lef(ms_cus_sensor *handle, u32 fps)
{
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period_HDR_DOL * 4);
    info->step = Preview_line_period_HDR_DOL;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    imx230_params *params = (imx230_params *)handle->private_data;
    info->max = Preview_line_period_HDR_DOL * params->max_rhs1;
    info->min = (Preview_line_period_HDR_DOL * 2);
    info->step = Preview_line_period_HDR_DOL;
    return SUCCESS;
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
    u32 i, j;
    imx230_params *params = (imx230_params *)handle->private_data;

    for(i=0,j=0;i< num ;i++,j+=2){
        params->tGain_gap_compensate[i].gain=pArray[i].gain;
        params->tGain_gap_compensate[i].offset=pArray[i].offset;
    }

    SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}

imx230_params *g_cur_init = NULL;

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx230_params *params;

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
    ////////////////////////////////////
    //    sensor private data         //
    ////////////////////////////////////
    params = (imx230_params *)handle->private_data;
    memcpy(params->tGain_reg,gain_reg,sizeof(gain_reg));
    memcpy(params->tGain_gap_compensate,gain_gap_compensate,sizeof(gain_gap_compensate));
    memcpy(params->tExpo_reg,expo_reg,sizeof(expo_reg));
    memcpy(params->tExpo_shr_dol1_reg,expo_shr_dol1_reg,sizeof(expo_shr_dol1_reg));
    memcpy(params->tExpo_rhs1_reg,expo_rhs1_reg,sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol2_reg,expo_shr_dol2_reg,sizeof(expo_shr_dol2_reg));
    memcpy(params->tVts_reg,vts_reg,sizeof(vts_reg));
    memcpy(params->tPatternTbl,PatternTbl,sizeof(PatternTbl));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX230_MIPI");

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
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = VIF_WIDTH;  // FIXME // Preview_WIDTH  // stVifPortInfoInfo.u32RectWidth
    handle->video_res_supported.res[0].height = VIF_HEIGHT; // FIXME // Preview_HEIGHT // stVifPortInfoInfo.u32RectHeight
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= VIF_WIDTH;  // FIXME //0xF18; // stRes.stOutputSize.u16Width
    handle->video_res_supported.res[0].nOutputHeight= VIF_HEIGHT; // FIXME //0x87E; // stRes.stOutputSize.u16Height
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@30fps");

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
    handle->ae_gain_delay       = 2;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    ///calibration
    handle->sat_mingain=SENSOR_MIN_GAIN;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_mipi4lane_linear;
    //handle->pCus_sensor_powerupseq  = pCus_powerupseq   ;
    handle->pCus_sensor_poweron     = pCus_poweron;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID   ;

    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes   ;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes   ;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien      ;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien      ;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS      ;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS      ;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = imx230_SetPatternMode;
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
    handle->pCus_sensor_SetAEGain_cal       = pCus_SetAEGain_cal;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

     //sensor calibration
   // handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx230_params *params = NULL;

    cus_camsensor_init_handle_linear(drv_handle);
    params = (imx230_params *)handle->private_data;

    sprintf(handle->model_id,"IMX230_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;
    handle->RGBIR_id    = SENSOR_RGBIRID;

    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_SONY_DOL;

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
    handle->video_res_supported.res[0].nOutputWidth= 0xF18;
    handle->video_res_supported.res[0].nOutputHeight= 0x881;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@15fps_HDR");

    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->mclk                        = Preview_MCLK_SPEED_HDR_DOL;

#if (SENSOR_MIPI_LANE_NUM == 2)
#endif
#if (SENSOR_MIPI_LANE_NUM == 4)
    handle->pCus_sensor_init        = pCus_init_mipi4lane_HDR_DOL;
#endif
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_DOL_SEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_SEF; //TBD

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_sef;
    params->expo.vts = vts_30fps_HDR_DOL;

    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx230_params *params;
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
    ////////////////////////////////////
    //    sensor private data         //
    ////////////////////////////////////
    params = (imx230_params *)handle->private_data;
    memcpy(params->tGain_reg,gain_reg,sizeof(gain_reg));
    memcpy(params->tGain_gap_compensate,gain_gap_compensate,sizeof(gain_gap_compensate));
    memcpy(params->tExpo_reg,expo_reg,sizeof(expo_reg));
    memcpy(params->tExpo_shr_dol1_reg,expo_shr_dol1_reg,sizeof(expo_shr_dol1_reg));
    memcpy(params->tExpo_rhs1_reg,expo_rhs1_reg,sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol2_reg,expo_shr_dol2_reg,sizeof(expo_shr_dol2_reg));
    memcpy(params->tVts_reg,vts_reg,sizeof(vts_reg));
    memcpy(params->tPatternTbl,PatternTbl,sizeof(PatternTbl));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX230_MIPI_HDR_LEF");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    //handle->data_fmt    = SENSOR_DATAFMT;   //CUS_DATAFMT_YUV;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID_HDR_DOL;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_SONY_DOL;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num =  0; //Long frame

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
    handle->video_res_supported.res[0].nOutputWidth= 0xF18;
    handle->video_res_supported.res[0].nOutputHeight= 0x8AA;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@15fps_HDR");

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
    handle->ae_gain_delay       = 1;//0;//1;
    handle->ae_shutter_delay    = 2;//1;//2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=SENSOR_MIN_GAIN;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    //handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_release     = cus_camsensor_release_handle_dol_lef;
    handle->pCus_sensor_init        = pCus_init_hdr_dol_lef;
    //handle->pCus_sensor_powerupseq  = pCus_powerupseq   ;
    handle->pCus_sensor_poweron     = pCus_poweron_hdr_dol_lef;
    handle->pCus_sensor_poweroff    = pCus_poweroff_hdr_dol_lef;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID_hdr_dol_lef;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
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
#if 0//defined(__MV5_FPGA__)
    handle->pCus_sensor_I2CWrite = pCus_I2CWrite_hdr_dol_lef; //Andy Liu
    handle->pCus_sensor_I2CRead = pCus_I2CRead_hdr_dol_lef; //Andy Liu
#endif
    params->expo.vts = vts_30fps_HDR_DOL;
    params->expo.fps = 29;
    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

int cus_camsensor_release_handle_dol_lef(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  IMX230_HDR,
                            cus_camsensor_init_handle_linear,
                            cus_camsensor_init_handle_hdr_dol_sef,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            imx230_params
                         );
