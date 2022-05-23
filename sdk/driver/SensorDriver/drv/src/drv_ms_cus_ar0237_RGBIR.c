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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(AR0237_RGBIR);

#define SENSOR_MODEL_ID     "AR0237"

#define SENSOR_ISP_TYPE     ISP_EXT
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_PARL      //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9098

//#define SENSOR_BAYERID      CUS_BAYER_GR            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_BAYERID      CUS_BAYER_GB            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
//#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
//#define SENSOR_BAYERID      CUS_BAYER_RG           //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_G1
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN     691//(16*2.7*16)                  // max sensor again, a-gain * conversion-gain*d-gain

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ   //MCLK from SCL

#define Preview_line_period 29630                      //33.333/1125
//#define vts_30fps           1125                    //74.25M = 2200*1125*30
#define vts_30fps           1126                    //74.25M < 2200*1126*30
#define Preview_WIDTH       1920                    //resolution Width when preview
#define Preview_HEIGHT      1080                    //resolution Height when preview
#define Preview_MAX_FPS     30                      //fastest preview FPS
#define Preview_MIN_FPS     8                       //slowest preview FPS

#define SENSOR_I2C_ADDR     0x20
#define SENSOR_I2C_FMT      I2C_FMT_A16D16
#define SENSOR_I2C_SPEED    200000                  //I2C speed,60000~320000

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG         // AR0237 no care, OE_BAR pull low always enable
#define SENSOR_RST_POL      CUS_CLK_POL_NEG         // AR0237 RESET active LOW

#define SENSOR_VSYNC_POL    CUS_CLK_POL_POS         // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

CUS_CAMSENSOR_CAP sensor_cap = {
    .length = sizeof(CUS_CAMSENSOR_CAP),
    .version = 0x0001,
};

typedef struct {
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
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty; //sensor setting need to update through I2C
    bool orient_dirty;
    I2C_ARRAY tVts_reg[1];
    I2C_ARRAY tGain_reg[6];
    I2C_ARRAY tExpo_reg[1];
} ar0237_params;

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3000, 0x0256},    // ar0237
    //{0x300E, 0x0256},    // ar0237
};

