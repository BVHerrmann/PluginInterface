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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_nrt_snd.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDS. Transmit           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  129
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_NRT_SND */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/**
 *  \brief      EDDS_NRTTxCheckParam.
 *
 *  \details    Check RQB parameter for EDD_SRV_NRT_SEND.
 *  
 *  \note       H-Sync send priority is not checked against active FILL here because ...\n
 *              1. FILL can not be activated with enabled H-Sync support. This would be declined on startup and parameterisation.\n
 *              2. With disabled H-Sync support a H-Sync send priority will always be declined. Indpending whether FILL is active or not.
 *
 *  \param      pDDB            Pointer to device description block.
 *  \param      pRQBSnd         Pointer to request parameter block.
 *
 */
static LSA_RESULT EDDS_NRTTxCheckParam(const EDDS_DDB_TYPE * const pDDB,
                                       const EDD_RQB_NRT_SEND_TYPE * const pRQBSnd)
{
    LSA_RESULT Status = EDD_STS_OK;
    
    if (  (pRQBSnd->Length < EDD_NRT_MIN_SND_LEN ) ||
          (pRQBSnd->Length > EDD_NRT_MAX_SND_LEN ) ||
          (pRQBSnd->PortID > pDDB->pGlob->HWParams.Caps.PortCnt    ) || /* we support AUTO */
          (LSA_HOST_PTR_ARE_EQUAL( pRQBSnd->pBuffer, LSA_NULL)) )
    {
        EDDS_NRT_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                          "EDDS_NRTTxCheckParam: Invalid RQB parameter(s). Length: %d, PortID: %d, pBuffer: 0x%X",
                          pRQBSnd->Length, pRQBSnd->PortID, pRQBSnd->pBuffer);
        Status = EDD_STS_ERR_PARAM;
    }
        
    if ( (EDD_STS_OK == Status) && (EDD_NRT_SEND_PRIO_HSYNC == pRQBSnd->Priority) )
    {
        if ( !( (1 == pDDB->pGlob->HWParams.Caps.PortCnt) && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist)) )
        {
            EDDS_NRT_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, 
                              "EDDS_NRTTxCheckParam: Invalid RQB parameter. HSYNC priority now allowed without HSYNC support!");
            Status = EDD_STS_ERR_PARAM;
        }
        
        if ( (EDD_NRT_TX_GRP_HSYNC1_LOW != pRQBSnd->TxFrmGroup) && (EDD_NRT_TX_GRP_HSYNC1_HIGH != pRQBSnd->TxFrmGroup) )
        {
            EDDS_NRT_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, 
                              "EDDS_NRTTxCheckParam: Invalid RQB parameter. HSYNC priority only allowed with TX_GRP_HSYNC1_LOW and TX_GRP_HSYNC1_HIGH!");
            Status = EDD_STS_ERR_PARAM;
        }
    }

    /* check for buffer-alignment */
    #if EDDS_TX_NRT_FRAME_BUFFER_ALIGN
    if ( EDD_STS_OK == Status )
    {
        if ( ((LSA_UINT32) (pRQBSnd->pBuffer)) & EDDS_TX_NRT_FRAME_BUFFER_ALIGN )
        {
            EDDS_NRT_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                              "EDDS_NRTTxCheckParam: Invalid alignment of pBuffer address");
            Status = EDD_STS_ERR_ALIGN;
        }
    }
    #endif
    
    return Status;
}

/**
 *  \brief      EDDS_NRTTxMapPriority.
 *
 *  \details    Map given priority to one of the four internal send queues within EDDS.
 *
 *  \param      Priority          Priority that is to be mapped. Special handling for HSYNC priority is needed. 
 *                                The corresponding queue is determined according to the TX_GRP.
 *
 */
