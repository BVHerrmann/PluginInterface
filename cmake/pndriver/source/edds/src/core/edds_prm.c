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
/*  F i l e               &F: edds_prm.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-PRM-functions                               */
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

#define LTRC_ACT_MODUL_ID  9
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_PRM */

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
/*                                    defines                                */
/*===========================================================================*/

/* Note: EDDS_CFG_PRM_DONT_CHECK_PHYTYPE should not be set.   */
/*       prevents check for PHYTYPE within MAUTYPE.           */

//#ifndef EDDS_CFG_PRM_DONT_CHECK_PHYTYPE
//#define EDDS_PRM_CHECK_PHYTYPE
//#endif

/* Note: EDDS_CFG_PRM_DONT_CHECK_ONE_PORT_UP should not be set. */
/*       prevents check for at least one Port remains UP       */

#ifndef EDDS_CFG_PRM_DONT_CHECK_ONE_PORT_UP
#define EDDS_PRM_CHECK_ONE_PORT_UP
#endif


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CopyUpper2Local                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CopyUpper2Local(
    LSA_UINT8   EDD_UPPER_MEM_ATTR const * pSrc,
    LSA_UINT8   EDDS_LOCAL_MEM_ATTR * pDst,
    LSA_UINT32  Length)
{
    while ( Length-- )
    {
        *pDst++ = *pSrc++;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CopyLocal2Upper                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CopyLocal2Upper(
    LSA_UINT8   EDDS_LOCAL_MEM_ATTR const * pSrc,
    LSA_UINT8   EDD_UPPER_MEM_ATTR * pDst,
    LSA_UINT32  Length)
{
    while ( Length-- )
    {
        *pDst++ = *pSrc++;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmAlloc                               +*/
/*+  Input/Output          :    EDDS_PRM_PTR_TYPE     pPrm                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates memory within PRM structure. PRM structure      +*/
/*+               has to be zero on entry!                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmAlloc(
    EDDS_PRM_PTR_TYPE     pPrm)
{

    LSA_RESULT              Status;

    EDDS_PRM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmAlloc(pPrm: 0x%X)",
                           pPrm);

    Status = EDD_STS_OK;
    EDDS_ALLOC_UPPER_MEM_LOCAL(((EDD_UPPER_MEM_PTR_TYPE *)&pPrm->PortData.pPortIDModeArray),sizeof(EDD_RQB_PORTID_MODE_TYPE));

    if ( LSA_HOST_PTR_ARE_EQUAL(pPrm->PortData.pPortIDModeArray, LSA_NULL) )
    {
        Status = EDD_STS_ERR_RESOURCE;
    }

    EDDS_PRM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmAlloc(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmFree                                +*/
/*+  Input/Output          :    EDDS_PRM_PTR_TYPE     pPrm                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees memory within PRM structure. checks if something to +*/
/*+               free so it can always be called.                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 pPrm is freed (if not yet LSA_NULL)
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmFree(
    EDDS_PRM_PTR_TYPE     pPrm)
{

    LSA_UINT16              FreeStat;

    EDDS_PRM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmFree(pPrm: 0x%X)",
                           pPrm);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pPrm->PortData.pPortIDModeArray, LSA_NULL))
    {
        EDDS_FREE_UPPER_MEM_LOCAL(&FreeStat,pPrm->PortData.pPortIDModeArray);
        EDDS_FREE_CHECK(FreeStat);
        pPrm->PortData.pPortIDModeArray = LSA_NULL;
    }

    EDDS_PRM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmFree()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmInit                                +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes PRM structures. Structure has to be zero on   +*/
/*+               entry and memory has to be allocated within structures!   +*/
/*+               Called after DDB creation.                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmInit(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT32             j;
    EDDS_PRM_PTR_TYPE      pPrm;

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmInit(pDDB: 0x%X)",
                           pDDB);

    pPrm = &pDDB->pGlob->Prm;
    pPrm->PrmState = EDDS_PRM_STATE_PREPARED;
    for( j=0; j<EDDS_MAX_PORT_CNT; j++ )
        pPrm->PrmChangePortState[j] = EDDS_PRMCHANGEPORT_STATE_ISPLUGGED;
    pPrm->FirstCommitExecuted = LSA_FALSE;

    pPrm->FrameDrop.CheckEnable          = LSA_FALSE;
    pPrm->FrameDrop.ErrorStatus          = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.ErrorStatusIndicated = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.RecordSet_A.Present  = LSA_FALSE;
    pPrm->FrameDrop.RecordSet_B.Present  = LSA_FALSE;
    pPrm->FrameDrop.DropCnt              = 0;

    pPrm->PDSCFDataCheck.RecordSet_A.Present = LSA_FALSE;
    pPrm->PDSCFDataCheck.RecordSet_B.Present = LSA_FALSE;

    for ( j=0; j<EDDS_FRAME_DROP_BUDGET_CNT; j++)
    {
        pPrm->FrameDrop.RecordSet_A.Budget[j].Enabled = LSA_FALSE;
        pPrm->FrameDrop.RecordSet_B.Budget[j].Enabled = LSA_FALSE;
    }

    pPrm->PRMUsed = LSA_FALSE;

    for ( j=0; j<EDDS_MAX_PORT_CNT; j++)
    {
        pPrm->DoIndicate[j] = LSA_FALSE;

        pPrm->PortData.RecordSet_A[j].Present = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].Present = LSA_FALSE;

        pPrm->PortData.RecordSet_A[j].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].PreambleLengthPresent    = LSA_FALSE;

        pPrm->PortData.RecordSet_B[j].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].PreambleLengthPresent    = LSA_FALSE;

        pPrm->MRPPort.Present_A[j]          = LSA_FALSE;
        pPrm->MRPPort.Present_B[j]          = LSA_FALSE;

        pPrm->SupportsMRPInterconnPortConfig.Present_A[j]        = LSA_FALSE;
        pPrm->SupportsMRPInterconnPortConfig.Present_B[j]        = LSA_FALSE;
        pPrm->DefPortStates.RecordSet_A[j].PortStatePresent  = LSA_FALSE;
        pPrm->DefPortStates.RecordSet_B[j].PortStatePresent  = LSA_FALSE;
    }

    pPrm->SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB = LSA_FALSE;

    pPrm->MRPPort.AtLeastTwoRecordsPresentB = LSA_FALSE;

    pPrm->MRPInterface.Present_A     = LSA_FALSE;
    pPrm->MRPInterface.Present_B     = LSA_FALSE;

    pPrm->DefPortStates.Present_A    = LSA_FALSE;
    pPrm->DefPortStates.Present_B    = LSA_FALSE;

    pPrm->PDNRTFill.RecordSet_A.Present = LSA_FALSE;
    pPrm->PDNRTFill.RecordSet_B.Present = LSA_FALSE;

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmInit()");

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SetupPrmIndEntry                        +*/
/*+  Input/Output          :    EDD_SRV_PRM_IND_DATA_PTR_TYPE   pPrm        +*/
/*+                             LSA_UINT32                      ErrorStatus +*/
/*+                             LSA_BOOL                        Appear      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pPrm        : Pointer to Diag-entry within IND-RQB                     +*/
/*+  ErrorStatus : ErrorStatus to Setup diag entry for                      +*/
/*+                                                                         +*/
/*+                EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED                +*/
/*+                EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED                +*/
/*+                EDDS_DIAG_FRAME_DROP_ERROR                               +*/
/*+                                                                         +*/
/*+  Appear      : Diag appears (LSA_TRUE) or disappears (LSA_FALSE)        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup entry within indication rqb structure               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SetupPrmIndEntry(
    EDD_SRV_PRM_IND_DATA_PTR_TYPE             pPrm,
    LSA_UINT32                                ErrorStatus,
    LSA_BOOL                                  Appear)
{
    switch ( ErrorStatus )
    {
        case EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED:
            pPrm->ChannelProperties  = EDD_SRV_PRM_CH_PROP_MAIN_REQUIRED;
            break;
        case EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED:
            pPrm->ChannelProperties  = EDD_SRV_PRM_CH_PROP_MAIN_DEMANDED;
            break;
        case EDDS_DIAG_FRAME_DROP_ERROR:
        default:
            pPrm->ChannelProperties  = 0;
            break;
    };

    pPrm->ChannelProperties   |= (Appear ? EDD_SRV_PRM_CH_PROP_APPEARS : EDD_SRV_PRM_CH_PROP_DISAPPEARS);
    pPrm->ChannelErrorType     = EDD_SRV_PRM_CHANNEL_ERROR_TYPE_NC_MISMATCH;
    pPrm->ExtChannelErrorType  = EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_NC_FD_NR;
    pPrm->ExtChannelAddValue   = 0;

    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagFrameDropUpdate                    +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT32                  DropCnt         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  DropCnt    : Dropped frames till last call                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called with DropCnt of frames.                            +*/
/*+               Updates current ErrorStatus depending on DropCnt.         +*/
/*+               Shall only be called if FrameDrop check enabled!          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropUpdate(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt)
{

    EDDS_PRM_NC_DATA_PTR_TYPE               pFrameDrop;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagFrameDropUpdate(pDDB: 0x%X, DropCnt: %d)",
                           pDDB,
                           DropCnt);

    pFrameDrop = &pDDB->pGlob->Prm.FrameDrop;

    pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_OK;

    /* Error > Demanded > Required */

    if ( DropCnt != 0 ) /*  usually we have no drops and so we have no errors .. */
    {
        if (pFrameDrop->RecordSet_A.Budget[EDDS_ERROR_BUDGET_IDX].Enabled &&
            DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_ERROR_BUDGET_IDX].Value)
        {
            pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_ERROR;
        }
        else
        {
            if (pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled &&
                DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value)
            {
                pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED;
            }
            else
            {
                if (pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled &&
                    DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value)
                {
                    pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED;
                }
            }
        }
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagFrameDropUpdate()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmSetBToNotPresent                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Inits record set B to "not present"                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 checked before function call
//WARNING: be careful when calling this function, make sure not to use pDDb as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmSetBToNotPresent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT32 i;
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm = &pDDB->pGlob->Prm;

    pPrm->FrameDrop.RecordSet_B.Present  = LSA_FALSE;

    for (i=0; i<EDDS_MAX_PORT_CNT; i++)
    {
        pPrm->PortData.RecordSet_B[i].Present = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].PreambleLengthPresent    = LSA_FALSE;

        pPrm->MRPPort.Present_B[i] = LSA_FALSE;
        pPrm->SupportsMRPInterconnPortConfig.Present_B[i] = LSA_FALSE;
        pPrm->DefPortStates.RecordSet_B[i].PortStatePresent = LSA_FALSE;
    }

    pPrm->SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB    = LSA_FALSE;
    pPrm->MRPPort.AtLeastTwoRecordsPresentB 	= LSA_FALSE;
    pPrm->MRPInterface.Present_B            	= LSA_FALSE;
    pPrm->DefPortStates.Present_B    			= LSA_FALSE;
    pPrm->PDNRTFill.RecordSet_B.Present  		= LSA_FALSE;
    pPrm->PDSCFDataCheck.RecordSet_B.Present 	= LSA_FALSE;

}

/**
 * executes prm commit actions of record PDNRTFeedInLoadLimitation (synchronous).
 * @param pDDB device handle
 * @return EDD_STS_OK
 */
static LSA_VOID EDDS_PrmCopySetB2A_PDNRTFeedInLoadLimitation(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{    
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm   = &pDDB->pGlob->Prm;
    pPrm->PRMUsed = LSA_TRUE;  /* we used PRM */
    /* --------------------------------------------------------------------------*/
    /* PDNRTFeedInLoadLimitation-Record (on interface)                           */
    /* --------------------------------------------------------------------------*/
    {
        /* FILL record present? */
        if( pPrm->PDNRTFill.RecordSet_B.Present )
        {
            pPrm->PDNRTFill.RecordSet_A = pPrm->PDNRTFill.RecordSet_B;

            /* set FeedInLoadLimitationActive according to the record */
            if(EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_ACTIVE == pPrm->PDNRTFill.RecordSet_A.LoadLimitationActive)
            {
                pDDB->pGlob->FeedInLoadLimitationActive = LSA_TRUE;
            }
            else
            {
                pDDB->pGlob->FeedInLoadLimitationActive = LSA_FALSE;
            }
            
            /* set IO_Configured according to the record */
            if(EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED == pPrm->PDNRTFill.RecordSet_A.IO_Configured)
            {
                pDDB->pGlob->IO_Configured = LSA_TRUE;

                // set NRT values corresponding parameterset - NRT and CRT
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_ALARM_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTAlarm[EDDS_DPB_PARAMETERSET_CRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_IP_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntARPIP[EDDS_DPB_PARAMETERSET_CRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTOther[EDDS_DPB_PARAMETERSET_CRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntOther[EDDS_DPB_PARAMETERSET_CRT_MODE];
            }
            else
            {
                pDDB->pGlob->IO_Configured = LSA_FALSE;

                // set NRT values corresponding parameterset - NRT only
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_ALARM_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTAlarm[EDDS_DPB_PARAMETERSET_NRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_IP_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntARPIP[EDDS_DPB_PARAMETERSET_NRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTOther[EDDS_DPB_PARAMETERSET_NRT_MODE];
                pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntOther[EDDS_DPB_PARAMETERSET_NRT_MODE];
            }
        }
        else
        {
            /* RecordSet_B is not present, so set RecordSet_A as not present as well */
            pPrm->PDNRTFill.RecordSet_A.Present = LSA_FALSE;

            /* setup FILL default configuration */
            pDDB->pGlob->IO_Configured              = pDDB->pGlob->IO_ConfiguredDefault; /* default: see OPEN */
            pDDB->pGlob->FeedInLoadLimitationActive = pDDB->pGlob->FeedInLoadLimitationActiveDefault; /* default: see OPEN */
        }

        EDDS_SchedulerUpdateCycleTime(pDDB);

        /* NOTE: even if the link is not changing the FILL could be switched ON/OFF --> re-calculate bandwith for Tx/Rx */
        EDDS_CalculateTxRxBandwith(pDDB);
    }

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                      "EDDS_PrmCopySetB2A(): LoadLimitationActive=0x%X, IO_Configured=0x%X",
                      pDDB->pGlob->FeedInLoadLimitationActive, pDDB->pGlob->IO_Configured);

    /* notify system adaption of current FeedInLoadLimitation settings */
    pDDB->pGlob->NRT_UseNotifySchedulerCall = (EDD_STS_OK_PENDING == EDDS_NOTIFY_FILL_SETTINGS(pDDB->hSysDev, ((pDDB->pGlob->FeedInLoadLimitationActive) ? EDD_SYS_FILL_ACTIVE : EDD_SYS_FILL_INACTIVE), ((pDDB->pGlob->IO_Configured) ? EDD_SYS_IO_CONFIGURED_ON : EDD_SYS_IO_CONFIGURED_OFF)))?LSA_TRUE:LSA_FALSE;
}

/**
 * executes prm commit actions of record PDNCDataCheck (synchronous).
 * @param pDDB device handle
 * @return ---
 */

static LSA_VOID EDDS_PrmCopySetB2A_PDNC(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    /* --------------------------------------------------------------------------*/
    /* PDNC-Record (on interface)                                                */
    /* --------------------------------------------------------------------------*/
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm   = &pDDB->pGlob->Prm;

    pPrm->FrameDrop.RecordSet_A = pPrm->FrameDrop.RecordSet_B;

    /* If the record set is present we have to indicate independend of ErrorStatus */
    /* we set a flag for next indication-check                                     */

    /* force indication */
    pPrm->DoIndicate[0] = LSA_TRUE;

    pPrm->FrameDrop.ErrorStatus          = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.ErrorStatusIndicated = EDDS_DIAG_FRAME_DROP_OK;

    /* Set global CheckEnable flag if at least one Budget ist enabled */
    if ( pPrm->FrameDrop.RecordSet_A.Present &&
         ( pPrm->FrameDrop.RecordSet_A.Budget[0].Enabled ||
           pPrm->FrameDrop.RecordSet_A.Budget[1].Enabled ||
           pPrm->FrameDrop.RecordSet_A.Budget[2].Enabled ))
    {
        pPrm->FrameDrop.CheckEnable = LSA_TRUE;
        /* actualize ErrorStatus depending on actual DropCnt */
        EDDS_DiagFrameDropUpdate(pDDB,pPrm->FrameDrop.DropCnt);
    }
    else
    {
        pPrm->FrameDrop.CheckEnable = LSA_FALSE;
    }
}

/**
 * executes prm commit actions of record PDSetDefaultPortStates (synchronous).
 *
 * The port states are saved internal. No LLIF calls.
 * @param pDDB device handle
 * @return ---
 */
static LSA_VOID EDDS_PrmCopySetB2A_PDSetDefaultPortStates(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_UINT32 i;
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm   = &pDDB->pGlob->Prm;
    for (i = 0; i < EDDS_MAX_PORT_CNT; i++)
    {
        /* flag for MRP handling to know if PortState still setup */
        pPrm->PortStateSet[i] = LSA_FALSE;
        pPrm->fireExtLinkChange[i] = LSA_FALSE;
        /* setup portstate to unchanged */
        pPrm->PortStates.PortIDState[i] = EDD_PORT_STATE_UNCHANGED;
    }
    /* --------------------------------------------------------------------------*/
    /* PDSetDefaultPortStates                                                    */
    /* Note: Can only be present on first COMMIT after startup !                 */
    /* --------------------------------------------------------------------------*/
    if (pDDB->pGlob->Prm.DefPortStates.Present_B)
    {
        LSA_UINT16 Old_PortState;

        for (i = 0; i < EDDS_MAX_PORT_CNT; i++)
        {
            if(i < pDDB->pGlob->HWParams.Caps.PortCnt )
            {
                /* PortState present for Port? */
                if ( pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortStatePresent)
                {
                    LSA_UINT16  PortState = EDD_PORT_STATE_FORWARDING;

                    switch (pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortState )
                    {
                        case EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE:
                            PortState = EDD_PORT_STATE_DISABLE;
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: PDSetDefaultPortState: Setting PortState to DISABLE (PortID: %d)!",i+1);
                            break;
                        case EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING:
                            PortState = EDD_PORT_STATE_BLOCKING;
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: PDSetDefaultPortState: Setting PortState to BLOCKING (PortID: %d)!",i+1);
                            break;
                        default:
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDDS_PrmCopySetB2A: Invalid PortState: %d",pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortState);
                            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                            EDDS_MODULE_ID,
                                            __LINE__);
                            break;
                    }

                    Old_PortState  = pDDB->pGlob->LinkStatus[i+1].PortState;

                    pDDB->pGlob->LinkStatus[i+1].PortState = (LSA_UINT16) PortState;

                    /* Set the PortState if changed */
                    if (Old_PortState != pDDB->pGlob->LinkStatus[i+1].PortState)
                    {
                        pPrm->PortStates.PortIDState[i] = PortState;
                        pPrm->fireExtLinkChange[i] = LSA_TRUE;    /* only real ports in this array - no +1 required */
                    }

                    pPrm->PortStateSet[i] = LSA_TRUE; /* for later use with MRP! */
                }
            }

            /* copy set B to A */
            pDDB->pGlob->Prm.DefPortStates.RecordSet_A[i] = pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i];

        } /* for */

        /* all port states will be set later - probably we have to set some states for MRP! (see below) */

    }

    /* copy state of B to A */
    pDDB->pGlob->Prm.DefPortStates.Present_A = pDDB->pGlob->Prm.DefPortStates.Present_B;
}

/**
 * Initializes local management of MrpDataAdjust during PRM commit. (synchronous)
 * @param pDDB device handle
 * @return
 */
static LSA_VOID EDDS_PrmCopySetB2A_MrpDataAdjust_Init(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_PRM_PTR_TYPE       pPrm;
    LSA_UINT16 i;
    LSA_UINT16 mode;

    pPrm   = &pDDB->pGlob->Prm;

    /* If MRP is used we set all none MRP-Ports to FORWARDING and all MRP    */
    /* Ports to BLOCKING. */
    /* if only a Interface record is present MRP is disabled! */
    if ( ( pDDB->pGlob->Prm.MRPInterface.Present_B ) &&
         (!pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB))
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                            "EDDS_PrmCopySetB2A: Only Interface MRP-Record present. MRP is disabled!");
        pPrm->MrpDisabled = LSA_TRUE;

        /* enable MRP-MC-forwarding (we have no MRP) */
        mode = EDD_MULTICAST_FWD_ENABLE;
    }
    else
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                            "EDDS_PrmCopySetB2A: Either none or sufficient MRP-records are present. MRP is NOT disabled!");
        pPrm->MrpDisabled = LSA_FALSE;

        /* disable MRP-MC-forwarding (we have MRP) */
        mode = EDD_MULTICAST_FWD_DISABLE;
    }

    /* save port modes for llif call */
    /* we set all Ports */
    for (i = 0; i < pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        pPrm->PortIDMode[i].Mode      = mode;
        pPrm->PortIDMode[i].DstPortID = (LSA_UINT16)(i+1);
        pPrm->PortIDMode[i].SrcPortID = EDD_PORT_ID_AUTO;
    }
}

/**
 * Calls LLIF multicast fowward control for MrpDataAdjust. (asynchronous)
 * @param pDDB device handle
 * @param Mode forwarding mode
 * @param MacAddrGroup MAC address group for LLIF call
 * @return EDD_STS_OK, if LLIF call is done, otherwise EDD_STS_OK_PENDING, EDD_STS_ERR_XXX should not occurs
 */
static LSA_RESULT EDDS_PrmCopySetB2A_MrpDataAdjust_MulticastFwd_LL(
        EDDS_LOCAL_DDB_PTR_TYPE   pDDB,
        const EDD_RQB_PORTID_MODE_TYPE  *pPortIDMode,
        LSA_UINT16                MacAddrGroup)
{
    LSA_RESULT              Status;

    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,
                        LSA_TRACE_LEVEL_NOTE,
                        "==> asynchronous EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(MacAddrGroup: 0x%X, pDDB: 0x%X)",
                        MacAddrGroup,
                        pDDB);

    Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(
                 pDDB,
                 pPortIDMode,
                 (LSA_UINT16)pDDB->pGlob->HWParams.Caps.PortCnt,
                 EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                 MacAddrGroup,
                 0);

    if(EDD_STS_OK_PENDING != Status)
    {
        if(EDD_STS_OK != Status)
        {
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,
                              LSA_TRACE_LEVEL_ERROR,
                              "EDDS_PrmCopySetB2A: EDDS_LL_SWITCH_MULTICAST_FWD_CTRL (MacAddrGroup: 0x%X - Status: %d)",
                              MacAddrGroup,
                              Status);

            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
        }
    }
    return Status;
}

