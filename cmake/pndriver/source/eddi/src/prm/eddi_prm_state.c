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
/*  F i l e               &F: eddi_prm_state.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-global functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  21.09.07    JS    added DCPBoundary handling                             */
/*  25.10.07    JS    consistency check with RTSync adapted to actual record */
/*                    format.                                                */
/*  12.11.07    JS    Bugfixes.*Domainboundary change possible without       */
/*                              PDIRData/PDSync change. -> always evaluate!  */
/*                             *DomainBoundary maybe NOT VALID after a       */
/*                              reprogramming (dont reuse old setting)       */
/*                             *PDSync-Data maybe NOT VALID                  */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_time.h"
#include "eddi_ext.h"

#if defined (EDDI_CFG_FRAG_ON)
#include "eddi_nrt_frag_tx.h"
#endif

#include "eddi_prm_record_pdtraceunit_control.h"
#include "eddi_prm_state.h"
#include "eddi_prm_state_diag_pdnc.h"
#include "eddi_prm_record_common.h"
#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_pdport_data_adjust.h"

#include "eddi_prm_req.h"
#include "eddi_swi_ext.h"
#include "eddi_sync_ir.h"
#include "eddi_sync_usr.h"
#include "eddi_sync_check.h"
#include "eddi_Tra.h"
//#include "eddi_irt_ext.h"
#include "eddi_ser_ext.h"
#include "eddi_ser.h"
#include "eddi_crt_dfp.h"
#include "eddi_nrt_ini.h"
#include "eddi_lock.h"


