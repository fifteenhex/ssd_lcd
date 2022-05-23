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

#ifdef __cplusplus
}
#endif
#define TEST_15FPS (1)

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(IMX317_HDR);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (4)
//#define SENSOR_MIPI_HDR_MODE (1) //0: Non-HDR mode. 1:Sony DOL mode
//MIPI config end.
//============================================

#define R_GAIN_REG 1
#define G_GAIN_REG 2
#define B_GAIN_REG 3

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
#define F_number  22                                  // CFG, demo module
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
#define SENSOR_MAX_GAIN     177
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_WIDTH       3840//3072                    //resolution Width when preview
#define Preview_HEIGHT      2160//2160                    //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS

#define SENSOR_I2C_ADDR    0x34                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_NEG        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;

#define Preview_CROP_START_X    0                       //CROP_START_X
#define Preview_CROP_START_Y    0                       //CROP_START_Y

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us);

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
        u32 expo_lef_us;
        u32 expo_sef_us;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    int max_rhs1;
    bool dirty;
    I2C_ARRAY tCurrent_Mirror_Flip_Tbl[1];
    I2C_ARRAY tGain_reg[3];
    CUS_GAIN_GAP_ARRAY tGain_gap_compensate[16];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tExpo_shr_dol1_reg[2];
    I2C_ARRAY tExpo_rhs1_reg[2];
    I2C_ARRAY tExpo_shr_dol2_reg[2];
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tPatternTbl[2];

} imx317_params;
// set sensor ID address and data,

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3004, 0x03},      // {address of ID, ID },
    {0x3005, 0x31},
};

const I2C_ARRAY Sensor_init_table_4lane_dol_30fps[] =
{
//hdr mode ,3840X2160@30fps MIPI clock 1440M
    {0x3000, 0x12},   //standby
    {0x3120, 0xd5},
    {0x3121, 0x00},
    {0x3122, 0x02},
   // {0x3123, 0x01},
    {0x3129, 0x8a},
    {0x312a, 0x02},
    {0x312d, 0x02},
   // {0x3ac4, 0x01},
    {0x310b, 0x00},
    {0x304c, 0x00},
    {0x304d, 0x03},
    {0x331c, 0x1a},
    {0x331d, 0x00},
    {0x3502, 0x02},
    {0x3529, 0x0e},
    {0x352a, 0x0e},
    {0x352b, 0x0e},
    {0x3538, 0x0e},
    {0x3539, 0x0e},
    {0x3553, 0x00},
    {0x357d, 0x05},
    {0x357f, 0x05},
    {0x3581, 0x04},
    {0x3583, 0x76},
    {0x3587, 0x01},
    {0x35bb, 0x0e},
    {0x35bc, 0x0e},
    {0x35bd, 0x0e},
    {0x35be, 0x0e},
    {0x35bf, 0x0e},
    {0x366e, 0x00},
    {0x366f, 0x00},
    {0x3670, 0x00},
    {0x3671, 0x00},
    {0x30ee, 0x01},
    {0x3304, 0x32},
    {0x3305, 0x00},
    {0x3306, 0x32},
    {0x3307, 0x00},
    {0x3590, 0x32},
    {0x3591, 0x00},
    {0x3686, 0x32},
    {0x3687, 0x00},

//mode registers 15fps
    {0x3004, 0x05},//shutter and gain
    {0x3005, 0x01},
    {0x3006, 0x00},
    {0x3007, 0x02},

    {0x300E, 0x0},
    {0x300F, 0x0},
    {0x3019, 0x31},
    {0x301A, 0x0},
    
    {0x3031, 0x05},//MSB
    {0x3030, 0x43},
    
    {0x3032, 0x40},
    {0x3033, 0x00},

    {0x3037, 0x00},
    {0x3038, 0x00},
    {0x3039, 0x00},//??
    {0x303a, 0x00},
    {0x303b, 0x00},

    {0x3041, 0x31},
    {0x3042, 0x07}, //terry
    {0x3043, 0x01}, //terry

    {0x306b, 0x05},
    {0x30dd, 0x00},
    {0x30de, 0x00},
    {0x30df, 0x00},
    {0x30e0, 0x00},
    {0x30e1, 0x00},
    {0x30e2, 0x01},
    {0x30e9, 0x01},

    {0x30ee, 0x01},
    {0x30f6, 0x29},//HMAX
    {0x30f7, 0x04},
    {0x30f8, 0xca},//VMAX
    {0x30f9, 0x08},
    {0x30fa, 0x00},//
    {0x3130, 0x86},
    {0x3131, 0x08},
    {0x3132, 0x7E},
    {0x3133, 0x08},
    {0x3342, 0x0A},
    {0x3343, 0x00},
    {0x3344, 0x16},
    {0x3345, 0x00},
    {0x33a6, 0x01},
    {0x3528, 0x0E},
    {0x3554, 0x1F},
    {0x3555, 0x01},
    {0x3556, 0x01},
    {0x3557, 0x01},
    {0x3558, 0x01},
    {0x3559, 0x00},
    {0x355a, 0x00},
    {0x35ba, 0x0E},
    {0x366a, 0x1B},
    {0x366b, 0x1A},
    {0x366c, 0x19},
    {0x366d, 0x17},
    {0x3a41, 0x08},
   // {0x3045, 0x28},//black level
    {0xffff, 0x0f},//delay >=10ms

    //{0x300a, 0x80},//analog gain yc add
   // {0x300b, 0x00},//yc add
    //{0x3012, 0x05},//digital gain

    //{0x300d, 0xff},//MSB
    //{0x300c, 0x02},

    //{0x300c, 0x08},//yc add
    //{0x300d, 0x01},//yc add


    //setting 2
    {0x3000, 0x00},
    {0x303e, 0x02},//??
    {0xffff, 0x0a},//delay >=7ms

   // {0x303c, 0x11},//test pattern
   // {0x303d, 0x0a},

    //setting3

    {0x30f4, 0x00},
    {0x3018, 0xa2},
    {0xffff, 0x60},//delay >=96ms
};

