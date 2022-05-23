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

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(IMX323);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE CAM_OS_ARRAY_SIZE
#endif

#define __FPGA_IMX323__ (1)
#define __ENABLE_AE__ (1)
#define USE_10_BIT 1
#define __FPGA_2FPS__ (0)
#define __FPGA_3FPS__ (1)

#define SENSOR_IFBUS_TYPE       CUS_SENIF_BUS_PARL      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
#if USE_10_BIT
#if __FPGA_IMX323__
#define SENSOR_DATAPREC         CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#else
#define SENSOR_DATAPREC         CUS_DATAPRECISION_10    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#endif
#else
#define SENSOR_DATAPREC         CUS_DATAPRECISION_12    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#endif

#define SENSOR_DATAMODE         CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID          CUS_BAYER_BG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT             CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,

#define lane_number             2
#define vc0_hs_mode             3                       //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00                    //UD1~UD8 (user define)

#if __FPGA_IMX323__
#define Preview_MCLK_SPEED      CUS_CMU_CLK_12MHZ            //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#else
#define Preview_MCLK_SPEED      CUS_CMU_CLK_27MHZ            //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#endif

#define Preview_line_period     30000                   //HTS/PCLK=4455 pixels/148.5MHZ=30usec
#define Prv_Max_line_number     1080                    //maximum exposure line munber of sensor when preview

#define vts_30fps 1125//1346,1616                       //for 29.1fps

#define Preview_WIDTH           1920                    //resolution Width when preview
#define Preview_HEIGHT          1080                    //resolution Height when preview
#define Preview_MAX_FPS         30                      //fastest preview FPS
#define Preview_MIN_FPS         8                       //slowest preview FPS
#define Preview_CROP_START_X    0x96                       //CROP_START_X
#define Preview_CROP_START_Y    0x21                       //CROP_START_Y

#define Cap_Max_line_number     1080                    //maximum exposure line munber of sensor when capture

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE
#define SENSOR_I2C_ADDR         0x34                    //I2C slave address
#define SENSOR_I2C_SPEED        200000                  //200KHz
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16
#define SENSOR_I2C_CHANNEL      1                       //I2C Channel
#define SENSOR_I2C_PAD_MODE     2                       //Pad/Mode Number

#define SENSOR_PWDN_POL     CUS_CLK_POL_POS             // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG             // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_NEG             //CUS_CLK_POL_NEG //CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_NEG             // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_NEG             // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG

static int Imx323SetAEGain(ms_cus_sensor *handle, u32 gain);
static int Imx323SetAEUSecs(ms_cus_sensor *handle, u32 us);

/*********************  I2C  ********************/

