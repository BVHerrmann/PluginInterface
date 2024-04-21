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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS user-interface functions with RQBs          */
/*                                                                           */
/*                          - edds_system()                                  */
/*                          - edds_open_channel()                            */
/*                          - edds_request()                                 */
/*                          - edds_close_channel()                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  10
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_USR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/* get pDDB from hDDB */
#define EDDSGETDDB(hDDB) ((EDDS_LOCAL_DDB_PTR_TYPE) (hDDB))

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/** Handle ExtLinkIndication on EDD_SRV_SWITCH_SET_PORT_STATE finish.
 *
 */
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_HandleExtLinkChangeAfterSWSetPortState(
        EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
        EDD_UPPER_SWI_SET_PORT_STATE_PTR_TO_CONST_TYPE const pPortStates)
{
    LSA_UINT32                                  Idx;
    /*------------------------------------------------------------------------*/
    /* For every Port with changed PortState look for handles requiring a     */
    /* ext-link indication                                                    */
    /*------------------------------------------------------------------------*/
    /* loop over all ports */
    for(Idx=1; Idx <= pDDB->pGlob->HWParams.Caps.PortCnt; ++Idx )
    {
        /* only if PortState changed */
        if ( pPortStates->PortIDState[Idx-1] != EDD_PORT_STATE_UNCHANGED)
        {
            /* PortState differs ? */
            if ( pDDB->pGlob->LinkStatus[Idx].PortState != pPortStates->PortIDState[Idx-1] )
            {
                pDDB->pGlob->LinkStatus[Idx].PortState = pPortStates->PortIDState[Idx-1]; /* save actual portstate */
                EDDS_IndicateExtLinkChange(pDDB,Idx);
            }
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TriggerRequest                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_TRIGGER            +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDDS_UPPER_DEV_INTERNAL_PTR_TYPE +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DEV_INTERNAL_TYPE                            +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     ID       : spezifies the trigger reason.                            +*/
/*+                EDDS_SRV_DEV_TRIGGER ids are described in edds_sys.h     +*/
/*+     Param    : not used.                                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from EDDS for further handling of events +*/
/*+                                                                         +*/
/*+               * Start Provider Cycle                                    +*/
/*+               * Start Consumer Check Cycle                              +*/
/*+               * Start Indication of Provider events.                    +*/
/*+               * cyclic rx overload timer                                +*/
/*+               * Rx overload event                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_TriggerRequest(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                          Status;
    EDDS_LOCAL_HDB_PTR_TYPE             pHDB;
    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE    pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_UINT32                          TraceIdx;

    TraceIdx = EDDS_UNDEF_TRACE_IDX;

    #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                        EDDS_MODULE_ID,
                        __LINE__);
    }
    #endif

    pRQBParam   = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;  //this comment should not be necessary #JB_LINTERROR

    /* use fast version getting pDDB */

    pDDB = EDDSGETDDB(pRQBParam->hDDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
    {
        TraceIdx = pDDB->pGlob->TraceIdx;

    }

    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TriggerRequest(pRQB: 0x%X)",
                           pRQB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
    {
        switch ( pRQBParam->ID)
        {
                /*-------------------------------------------------------------*/
                /* Consumer indications are present and should be handled..    */
                /* This is handled in RQB priority context                     */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_CONS_INDICATION:

                if( pDDB->pGlob->IsSrtUsed )
                {
                    /* confirm pending indication resources */
                    EDDS_RTConsumerConfirmIndication(pDDB);
                }
                break;

                /*-------------------------------------------------------------*/
                /* General RQB(s) processed in scheduler                       */
                /* -> triggered by edds_scheduler                              */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_GEN_REQUEST_PROCESSED:
                {
                    LSA_BOOL RQB_Finished = LSA_FALSE;
                    EDD_UPPER_PRM_COMMIT_PTR_TYPE   pPrmCommit;
                    EDD_UPPER_RQB_PTR_TYPE pLocalRQB;
                    EDDS_RQB_LIST_TYPE     LockFreeLocalQueue; /* local request queue; used to reduce lock */
                    EDDS_RQB_LIST_TYPE_PTR refFinishedGeneralRequestQueue;

                    pLocalRQB = LSA_NULL;
                    LockFreeLocalQueue.pBottom = LSA_NULL;
                    LockFreeLocalQueue.pTop = LSA_NULL;
                    refFinishedGeneralRequestQueue = &pDDB->pGlob->GeneralRequestFinishedQueue;

                    /* get all finished general RQBs; move them to local, lock free queue */
                    EDDS_ENTER(pDDB->hSysDev);
                    pDDB->pGlob->GeneralRequestFinishedTrigger.InUse = LSA_FALSE;
                    LockFreeLocalQueue.pBottom = refFinishedGeneralRequestQueue->pBottom;
                    LockFreeLocalQueue.pTop = refFinishedGeneralRequestQueue->pTop;
                    refFinishedGeneralRequestQueue->pBottom = LSA_NULL;
                    refFinishedGeneralRequestQueue->pTop = LSA_NULL;
                    EDDS_EXIT(pDDB->hSysDev);

                    do {
                        EDDS_RQB_REM_BLOCK_BOTTOM(LockFreeLocalQueue.pBottom,LockFreeLocalQueue.pTop,pLocalRQB);
                        /* now handle all RQBs in lock-free RQB context */
                        if(pLocalRQB)
                        {
                            switch(EDD_RQB_GET_SERVICE(pLocalRQB))
                            {
                                case EDD_SRV_SWITCH_SET_PORT_STATE:
                                {
                                    EDDS_HandleExtLinkChangeAfterSWSetPortState(pDDB,(EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE) pLocalRQB->pParam);
                                }
                                break;
                                case EDD_SRV_PRM_COMMIT:
                                {
                                    /* Finish PrmCommit */
                                    EDDS_PrmCommit_Finish(pDDB);
                                    pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE) pLocalRQB->pParam;

                                    pPrmCommit->RsvIntervalRed.RxEndNs                = 0;
                                    pPrmCommit->RsvIntervalRed.RxLocalTransferEndNs   = 0;
                                    pPrmCommit->RsvIntervalRed.TxEndNs                = 0;
                                    pPrmCommit->RsvIntervalRed.TxLocalTransferEndNs   = 0;
                                    pPrmCommit->RsvIntervalRed.TxLocalTransferStartNs = 0;

                                    RQB_Finished = LSA_TRUE;
                                    EDDS_ScheduledRequestFinish(pLocalRQB);
                                    EDDS_PrmIndicateAll(pDDB);
                                }
                                break;
                                case EDD_SRV_PRM_CHANGE_PORT:
                                {
                                    EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE  pPrmChangePort;
                                    pPrmChangePort = (EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE) pLocalRQB->pParam;
                                    switch(pPrmChangePort->ModuleState)
                                    {
                                        case EDD_PRM_PORTMODULE_PLUG_PREPARE:
                                            pPrmChangePort->PortparamsNotApplicable = pDDB->pGlob->PrmChangePort[pPrmChangePort->PortID-1].PortparamsNotApplicable;
                                            break;
                                        case EDD_PRM_PORTMODULE_PULLED:
                                            EDDS_IndicateExtLinkChange(pDDB,pPrmChangePort->PortID);
                                            break;
                                        case EDD_PRM_PORTMODULE_PLUG_COMMIT:
                                            EDDS_IndicateExtLinkChange(pDDB,pPrmChangePort->PortID);
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                break;
                                default:
                                    break;
                            }

                            if(!RQB_Finished)
                            {
                                EDDS_ScheduledRequestFinish(pLocalRQB);
                            }
                            else
                            {
                                RQB_Finished = LSA_FALSE;
                            }
                        }
                    } while(pLocalRQB);

                    /* do some actions, that may be necessary after finishing request, that where processed by scheduler */
                    {
                        /* stop sw filtering if there is no enabled mc mac address */
                        if(0 == pDDB->pGlob->pMCInfo->cntEnabledMcMac)
                        {
                            EDDS_McSWFilter_Statemachine(pDDB,EDDS_MC_MAC_FILTER_TRIGGER_STOP);
                        }
                    }
                }
                break;

                /*-------------------------------------------------------------*/
                /* ARP Frame filtering processed in RQB-Context                */
                /* -> triggered by edds_scheduler                              */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_FILTER_ARP_FRAME:
                {
                    EDD_UPPER_RQB_PTR_TYPE              pLocalRQB;
                    EDDS_RQB_LIST_TYPE                  LockFreeLocalQueue; /* local request queue; used to reduce lock */
                    EDDS_RQB_LIST_TYPE_PTR              refFilterARPFrameQueue;
                    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE    pNRTComp;
                    EDD_UPPER_NRT_RECV_PTR_TYPE         pLocalRQBRcv;
                    LSA_BOOL                            VLANTagPresent;

                    pLocalRQB               = LSA_NULL;
                    VLANTagPresent              = LSA_FALSE;
                    LockFreeLocalQueue.pBottom  = LSA_NULL;
                    LockFreeLocalQueue.pTop     = LSA_NULL;
                    refFilterARPFrameQueue      = &pDDB->pGlob->FilterARPFrameQueue;

                    /* get all finished general RQBs; move them to local, lock free queue */
                    EDDS_ENTER(pDDB->hSysDev);
                        pDDB->pGlob->FilterARPFrameTrigger.InUse = LSA_FALSE;
                        LockFreeLocalQueue.pBottom               = refFilterARPFrameQueue->pBottom;
                        LockFreeLocalQueue.pTop                  = refFilterARPFrameQueue->pTop;
                        refFilterARPFrameQueue->pBottom          = LSA_NULL;
                        refFilterARPFrameQueue->pTop             = LSA_NULL;
                        pNRTComp                                 = pDDB->pNRT;
                        pHDB                                     = pNRTComp->FilterInfo[EDDS_NRT_FILTER_ARP_IDX].pHDB;
                    EDDS_EXIT(pDDB->hSysDev);

                    do {
                        EDDS_RQB_REM_BLOCK_BOTTOM(LockFreeLocalQueue.pBottom,LockFreeLocalQueue.pTop,pLocalRQB);
                        /* now handle all RQBs in lock-free RQB context */
                        if(pLocalRQB)
                        {
                            /* do the magic */

                            pLocalRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE) pLocalRQB->pParam;

                            // problem: uint8 pBuffer gets casted to uint16 for shifting issues
                            if( EDDS_VLAN_TAG == EDDS_FRAME_GET_LEN_TYPE(pLocalRQBRcv->pBuffer) ) //lint !e826 cast is neccessary (JB 04/2016)
                            {
                                VLANTagPresent = LSA_TRUE;
                            }


                            if(  EDDS_NRTFilterARPFrame(pDDB,
                                                        pLocalRQBRcv->pBuffer,
                                                        VLANTagPresent)
                              && (!pHDB->pNRT->CancelInProgress)
                              )
                            {
                                EDDS_RequestFinish(pHDB,pLocalRQB,EDD_STS_OK);
                            }
                            else // Frame is invalid... we have to push the RQB back to main queue
                            {
                                LSA_UINT32 RxQueueIdx;
                                EDDS_ENTER(pDDB->hSysDev);
                                    RxQueueIdx = pHDB->pNRT->RxQueueIdx;
                                    EDDS_RQB_PUT_BLOCK_TOP(pNRTComp->RxReq[RxQueueIdx].pBottom,
                                                           pNRTComp->RxReq[RxQueueIdx].pTop,
                                                           pLocalRQB);
                                    pHDB->pNRT->RxQueueReqCnt++;
                                EDDS_EXIT(pDDB->hSysDev);
                            }
                        }
                    } while(pLocalRQB);
                }
                break;

                /*-------------------------------------------------------------*/
                /* DCP Frame filtering processed in RQB-Context                */
                /* -> triggered by edds_scheduler                              */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_FILTER_DCP_FRAME:
                {
                    EDD_UPPER_RQB_PTR_TYPE              pLocalRQB;
                    EDDS_RQB_LIST_TYPE                  LockFreeLocalQueue; /* local request queue; used to reduce lock */
                    EDDS_RQB_LIST_TYPE_PTR              refFilterDCPFrameQueue;
                    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE    pNRTComp;
                    EDD_UPPER_NRT_RECV_PTR_TYPE         pLocalRQBRcv;
                    LSA_UINT32                          FilterType;
                    LSA_UINT32                          FrameFilterInfo;
                    LSA_BOOL                            VLANTagPresent;

                    pLocalRQB                        = LSA_NULL;
                    VLANTagPresent              = LSA_FALSE;
                    FilterType                  = EDDS_NRT_FILTER_XRT_DCP_IDX;
                    FrameFilterInfo             = 0;
                    LockFreeLocalQueue.pBottom  = LSA_NULL;
                    LockFreeLocalQueue.pTop     = LSA_NULL;
                    refFilterDCPFrameQueue      = &pDDB->pGlob->FilterDCPFrameQueue;

                    /* get all finished general RQBs; move them to local, lock free queue */
                    EDDS_ENTER(pDDB->hSysDev);
                        pDDB->pGlob->FilterDCPFrameTrigger.InUse = LSA_FALSE;
                        LockFreeLocalQueue.pBottom               = refFilterDCPFrameQueue->pBottom;
                        LockFreeLocalQueue.pTop                  = refFilterDCPFrameQueue->pTop;
                        refFilterDCPFrameQueue->pBottom          = LSA_NULL;
                        refFilterDCPFrameQueue->pTop             = LSA_NULL;
                        pNRTComp                                 = pDDB->pNRT;
                        pHDB                                     = pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_DCP_IDX].pHDB;
                    EDDS_EXIT(pDDB->hSysDev);

                    do {
                        EDDS_RQB_REM_BLOCK_BOTTOM(LockFreeLocalQueue.pBottom,LockFreeLocalQueue.pTop,pLocalRQB);
                        /* now handle all RQBs in lock-free RQB context */
                        if(pLocalRQB)
                        {
                            /* do the magic */

                            pLocalRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE) pLocalRQB->pParam;

                            // problem: uint8 pBuffer gets casted to uint16 for shifting issues
                            if( EDDS_VLAN_TAG == EDDS_FRAME_GET_LEN_TYPE(pLocalRQBRcv->pBuffer) ) //lint !e826 cast is neccessary (JB 04/2016)
                            {
                                VLANTagPresent = LSA_TRUE;
                            }


                            if(  EDDS_NRTFilterXRTFrame(pDDB,
                                                        pLocalRQBRcv->pBuffer,
                                                        pLocalRQBRcv->IOCount,
                                                        VLANTagPresent,
                                                        FilterType,
                                                        &FrameFilterInfo)
                              && (!pHDB->pNRT->CancelInProgress)
                              )
                            {
                                EDDS_RequestFinish(pHDB,pLocalRQB,EDD_STS_OK);
                            }
                            else // Frame is invalid... we have to push the RQB back to main queue
                            {
                                LSA_UINT32 RxQueueIdx;
                                EDDS_ENTER(pDDB->hSysDev);
                                    RxQueueIdx = pHDB->pNRT->RxQueueIdx;
                                    EDDS_RQB_PUT_BLOCK_TOP(pNRTComp->RxReq[RxQueueIdx].pBottom,
                                                           pNRTComp->RxReq[RxQueueIdx].pTop,
                                                           pLocalRQB);
                                    pHDB->pNRT->RxQueueReqCnt++;
                                EDDS_EXIT(pDDB->hSysDev);
                            }
                        }
                    } while(pLocalRQB);
                }
                break;

                /*-------------------------------------------------------------*/
                /* DCP HELLO Frame filtering processed in RQB-Context          */
                /* -> triggered by edds_scheduler                              */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_FILTER_DCP_HELLO_FRAME:
                {
                    EDD_UPPER_RQB_PTR_TYPE              pLocalRQB;
                    EDDS_RQB_LIST_TYPE                  LockFreeLocalQueue; /* local request queue; used to reduce lock */
                    EDDS_RQB_LIST_TYPE_PTR              refFilterDCPHELLOFrameQueue;
                    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE    pNRTComp;
                    EDD_UPPER_NRT_RECV_PTR_TYPE         pLocalRQBRcv;
                    LSA_UINT32                          FilterType;
                    LSA_UINT32                          FrameFilterInfo;
                    LSA_BOOL                            VLANTagPresent;

                    pLocalRQB                        = LSA_NULL;
                    VLANTagPresent              = LSA_FALSE;
                    FilterType                  = EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX;
                    FrameFilterInfo             = 0;
                    LockFreeLocalQueue.pBottom  = LSA_NULL;
                    LockFreeLocalQueue.pTop     = LSA_NULL;
                    refFilterDCPHELLOFrameQueue = &pDDB->pGlob->FilterDCPHELLOFrameQueue;

                    /* get all finished general RQBs; move them to local, lock free queue */
                    EDDS_ENTER(pDDB->hSysDev);
                        pDDB->pGlob->FilterDCPHELLOFrameTrigger.InUse = LSA_FALSE;
                        LockFreeLocalQueue.pBottom                    = refFilterDCPHELLOFrameQueue->pBottom;
                        LockFreeLocalQueue.pTop                       = refFilterDCPHELLOFrameQueue->pTop;
                        refFilterDCPHELLOFrameQueue->pBottom          = LSA_NULL;
                        refFilterDCPHELLOFrameQueue->pTop             = LSA_NULL;
                        pNRTComp                                      = pDDB->pNRT;
                        pHDB                                          = pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX].pHDB;
                    EDDS_EXIT(pDDB->hSysDev);

                    do {
                        EDDS_RQB_REM_BLOCK_BOTTOM(LockFreeLocalQueue.pBottom,LockFreeLocalQueue.pTop,pLocalRQB);
                        /* now handle all RQBs in lock-free RQB context */
                        if(pLocalRQB)
                        {
                            pLocalRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE) pLocalRQB->pParam;

                            // problem: uint8 pBuffer gets casted to uint16 for shifting issues
                            if( EDDS_VLAN_TAG == EDDS_FRAME_GET_LEN_TYPE(pLocalRQBRcv->pBuffer) ) //lint !e826 cast is neccessary (JB 04/2016)
                            {
                                VLANTagPresent = LSA_TRUE;
                            }


                            if(  EDDS_NRTFilterXRTFrame(pDDB,
                                                        pLocalRQBRcv->pBuffer,
                                                        pLocalRQBRcv->IOCount,
                                                        VLANTagPresent,
                                                        FilterType,
                                                        &FrameFilterInfo)
                              && (!pHDB->pNRT->CancelInProgress)
                              )
                            {
                                pLocalRQBRcv->FrameFilterInfo = FrameFilterInfo;
                                EDDS_RequestFinish(pHDB,pLocalRQB,EDD_STS_OK);
                            }
                            else // Frame is invalid... we have to push the RQB back to main queue
                            {
                                LSA_UINT32 RxQueueIdx;
                                EDDS_ENTER(pDDB->hSysDev);
                                    RxQueueIdx = pHDB->pNRT->RxQueueIdx;
                                    EDDS_RQB_PUT_BLOCK_TOP(pNRTComp->RxReq[RxQueueIdx].pBottom,
                                                           pNRTComp->RxReq[RxQueueIdx].pTop,
                                                           pLocalRQB);
                                    pHDB->pNRT->RxQueueReqCnt++;
                                EDDS_EXIT(pDDB->hSysDev);
                            }
                        }
                    } while(pLocalRQB);
                }
                break;

            case EDDS_TRIGGER_SCHEDULER:
            {
                /* Calling edds_scheduler from RQB context is only allowed in NRT-mode;
                 * In NRT-mode, there are no run-time limitation for edds_scheduler!
                 *
                 * Note: there may be one EDDS_TRIGGER_SCHEDULER in request queue, after PRM-Sequence
                 * switched to IO-Mode ?! --> then, ignore this Trigger
                 */

                EDDS_ENTER(pDDB->hSysDev);
                pDDB->pGlob->triggerSchedulerRequest.InUse = LSA_FALSE;
                EDDS_EXIT(pDDB->hSysDev);

                if(! pDDB->pGlob->IO_Configured)/* no lock required, since this is set in RQB context (PRM-commit) */
                {
                    /* NRT-Mode */
                    if(!pDDB->pGlob->NRT_UseNotifySchedulerCall )
                    {
                        /* Locking between Edds scheduler context (cyclic calls in NRT-Mode are executed in this mode):
                         * EDDS_ENTER(pDDB->hSysDev) / EDDS_EXIT(pDDB->hSysDev) is done in edds_scheduler!
                         * call edds_scheduler with "intermediate call".
                         */
                        edds_scheduler(pDDB->hDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);
                    }
                    else
                    {
                        EDDS_DO_NOTIFY_SCHEDULER(pDDB->hSysDev);
                    }
                }
                else
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                           "EDDS_TriggerRequest(EDDS_TRIGGER_SCHEDULER): Trigger ignored, in the meantime IO-mode was set");
                }
            }
            break;

                /*-------------------------------------------------------------*/
                /* NRT-CANCEL Part2. This is triggered from Part1 of CANCEL    */
                /* -> triggered by EDDS_DO_NRT_CANCEL_PART2                    */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_NRT_CANCEL_PART2:
                Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB); /* get handle */

                /* NOTE: Trigger-RQB for NRT cancel contains the lower handle (pHDB) */

                if ( Status != EDD_STS_OK)
                {
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                }

                EDDS_NRTCancelPart2(pHDB);
                break;

            case EDDS_TRIGGER_SCHEDULER_EVENTS_DONE:
            {
                LSA_UINT32 events;

                events = pRQBParam->Param;

                EDDS_ENTER(pDDB->hSysDev);
                pDDB->pGlob->SchedulerEventsDone.InUse = LSA_FALSE;
                EDDS_EXIT(pDDB->hSysDev);

                if( (EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__PRM_DIAG_CYCLE & events) )
                {
                    EDDS_DiagCycleRequest(pDDB);
                }

                if( (EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__LINK_CHANGED & events) )
                {
                    EDDS_HandleLinkIndTrigger(pDDB);
                }
            }
            break;

            default:
                break;
        } /* switch */
    } /* if */

    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TriggerRequest()");

    LSA_UNUSED_ARG(TraceIdx);

}




    /* check if already TraceIdx can be accessed */
        /* TraceIdx can be accessed */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceOpen                             +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_OPEN               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_INI_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     pDDB     : Pointer to device-specific initialization parameters     +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         EDD_STS_ERR_NO_TIMER            +*/
