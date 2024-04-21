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
/*  F i l e               &F: eddi_crt_cons.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*              19.03.2004  ZA    Created for New Consumer-Tables and Entries*/
/*  09.04.2004  ZR    provide ui32SBTimerAdr, SBTimerRelValue and pLowerCtrlF*/
/*                                      with SBTimer use the ConsumerListType*/
/*       29.10.2007  JS    Quick Hack for error fix (see JS_NOFIX) -> Verfiy!*/
/*                   27.06.2008  JS    added ENTER_CRITICAL and EXIT_CRITICAL*/
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_csrt_ext.h"
#include "eddi_irt_ext.h"
#include "eddi_crt_ext.h"
#include "eddi_ser_ext.h"

#include "eddi_crt_check.h"
#include "eddi_ser_cmd.h"

#if defined (EDDI_CFG_USE_SW_RPS)
#include "eddi_crt_sb_sw_tree.h"
#endif

#include "eddi_crt_com.h"

#include "eddi_prm_record_pdir_data.h"
//#include "eddi_prm_record_common.h"
#include "eddi_crt_dfp.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_cons.h"
#endif

#include "eddi_sync_ir.h"
#include "eddi_pool.h"
#include "eddi_lock.h"

#define EDDI_MODULE_ID     M_ID_CRT_CONS
#define LTRC_ACT_MODUL_ID  103

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTDataImage( LSA_UINT16                       const FCode,
                                                       EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                       EDDI_LOCAL_DDB_PTR_TYPE          const pDDB );
#endif

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerInit( EDDI_CRT_CONSUMER_PTR_TYPE            const pConsumer,
                                                            EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const pConsParam,
                                                            EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                            LSA_UINT8                             const ListType,
                                                            LSA_UINT8                             const ConsumerType );

/*===========================================================================*/
/*             functions  CRTConsumer-"Class"                                */
/*===========================================================================*/

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerAddEvent( EDDI_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                EDD_UPPER_RQB_PTR_TYPE     const pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                                LSA_UINT8                  const ListType,
                                                                LSA_UINT8                  const ConsumerType );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerInitSingleBuffer( EDDI_CRT_CONSUMER_PTR_TYPE            const pConsumer,
                                                                      LSA_UINT32                            const DataOffset,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                                      LSA_UINT8                             const ConsumerType );

static LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerIsInitialized( EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerCheckData( EDDI_CRT_CONSUMER_PTR_TYPE           const pConsumer,
                                                                 EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const pFrmHandler,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE              const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERResetRedundancyAndTsbEntry( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                                        EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                        EDDI_CRT_CONS_HWWD               const eHwWatchdog );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERResetRedundancyAndTsbEntry1B( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                             EDDI_CRT_CONS_HWWD               const eHwWatchdog);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SEREvalReloadWDValue( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                  EDDI_CRT_CONS_HWWD                const  eHwWatchdog,
                                                                  LSA_UINT32                      * const  pLocalTimerSBEntry0,
                                                                  LSA_UINT32                      * const  pLocalTimerSBEntry1);

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERClearTsbEntry( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const pConsumer );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerAddListTypeACW( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                      EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer);

static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetToUnknown( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                     EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsSet2UK,
                                                                     EDDI_CRT_CONS_HWWD                    const  eHwWatchdog,
                                                                     LSA_BOOL                              const  bTrigConsSBSM );

static  LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerClearEvents( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                   EDDI_CRT_EVENT_TYPE         const  Event,
                                                                   LSA_UINT16                  const  CycleCnt );

static  LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetEvents( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                 EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                 EDDI_CRT_EVENT_TYPE         const  SetEvent,
                                                                 EDDI_CRT_EVENT_TYPE         const  RemoveEvent );

static  LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerIRTtopEventEvaluation( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                             EDDI_CRT_CONSUMER_PTR_TYPE           const  pConsumer,
                                                                             EDDI_CRT_EVENT_TYPE                  const  Event );

static  LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckConsumerPhase( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                                   EDDI_CONST_CRT_CONSUMER_PTR_TYPE       const pConsumer );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTConsumerAddToTree( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                    EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTWatchDogInit( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE               const  pConsumer );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlCheckParamsForActivate( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                        EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE  const  pRQBParam,
                                                                                        EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlActivateRTC123( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer,
                                                                                LSA_UINT8                                 const  ConsumerType,
                                                                                LSA_UINT16                                const  Mode,
                                                                                LSA_BOOL                                * const  pbActivated );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlActivateRTC3( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                              EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer,
                                                                              LSA_UINT16                                const  Mode);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlSetToUnknown( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer);


static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CheckLegacyConsistencyDataStatus(EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer);

/******************************************************************************
 *  Function:    EDDI_SRTWatchDogInit()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTWatchDogInit( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer)
                                                                       
{
    LSA_RESULT  const  Status = EDD_STS_OK;

    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        LSA_RESULT  const  Result = EDDI_DFPWatchDogInit(pDDB, pConsumer);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerInit, EDDI_DFPWatchDogInit failed. Result:0x%X, ConsID:0x%X, FrameID:0x%X", 
                              Result, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
        }
    }
    else
    {
        LSA_UINT32                        WDReloadValue;
        EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  pCRTComp;
        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        LSA_UINT32                        DHReloadValue;
        #endif
        
        pCRTComp = pDDB->pLocal_CRT;  /* has to be setup! not checked here */

        if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
        {
            //binary    RR of the RTC3-Consumer: AUX-Cons gets RR=128 and a WD-Time > 256ms
            //nonbinary RR of the RTC3-Consumer: AUX-Cons gets RR=rr_rtc3 and a WD-Time > 256ms
            if (EDDI_RedIsBinaryValue(pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CycleReductionRatio))
            {
                pConsumer->LowerParams.CycleReductionRatio = EDDI_AUX_PROVIDER_RR; //fixed RR
            }
            else
            {
                pConsumer->LowerParams.CycleReductionRatio  = pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CycleReductionRatio;
            }
            //1st: inherit WDF/DHF from RTC3-consumer 
            pConsumer->LowerParams.DataHoldFactor = pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.DataHoldFactor;
            
            //2nd: WD-Timeout has to be at least 256ms
            if ((pConsumer->LowerParams.DataHoldFactor * pConsumer->LowerParams.CycleReductionRatio * pDDB->CycCount.CycleLength_us) < (EDDI_AUX_MIN_WDT_TIMEOUT_MS * 1000UL))
            {
                //calc WD-Timeout to be >256ms
                pConsumer->LowerParams.DataHoldFactor = (LSA_UINT16)((EDDI_AUX_MIN_WDT_TIMEOUT_MS * 1000UL)/((LSA_UINT32)pConsumer->LowerParams.CycleReductionRatio * pDDB->CycCount.CycleLength_us)) + 1;
            }
            //3rd: WD-Timeout has to be max 1920ms
            else if ((pConsumer->LowerParams.DataHoldFactor * pConsumer->LowerParams.CycleReductionRatio * pDDB->CycCount.CycleLength_us) > (EDDI_MAX_WDT_TIMEOUT_MS * 1000UL))
            {
                //calc WD-Timeout to be <=1920ms
                //set RR to 1 again to get the most accurate results.
                pConsumer->LowerParams.DataHoldFactor = (LSA_UINT16)((EDDI_MAX_WDT_TIMEOUT_MS * 1000UL)/pDDB->CycCount.CycleLength_us);
                //calculate the highest possible RR to minimize swsb-load:
                pConsumer->LowerParams.CycleReductionRatio = 1;
                do
                {
                    //check if WDF can be divided by 2 
                    if (0 == (pConsumer->LowerParams.DataHoldFactor & 1))
                    {
                        pConsumer->LowerParams.DataHoldFactor = pConsumer->LowerParams.DataHoldFactor >> 1;
                        pConsumer->LowerParams.CycleReductionRatio = (LSA_UINT16)(pConsumer->LowerParams.CycleReductionRatio << 1);
                    }
                    else
                    {
                        break;
                    }
                }
                while (pConsumer->LowerParams.CycleReductionRatio < EDDI_AUX_PROVIDER_RR);
            }
        }
        
        WDReloadValue = EDDI_CRTCheckGetWDDHReloadValue(pConsumer->LowerParams.ConsumerType, pConsumer->LowerParams.DataHoldFactor, pConsumer->LowerParams.CycleReductionRatio, &pConsumer->LowerParams.TSBEntry_RR);
        if (WDReloadValue > EDDI_MAX_RELOAD_TIME_VALUE)
        {
            EDDI_Excp("EDDI_CRTConsumerInit, WDReloadValue > EDDI_MAX_RELOAD_TIME_VALUE",
                      EDDI_FATAL_ERR_EXCP, pConsumer->LowerParams.DataHoldFactor, pConsumer->LowerParams.CycleReductionRatio);
            return EDD_STS_ERR_EXCP;
        }

        pConsumer->LowerParams.WDReloadVal = WDReloadValue;

        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        {
            LSA_UINT32  NewCRR;

            DHReloadValue = EDDI_CRTCheckGetWDDHReloadValue(pConsumer->LowerParams.ConsumerType, pConsumer->LowerParams.DataHoldFactor, pConsumer->LowerParams.CycleReductionRatio, &NewCRR);

            if (   (DHReloadValue > EDDI_MAX_RELOAD_TIME_VALUE)
                || (NewCRR != pConsumer->LowerParams.TSBEntry_RR))
            {
                EDDI_Excp("EDDI_CRTConsumerInit, DHReloadValue > EDDI_MAX_RELOAD_TIME_VALUE",
                          EDDI_FATAL_ERR_EXCP, pConsumer->LowerParams.DataHoldFactor, pConsumer->LowerParams.CycleReductionRatio);
                return EDD_STS_ERR_EXCP;
            }
        
            pConsumer->LowerParams.DHReloadVal = DHReloadValue;
        }
        #endif

        pConsumer->PendingIndEvent          = 0;
        pConsumer->PendingCycleCnt          = 0;
        pConsumer->PendingTransferStatus    = 0;
        pConsumer->bAUXTriggeredMiss        = LSA_FALSE;
        pConsumer->EventStatus              = 0;

        #if defined (EDDI_CFG_REV7)
        if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
        {
            pConsumer->LowerParams.SCRR_ReloadValue = 32;
        }
        else
        {
            pConsumer->LowerParams.SCRR_ReloadValue = 1;
        } 
        pConsumer->LowerParams.SCRR_ReloadValue   *= (pConsumer->LowerParams.TSBEntry_RR  *  pCRTComp->CycleBaseFactor);
        pConsumer->LowerParams.SCRR_ReloadValue   /= pDDB->CycCount.Entity;
        pConsumer->LowerParams.SCRR_ReloadValue--;
        #endif

        pConsumer->pSB->OldDataStatus = (pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
        // Get related HW-TimerSBEntry
        pConsumer->LowerParams.pSBTimerEntry = &pCRTComp->Rps.SBList.pDevSBTimerBase[pConsumer->ConsumerId];
        pConsumer->LowerParams.ui32SBTimerAdr = DEV_kram_adr_to_asic_register(pConsumer->LowerParams.pSBTimerEntry, pDDB);

        #if defined (EDDI_CFG_USE_SW_RPS)
        EDDI_CRTSbSwAdd(pDDB, pConsumer);
        #endif

    }
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SRTConsumerAddToTree()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTConsumerAddToTree( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                    EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer )
                                                                       
{
    LSA_UINT8   const  ListType = pConsumer->LowerParams.ListType;
    LSA_RESULT  const  Status   = EDD_STS_OK;
    
    switch (ListType)
    {     
        case EDDI_LIST_TYPE_ACW:
        {
            //Add normal ACW-Consumer
            EDDI_CRTConsumerAddListTypeACW(pDDB, pConsumer); 
            break;
        }

        case EDDI_LIST_TYPE_FCW:
        {
            //only add aux-consumer at this time, class3-cons will be completed during CONSUMER_CONTROL
            if (pConsumer->IRTtopCtrl.pAscConsumer)
            {
                //Add AUX-ACW-Consumer
                EDDI_CRTConsumerAddListTypeACW(pDDB, pConsumer->IRTtopCtrl.pAscConsumer); 
            }
            break;
        }

        default:
        {
            return EDD_STS_ERR_PARAM;
        }
    }
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERClearTsbEntry()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERClearTsbEntry( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                              EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    EDDI_LOCAL_MEM_PTR_TYPE  const  pMem = (LSA_VOID*)(&pDDB->pLocal_CRT->Rps.SBList.pDevSBTimerBase[pConsumer->ConsumerId]);
    LSA_UINT32               const  Size = sizeof(EDDI_SER10_TIMER_SCORB_TYPE);

    EDDI_MEMSET(pMem, (LSA_UINT8)0, Size);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerAddEvent()
 *
 *  Description: Uses the RQB to initialize the Consumer and starts the enqueuing
 *               of the HW-substructures.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerAddEvent( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                   EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   LSA_UINT8                   const  ListType,
                                                                   LSA_UINT8                   const  ConsumerType )
{
    LSA_RESULT                                       Status;
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE      const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_CRT_CONSUMER_PTR_TYPE                       pAUXConsumer=(EDDI_CRT_CONSUMER_PTR_TYPE)0;  //IRTTOP -> done
    EDD_RQB_CSRT_CONSUMER_ADD_TYPE                   AUXConsParam;
       
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerAddEvent->");

    //Check MRPD (Redundant Frames)
    Status = EDDI_CRTCheckForConsumerRedundantFrames(pDDB, pConsumer, pConsParam);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                  "EDDI_CRTConsumerListAddEntry, -> EDDI_CRTCheckForConsumerRedundantFrames -> ERROR, FrameID:0x%X", pConsParam->FrameID);
        pConsumer->IRTtopCtrl.pAscConsumer = (EDDI_CRT_CONSUMER_PTR_TYPE)0; 
        return Status;
    }

    pConsumer->LowerParams.bKRAMDataBufferValid = LSA_FALSE;
    pConsumer->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)EDDI_NULL_PTR;

    if (   (EDDI_RTC3_CONSUMER == ConsumerType)
        && (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY))
    {
        //Add AUX-Consumer
        Status = EDDI_CRTConsumerListReserveFreeEntry(pDDB, &pAUXConsumer, pDDB->pLocal_CRT, EDDI_LIST_TYPE_ACW, LSA_FALSE /*bIsDFP*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerListAddEntry, ERROR finding free Entry, Status:0x%X", Status);
            return Status;
        }
        //Concatenate Class3-Cons. with AUX-Cons
        pConsumer->IRTtopCtrl.pAscConsumer = pAUXConsumer;
        pAUXConsumer->IRTtopCtrl.pAscConsumer = pConsumer;
        pAUXConsumer->LowerParams.bKRAMDataBufferValid = LSA_FALSE;
        pAUXConsumer->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)EDDI_NULL_PTR;
        
        //fill out AUX-Consumer
        AUXConsParam                            = *pConsParam;  //Copy Class3-ADD-Params
        AUXConsParam.IOParams.BufferProperties  = EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC;
        AUXConsParam.IOParams.DataOffset        = EDD_DATAOFFSET_UNDEFINED; //stays undefined
        AUXConsParam.Partial_DataLen            = 0;
        AUXConsParam.DataHoldFactor             = EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX;
        AUXConsParam.Properties                 = EDDI_CSRT_CONS_PROP_RTCLASS_AUX;

        Status = EDDI_CRTConsumerInit(pAUXConsumer, &AUXConsParam, pDDB, EDDI_LIST_TYPE_ACW, EDDI_RTC3_AUX_CONSUMER);
        if (Status != EDD_STS_OK)
        {
            return Status;
        }
    }
    else
    {
        pConsumer->IRTtopCtrl.pAscConsumer = (EDDI_CRT_CONSUMER_PTR_TYPE)0;
    }

    Status = EDDI_CRTConsumerInit(pConsumer, (EDD_RQB_CSRT_CONSUMER_ADD_TYPE*)pRQB->pParam, pDDB, ListType, ConsumerType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    //return APDUStatus-ptr to user
    if (   (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
        || EDDI_IS_DFP_CONS(pConsumer))
    {
        //DFP-consumers and legacy consumers do not return an APDU-Offset
        pConsParam->IOParams.APDUStatusOffset = EDD_DATAOFFSET_UNDEFINED;
    }
    else
    {
        #ifdef EDDI_PRAGMA_MESSAGE
        #pragma message ("MultiEDDI")
        #endif /* EDDI_PRAGMA_MESSAGE */
        #if defined (EDDI_CFG_REV7)
        //APDUStatusOffset only valid for PAEARAM-use, or single (synchronous) buffer
        if (pConsumer->usingPAEA_Ram)
        {
            //AUX-Consumer-frames have no data, only the APDUStatus resides in KRAM (no PAEARAM)
            //"real"Data-Frames have their APDUStatus in the StateBuffer in KRAM
            pConsParam->IOParams.APDUStatusOffset = (LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer - (LSA_UINT32)pDDB->pKRam;
        }
        else
        {
            pConsParam->IOParams.APDUStatusOffset = (EDD_DATAOFFSET_UNDEFINED != pConsParam->IOParams.DataOffset)?((LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer - (LSA_UINT32)pDDB->pKRam):EDD_DATAOFFSET_UNDEFINED;
        }
        #else
        //APDUStatusOffset is only constant in 1-buffer-interfaces
        pConsParam->IOParams.APDUStatusOffset = (EDD_DATAOFFSET_UNDEFINED != pConsParam->IOParams.DataOffset)?((LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer - (LSA_UINT32)pDDB->pKRam):EDD_DATAOFFSET_UNDEFINED;
        #endif
    }

    //Set ConsumerID for RQB-Response
    pConsParam->ConsumerID = pConsumer->ConsumerId;

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerControlPassivate()
 *
 *  Description: Passivate the Consumer
 *
 *  Arguments:   pConsumer (THIS): the related Consumer
 *               pDDB:
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlPassivate( EDDI_CRT_CONSUMER_PTR_TYPE         pConsumer,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                         LSA_BOOL                    const  bRemoveACWs)
{
    //Consumer is ACTIVE => can only be passivated
    //if a MISS-Event had been handled while the PASSIVATE.req was sent to EDDI, we also end up here!
    if /* Class1/2-Consumer */
       (EDDI_RTC3_CONSUMER != pConsumer->LowerParams.ConsumerType)
    {
        //Passivate consumer, clear all pending events
        EDDI_CRTConsumerPassivate(pDDB, pConsumer, LSA_TRUE /*bClearEvents */, bRemoveACWs);
    }
    else /* class3-Consumer */
    {
        if /* AUX-Consumer is active */
           (   (pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN)
            || bRemoveACWs )
        {
            if /* AUX-Consumer exists */
               (pConsumer->IRTtopCtrl.pAscConsumer != LSA_NULL)
            {
                //Passivate AUX-consumer, clear all pending events
                EDDI_CRTConsumerPassivate(pDDB, pConsumer->IRTtopCtrl.pAscConsumer, LSA_TRUE /*bClearEvents */, bRemoveACWs);
            }
            else if (!bRemoveACWs)
            {
                EDDI_Excp("EDDI_CRTConsumerControlPassivate, pConsumer->IRTtopCtrl.pAscConsumer not valid, pConsumerId:%i pConsumer ptr:0x%X", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer);
                return;                
            }
        }
        if /* Class3 Consumer is active */
           (pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_RTC3_MAIN)
        {
            //Passivate RTC3-consumer, clear all pending events
            EDDI_CRTConsumerPassivate(pDDB, pConsumer, LSA_TRUE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
        }
        EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_PASSIVATE);
    } 
}                                                                      
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerRemoveEvent()
 *
 *  Description: Uses the RQB to remove the pConsumer and starts the de-queuing
 *               of the HW-substructures.
 *
 *  Arguments:   pConsumer (THIS): the related Consumer
 *               pDDB:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerRemoveEvent( EDDI_CRT_CONSUMER_PTR_TYPE         pConsumer,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UINT16  KramDatalen;
    LSA_RESULT  Status = EDD_STS_OK;
    
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerRemoveEvent->");

    //Check Status-Conditions
    if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTConsumerRemoveEvent, ConsID:0x%X is removed while being active.Passivating.", pConsumer->ConsumerId);
        EDDI_CRTConsumerControlPassivate(pConsumer, pDDB, LSA_FALSE /*bRemoveACWs*/);
    }

    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        return EDDI_DFPConsRemovePFHandler(pDDB, pConsumer);
    }
    else
    {       
        //Provider-AutoStop-handling
        if (pConsumer->pNextAutoStopProv != EDDI_NULL_PTR)
        {       
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerRemoveEvent, ERROR AutoStop is enabled. ERROR getting Entry, pRemParam->ConsumerID:0x%X",
                              pConsumer->ConsumerId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_FrmHandler);
            return EDD_STS_ERR_SEQUENCE;
        } 
        Status = EDDI_CrtASCtrlAllProviderList(pDDB, pConsumer, LSA_TRUE /*RemoveASCoupling*/ , pConsumer->bEnableAutoStop /*DeactivateProvider*/ );

        KramDatalen = (pConsumer->LowerParams.Partial_DataLen)?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;

        if (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
        {
            #if defined (EDDI_CFG_REV7)
            if (!pConsumer->usingPAEA_Ram)
            {
                EDDI_CRTCheckAddRemoveMirror(pDDB, KramDatalen, pConsumer->LowerParams.DataOffset, pConsumer->LowerParams.ListType, LSA_FALSE, CRT_MIRROR_REMOVE);
            }
            else
            {
                //When using IOC/PAEA-RAM, the bookkeeping is up to the user
                //Here pKRAMDataBuffer points to a PAEA-APDU structure in the KRAM-Memory.
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTConsumerRemoveEvent, PAEA-APDU Pool, deleting buffer 0x%X",
                                  (LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer);

                //Clean Indirection ConsumerID -> APDU-slot (mark memory slot as free)
                        Status = EDDI_MemFreeApduBufferCons(pConsumer->ConsumerId, pDDB);
                if (Status != EDD_STS_OK)
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerRemoveEvent, Could not remove indircetion between ConsumerID-List and APDU-Slot (PAEA)");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_APDU_INDIRECTION);
                    return Status;
                }
            }
            #else
            EDDI_CRTCheckAddRemoveMirror(pDDB, KramDatalen, pConsumer->LowerParams.DataOffset, pConsumer->LowerParams.ListType, LSA_FALSE, CRT_MIRROR_REMOVE);
            #endif
        }

        switch (pConsumer->LowerParams.ListType)
        {
            case EDDI_LIST_TYPE_FCW:
            {
                EDDI_CRT_CONSUMER_PTR_TYPE  const pAUXConsumer = pConsumer->IRTtopCtrl.pAscConsumer;

                if (pConsumer->pFrmHandler)
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerRemoveEvent, Consumer not passivated? ConsID:0x%X FrameID:0x%X pFrmHandler:0x%X",
                                      pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, (LSA_UINT32)pConsumer->pFrmHandler);
                    return EDD_STS_ERR_SEQUENCE;
                }
                
                //now remove AUX-Consumer
                if /* no AUX-Cons exists */
                   (!pAUXConsumer)
                {               
                    break;
                }
                else
                {
                    pConsumer = pAUXConsumer; //for fallthrough to next case
                }
            }
            //no break;
            //lint -fallthrough

            case EDDI_LIST_TYPE_ACW:
            {
                if (EDDI_NULL_PTR != pConsumer->pLowerCtrlACW)
                {
                    if (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
                    {
                        EDDI_CRTDataImage(FCODE_DISABLE_DATA_IMAGE, pConsumer, pDDB);
                    }

                    if (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER)
                    {
                        EDDI_SRTConsumerRemove(pDDB, pConsumer->pLowerCtrlACW, pConsumer->LowerParams.pKRAMDataBuffer);
                    }
                    else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
                    {
                        EDDI_SRTConsumerRemove(pDDB, pConsumer->pLowerCtrlACW, EDDI_NULL_PTR);
                    }

                    pConsumer->pLowerCtrlACW  = EDDI_NULL_PTR;
                }
                else
                {
                    if (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER)
                    {
                        //deallocate aux apdu-buffer
                        if (pConsumer->LowerParams.pKRAMDataBuffer)
                        {
                            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.ApduAuxConsumer, pConsumer->LowerParams.pKRAMDataBuffer);
                        }
                    }
                }            
                
                pConsumer->pSB->Status = EDDI_CRT_CONS_STS_RESERVED;

                break;
            }
            default:
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerRemoveEvent, ListTyp Error, ListType:0x%X", pConsumer->LowerParams.ListType);
                EDDI_Excp("EDDI_CRTConsumerRemoveEvent, ListTyp Error, ListType:", EDDI_FATAL_ERR_EXCP, pConsumer->LowerParams.ListType, 0);
                return EDD_STS_ERR_PARAM;   
            }
        }
        return Status;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerControlEvent()
 *
 *  Description: Changes the ConsumerStatus
 *
 *  Arguments:   pConsumer (THIS): the current Consumer
 *               bActivate:        bActivate == FALSE dominates the SetToUnknownState-Bit .
 *               SetToUnknownState:if SetToUnknownState == TRUE && bActivate == TRUE
 *                                 the RxState is set to UnknownState. This forces the
 *                                 Consumer-Statemachine to fire a MISS or AGAIN-Indication,
 *                                 depending on the RxState.
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlEvent( EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer,
                                                               EDD_UPPER_RQB_PTR_TYPE                    const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                               LSA_UINT8                                 const  ConsumerType )
{
    EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE  const  pRQBParam  = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE)pRQB->pParam;
    LSA_BOOL                                         bActivated = LSA_FALSE;
    LSA_RESULT                                       Status     = EDD_STS_OK;
    LSA_UINT16                                const  Mode       = pRQBParam->Mode;                       
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent->");

    //******************************************************************************************************************
    // ACTIVATING a passive consumer
    //******************************************************************************************************************
    if (Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) // Activation
    {
        if (EDDI_CRT_CONS_STS_INACTIVE == pConsumer->pSB->Status)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent-> Activating Consumer, ConsumerID:0x%X ", pConsumer->ConsumerId);
            
            //Check if all parameters needed for activate are valid
            Status = EDDI_CRTConsumerControlCheckParamsForActivate(pDDB, pRQBParam, pConsumer);
            if (EDD_STS_OK != Status)
            {
                //Traces and detail have already been set in EDDI_CRTConsumerControlCheckParamsForActivate 
                return Status;
            }             

            //************************************************************************************
            //from here on, all parameters are supposed to be valid.
            //************************************************************************************
            Status = EDDI_CRTConsumerControlActivateRTC123(pDDB, pConsumer, ConsumerType, Mode, &bActivated);
            if (EDD_STS_OK != Status)
            {
                //Traces and detail have already been set in EDDI_CRTConsumerControlActivate 
                return Status;
            }             
            
        } //end if ( EDDI_CRT_CONS_STS_INACTIVE == pConsumer->pSB->Status)
    }

    //******************************************************************************************************************
    // PASSIVATE
    //******************************************************************************************************************
    if (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE) // Deactivation
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent-> Passivate (Main)Consumer, ConsumerID:0x%X ", pConsumer->ConsumerId);
        if (   (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
            || (Mode & EDD_CONS_CONTROL_MODE_REMOVE_ACW) )
        {        
            EDDI_CRTConsumerControlPassivate(pConsumer, pDDB, (Mode & EDD_CONS_CONTROL_MODE_REMOVE_ACW)?LSA_TRUE:LSA_FALSE/*bRemoveACWs*/);
        }   
        // if only main Consumer is passive, but the Aux Consumer is active
        else if (   (pConsumer->IRTtopCtrl.pAscConsumer) 
                 && (pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN) )
        {
            if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->IRTtopCtrl.pAscConsumer->pSB->Status)
            {
                EDDI_CRTConsumerControlPassivate(pConsumer->IRTtopCtrl.pAscConsumer, pDDB, (Mode & EDD_CONS_CONTROL_MODE_REMOVE_ACW)?LSA_TRUE:LSA_FALSE/*bRemoveACWs*/);
            }
        }
        pConsumer->bHasSeenAgain = LSA_FALSE;
    }

    //******************************************************************************************************************
    //Set enable or disable AutoStop
    //******************************************************************************************************************
    #if !defined (EDDI_CFG_DISABLE_PROVIDER_AUTOSTOP)
    if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE)
    {
        pConsumer->bEnableAutoStop = LSA_TRUE;
    }
    else if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE)
    {
        pConsumer->bEnableAutoStop = LSA_FALSE;
    }
    #endif //!defined (EDDI_CFG_DISABLE_PROVIDER_AUTOSTOP)

    //******************************************************************************************************************
    //SetToUnknownState-Handling 
    //******************************************************************************************************************
    if (   (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN)
        && (!bActivated))
    {
        EDDI_CRTConsumerControlSetToUnknown(pDDB, pConsumer);
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:     EDDI_CRTConsumerSetPendingEvent()
 *
 *  Description: Stores the Current Event for this Consumer
 *
 *               Event:
 *                      Maximum one Event of each pare can be set at the same time.
 *                      Following Event-Bit-Pares are handled:
 *                        MISS       <--> AGAIN
 *                        RUN        <--> STOP
 *                        STATION_OK <--> STATION_FAILURE
 *                        PRIMARY    <--> BACKUP
 *
 *                      Event 0 resets the whole Event Bitmask
 *
 *               CycleCnt:
 *                      Only will be stored if Event == 0 or pConsumer->PendingIndEvent changed.
 */

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetPendingEvent( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                EDDI_CRT_EVENT_TYPE         const  Event,
                                                                LSA_UINT16                  const  CycleCnt,
                                                                LSA_UINT8                   const  TransferStatus,
                                                                EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_CRT_EVENT_TYPE  const  OldEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
    EDDI_CRT_EVENT_TYPE  const  ClearAgainMissEvent = EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_AGAIN | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
    EDDI_CRT_EVENT_TYPE         NewEvent; 
    
    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTConsumerSetPendingEvent->FrameId:0x%X Event:0x%X ConsumerID:0x%X", 
                      pConsumer->LowerParams.FrameId, Event, pConsumer->ConsumerId);

    //check if any events shall be cleared (MSB is set)
    if (Event & EDDI_CSRT_CONS_EVENT_NEGATION)
    {
        EDDI_CRTConsumerClearEvents(pDDB, pConsumer, Event, CycleCnt);
    }
    else
    {
        switch (Event)
        {
            case EDD_CSRT_CONS_EVENT_MISS:
            case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
            {
                if(   (EDDI_RTC1_CONSUMER == pConsumer->LowerParams.ConsumerType)    //Class 1/2
                   || (EDDI_RTC2_CONSUMER == pConsumer->LowerParams.ConsumerType))
                {
                    if (EDD_CSRT_CONS_EVENT_MISS == Event)
                    {
                        //SYSRED consumers get set to state BACKUP when passive
                        //if (pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)
                        //{
                        //    EDDI_CRTConsumerSetEvents(pDDB, pConsumer, EDD_CSRT_CONS_EVENT_BACKUP, EDD_CSRT_CONS_EVENT_PRIMARY);
                        //}

                        //Passivate RTC1/2 Consumer (Consumer-AS)
                        EDDI_CRTConsumerPassivate(pDDB, pConsumer, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                    }
                    EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, ClearAgainMissEvent);
                }
                else if (   (EDDI_RTC3_CONSUMER     == pConsumer->LowerParams.ConsumerType)
                         || (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType))
                {
                    EDDI_CRTConsumerIRTtopEventEvaluation(pDDB, pConsumer, Event);
                }
                break;
            }

            case EDD_CSRT_CONS_EVENT_AGAIN:
            {
                if(   (EDDI_RTC1_CONSUMER == pConsumer->LowerParams.ConsumerType)    //Class 1/2
                   || (EDDI_RTC2_CONSUMER == pConsumer->LowerParams.ConsumerType))
                {
                    EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, ClearAgainMissEvent);
                }
                else if(   (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)   //Class 3/AUX
                        || (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType))
                {
                    EDDI_CRTConsumerIRTtopEventEvaluation(pDDB, pConsumer, Event);
                }
                break;
            }

            case EDD_CSRT_CONS_EVENT_ENTERING_RED:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, 0);
                break;
            } 

            case EDD_CSRT_CONS_EVENT_RUN:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_STOP);
                break;
            }

            case EDD_CSRT_CONS_EVENT_STOP:
            {

                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_RUN);
                break;
            }

            case EDD_CSRT_CONS_EVENT_STATION_OK:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_STATION_FAILURE);
                break;
            }

            case EDD_CSRT_CONS_EVENT_STATION_FAILURE:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_STATION_OK);
                break;
            }

            case EDD_CSRT_CONS_EVENT_PRIMARY:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_BACKUP);
                break;
            }

            case EDD_CSRT_CONS_EVENT_BACKUP:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_PRIMARY);
                break;
            }

            case EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS);
                break;
            }

            case EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS:
            {
                EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS);
                break;
            }

            case EDDI_CSRT_CONS_EVENT_NO:
            {
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_CRTConsumerSetPendingEvent, unknown Event", EDDI_FATAL_ERR_EXCP, Event, pConsumer->LowerParams.FrameId);
                return;
            }
        }
    }

    //anything changed?
    //Note: events are always generated for RTC1/2/3-consumers, never for AUX-consumers!
    //      all events from AUX-consumers are mapped to their corresponding RTC3-consumer.
    //      events regarding packframes will never appear here!
    NewEvent = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent:pConsumer->PendingIndEvent;
    if (OldEvent != NewEvent)
    {
        if (pConsumer->IRTtopCtrl.pAscConsumer != LSA_NULL)
        {
            // Store CycleCnt
            pConsumer->IRTtopCtrl.pAscConsumer->PendingCycleCnt       = CycleCnt;
            pConsumer->IRTtopCtrl.pAscConsumer->PendingTransferStatus = TransferStatus;
        }
        if (NewEvent & (EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED | EDD_CSRT_CONS_EVENT_MISS) )
        {
            if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
            {
                pConsumer->IRTtopCtrl.pAscConsumer->bAUXTriggeredMiss = LSA_TRUE;
            }
            else
            {
                pConsumer->bAUXTriggeredMiss = LSA_FALSE;
            }
        }

        // Store CycleCnt
        pConsumer->PendingCycleCnt       = CycleCnt;
        pConsumer->PendingTransferStatus = TransferStatus;

        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDI_CRTConsumerSetPendingEvent, PendingIndEvent changed, FrameId:0x%X CycleCnt:0x%X OldEvent:0x%X NewEvent:0x%X",
                          pConsumer->LowerParams.FrameId, CycleCnt, OldEvent, NewEvent);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERResetRedundancyAndTsbEntry( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                           EDDI_CRT_CONS_HWWD                const  eHwWatchdog )

