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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(TP2852_MIPI);

#define SENSOR_MODE_HD_1CH              (1)
#define SENSOR_MODE_HD_2VC              (2)
#define SENSOR_MODE_HD_3VC              (3)
#define SENSOR_MODE_FHD_1CH             (4)
#define SENSOR_MODE_FHD_2VC             (5)
#define SENSOR_MODE                     (SENSOR_MODE_HD_3VC)

#define SENSOR_30FPS                    (1)
#define SENSOR_25FPS                    (2)
#define SENSOR_FPS                      (SENSOR_30FPS)

//============================================
//MIPI config
#if((SENSOR_MODE == SENSOR_MODE_HD_1CH) || (SENSOR_MODE == SENSOR_MODE_FHD_1CH))
#define SENSOR_MIPI_LANE_NUM            (4) //4 or 2
#else
#define SENSOR_MIPI_LANE_NUM            (4) //4
#endif
//============================================

//============================================
//BT656 config begin.
#define SENSOR_BT656_CH_TOTAL_NUM (1)
#define SENSOR_BT656_CH_DET_ENABLE CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_CH_DET_SEL CUS_VIF_BT656_EAV_DETECT
#define SENSOR_BT656_BIT_SWAP CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_8BIT_MODE CUS_SENSOR_FUNC_ENABLE
#define SENSOR_BT656_VSYNC_DELAY CUS_VIF_BT656_VSYNC_DELAY_AUTO
#define SENSOR_HSYCN_INVERT CUS_SENSOR_FUNC_DISABLE
#define SENSOR_VSYCN_INVERT CUS_SENSOR_FUNC_DISABLE
#define SENSOR_CLAMP_ENABLE CUS_SENSOR_FUNC_DISABLE
//BT656 config end.
//============================================


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
//#define SENSOR_MIPI_DELAY   0x1212                  //CFG
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_16    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_RG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number SENSOR_MIPI_LANE_NUM
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_24MHZ       //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec
#define Prv_Max_line_number 1080                    //maximum exposure line munber of sensor when preview
#define vts_30fps 1125//1346,1616                      //for 29.1fps
#define Preview_WIDTH       1280                    //resolution Width when preview
#define Preview_HEIGHT      720                    //resolution Height when preview
#define Preview_MAX_FPS     25                     //fastest preview FPS
#define Preview_MIN_FPS     8                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

//#define Capture_MCLK_SPEED  CUS_CMU_CLK_16M     //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Cap_Max_line_number 720                   //maximum exposure line munber of sensor when capture

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A8D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16
#define SENSOR_I2C_ADDR     0x88                   //I2C slave address
#define SENSOR_I2C_SPEED   	20000 	//200KHz
#define SENSOR_I2C_CHANNEL 	1							//I2C Channel
#define SENSOR_I2C_PAD_MODE 2							//Pad/Mode Number

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG//CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
int tp2852_cus_camsensor_release_handle(ms_cus_sensor *handle);
#if 0
static CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};
#endif
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
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty; //sensor setting need to update through I2C
} tp2852_params;

#if(SENSOR_MODE == SENSOR_MODE_HD_1CH)
static I2C_ARRAY Sensor_init_table_HD_1CH[] =
{
    //video setting
    {0x40, 0x00}, //video page
    {0x4e, 0x00},
    {0xf5, 0xf0},

    {0x02, 0xc2},
    {0x07, 0xc0},
    {0x0b, 0xc0},
    {0x0c, 0x13},
    {0x0d, 0x50},

    {0x15, 0x13},
    {0x16, 0x15},
    {0x17, 0x00},
    {0x18, 0x19},
    {0x19, 0xd0},
    {0x1a, 0x25},

#if(SENSOR_FPS == SENSOR_30FPS)
    {0x1c, 0x06},  //1280*720, 30fps
    {0x1d, 0x72},  //1280*720, 30fps
#else
    {0x1c, 0x07},  //1280*720, 25fps
    {0x1d, 0xbc},  //1280*720, 25fps
#endif

    {0x20, 0x30},
    {0x21, 0x84},
    {0x22, 0x36},
    {0x23, 0x3c},

    {0x2b, 0x60},
    {0x2c, 0x0a},
    {0x2d, 0x30},
    {0x2e, 0x70},

    {0x30, 0x48},
    {0x31, 0xbb},
    {0x32, 0x2e},
    {0x33, 0x90},

    {0x35, 0x25},
    {0x38, 0x00},
    {0x39, 0x18},


    //mipi setting
    {0x40, 0x08}, //MIPI page
    {0x01, 0xf8},
    {0x02, 0x01},
    {0x08, 0x0f},
    {0x13, 0x24}, //0x04,

#if(SENSOR_MIPI_LANE_NUM == 4)
    {0x14, 0x56},
    {0x15, 0x0a},
    {0x20, 0x14}, //1CH, MIPI 4 Lane
#elif(SENSOR_MIPI_LANE_NUM == 2)
    {0x14, 0x46},
    {0x15, 0x08},
    {0x20, 0x12},
#endif

    {0x34, 0x1b},

    /* Disable MIPI CSI2 output */
    {0x23, 0x02},

    /* Enable MIPI CSI2 output */
    {0x23, 0x00},
};
#endif

