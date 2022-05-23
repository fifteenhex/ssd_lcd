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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC3235);

#define SENSOR_ISP_TYPE     ISP_EXT
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_PARL      //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_DATAPREC     CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000

#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAXGAIN     (4284*315 / 1000)                  // max sensor again, a-gain * conversion-gain*d-gain

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ            //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 29630                   //2640*1125*25
#define Prv_Max_line_number 1294                    //maximum exposure line munber of sensor when preview
#define vts_30fps           1350                    //for 30 fps
#define Preview_WIDTH       2304                    //resolution Width when preview
#define Preview_HEIGHT      1296                   //resolution Height when preview
#define Preview_MAX_FPS     30                      //fastest preview FPS
#define Preview_MIN_FPS     3                       //slowest preview FPS


#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8
#define SENSOR_I2C_ADDR    0x60                    //I2C slave address
#define SENSOR_I2C_SPEED   200000                  //200KHz

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG         //if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG         //if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG             //CUS_CLK_POL_NEG //CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS             // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
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
        u32 line;
    } expo;
    I2C_ARRAY tVts_reg[2];
    I2C_ARRAY tGain_reg[4];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tMirror_reg[1];
    I2C_ARRAY tNr_reg[3];
    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_dirty; //sensor setting need to update through I2C
    bool orient_dirty;
    //pthread_mutex_t rw_lock;
} sc3235_params;

const I2C_ARRAY Sensor_id_table[] =
{
    {0x3107, 0x23},
    {0x3108, 0x11},
};

const I2C_ARRAY Sensor_init_table[] =     // new_SC3235_12bit_DVP_74.25Mpclk_30fps
{
    {0x0103,0x01},
    {0x0100,0x00},
    {0x3641,0x0c},
    {0x36e9,0x07},
    {0x36eb,0x05},
    {0x36ec,0x15},
    {0x36ed,0x04},
    {0x36f9,0x33},
    {0x36fb,0x23},
    {0x36fc,0x01},
    {0x36fd,0x14},
    {0x3641,0x00},
    {0x3e09,0x20},
    {0x3637,0x2c},
    {0x3630,0x83},
    {0x3635,0x10},
    {0x363b,0x10},
    {0x363c,0x07},
    {0x3306,0x58},
    {0x330a,0x00},
    {0x330b,0xd8},
    {0x3638,0x28},
    {0x331c,0x01},
    {0x3304,0x30},
    {0x331e,0x29},
    {0x3320,0x03},
    {0x3356,0x01},
    {0x57a4,0xa0},
    {0x5781,0x04},
    {0x5782,0x04},
    {0x5783,0x02},
    {0x5784,0x02},
    {0x5785,0x40},
    {0x5786,0x20},
    {0x5787,0x18},
    {0x5788,0x10},
    {0x5789,0x10},
    {0x578a,0x30},
    {0x3908,0x41},
    {0x3622,0xf6},
    {0x3e25,0x03},
    {0x3e26,0x20},
    {0x3902,0xc5},
    {0x3905,0x99},
    {0x3314,0x94},
    {0x3347,0x05},
    {0x3301,0x80},
    {0x3630,0xc3},
    {0x3633,0x42},
    {0x363a,0xa8},
    {0x3614,0x80},
    {0x3632,0x18},
    {0x3631,0x8a},
    {0x3e01,0x62},
    {0x363b,0x20},
    {0x3635,0x20},
    {0x3314,0x94},
    {0x330e,0x30},
    {0x3367,0x10},
    {0x3368,0x04},
    {0x3369,0x00},
    {0x336a,0x00},
    {0x336b,0x00},
    {0x334c,0x10},
    {0x3633,0x44},
    {0x3622,0xf6},
    {0x3301,0x11},
    {0x3630,0xc3},
    {0x3632,0x18},
    {0x3306,0x50},
    {0x330b,0xd0},
    {0x360f,0x05},
    {0x367a,0x08},
    {0x367b,0x38},
    {0x3671,0xf6},
    {0x3672,0x76},
    {0x3673,0x16},
    {0x3670,0x08},
    {0x369c,0x08},
    {0x369d,0x38},
    {0x3690,0x64},
    {0x3691,0x63},
    {0x3692,0x64},
    {0x3670,0x0a},
    {0x367c,0x08},
    {0x367d,0x38},
    {0x3674,0xa0},
    {0x3675,0x98},
    {0x3676,0x6a},
    {0x3364,0x17},
    {0x3301,0x11},
    {0x3393,0x1a},
    {0x3394,0x88},
    {0x3395,0x88},
    {0x3390,0x08},
    {0x3391,0x38},
    {0x3392,0x38},
    {0x301f,0x02},
    {0x3933,0x0a},
    {0x3934,0x28},
    {0x3942,0x02},
    {0x3943,0x33},
    {0x3940,0x68},
    {0x3e1b,0x35},
    {0x3038,0x66},
    {0x363c,0x06},
    {0x3253,0x08},
    {0x391d,0x04},
    {0x391e,0x00},
    {0x3905,0xd1},
    {0x3905,0xd1},
    {0x3909,0x00},
    {0x390a,0x19},
    {0x390d,0x00},
    {0x390e,0x19},
    {0x390b,0x00},
    {0x390c,0x4c},
    {0x390f,0x00},
    {0x3910,0x4c},
    {0x3920,0x00},
    {0x3921,0x4c},
    {0x3924,0x00},
    {0x3925,0x4c},
    {0x3922,0x00},
    {0x3923,0x19},
    {0x3926,0x00},
    {0x3927,0x19},
    {0x3900,0x29},
    {0x3937,0x13},
    {0x3906,0x62},
    {0x3935,0x18},
    {0x3936,0x08},
    {0x391e,0x00},
    {0x3e01,0xa8},
    {0x3e02,0x40},
    {0x3632,0x18},
    {0x3306,0x50},
    {0x330b,0xd0},
    {0x3000,0x0f},
    {0x3001,0xff},
    {0x3002,0xf0},
    {0x301c,0x94},
    {0x3018,0x0f},
    {0x301a,0xf8},

	{0x3d08,0x03},

    {0x4603,0x01},
    {0x303f,0x81},
    {0x300a,0x24},
    {0x3641,0x02},
	{0x0100,0x01},

    {0xffff,0x0a},/////delay 10ms
};

