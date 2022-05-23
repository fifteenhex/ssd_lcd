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
#include <drv_sensor_init_table.h>
#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(SC210iot);

#define SENSOR_CHANNEL_NUM (0)
#define SENSOR_CHANNEL_MODE_LINEAR CUS_SENSOR_CHANNEL_MODE_REALTIME_NORMAL
//#define SENSOR_CHANNEL_MODE_SONY_DOL CUS_SENSOR_CHANNEL_MODE_RAW_STORE_HDR
#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif
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
#define SENSOR_DBG 1

#define SENSOR_ISP_TYPE     ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI     //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_10    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000
#define SENSOR_BAYERID      CUS_BAYER_BG            //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAXGAIN      (15875*31)/1000       //  (15.5*31)   max sensor gain, a-gain*conversion-gain*d-gain
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ        //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
//#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec @MCLK=36MHz
//#define vts_30fps 1125//1346,1616                 //for 29.1fps @ MCLK=36MHz
//#define Line_per_second     32727
//#define vts_30fps
#define Preview_line_period 29630         //1/30/1500 // 每一行的曝光时间，单位是: ns. 1 s = 1000 000 000 ns
#define vts_30fps           1125                //for 30 fps

#define Preview_WIDTH       1920                   //resolution Width when preview
#define Preview_HEIGHT      1080                  //resolution Height when preview
#define Preview_MAX_FPS     30  //25                     //fastest preview FPS
#define Preview_MIN_FPS     3                       //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

// #define SENSOR_I2C_ADDR     0x60                   //I2C slave address
#define SENSOR_I2C_ADDR     0x64                   //I2C slave address
#define SENSOR_I2C_SPEED    240000                  //I2C speed,60000~320000

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG            // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
#define ENABLE_NR 1

#if defined (SENSOR_MODULE_VERSION)
#define TO_STR_NATIVE(e) #e
#define TO_STR_PROXY(m, e) m(e)
#define MACRO_TO_STRING(e) TO_STR_PROXY(TO_STR_NATIVE, e)
static char *sensor_module_version = MACRO_TO_STRING(SENSOR_MODULE_VERSION);
module_param(sensor_module_version, charp, S_IRUGO);
#endif

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit);
static int g_sensor_ae_min_gain = 1024;

CUS_MCLK_FREQ UseParaMclk(void);

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
        u32 half_lines;
        u32 half_line;
        u32 fps;
        u32 line;
    } expo;
    struct {
        bool bVideoMode;
        u16 res_idx;
        //        bool binning;
        //        bool scaling;
        CUS_CAMSENSOR_ORIT  orit;
    } res;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool reg_mf;
    bool reg_dirty;
    bool nr_dirty;
    bool temperature_reg_1_dirty;
//    bool temperature_reg_2_dirty;
    CUS_CAMSENSOR_ORIT cur_orien;
} sc2335_params;
// set sensor ID address and data,

typedef struct {
    u64 gain;
    u8 fine_gain_reg;
} FINE_GAIN;

I2C_ARRAY Sensor_id_table[] = {
    {0x3107, 0xCB},
    {0x3108, 0x17}, // 0x07
};

I2C_ARRAY Sensor_init_table_2M30fps[] = {
    // printf("===0x01_FT_SC200IoT_MIPI_27MIput_2lane_371.25Mbps_10bit_1920x1080_30fps\n");
	{0x0103,0x01},
	//{0xffff,  50},
	{0x0100,0x00},
	{0x36e9,0x80},
	{0x36f9,0x80},
	{0x3622,0x06},
	{0x3630,0xc8},
	{0x3637,0x15},
	{0x3638,0x46},
	{0x3253,0x0c},
	{0x3333,0x10},
	{0x3304,0x68},
	{0x331e,0x61},
	{0x3306,0x40},
	{0x330b,0xc6},
	{0x3633,0x43},
	{0x3634,0x54},
	{0x3631,0x84},
	{0x3301,0x06},
	{0x363a,0x1f},
	{0x36f9,0xa4},
	{0x36fa,0x35},
	{0x36fc,0x11},
	{0x36fd,0x14},
	{0x36f9,0x24},
	{0x3364,0x17},
	{0x3390,0x08},
	{0x3391,0x18},
	{0x3392,0x38},
	{0x3301,0x06},
	{0x3393,0x08},
	{0x3394,0x0a},
	{0x3395,0x50},
	{0x3670,0x08},
	{0x369c,0x08},
	{0x369d,0x38},
	{0x3690,0x53},
	{0x3691,0x63},
	{0x3692,0x63},
	{0x3670,0x0c},
	{0x367e,0x08},
	{0x367f,0x38},
	{0x3677,0x84},
	{0x3678,0x84},
	{0x3679,0x88},
	{0x36e9,0x20},
	{0x36f9,0x24},
	{0x0100,0x01},
	{0x3802,0x00}, //sync
};

