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
/*  F i l e               &F: eddi_crt_sb_hw.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  *HW-Variante* of Inner Functions of              */
/*                          Remote-Provider-Surveillance (RPS)               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  29.10.2007  JS    Quick Hack for error fix (see JS_NOFIX) -> Verfiy!     */
/*  08.02.2008  JS    changed LEVEL_ERROR to LEVEL_NOTE_HIGH because its a   */
/*                    possible state not a ERROR.                            */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#include "eddi_dev.h"
#include "eddi_crt_ext.h"
#include "eddi_crt_check.h"
#include "eddi_crt_dfp.h"
#endif

#define EDDI_MODULE_ID     M_ID_CRT_SB_HW
#define LTRC_ACT_MODUL_ID  108

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_USE_SW_RPS)
#if defined (M_ID_CRT_SB_HW) //satisfy lint!
#endif
#else


/******************************************************************************
 *  Function:   EDDI_CRTConsumerGetKRAMDataStatus()
 *
 *  Description:Return the DataStatus of the specified pConsumer
 *
 *  Arguments:  pConsumer(THIS) : the related Consumer
 *              pDataStatus(OUT): The DataStatus of the Current Consumer
 *
 *  Return:     LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerGetKRAMDataStatus( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                  LSA_UINT8                      *  const  pTransferStatus,
                                                                  EDDI_SER_DATA_STATUS_TYPE      *  const  pDataStatus,
                                                                  LSA_UINT16                     *  const  pCycleCnt )
{
    EDDI_CRT_DATA_APDU_STATUS    *  pAPDU;
    EDDI_SER10_LL1_ACW_RCV_TYPE     Hw1;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerGetKRAMDataStatus->");
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        EDDI_DFPConsumerGetDS(pDDB, pConsumer, pDataStatus, pCycleCnt);
        *pTransferStatus = 0;
        return;
    }

    switch (pConsumer->LowerParams.ListType)
    {
        case EDDI_LIST_TYPE_ACW:
        {
            //ACW- or UDP-Communication
            Hw1.Value = pConsumer->pLowerCtrlACW->pKramCW->AcwRcv.Hw1.Value;

            if ((pConsumer->pLowerCtrlACW) && EDDI_GetBitField32(Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__IIIB))
            {
                EDDI_Excp("EDDI_CRTConsumerGetKRAMDataStatus no HW_IIIB-IF selected, but IIIB-bit in ACW set!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
            else
            {
                //Rev7/5: only knows 1 buffer, Rev6:either IF_1BSW or IF_SINGLE
                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;
            }

            break;
        }

        case EDDI_LIST_TYPE_FCW:
        {
            //FCW-Communication
            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_CRTConsumerGetKRAMDataStatus ListType", EDDI_FATAL_ERR_EXCP, pConsumer->LowerParams.ListType, 0);
            return;
        }
    }

    if (pConsumer->LowerParams.bKRAMDataBufferValid)
    {
        *pDataStatus       = pAPDU->Detail.DataStatus; 
        *pCycleCnt         = EDDI_IRTE2HOST16(pAPDU->Detail.CycleCnt);
        *pTransferStatus   = pAPDU->Detail.TransferStatus;
    }
    else
    {
        *pDataStatus       = 0; 
        *pCycleCnt         = 0;
        *pTransferStatus   = 0;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTRpsCalculateIndEntry() (HelperFunction only)
 *
 *  Description:Calculates one IndicationEventEntry depending on SBEntry,
 *              current DataStatus and old DataStatus of one Consumer
 *
 *  Arguments:  pConsumer       : the current Consumer
 *              SBEntryValue    : Value of the related Provider-ScoreBoard-Entry
 *                                (only lowest 4 Bits)
 *
 *  Return:     ...
 */
