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
/*  F i l e               &F: eddi_sync_usr.c                           :F&  */
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
/*  14.07.07    JS    Added new Sync-services                                */
/*  18.07.07    JS    added management for EDD_SRV_SYNC_RECV                 */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_nrt_usr.h"
#include "eddi_nrt_oc.h"
#include "eddi_sync_usr.h"
//#include "eddi_lock.h"
#include "eddi_ext.h"

#define EDDI_MODULE_ID     M_ID_SYNC_USR
#define LTRC_ACT_MODUL_ID  133

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SYNCCancelRX( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  RequestID,
                                                         LSA_BOOL                 const  bCheckRequestID );

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SYNCSyncSetRate( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SYNCSyncCancel( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCCancelRX()                              */
/*                                                                         */
/* D e s c r i p t i o n: Cancel all pending EDD_SRV_SYNC_RECV within      */
/*                        Sync RxUser-Queue. Note: Must be called under    */
/*                        if-lock!                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCCancelRX( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  RequestID,
                                                          LSA_BOOL                 const  bCheckRequestID )
{
    EDDI_NRT_RX_USER_TYPE    *  pRxUser;
    EDD_RQB_TYPE             *  pRQB_recv, *  pAct, *  pPrev;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCCancelRX->");

    pRxUser = &pDDB->SYNC.RxUser; /* receive rqbs are global (only one channel can use sync) */
    pIF     = pHDB->pIF;
    pAct    = pRxUser->pFirstRQB;
    pPrev   = EDDI_NULL_PTR;

    for (;;)
    {
        pRQB_recv = pAct;
        if (pRQB_recv == EDDI_NULL_PTR)   /* kein Request mehr da ! */
        {
            break;
        }

        if (   (bCheckRequestID)
            && (((EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB_recv->pParam)->RequestID != RequestID))
        {
            /* skip RQB */
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
            /* 1st one ! -> adjust pFirstRQB */
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

    /* adapt pLastRQB */
    pRxUser->pLastRQB = pPrev;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SYNCSyncCancel()
*
* D e s c r i p t i o n: Cancels Send/Recv-Requests
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCSyncCancel( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_NRT_CANCEL_PTR_TYPE     pParam;
    LSA_RESULT                        Status;
    EDD_UPPER_NRT_CANCEL_PTR_TYPE     pRQBCancel;
    LSA_BOOL                          bCheckRequestID = LSA_FALSE;
    LSA_UINT32                        RefCnt;
    EDDI_NRT_CHX_SS_IF_TYPE        *  pIF;
    EDD_UPPER_RQB_PTR_TYPE            ptmpRQB;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCSyncCancel->");

    Status = EDD_STS_OK;

    pParam = (EDD_UPPER_NRT_CANCEL_PTR_TYPE)pRQB->pParam;

    // tbd: Check of NullPtr can be ommitted 
    // This is already checked in EDDI_NRTReq or EDDI_RESTReq

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        /* invalid parameters */
        Status = EDD_STS_ERR_PARAM;
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
    else
    {
        pRQBCancel = (EDD_UPPER_NRT_CANCEL_PTR_TYPE)pRQB->pParam;

        /* this channel must also use NRT ! */
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
                    EDDI_SYNC_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCSyncCancel-> Cancel in Progress --> EDD_STS_ERR_SEQUENCE");
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
                                EDDI_SYNC_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                    "Too few ressoruces in EDDI_SYNCSyncCancel - intTxRqbCount %08u", pHDB->intTxRqbCount);
                                EDDI_Excp("EDDI_SYNCSyncCancel has too few ressources (TX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
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
                    EDDI_SYNC_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCSyncCancel-> Cancel in Progress --> EDD_STS_ERR_SEQUENCE");
                    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SEQUENCE);
                    break;
                }

                pHDB->pRQBTxCancelPending = pRQB;
                EDDI_NRTCancelTX(pHDB, LSA_FALSE, LSA_FALSE);

                pIF->Rx.LockFct(pIF);            
                EDDI_SYNCCancelRX(pHDB, pDDB, pRQBCancel->RequestID, bCheckRequestID);            
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
                                EDDI_SYNC_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                    "Too few ressoruces in EDDI_SYNCSyncCancel - intRxRqbCount %08u", pHDB->intRxRqbCount);
                                EDDI_Excp("EDDI_SYNCSyncCancel has too few ressources (RX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
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
                                EDDI_SYNC_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                    "Too few ressoruces in EDDI_SYNCSyncCancel - intTxRqbCount %08u", pHDB->intTxRqbCount);
                                EDDI_Excp("EDDI_SYNCSyncCancel has too few ressources (TX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
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
                EDDI_SYNCCancelRX(pHDB, pDDB, pRQBCancel->RequestID, bCheckRequestID);
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
                            EDDI_SYNC_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "Too few ressoruces in EDDI_SYNCSyncCancel - intRxRqbCount %08u", pHDB->intRxRqbCount);
                            EDDI_Excp("EDDI_SYNCSyncCancel has too few ressources (RX)!", EDDI_FATAL_ERR_EXCP, 0, 0);
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
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SYNCSyncSetRate()
*
* D e s c r i p t i o n: Sets Rate for SyncID
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCSyncSetRate( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_SYNC_SET_RATE_PTR_TYPE  pParam;
    LSA_RESULT                        Status;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCSyncSetRate->");

    Status = EDD_STS_OK;

    pParam = (EDD_UPPER_SYNC_SET_RATE_PTR_TYPE)pRQB->pParam;

    // tbd: Check of NullPtr can be ommitted 
    // This is already checked in EDDI_NRTReq or EDDI_RESTReq

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        /* invalid parameters */
        Status = EDD_STS_ERR_PARAM;
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
    else
    {
        if (   (EDD_SYNC_RATE_INVALID != pParam->RateValid)
            && (EDD_SYNC_RATE_VALID != pParam->RateValid) )
        {
            /* invalid parameters */
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SYNCSyncSetRate invalid RateValid (%d)", pParam->RateValid);
            Status = EDD_STS_ERR_PARAM;
            EDDI_RequestFinish(pHDB, pRQB, Status);
        }
        else
        {
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            // Set HW-Register
            EDDI_CycSetDriftCorrection(pDDB, pParam->RateIntervalNs);
            #else
            // nothing to do for Rev5
            #endif

            //inform PortStateMachines about rate change
            EDDI_SyncPortStmsRateChange(pDDB, (EDD_SYNC_RATE_VALID == pParam->RateValid)?LSA_TRUE:LSA_FALSE);

            Status = EDD_STS_OK;
            EDDI_RequestFinish(pHDB, pRQB, Status);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCRequest()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCRequest( EDD_UPPER_RQB_PTR_TYPE   pRQB,
                                                 EDDI_LOCAL_HDB_PTR_TYPE  pHDB )
{
    LSA_RESULT                      Status    = EDD_STS_OK;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB      = pHDB->pDDB;
    LSA_BOOL                        bIndicate = LSA_TRUE;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCRequest->");

    /* NOTE: The send/recv/cancel requests uses NRT and we require a UseNRT ! */
    /*       This is checked on channel open!                                 */

    if (!(pHDB->UsedComp & EDDI_COMP_SYNC) || !(pHDB->UsedComp & EDDI_COMP_NRT))
    {
        Status = EDD_STS_ERR_CHANNEL_USE;
    }
    else
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDD_SRV_SYNC_RECV:
                bIndicate = LSA_FALSE;
                EDDI_SyncRcvRecv(pRQB, pHDB);
                break;

            case EDD_SRV_SYNC_SEND:
                bIndicate = LSA_FALSE;
                EDDI_SyncSndSend(pRQB, pDDB, pHDB);
                break;

            case EDD_SRV_SYNC_CANCEL:
                bIndicate = LSA_FALSE;
                EDDI_SYNCSyncCancel(pRQB, pDDB, pHDB);
                break;

            case EDD_SRV_SYNC_SET_RATE:
                bIndicate = LSA_FALSE;
                EDDI_SYNCSyncSetRate(pRQB, pDDB, pHDB);
                break;

            default:
                EDDI_Excp("EDDI_SYNCRequest, Service", EDDI_FATAL_ERR_EXCP, EDD_RQB_GET_SERVICE(pRQB), 0);
                return;
        }
    }

    if (bIndicate)
    {
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCRequest<-Status:0x%X", Status);
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE     pSyncComp;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE      pNRTComp;
    EDDI_NRT_CHX_SS_IF_TYPE            *  pIF;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCOpenChannel->");

    pSyncComp = pDDB->pLocal_SYNC;
    pNRTComp  = pDDB->pLocal_NRT;

    if (pSyncComp->IrtActivity)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_SEQUENCE);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (pSyncComp->SyncActivity)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_SEQUENCE);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (pSyncComp->UsrHandleCnt)
    {
        return EDD_STS_ERR_CHANNEL_USE;
    }

    /* management for EDD_SRV_SYNC_RECV */
    pSyncComp->RxUser.pFirstRQB     = LSA_NULL;
    pSyncComp->RxUser.pLastRQB      = LSA_NULL;
    pSyncComp->RxUser.pHDB          = pHDB;
    pSyncComp->RxUser.RxQueueReqCnt = 0;

    pIF = &pNRTComp->IF[EDDI_NRT_CHB_IF_0]; /* always CHB IF0! */
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].Filter          = EDD_NRT_FRAME_PTCP_SYNC;
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].FrameFilterInfo = 0;
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].pRxUser         = &pSyncComp->RxUser;

    pSyncComp->UsrHandleCnt++;     // cnt of currently open handles
    pSyncComp->pHDB = pHDB;        // currently open handle

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCCloseChannel()                          */
/*                                                                         */
/* D e s c r i p t i o n: Close Sync-Channel.                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCCloseChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE     pSyncComp;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE      pNRTComp;
    EDDI_NRT_CHX_SS_IF_TYPE            *  pIF;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SYNCCloseChannel->");

    if (    (pHDB->pRQBTxCancelPending)
        ||  (pHDB->TxCount > 0)
        ||  (pHDB->RxCount > 0)        
        )
    {
        EDDI_SYNC_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SYNCCloseChannel: pRQBTxCancelPending:0x%X, TxCount:0x%X, RxCount:0x%X", pHDB->pRQBTxCancelPending, pHDB->TxCount, pHDB->RxCount);
        return EDD_STS_ERR_SEQUENCE;
    }

    pSyncComp = pDDB->pLocal_SYNC;
    pNRTComp  = pDDB->pLocal_NRT;

    if (pSyncComp->IrtActivity)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_SEQUENCE);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (pSyncComp->SyncActivity)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_SEQUENCE);
        return EDD_STS_ERR_SEQUENCE;
    }

    pIF = &pNRTComp->IF[EDDI_NRT_CHB_IF_0]; /* always CHB IF0! */
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].Filter          = EDD_NRT_FRAME_PTCP_SYNC;
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].FrameFilterInfo = 0;
    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].pRxUser         = LSA_NULL;

    pIF->Rx.UsedFilter = pIF->Rx.UsedFilter & (~pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].Filter);

    pSyncComp->UsrHandleCnt--;        // cnt of currently open handles

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_usr.c                                              */
/*****************************************************************************/

