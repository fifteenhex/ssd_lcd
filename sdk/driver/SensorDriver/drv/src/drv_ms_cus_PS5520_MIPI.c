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
#include <PS5520_MIPI_init_table.h>

#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(PS5520);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define SENSOR_MIPI_LANE_NUM (2)

//#undef SENSOR_DBG
#define SENSOR_DBG 0

#define SENSOR_ISP_TYPE         ISP_EXT                 //ISP_EXT, ISP_SOC
#define SENSOR_IFBUS_TYPE       CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#define SENSOR_MIPI_HSYNC_MODE  PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC         CUS_DATAPRECISION_10    //CFG //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE         CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID          CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID          CUS_RGBIR_NONE
#define SENSOR_ORIT             CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
#define SENSOR_MAX_GAIN         32                      // max sensor again, a-gain

#define Preview_MCLK_SPEED      CUS_CMU_CLK_24MHZ       //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define PREVIEW_LINE_TIME_NS    33670                   //How long producing one line takes. Line per frame = VTS+1 , line period = (1/FPS)/(VTS+1)

#define VTS                     1979                    //Vertical Totoal size, Line per frame. 0x07BB from init table address 0x0A:0x0B
#define Preview_WIDTH           2592                    //resolution Width when preview
#define Preview_HEIGHT          1944                    //resolution Height when preview
#define PREVIEW_MAX_FPS         15                      //fastest preview FPS
#define PREVIEW_MIN_FPS         5                       //slowest preview FPS
#define Preview_CROP_START_X    0                       //CROP_START_X
#define Preview_CROP_START_Y    0                       //CROP_START_Y

#define SENSOR_I2C_ADDR         0x90                    //I2C slave address
#define SENSOR_I2C_SPEED        200000                  //I2C speed, 60000~320000

#define SENSOR_I2C_LEGACY       I2C_NORMAL_MODE         //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT          I2C_FMT_A8D8            //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16

#define SENSOR_PWDN_POL         CUS_CLK_POL_POS         // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL          CUS_CLK_POL_NEG         // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL        CUS_CLK_POL_NEG         // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL        CUS_CLK_POL_NEG         // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL         CUS_CLK_POL_POS         // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

static volatile int FirstTime = 1;

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
        u32 sclk;
        u32 hts;
        u32 vts;        //target vts
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
        u32 cur_vts;    //actual vts
        u32 new_fps;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    bool dirty;
    bool orient_dirty;
    u32 cur_shutter;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[2];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY mirror_flip[4];
} ps5520_params;
// set sensor ID address and data,

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static I2C_ARRAY Sensor_id_table[] =
{
    {0x00, 0x52},       // {address of ID, ID },
    {0x01, 0x50},
};

I2C_ARRAY PatternTbl[] = {
    //pattern mode
};

static I2C_ARRAY mirr_flip_table[] = {
    {0xEF, 0x01},       //M0F0
    {0x1B, 0x0a},       //[3:0] Cmd_HSize_e1[11:8], [7]Cmd_HFlip
    {0x1C, 0x30},       //[7:0] Cmd_HSize_e1[7:0]
    {0x1D, 0x07},       //bit[7]   Vflip

    {0xEF, 0x01},       //M1F0
    {0x1B, 0x8a},       //[3:0] Cmd_HSize_e1[11:8], [7]Cmd_HFlip
    {0x1C, 0x2F},       //[7:0] Cmd_HSize_e1[7:0]
    {0x1D, 0x07},       //bit[7]   Vflip

    {0xEF, 0x01},       //M0F1
    {0x1B, 0x0a},       //[3:0] Cmd_HSize_e1[11:8], [7]Cmd_HFlip
    {0x1C, 0x30},       //[7:0] Cmd_HSize_e1[7:0]
    {0x1D, 0x87},       //bit[7:4] Cmd_ADC_Latency

    {0xEF, 0x01},       //M1F1
    {0x1B, 0x8a},       //[3:0] Cmd_HSize_e1[11:8], [7]Cmd_HFlip
    {0x1C, 0x2F},       //[7:0] Cmd_HSize_e1[7:0]
    {0x1D, 0x87},       //bit[7:4] Cmd_ADC_Latency
};