/**
 * Saves the port states of MrpDataAdjust (synchronous).
 * @param pDDB device handle
 * @return
 */
static LSA_VOID EDDS_PrmCopySetB2A_MrpDataAdjust_SavePortStates(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_UINT32 i;

    EDDS_PRM_PTR_TYPE       pPrm;

    pPrm   =                &pDDB->pGlob->Prm;

    /* set PortState for every Port */
    for (i = 0; i < EDDS_MAX_PORT_CNT; i++)
    {
        if(i < pDDB->pGlob->HWParams.Caps.PortCnt )
        {
            LSA_UINT16 PortState;
            LSA_UINT16 Old_PortState;
            /* Three cases: */
            /* 1. no MRP-Record present                    : MRP enabled use DSB setup MRP ports  */
            /* 2. only interface record                    : MRP disabled                         */
            /* 3. at least 2 MRP-Port records/ 1 interface : MRP enabled with this ports          */

            PortState = EDD_PORT_STATE_FORWARDING;

            if ( ! pPrm->MrpDisabled )
            {
                if (pDDB->pGlob->Prm.MRPInterface.Present_B)
                {
                    /* MRP used. Is this a R-Port? (= record present) */
                    if ( pDDB->pGlob->Prm.MRPPort.Present_B[i])
                    {
                        PortState = EDD_PORT_STATE_BLOCKING;
                    }
                    if ( pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.Present_B[i])
                    {
                        PortState = EDD_PORT_STATE_BLOCKING;
                    }
                }
                else
                {
                    /* No MRP-Records (we already checked that no MRPPort records present two)*/
                    /* -> use startup defaults                                                */

                    if ( EDD_MRP_RING_PORT_DEFAULT == pDDB->SWI.MRPRingPort[i] )
                    {
                        PortState = EDD_PORT_STATE_BLOCKING;
                    }
                }
            }

            if ( ! pPrm->PortStateSet[i]) /* set only if not already set by PDSetDefaultPortState !*/
            {
                Old_PortState = pDDB->pGlob->LinkStatus[i+1].PortState;

                if ( PortState == EDD_PORT_STATE_BLOCKING )
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: MRP: Setting PortState to BLOCKING (PortID: %d)!",i+1);
                }
                else
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: MRP: Setting PortState to FORWARDING (PortID: %d)!",i+1);
                }

                pDDB->pGlob->LinkStatus[i+1].PortState = (LSA_UINT16) PortState;

                /* Set the PortState if changed */
                if (Old_PortState != pDDB->pGlob->LinkStatus[i+1].PortState)
                {
                    pPrm->PortStates.PortIDState[i] = PortState;
                    pPrm->fireExtLinkChange[i] = LSA_TRUE;    /* only real ports in this array - no +1 required */
                }
            }

            /* copy set B to A */
            pDDB->pGlob->Prm.MRPPort.Present_A[i] = pDDB->pGlob->Prm.MRPPort.Present_B[i];
            pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.Present_A[i] = pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.Present_B[i];
        }
    } /* for */

    /* copy set B to A */
    pDDB->pGlob->Prm.MRPInterface.Present_A = pDDB->pGlob->Prm.MRPInterface.Present_B;
}

/**
 * Executes prm commit actions of PDXxxMrpDataAdjust. (asynchronous)
 *
 * Includes asynchronous LLIF calls, so the handling is asynchonous.
 * @param pDDB
 * @return EDD_STS_OK if PDXxxMrpDataAdjust handling is ready, otherwise EDD_STS_OK_PENDING (ERR is not allowed)
 */
static LSA_RESULT EDDS_PrmCopySetB2A_MrpDataAdjust(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_PRM_PTR_TYPE       pPrm;
    LSA_RESULT Status;

    Status = EDD_STS_OK_PENDING;
    pPrm   = &pDDB->pGlob->Prm;

    switch(pPrm->currentStatePrmCommitMrp)
    {
        case EDDS_PRM_COMMIT_MRP_INIT_STATE:
        {
            /* initializes mrp structure */
            /* synchronous function */
            EDDS_PrmCopySetB2A_MrpDataAdjust_Init(pDDB);

            pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_1_FWD_STATE_LL;
        }
        //lint -fallthrough

        case EDDS_PRM_COMMIT_MRP_1_FWD_STATE_LL:
        {
            LSA_RESULT resultLL;

            /* set mc fwd ctrl for mrp mac type 1 */
            resultLL = EDDS_PrmCopySetB2A_MrpDataAdjust_MulticastFwd_LL(pDDB,pPrm->PortIDMode,EDD_MAC_ADDR_GROUP_MRP_1);

            if(EDD_STS_OK_PENDING != resultLL)
            {
                /* llif call is ready */
                pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_2_FWD_STATE_LL;
            }
            break;
        }

        case EDDS_PRM_COMMIT_MRP_2_FWD_STATE_LL:
        {
            LSA_RESULT resultLL;

            /* set mc fwd ctrl for mrp mac type 2 */
            resultLL = EDDS_PrmCopySetB2A_MrpDataAdjust_MulticastFwd_LL(pDDB,pPrm->PortIDMode,EDD_MAC_ADDR_GROUP_MRP_2);
            if(EDD_STS_OK_PENDING != resultLL)
            {
                /* llif call is ready */                
                pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_SAVE_PORT_STATES_STATE;
                if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported) {
                    /* only set fwd state if MRPInterconnFwdRulesSupported. */
                    pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_IN_1_FWD_STATE_LL;
                }
            }
            break;
        }

        case EDDS_PRM_COMMIT_MRP_IN_1_FWD_STATE_LL:
        {
            LSA_RESULT resultLL;

            /* set mc fwd ctrl for MRPIn mac type 1 
            Initial FWD state is the same as for MRP if MRP Interconnection is enabled,
			so we can use pPrm->PortIDMode as set in EDDS_PrmCopySetB2A_MrpDataAdjust_Init! */
            resultLL = EDDS_PrmCopySetB2A_MrpDataAdjust_MulticastFwd_LL(pDDB,pPrm->PortIDMode,EDD_MAC_ADDR_GROUP_MRP_IN_1);

            if(EDD_STS_OK_PENDING != resultLL)
            {
                /* llif call is ready */
                pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_IN_2_FWD_STATE_LL;
            }
            break;
        }

        case EDDS_PRM_COMMIT_MRP_IN_2_FWD_STATE_LL:
        {
            LSA_RESULT resultLL;

            /* set mc fwd ctrl for MRPIn mac type 2 */
            resultLL = EDDS_PrmCopySetB2A_MrpDataAdjust_MulticastFwd_LL(pDDB,pPrm->PortIDMode,EDD_MAC_ADDR_GROUP_MRP_IN_2);

            if(EDD_STS_OK_PENDING != resultLL)
            {
                /* llif call is ready */
                pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_SAVE_PORT_STATES_STATE;
            }
            break;
        }

        case EDDS_PRM_COMMIT_MRP_SAVE_PORT_STATES_STATE:
        {
            /* save all port states of mrp */
            /* this is a synchronous function */
            EDDS_PrmCopySetB2A_MrpDataAdjust_SavePortStates(pDDB);

            /* MrpAdjust is ready */
            Status = EDD_STS_OK;
            pPrm->currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_INIT_STATE;
            break;
        }
        default:
        {
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                   "OUT:EDDS_PrmCopySetB2A_MrpDataAdjust - unknown state in edds prm - Status: 0x%X - State: %d",
                                   Status,
                                   pPrm->currentState);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
            break;
        }
    }
    return Status;
}

/**
 * Transfers local saved port states to llif (asynchronous).
 *
 * The portstates are saved during PDSetDefaultPortStates and PDXxxMrpDataAdjust.
 * @param pDDB
 * @return EDD_STS_OK, if LLIF call is done, otherwise EDD_STS_OK_PENDING, EDD_STS_ERR_XXX should not occurs
 */
static LSA_RESULT EDDS_PrmCopySetB2A_SetAllPortStates(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_PRM_PTR_TYPE       pPrm;
    LSA_RESULT Status;

    pPrm   =                &pDDB->pGlob->Prm;
    Status = EDD_STS_OK;

    /* set all PortStates in one call (DEF + MRP) */
    /* For function pointer setSwitchPortState only a PortCnt more than 1 makes sense*/
    if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,setSwitchPortState))
    {
        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_SET_PORT_STATE(pDDB: 0x%X)",pDDB);
        Status = EDDS_LL_SWITCH_SET_PORT_STATE(
                     pDDB,
                     &pPrm->PortStates);
        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_SET_PORT_STATE(pDDB: 0x%X) - Status: %d",pDDB,Status);

        if((EDD_STS_OK != Status) && (EDD_STS_OK_PENDING != Status))
        {
            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,
                              LSA_TRACE_LEVEL_FATAL,
                              "EDDS_PrmCopySetB2A: EDDS_LL_SWITCH_SET_PORT_STATE (Status: %d)",
                              Status);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
        }
    }
    return Status;
}

/**
 * Executes prm commit actions of PDPortDataAdjust record (asynchronous).
 *
 * @param pDDB
 * @return
 */
static LSA_RESULT EDDS_PrmCopySetB2A_PDPort(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_PRM_PTR_TYPE      pPrm;
    LSA_RESULT Status;
    LSA_RESULT StatusStatemachine;

    pPrm   = &pDDB->pGlob->Prm;
    Status = EDD_STS_OK_PENDING;
    StatusStatemachine = EDD_STS_OK_PENDING;

    /* --------------------------------------------------------------------------*/
    /* PDPort Record                                                             */
    /* --------------------------------------------------------------------------*/

    switch(pPrm->currentStatePrmCommitPDPort)
    {
        case EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY:
        {
            /* copy set B to A. B maybe not "present" */
            pPrm->PortData.RecordSet_A[pPrm->currentPort] = pPrm->PortData.RecordSet_B[pPrm->currentPort];

            /* ----------------------------------------------------------------------*/
            /* If a record for a port was written we have to shoot an indication.    */
            /* We set a flag to do so..                                              */
            /* Note: The following has to be extended if more records supported!     */
            /* ----------------------------------------------------------------------*/

           /* force indication */
            pPrm->DoIndicate[pPrm->currentPort+1] = LSA_TRUE;

            /* ----------------------------------------------------------------------*/
            /* MulticastBoundary                                                     */
            /* ----------------------------------------------------------------------*/

            pPrm->PortData.pPortIDModeArray->Mode      = EDD_MULTICAST_FWD_BITMASK ;
            pPrm->PortData.pPortIDModeArray->DstPortID = (LSA_UINT16)(pPrm->currentPort+1);
            pPrm->PortData.pPortIDModeArray->SrcPortID = EDD_PORT_ID_AUTO;

            if (( pPrm->PortData.RecordSet_A[pPrm->currentPort].Present                  ) &&
                ( pPrm->PortData.RecordSet_A[pPrm->currentPort].MulticastBoundaryPresent ))
            {
                /* configure forward. notation is inverse to prm-record (1=forward) */
                pPrm->MACAddrLow =  (~pPrm->PortData.RecordSet_A[pPrm->currentPort].MulticastBoundary);
            }
            else
            {
                /* default if no block present..  forward all */
                pPrm->MACAddrLow =  0xFFFFFFFF;
            }
            /* this is a synchronous state */
            /* so fall through next state -> LL call */
            pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY_LL;
        }
        //lint -fallthrough

        case EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY_LL:
        {
            /* For function pointer controlSwitchMulticastFwd only a PortCnt more than 1 makes sense*/
            if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd) )
            {
                EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X,0x%X,0x%X)",pDDB,EDD_MAC_ADDR_GROUP_RT_2_QUER,pPrm->MACAddrLow);

                Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,
                                                           pPrm->PortData.pPortIDModeArray,
                                                           1,
                                                           EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                                                           EDD_MAC_ADDR_GROUP_RT_2_QUER,
                                                           pPrm->MACAddrLow);

                if( (EDD_STS_OK_PENDING != Status) && (EDD_STS_OK != Status) )
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                           "EDDS_LL_SWITCH_MULTICAST_FWD_CTRL returned with status %d", Status);
                }
            }
            else
            {
                /* no LL call nessecary -> no PENDING */
                Status = EDD_STS_OK;
            }

            /* state is ready */
            if(EDD_STS_OK_PENDING != Status)
            {
                pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY;
            }

            break;
        }
        case EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY:
        {
            /* ----------------------------------------------------------------------*/
            /* DCPBoundary                                                           */
            /* ----------------------------------------------------------------------*/

            pPrm->PortData.pPortIDModeArray->Mode      = EDD_MULTICAST_FWD_BITMASK ;
            pPrm->PortData.pPortIDModeArray->DstPortID = (LSA_UINT16)(pPrm->currentPort+1);
            pPrm->PortData.pPortIDModeArray->SrcPortID = EDD_PORT_ID_AUTO;

            if (( pPrm->PortData.RecordSet_A[pPrm->currentPort].Present                  ) &&
                ( pPrm->PortData.RecordSet_A[pPrm->currentPort].DCPBoundaryPresent       ))
            {
                /* configure forward. notation is inverse to prm-record (1=forward) */
                pPrm->MACAddrLow =  (~pPrm->PortData.RecordSet_A[pPrm->currentPort].DCPBoundary);
            }
            else
            {
                /* default if no block present..  forward all */
                pPrm->MACAddrLow =  0xFFFFFFFF;
            }

            /* this is a synchronous state */
            /* so fall through next state -> LL call */
            pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY_LL;
        }
        //lint -fallthrough

        case EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY_LL:
        {
            /* For function pointer controlSwitchMulticastFwd only a PortCnt more than 1 makes sense*/
            if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd) )
            {
                EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X,0x%X,0x%X)",pDDB,EDD_MAC_ADDR_GROUP_DCP,pPrm->MACAddrLow);

                Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,
                                                           pPrm->PortData.pPortIDModeArray,
                                                           1,
                                                           EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                                                           EDD_MAC_ADDR_GROUP_DCP,
                                                           pPrm->MACAddrLow);

                if( (EDD_STS_OK_PENDING != Status) && (EDD_STS_OK != Status) )
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                           "EDDS_LL_SWITCH_MULTICAST_FWD_CTRL returned with status %d", Status);
                }
            }
            else
            {
                /* no LL call nessecary -> no PENDING */
                Status = EDD_STS_OK;
            }

            /* state is ready */
            if(EDD_STS_OK_PENDING != Status)
            {
                pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_NO_MAUTYPE;
            }

            break;
        }

        case EDDS_PRM_COMMIT_PDPORT_STATE_NO_MAUTYPE:
        {
            /* LL call is conditional -> so set status to OK */
            /* if LL call is pending it will overwrite it */
            Status = EDD_STS_OK;

            /* ----------------------------------------------------------------------*/
            /* PortState and MAUType                                                 */
            /* ----------------------------------------------------------------------*/

            /* if no PortStatePresent and no MAUTypePresent -> Autoneg and PowerON */
            /* this is equal to empty record!!! */
            if (( ! pPrm->PortData.RecordSet_A[pPrm->currentPort].PortStatePresent ) &&
                ( ! pPrm->PortData.RecordSet_A[pPrm->currentPort].MAUTypePresent   ))
            {
                LSA_UINT8 PHYPower = (pDDB->pGlob->LinkStatus[pPrm->currentPort + 1].IsPulled == EDD_PORTMODULE_IS_PULLED) ? EDDS_PHY_POWER_OFF : EDDS_PHY_POWER_ON;
                LSA_UINT8 LinkSpeedMode;
                
                if(EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->pGlob->PrmChangePort[pPrm->currentPort].ApplyDefaultPortparams)
                {
                    LinkSpeedMode = EDD_LINK_AUTONEG;
                }
                else
                {
                    LinkSpeedMode = EDD_LINK_AUTONEG;
                }

                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> asynchronous EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    pPrm->currentPort+1,
                                    LinkSpeedMode,
                                    PHYPower);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,pPrm->currentPort+1,&LinkSpeedMode,&PHYPower);

                if (EDD_STS_OK == Status)
                {

                    /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. Typically this configuration is identically to the PDPortDataAdjust */
                    /*       parameterization, but the "AutoNeg"-configuration (=default) maybe mapped to a fixed setting if autoneg is not supported.    */
                    /*       Typically the case with POF ports because autoneg is not supported with POF. The actual autoneg to fixed mapping depends on  */
                    /*       phy adaption by LL.                                                                                                          */

                    pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[pPrm->currentPort] = LinkSpeedMode;
                    pDDB->pGlob->HWParams.hardwareParams.PHYPower[pPrm->currentPort]      = PHYPower;
                }
                else if (EDD_STS_OK_PENDING != Status)
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                }
            }

            /* state is ready */
            if(EDD_STS_OK_PENDING != Status)
            {
                pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_OFF;
            }
            break;
        }

        case EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_OFF:
        {
            /* LL call is conditional -> so set status to OK */
            /* if LL call is pending it will overwrite it */
            Status = EDD_STS_OK;

            /* if PortStatePresent only DOWN allowed. Already checked */
            if (( pPrm->PortData.RecordSet_A[pPrm->currentPort].PortStatePresent ) &&
                ( pPrm->PortData.RecordSet_A[pPrm->currentPort].PortState == EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN )) /* should be ! */
            {
                LSA_UINT8 PHYPower = EDDS_PHY_POWER_OFF;
                LSA_UINT8 LinkSpeedMode = EDD_LINK_UNCHANGED;

                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> asynchronous EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    pPrm->currentPort+1,
                                    LinkSpeedMode,
                                    PHYPower);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,pPrm->currentPort+1,&LinkSpeedMode,&PHYPower);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[pPrm->currentPort] = LinkSpeedMode;
                    pDDB->pGlob->HWParams.hardwareParams.PHYPower[pPrm->currentPort]      = PHYPower;
                }
                else if(EDD_STS_OK_PENDING != Status)
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                }
            }

            /* state is ready */
            if(EDD_STS_OK_PENDING != Status)
            {
                pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_ON;
            }
            break;
        }

        case EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_ON:
        {
            /* LL call is conditional -> so set status to OK */
            /* if LL call is pending it will overwrite it */
            Status = EDD_STS_OK;

            if ( pPrm->PortData.RecordSet_A[pPrm->currentPort].MAUTypePresent )
            {
                LSA_UINT8 PHYPower;
                LSA_UINT8 LinkSpeedMode;
                                
                if(EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->pGlob->PrmChangePort[pPrm->currentPort].ApplyDefaultPortparams)
                {
                    LinkSpeedMode = EDD_LINK_AUTONEG;
                }
                else
                {
                    LinkSpeedMode = pPrm->PortData.RecordSet_A[pPrm->currentPort].LinkStat;
                }


                PHYPower = (pDDB->pGlob->LinkStatus[pPrm->currentPort + 1].IsPulled == EDD_PORTMODULE_IS_PULLED) ? EDDS_PHY_POWER_OFF : EDDS_PHY_POWER_ON;

                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> asynchronous EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    pPrm->currentPort+1,
                                    LinkSpeedMode,
                                    PHYPower);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,pPrm->currentPort+1,&LinkSpeedMode,&PHYPower);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. Typically this configuration is identically to the PDPortDataAdjust */
                    /*       parameterization, but the "AutoNeg"-configuration (=default) maybe mapped to a fixed setting if autoneg is not supported.    */
                    /*       Typically the case with POF ports because autoneg is not supported with POF. The actual autoneg to fixed mapping depends on  */
                    /*       phy adaption by LL.                                                                                                          */

                    pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[pPrm->currentPort]  = LinkSpeedMode;
                    pDDB->pGlob->HWParams.hardwareParams.PHYPower[pPrm->currentPort]       = PHYPower;
                }
                else if(EDD_STS_OK_PENDING != Status)
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                }
            }

            /* state is ready */
            if(EDD_STS_OK_PENDING != Status)
            {
                pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_FINISH_PORT;
            }
            break;
        }

        case EDDS_PRM_COMMIT_PDPORT_STATE_FINISH_PORT:
        {
            LSA_UINT8              PhyStatus;
            LSA_UINT8              Autoneg;
            /* Setup PhyStatus */
            if ( !pPrm->PortData.RecordSet_A[pPrm->currentPort].PortStatePresent )
            {
                PhyStatus = EDD_PHY_STATUS_ENABLED;
            }
            else
            {
                PhyStatus = EDD_PHY_STATUS_DISABLED;
            }

            /* check if Autoneg setting or PhyStatus changed. if so store and do a ext    */
            /* link indication                                                            */
            /* Note: Autoneg and Status/Mode not matching! Autoneg is only the configured!*/
            /*       setting!                                                             */

            Autoneg = EDD_AUTONEG_OFF;
            if ( pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[pPrm->currentPort] == EDD_LINK_AUTONEG ) Autoneg = EDD_AUTONEG_ON;

            /* Note: it is possible that the PhyStatus is no DISABELD but the link is still */
            /*       up. This must be taken into account by application. PhyStatus is the   */
            /*       configured value! the actual linkstate maybe changed later.            */

            if ((pDDB->pGlob->LinkStatus[pPrm->currentPort+1].Autoneg != Autoneg     ) ||
                (pDDB->pGlob->LinkStatus[pPrm->currentPort+1].PhyStatus != PhyStatus ))/* change ? */
            {
                /* Store configured Autoneg setting */
                pDDB->pGlob->LinkStatus[pPrm->currentPort+1].Autoneg = Autoneg;
                pDDB->pGlob->LinkStatus[pPrm->currentPort+1].PhyStatus  = PhyStatus;
                pPrm->fireExtLinkChange[pPrm->currentPort] = LSA_TRUE;    /* only real ports in this array - no +1 required */
            }

            /* reset to start */
            pPrm->currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY;

            /* we are ready for one port so increment port */
            if(pPrm->currentPort < pDDB->pGlob->HWParams.Caps.PortCnt-1) //IF does not count
            {
                ++pPrm->currentPort;
            }
            else
            {
                /* we are ready for all ports */
                /* so state machine is ready */
                pPrm->currentPort = 0;
                StatusStatemachine = EDD_STS_OK;
            }

            break;
        }
        default:
        {
            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                   "OUT:EDDS_PrmCopySetB2A - prohibited cycle in scheduler (intern) - case: %d",
                                   pPrm->currentStatePrmCommitPDPort);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
            break;
        }
    }
    return StatusStatemachine;
}

