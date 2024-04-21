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
/*  F i l e               &F: eddi_nrt_frag_tx.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time): Transmit Fragmentation      */
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
//#include "eddi_nrt_usr.h"

#if defined (EDDI_CFG_FRAG_ON)

#include "eddi_nrt_frag_tx.h"

//#include "eddi_ext.h"
//#include "eddi_nrt_q.h"
//#include "eddi_nrt_inc.h"
#include "eddi_lock.h"
#include "eddi_time.h"
#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_NRT_FRAG_TX
#define LTRC_ACT_MODUL_ID  212

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxFrag( EDDI_DDB_TYPE  *  const  pDDB,
                                                              LSA_UINT32        const  HwPortIndex );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxFrag1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                 EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                 LSA_UINT32                  const  QueueIdx,
                                                                 LSA_UINT32                  const  NrtSendDescrIrteAdr );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxDmacwQueue1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                       LSA_UINT32                  const  QueueIdx );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxSendQueues1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                       LSA_UINT32                  const  QueueIdx );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxPreQueues1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                      EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupFragDeallocQueue1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                           EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTFragInitStartup()                        */
/*                                                                         */
/* D e s c r i p t i o n: initialize fragmentation control data            */
/*                        at EDDI startup                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragInitStartup( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB    = &pDDB->NRT.TxFragData;
    LSA_UINT8                  *  const  pNRTFragHeaderBuffer = (LSA_UINT8 *)(LSA_VOID *)&pNrtTxFragDataDDB->NRTFragHeaderBuffer;

    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    if //fragment parameters initialized wrong?
       (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping)
    {
        EDDI_Excp("EDDI_NRTFragInitStartup, fragment parameters initialized wrong!", EDDI_FATAL_ERR_EXCP, pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping, 0);
        return;
    }
    #endif

    //init FRAG DDB parameters
    pNrtTxFragDataDDB->bTxFragmentationOn  = LSA_FALSE;
    pNrtTxFragDataDDB->bFragCleanupActive  = LSA_FALSE;
    pNrtTxFragDataDDB->SendLengthMaxNoFrag = 0xFFFFFFFFUL; //set to highest value = no fragmentation possible
    pNrtTxFragDataDDB->FragmentFrameIdLow  = (LSA_UINT8)0x80;
    #if !defined (EDDI_TX_FRAG_DEBUG_MODE)
    pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping = 0;
    #endif

    //preset global buffer with constant NRT Fragment Header data
    //EtherType
    pNRTFragHeaderBuffer[0] = (LSA_UINT8)0x88;
    pNRTFragHeaderBuffer[1] = (LSA_UINT8)0x92;
    //FrameID
    pNRTFragHeaderBuffer[2] = (LSA_UINT8)0xFF;
    //FragmentFrameIdLow is set later (consistency)!

    //TimerStart init
    pDDB->NRT.bNRTPreQueuesTimerStart = LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTFragCheckPrmCommitPart1()                */
