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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(OG0VA1B);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL

//============================================
//MIPI config begin.
#define SENSOR_MIPI_LANE_NUM (1)
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

#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     15872//(15.5)        // max sensor again, a-gain * conversion-gain*d-gain
#define MAX_A_GAIN          15872
#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ       //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 7839                   // 1s/(vts*fps) ns
#define vts_30fps           1063                    // VTS for 30fps

#define Preview_WIDTH       640                    //resolution Width when preview
#define Preview_HEIGHT      480                    //resolution Height when preview
#define Preview_MAX_FPS     120                      //fastest preview FPS
#define Preview_MIN_FPS     3                       //slowest preview FPS

#define SENSOR_I2C_ADDR     0xC0                    //I2C slave address
#define SENSOR_I2C_SPEED    240000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY   I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT      I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int g_sensor_ae_min_gain = 1024;


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
        u32 lines;
    } expo;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tMirror_reg[2];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty;
    bool orien_dirty;
} OG0VA1B_params;
// set sensor ID address and data

typedef struct {
    u64 total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

const I2C_ARRAY Sensor_id_table[] =
{
    
};

const I2C_ARRAY Sensor_init_table[] =     // 640X480_VGA_120FPS
{
    {0x0103, 0x01},
    {0x0302, 0x31},
    {0x0304, 0x01},
    {0x0305, 0xe0},
    {0x0306, 0x00},
    {0x0326, 0xd8},
    {0x3006, 0x0e},
    {0x300d, 0x08},
    {0x3018, 0xf0},
    {0x301c, 0xf0},
    {0x3020, 0x20},
    {0x3040, 0x0f},
    {0x3022, 0x01},
    {0x3107, 0x40},
    {0x3216, 0x01},
    {0x3217, 0x00},
    {0x3218, 0xc0},
    {0x3219, 0x55},
    {0x3500, 0x00},
    {0x3501, 0x01},
    {0x3502, 0xfe},
    {0x3506, 0x01},
    {0x3507, 0x50},
    {0x3508, 0x01},
    {0x3509, 0x00},
    {0x350a, 0x01},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x3541, 0x00},
    {0x3542, 0x40},
    {0x3605, 0x90},
    {0x3606, 0x41},
    {0x3612, 0x00},
    {0x3620, 0x08},
    {0x3630, 0x17},
    {0x3631, 0x99},
    {0x3639, 0x88},
    {0x3668, 0x00},
    {0x3674, 0x00},
    {0x3677, 0x3f},
    {0x368f, 0x06},
    {0x36a2, 0x19},
    {0x36a4, 0xf1},
    {0x36a5, 0x2d},
    {0x3706, 0x30},
    {0x370d, 0x72},
    {0x3713, 0x86},
    {0x3715, 0x03},
    {0x3716, 0x00},
    {0x376d, 0x24},
    {0x3770, 0x3a},
    {0x3778, 0x00},
    {0x37a8, 0x03},
    {0x37a9, 0x00},
    {0x37df, 0x7d},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x02},
    {0x3805, 0x8f},
    {0x3806, 0x01},
    {0x3807, 0xef},
    {0x3808, 0x02},
    {0x3809, 0x80},
    {0x380a, 0x01},
    {0x380b, 0xe0},
    {0x380c, 0x01},
    {0x380d, 0x78},
    {0x380e, 0x04},
    {0x380f, 0x27},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3816, 0x00},
    {0x3817, 0x01},
    {0x3818, 0x00},
    {0x3819, 0x05},
    {0x3820, 0x40},
    {0x3821, 0x04},
    {0x3823, 0x00},
    {0x3826, 0x00},
    {0x3827, 0x00},
    {0x382b, 0x52},
    {0x384a, 0xa2},
    {0x3858, 0x00},
    {0x3859, 0x00},
    {0x3860, 0x00},
    {0x3861, 0x00},
    {0x3866, 0x0c},
    {0x3867, 0x07},
    {0x3884, 0x00},
    {0x3885, 0x08},
    {0x3888, 0x50},
    {0x3893, 0x6c},
    {0x3898, 0x00},
    {0x389a, 0x04},
    {0x389b, 0x01},
    {0x389c, 0x0b},
    {0x389d, 0xdc},
    {0x389f, 0x08},
    {0x38a0, 0x00},
    {0x38a1, 0x00},
    {0x38b1, 0x04},
    {0x38b2, 0x00},
    {0x38b3, 0x08},
    {0x38c1, 0x46},
    {0x38c9, 0x02},
    {0x38d4, 0x06},
    {0x38d5, 0x5a},
    {0x38d6, 0x08},
    {0x38d7, 0x3a},
    {0x391e, 0x00},
    {0x391f, 0x00},
    {0x3920, 0xa5},
    {0x3921, 0x00},
    {0x3922, 0x00},
    {0x3923, 0x00},
    {0x3924, 0x05},
    {0x3925, 0x00},
    {0x3926, 0x00},
    {0x3927, 0x00},
    {0x3928, 0x1a},
    {0x3929, 0x01},
    {0x392a, 0xb4},
    {0x392b, 0x00},
    {0x392c, 0x10},
    {0x392f, 0x40},
    {0x3a06, 0x06},
    {0x3a07, 0x78},
    {0x3a08, 0x08},
    {0x3a09, 0x80},
    {0x3a52, 0x00},
    {0x3a53, 0x01},
    {0x3a54, 0x0c},
    {0x3a55, 0x04},
    {0x3a58, 0x0c},
    {0x3a59, 0x04},
    {0x4000, 0xcf},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x13},
    {0x400a, 0x02},
    {0x400b, 0x34},
    {0x4010, 0x71},
    {0x4042, 0xc3},
    {0x4306, 0x04},
    {0x4307, 0x12},
    {0x4500, 0x70},
    {0x4509, 0x00},
    {0x450b, 0x83},
    {0x4604, 0x68},
    {0x481b, 0x44},
    {0x481f, 0x30},
    {0x4823, 0x44},
    {0x4825, 0x35},
    {0x4837, 0x11},
    {0x4f00, 0x04},
    {0x4f10, 0x04},
    {0x4f21, 0x01},
    {0x4f22, 0x00},
    {0x4f23, 0x54},
    {0x4f24, 0x51},
    {0x4f25, 0x41},
    {0x5000, 0x3f},
    {0x5001, 0x80},
    {0x500a, 0x00},
    {0x5100, 0x00},
    {0x5111, 0x20},
    {0x0100, 0x01},
};