/*+                                         EDD_STS_ERR_HW                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a device. This request is the first request*/
/*+               needed for initialization of the device. It returns an    +*/
/*+               device-handle which have to be used in further calls.     +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//checked within EDDS_ENTER_CHECK_REQUEST
//pDDB gets set within this function
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceOpen(EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT                          Status;
    EDDS_UPPER_DDB_INI_PTR_TYPE         pRqbDDB;
    EDDS_UPPER_DPB_PTR_TO_CONST_TYPE    pDPB;   /* Pointer to device parameter*/
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_BOOL                            DDBInit;
    LSA_BOOL                            SWIError;
    LSA_UINT32                          PortIndex;
    LSA_UINT32                          MRPRedundantPortCnt = 0;
    LSA_UINT8                           RingPort;
    LSA_UINT32                          TraceIdx;

    TraceIdx        = EDDS_UNDEF_TRACE_IDX;

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceOpen(pRQB: 0x%X)",
                           pRQB);

    DDBInit         = LSA_FALSE;
    SWIError        = LSA_FALSE;
    Status          = EDD_STS_OK;
    pDDB            = LSA_NULL;  /* avoids compiler warning */

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceOpen: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        pRqbDDB = (EDDS_UPPER_DDB_INI_PTR_TYPE)pRQB->pParam;

        pDPB = pRqbDDB->pDPB;      /* pointer to hardwarespecific parameters */

        TraceIdx = pDPB->TraceIdx;


        //check for valid mrp instance count (0 .. EDD_CFG_MAX_MRP_INSTANCE_CNT)
        if( EDD_CFG_MAX_MRP_INSTANCE_CNT >= pDPB->SWI.MaxMRP_Instances )
        {
            // MRP is supported?
            if( 0 != pDPB->SWI.MaxMRP_Instances )
            {
                for (PortIndex = 0; PortIndex < EDDS_MAX_PORT_CNT; PortIndex++)
                {
                    RingPort = pDPB->SWI.MRPRingPort[PortIndex];   // EDD_MRP_RING_PORT, EDD_MRP_NO_RING_PORT or EDD_MRP_RING_PORT_DEFAULT

                    // count number of default ring ports
                    if(EDD_MRP_RING_PORT_DEFAULT == RingPort)
                    {
                        MRPRedundantPortCnt++;
                    }
                    else
                    {
                        // check for invalid ring port type
                        if( (EDD_MRP_RING_PORT    != RingPort) &&
                                (EDD_MRP_NO_RING_PORT != RingPort) )
                        {
                            EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPRingPort at index %d.", PortIndex);
                            SWIError = LSA_TRUE;
                        }
                    }
                } //end of loop

                // check default ring port count
                if( !SWIError )
                {
                    // check for even default ring port count
                    if( 0 != (MRPRedundantPortCnt & 1) )
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Uneven default ring port count.");
                        SWIError = LSA_TRUE;
                    }
                }

                // check for valid MRPSupportedRole
                if( !SWIError )
                {
                    if( pDPB->SWI.MRPSupportedRole & (~(EDD_MRP_ROLE_CAP_MANAGER | EDD_MRP_ROLE_CAP_CLIENT | EDD_MRP_ROLE_CAP_AUTOMANAGER)) )
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPSupportedRole.");
                        SWIError = LSA_TRUE;
                    }
                }

                // default role must be part of supported roles
                // NOTE: dont check MRPSupportedMultipleRole, this is done by MRP
                if( !SWIError )
                {
                    LSA_UINT32 MRPSupportedRoleCap = 0;

                    switch( pDPB->SWI.MRPDefaultRoleInstance0 )
                    {
                        case EDD_MRP_ROLE_MANAGER:
                        {
                            MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_MANAGER;
                            break;
                        }
                        case EDD_MRP_ROLE_CLIENT:
                        {
                            MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_CLIENT;
                            break;
                        }
                        case EDD_MRP_ROLE_AUTOMANAGER:
                        {
                            MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_AUTOMANAGER;
                            break;
                        }
                        case EDD_MRP_ROLE_NONE:
                        {
                            MRPSupportedRoleCap = 0;
                            break;
                        }

                        default:
                        {
                            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPDefaultRoleInstance0.");
                            SWIError = LSA_TRUE;
                            break;
                        }
                    }

                    if ( !SWIError )
                    {
                        if(    !(pDPB->SWI.MRPSupportedRole & MRPSupportedRoleCap)
                                &&  (MRPSupportedRoleCap != 0))
                        {
                            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: MRPSupportedRole does not match MRPDefaultRoleInstance0.");
                            SWIError = LSA_TRUE;
                        }
                    }
                    // default role must be part of supported roles

                    if ( !SWIError )
	                  {
	                      if( EDD_FEATURE_DISABLE != pDPB->FeatureSupport.ApplicationExist )
	                      {
	                          EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: EDDS called as HSYNC application or with illegal value (%d)", pDPB->FeatureSupport.ApplicationExist);
	                          SWIError = LSA_TRUE;
	                      }
	                  }
                }

                if ( !SWIError )
                {
                    //check for valid mrp interconnection instance count (0 .. EDD_CFG_MAX_MRP_IN_INSTANCE_CNT)
                    if( EDD_CFG_MAX_MRP_IN_INSTANCE_CNT >= pDPB->SWI.MaxMRPInterconn_Instances )
                    {
                        LSA_UINT8 InPort;
                        LSA_UINT8 MRPInPortCount = 0;

                        // MRPIn is supported?
                        if( 0 != pDPB->SWI.MaxMRPInterconn_Instances )
                        {
                            for (PortIndex = 0; PortIndex < EDDS_MAX_PORT_CNT; PortIndex++)
                            {
                                InPort = pDPB->SWI.SupportsMRPInterconnPortConfig[PortIndex];   // EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_YES, EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO
                                if( (EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_YES != InPort) && (EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO != InPort) )
                                {
                                    EDDS_SYSTEM_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                                         "EDDS_DeviceOpen: Illegal value for SupportsMRPInterconnPortConfig at index %d.", PortIndex);
                                    SWIError = LSA_TRUE;
                                }
                                else if ( EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_YES == InPort )
                                {
                                    MRPInPortCount++;
                                }
                            }

                            // check for valid SupportedMRPInterconnRole
                            if( !SWIError )
                            {
                                if( ( 0 == MRPInPortCount) && ( pDPB->SWI.SupportedMRPInterconnRole & (EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_MANAGER | EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_CLIENT ) ) )
                                {
                                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: MRP Interconnection Manager or Client possible, but no MRP Interconnection Port configured.");
                                    SWIError = LSA_TRUE;
                                }
                            }


                            // check for valid SupportedMRPInterconnRole
                            if( !SWIError )
                            {
                                if( pDPB->SWI.SupportedMRPInterconnRole & (~(EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_MANAGER | EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_CLIENT ) ) )
                                {
                                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for SupportedMRPInterconnRole.");
                                    SWIError = LSA_TRUE;
                                }
                            }
                        }
                    }
                    else
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MaxMRPInterconn_Instances.");
                        SWIError = LSA_TRUE;
                    }
                }
	          }
            else
            {
                if ( 0 != pDPB->SWI.MaxMRPInterconn_Instances )
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: MaxMRPInterconn_Instances not 0, but MRP is disabled.");
                    SWIError = LSA_TRUE;
                }
            }
        }
        else
        {
            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MaxMRP_Instances.");
            SWIError = LSA_TRUE;
        }

        // check for valid MRAEnableLegacyMode
        if (!SWIError)
        {
            if (   (EDD_MRA_ENABLE_LEGACY_MODE  != pDPB->SWI.MRAEnableLegacyMode)
                && (EDD_MRA_DISABLE_LEGACY_MODE != pDPB->SWI.MRAEnableLegacyMode))
            {
                EDDS_SYSTEM_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDS_DeviceOpen: Illegal value for MRAEnableLegacyMode(%u).", pDPB->SWI.MRAEnableLegacyMode);
                SWIError = LSA_TRUE;
            }
        }

        /* Check for valid parameters, more checks done in EDDS_LL_OPEN */

        if (( pDPB->InterfaceID < EDD_INTERFACE_ID_MIN  )        ||
                ( pDPB->InterfaceID > EDD_INTERFACE_ID_MAX  )        ||
                ( pDPB->MaxInterfaceCntOfAllEDD > EDD_CFG_MAX_INTERFACE_CNT) ||
                ( pDPB->MaxPortCntOfAllEDD > EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE) ||
                ( LSA_NULL == pDPB->pLLFunctionTable )||
                ( LSA_NULL == pDPB->pLLManagementData )||
                ( 0 == pDPB->RxBufferCnt )||
                ( 0 == pDPB->TxBufferCnt )||
                ( /*  ( EDDS_DPB_DO_NOT_USE_ZERO_COPY_IF != pDPB->ZeroCopyInterface )  // only ZeroCopy allowed, due to lack of testability
                 &&*/ ( EDDS_DPB_USE_ZERO_COPY_IF != pDPB->ZeroCopyInterface )) ||
                (   ( EDD_SYS_FILL_INACTIVE != pDPB->NRT.FeedInLoadLimitationActive )
                 && ( EDD_SYS_FILL_ACTIVE != pDPB->NRT.FeedInLoadLimitationActive )) ||
                (   ( EDD_SYS_IO_CONFIGURED_OFF != pDPB->NRT.IO_Configured )
                 && ( EDD_SYS_IO_CONFIGURED_ON != pDPB->NRT.IO_Configured )) ||
                (   ( EDD_SYS_UDP_WHITELIST_FILTER_OFF != pDPB->NRT.RxFilterUDP_Broadcast )
                 && ( EDD_SYS_UDP_WHITELIST_FILTER_ON != pDPB->NRT.RxFilterUDP_Broadcast )) ||
                (   ( EDD_SYS_UDP_WHITELIST_FILTER_OFF != pDPB->NRT.RxFilterUDP_Unicast )
                 && ( EDD_SYS_UDP_WHITELIST_FILTER_ON != pDPB->NRT.RxFilterUDP_Unicast )) ||
                (SWIError)  //lint !e731 JB 12/11/2014 on purpose
            )  //lint !e774 !e845 JB 12/11/2014 see #ifdef
        {
            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Invalid RQB parameter!");
            Status = EDD_STS_ERR_PARAM;
        }

        if (EDD_STS_OK == Status)
        {
            Status  = EDDS_CreateDDB(&pDDB,pDPB);
        }

        if (EDD_STS_OK == Status)
        {
            EDDS_IS_VALID_PTR(pDDB);

            DDBInit = LSA_TRUE;

            pDDB->pGlob->pLLFunctionTable  = (EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE) pDPB->pLLFunctionTable;
            pDDB->pGlob->pLLManagementData = pDPB->pLLManagementData;

            if( !EDDS_LL_AVAILABLE(pDDB,open)
                    || !EDDS_LL_AVAILABLE(pDDB,setup)
                    || !EDDS_LL_AVAILABLE(pDDB,shutdown)
                    || !EDDS_LL_AVAILABLE(pDDB,close)
                    || !EDDS_LL_AVAILABLE(pDDB,enqueueSendBuffer)
                    || !EDDS_LL_AVAILABLE(pDDB,getNextFinishedSendBuffer)
                    || !EDDS_LL_AVAILABLE(pDDB,triggerSend)
                    || !EDDS_LL_AVAILABLE(pDDB,getNextReceivedBuffer)
                    || !EDDS_LL_AVAILABLE(pDDB,provideReceiveBuffer)
                    || !EDDS_LL_AVAILABLE(pDDB,triggerReceive)
                    || !EDDS_LL_AVAILABLE(pDDB,getLinkState)
                    || !EDDS_LL_AVAILABLE(pDDB,getStatistics)
                    || !EDDS_LL_AVAILABLE(pDDB,enableMC)
                    || !EDDS_LL_AVAILABLE(pDDB,disableMC)
                    || !EDDS_LL_AVAILABLE(pDDB,recurringTask)
                    || !EDDS_LL_AVAILABLE(pDDB,setLinkState)
                    || !EDDS_LL_AVAILABLE(pDDB,backupLocationLEDs)
                    || !EDDS_LL_AVAILABLE(pDDB,restoreLocationLEDs)
                    || !EDDS_LL_AVAILABLE(pDDB,setLocationLEDs)
                    //|| !EDDS_LL_AVAILABLE(pDDB,changePort) /* not mandatory anymore */

                    /* EDDS_LL_AVAILABLE(pDDB,timeout) is not necessary, because this function is not mandatory! */
            )
            {
                EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_DeviceOpen: mandatory function pLLFunctionTable is missing!"
                );
                Status = EDD_STS_ERR_PARAM;
            }else
            {
                /* if MRP is supported the corresponding LL functions have to be implemented */
                if(    (!SWIError)
                    && (0 != pDPB->SWI.MaxMRP_Instances) )
                {
                    LSA_UINT8 setSwitchPortState, controlSwitchMulticastFwd, flushSwitchFilteringDB;
                    setSwitchPortState = EDDS_LL_AVAILABLE(pDDB, setSwitchPortState);
                    controlSwitchMulticastFwd = EDDS_LL_AVAILABLE(pDDB, controlSwitchMulticastFwd);
                    flushSwitchFilteringDB = EDDS_LL_AVAILABLE(pDDB, flushSwitchFilteringDB);
                    if(     !setSwitchPortState
                         || !controlSwitchMulticastFwd
                         || !flushSwitchFilteringDB )
                    {
                        EDDS_SYSTEM_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_DeviceOpen: mandatory function for mrp missing: %02u, %02u, %02u",
                                setSwitchPortState,
                                controlSwitchMulticastFwd,
                                flushSwitchFilteringDB
                        );
                        Status = EDD_STS_ERR_PARAM;
                    }
                }

                if( EDD_STS_OK == Status )
                {
                    /* returnvalue: DDB handle */

                    pRqbDDB->hDDB   = pDDB->hDDB;

                    pDDB->hSysDev = pDPB->hSysDev;   // save system handle of device

                    pDDB->pGlob->TraceIdx = pDPB->TraceIdx;

                    /* can use TraceIdx from here */
                    TraceIdx = pDDB->pGlob->TraceIdx;

                    pDDB->pGlob->schedulerCycleIO = pDPB->schedulerCycleIO;
                    pDDB->pGlob->schedulerCycleNRT = pDPB->schedulerCycleNRT;
                    pDDB->pGlob->schedulerRecurringTaskMS = pDPB->schedulerRecurringTaskMS;

                    pDDB->pGlob->HWParams.InterfaceID    = pDPB->InterfaceID;
                    pDDB->pGlob->HWParams.Caps.PortCnt   = EDDS_MAX_PORT_CNT;

                    pDDB->pGlob->MaxInterfaceCntOfAllEDD    = pDPB->MaxInterfaceCntOfAllEDD;
                    pDDB->pGlob->MaxPortCntOfAllEDD         = pDPB->MaxPortCntOfAllEDD;

                    pDDB->pGlob->RxRemainingByteCntPerCycle = 0;

                    pDDB->SWI                               = pDPB->SWI;
                    pDDB->FeatureSupport                    = pDPB->FeatureSupport;

                    /* Default for IO_Configured */
                    /* NOTE: default is IO_Configured=NO, this might be changed during SRT initialisation */
                    pDDB->pGlob->IO_Configured        = LSA_FALSE;
                    pDDB->pGlob->IO_ConfiguredDefault = LSA_FALSE;
                    pDDB->pGlob->NRT_UseNotifySchedulerCall     = LSA_FALSE;

                    /*----------------------------------------------------*/
                    /* Receive Limitation                                 */
                    /*----------------------------------------------------*/
                    pDDB->pGlob->RxPacketsQuota_IO  = pDPB->RxLimitationPackets_IO;
                    pDDB->pGlob->RxPacketsQuota_NRT = pDPB->RxLimitationPackets_NRT;

                    /*---------------------------------------------------------------*/
                    /* initialize HDB management                                     */
                    /*---------------------------------------------------------------*/
                    pDDB->HDBMgmt.MaxHandleCnt  = EDDS_CFG_MAX_CHANNELS;
                    pDDB->HDBMgmt.UsedHandleCnt = 0;

                    /*---------------------------------------------------------------*/
                    /* initialize internal statistics                                */
                    /*---------------------------------------------------------------*/
                    EDDS_INTERNAL_PERFORMANCE_STATISTICS_INIT(pDDB);

                    /*---------------------------------------------------------------*/
                    /* Init DIAG structures                                          */
                    /*---------------------------------------------------------------*/

                    EDDS_PrmInit(pDDB);

                    /*---------------------------------------------------------------*/
                    /* reset internal TIMER structure                                */
                    /*---------------------------------------------------------------*/

                    EDDS_ResetAllTimer(pDDB);
                }
            }
        }
        else
        {
            EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot create DDB (0x%X)!",Status);
        }

        if (EDD_STS_OK == Status)
        {
            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_NRTInitComponent()");
            Status = EDDS_NRTInitComponent(pRQB, pDDB);
        }

        if (EDD_STS_OK == Status)
        {
            EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_RTInitComponent()");
            Status = EDDS_RTInitComponent(pRQB, pDDB);

            if (EDD_STS_OK != Status)
            {
                LSA_RESULT Result;

                EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_NRTRelComponent()");
                Result = EDDS_NRTRelComponent(pDDB);

                if(EDD_STS_OK != Result)
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceOpen: EDDS_NRTRelComponent failed!");
                }
            }
        }

        if (EDD_STS_OK == Status)
        {
            EDDS_IS_VALID_PTR(pDDB);
            /* io configured was modified in RT/NRT Init Component...*/
            EDDS_SchedulerUpdateCycleTime(pDDB);

            /*---------------------------------------------------------------*/
            /* The following is very ethernethardware dependend              */
            /*---------------------------------------------------------------*/

            EDDS_LOWER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_OPEN(pDDB: 0x%X,pDPB: 0x%X)",pDDB,pDPB);

            Status = EDDS_LL_OPEN(pDDB,pDDB->hDDB,pDPB,pDDB->pGlob->TraceIdx, &(pDDB->pGlob->HWParams.Caps));

            EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_OPEN(). Status: 0x%X",Status);

            if(EDD_STS_OK == Status)
            {
                /* special hsync treatment for 1-porter
                 * allowed: ApplExists ; AddFwdRules are 'do not care'
                 * FILL must not be active
                 * HW must support Hsync Appl
                 */
                if(1 == pDDB->pGlob->HWParams.Caps.PortCnt
                    && EDD_FEATURE_ENABLE == pDPB->FeatureSupport.ApplicationExist)
                {
                    if(EDD_SYS_FILL_ACTIVE == pDPB->NRT.FeedInLoadLimitationActive)
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: FILL must not be active with special hsync treatment for 1 porter");
                        Status = EDD_STS_ERR_PARAM;
                    }
                    if(!(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_HSYNC_APPLICATION))
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Hsync Application not supported by current 1-port hardware");
                        Status = EDD_STS_ERR_PARAM;
                    }
                    if(EDD_STS_OK == Status)
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: special hsync treatment for 1 porter");
                    }
                }
                /* hsync cannot be supported without mrp! (without special treatment) */
                else if( (0 == pDPB->SWI.MaxMRP_Instances)
                    && ( (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.AdditionalForwardingRulesSupported)
                        || (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.ApplicationExist) ) )
                {
                    EDDS_SYSTEM_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: HSYNC without MRP, AdditionalForwardingRulesSupported(%d), ApplicationExist(%d)!",
                            pDPB->FeatureSupport.AdditionalForwardingRulesSupported, pDPB->FeatureSupport.ApplicationExist);
                    Status = EDD_STS_ERR_PARAM;
                }
                /* check if hw (not 1p hw!) supports the masked hsync features */
                else
                {
                    if( !(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_HSYNC_APPLICATION)
                    && (EDD_FEATURE_ENABLE == pDPB->FeatureSupport.ApplicationExist))
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Hsync Application not supported by current HW");
                        Status = EDD_STS_ERR_PARAM;
                    }
                    if( !(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_HSYNC_FORWARDER)
                    && (EDD_FEATURE_ENABLE == pDPB->FeatureSupport.AdditionalForwardingRulesSupported))
                    {
                        EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Hsync Forwarder not supported by current HW");
                        Status = EDD_STS_ERR_PARAM;
                    }
                }/* default role must be part of supported roles */

                if(EDD_STS_OK != Status)
                {
                    LSA_RESULT Result;

                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_LL_CLOSE()");

                    EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_CLOSE(pDDB: 0x%X)",pDDB);

                    Result = EDDS_LL_CLOSE(pDDB);

                    EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_CLOSE(). Status: 0x%X",Result);
                }
            }

            if(EDD_STS_OK == Status)
            {
                if(!(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_MRP_INTERCONN_FWD_RULES)
                   && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported))

                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: EDDS_LL_CAPS_HWF_MRP_INTERCONN_FWD_RULES not supported by LL but MRPInterconnFwdRulesSupported enabled");
                    Status = EDD_STS_ERR_PARAM;
                } else if(!(pDDB->pGlob->HWParams.Caps.HWFeatures &  EDDS_LL_CAPS_HWF_MRP_INTERCONN_ORIGINATOR)
                          && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnOriginatorSupported))

                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: EDDS_LL_CAPS_HWF_MRP_INTERCONN_ORIGINATOR not supported by LL but MRPInterconnOriginatorSupported enabled");
                    Status = EDD_STS_ERR_PARAM;
                }

            }

            if (EDD_STS_OK != Status)
            {
                LSA_RESULT Result;

                EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_RTRelComponent()");
                Result = EDDS_RTRelComponent(pDDB);

                if(EDD_STS_OK != Result)
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceOpen: EDDS_RTRelComponent failed!");
                }

                EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceOpen: EDDS_NRTRelComponent()");
                Result = EDDS_NRTRelComponent(pDDB);

                if(EDD_STS_OK != Result)
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceOpen: EDDS_NRTRelComponent failed!");
                }
            }
         }


        /*-----------------------------------------------------------------------*/
        /* If an error occured we close the DDB (must be present here)           */
        /*-----------------------------------------------------------------------*/

        if (( EDD_STS_OK != Status ) && ( DDBInit))
        {
            EDDS_CloseDDB(pRqbDDB->hDDB);
        }

        EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_OPEN finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }

    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_DeviceOpen()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceSetup                            +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     (not used)                      +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Component-ID (not used)         +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_SETUP              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_SETUP_TYPE                               +*/
