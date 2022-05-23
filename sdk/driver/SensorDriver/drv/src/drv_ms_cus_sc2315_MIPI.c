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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC2315_MIPI);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
//#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE            CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL   CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
#define ENABLE            1
#define DISABLE           0
#undef SENSOR_DBG
#define SENSOR_DBG        0

#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) LOGD(args)
//#define SENSOR_DMSG(args...) LOGE(args)
//#define SENSOR_DMSG(args...) printf(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
///////////////////////////////////////////////////////////////
//          @@@                                              //
//         @ @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//           @@      ==  S t a r t * H e r e  ==             //
//          @@@@                                             //
//                                                           //
//      Start Step 1 --  show preview on LCM                 //
//                                                           //
//  Fill these #define value and table with correct settings //
//      camera can work and show preview on LCM              //
//                                                           //
///////////////////////////////////////////////////////////////

////////////////////////////////////
// Sensor-If Info                 //
////////////////////////////////////
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM        (2)                     // Initial Setting Support Lane-Number

#define SENSOR_ISP_TYPE             ISP_EXT                 //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER       //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_MIPI      // Please Check enum of CUS_SENIF_BUS
#define SENSOR_MIPI_HSYNC_MODE      PACKET_HEADER_EDGE1     // Please Check enum of CUS_CSI_VC_HS_MODE
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10    // Please Check enum of CUS_DATAPRECISION
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9098     // Please Check enum of CUS_SEN_FMT_CONV_MODE
#define SENSOR_BAYERID              CUS_BAYER_BG            // Please Check enum of CUS_SEN_BAYER
#define SENSOR_RGBIRID              CUS_RGBIR_NONE          // Please Check enum of CUS_SEN_RGBIR
#define SENSOR_ORIT                 CUS_ORIT_M0F0           // Please Check enum of CUS_CAMSENSOR_ORIT

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_27MHZ       // Please Check enum of CUS_MCLK_FREQ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x60                //I2C slave address
#define SENSOR_I2C_SPEED             300000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL 	       1                 //I2C Channel
//#define SENSOR_I2C_PAD_MODE          2                 //Pad/Mode Number
#define SENSOR_I2C_LEGACY            I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT               I2C_FMT_A16D8       //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

////////////////////////////////////
// Sensor Signal                  //
////////////////////////////////////
#define SENSOR_PWDN_POL              CUS_CLK_POL_NEG     // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL               CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG
                                                         // VSYNC/HSYNC POL can be found in data sheet timing diagram,
                                                         // Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.
#define SENSOR_VSYNC_POL             CUS_CLK_POL_NEG     // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL             CUS_CLK_POL_NEG     // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL              CUS_CLK_POL_NEG     // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

////////////////////////////////////
// Sensor ID                      //
////////////////////////////////////
#undef SENSOR_NAME
#define SENSOR_NAME                  SC2315

#define SENSOR_ID_r3107              0x3107
#define SENSOR_ID_r3108              0x3108
#define SENSOR_ID                    0x2238
////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {
    enum {LINEAR_RES_1 = 0, LINEAR_RES_END}mode;   // Modify it based on number of support resolution

    struct _senout{                          // Sensor Output Image info
    	s32 width, height, min_fps, max_fps;
    }senout;

    struct _sensif{                          // VIF Get Image Info
    	s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;

    struct _senstr{                          // Show resolution string
    	const char* strResDesc;
    }senstr;
}sc2315_mipi_linear[] = {
    {
     LINEAR_RES_1,
     {1936, 1096, 3, 30},
     {0, 0, 1920, 1080},
     {"1920x1080@30fps"}
    },
};
#define Preview_line_period                         29600   //33.3ms/1125 =29600ns,
#define vts_30fps                                   1125    //for MCLK=37P125MHz

////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_GAIN_PRECISION_16                    0 // 1:1/16 0: 1/64
#if SENSOR_GAIN_PRECISION_16
#define SENSOR_MAX_GAIN                             (480 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#else
#define SENSOR_MAX_GAIN                             (465 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#endif
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)

////////////////////////////////////
// Mirror-Flip Info               //
////////////////////////////////////
#define SC2315_BLC                                  0x3928   // if mirror on set 0x5 else set 0x1
#define MIRROR_FLIP                                 0x3221
#define SENSOR_NOR                                  0x0
#define SENSOR_MIRROR_EN                            0x6 << (0)
#define SENSOR_FLIP_EN                              0x6 << (4)
#define SENSOR_MIRROR_FLIP_EN                       0x66

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

typedef struct {
    struct {
        bool bVideoMode;
        u16 res_idx;
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
        u32 expo_lines;
    } expo;

    u32 idx_tmp;
    u32 set_gain;
    bool dirty;
    bool change;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tGain_patch_reg[2];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tDpc_reg[2];
    I2C_ARRAY tNr2_reg[1];
    bool orien_dirty;
} sc2315_params;

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

