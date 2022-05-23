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
#include <mdrv_gpio_io.h>


#ifdef __cplusplus
}
#endif

SENSOR_DRV_ENTRY_IMPL_BEGIN_EX(TC358743XBG_MIPI);

#define SENSOR_CHANNEL_NUM (0)
//#define SENSOR_CHANNEL_NUM (1)
//#define SENSOR_CHANNEL_NUM (2)
#define SENSOR_CHANNEL_MODE CUS_SENSOR_CHANNEL_MODE_RAW_STORE
//============================================
//MIPI config

//============================================
//============================================
//BT656 config begin.
#define SENSOR_BT656_CH_TOTAL_NUM (1)
#define SENSOR_BT656_CH_DET_ENABLE CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_CH_DET_SEL CUS_VIF_BT656_EAV_DETECT
#define SENSOR_BT656_BIT_SWAP CUS_SENSOR_FUNC_DISABLE
#define SENSOR_BT656_8BIT_MODE CUS_SENSOR_FUNC_ENABLE
#define SENSOR_BT656_VSYNC_DELAY CUS_VIF_BT656_VSYNC_DELAY_AUTO
#define SENSOR_HSYCN_INVERT CUS_SENSOR_FUNC_DISABLE
#define SENSOR_VSYCN_INVERT CUS_SENSOR_FUNC_DISABLE
#define SENSOR_CLAMP_ENABLE CUS_SENSOR_FUNC_DISABLE
//BT656 config end.
//============================================


///////////////////////////////////////////////////////////////
//          @@@                                                                                       //
//       @   @@      ==  S t a r t * H e r e ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                            //
//            @@      ==  S t a r t * H e r e  ==                                           //
//         @@@@                                                                                  //
//                                                                                                     //
//      Start Step 1 --  show preview on LCM                                         //
//                                                                                                    �@//
//  Fill these #define value and table with correct settings                        //
//      camera can work and show preview on LCM                                 //
//                                                                                                       //
///////////////////////////////////////////////////////////////

#define SENSOR_ISP_TYPE     ISP_EXT                   //ISP_EXT, ISP_SOC
#define F_number  22                                  // CFG, demo module
//#define SENSOR_DATAFMT      CUS_DATAFMT_BAYER        //CUS_DATAFMT_YUV, CUS_DATAFMT_BAYER
#define SENSOR_IFBUS_TYPE   CUS_SENIF_BUS_MIPI      //CFG //CUS_SENIF_BUS_PARL, CUS_SENIF_BUS_MIPI
//#define SENSOR_MIPI_DELAY   0x1212                  //CFG
#define SENSOR_MIPI_HSYNC_MODE PACKET_HEADER_EDGE1
#define SENSOR_DATAPREC     CUS_DATAPRECISION_16    //CUS_DATAPRECISION_8, CUS_DATAPRECISION_10
#define SENSOR_DATAMODE     CUS_SEN_10TO12_9000     //CFG
#define SENSOR_BAYERID      CUS_BAYER_RG            //CFG //CUS_BAYER_GB, CUS_BAYER_GR, CUS_BAYER_BG, CUS_BAYER_RG
#define SENSOR_RGBIRID      CUS_RGBIR_NONE
#define SENSOR_ORIT         CUS_ORIT_M0F0           //CUS_ORIT_M0F0, CUS_ORIT_M1F0, CUS_ORIT_M0F1, CUS_ORIT_M1F1,
//#define SENSOR_YCORDER      CUS_SEN_YCODR_YC       //CUS_SEN_YCODR_YC, CUS_SEN_YCODR_CY
#define lane_number 4
#define vc0_hs_mode 3   //0: packet header edge  1: line end edge 2: line start edge 3: packet footer edge
#define long_packet_type_enable 0x00 //UD1~UD8 (user define)

#define Preview_MCLK_SPEED  CUS_CMU_CLK_27MHZ       //CFG //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Preview_line_period 30000                  ////HTS/PCLK=4455 pixels/148.5MHZ=30usec
#define Prv_Max_line_number 1080                    //maximum exposure line munber of sensor when preview
#define vts_30fps 1125//1346,1616                      //for 29.1fps
#define Preview_WIDTH       1280                    //resolution Width when preview
#define Preview_HEIGHT      720                    //resolution Height when preview
#define Preview_MAX_FPS     60                     //fastest preview FPS
#define Preview_MIN_FPS     8                      //slowest preview FPS
#define Preview_CROP_START_X     0                      //CROP_START_X
#define Preview_CROP_START_Y     0                      //CROP_START_Y

//#define Capture_MCLK_SPEED  CUS_CMU_CLK_16M     //CUS_CMU_CLK_12M, CUS_CMU_CLK_16M, CUS_CMU_CLK_24M, CUS_CMU_CLK_27M
#define Cap_Max_line_number 720                   //maximum exposure line munber of sensor when capture

#define SENSOR_I2C_LEGACY  I2C_NORMAL_MODE     //usally set CUS_I2C_NORMAL_MODE,  if use old OVT I2C protocol=> set CUS_I2C_LEGACY_MODE
#define SENSOR_I2C_FMT     I2C_FMT_A16D8        //CUS_I2C_FMT_A8D8, CUS_I2C_FMT_A8D16, CUS_I2C_FMT_A16D8, CUS_I2C_FMT_A16D16
#define SENSOR_I2C_ADDR     0x1E                   //I2C slave address
#define SENSOR_I2C_SPEED       200000     //200KHz
#define SENSOR_I2C_CHANNEL     1                            //I2C Channel
#define SENSOR_I2C_PAD_MODE 2                            //Pad/Mode Number

#define SENSOR_PWDN_POL     CUS_CLK_POL_NEG//CUS_CLK_POL_NEG        // if PWDN pin High can makes sensor in power down, set CUS_CLK_POL_POS
#define SENSOR_RST_POL      CUS_CLK_POL_NEG        // if RESET pin High can makes sensor in reset state, set CUS_CLK_POL_NEG

// VSYNC/HSYNC POL can be found in data sheet timing diagram,
// Notice: the initial setting may contain VSYNC/HSYNC POL inverse settings so that condition is different.

#define SENSOR_VSYNC_POL    CUS_CLK_POL_POS        // if VSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_HSYNC_POL    CUS_CLK_POL_POS        // if HSYNC pin High and data bus have data, set CUS_CLK_POL_POS
#define SENSOR_PCLK_POL     CUS_CLK_POL_POS        // depend on sensor setting, sometimes need to try CUS_CLK_POL_POS or CUS_CLK_POL_NEG
//static int  drv_Fnumber = 22;

#define RX_TEST_PATTERN     0
#define NEW_PIN_DEFINE      1


#define WAKE_UP(waitqueue)  \
    do{   \
            CamOsTcondSignalAll(&(waitqueue)); \
    }while(0)

static inline unsigned long time_diff(unsigned long start, unsigned long end)
{
#ifdef CAM_OS_RTK
   if(end<start)
       return   (unsigned long)(0xFFFFFFFF/6000 - ( long)start + (long)end);
   else
       return   (unsigned long)(( long)end - (long)start);

#else
    return   (unsigned long)(( long)end - (long)start);
#endif
}

#define WaitEventTimeout(Waitqueue,   condition,    s32TimeOutMs)  \
    do{  \
        u32 u32BegTick = jiffies;  \
        u32 u32CurTick;   \
    while(1)  \
    {  \
        if(condition)  \
            break;  \
       CamOsTcondTimedWait(&(Waitqueue),(s32TimeOutMs));  \
       u32CurTick =jiffies;   \
       if(time_diff(u32BegTick,u32CurTick)>msecs_to_jiffies(s32TimeOutMs))  \
          break;  \
    }  \
    }while(0)

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain);
static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us);
int tc358743xbg_cus_camsensor_release_handle(ms_cus_sensor *handle);
int  Tc358743_IsrThread(void *args);
void interrupt_func(u32 eIntNum, void* p1);

