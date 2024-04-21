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
/*  F i l e               &F: eddi_swi_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  07.11.07    JS    Added support PortOnlyHasSyncTx                        */
/*  08.01.08    JS    Added MRPRedundantPortsUsed                            */
/*  19.10.09    UL    Removed support PortOnlyHasSyncTx                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ext.h"
#include "eddi_time.h"
#include "eddi_ser_cmd.h"
#include "eddi_swi_ext.h"
#include "eddi_lock.h"
#include "eddi_Tra.h"
#include "eddi_sync_usr.h"

#define EDDI_MODULE_ID     M_ID_SWI_USR
#define LTRC_ACT_MODUL_ID  318

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                 definition                                */
/*===========================================================================*/

/*===========================================================================*/
/*                                 typedefs                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                            local function declaration                     */
/*===========================================================================*/
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrGetParams( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUsrCloseChRes( EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

/*===========================================================================*/
/*                            local function definition                      */
/*===========================================================================*/


/*=============================================================================
* function name: EDDI_SwiUsrGetParams()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrGetParams( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_SWI_GET_PARAMS_PTR_TYPE  pGetParams;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUsrGetParams->");

    pGetParams = (EDD_UPPER_SWI_GET_PARAMS_PTR_TYPE)pRQB->pParam;

    EDDI_SERGetFDBSize(pDDB, &pDDB->Glob.LLHandle, &pGetParams->MACSizeTable);

    pGetParams->VLANSizeTable = 0;
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                      "EDDI_SwiUsrGetParams<-MACSizeTable:0x%X VLANSizeTable:0x%X",
                      pGetParams->MACSizeTable, pGetParams->VLANSizeTable);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                            extern function definition                     */
/*===========================================================================*/


/*=============================================================================
* function name: EDDI_SwiUsrInitComponent()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT                                    UsrPortIndex;
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE  const  pSWITCHComp = &pDDB->SWITCH;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUsrInitComponent->");

    //EDDI_MemSet(pSWITCHComp, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_SWITCH_TYPE));

    pSWITCHComp->UsrHandleCnt = 0;

    //ini LINK
    #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
    pDDB->bFilterLinkIndTrace = LSA_FALSE;
    #endif

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32                      const  HwPortIndex     = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        SWI_LINK_PARAM_TYPE          *  const  pLinkPx         = &pSWITCHComp->LinkPx[HwPortIndex];
        SWI_LINK_IND_EXT_PARAM_TYPE  *  const  pLinkIndExtPara = &pSWITCHComp->LinkIndExtPara[HwPortIndex];

        //Last portspecific portstate
        pLinkPx->LastLinkStatus       = EDD_LINK_DOWN;
        pLinkPx->LastLinkMode         = EDD_LINK_UNKNOWN;
        pLinkPx->LastLinkSpeed        = EDD_LINK_UNKNOWN;

        //Current portspecific portstate
        pLinkPx->LinkStatus           = EDD_LINK_DOWN;
        pLinkPx->LinkMode             = EDD_LINK_UNKNOWN;
        pLinkPx->LinkSpeed            = EDD_LINK_UNKNOWN;
        pLinkPx->bMRPRedundantPort    = LSA_FALSE;

        pDDB->pProfKRamInfo->info.PHY_LinkStatus[HwPortIndex] = EDD_LINK_DOWN;
        pDDB->pProfKRamInfo->info.PHY_LinkMode  [HwPortIndex] = EDD_LINK_UNKNOWN;
        pDDB->pProfKRamInfo->info.PHY_LinkSpeed [HwPortIndex] = EDD_LINK_UNKNOWN;

        //Execute actions after LinkDown
        pLinkPx->bFireLinkDownAction  = LSA_TRUE;

        //Value has been adapted by DeviceSetup
        pLinkPx->PhyPowerOff          = LSA_FALSE;

        //Value has been adapted by DeviceSetup
        pLinkPx->LinkSpeedMode_Config = EDD_LINK_AUTONEG;

        //Last set SpanningTreeState
        pLinkPx->SpanningTreeState    = EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING;

        pLinkIndExtPara->LineDelayInTicksHardware       = EDD_DELAY_UNKNOWN;
        pLinkIndExtPara->LastLineDelayInTicksHardware   = EDD_DELAY_UNKNOWN;

        pLinkIndExtPara->LineDelayInTicksMeasured       = EDD_DELAY_UNKNOWN;
        pLinkIndExtPara->LastLineDelayInTicksMeasured   = EDD_DELAY_UNKNOWN;

        pLinkIndExtPara->CableDelayInNsMeasured         = EDD_DELAY_UNKNOWN;
        pLinkIndExtPara->LastCableDelayInNsMeasured     = EDD_DELAY_UNKNOWN;

        //Value has been adapted by DeviceSetup
        pLinkIndExtPara->PortTxDelay                    = EDD_DELAY_UNKNOWN;
        pLinkIndExtPara->LastPortTxDelay                = EDD_DELAY_UNKNOWN;

        pLinkIndExtPara->PortRxDelay                    = EDD_DELAY_UNKNOWN;
        pLinkIndExtPara->LastPortRxDelay                = EDD_DELAY_UNKNOWN;

        /* UNKNOWN value is not needed, because the default value for pluggable ports is "PRESENT" */
        pLinkIndExtPara->PortStatus                     = EDD_PORT_PRESENT;
        pLinkIndExtPara->LastPortStatus                 = EDD_PORT_PRESENT;

        pLinkIndExtPara->PhyStatus                      = EDD_PHY_STATUS_UNKNOWN;
        pLinkIndExtPara->LastPhyStatus                  = EDD_PHY_STATUS_UNKNOWN;

        pLinkIndExtPara->Autoneg                        = EDD_AUTONEG_UNKNOWN;
        pLinkIndExtPara->LastAutoneg                    = EDD_AUTONEG_UNKNOWN;

        pLinkIndExtPara->IRTPortStatus                  = EDD_IRT_NOT_SUPPORTED;
        pLinkIndExtPara->LastIRTPortStatus              = EDD_IRT_NOT_SUPPORTED;

        pLinkIndExtPara->RTClass2_PortStatus            = EDD_RTCLASS2_NOT_SUPPORTED;
        pLinkIndExtPara->LastRTClass2_PortStatus        = EDD_RTCLASS2_NOT_SUPPORTED;

        pLinkIndExtPara->PortState                      = EDD_PORT_STATE_UNCHANGED;
        pLinkIndExtPara->LastPortState                  = EDD_PORT_STATE_UNCHANGED;

        pLinkIndExtPara->MAUType                        = EDD_MAUTYPE_UNKNOWN;
        pLinkIndExtPara->LastMAUType                    = EDD_MAUTYPE_UNKNOWN;

        pLinkIndExtPara->LastMediaType                  = EDD_MEDIATYPE_UNKNOWN;
        pLinkIndExtPara->LastIsPOF                      = 0;

        pLinkIndExtPara->StateDeterminePhaseShift       = EDDI_STATE_PHASE_SHIFT_CHECK;

        pLinkIndExtPara->SyncId0_TopoOk                 = EDD_PORT_SYNCID0_TOPO_NOT_OK;
        pLinkIndExtPara->LastSyncId0_TopoOk             = EDD_PORT_SYNCID0_TOPO_NOT_OK;

        #if defined (EDDI_CFG_FRAG_ON)
        pLinkIndExtPara->TxFragmentation                = EDD_PORT_NO_TX_FRAGMENTATION;
        pLinkIndExtPara->LastTxFragmentation            = EDD_PORT_NO_TX_FRAGMENTATION;
        #endif

        pLinkIndExtPara->AddCheckEnabled                = EDD_LINK_ADD_CHECK_DISABLED;
        pLinkIndExtPara->LastAddCheckEnabled            = EDD_LINK_ADD_CHECK_DISABLED;

        pLinkIndExtPara->ChangeReason                   = EDD_LINK_CHANGE_REASON_NONE;
        pLinkIndExtPara->LastChangeReason               = EDD_LINK_CHANGE_REASON_NONE;
    }

    /*-------------------------------------------------------------------------*/
    /* Global LinkStatus                                                       */
    /*-------------------------------------------------------------------------*/
    //Last common Indication state
    pSWITCHComp->AutoLastLinkStatus.LinkStatus          = EDD_LINK_DOWN;
    pSWITCHComp->AutoLastLinkStatus.LinkSpeed           = EDD_LINK_UNKNOWN;
    pSWITCHComp->AutoLastLinkStatus.LinkMode            = EDD_LINK_UNKNOWN;

    //Value has been adapted by DeviceSetup
    pSWITCHComp->DisableLinkChangePHYSMI                = LSA_FALSE;

    //Value has been adapted by DeviceSetup
    pSWITCHComp->MinPreamble100Mbit                     = 0;
    pSWITCHComp->MinPreamble10Mbit                      = 0;

    /***  Service EDDI_SRV_SWITCH_SET_AGING_TIME ***/
    //pSWITCHComp->AgePollTimeBase                      = EDDI_TIME_BASE_1MS;
    pSWITCHComp->AgePollTimeBase                        = EDDI_TIME_BASE_1S;
    pSWITCHComp->AgePollTime                            = pDDB->pConstValues->AgePollTime;

    pSWITCHComp->ResetSQ_Cmd                            = 0;
    pSWITCHComp->ResetSQ_pRQB                           = EDDI_NULL_PTR;
    pSWITCHComp->ServiceCanceled                        = 0;
    pSWITCHComp->ResetSQ_Cmd_Fcode                      = SWI_FCODE_NOT_DEFINIED;
    pSWITCHComp->ResetSQ_Reestablish_SpanningTreeState  = LSA_FALSE;

    pSWITCHComp->EnableLinkChange_pRQB                  = EDDI_NULL_PTR;

    pSWITCHComp->SyncMACPrio                            = EDDI_SWI_FDB_PRIO_DEFAULT;
    pSWITCHComp->bMRPRedundantPortsUsed                 = LSA_FALSE;

    pDDB->pLocal_SWITCH                                 = pSWITCHComp;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiUsrIniTimer()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrIniTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                             Status;
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE     pSWITCHComp;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUsrIniTimer->");

    pSWITCHComp = &pDDB->SWITCH;

    //Start Age-Table-Timer in EDDI_DeviceSetupSER beim DeviceSetup, not in Switch-Modul
    Status = EDDI_AllocTimer(pDDB, &pSWITCHComp->AgePollTimerId, (void*)pDDB, (EDDI_TIMEOUT_CBF)EDDI_SWIAgeTimeout,
                             EDDI_TIMER_TYPE_CYCLIC, pSWITCHComp->AgePollTimeBase, EDDI_TIMER_REST);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_SwiUsrIniTimer, AgePollTimerId, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pSWITCHComp->AgePollTimerId);
        return;
    }

    if (EDD_MRP_ROLE_NONE != pDDB->SWITCH.MRPSupportedRole)
    { 
        /***  Service EDDI_SRV_SWITCH_CLEAR_TX_QUEUE ***/
        Status = EDDI_AllocTimer(pDDB, &pSWITCHComp->ResetSQ_TimerId, (void *)pDDB,
                                 (EDDI_TIMEOUT_CBF)EDDI_SwiPortResetSQTimeout,
                                 EDDI_TIMER_TYPE_ONE_SHOT, EDDI_TIME_BASE_10MS, EDDI_TIMER_REST);

        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_SwiUsrIniTimer, ResetSQ_TimerId, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pSWITCHComp->ResetSQ_TimerId);
            return;
        }
    }
    else
    {
        pSWITCHComp->ResetSQ_TimerId = EDDI_CFG_MAX_TIMER;
    }

    #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
    /*** NSC Autoneg Bug ***/
    Status = EDDI_AllocTimer(pDDB, &pSWITCHComp->NSCAutoNegBug.AutonegBugTimer100ms_id, (void *)pDDB,
                             (EDDI_TIMEOUT_CBF)EDDI_SwiNSCAutonegBugTimeout,
                             EDDI_TIMER_TYPE_CYCLIC, EDDI_TIME_BASE_100MS, EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_SwiUsrIniTimer, AutonegBug_TimerId, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pSWITCHComp->NSCAutoNegBug.AutonegBugTimer100ms_id);
        return;
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUsrRelComponent()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE  pSWITCHComp;
    LSA_RESULT                           ret;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiUsrRelComponent->");

    pSWITCHComp = &pDDB->SWITCH;

    //Timer are centrally freed in EDDI_DeviceClose

    //Has been done in EDDI_DeviceSetupSER at DeviceSetup, not in the Switch module
    ret = EDDI_StopTimer(pDDB, pDDB->pLocal_SWITCH->AgePollTimerId);
    switch (ret)
    {
        case EDD_STS_OK:
        case EDDI_STS_OK_TIMER_NOT_RUNNING:
            break;
        default:
            EDDI_Excp("EDDI_SwiUsrRelComponent, EDDI_StopTimer AgePollTimerId", EDDI_FATAL_ERR_EXCP, ret, 0);
            return;
    }
    pSWITCHComp->AgePollTimerId = 0;

    /*** Service EDDI_SRV_SWITCH_CLEAR_TX_QUEUES, Resource freigeben ***/
    if (EDD_MRP_ROLE_NONE != pDDB->SWITCH.MRPSupportedRole)
    {
        ret = EDDI_StopTimer(pDDB, pDDB->pLocal_SWITCH->ResetSQ_TimerId);
        switch (ret)
        {
            case EDD_STS_OK:
            case EDDI_STS_OK_TIMER_NOT_RUNNING:
                break;
            default:
                EDDI_Excp("EDDI_SwiUsrRelComponent, EDDI_StopTimer ResetSQ_TimerId", EDDI_FATAL_ERR_EXCP, ret, 0);
                return;
        }
        pSWITCHComp->ResetSQ_TimerId = 0;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUsrOpenChannel()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE  pSWITCHComp;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUsrOpenChannel->");

    pSWITCHComp = pDDB->pLocal_SWITCH;  /* has to be setup! not checked here */

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUsrOpenChannel, pSWITCHComp->UsrHandleCnt:0x%X", pSWITCHComp->UsrHandleCnt);

    pSWITCHComp->UsrHandleCnt++;        // cnt of currently open handles

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUsrCloseChannel()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrCloseChannel( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE  pSWITCHComp;

    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiUsrCloseChannel->");

    pSWITCHComp = pHDB->pDDB->pLocal_SWITCH;

    if (pSWITCHComp->UsrHandleCnt == 0)
    {
        EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUsrCloseChannel, Switch-Channel was not open.");
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_SwiUsrCloseChRes(pHDB);

    pSWITCHComp->UsrHandleCnt--;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUsrCloseChRes()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrCloseChRes( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB        = pHDB->pDDB;
    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE  const  pSWITCHComp = pDDB->pLocal_SWITCH;
    EDD_UPPER_RQB_PTR_TYPE                      pRQBDummy;
    EDDI_LOCAL_HDB_PTR_TYPE                     pHDB_RQB;
    LSA_RESULT                                  ret;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiUsrCloseChRes->UsrHandleCnt:0x%X", pSWITCHComp->UsrHandleCnt);

    /*** Service EDDI_SRV_SWITCH_LOW_WATER_IND_PROVIDE, free resource ***/
    for (;;)
    {
        pRQBDummy = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->LowWaterIndReq);

        if (pRQBDummy == EDDI_NULL_PTR)
        {
            break;
        }

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDI_SwiUsrCloseChRes, pHDB->LowWaterIndReq.Cnt:0x%X pRQBDummy:0x%X",
                          pHDB->LowWaterIndReq.Cnt, (LSA_UINT32)pRQBDummy);

        EDDI_RequestFinish(pHDB, pRQBDummy, EDD_STS_OK_CANCEL);
        pHDB->LowWaterIndReq.Cnt--;
    }

    pHDB->LowWaterIndReq.pBottom = EDDI_NULL_PTR;
    pHDB->LowWaterIndReq.pTop    = EDDI_NULL_PTR;
    pHDB->LowWaterIndReq.Cnt     = 0;
    pHDB->LowWaterLostMark       = 0;

    /*** Service EDDI_SRV_SWITCH_ENABLE_LINK_CHANGE, free resource ***/
    if (!(pDDB->pLocal_SWITCH->EnableLinkChange_pRQB == EDDI_NULL_PTR))
    {
        pHDB_RQB = (EDDI_LOCAL_HDB_PTR_TYPE)pDDB->pLocal_SWITCH->EnableLinkChange_pRQB->internal_context;

        //!!! Only the channel that allocated the EnableLinkChange_pRQB can free it
        if (pHDB == pHDB_RQB)
        {
            EDDI_RequestFinish(pHDB, pDDB->pLocal_SWITCH->EnableLinkChange_pRQB, EDD_STS_OK_CANCEL);
            pSWITCHComp->EnableLinkChange_pRQB = EDDI_NULL_PTR;
        }
    }

    /*** Service EDDI_SRV_SWITCH_CLEAR_TX_QUEUES, free resource ***/
    if (!(pDDB->pLocal_SWITCH->ResetSQ_pRQB == EDDI_NULL_PTR))
    {
        pHDB_RQB = (EDDI_LOCAL_HDB_PTR_TYPE)pDDB->pLocal_SWITCH->ResetSQ_pRQB->internal_context;

        //!!! Only the channel that allocated the ResetSQ_pRQB can free it
        if (pHDB == pHDB_RQB)
        {
            //Service EDDI_SRV_SWITCH_CLEAR_TX_QUEUES, free resources
            if (SWI_FCODE_NOT_DEFINIED != pSWITCHComp->ResetSQ_Cmd_Fcode)
            {
                ret = EDDI_StopTimer(pDDB, pSWITCHComp->ResetSQ_TimerId);
                switch (ret)
                {
                    case EDD_STS_OK:
                    case EDDI_STS_OK_TIMER_NOT_RUNNING:
                        break;
                    default:
                        EDDI_Excp("EDDI_SwiUsrCloseChRes, EDDI_StopTimer Reset SQ", EDDI_FATAL_ERR_EXCP, ret, 0);
                        return;
                }
            }

            pSWITCHComp->ResetSQ_Cmd                           = 0;
            pSWITCHComp->ResetSQ_Cmd_Fcode                     = SWI_FCODE_NOT_DEFINIED;
            pSWITCHComp->ResetSQ_Reestablish_SpanningTreeState = LSA_FALSE;

            if (!(pDDB->pLocal_SWITCH->ResetSQ_pRQB == EDDI_NULL_PTR))
            {
                pSWITCHComp->ServiceCanceled = pSWITCHComp->ResetSQ_pRQB->Service;
                EDDI_RequestFinish(pHDB, pDDB->pLocal_SWITCH->ResetSQ_pRQB, EDD_STS_OK_CANCEL);
                pSWITCHComp->ResetSQ_pRQB = EDDI_NULL_PTR;
            }

            if (pDDB->CmdIF.cRequest_CLEAR_DYN_FDB)
            {
                pDDB->CmdIF.cRequest_CLEAR_DYN_FDB--;
            }
            else
            {
                EDDI_Excp("EDDI_SwiUsrCloseChRes, cRequest_CLEAR_DYN_FDB is 0!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
        }

        //Free all pending FDB handling requests
        while (pDDB->CmdIF.cRequest_CLEAR_DYN_FDB)
        {
            EDDI_LOCAL_HDB_PTR_TYPE      pHDB_local;
            EDD_UPPER_RQB_PTR_TYPE const pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->ClearFDBQueue);
    
            if (pRQB == EDDI_NULL_PTR)
            {
                EDDI_Excp("EDDI_SwiUsrClearFDBGetNextRequestAndExecute!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }

            if (EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB_local) != EDD_STS_OK)
            {
                EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
                EDDI_RQB_ERROR(pRQB);
            }

            EDDI_RequestFinish(pHDB_local, pRQB, EDD_STS_OK_CANCEL);
            pDDB->CmdIF.cRequest_CLEAR_DYN_FDB--;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/




/*=============================================================================
* function name: EDDI_SwiUsrClearFDBGetNextRequestAndExecute()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrClearFDBGetNextRequestAndExecute( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    if (pDDB->CmdIF.cRequest_CLEAR_DYN_FDB)
    {
        EDDI_LOCAL_HDB_PTR_TYPE      pHDB;
        EDD_UPPER_RQB_PTR_TYPE const pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->ClearFDBQueue);
    
        if (pRQB == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_SwiUsrClearFDBGetNextRequestAndExecute!", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB) != EDD_STS_OK)
        {
            EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
            EDDI_RQB_ERROR(pRQB);
        }

        EDDI_SwiUsrClearFDBRequest(pRQB, pHDB);
    }
}



/*=============================================================================
* function name: EDDI_SwiUsrClearFDBRequest()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrClearFDBRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB)
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB   = pHDB->pDDB;
    

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDDI_SRV_SWITCH_CLEAR_DYN_FDB:
        {
            EDDI_SERAsyncCmd(FCODE_CLEAR_FDB, pHDB->pDDB->Glob.LLHandle.AlterAddressTableWait, 0UL, pDDB, pRQB, LSA_TRUE /*bLock*/);
            break;
        }
        case EDD_SRV_SWITCH_FLUSH_FILTERING_DB:
        {
            LSA_BOOL           bIndicate = LSA_FALSE;
            LSA_RESULT  const  Status    = EDDI_SwiPortFlushFilterDB(pRQB, &bIndicate, pDDB);

            if (bIndicate) //If an error is occurred, then give the cbf back to user
            {
                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiUsrClearFDBRequest, pRQB->Service:0x%X pHDB:0x%X", EDD_RQB_GET_SERVICE(pRQB), (LSA_UINT32)pHDB);
                EDDI_RequestFinish(pHDB, pRQB, Status);
                pDDB->CmdIF.cRequest_CLEAR_DYN_FDB--;
                EDDI_SwiUsrClearFDBGetNextRequestAndExecute(pDDB);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}


/*=============================================================================
* function name: EDDI_SwiUsrRequest()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUsrRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                   EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB   = pHDB->pDDB;
    LSA_RESULT                      Status = EDD_STS_OK;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUsrRequest->ServiceID:0x%X pHDB:0x%X", EDD_RQB_GET_SERVICE(pRQB), (LSA_UINT32)pHDB);

    if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDDI_SRV_SWITCH_SET_FDB_ENTRY:
            case EDDI_SRV_SWITCH_CLEAR_DYN_FDB:
                EDDI_SERSheduledRequest(pHDB->pDDB, pRQB, (LSA_UINT32)EDDI_SwiUsrRequest);
                return;
            default:
                break;
        }
    }

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_SWITCH_GET_PARAMS:
        {
            EDDI_SwiUsrGetParams(pRQB, pDDB);
        }
        break;

        case EDD_SRV_SWITCH_GET_PORT_STATE:
        {
            EDDI_SwiPortGetPortState((EDD_UPPER_SWI_GET_PORT_STATE_PTR_TYPE)pRQB->pParam, pDDB);
        }
        break;

        case EDD_SRV_SWITCH_SET_PORT_STATE:
        {
            Status = EDDI_SwiPortSetPortState((EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE)pRQB->pParam, pDDB);
        }
        break;


        case EDDI_SRV_SWITCH_SET_FDB_ENTRY:
        {
            Status = EDDI_SERSetFDBEntryRun(&pHDB->pDDB->Glob.LLHandle,
                                            (EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE)pRQB->pParam,
                                            LSA_TRUE,
                                            pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_REMOVE_FDB_ENTRY:
        {
            Status = EDDI_SERRemoveFDBEntry(&pDDB->Glob.LLHandle,
                                            (EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE)pRQB->pParam,
                                            pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_SET_AGING_TIME:
        {
            Status = EDDI_SWISetAgeTime(pRQB, pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_GET_AGING_TIME:
        {
            EDDI_SWIGetAgeTime(pRQB, pDDB);
        }
        break;

        case EDD_SRV_SWITCH_FLUSH_FILTERING_DB:
        {
            if (EDD_MRP_ROLE_NONE == pDDB->SWITCH.MRPSupportedRole)
            {
                Status = EDD_STS_ERR_NOT_ALLOWED;
                break;
            }
        } //lint -fallthrough

        case EDDI_SRV_SWITCH_CLEAR_DYN_FDB:
        {
            if (pDDB->CmdIF.cRequest_CLEAR_DYN_FDB)
            {
                pDDB->CmdIF.cRequest_CLEAR_DYN_FDB++;
                EDDI_AddToQueueEnd(pDDB,&pDDB->ClearFDBQueue,pRQB);
            }
            else
            {
                pDDB->CmdIF.cRequest_CLEAR_DYN_FDB++;
                EDDI_SwiUsrClearFDBRequest(pRQB, pHDB);
            }
            return;
        }

        case EDDI_SRV_SWITCH_GET_FDB_ENTRY_MAC:
        {
            Status = EDDI_SERGetFDBEntryMAC(pDDB, &pDDB->Glob.LLHandle,
                                            (EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE)pRQB->pParam);
        }
        break;

        case EDDI_SRV_SWITCH_GET_FDB_ENTRY_INDEX:
        {
            Status = EDDI_SERGetFDBEntryIndex(pDDB, &pDDB->Glob.LLHandle,
                                              (EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE)pRQB->pParam);
        }
        break;

        #if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
        case EDDI_SRV_SWITCH_SET_PORT_MONITOR:
        {
            EDDI_SwiPortSetPortMonitor((EDDI_UPPER_SWI_SET_PORT_MONITOR_PTR_TYPE)pRQB->pParam, pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_GET_PORT_MONITOR:
        {
            EDDI_SwiPortGetPortMonitor((EDDI_UPPER_SWI_GET_PORT_MONITOR_PTR_TYPE)pRQB->pParam, pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_SET_MIRROR_PORT:
        {
            Status = EDDI_SwiPortSetMirrorPort((EDDI_UPPER_SWI_SET_MIRROR_PORT_PTR_TYPE)pRQB->pParam, pDDB);
        }
        break;

        case EDDI_SRV_SWITCH_GET_MIRROR_PORT:
        {
            EDDI_SwiPortGetMirrorPort((EDDI_UPPER_SWI_GET_MIRROR_PORT_PTR_TYPE)pRQB->pParam, pDDB);
            Status = EDD_STS_OK;
            break;
        }
        #endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

        default:
        {
            Status = EDD_STS_ERR_NOT_IMPL;
            break;
        }
    }

    EDDI_RequestFinish(pHDB, pRQB, Status);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SWILinkInterrupt()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SWILinkInterrupt( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  para_1 )
{
    LSA_UINT32        UsrPortIndex;
    SER_SWI_LINK_TYPE LinkStatusPx[EDDI_MAX_IRTE_PORT_CNT];
    LSA_BOOL          PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWILinkInterrupt->");

    LSA_UNUSED_ARG(para_1); //satisfy lint!

    EDDI_ENTER_REST_S();

    //Get Port-Status (Speed and Duplex)
    EDDI_SwiPhyGetLinkState(&LinkStatusPx[0], pDDB);

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32              const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        SWI_LINK_PARAM_TYPE  *  const  pLinkPx     = &pDDB->pLocal_SWITCH->LinkPx[HwPortIndex];

        if //a port change has occured?
           (LinkStatusPx[HwPortIndex].Changed)
        {
            PortChanged[HwPortIndex] = LSA_TRUE;
            pLinkPx->LinkStatus      = LinkStatusPx[HwPortIndex].LinkStatus;
            pLinkPx->LinkSpeed       = LinkStatusPx[HwPortIndex].LinkSpeed;
            pLinkPx->LinkMode        = LinkStatusPx[HwPortIndex].LinkMode;

            //inform port state machines
            EDDI_SyncPortStmsLinkChange(pDDB,
                                        HwPortIndex,
                                        pLinkPx->LinkStatus,
                                        pLinkPx->LinkSpeed,
                                        pLinkPx->LinkMode);
        }
    }

    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_swi_usr.c                                               */
/*****************************************************************************/