///////////////////////////////////////////////////////////////
//          @@@                                              //
//         @  @@                                             //
//           @@                                              //
//          @@                                               //
//         @@@@@                                             //
//                                                           //
//      Start Step 2 --  set Sensor initial and              //
//                       adjust parameter                    //
//  Fill these register table with resolution settings       //
//      camera can work and show preview on LCM              //
//                                                           //
///////////////////////////////////////////////////////////////
const static I2C_ARRAY Sensor_init_table_1lane_linear[] =
{
    //[ParaList]
    {0x0103, 0x01},  // bit[0]: software reset
    {0xffff, 0x20},
    {0x0100, 0x00},  // bit[0]: 0h: manual mode; 1h sleep mode
    {0xffff, 0x20},

    {0x3001, 0xFE},//[0] c_y
    {0x3018, 0x32},//[7:5] lane_num-1
    {0x3034, 0x81},//pll2 bypass
    {0x3039, 0xa2},//pll1 bypass
    {0x320c, 0x0a},
    {0x320d, 0x50},  //a80->a50 2640
    {0x3e01, 0x23},  //exp_line
    {0x363c, 0x05},  //04
    {0x3635, 0xa8},  //c0
    {0x363b, 0x0d},  //0d
    {0x3620, 0x08},
    {0x3622, 0x02},
    {0x3635, 0xc0},  //
    {0x3908, 0x10},
    {0x3624, 0x08}, //count_clk inv  need debug  flash row in one channel
    {0x5000, 0x06},  //rts column test
    {0x3e06, 0x00},
    {0x3e08, 0x03},
    {0x3e09, 0x10},
    {0x3333, 0x10},
    {0x3306, 0x7e},

    //{0x3e08, 0x1f},
    //{0x3e09, 0x1f},
    //{0x3e06, 0x03},
    {0x3902, 0x05},
    //{0x3909, 0x01},  //auto blc
    //{0x390a, 0xf5},  //auto blc
    {0x3213, 0x08},
    {0x337f, 0x03}, //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
    {0x3368, 0x04},
    {0x3369, 0x00},
    {0x336a, 0x00},
    {0x336b, 0x00},
    {0x3367, 0x08},
    {0x330e, 0x30},
    {0x3366, 0x7c}, // div_rst gap

    {0x3633, 0x42},
    {0x330b, 0xe0},
    {0x3637, 0x57},
    {0x3302, 0x1f}, // adjust the gap betwen first and second cunt_en pos edage to even times the clk
    {0x3309, 0xde}, // adjust the gap betwen first and second cunt_en pos edage to even times the clk
	//{0x303f, 0x81}, // pclk sel pll_sclk_dig_div

    //leage current
    {0x3907, 0x00},
    {0x3908, 0x61},
    {0x3902, 0x45},
    {0x3905, 0xb8},
    //{0x3904, 0x06}, //10.18
    {0x3e01, 0x8a},
    {0x3e02, 0x10},
    {0x3e06, 0x00},
    {0x3038, 0x48},
    {0x3637, 0x5d},
    {0x3e06, 0x00},
    //0921
    {0x3908, 0x11},
    {0x335e, 0x01},  //ana dithering
    {0x335f, 0x03},
    {0x337c, 0x04},
    {0x337d, 0x06},
    {0x33a0, 0x05},
    {0x3301, 0x04},
    {0x3633, 0x4f},  //prnu
    {0x3622, 0x06},  //blksun
    {0x3630, 0x08},
    {0x3631, 0x84},
    {0x3306, 0x30},
    {0x366e, 0x08},  // ofs auto en [3]
    {0x366f, 0x22},  // ofs+finegain  real ofs in {0x3687[4:0]
    {0x3637, 0x59},  // FW to 4.6k //9.22
    {0x3320, 0x06},  // New ramp offset timing
    //{0x3321, 0x06},
    {0x3326, 0x00},
    {0x331e, 0x11},
    {0x331f, 0xc1},
    {0x3303, 0x20},
    {0x3309, 0xd0},
    {0x330b, 0xbe},
    {0x3306, 0x36},
    {0x3635, 0xc2}, //TxVDD},HVDD
    {0x363b, 0x0a},
    {0x3038, 0x88},
    //9.22
    {0x3638, 0x1f}, //ramp_gen by sc  {0x30
    {0x3636, 0x25}, //
    {0x3625, 0x02},
    {0x331b, 0x83},
    {0x3333, 0x30},

    //10.18
    {0x3635, 0xa0},
    {0x363b, 0x0a},
    {0x363c, 0x05},
    {0x3314, 0x13},//preprecharge
    //20171101 reduce hvdd pump lighting
    {0x3038, 0xc8},// high pump clk},low lighting
    {0x363b, 0x0b},//high hvdd },low lighting
    {0x3632, 0x18},//large current},low ligting  {0x38 (option)
    //20171102 reduce hvdd pump lighting
    {0x3038, 0xff},// high pump clk},low lighting
    {0x3639, 0x09},
    {0x3621, 0x28},
    {0x3211, 0x0c},
    //20171106
    {0x366f, 0x26},
    //20171121
    {0x366f, 0x2f},
    {0x3320, 0x01},
    {0x3306, 0x48},
    {0x331e, 0x19},
    {0x331f, 0xc9},
    {0x330b, 0xd3},
    {0x3620, 0x28},
    //20171122
    {0x3309, 0x60},
    {0x331f, 0x59},
    {0x3308, 0x10},
    {0x3630, 0x0c},

    //digital ctrl
    {0x3f00, 0x07},  // bit[2] = 1
    {0x3f04, 0x05},
    {0x3f05, 0x04},  // hts / 2 - {0x24

    {0x3802, 0x01},
    {0x3235, 0x08},
    {0x3236, 0xc8}, // vts x 2 - 2

    //20171127
    {0x3630, 0x1c},

    //20171130
    {0x33aa, 0x10},//low power

    //20171208  logical   inter
    {0x3670, 0x04}, //{0x3631 3670[2] enable  {0x3631 in {0x3682
    {0x3677, 0x86},//gain<gain0  0629
    {0x3678, 0x88},//gain0=<gain<gain1
    {0x3679, 0x88},//gain>=gain1
    {0x367e, 0x08},//gain0 {3e08[4:2]},3e09[3:1]}
    {0x367f, 0x28},//gain1

    {0x3670, 0x0c}, //{0x3633 3670[3] enable  {0x3633 in {0x3683       20171227
    {0x3690, 0x33},//gain<gain0  //0629
    {0x3691, 0x11},//gain0=<gain<gain1
    {0x3692, 0x43},//gain>=gain1  0629
    {0x369c, 0x08},//gain0{3e08[4:2]},3e09[3:1]}
    {0x369d, 0x28},//gain1

    {0x360f, 0x01}, //{0x3622 360f[0] enable  {0x3622 in {0x3680
    {0x3671, 0xc6},//gain<gain0
    {0x3672, 0x06},//gain0=<gain<gain1
    {0x3673, 0x16},//gain>=gain1
    {0x367a, 0x28},//gain0{3e08[4:2]},3e09[3:1]}
    {0x367b, 0x3f},//gain1

	// 20171218  30fps
    {0x320c, 0x08},//hts   2200
    {0x320d, 0x98},
    {0x320e, 0x04},//vts  1125
    {0x320f, 0x65},
    //digital ctrl
    {0x3f04, 0x04},
    {0x3f05, 0x28},  // hts / 2 - {0x24
    {0x3235, 0x08},
    {0x3236, 0xc8}, // vts x 2 - 2

    //20171225 BLC power save mode
    {0x3222, 0x29},
    {0x3901, 0x02},
    {0x3905, 0x98},

    //20171227
    {0x3e1e, 0x34}, // digital finegain enable

    //20180113
    {0x3314, 0x08},

    //20180408
    {0x3900, 0x19}, //frame average
    {0x391d, 0x04}, // digital dithering {0x01: open dithering @1x
    {0x391e, 0x00},

    //0629
    {0x3641, 0x01},
    {0x3213, 0x04},
    {0x3614, 0x80}, //rs_lo to pad
    {0x363b, 0x08},
    {0x363a, 0x9f},
    {0x3630, 0x9c},

    {0x3301, 0x0f}, //1127 [5},10]
    {0x3306, 0x48}, //20180916[hl},bs][1},36},4c][2},38},4c]
    {0x3632, 0x08},

    {0x330b, 0xcd}, //[c6},dd]	20180916[bs},of][1},c8},dd][2},c9},dd][3},c9},dd]

    //20180917 mipi
    //{0x3018, 0x33},//[7:5] lane_num-1
	//{0x3018, 0x32},//[7:5] lane_num-1 fix mipi rx receive error
    {0x3031, 0x0a},//[3:0] bitmode
    {0x3037, 0x20},//[6:5] bitsel
    //{0x3001, 0xFE},//[0] c_y
    //lane_dis of lane3~8
    //{0x3018, 0x12},
    //{0x3019, 0xfc},
    {0x4603, 0x00},//[0] data_fifo mipi mode
    {0x4827, 0x48},//[7:0] hs_prepare_time[7:0]
    {0x301c, 0x78},//close dvp
    {0x4809, 0x01},
    {0x3314, 0x04},
    {0x303c, 0x0e},
    {0x4837, 0x35},//[7:0] pclk period * 2

    //20180919
    //blc max
    {0x3933, 0x0a},
    {0x3934, 0x10},
    {0x3940, 0x60},
    {0x3942, 0x02},
    {0x3943, 0x1f},
    //blc temp
    {0x3960, 0xba},
    {0x3961, 0xae},
    {0x3962, 0x09},
    {0x3966, 0xba},
    {0x3980, 0xa0},
    {0x3981, 0x40},
    {0x3982, 0x18},
    {0x3903, 0x08},
    {0x3984, 0x08},
    {0x3985, 0x20},
    {0x3986, 0x50},
    {0x3987, 0xb0},
    {0x3988, 0x08},
    {0x3989, 0x10},
    {0x398a, 0x20},
    {0x398b, 0x30},
    {0x398c, 0x60},
    {0x398d, 0x20},
    {0x398e, 0x10},
    {0x398f, 0x08},
    {0x3990, 0x60},
    {0x3991, 0x24},
    {0x3992, 0x15},
    {0x3993, 0x08},
    {0x3994, 0x0a},
    {0x3995, 0x20},
    {0x3996, 0x38},
    {0x3997, 0xa0},
    {0x3998, 0x08},
    {0x3999, 0x10},
    {0x399a, 0x18},
    {0x399b, 0x30},
    {0x399c, 0x30},
    {0x399d, 0x18},
    {0x399e, 0x10},
    {0x399f, 0x08},
    {0x3934, 0x18},
    {0x3985, 0x40},
    {0x3986, 0x80},
    {0x3985, 0x18},//20190507
    {0x3986, 0x28},
    {0x3987, 0x70},
    {0x3990, 0x40},
    {0x3997, 0x70},

    //20180926
    {0x3637, 0x55},
    {0x363b, 0x06},
    {0x366f, 0x2c},
    //{0x330b, 0xcd}, //{0x3306={0x48	20180926[bs},of][1},c8},e1][2},c8},e1][3},c8},e1]
    //{0x3306, 0x48}, //{0x330b={0xcd	20180926[hl},bs][1},3a},4c][2},3a},4c][3},3a},4c]

    //20180927
    {0x5000, 0x06}, //dpc enable(Hbin & Hsub? {0x46)
	//{0x5780, 0xff}, //manual dpc
    {0x5780, 0x7f}, //auto dpc
    {0x5781, 0x04}, //white dead pixel threshold0
    {0x5782, 0x03}, //white dead pixel threshold1
    {0x5783, 0x02}, //white dead pixel threshold2
    {0x5784, 0x01}, //white dead pixel threshold3
    {0x5785, 0x18}, //black dead pixel threshold0
    {0x5786, 0x10}, //black dead pixel threshold1
    {0x5787, 0x08}, //black dead pixel threshold2
    {0x5788, 0x02}, //black dead pixel threshold3
    {0x57a0, 0x00}, //gain threshold1=2x
    {0x57a1, 0x71},
    {0x57a2, 0x01}, //gain threshold1=8x
    {0x57a3, 0xf1},

    //20180928 high temp
    {0x395e, 0xc0},
    {0x3962, 0x89},

    //20190114
    {0x3963, 0x80},//edges brighting when high temp

    //20190125
    {0x3039, 0xa6},//pll1 vco = 27/2*2*2.5*11=742.5
    {0x303b, 0x16},//sclk = vco/2/2/2.5=74.25

    //20190218
    {0x301f, 0x01},//setting id
    {0x3802, 0x00},//group hold

    //20190320	mirror & flip
    {0x3902, 0xc5},

    //init
    {0x3e00, 0x00},//max exp=vts*2-4}, min exp=0
    {0x3e01, 0x8a},
    {0x3e02, 0x60},
    {0x3e03, 0x0b},//gain mode
    {0x3e06, 0x00},//gain = 1x
    {0x3e07, 0x80},
    {0x3e08, 0x03},
    {0x3e09, 0x10},
    {0x3301, 0x0f}, //1127 [5},10]
    //{0x3306, 0x48}, //20171123
    {0x3632, 0x08},

    {0x3034, 0x01},//pll2 enable
    {0x3039, 0x26},//pll1 enable
	//{0xffff, 0x50},/////delay 20ms
    {0x0100, 0x01},
};

