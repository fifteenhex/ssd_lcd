/*
* hal_disp_utility.c- Sigmastar
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


#define __HAL_DISP_UTILITY_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_cmdq.h"
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_reg.h"
#include "hal_disp_util.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define RIU_GET_ADDR(addr)          ((void*)(gu32DispUtilityRiuBaseVir + ((addr)<<1)))

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispOsMutexConfig_t gstDispUtilityMutexCfg;
u32 u32DispUtilityCmdqInitFlag = 0;
HalDispUtilityCmdqContext_t gstDispUtilityCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM];
HalDispUtilityRegAccessMode_e geDispRegAccessMode = E_DISP_UTILITY_REG_ACCESS_CPU;
void *gpvRegBuffer = NULL;
void *gpvIndexBuffer = NULL;

static u32 gu32DispUtilityRiuBaseVir  = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void _HalDispUtilitySetInitFlagByIdx(u32 u32CtxId, bool bEnable)
{
    if(bEnable)
    {
        u32DispUtilityCmdqInitFlag |= (1<< u32CtxId);
    }
    else
    {
        u32DispUtilityCmdqInitFlag &= ~(1 << u32CtxId);
    }
}

bool _HalDispUtilityGetInitFlagByIdx(u32 u32CtxId)
{
    return (u32DispUtilityCmdqInitFlag & (1 << u32CtxId)) ? 1 : 0;
}

u32 _HalDispUtilityGetInitFlag(void)
{
    return u32DispUtilityCmdqInitFlag;
}

u32 _HalDispUtilityTransTypeToBank(u32 u32Type)
{
    u32 u32BankAddr = 0;

    switch(u32Type)
    {
/*        case E_DISP_UTILITY_REG_HDMITX_DTOP_BASE:
            u32BankAddr = REG_HDMITX_DTOP_BASE;
            break;
        case E_DISP_UTILITY_REG_HDMITX_ATOP_BASE:
            u32BankAddr = REG_HDMITX_ATOP_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_DAC_BASE:
            u32BankAddr = REG_DISP_DAC_BASE;
            break;*/
        case E_DISP_UTILITY_REG_DISP_TOP_0_BASE:
            u32BankAddr = REG_DISP_TOP_0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_OP2_0_BASE:
            u32BankAddr = REG_DISP_TOP_OP2_0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_MACE_BASE:
            u32BankAddr = REG_DISP_TOP_MACE_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_GAMMA_0_BASE:
            u32BankAddr = REG_DISP_TOP_GAMMA_0_BASE;
            break;
/*        case E_DISP_UTILITY_REG_DISP_TOP_1_BASE:
            u32BankAddr = REG_DISP_TOP_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_OP2_1_BASE:
            u32BankAddr = REG_DISP_TOP_OP2_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_GAMMA_1_BASE:
            u32BankAddr = REG_DISP_TOP_GAMMA_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK00_BASE:
            u32BankAddr = REG_DISP_MOPG_BK00_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK01_BASE:
            u32BankAddr = REG_DISP_MOPG_BK01_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK02_BASE:
            u32BankAddr = REG_DISP_MOPG_BK02_BASE;
            break;*/
        case E_DISP_UTILITY_REG_DISP_MOPS_BK0_BASE:
            u32BankAddr = REG_DISP_MOPS_BK0_BASE;
            break;
/*        case E_DISP_UTILITY_REG_DISP_MOP_ROT_BK0_BASE:
            u32BankAddr = REG_DISP_MOP_ROT_BK0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK10_BASE:
            u32BankAddr = REG_DISP_MOPG_BK10_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK11_BASE:
            u32BankAddr = REG_DISP_MOPG_BK11_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK12_BASE:
            u32BankAddr = REG_DISP_MOPG_BK12_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPS_BK1_BASE:
            u32BankAddr = REG_DISP_MOPS_BK1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOP_ROT_BK1_BASE:
            u32BankAddr = REG_DISP_MOP_ROT_BK1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_DMA_BASE:
            u32BankAddr = REG_DISP_DMA_BASE;
            break;*/
        case E_DISP_UTILITY_REG_CLKGEN_BASE:
            u32BankAddr = REG_CLKGEN_BASE;
            break;
        case E_DISP_UTILITY_REG_LCD_MISC_BASE:
            u32BankAddr = REG_LCD_MISC_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPS_HVSP_BASE:
            u32BankAddr = REG_DISP_MOPS_HVSP_BASE;
            break;
        default:
            u32BankAddr = 0;
            break;
    }

    return u32BankAddr;
}

u32 _HalDispUtilityTransBankToType(u32 u32Bank, u32 u32CtxId)
{
    HalDispUtilityRegConfigType_e enType = E_DISP_UTILITY_REG_CONFIG_NUM;

    switch(u32Bank)
    {
/*        case REG_HDMITX_DTOP_BASE:
            enType = E_DISP_UTILITY_REG_HDMITX_DTOP_BASE;
            break;
        case REG_HDMITX_ATOP_BASE:
            enType = E_DISP_UTILITY_REG_HDMITX_ATOP_BASE;
            break;
        case REG_DISP_DAC_BASE:
            enType = E_DISP_UTILITY_REG_DISP_DAC_BASE;
            break;*/
        case REG_DISP_TOP_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_0_BASE;
            break;
        case REG_DISP_TOP_OP2_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_OP2_0_BASE;
            break;
        case REG_DISP_TOP_MACE_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_MACE_BASE;
            break;
        case REG_DISP_TOP_GAMMA_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_GAMMA_0_BASE;
            break;
/*        case REG_DISP_TOP_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_1_BASE;
            break;
        case REG_DISP_TOP_OP2_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_OP2_1_BASE;
            break;
        case REG_DISP_TOP_GAMMA_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_GAMMA_1_BASE;
            break;
        case REG_DISP_MOPG_BK00_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK00_BASE;
            break;
        case REG_DISP_MOPG_BK01_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK01_BASE;
            break;
        case REG_DISP_MOPG_BK02_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK02_BASE;
            break;*/
        case REG_DISP_MOPS_BK0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPS_BK0_BASE;
            break;
/*        case REG_DISP_MOP_ROT_BK0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOP_ROT_BK0_BASE;
            break;
        case REG_DISP_MOPG_BK10_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK10_BASE;
            break;
        case REG_DISP_MOPG_BK11_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK11_BASE;
            break;
        case REG_DISP_MOPG_BK12_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK12_BASE;
            break;
        case REG_DISP_MOPS_BK1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPS_BK1_BASE;
            break;
        case REG_DISP_MOP_ROT_BK1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOP_ROT_BK1_BASE;
            break;
        case REG_DISP_DMA_BASE:
            enType = E_DISP_UTILITY_REG_DISP_DMA_BASE;
            break;*/
        case REG_CLKGEN_BASE:
            enType = E_DISP_UTILITY_REG_CLKGEN_BASE;
            break;
        case REG_LCD_MISC_BASE:
            enType = E_DISP_UTILITY_REG_LCD_MISC_BASE;
            break;
        case REG_DISP_MOPS_HVSP_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPS_HVSP_BASE;
            break;
        default:
            enType = E_DISP_UTILITY_REG_CONFIG_NUM;
            DISP_ERR("%s %d, Unknown Bank: %lx\n", __FUNCTION__, __LINE__, u32Bank);
            break;
    }

    return (u32)enType;
}