typedef struct {
#if 0
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
#endif
    struct {
        u8 bVideoMode;
        u16 res_idx;
        CUS_CAMSENSOR_ORIT  orit;
    } res;

    struct {
        //float sclk;
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
    //int still_min_fps;
    //int video_min_fps;
    u8 reg_dirty; //sensor setting need to update through I2C
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[3];
    I2C_ARRAY tExpo_reg[3];
} imx323_params;
// set sensor ID address and data,

//static I2C_ARRAY Sensor_id_table[] =
//{
//    {0x301C, 0x50},      // {address of ID, ID },
//};

const I2C_ARRAY Sensor_init_table[] =
{
#if defined(__FPGA_IMX323__)
    {0x0100, 0x00},   //standby control 0:standby 1: normal
    {0x0008, 0x00},   //black level offset [8]
    {0x0009, 0xF0},   //black level offset [7:0]

    {0x0101, 0x00},

    {0x0104, 0x00},

    {0x0112, 0x0C},
    {0x0113, 0x0C},

    {0x0202, 0x00},
    {0x0203, 0x00},

    //0x0340,   0x04,
    //0x0341,   0x71,//0x65, // FRM_LENGTH

    {0x0340, 0x04},//FPGA
    {0x0341, 0x65},//0x65, // FRM_LENGTH

    //0x0342,   0x04,
    //0x0343,   0x4C, // 1100 // LINE_LENGTH
#if __FPGA_2FPS__
    {0x0342, 0x15}, //FPGA
    {0x0343, 0x7C}, // 1100 // LINE_LENGTH
#elif __FPGA_3FPS__
    {0x0342, 0x0E}, //FPGA
    {0x0343, 0x52}, // 1100 // LINE_LENGTH
#else
    {0x0342, 0x08}, //FPGA
    {0x0343, 0x98}, // 1100 // LINE_LENGTH
#endif
    {0x0003, 0x08}, //FPGA
    //0x0004,   0x98, // HMAX
    {0x0004, 0xC8}, // HMAX

    {0x0005, 0x04}, //FPGA
    {0x0006, 0x65}, // VMAX
    {0x0011, 0x01}, //FPGA
    {0x009A, 0x44}, //FPGA
    {0x009B, 0x0C}, //FPGA

    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x00},
    {0x0347, 0x00},

    {0x034C, 0x07},
    {0x034D, 0xC0}, // 1984
    {0x034E, 0x04},
    {0x034F, 0xC9}, // 1225
    {0x3000, 0x31},
    {0x3001, 0x00},
    {0x3002, 0x0F},
    {0x300D, 0x00},
    {0x300E, 0x00},
    {0x300F, 0x00},
    {0x3010, 0x00},
    //0x3011,   0x00,
    {0x3011, 0x01}, //FPGA, FRSEL

    {0x3012, 0x82},

    {0x3016, 0x3C},
    {0x3017, 0x00},
    {0x301E, 0x00},
    {0x301F, 0x73},
    {0x3020, 0x3C},

    {0x3021, 0x20}, // 22clk //0x00, // XHSLNG
    {0x3022, 0x00},

    {0x3027, 0x20},
    {0x302C, 0x01},
    {0x303B, 0xE1},

    {0x307A, 0x00},
    {0x307B, 0x00},

    {0x3098, 0x26},
    {0x3099, 0x02},

    {0x309A, 0x26},
    {0x309B, 0x02},

    {0x30CE, 0x16},
    {0x30CF, 0x82},
    {0x30D0, 0x00},
    {0x3117, 0x0D},

    {0x302C, 0x00},
    {0x0100, 0x01},    //standby control 0:standby 1: normal
    //0x0100,   0x00    //standby control 0:standby 1: normal

#else
#if 1
    {0x0100, 0x00},   //standby control 0:standby 1: normal
    {0x0008, 0x00},   //black level offset [8]
    {0x0009, 0xF0},   //black level offset [7:0]
    {0x0101, 0x00},
    {0x0104, 0x00},

#if (USE_10_BIT)
    {0x0112, 0x0A},
    {0x0113, 0x0A},
#else
    {0x0112, 0x0C},
    {0x0113, 0x0C},
#endif

    {0x0202, 0x03},
    {0x0203, 0xE8},

#if (USE_10_BIT)
    {0x0340, 0x04},
    {0x0341, 0xA0},
#else
    {0x0340, 0x04},
    {0x0341, 0x71},//0x65, // FRM_LENGTH
#endif

    //0x0342,   0x04,
    //0x0343,   0x4C, // 1100 // LINE_LENGTH
    {0x0342, 0x04}, //0x42A , workaround for MCLK=36MHz , spec is 37MHz
    {0x0343, 0x2A}, //0x42A // LINE_LENGTH , HTS

    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x00},
    {0x0347, 0x00},

    {0x034C, 0x07},
    {0x034D, 0xC0}, // 1984
    {0x034E, 0x04},
    {0x034F, 0xC9}, // 1225
    {0x3000, 0x31},
    {0x3001, 0x00},
    {0x3002, 0x0F},
    {0x300D, 0x00},
    {0x300E, 0x00},
    {0x300F, 0x00},
    {0x3010, 0x00},
    {0x3011, 0x00},

#if (USE_10_BIT)
    {0x3012, 0x80},
#else
    {0x3012, 0x82},
#endif

    {0x3016, 0x3C},
    {0x3017, 0x00},
    {0x301E, 0x00},
    {0x301F, 0x73},
    {0x3020, 0x3C},

#if (USE_10_BIT)
    {0x3021, 0xA0}, // 22clk //0x00, // XHSLNG
    {0x3022, 0x00},