const static I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x22},      // {address of 0x3107, ID },
    {0x3108, 0x38},      // {address of 0x3108, ID },
    {0x3109, 0x20},      // {address of 0x3109, ID },
};

static I2C_ARRAY PatternTbl[] = {
    {0x0000,0x00},       // colorbar pattern , bit 0 to enable
};

const static I2C_ARRAY expo_reg[] = { // SHS0
    {0x3e01, 0x8a},
    {0x3e02, 0x60},      // bit[7:4]
};

const static I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65}, //bit0-3-->MSB
};

const static I2C_ARRAY gain_reg[] = {
    {0x3e09, 0x10},
    {0x3e08, 0x03},//hcg mode,bit 4
    {0x3e07, 0x80},
    {0x3e06, 0x00},//gain = 1x
};

const I2C_ARRAY nr2_reg[] = {
    {0x3314, 0x04},
};

const I2C_ARRAY dpc_reg[] = {
    {0x5781, 0x04},
    {0x5785, 0x18},
};

const I2C_ARRAY gain_patch_reg[] = {
    {0x3301, 0x0f},
    {0x3632, 0x08},
};

#if SENSOR_GAIN_PRECISION_16
const FINE_GAIN fine_sc2315_again[] = {
//gain map update for 1/16 precision
    {10000, 0x10},
    {10625, 0x11},
    {11250, 0x12},
    {11875, 0x13},
    {12500, 0x14},
    {13125, 0x15},
    {13750, 0x16},
    {14375, 0x17},
    {15000, 0x18},
    {15625, 0x19},
    {16250, 0x1a},
    {16875, 0x1b},
    {17500, 0x1c},
    {18125, 0x1d},
    {18750, 0x1e},
    {19375, 0x1f},
};

