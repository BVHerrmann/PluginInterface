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
/*  F i l e               &F: eddi_prm_req.c                            :F&  */
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
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_prm_state_diag_pdnc.h"
#include "eddi_prm_req.h"
#include "eddi_trac_ext.h"
#include "eddi_prm_record_common.h"
#include "eddi_ext.h"
#include "eddi_prm_state.h"
#if defined (EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)
#include "eddi_Tra.h"
#endif

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_REQ
#define LTRC_ACT_MODUL_ID  400

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenPrmIndProvide(EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                              EDDI_LOCAL_HDB_PTR_TYPE const pHDB,
                                                              LSA_BOOL                    * bIndicate);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_FireIndicationRessourceLack(EDDI_PRM_PDNC_DATA_CHECK  * const pPDNC,       //!! not pointing to const Data
                                                                         EDDI_PRM_PDSYNC_DATA      * const pPDSync,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenPrmRead(EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE const pHDB,
                                                        LSA_BOOL                    * bIndicate);

static LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_FirePortIndication(EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                LSA_UINT16              const UsrPortID);

static LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_FireInterfaceIndication(EDDI_LOCAL_DDB_PTR_TYPE const pDDB);


static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDNCDataCheck(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDSCFDataCheck(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDControlPLL(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDTraceUnitControl(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDTraceUnitData(EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                    LSA_BOOL                    * bIndicate);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDIRData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                             EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

#if defined (EDDI_CFG_DFP_ON)
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDIRSubframeData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);
#endif

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDSyncData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                               EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDPortDataAdjust(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDNRTFeedInLoadLimitation(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                              EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmTraceRequest(EDDI_LOCAL_DDB_PTR_TYPE const pDDB, 
                                                            EDD_SERVICE             const Service);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDPortStatistic(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                    EDDI_LOCAL_HDB_PTR_TYPE     const pHDB,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDirApplicationData( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                          EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB );

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmHandleReadPDIRSubframeData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE     const pDDB);

/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmRequest()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_RESULT                      Status;
    LSA_BOOL                        bIndicate;
    EDD_SERVICE                     Service;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmRequest");

    bIndicate = LSA_TRUE;
    Status    = EDD_STS_OK;

    Service = EDD_RQB_GET_SERVICE(pRQB);

    if ( (Service != EDD_SRV_PRM_WRITE)       &&
         (Service != EDD_SRV_PRM_PREPARE)     &&
         (Service != EDD_SRV_PRM_END)         &&
         (Service != EDD_SRV_PRM_COMMIT)      &&
         (Service != EDD_SRV_PRM_READ)        &&
         (Service != EDD_SRV_PRM_CHANGE_PORT) &&
         (Service != EDD_SRV_PRM_INDICATION) )
    {
        Status = EDD_STS_ERR_SERVICE;
    }
    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    else if (pDDB->PRM.Transition.bTransitionRunning)
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmRequest, RQB must be queued due to Running COMMIT-Transition");

        pRQB->internal_context = pHDB; /* remember for RX handling ! */

        //Queue RQB for later invocation
        EDDI_AddToQueueEnd(pDDB, &pDDB->PRM.Transition.RqbQueue, pRQB);
        bIndicate = LSA_FALSE;
    }
    #endif
    else
    {
        EDDI_PrmTraceRequest(pDDB, Service);

        switch (Service)
        {
            case EDD_SRV_PRM_WRITE:
            {
                EDD_UPPER_PRM_WRITE_PTR_TYPE pPrmWrite;

                EDDI_PRM_INIT_DETAIL_ERR(&pDDB->PRM.PrmDetailErr);

                pPrmWrite = (EDD_UPPER_PRM_WRITE_PTR_TYPE)pRQB->pParam;
//                EDD_RQB_PRM_WRITE_SET(pPrmWrite, EDD_PRM_ERR_OFFSET_DEFAULT);

                /* user defined records, independent from the A/B-Buffer */
                if ((pPrmWrite->record_index >= EDDI_RPM_INDEX_RANGE_B_LOW) &&
                    (pPrmWrite->record_index <= EDDI_RPM_INDEX_RANGE_B_HIGH))
                {
                    Status = EDDI_PrmCheckAndCopyRecord(pRQB, pDDB);
                }
                else
                {
                    Status = pDDB->PRM.StateFct(pRQB, pHDB, Service);
                }

                if (LSA_FALSE != pDDB->PRM.PrmDetailErr.ErrValid)
                {
                    EDD_RQB_PRM_WRITE_SET(pPrmWrite, pDDB->PRM.PrmDetailErr.ErrOffset);
                }
                else if (EDD_STS_OK != Status)
                {
                    EDD_RQB_PRM_WRITE_SET(pPrmWrite, EDD_PRM_ERR_OFFSET_DEFAULT);
                }
                break;
            }

            case EDD_SRV_PRM_PREPARE:
            {
                EDDI_MEMSET(&pDDB->PRM.PrmDetailErr, 0, sizeof(EDDI_PRM_DETAIL_ERR));
                Status = pDDB->PRM.StateFct(pRQB, pHDB, Service);
                break;
            }

            case EDD_SRV_PRM_CHANGE_PORT:
            {
                EDDI_PRM_INIT_DETAIL_ERR(&pDDB->PRM.PrmDetailErr);
                Status = pDDB->PRM.StateFct(pRQB, pHDB, Service);
                break;
            }

            case EDD_SRV_PRM_END:
            {
                LSA_UINT32                  HwPortIndex;

                EDDI_PRM_INIT_DETAIL_ERR(&pDDB->PRM.PrmDetailErr);
                EDDI_MEMSET(pDDB->PRM.PortparamsNotApplicable, EDD_PRM_PORT_PARAMS_APPLICABLE, sizeof(pDDB->PRM.PortparamsNotApplicable));

                if (EDDI_PRMChangePortState_IsAnySequenceRunning(pDDB, &HwPortIndex))
                {
                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmRequest,: ERROR -> EDD_SRV_PRM_END during a ChangePort Sequence at HwPort %d", HwPortIndex);
                    Status = EDD_STS_ERR_SEQUENCE;
                    EDDI_SET_DETAIL_ERR(pDDB, Status);
                }
                else
                {
                    Status = pDDB->PRM.StateFct(pRQB, pHDB, Service);

                    if (LSA_FALSE != pDDB->PRM.PrmDetailErr.ErrValid)
                    {
                        EDDI_RQB_PRM_END_SET(pRQB->pParam,  pDDB->PRM.PrmDetailErr.ErrPortID,
                                                            pDDB->PRM.PrmDetailErr.ErrIndex,
                                                            pDDB->PRM.PrmDetailErr.ErrOffset,
                                                            pDDB->PRM.PrmDetailErr.ErrFault);
                    }
                    else if (EDD_STS_OK != Status)
                    {
                        EDDI_RQB_PRM_END_SET(pRQB->pParam,  EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                            EDD_PRM_ERR_INDEX_DEFAULT,
                                                            EDD_PRM_ERR_OFFSET_DEFAULT,
                                                            EDD_PRM_ERR_FAULT_DEFAULT);
                    }
                    else
                    {
                        LSA_UINT32  j;
                        for (j = 0; j < EDD_CFG_MAX_PORT_CNT; ++j)
                        {
                            ((EDD_UPPER_PRM_END_PTR_TYPE)(pRQB->pParam))->PortparamsNotApplicable[j] = pDDB->PRM.PortparamsNotApplicable[j];
                        }
                    }
                }
                break;
            }

            case EDD_SRV_PRM_COMMIT:
            {
                EDD_UPPER_PRM_COMMIT_PTR_TYPE  const pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE)pRQB->pParam;

                pDDB->PRM.bPendingCommit = LSA_TRUE;
                Status = pDDB->PRM.StateFct(pRQB, pHDB, Service);

                #if defined (EDDI_RED_PHASE_SHIFT_ON)
                if (Status == EDD_STS_OK_PENDING)
                {
                    // Prepare for asynchronous finishing of COMMIT-Request
                    // Indication will be done later

                    bIndicate = LSA_FALSE;

                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmRequest, COMMIT will be finished asynchronously");
                    return;
                }
                else if (Status == EDD_STS_OK)
                {
                    // OK no transition active return at once
                }
                else
                {
                    // An error occurred, return at once
                }

                #endif

                EDDI_GenCalcTransferEndValues(pDDB, 
                                              &pPrmCommit->RsvIntervalRed, 
                                              pDDB->PRM.PDIRData.pRecordSet_A, 
                                              pDDB->PRM.PDirApplicationData.pRecordSet_A, 
                                              pDDB->PRM.PDIRData.State_A, 
                                              pDDB->PRM.PDirApplicationData.State_A);  

                // Commit confirmation
                EDDI_RequestFinish(pHDB, pRQB, Status);

                // pending RQB during Commit
                if ((pDDB->PRM.bPendingCommit) &&
                    (pDDB->PRM.pRQB_PendingCommit != EDDI_NULL_PTR))
                {
                    EDDI_RequestFinish(pHDB, pDDB->PRM.pRQB_PendingCommit, EDD_STS_OK);

                    pDDB->PRM.bPendingCommit     = LSA_FALSE;
                    pDDB->PRM.pRQB_PendingCommit = EDDI_NULL_PTR;
                }
                // empty PRM-Indication if no error occured during Commit
                else if (EDD_STS_OK == Status)
                {
                    EDDI_FireInterfaceIndication(pDDB);

                    pDDB->PRM.bPendingCommit = LSA_FALSE;
                }
                // Commit returned with failure --> no PRM-Indication
                else
                {
                    return;
                }

                {
                    LSA_UINT16         UsrPortID;
                    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

                    // FirePortIndication for all Ports
                    for (UsrPortID = 1; UsrPortID <= PortMapCnt; UsrPortID++)
                    {
                        EDDI_FirePortIndication(pDDB, UsrPortID);
                    }
                }
                return;
            }

            case EDD_SRV_PRM_READ:
            {
                Status = EDDI_GenPrmRead(pRQB, pHDB, &bIndicate);
                break;
            }

            case EDD_SRV_PRM_INDICATION:
            {
                Status = EDDI_GenPrmIndProvide(pRQB, pHDB, &bIndicate);
                break;
            }

            default:
            {
                Status = EDD_STS_ERR_SERVICE;
                break;
            }
        }
    }

    if (bIndicate)
    {
        EDDI_RequestFinish(pHDB, pRQB, Status);
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmOpenChannel( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_PRM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmOpenChannel");

    return EDDI_TRACEOpenChannel(pHDB->pDDB, pHDB);
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCloseChannel( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    LSA_UINT32                      UsrPortIndex;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCloseChannel");

    if (    (pDDB->PRM.PDSyncData.State_A        == EDDI_PRM_VALID)
         || (pDDB->PRM.PDIRData.State_A          == EDDI_PRM_VALID)
         #if defined (EDDI_CFG_DFP_ON)
         || (pDDB->PRM.PDIRSubFrameData.State_A  == EDDI_PRM_VALID)
         #endif
         )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDSyncData_PDIRData_Valid);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCloseChannel; -> PDSyncData.State_A, PDIRData.State_A :0x%X :0x%X",
                          pDDB->PRM.PDSyncData.State_A, pDDB->PRM.PDIRData.State_A);
        return EDD_STS_ERR_SEQUENCE;
    }

    for (;;)
    {
        pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Interface);

        if (pRQB == EDDI_NULL_PTR)
        {
            break;
        }

        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCloseChannel, Freeing PRM ind resource (IF) 0x%X", pRQB);
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK_CANCEL);
    }

    for (UsrPortIndex = 1; UsrPortIndex <= pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        for (;;)
        {
            pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Ports[UsrPortIndex-1]);

            if (!pRQB)
            {
                break;
            }

            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmCloseChannel, Freeing PRM ind resource (Port %d) 0x%X", UsrPortIndex, pRQB);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK_CANCEL);
        }
    }

    return EDDI_TRACECloseChannel(pHDB->pDDB);
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenPrmIndProvide( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                                LSA_BOOL                     *  bIndicate )
{
    LSA_RESULT                                Status;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE         pPrmInd;
    EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB = pHDB->pDDB;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenPrmIndProvide");

    Status = EDD_STS_ERR_PARAM;

    pPrmInd = (EDD_UPPER_PRM_INDICATION_PTR_TYPE)pRQB->pParam;

    if (pPrmInd->edd_port_id > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_PRM_Indication);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmIndProvide > pPrmInd->edd_port_id ->:0x%X", pPrmInd->edd_port_id);
        return EDD_STS_ERR_PARAM;
    }

    if (0 == pPrmInd->edd_port_id)
    {
        *bIndicate = LSA_FALSE;
        pPrmInd->diag_cnt    = 0;
        EDDI_AddToQueueEnd(pDDB, &pDDB->PRM.IndQueue_Interface, pRQB);

        if (pDDB->PRM.DoFireIndication_Interface)
        {
            Status = EDDI_FireIndicationRessourceLack(&pDDB->PRM.PDNCDataCheck, &pDDB->PRM.PDSyncData, pDDB);

            pDDB->PRM.DoFireIndication_Interface = LSA_FALSE;
            pDDB->PRM.PDSyncData.bPDSyncChanged  = LSA_FALSE;
            pDDB->PRM.PDNCDataCheck.bPDNCChanged = LSA_FALSE;
        }
    }
    else
    {
        *bIndicate = LSA_FALSE;
        EDDI_AddToQueueEnd(pDDB, &pDDB->PRM.IndQueue_Ports[pPrmInd->edd_port_id - 1], pRQB);

        if (pDDB->PRM.DoFireIndication_Ports[pPrmInd->edd_port_id - 1])
        {
            EDDI_FirePortIndication(pDDB, pPrmInd->edd_port_id);

            pDDB->PRM.DoFireIndication_Ports[pPrmInd->edd_port_id - 1] = LSA_FALSE;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_FireIndicationRessourceLack( EDDI_PRM_PDNC_DATA_CHECK  *  const  pPDNC,
                                                                           EDDI_PRM_PDSYNC_DATA      *  const  pPDSync,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDD_UPPER_RQB_PTR_TYPE             pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE  pPrmInd;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_FireIndicationRessourceLack");

    pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Interface);

    if (pRQB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDIFirePrmIndNoRessource!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    pDDB->PRM.pRQB_PendingRessource  = pRQB;
    pDDB->PRM.bPendingRessource      = LSA_TRUE;

    pPrmInd = (EDD_UPPER_PRM_INDICATION_PTR_TYPE)pRQB->pParam;

    pPrmInd->diag_cnt    = 0;
    pPrmInd->edd_port_id = 0;

    /* empty indication if PDNC and PDSync state machine continue both in state START */
    if ( (pPDNC->Last_Event_Appears      == EDDI_PRM_EV_START ) &&
         (pPDNC->Last_Event_Disappears   == EDDI_PRM_EV_START ) )
    {
        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
    }

    /* no indication if PDNC and PDSync state machine continue both in the same state */
    else if ( ( pPDNC->Last_Event_Appears   == pPDNC->Last_Event_Disappears ) )
    {
        EDDI_AddToQueueEnd(pDDB, &pDDB->PRM.IndQueue_Interface, pRQB);
    }

    else
    {
        if (EDDI_PRM_NOT_VALID != pPDNC->State_A)
        {
            EDDI_PrmSendIndicationPDNC(pPDNC, pPDNC->Last_Event_Appears, pPDNC->Last_Event_Disappears, pDDB);
        }
        if (EDDI_PRM_NOT_VALID != pPDSync->State_A)
        {
            //eddi_PrmSendIndication_PDSync(pPDSync, pPDSync->Last_Event_Appears, pPDSync->Last_Event_Disappears, pDDB);  PK: RTSync
        }

        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
    }

    pDDB->PRM.bPendingRessource      = LSA_FALSE;
    pDDB->PRM.pRQB_PendingRessource  = EDDI_NULL_PTR;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_FireIndicationRessourceLack");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenPrmRead( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                          LSA_BOOL                     *  bIndicate )
{
    EDD_UPPER_PRM_READ_PTR_TYPE         pPrmRead;
    EDDI_LOCAL_DDB_PTR_TYPE       const pDDB   = pHDB->pDDB;
    LSA_RESULT                          Result = EDD_STS_OK;
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    LTRC_LEVEL_TYPE                     Tracelevel;
    LSA_UINT32                          ReadLength;
    #endif

    *bIndicate = LSA_TRUE;

    pPrmRead = (EDD_UPPER_PRM_READ_PTR_TYPE)pRQB->pParam;
    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
    ReadLength = pPrmRead->record_data_length;
    #endif

    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenPrmRead->record_index:0x%X, record_data_length:0x%X, edd_port_id:%d",
                      pPrmRead->record_index, ReadLength, pPrmRead->edd_port_id);

    if (pPrmRead->edd_port_id > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortIDgreaterPortMapCnt_EDDGenPrmRead);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PARAM;
    }

    //Reads from portspecific records are rejected if port submodule is pulled.
    if (pPrmRead->edd_port_id > 0  &&  EDDI_PRMChangePortState_IsPulled(pDDB, EDDI_PmGetHwPortIndex(pDDB, pPrmRead->edd_port_id - 1)))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_WrongIndex_EDDGenPrmRead);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, pPrmRead->edd_port_id:0x%X - Is Pulled", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    switch (pPrmRead->record_index)
    {
        case EDDI_PRM_INDEX_PDNC_DATA_CHECK:     // supports network component function mismatch record 0x8070
        {
            Result = EDDI_PrmReadPDNCDataCheck(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmCheckPDNCData, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDSCF_DATA_CHECK:     // supports send clock factor record 0x10000
        {
            Result = EDDI_PrmReadPDSCFDataCheck(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDSCFDataCheck, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDCONTROL_PLL:     // supports PLL-Ext control record 0xB050
        {
            Result = EDDI_PrmReadPDControlPLL(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDControlPLL, Result:0x%X", Result);
            }

            #if defined (EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)
            EDDI_TRARegDump_NSC(pDDB);
            #endif //(EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)

            break;
        }

        case EDDI_PRM_INDEX_PDTRACEUNIT_CONTROL:     // supports PDTraceUnitControl record 0xB060
        {
            Result = EDDI_PrmReadPDTraceUnitControl(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDTraceUnitControl, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDTRACE_UNIT_DATA:  // supports 0xB061
        {
            Result = EDDI_PrmReadPDTraceUnitData(pRQB, pDDB, bIndicate);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDTraceUnitData, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDIR_DATA:     // supports PDIRData record 0x802C
        {
            Result = EDDI_PrmReadPDIRData(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDIRData, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA:     // supports PDIRSubframeData record 0x8020
        {
            Result=EDDI_PrmHandleReadPDIRSubframeData(pPrmRead,pDDB);
            break;
        }
        
        case EDDI_PRM_INDEX_PDSYNC_DATA:   // supports PDSyncData (PTCPoverRTC) record 0x802D
        {
            Result = EDDI_PrmReadPDSyncData(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDSyncData, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDPORT_DATA_ADJUST:
        {
            Result = EDDI_PrmReadPDPortDataAdjust(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDPortDataAdjust, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDPORT_STATISTIC:
        {
            Result = EDDI_PrmReadPDPortStatistic(pPrmRead, pHDB, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDPortStatistic, Result:0x%X", Result);
            }
            break;
        }

        case EDDI_PRM_INDEX_PDIR_APPLICATION_DATA:
        {
            Result = EDDI_PrmReadPDirApplicationData(pPrmRead, pHDB, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDirApplicationData, Result:0x%X", Result);
            }
            break;
        }
        
        case EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION: //supports PDNRTFeedInLoadLimitation record 0x10003
        {
            Result = EDDI_PrmReadPDNRTFeedInLoadLimitation(pPrmRead, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION, Result:0x%X", Result);
                return Result;
            }
            break;
        }


        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_WrongIndex_EDDGenPrmRead);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, pPrmRead->record_index:0x%X", pPrmRead->record_index);
            return EDD_STS_ERR_PRM_INDEX;
        }
    }

    #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
	Tracelevel = (EDD_STS_OK == Result)?LSA_TRACE_LEVEL_NOTE_LOW:LSA_TRACE_LEVEL_NOTE_HIGH;
    LSA_UNUSED_ARG(Tracelevel);
    LSA_UNUSED_ARG(ReadLength);
    #endif
    EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, Tracelevel, "EDDI_GenPrmRead<-record_index:0x%X record_data_length IN/OUT:0x%X/0x%X edd_port_id:%d",
                      pPrmRead->record_index, ReadLength, pPrmRead->record_data_length, pPrmRead->edd_port_id);

    return Result;
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
#if defined (EDDI_CFG_DFP_ON)
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmHandleReadPDIRSubframeData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE     const pDDB)
{
	LSA_RESULT	Result = EDD_STS_OK;

	Result = EDDI_PrmReadPDIRSubframeData(pPrmRead, pDDB);
	if (EDD_STS_OK != Result)
	{
		EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenPrmRead, EDDI_PrmReadPDIRSubframeData, Result:0x%X", Result);
	}

	return Result;

}
#else
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmHandleReadPDIRSubframeData(EDD_UPPER_PRM_READ_PTR_TYPE const pPrmRead,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE     const pDDB)
{
	LSA_UNUSED_ARG(pPrmRead);
	LSA_UNUSED_ARG(pDDB);

	EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_WrongIndex_EDDGenPrmRead);
	EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmHandleReadPDIRSubframeData, pPrmRead->record_index:0x%X not supported", pPrmRead->record_index);
	
	return EDD_STS_ERR_PRM_INDEX;
}
#endif
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FirePortIndication( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT16               const  UsrPortID )
{
    EDD_UPPER_RQB_PTR_TYPE             pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE  pPrmInd;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_FirePortIndication");

    pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Ports[UsrPortID - 1]);

    if (pRQB == EDDI_NULL_PTR)
    {
        pDDB->PRM.DoFireIndication_Ports[UsrPortID - 1] = LSA_TRUE;
        return;
    }

    pPrmInd = (EDD_UPPER_PRM_INDICATION_PTR_TYPE)pRQB->pParam;

    pPrmInd->diag_cnt    = 0;
    pPrmInd->edd_port_id = UsrPortID;

    EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FireInterfaceIndication( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_RQB_PTR_TYPE             pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE  pPrmInd;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_FireInterfaceIndication");

    pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Interface);

    if (pRQB == EDDI_NULL_PTR)
    {
        pDDB->PRM.DoFireIndication_Interface  = LSA_TRUE;
        return;
    }

    pPrmInd = (EDD_UPPER_PRM_INDICATION_PTR_TYPE)pRQB->pParam;

    pPrmInd->diag_cnt    = 0;
    pPrmInd->edd_port_id = 0;

    EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDNCDataCheck( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDNCDataCheck->");

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDNCDataCheck);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDNCDataCheck, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < EDDI_PRM_LENGTH_PDNC_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDNCDataCheck);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDNCDataCheck, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDNCDataCheck.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDNCDataCheck!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDNCDataCheck.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    EDDI_MemCopy(pPrmRead->record_data, pDDB->PRM.PDNCDataCheck.pA, (LSA_UINT32) sizeof(EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE));

    pPrmRead->record_data_length = EDDI_PRM_LENGTH_PDNC_DATA_CHECK;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDNCDataCheck<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDSCFDataCheck( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_PDSCF_DATA_CHECK  * const pPDSCF = &pDDB->PRM.PDSCFDataCheck;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDSCFDataCheck->");
        
    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDSCFDataCheck);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDSCFDataCheck, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    switch (pDDB->PRM.PDSCFDataCheck.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDSCFDataCheck!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDSCFDataCheck.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    if (pPrmRead->record_data_length < EDDI_PRM_LENGTH_PDSCF_DATA_MIN_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDSCFDataCheck);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDSCFDataCheck, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_MemCopy(pPrmRead->record_data, pDDB->PRM.PDSCFDataCheck.pA, pPDSCF->RecordSize );

    pPrmRead->record_data_length = pPDSCF->RecordSize;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDSCFDataCheck<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDControlPLL( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDControlPLL->");

    if (!pDDB->PRM.PDControlPLL.PDControlPllActive)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode_ReadPDControlPll);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDControlPLL, pPrmWrite->PDControlPllActive:0x%X", pDDB->PRM.PDControlPLL.PDControlPllActive);
        return EDD_STS_ERR_PRM_INDEX;
    }

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDControlPll);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDControlPLL, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < EDDI_PRM_LENGTH_PD_CONTROL_PLL)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDControlPll);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDControlPLL, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDControlPLL.State_A)
    {
        case EDDI_PRM_WRITE_DONE:
        case EDDI_PRM_NOT_VALID:
        {
        } break;

        case EDDI_PRM_VALID:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDControlPLL!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDControlPLL.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    EDDI_MemCopy(pPrmRead->record_data,
                 &pDDB->PRM.PDControlPLL.RecordSet_A,
                 (LSA_UINT32)sizeof(EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE));

    pPrmRead->record_data_length = EDDI_PRM_LENGTH_PD_CONTROL_PLL;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDControlPLL<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDTraceUnitControl( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDTraceUnitControl->");

    if (0 == pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_ReadPDTraceUnitControl);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDTraceUnitControl, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDTraceUnitControl);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDTraceUnitControl, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDTraceUnitControl.State_A[pPrmRead->edd_port_id - 1])
    {
        case EDDI_PRM_WRITE_DONE:
        case EDDI_PRM_NOT_VALID:
        {
        } break;

        case EDDI_PRM_VALID:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDTraceUnitControl!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDTraceUnitControl.State_A[pPrmRead->edd_port_id - 1], 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    EDDI_MemCopy(pPrmRead->record_data,
                 &pDDB->PRM.PDTraceUnitControl.RecordSet_A[pPrmRead->edd_port_id - 1],
                 (LSA_UINT32)sizeof(EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE));

    pPrmRead->record_data_length = EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL;

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDTraceUnitData( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      LSA_BOOL                     *  bIndicate )
{
    LSA_RESULT                   Status;
    EDD_UPPER_PRM_READ_PTR_TYPE  pPrmRead;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDTraceUnitData->");

    pPrmRead = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam;

    /* TraceUnit still in use */
    if (EDDI_NULL_PTR != pDDB->PRM.TRACE.Pending.pRQB)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TraceUnitInUse_ReadPDTraceUnitData);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDTraceUnitData, TraceUnit still in use");
        return EDD_STS_ERR_SEQUENCE;
    }

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDTraceUnitData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDTraceUnitData, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < sizeof(EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDTraceUnitData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDTraceUnitData, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    Status = EDDI_TRACEPRMIndicationFromUsr(pRQB, pDDB, bIndicate);

    if (EDD_STS_OK != Status)
    {
        return Status;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDTraceUnitData<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDIRData( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                               EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_RECORD_IRT_PTR_TYPE  pPDIRDataSet;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDIRData->");

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDIRData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDIRData, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < pDDB->PRM.PDIRData.pRecordSet_A->PDIRDataRecordActLen)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDIRData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDIRData, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDIRData.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDIRData!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDIRData.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    pPDIRDataSet = pDDB->PRM.PDIRData.pRecordSet_A;

    //Copy IRGlobalDataBlock to local Buffers
    EDDI_MemCopy(pPrmRead->record_data, pPDIRDataSet->pNetWorkPDIRDataRecord, pDDB->PRM.PDIRData.pRecordSet_A->PDIRDataRecordActLen);
    pPrmRead->record_data_length = pDDB->PRM.PDIRData.pRecordSet_A->PDIRDataRecordActLen;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDIRData<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_DFP_ON)
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDIRSubframeData( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  pPDIRSubframeDataSet;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDIRSubframeData->");

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDIRSubframeData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDIRSubframeData, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < pDDB->PRM.PDIRSubFrameData.pRecordSet_A->PDirSubFrameDataRecordActLen)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDIRDataSubframeData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDIRSubframeData, pPrmRead->record_data_length:0x%X", pPrmRead->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDIRSubFrameData.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDIRSubframeData!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDIRSubFrameData.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    pPDIRSubframeDataSet = pDDB->PRM.PDIRSubFrameData.pRecordSet_A;

    // Copy pNetWorkPDirSubframeDataRecord to local Buffers
    EDDI_MemCopy(pPrmRead->record_data, pPDIRSubframeDataSet->pNetWorkPDirSubframeDataRecord, pDDB->PRM.PDIRSubFrameData.pRecordSet_A->PDirSubFrameDataRecordActLen);
    pPrmRead->record_data_length = pDDB->PRM.PDIRSubFrameData.pRecordSet_A->PDirSubFrameDataRecordActLen;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDIRSubframeData<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDSyncData( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_RECORD_SYNC_PTR_TYPE  pPDSyncDataSet;
    LSA_UINT32                     PDSyncDataCopyLength1, PDSyncDataCopyLength2;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDSyncData->");

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_ReadPDSyncData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDSyncData, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < pDDB->PRM.PDSyncData.pRecordSet_A->PDSyncDataLength_All)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDSyncData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDSyncData, pPrmRead->record_data_length:0x%X :0x%X",
                          pPrmRead->record_data_length, pDDB->PRM.PDSyncData.pRecordSet_A->PDSyncDataLength_All);
        return EDD_STS_ERR_PRM_DATA;
    }

    switch (pDDB->PRM.PDSyncData.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDSyncData!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDSyncData.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    pPDSyncDataSet = pDDB->PRM.PDSyncData.pRecordSet_A;

    //Copy IRGlobalDataBlock to local Buffers
    PDSyncDataCopyLength1 = pDDB->PRM.PDSyncData.pRecordSet_A->PDSyncDataLength_All - pDDB->PRM.PDSyncData.pRecordSet_A->PaddingCount;

    EDDI_MemCopy(pPrmRead->record_data, pPDSyncDataSet->pNetWorkPDSyncDataRecord, PDSyncDataCopyLength1);

    PDSyncDataCopyLength2 = pDDB->PRM.PDSyncData.pRecordSet_A->PaddingCount;

    EDDI_MemCopy(&pPrmRead->record_data[PDSyncDataCopyLength1], &pPDSyncDataSet->pNetWorkPDSyncDataRecord->Padding_1[0], PDSyncDataCopyLength2);

    pPrmRead->record_data_length = pDDB->PRM.PDSyncData.pRecordSet_A->PDSyncDataLength_All;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDSyncData<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDirApplicationData( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                          EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE        PDirApplicationData;
    LSA_UINT32                                 const  PDirApplicationDataLength = sizeof(EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE);
     
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDirApplicationData->");

    if (pPrmRead->edd_port_id > pHDB->pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_ReadPDPortDataAdjust);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDirApplicationData, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < PDirApplicationDataLength)
    {           
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDStatistics);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDirApplicationData, Read PrmRecord PDirApplicationData: record_data_length (%d), PDirApplicationDataLength (%d)", 
                          pPrmRead->record_data_length, PDirApplicationDataLength);
        return EDD_STS_ERR_PRM_DATA;         
    }
   
    switch (pDDB->PRM.PDirApplicationData.State_A)
    {
        case EDDI_PRM_VALID:
            break;

        case EDDI_PRM_NOT_VALID:
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE:
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDirApplicationData!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDirApplicationData.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    PDirApplicationData.BlockHeader.BlockType        = EDDI_HTONS(EDDI_PRM_BLOCKTYPE_PDIR_APPLICATION_DATA);
    PDirApplicationData.BlockHeader.BlockLength      = EDDI_HTONS((LSA_UINT16)PDirApplicationDataLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH); 
    PDirApplicationData.BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
    PDirApplicationData.BlockHeader.BlockVersionLow  = EDDI_PRM_BLOCK_VERSION_LOW;

    PDirApplicationData.ConsumerCnt                  = EDDI_HTONS(pDDB->PRM.PDirApplicationData.pRecordSet_A->ConsumerCnt);
    PDirApplicationData.ConsumerIODataLength         = EDDI_HTONS(pDDB->PRM.PDirApplicationData.pRecordSet_A->ConsumerIODataLength);    
    PDirApplicationData.Reserved                     = 0;

    EDDI_MemCopy(pPrmRead->record_data, &PDirApplicationData, PDirApplicationDataLength);

    pPrmRead->record_data_length = PDirApplicationDataLength;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDirApplicationData <-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDPortStatistic( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                      EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT32                         const  PDPortStatisticLength = sizeof(EDDI_PRM_RECORD_PD_PORT_STATISTIC);
    EDDI_PRM_RECORD_PD_PORT_STATISTIC         PDPortStatistic;
    EDD_GET_STATISTICS_MIB_TYPE               MIB;
    LSA_RESULT                                Result;
    LSA_UINT16                                CounterStatus;
     
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDPortStatistic->");

    if (pPrmRead->edd_port_id > pHDB->pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_ReadPDPortDataAdjust);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDPortStatistic, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmRead->record_data_length < PDPortStatisticLength)
    {           
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDStatistics);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDPortStatistic, Read PrmRecord PDPortStatistic: record_data_length(%d) < EDDI_PRM_PDPORT_STATISTIC_RECORD_LENGTH(%d)", 
                          pPrmRead->record_data_length, PDPortStatisticLength);
        return EDD_STS_ERR_PRM_DATA;         
    }

    //Get Statistic
    MIB.RequestedCounters = EDD_MIB_SUPPORT_INOCTETS | EDD_MIB_SUPPORT_OUTOCTETS | EDD_MIB_SUPPORT_INDISCARDS | EDD_MIB_SUPPORT_OUTDISCARDS | EDD_MIB_SUPPORT_INERRORS | EDD_MIB_SUPPORT_OUTERRORS;
    Result = EDDI_GenCalcStatistics(pHDB, EDD_STATISTICS_GET_VALUES_MIB2, EDD_STATISTICS_DO_NOT_RESET_VALUES, LSA_NULL, &MIB, pPrmRead->edd_port_id, LSA_FALSE);
    
    if (EDD_STS_OK != Result)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, PortID:0x%X", pPrmRead->edd_port_id);
        return Result;
    }
    
    PDPortStatistic.BlockHeader.BlockType        = EDDI_HTONS(EDDI_PRM_BLOCKTYPE_PD_PORT_STATISTIC);
    PDPortStatistic.BlockHeader.BlockLength      = EDDI_HTONS((LSA_UINT16)PDPortStatisticLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH); 
    PDPortStatistic.BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
    PDPortStatistic.BlockHeader.BlockVersionLow  = EDDI_PRM_STATISTIC_BLOCK_VERSION_LOW;

    //"CounterStatus" is coded in negative logic (0=supported)
    CounterStatus = (0 == pPrmRead->edd_port_id)?EDDI_RECORD_SUPPORTED_COUNTERS_IF:EDDI_RECORD_SUPPORTED_COUNTERS_PORT;
    CounterStatus = (~CounterStatus) & EDD_RECORD_SUPPORT_MASK;

    PDPortStatistic.BlockHeader.CounterStatus    = EDDI_HTONS(CounterStatus);
    PDPortStatistic.ifInOctets                   = EDDI_HTONL(MIB.InOctets);
    PDPortStatistic.ifOutOctets                  = EDDI_HTONL(MIB.OutOctets);    
    PDPortStatistic.ifInDiscards                 = EDDI_HTONL(MIB.InDiscards);    
    PDPortStatistic.ifOutDiscards                = EDDI_HTONL(MIB.OutDiscards);      
    PDPortStatistic.ifInErrors                   = EDDI_HTONL(MIB.InErrors);  
    PDPortStatistic.ifOutErrors                  = EDDI_HTONL(MIB.OutErrors);
      
    EDDI_MemCopy(pPrmRead->record_data, &PDPortStatistic, PDPortStatisticLength);

    pPrmRead->record_data_length = PDPortStatisticLength;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDPortStatistic");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDPortDataAdjust( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE  pPDPortDataAdjustSet;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDPortDataAdjust->");

    if (0 == pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_ReadPDPortDataAdjust);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDPortDataAdjust, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    switch (pDDB->PRM.PDPortDataAdjust.RecordState_A[pPrmRead->edd_port_id - 1])
    {
        case EDDI_PRM_VALID:      /* commit executed */
        {
        } break;

        case EDDI_PRM_NOT_VALID:  /* not written until now or default */
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE: /* not allowed */
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDPortDataAdjust!", EDDI_FATAL_ERR_EXCP, pDDB->PRM.PDNCDataCheck.State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    pPDPortDataAdjustSet = &pDDB->PRM.PDPortDataAdjust.RecordSet_A[pPrmRead->edd_port_id - 1];

    if (pPrmRead->record_data_length < pPDPortDataAdjustSet->record_data_length)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_ReadPDPortDataAdjust);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDPortDataAdjust, record_data_length:0x%X record_data_length:0x%X",
                          pPrmRead->record_data_length, pPDPortDataAdjustSet->record_data_length);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_MemCopy(pPrmRead->record_data,
                 &pPDPortDataAdjustSet->PDPortDataAdjust_Max_Record_Net,
                 pPDPortDataAdjustSet->record_data_length);

    pPrmRead->record_data_length = pPDPortDataAdjustSet->record_data_length;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDPortDataAdjust<-");

    return EDD_STS_OK;
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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmReadPDNRTFeedInLoadLimitation( EDD_UPPER_PRM_READ_PTR_TYPE  const  pPrmRead,
                                                                                EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PRM_PDNRT_LOAD_LIMITATION  * const pPDNRTFill = &pDDB->PRM.PDNrtLoadLimitation;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDNRTFeedInLoadLimitation->");

    if (0 != pPrmRead->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDNRTFeedInLoadLimitation);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDNRTFeedInLoadLimitation, pPrmRead->edd_port_id:0x%X", pPrmRead->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    switch (pPDNRTFill->State_A)
    {
        case EDDI_PRM_VALID:      /* commit executed */
        {
        } break;

        case EDDI_PRM_NOT_VALID:  /* not written until now or default */
        {
            pPrmRead->record_data_length = 0;
            return EDD_STS_OK;
        }

        case EDDI_PRM_WRITE_DONE: /* not allowed */
        default:
        {
            EDDI_Excp("EDDI_PrmReadPDNRTFeedInLoadLimitation!", EDDI_FATAL_ERR_EXCP, pPDNRTFill->State_A, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    if (pPrmRead->record_data_length < EDDI_PRM_PDNRT_FILL_RECORD_LENGTH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDNRTFeedInLoadLimitation);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmReadPDNRTFeedInLoadLimitation, record_data_length:0x%X record_data_length:0x%X",
                          pPrmRead->record_data_length, EDDI_PRM_PDNRT_FILL_RECORD_LENGTH);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_MemCopy(pPrmRead->record_data, &pPDNRTFill->pRecordSet_A->RecordSet, EDDI_PRM_PDNRT_FILL_RECORD_LENGTH);
    pPrmRead->record_data_length = EDDI_PRM_PDNRT_FILL_RECORD_LENGTH;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmReadPDNRTFeedInLoadLimitation<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmTransitionDoneCbf()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmTransitionDoneCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_HDB_PTR_TYPE             pHDB;
    EDD_UPPER_RQB_PTR_TYPE              pRQB           = EDDI_NULL_PTR;
    EDDI_PRM_TRANSITION_TYPE  *  const  pPrmTransition = &pDDB->PRM.Transition;
    EDD_UPPER_PRM_COMMIT_PTR_TYPE       pPrmCommit;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTransitionDoneCbf->");

    if (!pPrmTransition->bTransitionRunning)
    {
        EDDI_Excp("EDDI_PrmTransitionDoneCbf, pDDB->PRM.bTransitionRunning != LSA_TRUE", EDDI_FATAL_ERR_EXCP, pDDB->PRM.StateFct, 0);
        return;
    }

    if (pPrmTransition->pRunningRQB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_PrmTransitionDoneCbf, pDDB->PRM.pRunningRQB == EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, pDDB->PRM.StateFct, 0);
        return;
    }

    EDDI_PrmMoveBToAPart2(pDDB);

    // Finish Running COMMIT-RQB
    pRQB = pPrmTransition->pRunningRQB;
    pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

    pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE)pRQB->pParam;
    pPrmCommit->RsvIntervalRed = pDDB->PRM.PDirApplicationData.pRecordSet_A->TransferEndValues; 

    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);

    pPrmTransition->bTransitionRunning = LSA_FALSE;
    pPrmTransition->pRunningRQB        = EDDI_NULL_PTR;

    //Unregister Cbf from PortSTM-Transition
    pDDB->SYNC.PortMachines.Transition.PrmTransitionEndCbf = EDDI_NULL_PTR;

    //Finally call all RQBs in Queue
    //Handle all queued RQBs if available
    while (   (pPrmTransition->RqbQueue.Cnt > 0)
           && (!pPrmTransition->bTransitionRunning))
    {
        pRQB = EDDI_RemoveFromQueue(pDDB, &pPrmTransition->RqbQueue);
        if (pRQB == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_PrmTransitionDoneCbf!", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
        else
        {
            pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

            //Retrigger Port - STMS
            EDDI_PrmRequest(pRQB, pHDB);
        }
    }

    //Pending IndicationRQB during Commit
    if (   (pDDB->PRM.bPendingCommit)
        && (pDDB->PRM.pRQB_PendingCommit != EDDI_NULL_PTR))
    {
        EDDI_RequestFinish(pHDB, pDDB->PRM.pRQB_PendingCommit, EDD_STS_OK);

        pDDB->PRM.bPendingCommit     = LSA_FALSE;
        pDDB->PRM.pRQB_PendingCommit = EDDI_NULL_PTR;
    }

    // empty PRM-Indication if no error occured during Commit
    EDDI_FireInterfaceIndication(pDDB);
    pDDB->PRM.bPendingCommit = LSA_FALSE;

    {
        LSA_UINT16         UsrPortID;
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        // FirePortIndication for all Ports
        for (UsrPortID = 1; UsrPortID <= PortMapCnt; UsrPortID++)
        {
            EDDI_FirePortIndication(pDDB, UsrPortID);
        }
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmTraceRequest()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmTraceRequest( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                             EDD_SERVICE              const  Service )
{
    switch (Service)
    {
        case EDD_SRV_PRM_WRITE:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_WRITE       ServiceId:0x%X", Service);
            break;
        }
        case EDD_SRV_PRM_PREPARE:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_PREPARE     ServiceId:0x%X", Service);
            break;
        }
        case EDD_SRV_PRM_END:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_END         ServiceId:0x%X", Service);
            break;
        }
        case EDD_SRV_PRM_CHANGE_PORT:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_CHANGE_PORT ServiceId:0x%X", Service);
            break;
        }
        case EDD_SRV_PRM_COMMIT:
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDIPrm ************************************************************************");
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_COMMIT      ServiceId:0x%X", Service);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDIPrm ************************************************************************");
            break;
        }
        case EDD_SRV_PRM_READ:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_READ        ServiceId:0x%X", Service);
            break;
        }
        case EDD_SRV_PRM_INDICATION:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmTraceRequest, EDD_SRV_PRM_INDICATION  ServiceId:0x%X", Service);
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_PrmTraceRequest, Unknown Service:", EDDI_FATAL_ERR_EXCP, Service, 0);
            return;
        }
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_req.c                                               */
/*****************************************************************************/

