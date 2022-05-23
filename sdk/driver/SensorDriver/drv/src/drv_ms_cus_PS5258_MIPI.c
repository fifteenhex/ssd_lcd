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
#include <drv_sensor_init_table.h> //TODO: move this header to drv_sensor_common.h
//#include <PS5258_MIPI_init_table.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN(PS5258);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

//#define SENSOR_PAD_GROUP_SET CUS_SENSOR_PAD_GROUP_A
//#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE            CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
//#define SENSOR_CHANNEL_MODE_SONY_DOL   CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR

#undef SENSOR_DBG
#define SENSOR_DBG 0

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
#define SENSOR_MIPI_LANE_NUM        (2)
//#define SENSOR_MIPI_LANE_NUM_DOL    (4)
//#define SENSOR_MIPI_HDR_MODE        (0) //0: Non-HDR mode. 1:Sony DOL mode

#define SENSOR_ISP_TYPE             ISP_EXT             //ISP_EXT, ISP_SOC (Non-used)
//#define SENSOR_DATAFMT             CUS_DATAFMT_BAYER    //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE           CUS_SENIF_BUS_MIPI  //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE      PACKET_HEADER_EDGE1
//#define SENSOR_MIPI_HSYNC_MODE_HDR_DOL PACKET_FOOTER_EDGE
#define SENSOR_DATAPREC             CUS_DATAPRECISION_10
//#define SENSOR_DATAPREC_DOL         CUS_DATAPRECISION_10
#define SENSOR_DATAMODE             CUS_SEN_10TO12_9000 //CFG
#define SENSOR_BAYERID              CUS_BAYER_BG        //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
//#define SENSOR_BAYERID_HDR_DOL      CUS_BAYER_GB
#define SENSOR_RGBIRID              CUS_RGBIR_NONE
#define SENSOR_ORIT                 CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER              CUS_SEN_YCODR_YC     //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
//#define long_packet_type_enable     0x00 //UD1~UD8 (user define)

////////////////////////////////////
// MCLK Info                      //
////////////////////////////////////
#define Preview_MCLK_SPEED          CUS_CMU_CLK_24MHZ    //CUS_CMU_CLK_24MHZ //CUS_CMU_CLK_37P125MHZ
//#define Preview_MCLK_SPEED_HDR_DOL  CUS_CMU_CLK_37P125MHZ

////////////////////////////////////
// I2C Info                       //
////////////////////////////////////
#define SENSOR_I2C_ADDR              0x90                //I2C slave address
#define SENSOR_I2C_SPEED             300000              //200000 //300000 //240000                  //I2C speed, 60000~320000
//#define SENSOR_I2C_CHANNEL           1                 //I2C Channel
//#define SENSOR_I2C_PAD_MODE          2                 //Pad/Mode Number
#define SENSOR_I2C_LEGACY            I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT               I2C_FMT_A16D8       //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

////////////////////////////////////
// Sensor Signal                  //
////////////////////////////////////
#define SENSOR_PWDN_POL              CUS_CLK_POL_POS     // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL               CUS_CLK_POL_NEG     // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG
                                                         // VSYNC/HSYNC POL can be found in data sheet timing diagram,
                                                         // Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.
#define SENSOR_VSYNC_POL             CUS_CLK_POL_NEG     // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL             CUS_CLK_POL_NEG     // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL              CUS_CLK_POL_POS     // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

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
}PS5258_mipi_linear[] = {
    {LINEAR_RES_1, {2200, 1124, 5, 30}, {0, 0, 1920, 1080}, {"1920x1080@30fps"}}, // Modify it
};

#define Preview_line_period           29629 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define vts_30fps                     1124
////////////////////////////////////
// AE Info                        //
////////////////////////////////////
#define SENSOR_MAX_GAIN                             (32 * 1024)        // max sensor again, a-gain * conversion-gain*d-gain
#define SENSOR_MIN_GAIN                             (1 * 1024)
#define SENSOR_GAIN_DELAY_FRAME_COUNT               (2)
#define SENSOR_SHUTTER_DELAY_FRAME_COUNT            (2)
//#define SENSOR_SHUTTER_DELAY_FRAME_COUNT_HDR_DOL    (2)

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);

