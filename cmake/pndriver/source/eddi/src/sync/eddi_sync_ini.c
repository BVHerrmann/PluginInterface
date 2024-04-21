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
/*  F i l e               &F: eddi_sync_ini.c                           :F&  */
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
/*  18.07.07    JS    added management for EDD_SRV_SYNC_RECV                 */
/*  24.10.07    JS    added support for redundancy buffered Sync send/recv   */
/*  19.02.08    JS    moved EDDI_IRT_MAX_TREE_ELEMENTS to eddi_sync_typ.h    */
/*  20.02.08    JS    added "EDDI_CFG_PDIRDATA_NO_TIME_OVERLAP_CHECK" handlin*/
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_pool.h"
#include "eddi_sync_ini.h"
#include "eddi_irt_ext.h"
#include "eddi_ser_ext.h"
#include "eddi_irt_tree.h"
#include "eddi_sync_ir.h"
#include "eddi_prm_req.h"

#define EDDI_MODULE_ID     M_ID_SYNC_INI
#define LTRC_ACT_MODUL_ID  131

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncInitMemPools( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncCloseMemPools( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                            EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE const pSyncComp );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncInitMemPoolsKRAM( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncReserveMemPoolsKRAM( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCInitComponent()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  pSyncComp;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SYNCInitComponent->");

    if (!(pDDB->pLocal_SYNC == EDDI_NULL_PTR))
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SYNCInitComponent, pLocal_SYNC <> EDDI_NULL_PTR");
        EDDI_Excp("EDDI_SYNCInitComponent, pLocal_SYNC, EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pSyncComp = &pDDB->SYNC;

    /* management for EDD_SRV_SYNC_RECV */
    pSyncComp->RxUser.pFirstRQB     = LSA_NULL;
    pSyncComp->RxUser.pLastRQB      = LSA_NULL;
    pSyncComp->RxUser.pHDB          = LSA_NULL;
    pSyncComp->RxUser.RxQueueReqCnt = 0;

    EDDI_SyncInitMemPools(pDDB);

    //Initialisation successfull: Set States and pLocal_SYNC;
    pDDB->pLocal_SYNC = pSyncComp;

    pDDB->pLocal_IRT  = &pDDB->IRT;

    EDDI_SyncReserveMemPoolsKRAM(pDDB);

    EDDI_SYNCInitIRT(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCInitComponentPart2()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCInitComponentPart2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  pSyncComp;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCInitComponentPart2->");

    if (pDDB->pLocal_SYNC == EDDI_NULL_PTR)
    {
        return;
    }

    pSyncComp = &pDDB->SYNC;

    //EDDI_MemSet(pSyncComp, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_SYNC_TYPE));

    pDDB->PRM.PDIRData.pRecordSet_A = &pDDB->PRM.PDIRData.RecordSet_A;
    pDDB->PRM.PDIRData.pRecordSet_B = &pDDB->PRM.PDIRData.RecordSet_B;

    pDDB->PRM.PDSyncData.pRecordSet_A = &pDDB->PRM.PDSyncData.RecordSet_A;
    pDDB->PRM.PDSyncData.pRecordSet_B = &pDDB->PRM.PDSyncData.RecordSet_B;

    //hFrameHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRData.pRecordSet_A->hFrameHandler = 0x0UL;
    (void)EDDI_MemIniPool(pSyncComp->FrameHandlerCnt,
                           (LSA_UINT32)sizeof(EDDI_IRT_FRM_HANDLER_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRData.pRecordSet_A->hFrameHandler ); // OUT

    //hFrameHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRData.pRecordSet_B->hFrameHandler = 0x0UL;
    (void)EDDI_MemIniPool(pSyncComp->FrameHandlerCnt,
                           (LSA_UINT32)sizeof(EDDI_IRT_FRM_HANDLER_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRData.pRecordSet_B->hFrameHandler ); // OUT

    pDDB->PRM.PDIRData.pRecordSet_A->PDIRDataRecordActLen  = 0;
    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
    if (EDDI_NULL_PTR == pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDIRData.pRecordSet_A->pNetWorkPDIRDataRecord, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
    if (EDDI_NULL_PTR ==  pDDB->PRM.PDIRData.pRecordSet_A->pNetWorkPDIRDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDIRData.pRecordSet_A->pNetWorkPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pDDB->PRM.PDIRData.pRecordSet_B->PDIRDataRecordActLen  = 0;
    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
    if (EDDI_NULL_PTR == pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
    if (EDDI_NULL_PTR == pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord,  (LSA_UINT32)EDDI_SYNC_MAX_SYNCDATA_LENGTH );
    if (EDDI_NULL_PTR == pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDSyncData.pRecordSet_A->pNetWorkPDSyncDataRecord,  (LSA_UINT32)EDDI_SYNC_MAX_SYNCDATA_LENGTH );
    if (EDDI_NULL_PTR == pDDB->PRM.PDSyncData.pRecordSet_A->pNetWorkPDSyncDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDSyncData.pRecordSet_A->pNetWorkPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord,  (LSA_UINT32)EDDI_SYNC_MAX_SYNCDATA_LENGTH );
    if (EDDI_NULL_PTR == pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((void **)&pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord,  (LSA_UINT32)EDDI_SYNC_MAX_SYNCDATA_LENGTH );
    if (EDDI_NULL_PTR == pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord)
    {
        EDDI_Excp("EDDI_SYNCInitComponentPart2, Alloc pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_PrmPDIRCheckFrameTimesInit(pDDB); /* init Time-check management */

    EDDI_MemSet(pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord, (LSA_UINT8)0, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
    EDDI_MemSet(pDDB->PRM.PDIRData.pRecordSet_A->pNetWorkPDIRDataRecord, (LSA_UINT8)0, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);

    EDDI_MemSet(pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord, (LSA_UINT8)0, EDDI_SYNC_MAX_SYNCDATA_LENGTH);
    EDDI_MemSet(pDDB->PRM.PDSyncData.pRecordSet_A->pNetWorkPDSyncDataRecord, (LSA_UINT8)0, EDDI_SYNC_MAX_SYNCDATA_LENGTH);

    pDDB->PRM.PDIRData.pRecordSet_A->FrameHandlerQueue.Count  = 0;
    pDDB->PRM.PDIRData.pRecordSet_A->FrameHandlerQueue.pFirst = EDDI_NULL_PTR;
    pDDB->PRM.PDIRData.pRecordSet_A->FrameHandlerQueue.pLast  = EDDI_NULL_PTR;

    pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.Count  = 0;
    pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pFirst = EDDI_NULL_PTR;
    pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pLast  = EDDI_NULL_PTR;

    //EDDI_SyncInitMemPools(pDDB);

    pSyncComp->IrtActivity  = LSA_FALSE;
    pSyncComp->SyncActivity = LSA_FALSE;

    //Reset IrRecord and its additional Infos
    EDDI_SyncIrIrRecordCleanUp(pDDB, pDDB->PRM.PDIRData.pRecordSet_A);
    EDDI_SyncIrIrRecordCleanUp(pDDB, pDDB->PRM.PDIRData.pRecordSet_B);

    EDDI_SyncPortStmsInit(pDDB);

    //Initialisation successfull: Set States and pLocal_SYNC
    pDDB->pLocal_SYNC = pSyncComp;

    pDDB->pLocal_IRT  = &pDDB->IRT;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCInitComponentPart2, Initialisation OK");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCInitIRT()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCInitIRT( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                     const  PortMapCnt         = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                     const  CfgCycleBaseFactor = pDDB->pLocal_CRT->CycleBaseFactor;
    EDDI_IRT_REDUCTION_AX_TYPE  *  const  pIrtRed            = &pDDB->CRT.IRTRedAx;
    LSA_UINT32                     const  HwPortIndex1       = EDDI_PmGetHwPortIndex(pDDB, 0);
    EDDI_TREE_TYPE              *  const  pTreeTx            = &pIrtRed->IrtRedTreeTx[HwPortIndex1];
    EDDI_TREE_TYPE              *  const  pTreeRx            = &pIrtRed->IrtRedTreeRx[HwPortIndex1];

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCInitIRT->");

    pIrtRed->KramSizeNOOP = sizeof(EDDI_SER10_NOOP_TYPE) * (PortMapCnt * 2);
    pIrtRed->KramSizeEOL  = sizeof(EDDI_SER10_EOL_TYPE)  * PortMapCnt * (EDDI_FCW_TX_EOL_CNT + 1);  //TX-EOLs + 1 RX-EOL

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    pIrtRed->KramSizeIrtPortStartTimeTxBlock = sizeof(EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE) * PortMapCnt;
    pIrtRed->KramSizeIrtPortStartTimeRxBlock = sizeof(EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE) * PortMapCnt;

    pDDB->CRT.KRAMSize_xRT_Trees += (pIrtRed->KramSizeEOL + pIrtRed->KramSizeNOOP + pIrtRed->KramSizeIrtPortStartTimeTxBlock + pIrtRed->KramSizeIrtPortStartTimeRxBlock);
    #else
    pDDB->CRT.KRAMSize_xRT_Trees += (pIrtRed->KramSizeEOL + pIrtRed->KramSizeNOOP);
    #endif

    //init 1st port
    EDDI_RedTreeInit(pDDB, pTreeTx,
                     EDDI_TREE_RT_CLASS_FCW_TX,
                     CfgCycleBaseFactor,
                     EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX,
                     EDDI_IRT_MAX_TREE_ELEMENTS);

    EDDI_RedTreeInit(pDDB, pTreeRx,
                     EDDI_TREE_RT_CLASS_FCW_RX,
                     CfgCycleBaseFactor,
                     EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX,
                     EDDI_IRT_MAX_TREE_ELEMENTS);

    //init remaining ports
    for (UsrPortIndex = 1; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32         const  HwPortIndex   = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        EDDI_TREE_TYPE  *  const  pTreeTxTarget = &pIrtRed->IrtRedTreeTx[HwPortIndex];
        EDDI_TREE_TYPE  *  const  pTreeRxTarget = &pIrtRed->IrtRedTreeRx[HwPortIndex];

        //ATTENTION: aggressive optimization!
        *pTreeTxTarget = *pTreeTx;
        *pTreeRxTarget = *pTreeRx;
    }

    pDDB->CRT.KRAMSize_xRT_Trees += ((pTreeTx->KramSize + pTreeRx->KramSize) * PortMapCnt);

    //Rest in EDDI_SYNCBuildIRT()
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCBuildIRT()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCBuildIRT( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    #if !defined (EDDI_RED_PHASE_SHIFT_ON)
    LSA_UINT32  UsrPortIndex;
    #endif

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCBuildIRT->");

    if (!pDDB->pLocal_CRT)
    {
        return;
    }

    EDDI_SyncInitMemPoolsKRAM(pDDB);

    EDDI_IRTTreeInitAx(pDDB, pDDB->pLocal_CRT->CycleBaseFactor);

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    {
        LSA_UINT32  const  IrtControlVal = IO_R32(IRT_CTRL);

        if (IrtControlVal)
        {
            EDDI_Excp("EDDI_SYNCBuildIRT, IRT_Control Register != 0", EDDI_FATAL_ERR_EXCP, IrtControlVal, 0);
            return;
        }
    }
    #else
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //ini IRT_Ctrl_Base-Register (BasePtr on all KRAM-IRT-Listheaders)
        EDDI_SERSetIrtPortActivityRxTx(HwPortIndex, LSA_FALSE, pDDB);
    }
    #endif

    //Now Init Lower IRTComponent
    EDDI_IRTInitFcwLists(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SYNCRelComponent()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16                         ret_val;
    EDDI_DDB_COMP_SYNC_TYPE  *  const  pSyncComp = pDDB->pLocal_SYNC;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SYNCRelComponent->");

    if (pDDB->pLocal_SYNC == EDDI_NULL_PTR)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SYNCRelComponent, Pointer to Sync-Struct is already Null");
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRData.pRecordSet_A->pNetWorkPDIRDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_A->pNetWorkPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_A->pLocalPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    pDDB->PRM.PDIRData.pRecordSet_A->PDIRDataRecordActLen = 0;

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_B->pNetWorkPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_B->pLocalPDIRDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    pDDB->PRM.PDIRData.pRecordSet_B->PDIRDataRecordActLen = 0;

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDSyncData.pRecordSet_A->pNetWorkPDSyncDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_A->pNetWorkPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_A->pLocalPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_B->pNetWorkPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_SYNCRelComponent, pRecordSet_B->pLocalPDSyncDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_PrmPDIRCheckFrameTimesExit(pDDB); /* release FrameCheck management */

    EDDI_SyncCloseMemPools(pDDB, pSyncComp);

    pDDB->pLocal_SYNC = EDDI_NULL_PTR;

    pSyncComp->SyncActivity = LSA_FALSE;
    pSyncComp->IrtActivity  = LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncInitMemPools()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncInitMemPools( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_IRT_INI_TYPE                  *  const  pIrtCfgPara = &(pDDB->pLocal_CRT->CfgPara.IRT);
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE     const  pSyncComp   = &pDDB->SYNC;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncInitMemPools->");

    // Needed FrmHandlers: 2 extra for SyncMaster (Prim and/or Secondary) or
    // SyncSlave (Prim and/or Secondary)
    pSyncComp->FrameHandlerCnt = (pIrtCfgPara->ConsumerCntClass3 +
                                  pIrtCfgPara->ProviderCnt       +
                                  pIrtCfgPara->ForwarderCnt      +
                                  2);

    // Needed LinkModifierWords (LMWs)  and NOOPs for IrtSyncSender, one for each UsrPort:
    pSyncComp->n_IrtSyncSndCnt  = pDDB->PM.PortMap.PortCnt;

    // Needed IrtProvider-Fcws: 1 extra for SyncMaster, Factor pDDB->PM.PortMap.PortCnt or multiple SndPorts.
    pSyncComp->n_IrtProviderFcwCnt = (pIrtCfgPara->ProviderCnt + 1)  * pDDB->PM.PortMap.PortCnt;

    // Needed IrtConsumer-Fcws: 2 extra for SyncSlave (Prim/Sec),  Factor 2 for redundant RcvPorts.
    pSyncComp->n_IrtConsumerFcwCnt = (pIrtCfgPara->ConsumerCntClass3 + 2)  * 2;

    // Needed IrtForwarder-Fcws:
    pSyncComp->n_IrtForwarderFcwCnt = pIrtCfgPara->ForwarderCnt + pDDB->PM.PortMap.PortCnt;  // + Sync Frame Forwarder

    /* Max number of Framebuffers needed with RTSync buffered send/receive. */
    /* Depends on Portcount. We assume the following:                       */
    /* Per send/receive        : 2 Buffers (for exchange)                   */
    /* Per redundancy Port     : Master: 1 Recv, 1 Send  (= 4 Buffers)      */
    /*                         : Slave : 2 Recv          (= 4 Buffers)      */
    /* Per non redundancy Port : Master: 1 Send          (= 2 Buffers)      */
    /*                         :                                            */
    /* We assume that we always have 2 redundancy ports and all other ports */
    /* are non redundancy.                                                  */
    /* Note: Recv is always a possible forwarder. no send required.         */
    /* This result in: 4 Port: 12 Buffers                                   */
    /* This result in: 2 Port:  8 Buffers                                   */

    pSyncComp->n_SyncFrameBuffer = 8;  /* at least 8 buffers for 2 ports */
    if (pDDB->PM.PortMap.PortCnt > 2)  /* for all further port 2 buffers */
    {
        pSyncComp->n_SyncFrameBuffer += (pDDB->PM.PortMap.PortCnt - 2) * 2;
    }
    // old:
    // pSyncComp->n_SyncFrameBuffer    = 4;

    pSyncComp->Size_SyncFrameBuffer = ((EDDI_SYNC_FRAME_KRAMDATA_SIZE + 4 + 7) / 8) * 8;
    // Attention: Deallocatin "+4" reserves space for the CHECK_PATTERN of memory-admin -> Exception
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncInitMemPoolsKRAM()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncInitMemPoolsKRAM( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_KRAM_RES_POOL_TYPE                *  pPool;
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  const  pSyncComp = &pDDB->SYNC;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncInitMemPoolsKRAM->");

    pPool = &pDDB->KramRes.pool;

    // hIrtSyncSndLmw Contains KRAM-Ressources for buffered SyncSnd
    pSyncComp->MemPool.hIrtSyncSnd = 0;
    pPool->fcw_snd = EDDI_MemIniPool(pSyncComp->n_IrtSyncSndCnt,
                                      (LSA_UINT32)sizeof(EDDI_SER10_SINGLE_SHOT_SND_TYPE),
                                      EDDI_POOL_TYPE_KRAM_8BA,
                                      EDDI_AllocLocalMem,
                                      EDDI_FREELOCALMEM,
                                      EDDI_MEM_DO_NO_PRESET,
                                      (LSA_UINT8)0, // Preset Value
                                      pDDB,
                                      &pSyncComp->MemPool.hIrtSyncSnd ); // OUT

    // hIrtProviderFcw Contains KRAM-Ressources for IRTProvider-FCWs
    pSyncComp->MemPool.hIrtProviderFcw = 0;
    pPool->fcw_snd = EDDI_MemIniPool(pSyncComp->n_IrtProviderFcwCnt,
                                      (LSA_UINT32)sizeof(EDDI_SER10_FCW_SND_TYPE),
                                      EDDI_POOL_TYPE_KRAM_8BA,
                                      EDDI_AllocLocalMem,
                                      EDDI_FREELOCALMEM,
                                      EDDI_MEM_DO_NO_PRESET,
                                      (LSA_UINT8)0, // Preset Value
                                      pDDB,
                                      &pSyncComp->MemPool.hIrtProviderFcw ); // OUT

    // hIrtConsumerFcw Contains KRAM-Ressources for IrtConsumer-FCWs
    // and forwarding IrtConsumer-FCWs
    pSyncComp->MemPool.hIrtConsumerFcw = 0;
    pPool->fcw_rcv = EDDI_MemIniPool(pSyncComp->n_IrtConsumerFcwCnt,
                                      (LSA_UINT32)sizeof(EDDI_SER10_FCW_RCV_TYPE),
                                      EDDI_POOL_TYPE_KRAM_8BA,
                                      EDDI_AllocLocalMem,
                                      EDDI_FREELOCALMEM,
                                      EDDI_MEM_DO_NO_PRESET,
                                      (LSA_UINT8)0, // Preset Value
                                      pDDB,
                                      &pSyncComp->MemPool.hIrtConsumerFcw ); // OUT

    // hIrtForwarderFcw Contains KRAM-Ressources for IrtForwarder-FCWs
    pSyncComp->MemPool.hIrtForwarderFcw = 0;
    pPool->fcw_rcv_forwarder = EDDI_MemIniPool(pSyncComp->n_IrtForwarderFcwCnt,
                                                (LSA_UINT32)sizeof(EDDI_SER10_FCW_RCV_FORWARDER_TYPE),
                                                EDDI_POOL_TYPE_KRAM_8BA,
                                                EDDI_AllocLocalMem,
                                                EDDI_FREELOCALMEM,
                                                EDDI_MEM_DO_NO_PRESET,
                                                (LSA_UINT8)0, // Preset Value
                                                pDDB,
                                                &pSyncComp->MemPool.hIrtForwarderFcw ); // OUT

    // hSyncFrameBuffer Contains KRAM-Ressources for IrtSyncFrames (For IRTSyncMaster or IRTSyncSlave)
    pSyncComp->MemPool.hSyncFrameBuffer = 0;
    pPool->sync_frame = EDDI_MemIniPool(pSyncComp->n_SyncFrameBuffer,
                                         pSyncComp->Size_SyncFrameBuffer,
                                         EDDI_POOL_TYPE_KRAM_8BA,
                                         EDDI_AllocLocalMem,
                                         EDDI_FREELOCALMEM,
                                         EDDI_MEM_DO_NO_PRESET,
                                         (LSA_UINT8)0, // Preset Value
                                         pDDB,
                                         &pSyncComp->MemPool.hSyncFrameBuffer ); // OUT
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncReserveMemPoolsKRAM()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncReserveMemPoolsKRAM( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  const  pSyncComp = &pDDB->SYNC;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncReserveMemPoolsKRAM->");

    pDDB->CRT.KRAMSize_xRT_Trees +=
        (pSyncComp->n_IrtSyncSndCnt      * sizeof(EDDI_SER10_SINGLE_SHOT_SND_TYPE)) +
        (pSyncComp->n_IrtProviderFcwCnt  * sizeof(EDDI_SER10_FCW_SND_TYPE)) +
        (pSyncComp->n_IrtConsumerFcwCnt  * sizeof(EDDI_SER10_FCW_RCV_TYPE)) +
        (pSyncComp->n_IrtForwarderFcwCnt * sizeof(EDDI_SER10_FCW_RCV_FORWARDER_TYPE)) +
        (pSyncComp->n_SyncFrameBuffer    * pSyncComp->Size_SyncFrameBuffer);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncCloseMemPools()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncCloseMemPools( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                               EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE  const  pSyncComp )
{
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncCloseMemPools->");

    EDDI_MemClosePool(pDDB->PRM.PDIRData.pRecordSet_A->hFrameHandler);
    EDDI_MemClosePool(pDDB->PRM.PDIRData.pRecordSet_B->hFrameHandler);
    EDDI_MemClosePool(pSyncComp->MemPool.hIrtSyncSnd);
    EDDI_MemClosePool(pSyncComp->MemPool.hIrtProviderFcw );
    EDDI_MemClosePool(pSyncComp->MemPool.hIrtConsumerFcw );
    EDDI_MemClosePool(pSyncComp->MemPool.hIrtForwarderFcw);
    EDDI_MemClosePool(pSyncComp->MemPool.hSyncFrameBuffer);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_ini.c                                              */
/*****************************************************************************/