/*+                                                                         +*/
/*+     hDDB     : Device-Handle                                            +*/
/*+     pDSB     : Pointer to device-specific setup parameters              +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         EDD_STS_ERR_HW                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to setup a device. This request has to be called  +*/
/*+               after ab open_device-call and all components are init.    +*/
/*+               This call setup the ethernet hardware.                    +*/
/*+               because this may cause interrupts, interrupts             +*/
/*+               have to be enabled before calling.                        +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceSetup(EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_SETUP_PTR_TYPE   pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_UPPER_DSB_PTR_TYPE         pDSB;   /* Pointer to device setup-parameter*/
    EDD_RQB_GET_LINK_STATUS_TYPE    LinkStatus;
    LSA_UINT32                      i, j;
    LSA_UINT16                      MAUType;
    LSA_UINT8                       MediaType;
    LSA_UINT8                       IsPOF;
    LSA_UINT32                      PortStatus;
    LSA_UINT8                       PhyStatus;
    LSA_UINT32                      AutonegCapAdvertised;
    LSA_UINT8                       LinkSpeedModeConfigured;
    LSA_UINT32                      TraceIdx;

    TraceIdx = EDDS_UNDEF_TRACE_IDX;

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
                "IN :EDDS_DeviceSetup(pRQB: 0x%X)", pRQB);

    Status = EDD_STS_OK;

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {

        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_UNEXP,
                "EDDS_DeviceSetup: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbDDB = (EDDS_UPPER_DDB_SETUP_PTR_TYPE) pRQB->pParam;

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        /* check if already TraceIdx can be accessed */
        if(Status == EDD_STS_OK)
        {
            /* TraceIdx can be accessed */
            TraceIdx = pDDB->pGlob->TraceIdx;
        }

        if (Status == EDD_STS_OK)
        {
            pDSB = pRqbDDB->pDSB; /* pointer to hardwarespezific parameters */

            /* Check if already setup or not all components initialized. if so   */
            /* we signal a sequence-error                                        */

            if ((pDDB->pGlob->HWIsSetup)
                    || (LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL))
                    || ((LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) && pDDB->pGlob->IsSrtUsed))
                    )
            {
                Status = EDD_STS_ERR_SEQUENCE;
            }
            else
            {
                EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE refHWStartupParam = &pDDB->pGlob->HWParams.hardwareParams;
                refHWStartupParam->PortCnt = EDDS_MAX_PORT_CNT; /* LL can check against max port count */

                /*---------------------------------------------------------------*/
                /* Init LED Blink management.                                    */
                /*---------------------------------------------------------------*/

                pDDB->pGlob->LEDBLink.Status = EDDS_LED_BLINK_INACTIVE;

                /*---------------------------------------------------------------*/
                /* Init PRM DropCnt Timer management.                            */
                /*---------------------------------------------------------------*/
                EDDS_MEMSET_LOCAL(&pDDB->pGlob->DropStats, 0,
                                                sizeof(pDDB->pGlob->DropStats));

                EDDS_StartTimer(
                        pDDB,
                        EDDS_TIMER_PRM,
                        EDDS_GET_SYSTEM_TIME_NS(pDDB->hSysDev) + 1000000 * 1000,
                        1000000 * 1000,
                        EDDS_PRMDiagCycleCBF);

                /*---------------------------------------------------------------*/
                /* Init link management.                                         */
                /*---------------------------------------------------------------*/

                EDDS_MEMSET_LOCAL(pDDB->pGlob->LinkStatus, 0, sizeof(pDDB->pGlob->LinkStatus));

                /*---------------------------------------------------------------*/
                /* Init PrmChangePort managemennt                                */
                /*---------------------------------------------------------------*/

                EDDS_MEMSET_LOCAL(pDDB->pGlob->PrmChangePort, 0, sizeof(pDDB->pGlob->PrmChangePort));

                /*---------------------------------------------------------------*/
                /* This parameters will be copied to DDB because they may be needed */
                /* when we have to (re)setup the ethernetcontroller.                */
                /*---------------------------------------------------------------*/

                pDDB->pGlob->DSBParams = *pDSB; /* save DSB in management */

                EDDS_LOWER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                        "==> EDDS_LL_SETUP(pDDB: 0x%X,pDSB: 0x%X)", pDDB, pDSB);

                Status = EDDS_LL_SETUP(pDDB, pDSB, refHWStartupParam);

                EDDS_LOWER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                        "<== EDDS_LL_SETUP(). Status: 0x%X", Status);

                if (Status == EDD_STS_OK)
                {
                    EDDS_SYSTEM_TRACE_06(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                            "EDDS_DeviceSetup(): Interface MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                            refHWStartupParam->MACAddress.MacAdr[0],refHWStartupParam->MACAddress.MacAdr[1],
                            refHWStartupParam->MACAddress.MacAdr[2],refHWStartupParam->MACAddress.MacAdr[3],
                            refHWStartupParam->MACAddress.MacAdr[4],refHWStartupParam->MACAddress.MacAdr[5]);

                    /* MAC Address shall not be 0! */
                    if (EDDS_MACADDR_IS_0(refHWStartupParam->MACAddress) )
                    {
                        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                    "EDDS_DeviceSetup: IF MACAddress is zero");
                        EDDS_FatalError( EDDS_FATAL_ERR_LL,
                        EDDS_MODULE_ID,
                        __LINE__);
                    }

                    /* MAC Address shall not be MC Address! */
                    if (EDDS_MACADDR_IS_MC(refHWStartupParam->MACAddress))
                    {
                        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                    "EDDS_DeviceSetup: IF MACAddress is MC mac");
                        EDDS_FatalError( EDDS_FATAL_ERR_LL,
                        EDDS_MODULE_ID,
                        __LINE__);
                    }

                    EDDS_SYSTEM_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                            "EDDS_DeviceSetup: PortCnt: %d",
                            pDDB->pGlob->HWParams.Caps.PortCnt);

                    /* Check for Valid PortCnt */
                    if ((pDDB->pGlob->HWParams.Caps.PortCnt < 1)
                            || (pDDB->pGlob->HWParams.Caps.PortCnt
                                    > EDDS_MAX_PORT_CNT))
                    {
                        EDDS_FatalError( EDDS_FATAL_ERR_LL,
                        EDDS_MODULE_ID,
                        __LINE__);
                    }

                    for (i = 1; i <= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
                    {
                        EDDS_SYSTEM_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "EDDS_DeviceSetup(): PortID=%d, LinkSpeedMode=0x%X, PHYPower=0x%X, IsWireless=0x%X",
                                i,
                                refHWStartupParam->LinkSpeedMode[i - 1],
                                refHWStartupParam->PHYPower[i - 1],
                                refHWStartupParam->IsWireless[i - 1]);

                        EDDS_SYSTEM_TRACE_06(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                                    "EDDS_DeviceSetup(): PortMAC: %02X-%02X-%02X-%02X-%02X-%02X",
                                                    refHWStartupParam->MACAddressPort[i-1].MacAdr[0],refHWStartupParam->MACAddressPort[i-1].MacAdr[1],
                                                    refHWStartupParam->MACAddressPort[i-1].MacAdr[2],refHWStartupParam->MACAddressPort[i-1].MacAdr[3],
                                                    refHWStartupParam->MACAddressPort[i-1].MacAdr[4],refHWStartupParam->MACAddressPort[i-1].MacAdr[5]);

                        /* PortMAC Address shall not be 0! */
                        if (EDDS_MACADDR_IS_0(
                                refHWStartupParam->MACAddressPort[i-1]))
                        {
                            EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                        "EDDS_DeviceSetup: MACAddressPort[%u] is zero", i-1);
                            EDDS_FatalError( EDDS_FATAL_ERR_LL,
                            EDDS_MODULE_ID,
                            __LINE__);
                        }

                        /* PortMAC Address shall not be MC! */
                        if (EDDS_MACADDR_IS_MC(
                                refHWStartupParam->MACAddressPort[i-1]))
                        {
                            EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                        "EDDS_DeviceSetup: MACAddressPort[%u] is MC mac", i-1);
                            EDDS_FatalError( EDDS_FATAL_ERR_LL,
                            EDDS_MODULE_ID,
                            __LINE__);
                        }

                        /* Setup PhyStatus Variable for Link */
                        if (refHWStartupParam->PHYPower[i-1]
                                == EDDS_PHY_POWER_ON)
                        {
                            PhyStatus = EDD_PHY_STATUS_ENABLED;
                        }
                        else
                        {
                            PhyStatus = EDD_PHY_STATUS_DISABLED;
                        }

                        /* Get actual Link-Status for the first time */

                        EDDS_LOWER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",
                                pDDB, i);

                        /* Init with Default Params, because automatic initialization not supported by all Lower Layers */
                        LinkStatus.AddCheckEnabled = EDD_LINK_ADD_CHECK_DISABLED;
                        LinkStatus.ChangeReason    = EDD_LINK_CHANGE_REASON_NONE;

                        /* on setup, this call may be synchronous -
                         * no need of LOCK to prevent race condition,
                         * because scheduler is let of the leash on end of this
                         * function (where HWIsSetup is set to LSA_TRUE).
                         *
                         * @note    The Out-Parameters LinkStatus, MAUType, MediaType,
                         *          IsPOF, AutonegCapAdvertised and LinkSpeedModeConfigured
                         *          must be filled to the internal LinkStatus structure,
                         *          before edds_scheduler runs!
                         *          PortStatus is not needed and shall be removed from LL_GET_LINK_STATE.
                         */
                        Status = EDDS_LL_GET_LINK_STATE(pDDB, i, &LinkStatus,
                                                        &MAUType, &MediaType, &IsPOF, &PortStatus,
                                                        &AutonegCapAdvertised,
                                                        &LinkSpeedModeConfigured);
                        /* NOTE: LinkSpeedModeConfigured is not used from this call to LL_GET_LINK_STATE */
                        /*       LinkSpeedMode already determined by EDDS_LL_SETUP                       */
                        if ( EDD_STS_OK != Status )
                        {
                            EDDS_LOWER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                                    "<== EDDS_LL_GET_LINK_STATE(Port: %d). Status <> EDD_STS_OK",
                                    i);
                        }
                        else if ((EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType)
                            && (EDD_PORT_OPTICALTYPE_ISNONPOF != IsPOF)
                            && (EDD_PORT_OPTICALTYPE_ISPOF != IsPOF))
                        {
                            EDDS_LOWER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                                "<== EDDS_LL_GET_LINK_STATE(Port: %d). IsPOF illegal value (%d)",
                                i, IsPOF);
                            Status = EDD_STS_ERR_PARAM;
                        }

                        if ( EDD_STS_OK != Status )
                        {
                            LinkStatus.Mode = EDD_LINK_UNKNOWN;
                            LinkStatus.Speed = EDD_LINK_UNKNOWN;
                            LinkStatus.Status = EDD_LINK_DOWN; /* On error we signal link down */
                            MAUType = EDD_MAUTYPE_UNKNOWN;
                            MediaType = EDD_MEDIATYPE_UNKNOWN;
                            IsPOF = EDD_PORT_OPTICALTYPE_ISNONPOF;
                            AutonegCapAdvertised = 0; /* default: No Capability */
                        }

                        EDDS_LOWER_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_GET_LINK_STATE(Port: %d, Mode: %d,Speed: %d, Status: %d)",
                                i, LinkStatus.Mode, LinkStatus.Speed,
                                LinkStatus.Status);

                        EDDS_LOWER_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_GET_LINK_STATE(PMAUType: %d, MediaType: %d",
                                MAUType, MediaType);

                        EDDS_LOWER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_GET_LINK_STATE(AutonegCap: 0x%X, LinkSpeedModeConfigured: 0x%X, Status : 0x%X",
                                AutonegCapAdvertised,
                                LinkSpeedModeConfigured, Status);

                        /* copy status to actual status */

                        pDDB->pGlob->LinkStatus[i].Status = LinkStatus.Status;
                        pDDB->pGlob->LinkStatus[i].Mode = LinkStatus.Mode;
                        pDDB->pGlob->LinkStatus[i].Speed = LinkStatus.Speed;

                        /* Setup Autoneg setting */
                        if (refHWStartupParam->LinkSpeedMode[i - 1]
                                == EDD_LINK_AUTONEG)
                        {
                            pDDB->pGlob->LinkStatus[i].Autoneg = EDD_AUTONEG_ON;
                        }
                        else
                        {
                            pDDB->pGlob->LinkStatus[i].Autoneg =
                                    EDD_AUTONEG_OFF;
                        }

                        pDDB->pGlob->LinkStatus[i].AddCheckEnabled = LinkStatus.AddCheckEnabled;
                        pDDB->pGlob->LinkStatus[i].ChangeReason = LinkStatus.ChangeReason;
                        pDDB->pGlob->LinkStatus[i].MAUType = MAUType;
                        pDDB->pGlob->LinkStatus[i].MediaType = MediaType;
                        pDDB->pGlob->LinkStatus[i].IsPOF = IsPOF;
                        pDDB->pGlob->LinkStatus[i].PortStatus = EDD_PORT_PRESENT; // has to be EDD_PORT_PRESENT at setup
                        pDDB->pGlob->LinkStatus[i].PortState =
                                (LSA_UINT16) EDD_PORT_STATE_FORWARDING;
                        pDDB->pGlob->LinkStatus[i].PhyStatus =
                                (LSA_UINT8) PhyStatus;
                        pDDB->pGlob->LinkStatus[i].IsPulled = EDD_PORTMODULE_IS_PLUGGED; // has to be EDD_PORTMODULE_IS_PLUGGED for CMTiR at setup
                        pDDB->pGlob->LinkStatus[i].CableDelayNs = 0;
                        pDDB->pGlob->LinkStatus[i].LineDelay = 0;
                        pDDB->pGlob->LinkStatus[i].AutonegCapAdvertised =
                                AutonegCapAdvertised;

                    } /* for */

                    Status = EDD_STS_OK; /* till now, all is ok. */

                    /* If MRP is used set the PortState of R-Ports to ON     */
                    /* Note that the LLIF must set the Portstate to BLOCKING */
                    /* for R-Ports in startup!                               */
                    if (0 != pDDB->SWI.MaxMRP_Instances)
                    {
                        LSA_INT idx;
                        for (idx = 0; idx < EDDS_MAX_PORT_CNT; ++idx)
                        {
                            if (EDD_MRP_RING_PORT_DEFAULT
                                    == pDDB->SWI.MRPRingPort[idx])
                            {
                                // NOTE: index 0 = Auto
                                pDDB->pGlob->LinkStatus[idx + 1].PortState =
                                        (LSA_UINT16) EDD_PORT_STATE_BLOCKING;
                            }
                        }
                    }

                    /* init AUTO mode setting. This is the fastest mode present on any port */
                    EDDS_FindLinkAutoMode(pDDB); //lint !e534 JB 12/11/2014 ret val has no meaning (here)

                    /* setup bandwith of Tx and Rx */
                    EDDS_CalculateTxRxBandwith(pDDB);

                    /*---------------------------------------------------------------*/
                    /* initialize sw filtering of mc mac                             */
                    /*---------------------------------------------------------------*/
                    /* initialize mc mac management */
                    EDDS_McSWFilter_Init(pDDB);

                    /* enable sw filtering if no exact hw filtering */
                    if( !(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_EXACT_MAC_FILTER) )
                    {
                        EDDS_McSWFilter_Statemachine(pDDB,EDDS_MC_MAC_FILTER_TRIGGER_ENABLE);
                    }

                    /* enable receiving of hsync frames within the EDDS for 1PIF spec. treatment
                     * Fwd Rules are 'do not care' */
                    if ( (1 == pDDB->pGlob->HWParams.Caps.PortCnt)
                      && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist) )
                        /* no need to check HWFeatures or FILL; done in DeviceOpen */
                    {
                        EDD_MAC_ADR_TYPE hsyncMac = {{0x01, 0x0E, 0xCF, 0x00, 0x05, 0x00}};

                        /* note: HSYNC mac must be the FIRST mc addr to be enabled! */
                        for (j=0; j<EDD_MAC_ADDR_SIZE; j++)
                        {
                            pDDB->pGlob->pMCInfo->McMac[0].MAC.MacAdr[j] = hsyncMac.MacAdr[j];
                        }

                        /* increment number of enabled mc mac addresses */
                        pDDB->pGlob->pMCInfo->cntEnabledMcMac++;

                        Status = EDDS_McSWFilter_InsertMcMac(pDDB,hsyncMac);
                        if(EDD_STS_OK != Status)
                        {
                            /* insert mac address to filter should not fail */
                            EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                        "EDDS_DeviceSetup: insertion of HSYNC mac failed");
                            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                        }

                        /* NOTE: this must be done at last, see EDDS_McSWFilter_IsEnabledMcMac */
                        pDDB->pGlob->pMCInfo->McMac[0].Cnt = 1;

                        /* first mc mac to be filtered by LL or EDDS: */
                        EDDS_McSWFilter_Statemachine(pDDB,EDDS_MC_MAC_FILTER_TRIGGER_START);

                        Status = EDDS_LL_MC_ENABLE(pDDB, &hsyncMac);
                    }
                }
                /* and now, edds_scheduler is let off the leash...*/
                pDDB->pGlob->HWIsSetup = LSA_TRUE;
            }
        }

        EDDS_SYSTEM_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE,
                "<-- Request: EDDS_SRV_DEV_SETUP finished. Status: 0x%X",
                Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf, pRQB);
    }

    EDDS_SYSTEM_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_DeviceSetup()");

    LSA_UNUSED_ARG(TraceIdx);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceShutdown                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     (not used)                      +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Component-ID (not used)         +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_SHUTDOWN           +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_SETUP_TYPE                               +*/
