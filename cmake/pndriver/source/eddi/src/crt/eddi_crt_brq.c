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
/*  F i l e               &F: eddi_crt_brq.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* SRT (soft real time) for EDDI.                                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_crt_ext.h"
#include "eddi_crt_check.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_crt_prov3B.h"
#endif

//#include "eddi_crt_brq.h"
#include "eddi_lock.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_lock.h"
#endif

#define EDDI_MODULE_ID     M_ID_CRT_BRQ
#define LTRC_ACT_MODUL_ID  100

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       eddi_BufferRequest_GetConsumerID()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_BufferRequest_GetConsumerID( EDDI_RT_BUFFER_REQ_GET_ID_TYPE  *  const  pBufReqGetID )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB       = (EDDI_LOCAL_DDB_PTR_TYPE)pBufReqGetID->hDDB;
    LSA_UINT32                        const  DataOffset = pBufReqGetID->DataOffset;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         pCRTComp;
    LSA_UINT32                               ctr;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_BufferRequest_GetConsumerID");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != pBufReqGetID->hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_BufferRequest_GetConsumerID", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (DataOffset == EDD_DATAOFFSET_INVALID)
    {
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ENTER_COM_S();

    pCRTComp = pDDB->pLocal_CRT;

    for (ctr = 0; ctr <= pCRTComp->ConsumerList.LastIndexRTC123; ctr++)
    {
        EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer = &pCRTComp->ConsumerList.pEntry[ctr];

        if (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_NOTUSED)
        {
            continue;
        }

        //Info: All RTC3-AUX-Consumer are using DataOffset = EDD_DATAOFFSET_UNDEFINED.

        if (DataOffset != pConsumer->LowerParams.DataOffset)
        {
            continue;
        }

        EDDI_EXIT_COM_S();

        pBufReqGetID->ConsumerProviderID = (LSA_UINT16)ctr;

        return EDD_STS_OK;
    }

    EDDI_EXIT_COM_S();

    pBufReqGetID->ConsumerProviderID = (LSA_UINT16)EDD_CONS_ID_INVALID;

    return EDD_STS_ERR_SEQUENCE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_BufferRequest_GetProviderID()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_BufferRequest_GetProviderID( EDDI_RT_BUFFER_REQ_GET_ID_TYPE  *  const  pBufReqGetID )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB       = (EDDI_LOCAL_DDB_PTR_TYPE)pBufReqGetID->hDDB;
    LSA_UINT32                        const  DataOffset = pBufReqGetID->DataOffset;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         pCRTComp;
    LSA_UINT32                               ctr;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_BufferRequest_GetProviderID");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE*)0) || (pDDB->hDDB != pBufReqGetID->hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_BufferRequest_GetProviderID", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    EDDI_ENTER_COM_S();

    pCRTComp = pDDB->pLocal_CRT;

    for (ctr = 0; ctr < pCRTComp->ProviderList.MaxEntries; ctr++)
    {
        EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider = &pCRTComp->ProviderList.pEntry[ctr];

        if (pProvider->Status == EDDI_CRT_PROV_STS_NOTUSED)
        {
            continue;
        }

        if (DataOffset != pProvider->LowerParams.DataOffset)
        {
            continue;
        }

        if (pProvider->LowerParams.ProviderType == EDDI_RTC3_AUX_PROVIDER)
        {
            continue;
        }

        EDDI_EXIT_COM_S();

        pBufReqGetID->ConsumerProviderID = (LSA_UINT16)ctr;

        return EDD_STS_OK;
    }

    EDDI_EXIT_COM_S();

    pBufReqGetID->ConsumerProviderID = (LSA_UINT16)EDD_PROV_ID_INVALID;

    return EDD_STS_ERR_SEQUENCE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_ProviderBufferRequest()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ProviderBufferRequest( const EDDI_RT_PROVIDER_BUFFER_REQ_TYPE  *  const  pBufReq )
{
    LSA_RESULT                               Status;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         pCRTComp;
    EDDI_CRT_PROVIDER_PTR_TYPE               pProvider;
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)pBufReq->hDDB;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_ProviderBufferRequest");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE*)0) || (pDDB->hDDB != pBufReq->hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_ProviderBufferRequest", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    EDDI_ENTER_COM_S();

    pCRTComp = pDDB->pLocal_CRT;

    Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, &pCRTComp->ProviderList, pBufReq->ProviderID);
    if (Status != EDD_STS_OK)
    {
        EDDI_EXIT_COM_S();
        return Status;
    }

    if (pProvider->LowerParams.ImageMode != EDDI_IMAGE_MODE_ASYNC)  /* 3 buffer interface is always asynchron */
    {
        EDDI_EXIT_COM_S();
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        switch (pBufReq->Service)
        {
            case EDDI_SRV_SRT_PROVIDER_LOCK_CURRENT:
                Status = EDDI_RtoCompProviderLockCurrent3B(pDDB, pProvider, pBufReq);
                break;
            case EDDI_SRV_SRT_PROVIDER_LOCK_NEW:
                Status = EDDI_RtoCompProviderLockNew3B(pDDB, pProvider, pBufReq);
                break;
            case EDDI_SRV_SRT_PROVIDER_UNLOCK:
                Status = EDDI_RtoCompProviderUnlock3B(pDDB, pProvider, pBufReq);
                break;

            default:
                Status = EDD_STS_ERR_SERVICE;
                break;
        }

        EDDI_EXIT_COM_S();
        return Status;
    }

    #endif
   
    Status = EDD_STS_ERR_SERVICE;

    EDDI_EXIT_COM_S();
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_brq.c                                               */
/*****************************************************************************/