const I2C_ARRAY Sensor_init_table[] =     // PARA12_1928*1088_30fps_27MCLK_74.25PCLK
{
    {0x301A, 0x0001},    // RESET_REGISTER
    {0xffff, 0x000A},
    {0x301A, 0x10D8},    // RESET_REGISTER
    {0xffff, 0x000A},
    #if 0
    {0x3088, 0x8000},    // SEQ_CTRL_PORT
    {0x3086, 0x4558},
    {0x3086, 0x72A6},
    {0x3086, 0x4A31},
    {0x3086, 0x4342},
    {0x3086, 0x8E03},
    {0x3086, 0x2A14},
    {0x3086, 0x4578},
    {0x3086, 0x7B3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA2A},
    {0x3086, 0x043D},
    {0x3086, 0x102A},
    {0x3086, 0x052A},
    {0x3086, 0x1535},
    {0x3086, 0x2A05},
    {0x3086, 0x3D10},
    {0x3086, 0x4558},
    {0x3086, 0x2A04},
    {0x3086, 0x2A14},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2A04},
    {0x3086, 0x622A},
    {0x3086, 0x288E},
    {0x3086, 0x0036},
    {0x3086, 0x2A08},
    {0x3086, 0x3D64},
    {0x3086, 0x7A3D},
    {0x3086, 0x0444},
    {0x3086, 0x2C4B},
    {0x3086, 0xA403},
    {0x3086, 0x430D},
    {0x3086, 0x2D46},
    {0x3086, 0x4316},
    {0x3086, 0x2A90},
    {0x3086, 0x3E06},
    {0x3086, 0x2A98},
    {0x3086, 0x5F16},
    {0x3086, 0x530D},
    {0x3086, 0x1660},
    {0x3086, 0x3E4C},
    {0x3086, 0x2904},
    {0x3086, 0x2984},
    {0x3086, 0x8E03},
    {0x3086, 0x2AFC},
    {0x3086, 0x5C1D},
    {0x3086, 0x5754},
    {0x3086, 0x495F},
    {0x3086, 0x5305},
    {0x3086, 0x5307},
    {0x3086, 0x4D2B},
    {0x3086, 0xF810},
    {0x3086, 0x164C},
    {0x3086, 0x0955},
    {0x3086, 0x562B},
    {0x3086, 0xB82B},
    {0x3086, 0x984E},
    {0x3086, 0x1129},
    {0x3086, 0x9460},
    {0x3086, 0x5C19},
    {0x3086, 0x5C1B},
    {0x3086, 0x4548},
    {0x3086, 0x4508},
    {0x3086, 0x4588},
    {0x3086, 0x29B6},
    {0x3086, 0x8E01},
    {0x3086, 0x2AF8},
    {0x3086, 0x3E02},
    {0x3086, 0x2AFA},
    {0x3086, 0x3F09},
    {0x3086, 0x5C1B},
    {0x3086, 0x29B2},
    {0x3086, 0x3F0C},
    {0x3086, 0x3E03},
    {0x3086, 0x3E15},
    {0x3086, 0x5C13},
    {0x3086, 0x3F11},
    {0x3086, 0x3E0F},
    {0x3086, 0x5F2B},
    {0x3086, 0x902B},
    {0x3086, 0x803E},
    {0x3086, 0x062A},
    {0x3086, 0xF23F},
    {0x3086, 0x103E},
    {0x3086, 0x0160},
    {0x3086, 0x29A2},
    {0x3086, 0x29A3},
    {0x3086, 0x5F4D},
    {0x3086, 0x1C2A},
    {0x3086, 0xFA29},
    {0x3086, 0x8345},
    {0x3086, 0xA83E},
    {0x3086, 0x072A},
    {0x3086, 0xFB3E},
    {0x3086, 0x6745},
    {0x3086, 0x8824},
    {0x3086, 0x3E08},
    {0x3086, 0x2AFA},
    {0x3086, 0x5D29},
    {0x3086, 0x9288},
    {0x3086, 0x102B},
    {0x3086, 0x048B},
    {0x3086, 0x1686},
    {0x3086, 0x8D48},
    {0x3086, 0x4D4E},
    {0x3086, 0x2B80},
    {0x3086, 0x4C0B},
    {0x3086, 0x3F36},
    {0x3086, 0x2AF2},
    {0x3086, 0x3F10},
    {0x3086, 0x3E01},
    {0x3086, 0x6029},
    {0x3086, 0x8229},
    {0x3086, 0x8329},
    {0x3086, 0x435C},
    {0x3086, 0x155F},
    {0x3086, 0x4D1C},
    {0x3086, 0x2AFA},
    {0x3086, 0x4558},
    {0x3086, 0x8E00},
    {0x3086, 0x2A98},
    {0x3086, 0x3F0A},
    {0x3086, 0x4A0A},
    {0x3086, 0x4316},
    {0x3086, 0x0B43},
    {0x3086, 0x168E},
    {0x3086, 0x032A},
    {0x3086, 0x9C45},
    {0x3086, 0x783F},
    {0x3086, 0x072A},
    {0x3086, 0x9D3E},
    {0x3086, 0x305D},
    {0x3086, 0x2944},
    {0x3086, 0x8810},
    {0x3086, 0x2B04},
    {0x3086, 0x530D},
    {0x3086, 0x4558},
    {0x3086, 0x3E08},
    {0x3086, 0x8E01},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x76A7},
    {0x3086, 0x77A7},
    {0x3086, 0x4644},
    {0x3086, 0x1616},
    {0x3086, 0xA57A},
    {0x3086, 0x1244},
    {0x3086, 0x4B18},
    {0x3086, 0x4A04},
    {0x3086, 0x4316},
    {0x3086, 0x0643},
    {0x3086, 0x1605},
    {0x3086, 0x4316},
    {0x3086, 0x0743},
    {0x3086, 0x1658},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x1645},
    {0x3086, 0x588E},
    {0x3086, 0x032A},
    {0x3086, 0x9C45},
    {0x3086, 0x787B},
    {0x3086, 0x3F07},
    {0x3086, 0x2A9D},
    {0x3086, 0x530D},
    {0x3086, 0x8B16},
    {0x3086, 0x863E},
    {0x3086, 0x2345},
    {0x3086, 0x5825},
    {0x3086, 0x3E10},
    {0x3086, 0x8E01},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x3E10},
    {0x3086, 0x8D60},
    {0x3086, 0x1244},
    {0x3086, 0x4BB9},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},    // SEQ_DATA_PORT
    #else //022718 modified
    {0x3088, 0x8000},    // SEQ_CTRL_PORT
    {0x3086, 0x4558},
    {0x3086, 0x72A6},
    {0x3086, 0x4A31},
    {0x3086, 0x4342},
    {0x3086, 0x8E03},
    {0x3086, 0x2A14},
    {0x3086, 0x4578},
    {0x3086, 0x7B3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA2A},
    {0x3086, 0x043D},
    {0x3086, 0x102A},
    {0x3086, 0x052A},
    {0x3086, 0x1535},
    {0x3086, 0x2A05},
    {0x3086, 0x3D10},
    {0x3086, 0x4558},
    {0x3086, 0x2A04},
    {0x3086, 0x2A14},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2A04},
    {0x3086, 0x622A},
    {0x3086, 0x288E},
    {0x3086, 0x0036},
    {0x3086, 0x2A08},
    {0x3086, 0x3D64},
    {0x3086, 0x7A3D},
    {0x3086, 0x0444},
    {0x3086, 0x2C4B},
    {0x3086, 0xA403},
    {0x3086, 0x430D},
    {0x3086, 0x2D46},
    {0x3086, 0x4316},
    {0x3086, 0x2A90},
    {0x3086, 0x3E06},
    {0x3086, 0x2A98},
    {0x3086, 0x5F16},
    {0x3086, 0x530D},
    {0x3086, 0x1660},
    {0x3086, 0x3E4C},
    {0x3086, 0x2904},
    {0x3086, 0x2984},
    {0x3086, 0x8E03},
    {0x3086, 0x2AFC},
    {0x3086, 0x5C1D},
    {0x3086, 0x5754},
    {0x3086, 0x495F},
    {0x3086, 0x5305},
    {0x3086, 0x5307},
    {0x3086, 0x4D2B},
    {0x3086, 0xF810},
    {0x3086, 0x164C},
    {0x3086, 0x0955},
    {0x3086, 0x562B},
    {0x3086, 0xB82B},
    {0x3086, 0x984E},
    {0x3086, 0x1129},
    {0x3086, 0x9460},
    {0x3086, 0x5C19},
    {0x3086, 0x5C1B},
    {0x3086, 0x4548},
    {0x3086, 0x4508},
    {0x3086, 0x4588},
    {0x3086, 0x29B6},
    {0x3086, 0x8E01},
    {0x3086, 0x2AF8},
    {0x3086, 0x3E02},
    {0x3086, 0x2AFA},
    {0x3086, 0x3F09},
    {0x3086, 0x5C1B},
    {0x3086, 0x29B2},
    {0x3086, 0x3F0C},
    {0x3086, 0x3E03},
    {0x3086, 0x3E15},
    {0x3086, 0x5C13},
    {0x3086, 0x3F11},
    {0x3086, 0x3E0F},
    {0x3086, 0x5F2B},
    {0x3086, 0x902B},
    {0x3086, 0x803E},
    {0x3086, 0x062A},
    {0x3086, 0xF23F},
    {0x3086, 0x103E},
    {0x3086, 0x0160},
    {0x3086, 0x29A2},
    {0x3086, 0x29A3},
    {0x3086, 0x5F4D},
    {0x3086, 0x1C2A},
    {0x3086, 0xFA29},
    {0x3086, 0x8345},
    {0x3086, 0xA83E},
    {0x3086, 0x072A},
    {0x3086, 0xFB3E},
    {0x3086, 0x6745},
    {0x3086, 0x8824},
    {0x3086, 0x3E08},
    {0x3086, 0x2AFA},
    {0x3086, 0x5D29},
    {0x3086, 0x9288},
    {0x3086, 0x102B},
    {0x3086, 0x048B},
    {0x3086, 0x1686},
    {0x3086, 0x8D48},
    {0x3086, 0x4D4E},
    {0x3086, 0x2B80},
    {0x3086, 0x4C0B},
    {0x3086, 0x3F36},
    {0x3086, 0x2AF2},
    {0x3086, 0x3F10},
    {0x3086, 0x3E01},
    {0x3086, 0x6029},
    {0x3086, 0x8229},
    {0x3086, 0x8329},
    {0x3086, 0x435C},
    {0x3086, 0x155F},
    {0x3086, 0x4D1C},
    {0x3086, 0x2AFA},
    {0x3086, 0x4558},
    {0x3086, 0x8E00},
    {0x3086, 0x2A98},
    {0x3086, 0x3F0A},
    {0x3086, 0x4A0A},
    {0x3086, 0x4316},
    {0x3086, 0x0B43},
    {0x3086, 0x168E},
    {0x3086, 0x032A},
    {0x3086, 0x9C45},
    {0x3086, 0x783F},
    {0x3086, 0x072A},
    {0x3086, 0x9D3E},
    {0x3086, 0x305D},
    {0x3086, 0x2944},
    {0x3086, 0x8810},
    {0x3086, 0x2B04},
    {0x3086, 0x530D},
    {0x3086, 0x4558},
    {0x3086, 0x3E08},
    {0x3086, 0x8E01},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x76A7},
    {0x3086, 0x77A7},
    {0x3086, 0x4644},
    {0x3086, 0x1616},
    {0x3086, 0xA57A},
    {0x3086, 0x1244},
    {0x3086, 0x4B18},
    {0x3086, 0x4A04},
    {0x3086, 0x4316},
    {0x3086, 0x0643},
    {0x3086, 0x1605},
    {0x3086, 0x4316},
    {0x3086, 0x0743},
    {0x3086, 0x1658},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x1645},
    {0x3086, 0x588E},
    {0x3086, 0x032A},
    {0x3086, 0x9C45},
    {0x3086, 0x787B},
    {0x3086, 0x3F07},
    {0x3086, 0x2A9D},
    {0x3086, 0x530D},
    {0x3086, 0x8B16},
    {0x3086, 0x863E},
    {0x3086, 0x2345},
    {0x3086, 0x5825},
    {0x3086, 0x3E10},
    {0x3086, 0x8E01},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x3E10},
    {0x3086, 0x8D60},
    {0x3086, 0x1244},
    {0x3086, 0x4BB9},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},    // SEQ_DATA_PORT
    #endif
    {0x301A, 0x10D8},    // RESET_REGISTER
    {0x30B0, 0x1A38},    // DIGITAL_TEST
    {0x31AC, 0x0C0C},    // DATA_FORMAT_BITS
    {0x302A, 0x0008},    // VT_PIX_CLK_DIV
    {0x302C, 0x0001},    // VT_SYS_CLK_DIV
    {0x302E, 0x0002},    // PRE_PLL_CLK_DIV , 74M
    {0x3030, 0x002C},    // PLL_MULTIPLIER
    {0x3036, 0x000C},    // OP_PIX_CLK_DIV
    {0x3038, 0x0001},    // OP_SYS_CLK_DIV
    {0x3002, 0x0000},    // Y_ADDR_START
    {0x3004, 0x0000},    // X_ADDR_START
    {0x3006, 0x043F},    // Y_ADDR_END,         1088
    {0x3008, 0x0787},    // X_ADDR_END,         1928
    {0x300A, 0x0465},    // FRAME_LENGTH_LINES, VT,1125
    {0x300C, 0x044C},    // LINE_LENGTH_PCK,    HT,1100
    {0x3012, 0x0416},    // COARSE_INTEGRATION_TIME
    {0x30A2, 0x0001},    // X_ODD_INC,subsampling
    {0x30A6, 0x0001},    // Y_ODD_INC
    {0x30AE, 0x0001},    // X_ODD_INC_CB
    {0x30A8, 0x0001},    // Y_ODD_INC_CB
    {0x3040, 0x0000},    // READ_MODE
    {0x31AE, 0x0301},    // SERIAL_FORMAT
    {0x3082, 0x0009},    // OPERATION_MODE_CTRL,ERS linear
    {0x30BA, 0x760C},    // DIGITAL_CTRL
    {0x3100, 0x0000},    // AECTRLREG
    {0x3060, 0x000B},    // GAIN
    {0x31D0, 0x0000},    // COMPANDING
    {0x3064, 0x1802},    // SMIA_TEST
    {0x3EEE, 0xA0AA},    // DAC_LD_34_35
    {0x30BA, 0x762C},    // DIGITAL_CTRL
    {0x3F4A, 0x0F70},    // DELTA_DK_PIX_THRES
    {0x309E, 0x016C},    // HIDY_PROG_START_ADDR
    {0x3092, 0x006F},    // ROW_NOISE_CONTROL
    {0x3EE4, 0x9937},    // DAC_LD_24_25
    {0x3EE6, 0x3863},    // DAC_LD_26_27
    {0x3EEC, 0x3B0C},    // DAC_LD_32_33
    {0x30B0, 0x1A3A},    // DIGITAL_TEST
    {0x30B0, 0x1A3A},    // DIGITAL_TEST
    {0x30BA, 0x762C},    // DIGITAL_CTRL
    {0x30B0, 0x1A3A},    // DIGITAL_TEST
    {0x30B0, 0x0A3A},    // DIGITAL_TEST
    {0x3EEA, 0x2838},    // DAC_LD_30_31
    {0x3ECC, 0x4E2D},    // DAC_LD_0_1
    {0x3ED2, 0xFEA6},    // DAC_LD_6_7
    {0x3ED6, 0x2CB3},    // DAC_LD_10_11
    {0x3EEA, 0x2819},    // DAC_LD_30_31
    {0x30B0, 0x1A3A},    // DIGITAL_TEST
    //{0x306E, 0x2418},    // DATAPATH_SELECT
    //{0x306E, 0x2C18},    // DATAPATH_SELECT , 12:10 , PCLK slew rate
    {0x306E, 0x4818},    // DATAPATH_SELECT , 12:10 , PCLK slew rate , from FAE for blue line noise
    {0x31E0, 0x0003},   //enable defect pixel correction
    {0x3096, 0x0080},   //Conversion gain
    {0x3098, 0x0080},   //Conversion gain
    {0x3200, 0x0001},   //BIT0:  AdaCD disable/enable , BIT1: 0=Standard AdaCD, 1=low light
    {0x301A, 0x10DC},    // RESET_REGISTER,para,stream on
    //{0x3070, 0x0003},    // TestPattern,
};