#define EDDI_MODULE_ID     M_ID_EDDI_PRM_STATE
#define LTRC_ACT_MODUL_ID  401

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmStateStart( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                          EDD_SERVICE              const  Service );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmStatePrepare( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                            EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                            EDD_SERVICE              const  Service );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmStateWrite( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                          EDD_SERVICE              const  Service );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmStateEnd( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                        EDD_SERVICE              const  Service );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_PrmGetSyncIrDataChange( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                                   EDDI_PRM_PDSYNC_DATA_PTR_TYPE  const  pPDSyncData,
                                                                   EDDI_PRM_PDIR_DATA_PTR_TYPE    const  pPDIRData );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_PrmMoveBToAPart1( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_PrmDeleteAllB( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmCheckConsistency( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT8                const  Local,
                                                                LSA_UINT8 const       *  const  IsModularPort,
                                                                LSA_UINT8                       PortparamsNotApplicable [] );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SetMauType( LSA_UINT32               const  UsrPortIndexMauType,
                                                       LSA_UINT32               const  HwPortIndexMauType,
                                                       LSA_UINT8                const  PHYPower,
                                                       LSA_UINT8                const  LinkState,
                                                       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SetMulticast( LSA_UINT32              const  HwPortIndex,
                                                         LSA_UINT32              const  MACAddrLow,
                                                         EDDI_LOCAL_DDB_PTR_TYPE const  pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SetDCPMulticast( LSA_UINT32              const  HwPortIndex,
                                                            LSA_UINT32              const  MACAddrLow,
                                                            EDDI_LOCAL_DDB_PTR_TYPE const  pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_SetMRPMulticast( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                            LSA_UINT16              const  Mode );

static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCompare( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                        EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pRecordA,
                                                        EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pRecordB,
                                                        LSA_UINT32                  const  RecordLen );

static  LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PrmCheckPDIRPDSyncConsistency( EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE const  pLocalPDSyncDataRecord,
                                                                          EDDI_PRM_RECORD_IRT_PTR_TYPE         const  pRecordSet,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB );

static  LSA_BOOL   EDDI_LOCAL_FCT_ATTR  EDDI_CheckPrmRecordSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_VOID   EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetDomainBoundary( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB,
                                                                   LSA_UINT32              const  HwPortIndex,
                                                                   LSA_BOOL                const  bSyncID0, 
                                                                   LSA_BOOL                    *  pBoundaryIngress,
                                                                   LSA_BOOL                    *  pBoundaryEgress );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_AssociateRedundantFrames        ( EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                                                EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB );
        
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPrmEndState                ( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB );


static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PRMChangePortStateMachine_DoPull( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                                LSA_UINT32                    const  UsrPortIndex,
                                                                                LSA_UINT32                    const  HWPortIndex);

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PRMChangePortStateMachine_DoPlugPrepare
                                                                              ( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                                LSA_UINT32                    const  UsrPortIndex,
                                                                                LSA_UINT32                    const  HWPortIndex);

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PRMChangePortStateMachine_DoPlugCommit
                                                                              ( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                                LSA_UINT32                    const  UsrPortIndex,
                                                                                LSA_UINT32                    const  HWPortIndex);

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PRMRequestPrmChangePort         ( EDD_UPPER_RQB_PTR_TYPE        const  pRQB,
                                                                                EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB);

#if defined (EDDI_CFG_REV7)
static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_PrmSetBLTableforPrmUsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                                  LSA_UINT32               const  UsrPortIndex,
                                                                  LSA_UINT32               const MACBandBeginnIndex, 
                                                                  LSA_UINT32               const MACBandLength, 
                                                                  LSA_BOOL                 const isBoundaryIngress,
                                                                  LSA_BOOL                 const isBoundaryEgress );
#endif


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmIni( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16                                              Uint16Temp;
    LSA_UINT32                                              UsrPortIndex;
    EDDI_PRM_PDNC_DATA_CHECK                             *  pPDNC;
    EDDI_PRM_PDSCF_DATA_CHECK                            *  pPDSCF;
    EDDI_PRM_PDIR_DATA                                   *  pPDIR;
    EDDI_PRM_PDSYNC_DATA                                 *  pPDSync;
    EDDI_PRM_PDTRACE_UNIT_CONTROL                        *  pPDTraceUnit;
    EDDI_PRM_PDCONTROL_PLL                               *  pPDPLL;
    EDDI_PRM_PDIR_APPLICATION_DATA                       *  pPDirApplicationData;
    EDDI_PRM_PDPORT_MRP_DATA_ADJUST_RECORD_PTR_TYPE  const  pPDPortDataAdjust = &pDDB->PRM.PDPortDataAdjust;
    EDDI_PRM_PDNRT_LOAD_LIMITATION                       *  pPDNRTFill;
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_PRM_PDIR_SUBFRAME_DATA                          *  pPDIRSubFrameData;
    #endif

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIni->");

    pDDB->PRM.bPendingCommit       = LSA_FALSE;
    pDDB->PRM.StateFct             = (PRM_STATE_FCT)EDDI_PrmStatePrepare;
    pDDB->PRM.bFirstCommitExecuted = LSA_FALSE;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_InitQueue(pDDB, &pDDB->PRM.Transition.RqbQueue);
    pDDB->PRM.Transition.bTransitionRunning = LSA_FALSE;
    pDDB->PRM.Transition.pRunningRQB        = EDDI_NULL_PTR;
    #endif

    pPDNC                           = &pDDB->PRM.PDNCDataCheck;
    pPDSCF                          = &pDDB->PRM.PDSCFDataCheck;
    pPDIR                           = &pDDB->PRM.PDIRData;
    pPDSync                         = &pDDB->PRM.PDSyncData;
    pPDPLL                          = &pDDB->PRM.PDControlPLL;
    pPDTraceUnit                    = &pDDB->PRM.PDTraceUnitControl;
    pPDirApplicationData            = &pDDB->PRM.PDirApplicationData;
    pPDNRTFill                      = &pDDB->PRM.PDNrtLoadLimitation;
    #if defined (EDDI_CFG_DFP_ON)
    pPDIRSubFrameData               = &pDDB->PRM.PDIRSubFrameData;

    pPDIRSubFrameData->State_A      = EDDI_PRM_NOT_VALID;
    pPDIRSubFrameData->State_B      = EDDI_PRM_NOT_VALID;  
    pPDIRSubFrameData->pRecordSet_A = &pPDIRSubFrameData->RecordSet_A;
    pPDIRSubFrameData->pRecordSet_B = &pPDIRSubFrameData->RecordSet_B;  
    #endif

    pPDNC->State_A                  = EDDI_PRM_NOT_VALID;
    pPDNC->State_B                  = EDDI_PRM_NOT_VALID;
    pPDNC->pA                       = &pPDNC->RecordSet_A;
    pPDNC->pB                       = &pPDNC->RecordSet_B;
    pPDNC->DiagStateFct             = EDDI_PrmStateDiagPDNCStart;
    pPDNC->Last_Event_Appears       = EDDI_PRM_EV_START;
    pPDNC->Last_Event_Disappears    = EDDI_PRM_EV_START;
    pPDNC->bPDNCChanged             = LSA_FALSE;

    pPDSync->State_A                = EDDI_PRM_NOT_VALID;
    pPDSync->State_B                = EDDI_PRM_NOT_VALID;
    pPDSync->pRecordSet_A           = &pPDSync->RecordSet_A;
    pPDSync->pRecordSet_B           = &pPDSync->RecordSet_B;
    //pPDSync->DiagStateFct          = eddi_PrmStateDiag_PDSync_Start;   PK: RTSync
    //pPDSync->Last_Event_Appears    = EDDI_PRM_EV_START_PDSYNC;         PK: RTSync
    //pPDSync->Appear_Event_Execute  = EDDI_PRM_EV_START_PDSYNC;         PK: RTSync
    //pPDSync->Last_Event_Disappears = EDDI_PRM_EV_START_PDSYNC;         PK: RTSync
    //pPDSync->bPDSyncChanged        = LSA_FALSE;                        PK: RTSync

    pPDIR->State_A                  = EDDI_PRM_NOT_VALID;
    pPDIR->State_B                  = EDDI_PRM_NOT_VALID;
    pPDIR->pRecordSet_A             = &pPDIR->RecordSet_A;
    pPDIR->pRecordSet_B             = &pPDIR->RecordSet_B;
    pPDIR->pRecordSet_A->NumberOfFrameDataElements = 0;
    pPDIR->pRecordSet_B->NumberOfFrameDataElements = 0;

    pPDSCF->State_A                 = EDDI_PRM_NOT_VALID;
    pPDSCF->State_B                 = EDDI_PRM_NOT_VALID;
    pPDSCF->pA                      = &pPDSCF->RecordSet_A;
    pPDSCF->pB                      = &pPDSCF->RecordSet_B;

    /* --------------------------------------------------------------------------*/
    /* PDControlPLL Record                                                       */
    /* --------------------------------------------------------------------------*/
    pPDPLL->State_A                                  = EDDI_PRM_NOT_VALID;
    pPDPLL->RecordSet_A.BlockHeader.BlockType        = EDDI_PRM_BLOCKTYPE_PD_CONTROL_PLL;
    pPDPLL->RecordSet_A.BlockHeader.BlockLength      = EDDI_PRM_H_BL_PD_CONTROL_PLL;
    pPDPLL->RecordSet_A.BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
    pPDPLL->RecordSet_A.BlockHeader.BlockVersionLow  = EDDI_PRM_BLOCK_VERSION_LOW;
    pPDPLL->RecordSet_A.BlockHeader.Padding1_1       = 0;
    pPDPLL->RecordSet_A.BlockHeader.Padding1_2       = 0;
    Uint16Temp                                       = EDDI_PRM_DEF_PLL_MODE_OFF;
    pPDPLL->RecordSet_A.PLL_Mode                     = EDDI_HTONS(Uint16Temp);
    pPDPLL->RecordSet_A.Reserved                     = 0;

    pPDPLL->PDControlPllActive                       = LSA_FALSE;

    /* --------------------------------------------------------------------------*/
    /* PDTraceUnit Record                                                        */
    /* --------------------------------------------------------------------------*/
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE  *  const  pRecordSet_A = &pPDTraceUnit->RecordSet_A[UsrPortIndex];

        pPDTraceUnit->State_A[UsrPortIndex]        = EDDI_PRM_NOT_VALID;
        pRecordSet_A->BlockHeader.BlockType        = EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_CONTROL;
        pRecordSet_A->BlockHeader.BlockLength      = EDDI_PRM_H_BL_PD_TRACE_UNIT_CONTROL;
        pRecordSet_A->BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
        pRecordSet_A->BlockHeader.BlockVersionLow  = EDDI_PRM_BLOCK_VERSION_LOW;
        pRecordSet_A->BlockHeader.Padding1_1       = 0;
        pRecordSet_A->BlockHeader.Padding1_2       = 0;
        pRecordSet_A->DiagnosticEnable             = 0UL;
        pRecordSet_A->Reserved                     = 0UL;
    }

    /* --------------------------------------------------------------------------*/
    /* PDPort Record                                                             */
    /* --------------------------------------------------------------------------*/
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MAUTypePresent           = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex].PortStatePresent         = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MulticastBoundaryPresent = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex].DomainBoundaryPresent    = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex].DCPBoundaryPresent       = LSA_FALSE;
        pDDB->PRM.PDPortDataAdjust.RecordState_A[UsrPortIndex]                = EDDI_PRM_NOT_VALID;

        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].MAUTypePresent           = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].PortStatePresent         = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].MulticastBoundaryPresent = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].DomainBoundaryPresent    = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].DCPBoundaryPresent       = LSA_FALSE;
        pDDB->PRM.PDPortDataAdjust.RecordState_B[UsrPortIndex]                = EDDI_PRM_NOT_VALID;

        pDDB->PRM.PDPortMrpDataAdjust.RecordState_A[UsrPortIndex]             = EDDI_PRM_NOT_VALID;
        pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex]             = EDDI_PRM_NOT_VALID;
        pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB               = LSA_FALSE;
    }

    pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_A = EDDI_PRM_NOT_VALID;
    pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B = EDDI_PRM_NOT_VALID;
    
    pDDB->PRM.PDSetDefaultPortStates.RecordState_A = EDDI_PRM_NOT_VALID;
    pDDB->PRM.PDSetDefaultPortStates.RecordState_B = EDDI_PRM_NOT_VALID;

    pPDirApplicationData->State_A                                    = EDDI_PRM_NOT_VALID;
    pPDirApplicationData->State_B                                    = EDDI_PRM_NOT_VALID;
    pPDirApplicationData->pRecordSet_A                               = &pPDirApplicationData->RecordSet_A;
    pPDirApplicationData->pRecordSet_B                               = &pPDirApplicationData->RecordSet_B;
    pPDirApplicationData->pRecordSet_A->bCalculatedTransferEndValues = LSA_FALSE;
    pPDirApplicationData->pRecordSet_B->bCalculatedTransferEndValues = LSA_FALSE;

    pPDNRTFill->State_A                            =  EDDI_PRM_NOT_VALID;
    pPDNRTFill->State_B                            =  EDDI_PRM_NOT_VALID;
    pPDNRTFill->pRecordSet_A                       = &pPDNRTFill->RecordSet_A;
    pPDNRTFill->pRecordSet_B                       = &pPDNRTFill->RecordSet_B;
    pPDNRTFill->pRecordSet_A->LoadLimitationActive =  LSA_TRUE; //pDDB->NRT.bFeedInLoadLimitationActiveDefault; // noch nicht gesetzt
    pPDNRTFill->pRecordSet_A->IO_Configuration     =  LSA_TRUE; // EDDI_PRM_FILL_IO_IS_NOT_DEFINED; // noch nicht gesetzt
    pPDNRTFill->pRecordSet_B->LoadLimitationActive =  LSA_TRUE; //pDDB->NRT.bFeedInLoadLimitationActiveDefault; // noch nicht gesetzt
    pPDNRTFill->pRecordSet_B->IO_Configuration     =  LSA_TRUE; // EDDI_PRM_FILL_IO_IS_NOT_DEFINED; // noch nicht gesetzt

    pDDB->PRM.bCommitExecuted    = LSA_FALSE;
    pDDB->PRM.LesserCheckQuality = LSA_FALSE;
    pDDB->PRM.bSyncIrDataChanged = LSA_FALSE;

    EDDI_PrmIniPDNCFctTable(pDDB);
    //eddi_prm_ini_PDSYNC_FctTable(pDDB);             PK: RTSync

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIni<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmIniTimer()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmIniTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  Status;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIniTimer->");

    Status = EDDI_AllocTimer(pDDB, &pDDB->PRM.TimerID, (void *)pDDB, (EDDI_TIMEOUT_CBF)EDDI_PRMCheckTimeOut,
                             EDDI_TIMER_TYPE_CYCLIC, EDDI_TIME_BASE_100MS, EDDI_TIMER_REST);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_PrmIniTimer, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    Status = EDDI_StartTimer(pDDB, pDDB->PRM.TimerID, (LSA_UINT16)10);
    if (EDD_STS_OK != Status)
    {
        EDDI_Excp("EDDI_PrmIniTimer, EDDI_StartTimer", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    if (!EDDI_CheckPrmRecordSize(pDDB))
    {
        EDDI_Excp("EDDI_PrmIniTimer, EDDI_CheckPrmRecordSize", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIniTimer<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmGetDomainBoundary()                      */
/*                                                                         */
/* D e s c r i p t i o n: Get the actual setting for DomainBoundary for    */
/*                        HwPortIndex set within record set A              */
/*                        Default if no record set A present: No Boundary  */
/*                                                                         */
/* A r g u m e n t s:     UsrPortIndex:      User Port index               */
/*                        *pBoundaryIngress: Has port a ingress boundary?  */
/*                        *pBoundaryIngress: Has port a egress boundary?   */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetDomainBoundary( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  UsrPortIndex,
                                                                  LSA_BOOL                 const  bSyncID0, 
                                                                  LSA_BOOL                     *  pBoundaryIngress,
                                                                  LSA_BOOL                     *  pBoundaryEgress )
{
    LSA_UINT32  const  SyncIDMask = (bSyncID0 != LSA_FALSE)?0x1UL:0x2UL;
    
    /* Record set present, and DomainBoundary Present ? */
    if ((pDDB->PRM.PDPortDataAdjust.RecordState_A[UsrPortIndex] == EDDI_PRM_VALID) &&
        (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryPresent))
    {
        //SyncID0
        if (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryEgress  & SyncIDMask)
        {
            *pBoundaryEgress  = LSA_TRUE;
        }
        else
        {
            *pBoundaryEgress = LSA_FALSE;
        }

        if (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryIngress & SyncIDMask)
        {
            *pBoundaryIngress = LSA_TRUE;
        }
        else
        {
            *pBoundaryIngress = LSA_FALSE;
        }
    }
    else
    {
        //Default of DomainBoundary is FALSE !
        *pBoundaryIngress     = LSA_FALSE;
        *pBoundaryEgress      = LSA_FALSE;
    }

    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmGetDomainBoundary, UsrPortIndex:%d IngressBoundary:%d EgressBoundary:%d", 
                      UsrPortIndex, *pBoundaryIngress, *pBoundaryEgress);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCalculateTriggerDeadLine()               */
/*                                                                         */
/* D e s c r i p t i o n: This function calculates the Value of            */
/*                        TriggerDeadLine. TriggerDeadLine is the time     */
/*                        which will be set into the IRT_TimeCompare       */
/*                        register of ERTEC. (IRT_COMP_TIME)               */
/*                        on PRM-COMMIT or Sendclockchange.                */
/*                                                                         */
/*                        The calculation is done with record set A!       */
/*                                                                         */
/* A r g u m e n t s:     pDDB                                             */
/*                        CycleBaseFactor       : CycleBaseFactor          */
/*                        *pTriggerDeadLine10ns : Calculated Time   (out)  */
/*                        *pTriggerMode         : IMAGE_MODE.SyncAPI(out)  */
/*                              EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC          */
/*                              EDDI_SYNC_TRIGMODE_IRT_END                 */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCalculateTriggerDeadLine( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  CycleBaseFactor,
                                                                 LSA_UINT32                   *  pTriggerDeadLine10ns,
                                                                 LSA_UINT16                   *  pTriggerMode )
{
    EDDI_PRM_RECORD_IRT_PTR_TYPE  pRecordSet_A;
    LSA_UINT32                    CompareTimeIn10ns;
    LSA_UINT32                    Case = 0;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCalculateTriggerDeadLine->");

    pRecordSet_A = pDDB->PRM.PDIRData.pRecordSet_A;

    /* ------------------------------------------------------------------------*/
    /* Cases:                                                                  */
    /*                                                                         */
    /* 1) Red and orange present                                               */
    /*    PDIRDataRecord present with red and orange periods                   */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to CycleTimeInTicks - 10                                    */
    /*                                                                         */
    /* 2) orange only present and sync                                         */
    /*    No PDIRDataRecord and PDsync with reserved Interval > 0              */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to end of reserved interval from pdsync record              */
    /*    (= end of orange phase)                                              */
    /*                                                                         */
    /* 3) red only present                                                     */
    /*    PDIRDataRecord present but only red periods                          */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_IRT_END                              */
    /*    Set time to:  CycleTimeInTicks - 10                                  */
    /*                                                                         */
    /*                                                                         */
    /* 4) No red and orange present but Sync                                   */
    /*    No PDIRDataRecord. PDSync with Reserved IntervalEnd = 0              */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to CycleTimeInTicks - 10                                    */
    /*                                                                         */
    /*                                                                         */
    /* 5) No red and orange present and no Sync                                */
    /*    No PDIRDataRecord. No PDSync                                         */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to CycleTimeInTicks - 10                                    */
    /*                                                                         */
    /* cases 6 and 7 not very likely to occur but we handle it:                */
    /*                                                                         */
    /* 6) PDIRDataRecord present but only orange periods and                   */
    /*    PDsync with reserved Interval > 0 (may not occur)                    */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to end of reserved interval from pdsync record              */
    /*    (= end of orange phase)                                              */
    /*                                                                         */
    /* 7) PDIRDataRecord present but only orange periods and                   */
    /*    PDsync with reserved Interval = 0 (may not occur)                    */
    /*                                                                         */
    /*    TriggerMode: EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC                       */
    /*    Set time to CycleTimeInTicks - 10                                    */
    /*                                                                         */
    /* ------------------------------------------------------------------------*/

    /* Default TriggerMode (SyncAPI): Use TimeCompare without TransfereEndCeck */
    *pTriggerMode = EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC;

    /* Setup default Value: CycleTimeInTicks - 10  */
    CompareTimeIn10ns = (CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY) - 10;

    /* determine case and set time-value */
    if ( pDDB->PRM.PDIRData.State_A == EDDI_PRM_VALID ) /* PDIRData present*/
    {
        if (pRecordSet_A->bBeginEndDataPresent)
        {
            switch (pRecordSet_A->BeginEndIRTType)
            {
                case EDDI_IRT_PHASE_TYPE_RED:
                    /* case 3 */
                    Case = 3;
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 3");

                    /* Transfer End on end of IRT-Phase. The Timecompare will be set at end of cycle */
                    /* (the -10 was the setting already used be EDDI. reason unknown)                */
                    *pTriggerMode     = EDDI_SYNC_TRIGMODE_IRT_END;
                    break;
                case EDDI_IRT_PHASE_TYPE_ORANGE:
                    /* case 6 or 7 Note: this case is not expected because senseless. but we handle it. */
                    if (( pDDB->PRM.PDSyncData.State_A == EDDI_PRM_VALID ) &&
                        ( pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd ) )
                    {
                        /* case 6  */
                        Case = 6;
                        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 6");
                        CompareTimeIn10ns = pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd / 10;
                    }
                    else
                    {
                        /* case 7  */
                        Case = 7;
                        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 7");
                    }
                    break;
                case EDDI_IRT_PHASE_TYPE_NONE:
                default:
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCalculateTriggerDeadLine, Unexpected Case");
                    /* should not occur. IRT without reserved Phases.. handle like case 4 */
                    /* use default-value */
                    break;
            }
        }
        else
        {
            /* no BeginEndDataPresent -> error. We need a beginEnd Block ! */
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCalculateTriggerDeadLine, EDDI_USE_PDIR_BEGINEND_DATA_BLOCK not set.");
            EDDI_Excp("EDDI_PrmCalculateTriggerDeadLine, EDDI_USE_PDIR_BEGINEND_DATA_BLOCK not set!", EDDI_FATAL_ERR_EXCP, 0, 0);
        }
    }
    else /* no PDIRdata record */
    {
        if (pDDB->PRM.PDSyncData.State_A == EDDI_PRM_VALID)
        {
            if (pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd)
            {
                /* case 2 */
                Case = 2;
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 2 (no PDIRData)");
                CompareTimeIn10ns = pDDB->PRM.PDSyncData.pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd / 10;
            }
            else
            {
                /* case 4 */
                /* use default-value */
                Case = 4;
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 4");
            }
        }
        else
        {
            /* case 5 */
            /* use default-value */
            Case = 5;
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCalculateTriggerDeadLine, Case 5");
        }
    }

    *pTriggerDeadLine10ns = CompareTimeIn10ns;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCalculateTriggerDeadLine, CompareTimeIn10ns:%d Case:%d", 
                      CompareTimeIn10ns, Case);
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCalculateTriggerDeadLine<-");
    LSA_UNUSED_ARG(Case);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateStart( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                             EDD_SERVICE              const  Service )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateStart->");

    switch (Service)
    {
        case EDD_SRV_PRM_WRITE:
        case EDD_SRV_PRM_END:
        case EDD_SRV_PRM_COMMIT:
        case EDD_SRV_PRM_CHANGE_PORT:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Commit_Write_End_eddiPrmStateStart);
            return EDD_STS_ERR_SEQUENCE;
        }

        case EDD_SRV_PRM_PREPARE:
        {
            pDDB->PRM.StateFct = EDDI_PrmStatePrepare;
            return EDD_STS_OK;
        }

        default:
        {
            LSA_UNUSED_ARG(pRQB); //satisfy lint!
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Default_eddiPrmStateStart);
            return EDD_STS_ERR_SERVICE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStatePrepare( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                               EDD_SERVICE              const  Service )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;
    LSA_RESULT                      Status;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStatePrepare->");

    EDDI_PrmDeleteAllB(pDDB);

    switch (Service)
    {
        case EDD_SRV_PRM_WRITE:
        {
            pDDB->PRM.StateFct = EDDI_PrmStateWrite;
            Status             = EDDI_PrmCheckAndCopyRecord(pRQB, pDDB);
            return Status;
        }

        case EDD_SRV_PRM_END:
        {
            EDD_UPPER_PRM_END_PTR_TYPE  const  pPrmEnd = (EDD_UPPER_PRM_END_PTR_TYPE)pRQB->pParam;

            Status = EDDI_PrmCheckConsistency(pDDB, pPrmEnd->Local, pPrmEnd->IsModularPort, pDDB->PRM.PortparamsNotApplicable); // pPrmEnd->PortparamsNotApplicable);

            if (EDD_STS_OK != Status)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmStatePrepare, ConsistencyCheck FAILED. PRM aborted. Status:0x%X", Status);
                pDDB->PRM.StateFct = EDDI_PrmStateStart;
                return Status;
            }

            Status = EDDI_CheckPrmEndState(pDDB);
            if (EDD_STS_OK != Status)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmStatePrepare, EDDI_CheckPrmEndState FAILED. PRM aborted. Status:0x%X", Status);
                pDDB->PRM.StateFct = EDDI_PrmStateStart;
                return Status;
            }

            pDDB->PRM.StateFct = EDDI_PrmStateEnd;

            return Status;
        }

        case EDD_SRV_PRM_COMMIT:
        {
            pDDB->PRM.StateFct = EDDI_PrmStateStart;
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Commit_eddiPrmStatePrepare);
            return EDD_STS_ERR_SEQUENCE;
        }

        case EDD_SRV_PRM_PREPARE:
            return EDD_STS_OK;

        case EDD_SRV_PRM_CHANGE_PORT:
            Status = EDDI_PRMRequestPrmChangePort(pRQB, pDDB);
            return Status;

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Default_eddiPrmStatePrepare);
            return EDD_STS_ERR_SERVICE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateWrite( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                             EDD_SERVICE              const  Service )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;
    LSA_RESULT                      Status;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateWrite->");

    Status = EDD_STS_OK;

    switch (Service)
    {
        case EDD_SRV_PRM_WRITE:
        {
            Status = EDDI_PrmCheckAndCopyRecord(pRQB, pDDB);
            return Status;
        }

        case EDD_SRV_PRM_END:
        {
            EDD_UPPER_PRM_END_PTR_TYPE  const  pPrmEnd = (EDD_UPPER_PRM_END_PTR_TYPE)pRQB->pParam;

            Status = EDDI_PrmCheckConsistency(pDDB, pPrmEnd->Local, pPrmEnd->IsModularPort, pDDB->PRM.PortparamsNotApplicable); // pPrmEnd->PortparamsNotApplicable);
            if (EDD_STS_OK != Status)
            {
                pDDB->PRM.StateFct = EDDI_PrmStateStart;
                return Status;
            }

            Status = EDDI_CheckPrmEndState(pDDB);
            if (EDD_STS_OK != Status)
            {
                pDDB->PRM.StateFct = EDDI_PrmStateStart;
                return Status;
            }

            pDDB->PRM.StateFct = EDDI_PrmStateEnd;

            return Status;
        }

        case EDD_SRV_PRM_COMMIT:
        {
            pDDB->PRM.StateFct = EDDI_PrmStateStart;
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Commit_eddiPrmStateWrite);
            return EDD_STS_ERR_SEQUENCE;
        }

        case EDD_SRV_PRM_PREPARE:
        {
            pDDB->PRM.StateFct = EDDI_PrmStatePrepare;
            return EDD_STS_OK;
        }

        case EDD_SRV_PRM_CHANGE_PORT:
            Status = EDDI_PRMRequestPrmChangePort(pRQB, pDDB);
            return Status;

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Default_eddiPrmStateWrite);
            return EDD_STS_ERR_SERVICE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateEnd( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                           EDD_SERVICE              const  Service )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB   = pHDB->pDDB;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateEnd->");

    switch (Service)
    {
        case EDD_SRV_PRM_CHANGE_PORT:
        case EDD_SRV_PRM_END:
        case EDD_SRV_PRM_WRITE:
        {
            pDDB->PRM.StateFct = EDDI_PrmStateStart;
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Write_End_eddiPrmStateEnd);
            return EDD_STS_ERR_SEQUENCE;
        }

        case EDD_SRV_PRM_COMMIT:
        {
            EDD_UPPER_PRM_COMMIT_PTR_TYPE const pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE)pRQB->pParam;
            LSA_UINT32                          UsrPortIndex;

            //check params
            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                if (   (EDD_PRM_APPLY_RECORD_PORT_PARAMS != pPrmCommit->ApplyDefaultPortparams[UsrPortIndex])
                    && (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != pPrmCommit->ApplyDefaultPortparams[UsrPortIndex]))
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMRequestPrmChangePort(): ApplyDefaultPortparams (%d) invalid (UsrPortIndex %d)",
                        pPrmCommit->ApplyDefaultPortparams[UsrPortIndex], UsrPortIndex);
                    return (EDD_STS_ERR_PARAM);
                }
            }

            pDDB->PRM.StateFct = EDDI_PrmStatePrepare;

            pDDB->PRM.Transition.bTransitionRunning = LSA_TRUE;
            pDDB->PRM.Transition.pRunningRQB        = pRQB;
            EDDI_MEMCOPY(pDDB->PRM.ApplyDefaultPortparams, pPrmCommit->ApplyDefaultPortparams, EDD_CFG_MAX_PORT_CNT);

            EDDI_PrmMoveBToAPart1(pDDB);

            #if defined (EDDI_RED_PHASE_SHIFT_ON)
            if (pDDB->PRM.bSyncIrDataChanged)
            {
                // A SOL-Transition is running
                // --> PRM-COMMIT must wait until this transition is finished, before FCW-Lists can be build again.
                // Part2 will be done later in NewCycleContext when SOL-Transition is over.
                return EDD_STS_OK_PENDING; //signalize RequestFinish will be done later
            }
            else
            {
                // No transition in PortStms --> Part2 can be executed directly
                EDDI_PrmMoveBToAPart2(pDDB);
                pDDB->PRM.Transition.bTransitionRunning = LSA_FALSE;
                pDDB->PRM.Transition.pRunningRQB        = EDDI_NULL_PTR;

                EDDI_GenCalcTransferEndValues(pDDB, 
                                              &pPrmCommit->RsvIntervalRed, 
                                              pDDB->PRM.PDIRData.pRecordSet_A, 
                                              pDDB->PRM.PDirApplicationData.pRecordSet_A, 
                                              pDDB->PRM.PDIRData.State_A, 
                                              pDDB->PRM.PDirApplicationData.State_A);
                return EDD_STS_OK;
            }
            #else
            EDDI_PrmMoveBToAPart2(pDDB);
            Status = EDD_STS_OK;
            #endif


        }

        case EDD_SRV_PRM_PREPARE:
        {
            pDDB->PRM.StateFct = EDDI_PrmStatePrepare;
            return EDD_STS_OK;
        }

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Default_eddiPrmStateEnd);
            return EDD_STS_ERR_SERVICE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPrmEndState( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  Status = EDD_STS_OK;

    if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRData.State_B)
    {
        /* --------------------------------------------------------------------------*/
        /* Check for FrameDataProperties                                             */
        /* --------------------------------------------------------------------------*/
        Status = EDDI_CheckFrameDataProperties(pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckFrameDataProperties, Result:0x%X", Status);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    
        /* --------------------------------------------------------------------------*/
        /* Check for FrameDataProperties                                             */
        /* --------------------------------------------------------------------------*/
        /* Attention: Fragmentation mode has to be set before                        */
        /* (EDDI_PrmCheckConsistency) Do not change order of execution!              */    
        #if defined (EDDI_CFG_DFP_ON) || defined (EDDI_CFG_FRAG_ON)
        if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRSubFrameData.State_B)
        {    
            LSA_UINT32 const CycleBaseFactor = EDDI_PrmGetCycleBaseFactor(pDDB);
            LSA_UINT32 const ActivePortCount = EDDI_PrmGetActivePortCount(pDDB);  
            LSA_UINT32 const FragSize        = EDDI_PrmGetFragSize(pDDB);

            //Not allowed: More then one active ports and cyclebasefactor smaller then 8 and no fragmentation
            if ((ActivePortCount > 1) && (CycleBaseFactor < 8) && (FragSize != EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF))
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, More then one Port is activate (given PortCnt:0x%X ) and CycleBaseFactor (given 0x%X) is smaller then 8", ActivePortCount, CycleBaseFactor);
                return EDD_STS_ERR_PRM_CONSISTENCY; 
            } 
        }
        #endif  
        
        /* --------------------------------------------------------------------------*/
        /* Set LinkState to Down for all other Ports for FastForwarding              */
        /* --------------------------------------------------------------------------*/ 
        Status = EDDI_SetLinkStatusForFastForwarding(pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetLinkStatusForFastForwarding, Result:0x%X", Status);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetSyncIrDataChange( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                                    EDDI_PRM_PDSYNC_DATA_PTR_TYPE  const  pPDSyncData,
                                                                    EDDI_PRM_PDIR_DATA_PTR_TYPE    const  pPDIRData )
{
    //compare only if current records exist and a new records was written
    if ( (pPDSyncData->State_A == EDDI_PRM_VALID) && (pPDSyncData->State_B == EDDI_PRM_WRITE_DONE) &&
         (pPDIRData->State_A   == EDDI_PRM_VALID) && (pPDIRData->State_B   == EDDI_PRM_WRITE_DONE) )   
    {     
        if ( (pPDSyncData->pRecordSet_B->PDSyncDataLength_All - pPDSyncData->pRecordSet_B->PaddingCount) == (pPDSyncData->pRecordSet_A->PDSyncDataLength_All - pPDSyncData->pRecordSet_A->PaddingCount)  )
        {
            //when current record and new record are identical then do not exceute commit
            if (!EDDI_PrmCompare(pDDB, (EDDI_LOCAL_MEM_U8_PTR_TYPE) (void*) pPDSyncData->pRecordSet_A->pNetWorkPDSyncDataRecord,
                                (EDDI_LOCAL_MEM_U8_PTR_TYPE) (void*) pPDSyncData->pRecordSet_B->pNetWorkPDSyncDataRecord,
                                (pPDSyncData->pRecordSet_B->PDSyncDataLength_All - pPDSyncData->pRecordSet_B->PaddingCount)))
            {
                //Old PDSyncData records are not equal 
                return LSA_TRUE;
            }
        }
        else
        {
            //Datalength was changed
            return LSA_TRUE;
        }

        //compare only if current records exist and a new records was written
        if (pPDIRData->pRecordSet_A->PDIRDataRecordActLen == pPDIRData->pRecordSet_B->PDIRDataRecordActLen)
        {
            //compare also the "hole" generated for UsedPortCnt < MAX_IRTE_PORT_CNT
            if (!EDDI_PrmCompare(pDDB, (EDDI_LOCAL_MEM_U8_PTR_TYPE)(void *)pPDIRData->pRecordSet_A->pNetWorkPDIRDataRecord,
                                (EDDI_LOCAL_MEM_U8_PTR_TYPE)(void *)pPDIRData->pRecordSet_B->pNetWorkPDIRDataRecord,
                                pPDIRData->pRecordSet_B->PDIRDataRecordActLen))
            {
                //Old PDDIRData records are not equal 
                return LSA_TRUE;
            }
        }
        else
        {
            //Datalength was changed        
            return LSA_TRUE;
        }
    }
    else
    {
        //PDIRData or PDSyncdata was not written
        return LSA_TRUE;
    } 

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmMoveBToAPart1()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmMoveBToAPart1( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB)
{
    EDDI_PRM_PDNC_DATA_CHECK                              *  pPDNC;
    EDDI_PRM_PDSCF_DATA_CHECK                             *  pPDSCF;
    EDDI_PRM_PDSYNC_DATA_PTR_TYPE                            pPDSyncData;
    EDDI_PRM_PDPORT_MRP_DATA_ADJUST_RECORD_PTR_TYPE          pPDPortDataAdjust;
    EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_RECORD_PTR_TYPE     pPDInterfaceMrpDataAdjust;
    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE                  *  pTmp;
    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE                 *  pPDSCFDataCheckTmp;
    EDDI_PRM_RECORD_SYNC_PTR_TYPE                            pPDSyncDataTmp;
    EDDI_PRM_PDNRT_LOAD_LIMITATION                        *  pPDNRTFill;
    LSA_UINT32                                               UsrPortIndex, HwPortIndex;
    LSA_UINT32                                               MACAddrLow;
    LSA_UINT32                                        const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_BOOL                                                 PortStateSet[EDDI_MAX_IRTE_PORT_CNT];
    LSA_BOOL                                                 MrpDisabled = LSA_TRUE;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart1->");

    pDDB->PRM.bCommitExecuted            = LSA_TRUE;

    pDDB->PRM.DoFireIndication_Interface = LSA_FALSE;   /* reset lost indication state */

    /* --------------------------------------------------------------------------*/
    /* PDNCDataCheck Record                                                      */
    /* --------------------------------------------------------------------------*/
    pPDNC = &pDDB->PRM.PDNCDataCheck;

    pPDNC->Last_Event_Appears    = EDDI_PRM_EV_START;    /* reset last Appear event */
    pPDNC->Last_Event_Disappears = EDDI_PRM_EV_START;    /* reset last Disappear event */
    pPDNC->bPDNCChanged          = LSA_FALSE;            /* reset PDNC mismatch state */

    if (pPDNC->State_B != EDDI_PRM_WRITE_DONE)
    {
        // deactivate Record-Set A
        pPDNC->State_A = EDDI_PRM_NOT_VALID;
    }
    else
    {
        // move record-set B to A
        pTmp      = pPDNC->pA;
        pPDNC->pA = pPDNC->pB;
        pPDNC->pB = pTmp;

        // activate record-set A
        pPDNC->State_A = EDDI_PRM_VALID;

        // move drop-budget value-set B to A
        pPDNC->DropBudget_A = pPDNC->DropBudget_B;

        // read initial value for drop counter
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pPDNC->StartValue_HOL[UsrPortIndex]  = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_HOLBC, pDDB);
            pPDNC->StartValue_DROP[UsrPortIndex] = EDDI_SwiPortGetDropCtr(HwPortIndex, pDDB);
        }

        // set initial state for PDNC state-machine
        pPDNC->DiagStateFct = EDDI_PrmStateDiagPDNCStart;

        EDDI_PRMCheckTimeOut(pDDB);
    }

    /* --------------------------------------------------------------------------*/
    /* PDSCFDataCheck Record                                                     */
    /* --------------------------------------------------------------------------*/
    pPDSCF = &pDDB->PRM.PDSCFDataCheck;

    if (pPDSCF->State_B != EDDI_PRM_WRITE_DONE)
    {
        // deactivate Record-Set A
        pPDSCF->State_A = EDDI_PRM_NOT_VALID;
    }
    else
    {
        // move record-set B to A
        pPDSCFDataCheckTmp  = pPDSCF->pA;
        pPDSCF->pA          = pPDSCF->pB;
        pPDSCF->pB          = pPDSCFDataCheckTmp;

        // activate record-set A
        pPDSCF->State_A = EDDI_PRM_VALID;
    }

    /* --------------------------------------------------------------------------*/
    /* PDSetDefaultPortStates                                                    */
    /* --------------------------------------------------------------------------*/

    for (UsrPortIndex = 0; UsrPortIndex < EDDI_MAX_IRTE_PORT_CNT; UsrPortIndex++)
    {
        /* flag for MRP handling to know if PortState still setup */
        PortStateSet[UsrPortIndex] = LSA_FALSE;
    }

    if (pDDB->PRM.PDSetDefaultPortStates.RecordState_B == EDDI_PRM_WRITE_DONE)
    {
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            /* PortState present for Port? */
            if (pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[UsrPortIndex].PortStatePresent)
            {
                LSA_UINT16  PortState = EDD_PORT_STATE_FORWARDING;

                switch (pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[UsrPortIndex].PortState)
                {
                    case EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_DISABLE:
                    {
                        PortState = EDD_PORT_STATE_DISABLE;
                        break;
                    }
                    case EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_BLOCKING:
                    {
                        PortState = EDD_PORT_STATE_BLOCKING;
                        break;
                    }
                    default:
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmMoveBToAPart1, PDSetDefaultPortStates, UsrPortIndex:0x%X Invalid PortState:%d", 
                                          UsrPortIndex, pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[UsrPortIndex].PortState);
                        EDDI_Excp("EDDI_PrmMoveBToAPart1, PDSetDefaultPortStates", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }
                }

                HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmMoveBToAPart1, PDSetDefaultPortStates, Set PortState of HwPortIndex:%d to %d", HwPortIndex, PortState);

                /* change the PortState only if not Linkdown. If LinkDown we only store the */
                /* state for a later LinkUp. While Link Down the DISABLED-State shall not   */
                /* be changed!                                                              */

                if (pDDB->SWITCH.LinkPx[HwPortIndex].LinkStatus != EDD_LINK_DOWN)
                {
                    /* Set the State (this will also set the State for ext-Link Indication!) */
                    EDDI_SwiPortSetSpanningTree(HwPortIndex, PortState, pDDB);
                }

                /* Save the actual PortState within management for restore after a LinkDown */
                /* (On LinkDown the Port is temporary set to CLOSED)                        */

                pDDB->SWITCH.LinkPx[HwPortIndex].SpanningTreeState = PortState;

                PortStateSet[UsrPortIndex] = LSA_TRUE; /* for later use with MRP! */
            }

            /* copy set B to A */
            pDDB->PRM.PDSetDefaultPortStates.RecordSet_A[UsrPortIndex] = pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[UsrPortIndex];
        }
    }

    /* copy state of B to A */
    pDDB->PRM.PDSetDefaultPortStates.RecordState_A = pDDB->PRM.PDSetDefaultPortStates.RecordState_B;


    /* --------------------------------------------------------------------------*/
    /* PDirApplicationData                                                       */
    /* --------------------------------------------------------------------------*/
    pDDB->PRM.PDirApplicationData.pRecordSet_A = pDDB->PRM.PDirApplicationData.pRecordSet_B;
    
    if (pDDB->PRM.PDirApplicationData.State_B == EDDI_PRM_WRITE_DONE)
    {
        /* copy recordset of B to A */
        pDDB->PRM.PDirApplicationData.State_A = EDDI_PRM_VALID;
    }
    else
    {
        pDDB->PRM.PDirApplicationData.State_A = EDDI_PRM_NOT_VALID;
    }

    /* --------------------------------------------------------------------------*/
    /* PDInterfaceMrpDataAdjust, PDPortMrpDataAdjust                             */
    /* --------------------------------------------------------------------------*/

    if (pDDB->SWITCH.bMRPRedundantPortsUsed) /* MRP used? */
    {
        /* if only a Interface record is present MRP is disabled! */
        if ( ( pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B == EDDI_PRM_WRITE_DONE ) &&
             (!pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB))
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PrmMoveBToAPart1, MRP is disabled (only IF-Record present).");
            MrpDisabled = LSA_TRUE;
        }
        else
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmMoveBToAPart1, MRP is NOT disabled.");
            MrpDisabled = LSA_FALSE;
        }
    }
    
    if (!pDDB->PRM.bFirstCommitExecuted) 
    {
        /* only done on first COMMIT after startup! It is not determined wether MRP or EDDI get to evaluate the records first. 
           If MRP is first, then EDDI will set the port set by MRP to FORWARDING, to BLOCKED again! */
        LSA_UINT16  PortState;

        /* If MRP is used we set all none MRP-Ports to FORWARDING and all MRP Ports */
        /* to BLOCKING.                                                             */

        if (pDDB->SWITCH.bMRPRedundantPortsUsed) /* MRP used? */
        {
            if (MrpDisabled)
            {
                /* enable MRP-MC-forwarding (we have no MRP) */
                EDDI_SetMRPMulticast(pDDB,EDD_MULTICAST_FWD_ENABLE);
            }
            else
            {
                /* disable MRP-MC-forwarding (we have MRP) */
                EDDI_SetMRPMulticast(pDDB,EDD_MULTICAST_FWD_DISABLE);
            }

            /* set PortState for every Port */
            for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
            {
                HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                /* Three cases: */
                /* 1. no MRP-Record present          : MRP enabled use DSB setup MRP ports */
                /* 2. only interface record          : MRP disabled                        */
                /* 3. 2 MRP-Port records/ 1 interface: MRP enabled with this ports         */

                PortState = EDD_PORT_STATE_FORWARDING;

                /* Note: PDSetDefaultPortStates is only present if Mrp is disabled! this was */
                /*       checked on PRM-END!                                                 */

                if (!MrpDisabled)
                {
                    if (pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B == EDDI_PRM_WRITE_DONE)
                    {
                        /* MRP used. Is this a R-Port? (= record present) */
                        if (pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex] == EDDI_PRM_WRITE_DONE)
                        {
                            PortState = EDD_PORT_STATE_BLOCKING;
                        }
                    }
                    else
                    {
                        /* No MRP-Records (we already checked that no MRPPort records present two)*/
                        /* -> use DSB defaults                                                    */
                        if (pDDB->SWITCH.LinkPx[HwPortIndex].bMRPRedundantPort)
                        {
                            PortState = EDD_PORT_STATE_BLOCKING;
                        }
                    }
                }

                /* Set PortState only if not already set by PDSetDefaultPortState!. Note that */
                /* the PDSetDefaultPortState is only present if MRP is disabled               */

                if (!PortStateSet[UsrPortIndex])
                {
                    if (PortState == EDD_PORT_STATE_BLOCKING)
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmMoveBToAPart1, MRP: Setting PortState to BLOCKING (HWPortIdx:%d UsrPortIdx:%d)", HwPortIndex, UsrPortIndex);
                    }
                    else
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmMoveBToAPart1, MRP: Setting PortState to FORWARDING (HWPortIdx:%d UsrPortIdx:%d", HwPortIndex, UsrPortIndex);
                    }

                    /* change the PortState only if not Linkdown. If LinkDown we only store the */
                    /* state for a later LinkUp. While Link Down the DISABLED-State shall not   */
                    /* be changed!                                                              */

                    if (pDDB->SWITCH.LinkPx[HwPortIndex].LinkStatus != EDD_LINK_DOWN )
                    {
                        /* Set the State (this will also set the State for ext-Link Indication!) */
                        EDDI_SwiPortSetSpanningTree(HwPortIndex, PortState, pDDB);
                    }

                    /* Save the actual PortState within management for restore after a LinkDown */
                    /* (On LinkDown the Port is temporary set to CLOSED)                        */

                    pDDB->SWITCH.LinkPx[HwPortIndex].SpanningTreeState = PortState;
                }

                /* copy set B to A */
                pDDB->PRM.PDPortMrpDataAdjust.RecordState_A[UsrPortIndex] = pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex];

            } /* for */

            /* copy set B to A */
            pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_A = pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B;

            /* We need a trigger for ext-link indication!                         */
            /* The trigger for Ext-Link Indication will be done on end of COMMIT! */
        }
    }

    /* --------------------------------------------------------------------------*/
    /* PDInterfaceMrpData Record                                                 */
    /* --------------------------------------------------------------------------*/
    pPDInterfaceMrpDataAdjust = &pDDB->PRM.PDInterfaceMrpDataAdjust;

    if (pPDInterfaceMrpDataAdjust->RecordState_B != EDDI_PRM_WRITE_DONE)
    {
        pPDInterfaceMrpDataAdjust->RecordState_A = EDDI_PRM_NOT_VALID;
    }
    else
    {
        /* record exist */
        pPDInterfaceMrpDataAdjust->RecordState_A = EDDI_PRM_VALID;
    }

    /* --------------------------------------------------------------------------*/
    /* Update the PRIO of MC SYNC-Frames depending on actual PRIO which may have */
    /* changed above!                                                            */
    /* --------------------------------------------------------------------------*/
    EDDI_SwiPNUpdateSyncMACPrio(pDDB);

    /* --------------------------------------------------------------------------*/
    /* PDPort Record                                                             */
    /* --------------------------------------------------------------------------*/
    // Reset Boundary-checking
    pDDB->PM.DomainBoundaryBLNotAllowed = LSA_FALSE;

    #if defined (EDDI_CFG_REV7)
    //clear all Borderline-Bits in FDB (default-state)
    EDDI_SwiPNClearAllBL(pDDB);
    #endif

    #if defined (EDDI_CFG_FRAG_ON)
    //check PRM data for NRT TX fragmentation
    EDDI_NRTFragCheckPrmCommitPart1(pDDB);
    #endif

    pPDPortDataAdjust = &pDDB->PRM.PDPortDataAdjust;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {       
        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pPDPortDataAdjust->RecordState_B[UsrPortIndex] != EDDI_PRM_WRITE_DONE)
        {
            /* default settings      */
            /* use default (Autoneg) */
            /* use default (PowerOn) */
            pPDPortDataAdjust->RecordState_A[UsrPortIndex] = EDDI_PRM_NOT_VALID;

            EDDI_SetMauType(UsrPortIndex, HwPortIndex, (LSA_UINT8)EDDI_PHY_POWER_ON, (LSA_UINT8)EDD_LINK_AUTONEG, pDDB);

            /* default if no block present..  forward all */
            EDDI_SetMulticast(HwPortIndex, 0xFFFFFFFFUL, pDDB);
            EDDI_SetDCPMulticast(HwPortIndex, 0xFFFFFFFFUL, pDDB);

            continue;
        }

        /* record exist */
        pPDPortDataAdjust->RecordState_A[UsrPortIndex] = EDDI_PRM_VALID;

        /* copy set B to A */
        pPDPortDataAdjust->RecordSet_A[UsrPortIndex] = pPDPortDataAdjust->RecordSet_B[UsrPortIndex];

        /* ----------------------------------------------------------------------*/
        /* MulticastBoundary                                                     */
        /* ----------------------------------------------------------------------*/
        if (pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MulticastBoundaryPresent)
        {
            /* 1 block the multicast MAC address 01-0E-CF-00-02-00 */
            /* a "1" in MulticastBoundaries in the record means "block this address",
               a "1" in MACAddrLow means "do not block this address" */
            MACAddrLow = (~pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MulticastBoundary);
        }
        else
        {
            /* record not exist */
            /* default if no block present..  forward all */
            MACAddrLow = 0xFFFFFFFFUL;
        }

        EDDI_SetMulticast(HwPortIndex, MACAddrLow, pDDB);

        /* ----------------------------------------------------------------------*/
        /* DCPBoundary                                                           */
        /* ----------------------------------------------------------------------*/
        if (pPDPortDataAdjust->RecordSet_A[UsrPortIndex].DCPBoundaryPresent)
        {
            /* 1 block the multicast MAC address 01-0E-CF-00-00-0x */
            MACAddrLow = (~pPDPortDataAdjust->RecordSet_A[UsrPortIndex].DCPBoundary);
        }
        else
        {
            /* record not exist */
            /* default if no block present..  forward all */
            MACAddrLow = 0xFFFFFFFFUL;
        }

        EDDI_SetDCPMulticast(HwPortIndex, MACAddrLow, pDDB);

        /* ----------------------------------------------------------------------*/
        /* PortState and MAUType                                                 */
        /* ----------------------------------------------------------------------*/
        if (!pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MAUTypePresent &&
            !pPDPortDataAdjust->RecordSet_A[UsrPortIndex].PortStatePresent)
        {
            /* if no PortState and no MAUType exist */
            /* use default (Autoneg) */
            /* use default (PowerOn) */
            LSA_UINT8   LinkStat = EDD_LINK_AUTONEG;

            if (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->PRM.ApplyDefaultPortparams[UsrPortIndex])
            {
                //Apply default port parameters
                LinkStat = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType) ? EDD_LINK_100MBIT_FULL : EDD_LINK_AUTONEG;
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_PrmMoveBToAPart1, Default params (Usecase: AUTONEG) applied to PortIndex %d, MediaType:0x%X",
                                  UsrPortIndex, pDDB->Glob.PortParams[HwPortIndex].MediaType);
            }
            else
            {
                //Apply AutonegMapping for optical ports
                LinkStat = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType) ? (LSA_UINT8)(pDDB->Glob.PortParams[HwPortIndex].AutonegMappingCapability & 0xFFUL) : EDD_LINK_AUTONEG;
            }

            EDDI_SetMauType(UsrPortIndex, HwPortIndex, (LSA_UINT8)EDDI_PHY_POWER_ON, LinkStat, pDDB);
        }
        else if (pPDPortDataAdjust->RecordSet_A[UsrPortIndex].PortStatePresent)
        {
            /* equivalent for pPDPortDataAdjust->RecordSet_A[UsrPortIndex].PortState == EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PORTE_STATE_DOWN */
            /* if only PortState -> PowerOff */
            EDDI_SetMauType(UsrPortIndex, HwPortIndex, (LSA_UINT8)EDDI_PHY_POWER_OFF, (LSA_UINT8)EDD_LINK_UNCHANGED, pDDB);
        }
        else if (pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MAUTypePresent)
        {
            LSA_UINT8   LinkStat = pPDPortDataAdjust->RecordSet_A[UsrPortIndex].LinkStat;
            if (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->PRM.ApplyDefaultPortparams[UsrPortIndex])
            {
                LinkStat = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType) ? EDD_LINK_100MBIT_FULL : EDD_LINK_AUTONEG;
                EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                  "EDDI_PrmMoveBToAPart1, Default params (Usecase: MAUType:0x%X) applied to PortIndex %d, MediaType:0x%X",
                                  pPDPortDataAdjust->RecordSet_A[UsrPortIndex].MAUType, UsrPortIndex, pDDB->Glob.PortParams[HwPortIndex].MediaType);
            }
            /* if only MauType -> Speed/Mode and PowerOn */
            EDDI_SetMauType(UsrPortIndex, HwPortIndex, (LSA_UINT8)EDDI_PHY_POWER_ON, LinkStat, pDDB);
        }
    }
        
    /* --------------------------------------------------------------------------*/
    /* PDIRData Record                                                           */
    /* --------------------------------------------------------------------------*/
    //pPDIRData = &pDDB->PRM.PDIRData;
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {       
        LSA_UINT32  NRTCtrl, NRTCtrlPre;

        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //Disable MC Learning if this port has a red phase enabled AND MRP is active for this port
        //Reason: a RTC3 frame accidentally slipped from red to green shall not be forwarded.
        //Side effect: Renewing the MACTable entries for DCP may take at least 2 aging cycles!
        NRTCtrl    = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);
        NRTCtrlPre = NRTCtrl;
        if (   (!MrpDisabled)
            && (EDD_MRP_NO_RING_PORT != pDDB->Glob.PortParams[HwPortIndex].MRPRingPort)
            && (   (pDDB->PRM.PDIRData.pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[0].Rx.GreenPeriodBegin)
                || (pDDB->PRM.PDIRData.pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[0].Tx.GreenPeriodBegin)))
        {
            //switch MC-learning off
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__MCLearningEn, 0);
        }
        else
        {
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__MCLearningEn, (LSA_FALSE != pDDB->pConstValues->MCLearningEn ? 1:0));
        }
        if (NRTCtrlPre != NRTCtrl)
        {
            EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, NRTCtrl, pDDB);
        }     
    }
        
    /* --------------------------------------------------------------------------*/
    /* PDSyncData Record                                                         */
    /* --------------------------------------------------------------------------*/
    pPDSyncData = &pDDB->PRM.PDSyncData;
    
    /* ----------------------------------------------------------------------------*/
    /* If PDIRData and/or PDSync-Data changed...                                   */
    /* ----------------------------------------------------------------------------*/
    
    if (pDDB->PRM.bSyncIrDataChanged)
    {
        //pPDSyncData->Last_Event_Appears      = EDDI_PRM_EV_START_PDSYNC;  /* reset last Appear event */            PK: RTSync
        //pPDSyncData->Last_Event_Disappears   = EDDI_PRM_EV_START_PDSYNC;  /* reset last Disappear event */         PK: RTSync
        //pPDSyncData->Appear_Event_Execute    = EDDI_PRM_EV_START_PDSYNC;  /* reset Appear Event No Execute */      PK: RTSync
        //pPDSyncData->bPDSyncChanged          = LSA_FALSE;                 /* reset PDSync mismatch state */        PK: RTSync

        if (pPDSyncData->State_B != EDDI_PRM_WRITE_DONE)
        {
            // deactivate record-set A
            pPDSyncData->State_A = EDDI_PRM_NOT_VALID;
        }
        else
        {
            // move new record-set B to A
            pPDSyncDataTmp            = pPDSyncData->pRecordSet_A;
            pPDSyncData->pRecordSet_A = pPDSyncData->pRecordSet_B;
            pPDSyncData->pRecordSet_B = pPDSyncDataTmp;

            // activate record-set A
            pPDSyncData->State_A      = EDDI_PRM_VALID;

            // set initial state for Sync-Mismatch state-machine
            //pPDSyncData->DiagStateFct  = eddi_PrmStateDiag_PDSync_Start;    PK: RTSync
        }

        /* --------------------------------------------------------------------------*/
        /* Reset SyncPortStms                                                        */
        /* --------------------------------------------------------------------------*/
        {
            LSA_BOOL  bRTSync_isBoundary[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

            // Set ReservedIntervalParameters
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin = 0;
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd   = 0;

            // Before Removing FCW-Elements in EDDI_SyncIrDeactivate() we have to shutdown all FCW-Lists by
            // calling EDDI_SyncPortStmsPrmChange()
            // This includes a wait state of 1 cycle after disabling IRTCTROL-Bits to guaranty that
            // there are no more activities in the FCW-lists.

            EDDI_SyncPortStmsPrmChange(pDDB, LSA_FALSE, &bRTSync_isBoundary[0], &bRTSync_isBoundary[0], LSA_TRUE, EDDI_PrmTransitionDoneCbf); // Reset of IRT_PORT_STM !
        }
    }
    else
    {
        /* when current record and new record are identical - actual PDSyncMismatch  */
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart1, PDIR and PDSync records are identical");

        //eddi_PrmSendIndication_PDSync(pPDSyncData, pPDSyncData->Appear_Event_Execute, EDDI_PRM_EV_START_PDSYNC, pDDB);  PK: RTSync
    }

    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart1<-bSyncIrDataChanged:0x%X", pDDB->PRM.bSyncIrDataChanged); 


    /* --------------------------------------------------------------------------*/
    /* PDNRT_FEDDIN_LOAD_LIMITATION Record                                       */
    /* --------------------------------------------------------------------------*/
    pPDNRTFill = &pDDB->PRM.PDNrtLoadLimitation;

    if (pPDNRTFill->State_B != EDDI_PRM_WRITE_DONE)
    {
        // deactivate Record-Set A
        pPDNRTFill->State_A = EDDI_PRM_NOT_VALID;
        // set default values
        pDDB->NRT.bFeedInLoadLimitationActive = pDDB->NRT.bFeedInLoadLimitationActiveDefault;
        pDDB->NRT.bIO_Configured              = pDDB->NRT.bIO_ConfiguredDefault;
    }
    else
    {
        // move record-set B to A
        EDDI_PRM_PDNRT_LOAD_LIMITATION_PTR_TYPE  pExchange;
        
        pExchange                = pPDNRTFill->pRecordSet_A;
        pPDNRTFill->pRecordSet_A = pPDNRTFill->pRecordSet_B;
        pPDNRTFill->pRecordSet_B = pExchange;

        // activate record-set A
        pPDNRTFill->State_A = EDDI_PRM_VALID;

        pDDB->NRT.bFeedInLoadLimitationActive = pPDNRTFill->pRecordSet_A->LoadLimitationActive ? LSA_TRUE : LSA_FALSE;
        pDDB->NRT.bIO_Configured              = pPDNRTFill->pRecordSet_A->IO_Configuration     ? LSA_TRUE : LSA_FALSE;
    }
    //Apply limits
    EDDI_NrtSetSendLimit(pDDB); 
    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart1 : LoadLimitationActive(%u), IO_Configured(%u)", 
                      pDDB->NRT.bFeedInLoadLimitationActive, pDDB->NRT.bIO_Configured); 
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmMoveBToAPart2()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmMoveBToAPart2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PRM_PDIR_DATA_PTR_TYPE           const  pPDIRData         = &pDDB->PRM.PDIRData;
    EDDI_PRM_PDSYNC_DATA_PTR_TYPE         const  pPDSyncData       = &pDDB->PRM.PDSyncData;
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_PRM_PDIR_SUBFRAME_DATA_PTR_TYPE  const  pPDIRSubFrameData = &pDDB->PRM.PDIRSubFrameData;
    #endif
    LSA_RESULT                                   Result;
    LSA_UINT32                                   UsrPortIndex;
    LSA_BOOL                                     PortChanged[EDDI_MAX_IRTE_PORT_CNT+1];

    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart2->pSyncIrDataChanged:0x%X", pDDB->PRM.bSyncIrDataChanged);

    if (pDDB->PRM.bSyncIrDataChanged)
    {
        /* --------------------------------------------------------------------------*/
        /* PDIRData Record                                                           */
        /* --------------------------------------------------------------------------*/

        //only remove IRT-FCWs if RecordSet_A is in use
        if (pDDB->SYNC.IrtActivity)
        {
            //remove IRT-FCWs
            Result = EDDI_SyncIrDeactivate(pDDB);

            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmMoveBToAPart2, SyncIr_EndRecord_B, Result:0x%X", Result);
                EDDI_Excp("EDDI_PrmMoveBToAPart2, SyncIr_EndRecord_B", EDDI_FATAL_ERR_EXCP, Result, 0);
                return;
            }
        }

        if (pPDIRData->State_B != EDDI_PRM_WRITE_DONE)
        {
            //no new data available
            //--> simply deactivate record-set A
            pPDIRData->State_A = EDDI_PRM_NOT_VALID;
        }
        else
        {
            EDDI_PRM_RECORD_IRT_PTR_TYPE  pPDIRDataTmp;

            //new data available
            //--> move new record-set B to A
            pPDIRDataTmp            = pPDIRData->pRecordSet_A;
            pPDIRData->pRecordSet_A = pPDIRData->pRecordSet_B;
            pPDIRData->pRecordSet_B = pPDIRDataTmp;

            //add IRT-FCWs
            Result = EDDI_SyncIrActivate(pDDB);

            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmMoveBToAPart2, EDDI_SyncIrActivate, Result:0x%X", Result);
                EDDI_Excp("EDDI_PrmMoveBToAPart2, EDDI_SyncIrActivate", EDDI_FATAL_ERR_EXCP, Result, 0);
                return;
            }

            //activate record-set A
            pPDIRData->State_A = EDDI_PRM_VALID;
        }
          
        #if defined (EDDI_CFG_DFP_ON)
        //Cleanup PF and SF
        EDDI_DFPPackFrameHandlerCleanUp(pDDB, pPDIRSubFrameData->pRecordSet_A);        
        
        if (pPDIRSubFrameData->State_B != EDDI_PRM_WRITE_DONE)
        {
            //no new data available
            //--> simply deactivate record-set A
            pPDIRSubFrameData->State_A = EDDI_PRM_NOT_VALID;
        }
        else
        {
            EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  pPDIRSubFrameDataTmp;

            //new data available
            //--> move new record-set B to A
            pPDIRSubFrameDataTmp            = pPDIRSubFrameData->pRecordSet_A;
            pPDIRSubFrameData->pRecordSet_A = pPDIRSubFrameData->pRecordSet_B;
            pPDIRSubFrameData->pRecordSet_B = pPDIRSubFrameDataTmp;

            //activate record-set A
            pPDIRSubFrameData->State_A = EDDI_PRM_VALID;
        }
        #endif

        if (EDD_FEATURE_ENABLE == pDDB->CRT.CfgPara.bUseTransferEnd)
        {
            /* --------------------------------------------------------------------------*/
            /* Setup the IRT_TimeCompare Register of ERTEC depending on actual setting. .*/
            /* Must be done after record state of set A is up to date!                   */
            /* Note that the DeadLine and TriggerMode will be setup on Sendclockchange to*/
            /* startup behaviour (like no records present)                               */
            /* --------------------------------------------------------------------------*/
            LSA_UINT32  TriggerDeadLine_10ns,Help;
            LSA_UINT16  TriggerMode;

            EDDI_PrmCalculateTriggerDeadLine(pDDB,pDDB->CRT.CycleBaseFactor,&TriggerDeadLine_10ns,&TriggerMode);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart2, Setting IRT_COMP_TIME to %d (Mode:0x%X)", TriggerDeadLine_10ns, TriggerMode);

            Help = IO_x32(IMAGE_MODE);
            EDDI_SetBitField32(&Help, EDDI_SER_IMAGE_MODE_BIT__SyncAPI, TriggerMode); /* does big/little endian handling! */
            IO_x32(IMAGE_MODE) = Help;

            IO_W32(IRT_COMP_TIME, TriggerDeadLine_10ns); //setup register

            /* save actual setting of TriggerDeadLine (just for reference)                */
            /* Dont set the TriggerMode within cfg because it will stay on startup setting*/
            /* for use with sendclock-change!                                             */
            pDDB->CRT.SyncImage.ImageExpert.TriggerDeadlineIn10ns = TriggerDeadLine_10ns;
        }
    }

    //Calculate TransferEnd values
    EDDI_GenCalcTransferEndValues(pDDB, 
                                  &pDDB->PRM.PDirApplicationData.pRecordSet_A->TransferEndValues, 
                                  pDDB->PRM.PDIRData.pRecordSet_A, 
                                  pDDB->PRM.PDirApplicationData.pRecordSet_A, 
                                  pDDB->PRM.PDIRData.State_A, 
                                  pDDB->PRM.PDirApplicationData.State_A);  

    #if defined (EDDI_CFG_FRAG_ON)
    //check PRM data for NRT TX fragmentation
    EDDI_NRTFragCheckPrmCommitPart2(pDDB);
    #endif

    /* ----------------------------------------------------------------------------*/
    /* Signal to Port-STMs ReservedInterval and DomainBoundarys.                   */
    /* Note: We always trigger the machine with actual settings. If no change      */
    /*       is present it doesnt matter. But it is possible that irdata and       */
    /*       syncdata are not changed but the domain-boundary!                     */
    /* ----------------------------------------------------------------------------*/

    {
        LSA_BOOL            RtClass2_isReservedIntervalPresent;
        LSA_BOOL            RTSync_isBoundaryEgress  [EDDI_MAX_IRTE_PORT_CNT] = { 0 };
        LSA_BOOL            RTSync_isBoundaryIngress [EDDI_MAX_IRTE_PORT_CNT] = { 0 };
        LSA_BOOL            RTSync_isBoundaryEgress1 [EDDI_MAX_IRTE_PORT_CNT];
        LSA_BOOL            RTSync_isBoundaryIngress1[EDDI_MAX_IRTE_PORT_CNT];
        LSA_UINT32   const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
        #if defined (EDDI_CFG_REV7)
        LSA_UINT32          MACBandBeginnIndex=0;
        #endif

        //Check RTClass2 reserved interval
        if (   (pPDSyncData->State_A == EDDI_PRM_VALID)
            && (pPDSyncData->pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd != 0))
        {
            //Set ReservedIntervalParameters
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin = pPDSyncData->pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalBegin;
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd   = pPDSyncData->pRecordSet_A->pLocalPDSyncDataRecord->ReservedIntervalEnd;

            RtClass2_isReservedIntervalPresent = LSA_TRUE;
        }
        else
        {
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin = 0;
            pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd   = 0;

            RtClass2_isReservedIntervalPresent = LSA_FALSE;
        }

        //Get all Boundaries
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32                                 const  HwPortIndex    = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
            EDDI_SYNC_PORT_MACHINES_INPUT           *  const  pInput         = &pDDB->SYNC.PortMachines.Input;
            EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE     const  pPortInputData = &pInput->PortData[HwPortIndex];


            pDDB->SYNC.PortMachines.SyncFwd_Machine[HwPortIndex].FirstRun=LSA_TRUE;

            EDDI_PrmGetDomainBoundary(pDDB,
                                      UsrPortIndex,
                                      LSA_TRUE /*bSyncID0*/, 
                                      &RTSync_isBoundaryIngress[HwPortIndex],
                                      &RTSync_isBoundaryEgress[HwPortIndex]);
            
            EDDI_PrmGetDomainBoundary(pDDB,
                                      UsrPortIndex,
                                      LSA_FALSE /*bSyncID0*/, 
                                      &RTSync_isBoundaryIngress1[HwPortIndex],
                                      &RTSync_isBoundaryEgress1[HwPortIndex]);

            #if defined (EDDI_CFG_REV7)
            //Set PortState for 1st start (coldstart)
            MACBandBeginnIndex=(EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX* EDDI_MAX_MACADR_BAND_SIZE);
            EDDI_PrmSetBLTableforPrmUsr(pDDB,UsrPortIndex,MACBandBeginnIndex,MAC_PTP_SYNC_LEN,RTSync_isBoundaryIngress[HwPortIndex],RTSync_isBoundaryEgress[HwPortIndex]);

            MACBandBeginnIndex=EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX* EDDI_MAX_MACADR_BAND_SIZE;
            EDDI_PrmSetBLTableforPrmUsr(pDDB,UsrPortIndex,MACBandBeginnIndex,MAC_PTP_SYNC_WITH_FU_LEN,RTSync_isBoundaryIngress[HwPortIndex],RTSync_isBoundaryEgress[HwPortIndex]);

            MACBandBeginnIndex=EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX* EDDI_MAX_MACADR_BAND_SIZE;
            EDDI_PrmSetBLTableforPrmUsr(pDDB,UsrPortIndex,MACBandBeginnIndex,MAC_PTP_FOLLOW_UP_LEN,RTSync_isBoundaryIngress[HwPortIndex],RTSync_isBoundaryEgress[HwPortIndex]);


            //check DomainBoundaryIngress/DomainBoundaryEngress: if on any port egress- without igress-filtering is set ==> disable borderlines
            if (   (pDDB->PRM.PDPortDataAdjust.RecordState_A[UsrPortIndex] == EDDI_PRM_VALID)
                && (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryPresent))
            {
                if (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryEgress != pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].DomainBoundaryIngress)
                {
                    pDDB->PM.DomainBoundaryBLNotAllowed = LSA_TRUE;
                }
            }
            #endif

            //set igress/egress for all ports for SyncID1
            pPortInputData->RTSync_isBoundaryIngress[1] = RTSync_isBoundaryIngress1[HwPortIndex];
            pPortInputData->RTSync_isBoundaryEgress[1]  = RTSync_isBoundaryEgress1[HwPortIndex];
        }

        EDDI_SyncPortStmsPrmChange(pDDB,
                                   RtClass2_isReservedIntervalPresent,
                                   &RTSync_isBoundaryIngress[0],
                                   &RTSync_isBoundaryEgress[0],
                                   LSA_FALSE,
                                   EDDI_NULL_PTR);  // RESET not needed here (Reset was done
                                                    // before in EDDI_PrmMoveBToAPart1 or
                                                    // RESET is not needed when we have our first
                                                    // PRM_COMMIT after DeviceSetup.
    }

    /* ----------------------------------------------------------------------------*/
    /* Trigger a link indication for all ports. maybe some event is still pending  */
    /* ----------------------------------------------------------------------------*/
    for (UsrPortIndex = 0; UsrPortIndex < EDDI_MAX_IRTE_PORT_CNT; UsrPortIndex++)
    {
        PortChanged[UsrPortIndex] = (UsrPortIndex < EDD_CFG_MAX_PORT_CNT)?LSA_TRUE:LSA_FALSE;
    }

    //check and gen LinkIndExt if neccessary
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    pDDB->PRM.bFirstCommitExecuted = LSA_TRUE; //at least the first commit after startup now executed

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmMoveBToAPart2<-");
}
/*---------------------- end [subroutine] ---------------------------------*/
#if defined (EDDI_CFG_REV7)
/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmSetBLTableforPrmUsr()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_PrmSetBLTableforPrmUsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, LSA_UINT32 const  UsrPortIndex, LSA_UINT32 const MACBandBeginnIndex, 
                                                                  LSA_UINT32 const MACBandLength, LSA_BOOL const isBoundaryIngress,LSA_BOOL const isBoundaryEgress  )
{
    LSA_UINT32 MacIndex = MACBandBeginnIndex;

    for (; MacIndex < (MACBandBeginnIndex+MACBandLength); MacIndex++)
    {
        if (isBoundaryIngress && isBoundaryEgress)
        {
            pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_PRM][MacIndex].PortState[UsrPortIndex] = EDDI_BL_IGRESS_EGRESS;
        }
        else if (isBoundaryEgress)
        {
            pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_PRM][MacIndex].PortState[UsrPortIndex] = EDDI_BL_EGRESS;
        }
        else
        {
            pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_PRM][MacIndex].PortState[UsrPortIndex] = EDDI_BL_FW;
        }
    }  
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmDeleteAllB()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmDeleteAllB( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                                              UsrPortIndex;
    EDDI_PRM_PDNC_DATA_CHECK                             *  pPDNC;
    EDDI_PRM_PDIR_DATA                                   *  pPDIR;
    EDDI_PRM_PDSYNC_DATA                                 *  pPDSync;
    EDDI_PRM_PDSCF_DATA_CHECK                            *  pPDSCF;
    EDDI_PRM_PDPORT_MRP_DATA_ADJUST_RECORD_PTR_TYPE         pPDPortDataAdjust;
    EDDI_PRM_PDNRT_LOAD_LIMITATION                       *  pPDNRTFill;
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_PRM_PDIR_SUBFRAME_DATA                          *  pPDIRSubFrameData;
    #endif
    LSA_UINT32                                       const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmDeleteAllB->");

    // Record PDNCDataCheck
    pPDNC                          = &pDDB->PRM.PDNCDataCheck;
    pPDNC->State_B                 = EDDI_PRM_NOT_VALID;

    EDDI_MEMSET(&pPDNC->DropBudget_B, (LSA_UINT8)0x00, (LSA_UINT32)sizeof(EDDI_PRM_CHECK_DROP_BUDGET));
    //clear MaxPortDelay-Field to avoid inconsistencies when UsedPorts<EDDI_MAX_IRTE_PORT_CNT
    EDDI_MEMSET(&pDDB->PRM.PDIRData.pRecordSet_B->pNetWorkPDIRDataRecord->PDIRGlobalDataExt.MaxPortDelay[0], (LSA_UINT8)0, sizeof(EDDI_PRM_RECORD_MAX_PORT_DELAY));

    // Record PDSyncData
    pPDSync                        = &pDDB->PRM.PDSyncData;
    pPDSync->State_B               = EDDI_PRM_NOT_VALID;

    // Record PDSCFData
    pPDSCF                         = &pDDB->PRM.PDSCFDataCheck;
    pPDSCF->State_B                = EDDI_PRM_NOT_VALID;

    // Record PDIRData
    pPDIR                          = &pDDB->PRM.PDIRData;
    pPDIR->State_B                 = EDDI_PRM_NOT_VALID;
    pPDIR->pRecordSet_B->NumberOfFrameDataElements = 0;

    #if defined (EDDI_CFG_DFP_ON)
    pPDIRSubFrameData              = &pDDB->PRM.PDIRSubFrameData;
    pPDIRSubFrameData->State_B     = EDDI_PRM_NOT_VALID;  
	EDDI_DFPPackFrameHandlerCleanUp(pDDB, pPDIRSubFrameData->pRecordSet_B);
    #endif

    // Record PDPortDataAdjust, PDPortMrpDataAdjust
    pPDPortDataAdjust          = &pDDB->PRM.PDPortDataAdjust;
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].MAUTypePresent           = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].PortStatePresent         = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].MulticastBoundaryPresent = LSA_FALSE;
        pPDPortDataAdjust->RecordSet_B[UsrPortIndex].DomainBoundaryPresent    = LSA_FALSE;
        pDDB->PRM.PDPortDataAdjust.RecordState_B[UsrPortIndex]                = EDDI_PRM_NOT_VALID;
        pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex]             = EDDI_PRM_NOT_VALID;
    }

    pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB = LSA_FALSE;

    // Record PDInterfaceMrpDataAdjust
    pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B = EDDI_PRM_NOT_VALID;

    // Record PDSetDefaultPortStates
    pDDB->PRM.PDSetDefaultPortStates.RecordState_B   = EDDI_PRM_NOT_VALID;

    // Record PDirApplicationData
    pDDB->PRM.PDirApplicationData.State_B                                                = EDDI_PRM_NOT_VALID;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->bCalculatedTransferEndValues             = LSA_FALSE;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->TransferEndValues.RxEndNs                = 0;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->TransferEndValues.RxLocalTransferEndNs   = 0;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->TransferEndValues.TxEndNs                = 0;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->TransferEndValues.TxLocalTransferEndNs   = 0;
    pDDB->PRM.PDirApplicationData.pRecordSet_B->TransferEndValues.TxLocalTransferStartNs = 0;

    for (UsrPortIndex = 0; UsrPortIndex < EDD_CFG_MAX_PORT_CNT; UsrPortIndex++)
    {
        pDDB->PRM.PDIRData.pRecordSet_B->BeginEndData[UsrPortIndex].NumberOfPhases = 0;
    }

    // Record PDNRT_LOAD_LIMITATION
    pPDNRTFill = &pDDB->PRM.PDNrtLoadLimitation;
    pPDNRTFill->pRecordSet_B->LoadLimitationActive  = pDDB->NRT.bFeedInLoadLimitationActiveDefault;
    pPDNRTFill->pRecordSet_B->IO_Configuration      = pDDB->NRT.bIO_ConfiguredDefault;
    pPDNRTFill->State_B                             = EDDI_PRM_NOT_VALID;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmDeleteAllB<-");
}
/*---------------------- end [subroutine] ---------------------------------*/

       
/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckConsistency()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckConsistency( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT8                const  Local,
                                                                   LSA_UINT8 const       *  const  IsModularPort,
                                                                   LSA_UINT8                       PortparamsNotApplicable []  )
{
    EDDI_PRM_PDPORT_MRP_DATA_ADJUST_RECORD_PTR_TYPE     pPDPortDataAdjust;
    EDDI_PRM_PDSCF_DATA_CHECK                        *  pPDSCFDataCheck;
    EDDI_PRM_PDSYNC_DATA_PTR_TYPE                       pPDSyncData;
    EDDI_PRM_PDIR_DATA_PTR_TYPE                         pPDIRData;
    EDDI_PRM_PDNRT_LOAD_LIMITATION                   *  pPDNRTFill;
    LSA_UINT32                                          UsrPortIndex, CntPowerOn, CntSpeedMode;
    LSA_UINT32                                          CntMrpPorts;
    LSA_BOOL                                            MrpDisabled, OnlyMRPIFRecordPresent;
    LSA_RESULT                                          Status      = EDD_STS_OK;
    LSA_UINT32                                   const  PortMapCnt  = pDDB->PM.PortMap.PortCnt;
    LSA_BOOL                                            IsLocal;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckConsistency->");
    EDDI_MEMSET(PortparamsNotApplicable, EDD_PRM_PORT_PARAMS_APPLICABLE, EDD_CFG_MAX_PORT_CNT);

    //parameter check
    if (   (EDD_PRM_PARAMS_ASSIGNED_REMOTE != Local)
        && (EDD_PRM_PARAMS_ASSIGNED_LOCALLY != Local))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_Mode);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Local (%d): invalid value", Local);
        return EDD_STS_ERR_PARAM;
    }
    else
    {
        IsLocal = (EDD_PRM_PARAMS_ASSIGNED_LOCALLY == Local) ? LSA_TRUE : LSA_FALSE;
    }

    /* --------------------------------------------------------------------------*/
    /* PDPort Record                                                             */
    /* --------------------------------------------------------------------------*/
    pPDPortDataAdjust = &pDDB->PRM.PDPortDataAdjust;

    CntPowerOn   = 0;
    CntSpeedMode = 0;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32 const HwPortIndex  = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //parameter check
        if (   (EDD_PRM_PORT_IS_MODULAR != IsModularPort[UsrPortIndex])
            && (EDD_PRM_PORT_IS_COMPACT != IsModularPort[UsrPortIndex]) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_Mode);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, IsModularPort (%d): invalid value (UsrPortIndex %d)", IsModularPort[UsrPortIndex], UsrPortIndex);
            return EDD_STS_ERR_PARAM;
        }

        if (EDDI_PRMChangePortState_IsPulled(pDDB, HwPortIndex))
        {
            //Port is pulled
            if (EDD_PRM_PORT_IS_MODULAR == IsModularPort[UsrPortIndex])
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PrmCheckConsistency, PortparamsNotApplicable - PortIndex:%u (IsPulled)", UsrPortIndex);

                PortparamsNotApplicable[UsrPortIndex] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
            }
            else
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Compact Port (UsrPortIndex:%u) is pulled", UsrPortIndex);
                EDDI_Excp("EDDI_PrmCheckConsistency, Compact Port IsPulled", EDDI_FATAL_ERR_EXCP, UsrPortIndex, 0);
                return EDD_STS_ERR_EXCP;
            }
        }
        /* record was written */
        else if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDPortDataAdjust.RecordState_B[UsrPortIndex])
        {
            LSA_UINT8       LinkMode;
            LSA_UINT8       LinkSpeed;
            LSA_UINT8       LinkState       = EDD_LINK_UNKNOWN;
            LSA_RESULT      HWPortStatus    = EDD_STS_OK;
            LSA_RESULT      RetvalPhyParams = EDD_STS_OK;

            /***************************/
            /* Evaluate MAUType        */
            /***************************/
            if (pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUTypePresent)
            {
                RetvalPhyParams = EDDI_LL_CHECK_PHY_PARAMS( pDDB->hSysDev, HwPortIndex, pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised,
                                                            pDDB->Glob.PortParams[HwPortIndex].AutonegMappingCapability,
                                                            pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUType, &LinkSpeed, &LinkMode);

                if (   (EDD_STS_ERR_PARAM != RetvalPhyParams)
                    && (EDD_STS_OK != RetvalPhyParams) )
                {
                    //EDDI_LL_CHECK_PHY_PARAMS detected an error
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmCheckConsistency, EDDI_LL_CHECK_PHY_PARAMS returned 0x%X, UsrPortIndex:%u",
                                        UsrPortIndex, RetvalPhyParams);
                    HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                }
                else if (   (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType)
                            && (EDD_LINK_SPEED_100              != LinkSpeed)
                            && (EDD_LINK_MODE_FULL              != LinkMode ) )
                {
                    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                        "EDDI_PrmCheckConsistency, For EDD_MEDIATYPE_FIBER_OPTIC_CABLE is only EDD_LINK_100MBIT_FULL allowed, PortIndex:%u, LinkSpeed:0x%X, LinkMode:0x%X",
                                        UsrPortIndex, LinkSpeed, LinkMode);
                    HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                }
                else
                {
                    //Copper Port
                    switch (LinkSpeed)
                    {
                        case EDD_LINK_SPEED_10:
                        {
                            if (IsLocal && (EDD_LINK_MODE_HALF == LinkMode))
                            {
                                LinkState = EDD_LINK_10MBIT_HALF;
                            }
                            else if (IsLocal && (EDD_LINK_MODE_FULL == LinkMode))
                            {
                                LinkState = EDD_LINK_10MBIT_FULL;
                            }
                            else
                            {
                                HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                            break;
                        }
                        case EDD_LINK_SPEED_100:
                        {
                            if (EDD_LINK_MODE_HALF == LinkMode)
                            {
                                LinkState = EDD_LINK_100MBIT_HALF;
                            }
                            else if (EDD_LINK_MODE_FULL == LinkMode)
                            {
                                LinkState = EDD_LINK_100MBIT_FULL;
                            }
                            else
                            {
                                HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                            break;
                        }
                        default:
                        {
                            HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                            break;
                        }
                    } //switch
                }

                if (EDD_STS_OK == HWPortStatus)
                {
                    /* check phy speed and mode capability allowed */
                    if (!EDDI_IfLinkSpeedModeInCapability(HwPortIndex, LinkState, pDDB))
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_PrmCheckConsistency, Invalid phy speed and mode capability, PortIndex:%u, LinkState:0x%X", UsrPortIndex, LinkState);
                        HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                    }
                    else
                    {
                        //either all ok or only MAUType not applicable until now
                        if (EDD_STS_OK != RetvalPhyParams) 
                        {
                            //MAUType not applicable
                            if (IsModularPort[UsrPortIndex])
                            {
                                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PrmCheckConsistency, PortparamsNotApplicable - PortIndex:%u", UsrPortIndex);

                                PortparamsNotApplicable[UsrPortIndex] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                            }
                            else
                            {
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_PHY_CAPABILITIES);
                                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, UsrPortIndex + 1,
                                                                                    EDDI_PRM_INDEX_PDPORT_DATA_ADJUST,
                                                                                    EDDI_PRM_RECORD_ADJUST_MAU_TYPE_OFFSET_MAUType,
                                                                                    EDD_PRM_ERR_FAULT_DEFAULT);
                                EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                                    "EDDI_PrmCheckConsistency, Invalid LinkState for NON-modular Port: PortIndex:%u, MAUType:0x%X LinkState:0x%X",
                                                    UsrPortIndex,  pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUType, LinkState);
                                HWPortStatus = EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                        }
                    }
                }

                if (EDD_STS_OK != HWPortStatus)
                {
                    return (HWPortStatus);
                }

                pPDPortDataAdjust->RecordSet_B[UsrPortIndex].LinkStat = LinkState; 
            }                    

            /****************************/
            /* Evaluate AdjustLinkState */
            /****************************/
            /* if PortState is present it is "DOWN"         */
            /* If not we have a MAUType or Default which is */
            /* both UP                                      */
            if (!pPDPortDataAdjust->RecordSet_B[UsrPortIndex].PortStatePresent)
            {
                /* PowerOn if PortState not exist */
                CntPowerOn++;

                if (pPDPortDataAdjust->RecordSet_B[UsrPortIndex].MAUTypePresent)
                {
                    /* Valid only TRUE if 100MBit/1Gbit/10Gbit full */
                    if  (( pPDPortDataAdjust->RecordSet_B[UsrPortIndex].LinkStat == EDD_LINK_100MBIT_FULL ) ||
                            ( pPDPortDataAdjust->RecordSet_B[UsrPortIndex].LinkStat == EDD_LINK_1GBIT_FULL   ) ||
                            ( pPDPortDataAdjust->RecordSet_B[UsrPortIndex].LinkStat == EDD_LINK_10GBIT_FULL  ))
                    {
                        CntSpeedMode++;
                    }
                }
                else
                {
                    /* Autoneg if MAUType not exist */
                    CntSpeedMode++;
                }
            }
        }
        else
        {
            /* use default (Autoneg) */
            /* use default (PowerOn) */
            CntSpeedMode++;
            CntPowerOn++;
        }

    } //end for

    /* at least one port must be poweron */
    if (   (0 == CntPowerOn)
        && !IsLocal )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AllPowerDown_Adjust_eddiPrmCheckConsistency);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                            EDDI_PRM_INDEX_PDPORT_DATA_ADJUST,
                                                            EDD_PRM_ERR_OFFSET_DEFAULT,
                                                            EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, All ports DOWN not allowed. PortCnt:0x%X CntPowerOn:0x%X",
                            PortMapCnt, CntPowerOn);
        return EDD_STS_ERR_PRM_CONSISTENCY;
    }

    /* at least one port must be > 100Mbit*/
    if (   (0 == CntSpeedMode)
        && !IsLocal )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CntSpeedMode_Adjust_eddiPrmCheckConsistency);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                            EDDI_PRM_INDEX_PDPORT_DATA_ADJUST,
                                                            EDD_PRM_ERR_OFFSET_DEFAULT,
                                                            EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, one port must be > 100Mbit. PortCnt:0x%X CntSpeedMode:0x%X",
                            PortMapCnt, CntSpeedMode);
        return EDD_STS_ERR_PRM_CONSISTENCY;
    }

    /* --------------------------------------------------------------------------*/
    /* PDSyncData / PDIRData Record                                              */
    /* --------------------------------------------------------------------------*/

    pPDSyncData      = &pDDB->PRM.PDSyncData;
    pPDIRData        = &pDDB->PRM.PDIRData;
    pPDSCFDataCheck  = &pDDB->PRM.PDSCFDataCheck;


    //If PDIRData was write done, then PDSyncData must be write done, too
    if (EDDI_PRM_WRITE_DONE == pPDIRData->State_B)
    {
        if (EDDI_PRM_WRITE_DONE != pPDSyncData->State_B)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRData_PDSyncData_eddiPrmCheckConsistency);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             LSA_FALSE);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, No pPDSyncData available");
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    /* both records are available */
    if (   (EDDI_PRM_WRITE_DONE == pPDSyncData->State_B) 
        && (EDDI_PRM_WRITE_DONE == pPDIRData->State_B)   )
    {
        Status = EDDI_PrmCheckPDIRPDSyncConsistency(pPDSyncData->pRecordSet_B->pLocalPDSyncDataRecord,
                                                    pPDIRData->pRecordSet_B,
                                                    pDDB);
        if (Status != EDD_STS_OK)
        {
            return Status;
        }
    }

    /* only PDSyncData record is available */
    if (   (EDDI_PRM_WRITE_DONE == pPDSyncData->State_B) 
        && (EDDI_PRM_WRITE_DONE != pPDIRData->State_B)   )
    {
        //Possibly IRTFlex?
        if  /* IRTFlex supported OR default parametrization (neither Flex nor TOP)*/
            (   (pDDB->Glob.bSupportIRTflex)
             || (    (pPDSyncData->pRecordSet_B->pLocalPDSyncDataRecord->ReservedIntervalBegin == 0) 
                  && (pPDSyncData->pRecordSet_B->pLocalPDSyncDataRecord->ReservedIntervalEnd   == 0)  ) )
        {
        }
        else
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRData_PDSyncData_eddiPrmCheckConsistency);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, IRTFlex not supported");
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    pDDB->PRM.bSyncIrDataChanged = EDDI_PrmGetSyncIrDataChange(pDDB, pPDSyncData, pPDIRData);

    /* check Consumer and Provider-Lists if PDIRData is active */
    if (    (EDDI_PRM_VALID == pPDIRData->State_A) 
         && (pDDB->PRM.bSyncIrDataChanged))
    {
        //All providers, consumers and forwarding consumers have to have their FCWs removed here
        //SyncMaster and SyncSlaves must not be Checked here
        if (!EDDI_SyncIrIsUsedByUpperFALSE(pDDB, pPDIRData->pRecordSet_A, LSA_FALSE))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRDataInUse_eddiPrmCheckConsistency);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDIR_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, still used by IRT-Prov/Cons, State_A:0x%X", pPDIRData->State_A);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    #if defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)
    if (   (EDDI_PRM_WRITE_DONE == pPDSyncData->State_B)
        && (8 > pDDB->CRT.CycleBaseFactor) )
    {
        //With a SendClockFactor below 8, the ReductionRatio must be one
        if (!EDDI_SyncIrDataCheckReductionRatio(pDDB, pPDIRData->pRecordSet_B))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRDataInUse_eddiPrmCheckConsistency);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDIR_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency: ERROR -> SCF=0x%X and below 8, but RR is not one.", pDDB->CRT.CycleBaseFactor);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }
    #endif // defined(EDDI_CFG_APPLY_IRTPU_RESTRICTIONS)

    /* nonbinary cyclelength and more than 1 phase (not supportable due to BCW-restrictions) */
    #if !defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
    if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRData.State_B)
    {
        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            if (pDDB->PRM.PDIRData.pRecordSet_B->BeginEndData[UsrPortIndex].NumberOfPhases != 1)
            {
                if (!EDDI_RedIsBinaryValue(pDDB->CycCount.Entity))
                {
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency: Nonbinary cyclelength only with 1 phase allowed. Phases:%d, PhaseCycles:0x%X", pDDB->PRM.PDIRData.pRecordSet_B->BeginEndData[UsrPortIndex].NumberOfPhases, pDDB->CycCount.Entity);
                    
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
        }
    }
    #endif //!defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)

    //check CycleBaseFactor if PDSCFDataCheck is active
    if (pPDSCFDataCheck->State_B == EDDI_PRM_WRITE_DONE)
    {
        if (pDDB->CRT.CycleBaseFactor != EDDI_NTOHS(pPDSCFDataCheck->pB->SendClockFactor))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSCF_DATA_CHECK,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, wrong SendClockFactor in PDSCFDataCheck, CfgPara0x%X PDSCF0x:0x%X",
                              pDDB->CRT.CycleBaseFactor, pPDSCFDataCheck->pB->SendClockFactor);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }

        //Any configuration with an ORANGE period and small send clocks (<8) is not supported
        if (   ((pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd - pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin) > 0)
            && (pDDB->CRT.CycleBaseFactor < 8)
           )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Any configuration with an ORANGE period and small send clocks (<8) is not supported, CycleBaseFactor: 0x%X ReservedInterval:0x%X",
                              pDDB->CRT.CycleBaseFactor, (pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd - pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin));
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }


        // PDIRBeginEndData contains for at least one port an Tx or Rx assignment with 
        // RedOrangePeriodBegin = OrangePeriodBegin < GreenPeriodBegin and SendClock < 250 µs
        if (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRData.State_B)
        {
            EDDI_PRM_RECORD_IRT_PTR_TYPE const pRecordSet = pDDB->PRM.PDIRData.pRecordSet_B;
            LSA_UINT32                   const SCFinNs    = pDDB->CRT.CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY_NS;
            LSA_UINT32                         Index;
            LSA_UINT32                         MinGreenRx = 0;
            LSA_UINT32                         MinGreenTx = 0;

            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                LSA_RESULT  Result;
                LSA_UINT32  ErrOffset;

                if (   (pRecordSet->BeginEndData[UsrPortIndex].bAtLeastOrangePresent)
                    && (pDDB->CRT.CycleBaseFactor < 8)
                   )
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Any configuration with an ORANGE period and small send clocks (<8) is not supported, CycleBaseFactor: 0x%X ReservedInterval:0x%X",
                                      pDDB->CRT.CycleBaseFactor, (pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd - pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin));
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }

                //Check MaxPort delays
                Result = EDDI_PrmCheckPDIRDataMaxPortDelays(pDDB, &pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord->PDIRGlobalDataExt, pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended, UsrPortIndex, &ErrOffset);

                if (   (EDD_STS_ERR_PARAM == Result)
                    && (EDD_PRM_PORT_IS_MODULAR == IsModularPort[UsrPortIndex]) )
                {
                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PrmCheckConsistency, PortparamsNotApplicable - PortIndex:%u", UsrPortIndex);
                    PortparamsNotApplicable[UsrPortIndex] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                }
                else if (EDD_STS_OK != Result)
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MaxPortDelayRx_PDIRData);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, UsrPortIndex + 1,
                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                     ErrOffset,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    //Traceentry is set in EDDI_PrmCheckPDIRDataMaxPortDelays
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }

            // Find minimum Green
            // The minimum configured GREEN period is misconfigured
            // The minimum configured GREEN period is determined by substracting the maximum value of 
            // GreenPeriodBegin in all Tx or Rx assignments from the SendClock.

            for (Index=0; Index < pRecordSet->BeginEndData[0].NumberOfAssignments; Index++)       //UL!!!! - guckst Du - Dacoda lint - warum nur ueber UserPort-1 ???
            {
                if (pRecordSet->BeginEndData[0].Assignment[Index].Tx.GreenPeriodBegin > MinGreenTx )
                {
                    MinGreenTx = pRecordSet->BeginEndData[0].Assignment[Index].Tx.GreenPeriodBegin;
                }

                if (pRecordSet->BeginEndData[0].Assignment[Index].Rx.GreenPeriodBegin > MinGreenRx )
                {
                    MinGreenRx = pRecordSet->BeginEndData[0].Assignment[Index].Rx.GreenPeriodBegin;
                }
            }
               

            if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pRecordSet->pLocalPDIRDataRecord->PDIRFrameData.BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
            { 
                LSA_UINT32  const  FragmentationMode = EDDI_GetBitField32NoSwap(pRecordSet->pLocalPDIRDataRecord->PDIRFrameData.FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FRAGMENTATION_MODE); 
                
                // A) the minimum configured GREEN period is smaller than 125 000 ns and PDIRFrame-Data.FragmentationMode 
                //    is set to inactive (0x00),
                if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF == FragmentationMode)
                {
                    if (  ((SCFinNs - MinGreenTx) < EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF) || ((SCFinNs - MinGreenRx) < EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF) )
                    {
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor);
                        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                         EDDI_PRM_INDEX_PDIR_DATA,
                                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                         EDD_PRM_ERR_FAULT_DEFAULT);
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, the minimum configured GREEN period is smaller than 125 000 ns and PDIRFrame-Data.FragmentationMode is set to inactive (0x00),, CycleBaseFactor: 0x%X ReservedInterval:0x%X",
                                            pDDB->CRT.CycleBaseFactor, (pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd - pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin));
                        return EDD_STS_ERR_PRM_CONSISTENCY;   
                    }
                }
                else if (EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_NO == pDDB->FeatureSupport.FragmentationtypeSupported)
                {
                    //Fragmentation is switched off
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor);
                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, FrameDataProperties.FragmentationMode is 0x%X, but switched off in DPB", FragmentationMode);
                    return EDD_STS_ERR_PRM_CONSISTENCY;   
                }
            
                // B) the minimum configured GREEN period is smaller than DIRGlobalDataExt.YellowTime
                if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12 == pRecordSet->pLocalPDIRDataRecord->PDIRGlobalDataExt.BlockHeader.BlockVersionLow) 
                {
                    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
                    {
                        LSA_UINT32  const  YellowTimeInNs  = pRecordSet->pLocalPDIRDataRecord->PDIRGlobalDataExt.MaxPortDelay[UsrPortIndex].YellowTime;

                        if ((SCFinNs - MinGreenTx) < YellowTimeInNs)
                        {
                            EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                              "EDDI_PrmCheckConsistency, FrameDataProperties: Green Tx (0x%x) is smaller then Yellowtime (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), YellowTimeInNs, UsrPortIndex);
                        }

                        if ((SCFinNs - MinGreenRx) < YellowTimeInNs)
                        {
                            EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                              "EDDI_PrmCheckConsistency, FrameDataProperties: Green Rx (0x%x) is smaller then Yellowtime (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), YellowTimeInNs, UsrPortIndex);
                        }

                        // PDIRGlobalData.YellowTime is smaller than 125 000 ns and PDIRFrameData.Fragmentation-Mode 
                        // is set to inactive (0x00)
                        if (   (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF == FragmentationMode) 
                            && (YellowTimeInNs < EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF))
                        {
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                              "EDDI_PrmCheckConsistency, YellowTime (0x%X) is smaller than 125 000 ns and PDIRFrameData.Fragmentation-Mode is set to inactive (0x00), PortMapCnt->:0x%X", YellowTimeInNs, UsrPortIndex);
                        }
                    }  
                }

                // C) the minimum configured GREEN period is smaller than the bandwidth needed for the maximum size of a 
                //    frame belonging to the protocol class "fragmentable protocols”
                if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_128B == FragmentationMode)
                {
                    if ((SCFinNs - MinGreenTx) < EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_128B)
                    {
                        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_PrmCheckConsistency, FrameDataProperties: Green Tx (0x%x) is smaller then bandwidth needed: (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_128B, UsrPortIndex);
                    }

                    if ((SCFinNs - MinGreenRx) < EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_128B)
                    {
                        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_PrmCheckConsistency, FrameDataProperties: Green Rx (0x%x) is smaller then bandwidth needed (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_128B, UsrPortIndex);
                    }
                }
                else if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_256B == FragmentationMode)
                {
                    if ((SCFinNs - MinGreenTx) < EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_256B)
                    {
                        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_PrmCheckConsistency, FrameDataProperties: Green Tx (0x%x) is smaller then bandwidth needed: (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_256B, UsrPortIndex);
                    }

                    if ((SCFinNs - MinGreenRx) < EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_256B)
                    {
                        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                            "EDDI_PrmCheckConsistency, FrameDataProperties: Green Rx (0x%x) is smaller then bandwidth needed (0x%x), PortMapCnt->:0x%X", (SCFinNs - MinGreenRx), EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_256B, UsrPortIndex);
                    }
                }
            }
        }
    }

    //MRPD Checks and Associate Redundant Frames
    if (   (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRData.State_B)
        && (pDDB->PRM.bSyncIrDataChanged))
    {
        Status = EDDI_AssociateRedundantFrames(pDDB->PRM.PDIRData.pRecordSet_B, pDDB);
                                                                        
        if (Status != EDD_STS_OK)
        {
            return Status;
        }                                                                                   
    }

    #if defined (EDDI_CFG_DFP_ON)
    //DFP Checks and Associate IRTFrameHandler with PackFrame
    if (pDDB->PRM.bSyncIrDataChanged)
    {
        if (   (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRData.State_B)
            && (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRSubFrameData.State_B))
        {
            //Allocate and build all DFP Data
            Status = EDDI_DFPBuildAllPackFrameHandler(pDDB, pDDB->PRM.PDIRSubFrameData.pRecordSet_B); 
            
            if (Status != EDD_STS_OK)
            {
                //Cleanup
                EDDI_DFPPackFrameHandlerCleanUp(pDDB, pDDB->PRM.PDIRSubFrameData.pRecordSet_B);
                
                return Status;
            }
                
            Status = EDDI_DFPCheckPackFrameAgainstIrtFrameHandler(pDDB, pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pFirstPackFrame, (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pFirst);
                                                                            
            if (Status != EDD_STS_OK)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRSubFrameData);
                //   EDDI_PRM_SET_DETAIL_ERR(...)  has already been set in EDDI_DFPCheckPackFrameAgainstIrtFrameHandler.
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, EDDI_DFPCheckPackFrameAgainstIrtFrameHandler failed: 0x%X",
                                  Status);
                return Status;
            }                                                                                   
        }
        else if ((EDDI_PRM_WRITE_DONE == pDDB->PRM.PDIRSubFrameData.State_B))
        {
            //only PDIRSubframeData ==> not allowed
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRSubFrameData);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDIR_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             LSA_FALSE);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDIRSubFrameData without PDIRData");
            return (EDD_STS_ERR_PRM_CONSISTENCY);
        }
    }
    #endif

    /* check CycleBaseFactor if PDSyncData is active */
    if (   (pPDSyncData->State_B == EDDI_PRM_WRITE_DONE)
        && pDDB->PRM.bSyncIrDataChanged)
    {
        EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  pPDSyncDataLocal;
        LSA_UINT32                            MaxIntervalEndNs;

        pPDSyncDataLocal = pPDSyncData->pRecordSet_B->pLocalPDSyncDataRecord;

        if (pPDSyncDataLocal->SendClockFactor != pDDB->CRT.CycleBaseFactor)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSyncData_SendClockFactor);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, wrong SendClockFactor in PDSyncData, SendClockFactor:0x%X CycleBaseFactor:0x%X",
                              pPDSyncDataLocal->SendClockFactor, pDDB->CRT.CycleBaseFactor);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }

        MaxIntervalEndNs = EDDI_GenGetMaxReservedIntEnd(pDDB);

        if (pPDSyncDataLocal->ReservedIntervalEnd > MaxIntervalEndNs)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSyncData_ReservedIntervalEnd);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_ReservedIntervalEnd_OFFSET,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, ReservedIntervalEnd:0x%X MaxIntervalEndNs:0x%X",
                              pPDSyncDataLocal->ReservedIntervalEnd, MaxIntervalEndNs);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* PDPortMrpDataAjdust/PDInterfaceMrpDataAdjust/PDSetDefaultPortStates Record*/
    /* --------------------------------------------------------------------------*/

    CntMrpPorts = 0;
    MrpDisabled = LSA_FALSE;
    OnlyMRPIFRecordPresent = LSA_FALSE;

    /* only 0 or two PDPortMrpDataAjdust are allowed */
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        /* we only need the info the a write was done! */
        if (pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex] == EDDI_PRM_WRITE_DONE)
        {
            CntMrpPorts++;
        }
    }

    /* If no MRP was configured on EDD startup (DSB) we dont allow  */
    /* MRP-Records to be written!                                   */

    if (!pDDB->SWITCH.bMRPRedundantPortsUsed)
    {
        if (   (CntMrpPorts)
            || (pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B == EDDI_PRM_WRITE_DONE))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSyncData_ReservedIntervalEnd);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDD_PRM_ERR_INDEX_DEFAULT,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDPortMrpDataAjdust. MRP records present, but MRP not configured within EDDI.");
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
        MrpDisabled = LSA_TRUE;
    }

    /* three cases (we only check for record presence!):            */
    /* no PDPortMrpDataAdjust record  + PDInterfaceMrpDataAdjust    */
    /* 2  PDPortMrpDataAdjust records + PDInterfaceMrpDataAdjust    */
    /* no records at all.                                           */

    /* Note that MRP records are only allowed if MRP is configured  */
    /* within EDD!                                                  */

    if (0 == CntMrpPorts)
    {
        if (pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B == EDDI_PRM_WRITE_DONE)
        {
            OnlyMRPIFRecordPresent = LSA_TRUE;
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckConsistency, MRP: OK. Only Interface MRP-Record present.");
        }
        else
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckConsistency, MRP: OK. No MRP-Records present.");
        }
        MrpDisabled = LSA_TRUE;
        pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB = LSA_FALSE;
    }
    else if (   (CntMrpPorts >= 2)  // MRP is ON
             && (CntMrpPorts <= PortMapCnt))  
    {        
        if (pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B != EDDI_PRM_WRITE_DONE)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDPortMrpDataAdjust);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDINTERFACE_MRP_DATA_ADJUST,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             LSA_FALSE);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDPortMrpDataAjdust. InterfaceRecord missing.");
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }

        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckConsistency, MRP: OK. 0x%X MRP-Records present.", CntMrpPorts);

        pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB = LSA_TRUE;
    }
    else
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDPortMrpDataAdjust);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDDI_PRM_INDEX_PDPORT_MRP_DATA_ADJUST,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         LSA_FALSE);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDPortMrpDataAjdust, invalid PortCnt:0x%X", CntMrpPorts);
        return EDD_STS_ERR_PRM_CONSISTENCY;
    }


    /* ----------------------------------------------------------------------------------------------*/
    /* HSync: with HSyncRole 'APPL_Support' the following rules apply:                               */
    /* 1) If MRP port + if records present: Exactly 2 PDPortMrpDataAdjust records have to exist      */
    /* 2) No MRP records at all are present                                                          */
    /* 3) MRP IF record only (=MRP OFF by engineering) is not allowed!                               */
    /* ----------------------------------------------------------------------------------------------*/
    if (EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole)
    {
        if (   ((0 != CntMrpPorts)  &&  (2 != CntMrpPorts))
            || OnlyMRPIFRecordPresent)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDPortMrpDataAdjust);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDPORT_MRP_DATA_ADJUST,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             LSA_FALSE);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDPortMrpDataAjdust, HSyncApplSupport requires 2 MRP ports, or no MRP records at all. Ports:%d, IFRecord present:0x%X", 
                CntMrpPorts, OnlyMRPIFRecordPresent);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }


    /* --------------------------------------------------------------------------*/
    /* SyncInRed is not allowed together with MRP                                */
    /* --------------------------------------------------------------------------*/
    if (   !MrpDisabled
        && (EDDI_PRM_WRITE_DONE == pPDIRData->State_B) )
    {
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
          
            if (   (0 != pDDB->PRM.PDIRData.pRecordSet_B->MetaData.IrtPortSyncRxCnt[HwPortIndex])
                || (0 != pDDB->PRM.PDIRData.pRecordSet_B->MetaData.IrtPortSyncTxCnt[HwPortIndex]))
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRData_PDSyncData_eddiPrmCheckConsistency);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDD_PRM_ERR_INDEX_DEFAULT,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, SyncInRed is not allowed together with MRP.");
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
    }        

    /* --------------------------------------------------------------------------*/
    /* A PDSetDefaultPortState record is only allowed with Ports not used by MRP */
    /* This is if MRP is disabled or the ports are not R-Ports                   */
    /* --------------------------------------------------------------------------*/

    if ((pDDB->PRM.PDSetDefaultPortStates.RecordState_B == EDDI_PRM_WRITE_DONE) &&
        (!MrpDisabled))
    {
        /* check for overlapping R-ports if MRP is enabled */
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_BOOL  RPort = LSA_FALSE;

            if (pDDB->PRM.PDPortMrpDataAdjust.AtLeastTwoRecordsPresentB) /* R-Ports from PRM */
            {
                if (pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[UsrPortIndex] == EDDI_PRM_WRITE_DONE)
                {
                    RPort = LSA_TRUE;
                }
            }
            else /* R-Ports from Setup */
            {
                LSA_UINT32 HwPortIndex;
                HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                if (pDDB->SWITCH.LinkPx[HwPortIndex].bMRPRedundantPort)
                {
                    RPort = LSA_TRUE;
                }
            }

            /* check if this port is set with PDSetDefaultPortStates and is a Rport -> not allowed */
            if (pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[UsrPortIndex].PortStatePresent && RPort)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSetDefaultPortStates);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, UsrPortIndex,
                                                                 EDDI_PRM_INDEX_PDSET_DEFAULT_PORT_STATES,
                                                                 pDDB->PRM.PrmDetailErr.EDDI_PRM_ADJUST_PORT_STATE_Offset[UsrPortIndex],
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, PDSetDefaultStates overlapping with MRP-R-Port. UsrPortIdx:%d", UsrPortIndex);
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
    }

    /* --------------------------------------------------------------------------*/
    /* Check FragmentationMode (pPDIRData must be written)                       */
    /* --------------------------------------------------------------------------*/
    if (pPDIRData->State_B == EDDI_PRM_WRITE_DONE)
    {
        if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pPDIRData->pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData.BlockHeader.BlockVersionLow)
        {
            EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE          *  const pFrameData             = &pPDIRData->pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData;
            EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE     *  const pPDIRGlobalDataExt     = &pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord->PDIRGlobalDataExt;
            LSA_UINT32                                             NewYellowTimeInNs;               
            LSA_UINT32                                             NewActivePortCnt       = 0;
            LSA_UINT32                                             NewActiveUsrPortIndex  = 0;
            LSA_UINT32                                             FragmentationMode;
            
            FragmentationMode = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FRAGMENTATION_MODE);

            pPDPortDataAdjust = &pDDB->PRM.PDPortDataAdjust; 
                       
            //check number of active ports
            for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
            {
                if (!pPDPortDataAdjust->RecordSet_B[UsrPortIndex].PortStatePresent)
                {
                    NewActivePortCnt++;
                    NewActiveUsrPortIndex = UsrPortIndex;
                }
            }
            
            if (!NewActivePortCnt) 
            {
                NewActivePortCnt = PortMapCnt;
            }

            pPDIRData->pRecordSet_B->NrtFrag.ActivePortCnt = NewActivePortCnt;
            NewYellowTimeInNs  = pPDIRGlobalDataExt->MaxPortDelay[NewActiveUsrPortIndex].YellowTime;

            //Check Fragmentation
            if (   (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_128B == FragmentationMode) 
                || (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_256B == FragmentationMode))        
            {
                //Fragmentation ON
                if (NewActivePortCnt != 1UL) 
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameProperties_Fragmentation);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_PrmCheckConsistency, Error: FrameDataProperties: Fragmention is only allowed with 1 active Port, PortMapCnt->:0x%X", NewActivePortCnt);
                    return EDD_STS_ERR_PRM_CONSISTENCY;  
                } 

                if (NewYellowTimeInNs >= EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF)
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Warning: Fragmentation is ON, FrameDataProperties: Wrong YellowTime, Given: YellowTime->:0x%X Ns, Max YellowtTime is:0x%X", NewYellowTimeInNs, EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF);  
                    return EDD_STS_ERR_PRM_BLOCK;  
                }

                //Set Fragmentation
                if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_128B == FragmentationMode) 
                {
                    pPDIRData->pRecordSet_B->NrtFrag.FragSize = EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_SHORT;
                }
                else if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_256B == FragmentationMode)
                {
                    pPDIRData->pRecordSet_B->NrtFrag.FragSize = EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_LONG;
                }

                pPDIRData->pRecordSet_B->NrtFrag.FragUsrPortIndex = NewActiveUsrPortIndex;
                pPDIRData->pRecordSet_B->NrtFrag.FragHwPortIndex  = pDDB->PM.PortMap.UsrPortID_x_to_HWPort_y[NewActiveUsrPortIndex] - 1;
            }
            else if (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF == FragmentationMode)
            {
                for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
                {
                    NewYellowTimeInNs  = pPDIRGlobalDataExt->MaxPortDelay[UsrPortIndex].YellowTime;

                    if (NewYellowTimeInNs < EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF)
                    {
                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency, Warning: Fragmentation is OFF, FrameDataProperties: Wrong YellowTime, Given: YellowTime->:0x%X Ns, must be:", NewYellowTimeInNs);                   
                        return EDD_STS_ERR_PRM_BLOCK;  
                    }
                }            

                //Fragmentation OFF
                pPDIRData->pRecordSet_B->NrtFrag.FragSize = EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF;
            }
            else
            {
                //nothing to do
            }
        }
    }
    
    /* --------------------------------------------------------------------------*/
    /* PDNRT_LOAD_LIMITATION Record                                              */
    /* --------------------------------------------------------------------------*/
    pPDNRTFill = &pDDB->PRM.PDNrtLoadLimitation;
    
    if (pPDNRTFill->State_B == EDDI_PRM_WRITE_DONE)
    {
        if (!pPDNRTFill->pRecordSet_B->IO_Configuration)
        {
            if ((pDDB->CRT.ProviderList.ActiveProvRTC12 > 0)  ||          // active RTC12 Provider exists
                (pDDB->CRT.ProviderList.ActiveProvRTC3  > 0)  ||          // active RTC3  Provider exists
                (pDDB->CRT.ConsumerList.ActiveConsRTC12 > 0)  ||          // active RTC12 Consumer exists
                (pDDB->CRT.ConsumerList.ActiveConsRTC3  > 0)   )          // active RTC3  Consumer exists
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IO_Configured_PDNRTFeedInLoadLimitation);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                    EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION,
                                                                    EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                    EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_PrmCheckConsistency, Error: PDNRT_LOAD_LIMITATION: IO is configured to NO, but %u/%u consumers or %u/%u providers are activated",
                                  pDDB->CRT.ConsumerList.ActiveConsRTC12, pDDB->CRT.ConsumerList.ActiveConsRTC3,
                                  pDDB->CRT.ProviderList.ActiveProvRTC12, pDDB->CRT.ProviderList.ActiveProvRTC3);
                return EDD_STS_ERR_PRM_CONSISTENCY;  
            }
        }
        else
        {
            ; // all possible errors were tested during PRM_Write already
        }
    }

    if ( (pPDNRTFill->pRecordSet_B->LoadLimitationActive == 0)  &&
        ((pDDB->CRT.ProviderList.ActiveProvRTC12 > 0)  ||            // active RTC12 Provider exists
         (pDDB->CRT.ProviderList.ActiveProvRTC3  > 0)  ||            // active RTC3  Provider exists
         (pDDB->CRT.ConsumerList.ActiveConsRTC12 > 0)  ||            // active RTC12 Consumer exists
         (pDDB->CRT.ConsumerList.ActiveConsRTC3  > 0)   )         )  // active RTC3  Consumer exists
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_LoadLimitationActive_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                            EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION,
                                                            EDD_PRM_ERR_OFFSET_DEFAULT,
                                                            EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                         "EDDI_PrmCheckConsistency, Error: PDNRT_LOAD_LIMITATION: LoadLimitationActive is inactive, but %u/%u consumers or %u/%u providers are activated",
                          pDDB->CRT.ConsumerList.ActiveConsRTC12, pDDB->CRT.ConsumerList.ActiveConsRTC3,
                          pDDB->CRT.ProviderList.ActiveProvRTC12, pDDB->CRT.ProviderList.ActiveProvRTC3);
        return EDD_STS_ERR_PRM_CONSISTENCY;  
    }

    /* --------------------------------------------------------------------------*/
    /* Setup IRTTrace                                                            */
    /* --------------------------------------------------------------------------*/
    //If a port has PDIRData and no IRTTRace parametrization (Record 0xB060),
    //then this parametrization is done automatically here.

    if (   (pPDIRData->State_B == EDDI_PRM_WRITE_DONE)
        && (pDDB->PRM.bSyncIrDataChanged))
    {
        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            EDD_RQB_PRM_WRITE_TYPE                     PrmWriteLocal;
            EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE  PDTraceUnitControlRecLocal;
            LSA_RESULT                                 Result;

            //check if for this port the IRTTrace has been configured already
            if (pDDB->PRM.PDTraceUnitControl.State_A[UsrPortIndex] != EDDI_PRM_WRITE_DONE)
            {
                //Fill record-request
                PDTraceUnitControlRecLocal.BlockHeader.BlockLength      = EDDI_PRM_H_BL_PD_TRACE_UNIT_CONTROL;
                PDTraceUnitControlRecLocal.BlockHeader.BlockType        = EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_CONTROL;
                PDTraceUnitControlRecLocal.BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
                PDTraceUnitControlRecLocal.BlockHeader.BlockVersionLow  = EDDI_PRM_BLOCK_VERSION_LOW;
                PDTraceUnitControlRecLocal.BlockHeader.Padding1_1       = 0;
                PDTraceUnitControlRecLocal.BlockHeader.Padding1_2       = 0;
                PDTraceUnitControlRecLocal.DiagnosticEnable             = EDDI_NTOHL(EDDI_PRM_PDTRACE_UNIT_DEFAULT_SETUP);
                PDTraceUnitControlRecLocal.Reserved                     = 0;

                PrmWriteLocal.edd_port_id           = (LSA_UINT16)(UsrPortIndex + 1);
                PrmWriteLocal.record_index          = EDDI_PRM_INDEX_PDTRACEUNIT_CONTROL;
                PrmWriteLocal.record_data_length    = EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL;
                PrmWriteLocal.slot_number           = 0;
                PrmWriteLocal.subslot_number        = 0;
                PrmWriteLocal.record_data           = (EDD_UPPER_MEM_U8_PTR_TYPE)(LSA_VOID *)&PDTraceUnitControlRecLocal;

                Result = EDDI_PrmCheckPDTraceUnitControl(&PrmWriteLocal, pDDB);
                if (EDD_STS_OK != Result)
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckConsistency calling EDDI_PrmCheckPDTraceUnitControl, Result > ->:0x%X :0x%X", Result, 0);
                    return Result;
                }
            }
        }
    }
    
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckConsistency<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_AssociateRedundantFrames()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_AssociateRedundantFrames( EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet,
                                                                        EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                   pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pRecordSet->FrameHandlerQueue.pFirst;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                   pFindFrmHandler = EDDI_NULL_PTR;
    LSA_UINT8                                       i, j;
    LSA_BOOL                                        bRedundantFrameID = LSA_FALSE;
    LSA_UINT32                               const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                                      iElem;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_AssociateRedundantFrames->");
   
    while (EDDI_NULL_PTR != pFrmHandler)
    {               
        if (EDDI_SYNC_IRT_FORWARDER != pFrmHandler->HandlerType)
        {
            for (iElem = 0; iElem < pDDB->PM.PortMap.PortCnt; iElem++)
            {
                if (EDDI_NULL_PTR != pFrmHandler->pIrFrameDataElem[iElem])
                {
                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pFrmHandler->pIrFrameDataElem[iElem];
                 
                    bRedundantFrameID = LSA_FALSE;
     
                    if (   (pFrmData->FrameID >= EDD_SRT_FRAMEID_IRT_REDUNDANT_START) 
                        && (pFrmData->FrameID <= EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP))
                    {
                        /* ------------------------------------------------ */     
                        /* 1st. Set pFrmHandlerRedundant to Null-Ptr        */
                        /* ------------------------------------------------ */
                        pFrmHandler->pFrmHandlerRedundant = EDDI_NULL_PTR;
                
                        /* ------------------------------------------------ */     
                        /* 2nd. Check if the FrameID is in Redunand-Range   */
                        /* ------------------------------------------------ */ 
                  
                        //Check if Frame ID is odd (x+1)        
                        if (0 != (pFrmData->FrameID & 1))
                        {               
                            LSA_UINT32  TxPortCount;

                            //Check if MRPD is activated
                            if (EDD_FEATURE_DISABLE == pDDB->FeatureSupport.bMRPDSupported)
                            {
                                //Feature MRPD is switched off. Only single, even FrameID allowed or IRT forwarder
                                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_AssociateRedundantFrames, support for MRPD is switched of in DPB!");
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID);
                                return EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                        
                            // Get the FrameID x (even)
                            pFindFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pFrmData->FrameID-1, pDDB->PRM.PDIRData.pRecordSet_B);
                    
                            if (pFindFrmHandler == EDDI_NULL_PTR)
                            {
                                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_AssociateRedundantFrames, No FrameID for a MRPD Consumer was found. expected ConsumerFrameID->:0x%X", pFrmData->FrameID + 1);
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerMRPD_NoFrameID);
                                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                                return EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                    
                            //Check if the port's have the same phase (search in x and x+1)
                            //On all Ports must be the same Reduction (search in x and x+1)
                            if (EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType)
                            {   
                                //Check if the port's have the same phase (search in x and x+1)
                                for (i=0; i<PortMapCnt; i++)
                                {
                                    if (pFrmHandler->pIrFrameDataElem[i])
                                    {
                                        for (j=0; j<PortMapCnt; j++)
                                        {
                                            if (pFindFrmHandler->pIrFrameDataElem[j])
                                            {
                                                if (pFrmHandler->pIrFrameDataElem[i]->Phase != pFindFrmHandler->pIrFrameDataElem[j]->Phase)
                                                {
                                                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_AssociateRedundantFrames, ERROR: No same Phase was found (x and x+1 FrameID), pFrmData->FrameID->:0x%X", pFrmData->FrameID);
                                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                                    return EDD_STS_ERR_PRM_CONSISTENCY;  
                                                } 
                                            }   
                                        }
                                    }
                                }

                                //On all Ports must be the same Reduction (search in x and x+1)
                                for (i=0; i<PortMapCnt; i++)
                                {
                                    if (pFrmHandler->pIrFrameDataElem[i])
                                    {
                                        for (j=0; j<PortMapCnt; j++)
                                        {
                                            if (pFindFrmHandler->pIrFrameDataElem[j])
                                            {
                                                if (pFrmHandler->pIrFrameDataElem[i]->ReductionRatio != pFindFrmHandler->pIrFrameDataElem[j]->ReductionRatio)
                                                {
                                                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_AssociateRedundantFrames, ERROR: On all Ports must be the same Reduction (search in x and x+1), pFrmData->FrameID->:0x%X", pFrmData->FrameID);
                                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                                    return EDD_STS_ERR_PRM_CONSISTENCY;   
                                                } 
                                            }   
                                        }
                                    }
                                }
                            }

                            TxPortCount = EDDI_PRMGetPortCountFromTxPortGroupArray(pFrmData->UsrTxPortGroupArray[0]);
                    
                            if ((EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType) && (TxPortCount != 1))
                            {
                                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, 
                                                  "EDDI_AssociateRedundantFrames, ERROR: -> The redundant provider FrameID(0x%X) has TxPortCount(%u) in PDIRData. UsrTxPortGroupArray=0x%X", 
                                                  pFrmData->FrameID, 
                                                  TxPortCount,
                                                  pFrmData->UsrTxPortGroupArray[0]);
                                                  
                                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                                return EDD_STS_ERR_PRM_CONSISTENCY; 
                            }
                    
                            //Forwarding Consumer (x+1) allowed max. 1 Forwarding Port
                            if (((EDDI_SYNC_IRT_FWD_CONSUMER == pFrmHandler->HandlerType) || (EDDI_SYNC_IRT_CONSUMER == pFrmHandler->HandlerType)) && TxPortCount > 1)
                            {
                                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, 
                                                  "EDDI_AssociateRedundantFrames, ERROR -> The redundant consumer/forwarder FrameID(0x%X) has TxPortCount(%u) in PDIRData. TxPortGroupArray=0x%X", 
                                                  pFrmData->FrameID, 
                                                  TxPortCount,
                                                  pFrmData->UsrTxPortGroupArray[0]);
                                                  
                                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                                return EDD_STS_ERR_PRM_CONSISTENCY;        
                            }                   
                        }
                        else //Check if Frame ID is even (x)  
                        {
                            //Get the FrameID x+1 (odd)
                            pFindFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pFrmData->FrameID+1, pDDB->PRM.PDIRData.pRecordSet_B);
                    
                            if (pFindFrmHandler == EDDI_NULL_PTR)
                            {
                                if (EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType)
                                {
                                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerControlEvent, No FrameID for a MRPD Provider was found. expected ConsumerFrameID->:0x%X", pFrmHandler->FrameId + 1);
                                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderMRPD_NoFrameID);
                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                     EDDI_PRM_INDEX_PDIR_DATA,
                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                    return EDD_STS_ERR_PRM_CONSISTENCY;
                                }
               
                                //FrameID x can be without a x+1 FrameID
                                break;
                            }
                        }     

                        bRedundantFrameID = LSA_TRUE;     
                    }

                    //IRTFlex and MRPD is not allowed!
                    if (   (bRedundantFrameID) 
                        && (EDDI_PrmCheckIsIRTFlex(pDDB)))
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_AssociateRedundantFrames, ERROR: IRTFlex and MRPD is not allowed, pFrmData->FrameID->:0x%X", pFrmData->FrameID);
                        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                         EDDI_PRM_INDEX_PDIR_DATA,
                                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                         EDD_PRM_ERR_FAULT_DEFAULT);
                        return EDD_STS_ERR_PRM_CONSISTENCY;        
                    }   

                    /* -------------------------------------------------------------------------- */     
                    /* 3rd. Associate vice versa main pFrmHandler with redundant pFindFrmHandler  */
                    /* -------------------------------------------------------------------------- */ 
                    if (bRedundantFrameID)
                    {
                        if (pFindFrmHandler)
                        {
                            pFrmHandler->pFrmHandlerRedundant     = pFindFrmHandler;
                            pFindFrmHandler->pFrmHandlerRedundant = pFrmHandler; 
                        }
                    }
                }
            }
        }
        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_AssociateRedundantFrames<-");
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/* Description: This function counts the number of TxPorts coded in        */
/*              TxPortGroupArray of PRM-Record.                            */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:  LSA_UINT32                                               */
/*                                                                         */
/***************************************************************************/
LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_PRMGetPortCountFromTxPortGroupArray( LSA_UINT8  const  TxPortGroupArray )
{
    LSA_UINT32  TxPortCount;
    LSA_UINT8   TxPorts;

    TxPortCount = 0;
    TxPorts     = TxPortGroupArray >> 1;    // skip Bit(0) API

    while (TxPorts != 0)
    {
        if (TxPorts & 0x1)
        {
            TxPortCount++;          // count TxPorts
        }
        TxPorts = TxPorts >> 1;     // next port of this TxPortGroupArray
    }

    return (TxPortCount);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetMauType()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetMauType( LSA_UINT32               const  UsrPortIndexMauType,
                                                        LSA_UINT32               const  HwPortIndexMauType,
                                                        LSA_UINT8                const  PHYPower,
                                                        LSA_UINT8                const  LinkState,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                            Result;
    LSA_UINT32                            UsrPortIndex;
    EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE  SpeedMode;
    LSA_UINT8                             LinkSpeedMode;
    LSA_UINT8                             TempLinkStatus;
    LSA_UINT8                             TempLinkSpeed;
    LSA_UINT8                             TempLinkMode;

    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SetMauType->HwPortIndexMauType:0x%X PHYPower:0x%X LinkState:0x%X", 
                      HwPortIndexMauType, PHYPower, LinkState);

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        SpeedMode.PortIDLinkSpeedMode[UsrPortIndex] = EDD_LINK_UNCHANGED;
    }

    // Patch only for fiber transceiver, change Autoneg to 100MBit/Full
    // Attention: any port can be changed from AUTONEG to fixed settings by this way, but normally only for optical 
    // ports the user sets AutonegMappingCapability to EDD_LINK_100MBIT_FULL instead of EDD_LINK_AUTONEG
    if (EDD_LINK_AUTONEG == LinkState)
    {
        EDDI_AutoNegCapToLinkSpeedMode(HwPortIndexMauType, &LinkSpeedMode, pDDB);

        SpeedMode.PortIDLinkSpeedMode[UsrPortIndexMauType] = LinkSpeedMode;
    }
    else
    {
        SpeedMode.PortIDLinkSpeedMode[UsrPortIndexMauType] = LinkState;
    }

    // in case of a formerly disabled optical port behind a Broadcom-PHY, the above changed values for linkspeed/linkmode are 
    // transferred to pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus/LinkSpeed only. When a simulated linkup is done 
    // (TRA_SetPowerDown_Broadcom), these values are then transferred to pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus/LinkSpeed,
    // hence another call of SwiPortSetSpMoConfig(...) may be necessary later to trigger the Class2/Class3-SMs
    Result = EDDI_SwiPortSetSpMoConfig(&SpeedMode, pDDB);
    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMauType, EDDI_SwiPortSetSpMoConfig, Result:0x%X", Result);
        EDDI_Excp("EDDI_SetMauType, EDDI_SwiPortSetSpMoConfig", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }
    TempLinkStatus = pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkStatus;
    TempLinkSpeed  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkSpeed;
    TempLinkMode   = pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkMode;

    if (EDDI_PHY_POWER_OFF == PHYPower)
    {
        //only record change, do not generate indication
        EDDI_SwiPortSetPhyStatus(HwPortIndexMauType, PHYPower, LSA_FALSE, pDDB);
    }

    if (!EDDI_PRMChangePortState_IsPulled(pDDB, HwPortIndexMauType))
    {
        Result = EDDI_TRASetPowerDown(pDDB, HwPortIndexMauType, PHYPower, LSA_FALSE /*bRaw*/);
        if (EDD_STS_OK != Result)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMauType, EDDI_SetMauType, Result:0x%X", Result);
            EDDI_Excp("EDDI_SetMauType, EDDI_SetMauType", EDDI_FATAL_ERR_EXCP, Result, 0);
            return;
        }
    }
    
    EDDI_SwiPortSetPhyStatus(HwPortIndexMauType, PHYPower, LSA_TRUE, pDDB);

    //now check if during TRA_SetPowerDown the link params have changed!
    if (   (TempLinkStatus != pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkStatus)
        || (TempLinkSpeed  != pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkSpeed)
        || (TempLinkMode   != pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkMode))
    {
        if (   (!pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI)
            && (SpeedMode.PortIDLinkSpeedMode[UsrPortIndexMauType]))
        {    
            EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SetMauType, HwPortIndexMauType:0x%X NewLinkStatus:0x%X NewLinkSpeed:0x%X NewLinkMode:0x%X", 
                              HwPortIndexMauType, pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkStatus, pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkSpeed, pDDB->pLocal_SWITCH->LinkPx[HwPortIndexMauType].LinkMode);
            Result = EDDI_SwiPortSetSpMoConfig(&SpeedMode, pDDB);
            if (EDD_STS_OK != Result)
            {
              EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMauType, SwiPortSetSpMoConfig, Result:0x%X", Result);
              EDDI_Excp("EDDI_SetMauType, SwiPortSetSpMoConfig", EDDI_FATAL_ERR_EXCP, Result, 0);
              return;
            }
        }
        else
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SetMauType, DisableLinkChangePHYSMI:0x%X PortIDLinkSpeedMode:0x%X", 
                              pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI, SpeedMode.PortIDLinkSpeedMode[UsrPortIndexMauType]);
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMauType<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetMulticast( LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT32               const  MACAddrLow,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                       Result;
    EDD_RQB_PORTID_MODE_TYPE         PortIDMode;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE  MCFWDCtrlParam;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMulticast->");

    MCFWDCtrlParam.pPortIDModeArray = &PortIDMode;
    #if defined (EDDI_CFG_REV7)
    PortIDMode.Mode                 = EDD_MULTICAST_FWD_BITMASK_PLUS_BL;
    #else
    PortIDMode.Mode                 = EDD_MULTICAST_FWD_BITMASK;
    #endif
    PortIDMode.DstPortID            = (LSA_UINT16)pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex];
    PortIDMode.SrcPortID            = (LSA_UINT16) EDD_PORT_ID_AUTO;
    MCFWDCtrlParam.PortIDModeCnt    = 1;                                        
    MCFWDCtrlParam.MACAddrGroup     = EDD_MAC_ADDR_GROUP_RT_2_QUER;
    MCFWDCtrlParam.MACAddrLow       = MACAddrLow;
    MCFWDCtrlParam.MACAddrPrio      = EDD_MULTICAST_FWD_PRIO_UNCHANGED;

    EDDI_ENTER_SYNC_S();
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,
                                 pDDB,
                                 EDDI_BL_USR_NONE);
    EDDI_EXIT_SYNC_S();
    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMulticast, EDDI_SwiPNMCFwdCtrl, Result:0x%X", Result);
        EDDI_Excp("EDDI_SetMulticast, EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMulticast<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetDCPMulticast()                           */
/*                                                                         */
/* D e s c r i p t i o n: Set engress filter for DCP-Multicast addresses   */
/*                        01-0E-CF-00-00-00 (DCP-Identity)                 */
/*                        01-0E-CF-00-00-01 (DCP-Hello)                    */
/*                                                                         */
/* A r g u m e n t s:     MACAddrLow                                       */
/*                                                                         */
/*                        Bit0: 01-0E-CF-00-00-00                          */
/*                        Bit1: 01-0E-CF-00-00-01                          */
/*                                                                         */
/*                        Bit = 0: dont forward                            */
/*                        Bit = 1: forward                                 */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetDCPMulticast( LSA_UINT32               const  HwPortIndex,
                                                             LSA_UINT32               const  MACAddrLow,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                       Result;
    EDD_RQB_PORTID_MODE_TYPE         PortIDMode;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE  MCFWDCtrlParam;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetDCPMulticast->");

    MCFWDCtrlParam.pPortIDModeArray = &PortIDMode;
    PortIDMode.Mode                 = EDD_MULTICAST_FWD_BITMASK;
    PortIDMode.DstPortID            = (LSA_UINT16)pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex];
    PortIDMode.SrcPortID            = (LSA_UINT16) EDD_PORT_ID_AUTO;
    MCFWDCtrlParam.PortIDModeCnt    = 1;
    MCFWDCtrlParam.MACAddrGroup     = EDD_MAC_ADDR_GROUP_DCP;
    MCFWDCtrlParam.MACAddrLow       = MACAddrLow;
    MCFWDCtrlParam.MACAddrPrio      = EDD_MULTICAST_FWD_PRIO_UNCHANGED;

    EDDI_ENTER_SYNC_S();
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,
                                 pDDB,
                                 EDDI_BL_USR_NONE);
    EDDI_EXIT_SYNC_S();

    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetDCPMulticast, EDDI_SwiPNMCFwdCtrl, Result:0x%X", Result);
        EDDI_Excp("EDDI_SetDCPMulticast, EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetDCPMulticast<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetMRPMulticast()                           */
/*                                                                         */
/* D e s c r i p t i o n: Set MRP Multicastaddresses FWD to ENABLE or      */
/*                        DISABLE                                          */
/*                                                                         */
/* A r g u m e n t s:     Mode: EDD_MULTICAST_FWD_ENABLE                   */
/*                              EDD_MULTICAST_FWD_DISABLE                  */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetMRPMulticast( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT16               const  Mode )
{
    LSA_RESULT                          Result;
    EDD_RQB_PORTID_MODE_TYPE            PortIDMode[EDDI_MAX_IRTE_PORT_CNT];
    LSA_UINT16                          UsrPortIndex;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE     MCFWDCtrlParam;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMRPMulticast->");

    /* we set all Ports */
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        PortIDMode[UsrPortIndex].Mode       = Mode;
        PortIDMode[UsrPortIndex].DstPortID  = (LSA_UINT16)(UsrPortIndex+1);
        PortIDMode[UsrPortIndex].SrcPortID  = (LSA_UINT16) EDD_PORT_ID_AUTO;
    }

    MCFWDCtrlParam.pPortIDModeArray = PortIDMode;
    MCFWDCtrlParam.PortIDModeCnt    = (LSA_UINT16)pDDB->PM.PortMap.PortCnt;
    MCFWDCtrlParam.MACAddrLow       = 0 /* dont care */;
    MCFWDCtrlParam.MACAddrPrio      = EDD_MULTICAST_FWD_PRIO_UNCHANGED;

    /* MRP_1 */
    MCFWDCtrlParam.MACAddrGroup     = EDD_MAC_ADDR_GROUP_MRP_1;
    EDDI_ENTER_SYNC_S();
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,
                                 pDDB,
                                 EDDI_BL_USR_NONE);
    EDDI_EXIT_SYNC_S();

    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMRPMulticast, EDDI_SwiPNMCFwdCtrl, Result:0x%X", Result);
        EDDI_Excp("EDDI_SetMRPMulticast, EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    /* MRP_2 */
    MCFWDCtrlParam.MACAddrGroup = EDD_MAC_ADDR_GROUP_MRP_2;
    EDDI_ENTER_SYNC_S();
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,
                                 pDDB,
                                 EDDI_BL_USR_NONE);
    EDDI_EXIT_SYNC_S();

    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetMRPMulticast, EDDI_SwiPNMCFwdCtrl, Result:0x%X", Result);
        EDDI_Excp("EDDI_SetMRPMulticast, EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMRPMulticast<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCompare()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCompare( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                        EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pRecordA,
                                                        EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pRecordB,
                                                        LSA_UINT32                  const  RecordLen )
{
    LSA_UINT32  RecordIndex;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCompare->");

    for (RecordIndex = 0; RecordIndex < RecordLen; RecordIndex++)
    {
        if (*(pRecordA + RecordIndex) != *(pRecordB + RecordIndex))
        {
            return LSA_FALSE;
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCompare<-");

    LSA_UNUSED_ARG(pDDB);
    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckPDIRPDSyncConsistency()             */
/*                                                                         */
/* D e s c r i p t i o n: Checks PDSync and corresponding PDIRData record  */
/*                        for consistency.                                 */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_CONSISTENCY                      */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDIRPDSyncConsistency( EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  const  pLocalPDSyncDataRecord,
                                                                             EDDI_PRM_RECORD_IRT_PTR_TYPE          const  pRecordSet,
                                                                             EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandlerA;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandlerB;
    LSA_UINT16                     SyncRole;
    LSA_UINT8                      SyncClass;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRPDSyncConsistency->");

    SyncRole  = EDDI_GetBF16Host(pLocalPDSyncDataRecord->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__ROLE);

    SyncClass = pLocalPDSyncDataRecord->PTCP_MasterPriority1;   /* EDDI_PRM_PDSYNCDATA_PTCP_SLAVE            */
                                                                /* EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER   */
                                                                /* EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER */

    /* ------------------------------------------------------------------------*/
    /* Check SyncRole and SyncClass                                            */
    /* ------------------------------------------------------------------------*/

    if (SyncRole == SYNC_PROP_ROLE_CLOCK_MASTER)
    {
        if (   (SyncClass != EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER)
            && (SyncClass != EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncProperties_PDIRPDSyncConsistency);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDSYNC_DATA,
                                                             pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_PTCP_MasterPriority1_OFFSET,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, Invalid SyncProperties for Master, SyncProperties:0x%X",
                              pLocalPDSyncDataRecord->SyncProperties.Value16);
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }

    if (SyncRole == SYNC_PROP_ROLE_EXTERNAL_SYNC)
    {
        /* Wird vorerst nicht geprüft, da Step7 in alten Versionen noch eine 3 abfüllt.
           --> Klärungsbedarf.
        if ( SyncClass != SYNC_PROP_SYNC_CLASS_TEST_STRATUM )
        {
          EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, Invalid SyncProperties for Slave, SyncProperties:0x%X",
                           pLocalPDSyncDataRecord->SyncProperties.Value);
          EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncProperties);
          EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                           EDDI_PRM_INDEX_PDSYNC_DATA,
                                                           pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_SyncProperties_OFFSET,
                                                           EDD_PRM_ERR_FAULT_DEFAULT);
          return EDD_STS_ERR_PRM_CONSISTENCY;
        }
        */
    }

    /* ------------------------------------------------------------------------*/
    /* Check for IRT Overlapping                                               */
    /* ------------------------------------------------------------------------*/

    if (   (pLocalPDSyncDataRecord->ReservedIntervalBegin != 0)
        && (pLocalPDSyncDataRecord->ReservedIntervalEnd   != 0)
        && (!EDDI_SyncIrCheckOverlapIrtIsrt(pDDB, pRecordSet, pLocalPDSyncDataRecord->ReservedIntervalBegin)))
    {
        EDDI_SET_DETAIL_ERR (pDDB, EDDI_ERR_INTERVAL_OVERLAP_IRT_ISRT);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDD_PRM_ERR_INDEX_DEFAULT,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         EDD_PRM_ERR_FAULT_DEFAULT);
        return EDD_STS_ERR_PRM_CONSISTENCY;
    }

    /* ------------------------------------------------------------------------*/

    pFrmHandlerA = EDDI_SyncIrFindFrameHandler(pDDB, (LSA_UINT16)EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3, pRecordSet);

    /* Note: EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B is used as dummy with secondary RTSync master! */
    pFrmHandlerB = EDDI_SyncIrFindFrameHandler(pDDB, (LSA_UINT16)EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B, pRecordSet);

    /* ------------------------------------------------------------------------*/
    /* Check if at least one Framehandler is present.                          */
    /* (If not we use sync without RTClass3 frames)                            */
    /* ------------------------------------------------------------------------*/

    if (   (LSA_HOST_PTR_ARE_EQUAL(pFrmHandlerA, LSA_NULL))
        && (LSA_HOST_PTR_ARE_EQUAL(pFrmHandlerB, LSA_NULL)))
    {
        /* No IRData for SYNC frames present.. we can finish here */
        return EDD_STS_OK;
    }

    /* ------------------------------------------------------------------------*/
    /* The following combinations are valid:                                   */
    /* (NOTE: we may have non RTClass3 Sync frames!)                    Redund */
    /*                                                                  Redund */
    /* A)  1 x Consumer (Primary or Secondary)     : Sync Slave            -   */
    /* B)  1 x Provider (Primary or Secondary)     : Sync Master           -   */
    /* C)  2 x Consumer (Primary  and  Secondary)  : Sync Slave            x   */
    /* D)  1 x Provider (Primary   or  Secondary)  : Primary or Secondary  x   */
    /*     1 x Consumer (Secondary or  Primary)      Sync Master               */
    /*                                                                         */
    /* Not expected: Two Masters                                               */
    /*                                                                         */
    /* Note on D): if Provider is primary, Consumer must be secondary          */
    /*             if Provider is secondary, Consumer must be primary          */
    /* ------------------------------------------------------------------------*/
    /* Note: A FrameHandler can only be a Consumer/Fwder or a Provider!        */
    /* ------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pFrmHandlerA, LSA_NULL))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckPDIRPDSyncConsistency, PRIM-IRData present.");
    }

    if (!LSA_HOST_PTR_ARE_EQUAL(pFrmHandlerB, LSA_NULL))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckPDIRPDSyncConsistency, SEC-IRData present.");
    }

    /* ------------------------------------------------------------------------*/
    /* Check for Slave (Cases A and C)                                         */
    /* ------------------------------------------------------------------------*/
    if (SyncRole == SYNC_PROP_ROLE_EXTERNAL_SYNC)
    {
        if (pFrmHandlerA)
        {
            if (   (pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_CONSUMER)
                && (pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_FWD_CONSUMER))
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatch_A_PDIRPDSyncConsistency);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, EDDI_ERR_INV_RoleIrDataMismatch, PDIRState:0x%X PDSyncState:0x%X",
                                  pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }

        /* If redundance (C) a secondary CONSUMER handler may be present*/
        if (pFrmHandlerB)
        {
            if (   (pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_CONSUMER)
                && (pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_FWD_CONSUMER))
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatch_B_PDIRPDSyncConsistency);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, EDDI_ERR_INV_RoleIrDataMismatch, PDIRState:0x%X PDSyncState:0x%X",
                                  pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
    }

    /* ------------------------------------------------------------------------*/
    /* Check for Master (Cases B and D)                                        */
    /* ------------------------------------------------------------------------*/
    else if (SyncRole == SYNC_PROP_ROLE_CLOCK_MASTER)
    {
        if ((pFrmHandlerA) && (pFrmHandlerB))
        {
            /* Case D */
            if (SyncClass == EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER)
            {
                /* if Secondary master */
                /* -> Handler A = Consumer, Handler B = Provider expected */
                if (((pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_CONSUMER)      &&
                     (pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_FWD_CONSUMER)) ||
                    (pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_PROVIDER))
                {
                    // RTC-SyncMaster has no Provider FCWs
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatchSec_PDIRPDSyncConsistency);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, Secondary redundant SyncMaster has unexpected Rx or Tx-FCWs, PDIRState:0x%X PDSyncState:0x%X",
                                      pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
            else
            {
                /* if primary master */
                /* -> Handler A = Provider, Handler B = Consumer expected */
                if (((pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_CONSUMER)      &&
                     (pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_FWD_CONSUMER)) ||
                    (pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_PROVIDER))
                {
                    // RTC-SyncMaster has no Provider FCWs
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatchSec_PDIRPDSyncConsistency);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, Primary redundant SyncMaster has unexpected Rx or Tx-FCWs, PDIRState:0x%X PDSyncState:0x%X",
                                      pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
        }
        else
        {
            /* Case B */

            if (pFrmHandlerA)
            {
                if (pFrmHandlerA->HandlerType != EDDI_SYNC_IRT_PROVIDER)
                {
                    // RTC-SyncMaster has no FCWs
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatchSec_PDIRPDSyncConsistency);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, RTC-SyncMaster PRI has no Tx-FCWs, PDIRState:0x%X PDSyncState:0x%X",
                                      pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }

            if (pFrmHandlerB)
            {
                if (pFrmHandlerB->HandlerType != EDDI_SYNC_IRT_PROVIDER)
                {
                    // RTC-SyncMaster has no FCWs
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RoleIrDataMismatchSec_PDIRPDSyncConsistency);
                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                     EDDI_PRM_INDEX_PDSYNC_DATA,
                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, RTC-SyncMaster SEC has no Tx-FCWs, PDIRState:0x%X PDSyncState:0x%X",
                                      pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                    return EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
        }
    }
    else
    {
        EDDI_Excp("EDDI_PrmCheckPDIRPDSyncConsistency, Wrong SyncRole", EDDI_FATAL_ERR_EXCP, SyncRole, 0);
        return EDD_STS_ERR_EXCP;
    }

    /* ------------------------------------------------------------------------*/
    /* Note: It was already checked that a maximum of two data-elements are    */
    /*       present if the handler is a consumer! not checked here again.     */
    /*       -> define: EDDI_SYNC_MAX_REDUNDANT_ELEMENTS                       */
    /* ------------------------------------------------------------------------*/

    /* ------------------------------------------------------------------------*/
    /* Additional checks for IRTE REV5                                         */
    /*                                                                         */
    /* With redundance we must asure that the forwarding of primary and second */
    /* are equal because of special handling of linedelay with Rev5!           */
    /*                                                                         */
    /* only possible for ring-ports. non ring ports are different!             */
    /*                                                                         */
    /* we currently check only:                                                */
    /* -> both must have the same RxPorts if a ring and redundancy             */
    /* ------------------------------------------------------------------------*/

    #if defined (EDDI_CFG_ERTEC_400)
    if (   (pFrmHandlerA)
        && (pFrmHandlerB)
        && (SyncRole == SYNC_PROP_ROLE_EXTERNAL_SYNC)) // slave
    {
        /* We have redundancy and we are a slave */
        LSA_UINT32  iElem;
        LSA_UINT32  AUsrRxPort = 0;
        LSA_UINT32  BUsrRxPort = 0;
        LSA_UINT32  Cnt;

        #if (EDDI_SYNC_MAX_REDUNDANT_ELEMENTS != 2)
        #error "EDDI_SYNC_MAX_REDUNDANT_ELEMENTS <> 2 not expected!"
        #endif

        Cnt = 0;
        /* collect relevant infos */
        for (iElem = 0; iElem < EDDI_SYNC_MAX_REDUNDANT_ELEMENTS; iElem++)
        {
            /*lint --e(613) */
            if (pFrmHandlerA->pIrFrameDataElem[iElem] != EDDI_NULL_PTR)
            {
                /* note: because we are consumer UsrRxPort_0_4 must not be 0 !*/
                AUsrRxPort |= 1 << pFrmHandlerA->pIrFrameDataElem[iElem]->UsrRxPort_0_4;
                Cnt++;
            }

            if (pFrmHandlerB->pIrFrameDataElem[iElem] != EDDI_NULL_PTR)

            {
                /* note: because we are consumer UsrRxPort_0_4 must not be 0 !*/
                BUsrRxPort |= 1 << pFrmHandlerB->pIrFrameDataElem[iElem]->UsrRxPort_0_4;
                Cnt++;
            }
        }

        if (Cnt == 4) // ring with redundancy?
        {
            if (AUsrRxPort!= BUsrRxPort)
            {
                // SyncClass Secondary is not allowed for Standalone Masters
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncPropertiesSec_PDIRPDSyncConsistency);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRPDSyncConsistency, Ringports for slave different for primary/secondary, PDIRState:0x%X PDSyncState:0x%X",
                                  pDDB->PRM.PDIRData.State_B, pDDB->PRM.PDSyncData.State_B);
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
        else
        {
            /* not 4 rx-ports. maybe an error. especially if 3.. */
            /* we dont check..                                   */
            if (Cnt == 3)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCheckPDIRPDSyncConsistency, Redundancy with %d RxPorts?", Cnt);
            }
        }
    }
    #endif //EDDI_CFG_ERTEC_400

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRPDSyncConsistency<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckPrmRecordSize()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPrmRecordSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  i;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPrmRecordSize->");

    i = sizeof (EDDI_MAC_ADR_TYPE);
    /*lint --e(774)*/
    if (EDD_MAC_ADDR_SIZE != i)
    {
        return LSA_FALSE;
    }

    i = sizeof (EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_LENGTH_PDNC_DATA_CHECK != i)
    {
        return LSA_FALSE;
    }

    i = sizeof (EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_MAX != i)
    {
        return LSA_FALSE;
    }

    i = sizeof (EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_LENGTH_PD_CONTROL_PLL != i)
    {
        return LSA_FALSE;
    }

    i = sizeof (EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL != i)
    {
        return LSA_FALSE;
    }

    i = sizeof (EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_LENGTH_PD_TRACE_UNIT_DATA != i)
    {
        return LSA_FALSE;
    }

    i = sizeof(EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE);
    /*lint --e(774)*/
    if (EDDI_PRM_LENGTH_PDSCF_DATA_MAX_CHECK != i)
    {
        return LSA_FALSE;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPrmRecordSize<-");

    LSA_UNUSED_ARG(pDDB);
    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMRequestPrmChangePort                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDI_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDI_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_CHANGE_PORT         +*/
/*+     LSA_RESPONSE_TYPE       Response:   Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDD_RQB_PRM_CHANGE_PORT_TYPE    +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_WRITE_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_HW                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_COMMIT request      +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PRMRequestPrmChangePort(
    EDD_UPPER_RQB_PTR_TYPE   const   pRQB,
    EDDI_LOCAL_DDB_PTR_TYPE  const   pDDB)
{
    LSA_RESULT                            Response     = EDD_STS_OK;
    EDD_RQB_PRM_CHANGE_PORT_TYPE*   const pParam       = (EDD_RQB_PRM_CHANGE_PORT_TYPE*) pRQB->pParam;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                            HwPortIndex;


    if (   (0 == pParam->PortID)
        || (pParam->PortID > pDDB->PM.PortMap.PortCnt))
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PRMRequestPrmChangePort, UsrPortID:0x%X", pParam->PortID);
        return EDD_STS_ERR_PARAM;
    }

    UsrPortIndex = pParam->PortID-1;
    HwPortIndex  = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

    if  (EDD_FEATURE_ENABLE != pDDB->Glob.PortParams[HwPortIndex].PhySupportsHotPlugging)
    {
        Response = EDD_STS_ERR_PARAM;
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMRequestPrmChangePort(): UsrPort %u does not support hot plugging", pParam->PortID);
    }
    else
    {
        /* PortID starts with 1, the -1 modification to get right array element */

        /* statemachine for pulling and plugging port modules */

        if(EDD_PRM_PORTMODULE_PULLED == pParam->ModuleState)
        {
            Response = EDDI_PRMChangePortStateMachine(pDDB, EDDI_PRM_PORT_MODULE_SM_TRIGGER_PULL, UsrPortIndex, HwPortIndex);
        }
        else if (EDD_PRM_PORTMODULE_PLUG_PREPARE == pParam->ModuleState)
        {
            Response = EDDI_PRMChangePortStateMachine(pDDB, EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_PREPARE, UsrPortIndex, HwPortIndex);
            if (EDD_STS_OK == Response)
            {
                pParam->PortparamsNotApplicable = pDDB->PRM.PortparamsNotApplicable[UsrPortIndex];
            }
        }
        else if (EDD_PRM_PORTMODULE_PLUG_COMMIT == pParam->ModuleState)
        {
            if (   (EDD_PRM_APPLY_RECORD_PORT_PARAMS != pParam->ApplyDefaultPortparams)
                && (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != pParam->ApplyDefaultPortparams) )
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMRequestPrmChangePort(): ApplyDefaultPortparams (%d) invalid ", pParam->ApplyDefaultPortparams);
                Response = EDD_STS_ERR_PARAM;
            }
            else
            {
                pDDB->PRM.ApplyDefaultPortparams[UsrPortIndex] = pParam->ApplyDefaultPortparams;
                Response = EDDI_PRMChangePortStateMachine(pDDB, EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_COMMIT, UsrPortIndex, HwPortIndex);
            }
        }
        else
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMRequestPrmChangePort(): unknown ModuleState 0x%X for UsrPort %u", pParam->ModuleState, pParam->PortID);
            Response = EDD_STS_ERR_PARAM;
        }
    }

    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "[H:--] OUT: EDDI_PRMRequestPrmChangePort(): Response=0x%X", Response);

    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortStateMachine_DoPull       +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+  Input                 :    LSA_UINT32                   UsrPortIndex   +*/
/*+  Input                 :    LSA_UINT32                   HWPortIndex    +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to DeviceDescriptionBlock             (in/out) +*/
/*+  UsrPortIndex  : Describes wich Port should be handeled        (in)     +*/
/*+  HWPortIndex   : Describes wich Port should be handeled        (in)     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Helperfunction for EDDI_PRMChangePortStateMachine      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortStateMachine_DoPull(
	EDDI_LOCAL_DDB_PTR_TYPE  const     pDDB,
    LSA_UINT32               const     UsrPortIndex,
    LSA_UINT32               const     HWPortIndex)
{
    /* Deklarations */
    LSA_RESULT Response;
    LSA_BOOL   PortChanged[EDDI_MAX_IRTE_PORT_CNT+1];
    LSA_UINT32 u;

    /* Initialisation */
    Response = EDD_STS_OK;

    pDDB->Glob.PortParams[HWPortIndex].IsPulled = EDD_PORTMODULE_IS_PULLED;
    //EDD will disable this port (by setting the transceiver to PowerDown). For port status signaling in EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT please see the table there.

    /* set PHY to PowerDown */
    Response = EDDI_TRASetPowerDown(pDDB, HWPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE /*bRaw*/);
    if (EDD_STS_OK != Response)
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPull, EDDI_TRASetPowerDown, Response:0x%X HWPortIndex:%d",
                          Response, HWPortIndex);
        EDDI_Excp("EDDI_PRMChangePortStateMachine_DoPull, EDDI_TRASetPowerDown", Response, HWPortIndex, 0);
        return Response;
    }

    /* Set PortStatus to EDD_PORT_NOT_PRESENT */
    pDDB->pLocal_SWITCH->LinkIndExtPara[HWPortIndex].PortStatus = EDD_PORT_NOT_PRESENT;

    /* Trigger ExtLinkIndication */
    for (u = 0; u < EDDI_MAX_IRTE_PORT_CNT; u++)
    {
        PortChanged[u] = LSA_FALSE;
    }
    PortChanged[UsrPortIndex] = LSA_TRUE;
    //check and gen LinkIndExt if neccessary
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    return Response;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortStateMachine_DoPlugPrepare+*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+  Input                 :    LSA_UINT32                   UsrPortIndex   +*/
/*+  Input                 :    LSA_UINT32                   HWPortIndex    +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to DeviceDescriptionBlock             (in/out) +*/
/*+  UsrPortIndex  : Describes wich Port should be handeled        (in)     +*/
/*+  HWPortIndex   : Describes wich Port should be handeled        (in)     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Helperfunction for EDDI_PRMChangePortStateMachine      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortStateMachine_DoPlugPrepare(
	EDDI_LOCAL_DDB_PTR_TYPE const    pDDB,
    LSA_UINT32              const    UsrPortIndex,
    LSA_UINT32              const    HWPortIndex)
{
    /* Deklarations */
    LSA_UINT32  AutonegCapAdvertised;
    LSA_UINT32  AutonegMappingCapability;
    LSA_UINT16  MAUType;
    LSA_UINT8   MediaType;
    LSA_UINT8   IsPOF;
    LSA_UINT8   FXTransceiverType;
    LSA_UINT8   LinkSpeed;
    LSA_UINT8   LinkMode;
    LSA_UINT8   PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_APPLICABLE;
    LSA_BOOL    IsPowerDown = LSA_FALSE;
    LSA_RESULT  RetvalPhyParams = EDD_STS_OK;
    LSA_UINT32  const AutonegCapAdvertisedTemp     = pDDB->Glob.PortParams[HWPortIndex].AutonegCapAdvertised;
    LSA_UINT32  const AutonegMappingCapabilityTemp = pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability;
    LSA_UINT8   const MediaTypeTemp                = pDDB->Glob.PortParams[HWPortIndex].MediaType;
    LSA_UINT8   const IsPOFTemp                    = pDDB->Glob.PortParams[HWPortIndex].IsPOF; 
    LSA_UINT8   const FXTransceiverTypeTemp        = pDDB->Glob.PortParams[HWPortIndex].FXTransceiverType;
    LSA_RESULT  Result;
    LSA_UINT32  ErrOffset;

    /* PHY must be in PowerDown ! If not --> exception*/
    if  (  (EDD_STS_OK ==   EDDI_TRACheckPowerDown(pDDB, HWPortIndex, &IsPowerDown))
        && (!IsPowerDown)  )
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, transceiver is not PowerDown, HWPortIndex:%d",
                          HWPortIndex);
        EDDI_Excp("EDDI_PRMChangePortStateMachine_DoPlugPrepare, transceiver is not PowerDown", EDDI_FATAL_ERR_EXCP, HWPortIndex, 0);
        return EDD_STS_ERR_EXCP;
    }

    //Get current MediaType and Capabilities but skip MAUType
    EDDI_LL_GET_PHY_PARAMS(pDDB->hSysDev, HWPortIndex, EDD_LINK_UNKNOWN /*Speed*/, EDD_LINK_UNKNOWN /*Mode*/, &AutonegCapAdvertised, &AutonegMappingCapability, &MAUType, &MediaType, &IsPOF, &FXTransceiverType );

    //plausible IsPOF
    if (   (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType)
        && (EDD_PORT_OPTICALTYPE_ISPOF != IsPOF)
        && (EDD_PORT_OPTICALTYPE_ISNONPOF != IsPOF))
    {
        EDDI_Excp("EDDI_PRMChangePortStateMachine_DoPlugPrepare, EDDI_LL_GET_PHY_PARAMS, IsPof illegal value", EDDI_FATAL_ERR_EXCP, HWPortIndex, IsPOF);
        return EDD_STS_ERR_EXCP;
    }

    //Stored values are needed for TRA-functions below. In case of an error, te old state is restored.
    pDDB->Glob.PortParams[HWPortIndex].AutonegCapAdvertised     = AutonegCapAdvertised;
    pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability = AutonegMappingCapability;
    pDDB->Glob.PortParams[HWPortIndex].MediaType                = MediaType;
    pDDB->Glob.PortParams[HWPortIndex].IsPOF                    = IsPOF;
    pDDB->Glob.PortParams[HWPortIndex].FXTransceiverType        = FXTransceiverType;

    EDDI_PRM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "  EDDI_PRMChangePortStateMachine_DoPlugPrepare():  HWPortIndex=0x%X AutonegCapAdvertised=0x%X AutonegMappingCapability=0x%X MAUType=0x%X MediaType=%d IsPOF=%d FXTransceiverType=%d",
                      HWPortIndex, AutonegCapAdvertised, AutonegMappingCapability, MAUType, MediaType, IsPOF, FXTransceiverType);

    if (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType  &&  EDD_FX_TRANSCEIVER_MAX <= FXTransceiverType)
    {
        EDDI_Excp("EDDI_PRMChangePortStateMachine_DoPlugPrepare, EDDI_LL_GET_PHY_PARAMS, FXTransceiverType for Fiber Optic HWPortIndex is invalid", EDDI_FATAL_ERR_EXCP, HWPortIndex, FXTransceiverType);
        return EDD_STS_ERR_EXCP;   
    }

    if (EDD_MEDIATYPE_UNKNOWN != MediaType)
    {
        LSA_RESULT  Response;
        
        Response = pDDB->pLocal_SWITCH->TraFctPtrs[HWPortIndex].pTRACheckSpeedModeCapability(pDDB, HWPortIndex, &AutonegCapAdvertised);
        if (EDD_STS_OK != Response)
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, TRACheckSpeedModeCapability failed, AutonegCapAdvertised:0x%X HWPortIndex:%d",
                              AutonegCapAdvertised, HWPortIndex);
        }
        else
        {
            Response = pDDB->pLocal_SWITCH->TraFctPtrs[HWPortIndex].pTRAAutonegMappingCapability(pDDB, HWPortIndex, &AutonegMappingCapability);
            if (EDD_STS_OK != Response)
            {
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, TRAAutonegMappingCapability failed, AutonegMappingCapability:0x%X HWPortIndex:%d",
                                  AutonegMappingCapability, HWPortIndex);
            }
        }

        if (EDD_STS_OK != Response)
        {
            //Revise changes made
            pDDB->Glob.PortParams[HWPortIndex].AutonegCapAdvertised     = AutonegCapAdvertisedTemp;
            pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability = AutonegMappingCapabilityTemp;
            pDDB->Glob.PortParams[HWPortIndex].MediaType                = MediaTypeTemp;
            pDDB->Glob.PortParams[HWPortIndex].IsPOF                    = IsPOFTemp;               
            pDDB->Glob.PortParams[HWPortIndex].FXTransceiverType        = FXTransceiverTypeTemp;
            return (Response);
        }
    }

    // Check PDPortDataAdjust
    if (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].MAUTypePresent)    //No AUTONEG
    {
        /* get Speed and Mode */
        RetvalPhyParams = EDDI_LL_CHECK_PHY_PARAMS( pDDB->hSysDev, HWPortIndex, pDDB->Glob.PortParams[HWPortIndex].AutonegCapAdvertised,
                                                    pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability,
                                                    pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].MAUType, &LinkSpeed, &LinkMode);

        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "  EDDI_PRMChangePortStateMachine_DoPlugPrepare():  HWPortIndex=0x%X LinkSpeed=%d, LinkMode=%d",
                          HWPortIndex, LinkSpeed, LinkMode);

        switch (RetvalPhyParams)
        {
            case EDD_STS_OK:
                if (   (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType)
                    && (EDD_LINK_SPEED_100              != LinkSpeed)
                    && (EDD_LINK_MODE_FULL              != LinkMode ) )
                {
                    //MAUType decode does not fit to optical port (detected in EDDI)
                    PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                }
                break;

            case EDD_STS_ERR_PARAM:
                //MAUType decode does not fit to port submodule currently plugged (detected in EDDI_LL_CHECK_PHY_PARAMS)
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, PortparamsNotApplicable - UsrPortIndex:%u", UsrPortIndex);
                PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                break;

            default:
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, EDDI_LL_CHECK_PHY_PARAMS detected an error, HWPortIndex:%d",
                                  HWPortIndex);
                pDDB->Glob.PortParams[HWPortIndex].AutonegCapAdvertised     = AutonegCapAdvertisedTemp;
                pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability = AutonegMappingCapabilityTemp;
                pDDB->Glob.PortParams[HWPortIndex].MediaType                = MediaTypeTemp;
                pDDB->Glob.PortParams[HWPortIndex].IsPOF                    = IsPOFTemp;               
                pDDB->Glob.PortParams[HWPortIndex].FXTransceiverType        = FXTransceiverTypeTemp;
                return EDD_STS_ERR_PARAM;
        }
    }

    
    //Load new delay parameters
    EDDI_SetDDBDelayParams(pDDB, HWPortIndex, pDDB->Glob.PortParams[HWPortIndex].PhyTransceiver, pDDB->Glob.PortParams[HWPortIndex].MediaType, pDDB->Glob.PortParams[HWPortIndex].FXTransceiverType);

    //Check PDIRData
    Result = EDDI_PrmCheckPDIRDataMaxPortDelays(pDDB, &pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord->PDIRGlobalDataExt, pDDB->PRM.PDIRData.pRecordSet_A->bGlobalDataExtended, UsrPortIndex, &ErrOffset);

    if (EDD_STS_ERR_PARAM == Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, PortparamsNotApplicable - PortIndex:%u", UsrPortIndex);
        PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
    }
    else if (EDD_STS_OK != Result)
    {
        //Traceentry is set in EDDI_PrmCheckPDIRDataMaxPortDelays
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugPrepare, EDDI_PrmCheckPDIRDataMaxPortDelays returned 0x%X for UsrPortIndex %u", Result, UsrPortIndex);
        return EDD_STS_ERR_PARAM;
    }

    pDDB->PRM.PortparamsNotApplicable[UsrPortIndex] = PortparamsNotApplicable;

    return EDD_STS_OK;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortStateMachine_DoPlugCommit +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+  Input                 :    LSA_UINT32                   UsrPortIndex   +*/
