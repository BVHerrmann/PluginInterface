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
/*  F i l e               &F: eddi_sync_fwd_stm.c                       :F&  */
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
/*  11.10.07    JS    With Announce Forwarding no TopoOk check.              */
/*  12.10.07    JS    With Announce Forwarding no MauType and PortState check*/
/*  31.10.07    JS    Changed SW-Fwd with buffered receive to FALSE.         */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*
 *                                 includes                                  *
 *===========================================================================*/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_SYNC_FWD_STM
#define LTRC_ACT_MODUL_ID  135

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncFwdStmsSetDomainBoundaries( LSA_UINT32              const HwPortIndex,
                                                                         LSA_UINT16              const MACAddrGroup,
                                                                         LSA_UINT32              const MACAddrLow,
                                                                         LSA_BOOL                const FdbFwdActivity,
                                                                         LSA_BOOL                const BLSet,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                         LSA_UINT8               const blTableIndex );


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncFwdStmsInit()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex )
{
    EDDI_SYNC_FWD_MACHINE  *  const  pSyncFwdStm_Machine = &(pDDB->SYNC.PortMachines.SyncFwd_Machine[HwPortIndex]);
    LSA_UINT32                       i;
    #if defined (EDDI_CFG_REV7)
    LSA_UINT32                       j, k;
    #endif

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncFwdStmsInit->");

    pSyncFwdStm_Machine->FwdActivity      = LSA_FALSE;
    pSyncFwdStm_Machine->AnnounceActivity = LSA_FALSE;
    pSyncFwdStm_Machine->BLSetFwd         = LSA_FALSE;
    pSyncFwdStm_Machine->BLSetAnno        = LSA_FALSE;
    pSyncFwdStm_Machine->FirstRun         = LSA_TRUE;

    for (i = 0; i< EDD_CFG_MAX_PORT_CNT; i++)
    {
        //SyncID0: controlled by SyncFWD statemachine
        //SyncID1: Always set to TRUE (FWD by GSY) TFS1867850
        pSyncFwdStm_Machine->UsrPorts_doSwFwd[i][0] = LSA_FALSE;
        pSyncFwdStm_Machine->UsrPorts_doSwFwd[i][1] = LSA_TRUE;
    }

    #if defined (EDDI_CFG_REV7)
    //Initilize BL table
    for(i=0;i<EDDI_BL_USR_SIZE;i++)
    {
        for(j=0;j<EDDI_BL_MAC_USR_SIZE*EDDI_MAX_MACADR_BAND_SIZE;j++)
        {
            for(k=0;k<EDDI_MAX_IRTE_PORT_CNT;k++)
            {
                if(EDDI_BL_USR_PRM == i)
                {
                    //Defaultstate for forwarding control by PDPortDataAdjust record is "FORWARDING"
                    pDDB->pLocal_SWITCH->BLTable[i][j].PortState[k] = EDDI_BL_FW;
                }
                else
                {
                    //Defaultstate for forwarding control by SyncFWStm is "EGRESS" according to
                    //BLSetFwd = LSA_FALSE and FwdActivity = LSA_FALSE
                    pDDB->pLocal_SWITCH->BLTable[i][j].PortState[k] = EDDI_BL_EGRESS;
                }
            }
        }
    }
    #endif

}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncFwdStmsTrigger()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsTrigger( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_UINT8                const  blTableIndex )
{
    LSA_BOOL                                          OldFwdActivity, OldAnnounceActivity;
    LSA_BOOL                                          OldBLSetFwd, OldBLSetAnno;
    LSA_BOOL                                          SyncFwdOutputState;
    #if !defined (EDDI_CFG_REV5)
    LSA_BOOL                                          FwdActivityForcedOff = LSA_FALSE;
    #endif
    LSA_UINT32                                        DestUsrPortId;
    EDDI_SYNC_FWD_MACHINE                   *  const  pSyncFwdMachine = &(pDDB->SYNC.PortMachines.SyncFwd_Machine[HwPortIndex]);
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE     const  pPortInputData  = &(pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex]);

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncFwdStmsTrigger->HwPortIndex:0x%X", HwPortIndex);

    OldFwdActivity      = pSyncFwdMachine->FwdActivity;
    OldAnnounceActivity = pSyncFwdMachine->AnnounceActivity;
    OldBLSetFwd         = pSyncFwdMachine->BLSetFwd;
    OldBLSetAnno        = pSyncFwdMachine->BLSetAnno;
    

    EDDI_SYNC_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, HwPortIndex:0x%X isPortStateOk:0x%X isMauTypeOk:0x%X isRateValid:0x%X RTSync_isLinedelayPresent:0x%X RTSync_isBoundaryIngress:0x%X RTSync_isBoundaryEgress:0x%X RTSync_TopoState:0x%X",
                       HwPortIndex,
                       pPortInputData->isPortStateOk,
                       pPortInputData->isMauTypeOk,
                       pDDB->SYNC.PortMachines.Input.isRateValid,
                       pPortInputData->RTSync_isLinedelayPresent,
                       pPortInputData->RTSync_isBoundaryIngress[0],
                       pPortInputData->RTSync_isBoundaryEgress[0],
                       pPortInputData->RTSync_TopoState);

    /*-------------------------------------------------------------------------*/
    /* Actions for Sync/Fu Softwareforwarding                                  */
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /* We only take the input of a port into account. The output states have   */
    /* to be checked by GSY!                                                   */
    /*                                                                         */
    /* NOTE: Currently we dont support a forwarding from RED to GREEN phase    */
    /* (e.g. RED receive -> GREEN send). If we shall support this additional   */
    /* checks have to be added. In additon if we receive a buffered sync frame */
    /* the frame does not contain a SRC-MAC!and Ethertype (see buffered receive*/
    /* Both are needed with software forwarding within GSY!                    */
    /*                                                                         */
    /* So the setting of UsrPorts_doSwFwd only applies to frames received      */
    /* with NRT! see EDDI_SyncFwdStmsGetSwFwd()                                */
    /*-------------------------------------------------------------------------*/

    if (   (pPortInputData->isPortStateOk)
        && (pPortInputData->isMauTypeOk)
        && (pDDB->SYNC.PortMachines.Input.isRateValid)
        && (pPortInputData->RTSync_isLinedelayPresent)
        && (pPortInputData->RTSync_TopoState  == EDD_SET_REMOTE_PORT_STATE_TOPO_OK))
    {
        LSA_BOOL  NewdoSwFwd;

        for (DestUsrPortId = 1; DestUsrPortId <= pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
        {
            //check SyncID0
            if (!(pPortInputData->RTSync_isBoundaryIngress[0]))
            {
                #if defined (EDDI_CFG_REV5)
                {
                    /* With ERTEC400 we always have to SW-Fwd if received via NRT */
                    NewdoSwFwd = LSA_TRUE;
                }
                #else
                {
                    LSA_BOOL    bPDPortMrpDataAdjustAvailable = LSA_FALSE;
                    LSA_BOOL    bMRPActive                    = LSA_FALSE;
                    LSA_UINT32  Index;
                                       
                    /* MRP selected AND records present */
                    for (Index=0; Index <EDD_CFG_MAX_PORT_CNT; Index++ )
                    {
                        if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[Index])
                        {
                            bPDPortMrpDataAdjustAvailable = LSA_TRUE;   
                            break;
                        }
                    }
                    
                    if (    ((EDDI_PRM_WRITE_DONE == pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B) &&  bPDPortMrpDataAdjustAvailable)
                         || ((EDDI_PRM_NOT_VALID  == pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B) && !bPDPortMrpDataAdjustAvailable))
                    {
                        bMRPActive = LSA_TRUE;
                    }

                    NewdoSwFwd = LSA_FALSE;

                    if (bMRPActive && pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB)
                    {
                        #if defined (EDDI_CFG_SYNCFRAME_FWD_BASED_ON_MRP3P)
                        //if MRP is active, and we are SyncMaster or have more than 2 ports (MRP with stub), we have to deactivate HW-forwarding 
                        if /* more than 2 ports OR SyncMaster */
                           (    (pDDB->PM.PortMap.PortCnt > 2) 
                             || (SYNC_PROP_ROLE_CLOCK_MASTER == pDDB->PRM.PDSyncData.RecordSet_A.pLocalPDSyncDataRecord->SyncProperties.Value16))
                        #else
                        //if MRP is active, we have to deactivate HW-forwarding 
                        #endif
                        {
                            NewdoSwFwd            = LSA_TRUE;
                            FwdActivityForcedOff  = LSA_TRUE;
                        }
                    }
                }
                #endif

                if (NewdoSwFwd != pSyncFwdMachine->UsrPorts_doSwFwd[DestUsrPortId - 1][0])
                {
                    //temporarily_disabled_lint -save -e774 //if' always evaluates to False --> REV6
                    if (NewdoSwFwd)
                    {
                        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, doSwFwd == TRUE  for RcvHwPortIndex:0x%X DestUsrPortId:0x%X",
                                           pDDB->PM.UsrPortID_to_HWPort_0_3[DestUsrPortId], DestUsrPortId);
                    }
                    else
                    {
                        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, doSwFwd == FALSE for RcvHwPortIndex:0x%X DestUsrPortId:0x%X",
                                           pDDB->PM.UsrPortID_to_HWPort_0_3[DestUsrPortId], DestUsrPortId);
                    }
                    //temporarily_disabled_lint -restore

                    pSyncFwdMachine->UsrPorts_doSwFwd[DestUsrPortId - 1][0] = NewdoSwFwd;
                }
            }
            else
            {
                pSyncFwdMachine->UsrPorts_doSwFwd[DestUsrPortId - 1][0] = LSA_FALSE;  //SyncID0
            }

            //SYNCID1: Always set doSwFwd to TRUE (FWD by GSY)
        }
    }
    else
    {
        for (DestUsrPortId = 1; DestUsrPortId <= pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
        {
            pSyncFwdMachine->UsrPorts_doSwFwd[DestUsrPortId - 1][0] = LSA_FALSE;  //SyncID0
        }
    }

    /*-------------------------------------------------------------------------*/
    /* Actions for Announce frame HW-forwarding                                */
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /* We currently only support a egress Boundary with Announce Frames!       */
    /*                                                                         */
    /* Announce-Frames shall only have a boundary if both (ingress and egress) */
    /* boundarys are setup (this is a special handling with announce frames!)  */
    /* So we only set the egress filter if both, egress and ingress, is setup. */
    /*                                                                         */
    /* No Mautype, PortState and ToptState will be checked!                    */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    //if (pPortInputData->RTSync_isLinedelayPresent)
    {
        #if defined (EDDI_CFG_REV7)
        if (LSA_FALSE == pPortInputData->RTSync_isBoundaryEgress[0])
        {
            //enable forwarding if no egress-boundary
            pSyncFwdMachine->AnnounceActivity = LSA_TRUE;
            pSyncFwdMachine->BLSetAnno        = LSA_FALSE;
        }
        else if (   (LSA_FALSE == pPortInputData->RTSync_isBoundaryIngress[0])
                 || (pDDB->PM.DomainBoundaryBLNotAllowed))
        {
            //New special handling for announce frames:
            //frames are only discarded if both egress AND igress boundaries are set
            pSyncFwdMachine->AnnounceActivity = LSA_TRUE;
            pSyncFwdMachine->BLSetAnno        = LSA_FALSE;
        }
        else
        {
            //Egress and Igress ==> no forwarding, with boundary
            pSyncFwdMachine->AnnounceActivity = LSA_FALSE;
            pSyncFwdMachine->BLSetAnno        = LSA_TRUE;
        }
        #else
        //New special handling for announce frames:
        //frames are only discarded if both egress AND igress boundaries are set
        if (   (pPortInputData->RTSync_isBoundaryEgress[0]) 
            && (pPortInputData->RTSync_isBoundaryIngress[0]))
        {
            pSyncFwdMachine->AnnounceActivity = LSA_FALSE;
        }
        else
        {
            /* boundary only if egress and ingress is set */
            pSyncFwdMachine->AnnounceActivity = LSA_TRUE;
        }
        pSyncFwdMachine->BLSetAnno = LSA_FALSE;
        #endif
    }

    if //announce activity changed
       (   (OldAnnounceActivity != pSyncFwdMachine->AnnounceActivity)
        || (OldBLSetAnno != pSyncFwdMachine->BLSetAnno))
    {
        EDDI_SYNC_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, HwPortIndex:0x%X AnnounceActivity:%d, BL:%d",
                           HwPortIndex, pSyncFwdMachine->AnnounceActivity, pSyncFwdMachine->BLSetAnno);

        EDDI_SyncFwdStmsSetDomainBoundaries(HwPortIndex,
                                            EDDI_MAC_ADDR_GROUP_PTCP_ANNOUNCE,
                                            0, //MACAddrLow = SyncId0
                                            pSyncFwdMachine->AnnounceActivity,
                                            pSyncFwdMachine->BLSetAnno,
                                            pDDB, 
                                            blTableIndex);
    }

    /*-------------------------------------------------------------------------*/
    /* Actions for sync/fu frame HW-forwarding                                 */
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /* We currently only support a egress Boundary with Sync/fu-Frames!        */
    /* This is only for Green/Orange phase! (red-Phase has planed frames)      */
    /*                                                                         */
    /* With SOC we can use the Boundary flag for ingress filtering             */
    /*-------------------------------------------------------------------------*/

    EDDI_SYNC_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, HwPortIndex:0x%X BoundAll:0x%X egress:0x%X igress:0x%X",
                       HwPortIndex, pDDB->PM.DomainBoundaryBLNotAllowed,
                       pPortInputData->RTSync_isBoundaryEgress[0], pPortInputData->RTSync_isBoundaryIngress[0]);

    if (   (pPortInputData->isPortStateOk)
        && (pPortInputData->isMauTypeOk)
        && (pDDB->SYNC.PortMachines.Input.isRateValid)
        && (pPortInputData->RTSync_isLinedelayPresent)
        && (pPortInputData->RTSync_isBoundaryEgress[0] == LSA_FALSE)
        && (pPortInputData->RTSync_TopoState  == EDD_SET_REMOTE_PORT_STATE_TOPO_OK))
    {
        //only activate HW-forwarding if not inhibited by MRP-conditions (see above)
        #if !defined (EDDI_CFG_REV5)
        pSyncFwdMachine->FwdActivity = (LSA_FALSE == FwdActivityForcedOff)?LSA_TRUE:LSA_FALSE; 
        #else
        pSyncFwdMachine->FwdActivity = LSA_TRUE; 
        #endif
        pSyncFwdMachine->BLSetFwd    = LSA_FALSE;
    }
    else
    {
        pSyncFwdMachine->FwdActivity = LSA_FALSE;

        /* check if igress-Filtering selected (only possible in conjunction with egress-filtering) */
        #if defined (EDDI_CFG_REV7)
        if (   (pPortInputData->RTSync_isBoundaryIngress[0] == LSA_FALSE)
            || (pDDB->PM.DomainBoundaryBLNotAllowed))
        {
            //Egress but no Igress ==> no forwarding, no boundary
            //Egress + Igress, but some port before had Egress without Igress => no boundary anymore!
            pSyncFwdMachine->BLSetFwd = LSA_FALSE;
        }
        else
        {
            /* Borderlines only if egress and ingress is set */
            pSyncFwdMachine->BLSetFwd = LSA_TRUE;
        }
        #else
        pSyncFwdMachine->BLSetFwd = LSA_FALSE;
        #endif
    }

  //SyncFwd-output-state changed

    SyncFwdOutputState=    pSyncFwdMachine->FirstRun
                       || (OldFwdActivity != pSyncFwdMachine->FwdActivity)
                       || (OldBLSetFwd != pSyncFwdMachine->BLSetFwd);

    #if !defined (EDDI_CFG_REV5)
    SyncFwdOutputState=SyncFwdOutputState|| (FwdActivityForcedOff) ;
    #endif

    if (SyncFwdOutputState)
    {
        EDDI_SYNC_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncFwdStmsTrigger, HwPortIndex:0x%X FwdActivity:%d BL:%d",
                           HwPortIndex, pSyncFwdMachine->FwdActivity, pSyncFwdMachine->BLSetFwd);

        pSyncFwdMachine->FirstRun=LSA_FALSE;

        // Switch HW-forwarding on or off

        #if defined (EDDI_CFG_REV5)
        /* ERTEC400 cannot support HW-Forwarding in Green/Orange phase */
        #else
        /* Currently only the destination port is taken into account!  */

        EDDI_SyncFwdStmsSetDomainBoundaries(HwPortIndex,
                                            EDDI_MAC_ADDR_GROUP_PTCP_SYNC,
                                            0, //MACAddrLow = SyncId0
                                            pSyncFwdMachine->FwdActivity,
                                            pSyncFwdMachine->BLSetFwd,
                                            pDDB,
                                            blTableIndex);

        EDDI_SyncFwdStmsSetDomainBoundaries(HwPortIndex,
                                            EDDI_MAC_ADDR_GROUP_PTCP_FOLLOW_UP,
                                            0, //MACAddrLow = SyncId0
                                            pSyncFwdMachine->FwdActivity,
                                            pSyncFwdMachine->BLSetFwd,
                                            pDDB,
                                            blTableIndex);

        EDDI_SyncFwdStmsSetDomainBoundaries(HwPortIndex,
                                            EDDI_MAC_ADDR_GROUP_PTCP_SYNC_WITH_FU,
                                            0, //MACAddrLow = SyncId0
                                            pSyncFwdMachine->FwdActivity,
                                            pSyncFwdMachine->BLSetFwd,
                                            pDDB,
                                            blTableIndex);
        #endif
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncFwdStmsTrigger<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_SyncFwdStmsGetSwFwd()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                          (OUT = doSwFwd-Array)                          */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsGetSwFwd( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                         LSA_BOOL                    const  bIsRcvBuffered,
                                                         LSA_BOOL                    const  bSyncID1,
                                                         LSA_UINT16                  const  RcvUsrPortID,
                                                         LSA_UINT8                *  const  pDoSwFwd )
{
    LSA_UINT32                       DestUsrPortId;
    LSA_UINT32                const  RcvHwPortIndex  = pDDB->PM.UsrPortID_to_HWPort_0_3[RcvUsrPortID];
    EDDI_SYNC_FWD_MACHINE  *  const  pSyncFwdMachine = &(pDDB->SYNC.PortMachines.SyncFwd_Machine[RcvHwPortIndex]);
    LSA_UINT8                 const  SyncID          = (LSA_FALSE == bSyncID1)?0:1;

    for (DestUsrPortId = 1; DestUsrPortId <= pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
    {
        if (bIsRcvBuffered)
        {
            // Call from buffered Sync-Rcv
            // If buffered receive (red) we currently only support HW-forwarding
            // within RED phase (e.g. destination ports must have a planed sync forwarder
            // and a RED phase established.
            // In addition the receive frame currently does NOT contain a SRC-MAC and Ethertype!!
            // (which is needed for correct SW-forwarding!)
            pDoSwFwd[DestUsrPortId - 1] = EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER;
        }
        else
        {
            // Call from queued NRT-Rcv
            pDoSwFwd[DestUsrPortId - 1] = (pSyncFwdMachine->UsrPorts_doSwFwd[DestUsrPortId - 1][SyncID]) ? EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER : EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER;
;

            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncFwdStmsGetSwFwd <- doSwFwd[0x%X] = 0x%X", DestUsrPortId-1, pDoSwFwd[DestUsrPortId-1]);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncFwdStmsSetDomainBoundaries()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncFwdStmsSetDomainBoundaries( LSA_UINT32               const  HwPortIndex,
                                                                            LSA_UINT16               const  MACAddrGroup,
                                                                            LSA_UINT32               const  MACAddrLow,
                                                                            LSA_BOOL                 const  FdbFwdActivity,
                                                                            LSA_BOOL                 const  BLSet,
                                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                            LSA_UINT8                const  blTableIndex )
{
    LSA_RESULT                       Result;
    EDD_RQB_PORTID_MODE_TYPE         PortIDMode;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE  MCFWDCtrlParam;

    LSA_UNUSED_ARG(BLSet); //satisfy lint!

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncFwdStmsSetDomainBoundaries->");

    MCFWDCtrlParam.pPortIDModeArray = &PortIDMode;

    #if defined (EDDI_CFG_REV7)
    if (FdbFwdActivity)
    {
        PortIDMode.Mode = EDD_MULTICAST_FWD_ENABLE;           /* forwarding enabled, no borderline */
    }
    else if (BLSet)
    {
        PortIDMode.Mode = EDD_MULTICAST_FWD_DISABLE_PLUS_BL;  /* forwarding disabled, with borderline (only selectable in conjunction) */
    }
    else
    {
        PortIDMode.Mode = EDD_MULTICAST_FWD_DISABLE;          /* forwarding disabled, no borderline */
    }
    #else
    PortIDMode.Mode                 = (LSA_UINT16) ((FdbFwdActivity != LSA_FALSE) ? EDD_MULTICAST_FWD_ENABLE : EDD_MULTICAST_FWD_DISABLE);
    #endif

    PortIDMode.DstPortID            = (LSA_UINT16) pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex];
    PortIDMode.SrcPortID            = (LSA_UINT16) EDD_PORT_ID_AUTO;
    MCFWDCtrlParam.PortIDModeCnt    = 1;
    MCFWDCtrlParam.MACAddrGroup     = MACAddrGroup;
    MCFWDCtrlParam.MACAddrLow       = MACAddrLow;
    MCFWDCtrlParam.MACAddrPrio      = EDD_MULTICAST_FWD_PRIO_UNCHANGED;

    //No EDDI_ENTER_SYNC_S() here, already applied by all functions calling EDDI_SyncPortStmsTrigger
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,                                
                                 pDDB,
                                 blTableIndex);

    if (EDD_STS_OK != Result)
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncFwdStmsSetDomainBoundaries, EDDI_SwiPNMCFwdCtrl, Result:0x%X", Result);
        EDDI_Excp("EDDI_SyncFwdStmsSetDomainBoundaries, EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncFwdStmsSetDomainBoundaries<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_fwd_stm.c                                          */
/*****************************************************************************/
