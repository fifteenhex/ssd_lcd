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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(IMX477_HDR);

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

#define DEBUG_INFO        0

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
#define SENSOR_MIPI_LANE_NUM        4//(lane_num)          //IMX477 Linear mode supports MIPI 4-Lane
//#define SENSOR_MIPI_LANE_NUM_DOL    (hdr_lane_num)      //IMX477 DOL mode supports MIPI 4-Lane
//#define SENSOR_MIPI_HDR_MODE        (0) //0: Non-HDR mode. 1:Sony DOL mode

#define SENSOR_ISP_TYPE             ISP_EXT             //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER    //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_MIPI  //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE      PACKET_HEADER_EDGE1
#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9098  //CFG
#define SENSOR_BAYERID              CUS_BAYER_RG         //0h: CUS_BAYER_RG, 1h: CUS_BAYER_GR, 2h: CUS_BAYER_BG, 3h: CUS_BAYER_GB
#define SENSOR_RGBIRID              CUS_RGBIR_NONE
#define SENSOR_ORIT                 CUS_ORIT_M0F0        //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER              CUS_SEN_YCODR_YC     //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define long_packet_type_enable     0x00 //UD1~UD8 (user define)

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_27MHZ    //CUS_CMU_CLK_24MHZ //CUS_CMU_CLK_37P125MHZ//CUS_CMU_CLK_27MHZ//CUS_CMU_CLK_12MHZ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x34                //I2C slave address
#define SENSOR_I2C_SPEED             300000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL           1                 //I2C Channel
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
//define SENSOR_ID

#undef SENSOR_NAME
#define SENSOR_NAME                   IMX477

////////////////////////////////////
// Image Info                     //
////////////////////////////////////
static struct {     // LINEAR
    // Modify it based on number of support resolution
    /*enum {LINEAR_RES_1 = 0, LINEAR_RES_2, LINEAR_RES_END}mode;*/
    enum {LINEAR_RES_1 = 0, LINEAR_RES_END}mode;
    // Sensor Output Image info
    struct _senout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _sensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _senstr{
        const char* strResDesc;
    }senstr;
}imx477_mipi_linear[] = {
    {LINEAR_RES_1, {4056, 3040, 3, 15}, {0, 0, 3840, 3040}, {"3840x3040@15fps"}},
    //{LINEAR_RES_2, {3280, 2456, 3, 15}, {0, 0, 3200, 2400}, {"3200x2400@30fps"}},
};

#if 0
static struct {     // HDR
    // Modify it based on number of support resolution
    enum {HDR_RES_1 = 0, HDR_RES_2, HDR_RES_3, HDR_RES_END}mode;
    // Sensor Output Image info
    struct _hsenout{
        s32 width, height, min_fps, max_fps;
    }senout;
    // VIF Get Image Info
    struct _hsensif{
        s32 crop_start_X, crop_start_y, preview_w, preview_h;
    }senif;
    // Show resolution string
    struct _hsenstr{
        const char* strResDesc;
    }senstr;
}imx477_mipi_hdr[] = {
    {HDR_RES_1, {2592, 1944, 3, 25}, {4, 0, 2592, 1944}, {"2592x1944@25fps_HDR"}}, // Modify it
    {HDR_RES_2, {2592, 1944, 3, 20}, {4, 0, 2592, 1944}, {"2592x1944@20fps_HDR"}}, // Modify it
    {HDR_RES_3, {2592, 1944, 3, 30}, {4, 0, 2592, 1944}, {"2592x1944@30fps_HDR"}}, // Modify it
};
#endif


u32 Preview_line_period;
u32 vts_30fps;
u32 Preview_MAX_FPS;


//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)
////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (978 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)

////////////////////////////////////
// Mirror-Flip Info               //
////////////////////////////////////
#define MIRROR_FLIP_REG                             0x0101

#define SENSOR_NOR                                  0x0
#define MIRROR_EN                                   0x1 << (0)
#define FLIP_EN                                     0x1 << (1)

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

