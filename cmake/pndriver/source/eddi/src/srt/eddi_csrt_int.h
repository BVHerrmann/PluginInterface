#ifndef EDDI_CSRT_INT_H         //reinclude-protection
#define EDDI_CSRT_INT_H

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
/*  F i l e               &F: eddi_csrt_int.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SRT-                                                                     */
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

/*===========================================================================*/
/*             functions  AutoStop                                           */
/*===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR EDDI_CrtASCtrlSingleProviderList( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                                  EDDI_CRT_PROVIDER_PTR_TYPE const pProvider,
                                                                  LSA_BOOL                   const bRemoveASCoupling, 
                                                                  LSA_BOOL                   const bDeactivateProvider );


LSA_RESULT  EDDI_LOCAL_FCT_ATTR EDDI_CrtASCtrlAllProviderList( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                               EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                               LSA_BOOL                         const bRemoveASCoupling, 
                                                               LSA_BOOL                         const bDeactivateProvider );


void  EDDI_LOCAL_FCT_ATTR EDDI_CrtASAddSingleProviderToEndList( EDDI_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                      EDDI_CRT_PROVIDER_PTR_TYPE const pProvider );


LSA_BOOL  EDDI_LOCAL_FCT_ATTR EDDI_CrtASIsProviderInList( EDDI_CONST_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                          EDDI_CONST_CRT_PROVIDER_PTR_TYPE const pProvider );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderIRTtopSM( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB, 
                                                       EDDI_CRT_PROVIDER_PTR_TYPE       const pProvider, 
                                                       EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE const Trigger );

LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderIRTtopSMStateCheck( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                                    EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider, 
                                                                    EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE  const  Trigger );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderPassivateAUX( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                           EDDI_CONST_CRT_PROVIDER_PTR_TYPE const pProvider );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderSetActivity( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                          EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const pProvider, 
                                                          LSA_BOOL                          const bActivate );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTSetActivityFCW (EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                     EDDI_TREE_ELEM_PTR_TYPE           const pTreeElem, 
                                                     LSA_BOOL                          const bActivate,
                                                     LSA_UINT8                         const HwPortIndex);
                                                            
/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/
/* CycleBaseFactor: Defines Cycletime based on formula:  31.25*2^n */
/* with EDDI_SRT_CYCLE_FACTOR_MIN<=n<=EDDI_SRT_CYCLE_FACTOR_MAX    */
//#define EDDI_SRT_CYCLE_FACTOR_MIN   (LSA_UINT16)0x0000
//#define EDDI_SRT_CYCLE_FACTOR_MAX   (LSA_UINT16)0x0010

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* SRT handle (channel) management-structures                                */
/*---------------------------------------------------------------------------*/
//typedef EDDI_LOCAL_MEM_PTR_TYPE  EDDI_LOCAL_HDB_COMP_SRT_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* SRT_DDB_COMP-Structure                                                    */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DDB_COMP_SRT_TYPE
{
    //LSA_UINT16                  CycleBaseFactor;

    //lower Params for SRT (configurable via  ExpertSRTPara):
    LSA_UINT16                    SRT_FCW_Count;      // Count of lower SRT-FCWs (needed for Sending SRT-Telegrams)
    LSA_UINT32                    iSRTRelTime;        // Reloadtime for the SRT-Timeoutcounter [in Ticks]

    // lower Params for SRT:
    EDDI_CCW_CTRL_HEAD_PTR_TYPE   pACWTxHeadCtrl;     // Management of ACWTx-Headers
    EDDI_CCW_CTRL_HEAD_PTR_TYPE   pACWRxHeadCtrl;     // Management of ACWRx-HeaderList
    LSA_UINT16                    ACWRxHeads;         // Number of Heads for AcwRx-Lists
    LSA_UINT16                    ACWRxMask;          // BitMask to calculate Header-Idx for SRT-Rx-List

    //EDDI_ACW_BASE_PTR_TYPE      pACWDevBase;        // Ptr to management of  ACW-HeaderList (EDDI-sight)

    EDDI_SER_ACW_HEAD_PTR_TYPE    pACWSerBase;        // Ptr to HW-ACW-HeaderList (EDDI-sight)

} EDDI_DDB_COMP_SRT_TYPE;

typedef struct _EDDI_DDB_COMP_SRT_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_SRT_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CSRT_INT_H


/*****************************************************************************/
/*  end of file eddi_csrt_int.h                                              */
/*****************************************************************************/