#else
    {0x3021, 0x20}, // 22clk //0x00, // XHSLNG
    {0x3022, 0x00},
#endif

    {0x3027, 0x20},
    {0x302C, 0x01},
    {0x303B, 0xE1},

#if (USE_10_BIT)
    {0x307A, 0x40}, // Rogers: 10bit //0x00,
    {0x307B, 0x02}, // Rogers: 10bit //0x00,
#else
    {0x307A, 0x00},
    {0x307B, 0x00},
#endif

    {0x3098, 0x26},
    {0x3099, 0x02},

#if (USE_10_BIT)
    {0x309A, 0x4C}, // Rogers: 10bit //0x26,
    {0x309B, 0x04}, // Rogers: 10bit //0x02,
#else
    {0x309A, 0x26},
    {0x309B, 0x02},
#endif

    {0x30CE, 0x16},
    {0x30CF, 0x82},
    {0x30D0, 0x00},
    {0x3117, 0x0D},

    {0x302C, 0x00},
    {0x0100, 0x01}    //standby control 0:standby 1: normal
#endif
#endif
};
#if 0
static I2C_ARRAY TriggerStartTbl[] =
{
    {0x302c,0x00}//Master mode start
};
#endif
static I2C_ARRAY PatternTbl[] =
{
    //pattern mode
    {0x3020,0x00},
    {0x3021,0x00},
    {0x3023,0x09},
    {0x3028,0x30},
    {0x3042,0x00},
    {0x3066,0x02}, //[0:1]:pattern select bit 1~2
    //{0x3065,0x81}, //[1]:pattern gen enable , [7]:pattern select bit 0
    {0x3065,0x81}, //[1]:pattern gen enable , [7]:pattern select bit 0
};

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
static I2C_ARRAY mirror_reg[] =
{
    {0x0101, 0x00},//M0F0
};

const static I2C_ARRAY gain_reg[] =
{
    //{0x350A, 0x00},//bit0, high bit
    {0x301E, 0x00},//low bit
};

static int g_sensor_ae_min_gain = 1024;

const static I2C_ARRAY expo_reg[] =
{
    {0x0202, 0x00},
    {0x0203, 0x00},
};

const static I2C_ARRAY vts_reg[] =
{
    {0x0340, 0x03},
    {0x0341, 0x18},
};

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
    sensor_if->MCLK(idx, 1, handle->mclk);

    sensor_if->SetIOPad(idx, handle->sif_bus, 0);

    SENSOR_DMSG("[%s] reset low\n", __FUNCTION__);
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(1);
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);
    SENSOR_MSLEEP(1);

    // power -> high, reset -> high
    SENSOR_DMSG("[%s] power high\n", __FUNCTION__);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_MSLEEP(1);
    SENSOR_DMSG("[%s] reset high\n", __FUNCTION__);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(1);

    //sensor_if->Set3ATaskOrder( def_order);
    // pure power on
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);

    //usleep(1000);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);		//Set_csi_if(0, 0);
    //sensor_if->MCLK(0, 0, handle->mclk);

    return SUCCESS;
}


#if 0
/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
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
			usleep(1*1000);
	}
	SENSOR_IMSG("[%s] %s get sensor id:0x%x, SUCCESS!\n", __FUNCTION__, handle->model_id, ( int )*id);
	return SUCCESS;
}
#endif

static int Imx323SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
	int i;
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

	return SUCCESS;
}

static int Imx323Init(ms_cus_sensor *handle)
{
    int i,cnt=0;
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);

    for(i=0;i< ARRAY_SIZE(Sensor_init_table);i++)
    {
        //SENSOR_EMSG("[%s] addr:0x%x, data:0x%x\n", __FUNCTION__, Sensor_init_table[i].reg, Sensor_init_table[i].data);
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
				SENSOR_DMSG("[%s] set Sensor_init_table retry : %d...\n",__FUNCTION__,cnt);
				if(cnt>=10)
				{
					SENSOR_EMSG("[%s:%d] retry FAIL!!\n", __FUNCTION__, __LINE__);
					return FAIL;
				}
				SENSOR_MSLEEP(10);
			}
		}
	}

