#ifndef EDDI_SYNC_FWD_STM_H     //reinclude-protection
#define EDDI_SYNC_FWD_STM_H

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
/*  F i l e               &F: eddi_sync_fwd_stm.h                       :F&  */
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

// Controls the SyncId0 - Forwarding - mechanisms for one RxPorts Ports
typedef struct _EDDI_SYNC_FWD_MACHINE
{
    LSA_BOOL    FwdActivity;      // For SyncFrames, SyncWithFrames, FuFrames
    LSA_BOOL    AnnounceActivity; // For AnnounceFrames only
    LSA_BOOL    BLSetFwd;
    LSA_BOOL    BLSetAnno;
    LSA_BOOL    UsrPorts_doSwFwd[EDD_CFG_MAX_PORT_CNT][2];      // SW-forwarding activity for each destination port (UsrPortIds !) (SYNC/SYNC_FU/PTCP)
    LSA_BOOL    FirstRun;

} EDDI_SYNC_FWD_MACHINE;

/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsInit     ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                         LSA_UINT32              const HwPortIndex );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsTrigger  ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                         LSA_UINT32              const HwPortIndex, 
                                                         LSA_UINT8               const blTableIndex );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsGetSwFwd ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                         LSA_BOOL                const bIsRcvBuffered,
                                                         LSA_BOOL                const bSyncID1,
                                                         LSA_UINT16              const RcvUsrPortID,
                                                         LSA_UINT8 *             const doSwFwd );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_FWD_STM_H


/*****************************************************************************/
/*  end of file eddi_sync_fwd_stm.h                                          */
/*****************************************************************************/