const I2C_ARRAY Sensor_init_table_4lane_30fps[] =
{
//Linear mode, MIPI clock 720M, VMAX 4620
    {0x3000, 0x12},   //standby
    {0x3120, 0xf0},
    {0x3121, 0x00},
    {0x3122, 0x02},
    {0x3123, 0x01},//
    {0x3129, 0x9c},
    {0x312a, 0x02},
    {0x312d, 0x02},
    {0x3ac4, 0x01},//
    {0x310b, 0x00},
    {0x304c, 0x00},
    {0x304d, 0x03},
    {0x331c, 0x1a},
    {0x331d, 0x00},
    {0x3502, 0x02},
    {0x3529, 0x0e},
    {0x352a, 0x0e},
    {0x352b, 0x0e},
    {0x3538, 0x0e},
    {0x3539, 0x0e},
    {0x3553, 0x00},
    {0x357d, 0x05},
    {0x357f, 0x05},
    {0x3581, 0x04},
    {0x3583, 0x76},
    {0x3587, 0x01},
    {0x35bb, 0x0e},
    {0x35bc, 0x0e},
    {0x35bd, 0x0e},
    {0x35be, 0x0e},
    {0x35bf, 0x0e},
    {0x366e, 0x00},
    {0x366f, 0x00},
    {0x3670, 0x00},
    {0x3671, 0x00},
    {0x30ee, 0x01},
    {0x3304, 0x32},
    {0x3305, 0x00},
    {0x3306, 0x32},
    {0x3307, 0x00},
    {0x3590, 0x32},
    {0x3591, 0x00},
    {0x3686, 0x32},
    {0x3687, 0x00},
    {0x3134, 0x77},
    {0x3135, 0x00},
    {0x3136, 0x67},
    {0x3137, 0x00},
    {0x3138, 0x37},
    {0x3139, 0x00},
    {0x313a, 0x37},
    {0x313b, 0x00},
    {0x313c, 0x37},
    {0x313d, 0x00},
    {0x313e, 0xdf},
    {0x313f, 0x00},
    {0x3140, 0x37},
    {0x3141, 0x00},
    {0x3142, 0x2f},
    {0x3143, 0x00},
    {0x3144, 0x0f},
    {0x3145, 0x00},
    {0x3a86, 0x47},
    {0x3a87, 0x00},
//mode registers 15fps
    {0x3004, 0x01},//shutter and gain
    {0x3005, 0x01},
    {0x3006, 0x00},
    {0x3007, 0x02},
    {0x3037, 0x00},
    {0x3038, 0x00},
    {0x3039, 0x00},//??
    {0x303a, 0x00},
    {0x303b, 0x00},
    {0x306b, 0x05},
    {0x30dd, 0x00},
    {0x30de, 0x00},
    {0x30df, 0x00},
    {0x30e0, 0x00},
    {0x30e1, 0x00},
    {0x30e2, 0x01},
    {0x30ee, 0x01},
    {0x30f6, 0x08},//HMAX
    {0x30f7, 0x02},
    {0x30f8, 0x0C},//VMAX
    {0x30f9, 0x12},
    {0x30fa, 0x00},//
    {0x3130, 0x86},
    {0x3131, 0x08},
    {0x3132, 0x7e},
    {0x3133, 0x08},

    {0x3342, 0x0a},
    {0x3343, 0x00},
    {0x3344, 0x16},
    {0x3345, 0x00},

    {0x33a6, 0x01},
    {0x3528, 0x0e},
    {0x3554, 0x1f},
    {0x3555, 0x01},
    {0x3556, 0x01},
    {0x3557, 0x01},
    {0x3558, 0x01},
    {0x3559, 0x00},
    {0x355a, 0x00},
    {0x35ba, 0x0e},
    {0x366a, 0x1b},
    {0x366b, 0x1a},
    {0x366c, 0x19},
    {0x366d, 0x17},
    {0x3a41, 0x08},
   // {0x3045, 0x28},//black level
    {0xffff, 0x0f},//delay >=10ms

    //{0x300a, 0x80},//analog gain yc add
   // {0x300b, 0x00},//yc add
    //{0x3012, 0x05},//digital gain

    //{0x300d, 0xff},//MSB
    //{0x300c, 0x02},

    {0x300c, 0x08},//yc add
    {0x300d, 0x01},//yc add


    //setting 2
    {0x3000, 0x00},
    {0x303e, 0x02},//??
    {0xffff, 0x0a},//delay >=7ms

   // {0x303c, 0x11},//test pattern
   // {0x303d, 0x0a},

    //setting3

    {0x30f4, 0x00},
    {0x3018, 0xa2},
    {0xffff, 0x60},//delay >=96ms
    //{0x312e, 0x01},
    //{0x3aa2, 0x01},

   // {0x3001, 0x06}, // ?��?MIPI 2lane Mode Change
};