#if 0
CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};
#endif

typedef struct {
    struct {
        bool bVideoMode;
        u16 res_idx;
        //bool binning;
        //bool scaling;
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
        u32 cur_fps;
        u32 target_fps;
        u32 line;
        u16 sens;
        u32 fps;
        u32 preview_fps;
    } expo;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[1];
    I2C_ARRAY tExpo_reg[2];
    I2C_ARRAY tFAE_data[5];
    bool dirty;
    bool orient_dirty;
    u32 cur_shutter;
} PS5258_params;

typedef struct {
    u32 total_gain;
    u16 reg_val;
} Gain_ARRAY;

typedef struct {
    u32 reg;
    u16 val;
} FAE_ARRAY;


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
// set sensor ID address and data,
const static I2C_ARRAY Sensor_id_table[] =
{
    {0x0100, 0x52}, // {high byte address of ID, ID_H},
    {0x0101, 0x58}, // {low byte address of ID, ID_L},
};

#if 1
const static I2C_ARRAY Sensor_init_table[] =
{
#if 0
    {0x0110, 0x00}, // Cmd_HLGain_Mode=0
    {0x010B, 0x07}, // Cmd_Sw_TriState[0]=1
    {0x0114, 0x08}, // Cmd_LineTime=2200
    {0x0115, 0x98}, // Cmd_LineTime=2200
    {0x0162, 0x02}, // Cmd_Np[3:0]=2
    {0x0178, 0xA0}, // A02A: Version
    {0x0179, 0x2A}, // A02A: Version
    {0x022D, 0x01}, // T_spll_enh[0]=1
    {0x0225, 0xEA}, // T_pos_pump2_lvl[3:0]=9->14
    {0x0226, 0xB1}, // rsel: vdday -> vddma
    {0x0227, 0x39}, // T_ldotg_lvl[3:0]=7->3
    {0x021C, 0x00}, // T_FAE_CLK_SEL[0]=0
    {0x0233, 0x70}, // T_iref_enl=1, T_vdda_enl=1
    {0x023C, 0x57}, // T_pxoi_fast[2:0]=7
    {0x024B, 0x05}, // T_mipi_bitclk_phase[0]=1
    {0x024D, 0x11}, // T_MIPI_EnL[0]=0, T_MIPI_sel[0]=1
    {0x0252, 0x19}, // T_pll_predivider[5:0]=25
    {0x0253, 0x26}, // T_pll_postdivider[5:0]=38
    {0x0254, 0x61}, // T_pll_enh[0]=1, T_icp_p[2:0]=1->6
    {0x0255, 0x11}, // 11//T_pll_div2_EnH=1
    {0x0624, 0x05}, // R_tg=1293
    {0x0625, 0x0D}, // R_tg=1293
    {0x0B01, 0x00}, // Cmd_Adc_sample_posedge=0 for work-around
    {0x0B02, 0x02}, // Cmd_RClkDly_Sel=7
    {0x0B0C, 0x00}, // Cmd_MIPI_Clk_Gated[0]=0
    {0x0C00, 0x02}, // Cmd_DG_EnH=1
    {0x0D00, 0x00}, // R_ImgSyn_SGHD_EnH=0
    {0x1409, 0x1A}, // R_tx_data_settle_prd[7:0]=26
    {0x140A, 0x15}, // R_tx_data_sp_blank_prd[7:0]=21
    {0x140C, 0x13}, // FF//R_phyclk_lps_prd[7:0]=19 for discontinue clock, defualt value is wrong
    {0x1411, 0x00}, // 01//R_phyclk_cont_mode[0]=1
    {0x1415, 0x04}, // R_LPX_prd[7:0]=7->4
    {0x1417, 0x03}, // R_HsPrep_prd[7:0]=6->3
    {0x1418, 0x02}, // R_HsEoT_prd[7:0]=5->2
    {0x1406, 0x04}, // RAW10
    {0x1410, 0x02}, // 2Lane
    {0x140F, 0x01}, // R_CSI2_enable=1
    {0x0111, 0x01}, // UpdateFlag
    {0x010F, 0x01}  // Sensor_EnH[0]=1
#endif
    {0x010B, 0x07},//Cmd_Sw_TriState[0]=1
    {0x0114, 0x09},//Cmd_LineTime[12:0]=2400
    {0x0115, 0x60},//Cmd_LineTime[12:0]=2400
    //expo
    {0x0118, 0x03},
    {0x0119, 0xF4},
    
    {0x0178, 0xA0},//A01A: Version
    {0x0179, 0x1A},//A01A: Version
    {0x022E, 0x0E},//T_spll_predivider[5:0]=14
    {0x022F, 0x19},//T_spll_postdivider[5:0]=25
    {0x022D, 0x01},//T_spll_enh[0]=1
    {0x021C, 0x00},//T_FAE_CLK_SEL[0]=0
    {0x0252, 0x16},//T_pll_predivider[5:0]=22
    {0x0254, 0x61},//T_pll_enh[0]=1
    {0x0B0C, 0x00},//Cmd_MIPI_Clk_Gated[0]=0
    {0x140F, 0x01},//R_CSI2_enable=1
    {0x0111, 0x01},//UpdateFlag
    {0x010F, 0x01} //Sensor_EnH=1
};
#endif
I2C_ARRAY TriggerStartTbl[] =
{
    //{0x30f4,0x00},//Master mode start
};