/*                                                                         */
/* D e s c r i p t i o n: check fragmentation control data                 */
/*                        at PRM-Commit Part1                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragCheckPrmCommitPart1( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    //here PDIRData Set B must be analysed
    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB     = &pDDB->NRT.TxFragData;
    //store old TX fragmentation status
    LSA_BOOL                      const  bOldTxFragmentationOn = pNrtTxFragDataDDB->bTxFragmentationOn;
    LSA_UINT32                    const  OldActiveHwPortIndex  = pNrtTxFragDataDDB->ActiveHwPortIndex;
    //new TX fragmentation status
    LSA_UINT32                    const  NewActiveHwPortIndex  = pDDB->PRM.PDIRData.pRecordSet_B->NrtFrag.FragHwPortIndex;
    LSA_UINT32                    const  NewActivePortCnt      = pDDB->PRM.PDIRData.pRecordSet_B->NrtFrag.ActivePortCnt;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTFragCheckPrmCommitPart1->");

    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    if //fragmentation cleanup active?
       (pNrtTxFragDataDDB->bFragCleanupActive)
    {
        EDDI_Excp("EDDI_NRTFragCheckPrmCommitPart1, fragmentation cleanup active!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    #endif

    if //old Tx Fragmentation ON?
       (bOldTxFragmentationOn)
    {
        if //   (PDIRData SetB invalid?)
           //OR (more than 1 port active?)
           //OR (the only one port changed?)
           (   (EDDI_PRM_WRITE_DONE != pDDB->PRM.PDIRData.State_B)
            || (NewActivePortCnt > 1UL)
            || ((NewActivePortCnt == 1UL) && (NewActiveHwPortIndex != OldActiveHwPortIndex)))
        {
            //cleanup the old port
            EDDI_NrtCleanupTxFrag(pDDB, OldActiveHwPortIndex);
        }
    }
    else //old Tx Fragmentation OFF
    {
        //nothing to do here
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTFragCheckPrmCommitPart2()                */
/*                                                                         */
/* D e s c r i p t i o n: check fragmentation control data                 */
/*                        at PRM-Commit Part2                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragCheckPrmCommitPart2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    //here PDIRData Set A must be analysed
    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB      = &pDDB->NRT.TxFragData;
    LSA_RESULT                           Status;
    //store old TX fragmentation status
    LSA_BOOL                      const  bOldTxFragmentationOn  = pNrtTxFragDataDDB->bTxFragmentationOn;
    LSA_UINT32                    const  OldSendLengthMaxNoFrag = pNrtTxFragDataDDB->SendLengthMaxNoFrag;
    LSA_UINT32                    const  OldActiveHwPortIndex   = pNrtTxFragDataDDB->ActiveHwPortIndex;
    //new TX fragmentation status
    LSA_BOOL                             bNewTxFragmentationOn  = LSA_FALSE;
    LSA_UINT32                    const  NewPrmFragSize         = pDDB->PRM.PDIRData.pRecordSet_A->NrtFrag.FragSize;
    LSA_UINT32                           NewSendLengthMaxNoFrag = 0xFFFFFFFFUL; //set to highest value = no fragmentation possible
    LSA_UINT32                           NewActivePortCnt       = pDDB->PRM.PDIRData.pRecordSet_A->NrtFrag.ActivePortCnt;
    LSA_UINT32                    const  NewActiveUsrPortIndex  = pDDB->PRM.PDIRData.pRecordSet_A->NrtFrag.FragUsrPortIndex;
    LSA_UINT32                    const  NewActiveHwPortIndex   = pDDB->PRM.PDIRData.pRecordSet_A->NrtFrag.FragHwPortIndex;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTFragCheckPrmCommitPart2->");

    if //PDIRData valid?
       (EDDI_PRM_VALID == pDDB->PRM.PDIRData.State_A)
    {
        //number of active ports is already checked by PRM

        //check PrmFragSize
        switch (NewPrmFragSize)
        {
            case 0: //fragmentation off
            {
                break;
            }   
            case 128: //fragmentation on with maximum fragment size = 128 Bytes
            {
                //Switch Prio to 0-3, disable SRT Prio Bit, because with Prio 4 the ordner is not guaranteed
                bNewTxFragmentationOn  = LSA_TRUE;
                NewSendLengthMaxNoFrag = MAX_TX_FRAG_NETTO_SIZE_LOW;
            }   break;
            case 256: //fragmentation on with maximum fragment size = 256 Bytes
            {
                //Switch Prio to 0-3, disable SRT Prio Bit, because with Prio 4 the ordner is not guaranteed
                bNewTxFragmentationOn  = LSA_TRUE;
                NewSendLengthMaxNoFrag = MAX_TX_FRAG_NETTO_SIZE_HIGH;
            }   break;
            default:
            {
                EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTFragCheckPrmCommitPart2, invalid PrmFragSize:%d", NewPrmFragSize);
                EDDI_Excp("EDDI_NRTFragCheckPrmCommitPart2, invalid PrmFragSize:", EDDI_FATAL_ERR_EXCP, NewPrmFragSize, 0);
                return;
            }
        }

        if //fragment parameters set wrong?
           (bNewTxFragmentationOn && (NewActivePortCnt != 1UL))
        {
            EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTFragCheckPrmCommitPart2, fragment parameters set wrong by PRM, bNewTxFragmentationOn:0x%X NewActivePortCnt:0x%X",
                              bNewTxFragmentationOn, NewActivePortCnt);
            EDDI_Excp("EDDI_NRTFragCheckPrmCommitPart2, fragment parameters set wrong by PRM!", EDDI_FATAL_ERR_EXCP, bNewTxFragmentationOn, NewActivePortCnt);
            return;
        }
    }
    else //PDIRData invalid
    {
        //fragmentation off
        NewActivePortCnt = 0;
    }

    if //fragmentation cleanup not yet done in EDDI_NRTFragCheckPrmCommitPart1()?
       (!pNrtTxFragDataDDB->bFragCleanupActive)
    {
        if //old Tx Fragmentation ON?
           (bOldTxFragmentationOn)
        {
            if //only 1 port active?
               (NewActivePortCnt == 1UL)
            {
                if //active port not changed?
                   (NewActiveHwPortIndex == OldActiveHwPortIndex)
                {
                    if //FragSize not changed?
                       (NewSendLengthMaxNoFrag == OldSendLengthMaxNoFrag)
                    {
                        //case 1: active port not changed AND FragSize not changed:
                        //        => TX fragmentation remains ON
                    }
                    else //FragSize changed
                    {
                        //case 2: active port not changed AND FragSize changed:
                        //cleanup the new = old port
                        EDDI_NrtCleanupTxFrag(pDDB, NewActiveHwPortIndex);
                    }
                }
                else //active port changed
                {
                    if //FragSize not changed?
                       (NewSendLengthMaxNoFrag == OldSendLengthMaxNoFrag)
                    {
                        //case 3: active port changed AND FragSize not changed:
                        //        => the Cleanup is already executed in EDDI_NRTFragCheckPrmCommitPart1()
                        //        => TX fragmentation remains ON
                    }
                    else //FragSize changed
                    {
                        //case 4: active port changed AND FragSize changed:
                        //        => the Cleanup is already executed in EDDI_NRTFragCheckPrmCommitPart1()
                    }
                }
            }
            else //not 1 port active
            {
                //case 5: more than 1 port active:
                //case 6: PDIRData.State_A != VALID:
                //        => the Cleanup is already executed in EDDI_NRTFragCheckPrmCommitPart1()
            }
        }
        else //old Tx Fragmentation OFF
        {
            if //    (PDIRData Set A valid?)
               //AND (only 1 port active?)
               //AND (FragSize != 0?)
               (bNewTxFragmentationOn)
            {
                //cleanup the new port
                EDDI_NrtCleanupTxFrag(pDDB, NewActiveHwPortIndex);
            }
        }
    }

    if //old Tx Fragmentation ON?
       (bOldTxFragmentationOn)
    {
        //prepare data for link indication
        pDDB->pLocal_SWITCH->LinkIndExtPara[OldActiveHwPortIndex].TxFragmentation = EDD_PORT_NO_TX_FRAGMENTATION;
    }

    if //new Tx Fragmentation ON?
       (bNewTxFragmentationOn)
    {
        EDDI_TIMER_TYPE  *  pTimer;

        //prepare data for link indication
        pDDB->pLocal_SWITCH->LinkIndExtPara[NewActiveHwPortIndex].TxFragmentation = EDD_PORT_TX_FRAGMENTATION;

        //init FRAG DDB parameters for positive case
        pNrtTxFragDataDDB->ActiveUsrPortIndex = NewActiveUsrPortIndex;
        pNrtTxFragDataDDB->ActiveHwPortIndex  = NewActiveHwPortIndex;

        (void)EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RESET, 0, 0);

        if (!pDDB->NRT.bNRTPreQueuesTimerStart)
        {
            pTimer = &g_pEDDI_Info ->Timer[pDDB->NRT.NRTPreQueuesTimerID];
            
            //init RQB for context-switch via DO-macro          
            pTimer->ShedObj.IntRQB.pParam = (EDD_UPPER_MEM_PTR_TYPE)pDDB; //RQB.pParam : contains pDDB

            //Start Timer
            Status = EDDI_StartTimer(pDDB, pDDB->NRT.NRTPreQueuesTimerID, 1);
            if (Status != EDD_STS_OK)
            {
                EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTFragCheckPrmCommitPart2, EDDI_StartTimer FAILED, Status:0x%X TimerID:0x%X",
                                  Status, pDDB->NRT.NRTPreQueuesTimerID);
                EDDI_Excp("EDDI_NRTFragCheckPrmCommitPart2, EDDI_StartTimer FAILED", EDDI_FATAL_ERR_EXCP, Status, pDDB->NRT.NRTPreQueuesTimerID);
                return;
            }
            pDDB->NRT.bNRTPreQueuesTimerStart = LSA_TRUE;
        }

        //disable SRT Prio Bit
        EDDI_SwiPortSetEnableSRTPrio(NewActiveHwPortIndex, LSA_FALSE, pDDB);
        pDDB->NRT.RxFragData.bDefragOn = LSA_TRUE;
    }
    else //new Tx Fragmentation OFF
    {
        //stop Timer, if a timer was started and the prm-data was changed and the new prm-data doesn't contain nrt-fragmentation!
        if (pDDB->NRT.bNRTPreQueuesTimerStart)
        {
            Status = EDDI_StopTimer(pDDB, pDDB->NRT.NRTPreQueuesTimerID);
            if (Status != EDD_STS_OK)
            {
                EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTFragCheckPrmCommitPart2, EDDI_StopTimer FAILED, Status:0x%X TimerID:0x%X",
                                  Status, pDDB->NRT.NRTPreQueuesTimerID);
                EDDI_Excp("EDDI_NRTFragCheckPrmCommitPart2, EDDI_StopTimer FAILED", EDDI_FATAL_ERR_EXCP, Status, pDDB->NRT.NRTPreQueuesTimerID);
                return;
            }
            pDDB->NRT.bNRTPreQueuesTimerStart = LSA_FALSE;
        }

        //Switch Prio to 4, enable SRT Prio Bit
        EDDI_SwiPortSetEnableSRTPrio(NewActiveHwPortIndex, LSA_TRUE, pDDB);
        pDDB->NRT.RxFragData.bDefragOn = LSA_FALSE;
    }

    //init FRAG DDB parameters
    pNrtTxFragDataDDB->bTxFragmentationOn  = bNewTxFragmentationOn;
    pNrtTxFragDataDDB->SendLengthMaxNoFrag = NewSendLengthMaxNoFrag;
    pNrtTxFragDataDDB->ActivePortCnt       = NewActivePortCnt;

    //a link indication is always executed by caller PRM afterwards in EDDI_PrmMoveBToAPart2() for all ports: TX fragmentation changed

    pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping = 0;
    pNrtTxFragDataDDB->bFragCleanupActive                      = LSA_FALSE;

    EDDI_SwiPNUpdateSyncMACPrio(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupTxFrag()                          */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup complete TX Fragmentation of             */
