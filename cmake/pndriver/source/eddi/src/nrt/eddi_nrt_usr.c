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
/*  F i l e               &F: eddi_nrt_usr.c                            :F&  */
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
/*  17.07.07    JS    added some exceptions                                  */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_nrt_usr.h"

#include "eddi_nrt_tx.h"

#if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)
#include "eddi_nrt_ts.h"
#endif

#include "eddi_nrt_q.h"
#include "eddi_nrt_inc.h"
#include "eddi_lock.h"

#include "eddi_nrt_arp.h"

#define EDDI_MODULE_ID     M_ID_NRT_USR
#define LTRC_ACT_MODUL_ID  207

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_NRTSetDCPFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                            EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_NRTSetDCPHelloFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                 EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );
#endif

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_NrtCheckSndRQB( EDDI_LOCAL_HDB_PTR_TYPE           const  pHDB,
                                                           EDD_UPPER_RQB_PTR_TYPE            const  pRQB,
                                                           EDDI_NRT_CHX_SS_IF_TYPE  const *  const  pIF,
                                                           EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB );


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCheckSndRQB()                            */
/*                                                                         */
/* D e s c r i p t i o n: check RQB for NRT-Send and NRT-Send-Timestamp    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckSndRQB( EDDI_LOCAL_HDB_PTR_TYPE           const  pHDB,
                                                            EDD_UPPER_RQB_PTR_TYPE            const  pRQB,
                                                            EDDI_NRT_CHX_SS_IF_TYPE  const *  const  pIF,
                                                            EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    EDD_UPPER_NRT_SEND_PTR_TYPE  const  pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;
    LSA_UINT32                          PrioIndex;

    if (LSA_HOST_PTR_ARE_EQUAL(pRQBSnd->pBuffer, LSA_NULL))
    {
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, no Buffer");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_PARA_pBuffer_NULL);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
        return LSA_FALSE;
    }

    if (   ((LSA_UINT32)(void *)pRQBSnd->pBuffer < pIF->LimitDown_BufferAddr)
        || ((LSA_UINT32)(void *)pRQBSnd->pBuffer > pIF->LimitUp_BufferAddr))
    {
        EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NrtCheckSndRQB, Data Buffer Address wrong, pBuffer:0x%X LimitDown_BufferAddr:0x%X LimitUp_BufferAddr:0x%X",
                          (LSA_UINT32)(void *)pRQBSnd->pBuffer, (LSA_UINT32)(void *)pIF->LimitDown_BufferAddr, (LSA_UINT32)(void *)pIF->LimitUp_BufferAddr);
        EDDI_Excp("EDDI_NrtCheckSndRQB, Data Buffer Address wrong", EDDI_FATAL_ERR_EXCP,
                  (LSA_UINT32)(void *)pRQBSnd->pBuffer, (LSA_UINT32)(void *)pIF->LimitDown_BufferAddr);
        return LSA_FALSE;
    }

    #if (EDDI_TX_NRT_FRAME_BUFFER_ALIGN)
    if (((LSA_UINT32)(void *)pRQBSnd->pBuffer) & EDDI_TX_NRT_FRAME_BUFFER_ALIGN)
    {
        EDDI_Excp("EDDI_NrtCheckSndRQB, Data Buffer Alignment wrong (see EDDI_TX_NRT_FRAME_BUFFER_ALIGN)", EDDI_FATAL_ERR_EXCP, pRQBSnd->pBuffer, 0);
        return LSA_FALSE;
    }
    #endif

    #if defined (EDDI_CFG_FRAG_ON)
    //NRT TX IF is always locked here
    if (pDDB->NRT.TxFragData.bFragCleanupActive)
    {
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NrtCheckSndRQB, FragCleanup active, RQB confirmed with OK_CANCEL");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_FRAG_CLEANUP_ACTIVE);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
        return LSA_FALSE;
    }
    #endif

    if (pIF->Tx.Dscr.Cnt == 0)
    {
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, no TX-DMACWS");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_TxInUse_SEE_EDD_RQB_CMP_NRT_INI_TYPE);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);
        return LSA_FALSE;
    }

    switch (pRQBSnd->Priority)
    {
        case EDD_NRT_SEND_PRIO_MGMT_HIGH:
        {
            PrioIndex = NRT_MGMT_HIGH_PRIO_INDEX; //Prio-Index
            break;
        }
        case EDD_NRT_SEND_PRIO_MGMT_LOW:
        {
            PrioIndex = NRT_MGMT_LOW_PRIO_INDEX; //Prio-Index
            break;
        }
        case EDD_NRT_SEND_PRIO_ASRT_HIGH:
        {
            PrioIndex = NRT_HIGH_PRIO_INDEX; //Prio-Index
            break;
        }
        case EDD_NRT_SEND_PRIO_ASRT_LOW:
        {
            PrioIndex = NRT_MEDIUM_PRIO_INDEX; //Prio-Index
            break;
        }
        case EDD_NRT_SEND_PRIO_HSYNC:
        {
            #if defined (EDDI_CFG_TRACE_HSA)
            EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "!!!HSA EDDI_NrtCheckSndRQB, P:%d, G(15:1L, 16:1H, 17:2L, 18:2H):%d, Lenght:0x%X, SeqID:0x%X", 
                pRQBSnd->PortID, pRQBSnd->TxFrmGroup, pRQBSnd->Length, pRQBSnd->TxTime);
            #endif
            switch (pRQBSnd->TxFrmGroup)
            {
                case EDD_NRT_TX_GRP_HSYNC1_LOW: 
                {
                    PrioIndex   = NRT_HSYNC1_LOW_PRIO_INDEX;
                    break;
                }
                case EDD_NRT_TX_GRP_HSYNC1_HIGH: 
                {
                    PrioIndex   = NRT_HSYNC1_HIGH_PRIO_INDEX;
                    break;
                }
                case EDD_NRT_TX_GRP_HSYNC2_LOW: 
                {
                    PrioIndex   = NRT_HSYNC2_LOW_PRIO_INDEX;
                    break;
                }
                case EDD_NRT_TX_GRP_HSYNC2_HIGH: 
                {
                    PrioIndex   = NRT_HSYNC2_HIGH_PRIO_INDEX;
                    break;
                }
                default:
                {
                    EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, invalid TxFrmGroup:0x%X", pRQBSnd->TxFrmGroup);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TxFrmGroup);
                    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                    return LSA_FALSE;
                }
            }

            //HSYNC frames can only be send if HSYNCRole “EDD_HSYNC_ROLE_APPL_SUPPORT” is configured in EDD startup (EDD_STS_ERR_PARAM).
            if (EDDI_HSYNC_ROLE_APPL_SUPPORT != pDDB->HSYNCRole)
            {
                EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, HSYNC frames can only be send if HSYNCRole is 'EDD_HSYNC_ROLE_APPL_SUPPORT'");
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_HSyncRole);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                return LSA_FALSE;
            }

            //HSYNC frames cannot be sent with PortID “EDD_PORT_ID_AUTO” (EDD_STS_ERR_PARAM).
            if (EDD_PORT_ID_AUTO == pRQBSnd->PortID)
            {
                EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, HSYNC frames with PortID 'EDD_PORT_ID_AUTO' cannot be sent");
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_HSyncPortID);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                return LSA_FALSE;
            }
            else if ((pRQBSnd->PortID != pDDB->SWITCH.HSyncUserPortID[0]) && (pRQBSnd->PortID != pDDB->SWITCH.HSyncUserPortID[1]))
            {
                EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, HSYNC PortId(0x%X) does not match any MRPRingPort(0x%X / 0x%X)",
                                    pRQBSnd->PortID, pDDB->SWITCH.HSyncUserPortID[0], pDDB->SWITCH.HSyncUserPortID[1]);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_HSyncPortID);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                return LSA_FALSE;
            }
            //else  is checked below
            break;
        }

        default:
        {
            if (pRQBSnd->Priority > EDD_NRT_SEND_PRIO_MGMT_HIGH)
            {
                EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, invalid Priority:0x%X", pRQBSnd->Priority);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_Priority);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                return LSA_FALSE;
            }

            PrioIndex = NRT_LOW_PRIO_INDEX; //Prio-Index
        }
    }

    switch (pRQBSnd->PortID)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            if (pRQBSnd->PortID > pDDB->PM.PortMap.PortCnt)
            {
                EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, invalid PortID, PortID:0x%X PortMapCnt:0x%X",
                                  pRQBSnd->PortID, pDDB->PM.PortMap.PortCnt);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
                EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
                return LSA_FALSE;
            }
            break;
        }

        case EDD_PORT_ID_LOCAL:
        case EDD_PORT_ID_AUTO:
            break;

        default:
        {
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, invalid PortID:0x%X", pRQBSnd->PortID);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
            return LSA_FALSE;
        }
    }

    if (   (pRQBSnd->Length < EDD_NRT_MIN_SND_LEN)
        || (pRQBSnd->Length > pIF->Tx.SndLimit.MaxFrameLen[PrioIndex]))
    {
        EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtCheckSndRQB, invalid SndLen, SndLen:0x%X MinSndLen:0x%X MaxSndLenSndLimit:0x%X PrioIndex:0x%X",
                          pRQBSnd->Length, (LSA_UINT32)EDD_NRT_MIN_SND_LEN, pIF->Tx.SndLimit.MaxFrameLen[PrioIndex], PrioIndex);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SEND_LENGTH);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
        return LSA_FALSE;
    }

    {
        //calculate necessary DBs for this service
        LSA_UINT32  const  FrameLenInDBs = (pRQBSnd->Length + (EDDI_NRT_DB_LEN - 1)) / EDDI_NRT_DB_LEN;

        pRQB->internal_context_1 = FrameLenInDBs | (PrioIndex<<24);
    }

    if (EDD_CDB_INSERT_SRC_MAC_ENABLED == pHDB->InsertSrcMAC)
    {
        EDDI_MEMCOPY(pRQBSnd->pBuffer + 6UL, &pDDB->Glob.LLHandle.xRT.MACAddressSrc, 6UL);
    }

    pHDB->TxCount++;

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRecv()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRecv( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDD_UPPER_NRT_RECV_PTR_TYPE  const  pRQBRcv     = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;
    EDD_UPPER_MEM_U8_PTR_TYPE    const  pDataBuffer = pRQBRcv->pBuffer;
    EDDI_LOCAL_DDB_PTR_TYPE             pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE          *  pIF;
    EDDI_NRT_RX_USER_TYPE            *  pRxUser;
    EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB = EDDI_NRTLockIFRx(pRQB);

    if (pHDB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTRecv, No valid handle found!", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
        return;
    }

    //initialize variables under lock
    pIF     = pHDB->pIF;
    pDDB    = pHDB->pDDB;
    pRxUser = pHDB->pRxUser;

    //initialize IOCount (RQ 1839107, Task 1839257)
    pRQBRcv->IOCount = 0;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTRecv->");

    if (   ((LSA_UINT32)(void *)pDataBuffer < pIF->LimitDown_BufferAddr)
        || ((LSA_UINT32)(void *)pDataBuffer > pIF->LimitUp_BufferAddr))
    {
        EDDI_Excp("EDDI_NRTRecv, Data Buffer Address wrong", EDDI_FATAL_ERR_EXCP, pDataBuffer, 0);
        pIF->Rx.UnLockFct(pIF);
        return;
    }

    if (((LSA_UINT32)(void *)pDataBuffer) & EDDI_RX_FRAME_BUFFER_ALIGN)
    {
        EDDI_Excp("EDDI_NRTRecv, Data Buffer Alignment wrong (see EDDI_RX_FRAME_BUFFER_ALIGN)", EDDI_FATAL_ERR_EXCP, pDataBuffer, 0);
        pIF->Rx.UnLockFct(pIF);
        return;
    }

    if (pIF->Rx.Dscr.Cnt && pRxUser)
    {
        //ATTENTION: do not change!
        #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
        //invalidate nrt-cache
        EDDI_SYNC_CACHE_NRT_RECEIVE(pDataBuffer, EDDI_FRAME_BUFFER_LENGTH);
        #endif

        //put RQB to end of list
        EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

        if (pRxUser->pFirstRQB == EDDI_NULL_PTR)
        {
            pRxUser->pFirstRQB = pRQB;
        }
        else
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pRxUser->pLastRQB, pRQB);
        }

        pRxUser->pLastRQB = pRQB;

        pRxUser->RxQueueReqCnt++;
        pHDB->RxCount++;

        if (pIF->Rx.Queued_RxCount == 0)
        {
            //enable nrt-channel for receiving
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
            //enable NRT-SS-QUEUE (for sending and receiving)
            IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_ENABLE;
        }

        pIF->Rx.Queued_RxCount++;
        pIF->Rx.UnLockFct(pIF);
        return; //Request is queued
    }

    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_RxInUse_SEE_EDD_RQB_CMP_NRT_INI_TYPE);
    
    pIF->Rx.UnLockFct(pIF);
    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSendUsr()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSendUsr( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;
    EDDI_LOCAL_DDB_PTR_TYPE     pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;
    LSA_BOOL                    bRet;

    pHDB = EDDI_NRTLockIFTx(pRQB);

    if (pHDB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTSendUsr, No valid handle found!", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
        return;
    }

    pIF  = pHDB->pIF;
    pDDB = pHDB->pDDB;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSendUsr->");

    if (pHDB->pRQBTxCancelPending)
    {
        pIF->Tx.UnLockFct(pIF);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SEQUENCE);
        return;
    }

    bRet = EDDI_NrtCheckSndRQB(pHDB, pRQB, pIF, pDDB);
    if (bRet)
    {
        #if defined (EDDI_CFG_FRAG_ON)
        if //RQB inserted into SendQueue?
           (EDDI_NrtAddToPrioQueue(pDDB, pIF, pRQB))
        #else
        EDDI_NrtAddToPrioQueue(pDDB, pIF, pRQB);
        #endif
        {
            EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
        }
    }

    pIF->Tx.UnLockFct(pIF);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSendTimestamp()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSendTimestamp( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_LOCAL_HDB_PTR_TYPE         pHDB;
    EDDI_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE      *  pIF;
    LSA_BOOL                        bRet;
    EDD_UPPER_NRT_SEND_PTR_TYPE     pRQBSnd;

    pHDB = EDDI_NRTLockIFTx(pRQB);

    if (pHDB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTSendTimestamp, No valid handle found!", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
        return;
    }

    pIF  = pHDB->pIF;
    pDDB = pHDB->pDDB;
   
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSendTimestamp->");
       
    if (pHDB->pRQBTxCancelPending)
    {
        pIF->Tx.UnLockFct(pIF);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SEQUENCE);
        return;
    }

    bRet = EDDI_NrtCheckSndRQB(pHDB, pRQB, pIF, pDDB);
    if (!bRet)
    {
        pIF->Tx.UnLockFct(pIF);
        return;
    }

    #if defined (EDDI_CFG_REV5)
    {
        if (pIF != pDDB->NRT.pTS_IF)
        {
            pIF->Tx.UnLockFct(pIF);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_INTERFACE);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
            return;
        }

        EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_USR_C, EDDI_TS_CALLER_NRT_SEND_TS_REQ, pDDB->NRT.TimeStamp.State);

        if (   (pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_CHECK_LIST_STATUS)
            || (pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_RECOVER))
        {
            pIF->Tx.UnLockFct(pIF);
            //Blocked and interrupt missing -> behave as having a timeout
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTSendTimestamp->EDD_STS_ERR_TX, TimeStamp.State:%u",
                              pDDB->NRT.TimeStamp.State);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_STATE);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_TX);
            return;
        }
    }
    #endif //EDDI_CFG_REV5

    pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;

    switch (pRQBSnd->PortID)
    {
        case EDD_PORT_ID_LOCAL:
        case EDD_PORT_ID_AUTO:
        {
            pIF->Tx.UnLockFct(pIF);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
            return;
        }
        default: break;
    }

    #if defined (EDDI_CFG_REV5)
    {
        if //another TS-RQB still in process?
           (pDDB->NRT.TimeStamp.pActReq)
        {
            EDDI_AddToQueueEnd(pDDB, &pDDB->NRT.TimeStamp.ReqQueue, pRQB);
            pIF->Tx.UnLockFct(pIF);
            return;
        }

        pDDB->NRT.TimeStamp.pActReq = pRQB;
        pDDB->NRT.TimeStamp.State   = EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS;

        EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_USR_C, EDDI_TS_CALLER_NRT_SEND_TS_REQ, pDDB->NRT.TimeStamp.State);
    }
    #endif //EDDI_CFG_REV5

    #if defined (EDDI_CFG_FRAG_ON)
    if //RQB inserted into SendQueue?
       (EDDI_NrtAddToPrioQueue(pDDB, pIF, pRQB))
    #else
    EDDI_NrtAddToPrioQueue(pDDB, pIF, pRQB);
    #endif
    {
        EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
    }

    pIF->Tx.UnLockFct(pIF);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitDCPFilter()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitDCPFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                               Ctr;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pDDB->pLocal_NRT;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitDCPFilter->");

    //deactivate SW-DCP-Filter
    pNRTComp->DCPFilterTotalCount        = 0;
    pNRTComp->DCPFilterSwCount           = 0;

    pNRTComp->DCPFilterRefCnt            = 0;
    pNRTComp->DCPFilterInvalidFrameCount = 0;
    pNRTComp->DCPFilterDropCount         = 0;

    for (Ctr = 0; Ctr < EDDI_NRT_MAX_DCP_FILTER_CNT; Ctr++)
    {
        NRT_DCP_FILTER_TYPE  *  const  pDCPFilter = &pNRTComp->DCPFilter[Ctr];

        pDCPFilter->pDCP_TLVFilter = &pDCPFilter->DCP_TLVBuffer[0];
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    {
        #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
        LSA_UINT32                                 NRTTransCtrl;
        #endif
        EDDI_R6_NRT_ARP_DCP_FILTER_TYPE  *  const  pR6_arp_dcp = &pNRTComp->R6_arp_dcp;
        LSA_UINT32                                 Index;

        //Init HW-DCP-Filter

        pR6_arp_dcp->pDev_DCP_Filter_Table = &pDDB->pKramFixMem->DCP_FilterTable;

        pDDB->KramRes.DCP_FilterTableLen = sizeof(EDDI_R6_DCP_FILTER_TABLE_TYPE);

        {
            //check DCP filter table memory
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            LSA_UINT32  const  DCPFilterTableLength = (LSA_UINT32)sizeof(EDDI_R6_DCP_FILTER_TABLE_TYPE);
            
            if (   (DCPFilterTableLength != 48UL)                            //lint !e506 BV 21/01/2016 define-based behaviour     //check memory size
                || ((LSA_UINT32)(pR6_arp_dcp->pDev_DCP_Filter_Table) & 7UL))                                                       //check memory alignment
            #else
            #error "Check Implementation!"
            #endif
            {
                EDDI_Excp("EDDI_NRTInitDCPFilter, DCP filter table memory invalid!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
        }

        //init HW DCP filter table
        for (Index = 0; Index < EDDI_MAX_IRTE_DCP_FILTER_TABLE_ENTRIES; Index++)
        {
            pR6_arp_dcp->pDev_DCP_Filter_Table->FilterType[Index] = EDDI_DCP_FILTERTYPE_INVALID;
            pR6_arp_dcp->pDev_DCP_Filter_Table->f[Index].Val32    = EDDI_PASTE_32(FF,FF,FF,FF);
        }

        //init HW dcp filter string buffer: only 1 buffer is allocated and used!

        pR6_arp_dcp->pDev_DCP_Filter_String_Buf = &pDDB->pKramFixMem->DCP_FilterStringBuf;

        pDDB->KramRes.DCP_FilterStringBufLen = sizeof(EDDI_R6_DCP_FILTER_STRING_BUF_TYPE);

        //insert filter string buffer in filter TLV 0
        EDDI_SetBitField32(&pR6_arp_dcp->pDev_DCP_Filter_Table->f[0].Val32,
                            EDDI_R6_DCP_FILTER_TABLE_BIT__FilterStringPtr,
                            DEV_kram_adr_to_asic_register(pR6_arp_dcp->pDev_DCP_Filter_String_Buf, pDDB));

        //deactivate HW-DCP-Filter

        pR6_arp_dcp->DCPFilterHwCount = 0;

        //HW-DCP-Filter is dynamically switched ON/OFF via filter type in filter TLV 0!

        //calculate and init HW DCP filter table address in asic-format with the right endianess
        {
            LSA_UINT32  const  TmpU32 = DEV_kram_adr_to_asic_register(pR6_arp_dcp->pDev_DCP_Filter_Table, pDDB);

            //init HW-DCP-Filter in IRTE-register DCP-TABLE-BASE
            IO_x32(DCP_TABLE_BASE) = EDDI_HOST2IRTE32(TmpU32);
        }

        #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
        EDDI_ENTER_CRITICAL_S(); //ATTENTION: Register-Read-Modify-Write!

        NRTTransCtrl = IO_x32(NRT_TRANS_CTRL);

        //activate HW-DCP-Filter in IRTE-register NRT-Transfer-Control
        EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_DCP_Filter, 1);

        IO_x32(NRT_TRANS_CTRL) = NRTTransCtrl;

        EDDI_EXIT_CRITICAL_S();
        #endif
    }
    #endif //(EDDI_CFG_REV6 || EDDI_CFG_REV7) 
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetDCPFilter()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetDCPFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB        = pHDB->pDDB;
    EDD_UPPER_NRT_SET_DCP_FILTER_PTR_TYPE  const  pRQBFilter  = (EDD_UPPER_NRT_SET_DCP_FILTER_PTR_TYPE)pRQB->pParam;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSetDCPFilter->");

    switch (pRQBFilter->Mode)
    {
        case EDD_DCP_FILTER_ON:
        {
            LSA_UINT32                                  Ctr;
            EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE     const  pNRTComp             = pDDB->pLocal_NRT;
            LSA_UINT32                           const  NewFilterTLVCnt      = (LSA_UINT32)pRQBFilter->FilterTLVCnt;
            EDD_UPPER_MEM_PTR_TYPE                      pNewDCP_TLVFilter[EDDI_NRT_MAX_DCP_FILTER_CNT];  //temporary pointer-list to new TLVs
            LSA_UINT16                                  NewDCP_Type[EDDI_NRT_MAX_DCP_FILTER_CNT];        //temporary type-list of new TLVs
            LSA_UINT16                                  NewDCP_ValueLength[EDDI_NRT_MAX_DCP_FILTER_CNT]; //temporary length-list of new TLVs
            LSA_UINT32                                  NewSwDcpFilterCount  = 0;
            LSA_UINT32                                  NewNosDcpFilterCount = 0;
            LSA_UINT32                                  DcpFilterRefCnt;
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            EDDI_R6_NRT_ARP_DCP_FILTER_TYPE   *  const  pR6_arp_dcp          = &pNRTComp->R6_arp_dcp;
            LSA_UINT32                                  NOS_FilterIndex      = 0xFFFFFFFFUL; //= unused
            #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
            EDD_UPPER_MEM_U8_PTR_TYPE                   pNOS_TLVFilter       = 0;
            #endif
            #endif

            //check new filter TLV count from rqb (maximum count = GetParams.MaxPortCnt+1)
            if (   (NewFilterTLVCnt  > (LSA_UINT32)(pDDB->PM.PortMap.PortCnt+1UL))
                || (NewFilterTLVCnt == 0))
            {
                return EDD_STS_ERR_PARAM;
            }

            for (Ctr = 0; Ctr < EDDI_NRT_MAX_DCP_FILTER_CNT; Ctr++)
            {
                pNewDCP_TLVFilter[Ctr] = (EDD_UPPER_MEM_PTR_TYPE)0;
                NewDCP_Type[Ctr] = 0;
                NewDCP_ValueLength[Ctr] = 0;
            }

            //1. check all new filter TLVs
            for (Ctr = 0; Ctr < NewFilterTLVCnt; Ctr++)
            {
                //following variable in host format!
                EDD_UPPER_MEM_U8_PTR_TYPE  const  pDCP_TLVFilter = (EDD_UPPER_MEM_U8_PTR_TYPE)(LSA_VOID *)(pRQBFilter->FilterTLV[Ctr].pTLV);

                //store new TLV pointer in a temporary pointer-list
                pNewDCP_TLVFilter[Ctr] = (EDD_UPPER_MEM_PTR_TYPE)pDCP_TLVFilter;

                if (LSA_HOST_PTR_ARE_EQUAL(pDCP_TLVFilter, LSA_NULL))
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_pFilterTLV_NULL);
                    return EDD_STS_ERR_PARAM;
                }
                else
                {
                    //following variable in host format!
                    LSA_UINT16  const  FilterType = (LSA_UINT16)((pDCP_TLVFilter[EDDI_DCP_TLV_TYPE_OFFSET] << 8) + pDCP_TLVFilter[EDDI_DCP_TLV_TYPE_OFFSET + 1]);

                    //store new filter-type in a temporary type-list
                    NewDCP_Type[Ctr] = FilterType;

                    switch (FilterType)
                    {
                        case EDDI_DCP_FILTERTYPE_NOS:
                        {
                            //check NOS-TLVs
                            if (NewNosDcpFilterCount) //only 1 NOS filter is allowed (Rev6/7: in HW)
                            {
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_DCP_Filter_NOS);
                                return EDD_STS_ERR_PARAM;
                            }
                            else
                            {
                                EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTSetDCPFilter, check NOS filter mode ON, RQB.FilterTLVCnt:%u",
                                                  NewFilterTLVCnt);
                                NewNosDcpFilterCount = 1UL;
                                #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
                                //filter can be realized in HW
                                //save parameters for later filter setting
                                //ATTENTION: code is optimized for only 1 HW-DCP-Filter-Entry (NOS)!
                                NOS_FilterIndex = Ctr;
                                #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
                                pNOS_TLVFilter = pDCP_TLVFilter;
                                #endif
                                #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7
                            }
                        }
                        break;

                        case EDDI_DCP_FILTERTYPE_ALL:
                        {
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_DCP_Filter_ALL);
                            return EDD_STS_ERR_PARAM;
                        }

                        default: break;
                    }

                    //check filter-length
                    {
                        //following variable in host format!
                        LSA_UINT16  const  ValueLength = (LSA_UINT16)((pDCP_TLVFilter[EDDI_DCP_TLV_LENGTH_OFFSET] << 8) + pDCP_TLVFilter[EDDI_DCP_TLV_LENGTH_OFFSET + 1]);

                        //check TLV-length
                        if (   (ValueLength == 0)                                                                //length too small?
                            || (ValueLength > EDDI_MAX_DCP_FILTER_STRING_BUFFER_LEN)                             //length too large? => length is limited (Rev6/7: for KRAM-saving)
                            || (ValueLength != (pRQBFilter->FilterTLV[Ctr].TLVLen - EDDI_DCP_TLV_VALUE_OFFSET))) //length inconsistent?
                        {
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_DCP_Filter_LEN);
                            return EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            //store new filter-length in a temporary length-list
                            NewDCP_ValueLength[Ctr] = ValueLength;
                        }
                    }
                }
            } //for-loop

            DcpFilterRefCnt = pNRTComp->DCPFilterRefCnt;

            //RefCnt == even: SW-DCP-Filter-Update currently not in progress!
            //RefCnt == odd:  SW-DCP-Filter-Update currently in progress!

            if //RefCnt odd/invalid?
               (DcpFilterRefCnt & 1UL)
            {
                EDDI_Excp("EDDI_NRTSetDCPFilter, invalid RefCnt:", EDDI_FATAL_ERR_EXCP, DcpFilterRefCnt, 0);
                return EDD_STS_ERR_EXCP;
            }

            //set RefCnt = odd: SW-DCP-Filter-Update currently in progress!
            DcpFilterRefCnt++;
            pNRTComp->DCPFilterRefCnt = DcpFilterRefCnt;

            //here all negative-checks are already done

            //deactivate all filter TLVs

            //deactivate SW-DCP-Filter
            pNRTComp->DCPFilterSwCount = 0;

            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            if //HW-DCP-Filter is currently active?
               (pR6_arp_dcp->DCPFilterHwCount)
            {
                #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
                //deactivate HW-DCP-Filter
                pR6_arp_dcp->pDev_DCP_Filter_Table->FilterType[0] = EDDI_DCP_FILTERTYPE_INVALID;
                #endif

                pR6_arp_dcp->DCPFilterHwCount = 0;
            }
            #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7

            //2. update all filter TLVs

            for (Ctr = 0; Ctr < NewFilterTLVCnt; Ctr++)
            {
                #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
                if //HW-Filter NOS can be used?
                   (NOS_FilterIndex == Ctr)
                {
                    LSA_UINT32  const  NOS_ValueLength = (LSA_UINT32)NewDCP_ValueLength[Ctr];

                    //NOS filter TLV is realized in HW

                    #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
                    //FilterType is set later (to NOS)!
                    //FilterStringPointer is already set!

                    //set FilterStringLength
                    EDDI_SetBitField32(&pR6_arp_dcp->pDev_DCP_Filter_Table->f[0].Val32,
                                       EDDI_R6_DCP_FILTER_TABLE_BIT__FilterStringLength,
                                       NOS_ValueLength);

                    //copy FilterString into KRAM (no endianess problem because of byte-stream)
                    //temporarily_disabled_lint --esym(613, pNOS_TLVFilter)
                    EDDI_MemCopy(pR6_arp_dcp->pDev_DCP_Filter_String_Buf,
                                 &pNOS_TLVFilter[EDDI_DCP_TLV_VALUE_OFFSET],
                                 NOS_ValueLength);
                    #endif
                }
                #if !defined (EDDI_CFG_DEFRAG_ON)
                else
                #endif //EDDI_CFG_DEFRAG_ON
                #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7
                {
                    NRT_DCP_FILTER_TYPE        *  const  pDCPFilter     = &pNRTComp->DCPFilter[NewSwDcpFilterCount];
                    EDD_UPPER_MEM_U8_PTR_TYPE     const  pNew_TLVFilter = (EDD_UPPER_MEM_U8_PTR_TYPE)pNewDCP_TLVFilter[Ctr];

                    //filter TLV is realized in SW

                    //filter-data are copied to the DDB
                    pDCPFilter->Type        = NewDCP_Type[Ctr];
                    pDCPFilter->ValueLength = NewDCP_ValueLength[Ctr];

                    //copy FilterString into DDB (no endianess problem because of byte-stream)
                    EDDI_MemCopy(&pDCPFilter->pDCP_TLVFilter[EDDI_DCP_TLV_VALUE_OFFSET],
                                 &pNew_TLVFilter[EDDI_DCP_TLV_VALUE_OFFSET],
                                 (LSA_UINT32)pDCPFilter->ValueLength);

                    NewSwDcpFilterCount++;
                }
            } //for-loop

            #if defined (EDDI_UNUSED)
            //reset remaining SW TLVs
            for (Ctr = NewSwDcpFilterCount; Ctr < EDDI_NRT_MAX_DCP_FILTER_CNT; Ctr++)
            {
                pNRTComp->DCPFilter[Ctr].pDCP_TLVFilter = EDDI_NULL_PTR;
            }
            #endif

            pNRTComp->DCPFilterHandle = pHDB->Handle; //channel that activates the DCP-Filter at last

            //activate SW-DCP-Filter
            pNRTComp->DCPFilterSwCount = NewSwDcpFilterCount;

            //insert new filter TLV counter into DDB
            pNRTComp->DCPFilterTotalCount = NewFilterTLVCnt;

            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            if //HW-DCP-Filter must be activated?
               (NewNosDcpFilterCount)
            {
                #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
                //activate HW-DCP-Filter
                pR6_arp_dcp->pDev_DCP_Filter_Table->FilterType[0] = EDDI_DCP_FILTERTYPE_NOS;
                #endif

                pR6_arp_dcp->DCPFilterHwCount = NewNosDcpFilterCount;
            }
            #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7

            //set RefCnt = even: SW-DCP-Filter-Update finished!
            DcpFilterRefCnt++;
            pNRTComp->DCPFilterRefCnt = DcpFilterRefCnt;

            break;
        }

        case EDD_DCP_FILTER_OFF:
        {
            EDDI_NRTSetDCPFilterOff(pDDB);
            break;
        }

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_Mode);
            return EDD_STS_ERR_PARAM;
        }
    }

    //here only positive returns occur!
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetDCPFilterOff()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetDCPFilterOff( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE     const  pNRTComp    = pDDB->pLocal_NRT;
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_R6_NRT_ARP_DCP_FILTER_TYPE   *  const  pR6_arp_dcp = &pNRTComp->R6_arp_dcp;
    #endif

    EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTSetDCPFilterOff, old DCPFilterSwCount:%u", pNRTComp->DCPFilterSwCount);

    //deactivate SW-DCP-Filter
    pNRTComp->DCPFilterSwCount = 0; //= deactive SW-DCP-Filter

    //insert new filter TLV counter into DDB
    pNRTComp->DCPFilterTotalCount = 0;

    pNRTComp->DCPFilterRefCnt += 2UL; //force SW-DCP-Filter-inconsistency

    #if defined (EDDI_UNUSED)
    //reset all SW TLVs
    for (Ctr = 0; Ctr < EDDI_NRT_MAX_DCP_FILTER_CNT; Ctr++)
    {
        pNRTComp->DCPFilter[Ctr].pDCP_TLVFilter = EDDI_NULL_PTR;
    }
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if //HW-DCP-Filter is currently active?
       (pR6_arp_dcp->DCPFilterHwCount)
    {
        #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
        //deactivate HW-DCP-Filter
        pR6_arp_dcp->pDev_DCP_Filter_Table->FilterType[0] = EDDI_DCP_FILTERTYPE_INVALID;
        #endif

        pR6_arp_dcp->DCPFilterHwCount = 0;
    }
    #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitDCPHelloFilter()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitDCPHelloFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pDDB->pLocal_NRT;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitDCPHelloFilter->");

    pNRTComp->DCPHelloFilterRefCnt            = 0;
    pNRTComp->DCPHelloFilterInvalidFrameCount = 0;
    pNRTComp->DCPHelloFilterDropCount         = 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetDCPHelloFilter()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetDCPHelloFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                    EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_UINT32                                          Ctr1, Ctr2;
    EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE  const  pRqbParam = (EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE) pRQB->pParam;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE             const  pNRTComp  = pHDB->pDDB->pLocal_NRT;

    EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSetDCPHelloFilter->");

    switch (pRqbParam->Mode)
    {
        case EDD_DCP_HELLO_FILTER_OFF_ALL:
        {
            //Turn off all filters
            for (Ctr1 = 0; Ctr1 < EDD_DCP_MAX_DCP_HELLO_FILTER; Ctr1++)
            {
                pNRTComp->DCPHelloFilter[Ctr1].bInUse = LSA_FALSE;
            }
            return EDD_STS_OK;
        }

        case EDD_DCP_HELLO_FILTER_OFF:
        {
            //search for UserID and turn off the first active filter with this UserID
            for (Ctr1 = 0; Ctr1 < EDD_DCP_MAX_DCP_HELLO_FILTER; Ctr1++)
            {
                NRT_DCP_HELLO_FILTER_TYPE  *  const  pFilter = &pNRTComp->DCPHelloFilter[Ctr1];

                if (   (pFilter->bInUse)
                    && (pRqbParam->UserID == pFilter->UserID)
                    && (pFilter->pHDB     == pHDB))
                {
                    pFilter->bInUse = LSA_FALSE;
                    return EDD_STS_OK;
                }
            }

            return EDD_STS_ERR_RESOURCE; //specified filter not found
        }

        case EDD_DCP_HELLO_FILTER_ON:
        {
            if (   (LSA_HOST_PTR_ARE_EQUAL(pRqbParam->pNameOfStation, LSA_NULL))
                || (pRqbParam->NameOfStationLen == 0)
                || (pRqbParam->NameOfStationLen > EDD_DCP_MAX_DCP_HELLO_NAME_OF_STATION))
            {
                return EDD_STS_ERR_PARAM;
            }

            //search for a free filter entry
            for (Ctr1 = 0; Ctr1 < EDD_DCP_MAX_DCP_HELLO_FILTER; Ctr1++)
            {
                NRT_DCP_HELLO_FILTER_TYPE  *  const  pFilter = &pNRTComp->DCPHelloFilter[Ctr1];

                if (!pFilter->bInUse)
                {
                    Ctr2 = pRqbParam->NameOfStationLen;

                    while (Ctr2)
                    {
                        Ctr2--;
                        pFilter->StationName[Ctr2] = pRqbParam->pNameOfStation[Ctr2];
                    }

                    pFilter->StationNameLen = pRqbParam->NameOfStationLen;
                    pFilter->UserID         = pRqbParam->UserID; //no check is executed if the same UserID is used multiple!
                    pFilter->pHDB           = pHDB;
                    pFilter->bInUse         = LSA_TRUE; //last action: activate filter
                    return EDD_STS_OK;
                }
            }

            return EDD_STS_ERR_RESOURCE; //free filter entry not available
        }

        default:
            return EDD_STS_ERR_PARAM;
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetFilters()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetFilters( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                   EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_RESULT  Status;

    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTSetFilters->");

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_NRT_SET_ARP_FILTER:
            #if defined (EDDI_CFG_DISABLE_ARP_FILTER)
            Status = EDD_STS_OK;
            #else
            Status = EDDI_NRTSetARPFilter(pRQB, pHDB);
            #endif
            break;
        case EDD_SRV_NRT_SET_DCP_FILTER:
            Status = EDDI_NRTSetDCPFilter(pRQB, pHDB);
            break;
        case EDD_SRV_NRT_SET_DCP_HELLO_FILTER:
            #if defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
            Status = EDD_STS_OK;
            #else
            Status = EDDI_NRTSetDCPHelloFilter(pRQB, pHDB);
            #endif
            break;
        default:
            Status = EDD_STS_ERR_SERVICE;
            break;
    }

    EDDI_RequestFinish(pHDB, pRQB, Status);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_usr.c                                               */
/*****************************************************************************/