//static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);

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
        u32 expo_lef_us;
        u32 expo_sef_us;
    } expo;
    u32 max_rhs1;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orien_dirty;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[2];
    //I2C_ARRAY tExpo_shr_dol1_reg[3];
    //I2C_ARRAY tExpo_rhs1_reg[3];
    //I2C_ARRAY tGain_hdr_dol_lef_reg[2];
    //I2C_ARRAY tGain_hdr_dol_sef_reg[2];
} imx477_params;
// set sensor ID address and data,

const static I2C_ARRAY Sensor_id_table[] =
{
    {0x3003, 0x00},      // {address of ID, ID },
    {0x3033, 0x00}
};

const static I2C_ARRAY Sensor_init_table_4lane_12m15fps[] =
{
    /*"IMX477LQN All-pixel scan CSI-2_4lane AD:10bit Output:10bit 27MHz Master Mode 15fps"*/
    {0x0136,0x1B},  //EXCK_FREQ[15:8]
    {0x0137,0x00},  //EXCK_FREQ[7:0]

    {0x0808,0x02},
    {0xE07A,0x01},
    {0xE000,0x00},
    {0x4AE9,0x18},
    {0x4AEA,0x08},
    {0xF61C,0x04},
    {0xF61E,0x04},
    {0x4AE9,0x21},
    {0x4AEA,0x80},
    {0x38A8,0x1F},
    {0x38A9,0xFF},
    {0x38AA,0x1F},
    {0x38AB,0xFF},
    {0x420B,0x01},
    {0x55D4,0x00},
    {0x55D5,0x00},
    {0x55D6,0x07},
    {0x55D7,0xFF},
    {0x55E8,0x07},
    {0x55E9,0xFF},
    {0x55EA,0x00},
    {0x55EB,0x00},
    {0x574C,0x07},
    {0x574D,0xFF},
    {0x574E,0x00},
    {0x574F,0x00},
    {0x5754,0x00},
    {0x5755,0x00},
    {0x5756,0x07},
    {0x5757,0xFF},
    {0x5973,0x04},
    {0x5974,0x01},
    {0x5D13,0xC3},
    {0x5D14,0x58},
    {0x5D15,0xA3},
    {0x5D16,0x1D},
    {0x5D17,0x65},
    {0x5D18,0x8C},
    {0x5D1A,0x06},
    {0x5D1B,0xA9},
    {0x5D1C,0x45},
    {0x5D1D,0x3A},
    {0x5D1E,0xAB},
    {0x5D1F,0x15},
    {0x5D21,0x0E},
    {0x5D22,0x52},
    {0x5D23,0xAA},
    {0x5D24,0x7D},
    {0x5D25,0x57},
    {0x5D26,0xA8},
    {0x5D37,0x5A},
    {0x5D38,0x5A},
    {0x5D77,0x7F},
    {0x7B7C,0x00},
    {0x7B7D,0x00},
    {0x8D1F,0x00},
    {0x8D27,0x00},
    {0x9004,0x03},
    {0x9200,0x50},
    {0x9201,0x6C},
    {0x9202,0x71},
    {0x9203,0x00},
    {0x9204,0x71},
    {0x9205,0x01},
    {0x9371,0x6A},
    {0x9373,0x6A},
    {0x9375,0x64},
    {0x990C,0x00},
    {0x990D,0x08},
    {0x9956,0x8C},
    {0x9957,0x64},
    {0x9958,0x50},
    {0x9A48,0x06},
    {0x9A49,0x06},
    {0x9A4A,0x06},
    {0x9A4B,0x06},
    {0x9A4C,0x06},
    {0x9A4D,0x06},
    {0xA001,0x0A},
    {0xA003,0x0A},
    {0xA005,0x0A},
    {0xA006,0x01},
    {0xA007,0xC0},
    {0xA009,0xC0},

    // MIPI Setting
    {0x0112,0x0A},
    {0x0113,0x0A},
    {0x0114,0x03},
    //expo
    {0x0202,0x05},  // bit8-15
    {0x0203,0xAD},  // bit0-7
    //Frame Horizontal Clock Count
    {0x0342,0x2E},
    {0x0343,0xE0},

    //Frame Vertical Clock Count
    {0x0340,0x0C},
    {0x0341,0x14},

    //Visible Size
    {0x0344,0x00},
    {0x0345,0x00},
    {0x0346,0x00},
    {0x0347,0x00},
    {0x0348,0x0F},
    {0x0349,0xD7},
    {0x034A,0x0B},
    {0x034B,0xDF},

    //Mode Setting
    {0x00E3, 0x00},
    {0x00E4, 0x00},
    {0x00FC, 0x0A},
    {0x00FD, 0x0A},
    {0x00FE, 0x0A},
    {0x00FF, 0x0A},
    {0x0220, 0x00},
    {0x0221, 0x11},
    {0x0381, 0x01},
    {0x0383, 0x01},
    {0x0385, 0x01},
    {0x0387, 0x01},
    {0x0900, 0x00},
    {0x0901, 0x11},
    {0x0902, 0x02},
    {0x3140, 0x02},
    {0x3C00, 0x00},
    {0x3C01, 0x03},
    {0x3C02, 0xDC},
    {0x3F0D, 0x00},
    {0x5748, 0x07},
    {0x5749, 0xFF},
    {0x574A, 0x00},
    {0x574B, 0x00},
    {0x7B75, 0x0E},
    {0x7B76, 0x09},
    {0x7B77, 0x0C},
    {0x7B78, 0x06},
    {0x7B79, 0x3B},
    {0x7B53, 0x01},
    {0x9369, 0x5A},
    {0x936B, 0x55},
    {0x936D, 0x28},
    {0x9304, 0x03},
    {0x9305, 0x00},
    {0x9E9A, 0x2F},
    {0x9E9B, 0x2F},
    {0x9E9C, 0x2F},
    {0x9E9D, 0x00},
    {0x9E9E, 0x00},
    {0x9E9F, 0x00},
    {0xA2A9, 0x60},
    {0xA2B7, 0x00},

    //Digital Crop & Scaling
    {0x0401, 0x00},
    {0x0404, 0x00},
    {0x0405, 0x10},
    {0x0408, 0x00},
    {0x0409, 0x00},
    {0x040A, 0x00},
    {0x040B, 0x00},
    {0x040C, 0x0F},
    {0x040D, 0xD8},
    {0x040E, 0x0B},
    {0x040F, 0xE0},

    //Output Crop
    {0x034C, 0x0F},
    {0x034D, 0xD8},
    {0x034E, 0x0B},
    {0x034F, 0xE0},

    //Clock Setting
    {0x0301, 0x05},
    {0x0303, 0x02},
    {0x0305, 0x03},
    {0x0306, 0x00},
    {0x0307, 0x9B},
    {0x0309, 0x0A},
    {0x030B, 0x02},
    {0x030D, 0x02},
    {0x030E, 0x01},
    {0x030F, 0x5E},
    {0x0310, 0x00},
    {0x0820, 0x0A},
    {0x0821, 0xE6},
    {0x0822, 0x00},
    {0x0823, 0x00},

    //Global Timing Setting
    {0x080A, 0x00},
    {0x080B, 0x5F},
    {0x080C, 0x00},
    {0x080D, 0x2F},
    {0x080E, 0x00},
    {0x080F, 0x47},
    {0x0810, 0x00},
    {0x0811, 0x3F},
    {0x0812, 0x00},
    {0x0813, 0x3F},
    {0x0814, 0x00},
    {0x0815, 0x2F},
    {0x0816, 0x00},
    {0x0817, 0xAF},
    {0x0818, 0x00},
    {0x0819, 0x27},
    {0xE04C, 0x00},
    {0xE04D, 0x4F},
    {0xE04E, 0x00},
    {0xE04F, 0x1F},

    //Output Data Select Setting
    {0x3E20, 0x01},
    {0x3E37, 0x01},

    //PowerSave Setting
    {0x3F50, 0x00},
    {0x3F56, 0x02},
    {0x3F57, 0x45},

    //Streaming setting
    {0x0100, 0x00},
    {0xFFFF, 0xC8},
    {0x0100, 0x01},
};