/*                        all NRT interfaces at Frag rescheduling          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxFrag( EDDI_DDB_TYPE  *  const  pDDB,
                                                              LSA_UINT32        const  HwPortIndex )
{
    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;
    SWI_LINK_PARAM_TYPE        *  const  pLinkPx           = &pDDB->SWITCH.LinkPx[HwPortIndex];
    SER_SWI_LINK_TYPE                    LinkState;
    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    LSA_UINT16                           SpanningTreeState;
    LSA_BOOL                             bFireLinkDownAction;
    #endif
    EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIFA0             = &pDDB->NRT.IF[EDDI_NRT_CHA_IF_0];
    EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIFB0             = &pDDB->NRT.IF[EDDI_NRT_CHB_IF_0];

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag->");

    pIFA0->Tx.LockFct(pIFA0);
    //disable nrt-channel for sending
    IO_x32(pIFA0->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE);
    pIFB0->Tx.LockFct(pIFB0);
    //disable nrt-channel for sending
    IO_x32(pIFB0->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE);

    pNrtTxFragDataDDB->bFragCleanupActive = LSA_TRUE;

    pIFB0->Tx.UnLockFct(pIFB0);
    pIFA0->Tx.UnLockFct(pIFA0);

    //store current LinkState
    LinkState.LinkStatus = pLinkPx->LinkStatus;
    LinkState.LinkSpeed  = pLinkPx->LinkSpeed;
    LinkState.LinkMode   = pLinkPx->LinkMode;
    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    SpanningTreeState    = pLinkPx->SpanningTreeState;
    bFireLinkDownAction  = pLinkPx->bFireLinkDownAction;
    #endif

    pLinkPx->bFireLinkDownAction = LSA_TRUE;
    EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);

    EDDI_NrtCleanupTxFrag1If(pDDB, pIFA0, EDDI_NRT_CHA_IF_0, NRT_SND_DESCRIPTOR_CHA0);
    EDDI_NrtCleanupTxFrag1If(pDDB, pIFB0, EDDI_NRT_CHB_IF_0, NRT_SND_DESCRIPTOR_CHB0);

    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    if //change occur in LinkStatus?
       (LinkState.LinkStatus != pLinkPx->LinkStatus)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag, change occur in LinkStatus, LinkStatus old:0x%X new:0x%X", LinkState.LinkStatus, pLinkPx->LinkStatus);
    }
    if //change occur in LinkSpeed?
       (LinkState.LinkSpeed != pLinkPx->LinkSpeed)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag, change occur in LinkSpeed, LinkSpeed old:0x%X new:0x%X", LinkState.LinkSpeed, pLinkPx->LinkSpeed);
    }
    if //change occur in LinkMode?
       (LinkState.LinkMode != pLinkPx->LinkMode)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag, change occur in LinkMode, LinkMode old:0x%X new:0x%X", LinkState.LinkMode, pLinkPx->LinkMode);
    }
    if //change occur in SpanningTreeState?
       (SpanningTreeState != pLinkPx->SpanningTreeState)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag, change occur in SpanningTreeState, SpanningTreeState old:0x%X new:0x%X", SpanningTreeState, pLinkPx->SpanningTreeState);
    }
    if //change occur in bFireLinkDownAction?
       (bFireLinkDownAction != pLinkPx->bFireLinkDownAction)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxFrag, change occur in bFireLinkDownAction, bFireLinkDownAction old:0x%X new:0x%X", bFireLinkDownAction, pLinkPx->bFireLinkDownAction);
    }
    #endif

    if //previous LinkStatus == LINK_UP?
       (   (EDD_LINK_UP        == LinkState.LinkStatus)
        || (EDD_LINK_UP_CLOSED == LinkState.LinkStatus))
    {
        if //Speed and Mode can be determined?
           ((EDD_LINK_UNKNOWN != LinkState.LinkSpeed) && (EDD_LINK_UNKNOWN != LinkState.LinkMode))
        {
            EDDI_SwiPhyActionForLinkUp(HwPortIndex,
                                       pLinkPx->LinkSpeedMode_Config,
                                       &LinkState,
                                       pDDB);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupTxFrag1If()                       */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup TX Fragmentation of one NRT interface    */