const I2C_ARRAY gain_reg[] = {    // again = power(2, coarse_gain)*(32/(32-fine_gain))*conversion gain
    {0x3060, 0x000B},    // again: coarse:[6:4],fine:[3:0]
    {0x305E, 0x0080},    // dgain: xxxx.yyyyyyy
    {0x3100, 0x0000},    // HCG: bit2 2.7x
    {0x3202, 0x0080},   //noise filter
    {0x3206, 0x0B08},   //noise filter
    {0x3208, 0x1E13},   //noise filter
};

const I2C_ARRAY expo_reg[] = {
    {0x3012, 0x0416},    // COARSE_INTEGRATION_TIME
};

const I2C_ARRAY vts_reg[] = {
    {0x300A, 0x0465},    // FRAME_LENGTH_LINES
};

I2C_ARRAY mirror_reg[] = {
   // {0x301A, 0x10D8}/*{0x3022, 0x0001}*/, //0x3022 group hold can not use, using stop stream
    {0x3040, 0x0000},
    {0x3004, 0x0000},    // X_ADDR_START
    {0x3008, 0x0787},    // X_ADDR_END,         1928
   // {0x301A, 0x10DC}/*{0x3022, 0x0000}*/,

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

#define SensorReg_Read(_reg,_data)  (handle->i2c_bus->i2c_rx(handle->i2c_bus,&(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data) (handle->i2c_bus->i2c_tx(handle->i2c_bus,&(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {

  ISensorIfAPI *sensor_if = &handle->sensor_if_api;

  //SENSOR_DMSG("[%s]", __FUNCTION__);

  sensor_if->HsyncPol(handle, handle->HSYNC_POLARITY);
  sensor_if->VsyncPol(handle, handle->VSYNC_POLARITY);
  sensor_if->ClkPol(handle, handle->PCLK_POLARITY);
  sensor_if->BayerFmt(handle, handle->bayer_id);
  sensor_if->RGBIRFmt(handle, handle->RGBIR_id);
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

    sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //Sensor power on sequence, MCLK, RSTn HLH
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);  // pwdn always low!
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    sensor_if->MCLK(idx, 1, handle->mclk);

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(30);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(2);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    //sensor_if->Set3ATaskOrder(handle, def_order);
    //ISP_config_io(handle);
    //usleep(5000);                                           // t5 150000clk/27M
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    return SUCCESS;

}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    sensor_if->MCLK(idx,0, handle->mclk);

    return SUCCESS;
}