/**
 * Finalizes a PRM commit in RQB context.
 *
 * Executes all actions of rqb context if prm commit is finished. \n
 * Actualice the PRM state. \n
 * Fires LinkIndications.
 * @param pDDB
 * @return
 */
LSA_VOID EDDS_PrmCommit_Finish(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_RESULT	           Status;
    LSA_UINT32             i;
    LSA_BOOL*              fireExtLinkChange;
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm   = &pDDB->pGlob->Prm;
    fireExtLinkChange = pPrm->fireExtLinkChange;

    /* in this last loop, check if a port was changed */
    for (i = 0; i < EDDS_MAX_PORT_CNT; i++)
    {
        /* array is only set by scheduler during PrmCommit */
        /* And PrmCommit of scheduler is ready */
        /* so no lock required */
        if(fireExtLinkChange[i])    /* only real ports in this array - no +1 required */
        {
            /* fire ExtLinkChangeIndication */
            EDDS_IndicateExtLinkChange(pDDB,i+1);
        }
    }

    pPrm->FirstCommitExecuted = LSA_TRUE;  /* at least the first COMMIT is executed */

    EDDS_PrmSetBToNotPresent(pDDB);

    Status = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_PRM_COMMIT_FINISH);

    if(EDD_STS_OK != Status)
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
            "EDDS_PrmCommit_Finish()");
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
    }
}

/**
 * Finalizes PRM commit in Scheduler context.
 * @param pDDB
 * @return
 */
static LSA_VOID EDDS_PrmCopySetB2A_FinishScheduler(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm   = &pDDB->pGlob->Prm;

    if (pPrm->PDSCFDataCheck.RecordSet_B.Present)
    {
        pPrm->PDSCFDataCheck.RecordSet_A = pPrm->PDSCFDataCheck.RecordSet_B;
        pPrm->PDSCFDataCheck.RecordSet_B.Present = LSA_FALSE;
        pPrm->PDSCFDataCheck.RecordSet_B.RecordLength = 0;
        pPrm->PDSCFDataCheck.RecordSet_B.SCF = 0;
        EDDS_MEMSET_LOCAL(pPrm->PDSCFDataCheck.RecordSet_B.Record, 0, sizeof(pPrm->PDSCFDataCheck.RecordSet_B.Record));
    }
}

/**
 * Executes the prm commit action in scheduler context. (asynchronous)
 * @param pDDB
 * @return EDD_STS_OK, if LLIF call is done, otherwise EDD_STS_OK_PENDING, EDD_STS_ERR_XXX should not occurs
 */
//lint -esym(550,PortStateSet)
//JB 11/11/2014 used when:  //#JB_TODO -> above
//JB 11/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_RESULT EDDS_PrmCopySetB2A(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    EDDS_PRM_PTR_TYPE      pPrm;
    LSA_RESULT             Status;

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmCopySetB2A(pDDB: 0x%X)",
                           pDDB);

    Status = EDD_STS_OK_PENDING;
    pPrm   = &pDDB->pGlob->Prm;

    switch(pPrm->currentState)
    {
        case EDDS_PRM_COMMIT_STATE_SYNCHRONOUS:
        {
            EDDS_PrmCopySetB2A_PDNRTFeedInLoadLimitation(pDDB);
            EDDS_PrmCopySetB2A_PDNC(pDDB);
            EDDS_PrmCopySetB2A_PDSetDefaultPortStates(pDDB);

            /* these are synchronous functions */
            /* so we could start with next state immediately -> fallthrough */
            pPrm->currentState = EDDS_PRM_COMMIT_STATE_MRP;
        }
        //lint -fallthrough

        case EDDS_PRM_COMMIT_STATE_MRP:
        {
            /* MrpDataAdjust is only handled on first commit and if mrp is used */
            /* else fallthrough */
            if(!pPrm->FirstCommitExecuted && (0 != pDDB->SWI.MaxMRP_Instances))
            {
                LSA_RESULT statusSubstate;

                /* --------------------------------------------------------------------------*/
                /* PDInterfaceMrpDataAdjust, PDPortMrpDataAdjust                             */
                /* Note: Only done on first COMMIT !                                         */
                /* --------------------------------------------------------------------------*/
                statusSubstate = EDDS_PrmCopySetB2A_MrpDataAdjust(pDDB);

                if( EDD_STS_OK_PENDING != statusSubstate)
                {
                    /* MrpDataAdjust_Part1 is ready */
                    pPrm->currentState = EDDS_PRM_COMMIT_STATE_SETPORT;
                }
                break;
            }
            else
            {
                pPrm->currentState = EDDS_PRM_COMMIT_STATE_SETPORT;
            }
        }
        //lint -fallthrough

        case EDDS_PRM_COMMIT_STATE_SETPORT:
        {
            LSA_RESULT statusSubstate;

            /* transfer previous saved port states to llif */
            /* default and/or mrp port states */
            statusSubstate = EDDS_PrmCopySetB2A_SetAllPortStates(pDDB);

            if( EDD_STS_OK_PENDING != statusSubstate)
            {
                /* transfer port states to lllif is ready */
                pPrm->currentState = EDDS_PRM_COMMIT_STATE_PDPORT;
            }
            break;
        }

        case EDDS_PRM_COMMIT_STATE_PDPORT:
        {
            LSA_RESULT statusSubstate;

            statusSubstate = EDDS_PrmCopySetB2A_PDPort(pDDB);

            if( EDD_STS_OK_PENDING != statusSubstate)
            {
                /* PDPort is ready -> CopyB2A is ready */
                pPrm->currentState = EDDS_PRM_COMMIT_STATE_SYNCHRONOUS;

                EDDS_PrmCopySetB2A_FinishScheduler(pDDB);

                /* return OK to signal we are ready */
                Status = EDD_STS_OK;
            }
            break;
        }
        default:
        {
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                   "OUT:EDDS_PrmCopySetB2A - prohibited cycle in edds prm - Status: 0x%X - State: %d",
                                   Status,
                                   pPrm->currentState);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
            break;
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmCopySetB2A() - cycle: %d",
                           pPrm->currentState);

    return Status;
}
//lint +esym(550,PortStateSet)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmIndicatePort                        +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  PortID     : PortID to indicate for (0=interface, 1.. EDDS_MAX_PORT_CNT)+*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called to check if a Diag has to be indicated for PortID  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use it with pDDB as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmIndicatePort(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT16                  PortID)
{

    LSA_RESULT                                Status;
    EDD_UPPER_RQB_PTR_TYPE                    pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE         pRQBParam;
    EDDS_PRM_PTR_TYPE                         pPrm;
    EDDS_LOCAL_HDB_PTR_TYPE                   pHDB;
    LSA_UINT32                                Cnt;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmIndicatePort(pDDB: 0x%X,PortID: %d)",
                           pDDB,
                           PortID);

    pPrm       = &pDDB->pGlob->Prm;
    Cnt        = 0;

    Status = EDDS_HandleGetHDB(pPrm->LowerHandle, &pHDB);

    if ( Status != EDD_STS_OK )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    pRQB = pPrm->PrmIndReq[PortID].pBottom;

    if ( ! ( LSA_HOST_PTR_ARE_EQUAL( pRQB, LSA_NULL) ))  /* Indication Resource present ! */
    {
        pRQBParam = (EDD_UPPER_PRM_INDICATION_PTR_TYPE) pRQB->pParam;

        switch (PortID)
        {
            case 0:  /* interface */
                /* --------------------------------------------------------------*/
                /* Check PDNC (frame drop)                                       */
                /* --------------------------------------------------------------*/

                /* enabled ? */
                if ( pPrm->FrameDrop.CheckEnable )
                {
                    if ( pPrm->FrameDrop.ErrorStatus != pPrm->FrameDrop.ErrorStatusIndicated )
                    {
                        if ( pPrm->FrameDrop.ErrorStatusIndicated != EDDS_DIAG_FRAME_DROP_OK )
                        {
                            /* disappear */
                            EDDS_SetupPrmIndEntry(&pRQBParam->diag[0],
                                                 pPrm->FrameDrop.ErrorStatusIndicated,
                                                 LSA_FALSE);
                            Cnt++;
                        }

                        if ( pPrm->FrameDrop.ErrorStatus != EDDS_DIAG_FRAME_DROP_OK )
                        {
                            /* appear */
                            EDDS_SetupPrmIndEntry(&pRQBParam->diag[Cnt],
                                                 pPrm->FrameDrop.ErrorStatus,
                                                 LSA_TRUE);
                            Cnt++;
                        }

                        pPrm->FrameDrop.ErrorStatusIndicated = pPrm->FrameDrop.ErrorStatus;
                    }
                }

                break;
            default: /* ports */
                /* --------------------------------------------------------------*/
                /* Check PDPort (nothing to indicate at all)                     */
                /* --------------------------------------------------------------*/
                break;
        }

        if (Cnt > EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES )
        {
            /* Cnt exceeds space within RQB! */
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                            EDDS_MODULE_ID,
                            __LINE__);
        }

        /* If DoIndicate is TRUE we have to indicate if Cnt = 0 */
        /* this is after a COMMIT with all ports a record was   */
        /* written.                                             */

        if (( Cnt > 0 ) || (pPrm->DoIndicate[PortID] ))
        {
            EDDS_RQB_REM_BLOCK_BOTTOM(pPrm->PrmIndReq[PortID].pBottom,
                                      pPrm->PrmIndReq[PortID].pTop,
                                      pRQB);

            if ( LSA_HOST_PTR_ARE_EQUAL( pRQB, LSA_NULL) )
            {
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                EDDS_MODULE_ID,
                                __LINE__);
            }

            pRQBParam->diag_cnt      = (LSA_UINT16) Cnt;
            pRQBParam->edd_port_id   = PortID;

            pPrm->DoIndicate[PortID] = LSA_FALSE;

            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_PrmIndicatePort: Indicate for PortID %d. Entrys: %d",
                                  PortID,
                                  Cnt);

            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
        }
    }
    else
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDS_PrmIndicatePort: No Indication resources present.");
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmIndicatePort()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmIndicateAll                         +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called to check if a Diag has to be indicated for any Port+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use it with pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmIndicateAll(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT16    i;

    for (i=0; i<=pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        EDDS_PrmIndicatePort(pDDB,i); /* Note: 0=Interface */
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagFrameDropHandler                   +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT32                  DropCnt         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  DropCnt    : Dropped frames till last call                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called with DropCnt of frames.                            +*/
/*+               Checks for current ErrorStatus and indicates if changed   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropHandler(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt)
{
    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagFrameDropHandler(pDDB: 0x%X, DropCnt: %d)",
                           pDDB,
                           DropCnt);

    if (pDDB->pGlob->Prm.FrameDrop.CheckEnable )
    {
        EDDS_DiagFrameDropUpdate(pDDB,DropCnt);
        EDDS_PrmIndicatePort(pDDB,0); /* FrameDrop is on Interface */
    }

    pDDB->pGlob->Prm.FrameDrop.DropCnt = DropCnt;  /* always save actual drop cnt */


    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagFrameDropHandler()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmValidateConsistency                 +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_BOOL                    Local           +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Local      : LSA_TRUE : Local  parametration                           +*/
/*+               LSA_FALSE: Remote parametration                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_CONSISTENCY                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks is all record set Bs are consistent.               +*/
/*+                                                                         +*/
/*+               PDPortDataAdjust: At least one PortState has to be UP     +*/
/*+                                 with Autoneg or with Mautype 100Mbit/   +*/
/*+                                 1Gbit/10Gbit and fullduplex.            +*/
/*                                  if Local = LSA_FALSE                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmValidateConsistency(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_PRM_END_PTR_TYPE  pPrmEnd)
{

    LSA_RESULT                               Status;
    LSA_UINT32                               i;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE pRecordSetB;
    LSA_BOOL                                 Valid;
    LSA_UINT32                               CntPowerOn, CntSpeedMode;
    LSA_UINT32                               CntMrpPorts;
    LSA_BOOL                                 MrpDisabled = LSA_TRUE;
    LSA_UINT8                    			 Local;
    LSA_RESULT 								 validMautype;

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmValidateConsistency(pDDB: 0x%X)",
                           pDDB);

    Local = pPrmEnd->Local;
    Status = EDD_STS_OK;
    Valid  = LSA_FALSE;
    validMautype = EDD_STS_OK;
    i      = 0;

    /* For PDPortData we have to check if at least one Port remains in UP State */

    #ifdef EDDS_PRM_CHECK_ONE_PORT_UP
    CntPowerOn   = 0;
    CntSpeedMode = 0;

    // we have to check all ports because we set PortparamsNotApplicable port specific
    while ((i< pDDB->pGlob->HWParams.Caps.PortCnt) /*&& ( ! Valid )*/)
    {
        pRecordSetB = &pDDB->pGlob->Prm.PortData.RecordSet_B[i];

        if (   (EDD_PRM_PORT_IS_MODULAR != pPrmEnd->IsModularPort[i])
            && (EDD_PRM_PORT_IS_COMPACT != pPrmEnd->IsModularPort[i]))
        {
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_PrmValidateConsistency: illegal value for IsModular (%d) at UsrPOrtIndex %d!", pPrmEnd->IsModularPort[i], i);
            Status = EDD_STS_ERR_PARAM;
        }
        else if(   (EDD_PORTMODULE_IS_PULLED == pDDB->pGlob->LinkStatus[i+1].IsPulled)
                && (EDD_PRM_PORT_IS_MODULAR != pPrmEnd->IsModularPort[i])
          )
        {
            EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST,
                pDDB->pGlob->Prm.PrmDetailErr.RecordOffset_MauType,
                EDD_PRM_ERR_FAULT_DEFAULT);
            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_PrmValidateConsistency, NON-modular Port is PULLED: Port:%d", i);
            Status = EDD_STS_ERR_PRM_CONSISTENCY;
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                EDDS_MODULE_ID,
                __LINE__);
        }
        else
        {
            if(EDD_PORTMODULE_IS_PULLED == pDDB->pGlob->LinkStatus[i+1].IsPulled)
            {
                pPrmEnd->PortparamsNotApplicable[i] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
            }
            else
            {
                pPrmEnd->PortparamsNotApplicable[i] = EDD_PRM_PORT_PARAMS_APPLICABLE;
            }
            if ( pRecordSetB->Present )
            {
                /* if PortState is present it is "DOWN" (OFF)   */
                /* If not we have a MAUType or Default which is */
                /* both UP                                      */

                if ( ! pRecordSetB->PortStatePresent ) /* != OFF */
                {
                    /* PowerOn if PortState not exist */
                    CntPowerOn++;

                    if ( pRecordSetB->MAUTypePresent ) /* fixed MAU-Type */
                    {
                        LSA_UINT8  LinkState;
                        /* check if MAUType supported by LLIF and translate MAUType into speed/mode */

                        EDDS_CHECK_AND_DECODE_MAUTYPE(&validMautype,pDDB->hSysDev,pDDB->hDDB,(i+1),pRecordSetB->MAUType,&LinkState);
                        if(  (EDD_STS_ERR_PARAM == validMautype)
                          && (EDD_PRM_PORT_IS_MODULAR == pPrmEnd->IsModularPort[i])
                          )
                        {
                            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                "EDDS_PrmValidateConsistency: MAUType (%d) not supported for Port(%d) by LLIF!",pRecordSetB->MAUType, i);

                            Valid = LSA_TRUE;
                            pPrmEnd->PortparamsNotApplicable[i] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                        }
                        else if(EDD_STS_OK != validMautype)
                        {
                            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_WARN,
                                "EDDS_PrmValidateConsistency: MAUType (%d) not supported for Port(%d) by LLIF!",pRecordSetB->MAUType, i);
                            EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST,
                                pDDB->pGlob->Prm.PrmDetailErr.RecordOffset_MauType,
                                EDD_PRM_ERR_FAULT_DEFAULT);
                            pPrmEnd->PortparamsNotApplicable[i] = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
                            Status = EDD_STS_ERR_PRM_CONSISTENCY;
                            break;
                        }
                        else
                        {
                            /* save LinkState for later use */
                            pRecordSetB->LinkStat = LinkState;


                            if(!pDDB->pGlob->Prm.PortData.LesserCheckQuality)
                            {
                                /* Valid only TRUE if 100MBit full */
                                if(EDD_LINK_100MBIT_FULL    == pRecordSetB->LinkStat)
                                {
                                    CntSpeedMode++;
                                }
                            }
                            else
                            {
                                /* Valid only TRUE if 100MBit/1Gbit/10Gbit full */
                                if(  (EDD_LINK_100MBIT_FULL == pRecordSetB->LinkStat)
                                  || (EDD_LINK_1GBIT_FULL   == pRecordSetB->LinkStat)
                                  || (EDD_LINK_10GBIT_FULL  == pRecordSetB->LinkStat)
                                  )
                                {
                                    CntSpeedMode++;
                                }
                            }
                        }
                    }
                    else  /* no MAUType -> Autoneg */
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
        }
        i++;
    }
