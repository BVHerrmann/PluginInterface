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
/*  F i l e               &F: eddi_rto_cons.c                           :F&  */
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

#define EDDI_MODULE_ID     M_ID_RTO_CONS
#define LTRC_ACT_MODUL_ID  124

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_RTO_CONS) //satisfy lint!
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

#include "eddi_dev.h"
#include "eddi_rto_cons.h"
#include "eddi_rto_udp.h"
#include "eddi_rto_check.h"
#include "eddi_crt_com.h"
#include "eddi_crt_check.h"
#include "eddi_nrt_inc.h"
#include "eddi_rto_sb.h"

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_RtoConsumerInit( EDDI_CRT_CONSUMER_PTR_TYPE  const pConsumer,
                                                            EDD_UPPER_RQB_PTR_TYPE      const pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                            LSA_UINT8                   const ListType );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsAddToQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                               EDDI_CRT_CONSUMER_PTR_TYPE     const  pConsumer,
                                                               EDDI_CONS_QUEUE_TYPE        *  const  pQueue );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                                    EDDI_CRT_CONSUMER_PTR_TYPE     const  pConsumer,
                                                                    EDDI_CONS_QUEUE_TYPE        *  const  pQueue );


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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsumerListAddEntry( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_DDB_COMP_RTOUDP_TYPE                    *  pRTOUDP;
    LSA_RESULT                                      Status;
    EDDI_CRT_CONSUMER_PTR_TYPE                      pConsumer;
    LSA_UINT8                                       ListType;
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE     const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_NRT_CHX_SS_IF_TYPE               *  const  pIFNrtUDP  = pDDB->RTOUDP.pIFNrtUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsumerListAddEntry->");

    ListType = EDDI_CRTGetListType(pConsParam->Properties, pConsParam->FrameID);

    Status = EDDI_RtoConsumerAddCheckRQB(pRQB, pDDB, ListType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    // get free ConsumerID
    Status = EDDI_CRTConsumerListReserveFreeEntry(pDDB, &pConsumer, pDDB->pLocal_CRT, ListType, LSA_FALSE /*bIsDFP*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerListAddEntry, ERROR finding free Entry, Status:0x%X", Status);
        return Status;
    }

    pRTOUDP = &pDDB->RTOUDP;

    pRTOUDP->cConsumer++;

    // Set new Status
    pConsumer->pSB->Status = EDDI_CRT_CONS_STS_INACTIVE;

    pIFNrtUDP->Rx.LockFct(pIFNrtUDP);

    Status = EDDI_RtoConsumerInit(pConsumer, pRQB, pDDB, ListType);
    if (Status != EDD_STS_OK)
    {
        pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);
        return Status;
    }

    if (pDDB->Glob.LocalIP.dw)
    {
        EDDI_RtoEnableRcvIF(pDDB);
    }

    // Set ConsumerID for RQB-Response
    pConsParam->ConsumerID = pConsumer->ConsumerId;

    EDDI_TreeAdd(pDDB, pDDB->RTOUDP.pTreeSB,
                 (LSA_UINT32)pConsumer->LowerParams.CycleReductionRatio,
                 (LSA_UINT32)pConsumer->LowerParams.CyclePhase,
                 (E_TREE_LIST *)(void *)pConsumer);

    EDDI_RtoConsAddToQueue(pDDB, pConsumer, &pDDB->CRT.UdpConsQueue);

    pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoConsumerInit()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsumerInit( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                               EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                               LSA_UINT8                   const  ListType )
{
    //pConsParam must not be checked
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE            pConsParam;
    LSA_UINT32                                      Ctr;
    LSA_UINT32                                      WDReloadValue;
    EDDI_NRT_CHX_SS_IF_TYPE               *  const  pIFNrtUDP = pDDB->RTOUDP.pIFNrtUDP;
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_UINT32                                      DHReloadValue;
    #endif

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsumerInit->");

    pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);

    pConsumer->LowerParams.RT_Type = EDDI_RT_TYPE_UDP;

    pConsumer->LowerParams.SrcIP.b.IP[0]      = 0xFF;
    pConsumer->LowerParams.SrcIP.b.IP[1]      = 0xFF;
    pConsumer->LowerParams.SrcIP.b.IP[2]      = 0xFF;
    pConsumer->LowerParams.SrcIP.b.IP[3]      = 0xFF;

    pConsumer->Locked                         = LSA_FALSE;
    pConsumer->pRQB_PendingRemove             = EDDI_NULL_PTR;

    pConsumer->UpperUserId                    = pConsParam->UserID;

    pConsumer->LowerParams.ListType           = ListType;

    pConsumer->LowerParams.FrameId            = pConsParam->FrameID;

    pConsumer->pFrmHandler                    = EDDI_NULL_PTR; 
    pConsumer->pLowerCtrlACW                  = EDDI_NULL_PTR;

    pConsumer->LowerParams.DataLen            = pConsParam->DataLen;
    pConsumer->LowerParams.DataOffset         = pConsParam->IOParams.DataOffset;
    pConsumer->LowerParams.Partial_DataLen    = pConsParam->Partial_DataLen;
    pConsumer->LowerParams.Partial_DataOffset = pConsParam->Partial_DataOffset;

    pConsumer->LowerParams.pKRAMDataBuffer    = EDDI_NULL_PTR;
    pConsumer->LowerParams.BufferProperties   = pConsParam->IOParams.BufferProperties;

    for (Ctr = 0; Ctr < EDD_MAC_ADDR_SIZE; Ctr++)
    {
        pConsumer->LowerParams.SrcMAC.MacAdr[Ctr] = 0;
    }

    pConsumer->LowerParams.CycleReductionRatio = pConsParam->CycleReductionRatio;   
    pConsumer->LowerParams.CyclePhase          = pConsParam->CyclePhase;

    pConsumer->LowerParams.DataHoldFactor      = pConsParam->DataHoldFactor;
                                              
    pConsumer->LowerParams.ImageMode           = EDDI_IMAGE_MODE_ASYNC;

    WDReloadValue   =  EDDI_CRTCheckGetWDDHReloadValue(EDDI_RTC1_CONSUMER, pConsParam->DataHoldFactor, pConsParam->CycleReductionRatio, &pConsumer->LowerParams.TSBEntry_RR);

    if (WDReloadValue > EDDI_MAX_RELOAD_TIME_VALUE)
    {
        EDDI_Excp("EDDI_RtoConsumerInit, DataHoldFactor > EDDI_MAX_RELOAD_TIME_VALUE",
                  EDDI_FATAL_ERR_EXCP, pConsParam->DataHoldFactor, pConsParam->CycleReductionRatio);
        return EDD_STS_ERR_EXCP;
    }

    pConsumer->LowerParams.WDReloadVal = WDReloadValue;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    {
        LSA_UINT32  NewCRR;

        DHReloadValue = EDDI_CRTCheckGetWDDHReloadValue(EDDI_RTC1_CONSUMER, pConsParam->DataHoldFactor, pConsParam->CycleReductionRatio, &NewCRR);

        if (   (DHReloadValue > EDDI_MAX_RELOAD_TIME_VALUE)
            || (NewCRR != pConsumer->LowerParams.TSBEntry_RR))
        {
            EDDI_Excp("EDDI_RtoConsumerInit, DataHoldFactor > EDDI_MAX_RELOAD_TIME_VALUE",
                      EDDI_FATAL_ERR_EXCP, pConsParam->DataHoldFactor, pConsParam->CycleReductionRatio);
            return EDD_STS_ERR_EXCP;
        }

        pConsumer->LowerParams.DHReloadVal = DHReloadValue;
    }
    #endif

    EDDI_DEVAllocMode(pDDB,
                      pIFNrtUDP->MemModeBuffer,
                      (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pConsumer->Udp.pFrameBuffer_2,
                      (LSA_UINT32)EDDI_FRAME_BUFFER_LENGTH,
                      pIFNrtUDP->UserMemIDRXDefault);

    EDDI_DEVAllocMode(pDDB,
                      pIFNrtUDP->MemModeBuffer,
                      (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pConsumer->Udp.pFrameBuffer_3,
                      (LSA_UINT32)EDDI_FRAME_BUFFER_LENGTH,
                      pIFNrtUDP->UserMemIDRXDefault);

    pConsumer->Udp.pData = EDDI_NULL_PTR;
    pConsumer->Udp.pUser = pConsumer->Udp.pFrameBuffer_2;
    pConsumer->Udp.pNext = pConsumer->Udp.pFrameBuffer_3;
    pConsumer->Udp.pFree = EDDI_NULL_PTR;

    pConsumer->Udp.OffsetRTData = EDDI_FRAME_HEADER_SIZE_WITH_VLAN + sizeof(EDDI_IP_HEADER_TYPE) + sizeof(EDDI_UDP_HEADER_TYPE) + 2;

    pConsumer->Udp.FrameLength = pConsumer->Udp.OffsetRTData + pConsumer->LowerParams.DataLen;

    pConsumer->Udp.OffsetAPDU = pConsumer->Udp.FrameLength;

    pConsumer->Udp.FrameLength += sizeof(EDDI_CRT_DATA_APDU_STATUS);

    pConsumer->Udp.Network_Ip_TotalLength = EDDI_HTONS(pConsumer->Udp.FrameLength - EDDI_FRAME_HEADER_SIZE_WITH_VLAN);

    pConsumer->Udp.StateFct = EDDI_RtoConsSINACTIVE;
    pConsumer->Udp.pDDB     = pDDB;
    pConsumer->Udp.pAPDU    = EDDI_NULL_PTR;

    pDDB->RTOUDP.pActConsumer = EDDI_NULL_PTR;

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsumerListCtrl( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                           EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                           LSA_UINT16                  const  Mode )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pDDB->RTOUDP.pIFNrtUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsumerListCtrl->");

    pIFNrtUDP->Rx.LockFct(pIFNrtUDP);

    if (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE)
    {
        pConsumer->Udp.StateFct(EDDI_SB_RTO_EV_CTRL_DEACT, pConsumer, pDDB);
        pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);
        return EDD_STS_OK;
    }

    if (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN)
    {
        pConsumer->Udp.StateFct(EDDI_SB_RTO_EV_CTRL_ACT, pConsumer, pDDB);
        pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);
        return EDD_STS_OK;
    }

    pConsumer->Udp.StateFct(EDDI_SB_RTO_EV_CTRL_ACT_UK, pConsumer, pDDB);
    pIFNrtUDP->Rx.UnLockFct(pIFNrtUDP);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoConsGetAPDUStatus()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsGetAPDUStatus( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer, 
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE   pRQBParam;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsGetAPDUStatus->");

    LSA_UNUSED_ARG(pDDB); //satisfy lint

    pRQBParam = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;

    if (pConsumer->Udp.StateFct != EDDI_RtoConsSACTRCV)
    {
        pRQBParam->Present = EDD_CSRT_APDU_STATUS_NOT_PRESENT;
        return;
    }

    pRQBParam->Present = EDD_CSRT_APDU_STATUS_PRESENT;

    pRQBParam->APDUStatus.CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

    pRQBParam->APDUStatus.DataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;

    //Patch Bit_3
    EDDI_SET_DS_BIT__Bit_3(&pRQBParam->APDUStatus.DataStatus,  0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoConsumerRemoveEvent()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsumerRemoveEvent( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pDDB->RTOUDP.pIFNrtUDP;

    pIFNrtUDP->Rx.LockFct(pIFNrtUDP);

    EDDI_TreeRemove(pDDB, (E_TREE_LIST *)(void *)pConsumer);

    pConsumer->pSB->Status = EDDI_CRT_CONS_STS_RESERVED;

    EDDI_DEVFreeMode(pDDB, pIFNrtUDP->MemModeBuffer, pConsumer->Udp.pUser, pIFNrtUDP->UserMemIDRXDefault);

    if (pConsumer->Udp.pNext)
    {
        EDDI_DEVFreeMode(pDDB, pIFNrtUDP->MemModeBuffer, pConsumer->Udp.pNext, pIFNrtUDP->UserMemIDRXDefault);
    }
    else
    {
        EDDI_DEVFreeMode(pDDB, pIFNrtUDP->MemModeBuffer, pConsumer->Udp.pFree, pIFNrtUDP->UserMemIDRXDefault);
    }

    EDDI_CRTConsumerListUnReserveEntry(pConsumer, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);

    EDDI_RtoConsRemoveFromQueue(pDDB, pConsumer, &pDDB->CRT.UdpConsQueue);

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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsAddToQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                               EDDI_CRT_CONSUMER_PTR_TYPE     const  pConsumer,
                                                               EDDI_CONS_QUEUE_TYPE        *  const  pQueue )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsAddToQueue->");

    if (pQueue->pTop == EDDI_NULL_PTR)
    {
        pQueue->pTop = pConsumer;
    }
    else
    {
        pQueue->pBottom->pNext = pConsumer;
    }

    pQueue->Cnt++;

    pQueue->pBottom = pConsumer;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoConsRemoveFromQueue()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                                    EDDI_CRT_CONSUMER_PTR_TYPE     const  pConsumer,
                                                                    EDDI_CONS_QUEUE_TYPE        *  const  pQueue )
{
    LSA_UINT32                  i, number;
    EDDI_CRT_CONSUMER_PTR_TYPE  pAct, pPrev;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsRemoveFromQueue->");

    pPrev  = EDDI_NULL_PTR;
    pAct   = pQueue->pTop;

    number = pQueue->Cnt;

    for (i = 0; i < number; i++)
    {
        if (pAct != pConsumer)
        {
            pPrev = pAct;
            pAct  = pAct->pNext;
            continue;
        }

        //remove 
        if (pPrev)
        {
            pPrev->pNext = pAct->pNext;

            if (pAct->pNext == EDDI_NULL_PTR)
            {
                pQueue->pBottom = pPrev;
            }
        }
        else
        {
            //first one -> update pFirst
            pQueue->pTop = pAct->pNext;
        }

        pQueue->Cnt--;
        return; //found

    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_Excp("EDDI_RtoConsRemoveFromQueue", EDDI_FATAL_ERR_ISR, pConsumer->LowerParams.FrameId, number);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif  //EDDI_XRT_OVER_UDP_SOFTWARE


/*****************************************************************************/
/*  end of file eddi_rto_cons.c                                              */
/*****************************************************************************/