const I2C_ARRAY mirror_reg[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
    //{0x3213, 0x08}, // crop for bayer
};

const I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x00|0x80},
    {0x3e08, 0x00|0x03},
    {0x3e09, 0x20}, //low bit, 0x10 - 0x3e0, step 1/32
};

const I2C_ARRAY expo_reg[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x8c}, // expo[16:8]
    {0x3e02, 0x40}, // expo[7:0], [3:0] fraction of line
};

const I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65},
};

const I2C_ARRAY nr_reg[] = {
    {0x3632,0x18},
    {0x3306,0x50},  //[42,]
    {0x330b,0xd0},  //[c8,df]
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
static int sc3235poweron(ms_cus_sensor *handle, u32 idx)
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

static int sc3235poweroff(ms_cus_sensor *handle, u32 idx)
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
static int sc3235GetSensorID(ms_cus_sensor *handle, u32 *id)
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

static int sc3235SetPatternMode(ms_cus_sensor *handle,u32 mode)
{

    return SUCCESS;
}

static int sc3235Init(ms_cus_sensor *handle)
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

static int sc3235GetVideoResNum( ms_cus_sensor *handle, u32 *ulres_num)
{
    *ulres_num = handle->video_res_supported.num_res;
    return SUCCESS;
}
static int sc3235GetVideoRes(ms_cus_sensor *handle, u32 res_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[res_idx];

    return SUCCESS;
}

static int sc3235GetCurVideoRes(ms_cus_sensor *handle, u32 *cur_idx, cus_camsensor_res **res)
{
    u32 num_res = handle->video_res_supported.num_res;

    *cur_idx = handle->video_res_supported.ulcur_res;

    if (*cur_idx >= num_res) {
        return FAIL;
    }

    *res = &handle->video_res_supported.res[*cur_idx];

    return SUCCESS;
}

static int sc3235SetVideoRes(ms_cus_sensor *handle, u32 res_idx)
{
    u32 num_res = handle->video_res_supported.num_res;

    if (res_idx >= num_res) {
        return FAIL;
    }
    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = sc3235Init;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int sc3235GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    char sen_data;
    sc3235_params *params = (sc3235_params *)handle->private_data;
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

static int sc3235SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{

  sc3235_params *params = (sc3235_params *)handle->private_data;

  switch(orit) {
    case CUS_ORIT_M0F0:
      if (params->tMirror_reg[0].data) {
          params->tMirror_reg[0].data = 0;
          //params->tMirror_reg[1].data = 8;
          params->orient_dirty = true;
      }
            break;
        case CUS_ORIT_M1F0:
      if (params->tMirror_reg[0].data!=6) {
          params->tMirror_reg[0].data = 6;
          //params->tMirror_reg[1].data = 8;
          params->orient_dirty = true;
      }
            break;
        case CUS_ORIT_M0F1:
      if (params->tMirror_reg[0].data!=0x60) {
          params->tMirror_reg[0].data = 0x60;
          //params->tMirror_reg[1].data = 8;
          params->orient_dirty = true;
      }
            break;
        case CUS_ORIT_M1F1:
      if (params->tMirror_reg[0].data!=0x66) {
          params->tMirror_reg[0].data = 0x66;
          //params->tMirror_reg[1].data = 8;
          params->orient_dirty = true;
      }
      break;
  }

  SENSOR_DMSG("pCus_SetOrien:%x\r\n", orit);

  return SUCCESS;
}

static int sc3235GetFPS(ms_cus_sensor *handle)
{
    sc3235_params *params = (sc3235_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 5000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int sc3235SetFPS(ms_cus_sensor *handle, u32 fps)
{
    int vts = 0;
    sc3235_params *params = (sc3235_params *)handle->private_data;
    SENSOR_DMSG("\n\n ****************  [%s], fps=%d  **************** \n", __FUNCTION__, fps);
    if(fps>=5 && fps <= 30) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30)/fps;
    }else if(fps>=5000 && fps <= 30000) {
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*30000)/fps;
    }else{
        SENSOR_DMSG("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > params->expo.vts -4){
        vts = (params->expo.line + 5)/2;
    }else{
        vts = params->expo.vts;
    }
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int sc3235AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc3235_params *params = (sc3235_params *)handle->private_data;


    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //SensorReg_Write(0x3001,0);
            break;
        case CUS_FRAME_ACTIVE:
        if(params->orient_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tMirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            params->orient_dirty = false;
        }
        if(params->reg_dirty)
        {
            SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
#if ENABLE_NR
            SensorReg_Write(0x3812,0x00);
            SensorRegArrayW((I2C_ARRAY*)params->tNr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
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

static int sc3235GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    int rc=0;
    u32 lines = 0;
    sc3235_params *params = (sc3235_params *)handle->private_data;
    lines |= (u32)(params->tExpo_reg[0].data&0x0f)<<16;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[2].data&0xf0)<<0;
    lines >>= 4;
    *us = (lines*Preview_line_period)/1000/2; //return us

    SENSOR_DMSG("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
    return rc;
}

static int sc3235SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    int i;
    u32 half_lines = 0, vts = 0;
    sc3235_params *params = (sc3235_params *)handle->private_data;

    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x8c}, // expo[16:8]
    {0x3e02, 0x40}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, params->tExpo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<3) half_lines=3;
    if (half_lines >  2 * (params->expo.vts)-4) {
        vts = (half_lines+5)/2;
    }
    else
        vts=params->expo.vts;

    params->expo.line = half_lines;

    SENSOR_DMSG("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);
    half_lines = half_lines<<4;
    params->tExpo_reg[0].data = (half_lines>>16) & 0x0f;
    params->tExpo_reg[1].data = (half_lines>>8) & 0xff;
    params->tExpo_reg[2].data = (half_lines>>0) & 0xf0;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;


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
static int sc3235GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    int rc = 0;

    return rc;
}

static int sc3235SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    sc3235_params *params = (sc3235_params *)handle->private_data;

    u8 i=0 ,Dgain = 1,  Coarse_gain = 1;
    u32 Fine_againx32 = 32,Fine_dgainx100 = 100;
    u8 Dgain_reg = 0, Coarse_gain_reg = 0, Fine_again_reg= 0x10,Fine_dgain_reg= 0x80;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x00|0x80},
        {0x3e08, 0x00|0x03},
        {0x3e09, 0x20},
    };
    memcpy(gain_reg_temp, params->tGain_reg, sizeof(gain_reg));


    if (gain<1024) {
        gain=1024;
    } else if (gain>=SENSOR_MAXGAIN*1024) {
        gain=SENSOR_MAXGAIN*1024;
    }

