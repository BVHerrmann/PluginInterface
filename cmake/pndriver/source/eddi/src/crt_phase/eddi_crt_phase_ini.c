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
/*  F i l e               &F: eddi_crt_phase_ini.c                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
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

#include "eddi_crt_phase_ext.h"

#include "eddi_ser_ext.h"
#include "eddi_lock.h"

#define EDDI_MODULE_ID     M_ID_CRT_PHASE_INI
#define LTRC_ACT_MODUL_ID  118

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseClass2Init( EDDI_RTCLASS2_TYPE  *  const  pClass2,
                                                        EDDI_DDB_TYPE       *  const  pDDB )
{
    LSA_UINT32  Size;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseClass2Init->");

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ROOT-ACW is always needed for iSRT-Bandwith (even if ProviderCount == 0)
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for special Root-ACW
    Size = sizeof(EDDI_SER10_ACW_SND_TYPE);

    pClass2->pRootAcw = &pDDB->pKramFixMem->RootAcw;

    pDDB->KramRes.pool.srt_eol_and_root_acw += Size; //see also pRootAcw

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // EOL is always needed for iSRT-Bandwith (even if ProviderCount == 0)
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for EOL-Entries
    Size = sizeof(EDDI_SER10_EOL_TYPE);

    pClass2->pEOL                           = &pDDB->pKramFixMem->RootEol;
    pDDB->KramRes.pool.srt_eol_and_root_acw = Size;  // see also pEOLArray !!

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // iSRT-Interval-End does not change automatically --> set ISRT_TIME_VALUE must be 0!
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    IO_W32(ISRT_TIME_VALUE, 0);

    //Init RootAcw

    EDDI_SERIniRootAcw(pClass2->pRootAcw, (LSA_UINT8 *)(void *)pClass2->pRootAcw, pDDB);

    EDDI_SERIniEOL(pDDB, pClass2->pEOL);

    EDDI_SERConnectCwToNext(pDDB,
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pClass2->pRootAcw,
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pClass2->pEOL);

    //Finally connect SerAcwHeader to first Entry in KRAM-Tree
    //But this will be done in Device-Setup-Call
    //with function EDDI_SERConnectCwToHeader(pDDB, pDDB->Glob.LLHandle.pACWDevBase->pTx, pTree->pRootElem->pCW);
}
/*---------------------- end [subroutine] ---------------------------------*/