void _HalDispUtilityClearIndexBuffer(HalDispUtilityRegConfigType_e enType, u32 u32CtxId)
{
    u16 i;
    u32 *pu32IndexBuffer = NULL;
    //u32 u32Mask;

    if(gpvIndexBuffer)
    {
        DrvDispOsObtainMutex(&gstDispUtilityMutexCfg);

        pu32IndexBuffer = (u32 *)(gpvIndexBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType));
        //u32Mask = u32CtxId ? 0x0000FFFF : 0xFFFF0000;
        for(i=0; i<HAL_DISP_UTILIYT_REG_NUM; i++)
        {
            pu32IndexBuffer[i] = pu32IndexBuffer[i] & u32CtxId;
        }
        DrvDispOsReleaseMutex(&gstDispUtilityMutexCfg);
    }
}


void _HalDispUtilityReadBankCpyToBuffer(HalDispUtilityRegConfigType_e enType)
{
    u32 u32Bank = 0;
    void *pvBankAddr = NULL;

    if (gpvRegBuffer)
    {
        u32Bank     = _HalDispUtilityTransTypeToBank(enType);
        pvBankAddr  = RIU_GET_ADDR(u32Bank);

        DrvDispOsObtainMutex(&gstDispUtilityMutexCfg);
        memcpy((void *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType)), (void *)pvBankAddr, HAL_DISP_UTILITY_REG_BANK_SIZE);
        DrvDispOsReleaseMutex(&gstDispUtilityMutexCfg);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Type [%d] Bank [%lx] Addr [%lx]\n",
                __FUNCTION__, __LINE__,
                enType, u32Bank, (u32)pvBankAddr);

        if( DISP_DBG_CHECK(DISP_DBG_LEVEL_UTILITY_CMDQ) )
        {
            u8 *pu8Src = DrvDispOsMemAlloc(512 * 8);
            u8 *pu8RegBuf = (u8 *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType));
            u16 idx;

            if(pu8Src)
            {
                DrvDispOsSprintfStrcat(pu8Src, "Bank:%lx\n", u32Bank);
                for(idx=0; idx<0x80; idx++)
                {
                    if( (idx%8) == 0)
                    {
                        DrvDispOsSprintfStrcat(pu8Src, "%02X: ", idx);
                    }

                    DrvDispOsSprintfStrcat(pu8Src, "0x%02X%02X ", pu8RegBuf[idx*4+1], pu8RegBuf[idx*4]);

                    if( ((idx+1)%8) == 0 )
                    {
                        DrvDispOsSprintfStrcat(pu8Src, "\n");
                    }
                }
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s\n", pu8Src);
                DrvDispOsMemRelease(pu8Src);
            }
        }
    }
}

void _HalDispUtilityReadBufferCpyToBank(HalDispUtilityRegConfigType_e enType)
{
    u32 u32Bank = 0;
    void *pvBankAddr = NULL;

    if (gpvRegBuffer)
    {
        u32Bank     = _HalDispUtilityTransTypeToBank(enType);
        pvBankAddr  = RIU_GET_ADDR(u32Bank);

        DrvDispOsObtainMutex(&gstDispUtilityMutexCfg);
        memcpy((void *)pvBankAddr, (void *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType)), HAL_DISP_UTILITY_REG_BANK_SIZE);
        DrvDispOsReleaseMutex(&gstDispUtilityMutexCfg);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Type [%d] Bank [%lx] Addr [%lx]\n",
                __FUNCTION__, __LINE__,
                enType, u32Bank, (u32)pvBankAddr);
    }
}

u32 _HalDispUtilityGetMaxType(void)
{
    return (u32)E_DISP_UTILITY_REG_CONFIG_NUM;
}

void* _HalDispUtilityGetRegBuffer(HalDispUtilityCmdqContext_t *pCtx)
{
    return pCtx->pvRegBuffer;
}

void * _HalDispUtilityGetIndexBuffer(HalDispUtilityCmdqContext_t *pCtx)
{
    return pCtx->pvIndexBuffer;
}

void* _HalDispUtilityGetInquireBuffer(HalDispUtilityCmdqContext_t *pCtx)
{
    return pCtx->pvInquireBuffer;
}

void* _HalDispUtilityGetDirectCmdqBuffer(HalDispUtilityCmdqContext_t *pCtx)
{
    return pCtx->pvDirectCmdqBuffer;
}

void* _HalDispUtilityGetInquireTblCnt(HalDispUtilityCmdqContext_t *pCtx)
{
    return (void *)&pCtx->u32TblCnt;
}

void* _HalDispUtilityGetDirectCmdqCnt(HalDispUtilityCmdqContext_t *pCtx)
{
    return (void *)&pCtx->u32DirectCmdqCnt;
}