static I2C_ARRAY gain_reg[] = {
    {0xEF, 0x01},
    //{0x80, 0x00},       // Digital Gain
    {0x83, 0x00},       // Analog Gain
};

static Gain_ARRAY gain_table[]={
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

static int g_sensor_ae_min_gain = 1280;
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

static I2C_ARRAY expo_reg[] = {
    {0xEF, 0x01},
    {0x0C, 0x00},
    {0x0D, 0x03},
};

static I2C_ARRAY vts_reg[] = {
    {0xEF, 0x01},
    {0x0A, 0x07},
    {0x0B, 0xBB},

};

/////////// function definition ///////////////////
#if SENSOR_DBG == 1
//#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
//#define SENSOR_DMSG(args...) LOGE(args)
#define SENSOR_DMSG(args...) SENSOR_DMSG(args)
#elif SENSOR_DBG == 0
//#define SENSOR_DMSG(args...)
#endif

#undef  SENSOR_NAME
#define SENSOR_NAME ps5520

#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);
static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status);
static int cus_camsensor_release_handle(ms_cus_sensor *handle);
static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps);

/////////////////// sensor hardware dependent //////////////

static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    int res = 0;
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] ", __FUNCTION__);

    //TStart = timeGetTimeU();
    /*PAD and CSI*/
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0, 0x1C00, 0);

    /*Power ON*/
    res = sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    if(res >= 0)//if success
        SENSOR_UDELAY(100);     //T1 = 100us

    /*Reset PIN*/
    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    res = sensor_if->Reset(idx, handle->reset_POLARITY);
    if(res >= 0)//if success
        SENSOR_UDELAY(1000);    //T3 = 1ms

    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    res = sensor_if->Reset(idx, !handle->reset_POLARITY);
    if(res >= 0)//if success
        SENSOR_UDELAY(100);     //T2 = 100us

    /*MCLK ON*/
    res = sensor_if->MCLK(idx, 1, handle->mclk);
    if(res >= 0)//if success
        CamOsUsSleep(3*1000);     //T4 = 3ms

    //CamOsPrintf("pCus_poweron = %d us \n",timeGetTimeU()-TStart);
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    // power/reset low
    ps5520_params *params = (ps5520_params *)handle->private_data;

    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_UDELAY(100);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);
    sensor_if->MCLK(idx, 0, handle->mclk);

    params->dirty = false;
    params->orient_dirty = false;

    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    int i,n;
    int table_length = ARRAY_SIZE(Sensor_id_table);
    I2C_ARRAY id_from_sensor[ARRAY_SIZE(Sensor_id_table)];

    SensorReg_Write(0xef,0x00);

    for(n = 0; n < table_length; ++n)
    {
        id_from_sensor[n].reg = Sensor_id_table[n].reg;
        id_from_sensor[n].data = 0;
    }

    *id = 0;
    if(table_length > 8)
        table_length = 8;

    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(n = 0; n < 4; ++n) //retry , until I2C success
    {
        if(n > 2)
            return FAIL;

        if( SensorRegArrayR((I2C_ARRAY*)id_from_sensor,table_length) == SUCCESS) //read sensor ID from I2C
            break;
        else
            continue;
    }

    //convert sensor id to u32 format
    for(i = 0; i < table_length; ++i)
    {
        if( id_from_sensor[i].data != Sensor_id_table[i].data )
            return FAIL;
        *id = ((*id)+ id_from_sensor[i].data) << 8;
    }

    *id >>= 8;
    SENSOR_DMSG("[%s]ps5520 Read sensor id, get 0x%x Success\n", __FUNCTION__, (int)*id);

    return SUCCESS;
}