/*                        at Frag rescheduling                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxFrag1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                 EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                 LSA_UINT32                  const  QueueIdx,
                                                                 LSA_UINT32                  const  NrtSendDescrIrteAdr )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtCleanupTxFrag1If->");

    if //at least 1 occupied TX-DMACW is existent on this IF?
       (pIF->Tx.BalanceSndReq)
    {
        SER_TOP_DMACW_ADR_TYPE  *  const  pTopAdrDMACW = &pDDB->Glob.LLHandle.TopAdrDMACW[QueueIdx];
        LSA_UINT32                        tmp;

        //check NRT-Descriptor-TX
        IO_x32(NrtSendDescrIrteAdr) = 0xFFFFFFF8UL; //8-byte-aligned!
        if (IO_x32(NrtSendDescrIrteAdr) != 0xFFFFFFF8UL)
        {
            EDDI_Excp("EDDI_NrtCleanupTxFrag1If, invalid readback of NRT send descriptor:", EDDI_FATAL_ERR_EXCP, IO_x32(NrtSendDescrIrteAdr), 0);
            return;
        }

        //cleanup TX DMACW queue of one NRT interface
        EDDI_NrtCleanupTxDmacwQueue1If(pDDB, pIF, QueueIdx);

        //init NRT-Descriptor-TX
        IO_x32(NrtSendDescrIrteAdr) = pTopAdrDMACW->Tx;
        tmp = IO_x32(NrtSendDescrIrteAdr);
        if (tmp != pTopAdrDMACW->Tx)
        {
            EDDI_Excp("EDDI_NrtCleanupTxFrag1If, invalid pTopAdrDMACW->Tx", EDDI_FATAL_ERR_EXCP, pTopAdrDMACW->Tx, tmp);
            return;
        }

        //init pNRTTxIF -> dynamic pointer to the ring
        pIF->Tx.Dscr.pReq     = pIF->Tx.Dscr.pTop;
        pIF->Tx.Dscr.pReqDone = pIF->Tx.Dscr.pTop;
    }

    //cleanup EDDI TX SendQueues of one NRT interface
    EDDI_NrtCleanupTxSendQueues1If(pDDB, pIF, QueueIdx);

    //cleanup EDDI TX PreQueues of one NRT interface
    EDDI_NrtCleanupTxPreQueues1If(pDDB, pIF);

    //cleanup Frag Dealloc Queue of one NRT interface
    EDDI_NrtCleanupFragDeallocQueue1If(pDDB, pIF);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupTxDmacwQueue1If()                 */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup TX DMACW queue of one NRT interface      */
/*                        at Frag rescheduling                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxDmacwQueue1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                       LSA_UINT32                  const  QueueIdx )
{
    NRT_CHX_SSX_TYPE   *  const  pDscr    = &pIF->Tx.Dscr;
    NRT_DSCR_PTR_TYPE            pReqDone = pDscr->pReqDone;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtCleanupTxDmacwQueue1If->");

    //sendback all rqbs from DMACW-ring to user regardless of whether the owner of the DMACW is IRTE or EDDI
    for (; pIF->Tx.BalanceSndReq; pIF->Tx.BalanceSndReq--)
    {
        LSA_UINT32                                internal_context_1;
        EDDI_LOCAL_HDB_PTR_TYPE                   pHDB;
        EDD_UPPER_RQB_PTR_TYPE             const  pRQB   = pReqDone->pSndRQB;
        volatile  EDDI_SER_DMACW_PTR_TYPE  const  pDMACW = pReqDone->pDMACW;

        #if defined (EDDI_TX_FRAG_DEBUG_MODE)
        if //no more rqb available in DMACW-ring (= no rqb at all)?
           (pRQB == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_NrtCleanupTxDmacwQueue1If, no more rqb available in DMACW-ring!", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
        #endif

        //owner of the DMACW is IRTE or EDDI => set owner to EDDI
        pDMACW->L0.Reg = 0;

        //clean entry
        pReqDone->pSndRQB = EDDI_NULL_PTR;

        //switch to next entry (DMACW)
        pReqDone = pReqDone->next_dscr_ptr;

        internal_context_1 = pRQB->internal_context_1;

        if //FRAG RQB?
           (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
        {
            EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;

            //TX semaphore is always locked here

            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            if //stopping fragment-service running?
               (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping)
            {
                EDDI_Excp("EDDI_NrtCleanupTxDmacwQueue1If, stopping fragment-service running during Frag rescheduling!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
            #endif

            if //not last fragment exits DMACW queue?
               (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt != 1UL)
            {
                pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt--;
                //do not indicate RQB
                continue;
            }
            else //last fragment exits DMACW queue
            {
                LSA_UINT32         NextFragInterfaceNr;
                LSA_UINT32  const  PrioIndex = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!

                //deallocate big fragmentation buffer
                EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)(pRQB->internal_context_Prio), pIF->UserMemIDTXFragArray[PrioIndex]);

                if //releasing TX Frag semaphore not successful?
                   (!EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RELEASE, QueueIdx, &NextFragInterfaceNr))
                {
                    EDDI_Excp("EDDI_NrtCleanupTxDmacwQueue1If, releasing TX Frag semaphore not successful!", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return;
                }

                //indicate RQB
            }
        }
        //else: no FRAG RQB: indicate RQB

        pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);
        EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

        #if defined (EDDI_CFG_REV5)
        #error "REV5 Timestamp handling not implemented here in connection with TX fragmentation!"
        #endif

        if (!(0 == pHDB->Cbf))
        {
            //indicate RQB
            EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);

            pHDB->TxCount--;

            if (    (0 == pHDB->TxCount) 
                &&  (0 == pHDB->intRxRqbCount)
                &&  (0 == pHDB->intTxRqbCount)
                &&  (pHDB->pRQBTxCancelPending)  )
            {
                EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                pHDB->pRQBTxCancelPending = LSA_NULL;

                //Inform User thats all Frames are sent. The TxCancel can give back now!
                EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxDmacwQueue1If-> Cancel in Progress <---");
            }
            else if (pHDB->TxCount < 0)
            {
                EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
            }
        }
        else
        {
            EDDI_Excp("EDDI_NrtCleanupTxDmacwQueue1If, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
    } //end of for-loop
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupTxSendQueues1If()                 */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup EDDI TX SendQueues of one NRT interface  */
/*                        at Frag rescheduling                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxSendQueues1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                                       LSA_UINT32                  const  QueueIdx )
{
    LSA_UINT32  Ctr;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtCleanupTxSendQueues1If->");

    //EDDI SendQueues

    if //any RQB available in the SendQueues of this interface?
       (pIF->Tx.QueuedSndReq)
    {
        //check all SendQueues of this interface
        for (Ctr = 0; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++)
        {
            EDDI_RQB_QUEUE_TYPE  *  const  pSndQueue = &pIF->Tx.SndReq[Ctr];

            for (;;)
            {
                LSA_UINT32                      internal_context_1;
                EDDI_LOCAL_HDB_PTR_TYPE         pHDB;
                EDD_UPPER_RQB_PTR_TYPE   const  pRQB = pSndQueue->pTop;

                if //no further RQB available in this SendQueue?
                   (pRQB == EDDI_NULL_PTR)
                {
                    break; //leave inner for-loop
                }

                //dequeue RQB from SendQueue
                pSndQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);

                internal_context_1 = pRQB->internal_context_1;

                if //1st fragment waiting (no fragment of this service is currently in DMACW-ring)?
                   ((LSA_UINT8)internal_context_1 == 0) //FragIndexNext (aggressive optimization!)
                {
                    LSA_UINT32  const  PrioIndex = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!

                    //deallocate big fragmentation buffer
                    EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)(pRQB->internal_context_Prio), pIF->UserMemIDTXFragArray[PrioIndex]);

                    //indicate RQB
                }
                else if //2nd...last fragment waiting (at least 1 fragment of this service is/was already in DMACW-ring)?
                        (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
                {
                    LSA_UINT32                           NextFragInterfaceNr;
                    LSA_UINT32                    const  PrioIndex         = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!
                    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;

                    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
                    if //stopping fragment-service running?
                       (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping)
                    {
                        EDDI_Excp("EDDI_NrtCleanupTxSendQueues1If, stopping fragment-service running during Frag rescheduling!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }
                    #endif

                    //deallocate big fragmentation buffer
                    EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)(pRQB->internal_context_Prio), pIF->UserMemIDTXFragArray[PrioIndex]);

                    //update FragmentFrameIdLow
                    pNrtTxFragDataDDB->FragmentFrameIdLow = (pNrtTxFragDataDDB->FragmentFrameIdLow + (LSA_UINT8)1) & (LSA_UINT8)0x8F; //increment low nibble

                    if //releasing TX Frag semaphore not successful?
                       (!EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RELEASE, QueueIdx, &NextFragInterfaceNr))
                    {
                        EDDI_Excp("EDDI_NrtCleanupTxSendQueues1If, releasing TX Frag semaphore not successful!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }

                    //indicate RQB
                }
                //else: no FRAG RQB: indicate RQB

                pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

                EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);
                EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

                if (!(0 == pHDB->Cbf))
                {
                    //indicate RQB
                    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);

                    pHDB->TxCount--;

                    if (    (0 == pHDB->TxCount) 
                        &&  (0 == pHDB->intRxRqbCount)
                        &&  (0 == pHDB->intTxRqbCount)
                        &&  (pHDB->pRQBTxCancelPending)  )
                    {
                        EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                        pHDB->pRQBTxCancelPending = LSA_NULL;

                        //Inform User thats all Frames are sent. The TxCancel can give back now!
                        EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                        EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxSendQueues1If-> Cancel in Progress <---");
                    }
                    else if (pHDB->TxCount < 0)
                    {
                        EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
                    }
                }
                else
                {
                    EDDI_Excp("EDDI_NrtCleanupTxSendQueues1If, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return;
                }
            } //end of for-loop

            pSndQueue->Cnt = 0;
        } //end of for-loop

        pIF->Tx.QueuedSndReq = 0;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupTxPreQueues1If()                  */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup EDDI TX PreQueues of one NRT interface   */
