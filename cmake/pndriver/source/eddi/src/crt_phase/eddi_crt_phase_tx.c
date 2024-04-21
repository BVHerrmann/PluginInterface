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
/*  F i l e               &F: eddi_crt_phase_tx.c                       :F&  */
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
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_CRT_PHASE_TX
#define LTRC_ACT_MODUL_ID  116

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

//Attention: bSOLRunning is only modified in this c-module!

//Typedef of FunctionPtr
typedef  LSA_VOID  (EDDI_LOCAL_FCT_ATTR  *  EDDI_CRT_PHASE_TX_FCT_TYPE)( EDDI_CRT_PHASE_TX_MACHINE  *  const pMachine,
                                                                         EDDI_CRT_PHASE_TX_EVENT       const Event,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE       const pDDB );

//Functions of SM (statemachine)
static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxStsNo           ( EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                                       EDDI_CRT_PHASE_TX_EVENT     const Event,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxStsIRT          ( EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                                       EDDI_CRT_PHASE_TX_EVENT     const Event,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxStsIRT_SHIFTED  ( EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                                       EDDI_CRT_PHASE_TX_EVENT     const Event,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxTrace( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                             EDDI_CRT_PHASE_TX_MACHINE  const * const  pMachine,
                                                             EDDI_CRT_PHASE_TX_EVENT            const  Event,
                                                             EDDI_CRT_PHASE_TX_STATE            const  OldState );

//ATTENTION: This FunctionTable is related to enum EDDI_CRT_PHASE_TX_STATE!!
static  EDDI_CRT_PHASE_TX_FCT_TYPE  CrtPhaseTxFctTable[] =
{
    EDDI_CrtPhaseTxStsNo,
    EDDI_CrtPhaseTxStsIRT,
    EDDI_CrtPhaseTxStsIRT_SHIFTED
};


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxInit()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxInit( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                    EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                    LSA_UINT32                    const  HwPortIndex )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxInit->State:0x%X HwPortIndex:0x%X", pMachine->State, HwPortIndex);

    pMachine->State          = EDDI_CRT_PHASE_TX_STS_NO;
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
 *  Function:    EDDI_CrtPhaseTxTrigger()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxTrigger( EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                       EDDI_CRT_PHASE_TX_EVENT       const  Event,
                                                       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    EDDI_CRT_PHASE_TX_STATE  const  OldState = pMachine->State;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseTxTrigger->State:0x%X Event:0x%X", OldState, Event);

    CrtPhaseTxFctTable[OldState](pMachine, Event, pDDB);

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if //old state differs from new state OR SOL is running OR DisablingFCWs is running?
       (   (OldState != pMachine->State)
        || (pMachine->bSOLRunning)
        || (pMachine->bDisablingFCWs))
    #else
    if //old state differs from new state
       (OldState != pMachine->State)
    #endif
    {
        EDDI_CrtPhaseTxTrace(pDDB, pMachine, Event, OldState);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxStsNo()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxStsNo( EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                             EDDI_CRT_PHASE_TX_EVENT       const  Event,
                                                             EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    //Start-Time-Register always 0 here!

    if (pMachine->State != EDDI_CRT_PHASE_TX_STS_NO)
    {
        EDDI_Excp("EDDI_CrtPhaseTxStsNo, ERROR wrong State", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
        return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE:
        {
            // 2) FCW-EOLs already set in IR-RecordWrite
            //    --> nothing to do here

            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            //insert Productive-List into List-Header
            *pMachine->pFcwListHead = pMachine->pRootCwAsic;  //regular: EDDI_SERConnectCwToHeader()
            #endif

            // 4) IRT_Snd_En_Port = 1"
            EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_TRUE, pDDB);

            // IRT
            pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT;
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_SHIFT:
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        case EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH:
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_CrtPhaseTxStsNo, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxStsIRT()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxStsIRT( EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                              EDDI_CRT_PHASE_TX_EVENT       const  Event,
                                                              EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    if (pMachine->State != EDDI_CRT_PHASE_TX_STS_IRT)
    {
        EDDI_Excp("EDDI_CrtPhaseTxStsIRT, ERROR wrong State", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
        return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT:
        {
            // 1) IRT_Snd_En_Port = 0
            EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_FALSE, pDDB);

            pMachine->FinishEvent    = EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
            pMachine->bDisablingFCWs = LSA_TRUE;

            // 2) FCW-EOL will be removed later (in IR_RECORD_REMOVE)
            //    --> nothing to do here

            //pMachine->State will be changed later on FINISH-Event
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            {
                if (pMachine->bSOLRunning)
                {
                    //A SOL-Running must not occur here!
                    EDDI_Excp("EDDI_CrtPhaseTxStsIRT, ERROR bSOLRunning == TRUE, Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
                    return;
                }

                if (!pMachine->bDisablingFCWs)
                {
                    //bDisablingFCWs must be set here!
                    EDDI_Excp("EDDI_CrtPhaseTxStsIRT, ERROR bDisablingFCWs != TRUE, Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
                    return;
                }

                pMachine->bDisablingFCWs = LSA_FALSE;
            }
            #endif

            pMachine->State = EDDI_CRT_PHASE_TX_STS_NO;
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_SHIFT:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            LSA_UINT32  const  StartTime10Ns = pMachine->TxFcwSolTimeMac10Ns;

            if (StartTime10Ns)
            {
                EDDI_SERSetIrtPortStartTimeTx(pDDB, pMachine, Event, StartTime10Ns);
                pMachine->FinishEvent = EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH;
                pMachine->bSOLRunning = LSA_TRUE;
                //finishing of SOL will be checked later
                //pMachine->State will be changed later on FINISH-Event
            }
            else
            {
                pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED;
            }
            #else
            pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED;
            #endif
        }
        break;

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH:
        {
            pMachine->bSOLRunning = LSA_FALSE;

            //remove LMW from List
            *pMachine->pFcwListHead = pMachine->pRootCwAsic;  //regular: EDDI_SERConnectCwToHeader()

            pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED;
        }
        break;
        #endif

        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_CrtPhaseTxStsIRT, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxStsIRT_SHIFTED()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxStsIRT_SHIFTED( EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                                      EDDI_CRT_PHASE_TX_EVENT       const  Event,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    if (pMachine->State != EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED)
    {
        EDDI_Excp("EDDI_CrtPhaseTxStsIRT_SHIFTED, ERROR wrong State", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
        return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pMachine->StartTime10Ns)
            {
                // 1) TX-Offset = 0
                EDDI_SERSetIrtPortStartTimeTx(pDDB, pMachine, Event, 0);
                pMachine->FinishEvent = EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
                pMachine->bSOLRunning = LSA_TRUE;
                //finishing of SOL will be checked later
                //pMachine->State will be changed later on FINISH-Event
            }
            else
            {
                //No unshifting needed but we have to disable FCW-List

                // 1) IRT_Snd_En_Port = 0
                EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_FALSE, pDDB);
                pMachine->FinishEvent    = EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
                pMachine->bDisablingFCWs = LSA_TRUE;
                //pMachine->State will be changed later on FINISH-Event
            }
            #else
            // 1) IRT_Snd_En_Port = 0
            EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_FALSE, pDDB);
            pMachine->FinishEvent    = EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH;
            pMachine->bDisablingFCWs = LSA_TRUE;
            #endif
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pMachine->bSOLRunning)
            {
                pMachine->bSOLRunning = LSA_FALSE;
                EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_FALSE, pDDB);
                //No additional wait state needed here because FCW List has been removed at least one cycle ago.

                if (pMachine->bDisablingFCWs)
                {
                    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CrtPhaseTxStsIRT_SHIFTED, bDisablingFCWs=1 AND bSOLRunning=1.");
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
                EDDI_Excp("EDDI_CrtPhaseTxStsIRT_SHIFTED, ERROR neither bSOLRunning nor bDisablingFCWs are set", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
                return;
            }
            #else
            //IRT_CONTROL-Bit has been already switched off at least one cycle ago.
            pMachine->bDisablingFCWs = LSA_FALSE;
            #endif

            // 3) FCW-EOL will be removed later (in IR_RECORD_REMOVE)
            //    --> nothing to do here

            pMachine->State = EDDI_CRT_PHASE_TX_STS_NO;
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_SHIFT:
            //ignore
            break;

        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT:
        {
            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pMachine->StartTime10Ns)
            {
                // 1) TX-Offset = 0
                EDDI_SERSetIrtPortStartTimeTx(pDDB, pMachine, Event, 0);
                pMachine->FinishEvent = EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH;
                pMachine->bSOLRunning = LSA_TRUE;
                //finishing of SOL will be checked later
                //pMachine->State will be changed later on FINISH-Event
            }
            else
            {
                pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT;
            }
            #else
            pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT;
            #endif
        }
        break;

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH:
        {
            pMachine->bSOLRunning = LSA_FALSE;

            //remove LMW from List
            *pMachine->pFcwListHead = pMachine->pRootCwAsic;  //regular: EDDI_SERConnectCwToHeader()

            pMachine->State = EDDI_CRT_PHASE_TX_STS_IRT;
        }
        break;
        #endif

        case EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_CrtPhaseTxStsIRT_SHIFTED, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxTriggerFinish()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxTriggerFinish( EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                             EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CrtPhaseTxTriggerFinish->pMachine:0x%X bSOLRunning:0x%X bDisablingFCWs:0x%X",
                           (LSA_UINT32)pMachine, (LSA_UINT32)pMachine->bSOLRunning, (LSA_UINT32)pMachine->bDisablingFCWs);

    if (pMachine->bSOLRunning)
    {
        if //new StartTime not reached yet?
           (IO_R32(pMachine->IrtStartTimeSndRegAdr) != pMachine->StartTime10Ns)
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

            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTriggerFinish, Although in async-mode CrtPhaseTx has not finished yet. HwPortIndex:0x%X", pMachine->HwPortIndex);

            //await new modified StartTime in IRTE-Register
            while (IO_R32(pMachine->IrtStartTimeSndRegAdr) != pMachine->StartTime10Ns)
            {
                //Wait to reduce PCI-traffic (polling)
                EDDI_WAIT_10_NS(pDDB->hSysDev, 50UL); //500ns

                WaitTime = IO_R32(CLK_COUNT_VALUE) - StartTime;
                if (WaitTime > MaxWaitTime)
                {
                    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CrtPhaseTxTriggerFinish, HwPortIndex:0x%X", pMachine->HwPortIndex);
                    EDDI_Excp("EDDI_CrtPhaseTxTriggerFinish, Transition in CrtPhaseTx could not be finished", EDDI_FATAL_ERR_EXCP, StartTime, WaitTime);
                    return;
                }
            }
        }

        EDDI_CrtPhaseTxTrigger(pMachine, pMachine->FinishEvent, pDDB);
    }
    else if (pMachine->bDisablingFCWs)
    {
        EDDI_CrtPhaseTxTrigger(pMachine, pMachine->FinishEvent, pDDB);
    }
    else
    {
        //nothing todo
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtPhaseTxTrace()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseTxTrace( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_CRT_PHASE_TX_MACHINE  const *  const  pMachine,
                                                             EDDI_CRT_PHASE_TX_EVENT             const  Event,
                                                             EDDI_CRT_PHASE_TX_STATE             const  OldState )
{
    LSA_UINT32  const  HwPortIndex = pMachine->HwPortIndex;

    switch (OldState)
    {
        case EDDI_CRT_PHASE_TX_STS_NO:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  from   STS_NO                              #### HwPortIndex:0x%X OldState:0x%X", HwPortIndex, OldState);
            break;
        case EDDI_CRT_PHASE_TX_STS_IRT:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  from   STS_IRT                             #### HwPortIndex:0x%X OldState:0x%X", HwPortIndex, OldState);
            break;
        case EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  from   STS_IRT_SHIFTED                     #### HwPortIndex:0x%X OldState:0x%X", HwPortIndex, OldState);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseTxTrace, wrong old State. HwPortIndex: OldState:", EDDI_FATAL_ERR_EXCP, HwPortIndex, OldState);
            return;
    }

    switch (Event)
    {
        case EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_IRT_ACTIVATE                    ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_IRT_DEACTIVATE_UNSHIFT          ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_SHIFT:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_SHIFT                           ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_UNSHIFT                         ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_IRT_DEACTIVATE_UNSHIFT_FINISH   ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_SHIFT_FINISH                    ## Event:0x%X", Event);
            break;
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH:
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ##    with   EVT_UNSHIFT_FINISH                  ## Event:0x%X", Event);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseTxTrace, wrong Event. HwPortIndex: Event:", EDDI_FATAL_ERR_EXCP, HwPortIndex, Event);
            return;
    }

    switch (pMachine->State)
    {
        case EDDI_CRT_PHASE_TX_STS_NO:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  to     STS_NO                              #### bSOLRunning:0x%X bDisablingFCWs:0x%X", pMachine->bSOLRunning, pMachine->bDisablingFCWs);
            break;
        case EDDI_CRT_PHASE_TX_STS_IRT:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  to     STS_IRT                             #### bSOLRunning:0x%X bDisablingFCWs:0x%X", pMachine->bSOLRunning, pMachine->bDisablingFCWs);
            break;
        case EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED:
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CrtPhaseTxTrace   ####  to     STS_IRT_SHIFTED                     #### bSOLRunning:0x%X bDisablingFCWs:0x%X", pMachine->bSOLRunning, pMachine->bDisablingFCWs);
            break;
        default:
            EDDI_Excp("EDDI_CrtPhaseTxTrace, wrong new State. HwPortIndex: State:", EDDI_FATAL_ERR_EXCP, HwPortIndex, pMachine->State);
            return;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_phase_tx.c                                          */
/*****************************************************************************/