const I2C_ARRAY TriggerStartTbl[] = {
    
};

const I2C_ARRAY PatternTbl[] = {
    {0x5100, 0x00},
};

const I2C_ARRAY mirror_reg[] = {
    {0x3820, 0x40}, //40/44
    {0x3821, 0x04}, //00/04
};

/////////////////////////////////////////////////////////////////
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

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const static I2C_ARRAY gain_reg[] = {
    {0x3508, 0x00},
    {0x3509, 0x10},
};

const I2C_ARRAY expo_reg[] = {
    {0x3500, 0x00},
    {0x3501, 0x38},
    {0x3502, 0x20},
};

const I2C_ARRAY vts_reg[] = {
    {0x380e,0x04},//MSB
    {0x380f,0x27},//LSB
};

const static Gain_ARRAY gain_table[]={
    {10000,	0x0100},
    {10625,	0x0110},
    {11250,	0x0120},
    {11875,	0x0130},
    {12500,	0x0140},
    {13125,	0x0150},
    {13750,	0x0160},
    {14375,	0x0170},
    {15000,	0x0180},
    {15625,	0x0190},
    {16250,	0x01A0},
    {16875,	0x01B0},
    {17500,	0x01C0},
    {18125,	0x01D0},
    {18750,	0x01E0},
    {19375,	0x01F0},
    {20000,	0x0200},
    {21250,	0x0220},
    {22500,	0x0240},
    {23750,	0x0260},
    {25000,	0x0280},
    {26250,	0x02A0},
    {27500,	0x02C0},
    {28750,	0x02E0},
    {30000,	0x0300},
    {31250,	0x0320},
    {32500,	0x0340},
    {33750,	0x0360},
    {35000,	0x0380},
    {36250,	0x03A0},
    {37500,	0x03C0},
    {38750,	0x03E0},
    {40000,	0x0400},
    {42500,	0x0440},
    {45000,	0x0480},
    {47500,	0x04C0},
    {50000,	0x0500},
    {52500,	0x0540},
    {55000,	0x0580},
    {57500,	0x05C0},
    {60000,	0x0600},
    {62500,	0x0640},
    {65000,	0x0680},
    {67500,	0x06C0},
    {70000,	0x0700},
    {72500,	0x0740},
    {75000,	0x0780},
    {77500,	0x07C0},
    {80000,	0x0800},
    {85000,	0x0880},
    {90000,	0x0900},
    {95000,	0x0980},
    {100000,0x0A00},
    {105000,0x0A80},
    {110000,0x0B00},
    {115000,0x0B80},
    {120000,0x0C00},
    {125000,0x0C80},
    {130000,0x0D00},
    {135000,0x0D80},
    {140000,0x0E00},
    {145000,0x0E80},
    {150000,0x0F00},
    {155000,0x0F80},
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
#define SENSOR_NAME OG0VA1B


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

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(1000);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(1000);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_USLEEP(1000);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x3C00, 0);
    SENSOR_USLEEP(5000);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);
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

  SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
  for(n=0;n<table_length;++n) {
    id_from_sensor[n].reg = Sensor_id_table[n].reg;
    id_from_sensor[n].data = 0;
  }

  *id =0;
  if(table_length>8) table_length=8;

  for(n=0; n<5; ++n) {              //retry , until I2C success
    if(n>3) return FAIL;

    if(SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
    break;
    else
        SENSOR_MSLEEP_(1);
  }

  for(i=0; i<table_length; ++i) {
    if( id_from_sensor[i].data != Sensor_id_table[i].data ) {
      SENSOR_DMSG("[%s]Read OG0VA1B id: 0x%x 0x%x\n", __FUNCTION__, id_from_sensor[0].data, id_from_sensor[1].data);
      return FAIL;
    }
    *id = id_from_sensor[i].data;
  }
  SENSOR_DMSG("[%s]Read OG0VA1B id, get 0x%x Success\n", __FUNCTION__, (int)*id);
      return SUCCESS;
}