bool _HalDispUtilitySetCmdqBufferIdx(u16 u16TblCnt, u32 u32Reg, HalDispUtilityRegConfigType_e enType, u32 u32TypeMax, void *pvInexBuffer, s32 s32CmdqId)
{
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;

    u16Addr = (u16)(u32Reg & 0xFE); // Only low byte

    if ((enType < u32TypeMax) && pvInexBuffer)
    {
        pu16BankAddr = s32CmdqId ?
                       (u16 *)(pvInexBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType) + (u16Addr << 1) + 2) :
                       (u16 *)(pvInexBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType) + (u16Addr << 1));

        *pu16BankAddr = (u16TblCnt | HAL_DISP_UTILITY_FLAG_CMDQ_IDX_EXIST);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqId:%ld, SetIdx : Reg [%lx] RegBufAddr [%lx] Val [%hx]\n",
            __FUNCTION__, __LINE__,
            s32CmdqId,
            u32Reg, (u32)pu16BankAddr,
            *pu16BankAddr);
        return 1;
    }
    else
    {
        return 0;
    }
}

bool _HalDispUtilityGetCmdqBufferIdx(u16 *pu16TblCnt, u32 u32Reg, HalDispUtilityRegConfigType_e enType, u32 u32TypeMax, void *pvInexBuffer, s32 s32CmdqId)
{
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;
    bool bNewBufPos = 0;

    u16Addr = (u16)(u32Reg & 0xFE); // Only low byte

    if ((enType < u32TypeMax) && pvInexBuffer)
    {
        pu16BankAddr = (s32CmdqId) ?
                       (u16 *)(pvInexBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType) + (u16Addr << 1) + 2) :
                       (u16 *)(pvInexBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType) + (u16Addr << 1));


        bNewBufPos = (*pu16BankAddr &  HAL_DISP_UTILITY_FLAG_CMDQ_IDX_EXIST) ? 0 : 1;

        *pu16TblCnt = (*pu16BankAddr ) & (~HAL_DISP_UTILITY_FLAG_CMDQ_IDX_EXIST);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqId:%ld, GetIdx : Reg [%lx] RegBufAddr [%lx] Val [%x]\n",
            __FUNCTION__, __LINE__,
            s32CmdqId,
            u32Reg, (u32)pu16BankAddr, *pu16BankAddr);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqId:%ld, GetIdx : Reg [%lx] Cnt [%hx] %s exist\n",
            __FUNCTION__, __LINE__,
            s32CmdqId,
            u32Reg, (u16)*pu16TblCnt,
            bNewBufPos ? "not" : "arleady");
    }
    else
    {
        bNewBufPos = 1;
    }
    return bNewBufPos;
}

void _HalDispUtilityFillCmdqStruct(HalDispUtilityInquire_t* pstInq, u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    u32 u32Bank = 0;
    HalDispUtilityCmdqContext_t *pvCmdqCtx = (HalDispUtilityCmdqContext_t *)pCtx;
    u32Bank = (u32Reg & 0xFFFF00);

    pstInq->enType          = _HalDispUtilityTransBankToType(u32Bank, (u32)pvCmdqCtx->s32CmdqId);
    pstInq->u32TypeMax      = _HalDispUtilityGetMaxType();
    pstInq->stCmd.u32Addr   = u32Reg;
    pstInq->stCmd.u16Mask   = u16Mask;
    pstInq->stCmd.u16Data   = u16Val;
    pstInq->bSet            = (pstInq->enType < pstInq->u32TypeMax) ? 1 : 0;
    pstInq->pCtx            = pCtx;
}

u16 _HalDispUtilityReadRegBuffer(u32 u32Reg, void *pvRegBuffer, u32 u32CtxId)
{
    u32 u32Bank = 0;
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;
    u32 u32Type = 0;
    u32 u32TypeMax = 0;

    if (pvRegBuffer)
    {
        u32Bank = (u32Reg & 0xFFFF00);
        u16Addr = (u16)(u32Reg & 0xFF);

        u32Type     = _HalDispUtilityTransBankToType(u32Bank, u32CtxId);
        u32TypeMax  = _HalDispUtilityGetMaxType();

        if (u32Type < u32TypeMax)
        {
            pu16BankAddr = (u16 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*u32Type) + (u16Addr<<1));
            return *pu16BankAddr;
        }
        else
        {
            DISP_ERR("%s %d, Type [%ld] Wrong\n", __FUNCTION__, __LINE__, u32Type);
            return 0;
        }
    }
    else
    {
        return 0;
    }
}


u8 _HalDispUtilityWrite1ByteRegBuffer(HalDispUtilityInquire_t* pstInq)
{
    u16 u16Addr = 0;
    u8 *pu8BankAddr = NULL;
    u16 *pu16BankAddr = NULL;
    u32 u32LocalReg = 0;
    void *pvRegBuffer = NULL;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    u32LocalReg = (pstInq->stCmd.u32Addr & 0xFE);
    u16Addr     = (u16)(pstInq->stCmd.u32Addr & 0xFF);

    if (pstInq->bSet && pvRegBuffer)
    {
        // (((u32Reg)<<1) - ((u32Reg) & 1))
        pu8BankAddr = (u8 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*pstInq->enType) + ((u16Addr<<1) - ((u16Addr) & 1)));
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, W1Byte : Reg [%lx] Val [%hx] Mask [%hx] OriRegVal [%hx]\n",
            __FUNCTION__, __LINE__,
            pstInq->stCmd.u32Addr, (u8)pstInq->stCmd.u16Data, (u8)pstInq->stCmd.u16Mask, (u8)*pu8BankAddr);

        if (pstInq->stCmd.u16Mask == 0xFF)
        {
            *pu8BankAddr = (u8)pstInq->stCmd.u16Data;
        }
        else
        {
            *pu8BankAddr = (u8)(((u8)(pstInq->stCmd.u16Data & pstInq->stCmd.u16Mask)) | (*pu8BankAddr & ((u8)(~pstInq->stCmd.u16Mask))));
        }

        pu16BankAddr = (u16 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*pstInq->enType) + (u32LocalReg<<1));
        pstInq->stCmd.u16Data = *pu16BankAddr; // The inquire data will be modified here.

        return 1;
    }
    else
    {
        return 0;
    }
}

