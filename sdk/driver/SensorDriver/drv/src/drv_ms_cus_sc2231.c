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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC2231);

#define SENSOR_ISP_TYPE     ISP_EXT
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_PARL      //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000

#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAXGAIN      (1575 * 315 / 1000)                  // max sensor again, a-gain *d-gain

#define Preview_MCLK_SPEED      CUS_CMU_CLK_27MHZ       //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period     29630                   //2640*1125*25
#define Prv_Max_line_number     1080                    //maximum exposure line munber of sensor when preview
#define vts_30fps               1125                    //for 30 fps
#define Preview_WIDTH           1920                    //resolution Width when preview
#define Preview_HEIGHT          1080                    //resolution Height when preview
#define Preview_MAX_FPS         30                      //fastest preview FPS
#define Preview_MIN_FPS         3                       //slowest preview FPS


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
#define SENSOR_PCLK_POL     CUS_CLK_POL_NEG             // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG


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
        u32 line;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty; //sensor setting need to update through I2C
    bool orient_dirty;
    //pthread_mutex_t rw_lock;
} sc2231_params;

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x22},
    {0x3108, 0x32},
    {0x3109, 0x20},
};

I2C_ARRAY Sensor_init_table[] =     // SC2230_27MInput_74.25M_10bit_1920x1080_30fps.ini;
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x3002,0x80},
    {0x3038,0x6e},
    {0x3039,0x52},
    {0x303b,0x14},
    {0x303d,0x10},
    {0x3221,0x80},
    {0x3301,0x08},
    {0x3303,0x38},
    {0x3306,0x50},
    {0x3309,0x70},
    {0x330a,0x00},
    {0x330b,0xd0},
    {0x330d,0x36},
    {0x330e,0x18},
    {0x330f,0x01},
    {0x3310,0x23},
    {0x3314,0x14},
    {0x331e,0x31},
    {0x331f,0x69},
    {0x3338,0x37},
    {0x3339,0x37},
    {0x333a,0x33},
    {0x335d,0x20},
    {0x3364,0x1d},
    {0x3367,0x08},
    {0x33aa,0x00},
    {0x33ae,0x32},
    {0x33b3,0x32},
    {0x33b4,0x32},
    {0x33b6,0x0f},
    {0x33b7,0x3e},
    {0x33b8,0x08},
    {0x33b9,0x80},
    {0x33ba,0xc0},
    {0x360f,0x05},
    {0x3614,0x80},
    {0x3622,0xf6},
    {0x3630,0x40},
    {0x3631,0x88},
    {0x3632,0x88},
    {0x3633,0x24},
    {0x3635,0x1c},
    {0x3637,0x2c},
    {0x3638,0x24},
    {0x363a,0x80},
    {0x363b,0x16},
    {0x363c,0x06},
    {0x3641,0x01},
    {0x366e,0x04},
    {0x3670,0x48},
    {0x3671,0xf6},
    {0x3672,0x16},
    {0x3673,0x16},
    {0x367a,0x38},
    {0x367b,0x38},
    {0x3690,0x24},
    {0x3691,0x44},
    {0x3692,0x44},
    {0x3699,0x80},
    {0x369a,0x80},
    {0x369b,0x9f},
    {0x369c,0x38},
    {0x369d,0x38},
    {0x36a2,0x00},
    {0x36a3,0x3f},
    {0x3902,0xc5},
    {0x391e,0x00},
    {0x3933,0x0a},
    {0x3934,0x0c},
    {0x3940,0x64},
    {0x3942,0x02},
    {0x3943,0x15},
    {0x3e00,0x00},
    {0x3e01,0x8c},
    {0x3e02,0x20},
    {0x3e03,0x0b},
    {0x3e06,0x00},
    {0x3e07,0x80},
    {0x3e08,0x03},
    {0x3e09,0x20},
    {0x3e26,0x20},
    {0x3f00,0x0f},
    {0x3f04,0x04},
    {0x3f05,0x28},
    {0x5780,0x7f},
    {0x5781,0x04},
    {0x5782,0x04},
    {0x5783,0x02},
    {0x5784,0x01},
    {0x57a0,0x00},
    {0x57a1,0x72},
    {0x57a2,0x01},
    {0x57a3,0xf2},
    {0x57a4,0x60},
    {0x3d08,0x03},
    {0x0100,0x01},
    {0xffff,0x0a},/////delay 10ms
};