static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i=0,n;
    unsigned short sen_data;
    int table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    //ar0237_params *params = (ar0237_params *)handle->private_data;

    //SENSOR_DMSG("\n\n[%s]", __FUNCTION__);
    for(n=0;n<table_length;++n) {
        id_from_sensor[n].reg = Sensor_id_table[n].reg;
        id_from_sensor[n].data = 0;
    }

    *id =0;
    for(n=0;n<4;++n) //retry , until I2C success
    {
      if(n>2)
      {
          return FAIL;
      }

      if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
          break;
      else
          SENSOR_MSLEEP(1);
    }

    if( id_from_sensor[0].data != Sensor_id_table[0].data ) {
      SENSOR_DMSG("[%s]Read sensor id: 0x%x fail.\n", __FUNCTION__, id_from_sensor[0].data);
      return FAIL;
    }
    *id = id_from_sensor[i].data;
    SENSOR_DMSG("[%s]Read sensor id, Addr=0x%X , ID=0x%x\n", __FUNCTION__,id_from_sensor[0].reg,(int)*id);

    SensorReg_Read(0x31fe, &sen_data);
    if((sen_data==0x00a1)||(sen_data==0x0121)){

        SENSOR_DMSG("[%s]This is RGBIR sensor and right driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return SUCCESS;
     }
    else{
        SENSOR_DMSG("[%s]This is Bayer sensor and wrong driver, sen_data=0x%X\n", __FUNCTION__,sen_data);
        return FAIL;
    }
}

