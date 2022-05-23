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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SP2309);

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
#define SENSOR_DMSK(args...) //printk(args)
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
#define SENSOR_MIPI_LANE_NUM        (2) //(lane_num)          //SP2309 Linear mode supports MIPI 2/4 Lane
#define SENSOR_MIPI_LANE_NUM_DOL    (2) //(hdr_lane_num)//SP2309 DOL mode supports MIPI 4 Lane
//#define SENSOR_MIPI_HDR_MODE        (0) //0: Non-HDR mode. 1:Sony DOL mode

#define SENSOR_ISP_TYPE             ISP_EXT             //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER    //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_MIPI  //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE      PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9000  //CFG
#define SENSOR_BAYERID              CUS_BAYER_RG//CUS_BAYER_BG       //0h: CUS_BAYER_RG, 1h: CUS_BAYER_GR, 2h: CUS_BAYER_BG, 3h: CUS_BAYER_GB
#define SENSOR_RGBIRID              CUS_RGBIR_NONE
#define SENSOR_ORIT                 CUS_ORIT_M1F1        //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER              CUS_SEN_YCODR_YC     //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define long_packet_type_enable     0x00 //UD1~UD8 (user define)

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_24MHZ    //CUS_CMU_CLK_24MHZ //CUS_CMU_CLK_37P125MHZ//CUS_CMU_CLK_27MHZ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x78                //I2C slave address
#define SENSOR_I2C_SPEED             200000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL           1                 //I2C Channel
//#define SENSOR_I2C_PAD_MODE          2                 //Pad/Mode Number
#define SENSOR_I2C_LEGACY            I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT               I2C_FMT_A8D8       //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

////////////////////////////////////
// Sensor Signal                  //
////////////////////////////////////
#define SENSOR_PWDN_POL              CUS_CLK_POL_NEG     // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL               CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG
                                                         // VSYNC/HSYNC POL can be found in data sheet timing diagram,
                                                         // Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.
#define SENSOR_VSYNC_POL             CUS_CLK_POL_POS     // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL             CUS_CLK_POL_POS     // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL              CUS_CLK_POL_POS     // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

////////////////////////////////////
// Sensor ID                      //
////////////////////////////////////
//define SENSOR_ID

