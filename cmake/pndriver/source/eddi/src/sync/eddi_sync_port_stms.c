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
/*  F i l e               &F: eddi_sync_port_stms.c                     :F&  */
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

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_ext.h"
#include "eddi_sync_ir.h"
#include "eddi_sync_usr.h"
#include "eddi_lock.h"
#include "eddi_swi_ext.h"
#include "eddi_crt_com.h"

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#include "eddi_crt_phase_ext.h"
#endif

#define EDDI_MODULE_ID     M_ID_SYNC_PORT_STMS
#define LTRC_ACT_MODUL_ID  137

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define EDDI_DELTA_IN_GLOBAL_INPUT_DATA  (1<<EDDI_MAX_IRTE_PORT_CNT)

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncPortStmsTrigger( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                LSA_UINT32                    delta_selector,
                                                                LSA_UINT8                     blTableIndex );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncPortStmsTriggerFinish( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_GetHwPortsChanged( LSA_UINT32   const delta_occured,
                                                              LSA_BOOL   * const pPortChanged );

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SyncPortStmsGetInputDelta( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                      EDDI_SYNC_PORT_MACHINES_INPUT * const pOldInput,
                                                                      EDDI_SYNC_PORT_MACHINES_INPUT * const pNewInput );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_SyncPortStmsTraceInputDelta( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                        EDDI_SYNC_PORT_MACHINES_INPUT * const pOldInput,
                                                                        EDDI_SYNC_PORT_MACHINES_INPUT * const pNewInput );


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsInit()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A t t e n t i o n:       PortCnt must already be set!                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                         HwPortIndex;
    EDDI_SYNC_PORT_MACHINES  *  const  pPortMachines = &pDDB->SYNC.PortMachines;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsInit->");

    EDDI_ENTER_SYNC_S();

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    pPortMachines->Transition.PrmTransitionEndCbf      = EDDI_NULL_PTR;
    pPortMachines->Transition.SndClkChTransitionEndCbf = EDDI_NULL_PTR;
    pPortMachines->Transition.bTransitionRunning       = LSA_FALSE;
    pPortMachines->Transition.Timeout10ns              = 0;
    pPortMachines->Transition.LastTimeStamp            = 0;
    #endif

    //init global input-data
    pPortMachines->Input.isRateValid                        = LSA_FALSE;
    pPortMachines->Input.isLocalSyncOk                      = LSA_FALSE;
    pPortMachines->Input.RtClass2_isReservedIntervalPresent = LSA_FALSE;

    //init all present ports
    for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortCnt; HwPortIndex++)
    {
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData = &pPortMachines->Input.PortData[HwPortIndex];

        //init port-specific input-data
        pPortInputData->doRtClass3Reset             = LSA_FALSE;
        pPortInputData->FreezeResetMask             = 0;
        pPortInputData->isMauTypeOk                 = LSA_FALSE;
        pPortInputData->isPortStateOk               = LSA_FALSE;
        pPortInputData->RTSync_isLinedelayPresent   = LSA_FALSE;
        pPortInputData->RTSync_isBoundaryIngress[0] = LSA_FALSE;   // Boundary == TRUE  means: This port has a  boundary --> Frames will NOT be forwarded
        pPortInputData->RTSync_isBoundaryIngress[1] = LSA_FALSE;   // Boundary == TRUE  means: This port has a  boundary --> Frames will NOT be forwarded
        // Boundary == FALSE means: This port has no boundary --> Frames will be forwarded
        pPortInputData->RTSync_isBoundaryEgress[0]  = LSA_FALSE;   // Boundary == TRUE  means: This port has a  boundary --> Frames will NOT be forwarded
        pPortInputData->RTSync_isBoundaryEgress[1]  = LSA_FALSE;   // Boundary == TRUE  means: This port has a  boundary --> Frames will NOT be forwarded
        // Boundary == FALSE means: This port has no boundary --> Frames will be forwarded
        pPortInputData->RTSync_TopoState            = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;
        pPortInputData->RtClass2_TopoState          = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;
        pPortInputData->RtClass3_TopoState          = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;
        pPortInputData->RtClass3_PDEVTopoState      = EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH;
        pPortInputData->RtClass3_RemoteState        = EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF;

        //init port-specific machines
        EDDI_IrtClass2StmsInit(pDDB, HwPortIndex);
        EDDI_IrtClass3StmsInit(pDDB, HwPortIndex);
        EDDI_SyncFwdStmsInit  (pDDB, HwPortIndex);
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/*          RtClass2_isReservedIntervalPresent is mandatory                */
/*          RTSync_isBoundary[] is mandatory for all connected ports       */
/*              - index = HwPortIndex!                                     */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsPrmChange(       EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                 LSA_BOOL                            const  RtClass2_isReservedIntervalPresent,
                                                           const LSA_BOOL                         *  const  pRTSync_isBoundaryIngress,
                                                           const LSA_BOOL                         *  const  pRTSync_isBoundaryEgress,
                                                                 LSA_BOOL                            const  doRtClass3Reset,
                                                                 EDDI_SYNC_TRANSITION_END_FCT_TYPE   const  PrmTransitionEndCbf )
{
    LSA_UINT32                                    UsrPortId;
    LSA_UINT32                                    delta_occured = 0;
    LSA_BOOL                                      PortChanged[EDDI_MAX_IRTE_PORT_CNT];
    EDDI_SYNC_PORT_MACHINES_INPUT       *  const  pInput = &pDDB->SYNC.PortMachines.Input;
    EDDI_SYNC_PORT_MACHINES_INPUT                 OldInput;
    LSA_UINT32                             const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_SYNC_PORT_MACHINES_TRANSITION  *  const  pSyncTransition  = &pDDB->SYNC.PortMachines.Transition;

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SyncPortStmsPrmChange->RtClass2_isReservedIntervalPresent:0x%X doRtClass3Reset:0x%X",
                       RtClass2_isReservedIntervalPresent, doRtClass3Reset);

    EDDI_ENTER_SYNC_S();

    //Store Cbf
    if ((PrmTransitionEndCbf != EDDI_NULL_PTR) && (pSyncTransition->PrmTransitionEndCbf != EDDI_NULL_PTR))
    {
        //Caller tries to store new Cbf while old Cbf is still running.
        EDDI_Excp("EDDI_SyncPortStmsPrmChange, pSyncTransition->PrmTransitionEndCbf <> EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP,
                  pSyncTransition->PrmTransitionEndCbf, 0);
        return;
    }

    //save old input data for later comparison
    OldInput = *pInput;

    pInput->RtClass2_isReservedIntervalPresent = RtClass2_isReservedIntervalPresent;

    //check all connected ports
    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                              const  HwPortIndexLoc = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData = &pInput->PortData[HwPortIndexLoc];

        //Set Param
        pPortInputData->RTSync_isBoundaryIngress[0] = pRTSync_isBoundaryIngress[HwPortIndexLoc];
        pPortInputData->RTSync_isBoundaryEgress[0]  = pRTSync_isBoundaryEgress[HwPortIndexLoc];

        if (doRtClass3Reset)
        {
            pPortInputData->doRtClass3Reset       = LSA_TRUE;
            pPortInputData->FreezeResetMask      |= EDDI_SYNC_FREEZE_RESET_FOR_PRM; // FreezeFlag will be removed after callback of pSyncTransition->PrmTransitionEndCbf
            pSyncTransition->PrmTransitionEndCbf  = PrmTransitionEndCbf;           // Cbf        will be removed after callback of pSyncTransition->PrmTransitionEndCbf
        }
        else
        {
            //no change here!
            //acknowledge of reset will be done later by SyncPortStm
        }
    }

    delta_occured = EDDI_SyncPortStmsGetInputDelta(pDDB, &OldInput, pInput);

    //force Triggering of all PortSTMs here by using EDDI_DELTA_IN_GLOBAL_INPUT_DATA
    EDDI_SyncPortStmsTrigger(pDDB, EDDI_DELTA_IN_GLOBAL_INPUT_DATA, EDDI_BL_USR_PRM);

    EDDI_GetHwPortsChanged(delta_occured, &PortChanged[0]);

    //new, incompatible irdata: trigger all affected prov-sm´s
    /*for (HwPortIndex = 0; HwPortIndex < EDDI_MAX_IRTE_PORT_CNT; HwPortIndex++)
    {
        if (PortChanged[HwPortIndex])
        {
            //1. disable TX for all class3-providers on this port
            EDDI_CRTSetActivityAllProv(pDDB, HwPortIndex, LSA_FALSE;
            //2. disable TX for all forwarders on this port
            EDDI_CRTSetActivityAllFWD (pDDB, HwPortIndex, LSA_FALSE );
        }
    } */

    //generate LinkIndExt
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:       all arguments are mandatory!                   */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsLinkChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT8                const  LinkStatus,
                                                            LSA_UINT8                const  LinkSpeed,
                                                            LSA_UINT8                const  LinkMode )
{
    LSA_BOOL                                       isMauTypeOk     = LSA_FALSE;
    LSA_BOOL                                       isPortStateOk   = LSA_FALSE;
    LSA_UINT32                                     delta_occured   = 0;
    LSA_UINT32                              const  delta_port_mask = ((LSA_UINT32)1<<HwPortIndex);
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData  = &(pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex]);

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsLinkChange->");

    if (   (LinkSpeed == EDD_LINK_SPEED_100)
        && (LinkMode  == EDD_LINK_MODE_FULL))
    {
        isMauTypeOk = LSA_TRUE;
    }

    if //port-state not discarding?
       (   (LinkStatus == EDD_LINK_UP)
        || (LinkStatus == EDD_LINK_UP_CLOSED))
    {
        isPortStateOk = LSA_TRUE;
    }

    EDDI_ENTER_SYNC_S();

    if //parameter changed?
       (isMauTypeOk != pPortInputData->isMauTypeOk)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsLinkChange, HwPortIndex:0x%X isMauTypeOk:0x%X", HwPortIndex, isMauTypeOk);

        //update port-specific input-data
        pPortInputData->isMauTypeOk = isMauTypeOk;

        delta_occured = delta_port_mask;
    }

    if //parameter changed?
       (isPortStateOk != pPortInputData->isPortStateOk)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsLinkChange, HwPortIndex:0x%X isPortStateOk:0x%X", HwPortIndex, isPortStateOk);

        //update port-specific input-data
        pPortInputData->isPortStateOk = isPortStateOk;

        delta_occured = delta_port_mask;
    }

    if (delta_occured)
    {
        EDDI_SyncPortStmsTrigger(pDDB, delta_occured, EDDI_BL_USR_GSY_OHA);
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:       not all arguments are mandatory!               */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsRemoteChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  HwPortIndex,
                                                              LSA_UINT32               const  RTSync_TopoState,
                                                              LSA_UINT32               const  RtClass2_TopoState,
                                                              LSA_UINT32               const  RtClass3_TopoState,
                                                              LSA_UINT32               const  RtClass3_PDEVTopoState,
                                                              LSA_UINT32               const  RtClass3_RemoteState)
{
    LSA_UINT32                                     delta_occured   = 0;
    LSA_UINT32                              const  delta_port_mask = ((LSA_UINT32)1<<HwPortIndex);
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData  = &(pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex]);

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsRemoteChange->");

    EDDI_ENTER_SYNC_S();

    if //parameter valid and changed?
       (   (RTSync_TopoState != EDD_SET_REMOTE_PORT_STATE_IGNORE)
        && (RTSync_TopoState != pPortInputData->RTSync_TopoState))
    {

        //update port-specific input-data
        pPortInputData->RTSync_TopoState = RTSync_TopoState;

        //set State for LinkIndication
        if (RTSync_TopoState == EDD_SET_REMOTE_PORT_STATE_TOPO_OK)
        {
            pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].SyncId0_TopoOk = EDD_PORT_SYNCID0_TOPO_OK;
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RTSync_TopoState:0x%X",
                               HwPortIndex, RTSync_TopoState);
        }
        else //EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH
        {
            pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].SyncId0_TopoOk = EDD_PORT_SYNCID0_TOPO_NOT_OK;
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RTSync_TopoState:0x%X",
                               HwPortIndex, RTSync_TopoState);
        }

        delta_occured = delta_port_mask;
    }

    if //parameter valid and changed?
       (   (RtClass2_TopoState != EDD_SET_REMOTE_PORT_STATE_IGNORE)
        && (RtClass2_TopoState != pPortInputData->RtClass2_TopoState))
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass2_TopoState:0x%X",
                           HwPortIndex, RtClass2_TopoState);

        //update port-specific input-data
        pPortInputData->RtClass2_TopoState = RtClass2_TopoState;

        delta_occured = delta_port_mask;
    }

    if //parameter valid and changed?
       (   (RtClass3_TopoState != EDD_SET_REMOTE_PORT_STATE_IGNORE)
        && (RtClass3_TopoState != pPortInputData->RtClass3_TopoState))
    {
        if (EDD_SET_REMOTE_PORT_STATE_TOPO_OK ==  RtClass3_TopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_TopoState:0x%X",
                               HwPortIndex, RtClass3_TopoState);
        }
        else
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_TopoState:0x%X",
                               HwPortIndex, RtClass3_TopoState);
        }

        //update port-specific input-data
        pPortInputData->RtClass3_TopoState = RtClass3_TopoState;

        delta_occured = delta_port_mask;
    }

    if //parameter valid and changed?
       (   (RtClass3_PDEVTopoState != EDD_SET_REMOTE_PORT_STATE_IGNORE)
        && (RtClass3_PDEVTopoState != pPortInputData->RtClass3_PDEVTopoState))
    {

        if (EDD_SET_REMOTE_PORT_STATE_TOPO_OK ==  RtClass3_PDEVTopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_PDEVTopoState:0x%X",
                               HwPortIndex, RtClass3_PDEVTopoState);
        }
        else
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_PDEVTopoState:0x%X",
                               HwPortIndex, RtClass3_PDEVTopoState);
        }

        //update port-specific input-data
        pPortInputData->RtClass3_PDEVTopoState = RtClass3_PDEVTopoState;

        delta_occured = delta_port_mask;
    }

    if //parameter valid and changed?
       (   (RtClass3_RemoteState != EDD_SET_REMOTE_PORT_STATE_IGNORE)
        && (RtClass3_RemoteState != pPortInputData->RtClass3_RemoteState))
    {

        if (EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX ==  RtClass3_RemoteState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_RemoteState:0x%X",
                               HwPortIndex, RtClass3_RemoteState);
        }
        else
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsRemoteChange, HwPortIndex:0x%X RtClass3_RemoteState:0x%X",
                               HwPortIndex, RtClass3_RemoteState);
        }

        //update port-specific input-data
        pPortInputData->RtClass3_RemoteState = RtClass3_RemoteState;

        delta_occured = delta_port_mask;
    }

    if (delta_occured)
    {
        EDDI_SyncPortStmsTrigger(pDDB, delta_occured, EDDI_BL_USR_GSY_OHA);
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:       all arguments are mandatory!                   */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsSyncChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_BOOL                 const  isLocalSyncOk )
{
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsSyncChange->");

    EDDI_ENTER_SYNC_S();

    if //parameter changed?
       (isLocalSyncOk != pDDB->SYNC.PortMachines.Input.isLocalSyncOk)
    {
        if (isLocalSyncOk)
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsSyncChange, GLOBAL isLocalSyncOk:0x%X", isLocalSyncOk);
        }
        else
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsSyncChange, GLOBAL isLocalSyncOk:0x%X", isLocalSyncOk);
        }

        //update global input-data
        pDDB->SYNC.PortMachines.Input.isLocalSyncOk = isLocalSyncOk;

        EDDI_SyncPortStmsTrigger(pDDB, EDDI_DELTA_IN_GLOBAL_INPUT_DATA, EDDI_BL_USR_GSY_OHA);
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsRateChange()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:       all arguments are mandatory!                   */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsRateChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_BOOL                 const  isRateValid )
{
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsRateChange->");

    //Locking already done in EDDI_SYNCReq
    //EDDI_ENTER_SYNC_S();

    if //parameter changed?
       (isRateValid != pDDB->SYNC.PortMachines.Input.isRateValid)
    {
        if (isRateValid)
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsRateChange, GLOBAL isRateValid:0x%X", isRateValid);
        }
        else
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsRateChange, GLOBAL isRateValid:0x%X", isRateValid);
        }

        //update global input-data
        pDDB->SYNC.PortMachines.Input.isRateValid = isRateValid;

        EDDI_SyncPortStmsTrigger(pDDB, EDDI_DELTA_IN_GLOBAL_INPUT_DATA, EDDI_BL_USR_GSY_OHA);
    }

    // Locking already done in EDDI_SYNCReq
    // EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsLinedelayChange()             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:       all arguments are mandatory!                   */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsLinedelayChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT32               const  LineDelayInTicksMeasured,
                                                                 LSA_UINT32               const  CableDelayInNsMeasured )
{
    LSA_UINT32                              const  delta_port_mask = ((LSA_UINT32)1<<HwPortIndex);
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pPortInputData  = &(pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex]);
    LSA_BOOL                                       RTSync_isLinedelayPresent;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsLinedelayChange->");

    EDDI_ENTER_SYNC_S();

    #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
    {
        //if linedelay-change is inside the hysteresis-band, it will be indicated but not traced!
        LSA_INT32  DeltaCableDelay = (LSA_INT32)pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].CableDelayInNsMeasured - (LSA_INT32)CableDelayInNsMeasured;

        if (DeltaCableDelay < 0)
        {
            DeltaCableDelay = -DeltaCableDelay;
        }

        if (DeltaCableDelay <= EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
        {
            pDDB->bFilterLinkIndTrace = LSA_TRUE;
        }
    }
    #endif

    //Store measured LineDelay from GSY
    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksMeasured = LineDelayInTicksMeasured;

    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].CableDelayInNsMeasured = CableDelayInNsMeasured;

    //At ERTEC400 the LineDelay is added when transmitting (not according to standard)
    //At ERTEC200 the LineDelay is added when receiving.
    EDDI_SyncIrUpdateLineDelay(pDDB,
                               (LSA_UINT8)(pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex]),
                               pDDB->PRM.PDIRData.pRecordSet_A);

    RTSync_isLinedelayPresent = (LSA_BOOL)((LineDelayInTicksMeasured == 0) ? LSA_FALSE : LSA_TRUE);

    if //parameter changed?
       (RTSync_isLinedelayPresent != pPortInputData->RTSync_isLinedelayPresent)
    {
        if (!RTSync_isLinedelayPresent)
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncPortStmsLinedelayChange, to 0, HwPortIndex:0x%X", HwPortIndex);
        }                       

        //update port-specific input-data
        pPortInputData->RTSync_isLinedelayPresent = RTSync_isLinedelayPresent;

        EDDI_SyncPortStmsTrigger(pDDB, delta_port_mask, EDDI_BL_USR_GSY_OHA);

        EDDI_EXIT_SYNC_S();

    }
    else
    {
        LSA_BOOL  PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};
 
        PortChanged[HwPortIndex] = LSA_TRUE;

        EDDI_EXIT_SYNC_S(); //execute EDDI_GenLinkInterrupt in REST level 
        
        //Extra LinkIndExt needed (EDDI_SyncPortStmsTrigger will not always be called)
        EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsSendClockChange()             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsSendClockChange( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_SYNC_TRANSITION_END_FCT_TYPE  const  SndClkChTransitionEndCbf )
{
    LSA_UINT32                                        UsrPortId;
    LSA_UINT32                                        HwPortIndex;
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE            pPortInputData;
    LSA_UINT32                                 const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_SYNC_PORT_MACHINES_TRANSITION      *  const  pSyncTransition  = &pDDB->SYNC.PortMachines.Transition;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsSendClockChange->");

    EDDI_ENTER_SYNC_S();

    //Store Cbf
    if (pSyncTransition->SndClkChTransitionEndCbf != EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SyncPortStmsSendClockChange, pSyncTransition->SndClkChTransitionEndCbf <> EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP,
                  pSyncTransition->SndClkChTransitionEndCbf, 0);
        return;
    }

    pSyncTransition->SndClkChTransitionEndCbf = SndClkChTransitionEndCbf;

    //Reset all IRT_PORT_STMs !
    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

        pPortInputData = &pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex];

        pPortInputData->doRtClass3Reset = LSA_TRUE;
        pPortInputData->FreezeResetMask |= EDDI_SYNC_FREEZE_RESET_FOR_SENDCLK; // FreezeFlag will be removed after Callback of pSyncTransition->SndClkChTransitionEndCbf()
    }

    pDDB->SYNC.PortMachines.Input.RtClass2_isReservedIntervalPresent = LSA_FALSE;
    pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin         = 0;
    pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd           = 0;

    EDDI_SyncPortStmsTrigger(pDDB, EDDI_DELTA_IN_GLOBAL_INPUT_DATA, EDDI_BL_USR_GSY_OHA);

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e:                                                */
/*                                                                         */
/***************************************************************************/
//Attention: Fct has to be called under EDDI_ENTER_SYNC_S() TFS2439701
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTrigger( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32                      delta_selector,
                                                                 LSA_UINT8                       blTableIndex )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32         HwPortIndex;
    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    LSA_BOOL    const  bOldCrtPhaseTransitionRunning = EDDI_CrtPhaseAreTransitionsRunning(pDDB);
    #else
    LSA_BOOL    const  bOldCrtPhaseTransitionRunning = LSA_FALSE;
    #endif

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTrigger->bOldCrtPhaseTransitionRunning:0x%X", bOldCrtPhaseTransitionRunning);

    if //global input-data changed?
       (delta_selector & EDDI_DELTA_IN_GLOBAL_INPUT_DATA)
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        //check all connected ports
        for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
        {
            //calculate HwPortIndex
            HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

            //trigger all statemachines
            EDDI_IrtClass2StmsTrigger(pDDB, HwPortIndex);
            EDDI_IrtClass3StmsTrigger(pDDB, HwPortIndex, bOldCrtPhaseTransitionRunning);
            EDDI_SyncFwdStmsTrigger(pDDB, HwPortIndex, blTableIndex);
        }
    }
    else //global input-data unchanged
    {
        for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortCnt; HwPortIndex++)
        {
            LSA_UINT32  const  delta_mask = ((LSA_UINT32)1<<HwPortIndex);

            if //port-specific input-data changed
               (delta_selector & delta_mask)
            {
                //trigger all statemachines
                EDDI_IrtClass2StmsTrigger(pDDB, HwPortIndex);
                EDDI_IrtClass3StmsTrigger(pDDB, HwPortIndex, bOldCrtPhaseTransitionRunning);
                EDDI_SyncFwdStmsTrigger(pDDB, HwPortIndex, blTableIndex);

                delta_selector &= (~delta_mask);

                if (delta_selector == 0)
                {
                    //leave for-loop
                    break;
                }
            }
        }
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if (!pDDB->SYNC.PortMachines.Transition.bTransitionRunning)
    {
        //Finishing can be done now
        EDDI_SyncPortStmsTriggerFinish(pDDB);
    }
    #else
    EDDI_SyncPortStmsTriggerFinish(pDDB );
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsTriggerFinish()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTriggerFinish( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_PORT_MACHINES_TRANSITION  *  const  pSyncTransition = &pDDB->SYNC.PortMachines.Transition;
    LSA_UINT32                                    UsrPortId;
    LSA_UINT32                             const  PortMapCnt      = pDDB->PM.PortMap.PortCnt;
    LSA_BOOL                                      bResetReached;
    LSA_BOOL                                      PortChanged[EDDI_MAX_IRTE_PORT_CNT+1];   //Lint

    for (UsrPortId = 0; UsrPortId < EDDI_MAX_IRTE_PORT_CNT; UsrPortId++)
    {
        PortChanged[UsrPortId] = (UsrPortId < EDD_CFG_MAX_PORT_CNT)?LSA_TRUE:LSA_FALSE;
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTriggerFinish->");

    //Check all connected ports if:
    //- finishing is required
    //- if Reset State is reached for all RtClass3-Machines
    bResetReached = LSA_TRUE;
    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                                 const  HwPortIndex       = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex];

        if (pRtClass3_Machine->bFinishNecessary)
        {
            EDDI_IrtClass3StmsFinish(pDDB, pRtClass3_Machine, HwPortIndex);
        }

        if (pRtClass3_Machine->RtClass3_OutputState != EDDI_RT_CLASS3_STATE_OFF)
        {
            bResetReached = LSA_FALSE;
        }
    }

    if (bResetReached)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTriggerFinish, ResetReached for all RtClass3-Machines");

        //Special Cbf-Function for PRM-COMMIT-Requests
        if (pSyncTransition->PrmTransitionEndCbf != EDDI_NULL_PTR)
        {
            EDDI_SYNC_TRANSITION_END_FCT_TYPE  const  Cbf = pSyncTransition->PrmTransitionEndCbf;

            //delete cbf-reference before(!) call to avoid recursion
            pSyncTransition->PrmTransitionEndCbf = EDDI_NULL_PTR;

            //Remove FreezeFlags
            for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
            {
                LSA_UINT32              const  HwPortIndex    = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
                EDDI_SYNC_PORT_DATA  *  const  pPortInputData = &pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex];

                pPortInputData->FreezeResetMask &= (~EDDI_SYNC_FREEZE_RESET_FOR_PRM);
            }

            EDDI_EXIT_SYNC_S();  //intentionally inverse locking here!
            Cbf(pDDB);
            EDDI_ENTER_SYNC_S(); //intentionally inverse locking here!
        }

        //Special Cbf-Function for SendClockChange-Requests
        if (pSyncTransition->SndClkChTransitionEndCbf != EDDI_NULL_PTR)
        {
            EDDI_SYNC_TRANSITION_END_FCT_TYPE  const  Cbf = pSyncTransition->SndClkChTransitionEndCbf;

            //delete cbf-reference before(!) call to avoid recursion
            pSyncTransition->SndClkChTransitionEndCbf = EDDI_NULL_PTR;

            //Remove FreezeFlags
            for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
            {
                LSA_UINT32              const  HwPortIndex    = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
                EDDI_SYNC_PORT_DATA  *  const  pPortInputData = &pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex];

                pPortInputData->FreezeResetMask &= (~EDDI_SYNC_FREEZE_RESET_FOR_SENDCLK);
            }

            EDDI_EXIT_SYNC_S();  //intentionally inverse locking here !
            Cbf(pDDB);
            EDDI_ENTER_SYNC_S(); //intentionally inverse locking here !
        }
    }

    //check and gen LinkIndExt if neccessary
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncPortStmsIsIrtSyncSndActive()          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_BOOL                                       */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsIsIrtSyncSndActive( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex )
{
    EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &(pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex]);

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsIsIrtSyncSndActive->HwPortIndex:0x%X", HwPortIndex);

    if (pRtClass3_Machine->RtClass3_OutputState == EDDI_RT_CLASS3_STATE_OFF)
    {
        return LSA_FALSE;
    }
    else
    {
        return LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
**************************************************************************
**************************************************************************
**************************************************************************
**************************************************************************
**************************************************************************

                                HelperFunctions

**************************************************************************
**************************************************************************
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/


/**************************************************************************
 * F u n c t i o n:
 *
 * D e s c r i p t i o n:
 *     Gets the global and port specific delta of two
 *     EDDI_SYNC_PORT_MACHINES_INPUT - Structs
 *
 * A r g u m e n t s:
 *
 * R e t u r n   V a l u e:
 *
 **************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsGetInputDelta( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                         EDDI_SYNC_PORT_MACHINES_INPUT  *  const  pOldInput,
                                                                         EDDI_SYNC_PORT_MACHINES_INPUT  *  const  pNewInput )
{
    LSA_UINT32         delta_occured = 0;
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    if (( pOldInput->isRateValid                         !=  pNewInput->isRateValid                        ) ||
        ( pOldInput->isLocalSyncOk                       !=  pNewInput->isLocalSyncOk                      ) ||
        ( pOldInput->RtClass2_isReservedIntervalPresent  !=  pNewInput->RtClass2_isReservedIntervalPresent ))
    {
        delta_occured |= EDDI_DELTA_IN_GLOBAL_INPUT_DATA;
    }

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                              const   HwPortIndex       = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const   pOldPortInputData = &(pOldInput->PortData[HwPortIndex]);
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const   pNewPortInputData = &(pNewInput->PortData[HwPortIndex]);

        if (( pOldPortInputData->doRtClass3Reset            !=  pNewPortInputData->doRtClass3Reset             ) ||
            ( pOldPortInputData->isMauTypeOk                !=  pNewPortInputData->isMauTypeOk                 ) ||
            ( pOldPortInputData->isPortStateOk              !=  pNewPortInputData->isPortStateOk               ) ||
            ( pOldPortInputData->RTSync_isLinedelayPresent  !=  pNewPortInputData->RTSync_isLinedelayPresent   ) ||
            ( pOldPortInputData->RTSync_isBoundaryIngress[0]   !=  pNewPortInputData->RTSync_isBoundaryIngress[0]    ) ||
            ( pOldPortInputData->RTSync_isBoundaryEgress[0]    !=  pNewPortInputData->RTSync_isBoundaryEgress[0]     ) ||
            ( pOldPortInputData->RTSync_TopoState           !=  pNewPortInputData->RTSync_TopoState            ) ||
            ( pOldPortInputData->RtClass2_TopoState         !=  pNewPortInputData->RtClass2_TopoState          ) ||
            ( pOldPortInputData->RtClass3_TopoState         !=  pNewPortInputData->RtClass3_TopoState          ) ||
            ( pOldPortInputData->RtClass3_PDEVTopoState     !=  pNewPortInputData->RtClass3_PDEVTopoState      ) ||
            ( pOldPortInputData->RtClass3_RemoteState       !=  pNewPortInputData->RtClass3_RemoteState        ))
        {
            delta_occured |= ((LSA_UINT32)1<<HwPortIndex) ;
        }
    }

    if (delta_occured)
    {
        EDDI_SyncPortStmsTraceInputDelta(pDDB, pOldInput, pNewInput);
    }

    return delta_occured;
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
 * F u n c t i o n: EDDI_SyncPortStmsTraceInputDelta()
 *
 * D e s c r i p t i o n:
 *  Print out all Changes of Parameters between pOldInput and pOldInput
 *
 * A r g u m e n t s:
 *
 * R e t u r n   V a l u e: LSA_VOID
 *
 **************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTraceInputDelta( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                         EDDI_SYNC_PORT_MACHINES_INPUT  *  const  pOldInput,
                                                                         EDDI_SYNC_PORT_MACHINES_INPUT  *  const  pNewInput )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    if (pOldInput->isRateValid !=
        pNewInput->isRateValid)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, GLOBAL: isRateValid changed from:0x%X to:0x%X",
                           pOldInput->isRateValid, pNewInput->isRateValid);
    }

    if (pOldInput->isLocalSyncOk !=
        pNewInput->isLocalSyncOk)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, GLOBAL: isLocalSyncOk changed from:0x%X to:0x%X",
                           pOldInput->isLocalSyncOk, pNewInput->isLocalSyncOk);
    }

    if (pOldInput->RtClass2_isReservedIntervalPresent !=
        pNewInput->RtClass2_isReservedIntervalPresent)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, GLOBAL: RtClass2_isReservedIntervalPresent changed from:0x%X to:0x%X",
                           pOldInput->RtClass2_isReservedIntervalPresent, pNewInput->RtClass2_isReservedIntervalPresent);
    }

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                              const  HwPortIndex       = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pOldPortInputData = &(pOldInput->PortData[HwPortIndex]);
        EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE  const  pNewPortInputData = &(pNewInput->PortData[HwPortIndex]);

        if (pOldPortInputData->doRtClass3Reset !=
            pNewPortInputData->doRtClass3Reset)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X doRtClass3Reset changed to:0x%X",
                               HwPortIndex, pNewPortInputData->doRtClass3Reset);
        }

        if (pOldPortInputData->isMauTypeOk !=
            pNewPortInputData->isMauTypeOk)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X isMauTypeOk changed to:0x%X",
                               HwPortIndex, pNewPortInputData->isMauTypeOk);
        }

        if (pOldPortInputData->isPortStateOk !=
            pNewPortInputData->isPortStateOk)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X isPortStateOk changed to:0x%X",
                               HwPortIndex, pNewPortInputData->isPortStateOk);
        }

        if (pOldPortInputData->RTSync_isLinedelayPresent !=
            pNewPortInputData->RTSync_isLinedelayPresent)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RTSync_isLinedelayPresent changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RTSync_isLinedelayPresent);
        }

        if (pOldPortInputData->RTSync_isBoundaryIngress[0]!=
            pNewPortInputData->RTSync_isBoundaryIngress[0])
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RTSync_isBoundaryIngress changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RTSync_isBoundaryIngress[0]);
        }

        if (pOldPortInputData->RTSync_isBoundaryEgress[0]!=
            pNewPortInputData->RTSync_isBoundaryEgress[0])
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RTSync_isBoundaryEgress changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RTSync_isBoundaryEgress[0]);
        }

        if (pOldPortInputData->RTSync_TopoState !=
            pNewPortInputData->RTSync_TopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RTSync_TopoState changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RTSync_TopoState);
        }

        if (pOldPortInputData->RtClass2_TopoState !=
            pNewPortInputData->RtClass2_TopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RtClass2_TopoState changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RtClass2_TopoState);
        }

        if (pOldPortInputData->RtClass3_TopoState !=
            pNewPortInputData->RtClass3_TopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RtClass3_TopoState changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RtClass3_TopoState);
        }

        if (pOldPortInputData->RtClass3_PDEVTopoState !=
            pNewPortInputData->RtClass3_PDEVTopoState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RtClass3_PDEVTopoState changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RtClass3_PDEVTopoState);
        }

        if (pOldPortInputData->RtClass3_RemoteState !=
            pNewPortInputData->RtClass3_RemoteState)
        {
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTraceInputDelta, HwPortIndex:0x%X RtClass3_RemoteState changed to:0x%X",
                               HwPortIndex, pNewPortInputData->RtClass3_RemoteState);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
* F u n c t i o n: EDDI_GetHwPortsChanged()
*
* D e s c r i p t i o n:
*  Convert from "delta_occured" to "PortChanged[EDDI_MAX_IRTE_PORT_CNT]"
*
* A r g u m e n t s:
*
*
* R e t u r n   V a l u e: LSA_VOID
*
**************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GetHwPortsChanged( LSA_UINT32     const  delta_occured,
                                                               LSA_BOOL    *  const  pPortChanged )
{
    LSA_UINT32  HwPortIndex;

    for (HwPortIndex = 0; HwPortIndex < EDDI_MAX_IRTE_PORT_CNT; HwPortIndex++)
    {
        if (   (delta_occured & ((LSA_UINT32)1<<HwPortIndex))
            || (delta_occured & EDDI_DELTA_IN_GLOBAL_INPUT_DATA))
        {
            pPortChanged[HwPortIndex] = LSA_TRUE;
        }
        else
        {
            pPortChanged[HwPortIndex] = LSA_FALSE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
 * F u n c t i o n: EDDI_SyncPortStmsTransitionBegin()
 *
 * D e s c r i p t i o n:
 *  Must be called after a transition was started i.e. an SOL was activated.
 *  Activates the EDDI_SyncPortStmsTransitionEndCbf to be called in the next
 *  NewCycle-IRQ
 *
 * A r g u m e n t s:
 *  Timeout10ns:    The maximum time needed for SOL transitions.
 *
 * R e t u r n   V a l u e: LSA_VOID
 *
 **************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTransitionBegin( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_PORT_MACHINES_TRANSITION  *  const  pSyncTransition = &pDDB->SYNC.PortMachines.Transition;

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncPortStmsTransitionBegin->Timeout10ns:0x%X", EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS);

    pSyncTransition->bTransitionRunning = LSA_TRUE;
    pSyncTransition->Timeout10ns        = EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS;
    pSyncTransition->LastTimeStamp      = IO_R32(CLK_COUNT_VALUE);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
 * F u n c t i o n: EDDI_SyncPortStmsTransitionEndCbf()
 *
 * D e s c r i p t i o n:
 *  Will be called in the NewCycleContext, if a SOL-Transition was started
 *  before.
 *  Checks if all transitions are done and calls the superordinate Cbfs.
 *
 * A r g u m e n t s:
 *
 * R e t u r n   V a l u e: LSA_VOID
 *
 **************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncPortStmsTransitionEndCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_PORT_MACHINES_TRANSITION  *  const  pSyncTransition  = &pDDB->SYNC.PortMachines.Transition;
    LSA_UINT32                             const  CurrentTimestamp = IO_R32(CLK_COUNT_VALUE);
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    LSA_UINT32                             const  DeltaTime10ns    = CurrentTimestamp - pSyncTransition->LastTimeStamp;
    #elif defined (EDDI_CFG_REV7)
    LSA_UINT32                             const  DeltaTime10ns    = (CurrentTimestamp - pSyncTransition->LastTimeStamp) / 10UL;
    #endif

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTransitionEndCbf->Timeout10ns:0x%X DeltaTime10ns:0x%X", pSyncTransition->Timeout10ns, DeltaTime10ns);

    //Count down
    pSyncTransition->Timeout10ns -= (LSA_INT32)DeltaTime10ns;

    //check state of all running SOLs
    if (!EDDI_CrtPhaseAreSolsCompleted(pDDB))
    {
        //if at least 1 PHY is in PowerDown-state, restart timeout-timer, because fcw-list is probably blocked
        if (!EDDI_SwiPhyAreAllPhysON(pDDB))
        {
            //restart timeout-timer
            pSyncTransition->Timeout10ns = EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS;
        }
        else if (pSyncTransition->Timeout10ns < 0)
        {
            EDDI_Excp("EDDI_SyncPortStmsTransitionEndCbf, Transition lasted too long. Timeout10ns: MAX_TRANSITION_TIMEOUT_10NS:",
                      EDDI_FATAL_ERR_EXCP, pSyncTransition->Timeout10ns, EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS);
            return;
        }

        //wait for next NewCycle
        pSyncTransition->LastTimeStamp = CurrentTimestamp;
    }
    else if //check state of all running Disabling FCWs
            (   (EDDI_CrtPhaseAreDisablingFCWsRunning(pDDB))
             && (pSyncTransition->Timeout10ns > (LSA_INT32)(EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS - pDDB->CycCount.CycleLength_10ns)))
    {
        //wait for next NewCycle
        pSyncTransition->LastTimeStamp = CurrentTimestamp;
    }
    else
    {
        //all SOLs completed and minimum waiting time of 1 cylce has elapsed.
        //The waiting time is needed due to a possible shutdown of IRT_CONTROL-Bits.

        EDDI_SyncPortStmsTriggerFinish(pDDB);

        //Clear TransitionFlag
        pSyncTransition->bTransitionRunning = LSA_FALSE;

        {
            //Retrigger all IrtClass3-statemachines to handle eventually pending inputs

            LSA_UINT32         UsrPortId;
            LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

            //check all connected ports
            for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
            {
                LSA_UINT32  const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

                EDDI_IrtClass3StmsTrigger(pDDB, HwPortIndex, LSA_FALSE /*currently no transition is running on this port*/);
            }
        }

        if (!EDDI_CrtPhaseAreTransitionsRunning(pDDB))
        {
            EDDI_SyncPortStmsTriggerFinish(pDDB);
        }
    }

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncPortStmsTransitionEndCbf<-bTransitionRunning:0x%X", pSyncTransition->bTransitionRunning);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_sync_port_stms.c                                        */
/*****************************************************************************/

