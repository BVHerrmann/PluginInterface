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
/*  F i l e               &F: eddi_sync_ir.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  25.10.07    JS    added support for secondary Sync-Master                */
/*                    EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B                  */
/*                    will now be used internally for secondary master!      */
/*                    FrameID 0x80, secondary is mapped to                   */
/*                    EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B!                 */
/*  07.11.07    JS    added SyncIr_PortOnlyHasSyncTx                         */
/*  14.11.07    JS    added EDDI_SyncIrCheckPoolResources                    */
/*  17.01.08    JS    dont reset HW-Linedelay on ports not used by forwarder */
/*                    (may be used by other forwarder, if redundancy used)   */
/*  15.02.08    JS    made EDDI_SyncDecodeFrameHandlerType() public          */
/*  20.02.08    JS    added "EDDI_CFG_PDIRDATA_NO_TIME_OVERLAP_CHECK" handlin*/
/*  19.03.08    UL    EDDI_SyncIrInitAllTimeElements: search now goes through*/
/*                    valid reduction-paths only                             */
/*  19.10.09    UL    removed SyncIr_PortOnlyHasSyncTx                       */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ser_ext.h"
#include "eddi_pool.h"

//#include "eddi_sync_check.h"
#include "eddi_sync_ir.h"

#include "eddi_irt_ext.h"
#include "eddi_swi_ext.h"

#include "eddi_ext.h"
#include "eddi_prm_req.h"
#include "eddi_prm_record_pdir_data.h"
//#include "eddi_prm_record_common.h"

#define EDDI_MODULE_ID     M_ID_SYNC_IR
#define LTRC_ACT_MODUL_ID  132

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncIrInitAllTimeElements( EDDI_PRM_RECORD_IRT_PTR_TYPE const pRecordSet,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE      const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncIrAddAllFcws( EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE const pRecordSet,
                                                             EDDI_LOCAL_DDB_PTR_TYPE            const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncIrDeleteAllFcws( EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const pRecordSet,
                                                                EDDI_LOCAL_DDB_PTR_TYPE             const pDDB );

static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrGetIrtSyncRxPort( EDDI_LOCAL_DDB_PTR_TYPE      const pDDB, 
                                                                    LSA_UINT16                   const FrameId,
                                                                    LSA_UINT32                   const iDataElement,
                                                                    LSA_UINT8                        * pRxPort,
                                                                    EDDI_PRM_RECORD_IRT_PTR_TYPE const pRecordSet );

#if defined (EDDI_CFG_ERTEC_400)
static LSA_BOOL EDDI_LOCAL_FCT_ATTR   EDDI_SyncIrIsIrtSyncRxPortGetUsrTXs( EDDI_LOCAL_DDB_PTR_TYPE              const pDDB,
                                                                           LSA_UINT8                            const RxPort,
                                                                           LSA_BOOL                           * const pPortTx,
                                                                           EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE   const pRecordSet );
#endif

static LSA_BOOL EDDI_LOCAL_FCT_ATTR    EDDI_SyncIrInitFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE             const pDDB,
                                                                    EDDI_IRT_FRM_HANDLER_PTR_TYPE             pFrmHandler,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pFrmData );

static LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrUpdateFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE             const pDDB,
                                                                    EDDI_IRT_FRM_HANDLER_PTR_TYPE       const pFrmHandler,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pAddFrmData );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrRegisterPortUser( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     const  pFrmData,
                                                                    EDDI_PRM_RECORD_IRT_PTR_TYPE                   pRecordSet );

static EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_SyncGetNextIrFrameDataElement( EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pFrmData );

#if defined (EDDI_CFG_REV5)
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  SyncIr_DeleteLineDelays( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SyncIrCheckPoolResources()             +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE       const pDDB    +*/
/*+                             EDDI_PRM_RECORD_IRT_PTR_TYPE  const pRecordSet*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+  pRecordSet : Pointer to record set with meta data                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if we have enough pool resources for FCWs for the  +*/
/*+               IRData from PRM. We use Meta-Data collected on PRM-WRITE  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrCheckPoolResources( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_BOOL                                  Result                       = LSA_TRUE;
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  const  pSyncComp                    = &pDDB->SYNC;
    LSA_UINT32                                n_IrtConsumerFcwCntRemaining = 0;

    EDDI_SYNC_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SyncIrCheckPoolResources, PROVIDER FCWs. Meta: %d, Cfg: %d, SYNC-PROVIDER FCWs. Meta: %d, Cfg: %d, CONSUMER/FWD  FCWs. Meta: %d, Cfg: %d, FORWARDER FCWs. Meta: %d, Cfg: %d",
                       pRecordSet->MetaData.n_IrtProviderFcwCnt,pSyncComp->n_IrtProviderFcwCnt,
                       pRecordSet->MetaData.n_IrtSyncSndCnt,pSyncComp->n_IrtSyncSndCnt,
                       pRecordSet->MetaData.n_IrtConsumerFcwCnt,pSyncComp->n_IrtConsumerFcwCnt,
                       pRecordSet->MetaData.n_IrtForwarderFcwCnt,pSyncComp->n_IrtForwarderFcwCnt);

    /* We have 4 pools to be checked */

    /* PROVIDER-Pool */
    if (pRecordSet->MetaData.n_IrtProviderFcwCnt > pSyncComp->n_IrtProviderFcwCnt)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrCheckPoolResources, Error: IRData PROVIDER FCWs exceeded. (%d,%d)", pRecordSet->MetaData.n_IrtProviderFcwCnt, pSyncComp->n_IrtProviderFcwCnt);
        Result = LSA_FALSE;
    }

    /* SYNC-PROVIDER-Pool */
    if (pRecordSet->MetaData.n_IrtSyncSndCnt > pSyncComp->n_IrtSyncSndCnt)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrCheckPoolResources, Error: IRData SYNC-PROVIDER FCWs exceeded. (%d,%d)", pRecordSet->MetaData.n_IrtSyncSndCnt, pSyncComp->n_IrtSyncSndCnt);
        Result = LSA_FALSE;
    }

    /* CONSUMER/FORWARDER-Pool */
    if (pRecordSet->MetaData.n_IrtConsumerFcwCnt > pSyncComp->n_IrtConsumerFcwCnt)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrCheckPoolResources, Error: IRData CONSUMER/FWD FCWs exceeded. (%d,%d)", pRecordSet->MetaData.n_IrtConsumerFcwCnt, pSyncComp->n_IrtConsumerFcwCnt);
        Result = LSA_FALSE;
    }
    else
    {
        n_IrtConsumerFcwCntRemaining = pSyncComp->n_IrtConsumerFcwCnt - pRecordSet->MetaData.n_IrtConsumerFcwCnt;
    }

    /* FORWARDER-Pool */
    /* NOTE: With Forwarded we can also use FCWs from the ConsumerFwd pool! see EDDI_RedTreeIrtAddCw() */
    if (pRecordSet->MetaData.n_IrtForwarderFcwCnt > (pSyncComp->n_IrtForwarderFcwCnt+n_IrtConsumerFcwCntRemaining))
    {
        EDDI_SYNC_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrCheckPoolResources, Error: IRData FORWARDER FCWs exceeded. (%d %d %d %d)",
                           pRecordSet->MetaData.n_IrtForwarderFcwCnt, pSyncComp->n_IrtForwarderFcwCnt, pRecordSet->MetaData.n_IrtForwarderFcwCnt, n_IrtConsumerFcwCntRemaining);
        Result = LSA_FALSE;
    }

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SyncIrFrameHandlerGetRealFrameID()     +*/
/*+  Input/Output          :    LSA_UINT16        FrameID                   +*/
/*+  Result                :    LSA_UINT16        FrameID                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  FrameID    : FrameID from FrameHandler                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Returns the "real" FrameID to be used with FCW from       +*/
/*+               the FrameID used by the FrameHandler.                     +*/
/*+               With RTSync, the secondary master got a dummy FrameID     +*/
/*+               which have to be resubstituted for use within FCW!        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFrameHandlerGetRealFrameID( LSA_UINT16  const  FrameID )
{
    /* is this the Dummy FrameID used by secondary master ? */
    if (FrameID == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B)
    {
        /* if yes substitute it back to the real one */
        return EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3;
    }

    return FrameID;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrActivate()                            */
