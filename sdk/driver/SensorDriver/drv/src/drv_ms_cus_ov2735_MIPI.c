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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(OV2735);

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
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_MAXGAIN      128
#define SENSOR_BAYERID      CUS_BAYER_RG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
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
#define Preview_line_period 24969                  //
//#define Line_per_second     32727
#define vts_30fps  1335//1090                              //for 29.091fps @ MCLK=36MHz
#define Prv_Max_line_number 1080                    //maximum exposure line munber of sensor when preview
#define Preview_WIDTH       1920                   //resolution Width when preview
#define Preview_HEIGHT      1080                   //resolution Height when preview
#define Preview_MAX_FPS     30                     //fastest preview FPS
#define Preview_MIN_FPS     3                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

#define MAX_A_GAIN (155000*1024)

#define SENSOR_I2C_ADDR     0x78                    //I2C slave address
#define SENSOR_I2C_SPEED   200000 //300000// 240000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
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
    } expo;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tMirror_reg[8];
    I2C_ARRAY tPatternTbl[1];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty;
    bool orien_dirty;
} ov2735_params;
// set sensor ID address and data

typedef struct {
    u64 total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

const I2C_ARRAY Sensor_id_table[] =
{
	    {0x02, 0x27},      // {address of ID, ID },
	    {0x03, 0x35},      // {address of ID, ID },
};


const I2C_ARRAY Sensor_init_table[] =     // 1920*1080_30fps_27MCLK_756MMIPI_2840*1109*30
{
//ov2735r1a_am04 mipi 1920x1080_2 lane_30fps_84M_raw10(for r1a and r1b version)
{0xfd,0x00},
{0x20,0x00},
{0xff,0x03},
{0xfd,0x00},
{0x2f,0x5b},//yc
{0x34,0x00},
{0x30,0x15},
{0x33,0x01},
{0x35,0x20},
{0xfd,0x01},
{0x0d,0x00},
{0x30,0x00},
{0x03,0x01},
{0x04,0x8f},
{0x01,0x01},
{0x09,0x00},
{0x0a,0x20},
{0x06,0x0a},
{0x24,0x10},
{0x01,0x01},
{0xfb,0x73},
{0x01,0x01},
{0xfd,0x01},
{0x1a,0x6b},
{0x1c,0xea},
{0x16,0x0c},
{0x21,0x00},
{0x11,0x63},
{0x19,0xc3},
{0x26,0xda},
{0x29,0x01},
{0x33,0x6f},
{0x2a,0xd2},
{0x2c,0x40},
{0xd0,0x02},
{0xd1,0x01},
{0xd2,0x20},
{0xd3,0x04},
{0xd4,0x2a},
{0x50,0x00},
{0x51,0x2c},
{0x52,0x29},
{0x53,0x00},
{0x55,0x44},
{0x58,0x29},
{0x5a,0x00},
{0x5b,0x00},
{0x5d,0x00},
{0x64,0x2f},
{0x66,0x62},
{0x68,0x5b},
{0x75,0x46},
{0x76,0xf0},
{0x77,0x4f},
{0x78,0xef},
{0x72,0xcf},
{0x73,0x36},
{0x7d,0x0d},
{0x7e,0x0d},
{0x8a,0x22},
{0x8b,0x22},
{0xfd,0x01},
{0xb1,0x83},//;DPHY enable 8b
{0xb3,0x0b},//;0b;09;1d
{0xb4,0x14},//;MIPI PLL enable;14;35;36
{0x9d,0x40},//;mipi hs dc level 40/03/55
{0xa1,0x05},//;speed/03
{0x94,0x44},//;dphy time
{0x95,0x33},//;dphy time
{0x96,0x1f},//;dphy time
{0x98,0x45},//;dphy time
{0x9c,0x10},//;dphy time
{0xb5,0x70},//;30
//{0xa0,0x01},//;mipi enable
{0x25,0xe0},
{0x20,0x7b},
{0x8f,0x88},
{0x91,0x40},
{0xfd,0x01},
{0xfd,0x02},
{0x5e,0x03},
{0xa1,0x04},
{0xa3,0x40},
{0xa5,0x02},
{0xa7,0xc4},
{0xfd,0x01},
{0x86,0x78},
{0x89,0x78},
{0x87,0x6b},
{0x88,0x6b},
{0xfc,0xe0},
{0xfe,0xe0},
{0xf0,0x10},//yc
{0xf1,0x10},//yc
{0xf2,0x10},//yc
{0xf3,0x10},//yc
{0xfd,0x02},	//crop to 1920x1080
{0xa0,0x00},//	;Image vertical start MSB3bits
{0xa1,0x08},//	;Image vertical start LSB8bits
{0xa2,0x04},//	;image vertical size  MSB8bits
{0xa3,0x38},//	;image vertical size  LSB8bits
{0xa4,0x00},//
{0xa5,0x04},//	;H start 8Lsb, keep center
{0xa6,0x03},//
{0xa7,0xc0},//	;Half H size Lsb8bits
{0xfd,0x01},//
{0x8e,0x07},//
{0x8f,0x80},//	;MIPI column number
{0x90,0x04},//	;MIPI row number
{0x91,0x38},//

{0xfd,0x03},
{0xc0,0x01},//	;enable transfer OTP BP information
{0xfd,0x04},
{0x22,0x14},
{0x23,0x14},//	;enhance normal and dummy BPC

{0xfd,0x01},//
{0x06,0xe0},//	;insert dummy line , the frame rate is 30.01.
{0x01,0x01},//	;
{0xa0,0x01},//;mipi enable

};

const I2C_ARRAY TriggerStartTbl[] = {
//{0x30f4,0x00},//Master mode start
};

const I2C_ARRAY PatternTbl[] = {
    {0xb2,0x40}, //colorbar pattern , bit 0 to enable
};

const I2C_ARRAY mirror_reg[] = {

  {0xfd, 0x01}, //0
  {0x01, 0x00}, //1
  {0x3f, 0x03}, //Mirror/Flip
  {0xf8, 0x00},
  {0x01, 0x01}, //4
  {0xfd, 0x02}, //5
  {0x5e, 0x07}, //mem down en + enable auto BR first
  {0xa1, 0x08}, //vertical start


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
    /*
       {0xEF, 0x01},//M0F0
       {0x1b, 0x00},//bit7,Hflip
    // {0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x00},//bit7,Vflip

    {0xEF, 0x01},//M1F0
    {0x1b, 0x80},//bit7,Hflip
    // {0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x00},//bit7,Vflip

    {0xEF, 0x01},//M0F1
    {0x1b, 0x00},//bit7,Hflip
    //{0x90, 0x00},//bit2 Cmd_ADC_Latency
    {0x1d, 0x80},//bit7,Vflip

    {0xEF, 0x01},//M1F1
    {0x1b, 0x80},//bit7,Hflip
    // {0x90, 0x04},//bit2 Cmd_ADC_Latency
    {0x1d, 0x80},//bit7,Vflip
     */
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

const static I2C_ARRAY gain_reg[] = {
    // {0x38, 0x00},//bit0, high bit
    {0x24, 0x00},//low byte
    {0x39, 0x80},
};

const static Gain_ARRAY gain_table[]={
    {10000  ,0x10},
    {10625 	,0x11},
    {11250 	,0x12},
    {11875 	,0x13},
    {12500 	,0x14},
    {13125 	,0x15},
    {13750 	,0x16},
    {14375 	,0x17},
    {15000 	,0x18},
    {15625 	,0x19},
    {16250 	,0x1a},
    {16875 	,0x1b},
    {17500 	,0x1c},
    {18125 	,0x1d},
    {18750 	,0x1e},
    {19375 	,0x1f},
    {20000	,0x20},
    {21250	,0x22},
    {22500	,0x24},
    {23750	,0x26},
    {25000	,0x28},
    {26250	,0x2a},
    {27500	,0x2c},
    {28750	,0x2e},
    {30000	,0x30},
    {31250	,0x32},
    {32500  ,0x34},
    {33750  ,0x36},
    {35000	,0x38},
    {36250  ,0x3a},
    {37500	,0x3c},
    {38750  ,0x3e},
    {40000	,0x40},
    {42500	,0x44},
    {45000	,0x48},
    {47500	,0x4c},
    {50000	,0x50},
    {52500	,0x54},
    {55000	,0x58},
    {57500	,0x5c},
    {60000	,0x60},
    {62500	,0x64},
    {65000	,0x68},
    {67500	,0x6c},
    {70000	,0x70},
    {72500	,0x74},
    {75000	,0x78},
    {77500	,0x7c},
    {80000  ,0x80},
    {85000	,0x88},
    {90000	,0x90},
    {95000	,0x98},
    {100000	,0xa0},
    {105000	,0xa8},
    {110000	,0xb0},
    {115000	,0xb8},
    {120000	,0xc0},
    {125000	,0xc8},
    {130000	,0xd0},
    {135000	,0xd8},
    {140000	,0xe0},
    {145000	,0xe8},
    {150000	,0xf0},
    {155000	,0xf8},
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

    {0x03, 0x01},
    {0x04, 0x00},
};

const I2C_ARRAY vts_reg[] = {
    {0x0d,0x10},//0x10 enable
    {0x0E, 0xFF&(vts_30fps>>8)},//MSB
    {0x0F, 0xFF&vts_30fps},//LSB
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
#define SENSOR_NAME ov2735


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
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x3C00, 0);

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

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    for(n=0; n < table_length; ++n) {
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
            SENSOR_DMSG("[%s]Read ov2735 id: 0x%x 0x%x\n", __FUNCTION__, id_from_sensor[0].data, id_from_sensor[1].data);
            return FAIL;
        }
        *id = id_from_sensor[i].data;
    }
    SENSOR_DMSG("[%s]Read ov2735 id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int ov2735_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i=0;
    ov2735_params *params = (ov2735_params *)handle->private_data;
    switch(mode)
    {
        case 1:
            params->tPatternTbl[0].data |= 0x01; //enable
            break;
        case 0:
            params->tPatternTbl[0].data &= 0xfe; //disable
            break;
        default:
            params->tPatternTbl[0].data &= 0xfe; //disable
            break;
    }

    for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
    {
        if(SensorReg_Write(params->tPatternTbl[i].reg,params->tPatternTbl[i].data) != SUCCESS)
        {
            return FAIL;
        }
    }
    return SUCCESS;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int pCus_init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    ov2735_params *params = (ov2735_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    for(i=0; i < ARRAY_SIZE(Sensor_init_table); i++)
    {
        if(Sensor_init_table[i].reg == 0xff)
            SENSOR_MSLEEP_(Sensor_init_table[i].data);
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table[i].reg, Sensor_init_table[i].data) != SUCCESS  && Sensor_init_table[i].reg != 0x20)
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
        case 1:
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
#if 1
    return SUCCESS;
#else
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
#endif
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit) {
    char index=0;
    ov2735_params *params = (ov2735_params *)handle->private_data;
    switch(orit) {
        case CUS_ORIT_M0F0:
            index = 0;
            params->tMirror_reg[3].data = 0x00;
            params->tMirror_reg[6].data = 0x07;
            params->tMirror_reg[7].data = 0x08;
            handle->bayer_id = CUS_BAYER_BG;
            break;
        case CUS_ORIT_M1F0:
            index = 0x01;
            params->tMirror_reg[3].data = 0x00;//Mirror/Flip
            params->tMirror_reg[6].data = 0x06; //mem down en + enable auto BR first
            params->tMirror_reg[7].data = 0x09; //vertical start
            handle->bayer_id = CUS_BAYER_RG;
            break;
        case CUS_ORIT_M0F1:
            index = 0x02;
            params->tMirror_reg[3].data = 0x02;
            params->tMirror_reg[6].data = 0x06;
            params->tMirror_reg[7].data = 0x09;
            handle->bayer_id = CUS_BAYER_BG;
            break;
        case CUS_ORIT_M1F1:
            index = 0x03;
            params->tMirror_reg[3].data = 0x02;
            params->tMirror_reg[6].data = 0x07;
            params->tMirror_reg[7].data = 0x08;//0x09;
            handle->bayer_id = CUS_BAYER_RG;
            break;
    }
    SENSOR_DMSG("pCus_SetOrien:%x\r\n", index);

    if (index != params->tMirror_reg[2].data) {
        params->tMirror_reg[2].data = index;
        params->orien_dirty = true;
    }
    //params->reg_dirty = true;

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ov2735_params *params = (ov2735_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[1].data << 8) | (params->tVts_reg[2].data << 0);

    if (params->expo.fps >= 3000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    ov2735_params *params = (ov2735_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);

    if(fps>=3 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
        params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
        return SUCCESS;
    }else if(fps>=3000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
        params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        SENSOR_DMSG("\n\n[%s], fps=%d, lines=%d\n", __FUNCTION__, fps, params->expo.vts);
        return SUCCESS;
    }else{
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
     ov2735_params *params = (ov2735_params *)handle->private_data;
	//ISensorIfAPI *sensor_if = handle->sensor_if_api;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:

            if(params->orien_dirty){
                 //sensor_if->SetSkipFrame(handle,2); //skip 2 frame to avoid bad frame after mirror/flip
                 SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
                 //sensor_if->BayerFmt(handle, handle->bayer_id);
                 params->orien_dirty = false;
            }

        break;
        case CUS_FRAME_ACTIVE:

            if(params->reg_dirty)
            {
                SensorReg_Write(0xfd,0x01);//page 1
                SensorReg_Write(0x01,0x00);//frame sync disable
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
                SensorReg_Write(0x01,0x01);//frame sync enable

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
    ov2735_params *params = (ov2735_params *)handle->private_data;
    lines  =  (u32)(params->tExpo_reg[1].data&0xff);
    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    u32 lines = 0, vts = 0;
    ov2735_params *params = (ov2735_params *)handle->private_data;

    lines=(u32)((1000*us+(Preview_line_period>>1))/Preview_line_period);
    if (lines < 2) lines = 2;
    if (lines >params->expo.vts-1)
        vts = lines +1;
    else
        vts=params->expo.vts;


    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

   // lines <<= 4;
    params->tExpo_reg[0].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[1].data =(u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[1].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (vts >> 0) & 0x00ff;

  params->reg_dirty = true;
  return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    int again;
    ov2735_params *params = (ov2735_params *)handle->private_data;
    again=params->tGain_reg[0].data;

    *gain = again<<6;
    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    ov2735_params *params = (ov2735_params *)handle->private_data;
    u32 i,input_gain = 0;
    u16 gain8 = 1024;
    u64 gain_double,total_gain_double;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;
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
            gain8=gain_table[i-1].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
           gain8=gain_table[i].reg_val;
           break;
        }
    }

    params->tGain_reg[0].data =gain8;


  SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
  params->reg_dirty = true;
  return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    ov2735_params *params = (ov2735_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u32 i,input_gain = 0;
    u16 gain8 = 0;
    u64 gain_double,total_gain_double;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;

    if(gain<1024)
        gain=1024;
    else if(gain>=MAX_A_GAIN)
        gain=MAX_A_GAIN;


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

    gain_double=(u64)gain;
    total_gain_double=(gain_double*10000)/1024;

    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            gain8=(gain_table[i].total_gain-total_gain_double > total_gain_double-gain_table[i-1].total_gain) ? gain_table[i-1].reg_val:gain_table[i].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            gain8=gain_table[i].reg_val;
            break;
        }
    }

    params->tGain_reg[0].data =gain8;
#if 0
    gain16=(u16)(gain>>6);
    if(gain16>255)
       gain16=255;
   // params->tGain_reg[0].data = (gain16>>8)&0x01;//high bit
    params->tGain_reg[0].data = gain16&0xff; //low byte
/*
    if(input_gain > MAX_A_GAIN){

            params->tGain_reg[1].data=(u16)( (input_gain/MAX_A_GAIN)*128)&0xFF;
        }
    else{

            params->tGain_reg[1].data=0x80;
        }

 */
#endif
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
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
    *max = SENSOR_MAXGAIN*1024;
    return SUCCESS;
}

static int ov2735_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle) {
    ms_cus_sensor *handle = drv_handle;
    ov2735_params *params;
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
    params = (ov2735_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tPatternTbl, PatternTbl, sizeof(PatternTbl));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"ov2735_MIPI");

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
    SENSOR_DMSG(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

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

    handle->ae_gain_ctrl_num    = 1;
    handle->ae_shutter_ctrl_num = 1;

    ///calibration
    handle->sat_mingain=g_sensor_ae_min_gain;


    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init;

    handle->pCus_sensor_poweron     = pCus_poweron;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID;

    handle->pCus_sensor_GetVideoResNum    = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes    = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien;
    handle->pCus_sensor_GetFPS            = pCus_GetFPS;
    handle->pCus_sensor_SetFPS            = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode = ov2735_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = ov2735_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->reg_dirty = false;
    params->orien_dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  OV2735,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ov2735_params
                         );

