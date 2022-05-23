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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC2232);

#define SENSOR_ISP_TYPE     ISP_EXT
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_PARL      //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000

#define SENSOR_BAYERID      CUS_BAYER_GR            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_ORIT             CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAXGAIN         (155 * 31 / 10)                  // max sensor again, a-gain * conversion-gain*d-gain

#define Preview_MCLK_SPEED      CUS_CMU_CLK_27MHZ            //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period     35556                   //2640*1125*25
#define Prv_Max_line_number     1080                    //maximum exposure line munber of sensor when preview
#define vts_25fps               1125                    //for 25 fps
#define Preview_WIDTH           1920                    //resolution Width when preview
#define Preview_HEIGHT      1080                   //resolution Height when preview
#define Preview_MAX_FPS     25                      //fastest preview FPS
#define Preview_MIN_FPS     3                       //slowest preview FPS


#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE
#define SENSOR_I2C_FMT      I2C_FMT_A16D8
#define SENSOR_I2C_ADDR         0x60                    //I2C slave address
#define SENSOR_I2C_SPEED        200000                  //200KHz

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG         //if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG         //if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG             //CUS_CLK_POL_NEG //CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
#define ENABLE_NR 1

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
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tMirror_reg[2];
    I2C_ARRAY tNr_reg[3];
    I2C_ARRAY tNr2_reg[1];
    I2C_ARRAY tDPC_reg[3];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty; //sensor setting need to update through I2C
    bool orient_dirty;
    //pthread_mutex_t rw_lock;
} sc2232_params;

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x22},
    {0x3108, 0x32},
};

