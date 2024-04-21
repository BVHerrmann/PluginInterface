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
/*  F i l e               &F: eddi_isr_nrt.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-interrupthandler for NRT                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  16.07.07    JS    added support for SYNC-Send (mapped to NRT-Send)       */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_EDDI_ISR_NRT
#define LTRC_ACT_MODUL_ID  6

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

#include "eddi_nrt_tx.h"

#if defined (EDDI_CFG_REV5)
#include "eddi_nrt_ts.h"
#endif

#include "eddi_nrt_inc.h"
#include "eddi_isr_nrt.h"
//#include "eddi_sync_usr.h"

#if defined (EDDI_CFG_FRAG_ON)
#include "eddi_nrt_frag_tx.h"
#endif

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadSendList( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  QueueIdx );

#if defined (EDDI_CFG_REV5)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadSendListTs( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  QueueIdx );
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTReloadAllSendLists()                     */
/*                                                                         */
/* D e s c r i p t i o n: NRT reload of all send-lists for all             */
/*                        NRT-interfaces                                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadAllSendLists( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  Dummy )
{
    LSA_UNUSED_ARG(Dummy); //satisfy lint!

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTReloadAllSendLists->");

    #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
	//acknowledge IRTE-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" for executing nrt-tx-reload. do not change without checking EDDI_SIIInterruptWork() => EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR!
    EDDI_SII_IO_x32(pDDB->SII.IAR_NRT_Adr) = EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
    #endif

    EDDI_NRTReloadSendList(pDDB, EDDI_NRT_CHA_IF_0);
    #if defined (EDDI_CFG_REV5)
    EDDI_NRTReloadSendListTs(pDDB, EDDI_NRT_CHB_IF_0);
    #else
    EDDI_NRTReloadSendList(pDDB, EDDI_NRT_CHB_IF_0);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTReloadSendList()                         */
