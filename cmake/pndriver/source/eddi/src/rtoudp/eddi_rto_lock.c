/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_rto_lock.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_RTO_LOCK
#define LTRC_ACT_MODUL_ID  125

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_RTO_LOCK) //satisfy lint!
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

#include "eddi_dev.h"

#include "eddi_rto_lock.h"


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoCompProviderLockCurrent3B( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                    EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                    EDDI_RT_PROVIDER_BUFFER_REQ_TYPE  *  const  pBufReq )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pProvider->Udp.pDDB->RTOUDP.pIFNrtUDP;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoCompProviderLockCurrent3B->");

    if (pProvider->Locked)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderLockCurrent3B, ERROR: OnputData already locked. You must unlock first.");
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!pProvider->bBufferParamsValid)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderLockNew3B, ERROR: BufferParamsValid is not valid. Provider-Activate is not done yet");
        return EDD_STS_ERR_SEQUENCE;
    }

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    //the user gets the buffer that has been sent last
    pBufReq->pBuffer = pProvider->Udp.pUser->pRTDataBuffer;

    pProvider->pLockedUserBuffer3B = pBufReq->pBuffer; // store for consistency check during UnLock
    pProvider->Locked              = LSA_TRUE;

    if (pProvider->Udp.pNext == EDDI_NULL_PTR)
    {
        EDDI_MemCopy(pProvider->Udp.pUser->pRTDataBuffer, pProvider->Udp.pData->pRTDataBuffer, pProvider->LowerParams.DataLen);
    }
    else
    {
        EDDI_MemCopy(pProvider->Udp.pUser->pRTDataBuffer, pProvider->Udp.pNext->pRTDataBuffer, pProvider->LowerParams.DataLen);
    }

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

   LSA_UNUSED_ARG(pDDB);
   return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoCompProviderLockNew3B( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                EDDI_RT_PROVIDER_BUFFER_REQ_TYPE  *  const  pBufReq )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pProvider->Udp.pDDB->RTOUDP.pIFNrtUDP;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoCompProviderLockNew3B->");

    if (pProvider->Locked)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderLockNew3B, ERROR: OnputData already locked. You must unlock first.");
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!pProvider->bBufferParamsValid)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderLockNew3B, ERROR: BufferParamsValid is not valid. Provider-Activate is not done yet");
        return EDD_STS_ERR_SEQUENCE;
    }

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    //send buffer to user
    pBufReq->pBuffer = pProvider->Udp.pUser->pRTDataBuffer;

    pProvider->pLockedUserBuffer3B = pBufReq->pBuffer; //store for consistency check during UnLock
    pProvider->Locked              = LSA_TRUE;

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoCompProviderUnlock3B( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                               EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                               EDDI_RT_PROVIDER_BUFFER_REQ_TYPE  *  const  pBufReq )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pProvider->Udp.pDDB->RTOUDP.pIFNrtUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoCompProviderUnlock3B->");

    if (!pProvider->Locked)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderUnlock3B, ERROR: InputData not locked.");
        return EDD_STS_ERR_SEQUENCE;
    }    LSA_UNUSED_ARG(pDDB);


    if (pBufReq->pBuffer != pProvider->pLockedUserBuffer3B) //Buffer
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompProviderUnlock3B, ERROR: Wrong pBuffer.");
        return EDD_STS_ERR_PARAM;
    }

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    if (pProvider->Udp.pNext == EDDI_NULL_PTR)
    {
        pProvider->Udp.pNext = pProvider->Udp.pUser;  // U -> N
        pProvider->Udp.pUser = pProvider->Udp.pFree;  // F -> U
    }
    else
    {
        EDDI_CRT_UDP_PROV_BUF_TYPE  *  const  pTmp = pProvider->Udp.pNext;

        //next was not sent yet -> change buffer
        pProvider->Udp.pNext = pProvider->Udp.pUser;  // U -> N
        pProvider->Udp.pUser = pTmp;                  // N -> U
    }

    //update APDU status
    pProvider->Udp.pNext->pAPDU->Block = pProvider->Udp.pData->pAPDU->Block;

    pProvider->Locked = LSA_FALSE;

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoCompConsumerLock3B( EDDI_CRT_CONSUMER_PTR_TYPE           const  pConsumer,
                                                             EDDI_RT_CONSUMER_BUFFER_REQ_TYPE  *  const  pBufReq,
                                                             EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB )
{
    EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIFNrtUDP = pDDB->RTOUDP.pIFNrtUDP;
    EDDI_CRT_DATA_APDU_STATUS         *  pAPDU;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoCompConsumerLock3B->");

    if (pConsumer->Locked)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompConsumerLock3B, ERROR: InputData already locked. You must unlock first.");
        return EDD_STS_ERR_SEQUENCE;
    }

    pIFNrtUDP->Rx.LockFct(pIFNrtUDP);

    //calculate the UserBufferPtr
    if (pConsumer->Udp.pNext)
    {
        pConsumer->Udp.pFree = pConsumer->Udp.pUser;
        pConsumer->Udp.pUser = pConsumer->Udp.pNext;
        pConsumer->Udp.pNext = EDDI_NULL_PTR;
    }

    pBufReq->pBuffer = pConsumer->Udp.pUser + pConsumer->Udp.OffsetRTData;

    pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer->Udp.pUser + pConsumer->Udp.OffsetAPDU);

    pBufReq->CycleCntAct   = 0xFFFF; 
    pBufReq->CycleCntStamp = EDDI_HOST2IRTE16(pAPDU->Detail.CycleCnt);

    pConsumer->Locked = LSA_TRUE;

    pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoCompConsumerUnLock3Buf( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                                 EDDI_CRT_CONSUMER_PTR_TYPE           const  pConsumer,
                                                                 EDDI_RT_CONSUMER_BUFFER_REQ_TYPE  *  const  pBufReq )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoCompConsumerUnLock3Buf->");

    if (!pConsumer->Locked)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoCompConsumerUnLock3Buf, ERROR: InputData not locked.");

        return EDD_STS_ERR_SEQUENCE;
    }

    if (pBufReq->pBuffer != (pConsumer->Udp.pUser + pConsumer->Udp.OffsetRTData)) //Buffer
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoCompConsumerUnLock3Buf, ERROR: Wrong pBuffer. pParam->pBuffer:0x%X pConsumer->pLockedUserBuffer3B:0x%X",
                              (LSA_UINT32)pBufReq->pBuffer, (LSA_UINT32)pConsumer->Udp.pUser);

        return EDD_STS_ERR_PARAM;
    }

    pConsumer->Locked = LSA_FALSE;

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif  //EDDI_XRT_OVER_UDP_SOFTWARE


/*****************************************************************************/
/*  end of file eddi_rto_lock.c                                              */
/*****************************************************************************/
