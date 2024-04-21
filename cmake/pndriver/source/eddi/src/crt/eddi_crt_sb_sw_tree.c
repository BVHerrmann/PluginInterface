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
/*  F i l e               &F: eddi_crt_sb_sw_tree.c                     :F&  */
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


#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_ERTEC_200)
#include "eddi_dev.h"
#include "eddi_crt_sb_sw_tree.h"
#endif

//#include "eddi_crt_check.h"

#if defined (EDDI_CFG_USE_SW_RPS)
#include "eddi_crt_ext.h"
#endif

#define EDDI_MODULE_ID     M_ID_CRT_SB_SW_TREE
#define LTRC_ACT_MODUL_ID  110

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


#if defined (EDDI_CFG_USE_SW_RPS)
/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTSbSwCheckDataStatesEvent()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSbSwCheckDataStatesEvent( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                 EDDI_CRT_RPS_TYPE        *  const  pRps )
{
    LSA_RESULT                    Status;
    LSA_UINT32                    i, k, CntEntries, LastIndex;
    EDDI_CRT_CONS_SB_TYPE      *  pSBCons;
    LSA_UINT32                    emptyEntry;
    LSA_UINT32                    PendingEventsCount;
    //LSA_UINT32                  CurrentCycleCounter;
    EDDI_SER_DATA_STATUS_TYPE     DataStatus, DeltaStatus;
    E_TREE                     *  pSBTree;
    LSA_UINT32                    LocalTimeStamp;
    LSA_BOOL                      UKCountDownExecute = LSA_TRUE;
    LSA_BOOL                      SBConsNilPtrAllowed = LSA_FALSE;
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LTRC_LEVEL_TYPE               Tracelevel = LSA_TRACE_LEVEL_FATAL;
    #endif


    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTSbSwCheckDataStatesEvent->");

    // Check current status
    if  (   (pRps->Status != EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT)
         && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED)
         && (pRps->Status != EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_Excp("EDDI_CRTSbSwCheckDataStatesEvent - Status", EDDI_FATAL_ERR_EXCP, pRps->Status, 0);
        return;
    }

    //Start Evaluating
    //CurrentCycleCounter = EDDI_IRTE2HOST32(IO_x32(CYCL_COUNT_VALUE)); // Only 16 lower Bits are relevant

    emptyEntry     = 0;
    PendingEventsCount = 0;

    pSBTree = pDDB->CRT.pSBTree;
    EDDI_TreeGetPathList(pSBTree);

    LastIndex = pSBTree->CntActPath;

    LocalTimeStamp = IO_R32(CLK_COUNT_VALUE); //get current timestamp (10ns)

    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    pDDB->SW_RPS_TotalCallsCtr++;
    #endif

    //1st call of this function?
    if (pDDB->SW_RPS_NextUK_Always)
    {
        pDDB->SW_RPS_NextUK_ClockCntVal = LocalTimeStamp+ pDDB->SW_RPS_NextUK_Cycle_10ns;
        pDDB->SW_RPS_NextUK_Always      = LSA_FALSE;
    }
    else
    {
        //check if calls do not follow too close to each other. Will only work if calls are no more than 21s apart.
        if //too early
           ((LSA_INT32)(LocalTimeStamp - pDDB->SW_RPS_NextUK_ClockCntVal) < 0)
        {
            UKCountDownExecute = LSA_FALSE;
            #if defined (EDDI_CFG_CONSTRACE_DEPTH)
            pDDB->SW_RPS_RejectedCallsCtr++;
            #endif
        }
        else
        {
            pDDB->SW_RPS_NextUK_ClockCntVal = LocalTimeStamp + pDDB->SW_RPS_NextUK_Cycle_10ns;
        }
    }

    for (i = 0; i < LastIndex; i++)
    {
        pSBCons = (EDDI_CRT_CONS_SB_TYPE *)pSBTree->ActPath[i].pList;

        CntEntries = pSBTree->ActPath[i].CntEntries;

        for (k = 0; k < CntEntries; k++)
        {
            if (!pSBCons)
            {
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, Tracelevel, "EDDI_CRTSbSwCheckDataStatesEvent Leaf not found: i/Lastindex:0x%X/0x%X k/CntEntries:0x%X/0x%X",
                                    i, LastIndex, k, CntEntries);
                if (SBConsNilPtrAllowed)
                {
                    break;
                }
                else
                {
                    EDDI_Excp("EDDI_CRTSbSwCheckDataStatesEvent Leaf not found", EDDI_FATAL_ERR_EXCP, i, k);
                    return;
                }
            }

            if (pSBCons->Status != EDDI_CRT_CONS_STS_ACTIVE)
            {
                pSBCons = pSBCons->Link.pNext;
                continue;
            }

            // Handle UnknownState-Handling
            if (pSBCons->UkStatus == EDDI_CRT_CONS_UK_COUNTING)
            {
                #if defined (EDDI_CFG_CONSTRACE_DEPTH)
                EDDI_CRT_CONSUMER_TYPE  *  const  pConsumer = &pDDB->CRT.ConsumerList.pEntry[pSBCons->ConsumerId];
                #endif
                if (UKCountDownExecute)
                {
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer)), pSBCons->UkCountdown, pSBCons->UkStatus, EDDI_CONSTRACE_STATE_UKCOUNTDOWN1);
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, (LSA_UINT32)pSBCons, k, i, EDDI_CONSTRACE_STATE_UKCOUNTDOWN2);

                    /* EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTSbSwCheckDataStatesEvent *** :0x%X ConsumerId:0x%X UkStatus:0x%X UkCountdown:0x%X",
                                      IO_R32(CYCL_COUNT_VALUE), pSBCons->ConsumerId, pSBCons->UkStatus, pSBCons->UkCountdown); */

                    pSBCons->UkCountdown--;
                    if (pSBCons->UkCountdown <= 0)
                    {
                        pSBCons->UkStatus = EDDI_CRT_CONS_UK_TIMED_OUT;
                        EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer)), pSBCons->OldDataStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTIMEDOUT);
                    }
                }
                else
                {
                    EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer)), pSBCons->UkCountdown, pSBCons->UkStatus, EDDI_CONSTRACE_STATE_UKCOUNTDOWNEARLY);
                }
            }

            DataStatus = *(pSBCons->pCopyKRAMConsDataStatus);

            //Get all changed Bits
            DeltaStatus = (LSA_UINT8)((pSBCons->OldDataStatus ^ DataStatus) & SER_DATA_STATUS_USED_BITS);

            if (DeltaStatus || (pSBCons->UkStatus == EDDI_CRT_CONS_UK_TIMED_OUT))
            {
                EDDI_CRT_CONSUMER_TYPE  *  const  pConsumer = &pDDB->CRT.ConsumerList.pEntry[pSBCons->ConsumerId];

                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTSbSwCheckDataStatesEvent *** :0x%X ConsumerId:0x%X UkStatus:0x%X KramDataStatus:0x%X",
                                  IO_R32(CYCL_COUNT_VALUE), pSBCons->ConsumerId, pSBCons->UkStatus, DataStatus);

                EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer)), DeltaStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_SWTRIG);

                //Change in DataStatus Detected
                if (EDDI_CRTRpsCalculateIndEntry(pConsumer, emptyEntry, pDDB))
                {
                    //OK we have a new Event for this Consumer:
                    PendingEventsCount++;
                    if (EDDI_CRT_CONS_STS_ACTIVE != pConsumer->pSB->Status)
                    {
                        //swsb leaf got removed by cons-as
                        SBConsNilPtrAllowed = LSA_TRUE;
                        #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
                        Tracelevel = LSA_TRACE_LEVEL_WARN;
                        #endif
                    }
                }
            }

            pSBCons = pSBCons->Link.pNext;
        } // end for
    }  // end LastIndex

    if (PendingEventsCount)
    {
        //check all RTC12- and RTC3-consumers. No potential for optimization here!
        Status = EDDI_CRTRpsFillAndSendIndication(pDDB, pDDB->CRT.ConsumerList.LastIndexRTC123, 0xFFFFFFFF /*DFP cons. are not needed here */, LSA_TRUE /*bDirectIndication*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRTRpsInformStateMachine(pRps, pDDB, EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE);
        }
    }

    //pRps->OldCycleCounter = CurrentCycleCounter;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTSbSwCheckDataStatesEvent<-");

    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LSA_UNUSED_ARG(Tracelevel);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTSbSwRemove()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSbSwRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                   EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTSbSwRemove->");
   
    EDDI_TreeRemove(pDDB, (E_TREE_LIST *)(void *)pConsumer->pSB);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSbSwAdd( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    E_TREE  *  const  pTree = pDDB->CRT.pSBTree;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTSbSwAdd->");

    EDDI_TreeAdd(pDDB, pTree,
                 (LSA_UINT32)pConsumer->LowerParams.CycleReductionRatio,
                 (LSA_UINT32)pConsumer->LowerParams.CyclePhase,
                 (E_TREE_LIST *)(void *)pConsumer->pSB);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSbSwSet2UK( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                   EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                   LSA_BOOL                    const  bActivate)
{
    LSA_UINT32  const   Reduction    = pDDB->SW_RPS_internal_reduction * pDDB->NewCycleReductionFactor;
    LSA_UINT32  const   WDF = (bActivate?2:1) * pConsumer->LowerParams.DataHoldFactor;
    LSA_UINT32          UkCd = (WDF/Reduction);

    //Unknown-State-Countdown is updated by Sb-Tree.
    //Aging for UkCountdown: -1 per Sb-Tree check
    //Doubled WD factor is only applied after activating (see AP00252717)

    //Worstcase is, when the reduced SB check is triggered exactly at time 0. Example for NewCycleReductionFactor=2, SW_RPS_internal_reduction=2, WDF=2:
    //NewCycleReductionCounter SW_RPS_internal_reductionCounter WDFCounter
    //                      2
    //                      1  ==> 2->1
    //                      2
    // t=0!                 1  ==> 1->0                         ==> 2->1
    //                      2
    //                      1  ==> 2->1
    //                      2
    // Expiry:4 cycles!     1  ==> 1->0                         ==> 1->0

    if (UkCd < 2)  {UkCd = 2;}

    while (((UkCd-1) * Reduction) < WDF)
    {
        //Activation: get as closest as possible to DataHoldFactor*2, but not above
        //!Activation: get at least above DataHoldFactor
        if (bActivate)
        {
            if (   (( UkCd    * Reduction) >= WDF)
                && (((UkCd-1) * Reduction) >  (WDF/2)))
            {
                break;
            }
        }
        UkCd++;
    }

    pConsumer->pSB->UkCountdown = (LSA_INT32)UkCd;

    EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTSbSwSet2UK, NewCycleReductionFactor:0x%X SW_RPS_internal_reduction:0x%X UkCountdown:0x%X DataHoldFactor:0x%X",
                      pDDB->NewCycleReductionFactor, pDDB->SW_RPS_internal_reduction,
                      pConsumer->pSB->UkCountdown, pConsumer->LowerParams.DataHoldFactor);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_USE_SW_RPS


/*****************************************************************************/
/*  end of file eddi_crt_sb_sw_tree.c                                        */
/*****************************************************************************/
