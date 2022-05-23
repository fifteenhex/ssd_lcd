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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(OS02C10);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (2)
#define SENSOR_MIPI_LANE_NUM_HDR (2)
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
//                                                                                                    ��//
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAPREC_HDR     CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID_HDR      CUS_BAYER_BG//CUS_BAYER_GR
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     248                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_MCLK_SPEED_HDR  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 25960//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
#define Preview_line_period_HDR 26666//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
#define vts_30fps  1284//1266//1150//1090                              //for 29.091fps @ MCLK=36MHz
#define vts_30fps_HDR  1250//1266//1150//1090                              //for 29.091fps @ MCLK=36MHz
#define Preview_WIDTH       1920//2688                    //resolution Width when preview
#define Preview_HEIGHT      1080//1520                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MAX_FPS_HDR 30                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x6c                   //I2C slave address
#define SENSOR_I2C_SPEED   240000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

#define hcg_hcg 1

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif
static int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
CUS_MCLK_FREQ UseParaMclk(void);

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
        u32 max_short;
        u32 line;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    u32 skip_cnt;
    bool dirty;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[5];
    I2C_ARRAY tGain_reg_HDR_SEF[5];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tExpo_reg_HDR_SEF[2];
    I2C_ARRAY tMirror_reg[1];
    I2C_ARRAY tMirror_reg_HDR[1];
    bool ori_dirty;
} OS02C10_params;
// set sensor ID address and data,

/* typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;
 */
const I2C_ARRAY Sensor_id_table[] =
{
    {0x300a, 0x53},      // {address of ID, ID },
    {0x300b, 0x05},      // {address of ID, ID },
    //{0x300c, 0x41},      // {address of ID, ID },
    // max 8 sets in this table
};

