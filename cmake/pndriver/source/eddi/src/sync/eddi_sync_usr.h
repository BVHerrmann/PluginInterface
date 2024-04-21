#ifndef EDDI_SYNC_USR_H         //reinclude-protection
#define EDDI_SYNC_USR_H

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
/*  F i l e               &F: eddi_sync_usr.h                           :F&  */
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
/*  07.12.07    JS    added FrameID defines (moved from sync_snd.c)          */
/*                                                                           */
/*****************************************************************************/

/* FrameIDs used with Sync */
#define EDDI_SRT_FRAMEID_PTCP_SYNC_W_FU_START       0x0020
#define EDDI_SRT_FRAMEID_PTCP_SYNC_W_FU_END         0x0021
#define EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_START      0x0080
#define EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_END        0x0081
#define EDDI_SRT_FRAMEID_PTCP_SYNC_FU_START         0xFF20
#define EDDI_SRT_FRAMEID_PTCP_SYNC_FU_END           0xFF21

#if defined (EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_END) //satisfy lint!
#endif

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SYNCRequest( EDD_UPPER_RQB_PTR_TYPE  pRQB,
                                                 EDDI_LOCAL_HDB_PTR_TYPE pHDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SYNCOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                     EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SYNCCloseChannel( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                      EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sync Send
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndSend( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

#if defined (EDDI_CFG_REV5)
LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncSnd_SyncFrameCCDELCorrection( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                     EDD_UPPER_RQB_PTR_TYPE  const pRQB );
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sync Recv
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvRecv             ( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvNRTDoneHook      ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                             EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                             LSA_BOOL                const bSyncID1 );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sync port state machines
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsPrmChange   (       EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                   LSA_BOOL                            const  RtClass2_isReservedIntervalPresent,
                                                             const LSA_BOOL                         *  const  pRTSync_isBoundaryIngress,
                                                             const LSA_BOOL                         *  const  pRTSync_isBoundaryEgress,
                                                                   LSA_BOOL                            const  doRtClass3Reset,
                                                                   EDDI_SYNC_TRANSITION_END_FCT_TYPE   const  PrmTransitionEndCbf );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsLinkChange  ( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                             LSA_UINT32              const  HwPortIndex,
                                                             LSA_UINT8               const  LinkStatus,
                                                             LSA_UINT8               const  LinkSpeed,
                                                             LSA_UINT8               const  LinkMode );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsRemoteChange( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                             LSA_UINT32              const  HwPortIndex,
                                                             LSA_UINT32              const  RTSync_TopoState,
                                                             LSA_UINT32              const  RtClass2_TopoState,
                                                             LSA_UINT32              const  RtClass3_TopoState,
                                                             LSA_UINT32              const  RtClass3_PDEVTopoState,
                                                             LSA_UINT32              const  RtClass3_RemoteState);

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsSyncChange  ( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                             LSA_BOOL                const  isLocalSyncOk );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsRateChange  ( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                             LSA_BOOL                const  isRateValid );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsLinedelayChange( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                                LSA_UINT32              const  HwPortIndex,
                                                                LSA_UINT32              const  LineDelayInTicksMeasured,
                                                                LSA_UINT32              const  CableDelayInNsMeasured );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsSendClockChange( EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                                 EDDI_SYNC_TRANSITION_END_FCT_TYPE  const SndClkChTransitionEndCbf );

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsIsIrtSyncSndActive ( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex);

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTransitionBegin( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTransitionEndCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
#endif

/*typedef  LSA_VOID  (EDDI_LOCAL_FCT_ATTR  *  EDDI_SYNC_TRANSITION_END_FCT_TYPE)(
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_USR_H


/*****************************************************************************/
/*  end of file eddi_sync_usr.h                                              */
/*****************************************************************************/