const I2C_ARRAY Sensor_init_table[] =     // PARA12_1928*1088_30fps_27MCLK_74.25PCLK
{
{0x0103,0x01},
{0xffff,0x05},    // delay
{0x0100,0x00},

//close mipi
{0x3018,0x1f},
{0x3019,0xff},
{0x301c,0xb4},

{0x320c,0x0a},
{0x320d,0x50},     //a80->a50

{0x3e01,0x23},

{0x363c,0x05},     //04
{0x3635,0xa8},     //c0
{0x363b,0x0d},     //0d

{0x3620,0x08},
{0x3622,0x02},
{0x3635,0xc0},
{0x3908,0x10},

{0x3624,0x08},     //count_clk inv  need debug  flash row in one channel

{0x5000,0x06},     //rts column test
{0x3e06,0x00},
{0x3e08,0x03},
{0x3e09,0x10},
{0x3333,0x10},
{0x3306,0x7e},

{0x3902,0x05},

{0x3213,0x08},

{0x337f,0x03},      //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
{0x3368,0x04},
{0x3369,0x00},
{0x336a,0x00},

{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x30},

{0x3366,0x7c},      // div_rst gap
{0x3633,0x42},
{0x330b,0xe0},

{0x3637,0x57},
{0x3302,0x1f},      // adjust the gap betwen first and second cunt_en pos edage to even times the clk
{0x3309,0xde},      // adjust the gap betwen first and second cunt_en pos edage to even times the clk
{0x303f,0x81},      // pclk sel pll_sclk_dig_div

//leage current
{0x3907,0x00},

{0x3908,0x61},
{0x3902,0x45},
{0x3905,0xb8},
{0x3e01,0x8c},
{0x3e02,0x10},

{0x3e06,0x00},
{0x3038,0x48},
{0x3637,0x5d},
{0x3e06,0x00},

//0921
{0x3908,0x11},
{0x335e,0x01},
{0x335f,0x03},
{0x337c,0x04},
{0x337d,0x06},
{0x33a0,0x05},
{0x3301,0x04},

{0x3633,0x4f},   //prnu
{0x3622,0x06},   //blksun
{0x3630,0x08},
{0x3631,0x84},
{0x3306,0x30},

{0x366e,0x08},   // ofs auto en [3]

{0x366f,0x22},   // ofs+finegain  real ofs in 0x3687[4:0]
{0x3637,0x59},   // FW to 4.6k //9.22
{0x3320,0x06},   // New ramp offset timing

{0x3326,0x00},
{0x331e,0x11},
{0x331f,0xc1},
{0x3303,0x20},

{0x3309,0xd0},
{0x330b,0xbe},
{0x3306,0x36},

{0x3635,0xc2},    //TxVDD,HVDD
{0x363b,0x0a},
{0x3038,0x88},

//9.22
{0x3638,0x1f},
{0x3636,0x25},
{0x3625,0x02},
{0x331b,0x83},
{0x3333,0x30},

//10.18
{0x3635,0xa0},
{0x363b,0x0a},
{0x363c,0x05},

{0x3314,0x13},   //preprecharge

//20171101 reduce hvdd pump lighting
{0x3038,0xc8},   // high pump clk,low lighting
{0x363b,0x0b},   //high hvdd ,low lighting
{0x3632,0x18},   //large current,low ligting  0x38 (option)

//20171102 reduce hvdd pump lighting
{0x3038,0xff}, // high pump clk,low lighting


{0x3639,0x09},
{0x3621,0x28},
{0x3211,0x0c},

//20171106
{0x366f,0x26},

////20171121
{0x366f,0x2f},
{0x3320,0x01},
{0x3306,0x48},
{0x331e,0x19},
{0x331f,0xc9},

{0x330b,0xd3},
{0x3620,0x28},

//20171122
{0x3309,0x60},
{0x331f,0x59},
{0x3308,0x10},
{0x3630,0x0c},

//digital ctrl
{0x3f00,0x07},    // bit[2] = 1
{0x3f04,0x05},
{0x3f05,0x04},    // hts / 2 - 0x24
{0x3802,0x01},
{0x3235,0x08},

{0x3236,0xc8},    // vts x 2 - 2
//20171127

{0x3630,0x1c},
//20171130
{0x33aa,0x10},    //low power
//20171208  logical   inter
{0x3670,0x04},
{0x3677,0x84},
{0x3678,0x88},
{0x3679,0x88},
{0x367e,0x08},
{0x367f,0x28},

{0x3670,0x0c},
{0x3690,0x34},
{0x3691,0x11},
{0x3692,0x42},
{0x369c,0x08},
{0x369d,0x28},

{0x360f,0x01},
{0x3671,0xc6},
{0x3672,0x06},
{0x3673,0x16},
{0x367a,0x28},
{0x367b,0x3f},

{0x3222,0x29},
{0x3901,0x02},
{0x3905,0x98},

{0x3e1e,0x34},

{0x3314,0x04},

{0x3301,0x06},
{0x3306,0x48},
{0x3632,0x08},
{0x3e00,0x00},
{0x3e01,0x46},
{0x3e02,0x10},
{0x3e03,0x0b},
{0x3e06,0x00},
{0x3e07,0x80},
{0x3e08,0x03},
{0x3e09,0x10},

{0x5780,0xff},

{0x0100,0x01},

#if 0
//{0x0103,0x01},
{0x0100,0x00},
{0x3621,0x28},
{0x3309,0x60},
{0x331f,0x4d},
{0x3321,0x4f},
{0x33b5,0x10},

{0x3303,0x20},
{0x331e,0x0d},
{0x3320,0x0f},

{0x3622,0x02},
{0x3633,0x42},
{0x3634,0x42},

{0x3306,0x66},
{0x330b,0xd1},

{0x3301,0x0e},
{0x320c,0x0a},
{0x320d,0x50},

{0x3364,0x05},

{0x363c,0x28},
{0x363b,0x0a},
{0x3635,0xa0},

{0x4500,0x59},
{0x3d08,0x00},
{0x3908,0x11},

{0x363c,0x08},
{0x3e03,0x03},
{0x3e01,0x46},

{0x3381,0x0a},
{0x3348,0x09},
{0x3349,0x50},
{0x334a,0x02},
{0x334b,0x60},

{0x3380,0x04},
{0x3340,0x06},
{0x3341,0x50},
{0x3342,0x02},
{0x3343,0x60},

{0x3632,0x88},
{0x3309,0xa0},
{0x331f,0x8d},
{0x3321,0x8f},

{0x335e,0x01},
{0x335f,0x03},
{0x337c,0x04},
{0x337d,0x06},
{0x33a0,0x05},
{0x3301,0x05},

{0x3670,0x08},
{0x367e,0x07},  //gain0
{0x367f,0x0f},  //gain1
{0x3677,0x2f},  //<gain0
{0x3678,0x22},  //gain0 - gain1
{0x3679,0x43},  //>gain1

{0x337f,0x03},
{0x3368,0x02},
{0x3369,0x00},
{0x336a,0x00},
{0x336b,0x00},
{0x3367,0x08},
{0x330e,0x30},

{0x3366,0x7c},

{0x3635,0xc1},
{0x363b,0x09},
{0x363c,0x07},
{0x391e,0x00},
{0x3637,0x14},

{0x3306,0x54},
{0x330b,0xd8},
{0x366e,0x08},
{0x366f,0x2f},

{0x3631,0x84},
{0x3630,0x48},
{0x3622,0x06},

{0x3638,0x1f},
{0x3625,0x02},
{0x3636,0x24},
{0x3348,0x08},
{0x3e03,0x0b},

//7.17fpn
{0x3342,0x03},
{0x3343,0xa0},
{0x334a,0x03},
{0x334b,0xa0},

//7.18
{0x3343,0xb0},
{0x334b,0xb0},



//7.20
{0x3802,0x01},
{0x3235,0x04},
{0x3236,0x63},  //vts-2
{0x3343,0xd0},  //fpn
{0x334b,0xd0},
{0x3348,0x07},
{0x3349,0x80},

{0x391b,0x4d},
{0x3222,0x29},
{0x3901,0x02},
{0x3f00,0x07},
{0x3f04,0x0a},
{0x3f05,0x2c}, //hts - 0x24

//{0x3d08,0x01},
{0x0100,0x01},
#endif
};