const I2C_ARRAY Sensor_init_table[] =
{
//OS02C10_EA_1920x1080_2exp_HL_30fps_10b_mipi864_2L - 27Mclk
    {0x302a , 0x00},
    {0x0103 , 0x01},
    {0xffff , 0x0a},
    {0x0109 , 0x01},
    {0x0104 , 0x02},
    {0x0102 , 0x00},
    {0x0305 , 0x5c},
    {0x0306 , 0x00},
    {0x0307 , 0x00},
    {0x0317 , 0x09},
    {0x0323 , 0x07},
    {0x0324 , 0x01},
    {0x0325 , 0x80},
    {0x0327 , 0x07},
    {0x032d , 0x02},
    {0x300f , 0x11},
    {0x3012 , 0x21},
    {0x3026 , 0x10},
    {0x3027 , 0x08},
    {0x302d , 0x24},
    {0x3103 , 0x29},
    {0x3106 , 0x11},
    {0x3400 , 0x00},
    {0x3406 , 0x08},
    {0x3408 , 0x05},
    {0x340c , 0x05},
    {0x3425 , 0x51},
    {0x3426 , 0x10},
    {0x3427 , 0x14},
    {0x3428 , 0x10},
    {0x3429 , 0x10},
    {0x342a , 0x10},
    {0x342b , 0x04},
    {0x3504 , 0x08},
    {0x3508 , 0x01},
    {0x3509 , 0x00},
    {0x3544 , 0x08},
    {0x3548 , 0x01},
    {0x3549 , 0x00},
    {0x3584 , 0x08},
    {0x3588 , 0x01},
    {0x3589 , 0x00},
    {0x3601 , 0x70},
    {0x3604 , 0xe3},
    {0x3605 , 0x7f},
    {0x3606 , 0x00},
    {0x3608 , 0xa8},
    {0x360a , 0xd0},
    {0x360b , 0x08},
    {0x360e , 0xc8},
    {0x360f , 0x66},
    {0x3610 , 0x81},
    {0x3611 , 0x89},
    {0x3612 , 0x4e},
    {0x3613 , 0xbd},
    {0x362a , 0x0e},
    {0x362b , 0x0e},
    {0x362c , 0x0e},
    {0x362d , 0x0e},
    {0x362e , 0x0c},
    {0x362f , 0x1a},
    {0x3630 , 0x32},
    {0x3631 , 0x64},
    {0x3638 , 0x00},
    {0x3643 , 0x00},
    {0x3644 , 0x00},
    {0x3645 , 0x00},
    {0x3646 , 0x00},
    {0x3647 , 0x00},
    {0x3648 , 0x00},
    {0x3649 , 0x00},
    {0x364a , 0x04},
    {0x364c , 0x0e},
    {0x364d , 0x0e},
    {0x364e , 0x0e},
    {0x364f , 0x0e},
    {0x3650 , 0xff},
    {0x3651 , 0xff},
    {0x3661 , 0x07},
    {0x3662 , 0x02},
    {0x3663 , 0x20},
    {0x3665 , 0x12},
    {0x3667 , 0xd4},
    {0x3668 , 0x80},
    {0x366f , 0x00},
    {0x3671 , 0x08},
    {0x3673 , 0x2a},
    {0x3681 , 0x80},
    {0x3700 , 0x26},
    {0x3701 , 0x1e},
    {0x3702 , 0x25},
    {0x3703 , 0x28},
    {0x3706 , 0x3e},
    {0x3707 , 0x0a},
    {0x3708 , 0x36},
    {0x3709 , 0x55},
    {0x370a , 0x00},
    {0x370b , 0xa3},
    {0x3714 , 0x01},
    {0x371b , 0x16},
    {0x371c , 0x00},
    {0x371d , 0x08},
    {0x3756 , 0x9b},
    {0x3757 , 0x9b},
    {0x3762 , 0x1d},
    {0x376c , 0x00},
    {0x3776 , 0x05},
    {0x3777 , 0x22},
    {0x3779 , 0x60},
    {0x377c , 0x48},
    {0x379c , 0x4d},
    {0x3784 , 0x06},
    {0x3785 , 0x0a},
    {0x37d1 , 0x3e},
    {0x37d2 , 0x00},
    {0x37d3 , 0xa3},
    {0x37d5 , 0x3e},
    {0x37d6 , 0x00},
    {0x37d7 , 0xa3},
    {0x37d8 , 0x01},
    {0x37dc , 0x00},
    {0x3790 , 0x10},
    {0x3793 , 0x04},
    {0x3794 , 0x07},
    {0x3796 , 0x00},
    {0x3797 , 0x02},
    {0x37a1 , 0x80},
    {0x37bb , 0x88},
    {0x37be , 0x01},
    {0x37bf , 0x00},
    {0x37c0 , 0x01},
    {0x37c7 , 0x56},
    {0x37ca , 0x21},
    {0x37cc , 0x13},
    {0x37cd , 0x90},
    {0x37cf , 0x02},
    {0x3800 , 0x00},
    {0x3801 , 0x00},
    {0x3802 , 0x00},
    {0x3803 , 0x04},
    {0x3804 , 0x07},
    {0x3805 , 0x8f},
    {0x3806 , 0x04},
    {0x3807 , 0x43},
    {0x3808 , 0x07},
    {0x3809 , 0x80},
    {0x380a , 0x04},
    {0x380b , 0x38},
    {0x380c , 0x04},
    {0x380d , 0x38},
    {0x380e , 0x04},
    {0x380f , 0xe2},
    {0x3811 , 0x08},
    {0x3813 , 0x04},
    {0x3814 , 0x01},
    {0x3815 , 0x01},
    {0x3816 , 0x01},
    {0x3817 , 0x01},
    {0x381c , 0x00},
    {0x3820 , 0x02},
    {0x3821 , 0x00},
    {0x3822 , 0x14},
    {0x384c , 0x04},
    {0x384d , 0x38},
    {0x3858 , 0x0d},
    {0x3865 , 0x00},
    {0x3866 , 0xc0},
    {0x3867 , 0x00},
    {0x3868 , 0xc0},
    {0x3900 , 0x13},
    {0x3940 , 0x13},
    {0x3980 , 0x13},
    {0x3c01 , 0x11},
    {0x3c05 , 0x00},
    {0x3c0f , 0x1c},
    {0x3c12 , 0x0d},
    {0x3c19 , 0x01},
    {0x3c21 , 0x40},
    {0x3c3b , 0x18},
    {0x3c3d , 0xc9},
    {0x3c55 , 0xcb},
    {0x3c5d , 0xcf},
    {0x3c5e , 0xcf},
    {0x3ce0 , 0x00},
    {0x3ce1 , 0x00},
    {0x3ce2 , 0x00},
    {0x3ce3 , 0x00},
    {0x3d8c , 0x70},
    {0x3d8d , 0x10},
    {0x4001 , 0x2f},
    {0x4033 , 0x80},
    {0x4008 , 0x02},
    {0x4009 , 0x11},
    {0x4004 , 0x00},
    {0x4005 , 0x40},
    {0x400a , 0x01},
    {0x400b , 0x3c},
    {0x410f , 0x01},
    {0x402e , 0x00},
    {0x402f , 0x40},
    {0x4030 , 0x00},
    {0x4031 , 0x40},
    {0x4032 , 0x0f},
    {0x4050 , 0x00},
    {0x4051 , 0x07},
    {0x4288 , 0xcf},
    {0x4289 , 0x03},
    {0x428a , 0x46},
    {0x430b , 0x0f},
    {0x430c , 0xfc},
    {0x430d , 0x00},
    {0x430e , 0x00},
    {0x4500 , 0x18},
    {0x4501 , 0x18},
    {0x4504 , 0x00},
    {0x4507 , 0x02},
    {0x4508 , 0x1a},
    {0x4603 , 0x00},
    {0x4640 , 0x62},
    {0x4646 , 0xaa},
    {0x4647 , 0x55},
    {0x4648 , 0x99},
    {0x4649 , 0x66},
    {0x464d , 0x00},
    {0x4654 , 0x11},
    {0x4655 , 0x22},
    {0x4800 , 0x04},
    {0x4810 , 0xff},
    {0x4811 , 0xff},
    {0x480e , 0x00},
    {0x4813 , 0x00},
    {0x4837 , 0x0e},
    {0x484b , 0x27},
    {0x4d00 , 0x4e},
    {0x4d01 , 0x0c},
    {0x4d09 , 0x4f},
    {0x5000 , 0x1f},
    {0x5080 , 0x00},
    {0x50c0 , 0x00},
    {0x5100 , 0x00},
    {0x5200 , 0x00},
    {0x5201 , 0x70},
    {0x5202 , 0x03},
    {0x5203 , 0x7f},
    {0x5780 , 0x53},
    {0x5786 , 0x01},
    {0x3501 , 0x02},
    {0x0305 , 0x3a},
    {0x4837 , 0x15},
    {0x032f , 0x00},
    {0x032e , 0x01},
    {0x032d , 0x06},
    {0x3106 , 0x12},
    {0x380c , 0x0a},
    {0x380d , 0xf0},
    {0x380e , 0x05},
    {0x380f , 0x05},
    {0x3501 , 0x02},
    {0x3502 , 0xC2},
    {0x0100 , 0x01},
    {0x376c , 0x10},
    {0x3c55 , 0x08},
    {0x376c , 0x00},
    {0x3c55 , 0xcb},
};