/*+                                                                         +*/
/*+     hDDB     : Device-Handle                                            +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to shutdown device. This request has to be called +*/
/*+               before closing a device. This function stops the ethernet +*/
/*+               hardware. after this no more interrupts occur. A new      +*/
/*+               DeviceSetup is required to continue.                      +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceShutdown(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    LSA_UINT32                      TraceIdx;

    TraceIdx = EDDS_UNDEF_TRACE_IDX;

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceShutdown(pRQB: 0x%X)",
                           pRQB);

    Status = EDD_STS_OK;

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceShutdown: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbDDB = (EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE)pRQB->pParam;

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        /* check if already TraceIdx can be accessed */
        if(Status == EDD_STS_OK)
        {
            /* TraceIdx can be accessed */
            TraceIdx = pDDB->pGlob->TraceIdx;
        }

        if ( Status == EDD_STS_OK )
        {
            if (  EDDS_IsAnyHandleInUse(pDDB) || /* any handle in use ?      */
                  ( ! pDDB->pGlob->HWIsSetup ))  /* and hw is not setup ?    */
            {
                Status = EDD_STS_ERR_SEQUENCE;   /* yes-> close handle first */
            }
            else
            {

                /*---------------------------------------------------------------*/
                /* The following is very ethernethardware dependend              */
                /* We shutdown the device.                                       */
                /* We set HWIsSetup before to avoid further sends to be done     */
                /* while shutting down.                                          */
                /*---------------------------------------------------------------*/
                EDDS_ENTER(pDDB->hSysDev); // this enter/exit has to be done because of race conditions in multicore environments
                pDDB->pGlob->HWIsSetup = LSA_FALSE;
                EDDS_EXIT(pDDB->hSysDev);

                EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SHUTDOWN(pDDB: 0x%X)",pDDB);

                Status = EDDS_LL_SHUTDOWN(pDDB);

                EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SHUTDOWN(). Status: 0x%X",Status);

                if ( Status != EDD_STS_OK )  /* dont expecting failure here */
                {
                    EDDS_FatalError( EDDS_FATAL_ERR_LL,
                                     EDDS_MODULE_ID,
                                     __LINE__);
                }

                /* Stop LEB Blink timer */
                EDDS_StopTimer(pDDB, EDDS_TIMER_LED_BLINK);

                /* Stop PRM DropCnt timer */
                EDDS_StopTimer(pDDB, EDDS_TIMER_PRM);

                /* now no more interrupts. */

                /*---------------------------------------------------------------*/
                /* Because it is possible that there are some internal tx        */
                /* requests pending we have to shutdown the tx-handling.         */
                /*---------------------------------------------------------------*/

                EDDS_TransmitShutdown(pDDB);

                /*---------------------------------------------------------------*/
                /* stop sw filtering of mc mac addresses                         */
                /*---------------------------------------------------------------*/
                EDDS_McSWFilter_Statemachine(pDDB,EDDS_MC_MAC_FILTER_TRIGGER_STOP);
            }
        }

        EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_SHUTDOWN finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }

    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceShutdown()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceClose                            +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_CLOSE              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_REL_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDevice  : Valid Device-Handle from device-open                     +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDDS_STS_ERR_REF                +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         or others                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to close device. This request finishes all        +*/