{
    EDDI_SERResetRedundancyAndTsbEntry1B(pDDB, pConsumer, eHwWatchdog);
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
/******************************************************************************
 *  Function:   EDDI_SERResetRedundancyAndTsbEntry1B()
 *
 *  Description:Only working for Single-Buffer !!
 *
 *  Arguments:  pDDB
 *              pConsumer 
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERResetRedundancyAndTsbEntry1B( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                             EDDI_CRT_CONS_HWWD                const  eHwWatchdog )
{
    volatile EDDI_SER10_TIMER_SCORB_TYPE  *  pKramTimerSBEntry;
    EDDI_SER10_TIMER_SCORB_TYPE              LocalTimerSBEntry;
    volatile EDDI_SER10_TIMER_SCORB_TYPE     ReadBackTimerSBEntry;
    LSA_UINT8                                LocalTRF;             // TransferStatus
    volatile LSA_UINT8                    *  pKramTRF;             // TransferStatus
    volatile LSA_UINT8                       ReadBackTRF;          // TransferStatus
    LSA_UINT32                               TryCnt;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERResetRedundancyAndTsbEntry1B->");

    //Get Ptr WatchdogTimerEntry
    pKramTimerSBEntry = (EDDI_SER10_TIMER_SCORB_TYPE *)(void *)pConsumer->LowerParams.pSBTimerEntry;
    pKramTRF          = &((EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer)->Detail.TransferStatus;

    #if defined (EDDI_CFG_REV7)
    EDDI_SEREvalReloadWDValue(pDDB, pConsumer, eHwWatchdog, &LocalTimerSBEntry.Reg[0], &LocalTimerSBEntry.Reg[1]);
    #else //Rev6
    EDDI_SEREvalReloadWDValue(pDDB, pConsumer, eHwWatchdog, &LocalTimerSBEntry.Reg[0], 0);
    #endif
    
    //LocalTimerSBEntry.---.WatchdogTimer = 0; // No AGAIN-bit is available at REV5. WDT is ot loaded.
    LocalTRF = EDDI_SER_RED_STATUS_INIT_VALUE;

    ReadBackTimerSBEntry.Reg[0] = 0;
    #if defined (EDDI_CFG_REV7)
    ReadBackTimerSBEntry.Reg[1] = 0;
    #endif

    ReadBackTRF = 0;

    /* The following sequence is timecritical. We must be sure that */
    /* we are able to successfully finish the loop so we must not   */
    /* be interrupted for too long or too often!                    */

    EDDI_ENTER_CRITICAL_S();

    for (TryCnt = 0; TryCnt < EDDI_CNS_TRY_CNT; TryCnt++)
    {
        // Overwriting TimerScoreboardEntries and APDU-Status for the 1st time
        pKramTimerSBEntry->Reg[0] = LocalTimerSBEntry.Reg[0];
        #if defined (EDDI_CFG_REV7)
        pKramTimerSBEntry->Reg[1] = LocalTimerSBEntry.Reg[1];
        #endif
        *pKramTRF = LocalTRF; // only works with single Bufferd Consumer !!

        EDDI_WAIT_10_NS(pDDB->hSysDev, 10UL);

        // Read back
        ReadBackTimerSBEntry.Reg[0] = pKramTimerSBEntry->Reg[0];
        #if defined (EDDI_CFG_REV7)
        ReadBackTimerSBEntry.Reg[1] = pKramTimerSBEntry->Reg[1];
        #endif

        ReadBackTRF = *pKramTRF;

        if (   (ReadBackTimerSBEntry.Reg[0] != LocalTimerSBEntry.Reg[0])
            #if defined (EDDI_CFG_REV7)
            || (ReadBackTimerSBEntry.Reg[1] != LocalTimerSBEntry.Reg[1])
            #endif
            || (ReadBackTRF != LocalTRF))
        {
            // IRTE has changed the values in the same moment than we did
            // INCONSISTENCY => write again!
            // EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
            //                   "EDDI_SERResetRedundancyAndTsbEntry1B, Retry Resetting TimerScoreboardentry ReadBackAPDU:0x%X TryCnt:0x%X",
            //                   ReadBackTRF, TryCnt);
            continue;
        }
        else
        {
            // OK: values have been consistently written
            EDDI_EXIT_CRITICAL_S();
            return;
        }
    }

    EDDI_EXIT_CRITICAL_S();

    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "EDDI_SERResetRedundancyAndTsbEntry1B, CONSISTENCY Check - CRITICAL LOCK ISSUE - ERROR on resetting TimerScoreboardentry ReadBackTimerSBEntry.Reg[0]:0x%X LocalTimerSBEntry.Reg[0]:0x%X ReadBackAPDU:0x%X",
                        ReadBackTimerSBEntry.Reg[0], LocalTimerSBEntry.Reg[0], ReadBackTRF);
                          
    EDDI_Excp("EDDI_SERResetRedundancyAndTsbEntry1B - CONSISTENCY Check - CRITICAL LOCK ISSUE", EDDI_FATAL_ERR_EXCP, ReadBackTimerSBEntry.Reg[0], LocalTimerSBEntry.Reg[0]);
}
/*---------------------- end [subroutine] ---------------------------------*/


#else //==Rev5