FINE_GAIN fine_again[] = {
//gain map update for 1/16 precision
    {100000, 0x10},
    {106250, 0x11},
    {112500, 0x12},
    {118750, 0x13},
    {125000, 0x14},
    {131250, 0x15},
    {137500, 0x16},
    {143750, 0x17},
    {150000, 0x18},
    {156250, 0x19},
    {162500, 0x1a},
    {168750, 0x1b},
    {175000, 0x1c},
    {181250, 0x1d},
    {187500, 0x1e},
    {193750, 0x1f},
};


const I2C_ARRAY mirror_reg[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    {0x3213, 0x04}, // crop for bayer
};

const I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},//Dgain [3:0]
    {0x3e07, 0x80},//[7:0]
    {0x3e08, 0x00|0x03},//Agian [4:2]
    {0x3e09, 0x10}, //[7:0]
};

const I2C_ARRAY expo_reg[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
};

const I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65},
};

#if ENABLE_NR
const I2C_ARRAY nr_reg[] = {
    {0x3301, 0x06},
    {0x3306, 0x48},
    {0x3632, 0x08},
};
#endif

const I2C_ARRAY nr2_reg[] = {
    {0x3314, 0x02},
};

const I2C_ARRAY DPC_reg[] = {
    {0x5781,0x04},
    {0x5785,0x18},
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
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {

  ISensorIfAPI *sensor_if = handle->sensor_if_api;

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
static int sc2232poweron(ms_cus_sensor *handle, u32 idx)
{

    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //Sensor power on sequence
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);

    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_USLEEP(5000);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);
    //SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_USLEEP(2000);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);

    return SUCCESS;
}

static int sc2232poweroff(ms_cus_sensor *handle, u32 idx)
{

    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_MSLEEP(50);
    sensor_if->MCLK(idx, 0, handle->mclk );
    return SUCCESS;
}


/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int sc2232GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i,n;
    int table_length= ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    pr_info("\n[%s]12345678\n",__FUNCTION__);
    for(n=0;n<table_length;++n)
    {
        id_from_sensor[n].reg = Sensor_id_table[n].reg;
        id_from_sensor[n].data = 0;
    }

    *id =0;
    if(table_length>8) table_length=8;

    for(n=0;n<4;++n)        //retry , until I2C success
    {
        if(n>2) return FAIL;

        if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS)        //read sensor ID from I2C
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
            SENSOR_USLEEP(1000);
    }
    pr_info("[%s] %s get sensor id:0x%x, SUCCESS!\n", __FUNCTION__, handle->model_id, ( int )*id);
    return SUCCESS;
}

static int sc2232SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    return SUCCESS;
}