/*                                                                         */
/* D e s c r i p t i o n: Function is only called if RTC3-Communication    */
/*                        is projected!                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrActivate( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT8                               RxPort = 0;
    LSA_UINT32                              iDataElement, iFrame;
    LSA_UINT32                              UsrPortIndex;
    EDDI_DDB_COMP_SYNC_TYPE       *  const  pSyncComp  = &pDDB->SYNC;
    EDDI_PRM_RECORD_IRT_PTR_TYPE     const  pRecordSet = pDDB->PRM.PDIRData.pRecordSet_A;
    LSA_UINT32                       const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrActivate->");

    if (pSyncComp->IrtActivity)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "EDDI_SyncIrActivate, EDDI_SYNC_STS_RECORD_IN_USE, pSyncComp->IrState:0x%X",
                           pSyncComp->IrtActivity);
        EDDI_Excp("EDDI_SyncIrActivate, pSyncComp->IrtActivity", EDDI_FATAL_ERR_EXCP, pSyncComp->IrtActivity, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (0 == pRecordSet->PDIRDataRecordActLen)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrActivate, PDIRDataRecordActLen:0x%X",
                           pRecordSet->PDIRDataRecordActLen);
        EDDI_Excp("EDDI_SyncIrActivate, PDIRDataRecordActLen == 0", EDDI_FATAL_ERR_EXCP, pRecordSet->PDIRDataRecordActLen, 0);
        return EDD_STS_ERR_EXCP;
    }

    //Disable and Forget RTClass2-Interval
    {
        // delete stored Params
        pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin = 0;
        pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd   = 0;
        pDDB->pLocal_CRT->RTClass2Interval.Status                = EDD_RTCLASS2_NOT_SUPPORTED;
        EDDI_GenSetRTClass2PortStatus(LSA_TRUE, 0, (LSA_UINT8)pDDB->pLocal_CRT->RTClass2Interval.Status, pDDB);
    }

    //Init all Time-Elements
    EDDI_SyncIrInitAllTimeElements(pRecordSet, pDDB);

    //Clear info on last local frame
    for (UsrPortIndex = 0; UsrPortIndex < EDD_CFG_MAX_PORT_CNT; UsrPortIndex++)
    {
        pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsRx         = 0;
        pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsTx         = 0;
        pRecordSet->BeginEndData[UsrPortIndex].LastLocalFrameLenRx  = 0;
        pRecordSet->BeginEndData[UsrPortIndex].LastLocalFrameLenTx  = 0;
    }
    
    //Add all FCWs
    EDDI_SyncIrAddAllFcws(pRecordSet, pDDB);

    //Init Buffered SyncSnd - STMs
    EDDI_SyncSndInitAll(pDDB);

    //Init Buffered SyncRcv - STMs
    EDDI_SyncRcvInitAll(pDDB);

    //Attention: Generation of StartOp and other signals is activated here
    EDDI_SERSetIrtGlobalActivity(LSA_TRUE, pDDB);

    pSyncComp->IrtActivity = LSA_TRUE;

    //Now we know about forwarding-way of Sync-Frame --> Update LineDelays
    for (iFrame = 0; iFrame < 2; iFrame++)
    {
        LSA_UINT16  const  FrameId = (LSA_UINT16)((iFrame == 0)?EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3
                                                               :EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B);

        for (iDataElement = 0; iDataElement < EDDI_SYNC_MAX_REDUNDANT_ELEMENTS; iDataElement++)
        {
            if (EDDI_SyncIrGetIrtSyncRxPort(pDDB, FrameId, iDataElement, &RxPort, pRecordSet))
            {
                EDDI_SyncIrUpdateLineDelay(pDDB, RxPort, pRecordSet);
            }
        }
    }

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (   (pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex])
            || (pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex]))
        {
            EDDI_GenSetIRTPortStatus(LSA_FALSE, HwPortIndex, (LSA_UINT8)EDD_IRT_PORT_INACTIVE, pDDB);
        }
        else
        {
            EDDI_GenSetIRTPortStatus(LSA_FALSE, HwPortIndex, (LSA_UINT8)EDD_IRT_NOT_SUPPORTED, pDDB);
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrDeactivate()                          */
/*                                                                         */
/* D e s c r i p t i o n: Function is only called if RTC3-Communication    */
/*                        is projected!                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrDeactivate( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                              UsrPortIndex;
    EDDI_DDB_COMP_SYNC_TYPE       *  const  pSyncComp  = &pDDB->SYNC;
    EDDI_PRM_RECORD_IRT_PTR_TYPE     const  pRecordSet = pDDB->PRM.PDIRData.pRecordSet_A;
    LSA_UINT32                       const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_BOOL                                PortChanged[EDDI_MAX_IRTE_PORT_CNT+1];  //Lint

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrDeactivate->");

    //EDDI_SYNC_STS_RECORD_IN_USE not used for IRT
    if (!pSyncComp->IrtActivity)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "EDDI_SyncIrDeactivate, EDDI_SYNC_STS_RECORD_IN_USE, pSyncComp->IrtActivity:0x%X",
                           pSyncComp->IrtActivity);
        EDDI_Excp("EDDI_SyncIrDeactivate, pSyncComp->IrtActivity", EDDI_FATAL_ERR_EXCP, pSyncComp->IrtActivity, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (0 == pRecordSet->PDIRDataRecordActLen)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrDeactivate, PDIRDataRecordActLen:0x%X",
                           pRecordSet->PDIRDataRecordActLen);
        EDDI_Excp("EDDI_SyncIrDeactivate, PDIRDataRecordActLen == 0", EDDI_FATAL_ERR_EXCP, pRecordSet->PDIRDataRecordActLen, 0);
        return EDD_STS_ERR_EXCP;
    }

    //Stop Buffered SyncSnd - STMs
    EDDI_SyncSndDeinitAll(pDDB);

    //Stop Buffered SyncRcv - STMs
    EDDI_SyncRcvDeinitAll(pDDB);

    //All providers, consumers and forwarding consumers have to have their FCWs removed
    //Also the SyncMaster and SynSlave have to be removed !!
    if (!EDDI_SyncIrIsUsedByUpperFALSE(pDDB, pRecordSet, LSA_TRUE))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SEQUENCE);
        return EDD_STS_ERR_SEQUENCE;
    }

    #if defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    //IRT is not allowed to be disabled in order to prevent Int_StartOp from being deactivated
    if (pDDB->SWITCH.bResetIRTCtrl)
    #endif
    {
        EDDI_SERSetIrtGlobalActivity(LSA_FALSE, pDDB);
    }

    //Remove all FCWs
    EDDI_SyncIrDeleteAllFcws(pRecordSet, pDDB);

    //Alle FrameHandler entfernen
    EDDI_SyncIrIrRecordCleanUp(pDDB, pRecordSet);

    //IRT-parametrization in Rx/Tx per Port
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex] = 0;
        pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex] = 0;
    }
    
    pRecordSet->MetaData.IrtAllPortUsrTxCnt = 0;
    pRecordSet->MetaData.IrtAllPortUsrRxCnt = 0;
    

    #if defined (EDDI_CFG_REV5)
    //remove old PDSync-RecordSet A
    SyncIr_DeleteLineDelays(pDDB);
    #endif

    pSyncComp->IrtActivity = LSA_FALSE;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_GenSetIRTPortStatus(LSA_FALSE, HwPortIndex, (LSA_UINT8)EDD_IRT_NOT_SUPPORTED, pDDB);
    }

    for (UsrPortIndex = 0; UsrPortIndex < EDDI_MAX_IRTE_PORT_CNT; UsrPortIndex++)
    {
        PortChanged[UsrPortIndex] = (UsrPortIndex < EDD_CFG_MAX_PORT_CNT)?LSA_TRUE:LSA_FALSE;
    }

    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrInitAllTimeElements()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
/* Example for the relation between Reduction.Phase and PathNr:
 *
 *                                  PathNr
 *                                  -------
 *                         --8.1--  path 1
 *                 --4.1--|
 *                |        --8.5--  path 5
 *         --2.1--|
 *        |       |        --8.3--  path 3
 *        |        --4.3--|
 *        |                --8.7--  path 7
 * --1.1--|
 *        |                --8.2--  path 2
 *        |        --4.2--|
 *        |       |        --8.6--  ...
 *         --2.2--|
 *                |        --8.4--
 *                 --4.4--|
 *                         --8.8--
 *
 ***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrInitAllTimeElements( EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                     const  PortMapCnt        = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                            MaxTxFcwSolTimeNs = 0;
    LSA_UINT32                            TxAcwEndTimeNs    = 0;
    EDDI_IRT_REDUCTION_AX_TYPE  *  const  pIrtRed           = &pDDB->CRT.IRTRedAx;

    if (!pRecordSet->bBeginEndDataPresent)
    {
        EDDI_Excp("EDDI_SyncIrInitAllTimeElements, no PDBeginEndData-Block present", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32                              GroupIndex;
        LSA_UINT32                              HwPortIndex;
        EDDI_IRT_BEGIN_END_DATA_TYPE  *  const  pBeginEndData = &pRecordSet->BeginEndData[UsrPortIndex];

        if (!pBeginEndData->bPresent)
        {
            continue;
        }

        if (pBeginEndData->Groups.TxOrangePeriodEndGroupCnt)
        {
            TxAcwEndTimeNs = pBeginEndData->Groups.TxOrangePeriodEnd[0]; //PRM has checked: only one value over all ports allowed!
        }

        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex])
        {
            LSA_UINT32                           CurrentRecordPathIndex;
            EDDI_TREE_TYPE             *  const  pTxTree                  = &pIrtRed->IrtRedTreeTx[HwPortIndex];
            EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine               = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
            LSA_UINT32                    const  TxRedOrangePeriodBeginGroupCnt = pBeginEndData->Groups.TxRedOrangePeriodBeginGroupCnt;
            LSA_UINT32                    const  TxRedPeriodEndGroupCnt   = pBeginEndData->Groups.TxRedPeriodEndGroupCnt;
            LSA_UINT32                    const  NumberOfRecordPhases     = pBeginEndData->NumberOfPhases;

            //calculate TxFcwSolTimeMac10Ns per port
            {
                LSA_UINT32  TxFcwSolTimeMac10Ns = 0;

                #if (EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT != 1)
                #error "EDD only supports EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT = 1"
                //this check is already done by PRM!
                #endif
                for (GroupIndex = 0; GroupIndex < TxRedOrangePeriodBeginGroupCnt; GroupIndex++)
                {
                    LSA_UINT32  const  TxFcwSolTimeNs = pBeginEndData->Groups.TxRedOrangePeriodBegin[GroupIndex];

                    if (TxFcwSolTimeNs > MaxTxFcwSolTimeNs)
                    {
                        MaxTxFcwSolTimeNs = TxFcwSolTimeNs;
                    }

                    if (TxFcwSolTimeNs > EDDI_PREFRAME_SEND_DELAY_100MBIT_NS)
                    {
                        TxFcwSolTimeMac10Ns = EDDI_IRT_CONVERT_FRAME_SEND_OFFSET(TxFcwSolTimeNs) / 10;
                    }
                }

                pTxMachine->TxFcwSolTimeMac10Ns = TxFcwSolTimeMac10Ns;
            }

            //insert EOL-Times into TX-FCW-EOLs
            for (GroupIndex = 0; GroupIndex < TxRedPeriodEndGroupCnt; GroupIndex++)
            {
                LSA_UINT32  const  TxFcwEolTimeNs      = pBeginEndData->Groups.TxRedOrangePeriodEnd[GroupIndex];
                LSA_UINT32  const  TxFcwEolTimeMac10Ns = EDDI_IRT_CONVERT_FRAME_SEND_OFFSET(TxFcwEolTimeNs) / 10;

                if (   (TxFcwEolTimeNs > EDDI_PREFRAME_SEND_DELAY_100MBIT_NS)
                    && (TxFcwEolTimeMac10Ns))
                {
                    EDDI_SERUpdateEOL(pTxTree->pEOLn[GroupIndex], TxFcwEolTimeMac10Ns);
                }
                else
                {
                    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrInitAllTimeElements, final TxFcwEolTime = 0");
                }
            }

            //insert TX-FCW-EOL-Max per port
            {
                EDDI_LOCAL_EOL_PTR_TYPE  const  pEOLMax = pTxTree->pEOLn[pBeginEndData->Groups.TxRedPeriodEndMaxIndex];

                pTxTree->pEOLMax    = pEOLMax;
                pTxMachine->pEOLMax = pEOLMax;
            }

            //insert suitable TX-FCW-EOLs or NULL into all paths
            for (CurrentRecordPathIndex = 0; CurrentRecordPathIndex < NumberOfRecordPhases; CurrentRecordPathIndex++)
            {
                LSA_UINT32               CurrentInternalPathIndex;
                EDDI_LOCAL_EOL_PTR_TYPE  pEOL;

                GroupIndex = pBeginEndData->PhaseAssignment[CurrentRecordPathIndex].Tx.RedPeriodEndGroup;

                if //GroupIndex > 0 (EOL necessary)?
                   (GroupIndex)
                {
                    pEOL = pTxTree->pEOLn[GroupIndex - 1];
                }
                else
                {
                    pEOL = EDDI_NULL_PTR;
                }

                //find all internal subbranches in irt-tree belonging to current path
                for (CurrentInternalPathIndex = CurrentRecordPathIndex + 1; CurrentInternalPathIndex <= pTxTree->MaxReduction; CurrentInternalPathIndex += NumberOfRecordPhases)
                {
                    EDDI_RedTreeIrtInsertTxEol(pDDB, pTxTree, pEOL, (LSA_UINT16)CurrentInternalPathIndex);
                }
            }
        }

        if (pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex])
        {
            EDDI_CRT_PHASE_RX_MACHINE  *  const  pRxMachine               = &pDDB->pLocal_CRT->PhaseRx[HwPortIndex];
            LSA_UINT32                    const  RxRedOrangePeriodBeginGroupCnt = pBeginEndData->Groups.RxRedOrangePeriodBeginGroupCnt;

            //calculate RxFcwSolTimeMac10Ns per port
            {
                LSA_UINT32  RxFcwSolTimeMac10Ns = 0;

                #if (EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT != 1)
                #error "EDD only supports EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT = 1"
                //this check is already done by PRM!
                #endif
                for (GroupIndex = 0; GroupIndex < RxRedOrangePeriodBeginGroupCnt; GroupIndex++)
                {
                    LSA_UINT32  const  RxFcwSolTimeNs = pBeginEndData->Groups.RxRedOrangePeriodBegin[GroupIndex];

                    if (RxFcwSolTimeNs > EDDI_PREFRAME_SEND_DELAY_100MBIT_NS)
                    {
                        RxFcwSolTimeMac10Ns = EDDI_IRT_CONVERT_FRAME_SEND_OFFSET(RxFcwSolTimeNs) / 10;
                    }
                }

                pRxMachine->RxFcwSolTimeMac10Ns = RxFcwSolTimeMac10Ns;
            }
        }
    }

    //store GlobalTxAcwStartTimeMac10Ns over all ports (also check EDDI_IrtClass2StmsTrigger() when changing)
    {
        LSA_UINT32  const  CorrectionBegTimeNs = EDDI_PREFRAME_SEND_DELAY_100MBIT_NS + EDDI_ROOT_ACW_LEAD_TIME_NS;

        if (MaxTxFcwSolTimeNs > CorrectionBegTimeNs)
        {
            pRecordSet->MetaData.GlobalTxAcwStartTimeMac10Ns = (MaxTxFcwSolTimeNs - CorrectionBegTimeNs) / 10;
        }
        else
        {
            pRecordSet->MetaData.GlobalTxAcwStartTimeMac10Ns = 0;
        }
    }

    //store GlobalTxAcwEndTimeMac10Ns over all ports (also check EDDI_IrtClass2StmsTrigger() when changing)
    {
        LSA_UINT32  const  CorrectionEndTimeNs = EDDI_PREFRAME_SEND_DELAY_100MBIT_NS;

        if (TxAcwEndTimeNs > CorrectionEndTimeNs)
        {
            pRecordSet->MetaData.GlobalTxAcwEndTimeMac10Ns = (TxAcwEndTimeNs - CorrectionEndTimeNs) / 10;
        }
        else
        {
            pRecordSet->MetaData.GlobalTxAcwEndTimeMac10Ns = 0;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrGetTxMaxTimeOfAllPorts()              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrGetTxMaxTimeOfAllPorts( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                    EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_UINT32         TxMaxTimeNs = 0;
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    if (pRecordSet == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SyncIrGetTxMaxTimeOfAllPorts, pRecordSet == EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    // Register PortLimits and update PortUserCount for this FrmData
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  TxRedPeriodEndMax = pRecordSet->BeginEndData[UsrPortIndex].Groups.TxRedPeriodEndMax;

        if (TxRedPeriodEndMax > TxMaxTimeNs)
        {
            TxMaxTimeNs = TxRedPeriodEndMax;
        }
    }

    return TxMaxTimeNs;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrDeleteAllFcws()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrDeleteAllFcws( EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrDeleteAllFcws->");

    //No consumer or provider is allowed to be setup
    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    while (!(EDDI_NULL_PTR == pFrmHandler))
    {
        if (EDDI_SYNC_IRT_INVALID_HANDLER == pFrmHandler->HandlerType)
        {
            EDDI_Excp("EDDI_SyncIrDeleteAllFcws, LSA_TRUE == pFrmHandler->UsedByUpper", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (pFrmHandler->UsedByUpper)
        {
            EDDI_Excp("EDDI_SyncIrDeleteAllFcws, LSA_TRUE == pFrmHandler->UsedByUpper", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_IRTFcwRemove(pDDB, pFrmHandler);

        pFrmHandler->UsedByUpper = LSA_FALSE;

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrIsUsedByUpperFALSE()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrIsUsedByUpperFALSE( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                              LSA_BOOL                            const  bCheckSyncUser )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrIsUsedByUpperFALSE->");

    //No consumer or provider is allowed to be setup
    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    while (pFrmHandler)
    {
        if ( ( pFrmHandler->UsedByUpper) &&
             ( (EDDI_SYNC_IRT_PROVIDER     == pFrmHandler->HandlerType) ||
               (EDDI_SYNC_IRT_CONSUMER     == pFrmHandler->HandlerType) ||
               (EDDI_SYNC_IRT_FWD_CONSUMER == pFrmHandler->HandlerType) ) )
        {
            if (   (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
                && (!bCheckSyncUser))
            {
                //bCheckSyncUser == LSA_FALSE
                //--> ignore an active SyncMaster or SyncSlave
            }
            else
            {
                return LSA_FALSE;
            }
        }

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }

    LSA_UNUSED_ARG(pDDB);
    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrDataCheckReductionRatio()             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Description:                                                           */
