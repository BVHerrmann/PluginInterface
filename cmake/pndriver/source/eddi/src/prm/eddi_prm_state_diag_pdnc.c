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
/*  F i l e               &F: eddi_prm_state_diag_pdnc.c                :F&  */
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

#include "eddi_swi_ext.h"

#include "eddi_prm_state_diag_pdnc.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_STATE_DIAG_PDNC
#define LTRC_ACT_MODUL_ID  402

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_PrmStateDiagPDNCGood      ( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE const pPDNC,
                                                                     EDDI_PRM_DIAG_PDNC_EVENT        const DiagEv,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_PrmStateDiagPDNCRequired  ( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE const pPDNC,
                                                                     EDDI_PRM_DIAG_PDNC_EVENT        const DiagEv,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_PrmStateDiagPDNCDemanded  ( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE const pPDNC,
                                                                     EDDI_PRM_DIAG_PDNC_EVENT        const DiagEv,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_PrmStateDiagPDNCError     ( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE const pPDNC,
                                                                     EDDI_PRM_DIAG_PDNC_EVENT        const DiagEv,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmSetMaintenancePDNC( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                                   LSA_UINT16                *  const  pCH_Prop,
                                                                   EDDI_PRM_DIAG_PDNC_EVENT     const  Event );


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmIniPDNCFctTable()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmIniPDNCFctTable( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                      i, anzahl;
    PRM_STATE_DIAG_PDNC_FCT  const  StateDiagFctTable[] = { EDDI_PrmStateDiagPDNCGood,
                                                            EDDI_PrmStateDiagPDNCRequired,
                                                            EDDI_PrmStateDiagPDNCDemanded,
                                                            EDDI_PrmStateDiagPDNCError,
                                                            EDDI_PrmStateDiagPDNCStart
                                                          }; //Reihenfolge == EDDI_PRM_DIAG_EVENT

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIniPDNCFctTable->");

    anzahl = sizeof(StateDiagFctTable) / sizeof(PRM_STATE_DIAG_PDNC_FCT);
    for (i = 0; i < anzahl; i++)
    {
        pDDB->PRM.PDNCStateDiagFctTable[i] = StateDiagFctTable[i];
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmIniPDNCFctTable<-");
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PRMCheckTimeOut( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    LSA_UINT32                      UsrPortIndex;
    EDDI_PRM_PDNC_DATA_CHECK     *  pPDNC;
    LSA_UINT32                      ValueHOL, ValueDROP, DiffHol, DiffDrop;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PRMCheckTimeOut->");

    //New KRam structure
    //EDDI_MemCopy(&pDDB->pProfKRamInfo->IntStatInfo, &pDDB->IntStat.info, (LSA_UINT32)sizeof(EDDI_PROF_INT_STATS_TYPE));

    pPDNC = &pDDB->PRM.PDNCDataCheck;

    if (pPDNC->State_A != EDDI_PRM_VALID)
    {
        return;
    }

    pPDNC->DiffAct = 0;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        ValueHOL  = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_HOLBC, pDDB);
        ValueDROP = EDDI_SwiPortGetDropCtr (HwPortIndex, pDDB);

        DiffHol  =  ValueHOL  - pPDNC->StartValue_HOL[UsrPortIndex];
        DiffDrop =  ValueDROP - pPDNC->StartValue_DROP[UsrPortIndex];

        pPDNC->DiffAct += (DiffHol + DiffDrop);

        pPDNC->StartValue_HOL[UsrPortIndex]  = ValueHOL;
        pPDNC->StartValue_DROP[UsrPortIndex] = ValueDROP;
    }

    if ( pPDNC->DropBudget_A.DoCheckError && (pPDNC->DiffAct >= pPDNC->DropBudget_A.ErrorDropBudget)                    )
    {
        pPDNC->DiagStateFct(pPDNC, EDDI_PRM_EV_ERROR, pDDB);
    }
    else if ( pPDNC->DropBudget_A.DoCheckDemanded && (pPDNC->DiffAct >= pPDNC->DropBudget_A.MaintenanceDemandedDropBudget)    )
    {
        pPDNC->DiagStateFct(pPDNC, EDDI_PRM_EV_DEMANDED, pDDB);
    }
    else if ( pPDNC->DropBudget_A.DoCheckRequired && (pPDNC->DiffAct >= pPDNC->DropBudget_A.MaintenanceRequiredDropBudget)   )
    {
        pPDNC->DiagStateFct(pPDNC, EDDI_PRM_EV_REQUIRED, pDDB);
    }
    else
    {
        if ( pPDNC->DropBudget_A.DoCheck )
        {
            pPDNC->DiagStateFct(pPDNC, EDDI_PRM_EV_GOOD, pDDB);
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PRMCheckTimeOut<-");
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmSendIndicationPDNC( EDDI_PRM_PDNC_DATA_CHECK      * const pPDNC,
                                                           EDDI_PRM_DIAG_PDNC_EVENT        const AppearEvent,
                                                           EDDI_PRM_DIAG_PDNC_EVENT        const DisappearEvent,
                                                           EDDI_LOCAL_DDB_PTR_TYPE         const pDDB)
{
    EDD_UPPER_RQB_PTR_TYPE                pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE     pPrmInd;
    EDD_SRV_PRM_IND_DATA_TYPE          *  pDiag;
    LSA_UINT16                            CH_Prop;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmSendIndicationPDNC->");

    // no indication after ressource lack
    if (!pDDB->PRM.bPendingRessource)
    {
        // only remove RQB, if no other RQB is pending during Commit
        if (EDDI_NULL_PTR == pDDB->PRM.pRQB_PendingCommit)
        {
            pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->PRM.IndQueue_Interface);
        }
        else
        {
            pRQB = pDDB->PRM.pRQB_PendingCommit;
        }

        // empty ind-queue -> ressource lack
        if (pRQB == EDDI_NULL_PTR)
        {
            // first NC-Change after ressource lack
            if ( ( LSA_FALSE == pDDB->PRM.DoFireIndication_Interface ) ||
                 ( LSA_FALSE == pDDB->PRM.PDNCDataCheck.bPDNCChanged ) )
            {
                pPDNC->Last_Event_Disappears          = DisappearEvent;
                pDDB->PRM.DoFireIndication_Interface  = LSA_TRUE;
                pDDB->PRM.PDNCDataCheck.bPDNCChanged  = LSA_TRUE;
            }

            pPDNC->Last_Event_Appears             = AppearEvent;

            return;
        }
    }
    // first indication after ressource lack
    else
    {
        pRQB = pDDB->PRM.pRQB_PendingRessource;
    }

    pPrmInd = (EDD_UPPER_PRM_INDICATION_PTR_TYPE)pRQB->pParam;

    if ( ( LSA_FALSE == pDDB->PRM.bPendingRessource ) && ( EDDI_NULL_PTR == pDDB->PRM.pRQB_PendingCommit ) )
    {
        pPrmInd->diag_cnt    = 0;
        pPrmInd->edd_port_id = pPDNC->edd_port_id;
    }

    if ( (AppearEvent == EDDI_PRM_EV_GOOD) && ( DisappearEvent ==  EDDI_PRM_EV_START ) )
    {
        if ( pDDB->PRM.bPendingCommit )
        {
            pDDB->PRM.pRQB_PendingCommit = pRQB;
            return;
        }

        if ( pDDB->PRM.bPendingRessource )
        {
            return;
        }

        //empty indication: nothing to do!
        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
        return;
    }

    pDiag             = &pPrmInd->diag[pPrmInd->diag_cnt];
    pPrmInd->diag_cnt++;

    pDiag->ChannelErrorType    = 0x8008;
    pDiag->ExtChannelErrorType = 0x8000;
    pDiag->ExtChannelAddValue  = pPDNC->DiffAct;

    CH_Prop = EDD_SRV_PRM_CH_PROP_NONE;

    // Good -> xxxx : nur Appear
    if ( ( DisappearEvent == EDDI_PRM_EV_GOOD ) || ( DisappearEvent == EDDI_PRM_EV_START ) )
    {
        CH_Prop = EDD_SRV_PRM_CH_PROP_APPEARS;
        EDDI_PrmSetMaintenancePDNC(pDDB, &CH_Prop, AppearEvent);

        pDiag->ChannelProperties   = CH_Prop;

        if (pDDB->PRM.bPendingCommit)
        {
            pDDB->PRM.pRQB_PendingCommit = pRQB;
            return;
        }

        if (pDDB->PRM.bPendingRessource)
        {
            return;
        }

        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
        return;
    }

    //  xxxx -> Good : nur Disappear
    if (AppearEvent == EDDI_PRM_EV_GOOD)
    {
        CH_Prop = EDD_SRV_PRM_CH_PROP_DISAPPEARS;
        EDDI_PrmSetMaintenancePDNC(pDDB, &CH_Prop, DisappearEvent);

        pDiag->ChannelProperties   = CH_Prop;

        if (pDDB->PRM.bPendingCommit)
        {
            pDDB->PRM.pRQB_PendingCommit = pRQB;
            return;
        }

        if (pDDB->PRM.bPendingRessource)
        {
            return;
        }

        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
        return;
    }

    // Disappear und Appear!

    // Disappear
    CH_Prop = EDD_SRV_PRM_CH_PROP_DISAPPEARS;
    EDDI_PrmSetMaintenancePDNC(pDDB, &CH_Prop, DisappearEvent);
    pDiag->ChannelProperties   = CH_Prop;

    // Appear
    pDiag             = &pPrmInd->diag[pPrmInd->diag_cnt];
    pPrmInd->diag_cnt++;

    pDiag->ChannelErrorType    = 0x8008;
    pDiag->ExtChannelErrorType = 0x8000;
    pDiag->ExtChannelAddValue  = pPDNC->DiffAct;

    CH_Prop = EDD_SRV_PRM_CH_PROP_APPEARS;
    EDDI_PrmSetMaintenancePDNC(pDDB, &CH_Prop, AppearEvent);

    pDiag->ChannelProperties   = CH_Prop;

    if (pDDB->PRM.bPendingCommit)
    {
        pDDB->PRM.pRQB_PendingCommit = pRQB;
        return;
    }

    if (pDDB->PRM.bPendingRessource)
    {
        return;
    }

    EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmSendIndicationPDNC<-");
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateDiagPDNCStart( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE  const  pPDNC,
                                                           EDDI_PRM_DIAG_PDNC_EVENT           const  DiagEv,
                                                           EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCStart->");

    switch (DiagEv)
    {
        case EDDI_PRM_EV_GOOD:
        {
            //special usecase after PRM_COMMIT
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_REQUIRED:
        case EDDI_PRM_EV_DEMANDED:
        case EDDI_PRM_EV_ERROR:
        {
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_START:
        default:
        {
            EDDI_Excp("EDDI_PrmStateDiagPDNCStart", EDDI_FATAL_ERR_EXCP, DiagEv, 0);
            return;
        }
    }

    EDDI_PrmSendIndicationPDNC(pPDNC, DiagEv, EDDI_PRM_EV_START, pDDB);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCStart<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmStateDiagPDNCGood()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateDiagPDNCGood( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE  const  pPDNC,
                                                                  EDDI_PRM_DIAG_PDNC_EVENT         const  DiagEv,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCGood->");

    switch (DiagEv)
    {
        case EDDI_PRM_EV_GOOD:
            return; //nothing to do!

        case EDDI_PRM_EV_REQUIRED:
        case EDDI_PRM_EV_DEMANDED:
        case EDDI_PRM_EV_ERROR:
        {
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_START:
        default:
        {
            EDDI_Excp("EDDI_PrmStateDiagPDNCGood", EDDI_FATAL_ERR_EXCP, DiagEv, 0);
            return;
        }
    }

    EDDI_PrmSendIndicationPDNC(pPDNC, DiagEv, EDDI_PRM_EV_GOOD, pDDB);  // AppearEvent only

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCGood<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmStateDiagPDNCRequired()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateDiagPDNCRequired( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE  const  pPDNC,
                                                                      EDDI_PRM_DIAG_PDNC_EVENT         const  DiagEv,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCRequired->");

    switch (DiagEv)
    {
        case EDDI_PRM_EV_REQUIRED:
            return; //nothing to do!

        case EDDI_PRM_EV_GOOD:
        case EDDI_PRM_EV_DEMANDED:
        case EDDI_PRM_EV_ERROR:
        {
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_START:
        default:
        {
            EDDI_Excp("EDDI_PrmStateDiagPDNCRequired", EDDI_FATAL_ERR_EXCP, DiagEv, 0);
            return;
        }
    }

    EDDI_PrmSendIndicationPDNC(pPDNC, DiagEv, EDDI_PRM_EV_REQUIRED, pDDB);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCRequired<-");
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateDiagPDNCDemanded( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE  const  pPDNC,
                                                                      EDDI_PRM_DIAG_PDNC_EVENT         const  DiagEv,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCDemanded->");

    switch (DiagEv)
    {
        case EDDI_PRM_EV_DEMANDED:
            return; //nothing to do!

        case EDDI_PRM_EV_GOOD:
        case EDDI_PRM_EV_REQUIRED:
        case EDDI_PRM_EV_ERROR:
        {
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_START:
        default:
            EDDI_Excp("PrmStateDiag_PDNC_Demanded", EDDI_FATAL_ERR_EXCP, DiagEv, 0);
            return;

    }

    EDDI_PrmSendIndicationPDNC(pPDNC, DiagEv, EDDI_PRM_EV_DEMANDED, pDDB);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCDemanded<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmStateDiagPDNCError()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmStateDiagPDNCError( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE  const  pPDNC,
                                                                   EDDI_PRM_DIAG_PDNC_EVENT         const  DiagEv,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCError->");

    switch (DiagEv)
    {
        case EDDI_PRM_EV_ERROR:
            return; //nothing to do!

        case EDDI_PRM_EV_GOOD:
        case EDDI_PRM_EV_REQUIRED:
        case EDDI_PRM_EV_DEMANDED:
        {
            pPDNC->DiagStateFct =  pDDB->PRM.PDNCStateDiagFctTable[DiagEv];
            break;
        }

        case EDDI_PRM_EV_START:
        default:
            EDDI_Excp("EDDI_PrmStateDiagPDNCError", EDDI_FATAL_ERR_EXCP, DiagEv, 0);
            return;

    }

    EDDI_PrmSendIndicationPDNC(pPDNC, DiagEv, EDDI_PRM_EV_ERROR, pDDB);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmStateDiagPDNCError<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmSetMaintenancePDNC()                     */
/*                                                                         */
/* D e s c r i p t i o n: Sets EDD_SRV_PRM_CH_PROP_MAIN_REQUIRED or        */
/*                        EDD_SRV_PRM_CH_PROP_MAIN_DEMANDED within         */
/*                        pCH_Prop                                         */
/*                                                                         */
/*                        Must not be set on entry!                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmSetMaintenancePDNC( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                                   LSA_UINT16                *  const  pCH_Prop,
                                                                   EDDI_PRM_DIAG_PDNC_EVENT     const  Event )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmSetMaintenancePDNC->");

    switch (Event)
    {
        case EDDI_PRM_EV_REQUIRED:
            *pCH_Prop |= EDD_SRV_PRM_CH_PROP_MAIN_REQUIRED;
            break;

        case EDDI_PRM_EV_DEMANDED:
            *pCH_Prop |= EDD_SRV_PRM_CH_PROP_MAIN_DEMANDED;
            break;

        case EDDI_PRM_EV_ERROR:
            break;

        case EDDI_PRM_EV_GOOD:
        case EDDI_PRM_EV_START:
        default:
            EDDI_Excp("EDDI_PrmSetMaintenancePDNC", EDDI_FATAL_ERR_EXCP, Event, 0);
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmSetMaintenancePDNC<-");
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_state_diag_pdnc.c                                   */
/*****************************************************************************/