const FINE_GAIN fine_sc2315_dgain[] = {
//gain map update for 1/16 precision
    {10000, 0x80},
    {10625, 0x88},
    {11250, 0x90},
    {11875, 0x98},
    {12500, 0xA0},
    {13125, 0xA8},
    {13750, 0xB0},
    {14375, 0xB8},
    {15000, 0xC0},
    {15625, 0xC8},
    {16250, 0xD0},
    {16875, 0xD8},
    {17500, 0xE0},
    {18125, 0xE8},
    {18750, 0xF0},
    {19375, 0xF8},
};
#endif

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

/////////////////////////////////////////////////////////////////
//       @@@@@@@                                               //
//           @@                                                //
//          @@                                                 //
//          @@@                                                //
//       @   @@                                                //
//        @@@@                                                 //
//                                                             //
//      Step 3 --  complete camera features                    //
//                                                             //
//  camera set EV, MWB, orientation, contrast, sharpness       //
//   , saturation, and Denoise can work correctly.             //
//                                                             //
/////////////////////////////////////////////////////////////////


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

/////////////////// I2C function definition ///////////////////

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent ///////////////////
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

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

/*******I5/I6 Support MCLK List*******
 *    CUS_CMU_CLK_27MHZ,
 *    CUS_CMU_CLK_21P6MHZ,
 *    CUS_CMU_CLK_12MHZ,
 *    CUS_CMU_CLK_5P4MHZ,
 *    CUS_CMU_CLK_36MHZ,
 *    CUS_CMU_CLK_54MHZ,
 *    CUS_CMU_CLK_43P2MHZ,
 *    CUS_CMU_CLK_61P7MHZ,
 *    CUS_CMU_CLK_72MHZ,
 *    CUS_CMU_CLK_48MHZ,
 *    CUS_CMU_CLK_24MHZ,
 *    CUS_CMU_CLK_37P125MHZ,
 ******End of Support MCLK List*******/
static CUS_MCLK_FREQ UseParaMclk(const char *mclk)
{
    if (strcmp(mclk, "27M") == 0) {
        return CUS_CMU_CLK_27MHZ;
    } else if (strcmp(mclk, "12M") == 0) {
        return CUS_CMU_CLK_12MHZ;
    } else if (strcmp(mclk, "36M") == 0) {
        return CUS_CMU_CLK_36MHZ;
    } else if (strcmp(mclk, "48M") == 0) {
        return CUS_CMU_CLK_48MHZ;
    } else if (strcmp(mclk, "54M") == 0) {
        return CUS_CMU_CLK_54MHZ;
    } else if (strcmp(mclk, "24M") == 0) {
        return CUS_CMU_CLK_24MHZ;
    } else if (strcmp(mclk, "37.125M") == 0) {
        return CUS_CMU_CLK_37P125MHZ;
    }

    return Preview_MCLK_SPEED;
}

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc2315_params *params = (sc2315_params *)handle->private_data;

    params->idx_tmp = idx;
    //Sensor power on sequence
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);   // Powerdn Pull Low
    sensor_if->Reset(idx, handle->reset_POLARITY);     // Rst Pull Low
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
#if 0
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, ENABLE);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
#endif
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }

    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);  // Powerdn Pull High
    SENSOR_UDELAY(1);
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_UDELAY(100);

    sensor_if->Reset(idx, !handle->reset_POLARITY);    // Rst Pull High
    SENSOR_UDELAY(100);
    sensor_if->Reset(idx, handle->reset_POLARITY);     // Rst Pull Low
    SENSOR_UDELAY(100);
    sensor_if->Reset(idx, !handle->reset_POLARITY);

    SENSOR_DMSG("Sensor Power On finished\n");
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);   // Powerdn Pull Low
    sensor_if->Reset(idx, handle->reset_POLARITY);     // Rst Pull Low
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }

    handle->orient = SENSOR_ORIT;

    return SUCCESS;
}