/*      This function checks if the ReductionRatio is in valid range       */
/*      of given PDIRFrameData entry.                                      */
/*      The FrameSendOffset must be already copied to PDIRData_B.          */
/*                                                                         */
/*      This check is only for IRT-FrameIDs, not for Sync-FrameIDs!        */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrDataCheckReductionRatio(  EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                    EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet)
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;
    LSA_UINT32  iElem;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrDataCheckReductionRatio->");

    //No consumer or provider is allowed to be setup
    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    while (pFrmHandler)
    {
        if ( ( (EDDI_SYNC_IRT_PROVIDER     == pFrmHandler->HandlerType) ||
               (EDDI_SYNC_IRT_CONSUMER     == pFrmHandler->HandlerType) ||
               (EDDI_SYNC_IRT_FWD_CONSUMER == pFrmHandler->HandlerType) ) )
        {
            for (iElem = 0; iElem < pDDB->PM.PortMap.PortCnt; iElem++)
            {
                if (!(EDDI_NULL_PTR == pFrmHandler->pIrFrameDataElem[iElem]))
                {
                    if (pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio != 1)
                    {
                        EDDI_SYNC_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, 
                            "EDDI_SyncIrDataCheckReductionRatio: FrameId: 0x%X, Port: %d, RR: %d,", 
                            pFrmHandler->FrameId,
                            iElem,
                            pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio);
                        return LSA_FALSE;
                    }
                }
            }
        }

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif // defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)


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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrAddAllFcws( EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                              EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrAddAllFcws->");

    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    while (!(EDDI_NULL_PTR == pFrmHandler))
    {
        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pFrmHandler->pIrFrameDataElem[0];

        if (EDDI_SYNC_IRT_INVALID_HANDLER == pFrmHandler->HandlerType)
        {
            EDDI_Excp("EDDI_SyncIrAddAllFcws: EDDI_SYNC_IRT_INVALID_HANDLER == pFrmHandler->HandlerType", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (EDDI_NULL_PTR == pFrmData)
        {
            EDDI_Excp("EDDI_SyncIrAddAllFcws: EDDI_NULL_PTR, pIrFrameDataElem[0]", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_IRTFcwAdd(pDDB, pFrmHandler);

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrInitFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                    EDDI_IRT_FRM_HANDLER_PTR_TYPE               pFrmHandler,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData )
{
    LSA_UINT32                      CommonPortIndex;
    EDDI_SYNC_IRT_FRM_HANDLER_ENUM  HandlerType;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "PRM_EDDI_SyncIrInitFrameHandler->");

    EDDI_MemSet(pFrmHandler, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_IRT_FRM_HANDLER_TYPE));

    HandlerType = EDDI_SyncDecodeFrameHandlerType(pDDB, pFrmData);

    if (EDDI_SYNC_IRT_INVALID_HANDLER == HandlerType)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrInitFrameHandler, Invalid HandlerType, RxPort:0x%X TxPortGroupArray[0]:0x%X",
                           pFrmData->UsrRxPort_0_4, pFrmData->UsrTxPortGroupArray[0]);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RxTxPortSettings);
        return LSA_FALSE;
    }

    pFrmHandler->QueueLink.next_ptr = EDDI_NULL_PTR;
    pFrmHandler->QueueLink.prev_ptr = EDDI_NULL_PTR;

    for (CommonPortIndex = 0; CommonPortIndex < EDD_CFG_MAX_PORT_CNT; CommonPortIndex++)
    {
        pFrmHandler->pIRTSndEl[CommonPortIndex]        = (EDDI_TREE_ELEM_PTR_TYPE)0;
        pFrmHandler->pIRTRcvEl[CommonPortIndex]        = (EDDI_TREE_ELEM_PTR_TYPE)0;
        pFrmHandler->pIrFrameDataElem[CommonPortIndex] = EDDI_NULL_PTR;
    }

    pFrmHandler->HandlerType          = HandlerType;
    pFrmHandler->UsedByUpper          = LSA_FALSE;

    /* Note: With RTSync the FrameID of the secondary Sync master was substituted to */
    /*       EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B !                                 */

    pFrmHandler->FrameId              = pFrmData->FrameID;

    pFrmHandler->pIrFrameDataElem[0]  = pFrmData;

    return LSA_TRUE;
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrUpdateFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                      EDDI_IRT_FRM_HANDLER_PTR_TYPE           const  pFrmHandler,
                                                                      EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     const  pAddFrmData )
{
    LSA_UINT32                                  DataIndex, FreeDataIndex;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pFirstFrmData;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pCurrentFrmData;
    EDDI_SYNC_IRT_FRM_HANDLER_ENUM              newHandlerType;
    LSA_UINT32                                  MaxDataIndex;
    LSA_UINT32                           const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "PRM_EDDI_SyncIrUpdateFrameHandler->");

    if (!pFrmHandler)
    {
        EDDI_Excp("PRM_EDDI_SyncIrUpdateFrameHandler, pFrmHandler == NULL ; - , -", EDDI_FATAL_ERR_EXCP, 0, 0);
        return LSA_FALSE;
    }

    if (!pFrmHandler->pIrFrameDataElem[0])
    {
        EDDI_Excp("PRM_EDDI_SyncIrUpdateFrameHandler, pIrFrameDataElem[0] == NULL ; FrameId, -", EDDI_FATAL_ERR_EXCP, pFrmHandler->FrameId, 0);
        return LSA_FALSE;
    }

    // Only allowed for IRTProvider and IRTConsumer
    if (   (EDDI_SYNC_IRT_PROVIDER     != pFrmHandler->HandlerType)
           && (EDDI_SYNC_IRT_CONSUMER     != pFrmHandler->HandlerType)
           && (EDDI_SYNC_IRT_FWD_CONSUMER != pFrmHandler->HandlerType))
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrUpdateFrameHandler, Unexpected HandlerType:0x%X",
                           pFrmHandler->HandlerType);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RxTxPortSettings);
        return LSA_FALSE;
    }

    // Check Type of pFrmData is of same HandlerType
    newHandlerType = EDDI_SyncDecodeFrameHandlerType(pDDB, pAddFrmData);

    if (   (pFrmHandler->HandlerType == EDDI_SYNC_IRT_CONSUMER)
        && (newHandlerType           == EDDI_SYNC_IRT_FWD_CONSUMER))
    {
        // Handle special CONSUMER - FWD_CONSUMER mixture  --> FWD_CONSUMER wins
        // May exist for mediaredundant consumers
        // Update HandlerType !
        pFrmHandler->HandlerType = EDDI_SYNC_IRT_FWD_CONSUMER;
    }

    if (pFrmHandler->HandlerType != newHandlerType)
    {
        if (   (pFrmHandler->HandlerType == EDDI_SYNC_IRT_FWD_CONSUMER)
            && (newHandlerType           == EDDI_SYNC_IRT_CONSUMER))
        {
            // this is the only allowed mixture
            // nothing todo
        }
        else
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                               "PRM_EDDI_SyncIrUpdateFrameHandler, Mismatch HandlerType, newHandlerType:0x%X Expected:0x%X",
                               newHandlerType, pFrmHandler->HandlerType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RxTxPortSettings);
            return LSA_FALSE;
        }
    }

    // Maximum of Allowed Dataelements depends on Handlertype
    // Provider:    up to pDDB->PM.PortMap.PortCnt elements
    // Consumer/Fwd
    if (pFrmHandler->HandlerType == EDDI_SYNC_IRT_PROVIDER)
    {
        MaxDataIndex = PortMapCnt;
    }
    else
    {
        MaxDataIndex = EDDI_MIN(PortMapCnt, EDDI_SYNC_MAX_REDUNDANT_ELEMENTS);
    }

    // Search for an emtpy FrmDataSlot for pAddFrmData"
    // Start with Index 1

    DataIndex     = 1;
    FreeDataIndex = 0;

    for (;;)
    {
        if (DataIndex >= MaxDataIndex)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                               "PRM_EDDI_SyncIrUpdateFrameHandler, Too many FrmData with same FrameId, FrameId:0x%X DataIndex:0x%X",
                               pFrmHandler->FrameId, DataIndex);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RxTxPortSettings);
            return LSA_FALSE;
        }

        if (EDDI_NULL_PTR== pFrmHandler->pIrFrameDataElem[DataIndex])
        {
            // OK Free space found.
            FreeDataIndex = DataIndex;
            break;
        }

        DataIndex++;
    }

    // Compare AddFrmData-members with first FrameData
    pFirstFrmData = pFrmHandler->pIrFrameDataElem[0];

    if (pFirstFrmData->DataLength != pAddFrmData->DataLength)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrUpdateFrameHandler, DataLength Mismatch, FrameId:0x%X",
                           pFrmHandler->FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchDatalength);
        return LSA_FALSE;
    }

    if (pFirstFrmData->Ethertype != pAddFrmData->Ethertype)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrUpdateFrameHandler, Ethertype Mismatch, FrameId:0x%X",
                           pFrmHandler->FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchEthertype);
        return LSA_FALSE;
    }

    if (pFirstFrmData->ReductionRatio != pAddFrmData->ReductionRatio)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrUpdateFrameHandler, ReductionRatio Mismatch, FrameId:0x%X",
                           pFrmHandler->FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchReductionRatio);
        return LSA_FALSE;
    }

    if (pFirstFrmData->Phase != pAddFrmData->Phase)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "PRM_EDDI_SyncIrUpdateFrameHandler, Phase Mismatch, FrameId:0x%X",
                           pFrmHandler->FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchPhase);
        return LSA_FALSE;
    }

    // Check overlapping with so far used external TxPorts and external RxPorts
    for (DataIndex = 0; DataIndex < PortMapCnt; DataIndex++)
    {
        pCurrentFrmData = pFrmHandler->pIrFrameDataElem[DataIndex];
        if (pCurrentFrmData)
        {
            // Check if we have Overlapping in external RxPorts
            if ((pAddFrmData->UsrRxPort_0_4 != 0) &&  (pAddFrmData->UsrRxPort_0_4 == pCurrentFrmData->UsrRxPort_0_4))
            {
                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                   "PRM_EDDI_SyncIrUpdateFrameHandler, External RxPort overlaps, FrameId:0x%X DataIndex:0x%X",
                                   pFrmHandler->FrameId, DataIndex);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchExtRxPorts);
                return LSA_FALSE;
            }

            // Check if we have bitwise overlapping in external TxPorts
            if ((pAddFrmData->UsrTxPortGroupArray[0] & pCurrentFrmData->UsrTxPortGroupArray[0]) & EDDI_SYNC_TXPORTS_EXTERNAL)
            {
                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                   "PRM_EDDI_SyncIrUpdateFrameHandler, External TxPorts overlaps, FrameId:0x%X DataIndex:0x%X",
                                   pFrmHandler->FrameId, DataIndex);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MismatchExtTxPorts);
                return LSA_FALSE;
            }
        }
    }

    pFrmHandler->pIrFrameDataElem[FreeDataIndex] = pAddFrmData;

    return LSA_TRUE;
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
EDDI_SYNC_IRT_FRM_HANDLER_ENUM  EDDI_LOCAL_FCT_ATTR  EDDI_SyncDecodeFrameHandlerType( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                      EDDI_PRM_RECORD_FRAME_DATA_TYPE  const * const  pFrmData )
{
    LSA_BOOL   const  bRxPortLocal = (LSA_BOOL)((pFrmData->UsrRxPort_0_4 == 0)  ? LSA_TRUE : LSA_FALSE);
    LSA_UINT8  const  TxPortLocal  = (LSA_UINT8)(pFrmData->UsrTxPortGroupArray[0] & EDDI_SYNC_TXPORT_LOCAL);
    LSA_UINT8  const  TxPortsExt   = (LSA_UINT8)(pFrmData->UsrTxPortGroupArray[0] & EDDI_SYNC_TXPORTS_EXTERNAL);

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncDecodeFrameHandlerType->");

    //Calculate HandlerType (vgl. Decodierhilfe in EDDI_ERTEC_SYNC-Schnittstellenbeschreibung v0.2:
    if (bRxPortLocal && !TxPortLocal && TxPortsExt)
    {
        return EDDI_SYNC_IRT_PROVIDER;
    }

    if (!bRxPortLocal && TxPortLocal && !TxPortsExt)
    {
        return EDDI_SYNC_IRT_CONSUMER;
    }

    if (!bRxPortLocal && TxPortLocal && TxPortsExt)
    {
        return EDDI_SYNC_IRT_FWD_CONSUMER;
    }

    if (!bRxPortLocal && !TxPortLocal && TxPortsExt)
    {
        return EDDI_SYNC_IRT_FORWARDER;
    }

    return EDDI_SYNC_IRT_INVALID_HANDLER;
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
EDDI_IRT_FRM_HANDLER_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                LSA_UINT16                          const  FrameId,
                                                                                EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrFindFrameHandler->");

    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *) pRecordSet->FrameHandlerQueue.pFirst;

    while (pFrmHandler)
    {
        /* Note: The FrameID within Framehandler for secondary RTSync master is substituted */
        /*       with EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B!                                */

        if (pFrmHandler->FrameId == FrameId)
        {
            return pFrmHandler;
        }

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *) pFrmHandler->QueueLink.next_ptr;
    }

    LSA_UNUSED_ARG(pDDB);
    return LSA_NULL;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:         EDDI_SyncIrFindFrameHandlerForSyncConsumer()
