/*
* halHDCPTx.h- Sigmastar
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

#ifndef _HAL_HDCPTX_H_
#define _HAL_HDCPTX_H_


#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define SHA160LENGTH            5UL
#define BLOCKSIZE               64UL
#define BLOCKSIZE2              BLOCKSIZE << 1

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    MS_U32   m_auiBuf[SHA160LENGTH]; //Maximum for SHA256
    MS_U32   m_auiBits[2];
    MS_U8   m_aucIn[BLOCKSIZE2];    //128 bytes for SHA384, SHA512
#ifndef HDMITX_OS_TYPE_UBOOT
} __attribute__((__packed__)) SHA1_DATA;
#else
} SHA1_DATA;
#endif
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
extern void MHal_HDMITx_HdcpDebugEnable(MS_BOOL benable);
extern void MHal_HDMITx_GetHdcpKey(MS_BOOL useinternalkey, MS_U8 *data);
extern void MHal_HDMITx_HdcpKeyInit(MS_BOOL useinternalkey);
extern void MHal_HDMITx_HdcpInit(void);
extern MS_BOOL MHal_HDMITx_HdcpRxActive(void);
extern void MHal_HDMITx_HdcpSetEncrypt(MS_BOOL enable);
extern MS_BOOL MHal_HDMITx_HdcpCheckBksvLn(void);
extern MS_BOOL MHal_HDMITx_HdcpWriteAn(MS_U8 internal);
extern MS_BOOL MHal_HDMITx_HdcpWriteAksv(void);
extern void MHal_HDMITx_GET74(MS_U8 udata, MS_U8 *pdata);
extern void MHal_HDMITx_HdcpStartCipher(void);
extern void MHal_HDMITx_HdcpAuthPass(void);
extern MS_BOOL MHal_HDMITx_HdcpCheckRi(MS_U16 *ptx_ri, MS_U16 *prx_ri);
extern void MHal_HDMITx_HdcpSha1Init(SHA1_DATA *dig);
extern void MHal_HDMITx_HdcpSha1AddData(SHA1_DATA *dig, MS_U8 const * pcData, MS_U16  iDataLength);
extern void MHal_HDMITx_HdcpSha1FinalDigest(SHA1_DATA *dig, MS_U8* pcDigest);
extern void MHal_HDMITx_DdcRam_WriteEdidData(MS_U8 HDMI_Port, MS_U8 * HDMI_Data, MS_U32 u32Size, MS_BOOL bDDCheck);
extern void MHal_HDMITx_DDCRam_Access(void);

// *************  For customer NDS **************//
extern MS_BOOL MHal_HDMITx_HDCP_Get_BCaps(MS_U8 *u8bcaps);
extern MS_BOOL MHal_HDMITx_HDCP_Get_BStatus(MS_U16 *u16bstatus);
extern MS_BOOL MHal_HDMITx_HDCP_Get_BKSV(MS_U8 *u8bksv);
extern void MHal_HDMITx_HDCP_SHA1_Transform(unsigned int *sha1, const MS_U8 *s);
#ifdef __cplusplus
}
#endif


#endif // _HAL_HDCPTX_H_