const I2C_ARRAY Sensor_init_table_HDR[] =
{
//OS02C10_EA_1920x1080_2exp_HL_30fps_10b_mipi864_2L - 27Mclk
    {0x302a , 0x00},
    {0x0103 , 0x01},
    {0x0109 , 0x01},
    {0x0104 , 0x02},
    {0x0102 , 0x01},
    {0x0305 , 0x40},
    {0x0306 , 0x00},
    {0x0307 , 0x00},
    {0x0317 , 0x09},
    {0x0323 , 0x07},
    {0x0324 , 0x01},
    {0x0325 , 0x80},
    {0x0327 , 0x07},
    {0x032d , 0x02},
    {0x300f , 0x11},
    {0x3012 , 0x21},
    {0x3026 , 0x00},
    {0x3027 , 0x00},
    {0x302d , 0x24},
    {0x3103 , 0x29},
    {0x3106 , 0x11},
    {0x3400 , 0x00},
    {0x3406 , 0x08},
    {0x3408 , 0x01},
    {0x340c , 0x10},
    {0x3425 , 0x00},
    {0x3426 , 0x00},
    {0x3427 , 0x00},
    {0x3428 , 0x00},
    {0x3429 , 0x00},
    {0x342a , 0x00},
    {0x342b , 0x00},
    {0x3501 , 0x04},//long exp[15,8]
    {0x3502 , 0x65},//long exp[7,0]
    {0x3504 , 0x08},
    {0x3508 , 0x01},
    {0x3509 , 0x00},
    {0x3544 , 0x08},
    {0x3548 , 0x01},
    {0x3549 , 0x00},
    {0x3584 , 0x08},
    {0x3588 , 0x01},
    {0x3589 , 0x00},
    {0x3601 , 0x70},
    {0x3604 , 0xe3},
    {0x3605 , 0x7f},
    {0x3606 , 0x00},
    {0x3608 , 0xa8},
    {0x360a , 0xd0},
    {0x360b , 0x08},
    {0x360e , 0xc8},
    {0x360f , 0x66},
    {0x3610 , 0x81},
    {0x3611 , 0x89},
    {0x3612 , 0x4e},
    {0x3613 , 0xbd},
    {0x362a , 0x0e},
    {0x362b , 0x0e},
    {0x362c , 0x0e},
    {0x362d , 0x0e},
    {0x362e , 0x0c},
    {0x362f , 0x1a},
    {0x3630 , 0x32},
    {0x3631 , 0x64},
    {0x3638 , 0x00},
    {0x3643 , 0x00},
    {0x3644 , 0x00},
    {0x3645 , 0x00},
    {0x3646 , 0x00},
    {0x3647 , 0x00},
    {0x3648 , 0x00},
    {0x3649 , 0x00},
    {0x364a , 0x04},
    {0x364c , 0x0e},
    {0x364d , 0x0e},
    {0x364e , 0x0e},
    {0x364f , 0x0e},
    {0x3650 , 0xff},
    {0x3651 , 0xff},
    {0x3661 , 0x07},
    {0x3662 , 0x02},
    {0x3663 , 0x20},
    {0x3665 , 0x12},
    {0x3667 , 0xd4},
    {0x3668 , 0x80},
    {0x366f , 0x00},
    {0x3670 , 0xc7},
    {0x3671 , 0x09},
    {0x3673 , 0x2a},
    {0x3681 , 0x80},
    {0x3700 , 0x26},
    {0x3701 , 0x1e},
    {0x3702 , 0x25},
    {0x3703 , 0x28},
    {0x3706 , 0x3e},
    {0x3707 , 0x0a},
    {0x3708 , 0x36},
    {0x3709 , 0x55},
    {0x370a , 0x00},
    {0x370b , 0xa3},
    {0x3714 , 0x01},
    {0x371b , 0x16},
    {0x371c , 0x00},
    {0x371d , 0x08},
    {0x3756 , 0x9b},
    {0x3757 , 0x9b},
    {0x3762 , 0x1d},
    {0x376c , 0x00},
    {0x3776 , 0x05},
    {0x3777 , 0x22},
    {0x3779 , 0x60},
    {0x377c , 0x48},
    {0x379c , 0x4d},
    {0x3784 , 0x06},
    {0x3785 , 0x0a},
    {0x3790 , 0x10},
    {0x3793 , 0x04},
    {0x3794 , 0x07},
    {0x3796 , 0x00},
    {0x3797 , 0x02},
    {0x37a1 , 0x80},
    {0x37bb , 0x88},
    {0x37be , 0x01},
    {0x37bf , 0x00},
    {0x37c0 , 0x01},
    {0x37c7 , 0x56},
    {0x37ca , 0x21},
    {0x37cc , 0x13},
    {0x37cd , 0x90},
    {0x37cf , 0x02},
    {0x37d1 , 0x3e},
    {0x37d2 , 0x00},
    {0x37d3 , 0xa3},
    {0x37d5 , 0x3e},
    {0x37d6 , 0x00},
    {0x37d7 , 0xa3},
    {0x37d8 , 0x01},
    {0x37da , 0x00},
    {0x37db , 0x00},
    {0x37dc , 0x00},
    {0x37dd , 0x00},
    {0x3800 , 0x00},
    {0x3801 , 0x00},
    {0x3802 , 0x00},
    {0x3803 , 0x04},
    {0x3804 , 0x07},
    {0x3805 , 0x8f},
    {0x3806 , 0x04},
    {0x3807 , 0x43},
    {0x3808 , 0x07},
    {0x3809 , 0x80},
    {0x380a , 0x04},
    {0x380b , 0x38},
    {0x380c , 0x04},
    {0x380d , 0x38},
    {0x380e , 0x04},
    {0x380f , 0xe2},
    {0x3811 , 0x08},
    {0x3813 , 0x04},
    {0x3814 , 0x01},
    {0x3815 , 0x01},
    {0x3816 , 0x01},
    {0x3817 , 0x01},
    {0x381c , 0x08},
    {0x3820 , 0x03},
    {0x3821 , 0x00},
    {0x3822 , 0x14},
    {0x384c , 0x04},
    {0x384d , 0x38},
    {0x3858 , 0x01},
    {0x3865 , 0x00},
    {0x3866 , 0xc0},
    {0x3867 , 0x00},
    {0x3868 , 0xc0},
    {0x3900 , 0x13},
    {0x3940 , 0x13},
    {0x3980 , 0x13},
    {0x3c01 , 0x11},
    {0x3c05 , 0x00},
    {0x3c0f , 0x1c},
    {0x3c12 , 0x0d},
    {0x3c19 , 0x01},
    {0x3c21 , 0x40},
    {0x3c3b , 0x18},
    {0x3c3d , 0xc9},
    {0x3c55 , 0xcb},
    {0x3c5d , 0xcf},
    {0x3c5e , 0xcf},
    {0x3ce0 , 0x00},
    {0x3ce1 , 0x00},
    {0x3ce2 , 0x00},
    {0x3ce3 , 0x00},
    {0x3d8c , 0x70},
    {0x3d8d , 0x10},
    {0x4001 , 0xef},
    {0x4033 , 0x80},
    {0x4008 , 0x02},
    {0x4009 , 0x11},
    {0x4004 , 0x00},
    {0x4005 , 0x40},
    {0x400a , 0x01},
    {0x400b , 0x3c},
    {0x4011 , 0xbb},
    {0x410f , 0x01},
    {0x402e , 0x00},
    {0x402f , 0x40},
    {0x4030 , 0x00},
    {0x4031 , 0x40},
    {0x4032 , 0x0f},
    {0x4050 , 0x00},
    {0x4051 , 0x07},
    {0x4288 , 0xce},
    {0x4289 , 0x03},
    {0x428a , 0x46},
    {0x430b , 0x0f},
    {0x430c , 0xfc},
    {0x430d , 0x00},
    {0x430e , 0x00},
    {0x4314 , 0x04},
    {0x4500 , 0x18},
    {0x4501 , 0x18},
    {0x4504 , 0x00},
    {0x4507 , 0x03},
    {0x4508 , 0x1a},
    {0x4603 , 0x00},
    {0x4640 , 0x62},
    {0x4646 , 0xaa},
    {0x4647 , 0x55},
    {0x4648 , 0x99},
    {0x4649 , 0x66},
    {0x464d , 0x00},
    {0x4654 , 0x11},
    {0x4655 , 0x22},
    {0x4800 , 0x04},
    {0x4810 , 0xff},
    {0x4811 , 0xff},
    {0x480e , 0x04},
    {0x4813 , 0x84},
    {0x4837 , 0x0d},
    {0x484b , 0x67},
    {0x4d00 , 0x4e},
    {0x4d01 , 0x0c},
    {0x4d09 , 0x4f},
    {0x5000 , 0x1f},
    {0x5080 , 0x00},
    {0x50c0 , 0x00},
    {0x5100 , 0x00},
    {0x5200 , 0x00},
    {0x5201 , 0x70},
    {0x5202 , 0x03},
    {0x5203 , 0x7f},
    {0x5780 , 0x53},
    {0x5786 , 0x01},
    {0x376c , 0x20},
    {0x032f , 0x00},
    {0x032e , 0x01},
    {0x032d , 0x05},
    {0x3106 , 0x12},
    {0x380c , 0x05}, //07
    {0x380d , 0xa0}, //d0
    {0x384c , 0x05},
    {0x384d , 0xa0},
    {0x380e , 0x04},
    {0x380f , 0xe2},
#if hcg_hcg
    {0x3c3c , 0x0f},
#else
    {0x3c3c , 0x0b},//hcg_lcg 03;lcg_hcg 07
#endif
    {0x0100 , 0x01},
};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

