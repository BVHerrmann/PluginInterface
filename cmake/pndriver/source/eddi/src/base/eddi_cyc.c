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
/*  F i l e               &F: eddi_cyc.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI General requests                            */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_EDDI_CYC
#define LTRC_ACT_MODUL_ID  4

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"
#include "eddi_ext.h"
//#include "eddi_prm_req.h"

#include "eddi_ser_cmd.h"
#include "eddi_irt_tree.h"

#include "eddi_ser_ext.h"

#include "eddi_nrt_ini.h"
#include "eddi_crt_ext.h"

#include "eddi_sync_ir.h"
#include "eddi_sync_usr.h"
#include "eddi_lock.h"
#include "eddi_io_iso.h"

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycStateRunning ( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB,
                                                              EDDI_DDB_CYC_COUNTER_TYPE       * const   pCycCount,
                                                              EDDI_CYC_COUNTER_EVENT_TYPE       const   Event );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycStateJump ( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB,
                                                           EDDI_DDB_CYC_COUNTER_TYPE             *   pCycCount,
                                                           EDDI_CYC_COUNTER_EVENT_TYPE       const   Event );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycStateAdjustPhase ( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB,
                                                                  EDDI_DDB_CYC_COUNTER_TYPE             *   pCycCount,
                                                                  EDDI_CYC_COUNTER_EVENT_TYPE       const   Event );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycBeginCycleCounterJump( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB,
                                                                      EDDI_DDB_CYC_COUNTER_TYPE             *   pCycCount );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR  EDDI_CycSetCounter(       EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB,
                                                           const EDDI_DDB_CYC_COUNTER_TYPE       * const   pCycCount,
                                                                 LSA_INT32                            *   pSyncError_10ns );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR  EDDI_CycSetSoftwareCounter( EDDI_LOCAL_DDB_PTR_TYPE          const   pDDB,
                                                                   LSA_UINT32                       const   SetCycleCounterHigh,
                                                                   LSA_UINT32                       const   SetCycleCounterLow,
                                                                   LSA_UINT32                       const   SetTimeStamp,
                                                                   LSA_BOOL                         const   bAvoidCriticalJumps );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_CycCheckPara ( EDDI_LOCAL_DDB_PTR_TYPE          const   pDDB,
                                                           LSA_BOOL                         const   bCallExcp,
                                                           LSA_UINT16                       const   CycleBaseFactor,
                                                           LSA_UINT32                       const   TriggerDeadline,
                                                           LSA_UINT16                       const   BufferMode );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycInitCycleLength( LSA_UINT16                       const   CycleBaseFactor,
                                                                LSA_UINT32                       const   TriggerDeadLine_10ns,
                                                                EDDI_LOCAL_DDB_PTR_TYPE          const   pDDB,
                                                                LSA_BOOL                         const   bHW_Running );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycInitCycleLengthExpansion ( EDDI_LOCAL_DDB_PTR_TYPE          const   pDDB,
                                                                          LSA_INT32                        const   SyncError_10ns );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycExpandCycleLength ( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR  EDDI_CycUpdateExpandedCycleLength ( EDDI_LOCAL_DDB_PTR_TYPE           const   pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycRestoreCycleLength ( EDDI_LOCAL_DDB_PTR_TYPE          const   pDDB,
                                                                    LSA_UINT32                       const   CycleLength_10ns );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycCalcEffectiveCycleLength( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                         EDDI_DDB_CYC_COUNTER_TYPE  *  const  pCycCount );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CycPassSoftwareCounterHigh( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                        LSA_UINT32               const  SetCycleCounterHigh );


/*=============================================================================
* function name: EDDI_CycInit()
*
* function:      create structs für IRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycInit(       EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE     const  pCyclComp,
                                             const EDDI_CRT_INI_TYPE                   *  const  pCrtIni,
                                                   EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB )
{
    LSA_UINT32  Value;
    LSA_RESULT  Result;
    LSA_UINT16  TriggerMode;
    LSA_UINT32  TriggerDeadlineIn10ns;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycInit->");

    TriggerMode = (EDD_FEATURE_ENABLE == pCrtIni->bUseTransferEnd)?EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC:EDDI_SYNC_TRIGMODE_IRT_END;
    /* Deadline shall be 100ns before cycleend as default */
    TriggerDeadlineIn10ns = (pDDB->CRT.CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY) - 10;

    Result = EDDI_CycCheckPara(pDDB,
                               LSA_TRUE,
                               pDDB->CRT.CycleBaseFactor,
                               TriggerDeadlineIn10ns,
                               (LSA_UINT16)EDDI_SYNC_IMAGE_BUF_MODE);
    if (EDD_STS_OK != Result)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInit, EDDI_CycCheckPara: CycleBaseFactor:0x%X TriggerDeadlineIn10ns:0x%X",
                               pDDB->CRT.CycleBaseFactor, TriggerDeadlineIn10ns);
        EDDI_Excp("EDDI_CycInit, EDDI_CycCheckPara", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    pDDB->CRT.SyncImage.ImageExpert.TriggerDeadlineIn10ns = TriggerDeadlineIn10ns;
    pDDB->CRT.SyncImage.ImageExpert.TriggerMode           = TriggerMode;

    EDDI_CycInitCycleLength(pDDB->CRT.CycleBaseFactor, TriggerDeadlineIn10ns, pDDB, LSA_TRUE);

    //Configure MicroCycle-Signals for MC
    //clear CLKGen-Command
    //CLK_COMMAND is 0 after reset anyway

    //config APPL-Clocks
    EDDI_IsoInitDefaultApplclock(pDDB, pCyclComp);

    //=== Set XPLLSignal
    switch (pCyclComp->PDControlPllMode)
    {
        case EDDI_CYCLE_INI_PLL_MODE_NOTUSED:
        {
            Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_SET, pCyclComp->AplClk_Divider, pCyclComp->ClkCy_Period);  
            if (EDD_STS_OK != Result)
            {
                EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInit, EDDI_IsoCtrlDefaultApplclock Result:0x%X, PDControlPllMode:0x%X, AplClk_Divider:0x%X, ClkCy_Period:0x%X", 
                    Result, pCyclComp->PDControlPllMode, pCyclComp->AplClk_Divider, pCyclComp->ClkCy_Period); 
                EDDI_Excp("EDDI_CycInit, EDDI_IsoCtrlDefaultApplclock", EDDI_FATAL_ERR_EXCP, Result, 0);
                return;
            }
            break;
        }

        case EDDI_CYCLE_INI_PLL_MODE_OFF:
        case EDDI_CYCLE_INI_PLL_MODE_XPLL_EXT_OUT:
        {
            pDDB->PRM.PDControlPLL.PDControlPllActive = LSA_TRUE;

            Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_SET, pDDB->CRT.CycleBaseFactor, EDDI_CRT_CYCLE_LENGTH_GRANULARITY);  
            if (EDD_STS_OK != Result)
            {
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInit, EDDI_IsoCtrlDefaultApplclock Result:0x%X, PDControlPllMode:0x%X, CycleBaseFactor:0x%X", 
                    Result, pCyclComp->PDControlPllMode, pDDB->CRT.CycleBaseFactor); 
                EDDI_Excp("EDDI_CycInit, EDDI_IsoCtrlDefaultApplclock", EDDI_FATAL_ERR_EXCP, Result, 0);
                return;
            }

            if (EDDI_CYCLE_INI_PLL_MODE_OFF == pCyclComp->PDControlPllMode)
            {
                Result = EDDI_XPLLSetPLLMode(EDDI_PRM_DEF_PLL_MODE_OFF, pDDB);
            }
            else
            {
                Result = EDDI_XPLLSetPLLMode(EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT, pDDB);
            }

            if (EDD_STS_OK != Result)
            {
                EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInit, EDDI_IsoCtrlDefaultApplclock Result:0x%X, PDControlPllMode:0x%X, AplClk_Divider:0x%X, ClkCy_Period:0x%X", 
                    Result, pCyclComp->PDControlPllMode, pCyclComp->AplClk_Divider, pCyclComp->ClkCy_Period); 
                EDDI_Excp("EDDI_CycInit, EDDI_XPLLSetPLLMode", EDDI_FATAL_ERR_EXCP, Result, 0);
                return;
            }

            break;
        }

        default:
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInit, PDControlPllMode, PDControlPllMode:0x%X", 
                pCyclComp->PDControlPllMode); 
            EDDI_Excp("EDDI_CycInit, PDControlPllMode:", EDDI_FATAL_ERR_EXCP, pCyclComp->PDControlPllMode, 0);
            return;
        }
    }

    //Set Phase-Correction
    //PHASE_CORRECTION is 0 after reset anyway

    //Enable StartOp-Signalisation for SyncImage

    Value = IO_R32(IRT_CYCL_ID) | PARA_START_APP | PARA_EOF_APP;
    IO_W32(IRT_CYCL_ID, Value);

    IO_W32(DATA_UPDATE, 0x10000);  // enable the data update counter
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycInitCycleLength()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycInitCycleLength( LSA_UINT16               const  CycleBaseFactor,
                                                                LSA_UINT32               const  TriggerDeadLine_10ns,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_BOOL                 const  bHW_Running )
{
    LSA_UINT32              deltaCounter;
    LSA_UINT32              newCycleCounterLow  = 0;
    LSA_UINT32              newCycleCounterHigh = 0;
    LSA_UINT32              TimeStampInTicks    = 0;
    EDDI_SYNC_GET_SET_TYPE  SyncGet;
    LSA_UINT32              tmp;
    LSA_RESULT              Result;
    LSA_UINT8               ErrCtr = 0;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycInitCycleLength->CycleBaseFactor:0x%X", (LSA_UINT32)CycleBaseFactor);

    pDDB->CycCount.bCBFIsBinary     = EDDI_RedIsBinaryValue((LSA_UINT32)CycleBaseFactor);
    pDDB->CycCount.Entity           = (LSA_UINT32)CycleBaseFactor;
    pDDB->CycCount.CycleLength_10ns = (LSA_UINT32)(CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY);
    pDDB->CycCount.CycleLength_us   = (LSA_UINT32)((CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY) + 50UL) / 100UL;

    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    EDDI_SIIGetNewCycleReductionFactors(pDDB, (LSA_UINT32)CycleBaseFactor, pDDB->CycCount.CycleLength_10ns, pDDB->CycCount.CycleLength_us);
    #elif defined (EDDI_CFG_SII_POLLING_MODE)
    pDDB->NewCycleReductionFactor = EDDI_SIIGetNewCycleReductionFactor(pDDB, pDDB->CycCount.CycleLength_10ns);
    #else
    pDDB->NewCycleReductionFactor = EDDI_SIIGetNewCycleReductionFactor(pDDB->CycCount.CycleLength_10ns);
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    if //ExtTimerMode on?
       (pDDB->SII.SII_ExtTimerInterval)
    #endif
    {
        pDDB->ExtTimerReductionFactor = EDDI_SIIGetExtTimerReductionFactor(pDDB);
    }
    #endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX
    #endif //EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE

    if (pDDB->NewCycleReductionFactor > ((EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS * 32)/EDDI_CRT_CYCLE_BASE_FACTOR_MIN)) //0 is already checked in EDDI_SIIGetNewCycleReductionFactors() or EDDI_SIIGetNewCycleReductionFactor()
    {
        EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInitCycleLength, invalid NewCycleReductionFactor:0x%X",
                               pDDB->NewCycleReductionFactor);
        EDDI_Excp("EDDI_CycInitCycleLength, invalid NewCycleReductionFactor:", EDDI_FATAL_ERR_EXCP, pDDB->NewCycleReductionFactor, 0);
        return;
    }

    EDDI_CycCalcEffectiveCycleLength(pDDB, &pDDB->CycCount);

    //try to set the Timestamp-timeout around 4ms
    if      (CycleBaseFactor > 32) {pDDB->NRT.TimeStamp.Timeout = 2;}
    else if (CycleBaseFactor > 16) {pDDB->NRT.TimeStamp.Timeout = 4;}
    else if (CycleBaseFactor >  8) {pDDB->NRT.TimeStamp.Timeout = 8;}
    else if (CycleBaseFactor >  4) {pDDB->NRT.TimeStamp.Timeout = 16;}
    else if (CycleBaseFactor >  2) {pDDB->NRT.TimeStamp.Timeout = 32;}
    else if (CycleBaseFactor >  1) {pDDB->NRT.TimeStamp.Timeout = 64;}
    else                           {pDDB->NRT.TimeStamp.Timeout = 128;}

    tmp = (pDDB->NewCycleReductionFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY) / 100UL;
    if (tmp > 32000UL)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInitCycleLength, invalid NewCycleReductionFactor:0x%X tmp:0x%X",
                               pDDB->NewCycleReductionFactor, tmp);
        EDDI_Excp("EDDI_CycInitCycleLength, invalid NewCycleReductionFactor:", EDDI_FATAL_ERR_EXCP, pDDB->NewCycleReductionFactor, tmp);
        return;
    }

    #if defined (EDDI_CFG_USE_SW_RPS)
    pDDB->SW_RPS_counter = 0;
    pDDB->SW_RPS_NextUK_Always    = LSA_TRUE;
    //next point in time, when swsb is allowed to be checked (same for SWSB based on EXTTIMER!) 
    pDDB->SW_RPS_NextUK_Cycle_10ns = (pDDB->CycCount.CycleLength_10ns * pDDB->NewCycleReductionFactor) - (2*EDDI_CRT_CYCLE_LENGTH_GRANULARITY); //subtract goodwill-time

    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    pDDB->SW_RPS_TotalCallsCtr    = 0;
    pDDB->SW_RPS_RejectedCallsCtr = 0;
    #endif

    //When CycleLength >  1ms  (big Cycles)   --> Call RPS-Trigger each Cycle
    //When CycleLength <= 1ms  (small Cycles) --> Call RPS-Trigger each 3ms
    if (pDDB->CycCount.CycleLength_us  > 1000)
    {
        pDDB->SW_RPS_internal_reduction = 2;
    }
    else
    {
        pDDB->SW_RPS_internal_reduction = (3 * 1000) / (pDDB->CycCount.CycleLength_us * pDDB->NewCycleReductionFactor);
        if (pDDB->SW_RPS_internal_reduction == 0)
        {
            pDDB->SW_RPS_internal_reduction = 2;
        }
    }
    #endif //(EDDI_CFG_USE_SW_RPS)

    EDDI_NrtSetSendLimit(pDDB);

    //signal stopping of xpllout to application
    EDDI_SIGNAL_SENDCLOCK_CHANGE(pDDB->hSysDev, pDDB->CycCount.Entity, EDDI_SENDCLOCK_CHANGE_XPLLOUT_DISABLED);

    Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_DISABLE, CycleBaseFactor, 0 /*irrelevant*/);  
    if (EDD_STS_OK != Result)
    {
        //#pragma remove
        //Due to a bug in IRTE (all Revs)Multicycles can only be used with a special workaround (needs multiple states).
        //see also documentation for EDDI-isochronous support
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_InitCycleLength, No MultiApplCycles allowed here! ClkGen_Command:0x%X", IO_R32(CLK_COMMAND));
        EDDI_Excp("EDDI_CycInitCycleLength, no MultiApplCycles allowed here", EDDI_FATAL_ERR_EXCP, IO_R32(CLK_COMMAND), 0);
        return;
    }

    //Set scoreboard-size
    #if defined (EDDI_CFG_SMALL_SCF_ON)
    {
        //for SCF<8, the scoreboard gets reduced to avoid the PS_Skipped interrupt
        LSA_UINT16  TimerScorebBlocks = (LSA_UINT16)pDDB->CRT.MetaInfo.FcwConsumerCnt + (LSA_UINT16)pDDB->CRT.MetaInfo.AcwConsumerCnt;

        if (pDDB->CycCount.Entity < EDDI_CRT_CYCLE_BASE_FACTOR_CRIT)
        {
            if (EDDICrtMaxConsSmallSCF[pDDB->CycCount.Entity] < TimerScorebBlocks)
            {
                TimerScorebBlocks = EDDICrtMaxConsSmallSCF[pDDB->CycCount.Entity];
            }
        }

        //calc count of Timer-Scoreboard-Blocks -> have to be a multiple of 16
        //reason: 1) TimerScoreBoardEntries  have to be a multiple of 4
        //reason: 2) ScoreBoardBase addresses have to be on a 8Byte boundary
        TimerScorebBlocks = (TimerScorebBlocks + 15) & 0xFFF0;

        //change HWSB entries
        if (!EDDI_CRTRpsIsStopped(&pDDB->CRT.Rps))
        {
            EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_InitCycleLength, HWSB is not stopped. UsedRTC12Cons/UsedRTC3Cons:%d/%d, ActiveRTC12Cons/ActiveRTC3Cons:%d/%d", 
                pDDB->CRT.ConsumerList.UsedEntriesRTC123, pDDB->CRT.ConsumerList.UsedEntriesDFP, pDDB->CRT.ConsumerList.ActiveConsRTC12, pDDB->CRT.ConsumerList.ActiveConsRTC3);
        
            EDDI_Excp("EDDICyc_InitCycleLength, HWSB is not stopped.", EDDI_FATAL_ERR_EXCP, pDDB->CRT.ConsumerList.ActiveConsRTC12, pDDB->CRT.ConsumerList.ActiveConsRTC3);
            return;
        }
        IO_W32(TIMER_SCOREB_ENTRIES, TimerScorebBlocks);
    }
    #endif //EDDI_CFG_SMALL_SCF_ON
    
    //Set Entity
    IO_W32(CYCL_CNT_ENTITY, pDDB->CycCount.Entity);
    //Set CYCLE-Length
    IO_W32(CYCL_LENGTH, pDDB->CycCount.CycleLength_10ns);

    Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_ENABLE, 0 /*irrelevant*/, 0 /*irrelevant*/);  
    if (EDD_STS_OK != Result)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInitCycleLength, EDDI_IsoCtrlDefaultApplclock: Result 0x%X", 
            Result); 
        EDDI_Excp("EDDI_CycInitCycleLength, EDDI_IsoCtrlDefaultApplclock", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    //signal starting of xpllout to application
    EDDI_SIGNAL_SENDCLOCK_CHANGE(pDDB->hSysDev, pDDB->CycCount.Entity, EDDI_SENDCLOCK_CHANGE_XPLLOUT_ENABLED);

    //Check if CycleCtr has to be aligned. This has to be done after the new entity had been set!
    if (bHW_Running)
    {
        for (ErrCtr=0; ErrCtr<3; ErrCtr++)
        {
            (void)EDDI_CycUpdateSoftwareCounter(pDDB, &SyncGet, EDDI_NULL_PTR);

            //calculate next fitting Low-Value according to new CycleBaseFactor;
            //--> We need this calculation to avoid unaligned CycleCounter-Iterations
            {
                LSA_UINT64  CycleCounter64;
                LSA_UINT64  newCycleCounter64;

                CycleCounter64      = ((LSA_UINT64)SyncGet.CycleCounterHigh << 32UL) + (LSA_UINT64)SyncGet.CycleCounterLow;
                //align CycleCounter to new CycleBaseFactor (must be a multiple)
                newCycleCounter64   = ((CycleCounter64 / CycleBaseFactor)) * CycleBaseFactor;
                deltaCounter = (LSA_UINT32)(CycleCounter64 - newCycleCounter64);

                //build new CycleCounterHigh und Low
                newCycleCounterHigh = (LSA_UINT32) (newCycleCounter64 >> 32);
                newCycleCounterLow  = (LSA_UINT32) (newCycleCounter64 & 0xFFFFFFFF);
                if (newCycleCounter64 != CycleCounter64)
                {
                    //calculate TimeStamp
                    TimeStampInTicks = SyncGet.CycleBeginTimeStampInTicks - (EDDI_CRT_CYCLE_LENGTH_GRANULARITY * deltaCounter * EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS);
                            (void)EDDI_CycSetSoftwareCounter(pDDB,
                                                             newCycleCounterHigh,
                                                             newCycleCounterLow,
                                                             TimeStampInTicks,
                                                             LSA_FALSE);
                }

                //Check if CC meets requirements.
                if (pDDB->CycCount.bCBFIsBinary)
                {
                    newCycleCounterLow = IO_R32(CYCL_COUNT_VALUE);
                    if (0 != (newCycleCounterLow % pDDB->CycCount.Entity) )
                    {
                        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDICyc_InitCycleLength, CycleCtr 0x%X not aligned to Entity 0x%X after try %i", 
                            newCycleCounterLow, pDDB->CycCount.Entity, ErrCtr);
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12) || defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
                    (void)EDDI_CycUpdateSoftwareCounter(pDDB, &SyncGet, EDDI_NULL_PTR);
                    CycleCounter64 = ((LSA_UINT64)SyncGet.CycleCounterHigh << 32UL) + (LSA_UINT64)SyncGet.CycleCounterLow;

                    if (0 != (CycleCounter64 % pDDB->CycCount.Entity) )
                    {
                        EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDICyc_InitCycleLength, CycleCtr 0x%X%X not aligned to Entity 0x%X after try %i", 
                            SyncGet.CycleCounterHigh, SyncGet.CycleCounterLow, pDDB->CycCount.Entity, ErrCtr);
                    }
                    else
                    {
                        break;
                    }
                    #else
                    // Only needed for binary entities, as for nonbinary entities a reduction >1 is not allowed.
                    break;
                    #endif
                }
            }
        }
    }

    if (ErrCtr >= 3)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_InitCycleLength, Could not manage to set aligned CycleCtr, Entity:0x%X", pDDB->CycCount.Entity); 
        EDDI_Excp("EDDICyc_InitCycleLength, Could not manage to set aligned CycleCtr", EDDI_FATAL_ERR_EXCP, 0, pDDB->CycCount.Entity);
        return;
    }
    
    //Set Start-Sycn-IRQ-Delay
    IO_W32(IRT_COMP_TIME, TriggerDeadLine_10ns);

    //Update Interrupt Statistik-Info:
    pDDB->IntStat.cycle_in_10ns_ticks = pDDB->CycCount.CycleLength_10ns;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycCheckPara()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CycCheckPara( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_BOOL                 const  bCallExcp,
                                                            LSA_UINT16               const  CycleBaseFactor,
                                                            LSA_UINT32               const  TriggerDeadline_10ns,
                                                            LSA_UINT16               const  BufferMode )
{
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDICyc_CheckPara->");

    if (CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_MIN)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDICyc_CheckPara, CycleBaseFactor too small, CycleBaseFactor:0x%X 0x0008UL:0x%X",
                              CycleBaseFactor, 0x0008UL);
        if (bCallExcp)
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_CheckPara, EDDI_CRT_CYCLE_BASE_FACTOR_MIN:0x%X",
                                   CycleBaseFactor);
            EDDI_Excp("EDDICyc_CheckPara, EDDI_CRT_CYCLE_BASE_FACTOR_MIN", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, 0);
        }
        else
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase_Min);
        }
        return EDD_STS_ERR_PARAM;
    }

    if (CycleBaseFactor > EDDI_CRT_CYCLE_BASE_FACTOR_MAX)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDICyc_CheckPara, CycleBaseFactor too big, CycleBaseFactor:0x%X 0x8000UL:0x%X",
                              CycleBaseFactor, 0x8000UL);
        if (bCallExcp)
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_CheckPara, EDDI_CRT_CYCLE_BASE_FACTOR_MIN:0x%X",
                                   CycleBaseFactor);
            EDDI_Excp("EDDICyc_CheckPara, EDDI_CRT_CYCLE_BASE_FACTOR_MAX", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, 0);
        }
        else
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase_Max);
        }

        return EDD_STS_ERR_PARAM;
    }

    //Check SyncImage - Params
    if (BufferMode != EDDI_SYNC_IMAGE_OFF)
    {
        LSA_UINT32  CycleLen_10ns; //In 10ns

        CycleLen_10ns = CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY;

        if (CycleLen_10ns <= TriggerDeadline_10ns) //StartOP-Trigger
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDICyc_CheckPara, TriggerDeadline > CycleLen, CycleLen_10ns:0x%X TriggerDeadline_10ns:0x%X",
                                  CycleLen_10ns, TriggerDeadline_10ns);
            if (bCallExcp)
            {
                EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDICyc_CheckPara, CycleLen_10ns:0x%X, TriggerDeadline:0x%X",
                                       CycleLen_10ns, TriggerDeadline_10ns);
                EDDI_Excp("EDDICyc_CheckPara, TriggerDeadline", EDDI_FATAL_ERR_EXCP, CycleLen_10ns, TriggerDeadline_10ns);
            }
            else
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CycleLength_TriggerDeadline);
            }

            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDICycSendclockChange()                         */