static int ps5520_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    SENSOR_DMSG("\n\n[%s], mode=%d \n", __FUNCTION__, mode);

    return SUCCESS;
}

static int pCus_init(ms_cus_sensor *handle)
{
    unsigned int i,cnt=0;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    for(i=0; (sizeof(Sensor_init_table) && (i < ARRAY_SIZE(Sensor_init_table))); i++)
    {
        /*if(Sensor_init_table[i].reg == 0xffff)
        {
            CamOsUsSleep(Sensor_init_table[i].data * 1000);
            continue;
        }*/
        cnt = 0;
        while(SensorReg_Write(Sensor_init_table[i].reg,Sensor_init_table[i].data) != SUCCESS)
        {
            cnt++;
            SENSOR_DMSG("Sensor_init_table -> Retry %d...\n",cnt);
            if(cnt >= 10)
            {
                SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
                return FAIL;
            }
            CamOsUsSleep(10*1000);
        }
    }

    for(i = 0; sizeof(PatternTbl) && (i < ARRAY_SIZE(PatternTbl)); i++)
    {
        if(SensorReg_Write(PatternTbl[i].reg,PatternTbl[i].data) != SUCCESS)
        {
            SENSOR_DMSG("[%s:%d]Sensor init fail!!\n", __FUNCTION__, __LINE__);
            return FAIL;
        }
    }

    FirstTime = 1;

    //pCus_SetAEGain(handle, 1536); //Set sensor gain = 1x
    //pCus_SetAEUSecs(handle, 15000);
    //pCus_SetFPS(handle, PREVIEW_MAX_FPS*1000);

    //CamOsPrintf("pCus_init = %d us \n",timeGetTimeU()-TStart);

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
    u32 num_res = handle->video_res_supported.num_res;
    if (res_idx >= num_res)
        return FAIL;

    switch (res_idx) {
        case 0:
            handle->video_res_supported.ulcur_res = 0;
            handle->pCus_sensor_init = pCus_init;
            break;
        case 1:
            handle->video_res_supported.ulcur_res = 1;
            handle->pCus_sensor_init = pCus_init;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
    *orit = handle->orient;
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    int table_length = ARRAY_SIZE(mirr_flip_table);
    int seg_length = table_length/4;
    int i,j;

    SENSOR_DMSG("\n\n[%s] orit=%d", __FUNCTION__, orit);

    if(handle->orient == orit)
        return SUCCESS;

    switch(orit)
    {
        case CUS_ORIT_M0F0:
            handle->orient = orit;
            for(i = 0, j = 0; i < seg_length; i++, j++)
            {
                params->mirror_flip[j].reg = mirr_flip_table[i].reg;
                params->mirror_flip[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M1F0:
            handle->orient = orit;
            for(i = seg_length, j = 0; i < seg_length*2; i++, j++)
            {
                params->mirror_flip[j].reg = mirr_flip_table[i].reg;
                params->mirror_flip[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M0F1:
            handle->orient = orit;
            for(i = seg_length*2, j = 0; i < seg_length*3; i++, j++)
            {
                params->mirror_flip[j].reg = mirr_flip_table[i].reg;
                params->mirror_flip[j].data = mirr_flip_table[i].data;
            }
            break;

        case CUS_ORIT_M1F1:
            handle->orient = orit;
            for(i = seg_length*3, j = 0; i < seg_length*4; i++, j++)
            {
                params->mirror_flip[j].reg = mirr_flip_table[i].reg;
                params->mirror_flip[j].data = mirr_flip_table[i].data;
            }
            break;
        default :
            handle->orient = CUS_ORIT_M0F0;
            for(i = 0, j = 0; i < seg_length; i++, j++)
            {
                params->mirror_flip[j].reg = mirr_flip_table[i].reg;
                params->mirror_flip[j].data = mirr_flip_table[i].data;
            }
            break;
    }

    params->orient_dirty = true;
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    return (VTS*PREVIEW_MAX_FPS*1000)/params->expo.cur_vts;
}

static int _SetFps(ms_cus_sensor *handle, u32 fps)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    SENSOR_DMSG("\n\n[%s]", __FUNCTION__);

    if(fps >= PREVIEW_MIN_FPS*1000 && fps <= PREVIEW_MAX_FPS*1000)
    {
        params->expo.vts =  (VTS * PREVIEW_MAX_FPS * 1000) / fps ;
        params->tVts_reg[1].data = (params->expo.vts >> 8) & 0x00ff;
        params->tVts_reg[2].data = (params->expo.vts >> 0) & 0x00ff;
        pCus_SetAEUSecs(handle, params->cur_shutter);
        return SUCCESS;
    }

    return FAIL;
}

static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    SENSOR_DMSG("[%s] Fps=%d", __FUNCTION__, fps);

    if(fps >= PREVIEW_MIN_FPS && fps <= PREVIEW_MAX_FPS)
      params->expo.target_fps = fps*1000;
    else if(fps >= PREVIEW_MIN_FPS*1000 && fps <= PREVIEW_MAX_FPS*1000)
        params->expo.target_fps = fps;
    else
      return FAIL;

    if( abs((int)params->expo.target_fps-(int)params->expo.cur_fps) >= params->expo.cur_fps )
    {
        int diff = params->expo.target_fps - params->expo.cur_fps;
        if(diff>0)
            params->expo.new_fps = params->expo.cur_fps + (params->expo.cur_fps/2);
        else
            params->expo.new_fps = params->expo.cur_fps - (params->expo.cur_fps/2);

        _SetFps( handle, params->expo.new_fps);
    }
    else
    {
        params->expo.new_fps = params->expo.target_fps;
        _SetFps( handle, params->expo.target_fps);
    }

    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification

static int pCus_AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //mirror flip
            if(params->orient_dirty)
            {
                /*Drop MIPI frame begin*/
                //SensorReg_Write(0xEF,5); //bank 5
                //SensorReg_Write(0x25,1); //CSI stall

                SensorRegArrayW( (I2C_ARRAY*)params->mirror_flip, ARRAY_SIZE(params->mirror_flip));
                SensorReg_Write(0x09,1);

                /*Drop MIPI frame end*/
                //CamOsMsSleep( (1000000/pCus_GetFPS(handle))+5 ); //delay 1 frame time + 5ms
                //SensorReg_Write(0xEF,5); //bank 5
                //SensorReg_Write(0x25,0); //CSI stall

                params->orient_dirty = false;
            }
             break;
        case CUS_FRAME_ACTIVE:
            //shutter, fps , gain
            if(params->dirty)
            {
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, ARRAY_SIZE(expo_reg));
                SensorRegArrayW((I2C_ARRAY*)&(params->tGain_reg[1]), ARRAY_SIZE(gain_reg)-1);
                SensorRegArrayW((I2C_ARRAY*)&(params->tVts_reg[1]), ARRAY_SIZE(vts_reg)-1);

                SensorReg_Write(0x09,1);
                params->expo.cur_fps = params->expo.new_fps;
                params->dirty = false;
            }
            else
            {
                if( params->expo.target_fps != params->expo.cur_fps ) //smooth fps
                    pCus_SetFPS(handle, params->expo.target_fps);
            }
            break;
    }
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    *us = params->expo.line * PREVIEW_LINE_TIME_NS/1000;
    return SUCCESS;
}

/************************************************************************************
Reference to PS5520_Sensor_Control_Guide_v1.2_SigmaStar.pdf P.3
    Exposure Time (us) = (Line_Num * Line_Time_Cycle + Line_Offset) / PixelClock

1. Line_Offset is a small number so let's omit it.
2. Linux_Num = Lpf - OffNy.
We get:
    Exposure Time (us) = [(Lpf - OffNy) * Line_Time_Cycle] / PixelClock
:=  Exposure_Time (us) = (Lpf - OffNy) * LINE_TIME_NS
:=  expo_line_num = Lpf - OffNy

Because it's must that 2 <= OffNy <= Lpf-1, if expo_line_num > Lpf-2, we increase
the Lpf. Otherwise, if expo_line_num <= Lpf-2, we increase OffNy.
*************************************************************************************/
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 expo_time_us)
{
    u32 expo_line_num = 0, Lpf = 0, OffNy = 0;
    ps5520_params *params = (ps5520_params *)handle->private_data;
    params->cur_shutter = expo_time_us;

    expo_line_num = (1000 * expo_time_us) / PREVIEW_LINE_TIME_NS;

    if (expo_line_num > params->expo.vts-2)
        Lpf = expo_line_num + 2;
    else
        Lpf = params->expo.vts;

    SENSOR_DMSG("[%s] us %ld, Exposure Line Num %ld, vts %ld\n", __FUNCTION__,
                expo_time_us,
                expo_line_num,
                params->expo.vts
                );

    params->expo.cur_vts = Lpf;
    params->expo.line = expo_line_num;
    OffNy = Lpf - expo_line_num;
    params->tExpo_reg[1].data = (u16)((OffNy >> 8) & 0x00ff);
    params->tExpo_reg[2].data = (u16)((OffNy >> 0) & 0x00ff);

    params->tVts_reg[1].data = (u16)((params->expo.cur_vts >> 8) & 0x00ff);
    params->tVts_reg[2].data = (u16)((params->expo.cur_vts >> 0) & 0x00ff);;

    params->dirty = true;
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;

    *gain=params->expo.final_gain;
    SENSOR_DMSG("[%s] set gain/reg=%d/0x%x\n", __FUNCTION__, gain,params->tGain_reg[1].data);

    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    ps5520_params *params = (ps5520_params *)handle->private_data;
    u32 i;
    u32 gain_double,total_gain_double;
    CUS_GAIN_GAP_ARRAY* Sensor_Gain_Linearity;
    //u16 gain16 = 0;

    params->expo.final_gain = gain;
    if(gain < handle->sat_mingain)
        gain = handle->sat_mingain;
    else if(gain >= SENSOR_MAX_GAIN*1024)
        gain = SENSOR_MAX_GAIN*1024;

    Sensor_Gain_Linearity = gain_gap_compensate;

    for(i = 0; i < ARRAY_SIZE(gain_gap_compensate); i++)
    {
        if (Sensor_Gain_Linearity[i].gain == 0)
            break;
        if((gain > Sensor_Gain_Linearity[i].gain) && (gain < (Sensor_Gain_Linearity[i].gain + Sensor_Gain_Linearity[i].offset)))
        {
              gain = Sensor_Gain_Linearity[i].gain;
              break;
        }
    }

    gain_double = (u32)gain;
    total_gain_double = (gain_double * 10000) / 1024;
    for(i = 1; i < ARRAY_SIZE(gain_table); i++)
    {
        if(gain_table[i].total_gain > total_gain_double)
        {
            //gain16 = gain_table[i-1].reg_val;
            break;
        }
        else if(i == ARRAY_SIZE(gain_table)-1)
        {
            //gain16 = gain_table[i].reg_val;
            break;
        }
    }
    params->tGain_reg[1].data = i;

    params->dirty = true;
    return SUCCESS;
}