/*+               device-handling by terminating all pending requests and   +*/
/*+               shut down the device.                                     +*/
/*+                                                                         +*/
/*+               If handles still open, we return EDD_STS_ERR_SEQUENCE.    +*/
/*+                                                                         +*/
/*+               When closing the device, the systemadaption have to make  +*/
/*+               sure, that no more interrupts come in for this device.    +*/
/*+               (i.e. with this DDB)                                      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_TIMEOUT is set if a timeout on hardwareaccess +*/
/*+               occured, so the hardware couldnt shut down. but the       +*/
/*+               device managment was closed.                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DeviceClose(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_REL_PTR_TYPE     pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    LSA_UINT32                      TraceIdx;

    TraceIdx = EDDS_UNDEF_TRACE_IDX;

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceClose(pRQB: 0x%X)",
                           pRQB);

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceClose: RQB->pParam is NULL!");

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbDDB = (EDDS_UPPER_DDB_REL_PTR_TYPE)pRQB->pParam;

        /* Here we have to check if any handle uses this device */
        /* if so we cant close the device.                      */

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        /* check if already TraceIdx can be accessed */
        if(Status == EDD_STS_OK)
        {
            /* TraceIdx can be accessed */
            TraceIdx = pDDB->pGlob->TraceIdx;
        }

        if ( Status == EDD_STS_OK )
        {
            /* TRACE out statistic counter */
            EDDS_INTERNAL_PERFORMANCE_STATISTICS_TRACE(pDDB);
            /*----------------------------------------------------------*/
            /* check if a RQB is in use (e.g. pending in the RQB-queue) */
            /* this must not occur. If so we signal an sequence-error.  */
            /* the systemadaption must make sure, that no request from  */
            /* isr are pending when closing the device!!                */
            /* (e.g disable interrupts. clear queue. then close device) */
            /* actually there is nothing left to check.               ) */
            /*----------------------------------------------------------*/
            /* DEINIT Internal Statistics                               */
            /*----------------------------------------------------------*/
            EDDS_INTERNAL_PERFORMANCE_STATISTICS_DEINIT(pDDB);

        }

        if ( Status == EDD_STS_OK )
        {
            if (  EDDS_IsAnyHandleInUse(pDDB) || /* any handle in use ?    */
                  ( pDDB->pGlob->HWIsSetup   ))  /* HW-not shutdown yet ?  */
            {
                Status = EDD_STS_ERR_SEQUENCE;   /* yes-> close handle first */
            }
            else
            {
                // do the deinit for RT and NRT
                EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceClose: EDDS_RTRelComponent()");
                Status = EDDS_RTRelComponent(pDDB);

                if (EDD_STS_OK == Status)
                {
                    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_DeviceClose: EDDS_NRTRelComponent()");
                    Status = EDDS_NRTRelComponent(pDDB);
                }

                if (( LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL) )
                    && ( LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) )
                    )
                {

                    /*---------------------------------------------------------------*/
                    /* The following is very ethernethardware dependend              */
                    /* We shutdown the device before removing the DDB-Info.          */
                    /* (It may be possible, that an request with an old DDB is queued*/
                    /* to the driver. This is checked on entry to RQB-handling.      */
                    /*---------------------------------------------------------------*/

                    EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_CLOSE(pDDB: 0x%X)",pDDB);

                    Status = EDDS_LL_CLOSE(pDDB);

                    EDDS_LOWER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_CLOSE(). Status: 0x%X",Status);

                    EDDS_ResetAllTimer(pDDB);

                    /* hDDB was already checked for validness..                      */

                    EDDS_CloseDDB(pRqbDDB->hDDB);

                }
                else
                    Status = EDD_STS_ERR_SEQUENCE; /* components not closed */
            }
        }

        EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_CLOSE finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);

    }
    EDDS_SYSTEM_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceClose()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_system                                 +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     Request-dependend               +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_xxxx               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_DEV_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values in Requestblock                                      +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         others                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles devicerequest which will be used in systemadaption+*/
