#ifndef EDDI_SWI_EXT_H          //reinclude-protection
#define EDDI_SWI_EXT_H

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
/*  F i l e               &F: eddi_swi_ext.h                            :F&  */
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

/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* eddi_gen.c                                                                */
/*---------------------------------------------------------------------------*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenChangeUsrPortID2HwPortIndex( LSA_UINT16                const UsrPortID,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                                    LSA_UINT32              * const pHwPortIndex );

/*---------------------------------------------------------------------------*/
/* eddi_swi_usr.c                                                            */
/*---------------------------------------------------------------------------*/
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrIniTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );


LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );


LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );


LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrRequest      ( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                         EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrCloseChannel ( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );


LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrClearFDBRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrClearFDBGetNextRequestAndExecute( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
/*---------------------------------------------------------------------------*/
/* eddi_swi_port.c                                                           */
/*---------------------------------------------------------------------------*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetPortState( EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE  const  pRQBSetPortStat,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPortGetPortState( EDD_UPPER_SWI_GET_PORT_STATE_PTR_TYPE         pRQBGetPortStat,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetSpMoConfig( EDDI_SWI_SET_PORT_SPEEDMODE_PTR_TYPE  const  pUsrSetPortSpeedMode,
                                                          EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPortFlushFilterDB( EDD_UPPER_RQB_PTR_TYPE             pRQB,
                                                          LSA_BOOL                 *  const  bIndicate,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB );

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetPortMonitor( EDDI_UPPER_SWI_SET_PORT_MONITOR_PTR_TYPE  const  pUsrSetPortMonitor,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortGetPortMonitor( EDDI_UPPER_SWI_GET_PORT_MONITOR_PTR_TYPE  const  pUsrGetPortMonitor,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortGetMirrorPort( EDDI_UPPER_SWI_GET_MIRROR_PORT_PTR_TYPE  const  pGetMirrorPort,
                                                        EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetMirrorPort( EDDI_UPPER_SWI_SET_MIRROR_PORT_PTR_TYPE  const  pUsrSetMirrorPort,
                                                          EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB );
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPortResetSQTimeout( LSA_VOID * const context);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortResetSQTimeoutConf( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetSpMoNRTMAC( LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT8                const  LinkSpeed,
                                                          LSA_UINT8                const  LinkMode,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetSpanningTree( LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT16               const  PortIDState,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPortSetiSRTMode( LSA_UINT32               const  HwPortIndex,
                                                      LSA_BOOL                 const  Enable_iSRT,
                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetPhyStatus( LSA_UINT32               const  HwPortIndex,
                                                         LSA_UINT8                const  PhyPower,
                                                         LSA_BOOL                 const  bGenInd,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetDropCtr( LSA_UINT32              const HwPortIndex,
                                                         EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

/*---------------------------------------------------------------------------*/
/* eddi_swi_xxx.c                                                            */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SWILowWaterTrigger( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                      SWI_LOW_WATER_EVENT_TYPE const Event );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SWILinkInterrupt( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                    LSA_UINT32              const para_1 );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SWISetAgeTime( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                   EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SWIGetAgeTime( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                   EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SWIAgeTimeout( LSA_VOID * const context );

#if defined (EDDI_CFG_FRAG_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetEnableSRTPrio( LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  Enable_SRTPrio,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
#endif

#if defined (EDDI_CFG_ENABLE_MC_FDB)
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERSetMCFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                      CONST_SER_HANDLE_PTR                              pSWIPara,
                                                      EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE       p_MC_RQB_FDBEntry );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERRemoveMCFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE                  const pDDB,    
                                                           CONST_SER_HANDLE_PTR                                 pSWIPara,
                                                           EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE       p_MC_RQB_FDBEntry );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERGetMCFDBEntryID( EDDI_LOCAL_DDB_PTR_TYPE                     const pDDB,
                                                        CONST_SER_HANDLE_PTR                              const pSWIPara,
                                                        EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE const p_MC_FDBEntry,
                                                        LSA_UINT32                                  const Index );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERGetMCFDBEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                   const pDDB,
                                                         CONST_SER_HANDLE_PTR                                  pSWIPara,
                                                         EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE       p_MC_FDBEntry );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetBLMC( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                LSA_UINT32                          BLMask,
                                                EDD_MAC_ADR_TYPE    const *  const  pMACBegin,
                                                LSA_UINT32                   const  MacLen );
#endif //(EDDI_CFG_ENABLE_MC_FDB)

/*---------------------------------------------------------------------------*/
/* eddi_swi_uc_fdb.c                                                         */
/*---------------------------------------------------------------------------*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERSetFDBEntryRun( CONST_SER_HANDLE_PTR                             pSWIPara,
                                                       EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE      pUsrFDBEntry,
                                                       LSA_BOOL                             const bCheckPN,
                                                       EDDI_LOCAL_DDB_PTR_TYPE                    pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_SERSetFDBEntryConfig( CONST_SER_HANDLE_PTR                        pSWIPara,
                                                         EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE pUsrFDBEntry,
                                                         EDDI_LOCAL_DDB_PTR_TYPE               pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERRemoveFDBEntry( CONST_SER_HANDLE_PTR                           pSWIPara,
                                                       EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE pFDBEntryRQB,
                                                       EDDI_LOCAL_DDB_PTR_TYPE const            pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERGetFDBEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE          const    pDDB,
                                                       CONST_SER_HANDLE_PTR                            pSWIPara,
                                                       EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE pFDBEntry );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetFDBSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                   CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                   LSA_UINT32                   *  pMACSizeTable );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERGetFDBEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE           const     pDDB,
                                                         CONST_SER_HANDLE_PTR                    const     pSWIPara,
                                                         EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE pFDBEntry );

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcCompareMac( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p1,
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p2 );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUcSetFDBDestPort( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                       LSA_UINT8              const bRQBCHA,
                                                       LSA_UINT8              const bRQBCHB,
                                                       LSA_UINT8      const * const pRQBPortID,
                                                       LSA_UINT8                  * pFDBDestPort );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUcGetFDBDestPort( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                       LSA_UINT8              const FDBDestPort,
                                                       LSA_UINT8                  * pRQBCHA,
                                                       LSA_UINT8                  * pRQBCHB,
                                                       LSA_UINT8                  * pRQBPortID );

/*==========================================================================*/
/* PHY-SMI-Function from eddi_swi_phy.c                                     */
/*==========================================================================*/
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyEnableLinkIRQ( LSA_UINT32               const  HwPortIndex,
                                                         LSA_BOOL                 const  bEnableLink,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyGetLinkState( SER_SWI_LINK_PTR_TYPE         pBasePx,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB  );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyAction4AutoNegOn2( LSA_UINT32               HwPortIndex,
                                                             SER_SWI_LINK_PTR_TYPE    pBasePx,
                                                             EDDI_LOCAL_DDB_PTR_TYPE   const pDDB);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPhySetSpMo( LSA_UINT32 const HwPortIndex,
                                                   LSA_BOOL   const AutoNeg,
                                                   LSA_UINT8  const LinkSpeed,
                                                   LSA_UINT8  const LinkMode,
                                                   EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyReadMDCA(  LSA_UINT16               const Adress_Register,
                                                   LSA_UINT16               const PhyAddr,
                                                   LSA_UINT32               * pMD_Data,
                                                   EDDI_LOCAL_DDB_PTR_TYPE   const   pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyWriteMDCA( LSA_UINT16               const Adress_Register,
                                                   LSA_UINT16               const PhyAddr,
                                                   LSA_UINT32               const * const pMD_Data,
                                                   EDDI_LOCAL_DDB_PTR_TYPE  const   pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyActionForLinkDown( LSA_UINT32              const HwPortIndex,
                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyActionForLinkUp( LSA_UINT32              const HwPortIndex,
                                                         LSA_UINT16              const LinkSpeedMode_Config,
                                                         SER_SWI_LINK_PTR_TYPE   const pBasePx,
                                                         EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyIsFiber( LSA_UINT32              const HwPortIndex,
                                                 EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

#if defined (EDDI_CFG_ERTEC_200)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200Reset( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200ConfigPHY( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200SetPhyExtern( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyPhaseShift_Timer( LSA_VOID  *  const  context );
#endif

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyAreAllPhysON( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
#endif

/*---------------------------------------------------------------------------*/
/* eddi_swi_misc.c                                                           */
/*---------------------------------------------------------------------------*/
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscSetClosedDisabled( LSA_UINT32 const HwPortIndex,
                                                              LSA_BOOL   const Closed,
                                                              LSA_BOOL   const Disabled,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscGetClosedDisabled( LSA_UINT32 const HwPortIndex,
                                                              LSA_BOOL*        pClosed,
                                                              LSA_BOOL*        pDisabled,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscSetDisable( LSA_UINT32              const HwPortIndex,
                                                       LSA_BOOL                const bDisabled,
                                                       EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscSetRegisterNoSwap( LSA_UINT32              const HwPortIndex,
                                                              LSA_UINT32              const RegType,
                                                              LSA_UINT32              const RegisterVal,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscSetRegister( LSA_UINT32              const HwPortIndex,
                                                        LSA_UINT32              const RegType,
                                                        LSA_UINT32              const RegisterVal,
                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscGetRegister( LSA_UINT32              const HwPortIndex,
                                                        LSA_UINT32              const RegType,
                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscGetRegisterNoSwap( LSA_UINT32              const HwPortIndex,
                                                              LSA_UINT32              const RegType,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscResetPortSendQueue( LSA_UINT32              const HwPortIndex,
                                                               EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscChangeSpMoConfigToSpMoPhy( EDDI_LOCAL_DDB_PTR_TYPE const pDDB, 
                                                                      LSA_UINT16              const LinkSpeedModeConfig,
                                                                      LSA_BOOL                    * pbAutoNeg,
                                                                      LSA_UINT8                   * pLinkSpeed,
                                                                      LSA_UINT8                   * pLinkMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_InsertBottomToDynList( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                             EDDI_DYN_LIST_HEAD_PTR_TYPE const pListHeader,
                                                             LSA_VOID_PTR_TYPE           const pData );

LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_GetTopFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                            const pDDB,
                                                        EDDI_DYN_LIST_ENTRY_PTR_TYPE EDDI_LOCAL_MEM_ATTR * const pTop,
                                                        LSA_VOID_PTR_TYPE            EDDI_LOCAL_MEM_ATTR * const ppData );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveBottomFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB, 
                                                               EDDI_DYN_LIST_HEAD_PTR_TYPE                  pListHeader,
                                                               LSA_VOID_PTR_TYPE  EDDI_LOCAL_MEM_ATTR     * ppData );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                          const pDDB,
                                                         EDDI_DYN_LIST_HEAD_PTR_TYPE                      const pListHeader,
                                                         EDDI_DYN_LIST_ENTRY_PTR_TYPE EDDI_LOCAL_MEM_ATTR const pListDataEntry );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_AutoNegCapToLinkSpeedMode( LSA_UINT32                        const HwPortIndex,
                                                               LSA_UINT8   EDDI_LOCAL_MEM_ATTR * const pLinkSpeedMode,
                                                               EDDI_LOCAL_DDB_PTR_TYPE           const pDDB );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_IfLinkSpeedModeInCapability( LSA_UINT32              const HwPortIndex,
                                                                 LSA_UINT32              const LinkSpeedMode,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

/*---------------------------------------------------------------------------*/
/* eddi_swi_ptp.c                                                            */
/*---------------------------------------------------------------------------*/

//This MacAddressgroups are only used internaly by SyncFwd-State-Machines !
#define  EDDI_MAC_ADDR_GROUP_PTCP_ANNOUNCE     0x8000
#define  EDDI_MAC_ADDR_GROUP_PTCP_SYNC_WITH_FU 0x8001
#define  EDDI_MAC_ADDR_GROUP_PTCP_FOLLOW_UP    0x8002
#define  EDDI_MAC_ADDR_GROUP_PTCP_SYNC         0x8003

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPNPNToFDB (EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                EDDI_UPPER_DSB_PTR_TYPE  const pDSB);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPNMCFwdCtrl  (EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE   const pMCFWDCtrlParam,
                                                     EDDI_LOCAL_DDB_PTR_TYPE                 const pDDB, 
                                                     LSA_UINT8                               const blTableIndex );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsCHBMAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                   EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC );

LSA_UINT16  EDDI_LOCAL_FCT_ATTR EDDI_SwiPNGetPrio    (EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                      EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE const pMCMAC );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsDCPMAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                   EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsSTBY1MAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                     EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsCARPMAC( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                    EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsMACInFDB( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                     EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC,    
                                                     LSA_BOOL                           const  bSetInRun );
#if defined (EDDI_CFG_ENABLE_MC_FDB)
LSA_UINT32  EDDI_LOCAL_FCT_ATTR EDDI_SwiPNPNToFDBGetSize( LSA_VOID );
#endif

EDD_MAC_ADR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_SwiPNGetRT3DA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

EDD_MAC_ADR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_SwiPNGetRT3INVALIDDA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

EDD_MAC_ADR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNGetPTPSYNCDA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniStatistic( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SWIStartPhys( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB );

#if defined (EDDI_CFG_REV7)
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiPNSetBL( EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                LSA_UINT32                               BLMask,
                                                EDD_MAC_ADR_TYPE         const  *  const pMACBegin,
                                                LSA_UINT32                         const MacLen );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPNClearAllBL( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPNSetSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                       LSA_UINT16              const Prio );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPNUpdateSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SWI_EXT_H


/*****************************************************************************/
/*  end of file eddi_swi_ext.h                                               */
/*****************************************************************************/
