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
/*  F i l e               &F: eddi_crt_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* SRT (soft real time) for EDDI.                                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_crt_ext.h"
#include "eddi_ser_cmd.h"
#include "eddi_crt_check.h"
#include "eddi_crt_dfp.h"

#if defined (EDDI_CFG_ERTEC_400)
#include "eddi_crt_xch.h"
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_prov.h"
#include "eddi_rto_cons.h"
#endif

#if defined (EDDI_CFG_REV7)
#include "eddi_crt_xch.h"
#endif
#include "eddi_io_provctrl.h"

#define EDDI_MODULE_ID     M_ID_CRT_USR
#define LTRC_ACT_MODUL_ID  111

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTCompIndProvide             ( EDD_UPPER_RQB_PTR_TYPE        pRQB,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCompGetAPDUStatus          ( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCompGetProviderAPDUStatus  ( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCompGetConsumerAPDUStatus  ( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerGetKRAMAPDUStatus  ( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB, 
                                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer,
                                                                           EDD_UPPER_MEM_U8_PTR_TYPE        const  pKRAMDataBuffer,
                                                                           LSA_UINT8                      * const  pTransferStatus,
                                                                           EDDI_SER_DATA_STATUS_TYPE      * const  pDataStatus,
                                                                           LSA_UINT16                     * const  pCycleCnt );


#if (EDDI_CFG_TRACE_MODE != 0)
static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTTraceRequest               ( EDD_UPPER_RQB_PTR_TYPE  const pRQB, 
                                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB  );
#endif

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_CRTCompHandlePendingProvEvents( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE   const pCRT,
                                                                           EDDI_DDB_TYPE                    * const pDDB );


/*===========================================================================*/
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Open/Close-IRTChannel                                                     */
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CRTOpenChannel                         +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                        :    EDDI_LOCAL_HDB_PTR_TYPE       pHDB          +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<= EDDI_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE aSRT already in used by a channel +*/
/*+               EDDI_STS_ERR_RESOURCES   Alloc failed                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for acyclic SRT. The Handle will specify    +*/
/*+               the channel and pDDB the Device. pCDB has all SRT         +*/
/*+               parameters to use for this channel                        +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!          +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Only on Channel can use acyclic SRT!                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = pDDB->pLocal_CRT;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTOpenChannel->");

    /*---------------------------------------------------------------------------*/
    /* CRT-Channel supports only one single User                                 */
    /*---------------------------------------------------------------------------*/
    if (pCRTComp->UsrHandleCnt != 0)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_ALREADY_IN_USE);
        return EDD_STS_ERR_CHANNEL_USE;
    }

    pCRTComp->UsrHandleCnt++; // cnt of currently open handles
    pCRTComp->pHDB = pHDB;

    // EDDI_CRTRpsTrigger(&pCRTComp->Rps, pDDB, EDDI_CRT_RPS_EVENT_START);

    pDDB->RTOUDP.OldCycleVal = (LSA_UINT16)IO_R32(CYCL_COUNT_VALUE);

    return EDD_STS_OK ;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CRTCloseChannel                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCloseChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = pDDB->pLocal_CRT;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTCloseChannel->");

    if ((pCRTComp->pHDB == EDDI_NULL_PTR) || (pCRTComp->UsrHandleCnt != 1))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCloseChannel, ERROR: CRT-Channel was not open.");
        return EDD_STS_ERR_SEQUENCE;
    }

    if ((pCRTComp->ConsumerList.UsedEntriesRTC123) || (pCRTComp->ProviderList.UsedEntries))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCloseChannel, Provider or Consumer must be removed first.");
        return EDD_STS_ERR_SEQUENCE;
    }

    // Stop Scoreboard
    if (!EDDI_CRTRpsIsStopped(&pCRTComp->Rps))
    {
        EDDI_CRTRpsTrigger(&pCRTComp->Rps, pDDB, EDDI_CRT_RPS_EVENT_STOP);
    }

    // Release all unused Indications
    if (pCRTComp->FreeIndicationQueue.Count)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTCloseChannel, releasing %d indication resources...", pCRTComp->FreeIndicationQueue.Count);
    }

    while (pCRTComp->FreeIndicationQueue.Count)
    {
        EDD_UPPER_RQB_PTR_TYPE  const  pIndRQB = (EDD_UPPER_RQB_PTR_TYPE)(void *)EDDI_QueueRemoveFromBegin(&pCRTComp->FreeIndicationQueue);

        if (pIndRQB == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_CRTCloseChannel pIndRQB == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
            return EDD_STS_ERR_EXCP;
        }

        EDDI_RequestFinish(pCRTComp->pHDB, pIndRQB, EDD_STS_OK_CANCEL);
    }

    // TODO Also release Provider-Xchange-Requests ?

    pCRTComp->pHDB          = EDDI_NULL_PTR;
    pCRTComp->UsrHandleCnt--; // cnt of currently open handles

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTCompIndProvide()
 *
 *  Description:Pops a provided Indication-RQB stored in the Indication-Queue
 *              of the CRT-Comp.
 *
 *  Arguments:  pRQB (IN): the RequestBlock
 *  Arguments:  pHDB (IN): the Handle Description Block
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDDI_STS_ERR_RES else.
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompIndProvide( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE     const  pCRT      = pDDB->pLocal_CRT;
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE  const  pIndParam = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE)pRQB->pParam;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCompIndProvide->");

    // clear ParamBlock for safety
    EDDI_MemSet(pIndParam, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDD_RQB_CSRT_IND_PROVIDE_TYPE));

    EDDI_QueueAddToEnd(&pCRT->FreeIndicationQueue,(EDDI_QUEUE_ELEMENT_PTR_TYPE)(void *)pRQB);

    // Now Check if we have pending SB-Entries
    if (   (pCRT->Rps.Status == EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED)
        || (pCRT->Rps.Status == EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED))
    {
        EDDI_CRTRpsTrigger(&pCRT->Rps, pDDB, EDDI_CRT_RPS_EVENT_NEW_IND_RESOURCE); // inform Remote-Provider-Surveillance
    }

    if (pCRT->PendingProvEventsCount)
    {
        EDDI_CRTCompHandlePendingProvEvents(pCRT, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCompHandlePendingProvEvents()
 *
 *  Description: Reads out all pending Provider Events an sends an Indication
 *
 *  Arguments:
 *
 *  Return:     LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompHandlePendingProvEvents( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE     const  pCRT,
                                                                            EDDI_DDB_TYPE                     *  const  pDDB )
{
    EDD_UPPER_RQB_PTR_TYPE               pIndRQB   = EDDI_NULL_PTR;
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE  pIndParam = LSA_NULL;
    LSA_RESULT                           Status;
    LSA_UINT16                           ProviderID;
    EDDI_CRT_PROVIDER_PTR_TYPE           pProvider;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCompHandlePendingProvEvents->");

    // OK we got a free indication Resource
    for (ProviderID = 0; ProviderID < pCRT->ProviderList.MaxEntries; ProviderID++)
    {
        Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, &(pCRT->ProviderList), ProviderID);
        if (Status != EDD_STS_OK)
        {
            continue;
        }

        if (pProvider->PendingIndEvent)
        {
            // OK we have an Event
            // Check if we first need to get an empty RQB
            if (pIndRQB == EDDI_NULL_PTR)
            {
                pIndRQB = (EDD_UPPER_RQB_PTR_TYPE)(void *)EDDI_QueueRemoveFromBegin(&pDDB->pLocal_CRT->FreeIndicationQueue);

                if (pIndRQB == EDDI_NULL_PTR)
                {
                    // no free Indication found --> Lets return and wait until new IndicationRessources arrive.
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCompHandlePendingProvEvents, Could not get free Indication-Ressource");
                    return;
                }
                pIndParam = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE)pIndRQB->pParam;
            }

            if (LSA_HOST_PTR_ARE_EQUAL(pIndParam, LSA_NULL))
            {
                EDDI_Excp("EDDI_CRTCompHandlePendingProvEvents", EDDI_FATAL_ERR_EXCP, 0, "pIndParam is LSA_NULL");
                return;
            }

            pIndParam->Data[pIndParam->Count].Event     = pProvider->PendingIndEvent;
            pIndParam->Data[pIndParam->Count].CycleCnt  = pProvider->PendingCycleCnt;
            pIndParam->Data[pIndParam->Count].UserID    = pProvider->UpperUserId;
            pIndParam->Count++;

            pProvider->PendingIndEvent = 0;
            pCRT->PendingProvEventsCount--;

            if (pIndParam->Count >= EDD_CFG_CSRT_MAX_INDICATIONS)
            {
                // OK IndicationParameter is full
                // --> Fire the Indication
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCompHandlePendingProvEvents, INFO: Firing CRT-Indication, pIndParam->Count:0x%X", pIndParam->Count);
                EDDI_RequestFinish(pDDB->CRT.pHDB, pIndRQB, EDD_STS_OK);
                // Set pIndRQB to Null to get a new one in the next loop
                pIndRQB   = EDDI_NULL_PTR;
                pIndParam = EDDI_NULL_PTR;
            }
        }
    }

    if (!(pIndRQB == EDDI_NULL_PTR))
    {
        // OK there are still some Indications to be sent
        // --> Fire the Indication
        // EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
        // "EDDI_CRTRpsFillAndSendIndication, INFO: Firing CRT-Indication, pIndParam->Count:0x%X", pIndParam->Count);
        EDDI_RequestFinish(pDDB->CRT.pHDB, pIndRQB, EDD_STS_OK);
        pIndRQB = EDDI_NULL_PTR;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCompGetAPDUStatus()
 *
 *  Description: Get APDU Status for spezified provider or consumer
 *               For a consumer there may be no status present, because no
 *               frame was received yet.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompGetAPDUStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                      ReturnValue=EDD_STS_ERR_PARAM;
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE  const  pRQBParam = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;



    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCompGetAPDUStatus->");

    pRQBParam->Present                   = EDD_CSRT_APDU_STATUS_NOT_PRESENT;
    pRQBParam->APDUStatus.CycleCnt       = 0; //sizeof(EDDI_DATA_APDU_STATUS_T);
    pRQBParam->APDUStatus.DataStatus     = 0;
    pRQBParam->APDUStatus.TransferStatus = 0; //always 0 for Provider and RTClass1,2 - Consumer


    switch (pRQBParam->Type)
    {
        case EDD_CSRT_TYPE_PROVIDER:
        {
            ReturnValue=EDDI_CRTCompGetProviderAPDUStatus(pRQB,pDDB);
            break;
        }

        case EDD_CSRT_TYPE_CONSUMER:
        {
            ReturnValue=EDDI_CRTCompGetConsumerAPDUStatus(pRQB,pDDB);
            break;
        }

        default:
        {
            ReturnValue=EDD_STS_ERR_PARAM;
        }
    }

    return ReturnValue;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_CRTCompGetProviderAPDUStatus()
 *
 *  Description: Get APDU Status for spezified  provider
 *               there may be no status present, because no
 *               frame was received yet.
 *
 *  Arguments:  pRQB (IN/OUT): Request Block.
 *              pDDB (IN): Device Description Block.
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompGetProviderAPDUStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT                                      Status;
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE  const  pRQBParam = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         const  pCRT      = pDDB->pLocal_CRT;
    EDDI_CRT_DATA_APDU_STATUS                       LocalAPDU;
    EDDI_CRT_PROVIDER_PTR_TYPE  pProvider;
    LSA_UINT8 ProviderType;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCompGetProviderAPDUStatus->");

    LocalAPDU.Block=0;
    Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, &pCRT->ProviderList, pRQBParam->ID);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        if (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status)
        {
            EDDI_RtoProvGetAPDUStatus(pRQB, pDDB, pProvider);
        }
        return EDD_STS_OK;
    }
    #endif

    ProviderType=pProvider->LowerParams.ProviderType;

    if //EDDI_RTC1_PROVIDER or EDDI_RTC2_PROVIDER
    (   ((EDDI_RTC1_PROVIDER ==ProviderType) || (EDDI_RTC2_PROVIDER == ProviderType))
        && (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status) )
    {
        #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        //get frameptr from acw, as we don´t know which of the 2/3 buffers is used currently
        LSA_UINT32  const  DBOffset = EDDI_GetBitField32(pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0);
        LocalAPDU = *((EDDI_CRT_DATA_APDU_STATUS *)(void *)EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(pDDB->hSysDev, DBOffset, pDDB->ERTEC_Version.Location));
        #else
        LocalAPDU = *((EDDI_CRT_DATA_APDU_STATUS *)(void *)pProvider->LowerParams.pKRAMDataBuffer);
        #endif
    }
    else if (EDDI_RTC3_PROVIDER == ProviderType)
    {
        switch (pProvider->IRTtopCtrl.ProvState)
        {
            case EDDI_PROV_STATE_WF_CLASS3_TX:
            {
                if (EDDI_CRT_PROV_STS_ACTIVE == pProvider->IRTtopCtrl.pAscProvider->Status)
                {
                    //RTC3-Prov has not been activated yet, return APDUStatus of AUX-Provider
                    LocalAPDU = *((EDDI_CRT_DATA_APDU_STATUS *)(void *)pProvider->IRTtopCtrl.pAscProvider->LowerParams.pKRAMDataBuffer);
                }
                else
                {
                    return EDD_STS_OK;
                }
                break;
            }
            case EDDI_PROV_STATE_RED_GREEN_ACTIVE:
            case EDDI_PROV_STATE_RED_ACTIVE:
            {
                //RTC3-Prov is active
                if (EDDI_IS_DFP_PROV(pProvider))
                {
                    EDDI_DFPProviderGetDS(pDDB, pProvider, &(LocalAPDU.Detail.DataStatus));
                }
                else
                {
                    LocalAPDU = *((EDDI_CRT_DATA_APDU_STATUS *)(void *)pProvider->LowerParams.pKRAMDataBuffer);
                }
                break;
            }
            case EDDI_PROV_STATE_PASSIVE:
            {
                return EDD_STS_OK;                
            }
            default:
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDDI_CRTCompGetProviderAPDUStatus, illegal providerstate. ProvID:0x%X, State:0x%X", 
                                    pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                EDDI_Excp("EDDI_CRTCompGetProviderAPDUStatus, illegal providerstate", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
            }
        }
    }
    else
    {
        return EDD_STS_OK;
    }

    //CycleCounter must be taken from CycleCount-Register because Cyclecounter in  Provider-APDU-Status
    //is not incremented by ERTEC

    pRQBParam->APDUStatus.CycleCnt = (LSA_UINT16)IO_R32(CYCL_COUNT_VALUE);

    pRQBParam->Present = EDD_CSRT_APDU_STATUS_PRESENT;

    //Patch Bit_3
    EDDI_SET_DS_BIT__Bit_3(&LocalAPDU.Detail.DataStatus, 0);

    pRQBParam->APDUStatus.DataStatus = LocalAPDU.Detail.DataStatus;
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_CRTCompGetConsumerAPDUStatus()
 *
 *  Description: Get APDU Status for spezified  consumer
 *               there may be no status present, because no
 *               frame was received yet.
 *
 *  Arguments:  pRQB (IN/OUT): Request Block.
 *              pDDB (IN): Device Description Block.
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompGetConsumerAPDUStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{

    LSA_RESULT                                      Status;
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE  const  pRQBParam = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         const  pCRT      = pDDB->pLocal_CRT;
    LSA_UINT16                                      CycleCnt = 0;
    EDDI_SER_DATA_STATUS_TYPE                       DataStatus = 0;
    LSA_UINT8                                       TransferStatus = 0; 
    EDDI_CRT_CONSUMER_PTR_TYPE                      pConsumer=(EDDI_CRT_CONSUMER_PTR_TYPE)0;
    LSA_UINT8                                       ConsumerType=EDDI_RT_CONSUMER_TYPE_INVALID;  
    EDD_UPPER_MEM_U8_PTR_TYPE                       pKRAMDataBuffer;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCompGetConsumerAPDUStatus->");

    Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, &pCRT->ConsumerList, pRQBParam->ID);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
        {
            EDDI_RtoConsGetAPDUStatus(pRQB, pConsumer, pDDB);
        }
        return EDD_STS_OK;
    }
    #endif

    ConsumerType=pConsumer->LowerParams.ConsumerType;
    if //EDDI_RTC1_CONSUMER or EDDI_RTC2_CONSUMER
       ((EDDI_RTC1_CONSUMER == ConsumerType) || (EDDI_RTC2_CONSUMER == ConsumerType))
    {
        //&& ((EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)  ||  pConsumer->bHasSeenAgain))
        if (pConsumer->bHasSeenAgain)
        {
            pKRAMDataBuffer=pConsumer->LowerParams.pKRAMDataBuffer;
            EDDI_CRTConsumerGetKRAMAPDUStatus(pDDB, pConsumer,pKRAMDataBuffer, &TransferStatus, &DataStatus, &CycleCnt);
            pRQBParam->Present= EDD_CSRT_APDU_STATUS_PRESENT;
        }
        else
        {
            pRQBParam->Present= EDD_CSRT_APDU_STATUS_NOT_PRESENT;
        }
    }
    else if (EDDI_RTC3_CONSUMER == ConsumerType)
    {
        switch (pConsumer->IRTtopCtrl.ConsState)
        {
            case EDDI_CONS_STATE_WF_IRDATA:
            {
                if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->IRTtopCtrl.pAscConsumer->pSB->Status)
                {
                    //RTC3-Cons has not been activated yet, return APDUStatus of AUX-Provider
                    pKRAMDataBuffer=pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.pKRAMDataBuffer;
                    EDDI_CRTConsumerGetKRAMAPDUStatus(pDDB, pConsumer,pKRAMDataBuffer, &TransferStatus, &DataStatus, &CycleCnt);
                }
                else
                {
                    return EDD_STS_OK;
                }
                break;
            }
            case EDDI_CONS_STATE_WF_CLASS3_RX_NO_LEGACY:
            case EDDI_CONS_STATE_WF_CLASS3_RX_LEGACY:
            case EDDI_CONS_STATE_ACTIVE:
            {
                //RTC3-Cons is active
                pKRAMDataBuffer=pConsumer->LowerParams.pKRAMDataBuffer;
                EDDI_CRTConsumerGetKRAMAPDUStatus(pDDB, pConsumer,pKRAMDataBuffer, &TransferStatus, &DataStatus, &CycleCnt);
                break;
            }
            case EDDI_CONS_STATE_PASSIVE:
            {
                return EDD_STS_OK;
            }
            default:
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDDI_CRTCompGetAPDUStatus, illegal consumerstate. ConsID:0x%X, State:0x%X", 
                    pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
                EDDI_Excp("EDDI_CRTCompGetAPDUStatus, illegal consumerstate", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->IRTtopCtrl.ConsState);
            }
        }

        if (EDDI_CONS_SCOREBOARD_STATE_PASSIVE==pConsumer->ScoreBoardStatus)
        {
            pRQBParam->Present= EDD_CSRT_APDU_STATUS_NOT_PRESENT;
        }
        else
        {
            pRQBParam->Present= EDD_CSRT_APDU_STATUS_PRESENT;
        }
    }
    else
    {
        return EDD_STS_OK;
    }

    //if //EDDI_RTC1_CONSUMER or EDDI_RTC2_CONSUMER
    //   ((EDDI_RTC1_CONSUMER == ConsumerType) || (EDDI_RTC2_CONSUMER == ConsumerType))
    //{
    //    pRQBParam->Present= EDD_CSRT_APDU_STATUS_PRESENT;
    //}
    //else if (EDDI_CONS_SCOREBOARD_STATE_PASSIVE==pConsumer->ScoreBoardStatus)
    //{
    //    pRQBParam->Present= EDD_CSRT_APDU_STATUS_NOT_PRESENT;
    //}
    //else
    //{
    //    pRQBParam->Present= EDD_CSRT_APDU_STATUS_PRESENT;
    //}

    //the transfer status is always 0
    pRQBParam->APDUStatus.TransferStatus = 0;
    pRQBParam->APDUStatus.CycleCnt = CycleCnt;
    //Patch Bit_3
    EDDI_SET_DS_BIT__Bit_3(&DataStatus, 0);
    pRQBParam->APDUStatus.DataStatus = DataStatus;
    return EDD_STS_OK;
 
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_CRTConsumerGetKRAMAPDUStatus()
 *
 *  Description: Get APDU status for spezified consumer
 *
 *  Arguments:  pDDB (IN): the Device Description Block.
                pConsumer (IN): the consumer to get it's APDU status.
                pKRAMDataBuffer (IN): the communication data buffer,
                    possible values:
                        pConsumer->IRTtopCtrl.pAscConsumer->LowerParams.pKRAMDataBuffer,
                        pConsumer->LowerParams.pKRAMDataBuffer.
 *  Arguments:  pAPDUStatus (OUT): the APDU status for a spezified consumer
 *  Return:     LSA_VOID
 *               
 */