static LSA_UINT32 EDDS_NRTTxMapPriority(const EDD_RQB_NRT_SEND_TYPE * const pRQBSnd)
{
    LSA_UINT32 MappedPriority;

    /* NOTE: all parameters are validated before, see EDDS_NRTTxCheckParam */
    
    switch (pRQBSnd->Priority)
    {
        case EDD_NRT_SEND_PRIO_MGMT_LOW:         /* LLDP */
        case EDD_NRT_SEND_PRIO_MGMT_HIGH:        /* GSY, MRP */
            MappedPriority = EDDS_NRT_TX_PRIO_5; /* used for ORG */
            break;
        case EDD_NRT_SEND_PRIO_HSYNC:            /* HSYNC */
            MappedPriority = (EDD_NRT_TX_GRP_HSYNC1_LOW == pRQBSnd->TxFrmGroup) ? EDDS_NRT_TX_PRIO_3_HSYNC_LOW : EDDS_NRT_TX_PRIO_4_HSYNC_HIGH;
            break;            
        case EDD_NRT_SEND_PRIO_ASRT_HIGH:
            MappedPriority = EDDS_NRT_TX_PRIO_2; /* used for ASRT high */
            break;
        case EDD_NRT_SEND_PRIO_ASRT_LOW:
            MappedPriority = EDDS_NRT_TX_PRIO_1; /* used for ASRT low */
            break;
        default:
            MappedPriority = EDDS_NRT_TX_PRIO_0; /* lowest prio. TCP */
            break;
    }
    
    return MappedPriority;
}

/**
 *  \brief      EDDS_NRTTxInsertSrcMac.
 *
 *  \details    Insert src MAC address into send buffer. This feature is enabled on opening a channel.
 *
 *  \param      pDDB            Pointer to device description block.
 *  \param      pBuffer         Pointer to frame buffer.
 *
 */
