#ifndef EDDI_CRT_CHECK_H        //reinclude-protection
#define EDDI_CRT_CHECK_H

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
/*  F i l e               &F: eddi_crt_check.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  CRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/
 
 
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV5)
#define EDDI_MAX_RELOAD_TIME_VALUE 0x1FFF // Maximum ReloadTime-Value (--> 13bit-Value of ACW / FCW)
#elif defined (EDDI_CFG_REV7)
#define EDDI_MAX_RELOAD_TIME_VALUE 0xFF // Maximum ReloadTime-Value 
#define EDDI_MAX_TSB_SCRR_VALUE (((0x7FFFFUL + 1)/32)-1)
#else
#error EDDI_CFG_ERROR: Revision not defined !
#endif

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                              LSA_UINT8               const ListType,
                                                              LSA_UINT8               const ConsumerType );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                              LSA_UINT8               const ListType,
                                                              LSA_UINT8               const ProviderType );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckClass12Bandwidth( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT,
                                                                LSA_UINT16                        const  CycleReductionRatio,
                                                                LSA_UINT16                        const  CyclePhase,
                                                                LSA_UINT32                        const  ProvDataLen );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckClass12Reduction( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                LSA_UINT32              const MaxAllowedReduction,
                                                                LSA_UINT16              const CycleReductionRatio,
                                                                LSA_UINT16              const CyclePhase,
                                                                LSA_BOOL                const bUndefAllowed);
typedef enum _CRT_MIRROR_TYPE
{
    CRT_MIRROR_ADD    = 0x11,
    CRT_MIRROR_REMOVE = 0x33

} CRT_MIRROR_TYPE;

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckAddRemoveMirror( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                           LSA_UINT16              const DataLen,
                                                           LSA_UINT32              const DataOffset,
                                                           LSA_UINT8               const ListType,
                                                           LSA_BOOL                const bProvider,
                                                           CRT_MIRROR_TYPE         const Action );

#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerXchangeCheckRQB( EDD_UPPER_CSRT_CONSUMER_XCHANGE_BUF_PTR_TYPE const pParam,
                                                                EDDI_CONST_CRT_CONSUMER_PTR_TYPE             const pConsumer,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderXchangeCheckRQB( EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE const pParam,
                                                                EDDI_CONST_CRT_PROVIDER_PTR_TYPE             const pProvider,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB );
#endif

// Defines for RT-Classes:
#define EDDI_RTC1            ((LSA_UINT8) 0)
#define EDDI_RTC2            ((LSA_UINT8) 1)
//#define EDDI_RTC3          ((LSA_UINT8) 2)

// Defines for List-Types:
#define EDDI_LIST_TYPE_SYNC         ((LSA_UINT8) 0)     //SyncFrames
#define EDDI_LIST_TYPE_ACW          ((LSA_UINT8) 1)
#define EDDI_LIST_TYPE_FCW          ((LSA_UINT8) 2)
#define EDDI_LIST_TYPE_UNDEFINED    ((LSA_UINT8) 0xFF)

// Defines for Provider-Types:
#define EDDI_RTC1_PROVIDER                     ((LSA_UINT8) 0)
#define EDDI_RTC2_PROVIDER                     ((LSA_UINT8) 1)
#define EDDI_RTC3_PROVIDER                     ((LSA_UINT8) 2)
#define EDDI_UDP_PROVIDER                      ((LSA_UINT8) 3)
#define EDDI_RTC3_AUX_PROVIDER                 ((LSA_UINT8) 4)
#define EDDI_RT_PROVIDER_TYPE_INVALID          ((LSA_UINT8) 0xFF)

// Defines for Consumer-Types:
#define EDDI_RTC1_CONSUMER                     ((LSA_UINT8) 0)
#define EDDI_RTC2_CONSUMER                     ((LSA_UINT8) 1)
#define EDDI_RTC3_CONSUMER                     ((LSA_UINT8) 2)
#define EDDI_UDP_CONSUMER                      ((LSA_UINT8) 3)
#define EDDI_RTC3_AUX_CONSUMER                 ((LSA_UINT8) 4)
#define EDDI_RT_CONSUMER_TYPE_INVALID          ((LSA_UINT8) 0xFF)

LSA_UINT8                      EDDI_CRTGetListType( LSA_UINT16 const Properties,
                                                    LSA_UINT16 const FrameID );

LSA_UINT8                      EDDI_CRTGetProviderType( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                        EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                        LSA_UINT8               const ListType );

LSA_UINT8                      EDDI_CRTGetConsumerType( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                        EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                        LSA_UINT8               const ListType );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckClass3Reduction( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                             LSA_UINT16              const CycleReductionRatio,
                                                             LSA_UINT16              const CyclePhase,
                                                             LSA_BOOL                const bCheckPhase );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckGetWDDHReloadValue( LSA_UINT8    const ConsumerType,
                                                                LSA_UINT16   const TimerFactor,
                                                                LSA_UINT16   const CycleReductionRatio,
                                                                LSA_UINT32 * const pNewCRR );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_CHECK_H


/*****************************************************************************/
/*  end of file eddi_crt_check.h                                             */
/*****************************************************************************/