/////////////////// Check Sensor Product ID /////////////////////////
static int pCus_CheckSensorProductID(ms_cus_sensor *handle)
{
	u16 sen_id_msb, sen_id_lsb, sen_data;

    /* Read Product ID */
    if (SensorReg_Read(SENSOR_ID_r3107, &sen_id_msb)) {
        return FAIL;
    }
    if (SensorReg_Read(SENSOR_ID_r3108, &sen_id_lsb)) {
        return FAIL;
    }

    sen_data = ((sen_id_msb & 0xFF) << 8) | (sen_id_lsb & 0xFF);

    if (sen_data != SENSOR_ID) {
    	printk("[**ERROR**] Product ID Fail: 0x%x, Please Check Sensor Insert Correct!!\n", sen_data);
        return FAIL;
    }
    return SUCCESS;
}

//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    u8 i,n;
    u8 table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    for(n=0;n<table_length;++n)
    {
      id_from_sensor[n].reg = Sensor_id_table[n].reg;
      id_from_sensor[n].data = 0;
    }

    *id =0;
    if(table_length>8)
    	table_length=8;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(n=0;n<4;++n) //retry , until I2C success
    {
       if(n > 3)
    	    return FAIL;
       if(SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
            break;
       else
    	    SENSOR_MSLEEP(1);
    }

    //convert sensor id to u32 format
    for(i=0;i<table_length;++i)
    {
       if(id_from_sensor[i].data != Sensor_id_table[i].data ){
    	    SENSOR_DMSG("[%s] Please Check IMX415 Sensor Insert!!\n", __FUNCTION__);
            return FAIL;
       }
       *id = id_from_sensor[i].data;
    }

    SENSOR_DMSG("[%s]SC2315 sensor ,Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int sc2315_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    u8 i;
    switch(mode)
    {
    case 1:
    	PatternTbl[0].data = 0x21; //enable
    	break;
    case 0:
    	PatternTbl[0].data &= 0xFE; //disable
    	break;
    default:
    	PatternTbl[0].data &= 0xFE; //disable
    	break;
    }
    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
#if 0
    	if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
    	{
        SENSOR_EMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
        return FAIL;
    	}
#endif
    }
    return SUCCESS;
}