static I2C_ARRAY mirror_reg[] =
{
    {0x3820, 0x02},//M0F0 [3:2]flip [1]mirror
};

static I2C_ARRAY mirror_reg_HDR[] =
{
    {0x3820, 0x03},//M0F0 [3:2]flip [1]mirror [0] hdr
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static int g_sensor_ae_min_gain = 1024;//1280;

const I2C_ARRAY gain_reg[] = {
    {0x3508, 0x10},//long a-gain[13:8]
    {0x3509, 0x00},//long a-gain[7:0]
    {0x350A, 0x10},// d-gain[13:8]
    {0x350B, 0x00},// d-gain[7:0]
    {0x350C, 0x00},// d-gain[7:0]
};

const I2C_ARRAY gain_reg_HDR_SEF[] = {
    {0x3548, 0x10},//short a-gain[13:8]
    {0x3549, 0x00},//short a-gain[7:0]
    {0x354A, 0x10},// d-gain[13:8]
    {0x354B, 0x00},// d-gain[7:0]
    {0x354C, 0x00},// d-gain[7:0]
};

const I2C_ARRAY expo_reg[] = {
    {0x3501, 0x02},//long exp[15,8]
    {0x3502, 0x00},//long exp[7,0]
};

const I2C_ARRAY expo_reg_HDR_SEF[] = {
    {0x3541, 0x00},//short
    {0x3542, 0x20},
};

const I2C_ARRAY vts_reg[] = {
    {0x380E, 0x04},
    {0x380F, 0xe2},
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
#define SENSOR_NAME OS02C10


#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus,&(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus,&(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

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
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 2);
    }
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

    //CamOsPrintf("pCus_poweron = %d us \n",timeGetTimeU()-TStart);
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
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
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_DCG) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    sensor_if->MCLK(idx, 0, handle->mclk);