/*+               This functions have a RQB-Parameter structure.            +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pRQB
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_system(EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_system(pRQB: 0x%X)",
                           pRQB);

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        // expect valid pointer
        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR, "edds_system(): ERROR -> pRQB is NULL!");
        EDDS_RQB_ERROR(pRQB);
        return;
    }

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM )
    {

        EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_system: Invalid opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
                /*-----------------------------------------------------------------------*/
                /* Trigger-request from edds                                             */
                /*-----------------------------------------------------------------------*/
            case EDDS_SRV_DEV_TRIGGER:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_TRIGGER (pRQB: 0x%X)",pRQB);
                EDDS_TriggerRequest(pRQB);
                EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_TRIGGER finished.");
                break;

                /*-----------------------------------------------------------------------*/
                /* Device open/close/setup/shutdown etc                                  */
                /*-----------------------------------------------------------------------*/

            case EDDS_SRV_DEV_OPEN:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_OPEN (pRQB: 0x%X)",pRQB);
                EDDS_DeviceOpen(pRQB);
                break;

            case EDDS_SRV_DEV_SETUP:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_SETUP (pRQB: 0x%X)",pRQB);
                EDDS_DeviceSetup(pRQB);
                break;

            case EDDS_SRV_DEV_SHUTDOWN:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_SHUTDOWN (pRQB: 0x%X)",pRQB);
                EDDS_DeviceShutdown(pRQB);
                break;

            case EDDS_SRV_DEV_CLOSE:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_CLOSE (pRQB: 0x%X)",pRQB);
                EDDS_DeviceClose(pRQB);
                break;

                /*-----------------------------------------------------------------------*/
                /* SWI-Component                                                         */
                /*-----------------------------------------------------------------------*/

            default:
                EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_system: Invalid service (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

                EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_SERVICE);
                EDDS_RQB_ERROR(pRQB);
                break;

        } /* switch */
    } /* else */


    EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_system()");


}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_open_channel                           +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_OPEN_CHANNEL            +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_OPEN_CHANNEL_TYPE                             +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         Handle:         channel-handle of EDDS (ret) +*/
/*+     LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user      +*/
/*+     LSA_SYS_PATH_TYPE       SysPath:        system-path of channel      +*/
/*+     LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)    +*/
/*+                                         (EDD_UPPER_RQB_PTR_TYPE pRQB)   +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_OPEN_CHANNEL            +*/
/*+     LSA_HANDLE_TYPE         Handle:     returned channel of user        +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_SYS_PATH            +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDDS_STS_ERR_LOWER_LAYER        +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel of a device.                    +*/
/*+                                                                         +*/
/*+               Within this request the output - macro EDDS_GET_PATH_INFO +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+               (e.g. the device-handle).                                 +*/
/*+                                                                         +*/
/*+               After a successful open a handle is channel-handle is     +*/
/*+               returned and channel-dependend requests can be used.      +*/
/*+                                                                         +*/
/*+               HandleUpper:                                              +*/
/*+                                                                         +*/
/*+               This handle will be returned in the RQB in all further    +*/
/*+               request-confirmations.                                    +*/
/*+                                                                         +*/
/*+               cbf:                                                      +*/
/*+                                                                         +*/
/*+               The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-funktion will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+               SysPath:                                                  +*/
/*+                                                                         +*/
/*+               The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via EDDS_GET_PATH_INFO.                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//pRQBChannel,pDDB and pDetail are set within this function - future called are checked using Status
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID EDD_UPPER_IN_FCT_ATTR  edds_open_channel(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB = LSA_NULL;
    LSA_SYS_PTR_TYPE                pSys = {0};
    EDDS_DETAIL_PTR_TYPE            pDetail;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE pRQBChannel;
    LSA_UINT16                      PathStat;
    LSA_BOOL                        HandleInit;
    LSA_BOOL                        SysPathInit;
    LSA_UINT16                      RelStat;
    LSA_BOOL                        do_release_path_info = LSA_FALSE;

    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_open_channel(pRQB: 0x%X)",
                           pRQB);

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        // expect valid pointer
        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR, "edds_open_channel(): ERROR -> pRQB is NULL!");
        EDDS_RQB_ERROR(pRQB);
        return;
    }

    HandleInit  = LSA_FALSE;
    SysPathInit = LSA_FALSE;
    Status      = EDD_STS_OK;
    pRQBChannel = LSA_NULL;
    pDetail     = LSA_NULL;  /* access prevents compiler warning */
    pDDB        = LSA_NULL;  /* access prevents compiler warning */

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_OPEN_CHANNEL )
    {
        EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,">>> edds_open_channel: Invalid RQB-Opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        Status = EDD_STS_ERR_OPCODE;
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_OPC_OPEN_CHANNEL (Handle: 0x%X)",EDD_RQB_GET_HANDLE(pRQB));

        if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
        {

            EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_ERROR,"edds_open_channel: pParam is NULL");

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
        else
        {
            pRQBChannel = (EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)pRQB->pParam;

            if ( 0 == pRQBChannel->Cbf)
            {

                EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_ERROR,"edds_open_channel: Cbf is NULL");

                EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
                Status = EDD_STS_ERR_PARAM;
                EDDS_RQB_ERROR(pRQB);
            }
        }
    }


    /*---------------------------------------------------------------------------*/
    /* If parameters are valid, we have a call-back-function now.                */
    /*---------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        /*-----------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                               */
        /*-----------------------------------------------------------------------*/

        EDDS_IS_VALID_PTR(pRQBChannel); /* pRQBChannel must be valid at this point, else it would be EDDS_RQB_ERROR before */

        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_GET_PATH_INFO.");

        EDDS_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQBChannel->SysPath); //lint !e534 TH ignore return value???

        if (PathStat != LSA_RET_OK)
        {
            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"<-- EDDS_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            Status = EDD_STS_ERR_SYS_PATH;
        }
        else if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
        {
            EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: EDDS_GET_PATH_INFO() pDetail is NULL");

            Status = EDD_STS_ERR_PARAM;
        }
        else if (   (   (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UsePRM)
                     && (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UsePRM))
                 || (   (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseNRT)
                     && (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseNRT))
                 || (   (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseCSRT)
                     && (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseCSRT))
            )
        {
            EDDS_UPPER_TRACE_03(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"<-- EDDS_GET_PATH_INFO illegal value for UsePRM (%d)/UseNRT(%d)/UseNRT(%d)",
                pDetail->UsedComp.UsePRM, pDetail->UsedComp.UseNRT, pDetail->UsedComp.UseCSRT);
            Status = EDD_STS_ERR_SYS_PATH;
        }
        else if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UsePRM)
        {
            if (!pDetail->pPRM)
            {
                EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_UNEXP, "<-- EDDS_GET_PATH_INFO pDetail->pPRM NULLPtr");
                Status = EDD_STS_ERR_SYS_PATH;
            }
            else if ((EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD != pDetail->pPRM->PortDataAdjustLesserCheckQuality)
                && (EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY != pDetail->pPRM->PortDataAdjustLesserCheckQuality))
            {
                EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_UNEXP, "<-- EDDS_GET_PATH_INFO PortDataAdjustLesserCheckQuality(%d)",
                    pDetail->pPRM->PortDataAdjustLesserCheckQuality);
                Status = EDD_STS_ERR_SYS_PATH;
            }
        }

        if (EDD_STS_OK == Status)
        {
            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);

            SysPathInit = LSA_TRUE;
        }

        /*-----------------------------------------------------------------------*/
        /* Get pointer to DDB      ..                                            */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            EDDS_IS_VALID_PTR(pDetail); /* pDetail must be valid at this point */

            Status = EDDS_GetDDB(pDetail->hDDB, &pDDB);

            if (Status != EDD_STS_OK)
            {

                EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: Invalid hDBB (0x%X) within CDB",pDetail->hDDB);

                Status = EDD_STS_ERR_PARAM;
            }
            else
            {
                if (pDDB->pGlob->HWIsSetup ) /* check if Device is setup.. */
                {
                    /*-----------------------------------------------------------------------*/
                    /* Get a Handle. On error abort                                          */
                    /*-----------------------------------------------------------------------*/

                    Status = EDDS_HandleAcquire(pDDB, &pHDB); // set pHDB->Handle with Handle

                    if (Status == EDD_STS_OK)
                    {
                        HandleInit = LSA_TRUE;

                        /* ini HDB (Handle-Discription-Block) */

                        pHDB->InUse         = LSA_TRUE;
                        pHDB->UsedComp      = 0;
                        pHDB->SysPath       = pRQBChannel->SysPath;
                        pHDB->UpperHandle   = pRQBChannel->HandleUpper;
                        pHDB->Cbf           = pRQBChannel->Cbf;
                        pHDB->pDDB          = LSA_NULL;

                        /* default */
                        pHDB->pNRT          = LSA_NULL;

                        pHDB->pDDB          = pDDB;
                        pHDB->InsertSrcMAC  = pDetail->InsertSrcMAC;

                        pHDB->pSys          = pSys;
                        pHDB->pDetail       = pDetail;
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: Cannot get free EDDS handle");
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: EDDS not SETUP yet!");

                    Status = EDD_STS_ERR_SEQUENCE;
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   PRM Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
            EDDS_IS_VALID_PTR(pDetail); /* pDetail must be valid at this point */

            if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UsePRM)
            {
                EDDS_IS_VALID_PTR(pDDB); /* pDDB must be valid at this point */

                /* only one channel supported per device */
                if ( pDDB->pGlob->Prm.HandleCnt == 0 )
                {
                    /* check if component is initialized */
                    if  ( !LSA_HOST_PTR_ARE_EQUAL(pDetail->pPRM, LSA_NULL) )
                    {
                        /* NOTE: pHDB must be valid here! */
                        EDDS_IS_VALID_PTR(pHDB);

                        /* IMPORTANT: only one PRM channel can be opened, */
                        /*            so it is OK to save the pHDB here   */

                        pHDB->UsedComp |= EDDS_COMP_PRM;
                        pDDB->pGlob->Prm.HandleCnt = 1;
                        pDDB->pGlob->Prm.LowerHandle = (EDD_HANDLE_LOWER_TYPE)pHDB;
                        pDDB->pGlob->Prm.PortData.LesserCheckQuality = (EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY == pDetail->pPRM->PortDataAdjustLesserCheckQuality)?LSA_TRUE:LSA_FALSE;
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: pPRM is NULL!");
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                else
                {
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   NRT Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
            EDDS_IS_VALID_PTR(pDetail); /* pDetail must be valid at this point */

            if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseNRT)
            {
                EDDS_IS_VALID_PTR(pDDB); /* pDDB must be valid at this point */

                /* check if component is initialized */

                if  (( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL)) &&
                     ( !LSA_HOST_PTR_ARE_EQUAL(pDetail->pNRT, LSA_NULL)) )
                {
                    /* NOTE: pHDB must be valid here! */
                    EDDS_IS_VALID_PTR(pHDB);

                    /* open channel for NRT */

                    Status = EDDS_NRTOpenChannel( pHDB,
                                                  pDetail->pNRT);

                    /* if EDD_STS_ERR_CHANNEL_USE EDDS_COMP_NRT must be set as well: because of the clean up later on */
                    if ( Status == EDD_STS_OK || Status == EDD_STS_ERR_CHANNEL_USE)
                    {
                        pHDB->UsedComp |= EDDS_COMP_NRT;
                    }

                }
                else
                {
                    EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: NRT component not initialized yet!");
                    Status = EDD_STS_ERR_SEQUENCE;
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init  cSRT Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
            EDDS_IS_VALID_PTR(pDDB); /* pDDB must be valid at this point */
            EDDS_IS_VALID_PTR(pDetail); /* pDetail must be valid at this point */

            if(EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseCSRT )
            {
                if( pDDB->pGlob->IsSrtUsed )
                {
                    if( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) )
                    {
                        /* NOTE: pHDB must be valid here! */
                        EDDS_IS_VALID_PTR(pHDB);

                        /* open channel for cSRT */

                        Status = EDDS_RTOpenChannelCyclic( pHDB );

                        if ( Status == EDD_STS_OK )
                        {
                            pHDB->UsedComp |= EDDS_COMP_CSRT;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: SRT component not initialized yet!");
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* On error we have to undo several things..                             */
        /* (without statuscheck, because we cant do anything .)                  */
        /*-----------------------------------------------------------------------*/

        if ( Status != EDD_STS_OK )
        {
            if(SysPathInit)
            {
                /* error occured, path info must be released */
                do_release_path_info = LSA_TRUE;

                /* HDB was previously acquired */
                if ( HandleInit )
                {
                    /* NOTE: pHDB must be valid here! */
                    EDDS_IS_VALID_PTR(pHDB);

                    /* close sub channels ... */

                    if ( pHDB->UsedComp & EDDS_COMP_PRM )
                    {
                        EDDS_IS_VALID_PTR(pDDB); /* pDDB must be valid at this point, UsedComp is set within edds_open_channel only when we have a valid pDDB */

                        pDDB->pGlob->Prm.HandleCnt = 0;
                        pHDB->UsedComp &= ~EDDS_COMP_PRM;
                    }

                    if ( pHDB->UsedComp & EDDS_COMP_NRT )
                    {
                        EDDS_NRTCloseChannel(pHDB);  //lint !e534 JB 12/11/2014 see code comment above
                        pHDB->UsedComp &= ~EDDS_COMP_NRT;
                    }

                    if ( pHDB->UsedComp & EDDS_COMP_CSRT )
                    {
                        EDDS_IS_VALID_PTR(pDDB); /* pDDB must be valid at this point, UsedComp is set within edds_open_channel only when we have a valid pDDB */

                        if ( pDDB->pGlob->IsSrtUsed )
                        {
                            EDDS_RTCloseChannelCyclic(pHDB); //lint !e534 JB 12/11/2014 see code comment above
                            pHDB->UsedComp &= ~EDDS_COMP_CSRT;
                        }
                    }

                    /* release HDB */
                    EDDS_HandleRelease(pHDB); //lint !e534 JB 12/11/2014 see code comment above
                }
            }
        }

        pRQBChannel->HandleLower = (EDD_HANDLE_LOWER_TYPE)pHDB;
        EDD_RQB_SET_HANDLE(pRQB, pRQBChannel->HandleUpper);
        EDDS_RQB_SET_STATUS(pRQB, Status);

        EDDS_CallChannelCbf(pRQBChannel->Cbf,pRQB,pSys);

        /* GET_PATH_INFO was successful, but another error occured, path info must be released */
        if(do_release_path_info) //lint !e731 JB 12/11/2014 on purpose
        {
            EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_RELEASE_PATH_INFO.");

            EDDS_RELEASE_PATH_INFO(&RelStat,pSys,pDetail);

            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_RELEASE_PATH_INFO (Rsp: 0x%X).",RelStat);
        }

    }

    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<<< Request: EDD_OPC_OPEN_CHANNEL (Status: 0x%X)",Status);

    EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_open_channel()");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_close_channel                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_CLOSE_CHANNEL           +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     LSA_NULL                        +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_CLOSE_CHANNEL           +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an channel.                                        +*/
/*+                                                                         +*/
/*+               Calls every components Close-Channel function. Every      +*/
/*+               component checks if it is used for this handle and if     +*/
/*+               so it trys to close the channel. if it fails the component+*/
/*+               returns an error-status.                                  +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_close_channel(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                          Status;
    LSA_UINT16                          Status2;
    EDDS_LOCAL_HDB_PTR_TYPE             pHDB;
    LSA_SYS_PTR_TYPE                    pSys;
    EDDS_DETAIL_PTR_TYPE                pDetail;
    LSA_HANDLE_TYPE                     UpperHandle;
    LSA_VOID                            LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE);
    EDD_UPPER_RQB_PTR_TYPE              pRQBDummy;
    LSA_UINT32                          i;
    LSA_BOOL                            do_release_path_info = LSA_FALSE;

    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_close_channel(pRQB: 0x%X)",
                           pRQB);

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        // expect valid pointer
        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR, "edds_close_channel(): ERROR -> pRQB is NULL!");
        EDDS_RQB_ERROR(pRQB);
        return;
    }

    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_CLOSE_CHANNEL )
    {

        EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,">>> edds_close_channel: Invalid RQB-Opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        //Status = EDD_STS_ERR_OPCODE;
        EDDS_RQB_ERROR(pRQB);
        return;
    }
    else
    {
        EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_OPC_CLOSE_CHANNEL (HandleLower: 0x%X)",EDD_RQB_GET_HANDLE_LOWER(pRQB));

        Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);

        /* does handle exceeds EDDS_CFG_MAX_CHANNELS */
        if ( Status != EDD_STS_OK )
        {

            EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_close_channel: Invalid Handle in RQB!");

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
        /* channel in use? */
        else if ( !pHDB->InUse )
        {
            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_close_channel: Handle not in use (pHDB: 0x%X)", pHDB);

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
    }


    if (Status == EDD_STS_OK)
    {
        UpperHandle  = pHDB->UpperHandle;   /* save this because we will free HDB*/
        Cbf  = pHDB->Cbf;   /* save this because we will free the HDB later on   */

        /*-----------------------------------------------------------------------*/
        /* Close Channel for NRT                                                 */
        /*-----------------------------------------------------------------------*/

        //if (Status == EDD_STS_OK)  //no need to check this again
        {

            if ( pHDB->UsedComp & EDDS_COMP_NRT )
            {
                /* because the close-sequence may be interrupted by a higher */
                /* prio recv/send-request we set this channel to inactive    */
                /* before closing, so send/recv-requests will be regarded and*/
                /* dont conflict with an close in progress. if the close     */
                /* fails we set the channel back to active. note that the    */
                /* user should not call recv/send - requests while closing   */
                /* so this is only for secure.                               */

                pHDB->UsedComp &= ~EDDS_COMP_NRT;
                Status = EDDS_NRTCloseChannel(pHDB);
                if  (Status != EDD_STS_OK ) pHDB->UsedComp |= EDDS_COMP_NRT;
            }
        }


        /*-----------------------------------------------------------------------*/
        /* Close Channel for cSRT                                                */
        /*-----------------------------------------------------------------------*/

        if( pHDB->pDDB->pGlob->IsSrtUsed )
        {
            if( Status == EDD_STS_OK)
            {
                if ( pHDB->UsedComp & EDDS_COMP_CSRT )
                {
                    /* we dont need to set the channel to inactive temporary, because*/
                    /* we have no higher-prio user request which may intercept this..*/

                    Status = EDDS_RTCloseChannelCyclic(pHDB);
                    if  (Status == EDD_STS_OK ) pHDB->UsedComp &= ~EDDS_COMP_CSRT;
                }

            }
        }

        /*-----------------------------------------------------------------------*/
        /* Release all Link-Indication RQBs with CANCEL.                         */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            /* Because this sequence runs in RQB context and must not */
            /* be interrupted by scheduler we have to use EDDS_ENTER */

            EDDS_ENTER(pHDB->pDDB->hSysDev);

            for ( i=0; i<=pHDB->pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
                do
                {
                    EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[i].pBottom,
                                              pHDB->LinkIndReq[i].pTop,
                                              pRQBDummy);

                    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
                    {
                        EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
                    }
                }
                while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL));
            }

            EDDS_EXIT(pHDB->pDDB->hSysDev);
        }

        /*-----------------------------------------------------------------------*/
        /* Release all Prm-Indication RQBs with CANCEL.                          */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            if ( pHDB->UsedComp & EDDS_COMP_PRM )
            {
                for ( i=0; i<=pHDB->pDDB->pGlob->HWParams.Caps.PortCnt; i++)
                {
                    do
                    {
                        EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->pDDB->pGlob->Prm.PrmIndReq[i].pBottom,
                                                  pHDB->pDDB->pGlob->Prm.PrmIndReq[i].pTop,
                                                  pRQBDummy);

                        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
                        {
                            EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
                        }
                    }
                    while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL));
                }

                pHDB->UsedComp &= ~EDDS_COMP_PRM;
                pHDB->pDDB->pGlob->Prm.HandleCnt = 0;
            }
        }

        pSys         = pHDB->pSys;          /* save this because we will free the HDB*/
        pDetail      = pHDB->pDetail;       /* save this because we will free the HDB*/

        /*-----------------------------------------------------------------------*/
        /* If all channels successfully closed, we release the handle and call   */
        /* the LSA release path info-function.                                   */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            Status = EDDS_HandleRelease(pHDB); /* release HDB, don't use pHDB anymore! */

            if (Status == EDD_STS_OK)
            {
                /* Call LSA-Release Path Info. This is done only on success */
                /* NOTE: If this fails we have already closed all things and*/
                /*       cant do anything so we ignore this error or fatal? */
                do_release_path_info = LSA_TRUE;
            }

        }

        /*-----------------------------------------------------------------------*/
        /* Finish the request. Note: dont use EDDS_RequestFinish() here, because */
        /* pHDB may be released and invalid!!                                    */
        /*-----------------------------------------------------------------------*/

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDD_RQB_SET_HANDLE(pRQB, UpperHandle );

        EDDS_CallChannelCbf(Cbf,pRQB,pSys);
        if(do_release_path_info)  //lint !e731 JB 12/11/2014 on purpose
        {
            EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_RELEASE_PATH_INFO.");

            EDDS_RELEASE_PATH_INFO(&Status2,pSys,pDetail);

            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_RELEASE_PATH_INFO (Rsp: 0x%X).",Status2);

            if (Status2 != LSA_RET_OK )
                EDDS_FatalError( EDDS_FATAL_ERR_RELEASE_PATH_INFO,
                                 EDDS_MODULE_ID,
                                 __LINE__);
        }
    }

    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<<< Request: EDD_OPC_CLOSE_CHANNEL (Status: 0x%X)",Status);

    EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_close_channel()");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_Request                                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_NRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SRT_xxx - Services                     +*/