typedef enum
{
    E_S0_STATUS_POWEROFF,
    E_RS1_STATUS_SETEDID,
    E_RS2_STATUS_SETHPDO,
    E_RS3_STATUS_ENMIPIOUT,
    E_RS4_STATUS_NOSIGNAL,
    E_RS5_STATUS_NOCONNECTION,
}Tcxbg_state_e;

typedef struct
{
    unsigned char bPcmData;
    unsigned char ucharBitWidth;
    unsigned char ucharChannels;
    unsigned int uintSampleRate;
}Tcxbg_audio_info_t;

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
        u32 expo_lines;
        u32 expo_lef_us;
        u32 expo_sef_us;
    } expo;

    int sen_init;
    int still_min_fps;
    int video_min_fps;
    u32 max_rhs1;
    u32 lef_shs2;
    u32 skip_cnt;
    bool dirty;
    bool change;
    I2C_ARRAY tVts_reg[3];
    I2C_ARRAY tGain_reg[3];
    I2C_ARRAY tExpo_reg[3];
    I2C_ARRAY tShs2_reg[3];
    I2C_ARRAY tRhs1_reg[3];
    I2C_ARRAY tGain_hdr_dol_lef_reg[1];
    I2C_ARRAY tGain_hdr_dol_sef_reg[1];
    bool orien_dirty;

    int         irqnumber;
    bool        bIrqFlag;
    bool        bEnCsiOut;
    CamOsThread  IsrTaskHandle;
    CamOsTcond_t IsrTaskwaitqueue;

    CamOsThread  WorkTaskHandle;
    CamOsTcond_t WorkTaskwaitqueue;

    Tcxbg_state_e estate;
} tc358743xbg_params;

typedef enum
{
    EN_CUST_CMD_GET_STATE,
    EN_CUST_CMD_GET_AUDIO_INFO,
    EN_CUST_CMD_MAX
}EN_TC358743_SENSOR_STATE;

#if 0
static I2C_ARRAY TriggerStartTbl[] =
{

};

static I2C_ARRAY PatternTbl[] =
{

};
#endif
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

typedef struct
{
    short reg;
    char startbit;
    char stopbit;
} COLLECT_REG_SET;

#if 0
static I2C_ARRAY mirror_reg[] =
{

};

static I2C_ARRAY gain_reg[] =
{

};
#endif
static int g_sensor_ae_min_gain = 1024;
static CUS_GAIN_GAP_ARRAY gain_gap_compensate[16] =
{

};
#if 0
static I2C_ARRAY expo_reg[] =
{

};

static I2C_ARRAY vts_reg[] =
{

};
#endif

/////////// function definition ///////////////////
#define SensorReg_Read(_reg,_data)     (handle->i2c_bus->i2c_rx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorReg_Write(_reg,_data)    (handle->i2c_bus->i2c_tx(handle->i2c_bus, &(handle->i2c_cfg),_reg,_data))
#define SensorRegArrayW(_reg,_len)  (handle->i2c_bus->i2c_array_tx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorRegArrayR(_reg,_len)  (handle->i2c_bus->i2c_array_rx(handle->i2c_bus, &(handle->i2c_cfg),(_reg),(_len)))
#define SensorReg_WriteBurst(burst, len)  (handle->i2c_bus->i2c_burst_tx(handle->i2c_bus, &(handle->i2c_cfg), &(burst), len))
#define SensorReg_ReadBurst(burst, len)  (handle->i2c_bus->i2c_burst_rx(handle->i2c_bus, &(handle->i2c_cfg), &(burst), len))


static s32 i2c_write8(u16 reg, u8 xbg_data, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;

    burst.reg = reg;
    burst.data = &xbg_data;
    return SensorReg_WriteBurst(burst, 1);
}
static s32 i2c_write16(u16 reg, u16 xbg_data, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;

    burst.reg = reg;
    burst.data = &xbg_data;
    return SensorReg_WriteBurst(burst, 2);
}
static s32 i2c_write32(u16 reg, u32 xbg_data, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;

    burst.reg = reg;
    burst.data = &xbg_data;
    return SensorReg_WriteBurst(burst, 4);
}
static char i2c_read8(u16 reg, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;
    u8 ret_data = 0;

    burst.reg = reg;
    burst.data = (void *)&ret_data;
    SensorReg_ReadBurst(burst, 1);

    return ret_data;
}
u16 i2c_read16(u16 reg, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;
    u16 ret_data = 0;

    burst.reg = reg;
    burst.data = (void *)&ret_data;
    SensorReg_ReadBurst(burst, 2);

    return ret_data;
}
u32 i2c_read32(u16 reg, ms_cus_sensor *handle)
{
    I2C_ARRAY_BURST burst;
    u32 ret_data = 0;

    burst.reg = reg;
    burst.data = (void *)&ret_data;
    SensorReg_ReadBurst(burst, 4);

    return ret_data;
}
#define i2c1_h2c_write8(reg, data) i2c_write8(reg, data, handle);
#define i2c1_h2c_write16(reg, data) i2c_write16(reg, data, handle);
#define i2c1_h2c_write32(reg, data) i2c_write32(reg, data, handle);
#define i2c1_h2c_read8(reg) i2c_read8(reg, handle);
#define i2c1_h2c_read16(reg) i2c_read16(reg, handle);
#define i2c1_h2c_read32(reg) i2c_read32(reg, handle);

static int Tc358743_EnableIntAndCrateThreat(ms_cus_sensor *handle)
{
    int irq = -1;
    int ret = 0;
    CamOsThreadAttrb_t IsrtAttr = {0};
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;

#ifdef CONFIG_SIGMASTAR_CHIP_I6E
    #define PAD_GPIO9 (108)
    #define IRQGPIO (PAD_GPIO9)
#else
    #define PAD_PWM0 (52)
    #define IRQGPIO (PAD_PWM0)
#endif


    camdriver_gpio_request(NULL, IRQGPIO);
    irq = camdriver_gpio_to_irq(NULL, IRQGPIO);

    ret = CamOsIrqRequest(irq, interrupt_func, "tc358743", params);
    if (ret != 0)
    {
        CamOsPrintf("request irq fail..!\n");
    }

    params->irqnumber = irq;

    params->bIrqFlag = 0;
    CamOsTcondInit(&params->IsrTaskwaitqueue);
    CamOsTcondInit(&params->WorkTaskwaitqueue);

    IsrtAttr.szName = "tc358743isrthread";
    IsrtAttr.nPriority = 70;
    IsrtAttr.nStackSize = 2*1024;
    CamOsThreadCreate(&params->IsrTaskHandle, &IsrtAttr, (void *)Tc358743_IsrThread, handle);
/*
    CamOsThreadAttrb_t WorktAttr = {0};
    WorktAttr.szName = "tc358743workthread";
    WorktAttr.nPriority = 70;
    WorktAttr.nStackSize = 2*1024;
    CamOsThreadCreate(&params->WorkTaskHandle, &WorktAttr, (void *)Tc358743_WorkThread, handle);
*/
    CamOsPrintf("Tc358743 power on!\n");

    return SUCCESS;
}

static int Tc358743_DisIntAndDestroyThreat(ms_cus_sensor *handle)
{
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;

    CamOsIrqFree(params->irqnumber, params);

    CamOsThreadStop(params->IsrTaskHandle);
    CamOsThreadStop(params->WorkTaskHandle);

    CamOsTcondDeinit(&params->IsrTaskwaitqueue);
    CamOsTcondDeinit(&params->WorkTaskwaitqueue);
    if (params->estate == E_RS3_STATUS_ENMIPIOUT)
    {
        i2c1_h2c_write16(0x0004,0x0CD4); // ConfCtl
    }
    if (params->estate == E_RS2_STATUS_SETHPDO
        || params->estate == E_RS4_STATUS_NOSIGNAL)
    {
        i2c1_h2c_write16(0x0002,0x0001); // SysCtl  enter sleep
    }
    params->estate = E_RS1_STATUS_SETEDID;
    CamOsPrintf("Tc358743 power off!\n");

    return SUCCESS;
}