*
* D e s c r i p t i o n :  Searches for FrameHandler for Sync Consumer
*
* A r g u m e n t s:       Primary: LSA_TRUE: find primary master handler
*                                   LSA_FALSE:find secondary master handler
*
* Return Value:            Pointer to Framehandler
*
***************************************************************************/
EDDI_IRT_FRM_HANDLER_TYPE *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandlerForSyncConsumer( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                              LSA_BOOL                            const  Primary,
                                                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    EDDI_IRT_FRM_HANDLER_TYPE  *  pFrmHandler;
    LSA_UINT16                    FrameID;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrFindFrameHandler->");

    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    /* Within FrameHandler we use the dummy-FrameID EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B */
    /* with Secondary Syncmaster.                                                         */

    if (Primary)
    {
        FrameID = EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3;
    }
    else
    {
        FrameID = EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B;
    }

    while (pFrmHandler)
    {
        if (pFrmHandler->FrameId == FrameID)
        {
            if (   (pFrmHandler->HandlerType == EDDI_SYNC_IRT_FWD_CONSUMER)
                   || (pFrmHandler->HandlerType == EDDI_SYNC_IRT_CONSUMER))
            {
                // OK Sync-Receiver found
                return pFrmHandler;
            }
            else
            {
                // no match
            }
        }
        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *) pFrmHandler->QueueLink.next_ptr;
    }

    LSA_UNUSED_ARG(pDDB);
    return LSA_NULL;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:         EDDI_SyncIrFindFrameHandlerForSyncProvider()
