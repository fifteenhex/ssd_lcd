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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC2335_MIPI);

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
#undef SENSOR_NAME
#define SENSOR_NAME                  SC2335

//#define SENSOR_ID_r3107              0x3107
//#define SENSOR_ID_r3108              0x3108
//#define SENSOR_ID                    0x2238
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
}sc2335_mipi_linear[] = {
    { LINEAR_RES_1, {1936, 1096, 3, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps"}},
};
#define Preview_line_period                         29630   //33.3ms/1125 =29600ns,
#define vts_30fps                                   1125    //for MCLK=37P125MHz

////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (155 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)
#define ENABLE_NR                                   (1)

////////////////////////////////////
// Mirror-Flip Info               //
////////////////////////////////////
#define SC2335_BLC                                  0x3928   // if mirror on set 0x5 else set 0x1
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
        u32 line;
    } expo;

    u32 idx_tmp;
    u32 set_gain;
    bool dirty;
    bool nr_dirty;
    bool orien_dirty;
    bool temperature_reg_1_dirty;
    bool temperature_reg_2_dirty;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tNr_reg[1];
    I2C_ARRAY tTemperature_reg_1[5];
    I2C_ARRAY tTemperature_reg_2[3];
} sc2335_params;

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
const static I2C_ARRAY Sensor_init_table_2M30_1lane_linear[] =
{
    {0x0103,0x01},
    {0xffff, 50},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x3f00,0x0d},
    {0x3f04,0x02},
    {0x3f05,0x0e},
    {0x3316,0x00},
    {0x3338,0x80},
    {0x337e,0x00},
    {0x3271,0x00},
    {0x3273,0x03},
    {0x3249,0x0f},
    {0x330f,0x03},
    {0x333a,0x02},
    {0x330d,0x18},
    {0x3301,0x22},
    {0x3302,0x09},
    {0x3304,0x20},
    {0x331e,0x19},
    {0x330b,0xb0},
    {0x330c,0x08},
    {0x332b,0x08},
    {0x3366,0x62},
    {0x33a2,0x07},
    {0x337c,0x05},
    {0x337d,0x09},
    {0x335f,0x04},
    {0x3207,0x3f},
    {0x4505,0x0a},
    {0x3f09,0x48},
    {0x3e01,0x20},
    {0x3e02,0x00},
    {0x3637,0x20},
    {0x330B,0x88},
    {0x391f,0x18},
    {0x3637,0x20},
    {0x3614,0x00},
    {0x3908,0x82},
    {0x3e01,0x8c},
    {0x3e02,0x20},
    {0x3333,0x10},
    {0x3306,0x2e},
    {0x330b,0x84},
    {0x3304,0x28},
    {0x331e,0x21},
    {0x33ac,0x04},
    {0x33ae,0x14},
    {0x330e,0x14},
    {0x334c,0x04},
    {0x3250,0x3f},
    {0x3310,0x06},
    {0x330f,0x05},
    {0x333a,0x04},
    {0x3630,0x68},
    {0x481d,0x0a},
    {0x4827,0x03},
    {0x3364,0x17},
    {0x3390,0x08},
    {0x3391,0x18},
    {0x3392,0x38},
    {0x3301,0x06},
    {0x3393,0x09},
    {0x3394,0x28},
    {0x3395,0x28},
    {0x3670,0x08},
    {0x369c,0x08},
    {0x369d,0x38},
    {0x3690,0x32},
    {0x3691,0x32},
    {0x3692,0x42},
    {0x3670,0x0a},
    {0x367c,0x18},
    {0x367d,0x38},
    {0x3674,0xa1},
    {0x3675,0x9c},
    {0x3676,0x9e},
    {0x301f,0x02},
    {0x363c,0x0d},
    {0x3306,0x2e},
    {0x3631,0x84},
    {0x3622,0x16},
    {0x363c,0x04},
    {0x36e9,0x20},
    {0x36f9,0x27},
    {0x5787,0x10},
    {0x5788,0x06},
    {0x5789,0x00},
    {0x578a,0x10},
    {0x578b,0x06},
    {0x578c,0x00},
    {0x5790,0x10},
    {0x5791,0x10},
    {0x5792,0x00},
    {0x5793,0x10},
    {0x5794,0x10},
    {0x5795,0x00},
    {0x5799,0x00},
    {0x57c7,0x10},
    {0x57c8,0x06},
    {0x57c9,0x00},
    {0x57ca,0x10},
    {0x57cb,0x06},
    {0x57cc,0x00},
    {0x57d0,0x10},
    {0x57d1,0x10},
    {0x57d2,0x00},
    {0x57d3,0x10},
    {0x57d4,0x10},
    {0x57d5,0x00},
    {0x57d9,0x00},
    {0x3905,0x98},//4 channel blc
    {0x0100,0x01},
    {0xffff, 100},
};