/*                        at Frag rescheduling                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupTxPreQueues1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                      EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    LSA_UINT32  Ctr;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtCleanupTxPreQueues1If->");

    //EDDI PreQueues

    if //any RQB available in the PreQueues of this interface?
       (pIF->Tx.QueuedPreSndReq)
    {
        //check all PreQueues of this interface with the exception of MGMT_HIGH_PRIO
        for (Ctr = 1UL; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++) //0 = highest prio
        {
            EDDI_RQB_QUEUE_TYPE  *  const  pPreQueue = &pIF->Tx.PreSndReq[Ctr];

            for (;;)
            {
                EDDI_LOCAL_HDB_PTR_TYPE         pHDB;
                EDD_UPPER_RQB_PTR_TYPE   const  pRQB = pPreQueue->pTop;

                if //no further RQB available in this PreQueue?
                   (pRQB == EDDI_NULL_PTR)
                {
                    break; //leave inner for-loop
                }

                //dequeue RQB from PreQueue
                pPreQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);

                pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

                EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);
                EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

                if (!(0 == pHDB->Cbf))
                {
                    //indicate RQB
                    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);

                    pHDB->TxCount--;

                    if (    (0 == pHDB->TxCount) 
                        &&  (0 == pHDB->intRxRqbCount)
                        &&  (0 == pHDB->intTxRqbCount)
                        &&  (pHDB->pRQBTxCancelPending)  )
                    {
                        EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                        pHDB->pRQBTxCancelPending = LSA_NULL;

                        //Inform User thats all Frames are sent. The TxCancel can give back now!
                        EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                        EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxPreQueues1If-> Cancel in Progress <---");
                    }
                    else if (pHDB->TxCount < 0)
                    {
                        EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
                    }
                }
                else
                {
                    EDDI_Excp("EDDI_NrtCleanupTxPreQueues1If, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return;
                }
            } //end of for-loop

            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            pPreQueue->Cnt = 0;
            #endif
        } //end of for-loop

        pIF->Tx.QueuedPreSndReq = 0;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCleanupFragDeallocQueue1If()             */
/*                                                                         */
/* D e s c r i p t i o n: Cleanup Frag Dealloc Queue of one NRT interface  */
/*                        at Frag rescheduling                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCleanupFragDeallocQueue1If( EDDI_DDB_TYPE            *  const  pDDB,
                                                                           EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;
    EDD_UPPER_RQB_PTR_TYPE            pHelpRQB; //here big fragmentation buffers are handled as RQBs

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtCleanupFragDeallocQueue1If->");

    //Frag Dealloc Queue

    //check old big fragmentation buffers in Frag Dealloc Queue of this interface
    while ((pHelpRQB = EDDI_RemoveFromQueue(pDDB, pFragDeallocQueue)) != EDDI_NULL_PTR)
    {
        LSA_UINT32  const  PrioIndex = pHelpRQB->internal_context_Prio;

        //deallocate big fragmentation buffer
        EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)pHelpRQB, pIF->UserMemIDTXFragArray[PrioIndex]);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCalcFragments()                          */