//    params->cur_orien = CUS_ORIT_M0F0;

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

    //SensorReg_Write(0xfe,0x00);

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
    SENSOR_DMSG("[%s]OS02C10 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int OS02C10_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

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
    //pCus_SetOrien(handle, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    //CamOsPrintf("pCus_init = %d us \n",timeGetTimeU()-TStart);
    return SUCCESS;
}

static int pCus_init_HDR(ms_cus_sensor *handle)
{
    int i,cnt=0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HDR);i++)
    {
        if(Sensor_init_table_HDR[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HDR[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_HDR[i].reg,Sensor_init_table_HDR[i].data) != SUCCESS)
            {
                cnt++;
                //SENSOR_DMSG("Sensor_init_table_HDR -> Retry %d...\n",cnt);
                if(cnt>=10)
                {
                    //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);
            }
        }
    }
    //pCus_SetOrien(handle, params->cur_orien);
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);

    //CamOsPrintf("pCus_init = %d us \n",timeGetTimeU()-TStart);
    return SUCCESS;
}

/*
int pCus_release(ms_cus_sensor *handle)
{
    ISensorIfAPI *sensor_if = &handle->sensor_if_api;
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

static int pCus_SetVideoRes_HDR(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_HDR;
            params->expo.vts = vts_30fps_HDR;
            params->expo.fps = 15;
            params->expo.max_short=105;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    sen_data = params->tMirror_reg[0].data & 0x30;
    SENSOR_DMSG("\n\n[%s]:mirror:%x\r\n\n\n\n",__FUNCTION__, sen_data);
    switch(sen_data&0x0E) {
        case 0x02:
        *orit = CUS_ORIT_M0F0;
        break;
        case 0x00:
        *orit = CUS_ORIT_M1F0;
        break;
        case 0x0e:
        *orit = CUS_ORIT_M0F1;
        break;
        case 0x0c:
        *orit = CUS_ORIT_M1F1;
        break;
    }
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    switch(orit) {
        case CUS_ORIT_M0F0:
            params->tMirror_reg[0].data = 0x02;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M1F0:
            params->tMirror_reg[0].data = 0x00;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M0F1:
            params->tMirror_reg[0].data = 0x0e;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M1F1:
            params->tMirror_reg[0].data = 0x0c;
            params->ori_dirty = true;
        break;
    }
    return SUCCESS;
}

static int pCus_SetOrien_HDR(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    switch(orit) {
        case CUS_ORIT_M0F0:
            params->tMirror_reg_HDR[0].data = 0x03;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M1F0:
            params->tMirror_reg_HDR[0].data = 0x01;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M0F1:
            params->tMirror_reg_HDR[0].data = 0x0f;
            params->ori_dirty = true;
        break;
        case CUS_ORIT_M1F1:
            params->tMirror_reg_HDR[0].data = 0x0d;
            params->ori_dirty = true;
        break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
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
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
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

    if ((params->expo.line) > (params->expo.vts)-8) {
        vts = params->expo.line + 8;
    }else
        vts = params->expo.vts;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS_HDR_SEF(ms_cus_sensor *handle)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_SEF(ms_cus_sensor *handle, u32 fps)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR*(max_fps*1000))/fps;
    }else{
      return FAIL;
    }

    params->expo.max_short = (((params->expo.vts-10)/17 - 1)>>1) << 1;
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetFPS_HDR_LEF(ms_cus_sensor *handle, u32 fps)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps_HDR*(max_fps*1000))/fps;
    }else{
      return FAIL;
    }

    params->expo.max_short = (((params->expo.vts-10)/17 - 1)>>1) << 1;
    params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x007f;
    params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
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
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        if(params->ori_dirty){
            //handle->sensor_if_api->SetSkipFrame(handle->snr_pad_group, params->expo.fps, 3);
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, ARRAY_SIZE(mirror_reg));
            params->ori_dirty = false;
        }
        break;
        case CUS_FRAME_ACTIVE:
        if(params->dirty)
        {
            SensorReg_Write(0x3208,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
            SensorReg_Write(0x3208,0x10);
            SensorReg_Write(0x3208,0xa0);
            params->dirty = false;
        }
        break;
        default :
        break;
    }

    return SUCCESS;
}

static int pCus_AEStatusNotify_HDR_LEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status){
    //OS02C10_params *params = (OS02C10_params *)handle->private_data;
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

static int pCus_AEStatusNotify_HDR_SEF(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status){
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->ori_dirty){
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg_HDR, ARRAY_SIZE(mirror_reg_HDR));
            params->ori_dirty = false;
        }
       /* if(params->skip_cnt){
            sensor_if->SetSkipFrame(handle->snr_pad_group, params->expo.fps, params->skip_cnt);
            params->skip_cnt = 0;
        } */
        if(params->dirty)
        {
            //SensorReg_Write(0x3208,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg_HDR_SEF, ARRAY_SIZE(expo_reg_HDR_SEF));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg_HDR_SEF, ARRAY_SIZE(gain_reg_HDR_SEF));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
            //SensorReg_Write(0x3208,0x10);
            //SensorReg_Write(0x3208,0xa0);
            params->dirty = false;
        }
        break;
        default :
        break;
    }

    return SUCCESS;
}