// See the comment above ps5520_GetShutterInfo(...)
static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = PREVIEW_LINE_TIME_NS;
    *max = 1000000 / PREVIEW_MIN_FPS;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    *min = handle->sat_mingain;
    *max = SENSOR_MAX_GAIN * 1024;
    return SUCCESS;
}

/************************************************************************************
Reference to PS5520_Sensor_Control_Guide_v1.2_SigmaStar.pdf P.3
1. Line_Num = Cmd_Lpf - Cmd_OffNy
2. 2 <= Cmd_OffNy1 <= Cmd_Lpf-1

So
    Line_Num >= Cmd_Lpf - (Cmd_Lpf - 1)
:=  Line_Num >= 1

So min exposure line = 1, min exposure time = PREVIEW_LINE_TIME_NS.

************************************************************************************/
static int ps5520_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    info->max = 1000000000 / PREVIEW_MIN_FPS;
    info->min =  PREVIEW_LINE_TIME_NS;
    info->step = PREVIEW_LINE_TIME_NS;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    u32 i, j;
    for(i = 0, j = 0; i < num; i++, j += 2)
    {
        gain_gap_compensate[i].gain = pArray[i].gain;
        gain_gap_compensate[i].offset = pArray[i].offset;
    }

    return SUCCESS;
}