u16 _HalDispUtilityWrite2ByteRegBuffer(HalDispUtilityInquire_t* pstInq)
{
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;
    void *pvRegBuffer = NULL;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    u16Addr     = (u16)(pstInq->stCmd.u32Addr & 0xFF);

    if (pstInq->bSet && pvRegBuffer)
    {
        pu16BankAddr = (u16 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE * pstInq->enType) + (u16Addr<<1));
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, W2Byte : Reg [%lx] Val [%hx] Mask [%hx] OriRegVal [%hx]\n",
            __FUNCTION__, __LINE__,
            pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask, *pu16BankAddr);

        if (pstInq->stCmd.u16Mask == 0xFFFF)
        {
            *pu16BankAddr = pstInq->stCmd.u16Data;
        }
        else
        {
            *pu16BankAddr = (u16)((pstInq->stCmd.u16Data & pstInq->stCmd.u16Mask) | (*pu16BankAddr & (~pstInq->stCmd.u16Mask)));
        }
        pstInq->stCmd.u16Data = *pu16BankAddr; // The inquire data will be modified here.

        return 1;
    }
    else
    {
        DISP_ERR("%s %d, W2Byte Fail : Type [%d] MaxType [%lu] Reg [%lx] Val [%hx] Mask [%hx]\n",
            __FUNCTION__, __LINE__,
                pstInq->enType, pstInq->u32TypeMax, pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);
        return 0;
    }
}

u16 _HalDispUtilityWrite2ByteInquireBuffer(HalDispUtilityInquire_t* pstInq)
{
    HalDispUtilityCmdReg_t  *pstCmdReg = NULL;
    u16 u16BufferCnt = 0;
    bool bNewBufferCnt = 0;
    bool bHByte = 0;
    u32  u32LocalReg = 0;
    void *pvIndexBuffer = NULL;
    void *pvInquireBuffer = NULL;
    u32  *pu32TblCnt = NULL;
    s32 s32CmdqId = 0;

    pvIndexBuffer   = _HalDispUtilityGetIndexBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    pvInquireBuffer = _HalDispUtilityGetInquireBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    pu32TblCnt      = (u32 *)_HalDispUtilityGetInquireTblCnt(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    s32CmdqId       = ((HalDispUtilityCmdqContext_t *)pstInq->pCtx)->s32CmdqId;

    u32LocalReg     = (pstInq->stCmd.u32Addr & 0xFFFFFE); // For 1Byte ignore Hbyte
    bHByte          = (pstInq->stCmd.u32Addr & 0x1);

    bNewBufferCnt   = _HalDispUtilityGetCmdqBufferIdx(&u16BufferCnt, u32LocalReg, pstInq->enType, pstInq->u32TypeMax, pvIndexBuffer, s32CmdqId);

    if (pstInq->bSet && pvInquireBuffer)
    {
        if (*pu32TblCnt > HAL_DISP_UTILITY_CMDQ_BUFFER_CNT)
        {
            DISP_ERR("%s %d, InquireBuf Full\n", __FUNCTION__, __LINE__);
            return 0;
        }

        if (bNewBufferCnt)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Not exist inquire : Cnt [%lu] Reg [%lx] Val [%hx] Msk [%hx]\n",
                __FUNCTION__, __LINE__,
                (*pu32TblCnt), u32LocalReg, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);

            _HalDispUtilitySetCmdqBufferIdx((*pu32TblCnt), u32LocalReg, pstInq->enType, pstInq->u32TypeMax, pvIndexBuffer, s32CmdqId);

            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
            pstCmdReg->u32Addr = u32LocalReg;
            pstCmdReg->u16Mask = pstInq->stCmd.u16Mask << ((bHByte) ? 8 : 0);
            (*pu32TblCnt)++;
        }
        else
        {
            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + (u16BufferCnt * sizeof(HalDispUtilityCmdReg_t)));

            if (pstCmdReg->u32Addr != u32LocalReg)
            {
                // If has only one not to create inst, may will in this condition.
                DISP_ERR("%s %d, Inquire & Reg NOT MATCH [%lx][%lx]\n",
                    __FUNCTION__, __LINE__,
                    u32LocalReg, pstCmdReg->u32Addr);

                DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Not exist inquire2 : Cnt [%lu] Reg [%lx] Val [%hx] Msk [%hx]\n",
                    __FUNCTION__, __LINE__,
                    (*pu32TblCnt), u32LocalReg, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);

                _HalDispUtilitySetCmdqBufferIdx((*pu32TblCnt), u32LocalReg, pstInq->enType, pstInq->u32TypeMax, pvIndexBuffer, s32CmdqId);

                pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
                pstCmdReg->u32Addr = u32LocalReg;
                pstCmdReg->u16Mask = pstInq->stCmd.u16Mask << ((bHByte) ? 8 : 0);
                (*pu32TblCnt)++;
            }
            else
            {
                pstCmdReg->u16Mask = (pstCmdReg->u16Mask | (pstInq->stCmd.u16Mask << ((bHByte) ? 8 : 0)));
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Exist inquire : Cnt [%hu] Reg [%lx] Val [%hx] Msk [%hx]\n",
                        __FUNCTION__, __LINE__,
                        u16BufferCnt, pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);
            }
        }

        pstCmdReg->u16Data = pstInq->stCmd.u16Data; // The pstInq->stCmd.u16Data is modified at _HalDispUtilityWrite2ByteRegBuffer()
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Real Reg [%lx] Val [%hx] Msk [%hx]\n",
            __FUNCTION__, __LINE__,
            pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);

        return pstCmdReg->u16Data;
    }
    else
    {
        DISP_ERR("%s %d, W2Byte Fail : Type [%d] MaxType [%lu] Reg [%lx] Val [%hx] Msk [%hx]\n",
            __FUNCTION__, __LINE__,
            pstInq->enType, pstInq->u32TypeMax, pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);
        return 0;
    }
}

