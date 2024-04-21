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
/*  F i l e               &F: eddi_nrt_oc.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI.                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.07.07    JS    Added Support for SYNC-frames                          */
/*  25.07.07    JS    Added check for PTCP and LLDP uses Channel B           */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_nrt_ini.h"
#include "eddi_nrt_oc.h"
//#include "eddi_nrt_rx.h"
#include "eddi_nrt_q.h"
#include "eddi_nrt_arp.h"

#include "eddi_nrt_usr.h"

//#include "eddi_lock.h"
#include "eddi_ext.h"

#if defined (EDDI_CFG_FRAG_ON)
#include "eddi_nrt_frag_tx.h"
#endif

#define EDDI_MODULE_ID     M_ID_NRT_OC
#define LTRC_ACT_MODUL_ID  208

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_NRTCloseDCPHelloFilter( EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_NRTCancelRX( EDDI_LOCAL_HDB_PTR_TYPE const pHDB,
                                                      LSA_UINT32              const RequestID,
                                                      LSA_BOOL                const bCheckRequestID );


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_NRTOpenChannel()                       +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                        :    EDDI_LOCAL_HDB_PTR_TYPE       pHDB          +*/
/*+                             LSA_HANDLE_TYPE               Handle        +*/
/*+                             EDDI_UPPER_CDB_NRT_PTR_TYPE   pCDB          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<= EDDI_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE Filter already in use by channel  +*/
/*+               EDDI_STS_ERR_RESOURCES   Alloc failed                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for NRT. The Handle will specify the Channel+*/
/*+               and pDDB the Device. pCDB has all NRT parameters to use   +*/
/*+               for this channel (e.g. Filters)                           +*/
/*+                                                                         +*/
/*+               Each Frametype (Filter) can only be used by one channel   +*/
/*+               but a channel can register for more filters (ORed)        +*/
/*+                                                                         +*/
/*+               It is possible, that the channel don't uses any filter    +*/
/*+               (in this case no receive is possible but sending)         +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                      EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                      EDDI_UPPER_CDB_NRT_PTR_TYPE  const  pCDB )
{
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pDDB->pLocal_NRT;
    EDDI_NRT_CHX_SS_IF_TYPE               *  pIF;
    EDDI_NRT_RX_USER_TYPE                 *  pRxUser = (EDDI_NRT_RX_USER_TYPE *)0;
    LSA_UINT32                               QueueIdx;
    LSA_UINT32                               eddi_nrt_frame_x;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTOpenChannel->");

    /* ----------------------------------------------------------------------*/
    /* If NRT-component not used return with error                           */
    /* ----------------------------------------------------------------------*/

    //check if any FrameFilter is already in use by another channel

    switch (pCDB->Channel)
    {
        case EDDI_NRT_CHANEL_A_IF_0: //A0
        {
            //check FrameFilter for channel A0
            if (pCDB->FrameFilter & ~(  EDD_NRT_FRAME_LEN_DCP
                                      | EDD_NRT_FRAME_LEN_TSYNC
                                      | EDD_NRT_FRAME_LEN_OTHER
                                      | EDD_NRT_FRAME_LEN_SINEC_FWL
                                      | EDD_NRT_FRAME_DCP
                                      | EDD_NRT_FRAME_DCP_HELLO
                                      | EDD_NRT_FRAME_OTHER
                                      | EDD_NRT_FRAME_ASRT
                                      | EDD_NRT_FRAME_UDP_ASRT
                                      | EDD_NRT_FRAME_UDP_DCP
                                      | EDD_NRT_FRAME_PTCP_ANNO	
                                      | EDD_NRT_FRAME_UDP_DCP_HELLO
                                      | EDD_NRT_FRAME_ARP
                                      | EDD_NRT_FRAME_IP_ICMP
                                      | EDD_NRT_FRAME_IP_IGMP
                                      | EDD_NRT_FRAME_IP_TCP
                                      | EDD_NRT_FRAME_IP_UDP
                                      | EDD_NRT_FRAME_IP_VRRP
                                      | EDD_NRT_FRAME_IP_OTHER
                                      | EDD_NRT_FRAME_RARP))
            {
                //FrameFilter not allowed
                EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTOpenChannel, invalid FrameFilter 0x%X for NRT channel A0", pCDB->FrameFilter);
                return EDD_STS_ERR_CHANNEL_USE;
            }

            QueueIdx = EDDI_NRT_CHA_IF_0;

            break;
        }
        case EDDI_NRT_CHANEL_B_IF_0: //B0
        {
            //PTCP and LLDP need EDDI_NRT_CHANEL_B_IF_0, because MC-MAC is assigned to channel B.
            //We check this to early detect configuration errors (because PTCP and LLDP can not be received on a wrong channel/interface).

            //check FrameFilter for channel B0
            if (pCDB->FrameFilter & ~(  EDD_NRT_FRAME_LEN_HSR
                                      | EDD_NRT_FRAME_LEN_STDBY
                                      | EDD_NRT_FRAME_LLDP
                                      | EDD_NRT_FRAME_PTCP_SYNC
                                      | EDD_NRT_FRAME_PTCP_DELAY
                                      | EDD_NRT_FRAME_MRP
                                      | ((EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole) ? EDD_NRT_FRAME_HSYNC : 0UL)))
            {
                //FrameFilter not allowed
                EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTOpenChannel, invalid FrameFilter 0x%X for NRT channel B0", pCDB->FrameFilter);
                return EDD_STS_ERR_CHANNEL_USE;
            }

            QueueIdx = EDDI_NRT_CHB_IF_0;

            break;
        }
        default:
        {
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTOpenChannel, invalid Channel:0x%X", pCDB->Channel);
            return EDD_STS_ERR_CHANNEL_USE;
        }
    }

    pIF = &pNRTComp->IF[QueueIdx];

    pIF->NRTChannel = pCDB->Channel;

    switch (pIF->StateDeferredAlloc)
    {
        case DEF_ALLOC_DONT_CARE:
        case DEF_ALLOC_OPEN:
            break;

        case DEF_ALLOC_CLOSE:
        {
            LSA_UINT32         Ctr;
            NRT_DSCR_PTR_TYPE  pReqDscr = pIF->Rx.Dscr.pTop;

            //belated allocation of the receive buffer
            for (Ctr = 0; Ctr < pIF->Rx.Dscr.Cnt; Ctr++)
            {
                EDDI_NRTAllocDataBuffer(pDDB, pIF, pReqDscr, pIF->UserMemIDRXDefault);
                pReqDscr = pReqDscr->next_dscr_ptr;
            }

            pIF->StateDeferredAlloc = DEF_ALLOC_OPEN;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_NRTOpenChannel, default pIF->StateDeferredAlloc", EDDI_FATAL_ERR_EXCP, pIF->StateDeferredAlloc, 0);
            return EDD_STS_ERR_CHANNEL_USE;
        }
    }

    if (pIF->Rx.UsedFilter & pCDB->FrameFilter)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_FILTER_NOT_FREE);
        return EDD_STS_ERR_CHANNEL_USE;
    }

    pIF->Rx.UsedFilter = pIF->Rx.UsedFilter | pCDB->FrameFilter;

    for (;;)
    {
        LSA_UINT32  Index;

        if (pCDB->FrameFilter == 0) //no reception
        {
            break;
        }

        if (pCDB->FrameFilter & (  EDD_NRT_FRAME_UDP_ASRT
                                 | EDD_NRT_FRAME_UDP_DCP_HELLO
                                 | EDD_NRT_FRAME_UDP_DCP))
        {
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTOpenChannel, FrameFilter 0x%X not implemented", pCDB->FrameFilter);
            return EDD_STS_ERR_NOT_IMPL;
        }

        eddi_nrt_frame_x = 1;
        for (Index = EDDI_NRT_FILTER_INDEX_ARP; Index <= EDDI_NRT_FILTER_INDEX_USER_LAST; Index++)
        {
            if (pCDB->FrameFilter & eddi_nrt_frame_x)
            {
                if (pRxUser == (EDDI_NRT_RX_USER_TYPE *)0)
                {
                    pRxUser = &pIF->Rx.User[Index];
                }
                pIF->Rx.Filter[Index].pRxUser = pRxUser;
                pIF->Rx.Filter[Index].Filter  = eddi_nrt_frame_x;
            }

            eddi_nrt_frame_x = eddi_nrt_frame_x << 1;
        }

        break;
    }

    if (pRxUser)
    {
        pRxUser->pFirstRQB     = (EDD_RQB_TYPE *)0;
        pRxUser->pLastRQB      = (EDD_RQB_TYPE *)0;
        pRxUser->pHDB          = pHDB;
        pRxUser->RxQueueReqCnt = 0;
    }

    pHDB->pIF         = pIF;
    pHDB->pRxUser     = pRxUser;
    pHDB->FrameFilter = pCDB->FrameFilter;

    pHDB->TxCount = 0;
    pHDB->RxCount = 0;
    pHDB->intTxRqbCount = 0;
    pHDB->intRxRqbCount = 0;
    pHDB->pRQBTxCancelPending = LSA_NULL;

    pNRTComp->HandleCnt++;  /* cnt of currently open handles */

    pIF->CntOpenChannel++;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_NRTCloseChannel()                      +*/