/******************************************************************************
 *  Function:   EDDI_SERResetRedundancyAndTsbEntry1B()
 *
 *  Description:Only working for Single-Buffer !!
 *
 *  Arguments:  pDDB
 *              pConsumer 
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERResetRedundancyAndTsbEntry1B( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                             EDDI_CRT_CONS_HWWD                const  eHwWatchdog)
{
    volatile EDDI_SER10_TIMER_SCORB_TYPE  *  pKramTimerSBEntry;
    EDDI_SER10_TIMER_SCORB_TYPE              LocalTimerSBEntry;
    LSA_UINT32                               u32LocalTimerSBEntry;
    volatile EDDI_SER10_TIMER_SCORB_TYPE     ReadBackTimerSBEntry;
    LSA_UINT8                                LocalTRF;             // TransferStatus
    volatile LSA_UINT8                    *  pKramTRF;             // TransferStatus
    volatile LSA_UINT8                       ReadBackTRF;          // TransferStatus
    LSA_UINT32                               TryCnt;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERResetRedundancyAndTsbEntry1B->");

    // Get Ptr WatchdogTimerEntry
    pKramTimerSBEntry = (EDDI_SER10_TIMER_SCORB_TYPE *)(void *)pConsumer->LowerParams.pSBTimerEntry;
    pKramTRF          = &((EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer)->Detail.TransferStatus;

    EDDI_SEREvalReloadWDValue(pDDB, pConsumer, eHwWatchdog, &u32LocalTimerSBEntry, 0);
    LocalTimerSBEntry.Reg = (LSA_UINT16)u32LocalTimerSBEntry;

    //LocalTimerSBEntry.---.WatchdogTimer = 0; // No AGAIN-bit is available at REV5. WDT is ot loaded.
    LocalTRF = EDDI_SER_RED_STATUS_INIT_VALUE;

    ReadBackTimerSBEntry.Reg = 0;
    ReadBackTRF              = 0;

    /* The following sequence is timecritical. We must be sure that */
    /* we are able to successfully finish the loop so we must not   */
    /* be interrupted for too long or too often!                    */

    EDDI_ENTER_CRITICAL_S();

    for (TryCnt = 0; TryCnt < EDDI_CNS_TRY_CNT; TryCnt++)
    {
        // Overwriting TimerScoreboardEntries and APDU-Status for the 1st time
        pKramTimerSBEntry->Reg = LocalTimerSBEntry.Reg;
        *pKramTRF              = LocalTRF; // only works with single Buffered Consumer !!

        EDDI_WAIT_10_NS(pDDB->hSysDev, 10UL);

        // 1st readback
        ReadBackTimerSBEntry.Reg = pKramTimerSBEntry->Reg;
        ReadBackTRF              = *pKramTRF;

        if (   (ReadBackTimerSBEntry.Reg != LocalTimerSBEntry.Reg)
            || (ReadBackTRF              != LocalTRF))
        {
            // IRTE has changed the values in the same moment than we did
            // INCONSISTENCY => write again!
            // EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
            //                   "EDDI_SERResetRedundancyAndTsbEntry1B, Retry Resetting TimerScoreboardentry, ReadBackAPDU:0x%X TryCnt:0x%X",
            //                   ReadBackTRF, TryCnt);
            continue;
        }
        else
        {
            // OK: values have been consistently written
            EDDI_EXIT_CRITICAL_S();
            return;
        }
    }

    EDDI_EXIT_CRITICAL_S();

    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                      "EDDI_SERResetRedundancyAndTsbEntry1B, CONSISTENCY Check - CRITICAL LOCK ISSUE - ERROR on resetting TimerScoreboardentry ReadBackTimerSBEntry.Reg[0]:0x%X LocalTimerSBEntry.Reg[0]:0x%X ReadBackAPDU:0x%X",
                      ReadBackTimerSBEntry.Reg, LocalTimerSBEntry.Reg, ReadBackTRF);

    EDDI_Excp("EDDI_SERResetRedundancyAndTsbEntry1B - CONSISTENCY Check - CRITICAL LOCK ISSUE", EDDI_FATAL_ERR_EXCP, ReadBackTimerSBEntry.Reg, LocalTimerSBEntry.Reg);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SEREvalReloadWDValue( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                  EDDI_CRT_CONS_HWWD                const  eHwWatchdog,
                                                                  LSA_UINT32                      * const  pLocalTimerSBEntry0,
                                                                  LSA_UINT32                      * const  pLocalTimerSBEntry1)
{
    #if !defined (EDDI_CFG_REV7)
    LSA_UNUSED_ARG(pLocalTimerSBEntry1); //LINT
    #else
    *pLocalTimerSBEntry1 = 0;
    #endif
    
    *pLocalTimerSBEntry0 = 0;

    switch (eHwWatchdog)
    {
        case EDDI_CRT_CONS_HWWD_LOAD:
        {
            #if defined (EDDI_CFG_REV7)
            *pLocalTimerSBEntry1 = 0;
            EDDI_SetBitField32(pLocalTimerSBEntry1, EDDI_SER10_TIMER_SCORB_BIT2__WatchdogTimer, pConsumer->LowerParams.WDReloadVal);
            EDDI_SetBitField32(pLocalTimerSBEntry0, EDDI_SER10_TIMER_SCORB_BIT__SCRR_ReloadValue, pConsumer->LowerParams.SCRR_ReloadValue);
                      
            #elif defined (EDDI_CFG_REV6)
            EDDI_SetBitField32(pLocalTimerSBEntry0, EDDI_SER10_TIMER_SCORB_BIT__WatchdogTimer,  pConsumer->LowerParams.WDReloadVal);
            #else 
            //Rev5: Nothing to do here: because UkCountDown will be handled by SW-Scoreboard
            #endif

            break;
        }
        case EDDI_CRT_CONS_HWWD_CLEAR:
        {
            #if defined (EDDI_CFG_REV7)
            *pLocalTimerSBEntry1 = 0;
            #endif
          
            break;
        }
        case EDDI_CRT_CONS_HWWD_UNTOUCH:
        {
            #if defined (EDDI_CFG_REV7)
            *pLocalTimerSBEntry0 = *((LSA_UINT32 *)(void *)pConsumer->LowerParams.pSBTimerEntry);
            *pLocalTimerSBEntry1 = *(((LSA_UINT32 *)(void *)pConsumer->LowerParams.pSBTimerEntry)+1);
            EDDI_SetBitField32(pLocalTimerSBEntry1, EDDI_SER10_TIMER_SCORB_BIT2__ClearMask, 0);
            #elif defined (EDDI_CFG_REV6)
            *pLocalTimerSBEntry0 = *((LSA_UINT32 *)(void *)pConsumer->LowerParams.pSBTimerEntry);
            EDDI_SetBitField32(pLocalTimerSBEntry0, EDDI_SER10_TIMER_SCORB_BIT__ClearMask, 0);
            #else 
            //Rev5: Nothing to do here: because UkCountDown will be handled by SW-Scoreboard
            #endif
          
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_SERResetRedundancyAndTsbEntry1B, illegal WD reset option ConsID:0x%X, FrameID:0x%X, eHwWatchdog:0x%X",
                              pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, eHwWatchdog);
            EDDI_Excp("EDDI_SERResetRedundancyAndTsbEntry1B", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, eHwWatchdog);
        }
    }
    LSA_UNUSED_ARG(pDDB);
}

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
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDataImage( LSA_UINT16                        const  FCode,
                                                          EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                          EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
#else
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDataImage( LSA_UINT16                        const  FCode,
                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                  EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
#endif
{
    LSA_UINT32  PrimPara;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    // revision6 needs the Pointer to ACW or FCW as PrimPara to execute Command

    if (pConsumer->LowerParams.ListType == EDDI_LIST_TYPE_ACW)
    {
        // Take the one and only ACW
        PrimPara = DEV_kram_adr_to_asic_register(pConsumer->pLowerCtrlACW->pKramCW, pDDB);
    }
    else
    {
        EDDI_PRM_RECORD_FRAME_DATA_TYPE  *  const  pIrFrameDataElem0 = pConsumer->pFrmHandler->pIrFrameDataElem[0];
        LSA_UINT32                          const  HwPortIndex       = EDDI_PmUsrPortIDToHwPort03(pDDB, pIrFrameDataElem0->UsrRxPort_0_4);

        // Class3 - Communication
        // Take pFCW from the Bottom of the ControlStructure-List
        PrimPara = DEV_kram_adr_to_asic_register(pConsumer->pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW, pDDB);
        
        if (pConsumer->pFrmHandler->pFrmHandlerRedundant)
        {
            EDDI_PRM_RECORD_FRAME_DATA_TYPE  *  const  pIrFrameDataElem0Rundant = pConsumer->pFrmHandler->pFrmHandlerRedundant->pIrFrameDataElem[0];
            LSA_UINT32                          const  HwPortIndexRundant       = EDDI_PmUsrPortIDToHwPort03(pDDB, pIrFrameDataElem0Rundant->UsrRxPort_0_4);
            
            PrimPara = DEV_kram_adr_to_asic_register(pConsumer->pFrmHandler->pFrmHandlerRedundant->pIRTRcvEl[HwPortIndexRundant]->pCW, pDDB);  
        }
        //PrimPara = DEV_kram_adr_to_asic_register(pConsumer->pLowerCtrlFCW->pBottom->pCCWCtrl->pKramCW, pDDB);
    }
    #else
    // revision5 needs the Pointer to APDU-Status as PrimPara to execute Command
    EDDI_CRT_DATA_APDU_STATUS  *  const  pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;

    PrimPara = DEV_kram_adr_to_asic_register(&pAPDU->Detail.DataStatus, pDDB);
    #endif

    EDDI_SERSingleCommand(FCode, PrimPara, 0UL, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerIsInitialized()
 *
 *  Description:Checks if the Consumer is initialized
 *
 *  Arguments:  pConsumer   (THIS): the related Consumer
 *
 *  Return:     LSA_TRUE   if Consumer is Active,
 *              LSA_FALSE  else
 */
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerIsInitialized( EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    if ((pConsumer->pSB->Status != EDDI_CRT_CONS_STS_NOTUSED) && (pConsumer->pSB->Status != EDDI_CRT_CONS_STS_RESERVED))
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCreateConsumerList()
 *
 *  Description: Constructor for the ConsumerList
 *               Advice: ConsumerList can also be initialized with
 *                       maxConsumerCount == 0 and
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCreateConsumerList( EDDI_LOCAL_DDB_PTR_TYPE  const   pDDB, 
                                                           EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const  pConsList,
                                                           LSA_UINT32                       const  maxAcwConsumerCount,
                                                           LSA_UINT32                       const  maxFcwConsumerCount )
{
    LSA_UINT32                     ctr;
    LSA_UINT32              const  maxConsumerCount = maxAcwConsumerCount + maxFcwConsumerCount
    #if defined (EDDI_CFG_DFP_ON)
    + maxFcwConsumerCount;
    #else
    ;
    #endif
    EDDI_CRT_CONSUMER_TYPE           * pConsumer;
    EDDI_CRT_CONS_SB_TYPE            * pSBEntry;
    EDDI_CRT_CONSRTC12EVENTLIST_TYPE * pConsRTC12EventID;
    LSA_UINT32                         alloc_size;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCreateConsumerList->maxAcwConsumerCount:0x%X maxFcwConsumerCount:0x%X",
                      maxAcwConsumerCount, maxFcwConsumerCount);

    if (maxConsumerCount > (0xFFFF / sizeof(EDDI_CRT_CONSUMER_PTR_TYPE))) // only 16Bit values allowed
    {
        EDDI_Excp("EDDI_CRTCreateConsumerList, ERROR maxConsumerCount is too big", EDDI_FATAL_ERR_EXCP, maxConsumerCount, 0);
        return;
    }

    pConsList->MaxEntries         = maxConsumerCount;
    pConsList->UsedEntriesRTC123  = 0;
    pConsList->UsedEntriesDFP     = 0;
    pConsList->LastIndexRTC123    = pDDB->CRT.MetaInfo.RTC123IDStart;            
    pConsList->LastIndexDFP       = pDDB->CRT.MetaInfo.DFPIDStart;
    pConsList->ActiveConsRTC12    = 0;
    pConsList->ActiveConsRTC3     = 0;
    pConsList->UsedACWs       = 0;

    if (maxConsumerCount)
    {
        // Allocate Consumer-Array
        alloc_size = maxConsumerCount * sizeof(EDDI_CRT_CONSUMER_TYPE);
        EDDI_AllocLocalMem((void**)&pConsumer, alloc_size);

        if (EDDI_NULL_PTR == pConsumer)
        {
            EDDI_Excp("EDDI_CRTCreateConsumerList, ERROR while allocating Consumer-Array",EDDI_FATAL_ERR_EXCP,  0, 0);
            return;
        }

        // Allocate ConsSB-Array
        alloc_size = maxConsumerCount * sizeof(EDDI_CRT_CONS_SB_TYPE);
        EDDI_AllocLocalMem((void * *)&pSBEntry, alloc_size);

        if (EDDI_NULL_PTR == pSBEntry)
        {
            EDDI_Excp("EDDI_CRTCreateConsumerList, ERROR while allocating Consumer-Array", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        //alloc list for RTC12 consumers having a pending event
        alloc_size = (maxAcwConsumerCount + maxFcwConsumerCount)* sizeof(EDDI_CRT_CONSRTC12EVENTLIST_TYPE);
        EDDI_AllocLocalMem((void**)&pConsRTC12EventID, alloc_size);

        if (EDDI_NULL_PTR == pConsRTC12EventID)
        {
            EDDI_Excp("EDDI_CRTCreateConsumerList, ERROR while allocating Consumer-Array",EDDI_FATAL_ERR_EXCP,  0, 0);
            return;
        }

        pConsList->pEntry            = pConsumer;
        pConsList->pSBEntry          = pSBEntry;
        pConsList->pConsRTC12EventID = pConsRTC12EventID;

        // reset all ACW-Consumer-Entries
        for (ctr = pDDB->CRT.MetaInfo.RTC123IDStart; ctr < pDDB->CRT.MetaInfo.RTC123IDEnd; ctr++)
        {
            pConsumer->ConsumerId           = (LSA_UINT16)ctr;
            pConsumer->pSB                  = pSBEntry;
            pConsumer->LowerParams.ListType = EDDI_LIST_TYPE_UNDEFINED;
            pSBEntry->Status                = EDDI_CRT_CONS_STS_NOTUSED;
            pSBEntry->ConsumerId            = (LSA_UINT16)ctr;
            EDDI_CRT_CONSTRACE_INIT(pConsumer);
            pConsumer++;
            pSBEntry++;
        }

        // reset all DFP-Consumer-Entries
        for (ctr = pDDB->CRT.MetaInfo.DFPIDStart; ctr < pDDB->CRT.MetaInfo.DFPIDEnd; ctr++)
        {
            pConsumer->ConsumerId           = (LSA_UINT16)ctr;
            pConsumer->pSB                  = pSBEntry;
            pConsumer->LowerParams.ListType = EDDI_LIST_TYPE_FCW;
            pSBEntry->Status                = EDDI_CRT_CONS_STS_NOTUSED;
            pSBEntry->ConsumerId            = (LSA_UINT16)ctr;
            EDDI_CRT_CONSTRACE_INIT(pConsumer);
            pConsumer++;
            pSBEntry++;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTDeleteConsumerList()  //DESTRUCTOR
 *
 *  Description: Destructor for the ConsumerList
 *
 *  Arguments:   pConsList: reference to listObject to be deleted
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDeleteConsumerList( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const  pConsList )
{
    LSA_UINT16  u16Status;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTDeleteConsumerList->");

    if (!(EDDI_NULL_PTR == pConsList->pEntry))
    {
        // free Consumer-Array
        EDDI_FREE_LOCAL_MEM(&u16Status, pConsList->pEntry);
        if (u16Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTDeleteConsumerList, ERROR while deleting pConsList->pEntry -> pConsList->MaxEntries:0x%X",
                              pConsList->MaxEntries);
            EDDI_Excp("EDDI_CRTDeleteConsumerList", EDDI_FATAL_ERR_EXCP, u16Status, 0);
            return;
        }
    }

    if (!(EDDI_NULL_PTR == pConsList->pSBEntry))
    {
        EDDI_FREE_LOCAL_MEM(&u16Status, pConsList->pSBEntry);
        if (u16Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTDeleteConsumerList, ERROR while deleting pConsList->pSBEntry -> pConsList->MaxEntries:0x%X",
                              pConsList->MaxEntries);
            EDDI_Excp("EDDI_CRTDeleteConsumerList", EDDI_FATAL_ERR_EXCP, u16Status, 0);
            return;
        }
    }

    if (!(EDDI_NULL_PTR == pConsList->pConsRTC12EventID))
    {
        EDDI_FREE_LOCAL_MEM(&u16Status, pConsList->pConsRTC12EventID);
        if (u16Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTDeleteConsumerList, ERROR while deleting pConsList->pConsRTC12EventID -> pConsList->MaxEntries:0x%X",
                              pConsList->MaxEntries);
            EDDI_Excp("EDDI_CRTDeleteConsumerList", EDDI_FATAL_ERR_EXCP, u16Status, 0);
            return;
        }
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerListAddEntry()
 *
 *  Description: Adds a new Consumer to the List and calls the CRTConsumerAdd-Function
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_LIMIT_REACHED else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListAddEntry( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT                                   Status;
    LSA_RESULT                                   Status2;
    LSA_UINT8                                    ListType;
    LSA_UINT8                                    ConsumerType;
    EDDI_CRT_CONSUMER_PTR_TYPE                   pConsumer;
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListAddEntry->");

    ListType     = EDDI_CRTGetListType(pConsParam->Properties, pConsParam->FrameID);
    ConsumerType = EDDI_CRTGetConsumerType(pDDB, pRQB, ListType);

    if (ConsumerType == EDDI_RT_CONSUMER_TYPE_INVALID)
    {
        return EDD_STS_ERR_PARAM;
    }
    else if (ConsumerType == EDDI_UDP_CONSUMER)
    {
        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        return EDDI_RtoConsumerListAddEntry (pRQB, pDDB);
        #elif !defined (EDDI_CFG_REV7)
        return EDD_STS_ERR_PARAM;
        #endif
    }

    Status = EDDI_CRTConsumerAddCheckRQB(pRQB, pDDB, ListType, ConsumerType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    // Get free ConsumerID
    Status = EDDI_CRTConsumerListReserveFreeEntry(pDDB, &pConsumer, pDDB->pLocal_CRT, ListType, EDDI_IS_DFP_CONS(pConsParam)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerListAddEntry, ERROR finding free Entry -> Status:0x%X", Status);
        return Status;
    }

    Status = EDDI_CRTConsumerAddEvent(pConsumer, pRQB, pDDB, ListType, ConsumerType);

    //clean up in case of failure
    if (Status != EDD_STS_OK)
    {
        if  /* aux consumer exists (can happen only if RTC3 consumer) */
            (pConsumer->IRTtopCtrl.pAscConsumer)
        {
            //deallocate aux apdu-buffer
            if (pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.pKRAMDataBuffer)
            {
                EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.ApduAuxConsumer, pConsumer->LowerParams.pKRAMDataBuffer);
            }

            EDDI_CRTConsumerListUnReserveEntry(pConsumer->IRTtopCtrl.pAscConsumer, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);
        }
        
        //checks if the conditions for removing a packframe are met
        Status2 = EDDI_DFPConsRemovePFHandler(pDDB, pConsumer);
        if (Status2 != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                              "EDDI_CRTConsumerListAddEntry, ERROR EDDI_DFPConsRemovePFHandler, Status:0x%X ConsumerID:0x%X", 
                              Status2, pConsumer->ConsumerId);
            EDDI_Excp("EDDI_CRTConsumerListAddEntry, ERROR EDDI_DFPConsRemovePFHandler",
                      EDDI_FATAL_ERR_EXCP, Status2, pConsumer->ConsumerId);
            return EDD_STS_ERR_EXCP;
        }

        #ifdef EDDI_PRAGMA_MESSAGE
        #pragma message ("MultiEDDI")
        #endif /* EDDI_PRAGMA_MESSAGE */
        #if defined (EDDI_CFG_REV7)
        //deallocate statebuffer (if allocated)
        if (   (pConsumer->LowerParams.pKRAMDataBuffer)
            && (!EDDI_IS_DFP_CONS(pConsParam)))
        {
            if (pConsumer->usingPAEA_Ram)
            {
                Status2 = EDDI_MemFreeApduBufferCons(pConsumer->ConsumerId, pDDB);

                if (Status2 != EDD_STS_OK)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                                          "EDDI_CRTConsumerListAddEntry, ERROR EDDI_MemFreeApduBufferCons, Status:0x%X ConsumerID:0x%X", 
                                      Status2, pConsumer->ConsumerId);
                        EDDI_Excp("EDDI_CRTConsumerListAddEntry, ERROR EDDI_MemFreeApduBufferCons",
                              EDDI_FATAL_ERR_EXCP, Status2, pConsumer->ConsumerId);
                    return EDD_STS_ERR_EXCP;
                }
            }
        }
        #endif
        
        EDDI_CRTConsumerListUnReserveEntry(pConsumer, pDDB->pLocal_CRT, EDDI_IS_DFP_CONS(pConsParam)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);
        return Status;
    }
   
    // Set new Status
    pConsumer->pSB->Status = EDDI_CRT_CONS_STS_INACTIVE;

    //EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(pConsumer->LowerParams.pKRAMDataBuffer)), pConsumer->LowerParams.FrameId, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_ADD);
    EDDI_CRT_CONSTRACE_SET_TABLE_ENTRY(pDDB, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerListRemoveEntry()
 *
 *  Description: Remove the Consumer from the List
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_RESOURCE else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListRemoveEntry( EDD_UPPER_RQB_PTR_TYPE     const  pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                                  LSA_BOOL                *  const  pIndicate )
{
    EDDI_CRT_CONSUMER_LIST_TYPE              *  const  pConsList = &pDDB->pLocal_CRT->ConsumerList;
    LSA_RESULT                                         Status;
    EDDI_CRT_CONSUMER_PTR_TYPE                         pConsumer;
    EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE     const  pRemParam = (EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE)pRQB->pParam;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListRemoveEntry->");

    *pIndicate = LSA_TRUE;
       
    // get ConsumerID
    Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, pConsList, pRemParam->ConsumerID);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListRemoveEntry, ERROR getting Entry, pRemParam->ConsumerID:0x%X",
                          pRemParam->ConsumerID);
        return Status;
    }

    #if !defined (EDDI_CFG_RTC3REMOVE_WITHOUT_PASSIVATING)
    //RTC3 consumers cannot be removed without being passivated
    if (EDDI_CONS_STATE_ACTIVE == pConsumer->IRTtopCtrl.ConsState)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                  "EDDI_CRTConsumerListRemoveEntry, RTC3/AUX Consumer is active. ERROR getting Entry, pRemParam->ConsumerID:0x%X",
                  pRemParam->ConsumerID);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerIsActive);
        return EDD_STS_ERR_SEQUENCE;     
    }
    #endif

    EDDI_CRT_CONSTRACE_SET_TABLE_ENTRY(pDDB, pConsumer->ConsumerId, 0xFFFF);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pConsumer->Locked)
    {
        if (pConsumer->pRQB_PendingRemove != EDDI_NULL_PTR)
        {
            return EDD_STS_ERR_SEQUENCE;
        }

        // Service will be confirmed when consumer will be unlocked 
        pConsumer->pRQB_PendingRemove = pRQB;
        *pIndicate                    = LSA_FALSE;
        return EDD_STS_OK;
    }
    #endif

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        Status = EDDI_RtoConsumerRemoveEvent(pConsumer, pDDB);
        return Status;
    }
    #endif

    Status = EDDI_CRTConsumerRemoveEvent(pConsumer, pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListRemoveEntry, ERROR removing CRTConsumer, Status:0x%X", Status);
        return Status;
    }

    if (pConsumer->IRTtopCtrl.pAscConsumer)  
    {
        EDDI_CRTConsumerListUnReserveEntry(pConsumer->IRTtopCtrl.pAscConsumer, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);       
    }
    EDDI_CRTConsumerListUnReserveEntry(pConsumer, pDDB->pLocal_CRT, EDDI_IS_DFP_CONS(pConsumer)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                   "private" Helper-Functions of CRTConsumer-"Class"       */
/*===========================================================================*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerInit()  //HelperFunction only
 *
 *  Description:Uses the RQB to initializes the pConsumer.
 *              Checks if Params are all in a good Range
 *              Sets the pKRAMDataBuffer relativ to the OUT_DATA_BASE-Image.
 *              The Paramblock of Requestblock will also be changed! ->pBuffer!
 *
 *  Arguments:  pConsumer (THIS): the Pointer of Consumer
 *              (the ConsumerID must already be initialized!)
 *              pRQB      (IN/OUT): the Pointer to the RQB containing all params
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_RESOURCE if APDU-Pool delivered Error,
 *              EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerInit( EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                               EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam, 
                                                               EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                               LSA_UINT8                             const  ListType,
                                                               LSA_UINT8                             const  ConsumerType )
{
    // pConsParam Must not be checked
    LSA_UINT32                               ctr;
    LSA_RESULT                               Status;
    #if defined (EDDI_CFG_REV7)
    EDDI_CRT_PAEA_TYPE                const  PAEAType   = pDDB->CRT.PAEA_Para;
    EDDI_CRT_PAEA_APDU_STATUS             *  pPAEA_APDU = EDDI_NULL_PTR;
    #endif

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerInit->");

    pConsumer->pNextAutoStopProv = EDDI_NULL_PTR;
    pConsumer->bEnableAutoStop   = LSA_FALSE;
    pConsumer->bFocusAutoStop    = LSA_FALSE;

    pConsumer->bHasSeenMiss      = LSA_FALSE;    //reset consumer MISS flag, see EDDI_CRTConsumerControlSetToUnknown
    pConsumer->bHasSeenAgain     = LSA_FALSE;    //reset consumer frame received once flag

    pConsumer->ScoreBoardStatus  = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;

    pConsumer->IRTtopCtrl.ConsState      = EDDI_CONS_STATE_PASSIVE;
    pConsumer->IRTtopCtrl.ConsActiveMask = (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)?0:EDDI_CONS_ACTIVE_CLASS12_MAIN;
    
    if (EDD_CSRT_CONS_PROP_RTCLASS_UDP == (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
    {
        pConsumer->LowerParams.RT_Type = EDDI_RT_TYPE_UDP;

        pConsumer->LowerParams.SrcIP.b.IP[0] = 0xFF;
        pConsumer->LowerParams.SrcIP.b.IP[1] = 0xFF;
        pConsumer->LowerParams.SrcIP.b.IP[2] = 0xFF;
        pConsumer->LowerParams.SrcIP.b.IP[3] = 0xFF;
    }
    else
    {
        pConsumer->LowerParams.RT_Type = EDDI_RT_TYPE_NORMAL;
    }

    //now init consumer with the given params

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    pConsumer->Locked                            = LSA_FALSE;
    pConsumer->pRQB_PendingRemove                = EDDI_NULL_PTR;
    pConsumer->LowerParams.DataHoldStatus        = 0x0;    //Default BACKUP-MODE
    #endif

    pConsumer->UpperUserId                       = pConsParam->UserID;

    pConsumer->LowerParams.ListType              = ListType;
    pConsumer->LowerParams.ConsumerType          = ConsumerType;
    pConsumer->LowerParams.FrameId               = pConsParam->FrameID;

    pConsumer->pFrmHandler                       = EDDI_NULL_PTR;  
    pConsumer->pLowerCtrlACW                     = EDDI_NULL_PTR;

    #if defined (EDDI_CFG_REV7)
    pConsumer->usingPAEA_Ram                     = LSA_FALSE;
    #endif

    pConsumer->Properties                        = pConsParam->Properties;
    pConsumer->LowerParams.DataLen               = pConsParam->DataLen;
    pConsumer->LowerParams.BufferProperties      = pConsParam->IOParams.BufferProperties;
    pConsumer->IRTtopCtrl.ConsState              = EDDI_CONS_STATE_PASSIVE;
    pConsumer->LowerParams.CyclePhase            = pConsParam->CyclePhase;
    pConsumer->LowerParams.CycleReductionRatio   = pConsParam->CycleReductionRatio;

    if (!EDDI_IS_DFP_CONS(pConsumer))
    {    
        #if defined (EDDI_CFG_REV7)
        if (PAEAType.PAEA_BaseAdr)
        {
            if (EDDI_RTC3_AUX_CONSUMER == ConsumerType)
            {
                pConsumer->LowerParams.DataOffset         = EDD_DATAOFFSET_UNDEFINED;  //see eddi_BufferRequest_GetConsumerID()!
                pConsumer->LowerParams.Partial_DataLen    = 0;
                pConsumer->LowerParams.Partial_DataOffset = 0;
                //allocate a small APDU-Buffer (4 Bytes) for Aux-Consumer
                EDDI_MemGetPoolBuffer(pDDB->CRT.hPool.ApduAuxConsumer, (void * *)&pConsumer->LowerParams.pKRAMDataBuffer);
                if (pConsumer->LowerParams.pKRAMDataBuffer == EDDI_NULL_PTR)
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerInit, out of internal Apdu-Buffer for Aux-Consumer, FcwConsumerCnt:0x%X", 
                                      pDDB->pLocal_CRT->MetaInfo.FcwConsumerCnt);
                    return EDD_STS_ERR_RESOURCE;
                }
                else
                {
                    pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
                }
            }
            else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
            {
                pConsumer->LowerParams.DataOffset         = pConsParam->IOParams.DataOffset;
                pConsumer->LowerParams.Partial_DataLen    = pConsParam->Partial_DataLen;
                pConsumer->LowerParams.Partial_DataOffset = pConsParam->Partial_DataOffset;

                if (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC != (pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) )
                {
                    LSA_UINT8   const DataStatus = (pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;

                    //Set to TRUE and alloc apdu-memory, even if properties are UNDEFINED.
                    //If they get set to SYNC during ACTIVATE, the buffer will be deallocated again
                    pConsumer->usingPAEA_Ram = LSA_TRUE;

                    //Place PAEA-APDU element to memory-pool according to its ID.
                    Status = EDDI_MemGetApduBuffer(pConsumer->ConsumerId, pDDB, &pPAEA_APDU, EDDI_MEM_APDU_CONSUMER_BUFFER);
                    if (EDD_STS_OK != Status)
                    {
                        return Status;
                    }

                    pConsumer->LowerParams.pKRAMDataBuffer      = (EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pPAEA_APDU;
                    pConsumer->usingPAEA_Ram                    = LSA_TRUE;
                    pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;

                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTConsumerInit, PAEA-APDU Pool, ConsID:0x%X /buffer:0x%X",
                                      pConsumer->ConsumerId, (LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer);

                    EDDI_CRTCompResetAPDUStatus(pDDB, pConsumer->LowerParams.pKRAMDataBuffer, DataStatus , (LSA_UINT8)0, LSA_FALSE /*bSysRed*/);

                    // Set user data application pointer to PAEA-RAM (offset relative to PAEA-Baseaddress)
                    // If the user passes EDD_DATAOFFSET_UNDEFINED, set the offset to INVALID,
                    // because UNDEFINED marks a slot as free!
                    pPAEA_APDU->pPAEA_Data = (EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset)?EDD_DATAOFFSET_INVALID:EDDI_HOST2IRTE32(pConsumer->LowerParams.DataOffset);
                }
                else
                {
                    if (EDD_DATAOFFSET_UNDEFINED == pConsParam->IOParams.DataOffset)
                    {
                        pConsumer->LowerParams.pKRAMDataBuffer  = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
                    }
                    else
                    {
                        pConsumer->LowerParams.pKRAMDataBuffer      = (LSA_UINT8*)((LSA_UINT32)pDDB->pKRam + pConsParam->IOParams.DataOffset);
                        pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
                    }
                }
            }
        }
        else
        #endif //defined (EDDI_CFG_REV7)
        {
            //Rev5/Rev6: AUX-Consumer gets no data buffer
            if (EDDI_RTC3_AUX_CONSUMER == ConsumerType)
            {
                pConsumer->LowerParams.DataOffset         = EDD_DATAOFFSET_UNDEFINED;  //see eddi_BufferRequest_GetConsumerID()!
                pConsumer->LowerParams.Partial_DataLen    = 0;
                pConsumer->LowerParams.Partial_DataOffset = 0;

                //allocate a small APDU-Buffer (4 Bytes) for Aux-Consumer
                EDDI_MemGetPoolBuffer(pDDB->CRT.hPool.ApduAuxConsumer, (void * *)&pConsumer->LowerParams.pKRAMDataBuffer);
                if (pConsumer->LowerParams.pKRAMDataBuffer == EDDI_NULL_PTR)
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerInit, out of internal Apdu-Buffer for Aux-Consumer, FcwConsumerCnt:0x%X", 
                                      pDDB->pLocal_CRT->MetaInfo.FcwConsumerCnt);
                    return EDD_STS_ERR_RESOURCE;
                }
                else
                {
                    pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
                }
            }
            else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
            {
                pConsumer->LowerParams.DataOffset         = pConsParam->IOParams.DataOffset;
                pConsumer->LowerParams.Partial_DataLen    = pConsParam->Partial_DataLen;
                pConsumer->LowerParams.Partial_DataOffset = pConsParam->Partial_DataOffset;
            
                if (EDD_DATAOFFSET_UNDEFINED == pConsParam->IOParams.DataOffset)
                {
                    pConsumer->LowerParams.pKRAMDataBuffer  = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
                }
                else
                {
                    pConsumer->LowerParams.pKRAMDataBuffer      = (LSA_UINT8*)((LSA_UINT32)pDDB->pKRam + pConsParam->IOParams.DataOffset);
                    pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
                }
            }
        }
    }
    else
    {
        //init DG-consumer
        Status = EDDI_DFPConsumerInit(pDDB, pConsumer, pConsParam);
        if (EDD_STS_OK != Status)
        {
            return Status;
        }
    }

    for (ctr = 0; ctr < EDD_MAC_ADDR_SIZE; ctr++)
    {
        pConsumer->LowerParams.SrcMAC.MacAdr[ctr] = 0xFF;
    }

    pConsumer->LowerParams.CyclePhase           = pConsParam->CyclePhase;
    pConsumer->LowerParams.CycleReductionRatio  = pConsParam->CycleReductionRatio;

    if (EDDI_RTC3_AUX_CONSUMER == ConsumerType)
    {
        pConsumer->LowerParams.DataHoldFactor       = EDDI_MAX_RELOAD_TIME_VALUE; 
    }
    else
    {
        pConsumer->LowerParams.DataHoldFactor       = pConsParam->DataHoldFactor;
    }  

    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;

    Status = EDD_STS_OK;

    switch (ListType)
    {
        case EDDI_LIST_TYPE_FCW:
        {
            //ImageMode gets set in SetBuffer
            break;
        }

        case EDDI_LIST_TYPE_ACW:
        {
            #if defined (EDDI_CFG_REV7)
            if (   (0 != PAEAType.PAEA_BaseAdr)
                || (EDDI_RTC3_AUX_CONSUMER == ConsumerType) )
            {
                pConsumer->LowerParams.ImageMode = EDDI_IMAGE_MODE_ASYNC;
            }
            else
            {
                pConsumer->LowerParams.ImageMode = EDDI_IMAGE_MODE_SYNC;
            }
            #else
            pConsumer->LowerParams.ImageMode = EDDI_IMAGE_MODE_ASYNC;
            #endif

            break;
        }

        default:
        {
            EDDI_Excp("EDDI_CRTConsumerInit", EDDI_FATAL_ERR_EXCP, ListType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return Status;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerInitSingleBuffer( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                         LSA_UINT32                  const  DataOffset,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                         LSA_UINT8                   const  ConsumerType )
{
    LSA_UINT8   const DataStatus = ((pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerInitSingleBuffer->");

    // Helper-Ptr to Quick-Access to DataStatus for Scoreboard-Workaround
    pConsumer->pSB->pCopyKRAMConsDataStatus = pConsumer->LowerParams.pKRAMDataBuffer + 2; // BytePointer pointing directly to DataStatus

    if (ConsumerType == EDDI_RTC3_AUX_CONSUMER)
    {
        //no DMA
        pConsumer->pUserDataBuffer = LSA_NULL;
    }
    else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
    {
        #if !defined (EDDI_CFG_REV7)
        if (pConsumer->LowerParams.ListType == EDDI_LIST_TYPE_FCW)
        {
            pConsumer->pUserDataBuffer = pDDB->ProcessImage.pUserDMA + DataOffset;
        }
        else
        {
            //no DMA
            pConsumer->pUserDataBuffer = pDDB->pKRam + DataOffset;
        }

        EDDI_CRTCompResetAPDUStatus(pDDB, pConsumer->pUserDataBuffer, (LSA_UINT8)((pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS), 
                                    (LSA_UINT8)EDDI_SER_RED_STATUS_INIT_VALUE, LSA_FALSE /*bSysRed*/);
        #else
        if (!pConsumer->usingPAEA_Ram)
        {
            pConsumer->pUserDataBuffer = pDDB->pKRam + DataOffset;
        }
        else
        {
            //no DMA
            pConsumer->pUserDataBuffer = LSA_NULL;
        }
        #endif
    }

    EDDI_CRTCompResetAPDUStatus(pDDB, pConsumer->LowerParams.pKRAMDataBuffer, DataStatus, (LSA_UINT8)EDDI_SER_RED_STATUS_INIT_VALUE, LSA_FALSE /*bSysRed*/);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerListControl()
 *
 *  Description: Uses the RQB to set Data-States of one Consumer and its Activity
 *
 *  Arguments:   ...
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListControl(  EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT                                          Status;
    EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE     const  pParam    = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE             const  pCRT      = pDDB->pLocal_CRT;
    EDDI_CRT_CONSUMER_LIST_TYPE               *  const  pConsList = &pCRT->ConsumerList;
    EDDI_CRT_CONSUMER_PTR_TYPE                          pConsumer;
    LSA_UINT16                                   const  Mode      = pParam->Mode;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListControl->");

    //Check EDDDataoffset
    if (EDD_DATAOFFSET_INVALID == pParam->IOParams.DataOffset)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerListControl, EDD_DATAOFFSET_INVALID.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }
    
    //Check BufferPro
    if (EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID == (pParam->IOParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK))
    {                           
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerListControl, EDD_BUFFER_PROPERTIES_INVALID.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRT_XRTProperties);
        return EDD_STS_ERR_PARAM;
    } 

    //Check forwarding mode
    if (   (EDD_FORWARDMODE_SF        != pParam->IOParams.ForwardMode)
        && (EDD_FORWARDMODE_UNDEFINED != pParam->IOParams.ForwardMode))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerListControl: illegal forwarding mode.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ForwardingMode);
        return EDD_STS_ERR_PARAM;
    }

    // get the Consumer
    Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, pConsList, pParam->ConsumerID);
    if (EDD_STS_OK != Status)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListControl, ERROR: Could not get pConsumer, pParam->ConsumerID:0x%X pConsList->MaxEntries:0x%X",
                          pParam->ConsumerID, pConsList->MaxEntries);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
        return EDD_STS_ERR_PARAM;
    }
 
    //Check if the user wants to control the DFP-PackFrame
    if (EDDI_DFPConsumerIsPF(pConsumer))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListControl, Not allowed to control (Mode:0x%X) a DFP-Packframe (ConsumerID:0x%X) by user",Mode, pConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_APDU_INDIRECTION);
        return EDD_STS_ERR_PARAM;
    }
 
    if (   (   (Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) && (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE ))   //1
        || (   (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE) && (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE )) //4
        || (   (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE) && (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN) )
        ) 
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListControl, invalid Mode:0x%X", pParam->Mode);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
        return EDD_STS_ERR_PARAM;
    } 

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        return EDDI_RtoConsumerListCtrl(pDDB, pConsumer, pParam->Mode);
    }
    #endif

    Status = EDDI_CRTConsumerControlEvent(pConsumer, pRQB, pDDB, pConsumer->LowerParams.ConsumerType);
    if (EDD_STS_OK != Status)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListControl, ERROR: EDDI_CRTConsumerControlEvent failed. -> pConsumer->ConsumerId:0x%X Status:0x%X",
                          pConsumer->ConsumerId, Status);
        //EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Unknown_Consumer_State);
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerListGetEntry()
 *
 *  Description:returns the Consumer from the specified ConsumerId.
 *              Remark: Only initialized Consumers will be returned.
 *
 *  Arguments:  *pConsumer(OUT) : return Value
 *              pConsList (THIS): the Pointer of Consumer
 *              ConsId          : the Id of the Consumer
 *
 *  Return:     EDD_STS_OK        if succeeded,
 *              EDD_STS_ERR_PARAM if entry is not in use.
 *
 */
 LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListGetEntry( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                              EDDI_CRT_CONSUMER_PTR_TYPE     * const ppConsumer,
                                                              EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const pConsList,
                                                              LSA_UINT16                       const ConsId )
{
    EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer = &pConsList->pEntry[ConsId];

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListGetEntry->");

    if (ConsId >= pConsList->MaxEntries)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                          "EDDI_CRTConsumerListGetEntry, NOTE_LOW: Parameter ConsId not valid. -> ConsId:0x%X", ConsId);
        return EDD_STS_ERR_PARAM;
    }

    if (pConsumer->LowerParams.ConsumerType == EDDI_RTC3_AUX_CONSUMER)
    {
       EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTConsumerListGetEntry, NOTE_LOW: EDDI_RTC3_AUX_CONSUMER not allowed. -> ConsId:0x%X", ConsId);
       return EDD_STS_ERR_PARAM;
    }

    if (   (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_NOTUSED) 
        || (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_RESERVED))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                          "EDDI_CRTConsumerListGetEntry, NOTE_LOW: ConsId not in Use. -> ConsId:0x%X", ConsId);
        return EDD_STS_ERR_PARAM;
    }

    *ppConsumer = pConsumer;

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerListFindByFrameId()  //HelperFunction only
 *
 *  Description:Returns the first Consumer with the specified MessageId
 *
 *  Arguments:  pConsList      (THIS) : the Pointer of ConsumerList
 *              FrameId               : the specified FrameId
 *              * ppRetConsumer (OUT) : if found: reference to the found Consumer
 *                                      else:     EDDI_NULL_PTR
 *
 *  Return:
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListFindByFrameId( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                  EDDI_CRT_CONSUMER_LIST_PTR_TYPE          const  pConsList,
                                                                  EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         const  pCRT,
                                                                  EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE     const  pConsParam,
                                                                  LSA_UINT8                                const  ListType,
                                                                  EDDI_CRT_CONSUMER_PTR_TYPE            *  const  ppRetConsumer, /* OUT */
                                                                  LSA_BOOL                                 const  bIsDfp )