u16 _HalDispUtilityWrite2ByteInquireBufferWithoutRegBuf(HalDispUtilityInquire_t* pstInq)
{
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;
    bool bHByte = 0;
    u32  u32LocalReg = 0;
    u32  *pu32TblCnt = NULL;
    void *pvInquireBuffer = NULL;

    pvInquireBuffer = _HalDispUtilityGetInquireBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    pu32TblCnt      = (u32*)_HalDispUtilityGetInquireTblCnt(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));

    u32LocalReg     = (pstInq->stCmd.u32Addr & 0xFFFFFE); // For 1Byte ignore Hbyte
    bHByte          = (pstInq->stCmd.u32Addr & 0x1);

    if (*pu32TblCnt > HAL_DISP_UTILITY_CMDQ_BUFFER_CNT)
    {
        DISP_ERR("%s %d, InquireBuf Full\n", __FUNCTION__, __LINE__);
        return 0;
    }

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Exception add : Cnt [%lu] Reg [%lx] Val [%hx] Msk [%hx]\n",
        __FUNCTION__, __LINE__,
        (*pu32TblCnt), u32LocalReg, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);

    pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
    pstCmdReg->u32Addr = u32LocalReg;
    pstCmdReg->u16Mask = pstInq->stCmd.u16Mask << ((bHByte) ? 8 : 0);
    pstCmdReg->u16Data = (pstInq->stCmd.u16Data & pstInq->stCmd.u16Mask) << ((bHByte) ? 8 : 0);
    (*pu32TblCnt)++;

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Exception add : Real Reg [%lx] Val [%hx] Msk [%hx]\n",
        __FUNCTION__, __LINE__,
        pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);

    return pstCmdReg->u16Data;
}
void _HalDispUtilityFilpRIURegFire(void *pvCtxIn)
{
    u32 *pu32TblCnt = NULL;
    u32 u32Idx = 0;
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;
    void *pvInquireBuffer = NULL;

    pvInquireBuffer = _HalDispUtilityGetInquireBuffer(pCtx);

    if (pvInquireBuffer)
    {
        pu32TblCnt = _HalDispUtilityGetInquireTblCnt(pCtx);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d Use RIU, CmdNum:%ld\n",
            __FUNCTION__, __LINE__, *pu32TblCnt);

        for (u32Idx = 0; u32Idx < *pu32TblCnt; u32Idx++)
        {
           pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + (u32Idx * sizeof(HalDispUtilityCmdReg_t)));

           W2BYTEMSK(pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);

           DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx][0x%02lx] Val [0x%04x] Mask [0x%04x]\n",
                   __FUNCTION__, __LINE__,
                   (pstCmdReg->u32Addr >> 8),
                   (pstCmdReg->u32Addr & 0xFF) >> 1,
                   pstCmdReg->u16Data,
                   pstCmdReg->u16Mask);
        }
    }
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

bool HalDispUtilityInit(u32 u32CtxId)
{
    bool bRet = 1;
    u16 i;

    if(u32CtxId > HAL_DISP_UTILITY_CMDQ_NUM)
    {
        DISP_ERR("%s %d, CtxId(%ld) is bigger than CMDQ NUM (%d)\n",
            __FUNCTION__, __LINE__, u32CtxId, HAL_DISP_UTILITY_CMDQ_NUM);
    }

    if( _HalDispUtilityGetInitFlagByIdx(u32CtxId) )
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqCtx (%ld) already Init\n", __FUNCTION__, __LINE__, u32CtxId);
        return 1;
    }
    else
    {
        if( _HalDispUtilityGetInitFlag() == 0)
        {
            gu32DispUtilityRiuBaseVir = DISP_OS_VIR_RIUBASE;
            if(DrvDispOsCreateMutex(&gstDispUtilityMutexCfg) == 0)
            {
                DISP_ERR("%s %d, Create Mutex Fail\n", __FUNCTION__, __LINE__);
                return 0;
            }

            gpvRegBuffer = DrvDispOsMemAlloc(HAL_DISP_UTILITY_REG_BANK_NUM * HAL_DISP_UTILITY_REG_BANK_SIZE);
            if(gpvRegBuffer == NULL)
            {
                DISP_ERR("%s %d, Memalloc RegBuffer Fail\n", __FUNCTION__, __LINE__);
                return 0;
            }

            gpvIndexBuffer = DrvDispOsMemAlloc(HAL_DISP_UTILITY_REG_BANK_NUM * HAL_DISP_UTILITY_INDEX_BUFFER_SIZE);
            if(gpvIndexBuffer == NULL)
            {
                DrvDispOsMemRelease(gpvRegBuffer);
                DISP_ERR("%s %d, Memalloc RegBuffer Fail\n", __FUNCTION__, __LINE__);
                return 0;
            }
        }

        if(u32CtxId < HAL_DISP_UTILITY_CMDQ_NUM)
        {
            memset(&gstDispUtilityCmdqCtx[u32CtxId], 0, sizeof(HalDispUtilityCmdqContext_t));
            gstDispUtilityCmdqCtx[u32CtxId].s32CmdqId = u32CtxId;
            gstDispUtilityCmdqCtx[u32CtxId].u32TblCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].pvRegBuffer = gpvRegBuffer;
            gstDispUtilityCmdqCtx[u32CtxId].pvIndexBuffer = gpvIndexBuffer;
            gstDispUtilityCmdqCtx[u32CtxId].pvInquireBuffer = DrvDispOsMemAlloc( sizeof(HalDispUtilityCmdReg_t) * HAL_DISP_UTILITY_CMDQ_BUFFER_CNT);

            gstDispUtilityCmdqCtx[u32CtxId].u16RegFlipCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].u16SyncCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].u16WaitDoneCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].bEnSyncCnt = 0;

            if( gstDispUtilityCmdqCtx[u32CtxId].pvInquireBuffer == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d , CmdqId:%ld, Memalloc InquireBuffer Fail\n", __FUNCTION__, __LINE__, u32CtxId);
            }
            else
            {
                memset(gstDispUtilityCmdqCtx[u32CtxId].pvRegBuffer, 0, HAL_DISP_UTILITY_REG_BANK_NUM * HAL_DISP_UTILITY_REG_BANK_SIZE);
                memset(gstDispUtilityCmdqCtx[u32CtxId].pvInquireBuffer, 0, sizeof(HalDispUtilityCmdReg_t) * HAL_DISP_UTILITY_CMDQ_BUFFER_CNT);

                gstDispUtilityCmdqCtx[u32CtxId].u32DirectCmdqCnt = 0;
                gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer = DrvDispOsMemAlloc(sizeof(HalDispUtilityCmdReg_t) * HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);

                if(gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer == NULL)
                {
                    bRet = 0;
                    DISP_ERR("%s %d, Cmdq:%ld, DirectCmdqBUffer Memallc Failn", __FUNCTION__, __LINE__, u32CtxId);
                }
            }
        }

        if( _HalDispUtilityGetInitFlag() == 0)
        {
            for(i=0; i<E_DISP_UTILITY_REG_CONFIG_NUM; i++)
            {
                _HalDispUtilityReadBankCpyToBuffer(i);
                _HalDispUtilityClearIndexBuffer(i, u32CtxId);
            }
        }

        _HalDispUtilitySetInitFlagByIdx(u32CtxId, 1);
    }

    return bRet;
}