static int sc2232Init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    pr_info("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
    for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
    {
        //pr_info("[%s] addr:0x%x, data:0x%x\n", __FUNCTION__, Sensor_init_table[i].reg, Sensor_init_table[i].data);
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
                pr_info("[%s] set Sensor_init_table retry : %d...\n",__FUNCTION__,cnt);
                if(cnt>=10)
                {
                    pr_info("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_USLEEP(10000);
            }
        }
    }
    SENSOR_MSLEEP(50);

    return SUCCESS;
}

static int sc2232GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}
static int sc2232GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int sc2232GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int sc2232SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = sc2232Init;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int sc2232GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{

    char sen_data;
    sc2232_params *params = (sc2232_params *)handle->private_data;
    sen_data = params->tMirror_reg[0].data;
    SENSOR_DMSG("[%s] mirror:%x\r\n", __FUNCTION__, sen_data & 0x66);
    switch(sen_data & 0x66)
    {
        case 0x00:
            *orit = CUS_ORIT_M0F0;
            break;
        case 0x06:
            *orit = CUS_ORIT_M1F0;
            break;
        case 0x60:
            *orit = CUS_ORIT_M0F1;
            break;
        case 0x66:
            *orit = CUS_ORIT_M1F1;
            break;
    }

    return SUCCESS;
}

static int sc2232SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{

  sc2232_params *params = (sc2232_params *)handle->private_data;

  switch(orit) {
    case CUS_ORIT_M0F0:
      if (params->tMirror_reg[0].data) {
          params->tMirror_reg[0].data = 0;
          params->tMirror_reg[1].data = 4;
          params->reg_dirty = true;
      }
            break;
        case CUS_ORIT_M1F0:
      if (params->tMirror_reg[0].data!=6) {
          params->tMirror_reg[0].data = 6;
          params->tMirror_reg[1].data = 4;
          params->reg_dirty = true;
      }
            break;
        case CUS_ORIT_M0F1:
      if (params->tMirror_reg[0].data!=0x60) {
          params->tMirror_reg[0].data = 0x60;
          params->tMirror_reg[1].data = 4;
          params->reg_dirty = true;
      }
            break;
        case CUS_ORIT_M1F1:
      if (params->tMirror_reg[0].data!=0x66) {
          params->tMirror_reg[0].data = 0x66;
          params->tMirror_reg[1].data = 4;
          params->reg_dirty = true;
      }
      break;
  }

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", orit);

  return SUCCESS;
}

static int sc2232GetFPS(ms_cus_sensor *handle)
{
    sc2232_params *params = (sc2232_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_25fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_25fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int sc2232SetFPS(ms_cus_sensor *handle, u32 fps)
{
    sc2232_params *params = (sc2232_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);
    if(fps>=5 && fps <= 25) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_25fps*25)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        return SUCCESS;
    }if(fps>=5000 && fps <= 25000) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_25fps*25000)/fps;
        params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = true;
        return SUCCESS;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int sc2232AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc2232_params *params = (sc2232_params *)handle->private_data;


    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //SensorReg_Write(0x3001,0);
            break;
        case CUS_FRAME_ACTIVE:
            if(params->reg_dirty)
            {
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
#if ENABLE_NR

            SensorReg_Write(0x3812,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tNr2_reg, sizeof(nr2_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tDPC_reg, sizeof(DPC_reg)/sizeof(I2C_ARRAY));
            SensorReg_Write(0x3812,0x30);
#endif

            params->reg_dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int sc2232GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    sc2232_params *params = (sc2232_params *)handle->private_data;
    lines |= (u32)(params->tExpo_reg[0].data&0x0f)<<16;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xf0)<<0;
    lines >>= 4;
  *us = (lines*Preview_line_period)/1000/2; //return us

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;
}

static int sc2232SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0, vts = 0;
    sc2232_params *params = (sc2232_params *)handle->private_data;

    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x00}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, params->tExpo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<1) half_lines=1;
    if (half_lines >  2 * (params->expo.vts)-4) {
        half_lines = 2 * (params->expo.vts)-4;
    }
    else
     vts=params->expo.vts;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);
    half_lines = half_lines<<4;
    params->tExpo_reg[0].data = (half_lines>>16) & 0x0f;
    params->tExpo_reg[1].data = (half_lines>>8) & 0xff;
    params->tExpo_reg[2].data = (half_lines>>0) & 0xf0;

    if (((half_lines >>8) & 0x0fff) < 5)
    {
        params->tNr2_reg[0].data = 0x14;
    }else if (((half_lines >>8) & 0x0fff) > 0xa)
    {
        params->tNr2_reg[0].data = 0x04;
    }

    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tExpo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }

    return SUCCESS;
}