/*+  Input                 :    LSA_UINT32                   HWPortIndex    +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to DeviceDescriptionBlock             (in/out) +*/
/*+  UsrPortIndex  : Describes wich Port should be handeled        (in)     +*/
/*+  HWPortIndex   : Describes wich Port should be handeled        (in)     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Helperfunction for EDDI_PRMChangePortStateMachine      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortStateMachine_DoPlugCommit(
	EDDI_LOCAL_DDB_PTR_TYPE  const          pDDB,
    LSA_UINT32               const          UsrPortIndex,
    LSA_UINT32               const          HWPortIndex)
{
    /* Deklarations */
    LSA_RESULT Response;
    LSA_BOOL   PortChanged[EDDI_MAX_IRTE_PORT_CNT+1];
    LSA_UINT32 u;
    LSA_UINT8  PHYPower;
    LSA_UINT8  LinkStat = EDD_LINK_AUTONEG;
    #if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
    LSA_UINT16 PhyAdr;
    #endif

    /* Initalisation */
    Response = EDD_STS_OK;

    #if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
    PhyAdr = pDDB->Glob.PortParams[HWPortIndex].PhyAdr;
    // Switch off polling of the transceiver status registers for this port
    EDDI_SwiPhyEnableLinkIRQ(HWPortIndex, LSA_FALSE, pDDB);
    // Wait synchronously for 12 us
    EDDI_WAIT_10_NS(pDDB->hSysDev, 1200UL);
    // Execute applicationspecific actions prior to initializing the transceiver
    EDDI_LL_NSC_PRESTART_ACTION(pDDB->hSysDev, HWPortIndex, EDDI_NSC_PRESTART_ACTION_CALLED_FROM_PLUG_COMMIT, &PhyAdr);
    if (PhyAdr != pDDB->Glob.PortParams[HWPortIndex].PhyAdr)
    {
        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                          "EDDI_PRMChangePortStateMachine_DoPlugCommit, EDDI_LL_NSC_PRESTART_ACTION, HWPortIndex:0x%X Change of PhyAdr from 0x%X to 0x%X",
                          HWPortIndex, pDDB->Glob.PortParams[HWPortIndex].PhyAdr, PhyAdr);
        pDDB->Glob.PortParams[HWPortIndex].PhyAdr = PhyAdr;

        //Complete reinitialization of the transceiver is needed
        pDDB->pLocal_SWITCH->TraFctPtrs[HWPortIndex].pTRAStartPhy(pDDB, HWPortIndex, LSA_TRUE /*bPlugCommit*/);
    }
    #endif

    /* Set PortStatus to EDD_PORT_PRESENT */
    pDDB->pLocal_SWITCH->LinkIndExtPara[HWPortIndex].PortStatus = EDD_PORT_PRESENT;

    pDDB->Glob.PortParams[HWPortIndex].IsMDIX = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HWPortIndex].MediaType)? EDD_PORT_MDIX_DISABLED : pDDB->Glob.PortParams[HWPortIndex].IsMDIXDSB;

    //Reconfigure PHY to FX or Copper
    Response = EDDI_TRASetFXMode(pDDB, HWPortIndex, (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HWPortIndex].MediaType));
    if (EDD_STS_OK != Response)
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRMChangePortStateMachine_DoPlugCommit, EDDI_TRASetFXMode, Response:0x%X HWPortIndex:%d",
                            Response, HWPortIndex);
        EDDI_Excp("EDDI_PRMChangePortStateMachine_DoPlugCommit, EDDI_TRASetFXMode", Response, HWPortIndex, 0);
        return Response;
    }

    //Signal new delay parameters
    pDDB->pLocal_SWITCH->LinkIndExtPara[HWPortIndex].PortTxDelay = pDDB->Glob.PortParams[HWPortIndex].DelayParams.PortTxDelay;
    pDDB->pLocal_SWITCH->LinkIndExtPara[HWPortIndex].PortRxDelay = pDDB->Glob.PortParams[HWPortIndex].DelayParams.PortRxDelay;

    //Enable the transceiver only, if AdjustPortState record is not in current record set
    PHYPower = (EDD_PHY_STATUS_ENABLED == pDDB->pLocal_SWITCH->LinkIndExtPara[HWPortIndex].PhyStatus ? EDDI_PHY_POWER_ON : EDDI_PHY_POWER_OFF);

    if (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->PRM.ApplyDefaultPortparams[UsrPortIndex])
    {
        //Apply default port parameters
        LinkStat = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HWPortIndex].MediaType) ? EDD_LINK_100MBIT_FULL : EDD_LINK_AUTONEG;
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                            "EDDI_PRMChangePortStateMachine_DoPlugCommit, Default params (Usecase: AUTONEG) applied to PortIndex %d, MediaType:0x%X",
                            UsrPortIndex, pDDB->Glob.PortParams[HWPortIndex].MediaType);
    }
    else
    {
        //Apply AutonegMapping for optical ports
        LinkStat = (pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].MAUTypePresent)?pDDB->PRM.PDPortDataAdjust.RecordSet_A[UsrPortIndex].LinkStat:EDD_LINK_AUTONEG;
        LinkStat = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HWPortIndex].MediaType) ? (LSA_UINT8)(pDDB->Glob.PortParams[HWPortIndex].AutonegMappingCapability & 0xFFUL): LinkStat;
    }
    EDDI_SetMauType(UsrPortIndex, HWPortIndex, PHYPower, LinkStat, pDDB);
    
    #if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
    // Switch on the polling of the transceiver status register again
    EDDI_SwiPhyEnableLinkIRQ(HWPortIndex, LSA_TRUE, pDDB);
    #endif

    /* Trigger ExtLinkIndication */
    for (u = 0; u < EDDI_MAX_IRTE_PORT_CNT; u++)
    {
        PortChanged[u] = LSA_FALSE;
    }
    PortChanged[UsrPortIndex] = LSA_TRUE;
    //check and gen LinkIndExt if neccessary
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    pDDB->Glob.PortParams[HWPortIndex].IsPulled = EDD_PORTMODULE_IS_PLUGGED;

    return Response;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortStateMachine              +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+  Input                 :    EDDI_PORT_MODULE_SM_STATE_TYPE   Trigger    +*/