//    if(!Valid)
//    {
//        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
//            "EDDS_PrmValidateConsistency: PDPortDataAdjust. No Mautype found that is applicable!");
//        EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
//            EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST,
//            EDD_PRM_ERR_OFFSET_DEFAULT,
//            EDD_PRM_ERR_FAULT_DEFAULT);
//        Status = EDD_STS_ERR_PRM_CONSISTENCY;
//    }

    if(EDD_PRM_PARAMS_ASSIGNED_REMOTE == Local)
    {
        /* at least one port must be poweron */
        /* at least one port must be >= 100 Mbit */
        if ( (0 == CntPowerOn || 0 == CntSpeedMode) && (!Valid) )
        {
            /* if there is no consistency error */
            if( EDD_STS_ERR_PRM_CONSISTENCY != Status)
            {
                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "EDDS_PrmValidateConsistency: PDPortDataAdjust. All ports DOWN, 10Mbit or halfduplex not allowed!");

                EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                    EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST,
                    EDD_PRM_ERR_OFFSET_DEFAULT,
                    EDD_PRM_ERR_FAULT_DEFAULT);

                Status = EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
    }
    #endif

    if ( Status == EDD_STS_OK)
    {

        /* --------------------------------------------------------------------------*/
        /* PDPortMrpDataAjdust/PDInterfaceMrpDataAdjust Record                       */
        /* --------------------------------------------------------------------------*/

        CntMrpPorts = 0;
        /* only 0 or two PDPortMrpDataAjdust are allowed */
        for (i = 0; i< pDDB->pGlob->HWParams.Caps.PortCnt; i++)
        {
            /* we only need the info the a write was done! */
            if ( pDDB->pGlob->Prm.MRPPort.Present_B[i] )
            {
                CntMrpPorts++;
            }
        }

        /* If no MRP was configured on EDDS startup we dont allow */
        /* MRP-Records to be written!                             */

        MrpDisabled = LSA_FALSE;
        if ( 0 == pDDB->SWI.MaxMRP_Instances )
        {
            if ( (CntMrpPorts) ||
                 (pDDB->pGlob->Prm.MRPInterface.Present_B))
            {
                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "EDDS_PrmValidateConsistency: MRP record(s) present, but MRP not configured at EDDS!");

                /* MRP records without MRP being configured at startup */
                EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                    EDD_PRM_ERR_INDEX_DEFAULT,
                    EDD_PRM_ERR_OFFSET_DEFAULT,
                    EDD_PRM_ERR_FAULT_DEFAULT);

                Status = EDD_STS_ERR_PRM_CONSISTENCY;
            }
            MrpDisabled = LSA_TRUE;
        }

        /* three cases (we only check for record presence!):            */
        /* no PDPortMrpDataAdjust record  + PDInterfaceMrpDataAdjust    */
        /* 2  PDPortMrpDataAdjust records + PDInterfaceMrpDataAdjust    */
        /* no records at all.                                           */

        /* Note that MRP records are only allowed if MRP is configured  */
        /* within EDDS!                                                 */

        if ( Status == EDD_STS_OK )
        {
            if(0 == CntMrpPorts)
            {
                if (pDDB->pGlob->Prm.MRPInterface.Present_B)
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                        "EDDS_PrmValidateConsistency: Only Interface MRP-Record present. MRP disabled.");
                    MrpDisabled = LSA_TRUE;
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                        "EDDS_PrmValidateConsistency: No MRP-Records present.");
                }

                pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB = LSA_FALSE;
            }
            else if( (CntMrpPorts >= 2) &&
                     (CntMrpPorts <= EDDS_MAX_PORT_CNT))
            {
                if (! pDDB->pGlob->Prm.MRPInterface.Present_B )
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmValidateConsistency: Interface MRP-Record missing!");

                    /* PDPortMrpDataAdjust written without PDInterfaceMrpDataAdjust written */
                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                            EDDS_PRM_RECORD_INDEX_PDINTERFACE_MRP_DATA_ADJUST,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            LSA_FALSE);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                        "EDDS_PrmValidateConsistency: Interface MRP-Record and at least 2 port MRP-Records present.");
                    pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB = LSA_TRUE;
                }
            }
            else
            {
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_PrmValidateConsistency: Invalid default ring port count (%d)!",CntMrpPorts);

                /* not enough PDPortMrpDataAdjust records written */
                EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                        EDDS_PRM_RECORD_INDEX_PDPORT_MRP_DATA_ADJUST,
                        EDD_PRM_ERR_OFFSET_DEFAULT,
                        LSA_FALSE);

                Status = EDD_STS_ERR_PRM_CONSISTENCY;
            }
        } /* if */
    }

    if ( Status == EDD_STS_OK)
    {
        /* --------------------------------------------------------------------------*/
        /* PDSetDefaultPortStates                                                    */
        /* --------------------------------------------------------------------------*/

        /* Only ports not currently a R-Port with MRP are allowed to be set ! */
        if ( pDDB->pGlob->Prm.DefPortStates.Present_B && (! MrpDisabled))
        {
            /* check for overlapping R-ports if MRP is enabled */
            for (i = 0; i< pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
                LSA_BOOL   RPort = LSA_FALSE;

                if ( pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB ) /* R-Ports from PRM */
                {
                    if ( pDDB->pGlob->Prm.MRPPort.Present_B[i])
                    {
                        RPort = LSA_TRUE;
                    }
                }
                else /* R-Ports from Setup */
                {
                    if (EDD_MRP_RING_PORT_DEFAULT == pDDB->SWI.MRPRingPort[i] )
                    {
                        RPort = LSA_TRUE;
                    }
                }

                /* check if this port is set with PDSetDefaultPortStates and is a Rport -> not allowed */
                if ( pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortStatePresent && RPort )
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmValidateConsistency: PDSetDefaultStates overlapping with MRP-R-Port! (PortId: %d)",i+1);

                    /* PDSetDefaultPortState written for a MRP port */
                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                            i+1,
                            EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            EDD_PRM_ERR_FAULT_DEFAULT);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }

                /* check if this port is set with PDSetDefaultPortStates and is an Interconnection port -> not allowed */
                if ( pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortStatePresent &&  pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.Present_B[i] )
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmValidateConsistency: PDSetDefaultStates overlapping with MRP-IN-Port! (PortId: %d)",i+1);

                    /* PDSetDefaultPortState written for an MRP Interconnection port */
                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                            i+1,
                            EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            EDD_PRM_ERR_FAULT_DEFAULT);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }

            } /* for */
        } /* if */
    }
    if (Status == EDD_STS_OK)
    {
        /* --------------------------------------------------------------------------*/
        /* PDPortMrpIcDataAdjust (MRP Interconnection)                               */
        /* --------------------------------------------------------------------------*/

        if(pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB && MrpDisabled) {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                              "EDDS_PrmValidateConsistency: MRPIn enabled but MRP disabled");

            EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                    i+1,
                    EDDS_PRM_RECORD_INDEX_PDPORT_MRP_IC_DATA_ADJUST,
                    EDD_PRM_ERR_OFFSET_DEFAULT,
                    EDD_PRM_ERR_FAULT_DEFAULT);
            Status = EDD_STS_ERR_PRM_CONSISTENCY;
        } else if(pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB &&
                  EDD_FEATURE_DISABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported) {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                              "EDDS_PrmValidateConsistency: MRPIn record present but FeatureSupport.MRPInterconnFwdRulesSupported disabled");

            EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                    i+1,
                    EDDS_PRM_RECORD_INDEX_PDPORT_MRP_IC_DATA_ADJUST,
                    EDD_PRM_ERR_OFFSET_DEFAULT,
                    EDD_PRM_ERR_FAULT_DEFAULT);
            Status = EDD_STS_ERR_PRM_CONSISTENCY;
        } else if(pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB &&
                  (! pDDB->pGlob->Prm.MRPInterface.Present_B )) {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                              "EDDS_PrmValidateConsistency: MRPIn record present but MRP is set to defaults (no PDInterfaceMrpDataAdjust record)");

            EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                    i+1,
                    EDDS_PRM_RECORD_INDEX_PDPORT_MRP_IC_DATA_ADJUST,
                    EDD_PRM_ERR_OFFSET_DEFAULT,
                    EDD_PRM_ERR_FAULT_DEFAULT);
            Status = EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }
    /* IO_Configured=NO is refused when consumer and/or provider currently are in use */
    if( pDDB->pGlob->IsSrtUsed )
    {
        if (Status == EDD_STS_OK)
        {
            EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp = pDDB->pSRT;
            EDDS_PRM_PDNRT_FILL_DATA_PTR_TYPE pFill = &pDDB->pGlob->Prm.PDNRTFill;

            if (pFill->RecordSet_B.Present)
            {

                if((EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_NOT_CONFIGURED == pFill->RecordSet_B.IO_Configured) &&
                        (pCSRTComp->Cons.ConsumerActCnt || pCSRTComp->Prov.ProviderActCnt))
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                      "EDDS_PrmValidateConsistency: PDNRTFeedInLoadLimitation: IO_Configured=NO invalid with activated consumer and/or providers!");

                    /* IO_Configured=NO and consumers/providers are in use */
                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                            EDD_PRM_ERR_PORT_ID_DEFAULT,
                            EDDS_PRM_RECORD_INDEX_PDNRT_FEED_IN_LOAD_LIMITATION,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            EDD_PRM_ERR_FAULT_DEFAULT);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
            else
            {
                /* check IO_Configured default values */
                if((EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_NOT_CONFIGURED == pDDB->pGlob->IO_ConfiguredDefault) &&
                        (pCSRTComp->Cons.ConsumerActCnt || pCSRTComp->Prov.ProviderActCnt))
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmValidateConsistency: PDNRTFeedInLoadLimitation: IO_Configured=NO invalid with activated consumer and/or providers!");

                    /* IO_Configured=NO and consumers/providers are in use */
                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                            EDD_PRM_ERR_PORT_ID_DEFAULT,
                            EDDS_PRM_RECORD_INDEX_PDNRT_FEED_IN_LOAD_LIMITATION,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            EDD_PRM_ERR_FAULT_DEFAULT);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
        }

        /* Validation of PDSCFDataCheck */
        if (EDD_STS_OK == Status)
        {
            if ( pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.Present )
            {
                if ( pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.SCF != pDDB->pSRT->Cfg.CycleBaseFactor )
                {
                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmValidateConsistency: Validation of PDSCFDataCheck. SCF: %d SCF != CycleBaseFactor: %d",
                            pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.SCF,
                            pDDB->pSRT->Cfg.CycleBaseFactor);

                    EDDS_PRM_SET_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr,
                            EDD_PRM_ERR_PORT_ID_DEFAULT,
                            EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK,
                            EDD_PRM_ERR_OFFSET_DEFAULT,
                            EDD_PRM_ERR_FAULT_DEFAULT);

                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
            }
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmValidateConsistency(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDNCDataCheckWrite                  +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDNCDataCheck record                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid record data and copy to record set B.    +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked where called
//WARNING: be careful while using this function, make sure not to use pRecordData as null ptr
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDNCDataCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData)
{

    LSA_RESULT                              Status;
    EDDS_PRM_NC_DATA_PTR_TYPE               pFrameDrop;
    EDDS_PRM_BUDGET_TYPE                    Budget[EDDS_FRAME_DROP_BUDGET_CNT];
    LSA_UINT32                              Value;
    LSA_UINT32                              i;

    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Begin;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Tmp;

    EDDS_PRM_DETAIL_ERR_PTR_TYPE            pDetailErr;

    pRecord_Begin = pRecordData;
    pDetailErr = &pDDB->pGlob->Prm.PrmDetailErr;

    for(i=0; i<EDDS_FRAME_DROP_BUDGET_CNT;++i)
    {
        Budget[i].Enabled = LSA_FALSE;
        Budget[i].Value   = 0;
    }

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDNCDataCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    pFrameDrop = &pDDB->pGlob->Prm.FrameDrop;
    pFrameDrop->RecordSet_B.Present = LSA_FALSE; /* overwrite actual set if present */

    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /*   PDNCDataCheck (= Frame dropped - no resource)                           */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                            */
    /*    LSA_UINT8           Padding                                            */
    /*    LSA_UINT8           Padding                                            */
    /*    LSA_UINT32          MaintenanceRequiredPowerBudget;                    */
    /*    LSA_UINT32          MaintenanceDemandedPowerBudget;                    */
    /*    LSA_UINT32          ErrorPowerBudget;                                  */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE;

    pRecord_Tmp = pRecordData;
    /* check padding bytes */
    if ( *pRecordData++ != 0 )
    {
        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }
    pRecord_Tmp = pRecordData;
    if ( *pRecordData++ != 0 )
    {
        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    /* check for valid budgets                   */
    /* order: 0: required, 1: demanded, 2: error */

    for(i=0; (EDD_STS_OK==Status) && (EDDS_FRAME_DROP_BUDGET_CNT > i);++i)
    {

        EDDS_GET_U32_INCR(pRecordData,Value);  /* budget */

        /* ON ? */
        if ( (Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_MSK) == EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON )
        {
            Value &= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;  /* Get Value */

            /* check range */
            if (( Value >= EDDS_PRM_PDNC_DROP_BUDGET_MIN ) &&
                ( Value <= EDDS_PRM_PDNC_DROP_BUDGET_MAX ))
            {
                Budget[i].Enabled = LSA_TRUE;
                Budget[i].Value   = Value;

                /* check if value of budget is: error > demanded > required  */
                switch ( i )
                {
                    case EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX: /* required budget */
                        break;

                    case EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX: /* demanded budget */

                        if (( Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) &&
                            ( Value <= Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value )) /* required enabled ? */
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Demanded Budget invalid (%d)", Value);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordData - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        break;

                    case EDDS_ERROR_BUDGET_IDX: /* Error budget */

                        if (( Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) &&
                            ( Value <= Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value )) /* required enabled ? */
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Error Budget invalid (%d)", Value);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordData - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        if (( Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled ) &&
                            (  Value <= Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value )) /* demanded enabled ? */
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Error Budget invalid (%d)", Value);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordData - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        break;
                    default:
                        break;

                } /* switch */
            }
            else
            {
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_PrmPDNCDataCheckWrite: Budget Value invalid (%d)", Value);

                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordData - pRecord_Begin);

                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
    }

    /* if OK copy to set B */

    if ( Status == EDD_STS_OK )
    {
        for (i=0; i<EDDS_FRAME_DROP_BUDGET_CNT;++i)
        {
            pFrameDrop->RecordSet_B.Budget[i] = Budget[i];
        }

        /* record set is present.. */
        pFrameDrop->RecordSet_B.Present = LSA_TRUE;
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDNCDataCheckWrite(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDPortDataAdjustWrite               +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_BOOL                    Local           +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+                             LSA_UINT32                  BlockLength     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  Local       : LSA_TRUE : Local  parametration                          +*/
/*+                LSA_FALSE: Remote parametration                          +*/
/*+  PortID      : PortID. 1.. EDDS_MAX_PORT_CNT                            +*/
/*+  pRecordData : Pointer to PDPortData    record                          +*/
/*+  BlockLength : BlockLength  (total length of block from Header)         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid record data and copy to record set B.    +*/
/*+                                                                         +*/
/*+               If Local is set all Speed/Mode settings allowed           +*/
/*+               If not, only 100/1000/1000 FullDuplex allowed.            +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+               On entry all settings marked as invalid!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked where called
//WARNING: be careful while using this function, make sure not to use pRecordData
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDPortDataAdjustWrite(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
    LSA_UINT8                       Local,
    LSA_UINT16                      PortID,
    EDD_UPPER_MEM_U8_PTR_TYPE       pRecordData,
    LSA_UINT32                      BlockLength,
    const EDD_UPPER_MEM_U8_PTR_TYPE pRecord_BlockLength)
{

    LSA_RESULT                               Status;
    EDDS_PRM_PORT_DATA_PTR_TYPE              pPortData;
    LSA_UINT16                               SubBlockType;
    LSA_UINT16                               SubBlockLength;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecordDataHelp;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE pRecordSetB;
    //LSA_UINT32                               i;
    LSA_UINT8                                VersionHigh;
    LSA_UINT8                                VersionLow;
    LSA_UINT8                                Pad1;
    LSA_UINT8                                Pad2;
    LSA_UINT8                                Pad3;
    LSA_UINT8                                Pad4;
    LSA_UINT32                               Position;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_Tmp;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_VersionHigh;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_VersionLow;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_SubBlockLength;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_Begin;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_MauType;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_MauTypeAP;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_MulticastBoundaryAP;

    EDDS_PRM_DETAIL_ERR_PTR_TYPE             pDetailErr;

    LSA_UNUSED_ARG(Local);

    pDetailErr = &pDDB->pGlob->Prm.PrmDetailErr;
    EDDS_PRM_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDPortDataAdjustWrite(pDDB: 0x%X, PortID: %d, pRecordData: 0x%X, Length: 0x%X)",
                           pDDB,
                           PortID,
                           pRecordData,
                           BlockLength);

    Status        = EDD_STS_OK;
    //i             = 0;
    Position      = 1;
    pRecord_Begin = pRecordData;
    pPortData   = &pDDB->pGlob->Prm.PortData;
    pRecordSetB = &pPortData->RecordSet_B[PortID-1];

    /* overwrite actual set if present */
    pRecordSetB->Present = LSA_FALSE;
    pRecordSetB->MAUTypePresent           = LSA_FALSE;
    pRecordSetB->MulticastBoundaryPresent = LSA_FALSE;
    pRecordSetB->PortStatePresent         = LSA_FALSE;
    pRecordSetB->PreambleLengthPresent    = LSA_FALSE;

    /*---------------------------------------------------------------------------*/
    /*   PDPortDataAdjust                                                        */
    /*---------------------------------------------------------------------------*/
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          SlotNumber                                    2    */
    /*    LSA_UINT16          SubslotNumber                                 2    */
    /*    [AdjustDomainBoundary]                             skipped   by EDDS   */
    /*    [AdjustMulticastBoundary]                          evaluated by EDDS   */
    /*    [AdjustMauType ^ AdjustPortState]                  evaluated by EDDS   */
    /*                                                                           */
    /*    AdjustMulticastBoundary:                                               */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          MulticastBoundary                             4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*    AdjustMauType                                                          */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          MAUType                                       2    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*                                                                    -----  */
    /*                                                                     12    */
    /*                                                                           */
    /*    AdjustPortState                                                        */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          PortState                                     2    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*                                                                    -----  */
    /*                                                                     12    */
    /*                                                                           */
    /*    AdjustPeerToPeerBoundary                                               */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          PeerToPeerBoundary                            4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*                                                                           */
    /*    AdjustDCPBoundary                                                      */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          DCPBoundary                                   4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*---------------------------------------------------------------------------*/

    /* Pointer to first Record behind SubslotNuber */
    pRecordData  += EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE;
    BlockLength  -= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE - 4;

    /* Note: maybe we have no sublocks! this is valid ! */

    Position = 1;  //lint !e838 JB 11/11/2014 see while loop below:

    while (( Status == EDD_STS_OK ) && ( BlockLength ))
    {

        if ( BlockLength >= EDDS_PRM_RECORD_HEADER_SIZE )
        {
            pRecordDataHelp = pRecordData;

            EDDS_GET_U16_INCR(pRecordDataHelp,SubBlockType);

            pRecord_SubBlockLength = pRecordDataHelp;
            EDDS_GET_U16_INCR(pRecordDataHelp,SubBlockLength);

            pRecord_VersionHigh = pRecordDataHelp;
            VersionHigh = *pRecordDataHelp++;
            pRecord_VersionLow = pRecordDataHelp;
            VersionLow  = *pRecordDataHelp++;

            SubBlockLength +=4;  /* add length of BlockType and Length */

            if ( ( SubBlockLength <= BlockLength                 ) &&
                 ( SubBlockLength >= EDDS_PRM_RECORD_HEADER_SIZE ) &&
                 ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH    ) )
            {

                /* pRecordDataHelp points behing blockheader */

                switch (SubBlockType)
                {
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_LOW_MC_BOUNDARY )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust MulticastBoundary): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE)
                        {
                            if ( Position <= 2 )
                            {
                                Position = 3;
                                pRecordSetB->MulticastBoundaryPresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U32_INCR(pRecordDataHelp,pRecordSetB->MulticastBoundary);

                                if( (pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_SET_MC_BOUNDARIES) ||
                                        !pRecordSetB->MulticastBoundary)
                                {
                                    pRecord_MulticastBoundaryAP = pRecordDataHelp;
                                    EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MulticastBoundaryAP);
                                    Pad3 = *pRecordDataHelp++;
                                    Pad4 = *pRecordDataHelp++;

                                    if (( pRecordSetB->MulticastBoundaryAP != EDDS_PRM_PDPORT_DATA_MC_ADJUST_PROPERTIES ) ||
                                        ( Pad1                             != 0                                        ) ||
                                        ( Pad2                             != 0                                        ) ||
                                        ( Pad3                             != 0                                        ) ||
                                        ( Pad4                             != 0                                        ))
                                    {
                                        EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                            "EDDS_PrmPDPortDataAdjustWrite: MulticastBoundary invalid (%d,%d)", pRecordSetB->MulticastBoundary,pRecordSetB->MulticastBoundaryAP);

                                        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_MulticastBoundaryAP - pRecord_Begin);

                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: cannot set McBoundary");

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust MC Boundary block length (%d)",
                                                SubBlockLength-4);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;

                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_LOW_MAUTYPE )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust MAUType): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE)
                        {
                            if ( Position <= 3 )
                            {
                                Position = 4;
                                pRecordSetB->MAUTypePresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                pRecord_MauType = pRecordDataHelp;
                                pDDB->pGlob->Prm.PrmDetailErr.RecordOffset_MauType = (LSA_UINT32)(pRecord_MauType-pRecord_Begin);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MAUType);
                                pRecord_MauTypeAP = pRecordDataHelp;
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MAUTypeAP);

                                if (( pRecordSetB->MAUTypeAP == EDDS_PRM_PDPORT_DATA_MAUTYPE_ADJUST_PROPERTIES ) &&
                                    ( Pad1                   == 0                                             ) &&
                                    ( Pad2                   == 0                                             ))
                                {
                                    /* checking and decoding MAUType has been moved to PRM-End */
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: MAUTypeAP or padding invalid (%d)",pRecordSetB->MAUTypeAP);

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_MauTypeAP - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust MAUType block length (%d)",
                                                SubBlockLength-4);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;

                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_LOW_PORTSTATE )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust Portstate): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE)
                        {
                            if ( Position <= 3 )
                            {
                                Position = 4;
                                pRecordSetB->PortStatePresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                pRecord_Tmp = pRecordDataHelp;
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PortState);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PortStateAP);

                                if (( pRecordSetB->PortState   != EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN             ) ||
                                    ( pRecordSetB->PortStateAP != EDDS_PRM_PDPORT_DATA_PORTSTATE_ADJUST_PROPERTIES ) ||
                                    ( Pad1                     != 0                                               ) ||
                                    ( Pad2                     != 0                                               ))
                                {
                                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: PortState invalid (%d,%d)", pRecordSetB->PortState,pRecordSetB->PortStateAP);

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                                else
                                {

                                    /* check if DOWN supported by LLIF */

                                    if(!(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_PHY_POWER_OFF))
                                    {
                                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDPortDataAdjustWrite: POWER OFF not supported!");

                                        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust Portstate block length (%d)",
                                                SubBlockLength-4);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        /* Must be first block if present */
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_D_BOUNDARY )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust DomainBoundary): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( Position == 1 )
                        {
                            Position = 2;
                        }
                        else
                        {
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_P2P_BOUNDARY )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust PeerToPeerBoundary): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( Position <= 4 )
                        {
                            Position = 5;
                            /* tbd */
                        }
                        else
                        {
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_DCP_BOUNDARY )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust DCPBoundary): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE)
                        {
                            if ( Position <= 5 )
                            {
                                Position = 6;
                                pRecordSetB->DCPBoundaryPresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U32_INCR(pRecordDataHelp,pRecordSetB->DCPBoundary);
                                pRecord_Tmp = pRecordDataHelp;
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->DCPBoundaryAP);
                                Pad3 = *pRecordDataHelp++;
                                Pad4 = *pRecordDataHelp++;

                                if (( pRecordSetB->DCPBoundaryAP       != EDDS_PRM_PDPORT_DATA_DCP_ADJUST_PROPERTIES ) ||
                                    ( Pad1                             != 0                                         ) ||
                                    ( Pad2                             != 0                                         ) ||
                                    ( Pad3                             != 0                                         ) ||
                                    ( Pad4                             != 0                                         ))
                                {
                                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: DCPBounary invalid (%d,%d)", pRecordSetB->DCPBoundary,pRecordSetB->DCPBoundaryAP);

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust DCP Boundary block length (%d)",
                                                SubBlockLength-4);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( VersionLow  != EDDS_PRM_BLOCK_VERSION_PREAMBLE_LENGTH )
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Wrong Version Low (Adjust PreambleLength): 0x%04x",
                                                VersionLow);

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else if ( Position <= 6 )
                        {
                            Position = 7;
                            pRecordSetB->PreambleLengthPresent = LSA_TRUE;

                            Pad1 = *pRecordDataHelp++;
                            Pad2 = *pRecordDataHelp++;
                            pRecord_Tmp = pRecordDataHelp;
                            EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PreambleLength);
                            EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PreambleLengthAP);

                            if (( pRecordSetB->PreambleLength   != EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_LONG )       ||
                                ( pRecordSetB->PreambleLengthAP != EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_PROPERTIES ) ||
                                ( Pad1                          != 0 )   ||
                                ( Pad2                          != 0 ))
                            {
                                EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: PreambleLength invalid (%d,%d)", pRecordSetB->PreambleLength,pRecordSetB->PreambleLengthAP);

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    default:
                        /*-----------------------------------------------------------*/
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                            "EDDS_PrmPDPortDataAdjustWrite: Unknown Subblock (0x%X)",SubBlockType);

                        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordDataHelp - pRecord_Begin);

                        Status = EDD_STS_ERR_PRM_BLOCK;
                        break;
                }

                pRecordData += SubBlockLength;
                BlockLength -= SubBlockLength;
            }
            else
            {
                if ( ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH    ) &&
                     ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW     ))
                {
                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmPDPortDataAdjustWrite: Invalid Subblock length (%d,%d)",SubBlockType,SubBlockLength);

                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_SubBlockLength - pRecord_Begin);

                    Status = EDD_STS_ERR_PRM_DATA;
                }
                else
                {
                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmPDPortDataAdjustWrite: Invalid Subblock version (%d,%d)",VersionHigh,VersionLow);

                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionHigh - pRecord_Begin);

                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
        }
        else
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDPortDataAdjustWrite: Invalid record length");

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_BlockLength - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_DATA;
        }
    }

    /* if OK set present else set all to invalid */
    /* tbd: reaction if entrys missing? */

    if ( Status == EDD_STS_OK )
    {
        pRecordSetB->Present = LSA_TRUE;  /* maybe no subblock present ! */
    }
    else
    {
        pRecordSetB->MAUTypePresent           = LSA_FALSE;
        pRecordSetB->MulticastBoundaryPresent = LSA_FALSE;
        pRecordSetB->PortStatePresent         = LSA_FALSE;
        pRecordSetB->PreambleLengthPresent    = LSA_FALSE;
        pRecordSetB->DCPBoundaryPresent       = LSA_FALSE;
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDPortDataAdjustWrite(), Status: 0x%X",
                           Status);

    return(Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDSCFDataCheckWrite                 +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDNCDataCheck record                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid Sendclockfactor.                         +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 pRecordData can not be a null pointer - checked where called; pDDB checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDSCFDataCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData)
{

    LSA_RESULT                               Status;
    LSA_UINT16                               SendClockFactor;

    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_Begin;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecord_Tmp = LSA_NULL;

    EDDS_PRM_DETAIL_ERR_PTR_TYPE             pDetailErr;

    pRecord_Begin = pRecordData;
    pDetailErr = &pDDB->pGlob->Prm.PrmDetailErr;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDSCFDataCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /*   PDSCFDataCheck (= Sendclock factor check)                               */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                            */
    /*    LSA_UINT16          SendClockFactor                                    */
    /*    LSA_UINT8           Data[x]                                            */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE;

    if( pDDB->pGlob->IsSrtUsed )
    {
        /* check for valid SendclockFactor           */
        pRecord_Tmp = pRecordData;
        EDDS_GET_U16_INCR(pRecordData,SendClockFactor);

        #ifndef EDDS_CFG_PRM_DONT_CHECK_SENDCLOCK

        switch (SendClockFactor)
        {
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:
        case 1024:
            break;
        default:
			EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDSCFDataCheckWrite: Invalid SendClockFactor(%d,%d)!",
                                    pDDB->pSRT->Cfg.CycleBaseFactor,
                                    SendClockFactor);

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
            break;
        }

        if(pDDB->pGlob->schedulerCycleIO*32 <= SendClockFactor)
        {
            pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.SCF = SendClockFactor;
        }
        else
        {
			    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDSCFDataCheckWrite: Invalid SendClockFactor: %d, schedulerCycleIO: %d!",
                                    SendClockFactor,
                                    pDDB->pGlob->schedulerCycleIO);

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }

        #endif
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDSCFDataCheckWrite(), Status: 0x%X",
                           Status);

    // is needed for compiling pciox without warnings
    LSA_UNUSED_ARG(pRecord_Begin);
    LSA_UNUSED_ARG(pRecord_Tmp);
    LSA_UNUSED_ARG(pDetailErr);

    return(Status); //lint !e438 JB 11/11/2014 last value of pRecordDate actually used as SendClockFactor after EDDS_GET_U16_INCR
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDNRTFeedInLoadLimitationWrite      +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDNRTFeedInLoadLimitation record              +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid record data and copies it to record set  +*/
/*+               B.                                                        +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has to be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked where called
//WARNING: be careful while using this function, make sure not to use pRecordData as null ptr
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDNRTFeedInLoadLimitationWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData)
{

    LSA_RESULT                              Status = EDD_STS_OK;
    EDDS_PRM_PDNRT_FILL_DATA_PTR_TYPE       pFill;

    LSA_UINT8 LoadLimitationActive = EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_ACTIVE; /* default */
    LSA_UINT8 IO_Configured = EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED; /* default */

    EDD_UPPER_MEM_U8_PTR_TYPE pRecord_Begin;
    EDD_UPPER_MEM_U8_PTR_TYPE pRecord_Tmp;

    EDDS_PRM_DETAIL_ERR_PTR_TYPE pDetailErr;

    pRecord_Begin = pRecordData;
    pDetailErr = &pDDB->pGlob->Prm.PrmDetailErr;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                      "IN :EDDS_PrmPDNRTFeedInLoadLimitationWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                      pDDB, pRecordData);

    pFill = &pDDB->pGlob->Prm.PDNRTFill;
    pFill->RecordSet_B.Present = LSA_FALSE; /* overwrite actual set if present */

    /*---------------------------------------------------------------------------*/
    /*   PDNRTFeedInLoadLimitation (= Feed-In Load Limitation)                   */
    /*---------------------------------------------------------------------------*/
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT16          Reserved                                      2    */
    /*    LSA_UINT8           LoadLimitationActive                          1    */
    /*    LSA_UINT8           IO_Configured                                 1    */
    /*    LSA_UINT16          Reserved                                      2    */
    /*                                                                    -----  */
    /*                                                                     12    */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE;

    pRecord_Tmp = pRecordData;

    /* check padding bytes */
    if ( *pRecordData++ != 0 )
    {
        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }
    pRecord_Tmp = pRecordData;
    if ( *pRecordData++ != 0 )
    {
        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    if( EDD_STS_OK == Status )
    {
        /* check for valid values for LoadLimitationActive */

        pRecord_Tmp = pRecordData;
        LoadLimitationActive = *pRecordData++;

        if(EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_INACTIVE != LoadLimitationActive &&
           EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_ACTIVE   != LoadLimitationActive)
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDNRTFeedInLoadLimitationWrite: LoadLimitationActive invalid!");

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }
        
        /* reject PRM record with LoadLimitationActive=ACTIVE and enabled HSync application support */
        if( (EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_ACTIVE == LoadLimitationActive) &&
            (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist) )
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDNRTFeedInLoadLimitationWrite(): Trying to set FeedInLoadLimitation on HSYNC is not allowed");
            
            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);
            
            Status = EDD_STS_ERR_PRM_BLOCK;
        }

        /* check for valid values for IO_Configured */

        pRecord_Tmp = pRecordData;
        IO_Configured = *pRecordData++;

        if(EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_NOT_CONFIGURED != IO_Configured &&
           EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED     != IO_Configured)
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDNRTFeedInLoadLimitationWrite: IO_Configured value invalid!");

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }

        /* reject PRM record with IO_Configured=YES without SRT support enabled */
        if((EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED == IO_Configured) &&
           (!pDDB->pGlob->IsSrtUsed))
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDNRTFeedInLoadLimitationWrite: IO_Configured=YES invalid without SRT support, see EDDS configuration!");

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }
        
        /* reject PRM record with IO_Configured=YES and LoadLimitationActive=INACTIVE */
        if((EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED == IO_Configured) &&
           (EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_INACTIVE      == LoadLimitationActive))
        {
            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDNRTFeedInLoadLimitationWrite: IO_Configured=YES invalid without send load limitation!");

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if ( EDD_STS_OK == Status )
    {
        /* if OK copy to set B */
        pFill->RecordSet_B.LoadLimitationActive = LoadLimitationActive;
        pFill->RecordSet_B.IO_Configured = IO_Configured;

        /* record set is present.. */
        pFill->RecordSet_B.Present = LSA_TRUE;

        EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDS_PrmPDNRTFeedInLoadLimitationWrite(): LoadLimitationActive=0x%X, IO_Configured=0x%X",
                          LoadLimitationActive, IO_Configured);
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                      "OUT:EDDS_PrmPDNRTFeedInLoadLimitationWrite(), Status: 0x%X",
                      Status);

    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDPortStatisticRecordRead           +*/