static int pCus_init_mipi1lane_linear(ms_cus_sensor *handle)
{
    sc2315_params *params = (sc2315_params *)handle->private_data;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    u32 i=0, cnt=0;
    //s16 sen_data;

    if (pCus_CheckSensorProductID(handle)) {
        return FAIL;
    }

    //SENSOR_EMSG("SC2315 Sensor_init_table_1lane_linear\n");
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_1lane_linear);i++)
    {
        if(Sensor_init_table_1lane_linear[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_1lane_linear[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_1lane_linear[i].reg, Sensor_init_table_1lane_linear[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                	SENSOR_EMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                //SENSOR_UDELAY(1);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_1lane_linear[i].reg, Sensor_init_table_1lane_linear[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_1lane_linear[i].reg, &sen_data );
            if(Sensor_init_table_1lane_linear[i].data != sen_data)
            	printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_1lane_linear[i].reg);
                //printk("SC2315 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_1lane_linear[i].reg, Sensor_init_table_1lane_linear[i].data, sen_data);
#endif
        }
    }
#if 1
    SENSOR_MSLEEP(7);
    sensor_if->SetCSI_Clk(params->idx_tmp, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(params->idx_tmp, handle->interface_attr.attr_mipi.mipi_lane_num, ENABLE);
    sensor_if->SetCSI_LongPacketType(params->idx_tmp, 0, 0x1C00, 0);
#endif
    //params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    //params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
    //pCus_SetOrien(handle, handle->orient);
    printk("Sensor SC2315 Linear MODE Initial Finished\n");

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
    sc2315_params *params = (sc2315_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    handle->video_res_supported.ulcur_res = res_idx;

    switch (res_idx) {
        case 0:
            handle->pCus_sensor_init = pCus_init_mipi1lane_linear;
            params->expo.vts = vts_30fps;
            break;
        default:
            break;
    }
    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    s16 sen_data;
    //Read SENSOR MIRROR-FLIP STATUS
    SensorReg_Read(MIRROR_FLIP, &sen_data);

    switch(sen_data & SENSOR_MIRROR_FLIP_EN)
    {
        case SENSOR_NOR:
            *orit = CUS_ORIT_M0F0;
            break;
        case SENSOR_FLIP_EN:
            *orit = CUS_ORIT_M0F1;
            break;
        case SENSOR_MIRROR_EN:
            *orit = CUS_ORIT_M1F0;
            break;
        case SENSOR_MIRROR_FLIP_EN:
            *orit = CUS_ORIT_M1F1;
            break;
    }
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    sc2315_params *params = (sc2315_params *)handle->private_data;

    handle->orient = orit;
    params->orien_dirty = true;

    return SUCCESS;
}

static int DoOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    s16 sen_data;
    //Read SENSOR MIRROR-FLIP STATUS
    SensorReg_Read(MIRROR_FLIP, &sen_data);
    sen_data &= ~(SENSOR_MIRROR_FLIP_EN);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
        	//sen_data |= SENSOR_NOR;
        	handle->orient = CUS_ORIT_M0F0;
            break;
        case CUS_ORIT_M1F0:
        	sen_data |= SENSOR_MIRROR_EN;
        	handle->orient = CUS_ORIT_M1F0;
            break;
        case CUS_ORIT_M0F1:
        	sen_data |= SENSOR_FLIP_EN;
        	handle->orient = CUS_ORIT_M0F1;
            break;
        case CUS_ORIT_M1F1:
        	sen_data |= SENSOR_MIRROR_FLIP_EN;
        	handle->orient = CUS_ORIT_M1F1;
            break;
        default :
        	handle->orient = CUS_ORIT_M0F0;
            break;
    }
    //Write SENSOR MIRROR-FLIP STATUS
    SensorReg_Write(MIRROR_FLIP, sen_data);

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc2315_params *params = (sc2315_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
	s32 vts=0;
    sc2315_params *params = (sc2315_params *)handle->private_data;

    if(fps>= 5 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
    }else if(fps>= 5000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts =  (vts_30fps*30000)/fps;
    }else{
        printk("[%s] FPS %d out of range.\n",__FUNCTION__, fps);
        return FAIL;
    }

    //update vts based on exposure line
    if(params->expo.expo_lines > ((params->expo.vts * 2 ) - 4)){
        vts = ((params->expo.expo_lines + 4) / 2) ;
#if 0        //Update FPS Status
        if(fps>=5 && fps <= 30)
        	fps = (vts_30fps*30000)/(params->expo.vts * 1000);
        else if(fps>=5000 && fps <= 30000)
            fps = (vts_30fps*30000)/(params->expo.vts);
#endif
    }else{
        vts = params->expo.vts;
    }

    params->expo.fps = fps;
    params->expo.vts = vts;
    params->dirty = true;
    params->tVts_reg[0].data = (vts & 0xf00) >> 8 ;
    params->tVts_reg[1].data = (vts & 0x0ff) >> 0 ;

    //SENSOR_DMSG("*** vts ==  %d, reg0 0x%x  reg1 0x%x *** \n", vts, params->tVts_reg[0].data, params->tVts_reg[1].data);

    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc2315_params *params = (sc2315_params *)handle->private_data;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty) {
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tNr2_reg, ARRAY_SIZE(nr2_reg));

            	SensorReg_Write(0x3812,0x00);
                if(params->dirty) {
                    SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                    SensorRegArrayW((I2C_ARRAY*)params->tGain_patch_reg, ARRAY_SIZE(gain_patch_reg));
                    SensorRegArrayW((I2C_ARRAY*)params->tDpc_reg, ARRAY_SIZE(dpc_reg));
                    SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                    params->dirty = false;
                }

                if(params->orien_dirty) {
                    DoOrien(handle, handle->orient);
                    params->orien_dirty = false;
                }
                SensorReg_Write(0x3812,0x30);
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    sc2315_params *params = (sc2315_params *)handle->private_data;

    *us = (params->expo.expo_lines*Preview_line_period)/1000;
    //SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, params->expo.expo_lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 expo_line = 0, vts = 0;
    sc2315_params *params = (sc2315_params *)handle->private_data;

    expo_line = 1000 * us / (Preview_line_period/2); // Preview_line_period in ns

    // Limited expo_line upper-value
    if (expo_line > ((params->expo.vts*2) - 4))
    	vts = (expo_line + 4) /2;
    else
    	vts=params->expo.vts;

    params->expo.expo_lines = expo_line;

    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
				expo_line,
                vts
                );

    if (expo_line < 0x250)
        params->tNr2_reg[0].data = 0x14;
    else if (expo_line > 0x450)
        params->tNr2_reg[0].data = 0x04;

    params->tExpo_reg[0].data = ((expo_line & 0xff0) >> 4);
    params->tExpo_reg[1].data = ((expo_line & 0x00f) << 4);

    params->tVts_reg[0].data = ((vts & 0xf00) >> 8);
    params->tVts_reg[1].data = ((vts & 0x0ff) >> 0);

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
	sc2315_params *params = (sc2315_params *)handle->private_data;

	*gain = params->set_gain;

    return SUCCESS;
}