I2C_ARRAY PatternTbl[] =
{
    //pattern mode
};

I2C_ARRAY Current_Mirror_Flip_Tbl[] =
{
    {0x0140, 0x00}, //Hflip M0F0
    {0x0141, 0x00}, //Vflip
    {0x0149, 0x00}, //Cmd_VStart
    {0x014A, 0x02}, //Cmd_VStart
};

const static I2C_ARRAY mirr_flip_table[] =
{
    {0x0140, 0x00}, //Hflip M0F0
    {0x0141, 0x00}, //Vflip
    {0x0149, 0x00}, //Cmd_VStart
    {0x014A, 0x02}, //Cmd_VStart

    {0x0140, 0x01}, //Hflip M1F0
    {0x0141, 0x00}, //Vflip
    {0x0149, 0x00}, //Cmd_VStart
    {0x014A, 0x02}, //Cmd_VStart

    {0x0140, 0x00}, //Hflip M0F1
    {0x0141, 0x01}, //Vflip
    {0x0149, 0x04}, //Cmd_VStart
    {0x014A, 0x44}, //Cmd_VStart

    {0x0140, 0x01}, //Hflip M1F1
    {0x0141, 0x01}, //Vflip
    {0x0149, 0x04}, //Cmd_VStart
    {0x014A, 0x44}, //Cmd_VStart
};

const static I2C_ARRAY gain_reg[] =
{
    {0x012B, 0x00}, //analog gain index
    //{0x012A, 0x00}, //digital gain index
};