/////////////////// sensor hardware dependent //////////////
static int pCus_poweron(ms_cus_sensor *handle, u32 idx)
{
    ISensorIfAPI *sensor_if = handle->sensor_if_api;

    //Sensor power on sequence
    //sensor_if->SetIOPad(idx, handle->sif_bus, 0);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);        //Set_csi_if(0, 0); //disable MIPI
    //
    sensor_if->SetIOPad(idx, handle->sif_bus, handle->interface_attr.attr_mipi.mipi_lane_num);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_216M);
    sensor_if->SetCSI_Lane(idx, handle->interface_attr.attr_mipi.mipi_lane_num, 1);
    sensor_if->SetCSI_LongPacketType(idx, 0x4000, 0x0, 0);

    sensor_if->MCLK(idx, 1, handle->mclk);
    //
    sensor_if->Reset(idx, handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    sensor_if->Reset(idx, !handle->reset_POLARITY);
    SENSOR_MSLEEP(5);
    sensor_if->PowerOff(idx, !handle->pwdn_POLARITY);
    SENSOR_MSLEEP(5);

    Tc358743_EnableIntAndCrateThreat(handle);

    return SUCCESS;
}

static int pCus_poweroff(ms_cus_sensor *handle, u32 idx)
{

    ISensorIfAPI *sensor_if = handle->sensor_if_api;
    SENSOR_DMSG("[%s] power low\n", __FUNCTION__);
    sensor_if->PowerOff(idx, handle->pwdn_POLARITY);

    SENSOR_MSLEEP(1);
    //sensor_if->SetCSI_Clk(CUS_CSI_CLK_DISABLE);        //Set_csi_if(0, 0);
    sensor_if->MCLK(idx, 0, handle->mclk);
    sensor_if->SetCSI_Clk(idx, CUS_CSI_CLK_DISABLE);

    Tc358743_DisIntAndDestroyThreat(handle);

    return SUCCESS;
}

/////////////////// image function /////////////////////////
//Get and check sensor ID
//if i2c error or sensor id does not match then return FAIL
static int pCus_GetSensorID(ms_cus_sensor *handle, u32 *id)
{
    return SUCCESS;
}

