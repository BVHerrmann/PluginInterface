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
/*  F i l e               &F: eddi_isr_rest.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-interrupthandler                            */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_EDDI_ISR_REST
#define LTRC_ACT_MODUL_ID  19

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

#include "eddi_isr_rest.h"
#include "eddi_ext.h"
#include "eddi_ser_cmd.h"
#include "eddi_crt_ext.h"
#include "eddi_trac_ext.h"
//#include "eddi_swi_ext.h"

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV7)
#include "eddi_crt_xch.h"
#endif

#include "eddi_lock.h"

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#include "eddi_sync_usr.h"
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_NewCycleReduced()                           */
/*                                                                         */
/* D e s c r i p t i o n: function for reduced new-cycle                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NewCycleReduced( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  para_1 )
{
    EDDI_DDB_INT_STAT_TYPE  *  const  pIntStat = &pDDB->IntStat;

    LSA_UNUSED_ARG(para_1); //satisfy lint!

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NewCycleReduced->");

	//acknowledge IRTE-irt-interrupt "IRQ_CRT_SND_IRQ1" for reduced new-cycle. do not change without checking EDDI_SIIInterruptWork() => IRQ_CRT_SND_IRQ1!
    EDDI_SII_IO_x32(pDDB->SII.IAR_IRT_Adr) = IRQ_CRT_SND_IRQ1;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++   SYNC
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (pDDB->SYNC.bSyncInRedActive)
    {
        EDDI_ENTER_SYNC_S();
        EDDI_SyncSndNewCycleCbf(pDDB);
        EDDI_SyncRcvNewCycleCbf(pDDB);
        EDDI_EXIT_SYNC_S();
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON) //todo attention, also needed for bDisablingFCWs !
    if (pDDB->SYNC.PortMachines.Transition.bTransitionRunning)
    {
        EDDI_ENTER_SYNC_S();
        EDDI_SyncPortStmsTransitionEndCbf(pDDB);
        EDDI_EXIT_SYNC_S();
    }
    #endif

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++   REST
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    EDDI_ENTER_REST_S();
    {
        pDDB->ClkCounter_in_10ns_ticks_NewCycle = IO_GET_CLK_COUNT_VALUE_10NS;

        #if defined (EDDI_CFG_INTERRUPT_STATISTIC)
        pIntStat->cycleLostRingBuffer[pIntStat->actBufferElem++ % EDDI_MAX_CNT_CYCLE_LOST_ARRAY] = pDDB->ClkCounter_in_10ns_ticks_NewCycle;
        #endif

        if (pDDB->CycCount.State != EDDI_CYC_COUNTER_STATE_RUNNING)
        {
            EDDI_CycStateMachine(pDDB, &pDDB->CycCount, EDDI_CYC_COUNTER_EVENT_NEWCYCLE);
        }

        #if defined (EDDI_CFG_USE_SW_RPS)
        pDDB->SW_RPS_counter++;
        if (pDDB->SW_RPS_counter >= pDDB->SW_RPS_internal_reduction)
        {
            pDDB->SW_RPS_CheckDataStatesEvent(pDDB, &pDDB->CRT.Rps);
            pDDB->SW_RPS_counter = 0;
        }
        #endif //EDDI_CFG_USE_SW_RPS
        //call Provider-Xchange-Buffer-Handling
        #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV7)
        if (pDDB->CRT.XChangeHandling.RQBQueue.Count)
        {
            EDDI_CRTTriggerBufferXchange(pDDB);
        }
        #endif

        #if defined (EDDI_CFG_ERTEC_200)
        pDDB->pLocal_SWITCH->PhyBugs.EDDI_SwiPhyBugTimerFct(pDDB);
        #endif
    }
    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTInstructionDone()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTInstructionDone( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  para_1 )
{
    LSA_UNUSED_ARG(para_1); //satisfy lint!

    EDDI_ENTER_REST_S();

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTInstructionDone->");

    EDDI_SERAsyncConf(pDDB);

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTTCWDone()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTTCWDone( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                LSA_UINT32               const  para_1 )
{
    LSA_UNUSED_ARG(para_1); //satisfy lint!

    EDDI_ENTER_REST_S();

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTTCWDone->");

    (void)EDDI_TRACETrigger(pDDB, TRACE_DIAG_EVENT_IRQ);

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTACWListTerm()                            */
/*                                                                         */
/* D e s c r i p t i o n: ACW-List-Handling takes longer than duration     */
/*                        of Cycle.                                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTACWListTerm( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    LSA_UINT32               const  para_1 )
{
    LSA_UNUSED_ARG(para_1); //satisfy lint!
    LSA_UNUSED_ARG(pDDB  ); //satisfy lint!

    EDDI_ENTER_REST_S();

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_INTACWListTerm->ACW-List-Handling takes longer than duration of Cycle");

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTSRTCycleSkipped()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTSRTCycleSkipped( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  para_1 )
{
    LSA_UNUSED_ARG(para_1); //satisfy lint!
    LSA_UNUSED_ARG(pDDB  ); //satisfy lint!

    EDDI_ENTER_REST_S();

	EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_INTSRTCycleSkipped ERROR: maybe ring closed without MRP.");

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTPSScoreboardChanged()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTPSScoreboardChanged( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  para_1 )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  pCRTComp;

    LSA_UNUSED_ARG(para_1); //satisfy lint!

    EDDI_ENTER_REST_S();

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTPSScoreboardChanged->");

    pCRTComp = pDDB->pLocal_CRT;

    if (LSA_HOST_PTR_ARE_EQUAL(pCRTComp, LSA_NULL))
    {
        EDDI_EXIT_REST_S();
        return;
    }

    EDDI_CRTRpsTrigger(&pCRTComp->Rps, pDDB, EDDI_CRT_RPS_EVENT_SB_CHANGED);

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/




/*****************************************************************************/
/*  end of file eddi_isr_rest.c                                              */
/*****************************************************************************/