bool HalDispUtilityDeInit(u32 u32CtxId)
{
    if( _HalDispUtilityGetInitFlagByIdx(u32CtxId) )
    {
        gstDispUtilityCmdqCtx[u32CtxId].s32CmdqId = -1;
        gstDispUtilityCmdqCtx[u32CtxId].u32TblCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].u16RegFlipCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].u16SyncCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].u16WaitDoneCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].bEnSyncCnt = 0;

        DrvDispOsMemRelease(gstDispUtilityCmdqCtx[u32CtxId].pvInquireBuffer);
        gstDispUtilityCmdqCtx[u32CtxId].u32DirectCmdqCnt = 0;
        DrvDispOsMemRelease(gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer);
        _HalDispUtilitySetInitFlagByIdx(u32CtxId, 0);
    }

    if( _HalDispUtilityGetInitFlag() == 0 )
    {
        DrvDispOsMemRelease(gpvIndexBuffer);
        DrvDispOsMemRelease(gpvRegBuffer);
        DrvDispOsDestroyMutex(&gstDispUtilityMutexCfg);
    }

    return 1;
}

bool HalDispUtilityGetCmdqContext(void **pvCtx, u32 u32CtxId)
{
    bool bRet = 1;
    if(u32DispUtilityCmdqInitFlag)
    {
        *pvCtx = (void *)&gstDispUtilityCmdqCtx[u32CtxId];
    }
    else
    {
        *pvCtx = NULL;
        bRet = 0;
        DISP_ERR("%s %d, No Disp Utility Init\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

void HalDispUtilitySetCmdqInf(void *pCmdqInf, u32 u32CtxId)
{
    gstDispUtilityCmdqCtx[u32CtxId].pvCmdqInf = pCmdqInf;
}

u16 HalDispUtilityR2BYTEDirect(u32 u32Reg)
{
    u16 u16RetVal = 0;

    u16RetVal = R2BYTE(u32Reg);
    return u16RetVal;
}

u16 HalDispUtilityR2BYTEMaskDirect(u32 u32Reg, u16 u16Mask)
{
    u16 u16RetVal = 0;

    u16RetVal = R2BYTEMSK(u32Reg, u16Mask);
    return u16RetVal;
}

u32 HalDispUtilityR4BYTEDirect(u32 u32Reg)
{
    u32 u32RetVal = 0;

    u32RetVal = R4BYTE(u32Reg);
    return u32RetVal;
}

u16 HalDispUtilityR2BYTERegBuf(u32 u32Reg, void *pvCtxIn)
{
    u16 u16RetVal = 0;
    void *pvRegBuffer = NULL;
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(pCtx);
    if (pvRegBuffer)
    {
        u16RetVal = _HalDispUtilityReadRegBuffer(u32Reg, pvRegBuffer, (u32)pCtx->s32CmdqId);
    }
    return u16RetVal;
}

void HalDispUtilityWBYTEMSKRegBuf(u32 u32Reg, u8 u8Val, u8 u8Mask, void *pCtx)
{
    HalDispUtilityInquire_t stInq;
    u16 u16Val = 0, u16Mask = 0;

    u16Val  = u8Val;
    u16Mask = u8Mask;
    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pCtx);
    _HalDispUtilityWrite1ByteRegBuffer(&stInq);
}

void HalDispUtilityWBYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pCtx);

    if (_HalDispUtilityWrite1ByteRegBuffer(&stInq))
    {
        _HalDispUtilityWrite2ByteInquireBuffer(&stInq);
    }
    else
    {
        _HalDispUtilityWrite2ByteInquireBufferWithoutRegBuf(&stInq);
    }
}

void HalDispUtilityWBYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    HalDispUtilityWBYTEMSKRegBuf(u32Reg, (u8)u16Val, (u8)u16Mask, pCtx);
    WBYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityWBYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityWBYTEMSKInquireBuf(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
    else
    {
        HalDispUtilityWBYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKRegBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pCtx);
    _HalDispUtilityWrite2ByteRegBuffer(&stInq);
}

void HalDispUtilityW2BYTEInqireBuf(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, 0xFFFF, pvCtxIn);

    if (_HalDispUtilityWrite2ByteRegBuffer(&stInq))
    {
        _HalDispUtilityWrite2ByteInquireBuffer(&stInq);
    }
    else
    {
        _HalDispUtilityWrite2ByteInquireBufferWithoutRegBuf(&stInq);
    }
}