#if SENSOR_GAIN_PRECISION_16
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
	sc2315_params *params = (sc2315_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u8 i = 0;
    u8 Dgain = 1, Ana_gain = 1;
	u64 Fine_again = 100000, Fine_dgain = 100000;
    u8 Dgain_reg = 0, Ana_gain_reg = 0;
    u8 Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;
    u8 dpc_5781 = 0x04, dpc_5785 = 0x08;
    u8 gain_patch_3301 = 0, gain_patch_3632 = 0;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){
        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    if (gain < 1024)
        gain = 1024;
    else if (gain > SENSOR_MAX_GAIN*1024)
        gain = SENSOR_MAX_GAIN*1024;

    params->set_gain = gain;
    gain_patch_3301 = 0x0f;
    gain_patch_3632 = 0x48;

    if (gain < 2 * 1024) {      // gain < 2x
         Ana_gain = 1;
         Ana_gain_reg = 0x00;
         Fine_dgain_reg = 0x80;
         Dgain_reg = 0x00;
         Dgain = 1;
         Fine_dgain = 10000;
         gain_patch_3301 = 0x0f;
         gain_patch_3632 = 0x08;
    }
    else if (gain <  4 * 1024) { // gain = 2x ~ < 4x
         Ana_gain = 2;
         Ana_gain_reg = 0x01;
         Dgain_reg = 0x00;
         Fine_dgain_reg = 0x80;
         Dgain = 1;
         Fine_dgain = 10000;
         gain_patch_3301 = 0x20;
         gain_patch_3632 = 0x08;
    }
    else if (gain <  8 * 1024) { // gain = 4x ~ < 8x
         Ana_gain = 4;
         Ana_gain_reg = 0x03;
         Dgain_reg = 0x00;
         Fine_dgain_reg = 0x80;
         Dgain = 1;
         Fine_dgain = 10000;
         gain_patch_3301 = 0x28;
         gain_patch_3632 = 0x08;
    }
    else if (gain < (155 * 1024) / 10) {  // gain = 8x ~ < 15.5x
         Ana_gain = 8;
         Ana_gain_reg = 0x07;
         Dgain_reg = 0x00;
         Fine_dgain_reg = 0x80;
         Dgain = 1;
         Fine_dgain = 10000;
         gain_patch_3301 = 0x80;
         gain_patch_3632 = 0x08;
    }
    else if (gain < (310* 1024) / 10) {  // Dgain x1
         Ana_gain = 8;
         Ana_gain_reg = 0x07;
         Fine_again_reg = 0x1f;
         Dgain_reg = 0x00;
         Dgain = 1;
         Fine_again = 19375;
    }
    else if (gain < (620 * 1024) / 10) {  // x4 Dgain x2
         Ana_gain = 8;
         Ana_gain_reg = 0x07;
         Fine_again_reg = 0x1f;
         Dgain = 2;
         Dgain_reg = 0x01;
         Fine_again = 19375;
    }
    else if (gain < (1240 * 1024) / 10) { // Dgain x8
    	 Ana_gain = 8;
    	 Ana_gain_reg = 0x07;
         Fine_again_reg = 0x1f;
    	 Dgain = 4;
    	 Dgain_reg = 0x03;
    	 Fine_again = 19375;
    }
    else if (gain < (2480 * 1024) / 10) { // Dgain x8
    	 Ana_gain = 8;
    	 Ana_gain_reg = 0x07;
         Fine_again_reg = 0x1f;
    	 Dgain = 8;
    	 Dgain_reg = 0x07;
    	 Fine_again = 19375;
    }
    else if (gain <= SENSOR_MAX_GAIN * 1024) {
    	 Ana_gain = 8;
    	 Ana_gain_reg = 0x07;
         Fine_again_reg = 0x1f;
         Dgain = 8;
         Dgain_reg = 0x0f;
         Fine_again = 19375;
    }

    if (gain <= (155 * 1024) / 10)
    {
        Fine_again =(gain*1000)/ ((Dgain * Ana_gain * Fine_dgain * 1024)/ 10000) * 10;
        for(i = 1; i< sizeof(fine_sc2315_again)/sizeof(FINE_GAIN);i++)
        {
            if(Fine_again >= fine_sc2315_again[i-1].gain && Fine_again <= fine_sc2315_again[i].gain)
            {
                Fine_again_reg = (fine_sc2315_again[i].gain - Fine_again) > (Fine_again - fine_sc2315_again[i-1].gain) ? fine_sc2315_again[i-1].fine_gain_reg:fine_sc2315_again[i].fine_gain_reg;
                break;
            }
            else if(Fine_again > fine_sc2315_again[(sizeof(fine_sc2315_again)/sizeof(FINE_GAIN)) - 1].gain)
            {
                Fine_again_reg = 0x1f;
                break;
            }
        }
    }
    else
    {
        Fine_dgain =(gain *1000)/((Dgain * Ana_gain * Fine_again*1024)/ 10000) * 10;
        for(i = 1; i< sizeof(fine_sc2315_dgain)/sizeof(FINE_GAIN);i++)
        {
            if(Fine_dgain >= fine_sc2315_dgain[i-1].gain && Fine_dgain <= fine_sc2315_dgain[i].gain)
            {
                Fine_dgain_reg = (fine_sc2315_dgain[i].gain - Fine_dgain) > (Fine_dgain - fine_sc2315_dgain[i-1].gain) ? fine_sc2315_dgain[i-1].fine_gain_reg:fine_sc2315_dgain[i].fine_gain_reg;
                break;
            }
            else if(Fine_dgain > fine_sc2315_dgain[(sizeof(fine_sc2315_dgain)/sizeof(FINE_GAIN)) - 1].gain)
            {
                Fine_dgain_reg = 0xf8;
                break;
            }
        }
    }
    // DPC Fix
    if (gain <  8 * 1024) {
    	dpc_5781 = 0x04;
    	dpc_5785 = 0x18;
    }
    else{ // >= 8x
    	dpc_5781 = 0x02;
    	dpc_5785 = 0x08;
    }
    SENSOR_DMSG("[%s] gain %d ; again 0x%x, f_again 0x%x, Fine_dgain %lld, dgain 0x%x, f_daing 0x%x\n", __FUNCTION__,
                                                                                                        gain, \
                                                                                                        Ana_gain_reg, \
                                                                                                        Fine_again_reg, \
                                                                                                        Fine_dgain, \
                                                                                                        Dgain_reg, \
                                                                                                        Fine_dgain_reg);
    params->tDpc_reg[0].data = dpc_5781;
    params->tDpc_reg[1].data = dpc_5785;

    params->tGain_patch_reg[0].data = gain_patch_3301;
    params->tGain_patch_reg[1].data = gain_patch_3632;

    params->tGain_reg[0].data = Fine_again_reg;
    params->tGain_reg[1].data = ((Ana_gain_reg & 0x7) << 2) + 3;
    params->tGain_reg[2].data = Fine_dgain_reg;
    params->tGain_reg[3].data = Dgain_reg & 0xF;

    params->dirty = true;
    return SUCCESS;
}
#else
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
	sc2315_params *params = (sc2315_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u8 i = 0, Dgain = 1, Ana_gain = 1;
	//u64 Fine_dgain = 10000;//Fine_again = 10000,
    u8 Dgain_reg = 0, Ana_gain_reg = 0;
    u8 Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;
    u8 dpc_5781 = 0x04, dpc_5785 = 0x08;
    u8 gain_patch_3301 = 0x0f, gain_patch_3632 = 0x08;

#if 0
    static u8 old_gain = 0;

    if (old_gain == gain)
        return SUCCESS;
    else
        old_gain = gain;
#endif

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){
        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    if (gain < 1024)
        gain = 1024;
    else if (gain > SENSOR_MAX_GAIN*1024)
        gain = SENSOR_MAX_GAIN*1024;

    params->set_gain = gain;

    if (gain < 2 * 1024) {      // gain < 2x
        Ana_gain = 1;
        Ana_gain_reg = 0x00;
        Fine_again_reg = 0x10;
        gain_patch_3301 = 0x0f;
        gain_patch_3632 = 0x08;
    }
    else if (gain <  4 * 1024) { // gain = 2x ~ < 4x
        Ana_gain = 2;
        Ana_gain_reg = 0x01;
        Fine_again_reg = 0x10;
        gain_patch_3301 = 0x20;
        gain_patch_3632 = 0x08;
    }
    else if (gain <  8 * 1024) { // gain = 4x ~ < 8x
        Ana_gain = 4;
        Ana_gain_reg = 0x03;
        Fine_again_reg = 0x10;
        gain_patch_3301 = 0x28;
        gain_patch_3632 = 0x08;
    }
    else if (gain < 16 * 1024) {  // gain = 8x ~ < 16x
        Ana_gain = 8;
        Ana_gain_reg = 0x07;
        Fine_again_reg = 0x10;
        gain_patch_3301 = 0x80;
        gain_patch_3632 = 0x08;
    }
    else if (gain >= 16) {
        Ana_gain = 15;
        Ana_gain_reg = 0x07;
        Fine_again_reg = 0x1e;
        gain_patch_3301 = 0x80;
        gain_patch_3632 = 0x48;
    }

    Dgain = gain / (Ana_gain * 1024);

    if (Dgain >= 2 && Dgain < 4)
        Dgain = (Dgain >> 1) << 1;
    else if (Dgain >= 4 && Dgain < 8)
    	 Dgain = (Dgain >> 2) << 2;
    else if (Dgain >= 8)
    	 Dgain = (Dgain >> 3) << 3;

    Dgain_reg = Dgain - 1;


    //Fix Dgain
    Fine_dgain_reg = (gain * 128) / (Ana_gain * Dgain * 1024);

    // DPC Fix
    if (gain <  8 * 1024) {
        dpc_5781 = 0x04;
        dpc_5785 = 0x18;
    }
    else { // >= 8x
        dpc_5781 = 0x02;
        dpc_5785 = 0x08;
    }

    SENSOR_DMSG("[%s] gain %d ; again 0x%x, f_again 0x%x, dgain 0x%x, f_daing %d\n", __FUNCTION__,
                                                                                     gain, \
                                                                                     Ana_gain_reg, \
                                                                                     Fine_again_reg, \
                                                                                     Dgain_reg, \
                                                                                     Fine_dgain_reg);

    params->tDpc_reg[0].data = dpc_5781;
    params->tDpc_reg[1].data = dpc_5785;
    params->tGain_patch_reg[0].data = gain_patch_3301;
    params->tGain_patch_reg[1].data = gain_patch_3632;

    params->tGain_reg[0].data = Fine_again_reg;
    params->tGain_reg[1].data = ((Ana_gain_reg & 0x7) << 2) + 0x3; // Default bit[0: 1] 0x3
    params->tGain_reg[2].data = Fine_dgain_reg;
    params->tGain_reg[3].data = Dgain_reg & 0xF;

    params->dirty = true;
    return SUCCESS;
}
#endif

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    //*max = 1000000/Preview_MIN_FPS;
    *max = 1000000/sc2315_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int SC2315_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    //info->max = 1000000000/Preview_MIN_FPS;//
    info->max = 1000000000/sc2315_mipi_linear[0].senout.min_fps;
    info->min = (Preview_line_period * 1);
    info->step = Preview_line_period;

    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc2315_params *params;
    s32 res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }

    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (sc2315_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_patch_reg, gain_patch_reg, sizeof(gain_patch_reg));
    memcpy(params->tDpc_reg, dpc_reg, sizeof(dpc_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tNr2_reg, nr2_reg, sizeof(nr2_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"SC2315_MIPI");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x60;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //320000;

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type    = SENSOR_ISP_TYPE;
    //handle->data_fmt    = SENSOR_DATAFMT;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;
    handle->data_prec   = SENSOR_DATAPREC;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
    handle->interface_attr.attr_mipi.mipi_lane_num    = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order   = 0; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode  = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode    = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0; //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.num_res = LINEAR_RES_END;
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.res[res].width         = sc2315_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = sc2315_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = sc2315_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = sc2315_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = sc2315_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = sc2315_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = sc2315_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = sc2315_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, sc2315_mipi_linear[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                  = UseParaMclk(SENSOR_DRV_PARAM_MCLK());

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY         = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY        = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY        = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY        = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY         = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release        = cus_camsensor_release_handle;
    handle->pCus_sensor_init           = pCus_init_mipi1lane_linear;
    //handle->pCus_sensor_powerupseq     = pCus_powerupseq;
    handle->pCus_sensor_poweron        = pCus_poweron;
    handle->pCus_sensor_poweroff       = pCus_poweroff;
    handle->pCus_sensor_GetSensorID    = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes    = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes    = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien       = pCus_GetOrien;
    handle->pCus_sensor_SetOrien       = pCus_SetOrien;
    handle->pCus_sensor_GetFPS         = pCus_GetFPS;
    handle->pCus_sensor_SetFPS         = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = sc2315_SetPatternMode; //NONE

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT;
    handle->ae_gain_ctrl_num           = 1;
    handle->ae_shutter_ctrl_num        = 1;
    handle->sat_mingain                = SENSOR_MIN_GAIN;//g_sensor_ae_min_gain;
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;
    //handle->pCus_sensor_GetDGainRemainder = pCus_GetDGainRemainder;

    //sensor calibration
    //handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    //handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo  = SC2315_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->expo.expo_lines = 1672;
    params->expo.fps        = 30;
    params->orien_dirty     = 0;
    params->dirty           = 0;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(SC2315_MIPI,
                            cus_camsensor_init_handle_linear,
							NULL,
							NULL,
                            sc2315_params
                         );