const I2C_ARRAY Sensor_init_table_4lane_dol_15fps[] =
{
//DOL mode, MIPI clock 718M, VMAX 2250 @15fps
    {0x3000, 0x12},   //standby
    {0x3120, 0xD5},
    {0x3121, 0x00},
    {0x3122, 0x02},
    {0x3123, 0x01},//
    {0x3129, 0x8A},
    {0x312a, 0x02},
    {0x312d, 0x02},
    {0x3ac4, 0x01},//
    {0x310b, 0x00},
    {0x304c, 0x00},
    {0x304d, 0x03},
    {0x331c, 0x1a},
    {0x331d, 0x00},
    {0x3502, 0x02},
    {0x3529, 0x0e},
    {0x352a, 0x0e},
    {0x352b, 0x0e},
    {0x3538, 0x0e},
    {0x3539, 0x0e},
    {0x3553, 0x00},
    {0x357d, 0x05},
    {0x357f, 0x05},
    {0x3581, 0x04},
    {0x3583, 0x76},
    {0x3587, 0x01},
    {0x35bb, 0x0e},
    {0x35bc, 0x0e},
    {0x35bd, 0x0e},
    {0x35be, 0x0e},
    {0x35bf, 0x0e},
    {0x366e, 0x00},
    {0x366f, 0x00},
    {0x3670, 0x00},
    {0x3671, 0x00},
    {0x30ee, 0x01},
    {0x3304, 0x32},
    {0x3305, 0x00},
    {0x3306, 0x32},
    {0x3307, 0x00},
    {0x3590, 0x32},
    {0x3591, 0x00},
    {0x3686, 0x32},
    {0x3687, 0x00},
    {0x3134, 0x77},
    {0x3135, 0x00},
    {0x3136, 0x67},
    {0x3137, 0x00},
    {0x3138, 0x37},
    {0x3139, 0x00},
    {0x313a, 0x37},
    {0x313b, 0x00},
    {0x313c, 0x37},
    {0x313d, 0x00},
    {0x313e, 0xdf},
    {0x313f, 0x00},
    {0x3140, 0x37},
    {0x3141, 0x00},
    {0x3142, 0x2f},
    {0x3143, 0x00},
    {0x3144, 0x0f},
    {0x3145, 0x00},
    {0x3a86, 0x47},
    {0x3a87, 0x00},
//mode registers 15fps
    {0x3004, 0x05},//shutter and gain
    {0x3005, 0x01},
    {0x3006, 0x00},
    {0x3007, 0x02},

    {0x300E, 0x00},
    {0x300F, 0x00},
    {0x3019, 0x31},
    {0x301A, 0x00},
    {0x3032, 0x40},
    {0x3033, 0x00},

    {0x3037, 0x00},
    {0x3038, 0x00},
    {0x3039, 0x00},//??
    {0x303a, 0x00},
    {0x303b, 0x00},

    {0x3041, 0x31},
    {0x3042, 0xf9},
    {0x3043, 0x01},

    {0x306b, 0x05},
    {0x30dd, 0x00},
    {0x30de, 0x00},
    {0x30df, 0x00},
    {0x30e0, 0x00},
    {0x30e1, 0x00},
    {0x30e2, 0x01},
    {0x30e9, 0x01},

    {0x30ee, 0x01},
    {0x30f6, 0x52},//HMAX
    {0x30f7, 0x08},
    {0x30f8, 0xca},//VMAX
    {0x30f9, 0x08},
    {0x30fa, 0x00},//
    {0x3130, 0x86},
    {0x3131, 0x08},
    {0x3132, 0x7e},
    {0x3133, 0x08},

    {0x3342, 0x0a},
    {0x3343, 0x00},
    {0x3344, 0x16},
    {0x3345, 0x00},

    {0x33a6, 0x01},
    {0x3528, 0x0e},
    {0x3554, 0x1f},
    {0x3555, 0x01},
    {0x3556, 0x01},
    {0x3557, 0x01},
    {0x3558, 0x01},
    {0x3559, 0x00},
    {0x355a, 0x00},
    {0x35ba, 0x0e},
    {0x366a, 0x1b},
    {0x366b, 0x1a},
    {0x366c, 0x19},
    {0x366d, 0x17},
    {0x3a41, 0x08},
   // {0x3045, 0x28},//black level
    {0xffff, 0x0f},//delay >=10ms

    //{0x300a, 0x80},//analog gain yc add
   // {0x300b, 0x00},//yc add
    //{0x3012, 0x05},//digital gain

    //{0x300d, 0xff},//MSB
    //{0x300c, 0x02},

    {0x300c, 0x08},//yc add
    {0x300d, 0x01},//yc add


    //setting 2
    {0x3000, 0x00},
    {0x303e, 0x02},//??
    {0xffff, 0x0a},//delay >=7ms

   // {0x303c, 0x11},//test pattern
   // {0x303d, 0x0a},

    //setting3

    {0x30f4, 0x00},
    {0x3018, 0xa2},
    {0xffff, 0x60},//delay >=96ms
    //{0x312e, 0x01},
    //{0x3aa2, 0x01},

   // {0x3001, 0x06}, // ?��?MIPI 2lane Mode Change
};