#if(SENSOR_MODE == SENSOR_MODE_HD_2VC)
static I2C_ARRAY Sensor_init_table_HD_2VC[] =
{
    //video setting
    {0x40, 0x04}, //video page, write all
    {0x4e, 0x00},
    {0xf5, 0xf0},

    {0x02, 0xc2},
    {0x07, 0xc0},
    {0x0b, 0xc0},
    {0x0c, 0x13},
    {0x0d, 0x50},

    {0x15, 0x13},
    {0x16, 0x15},
    {0x17, 0x00},
    {0x18, 0x19},
    {0x19, 0xd0},
    {0x1a, 0x25},
#if(SENSOR_FPS == SENSOR_30FPS)
    {0x1c, 0x06},  //1280*720, 30fps
    {0x1d, 0x72},  //1280*720, 30fps
#else
    {0x1c, 0x07},  //1280*720, 25fps
    {0x1d, 0xbc},  //1280*720, 25fps
#endif
    {0x20, 0x30},
    {0x21, 0x84},
    {0x22, 0x36},
    {0x23, 0x3c},

    {0x2b, 0x60},
    {0x2c, 0x0a},
    {0x2d, 0x30},
    {0x2e, 0x70},

    {0x30, 0x48},
    {0x31, 0xbb},
    {0x32, 0x2e},
    //0x32, 0x3e,//0x32, 0x2e,//color carrier
    {0x33, 0x90},

    {0x35, 0x25},
    {0x38, 0x00},
    {0x39, 0x18},

    //mipi setting
    {0x40, 0x08}, //MIPI page
    {0x01, 0xf8},
    {0x02, 0x01},
    {0x08, 0x0f},
    {0x13, 0x24},
    {0x14, 0x45},
    {0x15, 0x05},
    {0x20, 0x24},
    {0x34, 0x1b},

    /* Disable MIPI CSI2 output */
    {0x23, 0x02},

    /* Enable MIPI CSI2 output */
    {0x23, 0x00},
};
#endif //(SENSOR_MODE == SENSOR_MODE_HD_2VC)

#if(SENSOR_MODE == SENSOR_MODE_HD_3VC)
static I2C_ARRAY Sensor_init_table_HD_3VC[] =
{
    //video setting
    {0x40, 0x04}, //video page, write all
    {0x4e, 0x00},
    {0xf5, 0xf0},

    {0x02, 0xc2},
    {0x07, 0xc0},
    {0x0b, 0xc0},
    {0x0c, 0x13},
    {0x0d, 0x50},

    {0x15, 0x13},
    {0x16, 0x15},
    {0x17, 0x00},
    {0x18, 0x19},
    {0x19, 0xd0},
    {0x1a, 0x25},
#if(SENSOR_FPS == SENSOR_30FPS)
    {0x1c, 0x06},  //1280*720, 30fps
    {0x1d, 0x72},  //1280*720, 30fps
#else
    {0x1c, 0x07},  //1280*720, 25fps
    {0x1d, 0xbc},  //1280*720, 25fps
#endif
    {0x20, 0x30},
    {0x21, 0x84},
    {0x22, 0x36},
    {0x23, 0x3c},

    {0x2b, 0x60},
    {0x2c, 0x0a},
    {0x2d, 0x30},
    {0x2e, 0x70},

    {0x30, 0x48},
    {0x31, 0xbb},
    {0x32, 0x2e},
    //0x32, 0x3e,//0x32, 0x2e,//color carrier
    {0x33, 0x90},

    {0x35, 0x25},
    {0x38, 0x00},
    {0x39, 0x18},

    //mipi setting
    {0x40, 0x08}, //MIPI page
    {0x01, 0xf8},
    {0x02, 0x01},
    {0x08, 0x0f},
    {0x13, 0x24},
    {0x14, 0x04}, //0x45,
    {0x15, 0x00}, //0x05,
    {0x20, 0x34}, //0x24,
    {0x34, 0x1b},

    /* Disable MIPI CSI2 output */
    {0x23, 0x02},

    {0xffff, 20}, //Delay
    
    /* Enable MIPI CSI2 output */
    {0x23, 0x00},
};
#endif //(SENSOR_MODE == SENSOR_MODE_HD_3VC)