FINE_GAIN fine_again[] = {
//gain map update for 1/32 precision
    {100000, 0x20},
    {103125, 0x21},
    {106250, 0x22},
    {109375, 0x23},
    {112500, 0x24},
    {115625, 0x25},
    {118750, 0x26},
    {121875, 0x27},
    {125000, 0x28},
    {128125, 0x29},
    {131250, 0x2a},
    {134375, 0x2b},
    {137500, 0x2c},
    {140625, 0x2d},
    {143750, 0x2e},
    {146875, 0x2f},
    {150000, 0x30},
    {153125, 0x31},
    {156250, 0x32},
    {159375, 0x33},
    {162500, 0x34},
    {165625, 0x35},
    {168750, 0x36},
    {171875, 0x37},
    {175000, 0x38},
    {178125, 0x39},
    {181250, 0x3a},
    {184375, 0x3b},
    {187500, 0x3c},
    {190625, 0x3d},
    {193750, 0x3e},
    {196875, 0x3f},
};

I2C_ARRAY mirror_reg[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    //{0x3213, 0x04}, // crop for bayer
};

I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x00|0x80},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x20}, //low bit, 0x10 - 0x3e0, step 1/32
};

I2C_ARRAY expo_reg[] = {  // max expo line vts*2-6
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x8c}, // expo[16:8]
    {0x3e02, 0x20}, // expo[7:0], [3:0] fraction of line
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65},
};

I2C_ARRAY nr_reg[] = {
    {0x3630, 0x40},
    {0x3632, 0x88},
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
static int sc2231poweron(ms_cus_sensor *handle, u32 idx)
{

    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //Sensor power on sequence
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);

    sensor_if->MCLK(idx, 1, handle->mclk);
    SENSOR_USLEEP(5000);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_USLEEP(5000);

    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_USLEEP(5000);

    return SUCCESS;
}

static int sc2231poweroff(ms_cus_sensor *handle, u32 idx)
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
static int sc2231GetSensorID(ms_cus_sensor *handle, u32 *id)
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

static int sc2231SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    return SUCCESS;
}

static int sc2231Init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = handle->sensor_if_api;
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

static int sc2231GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}
static int sc2231GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int sc2231GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int sc2231SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = sc2231Init;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int sc2231GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{

    char sen_data;

    sen_data = mirror_reg[0].data;
    SENSOR_DMSG("[%s] mirror:%x\r\n", __FUNCTION__, sen_data & 0x66);
    switch(sen_data)
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

static int sc2231SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{

  sc2231_params *params = (sc2231_params *)handle->private_data;

  switch(orit) {
    case CUS_ORIT_M0F0:
    if (mirror_reg[0].data) {
        mirror_reg[0].data = 0;
        //mirror_reg[1].data = 4;
        params->orient_dirty = true;
    }
    break;
      case CUS_ORIT_M1F0:
    if (mirror_reg[0].data!=6) {
        mirror_reg[0].data = 6;
        //mirror_reg[1].data = 4;
        params->orient_dirty = true;
    }
    break;
    case CUS_ORIT_M0F1:
    if (mirror_reg[0].data!=0x60) {
        mirror_reg[0].data = 0x60;
        //mirror_reg[1].data = 4;
        params->orient_dirty = true;
    }
    break;
    case CUS_ORIT_M1F1:
    if (mirror_reg[0].data!=0x66) {
        mirror_reg[0].data = 0x66;
        //mirror_reg[1].data = 4;
        params->orient_dirty = true;
    }
    break;
  }

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", orit);

  return SUCCESS;
}

static int sc2231GetFPS(ms_cus_sensor *handle)
{
    sc2231_params *params = (sc2231_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (vts_reg[0].data << 8) | (vts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}
static int sc2231SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0;
    sc2231_params *params = (sc2231_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);
    if(fps>=5 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30+(fps>>1))/fps)-1;
    }else if(fps>=5000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts=  ((vts_30fps*30000+(fps>>1))/fps)-1;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > 2*(params->expo.vts) -8){
        vts = (params->expo.line + 9)/2;
    }else{
        vts = params->expo.vts;
    }

    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int sc2231AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc2231_params *params = (sc2231_params *)handle->private_data;


    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //SensorReg_Write(0x3001,0);
            break;
        case CUS_FRAME_ACTIVE:
        if(params->orient_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            params->orient_dirty = false;
        }
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
            SensorReg_Write(0x3812,0x00);
            SensorRegArrayW((I2C_ARRAY*)nr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
            SensorReg_Write(0x3812,0x30);
            params->reg_dirty = false;
        }
        break;
        default :
        break;
    }
    return SUCCESS;
}

