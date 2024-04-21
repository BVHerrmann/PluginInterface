
#ifndef EDDS_SRT_EXT_H                        /* ----- reinclude-protection ----- */
#define EDDS_SRT_EXT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_srt_ext.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  03.06.02    JS    initial version.                                       */
/*  17.10.03    JS    added parameter SetToUnknownState                      */
/*  27.11.03    JS    added EDDS_RTProviderChangePhase                       */
/*  20.01.04    JS    added EDDS_RTProviderGetCycleCounter                   */
/*  21.01.04    JS    Changed CycleBaseFactor and ReductionRatio interpretion*/
/*  15.06.04    JS    removed ASRT-functions                                 */
/*  24.11.04    JS    added AcceptDataInvalid parameter                      */
/*  11.10.05    JS    changed a memory attribute to avoid compiler warning   */
/*  10.11.05    JS    Added CycleCounter                                     */
/*  19.05.06    JS    Added EDDS_RTSendclockChange                           */
/*  30.06.06    JS    Added EDDS_RTGetParams                                 */
/*  30.01.09    JS    consumer events changed from UINT16 to UINT32          */
#endif
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTInitComponent(
        EDD_UPPER_RQB_PTR_TYPE pRQB,
        EDDS_LOCAL_DDB_PTR_TYPE pDDB);

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTRelComponent(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTSendclockChange(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRequestCyclic(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTCloseChannelCyclic(
	EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTOpenChannelCyclic(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxReadyCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_RESULT                  Status);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxReadyCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_RESULT                  Status);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxQueryCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE                           pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR       *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pLength);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxQueryCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRxIndicateCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE                             pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR      *   pBuffer,
    LSA_UINT32                                          Length,
    LSA_UINT16                                          FrameID,
    LSA_BOOL                                            VLANTagPresent);

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRxIndicateCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE                             pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR      *   pBuffer,
    EDD_UPPER_MEM_PTR_TYPE                              pUDPData,
    EDDS_IP_HEADER_TYPE                             *   pHeader,
    LSA_UINT32                                          UDPLen,
    LSA_UINT16                                          FrameID);
#endif

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderADD(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderRemove(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderControl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderChangePhase(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderControlLow(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRT,
    LSA_UINT16                      ProvID,
    LSA_BOOL                        Activate,
    LSA_UINT16                      Mask,
    LSA_UINT16                      Status);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControlLow(
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB,
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE    pCSRT,
    LSA_UINT16                          ConsID,
    LSA_UINT16                          Mode);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRemove(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerAdd(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerConfirmIndication(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerIndicationFinish(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE    pCSRTComp);

LSA_EXTERN LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerIndicationFill(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
    LSA_UINT32                  UserID,
    LSA_UINT32                  Event,
    LSA_UINT32                  EventStatus,
    LSA_UINT16                  CycleCounter);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetCycleCounter(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
    EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter);


LSA_EXTERN LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTGetAndCheckReductionFactor(
    LSA_UINT16  ReductionRatio,
    LSA_BOOL    Udp);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTKnotGet(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
    LSA_UINT32                  KnotTableIdx);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTKnotFree(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
    LSA_UINT32                  KnotTableIdx);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderStop(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRT,
    EDDS_CSRT_PROVIDER_TYPE_PTR     pProv);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerStop(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE           pCSRT,
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR     pConsFrame);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCheckTimerSwitch(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE           pCSRT);

LSA_EXTERN LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTCalculateCycleStampAjdustShift(
    LSA_UINT16 CycleBaseFactor);

LSA_EXTERN LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTCalculateSchedulerReduction(
    LSA_UINT16 CycleBaseFactor, LSA_UINT16 schedulerCycle);

/*****************************************************************************/
/*  end of file srt_ext.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SRT_EXT_H */