/*+  Input                 :    LSA_UINT32                   UsrPortIndex   +*/
/*+  Input                 :    LSA_UINT32                   HWPortIndex    +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to DeviceDescriptionBlock             (in/out) +*/
/*+  Trigger       : Trigger to control the statemachine           (in)     +*/
/*+  UsrPortIndex  : Describes wich Port should be handeled        (in)     +*/
/*+  HWPortIndex   : Describes wich Port should be handeled        (in)     +*/
/*+                  0 .. EDDI_MAX_IRTE_PORT_CNT - 1                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Controls PHY up an down configuration                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortStateMachine(
	EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
    EDDI_PRM_PORT_MODULE_SM_TRIGGER_TYPE  const  Trigger,
    LSA_UINT32                            const  UsrPortIndex,
    LSA_UINT32                            const  HWPortIndex)
{
    LSA_RESULT    Response = EDD_STS_OK;

    switch(pDDB->Glob.PortModuleSm[HWPortIndex].State)
    {
        case EDDI_PORT_MODULE_SM_STATE_PULLED:
        {
            if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PULL == Trigger)
            {
                // state persists
            }
            else if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_PREPARE == Trigger)
            {
                /* do things for transition */
                Response = EDDI_PRMChangePortStateMachine_DoPlugPrepare(pDDB, UsrPortIndex, HWPortIndex);
                Response = EDD_STS_OK;  // Dont care

                /* change state */
                pDDB->Glob.PortModuleSm[HWPortIndex].State = EDDI_PORT_MODULE_SM_STATE_PLUGGING;

                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "[H: ] EDDI_PRMChangePortStateMachine():  HWPortIndex=0x%X EDDI_PORT_MODULE_SM_STATE_PULLED -> EDDI_PORT_MODULE_SM_STATE_PLUGGING",
                                  HWPortIndex);
            }
            else
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "[H: ] EDDI_PRMChangePortStateMachine(): ERROR -> Wrong Trigger %d for State EDDI_PORT_MODULE_SM_STATE_PULLED",
                                  Trigger);
                Response = EDD_STS_ERR_SEQUENCE;
            }
        }
        break;

        case EDDI_PORT_MODULE_SM_STATE_PLUGGING:
        {
            if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_PREPARE == Trigger)
            {
                // state persists
            }
            else if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PULL == Trigger)
            {
                /* do things for transition */
                Response = EDDI_PRMChangePortStateMachine_DoPull(pDDB, UsrPortIndex, HWPortIndex);

                /* change state */
                pDDB->Glob.PortModuleSm[HWPortIndex].State = EDDI_PORT_MODULE_SM_STATE_PULLED;

                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "[H: ] EDDI_PRMChangePortStateMachine():  HWPortIndex=0x%X EDDI_PORT_MODULE_SM_STATE_PLUGGING -> EDDI_PORT_MODULE_SM_STATE_PULLED",
                                  HWPortIndex);
            }
            else if (EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_COMMIT == Trigger)
            {
                /* do things for transition */
                Response = EDDI_PRMChangePortStateMachine_DoPlugCommit(pDDB, UsrPortIndex, HWPortIndex);

                /* change state */
                pDDB->Glob.PortModuleSm[HWPortIndex].State = EDDI_PORT_MODULE_SM_STATE_PLUGGED;

                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "[H: ] EDDI_PRMChangePortStateMachine():  HWPortIndex=0x%X EDDI_PORT_MODULE_SM_STATE_PLUGGING -> EDDI_PORT_MODULE_SM_STATE_PLUGGED",
                                  HWPortIndex);
            }
            else
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "[H: ] EDDI_PRMChangePortStateMachine(): ERROR -> Wrong Trigger %d for State EDDI_PORT_MODULE_SM_STATE_PLUGGING",
                                  Trigger);
                Response = EDD_STS_ERR_SEQUENCE;
            }
        }
        break;

        case EDDI_PORT_MODULE_SM_STATE_PLUGGED:
        {
            if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_COMMIT == Trigger)
            {
                // state persists
            }
            else if(EDDI_PRM_PORT_MODULE_SM_TRIGGER_PULL == Trigger)
            {
                /* do things for transition */
                Response = EDDI_PRMChangePortStateMachine_DoPull(pDDB, UsrPortIndex, HWPortIndex);

                /* change state */
                pDDB->Glob.PortModuleSm[HWPortIndex].State = EDDI_PORT_MODULE_SM_STATE_PULLED;

                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "[H: ] EDDI_PRMChangePortStateMachine():  HWPortIndex=0x%X EDDI_PORT_MODULE_SM_STATE_PLUGGED -> EDDI_PORT_MODULE_SM_STATE_PULLED",
                                  HWPortIndex);
            }
            else
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "[H: ] EDDI_PRMChangePortStateMachine(): ERROR -> Wrong Trigger %d for State EDDI_PORT_MODULE_SM_STATE_PLUGGED",
                                  Trigger);
                Response = EDD_STS_ERR_SEQUENCE;
            }
        }
        break;

        default:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "[H: ] EDDI_PRMChangePortStateMachine(): ERROR -> Wrong State 0x%X",
                              pDDB->Glob.PortModuleSm[HWPortIndex].State);
            Response = EDD_STS_ERR_SEQUENCE;
        }
        break;


    }

    return Response;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortState_IsPulled            +*/