static int pCus_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
  SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

  return SUCCESS;
}

static int pCus_init(ms_cus_sensor *handle)
{
    int i;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;

    //ar0237_params *params = (ar0237_params *)handle->private_data;

    //sensor_if->PCLK(NULL,CUS_PCLK_SR_PAD);
    for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
    {
        if(Sensor_init_table[i].reg==0xffff)
        {
            SENSOR_MSLEEP(Sensor_init_table[i].data);
        }
        else
        {
            int n=0;
            while(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS)
            {//retry
                if(++n>10)
                {
                    return FAIL;
                }
            }
        }
    }
    //printf("[%s] ar0237RGBIR DONE!\n",__func__);
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
  short sen_data;

  sen_data = mirror_reg[0].data;
  SENSOR_DMSG("mirror:%x\r\n", sen_data);
  switch(sen_data & 0xc000) {
    case 0x0000:
      *orit = CUS_ORIT_M0F0;
      break;
    case 0x4000:
      *orit = CUS_ORIT_M1F0;
      break;
    case 0x8000:
      *orit = CUS_ORIT_M0F1;
      break;
    case 0xC000:
      *orit = CUS_ORIT_M1F1;
      break;
  }

  return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
  short index=0;
  ar0237_params *params = (ar0237_params *)handle->private_data;
  //ISensorIfAPI *sensor_if = handle->sensor_if_api;
  //ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;



  switch(orit) {
    case CUS_ORIT_M0F0:
      index = 0;
      handle->bayer_id=    CUS_BAYER_GB;
      mirror_reg[1].data = 0x0;
      mirror_reg[2].data = 0x787;
      break;
    case CUS_ORIT_M1F0:
      index = 0x4000;
      handle->bayer_id=    CUS_BAYER_GR;
      mirror_reg[1].data = 0x1;
      mirror_reg[2].data = 0x78A;
      break;
    case CUS_ORIT_M0F1:
      index = 0x8000;
      handle->bayer_id=    CUS_BAYER_GB;
      break;
    case CUS_ORIT_M1F1:
      index = 0xC000;
      handle->bayer_id=    CUS_BAYER_GR;
      break;
  }

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", index);
  if (index != mirror_reg[0].data) {
    mirror_reg[0].data = index;
    //sensor_if1->SetSkipFrame(handle,2); //skip 2 frame to avoid bad frame after mirror/flip
    //sensor_if->BayerFmt(handle, handle->bayer_id);
   // SensorRegArrayW((I2C_ARRAY*)mirror_reg, ARRAY_SIZE(mirror_reg));
    params->orient_dirty = true;
  }

  return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ar0237_params *params = (ar0237_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = params->tVts_reg[0].data;

    if (params->expo.fps >= 10000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    ar0237_params *params = (ar0237_params *)handle->private_data;

    SENSOR_DMSG("\n\n[%s], fps=%d\n", __FUNCTION__, fps);
    if(fps>=10 && fps <= 30) {//1 based
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
        params->reg_dirty = true;
    }else if(fps>=10000 && fps <= 30000) {//1000 based
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
        params->reg_dirty = true;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    return SUCCESS;
}

static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
  if (cap)
    memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
  else
    return FAIL;
  return SUCCESS;
}


static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    ar0237_params *params = (ar0237_params *)handle->private_data;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
   // ISensorIfAPI2 *sensor_if1 = handle->sensor_if_api2;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        if(params->orient_dirty){
             //sensor_if1->SetSkipFrame(handle,2); //skip 2 frame to avoid bad frame after mirror/flip
             SensorRegArrayW((I2C_ARRAY*)mirror_reg, ARRAY_SIZE(mirror_reg));
             //sensor_if->BayerFmt(handle, handle->bayer_id);
             params->orient_dirty = false;
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

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    ar0237_params *params = (ar0237_params *)handle->private_data;
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

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    ar0237_params *params = (ar0237_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;
    if(lines<1) lines=1;
    if (lines >params->expo.vts-1)
    {
        vts = lines +1;
    }
    else
        vts=params->expo.vts;

    params->tExpo_reg[0].data = lines;
    params->tVts_reg[0].data = vts;

    SENSOR_DMSG("[%s] us %u, lines %u, vts %u\n", __FUNCTION__, us, lines, params->expo.vts);
    params->reg_dirty = true;    //reg need to update
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int rc = SUCCESS;
    int i,tmp,coarse_gain, dgain;
    //float fine_gain, hcg;
    ar0237_params *params = (ar0237_params *)handle->private_data;
//  rc = SensorRegArrayR((I2C_ARRAY*)params->tGain_reg, ARRAY_SIZE(gain_reg));
  if (SUCCESS == rc) {
    tmp = (params->tGain_reg[0].data&0x70) >> 4;
    coarse_gain = 1;
    for (i=0; i<tmp; i++)
        coarse_gain = 2*coarse_gain;

    tmp = params->tGain_reg[0].data&0x0F;
    //fine_gain = 32.0/(32-tmp);

    dgain = params->tGain_reg[2].data & 0x3ff;
    if (params->tGain_reg[2].data & 4) {
        *gain = (coarse_gain * 32 * dgain * 27 * 1024) / (128*10*(32-tmp));
        //hcg =  2.7;
    } else {
        *gain = (coarse_gain * 32 * dgain * 1024) / (128*(32-tmp));
        //hcg = 1;
    }
    //*gain = (coarse_gain * fine_gain * dgain * hcg * 1024) / 128; // 128 for dgain xxxx.yyyyyyy
  }

//  SENSOR_DMSG("[%s] gain/reg = 0x%x/0x%x\n", __FUNCTION__, *gain, temp_gain);
  return rc;
}

 // again = power(2, coarse_gain)*(32/(32-fine_gain))*conversion gain
 // fine_gain = 32 - ( (power(2, coarse_gain)*conversion_gain*32) / again )
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
#define MAX_A_GAIN 44237//(16*1024*2.7)
    int coarse_gain, fine_gain,conversion_gain;
    u8 coarse_reg=0;
    u32 again = 0;
    u32 dgain = 0;
    u32 input_gain = 0;
    bool hcg =0;
    ar0237_params *params = (ar0237_params *)handle->private_data;
    params->expo.final_gain = gain;

    input_gain = gain;

    if(gain<1557/*1024*1.52*/)
        gain=1557;/*1024*1.52*/
    else if(gain>=SENSOR_MAX_GAIN*1024)
        gain=SENSOR_MAX_GAIN*1024;


    if (gain > 2765/*2.7*1024*/) {    // 2.7x must use HCG!
        hcg =1;
        params->tGain_reg[2].data |= 4;
        gain = (gain *10) / 27;//gain /2.7;
        conversion_gain = 270;//100*2.7;
        params->tGain_reg[3].data = 0x00B0;   //0x3202 , noise filter
        params->tGain_reg[4].data = 0x1C0E;   //0x3206, noise filter
        params->tGain_reg[5].data = 0x4E39;   //0x3208, noise filter
    } else {
        hcg =0;
        params->tGain_reg[2].data &= 0xfffb;
        conversion_gain = 100;
        params->tGain_reg[3].data = 0x0080;   //0x3202, noise filter
        params->tGain_reg[4].data = 0x0B08;   //0x3206, noise filter
        params->tGain_reg[5].data = 0x1E13;   //0x3208, noise filter
    }

    if (gain >= 16*1024) {
        coarse_gain = 16;
        coarse_reg = 0x4;
    } else if (gain >= 8*1024) {
        coarse_gain = 8;
        coarse_reg = 0x3;
    } else if (gain >= 4*1024) {
        coarse_gain = 4;
        coarse_reg = 0x2;
    } else if (gain >= 2*1024) {
        coarse_gain = 2;
        coarse_reg = 0x1;
    } else {
        coarse_gain = 1;
        coarse_reg = 0x0;
    }

    if(input_gain <= 44237) //44237 = 2^4 * 2.7 * 1024 Adjust A-gain when < 43.2x
    {
        fine_gain = 32 - ( (1024*coarse_gain*32+512) / (gain>MAX_A_GAIN?MAX_A_GAIN:gain) );

        if(/*(hcg == 1) && */(coarse_gain >= 2) && ((fine_gain & 0x01) == 1)){ //fine gain one step become 2x when coarse_gain >=1 reference to excel gain table
           fine_gain &=0xFFFFFFFE;
        }

        params->tGain_reg[0].data = (coarse_reg<<4) | fine_gain;
        //pr_err("params->tGain_reg[0].data %d\n", params->tGain_reg[0].data);
        if (hcg == 1) {
            if ((fine_gain == 16) && (coarse_reg == 1)) {
                params->tGain_reg[0].data+=16;
            }
            if (params->tGain_reg[0].data > 0x10) {
                params->tGain_reg[0].data -= 2;
                fine_gain = params->tGain_reg[0].data & 0xF;
                coarse_reg = (params->tGain_reg[0].data >> 4) & 0xF;
                coarse_gain = (1 << coarse_reg);
            } else {
                if (params->tGain_reg[0].data == 0) {
                    params->tGain_reg[0].data = 0x18;
                    fine_gain = 0x8;
                    coarse_gain = 2;
                    coarse_reg = 0x1;
                    hcg =0;
                    params->tGain_reg[2].data &= 0xfffb;
                    conversion_gain = 100;
                    params->tGain_reg[3].data = 0x0080;   //0x3202, noise filter
                    params->tGain_reg[4].data = 0x0B08;   //0x3206, noise filter
                    params->tGain_reg[5].data = 0x1E13;   //0x3208, noise filter
                } else {
                    params->tGain_reg[0].data --;
                    fine_gain = params->tGain_reg[0].data & 0xF;
                }
            }

        }else {
            if (params->tGain_reg[0].data == 0x10) {
                params->tGain_reg[0].data -= 1;
                fine_gain = params->tGain_reg[0].data & 0xF;
                coarse_gain = 1;
                coarse_reg = 0x0;
            } else if (params->tGain_reg[0].data > 0x10) {
                params->tGain_reg[0].data -= 2;
                fine_gain = params->tGain_reg[0].data & 0xF;
            } else {
                if (params->tGain_reg[0].data < 0xB) {
                    params->tGain_reg[0].data = 0xB;
                    fine_gain = 0xB;

                } else {
                    params->tGain_reg[0].data --;
                    fine_gain = params->tGain_reg[0].data & 0xF;
                }
            }

        }

        again = /*(float)*/(32*coarse_gain*conversion_gain)/(32-fine_gain)*100;

        dgain = (input_gain*1000/1024)*1000/again*128;  //use dgain compensate again linearity
        //pr_err("again %d dgain %d fine_gain %d\n", again, dgain, fine_gain);
        if(dgain < 13300)
            dgain = 13300;

        params->tGain_reg[1].data = dgain/100;

        //printf("~~~input_gain : %d again:%d again_next:%d dgain_c:%d\n",input_gain,again,again_next,dgain);
    }
    else if(input_gain > 44237){      //Adjust D-gain when > 43.2x

        params->tGain_reg[0].data = 0x40;
        dgain = /*(float)*/(input_gain*100)/44237;

        dgain *=128;

        if(dgain < 13300)
            dgain = 13300;

        params->tGain_reg[1].data = dgain/100;
    }
    //printf("[%s] TotalGain=%d(%dX) ,A-gain reg=0x%x , D-gain reg=%d\n", __FUNCTION__,input_gain,input_gain/1024,params->tGain_reg[0].data ,params->tGain_reg[1].data);
    //pr_err("input_gain %d, 0x3060=0x%X, 0x305E=0x%X, 0x3100=0x%X, 0x3202=0x%X, 0x3206=0x%X, 0x3208=0x%X\n",
    //                input_gain, params->tGain_reg[0].data, params->tGain_reg[1].data, params->tGain_reg[2].data, params->tGain_reg[3].data, params->tGain_reg[4].data, params->tGain_reg[5].data);
    //pr_err("gain %d, coarse_gain %d\n", gain, coarse_gain);
    params->reg_dirty = true;    //reg need to update
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 30;
  *max = 1000000/12;
  return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 1557;//1024*1.52;
  *max = SENSOR_MAX_GAIN*1024;

  return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    //u32 i, j;

    SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}

static int AR0237_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 1;
    info->step = Preview_line_period;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle)
{

    return SUCCESS;
}