#undef SENSOR_NAME
#define SENSOR_NAME     SP2309

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
}sp2309_mipi_linear[] = {
    {LINEAR_RES_1, {1932, 1090, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps"}},
};

u32 Preview_line_period;
u32 vts_30fps;
u32 Preview_MAX_FPS;

//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
//#define long_packet_type_enable 0x00 //UD1~UD8 (user define)
////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (32 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)


#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

//static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
//static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);

int last_us, last_line, last_vts;
u32 last_input_gain;
u32 last_Gain_Slope;
u32 last_Gain_Table_Index, last_Gain_Reg_Sum, last_Gain_Pow;

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
    u32 max_rhs1;
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orien_dirty;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[3];
    I2C_ARRAY tExpo_reg[2];
} sp2309_params;

typedef struct {
    u64 total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

const static I2C_ARRAY Sensor_init_table_2lane_2m30fps[] =
{
    // 1080p 30fps data format raw10
    {0xfd, 0x00},
    {0x36, 0x01},
    {0xfd, 0x00},
    {0x36, 0x00},
    {0xfd, 0x00},
    {0x20, 0x00},
    {0xFF, 0x14}, //delay 14ms
    {0xfd, 0x00},
    {0x41, 0x05},
    {0xfd, 0x01},
    {0x03, 0x01},
    {0x04, 0x69},
    {0x06, 0x58},
    {0x0a, 0xa0},
    {0x24, 0x40},
    {0x01, 0x01},
    {0x12, 0x01},
    {0x11, 0x4f},
    {0x19, 0xa0},
    {0xd5, 0x44},
    {0xd6, 0x01},
    {0xd7, 0x19},
    {0x16, 0x38},
    {0x1d, 0xa1},
    {0x1f, 0x23},
    {0x21, 0x05},
    {0x25, 0x0f},
    {0x27, 0x46},
    {0x2a, 0x03},
    {0x2c, 0x05},
    {0x2b, 0x22},
    {0x20, 0x08},
    {0x38, 0x10},
    {0x45, 0xce},
    {0x51, 0x2a},
    {0x52, 0x2a},
    {0x55, 0x15},
    {0x57, 0x20},
    {0x66, 0x58},
    {0x68, 0x50},
    {0x71, 0xf0},
    {0x72, 0x25},
    {0x73, 0x30},
    {0x74, 0x25},
    {0x77, 0x02},
    {0x79, 0x01},
    {0x7a, 0x6f},
    {0x8a, 0x11},
    {0x8b, 0x11},
    {0xb1, 0x01},
    {0xc4, 0x7a},
    {0xc5, 0x7a},
    {0xc6, 0x7a},
    {0xc7, 0x7a},
    {0xf0, 0x40},
    {0xf1, 0x40},
    {0xf2, 0x40},
    {0xf3, 0x40},
    {0xf4, 0x08},
    {0xf7, 0xf7},
    {0xfe, 0xf7},
    {0x48, 0xf7},
    {0xfa, 0x10},
    {0xfb, 0x58},
    {0xb1, 0x03},
    {0xfd, 0x02},
    {0x34, 0xff},
    {0xa0, 0x00},
    {0xa1, 0x08},
    {0xa2, 0x04},
    {0xa3, 0x38},
    {0xa4, 0x00},
    {0xa5, 0x08},
    {0xa6, 0x07},
    {0xa7, 0x80},
    {0xfd, 0x01},
    {0x8e, 0x07},
    {0x8f, 0x80},
    {0x90, 0x04},
    {0x91, 0x38},
    {0x01, 0x01},
    {0xfd, 0x01},
    {0x3F, 0x03},
	{0xfd, 0x01},
};



const static I2C_ARRAY mirr_flip_table[] =
{
    {0x3F, 0x00},
    {0x3F, 0x01},
    {0x3F, 0x02},
    {0x3F, 0x03},
};

//static int g_sensor_ae_min_gain = 1024;
static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] = {  //compensate  gain gap
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
    {0x00, 0x00},
};
const static Gain_ARRAY gain_table[] = {
    {10000  ,0x10},
    {10625  ,0x11},
    {11250  ,0x12},
    {11875  ,0x13},
    {12500  ,0x14},
    {13125  ,0x15},
    {13750  ,0x16},
    {14375  ,0x17},
    {15000  ,0x18},
    {15625  ,0x19},
    {16250  ,0x1a},
    {16875  ,0x1b},
    {17500  ,0x1c},
    {18125  ,0x1d},
    {18750  ,0x1e},
    {19375  ,0x1f},
    {20000  ,0x20},
    {21250  ,0x22},
    {22500  ,0x24},
    {23750  ,0x26},
    {25000  ,0x28},
    {26250  ,0x2a},
    {27500  ,0x2c},
    {28750  ,0x2e},
    {30000  ,0x30},
    {31250  ,0x32},
    {32500  ,0x34},
    {33750  ,0x36},
    {35000  ,0x38},
    {36250  ,0x3a},
    {37500  ,0x3c},
    {38750  ,0x3e},
    {40000  ,0x40},
    {42500  ,0x44},
    {45000  ,0x48},
    {47500  ,0x4c},
    {50000  ,0x50},
    {52500  ,0x54},
    {55000  ,0x58},
    {57500  ,0x5c},
    {60000  ,0x60},
    {62500  ,0x64},
    {65000  ,0x68},
    {67500  ,0x6c},
    {70000  ,0x70},
    {72500  ,0x74},
    {75000  ,0x78},
    {77500  ,0x7c},
    {80000  ,0x80},
    {85000  ,0x88},
    {90000  ,0x90},
    {95000  ,0x98},
    {100000 ,0xa0},
    {105000 ,0xa8},
    {110000 ,0xb0},
    {115000 ,0xb8},
    {120000 ,0xc0},
    {125000 ,0xc8},
    {130000 ,0xd0},
    {135000 ,0xd8},
    {140000 ,0xe0},
    {145000 ,0xe8},
    {150000 ,0xf0},
    {155000 ,0xf8},
};

const static I2C_ARRAY expo_reg[] = {
    {0x03, 0x01},
    {0x04, 0x00},
};
const static I2C_ARRAY gain_reg[] = {
    {0x24, 0x00},
    {0x38, 0x10},
    {0x39, 0x08},
};
const static I2C_ARRAY vts_reg[] = {

    {0x0D, 0x10},
    {0x0E, 0x05},
    {0x0F, 0x46},
    //{0x0D, 0x01}, //test pettern
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] = {
     {0x3F, 0x03},    // bit[1:0]
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
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg), _reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg), _reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg), (_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg), (_reg),(_len)))

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

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSK("[%s] ", __FUNCTION__);

    //Sensor power on sequence
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);//!handle->pwdn_POLARITY); It should set off
    sensor_if->Reset(idx, handle->reset_POLARITY);//!handle->reset_POLARITY); It should set off
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL)
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 1);

    SENSOR_MSLEEP(5);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->MCLK(idx, 1, handle->mclk);
    //Sensor board PWDN Enable, 1.8V & 2.9V need 4ms then Pull High
    SENSOR_MSLEEP(4);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    //Sensor board PWDN Enable, clock need 4ms then Pull High
    SENSOR_DMSK("Sensor Power On finished\n");

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    SENSOR_DMSK("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    sensor_if->MCLK(idx, 0, handle->mclk);

    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    if (handle->interface_attr.attr_mipi.mipi_hdr_mode == CUS_HDR_MODE_SONY_DOL) {
        sensor_if->SetCSI_hdr_mode(idx, handle->interface_attr.attr_mipi.mipi_hdr_mode, 0);
    }
    handle->orient = SENSOR_ORIT;
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    return SUCCESS;
}