/*+  Input                 :    EDDI_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Input                 :    LSA_UINT32                     HWPortIndex  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to Device Description Block           (in)     +*/
/*+  HWPortIndex   : Describes which Port should be handeled       (in)     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Returns LSA_TRUE in case of IsPulled State is set      +*/
/*+                  in the PRM Change Port State Machine,                  +*/
/*+                  otherwise LSA_FALSE                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortState_IsPulled(
	EDDI_LOCAL_DDB_PTR_TYPE  const     pDDB,
    LSA_UINT32               const     HWPortIndex)
{
    return (pDDB->Glob.PortModuleSm[HWPortIndex].State == EDDI_PORT_MODULE_SM_STATE_PULLED);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortState_IsPlugged           +*/
/*+  Input                 :    EDDI_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Input                 :    LSA_UINT32                     HWPortIndex  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to Device Description Block           (in)     +*/
/*+  HWPortIndex   : Describes which Port should be handeled       (in)     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Returns LSA_TRUE in case of IsPlugged State is set     +*/
/*+                  in the PRM Change Port State Machine,                  +*/
/*+                  otherwise LSA_FALSE                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortState_IsPlugged(
	EDDI_LOCAL_DDB_PTR_TYPE  const     pDDB,
    LSA_UINT32               const     HWPortIndex)
{
    return (pDDB->Glob.PortModuleSm[HWPortIndex].State == EDDI_PORT_MODULE_SM_STATE_PLUGGED);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PRMChangePortState_SequenceIsRunning   +*/
/*+  Input                 :    EDDI_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Input                 :    LSA_UINT32                 *   pHWPortIndex +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB          : Pointer to Device Description Block           (in)     +*/
/*+  pHWPortIndex  : First Port in a ChangePort Sequence           (out)    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    Returns LSA_TRUE during the ChangePort Sequence        +*/
/*+                  in the PRM Change Port State Machine,                  +*/
/*+                  otherwise LSA_FALSE                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_PRMChangePortState_IsAnySequenceRunning(
	EDDI_LOCAL_DDB_PTR_TYPE  const     pDDB,
    LSA_UINT32                     *   pHWPortIndex)
{
    LSA_UINT32 HWPortIndex;

    for (HWPortIndex = 0; HWPortIndex < pDDB->PM.PortMap.PortCnt; HWPortIndex++)
    {
        if (!EDDI_PRMChangePortState_IsPlugged(pDDB, HWPortIndex) && !EDDI_PRMChangePortState_IsPulled(pDDB, HWPortIndex))
        {
            if (pHWPortIndex)
            {
                *pHWPortIndex = HWPortIndex;
            }
            return LSA_TRUE;
        }
    }
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_state.c                                             */
/*****************************************************************************/