static LSA_VOID EDDS_NRTTxInsertSrcMac(const EDDS_DEVICE_GLOB_TYPE * const pGlob,
                                       LSA_UINT8 * const pBuffer)
{
    LSA_UINT32 i;
    
    EDDS_NRT_TRACE_00(pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDS_NRTSend: Insert SRC-Mac.");
    
    for (i = 0; i < EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE) pBuffer)[6 + i] =
                pGlob->HWParams.hardwareParams.MACAddress.MacAdr[i];
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTTxQuery                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_UINT32                     Prio         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pPortID      +*/
/*+                             LSA_UINT32                     MaxLength    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Prio       : Priority to test for pendings sends (0,1,2)               +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+  pLength    : Address for LSA_UINT32 of PortID        (output)          +*/
/*+  MaxLength  : Max Framelength to provide (input)                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present and is <= MaxLength                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 17/11/2014 pDDB checked where called - other: set in function
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxQuery(
    EDDS_LOCAL_DDB_PTR_TYPE                           pDDB,
    LSA_UINT32                                        Prio,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR       *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pPortID)

{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB;
    LSA_UINT32                      TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_NRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTTxQuery(pDDB: 0x%X, Prio: %d)",
                           pDDB,
                           Prio);

    pNRTComp    = pDDB->pNRT;

    pRQB = pNRTComp->Tx[Prio].Req.pBottom;

    /*----------------------------------------------------------------- */
    /* Something present and < MaxLength? and allowed to send more?     */
    /*----------------------------------------------------------------- */

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        /* determine length of frame including ethernet frame header and tailor */
        LSA_UINT32 length = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->Length + EDDS_ETHERNET_FRAME_HEADER_TRAILER_LENGTH;

        /* check for enough Tx bandwith */
        if( ( (pNRTComp->Tx[Prio].TxByteCntCurrent + length) <= pNRTComp->Tx[Prio].TxByteCntMax ) && ((pDDB->pGlob->actTxBytesCycle + length) <= pDDB->pGlob->TxMaxByteCntPerCycle ) )
        {
            EDDS_RQB_REM_BLOCK_BOTTOM(pNRTComp->Tx[Prio].Req.pBottom,
                                      pNRTComp->Tx[Prio].Req.pTop,
                                      pRQB);

            /*------------------------------------------------------------- */
            /* Queue request in "inprogress" queue.                         */
            /*------------------------------------------------------------- */

            EDDS_RQB_PUT_BLOCK_TOP( pNRTComp->TxInProg.pBottom,
                                    pNRTComp->TxInProg.pTop,
                                    pRQB);
            /*------------------------------------------------------------- */
            /* Manipulating of framebuffer is only allowed for NRT-Send     */
            /*------------------------------------------------------------- */

            if (EDD_SRV_NRT_SEND == EDD_RQB_GET_SERVICE(pRQB))
            {
                EDDS_NRT_SEND_HOOK(pDDB->hSysDev, pRQB);
            }

            /*------------------------------------------------------------- */
            /* No parameter check here. already done when request is queued */
            /*------------------------------------------------------------- */

            *pBuffer = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->pBuffer;
            *pLength = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->Length;
            *pPortID = ((EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam)->PortID;

            if(!pDDB->pGlob->ZeroCopyInterface)
            {
                /* copy Ethernet frame from upper layer into transfer buffer */
                EDDS_MEMCOPY_UPPER(pDDB->pGlob->pTxTransferMem[pDDB->pGlob->TxFreeIdx],*pBuffer,*pLength);
                /* set *pBuffer to transfer buffer (for EDDS_LL_SEND)*/
                *pBuffer = pDDB->pGlob->pTxTransferMem[pDDB->pGlob->TxFreeIdx];
            }

            EDDS_NRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_NRTTxQuery: Prio: %d,pBuffer: %Xh, Length: %d, PortID: %d",
                                  Prio,
                                  *pBuffer,
                                  *pLength,
                                  *pPortID);

            Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB); /* get handle */

            /* we dont expect an error here because the handle has to be valid */

            if ( Status != EDD_STS_OK )
            {
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                EDDS_MODULE_ID,
                                __LINE__);
            }

            Status = EDD_STS_OK;

            pNRTComp->Tx[Prio].InProgCnt++;  /* one more transmit in progress at all     */
            pNRTComp->Tx[Prio].TxByteCntCurrent += length; /* regard ehthernet frame header and trailer */

            pHDB->pNRT->TxInProgCnt++;       /* one more transmit in progress for handle */
            pHDB->pNRT->TxQueueReqCnt--;
        }
        else
        {
            Status = EDD_STS_OK_NO_DATA;
        }
    }
    else
    {
        Status = EDD_STS_OK_NO_DATA;
    }


    EDDS_NRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTTxQuery(*pBuffer: 0x%X, Length: %d, Port: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           *pPortID,
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTTxReady                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_UINT32                     Prio         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Prio       : Priority of sended frame (0,1,2)                          +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishes the first pending tx-request in "inprogress"     +*/
/*+               queue. Unqueuing the reqeuest and calling the handles     +*/
/*+               call-back-function.                                       +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               EDD_STS_OK_NO_DATA is returned, if nothing is pending.    +*/
/*+               EDD_STS_ERR_PARAM  is returned, if handle is invalid.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 17/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
//@fixme am2219 NEA6.0 function refactoring
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxReady(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_UINT32              Prio,
    LSA_RESULT              TxStatus)

{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB;
    LSA_UINT32                      TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_NRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTTxReady(pDDB: 0x%X, Prio: %d, TxStatus: %d)",
                           pDDB,
                           Prio,
                           TxStatus);

    pNRTComp    = pDDB->pNRT;

    EDDS_RQB_REM_BLOCK_BOTTOM(pNRTComp->TxInProg.pBottom,
                              pNRTComp->TxInProg.pTop,
                              pRQB);


    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) ) /* something present ?*/
    {

        pNRTComp->Tx[Prio].InProgCnt--; /* transmit ready */

        Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);

        if ( Status != EDD_STS_OK )
        {

            EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                  "EDDS_NRTTxReady. Invalid Handle in RQB!");

            Status = EDD_STS_ERR_PARAM;  /* handle not present */
        }
        else
        {

            EDDS_NRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_NRTTxReady. pHDB: 0x%X, HDBIndex: %d, pRQB: 0x%X, TxStatus: 0x%X",
                                  pHDB,
                                  pHDB->HDBIndex,
                                  pRQB,
                                  TxStatus);

            pHDB->pNRT->TxInProgCnt--;

            #ifdef EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_SEND_ENABLE
            {
                EDDS_RQB_LIST_TYPE_PTR refFinishedGeneralRequestQueue = &pDDB->pGlob->GeneralRequestFinishedQueue;
                EDD_RQB_SET_RESPONSE(pRQB,TxStatus);
                EDDS_RQB_PUT_BLOCK_TOP(refFinishedGeneralRequestQueue->pBottom,refFinishedGeneralRequestQueue->pTop,pRQB);
                pDDB->pGlob->GeneralRequestFinishedTriggerPending = LSA_TRUE;
            }
            #else
            {
                EDDS_RequestFinish(pHDB,pRQB,TxStatus);
            }
            #endif //EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_SEND_ENABLE
            /* If cancel is in progress and this was the last */
            /* tx for this channel we continue with part 2 of */
            /* cancel-handling..                              */

            if (( pHDB->pNRT->CancelInProgress  ) &&
                ( ! pHDB->pNRT->TxInProgCnt     ) &&
                ( ! pHDB->pNRT->CancelRQB.InUse ))
            {
                pHDB->pNRT->CancelRQB.InUse = LSA_TRUE;
                EDDS_DO_NRT_CANCEL_PART2(pDDB->hSysDev, pHDB->pNRT->CancelRQB.pRQB);
            }

            Status = EDD_STS_OK;

        }

    }
    else Status = EDD_STS_OK_NO_DATA;

    EDDS_NRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTTxReady(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTSend                                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_NRT_SEND Request                          +*/
/*+               Checks for Parameters and if ok queues request in tx-queue+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 17/11/2014 pRQB and pHDB are already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTSend(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                    Status;
    EDD_UPPER_NRT_SEND_PTR_TYPE   pRQBSnd;
    LSA_UINT32                    TraceIdx;
    EDDS_LOCAL_DDB_PTR_TYPE       pDDB;

    pDDB     = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_NRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                      "IN :EDDS_NRTSend(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                      pRQB, pDDB, pHDB);

    pRQBSnd  = (EDD_UPPER_NRT_SEND_PTR_TYPE) pRQB->pParam;

    Status = EDDS_NRTTxCheckParam(pDDB, pRQBSnd);
    
    if ( EDD_STS_OK == Status )
    {
        LSA_UINT32 MappedPrio = EDDS_NRTTxMapPriority(pRQBSnd);

        if ( EDD_CDB_INSERT_SRC_MAC_ENABLED == pHDB->InsertSrcMAC )
        {
            EDDS_NRTTxInsertSrcMac(pDDB->pGlob, pRQBSnd->pBuffer);
        }
             
        EDDS_NRT_TRACE_07(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                          "EDDS_NRTSend: Send RQB queued: pHDB: 0x%X, HDBIndex: %d, pRQB: 0x%X, pBuffer: 0x%X, Len: %d, Prio: 0x%X, mapped internal Prio: 0x%X",
                          pHDB, pHDB->HDBIndex, pRQB, pRQBSnd->pBuffer, pRQBSnd->Length, pRQBSnd->Priority, MappedPrio);

        /* Because this sequence runs in RQB context and must not */
        /* be interrupted by scheduler we have to use EDDS_ENTER */

        EDDS_ENTER(pDDB->hSysDev);

        EDDS_RQB_PUT_BLOCK_TOP( pDDB->pNRT->Tx[MappedPrio].Req.pBottom,
                                pDDB->pNRT->Tx[MappedPrio].Req.pTop,
                                pRQB);

        pHDB->pNRT->TxQueueReqCnt++;

        EDDS_EXIT(pDDB->hSysDev);

        /* An EDD_SRV_NRT_SEND has been put into one of the priority queues to the scheduler.
         * call the edds_scheduler:
         * - indirectly, trigger it with EDDS_DO_NOTIFY_SCHEDULER
         * - directly, if in NRT mode and NRT_UseNotifySchedulerCall is set
         */
        if ( (pDDB->pGlob->IO_Configured) || (pDDB->pGlob->NRT_UseNotifySchedulerCall) )
        {
            EDDS_DO_NOTIFY_SCHEDULER(pDDB->hSysDev);
        }
        else
        {
            /* call edds_scheduler with "intermediate call" */
            edds_scheduler(pDDB->hDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);
        }
    }

    EDDS_NRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                      "OUT:EDDS_NRTSend(), Status: 0x%X",
                      Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



/*****************************************************************************/
/*  end of file NRT_SND.C                                                    */
/*****************************************************************************/
