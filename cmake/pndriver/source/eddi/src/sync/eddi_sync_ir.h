#ifndef EDDI_SYNC_IR_H          //reinclude-protection
#define EDDI_SYNC_IR_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_sync_ir.h                            :F&  */
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
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  25.10.07    JS    added EDDI_SyncIrFrameHandlerGetRealFrameID            */
/*  07.11.07    JS    added EDDI_SyncIrSetupPortOnlyHasSyncTx                */
/*  14.11.07    JS    added EDDI_SyncIrCheckPoolResources                    */
/*  15.02.08    JS    added EDDI_SyncDecodeFrameHandlerType                  */
/*  19.10.09    UL    removed EDDI_SyncIrSetupPortOnlyHasSyncTx              */
/*****************************************************************************/

EDDI_IRT_FRM_HANDLER_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                LSA_UINT16                          const  FrameId,
                                                                                EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

EDDI_IRT_FRM_HANDLER_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandlerForSyncConsumer
                                                                                ( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                  LSA_BOOL                            const  Primary,
                                                                                  EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

EDDI_IRT_FRM_HANDLER_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFindFrameHandlerForSyncProvider( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                                               EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

LSA_VOID     EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrUpdateLineDelay( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                              LSA_UINT8                           const  UsrRxPort_0_4,
                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

LSA_VOID     EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrIrRecordCleanUp( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                              EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

LSA_UINT32   EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrGetTxMaxTimeOfAllPorts( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                     EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

LSA_RESULT   EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrActivate( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_RESULT   EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrDeactivate( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_SyncBuildAllFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                             LSA_UINT16                           const  NumberOfElements,
                                                             EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pFrmData,
                                                             EDDI_PRM_RECORD_IRT_PTR_TYPE                pRecordSet );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrIsUsedByUpperFALSE( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                              LSA_BOOL                            const  bCheckSyncUser );

#if defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrDataCheckReductionRatio(  EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                                    EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet);
#endif // defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrHasIrtPortSyncSender( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex );

LSA_UINT16 EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrFrameHandlerGetRealFrameID( LSA_UINT16  const  FrameID );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrCheckPoolResources( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                              EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );

EDDI_SYNC_IRT_FRM_HANDLER_ENUM  EDDI_LOCAL_FCT_ATTR EDDI_SyncDecodeFrameHandlerType( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                     EDDI_PRM_RECORD_FRAME_DATA_TYPE  const * const  pCurrentElem );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_IR_H


/*****************************************************************************/
/*  end of file eddi_sync_ir.h                                               */
/*****************************************************************************/