// Davis 20181101
const static Gain_ARRAY gain_table[] =
{
    {10000  ,4096},
    {10625  ,3855},
    {11250  ,3641},
    {11875  ,3449},
    {12500  ,3277},
    {13125  ,3121},
    {13750  ,2979},
    {14375  ,2849},
    {15000  ,2731},
    {15625  ,2621},
    {16250  ,2521},
    {16875  ,2427},
    {17500  ,2341},
    {18125  ,2260},
    {18750  ,2185},
    {19375  ,2114},
    {20000  ,2048},
    {21250  ,1928},
    {22500  ,1820},
    {23750  ,1725},
    {25000  ,1638},
    {26250  ,1560},
    {27500  ,1489},
    {28750  ,1425},
    {30000  ,1365},
    {31250  ,1311},
    {32500  ,1260},
    {33750  ,1214},
    {35000  ,1170},
    {36250  ,1130},
    {37500  ,1092},
    {38750  ,1057},
    {40000  ,1024},
    {42500  ,964 },
    {45000  ,910 },
    {47500  ,862 },
    {50000  ,819 },
    {52500  ,780 },
    {55000  ,745 },
    {57500  ,712 },
    {60000  ,683 },
    {62500  ,655 },
    {65000  ,630 },
    {67500  ,607 },
    {70000  ,585 },
    {72500  ,565 },
    {75000  ,546 },
    {77500  ,529 },
    {80000  ,512 },
    {85000  ,482 },
    {90000  ,455 },
    {95000  ,431 },
    {100000 ,410 },
    {105000 ,390 },
    {110000 ,372 },
    {115000 ,356 },
    {120000 ,341 },
    {125000 ,328 },
    {130000 ,315 },
    {135000 ,303 },
    {140000 ,293 },
    {145000 ,282 },
    {150000 ,273 },
    {155000 ,264 },
    {160000 ,256 },
    {169959 ,241 },
    {180441 ,227 },
    {190512 ,215 },
    {199805 ,205 },
    {210051 ,195 },
    {220215 ,186 },
    {230112 ,178 },
    {239532 ,171 },
    {249756 ,164 },
    {259241 ,158 },
    {269474 ,152 },
    {280548 ,146 },
    {290496 ,141 },
    {298948 ,137 },
    {310303 ,132 },
    {320000 ,128 },
};

static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] =  //compensate  gain gap
{
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

const static I2C_ARRAY expo_reg[] =
{
    {0x0118, 0x00},
    {0x0119, 0x02},
};

const static I2C_ARRAY vts_reg[] =
{
    {0x0116, 0x04},
    {0x0117, 0xF2},
};

#if 0
const static FAE_ARRAY fae_reg[] =
{
    {0x0F37, 0x0}, // to Cmd_SGHD
    {0x0F38, 0x0}, // to Cmd_DG_gain_idx
    {0x0F39, 0x0}, // to Cmd_gain_idx
    {0x0F3C, 0x0}, // to Cmd_offNY[15:8]
    {0x0F3D, 0x0}, // to Cmd_offNY[7:0]
};
#endif
/////////// function definition ///////////////////
#undef SENSOR_NAME
#define SENSOR_NAME PS5258
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

static int cus_camsensor_release_handle(ms_cus_sensor *handle);

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

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    u8 i,n;
    u8 table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    SensorReg_Write(0xef,0x00);

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
    SENSOR_DMSG("[%s]PS5258 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    //u8 res = 0;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    /*PAD and CSI*/
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    /*RST ON*/
    //res = sensor_if->Reset(idx, !handle->reset_POLARITY);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    //if(res>=0)//if success
    //    SENSOR_UDELAY(100);    //T0 = 100us

    /*MCLK ON*/
    //res = sensor_if->MCLK(idx, 1, handle->mclk);
    sensor_if->MCLK(idx, 1, handle->mclk);
    //if(res>=0)//if success
    //    SENSOR_MSLEEP(53);     //T3 = 53ms

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    //sensor_if->Reset(idx, handle->reset_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    SENSOR_UDELAY(100);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    return SUCCESS;
}

static int PS5258_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);
    return SUCCESS;
}

