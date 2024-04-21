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
/*  F i l e               &F: eddi_crt_sb_sw.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n: *SW-Variante* of Inner Functions of               */
/*                         Remote-Provider-Surveillance (RPS)                */
/*                         Needed due to Hardware-Bug in                     */
/*                         TimerScoreboard of ERTEC400                       */
/*                         See ARTS-Request Nr. 186315                       */
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

#define EDDI_MODULE_ID     M_ID_CRT_SB_SW
#define LTRC_ACT_MODUL_ID  109

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_CRT_SB_SW) //satisfy lint!
#endif

#if defined (EDDI_CFG_USE_SW_RPS)

#include "eddi_dev.h"

#include "eddi_crt_ext.h"
#include "eddi_crt_check.h"


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsCheckDataStatesEventDummy( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                     EDDI_CRT_RPS_TYPE        *  const  pRps )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsCheckDataStatesEventDummy->");

    LSA_UNUSED_ARG(pRps); //satisfy lint!
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTRpsCalculateIndEntry() //HelperFunction only
 *
 *  Description: Calculates one IndicationEventEntry depending on SBEntry,
 *               current DataStatus and old DataStatus of one Consumer
 *
 *  Arguments:   pConsumer       : the current Consumer
 *               SBEntryValue    : Value of the related Provider-ScoreBoard-Entry
 *                                 (only lowest 4 Bits)
 *
 *  Return:
 */