const static I2C_ARRAY Sensor_id_table[] =
{
    //{0x3107, 0x22},      // {address of 0x3107, ID },
    //{0x3108, 0x38},      // {address of 0x3108, ID },
    //{0x3109, 0x20},      // {address of 0x3109, ID },
};

static I2C_ARRAY PatternTbl[] = {
    {0x4501,0xc8}, //testpattern , bit 3 to enable
};

const static I2C_ARRAY expo_reg[] = { // SHS0
    {0x3e00, 0x00}, //expo [20:17]
    {0x3e01, 0x8a},
    {0x3e02, 0x60}, // bit[7:4]
};

const static I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65}, //bit0-3-->MSB
};

const static I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x80},
    {0x3e08, (0x00|0x03)},
    {0x3e09, 0x40},
};

const I2C_ARRAY nr_reg[] = {
    {0x363c, 0x0e},
};

const I2C_ARRAY temperature_reg_1[] = {
    {0x5787, 0x00},
    {0x5788, 0x00},
    {0x5790, 0x00},
    {0x5791, 0x00},
    {0x5799, 0x07},
};

const I2C_ARRAY temperature_reg_2[] = {
    {0x3637, 0x20},
    {0x391f, 0x18},
    {0x3908, 0x82},
};

#if 0
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
    sc2335_params *params = (sc2335_params *)handle->private_data;

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

    //sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);  // Powerdn Pull High
    //SENSOR_UDELAY(1);
    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_UDELAY(100);

    sensor_if->Reset(idx, !handle->reset_POLARITY);    // Rst Pull High
    SENSOR_UDELAY(100);
    //sensor_if->Reset(idx, handle->reset_POLARITY);     // Rst Pull Low
    //SENSOR_UDELAY(100);
    sensor_if->PowerOff(idx, !handle->reset_POLARITY);

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
    //u16 sen_id_msb, sen_id_lsb, sen_data;

    /* Read Product ID */
    //if (SensorReg_Read(SENSOR_ID_r3107, &sen_id_msb)) {
    //    return FAIL;
    //}
    //if (SensorReg_Read(SENSOR_ID_r3108, &sen_id_lsb)) {
    //    return FAIL;
    //}

    //sen_data = ((sen_id_msb & 0xFF) << 8) | (sen_id_lsb & 0xFF);

    //if (sen_data != SENSOR_ID) {
    //    printk("[**ERROR**] Product ID Fail: 0x%x, Please Check Sensor Insert Correct!!\n", sen_data);
    //    return FAIL;
    //}
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

    SENSOR_DMSG("[%s]SC2335 sensor ,Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);
    return SUCCESS;
}

static int sc2335_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    int i;
    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    switch(mode) {
    case 1:
      PatternTbl[0].data = 0xc8; //enable
      break;
    case 0:
      PatternTbl[0].data = 0xc0; //disable
      break;
    default:
      PatternTbl[0].data = 0xc0; //disable
      break;
    }
    for(i=0; i< ARRAY_SIZE(PatternTbl); i++) {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            return FAIL;
    }

    return SUCCESS;
}

