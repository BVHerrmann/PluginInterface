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
/*  F i l e               &F: eddi_irt_class3_stm.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
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
#include "eddi_ext.h"
#include "eddi_crt_phase_ext.h"
#include "eddi_crt_com.h"

#define EDDI_MODULE_ID     M_ID_IRT_CLASS3_STM
#define LTRC_ACT_MODUL_ID  122

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#include "eddi_sync_usr.h"
#endif

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3StmsTrace( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                                EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine,
                                                                LSA_UINT32                                 const  OldRtClass3_OutputState,
                                                                LSA_UINT32                                 const  NewRtClass3_OutputState );

static LSA_UINT8  EDDI_LOCAL_FCT_ATTR   EDDI_IrtClass3CalcIrtPortStateFromOutput( LSA_UINT32  const  RtClass3_OutputState,
                                                                                  LSA_UINT32  const  IrtPortUsrCnt );


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass3StmsInit()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3StmsInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex )
{
    EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex];

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IrtClass3StmsInit->");

    //init rt-class3-output-state
    pRtClass3_Machine->RtClass3_OutputState = EDDI_RT_CLASS3_STATE_OFF;
    pRtClass3_Machine->bFinishNecessary     = LSA_FALSE;
    pRtClass3_Machine->bHandleAsyncLinkDown = LSA_FALSE;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "####### EDDI_IrtClass3StmsInit<-bFinishNecessary = FALSE");
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
* F u n c t i o n:         EDDI_IrtClass3CalcOutputFromInput()
*
* D e s c r i p t i o n:   The central logic table for the IRTClass3-PortStm:
*                          portOutput = f ( portInput, globalInput )
*
* A r g u m e n t s:
*
* R e t u r n   V a l u e:
*
**************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3CalcOutputFromInput( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                            EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData,
                                                                            LSA_BOOL                                const  isLocalSyncOk,
                                                                            LSA_BOOL                                const  DomainBoundary)
{
    LSA_UINT32  NewRtClass3_OutputState;

    if (   ( isLocalSyncOk)
        && (!pPortInputData->doRtClass3Reset)
        && (!pPortInputData->FreezeResetMask)
        && ( pPortInputData->isMauTypeOk)
        && ( pPortInputData->isPortStateOk)
        && ( EDD_SET_REMOTE_PORT_STATE_TOPO_OK == pPortInputData->RtClass3_TopoState)
        #if !defined (EDDI_CFG_UP_ON_REMOTE_TOPO_MISMATCH)
        && ( EDD_SET_REMOTE_PORT_STATE_TOPO_OK == pPortInputData->RtClass3_PDEVTopoState)
        #endif
        && (!DomainBoundary)) /* no UP-State possible with boundary set! */

    {
        #if defined (EDDI_CFG_UP_ON_REMOTE_TOPO_MISMATCH)
        if (EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH == pPortInputData->RtClass3_PDEVTopoState)
        {
            //Engineering still wrong ==> set local port to UP (TX) anyway
            //OFF --> UP
            NewRtClass3_OutputState = EDDI_RT_CLASS3_STATE_TX;    

            if (   (pPortInputData->RtClass3_RemoteState != EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF)
                && (pPortInputData->RtClass3_RemoteState != EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX))
            {
                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_IrtClass3CalcOutputFromInput, RtClass3_RemoteState:0x%X RtClass3_TopoState:0x%X", 
		                           pPortInputData->RtClass3_RemoteState, pPortInputData->RtClass3_TopoState);
            }
        }
        else
        #endif
        {
            //Engineering (RtClass3_PDEVTopoState) is OK or IGNORE ==> continue with UP/RUN

            switch //by remote rt-class3-state
                   (pPortInputData->RtClass3_RemoteState)
            {
                case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF:
                {
                    //OFF --> UP
                    NewRtClass3_OutputState = EDDI_RT_CLASS3_STATE_TX;
                    break;
                }
                case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX:
                #if defined (EDDI_RED_PHASE_SHIFT_ON)
                {
                    //UP --> RUN
                    NewRtClass3_OutputState = EDDI_RT_CLASS3_STATE_RXsTX;
                    break;
                }
                #endif
                case EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX:
                {
                    //RUN --> RUN_SHIFTED
                    NewRtClass3_OutputState = EDDI_RT_CLASS3_STATE_RXsTXs;
                    break;
                }
                default:
                {
                    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_IrtClass3CalcOutputFromInput, RtClass3_RemoteState:0x%X RtClass3_PDEVTopoState:0x%X", 
                                       pPortInputData->RtClass3_RemoteState, pPortInputData->RtClass3_PDEVTopoState);
                    EDDI_Excp("EDDI_IrtClass3CalcOutputFromInput, pPortInputData->RtClass3_RemoteState == invalid", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return 0;
                }
            }
        }
    }
    else
    {
        NewRtClass3_OutputState = EDDI_RT_CLASS3_STATE_OFF;
    }

    LSA_UNUSED_ARG(pDDB);
    return NewRtClass3_OutputState;
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
* F u n c t i o n:         EDDI_IrtClass3CalcIrtPortStateFromOutput()
*
* D e s c r i p t i o n:   Conversion from internal RtClass3_OutputState
*                          to IrtPortState as used in ExtLinkIndication
* A r g u m e n t s:
*
* R e t u r n   V a l u e:
*
**************************************************************************/
static  LSA_UINT8  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3CalcIrtPortStateFromOutput( LSA_UINT32  const  RtClass3_OutputState,
                                                                                  LSA_UINT32  const  IrtPortUsrCnt )
{
    LSA_UINT8  LinkIndIrtPortState;

    switch //by rt-class3-output-state
           (RtClass3_OutputState)
    {
        case EDDI_RT_CLASS3_STATE_OFF:
        {
            if (IrtPortUsrCnt)
            {
                LinkIndIrtPortState = EDD_IRT_PORT_INACTIVE;
            }
            else
            {
                LinkIndIrtPortState = EDD_IRT_NOT_SUPPORTED;
            }
            break;
        }

        case EDDI_RT_CLASS3_STATE_TX:
        {
            LinkIndIrtPortState = EDD_IRT_PORT_ACTIVE_TX_UP;
            break;
        }

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_RT_CLASS3_STATE_RXsTX:
        #endif
        case EDDI_RT_CLASS3_STATE_RXsTXs:
        {
            LinkIndIrtPortState = EDD_IRT_PORT_ACTIVE_TX_RX;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_IrtClass3CalcIrtPortStateFromOutput", EDDI_FATAL_ERR_EXCP, RtClass3_OutputState, 0);
            return 0;
        }
    }

    return LinkIndIrtPortState;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass3StmsTrigger()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3StmsTrigger( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bOldSolTransitionRunning )
{
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE        const  pPortInputData          = &pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex];
    EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE     const  pRtClass3_Machine       = &pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex];
    EDDI_IRT_META_DATA_TYPE                    *  const  pIrtMetaData            = &pDDB->PRM.PDIRData.pRecordSet_A->MetaData;
    LSA_UINT32                                    const  OldRtClass3_OutputState = pRtClass3_Machine->RtClass3_OutputState;
    LSA_UINT32                                           NewRtClass3_OutputState;
    LSA_BOOL                                             bAny2RUN = LSA_FALSE;
    LSA_BOOL                                             bRUN2Any = LSA_FALSE;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger -> HwPortIndex:0x%X OldRtClass3_OutputState:0x%X", 
                           HwPortIndex, OldRtClass3_OutputState);

    //process new rt-class3-output-state according state-machine
    NewRtClass3_OutputState = EDDI_IrtClass3CalcOutputFromInput(pDDB, pPortInputData,
                                                                pDDB->SYNC.PortMachines.Input.isLocalSyncOk,
                                                                (((pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex]==0) && (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex]==0))?LSA_TRUE:LSA_FALSE));  /* Domainboundary */

    //handling of reset-flag
    if (NewRtClass3_OutputState == EDDI_RT_CLASS3_STATE_OFF)
    {
        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        if (   (pPortInputData->doRtClass3Reset)
            && (!pPortInputData->FreezeResetMask)
            && (!bOldSolTransitionRunning)
            && (OldRtClass3_OutputState == EDDI_RT_CLASS3_STATE_OFF))
        {
            //Reset State already reached
            //--> Acknowledge of reset can be done here

            pPortInputData->doRtClass3Reset = LSA_FALSE;

            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger, reset Flag removed without action. HwPortIndex:0x%X OldRtClass3_OutputState:0x%X",
                               HwPortIndex, OldRtClass3_OutputState);
        }
        else
        {
            //Acknowledge of reset will be done later
        }
        #else
        //Acknowledge of reset is done here
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger, reset Flag removed without action. HwPortIndex:0x%X OldRtClass3_OutputState:0x%X",
                           HwPortIndex, OldRtClass3_OutputState);

        pPortInputData->doRtClass3Reset = LSA_FALSE;

        LSA_UNUSED_ARG(bOldSolTransitionRunning); //satisfy lint!
        #endif
    }

    if //rt-class3-output-state did not change?
       (NewRtClass3_OutputState == OldRtClass3_OutputState)
    {
        //nothing to do
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger, NewRtClass3_OutputState == OldRtClass3_OutputState --> no action. HwPortIndex:0x%X NewRtClass3_OutputState:0x%X",
                           HwPortIndex, NewRtClass3_OutputState);
    }
    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    else if //rt-class3-output-state changed, but: a transition is still running and NO special handling for LinkDown is needed.
            (bOldSolTransitionRunning && (!pRtClass3_Machine->bHandleAsyncLinkDown))
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger, old SOL-Transition still running --> no action. HwPortIndex:0x%X NewRtClass3_OutputState:0x%X",
                           HwPortIndex, NewRtClass3_OutputState);
    }
    #endif
    else //rt-class3-output-state changed OR special handling for LinkDown is needed.
    {
        //This block is also called in case of a preceding EDDI_SwiPhyActionForLinkDown(),
        //as the TxPortStateMachine is reset without informing the IRT_CLASS_3Machine.
        //All skipped actions due to this procedure have to be cought up now. 

        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsTrigger, HwPortIndex:0x%X NewRtClass3_OutputState:0x%X", HwPortIndex, NewRtClass3_OutputState);

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        if (pRtClass3_Machine->bHandleAsyncLinkDown)
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "####### EDDI_IrtClass3StmsTrigger, bHandleAsyncLinkDown = TRUE (handle a ActionForLinkDown)");
        }
        #endif

        if (pRtClass3_Machine->bFinishNecessary)
        {
            //Reset Class3-SM asynchronously (this also resets bFinishNecessary)
            EDDI_IrtClass3StmsFinish(pDDB, pRtClass3_Machine, HwPortIndex);
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "####### EDDI_IrtClass3StmsTrigger, asynchronous finishing of RTC3-SM. HWPortIndex:0x%X State:0x%X", HwPortIndex, NewRtClass3_OutputState);
            return;
        }

        switch //by old rt-class3-output-state
               (OldRtClass3_OutputState)
        {
            case EDDI_RT_CLASS3_STATE_OFF:  //old rt-class3-output-state
            {
                switch //by new rt-class3-output-state
                       (NewRtClass3_OutputState)
                {
                    case EDDI_RT_CLASS3_STATE_TX:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&pDDB->pLocal_CRT->PhaseTx[HwPortIndex],
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE,
                                                   pDDB);
                        }
                        break;
                    }

                    #if defined (EDDI_RED_PHASE_SHIFT_ON)
                    case EDDI_RT_CLASS3_STATE_RXsTX:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&pDDB->pLocal_CRT->PhaseTx[HwPortIndex],
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE,
                                                   pDDB);
                        }

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT,
                                                   pDDB);
                        }
    
                        bAny2RUN = LSA_TRUE;
                        break;
                    }
                    #endif

                    case EDDI_RT_CLASS3_STATE_RXsTXs:  //not possible with EDDI_RED_PHASE_SHIFT_ON!
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&pDDB->pLocal_CRT->PhaseTx[HwPortIndex],
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE,
                                                   pDDB);

                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_SHIFT,
                                                   pDDB);
                        }

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT,
                                                   pDDB);
                        }

                        bAny2RUN = LSA_TRUE;
                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_OFF:
                    default:
                    {
                        EDDI_Excp("EDDI_IrtClass3StmsTrigger, new RtClass3_OutputState == invalid", EDDI_FATAL_ERR_EXCP, NewRtClass3_OutputState, 0);
                        return;
                    }
                }

                break;
            }

            case EDDI_RT_CLASS3_STATE_TX:  //old rt-class3-output-state
            {
                switch //by new rt-class3-output-state
                       (NewRtClass3_OutputState)
                {
                    case EDDI_RT_CLASS3_STATE_OFF:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }
                        break;
                    }

                    #if defined (EDDI_RED_PHASE_SHIFT_ON)
                    case EDDI_RT_CLASS3_STATE_RXsTX:
                    {
                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT,
                                                   pDDB);
                        }
                        bAny2RUN = LSA_TRUE;
                        break;
                    }
                    #endif

                    case EDDI_RT_CLASS3_STATE_RXsTXs:  //not possible with EDDI_RED_PHASE_SHIFT_ON!
                    {
                        #if defined (EDDI_RED_PHASE_SHIFT_ON)
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_SHIFT,
                                                   pDDB);
                        }
                        #endif

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT,
                                                   pDDB);
                        }
                        bAny2RUN = LSA_TRUE;
                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_TX:
                    default:
                    {
                        EDDI_Excp("EDDI_IrtClass3StmsTrigger, new RtClass3_OutputState == invalid", EDDI_FATAL_ERR_EXCP, NewRtClass3_OutputState, 0);
                        return;
                    }
                }

                break;
            }

            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            case EDDI_RT_CLASS3_STATE_RXsTX:  //old rt-class3-output-state
            {
                switch //by new rt-class3-output-state
                       (NewRtClass3_OutputState)
                {
                    case EDDI_RT_CLASS3_STATE_OFF:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }

                        bRUN2Any = LSA_TRUE;
                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_TX:
                    {
                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }

                        bRUN2Any = LSA_TRUE;
                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_RXsTXs:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_SHIFT,
                                                   pDDB);
                        }

                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_RXsTX:
                    default:
                    {
                        EDDI_Excp("EDDI_IrtClass3StmsTrigger, new RtClass3_OutputState == invalid", EDDI_FATAL_ERR_EXCP, NewRtClass3_OutputState, 0);
                        return;
                    }
                }

                break;
            }
            #endif

            case EDDI_RT_CLASS3_STATE_RXsTXs:  //old rt-class3-output-state
            {
                switch //by new rt-class3-output-state
                       (NewRtClass3_OutputState)
                {
                    case EDDI_RT_CLASS3_STATE_OFF:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }

                        bRUN2Any = LSA_TRUE;
                        break;
                    }

                    case EDDI_RT_CLASS3_STATE_TX:
                    {
                        #if defined (EDDI_RED_PHASE_SHIFT_ON)
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_UNSHIFT,
                                                   pDDB);
                        }
                        #endif

                        if (pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex])
                        {
                            EDDI_CrtPhaseRxTrigger(&(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT,
                                                   pDDB);
                        }
                        bRUN2Any = LSA_TRUE;
                        break;
                    }

                    #if defined (EDDI_RED_PHASE_SHIFT_ON)
                    case EDDI_RT_CLASS3_STATE_RXsTX:
                    {
                        if (pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] && (!pRtClass3_Machine->bHandleAsyncLinkDown))
                        {
                            EDDI_CrtPhaseTxTrigger(&(pDDB->pLocal_CRT->PhaseTx[HwPortIndex]),
                                                   EDDI_CRT_PHASE_TX_EVT_UNSHIFT,
                                                   pDDB);
                        }
                        break;
                    }
                    #endif

                    case EDDI_RT_CLASS3_STATE_RXsTXs:
                    default:
                    {
                        EDDI_Excp("EDDI_IrtClass3StmsTrigger, new RtClass3_OutputState == invalid", EDDI_FATAL_ERR_EXCP, NewRtClass3_OutputState, 0);
                        return;
                    }
                }

                break;
            }
            default:
            {
                EDDI_Excp("EDDI_IrtClass3StmsTrigger, old RtClass3_OutputState == invalid", EDDI_FATAL_ERR_EXCP, OldRtClass3_OutputState, 0);
                return;
            }
        }

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        if (EDDI_CrtPhaseIsTransitionRunning(pDDB, HwPortIndex))
        {
            // A transition was started.
            // --> Finishing will be done later in NewCycleContext
            EDDI_SyncPortStmsTransitionBegin(pDDB);
        }
        #endif

        //update rt-class3-output-state
        pRtClass3_Machine->RtClass3_OutputState = NewRtClass3_OutputState;

        pRtClass3_Machine->bHandleAsyncLinkDown = LSA_FALSE;

        pRtClass3_Machine->bFinishNecessary = LSA_TRUE;
    }

    if (OldRtClass3_OutputState != NewRtClass3_OutputState)
    {
        EDDI_IrtClass3StmsTrace(pDDB, pRtClass3_Machine, OldRtClass3_OutputState, NewRtClass3_OutputState);
    }

    if (pRtClass3_Machine->bFinishNecessary)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "####### EDDI_IrtClass3StmsTrigger, bFinishNecessary = TRUE");
    }
    
    //check for changes for the class3-providers/forwarders/forwarding-consumers
    if (bAny2RUN)
    {
        //enable TX for all forwarders on this port
        EDDI_CRTSetActivityAllFWD (pDDB, HwPortIndex, LSA_TRUE /*bActivate*/);
    }
    else if (bRUN2Any)
    {
        //disable TX for all forwarders on this port
        EDDI_CRTSetActivityAllFWD (pDDB, HwPortIndex, LSA_FALSE /*bActivate*/);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass3StmsFinish()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3StmsFinish( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                         EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine,
                                                         LSA_UINT32                                 const  HwPortIndex )
{
    LSA_UINT8                          LinkIndIrtPortState;
    LSA_UINT32                  const  RtClass3_OutputState = pRtClass3_Machine->RtClass3_OutputState;
    EDDI_IRT_META_DATA_TYPE  *  const  pIrtMetaData         = &pDDB->PRM.PDIRData.pRecordSet_A->MetaData;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsFinish->");

    if (!pRtClass3_Machine->bFinishNecessary)
    {
        EDDI_Excp("EDDI_IrtClass3StmsFinish, FinishNecessary == FALSE", EDDI_FATAL_ERR_EXCP, RtClass3_OutputState, 0);
        return;
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_CrtPhaseTxTriggerFinish(&pDDB->pLocal_CRT->PhaseTx[HwPortIndex], pDDB);
    EDDI_CrtPhaseRxTriggerFinish(&pDDB->pLocal_CRT->PhaseRx[HwPortIndex], pDDB);

    if (   (RtClass3_OutputState == EDDI_RT_CLASS3_STATE_OFF)
        && (pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex].doRtClass3Reset))
    {
        //Ok, this port has reached its ResetState
        //Now check if reset state has to be frozen.
        if (pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex].FreezeResetMask)
        {
            //Frozen reset state due to SendClockChange-Transition or PRM_COMMIT-Transition
            //No Change of doRtClass3Reset-State here.
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsFinish, Reset-State reached and frozen. HwPortIndex:0x%X", HwPortIndex);
        }
        else
        {
            //No Freeze of Reset state required, we now can clear the ResetFlag
            pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex].doRtClass3Reset = LSA_FALSE;
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass3StmsFinish, Reset-State reached. HwPortIndex:0x%X", HwPortIndex);
        }
    }
    #endif

    LinkIndIrtPortState = EDDI_IrtClass3CalcIrtPortStateFromOutput(RtClass3_OutputState,
                                                                   pIrtMetaData->IrtPortUsrTxCnt[HwPortIndex] + pIrtMetaData->IrtPortUsrRxCnt[HwPortIndex]);

    EDDI_GenSetIRTPortStatus(LSA_FALSE, HwPortIndex, LinkIndIrtPortState, pDDB);

    pRtClass3_Machine->bFinishNecessary = LSA_FALSE;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "####### EDDI_IrtClass3StmsFinish<-bFinishNecessary = FALSE");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass3StmsTrace()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass3StmsTrace( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                                EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine,
                                                                LSA_UINT32                                 const  OldRtClass3_OutputState,
                                                                LSA_UINT32                                 const  NewRtClass3_OutputState )
{
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LTRC_LEVEL_TYPE  tracelevel = LSA_TRACE_LEVEL_NOTE_HIGH;

    if  //leaving productive level?
        (   ((EDDI_RT_CLASS3_STATE_RXsTX == OldRtClass3_OutputState) || (EDDI_RT_CLASS3_STATE_RXsTXs == OldRtClass3_OutputState))
         && ((EDDI_RT_CLASS3_STATE_OFF   == NewRtClass3_OutputState) || (EDDI_RT_CLASS3_STATE_TX     == NewRtClass3_OutputState)))
    {
        tracelevel = LSA_TRACE_LEVEL_WARN;
    }
    #endif

    switch (OldRtClass3_OutputState)
    {
        case EDDI_RT_CLASS3_STATE_OFF:
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  from   OFF     ####");
            break;
        }
        case EDDI_RT_CLASS3_STATE_TX:
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  from   TX      ####");
            break;
        }
        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_RT_CLASS3_STATE_RXsTX:
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  from   RXsTX   ####");
            break;
        }
        #endif
        case EDDI_RT_CLASS3_STATE_RXsTXs:
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  from   RXsTXs  ####");
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_IrtClass3StmsTrace", EDDI_FATAL_ERR_EXCP, pRtClass3_Machine->RtClass3_OutputState, 0);
            return;
        }
    }

    switch (NewRtClass3_OutputState)
    {
        case EDDI_RT_CLASS3_STATE_OFF:
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  to     OFF     ####  bFinishNecessary:0x%X", pRtClass3_Machine->bFinishNecessary);
            break;
        }
        case EDDI_RT_CLASS3_STATE_TX:
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  to     TX      ####  bFinishNecessary:0x%X", pRtClass3_Machine->bFinishNecessary);
            break;
        }
        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_RT_CLASS3_STATE_RXsTX:
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  to     RXsTX   ####  bFinishNecessary:0x%X", pRtClass3_Machine->bFinishNecessary);
            break;
        }
        #endif
        case EDDI_RT_CLASS3_STATE_RXsTXs:
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, tracelevel, "EDDI_IrtClass3StmsTrace, ####  to     RXsTXs  ####  bFinishNecessary:0x%X", pRtClass3_Machine->bFinishNecessary);
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_IrtClass3StmsTrace", EDDI_FATAL_ERR_EXCP, pRtClass3_Machine->RtClass3_OutputState, 0);
            return;
        }
    }
    LSA_UNUSED_ARG(pDDB);
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LSA_UNUSED_ARG(tracelevel);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_irt_class3_stm.c                                        */
/*****************************************************************************/