/***************************************************************************/
/* F u n c t i o n:       EDDI_CrtPhaseReset()                             */
/*                                                                         */
/* D e s c r i p t i o n: resets the Crt-Phase-SM on all connected ports   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseReset( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseReset->");

    EDDI_ENTER_SYNC_S();

    //check all connected ports
    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                    const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine  = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
        EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine  = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];

        //finish a TX-red-shifting-sequence (SOL running)
        EDDI_CrtPhaseTxTriggerFinish(pTxMachine, pDDB);
        //switch off and unshift TX
        EDDI_CrtPhaseTxTrigger(pTxMachine, EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT, pDDB);
        //finish a RX-red-shifting-sequence (SOL running)
        EDDI_CrtPhaseRxTriggerFinish(pRxMachine, pDDB);
        //switch off and unshift RX
        EDDI_CrtPhaseRxTrigger(pRxMachine, EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT, pDDB);
    }

    //check all connected ports
    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32  const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

        //reset rtc3-SM
        //Attention: OFF-Indication??
        //pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex].RtClass3_OutputState = EDDI_RT_CLASS3_STATE_OFF;

        //finish a TX-red-shifting-sequence (SOL running)
        EDDI_CrtPhaseTxTriggerFinish(&pDDB->pLocal_CRT->PhaseTx[HwPortIndex], pDDB);
        //finish a RX-red-shifting-sequence (SOL running)
        EDDI_CrtPhaseRxTriggerFinish(&pDDB->pLocal_CRT->PhaseRx[HwPortIndex], pDDB);
    }

    EDDI_EXIT_SYNC_S();
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
* F u n c t i o n:       EDDI_CrtPhaseAreSolsCompleted()
*
* D e s c r i p t i o n: Global function for all CRTPhase-STMS
                         to check the activity of the 8 SOLs used to
*                        change the value of the 8 IRTStartTimeRegisters
*
* A r g u m e n t s:
*
* Return Value:          Returns LSA_FALSE if one of the 8 IRTStartTimeRegisters
*                        has not yet reached the target value.
*                        Otherwise returns LSA_TRUE.
*
***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreSolsCompleted( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseAreSolsCompleted->");

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                    const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine  = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
        EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine  = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];

        if (   ((pTxMachine->bSOLRunning) && (IO_R32(pTxMachine->IrtStartTimeSndRegAdr) != pTxMachine->StartTime10Ns))
            || ((pRxMachine->bSOLRunning) && (IO_R32(pRxMachine->IrtStartTimeRcvRegAdr) != pRxMachine->StartTime10Ns)))
        {
            return LSA_FALSE;
        }
    }

    //no active SOL found
    return LSA_TRUE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
* F u n c t i o n:       EDDI_CrtPhaseAreDisablingFCWsRunning()
*
* D e s c r i p t i o n: Global function for all CRTPhase-STMS
                         to check the activity of disabling FCWs
*
* A r g u m e n t s:
*
* Return Value:          Returns LSA_TRUE if one of the 8 CrtPhase-STMs
*                        is shutting down the FCW-lists (disabling FCWs).
*                        Otherwise returns LSA_FALSE.
*
***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreDisablingFCWsRunning( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseAreDisablingFCWsRunning->");

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                    const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine  = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
        EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine  = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];

        if (pTxMachine->bDisablingFCWs || pRxMachine->bDisablingFCWs)
        {
            return LSA_TRUE;
        }
    }

    //no disabling FCWs found
    return LSA_FALSE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
* F u n c t i o n:       EDDI_CrtPhaseAreTransitionsRunning()
*
* D e s c r i p t i o n: Global function for all CRTPhase-STMS
                         to check the transaction-state
*
* A r g u m e n t s:
*
* Return Value:          Returns LSA_TRUE if one of the 8 CrtPhase-STMs
*                        has a running SOL-Transition or is shutting down the
*                        FCW-lists.
*                        Otherwise returns LSA_FALSE.
*
***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreTransitionsRunning( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseAreTransitionsRunning->");

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                    const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine  = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
        EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine  = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];

        if (   (pTxMachine->bSOLRunning    || pRxMachine->bSOLRunning)
            || (pTxMachine->bDisablingFCWs || pRxMachine->bDisablingFCWs))
        {
            if (pTxMachine->bSOLRunning || pRxMachine->bSOLRunning)
            {
                EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                       "EDDI_CrtPhaseAreTransitionsRunning, bSOLRunning:0x%X bSOLRunning:0x%X",
                                       pTxMachine->bSOLRunning, pRxMachine->bSOLRunning);
            }

            if (pTxMachine->bDisablingFCWs || pRxMachine->bDisablingFCWs)
            {
                EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                       "EDDI_CrtPhaseAreTransitionsRunning, bDisablingFCWs:0x%X bDisablingFCWs:0x%X",
                                       pTxMachine->bDisablingFCWs, pRxMachine->bDisablingFCWs);
            }

            return LSA_TRUE;
        }
    }

    //no active transition found
    return LSA_FALSE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
* F u n c t i o n:       EDDI_CrtPhaseIsTransitionRunning()
*
* D e s c r i p t i o n: Port - specific function for both directions
*                        (RX and TX) of a CRTPhase-STMS
*                        to check the transaction-state
*
* A r g u m e n t s:
*
* Return Value:          Returns TRUE if one of the 2 CrtPhase-STMs
*                        has a running SOL-Transition or is shutting down the
*                        FCW-lists.
*                        Otherwise returns FALSE.
*
***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseIsTransitionRunning( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex )
{
    EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
    EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseIsTransitionRunning->HwPortIndex:0x%X", HwPortIndex);

    if (   (pTxMachine->bSOLRunning    || pRxMachine->bSOLRunning)
        || (pTxMachine->bDisablingFCWs || pRxMachine->bDisablingFCWs))
    {
        return LSA_TRUE;
    }
    else
    {
        //no active transition found
        return LSA_FALSE;
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_phase_ini.c                                         */
/*****************************************************************************/