EDDI_CRT_EVENT_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsCalculateIndEntry( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                        LSA_UINT32                  const  SBEntryValue,
                                                                        EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB)
{
    EDDI_SER_DATA_STATUS_TYPE   DataStatus;
    EDDI_SER_DATA_STATUS_TYPE   DeltaStatus;
    LSA_UINT16                  CycleCnt;
    LSA_UINT32                  PendingEvent;
    EDDI_CRT_EVENT_TYPE         NewEvent; 
    LSA_UINT8                   TransferStatus = 0;
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LTRC_LEVEL_TYPE             tracelevel = LSA_TRACE_LEVEL_NOTE_HIGH;
    LSA_UINT32                  CycleCntLow = 0;
    #endif

    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    volatile  EDDI_CRT_DATA_APDU_STATUS  *  const  pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;
    #endif

    DeltaStatus = 0;

    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    if (EDDI_PS_COND__MISS(SBEntryValue))
    {
        tracelevel = LSA_TRACE_LEVEL_WARN;
        CycleCntLow = IO_R32(CYCL_COUNT_VALUE);
    }
    #endif

    // get the current DataStatus
    EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer, &TransferStatus, &DataStatus, &CycleCnt);
    pConsumer->pSB->CurrentDataStatus = DataStatus;

    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        EDDI_CRT_TRACE_08(pDDB->Glob.TraceIdx, tracelevel, "EDDI_CRTRpsCalculateIndEntry, > DGFrameID:0x%X SBEntryValue:0x%X APDUStatus:0x%X/%X/%X pConsumer->ScoreBoardStatus:0x%X, SFPos:%d, CurrCycleCnt:0x%X", 
            (LSA_UINT32)pConsumer->LowerParams.FrameId, SBEntryValue, TransferStatus, DataStatus, CycleCnt, pConsumer->ScoreBoardStatus, EDDI_CRT_GET_SFPOS(pConsumer), CycleCntLow);
    }
    else
    {
        EDDI_CRT_TRACE_08(pDDB->Glob.TraceIdx, tracelevel, "EDDI_CRTRpsCalculateIndEntry, > NonDGFrameID:0x%X SBEntryValue:0x%X APDUStatus:0x%X/%X/%X pConsumer->ScoreBoardStatus:0x%X, bIsPF:%d, CurrCycleCnt:0x%X", 
            (LSA_UINT32)pConsumer->LowerParams.FrameId, SBEntryValue, TransferStatus, DataStatus, CycleCnt, pConsumer->ScoreBoardStatus, EDDI_DFPConsumerIsPF(pConsumer), CycleCntLow);
    }

    // If DS-Changed is set, check for following events:
    // - RUN        0->1 / STOP            1->0
    // - STATION_OK 0->1 / STATION_FAILURE 1->0
    // - PRIMARY    0->1 / BACKUP          1->0
    // For this, we compare bits 1,4 and 5 of the old DataStatus with the new one.
    
    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), SBEntryValue, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_EVAL);


    if (EDDI_PS_BIT__DataStateCh(SBEntryValue))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDI_CRTRpsCalculateIndEntry, DSChange-Bit is set; OldDataStatus, DataStatus -> pConsumer->pSB->OldDataStatus:0x%X DataStatus:0x%X",
                          pConsumer->pSB->OldDataStatus, DataStatus);

        DeltaStatus = (LSA_UINT8)(pConsumer->pSB->OldDataStatus ^ DataStatus);

        EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), 0,DeltaStatus, EDDI_CONSTRACE_STATE_DSChanged);

        // remember the old DataStatus here !
        pConsumer->pSB->OldDataStatus = DataStatus;

        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRpsCalculateIndEntry, DeltaStatus:0x%X", DeltaStatus);

        if (EDDI_DS_BIT__State_backup0_primary1(DeltaStatus))
        {
            // EDD_CSRT_DSTAT_BIT_STATE changed
            if (EDDI_DS_BIT__State_backup0_primary1(DataStatus))
            {
                pConsumer->PrimaryEventStartTime_ticks = IO_R32(CLK_COUNT_VALUE);   //first record event start time
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_PRIMARY -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_BACKUP, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_BACKUP -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
        }

        if (EDDI_DS_BIT__Stop0_Run1(DeltaStatus))
        {
            // EDD_CSRT_DSTAT_BIT_STOP_RUN changed
            if (EDDI_DS_BIT__Stop0_Run1(DataStatus))
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_RUN, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_RUN -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STOP, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_STOP -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
        }

        if (EDDI_DS_BIT__StationFailure(DeltaStatus))
        {
            // EDD_CSRT_DSTAT_BIT_STATION_FAILURE   changed
            if (EDDI_DS_BIT__StationFailure(DataStatus))
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STATION_OK, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_STATION_OK -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STATION_FAILURE, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_STATION_FAILURE -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
        }

        if (EDDI_DS_BIT__Redundancy(DeltaStatus))
        {
            // EDD_CSRT_DSTAT_BIT_STATE changed
            if (EDDI_DS_BIT__Redundancy(DataStatus))
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_NO_PRIMARY_AR_EXISTS -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. NewEvent EVENT_PRIMARY_AR_EXISTS -> DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
        }

        // diagnostic check if HW-change-detection had a spike
        if (!(EDDI_DS_BIT__Stop0_Run1(DeltaStatus)             || EDDI_DS_BIT__StationFailure(DeltaStatus) ||
              EDDI_DS_BIT__State_backup0_primary1(DeltaStatus) || EDDI_DS_BIT__DataValid(DeltaStatus)      ||
              EDDI_DS_BIT__Redundancy(DeltaStatus)))
        {
            // We had a spike in one the of the four HW-relevant Bits of DataStatus
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_CRTRpsCalculateIndEntry, WARNING: Spike detected in TimerSB; DeltaStatus,DataStatus -> DeltaStatus:0x%X DataStatus:0x%X",
                              DeltaStatus, DataStatus);
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_CRTRpsCalculateIndEntry, SB_entry, OldData WARNING: Spike detected in TimerSB; SBEntryValue,OldDataStatus -> SBEntryValue:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                              SBEntryValue, pConsumer->pSB->OldDataStatus);
        }
    }
    else
    {
        // DataStatus did not Change --> nevertheless use current DataStatus for calculcation, because something can be received in
        // the meantime. Otherwise an erroneous MISS will be generated further down. 
        //DataStatus = pConsumer->pSB->OldDataStatus;

        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDI_CRTRpsCalculateIndEntry, DSChange-Bit is NOT set; OldDataStatus, 0 -> pConsumer->pSB->OldDataStatus:0x%X",
                          pConsumer->pSB->OldDataStatus);
    }

    if (pConsumer->pSB->UkStatus == EDDI_CRT_CONS_UK_COUNTING)
    {
        // Normal Consumers must have both set ProdState AND DataValid.
        // Aux    Consumers must have set ProdState only. Datavalid must be ignored.
        //PSChanged is not taken into account because DataValid has to be checked her also
        if (   (EDDI_PS_BIT__ProdState(SBEntryValue))
            && (EDDI_DS_BIT__DataValid(DataStatus) || (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER)))
        {
            // Consumer received and valid --> EVENT_AGAIN
            EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_Again);

            PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT);
            EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);

            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "CRTRpsCalc.. UKState EVENT_AGAIN; DeltaStatus, SBEntryValue -> DeltaStatus:0x%X SBEntryValue:0x%X",
                              DeltaStatus, SBEntryValue);

            // OK: UKCounting can be switched off again
            pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
        }
        else
        {
            if (   EDDI_PS_COND__AGAIN(SBEntryValue)
                && (EDDI_SER_RED_STATUS_INIT_VALUE == TransferStatus) )
            {
                //no MISS! HW detected an AGAIN, while EDDI tried to write a consistent pair 
                //of TransferStatus/TimerSBValue in EDDI_SERResetRedundancyAndTsbEntry1B!
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "CRTRpsCalc.. UKState spurious again, TransferStatus INIT; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X SBEntryValue:0x%X",
                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), SBEntryValue);
            }
            else
            {
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_Miss);

                // WatchdogCounter timed out --> EVENT_MISS
                PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);

                if (   (EDDI_PS_BIT__ProdState(SBEntryValue))
                    && !(EDDI_DS_BIT__DataValid(DataStatus)) )
                {
                    EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                      "CRTRpsCalc.. UKState EVENT_MISS DataStatusInvalid; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X PendingEvent:0x%X DeltaStatus:0x%X SBEntryValue:0x%X",
                                      pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), PendingEvent, DeltaStatus, SBEntryValue);
                }
                else
                {
                    EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                      "CRTRpsCalc.. UKState EVENT_MISS; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X PendingEvent:0x%X DeltaStatus:0x%X SBEntryValue:0x%X",
                                      pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), PendingEvent, DeltaStatus, SBEntryValue);
                }
                // UKCounting can be switched off again
                pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
            }
        }
        
        NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
        return NewEvent;
    }

    // normal treatment
    // for Aux Consumers we do not check Datavalid-Bit !!
    /* ProdState=AGAIN AND DataValid */
    /* For AGAIN recognition only the ProdState bit may be checked, as after Set2UK IRTE does not set the ProdStateChanged bit.
       (PSO in the TimerSB will only be set if the WDT is triggered after it has already been 0.) */
    if  ( EDDI_PS_BIT__ProdState(SBEntryValue)
         && (EDDI_DS_BIT__DataValid(DataStatus) || (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER)))
    {
        EDDI_CONS_SCOREBOARD_SM_TRIGGER_TYPE scoreboardTrigger = EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT;

        /* producer changed -> hw generates again */
        if(EDDI_PS_BIT__ProdStateCh(SBEntryValue))
        {
            scoreboardTrigger = EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT;
        }

        // Consumer received and valid --> EVENT_AGAIN
        PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, scoreboardTrigger);

        if (PendingEvent)
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
            if (EDD_CSRT_CONS_EVENT_AGAIN == PendingEvent)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_AGAIN; DeltaStatus:0x%X SBEntryValue:0x%X",
                                  DeltaStatus, SBEntryValue);

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_MSAgain);
            }
            else if(EDD_CSRT_CONS_EVENT_MISS == PendingEvent)
            {
                EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_MISS; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X PendingEvent:0x%X DeltaStatus:0x%X SBEntryValue:0x%X",
                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), PendingEvent, DeltaStatus, SBEntryValue);

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_OKMiss);
            }
            else
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_???? -> AGAIN expected; DeltaStatus:0x%X SBEntryValue:0x%X Event:0x%X",
                                  DeltaStatus, SBEntryValue, PendingEvent);
            }
        }
    }
    else if /* (DataValid changed to Invalid AND normal Consumer)
               OR ProdState=MISS */
            (   (     (!EDDI_DS_BIT__DataValid(DataStatus))
                   && (EDDI_DS_BIT__State_backup0_primary1(DataStatus)) 
                   && (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER))
             || (!EDDI_PS_BIT__ProdState(SBEntryValue)))
    {
        // WatchdogCounter timed out --> EVENT_MISS
        PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);

        if (PendingEvent)
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
            if (   (EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED == PendingEvent)
                || (EDD_CSRT_CONS_EVENT_MISS == PendingEvent) )
            {
                EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_MISS; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X PendingEvent:0x%X DeltaStatus:0x%X SBEntryValue:0x%X",
                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), PendingEvent, DeltaStatus, SBEntryValue);

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_OKMiss);
            }
            else
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_???? -> MISS expected; DeltaStatus:0x%X SBEntryValue:0x%X Event:0x%X",
                                  DeltaStatus, SBEntryValue, PendingEvent);
            }
        }
    }

    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
    {
        NewEvent = pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent;
    }
    else
    {
        NewEvent = pConsumer->PendingIndEvent;
    }

    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LSA_UNUSED_ARG(tracelevel);
    LSA_UNUSED_ARG(CycleCntLow);  //LINT
    #endif
    return NewEvent;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif


/*****************************************************************************/
/*  end of file eddi_crt_sb_hw.c                                             */
/*****************************************************************************/