/*+                         EDDS_SRV_IRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SWI_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given back on open_channel-request.        +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//check pRQB/pHQB
static LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_Request(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                  Status;
    EDD_SERVICE                 Service;
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB;
    LSA_UINT32                  TraceIdx;

    /* TraceIdx cannot be accessed before being validated */
    TraceIdx = EDDS_UNDEF_TRACE_IDX;

    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_Request(pRQB: 0x%X)",
                           pRQB);

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB); /* get handle */

    /* does handle exceeds EDDS_CFG_MAX_CHANNELS */
    if ( Status != EDD_STS_OK )
    {
        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid handle in RQB!");

        Status = EDD_STS_ERR_PARAM;
        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_RQB_ERROR(pRQB);
    }
    /* channel in use? */
    else if ( !pHDB->InUse )
    {
        EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"Channel Handle not in use (pHDB: 0x%X)", pHDB);

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        Status = EDD_STS_ERR_PARAM;
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        TraceIdx = pHDB->pDDB->pGlob->TraceIdx;

        Service = EDD_RQB_GET_SERVICE(pRQB);

        Status  = EDD_STS_OK;

        switch (Service & EDD_SRV_TYPE_MASK)
        {
            /*------------------------------------------------------------*/
            /* NRT-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_NRT_TYPE:
            case EDD_SRV_NRT_FILTER_TYPE:
            {
                /* check if channel configured for this services */
                if ( pHDB->UsedComp & EDDS_COMP_NRT )
                {
                    EDDS_NRTRequest(pRQB,pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
            }
            break;

            /*------------------------------------------------------------*/
            /* SRT-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_SRT_TYPE:
            {
                if( pHDB->pDDB->pGlob->IsSrtUsed )
                {
                    /* check if channel configured for this services */
                    if(  pHDB->UsedComp & EDDS_COMP_CSRT )
                    {
                        EDDS_RTRequestCyclic(pRQB,pHDB);
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                        Status = EDD_STS_ERR_CHANNEL_USE;
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid SRT-Request, no consumers/providers configured!");
                    Status = EDD_STS_ERR_SERVICE;
                }
            }
            break;

            /*------------------------------------------------------------*/
            /* SWI-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_SWI_TYPE:
            {
                EDDS_SwitchRequest(pRQB,pHDB);
            }
            break;

            /*------------------------------------------------------------*/
            /* General requests                                           */
            /*------------------------------------------------------------*/
            case EDD_SRV_GENERAL_TYPE:
            {
                EDDS_GeneralRequest(pRQB,pHDB);
            }
            break;

            /*------------------------------------------------------------*/
            /* PRM-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_PRM_TYPE:
            {
                /* check if channel configured for this services */
                if ( pHDB->UsedComp & EDDS_COMP_PRM )
                {
                    EDDS_PrmRequest(pRQB,pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
            }
            break;

            /*------------------------------------------------------------*/
            /* Debug Requests                                             */
            /*------------------------------------------------------------*/
            case EDDS_SRV_DEBUG_TYPE:
            {
                EDDS_DebugRequest(pRQB,pHDB);
            }
            break;

            /*------------------------------------------------------------*/
            /* Unknown/unsupported Service                                */
            /*------------------------------------------------------------*/
            default:
            {
                EDDS_UPPER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid service (0x%X)",Service);
                Status = EDD_STS_ERR_SERVICE;
            }
            break;
        }

        if ( Status != EDD_STS_OK )
        {
            EDDS_RequestFinish(pHDB,pRQB,Status);
        }

    }

    EDDS_UPPER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_Request(RQB-Status: 0x%X)",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_request                                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+                                         EDD_OPC_REQUEST_SRT_BUFFER      +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_NRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SRT_xxx - Services                     +*/
/*+                         EDDS_SRV_IRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SWI_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST                 +*/
/*+                                         EDD_OPC_REQUEST_SRT_BUFFER      +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+                                         :                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given back on open_channel-request.        +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_request(EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_request(pRQB: 0x%X)",
                           pRQB);

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        // expect valid pointer
        EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR, "edds_request(): ERROR -> pRQB is NULL!");
        EDDS_RQB_ERROR(pRQB);
        return;
    }

    switch ( EDD_RQB_GET_OPCODE(pRQB))
    {
        case EDD_OPC_REQUEST_SRT_BUFFER:
            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_request: Unsupported opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));
            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_NOT_SUPPORTED);
            EDDS_RQB_ERROR(pRQB);
            break;

        case EDD_OPC_REQUEST:
            EDDS_Request(pRQB);
            break;
        default:
            EDDS_UPPER_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_UNEXP,"edds_request: Invalid opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
            EDDS_RQB_ERROR(pRQB);
            break;

    } /* switch */


    EDDS_UPPER_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_request()");
}


/*****************************************************************************/
/*  end of file EDDS_USR.C                                                   */
/*****************************************************************************/