void HalDispUtilityW2BYTEDirect(u32 u32Reg, u16 u16Val, void *pCtx)
{
    HalDispUtilityW2BYTEMSKRegBuf(u32Reg, u16Val, 0xFFFF, pCtx);
    W2BYTE(u32Reg, u16Val);
}

void HaDisplUtilityW2BYTE(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEInqireBuf(u32Reg, u16Val, pvCtxIn);
    }
    else
    {
        HalDispUtilityW2BYTEDirect(u32Reg, u16Val, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pCtx);

    if (_HalDispUtilityWrite2ByteRegBuffer(&stInq))
    {
        _HalDispUtilityWrite2ByteInquireBuffer(&stInq);
    }
    else
    {
        _HalDispUtilityWrite2ByteInquireBufferWithoutRegBuf(&stInq);
    }
}

void HalDispUtilityW2BYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCtx)
{
    HalDispUtilityW2BYTEMSKRegBuf(u32Reg, u16Val, u16Mask, pCtx);
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSKInquireBuf(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKDirectCmdq(void *pvCtxIn, u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;
    u32 *pu32TblCnt = NULL;
    void *pvTblBuffer = NULL;
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer = _HalDispUtilityGetDirectCmdqBuffer(pCtx);
    pu32TblCnt  = _HalDispUtilityGetDirectCmdqCnt(pCtx);

    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32TblCnt >= HAL_DISP_UTILITY_DIRECT_CMDQ_CNT)
        {
            DISP_ERR("%s %d, Cnt [%ld] over max [%d] \n",
                __FUNCTION__, __LINE__,
                *pu32TblCnt, HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);

            return;
        }

        pstCmdReg = (HalDispUtilityCmdReg_t *)(pvTblBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
        pstCmdReg->u32Addr = u32Reg;
        pstCmdReg->u16Mask = u16Mask;
        pstCmdReg->u16Data = u16Val;
        (*pu32TblCnt)++;
    }
    else
    {
        W2BYTEMSK(u32Reg, u16Val, u16Mask);
    }
}

void HalDispUtilityW2BYTEMSKDirectCmdqWrite(void *pvCtxIn)
{
    u32 i = 0;
    HalDispUtilityCmdReg_t *pstCmdReg = 0;
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    u32 *pu32TblCnt = NULL;
    void *pvTblBuffer = NULL;
    s32 s32CmdqRet = 0;
    u32 u32CmdqState = 0;
    u32 u32CmdqInfRetryCnt = 0;

    if (pCtx)
    {
        pstCmdqInf = pCtx->pvCmdqInf;
    }

    pvTblBuffer = _HalDispUtilityGetDirectCmdqBuffer(pCtx);
    pu32TblCnt  = _HalDispUtilityGetDirectCmdqCnt(pCtx);

    if (pvTblBuffer && *pu32TblCnt > 0)
    {
        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Direct Cmdq\n", __FUNCTION__, __LINE__);

            for (i = 0; i < *pu32TblCnt; i++)
            {
                pstCmdReg = (HalDispUtilityCmdReg_t *)(pvTblBuffer + (i * sizeof(HalDispUtilityCmdReg_t)));
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx][0x%02lx] Val [0x%04x] Mask [0x%04x]\n",
                    __FUNCTION__, __LINE__,
                    (pstCmdReg->u32Addr & 0xFFFF00) >> 8,
                    (pstCmdReg->u32Addr & 0xFF) >> 1,
                    pstCmdReg->u16Data,
                    pstCmdReg->u16Mask);
            }

            while (0 == (s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CheckBufAvailable(pstCmdqInf, *pu32TblCnt + 10)))
            {
                s32CmdqRet = pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq(pstCmdqInf, (MS_U32*)&u32CmdqState);
                if (s32CmdqRet)
                {
                    //DISP_ERR("%s %d, MHAL_CMDQ_ReadStatusCmdq Error\n", __FUNCTION__, __LINE__);
                }

                if ((u32CmdqState & MHAL_CMDQ_ERROR_STATUS) != 0)
                {
                    DISP_ERR("%s %d, Cmdq Status Error\n", __FUNCTION__, __LINE__);
                    pstCmdqInf->MHAL_CMDQ_CmdqResetEngine(pstCmdqInf);
                }

                DISP_ERR("%s %d, Cmdq is Busy, Retry [%ld]\n",
                    __FUNCTION__, __LINE__, u32CmdqInfRetryCnt);
            }

            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti(pstCmdqInf,
                                                                    (MHAL_CMDQ_MultiCmdBufMask_t *)pvTblBuffer,
                                                                    *pu32TblCnt);
            if (s32CmdqRet < *pu32TblCnt)
            {
                DISP_ERR("%s %d, DirectCmdqTbl Cmdq Write Multi Fail, Ret [%ld] Cnt [%ld]\n",
                    __FUNCTION__, __LINE__, s32CmdqRet, *pu32TblCnt);
            }
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Use RIU\n", __FUNCTION__, __LINE__);

            for (i = 0; i < *pu32TblCnt; i++)
            {
                pstCmdReg = (HalDispUtilityCmdReg_t *)(pvTblBuffer + (i* sizeof(HalDispUtilityCmdReg_t)));
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx][0x%02lx] Val [0x%04x] Mask [0x%04x]\n",
                    __FUNCTION__, __LINE__,
                    (pstCmdReg->u32Addr & 0xFFFF00) >> 8,
                    (pstCmdReg->u32Addr & 0xFF) >> 1,
                    pstCmdReg->u16Data,
                    pstCmdReg->u16Mask);

                W2BYTEMSK(pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);
            }
        }

        (*pu32TblCnt) = 0;
    }
}

void HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityFilpRegFire(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = (MHAL_CMDQ_CmdqInterface_t *)pCtx->pvCmdqInf;
    void *pvInquireBuffer = NULL;
    u32 *pu32TblCnt = NULL;
    s32 s32CmdqRet = 0;
    u32 u32CmdqState = 0;
    u32 u32CmdqInfRetryCnt = 0;
    u32 i = 0;
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;

    pvInquireBuffer = _HalDispUtilityGetInquireBuffer(pCtx);

    if ((HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ) && pstCmdqInf)
    {

        pu32TblCnt = _HalDispUtilityGetInquireTblCnt(pCtx);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Use Cmdq, CmdNum=%ld\n",
            __FUNCTION__, __LINE__, *pu32TblCnt);

        for (i = 0; i < *pu32TblCnt; i++)
        {
            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvInquireBuffer + (i* sizeof(HalDispUtilityCmdReg_t)));
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx][0x%02lx] Val [0x%04x] Mask [0x%04x]\n",
                    __FUNCTION__, __LINE__,
                    (pstCmdReg->u32Addr & 0xFFFF00) >> 8,
                    (pstCmdReg->u32Addr & 0xFF) >> 1,
                    pstCmdReg->u16Data,
                    pstCmdReg->u16Mask);
        }

        while (0 == (s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CheckBufAvailable(pstCmdqInf, *pu32TblCnt + 10)))
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq(pstCmdqInf, (MS_U32*)&u32CmdqState);
            if (s32CmdqRet)
            {
                //DISP_ERR("%s %d, MHAL_CMDQ_ReadStatusCmdq Error\n", __FUNCTION__, __LINE__);
            }

            if ((u32CmdqState & MHAL_CMDQ_ERROR_STATUS) != 0)
            {
                DISP_ERR("%s %d, Cmdq Status Error\n", __FUNCTION__, __LINE__);
                pstCmdqInf->MHAL_CMDQ_CmdqResetEngine(pstCmdqInf);
            }
            DISP_ERR("%s %d, Cmdq is Busy, Retry [%ld]\n", __FUNCTION__, __LINE__, u32CmdqInfRetryCnt);
        }

        if (pvInquireBuffer)
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti(pstCmdqInf, (MHAL_CMDQ_MultiCmdBufMask_t *)pvInquireBuffer, *pu32TblCnt);
            if (s32CmdqRet < *pu32TblCnt)
            {
                DISP_ERR("%s %d, InquireTbl Cmdq Write Multi Fail! Ret [%ld] Cnt [%ld]\n",
                    __FUNCTION__, __LINE__, s32CmdqRet, *pu32TblCnt);
            }
        }
    }
    else
    {
        _HalDispUtilityFilpRIURegFire(pCtx);
    }
}

bool _HalDispUtilityPollWait(void *pvCmdqInf, u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = (MHAL_CMDQ_CmdqInterface_t *)pvCmdqInf;
    s32 s32CmdqRet = 0;
    u32 u32WaitCnt = 0;
    bool bRet = 1;

    if (pstCmdqInf == NULL || pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits == NULL)
    {
        while (!(HalDispUtilityR2BYTEDirect(u32Reg) & (u16Mask & u16Val)))
        {
            if (++u32WaitCnt > 200)
            {
                return 1;
            }
            DrvDispOsUsSleep(((u32PollTime/10000) ? (u32PollTime/10000) : 1)); // Wait time ns/1000
        }
    }
    else
    {
        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits_ByTime(pstCmdqInf, u32Reg, u16Val, u16Mask, TRUE, u32PollTime);
        if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
        {
            bRet = 0;
            DISP_ERR("%s %d, CmdqPollRegBits_ByTime [%lu] Fail\n", __FUNCTION__, __LINE__, u32PollTime);
        }
    }
    return bRet;
}


bool HalDispUtilityAddWaitCmd(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    MHAL_CMDQ_EventId_e     enCmdqEvent = 0;
    s32 s32CmdqRet = 0;
    bool bRet = 1;

    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (pCtx)
        {
            pstCmdqInf  = pCtx->pvCmdqInf;
        }

        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd)
        {
            enCmdqEvent =  HAL_DISP_UTILITY_CMDQ0_WAIT_DONE_EVENT(pCtx->s32CmdqId);
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdqInf, enCmdqEvent);
            if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
            {
                bRet = 0;
                DISP_ERR("%s %d, CmdqAddWaitEventCmd Fail\n", __FUNCTION__, __LINE__);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d\n", __FUNCTION__, __LINE__);
            }
        }
    }
    return bRet;
}

bool HalDispUtilityAddDelayCmd(void *pvCtxIn, u16 u16PollingTime)
{
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    s32 s32CmdqRet = 0;
    bool bRet = 1;

    if (pCtx)
    {
        pstCmdqInf  = pCtx->pvCmdqInf;
    }

    if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdDelay)
    {
        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdDelay(pstCmdqInf, 1000*u16PollingTime);
        if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
        {
            bRet = 0;
            DISP_ERR("%s %d, CmdDelay Fail\r\n", __FUNCTION__, __LINE__ );
        }
    }
    else
    {
        bRet = 0;
    }

    return bRet;
}

void HalDispUtilitySetRegAccessMode(u32 u32Mode)
{
    geDispRegAccessMode = u32Mode;
}

u32 HalDispUtilityGetRegAccessMode(void)
{
    return (u32)geDispRegAccessMode;
}

void HalDispUtilityResetIndexBuffer(void *pvCtxIn)
{
    u16 i;
    u32 *pu32TblCnt = NULL;
    HalDispUtilityCmdqContext_t *pCtx = NULL;

    pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    pu32TblCnt = _HalDispUtilityGetInquireTblCnt(pCtx);

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Cmdq_%ld, Rest Index Buffer, TblCnt=%ld\n",
        __FUNCTION__, __LINE__,
        pCtx->s32CmdqId, *pu32TblCnt);

    for(i=0; i<E_DISP_UTILITY_REG_CONFIG_NUM; i++)
    {
        _HalDispUtilityClearIndexBuffer(i, (u32)pCtx->s32CmdqId);
    }

    *pu32TblCnt = 0;
}

void HalDispUtilityResetCmdqCnt(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pCtx->u16RegFlipCnt = 0;
    pCtx->u16SyncCnt = 0;
    pCtx->u16WaitDoneCnt = 0;
    pCtx->bEnSyncCnt = 0;
}