#if ENABLE_NR
    if (gain < 2*1024) {
        params->tNr_reg[0].data = 0x18;
        params->tNr_reg[1].data = 0x50;
        params->tNr_reg[2].data = 0xd0;
    } else if (gain < 8*1024) {
        params->tNr_reg[0].data = 0x58;
        params->tNr_reg[1].data = 0x58;
        params->tNr_reg[2].data = 0xd8;
    } else {
        params->tNr_reg[0].data = 0xd8;
        params->tNr_reg[1].data = 0x5c;
        params->tNr_reg[2].data = 0xdb;
    }
#endif

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
        //Fine_againx32 =gain*100000 / ((Dgain * Coarse_gain * Fine_dgainx100*1024)/ 100000);
        Fine_again_reg = abs(100 * gain / (Dgain * Coarse_gain * Fine_dgainx100)/32);
    }
    else
    {
        //Fine_dgainx100 =gain*100000 / (Dgain * Coarse_gain * Fine_againx32)*100000/ 1024;
        Fine_dgain_reg = abs(4 * gain / (Dgain * Coarse_gain * Fine_againx32));
    }

    SENSOR_DMSG("[%s]  gain : %d,%d\n", __FUNCTION__,DIG_gain,DIG_Fine_gainx1000);
    SENSOR_DMSG("[%s]  reg : 0x%x ,0x%x\n", __FUNCTION__,DIG_gain_reg,DIG_Fine_gain_reg);

    params->tGain_reg[3].data = Fine_again_reg;
    params->tGain_reg[2].data = Coarse_gain_reg;
    params->tGain_reg[1].data = Fine_dgain_reg;
    params->tGain_reg[0].data = Dgain_reg;

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