/*                                                                         */
/* D e s c r i p t i o n: calculates all fragments of a NRT-Send           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCalcFragments( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                      EDD_UPPER_RQB_PTR_TYPE             const  pRQB,
                                                      EDD_UPPER_NRT_SEND_PTR_TYPE        const  pRQBSnd,
                                                      LSA_UINT32                         const  SendLength,
                                                      EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  *  const  pNrtTxFragPreData )
{
    LSA_UINT32                   FragmentNumber = 0;
    LSA_BOOL                     bVLAN;
    LSA_UINT32                   PDUHeaderSize;
    EDDI_TYPE_LEN_FRAME_ID_TYPE  FrameData;
    LSA_UINT32                   OrgPayloadLen;             //length of remaining payload of original frame
    LSA_UINT32                   FragPayloadLen;            //length of fragment payload
    LSA_UINT32                   MaxFragPayloadBufLen = 0;  //used for required fragment buffer length
    LSA_UINT32                   MinLastFragPayloadLen;     //minimal fragment payload length of last fragment (including leftovers)
    LSA_UINT32                   MaxLastFragPayloadLen;     //maximal fragment payload length of last fragment (including leftovers)
    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    LSA_UINT32                   MaxFragmentNumber;
    #endif

    //calculate VLAN tag
    {
        FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)(void *)pRQBSnd->pBuffer)[3]; //Dest and Src, start at offset 12

        /*-----------------------------------------------------------------------*/
        /* get bytes 12..15 from frame                                           */
        /* Note that this is in inet-format (big endian)                         */
        /*-----------------------------------------------------------------------*/

        //check VLAN tag
        if (FrameData.w.TypeLen == EDDI_VLAN_TAG)
        {
            bVLAN = LSA_TRUE;
        }
        else
        {
            bVLAN = LSA_FALSE;
        }
    }

    if //SendLengthMaxNoFrag == MAX_TX_FRAG_NETTO_SIZE_LOW?
       (pDDB->NRT.TxFragData.SendLengthMaxNoFrag == MAX_TX_FRAG_NETTO_SIZE_LOW)
    {
        //all values calculated for FragSize = 124 bytes
        if (bVLAN)
        {
            PDUHeaderSize         = 16UL;  //DA+SA+VLAN
            FragPayloadLen        = 96UL;  //PduSize = 122 bytes
            MinLastFragPayloadLen = 38UL;
            MaxLastFragPayloadLen = 98UL;  //PduSize = 124 bytes
            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            MaxFragmentNumber     = 16UL;
            #endif
        }
        else
        {
            PDUHeaderSize         = 12UL;  //DA+SA
            FragPayloadLen        = 96UL;  //PduSize = 118 bytes
            MinLastFragPayloadLen = 42UL;
            MaxLastFragPayloadLen = 102UL; //PduSize = 124 bytes
            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            MaxFragmentNumber     = 16UL;
            #endif
        }
    }
    else //SendLengthMaxNoFrag == MAX_TX_FRAG_NETTO_SIZE_HIGH
    {
        //all values calculated for FragSize = 244 bytes
        if (bVLAN)
        {
            PDUHeaderSize         = 16UL;  //DA+SA+VLAN
            FragPayloadLen        = 216UL; //PduSize = 242 bytes
            MinLastFragPayloadLen = 38UL;
            MaxLastFragPayloadLen = 218UL; //PduSize = 244 bytes
            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            MaxFragmentNumber     = 7UL;
            #endif
        }
        else
        {
            PDUHeaderSize         = 12UL;  //DA+SA
            FragPayloadLen        = 216UL; //PduSize = 238 bytes
            MinLastFragPayloadLen = 42UL;
            MaxLastFragPayloadLen = 222UL; //PduSize = 244 bytes
            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            MaxFragmentNumber     = 7UL;
            #endif
        }
    }

    pNrtTxFragPreData->PDUHeaderSize = PDUHeaderSize;

	OrgPayloadLen = SendLength - PDUHeaderSize; //SendLength without FCS!

    //build maximal fragments
    {
        LSA_UINT32  const  MinFragPayloadLen = FragPayloadLen + MinLastFragPayloadLen;
	    while (OrgPayloadLen >= MinFragPayloadLen)
	    {
            //fragment 1...(n-1/2) with Payload of size FragPayloadLen
            MaxFragPayloadBufLen = FragPayloadLen;
            pNrtTxFragPreData->FragPayloadLenArray[FragmentNumber] = (LSA_UINT8)FragPayloadLen;
	        FragmentNumber++;
		    OrgPayloadLen -= FragPayloadLen;
	    }
    }

    //check next-to-last fragment
	if (OrgPayloadLen > MaxLastFragPayloadLen)
	{
	    FragPayloadLen = (OrgPayloadLen - MinLastFragPayloadLen) & 0xFFFFFFF8UL; //round down to requested alignment
        //fragment n-1 with Payload of size FragPayloadLen
        if (FragPayloadLen > MaxFragPayloadBufLen)
        {
            MaxFragPayloadBufLen = FragPayloadLen;
        }
        pNrtTxFragPreData->FragPayloadLenArray[FragmentNumber] = (LSA_UINT8)FragPayloadLen;
	    FragmentNumber++;
	    OrgPayloadLen -= FragPayloadLen;
	}

    //build last fragment
	FragPayloadLen = OrgPayloadLen;
    //fragment n with Payload of size FragPayloadLen
    if (FragPayloadLen > MaxFragPayloadBufLen)
    {
        MaxFragPayloadBufLen = FragPayloadLen;
    }
    pNrtTxFragPreData->FragPayloadLenArray[FragmentNumber] = (LSA_UINT8)FragPayloadLen;
    FragmentNumber++;
    pNrtTxFragPreData->FragCnt = FragmentNumber;

    #if defined (EDDI_TX_FRAG_DEBUG_MODE)
    if (   (FragmentNumber < 2UL)
        || (FragmentNumber > MAX_TX_FRAG_NUMBER)
        || (FragmentNumber > MaxFragmentNumber))
    {
        EDDI_Excp("EDDI_NRTCalcFragments, Error in NRT TX fragment number!", EDDI_FATAL_ERR_EXCP, FragmentNumber, MaxFragmentNumber);
        return;
    }
    #endif

    {
        //calculate FragBufNettoSize without FCS!
        LSA_UINT32  const  FragBufNettoSize = (MaxFragPayloadBufLen + PDUHeaderSize + TX_FRAG_HEADER_SIZE + TX_FRAG_BUF_NETTO_SIZE_ALIGN) & (~TX_FRAG_BUF_NETTO_SIZE_ALIGN); //round up to requested alignment
        pNrtTxFragPreData->FragBufNettoSize = FragBufNettoSize;
        #if defined (EDDI_TX_FRAG_DEBUG_MODE)
        if //FragBufNettoSize not ok?
           (FragBufNettoSize > pDDB->NRT.TxFragData.SendLengthMaxNoFrag)
        {
            EDDI_Excp("EDDI_NRTCalcFragments, Error in NRT TX fragment buffer netto size!", EDDI_FATAL_ERR_EXCP, FragBufNettoSize, pDDB->NRT.TxFragData.SendLengthMaxNoFrag);
            return;
        }
        #endif
        pNrtTxFragPreData->BigFragBufTotalSize = (FragBufNettoSize + TX_FRAG_BUF_PRIV_SIZE) * FragmentNumber;
        //insert private data into FRAG-RQB
        pRQB->internal_context_1 = (  (pRQB->internal_context_1 & 0x0F000000UL) //do not touch PrioIndex!
                                    | (EDDI_TX_FRAG_RQB_MARK)
                                    | (((LSA_UINT32)FragmentNumber<<8) & 0xFF00UL) //FragCntTotal
                                    | (((LSA_UINT32)FragBufNettoSize<<16) & 0xFF0000UL)); //FragBufNettoSize
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTBuildFragments()                         */
/*                                                                         */
/* D e s c r i p t i o n: build all fragments of a NRT-Send                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTBuildFragments( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                       EDD_UPPER_RQB_PTR_TYPE             const  pRQB,
                                                       EDD_UPPER_NRT_SEND_PTR_TYPE        const  pRQBSnd,
                                                const  EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  *  const  pNrtTxFragPreData )
{
    LSA_UINT32            FragmentNumber;
    LSA_UINT8          *  pDstNRTFragBuffer           = pNrtTxFragPreData->pDstNRTFragBuffer; //pointer to big fragmentation buffer
    LSA_UINT32     const  PDUHeaderSize               = pNrtTxFragPreData->PDUHeaderSize;     //respects VLAN tag
    LSA_UINT32     const  FragHeaderSize              = PDUHeaderSize + TX_FRAG_HEADER_SIZE;  //complete header size of a fragment
    LSA_UINT32     const  PDUFragmentFrameIdLowOffset = PDUHeaderSize + 3UL;
    LSA_UINT32            CopyPayloadSrcDataOffset    = PDUHeaderSize;
    LSA_UINT8   *  const  pSrcNRTFragBuffer           = pRQBSnd->pBuffer; //pointer to user PDU Data buffer
    LSA_UINT32     const  FragBufNettoSize            = pNrtTxFragPreData->FragBufNettoSize;
    LSA_UINT32     const  FragCnt                     = pNrtTxFragPreData->FragCnt;
    LSA_UINT32            TmpNRTPrivFragBuffer        = 0;
    LSA_UINT8   *  const  pTmpNRTPrivFragBuffer       = (LSA_UINT8 *)(LSA_VOID *)&TmpNRTPrivFragBuffer;
    LSA_UINT32     const  NRTFragHeaderBuffer         = pDDB->NRT.TxFragData.NRTFragHeaderBuffer;

    //insert private data into FRAG-RQB
    pRQB->internal_context_Prio = (LSA_UINT32)pDstNRTFragBuffer; //pointer to big fragmentation buffer

    //prepare private data part 1
    pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_FRAMEID_LOW_OFFSET] = (LSA_UINT8)PDUFragmentFrameIdLowOffset;

    for (FragmentNumber = 0; FragmentNumber < FragCnt; FragmentNumber++)
    {
        LSA_UINT32     const  FragPayloadLen        = (LSA_UINT32)pNrtTxFragPreData->FragPayloadLenArray[FragmentNumber]; //payload length in bytes
        LSA_UINT32     const  FragSendLength        = FragPayloadLen + FragHeaderSize; //length in bytes without FCS
        LSA_UINT8   *  const  pDstNRTPrivFragBuffer = pDstNRTFragBuffer + FragBufNettoSize;

        //Copy DstMac, SrcMac and VLAN tag optional from user frame buffer
        EDDI_MEMCOPY(pDstNRTFragBuffer, pSrcNRTFragBuffer, (LSA_UINT)PDUHeaderSize);
        pDstNRTFragBuffer+=PDUHeaderSize;

        //Now fill the NRT Fragment Header
        //EtherType + FrameID (constant data of NRT Fragment Header are already prepared in a global buffer)
        //FragmentFrameIdLow is set later (consistency)!
        *((LSA_UINT32 *)(LSA_VOID *)pDstNRTFragBuffer) = NRTFragHeaderBuffer;
        pDstNRTFragBuffer+=4UL;
        //FragDataLength
        *pDstNRTFragBuffer = (LSA_UINT8)(FragPayloadLen / 8UL);
        pDstNRTFragBuffer++;

        if //not last fragment?
           ((FragCnt - 1UL) != FragmentNumber)
        {
            //FragStatus
            *pDstNRTFragBuffer = (LSA_UINT8)FragmentNumber /*Bit0..5*/ | (LSA_UINT8)0x80; /*Bit7*/
        }
        else //last fragment
        {
            //FragStatus
            *pDstNRTFragBuffer = (LSA_UINT8)FragmentNumber; /*Bit0..5*/
        }
        pDstNRTFragBuffer++;

        //copy PayLoad Data
        EDDI_MEMCOPY(pDstNRTFragBuffer, (pSrcNRTFragBuffer + CopyPayloadSrcDataOffset), (LSA_UINT)FragPayloadLen);

        //insert private data into each FragBuffer
        {
            //calculate necessary DBs for this fragment
            LSA_UINT32  const  FrameLenInDBs = (FragSendLength + (EDDI_NRT_DB_LEN - 1)) / EDDI_NRT_DB_LEN;

            //prepare private data part 2
            pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN_IN_DBS] = (LSA_UINT8)FrameLenInDBs;

            pTmpNRTPrivFragBuffer[TX_FRAG_BUF_PRIV_OFFS_SEND_LEN]        = (LSA_UINT8)FragSendLength;

            //copy all private data from buffer to target FragBuffer
            *((LSA_UINT32 *)(LSA_VOID *)pDstNRTPrivFragBuffer) = TmpNRTPrivFragBuffer;
        }

        CopyPayloadSrcDataOffset += FragPayloadLen;
        pDstNRTFragBuffer         = pDstNRTPrivFragBuffer + TX_FRAG_BUF_PRIV_SIZE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTTxFragSemaphore()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     InterfaceNr is not relevant with                 */