// EDDI_LOCAL_DDB_PTR_TYPE                                                                        
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerGetKRAMAPDUStatus( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                                         EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer,
                                                                         EDD_UPPER_MEM_U8_PTR_TYPE          const  pKRAMDataBuffer,
                                                                         LSA_UINT8                        * const  pTransferStatus,
                                                                         EDDI_SER_DATA_STATUS_TYPE        * const  pDataStatus,
                                                                         LSA_UINT16                       * const  pCycleCnt )
{
#ifdef EDDI_CFG_USE_SW_RPS
    //Handling for ERTEC400 with SW-SB
    LSA_UNUSED_ARG(pDDB);
    if (pConsumer->LowerParams.bKRAMDataBufferValid)
    {
        EDDI_CRT_DATA_APDU_STATUS APDUStatusLocal;
        APDUStatusLocal.Block = *((LSA_UINT32 *)pKRAMDataBuffer);
        *pCycleCnt            = EDDI_HOST2IRTE16(APDUStatusLocal.Detail.CycleCnt);
        *pDataStatus          = APDUStatusLocal.Detail.DataStatus;
        *pTransferStatus      = APDUStatusLocal.Detail.TransferStatus;
    }
    else
    {
        *pTransferStatus    = 0;
        *pDataStatus        = 0;
        *pCycleCnt          = 0;
    }
#else
    //Handling for SOC and ERTEC200
    LSA_UNUSED_ARG(pKRAMDataBuffer);
    EDDI_CRTConsumerGetKRAMDataStatus(pDDB, pConsumer, pTransferStatus, pDataStatus, pCycleCnt);

#endif
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_CRTCompResetAPDUStatus()
 *
 *  Description: Reset APDU Status specified by the pDataBuffer-Pointer and set
 *               set DataStatus to DataStatusInitValue
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompResetAPDUStatus( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                            EDD_UPPER_MEM_U8_PTR_TYPE         pDataBuffer,
                                                            LSA_UINT8                  const  DataStatusInitValue,
                                                            LSA_UINT8                  const  TransferStatusInitValue,
                                                            LSA_BOOL                   const  bSysRed)
{
    EDDI_CRT_DATA_APDU_STATUS            LocalAPDU;
    EDDI_CRT_DATA_APDU_STATUS  *  const  pAPDU      = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pDataBuffer;
    //#if defined (EDDI_CFG_SYSRED_2PROC)
    //LSA_UINT32                           ProviderID = ((LSA_UINT32) pDataBuffer - (LSA_UINT32) pDDB->pKRamStateBuffer) / sizeof(EDDI_CRT_PAEA_APDU_STATUS);
    //#endif

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTCompResetAPDUStatus->");

    LocalAPDU.Detail.CycleCnt       = 0x0000;
    LocalAPDU.Detail.DataStatus     = DataStatusInitValue;     /* Byte kein Problem BIG Endian ! */
    LocalAPDU.Detail.TransferStatus = TransferStatusInitValue;

    EDDI_ENTER_IO_S_INT(pDDB->Glob.InterfaceID);
    if (bSysRed)
    {
        //DataStatus.State and .Redundancy from KRAM wins over DataStatusInitValue (situation when a SysRedProv is being activated)
        LocalAPDU.Detail.DataStatus = (DataStatusInitValue & ~(EDD_CSRT_DSTAT_BIT_STATE+EDD_CSRT_DSTAT_BIT_REDUNDANCY)) | (pAPDU->Detail.DataStatus & (EDD_CSRT_DSTAT_BIT_STATE+EDD_CSRT_DSTAT_BIT_REDUNDANCY));
        pAPDU->Block                = LocalAPDU.Block;

        //#if defined (EDDI_CFG_SYSRED_2PROC)
        //    pDDB->pGSharedRAM->DataStatusShadow[ProviderID]  = LocalAPDU.Detail.DataStatus;
        //#endif
    }
    else
    {
        pAPDU->Block = LocalAPDU.Block;
    }
    EDDI_EXIT_IO_S_INT(pDDB->Glob.InterfaceID);
}
/*---------------------- end [subroutine] ---------------------------------*/


#if (EDDI_CFG_TRACE_MODE != 0)
/******************************************************************************
 *  Function:    EDDI_CRTTraceRequest()  //for Tracing only
 *
 *  Description: Only Creates TraceEntries for some Services.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTTraceRequest( EDD_UPPER_RQB_PTR_TYPE  const  pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE const pDDB  )
{   
    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_SRT_PROVIDER_ADD:
        {
            EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pAddProvider = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;

            if (pAddProvider) 
            {
                if (EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_PROVIDER_ADD)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      "EDDI_CRTRequest:Add_Provider ServiceID == SRT_PROVIDER_ADD  FrameID:0x%X CyclePhase:0x%X",
                                      pAddProvider->FrameID, pAddProvider->CyclePhase);
                }
                else
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      "EDDI_CRTRequest:Add_Provider ServiceID == AUX_PROVIDER_ADD  FrameID:0x%X CyclePhase:0x%X",
                                      pAddProvider->FrameID, pAddProvider->CyclePhase);
                }
                EDDI_CRT_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRequest:Add_Provider CyclePhaseSequence:0x%X CycleReductionRatio:0x%X DataLen:0x%X GroupID:0x%X UserID:0x%X DataOffset:0x%X BufferProperties:0x%X",
                                  pAddProvider->CyclePhaseSequence, pAddProvider->CycleReductionRatio, pAddProvider->DataLen, pAddProvider->GroupID, pAddProvider->UserID, pAddProvider->IOParams.DataOffset, pAddProvider->IOParams.BufferProperties);
            }
            break;
        }

        case EDD_SRV_SRT_PROVIDER_REMOVE:
        {
            EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE  const  pRemProvider = (EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE)pRQB->pParam;

            if (pRemProvider) 
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRequest:Rem_Provider ProviderID:0x%X", pRemProvider->ProviderID);
            }
            break;
        }

        case EDD_SRV_SRT_PROVIDER_CONTROL:
        {
            EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE  const  pProviderControl = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)pRQB->pParam;

            if (pProviderControl) 
            {
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                    "EDDI_CRTRequest:Provider_Control ProvID:0x%X Mask:0x%X Status:0x%X pProviderControl->Mode:0x%X",
                                  pProviderControl->ProviderID, pProviderControl->Mask, pProviderControl->Status, pProviderControl->Mode); 
                EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                    "EDDI_CRTRequest:Provider_Control CyclePhaseSequence:0x%X CycleReductionRatio:0x%X DataOffset:0x%X BufferProperties:0x%X AutoStopConsumerID:0x%X",
                    pProviderControl->CyclePhaseSequence, pProviderControl->CycleReductionRatio, pProviderControl->IOParams.DataOffset, pProviderControl->IOParams.BufferProperties, pProviderControl->AutoStopConsumerID);
            }
            break;
        }

        case EDD_SRV_SRT_CONSUMER_ADD:
        {
            EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pAddConsumer = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;

            if (pAddConsumer) 
            {
                if (EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_CONSUMER_ADD)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      "EDDI_CRTRequest:Add_Consumer ServiceID == SRT_CONSUMER_ADD  FrameID:0x%X CyclePhase:0x%X",
                                      pAddConsumer->FrameID, pAddConsumer->CyclePhase);
                }
                else
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      "EDDI_CRTRequest:Add_Consumer ServiceID == AUX_CONSUMER_ADD  FrameID:0x%X CyclePhase:0x%X",
                                      pAddConsumer->FrameID, pAddConsumer->CyclePhase);
                }

                EDDI_CRT_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRequest:Add_Consumer CycleReductionRatio:0x%X DataLen:0x%X DataHoldFactor:0x%X UserID:0x%X DataOffset:0x%X, BufferProperties:0x%X Partial_DataLen:0x%X Partial_DataOffset:0x%X",
                                  pAddConsumer->CycleReductionRatio,
                                  pAddConsumer->DataLen, pAddConsumer->DataHoldFactor,
                                  pAddConsumer->UserID,  pAddConsumer->IOParams.DataOffset, pAddConsumer->IOParams.BufferProperties,
                                  pAddConsumer->Partial_DataLen, pAddConsumer->Partial_DataOffset); 
            }
            break;
        }

        case EDD_SRV_SRT_CONSUMER_REMOVE:
        {
            EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE const pRemConsumer = (EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE)pRQB->pParam;
            
            if (pRemConsumer) 
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRequest:Rem_Consumer ConsumerID:0x%X", pRemConsumer->ConsumerID);
            }
            break;
        }

        case EDD_SRV_SRT_CONSUMER_CONTROL:
        {
            EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE  const  pConsumerControl = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE)pRQB->pParam;
            
            if (pConsumerControl)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_CRTRequest:Consumer_Control ConsID:0x%X pConsumerControl->Mode:0x%X",
                                  pConsumerControl->ConsumerID, pConsumerControl->Mode);
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                  "EDDI_CRTRequest:Consumer_Control CycleReductionRatio:0x%X DataOffset:0x%X, BufferProperties:0x%X",
                                  pConsumerControl->CycleReductionRatio, 
                                  pConsumerControl->IOParams.DataOffset, pConsumerControl->IOParams.BufferProperties); 
            }
            break;
        }

        default:
            return;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*=============================================================================
 * function name: EDDI_CRTRequest()
 *
 * function:
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_RESULT                               Status;
    LSA_BOOL                                 Indicate;
    EDD_SERVICE                       const  Service = EDD_RQB_GET_SERVICE(pRQB);
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB    = pHDB->pDDB;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT    = pDDB->pLocal_CRT;
    LSA_BOOL                                 bDebugInfoExists = LSA_FALSE;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRequest->pRQB:0x%X SrvId:0x%X", 
                      (LSA_UINT32)pRQB, Service);

    Indicate = LSA_TRUE;

    #if (EDDI_CFG_TRACE_MODE != 0)
    EDDI_CRTTraceRequest(pRQB, pDDB);
    #endif

    Status = EDD_STS_OK;

    if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
    {
        switch (Service)
        {
            #if defined (EDDI_CFG_ERTEC_400)
            case EDD_SRV_SRT_CONSUMER_XCHANGE_BUF:
            #endif
            case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE:
            case EDD_SRV_SRT_CONSUMER_ADD:
            case EDD_SRV_SRT_PROVIDER_ADD:
            case EDD_SRV_SRT_CONSUMER_CONTROL:
            case EDD_SRV_SRT_CONSUMER_REMOVE:
            case EDD_SRV_SRT_PROVIDER_CONTROL:
            case EDD_SRV_SRT_PROVIDER_REMOVE:
            {
                EDDI_SERSheduledRequest(pHDB->pDDB, pRQB, (LSA_UINT32)EDDI_CRTRequest);
                return;
            }

            default:
                break;
        }
    }

    switch (Service)
    {
        //common usage
        case EDD_SRV_SRT_IND_PROVIDE:
        {
            EDDI_CRTCompIndProvide(pRQB, pDDB);
            Indicate = LSA_FALSE;
            break;
        }

        //no separate processing
        case EDD_SRV_SRT_CONSUMER_ADD:
            Status = EDDI_CRTConsumerListAddEntry(pRQB, pDDB);
            break;
        case EDD_SRV_SRT_PROVIDER_ADD:
            Status = EDDI_CRTProviderListAddEntry(pRQB, pDDB);
            break;

        case EDD_SRV_SRT_CONSUMER_CONTROL:
            Status = EDDI_CRTConsumerListControl(pRQB, pDDB);
            bDebugInfoExists = LSA_TRUE;
            break;
        case EDD_SRV_SRT_CONSUMER_REMOVE:
            Status = EDDI_CRTConsumerListRemoveEntry(pRQB, pDDB, &Indicate);
            break;
        case EDD_SRV_SRT_PROVIDER_CONTROL:
            Status = EDDI_CRTProviderListCtrl(pRQB, pDDB, &Indicate);
            bDebugInfoExists = LSA_TRUE;
            break;
        case EDD_SRV_SRT_PROVIDER_REMOVE:
            Status = EDDI_CRTProviderListRemoveEntry(pRQB, pDDB, &Indicate);
            break;
        case EDD_SRV_SRT_GET_APDU_STATUS:
            Status = EDDI_CRTCompGetAPDUStatus(pRQB, pDDB);
            break;

        case EDD_SRV_SRT_PROVIDER_SET_STATE:
            Status = EDDI_CRTProviderListSetGroupStatus(pRQB, pDDB);
            break;
        case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE:
            Status = EDDI_CRTProviderListChangePhase(pRQB, pDDB, &Indicate);
            break;

            #if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
        case EDD_SRV_SRT_CONSUMER_XCHANGE_BUF:
            Status = EDDI_CRTConsumerListXchangeBuf(pRQB, pDDB);
            break;
        case EDD_SRV_SRT_PROVIDER_XCHANGE_BUF:
            Status = EDDI_CRTProviderListXchangeBuf(pRQB, pDDB, &Indicate);
            break;
            #endif

        default:
            Status = EDD_STS_ERR_NOT_IMPL;
            break;
    }

    if (Indicate)
    {
        if (   bDebugInfoExists
            && (EDD_STS_OK != Status) )
        {
            if (pDDB->ErrDetail.bSet)
            {
                EDDI_RQB_CSRT_CONTROL_DEBUGINFO_TYPE * pDebugInfoRes; 

                if (EDD_SRV_SRT_CONSUMER_CONTROL == Service)
                {
                    EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE  const  pParam = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
                    pDebugInfoRes = (EDDI_RQB_CSRT_CONTROL_DEBUGINFO_TYPE *)(LSA_VOID *)&pParam->DebugInfo[0];
                }
                else if (EDD_SRV_SRT_PROVIDER_CONTROL == Service)
                {
                    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE  const  pParam = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
                    pDebugInfoRes = (EDDI_RQB_CSRT_CONTROL_DEBUGINFO_TYPE *)(LSA_VOID *)&pParam->DebugInfo[0];
                }
                else
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTRequest: FATAL: no DebugInfo. Service:0x%X", 
                                      Service);
                    EDDI_Excp("EDDI_CRTRequest: FATAL: no DebugInfo.", EDDI_FATAL_ERR_EXCP, Service, 0);
                    return;
                }

                //copy error-info from DDB to RQB
                pDebugInfoRes->Ident        = EDD_DEBUGINFO_CONTROL_IDENT_EDDI;
                pDebugInfoRes->ResponseHigh = (LSA_UINT8)((pDDB->ErrDetail.Error & 0xFF00UL)>>8);
                pDebugInfoRes->ResponseLow  = (LSA_UINT8)(pDDB->ErrDetail.Error & 0xFFUL);
                pDebugInfoRes->ModuleIDHigh = (LSA_UINT8)((pDDB->ErrDetail.ModuleID & 0xFF00UL)>>8);
                pDebugInfoRes->ModuleIDLow  = (LSA_UINT8)(pDDB->ErrDetail.ModuleID & 0xFFUL);
                #if !defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //leads to smaller code
                pDebugInfoRes->LineHigh     = (LSA_UINT8)((pDDB->ErrDetail.Line & 0xFF00UL)>>8);
                pDebugInfoRes->LineLow      = (LSA_UINT8)(pDDB->ErrDetail.Line & 0xFFUL);
                #else
                pDebugInfoRes->LineHigh     = 0;
                pDebugInfoRes->LineLow      = 0;
                #endif
                
                pDDB->ErrDetail.bSet = 0;
            }
        }
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }

    //check if an event occured during one of the services above
    if (pDDB->CRT.Rps.PendingEvents)
    {
        Status = EDDI_CRTRpsFillAndSendIndication(pDDB, pDDB->CRT.ConsumerList.LastIndexRTC123, pDDB->CRT.ConsumerList.LastIndexDFP, LSA_TRUE /*bDirectIndication*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRTRpsInformStateMachine(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE);
        }
        
        pDDB->CRT.Rps.PendingEvents = LSA_FALSE;
    }

    if (pCRT->PendingProvEventsCount)
    {
        EDDI_CRTCompHandlePendingProvEvents(pCRT, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_usr.c                                               */
/*****************************************************************************/