static int pCus_init_mipi_2m30fps_1lane_linear(ms_cus_sensor *handle)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    u16 reg_0x3040;
    u32 i=0, cnt=0;


    if (pCus_CheckSensorProductID(handle)) {
        printk("Please check Sensor SC2335 product!!!!!!!\n");
        return FAIL;
    }
    //logic
     SensorReg_Read(0x3040, &reg_0x3040);

    //SENSOR_EMSG("SC2335 Sensor_init_table_2M30_1lane_linear\n");
    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2M30_1lane_linear);i++)
    {
        if(Sensor_init_table_2M30_1lane_linear[i].reg==0x0100 && 0x01 == Sensor_init_table_2M30_1lane_linear[i].data)
        {
            sensor_if->SetCSI_Clk(0, CUS_CSI_CLK_216M);
            sensor_if->SetCSI_Lane(0, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
            sensor_if->SetCSI_LongPacketType(0, 0, 0x1C00, 0);
        }

        if(Sensor_init_table_2M30_1lane_linear[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table_2M30_1lane_linear[i].data);
        }
        else
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2M30_1lane_linear[i].reg, Sensor_init_table_2M30_1lane_linear[i].data) != SUCCESS)
            {
                cnt++;
                if(cnt>=10)
                {
                    SENSOR_EMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_UDELAY(10);
            }
            //printk("\n reg 0x%x, 0x%x",Sensor_init_table_2M30_1lane_linear[i].reg, Sensor_init_table_2M30_1lane_linear[i].data);
#if 0
            SensorReg_Read(Sensor_init_table_2M30_1lane_linear[i].reg, &sen_data );
            if(Sensor_init_table_2M30_1lane_linear[i].data != sen_data)
                printk("R/W Differ Reg: 0x%x\n",Sensor_init_table_2M30_1lane_linear[i].reg);
                //printk("SC2335 reg: 0x%x, data: 0x%x, read: 0x%x.\n",Sensor_init_table_2M30_1lane_linear[i].reg, Sensor_init_table_2M30_1lane_linear[i].data, sen_data);
#endif
        }
    }

    if(reg_0x3040 == 0x41) {
        SensorReg_Write(0x363c, 0x07);
    }
    printk("Sensor SC2335 Linear MODE Initial Finished\n");

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
    sc2335_params *params = (sc2335_params *)handle->private_data;
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    handle->video_res_supported.ulcur_res = res_idx;

    switch (res_idx) {
        case 0:
            handle->pCus_sensor_init = pCus_init_mipi_2m30fps_1lane_linear;
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
    sc2335_params *params = (sc2335_params *)handle->private_data;

    handle->orient = orit;
    params->orien_dirty = true;

    return SUCCESS;
}

static int DoOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    u16 sen_data = 0, sen_blc = 1;
    //Read SENSOR MIRROR-FLIP STATUS
    SensorReg_Read(MIRROR_FLIP, &sen_data);
    sen_data &= ~(SENSOR_MIRROR_FLIP_EN);

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            //sen_data |= SENSOR_NOR;
            handle->orient = CUS_ORIT_M0F0;
            sen_blc = 0x1;
            break;
        case CUS_ORIT_M1F0:
            sen_data |= SENSOR_MIRROR_EN;
            handle->orient = CUS_ORIT_M1F0;
            sen_blc = 0x5;
            break;
        case CUS_ORIT_M0F1:
            sen_data |= SENSOR_FLIP_EN;
            handle->orient = CUS_ORIT_M0F1;
            sen_blc = 0x1;
            break;
        case CUS_ORIT_M1F1:
            sen_data |= SENSOR_MIRROR_FLIP_EN;
            handle->orient = CUS_ORIT_M1F1;
            sen_blc = 0x5;
            break;
        default :
            handle->orient = CUS_ORIT_M0F0;
            break;
    }
    //Write SENSOR MIRROR-FLIP STATUS
    SensorReg_Write(MIRROR_FLIP, sen_data);
    SensorReg_Write(SC2335_BLC, sen_blc);

    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= min_fps*1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    s32 vts=0;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;
    sc2335_params *params = (sc2335_params *)handle->private_data;

    if(fps>= min_fps && fps <= max_fps) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
    }else if(min_fps*1000>= 5000 && max_fps*1000 <= 30000) {
        params->expo.fps = fps;
        params->expo.vts =  (vts_30fps*30000)/fps;
    }else{
        printk("[%s] FPS %d out of range.\n",__FUNCTION__, fps);
        return FAIL;
    }

    //update vts based on exposure line
    if(params->expo.expo_lines > 2* (params->expo.vts) -10){
        vts = (params->expo.expo_lines + 11)/2;
    }else{
        vts = params->expo.vts;
    }

    params->expo.fps = fps;
    params->expo.vts = vts;
    params->tVts_reg[0].data = (vts & 0xf00) >> 8 ;
    params->tVts_reg[1].data = (vts & 0x0ff) >> 0 ;
    params->dirty = true;
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
    sc2335_params *params = (sc2335_params *)handle->private_data;
    //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;

    switch(status)
    {
        case CUS_FRAME_INACTIVE:
             break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty || params->orien_dirty) {

                if(params->dirty) {
                    SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                    SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                    SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                    params->dirty = false;
                }
#if ENABLE_NR
                SensorReg_Write(0x3812,0x00);
                //if(params->nr_dirty){
                //    SensorRegArrayW((I2C_ARRAY*)nr_reg, ARRAY_SIZE(nr_reg));
                //}
                SensorRegArrayW((I2C_ARRAY*)nr_reg, ARRAY_SIZE(nr_reg)); //because GPH can't be operated with nothing modify;
                if(params->temperature_reg_1_dirty) {
                    SensorRegArrayW((I2C_ARRAY*)temperature_reg_1, ARRAY_SIZE(temperature_reg_1));
                }
                if(params->temperature_reg_2_dirty) {
                    SensorRegArrayW((I2C_ARRAY*)temperature_reg_2, ARRAY_SIZE(temperature_reg_2));
                }
                params->temperature_reg_1_dirty = false;
                params->temperature_reg_2_dirty = false;

                SensorReg_Write(0x3812,0x30);
#endif


                if(params->orien_dirty) {
                    DoOrien(handle, handle->orient);
                    params->orien_dirty = false;
                }
            }
            break;
        default :
             break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;

    *us = (params->expo.expo_lines*Preview_line_period)/1000;
    //SENSOR_DMSG("[%s] sensor expo lines/us %u,%u us\n", __FUNCTION__, params->expo.expo_lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 expo_line = 0, vts = 0;
    sc2335_params *params = (sc2335_params *)handle->private_data;

    expo_line = (1000*us*2)/Preview_line_period; // Preview_line_period in ns

    // Limited expo_line upper-value
    if (expo_line < 3)
        expo_line = 3;

    if (expo_line > ((params->expo.vts*2) - 4))
        vts = (expo_line + 5) /2;
    else
        vts=params->expo.vts;

    params->expo.expo_lines = expo_line;

    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__,
                us,
                expo_line,
                vts
                );

    expo_line = expo_line << 4;
    params->tExpo_reg[0].data = ((expo_line >> 16) & 0x0f);
    params->tExpo_reg[1].data = ((expo_line >>  8) & 0xff);
    params->tExpo_reg[2].data = ((expo_line >>  0) & 0xf0);

    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;

    *gain = params->set_gain;

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u8 i=0 ,Dgain = 1,  Ana_gain = 1;
    u64 Fine_againx64 = 64,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Ana_gain_reg = 0, Fine_again_reg= 0x40,Fine_dgain_reg= 0x80;