/*                        SemaphoreCommand == EDDI_TX_FRAG_SEMA_RESET.     */
/*                        pNextFragInterfaceNr is only relevant with       */
/*                        SemaphoreCommand == EDDI_TX_FRAG_SEMA_RELEASE.   */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTxFragSemaphore( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                        LSA_UINT32                  const  SemaphoreCommand,
                                                        LSA_UINT32                  const  InterfaceNr,
                                                        LSA_UINT32               *  const  pNextFragInterfaceNr ) //OUT
{
    EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;

    switch (SemaphoreCommand)
    {
        case EDDI_TX_FRAG_SEMA_CLAIM:
        {
            if (InterfaceNr >= EDDI_NRT_IF_CNT)
            {
                EDDI_Excp("EDDI_NRTTxFragSemaphore, Error in NRT TX fragment InterfaceNr, EDDI_NRT_IF_CNT!", EDDI_FATAL_ERR_EXCP, InterfaceNr, 0);
                return LSA_FALSE;
            }
            EDDI_ENTER_CRITICAL_S();
            if //Semaphore is free?
               (pNrtTxFragDataDDB->SemaphoreState == EDDI_TX_FRAG_SEMA_FREE)
            {
                //Semaphore round-robin scheduler optimized / A0, B0
                if (   (!pNrtTxFragDataDDB->bNewClaimRequest[(InterfaceNr + 1UL) & 1UL]) //the other semaphore is not waiting?
                    || (pNrtTxFragDataDDB->SemaphoreCurrentInterfaceNr != InterfaceNr))  //the last claimed semaphore was another?
                {
                    //lock Semaphore
                    pNrtTxFragDataDDB->SemaphoreState                = EDDI_TX_FRAG_SEMA_LOCKED;
                    pNrtTxFragDataDDB->SemaphoreCurrentInterfaceNr   = InterfaceNr;
                    pNrtTxFragDataDDB->bNewClaimRequest[InterfaceNr] = LSA_FALSE;

                    pNrtTxFragDataDDB->SemaphoreClaimRefCnt++;

                    EDDI_EXIT_CRITICAL_S();
                    return LSA_TRUE;
                }
                else //the other semaphore is waiting and the last claimed semaphore was the new semaphore
                {
                    //store new claim request
                    pNrtTxFragDataDDB->bNewClaimRequest[InterfaceNr] = LSA_TRUE;

                    EDDI_EXIT_CRITICAL_S();
                    return LSA_FALSE;
                }
            }
            else //Semaphore is locked
            {
                //store new claim request
                pNrtTxFragDataDDB->bNewClaimRequest[InterfaceNr] = LSA_TRUE;

                EDDI_EXIT_CRITICAL_S();
                return LSA_FALSE;
            }
        }
        case EDDI_TX_FRAG_SEMA_RELEASE:
        {
            if (InterfaceNr >= EDDI_NRT_IF_CNT)
            {
                EDDI_Excp("EDDI_NRTTxFragSemaphore, Error in NRT TX fragment InterfaceNr, EDDI_NRT_IF_CNT!", EDDI_FATAL_ERR_EXCP, InterfaceNr, 0);
                return LSA_FALSE;
            }
            EDDI_ENTER_CRITICAL_S();
            if (   (pNrtTxFragDataDDB->SemaphoreState              == EDDI_TX_FRAG_SEMA_LOCKED)
                && (pNrtTxFragDataDDB->SemaphoreCurrentInterfaceNr == InterfaceNr))
            {
                LSA_UINT32  const  NextInterfaceNr = (InterfaceNr + 1UL) & 1UL;

                //release Semaphore
                pNrtTxFragDataDDB->SemaphoreState = EDDI_TX_FRAG_SEMA_FREE;

                //search for next claim request of Semaphore round-robin scheduler on another NRT interface
                //Semaphore round-robin scheduler optimized / A0, B0
                if (pNrtTxFragDataDDB->bNewClaimRequest[NextInterfaceNr])
                {
                    *pNextFragInterfaceNr = NextInterfaceNr;
                }
                else
                {
                    *pNextFragInterfaceNr = EDDI_NRT_NO_IF;
                }

                EDDI_EXIT_CRITICAL_S();
                return LSA_TRUE;
            }
            else
            {
                EDDI_EXIT_CRITICAL_S();
                return LSA_FALSE;
            }
        }
        case EDDI_TX_FRAG_SEMA_RESET:
        {
            pNrtTxFragDataDDB->bNewClaimRequest[EDDI_NRT_CHA_IF_0] = LSA_FALSE;
            pNrtTxFragDataDDB->bNewClaimRequest[EDDI_NRT_CHB_IF_0] = LSA_FALSE;
            return LSA_TRUE; //always return TRUE!
        }
        case EDDI_TX_FRAG_SEMA_CLEAN:
        {
            LSA_UINT32  const  CurrSemaphoreClaimRefCnt = pNrtTxFragDataDDB->SemaphoreClaimRefCnt;

            if //SemaphoreClaimRefCnt not changed?
               (pNrtTxFragDataDDB->LastSemaphoreClaimRefCnt == CurrSemaphoreClaimRefCnt)
            {
                if //Semaphore is free?
                   (pNrtTxFragDataDDB->SemaphoreState == EDDI_TX_FRAG_SEMA_FREE)
                {
                    pNrtTxFragDataDDB->bNewClaimRequest[EDDI_NRT_CHA_IF_0] = LSA_FALSE;
                    pNrtTxFragDataDDB->bNewClaimRequest[EDDI_NRT_CHB_IF_0] = LSA_FALSE;
                }
            }
            else //SemaphoreClaimRefCnt changed
            {
                pNrtTxFragDataDDB->LastSemaphoreClaimRefCnt = CurrSemaphoreClaimRefCnt;
            }
            return LSA_TRUE; //always return TRUE!
        }
        default:
        {
            EDDI_Excp("EDDI_NRTTxFragSemaphore, invalid SemaphoreCommand!", EDDI_FATAL_ERR_EXCP, SemaphoreCommand, 0);
            return LSA_FALSE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_FRAG_ON


/*****************************************************************************/
/*  end of file eddi_nrt_frag_tx.c                                           */
/*****************************************************************************/