static I2C_ARRAY PatternTbl[] = {
    {0x0000, 0x00}, //colorbar pattern , bit 0 to enable
};

const static I2C_ARRAY expo_reg[] =
{ //SEL
    {0x0202, 0x00},  // bit8-15
    {0x0203, 0x00},  // bit0-7
};

const static I2C_ARRAY gain_reg[] = {
    {0x0204, 0x00}, // bit0-1(8-10)
    {0x0205, 0x00}, // bit0-7 low
};

const static I2C_ARRAY vts_reg[] = {
    {0x0340, 0x0C},  // bit0-7(8-15)
    {0x0341, 0x14},  // bit0-7
};

//static int g_sensor_ae_min_gain = 1024;
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

/////////// function definition ///////////////////
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////

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
 #if 0
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
#endif

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    //Sensor board PWDN/RST Pull Low
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);
    }
    SENSOR_UDELAY(100);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    //Sensor board RST Pull High after PWDN Enable
    SENSOR_UDELAY(1);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_UDELAY(1);
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_MDELAY(1);
    SENSOR_DMSG("Sensor Power On finished\n");
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
    handle->orient = SENSOR_ORIT;

    return SUCCESS;
}

/////////////////// Check Sensor Product ID /////////////////////////
#if 0
static int pCus_CheckSensorProductID(ms_cus_sensor *handle)
{
    u16 sen_id_msb, sen_id_lsb, sen_data;

    /* Read Product ID */
    SensorReg_Read(0x3f12, &sen_id_lsb);
    SensorReg_Read(0x3f13, &sen_id_msb);//CHIP_ID_r3F13
    sen_data = ((sen_id_lsb & 0x0F) << 8) | (sen_id_lsb & 0xF0) | (sen_id_msb & 0x0F);
#if 0
    if (sen_data != CHIP_ID) {
        printk("[***ERROR***]Check Product ID Fail: 0x%x\n", sen_data);
        return FAIL;
    }
#endif
    return SUCCESS;
}
#endif
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

      if(/* SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == */SUCCESS) //read sensor ID from I2C
          break;
      else
          SENSOR_MSLEEP(1);
    }

    //convert sensor id to u32 format
    for(i=0;i<table_length;++i)
    {
      if( id_from_sensor[i].data != Sensor_id_table[i].data )
        return FAIL;
      *id = id_from_sensor[i].data;
    }

    SENSOR_DMSG("[%s]IMX477 sensor ,Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int imx477_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;
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
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }
    return SUCCESS;
}