#if ENABLE_NR
    u16 reg_0x3040;
    u16 reg_0x3974, reg_0x3975, reg_0x397475;
    static u16 BLC_value=0, BLC_ratio=1;

    I2C_ARRAY temperature_reg_1_temp[] ={
        {0x5787, 0x00},
        {0x5788, 0x00},
        {0x5790, 0x00},
        {0x5791, 0x00},
        {0x5799, 0x07},
    };

    I2C_ARRAY temperature_reg_2_temp[] ={
        {0x3637, 0x20},
        {0x391f, 0x18},
        {0x3908, 0x82},
    };
#endif

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < sizeof(gain_gap_compensate)/sizeof(CUS_GAIN_GAP_ARRAY); i++) {
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
    }

    if (gain < 2 * 1024) {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 1;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x03;
    }
    else if (gain <  4 * 1024) {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 2;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x07;
    }
    else if (gain < 8 * 1024) {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 4;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x0f;
    }
    else if (gain <=  16256) {
        Dgain = 1;      Fine_dgainx128 = 128;         Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  32512) {
        Dgain = 1;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain <  65024) {
        Dgain = 2;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x01;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain < 127 * 1024) {
        Dgain = 4;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x03;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain < 254 * 1024) {
        Dgain = 8;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x07;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }
    else if (gain <= SENSOR_MAX_GAIN * 1024) {
        Dgain = 16;      Fine_againx64 = 127;    Ana_gain = 8;
        Dgain_reg = 0x0f;  Fine_again_reg = 0x7f;  Ana_gain_reg = 0x1f;
    }

    if (gain <= 16256) {
        Fine_againx64 = abs(8 * gain/ (Dgain * Ana_gain * Fine_dgainx128));
        //Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_again_reg = Fine_againx64;
        //Fine_dgain_reg = Fine_dgainx128;
    }
    else {
        Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_dgain_reg = Fine_dgainx128;
    }

    SENSOR_DMSG("[%s] gain %d ; again 0x%x, f_again 0x%x, dgain 0x%x, f_daing %d\n", __FUNCTION__,
                                                                                     gain, \
                                                                                     Ana_gain_reg, \
                                                                                     Fine_again_reg, \
                                                                                     Dgain_reg, \
                                                                                     Fine_dgain_reg);
    params->tGain_reg[0].data = Dgain_reg & 0xF;
    params->tGain_reg[1].data = Fine_dgain_reg;
    params->tGain_reg[2].data = Ana_gain_reg; //((Ana_gain_reg & 0x7) << 2) + 0x3; // Default bit[0: 1] 0x3;
    params->tGain_reg[3].data = Fine_again_reg;

    params->dirty = true;