#if(SENSOR_MODE == SENSOR_MODE_FHD_1CH)
static I2C_ARRAY Sensor_init_table_FHD_1CH[] =
{
//video setting
    {0x40, 0x04}, //Old:0x00, //video page, write all
    {0x4e, 0x00},
    {0xf5, 0xf0},

    {0x02, 0xc0},
    {0x07, 0xc0},
    {0x0b, 0xc0},
    {0x0c, 0x03},
    {0x0d, 0x50},

    {0x15, 0x03},
    {0x16, 0xd2},
    {0x17, 0x80},
    {0x18, 0x29},
    {0x19, 0x38},
    {0x1a, 0x47},

#if(SENSOR_FPS == SENSOR_30FPS)
    {0x1c, 0x08},  //1920*1080, 30fps
    {0x1d, 0x98},  //
#else
    {0x1c, 0x0a},  //1920*1080, 25fps
    {0x1d, 0x50},  //
#endif

    {0x20, 0x30},
    {0x21, 0x84},
    {0x22, 0x36},
    {0x23, 0x3c},

    {0x2b, 0x60},
    {0x2c, 0x0a},
    {0x2d, 0x30},
    {0x2e, 0x70},

    {0x30, 0x48},
    {0x31, 0xbb},
    {0x32, 0x2e},
    {0x33, 0x90},

    {0x35, 0x05},
    {0x38, 0x00},
    {0x39, 0x1C},


    //mipi setting
    {0x40, 0x08}, //MIPI page
    {0x01, 0xf8},
    {0x02, 0x01},
    {0x08, 0x0f},
    {0x13, 0x04},

//0x25, 0x08,
//0x26, 0x01,
//0x27, 0x0e,

#if(SENSOR_MIPI_LANE_NUM == 4)
    {0x14, 0x45},
    {0x15, 0x05},
    {0x20, 0x14}, //1CH, MIPI 4 Lane
#elif(SENSOR_MIPI_LANE_NUM == 2)
	{0x14, 0x46},
    {0x15, 0x08},
    {0x20, 0x12}, //1CH, MIPI 2 Lane
#endif

	{0x34, 0x1b},

    /* Disable MIPI CSI2 output */
    {0x23, 0x02},

    {0xffff, 20}, //Delay
	
    /* Enable MIPI CSI2 output */
    {0x23, 0x00},


};
#endif //(SENSOR_MODE == SENSOR_MODE_FHD_1CH)

#if(SENSOR_MODE == SENSOR_MODE_FHD_2VC)
static I2C_ARRAY Sensor_init_table_FHD_2VC[] =
{
    //video setting
    {0x40, 0x04}, //video page, write all
    {0x4e, 0x00},
    {0xf5, 0xf0},

    {0x02, 0xc0},
    {0x07, 0xc0},
    {0x0b, 0xc0},
    {0x0c, 0x03},
    {0x0d, 0x50},

    {0x15, 0x03},
    {0x16, 0xd2},
    {0x17, 0x80},
    {0x18, 0x29},
    {0x19, 0x38},
    {0x1a, 0x47},

#if(SENSOR_FPS == SENSOR_30FPS)
    {0x1c, 0x08},  //1920*1080, 30fps
    {0x1d, 0x98},  //
#else
    {0x1c, 0x0a},  //1920*1080, 25fps
    {0x1d, 0x50},  //
#endif

    {0x20, 0x30},
    {0x21, 0x84},
    {0x22, 0x36},
    {0x23, 0x3c},

    {0x2b, 0x60},
    {0x2c, 0x0a},
    {0x2d, 0x30},
    {0x2e, 0x70},

    {0x30, 0x48},
    {0x31, 0xbb},
    {0x32, 0x2e},
    {0x33, 0x90},

    {0x35, 0x05},
    {0x38, 0x00},
    {0x39, 0x1C},

    //mipi setting
    {0x40, 0x08}, //MIPI page
    {0x01, 0xf8},
    {0x02, 0x01},
    {0x08, 0x0f},
    {0x13, 0x04},

#if (SENSOR_MIPI_LANE_NUM == 4) //MIPI 4 Lanes
    {0x14, 0x45},
    {0x15, 0x05},
    {0x20, 0x24},
#endif	
#if (SENSOR_MIPI_LANE_NUM == 2) //MIPI 2 Lanes
    {0x14, 0x05},
    {0x15, 0x04},
    {0x20, 0x22},

    {0x25, 0x08},
    {0x26, 0x06},
    {0x27, 0x11},
#endif

    {0x34, 0x1b},


    /* Disable MIPI CSI2 output */
    {0x23, 0x02},

    {0xffff, 20}, //Delay
	
    /* Enable MIPI CSI2 output */
    {0x23, 0x00},
};
#endif //(SENSOR_MODE == SENSOR_MODE_FHD_2VC)

#if 0
static I2C_ARRAY TriggerStartTbl[] =
{

};

static I2C_ARRAY PatternTbl[] =
{

};
#endif
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

typedef struct
{
	short reg;
	char startbit;
	char stopbit;
} COLLECT_REG_SET;

#if 0
static I2C_ARRAY mirror_reg[] =
{

};

static I2C_ARRAY gain_reg[] =
{

};
#endif
static int g_sensor_ae_min_gain = 1024;
static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] =
{

};
#if 0
static I2C_ARRAY expo_reg[] =
{

};

static I2C_ARRAY vts_reg[] =
{

};
#endif
/*
static CUS_INT_TASK_ORDER def_order =
{
	.RunLength = 9,
	.Orders =
	{
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
};*/