/*+  Input/Output          :    ...                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel to be closed                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE         Channel has pending requests +*/
/*+               EDD_STS_ERR_RESOURCE         Error on freeing memory      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes the Channel. If there are any pending requests     +*/
/*+               the channel can not be closed. If NRT is not used return  +*/
/*+               with ok.                                                  +*/
/*+                                                                         +*/
/*+               Note:  If EDD_STS_ERR_RESOURCE is returned the channel is +*/
/*+                      closed but we had an error while freeing memory.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCloseChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE     const  pNRTComp = pDDB->pLocal_NRT;
    EDDI_NRT_CHX_SS_IF_TYPE           *  const  pIF      = pHDB->pIF;
    LSA_UINT32                                  Ctr;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTCloseChannel->");

    if (pIF == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTCloseChannel, pIF == EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, pIF, 0);
        return EDD_STS_ERR_CHANNEL_USE;
    }

    if (   (pHDB->pRQBTxCancelPending)
        || (0 < pHDB->TxCount)
        || (0 < pHDB->RxCount)
        || (0 < pHDB->intTxRqbCount)
        || (0 < pHDB->intRxRqbCount)
       )
    {
        EDDI_SYNC_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTCloseChannel: pRQBTxCancelPending:0x%X, TxCount:0x%X, RxCount:0x%X!", pHDB->pRQBTxCancelPending, pHDB->TxCount, pHDB->RxCount);
        return EDD_STS_ERR_SEQUENCE;
    }

    //deactivate DCP-Filter if it was activated by this channel at last
    if (   (pNRTComp->DCPFilterTotalCount)
        && (pNRTComp->DCPFilterHandle == pHDB->Handle))
    {
        EDDI_NRTSetDCPFilterOff(pDDB);
    }

    EDDI_NRTCloseDCPHelloFilter(pHDB);

    /* ----------------------------------------------------------------------*/
    /* If all is ok we free all memory and release the filters               */
    /* ----------------------------------------------------------------------*/
    for (Ctr = 0; Ctr < EDDI_NRT_FILTER_INDEX_LAST; Ctr++)
    {
        if (pIF->Rx.Filter[Ctr].pRxUser != pHDB->pRxUser)
        {
            continue;
        }

        pIF->Rx.Filter[Ctr].pRxUser = (EDDI_NRT_RX_USER_TYPE *)0;
        pIF->Rx.UsedFilter          = pIF->Rx.UsedFilter & (~pIF->Rx.Filter[Ctr].Filter);
    }

    #if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
    //deactivate all ARP filters associated with this LSA channel
    {
        NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter = &pNRTComp->ARPFilter;

        if (pARPFilter->FilterCnt)
        {
            LSA_UINT8                       i;
            EDD_RQB_NRT_SET_ARP_FILTER_TYPE RqbParamLocal;

            for (i=0; i<EDDI_MAX_ARP_FILTER; i++)
            {
                if (pHDB->Handle == pARPFilter->Handle[i])
                {
                    RqbParamLocal.UserIndex = i;
                    EDDI_NRTSetARPFilterONOFF(&RqbParamLocal, pHDB, LSA_FALSE /*bON*/);
                }
            }
        }
    }
    #endif

    //handle is centrally released
    pNRTComp->HandleCnt--;

    pIF->CntOpenChannel--;

    switch (pIF->StateDeferredAlloc)
    {
        case DEF_ALLOC_DONT_CARE:
            return EDD_STS_OK;

        case DEF_ALLOC_OPEN:
        {
            NRT_DSCR_PTR_TYPE  pReqDscr;

            if (pIF->CntOpenChannel)
            {
                return EDD_STS_OK;
            }

            pReqDscr = pIF->Rx.Dscr.pTop;

            //deallocate receive-buffers
            for (Ctr = 0; Ctr < pIF->Rx.Dscr.Cnt; Ctr++)
            {
                EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, pReqDscr->pRcvDataBuffer, pIF->UserMemIDRXDefault);
                pReqDscr = pReqDscr->next_dscr_ptr;
            }

            pIF->StateDeferredAlloc = DEF_ALLOC_CLOSE;
            return EDD_STS_OK;
        }

        case DEF_ALLOC_CLOSE:
        default:
        {
            EDDI_Excp("EDDI_NRTCloseChannel, default pIF->StateDeferredAlloc", EDDI_FATAL_ERR_EXCP, pIF->StateDeferredAlloc, 0);
            return EDD_STS_ERR_CHANNEL_USE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCancel()                                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCancel( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDD_UPPER_NRT_CANCEL_PTR_TYPE     pRQBCancel;
    LSA_BOOL                          bCheckRequestID = LSA_FALSE;
    LSA_RESULT                        Status;
    EDDI_LOCAL_HDB_PTR_TYPE           pHDB;
    LSA_UINT32                        RefCnt;
    EDDI_NRT_CHX_SS_IF_TYPE        *  pIF;
    EDD_UPPER_RQB_PTR_TYPE            ptmpRQB;

    pRQBCancel = (EDD_UPPER_NRT_CANCEL_PTR_TYPE)pRQB->pParam;

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
   
    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancel->");

    if (!(pHDB->UsedComp & EDDI_COMP_NRT))
    {
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);
        return;
    }

    RefCnt = pHDB->RefCnt;
    pIF    = pHDB->pIF;

    pIF->Tx.LockFct(pHDB->pIF);

    if (RefCnt != pHDB->RefCnt)
    {
        pIF->Tx.UnLockFct(pIF);
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    switch (pRQBCancel->Mode)
    {
        case EDD_NRT_CANCEL_MODE_TX_ALL:
        {
            if (pHDB->pRQBTxCancelPending)
            {
                pIF->Tx.UnLockFct(pIF);
                EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancel-> pRQBTxCancelPending --> EDD_STS_ERR_SEQUENCE");
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SEQUENCE);
                break;
            }

            pHDB->pRQBTxCancelPending = pRQB;

            EDDI_NRTCancelTX(pHDB, LSA_FALSE, LSA_FALSE);

            if(     (0 == pHDB->TxCount)
                &&  (0 == pHDB->intTxRqbCount) )
            {
                //no rqb needs to be finished
                pHDB->pRQBTxCancelPending = LSA_NULL;
                pIF->Tx.UnLockFct(pIF);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                break;
            }
            else
            {
                //handle internal tx rqbs
                while(LSA_TRUE)  //endless loop: locked/unlocked parts must not be optimized by the compiler
                {
                    if(0 != pHDB->intTxRqbCount)
                    {
                        //only the internal counters need to be locked
                        pIF->Tx.UnLockFct(pIF);
                        ptmpRQB = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->intTxRqbQueue);

                        if (EDDI_NULL_PTR == ptmpRQB)
                        {
                            EDDI_NRT_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "Too few ressoruces in EDDI_NRTCancel - intTxRqbCount %08u", pHDB->intTxRqbCount);
                            EDDI_Excp("EDDI_NRTCancel has too few ressources (TX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
                            return;
                        }
                        //get and finish current RQB
                        EDDI_RequestFinish(pHDB, ptmpRQB, EDD_STS_OK_CANCEL);
                        pIF->Tx.LockFct(pIF);
                        --pHDB->intTxRqbCount;
                    }
                    else
                    {
                        break;
                    }
                }

                //check if there are rqbs waiting for TX_DONE (TxCount)
                if(0 != pHDB->TxCount)
                {
                    pIF->Tx.UnLockFct(pIF);
                }
                else
                {
                    if(LSA_NULL == pHDB->pRQBTxCancelPending)  //could be set to null in ISR (during handling of internal rbq queues)
                    {
                        //ok. RQB does not need to be finished ==> finished in ISR
                        pIF->Tx.UnLockFct(pIF);
                    }
                    else
                    {                        
                        //finish RQB
                        pHDB->pRQBTxCancelPending = LSA_NULL;
                        pIF->Tx.UnLockFct(pIF);
                        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                    }
                }
            }
            break;
        }

        case EDD_NRT_CANCEL_MODE_ALL:
        {
            if (pHDB->pRQBTxCancelPending)
            {
                pIF->Tx.UnLockFct(pIF);
                EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancel-> Cancel in Progress --> EDD_STS_ERR_SEQUENCE");
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SEQUENCE);
                break;
            }

            pHDB->pRQBTxCancelPending = pRQB;
            EDDI_NRTCancelTX(pHDB, LSA_FALSE, LSA_FALSE);

            pIF->Rx.LockFct(pIF);            
            EDDI_NRTCancelRX(pHDB, pRQBCancel->RequestID, bCheckRequestID);            
            pIF->Rx.UnLockFct(pIF);

            if(     (0 == pHDB->TxCount)
                &&  (0 == pHDB->intRxRqbCount)
                &&  (0 == pHDB->intTxRqbCount) )
            {
                //no rqb needs to be finished
                pHDB->pRQBTxCancelPending = LSA_NULL;
                pIF->Tx.UnLockFct(pIF);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                break;
            }
            else
            {
                //handle internal rx rqbs
                while(LSA_TRUE)  //endless loop: locked/unlocked parts must not be optimized by the compiler
                {
                    if(0 != pHDB->intRxRqbCount)
                    {                
                        //only the internal counters need to be locked
                        pIF->Tx.UnLockFct(pIF);
                        ptmpRQB = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->intRxRqbQueue);

                        if (EDDI_NULL_PTR == ptmpRQB)
                        {                            
                            EDDI_NRT_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "Too few ressoruces in EDDI_NRTCancel - intRxRqbCount %08u", pHDB->intRxRqbCount);
                            EDDI_Excp("EDDI_NRTCancel has too few ressources (RX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
                            return;
                        }
                        //get and finish current RQB
                        EDDI_RequestFinish(pHDB, ptmpRQB, EDD_STS_OK_CANCEL);
                        pIF->Tx.LockFct(pIF); //"re"-lock 
                        --pHDB->intRxRqbCount;                       
                    }
                    else
                    {
                        break;
                    }
                }
                //Tx lock applied after loop

                //handle internal tx rqbs
                while(LSA_TRUE)  //endless loop: locked/unlocked parts must not be optimized by the compiler
                {
                    if(0 != pHDB->intTxRqbCount)
                    {
                        //only the internal counters need to be locked
                        pIF->Tx.UnLockFct(pIF);
                        ptmpRQB = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->intTxRqbQueue);

                        if (EDDI_NULL_PTR == ptmpRQB)
                        {                            
                            EDDI_NRT_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "Too few ressoruces in EDDI_NRTCancel - intTxRqbCount %08u", pHDB->intTxRqbCount);
                            EDDI_Excp("EDDI_NRTCancel has too few ressources (TX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
                            return;
                        }
                        //get and finish current RQB
                        EDDI_RequestFinish(pHDB, ptmpRQB, EDD_STS_OK_CANCEL);
                        pIF->Tx.LockFct(pIF); //"re"-lock
                        --pHDB->intTxRqbCount;
                    }
                    else
                    {
                        break;
                    }
                }
                //Tx lock applied after loop

                //check if there are rqbs waiting for TX_DONE (TxCount)
                if(0 != pHDB->TxCount)
                {
                    pIF->Tx.UnLockFct(pIF);
                }
                else
                {
                    if(LSA_NULL == pHDB->pRQBTxCancelPending)  //could be set to null in ISR (during handling of internal rbq queues)
                    {
                        //ok. RQB does not need to be finished ==> finished in ISR
                        pIF->Tx.UnLockFct(pIF);
                    }
                    else
                    {
                        //finish RQB
                        pHDB->pRQBTxCancelPending = LSA_NULL;
                        pIF->Tx.UnLockFct(pIF);
                        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                    }
                }
            }
            break;
        }
        
        case EDD_NRT_CANCEL_MODE_RX_BY_ID:
        {
            bCheckRequestID = LSA_TRUE;
        }
        //lint -fallthrough
        case EDD_NRT_CANCEL_MODE_RX_ALL:    
        {
            pIF->Rx.LockFct(pIF);
            EDDI_NRTCancelRX(pHDB, pRQBCancel->RequestID, bCheckRequestID);
            pIF->Rx.UnLockFct(pIF);

            //handle internal rx rqbs
            while(LSA_TRUE)  //endless loop: locked/unlocked parts must not be optimized by the compiler
            {
                if(0 != pHDB->intRxRqbCount)
                {                
                    //only the internal counters need to be locked
                    pIF->Tx.UnLockFct(pIF);
                    ptmpRQB = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->intRxRqbQueue);

                    if (EDDI_NULL_PTR == ptmpRQB)
                    {                            
                        EDDI_NRT_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "Too few ressoruces in EDDI_NRTCancel - intRxRqbCount %08u", pHDB->intRxRqbCount);
                        EDDI_Excp("EDDI_NRTCancel has too few ressources (RX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }
                    //get and finish current RQB
                    EDDI_RequestFinish(pHDB, ptmpRQB, EDD_STS_OK_CANCEL);
                    pIF->Tx.LockFct(pIF); //"re"-lock 
                    --pHDB->intRxRqbCount;                       
                }
                else
                {
                    break;
                }
            }
            //Tx lock applied after loop
            pIF->Tx.UnLockFct(pIF);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
        }
            break;

        default:
        {
            pIF->Tx.UnLockFct(pIF);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);
            break;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCancelRX()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCancelRX( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                         LSA_UINT32               const  RequestID,
                                                         LSA_BOOL                 const  bCheckRequestID )
{
    EDDI_NRT_RX_USER_TYPE    *  pRxUser;
    EDD_RQB_TYPE             *  pRQB_recv, *  pAct, *  pPrev;
    EDDI_LOCAL_DDB_PTR_TYPE     pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;

    EDDI_NRT_TRACE_02(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelRX-> RequestID: 0x%X, bCheckRequestID: 0x%X", RequestID, bCheckRequestID);

    pRxUser = pHDB->pRxUser;
    pDDB    = pHDB->pDDB;
    pIF     = pHDB->pIF;

    /* case Frame Filter == 0 */
    if (pRxUser == (EDDI_NRT_RX_USER_TYPE *)0)
    {
        EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelRX-> No Framefilter, pRxUser==0");

        return;
    }

    pAct  = pRxUser->pFirstRQB;
    pPrev = EDDI_NULL_PTR;

    for (;;)
    {
        pRQB_recv = pAct;
        if (pRQB_recv == EDDI_NULL_PTR)   /* no request left to handle ! */
        {
            EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelRX-> no request left to handle");
            break;
        }

        if (   (bCheckRequestID)
            && (((EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB_recv->pParam)->RequestID != RequestID))
        {
            EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelRX-> leave out the next RQB");
            /* leave out the next RQB */
            pPrev  = pAct;
            pAct   = (EDD_RQB_TYPE *)EDD_RQB_GET_NEXT_RQB_PTR(pAct);
            continue;
        }

        /* dequeue RQB */
        if (pPrev)
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pPrev, EDD_RQB_GET_NEXT_RQB_PTR(pAct));
        }
        else
        {
            /* we got the 1st one -> update pFirstRQB */
            pRxUser->pFirstRQB = (EDD_RQB_TYPE *)EDD_RQB_GET_NEXT_RQB_PTR(pRQB_recv);
        }

        pAct = (EDD_RQB_TYPE *)EDD_RQB_GET_NEXT_RQB_PTR(pAct);

        //move rqb to internal queue (do not call CBF under lock!)
        EDDI_AddToQueueEnd(pDDB, &pHDB->intRxRqbQueue, pRQB_recv);
        ++pHDB->intRxRqbCount;

        pRxUser->RxQueueReqCnt--;
        ((EDD_UPPER_NRT_RECV_PTR_TYPE)(pRQB_recv->pParam))->RequestCnt = pRxUser->RxQueueReqCnt;
        
        pHDB->RxCount--;
        pIF->Rx.Queued_RxCount--;
    }

    /* update pLastRQB */
    pRxUser->pLastRQB = pPrev;

    if (pIF->Rx.Queued_RxCount == 0)
    {
        //disable NRT channel
        IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_DISABLE;
    }

    EDDI_NRT_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelRX-> pHDB->RxCount: 0x%X", pHDB->RxCount);

}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCancelTX()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCancelTX( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                 LSA_BOOL                 const  bClosing,
                                                 LSA_BOOL                 const  bSync )
{
    LSA_UINT32                         Ctr;
    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB = pHDB->pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF  = pHDB->pIF;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCancelTX->");

    //EDDI SendQueues
    for (Ctr = 0; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++)
    {
        EDDI_RQB_QUEUE_TYPE  *  const  pSndQueue = &pIF->Tx.SndReq[Ctr];

        for (;;)
        {
            EDD_UPPER_RQB_PTR_TYPE  const  pRQB = EDDI_NRTCheckAndRemoveFromQueue(pDDB, pSndQueue, pHDB, bSync);

            if //no further desired RQB available?
               (pRQB == EDDI_NULL_PTR)
            {
                break; //leave inner for-loop
            }

            #if defined (EDDI_CFG_FRAG_ON)
            {
                LSA_UINT32  const  internal_context_1 = pRQB->internal_context_1;

                if //1st fragment waiting (=> no fragment of this service is currently in DMACW-ring)?
                   ((LSA_UINT8)internal_context_1 == 0) //FragIndexNext (aggressive optimization!)
                {
                    LSA_UINT32                 const  PrioIndex         = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!
                    EDD_UPPER_RQB_PTR_TYPE     const  pHelpRQB          = (EDD_UPPER_RQB_PTR_TYPE)(pRQB->internal_context_Prio); //here fragmentation buffer is handled as RQB
                    EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;

                    //store PrioIndex for later deallocation
                    pHelpRQB->internal_context_Prio = PrioIndex;

                    //put big fragmentation buffer to Frag Dealloc Queue
                    EDDI_AddToQueueEnd(pDDB, pFragDeallocQueue, pHelpRQB);

                    //indicate RQB
                }
                else if //2nd...last fragment waiting (=> at least 1 fragment of this service is/was already in DMACW-ring)?
                        (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
                {
                    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB                       = &pDDB->NRT.TxFragData;
                    LSA_UINT32                    const  FragCntTotal                            = (LSA_UINT32)((LSA_UINT8)(internal_context_1>>8)); //FragCntTotal
                    LSA_UINT32                    const  FragmentDmacwInCnt                      = (LSA_UINT32)((LSA_UINT8)internal_context_1); //FragIndexNext
                    LSA_UINT32                    const  FragmentDmacwOutCnt                     = FragCntTotal - pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt;
                    LSA_UINT32                    const  RemainingFragmentDmacwExitCntAtStopping = FragmentDmacwInCnt - FragmentDmacwOutCnt;

                    //TX semaphore is always locked here

                    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
                    //plausible internal fragment parameters
                    if (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt > FragCntTotal)
                    {
                        EDDI_Excp("EDDI_NRTCancelTX, internal fragment parameter error!", EDDI_FATAL_ERR_EXCP, pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt, FragCntTotal);
                        return;
                    }
                    if (RemainingFragmentDmacwExitCntAtStopping > FragCntTotal)
                    {
                        EDDI_Excp("EDDI_NRTCancelTX, internal fragment parameter error!", EDDI_FATAL_ERR_EXCP, RemainingFragmentDmacwExitCntAtStopping, FragCntTotal);
                        return;
                    }
                    if (FragmentDmacwOutCnt > FragmentDmacwInCnt)
                    {
                        EDDI_Excp("EDDI_NRTCancelTX, internal fragment parameter error!", EDDI_FATAL_ERR_EXCP, FragmentDmacwOutCnt, FragmentDmacwInCnt);
                        return;
                    }
                    #endif

                    //update FragmentFrameIdLow
                    pNrtTxFragDataDDB->FragmentFrameIdLow = (pNrtTxFragDataDDB->FragmentFrameIdLow + (LSA_UINT8)1) & (LSA_UINT8)0x8F; //increment low nibble

                    if //no fragment of this service is currently in DMACW-ring?
                       (RemainingFragmentDmacwExitCntAtStopping == 0)
                    {
                        LSA_UINT32                        NextFragInterfaceNr;
                        LSA_UINT32                 const  PrioIndex         = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!
                        EDD_UPPER_RQB_PTR_TYPE     const  pHelpRQB          = (EDD_UPPER_RQB_PTR_TYPE)(pRQB->internal_context_Prio); //here fragmentation buffer is handled as RQB
                        EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;

                        //store PrioIndex for later deallocation
                        pHelpRQB->internal_context_Prio = PrioIndex;

                        //put big fragmentation buffer to Frag Dealloc Queue
                        EDDI_AddToQueueEnd(pDDB, pFragDeallocQueue, pHelpRQB);

                        if //releasing TX semaphore not successful?
                           (!EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RELEASE, pIF->QueueIdx, &NextFragInterfaceNr))
                        {
                            EDDI_Excp("EDDI_NRTCancelTX, releasing TX Frag semaphore not successful!", EDDI_FATAL_ERR_EXCP, 0, 0);
                            return;
                        }

                        //indicate RQB
                    }
                    else //at least 1 fragment of this service is currently in DMACW-ring
                    {
                        //set marker for stopping fragment-service -> further processing in TxDone-interrupt
                        pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping = RemainingFragmentDmacwExitCntAtStopping;
                        continue;
                    }
                }
                //else: no FRAG RQB: indicate RQB
            }
            #endif

            //move rqb to internal queue (do not call CBF under lock!)
            EDDI_AddToQueueEnd(pDDB, &pHDB->intTxRqbQueue, pRQB);
            ++pHDB->intTxRqbCount;

            pHDB->TxCount--;

        } //end of for-loop
    } //end of for-loop

    #if defined (EDDI_CFG_FRAG_ON)
    //EDDI PreQueues
    //check all PreQueues of this interface with the exception of MGMT_HIGH_PRIO
    for (Ctr = 1UL; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++) //0 = highest prio
    {
        EDDI_RQB_QUEUE_TYPE  *  const  pPreQueue = &pIF->Tx.PreSndReq[Ctr];

        for (;;)
        {
            EDD_UPPER_RQB_PTR_TYPE  const  pRQB = EDDI_NRTCheckAndRemoveFromQueue(pDDB, pPreQueue, pHDB, bSync);

            if //no further desired RQB available?
               (pRQB == EDDI_NULL_PTR)
            {
                break; //leave inner for-loop
            }

            //move rqb to internal queue (do not call CBF under lock!)
            EDDI_AddToQueueEnd(pDDB, &pHDB->intTxRqbQueue, pRQB);
            ++pHDB->intTxRqbCount;

            pHDB->TxCount--;
        }
    }
    #endif

    #if defined (EDDI_CFG_REV5)
    //TimeStampRQBs
    {
        EDDI_RQB_QUEUE_TYPE  *  const  pTimeStampSndQueue = &pDDB->NRT.TimeStamp.ReqQueue;
        EDD_UPPER_RQB_PTR_TYPE  const  pTimeStampRQB      = pDDB->NRT.TimeStamp.pActReq;

        if ((pTimeStampRQB != EDDI_NULL_PTR) && bClosing)
        {
            if (pTimeStampRQB->internal_context == (void *)pHDB)
            {
                //check if desired service
                if (   (  bSync  && (EDD_RQB_GET_SERVICE(pTimeStampRQB) == EDD_SRV_SYNC_SEND))
                    || ((!bSync) && (EDD_RQB_GET_SERVICE(pTimeStampRQB) != EDD_SRV_SYNC_SEND)))
                {
                    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_OC_C, EDDI_TS_CALLER_NRT_CANCEL_TX, pDDB->NRT.TimeStamp.State);

                    if (pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT)
                    {
                        //move rqb to internal queue (do not call CBF under lock!)
                        EDDI_AddToQueueEnd(pDDB, &pHDB->intTxRqbQueue, pTimeStampRQB);
                        ++pHDB->intTxRqbCount;

                        pHDB->TxCount--;
                    }

                    pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_CLOSING;

                    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_OC_C, EDDI_TS_CALLER_NRT_CANCEL_TX, pDDB->NRT.TimeStamp.State);
                }
            }
        }

        for (;;)
        {
            EDD_UPPER_RQB_PTR_TYPE  const  pRQB = EDDI_NRTCheckAndRemoveFromQueue(pDDB, pTimeStampSndQueue, pHDB, bSync);

            if //no further desired RQB available?
               (pRQB == EDDI_NULL_PTR)
            {
                break; //leave for-loop
            }

            //move rqb to internal queue (do not call CBF under lock!)
            EDDI_AddToQueueEnd(pDDB, &pHDB->intTxRqbQueue, pRQB);
            ++pHDB->intTxRqbCount;

            pHDB->TxCount--;
        }
    }
    #else
    LSA_UNUSED_ARG(bClosing); //satisfy lint!
    #endif

    if (pHDB->TxCount < 0)
    {
        EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCloseDCPHelloFilter()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCloseDCPHelloFilter( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_UINT32                               Ctr;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pHDB->pDDB->pLocal_NRT;

    EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCloseDCPHelloFilter->");

    for (Ctr = 0; Ctr < EDD_DCP_MAX_DCP_HELLO_FILTER; Ctr++)
    {
        NRT_DCP_HELLO_FILTER_TYPE  *  const  pFilter = &pNRTComp->DCPHelloFilter[Ctr];

        if ((pFilter->bInUse) && (pFilter->pHDB == pHDB))
        {
            pFilter->bInUse = LSA_FALSE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_oc.c                                                */
/*****************************************************************************/