static int OG0VA1B_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
        for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
        {
            if(Sensor_init_table[i].reg==0xff)
            {
                SENSOR_MSLEEP_(Sensor_init_table[i].data);
            }
            else
            {
                cnt = 0;
                while(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS  && Sensor_init_table[i].reg != 0x20)
                {
                    cnt++;
                    //SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
                    if(cnt>=10)
                    {
                        //SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                        return FAIL;
                    }
                    SENSOR_MSLEEP_(10);
                }
            }
        }


    for(i=0;i< ARRAY_SIZE(mirror_reg); i++) {
        if(SensorReg_Write(params->tMirror_reg[i].reg,params->tMirror_reg[i].data) != SUCCESS) {
            return FAIL;
        }
    }

    //pCus_SetAEUSecs(handle, 25000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
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
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;

    sen_data = params->tMirror_reg[1].data & 0x03;
    SENSOR_DMSG("\n\n[%s]:mirror:%x\r\n\n\n\n",__FUNCTION__, sen_data);
    switch(sen_data) {
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

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;

   switch(orit) {
      case CUS_ORIT_M0F0:
            params->tMirror_reg[0].data = 0x40;
            params->tMirror_reg[1].data = 0x04;
            params->orien_dirty = true;
        break;
        case CUS_ORIT_M1F0:
            params->tMirror_reg[0].data = 0x40;
            params->tMirror_reg[1].data = 0x00;
            params->orien_dirty = true;
        break;
        case CUS_ORIT_M0F1:
            params->tMirror_reg[0].data = 0x44;
            params->tMirror_reg[1].data = 0x04;
            params->orien_dirty = true;
        break;
        case CUS_ORIT_M1F1:
            params->tMirror_reg[0].data = 0x44;
            params->tMirror_reg[1].data = 0x00;
            params->orien_dirty = true;
        break;
    }

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
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
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
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

    if ((params->expo.lines) > (params->expo.vts - 14))
        vts = params->expo.lines + 14;
    else
        vts = params->expo.vts;
    
    SENSOR_DMSG("[%s] min max fps [%d, %d], fps %d, params->expo.lines %d, params->expo.vts %d, vts %d\n", __FUNCTION__,
                min_fps,
                max_fps,
                fps,
                params->expo.lines,
                params->expo.vts,
                vts
                );
                
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
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
     OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            if(params->orien_dirty){
                handle->sensor_if_api->SetSkipFrame(handle->snr_pad_group, params->expo.fps, 3);
                SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, ARRAY_SIZE(mirror_reg));
                params->orien_dirty = false;
            }
            break;
        case CUS_FRAME_ACTIVE:
            if(params->reg_dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                params->reg_dirty = false;
            }
            break;
        default :
            break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    u32 lines = 0;
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
    lines  = (u32)(params->tExpo_reg[2].data&0xff);
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    //lines >>= 4;
    *us = (lines*Preview_line_period)/1000;
    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;

    lines=(u32)((1000*us)/Preview_line_period);
    if (lines < 1) lines = 1;
    
    params->expo.lines = lines;
    
    if (lines > (params->expo.vts - 14))
        vts = lines + 14;
    else
        vts=params->expo.vts;
    
    SENSOR_DMSG("[%s] us %d, lines %d, vts %d, params->expo.vts %d\n", __FUNCTION__,
                us,
                lines,
                vts,
                params->expo.vts
                );
    //lines <<= 4;
    params->tExpo_reg[1].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[2].data =(u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

  params->reg_dirty = true;
  return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
    *gain= ((params->tGain_reg[0].data)*10000 + (params->tGain_reg[1].data)*625)*1024/10000;
    SENSOR_DMSG("[%s] get gain %d\n", __FUNCTION__, *gain);
    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    OG0VA1B_params *params = (OG0VA1B_params *)handle->private_data;
    u32 i;
    u16 gain16 = 0;
    u64 gain_double,total_gain_double;
    
    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    if(gain<1024)
        gain=1024;
    else if(gain>=MAX_A_GAIN)
        gain=MAX_A_GAIN;
    
    gain_double=(u64)gain;
    total_gain_double=(gain_double*10000)/1024;

    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            gain16=(gain_table[i].total_gain-total_gain_double > total_gain_double-gain_table[i-1].total_gain) ? gain_table[i-1].reg_val:gain_table[i].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            gain16=gain_table[i].reg_val;
            break;
        }
    }
    
    SENSOR_DMSG("[%s] set gain/reg/=%d/0x%x\n", __FUNCTION__, gain,gain16);
    
    params->tGain_reg[0].data = (gain16 >> 8)& 0x0f; //high bit
    params->tGain_reg[1].data = (gain16 >> 0)& 0xf0; //low byte

    params->reg_dirty = true;
    
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1;
  *max = 1000000/Preview_MIN_FPS;
  return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1024;
  *max = SENSOR_MAX_GAIN;
  return SUCCESS;
}

static int OG0VA1B_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    OG0VA1B_params *params;
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
    params = (OG0VA1B_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"OG0VA1B_MIPI");

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
    handle->video_res_supported.res[0].nOutputWidth= 640;
    handle->video_res_supported.res[0].nOutputHeight= 480;
    sprintf(handle->video_res_supported.res[0].strResDesc, "640x480@120fps");

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
    handle->pCus_sensor_SetPatternMode = OG0VA1B_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = OG0VA1B_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 120;
    params->expo.lines = 100;
    params->reg_dirty = false;
    params->orien_dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  OG0VA1B,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            OG0VA1B_params
                         );