// Gain: 1x = 1024
static int sc2232GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int rc = 0;
    u8 Dgain = 1,  Coarse_gain = 1;
    sc2232_params *params = (sc2232_params *)handle->private_data;

    Coarse_gain = ((params->tGain_reg[2].data&0x1C)>>2) +1;
    Dgain = ((params->tGain_reg[0].data&0x0f) + 1);

    *gain = (Coarse_gain*Dgain*(params->tGain_reg[1].data)*(params->tGain_reg[3].data))/2;

    //SENSOR_DMSG("[%s] gain/reg = %d, 0x%x,  0x%x  0x%x\n", __FUNCTION__, *gain,gain_reg[0].data,gain_reg[1].data,gain_reg[2].data);
    //SENSOR_DMSG("[%s] gain/reg = %d,%d, %d\n", __FUNCTION__, *gain,Coarse_gain,Dgain);
    return rc;
}

static int sc2232SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    sc2232_params *params = (sc2232_params *)handle->private_data;

    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u64 Fine_againx16 = 31,Fine_dgain = 128;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x00},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x10},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain<1024) {
        gain=1024;
    } else if (gain>=SENSOR_MAXGAIN*1024) {
        gain=SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        params->tNr_reg[0].data = 0x06;
        params->tNr_reg[1].data = 0x48;
        params->tNr_reg[2].data = 0x08;
    } else if (gain < 4*1024) {
        params->tNr_reg[0].data = 0x14;
        params->tNr_reg[1].data = 0x48;
        params->tNr_reg[2].data = 0x08;
    } else if (gain < 8*1024) {
        params->tNr_reg[0].data = 0x18;
        params->tNr_reg[1].data = 0x48;
        params->tNr_reg[2].data = 0x08;
    } else if (gain < (155/10)*1024) {
        params->tNr_reg[0].data = 0x13;
        params->tNr_reg[1].data = 0x48;
        params->tNr_reg[2].data = 0x08;
    } else if (gain < 31*1024) {
        params->tNr_reg[0].data = 0xf9;
        params->tNr_reg[1].data = 0x78;
        params->tNr_reg[2].data = 0x48;
    } else {
        params->tNr_reg[0].data = 0xf9;
        params->tNr_reg[1].data = 0x78;
        params->tNr_reg[2].data = 0x78;
    }
#endif


    if (gain < 10*1024) {
        params->tDPC_reg[0].data = 0x04;
        params->tDPC_reg[1].data = 0x18;
    } else {
        params->tDPC_reg[0].data = 0x02;
        params->tDPC_reg[1].data = 0x08;
    }

        if (gain < 2 * 1024)
        {
        Dgain = 1;      Fine_dgain = 128;         Coarse_gain = 1;
            Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x3;
        }
        else if (gain <  4 * 1024)
        {
        Dgain = 1;      Fine_dgain = 128;         Coarse_gain = 2;
            Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x7;
        }
        else if (gain < 8 * 1024)
        {
        Dgain = 1;      Fine_dgain = 128;         Coarse_gain = 4;
            Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0xf;
        }
    else if (gain <=  15872)
        {
        Dgain = 1;      Fine_dgain = 128;         Coarse_gain = 8;
            Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
        }
        else if (gain < 31 * 1024)
        {
            Dgain = 1;      Fine_againx16 = 31;    Coarse_gain = 8;
            Dgain_reg = 0;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
        }
        else if (gain < 62 * 1024)
        {
            Dgain = 2;      Fine_againx16 = 31;    Coarse_gain = 8;
            Dgain_reg = 1;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
        }
        else if (gain < 128 * 1024)
        {
            Dgain = 4;      Fine_againx16 = 31;    Coarse_gain = 8;
            Dgain_reg = 3;  Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
        }
        else if (gain < SENSOR_MAXGAIN * 1024)
        {
            Dgain = 8;      Fine_againx16 = 31;    Coarse_gain = 8;
            Dgain_reg = 7; Fine_again_reg = 0x1f;  Coarse_gain_reg = 0x1f;
        }

    if (gain <= 15872)
    {
        Fine_again_reg = gain*2 / (Dgain * Coarse_gain * Fine_dgain);
    }
    else
    {
        Fine_dgain_reg = gain*2 / (Dgain * Coarse_gain * Fine_againx16);
    }

        SENSOR_DMSG("[%s]  gain : %f ,%f ,%d , %d\n\n", __FUNCTION__,Fine_again,Fine_dgain,Dgain,Coarse_gain);

        //Fine_again =(float)gain / (Dgain * Coarse_gain * Fine_dgain)/ 1024;
        //Fine_again_reg = abs((Fine_again - 1) * 16) + 16;

        params->tGain_reg[3].data = Fine_again_reg;
        params->tGain_reg[2].data = Coarse_gain_reg;
        params->tGain_reg[1].data = Fine_dgain_reg;
        params->tGain_reg[0].data = Dgain_reg & 0xF;
        //SENSOR_DMSG("[%s] set gain =%d %f %f 0x%x ,0x%x, 0x%x ,0x%x\n", __FUNCTION__, gain,Fine_again,Fine_dgain,gain_reg[0].data,gain_reg[1].data,gain_reg[2].data,gain_reg[3].data);
    for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (params->tGain_reg[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }

    return SUCCESS;
}

