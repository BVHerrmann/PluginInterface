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
/*  F i l e               &F: eddi_crt_phase_rx.c                       :F&  */
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
#include "eddi_crt_com.h"

#define EDDI_MODULE_ID     M_ID_CRT_PHASE_RX
#define LTRC_ACT_MODUL_ID  117

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

//Attention: bSOLRunning is only modified in this c-module!

//Typedef of FunctionPtr
typedef  LSA_VOID  (EDDI_LOCAL_FCT_ATTR  *  EDDI_CRT_PHASE_RX_FCT_TYPE)( EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                                         EDDI_CRT_PHASE_RX_EVENT     const Event,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxStsNo( EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                          EDDI_CRT_PHASE_RX_EVENT     const Event,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxStsIRTSHIFTED( EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                                  EDDI_CRT_PHASE_RX_EVENT     const Event,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxTrace( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                             EDDI_CRT_PHASE_RX_MACHINE  const * const  pMachine,
                                                             EDDI_CRT_PHASE_RX_EVENT            const  Event,
                                                             EDDI_CRT_PHASE_RX_STATE            const  OldState );

//ATTENTION: This FunctionTable is related to enum EDDI_CRT_PHASE_RX_STATE !!
static  EDDI_CRT_PHASE_RX_FCT_TYPE  CrtPhaseRxFctTable[] =
{
    EDDI_CrtPhaseRxStsNo,
    EDDI_CrtPhaseRxStsIRTSHIFTED
};


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxInit()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                  EDDI_CRT_PHASE_RX_MACHINE * const  pMachine,
                                                  LSA_UINT32                  const  HwPortIndex )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxInit->State:0x%X HwPortIndex:0x%X", pMachine->State, HwPortIndex);

    pMachine->State          = EDDI_CRT_PHASE_RX_STS_NO;
    pMachine->HwPortIndex    = HwPortIndex;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    pMachine->StartTime10Ns  = 0;
    pMachine->bSOLRunning    = LSA_FALSE;
    pMachine->bDisablingFCWs = LSA_FALSE;
    #endif
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxTrigger()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxTrigger( EDDI_CRT_PHASE_RX_MACHINE  *  const  pMachine,
                                                       EDDI_CRT_PHASE_RX_EVENT       const  Event,
                                                       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    EDDI_CRT_PHASE_RX_STATE  const  OldState = pMachine->State;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseRxTrigger->State:0x%X Event:0x%X", OldState, Event);

    CrtPhaseRxFctTable[OldState](pMachine, Event, pDDB);

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if //old state differs from new state OR SOL is running OR DisablingFCWs is running
    (   (OldState != pMachine->State)
     || (pMachine->bSOLRunning)
     || (pMachine->bDisablingFCWs))
    #else
    if //old state differs from new state
       (OldState != pMachine->State)
    #endif
    {
        EDDI_CrtPhaseRxTrace(pDDB, pMachine, Event, OldState);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxStsNo()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxStsNo( EDDI_CRT_PHASE_RX_MACHINE  *  const  pMachine,
                                                             EDDI_CRT_PHASE_RX_EVENT       const  Event,
                                                             EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    //Start-Time-Register always 0 here!

    if (pMachine->State != EDDI_CRT_PHASE_RX_STS_NO)
    {
        EDDI_Excp("EDDI_CrtPhaseRxStsNo, ERROR wrong State", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
        return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            LSA_UINT32                   const  StartTime10Ns = pMachine->RxFcwSolTimeMac10Ns;

            if (StartTime10Ns)
            {
                EDDI_SERSetIrtPortStartTimeRx(pDDB, pMachine, StartTime10Ns);
            }
            else
            {
                //insert Productive-List into List-Header
                *pMachine->pFcwListHead = pMachine->pRootCwAsic;  //regular: EDDI_SERConnectCwToHeader()
            }
            #endif

            // 2) IRT_Snd_En_Port = 1
            EDDI_SERSetIrtPortActivityRx(HwPortIndex, LSA_TRUE, pDDB);

            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (StartTime10Ns)
            {
                pMachine->FinishEvent = EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT_FINISH;
                pMachine->bSOLRunning = LSA_TRUE;
                //finishing of SOL will be checked later
                //pMachine->State will be changed later on FINISH-Event
            }
            else
            {
                pMachine->State = EDDI_CRT_PHASE_RX_STS_IRT;
            }
            #else
            pMachine->State = EDDI_CRT_PHASE_RX_STS_IRT;
            #endif
        }
        break;

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT_FINISH:
        {
            pMachine->bSOLRunning = LSA_FALSE;

            //remove LMW from List
            *pMachine->pFcwListHead = pMachine->pRootCwAsic;  //regular: EDDI_SERConnectCwToHeader()

            pMachine->State = EDDI_CRT_PHASE_RX_STS_IRT;
        }
        break;
        #endif

        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_CrtPhaseRxStsNo, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxStsIRTSHIFTED()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxStsIRTSHIFTED( EDDI_CRT_PHASE_RX_MACHINE  *  const  pMachine,
                                                                     EDDI_CRT_PHASE_RX_EVENT       const  Event,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    if (pMachine->State != EDDI_CRT_PHASE_RX_STS_IRT)
    {
        EDDI_Excp("EDDI_CrtPhaseRxStsIRTSHIFTED, ERROR wrong State", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
        return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pMachine->StartTime10Ns)
            {
                // 1) RX-Offset = 0
                EDDI_SERSetIrtPortStartTimeRx(pDDB, pMachine, 0);
                pMachine->FinishEvent = EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
                pMachine->bSOLRunning = LSA_TRUE;
                //finishing of SOL will be checked later
                //pMachine->State will be changed later on FINISH-Event
            }
            else
            {
                //No unshifting needed but we have to disable FCW-List
                EDDI_SERSetIrtPortActivityRx(HwPortIndex, LSA_FALSE, pDDB);
                pMachine->FinishEvent    = EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
                pMachine->bDisablingFCWs = LSA_TRUE;
                //pMachine->State will be changed later on FINISH-Event
            }
            #else
            //No unshifting needed but we have to disable FCW-List
            EDDI_SERSetIrtPortActivityRx(HwPortIndex, LSA_FALSE, pDDB);
            pMachine->FinishEvent    = EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
            pMachine->bDisablingFCWs = LSA_TRUE;
            //pMachine->State will be changed later on FINISH-Event
            #endif
        }
        break;

        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pMachine->bSOLRunning)
            {
                pMachine->bSOLRunning = LSA_FALSE;
                EDDI_SERSetIrtPortActivityRx(HwPortIndex, LSA_FALSE, pDDB);

                //No additional wait state needed here because FCW List has been removed at least one cycle ago.

                if (pMachine->bDisablingFCWs)
                {
                    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CrtPhaseRxStsIRTSHIFTED, bDisablingFCWs=1 AND bSOLRunning=1.");
                    pMachine->bDisablingFCWs = LSA_FALSE;
                }
            }
            else if (pMachine->bDisablingFCWs)
            {
                //IRT_CONTROL-Bit has been already switched off at least one cycle ago.
                pMachine->bDisablingFCWs = LSA_FALSE;
            }
            else
            {
                EDDI_Excp("EDDI_CrtPhaseRxStsIRTSHIFTED, ERROR neither bSOLRunning nor bDisablingFCWs are set", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
                return;
            }
            #else
            //IRT_CONTROL-Bit has been already switched off at least one cycle ago.
            pMachine->bDisablingFCWs = LSA_FALSE;
            #endif

            pMachine->State = EDDI_CRT_PHASE_RX_STS_NO;
        }
        break;

        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_CrtPhaseRxStsIRTSHIFTED, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxTriggerFinish()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxTriggerFinish( EDDI_CRT_PHASE_RX_MACHINE  *  const  pMachine,
                                                             EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseRxTriggerFinish->pMachine:0x%X bSOLRunning:0x%X bDisablingFCWs:0x%X",
                           (LSA_UINT32)pMachine, (LSA_UINT32)pMachine->bSOLRunning, (LSA_UINT32)pMachine->bDisablingFCWs);

    if (pMachine->bSOLRunning)
    {
        if //new StartTime not reached yet?
           (IO_R32(pMachine->IrtStartTimeRcvRegAdr) != pMachine->StartTime10Ns)
        {
            volatile  LSA_UINT32  WaitTime;
            LSA_UINT32  const     StartTime   = IO_R32(CLK_COUNT_VALUE);
            #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
            //Revision 5/6 counts in 10ns - Steps
            LSA_UINT32  const     MaxWaitTime = 20UL * 1000UL * 100UL;  //Timeout=20ms
            #elif defined (EDDI_CFG_REV7)
            //Revision 7 counts in 1ns - Steps
            LSA_UINT32  const     MaxWaitTime = 20UL * 1000UL * 1000UL; //Timeout=20ms
            #endif

            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTriggerFinish, Although in async-mode CrtPhaseRx has not finished yet. HwPortIndex:0x%X", pMachine->HwPortIndex);
            #endif

            //await new modified StartTime in IRTE-Register
            while (IO_R32(pMachine->IrtStartTimeRcvRegAdr) != pMachine->StartTime10Ns)
            {
                //Wait to reduce PCI-traffic (polling)
                EDDI_WAIT_10_NS(pDDB->hSysDev, 50UL); //500ns

                WaitTime = IO_R32(CLK_COUNT_VALUE) - StartTime;
                if (WaitTime > MaxWaitTime)
                {
                    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CrtPhaseRxTriggerFinish, HwPortIndex:0x%X", pMachine->HwPortIndex);
                    EDDI_Excp("EDDI_CrtPhaseRxTriggerFinish, Transition in CrtPhaseRx could not be finished", EDDI_FATAL_ERR_EXCP, StartTime, WaitTime);
                    return;
                }
            }
        }

        EDDI_CrtPhaseRxTrigger(pMachine, pMachine->FinishEvent, pDDB);
    }
    else if (pMachine->bDisablingFCWs)
    {
        EDDI_CrtPhaseRxTrigger(pMachine, pMachine->FinishEvent, pDDB);
    }
    else
    {
        //nothing todo
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseRxTrace()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseRxTrace( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_CRT_PHASE_RX_MACHINE  const *  const  pMachine,
                                                             EDDI_CRT_PHASE_RX_EVENT             const  Event,
                                                             EDDI_CRT_PHASE_RX_STATE             const  OldState )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    switch (OldState)
    {
        case EDDI_CRT_PHASE_RX_STS_NO:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ####  from   STS_NO                              #### HwPortIndex:0x%X OldState:0x%X", HwPortIndex, OldState);
            break;
        case EDDI_CRT_PHASE_RX_STS_IRT:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ####  from   STS_IRT                             #### HwPortIndex:0x%X OldState:0x%X", HwPortIndex, OldState);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseRxTrace, wrong old State. HwPortIndex: OldState:", EDDI_FATAL_ERR_EXCP, HwPortIndex, OldState);
            return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ##    with   EVT_IRT_ACTIVATE_SHIFT              ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ##    with   EVT_IRT_DEACTIVATE_UNSHIFT          ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_RX_EVT_IRT_ACTIVATE_SHIFT_FINISH:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ##    with   EVT_IRT_ACTIVATE_SHIFT_FINISH       ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_RX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ##    with   EVT_IRT_DEACTIVATE_UNSHIFT_FINISH   ## Event:0x%X", Event);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseRxTrace, wrong Event. HwPortIndex: Event:", EDDI_FATAL_ERR_EXCP, HwPortIndex, Event);
            return;
    }

    switch (pMachine->State)
    {
        case EDDI_CRT_PHASE_RX_STS_NO:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ####  to     STS_NO                              #### bSOLRunning:0x%X bDisablingFCWs:0x%X", pMachine->bSOLRunning, pMachine->bDisablingFCWs);
            break;
        case EDDI_CRT_PHASE_RX_STS_IRT:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseRxTrace   ####  to     STS_IRT                             #### bSOLRunning:0x%X bDisablingFCWs:0x%X", pMachine->bSOLRunning, pMachine->bDisablingFCWs);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseRxTrace, wrong new State. HwPortIndex: State:", EDDI_FATAL_ERR_EXCP, HwPortIndex, pMachine->State);
            return;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_phase_rx.c                                          */
/*****************************************************************************/