#if 0 //FPGA test
	for(i=0;i< ARRAY_SIZE(mirror_reg); i++)
	{
		if(SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data) != SUCCESS)
		{
		  SENSOR_EMSG("[%s:%d] set mirror_reg fail!!\n", __FUNCTION__, __LINE__);
		  return FAIL;
		}
	}

	for(i=0;i< ARRAY_SIZE(TriggerStartTbl);i++)
	{
		if(SensorRegW(TriggerStartTbl[i].reg,TriggerStartTbl[i].data) != SUCCESS)
		{
			SENSOR_EMSG("[%s:%d] set TriggerStartTbl fail!!\n", __FUNCTION__, __LINE__);
			return FAIL;
		}
	}
#endif
	SENSOR_MSLEEP(50);
	Imx323SetAEGain(handle,2048);
	Imx323SetAEUSecs(handle, 25000);

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
            handle->pCus_sensor_init = Imx323Init;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int Imx323GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
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
	return SUCCESS;
}

static int Imx323SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
	char index=0;
	imx323_params *params = (imx323_params *)handle->private_data;

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
        u32 i;
        mirror_reg[0].data = index;
		//SensorRegArrayW((RegTable_t*)mirror_reg, sizeof(mirror_reg)/sizeof(I2C_ARRAY));
        for(i=0;i<sizeof(mirror_reg)/sizeof(I2C_ARRAY);++i)
        {
            SensorReg_Write(mirror_reg[i].reg,mirror_reg[i].data);
        }
		params->reg_dirty = 1;
	}
	return SUCCESS;
}

static int Imx323GetFPS(ms_cus_sensor *handle)
{
	imx323_params *params = (imx323_params *)handle->private_data;
	u32 max_fps = handle->video_res_supported.res[handle->video_res_supported.ulcur_res].max_fps;
    u32 tVts = (params->tVts_reg[0].data << 8) | (params->tVts_reg[1].data << 0);

    if (params->expo.fps >= 1000)
        params->expo.preview_fps = (vts_30fps*max_fps*1000)/tVts;
    else
        params->expo.preview_fps = (vts_30fps*max_fps)/tVts;

    return params->expo.preview_fps;
}

static int Imx323SetFPS(ms_cus_sensor *handle, u32 fps)
{
	imx323_params *params = (imx323_params *)handle->private_data;
	//SENSOR_DMSG("[%s]\n", __FUNCTION__);
	if(fps>=3 && fps <= 30)
	{
		params->expo.fps = fps;
		params->expo.vts=  (vts_30fps*30)/fps;
		params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
		params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
	    params->reg_dirty = 1; //reg need to update = true;
	   return SUCCESS;
	}
	else if(fps>=3000 && fps <= 30000)
	{
		params->expo.fps = fps;
		params->expo.vts=  (vts_30fps*30100)/fps;
		params->tVts_reg[0].data = (params->expo.vts >> 8) & 0x00ff;
		params->tVts_reg[1].data = (params->expo.vts >> 0) & 0x00ff;
        params->reg_dirty = 1; //reg need to update = true;
		return SUCCESS;
	}
	else
	{
		SENSOR_EMSG("[%s] FPS %u out of range.\n",__FUNCTION__,fps);
		return FAIL;
	}
}

///////////////////////////////////////////////////////////////////////
// auto exposure
///////////////////////////////////////////////////////////////////////
// unit: micro seconds
//AE status notification
static int Imx323AEStatusNotify(ms_cus_sensor *handle, CUS_CAMSENSOR_AE_STATUS_NOTIFY status)
{
#if __ENABLE_AE__
    imx323_params *params = (imx323_params *)handle->private_data;
    //SENSOR_DMSG("[%s]\n", __FUNCTION__);
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            //SensorReg_Write(0x3001,0);
            break;
        case CUS_FRAME_ACTIVE:
            if(params->reg_dirty)
            {
                SensorReg_Write(0x3001,1);
                SensorRegArrayW((I2C_ARRAY*)params->tExpo_reg, sizeof(expo_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tGain_reg, sizeof(gain_reg)/sizeof(I2C_ARRAY));
                SensorRegArrayW((I2C_ARRAY*)params->tVts_reg, sizeof(vts_reg)/sizeof(I2C_ARRAY));
                SensorReg_Write(0x3001,0);
                params->reg_dirty = 0;
            }
            break;
        default :
            break;
    }
#endif
    return SUCCESS;
}

