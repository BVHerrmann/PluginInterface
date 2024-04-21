#ifndef EDDI_CRT_COM_H          //reinclude-protection
#define EDDI_CRT_COM_H

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
/*  F i l e               &F: eddi_crt_com.h                            :F&  */
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
/*                                                                           */
/*****************************************************************************/

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListReserveFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                     EDDI_CRT_PROVIDER_PTR_TYPE       * const  ppProvider,
                                                                     EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE   const  pCRT,
                                                                     LSA_UINT8                          const  ProviderType,
                                                                     LSA_BOOL                           const  bIsDfp );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListUnReserveEntry( EDDI_CRT_PROVIDER_PTR_TYPE        const  pProvider,
                                                                 EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT,
                                                                 LSA_BOOL                          const  bIsDfp );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderInit( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                     EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                     EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE const  pProvParam,
                                                     LSA_UINT8                            const  ListType,
                                                     LSA_UINT8                            const  ProviderType );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListReserveFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                       EDDI_CRT_CONSUMER_PTR_TYPE        *  const  ppConsumer,  //OUT
                                                                       EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE     const  pCRT,
                                                                       LSA_UINT8                            const  ListType,
                                                                       LSA_BOOL                             const  bIsDfp );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerListUnReserveEntry( EDDI_CRT_CONSUMER_PTR_TYPE       const  pConsumer,
                                                                 EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE const  pCRT,
                                                                 LSA_BOOL                         const  bIsDfp );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProvInit( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                 EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                 LSA_UINT8                   const  ProviderType );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTSetActivityAllFWD( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                        LSA_UINT32              const HwPortIndex,
                                                        LSA_BOOL                const bActivate );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckForConsumerRedundantFrames( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                          EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                                          EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckForProviderRedundantFrames( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                                          EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                                          EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetMACAddrRTC3( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB, 
                                                    LSA_UINT32                const  FrameDataProperties,
                                                    LSA_UINT8                     *  MacAdr,
                                                    LSA_UINT16                const  FrameID,
                                                    LSA_BOOL                  const  bIsDFP);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GetMaxProviderParams( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB, 
                                                            LSA_UINT32              *  const ProviderCyclePhaseMaxCnt_Real,
                                                            LSA_UINT32              *  const ProviderCyclePhaseMaxByteCnt_Real);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSetImageMode( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB, 
                                                     void                  *  const pConsProv,
                                                     LSA_BOOL                 const bIsProv);
  
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_CRTIsMACMulticastAddr( EDD_MAC_ADR_TYPE const * const  pSrcMAC );                                                    

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_CRTIsIPMulticastAddr( EDD_IP_ADR_TYPE  const * const  pSrcIP);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetRTClass3MultiCastMACAddr( LSA_UINT8  *  MacAdr);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetFastForwardingMulticastMACAdd( LSA_UINT8       *  MacAdr,
                                                                      LSA_UINT16  const  FrameID);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetProviderMacAndIpAddr( EDDI_CRT_PROVIDER_PTR_TYPE          const  pProvider,
                                                             LSA_UINT16                          const  Properties,
                                                             EDD_MAC_ADR_TYPE            const * const  pDstMAC,
                                                             EDD_IP_ADR_TYPE             const * const  pDstIP);
                                                                          
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckProviderMacAndIpAddr( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                 LSA_UINT16                        const  Properties,
                                                                 EDD_MAC_ADR_TYPE          const * const  DstMAC,
                                                                 EDD_IP_ADR_TYPE           const * const  DstIP,
                                                                 LSA_BOOL                          const  bUndefAllowed);
                                                                          
LSA_RESULT  EDDI_CRTCheckListType( LSA_UINT16  const  Properties, 
                                   LSA_UINT16  const  FrameID,
                                   LSA_UINT8   const  ListType);
                                                                          
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAndSetProviderControlMacAddr( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                                         EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                         EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler);                                                               

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_COM_H


/*****************************************************************************/
/*  end of file eddi_crt_com.h                                               */
/*****************************************************************************/
