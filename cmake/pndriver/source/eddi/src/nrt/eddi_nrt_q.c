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
/*  F i l e               &F: eddi_nrt_q.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI: Transmit           */
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
#include "eddi_dev.h"
#include "eddi_nrt_q.h"

#if defined (EDDI_CFG_FRAG_ON)
#include "eddi_nrt_tx.h"
#include "eddi_nrt_frag_tx.h"
#endif

#define EDDI_MODULE_ID     M_ID_NRT_Q
#define LTRC_ACT_MODUL_ID  206

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_FRAG_ON)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues( EDDI_DDB_TYPE  *  const  pDDB );
#if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues1If( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );
#else
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues1If( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );
#endif
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtGetFromPrioQueueWithLimit()              */
/*                                                                         */
/* D e s c r i p t i o n: get RQB from SendQueue                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          RQB-PTR                                          */
/*                                                                         */
/***************************************************************************/
//Const arrays for lookup to avoid ()?: during runtime.
static LSA_UINT8 const HSYNCQueueSwapper[NRT_HSYNC2_LOW_PRIO_INDEX+1] = {0, NRT_HSYNC2_HIGH_PRIO_INDEX, NRT_HSYNC1_HIGH_PRIO_INDEX, 0, 0, 0, NRT_HSYNC2_LOW_PRIO_INDEX, NRT_HSYNC1_LOW_PRIO_INDEX};
static LSA_UINT8 const HSYNCFillQueueArr[NRT_HSYNC2_LOW_PRIO_INDEX+1] = {0, NRT_HSYNC1_HIGH_PRIO_INDEX, NRT_HSYNC2_HIGH_PRIO_INDEX, 0, 0, 0, NRT_HSYNC1_HIGH_PRIO_INDEX, NRT_HSYNC2_HIGH_PRIO_INDEX};

EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NrtGetFromPrioQueueWithLimit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                                EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    LSA_UINT32                         PrioQueueArrayIndex; 
    LSA_UINT32                         TmpBytesPerPrio, TmpAlloverBytes, TmpAlloverBytesLMH;
    EDDI_NRT_SND_LIMIT_TYPE  *  const  pSndLimit                            = &pIF->Tx.SndLimit;
    #if defined (EDDI_CFG_FRAG_ON)
    LSA_BOOL                           bFragFoundWithoutSemaphoreClaimingOk = LSA_FALSE;
    #endif

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtGetFromPrioQueueWithLimit->");

    //only check the queues relevant for this IF. PrioQueueArrayStartIndex will be set ...
    //... to 0 if SLL is reset (EDDI_NRTReloadSendList).
    //... to current index if a RQB is found (to check this queue again on next call of EDDI_NrtGetFromPrioQueueWithLimit).
    //... to a lower index if at EDDI_NrtAddToPrioQueue a frame with a higher prio than pIF->Tx.PrioQueueArrayIndex[pIF->Tx.PrioQueueArrayStartIndex] is to be sent.
    for (PrioQueueArrayIndex = pIF->Tx.PrioQueueArrayStartIndex; PrioQueueArrayIndex <= pIF->Tx.MaxPrioQueueArrayIndex; PrioQueueArrayIndex++) 
    {
        EDDI_RQB_QUEUE_TYPE     *  pQueue;
        EDD_UPPER_RQB_PTR_TYPE     pRQB;
        LSA_BOOL                   bHSYNCQueue          = LSA_FALSE;
        LSA_BOOL                   bHSYNCQueueNoSwap    = LSA_FALSE;
        LSA_UINT8            const PrioQueueIndex       = pIF->Tx.PrioQueueArrayIndex[PrioQueueArrayIndex];
        LSA_UINT8                  CurrentHSYNCPrioQueue = 0;

        switch (PrioQueueIndex)
        {
            //special queue handling for HSYNC queues. The index array contains the PRIO_INDEX values for H1 and L1 only, as H1/H2 and L1/L2 shall be treated equaly.
            //This is done by toggling the next prio to be used if a HSYNC frame was found in a queue and returned for sending
            case NRT_HSYNC1_HIGH_PRIO_INDEX:
            {
                CurrentHSYNCPrioQueue   = pIF->Tx.NextHSYNCPrioQueueHigh;
                pQueue                  = &pIF->Tx.SndReq[CurrentHSYNCPrioQueue];
                bHSYNCQueue = LSA_TRUE;
                break;
            }
            case NRT_HSYNC1_LOW_PRIO_INDEX :
            {
                CurrentHSYNCPrioQueue   = pIF->Tx.NextHSYNCPrioQueueLow;
                pQueue                  = &pIF->Tx.SndReq[CurrentHSYNCPrioQueue];
                bHSYNCQueue = LSA_TRUE;
                break;
            }
            default:
            {
                pQueue = &pIF->Tx.SndReq[PrioQueueIndex];
            }
        }
        pRQB = pQueue->pTop;

        if //no RQB available in SendQueue, but HSYNC queue?
           ((EDDI_NULL_PTR == pRQB) && (bHSYNCQueue))
        {
            //check other HSYNC queue with same priority
            CurrentHSYNCPrioQueue   = HSYNCQueueSwapper[CurrentHSYNCPrioQueue];
            pQueue                  = &pIF->Tx.SndReq[CurrentHSYNCPrioQueue];
            pRQB                    = pQueue->pTop;
            bHSYNCQueueNoSwap = LSA_TRUE;  
        }
        
        if //RQB available in SendQueue?
                (pRQB != EDDI_NULL_PTR)
        {
            EDD_UPPER_NRT_SEND_PTR_TYPE pRQBSnd     = (EDD_UPPER_NRT_SEND_PTR_TYPE) pRQB->pParam;
            LSA_BOOL                    bRqbFound   = LSA_FALSE;

            if (bHSYNCQueue)
            {
                LSA_UINT32 HSYNCFillQueue = HSYNCFillQueueArr[CurrentHSYNCPrioQueue]; //HSYNC LOW and HSYNC HIGH share 1 FILL ctr

                //check NRT-Send-Limitation per HSync-Prio-Queue-Couple
                TmpBytesPerPrio = pSndLimit->ActCnt.BytesPerPrio[HSYNCFillQueue] + pRQBSnd->Length + EDDI_NRT_FRAME_HEADER_SIZE;

                if (TmpBytesPerPrio > pSndLimit->Prio_x_BytesPerMs[HSYNCFillQueue])
                {
                    //HSYNC FILL limit for this port reached. Check the same HSYNC prio for the other port.
                    if (bHSYNCQueueNoSwap)
                    {
                        //a) The other port has already been checked and its queue is empty ==> continue
                        continue;
                    }
                    else
                    {
                        //check other HSYNC queue with same priority
                        CurrentHSYNCPrioQueue   = HSYNCQueueSwapper[CurrentHSYNCPrioQueue];
                        pQueue                  = &pIF->Tx.SndReq[CurrentHSYNCPrioQueue];
                        pRQB                    = pQueue->pTop;
                        bHSYNCQueueNoSwap = LSA_TRUE;  

                        if //RQB available in SendQueue?
                           (pRQB != EDDI_NULL_PTR)
                        {
                            pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE) pRQB->pParam;
                            HSYNCFillQueue = HSYNCFillQueueArr[CurrentHSYNCPrioQueue]; //HSYNC LOW and HSYNC HIGH share 1 FILL ctr

                            //check NRT-Send-Limitation per HSync-Prio-Queue-Couple
                            TmpBytesPerPrio = pSndLimit->ActCnt.BytesPerPrio[HSYNCFillQueue] + pRQBSnd->Length + EDDI_NRT_FRAME_HEADER_SIZE;
                            if (TmpBytesPerPrio > pSndLimit->Prio_x_BytesPerMs[HSYNCFillQueue])
                            {
                            //b) HSYNC FILL limit for this port also reached. ==> continue
                            continue; 
                            }
                        }
                        else
                        {
                            //c) This queue is also empty. ==> continue
                            continue; 
                        }
                    }
                }
                //update SLL
                pSndLimit->ActCnt.BytesPerPrio[HSYNCFillQueue] = TmpBytesPerPrio;
                bRqbFound = LSA_TRUE;

                #if defined (EDDI_CFG_TRACE_HSA)
                {
                    EDDI_NRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "!!!HSA EDDI_NrtGetFromPrioQueueWithLimit, P:%d, G:%d, Queue(6:1L, 1:1H, 7:2L, 2:2H):%d, Lenght:0x%X, SeqID:0x%X", 
                        pRQBSnd->PortID, pRQBSnd->TxFrmGroup, CurrentHSYNCPrioQueue, pRQBSnd->Length, pRQBSnd->TxTime);
                }
                #endif      

                //swap P1/P1 only for next round if RQB has been found in a normal way (empty queue or FILL limit on P(n) will NOT lead to swapping to P(n+1)!).
                if (!bHSYNCQueueNoSwap)
                {
                    if (NRT_HSYNC1_HIGH_PRIO_INDEX == PrioQueueIndex) //HSYNC high?
                    {
                        //swap portqueue prio
                        pIF->Tx.NextHSYNCPrioQueueHigh = HSYNCQueueSwapper[CurrentHSYNCPrioQueue];
                    }
                    else
                    {
                        //swap portqueue prio
                        pIF->Tx.NextHSYNCPrioQueueLow = HSYNCQueueSwapper[CurrentHSYNCPrioQueue];
                    }
                }
            }
            else
            {
            LSA_UINT32                          FramelengthInBytes;

                #if defined (EDDI_CFG_FRAG_ON)
                EDDI_NRT_TX_FRAG_SEND_DATA_TYPE  *  const  pNrtTxFragSendData    = &(pIF->Tx.SndReqFragData[PrioQueueIndex]);
                LSA_UINT8                        *  const  pTmpNRTPrivFragBuffer = (LSA_UINT8 *)(LSA_VOID *)&(pNrtTxFragSendData->TmpNRTPrivFragBuffer);
                LSA_UINT32                          const  internal_context_1    = pRQB->internal_context_1;

                if //FRAG RQB?
                    (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
                {
                    if //SendData of the current fragment already stored?
                        (internal_context_1 & EDDI_TX_FRAG_SEND_DATA_STORED)
                    {
                        //restore FRAG data from SendData
                        FramelengthInBytes = (LSA_UINT32)(pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN]);
                    }
                    else //SendData of the current fragment not stored
                    {
                        LSA_UINT8  const  FragBufNettoSize = (LSA_UINT8)(internal_context_1>>16);
                        LSA_UINT8      *  pNRTFragBuffer;
                        LSA_UINT8      *  pNRTFragBufferPriv;

                        //calculate pointer to fragment buffer of current fragment
                        pNRTFragBuffer = (LSA_UINT8 *)(  (LSA_UINT32)(pRQB->internal_context_Prio) //start of large fragment buffer
                                                        + (LSA_UINT32)((LSA_UINT8)internal_context_1/*FragIndexNext*/ * (FragBufNettoSize + TX_FRAG_BUF_PRIV_SIZE)));

                        //calculate pointer to private area of current fragment
                        pNRTFragBufferPriv = pNRTFragBuffer + FragBufNettoSize;

                        //read all private data from target FragBuffer to temporary buffer in SendData
                        pNrtTxFragSendData->TmpNRTPrivFragBuffer = *((LSA_UINT32 *)(LSA_VOID *)pNRTFragBufferPriv);

                        FramelengthInBytes = (LSA_UINT32)(pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN]);

                        //store FRAG data in SendData
                        pNrtTxFragSendData->pNRTFragBuffer   = pNRTFragBuffer;

                        //mark RQB
                        pRQB->internal_context_1 = internal_context_1 | EDDI_TX_FRAG_SEND_DATA_STORED;
                    }
                }
                else //no FRAG RQB
                {
                    FramelengthInBytes = pRQBSnd->Length;
                }
                #elif !defined (EDDI_CFG_FRAG_ON)
                    FramelengthInBytes = pRQBSnd->Length;
                #endif

                //check NRT-Send-Limitation per Prio-Queue
                TmpBytesPerPrio = pSndLimit->ActCnt.BytesPerPrio[PrioQueueIndex] + FramelengthInBytes + EDDI_NRT_FRAME_HEADER_SIZE;
                if (TmpBytesPerPrio > pSndLimit->Prio_x_BytesPerMs[PrioQueueIndex])
                {
                    continue;
                }

                //check global NRT-Send-limitation for NRT_LOW/_MEDIUM/_HIGH
                //Limits the usage below HOL_Limit_Port_Up
                switch (PrioQueueIndex)
                {
                    case NRT_LOW_PRIO_INDEX:
                    case NRT_MEDIUM_PRIO_INDEX:
                    case NRT_HIGH_PRIO_INDEX:
                    {
                        LSA_UINT32 const FramelengthInBytesLMH = ((FramelengthInBytes+(EDDI_NRT_DB_LEN-1)) / EDDI_NRT_DB_LEN) * EDDI_NRT_DB_LEN;
                        TmpAlloverBytesLMH = pSndLimit->ActCnt.BytesPerCycleLMH + FramelengthInBytesLMH;
                        if (TmpAlloverBytesLMH > pSndLimit->BytesPerCycleLMH)
                        {
                            continue;
                        }
                        break;
                    }
                    default:
                    {
                        TmpAlloverBytesLMH = pSndLimit->ActCnt.BytesPerCycleLMH;
                        break;
                    }
                }

                //Attention: Check of BytesPerCycle (Overall) has to be the last FILL check!     
                //check global NRT-Send-limitation
                TmpAlloverBytes = pSndLimit->ActCnt.BytesPerCycle + FramelengthInBytes + EDDI_NRT_FRAME_HEADER_SIZE;
                if (TmpAlloverBytes > pSndLimit->BytesPerCycle)
                {
                    continue;
                }

                #if defined (EDDI_CFG_FRAG_ON)
                {
                    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;

                    if //1st fragment waiting?
                        ((LSA_UINT8)internal_context_1 == 0) //FragIndexNext (aggressive optimization!)
                    {
                        if (bFragFoundWithoutSemaphoreClaimingOk)
                        {
                            //wait till TX semaphore is free again to avoid overtakings between fragment services
                            continue;
                        }

                        pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex;

                        if //claiming TX semaphore successful?
                            (EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_CLAIM, pIF->QueueIdx, 0))
                        {
                            //TX semaphore is always locked here

                            //update SLL
                            pSndLimit->ActCnt.BytesPerCycle                = TmpAlloverBytes;
                            pSndLimit->ActCnt.BytesPerPrio[PrioQueueIndex] = TmpBytesPerPrio;
                            pSndLimit->ActCnt.BytesPerCycleLMH             = TmpAlloverBytesLMH;

                            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
                            if //fragment parameters initialized wrong?
                                (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping)
                            {
                                EDDI_Excp("EDDI_NrtGetFromPrioQueueWithLimit, internal fragment parameter error!", EDDI_FATAL_ERR_EXCP, pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping, 0);
                                return EDDI_NULL_PTR;
                            }
                            #endif

                            //store FRAG DDB parameters
                            pNrtTxFragDataDDB->pSndBufferCurrent             = pNrtTxFragSendData->pNRTFragBuffer;
                            pNrtTxFragDataDDB->SndLengthCurrent              = (LSA_UINT32)(pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN]);
                            pNrtTxFragDataDDB->PDUFragmentFrameIdLowOffset   = (LSA_UINT32)(pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_FRAMEID_LOW_OFFSET]);
                            pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt = (LSA_UINT32)((LSA_UINT8)(internal_context_1>>8)) /*FragCntTotal*/;
                            pNrtTxFragDataDDB->bLastFragmentDmacwEntry       = LSA_FALSE;
                            //reset FRAG DDB parameters
                            pNrtTxFragDataDDB->StoredTxStatus                = EDD_STS_OK;

                            //FragIndexNext++;
                            pRQB->internal_context_1 = (internal_context_1 + 1UL) & (~EDDI_TX_FRAG_SEND_DATA_STORED);

                            return pRQB;
                        }
                        else //claiming TX semaphore not successful
                        {
                            //bFragFoundWithoutSemaphoreClaimingOk is already plausibled above!
                            bFragFoundWithoutSemaphoreClaimingOk = LSA_TRUE;

                            //wait till TX semaphore is free again
                            continue;
                        }
                    }
                    else if //2nd...last fragment waiting?
                            (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
                    {
                        //TX semaphore is always locked here

                        //store FRAG DDB parameters
                        pNrtTxFragDataDDB->pSndBufferCurrent = pNrtTxFragSendData->pNRTFragBuffer;
                        pNrtTxFragDataDDB->SndLengthCurrent  = (LSA_UINT32)(pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN]);

                        if //not last fragment?
                            (((LSA_UINT8)internal_context_1 /*FragIndexNext*/ + (LSA_UINT8)1) < (LSA_UINT8)(internal_context_1>>8) /*FragCntTotal*/)
                        {
                            //update SLL
                            pSndLimit->ActCnt.BytesPerCycle                = TmpAlloverBytes;
                            pSndLimit->ActCnt.BytesPerPrio[PrioQueueIndex] = TmpBytesPerPrio;
                            pSndLimit->ActCnt.BytesPerCycleLMH             = TmpAlloverBytesLMH;

                            //FragIndexNext++;
                            pRQB->internal_context_1 = (internal_context_1 + 1UL) & (~EDDI_TX_FRAG_SEND_DATA_STORED);

                            if (!bFragFoundWithoutSemaphoreClaimingOk)
                            {
                                pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex;
                            }

                            return pRQB;
                        }
                        else //last fragment
                        {
                            //set FRAG DDB parameter
                            pNrtTxFragDataDDB->bLastFragmentDmacwEntry = LSA_TRUE;
                        }
                    }
                }
                #endif //EDDI_CFG_FRAG_ON

                //update SLL
                pSndLimit->ActCnt.BytesPerCycle                = TmpAlloverBytes;
                pSndLimit->ActCnt.BytesPerPrio[PrioQueueIndex] = TmpBytesPerPrio;
                pSndLimit->ActCnt.BytesPerCycleLMH             = TmpAlloverBytesLMH;

                bRqbFound = LSA_TRUE;
            }

            if (bRqbFound)
            {
                //dequeue RQB from SendQueue
                pQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);
                pQueue->Cnt--;
                pIF->Tx.QueuedSndReq--;

                #if defined (EDDI_CFG_FRAG_ON)
                if (!bFragFoundWithoutSemaphoreClaimingOk)
                #endif
                {
                    pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex;
                }

                return pRQB;
            }
        }
    } //end of for-loop

    #if defined (EDDI_CFG_FRAG_ON)
    if (!bFragFoundWithoutSemaphoreClaimingOk)
    #endif
    {
        pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex; //= end of array
    }

    LSA_UNUSED_ARG(pDDB);
    return EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtFragQueuesTimerCBF()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_FRAG_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtFragQueuesTimerCBF( LSA_VOID  *  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    EDDI_TIMER_TYPE              *  pTimer;
    EDDI_SHED_OBJ_TYPE           *  pShedObj;

    if (pDDB->NRT.NRTPreQueuesTimerID  >= EDDI_CFG_MAX_TIMER)
    {
        EDDI_Excp("EDDI_NrtFragQueuesTimerCBF, pDDB->NRT.NRTPreQueuesTimerID >= EDDI_CFG_MAX_TIMER", EDDI_FATAL_ERR_EXCP, pDDB->NRT.NRTPreQueuesTimerID, 0);
        return;
    }

    pTimer = &g_pEDDI_Info ->Timer[pDDB->NRT.NRTPreQueuesTimerID];
    
    pShedObj = &pTimer->ShedObj;

    if (   (0 == pTimer->pDDB->Glob.OpenCount) //do not send rqb anymore if all channels are closed
        || (0 == pTimer->used)                 //Race Condition
        || (pShedObj->bUsed)
        || (!pTimer->pDDB->Glob.HWIsSetup))
    {
        return;
    }

    pShedObj->bUsed = LSA_TRUE;

    EDDI_NrtCheckFragQueues(pDDB);

    pTimer                = &g_pEDDI_Info ->Timer[pDDB->NRT.NRTPreQueuesTimerID];
    pTimer->ShedObj.bUsed = LSA_FALSE;
}
#endif //EDDI_CFG_FRAG_ON
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCheckFragQueues()                        */
/*                                                                         */
/* D e s c r i p t i o n: Check Frag DeallocQueue and Frag PreQueue of     */
/*                        all NRT interfaces                               */
/*                        - triggered by cyclic timer                      */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_FRAG_ON)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues( EDDI_DDB_TYPE  *  const  pDDB )
{
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCheckFragQueues->");

    (void)EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_CLEAN, 0, 0);

    {             
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[EDDI_NRT_CHA_IF_0];
        pIF->Tx.LockFct(pIF);
        if //fragmentation cleanup active?
           (pDDB->NRT.TxFragData.bFragCleanupActive)
        {
            pIF->Tx.UnLockFct(pIF);
            return;
        }
        else //fragmentation cleanup not active
        {
            #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
            if //RQB inserted into SendQueue?
               (EDDI_NrtCheckFragQueues1If(pDDB, pIF))
            {
                EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
            }
            #else
            EDDI_NrtCheckFragQueues1If(pDDB, pIF);
            #endif
        }
        pIF->Tx.UnLockFct(pIF);
    }
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[EDDI_NRT_CHB_IF_0];
        pIF->Tx.LockFct(pIF);
        if //fragmentation cleanup active?
           (pDDB->NRT.TxFragData.bFragCleanupActive)
        {
            pIF->Tx.UnLockFct(pIF);
            return;
        }
        else //fragmentation cleanup not active
        {
            #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
            if //RQB inserted into SendQueue?
               (EDDI_NrtCheckFragQueues1If(pDDB, pIF))
            {
                EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
            }
            #else
            EDDI_NrtCheckFragQueues1If(pDDB, pIF);
            #endif
        }
        pIF->Tx.UnLockFct(pIF);
    }
}
#endif //EDDI_CFG_FRAG_ON
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCheckFragQueues1If()                     */
/*                                                                         */
/* D e s c r i p t i o n: check Frag DeallocQueue and Frag PreQueue of one */
/*                        NRT interface                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL: LSA_TRUE = RQB inserted into SendQueue */
/*                        LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_FRAG_ON)
#if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues1If( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
#else
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckFragQueues1If( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
#endif
{
    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1)
    LSA_UINT32                        PrioIndex         = 0xFFFFFFFFUL;
    #endif
    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
    LSA_BOOL                          bRetVal           = LSA_FALSE; //no RQB inserted into SendQueue
    #endif
    EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;
    EDD_UPPER_RQB_PTR_TYPE            pHelpRQB; //here big fragmentation buffers are handled as RQBs

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCheckFragQueues1If->");

    //check old big fragmentation buffers in Frag Dealloc Queue of this interface
    while ((pHelpRQB = EDDI_RemoveFromQueue(pDDB, pFragDeallocQueue)) != EDDI_NULL_PTR)
    {
        #if defined (EDDI_CFG_FRAG_RELOAD_MODE1)
        PrioIndex = pHelpRQB->internal_context_Prio;
        #else
        LSA_UINT32  const  PrioIndex = pHelpRQB->internal_context_Prio;
        #endif
        //deallocate big fragmentation buffer
        EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)pHelpRQB, pIF->UserMemIDTXFragArray[PrioIndex]);
    }

    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1)
    if //any fragmentation buffer has just been deallocated?
       (PrioIndex != 0xFFFFFFFFUL)
    #endif
    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
    {
        if //any RQB available in the PreQueues of this interface?
           (pIF->Tx.QueuedPreSndReq)
        {
            LSA_UINT32         Ctr;
            LSA_UINT32  const  SendLengthMaxNoFrag = pDDB->NRT.TxFragData.SendLengthMaxNoFrag;

            //check all PreQueues of this interface with the exception of MGMT_HIGH_PRIO, HSYNC1_HIGH, HSYNC2_HIGH
            for (Ctr = NRT_MGMT_LOW_PRIO_INDEX; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++) //0 = highest prio
            {
                EDDI_RQB_QUEUE_TYPE             *  const  pPreQueue         = &pIF->Tx.PreSndReq[Ctr];
                EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  *  const  pNrtTxFragPreData = &pIF->Tx.PreSndReqFragData[Ctr];
                EDDI_USERMEMID_TYPE                const  UserMemID         = pIF->UserMemIDTXFragArray[Ctr];

                //try to reload PreQueue
                for (;;)
                {
                    EDD_UPPER_RQB_PTR_TYPE  const  pPreRQB = pPreQueue->pTop;

                    if //further RQB available in PreQueue?
                       (pPreRQB != EDDI_NULL_PTR)
                    {
                        EDD_UPPER_NRT_SEND_PTR_TYPE  const  pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pPreRQB->pParam;

                        if //FRAG RQB (with pre-calculated fragments)?
                           (pPreRQB->internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
                        {
                            //try to allocate the big fragmentation buffer
                            EDDI_DEVAllocModeTxFrag(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pNrtTxFragPreData->pDstNRTFragBuffer, pNrtTxFragPreData->BigFragBufTotalSize, UserMemID);

                            if //big fragmentation buffer available?
                               (!LSA_HOST_PTR_ARE_EQUAL(pNrtTxFragPreData->pDstNRTFragBuffer, LSA_NULL))
                            {
                                EDDI_NRTBuildFragments(pDDB, pPreRQB, pRQBSnd, pNrtTxFragPreData);

                                //insert FRAG-RQB to bottom of the SendQueue
                            }
                            else //big fragmentation buffer not available
                            {
                                break; //leave for-loop
                            }
                        }
                        else //no FRAG RQB (with pre-calculated fragments)
                        {
                            LSA_UINT32  const  SendLength = pRQBSnd->Length; //length without FCS

                            if //frame must be fragmented?
                               (SendLength > SendLengthMaxNoFrag)
                            {
                                //calculate fragments
                                EDDI_NRTCalcFragments(pDDB, pPreRQB, pRQBSnd, SendLength, pNrtTxFragPreData);

                                //try to allocate the big fragmentation buffer
                                EDDI_DEVAllocModeTxFrag(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pNrtTxFragPreData->pDstNRTFragBuffer, pNrtTxFragPreData->BigFragBufTotalSize, UserMemID);

                                if //big fragmentation buffer available?
                                   (!LSA_HOST_PTR_ARE_EQUAL(pNrtTxFragPreData->pDstNRTFragBuffer, LSA_NULL))
                                {
                                    EDDI_NRTBuildFragments(pDDB, pPreRQB, pRQBSnd, pNrtTxFragPreData);

                                    //insert FRAG-RQB to bottom of the SendQueue
                                }
                                else //big fragmentation buffer not available
                                {
                                    break; //leave for-loop
                                }
                            }
                            //else: frame must not be fragmented => insert RQB to bottom of the SendQueue
                        }

                        //dequeue RQB from PreQueue
                        pPreQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pPreRQB);
                        #if defined (EDDI_TX_FRAG_DEBUG_MODE)
                        pPreQueue->Cnt--;
                        #endif
                        pIF->Tx.QueuedPreSndReq--;

                        //insert RQB to bottom of the SendQueue
                        {
                            EDDI_RQB_QUEUE_TYPE  *  const  pQueue = &pIF->Tx.SndReq[Ctr];
                            LSA_UINT8                      PrioQueueArrayIndex;

                            EDD_RQB_SET_NEXT_RQB_PTR(pPreRQB, EDDI_NULL_PTR);

                            if //Queue empty? (= RQB not available in Queue)
                               (pQueue->pTop == EDDI_NULL_PTR)
                            {
                                pQueue->pTop = pPreRQB;

                                pQueue->Cnt = 1UL;

                                //"Ctr" refers to NRT Prio queues, lookup PrioQueueArray index first
                                PrioQueueArrayIndex = pIF->Tx.RevPrioQueueArrayIndex[Ctr];
                                if (PrioQueueArrayIndex < pIF->Tx.PrioQueueArrayStartIndex)
                                {
                                    pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex;
                                }
                                else if (PrioQueueArrayIndex > pIF->Tx.MaxPrioQueueArrayIndex)
                                {
                                    EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NrtCheckFragQueues1If, invalid PrioQueueArrayIndex, PrioQueueArrayIndex:0x%X MaxPrioQueueArrayIndex:0x%X", PrioQueueArrayIndex, pIF->Tx.MaxPrioQueueArrayIndex);
                                    EDDI_Excp("EDDI_NrtCheckFragQueues1If, invalid PrioQueueArrayIndex", EDDI_FATAL_ERR_EXCP, PrioQueueArrayIndex, pIF->Tx.MaxPrioQueueArrayIndex);
                                }
                            }
                            else //Queue not empty (= RQB available in Queue)
                            {
                                EDD_RQB_SET_NEXT_RQB_PTR(pQueue->pBottom, pPreRQB);

                                pQueue->Cnt++;
                            }

                            pQueue->pBottom = pPreRQB;

                            pIF->Tx.QueuedSndReq++;
                        }

                        bRetVal = LSA_TRUE; //RQB inserted into SendQueue
                    }
                    else //no further RQB available in PreQueue
                    {
                        break; //leave for-loop
                    }
                } //end of for-loop
            } //end of for-loop
        }
    }

    return bRetVal;
    #endif //EDDI_CFG_FRAG_RELOAD_MODE1 || EDDI_CFG_FRAG_RELOAD_MODE2
}
#endif //EDDI_CFG_FRAG_ON
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtAddToPrioQueue()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL: LSA_TRUE = RQB inserted into SendQueue */
/*                        LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_FRAG_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NrtAddToPrioQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                       EDD_UPPER_RQB_PTR_TYPE      const  pRQB )
#else
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtAddToPrioQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                       EDD_UPPER_RQB_PTR_TYPE      const  pRQB )
#endif
{
    LSA_UINT32              const  PrioIndex = pRQB->internal_context_1>>24; //EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED cannot be set here!
    #if defined (EDDI_CFG_FRAG_ON)
    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
    LSA_BOOL                       bRetVal;
    #endif
    EDDI_RQB_QUEUE_TYPE  *  const  pPreQueue = &pIF->Tx.PreSndReq[PrioIndex];
    #endif

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtAddToPrioQueue->");

    #if defined (EDDI_CFG_FRAG_ON)
    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    if //fragmentation cleanup active?
       (pDDB->NRT.TxFragData.bFragCleanupActive)
    {
        EDDI_Excp("EDDI_NrtAddToPrioQueue, fragmentation cleanup active!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return LSA_FALSE;
    }
    #endif
    #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
    bRetVal = EDDI_NrtCheckFragQueues1If(pDDB, pIF);
    #else
    EDDI_NrtCheckFragQueues1If(pDDB, pIF);
    #endif

    if //PreQueue not empty? (= RQB available in PreQueue)
       (pPreQueue->pTop != EDDI_NULL_PTR)
    {
        //insert new RQB to bottom of the non-empty PreQueue
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

            EDD_RQB_SET_NEXT_RQB_PTR(pPreQueue->pBottom, pRQB);

            pPreQueue->pBottom = pRQB;

            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            pPreQueue->Cnt++;
            #endif

            pIF->Tx.QueuedPreSndReq++;
        }

        #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
        return bRetVal;
        #else
        return LSA_FALSE; //no RQB inserted into SendQueue
        #endif
    }
    else //here PreQueue always empty
    {
        EDD_UPPER_NRT_SEND_PTR_TYPE  const  pRQBSnd    = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;
        LSA_UINT32                   const  SendLength = pRQBSnd->Length; //length without FCS

        if //frame must be fragmented?
           (SendLength > pDDB->NRT.TxFragData.SendLengthMaxNoFrag)
        {
            EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  *  const  pNrtTxFragPreData = &pIF->Tx.PreSndReqFragData[PrioIndex];

            if (PrioIndex == NRT_MGMT_HIGH_PRIO_INDEX)
            {
                EDDI_Excp("EDDI_NrtAddToPrioQueue, Error with NRT TX fragment PrioIndex MGMT_HIGH!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return LSA_FALSE; //no RQB inserted into SendQueue
            }

            //calculate fragments
            EDDI_NRTCalcFragments(pDDB, pRQB, pRQBSnd, SendLength, pNrtTxFragPreData);

            //try to allocate the big fragmentation buffer
            EDDI_DEVAllocModeTxFrag(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pNrtTxFragPreData->pDstNRTFragBuffer, pNrtTxFragPreData->BigFragBufTotalSize, pIF->UserMemIDTXFragArray[PrioIndex]);

            if //big fragmentation buffer available?
               (!LSA_HOST_PTR_ARE_EQUAL(pNrtTxFragPreData->pDstNRTFragBuffer, LSA_NULL))
            {
                EDDI_NRTBuildFragments(pDDB, pRQB, pRQBSnd, pNrtTxFragPreData);

                //insert FRAG-RQB to bottom of the SendQueue
            }
            else //big fragmentation buffer not available
            {
                //insert FRAG-RQB to bottom of the empty PreQueue (FragData are also stored in NRT Frag pre data)

                EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

                pPreQueue->pTop = pRQB;

                pPreQueue->pBottom = pRQB;

                #if defined (EDDI_TX_FRAG_DEBUG_MODE)
                pPreQueue->Cnt = 1UL;
                #endif

                pIF->Tx.QueuedPreSndReq++;

                #if defined (EDDI_CFG_FRAG_RELOAD_MODE1) || defined (EDDI_CFG_FRAG_RELOAD_MODE2)
                return bRetVal;
                #else
                return LSA_FALSE; //no RQB inserted into SendQueue
                #endif
            }
        }
        //else: frame must not be fragmented => insert RQB to bottom of the SendQueue
    }
    #endif //EDDI_CFG_FRAG_ON

    //insert RQB to bottom of the SendQueue
    {
        EDDI_RQB_QUEUE_TYPE  *  const  pQueue = &pIF->Tx.SndReq[PrioIndex];
        LSA_UINT8                      PrioQueueArrayIndex;

        EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

        if //Queue empty? (= RQB not available in Queue)
           (pQueue->pTop == EDDI_NULL_PTR)
        {
            pQueue->pTop = pRQB;

            pQueue->Cnt = 1UL;

            //"PrioIndex" refers to NRT Prio queues, lookup PrioQueueArray index first
            PrioQueueArrayIndex = pIF->Tx.RevPrioQueueArrayIndex[PrioIndex];
            if (PrioQueueArrayIndex < pIF->Tx.PrioQueueArrayStartIndex)
            {
                pIF->Tx.PrioQueueArrayStartIndex = PrioQueueArrayIndex;
            }
            else if (PrioQueueArrayIndex > pIF->Tx.MaxPrioQueueArrayIndex)
            {
                EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NrtAddToPrioQueue, invalid PrioQueueArrayIndex, PrioQueueArrayIndex:0x%X MaxPrioQueueArrayIndex:0x%X", PrioQueueArrayIndex, pIF->Tx.MaxPrioQueueArrayIndex);
                EDDI_Excp("EDDI_NrtAddToPrioQueue, invalid PrioQueueArrayIndex", EDDI_FATAL_ERR_EXCP, PrioQueueArrayIndex, pIF->Tx.MaxPrioQueueArrayIndex);
            }

        }
        else //Queue not empty (= RQB available in Queue)
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pQueue->pBottom, pRQB);

            pQueue->Cnt++;
        }

        pQueue->pBottom = pRQB;

        pIF->Tx.QueuedSndReq++;
    }

    LSA_UNUSED_ARG(pDDB);
    #if defined (EDDI_CFG_FRAG_ON)
    return LSA_TRUE; //RQB inserted into SendQueue
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCheckAndRemoveFromQueue()                */
/*                                                                         */
/* D e s c r i p t i o n: search and remove 1 RQB of a specified           */
/*                        LSA-channel from a RQB queue                     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          RQB-PTR                                          */
/*                                                                         */
/***************************************************************************/
EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCheckAndRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                              EDDI_RQB_QUEUE_TYPE      *  const  pQueue,
                                                                              EDDI_LOCAL_HDB_PTR_TYPE     const  pHDB,
                                                                              LSA_BOOL                    const  bSync )
{
    EDD_UPPER_RQB_PTR_TYPE   pRQB;
    EDD_UPPER_RQB_PTR_TYPE   pAct  = pQueue->pTop;
    EDD_UPPER_RQB_PTR_TYPE   pPrev = EDDI_NULL_PTR;
    EDDI_LOCAL_HDB_PTR_TYPE  pLocalHDB;

    //ATTENTION: Queue RQB counter are not updated!

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTCheckAndRemoveFromQueue->");

    for (;;)
    {
        pRQB = pAct;
        if (pRQB == EDDI_NULL_PTR) //no request available
        {
            return pRQB;
        }

        pLocalHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

        //check if desired handle
        if (pLocalHDB != pHDB)
        {
            //skip to next RQB
            pPrev = pAct;
            pAct  = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pAct);
            continue;
        }

        //check if desired service
        if ((  bSync  && (EDD_RQB_GET_SERVICE(pRQB) != EDD_SRV_SYNC_SEND)) ||
            ((!bSync) && (EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SYNC_SEND)))
        {
            //skip to next RQB
            pPrev = pAct;
            pAct  = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pAct);
            continue;
        }

        //dequeue RQB
        if (pPrev)
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pPrev, (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pAct));

            if (EDD_RQB_GET_NEXT_RQB_PTR(pAct) == EDDI_NULL_PTR)
            {
                pQueue->pBottom = pPrev;
            }
        }
        else
        {
            //we got the 1st one -> update pFirstRQB 
            pQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);
        }

        LSA_UNUSED_ARG(pDDB);
        return pRQB;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_q.c                                                 */
/*****************************************************************************/



