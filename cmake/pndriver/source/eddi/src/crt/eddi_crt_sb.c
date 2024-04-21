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
/*  F i l e               &F: eddi_crt_sb.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  *General* Functions of                           */
/*                          Remote-Provider-Surveillance (RPS)               */
/*                          - RPS-Statemachine                               */
/*                          - Indication - Firing                            */
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

#include "eddi_ser_ext.h"
#include "eddi_crt_ext.h"
#include "eddi_crt_check.h"
#include "eddi_crt_dfp.h"

#if defined (EDDI_CFG_USE_SW_RPS)
#include "eddi_crt_sb_sw_tree.h"
#endif

#define EDDI_MODULE_ID     M_ID_CRT_SB
#define LTRC_ACT_MODUL_ID  107

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/*****************************************************************************/
/*  Locals                                                                   */
/*****************************************************************************/
static const LSA_UINT8 DebugInfoRTC3PortStatus[EDD_IRT_PORT_ACTIVE_TX_RX+1] = {EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_NOT_SUPPORTED, 
                                                                               EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_INACTIVE,
                                                                               EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_TX,
                                                                               EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_NOT_SUPPORTED,
                                                                               EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_NOT_SUPPORTED,
                                                                               EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_RX_TX};

static const LSA_UINT8 DebugInfoRTC2PortStatus[EDD_RTCLASS2_INACTIVE+1] = {EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_NOT_SUPPORTED,
                                                                           EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_ACTIVE,
                                                                           EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_INACTIVE};

static const LSA_UINT8 DebugInfoRTC3RemotePortStatus[EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX+1] = {EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_OFF,
                                                                                                    EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_TX,
                                                                                                    EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_RX_TX};