const I2C_ARRAY Sensor_init_table_4lane_15fps[] =
{
//Linear mode, MIPI clock 718M, VMAX 4486 @15fps
    {0x3000, 0x12},   //standby
    {0x3120, 0xD5},
    {0x3121, 0x00},
    {0x3122, 0x02},
    {0x3123, 0x01},//
    {0x3129, 0x8A},
    {0x312a, 0x02},
    {0x312d, 0x02},
    {0x3ac4, 0x01},//
    {0x310b, 0x00},
    {0x304c, 0x00},
    {0x304d, 0x03},
    {0x331c, 0x1a},
    {0x331d, 0x00},
    {0x3502, 0x02},
    {0x3529, 0x0e},
    {0x352a, 0x0e},
    {0x352b, 0x0e},
    {0x3538, 0x0e},
    {0x3539, 0x0e},
    {0x3553, 0x00},
    {0x357d, 0x05},
    {0x357f, 0x05},
    {0x3581, 0x04},
    {0x3583, 0x76},
    {0x3587, 0x01},
    {0x35bb, 0x0e},
    {0x35bc, 0x0e},
    {0x35bd, 0x0e},
    {0x35be, 0x0e},
    {0x35bf, 0x0e},
    {0x366e, 0x00},
    {0x366f, 0x00},
    {0x3670, 0x00},
    {0x3671, 0x00},
    {0x30ee, 0x01},
    {0x3304, 0x32},
    {0x3305, 0x00},
    {0x3306, 0x32},
    {0x3307, 0x00},
    {0x3590, 0x32},
    {0x3591, 0x00},
    {0x3686, 0x32},
    {0x3687, 0x00},
    {0x3134, 0x77},
    {0x3135, 0x00},
    {0x3136, 0x67},
    {0x3137, 0x00},
    {0x3138, 0x37},
    {0x3139, 0x00},
    {0x313a, 0x37},
    {0x313b, 0x00},
    {0x313c, 0x37},
    {0x313d, 0x00},
    {0x313e, 0xdf},
    {0x313f, 0x00},
    {0x3140, 0x37},
    {0x3141, 0x00},
    {0x3142, 0x2f},
    {0x3143, 0x00},
    {0x3144, 0x0f},
    {0x3145, 0x00},
    {0x3a86, 0x47},
    {0x3a87, 0x00},
//mode registers 15fps
    {0x3004, 0x01},//shutter and gain
    {0x3005, 0x01},
    {0x3006, 0x00},
    {0x3007, 0x02},

    {0x300E, 0x00},
    {0x300F, 0x00},
    {0x3019, 0x10},
    {0x301A, 0x00},
    {0x3032, 0x08},
    {0x3033, 0x00},

    {0x3037, 0x00},
    {0x3038, 0x00},
    {0x3039, 0x00},//??
    {0x303a, 0x00},
    {0x303b, 0x00},

    {0x3041, 0x30},
    {0x3042, 0x08},
    {0x3043, 0x01},

    {0x306b, 0x05},
    {0x30dd, 0x00},
    {0x30de, 0x00},
    {0x30df, 0x00},
    {0x30e0, 0x00},
    {0x30e1, 0x00},
    {0x30e2, 0x01},
    {0x30e9, 0x00},

    {0x30ee, 0x01},
    {0x30f6, 0x2e},//HMAX
    {0x30f7, 0x04},
    {0x30f8, 0x86},//VMAX
    {0x30f9, 0x11},
    {0x30fa, 0x00},//
    {0x3130, 0x86},
    {0x3131, 0x08},
    {0x3132, 0x7e},
    {0x3133, 0x08},

    {0x3342, 0x0a},
    {0x3343, 0x00},
    {0x3344, 0x16},
    {0x3345, 0x00},

    {0x33a6, 0x01},
    {0x3528, 0x0e},
    {0x3554, 0x1f},
    {0x3555, 0x01},
    {0x3556, 0x01},
    {0x3557, 0x01},
    {0x3558, 0x01},
    {0x3559, 0x00},
    {0x355a, 0x00},
    {0x35ba, 0x0e},
    {0x366a, 0x1b},
    {0x366b, 0x1a},
    {0x366c, 0x19},
    {0x366d, 0x17},
    {0x3a41, 0x08},
   // {0x3045, 0x28},//black level
    {0xffff, 0x0f},//delay >=10ms

    //{0x300a, 0x80},//analog gain yc add
   // {0x300b, 0x00},//yc add
    //{0x3012, 0x05},//digital gain

    //{0x300d, 0xff},//MSB
    //{0x300c, 0x02},

    {0x300c, 0x08},//yc add
    {0x300d, 0x01},//yc add


    //setting 2
    {0x3000, 0x00},
    {0x303e, 0x02},//??
    {0xffff, 0x0a},//delay >=7ms

   // {0x303c, 0x11},//test pattern
   // {0x303d, 0x0a},

    //setting3

    {0x30f4, 0x00},
    {0x3018, 0xa2},
    {0xffff, 0x60},//delay >=96ms
    //{0x312e, 0x01},
    //{0x3aa2, 0x01},

   // {0x3001, 0x06}, // ?��?MIPI 2lane Mode Change
};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
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
    {0x301a, 0x00},//M0F0
    {0x301a, 0x00},//M1F0
    {0x301a, 0x01},//M0F1
    {0x301a, 0x01},//M1F1

};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const static I2C_ARRAY gain_reg[] = {
    {0x300b, 0x00},//high 10~8bit, analog gain
    {0x300a, 0x10},//low byte
    {0x3012, 0x10},//low bit 0~2
};