/*                                                                         */
/* D e s c r i p t i o n: A SendClockChange has to be performed in 3       */
/*                        steps:                                           */
/*                                                                         */
/*                        1)  Tell SyncPortStms to shutdown IRT-           */
/*                            Communication and freeze its reset state.    */
/*                                                                         */
/*                        2a) When SyncPortStms is finished it calls back  */
/*                            (via EDDICycSndClkCh_TransitionDoneCbf) all  */
/*                            FCWs will be removed safely and              */
/*                                                                         */
/*                        2b) the SendClock can now be changed.            */
/*                                                                         */
/*                        3)  Tell SyncPortStms to unfreeze reset state.   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CycSendclockChange( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          LSA_BOOL                 *  const  pbIndicate )
{
    LSA_RESULT                                     Result;
    EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE     const  pParam              = (EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE        const  pCRTComp            = &pDDB->CRT;
    EDDI_SNDCLKCH_TRANSITION_TYPE        *  const  pSndClkChTransition = &pDDB->CycCount.SndClkChTransition;
    LSA_UINT32                                     NewCycleLength_10ns;
    LSA_UINT16                              const  CycleBaseFactor     = pParam->CycleBaseFactor;
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSendclockChange->Current SendClock:0x%X pParam->CycleBaseFactor:0x%X",
                           pCRTComp->CycleBaseFactor, CycleBaseFactor);

    *pbIndicate = LSA_TRUE;
                                      
    //no change of CycleBaseFactor and limits
    if (CycleBaseFactor == pCRTComp->CycleBaseFactor)
    {
        return EDD_STS_OK;
    }

    if (CycleBaseFactor != 0)
    {
        if ((CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_CRIT)  &&  (EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole))
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, ERROR: CBF:%i < %i not allowed with HSYNC_APPLiCATION",
                                   CycleBaseFactor, EDDI_CRT_CYCLE_BASE_FACTOR_CRIT);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TooSmallCBFWithHSyncApplication);
            return EDD_STS_ERR_PARAM;
        }

        if (CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_MIN)
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, ERROR: CBF:%i < %i not allowed",
                                   CycleBaseFactor, EDDI_CRT_CYCLE_BASE_FACTOR_MIN);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerNotRemoved);
            return EDD_STS_ERR_SEQUENCE;
        }

        if (   (CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_CRIT)
            && (pCRTComp->ConsumerList.UsedEntriesRTC123) )
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, no consumer may be added for CBF:%i, UsedEntriesRTC123:%i",
                                   pCRTComp->CycleBaseFactor, pCRTComp->ConsumerList.UsedEntriesRTC123);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerNotRemoved);
            return EDD_STS_ERR_SEQUENCE;
        }
    }

    if (pCRTComp->ConsumerList.UsedACWs)
    {
        EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, EDDI_ERR_INV_ConsumerNotRemoved, ConsumerList.UsedACWs:%i",
                               pCRTComp->ConsumerList.UsedACWs);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerNotRemoved);
        return EDD_STS_ERR_SEQUENCE;
    }
    else if (pCRTComp->ProviderList.UsedACWs)
    {
        EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, EDDI_ERR_INV_ProviderNotRemoved, ProviderList.UsedACWs:%i",
                               pCRTComp->ProviderList.UsedACWs);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderNotRemoved);
        return EDD_STS_ERR_SEQUENCE;
    }
    else if (pDDB->CycCount.SndClkChTransition.bTransitionRunning)
    {
        EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, EDDI_ERR_INV_SndClkChTransitionRunning");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SndClkChTransitionRunning);
        return EDD_STS_ERR_SEQUENCE;
    }

    #if defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)
    if((CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_250)
       && (EDDI_CRT_CYCLE_BASE_FACTOR_125 != CycleBaseFactor)
       && (EDDI_CRT_CYCLE_BASE_FACTOR_188 != CycleBaseFactor)
       && (0 != CycleBaseFactor))
    {
        EDDI_FUNCTION_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycSendclockChange, ERROR -> CycleBaseFactor(%d) is out of range (%d;%d;%d..%d)!",
                                CycleBaseFactor, EDDI_CRT_CYCLE_BASE_FACTOR_125, EDDI_CRT_CYCLE_BASE_FACTOR_188, EDDI_CRT_CYCLE_BASE_FACTOR_250, EDDI_CRT_CYCLE_BASE_FACTOR_MAX);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PARAM);
        return EDD_STS_ERR_PARAM;
    }
    #endif // defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)

    //calc new values only if now clearing up (CycleBaseFactor == 0) takes place
    pSndClkChTransition->TriggerDeadline_10ns = 0;

    if (CycleBaseFactor)
    {
        NewCycleLength_10ns = (CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY);

        /* Deadline shall be 100ns before cycleend as default */
        /* Note. This setting will also be changed on PRM-COMMIT!  */
        /*       We use the defaults here because PRM will be done */
        /*       after a change of the sendclock, so we dont know  */
        /*       for example the reserved_interval here!           */
        pSndClkChTransition->TriggerDeadline_10ns = NewCycleLength_10ns - 10;

        Result = EDDI_CycCheckPara(pDDB, LSA_FALSE, CycleBaseFactor, pSndClkChTransition->TriggerDeadline_10ns,
                                   (LSA_UINT16)EDDI_SYNC_IMAGE_BUF_MODE);
        if (EDD_STS_OK != Result)
        {
            return Result;
        }
    }

    // Inform SyncPortStms to shutdown all FCW-Lists
    // --> Reason:Manipulation of FCW-Lists can only be done
    //     when FCW-lists are inactive.
    pDDB->CycCount.SndClkChTransition.pRunningRQB       = pRQB;
    pDDB->CycCount.SndClkChTransition.bTransitionRunning = LSA_TRUE;

    EDDI_SyncPortStmsSendClockChange(pDDB, EDDI_CycSndClkChTransitionDoneCbf);

    *pbIndicate = LSA_FALSE; // RQB-Cbf will be called asynchronosly.

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycSndClkChTransitionDoneCbf()              */
/*                                                                         */
/* D e s c r i p t i o n: see EDDI_CycSetSyncTime()                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycSndClkChTransitionDoneCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16                                     OldCycleBaseFactor;
    LSA_RESULT                                     Result;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE        const  pCRTComp            = &pDDB->CRT;
    EDDI_SNDCLKCH_TRANSITION_TYPE        *  const  pSndClkChTransition = &pDDB->CycCount.SndClkChTransition;
    EDDI_LOCAL_HDB_PTR_TYPE                        pHDB;
    EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE            pParam;

    if (   (!pSndClkChTransition->bTransitionRunning)
        || (pSndClkChTransition->pRunningRQB == EDDI_NULL_PTR))
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSndClkChTransitionDoneCbf, bTransitionRunning:%d, pRunningRQB:0x%X",
                               pSndClkChTransition->bTransitionRunning, pSndClkChTransition->pRunningRQB);
        EDDI_Excp("EDDI_CycSndClkChTransitionDoneCbf, Mismatch in SendClockChangeTransition",
                  EDDI_FATAL_ERR_EXCP, pSndClkChTransition->bTransitionRunning, pSndClkChTransition->pRunningRQB);
        return;
    }

    pParam = (EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE)pSndClkChTransition->pRunningRQB->pParam;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSndClkChTransitionDoneCbf, Current SendClock:0x%X pParam->CycleBaseFactor:0x%X",
                           pCRTComp->CycleBaseFactor, pParam->CycleBaseFactor);

    if (pDDB->PRM.PDSyncData.State_A == EDDI_PRM_VALID)
    {
        pDDB->PRM.PDSyncData.State_A = EDDI_PRM_NOT_VALID;
    }
    
    #if defined (EDDI_CFG_DFP_ON)
    if (pDDB->PRM.PDIRSubFrameData.State_A == EDDI_PRM_VALID)
    {
        pDDB->PRM.PDIRSubFrameData.State_A = EDDI_PRM_NOT_VALID;
    }
    #endif
    
    //only remove IRT-FCWs if RecordSet_A is in use
    if (EDDI_PRM_VALID == pDDB->PRM.PDIRData.State_A)
    {
        Result = EDDI_SyncIrDeactivate(pDDB);
        if (EDD_STS_OK != Result)
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_prm_move_B_to_A, SyncIr_EndRecord_B, Result:0x%X", Result);
            EDDI_Excp("eddi_prm_move_B_to_A, SyncIr_EndRecord_B", EDDI_FATAL_ERR_EXCP, Result, 0);
            return;
        }

        pDDB->PRM.PDIRData.State_A = EDDI_PRM_NOT_VALID;
    }

    if (0 == pParam->CycleBaseFactor)
    {
        // Special Usecase of SendClockChange: Only Deactivate IRT but leave all the rest.
        // --> No change of current Sendclock.
        Result = EDD_STS_OK;
    }
    else
    {
        // SRT-Phasenbaum loeschen und anschliessend wieder aufbauen
        if (pCRTComp->MetaInfo.AcwProviderCnt)
        {
            LSA_UINT32                         StartMask;
            EDDI_LOCAL_EOL_PTR_TYPE            pEOLn[1];
            EDDI_TREE_TYPE           *  const  pSrtRedTree = &pCRTComp->SrtRedTree;

            StartMask = 0x7FFFUL;
            EDDI_SERSingleDirectCmd(FCODE_DISABLE_ACW_TX_LIST, StartMask, 0, pDDB);

            EDDI_RedTreeClose(pDDB, pSrtRedTree);

            EDDI_RedTreeInit(pDDB, pSrtRedTree,
                             EDDI_TREE_RT_CLASS_ACW_TX,
                             pParam->CycleBaseFactor,
                             EDDI_TREE_MAX_BINARY_REDUCTION,
                             pCRTComp->MetaInfo.AcwProviderCnt);

            pEOLn[0] = pCRTComp->RTClass2Interval.pEOL;
            EDDI_RedTreeBuild(pDDB,
                              pSrtRedTree,
                              pParam->CycleBaseFactor,
                              (EDDI_SER_CCW_PTR_TYPE)(void *)pCRTComp->RTClass2Interval.pRootAcw,
                              &pEOLn[0]);

            EDDI_SERSingleDirectCmd(FCODE_ENABLE_ACW_TX_LIST, StartMask, 0, pDDB);
        }

        if (EDD_FEATURE_ENABLE == pCRTComp->CfgPara.bUseTransferEnd)
        {
            /* ---------------------------------------------------------------------- */
            /* Setup the TriggerMode to Default (startup) setting on sendclock change */
            /* This setting will be changed on PRM-COMMIT!                            */
            /* ---------------------------------------------------------------------- */
            LSA_UINT32  Help;

            Help = IO_x32(IMAGE_MODE);
            EDDI_SetBitField32(&Help, EDDI_SER_IMAGE_MODE_BIT__SyncAPI, pCRTComp->SyncImage.ImageExpert.TriggerMode); /* does big/little endian handling! */
            IO_x32(IMAGE_MODE) = Help;

            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSndClkChTransitionDoneCbf, TriggerMode:0x%X TimeCompare10ns:%d",
                                   pCRTComp->SyncImage.ImageExpert.TriggerMode, pSndClkChTransition->TriggerDeadline_10ns);
        }

        EDDI_CycInitCycleLength(pParam->CycleBaseFactor, pSndClkChTransition->TriggerDeadline_10ns, pDDB, LSA_TRUE);

        //save old CycleBaseFactor
        OldCycleBaseFactor = pCRTComp->CycleBaseFactor;

        //save new CFG data
        pCRTComp->CycleBaseFactor                              = pParam->CycleBaseFactor; 

        pCRTComp->SyncImage.ImageExpert.TriggerDeadlineIn10ns  = pSndClkChTransition->TriggerDeadline_10ns;

        EDDI_IRTTreeSendClockChange(pDDB, pParam->CycleBaseFactor, OldCycleBaseFactor);

        Result = EDD_STS_OK;
    }

    pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pSndClkChTransition->pRunningRQB->internal_context;
    EDDI_RequestFinish(pHDB, pSndClkChTransition->pRunningRQB, Result);

    pSndClkChTransition->bTransitionRunning = LSA_FALSE;
    pSndClkChTransition->pRunningRQB        = EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycSetSyncTime()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CycSetSyncTime( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                      EDDI_LOCAL_HDB_PTR_TYPE     const  pHDB,
                                                      LSA_BOOL                 *  const  bIndicate )
{
    LSA_UINT64              Nanoseconds;
    LSA_UINT64              ClockCount64;
    LSA_UINT64              CycleNanoseconds;
    LSA_UINT32              DeltaInNanoSeconds, DeltaTimeStamp;
    LSA_UINT32        const ActSendClockInNS = pHDB->pDDB->CycCount.Entity * (LSA_UINT32)EDDI_CRT_CYCLE_LENGTH_GRANULARITY_NS;

    EDD_UPPER_SET_SYNC_TIME_PTR_TYPE     const  pParam    = (EDD_UPPER_SET_SYNC_TIME_PTR_TYPE)pRQB->pParam;
    EDDI_DDB_CYC_COUNTER_TYPE         *  const  pCycCount = &pHDB->pDDB->CycCount;

    EDDI_FUNCTION_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycSetSyncTime->");


    if (pParam->Mode != EDD_SET_SYNC_PTCP_MODE_ABSOLUTE)
    {
        EDDI_FUNCTION_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CycSetSyncTime, Mode not EDD_SET_SYNC_PTCP_MODE_ABSOLUTE Mode:0x%X", pParam->Mode);
        return EDD_STS_ERR_PARAM;
    }
    
    if (pCycCount->State != EDDI_CYC_COUNTER_STATE_RUNNING)
    {
        EDDI_FUNCTION_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CycSetSyncTime, not in RUNNING state:0x%X", pCycCount->State);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (pCycCount->pRQB != EDDI_NULL_PTR)
    {
        EDDI_FUNCTION_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSyncTime, pCycCount->pRQB != EDDI_NULL_PTR, pRQB:0x%X", pCycCount->pRQB); 
        EDDI_Excp("EDDI_CycSetSyncTime, pCycCount->pRQB != EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, pCycCount->pRQB, 0);
        return EDD_STS_ERR_EXCP;
    }

    /*************************************/
    /* calculate CycleCtr from PTCP time */
    Nanoseconds = (LSA_UINT64)pParam->PTCPSeconds * (LSA_UINT64)1000000000;
    Nanoseconds = Nanoseconds + (LSA_UINT64)pParam->PTCPNanoSeconds;

    //The rounding error from ClockCount has to be corrected by reducing the timestamp value 
    DeltaInNanoSeconds = (LSA_UINT32)(Nanoseconds % (LSA_UINT64)ActSendClockInNS);

    //Integer remainder at cycle begin
    DeltaTimeStamp = DeltaInNanoSeconds / (LSA_UINT32)EDDI_CRT_NS_PER_CLK_COUNT_TICK; 

    //Slide the local timestamps to cycle begin
    pCycCount->Param.ReferenceTimeStampInTicks = pParam->ReferenceTimeStampInTicks - DeltaTimeStamp;
    
    //Set MasterTime to cycle begin 
    CycleNanoseconds = Nanoseconds - (LSA_UINT64)DeltaInNanoSeconds;
    
    ClockCount64 = CycleNanoseconds / (LSA_UINT64)EDDI_CRT_CYCLE_LENGTH_GRANULARITY_NS;

    //From Mastertime and RcvTimeStamp: CycleCounter and local TimeStamp standardised on SendClock and cycle begin
    pCycCount->CycleCounterLow    = (LSA_UINT32)ClockCount64;
    pCycCount->CycleCounterHigh   = (LSA_UINT32)(ClockCount64 >> 32);

    EDDI_FUNCTION_TRACE_07(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_CycSetSyncTime: Nanoseconds=%08x%08x DeltaInNanoSeconds=%08x CycleNanoseconds=%08x%08x TimeStamp=%08x DeltaTimeStamp=%08x",
                        (LSA_UINT32)(Nanoseconds>>32), (LSA_UINT32)(Nanoseconds&0xffffffff), DeltaInNanoSeconds, (LSA_UINT32)(CycleNanoseconds>>32), (LSA_UINT32)(CycleNanoseconds&0xffffffff), 
                        pParam->ReferenceTimeStampInTicks, DeltaTimeStamp);
    EDDI_FUNCTION_TRACE_03(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_CycSetSyncTime: CycleCounterLow=%08x CycleCounterHigh=%08x CycleTimeStamp=%08x",
                        pCycCount->CycleCounterLow, pCycCount->CycleCounterHigh, pCycCount->Param.ReferenceTimeStampInTicks);

    //Check CycleCtr
    {
        LSA_BOOL    bError=LSA_FALSE;

        if (   pHDB->pDDB->CycCount.bCBFIsBinary
            && (0 != (pCycCount->CycleCounterLow % pHDB->pDDB->CycCount.Entity)) )
        {
            bError = LSA_TRUE;
        }
        #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12) || defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
        else if  (!pHDB->pDDB->CycCount.bCBFIsBinary)
        {
            if (0 != (ClockCount64 % pHDB->pDDB->CycCount.Entity) )
            {
                bError = LSA_TRUE;
            }
        }
        #endif
        if (bError)
        {
            EDDI_FUNCTION_TRACE_04(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSyncTime, ERROR CycleCtrHigh/Low:0x%X/0x%X misaligned to Entity:0x%X, CycleBeginTimeStampInTicks:0x%X", 
                pCycCount->CycleCounterHigh, pCycCount->CycleCounterLow, pHDB->pDDB->CycCount.Entity, pParam->ReferenceTimeStampInTicks);
            EDDI_Excp("EDDI_CycSetSyncTime, EDDI_CycSetSyncTime, ERROR CycleCtrHigh/Low:", EDDI_FATAL_ERR_EXCP, pCycCount->CycleCounterLow, pHDB->pDDB->CycCount.Entity);
            return EDD_STS_ERR_EXCP;
        }
    }    
    
    // Store RQB and HDB for later use
    pCycCount->pRQB                             = pRQB;
    pCycCount->pHDB                             = pHDB;
    pCycCount->Param.RCFInterval                = pParam->RCFInterval;

    EDDI_CycStateMachine(pHDB->pDDB, pCycCount, EDDI_CYC_COUNTER_EVENT_SET);

    *bIndicate = LSA_FALSE; // Request will be finished  in  AdjustPhase-State

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycStateMachine()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycStateMachine( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                     EDDI_DDB_CYC_COUNTER_TYPE        *  pCycCount,
                                                     EDDI_CYC_COUNTER_EVENT_TYPE  const  Event )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycStateMachine->State:0x%X Event:0x%X", pCycCount->State, Event);

    (void)EDDI_CycUpdateSoftwareCounter(pDDB, EDDI_NULL_PTR, EDDI_NULL_PTR);

    switch (pCycCount->State)
    {
        case EDDI_CYC_COUNTER_STATE_RUNNING:
        {
            EDDI_CycStateRunning(pDDB, pCycCount, Event);
            break;
        }

        case EDDI_CYC_COUNTER_STATE_JUMP:
        {
            EDDI_CycStateJump(pDDB, pCycCount, Event);
            break;
        }

        case EDDI_CYC_COUNTER_STATE_ADJUST_PHASE:
        {
            EDDI_CycStateAdjustPhase(pDDB, pCycCount, Event);
            break;
        }

        default:
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycStateMachine, illegal state(0x%X)", pCycCount->State); 
            EDDI_Excp("EDDI_CycStateMachine!", EDDI_FATAL_ERR_EXCP, pCycCount->State, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycSetCounter()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CycSetCounter(       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                           const EDDI_DDB_CYC_COUNTER_TYPE  *  const  pCycCount,
                                                                 LSA_INT32                         *  pSyncError_10ns )
{
    LSA_INT32                       SyncError_10ns;                 // non-compensated SyncError
    LSA_INT32                       SyncErrorComp_10ns;             // Runtime-Compensated SyncError
    LSA_UINT32                      LocalCycleBeginTimeStampInTicks;  // TimeStamp of local CycleBegin
    LSA_UINT32               const  ActCycLen_10ns = pCycCount->CycleLength_10ns;
    EDDI_CYCLE_COUNTER_TYPE         NewCounter;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycSetCounter->");

    if (ActCycLen_10ns != IO_R32(CYCL_LENGTH))
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetCounter, ActCycLen (0x%X) != IO_R32(CYCL_LENGTH) (0x%X) ", 
            ActCycLen_10ns, IO_R32(CYCL_LENGTH)); 
        EDDI_Excp("EDDI_CycSetCounter, ActCycLen != IO_R32(CYCL_LENGTH)", EDDI_FATAL_ERR_EXCP, ActCycLen_10ns, IO_R32(CYCL_LENGTH));
        return LSA_FALSE;
    }

    NewCounter.High = pCycCount->CycleCounterHigh;
    NewCounter.Low  = pCycCount->CycleCounterLow;

    LocalCycleBeginTimeStampInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);

    // SyncError(MasterTimeBase) = SyncError(LocalTimeBase) * RCF
    //                           = SyncError(LocalTimeBase) * ( 1 + (1 / RCFInterval) )
    //                           = SyncError(LocalTimeBase) + SyncError(LocalTimeBase) / RCFInterval
    SyncError_10ns = (LSA_INT32)(pCycCount->Param.ReferenceTimeStampInTicks - LocalCycleBeginTimeStampInTicks)/EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS;

    EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetCounter, **  NewCounter.High:0x%X NewCounter.Low:0x%X Param.CycleBeginTimeStampInTicks:0x%X LocalCycleBeginTimeStamp:0x%X SyncError:0x%X RCFInterval:0x%X", 
                           NewCounter.High, NewCounter.Low, pCycCount->Param.ReferenceTimeStampInTicks, LocalCycleBeginTimeStampInTicks, SyncError_10ns, pCycCount->Param.RCFInterval);

    SyncErrorComp_10ns = SyncError_10ns;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    // No SW-compansation of RCF-Interval needed.
    // GSY must use Drift-Correction-Register of ERTEC200/SOC for RCF compensation.
    #elif defined (EDDI_CFG_REV5)
    if (pCycCount->Param.RCFInterval != 0)
    {
        // Reaction Time must be compensated.
        // SyncError is related to MasterTimeBase. (-> take RCF into account!)
        // SyncErrorComp(MasterTimeBase) = SyncErrorComp(LocalTimeBase) * RCF
        //                               = SyncErrorComp(LocalTimeBase) * 1 + (1 / RCFInterval)
        //                               = SyncErrorComp(LocalTimeBase) +   SyncErrorComp(LocalTimeBase) / RCFInterval
        //                               = SyncErrorComp(LocalTimeBase) +  (SyncErrorComp(LocalTimeBase) + 0.5 * RCFInterval ) DIV RCFInterval
        SyncErrorComp_10ns += ((SyncError_10ns + (pCycCount->Param.RCFInterval / 2)) / pCycCount->Param.RCFInterval);
    }
    #endif

    SyncErrorComp_10ns = (SyncErrorComp_10ns < 0)?(-(-SyncErrorComp_10ns % ((LSA_INT32)ActCycLen_10ns))):(SyncErrorComp_10ns % ((LSA_INT32)ActCycLen_10ns));

    /*----------------------------------------------------------*/
    /* correct the cycle-timer via cycle-length (only extend!)  */
    /*----------------------------------------------------------*/
    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetCounter, RCFInterval:0x%X", pCycCount->Param.RCFInterval);

    if (SyncErrorComp_10ns >= 0)
    {
        // Slave is ahead of Master: Slave has to slow down
        // --> prolong cycle for a short time
        // (NewCycleCounter is NOT changed)
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetCounter, IF SyncError:0x%X SyncErrorComp(pos):0x%X", SyncError_10ns, SyncErrorComp_10ns);
    }
    else
    {
        LSA_UINT32  carry;
        // Slave is behind the Master: Slave has to accelerate
        // --> shorten cycle for a short time. ATTENTION: As only the lengthening of the cycle is allowed,
        // it has to have a length of ((2UL * ActCycLen) - SyncError) for a short time!
        // --> Is handled in ExpandCycleLength.
        // Here the NewCycleCounter has to be adapted too.
        ADD_UI64_AND_UI32_WITH_CARRY(NewCounter.High, NewCounter.Low, pDDB->CycCount.Entity, carry);
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetCounter, ELSE SyncError:0x%X SyncErrorComp(neg):0x%X", 
                               SyncError_10ns, (-1)*SyncErrorComp_10ns);
        LSA_UNUSED_ARG(carry);
    }   

    *pSyncError_10ns = SyncErrorComp_10ns;

    // try to set SoftwareCouter
    // if failed (due to "Forbidden Jump") we need to do a 0x8000 - jump

    //Store CC for next step. Usage of EDDI_CycUpdateSoftwareCounter() is necessary to get all 48bits. 
    (void)EDDI_CycUpdateSoftwareCounter(pDDB, EDDI_NULL_PTR, EDDI_NULL_PTR);

    return EDDI_CycSetSoftwareCounter(pDDB,
                                      NewCounter.High,
                                      NewCounter.Low,
                                      pCycCount->Param.ReferenceTimeStampInTicks,
                                      LSA_TRUE);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycStateRunning()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycStateRunning( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                             EDDI_DDB_CYC_COUNTER_TYPE    *  const  pCycCount,
                                                             EDDI_CYC_COUNTER_EVENT_TYPE     const  Event )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycStateRunning->ClockCountValue:0x%X Event:0x%X", 
                           IO_R32(CLK_COUNT_VALUE), Event);

    switch (Event)
    {
        case EDDI_CYC_COUNTER_EVENT_SET:
        {
            LSA_INT32  SyncError_10ns;

            //try to set Counter and get Offset as ExpandedCycleLength

            if (EDDI_CycSetCounter(pDDB, pCycCount, &SyncError_10ns))
            {
                EDDI_CycInitCycleLengthExpansion(pDDB, SyncError_10ns);
                EDDI_CycExpandCycleLength(pDDB);
                pCycCount->State = EDDI_CYC_COUNTER_STATE_ADJUST_PHASE;
            }
            else
            {
                //do a 0x8000 - jump
                EDDI_CycBeginCycleCounterJump(pDDB, pCycCount);
                pCycCount->State = EDDI_CYC_COUNTER_STATE_JUMP;
            }

            break;
        }

        case EDDI_CYC_COUNTER_EVENT_NEWCYCLE:
        default:
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycStateRunning, Unhandled Event (0x%X) at state 0x%X", 
                Event, pCycCount->State); 
            EDDI_Excp("EDDI_CycStateRunning, Unhandled Event", EDDI_FATAL_ERR_EXCP, pCycCount->State, Event);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycStateJump()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycStateJump( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                          EDDI_DDB_CYC_COUNTER_TYPE        *  pCycCount,
                                                          EDDI_CYC_COUNTER_EVENT_TYPE  const  Event )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycStateJump->ClockCountValue:0x%X Event:0x%X", 
                           IO_R32(CLK_COUNT_VALUE), Event);

    switch (Event)
    {
        case EDDI_CYC_COUNTER_EVENT_NEWCYCLE:
        {
            LSA_INT32  SyncError_10ns;

            if (   (pCycCount->SyncGet.CycleCounterHigh < pCycCount->CycleCountEndOfJump.High)
                || (pCycCount->SyncGet.CycleCounterLow  < pCycCount->CycleCountEndOfJump.Low))
            {
                //Nothing to do: Keep waiting!
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycStateJump, WAIT curr_cc_low:0x%X EndOfJump_cc_low:0x%X",
                                      pCycCount->SyncGet.CycleCounterLow, pCycCount->CycleCountEndOfJump.Low);
            }
            else
            {
                //First half of Jump is over, now do the rest jump
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycStateJump, SET REST curr_cc_low:0x%X EndOfJump_cc_low:0x%X",
                                      pCycCount->SyncGet.CycleCounterLow, pCycCount->CycleCountEndOfJump.Low);

                if (EDDI_CycSetCounter(pDDB, pCycCount, &SyncError_10ns))
                {
                    EDDI_CycInitCycleLengthExpansion(pDDB, SyncError_10ns);
                    EDDI_CycExpandCycleLength(pDDB);
                    pCycCount->State = EDDI_CYC_COUNTER_STATE_ADJUST_PHASE;
                }
                else
                {
                    EDDI_FUNCTION_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycStateJump, Second half of jump failed. Event:0x%X, State:0x%X, CLK_COUNT_VALUE:0x%X, CycleCounterLow:0x%X, CycleCountEndOfJump.Low:0x%X ", 
                        Event, pCycCount->State, IO_R32(CLK_COUNT_VALUE),pCycCount->SyncGet.CycleCounterLow, pCycCount->CycleCountEndOfJump.Low); 
                    EDDI_Excp("EDDI_CycStateJump, Second half of jump failed", EDDI_FATAL_ERR_EXCP, pCycCount->State, Event);
                    return;
                }
            }
            break;
        }

        case EDDI_CYC_COUNTER_EVENT_SET:
        default:
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycStateJump, Unhandled Event (0x%X) at state 0x%X", 
                Event, pCycCount->State); 
            EDDI_Excp("EDDI_CycStateJump, Unhandled Event", EDDI_FATAL_ERR_EXCP, pCycCount->State, Event);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycStateAdjustPhase()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycStateAdjustPhase( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                 EDDI_DDB_CYC_COUNTER_TYPE        *  pCycCount,
                                                                 EDDI_CYC_COUNTER_EVENT_TYPE  const  Event )
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycStateAdjustPhase->ClockCountValue:0x%X Event:0x%X", 
                           IO_R32(CLK_COUNT_VALUE), Event);

    switch (Event)
    {
        case EDDI_CYC_COUNTER_EVENT_NEWCYCLE:
        {
            if (!EDDI_CycUpdateExpandedCycleLength(pDDB))
            {
                // nothing todo
                // no status change
            }
            else
            {
                // Hard Setting ready, Restore CycleLength
                EDDI_CycRestoreCycleLength(pDDB, pDDB->CycCount.CycleLength_10ns);

                // Change State BEFORE (!) callback
                pCycCount->State = EDDI_CYC_COUNTER_STATE_RUNNING;

                EDDI_RequestFinish(pCycCount->pHDB, pCycCount->pRQB, EDD_STS_OK);

                pCycCount->pHDB                             = 0;
                pCycCount->pRQB                             = 0;
                pCycCount->Param.ReferenceTimeStampInTicks  = 0;
                pCycCount->CycleCounterHigh                 = 0;
                pCycCount->CycleCounterLow                  = 0;
            }
            break;
        }

        case EDDI_CYC_COUNTER_EVENT_SET:
        default:
        {
            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycStateAdjustPhase, Unhandled Event (0x%X) at state 0x%X", 
                Event, pCycCount->State); 
            EDDI_Excp("EDDI_CycStateAdjustPhase, Unhandled Event", EDDI_FATAL_ERR_EXCP, pCycCount->State, Event);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycPassSoftwareCounterHigh()                */
/*                                                                         */
/* D e s c r i p t i o n: Rev5 only has 32bit CycleCtr. This function      */
/*                        writes the CycleCtrHigh (built in SW) to a known */
/*                        location in KRAM                                 */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycPassSoftwareCounterHigh( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                        LSA_UINT32               const  SetCycleCounterHigh)
{
    #if (defined (EDDI_CFG_REV5) && defined (EDDI_INTCFG_ISO_SUPPORT_ON))
    if (pDDB->Glob.LLHandle.pCTRLDevBaseAddr != EDDI_NULL_PTR)
    {
    LSA_UINT32 * const pCycleCtrHigh = &(pDDB->Glob.LLHandle.pCTRLDevBaseAddr->Arg.CycleCtrHigh);
        
        *pCycleCtrHigh = SetCycleCounterHigh;
    }
    #else
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(SetCycleCounterHigh);
    #endif
}

/***************************************************************************/
/* F u n c t i o n:       EDDI_CycSetSoftwareCounter()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CycSetSoftwareCounter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  SetCycleCounterHigh,
                                                                   LSA_UINT32               const  SetCycleCounterLow,
                                                                   LSA_UINT32               const  SetTimeStampInTicks,
                                                                   LSA_BOOL                 const  bAvoidCriticalJumps )
{
    volatile  LSA_UINT32  CycleBeginTimeStampInTicks, TestCycleBeginTimeStampInTicks;
    volatile  LSA_INT32   TotalDelayInTicks;          //Age of NewCycleCounter
    volatile  LSA_UINT32  CycleLengthInTicks;
    volatile  LSA_UINT32  NewCycleCounterLow, NewCycleCounterHigh;
    volatile  LSA_UINT32  BeforeCycleCounterLow;    //to detect forbidden jumps !
    volatile  LSA_UINT32  tmp;
    LSA_UINT32            TryCnt;
    LSA_UINT32            EffCycLenTicks, CCCorr;    
    LSA_UINT32            TempNewCycleCounterLow, TempNewCycleCounterHigh;
    LSA_UINT64            TempNewCycleCounter;

    EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter->cHigh:0x%X cLow:0x%X SetCycleCounterHigh:0x%X SetCycleCounterLow:0x%X SetTimeStamp:0x%X",
                          pDDB->CycCount.SyncGet.CycleCounterHigh, pDDB->CycCount.SyncGet.CycleCounterLow,
                          SetCycleCounterHigh, SetCycleCounterLow, SetTimeStampInTicks);

    EDDI_ENTER_COM_S();

    CycleLengthInTicks = IO_R32(CYCL_LENGTH) * EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS;

        /*----------------------------------------------------------------------------*/
        /* read a consistent pair of CLK_COUNT_BEGIN_VALUE and CYCL_COUNT_VALUE       */
        /* Protect by critical section to avoid interruption of reading of consistent */
        /* data.                                                                      */
        /*----------------------------------------------------------------------------*/

    TryCnt = 0;
    EDDI_ENTER_CRITICAL_S();
    do
    {
        CycleBeginTimeStampInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
        BeforeCycleCounterLow      = IO_R32(CYCL_COUNT_VALUE);
        tmp                        = IO_R32(CLK_COUNT_BEGIN_VALUE);
    }
    while ((CycleBeginTimeStampInTicks != tmp) && (++TryCnt < EDDI_CNS_TRY_CNT));
    EDDI_EXIT_CRITICAL_S();

    if (TryCnt >= EDDI_CNS_TRY_CNT)
    {
        EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSoftwareCounter: CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not read an consistent CycleCounterLow 0x%X and CycleBeginTimeStampInTicks 1:0x%X, 2:0x%X",
                               BeforeCycleCounterLow, CycleBeginTimeStampInTicks, tmp);
        EDDI_Excp("EDDI_CycSetSoftwareCounter: CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not read an consistent CycleCounterLow and CycleBeginTimeStampInTicks", EDDI_FATAL_ERR_ISR, TryCnt, 0);
        EDDI_EXIT_COM_S();
        return LSA_FALSE;
    }

    /* Note: We allow the SetTimeStamp to be newer as the CycleBegin timestamp.   */
    /*       With this we must adjust the Counter by adding or substracting       */
    /*       additional cycles depending on timestamp-difference.                 */
    /*                                                                            */
    /* Restriction:                                                               */
    /*       The Settimestamp difference must not exceed half of the 32-Bit range */
    /*       of the CycleBeginTimestamp to be able to do the proper adjustment.   */
    /*       If it execeeds, it is not possbile to differ between "newer" and     */
    /*       "older" and the adjustment went wrong! (wraparound of 32-Bit int     */
    /*       value!)                                                              */
    /*                                                                            */
    /* Additional Note to wraparound of TimeStamp:                                */
    /*       The calculation handles a wraparound of the timestamp right!         */
    /*       e.g. if the SetTimestamp is for example 0x100 and the                */
    /*       CycleBeginTimestamp is 0xFFFF0000 TotalDelay will get negative       */
    /*       which means the SetTimeStamp is newer.                               */
    /*       The same if SetTimeStamp is for example 0xFFFF0000 and the           */
    /*       CycleBeginTimestamp is 0x100. TotalDelay will get positive!          */

    TotalDelayInTicks = (LSA_INT32)(CycleBeginTimeStampInTicks - SetTimeStampInTicks);
    // add additional Cycles
    NewCycleCounterHigh = SetCycleCounterHigh;

    NewCycleCounterLow  = (LSA_UINT32)((LSA_INT32)SetCycleCounterLow + ((TotalDelayInTicks / (LSA_INT32)CycleLengthInTicks) * (LSA_INT32)pDDB->CycCount.Entity));
    if (TotalDelayInTicks > 0)
    {
        if (NewCycleCounterLow < SetCycleCounterLow) // handle overflow
        {
            NewCycleCounterHigh++;
        }
    }
    else //substraction of cycles..
    {
        if (NewCycleCounterLow > SetCycleCounterLow) // handle unverflow
        {
            NewCycleCounterHigh--;
        }
    }

    if (   (bAvoidCriticalJumps)
        && (pDDB->CRT.ProviderList.UsedEntries)
        && (((NewCycleCounterLow - BeforeCycleCounterLow) & 0xFFFF) >= (0xE000 - (pDDB->CRT.ProviderList.MaxUsedReduction * pDDB->CycCount.Entity))))
    {
        #if defined (EDDI_CFG_REV5)
        if ((BeforeCycleCounterLow & 0xFFFFUL) == ((NewCycleCounterLow + pDDB->CycCount.Entity) & 0xFFFFUL) )
        #else
        if (BeforeCycleCounterLow == (NewCycleCounterLow + pDDB->CycCount.Entity))
        #endif
        {
            /* Special handling if we already setup the CycleCounter and loop again        */
            /* because of a new cycle while the loop is running.                           */
            /*                                                                             */
            /* With the drift_correction register it is possible, that the timestamp       */
            /* difference between two cycles is not excactly a CycleLength! If the length  */
            /* is to less the TotalDelay maybe one cycle to short (rounding error).        */
            /* If this occurs the BeforeCycleCounterLow may be one cycle "greater" than    */
            /* NewCycleCounterLow which results in an "old window" error.                  */
            /*                                                                             */
            /* The only possible situations are that the BeforeCycleCounterLow is lower    */
            /* or equal to NewCycleCounterLow here!                                        */
            /*                                                                             */
            /* If the BeforeCycleCounterLow is one cycle above NewCycleCounterLow we have  */
            /* this jitter situation and setup NewCycleCounterLow to BeforeCycleCounterLow */
            /*                                                                             */

            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, Adjusting1 NewCycleCounterLow BeforeCycleCounterLow:0x%X NewCycleCounterLow:0x%X",
                                   BeforeCycleCounterLow, NewCycleCounterLow);

            /* Note: pDDB->CycCount.SyncGet has setting from previous set */

            NewCycleCounterLow  += pDDB->CycCount.Entity; //NewCycleCounterLow  = BeforeCycleCounterLow will not work for Rev5! (BeforeCCLow is only 16bit) 
            NewCycleCounterHigh = pDDB->CycCount.SyncGet.CycleCounterHigh;
            //wrap around
            if (NewCycleCounterLow < pDDB->CycCount.SyncGet.CycleCounterLow)
            {
                NewCycleCounterHigh++;
            }

            EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, Adjusting2 NewCycleCounterLow NewCycleCounterLow:0x%X NewCycleCounterHigh:0x%X",
                                   NewCycleCounterLow, NewCycleCounterHigh);
        }
        else
        {
            //volatile LSA_UINT32 const ReReadCycleCtr = IO_R32(CYCL_COUNT_VALUE);
            
            EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, forbidden jump, BeforeCycleCounterLow:0x%X NewCycleCounterLow:0x%X CycleBeginTimeStampInTicks:0x%X TotalDelay:%d CRT.ProviderList.MaxUsedReduction:0x%X CycCount.Entity:0x%X",
                                   BeforeCycleCounterLow, NewCycleCounterLow, CycleBeginTimeStampInTicks, TotalDelayInTicks, pDDB->CRT.ProviderList.MaxUsedReduction, pDDB->CycCount.Entity);
            EDDI_EXIT_COM_S();
            return LSA_FALSE;
        }
    }

    // We try max. n times to set a consistent pair of timestamp and CycleCounter within one Cycle
    for (TryCnt = 0; TryCnt < EDDI_CNS_TRY_CNT; TryCnt++)
    {
        EffCycLenTicks = pDDB->CycCount.EffCycLen_10ns * EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS;
        EDDI_ENTER_CRITICAL_S();
        {
            TestCycleBeginTimeStampInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
            CCCorr = ((TestCycleBeginTimeStampInTicks - CycleBeginTimeStampInTicks)/EffCycLenTicks) * pDDB->CycCount.Entity;
            TempNewCycleCounter = (LSA_UINT64)NewCycleCounterLow + (((LSA_UINT64)NewCycleCounterHigh)<<32) + CCCorr;

            TempNewCycleCounterLow  =  (LSA_UINT32)(TempNewCycleCounter & (LSA_UINT64)0xFFFFFFFF);
            TempNewCycleCounterHigh =  (LSA_UINT32)(TempNewCycleCounter >> 32);

            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            {
                //try to set consistent pair of registers
                IO_W32(CYCL_COUNT_VALUE      , TempNewCycleCounterLow );
                IO_W32(CYCL_COUNT_VALUE_47_32, (TempNewCycleCounterHigh & 0x0000FFFFUL)   );
            }
            #elif defined (EDDI_CFG_REV5)
            {
                IO_W32(CYCL_COUNT_VALUE, (TempNewCycleCounterLow & 0xFFFFUL));
            }
            #endif

            tmp = IO_R32(CLK_COUNT_BEGIN_VALUE);
        }
        EDDI_EXIT_CRITICAL_S();
        
       
        if (CCCorr != 0)
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, Calculation time:0x%X", CCCorr);
        }

        if (TestCycleBeginTimeStampInTicks != tmp) 
        {        
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, Set time:0x%X", tmp - TestCycleBeginTimeStampInTicks);
            continue; //could not set CC regs within 1 network cycle
        }
        else
        {
            //OK CycleBeginTimeStampInTicks and CycleCounter are a valid pair.

            NewCycleCounterHigh = TempNewCycleCounterHigh;
            NewCycleCounterLow  = TempNewCycleCounterLow;
            pDDB->CycCount.SyncGet.CycleCounterHigh = NewCycleCounterHigh;
            pDDB->CycCount.SyncGet.CycleCounterLow  = NewCycleCounterLow;

            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            {
                pDDB->CycCount.cOldHardwareHigh = (NewCycleCounterHigh & 0x0000FFFFL);
                pDDB->CycCount.cOldHardwareLow  =  NewCycleCounterLow; // 32 Bits
            }
            #elif defined (EDDI_CFG_REV5)
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycSetSoftwareCounter, CycleCtr written BeforeCycleCounterLow:0x%X NewCycleCounterLow:0x%X CycleBeginTimeStampInTicks:0x%X",
                                       BeforeCycleCounterLow, NewCycleCounterLow, CycleBeginTimeStampInTicks);
            }
            #endif

            EDDI_CycPassSoftwareCounterHigh(pDDB, (pDDB->CycCount.SyncGet.CycleCounterHigh<<16)+(pDDB->CycCount.SyncGet.CycleCounterLow>>16));  //pass sw-generated b16..b47

            EDDI_EXIT_COM_S();

            EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycSetSoftwareCounter, End cHigh:0x%X cLow:0x%X CycleBeginTimeStampInTicks:0x%X SetTimeStamp:0x%X",
                                  pDDB->CycCount.SyncGet.CycleCounterHigh, pDDB->CycCount.SyncGet.CycleCounterLow,CycleBeginTimeStampInTicks, SetTimeStampInTicks);

            if (TryCnt != 0)
            {
                EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycSetSoftwareCounter, success, but attempts > 0, Attempts:0x%X", TryCnt);
            }

            if (pDDB->CycCount.bCBFIsBinary)
            {
                LSA_UINT32 const CycleCtrLow = IO_R32(CYCL_COUNT_VALUE);
                
                if (0 != (CycleCtrLow % pDDB->CycCount.Entity) )
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSoftwareCounter, ERROR: CC misaligned. CycleCtr:0x%X, Entity:0x%X",
                                       CycleCtrLow, pDDB->CycCount.Entity);
                    EDDI_Excp("EDDI_CycSetSoftwareCounter, ERROR: CC misaligned", EDDI_FATAL_ERR_EXCP, CycleCtrLow, pDDB->CycCount.Entity);

                    return LSA_FALSE;
                }
            }
            #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12) || defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
            else
            {
                LSA_UINT64              CycleCounter64 ;
                EDDI_SYNC_GET_SET_TYPE  SyncGet;

                (void)EDDI_CycUpdateSoftwareCounter(pDDB, &SyncGet, EDDI_NULL_PTR);
                CycleCounter64 = ((LSA_UINT64)SyncGet.CycleCounterHigh << 32UL) + (LSA_UINT64)SyncGet.CycleCounterLow;                

                if (0 != ((LSA_INT64)CycleCounter64 % pDDB->CycCount.Entity) )
                {
                    EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSoftwareCounter, ERROR: CC misaligned. CycleCtr:0x%X%X, Entity:0x%X",
                                       SyncGet.CycleCounterHigh, SyncGet.CycleCounterLow, pDDB->CycCount.Entity);
                    EDDI_Excp("EDDI_CycSetSoftwareCounter, ERROR: CC misaligned", EDDI_FATAL_ERR_EXCP, SyncGet.CycleCounterLow, pDDB->CycCount.Entity);

                    return LSA_FALSE;
                }
            }
            #endif

            return LSA_TRUE;
        }
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycSetSoftwareCounter, unable to set valid CycleCounter");
    EDDI_Excp("EDDI_CycSetSoftwareCounter, unable to set valid CycleCounter", EDDI_FATAL_ERR_EXCP, 0, 0);
    EDDI_EXIT_COM_S();
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycUpdateSoftwareCounter()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_CYC_COUNTER_STATE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_CycUpdateSoftwareCounter( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                                 EDDI_SYNC_GET_SET_TYPE   *  const  pSyncGet,
                                                                                 LSA_UINT32               *  const  pExtPLLTimeStampInTicks )
{
    LSA_UINT32                   TryCnt;
    volatile  LSA_UINT32         CycleBeginValueInTicks;
    volatile  LSA_UINT32         tmp;
    EDDI_CYC_COUNTER_STATE_TYPE  State;  //For asynchronous SET_CYCLE_COUNT-Service

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycUpdateSoftwareCounter->");

    EDDI_ENTER_COM_S();

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    {
        volatile  LSA_UINT32  CurrentCycleCounterHigh, CurrentCycleCounterLow;

        /*----------------------------------------------------------------------------*/
        /* read a consistent pair of CLK_COUNT_BEGIN_VALUE and CYCL_COUNT_VALUE       */
        /* Protect by critical section to avoid interruption of reading of consistent */
        /* data.                                                                      */
        /*----------------------------------------------------------------------------*/

        TryCnt = 0;
        if (pExtPLLTimeStampInTicks)
        {
            EDDI_ENTER_CRITICAL_S();
            do
            {
                CycleBeginValueInTicks   = IO_R32(CLK_COUNT_BEGIN_VALUE);
                CurrentCycleCounterHigh  = (IO_R32(CYCL_COUNT_VALUE_47_32) & 0x0000FFFFL);
                CurrentCycleCounterLow   = IO_R32(CYCL_COUNT_VALUE);
                *pExtPLLTimeStampInTicks = IO_R32(CLK_COUNT_BEGIN_EXT);
                tmp                      = IO_R32(CLK_COUNT_BEGIN_VALUE);
            }
            while ((CycleBeginValueInTicks != tmp) && (++TryCnt < EDDI_CNS_TRY_CNT));
            EDDI_EXIT_CRITICAL_S();
        }
        else
        {
            EDDI_ENTER_CRITICAL_S();
            do
            {
                CycleBeginValueInTicks  = IO_R32(CLK_COUNT_BEGIN_VALUE);
                CurrentCycleCounterHigh = (IO_R32(CYCL_COUNT_VALUE_47_32) & 0x0000FFFFL);
                CurrentCycleCounterLow  = IO_R32(CYCL_COUNT_VALUE);
                tmp                     = IO_R32(CLK_COUNT_BEGIN_VALUE);
            }
            while ((CycleBeginValueInTicks != tmp) && (++TryCnt < EDDI_CNS_TRY_CNT));
            EDDI_EXIT_CRITICAL_S();
        }
        
        if (TryCnt >= EDDI_CNS_TRY_CNT)
        {
            EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycUpdateSoftwareCounter, CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not update consistent CycleCounter 0x%X, CycleBeginValueInTicks: 1:0x%X, 2:0x%X",
                                   CurrentCycleCounterLow, CycleBeginValueInTicks, tmp);
            EDDI_Excp("EDDI_CycUpdateSoftwareCounter, CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not update consistent CycleCounter", EDDI_FATAL_ERR_ISR, TryCnt, 0);
            /* EDDI_Excp will not return! */
            return (EDDI_CYC_COUNTER_STATE_TYPE)0;
        }


        pDDB->CycCount.SyncGet.CycleCounterLow = CurrentCycleCounterLow;
        //wrap around
        if (CurrentCycleCounterHigh < pDDB->CycCount.cOldHardwareHigh)
        {
            pDDB->CycCount.SyncGet.CycleCounterHigh++;
        }

        pDDB->CycCount.SyncGet.CycleCounterHigh = (pDDB->CycCount.SyncGet.CycleCounterHigh & 0xFFFF0000UL) | CurrentCycleCounterHigh;

        pDDB->CycCount.cOldHardwareHigh                   = CurrentCycleCounterHigh;
        pDDB->CycCount.SyncGet.CycleBeginTimeStampInTicks = CycleBeginValueInTicks;
    }
    #elif defined (EDDI_CFG_REV5)
    {
        volatile  LSA_UINT32  cLow, CurrentCycleCounterLow;
        LSA_UINT16            uiDiff;

        /*----------------------------------------------------------------------------*/
        /* read a consistent pair of CLK_COUNT_BEGIN_VALUE and CYCL_COUNT_VALUE       */
        /* Protect by critical section to avoid interruption of reading of consistent */
        /* data.                                                                      */
        /*----------------------------------------------------------------------------*/

        TryCnt = 0;
        if (pExtPLLTimeStampInTicks)
        {
            EDDI_ENTER_CRITICAL_S();
            do
            {
                CycleBeginValueInTicks   = IO_R32(CLK_COUNT_BEGIN_VALUE);
                CurrentCycleCounterLow   = IO_R32(CYCL_COUNT_VALUE); // Only 16 lower Bits are relevant
                *pExtPLLTimeStampInTicks = IO_R32(CLK_COUNT_BEGIN_EXT);
                tmp                      = IO_R32(CLK_COUNT_BEGIN_VALUE);
            }
            while ((CycleBeginValueInTicks != tmp) && (++TryCnt < EDDI_CNS_TRY_CNT));
            EDDI_EXIT_CRITICAL_S();
        }
        else
        {
            EDDI_ENTER_CRITICAL_S();
            do
            {
                CycleBeginValueInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
                CurrentCycleCounterLow = IO_R32(CYCL_COUNT_VALUE); // Only 16 lower Bits are relevant
                tmp                    = IO_R32(CLK_COUNT_BEGIN_VALUE);
            }
            while ((CycleBeginValueInTicks != tmp) && (++TryCnt < EDDI_CNS_TRY_CNT));
            EDDI_EXIT_CRITICAL_S();
        }
        if (TryCnt >= EDDI_CNS_TRY_CNT)
        {
            EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycUpdateSoftwareCounter, CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not update consistent CycleCounter 0x%X, CycleBeginValueInTicks: 1:0x%X, 2:0x%X",
                                   CurrentCycleCounterLow, CycleBeginValueInTicks, tmp);
            EDDI_Excp("EDDI_CycUpdateSoftwareCounter, CONSISTENCY Check - CRITICAL LOCK ISSUE - Could not update consistent CycleCounter", EDDI_FATAL_ERR_ISR, TryCnt, 0);
            /* EDDI_Excp will not return! */
            return (EDDI_CYC_COUNTER_STATE_TYPE)0;
        }

        // Rev5 has 16Bit-CycleCounter only
        uiDiff = (LSA_UINT16)((LSA_UINT16)CurrentCycleCounterLow - (LSA_UINT16)pDDB->CycCount.SyncGet.CycleCounterLow);

        cLow = pDDB->CycCount.SyncGet.CycleCounterLow + (LSA_UINT32)uiDiff;

        // check for overflow
        if (cLow < pDDB->CycCount.SyncGet.CycleCounterLow) /* overflow 32Bit cLow */
        {
            pDDB->CycCount.SyncGet.CycleCounterHigh++;
        }

        EDDI_CycPassSoftwareCounterHigh(pDDB, (pDDB->CycCount.SyncGet.CycleCounterHigh<<16)+(cLow>>16));  //pass sw-generated b16..b47

        pDDB->CycCount.SyncGet.CycleCounterLow            = cLow;
        pDDB->CycCount.SyncGet.CycleBeginTimeStampInTicks = CycleBeginValueInTicks;

        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CycUpdateSoftwareCounter, DDB.CycCount.CycleCounterLow:0x%X DDB.CycCount.CycleBeginTimeStampInTicks:0x%X",
                               pDDB->CycCount.SyncGet.CycleCounterLow, pDDB->CycCount.SyncGet.CycleBeginTimeStampInTicks);

        if (pDDB->CycCount.SyncGet.CycleCounterHigh)
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CycUpdateSoftwareCounter, DDB.CycCount.CycleCounterHigh:0x%X",
                                   pDDB->CycCount.SyncGet.CycleCounterHigh);
        }
    }
    #endif

    if (pSyncGet)
    {
        pSyncGet->CycleCounterHigh           = pDDB->CycCount.SyncGet.CycleCounterHigh;
        pSyncGet->CycleCounterLow            = pDDB->CycCount.SyncGet.CycleCounterLow;
        pSyncGet->CycleBeginTimeStampInTicks = pDDB->CycCount.SyncGet.CycleBeginTimeStampInTicks;
    }

    State = pDDB->CycCount.State;
    EDDI_EXIT_COM_S();

    return State;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CycBeginCycleCounterJump()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycBeginCycleCounterJump( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                                      EDDI_DDB_CYC_COUNTER_TYPE      *  pCycCount )
{
    EDDI_CYCLE_COUNTER_TYPE  NewCycleCounter;
    LSA_UINT32               carry;
    EDDI_SYNC_GET_SET_TYPE   SyncGet;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycBeginCycleCounterJump->");

    // 1) execute jump for a 0x8000 distance from the existing (unsynchronized but aligned!) cyclectr
    EDDI_ENTER_COM_S();
    NewCycleCounter.High = pDDB->CycCount.SyncGet.CycleCounterHigh;
    NewCycleCounter.Low  = pDDB->CycCount.SyncGet.CycleCounterLow;
    EDDI_EXIT_COM_S();

    #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12) || defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
    if (pDDB->CycCount.bCBFIsBinary)
    #endif
    {
        ADD_UI64_AND_UI32_WITH_CARRY(NewCycleCounter.High, NewCycleCounter.Low, 0x8000UL, carry);
    }
    #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12) || defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
    else
    {
        //Add between 0x8000 and 0x8000+CBF
        LSA_UINT64  CycleCounter64 = ((LSA_UINT64)NewCycleCounter.High << 32UL) + (LSA_UINT64)NewCycleCounter.Low + (LSA_UINT64)0x7FFF + (LSA_UINT64)pDDB->CycCount.Entity;
        //align CycleCounter to new CycleBaseFactor (must be a multiple)
        CycleCounter64 = ((CycleCounter64 / pDDB->CycCount.Entity)) * pDDB->CycCount.Entity;
        NewCycleCounter.High = (LSA_UINT32) (CycleCounter64 >> 32);
        NewCycleCounter.Low  = (LSA_UINT32) (CycleCounter64 & 0xFFFFFFFF);
    }
    #endif
    
    if (!EDDI_CycSetSoftwareCounter(pDDB,
                                    NewCycleCounter.High,
                                    NewCycleCounter.Low,
                                    pDDB->CycCount.SyncGet.CycleBeginTimeStampInTicks,
                                    LSA_TRUE))
    {
        //Setting SoftwareCounter failed due to "forbitten CycleCounter-Jump"
        EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycBeginCycleCounterJump, EDDI_CycSetSoftwareCounter failed. NewCycleCounter.High:0x%X, NewCycleCounter.Low:0x%X, CycleBeginTimeStampInTicks:0x%X",
                               NewCycleCounter.High, NewCycleCounter.Low, pCycCount->SyncGet.CycleBeginTimeStampInTicks);
        EDDI_Excp("EDDI_CycBeginCycleCounterJump, EDDI_CycSetSoftwareCounter failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    // 2) calculate end of waiting Time (depends on Maximum-Reduction used in CRT-Provider-List)
    //re-read current cyclectr
    (void)EDDI_CycUpdateSoftwareCounter(pDDB, &SyncGet, EDDI_NULL_PTR);
    pCycCount->CycleCountEndOfJump.High = SyncGet.CycleCounterHigh;
    pCycCount->CycleCountEndOfJump.Low  = SyncGet.CycleCounterLow;

    //to be sure that even the providerframe with the lowest reduction has been sent at least once since now,
    //add twice the lowest cycle-time!
    ADD_UI64_AND_UI32_WITH_CARRY(pCycCount->CycleCountEndOfJump.High, pCycCount->CycleCountEndOfJump.Low,
                                 2*(pDDB->CRT.ProviderList.MaxUsedReduction * pDDB->CycCount.Entity), carry);

    LSA_UNUSED_ARG(carry);
} 
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CycTimerCbfSoftwareCounter()                */
/*                                                                         */
/* D e s c r i p t i o n: Cyclic SW-Timer-Based CallBack                   */
/*                        to track changes of HW-Counter                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycTimerCbfSoftwareCounter( LSA_VOID  EDDI_LOCAL_MEM_ATTR  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_int_software_cycle_counter->");

    (void)EDDI_CycUpdateSoftwareCounter(pDDB, EDDI_NULL_PTR, EDDI_NULL_PTR);

    if (pDDB->PABorderLine.pBoLiKram->dw[pDDB->PABorderLine.act_index & 0x03] != EDDI_BORDERLINE_PAT)
    {
        //Application has written beyond the processimage!
        EDDI_Excp("EDDI_CycTimerCbfSoftwareCounter, PA Borderline!", EDDI_FATAL_ERR_ISR,
                  pDDB->PABorderLine.pBoLiKram->dw[0], pDDB->PABorderLine.pBoLiKram->dw[3]);
        return;
    }

    pDDB->PABorderLine.act_index++;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CycSetAdjustInterval()
 *
 *  Description:
 *
 *      AdjustIntervalTicks < 0: CycleTimeCounter nach AdjustInterval um 1 Tick anhalten
 *                               --> SyncSlave wird gebremst.
 *
 *      AdjustIntervalTicks > 0: CycleTimeCounter nach AdjustInterval um 1 Tick zusätzlich erhöhen
 *                               --> SyncSlave wird beschleunigt.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycSetAdjustInterval( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                          EDDI_DDB_CYC_COUNTER_TYPE      *  pCycCount,
                                                          LSA_INT32                  const  AdjustInterval )
{
    LSA_UINT32  PhaseCorrection;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycSetAdjustInterval->AdjustInterval:0x%X", AdjustInterval);

    PhaseCorrection = 0;

    if (AdjustInterval == 0)
    {
        // No adjustment by HW
        // Slopes of CycleTimeCounter-Register and ClockCountValue-Register are equal.
        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__CntCycl, 0);
        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__Inkrement, 0);
    }
    else if (AdjustInterval > 0)
    {
        // The CycleTimeCounter gets an additional 1ns increment  each AdjustIntervalNs[interpreted as Ns].
        // To get the same slope on a ERTEC machine
        // the 100Mhz CycleTimeCounter will be incremented by 10ns (1Tick !) in each AdjustIntervalNs[interpreted as Ticks !!]

        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__CntCycl,  (LSA_UINT32)AdjustInterval);
        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__Inkrement, 1);
    }
    else  // AdjustIntervalNs < 0
    {
        // The CycleTimeCounter gets an additional 1ns decrement each AdjustIntervalNs[interpreted as Ns].
        // To get the same slope on a ERTEC machine
        // the 100Mhz CycleTimeCounter will be decremented by 10ns (1Tick !) in each AdjustIntervalNs[interpreted as Ticks !!]

        // WARNING: When AdjustIntervalNs == -1
        //          --> CntCycl = 0 !!
        //          --> Has same effect as AdjustIntervalNs == 0

        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__CntCycl, (LSA_UINT32) ((-1) * AdjustInterval));
        EDDI_SetBitField32(&PhaseCorrection, EDDI_SER_PHASE_COR_BIT__Inkrement, 0);
    }

    pCycCount->AdjustInterval = AdjustInterval;

    //Update effective CycleLen
    EDDI_CycCalcEffectiveCycleLength(pDDB, pCycCount);

    IO_x32(PHASE_CORRECTION) = PhaseCorrection;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
/******************************************************************************
 *  Function:    EDDI_CycSetDriftCorrection()
 *
 *  Description:
 *
 *      DriftInterval < 0: ClockCounter nach DriftInterval um 1 Tick anhalten
 *                               --> SyncSlave wird gebremst.
 *
 *      DriftInterval > 0: ClockCounter nach DriftInterval um 1 Tick zusätzlich erhöhen
 *                               --> SyncSlave wird beschleunigt.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycSetDriftCorrection( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_INT32                const  DriftInterval )
{
    EDDI_DDB_CYC_COUNTER_TYPE  *  const  pCycCount = &pDDB->CycCount;
    LSA_UINT32                           DriftCorrectionReg;
    LSA_INT32                            CalcDriftInterval;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycSetDriftCorrection->DriftInterval:0x%X", DriftInterval);

    DriftCorrectionReg = 0;

    #if defined (EDDI_CFG_REV7)
    if (DriftInterval > 0)
    {
        //Rev7 has a correction of +-1ns each n*10ns
        CalcDriftInterval = (DriftInterval + 9)/10; //always round up! 
    }
    else
    {
        //Rev7 has a correction of +-1ns each n*10ns
        CalcDriftInterval = (DriftInterval - 9)/10; //always round down! 
    }
    #else
    //Rev6 has a correction of +-10ns each n*10ns
    CalcDriftInterval = DriftInterval;
    #endif

    if (DriftInterval == 0)
    {
        //No adjustment by HW
        //Slopes of CycleTimeCounter-Register and ClockCountValue-Register are equal.
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__CntCycl, 0);
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__Inkrement, 0);
    }
    else if (DriftInterval > 0)
    {
        //The CycleTimeCounter gets an additional 1ns increment  each DriftInterval[interpreted as Ns].
        //To get the same slope on a ERTEC machine
        //the 100Mhz CycleTimeCounter will be incremented by 10ns (1Tick !) in each DriftInterval[interpreted as Ticks !!]

        //   Rev6: alle DriftInterval*10ns eine Korrektur um +10ns
        //   Rev7: alle DriftInterval*10ns eine Korrektur um +1ns 
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__CntCycl,  (LSA_UINT32)CalcDriftInterval);
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__Inkrement, 1);
    }
    else //DriftInterval < 0
    {
        //The CycleTimeCounter gets an additional 1ns decrement each DriftInterval[interpreted as Ns].
        //To get the same slope on a ERTEC machine
        //the 100Mhz CycleTimeCounter will be decremented by 10ns (1Tick !) in each DriftInterval[interpreted as Ticks !!]

        //   Rev6: alle DriftInterval*10ns eine Korrektur um -10ns
        //   Rev7: alle DriftInterval*10ns eine Korrektur um -1ns 
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__CntCycl, (LSA_UINT32) ((-1) * CalcDriftInterval));
        EDDI_SetBitField32(&DriftCorrectionReg, EDDI_SER_PHASE_COR_BIT__Inkrement, 0);
    }

    pCycCount->DriftInterval = DriftInterval;

    //Update effective CycleLen
    EDDI_CycCalcEffectiveCycleLength(pDDB, pCycCount);

    IO_x32(DRIFT_CORRECTION) = DriftCorrectionReg;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/******************************************************************************
 *  Function:   EDDI_CycCalcEffectiveCycleLength()
 *
 *  Description: Berechnet die ZyklusLänge gemessen in Einheiten der TimestampClockDomäne.
 *
 *      AdjustInterval < 0: CycleTimeCounter nach AdjustInterval um 1 Tick anhalten
 *                          --> CycleTimer des SyncSlave wird gebremst.
 *                          --> Effektive CycleLen (bezogen auf TimestampClockDomäne) wird größer
 *
 *      AdjustInterval > 0: CycleTimeCounter nach AdjustInterval um 1 Tick zusätzlich erhöhen
 *                          --> CycleTimer des SyncSlave wird beschleunigt.
 *                          --> Effektive CycleLen (bezogen auf TimestampClockDomäne) wird kleiner
 *
 * Rev6/Rev7 only:
 *      DriftInterval  < 0: TimeStampCounter nach DriftInterval um 1 Tick anhalten
 *                          --> Effektive CycleLen (bezogen auf TimestampClockDomäne) wird kleiner
 *
 *      DriftInterval  > 0: TimeStampCounter nach DriftInterval um 1 Tick zusätzlich erhöhen
 *                          --> Effektive CycleLen (bezogen auf TimestampClockDomäne) wird größer
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycCalcEffectiveCycleLength( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                         EDDI_DDB_CYC_COUNTER_TYPE  *  const  pCycCount )
{
    LSA_INT32  PhaseCorrectionsPerCycle;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CycCalcEffectiveCycleLength->");

    if (pCycCount->AdjustInterval == 0)
    {
        PhaseCorrectionsPerCycle = 0;
    }
    else
    {
        //Calculate: AdjustmentsPerCycle = (CycleLength + 0.5 * AdjustInterval) / AdjustInterval
        //           (Adding 0.5 to compensate rounding effects of integer division)
        PhaseCorrectionsPerCycle = ((LSA_INT32)(pCycCount->CycleLength_10ns) + (pCycCount->AdjustInterval / 2)) / pCycCount->AdjustInterval;
    }

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycCalcEffectiveCycleLength, PhaseCorrectionsPerCycle:0x%X", PhaseCorrectionsPerCycle);

    pCycCount->EffCycLen_10ns = (LSA_UINT32)((LSA_INT32)pCycCount->CycleLength_10ns - PhaseCorrectionsPerCycle);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    {
        //ERTEC200 and SOC are using DriftCorrectionRegister to control speed of CycleTimeUnit
        //We have to respect this additionally

        LSA_INT32  DriftCorrectionsPerCycle;

        if (pCycCount->DriftInterval == 0)
        {
            DriftCorrectionsPerCycle = 0;
        }
        else
        {
            //Calculate: DriftCorrectionsPerCycle = (CycleLength + 0.5 * DriftInterval) / DriftInterval
            //           (Adding 0.5 to compensate rounding effects of integer division)
            DriftCorrectionsPerCycle = ((LSA_INT32)(pCycCount->CycleLength_10ns) + (pCycCount->DriftInterval / 2)) / pCycCount->DriftInterval;
        }

        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycCalcEffectiveCycleLength, DriftCorrectionsPerCycle:0x%X", DriftCorrectionsPerCycle);

        pCycCount->EffCycLen_10ns = (LSA_UINT32)((LSA_INT32)pCycCount->EffCycLen_10ns + DriftCorrectionsPerCycle);
    }
    #endif

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CycCalcEffectiveCycleLength, EffCycLen_10ns:0x%X", pCycCount->EffCycLen_10ns);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CycGetRealPhaseOffsetTicks()
 *
 *              Return-Value:           Phaseoffset between *local* cycle begin and
 *                                      *external* reference cycle begin.
 *                                      (triggered via ExtPLL-Signal)
 *
 *                                      Return-Value < 0 means: local clock is too fast
 *                                      Return-Value > 0 means: local clock is too slow
 *
 *                                      Minimum:  -0.5 * CycleLength_In_Ticks
 *                                      Maximum:   0.5 * CycleLength_In_Ticks
 */
LSA_INT32  EDDI_LOCAL_FCT_ATTR  EDDI_CycGetRealPhaseOffsetTicks( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  BeginLocalInTicks, BeginExtInTicks;
    LSA_INT32   ExtOffset_10ns, HalfCycleLength_10ns ;

    BeginLocalInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
    BeginExtInTicks   = IO_R32(CLK_COUNT_BEGIN_EXT);

    ExtOffset_10ns  = (LSA_INT32)((BeginLocalInTicks - (BeginExtInTicks - EXT_PLL_IN_INTERNAL_DELAY_10NS*EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS)))/EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS;

    //max. 10 cycle allowed
    if (   (ExtOffset_10ns > ((LSA_INT32)pDDB->CycCount.CycleLength_10ns * 10))
        || (ExtOffset_10ns < ((-1)*(LSA_INT32)pDDB->CycCount.CycleLength_10ns * 10)))
    {
        // ExtPLL-Signal is not available or ExtPll-Period too big
        return EDDI_CYC_NO_EXT_PLL_IN_SIGNAL;
    }

    //Shorten ExtOffset
    if (ExtOffset_10ns >= (LSA_INT32)pDDB->CycCount.CycleLength_10ns)
    {
        ExtOffset_10ns = ExtOffset_10ns % ((LSA_INT32)pDDB->CycCount.CycleLength_10ns);
    }
    else if (ExtOffset_10ns < ((-1)*(LSA_INT32)pDDB->CycCount.CycleLength_10ns))
    {
        //For negative arguments we have to assert *symmetrical* modulo-operation:
        ExtOffset_10ns = (-1)*(((-1)*ExtOffset_10ns) % ((LSA_INT32)pDDB->CycCount.CycleLength_10ns));
    }
    else
    {
        //nothing todo
    }

    //Shift ExtOffset into right quadrant:
    HalfCycleLength_10ns = (LSA_INT32)pDDB->CycCount.CycleLength_10ns / 2;
    if (ExtOffset_10ns >= HalfCycleLength_10ns)
    {
        ExtOffset_10ns -= (LSA_INT32)pDDB->CycCount.CycleLength_10ns;
    }
    else if (ExtOffset_10ns < ((-1)*HalfCycleLength_10ns))
    {
        ExtOffset_10ns += (LSA_INT32)pDDB->CycCount.CycleLength_10ns;
    }
    else
    {
        // nothing todo
    }

    return ExtOffset_10ns;
}
/*---------------------- end [subroutine] ---------------------------------*/


// Symmetrical integer division
// Actually following calculation is made:
// numerator DIV denominator + 0.5
// = (numerator + denominator / 2) DIV denominator
#define EDDI_CALC_SYM_DIVISION_INT(_numerator, _denominator ) ( ( (_numerator) + ( (_denominator) / 2 ) ) / (_denominator) )

// #define EDDI_CALC_SYM_DIVISION_UINT(_numerator, _denominator ) ( ( (_numerator) + ( (_denominator) / 2UL ) ) / (_denominator) )

/******************************************************************************
 *  Function:   EDDI_CycInitCycleLengthExpansion()
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycInitCycleLengthExpansion( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_INT32                const  SyncError_10ns )
{
    EDDI_CYCLE_EXPANSION_TYPE  *  const  pCycleExpansion = &pDDB->CycCount.CycleExpansion;
    LSA_UINT32                    const  EffCycLen_10ns  = pDDB->CycCount.EffCycLen_10ns;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycInitCycleLengthExpansion->SyncError_10ns:0x%X EffCycLen_10ns:0x%X", 
                          SyncError_10ns, EffCycLen_10ns);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycInitCycleLengthExpansion, PhaseCorReg:0x%X DriftCorReg:0x%X",
                          IO_R32(PHASE_CORRECTION), IO_R32(DRIFT_CORRECTION));
    #elif defined (EDDI_CFG_REV5)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycInitCycleLengthExpansion, PhaseCorReg:0x%X",
                          IO_R32(PHASE_CORRECTION));
    #endif

    if (pDDB->NewCycleReductionFactor == 0)
    {
        EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycInitCycleLengthExpansion, pDDB->NewCycleReductionFactor == 0");
        EDDI_Excp("EDDI_CycInitCycleLengthExpansion, pDDB->NewCycleReductionFactor == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pCycleExpansion->WorstCaseCycleReduction  = pDDB->NewCycleReductionFactor * 2UL; //For worstcase we assume double of nominal NewCycleReductionFactor
    pCycleExpansion->WorstCaseInterval_10ns   = pCycleExpansion->WorstCaseCycleReduction *
                                                pDDB->CycCount.Entity * EDDI_CRT_CYCLE_LENGTH_GRANULARITY;
    pCycleExpansion->LastNewCycleBeginInTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
    pCycleExpansion->Time_10ns                = 0;

    if (SyncError_10ns >= 0)
    {
        // Slave is ahead of Master: Slave has to slow down
        // --> prolong cycle for a short time
        pCycleExpansion->TotalExpansion = SyncError_10ns;
        pCycleExpansion->RestExpansion  = pCycleExpansion->TotalExpansion;
    }
    else
    {
        // Slave is behind the Master: Slave has to accelerate
        // --> shorten cycle for a short time. ATTENTION: As only the lengthening of the cycle is allowed,
        // it has to have a length of ((2UL * ActCycLen) - |SyncError| ) for a short time!
        // --> Is handled in ExpandCycleLength.
        // Here the NewCycleCounter has to be adapted too.
        pCycleExpansion->TotalExpansion = (LSA_INT32)EffCycLen_10ns + SyncError_10ns;   // SyncError is negative here !!
        pCycleExpansion->RestExpansion  = pCycleExpansion->TotalExpansion;
    }

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycInitCycleLengthExpansion<-RestExpansion:0x%X TotalExpansion:0x%X",
                          pCycleExpansion->RestExpansion, pCycleExpansion->TotalExpansion);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CycExpandCycleLength()
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycExpandCycleLength( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                           ExpandedCycleLength_10ns;
    EDDI_CYCLE_EXPANSION_TYPE  *  const  pCycleExpansion = &pDDB->CycCount.CycleExpansion;
    LSA_RESULT                           Result;

    ExpandedCycleLength_10ns = (LSA_UINT32)((LSA_INT32)pDDB->CycCount.CycleLength_10ns +
                                            EDDI_CALC_SYM_DIVISION_INT (pCycleExpansion->RestExpansion, (LSA_INT32)pCycleExpansion->WorstCaseCycleReduction));

    if (ExpandedCycleLength_10ns < pDDB->CycCount.CycleLength_10ns)
    {
        ExpandedCycleLength_10ns = pDDB->CycCount.CycleLength_10ns;
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CycExpandCycleLength, ExpandedCycleLength < pDDB->CycCount.CycleLength_10ns, :0x%X :0x%X", ExpandedCycleLength_10ns, pDDB->CycCount.CycleLength_10ns);
    }

    // === Switch off Application-Clock ===
    // Technical Background:
    // When the ENABLE_MULTICYCLE_APL_CLOCKS-Bit is set, the HW-internal
    // Application-Clock-Counter acts as a free running counter.
    // To avoid a phase-difference between CycleTimeCounter and ApplicationClockCounter
    // during CycleLength expansion, we before need to reset the ApplicationClockCounter.

    //signal stopping of xpllout to application
    EDDI_SIGNAL_SENDCLOCK_CHANGE(pDDB->hSysDev, pDDB->CycCount.Entity, EDDI_SENDCLOCK_CHANGE_XPLLOUT_DISABLED);

    Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_STOP, 0 /*irrelevant*/, 0 /*irrelevant*/);  
    if (EDD_STS_OK != Result)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycExpandCycleLength, EDDI_IsoCtrlDefaultApplclock Result:0x%X", Result);
        EDDI_Excp("EDDI_CycExpandCycleLength, EDDI_IsoCtrlDefaultApplclock", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    // Now change CycleLength
    IO_W32(CYCL_LENGTH, ExpandedCycleLength_10ns);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycExpandCycleLength<-ExpandedCycleLength_10ns set to:0x%X", ExpandedCycleLength_10ns);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function: EDDI_CycRestoreCycleLength()
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CycRestoreCycleLength( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  CycleLength_10ns )
{
    LSA_UINT32  ClkStatus;
    LSA_RESULT  Result;

    //Check Status of Clock
    ClkStatus = IO_R32(CLK_STATUS);

    if (   (ClkStatus & ENABLE_MULTICYCLE_APL_CLOCKS)
        && (ClkStatus & START_APP_CLK))
    {
        //START_APP_CLK must have been switched off one cycle before (by using EDDI_CycExpandCycleLength()
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycRestoreCycleLength, unexpected Clkstatus: 0x%X", ClkStatus);
        EDDI_Excp("EDDI_CycRestoreCycleLength, unexpected Clkstatus", EDDI_FATAL_ERR_EXCP, ClkStatus, 0);
        return;
    }

    //Now change CycleLength
    IO_W32(CYCL_LENGTH, CycleLength_10ns);

    //Reactivate ApplicationClock
    Result = EDDI_IsoCtrlDefaultApplclock(pDDB, EDDI_ISO_DEFAULT_APPLCLK_START, 0 /*irrelevant*/, 0 /*irrelevant*/);  
    if (EDD_STS_OK != Result)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycRestoreCycleLength, EDDI_IsoCtrlDefaultApplclock Result: 0x%X", Result);
        EDDI_Excp("EDDI_CycRestoreCycleLength, EDDI_IsoCtrlDefaultApplclock", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    //signal starting of xpllout to application
    EDDI_SIGNAL_SENDCLOCK_CHANGE(pDDB->hSysDev, pDDB->CycCount.Entity, EDDI_SENDCLOCK_CHANGE_XPLLOUT_ENABLED);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycRestoreCycleLength<-Offset after hard setting, CycleLength:0x%X", CycleLength_10ns);
}
/*---------------------- end [subroutine] ---------------------------------*/


#define EDDI_CYCLE_REST_ERROR_TICKS                              10  // Limit needed in EDDI_CycUpdateExpandedCycleLength()
#define EDDI_CYCLE_MAX_DURATION_FOR_HARD_SETTING  (500 * 1000 * 100)  // 500ms limit duration of hard setting.

/******************************************************************************
 *  Function:   EDDI_CycUpdateExpandedCycleLength()
 *
 *  return:     LSA_TRUE:  if RestError is small enough
 *              LSA_FALSE: if RestError is still too big
 */
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CycUpdateExpandedCycleLength( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_CYCLE_EXPANSION_TYPE  *  const  pCycleExpansion           = &pDDB->CycCount.CycleExpansion;
    LSA_UINT32                    const  CurrentNewCycleBeginTicks = IO_R32(CLK_COUNT_BEGIN_VALUE);
    LSA_UINT32                    const  NominalCycLen_10ns        = pDDB->CycCount.CycleLength_10ns;
    LSA_UINT32                    const  EffCycLen_10ns            = pDDB->CycCount.EffCycLen_10ns;
    LSA_UINT32                           ClkStatus;
    LSA_UINT32                           DeltaTs_10ns;

    //Check Status of Clock
    ClkStatus = IO_R32(CLK_STATUS);

    if (   (ClkStatus & ENABLE_MULTICYCLE_APL_CLOCKS)
        && (ClkStatus & START_APP_CLK))
    {
        //START_APP_CLK must have been switched off one cycle before (by using EDDI_CycExpandCycleLength()
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CycUpdateExpandedCycleLength, unexpected Clkstatus: 0x%X", ClkStatus);
        EDDI_Excp("EDDI_CycUpdateExpandedCycleLength, unexpected Clkstatus", EDDI_FATAL_ERR_EXCP, ClkStatus, 0);
        return LSA_FALSE;
    }

    //Calculate DeltaTime since last change of CycleLen
    DeltaTs_10ns = (CurrentNewCycleBeginTicks - pCycleExpansion->LastNewCycleBeginInTicks)/EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS;

    if (DeltaTs_10ns < (pCycleExpansion->WorstCaseInterval_10ns / 2UL))
    {
        //Too early --> no Update needed.
        //          --> keep current CycleLength-Expansion
        return LSA_FALSE;
    }

    pCycleExpansion->RestExpansion = pCycleExpansion->RestExpansion - (LSA_INT32)(DeltaTs_10ns % EffCycLen_10ns);

    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycUpdateExpandedCycleLength, DeltaTs:0x%X (DeltaTs modulo EffCycLen):0x%X pCycleExpansion->RestExpansion:0x%X CurrentNewCycleBeginTs:0x%X",
                          DeltaTs_10ns, (DeltaTs_10ns % EffCycLen_10ns),
                          pCycleExpansion->RestExpansion, CurrentNewCycleBeginTicks);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycUpdateExpandedCycleLength, PhaseCorReg:0x%X DriftCorReg:0x%X",
                          IO_R32(PHASE_CORRECTION), IO_R32(DRIFT_CORRECTION));
    #elif defined (EDDI_CFG_REV5)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CycUpdateExpandedCycleLength, PhaseCorReg:0x%X",
                          IO_R32(PHASE_CORRECTION));
    #endif

    pCycleExpansion->Time_10ns += DeltaTs_10ns;
    if (   (pCycleExpansion->RestExpansion < EDDI_CYCLE_REST_ERROR_TICKS)
        || (pCycleExpansion->Time_10ns     > (LSA_UINT32)EDDI_CYCLE_MAX_DURATION_FOR_HARD_SETTING))
    {
        return LSA_TRUE;
    }
    else
    {
        LSA_UINT32  CorrectionCycLen_10ns;

        //Calculate length of following cycles to compensate current Offset.
        //Do NOT use the whole offset for expansion of Cyclelength !
        //The size of the correction depends on the size of WorstCaseCycleReduction.
        //The bigger the exptected CycleReduction
        //the smaller must be the corrected offset per cycle.

        CorrectionCycLen_10ns = NominalCycLen_10ns + (LSA_UINT32) EDDI_CALC_SYM_DIVISION_INT(pCycleExpansion->RestExpansion, (LSA_INT32)pCycleExpansion->WorstCaseCycleReduction);

        //Now change CycleLength
        IO_W32(CYCL_LENGTH, CorrectionCycLen_10ns);

        //Store Timestamp
        pCycleExpansion->LastNewCycleBeginInTicks = CurrentNewCycleBeginTicks;

        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_cyc.c                                                   */
/*****************************************************************************/