/*+                                                                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_IDB_PTR_TYPE     pIDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pIDB       : Pointer to InterfaceDescriptionBlock                      +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function reads the Port/Inteface-Statistic           +*/
/*+               and stores it to PrmRead RQB.                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
static EDD_RSP EDDS_LOCAL_FCT_ATTR EDDS_PrmPDPortStatisticRecordRead(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    EDD_RSP                         Response;
    EDD_UPPER_PRM_READ_PTR_TYPE     pPrmReadParam;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    LSA_UINT32						TraceIdx ;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    Response                            = EDD_STS_OK;
    pPrmReadParam                       = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam;  //JB 11/11/2014 should not be necessary (cast as reason?)
    pPrmReadParam->record_data_length   = 0;

    EDDS_PRM_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN: EDDS_PrmPDPortStatisticRecordRead(pRQB=0x%X), pHDB=0x%X, PortID=%d",
        pRQB, pHDB, pPrmReadParam->edd_port_id);

    /* ------------------------------------ */
    /* check PortID                         */
    /* ------------------------------------ */
    // PortID=0     : interface
    // PortID=1..n  : Port
    if (pPrmReadParam->edd_port_id > pHDB->pDDB->pGlob->HWParams.Caps.PortCnt)
    {
        Response = EDD_STS_ERR_PRM_PORTID;
        EDDS_PRM_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmPDPortStatisticRecordRead(pHDB=0x%X): ERROR -> PortID(%d) is not between 0..%d!",
            pHDB, pPrmReadParam->edd_port_id, pDDB->pGlob->HWParams.Caps.PortCnt);
    }
    else if (EDD_PORT_NOT_PRESENT == pHDB->pDDB->pGlob->LinkStatus[pPrmReadParam->edd_port_id].PortStatus)
    {
        EDDS_PRM_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_WARN,
            "EDDS_PrmPDPortStatisticRecordRead(pHDB=0x%X): PortStatus is EDD_PORT_NOT_PRESENT for PortID: %d!",
            pHDB,
            pPrmReadParam->edd_port_id);
        Response = EDD_STS_ERR_PRM_PORTID;
    }
    else
    {
        /* ------------------------------------ */
        /* get current StatisticData            */
        /* ------------------------------------ */
        EDDS_LOWER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_GET_STATS(pDDB: 0x%X, PortID: 0x%X)",pDDB,pPrmReadParam->edd_port_id);

        Response = EDDS_HandleRQB_byScheduler (pDDB, pRQB);
    }

    EDDS_PRM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:EDDS_PrmPDPortStatisticRecordRead(): Response=0x%X", Response);

    LSA_UNUSED_ARG(TraceIdx);

    return (Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDSetDefPortStatesCheckWrite        +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+                             LSA_UINT32                  BlockLength     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDSetDefaultPortStates record                 +*/
/*+  BlockLength : BlockLength from header                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid SetDefaultPortStates                     +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 pRecordData can not be a null pointer - checked where called; pDDB checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDSetDefPortStatesCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  BlockLength)
{

    LSA_RESULT                              Status;
    EDDS_PRM_SET_DEF_PORTSTATES_PTR_TYPE    pDefPortStates;
    LSA_UINT8                               PortId;    /* User PortId 1..x */
    LSA_UINT8                               PortState;
    //LSA_UINT8                               Pad1,Pad2;
    LSA_UINT32                              i;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Begin;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Tmp    = LSA_NULL;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_PortId = LSA_NULL;
    EDDS_PRM_DETAIL_ERR_PTR_TYPE            pDetailErr;

    Status         = EDD_STS_OK;
    pDefPortStates = &pDDB->pGlob->Prm.DefPortStates;
    pDefPortStates->Present_B = LSA_FALSE; /* not present */
    pRecord_Begin  = pRecordData;
    pDetailErr     = &pDDB->pGlob->Prm.PrmDetailErr;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDSetDefPortStatesCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    /* Set all entrys to not present*/
    for (i=0; i<EDDS_MAX_PORT_CNT; i++)
    {
        pDefPortStates->RecordSet_B[i].PortStatePresent = LSA_FALSE;
    }

    /*---------------------------------------------------------------------------*/
    /*   PDSetDefaultPortStates                                                  */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    (                                                                      */
    /*      LSA_UINT8           PortId                                      1    */
    /*      LSA_UINT8           PortState                                   1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*    )*                                                                     */
    /*                                                                           */
    /* PortID: 1..255 (a valid PortId)                                           */
    /* each PortId must be present Port and only appears once within record!     */
    /* Multiple PortId settings may be present within record.                    */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader and padding */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE+2;

    BlockLength -= (EDDS_PRM_RECORD_HEADER_SIZE - 4) +2 ; /* remaining length behind paddings */

    while (BlockLength >= 4 ) /* PortId, PortState, Pad1, Pad2 = 4 Bytes */
    {
        pRecord_PortId = pRecordData;
        PortId    = *pRecordData++;
        pRecord_Tmp = pRecordData;
        PortState = *pRecordData++;
        //Pad1 = *pRecordData++;
        //Pad2 = *pRecordData++;
        pRecordData +=2;
        BlockLength -= 4;

        /* check for valid PortId */
        if (( PortId > 0 ) && ( PortId <= pDDB->pGlob->HWParams.Caps.PortCnt))
        {
            /* check for duplicate entry */
            if ( ! pDefPortStates->RecordSet_B[PortId-1].PortStatePresent )
            {
                /* check for valid PortState */
                if (( PortState == EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE ) ||
                    ( PortState == EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING))
                {
                    /* all ok. store values.. */
                    pDefPortStates->RecordSet_B[PortId-1].PortStatePresent = LSA_TRUE;
                    pDefPortStates->RecordSet_B[PortId-1].PortState        = PortState;

                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_PrmPDSetDefPortStatesCheckWrite: PortID: %d, PortState: %d",PortId,PortState);
                }
                else
                {
                    /* Invalid PortState! */
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmPDSetDefPortStatesCheckWrite, Invalid PortState: 0x%X",PortState);

                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Tmp - pRecord_Begin);

                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else
            {
                /* PortState for PortID already set within record! */
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Multiple PortId: 0x%X",PortId);

                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecordData - pRecord_Begin);

                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
        else
        {
            /* invalid PortId */
            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Invalid PortId: 0x%X",PortId);

            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_PortId - pRecord_Begin);

            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    } /* while */

    /* if ok check if Blocklength matches (no bytes left) */
    if ((Status == EDD_STS_OK ) && ( BlockLength != 0 ))
    {
        /* invalid BlockLength (dont matches content) */
        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Invalid remaining BlockLength (not 0): 0x%X",BlockLength);

        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    /*---------------------------------------------------------------------------*/
    /* on error clear content. if ok set WRITE_DONE.                             */
    /*---------------------------------------------------------------------------*/
    if ( Status != EDD_STS_OK )
    {
        for (i=0; i<EDDS_MAX_PORT_CNT; i++)
        {
            pDefPortStates->RecordSet_B[i].PortStatePresent = LSA_FALSE;
        }
    }
    else
    {
        pDefPortStates->Present_B = LSA_TRUE; /* present */
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDSetDefPortStatesCheckWrite(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmRead                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_READ                +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_READ_PTR_TYPE                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_PORTID                                    +*/
/*+               EDD_STS_ERR_PRM_INDEX                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_READ request        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmRead(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                               	Status;
    EDD_UPPER_PRM_READ_PTR_TYPE              	pPrmRead;
    EDDS_PRM_PTR_TYPE                        	pPrm;
    EDD_UPPER_MEM_U8_PTR_TYPE                	pRecordData;
    EDDS_NC_DATA_CHECK_RECORD_SET_PTR_TYPE   	pNCDataRecordSet;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE 	pPORTDataRecordSet;
    EDDS_PDNRT_FILL_RECORD_SET_PTR_TYPE         pPDNRTDataRecordSet;
    LSA_UINT32                               	Budget;

    EDDS_LOCAL_DDB_PTR_TYPE						pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmRead(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK;
    pPrmRead = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam; //JB 11/11/2014 should not be necessary (cast as reason?)

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPrmRead, LSA_NULL) ) ||
         ((pPrmRead->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt)) ||
         ( LSA_HOST_PTR_ARE_EQUAL( pPrmRead->record_data, LSA_NULL) )
       )
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pPrm = &pDDB->pGlob->Prm;

        switch (pPrmRead->record_index)
        {

            /* ---------------------------------------- */
            /* PDPortStatistic                          */
            /* ---------------------------------------- */
            case EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX:
            {
                if ( pPrmRead->record_data_length >= EDDS_PRM_PDPORT_STATISTIC_SIZE )
                {
                    Status = EDDS_PrmPDPortStatisticRecordRead (pRQB, pHDB);
                }
                else
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_RequestPrmRead: Invalid record_data_length (%d)", pPrmRead->record_data_length);

                    Status = EDD_STS_ERR_PRM_DATA;
                }
            }
            break;

            /* --------------------------------------------------------------*/
            /* PDNCDataCheck                                                 */
            /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK:
                if (pPrmRead->edd_port_id == 0)
                {
                    pNCDataRecordSet = &pPrm->FrameDrop.RecordSet_A;

                    if (pNCDataRecordSet->Present )
                    {
                        if ( pPrmRead->record_data_length >= EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE )
                        {
                            pPrmRead->record_data_length = EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE;

                            pRecordData = pPrmRead->record_data;

                            /*--------------------------------------------------------*/
                            /*   BlockHeader                                          */
                            /*--------------------------------------------------------*/
                            /*    LSA_UINT16          BlockType;                      */
                            /*    LSA_UINT16          BlockLength;                    */
                            /*    LSA_UINT8           BlockVersionHigh;               */
                            /*    LSA_UINT8           BlockVersionLow;                */
                            /*--------------------------------------------------------*/

                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE);
                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE - 4); //lint !e572 !e778 correct shift operation
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                            /*--------------------------------------------------------*/
                            /*   PDNCDataCheck (= Frame dropped - no resource)        */
                            /*--------------------------------------------------------*/
                            /*    LSA_UINT8           Padding                         */
                            /*    LSA_UINT8           Padding                         */
                            /*    LSA_UINT32          MaintenanceRequiredPowerBudget; */
                            /*    LSA_UINT32          MaintenanceDemandedPowerBudget; */
                            /*    LSA_UINT32          ErrorPowerBudget;               */
                            /* -------------------------------------------------------*/
                            *pRecordData++ = 0; /* padding */
                            *pRecordData++ = 0; /* padding */

                            Budget = pNCDataRecordSet->Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            Budget = pNCDataRecordSet->Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            Budget = pNCDataRecordSet->Budget[EDDS_ERROR_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_ERROR_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            /* -------------------------------------------------------*/
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmRead: Invalid record_data_length (%d)", pPrmRead->record_data_length);

                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDNC: Invalid port_id (<>0)!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
            break;
            /* --------------------------------------------------------------*/
            /* PDPortDataAdjust                                              */
            /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST:
                if ( pPrmRead->edd_port_id != 0 ) /* 0 not allowed ! */
                {
                    if(EDD_PORT_NOT_PRESENT == pHDB->pDDB->pGlob->LinkStatus[pPrmRead->edd_port_id].PortStatus)
                    {
                        EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_WARN,
                            "EDDS_RequestPrmRead(pHDB=0x%X): PortStatus is EDD_PORT_NOT_PRESENT for PortID: %d!",
                            pHDB,
                            pPrmRead->edd_port_id);
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    else
                    {
                        pPORTDataRecordSet = &pPrm->PortData.RecordSet_A[pPrmRead->edd_port_id-1];

                        /* record present? */
                        if (pPORTDataRecordSet->Present )
                        {
                            #if 1
                            /* copy record */
                            if ( pPrmRead->record_data_length >= pPORTDataRecordSet->RecordLength )
                            {
                              EDDS_CopyLocal2Upper(pPORTDataRecordSet->Record,
                                         pPrmRead->record_data,
                                         pPORTDataRecordSet->RecordLength);

                              pPrmRead->record_data_length = pPORTDataRecordSet->RecordLength;

                            }
                            else
                            {
                              EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d", pPrmRead->record_data_length,pPORTDataRecordSet->RecordLength);

                              Status = EDD_STS_ERR_PRM_DATA;
                            }
                            #else
                            /* calculate total length needed */

                            Len = EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE;
                            /* Note: MAUType and PortState not both present ! */
                            if  ( pPORTDataRecordSet->MulticastBoundaryPresent)
                              Len += EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE;
                            if  ( pPORTDataRecordSet->MAUTypePresent)
                              Len += EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE;
                            if ( pPORTDataRecordSet->PortStatePresent)
                              Len += EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE;

                            if ( pPrmRead->record_data_length >= Len )
                            {

                              pPrmRead->record_data_length = Len;

                              pRecordData = pPrmRead->record_data;

                              /*--------------------------------------------------------*/
                              /*   BlockHeader                                          */
                              /*--------------------------------------------------------*/
                              /*    LSA_UINT16          BlockType;                      */
                              /*    LSA_UINT16          BlockLength;                    */
                              /*    LSA_UINT8           BlockVersionHigh;               */
                              /*    LSA_UINT8           BlockVersionLow;                */
                              /*--------------------------------------------------------*/

                              EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE);
                              EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(Len - 4)));
                              *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                              *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                              /*--------------------------------------------------------*/
                              /*    LSA_UINT8           Padding                         */
                              /*    LSA_UINT8           Padding                         */
                              /*    LSA_UINT16          SlotNumber                      */
                              /*    LSA_UINT16          SubslotNumber                   */
                              /*--------------------------------------------------------*/

                              *pRecordData++ = 0; /* padding */
                              *pRecordData++ = 0; /* padding */
                              EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->SlotNumber);
                              EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->SubSlotNumber);

                              /*----------------------------------------------------*/
                              /*    AdjustMulticastBoundary:                        */
                              /*----------------------------------------------------*/
                              /*    BlockHeader                                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT32          MulticastBoundary           */
                              /*    LSA_UINT16          AdjustProperties            */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT8           Padding                     */
                              /*----------------------------------------------------*/

                              if (pPORTDataRecordSet->MulticastBoundaryPresent)
                              {
                                EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE);
                                EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE - 4)));
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                                *pRecordData++ = 0; /* padding */
                                *pRecordData++ = 0; /* padding */
                                EDDS_SET_U32_INCR(pRecordData,pPORTDataRecordSet->MulticastBoundary);
                                EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MulticastBoundaryAP);
                                *pRecordData++ = 0; /* padding */
                                *pRecordData++ = 0; /* padding */
                              }

                              /*----------------------------------------------------*/
                              /*    AdjustMauType                                   */
                              /*----------------------------------------------------*/
                              /*    BlockHeader                                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT16          MAUType                     */
                              /*    LSA_UINT16          AdjustProperties            */
                              /*----------------------------------------------------*/

                              if (pPORTDataRecordSet->MAUTypePresent)
                              {
                                EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE);
                                EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE - 4)));
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                                *pRecordData++ = 0; /* padding */
                                *pRecordData++ = 0; /* padding */
                                EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MAUType);
                                EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MAUTypeAP);
                              }

                              /*----------------------------------------------------*/
                              /*    AdjustPortState                                 */
                              /*----------------------------------------------------*/
                              /*    BlockHeader                                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT8           Padding                     */
                              /*    LSA_UINT16          PortState                   */
                              /*    LSA_UINT16          AdjustProperties            */
                              /*----------------------------------------------------*/

                              if (pPORTDataRecordSet->PortStatePresent)
                              {
                                EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE);
                                EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE - 4)));
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                                *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                                *pRecordData++ = 0; /* padding */
                                *pRecordData++ = 0; /* padding */
                                EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->PortState);
                                EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->PortStateAP);
                              }
                            }
                            else
                            {
                              EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d", pPrmRead->record_data_length,Len);

                              Status = EDD_STS_ERR_PRM_DATA;
                            }
                            #endif

                        }
                        else
                        {
                            pPrmRead->record_data_length = 0; /* no record present */
                        }
                    }
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDPORT: Invalid port_id (=0)!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
            break;

            /* --------------------------------------------------------------*/
            /* PDSCFDataCheck                                                */
            /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK:
                if ( 0 == pPrmRead->edd_port_id) /* only 0 allowed ! */
                {
                    /* record present? */
                    if (pPrm->PDSCFDataCheck.RecordSet_A.Present )
                    {
                        /* copy record */
                        if ( pPrmRead->record_data_length >= pPrm->PDSCFDataCheck.RecordSet_A.RecordLength )
                        {
                          EDDS_CopyLocal2Upper(pPrm->PDSCFDataCheck.RecordSet_A.Record,
                                     pPrmRead->record_data,
                                     pPrm->PDSCFDataCheck.RecordSet_A.RecordLength);

                          pPrmRead->record_data_length = pPrm->PDSCFDataCheck.RecordSet_A.RecordLength;

                        }
                        else
                        {
                          EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d",
                                    pPrmRead->record_data_length,
                                    pPrm->PDSCFDataCheck.RecordSet_A.RecordLength);

                          Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDSCFDataCheck: only port 0 allowed!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
            break;
            /* ---------------------------------------- */
            /* User specific records                    */
            /* These records are not used by EDDS;      */
            /* return OK!                               */
            /* ---------------------------------------- */
            case EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX:
            case EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX:
            case EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX:
            {
            pPrmRead->record_data_length = 0;
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_RequestPrmRead(): PrmRecord(0x%X) is not used by EDDS",
                    pPrmRead->record_index);
            }
            break;

            /* --------------------------------------------------------------*/
            /* PDNRTFeedInLoadLimitation                                     */
            /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDNRT_FEED_IN_LOAD_LIMITATION:
                if ( 0 == pPrmRead->edd_port_id) /* only 0 (interface) allowed ! */
                {
                    pPDNRTDataRecordSet = &pPrm->PDNRTFill.RecordSet_A;

                    if ( pPDNRTDataRecordSet->Present )
                    {
                        if ( pPrmRead->record_data_length >= EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE )
                        {
                            pPrmRead->record_data_length = EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE;

                            pRecordData = pPrmRead->record_data;

                            /*--------------------------------------------------------*/
                            /*   BlockHeader                                          */
                            /*--------------------------------------------------------*/
                            /*    LSA_UINT16          BlockType;                      */
                            /*    LSA_UINT16          BlockLength;                    */
                            /*    LSA_UINT8           BlockVersionHigh;               */
                            /*    LSA_UINT8           BlockVersionLow;                */
                            /*--------------------------------------------------------*/

                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_TYPE);
                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE - 4); //lint !e572 !e778 correct shift operation
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                            /*---------------------------------------------------------------------------*/
                            /*   PDNRTFeedInLoadLimitation (= Feed-In Load Limitation)                   */
                            /*---------------------------------------------------------------------------*/
                            /*                                                                           */
                            /*    BlockHeader                                                       6    */
                            /*    LSA_UINT16          Reserved                                      2    */
                            /*    LSA_UINT8           LoadLimitationActive                          1    */
                            /*    LSA_UINT8           IO_Configured                                 1    */
                            /*    LSA_UINT16          Reserved                                      2    */
                            /*                                                                    -----  */
                            /*                                                                     12    */
                            /*---------------------------------------------------------------------------*/
                            *pRecordData++ = 0; /* padding */
                            *pRecordData++ = 0; /* padding */

                            *pRecordData++ = pPDNRTDataRecordSet->LoadLimitationActive;
                            *pRecordData++ = pPDNRTDataRecordSet->IO_Configured;

                            *pRecordData++ = 0; /* padding */
                            *pRecordData   = 0; /* padding */

                            /* -------------------------------------------------------*/
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmRead: PDNRTFeedInLoadLimitation: Invalid record_data_length (%d)", pPrmRead->record_data_length);

                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDNRTFeedInLoadLimitation: only port 0 allowed!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
            break;

            /* --------------------------------------------------------------*/
            /* unknown/unsupported                                           */
            /* --------------------------------------------------------------*/
            default:
                #ifndef EDDS_CFG_PRM_READ_UNKNOWN_INDEX_ALLOWED
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RequestPrmRead: record_index (%d)", pPrmRead->record_index);

                Status = EDD_STS_ERR_PRM_INDEX;
                #else
                /* no error. we return an empty record instead */
                pPrmRead->record_data_length = 0; /* no record present */
                #endif
                break;
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmRead(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmPrepare                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_PREPARE             +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam not used                                                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_PREPARE request     +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmPrepare(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT              Status;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;

    pDDB = pHDB->pDDB;

    Status = EDDS_Prm_Statemachine(pDDB,EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE);
    if(EDD_STS_OK == Status)
    {
        EDDS_PrmSetBToNotPresent(pDDB);
        EDDS_PRM_INIT_DETAIL_ERR(&pDDB->pGlob->Prm.PrmDetailErr);
    }

    LSA_UNUSED_ARG(pRQB);

    return Status;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmEnd                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_END                 +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_PRM_END_TYPE                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_CONSISTENCY                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_END request         +*/
/*+                                                                         +*/
/*+               END is only allowed after PREPARE or WRITE                +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmEnd(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_PRM_END_PTR_TYPE              pPrmEnd;

    EDDS_LOCAL_DDB_PTR_TYPE                 pDDB;
    EDDS_PRM_DETAIL_ERR_PTR_TYPE            pDetailErr;

    pDDB    = pHDB->pDDB;
    pDetailErr  = &pDDB->pGlob->Prm.PrmDetailErr;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmEnd(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;
    pPrmEnd = (EDD_UPPER_PRM_END_PTR_TYPE) pRQB->pParam; //JB 11/11/2014 should not be necessary (cast as reason?)

    EDDS_PRM_INIT_DETAIL_ERR(pDetailErr);

    if ( LSA_HOST_PTR_ARE_EQUAL( pPrmEnd, LSA_NULL))
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmEnd: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else if (   (EDD_PRM_PARAMS_ASSIGNED_REMOTE != pPrmEnd->Local)
             && (EDD_PRM_PARAMS_ASSIGNED_LOCALLY != pPrmEnd->Local))
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "EDDS_RequestPrmEnd: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_SRV_PRM_END);
        if (EDD_STS_OK == Status)
        {
            Status = EDDS_PrmValidateConsistency(pDDB,pPrmEnd);

            if (pDetailErr->ErrValid)
            {
                EDDS_RQB_PRM_END_SET(pPrmEnd, pDetailErr->ErrPortID, pDetailErr->ErrIndex, pDetailErr->ErrOffset, pDetailErr->ErrFault);
                EDDS_PRM_TRACE_04(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR, "[H:--] EDDS_RequestPrmEnd: ERROR -> err_port_id=%d err_index=%d err_offset=%d err_fault=%d",
                    pPrmEnd->err_port_id, pPrmEnd->err_index, pPrmEnd->err_offset, pPrmEnd->err_fault);
            }
            else
            {
                if (Status != EDD_STS_OK)
                {
                    EDDS_RQB_PRM_END_SET(pPrmEnd, EDD_PRM_ERR_PORT_ID_DEFAULT, EDD_PRM_ERR_INDEX_DEFAULT, EDD_PRM_ERR_OFFSET_DEFAULT, EDD_PRM_ERR_FAULT_DEFAULT);
                }
            }
            if(EDD_STS_OK != Status)
            {
                LSA_RESULT Unused;
                Unused = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE);
                LSA_UNUSED_ARG(Unused);
            }
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmEnd(), Status: 0x%X",
                           Status);

    return Status;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmCommit                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_COMMIT              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam not used                                                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_COMMIT request      +*/
/*+               We copy record set B to record set A                      +*/
/*+               For all present records we have to indicate the current   +*/
/*+               ErrorStatus. If no error present we have to indicate an   +*/
/*+               empty message for ports some record was written. So all   +*/
/*+               written ports get a flag (doIndicate) to force a indicat. +*/
/*+                                                                         +*/
/*+               COMMIT is only allowed after an END. COMMIT shall only    +*/
/*+               be called if all WRITE and END were successful! CM does.  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmCommit(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                    Status;
    EDD_UPPER_PRM_COMMIT_PTR_TYPE pPrmCommit;
    LSA_UINT32					  i;
    EDDS_LOCAL_DDB_PTR_TYPE		  pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmCommit(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);



    Status = EDD_STS_OK;
    pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE) pRQB->pParam;

    if ( LSA_HOST_PTR_ARE_EQUAL( pPrmCommit, LSA_NULL) )
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
            "EDDS_RequestPrmCommit: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        for (i = 0; i < pDDB->pGlob->HWParams.Caps.PortCnt; i++)
        {
            if (   (EDD_PRM_APPLY_RECORD_PORT_PARAMS != pPrmCommit->ApplyDefaultPortparams[i])
                && (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != pPrmCommit->ApplyDefaultPortparams[i]))
            {
                EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RequestPrmCommit: RQB parameter error!");
                Status = EDD_STS_ERR_PARAM;
            }
        }
        if (EDD_STS_OK == Status)
        {
            Status = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_SRV_PRM_COMMIT);
            if (EDD_STS_OK == Status)
            {
                for (i = 0; i < pDDB->pGlob->HWParams.hardwareParams.PortCnt; i++)
                {
                    pDDB->pGlob->PrmChangePort[i].ApplyDefaultPortparams = (i >= pDDB->pGlob->HWParams.Caps.PortCnt)?EDD_PRM_APPLY_RECORD_PORT_PARAMS:pPrmCommit->ApplyDefaultPortparams[i];
                }

                pDDB->pGlob->Prm.currentState = EDDS_PRM_COMMIT_STATE_SYNCHRONOUS;
                pDDB->pGlob->Prm.currentStatePrmCommitPDPort = EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY;
                pDDB->pGlob->Prm.currentStatePrmCommitMrp = EDDS_PRM_COMMIT_MRP_INIT_STATE;
                pDDB->pGlob->Prm.currentPort = 0;

                Status = EDDS_HandleHighRQB_byScheduler(pDDB, pRQB);
            }
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:EDDS_RequestPrmCommit(), Status: 0x%X",
        Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmWrite                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_WRITE               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_WRITE_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_INDEX                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_PORTID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_WRITE request       +*/
/*+                                                                         +*/
/*+               WRITE is only allowed after PREPARE or WRITE              +*/
/*+                                                                         +*/
/*+               Note: A PRM error will not cause a state-change!          +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(438,pRecordData)
//JB 11/11/2014 checked within EDDS_Request - pRecordData -> ++ on purpose!
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmWrite(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_PRM_WRITE_PTR_TYPE            pPrmWrite;
    EDDS_PRM_PTR_TYPE                       pPrm;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecordData;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Blocklength;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_BlockType;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Pad;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Slot;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_SubSlot;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_VersionHigh;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_VersionLow;
    LSA_UINT16                              BlockType;
    LSA_UINT16                              BlockLength;
    LSA_UINT8                               VersionHigh;
    LSA_UINT8                               VersionLow;
    LSA_UINT8                               Pad1;
    LSA_UINT8                               Pad2;
    LSA_UINT16                              Slot;
    LSA_UINT16                              SubSlot;
    LSA_UINT8                               Local;
    EDDS_LOCAL_DDB_PTR_TYPE                 pDDB;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecord_Begin;
    EDDS_PRM_DETAIL_ERR_PTR_TYPE            pDetailErr;

    pDDB      = pHDB->pDDB;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmWrite(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);
    pDetailErr    = &pDDB->pGlob->Prm.PrmDetailErr;
    Status    = EDD_STS_OK;
    pPrmWrite = (EDD_UPPER_PRM_WRITE_PTR_TYPE) pRQB->pParam;
    pPrm      = &pDDB->pGlob->Prm;

    pRecord_Begin = pPrmWrite->record_data;
    EDDS_PRM_INIT_DETAIL_ERR(pDetailErr);

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPrmWrite, LSA_NULL) ) ||
         ((pPrmWrite->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt  ) ) ||
         ( LSA_HOST_PTR_ARE_EQUAL( pPrmWrite->record_data, LSA_NULL) )||   
         (   (EDD_PRM_PARAMS_ASSIGNED_REMOTE != pPrmWrite->Local)
          && (EDD_PRM_PARAMS_ASSIGNED_LOCALLY != pPrmWrite->Local))
        )
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_SRV_PRM_WRITE);
        if (EDD_STS_OK == Status)
        {
            switch (pPrmWrite->record_index)
            {
                    /* --------------------------------------------------------------*/
                    /* PDNCDataCheck                                                 */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK:
                    if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                    {
                        if ( pPrmWrite->record_data_length == EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE )
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);
                            pRecord_Blocklength = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            //@fixme am219 implement EDDS_GET_U08_INCR
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            if ( BlockLength == ( EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE - 4) )
                            {
                                if (( BlockType   == EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE    ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH            ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW             ))
                                {
                                    /* check record set and fill RecordSetB if OK */
                                    Status = EDDS_PrmPDNCDataCheckWrite(pDDB,pPrmWrite->record_data);
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDNC: BlockHeader wrong version (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDNC: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Blocklength - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDNC: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDNC: Invalid port_id (<>0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDPortDataAdjust                                              */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST:
                    if ( pPrmWrite->edd_port_id != 0 ) /* 0 not allowed ! */
                    {
                        if (( pPrmWrite->record_data_length >= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE ) &&
                            ( pPrmWrite->record_data_length <= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE ))
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            pRecord_BlockType = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,BlockType);

                            pRecord_Blocklength = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);

                            pRecord_VersionHigh = pRecordData;
                            VersionHigh = *pRecordData++;

                            pRecord_VersionLow = pRecordData;
                            VersionLow  = *pRecordData++;

                            /* --------------------------------*/
                            /*    LSA_UINT8      Padding       */
                            /*    LSA_UINT8      Padding       */
                            /*    LSA_UINT16     SlotNumber    */
                            /*    LSA_UINT16     SubslotNumber */
                            /* --------------------------------*/

                            pRecord_Pad = pRecordData;
                            Pad1 = *pRecordData++;
                            Pad2 = *pRecordData++;

                            pRecord_Slot = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,Slot);
                            pRecord_SubSlot = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,SubSlot);

                            if ( BlockLength == (pPrmWrite->record_data_length -4) )
                            {
                                if( BlockType == EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE )
                                {
                                    if( (Pad1 == 0) && (Pad2 == 0) )
                                    {
                                        if(Slot == pPrmWrite->slot_number)
                                        {
                                            if(SubSlot == pPrmWrite->subslot_number)
                                            {
                                                if(VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH)
                                                {
                                                    if(VersionLow == EDDS_PRM_BLOCK_VERSION_LOW)
                                                    {
                                                        /* store slot/subslot for use with prmread */
                                                        pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].SlotNumber    = Slot;
                                                        pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].SubSlotNumber = SubSlot;

                                                        Local = pPrmWrite->Local;
                                                        /* If LesserCheckQuality we verify the same way as Local.. so set Local */
                                                        if ( pDDB->pGlob->Prm.PortData.LesserCheckQuality ) Local = EDD_PRM_PARAMS_ASSIGNED_LOCALLY;

                                                        /* check record set and fill RecordSetB if OK */
                                                        Status = EDDS_PrmPDPortDataAdjustWrite(pDDB,Local,pPrmWrite->edd_port_id,pPrmWrite->record_data,BlockLength,pRecord_Blocklength);

                                                        /* copy record to SetB if OK */
                                                        if ( Status == EDD_STS_OK )
                                                        {
                                                            EDDS_CopyUpper2Local(pPrmWrite->record_data,
                                                                                 pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].Record,
                                                                                 pPrmWrite->record_data_length);

                                                            pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].RecordLength = pPrmWrite->record_data_length;

                                                        }
                                                    }
                                                    else
                                                    {
                                                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                                            "EDDS_RequestPrmWrite: PDPORT: VersionLow wrong (0x%X)",VersionLow);

                                                        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionLow - pRecord_Begin);

                                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                                    }
                                                }
                                                else
                                                {
                                                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                                        "EDDS_RequestPrmWrite: PDPORT: VersionHigh wrong (0x%X)",VersionHigh);

                                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_VersionHigh - pRecord_Begin);

                                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                                }
                                            }
                                            else
                                            {
                                                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                                    "EDDS_RequestPrmWrite: PDPORT: SubSlot wrong (0x%X)",pPrmWrite->subslot_number);

                                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_SubSlot - pRecord_Begin);

                                                Status = EDD_STS_ERR_PRM_BLOCK;
                                            }
                                        }
                                        else
                                        {
                                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                                "EDDS_RequestPrmWrite: PDPORT: Slot wrong (0x%X)",pPrmWrite->slot_number);

                                            EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Slot - pRecord_Begin);

                                            Status = EDD_STS_ERR_PRM_BLOCK;
                                        }

                                    }
                                    else
                                    {
                                        EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                            "EDDS_RequestPrmWrite: PDPORT: Padding wrong (Pad1: 0x%X, Pad2: 0x%X)",Pad1,Pad2);

                                        EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Pad - pRecord_Begin);

                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDPORT: Blocktype wrong (0x%X)",BlockType);

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_BlockType - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDPORT: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Blocklength - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDPORT: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDPORT: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* PDSCFDataCheck                                                */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK:
                    if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                    {
                        if ( (EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE <= pPrmWrite->record_data_length)&&(EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE >= pPrmWrite->record_data_length) )
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);

                            pRecord_Blocklength = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            if ( BlockLength == (pPrmWrite->record_data_length -4) )
                            {
                                if (( BlockType   == EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_TYPE   ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH            ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW             ))
                                {
                                    /* check record set */
                                    Status = EDDS_PrmPDSCFDataCheckWrite(pDDB,pPrmWrite->record_data);
                                    /* copy record for read back if OK */
                                    if ( Status == EDD_STS_OK )
                                    {
                                        EDDS_CopyUpper2Local(pPrmWrite->record_data,
                                                             pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.Record,
                                                             pPrmWrite->record_data_length);

                                        pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.RecordLength = pPrmWrite->record_data_length;
                                        pDDB->pGlob->Prm.PDSCFDataCheck.RecordSet_B.Present = LSA_TRUE;
                                    }
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDSCF: BlockHeader wrong version (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDSCF: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Blocklength - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDSCF: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDNC: Invalid port_id (<>0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDInterfaceMrpDataAdjust                                      */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDINTERFACE_MRP_DATA_ADJUST:
                    /* we only need the info the a write was done! */
                    pDDB->pGlob->Prm.MRPInterface.Present_B = LSA_TRUE;
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDPortMrpDataAdjust                                           */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDPORT_MRP_DATA_ADJUST:
                    /* we only need the info the a write was done! */
                    if ( pPrmWrite->edd_port_id != 0 ) /* only 0 allowed */
                    {
                        pDDB->pGlob->Prm.MRPPort.Present_B[pPrmWrite->edd_port_id-1] = LSA_TRUE;
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDPORT: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                    /* PDPortMrpIcDataAdjust                                         */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDPORT_MRP_IC_DATA_ADJUST:
                    /* we only need the info the a write was done! */
                    if ( pPrmWrite->edd_port_id != 0 ) 
                    {
                        pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.Present_B[pPrmWrite->edd_port_id-1] = LSA_TRUE;
                        pDDB->pGlob->Prm.SupportsMRPInterconnPortConfig.AtLeastOneRecordPresentB = LSA_TRUE;
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                          "EDDS_RequestPrmWrite: PDPORT MRP_IN: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDSetDefaultPortStates                                        */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES:
                    if ( ! pPrm->FirstCommitExecuted )
                    {
                        /* we only need the info the a write was done! */
                        if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                        {
                            if ( pPrmWrite->record_data_length >= EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_MIN_SIZE )
                            {
                                pRecordData = pPrmWrite->record_data;
                                /* --------------------------------*/
                                /* Record Header + padding         */
                                /* --------------------------------*/
                                EDDS_GET_U16_INCR(pRecordData,BlockType);

                                pRecord_Blocklength = pRecordData;
                                EDDS_GET_U16_INCR(pRecordData,BlockLength);
                                VersionHigh = *pRecordData++;
                                VersionLow  = *pRecordData++;
                                Pad1 = *pRecordData++;
                                Pad2 = *pRecordData++;

                                if ( BlockLength == (pPrmWrite->record_data_length -4) )
                                {
                                    if (( BlockType   == EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_TYPE   ) &&
                                        ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH                ) &&
                                        ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW                 ) &&
                                        ( Pad1        == 0                                          ) &&
                                        ( Pad2        == 0                                          ))
                                    {
                                        /* check record set */
                                        Status = EDDS_PrmPDSetDefPortStatesCheckWrite(pDDB,pPrmWrite->record_data,BlockLength);
                                    }
                                    else
                                    {
                                        EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                            "EDDS_RequestPrmWrite: PDSetDefaultPortStats: BlockHeader wrong (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDSetDefaultPortStats: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);

                                    EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Blocklength - pRecord_Begin);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDSetDefaultPortStats: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                                Status = EDD_STS_ERR_PRM_DATA;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDSetDefaultPortStats: Invalid port_id (=0)!");
                            Status = EDD_STS_ERR_PRM_PORTID;
                        }
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestPrmWrite: PDSetDefaultPortStats: Not allowed because first COMMIT already done!");
                        Status = EDD_STS_ERR_PRM_INDEX;
                    }
                    break;

                /* ---------------------------------------- */
                /* User specific records                    */
                /* These records are not used by EDDS;      */
                /* return OK!                               */
                /* ---------------------------------------- */
                case EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX:
                case EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX:
                case EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX:
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_PRMRequestPrmWrite(): PrmRecord(0x%X) is not used by EDDS",
                        pPrmWrite->record_index);
                }
                break;

                /* --------------------------------------------------------------*/
                /* PDNRTFeedInLoadLimitation                                     */
                /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDNRT_FEED_IN_LOAD_LIMITATION:
                    if ( pPrmWrite->edd_port_id == 0 ) /* only 0 (interface) allowed */
                    {
                        if ( pPrmWrite->record_data_length == EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE )
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);
                            pRecord_Blocklength = pRecordData;
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            if ( BlockLength == EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_LENGTH )
                            {
                                if (( BlockType   == EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_TYPE ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH                       ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW                        ))
                                {
                                    /* check record set and fill RecordSetB if OK */
                                    Status = EDDS_PrmPDNRTFeedInLoadLimitationWrite(pDDB,pPrmWrite->record_data);
                                }
                                else
                                {
                                    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDNRTFeedInLoadLimitation: BlockHeader wrong version (0x%X,%d,%d)", BlockType, VersionHigh, VersionLow);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDNRTFeedInLoadLimitation: BlockHeader wrong length (0x%X,%d)", BlockType, BlockLength);

                                EDDS_PRM_SET_ERR_OFFSET(pDetailErr, pRecord_Blocklength - pRecord_Begin);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDNRTFeedInLoadLimitation: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDNRTFeedInLoadLimitation: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* unknown/unsupported                                           */
                    /* --------------------------------------------------------------*/
                default:
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RequestPrmWrite: record_index (%d)", pPrmWrite->record_index);

                    Status = EDD_STS_ERR_PRM_INDEX;
                    break;
            }
        }
    }

    EDDS_IS_VALID_PTR(pPrmWrite);

    if (pDetailErr->ErrValid)
    {
        EDD_RQB_PRM_WRITE_SET(pPrmWrite,pDetailErr->ErrOffset); /* Offset into pPrmWrite->record_data */
    }
    else
    {
        if (EDD_STS_OK != Status)
        {
            EDD_RQB_PRM_WRITE_SET(pPrmWrite,pDetailErr->ErrOffset); /* Offset into pPrmWrite->record_data */
        }
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmWrite(), Status: 0x%X",
                           Status);

    return(Status);  //lint !e438 JB 03/12/2014 fix NEA6.0, at the moment we increment one time too much (on purpose -> streaming)
}
//lint +esym(438,pRecordData)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmChangePort                   +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE           pRQB       +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE          pHDB       +*/
/*+                             LSA_BOOL                       * pIndicate  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  pIndicate  :                                                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_PENDING                                        +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_CHANGE_PORT request +*/
/*+               called in scheduler-Context                               +*/
/*+               --> no Locks in this function allowed                     +*/
/*+                                                                         +*/
/*+               The RQB will be pushed back to RQB-Context when the       +*/
/*+               scheduler finished                                        +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_SchedulerPrmChangePort(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
    EDD_UPPER_RQB_PTR_TYPE      pRQB)
{
    LSA_RESULT                          Status;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;
    EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE  pRQBParam;

    pDDB = pHDB->pDDB;
    pRQBParam = (EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE) pRQB->pParam;

    Status = EDDS_PrmChangePort_Statemachine(pDDB, pRQBParam->PortID, EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE);

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmChangePort                   +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE           pRQB       +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE          pHDB       +*/
/*+                             LSA_BOOL                       * pIndicate  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  pIndicate  :                                                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_PENDING                                        +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_CHANGE_PORT request +*/
/*+                                                                         +*/
/*+               It indicates if the RQB can be finished immediately or    +*/
/*+               if the scheduler will finish work first                   +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmChangePort(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
    LSA_BOOL                  * pIndicate)
{
    LSA_RESULT                          Status;
    EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE  pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;

    Status    = EDD_STS_OK;
    pRQBParam = (EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE) pRQB->pParam;
    pDDB      = pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :EDDS_RequestPrmChangePort(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
        pRQB,
        pDDB,
        pHDB);

    if(!(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_PHY_POWER_OFF))
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
            "EDDS_RequestPrmChangePort: POWER OFF not supported therefore PrmChangePort not applicable!");
        Status = EDD_STS_ERR_PARAM;
    }
    else if (   (EDD_PRM_APPLY_RECORD_PORT_PARAMS != pRQBParam->ApplyDefaultPortparams)
             && (EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != pRQBParam->ApplyDefaultPortparams))
    {
        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "EDDS_RequestPrmChangePort: ApplyDefaultPortparams (%d) illegal value!", pRQBParam->ApplyDefaultPortparams);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_Prm_Statemachine(pDDB, EDDS_PRM_TRIGGER_TYPE_SRV_PRM_CHANGE_PORT);
        if(EDD_STS_OK == Status)
        {
            EDDS_PRMCHANGEPORT_TRIGGER_TYPE Trigger;
            switch(pRQBParam->ModuleState)
            {
                case EDD_PRM_PORTMODULE_PULLED:
                    Trigger = EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL;
                    break;
                case EDD_PRM_PORTMODULE_PLUG_PREPARE:
                    Trigger = EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE;
                    break;
                case EDD_PRM_PORTMODULE_PLUG_COMMIT:
                {
                    Trigger = EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT;
                    pDDB->pGlob->PrmChangePort[pRQBParam->PortID-1].ApplyDefaultPortparams = pRQBParam->ApplyDefaultPortparams;
                }
                break;
                default:
                {
                    Trigger = EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR;
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                    "EDDS_RequestPrmChangePort: ModuleState(%d) not allowed",
                    pRQBParam->ModuleState);
                    Status = EDD_STS_ERR_PARAM;
                }
                break;
            }

            if(EDD_STS_OK == Status)
            {
                Status = EDDS_PrmChangePort_Statemachine(pDDB, pRQBParam->PortID, Trigger);

                if(EDD_STS_OK_PENDING == Status)
                {
                    *pIndicate = LSA_FALSE;
                    EDDS_ENTER(pDDB->hSysDev);
                    EDDS_RQB_PUT_BLOCK_TOP(pDDB->pGlob->HighPriorGeneralRequestQueue.pBottom, pDDB->pGlob->HighPriorGeneralRequestQueue.pTop, pRQB);
                    EDDS_EXIT(pDDB->hSysDev);
                }
            }
        }
    }

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmChangePort_PlugPrepare              +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE          pDDB       +*/
/*+                             LSA_UINT16                       PortID     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  PortID     : PortID for which the Statemachine is called               +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_EXCP                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: HelperFunction for the PrmChangePort-Statemachine         +*/
/*+               Decode Mautype of RecordSetA and check if still valid     +*/
/*+               called in scheduler-Context                               +*/
/*+               --> no Locks in this function allowed                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**
 * <b>called in scheduler-Context --> no Locks in this function allowed</b>
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmChangePort_PlugPrepare(
    EDDS_LOCAL_DDB_PTR_TYPE           pDDB,
    LSA_UINT16                        PortID)
{
    LSA_RESULT  Status;
    LSA_UINT8   LinkSpeedMode;

    Status = EDD_STS_OK;

    if(PortID < 1 || pDDB->pGlob->HWParams.hardwareParams.PortCnt > EDDS_MAX_PORT_CNT)
    {
        EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "EDDS_PrmChangePort_PlugPrepare: PortID(%d) out of bounds (1..%d)", PortID, EDDS_MAX_PORT_CNT);
        Status = EDD_STS_ERR_PARAM;
    }
    if(EDD_STS_OK == Status)
    {
        if(pDDB->pGlob->Prm.PortData.RecordSet_A[PortID-1].MAUTypePresent)
        {
            LSA_RESULT validMautype;

            EDDS_CHECK_AND_DECODE_MAUTYPE(&validMautype, pDDB->hSysDev, pDDB->hDDB, PortID, pDDB->pGlob->Prm.PortData.RecordSet_A[PortID-1].MAUType, &LinkSpeedMode);
            if(EDD_STS_OK == validMautype)
            {
                pDDB->pGlob->PrmChangePort[PortID-1].LinkSpeedModeApplicable = LinkSpeedMode;
                pDDB->pGlob->PrmChangePort[PortID-1].PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_APPLICABLE;
            }
            else
            {
                pDDB->pGlob->PrmChangePort[PortID-1].LinkSpeedModeApplicable = EDD_LINK_UNKNOWN;
                pDDB->pGlob->PrmChangePort[PortID-1].PortparamsNotApplicable = EDD_PRM_PORT_PARAMS_NOT_APPLICABLE;
            }
        }
    }
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_Prm_Statemachine                       +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE          pDDB       +*/
/*+                             EDDS_PRM_TRIGGER_TYPE            Trigger    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Trigger    : Event that triggers the Statemachine                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_PENDING                                        +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the Prm Statemachine                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_Prm_Statemachine(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDDS_PRM_TRIGGER_TYPE       Trigger)
{
    LSA_RESULT      Status;
    EDDS_PRM_STATE  state;

    Status    = EDD_STS_OK;
    state     = pDDB->pGlob->Prm.PrmState;

    switch(state)
    {
        case EDDS_PRM_STATE_PREPARED:
        {
            switch(Trigger)
            {
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;
                }
                break;

                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_WRITE:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_WRITE;
                }
                break;

                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_END:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_END;
                }
                break;

                // SRV_PRM_CHANGE_PORT allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_CHANGE_PORT:
                {
                }
                break;

                // SRV_PRM_COMMIT not allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_COMMIT:
                case EDDS_PRM_TRIGGER_TYPE_PRM_COMMIT_FINISH:
                case EDDS_PRM_TRIGGER_TYPE_ERROR:
                default:
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_Prm_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);
                    Status = EDD_STS_ERR_SEQUENCE;
                }
                break;
            }
        }
        break;
        case EDDS_PRM_STATE_WRITE:
        {
            switch(Trigger)
            {
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;
                }
                break;

                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_WRITE:
                {
                }
                break;

                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_END:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_END;
                }
                break;

                // SRV_PRM_CHANGE_PORT allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_CHANGE_PORT:
                {
                }
                break;

                // SRV_PRM_COMMIT not allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_COMMIT:
                case EDDS_PRM_TRIGGER_TYPE_PRM_COMMIT_FINISH:
                case EDDS_PRM_TRIGGER_TYPE_ERROR:
                default:
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_Prm_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);
                    Status = EDD_STS_ERR_SEQUENCE;
                }
                break;
            }
        }
        break;
        case EDDS_PRM_STATE_END:
        {
            switch(Trigger)
            {
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;
                }

                break;

                case EDDS_PRM_TRIGGER_TYPE_PRM_COMMIT_FINISH:
                {
                    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;
                }
                break;

                // SRV_PRM_END/SRV_PRM_COMMIT allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_END:
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_COMMIT:
                {
                }
                break;

                // SRV_PRM_WRITE/SRV_PRM_CHANGE_PORT not allowed
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_WRITE:
                case EDDS_PRM_TRIGGER_TYPE_SRV_PRM_CHANGE_PORT:
                case EDDS_PRM_TRIGGER_TYPE_ERROR:
                default:
                {
                    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_Prm_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);
                    Status = EDD_STS_ERR_SEQUENCE;
                }
                break;
            }

        }
        break;
        default:
        {
            EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "EDDS_Prm_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);
            Status = EDD_STS_ERR_SEQUENCE;
        }
        break;
    }

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmChangePort_Statemachine             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE          pDDB       +*/
/*+                             LSA_UINT16                       PortID     +*/
/*+                             EDDS_PRMCHANGEPORT_TRIGGER_TYPE  Trigger    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  PortID     : PortID for which the Statemachine is called               +*/
/*+  Trigger    : Event that triggers the Statemachine                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_PENDING                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the PrmChangePort Statemachine      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmChangePort_Statemachine(
    EDDS_LOCAL_DDB_PTR_TYPE           pDDB,
    LSA_UINT16                        PortID,
    EDDS_PRMCHANGEPORT_TRIGGER_TYPE   Trigger)
{
    LSA_RESULT                  Status;
    EDDS_PRMCHANGEPORT_STATE    state;

    Status    = EDD_STS_OK;

    if(PortID < 1 || pDDB->pGlob->HWParams.hardwareParams.PortCnt > EDDS_MAX_PORT_CNT)
    {
        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "EDDS_PrmChangePort_Statemachine: invalid PortID(%d)", PortID);
        Status = EDD_STS_ERR_PARAM;
    }

    if(EDD_STS_OK == Status)
    {
        state     = pDDB->pGlob->Prm.PrmChangePortState[PortID-1];

        switch(state)
        {
            // stationary state: Transceiver is Plugged
            case EDDS_PRMCHANGEPORT_STATE_ISPLUGGED:
            {
                switch(Trigger)
                {
                    // set Statemachine into transitionary state PULLINPROGRESS and put RQB in Scheduler-Queue
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    {
                        pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_PULLINPROGRESS;
                        EDDS_ENTER(pDDB->hSysDev);
                        pDDB->pGlob->LinkStatus[PortID].IsPulled = EDD_PORTMODULE_IS_PULLED;
                        EDDS_EXIT(pDDB->hSysDev);
                        Status = EDD_STS_OK_PENDING;
                    }
                    break;

                    // SRV_PRM_END/SRV_PRM_COMMIT allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    {
                    }
                    break;

                    // Plug_Prepare/PlugCommit not allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    // no scheduler calls allowed during stationary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // transitionary state: Transceiver is being pulled
            case EDDS_PRMCHANGEPORT_STATE_PULLINPROGRESS:
            {
                switch(Trigger)
                {
                    // scheduler executes LL-Function and sets Statemachine in transitionary state PHYPOWERDOWN
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    {
                        if(EDDS_LL_AVAILABLE(pDDB,changePort))
                        {
                            Status = EDDS_LL_SWITCH_CHANGE_PORT(pDDB, PortID, EDD_PORTMODULE_IS_PULLED);
                            if(EDD_STS_OK == Status)
                            {
                                pDDB->pGlob->LinkStatus[PortID].PortStatus = EDD_PORT_NOT_PRESENT;
                                pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_PHYPOWERDOWN;
                                Status = EDD_STS_OK_PENDING;
                            }
                        }
                        else
                        {
                            Status = EDD_STS_ERR_NOT_SUPPORTED;
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmChangePort_Statemachine: changePort not supported by LL");
                        }
                    }
                    break;

                    // SRV_PRM_END/SRV_PRM_COMMIT allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    {
                    }
                    break;

                    // no SRV_CHANGE_PORT allowed during transitionary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // transitionary state: Transceiver is pulled but LL has to be updated
            case EDDS_PRMCHANGEPORT_STATE_PHYPOWERDOWN:
            {
                switch(Trigger)
                {
                    // scheduler executes LL-Function and sets Statemachine in stationary state ISPULLED
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    {
                        LSA_UINT8 LinkSpeedMode;
                        LSA_UINT8 PHYPower;
                        LinkSpeedMode = EDD_LINK_UNCHANGED;
                        PHYPower = EDDS_PHY_POWER_OFF;
                        Status = EDDS_LL_SET_LINK_STATE(pDDB, PortID, &LinkSpeedMode, &PHYPower);
                        if(EDD_STS_OK == Status)
                        {
                            pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[PortID-1] = LinkSpeedMode;
                            pDDB->pGlob->HWParams.hardwareParams.PHYPower[PortID-1]      = PHYPower;
                            pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_ISPULLED;
                        }
                    }
                    break;

                    // SRV_PRM_END/SRV_PRM_COMMIT allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    {
                    }
                    break;

                    // no SRV_CHANGE_PORT allowed during transitionary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // stationary state: Transceiver is pulled
            case EDDS_PRMCHANGEPORT_STATE_ISPULLED:
            {
                switch(Trigger)
                {
                    // set Statemachine into transitionary state GETLINKSTATE and put RQB in Scheduler-Queue
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    {
                        pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_GETLINKSTATE;
                        Status = EDD_STS_OK_PENDING;
                    }
                    break;

                    // SRV_PRM_END/SRV_PRM_COMMIT allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    // PORT_PULL has no effect during state ISPULLED
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    {
                    }
                    break;

                    // PlugCommit not allowed
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    // no scheduler calls allowed during stationary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // transitionary state: Transceiver will be prepared for plugging
            case EDDS_PRMCHANGEPORT_STATE_GETLINKSTATE:
            {
                switch(Trigger)
                {
                    // scheduler gets LinkState-Parameter and sets Statemachine in stationary state PLUGGING
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    {
                        EDDS_LINK_STAT_PTR_TYPE                     pLinkStat;
                        EDD_RQB_GET_LINK_STATUS_TYPE                LinkStatus;
                        LSA_UINT16                                  MAUType;
                        LSA_UINT8                                   MediaType;
                        LSA_UINT8                                   IsPOF;
                        LSA_UINT32                                  PortStatus;
                        LSA_UINT32                                  AutonegCapAdvertised;
                        LSA_UINT8                                   LinkSpeedModeConfigured;

                        pLinkStat = &pDDB->pGlob->LinkStatus[PortID];
                        Status = EDDS_LL_GET_LINK_STATE(pDDB,PortID,&LinkStatus,&MAUType,&MediaType,&IsPOF,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);
                        if(EDD_STS_OK == Status)
                        {
                            Status = EDDS_PrmChangePort_PlugPrepare(pDDB, PortID);
                            if(EDD_STS_OK == Status)
                            {
                                if (( MediaType             != pLinkStat->MediaType            ) ||
                                    ( AutonegCapAdvertised  != pLinkStat->AutonegCapAdvertised ) ||
                                    ( IsPOF                 != pLinkStat->IsPOF                ))
                                {
                                    /* copy new status to actual status */
                                    pLinkStat->IsPOF                = IsPOF;
                                    pLinkStat->MediaType            = MediaType;
                                    pLinkStat->AutonegCapAdvertised = AutonegCapAdvertised;
                                }

                                pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_PLUGGING;
                            }
                        }
                    }
                    break;

                    // no SRV_CHANGE_PORT allowed during transitionary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    // SRV_PRM_END/SRV_PRM_COMMIT not allowed during plugging sequence
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // stationary state: Transceiver is prepared for plugging
            case EDDS_PRMCHANGEPORT_STATE_PLUGGING:
            {
                switch(Trigger)
                {
                    // set Statemachine into transitionary state PLUGINPROGRESS and put RQB in Scheduler-Queue
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    {
                        pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_PLUGINPROGRESS;
                        Status = EDD_STS_OK_PENDING;
                    }
                    break;

                    // set Statemachine into stationary state ISPULLED
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    {
                        pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_ISPULLED;
                    }
                    break;

                    // set Statemachine into transitionary state GETLINKSTATE and put RQB in Scheduler-Queue
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    {
                        pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_GETLINKSTATE;
                            Status = EDD_STS_OK_PENDING;
                    }
                    break;

                    // SRV_PRM_END/SRV_PRM_COMMIT not allowed during plugging sequence
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    // no scheduler calls allowed during stationary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // transitionary state: Transceiver plugging is in progress
            case EDDS_PRMCHANGEPORT_STATE_PLUGINPROGRESS:
            {
                switch(Trigger)
                {
                    // scheduler executes LL-Function and sets Statemachine in transitionary state PHYPOWERUP
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    {
                        if(EDDS_LL_AVAILABLE(pDDB,changePort))
                        {
                            Status = EDDS_LL_SWITCH_CHANGE_PORT(pDDB, PortID, EDD_PORTMODULE_IS_PLUGGED);
                            if(EDD_STS_OK == Status)
                            {
                                pDDB->pGlob->LinkStatus[PortID].PortStatus = EDD_PORT_PRESENT;
                                pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_PHYPOWERUP;
                                Status = EDD_STS_OK_PENDING;
                            }
                        }
                        else
                        {
                            Status = EDD_STS_ERR_NOT_SUPPORTED;
                            EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmChangePort_Statemachine: changePort not supported by LL");
                        }
                    }
                    break;

                    // no SRV_CHANGE_PORT allowed during transitionary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    // SRV_PRM_END/SRV_PRM_COMMIT not allowed during plugging sequence
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            // transitionary state: Transceiver is plugged but LL has to be updated
            case EDDS_PRMCHANGEPORT_STATE_PHYPOWERUP:
            {
                switch(Trigger)
                {
                    // scheduler executes LL-Function and sets Statemachine in stationary state ISPLUGGED
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE:
                    {
                        LSA_UINT8 PHYPower;
                        LSA_UINT8 LinkSpeedMode;
                        EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE pRecordSetA;

                        pRecordSetA = &pDDB->pGlob->Prm.PortData.RecordSet_A[PortID-1];

                        if( !(pRecordSetA->PortStatePresent) )
                        {
                            PHYPower = EDDS_PHY_POWER_ON;

                            if(EDD_PRM_APPLY_DEFAULT_PORT_PARAMS == pDDB->pGlob->PrmChangePort[PortID-1].ApplyDefaultPortparams)
                            {
                                    LinkSpeedMode = EDD_LINK_AUTONEG;

                                    /* Store new configured Autoneg setting */
                                    pDDB->pGlob->LinkStatus[PortID].Autoneg = EDD_AUTONEG_ON;
                            }
                            else
                            {
                                if(pRecordSetA->MAUTypePresent)
                                {
                                    LinkSpeedMode = pDDB->pGlob->PrmChangePort[PortID-1].LinkSpeedModeApplicable;
                                }
                                else
                                {
                                    LinkSpeedMode = EDD_LINK_AUTONEG;
                                }
                            }
                        }
                        else
                        {
                            PHYPower = EDDS_PHY_POWER_OFF;
                            LinkSpeedMode = EDD_LINK_UNCHANGED;
                        }
                        Status = EDDS_LL_SET_LINK_STATE(pDDB, PortID, &LinkSpeedMode, &PHYPower);
                        if(EDD_STS_OK == Status)
                        {
                            pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[PortID-1] = LinkSpeedMode;
                            pDDB->pGlob->HWParams.hardwareParams.PHYPower[PortID-1] = PHYPower;

                            pDDB->pGlob->Prm.PrmChangePortState[PortID-1] = EDDS_PRMCHANGEPORT_STATE_ISPLUGGED;
                            pDDB->pGlob->LinkStatus[PortID].IsPulled = EDD_PORTMODULE_IS_PLUGGED;
                        }
                    }
                    break;

                    // no SRV_CHANGE_PORT allowed during transitionary state
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT:
                    // SRV_PRM_END/SRV_PRM_COMMIT not allowed during plugging sequence
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT:
                    case EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR:
                    default:
                    {
                        EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                        Status = EDD_STS_ERR_SEQUENCE;
                    }
                    break;
                }
            }
            break;

            default:
            {
                EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "EDDS_PrmChangePort_Statemachine: Wrong state (%d)", pDDB->pGlob->Prm.PrmChangePortState[PortID-1]);
                Status = EDD_STS_ERR_SEQUENCE;
            }
            break;
        }
    }
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmIndication                   +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_INDICATION          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_WRITE_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_CANCEL                                         +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_INDICATION request  +*/
/*+                                                                         +*/
/*+               PortID 0 = Interface                                      +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmIndication(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                        	Status;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE 	pRQBParam;
    LSA_UINT16                        	PortID;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmIndication(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRQBParam = (EDD_UPPER_PRM_INDICATION_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) )||
         ((pRQBParam->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt)) )
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmIndication: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        PortID = pRQBParam->edd_port_id;

        EDDS_RQB_PUT_BLOCK_TOP( pDDB->pGlob->Prm.PrmIndReq[PortID].pBottom,
                                pDDB->pGlob->Prm.PrmIndReq[PortID].pTop,
                                pRQB);

        pRQBParam->diag_cnt = 0; /* init with 0 */

        /* check if something to indicate */
        EDDS_PrmIndicatePort(pDDB,PortID);

    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmIndication(), Status: 0x%X",
                           Status);


    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmRequest                             +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
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
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_READ                +*/
/*+                                         EDD_SRV_PRM_PREPARE             +*/
/*+                                         EDD_SRV_PRM_WRITE               +*/
/*+                                         EDD_SRV_PRM_INDICATION          +*/
/*+                                         EDD_SRV_PRM_END                 +*/
/*+                                         EDD_SRV_PRM_COMMIT              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all EDDS General (Component         +*/
/*+               independent) requests.                                    +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    LSA_BOOL                        Indicate;
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB;
    LSA_UINT16						portIndex;

    pDDB    = pHDB->pDDB;
    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmRequest(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;
    Indicate = LSA_TRUE;

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_PRM_READ:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_READ (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestPrmRead(pRQB,pHDB);
            if(EDD_STS_OK_PENDING == Status)
            {
                Indicate = LSA_FALSE;
            }
            break;
        case EDD_SRV_PRM_PREPARE:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_PREPARE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestPrmPrepare(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_WRITE:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_WRITE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestPrmWrite(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_INDICATION:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_INDICATION (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestPrmIndication(pRQB,pHDB);
            if ( EDD_STS_OK == Status )
                Indicate = LSA_FALSE;
            break;
        case EDD_SRV_PRM_END:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_END (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            for(portIndex = 1; portIndex <= pDDB->pGlob->HWParams.Caps.PortCnt; portIndex++)
            {
                Status = EDDS_PrmChangePort_Statemachine(pDDB, portIndex, EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END);
                if(EDD_STS_OK != Status)
                {
                    break;
                }
            }
            if(EDD_STS_OK == Status)
            {
                Status = EDDS_RequestPrmEnd(pRQB,pHDB);
            }

            break;
        case EDD_SRV_PRM_COMMIT:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_COMMIT (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            for(portIndex = 1; portIndex <= pDDB->pGlob->HWParams.Caps.PortCnt; portIndex++)
            {
                Status = EDDS_PrmChangePort_Statemachine(pDDB, portIndex, EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT);
                if(EDD_STS_OK != Status)
                {
                    break;
                }
            }
            if(EDD_STS_OK == Status)
            {
                Status = EDDS_RequestPrmCommit(pRQB,pHDB);
                if(EDD_STS_OK_PENDING == Status)
                {
                    Indicate = LSA_FALSE;
                }
                /* Indicate all neccessary. Has to be done after COMMIT confirmation! */
            }
            break;
        case EDD_SRV_PRM_CHANGE_PORT:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_CHANGE_PORT (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestPrmChangePort(pRQB, pHDB, &Indicate);
            break;
        default:
            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_PrmRequest: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }


    if (Indicate) EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmRequest(), Status: 0x%X",
                           Status);

}



/*****************************************************************************/
/*  end of file EDDS_PRM.C                                                   */
/*****************************************************************************/