/*****************************************************************************/
/*  Protos                                                                   */
/*****************************************************************************/
static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsNewIndResourceEvent( EDDI_CRT_RPS_TYPE       * const pRps,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsStartEvent( EDDI_CRT_RPS_TYPE       const * const pRps,
                                                             EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsStopEvent( EDDI_CRT_RPS_TYPE       const * const pRps,
                                                            EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsSBChangedEvent( EDDI_CRT_RPS_TYPE       const * const pRps,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsDoEvaluation( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsFillCommonDebugInfo( EDDI_LOCAL_DDB_PTR_TYPE                   const pDDB,
                                                                       EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE * const pDebugInfoCommon);

#define EDDI_CRT_SB_EVAL_MASK 0x66666666UL


/******************************************************************************
 *  Function:   EDDI_CRTRpsDoEvaluation() (Helper-Function)
 *
 *  Description:Does the Evaluation of all new Scoreboard-Entries:
 *              all new SB-Entries
 *              will be translated to a CRT-Indication and sent to the user
 *              and sets the
 *              Enable Change-Bit to allow new SB-Change-Interrupts.
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     -
 *
 *  Exception:  EDDI_ERR_SB_INDICATION_PARAM
 *              EDDI_ERR_SB_CONSUMER_INCONSISTENT
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsDoEvaluation( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                               Status;
    LSA_UINT32                               SBEntryValue;
    LSA_UINT32                               ConsumerID;
    LSA_UINT32                               ConsumerCtrl;
    EDDI_CRT_CONSUMER_PTR_TYPE               pConsumer;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = pDDB->pLocal_CRT;
    EDDI_SER10_PROV_SCORB_PTR_TYPE           pDevSBChangeList;
    LSA_INT32                                PendingEventMaxConsID  = -1;
    LSA_UINT32                        const  ConsumerIDStart = pDDB->CRT.MetaInfo.RTC123IDStart;
    LSA_UINT32                        const  ConsumerIDEnd = pDDB->CRT.ConsumerList.LastIndexRTC123;
    LSA_UINT32                               SBOffsetStart;
    LSA_UINT32                        const  SBOffsetEnd = ConsumerIDEnd >> 3;
    LSA_UINT32                            *  pSBWord;
    LSA_UINT32                               SBWord;
    LSA_UINT16                               NextIndexRTC12Event = 0;
    EDDI_CRT_CONSRTC12EVENTLIST_TYPE      *  pConsRTC12EventID;

    #if defined (EDDI_CFG_ENABLE_FAST_SB_TIMING)
    volatile LSA_UINT32  Time = IO_R32(CLK_COUNT_VALUE);
    #endif
    
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsDoEvaluation");

    //get the actual pDevSBProv;
    ConsumerCtrl = IO_x32(PROD_CTRL);
    // evaluate the SB that is available for EDDI
    pDevSBChangeList = pCRTComp->Rps.SBList.pDevSBChangeList[EDDI_GetBitField32(ConsumerCtrl,
                                                                                 EDDI_SER_PROD_CTRL_BIT__PsSCOREBSelect)];

    // Check all DataStatus-Bytes of all active Consumers

    /*
      Organisation of CSB:
              bit7 bit0 | bit7 bit0 | bit7 bit0 | bit7 bit0 
                Byte0   |   Byte1   |   Byte2   |   Byte3
      ConsID   1  |  0  |  3  |  2  |  5  |  4  |  7  |  6
      
             31 30 29 28|27 26 25 24|23 22 21 20|19 18 17 16|15 14 13 12|11 10 09 08|07 06 05 04|03 02 01 00
      ==> Little Endian:
      ConsID      7           6           5           4           3           2           1           0

      ==> Big Endian:
      ConsID      1           0           3           2           5           4           7           6
    */    

    SBOffsetStart     = ConsumerIDStart >> 3;
    pSBWord           = ((LSA_UINT32 *)(void *)pDevSBChangeList) + SBOffsetStart;
    pConsRTC12EventID = pCRTComp->ConsumerList.pConsRTC12EventID;

    do
    {
        SBWord= *pSBWord;

        //check 8 entries at once
        if (SBWord & EDDI_CRT_SB_EVAL_MASK)
        {
            #if defined (EDDI_CFG_LITTLE_ENDIAN)
            ConsumerID = SBOffsetStart*8;
            #else
            ConsumerID = SBOffsetStart*8 + 6;
            #endif
            do
            {
                SBEntryValue = (SBWord & 0xFUL);
                if (EDDI_PS_BIT__ProdDataStateCh(SBEntryValue))
                {
                    //check for overlapping RTC123-ranges
                    if (    (ConsumerID >= ConsumerIDStart) 
                        &&  (ConsumerID <= ConsumerIDEnd) )
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                          "EDDI_CRTRpsDoEvaluation INFO: Checking Scoreboardentry; ConsumerID:0x%X SBEntryValue.Reg:0x%X",
                                          ConsumerID, SBEntryValue);

                        pConsumer = &pCRTComp->ConsumerList.pEntry[ConsumerID];
                        if (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_ACTIVE)
                        {
                            if (EDDI_LIST_TYPE_FCW == pConsumer->LowerParams.ListType)
                            {
                                //RTC3 consumers get handled at once                            
                                if (EDDI_CRTRpsCalculateIndEntry(pConsumer, SBEntryValue, pDDB))
                                {
                                    // OK we have a new Event for this Consumer
                                    if (PendingEventMaxConsID < (LSA_INT32)ConsumerID)
                                    {
                                        PendingEventMaxConsID = (LSA_INT32)ConsumerID;
                                    }
                                }

                                // trace the ComsumerID //
                                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsDoEvaluation->Event generated for ConsumerID:0x%X, SB:0x%X", ConsumerID, SBEntryValue);
                            }
                            else //EDDI_LIST_TYPE_ACW
                            {
                                //RTC12 consumers get delayed and stored in a list
                                pConsRTC12EventID->SBEntryValue = SBEntryValue;
                                pConsRTC12EventID->pEntry       = pConsumer;
                                pConsRTC12EventID++;
                                NextIndexRTC12Event++;
                            }
                        }
                    }                        
                }
                SBWord = SBWord >> 4;

                #if defined (EDDI_CFG_LITTLE_ENDIAN)
                ConsumerID++;
                #else
                if (ConsumerID & 1)
                {
                    //High nibble of current byte ==> select next byte, low nibble
                    ConsumerID = ConsumerID - 3;
                }
                else
                {
                    //Low nibble of current byte ==> select high nibble
                    ConsumerID++;
                }
                #endif
            }
            while (SBWord);
        }
        
        pSBWord++;
        SBOffsetStart++;
    }  
    while (SBOffsetStart <= SBOffsetEnd);

    //now process stored RTC12 consumers 
    if (NextIndexRTC12Event)
    {
        pConsRTC12EventID = pCRTComp->ConsumerList.pConsRTC12EventID;
        do
        {
            pConsumer = pConsRTC12EventID->pEntry;
            if (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_ACTIVE)
            {
                ConsumerID = pConsumer->ConsumerId;
                SBEntryValue = pConsRTC12EventID->SBEntryValue;

                if (EDDI_CRTRpsCalculateIndEntry(pConsumer, SBEntryValue, pDDB))
                {
                    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        if (PendingEventMaxConsID < (LSA_INT32)pConsumer->IRTtopCtrl.pAscConsumer->ConsumerId)
                        {
                            PendingEventMaxConsID = (LSA_INT32)pConsumer->IRTtopCtrl.pAscConsumer->ConsumerId;
                        }
                    }
                    else
                    {
                        if (PendingEventMaxConsID < (LSA_INT32)ConsumerID)
                        {
                            PendingEventMaxConsID = (LSA_INT32)ConsumerID;
                        }
                    }
                }

                // trace the ConsumerID //
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsDoEvaluation->Event generated for ConsumerID:0x%X, SB:0x%X", ConsumerID, SBEntryValue);
            }
            pConsRTC12EventID++;
            NextIndexRTC12Event--;
        }
        while (NextIndexRTC12Event);
    }

    Status = EDDI_CRTRpsFillAndSendIndication(pDDB, (LSA_UINT32)PendingEventMaxConsID, pDDB->CRT.Rps.MaxPendingDGCons, LSA_TRUE /*bDirectIndication*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRTRpsInformStateMachine(&pCRTComp->Rps, pDDB, EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE); 
    }
    pDDB->CRT.Rps.MaxPendingDGCons = 0xFFFFFFFF;
    
    //now IRTE may change the SB again if new changes are found
    EDDI_SERScoreBoardEnableChange(pDDB);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsDoEvaluation");
    
    #if defined (EDDI_CFG_ENABLE_FAST_SB_TIMING)
    Time =  IO_R32(CLK_COUNT_VALUE) - Time;
    pDDB->Glob.LLHandle.pCTRLDevBaseAddr->Time = Time;
    if (Time > pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMax) {pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMax = Time;};
    if (Time < pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMin) {pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMin = Time;};
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTRpsInit()
 *
 *  Description:Initializes the Remote-Provider-Surveillance.
 *              Typically called in EDDI_CRTInitComponent().
 *              The Status is set to STOPPED.
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     EDD_STS_OK if Start was OK
 *              EDD_STS_ERR_SEQUENCE else.
 *
 *  Exception:  EDDI_ERR_SB_INIT_NO_MEM1
 *              EDDI_ERR_SB_INIT_NO_MEM2
 *              EDDI_ERR_SB_INIT_NO_MEM3
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsInit( EDDI_CRT_RPS_TYPE        *  const  pRps,
                                                EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UINT16            TimerScorebBlocks;
    LSA_UINT16            length;
    LSA_UINT16            length_block, length_all;
    LSA_UINT16            mod_rest;
    LSA_UINT32            ret;
    LSA_UINT16            ConsumerCnt;
    EDDI_MEM_BUF_EL_H  *  pMemHeader;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsInit->");

    ConsumerCnt = (LSA_UINT16)(pDDB->CRT.MetaInfo.AcwConsumerCnt + pDDB->CRT.MetaInfo.FcwConsumerCnt);  //do not take DFP-consumers into account!

    //ConsumerCnt = (LSA_UINT16)EDD_CFG_CSRT_MAX_INDICATIONS;

    if (ConsumerCnt == 0)
    {
        pRps->Status = EDDI_CRT_RPS_STS_NOT_AVAILABLE;
        return;
    }

    //calc count of Timer-Scoreboard-Blocks -> have to be a multiple of 16
    //reason: 1) TimerScoreBoardEntries  have to be a multiple of 4
    //reason: 2) ScoreBoardBase addresses have to be on a 8Byte boundary
    TimerScorebBlocks = ConsumerCnt;
    mod_rest          = (LSA_UINT16)(ConsumerCnt % 16);

    if (mod_rest)
    {
        TimerScorebBlocks = (LSA_UINT16)(TimerScorebBlocks + (LSA_UINT16)(16 - mod_rest));
    }

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsInit, NOTE: Count of ConsumerCnt -> ConsumerCnt:0x%X TScorebBlocks -> TimerScorebBlocks:0x%X", ConsumerCnt, TimerScorebBlocks);

    //malloc Timer-Scoreboard and Change

    length       = (LSA_UINT16)(TimerScorebBlocks * sizeof(EDDI_SER10_TIMER_SCORB_TYPE));
    length_block = (LSA_UINT16)(TimerScorebBlocks / 2 );  /* 4 Bit ein Eintrag */
    length_all   = (LSA_UINT16)(length + (length_block * 2));
    pRps->SBList.DevSBChangeListSizeInUint32 = (length_block * 2)/sizeof(LSA_UINT32);

    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, (LSA_UINT32)length_all);

    if (ret != EDDI_MEM_RET_OK)
    {
        if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
        {
            EDDI_Excp("EDDI_CRTRpsInit, Alloc 1 - Not enough free KRAM !!", EDDI_FATAL_ERR_EXCP, ret, 0);
        }
        else
        {
            EDDI_Excp("EDDI_CRTRpsInit, Alloc 1 - EDDI_MEMGetBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, ret, 0);
        }
        return;
    }

    pDDB->KramRes.ScoreBoard = length_all;

    //ini Timer-Scoreboard
    EDDI_MemSet(pMemHeader->pKRam, (LSA_UINT8)0, (LSA_UINT32)length_all);

    pRps->SBList.pDevSBTimerBase   = (EDDI_SER10_TIMER_SCORB_TYPE  *)(void *)(pMemHeader->pKRam); // start address of the TimerScoreBoard

    pRps->SBList.pDevSBChangeList[0] =
        (EDDI_SER10_PROV_SCORB_PTR_TYPE)(void *)(pMemHeader->pKRam + length);                // ChangeList 1

    pRps->SBList.pDevSBChangeList[1] =
        (EDDI_SER10_PROV_SCORB_PTR_TYPE)(void *)(pMemHeader->pKRam + length + length_block); // ChangeList 2

    //CRT-Register-Configuration
    IO_W32(TIMER_SCOREB_ENTRIES, TimerScorebBlocks);
    IO_W32(TIMER_SCOREB_BASE, DEV_kram_adr_to_asic_register(pRps->SBList.pDevSBTimerBase, pDDB));

    IO_W32(PS_SCOREB_BASE_0, DEV_kram_adr_to_asic_register(pRps->SBList.pDevSBChangeList[0], pDDB));
    IO_W32(PS_SCOREB_BASE_1, DEV_kram_adr_to_asic_register(pRps->SBList.pDevSBChangeList[1], pDDB));

    pRps->PendingEvents    = LSA_FALSE;
    pRps->MaxPendingDGCons = 0xFFFFFFFFUL;

    pDDB->CRT.Rps.Status = EDDI_CRT_RPS_STS_SB_STOPPED; 
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTRpsTrigger()
 *
 *  Description: Central Function of the RemoteProviderFunction
 *               (Triggers the RPS-StateMachine)
 *
 *  Arguments:   pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *               pDDB            : the underlying Device-Descriptor
 *               RpsEvent        : the Event
 *               Param           : a helper-Param
 *
 *  Return:      -
 *
 *  Exception:   EDDI_ERR_SB_UNKNOWN_EVENT
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsTrigger( EDDI_CRT_RPS_TYPE        *  const  pRps,
                                                   EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                   EDDI_CRT_RPS_EVENT          const  RpsEvent )
{
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsTrigger > ->pRps->Status:0x%X :RpsEvent0x%X", pRps->Status, RpsEvent);

    switch (RpsEvent)
    {
        case EDDI_CRT_RPS_EVENT_START:
        {
            EDDI_CRTRpsStartEvent(pRps, pDDB);
            break;
        }

        case EDDI_CRT_RPS_EVENT_STOP:
        {
            EDDI_CRTRpsStopEvent(pRps, pDDB);
            break;
        }

        case EDDI_CRT_RPS_EVENT_SB_CHANGED:
        {
            EDDI_CRTRpsSBChangedEvent(pRps, pDDB);
            break;
        }

        case EDDI_CRT_RPS_EVENT_NEW_IND_RESOURCE:
        {
            EDDI_CRTRpsNewIndResourceEvent(pRps, pDDB);
            break;
        }

        case EDDI_CRT_RPS_EVENT_NEEDED_IND_SENT_TO_USER:
        case EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE:
        {
            //nothing to do!
            break;
        }

        default:
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsTrigger ERROR unknown Event");
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
            EDDI_Excp("EDDI_CRTRpsTrigger", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
            return;
        }
    }

    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsTrigger < RPS-Statemachine:  NewRpsStatus -> pRps->Status:0x%X", pRps->Status);
}
/*---------------------- end [subroutine] ---------------------------------*/





/******************************************************************************
 *  Function:    EDDI_CRTRpsInformStateMachine()
 *
 *  Description: Central Function of the RemoteProviderFunction
 *               (Triggers the RPS-StateMachine)
 *
 *  Arguments:   pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *               pDDB            : the underlying Device-Descriptor
 *               RpsEvent        : the Event
 *               Param           : a helper-Param
 *
 *  Return:      -
 *
 *  Exception:   EDDI_ERR_SB_UNKNOWN_EVENT
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsInformStateMachine( EDDI_CRT_RPS_TYPE        *  const  pRps,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                              EDDI_CRT_RPS_EVENT          const  RpsEvent )
{
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsInformStateMachine > ->pRps->Status:0x%X :RpsEvent0x%X", pRps->Status, RpsEvent);

    switch (RpsEvent)
    {
        case EDDI_CRT_RPS_EVENT_START:
        {
            switch (pRps->Status)
            {
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED:
                {
                    pDDB->CRT.Rps.Status = EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED;
                    break;
                }
                case EDDI_CRT_RPS_STS_SB_STOPPED:
                {
                    pDDB->CRT.Rps.Status = EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT;
                    break;
                }

                case EDDI_CRT_RPS_STS_NOT_AVAILABLE:        
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED:
                case EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT:
                default:
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsInformStateMachine ERROR Wrong RPS State");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
                    EDDI_Excp("EDDI_CRTRpsInformStateMachine", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
                    return;
                }
            }
            break;
        }

        case EDDI_CRT_RPS_EVENT_STOP:
        {
            switch (pRps->Status)
            {
                case EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT:
                {
                    pDDB->CRT.Rps.Status = EDDI_CRT_RPS_STS_SB_STOPPED;
                    break;
                }
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED:
                {
                    pDDB->CRT.Rps.Status = EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED;
                    break;
                }

                case EDDI_CRT_RPS_STS_SB_STOPPED:
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED:
                {
                    //do nothing
                    break;
                }

                case EDDI_CRT_RPS_STS_NOT_AVAILABLE:        
                default:
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsInformStateMachine ERROR Wrong RPS State");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
                    EDDI_Excp("EDDI_CRTRpsInformStateMachine", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
                    return;
                }
            }
            break;
        }

        case EDDI_CRT_RPS_EVENT_NEEDED_IND_SENT_TO_USER:
        {
            switch (pRps->Status)
            {
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED:
                {
                    pRps->Status = EDDI_CRT_RPS_STS_SB_STOPPED;
                    break;
                }

                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED:
                {
                    pRps->Status = EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT;
                    break;
                }

                case EDDI_CRT_RPS_STS_NOT_AVAILABLE:        
                case EDDI_CRT_RPS_STS_SB_STOPPED:
                case EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT:
                default:
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsInformStateMachine ERROR Wrong RPS State");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
                    EDDI_Excp("EDDI_CRTRpsInformStateMachine", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
                    return;
                }
            }
            break;
        }

        case EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE:
        {
            switch (pRps->Status)
            {
                case EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT:
                {
                    pRps->Status = EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED;
                    break;
                }

                case EDDI_CRT_RPS_STS_SB_STOPPED:
                {
                    pRps->Status = EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED;
                    break;
                }

                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED:
                case EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED:
                {
                    //nothing to do!
                    break;
                }

                case EDDI_CRT_RPS_STS_NOT_AVAILABLE:        
                default:
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsInformStateMachine ERROR Wrong RPS State");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
                    EDDI_Excp("EDDI_CRTRpsInformStateMachine", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
                    return;
                }
            }
            break;
        }

        case EDDI_CRT_RPS_EVENT_SB_CHANGED:
        case EDDI_CRT_RPS_EVENT_NEW_IND_RESOURCE:
        {
            //Nothing to do
            break;
        }

        default:
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsTrigger ERROR unknown Event");
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNKNOWN_EVENT);
            EDDI_Excp("EDDI_CRTRpsTrigger", EDDI_FATAL_ERR_EXCP, RpsEvent, 0);
            return;
        }
    }
}



/******************************************************************************
 *  Function:   EDDI_CRTRpsStartEvent() (EVENT of the Rps-Statemachine)
 *
 *  Description:Starts the Remote-Provider-Surveillance by calling the related
 *              HW-Functions.
 *              From now on, changes of the DataStatusByte of all active
 *              Consumers will be evaluates and indicated to the upper Component.
 *              Statemachine changes from Status STOPPED to WAITING_FOR_SB_INT.
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     -
 *
 *  Exception:  EDDI_ERR_SB_UNEXPECTED_EVENT_START
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsStartEvent( EDDI_CRT_RPS_TYPE        const *  const  pRps,
                                                              EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsStartEvent");

    // Check current status
    if (   (pRps->Status != EDDI_CRT_RPS_STS_SB_STOPPED)
        && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        /*if (pRps->Status == EDDI_CRT_RPS_STS_NOT_AVAILABLE) // ConsumerCnt = 0 !!
        {
          return;
        } */

        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsStartEvent ERROR wrong RPS-Status -> pRps->Status:0x%X", pRps->Status);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNEXPECTED_EVENT_START);
        EDDI_Excp("EDDI_CRTRpsStartEvent wrong RPS-Status", EDDI_FATAL_ERR_EXCP, pRps->Status, 0);
        return;
    }

    EDDI_SERScoreBoardStart(LSA_TRUE, LSA_TRUE, pDDB);

    #if defined (EDDI_CFG_USE_SW_RPS)
    // Initialize
    // pRps->OldCycleCounter = (LSA_UINT16)IO_R32(CYCL_COUNT_VALUE);

    if (pDDB->CRT.MetaInfo.AcwConsumerCnt || pDDB->CRT.MetaInfo.FcwConsumerCnt)
    {
        pDDB->SW_RPS_CheckDataStatesEvent = EDDI_CRTSbSwCheckDataStatesEvent;
    }
    #endif

    EDDI_CRTRpsInformStateMachine(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_START);

    //Start DFP WD handling
    EDDI_DFPWatchDogStart(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTRpsStopEvent() (EVENT of the Rps-Statemachine)
 *
 *  Description:Stops the Remote-Provider-Surveillance by calling the related
 *              HW-Functions.
 *              From now on changes of any Consumer-Status-Byte will be ignored.
 *              Statemachine changes from Status (WAITING_FOR_SB_INT or WAITING_FOR_INDPROV) to STOPPED.
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     -
 *
 *  Exception:  EDDI_ERR_SB_UNEXPECTED_EVENT_STOP
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsStopEvent( EDDI_CRT_RPS_TYPE        const *  const  pRps,
                                                             EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsStopEvent");

    // Check current status
    if (   (pRps->Status != EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT)
        && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED)
        && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNEXPECTED_EVENT_STOP);
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsStopEvent ERROR wrong RPS-Status -> pRps->Status:0x%X", pRps->Status);
        EDDI_Excp("EDDI_CRTRpsStopEvent", EDDI_FATAL_ERR_EXCP, pRps->Status, 0);
        return;
    }

    #if defined (EDDI_CFG_USE_SW_RPS)
    pDDB->SW_RPS_CheckDataStatesEvent = EDDI_CRTRpsCheckDataStatesEventDummy;
    #endif

    EDDI_SERScoreBoardStop(LSA_TRUE, LSA_TRUE, pDDB);

    // Update Status

    EDDI_CRTRpsInformStateMachine(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_STOP);

    //Stop DFP WD handling
    EDDI_DFPWatchDogStop(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTRpsSBChangedEvent() (EVENT of the Rps-Statemachine)
 *
 *  Description:This function is called by the SBChangedInterrupt, if new
 *              Scoreboard-Entries are available.
 *
 *              If a free Indication-Resource is available, the function
 *                 EDDI_CRTRpsDoEvaluation() will be called.
 *
 *              Else no evaluation is done and is set to WAITING_FOR_INDPROV until
 *                 a new IndicationResource will be available.
 *                 To be informed on new Indications the flag "RpsWaitsForResources"
 *                 in CRTComp will be set to TRUE.
 *                 The Change Enable-Bit will *NOT* be set.
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     -
 *
 *  Exception:  EDDI_ERR_SB_UNEXPECTED_EVENT_SB_CHANGED
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsSBChangedEvent( EDDI_CRT_RPS_TYPE        const *  const  pRps,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsSBChangedEvent");

    // Check current status
    if (   (EDDI_CRT_RPS_STS_SB_STOPPED == pRps->Status)
        && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTRpsSBChangedEvent old SB-Int -> pRps->Status:0x%X", pRps->Status);
        return;
    }
    else if (   (pRps->Status != EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT) 
             && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED)
             && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNEXPECTED_EVENT_SB_CHANGED);
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsSBChangedEvent ERROR wrong RPS-Status; pRps->Status, 0 -> pRps->Status:0x%X", pRps->Status);
        EDDI_Excp("EDDI_CRTRpsSBChangedEvent", EDDI_FATAL_ERR_EXCP, pRps->Status, 0);
        return;
    }

    //Start Evaluating
    EDDI_CRTRpsDoEvaluation(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTRpsNewIndResourceEvent() (EVENT of the Rps-Statemachine)
 *
 *  Description:This function is called by the CRTComp, if a new IndicationResource
 *              is available and the function  "CRTRpsIsWaitingForIndResource"
 *              called before by CRTComp returned LSA_TRUE;
 *
 *              Now the pending SB-Entries are evaluated by calling the function
 *              EDDI_CRTRpsDoEvaluation()
 *
 *  Arguments:  pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *              pDDB            : the Device-Description-Block
 *
 *  Return:     -
 *
 *  Exception:  EDDI_ERR_SB_UNEXPECTED_EVENT_NEW_IND_RESOURCE
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsNewIndResourceEvent( EDDI_CRT_RPS_TYPE        *  const  pRps,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_RESULT  Status;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTRpsNewIndResourceEvent");

    // Check current status
    if (   (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED)
        && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTRpsNewIndResourceEvent ERROR wrong RPS-Status; pRps->Status:0x%X", pRps->Status);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SB_UNEXPECTED_EVENT_NEW_IND_RESOURCE);
        EDDI_Excp("EDDI_CRTRpsNewIndResourceEvent", EDDI_FATAL_ERR_EXCP, pRps->Status, 0);
        return;
    }

    //check complete consumerid ranges
    Status = EDDI_CRTRpsFillAndSendIndication(pDDB, pDDB->CRT.ConsumerList.LastIndexRTC123, pDDB->CRT.ConsumerList.LastIndexDFP, LSA_FALSE /*bDirectIndication*/);

    if (Status != EDD_STS_OK)
    {
        // no more free Indications found
        // --> wait for more
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsNewIndResourceEvent WARNING: Could not get free Indication-Ressource -> Status:0x%X", Status);
    }
    else
    {
        EDDI_CRTRpsInformStateMachine(pRps, pDDB, EDDI_CRT_RPS_EVENT_NEEDED_IND_SENT_TO_USER);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/



/******************************************************************************
 *  Function:    EDDI_CRTRpsIsStopped()
 *
 *  Description: This function is called by the CRTComp, to check if the
 *               Scoreboard is already running.
 *
 *  Arguments:   pRps (THIS)     : the Pointer of the Remote-Provider-Surveillance;
 *
 *  Return:     
 */
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsIsStopped( EDDI_CRT_RPS_TYPE  const *  const  pRps )

{
    if (   (pRps->Status == EDDI_CRT_RPS_STS_SB_STOPPED)
        || (pRps->Status == EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED)
        || (pRps->Status == EDDI_CRT_RPS_STS_NOT_AVAILABLE))
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsFillAndSendIndication( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  PendingEventMaxConsID123, 
                                                                   LSA_UINT32               const  PendingEventMaxConsIDDFP,
                                                                   LSA_BOOL                 const  bDirectIndication)
{
    LSA_RESULT                           Status;
    LSA_UINT16                           ConsumerID;
    EDDI_CRT_CONSUMER_PTR_TYPE           pConsumer;
    EDD_UPPER_RQB_PTR_TYPE               pIndRQB   = EDDI_NULL_PTR;
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE  pIndParam = EDDI_NULL_PTR;
    LSA_UINT8                            ListCnt;
    LSA_BOOL                             bMISSPresent = LSA_FALSE;
    LSA_BOOL                             bCopyDebugInfo = LSA_FALSE;
    EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE DebugInfoCommon;

    #if defined (EDDI_CFG_DFP_ON)
    LSA_UINT32                                  ConsumerIDStart       = pDDB->CRT.MetaInfo.RTC123IDStart;
    LSA_UINT32                                  PendingEventMaxConsID = PendingEventMaxConsID123;
    LSA_UINT8                      const ListCntMax = 2;
    #else
    LSA_UINT32                           const  ConsumerIDStart       = pDDB->CRT.MetaInfo.RTC123IDStart;
    LSA_UINT32                           const  PendingEventMaxConsID = PendingEventMaxConsID123;
    LSA_UINT8                            const  ListCntMax            = 1;
    LSA_UNUSED_ARG(PendingEventMaxConsIDDFP); //not used yet
    #endif
    
    DebugInfoCommon.Ident = 0; //avoid compiler warning
    
    for (ListCnt=0; ListCnt<ListCntMax; ListCnt++)
    {
        //Step1: Start with RTC123 consumers
        if  /* anything to process for this consumerid-range */
           (PendingEventMaxConsID <= 0xFFFFUL)
        {
            for (ConsumerID = (LSA_UINT16)ConsumerIDStart; ConsumerID <= PendingEventMaxConsID; ConsumerID++)
            {
                Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, &pDDB->CRT.ConsumerList, ConsumerID);
                if ((Status != EDD_STS_OK) || (pConsumer->PendingIndEvent == 0))
                {
                    continue;
                }

                // OK we have an Event
                // Check if we first need to get an empty RQB
                if (pIndRQB == EDDI_NULL_PTR)
                {
                    pIndRQB = (EDD_UPPER_RQB_PTR_TYPE)(void *)EDDI_QueueRemoveFromBegin(&pDDB->pLocal_CRT->FreeIndicationQueue);

                    if (pIndRQB == EDDI_NULL_PTR)
                    {
                        // no free Indication found --> Lets return and wait until new IndicationRessources arrive.
                        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsFillAndSendIndication Could not get free Indication-Ressource");

                        return EDD_STS_ERR_RESOURCE;
                    }
                    pIndParam = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE)pIndRQB->pParam;
                }

                if (LSA_HOST_PTR_ARE_EQUAL(pIndParam, LSA_NULL))
                {
                    EDDI_Excp("EDDI_CRTRpsFillAndSendIndication", EDDI_FATAL_ERR_EXCP, 0, "pIndParam is LSA_NULL");
                    return EDD_STS_ERR_EXCP;
                }

                // Move Event into Indication-Container
                pIndParam->Data[pIndParam->Count].Event       = pConsumer->PendingIndEvent;
                pIndParam->Data[pIndParam->Count].EventStatus = pConsumer->EventStatus;
                pIndParam->Data[pIndParam->Count].CycleCnt    = pConsumer->PendingCycleCnt;
                pIndParam->Data[pIndParam->Count].UserID      = pConsumer->UpperUserId;
                if (pConsumer->PendingIndEvent & (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED))
                {
                    EDDI_RQB_CSRT_IND_DATA_DEBUGINFO_TYPE * const pDebugInfoEvent = (EDDI_RQB_CSRT_IND_DATA_DEBUGINFO_TYPE * )(LSA_VOID *)&pIndParam->Data[pIndParam->Count].DebugInfo[0];
                    
                    if (EDDI_IS_DFP_CONS(pConsumer))
                    {
                        pDebugInfoEvent->FrameType = EDDI_DEBUGINFO_FRAMETYPE_DFP_SUBFRAME;
                    }
                    else if (pConsumer->bAUXTriggeredMiss)
                    {
                        pDebugInfoEvent->FrameType = EDDI_DEBUGINFO_FRAMETYPE_AUX;
                    }
                    else
                    {
                        pDebugInfoEvent->FrameType = EDDI_DEBUGINFO_FRAMETYPE_RTC123;
                    }
                    pDebugInfoEvent->FrameType   |= ((pConsumer->PendingTransferStatus & 0x0F) << 4);
                    pDebugInfoEvent->CycleCtrHigh = (LSA_UINT8)(pConsumer->PendingCycleCnt >> 8);
                    pDebugInfoEvent->CycleCtrLow  = (LSA_UINT8)(pConsumer->PendingCycleCnt & 0xFFU);
                    
                    bMISSPresent = LSA_TRUE;
                }
                pIndParam->Count++;

                //check for PRIMARY-Event wait time
                if (pConsumer->PendingIndEvent & EDD_CSRT_CONS_EVENT_PRIMARY)
                {
                    volatile LSA_UINT32  Delta_ticks;
                    //if not enough time passed since event was detected: wait
                    do
                    {
                        Delta_ticks = IO_R32(CLK_COUNT_VALUE) - pConsumer->PrimaryEventStartTime_ticks;
                    } while (Delta_ticks < (EDDI_CRT_CONS_PRIM_WAIT_TIME_NS/EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS));
                }
                
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDDI_CSRT_CONS_EVENT_CLEAR_PENDING_EVENTS, (LSA_UINT16)0, 0, pDDB);

                if (pIndParam->Count >= EDD_CFG_CSRT_MAX_INDICATIONS)
                {
                    pIndParam->DebugInfo[0] = EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDI;
                    if (bMISSPresent & bDirectIndication)
                    {
                        if (!bCopyDebugInfo)
                        {                     
                            EDDI_CRTRpsFillCommonDebugInfo(pDDB, &DebugInfoCommon);
                            bCopyDebugInfo = LSA_TRUE;
                        }
                        EDDI_MEMCOPY(&pIndParam->DebugInfo[0], &DebugInfoCommon.Ident, sizeof(EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE));
                        bMISSPresent = LSA_FALSE;
                    }
                    else
                    {
                        pIndParam->DebugInfo[1] = 0;    //mark as invalid
                    }

                    // OK IndicationParameter is full
                    // --> Fire the Indication
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRpsFillAndSendIndication INFO: Firing CRT-Indication; pIndParam->Count:0x%X", pIndParam->Count);

                    EDDI_RequestFinish(pDDB->CRT.pHDB, pIndRQB, EDD_STS_OK);
                    // Set pIndRQB to Null to get a new one in the next loop
                    pIndRQB   = EDDI_NULL_PTR;
                    pIndParam = EDDI_NULL_PTR;
                }
            } //end for
        } //end if
        
        #if defined (EDDI_CFG_DFP_ON)
        //Step2: Check DFP consumers
        ConsumerIDStart       = pDDB->CRT.MetaInfo.RTC123IDEnd;
            PendingEventMaxConsID = PendingEventMaxConsIDDFP;
        #endif
    } //end for

    if (!(pIndRQB == EDDI_NULL_PTR))
    {
        pIndParam = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE)pIndRQB->pParam;

        // DebugInfo will only be filled if a MISS is present in at least one event, 
        // and if the indication has not been stored due to lack of resources
        if (bMISSPresent & bDirectIndication)
        {
            if (!bCopyDebugInfo)
            {                     
                EDDI_CRTRpsFillCommonDebugInfo(pDDB, &DebugInfoCommon);
                bCopyDebugInfo = LSA_TRUE;
            }
            EDDI_MEMCOPY(&pIndParam->DebugInfo[0], &DebugInfoCommon.Ident, sizeof(EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE));
            bMISSPresent = LSA_FALSE;
        }
        else
        {
            pIndParam->DebugInfo[0] = EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDI;
            pIndParam->DebugInfo[1] = 0;    //mark as invalid
        }
        // OK there are still some Indications to be sent
        // --> Fire the Indication
        // EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
        //                  "EDDI_CRTRpsFillAndSendIndication INFO: Firing CRT-Indication; Count,0 -> pIndParam->Count:0x%X 0:0x%X",
        //                  pIndParam->Count, 0);
        EDDI_RequestFinish(pDDB->CRT.pHDB, pIndRQB, EDD_STS_OK);
        pIndRQB = EDDI_NULL_PTR;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsFillCommonDebugInfo( EDDI_LOCAL_DDB_PTR_TYPE                   const pDDB,
                                                                      EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE * const pDebugInfoCommon) 
{
    LSA_UINT8   UsrPortIndex;
    LSA_UINT32  HwPortIndex;
    LSA_UINT8   PortState;

    EDDI_MEMSET(pDebugInfoCommon, (LSA_UINT8)0, sizeof(EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE));   
    pDebugInfoCommon->Ident  = EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDI;
    pDebugInfoCommon->Global = EDDI_DEBUGINFO_GLOBAL_IS_VALID + ((pDDB->SYNC.PortMachines.Input.isLocalSyncOk)? EDDI_DEBUGINFO_GLOBAL_IS_SYNCOK:0);

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        PortState = (LSA_UINT8)(pDDB->SWITCH.LinkIndExtPara[HwPortIndex].PortState & 0x0F)<<4;

        //PortStatus1:
        pDebugInfoCommon->PortStatus[UsrPortIndex].Status1 =   DebugInfoRTC3PortStatus[pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].IRTPortStatus]
                                                             + DebugInfoRTC2PortStatus[pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].RTClass2_PortStatus]
                                                             + DebugInfoRTC3RemotePortStatus[pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex].RtClass3_RemoteState];
        //PortStatus2:
        pDebugInfoCommon->PortStatus[UsrPortIndex].Status2 =   ((EDD_RTCLASS2_ACTIVE == pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].RTClass2_PortStatus)?                    1:0)
                                                             + ((EDD_IRT_PORT_ACTIVE_TX_RX == pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].IRTPortStatus)?                    2:0)
                                                             + ((pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].SyncId0_TopoOk)?                                                4:0)
                                                             + ((EDD_SET_REMOTE_PORT_STATE_TOPO_OK == pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex].RtClass3_PDEVTopoState)?8:0)
                                                             + PortState;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_sb.c                                                */
/*****************************************************************************/