static int tc358743xbg_SetPatternMode(ms_cus_sensor *handle,u32 mode)
{
    return SUCCESS;
}
static void tc358743xbg_GetInputInfo(ms_cus_sensor *handle)
{
    u16 rdata16 = 0;
    u16 width = 0, height = 0;

    // Check HDMI resolution
    handle->video_res_supported.num_res =1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.

    rdata16=i2c1_h2c_read8(0x852E); // PX_FREQ0
    CamOsPrintf("PX_FREQ0: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x852F); // PX_FREQ1
    CamOsPrintf("PX_FREQ1: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x858A); // H_SIZE0
    CamOsPrintf("H_SIZE0: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x858B); // H_SIZE1
    CamOsPrintf("H_SIZE1: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8582); // DE_WIDTH_H0
    CamOsPrintf("DE_WIDTH_H0: 0x%x\n", rdata16);
    width =  rdata16;
    rdata16=i2c1_h2c_read8(0x8583); // DE_WIDTH_H1    
    width |=  (rdata16 << 8);
    CamOsPrintf("DE_WIDTH_H1: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x858C); // V_SIZE0
    CamOsPrintf("V_SIZE0: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x858D); // V_SIZE1
    CamOsPrintf("V_SIZE1: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8588); // DE_WIDTH_V0
    height =  rdata16;
    CamOsPrintf("DE_WIDTH_V0: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8589); // DE_WIDTH_V1
    height |= (rdata16 << 8);
    CamOsPrintf("DE_WIDTH_V1: 0x%x\n", rdata16);
    // Check HDMI format
    rdata16=i2c1_h2c_read8(0x8528); // VI_STATUS3
    CamOsPrintf("VI_STATUS3: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8522); // VI_STATUS
    CamOsPrintf("VI_STATUS: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8525); // VI_STATUS2
    CamOsPrintf("VI_STATUS2: 0x%x\n", rdata16);
    rdata16=i2c1_h2c_read8(0x8526); // CLK_STATUS
    CamOsPrintf("CLK_STATUS: 0x%x\n", rdata16);
    handle->video_res_supported.res[0].width = width;
    handle->video_res_supported.res[0].height = height;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= width;
    handle->video_res_supported.res[0].nOutputHeight= height;
    sprintf(handle->video_res_supported.res[0].strResDesc, "%dx%d", width, height);

}
static void tc358743xbg_EnableCsiOut(ms_cus_sensor *handle)
{
    u16 rdata8 = 0;

    // Check HDMI resolution/format
    // MIPI Output Setting
    // Stop Video and Audio
    i2c1_h2c_write16(0x0004,0x0CD4); // ConfCtl
    // Reset CSI-TX Block, Enter Sleep mode
    i2c1_h2c_write16(0x0002,0x0200); // SysCtl
    i2c1_h2c_write16(0x0002,0x0000); // SysCtl
    // PLL Setting in Sleep mode, Int clear
    i2c1_h2c_write16(0x0002,0x0001); // SysCtl
    i2c1_h2c_write16(0x0020,0x306D); // PLLCtl0
    i2c1_h2c_write16(0x0022,0x0203); // PLLCtl1
    SENSOR_USLEEP(1);
    i2c1_h2c_write16(0x0022,0x0213); // PLLCtl1
    i2c1_h2c_write16(0x0002,0x0000); // SysCtl
    SENSOR_USLEEP(10);
    // Video Setting
    i2c1_h2c_write8(0x8573,0xC1); // VOUT_SET2
    i2c1_h2c_write8(0x8574,0x08); // VOUT_SET3
    i2c1_h2c_write8(0x8576,0xA0); // VI_REP
    // FIFO Delay Setting
    i2c1_h2c_write16(0x0006,0x015E); // FIFO Ctl
    // Special Data ID Setting.
    // CSI Lane Enable
    i2c1_h2c_write32(0x0140,0x00000000); // CLW_CNTRL
    i2c1_h2c_write32(0x0144,0x00000000); // D0W_CNTRL
    i2c1_h2c_write32(0x0148,0x00000000); // D1W_CNTRL
    i2c1_h2c_write32(0x014C,0x00000000); // D2W_CNTRL
    i2c1_h2c_write32(0x0150,0x00000000); // D3W_CNTRL
    // CSI Transition Timing
    i2c1_h2c_write32(0x0210,0x00002EE0); // LINEINITCNT
    i2c1_h2c_write32(0x0214,0x00000004); // LPTXTIMECNT
    i2c1_h2c_write32(0x0218,0x00002403); // TCLK_HEADERCNT
    i2c1_h2c_write32(0x021C,0x00000003); // TCLK_TRAILCNT
    i2c1_h2c_write32(0x0220,0x00000305); // THS_HEADERCNT
    i2c1_h2c_write32(0x0224,0x00006978); // TWAKEUP
    i2c1_h2c_write32(0x0228,0x0000000C); // TCLK_POSTCNT
    i2c1_h2c_write32(0x022C,0x00000004); // THS_TRAILCNT
    i2c1_h2c_write32(0x0230,0x00000005); // HSTXVREGCNT
    i2c1_h2c_write32(0x0234,0x0000001F); // HSTXVREGEN
    i2c1_h2c_write32(0x0238,0x00000000); // TXOPTIONACNTRL
    i2c1_h2c_write32(0x0204,0x00000001); // STARTCNTRL
    i2c1_h2c_write32(0x0518,0x00000001); // CSI_START
    i2c1_h2c_write32(0x0500,0xA3008087); // CSI_CONFW
    // Enable Interrupt
    i2c1_h2c_write8(0x8502,0xFF); // SYS_INTS_C
    i2c1_h2c_write8(0x8503,0x7F); // CLK_INTS_C
    i2c1_h2c_write8(0x8504,0xFF); // PACKET_INTS_C
    i2c1_h2c_write8(0x8505,0xFF); // AUDIO_INTS_C
    i2c1_h2c_write8(0x8506,0xFF); // ABUF_INTS_C
    i2c1_h2c_write8(0x850B,0x1F); // MISC_INTS_C
    i2c1_h2c_write16(0x0014,0x073F); // TOP_INTS_C
    i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM
    // Start CSI output
    i2c1_h2c_write16(0x0004,0x0CD7); // ConfCtl

#if 1 // left justified
    // I2s config
    rdata8 = i2c1_h2c_read8(0x8652); // SDO_MODE1
    rdata8 = ((rdata8 & 0xFC) | 0x01); //SDO_MODE1.SDO_FMT = 2'b01
    CamOsPrintf("0x8652 = 0x%x\n", rdata8);
    i2c1_h2c_write8(0x8652, rdata8);
    i2c1_h2c_write16(0x0004,0x0CD7); // ConfCtl.I2SDlyopt = 1'b0
    rdata8 = i2c1_h2c_read8(0x8651); // SDO_MODE1
    rdata8 = ((rdata8 & 0xFE) | 0x1); //SDO_MODE0.LR_POL = 1'b1)
    CamOsPrintf("0x8651 = 0x%x\n", rdata8);
    i2c1_h2c_write8(0x8651, rdata8);
#else
    // I2s config
    rdata8 = i2c1_h2c_read8(0x8652); // SDO_MODE1
    rdata8 = ((rdata8 & 0xFC) | 0x10); //SDO_MODE1.SDO_FMT = 2'b10
    CamOsPrintf("0x8652 = 0x%x\n", rdata8);
    i2c1_h2c_write8(0x8652, rdata8);
    i2c1_h2c_write16(0x0004,0x0DD7); // ConfCtl.I2SDlyopt = 1'b1
    rdata8 = i2c1_h2c_read8(0x8651); // SDO_MODE1
    rdata8 = (rdata8 & 0xFE); //SDO_MODE0.LR_POL = 1'b0
    CamOsPrintf("0x8651 = 0x%x\n", rdata8);
    i2c1_h2c_write8(0x8651, rdata8);

#endif

}

static void tc358743xbg_InitTable(ms_cus_sensor *handle)
{
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;
    u8 rdata8 = 0;
    u8 i = 0;

    // Initialization for Stand-by (RS1)
    // Software Reset
    i2c1_h2c_write16(0x0002,0x0F00); // SysCtl
    i2c1_h2c_write16(0x0002,0x0000); // SysCtl
    // PLL Setting
    i2c1_h2c_write16(0x0020,0x3057); // PLLCtl0
    i2c1_h2c_write16(0x0022,0x0203); // PLLCtl1
    SENSOR_USLEEP(10);
    i2c1_h2c_write16(0x0022,0x0213); // PLLCtl1
    // HDMI Interrupt Control
    i2c1_h2c_write16(0x0016,0x073F); // TOP_INTM
    i2c1_h2c_write8(0x8502,0xFF); // SYS_INTS_C
    i2c1_h2c_write8(0x850B,0x1F); // MISC_INTS_C
    i2c1_h2c_write16(0x0014,0x073F); // TOP_INTS_C
    i2c1_h2c_write8(0x8512,0xFE); // SYS_INTM
    i2c1_h2c_write8(0x851B,0x1D); // MISC_INTM
    // HDMI PHY
    i2c1_h2c_write8(0x8532,0x80); // PHY CTL1
    i2c1_h2c_write8(0x8536,0x40); // PHY_BIAS
    i2c1_h2c_write8(0x853F,0x0A); // PHY_CSQ
    i2c1_h2c_write8(0x8537,0x02); // PHY_EQ
    // HDMI SYSTEM
    i2c1_h2c_write8(0x8543,0x32); // DDC_CTL
    i2c1_h2c_write8(0x8544,0x10); // HPD_CTL
    i2c1_h2c_write8(0x8545,0x31); // ANA_CTL
    i2c1_h2c_write8(0x8546,0x2D); // AVM_CTL
    // HDCP Setting
    i2c1_h2c_write8(0x85D1,0x01); //
    i2c1_h2c_write8(0x8560,0x24); // HDCP_MODE
    i2c1_h2c_write8(0x8563,0x11); //
    i2c1_h2c_write8(0x8564,0x0F); //
    // HDMI Audio REFCLK
    i2c1_h2c_write8(0x8531,0x01); // PHY_CTL0
    i2c1_h2c_write8(0x8532,0x80); // PHY_CTL1
    i2c1_h2c_write8(0x8540,0x8C); // SYS_FREQ0
    i2c1_h2c_write8(0x8541,0x0A); // SYS_FREQ1
    i2c1_h2c_write8(0x8630,0xB0); // LOCKDET_REF0
    i2c1_h2c_write8(0x8631,0x1E); // LOCKDET_REF1
    i2c1_h2c_write8(0x8632,0x04); // LOCKDET_REF2
    i2c1_h2c_write8(0x8670,0x01); // NCO_F0_MOD
    // HDMI Audio Setting
    i2c1_h2c_write8(0x8600,0x00); // AUD_Auto_Mute
    i2c1_h2c_write8(0x8602,0xF3); // Auto_CMD0
    i2c1_h2c_write8(0x8603,0x02); // Auto_CMD1
    i2c1_h2c_write8(0x8604,0x0C); // Auto_CMD2
    i2c1_h2c_write8(0x8606,0x05); // BUFINIT_START
    i2c1_h2c_write8(0x8607,0x00); // FS_MUTE
    i2c1_h2c_write8(0x8620,0x22); // FS_IMODE
    i2c1_h2c_write8(0x8640,0x01); // ACR_MODE
    i2c1_h2c_write8(0x8641,0x65); // ACR_MDF0
    i2c1_h2c_write8(0x8642,0x07); // ACR_MDF1
    i2c1_h2c_write8(0x8652,0x02); // SDO_MODE1
    i2c1_h2c_write8(0x85AA,0x50); // FH_MIN0
    i2c1_h2c_write8(0x85AF,0xC6); // HV_RST
    i2c1_h2c_write8(0x85AB,0x00); // FH_MIN1
    i2c1_h2c_write8(0x8665,0x10); // DIV_MODE
    // Info Frame Extraction
    i2c1_h2c_write8(0x8709,0xFF); // PK_INT_MODE
    i2c1_h2c_write8(0x870B,0x2C); // NO_PKT_LIMIT
    i2c1_h2c_write8(0x870C,0x53); // NO_PKT_CLR
    i2c1_h2c_write8(0x870D,0x01); // ERR_PK_LIMIT
    i2c1_h2c_write8(0x870E,0x30); // NO_PKT_LIMIT2
    i2c1_h2c_write8(0x9007,0x10); // NO_GDB_LIMIT
    // EDID
    i2c1_h2c_write8(0x85C7,0x01); // EDID_MODE
    i2c1_h2c_write8(0x85CA,0x00); // EDID_LEN1
    i2c1_h2c_write8(0x85CB,0x01); // EDID_LEN2
    // EDID Data
    i2c1_h2c_write8(0x8C00,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C01,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C02,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C03,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C04,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C05,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C06,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C07,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C08,0x52); // EDID_RAM
    i2c1_h2c_write8(0x8C09,0x62); // EDID_RAM
    i2c1_h2c_write8(0x8C0A,0x88); // EDID_RAM
    i2c1_h2c_write8(0x8C0B,0x88); // EDID_RAM
    i2c1_h2c_write8(0x8C0C,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C0D,0x88); // EDID_RAM
    i2c1_h2c_write8(0x8C0E,0x88); // EDID_RAM
    i2c1_h2c_write8(0x8C0F,0x88); // EDID_RAM
    i2c1_h2c_write8(0x8C10,0x1C); // EDID_RAM
    i2c1_h2c_write8(0x8C11,0x15); // EDID_RAM
    i2c1_h2c_write8(0x8C12,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C13,0x03); // EDID_RAM
    i2c1_h2c_write8(0x8C14,0x80); // EDID_RAM
    i2c1_h2c_write8(0x8C15,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C16,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C17,0x78); // EDID_RAM
    i2c1_h2c_write8(0x8C18,0x0A); // EDID_RAM
    i2c1_h2c_write8(0x8C19,0x0D); // EDID_RAM
    i2c1_h2c_write8(0x8C1A,0xC9); // EDID_RAM
    i2c1_h2c_write8(0x8C1B,0xA0); // EDID_RAM
    i2c1_h2c_write8(0x8C1C,0x57); // EDID_RAM
    i2c1_h2c_write8(0x8C1D,0x47); // EDID_RAM
    i2c1_h2c_write8(0x8C1E,0x98); // EDID_RAM
    i2c1_h2c_write8(0x8C1F,0x27); // EDID_RAM
    i2c1_h2c_write8(0x8C20,0x12); // EDID_RAM
    i2c1_h2c_write8(0x8C21,0x48); // EDID_RAM
    i2c1_h2c_write8(0x8C22,0x4C); // EDID_RAM
    i2c1_h2c_write8(0x8C23,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C24,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C25,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C26,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C27,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C28,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C29,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2A,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2B,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2C,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2D,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2E,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C2F,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C30,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C31,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C32,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C33,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C34,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C35,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C36,0x02); // EDID_RAM
    i2c1_h2c_write8(0x8C37,0x3A); // EDID_RAM
    i2c1_h2c_write8(0x8C38,0x80); // EDID_RAM
    i2c1_h2c_write8(0x8C39,0x18); // EDID_RAM
    i2c1_h2c_write8(0x8C3A,0x71); // EDID_RAM
    i2c1_h2c_write8(0x8C3B,0x38); // EDID_RAM
    i2c1_h2c_write8(0x8C3C,0x2D); // EDID_RAM
    i2c1_h2c_write8(0x8C3D,0x40); // EDID_RAM
    i2c1_h2c_write8(0x8C3E,0x58); // EDID_RAM
    i2c1_h2c_write8(0x8C3F,0x2C); // EDID_RAM
    i2c1_h2c_write8(0x8C40,0x45); // EDID_RAM
    i2c1_h2c_write8(0x8C41,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C42,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8C43,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8C44,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C45,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C46,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C47,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8C48,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C49,0x1D); // EDID_RAM
    i2c1_h2c_write8(0x8C4A,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C4B,0x72); // EDID_RAM
    i2c1_h2c_write8(0x8C4C,0x51); // EDID_RAM
    i2c1_h2c_write8(0x8C4D,0xD0); // EDID_RAM
    i2c1_h2c_write8(0x8C4E,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8C4F,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C50,0x6E); // EDID_RAM
    i2c1_h2c_write8(0x8C51,0x28); // EDID_RAM
    i2c1_h2c_write8(0x8C52,0x55); // EDID_RAM
    i2c1_h2c_write8(0x8C53,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C54,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8C55,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8C56,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C57,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C58,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C59,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8C5A,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C5B,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C5C,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C5D,0xFC); // EDID_RAM
    i2c1_h2c_write8(0x8C5E,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C5F,0x54); // EDID_RAM
    i2c1_h2c_write8(0x8C60,0x6F); // EDID_RAM
    i2c1_h2c_write8(0x8C61,0x73); // EDID_RAM
    i2c1_h2c_write8(0x8C62,0x68); // EDID_RAM
    i2c1_h2c_write8(0x8C63,0x69); // EDID_RAM
    i2c1_h2c_write8(0x8C64,0x62); // EDID_RAM
    i2c1_h2c_write8(0x8C65,0x61); // EDID_RAM
    i2c1_h2c_write8(0x8C66,0x2D); // EDID_RAM
    i2c1_h2c_write8(0x8C67,0x48); // EDID_RAM
    i2c1_h2c_write8(0x8C68,0x32); // EDID_RAM
    i2c1_h2c_write8(0x8C69,0x43); // EDID_RAM
    i2c1_h2c_write8(0x8C6A,0x0A); // EDID_RAM
    i2c1_h2c_write8(0x8C6B,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C6C,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C6D,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C6E,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C6F,0xFD); // EDID_RAM
    i2c1_h2c_write8(0x8C70,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C71,0x14); // EDID_RAM
    i2c1_h2c_write8(0x8C72,0x78); // EDID_RAM
    i2c1_h2c_write8(0x8C73,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C74,0xFF); // EDID_RAM
    i2c1_h2c_write8(0x8C75,0x10); // EDID_RAM
    i2c1_h2c_write8(0x8C76,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C77,0x0A); // EDID_RAM
    i2c1_h2c_write8(0x8C78,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C79,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C7A,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C7B,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C7C,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C7D,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C7E,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C7F,0x8B); // EDID_RAM
    i2c1_h2c_write8(0x8C80,0x02); // EDID_RAM
    i2c1_h2c_write8(0x8C81,0x03); // EDID_RAM
    i2c1_h2c_write8(0x8C82,0x1A); // EDID_RAM
    i2c1_h2c_write8(0x8C83,0x71); // EDID_RAM
    i2c1_h2c_write8(0x8C84,0x47); // EDID_RAM
    i2c1_h2c_write8(0x8C85,0x90); // EDID_RAM
    i2c1_h2c_write8(0x8C86,0x04); // EDID_RAM
    i2c1_h2c_write8(0x8C87,0x02); // EDID_RAM
    i2c1_h2c_write8(0x8C88,0x1F); // EDID_RAM
    i2c1_h2c_write8(0x8C89,0x13); // EDID_RAM
    i2c1_h2c_write8(0x8C8A,0x11); // EDID_RAM
    i2c1_h2c_write8(0x8C8B,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C8C,0x23); // EDID_RAM
    i2c1_h2c_write8(0x8C8D,0x09); // EDID_RAM
    i2c1_h2c_write8(0x8C8E,0x07); // EDID_RAM
    i2c1_h2c_write8(0x8C8F,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C90,0x83); // EDID_RAM
    i2c1_h2c_write8(0x8C91,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8C92,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C93,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C94,0x65); // EDID_RAM
    i2c1_h2c_write8(0x8C95,0x03); // EDID_RAM
    i2c1_h2c_write8(0x8C96,0x0C); // EDID_RAM
    i2c1_h2c_write8(0x8C97,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C98,0x10); // EDID_RAM
    i2c1_h2c_write8(0x8C99,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8C9A,0x8C); // EDID_RAM
    i2c1_h2c_write8(0x8C9B,0x0A); // EDID_RAM
    i2c1_h2c_write8(0x8C9C,0xD0); // EDID_RAM
    i2c1_h2c_write8(0x8C9D,0x8A); // EDID_RAM
    i2c1_h2c_write8(0x8C9E,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8C9F,0xE0); // EDID_RAM
    i2c1_h2c_write8(0x8CA0,0x2D); // EDID_RAM
    i2c1_h2c_write8(0x8CA1,0x10); // EDID_RAM
    i2c1_h2c_write8(0x8CA2,0x10); // EDID_RAM
    i2c1_h2c_write8(0x8CA3,0x3E); // EDID_RAM
    i2c1_h2c_write8(0x8CA4,0x96); // EDID_RAM
    i2c1_h2c_write8(0x8CA5,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CA6,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8CA7,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8CA8,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CA9,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CAA,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CAB,0x18); // EDID_RAM
    i2c1_h2c_write8(0x8CAC,0x02); // EDID_RAM
    i2c1_h2c_write8(0x8CAD,0x3A); // EDID_RAM
    i2c1_h2c_write8(0x8CAE,0x80); // EDID_RAM
    i2c1_h2c_write8(0x8CAF,0xD0); // EDID_RAM
    i2c1_h2c_write8(0x8CB0,0x72); // EDID_RAM
    i2c1_h2c_write8(0x8CB1,0x38); // EDID_RAM
    i2c1_h2c_write8(0x8CB2,0x2D); // EDID_RAM
    i2c1_h2c_write8(0x8CB3,0x40); // EDID_RAM
    i2c1_h2c_write8(0x8CB4,0x10); // EDID_RAM
    i2c1_h2c_write8(0x8CB5,0x2C); // EDID_RAM
    i2c1_h2c_write8(0x8CB6,0x45); // EDID_RAM
    i2c1_h2c_write8(0x8CB7,0x80); // EDID_RAM
    i2c1_h2c_write8(0x8CB8,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8CB9,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8CBA,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CBB,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CBC,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CBD,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8CBE,0x01); // EDID_RAM
    i2c1_h2c_write8(0x8CBF,0x1D); // EDID_RAM
    i2c1_h2c_write8(0x8CC0,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CC1,0xBC); // EDID_RAM
    i2c1_h2c_write8(0x8CC2,0x52); // EDID_RAM
    i2c1_h2c_write8(0x8CC3,0xD0); // EDID_RAM
    i2c1_h2c_write8(0x8CC4,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8CC5,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8CC6,0xB8); // EDID_RAM
    i2c1_h2c_write8(0x8CC7,0x28); // EDID_RAM
    i2c1_h2c_write8(0x8CC8,0x55); // EDID_RAM
    i2c1_h2c_write8(0x8CC9,0x40); // EDID_RAM
    i2c1_h2c_write8(0x8CCA,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8CCB,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8CCC,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CCD,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CCE,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CCF,0x1E); // EDID_RAM
    i2c1_h2c_write8(0x8CD0,0x8C); // EDID_RAM
    i2c1_h2c_write8(0x8CD1,0x0A); // EDID_RAM
    i2c1_h2c_write8(0x8CD2,0xD0); // EDID_RAM
    i2c1_h2c_write8(0x8CD3,0x90); // EDID_RAM
    i2c1_h2c_write8(0x8CD4,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8CD5,0x40); // EDID_RAM
    i2c1_h2c_write8(0x8CD6,0x31); // EDID_RAM
    i2c1_h2c_write8(0x8CD7,0x20); // EDID_RAM
    i2c1_h2c_write8(0x8CD8,0x0C); // EDID_RAM
    i2c1_h2c_write8(0x8CD9,0x40); // EDID_RAM
    i2c1_h2c_write8(0x8CDA,0x55); // EDID_RAM
    i2c1_h2c_write8(0x8CDB,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CDC,0xC0); // EDID_RAM
    i2c1_h2c_write8(0x8CDD,0x6C); // EDID_RAM
    i2c1_h2c_write8(0x8CDE,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CDF,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE0,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE1,0x18); // EDID_RAM
    i2c1_h2c_write8(0x8CE2,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE3,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE4,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE5,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE6,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE7,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE8,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CE9,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CEA,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CEB,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CEC,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CED,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CEE,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CEF,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF0,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF1,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF2,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF3,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF4,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF5,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF6,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF7,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF8,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CF9,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFA,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFB,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFC,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFD,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFE,0x00); // EDID_RAM
    i2c1_h2c_write8(0x8CFF,0x4B); // EDID_RAM

    // Check HDMI cable status
    for (i = 0; i < 3; i++)
    {
        rdata8 = i2c1_h2c_read8(0x8520); // MISC_INTS_C
        if(rdata8 & 0x1)
        {
            // Enable Interrupt
            i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM
            // Set HPDO to "H"
            i2c1_h2c_write8(0x854A,0x01); // INIT_END
            params->estate = E_RS2_STATUS_SETHPDO;
            CamOsPrintf("[RS2]: 0x8520 value %d, DDC_5V detected!\n", rdata8);
            return;
        }
        SENSOR_USLEEP(100 * 1000);
    }
    // Enable Interrupt
    i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM
    // Enter Sleep
    i2c1_h2c_write16(0x0002,0x0001); // SysCtl
    params->estate = E_RS1_STATUS_SETEDID;
    CamOsPrintf("[RS1]: 0x8520 value %d, DDC_5V not on enter sleep\n", rdata8);

}

static int pCus_init(ms_cus_sensor *handle)
{
    //ISensorIfAPI *sensor_if = &(handle->sensor_if_api);
    //sensor_if->PCLK(CUS_SNR_PCLK_SR_PAD);

    SENSOR_MSLEEP(10); // Sleep for i2c timeout

    tc358743xbg_InitTable(handle);

    //SENSOR_MSLEEP(10);//usleep(50*1000);
    //pCus_SetAEGain(handle,2048);
    //pCus_SetAEUSecs(handle, 25000);

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
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    handle->video_res_supported.ulcur_res = res_idx; //TBD
    //TODO: Set sensor output resolution
    return SUCCESS;
}

static int pCus_GetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT *orit)
{
#if 0
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
#endif
    return SUCCESS;
}

static int pCus_SetOrien(ms_cus_sensor *handle, CUS_CAMSENSOR_ORIT orit)
{
    return SUCCESS;
}

static int pCus_GetFPS(ms_cus_sensor *handle)
{
    return SUCCESS;
}
static int pCus_SetFPS(ms_cus_sensor *handle, u32 fps)
{
    return SUCCESS;
}
static int pCus_CustDefineFunction(ms_cus_sensor *handle, u32 cmd_id, void *param)
{
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;

    switch (cmd_id)
    {
        case EN_CUST_CMD_GET_STATE:
        {
            if (param)
                memcpy(param, &params->estate, sizeof(Tcxbg_state_e));
            else
                return FAIL;
        }
        break;
        case EN_CUST_CMD_GET_AUDIO_INFO:
        {
            u16 rdata8 = 0;
            Tcxbg_audio_info_t stTcAudInfo;
            memset(&stTcAudInfo, 0, sizeof(Tcxbg_audio_info_t));
            if (params->estate == E_RS3_STATUS_ENMIPIOUT)
            {
                rdata8 = i2c1_h2c_read8(0x8735);
                CamOsPrintf("0x8735 = 0x%x\n", rdata8);
                switch ((rdata8 & 0x1C) >> 2)
                {
                    case 1:
                        stTcAudInfo.uintSampleRate = 32000;
                        break;
                    case 2:
                        stTcAudInfo.uintSampleRate = 44100;
                        break;
                    case 3:
                        stTcAudInfo.uintSampleRate = 48000;
                        break;
                    case 4:
                        stTcAudInfo.uintSampleRate = 88200;
                        break;
                    case 5:
                        stTcAudInfo.uintSampleRate = 96000;
                        break;
                    case 6:
                        stTcAudInfo.uintSampleRate = 176400;
                        break;
                    case 7:
                        stTcAudInfo.uintSampleRate = 192000;
                        break;
                    default:
                        goto READ_STREAM_HEADER;
                }
                switch (rdata8 & 0x3)
                {
                    case 1:
                    {
                        stTcAudInfo.ucharBitWidth = 16;
                    }
                    break;
                    case 2:
                    {
                        stTcAudInfo.ucharBitWidth = 20;
                    }
                    break;
                    case 3:
                    {
                        stTcAudInfo.ucharBitWidth = 24;
                    }
                    break;
                    default:
                        goto READ_STREAM_HEADER;
                }
                goto READ_CHANNELS;

READ_STREAM_HEADER:
                rdata8 = i2c1_h2c_read8(0x8625);
                CamOsPrintf("0x8625 = 0x%x\n", rdata8);
                switch (rdata8 & 0xF)
                {
                    case 0:
                        stTcAudInfo.uintSampleRate = 44100;
                        break;
                    case 2:
                        stTcAudInfo.uintSampleRate = 48000;
                        break;
                    case 3:
                        stTcAudInfo.uintSampleRate = 32000;
                        break;
                    case 4:
                        stTcAudInfo.uintSampleRate = 22050;
                        break;
                    case 6:
                        stTcAudInfo.uintSampleRate = 24000;
                        break;
                    case 8:
                        stTcAudInfo.uintSampleRate = 88200;
                        break;
                    case 0xA:
                        stTcAudInfo.uintSampleRate = 32000;
                        break;
                    case 0xC:
                        stTcAudInfo.uintSampleRate = 176400;
                        break;
                    case 0xE:
                        stTcAudInfo.uintSampleRate = 192000;
                        break;
                    default:
                        return FAIL;
                }
                rdata8 = i2c1_h2c_read8(0x8626);
                CamOsPrintf("0x8626 = 0x%x\n", rdata8);
                switch (rdata8 & 0xF)
                {
                    case 2:
                    {
                        stTcAudInfo.ucharBitWidth = 16;
                    }
                    break;
                    case 3:
                    {
                        stTcAudInfo.ucharBitWidth = 20;
                    }
                    break;
                    case 4:
                    {
                        stTcAudInfo.ucharBitWidth = 18;
                    }
                    break;
                    case 5:
                    {
                        stTcAudInfo.ucharBitWidth = 22;
                    }
                    break;
                    case 8:
                    {
                        stTcAudInfo.ucharBitWidth = 19;
                    }
                    break;
                    case 9:
                    {
                        stTcAudInfo.ucharBitWidth = 23;
                    }
                    break;
                    case 0xA:
                    {
                        stTcAudInfo.ucharBitWidth = 20;
                    }
                    break;
                    case 0xB:
                    {
                        stTcAudInfo.ucharBitWidth = 24;
                    }
                    break;
                    case 0xC:
                    {
                        stTcAudInfo.ucharBitWidth = 17;
                    }
                    break;
                    case 0xD:
                    {
                        stTcAudInfo.ucharBitWidth = 21;
                    }
                    break;

                    default:
                        return FAIL;
                }
READ_CHANNELS:
                rdata8 = i2c1_h2c_read8(0x8734);
                CamOsPrintf("0x8734 = 0x%x\n", rdata8);
                switch (rdata8 & 0x7)
                {
                    case 1:
                    {
                        stTcAudInfo.ucharChannels = 2;
                    }
                    break;
                    case 2:
                    {
                        stTcAudInfo.ucharChannels = 3;
                    }
                    break;
                    case 3:
                    {
                        stTcAudInfo.ucharChannels = 4;
                    }
                    break;
                    case 4:
                    {
                        stTcAudInfo.ucharChannels = 5;
                    }
                    break;
                    case 5:
                    {
                        stTcAudInfo.ucharChannels = 6;
                    }
                    break;
                    case 6:
                    {
                        stTcAudInfo.ucharChannels = 7;
                    }
                    break;
                    case 7:
                    {
                        stTcAudInfo.ucharChannels = 8;
                    }
                    break;
                    default:
                        return FAIL;
                }
                stTcAudInfo.bPcmData = 1;
                if (param)
                    memcpy(param, &stTcAudInfo, sizeof(Tcxbg_audio_info_t));
                else
                    return FAIL;
            }
        }
        break;
        default:
            CamOsPrintf("Do not support current cmd: %d\n", cmd_id);
            return FAIL;
    }

    return SUCCESS;
}
#if 0
static int pCus_GetSensorCap(ms_cus_sensor *handle, CUS_CAMSENSOR_CAP *cap)
{
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    if (cap)
        memcpy(cap, &sensor_cap, sizeof(CUS_CAMSENSOR_CAP));
    else
        return FAIL;
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
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;
   // ISensorIfAPI *sensor_if = handle->sensor_if_api;
   // printk("[%s]\n", __FUNCTION__);
    switch(status)
    {
        case CUS_FRAME_INACTIVE:
            break;
        case CUS_FRAME_ACTIVE:
            if(params->dirty)
            {
                params->dirty = false;
            }
            break;
        default :
            break;
    }
/*
    if(params->skip_cnt > 0)
    {
        printk("skip cnt %d \n", params->skip_cnt );
        sensor_if->SetSkipFrame(handle->snr_pad_group, params->expo.fps, params->skip_cnt);
        params->skip_cnt = 0;
    }
    */
    return SUCCESS;
}

static int pCus_GetAEUSecs(ms_cus_sensor *handle, u32 *us)
{
    return SUCCESS;
}

static int pCus_SetAEUSecs(ms_cus_sensor *handle, u32 us)
{
    return SUCCESS;
}

// Gain: 1x = 1024
static int pCus_GetAEGain(ms_cus_sensor *handle, u32* gain)
{
    return SUCCESS;
}

static int pCus_SetAEGain_cal(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int pCus_SetAEGain(ms_cus_sensor *handle, u32 gain)
{
    return SUCCESS;
}

static int pCus_GetAEMinMaxUSecs(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    *min = 1;
    *max = 1000000/4;
    return SUCCESS;
}

static int pCus_GetAEMinMaxGain(ms_cus_sensor *handle, u32 *min, u32 *max)
{
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    *min = 1024;
    *max = 177*1024;
    return SUCCESS;
}

static int pCus_setCaliData_gain_linearity(ms_cus_sensor* handle, CUS_GAIN_GAP_ARRAY* pArray, u32 num)
{
    u32 i, j;

    for(i=0,j=0;i< num ;i++,j+=2)
    {
        gain_gap_compensate[i].gain=pArray[i].gain;
        gain_gap_compensate[i].offset=pArray[i].offset;
    }
    SENSOR_DMSG("[%s]%d, %d, %d, %d\n", __FUNCTION__, num, pArray[0].gain, pArray[1].gain, pArray[num-1].offset);
    return SUCCESS;
}

static int tc358743xbg_GetShutterInfo(struct __ms_cus_sensor* handle,CUS_SHUTTER_INFO *info)
{
    SENSOR_DMSG("[%s]\n", __FUNCTION__);
    info->max = 1000000000/Preview_MIN_FPS;
    info->min = Preview_line_period * 5;
    info->step = Preview_line_period;
    return SUCCESS;
}

void interrupt_func(u32 eIntNum, void* p1)
{
    tc358743xbg_params *Params =(tc358743xbg_params *)p1;

    Params->bIrqFlag = 1;
    WAKE_UP(Params->IsrTaskwaitqueue);
}


int  Tc358743_IsrThread(void *args)
{
    ms_cus_sensor *handle = (ms_cus_sensor *)args;
    tc358743xbg_params *params = (tc358743xbg_params *)handle->private_data;
    u8  rdata8[3] ={0, 0, 0};
    u16 rdata16 = 0;
    bool bDDC_5V= FALSE;
    bool bEnMipiOut = FALSE;
    bool bSyncLost = FALSE;
    bool bCutoff = FALSE;
    bool bUpdateInfo = FALSE;

    while(CAM_OS_OK != CamOsThreadShouldStop())
    {
        WaitEventTimeout(params->WorkTaskwaitqueue, params->bIrqFlag, 1000);
        if(params->bIrqFlag == 1)
        {
            // Interrupt Service Routine(RS_Int)
            // Exit from Sleep
            i2c1_h2c_write16(0x0002,0x0000); // SysCtl
            SENSOR_USLEEP(10);
            rdata16 = i2c1_h2c_read16(0x0014); // TOP_INTS_C
            rdata8[0] = i2c1_h2c_read8(0x8502); // SYS_INTS_C
            rdata8[1] = i2c1_h2c_read8(0x850B); // MISC_INTS_C
            rdata8[2] = i2c1_h2c_read8(0x8520); // MISC_INTS_C
            i2c1_h2c_write16(0x0016,0x073F); // TOP_INTS_C disable int
            i2c1_h2c_write8(0x8502,0xFF); // SYS_INTS_C
            i2c1_h2c_write8(0x850B,0x1F); // MISC_INTS_C
            i2c1_h2c_write16(0x0014,0x073F); // TOP_INTS_C  clear int

            CamOsPrintf("0x0014 0x%x, 0x8502 0x%x, 0x850b 0x%x, 0x8520 0x%x\n", rdata16, rdata8[0], rdata8[1], rdata8[2]);

            if((rdata8[0] & 0x1) && (rdata8[2] & 0x1)
                && (params->estate == E_RS1_STATUS_SETEDID || params->estate == E_RS5_STATUS_NOCONNECTION))
            {
                bDDC_5V = TRUE;
            }
            else if((rdata8[1] & (0x1 << 1)) && (rdata8[2] & (0x1 << 7))
                     && (params->estate == E_RS2_STATUS_SETHPDO || params->estate == E_RS4_STATUS_NOSIGNAL))
            {
                bEnMipiOut = TRUE;
            }
            else if((rdata8[0] & 0x1) && ((rdata8[2] & 0x1) ==0)
                     && (params->estate == E_RS3_STATUS_ENMIPIOUT || params->estate == E_RS4_STATUS_NOSIGNAL || params->estate == E_RS2_STATUS_SETHPDO))
            {
                 bCutoff= TRUE;
            }
            else if((rdata8[1] & (0x1<<1)) && (rdata8[2] & 0x1) && !(rdata8[2] & (0x1 << 7))
                     && params->estate == E_RS3_STATUS_ENMIPIOUT)
            {
                 bSyncLost= TRUE;
            }
            else if (rdata8[0] && (rdata8[1] & (0x1 << 1)) && (rdata8[2] & (0x1 << 7))
                     && params->estate == E_RS3_STATUS_ENMIPIOUT)
            {
                bUpdateInfo = TRUE;
            }
            else
            {
                CamOsPrintf("Cur state %d, Next state error!!\n", params->estate);
            }

            if(bDDC_5V == TRUE)
            {
                // Set HPDO to "H"
                bDDC_5V = FALSE;
                i2c1_h2c_write8(0x854A,0x01); // INIT_END
                i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM enable int
                params->estate = E_RS2_STATUS_SETHPDO;
                CamOsPrintf("[RS2]: DDC 5V detected! set HPDO \n");
            }
            else if(bEnMipiOut == TRUE)
            {
                bEnMipiOut = FALSE;
                SENSOR_MSLEEP(2000);
                tc358743xbg_GetInputInfo(handle);
                tc358743xbg_EnableCsiOut(handle);
                params->bEnCsiOut = TRUE;
                params->estate = E_RS3_STATUS_ENMIPIOUT;
                CamOsPrintf("[RS3]: Enable Mipi Out \n");
            }
            else if(bSyncLost == TRUE)
            {
                bSyncLost = FALSE;
                i2c1_h2c_write16(0x0004,0x0CD4); // ConfCtl
                i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM enable int
                params->estate = E_RS4_STATUS_NOSIGNAL;
                CamOsPrintf("[RS4]: sync lost \n");
            }
            else if(bCutoff == TRUE)
            {
                bCutoff = FALSE;
                i2c1_h2c_write16(0x0004,0x0CD4); // ConfCtl
                i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM enable int
                i2c1_h2c_write16(0x0002,0x0001); // SysCtl  enter sleep
                params->estate = E_RS5_STATUS_NOCONNECTION;
                CamOsPrintf("[RS5]: DDC CutOff \n");
            }
            else if (bUpdateInfo == TRUE)
            {
                bUpdateInfo = FALSE;
                tc358743xbg_GetInputInfo(handle);
                i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM enable int
            }
            else
            {
                CamOsPrintf("Ignore operation!!\n", params->estate);
                i2c1_h2c_write16(0x0016,0x053F); // TOP_INTM enable int
            }
            params->bIrqFlag = 0;
        }
        else
        {
            //CamOsPrintf("wait irq time out\n");
        }
    }

    return SUCCESS;
}
int tc358743xbg_cus_camsensor_init_handle(ms_cus_sensor* handle)
{
    tc358743xbg_params *params;
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
    params = (tc358743xbg_params *)handle->private_data;

    ////////////////////////////////////
    //    sensor model ID                           //
    ////////////////////////////////////
    sprintf(handle->model_id,"tc358743xbg_MIPI");

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
    handle->pCus_sensor_release     = tc358743xbg_cus_camsensor_release_handle;
    handle->pCus_sensor_init        = pCus_init    ;
    handle->pCus_sensor_poweron     = pCus_poweron ;
    handle->pCus_sensor_poweroff    = pCus_poweroff;

    // Normal
    handle->pCus_sensor_GetSensorID       = pCus_GetSensorID;
    handle->pCus_sensor_GetVideoResNum    = pCus_GetVideoResNum;
    handle->pCus_sensor_GetVideoRes       = pCus_GetVideoRes;
    handle->pCus_sensor_GetCurVideoRes    = pCus_GetCurVideoRes;
    handle->pCus_sensor_SetVideoRes       = pCus_SetVideoRes;
    handle->pCus_sensor_GetOrien          = pCus_GetOrien;
    handle->pCus_sensor_SetOrien          = pCus_SetOrien;
    handle->pCus_sensor_GetFPS            = pCus_GetFPS;
    handle->pCus_sensor_SetFPS            = pCus_SetFPS;
    //handle->pCus_sensor_GetSensorCap    = pCus_GetSensorCap;
    handle->pCus_sensor_SetPatternMode    = tc358743xbg_SetPatternMode;
    handle->pCus_sensor_CustDefineFunction = pCus_CustDefineFunction;
    ///////////////////////////////////////////////////////
    // AE
    ///////////////////////////////////////////////////////
    // unit: micro seconds
    handle->pCus_sensor_AEStatusNotify   = pCus_AEStatusNotify;
    handle->pCus_sensor_GetAEUSecs       = pCus_GetAEUSecs;
    handle->pCus_sensor_SetAEUSecs       = pCus_SetAEUSecs;
    handle->pCus_sensor_GetAEGain        = pCus_GetAEGain;
    handle->pCus_sensor_SetAEGain_cal    = pCus_SetAEGain_cal;
    handle->pCus_sensor_SetAEGain        = pCus_SetAEGain;
    handle->pCus_sensor_GetShutterInfo   = tc358743xbg_GetShutterInfo;
    handle->pCus_sensor_GetAEMinMaxGain  = pCus_GetAEMinMaxGain;
    handle->pCus_sensor_GetAEMinMaxUSecs = pCus_GetAEMinMaxUSecs;

    //sensor calibration
    handle->pCus_sensor_setCaliData_gain_linearity = pCus_setCaliData_gain_linearity;
    params->expo.vts  = vts_30fps;
    params->expo.fps  = 30;
    params->dirty = false;
    params->skip_cnt = 0;

    //MIPI config
    handle->interface_attr.attr_mipi.mipi_lane_num = lane_number;
    handle->interface_attr.attr_mipi.mipi_data_format = CUS_SEN_INPUT_FORMAT_YUV422; // RGB pattern.
    handle->interface_attr.attr_mipi.mipi_yuv_order = CUS_SENSOR_YUV_ORDER_CY; //don't care in RGB pattern.
    handle->interface_attr.attr_mipi.mipi_hsync_mode = SENSOR_MIPI_HSYNC_MODE;
    handle->interface_attr.attr_mipi.mipi_hdr_mode = CUS_HDR_MODE_NONE;
    handle->interface_attr.attr_mipi.mipi_hdr_virtual_channel_num = 0;
    handle->video_res_supported.num_res =1;
    handle->video_res_supported.ulcur_res = 0; //default resolution index is 0.
    handle->video_res_supported.res[0].width = 1920;
    handle->video_res_supported.res[0].height = 1080;
    handle->video_res_supported.res[0].max_fps= Preview_MAX_FPS;
    handle->video_res_supported.res[0].min_fps= Preview_MIN_FPS;
    handle->video_res_supported.res[0].crop_start_x= Preview_CROP_START_X;
    handle->video_res_supported.res[0].crop_start_y= Preview_CROP_START_Y;
    handle->video_res_supported.res[0].nOutputWidth= 1920;
    handle->video_res_supported.res[0].nOutputHeight= 1080;
    sprintf(handle->video_res_supported.res[0].strResDesc, "%dx%d", 1920, 1080);

    return SUCCESS;
}

int tc358743xbg_cus_camsensor_release_handle(ms_cus_sensor *handle)
{
    return SUCCESS;
}
/*
SENSOR_DRV_ENTRY_3CHAHD_IMPL_END_EX(tc358743xbg,
                                    NULL,
                                    tc358743xbg_cus_camsensor_init_handle,
                                    NULL,
                                    NULL,
                                    tc358743xbg_params
                                    );
                                    */
SENSOR_DRV_ENTRY_IMPL_END_EX(  tc358743xbg,
                            tc358743xbg_cus_camsensor_init_handle,
                            NULL,
                            NULL,
                            tc358743xbg_params
                         );