/////////// function definition ///////////////////
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////
static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    //Sensor power on sequence
    //sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);		//Set_csi_if(0, 0); //disable MIPI
    //
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0x4000, 0x0, 0);

    sensor_if->MCLK(idx, 1, handle->mclk);
    //
    #if 0
    SENSOR_EMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    SENSOR_EMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_MSLEEP(5);

    // power -> high, reset -> high
    SENSOR_EMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_MSLEEP(5);
    SENSOR_EMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(5);

    //sensor_if->Set3ATaskOrder( def_order);
    // pure power on
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    #endif
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_MSLEEP(5);
    
    return SUCCESS;
}
static int pCus_poweron_dummy(ms_cus_sensor *handle, u32 idx)
{   
    
return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{

    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);

    SENSOR_MSLEEP(1);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);		//Set_csi_if(0, 0);
    sensor_if->MCLK(idx, 0, handle->mclk);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    
    return SUCCESS;
}
static int pCus_poweroff_dummy(ms_cus_sensor *handle, u32 idx)
{
    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
#if 0
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

	for(n=0;n<4;++n)		//retry , until I2C success
	{
		if(n>2) return FAIL;

		if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS)		//read sensor ID from I2C
		{
			//convert sensor id to u32 format
			for(i=0;i<table_length;++i)
			{
				*id = id_from_sensor[i].data;
                SENSOR_DMSG("[%s] %s read id[%d], get 0x%x\n", __FUNCTION__, handle->model_id, i, ( int )*id);

				if( id_from_sensor[i].data != Sensor_id_table[i].data )
				{
				    SENSOR_EMSG("[%s] %s,get 0x%x, error id!\n", __FUNCTION__, handle->model_id, Sensor_id_table[i].data);
					return FAIL;
				}
			}
			break;
		}
		else
		    SENSOR_MSLEEP(10);//usleep(1*1000);
	}
	SENSOR_IMSG("[%s] %s get sensor id:0x%x, SUCCESS!\n", __FUNCTION__, handle->model_id, ( int )*id);
#endif
	return SUCCESS;
}

static int tp2852_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
#if 0
	int i;
    	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	switch(mode)
	{
		case 1:
			PatternTbl[2].data = 0x08;
			PatternTbl[3].data = 0x30;
			PatternTbl[4].data = 0x00;
			PatternTbl[6].data |= 0x01; //enable
			break;
		case 0:
		default:
			PatternTbl[2].data = 0x08;
			PatternTbl[3].data = 0x34;
			PatternTbl[4].data = 0x3C;
			PatternTbl[6].data &= ~0x01; //disable
			break;
	}

	for(i=0;i< ARRAY_SIZE(PatternTbl);i++)
	{
		if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
		{
			SENSOR_EMSG("[%s:%d]SensorReg_Write fail!!\n", __FUNCTION__, __LINE__);
			return FAIL;
		}
	}
#endif
	return SUCCESS;
}

#if 0//defined(__MV5_FPGA__)
static int pCus_I2CWrite(ms_cus_sensor *handle, unsigned short usreg, unsigned short usval)
{
    unsigned short sen_data;

    SensorReg_Write(usreg, usval);
    SensorReg_Read(usreg, &sen_data);
    UartSendTrace("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n", usreg, usval, sen_data);

    return SUCCESS;
}

static int pCus_I2CRead(ms_cus_sensor *handle, unsigned short usreg, unsigned short* pusval)
{
    unsigned short usread_data;

    SensorReg_Read(usreg, &usread_data);
    *pusval = usread_data;
    UartSendTrace("tp2852 reg: 0x%x, data: 0x%x\n", usreg, usread_data);

    return SUCCESS;
}
#endif