I2C_ARRAY mirror_reg[] = {
    {0x3221, 0x00}, // mirror[2:1], flip[6:5]
};

typedef struct {
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

static I2C_ARRAY gain_reg[] = {
    {0x3e06, 0x00},
    {0x3e07, 0x80}, // 128
    {0x3e08, 0x03},
    {0x3e09, 0x20}, // 32
};

I2C_ARRAY expo_reg[] = {
    {0x3e00, 0x00}, //expo [20:17]
    {0x3e01, 0x8c}, // expo[15:8]
    {0x3e02, 0x60}, // expo[7:0], [3:0] fraction of line
};

I2C_ARRAY vts_reg[] = {
    {0x320e, 0x04},
    {0x320f, 0x65},
};

#if 0 // ENABLE_NR
I2C_ARRAY nr_reg[] = {
    {0x363c, 0x0e},
};

I2C_ARRAY temperature_reg_1[] = {
    {0x5787, 0x00},
    {0x5788, 0x00},
    {0x5790, 0x00},
    {0x5791, 0x00},
    {0x5799, 0x07},
};
#endif

// I2C_ARRAY temperature_reg_2[] = {
    // {0x3637, 0x20},
    // {0x391f, 0x18},
    // {0x3908, 0x82},
// };

I2C_ARRAY PatternTbl[] = {
    {0x4501,0xa4}, //testpattern , bit 3 to enable
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

/////////// function definition ///////////////////
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) CamOsPrintf(args)
//#define SENSOR_DMSG(args...) LOGE(args)
//#define SENSOR_DMSG(args...) printk(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif
#undef SENSOR_NAME
#define SENSOR_NAME ov2710



#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
int cus_camsensor_release_handle(ms_cus_sensor *handle);

/////////////////// sensor hardware dependent //////////////
#if 0
static int ISP_config_io(ms_cus_sensor *handle) {
    ISensorIfAPI *sensor_if = &handle->sensor_if_api;

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
    int res = 0;
    CamOsPrintf("[%s] ", __FUNCTION__);
    res = sensor_if->PowerOff(idx, handle->pwdn_POLARITY);////pwd low
    res = sensor_if->Reset(idx, handle->reset_POLARITY);
    if(res>=0)//if success
    CamOsMsSleep(1);
    //Sensor power on sequence
    res = sensor_if->MCLK(idx, 1, handle->mclk);

    res = sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    //sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    //sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    //sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);
    if(res>=0)//if success
    CamOsMsSleep(2);
    res = sensor_if->Reset(idx, !handle->reset_POLARITY);
    if(res>=0)//if success
    CamOsMsSleep(1);

    SENSOR_DMSG("[%s] pwd high\n", __FUNCTION__);
    res = sensor_if->PowerOff(idx, !handle->reset_POLARITY);
    if(res>=0)//if success
    CamOsMsSleep(2);

    //sensor_if->Set3ATaskOrder(handle, def_order);
    // pure power on
    //ISP_config_io(handle);
   // sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
   // CamOsMsSleep(5);
    //handle->i2c_bus->i2c_open(handle->i2c_bus,&handle->i2c_cfg);

    //sensor_if->SetCSI_Clk(0, CUS_CSI_CLK_216M);
    //sensor_if->SetCSI_Lane(0, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    //sensor_if->SetCSI_LongPacketType(0, 0, 0x1C00, 0);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc2335_params *params = (sc2335_params *)handle->private_data;
    CamOsPrintf("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    //handle->i2c_bus->i2c_close(handle->i2c_bus);
    CamOsMsSleep(1);
    //Set_csi_if(0, 0);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    params->cur_orien = CUS_ORIT_M0F0;

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

  CamOsPrintf("\n\n[%s]", __FUNCTION__);
  for(n=0;n<table_length;++n) {
    id_from_sensor[n].reg = Sensor_id_table[n].reg;
    id_from_sensor[n].data = 0;
  }

  *id =0;
  if(table_length>8) table_length=8;
  for(n=0; n<4; ++n) {              //retry , until I2C success
    if(n>2) return FAIL;
    if(SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
        break;
    else
        CamOsMsSleep(1);
  }

    for(i=0; i<table_length; ++i) {
    if (id_from_sensor[i].data != Sensor_id_table[i].data)
            return FAIL;
    *id = ((*id)+ id_from_sensor[i].data)<<8;
  }
  *id >>= 8;
    CamOsPrintf("[%s]Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int sc2335_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
  int i;
  CamOsPrintf("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

  switch(mode) {
  case 1:
    PatternTbl[0].data = 0xac; //enable
    break;
  case 0:
    PatternTbl[0].data = 0xa4; //disable
    break;
  default:
    PatternTbl[0].data = 0xa4; //disable
    break;
  }
  for(i=0; i< ARRAY_SIZE(PatternTbl); i++) {
      if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
            return FAIL;
  }

  return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);

// static u16 reg3040;

static int pCus_init(ms_cus_sensor *handle)
{
	//ISensorIfAPI *sensor_if = handle->sensor_if_api;
    sc2335_params *params = (sc2335_params *)handle->private_data;
    int i,cnt;

    CamOsPrintf("wink[%s]\n", __FUNCTION__);

	// r/g = 128/reg5e24, b/g = 128/reg5e25;

	//logic
	// SensorReg_Read(0x3040, &reg3040);

    //sensor_if->PCLK(NULL,CUS_PCLK_MIPI_TOP);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table_2M30fps);i++)
    {
   
        if(Sensor_init_table_2M30fps[i].reg==0x0100 && 0x01 == Sensor_init_table_2M30fps[i].data)
        {
			//platform dependent
			//SensorReg_Write(0x3802, 0x00);  //sync
            //sensor_if->SetCSI_Clk(0, CUS_CSI_CLK_216M);
            //sensor_if->SetCSI_Lane(0, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
            //sensor_if->SetCSI_LongPacketType(0, 0, 0x1C00, 0);
            //SENSOR_MSLEEP(50);
        }

        if(Sensor_init_table_2M30fps[i].reg==0xffff)
        {
            //CamOsPrintf("wink[111111%s %d]\n", __FUNCTION__,__LINE__);
            ;
           // SENSOR_MSLEEP(Sensor_init_table_2M30fps[i].data);
        }
        else
            
        {
            cnt = 0;
            while(SensorReg_Write(Sensor_init_table_2M30fps[i].reg, Sensor_init_table_2M30fps[i].data) != SUCCESS)
            {
                cnt++;
                //CamOsPrintf("Sensor_init_table -> Retry %d...\n",cnt);
                if(cnt>=3)
                {
                    //CamOsPrintf("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                    return FAIL;
                }
                SENSOR_MSLEEP(1);
            }
        }
    }
#if 0
	if(reg3040 == 0x40) {
		SensorReg_Write(0x363c, 0x0e);
	} else {
		SensorReg_Write(0x363c, 0x0f);
	}
#endif
    //CamOsPrintf("wink[111111%s %d]\n", __FUNCTION__,__LINE__);
    pCus_SetOrien(handle, params->cur_orien);
   //CamOsPrintf("cur_orien %s pCus_SetOrien %x\n",__FUNCTION__, params->cur_orien);
    vts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
    vts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
// quick code end
// ===================================================================

    // usleep(50*1000);
    //pCus_SetAEGain(handle,1024);
    //pCus_SetAEUSecs(handle, 40000);
    //pCus_AEStatusNotify(handle,CUS_FRAME_ACTIVE);
    return SUCCESS;
}


//int (*pCus_sensor_post_init)(struct __ms_cus_sensor* handle, u32 idx)
int pCus_sensor_post_init(struct __ms_cus_sensor* handle, u32 idx)
{
    int i;
    u16 reg5e2e;//
    u16 reg3e01, reg3e02, reg3e08, reg3e09, reg320e, reg320f; // reg5e24, reg5e25;
    u16 reg3e013e02, reg3e083e09, reg3e013e02_x_reg3e083e09_div16, reg320e320f, reg_gain_2;

    //sc2335_params *params = (sc2335_params *)handle->private_data;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    // ===================================================================
    // quick code start
    //CamOsPrintf("wink[111111%s %d]\n", __FUNCTION__,__LINE__);

    sensor_if->SetCSI_Clk(0, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(0, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(0, 0, 0x1C00, 0);

    SensorReg_Read(0x5e2e, &reg5e2e);
    while (0x1 != (reg5e2e & 0x1))
    {
        // delay
        for(i=0; i < 10; i++)
            ;

        SensorReg_Read(0x5e2e, &reg5e2e);
    }
            CamOsPrintf("[wink]Sensor_reg5e2e %d...\n",reg5e2e);
    SensorReg_Read(0x3e01, &reg3e01);
    SensorReg_Read(0x3e02, &reg3e02);
    reg3e013e02 = ((reg3e01 << 4) | ((reg3e02 & 0xf0) >> 4)); // exp1

    SensorReg_Read(0x3e08, &reg3e08);
    SensorReg_Read(0x3e09, &reg3e09);
    reg3e083e09 = ((reg3e08 << 8) | reg3e09); // gain0

    SensorReg_Read(0x320e, &reg320e);
    SensorReg_Read(0x320f, &reg320f);
    reg320e320f = ((reg320e << 8) | reg320f);

    reg3e013e02_x_reg3e083e09_div16 = reg3e013e02 * reg3e083e09 >> 4;
    if (reg3e013e02_x_reg3e083e09_div16 < (2*reg320e320f - 8))
    {
        SensorReg_Write(0x3812, 0x00);
        SensorReg_Write(0x0100, 0x00);
        SensorReg_Write(0x5012, 0x01);
        SensorReg_Write(0x3620, 0x88);
        SensorReg_Write(0x301C, 0x78);
        SensorReg_Write(0x3208, 0x07);
        SensorReg_Write(0x3209, 0x80);
        SensorReg_Write(0x320a, 0x04);
        SensorReg_Write(0x320b, 0x38);
        SensorReg_Write(0x320e, 0x04);
        SensorReg_Write(0x320f, 0x65);
        SensorReg_Write(0x3214, 0x11);
        SensorReg_Write(0x3215, 0x11);
        SensorReg_Write(0x3274, 0x09);
        SensorReg_Write(0x3e1b, 0x15);
        SensorReg_Write(0x3e03, 0x0b);
        SensorReg_Write(0x3e01, ((reg3e013e02_x_reg3e083e09_div16 >> 4) & 0xff));
        SensorReg_Write(0x3e02, ((reg3e013e02_x_reg3e083e09_div16 << 4) & 0xf0));
        SensorReg_Write(0x3e06, 0x00);
        SensorReg_Write(0x3e07, 0x80);
        SensorReg_Write(0x3e08, 0x03);
        SensorReg_Write(0x3e09, 0x20);
        SensorReg_Write(0x0100, 0x01);
        SensorReg_Write(0x3812, 0x30);
    }
    else
    {
        // reg_gain_2 = reg3e083e09 / (((2 * reg320e320f) - 8) / reg3e013e02);
        reg_gain_2 = reg3e013e02_x_reg3e083e09_div16 / (2 * reg320e320f - 8) ;
        SensorReg_Write(0x3812, 0x00);
        SensorReg_Write(0x0100, 0x00);
        SensorReg_Write(0x5012, 0x01);
        SensorReg_Write(0x3620, 0x88);
        SensorReg_Write(0x301C, 0x78);
        SensorReg_Write(0x3208, 0x07);
        SensorReg_Write(0x3209, 0x80);
        SensorReg_Write(0x320a, 0x04);
        SensorReg_Write(0x320b, 0x38);
        SensorReg_Write(0x320e, 0x04);
        SensorReg_Write(0x320f, 0x65);
        SensorReg_Write(0x3214, 0x11);
        SensorReg_Write(0x3215, 0x11);
        SensorReg_Write(0x3274, 0x09);
        SensorReg_Write(0x3e1b, 0x15);
        SensorReg_Write(0x3e03, 0x0b);
        SensorReg_Write(0x3e01, ((reg3e083e09 >> 4) & 0xff));
        SensorReg_Write(0x3e02, ((reg3e083e09 << 4) & 0xf0));
        SensorReg_Write(0x3e06, 0x00);
        SensorReg_Write(0x3e07, 0x80);
        SensorReg_Write(0x3e08, ((reg_gain_2 >> 8) & 0xff));
        SensorReg_Write(0x3e09, (reg_gain_2 & 0xff));
        SensorReg_Write(0x0100, 0x01);
        SensorReg_Write(0x3812, 0x30);
    }
    return 0;
}

/*
int pCus_release(ms_cus_sensor *handle)
{
    ISensorIfAPI *sensor_if = &handle->sensor_if_api;
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
    CamOsPrintf("wink[%s] res_idx :%d num=%d \n", __FUNCTION__,res_idx,handle->video_res_supported.num_res);
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

    CamOsPrintf("wink[%s] num=%d\n", __FUNCTION__,handle->video_res_supported.num_res);

    switch (res_idx) {
        case 0: //"1920x1080@30fps"
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            break;

        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit) {
    char sen_data;
    sen_data = mirror_reg[0].data;
    CamOsPrintf("mirror:%x\r\n", sen_data);
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

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
   sc2335_params *params = (sc2335_params *)handle->private_data;
  //  pr_info("Connect %s table_length %d\n",__FUNCTION__, table_length);
    CamOsPrintf("\n\n[%s]", __FUNCTION__);
    switch(orit)
    {
        case CUS_ORIT_M0F0:
            //SensorReg_Write(mirr_flip_table[0].reg,mirr_flip_table[0].data);
            mirror_reg[0].data = 0x00;
            //mirror_reg[1].data = 0x01;
            params->cur_orien = CUS_ORIT_M0F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F0:
            //SensorReg_Write(mirr_flip_table[1].reg,mirr_flip_table[1].data);
            mirror_reg[0].data = 0x06;
            //mirror_reg[1].data = 0x05;
            params->cur_orien = CUS_ORIT_M1F0;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M0F1:
            //SensorReg_Write(mirr_flip_table[2].reg,mirr_flip_table[2].data);
            mirror_reg[0].data = 0x60;
            //mirror_reg[1].data = 0x01;
            params->cur_orien = CUS_ORIT_M0F1;
            params->reg_mf = true;
            break;
        case CUS_ORIT_M1F1:
            //SensorReg_Write(mirr_flip_table[i].reg,mirr_flip_table[i].data);
            mirror_reg[0].data = 0x66;
            //mirror_reg[1].data = 0x05;
            params->cur_orien = CUS_ORIT_M1F1;
            params->reg_mf = true;
            break;
        default :
            break;
    }
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;
    //CamOsPrintf("[%s]", __FUNCTION__);

    return  params->expo.fps;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    u32 vts=0;
    sc2335_params *params = (sc2335_params *)handle->private_data;
    u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 min_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].min_fps;

    if(fps>=min_fps && fps <= max_fps){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*max_fps)/fps;
    }else if((fps >= (min_fps*1000)) && (fps <= (max_fps*1000))){
        params->expo.fps = fps;
        params->expo.vts=  (vts_30fps*(max_fps*1000))/fps;
    }else{
        CamOsPrintf("[%s] FPS %d out of range.\n",__FUNCTION__,fps);
        return FAIL;
    }

    if(params->expo.line > 2* (params->expo.vts) - 8){
        vts = (params->expo.line + 9)/2;
    }else{
        vts = params->expo.vts;
    }

    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;
    params->reg_dirty = true;
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

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    sc2335_params *params = (sc2335_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
        break;
        case CUS_FRAME_ACTIVE:
         if(params->reg_mf) {
            SensorRegArrayW((I2C_ARRAY*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
            params->reg_mf = false;
        }
        if(params->reg_dirty) {
            SensorRegArrayW((I2C_ARRAY*)expo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)gain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
            SensorRegArrayW((I2C_ARRAY*)vts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
            params->reg_dirty = false;
        }
#if 0

		if(params->nr_dirty || params->temperature_reg_1_dirty) {
			SensorReg_Write(0x3812,0x00);
			if(params->nr_dirty) {
				SensorRegArrayW((I2C_ARRAY*)nr_reg, sizeof(nr_reg)/sizeof(I2C_ARRAY));
				params->nr_dirty = false;
			}
			if(params->temperature_reg_1_dirty)	{
				SensorRegArrayW((I2C_ARRAY*)temperature_reg_1, sizeof(temperature_reg_1)/sizeof(I2C_ARRAY));
				params->temperature_reg_1_dirty = false;
			}
#if 0
			if(params->temperature_reg_2_dirty) {
				SensorRegArrayW((I2C_ARRAY*)temperature_reg_2_dirty, sizeof(temperature_reg_2_dirty)/sizeof(I2C_ARRAY));
				//params->temperature_reg_2_dirty = false;
			}
#endif
			SensorReg_Write(0x3812,0x30);
		}
#endif

        break;
        default :
        break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us) {
  int rc=0;
  u32 lines = 0;
    lines |= (u32)(expo_reg[0].data&0x000f)<<16;
    lines |= (u32)(expo_reg[1].data&0x00ff)<<8;
    lines |= (u32)(expo_reg[2].data&0x00f0)<<0;
    lines >>= 4;
    *us = (lines*Preview_line_period)/1000/2; //return us

  CamOsPrintf("[%s] sensor expo lines/us %d, %dus\n", __FUNCTION__, lines, *us);
  return rc;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us) {
    int i;
    u32 half_lines = 0,vts = 0;
    sc2335_params *params = (sc2335_params *)handle->private_data;
    I2C_ARRAY expo_reg_temp[] = {  // max expo line vts-4!
    {0x3e00, 0x00},//expo [20:17]
    {0x3e01, 0x00}, // expo[16:8]
    {0x3e02, 0x10}, // expo[7:0], [3:0] fraction of line
    };
    memcpy(expo_reg_temp, expo_reg, sizeof(expo_reg));

    half_lines = (1000*us*2)/Preview_line_period; // Preview_line_period in ns
    if(half_lines<1)
		half_lines=1;//
    if (half_lines >  2 * (params->expo.vts) - 8) {
        vts = (half_lines+9)/2;
    }
    else
        vts= params->expo.vts;

    params->expo.line = half_lines;
    //CamOsPrintf("[%s] us %ld, half_lines %ld, vts %ld\n", __FUNCTION__, us, half_lines, params->expo.vts);

    half_lines = half_lines<<4;
//  printf("===================================================================\n");
    // printk("us = %d  half_lines = %x params->expo.vts = %x\n",us, half_lines, params->expo.vts);
//  printf("===================================================================\n");
    expo_reg[0].data = (half_lines>>16) & 0x0f;
    expo_reg[1].data =  (half_lines>>8) & 0xff;
    expo_reg[2].data = (half_lines>>0) & 0xf0;
    vts_reg[0].data = (vts >> 8) & 0x00ff;
    vts_reg[1].data = (vts >> 0) & 0x00ff;

    for (i = 0; i < sizeof(expo_reg)/sizeof(I2C_ARRAY); i++) {
      if (expo_reg[i].data != expo_reg_temp[i].data) {
        params->reg_dirty = true;
        break;
      }
    }
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain) {
    int rc = 0;

    return rc;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain) {

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain) {
    sc2335_params *params = (sc2335_params *)handle->private_data;
    u8 i=0 ,Dgain = 1,  Ana_gain = 1;
    u64 Fine_againx64 = 32,Fine_dgainx128 = 128;
    u8 Dgain_reg = 0, Ana_gain_reg = 0, Fine_again_reg= 0x20,Fine_dgain_reg= 0x80;
	//static u32 reg_0x397475=0/*, BLC_value=0, BLC_ratio=1*/;
	//u16 reg3974, reg3974_2, reg3975;
	//static u8 frmcount=0;

    I2C_ARRAY gain_reg_temp[] = {
        {0x3e06, 0x00},
        {0x3e07, 0x80},
        {0x3e08, 0x03},
        {0x3e09, 0x20},
    };
#if 0
	I2C_ARRAY nr_reg_temp[] ={
		{0x363c, 0x0e},
	};
	I2C_ARRAY temperature_reg_1_temp[] ={
		{0x5787, 0x00},
		{0x5788, 0x00},
		{0x5790, 0x00},
		{0x5791, 0x00},
		{0x5799, 0x07},
	};
	// I2C_ARRAY temperature_reg_2_temp[] ={
		// {0x3637, 0x20},
		// {0x391f, 0x18},
		// {0x3908, 0x82},
	// };
#endif
    memcpy(gain_reg_temp, gain_reg, sizeof(gain_reg));
	//memcpy(nr_reg_temp, nr_reg, sizeof(nr_reg));
	//memcpy(temperature_reg_1_temp, temperature_reg_1, sizeof(temperature_reg_1));
	//memcpy(temperature_reg_2_temp, temperature_reg_2, sizeof(temperature_reg_2));

    if (gain<1024){
        gain=1024;
    }else if (gain>=SENSOR_MAXGAIN*1024){
        gain=SENSOR_MAXGAIN*1024;
    }

    if (gain < 2 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 1;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x03;
    } else if (gain < 4 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 2;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x07;
    } else if (gain < 8 * 1024) {
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 4;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x0f;
    } else if (gain <=  16128) {// 16256
        Dgain = 0;      Fine_dgainx128 = 128;         Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_dgain_reg = 0x80;  Ana_gain_reg = 0x1f;
    }
#if 1
	else if (gain <  16128 * 2) {
        Dgain = 1;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x00;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (gain <  16128 * 4) {
        Dgain = 2;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x01;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (gain < 16128 * 8) {
        Dgain = 4;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x03;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (gain < 16128 * 16) {
        Dgain = 8;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x07;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    } else if (gain <= 508032) { // 16128 * 31.5  ---   SENSOR_MAXGAIN * 1024
        Dgain = 16;      Fine_againx64 = 63;    Ana_gain = 8;
        Dgain_reg = 0x0f;  Fine_again_reg = 0x3f;  Ana_gain_reg = 0x1f;
    }
#endif

    if (gain <= 16128) {
        Fine_againx64 = abs(4 * gain/ (Ana_gain * Fine_dgainx128));
        //Fine_dgainx128 = abs(8 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_again_reg = Fine_againx64;
        //Fine_dgain_reg = Fine_dgainx128;
    } else {
        Fine_dgainx128 = abs(4 * gain/ (Dgain * Ana_gain * Fine_againx64));
        Fine_dgain_reg = Fine_dgainx128;
    }
	// printk("[%s]  gain : %d,%lld,%lld,%d, %d\n", __FUNCTION__,gain,Fine_againx64,Fine_dgainx128,Dgain,Ana_gain);
    // printk("[%s]  gain:%d. gain_reg : %x ,%x ,%x , %x\n", __FUNCTION__,gain,Fine_again_reg,Ana_gain_reg,Fine_dgain_reg,Dgain_reg);

    gain_reg[3].data = Fine_again_reg;      // 3e09
    gain_reg[2].data = Ana_gain_reg;		// 3e08
    gain_reg[1].data = Fine_dgain_reg;		// 3e07
    gain_reg[0].data = Dgain_reg & 0x0f;	// 3e06

	for (i = 0; i < sizeof(gain_reg)/sizeof(I2C_ARRAY); i++) {
      if (gain_reg[i].data != gain_reg_temp[i].data) {
        params->reg_dirty = true;
        break;
      }
    }

#if 0
	if(reg3040 == 0x40) {
		if (gain_reg_temp[2].data < 0x07) { // delay 1 frame group, group & exp & gain active at the same time
			nr_reg[0].data = 0x0e;
		} else {
			nr_reg[0].data = 0x07;
		}
	} else if(reg3040 == 0x41) {
		if (gain_reg_temp[2].data < 0x07) {
			nr_reg[0].data = 0x0f;
		} else {
			nr_reg[0].data = 0x07;
		}
	} else {
		nr_reg[0].data = 0x07;
	}

	/* Each frame refresh to avoid the red flash when linear and wide dynamic switching */
	for (i = 0; i < sizeof(nr_reg)/sizeof(I2C_ARRAY); i++) {
	  if (nr_reg[i].data != nr_reg_temp[i].data) {
		params->nr_dirty = true;
		break;
	  }
	}
#endif

#if 0
	if (frmcount < 3) { // don't triggle logic for the beginning three frames.
		frmcount++;
	} else {
		// highTemp dpc

		SensorReg_Read(0x3974, &reg3974);
		SensorReg_Read(0x3975, &reg3975);
		reg_0x397475 = ((reg3974 << 8) & 0xFF00) | (reg3975 & 0x00FF);
		// BLC_value = ((reg_0x397475 >> 3) - 0x400)*BLC_ratio;
		// high temp 1
		SensorReg_Read(0x3974, &reg3974_2);

		if (reg3974 == reg3974_2) {
			if ((reg_0x397475 > 0x1040) || (gain >= 32 * 1024)) {
				if (reg_0x397475 > 0x1040) {
					temperature_reg_1[0].data = 0x00;
					temperature_reg_1[1].data = 0x00;
					temperature_reg_1[2].data = 0x00;
					temperature_reg_1[3].data = 0x00;
					temperature_reg_1[4].data = 0x07;
					//printk("11111111111111111\n");
				} else {
					temperature_reg_1[0].data = 0x10;
					temperature_reg_1[1].data = 0x06;
					temperature_reg_1[2].data = 0x10;
					temperature_reg_1[3].data = 0x10;
					temperature_reg_1[4].data = 0x07;
					//printk("222222222222222 reg_0x397475 = %d.\n", reg_0x397475);
				}
			} else if ((reg_0x397475 < 0x1030) && (gain <= 24 * 1024)) {
				temperature_reg_1[0].data = 0x10;
				temperature_reg_1[1].data = 0x06;
				temperature_reg_1[2].data = 0x10;
				temperature_reg_1[3].data = 0x10;
				temperature_reg_1[4].data = 0x00;
				//printk("33333333333333\n");
			}
		}
		else {
			//printk("44444444444444\n");
		}

		for (i = 0; i < sizeof(temperature_reg_1)/sizeof(I2C_ARRAY); i++) {
		  if (temperature_reg_1[i].data != temperature_reg_1_temp[i].data) {
			params->temperature_reg_1_dirty = true;
			break;
		  }
		}
	}
#endif

    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max) {
    *min = 1;
    *max = 1000000000/Preview_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max) {
  *min = 1024;
  *max = SENSOR_MAXGAIN*1024;
  return SUCCESS;
}

static int sc2335_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period; //  * 3;
    info->step = Preview_line_period/2;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num) {

    return SUCCESS;
}

int cus_camsensor_init_handle_linear(ms_cus_sensor* drv_handle) {
   ms_cus_sensor *handle = drv_handle;
    sc2335_params *params;
    if (!handle) {
        CamOsPrintf("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    CamOsPrintf("[%s]", __FUNCTION__);
    //private data allocation & init
    if (handle->private_data == NULL) {
        CamOsPrintf("[%s] Private data is empty!\n", __FUNCTION__);
        return FAIL;
    }
    params = (sc2335_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    strcpy(handle->model_id,"sc210iot_MIPI");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //CamOsPrintf("[%s] entering function with id %d\n", __FUNCTION__, id);
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
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = Preview_WIDTH;
    handle->video_res_supported.res[0].height = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 0;
    handle->video_res_supported.res[0].nOutputHeight= 0;
    sprintf(handle->video_res_supported.res[0].strResDesc, "1920x1080@30fps");


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
    handle->pCus_sensor_init        = pCus_init;

    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    handle->pCus_sensor_post_init   = pCus_sensor_post_init;

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
    handle->pCus_sensor_SetPatternMode = sc2335_SetPatternMode;
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
    handle->pCus_sensor_GetShutterInfo = sc2335_GetShutterInfo;
    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->expo.line= 1000;
    params->reg_dirty = false;
    params->reg_mf = false;
    params->nr_dirty = false;
    return SUCCESS;
}

int cus_camsensor_release_handle(ms_cus_sensor *handle) {
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  SC210iot,
                            cus_camsensor_init_handle_linear,
                            NULL,
                            NULL,
                            sc2335_params
                         );