static int cus_camsensor_init_handle(ms_cus_sensor* drv_handle)
{
    ms_cus_sensor *handle = drv_handle;
    ps5520_params *params;
    if (!handle)
    {
        SENSOR_DMSG("[%s] not enough memory!\n", __FUNCTION__);
        return FAIL;
    }
    SENSOR_DMSG("[%s]", __FUNCTION__);

    //private data allocation & init
    params = (ps5520_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));
    memcpy(params->mirror_flip, mirr_flip_table, sizeof(I2C_ARRAY)*4);

    ////////////////////////////////////
    //    sensor model ID             //
    ////////////////////////////////////
    strcpy(handle->model_id, "ps5520_MIPI");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    handle->isp_type    = SENSOR_ISP_TYPE;      //ISP_EXT;
    handle->sif_bus     = SENSOR_IFBUS_TYPE;    //CUS_SENIF_BUS_MIPI;
    handle->data_prec   = SENSOR_DATAPREC;      //CUS_DATAPRECISION_10;
    handle->data_mode   = SENSOR_DATAMODE;
    handle->bayer_id    = SENSOR_BAYERID;       //CUS_BAYER_BG;
    handle->RGBIR_id    = SENSOR_RGBIRID;
    handle->orient      = SENSOR_ORIT;          //CUS_ORIT_M0F0;
    handle->interface_attr.attr_mipi.mipi_lane_num                  = SENSOR_MIPI_LANE_NUM;
    handle->interface_attr.attr_mipi.mipi_data_format               = CUS_SEN_INPUT_FORMAT_RGB; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order                 = 0;                        //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode                = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode                  = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num   = 0;                        //Short frame

    ////////////////////////////////////
    //    resolution capability       //
    ////////////////////////////////////

    handle->video_res_supported.num_res             = 1;
    handle->video_res_supported.ulcur_res           = 0;
    handle->video_res_supported.res[0].width        = Preview_WIDTH;
    handle->video_res_supported.res[0].height       = Preview_HEIGHT;
    handle->video_res_supported.res[0].max_fps      = PREVIEW_MAX_FPS;
    handle->video_res_supported.res[0].min_fps      = PREVIEW_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x = Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y = Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth = 2592;
    handle->video_res_supported.res[0].nOutputHeight= 1944;
    strcpy(handle->video_res_supported.res[0].strResDesc, "2592x1944@15fps");