/*                                                                         */
/* D e s c r i p t i o n: NRT reload of one send-list without Timestamp    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadSendList( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  QueueIdx )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF       = &pDDB->NRT.IF[QueueIdx];
    EDDI_NRT_SND_LIMIT_TYPE  *  const  pSndLimit = &pIF->Tx.SndLimit;
    #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
    LSA_BOOL                           bLocked   = LSA_FALSE;
    #endif

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTReloadSendList->");

    if (pIF->CntOpenChannel == 0)
    {
        //interface unused
        return;
    }

    if //at least 1 occupied TX-DMACW is existent?
       (pIF->Tx.BalanceSndReq)
    {
        if //owner of the DMACW is still IRTE?
           (pIF->Tx.Dscr.pReqDone->pDMACW->L0.Reg & BM_S_DMACW_OwnerHW_29)
        {
            #if defined (EDDI_CFG_FRAG_ON)
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.LockFct(pIF);
            #endif
            if (!pDDB->NRT.TxFragData.bFragCleanupActive)
            #endif
            {
                //retrigger IRTE for security to check the DMACW-queue (maybe trigger can be lost)
                IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            }
            #if defined (EDDI_CFG_FRAG_ON)
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
            #endif
        }
        else //owner of the DMACW is EDDI
        {
		    //generate nrt-tx-done-interrupt by SW for the case, that EDDI doesn't process the DMACWs (e.g. nrt-tx-done-interrupt is lost)
            EDDI_SII_IO_x32(pDDB->SII.IRR_NRT_Adr) = pIF->Value_for_IO_NRT_TX_Interrupt_IRTE;
        }
    }

    if //at least 1 occupied RX-DMACW is existent? (= owner of the DMACW is EDDI?)
       (pIF->Rx.Dscr.Cnt && ((pIF->Rx.Dscr.pReqDone->pDMACW->L0.Reg & BM_S_DMACW_OwnerHW_29) == 0))
    {
		//generate nrt-rx-done-interrupt by SW for the case, that EDDI doesn't process the DMACWs (e.g. nrt-rx-done-interrupt is lost)
        EDDI_SII_IO_x32(pDDB->SII.IRR_NRT_Adr) = pIF->Value_for_IO_NRT_RX_Interrupt_IRTE;
    }

    #if !defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
    if (pSndLimit->SllReduction > 1)
    {
        if (pSndLimit->ActRedCnt != pSndLimit->SllReduction)
        {
            pSndLimit->ActRedCnt++;
            return;
        }
        else
        {
            pSndLimit->ActRedCnt = 1;
        }
    }
    #endif

    if (   (EDDI_NRT_CHB_IF_0 == QueueIdx)
        && (   (pSndLimit->ActCnt.BytesPerPrio[NRT_HSYNC1_HIGH_PRIO_INDEX])
            || (pSndLimit->ActCnt.BytesPerPrio[NRT_HSYNC2_HIGH_PRIO_INDEX]) )
        )
    {
        #if defined (EDDI_CFG_TRACE_HSA)
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "!!!HSA EDDI_NRTReloadSendList, IF(0:A0, 1:B0):0x%X", QueueIdx);
        #endif      
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.LockFct(pIF);
        bLocked = LSA_TRUE;
        #endif
        pSndLimit->ActCnt.BytesPerPrio[NRT_HSYNC1_HIGH_PRIO_INDEX]  = 0;
        pSndLimit->ActCnt.BytesPerPrio[NRT_HSYNC2_HIGH_PRIO_INDEX]  = 0;
        //HSYNC Low counters are not used
    }

    if (pIF->Tx.QueuedSndReq == 0)
    {
        //interfacequeue empty
        if (pSndLimit->ActCnt.BytesPerCycle)
        {
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            if (!bLocked)
            {
                pIF->Tx.LockFct(pIF);
            }
            #endif
            pSndLimit->ActCnt.BytesPerCycle = 0;
            pIF->Tx.PrioQueueArrayStartIndex = 0;
            if (EDDI_NRT_CHA_IF_0 == QueueIdx)
            {
                pSndLimit->ActCnt.BytesPerPrio[NRT_LOW_PRIO_INDEX]      = 0;
                pSndLimit->ActCnt.BytesPerPrio[NRT_MEDIUM_PRIO_INDEX]   = 0;
                pSndLimit->ActCnt.BytesPerPrio[NRT_HIGH_PRIO_INDEX]     = 0;
                pSndLimit->ActCnt.BytesPerCycleLMH                      = 0;
            }
            else
            {
                pSndLimit->ActCnt.BytesPerPrio[NRT_MGMT_LOW_PRIO_INDEX]     = 0;
                pSndLimit->ActCnt.BytesPerPrio[NRT_MGMT_HIGH_PRIO_INDEX]    = 0;
            }
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
        }
        else
        {
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            if (bLocked)
            {
                pIF->Tx.UnLockFct(pIF);
            }
            #endif
        }
    }
    else
    {
        //interfacequeue not empty
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        if (!bLocked)
        {
            pIF->Tx.LockFct(pIF);
        }
        #endif
        pIF->Tx.PrioQueueArrayStartIndex = 0;
        pSndLimit->ActCnt.BytesPerCycle = 0;
        if (EDDI_NRT_CHA_IF_0 == QueueIdx)
        {
            pSndLimit->ActCnt.BytesPerPrio[NRT_LOW_PRIO_INDEX]      = 0;
            pSndLimit->ActCnt.BytesPerPrio[NRT_MEDIUM_PRIO_INDEX]   = 0;
            pSndLimit->ActCnt.BytesPerPrio[NRT_HIGH_PRIO_INDEX]     = 0;
            pSndLimit->ActCnt.BytesPerCycleLMH = 0;
        }
        else
        {
            pSndLimit->ActCnt.BytesPerPrio[NRT_MGMT_LOW_PRIO_INDEX]     = 0;
            pSndLimit->ActCnt.BytesPerPrio[NRT_MGMT_HIGH_PRIO_INDEX]=    0;
        }
        #if defined (EDDI_CFG_FRAG_ON)
        if (!pDDB->NRT.TxFragData.bFragCleanupActive)
        #endif
        {
            EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
        }
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.UnLockFct(pIF);
        #endif
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTReloadSendListTs()                       */
/*                                                                         */
/* D e s c r i p t i o n: NRT reload of one send-list with Timestamp       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV5)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadSendListTs( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  QueueIdx )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF       = &pDDB->NRT.IF[QueueIdx];
    EDDI_NRT_SND_LIMIT_TYPE  *  const  pSndLimit = &pIF->Tx.SndLimit;

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTReloadSendListTs->");

    if (pIF->CntOpenChannel == 0)
    {
        //interface unused
        return;
    }

    if (pDDB->NRT.TimeStamp.cTimeStamp)
    {
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.LockFct(pIF);
        #endif

        if (pDDB->NRT.TimeStamp.cTimeStamp > pDDB->NRT.TimeStamp.Timeout)
        {
            EDDI_NRTTimeStampTimeOut(pDDB);
        }
        else
        {
            pDDB->NRT.TimeStamp.cTimeStamp++;
        }

        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.UnLockFct(pIF);
        #endif
    }

    if //at least 1 occupied TX-DMACW is existent?
       (pIF->Tx.BalanceSndReq)
    {
        if //owner of the DMACW is still IRTE?
           (pIF->Tx.Dscr.pReqDone->pDMACW->L0.Reg & BM_S_DMACW_OwnerHW_29)
        {
            #if defined (EDDI_CFG_FRAG_ON)
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.LockFct(pIF);
            #endif
            if (!pDDB->NRT.TxFragData.bFragCleanupActive)
            #endif
            {
                //retrigger IRTE for security to check the DMACW-queue (maybe trigger can be lost)
                IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            }
            #if defined (EDDI_CFG_FRAG_ON)
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
            #endif
        }
        else //owner of the DMACW is EDDI
        {
		    //generate nrt-tx-done-interrupt by SW for the case, that EDDI doesn't process the DMACWs (e.g. nrt-tx-done-interrupt is lost)
            EDDI_SII_IO_x32(pDDB->SII.IRR_NRT_Adr) = pIF->Value_for_IO_NRT_TX_Interrupt_IRTE;
        }
    }

    if //at least 1 occupied RX-DMACW is existent? (= owner of the DMACW is EDDI?)
       (pIF->Rx.Dscr.Cnt && ((pIF->Rx.Dscr.pReqDone->pDMACW->L0.Reg & BM_S_DMACW_OwnerHW_29) == 0))
    {
		//generate nrt-rx-done-interrupt by SW for the case, that EDDI doesn't process the DMACWs (e.g. nrt-rx-done-interrupt is lost)
        EDDI_SII_IO_x32(pDDB->SII.IRR_NRT_Adr) = pIF->Value_for_IO_NRT_RX_Interrupt_IRTE;
    }

    #if !defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
    if (pSndLimit->SllReduction > 1)
    {
        if (pSndLimit->ActRedCnt != pSndLimit->SllReduction)
        {
            pSndLimit->ActRedCnt++;
            return;
        }
        else
        {
            pSndLimit->ActRedCnt = 1;
        }
    }
    #endif

    if (pIF->Tx.QueuedSndReq == 0)
    {
        //interfacequeue empty

        if (pSndLimit->ActCnt.BytesPerCycle)
        {
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.LockFct(pIF);
            #endif
            EDDI_MEMSET(&pSndLimit->ActCnt, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_NRT_SND_LIMIT_CNT_TYPE)); //memset allowed here, because no HSYNC application for REV5
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
        }
    }
    else
    {
        //interfacequeue not empty

        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.LockFct(pIF);
        #endif
        pIF->Tx.PrioQueueArrayStartIndex = 0;
        EDDI_MEMSET(&pSndLimit->ActCnt, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_NRT_SND_LIMIT_CNT_TYPE)); //memset allowed here, because no HSYNC application for REV5
        #if defined (EDDI_CFG_FRAG_ON)
        if (!pDDB->NRT.TxFragData.bFragCleanupActive)
        #endif
        {
            EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
        }
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.UnLockFct(pIF);
        #endif
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTOverflowHOL()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTOverflowHOL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    LSA_UINT32               const  para_1 )
{
    LSA_UINT32     Ctr;
    /* LSA_UINT32  B1, P0, P1, P2, P3;

    B1 = pDDB->irte->s16000_KRAM_control.x1643c_HOL_Count_CHB_SS1;
    P0 = pDDB->irte->s16000_KRAM_control.x16444_HOL_Count_P0;
    P1 = pDDB->irte->s16000_KRAM_control.x1644c_HOL_Count_P1;
    P2 = pDDB->irte->s16000_KRAM_control.x16454_HOL_Count_P2;
    P3 = pDDB->irte->s16000_KRAM_control.x1645c_HOL_Count_P3;

    if (B1 || P0 || P1 || P2 || P3)
    {
    } */

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTOverflowHOL->");

    LSA_UNUSED_ARG(para_1); //satisfy lint!

    for (Ctr = EDDI_NRT_CHA_IF_0; Ctr < EDDI_NRT_IF_CNT; Ctr++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[Ctr];

        if (!pIF->Rx.Queued_RxCount)
        {
            continue;
        }

        pIF->HOLCount_CH = IO_R32(pIF->Value_for_IO_HOL_COUNT_CH);

        if (pIF->HOLCount_CH == 0)
        {
            continue;
        }

        //IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
        //pIF->Rx.DoneFct(pDDB, Ctr); //not necessary!

        pDDB->Glob.ErrInt.hol_interrupts[Ctr]++;
    }
}
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_INTNRTTxDone()                              */
/*                                                                         */
/* D e s c r i p t i o n: process TX-DMACWs                                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_INTNRTTxDone( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  LSA_UINT32               const  QueueIdx )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF   = &pDDB->NRT.IF[QueueIdx];
    NRT_CHX_SSX_TYPE         *  const  pDscr = &pIF->Tx.Dscr;
    NRT_DSCR_PTR_TYPE                  pReqDone;

    #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
    pIF->Tx.LockFct(pIF);
    #endif

    #if defined (EDDI_CFG_FRAG_ON)
    if (pDDB->NRT.TxFragData.bFragCleanupActive)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTNRTTxDone->fragmentation cleanup active");
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        pIF->Tx.UnLockFct(pIF);
        #endif
        return;
    }
    #endif

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_INTNRTTxDone->");

    pReqDone = pDscr->pReqDone;

    //sendback all rqbs from DMACW-ring to user, that are already processed by IRTE!
    for (;;)
    {
        LSA_UINT32                                Status;
        LSA_RESULT                                TxStatus;
        EDD_SERVICE                               Service;
        LSA_UINT32                                LocalL0;
        EDD_UPPER_RQB_PTR_TYPE             const  pRQB   = pReqDone->pSndRQB;
        volatile  EDDI_SER_DMACW_PTR_TYPE  const  pDMACW = pReqDone->pDMACW;
        EDD_UPPER_NRT_SEND_PTR_TYPE               pRQBSnd;
        EDDI_LOCAL_HDB_PTR_TYPE                   pHDB;

		//acknowledge nrt-tx-done-interrupt (posted write!)
        EDDI_SII_IO_x32(pDDB->SII.IAR_NRT_Adr) = pIF->Value_for_IO_NRT_TX_Interrupt_IRTE;

        if //no more rqb available in DMACW-ring to sendback (= no rqb at all)?
           (pRQB == EDDI_NULL_PTR)
        {
            if //new NRT-Sends are queued?
               (pIF->Tx.QueuedSndReq)
            {
                EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
            }
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
            return;
        }

        //read DMACW L0
        LocalL0 = pDMACW->L0.Reg;

        if //owner of the DMACW is still IRTE?
           (LocalL0 & BM_S_DMACW_OwnerHW_29)
        {
            //IRTE hasn't processed this DMACW yet!

            if //new/further NRT-Sends are queued?
               (pIF->Tx.QueuedSndReq)
            {
                EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
                //IRTE-Trigger TX-Enable is always executed within EDDI_NRTSendWithLimit()!
            }
            else //no new NRT-Sends are queued
            {
                //retrigger IRTE for security to check the DMACW-queue (maybe trigger can be lost)
                IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            }
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
            return;
        }

        #if defined (EDDI_CFG_REV6)
        //read DMACW L0 again to make sure that the DMACW has been read consistently (LBU 16bit mode!)
        LocalL0 = pDMACW->L0.Reg;
        #endif

        pIF->Tx.BalanceSndReq--; //do not change location!

        pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;

        NRT_GET_Status(Status, LocalL0);

        switch (Status)
        {
            case 0x0:
            {
                TxStatus        = EDD_STS_OK;
                pDDB->LocalIFStats.TxGoodCtr++;
                break;
            }
            case 0x1:
            {
                TxStatus        = EDD_STS_ERR_RESOURCE;
                break;
            }
            case 0x2:
            {
                TxStatus        = EDD_STS_ERR_RESOURCE;
                EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, NRT_TX_ERR_NO_MEM - buffer invalid, Status:0x%X, LocalL0:0x%X, pDMACW:0x%X, NRTChannel:%d", 
                    Status, LocalL0, pDMACW, pIF->NRTChannel);
                EDDI_Excp("EDDI_INTNRTTxDone, NRT_TX_ERR_NO_MEM - buffer invalid!", EDDI_FATAL_ERR_EXCP, 0, 0);
                #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                pIF->Tx.UnLockFct(pIF);
                #endif
                return;
            }
            default:
            {
                TxStatus        = EDD_STS_ERR_RESOURCE;
            }
        }

        //clean entry
        pReqDone->pSndRQB = EDDI_NULL_PTR;

        //switch to next entry (DMACW)
        pReqDone        = pReqDone->next_dscr_ptr;
        pDscr->pReqDone = pReqDone;

        #if defined (EDDI_CFG_FRAG_ON)
        {
            LSA_UINT32                    const  internal_context_1 = pRQB->internal_context_1;
            EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB  = &pDDB->NRT.TxFragData;

            if //FRAG RQB?
               (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
            {
                LSA_UINT32  NextFragInterfaceNr;

                //TX semaphore is always locked here

                if //not last fragment exits DMACW queue?
                   (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt != 1UL)
                {
                    if //stopping fragment-service running?
                       (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping)
                    {
                        if //not last fragment exits DMACW queue at stopping?
                           (pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping != 1UL)
                        {
                            //wait for next fragment
                            pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping--;
                            continue;
                        }
                        else //last fragment exits DMACW queue at stopping
                        {
                            LSA_UINT32                 const  PrioIndex         = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!
                            EDD_UPPER_RQB_PTR_TYPE     const  pHelpRQB          = (EDD_UPPER_RQB_PTR_TYPE)(pRQB->internal_context_Prio); //here fragmentation buffer is handled as RQB
                            EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;

                            //store PrioIndex for later deallocation
                            pHelpRQB->internal_context_Prio = PrioIndex;

                            //put big fragmentation buffer to Frag Dealloc Queue
                            EDDI_AddToQueueEnd(pDDB, pFragDeallocQueue, pHelpRQB);

                            pNrtTxFragDataDDB->RemainingFragmentDmacwExitCntAtStopping = 0;

                            if //releasing TX semaphore not successful?
                               (!EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RELEASE, QueueIdx, &NextFragInterfaceNr))
                            {
                                EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, releasing TX Frag semaphore not successful!, QueueIdx:0x%X, NextFragInterfaceNr:0x%X, NRTChannel:%d", 
                                    QueueIdx, NextFragInterfaceNr, pIF->NRTChannel);
                                EDDI_Excp("EDDI_INTNRTTxDone, releasing TX Frag semaphore not successful!", EDDI_FATAL_ERR_EXCP, 0, 0);
                                #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                                pIF->Tx.UnLockFct(pIF);
                                #endif
                                return;
                            }

                            pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

            
                            EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);
                            EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

                            if (!(0 == pHDB->Cbf))
                            {
                                //indicate RQB
                                EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);
                            }
                            else
                            {
                                EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, no Cbf in HDB! Handle:0x%X, UpperHandle:0x%X, NRTChannel:%d", 
                                    pHDB->Handle, pHDB->UpperHandle, pIF->NRTChannel);
                                EDDI_Excp("EDDI_INTNRTTxDone, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
                                #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                                pIF->Tx.UnLockFct(pIF);
                                #endif
                                return;
                            }

                            pHDB->TxCount--;
                            
                            if (    (0 == pHDB->TxCount) 
                                &&  (0 == pHDB->intTxRqbCount)
                                &&  (0 == pHDB->intRxRqbCount) 
                                &&  (pHDB->pRQBTxCancelPending) )
                            {
                                EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                                pHDB->pRQBTxCancelPending = LSA_NULL;

                                //Inform User thats all Frames are sent. The TxCancel can give back now!
                                EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                                EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_INTNRTTxDone-> Cancel in Progress <---");
                            }
                            else if (pHDB->TxCount < 0)
                            {
                                EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "pHDB->TxCount (0x%X) is negativ!, NRTChannel:%d", 
                                    pHDB->TxCount, pIF->NRTChannel);
                                EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
                            }

                            continue;
                        }
                    }

                    if //TxStatus negative?
                       (TxStatus != EDD_STS_OK)
                    {
                        if //negative TxStatus not yet stored?
                           (pNrtTxFragDataDDB->StoredTxStatus == EDD_STS_OK)
                        {
                            //store negative states
                            pNrtTxFragDataDDB->StoredTxStatus  = TxStatus;
                        }
                    }

                    pNrtTxFragDataDDB->RemainingFragmentDmacwExitCnt--;
                    //do not indicate RQB
                    continue;
                }
                else //last fragment exits DMACW queue
                {
                    LSA_UINT32  const  PrioIndex = (internal_context_1>>24) & 0xFUL; //ignore EDDI_TX_FRAG_RQB_MARK and EDDI_TX_FRAG_SEND_DATA_STORED!

                    if //negative TxStatus stored?
                       (pNrtTxFragDataDDB->StoredTxStatus != EDD_STS_OK)
                    {
                        //restore negative states
                        TxStatus        = pNrtTxFragDataDDB->StoredTxStatus;
                    }

                    //put big fragmentation buffer to Frag Dealloc Queue
                    {
                        EDD_UPPER_RQB_PTR_TYPE     const  pHelpRQB          = (EDD_UPPER_RQB_PTR_TYPE)(pRQB->internal_context_Prio); //here fragmentation buffer is handled as RQB
                        EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;

                        //store PrioIndex for later deallocation
                        pHelpRQB->internal_context_Prio = PrioIndex;

                        //put big fragmentation buffer to Frag Dealloc Queue
                        EDDI_AddToQueueEnd(pDDB, pFragDeallocQueue, pHelpRQB);
                    }

                    //update statistic
                    pNrtTxFragDataDDB->Statistic[pNrtTxFragDataDDB->ActiveHwPortIndex].SentFragmentedFrames++;

                    if //releasing TX semaphore not successful?
                       (!EDDI_NRTTxFragSemaphore(pDDB, EDDI_TX_FRAG_SEMA_RELEASE, QueueIdx, &NextFragInterfaceNr))
                    {
                        EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, releasing TX Frag semaphore not successful!, QueueIdx:0x%X, NextFragInterfaceNr:0x%X, NRTChannel:%d", 
                            QueueIdx, NextFragInterfaceNr, pIF->NRTChannel);
                        EDDI_Excp("EDDI_INTNRTTxDone, releasing TX Frag semaphore not successful!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                        pIF->Tx.UnLockFct(pIF);
                        #endif
                        return;
                    }

                    if //another fragment service is already waiting on another NRT interface?
                       (EDDI_NRT_NO_IF != NextFragInterfaceNr)
                    {
                        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF2 = &pDDB->NRT.IF[NextFragInterfaceNr];

                        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                        pIF2->Tx.LockFct(pIF2);
                        #endif

                        EDDI_NRTSendWithLimit(pDDB, pIF2); //try to reload the next fragment service of another NRT interface!

                        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                        pIF2->Tx.UnLockFct(pIF2);
                        #endif
                    }

                    //indicate RQB
                }
            }
            else //no FRAG RQB
            {
                //update statistic
                pNrtTxFragDataDDB->Statistic[pNrtTxFragDataDDB->ActiveHwPortIndex].SentNonFragmentedFrames++;
                //indicate RQB
            }
        }
        #endif //EDDI_CFG_FRAG_ON

        pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        if (pHDB == EDDI_NULL_PTR)
        {
            pRQBSnd->LineDelayInTicksHardware = 1; //to prevent adding to queue twice
            continue;
        }
        #endif

        EDD_RQB_SET_RESPONSE(pRQB, TxStatus);
        EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

        Service = EDD_RQB_GET_SERVICE(pRQB);

        //for SYNC we send with or without timestamp depending on TxTimePresent set
        //TxTimePresent was set by EDDI on request entry depending on FrameID

        if (   (Service == EDD_SRV_NRT_SEND_TIMESTAMP)
            || ((Service == EDD_SRV_SYNC_SEND) && (pRQBSnd->TxTimePresent)))
        {
            //IRTE-Service NRT_SEND_TIMESTAMP

            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            if (LocalL0 & BM_S_DMACW_SND_PTFrame_26)
            {
                LSA_UINT32  const TimeLoc = pDMACW->Time;
                pRQBSnd->TxTime                   = EDDI_IRTE2HOST32(TimeLoc);
                pRQBSnd->LineDelayInTicksHardware = 0;
            }
            else
            {
                pRQBSnd->LineDelayInTicksHardware = 0xFFFFFFFFUL;
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_INTNRTTxDone->EDD_STS_ERR_TX, Frame is no PTFrame, DMACW:0x%X, Service:0x%X, QueueIdx:%u",
                                      LocalL0, Service, QueueIdx);
                EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_TX);
            }
            #elif defined (EDDI_CFG_REV5)
            {
                LSA_UINT32  const  Caller = EDDI_TS_CALLER_TX_DONE;

                EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_EDD_ISR_NRT_C, Caller, pDDB->NRT.TimeStamp.State);

                switch (pDDB->NRT.TimeStamp.State)
                {
                    case EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS:
                    {
                        pDDB->NRT.TimeStamp.cTimeStamp = 1; //start timeout timer
                        pDDB->NRT.TimeStamp.State      = EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT;
                        EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_EDD_ISR_NRT_C, Caller, pDDB->NRT.TimeStamp.State);
                        //continue with TimeStamp interrupt or timeout!
                        continue;
                    }

                    case EDDI_TS_STATE_TS_INT_AWAIT_TX_DONE:
                    {
                        //TimeStamp interrupt has already occured! The sequence of NRTTxDone interrupt and TimeStamp interrupt is no longer deterministic!
                        //TS-RQB is ready and can be send back
                        EDDI_NRTTimeStamp(pDDB, Caller);
                        continue;
                    }

                    case EDDI_TS_STATE_CLOSING:
                    {
                        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK_CANCEL);
                        break;
                    }

                    case EDDI_TS_STATE_NO_REQ:
                    case EDDI_TS_STATE_CHECK_LIST_STATUS:
                    case EDDI_TS_STATE_RECOVER:
                    case EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT:
                    default:
                    {
                        EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, invalid TS-State:0x%X, NRTChannel:%d", 
                            pDDB->NRT.TimeStamp.State, pIF->NRTChannel);
                        EDDI_Excp("EDDI_INTNRTTxDone, invalid TS-State:", EDDI_FATAL_ERR_EXCP, pDDB->NRT.TimeStamp.State, 0);
                        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
                        pIF->Tx.UnLockFct(pIF);
                        #endif
                        return;
                    }
                }
            }
            #else
            #error "REV not yet implemented!"
            #endif
        }

        if (!(0 == pHDB->Cbf))
        {
            EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);

            pHDB->TxCount--;

            if (    (0 == pHDB->TxCount) 
                &&  (0 == pHDB->intTxRqbCount)
                &&  (0 == pHDB->intRxRqbCount) 
                &&  (pHDB->pRQBTxCancelPending) )
            {
                EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                pHDB->pRQBTxCancelPending = LSA_NULL;

                //Inform User thats all Frames are sent. The TxCancel can give back now!
                EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_INTNRTTxDone-> Cancel in Progress <---");
            }
            else if (pHDB->TxCount < 0)
            {
                EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "pHDB->TxCount (0x%X) is negativ!, NRTChannel:%d", 
                    pHDB->TxCount, pIF->NRTChannel);
                EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
            }
        }
        else
        {
            EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_INTNRTTxDone, no Cbf in HDB! Handle:0x%X, UpperHandle:0x%X, NRTChannel:%d", 
                pHDB->Handle, pHDB->UpperHandle, pIF->NRTChannel);
            EDDI_Excp("EDDI_INTNRTTxDone, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
            #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
            pIF->Tx.UnLockFct(pIF);
            #endif
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_isr_nrt.c                                               */
/*****************************************************************************/