static int sc2231GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    lines |= (u32)(expo_reg[0].data&0x000f)<<16;
    lines |= (u32)(expo_reg[1].data&0x00ff)<<8;
    lines |= (u32)(expo_reg[2].data&0x00f0)<<0;
    lines >>= 4;
    *us = (lines*Preview_line_period)/1000/2; //return us

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;
}

static int sc2231SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0, vts = 0;
    sc2231_params *params = (sc2231_params *)handle->private_data;

    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts*2-6
        {0x3e00, 0x00},//expo [20:17]
        {0x3e01, 0x8c}, // expo[16:8]
        {0x3e02, 0x20}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-8) {
        vts=(half_lines + 9)/2;
    } else
        vts=params->expo.vts;
    params->expo.line = half_lines;
    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);
    half_lines = half_lines<<4;
    expo_reg[0].data = (half_lines>>16) & 0x000f;
    expo_reg[1].data = (half_lines>>8) & 0x00ff;
    expo_reg[2].data = (half_lines>>0) & 0x00f0;
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;

    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (expo_reg[i].data != expo_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }

    return SUCCESS;
}

// Gain: 1x = 1024
static int sc2231GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int rc = 0;

    return rc;
}

static int sc2231SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    sc2231_params *params = (sc2231_params *)handle->private_data;

    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx100 = 100;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x20,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, (0x00|0x03)},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));

    if (gain<1024) {
        gain=1024;
    } else if (gain>=SENSOR_MAXGAIN*1024) {
        gain=SENSOR_MAXGAIN*1024;
    }

    if (gain < 256*1024) {
        nr_reg[0].data = 0x40;
        nr_reg[1].data = 0x88;
    }else {
        nr_reg[0].data = 0x4f;
        nr_reg[1].data = 0xc8;
    }

    if (gain < 2 * 1024)
    {
        Dgain = 1;      Fine_dgainx100 = 100;         Coarse_gain = 1;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x3;
    }
    else if (gain <  4 * 1024)
    {
        Dgain = 1;      Fine_dgainx100 = 100;         Coarse_gain = 2;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x7;
    }
    else if (gain <  8 * 1024)
    {
        Dgain = 1;      Fine_dgainx100 = 100;         Coarse_gain = 4;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0xf;
    }
    else if (gain <=  16128)
    {
        Dgain = 1;      Fine_dgainx100 = 100;         Coarse_gain = 8;
        Dgain_reg = 0;  Fine_dgain_reg = 0x80;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <  32256)
    {
        Dgain = 1;      Fine_againx32 = 63;    Coarse_gain = 8;
        Dgain_reg = 0;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }
    else if (gain <=(SENSOR_MAXGAIN * 1024))
    {
        Dgain = 2;      Fine_againx32 = 63;    Coarse_gain = 8;
        Dgain_reg = 1;  Fine_again_reg = 0x3f;  Coarse_gain_reg = 0x1f;
    }


    if (gain <=  16128)
    {
        Fine_again_reg = abs(100 * gain / (Dgain * Coarse_gain * Fine_dgainx100)/32);
    }
    else
    {
        Fine_dgain_reg = abs(4 * gain / (Dgain * Coarse_gain * Fine_againx32));
    }

   // printk("[%s]  gain : %d,%d, %d\n\n", __FUNCTION__,gain,Dgain,Coarse_gain);
   // printk("[%s]  gain_reg : %x ,%x ,%x , %x\n\n", __FUNCTION__,Fine_again_reg,Coarse_gain_reg,Fine_dgain_reg,Dgain_reg);

        gain_reg[3].data = Fine_again_reg;
        gain_reg[2].data = Coarse_gain_reg;
        gain_reg[1].data = Fine_dgain_reg;
        gain_reg[0].data = Dgain_reg & 0xF;
        //SENSOR_DMSG("[%s] set gain =%d %f %f 0x%x ,0x%x, 0x%x ,0x%x\n", __FUNCTION__, gain,Fine_again,Fine_dgain,gain_reg[0].data,gain_reg[1].data,gain_reg[2].data,gain_reg[3].data);
    for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++)
    {
      if (gain_reg[i].data != gain_reg_temp[i].data)
      {
        params->reg_dirty = true;
        break;
      }
    }

    return SUCCESS;
}

