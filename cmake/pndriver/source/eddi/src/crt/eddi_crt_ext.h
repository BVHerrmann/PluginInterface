#ifndef EDDI_CRT_EXT_H          //reinclude-protection
#define EDDI_CRT_EXT_H

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
/*  F i l e               &F: eddi_crt_ext.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  IRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                    EDDI_LOCAL_HDB_PTR_TYPE  const pHDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCloseChannel( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRequest( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                              LSA_UINT8                 const  ListType,
                                                              LSA_UINT32                const  BufferProperties,
                                                              LSA_BOOL                  const  bIsProvider,
                                                              LSA_BOOL                  const  bAllowUndefined);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckOverlaps( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                        LSA_UINT32                      const  DataOffset,
                                                        LSA_UINT16                      const  DataLen,
                                                        LSA_UINT8                       const  ListType,
                                                        LSA_BOOL                        const  bProvider,
                                                        LSA_BOOL                        const  bSync);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniMACAdr( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                  EDDI_SER10_LL2_COMMON_TYPE         *  const  pHw2,
                                                  EDD_MAC_ADR_TYPE            const  *  const  pMAC );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniProviderACWFrameID(EDDI_CONST_CRT_PROVIDER_PTR_TYPE const  pProvider);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniProviderXCWDstMacIP( EDDI_LOCAL_DDB_PTR_TYPE            const   pDDB,
                                                               EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const   pProvider);

/*---------------------------------------------------------------------------*/
/* CRT-Provider-List-Functions                                               */
/*---------------------------------------------------------------------------*/

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTCreateProviderList( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                           EDDI_CRT_PROVIDER_LIST_PTR_TYPE const pProvList,
                                                           LSA_UINT32                      const maxAcwProviderCount,
                                                           LSA_UINT32                      const maxFcwProviderCount,
                                                           LSA_UINT16                      const maxGroupCount );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDeleteProviderList( EDDI_CRT_PROVIDER_LIST_PTR_TYPE  const  pProvList );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListAddEntry( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE         const pDDB);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListCtrl( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                         LSA_BOOL                     *  const  pIndicate);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListRemoveEntry ( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                 LSA_BOOL                      * const pIndicate );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListGetEntry( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                               EDDI_CRT_PROVIDER_PTR_TYPE       *  const  ppProvider, //OUT
                                                               EDDI_CRT_PROVIDER_LIST_PTR_TYPE     const  pProvList,
                                                               LSA_UINT16                          const  ProvId );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListSetGroupStatus( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListChangePhase( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                LSA_BOOL                      * const pIndicate );

LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_CRTProviderRemoveEventFinish( EDD_UPPER_RQB_PTR_TYPE          const pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                  LSA_BOOL                        const bPassivate);

/*---------------------------------------------------------------------------*/
/* CRT-Provider-Functions                                                    */
/*---------------------------------------------------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderSetBuffer( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                          EDDI_CRT_PROVIDER_PTR_TYPE   const pProvider);

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderCheckDataLenClass12( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB, 
                                                                    LSA_UINT16               const DataLen, 
                                                                    LSA_UINT16               const CycleReductionRatio, 
                                                                    LSA_UINT16               const CyclePhase, 
                                                                    LSA_UINT8                const ProviderType);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEvent( EDDI_CRT_PROVIDER_PTR_TYPE    const  pProvider,
                                                               EDD_UPPER_RQB_PTR_TYPE        const  pRQB,
                                                               LSA_UINT8                     const  GroupDataStatus,
                                                               EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                               LSA_BOOL                      const  bUseAutoStop,
                                                               LSA_BOOL                    * const  pIndicate);
 
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderRemoveEvent( EDD_UPPER_RQB_PTR_TYPE         const  pRQB,
                                                              EDDI_CRT_PROVIDER_PTR_TYPE            pProvider,
                                                              EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                              LSA_BOOL                    *  const  pIndicate );

/*---------------------------------------------------------------------------*/
/* CRT-Consumer-List-Functions                                               */
/*---------------------------------------------------------------------------*/

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTCreateConsumerList( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const  pConsList,
                                                           LSA_UINT32                       const  maxAcwConsumerCount,
                                                           LSA_UINT32                       const  maxFcwConsumerCount );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDeleteConsumerList( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const  pConsList );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListAddEntry( EDD_UPPER_RQB_PTR_TYPE           const pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE          const pDDB);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListControl( EDD_UPPER_RQB_PTR_TYPE           const pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE          const pDDB);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListRemoveEntry( EDD_UPPER_RQB_PTR_TYPE           const pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                                LSA_BOOL                       * const pIndicate );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListGetEntry( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                             EDDI_CRT_CONSUMER_PTR_TYPE     * const ppConsumer,
                                                             EDDI_CRT_CONSUMER_LIST_PTR_TYPE  const pConsList,
                                                             LSA_UINT16                       const ConsId );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListFindByFrameId( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                  EDDI_CRT_CONSUMER_LIST_PTR_TYPE       const  pConsList,
                                                                  EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE      const  pCRT,
                                                                  EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam,
                                                                  LSA_UINT8                             const  ListType,
                                                                  EDDI_CRT_CONSUMER_PTR_TYPE         *  const  ppRetConsumer,
                                                                  LSA_BOOL                              const  bIsDfp );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListFindByFrameId( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                  EDDI_CRT_PROVIDER_LIST_PTR_TYPE       const  pProvList,
                                                                  EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE      const  pCRT,
                                                                  EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam,
                                                                  LSA_UINT8                             const  ListType,
                                                                  EDDI_CRT_PROVIDER_PTR_TYPE         *  const  ppRetProvider,
                                                                  LSA_BOOL                              const  bIsDfp );

/*---------------------------------------------------------------------------*/
/* CRT-Consumer-Functions                                                    */
/*---------------------------------------------------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompResetAPDUStatus( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                            EDD_UPPER_MEM_U8_PTR_TYPE         pDataBuffer,
                                                            LSA_UINT8                  const  DataStatusInitValue,
                                                            LSA_UINT8                  const  TransferStatusInitValue,
                                                            LSA_BOOL                   const  bSysRed );

LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetPendingEvent( EDDI_CRT_CONSUMER_PTR_TYPE       const pConsumer,
                                                                EDDI_CRT_EVENT_TYPE               const Event,
                                                                LSA_UINT16                        const CycleCnt,
                                                                LSA_UINT8                         const TransferStatus,
                                                                EDDI_LOCAL_DDB_PTR_TYPE           const pDDB);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerSetBuffer( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE  const pConsumer);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetMACIP(EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                        EDDI_CRT_CONSUMER_PTR_TYPE        const pConsumer,
                                                        EDD_MAC_ADR_TYPE          const * const pSrcMAC,
                                                        EDD_IP_ADR_TYPE           const * const pSrcIP);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerSetMACIPACW( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                            EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const pConsumer);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerCheckMACIP( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE    const  pConsumer,
                                                             EDD_MAC_ADR_TYPE            const * const  pSrcMAC,
                                                             EDD_IP_ADR_TYPE             const * const  pSrcIP );
                                                               
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerControlEvent( EDDI_CRT_CONSUMER_PTR_TYPE               const pConsumer,
                                                             EDD_UPPER_RQB_PTR_TYPE                   const pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE                  const pDDB,
                                                             LSA_UINT8                                const ConsumerType );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerRemoveEvent( EDDI_CRT_CONSUMER_PTR_TYPE         pConsumer,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerPassivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE         pConsumer,
                                                          LSA_BOOL                    const  bClearEvents,
                                                          LSA_BOOL                    const  bRemoveACWs);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerIRTtopSM( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                         EDDI_CRT_CONSUMER_PTR_TYPE         const  pConsumer,
                                                         EDDI_CONS_IRTTOP_SM_TRIGGER_TYPE   const  Trigger );


/*---------------------------------------------------------------------------*/
/* CRT-Remote-Provider_Surveillance-Functions                                */
/*---------------------------------------------------------------------------*/

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsInit( EDDI_CRT_RPS_TYPE             * const pRps,
                                                EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsTrigger( EDDI_CRT_RPS_TYPE             * const pRps,
                                                   EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                   EDDI_CRT_RPS_EVENT              const RpsEvent );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsInformStateMachine( EDDI_CRT_RPS_TYPE        *  const  pRps,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                              EDDI_CRT_RPS_EVENT          const  RpsEvent );

LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsIsStopped( EDDI_CRT_RPS_TYPE              const * const pRps );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRpsFillAndSendIndication( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  PendingEventMaxConsID123, 
                                                                   LSA_UINT32               const  PendingEventMaxConsIDDFP,
                                                                   LSA_BOOL                 const  bDirectIndication);

#if defined (EDDI_CFG_USE_SW_RPS)
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsCheckDataStatesEventDummy( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                     EDDI_CRT_RPS_TYPE             * const pRps );
#else
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerGetKRAMDataStatus( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                  LSA_UINT8                      *  const  pTransferStatus,
                                                                  EDDI_SER_DATA_STATUS_TYPE      *  const  pDataStatus,
                                                                  LSA_UINT16                     *  const  pCycleCnt );
#endif

EDDI_CRT_EVENT_TYPE EDDI_LOCAL_FCT_ATTR EDDI_CRTRpsCalculateIndEntry( EDDI_CRT_CONSUMER_PTR_TYPE      const pConsumer,
                                                                      LSA_UINT32                      const SBEntryValue,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE         const pDDB);

#if defined (EDDI_CFG_REV5)
LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_CRTDataImage( LSA_UINT16                        const FCode,
                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const pConsumer,
                                                  EDDI_LOCAL_DDB_PTR_TYPE           const pDDB );
#endif

/*---------------------------------------------------------------------------*/
/* CRT-Consumer Scoreboard State machine                                     */
/*---------------------------------------------------------------------------*/

LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerScoreBoardSM( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                              EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                              EDDI_CONS_SCOREBOARD_SM_TRIGGER_TYPE  const  Trigger );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_EXT_H


/*****************************************************************************/
/*  end of file eddi_crt_ext.h                                               */
/*****************************************************************************/