static int pCus_init_mipi4lane_12m15fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0;

    SENSOR_DMSG("\n[%s]", __FUNCTION__);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_4lane_12m15fps);i++)
    {
        if(Sensor_init_table_4lane_12m15fps[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_4lane_12m15fps[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_4lane_12m15fps[i].reg,Sensor_init_table_4lane_12m15fps[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_4lane_12m15fps[i].reg, Sensor_init_table_4lane_12m15fps[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_4lane_12m15fps[i].reg, &sen_data );
            if(Sensor_init_table_4lane_12m15fps[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_4lane_12m15fps[i].reg);
                //printk("IMX477 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_4lane_12m15fps[i].reg, Sensor_init_table_4lane_12m15fps[i].data, sen_data);
#endif
        }
    }
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
    imx477_params *params = (imx477_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init_mipi4lane_12m15fps_linear;
            vts_30fps = 3092;
            Preview_MAX_FPS = imx477_mipi_linear[handle->video_res_supported.ulcur_res].senout.max_fps;;
            Preview_line_period  = 20633;
            break;
        case 1:
            break;
        default:
            break;
    }
    params->expo.vts = vts_30fps;
    params->expo.fps = Preview_MAX_FPS;

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    s16 sen_data;
    //Read SENSOR MIRROR-FLIP STATUS
    SensorReg_Read(MIRROR_FLIP_REG, &sen_data);

    switch(sen_data)
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
    imx477_params *params = (imx477_params *)handle->private_data;

    handle->orient = orit;
    params->orien_dirty = true;

    return SUCCESS;
}

static int DoOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    s16 sen_data;
    //Read SENSOR MIRROR-FLIP STATUS
    SensorReg_Read(MIRROR_FLIP_REG, (void*)&sen_data);
    sen_data &= ~(MIRROR_EN | FLIP_EN);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            //sen_data |= SENSOR_NOR;
            handle->orient = CUS_ORIT_M0F0;
            break;
        case CUS_ORIT_M1F0:
            sen_data |= MIRROR_EN;
            handle->orient = CUS_ORIT_M1F0;
            break;
        case CUS_ORIT_M0F1:
            sen_data |= FLIP_EN;
            handle->orient = CUS_ORIT_M0F1;
            break;
        case CUS_ORIT_M1F1:
            sen_data |= MIRROR_EN;
            sen_data |= FLIP_EN;
            handle->orient = CUS_ORIT_M1F1;
            break;
        default :
            handle->orient = CUS_ORIT_M0F0;
            break;
    }
    //Write SENSOR MIRROR-FLIP STATUS
    SensorReg_Write(MIRROR_FLIP_REG, sen_data);

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    imx477_params *params = (imx477_params *)handle->private_data;
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
    u32 vts = 0;
    imx477_params *params = (imx477_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000) + fps * 500 )/ (fps * 1000);
    }else if((fps>=(min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts = (vts_30fps*(max_fps*1000) + (fps>>1))/fps;
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=25;
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.expo_lines > params->expo.vts -2){
        vts = params->expo.expo_lines + 8;
    }else{
        vts = params->expo.vts;
    }
    params->expo.vts = vts;
    pCus_SetAEUSecs(handle, params->expo.expo_lef_us);

    params->dirty = true;
    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    imx477_params *params = (imx477_params *)handle->private_data;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             //SensorReg_Write(0x0104,1);
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty) {
                SensorReg_Write(0x0104,1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));

                if(params->orien_dirty) {
                    DoOrien(handle, handle->orient);
                    params->orien_dirty = false;
                }
                SensorReg_Write(0x0104,0);
                params->dirty = false;
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    u32 lines = 0;
    imx477_params *params = (imx477_params *)handle->private_data;

    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<0;

    *us = (lines*Preview_line_period)/1000;
    SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0,activeline = 0;
    imx477_params *params = (imx477_params *)handle->private_data;

    params->expo.expo_lef_us = us;

    lines = (1000 * us) / Preview_line_period;
    if(lines < 9) lines = 9;
    params->expo.expo_lines = lines;

    if (lines >params->expo.vts-1)
        vts = lines +1;
    else
        vts = params->expo.vts;

    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    activeline = vts - lines;
    if(activeline < 9) activeline = 9;

    params->tExpo_reg[0].data = (activeline>>8) & 0xff;
    params->tExpo_reg[1].data = (activeline>>0) & 0xff;

    params->tVts_reg[0].data = (vts >> 8) & 0xff;
    params->tVts_reg[1].data = (vts >> 0) & 0xff;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    //int rc = SensorRegArrayR((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
    unsigned short temp_gain;
  //  *gain=params->expo.final_gain;
    temp_gain = gain_reg[0].data << 8 | gain_reg[1].data << 0 ;

    *gain=(u32)(10^((temp_gain*3)/200))*1024;
    //if (gain_reg[1].data & 0x10)
    //   *gain = (*gain) * 2;

    SENSOR_DMSG("[%s] get gain/reg (1024=1X)= %u/0x%x\n", __FUNCTION__, *gain,gain_reg[0].data);
    //return rc;
    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    imx477_params *params = (imx477_params *)handle->private_data;
    //double gain_double;
    u64 gain_double;
    params->expo.final_gain = gain;

    if(gain<1024)
       gain=1024;
    else if(gain>=3980*1024)
        gain=3980*1024;

    gain_double = 20*(intlog10(gain)-intlog10(1024));

    params->tGain_reg[0].data=(u16)((((gain_double*10) >> 24)/3) >> 8)& 0xff;
    params->tGain_reg[1].data=(u16)(((gain_double*10) >> 24)/3) & 0xff;

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);

    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    imx477_params *params = (imx477_params *)handle->private_data;
    u32 i=0;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u64 gain_double;

    params->expo.final_gain = gain;
    if(gain < SENSOR_MIN_GAIN)
        gain = SENSOR_MIN_GAIN;
    else if(gain >= SENSOR_MAX_GAIN)
        gain = SENSOR_MAX_GAIN;
    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }


    gain_double = 20*(intlog10(gain)-intlog10(1024));
    params->tGain_reg[0].data=(u16)((((gain_double*10) >> 24)/3) >> 8)& 0xff;
    params->tGain_reg[1].data=(u16)(((gain_double*10) >> 24)/3) & 0xff;

    SENSOR_DMSG("[%s] set gain/reg=%u/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);
    params->dirty = true;
    return SUCCESS;
}


