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

SENSOR_DRV_ENTRY_IMPL_BEGIN(ar0144);

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
//                                                                                               //
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
#define SENSOR_BAYERID      CUS_BAYER_GR            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     16                 // max sensor again, a-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 2
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
#define Preview_line_period 20153//30580                  //(36M/37.125M)*30fps=29.091fps(34.375msec), hts=34.375/1125=30556,
//#define Line_per_second     32727
#define vts_30fps 827 //724 + 16 + 4 + 72 (vb)                               
#define Prv_Max_line_number 2200                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1280                    //resolution Width when preview
#define Preview_HEIGHT      720                    //resolution Height when preview
#define Preview_MAX_FPS     60                     //fastest preview FPS
#define Preview_MIN_FPS     5                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define SENSOR_I2C_ADDR    0x20                   //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D16        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

int blk_flag=1;
int times = 0;
//static int  drv_Fnumber = 22;
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
CUS_MCLK_FREQ UseParaMclk(void);
////////////////////////////////////
// Image Info                     //
////////////////////////////////////

static struct {     // LINEAR
    // Modify it based on number of support resolution
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
}ar0144_mipi_linear[] = {
    {LINEAR_RES_1, {1280, 720, 3, 60}, {0, 0, 1280, 720}, {"1280X720@60fps"}},
};

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
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
	I2C_ARRAY tVts_reg[1];
    I2C_ARRAY tGain_reg[1];
    I2C_ARRAY tExpo_reg[1];
} ar0144_params;
// set sensor ID address and data,

 typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static I2C_ARRAY Sensor_id_table[] =
{
{0xf0, 0x20},      // {address of ID, ID },
{0xf1, 0x53},
};


static I2C_ARRAY Sensor_init_table[] =
{
	{0xffff, 0x00C8},
    {0x301A, 0x00D9},// RESET_REGISTER
    {0x301A, 0x3058},// RESET_REGISTER
    {0xffff, 0x0064},
    {0x3F4C, 0x003F},// PIX_DEF_1D_DDC_LO_DEF
    {0x3F4E, 0x0018},// PIX_DEF_1D_DDC_HI_DEF
    {0x3F50, 0x17DF},// PIX_DEF_1D_DDC_EDGE
    {0x30B0, 0x0028},// DIGITAL_TEST
    {0x3060, 0x000D},// ANALOG_GAIN
    {0x30FE, 0x00A8},// NOISE_PEDESTAL
    {0x306E, 0x4810},// DATAPATH_SELECT
    {0x3064, 0x1802},// SMIA_TEST
    {0x302A, 0x0006},	//VT_PIX_CLK_DIV = 6
    {0x302C, 0x0001},	//VT_SYS_CLK_DIV = 1
    {0x302E, 0x0004},	//PRE_PLL_CLK_DIV = 4
    {0x3030, 0x004A},	//PLL_MULTIPLIER = 74
    {0x3036, 0x000C},	//OP_PIX_CLK_DIV = 12
    {0x3038, 0x0001},	//OP_SYS_CLK_DIV = 1
    {0x30B0, 0x0028},// DIGITAL_TEST
    {0x31B0, 0x004D},	//FRAME_PREAMBLE = 77
    {0x31B2, 0x0036},	//LINE_PREAMBLE = 54
    {0x31B4, 0x3634},	//MIPI_TIMING_0 = 13876
    {0x31B6, 0x210E},	//MIPI_TIMING_1 = 8462
    {0x31B8, 0x20C7},	//MIPI_TIMING_2 = 8391
    {0x31BA, 0x0185},	//MIPI_TIMING_3 = 389
    {0x31BC, 0x0004},	//MIPI_TIMING_4 = 4
    {0x3354, 0x002C},// MIPI_CNTRL
    {0x31AE, 0x0202},// SERIAL_FORMAT
    {0x3002, 0x0028},// Y_ADDR_START
    {0x3004, 0x0004},// X_ADDR_START
    {0x3006, 0x02F7},// Y_ADDR_END
    {0x3008, 0x0503},// X_ADDR_END
    {0x300A, 0x02EB},// FRAME_LENGTH_LINES
    {0x300C, 0x05D0},// LINE_LENGTH_PCK
    {0x3012, 0x02EE},// COARSE_INTEGRATION_TIME
    {0x31AC, 0x0C0C},// DATA_FORMAT_BITS
    {0x306E, 0x9010},// DATAPATH_SELECT
    {0x30A2, 0x0001},// X_ODD_INC
    {0x30A6, 0x0001},// Y_ODD_INC
    {0x3082, 0x0003},// OPERATION_MODE_CTRL
    {0x3040, 0x0000},// READ_MODE
    {0x31D0, 0x0000},// COMPANDING
    {0x301A, 0x005C},// RESET_REGISTER
    {0x311C, 0x02EB},// AE_MAX_EXPOSURE_REG
    {0x300A, 0x033A},// FRAME_LENGTH_LINES
    {0x311C, 0x033A},// AE_MAX_EXPOSURE_REG
};