*
* D e s c r i p t i o n :  Searches for FrameHandler for a Sync Provider
*                          (primary or secondary)
* A r g u m e n t s:
*
* Return Value:            Pointer to Framehandler
*
***************************************************************************/
EDDI_IRT_FRM_HANDLER_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandlerForSyncProvider( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                               EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrFindFrameHandlerForSyncProvider->");

    pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;

    while (pFrmHandler)
    {
        /* Within FrameHandler we use the dummy-FrameID EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B */
        /* with Secondary Syncmaster.                                                         */
        /* We search till first Provider we found. There should only be one provider present! */

        if (   (pFrmHandler->FrameId == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
            || (pFrmHandler->FrameId == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B))
        {
            if (pFrmHandler->HandlerType == EDDI_SYNC_IRT_PROVIDER)
            {
                // OK Sync-Sender found
                return pFrmHandler;
            }
            else
            {
                // no match
            }
        }
        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncIrFindFrameHandlerForSyncProvider, No Provider found");

   LSA_UNUSED_ARG(pDDB);
   return EDDI_NULL_PTR;
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrGetIrtSyncRxPort( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB, 
                                                                    LSA_UINT16                    const  FrameId,
                                                                    LSA_UINT32                    const  iDataElement,
                                                                    LSA_UINT8                         *  pRxPort,
                                                                    EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrGetIrtSyncRxPort->");

    //GetSyncFrame
    pFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, FrameId, pRecordSet);

    if (!pFrmHandler)
    {
        return LSA_FALSE;
    }

    //Search only for *forwarding* SyncSlave
    if (pFrmHandler->HandlerType != EDDI_SYNC_IRT_FWD_CONSUMER)
    {
        return LSA_FALSE;
    }

    if (!pFrmHandler->pIrFrameDataElem[iDataElement])
    {
        return LSA_FALSE;
    }

    *pRxPort = pFrmHandler->pIrFrameDataElem[iDataElement]->UsrRxPort_0_4;

    return LSA_TRUE;
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
#if defined (EDDI_CFG_ERTEC_400)
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrIsIrtSyncRxPortGetUsrTXs( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                            LSA_UINT8                              const  RxPort,
                                                                            LSA_BOOL                            *  const  pUsrPortTx,
                                                                            EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE     const  pRecordSet )
{
    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE         pFrmHandler;
    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE         pFrmHandlerB;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pDataElem,pDataElemB;
    LSA_UINT32                                  UsrPortIndex, TxPortBit, iElem;
    LSA_BOOL                                    Response;
    LSA_UINT32                           const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SyncIr_GetIrtSyncTxPorts->");

    //GetSyncFrame Handler
    pFrmHandler  = EDDI_SyncIrFindFrameHandlerForSyncConsumer(pDDB, LSA_TRUE,   // Primary Master
                                                              pRecordSet);

    pFrmHandlerB = EDDI_SyncIrFindFrameHandlerForSyncConsumer(pDDB, LSA_FALSE,  // Secondary Master
                                                              pRecordSet);

    pDataElem  = LSA_NULL;
    pDataElemB = LSA_NULL;
    Response   = LSA_TRUE;

    // find Framehandler for this RxPort for Primary
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pFrmHandler,LSA_NULL) &&
         ((pFrmHandler->HandlerType == EDDI_SYNC_IRT_FORWARDER) ||
          (pFrmHandler->HandlerType == EDDI_SYNC_IRT_FWD_CONSUMER)))
    {
        for (iElem = 0; iElem < EDDI_SYNC_MAX_REDUNDANT_ELEMENTS; iElem++)
        {
            if ((pFrmHandler->pIrFrameDataElem[iElem] != EDDI_NULL_PTR) &&
                (pFrmHandler->pIrFrameDataElem[iElem]->UsrRxPort_0_4 == RxPort) &&
                (pFrmHandler->pIrFrameDataElem[iElem]->UsrTxPortGroupArray[0] & EDDI_SYNC_TXPORTS_EXTERNAL))
            {
                pDataElem = pFrmHandler->pIrFrameDataElem[iElem];
                break;
            }
        }
    }

    // find Framehandler for this RxPort for Secondary
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pFrmHandlerB,LSA_NULL) &&
         ((pFrmHandlerB->HandlerType == EDDI_SYNC_IRT_FORWARDER) ||
          (pFrmHandlerB->HandlerType == EDDI_SYNC_IRT_FWD_CONSUMER)))
    {
        pDataElemB  = EDDI_NULL_PTR;
        for (iElem = 0; iElem < EDDI_SYNC_MAX_REDUNDANT_ELEMENTS; iElem++)
        {
            if ((pFrmHandlerB->pIrFrameDataElem[iElem] != EDDI_NULL_PTR) &&
                (pFrmHandlerB->pIrFrameDataElem[iElem]->UsrRxPort_0_4 == RxPort) &&
                (pFrmHandlerB->pIrFrameDataElem[iElem]->UsrTxPortGroupArray[0] & EDDI_SYNC_TXPORTS_EXTERNAL))
            {
                pDataElemB = pFrmHandlerB->pIrFrameDataElem[iElem];
                break;
            }
        }
    }

    /* We may have two Forwarder for this Port (Primary and optional secondary */
    /* sync master). We use both and combine the destination ports.            */
    /* NOTE: We cant detect here if we have overlapping destination ports from */
    /*       other source ports! This have to be checked before!               */
    /*       (e.g. on Prm-consistency-check).                                  */

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDataElem ,LSA_NULL) ||
         ! LSA_HOST_PTR_ARE_EQUAL(pDataElemB,LSA_NULL))  /* at least one exist */
    {
        /* if one pointer doesnt exist set to other one.. simplyfies loop below */
        if (LSA_HOST_PTR_ARE_EQUAL(pDataElem,LSA_NULL))
        {
            pDataElem  = pDataElemB;
        }
        if (LSA_HOST_PTR_ARE_EQUAL(pDataElemB,LSA_NULL))
        {
            pDataElemB = pDataElem;
        }

        // Now get all related Destination-Ports
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            TxPortBit = 2 << UsrPortIndex; // start with Bit1 (Bit0 is NOT relevant -> it only indicates local Port)

            if (pDataElem && pDataElemB)
            {
                if (   (TxPortBit & pDataElem->UsrTxPortGroupArray[0])
                       || (TxPortBit & pDataElemB->UsrTxPortGroupArray[0]))
                {
                    pUsrPortTx[UsrPortIndex] = LSA_TRUE;
                }
                else
                {
                    pUsrPortTx[UsrPortIndex] = LSA_FALSE;
                }
            }
        }
    }
    else
    {
        Response = LSA_FALSE;
    }

    return Response;
}
#endif
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrUpdateLineDelay( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                           LSA_UINT8                           const  UsrRxPort_0_4,
                                                           EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_UINT32  const  HardwareRxPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, UsrRxPort_0_4);
    LSA_UINT32  const  PortMapCnt          = pDDB->PM.PortMap.PortCnt;
    #if defined (EDDI_CFG_ERTEC_400)
    LSA_UINT32         UsrPortTxIndex;
    LSA_BOOL           UsrPortTX[EDD_CFG_MAX_PORT_CNT];
    #endif

    #if !defined (EDDI_CFG_ERTEC_400)
    LSA_UNUSED_ARG(pRecordSet); //satisfy lint!
    #endif

    if ((UsrRxPort_0_4 == 0) || (UsrRxPort_0_4 > PortMapCnt))
    {
        EDDI_Excp("EDDI_SyncIrUpdateLineDelay, RxPort < 0) || (RxPort > pDDB->PM.PortMap.PortCnt",
                  EDDI_FATAL_ERR_EXCP, UsrRxPort_0_4, PortMapCnt);
        return;
    }

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrUpdateLineDelay, UsrRxPort_0_4:0x%X", UsrRxPort_0_4);

    #if defined (EDDI_CFG_ERTEC_400)
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+  ERTEC400 Linedelay - Work-Around
    //+
    //+  ERTERC400 does not support linedelay-adding on Rcv-Side (only Send-Side is supported)
    //+  --> For forwarding Syncframes we need to "mirror" the Linedelay.
    //+      1) Get the RxPortId used in SyncFrame.
    //+      2) Get the Linedelay meassured on Rx(!)PortId.
    //+      3) Write this Linedelay to all Tx(!)PortIds used by the SyncForwarder
    //+
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (EDDI_SyncIrIsIrtSyncRxPortGetUsrTXs(pDDB, UsrRxPort_0_4, &UsrPortTX[0], pRecordSet))
    {
        //Write the LineDelay values for all TX-Ports 
        for (UsrPortTxIndex = 0; UsrPortTxIndex < PortMapCnt; UsrPortTxIndex++)
        {
            LSA_UINT32  const  HwPortTxIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortTxIndex);

            if (UsrPortTX[UsrPortTxIndex])
            {
                if (pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksMeasured !=
                    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex ].LineDelayInTicksHardware)
                {
                    //store LineDelay
                    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex ].LineDelayInTicksHardware =
                        pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksMeasured;

                    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrUpdateLineDelay, HwPortTxIndex:0x%X LineDelayInTicksHardware:0x%X",
                                       HwPortTxIndex, pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex].LineDelayInTicksHardware);

                    // LineDelay is delivered in ticks
                    EDDI_SwiMiscSetRegister(HwPortTxIndex, REG_LINE_DELAY,
                                            pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex].LineDelayInTicksHardware, pDDB);
                }
            }
            #if defined (EDDI_OLD_HWLD_HANDLING)
            /* Removed.                                                                   */
            /* Dont set Ports to 0 because with redundancy this port may be a destination */
            /* Port of the redundant way and must not be set to 0!                        */
            /* The Register is set to 0 within EDDI_SyncIrDeactivate and within startup.  */
            else
            {
                if (0UL != pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex].LineDelayInTicksHardware)
                {
                    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex].LineDelayInTicksHardware = 0UL;

                    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncIrUpdateLineDelay, HwPortTxIndex:0x%X LineDelayInTicksHardware:0x%X",
                                       HwPortTxIndex, pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortTxIndex].LineDelayInTicksHardware);

                    //Port wird nicht zur Syncweiterleitung benutzt --> Linedelay = 0 !
                    EDDI_SwiMiscSetRegister(HwPortTxIndex, REG_LINE_DELAY, 0UL, pDDB);
                }
            }
            #endif
        }
    }
    #else
    if (pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksHardware !=
        pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksMeasured)
    {
        //store LineDelay
        pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksHardware =
            pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksMeasured;

        // LineDelay is delivered in ticks
        EDDI_SwiMiscSetRegister(HardwareRxPortIndex , REG_LINE_DELAY,
                                pDDB->pLocal_SWITCH->LinkIndExtPara[HardwareRxPortIndex].LineDelayInTicksHardware, pDDB);
    }
    #endif
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrIrRecordCleanUp( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                           EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_UINT32                              UsrPortIndex;
    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE     pFrmHandler;
    LSA_UINT32                       const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrIrRecordCleanUp->");

    // Remove all Framehandlers
    for (;;)
    {
        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)EDDI_QueueRemoveFromEnd((&pRecordSet->FrameHandlerQueue));

        if (EDDI_NULL_PTR == pFrmHandler)
        {
            // OK List is empty
            break;
        }

        if (pFrmHandler->UsedByUpper)
        {
            EDDI_Excp("EDDI_SyncIrIrRecordCleanUp: pFrmHandler->UsedByUpper == TRUE; FrameId", EDDI_FATAL_ERR_EXCP, pFrmHandler->FrameId, 0);
            return;
        }

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            if (pFrmHandler->pIRTRcvEl[HwPortIndex])
            {
                EDDI_Excp("EDDI_SyncIrIrRecordCleanUp: pFrmHandler->pIRTRcvEl HwPortIndex", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
                return;
            }
        }

        // Free  in Pool
        EDDI_MemFreePoolBuffer(pRecordSet->hFrameHandler, (void *)pFrmHandler);
    }

    pRecordSet->FrameHandlerQueue.Count  = 0;
    pRecordSet->FrameHandlerQueue.pFirst = EDDI_NULL_PTR;
    pRecordSet->FrameHandlerQueue.pLast  = EDDI_NULL_PTR;

    // RecordBuffer Reset
    pRecordSet->PDIRDataRecordActLen = 0;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        // Reset Counter of Port-User
        pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex] = 0;
        pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex] = 0;

        pRecordSet->MetaData.IrtPortSyncTxCnt[HwPortIndex] = 0;
        pRecordSet->MetaData.IrtPortSyncRxCnt[HwPortIndex] = 0;
    }
    pDDB->SYNC.bSyncInRedActive = LSA_FALSE;
    
    pRecordSet->MetaData.IrtAllPortUsrTxCnt = 0;
    pRecordSet->MetaData.IrtAllPortUsrRxCnt = 0;

    // Reset Counters
    pRecordSet->MetaData.n_IrtSyncSndCnt      = 0;
    pRecordSet->MetaData.n_IrtProviderFcwCnt  = 0;
    pRecordSet->MetaData.n_IrtConsumerFcwCnt  = 0;
    pRecordSet->MetaData.n_IrtForwarderFcwCnt = 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrRegisterPortUser()                    */