/////////////////// Check Sensor Product ID /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    return SUCCESS;
}

static int sp2309_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    switch(mode)
    {
        case 1:
            params->tVts_reg[0].data |= 0x01; //enable
            break;
        case 0:
            params->tVts_reg[0].data &= 0xFE; //disable
            break;
        default:
            params->tVts_reg[0].data &= 0xFE; //disable
            break;
    }

    SensorReg_Write(0xfd,0x01);
    if(SensorReg_Write(params->tVts_reg[0].reg, params->tVts_reg[0].data) != SUCCESS)
    {
        SENSOR_DMSK("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
        return FAIL;
    }
    return SUCCESS;
}

static int pCus_init_mipi2lane_2m60fps_linear(ms_cus_sensor *handle)
{
    int i,cnt=0, page=0, reg, data;
    printk(KERN_EMERG"[%s]\n", __FUNCTION__);
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2lane_2m30fps);i++)
    {
        if(Sensor_init_table_2lane_2m30fps[i].reg==0xff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2lane_2m30fps[i].data);
        }
        else
        {
            cnt = 0;
            reg = Sensor_init_table_2lane_2m30fps[i].reg;
            data = Sensor_init_table_2lane_2m30fps[i].data;
            while(SensorReg_Write(reg, data) != SUCCESS && !(!page && reg == 0x20))
            {
                cnt++;
                if(cnt>=10)
                {
                    return FAIL;
                }
                //usleep(10*1000);
            }
            if(reg == 0xfd)
                page = data;
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

    if (res_idx >= num_res)
        return FAIL;

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int pCus_GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res)
        return FAIL;

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int pCus_SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res)
        return FAIL;

    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->data_prec = CUS_DATAPRECISION_10;
            handle->pCus_sensor_init = pCus_init_mipi2lane_2m60fps_linear;
            vts_30fps = 1205;
            Preview_MAX_FPS = 30;
            Preview_line_period  = 27662;//27662; //(1/30)/1205 = 0.00002766251...
            break;
        default:
            SENSOR_DMSK("[WARN]Set resolution fail in error parameters\n");
            break;
    }
    params->expo.vts = vts_30fps;
    params->expo.fps = Preview_MAX_FPS;

    return SUCCESS;
}


