#ifndef EDDI_EXT_H              //reinclude-protection
#define EDDI_EXT_H

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
/*  F i l e               &F: eddi_ext.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  17.02.04    ZR    - add icc in Device-Description-structure              */
/*                    - EDDI_COMP_ICC added                                  */
/*                    for new icc modul                                      */
/*  26.11.07    JS    added EDDI_GenGetDestPortMCDef                         */
/*                                                                           */
/*****************************************************************************/

//-------------------------------------------------------------------------------------------
//  declarations
//-------------------------------------------------------------------------------------------
#define EDDI_CYC_NO_EXT_PLL_IN_SIGNAL 0x7FFFFFFF

//-------------------------------------------------------------------------------------------
//  Function Header for eddi_gen.c
//-------------------------------------------------------------------------------------------

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenLinkInterrupt( LSA_BOOL                const * const pPortChanged,
                                                      EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenDelAllDynMCMACTab( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenLEDOnOffDuration( LSA_VOID * const context );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_GenGetMaxReservedIntEnd (EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenGetDestPortMCDef( EDDI_LOCAL_MAC_ADR_PTR_TYPE           const pMACAdr,
                                                         LSA_BOOL                                  * pDestSameMCDef,
                                                         LSA_BOOL                                  * pValid,
                                                         EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE       pUsrSetEntryFDB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE               const pDDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenCalcStatistics( EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                         LSA_UINT8                    const  RawUsed,   
                                                         LSA_UINT8                    const  Reset,   
                                                         EDD_GET_STATISTICS_RAW_TYPE      *  pRaw,
                                                         EDD_GET_STATISTICS_MIB_TYPE      *  pMIB,
                                                         LSA_UINT16                   const  PortID,
                                                         LSA_BOOL                     const  bUseShadowRegister );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenCalcTransferEndValues( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                              EDD_RESERVED_INTERVAL_IF_RED_TYPE  *         pTransferEndValues,                                                          
                                                              EDDI_PRM_RECORD_IRT_PTR_TYPE                 pPDirDataRecordSet,
                                                              EDDI_PRM_PDIR_APPLICATION_DATA_PTR_TYPE      pPDirApplicationDataRecordSet,
                                                              EDDI_PRM_STATE                        const  PDirDataRecordState,
                                                              EDDI_PRM_STATE                        const  PDirApplicationDataRecordState);
//-------------------------------------------------------------------------------------------
//  Function Header for eddi_dev.c
//-------------------------------------------------------------------------------------------

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GetDDB( const EDDI_HANDLE                             const hDDB,
                                            EDDI_LOCAL_DDB_PTR_TYPE EDDI_LOCAL_MEM_ATTR * const ppDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CreateDDB( EDDI_LOCAL_DDB_PTR_TYPE EDDI_LOCAL_MEM_ATTR * const ppDDB,
                                               LSA_UINT32                                    const IRTE_SWI_BaseAdr );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CloseDDB( const EDDI_HANDLE const hDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_HandleNew( LSA_HANDLE_TYPE EDDI_LOCAL_MEM_ATTR     * pHandle,
                                               EDDI_LOCAL_HDB_PTR_TYPE                 * ppHDB,
                                               EDDI_LOCAL_DDB_PTR_TYPE             const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenRequest( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_HandleRel( EDD_HANDLE_LOWER_TYPE const Handle );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_HandleGetHDB( EDD_HANDLE_LOWER_TYPE                         const Handle,
                                                  EDDI_LOCAL_HDB_PTR_TYPE EDDI_LOCAL_MEM_ATTR * const ppHDB );

//-------------------------------------------------------------------------------------------
//  Function Header for eddi_g_r6.c
//-------------------------------------------------------------------------------------------

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenR6Ini( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif

//-------------------------------------------------------------------------------------------
//  Function Header for eddi_usr.c
//-------------------------------------------------------------------------------------------

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_DeviceClosePart2( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                      EDD_UPPER_RQB_PTR_TYPE  const pRQB );

//-------------------------------------------------------------------------------------------
//  Function Header for eddi_cyc.c
//-------------------------------------------------------------------------------------------

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycInit(       EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE   const pCyclComp,
                                             const EDDI_CRT_INI_TYPE                  * const pCrtIniCfgPara,
                                                   EDDI_LOCAL_DDB_PTR_TYPE              const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CycSendclockChange( EDD_UPPER_RQB_PTR_TYPE    const pRQB,
                                                        EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                        LSA_BOOL                * const pbIndicate );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycSndClkChTransitionDoneCbf( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CycSetSyncTime( EDD_UPPER_RQB_PTR_TYPE    const pRQB,
                                                    EDDI_LOCAL_HDB_PTR_TYPE   const pHDB,
                                                    LSA_BOOL                * const pbIndicate );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycSetAdjustInterval( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                          EDDI_DDB_CYC_COUNTER_TYPE     * pCycCount,
                                                          LSA_INT32                 const AdjustInterval );

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycSetDriftCorrection( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                           LSA_INT32               const DriftInterval );
#endif

EDDI_CYC_COUNTER_STATE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_CycUpdateSoftwareCounter( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                                 EDDI_SYNC_GET_SET_TYPE   *  const  pSyncGet,
                                                                                 LSA_UINT32                * const  pExtPLLTimeStampInTicks);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycTimerCbfSoftwareCounter( LSA_VOID EDDI_LOCAL_MEM_ATTR * const context );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CycStateMachine( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                     EDDI_DDB_CYC_COUNTER_TYPE       * pCycCount,
                                                     EDDI_CYC_COUNTER_EVENT_TYPE const Event );

LSA_INT32  EDDI_LOCAL_FCT_ATTR EDDI_CycGetRealPhaseOffsetTicks( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

//-------------------------------------------------------------------------------------------
//  Function Header for eddi_oc.c
//-------------------------------------------------------------------------------------------

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_DeferredCloseChannelReq( LSA_VOID * const context );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_GenSetIRTPortStatus( LSA_BOOL                const bAllPort,
                                                       LSA_UINT32              const HwPortIndexIRTPortStatus,
                                                       LSA_UINT8               const IRTPortStatus,
                                                       EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_GenSetRTClass2PortStatus( LSA_BOOL                const bAllPort,
                                                            LSA_UINT32              const PortIndexRTClass2_PortStatus,
                                                            LSA_UINT8               const RTClass2_PortStatus,
                                                            EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_XPLLSetPLLMode( LSA_UINT16              const PllMode,
                                                    EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_EXT_H


/*****************************************************************************/
/*  end of file eddi_ext.h                                                   */
/*****************************************************************************/