static int Imx323GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    imx323_params *params = (imx323_params *)handle->private_data;
    u32 lines = 0;
    lines |= (u32)(params->tExpo_reg[0].data&0xff)<<8;
    lines |= (u32)(params->tExpo_reg[1].data&0xff)<<0;
    // lines >>= 4;
    // *us = (lines+dummy) * params->expo.us_per_line;
    *us = lines;//(lines*Preview_line_period);
    SENSOR_DMSG("[%s] sensor expo lines/us 0x%x,0x%x us\n", __FUNCTION__, lines, *us);
    return SUCCESS;
}

static int Imx323SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    u32 lines = 0, vts = 0;
    imx323_params *params = (imx323_params *)handle->private_data;

    lines=(1000*us)/Preview_line_period;

    if(lines<1) lines=1;

    if (lines >params->expo.vts-1)
    {
    	vts = lines +1;
    }
    else
    vts=params->expo.vts;

    // lines=us/Preview_line_period;
    //SENSOR_DMSG("[%s] us %ld, lines %ld, vts %ld\n", __FUNCTION__,us,lines,params->expo.vts);
    lines=vts-lines-1;
    //params->tExpo_reg[0].data = (lines>>16) & 0x0003;
    params->tExpo_reg[0].data = (lines>>8) & 0x00ff;
    params->tExpo_reg[1].data = (lines>>0) & 0x00ff;

    // vts_reg[0].data = (vts >> 16) & 0x0003;
    params->tVts_reg[0].data = (vts >> 8) & 0x00ff;
    params->tVts_reg[1].data = (vts >> 0) & 0x00ff;

    params->reg_dirty = 1; //reg need to update
    return SUCCESS;

}

const u32 imx322_GainTable[151] = {
    1024,
    1060,
    1097,
    1136,
    1176,
    1218,
    1261,
    1305,
    1351,
    1399,
    1448,
    1499,
    1552,
    1607,
    1663,
    1722,
    1783,
    1846,
    1911,
    1978,
    2048,
    2120,
    2195,
    2272,
    2353,
    2435,
    2521,
    2610,
    2702,
    2798,
    2896,
    2998,
    3104,
    3214,
    3327,
    3444,
    3566,
    3692,
    3822,
    3956,
    4096,
    4240,
    4390,
    4545,
    4705,
    4871,
    5043,
    5221,
    5405,
    5595,
    5793,
    5997,
    6208,
    6427,
    6654,
    6889,
    7132,
    7383,
    7643,
    7913,
    8192,
    8481,
    8780,
    9090,
    9410,
    9742,
    10086,
    10441,
    10809,
    11191,
    11585,
    11994,
    12417,
    12855,
    13308,
    13777,
    14263,
    14766,
    15287,
    15826,
    16384,
    16962,
    17560,
    18179,
    18820,
    19484,
    20171,
    20882,
    21619,
    22381,
    23170,
    23988,
    24834,
    25709,
    26616,
    27554,
    28526,
    29532,
    30574,
    31652,
    32768,
    33924,
    35120,
    36358,
    37641,
    38968,
    40342,
    41765,
    43238,
    44762,
    46341,
    47975,
    49667,
    51419,
    53232,
    55109,
    57052,
    59064,
    61147,
    63304,
    65536,
    67847,
    70240,
    72717,
    75281,
    77936,
    80684,
    83530,
    86475,
    89525,
    92682,
    95950,
    99334,
    102837,
    106464,
    110218,
    114105,
    118129,
    122295,
    126607,
    131072,
    135694,
    140480,
    145434,
    150562,
    155872,
    161369,
    167059,
    172951,
    179050,
    185364
};