{
    LSA_UINT32         ctr, IdBegin, IdEnd;
    LSA_UINT16  const  FrameId = pConsParam->FrameID;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListFindByFrameId->");

    if (   (ListType == EDDI_LIST_TYPE_FCW)
        && (bIsDfp))
    {
        IdBegin = pCRT->MetaInfo.DFPIDStart;
        IdEnd   = pConsList->LastIndexDFP + 1;
    }
    else
    {
        IdBegin = pCRT->MetaInfo.RTC123IDStart;
        IdEnd   = pConsList->LastIndexRTC123 + 1;
    }

    //search for an initialized consumer with the same FrameID with the same ListType
    for (ctr = IdBegin; ctr < IdEnd; ctr++)
    {
        EDDI_CRT_CONSUMER_TYPE  *  const  pConsumer = &pConsList->pEntry[ctr];

        if ((FrameId == pConsumer->LowerParams.FrameId) && EDDI_CRTConsumerIsInitialized(pConsumer))
        {
            if (!bIsDfp)
            {
                *ppRetConsumer = pConsumer;
                return;
            }
            else
            {
                //check if PosT matches
                if (   (pConsumer->LowerParams.SFPosition != EDD_SFPOSITION_UNDEFINED)
                    && (pConsumer->LowerParams.SFPosition == pConsParam->IOParams.SFPosition))
                {
                    *ppRetConsumer = pConsumer;
                    return;
                }
            }
        }
    }

    *ppRetConsumer = EDDI_NULL_PTR;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerCheckData()
 *
 *  Description: Uses the RQB to remove the pConsumer and starts the de-queuing
 *               of the HW-substructures.
 *
 *  Arguments:   ...
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerCheckData( EDDI_CRT_CONSUMER_PTR_TYPE           const  pConsumer,
                                                                    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB )
{
    LSA_UINT32                           iElem;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  pFrmData;

    if (   (EDDI_SYNC_IRT_CONSUMER     != pFrmHandler->HandlerType)
        && (EDDI_SYNC_IRT_FWD_CONSUMER != pFrmHandler->HandlerType))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerCheckData, EDDI_SYNC_IRT_CONSUMER <> pFrmHandler->HandlerType, pFrmHandler->HandlerType:0x%X",
                          pFrmHandler->HandlerType);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_HandlerType);                           
        return EDD_STS_ERR_PARAM;
    }

    if (pFrmHandler->UsedByUpper)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerCheckData, LSA_FALSE <> pFrmHandler->UsedByUpper, pFrmHandler->UsedByUpper:0x%X",
                          pFrmHandler->UsedByUpper);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_UsedByUpper);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!(EDDI_NULL_PTR == pConsumer->pFrmHandler))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerCheckData, EDDI_NULL_PTR, pConsumer->pFrmHandler");
        EDDI_Excp("EDDI_CRTConsumerCheckData, pConsumer->pFrmHandler", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    // 1st element must be present 
    pFrmData = pFrmHandler->pIrFrameDataElem[0];
    if (pFrmData == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_CRTConsumerCheckData:EDDI_NULL_PTR, pIrFrameDataElem[0]", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    // DataLenWithPadding
    //if (pConsumer->LowerParams.DataLen < 0x28

    //Checken gegenuber den IRT-Record
    for (iElem = 0; iElem < EDDI_SYNC_MAX_REDUNDANT_ELEMENTS; iElem++)
    {
        if (!(EDDI_NULL_PTR == pFrmHandler->pIrFrameDataElem[iElem]))
        {
            if (pFrmHandler->pIrFrameDataElem[iElem]->DataLength != pConsumer->LowerParams.DataLen)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerCheckData, DataLength -> pConsumer->LowerParams.DataLen:0x%X pFrmHandler->pIrFrameDataElem[iElem]->DataLength:0x%X",
                                  pConsumer->LowerParams.DataLen, pFrmHandler->pIrFrameDataElem[iElem]->DataLength);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDataLen);
                return EDD_STS_ERR_PARAM;
            }
        
            if (pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio != pConsumer->LowerParams.CycleReductionRatio)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerCheckData, ReductionRatio -> pConsumer->LowerParams.CycleReductionRatio:0x%X pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio:0x%X",
                                  pConsumer->LowerParams.CycleReductionRatio, pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                return EDD_STS_ERR_PARAM;
            }

            if (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
            {
                if (pFrmHandler->pIrFrameDataElem[iElem]->Phase != pConsumer->LowerParams.CyclePhase)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerCheckData, Phase -> pConsumer->LowerParams.CyclePhase:0x%X pFrmHandler->pIrFrameDataElem[iElem]->Phase:0x%X",
                                      pConsumer->LowerParams.CyclePhase, pFrmHandler->pIrFrameDataElem[iElem]->Phase);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Phase);
                    return EDD_STS_ERR_PARAM;
                }
            }
            else
            {
                //non-legacy consumers take their phase directly from the PDIRData
                pConsumer->LowerParams.CyclePhase = pFrmHandler->pIrFrameDataElem[iElem]->Phase;
            }

            if (pFrmHandler->pIrFrameDataElem[iElem]->FrameID != pConsumer->LowerParams.FrameId)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerCheckData, FrameId -> pConsumer->LowerParams.FrameId:0x%X pFrmHandler->pIrFrameDataElem[iElem]->FrameID:0x%X",
                                  pConsumer->LowerParams.FrameId, pFrmHandler->pIrFrameDataElem[iElem]->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerPassivate()
 *
 *  Description: 
 *
 *  Arguments:  
 *
 *  Return:     
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerPassivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE         pConsumer,
                                                          LSA_BOOL                    const  bClearEvents,
                                                          LSA_BOOL                    const  bRemoveACWs)
{
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        if (EDDI_CONS_SCOREBOARD_STATE_PASSIVE == pConsumer->ScoreBoardStatus)
        {
            //do nothing else if the Consumer is passive
            return; 
        }
        else
        {
            LSA_RESULT Status = EDDI_DFPConsumerPassivate(pDDB, pConsumer, bClearEvents);
            if (EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerPassivate EDDI_DFPConsumerPassivate failed. ConsID:0x%X", 
                                  pConsumer->ConsumerId);
                EDDI_Excp("EDDI_CRTConsumerPassivate EDDI_DFPConsumerPassivate failed", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, 0);
                return;
            }
            Status = EDDI_DFPConsPassPFFrame(pDDB, pConsumer);
            if (EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerPassivate EDDI_DFPConsPassPFFrame failed. ConsID:0x%X", 
                                  pConsumer->ConsumerId);
                EDDI_Excp("EDDI_CRTConsumerPassivate EDDI_DFPConsPassPFFrame failed", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, 0);
                return;
            }
        }
    }
    else
    {
        if (EDDI_CONS_SCOREBOARD_STATE_PASSIVE == pConsumer->ScoreBoardStatus)
        {
            //Remove ACW 
            if (   bRemoveACWs
                && (EDDI_LIST_TYPE_ACW == pConsumer->LowerParams.ListType) )
            {
                if (EDDI_NULL_PTR != pConsumer->pLowerCtrlACW)
                {
                    //synchronous removal of ACW
                    EDDI_SRTConsumerRemove(pDDB, pConsumer->pLowerCtrlACW, EDDI_NULL_PTR);
                    pConsumer->pLowerCtrlACW = EDDI_NULL_PTR;
                }
            }

            //do nothing else if the Consumer is passive
            return; 
        }
        else
        {
            EDDI_CRT_DATA_APDU_STATUS * pAPDU;
            LSA_UINT8                   DataStatus = EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
            
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerPassivate->ConsID:0x%X ConsType:0x%X SBStatus:%d", 
                              pConsumer->ConsumerId, pConsumer->LowerParams.ConsumerType, pConsumer->pSB->Status);

            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer;

            // 1) Disable Receiving Data
            EDDI_CRTDataImage(FCODE_DISABLE_DATA_IMAGE, pConsumer, pDDB);

            if (bClearEvents)
            {
                //clear all pending events
                EDDI_CRTConsumerSetPendingEvent(pConsumer, 0xFFFFFFFFUL, (LSA_UINT16)0, 0, pDDB);
            }

            #if defined (EDDI_CFG_USE_SW_RPS) 
            //2) Remove Software Scoreboard leave for ERTEC400
            if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
            {
                EDDI_CRTSbSwRemove(pDDB, pConsumer);
            }
            #endif //EDDI_CFG_USE_SW_RPS
            
            //3) deactivate Consumer
            pConsumer->pSB->Status = EDDI_CRT_CONS_STS_INACTIVE;

            if  /* passivating of the AUX consumer does not have to influence the RTC3 consumer */
                ( (pConsumer->IRTtopCtrl.pAscConsumer) && (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER))
            {
                #if defined (EDDI_CFG_USE_SW_RPS) 
                if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
                {
                    EDDI_CRTSbSwRemove(pDDB, pConsumer->IRTtopCtrl.pAscConsumer);
                }
                #endif //EDDI_CFG_USE_SW_RPS
                pConsumer->IRTtopCtrl.pAscConsumer->pSB->Status = EDDI_CRT_CONS_STS_INACTIVE;
            }

            if (pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)
            {
                //do not risk a spike in DataStatus.State
                    EDDI_CSRT_KEEP_DATASTATUS(DataStatus, pAPDU, EDD_CSRT_DSTAT_BIT_STATE);
            }
            pAPDU->Detail.DataStatus     = DataStatus;
            pConsumer->pSB->OldDataStatus= DataStatus;

            if (EDDI_LIST_TYPE_ACW == pConsumer->LowerParams.ListType)
            {
                #if defined (EDDI_INTCFG_CONS_BUFFER_IF_3BHW)
                if (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
                {
                    #if defined (EDDI_CFG_REV7)
                    if (!pConsumer->usingPAEA_Ram)
                    #endif
                    {
                        pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer1;
                        pAPDU->Detail.DataStatus = DataStatus;

                        pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer2;
                        pAPDU->Detail.DataStatus = DataStatus;
                    }
                }
                #endif
                EDDI_SERSrtConsumerSetActivity(pConsumer->pLowerCtrlACW, LSA_FALSE);

                #if defined (EDDI_CFG_SMALL_SCF_ON)
                if (0 == pDDB->CRT.ConsumerList.ActiveConsRTC12)
                {
                    EDDI_Excp("EDDI_CRTConsumerPassivate, ActiveConsRTC12 underflow", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                    return;
                }
                pDDB->CRT.ConsumerList.ActiveConsRTC12--;
                if (0 == (pDDB->CRT.ConsumerList.ActiveConsRTC3 + pDDB->CRT.ConsumerList.ActiveConsRTC12))
                {
                    if (!EDDI_CRTRpsIsStopped(&pDDB->CRT.Rps))
                    {
                        EDDI_CRTRpsTrigger(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_STOP);
                    }
                }
                #endif //defined (EDDI_CFG_SMALL_SCF_ON)

                //SYSRED consumers get set to state BACKUP when passive
                if (pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)
                {
                    //EDDI_CSRT_SET_DATASTATUS_STATE(DataStatus, EDD_CSRT_DSTAT_BIT_STATE_BACKUP);
                    //pAPDU->Detail.DataStatus = DataStatus;
                    #if defined (EDDI_INTCFG_CONS_BUFFER_IF_3BHW)
                    if (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
                    {
                        #if defined (EDDI_CFG_REV7)
                        if (!pConsumer->usingPAEA_Ram)
                        #endif
                        {
                            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer1;
                            pAPDU->Detail.DataStatus = DataStatus;

                            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pConsumer->LowerParams.pKRAMDataBuffer2;
                            pAPDU->Detail.DataStatus = DataStatus;
                        }
                    }
                    #endif
                }
            }

            //4) trigger Scoreboard-SM
            if (EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE)) {} //Lint
            
            pConsumer->pSB->UkStatus    = EDDI_CRT_CONS_UK_OFF;
            #if defined (EDDI_CFG_USE_SW_RPS)
            pConsumer->pSB->UkCountdown = 0;
            #endif

            EDDI_CRT_CONSTRACE_TRACE(pConsumer, *((LSA_UINT32 *)(LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer)), 0, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_DEACT);

            //4b)Remove ACW 
            if (   bRemoveACWs
                && (EDDI_LIST_TYPE_ACW == pConsumer->LowerParams.ListType) )
            {
                if (EDDI_NULL_PTR != pConsumer->pLowerCtrlACW)
                {
                    //synchronous removal of ACW
                    EDDI_SRTConsumerRemove(pDDB, pConsumer->pLowerCtrlACW, EDDI_NULL_PTR);
                    pConsumer->pLowerCtrlACW = EDDI_NULL_PTR;
                }
            }

            //5) Deactivate associated providers ("Provider AutoStop")
            if (    (pConsumer->bEnableAutoStop)
                && (
                        ((pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_RTC3_MAIN)    == EDDI_CONS_ACTIVE_RTC3_MAIN)
                     || ((pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_CLASS12_MAIN) == EDDI_CONS_ACTIVE_CLASS12_MAIN)
                   )
               ) //STATE: ACTIVE
            {
                LSA_RESULT const Status = EDDI_CrtASCtrlAllProviderList(pDDB, pConsumer, LSA_FALSE /*RemoveASCoupling*/, LSA_TRUE /*DeactivateProvider*/ );
            
                if (Status != EDD_STS_OK)
                {
                    EDDI_Excp("EDDI_CrtASCtrlAllProviderList, Error", EDDI_FATAL_ERR_EXCP, Status, pConsumer);
                }

                //switch ProvAS off until next ACTIVATE
                pConsumer->bEnableAutoStop = LSA_FALSE;
            }
            
            //6) free framehandler
            if (EDDI_LIST_TYPE_FCW ==  pConsumer->LowerParams.ListType)
            {     
                //remove bindings to fcw
                if (!pConsumer->pFrmHandler)
                {
                    //UsedByUpper does not have to be checked (is always paired with pFrmHandler)
                    //no framehandler present.
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTConsumerPassivate, EDDI_NULL_PTR, pProvider->pFrmHandler, ConsID:0x%X", 
                                      pConsumer->ConsumerId);
                    //EDDI_Excp("EDDI_CRTConsumerPassivate EDDI_NULL_PTR, pProvider->pFrmHandler", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, 0);
                    return;
                }
                
                //Remove first the x+1 Redundant Consumer (only Framehandler), if exist
                if (pConsumer->pFrmHandler->pFrmHandlerRedundant)
                {
                    EDDI_IRTConsumerRemove(pDDB, pConsumer->pFrmHandler->pFrmHandlerRedundant);
                    pConsumer->pFrmHandler->pFrmHandlerRedundant->UsedByUpper = LSA_FALSE;
                    //Do not kill pFrmHandler->pFrmHandlerRedundant (= EDDI_NULL_PTR), because you will destroy the associate redundant FrameID list            
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerPassivate, FrameHandler->pFrmHandlerRedundant FREED: ConsId:0x%X FrameId:0x%X pFrmHandler:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, (LSA_UINT32)pConsumer->pFrmHandler);
                }
                
                EDDI_IRTConsumerRemove(pDDB, pConsumer->pFrmHandler);
                pConsumer->pFrmHandler->UsedByUpper = LSA_FALSE;
                pConsumer->pFrmHandler              = EDDI_NULL_PTR;
                
                #if defined (EDDI_CFG_SMALL_SCF_ON)
                if (0 == pDDB->CRT.ConsumerList.ActiveConsRTC3)
                {
                    EDDI_Excp("EDDI_CRTConsumerPassivate, ActiveConsRTC3 underflow", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                    return;
                }
                pDDB->CRT.ConsumerList.ActiveConsRTC3--;
                if (0 == (pDDB->CRT.ConsumerList.ActiveConsRTC3 + pDDB->CRT.ConsumerList.ActiveConsRTC12))
                {
                    if (!EDDI_CRTRpsIsStopped(&pDDB->CRT.Rps))
                    {
                        EDDI_CRTRpsTrigger(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_STOP);
                    }
                }
                #endif //defined (EDDI_CFG_SMALL_SCF_ON)
                
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerPassivate, FrameHandler FREED: ConsId:0x%X FrameId:0x%X pFrmHandler:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, (LSA_UINT32)pConsumer->pFrmHandler);
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerSetBuffer()
 *
 *  Description:Set all paramaters in DDB and ACW that are related to buffer-offset
 *              and buffer-length.
 *
 *  Arguments:  
 *
 *  Return:
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetBuffer( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    //init buffer-params
    #if defined (EDDI_CFG_REV7)
    if (pConsumer->usingPAEA_Ram)
    {
        if (   (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
            && (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset))
        {
            //pKRAMDataBuffer points to the 8Byte statebuffer and had been set before!
            EDDI_CRT_PAEA_APDU_STATUS * const pPAEA_APDU = (EDDI_CRT_PAEA_APDU_STATUS *)((void *)pConsumer->LowerParams.pKRAMDataBuffer);

            //Set user data application pointer to PAEA-RAM (offset relative to PAEA-Baseaddress)
            pPAEA_APDU->pPAEA_Data = EDDI_HOST2IRTE32(pConsumer->LowerParams.DataOffset);
        }
    }
    else
    #endif //defined (EDDI_CFG_REV7)
    {
        //Rev5/Rev6: AUX-Consumer gets no data buffer
        if (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER)
        {
            if (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
            {
                pConsumer->LowerParams.pKRAMDataBuffer = pDDB->pKRam + pConsumer->LowerParams.DataOffset;
                pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
            }
            else
            {
                pConsumer->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
            }
        }
    }

    //---- Class1/2 -----
    if (EDDI_LIST_TYPE_ACW == pConsumer->LowerParams.ListType)
    {
        if (   (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
            || (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType))
        {
            EDDI_CRTConsumerInitSingleBuffer(pConsumer, pConsumer->LowerParams.DataOffset, pDDB, pConsumer->LowerParams.ConsumerType);
        }

        //init ACW
        if (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
        {
            EDDI_SER10_ACW_RCV_TYPE  AcwRcvLocal;

            //get acw contents
            DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)&AcwRcvLocal, (LSA_UINT32 *)(LSA_VOID *)&(pConsumer->pLowerCtrlACW->pKramCW->AcwRcv), sizeof(AcwRcvLocal));

            //set values in local ACW
            EDDI_SetBitField32(&AcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__pDB0,
                               DEV_kram_adr_to_asic_register(pConsumer->LowerParams.pKRAMDataBuffer, pDDB));                     /* 21 Bit */

            EDDI_SetBitField32(&AcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__Sync,
                               (pConsumer->LowerParams.ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);                                 /*  1 Bit */

            EDDI_SetBitField32(&AcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__Async,
                               (pConsumer->LowerParams.ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0);                                /*  1 Bit */

            //write back ACW contents
            DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)&(pConsumer->pLowerCtrlACW->pKramCW->AcwRcv), (LSA_UINT32 *)(LSA_VOID *)&AcwRcvLocal, sizeof(AcwRcvLocal));
        }
    }
        //---- Class3 -----
    else if (EDDI_LIST_TYPE_FCW == pConsumer->LowerParams.ListType)
    {
        EDDI_CRTSetImageMode(pDDB, pConsumer, LSA_FALSE /*bIsProv*/);

        if (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
        {
            EDDI_CRTConsumerInitSingleBuffer(pConsumer, pConsumer->LowerParams.DataOffset, pDDB, pConsumer->LowerParams.ConsumerType);
        }

        //init FCW
        if (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
        {
            EDDI_SER10_FCW_RCV_TYPE                   FcwRcvLocal;
            EDDI_SER10_FCW_RCV_TYPE                 * pFcwRcvKRAM;
            EDDI_IRT_FRM_HANDLER_PTR_TYPE             pFrmHandler[2];
            LSA_UINT32                                iDataElem;
            EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE       pFrmData;
            LSA_UINT32                                HwPortIndex;
            LSA_UINT8                                 i;

            //find FCW
            pFrmHandler[0] = EDDI_SyncIrFindFrameHandler(pDDB, pConsumer->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
            if (EDDI_NULL_PTR == pFrmHandler[0])
            {
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerSetBuffer, EDDI_NULL_PTR == pFrmHandler");
                EDDI_Excp("EDDI_CRTConsumerSetBuffer, EDDI_NULL_PTR, pFrmHandler", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
            
            //pFrmHandler[0]: Main FCW Frame
            //pFrmHandler[1]: Associated Redunandant FCW Frame
            
            if (EDDI_IS_DFP_CONS(pConsumer))
            {
                //MRPD Associated Redunandant (x+1) not allowed with DFP  
                pFrmHandler[1] = (EDDI_IRT_FRM_HANDLER_TYPE *)0;        
            }
            else
            {
                pFrmHandler[1] = (pFrmHandler[0]->pFrmHandlerRedundant)?pFrmHandler[0]->pFrmHandlerRedundant:(EDDI_IRT_FRM_HANDLER_TYPE *)0;
            }
                
            for (i=0; i<2; i++)
            {
                if (pFrmHandler[i]) 
                {
                    for (iDataElem = 0; iDataElem < pDDB->PM.PortMap.PortCnt; iDataElem++)
                    {
                        pFrmData = pFrmHandler[i]->pIrFrameDataElem[iDataElem];

                        if (EDDI_NULL_PTR == pFrmData)
                        {
                            if (iDataElem == 0)
                            {
                                //At least one DataElement must exist.
                                EDDI_Excp("EDDI_CRTConsumerSetBuffer: EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
                            }
                            break;
                        }

                        if (   (EDDI_SYNC_IRT_CONSUMER     != pFrmHandler[i]->HandlerType)
                            && (EDDI_SYNC_IRT_FWD_CONSUMER != pFrmHandler[i]->HandlerType))
                          {
                            EDDI_Excp("EDDI_CRTConsumerSetBuffer: Wrong HandlerType", EDDI_FATAL_ERR_EXCP, pFrmHandler[i]->HandlerType, 0);
                            return;
                        }

                        HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4);
                        pFcwRcvKRAM = &pFrmHandler[i]->pIRTRcvEl[HwPortIndex]->pCW->FcwAcw.FcwRcv;

                        //get fcw contents
                        DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)&FcwRcvLocal, (LSA_UINT32 *)(LSA_VOID *)pFcwRcvKRAM, sizeof(FcwRcvLocal));

                        EDDI_SetBitField32(&FcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__pDB0,
                                            DEV_kram_adr_to_asic_register(pConsumer->LowerParams.pKRAMDataBuffer, pDDB)); //  21 Bit

                        //Set to 1 only, to keep settings for SOL-Dummy-FCW!
                        if (pConsumer->LowerParams.ImageMode & EDDI_IMAGE_MODE_SYNC)
                        {
                            EDDI_SetBitField32(&FcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync, 1);  // 1 Bit
                        }

                        //Set to 1 only, to keep settings for SOL-Dummy-FCW!
                        if (pConsumer->LowerParams.ImageMode & EDDI_IMAGE_MODE_ASYNC)
                        {
                            EDDI_SetBitField32(&FcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 1);  // 1 Bit
                        }

                        #ifdef EDDI_PRAGMA_MESSAGE
                        #pragma message ("MultiEDDI")
                        #endif /* EDDI_PRAGMA_MESSAGE */
                        #if defined (EDDI_CFG_REV7)
                        EDDI_SetBitField32(&FcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EXT, (pConsumer->usingPAEA_Ram)?1:0);  // 1 Bit
                        #endif

                        //store fcw contents
                        DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)pFcwRcvKRAM, (LSA_UINT32 *)(LSA_VOID *)&FcwRcvLocal, sizeof(FcwRcvLocal));
                    }
                } //end if
            } //end for
        } //end for
    }

    if (   (EDD_DATAOFFSET_UNDEFINED != pConsumer->LowerParams.DataOffset)
        && (pConsumer->LowerParams.ConsumerType != EDDI_RTC3_AUX_CONSUMER))
    {
        #if defined (EDDI_CFG_REV7)
        if (!pConsumer->usingPAEA_Ram)
        #endif
        {
            LSA_UINT16 const DataLen = (0 != pConsumer->LowerParams.Partial_DataLen)?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;

            EDDI_CRTCheckAddRemoveMirror(pDDB, DataLen, pConsumer->LowerParams.DataOffset, pConsumer->LowerParams.ListType, LSA_FALSE, CRT_MIRROR_ADD);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetMACIP(       EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE  const pConsumer,
                                                         const EDD_MAC_ADR_TYPE          * const pSrcMAC,
                                                         const EDD_IP_ADR_TYPE           * const pSrcIP )
{
    LSA_UINT8  Ctr;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsSetMAC_IP->");

    if (pConsumer->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
    {
        //change MAC-Adress
        pConsumer->LowerParams.SrcMAC = *pSrcMAC;
        
        if  /* AUX-COns exists */
            (pConsumer->IRTtopCtrl.pAscConsumer)
        {
            //Enter MACAdr for AUX-Cons, as initACW and EDDI_CRTConsumerSetMACIPACW access the AUX-LowerParams
            pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.SrcMAC = *pSrcMAC;
        }
    }
    else
    {  
        //change IP-Adress
        for (Ctr = 0; Ctr < EDD_IP_ADDR_SIZE; Ctr++)
        {
            pConsumer->LowerParams.SrcIP.b.IP[Ctr] = pSrcIP->IPAdr[Ctr];
        }  
    }

    //---- Class1/2 -----
    //MAC/IP-Addr is set in ACW before activating the consumer
    //---- Class3 -----
    //MAC-Addr is set in FCW before activating the consumer 
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetMACIPACW( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                            EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const pConsumer)
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerSetMACIPACW->");

    //---- Class1/2 -----
    if (EDDI_LIST_TYPE_ACW == pConsumer->LowerParams.ListType)
    {
        EDDI_SER10_ACW_RCV_TYPE  AcwRcvLocal;

        //get acw contents
        DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)&AcwRcvLocal, (LSA_UINT32 *)(LSA_VOID *)&(pConsumer->pLowerCtrlACW->pKramCW->AcwRcv), sizeof(AcwRcvLocal));

        if (   (pConsumer->LowerParams.SrcMAC.MacAdr[0] == 0)
            && (pConsumer->LowerParams.SrcMAC.MacAdr[1] == 0)
            && (pConsumer->LowerParams.SrcMAC.MacAdr[2] == 0)
            && (pConsumer->LowerParams.SrcMAC.MacAdr[3] == 0)
            && (pConsumer->LowerParams.SrcMAC.MacAdr[4] == 0)
            && (pConsumer->LowerParams.SrcMAC.MacAdr[5] == 0))
        {
            EDDI_SetBitField32(&AcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__CSrcAdr, 0);
        }
        else
        {
            EDDI_SetBitField32(&AcwRcvLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__CSrcAdr, 1);
        }

        EDDI_SERIniMACAdr(pDDB, &AcwRcvLocal.Hw2, &pConsumer->LowerParams.SrcMAC);

        #if defined (EDDI_CFG_REV7)
        //IP-Adr. simply overwrites the MAC-Adr here
        if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
        {
            EDDI_SetBitField32(&AcwRcvLocal.Hw4.Value.U32_0, EDDI_SER10_LLx_UDP_ACW_RCV_BIT__SourceIPAddr,
                                EDDI_HTONL(pConsumer->LowerParams.SrcIP.dw));
        }
        #endif //EDDI_CFG_REV7

        //write back ACW contents
        DEV32_MEMCOPY((LSA_UINT32 *)(LSA_VOID *)&(pConsumer->pLowerCtrlACW->pKramCW->AcwRcv), (LSA_UINT32 *)(LSA_VOID *)&AcwRcvLocal, sizeof(AcwRcvLocal));
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsCheckMAC_IP()
 *
 *  Description:Check if mac- or ip-adr is valid
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *
 *  Return:     EDD_STS_OK if MAC and IP adr are valid
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerCheckMACIP( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE    const  pConsumer,
                                                             EDD_MAC_ADR_TYPE            const * const  pSrcMAC,
                                                             EDD_IP_ADR_TYPE             const * const  pSrcIP )

{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsCheckMAC_IP->");

    if (pConsumer->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
    {
        //is MAC-Adress valid?
        if (pSrcMAC->MacAdr[0] & 0x1)  //only unicast
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsCheckMAC_IP, Invalid MAC-Adress.");
            return EDD_STS_ERR_PARAM;
        }
    }
    else
    {  
        //is IP-Adress valid
        if (  ((pSrcIP->IPAdr[0] == 0x0)
            && (pSrcIP->IPAdr[1] == 0x0)
            && (pSrcIP->IPAdr[2] == 0x0)
            && (pSrcIP->IPAdr[3] == 0x0))
            || (pSrcIP->IPAdr[0] &  0xE0) ) //no IP-Multicast
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsCheckMAC_IP, Invalid IP-Adress.");
            return EDD_STS_ERR_PARAM;
        }
    } 
    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerAddListTypeACW()
 *
 *  Description:
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *
 *  Return:
 */
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerAddListTypeACW( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                      EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer)
{
    // Reset ScoreBoard
    EDDI_SERClearTsbEntry(pDDB, pConsumer);
    EDDI_SRTConsumerAdd(pDDB, pConsumer);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsSetAndResetRedundancyAndPendingEvent()
 *
 *  Description:
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *
 *  Return:     LSA_TRUE: An event is pending
 */
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetToUnknown( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                     EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsSet2UK,
                                                                     EDDI_CRT_CONS_HWWD          const  eHwWatchdog,
                                                                     LSA_BOOL                    const  bTrigConsSBSM )
{
    LSA_BOOL  bPendingEvent = LSA_FALSE;
    
    if (EDDI_IS_DFP_CONS(pConsSet2UK))
    {
        return (EDDI_DFPConsumerSetToUnknown(pDDB, pConsSet2UK, eHwWatchdog,  bTrigConsSBSM));
    }
    else
    {
        LSA_UINT32                  PendingEvent;
        EDDI_CRT_CONSUMER_PTR_TYPE  pConsumer[2];
        LSA_UINT8                   LoopCtr;

        //check for consumers to handle
        pConsumer[0] = 
        pConsumer[1] = (EDDI_CRT_CONSUMER_PTR_TYPE)0;
        
        if (   ( EDDI_RTC3_CONSUMER == pConsSet2UK->LowerParams.ConsumerType)
            && ( pConsSet2UK->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN))
        {
            if (pConsSet2UK->IRTtopCtrl.pAscConsumer != LSA_NULL)
            {
                //handle AUX-Cons first
                pConsumer[0] = pConsSet2UK->IRTtopCtrl.pAscConsumer; 
            }
        }
        
        if (   (EDDI_RTC1_CONSUMER == pConsSet2UK->LowerParams.ConsumerType)
            || (EDDI_RTC2_CONSUMER == pConsSet2UK->LowerParams.ConsumerType)
            || (   (EDDI_RTC3_CONSUMER == pConsSet2UK->LowerParams.ConsumerType)
                && (pConsSet2UK->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_RTC3_MAIN)))
        {
            //handle RTC3-Cons second
            pConsumer[1] = pConsSet2UK; 
        }

        for (LoopCtr=0; LoopCtr<2; LoopCtr++)
        {
            if (pConsumer[LoopCtr])
            {
                if /* set2unknown-command has been given */
                   (bTrigConsSBSM)
                {
                    if( EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer[LoopCtr], EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN)){} //Lint
                }

                if (EDDI_CRT_CONS_HWWD_LOAD == eHwWatchdog)
                {
                    //PASSIVE->ACTIVE: retrigger WDT/DHT, reset WDT expired
                    //all events have been cleared during ACIVATE
                    EDDI_SERResetRedundancyAndTsbEntry(pDDB, pConsumer[LoopCtr], eHwWatchdog);
                    
                    #if defined (EDDI_CFG_USE_SW_RPS)
                    EDDI_CRTSbSwSet2UK(pDDB, pConsumer[LoopCtr], LSA_TRUE /*bActivate*/);
                    #endif //EDDI_CFG_USE_SW_RPS
                    pConsumer[LoopCtr]->pSB->UkStatus = EDDI_CRT_CONS_UK_COUNTING;
                }
                else
                {
                    //An active consumer is set to UK state
                    EDDI_SER10_TIMER_SCORB_TYPE  *  const pKramTimerSBEntry = (EDDI_SER10_TIMER_SCORB_TYPE *)(void *)pConsumer[LoopCtr]->LowerParams.pSBTimerEntry;
                    LSA_UINT32                            WDTVal;
                    
                    //check if Cons. is still active, but WD has expired
                    #if defined (EDDI_CFG_REV7)
                        WDTVal = EDDI_GetBitField32(pKramTimerSBEntry->Reg[1], EDDI_SER10_TIMER_SCORB_BIT2__WatchdogTimer);
                    #elif defined (EDDI_CFG_REV6)
                        WDTVal = EDDI_GetBitField32(pKramTimerSBEntry->Reg[0], 12, 0 /*EDDI_SER10_TIMER_SCORB_BIT__WatchdogTimer*/);
                    #else
                        WDTVal = EDDI_GetBitField16(pKramTimerSBEntry->Reg, EDDI_SER10_TIMER_SCORB_BIT__WatchdogTimer);
                    #endif

                    //check if WDT has expired
                    #if defined (EDDI_CFG_REV5)
                    if ((0 == WDTVal) && (EDDI_CRT_CONS_UK_COUNTING != pConsumer[LoopCtr]->pSB->UkStatus) )
                    #else
                    if (0 == WDTVal)
                    #endif
                    {
                        //maybe here is a racecondition between SetToUnknown and the MISS-Ind. This can only occur
                        //for the AUX-Cons., as it has no Consumer-AutoStop
                        //for safety-reasons the MISS-event is sent (again)
                        //get the current CycleCtr
                        #if defined (EDDI_CFG_USE_SW_RPS)
                        LSA_UINT16  const CycleCnt = ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer[LoopCtr]->LowerParams.pKRAMDataBuffer))->Detail.CycleCnt;
                        LSA_UINT8   const TransferStatus = ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer[LoopCtr]->LowerParams.pKRAMDataBuffer))->Detail.TransferStatus;
                        #else
                        LSA_UINT16                            CycleCnt;
                        EDDI_SER_DATA_STATUS_TYPE             DataStatus;
                        LSA_UINT8                             TransferStatus;
                        EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer[LoopCtr], &TransferStatus, &DataStatus, &CycleCnt);
                        #endif
                        
                        PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer[LoopCtr], EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                        EDDI_CRTConsumerSetPendingEvent(pConsumer[LoopCtr], PendingEvent, CycleCnt, TransferStatus, pDDB);
                        bPendingEvent = LSA_TRUE;
                    }                
                    else
                    {
                        //if consumer is only set to unknown-state (has been active before) -> clear only pending MISS/AGAIN-events                                                                    
                        EDDI_CRTConsumerSetPendingEvent(pConsumer[LoopCtr], (EDDI_CSRT_CONS_EVENT_NEGATION + EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED + EDD_CSRT_CONS_EVENT_MISS + EDD_CSRT_CONS_EVENT_AGAIN), (LSA_UINT16)0, 0, pDDB);
                        //do not reload WDT/DHT
                        EDDI_SERResetRedundancyAndTsbEntry(pDDB, pConsumer[LoopCtr], eHwWatchdog);

                        #if defined (EDDI_CFG_USE_SW_RPS)
                        EDDI_CRTSbSwSet2UK(pDDB, pConsumer[LoopCtr], LSA_FALSE /*bActivate*/);
                        pConsumer[LoopCtr]->pSB->UkStatus = EDDI_CRT_CONS_UK_COUNTING;
                        #endif //EDDI_CFG_USE_SW_RPS
                    }
                }

                #if defined (EDDI_CFG_USE_SW_RPS)
                EDDI_CRT_CONSTRACE_TRACE(pConsumer[LoopCtr], *((LSA_UINT32 *)(LSA_VOID *)(pConsumer[LoopCtr]->LowerParams.pKRAMDataBuffer)), pConsumer[LoopCtr]->pSB->UkCountdown, pConsumer[LoopCtr]->ScoreBoardStatus, EDDI_CONSTRACE_STATE_SETTOUK);
                #else
                EDDI_CRT_CONSTRACE_TRACE(pConsumer[LoopCtr], *((LSA_UINT32 *)(LSA_VOID *)(pConsumer[LoopCtr]->LowerParams.pKRAMDataBuffer)), 0, pConsumer[LoopCtr]->ScoreBoardStatus, EDDI_CONSTRACE_STATE_SETTOUK);
                #endif
            }   //end if (pConsumer[LoopCtr])
        } //end for()
    } //end if
    
    return bPendingEvent;
}
/*---------------------- end [subroutine] ---------------------------------*/


static  LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerClearEvents( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                   EDDI_CRT_EVENT_TYPE         const  Event,
                                                                   LSA_UINT16                  const  CycleCnt )
{
    //clear events
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTConsumerClearEvents->PendingIndEvent reset, FrameId:0x%X res:0x%X", pConsumer->LowerParams.FrameId, Event);
                      
    pConsumer->PendingIndEvent &= ~(Event);
    pConsumer->PendingCycleCnt =  CycleCnt;

    if (EDDI_CSRT_CONS_EVENT_RESET_EVENT_HANDLING == Event)
    {
        //Take the OldDataStatus as reference, as it should contain the init value
        pConsumer->EventStatus =   (EDDI_DS_BIT__Stop0_Run1(pConsumer->pSB->OldDataStatus)?EDD_CSRT_CONS_EVENT_RUN:EDD_CSRT_CONS_EVENT_STOP)
                                 | (EDDI_DS_BIT__StationFailure(pConsumer->pSB->OldDataStatus)?EDD_CSRT_CONS_EVENT_STATION_OK:EDD_CSRT_CONS_EVENT_STATION_FAILURE)
                                 | (EDDI_DS_BIT__State_backup0_primary1(pConsumer->pSB->OldDataStatus)?EDD_CSRT_CONS_EVENT_PRIMARY:EDD_CSRT_CONS_EVENT_BACKUP)
                                 | (EDDI_DS_BIT__Redundancy(pConsumer->pSB->OldDataStatus)?EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS:EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS);
    }
    else if (EDDI_CSRT_CONS_EVENT_CLEAR_PENDING_EVENTS == Event) 
    {
        //EDDI_CSRT_CONS_EVENT_CLEAR_PENDING_EVENTS does not affect the EventStatus!
    }
    else
    {
        //reset flags 
        pConsumer->EventStatus &= ~(Event);
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerSetEvents()
 *
 *  Description:
 *
 *  Arguments:
 *
 */
//IRTOP kommentieren
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetEvents( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                  EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                  EDDI_CRT_EVENT_TYPE         const  SetEvent,
                                                                  EDDI_CRT_EVENT_TYPE         const  RemoveEvent )
{
    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
    {
        //event from AUX-cons
        if (   (pConsumer->IRTtopCtrl.pAscConsumer != LSA_NULL) 
            && (pConsumer->IRTtopCtrl.pAscConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN))
        {
            //if AUX-cons is still valid and event has not been indicated yet
            if (!(pConsumer->IRTtopCtrl.pAscConsumer->EventStatus & SetEvent))
            {
                //indicate event to class3-cons
                pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent &= ~RemoveEvent;
                pConsumer->IRTtopCtrl.pAscConsumer->PendingIndEvent |=  SetEvent;
                pConsumer->IRTtopCtrl.pAscConsumer->EventStatus      = (pConsumer->IRTtopCtrl.pAscConsumer->EventStatus & (~RemoveEvent)) | SetEvent;
            }
        }
    }
    else
    {
        //Check if event occured in a packframe 
        if (!EDDI_DFPConsumerSetEvents(pDDB, pConsumer, SetEvent))
        {
            //event from class123-cons
            if /* any new event */
               (SetEvent != (pConsumer->EventStatus & SetEvent))
            {    
                pConsumer->PendingIndEvent &= ~RemoveEvent;
                pConsumer->PendingIndEvent |=  SetEvent;
                pConsumer->EventStatus      = (pConsumer->EventStatus & (~RemoveEvent)) | SetEvent;
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTCons_IRTtopSM()
 *
 *  Description: Consumer IRTTOP-state-machine
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *              Trigger:
 *
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerIRTtopSM( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                         EDDI_CRT_CONSUMER_PTR_TYPE        const  pConsumer,
                                                         EDDI_CONS_IRTTOP_SM_TRIGGER_TYPE  const  Trigger )
{
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCons_IRTtopSM->ConsID:0x%X Trigger:0x%X", 
                      pConsumer->ConsumerId, Trigger);

    LSA_UNUSED_ARG(pDDB);

    switch (pConsumer->IRTtopCtrl.ConsState)
    {
        //******************************************
        //STATE: "Passive" -> AUX passive, Class3 passive
        case EDDI_CONS_STATE_PASSIVE:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA:
                {
                    if(pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
                    {
                        //Only the Class3-consumer will be activated AFTER calling this function
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_PASSIVE to STATE_WF_IRDATA with trigger 0x%X", Trigger);
                        pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_AUX_MAIN;
                        pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_WF_IRDATA;
                    }
                    break;
                }
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA:
                {
                    //check legacy-mode
                    if(pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
                    {
                        //Wait for AGAIN(RED) or MISS(GREEN)
                        //Class3-consumer and AUX-consumer will both be activated AFTER calling this function
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_PASSIVE to STATE_WF_CLASS3_RX_LEGACY with trigger 0x%X", Trigger);
                        pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_RTC3_MAIN + EDDI_CONS_ACTIVE_AUX_MAIN;
                        pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_WF_CLASS3_RX_LEGACY;
                        break;
                    }
                    else
                    {
                        //Only the Class3-consumer will be activated AFTER calling this function
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_PASSIVE to STATE_WF_CLASS3_RX_NO_LEGACY with trigger 0x%X", Trigger);
                        pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_RTC3_MAIN;
                        pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_WF_CLASS3_RX_NO_LEGACY;
                    }
                    break;

                }
                case EDDI_CONS_SM_TRIGGER_PASSIVATE:
                case EDDI_CONS_SM_TRIGGER_AGAIN_GREEN:
                case EDDI_CONS_SM_TRIGGER_AGAIN_RED:
                case EDDI_CONS_SM_TRIGGER_MISS_GREEN:
                case EDDI_CONS_SM_TRIGGER_MISS_RED:
                {
                    //do nothing
                    break;
                }


                default:
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                                      pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
                    EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                }
            }
            break;
        }

        //*********************************************************
        //STATE: "Wait for irdata" -> Active, but no irdata present: AUX active, Class3 not available
        case EDDI_CONS_STATE_WF_IRDATA:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_TRIGGER_PASSIVATE:
                case EDDI_CONS_SM_TRIGGER_MISS_GREEN:
                {
                    //AUX has been passivated before by the calling fct!
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_WF_IRDATA to STATE_PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_NONE;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_PASSIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_AGAIN_GREEN:
                {
                    //do nothing
                    break;
                }

                case EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA:
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA:
                case EDDI_CONS_SM_TRIGGER_AGAIN_RED:
                case EDDI_CONS_SM_TRIGGER_MISS_RED:
                default:
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                                      pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
                    EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                }
            }
            break;
        }

        //**************************************************************************
        //STATE: "Wait for Class3 RX (Legacy)" -> Active, irdata present, but RTC3PSM not in RUN yet: AUX active, Class3 active
        case EDDI_CONS_STATE_WF_CLASS3_RX_LEGACY:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_TRIGGER_PASSIVATE:
                case EDDI_CONS_SM_TRIGGER_MISS_GREEN:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_WF_CLASS3_RX_LEGACY to STATE_PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_NONE;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_PASSIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_AGAIN_RED:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_WF_CLASS3_RX_LEGACY to STATE_ACTIVE with trigger 0x%X", Trigger);
                    //Passivate AUX consumer
                    if /* AUX consumer exists */
                       (pConsumer->IRTtopCtrl.pAscConsumer != LSA_NULL)
                    {
                        //Passivate AUX consumer, clear all pending events, do not influence RTC3 consumer!
                        EDDI_CRTConsumerPassivate(pDDB, pConsumer->IRTtopCtrl.pAscConsumer, LSA_TRUE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                    }
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_RTC3_MAIN;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_ACTIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_AGAIN_GREEN:
                case EDDI_CONS_SM_TRIGGER_MISS_RED:
                {
                    //do nothing
                    break;
                }

                case EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA:
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA:
                default:
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                                      pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
                    EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                }
            }
            break;
        }

        //**************************************************************************
        //STATE: "Wait for Class3 RX" ->Active, irdata present, but RTC3PSM not in RUN yet: AUX not available, Class3 passive
        case EDDI_CONS_STATE_WF_CLASS3_RX_NO_LEGACY:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_TRIGGER_MISS_RED:
                {
                    //do nothing
                    break;
                }

                case EDDI_CONS_SM_TRIGGER_AGAIN_RED:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from WF_CLASS3_RX_NO_LEGACY to ACTIVE_RTC3_MAIN with trigger 0x%X", Trigger);
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_RTC3_MAIN;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_ACTIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from WF_CLASS3_RX_NO_LEGACY to STATE_PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_NONE;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_PASSIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA:
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA:
                case EDDI_CONS_SM_TRIGGER_AGAIN_GREEN:
                case EDDI_CONS_SM_TRIGGER_MISS_GREEN:
                default:
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                                      pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
                    EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                }
            }
            break;
        }

        //***************************************************************************
        //STATE: "Active" -> Active, irdata present, RTC3PSM RUN: Final state: AUX passive, Class3 active
        case EDDI_CONS_STATE_ACTIVE:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_TRIGGER_MISS_RED:
                case EDDI_CONS_SM_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ConsSM: Transition from STATE_ACTIVE to STATE_PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->IRTtopCtrl.ConsActiveMask = EDDI_CONS_ACTIVE_NONE;
                    pConsumer->IRTtopCtrl.ConsState = EDDI_CONS_STATE_PASSIVE;
                }
                break;

                case EDDI_CONS_SM_TRIGGER_AGAIN_RED:
                case EDDI_CONS_SM_TRIGGER_AGAIN_GREEN:
                case EDDI_CONS_SM_TRIGGER_MISS_GREEN:
                {
                    //do nothing
                    break;
                }

                case EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA:
                case EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA:
                default:
                {
                    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                                      pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
                    EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                }
            }
            break;
        }

        default:
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCons_IRTtopSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                              pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState, Trigger);
            EDDI_Excp("EDDI_CRTCons_IRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerScoreBoardSM()
 *
 *  Description: Consumer CRTConsumer ScoreBoard-state-machine
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *              Trigger:
 *
 */
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerScoreBoardSM( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                               EDDI_CONS_SCOREBOARD_SM_TRIGGER_TYPE  const  Trigger )
{

    LSA_UINT32         PendingEvent = EDDI_CSRT_CONS_EVENT_NO;
    LSA_BOOL    const  bConsIsPF    = EDDI_DFPConsumerIsPF(pConsumer);

    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTConsumerScoreBoardSM->ConsID:0x%X Trigger:0x%X bIsPF:0x%X", 
                      pConsumer->ConsumerId, Trigger, bConsIsPF);

    LSA_UNUSED_ARG(pDDB);

    switch (pConsumer->ScoreBoardStatus)
    {
        //******************************************
        //STATE: "Miss"
        case EDDI_CONS_SCOREBOARD_STATE_MISS:
        {
            switch (Trigger)
            {               
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    if (bConsIsPF)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine (PF): Transition from MISS to AGAIN with trigger 0x%X", Trigger);
                        pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_AGAIN;
                    }
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                {
                    pConsumer->bHasSeenAgain = LSA_FALSE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                {
                    //do nothing
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                default:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }
            }
        } //lint -fallthrough
        
        //******************************************
        //STATE: "Passive" 
        case EDDI_CONS_SCOREBOARD_STATE_PASSIVE:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                {
                    //Only the Class3-consumer will be activated AFTER calling this function
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from STATE_PASSIVE to MISS_NOT_STOPPED_INIT with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_INIT;
                    
                    #if defined (EDDI_CFG_USE_SW_RPS)
                    pConsumer->pSB->Invalid = LSA_FALSE;
                    #endif
                    
                    if (pConsumer->bHasSeenMiss)
                    {
                        pConsumer->bHasSeenAgain = LSA_FALSE;
                    }
                    pConsumer->bHasSeenMiss = LSA_FALSE;

                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    pConsumer->bHasSeenAgain    = LSA_TRUE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                //case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                //case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                default:
                {
                    //do nothing
                    break;
                }
            }
            break;
        }

        //******************************************
        //STATE: "Miss not stopped init"
        case EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_INIT:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_INIT to MISS_NOT_STOPPED with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "#### Consumer Scoreboard state machine: Transition from EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_INIT to MISS with trigger 0x%X (PF is in state PASSIVE/MISS)", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS;
                    pConsumer->bHasSeenMiss = LSA_TRUE;
                    PendingEvent = EDD_CSRT_CONS_EVENT_MISS;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_INIT to MISS_NOT_STOPPED_UK with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_UK;
                    break;
                }       
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_INIT to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }  
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_INIT to AGAIN with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_AGAIN;
                    pConsumer->bHasSeenAgain    = LSA_TRUE;
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN;
                    break;
                }               
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                default:
                {
                    //do nothing
                    break;
                }
            }
            break;
        }
        
        //******************************************
        //STATE: "Miss not stopped" 
        case EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED to AGAIN with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_AGAIN;
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN;
                    pConsumer->bHasSeenAgain = LSA_TRUE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                default:
                {
                    //do nothing
                    break;
                }
            }
            break;
        }
        
        //******************************************
        //STATE: "Miss not stopped uk"
        case EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_UK:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_UK to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_UK to AGAIN with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_AGAIN;
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN;
                    pConsumer->bHasSeenAgain = LSA_TRUE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from MISS_NOT_STOPPED_UK to MISS_NOT_STOPPED with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED;
                    PendingEvent = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                default:
                {
                    //do nothing
                    break;
                }
            }
            break;
        } 
        
        //******************************************
        //STATE: "Again"
        case EDDI_CONS_SCOREBOARD_STATE_AGAIN:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from AGAIN to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from AGAIN to UNKNOWN with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_UNKNOWN;
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN; 
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "#### Consumer Scoreboard state machine: Transition from AGAIN to MISS with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS;
                    pConsumer->bHasSeenMiss = LSA_TRUE;
                    PendingEvent = EDD_CSRT_CONS_EVENT_MISS;   
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    /* we are in AGAIN and triggered by hw (Prod state changed to AGAIN) */
                    /* that means we lost a MISS (if we are active) */
                    if(EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "#### Consumer Scoreboard state machine: Transition from AGAIN to MISS with trigger 0x%X(lost MISS)", Trigger);
                        pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS;
                        PendingEvent = EDD_CSRT_CONS_EVENT_MISS;
                    }
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                {
                    //do nothing
                    break;
                }
                default:
                {
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN; 
                    break;
                }
            }
            break;
        } 

        //******************************************
        //STATE: "Unknow" 
        case EDDI_CONS_SCOREBOARD_STATE_UNKNOWN:
        {
            switch (Trigger)
            {
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from UNKNOWN to AGAIN with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_AGAIN;
                    pConsumer->bHasSeenAgain    = LSA_TRUE;
                    PendingEvent = EDD_CSRT_CONS_EVENT_AGAIN;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "#### Consumer Scoreboard state machine: Transition from UNKNOWN to MISS with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_MISS;
                    pConsumer->bHasSeenMiss = LSA_TRUE;
                    PendingEvent = EDD_CSRT_CONS_EVENT_MISS;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE:
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### Consumer Scoreboard state machine: Transition from UNKNOWN to PASSIVE with trigger 0x%X", Trigger);
                    pConsumer->ScoreBoardStatus = EDDI_CONS_SCOREBOARD_STATE_PASSIVE;
                    break;
                }
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE:
                case EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN:
                default:
                {
                    //do nothing
                    break;
                }
            }
            break;
        }

        default:
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTConsumerScoreBoardSM, TRIGGER ERROR, ConsID:0x%X State:0x%X Trigger:0x%X",
                              pConsumer->ConsumerId, pConsumer->ScoreBoardStatus, Trigger);
            EDDI_Excp("EDDI_CRTConsumerScoreBoardSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->ScoreBoardStatus);
        }
    }

    return PendingEvent;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:   EDDI_CheckLegacyConsistencyDataStatus()
 *
 *  Description: Updates the Event from Current DataStatus (RTC3 Consumer)
 *
 *  Arguments:  pConsumer: the related Consumer
 *
 */
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CheckLegacyConsistencyDataStatus(EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer)
{
     LSA_UINT32  LocalEventStatus = 0;  
                       
     //EDD_CSRT_DSTAT_BIT_STATE and EDD_CSRT_DSTAT_BIT_STATE not used with RTC3 legacy!

     // EDD_CSRT_DSTAT_BIT_STOP_RUN changed
     if ((EDDI_DS_BIT__Stop0_Run1(pConsumer->pSB->CurrentDataStatus))  && (!(pConsumer->EventStatus & EDD_CSRT_CONS_EVENT_RUN)))
     {
         LocalEventStatus += EDD_CSRT_CONS_EVENT_RUN;
         EDDI_SET_DS_BIT__Stop0_Run1(&pConsumer->pSB->OldDataStatus, 1);
     }

     if ((!(EDDI_DS_BIT__Stop0_Run1(pConsumer->pSB->CurrentDataStatus))) && ((pConsumer->EventStatus & EDD_CSRT_CONS_EVENT_STOP)))
     {
         LocalEventStatus += EDD_CSRT_CONS_EVENT_STOP;
         EDDI_SET_DS_BIT__Stop0_Run1(&pConsumer->pSB->OldDataStatus, 0);
     }

     // EDD_CSRT_DSTAT_BIT_STATION_FAILURE changed
     if ((EDDI_DS_BIT__StationFailure(pConsumer->pSB->CurrentDataStatus)) && (!(pConsumer->EventStatus & EDD_CSRT_CONS_EVENT_STATION_OK)))
     {
         LocalEventStatus += EDD_CSRT_CONS_EVENT_STATION_OK;
         EDDI_SET_DS_BIT__StationFailure(&pConsumer->pSB->OldDataStatus, 1);
     }

     if ((!(EDDI_DS_BIT__StationFailure(pConsumer->pSB->CurrentDataStatus)))  && (pConsumer->EventStatus & EDD_CSRT_CONS_EVENT_STATION_FAILURE))
     {
         LocalEventStatus += EDD_CSRT_CONS_EVENT_STATION_FAILURE;
         EDDI_SET_DS_BIT__StationFailure(&pConsumer->pSB->OldDataStatus, 0);
     }

     return LocalEventStatus;
 }