static int pCus_SetAEUSecs_HDR_LEF(ms_cus_sensor *handle, u32 us) {
    u32 vts = 0, lines = 0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period_HDR;

    if(lines<1) lines=1;
    if (lines > (params->expo.vts - params->expo.max_short - 2)) {
        lines = params->expo.vts - params->expo.max_short - 2;
    }else
        vts = params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    params->tExpo_reg[0].data = (lines>>8) & 0xff;
    params->tExpo_reg[1].data = (lines>>0) & 0xff;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEUSecs_HDR_SEF(ms_cus_sensor *handle, u32 *us) {
    u32 lines = 0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    lines |= (u32)(params->tExpo_reg_HDR_SEF[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg_HDR_SEF[1].data&0xff)<<0;

    *us = (lines*Preview_line_period_HDR)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs_HDR_SEF(ms_cus_sensor *handle, u32 us) {
    u32 vts=0, lines = 0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period_HDR;
    if(lines<1) lines=1;
    if (lines > params->expo.max_short - 2) {
        lines = params->expo.max_short - 2;
    }else
        vts = params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    params->tExpo_reg_HDR_SEF[0].data = (lines>>8) & 0xff;
    params->tExpo_reg_HDR_SEF[1].data = (lines>>0) & 0xff;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {

    u32 lines = 0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<0;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    OS02C10_params *params = (OS02C10_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if(lines<1) lines=1;
    if (lines > params->expo.vts-8) {
        vts = lines + 8;
    }else
        vts = params->expo.vts;

    params->expo.line = lines;
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    params->tExpo_reg[0].data = (lines>>8) & 0xff;
    params->tExpo_reg[1].data = (lines>>0) & 0xff;

    params->tVts_reg[0].data = (vts >> 8) & 0xff;
    params->tVts_reg[1].data = (vts >> 0) & 0xff;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
  //  OS02C10_params *params = (OS02C10_params *)handle->private_data;
    return SUCCESS;
}

#define MAX_A_GAIN 15872//(15.5*1024)
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 gain16 = 0, Again = 0x01, gain1024 = 0x00;

    if (gain<1024) {
        gain = 1024;
    } else if (gain>= MAX_A_GAIN) {
        gain = MAX_A_GAIN; //again max 15.5x,Without using digital gain
    }

    if(gain < 2048) {
        Again = gain>>10;  gain16 = (gain&0x3ff)>>6;
    }
    else if((gain>=2048 )&&(gain<4096))//X2~X4
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>7)<<1;
    }
    else if((gain>=4096 )&&(gain<8192))//X4~X8
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>8)<<2;
    }
    else if((gain>=8192 )&&(gain<=15872))//X8~X15.5
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>9)<<3;
    }

    gain1024 = (16*gain)/((Again<<4) + gain16);

    params->tGain_reg[0].data = Again;//high bit
    params->tGain_reg[1].data = (gain16<<4)&0xf0; //low byte
    params->tGain_reg[2].data = 0x01; //low byte
    params->tGain_reg[3].data = (gain1024&0x3fc)>>2;
    params->tGain_reg[4].data = (gain1024&0x03); //low byte

    SENSOR_DMSG("[%s] set gain =%d ,0x%x\n", __FUNCTION__, gain,gain_reg[0].data);
    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain_HDR_SEF(ms_cus_sensor *handle, u32 gain) {
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 gain16 = 0, Again = 0x01, gain1024 = 0x00;

    if (gain<1024) {
        gain = 1024;
    } else if (gain>= MAX_A_GAIN) {
        gain = MAX_A_GAIN; //again max 15.5x,Without using digital gain
    }

    if(gain < 2048) {
        Again = gain>>10;  gain16 = (gain&0x3ff)>>6;
    }
    else if((gain>=2048 )&&(gain<4096))//X2~X4
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>7)<<1;
    }
    else if((gain>=4096 )&&(gain<8192))//X4~X8
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>8)<<2;
    }
    else if((gain>=8192 )&&(gain<=15872))//X8~X15.5
    {
        Again = gain>>10;  gain16 = ((gain&0x3ff)>>9)<<3;
    }

    gain1024 = (16*gain)/((Again<<4) + gain16);

    params->tGain_reg_HDR_SEF[0].data = Again;//high bit
    params->tGain_reg_HDR_SEF[1].data = (gain16<<4)&0xf0; //low byte
    params->tGain_reg_HDR_SEF[2].data = 0x01; //low byte
    params->tGain_reg_HDR_SEF[3].data = (gain1024&0x3fc)>>2;
    params->tGain_reg_HDR_SEF[4].data = (gain1024&0x03); //low byte

    SENSOR_DMSG("[%s] set gain =%d ,0x%x\n", __FUNCTION__, gain,gain_reg[0].data);
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