#if ENABLE_NR
    SensorReg_Read(0x3040, &reg_0x3040);

    if(reg_0x3040 != 0x41) {
        if (Fine_dgain_reg < 0x07) {
            params->tNr_reg[0].data = 0x0e;
        }
        else if (Fine_dgain_reg <= 0x1f) {
            params->tNr_reg[0].data = 0x07;
        }
    }
    else {
        params->tNr_reg[0].data = 0x07;
    }
    /* Each frame refresh to avoid the red flash when linear and wide dynamic switching */
    //for (i = 0; i < sizeof(nr_reg)/sizeof(I2C_ARRAY); i++) {
    //    if (nr_reg[i].data != nr_reg[i].data) {
    //        params->nr_dirty = true;
    //        break;
    //    }
    //}

    // highTemp dpc
    SensorReg_Read(0x3974, &reg_0x3974);
    SensorReg_Read(0x3975, &reg_0x3975);
    reg_0x397475 = ((reg_0x3974 << 8) & 0xFF00) | (reg_0x3975 & 0xFF);
    BLC_value = ((reg_0x397475 >> 3) - 0x400)*BLC_ratio;

    // high temp 1
    if (reg_0x397475 > 0x2060) {
        params->tTemperature_reg_1[0].data = 0x00;
        params->tTemperature_reg_1[1].data = 0x00;
        params->tTemperature_reg_1[2].data = 0x00;
        params->tTemperature_reg_1[3].data = 0x00;
        params->tTemperature_reg_1[4].data = 0x07;
    }
    else if (reg_0x397475 < 0x2040) {
        params->tTemperature_reg_1[0].data = 0x10;
        params->tTemperature_reg_1[1].data = 0x06;
        params->tTemperature_reg_1[2].data = 0x10;
        params->tTemperature_reg_1[3].data = 0x10;
        params->tTemperature_reg_1[4].data = 0x00;
    }
    // high temp 2
    if ((BLC_value > 0xD0) && (params->expo.vts > (vts_30fps * 30 / 18))) {  //vts > vts_30fps * 30 / 18 means frame rate less than 18fps
        params->tTemperature_reg_2[0].data = 0x40;
        params->tTemperature_reg_2[1].data = 0x10;
        params->tTemperature_reg_2[2].data = 0x41;
        BLC_ratio = 2;
    }
    else if ((BLC_value < 0xB0) || (params->expo.vts <= (vts_30fps * 30 / 18))) {  //vts <= vts_30fps * 30 / 18 means frame rate no less than 18fps
        params->tTemperature_reg_2[0].data = 0x20;
        params->tTemperature_reg_2[1].data = 0x18;
        params->tTemperature_reg_2[2].data = 0x82;
        BLC_ratio = 1;
    }

    for (i = 0; i < sizeof(temperature_reg_1)/sizeof(I2C_ARRAY); i++) {
        if (temperature_reg_1[i].data != temperature_reg_1_temp[i].data) {
            params->temperature_reg_1_dirty = true;
            break;
        }
    }

    for (i = 0; i < sizeof(temperature_reg_2)/sizeof(I2C_ARRAY); i++) {
        if (temperature_reg_2[i].data != temperature_reg_2_temp[i].data) {
            params->temperature_reg_2_dirty = true;
            break;
        }
    }
#endif

    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = 1;
    //*max = 1000000/Preview_MIN_FPS;
    *max = 1000000/sc2335_mipi_linear[0].senout.min_fps;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = SENSOR_MIN_GAIN;//handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int SC2335_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    //info->max = 1000000000/Preview_MIN_FPS;//
    info->max = 1000000000/sc2335_mipi_linear[0].senout.min_fps;
    info->min = (Preview_line_period * 1);
    info->step = Preview_line_period;

    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc2335_params *params;
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

    params = (sc2335_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
    memcpy(params->tTemperature_reg_1, temperature_reg_1, sizeof(temperature_reg_1));
    memcpy(params->tTemperature_reg_2, temperature_reg_2, sizeof(temperature_reg_2));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    sprintf(handle->model_id,"SC2335_MIPI");

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
        handle->video_res_supported.res[res].width         = sc2335_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = sc2335_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = sc2335_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = sc2335_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = sc2335_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = sc2335_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = sc2335_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = sc2335_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, sc2335_mipi_linear[res].senstr.strResDesc);
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
    handle->pCus_sensor_init           = pCus_init_mipi_2m30fps_1lane_linear;
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
    handle->pCus_sensor_SetPatternMode = sc2335_SetPatternMode; //NONE

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
    handle->pCus_sensor_GetShutterInfo  = SC2335_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->expo.expo_lines = 1000;
    params->expo.fps        = 30;
    params->orien_dirty     = 0;
    params->dirty           = 0;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(SC2335_MIPI,
                            cus_camsensor_init_handle_linear,
                            NULL,
                            NULL,
                            sc2335_params
                         );