I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
     {0x17, 0xC0},    // bit[1:0]
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


/*static I2C_ARRAY mirr_flip_table[] =
{
    #if 0
     {0x17, 0x83},    // bit[1:0]

     {0x17, 0x82},    // bit[1:0]

     {0x17, 0x81},    // bit[1:0]

     {0x17, 0x80},    // bit[1:0]
    #endif
     {0x17, 0xC0},    // bit[1:0]

     {0x17, 0xC1},    // bit[1:0]

     {0x17, 0xC2},    // bit[1:0]

     {0x17, 0xC3},    // bit[1:0]

};*/

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;


static int g_sensor_ae_min_gain = 1024;//1280;

static I2C_ARRAY gain_reg[] = { //need to fix
	{0x3060, 0x000D},    // again: coarse:[6:4],fine:[3:0]
};


static I2C_ARRAY expo_reg[] = { //ok
	{0x3012, 0x02EE},
};


static I2C_ARRAY vts_reg[] = { //need to fix
	{0x300A, 0x033A},// FRAME_LENGTH_LINES
};

I2C_ARRAY temperature_reg[] = {
    {0x0c, 0x30},
    {0x0d, 0x30},
    {0x0e, 0x30},
    {0x0f, 0x30},
};

static Gain_ARRAY again_table[] = {
    {1024,0x0000 },
    {1057,0x0001 },
    {1092,0x0002 },
    {1130,0x0003 },
    {1170,0x0004 },
    {1214,0x0005 },
    {1260,0x0006 },
    {1311,0x0007 },
    {1365,0x0008 },
    {1425,0x0009 },
    {1489,0x000A },
    {1560,0x000B },
    {1638,0x000C },
    {1725,0x000D },
    {1820,0x000E },
    {1928,0x000F },
    {2048,0x0010 },
    {2185,0x0012 },
    {2341,0x0014 },
    {2521,0x0016 },
    {2731,0x0018 },
    {2979,0x001A },
    {3277,0x001C },
    {3641,0x001E },
    {4096,0x0020 },
    {4228,0x0021 },
    {4369,0x0022 },
    {4520,0x0023 },
    {4681,0x0024 },
    {4855,0x0025 },
    {5041,0x0026 },
    {5243,0x0027 },
    {5461,0x0028 },
    {5699,0x0029 },
    {5958,0x002A },
    {6242,0x002B },
    {6554,0x002C },
    {6899,0x002D },
    {7282,0x002E },
    {7710,0x002F },
    {8192,0x0030 },
    {8738,0x0032 },
    {9362,0x0034 },
    {10082,0x0036},
    {10923,0x0038},
    {11916,0x003A},
    {13210,0x003C},
    {14564,0x003E},
    {16384,0x0040},
    
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
#define SENSOR_NAME ar0144


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
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x3C00, 0);

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(5000);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);

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
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);
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
    SensorReg_Write(0xfe,0x00);

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
    SENSOR_DMSG("[%s]ar0144 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    //SENSOR_DMSG("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ar0144_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
//static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);
    //TStart = timeGetTimeU();
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

/*
        for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
        {
            if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            {
                //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                return FAIL;
            }
        }
*/

    //pCus_SetAEGain(handle,1024); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 30000);
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

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    s16 HFlip;//,VFlip;

    SensorReg_Write(0xfe,0x00);//page 0
    SensorReg_Read(0x17, (void*)&HFlip);
    //SensorReg_Read(0x1d, &VFlip);

    if(((HFlip&0x03)==0x03)) //&&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M0F0;
    else if(((HFlip&0x03)==0x02)) //&&((VFlip&0x80)==0))
       *orit = CUS_ORIT_M1F0;
    else if(((HFlip&0x03)==0x01)) //&&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M0F1;
    else if(((HFlip&0x03)==0x00)) //&&((VFlip&0x80)!=0))
       *orit = CUS_ORIT_M1F1;

    //SENSOR_DMSG("mirror:%x\r\n", HFlip&0x80);
    //SENSOR_DMSG("Flip:%x\r\n", VFlip&0x80);

    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    ar0144_params *params = (ar0144_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            //SensorReg_Write(0xfe,0x00);//page 0
            //SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            //Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
           // Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
           // handle->bayer_id = CUS_BAYER_RG;
         //  handle->bayer_id=  CUS_BAYER_BG;
            break;
        case CUS_ORIT_M1F0:
            //SensorReg_Write(0xfe,0x00);//page 0
            //SensorReg_Write(mirr_flip_table[1].reg,mirr_flip_table[1].data);
            //Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[1].reg;
           // Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[1].data;
           // handle->bayer_id = CUS_BAYER_GR;
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
        case CUS_ORIT_M0F1:
            //SensorReg_Write(0xfe,0x00);//page 0
            //SensorReg_Write(mirr_flip_table[2].reg,mirr_flip_table[2].data);
          //  Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[2].reg;
           // Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[2].data;
           // handle->bayer_id = CUS_BAYER_GB;
     // handle->bayer_id= CUS_BAYER_GR;
            break;
        case CUS_ORIT_M1F1:
            //SensorReg_Write(0xfe,0x00);//page 0
            //SensorReg_Write(mirr_flip_table[3].reg,mirr_flip_table[3].data);
          //  Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[3].reg;
          //  Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[3].data;
          //  handle->bayer_id = CUS_BAYER_BG;
     // handle->bayer_id= CUS_BAYER_GR;
            break;
        default :
            //SensorReg_Write(0xfe,0x00);//page 0
            //SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
         //   Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
          //  Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
          //  handle->bayer_id = CUS_BAYER_RG;
    //  handle->bayer_id= CUS_BAYER_BG;
            break;
    }
    //SensorReg_Write(0xef,0x01);
 //   SensorReg_Write(0x09,1);

    params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ar0144_params *params = (ar0144_params *)handle->private_data;
    u32 tVts = vts_reg[0].data;
	
    params->expo.preview_fps = (vts_30fps*Preview_MAX_FPS)/tVts;

    return  params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    int vts=0;
    
    ar0144_params *params = (ar0144_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    //return SUCCESS;

    if(fps>=5 && fps <= 60){
      params->expo.fps = fps;
      params->expo.vts=  (vts_30fps*Preview_MAX_FPS)/fps;
    }else{
      //params->expo.vts=vts_30fps;
      //params->expo.fps=30;
      //SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
      return FAIL;
    }

    vts = params->expo.vts;

	vts_reg[0].data = vts;
	
	//printk("[%s] us %u, lines 0x%X\n", __FUNCTION__, fps, vts_reg[0].data);//,
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
    ar0144_params *params = (ar0144_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:

        break;
        case CUS_FRAME_ACTIVE:
        if(params->dirty)
        {
			SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
            params->dirty = false; 
        }
        break;
        default :
        break;
    }

    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
    ar0144_params *params = (ar0144_params *)handle->private_data;
    int rc;
    u32 lines = 0;
    rc = SensorRegArrayR((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
    if (SUCCESS == rc) {
    lines = params->tExpo_reg[0].data;
        *us = lines*Preview_line_period/1000;
    }
    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0; //vts = 0;
    ar0144_params *params = (ar0144_params *)handle->private_data;
	if(us >= 16666)
	{
		us = 16666;	
	}
	lines=(1000*us)/Preview_line_period;
    if(lines<1) lines=1;
			
	//vts = vts_30fps;
    params->tExpo_reg[0].data = lines;
    //params->tVts_reg[0].data = vts;
	
    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__, us, lines, params->expo.vts);
	//printk("[%s] us %u, lines 0x%X\n", __FUNCTION__, us, lines);//,vts);
    params->dirty = true;    //reg need to update

    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    
    int rc = SUCCESS;
    //float fine_gain, hcg;
    ar0144_params *params = (ar0144_params *)handle->private_data;
    rc = SensorRegArrayR((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
	
	*gain = params->tGain_reg[0].data;
 
	//printk("[%s] gain[%d]\n", __FUNCTION__, *gain);
  return rc; 
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
	s16 a;
     u32 input_gain = 0;
	int i;
    //bool hcg =0;
    ar0144_params *params = (ar0144_params *)handle->private_data;
    params->expo.final_gain = gain;

    input_gain = gain;
	//printk("gain[%d]\n",gain);
    if(gain<1024)
	{
        gain=1024;
		params->tGain_reg[0].data = again_table[0].reg_val;
		//SensorReg_Write(0x3060,0x0000);		
	}
    else if(gain>=SENSOR_MAX_GAIN*1024)
	{
        gain=SENSOR_MAX_GAIN*1024;
		params->tGain_reg[0].data = again_table[(sizeof(again_table)/sizeof(again_table[0]))-1].reg_val;
		//SensorReg_Write(0x3060,0x0040);			
	}
	else
	{
		for(i = 0;i < 49;i++)
		{
			if(gain <= again_table[i].total_gain)
			{
				params->tGain_reg[0].data = again_table[i].reg_val;
				 //SensorReg_Write(0x3060,again_table[i].reg_val);
				SensorReg_Read(0x3060, (void*)&a);				 
				 //printk("[%s] input_gain=%d ,total_gain val=%d , reg_val=0x%X,a=%d\n", __FUNCTION__,input_gain,again_table[i].total_gain,again_table[i].reg_val,a);
				
				break;
			}
		}
	}
	//SensorReg_Write(0x30BA,0x7606);

   
    //pr_err("input_gain %d, 0x3060=0x%X, 0x305E=0x%X, 0x3100=0x%X, 0x3202=0x%X, 0x3206=0x%X, 0x3208=0x%X\n",
    //                input_gain, params->tGain_reg[0].data, params->tGain_reg[1].data, params->tGain_reg[2].data, params->tGain_reg[3].data, params->tGain_reg[4].data, params->tGain_reg[5].data);
    //pr_err("gain %d, coarse_gain %d\n", gain, coarse_gain);
    params->dirty  = true;    //reg need to update
    return SUCCESS; 
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;//30
    *max = 1000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN*1024;
    return SUCCESS;
}

static int ar0144_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min =  Preview_line_period*1;//2
    info->step = Preview_line_period*1;//2
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
  //  u32 i, j;
/*
    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    //SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);
*/
    return SUCCESS;
}

static int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    ar0144_params *params;
    int res;
    
    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(ar0144_params));
    params = (ar0144_params *)handle->private_data;
	
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ar0144_MIPI");

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
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.num_res = res+1;
        handle->video_res_supported.res[res].width         = ar0144_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = ar0144_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = ar0144_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = ar0144_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = ar0144_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = ar0144_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = ar0144_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = ar0144_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, ar0144_mipi_linear[res].senstr.strResDesc);
    }

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
    handle->pCus_sensor_SetPatternMode = ar0144_SetPatternMode;
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
//    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = ar0144_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line = 100;
    params->dirty = false;
    params->orient_dirty = false;
    handle->sensor_if_api->Reset(1,0);
    //insmod set sensor reset mode
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
        CamOsMemRelease(handle->private_data);
        handle->private_data = NULL;
    }
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END(  ar0144,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL
                         );