static int pCus_init(ms_cus_sensor *handle)
{
    u8 i,cnt=0;
    //u16 data;
    //FAE_ARRAY fae_status[ARRAY_SIZE(fae_reg)];

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

#if 0
    // CHECK Sensor ID
    for(i = 0 ; i < ARRAY_SIZE(Sensor_id_table) ; ++i)
    {
        SensorReg_Read(Sensor_id_table[i].reg, &data);

        if( Sensor_id_table[i].data != data )
        {
            //SENSOR_DMSG("\n\n Read Sensor ID Error, Please Check insert Sensor Correct\n");
            return FAIL;
        }
    }

    for(i = 0 ; i < ARRAY_SIZE(fae_reg) ; ++i)
    {
    	fae_status[i].reg = fae_reg[i].reg;
        SensorReg_Read(fae_status[i].reg, &data);
        fae_status[i].val = data;

        //SENSOR_DMSG("\nREG 0x%X Get 0x%X \n", fae_status[i].reg, fae_status[i].val);
    }
#endif
    for(i=0;(sizeof(Sensor_init_table)&&(i< ARRAY_SIZE(Sensor_init_table)));i++)
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

    for(i=0;(sizeof(PatternTbl)&&(i<ARRAY_SIZE(PatternTbl)));i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            //MSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
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

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
#if 0
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
#endif
    *orit = handle->orient;
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;
    u8 table_length = ARRAY_SIZE(mirr_flip_table);
    u8 seg_length=table_length/4;
    u8 i,j;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    //sensor_if1->SetSkipFrame(handle,2); //skip 2 frame to avoid bad frame after mirror/flip

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = orit;
            for(i=0,j=0;i<seg_length;i++,j++){
                SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //handle->bayer_id=  CUS_BAYER_BG;
            break;

        case CUS_ORIT_M1F0:
            handle->orient = orit;
            for(i=seg_length,j=0;i<seg_length*2;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //handle->bayer_id= CUS_BAYER_BG;
            break;

        case CUS_ORIT_M0F1:
            handle->orient = orit;
            for(i=seg_length*2,j=0;i<seg_length*3;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //handle->bayer_id= CUS_BAYER_GR;
            break;

        case CUS_ORIT_M1F1:
            handle->orient = orit;
            for(i=seg_length*3,j=0;i<seg_length*4;i++,j++){
                //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //handle->bayer_id= CUS_BAYER_GR;
            break;

        default :
            handle->orient = CUS_ORIT_M0F0;
            for(i=0,j=0;i<seg_length;i++,j++){
                ///SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
                Current_Mirror_Flip_Tbl[j].reg = mirr_flip_table[i].reg;
                Current_Mirror_Flip_Tbl[j].data = mirr_flip_table[i].data;
            }
            //handle->bayer_id= CUS_BAYER_BG;
            break;
    }
    //SensorReg_Write(0xef,0x01);
    //SensorReg_Write(0x09,1);

    params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int _SetFps(ms_cus_sensor *handle, u32 fps)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    params->expo.fps = fps;

    if(fps>=5000 && fps <= 30000)
    {
        //params->expo.target_fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        pCus_SetAEUSecs(handle, params->cur_shutter);
        return SUCCESS;
    }else{
      return FAIL;
    }
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps>=5 && fps <= 30)
    {
      params->expo.target_fps = fps*1000;
    }
    else if(fps>=5000 && fps <= 30000)
    {
        params->expo.target_fps = fps;
    }else
    {
      return FAIL;
    }

    if( params->expo.target_fps >= params->expo.cur_fps*2)
    {
        u32 new_fps = (params->expo.cur_fps*3)/2;
        _SetFps( handle, new_fps);
        params->expo.cur_fps = new_fps;
    }
    else
    {
        _SetFps(handle, params->expo.target_fps);
        params->expo.cur_fps = params->expo.target_fps;
    }

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
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;

    switch (status)
    {
        case CUS_FRAME_INACTIVE:
            if (params->orient_dirty) {
                SensorRegArrayW((I2C_ARRAY*)Current_Mirror_Flip_Tbl, ARRAY_SIZE(Current_Mirror_Flip_Tbl));
                SensorReg_Write(0x0111, 1);
                params->orient_dirty = false;
            }
            break;
        case CUS_FRAME_ACTIVE:
            if (params->dirty) {
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, ARRAY_SIZE(vts_reg));
                SensorReg_Write(0x0111, 1);
                params->dirty = false;
            } else {
                if (params->expo.target_fps != params->expo.cur_fps) {
                    pCus_SetFPS(handle, params->expo.target_fps);
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
    PS5258_params *params = (PS5258_params *)handle->private_data;
    u32 lines = 0;
    u16 tExp_L, tExp_H;
    u32 tExp;

    SensorReg_Read(expo_reg[0].reg, &tExp_H);
    SensorReg_Read(expo_reg[1].reg, &tExp_L);
    tExp = tExp_L | (tExp_H << 8);

    lines = params->expo.vts - tExp;

    *us = lines*Preview_line_period/1000;

    SENSOR_DMSG("[%s] sensor expo lines/us %ld,%ld us\n", __FUNCTION__, lines, *us);

    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    PS5258_params *params = (PS5258_params *)handle->private_data;
#if 0
    static u16 cnt = 0;
    u16 tExp_L, tExp_H;
    u32 tExp;

    if(cnt <=5)
    {
        SensorReg_Read(expo_reg[0].reg, &tExp_H);
        SensorReg_Read(expo_reg[1].reg, &tExp_L);

        tExp = tExp_L | (tExp_H << 8);
        SENSOR_DMSG("[%s] %d\n", __func__, tExp);
        cnt++;
        return SUCCESS;
    }
#endif
    params->cur_shutter = us;

    lines=(1000*us)/Preview_line_period;

    if (lines < 1) lines = 1;

    if (lines > params->expo.vts - 2) {
        vts = lines + 2;
    }
    else
        vts=params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,
                us,
                lines,
                params->expo.vts
                );

    lines = vts-lines;
    params->tExpo_reg[0].data =(u16)( (lines>>8) & 0x00ff);
    params->tExpo_reg[1].data =(u16)( (lines>>0) & 0x00ff);

    params->tVts_reg[0].data = (u16)((vts >> 8) & 0x00ff);
    params->tVts_reg[1].data = (u16)((vts >> 0) & 0x00ff);

    params->dirty = true;

    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    PS5258_params *params = (PS5258_params *)handle->private_data;

    *gain = params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain, params->tGain_reg[0].data);

    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
#if 0
    PS5258_params *params = (PS5258_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;

    u16 gain16=1024;

    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN)
        gain=SENSOR_MAX_GAIN;


    gain_double=(double)gain;
    total_gain_double=((gain_double)*10000)/1024;
    for(i=1;i<ARRAY_SIZE(gain_table);i++)
    {
        if(gain_table[i].total_gain>total_gain_double)
        {
            gain16=gain_table[i-1].reg_val;
            break;
        }
        else if(i==ARRAY_SIZE(gain_table)-1)
        {
            gain16=gain_table[i].reg_val;
            break;
        }
    }

    params->tGain_reg[0].data=(gain16>>8)&0x01f;
    params->tGain_reg[1].data=gain16&0xff;//low byte,LSB
    SENSOR_DMSG("[%s] set gain/regH/regL=%d/0x%x/0x%x\n", __FUNCTION__, gain,params->tGain_reg[0].data,params->tGain_reg[1].data);
#endif
   // printf("set gain/reg=%d/0x%x\n", gain,params->tGain_reg[1].data);
  //  params->dirty = true;
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    //extern DBG_ITEM Dbg_Items[DBG_TAG_MAX];
    PS5258_params *params = (PS5258_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    u16 gain_index = 0;
#if 0
    static u8 cnt_g = 0;
    u16 tGain_idx;

    if(cnt_g <= 5)
    {
        SensorReg_Read(gain_reg[0].reg, &tGain_idx);

        //printk("[%s] %d\n", __func__, tGain_idx);
        cnt_g++;
        return SUCCESS;
    }
#endif
    params->expo.final_gain = gain;
    if(gain<handle->sat_mingain)
        gain=handle->sat_mingain;
    else if(gain>=SENSOR_MAX_GAIN)
        gain=SENSOR_MAX_GAIN;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < ARRAY_SIZE(gain_gap_compensate); i++)
    {
        //LOGD("GAP:%x %x\r\n",Sensor_Gain_Linearity[i].gain, Sensor_Gain_Linearity[i].offset);
        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain>Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset))){
              gain=Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double = (u32)gain;
    total_gain_double=((gain_double)*10000)/1024;

    for (i = 1; i < ARRAY_SIZE(gain_table); i++) {
        if(gain_table[i].total_gain > total_gain_double) {
            gain_index = (i - 1);
            break;
        } else if (i == ARRAY_SIZE(gain_table) - 1) {
            gain_index = i;
            break;
        }
    }

    /* Sensor Analog Gain Index */
    params->tGain_reg[0].data = gain_index;

    SENSOR_DMSG("[%s] set gain/reg gain(x10000)/index=%d/%d/%d\n", __FUNCTION__, gain, gain_table[gain_index].total_gain, params->tGain_reg[0].data);
    params->dirty = true;

    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000/30;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min =handle->sat_mingain;
    *max = SENSOR_MAX_GAIN;
    return SUCCESS;
}

static int PS5258_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000 / PS5258_mipi_linear[handle->video_res_supported.ulcur_res].senout.min_fps;
    info->min =  Preview_line_period*3;
    info->step = Preview_line_period;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2){
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    //SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}

static int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    PS5258_params *params;
    u8 res;

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

    params = (PS5258_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    strcpy(handle->model_id,"PS5258_MIPI");

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
    handle->mclk                  = UseParaMclk(SENSOR_DRV_PARAM_MCLK());

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
    handle->video_res_supported.num_res = LINEAR_RES_END;
    for (res = 0; res < LINEAR_RES_END; res++) {
        handle->video_res_supported.res[res].width         = PS5258_mipi_linear[res].senif.preview_w;
        handle->video_res_supported.res[res].height        = PS5258_mipi_linear[res].senif.preview_h;
        handle->video_res_supported.res[res].max_fps       = PS5258_mipi_linear[res].senout.max_fps;
        handle->video_res_supported.res[res].min_fps       = PS5258_mipi_linear[res].senout.min_fps;
        handle->video_res_supported.res[res].crop_start_x  = PS5258_mipi_linear[res].senif.crop_start_X;
        handle->video_res_supported.res[res].crop_start_y  = PS5258_mipi_linear[res].senif.crop_start_y;
        handle->video_res_supported.res[res].nOutputWidth  = PS5258_mipi_linear[res].senout.width;
        handle->video_res_supported.res[res].nOutputHeight = PS5258_mipi_linear[res].senout.height;
        sprintf(handle->video_res_supported.res[res].strResDesc, PS5258_mipi_linear[res].senstr.strResDesc);
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
    handle->pCus_sensor_release        = cus_camsensor_release_handle;
    handle->pCus_sensor_init           = pCus_init;
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
    handle->pCus_sensor_SetPatternMode = PS5258_SetPatternMode;

    ////////////////////////////////////
    //    AE parameters               //
    ////////////////////////////////////
    handle->ae_gain_delay              = SENSOR_GAIN_DELAY_FRAME_COUNT;
    handle->ae_shutter_delay           = SENSOR_SHUTTER_DELAY_FRAME_COUNT;
    handle->ae_gain_ctrl_num           = 1;
    handle->ae_shutter_ctrl_num        = 1;
    handle->sat_mingain                = SENSOR_MIN_GAIN;
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

     //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain_cal;
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo  = PS5258_GetShutterInfo;

    params->expo.vts        = vts_30fps;
    params->expo.cur_fps    = 30000; //fps x 1000
    params->expo.target_fps = 30000;
    params->expo.line       = 100;
    params->expo.sens       = 1;
    params->dirty           = false;
    params->orient_dirty    = false;
    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX( PS5258,
                              cus_camsensor_init_handle_linear,
                              NULL,
                              NULL,
                              PS5258_params
                            );