static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    *max = 1000000/imx477_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;//3980*1024;
    return SUCCESS;
}

static int IMX477_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/imx477_mipi_linear[0].senout.min_fps;
    info->min = (Preview_line_period * 9);
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

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    imx477_params *params;
    int res;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (imx477_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX477_MIPI");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D8;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x34;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //300000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                  = Preview_MCLK_SPEED;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type              = SENSOR_ISP_TYPE;
    //handle->data_fmt              = SENSOR_DATAFMT;
    handle->sif_bus               = SENSOR_IFBUS_TYPE;
    handle->data_prec             = SENSOR_DATAPREC;
    handle->data_mode             = SENSOR_DATAMODE;
    handle->bayer_id              = SENSOR_BAYERID;
    handle->RGBIR_id              = SENSOR_RGBIRID;
    handle->orient                = SENSOR_ORIT;
    //handle->YC_ODER               = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;
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
   //handle->video_res_supported.num_res = LINEAR_RES_END;
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = imx477_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = imx477_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = imx477_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = imx477_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = imx477_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = imx477_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = imx477_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = imx477_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, imx477_mipi_linear[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    //handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    //handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init_mipi4lane_12m15fps_linear;
    //handle->pCus_sensor_powerupseq      = pCus_powerupseq;
    handle->pCus_sensor_poweron         = pCus_poweron;
    handle->pCus_sensor_poweroff        = pCus_poweroff;
    handle->pCus_sensor_GetSensorID     = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum  = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes     = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes     = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien        = pCus_GetOrien;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode  = imx477_SetPatternMode;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT;
    handle->ae_gain_ctrl_num           = 1;
    handle->ae_shutter_ctrl_num        = 1;
    handle->sat_mingain                = SENSOR_MIN_GAIN;  //calibration
    //handle->dgain_remainder = 0;

    ////////////////////////////////////
    //  AE Control and Get Info       //
    ////////////////////////////////////
    // unit: micro seconds
    //handle->pCus_sensor_GetAETrigger_mode      = pCus_GetAETrigger_mode;
    //handle->pCus_sensor_SetAETrigger_mode      = pCus_SetAETrigger_mode;
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;
    //handle->pCus_sensor_GetDGainRemainder = pCus_GetDGainRemainder;

    //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = IMX477_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->expo.expo_lines = 5000;
    params->dirty           = false;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(IMX477_HDR,
                            cus_camsensor_init_handle_linear,
                            NULL,
                            NULL,
                            imx477_params
                         );
