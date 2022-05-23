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

#ifndef __RTSP_H__
#define __RTSP_H__

#include <map>
#include <string>

#include "mi_common.h"

#include "tem.h"

#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "Live555RTSPServer.hh"

#include "sys.h"

typedef struct
{
    void *pDataAddr;
    void *pDataAddrStart;
    void *pDataAddrAlign;
    MI_U32 u32DataLen;
    MI_BOOL bExit;
}stRtspDataPackage_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_condattr_t condattr;
}stRtspDataMutexCond_t;
typedef struct
{
    unsigned int totalsize;
    std::vector<stRtspDataPackage_t> package;
}stRtspDataPackageHead_t;
typedef struct stRtspInputInfo_s
{
    unsigned int uintCurInPortId;
    unsigned int uintPreModChnId;
    int intUseBulPool;
    int intEncodeType;
    Sys *pInstance;
}stRtspInputInfo_t;

class Rtsp: public Sys
{
    public:
        Rtsp();
        virtual ~Rtsp();
    private:
        virtual void Init();
        virtual void Deinit();
        virtual void BindBlock(stModInputInfo_t & stIn);
        virtual void UnBindBlock(stModInputInfo_t & stIn);
        virtual void Start();
        virtual void Stop();

        Live555RTSPServer *pRTSPServer;
        unsigned char bOpenOnvif;
        static void *OpenStream(char const *szStreamName, void *arg);
        static int CloseStream(void *handle, void *arg);
        static int ReadStream(void *handle, unsigned char *ucpBuf, int BufLen, struct timeval *p_Timestamp, void *arg);
        static MI_S32 TermBufPool(void);
        static MI_BOOL BufPoolEmptyAndWait(void);
        static MI_S32 OpenBufPool(MI_VENC_CHN stVencChn);
        static MI_S32 CloseBufPool(MI_VENC_CHN stVencChn);
        static MI_S32 DequeueBufPool(MI_VENC_CHN stVencChn, void *pData, MI_U32 u32Size);
        static MI_S32 FlushBufPool(MI_VENC_CHN stVencChn);
        static MI_S32 GetDataDirect(MI_VENC_CHN stVencChn, void *pData, MI_U32 u32Maxlen);
        static void DataReceiver(void *pData, unsigned int dataSize, void *pUsrData, unsigned char portId);

        static std::map<std::string, stRtspInputInfo_t> mRtspInputInfo;
        static std::map<MI_U32, stRtspDataPackageHead_t> mapVencPackage;
        static stRtspDataMutexCond_t stDataMuxCond;

};
#endif