/*                                                                         */
/* D e s c r i p t i o n: Register the TimeSlice occupied by pFrmHandler   */
/*                        So, the minium / maximum IRT-Time expansion      */
/*                        of each port and direction will be calculated.   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrRegisterPortUser( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     const  pFrmData,
                                                                    EDDI_PRM_RECORD_IRT_PTR_TYPE                   pRecordSet )
{
    LSA_UINT32                             UsrPortIndex;
    LSA_UINT32                      const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_SYNC_IRT_FRM_HANDLER_ENUM  const  HandlerType = EDDI_SyncDecodeFrameHandlerType(pDDB, pFrmData);
    LSA_UINT16                      const  FrameId = pFrmData->FrameID;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrRegisterPortUser->");

    // only PROVIDER, FORWARDER and FWD_CONSUMER influence Max/MinTimes
    switch (HandlerType)
    {
        case EDDI_SYNC_IRT_PROVIDER:
        {
            /* Update MetaData for Provider. For every Port we shall send we need a FCW */
            for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
            {
                // UsrPortIndex is used on purpose -> we compare pFrmData to the user-sight (S7-SDB)
                if (EDDI_IRTIsTxPortSet(pFrmData, PortMapCnt, UsrPortIndex))
                {
                    if (EDDI_SyncIrFrameHandlerGetRealFrameID(FrameId) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
                    {
                        pRecordSet->MetaData.n_IrtSyncSndCnt++;      /* SYNC-PROVIDER */
                    }
                    else
                    {
                        pRecordSet->MetaData.n_IrtProviderFcwCnt++;  /* PROVIDER      */
                    }
                }
            }

            break;
        }

        case EDDI_SYNC_IRT_CONSUMER:
        {
            pRecordSet->MetaData.n_IrtConsumerFcwCnt++;  /* CONSUMER or CONSUMER with FORWARDER */

            break;
        }

        case EDDI_SYNC_IRT_FWD_CONSUMER:
            pRecordSet->MetaData.n_IrtConsumerFcwCnt++;  /* CONSUMER or CONSUMER with FORWARDER */
            //lint -fallthrough

        case EDDI_SYNC_IRT_FORWARDER:
        {
            if (HandlerType == EDDI_SYNC_IRT_FORWARDER)
            {
                pRecordSet->MetaData.n_IrtForwarderFcwCnt++;  /* only FORWARDER, no CONSUMER */
            }

            break;
        }

        case EDDI_SYNC_IRT_INVALID_HANDLER:
        default:
        {
            EDDI_Excp("EDDI_SyncIrRegisterPortUser, Unsupported FrameHandlerType", EDDI_FATAL_ERR_EXCP, HandlerType, 0);
            return;
        }
    }

    //Register PortLimits and update PortUserCount for this FrmData
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //UsrPortIndex ist Absicht -> Vergleich auf User Sicht in pFrmData
        if (EDDI_IRTIsTxPortSet(pFrmData, PortMapCnt, UsrPortIndex))
        {
            if(0 == pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex])
            {
                // first increment for this port ==> inc a new tx port user
                pRecordSet->MetaData.IrtAllPortUsrTxCnt++;
            }
            pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex]++;
    
            if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmData->FrameID) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
            {
                pRecordSet->MetaData.IrtPortSyncTxCnt[HwPortIndex]++;
                pDDB->SYNC.bSyncInRedActive = LSA_TRUE; //at least one port needs the SIR statemachines
            }
        }

        if (pFrmData->UsrRxPort_0_4 == 0)
        {
            continue;
        }

        if (EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4) == HwPortIndex)
        {
        
            if( 0 == pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex])
            {
                 // first increment for this port ==> inc a new rx port user
                pRecordSet->MetaData.IrtAllPortUsrRxCnt++;
            }
            pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex]++;    
    
            if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmData->FrameID) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
            {
                pRecordSet->MetaData.IrtPortSyncRxCnt[HwPortIndex]++;
                pDDB->SYNC.bSyncInRedActive = LSA_TRUE; //at least one port needs the SIR statemachines
            }
        }
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
static  EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SyncGetNextIrFrameDataElement( EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData )
{
    LSA_UINT32     Elementlength;
    LSA_UINT32     Padding;
    LSA_UINT32     Rest;
    LSA_UINT8   *  pNext;

    Elementlength = 17 + pFrmData->NumberOfTxPortGroups; //TODO rename  17 to define

    Rest = Elementlength % 4;

    //Calculate Padding
    if (Rest)
    {
        Padding = 4 - Rest;
    }
    else
    {
        Padding = 0;
    }

    pNext = ((LSA_UINT8 *)(void *)pFrmData) + Padding + Elementlength;

    return (EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE)(void *)pNext;
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
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncBuildAllFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                              LSA_UINT16                           const  NumberOfElements,
                                                              EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pFrmData,
                                                              EDDI_PRM_RECORD_IRT_PTR_TYPE                pRecordSet )
{
    LSA_UINT16                            iElement;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE         pFrmHandler;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                     const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE   pLocalFrmData;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncBuildAllFrameHandler->");

    /* setup management for Time overlap check of IRT-Frames */
    EDDI_PrmPDIRCheckFrameTimesSetup(pDDB);

    //Setup counters for IRT providers, consumers and forwarders
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pRecordSet->MetaData.IrtPortUsrTxCnt[HwPortIndex] = 0;
        pRecordSet->MetaData.IrtPortUsrRxCnt[HwPortIndex] = 0;

        pRecordSet->MetaData.IrtPortSyncTxCnt[HwPortIndex] = 0;
        pRecordSet->MetaData.IrtPortSyncRxCnt[HwPortIndex] = 0;
    }
    pDDB->SYNC.bSyncInRedActive = LSA_FALSE;
    
    pRecordSet->MetaData.IrtAllPortUsrTxCnt = 0;
    pRecordSet->MetaData.IrtAllPortUsrRxCnt = 0;

    // Reset Counters about used FCW types
    pRecordSet->MetaData.n_IrtSyncSndCnt      = 0;
    pRecordSet->MetaData.n_IrtProviderFcwCnt  = 0;
    pRecordSet->MetaData.n_IrtConsumerFcwCnt  = 0;
    pRecordSet->MetaData.n_IrtForwarderFcwCnt = 0;

    pLocalFrmData = pFrmData;

    /* --------------------------------------------------------------------------*/
    /* 1. Check if we have enough pool resources for FCWs                        */
    /* --------------------------------------------------------------------------*/
    for (iElement = 0; iElement < NumberOfElements; iElement++)
    {
        // Check for Special FrameDetail-Flags
        if (EDDI_GetBitField8Bit(pLocalFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET))
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, SyncIr_GetIrtTimeWindow failed");
            return LSA_FALSE;
        }
        else
        {
            //Register the TimeSlice occupied by pFrmHandler. So, the minium / maximum IRT-Time expansion of each port and direction (FCW Consumer/Forwarder/Provider) will be calculated.
            EDDI_SyncIrRegisterPortUser(pDDB, pLocalFrmData, pRecordSet);
        }
        pLocalFrmData = EDDI_SyncGetNextIrFrameDataElement(pLocalFrmData);
    }

    if (!EDDI_SyncIrCheckPoolResources(pDDB,pRecordSet))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, EDDI_SyncIrCheckPoolResources failed");

        /* Note: We use EDD_STS_ERR_PARAM because "out of resources" is not supported as PRM-Error */
        /*       CM knows about this special case and will not threat a PARM-Error as fatal        */
        return LSA_FALSE;
    }

    /* --------------------------------------------------------------------------*/
    /* 2. Now build all framehandler                                             */
    /* --------------------------------------------------------------------------*/
    for (iElement = 0; iElement < NumberOfElements; iElement++)
    {
        //Check if FrameId already exists
        //Note: FrameID for secondary RTSync was already substituted with  EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B
        pFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pFrmData->FrameID, pRecordSet);

        if (!pFrmHandler)
        {                          
            //Setup new FrameHandler
            EDDI_MemGetPoolBuffer(pRecordSet->hFrameHandler, (void * *)&pFrmHandler);
            if (!pFrmHandler)
            {
                EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, No more Memory for FrameHandler");
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TooManyElements);
                return LSA_FALSE;
            }

            //Init FrameHandler
            if (!EDDI_SyncIrInitFrameHandler(pDDB, pFrmHandler, pFrmData))
            {
                // Free  in Pool
                EDDI_MemFreePoolBuffer(pRecordSet->hFrameHandler, (void *)pFrmHandler);
                return LSA_FALSE;
            }

            //Queue FrameHandler
            EDDI_QueueAddToEnd(&pRecordSet->FrameHandlerQueue, &pFrmHandler->QueueLink);
        }
        else
        {
            LSA_UINT8  HwPortIndex;
                            
            if (!EDDI_SyncIrUpdateFrameHandler(pDDB, pFrmHandler, pFrmData))
            {
                EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, Updating FrameHandler failed");
                return LSA_FALSE;
            }
                
            //Check FrameDetails
            for (HwPortIndex = 0; HwPortIndex < EDDI_MAX_IRTE_PORT_CNT; HwPortIndex++)
            {
                if (pFrmHandler->pIrFrameDataElem[HwPortIndex])
                {               
                    //Only allowed EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEANING_FRAMESENDOFFSET_SPECIFIES_TIME = 0x0 or EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEDIA_REDUNDANCY_WATCHDOG_DISABLE = 0x0
                    LSA_UINT32  const  FrameDetailsMediaRedundancyWatchdog = EDDI_GetBitField32NoSwap(pFrmHandler->pIrFrameDataElem[HwPortIndex]->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_MEDIA_REDUNDANCY_WATCHDOG);
                    LSA_UINT32  const  FrameDataPropertiesMeaningFramesendoffset = EDDI_GetBitField32NoSwap(pFrmHandler->pIrFrameDataElem[HwPortIndex]->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET);

                    if (   (EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEDIA_REDUNDANCY_WATCHDOG_DISABLE != FrameDetailsMediaRedundancyWatchdog)
                        || (EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEANING_FRAMESENDOFFSET_SPECIFIES_TIME != FrameDataPropertiesMeaningFramesendoffset))
                    {
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_SyncBuildAllFrameHandler, Error: framedetails error. FrameID:0x%X  FrameDetails:0x%X", pFrmHandler->FrameId, pFrmHandler->pIrFrameDataElem[HwPortIndex]->FrameDetails.Byte);
                        return EDD_STS_ERR_PRM_BLOCK;
                    } 
                }
            }  
        }

        //check for overlap of frame within branch
        if (EDDI_PrmPDIRCheckFrameTimesAddElement(pDDB,pFrmData) != EDD_STS_OK)
        {
            EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, PDIRFrameData_CheckFrameTimesAddElement failed");
            return LSA_FALSE;
        }

        pFrmData = EDDI_SyncGetNextIrFrameDataElement(pFrmData);
    }

    //Now check the branch connection points..
    if (EDDI_PrmPDIRCheckFrameTimesCheckBranches(pDDB) != EDD_STS_OK)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncBuildAllFrameHandler, PDIRFrameData_CheckFrameTimesAddElement failed");

        return LSA_FALSE;
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrHasIrtPortSyncSender()                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrHasIrtPortSyncSender( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet = pDDB->PRM.PDIRData.pRecordSet_A;

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrHasIrtPortSyncSender->HwPortIndex:0x%X", HwPortIndex);

    if (pRecordSet->MetaData.IrtPortSyncTxCnt[HwPortIndex] != 0)
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV5)
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  SyncIr_DeleteLineDelays( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SyncDr_DeleteLineDelays->");

    //set LineDelay to NULL for all ports

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksHardware = 0;

        EDDI_SwiMiscSetRegister(HwPortIndex, REG_LINE_DELAY,
                                pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksHardware, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_sync_ir.c                                               */
/*****************************************************************************/