static int OS02C10_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS; ///12;
    info->min =  Preview_line_period*1;//2
    info->step = Preview_line_period*1;//2
    return SUCCESS;
}

static int OS02C10_GetShutterInfo_HDR_LEF(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS; ///12;
    info->min =  Preview_line_period_HDR*1;//2
    info->step = Preview_line_period_HDR*1;//2
    return SUCCESS;
}

static int OS02C10_GetShutterInfo_HDR_SEF(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    info->max = (Preview_line_period_HDR * params->expo.max_short);
    info->min =  Preview_line_period_HDR*1;//2
    info->step = Preview_line_period_HDR*1;//2
    return SUCCESS;
}

static int pCus_poweron_HDR_LEF(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

static int pCus_poweroff_HDR_LEF(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}
static int pCus_GetSensorID_HDR_LEF(ms_cus_sensor *handle, u32 *id)
{
    *id = 0;
     return SUCCESS;
}
static int pCus_init_HDR_LEF(ms_cus_sensor *handle)
{
    return SUCCESS;
}
#if 0
static int pCus_GetVideoRes_HDR_LEF( ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res )
{
    *res = &handle->video_res_supported.res[res_idx];
    return SUCCESS;
}

static int pCus_SetVideoRes_HDR_LEF( ms_cus_sensor *handle, u32 res )
{
    handle->video_res_supported.ulcur_res = 0; //TBD
    return SUCCESS;
}
#endif

static int pCus_GetFPS_HDR_LEF(ms_cus_sensor *handle)
{
    OS02C10_params *params = (OS02C10_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps_HDR*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps_HDR*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_setCaliData_gain_linearity_HDR_LEF(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    return SUCCESS;
}
static int pCus_SetAEGain_cal_HDR_LEF(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}
static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

#define CMDID_I2C_READ   (0x01)
#define CMDID_I2C_WRITE  (0x02)

static int pCus_sensor_CustDefineFunction(ms_cus_sensor* handle,u32 cmd_id, void *param) {

    if(param == NULL || handle == NULL)
    {
        SENSOR_EMSG("param/handle data NULL \n");
        return FAIL;
    }

    switch(cmd_id)
    {
        case CMDID_I2C_READ:
        {
            I2C_ARRAY *reg = (I2C_ARRAY *)param;
            SensorReg_Read(reg->reg, &reg->data);
            SENSOR_EMSG("reg %x, read data %x \n", reg->reg, reg->data);
            break;
        }
        case CMDID_I2C_WRITE:
        {
            I2C_ARRAY *reg = (I2C_ARRAY *)param;
            SENSOR_EMSG("reg %x, write data %x \n", reg->reg, reg->data);
            SensorReg_Write(reg->reg, reg->data);
            break;
        }
        default:
            SENSOR_EMSG("cmd id %d err \n", cmd_id);
            break;
    }

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
    ms_cus_sensor *handle = drv_handle;
    OS02C10_params *params;
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
    params = (OS02C10_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_reg_HDR_SEF, gain_reg_HDR_SEF, sizeof(gain_reg_HDR_SEF));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tExpo_reg_HDR_SEF, expo_reg_HDR_SEF, sizeof(expo_reg_HDR_SEF));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tMirror_reg_HDR, mirror_reg_HDR, sizeof(mirror_reg_HDR));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    SENSOR_DMSG(handle->model_id,"OS02C10_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

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
    handle->pCus_sensor_SetPatternMode = OS02C10_SetPatternMode;
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
    handle->pCus_sensor_CustDefineFunction = pCus_sensor_CustDefineFunction;

     //sensor calibration
//    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = OS02C10_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line = 100;
    params->dirty = false;
    params->ori_dirty = false;
    return SUCCESS;
}

int cus_camsensor_init_handle_HDR_SEF(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    OS02C10_params *params = NULL;

    cus_camsensor_init_handle(drv_handle);
    params = (OS02C10_params *)handle->private_data;

    sprintf(handle->model_id,"OS02C10_MIPI_HDR_SEF");

    handle->bayer_id    = SENSOR_BAYERID_HDR;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->data_prec   = SENSOR_DATAPREC_HDR;
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_HDR;
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS_HDR;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth = 1920;
    handle->video_res_supported.res[0].nOutputHeight = 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps_HDR");

    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR;
    handle->mclk                        = Preview_MCLK_SPEED_HDR;

    handle->pCus_sensor_init        = pCus_init_HDR;

    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_SEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_SEF;
    handle->pCus_sensor_GetOrien        = pCus_GetOrien;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien_HDR;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify_HDR_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_HDR_SEF;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs_HDR_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_HDR_SEF;
    handle->pCus_sensor_GetShutterInfo = OS02C10_GetShutterInfo_HDR_SEF;

    params->expo.vts = vts_30fps_HDR;
    params->expo.line = 1000;
    params->expo.fps = 30;
    params->expo.max_short = 105;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 2;
    handle->ae_shutter_ctrl_num = 2;

    return SUCCESS;
}

int cus_camsensor_init_handle_HDR_LEF(ms_cus_sensor* drv_handle) {
    ms_cus_sensor *handle = drv_handle;
    OS02C10_params *params;
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
    params = (OS02C10_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_reg_HDR_SEF, gain_reg_HDR_SEF, sizeof(gain_reg_HDR_SEF));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tExpo_reg_HDR_SEF, expo_reg_HDR_SEF, sizeof(expo_reg_HDR_SEF));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tMirror_reg_HDR, mirror_reg_HDR, sizeof(mirror_reg_HDR));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"OS02C10_MIPI_HDR_LEF");

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
    handle->interface_attr.attr_mipi.mipi_lane_num = SENSOR_MIPI_LANE_NUM_HDR;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE_HDR;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_DCG;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Long frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS_HDR;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps_HDR");

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED_HDR;

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
    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 2;
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;


    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_HDR_LEF;

    handle->pCus_sensor_poweron     = pCus_poweron_HDR_LEF ;
    handle->pCus_sensor_poweroff    = pCus_poweroff_HDR_LEF;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID_HDR_LEF;

    handle->pCus_sensor_GetVideoResNum = NULL;
    handle->pCus_sensor_GetVideoRes       = NULL;
    handle->pCus_sensor_GetCurVideoRes  = NULL;
    handle->pCus_sensor_SetVideoRes       = NULL;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien      ;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien_HDR;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_LEF;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_LEF;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = OS02C10_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify_HDR_LEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs_HDR_LEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;

    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    //handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_HDR_LEF;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity_HDR_LEF;
    handle->pCus_sensor_GetShutterInfo = OS02C10_GetShutterInfo_HDR_LEF;
    handle->pCus_sensor_CustDefineFunction = pCus_sensor_CustDefineFunction;

    params->expo.vts=vts_30fps_HDR;
    params->expo.fps = 25;
    params->expo.max_short=105;
    params->dirty = false;
    params->ori_dirty = false;
    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle) {

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  OS02C10,
                            cus_camsensor_init_handle,
                            cus_camsensor_init_handle_HDR_SEF,
                            cus_camsensor_init_handle_HDR_LEF,
                            OS02C10_params
                         );