// Gain: 1x = 1024
static int Imx323GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    //u16 temp_gain;
    //double temp_gain_double;

    //temp_gain=params->tGain_reg[0].data;
    //temp_gain_double=((double)(temp_gain*3)/200.0f);
    //*gain=(u32)(pow(10,temp_gain_double)*1024);

    //SENSOR_DMSG("[%s] get gain/reg (1024=1X)= 0x%x/0x%x\n", __FUNCTION__, *gain,params->tGain_reg[0].data);
    return SUCCESS;
}

static int Imx323SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    imx323_params *params = (imx323_params *)handle->private_data;
    int i, sensor_gain = 1024;

    params->expo.final_gain = gain;

    if(gain<1024)
        gain=1024;
    else if(gain>=177*1024)
        gain=177*1024;

    for( i = 0; i< ARRAY_SIZE(imx322_GainTable); i++)
    {
        if (gain >= imx322_GainTable[i])
            sensor_gain = i;
        else
            break;
    }

    params->tGain_reg[0].data = sensor_gain;
    params->reg_dirty = 1;    //reg need to update
    return 0;
}

static int Imx323GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	*min = 1;
	*max = 1000000/4;
	return SUCCESS;
}

static int Imx323GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	*min = 1024;
	*max = 177*1024;
	return SUCCESS;
}

static int Imx323GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
	SENSOR_DMSG("[%s]\n", __FUNCTION__);
	info->max = 1000000000/Preview_MIN_FPS;
	info->min = Preview_line_period * 5;
	info->step = Preview_line_period;
	return SUCCESS;
}

int Imx323ReleaseHandle(ms_cus_sensor *handle)
{
    return SUCCESS;
}

int Imx323InitHandle(ms_cus_sensor* handle)
{
    imx323_params *params = 0;
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
    params = (imx323_params *)handle->private_data;
    memcpy(params->tVts_reg, vts_reg, sizeof(vts_reg));
    memcpy(params->tGain_reg, gain_reg, sizeof(gain_reg));
    memcpy(params->tExpo_reg, expo_reg, sizeof(expo_reg));

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"IMX323_PARL");

    ////////////////////////////////////
    //    sensor interface info       //
    ////////////////////////////////////
    //SENSOR_DMSG("[%s] entering function with id %d\n", __FUNCTION__, id);
    //handle->isp_type    = SENSOR_ISP_TYPE;  //ISP_SOC;
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
    handle->video_res_supported.res[0].width = 1920;
    handle->video_res_supported.res[0].height = 1080;
    handle->video_res_supported.res[0].max_fps= 30;
    handle->video_res_supported.res[0].min_fps= 3;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 0x882;
    handle->video_res_supported.res[0].nOutputHeight= 0x464;
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
    handle->sat_mingain=g_sensor_ae_min_gain;
    handle->pCus_sensor_release     = Imx323ReleaseHandle;
    handle->pCus_sensor_init        = Imx323Init;
    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    //handle->pCus_sensor_GetSensorID     = Imx323GetSensorID   ;

    handle->pCus_sensor_GetVideoResNum = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes  = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;

    handle->pCus_sensor_GetOrien        = Imx323GetOrien      ;
    handle->pCus_sensor_SetOrien        = Imx323SetOrien      ;
    handle->pCus_sensor_GetFPS          = Imx323GetFPS      ;
    handle->pCus_sensor_SetFPS          = Imx323SetFPS      ;
    handle->pCus_sensor_SetPatternMode  = Imx323SetPatternMode;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify  = Imx323AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs      = Imx323GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs      = Imx323SetAEUSecs;
    handle->pCus_sensor_GetAEGain       = Imx323GetAEGain;
    handle->pCus_sensor_SetAEGain       = Imx323SetAEGain;
    handle->pCus_sensor_GetShutterInfo  = Imx323GetShutterInfo;

    handle->pCus_sensor_GetAEMinMaxGain = Imx323GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs= Imx323GetAEMinMaxUSecs;

    params->expo.vts=vts_30fps;
    params->expo.fps = 30;
    params->reg_dirty = 0;

    return SUCCESS;
}

SENSOR_DRV_ENTRY_IMPL_END_EX(  IMX323,
                            Imx323InitHandle,
                            NULL,
                            NULL,
                            imx323_params
                         );