static int g_sensor_ae_min_gain = 1024;

const static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] = {  //compensate  gain gap
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

const I2C_ARRAY PatternTbl[] = {
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

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

int cus_camsensor_release_handle(ms_cus_sensor *handle);
int cus_camsensor_release_handle_dol_lef(ms_cus_sensor *handle);

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }

    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_UDELAY(20);

    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_UDELAY(20);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_UDELAY(20);

    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);

    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_UDELAY(20);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }

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

static int imx317_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;

    imx317_params *params = (imx317_params *)handle->private_data;


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

static int pCus_init_4lane_linear_30fps(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    int i, cnt;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_30fps);i++)
    {
        if(Sensor_init_table_4lane_30fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_30fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_30fps[i].reg,Sensor_init_table_4lane_30fps[i].data) != SUCCESS)
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

    for(i=0;i< ARRAY_SIZE(Current_Mirror_Flip_Tbl); i++)
    {
        if(SensorReg_Write(params->tCurrent_Mirror_Flip_Tbl[i].reg, params->tCurrent_Mirror_Flip_Tbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }

    }

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

static int pCus_init_4lane_linear_15fps(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    int i, cnt;

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_15fps);i++)
    {
        if(Sensor_init_table_4lane_15fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_15fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_15fps[i].reg, Sensor_init_table_4lane_15fps[i].data) != SUCCESS)
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

    for(i=0;i< ARRAY_SIZE(Current_Mirror_Flip_Tbl); i++)
    {
        if(SensorReg_Write(params->tCurrent_Mirror_Flip_Tbl[i].reg, params->tCurrent_Mirror_Flip_Tbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }

    }

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

static int pCus_init_4lane_dol_30fps(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    int i,cnt=0;

#if 0
    if (pCus_CheckSonyProductID(handle)) {
        return FAIL;
    }
#endif

    //UartSendTrace("IMX317 Sensor_init_table_HDR_DOL_4lane\n");
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_dol_30fps);i++)
    {
        if(Sensor_init_table_4lane_dol_30fps[i].reg==0xffff)
        {
            //MsSleep(RTK_MS_TO_TICK(1));//usleep(1000*Sensor_init_table_HDR_DOL_4lane[i].data);
            SENSOR_MSLEEP(Sensor_init_table_4lane_dol_30fps[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("reg =  %x, data = %x\n", Sensor_init_table_4lane_dol_30fps[i].reg, Sensor_init_table_4lane_dol_30fps[i].data);
            while(SensorReg_Write(Sensor_init_table_4lane_dol_30fps[i].reg, Sensor_init_table_4lane_dol_30fps[i].data) != SUCCESS)
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
            //UartSendTrace("IMX317 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data, sen_data);
        }
    }
#if 0
    if (pCus_CheckSonyProductID(handle)) {
        return FAIL;
    }
#endif
    params->tVts_reg[0].data = (params->expo.vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
    //MSG("[%s:%d]Sensor init success!!\n", __FUNCTION__, __LINE__);
    return SUCCESS;
}

static int pCus_init_4lane_dol_15fps(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    int i,cnt=0;

#if 0
    if (pCus_CheckSonyProductID(handle)) {
        return FAIL;
    }
#endif

    //UartSendTrace("IMX317 Sensor_init_table_HDR_DOL_4lane\n");
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_dol_15fps);i++)
    {
        if(Sensor_init_table_4lane_dol_15fps[i].reg==0xffff)
        {
            //MsSleep(RTK_MS_TO_TICK(1));//usleep(1000*Sensor_init_table_HDR_DOL_4lane[i].data);
            SENSOR_MSLEEP(Sensor_init_table_4lane_dol_15fps[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("reg =  %x, data = %x\n", Sensor_init_table_4lane_dol_15fps[i].reg, Sensor_init_table_4lane_dol_15fps[i].data);
            while(SensorReg_Write(Sensor_init_table_4lane_dol_15fps[i].reg, Sensor_init_table_4lane_dol_15fps[i].data) != SUCCESS)
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
            //UartSendTrace("IMX317 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HDR_DOL_4lane[i].reg, Sensor_init_table_HDR_DOL_4lane[i].data, sen_data);
        }
    }
#if 0
    if (pCus_CheckSonyProductID(handle)) {
        return FAIL;
    }
#endif
    params->tVts_reg[0].data = (params->expo.vts >> 16) & 0x000f;
    params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
    //MSG("[%s:%d]Sensor init success!!\n", __FUNCTION__, __LINE__);
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
    imx317_params *params = (imx317_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }

    switch (res_idx) {
        case 0: //"3840x2160@30fps"
            handle->video_res_supported.ulcur_res = res_idx;
            handle->pCus_sensor_init = pCus_init_4lane_linear_30fps;
            handle->mclk = CUS_CMU_CLK_24MHZ;
            params->expo.vts = 4620;
            params->expo.fps = handle->video_res_supported.res[res_idx].max_fps;
            params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);
            break;
        case 1: //"3840x2160@15fps"
            handle->video_res_supported.ulcur_res = res_idx;
            handle->pCus_sensor_init = pCus_init_4lane_linear_15fps;
            handle->mclk = CUS_CMU_CLK_27MHZ;
            params->expo.vts = 4486;
            params->expo.fps = handle->video_res_supported.res[res_idx].max_fps;
            params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_SetVideoRes_HDR_DOL(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;
    imx317_params *params = (imx317_params *)handle->private_data;

    if (res_idx >= num_res) {
        return FAIL;
    }

    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = res_idx;
            handle->pCus_sensor_init = pCus_init_4lane_dol_30fps;
            params->expo.vts = 2250;
            params->expo.fps = handle->video_res_supported.res[res_idx].max_fps;
            params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);
            break;
        case 1:
            handle->video_res_supported.ulcur_res = res_idx;
            handle->pCus_sensor_init = pCus_init_4lane_dol_15fps;
            params->expo.vts = 2250;
            params->expo.fps = handle->video_res_supported.res[res_idx].max_fps;
            params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);
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

    imx317_params *params = (imx317_params *)handle->private_data;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->tCurrent_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                params->tCurrent_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->tCurrent_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                params->tCurrent_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //  handle->bayer_id=	CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->tCurrent_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                params->tCurrent_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            // handle->bayer_id=	CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->tCurrent_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                params->tCurrent_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            // handle->bayer_id=	CUS_BAYER_GR;
            break;

        default :
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                params->tCurrent_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                params->tCurrent_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //  handle->bayer_id=	CUS_BAYER_BG;
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_max_fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_max_fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts =  (vts_max_fps*max_fps)/fps;
        params->dirty = true; //reg need to update = true;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_max_fps*(max_fps*1000))/fps;
        params->dirty = true; //reg need to update = true;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    pCus_SetAEUSecs(handle, params->expo.expo_lef_us);

    return SUCCESS;
}

static int pCus_GetFPS_HDR_DOL_SEF(ms_cus_sensor *handle)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_max_fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_max_fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS_HDR_DOL_SEF(ms_cus_sensor *handle, u32 fps)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts =  (vts_max_fps*max_fps)/fps;
        params->dirty = true; //reg need to update = true;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_max_fps*(max_fps*1000))/fps;
        params->dirty = true; //reg need to update = true;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    pCus_SetAEUSecsHDR_DOL_SEF(handle, params->expo.expo_sef_us);

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
    imx317_params *params = (imx317_params *)handle->private_data;

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
    imx317_params *params = (imx317_params *)handle->private_data;
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

/*
                if(params->change){

                    // sensor_if1->SetSkipFrame(handle,3);
                     params->change = false;

                    }
*/
                //SensorReg_Write(0x3001,1);
             //   SensorRegArrayW((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
             //   SensorRegArrayW((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
             //   SensorRegArrayW((I2C_ARRAY*)vts_reg, ARRAY_SIZE(vts_reg));
                //SensorReg_Write(0x3001,0);
               // printf("0x3009=0x%x,0x3014=0x%x,0x3016=0x%x,0x3020=0x%x,0x3021=0x%x\n", gain_reg[1].data,gain_reg[0].data,gain_reg[2].data,expo_reg[2].data,expo_reg[1].data);
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
   // imx317_params *params = (imx317_params *)handle->private_data;
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
    imx317_params *params = (imx317_params *)handle->private_data;

    params->expo.expo_lef_us = us;

    // us = 1000000/30;
    //lines = us/Preview_line_period_HDR_DOL;
    lines = (1000 * us) / params->expo.us_per_line;
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

static int pCus_GetAEGain_hdr_dol_lef(ms_cus_sensor *handle, u32* gain)
{
    *gain = 0;
    return SUCCESS;
}

static int pCus_SetAEUSecsHDR_DOL_SEF(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    u32 rhs1 = 0, shr_dol1 = 0, shr_dol2 = 0;
    imx317_params *params = (imx317_params *)handle->private_data;

    params->expo.expo_sef_us = us;
    // us = 1000000/30;
    //lines = us/Preview_line_period_HDR_DOL;
    lines = (1000 * us) / params->expo.us_per_line;
    //vts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);
    vts = params->expo.vts;
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

static int pCus_GetAEUSecs_hdr_dol_lef(ms_cus_sensor *handle, u32 *us)
{
    *us = 0;
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    //int rc;
    u32 lines = 0;
    //int rc = SensorRegArrayR((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));
    imx317_params *params = (imx317_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<0;
   // lines >>= 4;
   // *us = (lines+dummy) * params->expo.us_per_line;
    //*us = lines;//(lines*Preview_line_period);
    *us = lines * params->expo.us_per_line /1000;
    SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, lines, *us);
    //return rc;
    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    imx317_params *params = (imx317_params *)handle->private_data;

    params->expo.expo_lef_us = us;

    lines=(1000*us)/params->expo.us_per_line;

    if (lines >params->expo.vts-12) {
        vts = lines +12;
    }
    else
        vts = params->expo.vts;

#if 0
    // lines=us/Preview_line_period;
    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
    us,
    lines,
    params->expo.vts
    );
#endif
    lines = vts - lines;

    params->tExpo_reg[0].data = (u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[1].data = (u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[0].data = (u16)((vts >> 16) & 0x000f);
    params->tVts_reg[1].data = (u16)((vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((vts >> 0) & 0x00ff);
    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
 imx317_params *params = (imx317_params *)handle->private_data;
    //u16  temp_gain;
    //double  temp_gain_double;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] get gain/regL/regH (1024=1X)= %d/0x%x/0x%x\n", __FUNCTION__, *gain,gain_reg[0].data,gain_reg[1].data);

    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {
    imx317_params *params = (imx317_params *)handle->private_data;
    u16 gain16;
    //double gain_double;

    params->expo.final_gain = gain;
    if(gain<1024)
        gain=1024;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

    gain16=(u16)(2048-((2048*1024)/gain));
    //gain16=(u16)(gain_double*10);

    params->tGain_reg[0].data=(gain16>>8)&0x07;//MSB
    params->tGain_reg[1].data=gain16&0xff;//LSB

    SENSOR_DMSG("[%s] set gain/reg0/reg1=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
    return SUCCESS;

}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 i;
    //double gain_double;
    const CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u16 gain16;

    params->expo.final_gain = gain;
    if(gain<1024)
        gain=1024;
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;

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
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int pCus_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    info->max = 1000000000/handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    info->min =  params->expo.us_per_line * 4;
    info->step = params->expo.us_per_line;
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
    imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);
    u32 tVts = (params->tVts_reg[0].data << 16) | (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_max_fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_max_fps*max_fps)/tVts;

    return params->expo.preview_fps;
}
static int pCus_SetFPS_hdr_dol_lef(ms_cus_sensor *handle, u32 fps)
{
	imx317_params *params = (imx317_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    u32 vts_max_fps = 1000000000 /(max_fps * params->expo.us_per_line);

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts =  (vts_max_fps*max_fps)/fps;
        params->dirty = true; //reg need to update = true;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_max_fps*(max_fps*1000))/fps;
        params->dirty = true; //reg need to update = true;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=30;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    pCus_SetAEUSecsHDR_DOL_LEF(handle, params->expo.expo_lef_us);

    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_lef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (params->expo.us_per_line * 4);
    info->step = params->expo.us_per_line;
    return SUCCESS;
}
static int pCus_GetShutterInfo_hdr_dol_sef(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    imx317_params *params = (imx317_params *)handle->private_data;
    info->max = params->expo.us_per_line * params->max_rhs1;
    info->min = (params->expo.us_per_line * 2);
    info->step = params->expo.us_per_line;
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
    imx317_params *params = (imx317_params *)handle->private_data;

    for(i=0,j=0;i< num ;i++,j+=2){
        params->tGain_gap_compensate[i].gain=pArray[i].gain;
        params->tGain_gap_compensate[i].offset=pArray[i].offset;
    }

    SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}
int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx317_params *params;

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
    params = (imx317_params *)handle->private_data;

    memcpy(params->tCurrent_Mirror_Flip_Tbl, Current_Mirror_Flip_Tbl, sizeof(Current_Mirror_Flip_Tbl));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_gap_compensate, gain_gap_compensate, sizeof(gain_gap_compensate));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tExpo_shr_dol1_reg, expo_shr_dol1_reg, sizeof(expo_shr_dol1_reg));
    memcpy(params->tExpo_rhs1_reg, expo_rhs1_reg, sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol2_reg, expo_shr_dol2_reg, sizeof(expo_shr_dol2_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tPatternTbl, PatternTbl, sizeof(PatternTbl));
    ////////////////////////////////////
    //    sensor model ID                                   //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX317_MIPI");

    ////////////////////////////////////
    //    sensor interface info                             //
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
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 0xF18;
    handle->video_res_supported.res[0].nOutputHeight= 0x87E;
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@30fps");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= 15;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 0xF18;
    handle->video_res_supported.res[1].nOutputHeight= 0x87E;
    sprintf(handle->video_res_supported.res[1].strResDesc, "3840x2160@15fps");

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = CUS_CMU_CLK_24MHZ;

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

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_4lane_linear_30fps;
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
    handle->pCus_sensor_SetPatternMode = imx317_SetPatternMode;
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

    params->expo.vts = 4620;
    params->expo.fps = handle->video_res_supported.res[0].max_fps;
    params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);

    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_sef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx317_params *params = NULL;

    cus_camsensor_init_handle_linear(drv_handle);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (imx317_params *)handle->private_data;

    memcpy(params->tCurrent_Mirror_Flip_Tbl, Current_Mirror_Flip_Tbl, sizeof(Current_Mirror_Flip_Tbl));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_gap_compensate, gain_gap_compensate, sizeof(gain_gap_compensate));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tExpo_shr_dol1_reg, expo_shr_dol1_reg, sizeof(expo_shr_dol1_reg));
    memcpy(params->tExpo_rhs1_reg, expo_rhs1_reg, sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol2_reg, expo_shr_dol2_reg, sizeof(expo_shr_dol2_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tPatternTbl, PatternTbl, sizeof(PatternTbl));

    sprintf(handle->model_id,"IMX317_MIPI_HDR_SEF");

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
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@30fps_HDR");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= 15;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 0xF18;
    handle->video_res_supported.res[1].nOutputHeight= 0x881;
    sprintf(handle->video_res_supported.res[1].strResDesc, "3840x2160@15fps_HDR");

    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes_HDR_DOL;
    handle->mclk                        = CUS_CMU_CLK_27MHZ;

#if (SENSOR_MIPI_LANE_NUM == 2)
#endif
#if (SENSOR_MIPI_LANE_NUM == 4)
    handle->pCus_sensor_init        = pCus_init_4lane_dol_30fps;
#endif
    handle->pCus_sensor_GetFPS          = pCus_GetFPS_HDR_DOL_SEF; //TBD
    handle->pCus_sensor_SetFPS          = pCus_SetFPS_HDR_DOL_SEF; //TBD

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotifyHDR_DOL_SEF;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_SEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_sef;

    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1; //Short frame

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 2;

    params->expo.vts = 2250;
    params->expo.fps = handle->video_res_supported.res[0].max_fps;
    params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);

    return SUCCESS;
}

int cus_camsensor_init_handle_hdr_dol_lef(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx317_params *params;
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);

    //SENSOR_DMSG("----~~~~~~~~~~~~-----cus_camsensor_init_handle_hdr_dol_lef----~~~~~~~~~~~~~~`-\n");

    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (imx317_params *)handle->private_data;

    memcpy(params->tCurrent_Mirror_Flip_Tbl, Current_Mirror_Flip_Tbl, sizeof(Current_Mirror_Flip_Tbl));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tGain_gap_compensate, gain_gap_compensate, sizeof(gain_gap_compensate));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tExpo_shr_dol1_reg, expo_shr_dol1_reg, sizeof(expo_shr_dol1_reg));
    memcpy(params->tExpo_rhs1_reg, expo_rhs1_reg, sizeof(expo_rhs1_reg));
    memcpy(params->tExpo_shr_dol2_reg, expo_shr_dol2_reg, sizeof(expo_shr_dol2_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tPatternTbl, PatternTbl, sizeof(PatternTbl));


    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX317_MIPI_HDR_LEF");

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
    sprintf(handle->video_res_supported.res[0].strResDesc, "3840x2160@30fps_HDR");

    handle->video_res_supported.num_res = 2;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[1].width = Preview_WIDTH;
    handle->video_res_supported.res[1].height = Preview_HEIGHT;
    handle->video_res_supported.res[1].max_fps= 15;
    handle->video_res_supported.res[1].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth= 0xF18;
    handle->video_res_supported.res[1].nOutputHeight= 0x8AA;
    sprintf(handle->video_res_supported.res[1].strResDesc, "3840x2160@15fps_HDR");

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //320000;

    // mclk
    handle->mclk                        = CUS_CMU_CLK_27MHZ;

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
    handle->ae_shutter_ctrl_num = 2;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;

    //LOGD("[%s:%d]\n", __FUNCTION__, __LINE__);
    //handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_release     = cus_camsensor_release_handle_dol_lef;
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
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs_hdr_dol_lef;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecsHDR_DOL_LEF;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain_hdr_dol_lef;
    //handle->pCus_sensor_SetAEGain       = pCus_SetAEGain_hdr_dol_lef;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    //handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs_hdr_dol_lef;

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal_hdr_dol_lef;
    handle->pCus_sensor_setCaliData_gain_linearity= pCus_setCaliData_gain_linearity_hdr_dol_lef;
    handle->pCus_sensor_GetShutterInfo = pCus_GetShutterInfo_hdr_dol_lef;

    params->expo.vts = 2250;
    params->expo.fps = handle->video_res_supported.res[0].max_fps;
    params->expo.us_per_line = 1000000000 / (params->expo.vts * params->expo.fps);

    params->dirty = false;

    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
#if 0
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
    //sensor_if->PCLK(NULL,CUS_PCLK_OFF);
    //sensor_if->SetCSI_Clk(handle,CUS_CSI_CLK_DISABLE);

    imx317_params *params = (imx317_params *)handle->private_data;

    if (handle && handle->private_data) {
        SENSOR_DMSG("[%s] release handle, handle %x, private data %x",
                __FUNCTION__,
                (int)handle,
                (int)handle->private_data);

        CamOsMemRelease(params->tCurrent_Mirror_Flip_Tbl);
        CamOsMemRelease(params->tGain_reg);
        CamOsMemRelease(params->tGain_gap_compensate);
        CamOsMemRelease(params->tExpo_reg);
        CamOsMemRelease(params->tExpo_shr_dol1_reg);
        CamOsMemRelease(params->tExpo_rhs1_reg);
        CamOsMemRelease(params->tExpo_shr_dol2_reg);
        CamOsMemRelease(params->tVts_reg);
        CamOsMemRelease(params->tPatternTbl);

        CamOsMemRelease(handle->private_data);
        handle->private_data = NULL;
    }
#endif
    return SUCCESS;
}

int cus_camsensor_release_handle_dol_lef(ms_cus_sensor *handle)
{
    return SUCCESS;
}


SENSOR_DRV_ENTRY_IMPL_END_EX(  IMX317_HDR,
                            cus_camsensor_init_handle_linear,
                            cus_camsensor_init_handle_hdr_dol_sef,
                            cus_camsensor_init_handle_hdr_dol_lef,
                            imx317_params
                            );