static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    switch(Current_Mirror_Flip_Tbl[0].data)
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
    SENSOR_DMSK("[%s] orit: %d\n", __FUNCTION__, orit);
    SensorReg_Write(0xfd,0x01);//page 1
    switch(orit)
    {
        case CUS_ORIT_M0F0:

            SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
            break;
        case CUS_ORIT_M1F0:
            SensorReg_Write(mirr_flip_table[1].reg,mirr_flip_table[1].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[1].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[1].data;
            break;
        case CUS_ORIT_M0F1:
            SensorReg_Write(mirr_flip_table[2].reg,mirr_flip_table[2].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[2].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[2].data;
            break;
        case CUS_ORIT_M1F1:
            SensorReg_Write(mirr_flip_table[3].reg,mirr_flip_table[3].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[3].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[3].data;
            break;
        default :
            SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            Current_Mirror_Flip_Tbl[0].reg = mirr_flip_table[0].reg;
            Current_Mirror_Flip_Tbl[0].data = mirr_flip_table[0].data;
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;

    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;
    SENSOR_DMSK("[%s] Get fps: %d\n", __FUNCTION__, params->expo.preview_fps);
    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    SENSOR_DMSK("[%s]\n", __FUNCTION__);

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  vts_30fps * (max_fps/fps);
    }else if((fps>=(min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts = vts_30fps*(max_fps*1000/fps);
    }else{
        //params->expo.vts=vts_30fps;
        //params->expo.fps=25;
        SENSOR_DMSK("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
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
    sp2309_params *params = (sp2309_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty)
            {
                SensorReg_Write(0xfd,0x01);
                SensorReg_Write(0x01,0x00);//frame sync disable
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                SensorReg_Write(0x01,0x01);//frame sync enable
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
    sp2309_params *params = (sp2309_params *)handle->private_data;
    lines  =  (u32)(params->tExpo_reg[1].data&0xff);
    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;

    *us = (lines*Preview_line_period)/1000;

    SENSOR_DMSK("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    sp2309_params *params = (sp2309_params *)handle->private_data;

    lines = (u32)((1000*us)/Preview_line_period);

    if (lines < 2) lines = 2;
    if (lines >params->expo.vts-1)
        vts = lines +1;
    else
        vts = params->expo.vts;

    if((last_us != us) || (last_line != lines) || (last_vts != params->expo.vts))
    {
        SENSOR_DMSK("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );
    }

    // lines <<= 4;
    params->tExpo_reg[0].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[1].data =(u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[1].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[2].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int again = 0;
    sp2309_params *params = (sp2309_params *)handle->private_data;

    again=params->tGain_reg[0].data;

    *gain = again<<6;
    SENSOR_DMSK("[%s] Gain: %d \n", __FUNCTION__, (again<<6));
    return SUCCESS;
}
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity = NULL;
    u32 i = 0, input_gain = 0;
    u16 gain8 = 0;
    u64 gain_double = 0, total_gain_double = 0;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    input_gain = gain;

    if(gain < SENSOR_MIN_GAIN)
        gain = SENSOR_MIN_GAIN;
    else if(gain >= SENSOR_MAX_GAIN)
        gain = SENSOR_MAX_GAIN;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++)
    {
        //LOGD("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain > Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset)))
        {
            gain=Sensor_Gain_Linearity[i].gain;
            break;
        }
    }

    gain_double = (u64)gain;
    total_gain_double = (gain_double*10000)/1024;

    for(i=1; i < ARRAY_SIZE(gain_table); i++)
    {
        if(gain_table[i].total_gain > total_gain_double)
        {
            gain8 = (gain_table[i].total_gain-total_gain_double > total_gain_double-gain_table[i-1].total_gain) ? gain_table[i-1].reg_val:gain_table[i].reg_val;
            break;
        }
        else if(i == ARRAY_SIZE(gain_table)-1)
        {
            gain8 = gain_table[i].reg_val;
            break;
        }
    }

    params->tGain_reg[0].data = gain8;

    params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    sp2309_params *params = (sp2309_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u8 Dgain = 1;
    u16 gain8 = 0;
    u32 i = 0;
    u64 gain_double = 0, total_gain_double = 0;

    gain = (gain * handle->sat_mingain + 512)>>10; // need to add min sat gain

    if(gain < SENSOR_MIN_GAIN)
        gain = SENSOR_MIN_GAIN;
    else if(gain >= SENSOR_MAX_GAIN)
        gain = SENSOR_MAX_GAIN;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++){
        //LOGD("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);

        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain = Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double = (u64)gain;
    total_gain_double = (gain_double*10000)/1024;

    if(total_gain_double > 155000)
    {
        if(total_gain_double / 2 > 155000)
            Dgain = 3;
        else
            Dgain = 2;
        total_gain_double /= Dgain;
    }

    for(i=1; i<ARRAY_SIZE(gain_table); i++)
    {
        if(gain_table[i].total_gain > total_gain_double)
        {
            gain8 = (gain_table[i].total_gain - total_gain_double > total_gain_double - gain_table[i-1].total_gain) ? gain_table[i-1].reg_val : gain_table[i].reg_val;
            break;
        }
        else if(i == ARRAY_SIZE(gain_table)-1)
        {
            gain8 = gain_table[i].reg_val;
            break;
        }
    }

    params->tGain_reg[0].data = gain8 & 0xFF;
    switch(Dgain)
    {
        case 1:
            params->tGain_reg[2].data = 0x08;
            break;
        case 2:
            params->tGain_reg[2].data = 0x10;
            break;
        case 3:
            params->tGain_reg[2].data = 0x18;
            break;
    }

    params->dirty = true;
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    *max = 1000000/sp2309_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int SP2309_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/sp2309_mipi_linear[0].senout.min_fps;
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
    sp2309_params *params;
    int res;

    if (!handle) {
        SENSOR_DMSK("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSK("[%s]", __FUNCTION__);
    ////////////////////////////////////
    // private data allocation & init //
    ////////////////////////////////////
    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }

    params = (sp2309_params *)handle->private_data;
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"SP2309_MIPI");

    ////////////////////////////////////
    //    i2c config                  //
    ////////////////////////////////////
    handle->i2c_cfg.mode          = SENSOR_I2C_LEGACY;    //(CUS_ISP_I2C_MODE) FALSE;
    handle->i2c_cfg.fmt           = SENSOR_I2C_FMT;       //CUS_I2C_FMT_A16D8;
    handle->i2c_cfg.address       = SENSOR_I2C_ADDR;      //0x78;
    handle->i2c_cfg.speed         = SENSOR_I2C_SPEED;     //300000;

    ////////////////////////////////////
    //    mclk                        //
    ////////////////////////////////////
    handle->mclk                    = Preview_MCLK_SPEED;//UseParaMclk(SENSOR_DRV_PARAM_MCLK());

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
        handle->video_res_supported.res[res].width         = sp2309_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = sp2309_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = sp2309_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = sp2309_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = sp2309_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = sp2309_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = sp2309_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = sp2309_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, sp2309_mipi_linear[res].senstr.strResDesc);
    }

    ////////////////////////////////////
    //    Sensor polarity             //
    ////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_NEG;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_POS;
    handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_POS;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error

    ////////////////////////////////////////
    // Sensor Status Control and Get Info //
    ////////////////////////////////////////
    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init_mipi2lane_2m60fps_linear;
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
    handle->pCus_sensor_SetPatternMode  = sp2309_SetPatternMode;

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
    handle->pCus_sensor_GetShutterInfo = SP2309_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->dirty           = false;

    return SUCCESS;
}





SENSOR_DRV_ENTRY_IMPL_END_EX(SP2309,
                            cus_camsensor_init_handle_linear,
                            NULL,
                            NULL,
                            sp2309_params
                         );