static int sc2232GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 30;
  *max = 1000000/Preview_MIN_FPS;

  return SUCCESS;
}

static int sc2232GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 1024;
  *max = SENSOR_MAXGAIN*1024;

  return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {
    //u32 i, j;

    SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);

    return SUCCESS;
}

static int sc2232GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 1;
    info->step = Preview_line_period;
    return SUCCESS;
}

int sc2232ReleaseHandle(ms_cus_sensor *handle)
{
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    if (handle && handle->private_data)
    {
        SENSOR_IMSG("[%s] release handle 0x%x, private data %x\n",__FUNCTION__,(int)handle,(int)handle->private_data);
        CamOsMemRelease(handle->private_data);
        handle->private_data = NULL;
    }
    return SUCCESS;
}

int sc2232InitHandle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc2232_params *params = 0;
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
    params = (sc2232_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
    memcpy(params->tNr2_reg, nr2_reg, sizeof(nr2_reg));
    memcpy(params->tDPC_reg, DPC_reg, sizeof(DPC_reg));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc2232_PARL");

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
    handle->orient      = SENSOR_ORIT;      //CUS_ORIT_M1F1;
    //handle->YC_ODER     = SENSOR_YCORDER;   //CUS_SEN_YCODR_CY;

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
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@25fps");

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
    handle->sat_mingain=1024;
    handle->pCus_sensor_release     = sc2232ReleaseHandle;
    handle->pCus_sensor_init        = sc2232Init;
    handle->pCus_sensor_poweron     = sc2232poweron ;
    handle->pCus_sensor_poweroff    = sc2232poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID     = sc2232GetSensorID;

    handle->pCus_sensor_GetVideoResNum = sc2232GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = sc2232GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = sc2232GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = sc2232SetVideoRes;

    handle->pCus_sensor_GetOrien        = sc2232GetOrien;
    handle->pCus_sensor_SetOrien        = sc2232SetOrien;
    handle->pCus_sensor_GetFPS          = sc2232GetFPS;
    handle->pCus_sensor_SetFPS          = sc2232SetFPS;
    handle->pCus_sensor_SetPatternMode  = sc2232SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = sc2232AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = sc2232GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = sc2232SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = sc2232GetAEGain;
    handle->pCus_sensor_SetAEGain       = sc2232SetAEGain;
    handle->pCus_sensor_SetAEGain_cal   = sc2232SetAEGain;
    handle->pCus_sensor_GetShutterInfo  = sc2232GetShutterInfo;

    handle->pCus_sensor_GetAEMinMaxGain = sc2232GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= sc2232GetAEMinMaxUSecs;

    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    params->expo.vts=vts_25fps;
    params->expo.fps = 25;
      params->reg_dirty = false;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX( SC2232,
                            sc2232InitHandle,
                            NULL,
                            NULL,
                            sc2232_params
                         );