/*
    handle->video_res_supported.num_res             = 2;
    handle->video_res_supported.ulcur_res           = 0;
    handle->video_res_supported.res[1].width        = 1928;
    handle->video_res_supported.res[1].height       = 1088;
    handle->video_res_supported.res[1].max_fps      = PREVIEW_MAX_FPS;
    handle->video_res_supported.res[1].min_fps      = PREVIEW_MIN_FPS;
    handle->video_res_supported.res[1].crop_start_x = Preview_CROP_START_X;
    handle->video_res_supported.res[1].crop_start_y = Preview_CROP_START_Y;
    handle->video_res_supported.res[1].nOutputWidth = 1928;
    handle->video_res_supported.res[1].nOutputHeight= 1088;
    strcpy(handle->video_res_supported.res[1].strResDesc, "1928x1088@30fps");
*/
    // i2c
    handle->i2c_cfg.mode                = SENSOR_I2C_LEGACY;    //I2C_NORMAL_MODE;
    handle->i2c_cfg.fmt                 = SENSOR_I2C_FMT;       //I2C_FMT_A8D8;
    handle->i2c_cfg.address             = SENSOR_I2C_ADDR;      //0x90;
    handle->i2c_cfg.speed               = SENSOR_I2C_SPEED;     //200000;

    // mclk
    handle->mclk                        = Preview_MCLK_SPEED;   //CUS_CMU_CLK_24MHZ

    //polarity
    /////////////////////////////////////////////////////
    handle->pwdn_POLARITY               = SENSOR_PWDN_POL;  //CUS_CLK_POL_POS;
    handle->reset_POLARITY              = SENSOR_RST_POL;   //CUS_CLK_POL_NEG;
    handle->VSYNC_POLARITY              = SENSOR_VSYNC_POL; //CUS_CLK_POL_NEG;
    handle->HSYNC_POLARITY              = SENSOR_HSYNC_POL; //CUS_CLK_POL_NEG;
    handle->PCLK_POLARITY               = SENSOR_PCLK_POL;  //CUS_CLK_POL_POS);    // use '!' to clear board latch error
    /////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // AE parameters
    ////////////////////////////////////////////////////
    handle->ae_gain_delay               = 2;
    handle->ae_shutter_delay            = 2;

    handle->ae_gain_ctrl_num            = 1;
    handle->ae_shutter_ctrl_num         = 1;

    ///calibration
    handle->sat_mingain                 = g_sensor_ae_min_gain;

    handle->pCus_sensor_release         = cus_camsensor_release_handle;
    handle->pCus_sensor_init            = pCus_init;

    handle->pCus_sensor_poweron         = pCus_poweron ;
    handle->pCus_sensor_poweroff        = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID     = pCus_GetSensorID;

    handle->pCus_sensor_GetVideoResNum  = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes     = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes     = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien        = pCus_GetOrien;
    handle->pCus_sensor_SetOrien        = pCus_SetOrien;
    handle->pCus_sensor_GetFPS          = pCus_GetFPS;
    handle->pCus_sensor_SetFPS          = pCus_SetFPS;
    handle->pCus_sensor_SetPatternMode = ps5520_SetPatternMode;

    ///////////////////////////////////////////////////////
    // AE, unit: micro seconds                           //
    ///////////////////////////////////////////////////////
    handle->pCus_sensor_AEStatusNotify  = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = pCus_GetAEGain;

    handle->pCus_sensor_SetAEGain       = pCus_SetAEGain;

    handle->pCus_sensor_GetAEMinMaxGain = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= pCus_GetAEMinMaxUSecs;

    //sensor calibration
    handle->pCus_sensor_SetAEGain_cal   = NULL;
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    handle->pCus_sensor_GetShutterInfo  = ps5520_GetShutterInfo;
    params->expo.vts                    = VTS;
    params->expo.cur_fps                = PREVIEW_MAX_FPS*1000; //fps x 1000
    params->expo.target_fps             = PREVIEW_MAX_FPS*1000;
    params->expo.new_fps                = PREVIEW_MAX_FPS*1000;
    params->expo.line                   = VTS - 3;              // Accordint to init table, [0C:0D] = 0x03, which means the Cmd_OffNy.
    params->expo.sens                   = 1;
    params->dirty                       = false;
    params->orient_dirty                = false;
    return SUCCESS;
}

static int cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    if (handle && handle->private_data)
    {
        SENSOR_DMSG("[%s] release handle, handle %x, private data %x",
                __FUNCTION__,
                (int)handle,
                (int)handle->private_data);
        handle->private_data = NULL;
    }
    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(
                                PS5520,
                                cus_camsensor_init_handle,
                                NULL,
                                NULL,
                                ps5520_params
                            );
