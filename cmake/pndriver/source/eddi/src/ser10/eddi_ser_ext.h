#ifndef EDDI_SER_EXT_H          //reinclude-protection
#define EDDI_SER_EXT_H

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
/*  F i l e               &F: eddi_ser_ext.h                            :F&  */
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
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetup( SER_HANDLE_PTR          const pHandle,
                                              EDDI_UPPER_DSB_PTR_TYPE const pParam,
                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIRTSAdress( EDDI_LOCAL_MAC_ADR_PTR_TYPE const pMACAdress,
                                                      EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERReset( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB, LSA_BOOL bCalledForOpen);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERAcwSetup( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERImageDMASetup( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                      EDDI_CRT_SYNC_IMAGE_TYPE const * const pSyncImage );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCcwAdd( EDDI_SER_CCW_PTR_TYPE         pCCW,
                                               EDDI_CCW_CTRL_HEAD_PTR_TYPE   pCCWHead,
                                               EDDI_CCW_CTRL_PTR_TYPE      * ppCCWCtrl,
                                               EDDI_LOCAL_DDB_PTR_TYPE       pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCcwRemove( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                  EDDI_CCW_CTRL_PTR_TYPE    const pCCWCtrl,
                                                  void                    * const pKRAMSmallApduBuffer );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERXchangeBuffer( EDDI_DDB_TYPE *            const pDDB,
                                                      EDDI_SER_CCW_PTR_TYPE      const pCW,
                                                      EDDI_LOCAL_MEM_U8_PTR_TYPE const pNewBuffer );

/*==========================================================================*/
/* HW-TimerScoreboard-Function from ser_sb.c                                */
/*==========================================================================*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERScoreBoardStart( LSA_BOOL                const ComClass_1_2,
                                                      LSA_BOOL                const ComClass3,
                                                      EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERScoreBoardStop( LSA_BOOL                const ComClass_1_2,
                                                     LSA_BOOL                const ComClass3,
                                                     EDDI_LOCAL_DDB_PTR_TYPE const pDDB );


LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_SERScoreBoardEnableChange( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

/*==========================================================================*/
/* HW-ReductionTree-Functions                                               */
/*==========================================================================*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                 EDDI_TREE_TYPE           *  const  pTree,
                                                 EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                 LSA_UINT32                  const  CycleBaseFactor,
                                                 LSA_UINT32                  const  CfgMaxReduction,
                                                 LSA_UINT32                  const  TxRxElementCnt );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeBuild( EDDI_DDB_TYPE                 * const pDDB,
                                                EDDI_TREE_TYPE                * const pTree,
                                                LSA_UINT32                      const CycleBaseFactor,
                                                EDDI_SER_CCW_PTR_TYPE           const pRootCw,
                                                EDDI_LOCAL_EOL_PTR_TYPE const * const ppEOLn );

#if defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBcwModInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                       EDDI_TREE_TYPE           *  const  pTree,
                                                       EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                       LSA_UINT32                  const  CycleBaseFactor,
                                                       LSA_UINT32                  const  CfgMaxReduction,
                                                       LSA_UINT32                  const  TxRxElementCnt );

LSA_VOID EDDI_LOCAL_FCT_ATTR RedTreeBcwModBuild( EDDI_DDB_TYPE                 * const pDDB,
                                                 EDDI_TREE_TYPE                * const pTree,
                                                 LSA_UINT32                      const CycleBaseFactor,
                                                 EDDI_SER_CCW_PTR_TYPE           const pRootCw,
                                                 EDDI_LOCAL_EOL_PTR_TYPE const * const ppEOLn );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeUpdateBCWMods( EDDI_DDB_TYPE             * const pDDB,
                                                        EDDI_TREE_TYPE            * const pTree,
                                                        LSA_UINT16                  const CurrentReduction,
                                                        LSA_UINT16                  const CurrentPhase,
                                                        LSA_UINT16                  const CycleBaseFactor);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBcwModClose( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                        EDDI_TREE_TYPE            *  const  pTree );
                                                        
#elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeRcwInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                    EDDI_TREE_TYPE           *  const  pTree,
                                                    EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                    LSA_UINT32                  const  CycleBaseFactor,
                                                    LSA_UINT32                  const  CfgMaxReduction,
                                                    LSA_UINT32                  const  ProviderCnt );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeRcwBuild( EDDI_DDB_TYPE                 * const pDDB,
                                                   EDDI_TREE_TYPE                * const pTree,
                                                   LSA_UINT32                      const CycleBaseFactor,
                                                   EDDI_SER_CCW_PTR_TYPE           const pRootCw,
                                                   EDDI_LOCAL_EOL_PTR_TYPE const * const ppEOLn );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBuildBranchRCWPHS( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              EDDI_TREE_ELEM_PTR_TYPE  const  pHeadElem,
                                                              EDDI_SER_CCW_PTR_TYPE    const  pHeadCw,
                                                              EDDI_TREE_ELEM_PTR_TYPE  const  pTailElem );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeBuildBranch( LSA_UINT32                 const HeadOpc,
                                                      EDDI_TREE_ELEM_PTR_TYPE    const pHeadElem,
                                                      EDDI_SER_CCW_PTR_TYPE      const pHeadCw,
                                                      EDDI_TREE_ELEM_PTR_TYPE    const pTailElem,
                                                      EDDI_SER_CCW_PTR_TYPE      const pTailCw,
                                                      EDDI_DDB_TYPE            * const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeUpdateRCWHeads( EDDI_DDB_TYPE             * const pDDB,
                                                         EDDI_TREE_TYPE      const * const pTree,
                                                         LSA_UINT16                  const CycleBaseFactor);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeRcwClose( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                     EDDI_TREE_TYPE          const * const  pTree );
#endif

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeClose( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                  EDDI_TREE_TYPE            *  const  pTree );

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtWaitForDeadline( LSA_UINT32                        const DeadLine,
                                                              EDDI_LOCAL_DDB_PTR_TYPE           const pDDB);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_RedTreeSrtProviderAdd( EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                           EDDI_CRT_PROVIDER_PARAM_TYPE     * const pLowerParams,
                                                           EDDI_TREE_ELEM_PTR_TYPE          * const ppElemHandle,
                                                           EDDI_SER_CCW_PTR_TYPE              const  pCWStored );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeSrtProviderSetActivity( EDDI_TREE_ELEM_PTR_TYPE      const pLowerCtrlACW,
                                                                 LSA_BOOL                     const bActivate);

#if !defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderChPhasePart1( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                      EDD_UPPER_RQB_PTR_TYPE           const  pRQB,
                                                                      EDDI_CRT_PROVIDER_PTR_TYPE       const  pProvider,
                                                                      LSA_UINT16                       const  NewCyclePhase,
                                                                      LSA_UINT32                       const  NewCyclePhaseSequence,
                                                                      LSA_BOOL                      *  const  pIndicate );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeSrtProviderChPhasePart2( EDD_UPPER_RQB_PTR_TYPE         const pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE        const pDDB);
#endif //!defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_RedTreeSrtProviderACWRemovePart1( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                      EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                                      EDDI_TREE_ELEM_PTR_TYPE         const pDelElemHandle);

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeSrtProviderRemovePart2( EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                                  EDDI_TREE_ELEM_PTR_TYPE                     pDelElemHandle,
                                                                  EDDI_CRT_PROVIDER_PARAM_TYPE  const * const pLowerParams,
                                                                  LSA_BOOL                              const bFreeACW);

LSA_BOOL  EDDI_LOCAL_FCT_ATTR EDDI_RedIsBinaryValue( LSA_UINT32 const Value);

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtInsertTxEol( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_TREE_TYPE           *  const  pTxTree,
                                                          EDDI_LOCAL_EOL_PTR_TYPE     const  pEOL,
                                                          LSA_UINT16                  const  CyclePhase);

#define RedGetQuickBinaryIndex(CycleReduction, CyclePhase)    ((1 * ((CycleReduction) - 1)) + (((CyclePhase) - 1)))

// JM LSA_UINT32 EDDI_LOCAL_FCT_ATTR RedGetQuickBinaryIndex( LSA_UINT16                const CycleReduction,
//                                                           LSA_UINT16                const CyclePhase);

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeGetMaxPathLoad ( EDDI_TREE_PTR_TYPE        const pTree,
                                                           LSA_UINT16                const CycleReductionRatio,
                                                           LSA_UINT16                const CyclePhase,
                                                           LSA_UINT32              * pMaxByteCounter,
                                                           LSA_UINT32              * pMaxProvCounter);

EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtAddCw( EDDI_LOCAL_DDB_PTR_TYPE             const pDDB,
                                                                   EDDI_TREE_PTR_TYPE                  const pTree,
                                                                   EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE const pFrmHandler,
                                                                   EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pFrmData,
                                                                   LSA_UINT32                          const FcwTimeNs,
                                                                   EDDI_SER10_CCW_TYPE               * const pFcw);

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtRemoveCw( EDDI_TREE_PTR_TYPE               const pTree,
                                                       EDDI_TREE_ELEM_PTR_TYPE          const pTreeElem,
                                                       EDDI_LOCAL_DDB_PTR_TYPE          const pDDB);

EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR EDDI_RedTreeGetPrevElemByTime( EDDI_TREE_PTR_TYPE           const pTree,
                                                                            LSA_UINT16                   const CycleReductionRatio,
                                                                            LSA_UINT16                   const CyclePhase,
                                                                            LSA_UINT32                   const Time_10ns);

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtSetTxActivity( EDDI_TREE_ELEM_PTR_TYPE       const pTreeElem,
                                                                          LSA_BOOL                      const bActivate);

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreeIrtSetRxLocalActivity( EDDI_TREE_ELEM_PTR_TYPE      const pTreeElem,
                                                                               LSA_BOOL                     const bActivate);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERConnectCwToHeader(  EDDI_DDB_TYPE                * const pDDB,
                                                         EDDI_LOCAL_LIST_HEAD_PTR_TYPE  const pKramListHead,
                                                         EDDI_SER_CCW_PTR_TYPE          const pNewCW);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERConnectCwToNext( EDDI_DDB_TYPE                * const pDDB,
                                                      EDDI_SER_CCW_PTR_TYPE          const pCwPrev,
                                                      EDDI_SER_CCW_PTR_TYPE          const pCwNext);

#if !defined (EDDI_CFG_REV5)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERInsertCw( EDDI_DDB_TYPE                * const pDDB,
                                               EDDI_SER_CCW_PTR_TYPE          const pCwPrev,
                                               EDDI_SER_CCW_PTR_TYPE          const pCwNew);

#else
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERInsertCwDyn( EDDI_DDB_TYPE                * const pDDB,
                                                  EDDI_SER_CCW_PTR_TYPE          const pCwPrev,
                                                  EDDI_SER_CCW_PTR_TYPE          const pCwNext,
                                                  EDDI_SER_CCW_PTR_TYPE          const pCwNew,
                                                  LSA_BOOL                        const bCwBranchLinking);
#endif

#if !defined (EDDI_CFG_REV5)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERDisconnectCw( EDDI_SER_CCW_PTR_TYPE          const pCwPrev,
                                                   EDDI_SER_CCW_PTR_TYPE          const pCw);

#else
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERDisconnectCwDyn( EDDI_DDB_TYPE                * const pDDB,
                                                      EDDI_SER_CCW_PTR_TYPE          const pCwPrev,
                                                      EDDI_SER_CCW_PTR_TYPE          const pCwNext,
                                                      EDDI_SER_CCW_PTR_TYPE          const pCw,
                                                      LSA_BOOL                       const bCwBranchLinking);
#endif

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERConnectCwToRcwPhase( EDDI_DDB_TYPE                * const pDDB,
                                                          EDDI_LOCAL_LIST_HEAD_PTR_TYPE  const pKramListHead,
                                                          EDDI_SER_CCW_PTR_TYPE          const pCwNext);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERInsertCwToRcwPhase( EDDI_DDB_TYPE                * const pDDB,
                                                         EDDI_LOCAL_LIST_HEAD_PTR_TYPE  const pKramListHead,
                                                         EDDI_SER_CCW_PTR_TYPE          const pCwNext);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERDisconnectCwFromRcwPhase( EDDI_LOCAL_LIST_HEAD_PTR_TYPE  const pKramListHead,
                                                               EDDI_SER_CCW_PTR_TYPE          const pCw);
#endif

/*---------------------------------------------------------------------------*/
/* Diagnose Functions                                                        */
/*---------------------------------------------------------------------------*/

//LSA_VOID EDDI_LOCAL_FCT_ATTR SERCcwSetEnableDiag(EDDI_DDB_TYPE *            const pDDB,
//                                                EDDI_SER10_FCW_ACW_PTR_TYPE const pFCW_ACW,
//                                                LSA_BOOL                    const bEnableDiag);


/*---------------------------------------------------------------------------*/
/* IRT-specific Functions                                                    */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERStopIrtGlobalAndIrtPorts ( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtGlobalActivity ( LSA_BOOL                 const  bActivate,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtPortActivityTx ( LSA_UINT32               const  HwPortIndex,
                                                            LSA_BOOL                 const  bTxActivity,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtPortActivityRx ( LSA_UINT32               const  HwPortIndex,
                                                            LSA_BOOL                 const  bRxActivity,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#if !defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtPortActivityRxTx( LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bRxTxActivity,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
#endif

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtPortStartTimeTx( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                            EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                            EDDI_CRT_PHASE_TX_EVENT     const Event,
                                                            LSA_UINT32                  const StartTime10Ns );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetIrtPortStartTimeRx( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                            EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                            LSA_UINT32                  const StartTime10Ns );
#endif

/* LSA_VOID EDDI_LOCAL_FCT_ATTR SERIniProviderFcw(     SER_IRT_INI_FCW_TYPE    const * const pComCfg,
                                                      EDDI_SER10_CCW_TYPE            * const pCCW,
                                                      EDDI_LOCAL_DDB_PTR_TYPE          const pDDB );*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniSleepingProviderFcw(SER_IRT_INI_FCW_TYPE    const * const pComCfg,
                                                            EDDI_SER10_CCW_TYPE           * const pCCW,
                                                            EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIrtProviderBufferAdd( EDDI_SER10_CCW_TYPE            * const pCCW,
                                                           LSA_UINT8                        const ImageMode,
                                                           EDDI_DEV_MEM_U8_PTR_TYPE         const pDataBuffer,
                                                           EDD_MAC_ADR_TYPE         const * const pDstMacAdr,
                                                           EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                           LSA_BOOL                         const bXCW_DBInitialized);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniSleepingConsumerFcw( SER_IRT_INI_FCW_TYPE    const * const pComCfg,
                                                             LSA_UINT16                      const TxPorts,
                                                             EDDI_SER10_CCW_TYPE           * const pCCW,
                                                             EDDI_LOCAL_DDB_PTR_TYPE         const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIrtConsumerBufferAdd( EDDI_SER10_CCW_TYPE              * const pCCW,
                                                           EDDI_CRT_CONSUMER_PARAM_PTR_TYPE   const pLowerParams,
                                                           EDDI_DEV_MEM_U8_PTR_TYPE           const pDataBuffer,
                                                           EDDI_SER10_IRQ_QUALIFIER_ENUM      const IrqQualifier,
                                                           EDDI_LOCAL_DDB_PTR_TYPE            const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIrtConsumerBufferRemove( EDDI_SER_CCW_PTR_TYPE           const pCW,
                                                              EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

/* LSA_VOID EDDI_LOCAL_FCT_ATTR SERIniConsumerFcw( SER_IRT_INI_FCW_TYPE    const * const pComCfg,
                                                      LSA_UINT16                      const TxPorts,
                                                      EDDI_SER10_CCW_TYPE            * const pCCW,
                                                      LSA_UINT32                      const ui32SBTimerAdr,
                                                      LSA_UINT16                      const WDReloadVal,
                                                      LSA_UINT16                      const SBDataHoldRelValue,
                                                      EDDI_SER10_IRQ_QUALIFIER_ENUM    const IrqQualifier,
                                                      EDDI_LOCAL_DDB_PTR_TYPE          const pDDB );*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniForwarderFcw( SER_IRT_INI_FCW_TYPE    const * const pComCfg,
                                                      LSA_UINT32                      const TxPorts,
                                                      EDDI_SER10_CCW_TYPE           * const pCCW,
                                                      EDDI_LOCAL_DDB_PTR_TYPE         const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIrtForwardingDeactivate( EDDI_SER10_CCW_TYPE          * const pCCW);

/*---------------------------------------------------------------------------*/
/* SRT-Specific Functions                                                    */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniProviderACW( EDDI_SER10_FCW_ACW_PTR_TYPE                  pFcwAcw,
                                                     EDDI_CRT_PROVIDER_PARAM_TYPE   const * const pLowerParams,
                                                     LSA_BOOL                               const bActive,
                                                     LSA_BOOL                               const bInitBuf,
                                                     EDDI_LOCAL_DDB_PTR_TYPE                const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniConsumerACW( EDDI_SER10_CCW_TYPE              * const pCCW,
                                                     EDDI_CRT_CONSUMER_PARAM_PTR_TYPE   const pComCfg,
                                                     EDDI_LOCAL_DDB_PTR_TYPE            const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERConnectCwToBranch( EDDI_DDB_TYPE                    * const pDDB,
                                                        EDDI_SER_CCW_PTR_TYPE              const pCwPrev,
                                                        EDDI_SER_CCW_PTR_TYPE              const pCwNext);

#if defined (EDDI_CFG_REV5)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERConnectCwToNextDyn( EDDI_DDB_TYPE                    * const pDDB,
                                                         EDDI_SER_CCW_PTR_TYPE              const pCwPrev,
                                                         EDDI_SER_CCW_PTR_TYPE              const pCwNext,
                                                         LSA_BOOL                           const bCwBranchLinking);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  SERAlterBCWMOD( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                               EDDI_LOCAL_BCW_MOD_PTR_TYPE  const  pBCW,
                                               LSA_UINT32                   const  ModMask );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  SERIniBCWMOD( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,  
                                             EDDI_LOCAL_BCW_MOD_PTR_TYPE  const  pBCW,
                                             LSA_UINT32                   const  ModMask );
#endif

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniNOOP( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB, 
                                                EDDI_LOCAL_NOOP_PTR_TYPE  const  pNOOP );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniEOL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                               EDDI_LOCAL_EOL_PTR_TYPE  const  pEOL );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERUpdateEOL( EDDI_SER_EOL_PTR_TYPE              const pEOL,
                                                LSA_UINT32                         const Time);

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERUpdateSOL( EDDI_LOCAL_SOL_PTR_TYPE            const pSOL,
                                                LSA_UINT32                         const Time);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniIrtPortStartTimeTxBlock( EDDI_LOCAL_DDB_PTR_TYPE                          const pDDB,
                                                                 EDDI_TREE_TYPE                           const * const pTree,
                                                                 LSA_UINT32                                       const HwPortIndex,
                                                                 EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE   const pIrtPortStartTimeTxBlock);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniIrtPortStartTimeRxBlock( EDDI_LOCAL_DDB_PTR_TYPE                          const pDDB,
                                                                 EDDI_TREE_TYPE                           const * const pTree,
                                                                 LSA_UINT32                                       const HwPortIndex,
                                                                 EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE   const pIrtPortStartTimeRxBlock);
#endif

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniRootAcw( EDDI_SER10_ACW_SND_TYPE         * const pAcwSnd,
                                                 LSA_UINT8                       * const pDummyBuffer,
                                                 EDDI_LOCAL_DDB_PTR_TYPE           const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERUpdateRootAcw( EDDI_SER10_ACW_SND_TYPE         * const pAcw,
                                                    LSA_UINT32                        const TimeTicks_10ns,
                                                    EDDI_LOCAL_DDB_PTR_TYPE           const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSrtConsumerSetActivity( EDDI_CCW_CTRL_PTR_TYPE           const pLowerCtrlACW,
                                                             LSA_BOOL                         const bActivate);

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniRcwSetup( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                    EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW,
                                                    LSA_UINT32                   const  PhaseCnt,
                                                    LSA_UINT32                   const  SendClock,
                                                    LSA_UINT32                   const  ReductionRate,
                                                    LSA_UINT32                   const  SendClockShift );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniRcwChange( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                     EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW,
                                                     LSA_UINT32                   const  PhaseCnt,
                                                     LSA_UINT32                   const  SendClock,
                                                     LSA_UINT32                   const  ReductionRate,
                                                     LSA_UINT32                   const  SendClockShift );

EDDI_LOCAL_RCW_RED_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SERUpdateRcwSendClock( EDDI_DDB_TYPE *                   const pDDB,
                                                                              EDDI_LOCAL_RCW_RED_PTR_TYPE       const pRCW,
                                                                              LSA_UINT8                         const SendClock);

LSA_UINT8  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetRcwSendClock( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                         EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW );
#endif

/*---------------------------------------------------------------------------*/
/* Buffered Sync - Specific Functions                                        */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSingleShotSndIni( SER_IRT_INI_FCW_TYPE      const * const pComCfg,
                                                       EDDI_SER10_CCW_TYPE             * const pCCW,
                                                       EDDI_LOCAL_DDB_PTR_TYPE           const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSingleShotSndIniTake2( EDDI_SER10_SINGLE_SHOT_SND_TYPE * const pSingleShotCw,
                                                            EDDI_DEV_MEM_U8_PTR_TYPE          const pDataBuffer,
                                                            EDD_MAC_ADR_TYPE          const * const pDstMacAdr,
                                                            EDDI_LOCAL_DDB_PTR_TYPE           const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSingleShotSndLoad( EDDI_SER10_SINGLE_SHOT_SND_TYPE  * const pSingleShotCw,
                                                        EDDI_LOCAL_DDB_PTR_TYPE            const pDDB );

/*---------------------------------------------------------------------------*/
/* ser_ini.c - Specific Functions                                            */
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_EXT_H


/*****************************************************************************/
/*  end of file eddi_ser_ext.h                                               */
/*****************************************************************************/