EDDI_CRT_EVENT_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsCalculateIndEntry( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                        LSA_UINT32                  const  SBEntryValue,
                                                                        EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_SER_DATA_STATUS_TYPE  DataStatus;
    EDDI_SER_DATA_STATUS_TYPE  DeltaStatus;
    LSA_UINT8                  TransferStatus = 0;
    LSA_UINT16                 CycleCnt;
    LSA_UINT32                 PendingEvent;
    EDDI_CRT_EVENT_TYPE        NewEvent;

    volatile  EDDI_CRT_DATA_APDU_STATUS  *  const  pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRpsCalculateIndEntry, > SBEntryValue:0x%X APDUStatus:0x%X", SBEntryValue, *((LSA_UINT32 *)pAPDU));

    // Read DataStatus
    DataStatus = pAPDU->Detail.DataStatus;
    TransferStatus   = pAPDU->Detail.TransferStatus;
    pConsumer->pSB->CurrentDataStatus = DataStatus;

    // Get all changed Bits
    DeltaStatus = (LSA_UINT8)((pConsumer->pSB->OldDataStatus ^ DataStatus) & SER_DATA_STATUS_USED_BITS);

    if (DeltaStatus)
    {
        pConsumer->pSB->OldDataStatus = DataStatus;
    }

    //CycleCnt = *((LSA_UINT16 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer));
    EDDI_HOST2IRTE16x(CycleCnt, pAPDU->Detail.CycleCnt);

    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), SBEntryValue, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_EVAL);

    // Check if we have a change in DataStatus or in ProviderStatus
    if (EDDI_DS_BIT__DataValid(DeltaStatus) || EDDI_PS_BIT__ProdStateCh(SBEntryValue))
    {
        if /* consumer has been received before */
           (EDDI_CONS_SCOREBOARD_STATE_AGAIN == pConsumer->ScoreBoardStatus)
        {
            // EVENT: ProvState 1->0
            if (EDDI_PS_BIT__ProdStateCh(SBEntryValue) && (!EDDI_PS_BIT__ProdState(SBEntryValue)))
            {
                if (pConsumer->pSB->UkStatus == EDDI_CRT_CONS_UK_OFF)
                {
                    // ScoreBoardStatus lost connection -> EVENT_MISS
                    PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
                }
                pConsumer->pSB->Invalid = LSA_FALSE;

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_VALPS1to0);
            }
            // EVENT: DataValid 1->0 // changes in datavalid are NOT relevant for AuxConsumer
            else if (    (!EDDI_DS_BIT__DataValid(DataStatus)) 
                      && (EDDI_DS_BIT__State_backup0_primary1(DataStatus)) 
                      && (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER))
            {
                if (EDDI_CRT_CONS_UK_OFF == pConsumer->pSB->UkStatus)
                {
                    // ScoreBoardStatus lost connection -> EVENT_MISS
                    PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
                }
                pConsumer->pSB->Invalid = LSA_TRUE;
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_VALDS1to0);
            }
            // EVENT ProvState 0->1  ( can happen after ConsumerCtrl with SetToUkState = TRUE)
            else if (EDDI_PS_BIT__ProdStateCh(SBEntryValue) && (EDDI_PS_BIT__ProdState(SBEntryValue)))
            {
                if /* Scoreboard reports "AGAIN", but the consumer has been set to UK */
                   (   (pConsumer->pSB->UkStatus != EDDI_CRT_CONS_UK_OFF)
                    && (EDDI_SER_RED_STATUS_INIT_VALUE == pAPDU->Detail.TransferStatus) )
                {
                    //This can happen under the following condition:
                    //- the scoreboard records a series of miss/again without beeing exchanged.
                    //- the frame is set to UK, directly followed by scoreboard evaluation
                    //- the currently recorded state is "AGAIN", because the SB does not stick at "MISS"
                    //In this case, do nothing and wait for the UK-timer to timeout
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry < WARNING inconsistent state:EDDI_CRT_CONS_RX_STS_VALID/TrStat=0x0F/PS=1");
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_AGAIN0xF);

                    NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
                    return NewEvent;
                }
                else
                {
                    // new Code reports AGAIN at once and switches off UkCounter
                    // ScoreBoardStatus received-> AGAIN_EVENT
                    PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT);
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);

                    pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
                    pConsumer->pSB->UkCountdown = 0;

                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_VALPS0to1);
                }
            }
            else
            {
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry < WARNING quiet return from EDDI_CRT_CONS_RX_STS_VALID");

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_VALNoEvent);

                NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
                return NewEvent;
            }
        }
        else if /* consumer has never before been received */
               (!pConsumer->pSB->Invalid)
        {
            //EVENT: ProvState 0->1
            if (EDDI_PS_BIT__ProdStateCh(SBEntryValue) && EDDI_PS_BIT__ProdState(SBEntryValue))
            {
                if (EDDI_DS_BIT__DataValid(DataStatus) || (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER))
                {
                    //only report AGAIN-INDICATION, if DataValid is OK and UkCountdown is not running

                    if /* Scoreboard reports "AGAIN", but the consumer has been set to UK */
                       (   (pConsumer->pSB->UkStatus != EDDI_CRT_CONS_UK_OFF)
                        && (EDDI_SER_RED_STATUS_INIT_VALUE == pAPDU->Detail.TransferStatus) )
                    {
                        //This can happen under the following condition:
                        //- the scoreboard records a series of miss/again without beeing exchanged.
                        //- the frame is set to UK, directly followed by scoreboard evaluation
                        //- the currently recorded state is "AGAIN", because the SB does not stick at "MISS"
                        //In this case, do nothing and wait for the UK-timer to timeout
                        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry < WARNING inconsistent state:EDDI_CRT_CONS_RX_STS_VALID/TrStat=0x0F/PS=1");
                        EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_AGAIN0xF);
                         
                        NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
                        return NewEvent;
                    }
                    else
                    {
                        // new Code reports AGAIN at once and switches off UkCounter
                        PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT);
                        EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);

                        pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
                        pConsumer->pSB->UkCountdown = 0;
                    }
                    pConsumer->pSB->Invalid = LSA_FALSE;
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_NOAgainIfNotUk);
                }
                else
                {
                    pConsumer->pSB->Invalid = LSA_TRUE;
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_NONoAgainPS0to1);
                    EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                      "EDDI_CRTRpsCalculateIndEntry.. NO EVENT_AGAIN -> DataStatusInvalid; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X DeltaStatus:0x%X SBEntryValue:0x%X",
                                      pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), DeltaStatus, SBEntryValue);
                }
            }
        }
        else /* consumer has been received before, but with APDUStatus.DataValid=0 */
        {
            // EVENT: ProvState 1->0
            if (EDDI_PS_BIT__ProdStateCh(SBEntryValue) && !EDDI_PS_BIT__ProdState(SBEntryValue))
            {
                pConsumer->pSB->Invalid = LSA_FALSE;
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_INVPS1to0);
            }
            // EVENT: DataValid 0->1
            else if (   (EDDI_DS_BIT__DataValid(DeltaStatus) && EDDI_DS_BIT__DataValid(DataStatus))
                     && (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER))
            {
                if /* Scoreboard reports "AGAIN", but the consumer has been set to UK */
                   (   (pConsumer->pSB->UkStatus != EDDI_CRT_CONS_UK_OFF)
                    && (EDDI_SER_RED_STATUS_INIT_VALUE == pAPDU->Detail.TransferStatus) )
                {
                    //This can happen under the following condition:
                    //- the scoreboard records a series of miss/again without beeing exchanged.
                    //- the frame is set to UK, directly followed by scoreboard evaluation
                    //- the currently recorded state is "AGAIN", because the SB does not stick at "MISS"
                    //In this case, do nothing and wait for the UK-timer to timeout
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry < WARNING inconsistent state:EDDI_CRT_CONS_RX_STS_VALID/TrStat=0x0F/PS=1");
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_AGAIN0xF);

                    NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
                    return NewEvent;
                }
                else
                {
                    // new Code reports AGAIN at once and switches off UkCounter
                    PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT);
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);

                    pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
                    pConsumer->pSB->UkCountdown = 0;
                }

                pConsumer->pSB->Invalid = LSA_FALSE;

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_INVDV0to1);
            }
            else
            {
                // This case only happens if the Scoreboard detects a spike either
                // in DatavalidBit or ProducerStateBit;
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry, < WARNING quiet return from EDDI_CRT_CONS_RX_STS_INVALID");
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_INVNoEvent);

                NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
                return NewEvent;
            }
        }
    }

    // Handle UnknownState (Betrifft nur die Indications MISS/AGAIN
    if (pConsumer->pSB->UkStatus == EDDI_CRT_CONS_UK_TIMED_OUT)
    {
        //consumer has never been received after "set to unknown"
        if (pAPDU->Detail.TransferStatus == EDDI_SER_RED_STATUS_INIT_VALUE)
        {
            EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_0xF);

            // durch Activate und SetToUnknownState kann der Scoreboard Interrupt für den
            // Ausfall unterdrückt werden -> Fall wird jetzt durch den TransferStatus detektiert
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry, TransferStatus = INITVAL");
            PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
            EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
        }
        else
        {
            if // state has been set to AGAIN above
               (EDDI_CONS_SCOREBOARD_STATE_AGAIN == pConsumer->ScoreBoardStatus )
            {
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_Again);
    
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRpsCalculateIndEntry, *** EDDI_CRT_CONS_UK_TIMED_OUT Valid");
                // ScoreBoardStatus nimmt Verbindung wieder auf --> EVENT_AGAIN
                PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT);
                EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
            }
            else
            {
                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_Miss);
    
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRpsCalculateIndEntry, *** EDDI_CRT_CONS_UK_TIMED_OUT InValid");
                // ScoreBoardStatus         --> EVENT_MISS
                PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, CycleCnt, TransferStatus, pDDB);
            }
        }
        // Reset UkStatus
        pConsumer->pSB->UkStatus    = EDDI_CRT_CONS_UK_OFF;
        pConsumer->pSB->UkCountdown = 0;
        pConsumer->pSB->Invalid = LSA_FALSE;

    }

    // If DS-Changed is set, check for following events:
    // - RUN        0->1 / STOP            1->0
    // - STATION_OK 0->1 / STATION_FAILURE 1->0
    // - PRIMARY    0->1 / BACKUP          1->0
    // - SR_APPEARS 0->1 / SR_DISAPPEARS   1->0

    // Here the UnknownState has no influence on the generation of indications!
    if (DeltaStatus & (  EDD_CSRT_DSTAT_BIT_STATE           | EDD_CSRT_DSTAT_BIT_STOP_RUN
                             | EDD_CSRT_DSTAT_BIT_STATION_FAILURE | EDD_CSRT_DSTAT_BIT_REDUNDANCY))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsCalculateIndEntry, DeltaStatus:0x%X", DeltaStatus);

        EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pAPDU)), 0, DeltaStatus, EDDI_CONSTRACE_STATE_DSChanged);

        if (EDDI_DS_BIT__State_backup0_primary1(DeltaStatus))
        {
            // EDD_CSRT_DSTAT_BIT_STATE changed
            if (EDDI_DS_BIT__State_backup0_primary1(DataStatus))
            {
                pConsumer->PrimaryEventStartTime_ticks = IO_R32(CLK_COUNT_VALUE);   //first record event start time
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_PRIMARY, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_BACKUP, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_BACKUP, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
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
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_RUN, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STOP, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_STOP, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
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
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_STATION_OK, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STATION_FAILURE, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_STATION_FAILURE, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
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
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_NO_PRIMARY_AR_EXISTS, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
            else
            {
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS, CycleCnt, TransferStatus, pDDB);
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRpsCalculateIndEntry, EVENT_PRIMARY_AR_EXISTS, DataStatus:0x%X pConsumer->pSB->OldDataStatus:0x%X",
                                  DataStatus, pConsumer->pSB->OldDataStatus);
            }
        }
    }

    // For Debugging:
    if (pConsumer->PendingIndEvent & EDD_CSRT_CONS_EVENT_AGAIN)
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRpsCalculateIndEntry, *** EVENT_AGAIN FrameID:0x%X APDU:0x%X PendingEvent:0x%X EventStatus:0x%X ",
                          (LSA_UINT32)pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), 
                          pConsumer->PendingIndEvent, pConsumer->EventStatus);
    }

    if (pConsumer->PendingIndEvent & (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED))
    {
        EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                          "EDDI_CRTRpsCalculateIndEntry, *** EVENT_MISS; ConsID:0x%X FrameID:0x%X PendingEvent:0x%X APDUStatus:0x%X EventStatus:0x%X",
                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, *((LSA_UINT32 *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer)), pConsumer->PendingIndEvent, pConsumer->EventStatus);
    }

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsCalculateIndEntry");

    EDDI_CRT_CONSTRACE_TRACE(pConsumer, 0, 0,pConsumer->PendingIndEvent, EDDI_CONSTRACE_STATE_PendingEvent);

    NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
    return NewEvent;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_USE_SW_RPS


/*****************************************************************************/
/*  end of file eddi_crt_sb_sw.c                                             */
/*****************************************************************************/