/******************************************************************************
 *  Function:   EDDI_CRTConsumerEventEvaluation()
 *
 *  Description: Consumer CRTConsumer Event Evaluation
 *
 *  Arguments:  pConsumer: the related Consumer
 *              pDDB:
 *              Trigger:
 *
 */
static  LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerIRTtopEventEvaluation( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                             EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                                             EDDI_CRT_EVENT_TYPE                   const  Event )
{
    EDDI_CRT_EVENT_TYPE             const  ClearEvent    = EDD_CSRT_CONS_EVENT_AGAIN + EDD_CSRT_CONS_EVENT_MISS + EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
    EDDI_CONS_IRTTOP_SM_STATE_TYPE  const  ConsState     = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer->IRTtopCtrl.ConsState:pConsumer->IRTtopCtrl.ConsState;
    EDDI_CRT_CONSUMER_PTR_TYPE      const  pConsumerRTC3 = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer->IRTtopCtrl.pAscConsumer:pConsumer;
    EDDI_CRT_CONSUMER_PTR_TYPE      const  pConsumerAUX  = (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)?pConsumer:pConsumer->IRTtopCtrl.pAscConsumer;
    
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTConsumerIRTtopEventEvaluation->ConsID:0x%X Event:0x%X", 
                      pConsumer->ConsumerId, Event);

    switch (ConsState)
    {
        case EDDI_CONS_STATE_WF_IRDATA:
        {
            if (EDDI_IS_DFP_CONS(pConsumer))
            {
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, 
                                  "Error: EDDI_CRTConsumerIRTtopEventEvaluation DFP-Frame.-> ConsID:0x%X, FrameID:0x%X, State:%d, Event:0x%X", 
                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, ConsState, Event);
                EDDI_Excp("Error: EDDI_CRTConsumerIRTtopEventEvaluation DFP-Frame", EDDI_FATAL_ERR_EXCP, Event, pConsumer->LowerParams.FrameId);
                return;
            }

            switch (Event)
            {
                case EDD_CSRT_CONS_EVENT_MISS:
                case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
                {
                    //MISS(GREEN)
                    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        if (pConsumerRTC3 != LSA_NULL)
                        {
                            //Passivate AUX-Consumer (Consumer-AS)
                            EDDI_CRTConsumerPassivate(pDDB, pConsumerAUX, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                            //Passivate RTC3-Consumer (Consumer-AS)
                            EDDI_CRTConsumerPassivate(pDDB, pConsumerRTC3, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                            EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_MISS_GREEN);
                            EDDI_CRTConsumerSetEvents(pDDB, pConsumerRTC3, Event, ClearEvent);
                        }
                        else
                        {
                            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "Error: EDDI_CRTConsumerIRTtopEventEvaluation no Aux Consumer available -> ConsID:0x%X", pConsumer->ConsumerId);
                        }
                    }
                    break;
                }

                case EDD_CSRT_CONS_EVENT_AGAIN:
                {
                    //AGAIN(GREEN)
                    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        if (pConsumerRTC3 != LSA_NULL)
                        {
                            EDDI_CRTConsumerSetEvents(pDDB, pConsumerRTC3, EDD_CSRT_CONS_EVENT_AGAIN, ClearEvent);
                            EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_AGAIN_GREEN);
                        }
                        else
                        {
                            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "Error: EDDI_CRTConsumerIRTtopEventEvaluation no Aux Consumer available -> ConsID:0x%X", pConsumer->ConsumerId);
                        }
                    }
                    break;
                }
                default:
                {
                    //do nothing
                }
            }
            break;
        }

        case EDDI_CONS_STATE_WF_CLASS3_RX_LEGACY:
        {
            if (EDDI_IS_DFP_CONS(pConsumer))
            {
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, 
                                  "Error: EDDI_CRTConsumerIRTtopEventEvaluation DFP-Frame.-> ConsID:0x%X, FrameID:0x%X, State:%d, Event:0x%X", 
                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, ConsState, Event);
                EDDI_Excp("Error: EDDI_CRTConsumerIRTtopEventEvaluation DFP-Frame", EDDI_FATAL_ERR_EXCP, Event, pConsumer->LowerParams.FrameId);
                return;
            }

            switch (Event)
            {
                case EDD_CSRT_CONS_EVENT_MISS:
                case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
                {
                    //MISS(GREEN)
                    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        //Deactivate RTC3-Consumer
                        if (pConsumerRTC3 != LSA_NULL)
                        {
                            if (EDD_CSRT_CONS_EVENT_MISS == Event)
                            {
                                //Passivate AUX-Consumer (Consumer-AS)
                                EDDI_CRTConsumerPassivate(pDDB, pConsumerAUX, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                                //Passivate RTC3-Consumer (Consumer-AS)
                                EDDI_CRTConsumerPassivate(pDDB, pConsumerRTC3, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);

                                EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_MISS_GREEN); //only a "real" MISS triggers the SM
                            }
                            EDDI_CRTConsumerSetEvents(pDDB, pConsumerRTC3, Event, ClearEvent);
                        }
                        else
                        {
                            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "Error: EDDI_CRTConsumerIRTtopEventEvaluation no Aux Consumer available -> ConsID:0x%X", pConsumer->ConsumerId);
                        }
                    }
                    //MISS(RED)
                    else if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_MISS_RED);
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx,  LSA_TRACE_LEVEL_NOTE, "EDDI_CRTConsumerIRTtopEventEvaluation, EVENT_MISS for ConsID:0x%X ignored.", pConsumer->ConsumerId);
                        //RTC3-Miss -> do Nothing
                    }
                    break;
                }
                case EDD_CSRT_CONS_EVENT_AGAIN:
                {
                    //AGAIN(GREEN)
                    if (EDDI_RTC3_AUX_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        if (pConsumerRTC3 != LSA_NULL)
                        {
                            EDDI_CRTConsumerSetEvents(pDDB, pConsumerRTC3, EDD_CSRT_CONS_EVENT_AGAIN, ClearEvent);
                            EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_AGAIN_GREEN);
                        }
                        else
                        {
                            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "Error: EDDI_CRTConsumerIRTtopEventEvaluation no Aux Consumer available -> ConsID:0x%X", pConsumer->ConsumerId);
                        }
                        
                    }
                    //AGAIN(RED)
                    else if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        LSA_UINT32  const  LocalEventStatus = EDDI_CheckLegacyConsistencyDataStatus(pConsumerRTC3);  
 
                        EDDI_CRTConsumerSetEvents(pDDB, pConsumerRTC3, EDD_CSRT_CONS_EVENT_AGAIN + EDD_CSRT_CONS_EVENT_ENTERING_RED + LocalEventStatus, ClearEvent);
                        EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_AGAIN_RED);
                    }
                    break;
                }
                default:
                {
                    //do nothing
                }
            }
            break;
        }

        case EDDI_CONS_STATE_WF_CLASS3_RX_NO_LEGACY:
        {
            switch (Event)
            {
                case EDD_CSRT_CONS_EVENT_MISS:
                case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
                {
                    // MISS(RED)
                    if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, ClearEvent);
                        if (EDD_CSRT_CONS_EVENT_MISS == Event)
                        {
                            if (!EDDI_DFPConsumerIsPF(pConsumer))
                            {
                                //Passivate RTC3-Consumer (Consumer-AS)
                                //no Consumer-AS for packframes! these get passivated when the last DG is passivated through Consumer-AS
                                EDDI_CRTConsumerPassivate(pDDB, pConsumer, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                                EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_MISS_RED); //only a "real" MISS triggers the SM
                            }
                            else
                            {
                                //DFP PF will stay in this state
                                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTConsumerIRTtopEventEvaluation: Event:0x%X skipped for PF-ConsID:0x%X, FrameID:0x%X in State WF_CLASS3_RX_NO_LEGACY",
                                                  Event, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                            }
                        }
                    }
                    break;
                }
                case EDD_CSRT_CONS_EVENT_AGAIN:
                {
                    //no AGAIN set before or AGAIN still pending
                    EDDI_CRTConsumerSetEvents(pDDB, pConsumer, EDD_CSRT_CONS_EVENT_AGAIN + EDD_CSRT_CONS_EVENT_ENTERING_RED, ClearEvent);
                    EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_AGAIN_RED);
                    break;
                }
                default:
                {
                    //do nothing
                }
            }
            break;
        }

        case EDDI_CONS_STATE_ACTIVE:
        {
            switch (Event)
            {
                case EDD_CSRT_CONS_EVENT_MISS:
                case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
                {
                    // MISS(RED):
                    if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        //Passivate RTC3-Consumer (Consumer-AS)
                        if (!EDDI_DFPConsumerIsPF(pConsumer))
                        {
                            //no Consumer-AS for packframes! these get passivated when the last DG is passivated through Consumer-AS
                            if (pConsumer->IRTtopCtrl.pAscConsumer)
                            {
                                //passivate AUX also
                                EDDI_CRTConsumerPassivate(pDDB, pConsumer->IRTtopCtrl.pAscConsumer, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                            }
                            EDDI_CRTConsumerPassivate(pDDB, pConsumer, LSA_FALSE /*bClearEvents */, LSA_FALSE /*bRemoveACWs*/);
                            EDDI_CRTConsumerSetEvents(pDDB, pConsumer, EDD_CSRT_CONS_EVENT_MISS, ClearEvent);  //LEAVING_RED is never generated!
                            EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_MISS_RED);

                        }
                        else
                        {
                            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTConsumerIRTtopEventEvaluation: Event:0x%X skipped for PF-ConsID:0x%X, FrameID:0x%X in State ACTIVE",
                                Event, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                            if (EDDI_DFPConsumerSetEvents(pDDB, pConsumer, EDD_CSRT_CONS_EVENT_MISS)) {} //Lint
                        }
                    }
                    break;
                }
                case EDD_CSRT_CONS_EVENT_AGAIN:
                {
                    if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType)
                    {
                        // AGAIN(RED): //Consumer was reset (SetToUnknown)
                        EDDI_CRTConsumerSetEvents(pDDB, pConsumer, Event, ClearEvent);
                        EDDI_CRTConsumerIRTtopSM(pDDB, pConsumerRTC3, EDDI_CONS_SM_TRIGGER_AGAIN_RED);
                    }
                    break;
                }
                default:
                {
                    //do nothing
                }
            }
            break;
        }

        case EDDI_CONS_STATE_PASSIVE:
        {
            //do nothing
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_CRTConsumerIRTtopEventEvaluation, unknown Event", EDDI_FATAL_ERR_EXCP, Event, pConsumer->LowerParams.FrameId);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCheckConsumerPhase()
 *
 *  Description: Check for valid CyclePhase and CycleReductionRatio 
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckConsumerPhase( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                     EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    LSA_RESULT  const  Status   = EDD_STS_OK;
    LSA_UINT8   const  ListType = pConsumer->LowerParams.ListType;

    //check CyclePhase (RTC3 legacy and RTC1/2 only)
    if (   (EDD_CYCLE_PHASE_UNDEFINED == pConsumer->LowerParams.CyclePhase)
        && (EDDI_RTC3_AUX_CONSUMER != pConsumer->LowerParams.ConsumerType)
        && (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckConsumerPhase, illegal CyclePhase:0x%X", pConsumer->LowerParams.CyclePhase);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
        return EDD_STS_ERR_PARAM;
    }

    //check CycleReductionRatio
    if (   ( EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pConsumer->LowerParams.CycleReductionRatio)
        && (EDDI_RTC3_AUX_CONSUMER != pConsumer->LowerParams.ConsumerType))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTCheckConsumerPhase, illegal CycleReductionRatio:0x%X", pConsumer->LowerParams.CycleReductionRatio);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
        return EDD_STS_ERR_PARAM;
    }
    else if (EDDI_RTC3_AUX_CONSUMER != pConsumer->LowerParams.ConsumerType)
    {
        switch (ListType)
        {
            case EDDI_LIST_TYPE_ACW:
            {
                if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB,
                                                                pDDB->pLocal_CRT->SrtRedTree.MaxReduction,
                                                                pConsumer->LowerParams.CycleReductionRatio,
                                                                pConsumer->LowerParams.CyclePhase,
                                                                LSA_FALSE /*bUndefAllowed*/))
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTCheckConsumerPhase, illegal CycleReductionRatio:0x%X", pConsumer->LowerParams.CycleReductionRatio);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                    return EDD_STS_ERR_PARAM;
                }
                break;
            }
            case EDDI_LIST_TYPE_FCW:
            {
                if (!EDDI_IS_DFP_CONS(pConsumer))
                {
                    if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pConsumer->LowerParams.CycleReductionRatio, 
                                                                   pConsumer->LowerParams.CyclePhase, (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)?LSA_TRUE:LSA_FALSE /*bCheckPhase*/))
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTCheckConsumerPhase, illegal CycleReductionRatio:0x%X", pConsumer->LowerParams.CycleReductionRatio);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }
            default:
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTCheckConsumerPhase, Invalid ListType:0x%X", ListType);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
                return EDD_STS_ERR_PARAM;
            }
        }
    }
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerCheckParamsForActivate()
 *
 *  Description: Check all parameters prior to activate 
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlCheckParamsForActivate( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                        EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE  const  pRQBParam,
                                                                                        EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer )
{
    LSA_RESULT                           Status = EDD_STS_OK;
    LSA_UINT32                    const  BufferProperties = (EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED == (pConsumer->LowerParams.BufferProperties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))?pRQBParam->IOParams.BufferProperties:pConsumer->LowerParams.BufferProperties;
    LSA_UINT32                    const  DataOffset       = (EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset)?pRQBParam->IOParams.DataOffset:pConsumer->LowerParams.DataOffset;
    LSA_BOOL                             bBufferPropChangedToSync = LSA_FALSE;
    
    // FILL has to be switched ON to activate consumers
    if (!pDDB->NRT.bIO_Configured)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerControlCheckParamsForActivate: activating Consumer(ID:0x%X) when IO-Configuration is NO, is not allowed",
                          pConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Consumer_Properties);
        return EDD_STS_ERR_SERVICE;
    }

    if (!pDDB->NRT.bFeedInLoadLimitationActive)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                         "EDDI_CRTConsumerControlCheckParamsForActivate: activating Consumer(ID:0x%X) when LoadLimitationActive is FALSE, is not allowed",
                          pConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Consumer_Properties);
        return EDD_STS_ERR_SERVICE;
    }

    //************************************************************************************
    //RTC1/2/3 Step1:
    //consumer gets activated ==> check parameters for dataoffset, BufferProperties
    //SrcIP and SrcMAC can always be changed (but have to be valid)!
    //if there had been valid parameters given at CONSUMER_ADD, ignore parameters here!

    #if defined (EDDI_CFG_REV7)
    if (     (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK))
         &&  (EDD_CSRT_CONS_PROP_RTCLASS_3 == (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
         #if defined (EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
         &&  (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
         #endif //(EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3) 
       )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerControlCheckParamsForActivate: Sync Image and Legacy Mode and RTC3 is on SOC1/2 not allowed");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Consumer_Properties);
        return EDD_STS_ERR_PARAM;
    }
    #endif


    //check for change in Buffer properties (can only happen once from invalid->valid)!
    if (EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED == (pConsumer->LowerParams.BufferProperties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
    {
        if (   (EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED != (pRQBParam->IOParams.BufferProperties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
            && (EDDI_RTC3_AUX_CONSUMER != pConsumer->LowerParams.ConsumerType))
        {
            if (!EDDI_CRTCheckBufferProperties(pDDB, pConsumer->LowerParams.ListType, BufferProperties, LSA_FALSE /*bIsProvider*/, LSA_FALSE /*bAllowUndefined*/))
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                    "EDDI_CRTConsumerControlCheckParamsForActivate, illegal BufferProperties:0x%X", BufferProperties);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
                return EDD_STS_ERR_PARAM;
            }
            pConsumer->LowerParams.BufferProperties = pRQBParam->IOParams.BufferProperties;
            if (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (pConsumer->LowerParams.BufferProperties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
            {
                #ifdef EDDI_PRAGMA_MESSAGE
                #pragma message ("MultiEDDI")
                #endif /* EDDI_PRAGMA_MESSAGE */
                #if defined (EDDI_CFG_REV7)
                if (!EDDI_IS_DFP_CONS(pConsumer))
                {
                    //If bufferproperties changed from UNDEFINED to SYNC, deallocate APDUBuffer
                    Status = EDDI_MemFreeApduBufferCons(pConsumer->ConsumerId, pDDB);

                    if (Status != EDD_STS_OK)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                                            "EDDI_CRTConsumerControlCheckParamsForActivate, ERROR EDDI_MemFreeApduBufferCons, Status:0x%X ConsumerID:0x%X", 
                                            Status, pConsumer->ConsumerId);
                        EDDI_Excp("EDDI_CRTConsumerControlCheckParamsForActivate, ERROR EDDI_MemFreeApduBufferCons",
                                    EDDI_FATAL_ERR_EXCP, Status, pConsumer->ConsumerId);
                        return EDD_STS_ERR_EXCP;
                    }
                }
                pConsumer->usingPAEA_Ram = LSA_FALSE;
                #endif //(EDDI_CFG_REV7)

                bBufferPropChangedToSync = LSA_TRUE;
            }
        }
    }

    //check for change in DataOffset (can only happen once from invalid->valid)!
    if ( EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset )
    {
        if (   (EDD_DATAOFFSET_UNDEFINED != pRQBParam->IOParams.DataOffset)
            && (EDDI_RTC3_AUX_CONSUMER != pConsumer->LowerParams.ConsumerType))
        {
            //change dataoffset
            if (!EDDI_IS_DFP_CONS(pConsumer))
            {
                LSA_UINT16  const  KramDatalen = (pConsumer->LowerParams.Partial_DataLen)?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;

                Status = EDDI_CRTCheckOverlaps(pDDB, DataOffset, KramDatalen, pConsumer->LowerParams.ListType, LSA_FALSE, ((LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK)) );
                if (EDD_STS_OK != Status)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerControlCheckParamsForActivate, illegal DataOffset:0x%X Status:0x%X", DataOffset, Status);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
                    return EDD_STS_ERR_PARAM;
                }
            }
            pConsumer->LowerParams.DataOffset = pRQBParam->IOParams.DataOffset;
        }
    }

    //Adapt KRAMBuffer if properties changed
    if (   bBufferPropChangedToSync
        && !EDDI_IS_DFP_CONS(pConsumer))
    {
        if (EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset)
        {
            pConsumer->LowerParams.pKRAMDataBuffer  = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
        }
        else
        {
            pConsumer->LowerParams.pKRAMDataBuffer      = (LSA_UINT8*)((LSA_UINT32)pDDB->pKRam + pConsumer->LowerParams.DataOffset);
            pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
        }
    }  

    //Set CyclePhase if is undefined
    if (EDD_CYCLE_PHASE_UNDEFINED == pConsumer->LowerParams.CyclePhase)
    {
        pConsumer->LowerParams.CyclePhase = pRQBParam->CyclePhase;
    }

    if (pConsumer->IRTtopCtrl.pAscConsumer)
    {
        if (EDD_CYCLE_PHASE_UNDEFINED == pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CyclePhase)
        {
            pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CyclePhase = pRQBParam->CyclePhase;    
        }
    }         

    //copy CyclePhase if is CycleReductionRatio
    if (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pConsumer->LowerParams.CycleReductionRatio)
    {
        pConsumer->LowerParams.CycleReductionRatio = pRQBParam->CycleReductionRatio;
    }

    if (pConsumer->IRTtopCtrl.pAscConsumer)
    {
        if (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CycleReductionRatio)
        {
            pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.CycleReductionRatio = pRQBParam->CycleReductionRatio;    
        }
    } 

    //check dataoffset
    if (   (EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset)
        || (EDD_DATAOFFSET_INVALID == pConsumer->LowerParams.DataOffset))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerControlCheckParamsForActivate, illegal DataOffset:0x%X", 
                          DataOffset);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    {
        #if defined (EDDI_CFG_REV5)
        LSA_UINT32  const  Length = pConsumer->LowerParams.DataOffset + pConsumer->LowerParams.DataLen;  /*lint -e953*/
        #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        LSA_UINT32  Length;
        if (pConsumer->LowerParams.Partial_DataLen != 0)
        {
            Length = pConsumer->LowerParams.DataOffset + pConsumer->LowerParams.Partial_DataLen;
        }
        else
        {
            Length = pConsumer->LowerParams.DataOffset + pConsumer->LowerParams.DataLen;
        }
        #endif

        if ((Length > pDDB->ProcessImage.offset_ProcessImageEnd) && (!EDDI_IS_DFP_CONS(pConsumer)))
        {
            #ifdef EDDI_PRAGMA_MESSAGE
            #pragma message ("MultiEDDI")
            #endif /* EDDI_PRAGMA_MESSAGE */
            #if defined (EDDI_CFG_REV7)
            if (   !pConsumer->usingPAEA_Ram
                || (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK)))
            #endif
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlCheckParamsForActivate, (DataOffset(0x%X)+DataLen(0x%X)) exceeds offset_ProcessImageEnd(0x%X)", 
                                  pConsumer->LowerParams.DataOffset, pConsumer->LowerParams.DataLen, pDDB->ProcessImage.offset_ProcessImageEnd);
                pConsumer->LowerParams.pKRAMDataBuffer      = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
                pConsumer->LowerParams.DataOffset           = EDD_DATAOFFSET_UNDEFINED;
                pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    Status = EDDI_CRTCheckConsumerPhase(pDDB, pConsumer);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlCheckParamsForActivate, illegal EDDI_CRTCheckConsumerPhase, ConsumerID:0x%X", pConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
        return Status;
    }

    if (pConsumer->IRTtopCtrl.pAscConsumer)
    {
        Status = EDDI_CRTCheckConsumerPhase(pDDB, pConsumer->IRTtopCtrl.pAscConsumer);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlCheckParamsForActivate, illegal EDDI_CRTCheckConsumerPhase, ConsumerID:0x%X", pConsumer->ConsumerId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
            return Status;
        }             
    }

    //******************************
    //RTC1/2/3 Step2:
    // check for valid parameters

    //check Mac or IP Address (can be changed on every PASSIVE->ACTIVE transistion
    if (!EDDI_IS_DFP_CONS(pConsumer))
    {
        EDDI_CRTConsumerSetMACIP(pDDB, pConsumer, &pRQBParam->SrcMAC, &pRQBParam->SrcIP);

        Status = EDDI_CRTConsumerCheckMACIP(pDDB, pConsumer, &pConsumer->LowerParams.SrcMAC, (EDD_IP_ADR_TYPE*)(void *)pConsumer->LowerParams.SrcIP.b.IP);
        if (EDD_STS_OK != Status)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerControlCheckParamsForActivate, illegal MAC/IP Adr, Status:0x%X", Status);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MAC_IP);
            Status = EDD_STS_ERR_PARAM;
            return EDD_STS_ERR_PARAM;
        }
    }

    //2. copy parameters to LowerParams
    pConsumer->LowerParams.DataOffset       = DataOffset;
    pConsumer->LowerParams.BufferProperties = BufferProperties;

    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        //Check an Set SFOffset, if necessary
        if (EDD_SFOFFSET_UNDEFINED == pConsumer->LowerParams.SFOffset)
        {
            pConsumer->LowerParams.SFOffset = pRQBParam->IOParams.SFOffset;        
        }
        
        if (   (EDD_SFOFFSET_UNDEFINED == pConsumer->LowerParams.SFOffset)
            || (EDD_SFOFFSET_INVALID   == pConsumer->LowerParams.SFOffset))
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerControlCheckParamsForActivate, illegal DFP SFOffset:0x%X", pConsumer->LowerParams.SFOffset);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
            return EDD_STS_ERR_PARAM;     
        }
        
        //Check an Set SFPosition, if necessary
        if (EDD_SFPOSITION_UNDEFINED == pConsumer->LowerParams.SFPosition)
        {
            pConsumer->LowerParams.SFPosition = pRQBParam->IOParams.SFPosition;        
        }
               
        if (!EDDI_CRT_IS_DFP_CONS_SFPOSITION_IN_VALID_RANGE(pConsumer))
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerControlCheckParamsForActivate, illegal DFP SFPosition:0x%X", pConsumer->LowerParams.SFPosition);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
            return EDD_STS_ERR_PARAM;     
        }  
    }

    //check DFP buffer properties and phase
    //the first activated DFP consumer determines the bufferproperties of the PF (in EDDI_DFPConsActPFFrame)  
    if (EDD_STS_OK != EDDI_DFPCheckBufferProperties(pDDB, pConsumer))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerControlCheckParamsForActivate, illegal DFP BufferProperties:0x%X", BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
        return EDD_STS_ERR_PARAM;
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerActivateRTC123()
 *
 *  Description: Activate a consumer (class1,2,3)
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlActivateRTC123( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer,
                                                                                LSA_UINT8                                 const  ConsumerType,
                                                                                LSA_UINT16                                const  Mode,
                                                                                LSA_BOOL                                * const  pbActivated )
{
    LSA_RESULT                               Status   = EDD_STS_OK;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT     = pDDB->pLocal_CRT;            
    LSA_UINT8                         const  ListType = pConsumer->LowerParams.ListType;

    //RTC1/2/3 Step1b:
    //ACTIVATING a passive consumer

    //Init Score Board
    if (EDDI_CONS_SCOREBOARD_STATE_PASSIVE == pConsumer->ScoreBoardStatus)
    {
        Status = EDDI_SRTWatchDogInit(pDDB, pConsumer); //inits DFP-WD also!
        if(EDD_STS_OK != Status)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, illegal EDDI_SRTWatchDogInit, ConsumerID:0x%X", pConsumer->ConsumerId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerScoreboardInit);
            return Status;
        }
        
        //Init WatchDog for AUX aswell!
        if (pConsumer->IRTtopCtrl.pAscConsumer)
        {
            Status = EDDI_SRTWatchDogInit(pDDB, pConsumer->IRTtopCtrl.pAscConsumer);
            if(EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, illegal EDDI_SRTWatchDogInit, ConsumerID:0x%X", pConsumer->IRTtopCtrl.pAscConsumer);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerScoreboardInit);
                return Status;
            }
        }
    }
    else
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTConsumerControlEvent, SBStatus not PASSIVE. ConsID:0x%X, FrameID:0x%X SBStatus:0x%X", 
                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, pConsumer->ScoreBoardStatus);
        EDDI_Excp("EDDI_CRTConsumerControlEvent, SBStatus not PASSIVE!", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->ScoreBoardStatus);
        return EDD_STS_ERR_PARAM;
    }

    //store transition inactive->active for later
    *pbActivated = LSA_TRUE;

    if (EDDI_RTC3_CONSUMER != ConsumerType)
    {                                                    
        if (EDDI_NULL_PTR == pConsumer->pLowerCtrlACW)
        {
            Status = EDDI_SRTConsumerAddToTree(pDDB, pConsumer);
            if (EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, illegal EDDI_SRTConsumerAddToTree, ConsumerID:0x%X", pConsumer->ConsumerId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AddToTree);
                return Status;
            }
        }
        else
        {
            //change MAC/IP-Adress in existing ACW
            EDDI_CRTConsumerSetMACIPACW(pDDB, pConsumer);
        }
        
        //3. set buffer-data on Class1/2-Consumers
        EDDI_CRTConsumerSetBuffer(pDDB, pConsumer);
    }

    //********************************************
    //RTC3 Step3:
    //Check for existing IRDATA (RTC3 only) and activate consumer
    if (EDDI_RTC3_CONSUMER == ConsumerType)
    {
        Status = EDDI_CRTConsumerControlActivateRTC3(pDDB, pConsumer, Mode);
        if (EDD_STS_OK != Status)
        {
            //Traces and detail have already been set in EDDI_CRTConsumerActivateRTC3 
            return Status;
        }             
    }

    //********************************************
    //RTC1/2 Step3:  RTC3 Step4:
    //Trigger ConsSM and init Scoreboard
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        if (EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE)){} 
    }
    else
    {
        if (pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN)
        {               
            //3. set buffer-data for AUX-Consumer
            if (pConsumer->IRTtopCtrl.pAscConsumer != LSA_NULL)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent-> Activate AUX-Consumer, ConsumerID:0x%X ", pConsumer->IRTtopCtrl.pAscConsumer->ConsumerId);

                if (pConsumer->IRTtopCtrl.pAscConsumer->pLowerCtrlACW == EDDI_NULL_PTR)
                {
                    Status = EDDI_SRTConsumerAddToTree(pDDB, pConsumer);
                
                    if(EDD_STS_OK != Status)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, illegal EDDI_SRTConsumerAddToTree, ConsumerID:0x%X", pConsumer->ConsumerId);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AddToTree);
                        return Status;
                    }
                
                    EDDI_CRTConsumerSetBuffer(pDDB, pConsumer->IRTtopCtrl.pAscConsumer);
                }
                else
                {
                    //change MAC/IP-Adress in existing ACW
                    EDDI_CRTConsumerSetMACIPACW(pDDB, pConsumer->IRTtopCtrl.pAscConsumer);
                }
            
                pConsumer->IRTtopCtrl.pAscConsumer->pSB->OldDataStatus = EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
       
                if (EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer->IRTtopCtrl.pAscConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE)){} // We know, that we did not receive anything yet !
            
                #if defined (EDDI_CFG_USE_SW_RPS)
                pConsumer->IRTtopCtrl.pAscConsumer->pSB->UkCountdown = 0;
                pConsumer->IRTtopCtrl.pAscConsumer->pSB->UkStatus    = EDDI_CRT_CONS_UK_OFF;
                #endif 
            
                EDDI_SERSrtConsumerSetActivity(pConsumer->IRTtopCtrl.pAscConsumer->pLowerCtrlACW, LSA_TRUE);
                #if defined (EDDI_CFG_SMALL_SCF_ON)
                pDDB->CRT.ConsumerList.ActiveConsRTC12++;
                #endif
            }
            else
            {
                EDDI_Excp("EDDI_CRTConsumerControlEvent, pConsumer->IRTtopCtrl.pAscConsumer not valid, pConsumerId:%i, pConsumer ptr: 0x%X", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer);
                return EDD_STS_ERR_EXCP;                
            }
        }
    
        if (pConsumer->IRTtopCtrl.ConsActiveMask & (EDDI_CONS_ACTIVE_CLASS12_MAIN + EDDI_CONS_ACTIVE_RTC3_MAIN) )
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent-> Activate (Main)Consumer, ConsumerID:0x%X ", pConsumer->ConsumerId);
            pConsumer->pSB->OldDataStatus = ((pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS);
        
            if(EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE)){}  // We know, that we didnt receive anything yet !
                 
            pConsumer->pSB->UkStatus    = EDDI_CRT_CONS_UK_OFF;
            #if defined (EDDI_CFG_USE_SW_RPS)
            pConsumer->pSB->UkCountdown = 0;
            #endif
        }

        // 2) Check if TimerScoreboard is already running
        if (EDDI_CRTRpsIsStopped(&pCRT->Rps))
        {
            // Using the HW-SB we have to activate Remote-Provider-Scoreboard here
            // to detect changes in APDU-Status and Connection-Status
            // Start the Scoreboard-HW-Activity:
            EDDI_CRTRpsTrigger(&pCRT->Rps, pDDB, EDDI_CRT_RPS_EVENT_START);
        }

        //********************************************
        //RTC1/2 Step4:
        //Activate consumer
        #ifdef EDDI_PRAGMA_MESSAGE
        #pragma message ("MultiEDDI")
        //if (pConsumer->usingPAEA_Ram) entfernt
        #endif /* EDDI_PRAGMA_MESSAGE */
        if (pConsumer->IRTtopCtrl.ConsActiveMask & (EDDI_CONS_ACTIVE_CLASS12_MAIN + EDDI_CONS_ACTIVE_RTC3_MAIN) )
        {
            // 3) Enable Receiving
            EDDI_CRTDataImage(FCODE_ENABLE_DATA_IMAGE, pConsumer, pDDB);
        }

        if (ListType == EDDI_LIST_TYPE_ACW)
        {
            EDDI_SERSrtConsumerSetActivity(pConsumer->pLowerCtrlACW, LSA_TRUE);
            #if defined (EDDI_CFG_SMALL_SCF_ON)
            pDDB->CRT.ConsumerList.ActiveConsRTC12++;
            #endif
        }
    }

    //********************************************
    //RTC1/2/3 Step5:
    //Set RTC1/2 or RTC3(+AUX, if existing) to Unknown
    //WDT is triggered initially here, and ONLY here! reception has to be enabled before, because the WDT starts counting now!
    if (EDDI_CRTConsumerSetToUnknown(pDDB, pConsumer, EDDI_CRT_CONS_HWWD_LOAD /*bTRigWDT*/, (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN)?LSA_TRUE:LSA_FALSE))
    {
        pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
    }

    //********************************************
    //RTC1/2/3 Step6:
    //Reset all pending events
    if (pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_AUX_MAIN) 
    {
        //Set pSB->Status active
        pConsumer->IRTtopCtrl.pAscConsumer->pSB->Status = EDDI_CRT_CONS_STS_ACTIVE;
        //clear all pending events
        EDDI_CRTConsumerSetPendingEvent(pConsumer->IRTtopCtrl.pAscConsumer, EDDI_CSRT_CONS_EVENT_RESET_EVENT_HANDLING, (LSA_UINT16)0, 0, pDDB);
    }

    if (pConsumer->IRTtopCtrl.ConsActiveMask & (EDDI_CONS_ACTIVE_CLASS12_MAIN + EDDI_CONS_ACTIVE_RTC3_MAIN) )
    {
        //Set pSB->Status active
        pConsumer->pSB->Status = EDDI_CRT_CONS_STS_ACTIVE;
        //clear all pending events
        EDDI_CRTConsumerSetPendingEvent(pConsumer, EDDI_CSRT_CONS_EVENT_RESET_EVENT_HANDLING, (LSA_UINT16)0, 0, pDDB);
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerActivateRTC3()
 *
 *  Description: Activate a consumer (class3)
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlActivateRTC3( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                              EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer,
                                                                              LSA_UINT16                                const  Mode)
{
    LSA_RESULT  Status = EDD_STS_OK;
            
    LSA_UNUSED_ARG(Mode);
    
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        //DFP consumer
        Status = EDDI_DFPConsumerActivate(pDDB, pConsumer);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, SFHandler does not match., ConsumerID:0x%X", pConsumer->ConsumerId);
            //Details have already been set
            return EDD_STS_ERR_PARAM;
        }
    }
    else
    {
        EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

        //"Real" RTC3 consumer
        pFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pConsumer->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
        
        if (!EDDI_DFPConsumerIsPF(pConsumer))
        {
            //Not allowed to activate the DFP-PackFrame by user
            EDDI_DFPCheckFrameHandlerPF( pDDB, pConsumer->ConsumerId, &pFrmHandler);
        }
        
        if (EDDI_NULL_PTR == pFrmHandler)
        {
            if (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) //Legacy IRTTOP-Startup ***
            {
                //no IRDATA present
                EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA);
            }
            else
            {
                //MC-cons cannot be activated without IRDATA being present!
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, MC-cons cannot be activated without IRDATA being present., ConsumerID:0x%X", pConsumer->ConsumerId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                return EDD_STS_ERR_SEQUENCE;
            }
        }
        else
        {
            //IRDATA found and valid!
            Status = EDDI_CRTConsumerCheckData(pConsumer, pFrmHandler, pDDB);
            if (Status != EDD_STS_OK)
            {
                if (   (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) //Legacy IRTTOP-Startup ***
                    || (EDDI_NULL_PTR != pConsumer->IRTtopCtrl.pAscConsumer))
                {
                    //no IRDATA present
                    EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA);
                }
                else
                {
                    //MC-cons cannot be activated without IRDATA being present!
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerControlEvent, MC-cons cannot be activated without IRDATA being present., ConsumerID:0x%X", pConsumer->ConsumerId);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                    return EDD_STS_ERR_PARAM;
                }
            }
            else
            {                    
                EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * const  pFrameData = &pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord->PDIRFrameData;
                                                             
                if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pFrameData->BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
                {
                    LSA_UINT32  const  FrameDataProperties = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD);
                    
                    if (    (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataProperties)
                         && (pConsumer->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) )
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTConsumerControlEvent, Multicast Fast Forwarding is not allowed with Lagacy mode. ConsID:0x%X, FrameID:0x%X, Mode:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, Mode);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                        return EDD_STS_ERR_PARAM;    
                    }
                }                                
                 
                pConsumer->pFrmHandler   = pFrmHandler;
                pFrmHandler->UsedByUpper = LSA_TRUE;  //mark framehandler as used and initialized 
                                  
                EDDI_SERClearTsbEntry(pDDB, pConsumer);

                //3. set buffer-data for RTC3-Consumer
                EDDI_CRTConsumerSetBuffer(pDDB, pConsumer);

                //Add FCW, enable SB, and set L-Bit for local reception
                EDDI_IRTConsumerAdd(pDDB, pFrmHandler, &pConsumer->LowerParams, EDDI_NULL_PTR); 
                #if defined (EDDI_CFG_SMALL_SCF_ON)
                    pDDB->CRT.ConsumerList.ActiveConsRTC3++; //red. consumers count as 1
                #endif

                if (pConsumer->bRedundantFrameID)
                {
                    if (pFrmHandler->pFrmHandlerRedundant) 
                    {
                        if ((pFrmHandler->FrameId + 1) != (pFrmHandler->pFrmHandlerRedundant->FrameId))
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTpConsumerControlEvent, No x+1 MRPD Frame was associated. ConsId:0x%X, FrameID:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerMRPD_NoFrameID);
                            return EDD_STS_ERR_PARAM; 
                        }
                        //Add Redundant FrameHandler with same Framebuffer
                        //Add FCW and set L-Bit for local reception
                        EDDI_IRTConsumerAdd(pDDB, pFrmHandler->pFrmHandlerRedundant, &pConsumer->LowerParams, EDDI_NULL_PTR);
                        pFrmHandler->pFrmHandlerRedundant->UsedByUpper = LSA_TRUE; 
                    }
                    else
                    {
                        if (EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType)
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTpConsumerControlEvent, No x+1 MRPD Frame was associated. ProvId:0x%X, FrameID:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerMRPD_NoFrameID);
                            return EDD_STS_ERR_PARAM; 
                        }
                    }
                }

                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerCtrlEvent, FrameHandler USED, ConsId:0x%X FrameId:0x%X pFrmHandler:0x%X", pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, (LSA_UINT32)pFrmHandler);

                EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA);
            }
        }
    }

    LSA_UNUSED_ARG(pDDB);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerControlSetToUnknown()
 *
 *  Description: Set a consumer to unknown state during ConsumerControl
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerControlSetToUnknown( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            EDDI_CRT_CONSUMER_PTR_TYPE                const  pConsumer)
{
    #ifndef EDDI_CFG_USE_SW_RPS
    LSA_UINT16                           CycleCnt;
    LSA_UINT8                            TransferStatus;
    EDDI_SER_DATA_STATUS_TYPE            DummyDS;
    #endif

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerControlEvent-> SetToUnknown, ConsumerID:0x%X, pConsumer->pSB->Status :0x%X", pConsumer->ConsumerId, pConsumer->pSB->Status);

    if (   (pConsumer->IRTtopCtrl.pAscConsumer)
        && (EDDI_CONS_STATE_WF_IRDATA == pConsumer->IRTtopCtrl.ConsState))
    {
        //Special case: only AUX active
        //set RTC1/2 or RTC3+AUX to Unknown
        //WDT is NEVER triggered (if the consumer has just been activated the WDT had been triggered above!)
        if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->IRTtopCtrl.pAscConsumer->pSB->Status)
        {
            if (EDDI_CRTConsumerSetToUnknown(pDDB, pConsumer, EDDI_CRT_CONS_HWWD_UNTOUCH /*bTrigWDT*/, LSA_TRUE /*bTrigConsSBSM*/))
            {
               pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
            }
        }                                                                                                            
    }
    else if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
    {          
        if  /* Consumer is still active, but has experienced a MISS after activating */
           (EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED == pConsumer->ScoreBoardStatus)
        {
            if (EDDI_RTC3_CONSUMER == pConsumer->LowerParams.ConsumerType && pConsumer->IRTtopCtrl.pAscConsumer) //RTC3Consumer && AUX is active)
            {
                if (EDDI_CONS_SCOREBOARD_STATE_AGAIN == pConsumer->IRTtopCtrl.pAscConsumer->ScoreBoardStatus)
                {
                    //special case: GREEN is running (AGAIN), RED is still MISSING
                    //set RTC3+AUX to Unknown
                    //WDT is NEVER triggered (if the consumer has just been activated the WDT had been triggered above!)
                    if (EDDI_CRTConsumerSetToUnknown(pDDB, pConsumer, EDDI_CRT_CONS_HWWD_UNTOUCH /*bTrigWDT*/, LSA_TRUE /*bTrigConsSBSM*/))
                    {
                       pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
                    }
                }
                else
                {
                    #ifndef EDDI_CFG_USE_SW_RPS
                    EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer, &TransferStatus, &DummyDS, &CycleCnt);
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED, CycleCnt, TransferStatus, pDDB);        
                    #else
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED, 0, 0, pDDB);
                    #endif  

                    //Set event again as PendingEvent could be 0. AGAIN and MISS_NOT_STOPPED have been cleared from EventStatus before.
                    pConsumer->PendingIndEvent |= EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
                    pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
                }
            }
            else
            {
                #ifndef EDDI_CFG_USE_SW_RPS
                EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer, &TransferStatus, &DummyDS, &CycleCnt);
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED, CycleCnt, TransferStatus, pDDB);
                #else
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED, 0, 0, pDDB);
                #endif  

                //Set event again as PendingEvent could be 0. AGAIN and MISS_NOT_STOPPED have been cleared from EventStatus before.
                pConsumer->PendingIndEvent |= EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
                pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
            }
        }
        else
        {    
            //set RTC1/2 or RTC3+AUX to Unknown
            //WDT is NEVER triggered (if the consumer has just been activated the WDT had been triggered above!)
            if (EDDI_CRTConsumerSetToUnknown(pDDB, pConsumer, EDDI_CRT_CONS_HWWD_UNTOUCH /*bTrigWDT*/, LSA_TRUE /*bTrigConsSBSM*/))
            {
               pDDB->CRT.Rps.PendingEvents = LSA_TRUE;
            }
        }
    }
    else if (EDDI_CRT_CONS_STS_INACTIVE == pConsumer->pSB->Status)
    {
        //consumer is PASSIVE (either passivated or by ConsumerAutoStop)

        //Event is MISS (consumer is passive and has seen a MISS before)
        if (pConsumer->bHasSeenMiss)
        {
            #ifndef EDDI_CFG_USE_SW_RPS
            EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer, &TransferStatus, &DummyDS, &CycleCnt);
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, TransferStatus, pDDB);
            #else
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS, 0, 0, pDDB);
            #endif

            //Set event again as PendingEvent could be 0. AGAIN and MISS_NOT_STOPPED have been cleared from EventStatus before.
            pConsumer->PendingIndEvent |= EDD_CSRT_CONS_EVENT_MISS;
        }
        else
        {
            //Event is MISS_NOT_STOPPED (consumer is passive and/or has never sent MISS before)
            //CycleCnt cannot be read, as we don´t know wether a valid KRAMBuffer exists!
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED, 0, 0, pDDB);
            
            //Set event again as PendingEvent could be 0. AGAIN and MISS_NOT_STOPPED have been cleared from EventStatus before.
            pConsumer->PendingIndEvent |= EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
        }

        pDDB->CRT.Rps.PendingEvents = LSA_TRUE;

        //indication will be sent after confirming the current request
    }
    else
    {
        EDDI_Excp("EDDI_CRTConsumerControlEvent, illegal consumer status", EDDI_FATAL_ERR_EXCP, pConsumer->pSB->Status, pConsumer->ConsumerId);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_cons.c                                              */
/*****************************************************************************/

