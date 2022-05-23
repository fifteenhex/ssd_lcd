/*
* hal_disp_util.h- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/

#ifndef __HAL_DISP_UTIL_H__
#define __HAL_DISP_UTIL_H__

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#define RIU_READ_BYTE(addr)         ( READ_BYTE( 0x1F000000+ (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( 0x1F000000 + (addr) ) )
#define RIU_READ_4BYTE(addr)        ( READ_LONG( 0x1F000000 + (addr) ) )

#define RIU_WRITE_BYTE(addr, val)    WRITE_BYTE( (0x1F000000 + (addr)), val)
#define RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( 0x1F000000 + (addr), val)
#define RIU_WRITE_4BYTE(addr, val)   WRITE_LONG( 0x1F000000 + (addr), val)


#define RBYTE( u32Reg ) RIU_READ_BYTE( (u32Reg) << 1)

#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)

#define R2BYTEMSK( u32Reg, u16mask)\
    ( ( RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask  ) )

#define R4BYTE( u32Reg )\
    ( {  (RIU_READ_4BYTE( (u32Reg)<<1)) ; } )

#define R4BYTEMSK( u32Reg, u32Mask) \
    ( RIU_READ_4BYTE( (u32Reg) << 1) & u32Mask  )

#define WBYTE(u32Reg, u8Val) RIU_WRITE_BYTE( ((u32Reg) << 1), u8Val )

#define WBYTEMSK(u32Reg, u8Val, u8Mask) \
               RIU_WRITE_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)), ( RIU_READ_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)) ) & ~(u8Mask)) | ((u8Val) & (u8Mask)) )

#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )

#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
              RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )

#if 0
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )
#else
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_4BYTE( (u32Reg)<<1, u32Val);})
#endif

#define W4BYTEMSK( u32Reg, u32Val, u32Mask)\
                  RIU_WRITE_4BYTE( (u32Reg)<< 1 , (RIU_READ_4BYTE((u32Reg) << 1) & ~(u32Mask)) | ((u32Val) & (u32Mask)) )


#define W3BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1,  u32Val); \
                RIU_WRITE_BYTE( (u32Reg + 2) << 1 ,  ((u32Val) >> 16)); } )



#define DISP_BIT0                           0x00000001
#define DISP_BIT1                           0x00000002
#define DISP_BIT2                           0x00000004
#define DISP_BIT3                           0x00000008
#define DISP_BIT4                           0x00000010
#define DISP_BIT5                           0x00000020
#define DISP_BIT6                           0x00000040
#define DISP_BIT7                           0x00000080
#define DISP_BIT8                           0x00000100
#define DISP_BIT9                           0x00000200
#define DISP_BIT10                          0x00000400
#define DISP_BIT11                          0x00000800
#define DISP_BIT12                          0x00001000
#define DISP_BIT13                          0x00002000
#define DISP_BIT14                          0x00004000
#define DISP_BIT15                          0x00008000
#define DISP_BIT16                          0x00010000
#define DISP_BIT17                          0x00020000
#define DISP_BIT18                          0x00040000
#define DISP_BIT19                          0x00080000
#define DISP_BIT20                          0x00100000
#define DISP_BIT21                          0x00200000
#define DISP_BIT22                          0x00400000
#define DISP_BIT23                          0x00800000
#define DISP_BIT24                          0x01000000
#define DISP_BIT25                          0x02000000
#define DISP_BIT26                          0x04000000
#define DISP_BIT27                          0x08000000
#define DISP_BIT28                          0x10000000
#define DISP_BIT29                          0x20000000
#define DISP_BIT30                          0x40000000
#define DISP_BIT31                          0x80000000


#define HAL_DISP_UTILITY_CMDQ_NUM               (2)

#define HAL_DISP_UTILITY_REG_BANK_NUM           (E_DISP_UTILITY_REG_CONFIG_NUM)         // MAX number of reigster bank
#define HAL_DISP_UTILIYT_REG_NUM                (0x80)
#define HAL_DISP_UTILITY_REG_BANK_SIZE          (HAL_DISP_UTILIYT_REG_NUM * 4)                   // Each bank has 128 register and each 32bit for value & Index
#define HAL_DISP_UTILITY_INDEX_BUFFER_SIZE      (HAL_DISP_UTILIYT_REG_NUM * 4) // Index buffer
#define HAL_DISP_UTILITY_CMDQ_BUFFER_CNT        (512)
#define HAL_DISP_UTILITY_DIRECT_CMDQ_CNT        (50)

#define HAL_DISP_UTILITY_FLAG_CMDQ_IDX_EXIST    0x8000

#define HAL_DISP_UTILITY_CMDQ0_WAIT_DONE_EVENT(id)  ((id) == 0 ? E_MHAL_CMDQEVE_DISP0_INT : E_MHAL_CMDQEVE_DISP1_INT)

#define HAL_DISP_UTILITY_DISP_TO_CMDQ_CLEAR(id)     ((id) == 0 ? REG_DISP_TOP_0_40_L : REG_DISP_TOP_0_40_L)

#define REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(id)    ((id) == 0 ? REG_DISP_MOPS_HVSP_7C_L : REG_DISP_MOPS_HVSP_7C_L) //dummy register
#define REG_CMDQ_IN_PROCESS_MSK                     (0x0100)
#define REG_CMDQ_IN_PROCESS_ON                      (0x0100)
#define REG_CMDQ_IN_PROCESS_OFF                     (0x0000)


#define REG_HAL_DISP_UTILITY_CMDQ_WAIT_CNT(id)      ((id) == 0 ? REG_DISP_MOPS_HVSP_7D_L : REG_DISP_MOPS_HVSP_7D_L)
#define REG_CMDQ_WAIT_CNT_MSK                       (0xFF00)
#define REG_CMDQ_WAIT_CNT_SHIFT                     (8)

#define REG_HAL_DISP_UTILITY_CMDQ_FLIP_CNT(id)      ((id) == 0 ? REG_DISP_MOPS_HVSP_7E_L : REG_DISP_MOPS_HVSP_7E_L)
#define REG_CMDQ_FLIP_CNT_MSK                       (0xFF00)
#define REG_CMDQ_FLIPT_CNT_SHIFT                    (8)

#define HAL_DISP_UTILITY_DISP_TOP_RST(id)           ((id) == 0 ? REG_DISP_TOP_0_00_L : REG_DISP_TOP_0_00_L)
#define HAL_DISP_UTILITY_DISP_DSI_GATE_EN(id)       ((id) == 0 ? REG_DISP_TOP_0_25_L : REG_DISP_TOP_0_25_L)


#define HAL_DISP_UTILITY_CMDQ_CNT_MAX                (0xFFFF)
#define HAL_DISP_UTILITY_CNT_ADD(x,y)                (x = (x+y) >= HAL_DISP_UTILITY_CMDQ_CNT_MAX ? 1 : (x+y))
//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

typedef enum
{
//    E_DISP_UTILITY_REG_HDMITX_DTOP_BASE,
//    E_DISP_UTILITY_REG_HDMITX_ATOP_BASE,
//    E_DISP_UTILITY_REG_DISP_DAC_BASE,
    E_DISP_UTILITY_REG_DISP_TOP_0_BASE,
    E_DISP_UTILITY_REG_DISP_TOP_OP2_0_BASE,
    E_DISP_UTILITY_REG_DISP_TOP_MACE_BASE,
    E_DISP_UTILITY_REG_DISP_TOP_GAMMA_0_BASE,
//    E_DISP_UTILITY_REG_DISP_TOP_1_BASE,
//    E_DISP_UTILITY_REG_DISP_TOP_OP2_1_BASE,
//    E_DISP_UTILITY_REG_DISP_TOP_GAMMA_1_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK00_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK01_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK02_BASE,
    E_DISP_UTILITY_REG_DISP_MOPS_BK0_BASE,
//    E_DISP_UTILITY_REG_DISP_MOP_ROT_BK0_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK10_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK11_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPG_BK12_BASE,
//    E_DISP_UTILITY_REG_DISP_MOPS_BK1_BASE,
//    E_DISP_UTILITY_REG_DISP_MOP_ROT_BK1_BASE,
//    E_DISP_UTILITY_REG_DISP_DMA_BASE,
    E_DISP_UTILITY_REG_CLKGEN_BASE,
    E_DISP_UTILITY_REG_LCD_MISC_BASE,
    E_DISP_UTILITY_REG_DISP_MOPS_HVSP_BASE,
    E_DISP_UTILITY_REG_CONFIG_NUM,
} HalDispUtilityRegConfigType_e;

typedef enum
{
    E_DISP_UTILITY_REG_ACCESS_CPU,
    E_DISP_UTILITY_REG_ACCESS_CMDQ,
} HalDispUtilityRegAccessMode_e;
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
   u32                  u32Addr;  // 16bit Bank addr + 8bit/16bit-regaddr
   u16                  u16Data;  // 16bit data
   u16                  u16Mask;  // inverse normal case
} HalDispUtilityCmdReg_t;

typedef struct
{
    HalDispUtilityCmdReg_t  stCmd;
    HalDispUtilityRegConfigType_e  enType;
    u32                 u32TypeMax;
    bool                bSet;
    void                *pCtx;
} HalDispUtilityInquire_t;


typedef struct
{
    s32  s32CmdqId;

    void *pvRegBuffer;
    void *pvInquireBuffer;
    void *pvIndexBuffer;
    u32  u32TblCnt;

    void *pvDirectCmdqBuffer;
    u32  u32DirectCmdqCnt;

    void *pvCmdqInf;
    u16  u16RegFlipCnt;
    u16  u16WaitDoneCnt;
    u16  u16SyncCnt;
    bool bEnSyncCnt;
}HalDispUtilityCmdqContext_t;

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __HAL_DISP_UTILITY_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE bool HalDispUtilityInit(u32 u32CtxId);
INTERFACE bool HalDispUtilityDeInit(u32 u32CtxId);
INTERFACE bool HalDispUtilityGetCmdqContext(void **pvCtx, u32 u32CtxId);
INTERFACE void HalDispUtilitySetCmdqInf(void *pCmdqInf, u32 u32CtxId);

INTERFACE u16  HalDispUtilityR2BYTEDirect(u32 u32Reg);
INTERFACE u16  HalDispUtilityR2BYTEMaskDirect(u32 u32Reg, u16 u16Mask);
INTERFACE u32  HalDispUtilityR4BYTEDirect(u32 u32Reg);
INTERFACE u16  HalDispUtilityR2BYTERegBuf(u32 u32Reg, void *pvCtxIn);
INTERFACE void HalDispUtilityWBYTEMSKRegBuf(u32 u32Reg, u8 u8Val, u8 u8Mask, void *pCtx);
INTERFACE void HalDispUtilityWBYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx);
INTERFACE void HalDispUtilityWBYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx);
INTERFACE void HalDispUtilityWBYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKRegBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx);
INTERFACE void HalDispUtilityW2BYTEInqireBuf(u32 u32Reg, u16 u16Val, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEDirect(u32 u32Reg, u16 u16Val, void *pCtx);
INTERFACE void HaDisplUtilityW2BYTE(u32 u32Reg, u16 u16Val, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx);
INTERFACE void HalDispUtilityW2BYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx);
INTERFACE void HalDispUtilityW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKDirectCmdq(void *pvCtxIn, u32 u32Reg, u16 u16Val, u16 u16Mask);
INTERFACE void HalDispUtilityW2BYTEMSKDirectCmdqWrite(void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32 u32Reg, u16 u16Val, u16 u16Mask);
INTERFACE void HalDispUtilityFilpRegFire(void *pvCtxIn);
INTERFACE bool HalDispUtilityAddWaitCmd(void *pvCtxIn);
INTERFACE bool HalDispUtilityAddDelayCmd(void *pvCtxIn, u16 u16PollingTime);
INTERFACE void HalDispUtilitySetRegAccessMode(u32 u32Mode);
INTERFACE u32  HalDispUtilityGetRegAccessMode(void);
INTERFACE void HalDispUtilityResetIndexBuffer(void *pvCtxIn);
INTERFACE void HalDispUtilityResetCmdqCnt(void *pvCtxIn);

#undef  INTERFACE
#endif /* __HAL_DISP_UTIL_H__ */