static int sc3235GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
  *min = 30;
  *max = 1000000/Preview_MIN_FPS;

  return SUCCESS;
}

static int sc3235GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
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

static int sc3235GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 1;
    info->step = Preview_line_period;
    return SUCCESS;
}

int sc3235ReleaseHandle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

int sc3235InitHandle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    sc3235_params *params = 0;
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
    params = (sc3235_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->tMirror_reg, mirror_reg, sizeof(mirror_reg));
    memcpy(params->tNr_reg, nr_reg, sizeof(nr_reg));
    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"sc3235_PARL");

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
    handle->video_res_supported.res[0].nOutputWidth= Preview_WIDTH;
    handle->video_res_supported.res[0].nOutputHeight= Preview_HEIGHT;
    sprintf(handle->video_res_supported.res[0].strResDesc, "2304x1296@25fps");

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
    handle->pCus_sensor_release     = sc3235ReleaseHandle;
    handle->pCus_sensor_init        = sc3235Init;
    handle->pCus_sensor_poweron     = sc3235poweron ;
    handle->pCus_sensor_poweroff    = sc3235poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID     = sc3235GetSensorID;

    handle->pCus_sensor_GetVideoResNum = sc3235GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = sc3235GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = sc3235GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = sc3235SetVideoRes;

    handle->pCus_sensor_GetOrien        = sc3235GetOrien;
    handle->pCus_sensor_SetOrien        = sc3235SetOrien;
    handle->pCus_sensor_GetFPS          = sc3235GetFPS;
    handle->pCus_sensor_SetFPS          = sc3235SetFPS;
    handle->pCus_sensor_SetPatternMode  = sc3235SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = sc3235AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = sc3235GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = sc3235SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = sc3235GetAEGain;
    handle->pCus_sensor_SetAEGain       = sc3235SetAEGain;
    handle->pCus_sensor_SetAEGain_cal   = sc3235SetAEGain;
    handle->pCus_sensor_GetShutterInfo  = sc3235GetShutterInfo;

    handle->pCus_sensor_GetAEMinMaxGain = sc3235GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= sc3235GetAEMinMaxUSecs;

    handle->pCus_sensor_setCaliData_gain_linearity=pCus_setCaliData_gain_linearity;
    params->expo.vts=vts_30fps;
    params->expo.line=1000;
    params->expo.fps = 30;
    params->reg_dirty = false;
    params->orient_dirty = false;

    //handle->snr_pad_group = SENSOR_PAD_GROUP_SET;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX( SC3235,
                            sc3235InitHandle,
                            NULL,
                            NULL,
                            sc3235_params
                         );