static int sc2231GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 30;
  *max = 1000000/Preview_MIN_FPS;

  return SUCCESS;
}

static int sc2231GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
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

static int sc2231GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = (Preview_line_period * 3) / 2;
    info->step = Preview_line_period;
    return SUCCESS;
}

int sc2231ReleaseHandle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

int sc2231InitHandle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc2231_params *params = 0;
    if (!handle)
    {
        SENSOR_EMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }

    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    //private data allocation & init
    handle->private_data = CamOsMemCalloc(1, sizeof(sc2231_params));
    params = (sc2231_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc2231_PARL");

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
    handle->video_res_supported.ulcur_res = 0;
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= 0;
    handle->video_res_supported.res[0].crop_start_y= 0;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");

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
    handle->pCus_sensor_release     = sc2231ReleaseHandle;
    handle->pCus_sensor_init        = sc2231Init;
    handle->pCus_sensor_poweron     = sc2231poweron ;
    handle->pCus_sensor_poweroff    = sc2231poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID     = sc2231GetSensorID;

    handle->pCus_sensor_GetVideoResNum = sc2231GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = sc2231GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = sc2231GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = sc2231SetVideoRes;

    handle->pCus_sensor_GetOrien        = sc2231GetOrien;
    handle->pCus_sensor_SetOrien        = sc2231SetOrien;
    handle->pCus_sensor_GetFPS          = sc2231GetFPS;
    handle->pCus_sensor_SetFPS          = sc2231SetFPS;
    handle->pCus_sensor_SetPatternMode  = sc2231SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = sc2231AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = sc2231GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = sc2231SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = sc2231GetAEGain;
    handle->pCus_sensor_SetAEGain       = sc2231SetAEGain;
    handle->pCus_sensor_SetAEGain_cal   = sc2231SetAEGain;
    handle->pCus_sensor_GetShutterInfo  = sc2231GetShutterInfo;

    handle->pCus_sensor_GetAEMinMaxGain = sc2231GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= sc2231GetAEMinMaxUSecs;

    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line= 1000;
    params->reg_dirty = false;
    params->orient_dirty = false;

    //handle->snr_pad_group = SENSOR_PAD_GROUP_SET;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX( SC2231,
                            sc2231InitHandle,
                            NULL,
                            NULL,
                            sc2231_params
                         );