int cus_camsensor_init_handle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    ar0237_params *params;

    if (!handle) {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }

    SENSOR_DMSG("[%s]", __FUNCTION__);

    if (handle->private_data == NULL) {
        SENSOR_EMSG("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (ar0237_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    sprintf(handle->model_id, SENSOR_MODEL_ID);
    handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;//CUS_SENIF_BUS_PARL;
    handle->data_prec   = SENSOR_DATAPREC;  //CUS_DATAPRECISION_8;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;   //CUS_BAYER_GB;
    handle->RGBIR_id    = SENSOR_RGBIRID;   //CUS_BAYER_GB;
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;

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
    handle->video_res_supported.res[0].nOutputWidth= 0x788;
    handle->video_res_supported.res[0].nOutputHeight= 0x440;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

    handle->i2c_cfg.mode                = I2C_NORMAL_MODE;      //old OVT I2C protocol=>I2C_LEGACY_MODE
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;

    handle->mclk                        = Preview_MCLK_SPEED;

    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;
    handle->reset_POLARITY              = SENSOR_RST_POL;
    handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL;
    handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;

    handle->ae_gain_delay       = 2;
    handle->ae_shutter_delay    = 2;

    handle->ae_gain_ctrl_num = 1;
    handle->ae_shutter_ctrl_num = 1;

    handle->sat_mingain = 1024;
    handle->pCus_sensor_release     = cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init;
    handle->pCus_sensor_poweron     = pCus_poweron;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;
    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien;
    handle->pCus_sensor_GetFPS            = pCus_GetFPS;
    handle->pCus_sensor_SetFPS            = pCus_SetFPS;
    handle->pCus_sensor_GetSensorCap      = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode    = pCus_SetPatternMode;

    handle->pCus_sensor_AEStatusNotify = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal   = pCus_SetAEGain; //pCus_SetAEGain_cal;
    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo = AR0237_GetShutterInfo;
    params->expo.vts = vts_30fps;
    params->expo.fps = 30;

    //pthread_mutex_init(&params->rw_lock,NULL);

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  AR0237_RGBIR,
                            cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            ar0237_params
                         );