#if(SENSOR_MODE == SENSOR_MODE_HD_1CH)
static int pCus_init_HD_1CH(ms_cus_sensor *handle)
{
    int i,cnt=0;
    unsigned short sen_data;

    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_EMSG("\n===== tp2852 HD_1CH init. =====\n");

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HD_1CH);i++)
    {
        if(Sensor_init_table_HD_1CH[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HD_1CH[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("{0x%x,  0x%2x},\n", Sensor_init_table_HD_1CH[i].reg, Sensor_init_table_HD_1CH[i].data);
            while(SensorReg_Write(Sensor_init_table_HD_1CH[i].reg,Sensor_init_table_HD_1CH[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("[%s] set Sensor_init_table_HD_1CH retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);//usleep(10*1000);
            }
#if 1
            SensorReg_Read(Sensor_init_table_HD_1CH[i].reg, &sen_data);
            SENSOR_EMSG("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HD_1CH[i].reg, Sensor_init_table_HD_1CH[i].data, sen_data);
#endif
        }
    }

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

    return SUCCESS;
}
#endif

#if(SENSOR_MODE == SENSOR_MODE_HD_2VC)
static int pCus_init_HD_2VC(ms_cus_sensor *handle)
{
    int i,cnt=0;
    unsigned short sen_data;

    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_EMSG("\n===== tp2852 HD_2VC init. =====\n");

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HD_2VC);i++)
    {
        if(Sensor_init_table_HD_2VC[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HD_2VC[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("{0x%x,  0x%2x},\n", Sensor_init_table_HD_2VC[i].reg, Sensor_init_table_HD_2VC[i].data);
            while(SensorReg_Write(Sensor_init_table_HD_2VC[i].reg,Sensor_init_table_HD_2VC[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("[%s] set Sensor_init_table_HD_2VC retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);//usleep(10*1000);
            }
#if 1
            SensorReg_Read(Sensor_init_table_HD_2VC[i].reg, &sen_data);
            SENSOR_EMSG("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HD_2VC[i].reg, Sensor_init_table_HD_2VC[i].data, sen_data);
#endif
        }
    }

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

    return SUCCESS;
}
#endif

#if(SENSOR_MODE == SENSOR_MODE_HD_3VC)
static int pCus_init_HD_3VC(ms_cus_sensor *handle)
{
    int i,cnt=0;
    unsigned short sen_data;

    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_EMSG("\n===== tp2852 HD_3VC init. =====\n");

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_HD_3VC);i++)
    {
        if(Sensor_init_table_HD_3VC[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_HD_3VC[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("{0x%x,  0x%2x},\n", Sensor_init_table_HD_3VC[i].reg, Sensor_init_table_HD_3VC[i].data);
            while(SensorReg_Write(Sensor_init_table_HD_3VC[i].reg,Sensor_init_table_HD_3VC[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("[%s] set Sensor_init_table_HD_3VC retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);//usleep(10*1000);
            }
#if 1
            SensorReg_Read(Sensor_init_table_HD_3VC[i].reg, &sen_data);
            SENSOR_EMSG("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_HD_3VC[i].reg, Sensor_init_table_HD_3VC[i].data, sen_data);
#endif
        }
    }

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

    return SUCCESS;
}
#endif

#if(SENSOR_MODE == SENSOR_MODE_FHD_1CH)
static int pCus_init_FHD_1CH(ms_cus_sensor *handle)
{
    int i,cnt=0;
    unsigned short sen_data;

    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_EMSG("\n===== tp2852 FHD_1CH init. =====\n");

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_FHD_1CH);i++)
    {
        if(Sensor_init_table_FHD_1CH[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_FHD_1CH[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("{0x%x,  0x%2x},\n", Sensor_init_table_FHD_1CH[i].reg, Sensor_init_table_FHD_1CH[i].data);
            while(SensorReg_Write(Sensor_init_table_FHD_1CH[i].reg,Sensor_init_table_FHD_1CH[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("[%s] set Sensor_init_table_FHD_1CH retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);//usleep(10*1000);
            }
#if 1
            SensorReg_Read(Sensor_init_table_FHD_1CH[i].reg, &sen_data);
            SENSOR_EMSG("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_FHD_1CH[i].reg, Sensor_init_table_FHD_1CH[i].data, sen_data);
#endif
        }
    }

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

    return SUCCESS;
}
#endif

#if(SENSOR_MODE == SENSOR_MODE_FHD_2VC)
static int pCus_init_FHD_2VC(ms_cus_sensor *handle)
{
    int i,cnt=0;
    unsigned short sen_data;

    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_EMSG("\n===== tp2852 FHD_2VC init. =====\n");

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_FHD_2VC);i++)
    {
        if(Sensor_init_table_FHD_2VC[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_FHD_2VC[i].data);
        }
        else
        {
            cnt = 0;
            SENSOR_DMSG("{0x%x,  0x%2x},\n", Sensor_init_table_FHD_2VC[i].reg, Sensor_init_table_FHD_2VC[i].data);
            while(SensorReg_Write(Sensor_init_table_FHD_2VC[i].reg,Sensor_init_table_FHD_2VC[i].data) != SUCCESS)
            {
                cnt++;
                SENSOR_DMSG("[%s] set Sensor_init_table_FHD_2VC retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(10);//usleep(10*1000);
            }
#if 1
            SensorReg_Read(Sensor_init_table_FHD_2VC[i].reg, &sen_data);
            SENSOR_EMSG("tp2852 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_FHD_2VC[i].reg, Sensor_init_table_FHD_2VC[i].data, sen_data);
#endif
        }
    }

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

    return SUCCESS;
}
#endif

static int pCus_init_dummy(ms_cus_sensor *handle)
{
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
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    handle->video_res_supported.ulcur_res = 0; //TBD
    //TODO: Set sensor output resolution
    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
#if 0
	char sen_data;

	sen_data = mirror_reg[0].data;
	SENSOR_DMSG("[%s] mirror:%x\r\n", __FUNCTION__, sen_data & 0x03);
	switch(sen_data & 0x03)
	{
		case 0x00:
			*orit = CUS_ORIT_M0F0;
			break;
		case 0x02:
			*orit = CUS_ORIT_M1F0;
			break;
		case 0x01:
			*orit = CUS_ORIT_M0F1;
			break;
		case 0x03:
			*orit = CUS_ORIT_M1F1;
			break;
	}
#endif
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
#if 0
	char index=0;
	tp2852_params *params = (tp2852_params *)handle->private_data;

	switch(orit)
	{
		case CUS_ORIT_M0F0:
			index = 0;
			break;
		case CUS_ORIT_M1F0:
			index = 1;
			break;
		case CUS_ORIT_M0F1:
			index = 2;
			break;
		case CUS_ORIT_M1F1:
			index = 3;
			break;
	}
	SENSOR_DMSG("[%d] CUS_CAMSENSOR_ORIT orit : %x\r\n",__FUNCTION__, orit);
	if (index != mirror_reg[0].data)
	{
		mirror_reg[0].data = index;
		SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
		params->reg_dirty = true;
	}
#endif
	return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
	//tp2852_params *params = (tp2852_params *)handle->private_data;
	//SENSOR_DMSG("[%s] FPS %d\n", __FUNCTION__, params->expo.fps);

	return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
#if 0
	tp2852_params *params = (tp2852_params *)handle->private_data;

	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	return 0; //test only

	if(fps>=3 && fps <= 30)
	{
		params->expo.fps = fps;
		params->expo.vts=  (vts_30fps*30.1f)/fps;
		//vts_reg[0].data = (params->expo.vts>> 16) & 0x0003;
		vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
		vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
	   // params->reg_dirty = true; //reg need to update = true;
	   return SUCCESS;
	}
	else if(fps>=3000 && fps <= 30000)
	{
		params->expo.fps = fps;
		params->expo.vts=  (vts_30fps*30100.0f)/fps;
		vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
		vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
		return SUCCESS;
	}
	else
	{
		//SENSOR_EMSG("[%s] FPS %d out of range.\n",fps,__FUNCTION__);
		return FAIL;
	}
#endif
    return SUCCESS;
}
#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	if (cap)
		memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
	else
		return FAIL;
	return SUCCESS;
}
#endif

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
	tp2852_params *params = (tp2852_params *)handle->private_data;
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	switch(status)
	{
		case CUS_FRAME_INACTIVE:
			//SensorReg_Write(0x3001,0);
			break;
		case CUS_FRAME_ACTIVE:
			if(params->reg_dirty)
			{
				//SensorReg_Write(0x3001,1);

				// SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
				// SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
				// SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
				//  SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));

				//SensorReg_Write(0x3001,0);
				params->reg_dirty = false;
			}
			break;
		default :
			break;
	}
	return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
#if 0
	//int rc;
	u32 lines = 0;
	//rc = SensorRegArrayR((I2C_ARRAY*)expo_reg, ARRAY_SIZE(expo_reg));

	//lines |= (u32)(expo_reg[0].data&0xff)<<16;
	lines |= (u32)(expo_reg[0].data&0xff)<<8;
	lines |= (u32)(expo_reg[1].data&0xff)<<0;
	// lines >>= 4;
	// *us = (lines+dummy) * params->expo.us_per_line;
	*us = lines;//(lines*Preview_line_period);
	SENSOR_DMSG("[%s] sensor expo lines/us 0x%x,0x%x us\n", __FUNCTION__, lines, *us);
#endif
	return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
#if 0
	u32 lines = 0, vts = 0;
	tp2852_params *params = (tp2852_params *)handle->private_data;

	return 0; //test only

	lines=(1000*us)/Preview_line_period;
	if(lines<1) lines=1;
	if (lines >params->expo.vts-1)
	{
		vts = lines +1;
	}
	else
	vts=params->expo.vts;

	// lines=us/Preview_line_period;
	SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,us,lines,params->expo.vts);
	lines=vts-lines-1;
	//expo_reg[0].data = (lines>>16) & 0x0003;
	expo_reg[0].data = (lines>>8) & 0x00ff;
	expo_reg[1].data = (lines>>0) & 0x00ff;

	// vts_reg[0].data = (vts >> 16) & 0x0003;
	vts_reg[0].data = (vts >> 8) & 0x00ff;
	vts_reg[1].data = (vts >> 0) & 0x00ff;

	SensorReg_Write(0x3001,1);
	SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
	SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
	SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
	//SensorReg_Write(0x3001,0);
	params->reg_dirty = true; //reg need to update
#endif
	return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
#if 0
	//int rc = SensorRegArrayR((I2C_ARRAY*)gain_reg, ARRAY_SIZE(gain_reg));
	u16 temp_gain;
	double temp_gain_double;

	temp_gain=gain_reg[0].data;
	temp_gain_double=((double)(temp_gain*3)/200.0f);
	*gain=(u32)(pow(10,temp_gain_double)*1024);

	SENSOR_DMSG("[%s] get gain/reg (1024=1X)= 0x%x/0x%x\n", __FUNCTION__, *gain,gain_reg[0].data);
#endif
	return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
#if 0
	tp2852_params *params = (tp2852_params *)handle->private_data;
	//double gain_double;
	params->expo.final_gain = gain;

	return 0; //test only

	if(gain<1024)
		gain=1024;
	else if(gain>=177*1024)
		gain=177*1024;

	//gain_double = 20*log10((double)gain/1024);
	//gain_reg[0].data=(u16)((gain_double*10)/3);
	params->reg_dirty = true;    //reg need to update

	SENSOR_DMSG("[%s] set gain/reg=0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[0].data);
#endif
	return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
#if 0
	//extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
	tp2852_params *params = (tp2852_params *)handle->private_data;
	u32 i;
	CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
	//double gain_double;
	// u32 times = log2((double)gain/1024.0f)/log(2);

	return 0; //test only

	if(gain<1024)
	gain=1024;
	else if(gain>=177*1024)
	gain=177*1024;

	Sensor_Gain_Linearity = gain_gap_compensate;

	for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++)
	{
		//SENSOR_DMSG("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

		if (Sensor_Gain_Linearity[i].gain == 0)
			break;
		if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset)))
		{
			gain=Sensor_Gain_Linearity[i].gain;
			break;
		}
	}

	gain = (gain * handle->sat_mingain+512)>>10;
	params->expo.final_gain = gain;
	//gain_double = 20*log10((double)gain/1024);
	//gain_reg[0].data=(u16)((gain_double*10)/3);
	// params->reg_dirty = true;    //reg need to update
	SENSOR_DMSG("[%s] set gain/reg=0x%x/0x%x\n", __FUNCTION__, gain,gain_reg[0].data);
#endif
	return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	*min = 1;
	*max = 1000000/4;
	return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	*min = 1024;
	*max = 177*1024;
	return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
	u32 i, j;

	for(i=0,j=0;i< num ;i++,j+=2)
	{
		gain_gap_compensate[i].gain=pArray[i].gain;
		gain_gap_compensate[i].offset=pArray[i].offset;
	}
	SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);
	return SUCCESS;
}

static int tp2852_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	info->max = 1000000000/Preview_MIN_FPS;
	info->min = Preview_line_period * 5;
	info->step = Preview_line_period;
	return SUCCESS;
}
#if 0
static int pCus_BT656_GetSrcType(ms_cus_sensor *handle, CUS_SNR_ANADEC_SRC_TYPE *psrc_type)
{
    unsigned short usreg, usval;

    *psrc_type = CUS_SNR_ANADEC_SRC_NO_READY;

    if(pCus_I2CRead == NULL){
        SENSOR_EMSG("[%s] I2C function is not ready!\n", __FUNCTION__);
        return FAIL;
    }

    //TBD
    *psrc_type = CUS_SNR_ANADEC_SRC_HD;

    return SUCCESS;
}
#endif
int tp2852_cus_camsensor_init_handle_ch0(ms_cus_sensor* handle)
{
    tp2852_params *params;
    if (!handle)
    {
        SENSOR_EMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (tp2852_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"tp2852_MIPI_CH0");

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

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    #if (SENSOR_MODE == SENSOR_MODE_HD_1CH || SENSOR_MODE == SENSOR_MODE_HD_2VC || SENSOR_MODE == SENSOR_MODE_HD_3VC)
    handle->video_res_supported.res[0].width = 1280;
    handle->video_res_supported.res[0].height = 720;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1280;
    handle->video_res_supported.res[0].nOutputHeight= 720;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1280x720");
    #elif (SENSOR_MODE == SENSOR_MODE_FHD_1CH || SENSOR_MODE == SENSOR_MODE_FHD_2VC)
    handle->video_res_supported.res[0].width = 1920;
    handle->video_res_supported.res[0].height = 1080;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080");
    #endif

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //200000;


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
    handle->sat_mingain=g_sensor_ae_min_gain;
    handle->pCus_sensor_release     = tp2852_cus_camsensor_release_handle;
    #if (SENSOR_MODE == SENSOR_MODE_HD_1CH)
    handle->pCus_sensor_init        = pCus_init_HD_1CH;
    #elif (SENSOR_MODE == SENSOR_MODE_HD_2VC)
    handle->pCus_sensor_init        = pCus_init_HD_2VC;
    #elif (SENSOR_MODE == SENSOR_MODE_HD_3VC)
    handle->pCus_sensor_init        = pCus_init_HD_3VC;
    #elif (SENSOR_MODE == SENSOR_MODE_FHD_1CH)
    handle->pCus_sensor_init        = pCus_init_FHD_1CH;
    #elif (SENSOR_MODE == SENSOR_MODE_FHD_2VC)
    handle->pCus_sensor_init        = pCus_init_FHD_2VC;
    #endif
    handle->pCus_sensor_poweron     = pCus_poweron ;
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
    handle->pCus_sensor_SetPatternMode    = tp2852_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify   = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs       = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs       = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain        = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal    = pCus_SetAEGain_cal;
    handle->pCus_sensor_SetAEGain        = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo   = tp2852_GetShutterInfo;
    handle->pCus_sensor_GetAEMinMaxGain  = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs = pCus_GetAEMinMaxUSecs;

    //sensor calibration
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    params->expo.vts  = vts_30fps;
    params->expo.fps  = 30;
    params->reg_dirty = false;

    //MIPI config
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_number;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_YUV422; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = CUS_SENSOR_YUV_ORDER_CY; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_MULTI_VC;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0;
    return SUCCESS;
}

int tp2852_cus_camsensor_init_handle_ch1(ms_cus_sensor* handle)
{
    tp2852_params *params;
    if (!handle)
    {
        SENSOR_EMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (tp2852_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"tp2852_MIPI_CH1");

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

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    #if (SENSOR_MODE == SENSOR_MODE_HD_1CH || SENSOR_MODE == SENSOR_MODE_HD_2VC || SENSOR_MODE == SENSOR_MODE_HD_3VC)
    handle->video_res_supported.res[0].width = 1280;
    handle->video_res_supported.res[0].height = 720;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1280;
    handle->video_res_supported.res[0].nOutputHeight= 720;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1280x720");
    #elif (SENSOR_MODE == SENSOR_MODE_FHD_1CH || SENSOR_MODE == SENSOR_MODE_FHD_2VC)
    handle->video_res_supported.res[0].width = 1920;
    handle->video_res_supported.res[0].height = 1080;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080");
    #endif

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //200000;


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
    handle->sat_mingain=g_sensor_ae_min_gain;
    handle->pCus_sensor_release     = tp2852_cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_dummy    ;
    handle->pCus_sensor_poweron     = pCus_poweron_dummy ;
    handle->pCus_sensor_poweroff    = pCus_poweroff_dummy;

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
    handle->pCus_sensor_SetPatternMode    = tp2852_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify   = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs       = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs       = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain        = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal    = pCus_SetAEGain_cal;
    handle->pCus_sensor_SetAEGain        = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo   = tp2852_GetShutterInfo;
    handle->pCus_sensor_GetAEMinMaxGain  = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs = pCus_GetAEMinMaxUSecs;

    //sensor calibration
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    params->expo.vts  = vts_30fps;
    params->expo.fps  = 30;
    params->reg_dirty = false;

    //MIPI config
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_number;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_YUV422; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = CUS_SENSOR_YUV_ORDER_CY; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_MULTI_VC;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 1;
    return SUCCESS;
}


int tp2852_cus_camsensor_init_handle_ch2(ms_cus_sensor* handle)
{
    tp2852_params *params;
    if (!handle)
    {
        SENSOR_EMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (tp2852_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"tp2852_MIPI_CH2");

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

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res = 1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    #if (SENSOR_MODE == SENSOR_MODE_HD_1CH || SENSOR_MODE == SENSOR_MODE_HD_2VC || SENSOR_MODE == SENSOR_MODE_HD_3VC)
    handle->video_res_supported.res[0].width = 1280;
    handle->video_res_supported.res[0].height = 720;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1280;
    handle->video_res_supported.res[0].nOutputHeight= 720;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1280x720");
    #elif (SENSOR_MODE == SENSOR_MODE_FHD_1CH || SENSOR_MODE == SENSOR_MODE_FHD_2VC)
    handle->video_res_supported.res[0].width = 1920;
    handle->video_res_supported.res[0].height = 1080;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080");
    #endif

    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D16;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x5a;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //200000;


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
    handle->sat_mingain=g_sensor_ae_min_gain;
    handle->pCus_sensor_release     = tp2852_cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init_dummy    ;
    handle->pCus_sensor_poweron     = pCus_poweron_dummy ;
    handle->pCus_sensor_poweroff    = pCus_poweroff_dummy;

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
    handle->pCus_sensor_SetPatternMode    = tp2852_SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify   = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs       = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs       = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain        = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal    = pCus_SetAEGain_cal;
    handle->pCus_sensor_SetAEGain        = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo   = tp2852_GetShutterInfo;
    handle->pCus_sensor_GetAEMinMaxGain  = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs = pCus_GetAEMinMaxUSecs;

    //sensor calibration
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    params->expo.vts  = vts_30fps;
    params->expo.fps  = 30;
    params->reg_dirty = false;

    //MIPI config
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_number;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_YUV422; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = CUS_SENSOR_YUV_ORDER_CY; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_MULTI_VC;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 2;
    return SUCCESS;
}


int tp2852_cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

#if (SENSOR_MODE == SENSOR_MODE_HD_1CH || SENSOR_MODE == SENSOR_MODE_FHD_1CH)
SENSOR_DRV_ENTRY_IMPL_END_EX(tp2852,
                            tp2852_cus_camsensor_init_handle_ch0,
                            NULL,
                            NULL,
                            tp2852_params);
#elif (SENSOR_MODE == SENSOR_MODE_HD_2VC || SENSOR_MODE == SENSOR_MODE_FHD_2VC)
SENSOR_DRV_ENTRY_3CHAHD_IMPL_END_EX(tp2852,
                                    tp2852_cus_camsensor_init_handle_ch0,
                                    tp2852_cus_camsensor_init_handle_ch0,
                                    tp2852_cus_camsensor_init_handle_ch1,
                                    NULL,
                                    tp2852_params
                                    );
#elif (SENSOR_MODE == SENSOR_MODE_HD_3VC)
SENSOR_DRV_ENTRY_3CHAHD_IMPL_END_EX(tp2852,
                                    tp2852_cus_camsensor_init_handle_ch0,
                                    tp2852_cus_camsensor_init_handle_ch0,
                                    tp2852_cus_camsensor_init_handle_ch1,
                                    tp2852_cus_camsensor_init_handle_ch2,
                                    tp2852_params
                                    );
#endif
