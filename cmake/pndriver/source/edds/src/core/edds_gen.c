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
/*  F i l e               &F: edds_gen.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS General and Debug requests                  */
/*                                                                           */
/*                          - EddsGeneralRequest()                           */
/*                          - EddsDebugRequest()                             */
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
#define LTRC_ACT_MODUL_ID  8
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_GEN */

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
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_SetupLinkRQBParams                             +*/
/*+  Input/Output   :    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE   pParam+*/
/*+                      EDDS_LINK_STAT_PTR_TYPE  pLinkStat                 +*/
/*+  Result         :    --                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup Link RQB Params with actual ones from DDB           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SetupLinkParams(
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam,
    EDDS_LINK_STAT_PTR_TO_CONST_TYPE            pLinkStat
)
{
    pParam->Status                   = pLinkStat->Status;
    pParam->PhyStatus                = pLinkStat->PhyStatus;
    pParam->Speed                    = pLinkStat->Speed;
    pParam->Mode                     = pLinkStat->Mode;
    pParam->Autoneg                  = pLinkStat->Autoneg;
    pParam->PortStatus               = pLinkStat->PortStatus;
    pParam->IRTPortStatus            = EDD_IRT_NOT_SUPPORTED;
    pParam->RTClass2_PortStatus      = EDD_RTCLASS2_NOT_SUPPORTED;
    pParam->MAUType                  = pLinkStat->MAUType;
    pParam->MediaType                = pLinkStat->MediaType;
    pParam->IsPOF                    = pLinkStat->IsPOF;
    pParam->LineDelayInTicksMeasured = pLinkStat->LineDelay;
    pParam->CableDelayInNsMeasured   = pLinkStat->CableDelayNs;
    pParam->PortTxDelay              = 0;
    pParam->PortRxDelay              = 0;
    pParam->PortState                = pLinkStat->PortState;
    pParam->SyncId0_TopoOk           = EDD_PORT_SYNCID0_TOPO_NOT_OK;
    pParam->ShortPreamble            = EDD_PORT_LONG_PREAMBLE;
    pParam->TxFragmentation          = EDD_PORT_NO_TX_FRAGMENTATION;
    pParam->AddCheckEnabled          = pLinkStat->AddCheckEnabled;
    pParam->ChangeReason             = pLinkStat->ChangeReason;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :   EDDS_ValidateMcMac                                  +*/
/*+  Input          :   EDDS_LOCAL_DDB_PTR_TYPE    pDDB                     +*/
/*+                     EDD_MAC_ADR_TYPE           Mac                      +*/
/*+  Result         :   bool'sch                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description    :   validates mac for EDD_SRV_MULTICAST                 +*/
/*+                     allowed: multicast mac addresses or CARP IF mac     +*/
/*+                     (only if cluster ip support enabled!)               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL EDDS_ValidateMcMacForAdd(
    EDDS_LOCAL_DDB_PTR_TYPE     const   pDDB,
    EDD_MAC_ADR_TYPE                    Mac
)
{
    /* default: validation failed */
    LSA_BOOL ret = LSA_FALSE;
    
    /* valid mac addresses: mc mac or carp uc mac or hsr mac */
    
    /* mc? */
    if(EDDS_IS_MC_MAC(Mac.MacAdr))
    {
        ret = LSA_TRUE;
    }
    /* hsr uc mac? */
    else if (EDDS_IS_HSR_MAC(Mac.MacAdr))
    {
        ret = LSA_TRUE;
    }
    /* carp mac? */
    else if(    (EDDS_IS_CARP_MAC(Mac.MacAdr))
            && (pDDB->pNRT->CARPMacCnt < 1)  /* only one CARP mac at a time */
            && (pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_CLUSTER_IP_SUPPORT)
            && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ClusterIPSupport) 
            )
    {
        ++pDDB->pNRT->CARPMacCnt;
        ret = LSA_TRUE;
    }
    
    return ret;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_HandleLinkIndTrigger                          +*/
/*+  Input          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB                   +*/
/*+  Result         :    --                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a (ext)link change for all ports with a pending +*/
/*+               link / port change for all channels with an indication    +*/
/*+               ressource                                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandleLinkIndTrigger(
        EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT16                                  Index;
    EDDS_LOCAL_HDB_PTR_TYPE                     pHDB;
    EDD_UPPER_RQB_PTR_TYPE                      pRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam;
    LSA_UINT32                                  i;
    LSA_BOOL                                    LinkChanged,PortChanged;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleLinkIndTrigger(pDDB: 0x%X)",
                           pDDB);

    pHDB   = LSA_NULL;

    LinkChanged = LSA_FALSE;
    PortChanged = LSA_FALSE;

    /* --------------------------------------------------------------------------*/
    /* Loop all ports and get actual link states                                 */
    /* --------------------------------------------------------------------------*/

    for ( i=0; i<= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        /* should not interrupted by scheduler context */
        /* because we read and write shared resources */
        /* xxx.LinkChange.LinkStateChanged */
        /* xxx.LinkChange.PortStateChanged */
        EDDS_ENTER(pDDB->hSysDev);

        if ( EDDS_LINK_STATE_CHANGE_PENDING == pDDB->pGlob->LinkStatus[i].LinkChange.LinkStateChanged)
        {
            /* save local */
            LinkChanged = LSA_TRUE;
            /* and reset in shared resource */
            pDDB->pGlob->LinkStatus[i].LinkChange.LinkStateChanged = EDDS_LINK_STATE_NO_CHANGE;
        }
        else
        {
            LinkChanged = LSA_FALSE;
        }

        if( EDDS_LINK_STATE_CHANGE_PENDING == pDDB->pGlob->LinkStatus[i].LinkChange.PortStateChanged)
        {
            /* save local */
            PortChanged = LSA_TRUE;
            /* and reset in shared resource */
            pDDB->pGlob->LinkStatus[i].LinkChange.PortStateChanged = EDDS_LINK_STATE_NO_CHANGE;
        }
        else
        {
            PortChanged = LSA_FALSE;
        }

        EDDS_EXIT(pDDB->hSysDev);

        if ( LinkChanged || PortChanged )
        {
            Index = 0;

            /* loop over all handles owned by DDB */
            while ( Index < pDDB->HDBMgmt.MaxHandleCnt )
            {                
                pHDB = &pDDB->HDBMgmt.HDBTable[Index];
                
                if (( pHDB->InUse ) &&
                    ( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->LinkIndReq[i].pBottom,LSA_NULL)))  /* Indication RQB present ? */
                {
                    EDDS_ASSERT(pDDB == pHDB->pDDB);
                    
                    /* PortChanged only indicated if EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT */
                    /* or LinkChanged too.                                               */

                    if ( ( LinkChanged ) ||
                         ( EDD_RQB_GET_SERVICE(pHDB->LinkIndReq[i].pBottom) == EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT))
                    {

                        EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[i].pBottom,
                                                  pHDB->LinkIndReq[i].pTop,
                                                  pRQB);

                        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* should be, already checked */
                        {
                            pParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

                            EDDS_SetupLinkParams(pParam,&pDDB->pGlob->LinkStatus[i]); /* copy actual parameters */

                            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
                        }
                    }
                }

                Index++;
            }
        }

    } /* for */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IndicateExtLinkChange                         +*/
/*+  Input          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB                   +*/
/*+                      LSA_UINT32                  PortID (0=auto,1..x)   +*/
/*+  Result         :    --                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a ext link change to all channels with          +*/
/*+               indication resource.                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//JB 11/11/2014 check within EDDS_GetDDB (previously called)
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_IndicateExtLinkChange(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  PortID
)
{
    LSA_UINT16                              Index;
    EDDS_LOCAL_HDB_PTR_TYPE                 pHDB;
    EDD_UPPER_RQB_PTR_TYPE                  pRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam;

    Index = 0;

    /* Because this sequence runs in RQB context and must not */
    /* be interrupted by scheduler we have to use EDDS_ENTER */

    EDDS_ENTER(pDDB->hSysDev);

    /* loop over all handles owned by DDB and check if we have to indicate a change */

    while ( Index < pDDB->HDBMgmt.MaxHandleCnt )
    {
        pHDB = &pDDB->HDBMgmt.HDBTable[Index];

        if ( pHDB->InUse )
        {
            EDDS_ASSERT( pHDB->pDDB == pDDB );
            
            /* only if we have a EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT pending */

            if (( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->LinkIndReq[PortID].pBottom, LSA_NULL)) &&
                ( EDD_RQB_GET_SERVICE(pHDB->LinkIndReq[PortID].pBottom) == EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT ))
            {

                EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[PortID].pBottom,
                                          pHDB->LinkIndReq[PortID].pTop,
                                          pRQB);

                pParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

                EDDS_SetupLinkParams(pParam,&pDDB->pGlob->LinkStatus[PortID]);
                EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
            }
        }
        Index++;

    }

    EDDS_EXIT(pDDB->hSysDev);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandlePHYEvent                         +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Results    : LSA_VOID                                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the PHY-Interrupt request.          +*/
/*+                                                                         +*/
/*+               This function is called if a link-status change was       +*/
/*+               detected.                                                 +*/
/*+               We will determine the acutal Link-Status and store it     +*/
/*+               within device-management.                                 +*/
/*+                                                                         +*/
/*+               If Indication-resources are present, we indicate the      +*/
/*+               Status-change.                                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandlePHYEvent(
    EDDS_LOCAL_DDB_PTR_TYPE  const   pDDB)
{

    LSA_RESULT                                  Status;
    EDD_RQB_GET_LINK_STATUS_TYPE                LinkStatus;
    LSA_UINT32                                  i;
    LSA_UINT16                                  MAUType;
    LSA_UINT8                                   MediaType;
    LSA_UINT8                                   IsPOF;
    LSA_UINT32                                  AutonegCapAdvertised;
    LSA_UINT8                                   LinkSpeedModeConfigured;
    LSA_UINT32                                  PortStatus;
    LSA_BOOL                                    LinkChanged,PortChanged;
    LSA_BOOL                                    LinkChangedOverall; /* there was a link change */
    LSA_BOOL                                    ChangeEventPending; /* there was any change (link or port) */
    EDDS_LINK_STAT_PTR_TYPE                     pLinkStat;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandlePHYEvent(pDDB: 0x%X)",
                           pDDB);

    Status = EDD_STS_OK;

    LinkChanged = LSA_FALSE;
    PortChanged = LSA_FALSE;
    LinkChangedOverall = LSA_FALSE;
    ChangeEventPending = LSA_FALSE;

    /* --------------------------------------------------------------------------*/
    /* Loop all ports and get actual link states                                 */
    /* --------------------------------------------------------------------------*/

    for ( i=1; i<= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,i);

        /* Init with Default Params, because automatic initialization not supported by all Lower Layers */
        LinkStatus.AddCheckEnabled = EDD_LINK_ADD_CHECK_DISABLED;
        LinkStatus.ChangeReason    = EDD_LINK_CHANGE_REASON_NONE;

        /* NOTE: LinkSpeedModeConfigured is not used here                          */
        /*       whenever LL_SET_LINK_STATE is called the actual phy setup is read */
        Status = EDDS_LL_GET_LINK_STATE(pDDB,i,&LinkStatus,&MAUType,&MediaType,&IsPOF,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);

        EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(Port: %d, Mode: %d,Speed: %d, Stat: %d)",
                            i,
                            LinkStatus.Mode,
                            LinkStatus.Speed,
                            LinkStatus.Status);

        EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(PMAUType: %d, MediaType: %d,PortStatus: %d",
                            MAUType,
                            MediaType,
                            PortStatus);

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(AutonegCap: 0x%X, Status : 0x%X",
                            AutonegCapAdvertised,
                            Status);

        if ( Status != EDD_STS_OK )
        {
            LinkStatus.Mode   = EDD_LINK_UNKNOWN;
            LinkStatus.Speed  = EDD_LINK_UNKNOWN;
            LinkStatus.Status = EDD_LINK_DOWN;  /* On error we signal link down */
            MAUType           = EDD_MAUTYPE_UNKNOWN;
            MediaType         = EDD_MEDIATYPE_UNKNOWN;
            IsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
            PortStatus        = EDD_PORT_NOT_PRESENT;
            AutonegCapAdvertised     = 0;
        }

        /* Check if there are changes to the actual values. if so we */
        /* indicate to all handles which have indication resources   */

        LinkChanged = LSA_FALSE;
        PortChanged = LSA_FALSE;
        pLinkStat   = &pDDB->pGlob->LinkStatus[i];


        if (( LinkStatus.Status != pLinkStat->Status    ) ||
            ( LinkStatus.Mode   != pLinkStat->Mode      ) ||
            ( LinkStatus.Speed  != pLinkStat->Speed     ))
        {
            LinkChanged = LSA_TRUE;
            LinkChangedOverall = LSA_TRUE;
            ChangeEventPending = LSA_TRUE;
            /* save changed states in shared resources (shared with rqb context) */
            pDDB->pGlob->LinkStatus[i].LinkChange.LinkStateChanged = EDDS_LINK_STATE_CHANGE_PENDING;

            if( EDD_LINK_DOWN == LinkStatus.Status )
            {
                EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_WARN,
                                       "EDDS_HandlePHYEvent(), Link has changed and is now EDD_LINK_DOWN. LinkStatus.Status: 0x%X",
                                       LinkStatus.Status);
            }
        }
        
        if ( MAUType != pLinkStat->MAUType )
        {
            PortChanged = LSA_TRUE;
            ChangeEventPending = LSA_TRUE;
            /* save changed states in shared resources (shared with rqb context) */
            pDDB->pGlob->LinkStatus[i].LinkChange.PortStateChanged = EDDS_LINK_STATE_CHANGE_PENDING;
        }

        /* NOTE PortStatus: PortStatus only depends on AdjustLinkState sub block within PDPortDataAdjust */
        
        if( LinkChanged || PortChanged )
        {
            /* copy new status to actual status */
            pLinkStat->Status          = LinkStatus.Status;
            pLinkStat->Mode            = LinkStatus.Mode;
            pLinkStat->Speed           = LinkStatus.Speed;
            pLinkStat->MAUType         = MAUType;
            pLinkStat->AddCheckEnabled = LinkStatus.AddCheckEnabled;
            pLinkStat->ChangeReason    = LinkStatus.ChangeReason;   
            //pLinkStat->MediaType     = MediaType;
            //pLinkStat->IsPOF         = IsPOF;
            //pLinkStat->PortStatus    = (LSA_UINT8) PortStatus;
            //pLinkStat->AutonegCapAdvertised = AutonegCapAdvertised;
        }

    } /* for */

    /*---------------------------------------------------------------------------*/
    /* init AUTO mode setting. This is the fastest mode present on any port      */
    /*---------------------------------------------------------------------------*/

    if ( EDDS_FindLinkAutoMode(pDDB) ) /* if AUTO link changed .. */
    {
        /* link change of AUTO_PORT */
        /* save changed states in shared resources (shared with rqb context) */
        pDDB->pGlob->LinkStatus[0].LinkChange.LinkStateChanged = EDDS_LINK_STATE_CHANGE_PENDING;

        ChangeEventPending = LSA_TRUE;

    }

    /*---------------------------------------------------------------------------*/

    /* setup bandwith of Tx and Rx */
    if( LinkChangedOverall )
    {
        EDDS_CalculateTxRxBandwith(pDDB);
    }

    /* generate trigger to rqb context */
    if(ChangeEventPending)
    {
        pDDB->pGlob->SchedulerEventsDonePending |= EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__LINK_CHANGED;
    }

    EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandlePHYEvent()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GetMcMACEntry                          +*/
/*+  Input/Output               EDDS_MC_MAC_INFO_PTR_TYPE            pMCInfo+*/
/*+                        :    EDD_UPPER_MEM_U8_PTR_TYPE            pMCAddr+*/
/*+                             LSA_UIN32                           *pIdx   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pMCInfo              : Pointer to MC Management                        +*/
/*+  pMCAddr              : pointer to MC-MAC address (6 Bytes)             +*/
/*+  pIdx                 : Returned Index within MC-Info                   +*/
/*+                         if result = LSA_TRUE :Index of used MC entry    +*/
/*+                         if result = LSA_FALSE:Index of free MC entry    +*/
/*+                                               or EDDS_MC_NO_IDX         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs MC-management for pMCAddr. if one is found        +*/
/*+               result is set to LSA_TRUE and pIdx gets the index.        +*/
/*+               If not not found (LSA_FALSE), but a free empty entry is   +*/
/*+               present pIdx gets the idx. If no free or used entry then  +*/
/*+               pIdx gets EDDS_MC_NO_IDX                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 pIdx will be initialized within this function -> no danger of null ptr
//AD_DISCUSS 20/11/2014 what about if pIdx point to null
static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_GetMcMACEntry(
    EDDS_MC_MAC_INFO_PTR_TYPE            pMCInfo,
    LSA_UINT8 const *                    pMCAddr,
    LSA_UINT32                           *pIdx)
{

    EDDS_MC_MAC_PTR_TYPE pMC;
    LSA_BOOL            UsedFound;
    LSA_BOOL            FreeFound;
    LSA_UINT32          i;

    UsedFound = LSA_FALSE;
    FreeFound = LSA_FALSE;
    i         = 0;
    pMC       = &pMCInfo->McMac[0];
    *pIdx     = EDDS_MC_NO_IDX; /* none */
    while (( i< EDDS_MAX_MC_MAC_CNT ) && ( ! UsedFound ))
    {
        if (pMC->Cnt) /* Entry used */
        {
            if ((pMC->MAC.MacAdr[5] == pMCAddr[5]) &&
                (pMC->MAC.MacAdr[4] == pMCAddr[4]) &&
                (pMC->MAC.MacAdr[3] == pMCAddr[3]) &&
                (pMC->MAC.MacAdr[2] == pMCAddr[2]) &&
                (pMC->MAC.MacAdr[1] == pMCAddr[1]) &&
                (pMC->MAC.MacAdr[0] == pMCAddr[0]))
            {
                UsedFound   = LSA_TRUE;
                *pIdx       = i;
            }
        }
        else
        {
            if ( ! FreeFound )
            {
                FreeFound   = LSA_TRUE;
                *pIdx       = i;
            }
        }

        i++;
        pMC++;
    }

    return(UsedFound);

}


/** Handle high priority RQB by edds_scheduler.
 * 
 * The given RQB is enqueued into high priority RQB queue and edds_scheduler is
 * triggered or called directly. The response from this RQB is always enqueued in
 * EDDS request RQB context queue.
 * 
 * @param pDDB  reference to EDDS interface.
 * @param pRQB  reference to RQB that should be handled.
 * @return      EDD_STS_OK_PENDING is returned, if the RQB is enqueued into scheduler high request queue.
 * 
 * @note        Actually only EDD_STS_OK_PENDING is returned, even if edds_scheduler is called directly;
 *              the response from this RQB is always enqueued in edds-request RQB context queue.  
 */
LSA_RESULT EDDS_HandleHighRQB_byScheduler (EDDS_LOCAL_DDB_PTR_TYPE pDDB, EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT Status;
    
    /* push to edds scheduler high rqb queue */
    EDDS_ENTER(pDDB->hSysDev);
    EDDS_RQB_PUT_BLOCK_TOP(pDDB->pGlob->HighPriorGeneralRequestQueue.pBottom,
                           pDDB->pGlob->HighPriorGeneralRequestQueue.pTop, pRQB);
    EDDS_EXIT(pDDB->hSysDev);
    
    /* An EDDS high prio RQB has been put into the high priority queue to the edds_scheduler.
     * call the edds_scheduler:
     * - indirectly, trigger it with EDDS_DO_NOTIFY_SCHEDULER
     * - directly, if in NRT mode and NRT_UseNotifySchedulerCall is set
     */
    if((pDDB->pGlob->IO_Configured) || (pDDB->pGlob->NRT_UseNotifySchedulerCall) )
    {
        EDDS_DO_NOTIFY_SCHEDULER(pDDB->hSysDev);
    }
    else
    {
        /* call edds_scheduler with "intermediate call" */
        edds_scheduler(pDDB->hDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);
    }    
    
    Status = EDD_STS_OK_PENDING;
    
    return Status;
}


/** Handle RQB by edds_scheduler.
 * 
 * The given RQB is enqueued into general request queue and edds_scheduler is
 * later triggered by cyclic event or called directly. The response from this RQB is always enqueued in
 * EDDS request RQB context queue.
 * 
 * @param pDDB  reference to EDDS interface.
 * @param pRQB  reference to RQB that should be handled.
 * @return      EDD_STS_OK_PENDING is returned, if the RQB is enqueued into scheduler high request queue.
 * 
 * @note        Actually only EDD_STS_OK_PENDING is returned, even if edds_scheduler is called directly;
 *              the response from this RQB is always enqueued in edds-request RQB context queue.  
 */LSA_RESULT EDDS_HandleRQB_byScheduler (EDDS_LOCAL_DDB_PTR_TYPE pDDB, EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT Status;
    
    /* push to edds scheduler rqb queue */
    EDDS_ENTER(pDDB->hSysDev);
    EDDS_RQB_PUT_BLOCK_TOP( pDDB->pGlob->GeneralRequestQueue.pBottom,
                            pDDB->pGlob->GeneralRequestQueue.pTop,
                            pRQB);
    EDDS_EXIT(pDDB->hSysDev);

    /* An EDDS RQB has been put into the general request queue to the edds_scheduler.
     * call the edds_scheduler:
     * - indirectly, cyclic call of scheduler will handle the enqueued RQB
     * - directly, if in NRT mode and NRT_UseNotifySchedulerCall is set
     */
    if((pDDB->pGlob->IO_Configured) || (pDDB->pGlob->NRT_UseNotifySchedulerCall) )
    {
        /* EDDS_DO_NOTIFY_SCHEDULER(pDDB->hSysDev);
         * do nothing, cyclic call will handle the enqueued RQB
         */         
    }
    else
    {
        /* call edds_scheduler with "intermediate call" */
        edds_scheduler(pDDB->hDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);
    }    
    
    Status = EDD_STS_OK_PENDING;
    
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestMC                              +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_MULTICAST               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_MULTICAST_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+     LSA_UINT8               Mode            Mode                        +*/
/*+                                             EDDS_MUTLICAST_ENABLE       +*/
/*+                                             EDD_MULTICAST_DISABLE       +*/
/*+                                             EDD_MULTICAST_DISABLE_ALL   +*/
/*+                                                                         +*/
/*+     EDD_MAC_ADR_TYPE        MACAddr         6-Byte MC address if        +*/
/*+                                             Mode <> DISABLE_ALL         +*/
/*+  RQB-will not be changed.                                               +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_MULTICAST request.      +*/
/*+                                                                         +*/
/*+               Depending on Mode the spezified MACAddr will be enabled   +*/
/*+               or disabled.                                              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: MACAddr is no multicast address or     +*/
/*+                                  MACAddr not found (on disable)         +*/
/*+                                  pParam is LSA_NULL                     +*/
/*+                                  Mode unknown                           +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_RESOURCE: No free entry for Multicastaddress  +*/
/*+                                                                         +*/
/*+               The EDDS checks if a MC Address already enabled. If so    +*/
/*+               the EDDS only increments a "used" counter. So the LLIF    +*/
/*+               dont gets a ENABLE for the same MC-Address again.         +*/
/*+                                                                         +*/
/*+               On disable, this counter is decremented and the MC        +*/
/*+               address is only disabled within LLIF if the counter       +*/
/*+               reaches 0 (not in use anymore)                            +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestMC(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDD_UPPER_MULTICAST_PTR_TYPE        pMCParam;
    LSA_UINT32                          Idx;
    EDDS_MC_MAC_INFO_PTR_TYPE           pMCInfo;
    LSA_UINT32                          i;
    EDDS_LOCAL_DDB_PTR_TYPE     		pDDB;

    pDDB  = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestMC(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pMCParam = (EDD_UPPER_MULTICAST_PTR_TYPE) pRQB->pParam;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pMCParam, LSA_NULL) )
    {
        pMCInfo = pDDB->pGlob->pMCInfo;
        pMCParam->RefCnt = 0;

        switch (pMCParam->Mode)
        {
                /* ------------------------------------------------------------*/
                /* enable one MC-MACs                                          */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_ENABLE:
            {
                /* check if MC already used */
                if ( EDDS_GetMcMACEntry(
                         pMCInfo,
                         &pMCParam->MACAddr.MacAdr[0],
                         &Idx))
                {
                    /* already active. only increment used counter */
                    pMCInfo->McMac[Idx].Cnt++;
                    pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;
                }
                else
                {
                    /* free idx found and mac addr valid? */
                    if ( Idx < EDDS_MAX_MC_MAC_CNT )
                    {
                        if(EDDS_ValidateMcMacForAdd(pDDB,pMCParam->MACAddr))
                        {
    
                            for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
                            {
                                pMCInfo->McMac[Idx].MAC.MacAdr[i] = pMCParam->MACAddr.MacAdr[i];
                            }
    
                            /* increment number of enabled mc mac addresses */
                            pMCInfo->cntEnabledMcMac++;
                            /* add mc mac address to sw filter */
                            Status = EDDS_McSWFilter_InsertMcMac(pDDB,pMCParam->MACAddr);
    
                            if(EDD_STS_OK != Status)
                            {
                                /* insert mac address to filter should not fail */
                                EDDS_CORE_TRACE_07(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                            "EDDS_RequestMC: insert mc mac to filter failed(pDDB: 0x%X - MAC: %02X-%02X-%02X-%02X-%02X-%02X)",
                                            pDDB,pMCParam->MACAddr.MacAdr[0],pMCParam->MACAddr.MacAdr[1],pMCParam->MACAddr.MacAdr[2],
                                            pMCParam->MACAddr.MacAdr[3],pMCParam->MACAddr.MacAdr[4],pMCParam->MACAddr.MacAdr[5]);
                                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                            }
    
                            /* NOTE: this must be done at last, see EDDS_McSWFilter_IsEnabledMcMac */
                            pMCInfo->McMac[Idx].Cnt = 1;
    
                            /* set rqb out value */
                            pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;
    
                            /* trigger scheduler to add MC to hardware */
                            EDDS_LOWER_TRACE_07(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                                "==> asynchronous EDDS_LL_MC_ENABLE(pDDB: 0x%X, MAC: %02X-%02X-%02X-%02X-%02X-%02X)",
                                                pDDB,
                                                pMCParam->MACAddr.MacAdr[0],
                                                pMCParam->MACAddr.MacAdr[1],
                                                pMCParam->MACAddr.MacAdr[2],
                                                pMCParam->MACAddr.MacAdr[3],
                                                pMCParam->MACAddr.MacAdr[4],
                                                pMCParam->MACAddr.MacAdr[5]);
    
                            Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
                        }
                        else
                        {
                            EDDS_LOWER_TRACE_06(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDS_RequestMC(): This is no multicast address: MAC(%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X)",
                                            pMCParam->MACAddr.MacAdr[0],
                                            pMCParam->MACAddr.MacAdr[1],
                                            pMCParam->MACAddr.MacAdr[2],
                                            pMCParam->MACAddr.MacAdr[3],
                                            pMCParam->MACAddr.MacAdr[4],
                                            pMCParam->MACAddr.MacAdr[5]);
                            Status = EDD_STS_ERR_PARAM;
                        }
                    }
                    else
                    {
                        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestMC(): No more free multicast resources!");
                        Status = EDD_STS_ERR_RESOURCE;
                    }

                    /* we have min. one mc mac enabled */
                    /* start sw filtering */
                    if( (EDD_STS_OK == Status )||((EDD_STS_OK_PENDING == Status )))
                    {
                        EDDS_McSWFilter_Statemachine(pDDB,EDDS_MC_MAC_FILTER_TRIGGER_START);
                    }
                }
            }
            break;

                /* ------------------------------------------------------------*/
                /* disable one MC-MACs                                         */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_DISABLE:
            {
                /* check if MC is enabled (if not we do nothing) */
                if ( EDDS_GetMcMACEntry(
                         pMCInfo,
                         &pMCParam->MACAddr.MacAdr[0],
                         &Idx))
                {                    
                    /* Found one. decrement usage counter */
                    pMCInfo->McMac[Idx].Cnt--;

                    /* CARP mac? decrement this counter as well */
                    if(EDDS_IS_CARP_MAC(pMCParam->MACAddr.MacAdr))
                    {
                        --pDDB->pNRT->CARPMacCnt;
                    }

                    /* last one ? */
                    if ( 0 == pMCInfo->McMac[Idx].Cnt)
                    {
                        for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
                        {
                            pMCInfo->McMac[Idx].MAC.MacAdr[i] = 0;
                        }

                        /* decrement number of enabled mc mac addresses */
                        pMCInfo->cntEnabledMcMac--;
                        /* remove mc mac address to sw filter */
                        Status = EDDS_McSWFilter_DeleteMcMac(pDDB,pMCParam->MACAddr);

                        if(EDD_STS_OK != Status)
                        {
                            /* delete mac address to filter should not fail */
                            EDDS_CORE_TRACE_07(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                        "EDDS_RequestMC: delete mc mac to filter failed(pDDB: 0x%X - MAC: %02X-%02X-%02X-%02X-%02X-%02X)",
                                        pDDB,pMCParam->MACAddr.MacAdr[0],pMCParam->MACAddr.MacAdr[1],pMCParam->MACAddr.MacAdr[2],
                                        pMCParam->MACAddr.MacAdr[3],pMCParam->MACAddr.MacAdr[4],pMCParam->MACAddr.MacAdr[5]);
                            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,EDDS_MODULE_ID,__LINE__);
                        }

                        EDDS_LOWER_TRACE_07(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                            "==> asynchronous EDDS_LL_MC_DISABLE(pDDB: 0x%X, MAC: %02X-%02X-%02X-%02X-%02X-%02X)",
                                            pDDB,
                                            pMCParam->MACAddr.MacAdr[0],
                                            pMCParam->MACAddr.MacAdr[1],
                                            pMCParam->MACAddr.MacAdr[2],
                                            pMCParam->MACAddr.MacAdr[3],
                                            pMCParam->MACAddr.MacAdr[4],
                                            pMCParam->MACAddr.MacAdr[5]);

                        Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
                    }

                    pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;
                }
            }
            break;

                /* ------------------------------------------------------------*/
                /* disable all MC-MACs                                         */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_DISABLE_ALL:
            {
                EDDS_MEMSET_LOCAL(pMCInfo->McMac,0,(EDDS_MAX_MC_MAC_CNT * sizeof(EDDS_MC_MAC_TYPE)));
                pMCInfo->cntEnabledMcMac = 0;
                pMCParam->RefCnt = 0;

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> asynchronous EDDS_LL_MC_DISABLE(pDDB: 0x%X, MAC: ALL)",
                                    pDDB);

                Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
            }
            break;

            default:
            {
                EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestMC: Invalid mode (0x%X)!",pMCParam->Mode);
                Status = EDD_STS_ERR_PARAM;
            }
            break;
        }
    }
    else
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestMC: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }


    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestMC(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McFwdCtrl_DataCheck                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                   EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE pMCParam        +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pMCParam   : Pointer to MCParamBlock                                   +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check the input data for a MC_FWD_CTRL record             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_RESULT EDDS_McFwdCtrl_DataCheck(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB, 
        EDD_RQB_MULTICAST_FWD_CTRL_TYPE const * pMCParam)
{
    LSA_UINT16 i;
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT8 bitmaskAllowed = 0;
    LSA_UINT8 checkPrio = 0;
    
    /* check MAC Addr Group */
    switch(pMCParam->MACAddrGroup)
    {
        case EDD_MAC_ADDR_GROUP_MRP_1:
        case EDD_MAC_ADDR_GROUP_MRP_2:
        case EDD_MAC_ADDR_GROUP_HSYNC:
        case EDD_MAC_ADDR_GROUP_MRP_IN_1:
        case EDD_MAC_ADDR_GROUP_MRP_IN_2:
            break;
        case EDD_MAC_ADDR_GROUP_HSR_1:
        case EDD_MAC_ADDR_GROUP_HSR_2:
        case EDD_MAC_ADDR_GROUP_STBY_1:
        case EDD_MAC_ADDR_GROUP_STBY_2:
            checkPrio = 1;
            break;
        case EDD_MAC_ADDR_GROUP_RT_2_QUER:
        case EDD_MAC_ADDR_GROUP_DCP:
            bitmaskAllowed = 1;            
            break;
        default: 
            Status = EDD_STS_ERR_PARAM;       
            EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: MACAddrGroup %u not supported!",
                            pMCParam->MACAddrGroup);
            break;        
    }
    
    if(checkPrio)
    {
        switch(pMCParam->MACAddrPrio)
        {
            case EDD_MULTICAST_FWD_PRIO_UNCHANGED:
            case EDD_MULTICAST_FWD_PRIO_NORMAL:
            case EDD_MULTICAST_FWD_PRIO_HIGH:
            case EDD_MULTICAST_FWD_PRIO_ORG:
                break;
            default:
                Status = EDD_STS_ERR_PARAM;
                EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: MACAddrPrio %u not supported!",
                                pMCParam->MACAddrPrio);
        }
    }
    
    //not break on error since we want every single error traced!
    for(i=0;i<pMCParam->PortIDModeCnt;++i)
    {
        if(bitmaskAllowed)
        {
            if(pMCParam->pPortIDModeArray[i].Mode != EDD_MULTICAST_FWD_BITMASK)
            {
                Status = EDD_STS_ERR_PARAM;
                EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: pPortIDModeArray[%u].Mode %u not supported!",
                                i,
                                pMCParam->pPortIDModeArray[i].Mode);
            }
            else
            {
                /* SrcPortID != Auto not supported for FWD_BITMASK mode */
                if(pMCParam->pPortIDModeArray[i].SrcPortID != EDD_PORT_ID_AUTO)
                {
                    Status = EDD_STS_ERR_PARAM;
                    EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: pPortIDModeArray[%u].SrcPortID %u not supported!",
                                    i,
                                    pMCParam->pPortIDModeArray[i].SrcPortID);
                }
            }
        }
        else
        {
            if( (pMCParam->pPortIDModeArray[i].Mode != EDD_MULTICAST_FWD_ENABLE)
                    && (pMCParam->pPortIDModeArray[i].Mode != EDD_MULTICAST_FWD_DISABLE) )
            {
                Status = EDD_STS_ERR_PARAM;
                EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: pPortIDModeArray[%u].Mode %u not supported!",
                                i,
                                pMCParam->pPortIDModeArray[i].Mode);
            }
            else
            {
                /* SrcPortID check - if != Auto is supported depends on the Lower Layer */
                if( EDD_PORT_ID_AUTO != pMCParam->pPortIDModeArray[i].SrcPortID )
                {
                    if(pMCParam->pPortIDModeArray[i].SrcPortID > pDDB->pGlob->HWParams.Caps.PortCnt
                            || 0 == pMCParam->pPortIDModeArray[i].SrcPortID) /* in case EDD_PORT_ID_AUTO != 0 */
                    {
                        Status = EDD_STS_ERR_PARAM;
                        EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: pPortIDModeArray[%u].SrcPortID %u not supported!",
                                        i,
                                        pMCParam->pPortIDModeArray[i].SrcPortID);
                    }
                }
            }
        }
        
        if(pMCParam->pPortIDModeArray[i].DstPortID > pDDB->pGlob->HWParams.Caps.PortCnt
                || pMCParam->pPortIDModeArray[i].DstPortID == EDD_PORT_ID_AUTO)
        {
            Status = EDD_STS_ERR_PARAM;
            EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_McFwdCtrl_DataCheck: pPortIDModeArray[%u].DstPortID %u not supported!",
                            i,
                            pMCParam->pPortIDModeArray[i].DstPortID);
        }
            
        
    }
    
    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestMCFwdCtrl                       +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_MULTICAST_FWD_CTRL      +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_MULTICAST_FWD_CTRL_TYPE                       +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_MULTICAST_FWD_CTRL req. +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestMCFwdCtrl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE   pMCParam;
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB;

    pDDB= pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestMCFwdCtrl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pMCParam = (EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE) pRQB->pParam;

    if ( LSA_HOST_PTR_ARE_EQUAL( pMCParam, LSA_NULL) )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestMCFwdCtrl: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* For function pointer controlSwitchMulticastFwd only a PortCnt more than 1 makes sense*/
    	if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd) )
    	{
    	    /* verify RQB data */
    	    Status = EDDS_McFwdCtrl_DataCheck(pDDB, pMCParam);

			if(Status == EDD_STS_OK)
			{
	            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X)",pDDB);
			    Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
			}
    	}
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestMCFwdCtrl(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSetLineDelay                    +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SET_LINE_DELAY          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_SET_LINE_DELAY_TYPE                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SET_LINE_DELAY request  +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetLineDelay(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_SET_LINE_DELAY_PTR_TYPE       pLineDelay;
    LSA_UINT32                              Idx;
    LSA_BOOL                                Changed;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSetLineDelay(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pLineDelay = (EDD_UPPER_SET_LINE_DELAY_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pLineDelay, LSA_NULL) ) ||
         ( (pLineDelay->PortID == 0) || (pLineDelay->PortID > pDDB->pGlob->HWParams.Caps.PortCnt))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetLineDelay: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Changed = LSA_FALSE;
        Idx     = pLineDelay->PortID;

        if (( pDDB->pGlob->LinkStatus[Idx].LineDelay    != pLineDelay->LineDelayInTicksMeasured ) ||
            ( pDDB->pGlob->LinkStatus[Idx].CableDelayNs != pLineDelay->CableDelayInNsMeasured   ))
        {
            Changed = LSA_TRUE;
        }

        pDDB->pGlob->LinkStatus[Idx].LineDelay    = pLineDelay->LineDelayInTicksMeasured;
        pDDB->pGlob->LinkStatus[Idx].CableDelayNs = pLineDelay->CableDelayInNsMeasured;

        if (Changed) EDDS_IndicateExtLinkChange(pDDB,Idx);

        Status = EDD_STS_OK;  /* no error */
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSetLineDelay(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetPortParams                   +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_PORT_PARAMS         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_GET_PORT_PARAMS_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_PORT_PARAMS request +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetPortParams(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE      pPortParams;
    LSA_UINT32                              Idx;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;
    EDDS_LINK_STAT_PTR_TYPE                 pGlobLinkStatus;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetPortParams(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pPortParams = (EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPortParams, LSA_NULL) ) ||
         ( (pPortParams->PortID == 0) || (pPortParams->PortID > pDDB->pGlob->HWParams.Caps.PortCnt))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetPortParams: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Idx = pPortParams->PortID;
        pGlobLinkStatus = &pDDB->pGlob->LinkStatus[Idx];

        /* EDDS does not support this values yet. using 0 */

        pPortParams->LineDelayInTicksMeasured       = pGlobLinkStatus->LineDelay;
        pPortParams->CableDelayInNsMeasured         = pGlobLinkStatus->CableDelayNs;
        pPortParams->MACAddr                        = pDDB->pGlob->HWParams.hardwareParams.MACAddressPort[Idx-1];
        pPortParams->PortStatus                     = pGlobLinkStatus->PortStatus;
        pPortParams->PhyStatus                      = pGlobLinkStatus->PhyStatus;
        pPortParams->IsPulled                       = pGlobLinkStatus->IsPulled;
        pPortParams->MRPRingPort                    = pDDB->SWI.MRPRingPort[Idx-1];
        pPortParams->SupportsMRPInterconnPortConfig = pDDB->SWI.SupportsMRPInterconnPortConfig[Idx-1];
        pPortParams->MediaType                      = pGlobLinkStatus->MediaType;
        pPortParams->IsPOF                          = pGlobLinkStatus->IsPOF;
        pPortParams->IRTPortStatus                  = EDD_IRT_NOT_SUPPORTED;
        pPortParams->RTClass2_PortStatus            = EDD_RTCLASS2_NOT_SUPPORTED;
        pPortParams->LinkSpeedModeConfigured        = pDDB->pGlob->HWParams.hardwareParams.LinkSpeedMode[Idx-1];
        pPortParams->AutonegCapAdvertised           = pGlobLinkStatus->AutonegCapAdvertised;
        pPortParams->RsvIntervalOrange.Rx.BeginNs     = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Rx.EndNs       = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Tx.BeginNs     = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Tx.EndNs       = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalRed.Rx.BeginNs        = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.EndNs          = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.EndLocalNs     = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.LastLocalFrameLen  = 0; /* EDDS dont support Red -Phase (RT-Class3) */
        pPortParams->RsvIntervalRed.Tx.BeginNs        = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.EndNs          = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.EndLocalNs     = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.LastLocalFrameLen  = 0; /* EDDS dont support Red -Phase (RT-Class3) */
        pPortParams->IsWireless                       = pDDB->pGlob->HWParams.hardwareParams.IsWireless[Idx-1];
        pPortParams->IsMDIX                           = EDD_PORT_MDIX_DISABLED;
        pPortParams->ShortPreamble				      = EDD_PORT_LONG_PREAMBLE;	/* not supported */
        pPortParams->TxFragmentation				  = EDD_PORT_NO_TX_FRAGMENTATION;	/* not supported */
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetPortParams(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLinkIndProvide                  +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_LINK_STATUS_IND_PROVIDE.+*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE                  +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam:                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function puts a MAC-Link-Status-indication resource  +*/
/*+               to the EDDS. This RQB remains within EDDS till it detects +*/
/*+               a change in Link-Status or the Channel will be closed.    +*/
/*+                                                                         +*/
/*+               If a link-status changed is detected, the RQB will be     +*/
/*+               used for indication of the event and pParam will be filled+*/
/*+               with the current link-status                              +*/
/*+                                                                         +*/
/*+               If the channel is closed, the RQB will be canceled.       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLinkIndProvide(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    LSA_BOOL                                   QueueRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pRQBParam;
    LSA_UINT32                                 Idx;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLinkIndProvide(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK_PENDING;
    QueueRQB = LSA_TRUE;

    pRQBParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    /* PortID has to be EDD_PORT_ID_AUTO or 1..EDDS_MAX_PORT_CNT to be valid */

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) ) ||
         ( (pRQBParam->PortID != EDD_PORT_ID_AUTO) && ( (pRQBParam->PortID == 0) || (pRQBParam->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestLinkIndProvide: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Idx = pRQBParam->PortID;

        if (( pRQBParam->Status != pDDB->pGlob->LinkStatus[Idx].Status    ) ||
            ( pRQBParam->Mode   != pDDB->pGlob->LinkStatus[Idx].Mode      ) ||
            ( pRQBParam->Speed  != pDDB->pGlob->LinkStatus[Idx].Speed     ))
        {

            EDDS_SetupLinkParams(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);

            QueueRQB = LSA_FALSE;

        }

        if ( QueueRQB )
        {
            /* Because this sequence runs in RQB context and must not */
            /* be interrupted by scheduler we have to use EDDS_ENTER */

            EDDS_ENTER(pDDB->hSysDev);

            /* EDD API only allows 1 request per port ! */
            if(! EDDS_RQB_QUEUE_TEST_EMPTY(&pHDB->LinkIndReq[Idx]))
            {
                Status = EDD_STS_ERR_SEQUENCE;
            }
            else
            {

                /* Queue the request within handle-management.  */

                EDDS_RQB_PUT_BLOCK_TOP( pHDB->LinkIndReq[Idx].pBottom,
                                        pHDB->LinkIndReq[Idx].pTop,
                                        pRQB);
            }

            EDDS_EXIT(pDDB->hSysDev);
        }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLinkIndProvide(), Status: 0x%X",
                           Status);


    //pDDB = LSA_NULL; /* not used yet. to prevent compiler warning */

	return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLinkIndProvideExt               +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_LINK_STATUS_IND_PROVIDE.+*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE                  +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam:                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function puts a MAC-Link-Status-indication resource  +*/
/*+               to the EDDS. This RQB remains within EDDS till it detects +*/
/*+               a change in Link-Status or the Channel will be closed.    +*/
/*+                                                                         +*/
/*+               If a link-status changed is detected, the RQB will be     +*/
/*+               used for indication of the event and pParam will be filled+*/
/*+               with the current link-status                              +*/
/*+                                                                         +*/
/*+               If the channel is closed, the RQB will be canceled.       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLinkIndProvideExt(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    LSA_BOOL                                   QueueRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pRQBParam;
    LSA_UINT32                                 Idx;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLinkIndProvideExt(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK_PENDING;
    QueueRQB = LSA_TRUE;

    pRQBParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    /* PortID has to be 1..EDDS_MAX_PORT_CNT to be valid */

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) ) ||
         ( ( (pRQBParam->PortID == 0) || (pRQBParam->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestLinkIndProvideExt: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Idx = pRQBParam->PortID;

        /* note: maybe memcmp(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
         *       --> LinkStatus should be changed from EDDS_LINK_STAT_TYPE
         *           to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE ???
         */
        if (( pRQBParam->Status  != pDDB->pGlob->LinkStatus[Idx].Status    ) ||
            ( pRQBParam->Mode    != pDDB->pGlob->LinkStatus[Idx].Mode      ) ||
            ( pRQBParam->Speed   != pDDB->pGlob->LinkStatus[Idx].Speed     ) ||
            ( pRQBParam->PortRxDelay != 0                                 ) ||
            ( pRQBParam->PortTxDelay != 0                                 ) ||
            ( pRQBParam->PortStatus != pDDB->pGlob->LinkStatus[Idx].PortStatus   ) ||
            ( pRQBParam->PortState  != pDDB->pGlob->LinkStatus[Idx].PortState    ) ||
            ( pRQBParam->PhyStatus  != pDDB->pGlob->LinkStatus[Idx].PhyStatus    ) ||
            ( pRQBParam->IRTPortStatus != EDD_IRT_NOT_SUPPORTED                  ) ||
            ( pRQBParam->SyncId0_TopoOk                                          ) ||
            ( pRQBParam->RTClass2_PortStatus != EDD_RTCLASS2_NOT_SUPPORTED       ) ||
            ( pRQBParam->MAUType    != pDDB->pGlob->LinkStatus[Idx].MAUType      ) ||
            ( pRQBParam->MediaType  != pDDB->pGlob->LinkStatus[Idx].MediaType    ) ||
            ( pRQBParam->IsPOF  != pDDB->pGlob->LinkStatus[Idx].IsPOF            ) ||
            ( pRQBParam->CableDelayInNsMeasured    != pDDB->pGlob->LinkStatus[Idx].CableDelayNs ) ||
            ( pRQBParam->LineDelayInTicksMeasured  != pDDB->pGlob->LinkStatus[Idx].LineDelay    ) ||
            ( pRQBParam->AddCheckEnabled != pDDB->pGlob->LinkStatus[Idx].AddCheckEnabled        ) ||
            ( pRQBParam->ChangeReason    != pDDB->pGlob->LinkStatus[Idx].ChangeReason           ))
        {

            EDDS_SetupLinkParams(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);

            QueueRQB = LSA_FALSE;

        }


        if ( QueueRQB )
        {
            /* Because this sequence runs in RQB context and must not */
            /* be interrupted by scheduler we have to use EDDS_ENTER */

            EDDS_ENTER(pDDB->hSysDev);

            /* EDD API only allows 1 request per port ! */
            if(! EDDS_RQB_QUEUE_TEST_EMPTY(&pHDB->LinkIndReq[Idx]))
            {
                Status = EDD_STS_ERR_SEQUENCE;
            }
            else
            {

                /* Queue the request within handle-management.  */

                EDDS_RQB_PUT_BLOCK_TOP( pHDB->LinkIndReq[Idx].pBottom,
                                        pHDB->LinkIndReq[Idx].pTop,
                                        pRQB);
            }

            EDDS_EXIT(pDDB->hSysDev);
        }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLinkIndProvideExt(), Status: 0x%X",
                           Status);

    //pDDB = LSA_NULL; /* not used yet. to prevent compiler warning */

	return(Status);

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetLinkState                    +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_LINK_STATUS         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_LINK_STATUS_PTR_TYPE                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pParam:     filled with link    +*/
/*+                                                     status.             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_LINK_STATUS request.+*/
/*+               I fills the pParam structure with the actual Link status  +*/
/*+               which is stored within Managementstructure.               +*/
/*+               Note that this status is not get from the PHY here. The   +*/
/*+               real PHY-Link-Status is only determined on PHY-Interrupt! +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetLinkState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStatus;
    LSA_UINT32                          Idx;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetLinkStatus(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pLinkStatus = (EDD_UPPER_GET_LINK_STATUS_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pLinkStatus, LSA_NULL) ) ||
         ( (pLinkStatus->PortID != EDD_PORT_ID_AUTO) && ( (pLinkStatus->PortID == 0) || (pLinkStatus->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStats: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Idx = pLinkStatus->PortID;

        pLinkStatus->Mode            = pDDB->pGlob->LinkStatus[Idx].Mode;
        pLinkStatus->Speed           = pDDB->pGlob->LinkStatus[Idx].Speed;
        pLinkStatus->Status          = pDDB->pGlob->LinkStatus[Idx].Status;
        pLinkStatus->AddCheckEnabled = pDDB->pGlob->LinkStatus[Idx].AddCheckEnabled;
        pLinkStatus->ChangeReason    = pDDB->pGlob->LinkStatus[Idx].ChangeReason;
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetLinkStatus(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetStatistics                   +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_STATISTICS          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_STATISTICS_PTR_TYPE                     +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_STATISTICS_PTR_TYPE    pParam: filled with statistics +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_STATISTICS request. +*/
/*+               It fills the pParam structure with the actual statistics  +*/
/*+                                                                         +*/
/*+               The statistics will be per channel and IF/Port.           +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS 20/11/2014  pHDB is on the stack of EDDS_Request
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatistics(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                       		Status;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;
    EDD_UPPER_GET_STATISTICS_PTR_TYPE       pStats;

    pStats = (EDD_UPPER_GET_STATISTICS_PTR_TYPE) pRQB->pParam;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetStatistics(pStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pStats,
                           pDDB,
                           pHDB);

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pStats, LSA_NULL) ) ||
         ( pStats->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)
        || (   (EDD_STATISTICS_DO_NOT_RESET_VALUES != pStats->Reset)
            && (EDD_STATISTICS_RESET_VALUES != pStats->Reset))
        )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStatistics: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_GET_STATS(pDDB: 0x%X, PortID: 0x%X)",pDDB,pStats->PortID);

        Status = EDDS_HandleRQB_byScheduler (pDDB, pRQB);
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetStatistics(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ResetStatistics                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  PortID     : PortID:  0 = interface                                    +*/
/*+                        1..x PortID                                      +*/
/*+                        EDD_STATISTICS_RESET_ALL : all                   +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function reset the statistics of the IF and/or Port. +*/
/*+               This is done by setting the "resetvalue" to the actual    +*/
/*+               values (because the reset is done per if/port and per     +*/
/*+               handle!)                                                  +*/
/*+                                                                         +*/
/*+               Note: PortID must be valid! not checked                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_ResetStatistics(
    EDDS_LOCAL_HDB_PTR_TYPE     const pHDB,
    EDD_UPPER_RQB_PTR_TYPE      pRQB)
{
    LSA_RESULT                          Status;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;

    pDDB    = pHDB->pDDB;

    Status = EDDS_HandleRQB_byScheduler (pDDB, pRQB);
    
    return(Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetStatisticsAll                +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_STATISTICS_ALL      +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE                 +*/
/*+                                                                         +*/
/*+    RQB-header:                                                          +*/
/*+    LSA_OPCODE_TYPE         opcode:     unchanged                        +*/
/*+    LSA_HANDLE_TYPE         Handle:     unchanged                        +*/
/*+    LSA_USER_ID_TYPE        UserId:     unchanged                        +*/
/*+    LSA_COMP_ID_TYPE        CompId:     unchanged                        +*/
/*+    EDD_SERVICE             Service:    unchanged                        +*/
/*+    LSA_RESULT              Status:     unchanged                        +*/
/*+    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE pParam: filled with statistics +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_STATISTICS_ALL      +*/
/*+               request.                                                  +*/
/*+               It fills the pParam structure with the actual statistics  +*/
/*+                                                                         +*/
/*+               The statistics will be for all IF/Ports.                  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatisticsAll(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                              Status;
    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE   pStats;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;

    pStats = (EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE) pRQB->pParam;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetStatisticsAll(pStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pStats,
                           pDDB,
                           pHDB);

    if (   LSA_HOST_PTR_ARE_EQUAL( pStats, LSA_NULL)
        || (   (EDD_STATISTICS_DO_NOT_RESET_VALUES != pStats->Reset)
            && (EDD_STATISTICS_RESET_VALUES != pStats->Reset))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStatisticsAll: pStats is null!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_HandleRQB_byScheduler (pDDB, pRQB);
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetStatisticsAll(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestResetStatistics                 +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_RESET_STATISTICS        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_RESET_STATISTICS_PTR_TYPE                   +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_RESET_STATISTICS_PTR_TYPE   pParam: filled with statistics+*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_RESET_STATISTICS request+*/
/*+               It will reset the statistics of the IF and/or Port. This  +*/
/*+               is done by setting the "resetvalue" to the actual values  +*/
/*+               (because the reset is done per if/port and per handle!)   +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestResetStatistics(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                            	Status;
    EDD_UPPER_RESET_STATISTICS_PTR_TYPE     pResStats;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;

    pDDB    = pHDB->pDDB;

    pResStats = (EDD_UPPER_RESET_STATISTICS_PTR_TYPE) pRQB->pParam;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestResetStatistics(pResStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pResStats,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pResStats, LSA_NULL) ) ||
         (( pResStats->PortID > pDDB->pGlob->HWParams.Caps.PortCnt) && ( pResStats->PortID != EDD_STATISTICS_RESET_ALL ))
       )
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestResetStatistics: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_ResetStatistics(pHDB,pRQB);
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestResetStatistics(), Status: 0x%X",
                           Status);

    return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSetIP                           +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SET_IP                  +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_SET_IP_PTR_TYPE                             +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_SET_IP_PTR_TYPE pParam:     Local IP Address to use       +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SET_IP request          +*/
/*+               It stores the spezified local IP address for use with     +*/
/*+               xRToverUDP.                                               +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetIP(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    EDD_UPPER_SET_IP_PTR_TYPE       pIP;
    EDD_IP_ADR_TYPE                 IPAddress = {{0,0,0,0}};
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSetIP(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pIP = (EDD_UPPER_SET_IP_PTR_TYPE) pRQB->pParam;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pIP, LSA_NULL) )
    {
        IPAddress = pIP->LocalIP;

        EDDS_ENTER(pDDB->hSysDev);
        pDDB->pGlob->HWParams.IPAddress = IPAddress;
        EDDS_EXIT(pDDB->hSysDev);
    }
    else
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetIP: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSetIP(), Status: 0x%X",
                           Status);

    return(Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetParams                       +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_PARAMS              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_PARAMS_PTR_TYPE                         +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_PARAMS_PTR_TYPE pParam:     filled with parameters    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDDS_SRV_GET_PARAMSS request    +*/
/*+               It fills the pParam structure with all parameters of the  +*/
/*+               Ethernetcontroller.                                       +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetParams(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                      Status;
    EDD_UPPER_GET_PARAMS_PTR_TYPE   pParams;
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetParams(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pParams = (EDD_UPPER_GET_PARAMS_PTR_TYPE) pRQB->pParam;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pParams, LSA_NULL))
    {
        LSA_BOOL IsCutThroughSupported = (pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_CUT_THROUGH_SUPPORT) ? LSA_TRUE : LSA_FALSE;
      
        pParams->InterfaceID = pDDB->pGlob->HWParams.InterfaceID;
      
        //lint -e835
        //JB 11/11/2014 we use #define's that are initialized with zero on purpose //#JB_TODO re-structure - do not user -save!
        pParams->HardwareType = EDD_HW_TYPE_USED_STANDARD_MAC |
                                EDD_HW_TYPE_FEATURE_NO_TIMESTAMP_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_DELAY_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_12_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_3_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_1_UDP_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE |
                                EDD_HW_TYPE_FEATURE_SYNC_SLAVE_NO_SUPPORT  |
                                EDD_HW_TYPE_FEATURE_SYNC_MASTER_NO_SUPPORT |
                                EDD_HW_TYPE_FEATURE_SYNC_DELAY_NO_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT |
                                EDD_HW_TYPE_FEATURE_NO_FRAGMENTATION_SUPPORT |
                                EDD_HW_TYPE_FEATURE_TIME_SLAVE_NO_SUPPORT |
                                EDD_HW_TYPE_FEATURE_TIME_MASTER_NO_SUPPORT ;

        /* does LL support cut through? */
        if(IsCutThroughSupported)
        {
            pParams->HardwareType |= EDD_HW_TYPE_FEATURE_CT_SUPPORT; //lint !e835 am2219 2015-02-09 edd_usr.h define is zero
        }
        else
        {
            pParams->HardwareType |= EDD_HW_TYPE_FEATURE_NO_CT_SUPPORT;
        }

        pParams->TraceIdx						            = pDDB->pGlob->TraceIdx;
        pParams->TimeResolutionInNs				      = 0 ; /* not supported */
        pParams->MACAddr						            = pDDB->pGlob->HWParams.hardwareParams.MACAddress;

        pParams->LocalIP                        = pDDB->pGlob->HWParams.IPAddress;

        pParams->MaxPortCnt                     = (LSA_UINT16) pDDB->pGlob->HWParams.Caps.PortCnt;
        pParams->MaxPortCntOfAllEDD             = pDDB->pGlob->MaxPortCntOfAllEDD;
        pParams->MaxInterfaceCntOfAllEDD        = pDDB->pGlob->MaxInterfaceCntOfAllEDD;
        pParams->MaxMRP_Instances               = pDDB->SWI.MaxMRP_Instances;
        pParams->MaxMRPInterconn_Instances      = pDDB->SWI.MaxMRPInterconn_Instances;
        pParams->SupportedMRPInterconnRole      = pDDB->SWI.SupportedMRPInterconnRole;
        pParams->MRPDefaultRoleInstance0        = pDDB->SWI.MRPDefaultRoleInstance0;
        pParams->MRPSupportedRole               = pDDB->SWI.MRPSupportedRole;
        pParams->MRPSupportedMultipleRole       = pDDB->SWI.MRPSupportedMultipleRole;
        pParams->MRAEnableLegacyMode            = pDDB->SWI.MRAEnableLegacyMode;

        /* configuration for hsync */
        pParams->FeatureMask                    = 0x0;
        if(pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_MCMAC_SRCPORT_SUPPORT)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_MCMAC_SRCPORT_MSK;
        }
        if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.AdditionalForwardingRulesSupported)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_ADD_FWD_RULES_MSK;
        }
        if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_APPL_EXIST_MSK;
        }
        if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ClusterIPSupport
                && pDDB->pGlob->HWParams.Caps.HWFeatures & EDDS_LL_CAPS_HWF_CLUSTER_IP_SUPPORT)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_CLUSTERIP_MSK;
        }
        /* configuration for mrp */
        if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_MRP_INTERCONN_FWD_RULES_MSK;
        }
        if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnOriginatorSupported)
        {
            pParams->FeatureMask                |= EDD_FEATURE_SUPPORT_MRP_INTERCONN_ORIGINATOR_MSK;
        }                

        if( pDDB->pGlob->IsSrtUsed )
        {
            pParams->CycleBaseFactor			= pDDB->pSRT->Cfg.CycleBaseFactor;
            pParams->ConsumerFrameIDBaseClass3	= 0;
            pParams->ConsumerCntClass3			= 0;
            /**@note ConsumerFrameIDBaseClass1 vs. ConsumerFrameIDBaseClass2 */
            pParams->ConsumerFrameIDBaseClass2	= pDDB->pSRT->Cons.ConsumerFrameIDBase1;
            pParams->ConsumerCntClass2			= pDDB->pSRT->Cfg.ConsumerCntClass2;
            pParams->ConsumerFrameIDBaseClass1	= pDDB->pSRT->Cons.ConsumerFrameIDBase2;
            pParams->ConsumerCntClass1			= pDDB->pSRT->Cfg.ConsumerCntClass1;
            pParams->ConsumerCntClass12Combined	= EDD_CONSUMERCNT_CLASS12_SEPARATE;
            pParams->ProviderCnt				= pDDB->pSRT->Cfg.ProviderCnt;
            pParams->ProviderCntClass3			= 0;
            pParams->ProviderGroupCnt			= EDD_CFG_CSRT_MAX_PROVIDER_GROUP;
            pParams->DataLenMin					= EDD_CSRT_DATALEN_MIN;
            pParams->DataLenMax					= EDD_CSRT_DATALEN_MAX;
            pParams->DataLenMinUDP				= EDD_CSRT_UDP_DATALEN_MIN;
            pParams->DataLenMaxUDP				= EDD_CSRT_UDP_DATALEN_MAX;
        }
        else
        {
            pParams->CycleBaseFactor				 	  = 32;	/* fixed */
            pParams->ConsumerFrameIDBaseClass3		 	  = 0;
            pParams->ConsumerCntClass3				 	  = 0;
            /**@note ConsumerFrameIDBaseClass1 vs. ConsumerFrameIDBaseClass2 */
            pParams->ConsumerFrameIDBaseClass2			  = 0;
            pParams->ConsumerCntClass2					  = 0;
            pParams->ConsumerFrameIDBaseClass1			  = 0;
            pParams->ConsumerCntClass1					  = 0;
            pParams->ConsumerCntClass12Combined			  = EDD_CONSUMERCNT_CLASS12_SEPARATE;
            pParams->ProviderCnt						  = 0;
            pParams->ProviderCntClass3					  = 0;
            pParams->ProviderGroupCnt					  = 0;
            pParams->DataLenMin							  = 0;
            pParams->DataLenMax							  = 0;
            pParams->DataLenMinUDP						  = 0;
            pParams->DataLenMaxUDP						  = 0;
        }
        
        /* KRAM not present. Set to 0 */
        pParams->KRAMInfo.KRAMBaseAddr                 = 0;
        pParams->KRAMInfo.offset_ProcessImageStart     = 0;
        pParams->KRAMInfo.offset_async_in_start        = 0;
        pParams->KRAMInfo.size_async_in                = 0;
        pParams->KRAMInfo.offset_async_out_start       = 0;
        pParams->KRAMInfo.size_async_out               = 0;
        pParams->KRAMInfo.offset_ProcessImageEnd       = 0;
        
        pParams->RsvIntervalRed.RxEndNs                = 0;
        pParams->RsvIntervalRed.RxLocalTransferEndNs   = 0;
        pParams->RsvIntervalRed.TxEndNs                = 0;
        pParams->RsvIntervalRed.TxLocalTransferEndNs   = 0;
        pParams->RsvIntervalRed.TxLocalTransferStartNs = 0;
    }
    else
    {
        EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetParams: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetParams(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLEDBlink                        +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SETUP_PHY               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_SETUP_PHY_TYPE                               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDDS_UPPER_SETUP_PHY_PTR_TYPE pParam:  points to PHY parameters     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_LED_BLINK request.      +*/
/*+               We setup blink all LEDs and then switch them back to      +*/
/*+               the original mode.                                        +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLEDBlink(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDDS_LOCAL_DDB_PTR_TYPE     		pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLEDBlink(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDDS_HandleRQB_byScheduler (pDDB, pRQB);
    
    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLEDBlink(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(pRQB);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GeneralRequest                         +*/
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
/*+     EDD_SERVICE             Service:    EDDS_SRV_GET_STATS              +*/
/*+                                         EDD_SRV_GET_LINK_STATUS         +*/
/*+                                         EDD_SRV_MULTICAST               +*/
/*+                                         EDD_SRV_PRM_READ                +*/
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


//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_GeneralRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    LSA_UINT32						TraceIdx;

    TraceIdx = pHDB->pDDB->pGlob->TraceIdx;

    EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_GeneralRequest(pRQB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pHDB);

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_GET_LINK_STATUS:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_LINK_STATUS (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestGetLinkState(pRQB,pHDB);
            break;
        case EDD_SRV_LINK_STATUS_IND_PROVIDE:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LINK_STATUS_IND_PROVIDE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestLinkIndProvide(pRQB,pHDB);
            break;
        case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestLinkIndProvideExt(pRQB,pHDB);
            break;
        case EDD_SRV_MULTICAST:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_MULTICAST (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestMC(pRQB,pHDB);
            break;
        case EDD_SRV_GET_PARAMS:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_PARAMS (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestGetParams(pRQB,pHDB);
            break;
        case EDD_SRV_SET_IP:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_IP (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestSetIP(pRQB,pHDB);
            break;
        case EDD_SRV_SET_LINE_DELAY:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_LINE_DELAY (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestSetLineDelay(pRQB,pHDB);
            break;
        case EDD_SRV_GET_PORT_PARAMS:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_PORT_PARAMS (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestGetPortParams(pRQB,pHDB);
            break;
        case EDD_SRV_MULTICAST_FWD_CTRL:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_MULTICAST_FWD_CTRL (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestMCFwdCtrl(pRQB,pHDB);
            break;

        case EDD_SRV_SENDCLOCK_CHANGE:
            if( pHDB->pDDB->pGlob->IsSrtUsed )
            {
				EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SENDCLOCK_CHANGE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
				Status = EDDS_RTSendclockChange(pRQB,pHDB);
            }
			else
			{
				if((32 == ((EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam)->CycleBaseFactor) || (0 == ((EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam)->CycleBaseFactor))
				{
					Status = EDD_STS_OK;
				}else
				{
					Status = EDD_STS_ERR_PARAM;
				}
			}
			break;
        case EDD_SRV_LED_BLINK:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LED_BLINK (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestLEDBlink(pRQB,pHDB);
            break;
        case EDD_SRV_SET_SYNC_STATE:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_SYNC_STATE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            /* no action. just accept */
            Status = EDD_STS_OK;
            break;
        case EDD_SRV_SET_REMOTE_PORT_STATE:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_REMOTE_PORT_STATE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            /* no action. just accept */
            Status = EDD_STS_OK;
            break;
        case EDD_SRV_GET_STATISTICS:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_STATISTICS (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestGetStatistics(pRQB,pHDB);
            break;
        case EDD_SRV_RESET_STATISTICS:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_RESET_STATISTICS (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestResetStatistics(pRQB,pHDB);
            break;
        case EDD_SRV_GET_STATISTICS_ALL:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_STATISTICS_ALL (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestGetStatisticsAll(pRQB,pHDB);
            break;
        default:
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_GeneralRequest: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }

    if (EDD_STS_OK_PENDING != Status) EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_CORE_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_GeneralRequest(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DebugRequest                           +*/
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
/*+     EDD_SERVICE             Service:    EDDS_SRV_GET_DEBUG_STATS        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all EDDS Debug  (Component          +*/
/*+               independent) requests.                                    +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 pRQB is checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRQB or pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DebugRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                         		Status;
    EDDS_UPPER_GET_DEBUG_STATS_PTR_TYPE		pParams;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB;

    pDDB    = pHDB->pDDB;
    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DebugRequest(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDDS_SRV_GET_DEBUG_STATS:

                EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDDS_SRV_GET_DEBUG_STATS (pHDB: 0x%X)", pHDB);

                pParams = (EDDS_UPPER_GET_DEBUG_STATS_PTR_TYPE) pRQB->pParam;

                EDDS_MEMSET_UPPER(pParams,0,sizeof(EDDS_RQB_GET_DEBUG_STATS_TYPE));

                pParams->Glob.RxXRToUDPIPWrongChecksum = pDDB->pGlob->RxStats.RxXRToUDPIPWrongChecksum;
                pParams->Glob.RxXRToUDPWrongHeader     = pDDB->pGlob->RxStats.RxXRToUDPWrongHeader;
                pParams->Glob.TxResources              = pDDB->pGlob->Errors.TxResource;
                pParams->Glob.RxOverloadCnt            = pDDB->pGlob->Errors.RxOverloadCnt;

                pParams->NRT.RxARPIPSD                 = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_IP_IDX];
                pParams->NRT.RxASRTOtherSD             = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_ASRT_OTHER_IDX];
                pParams->NRT.RxASRTAlarmSD             = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_ASRT_ALARM_IDX];
                pParams->NRT.RxOtherSD                 = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_OTHER_IDX];

                pParams->NRT.RxARPIP_Drop              = pDDB->pNRT->Stats.RxARPIP_Drop;
                pParams->NRT.RxASRTAlarm_Drop          = pDDB->pNRT->Stats.RxASRTAlarm_Drop;
                pParams->NRT.RxLenDCP_Drop             = pDDB->pNRT->Stats.RxLenDCP_Drop;
                pParams->NRT.RxLenOther_Drop           = pDDB->pNRT->Stats.RxLenOther_Drop;
                pParams->NRT.RxXRTDCP_Drop             = pDDB->pNRT->Stats.RxXRTDCP_Drop;
                pParams->NRT.RxLLDP_Drop               = pDDB->pNRT->Stats.RxLLDP_Drop;
                pParams->NRT.RxPTP_Drop                = pDDB->pNRT->Stats.RxPTP_Drop;
                pParams->NRT.RxMRP_Drop                = pDDB->pNRT->Stats.RxMRP_Drop;
                pParams->NRT.RxLenTSYNC_Drop           = pDDB->pNRT->Stats.RxLenTSYNC_Drop;
                pParams->NRT.RxOther_Drop              = pDDB->pNRT->Stats.RxOther_Drop;
                pParams->NRT.RxUDP_XRTDCP_Drop         = pDDB->pNRT->Stats.RxUDP_XRTDCP_Drop;
                pParams->NRT.RxUDP_ASRTAlarm_Drop      = pDDB->pNRT->Stats.RxUDP_ASRTAlarm_Drop;

                pParams->NRT.RxDCPFilterDrop           = pDDB->pNRT->Stats.RxDCPFilterDrop;
                pParams->NRT.RxDCPFilterInvalidFrame   = pDDB->pNRT->Stats.RxDCPFilterInvalidFrame;

                if( pDDB->pGlob->IsSrtUsed )
                {
                    pParams->SRT.ConsCycleCounterDropCnt   = pDDB->pSRT->Stats.ConsCycleCounterDropCnt;
                    pParams->SRT.ConsCycleJitterDrop       = pDDB->pSRT->Stats.ConsCycleJitterDrop;
                    pParams->SRT.ConsCycleOverloadCnt      = pDDB->pSRT->Stats.ConsCycleOverloadCnt;
                    pParams->SRT.ConsRxCycleCounterOld     = pDDB->pSRT->Stats.ConsRxCycleCounterOld;
                    pParams->SRT.ConsRxWrongUDPLenOrIP     = pDDB->pSRT->Stats.ConsRxWrongUDPLenOrIP;
                    pParams->SRT.ConsRxWrongXRTLenOrMAC    = pDDB->pSRT->Stats.ConsRxWrongXRTLenOrMAC;
                    pParams->SRT.NoMoreKnotElements        = pDDB->pSRT->Stats.NoMoreKnotElements;
                    pParams->SRT.ProvCycleCounterDropCnt   = pDDB->pSRT->Stats.ProvCycleCounterDropCnt;
                    pParams->SRT.ProvCycleOverloadCnt      = pDDB->pSRT->Stats.ProvCycleOverloadCnt;
                }
                Status = EDD_STS_OK;
                break;

            default:
                EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_DebugRequest: Invalid Service (Service: 0x%X)",EDD_RQB_GET_SERVICE(pRQB));
                Status = EDD_STS_ERR_SERVICE;
                break;
        }
    }
    else
    {
        EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> Request: Service: 0x%X, RQB->pParam is NULL",EDD_RQB_GET_SERVICE(pRQB));
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DebugRequest(), Status: 0x%X",
                           Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSwitchSetPortState              +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_SET_PORT_STATE   +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SWITCH_SET_PORT_STATE rq+*/
/*+                                                                         +*/
/*+               We call the LLIF function to change the PortStates and    +*/
/*+               on success we indicate a link-change for all Ports/handles+*/
/*+               provided a ext-link indication resource.                  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSwitchSetPortState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                  Status;
    EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE       pPortStates;
    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB;

    pDDB      = pHDB->pDDB;
    EDDS_SWI_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSwitchSetPortState(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    /* For function pointer setSwitchPortState only a PortCnt more than 1 makes sense*/
    if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,setSwitchPortState))
   	{
		Status = EDD_STS_OK;
		pPortStates = (EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE) pRQB->pParam;

		if ( LSA_HOST_PTR_ARE_EQUAL( pPortStates, LSA_NULL) )
		{
			EDDS_SWI_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSwitchSetPortState: RQB->pParam is NULL!");
			Status = EDD_STS_ERR_PARAM;
		}
		else
		{

			EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_SET_PORT_STATE(pDDB: 0x%X, pPortStates: 0x%X)",pDDB,pPortStates);

			Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
		}
   	}
    else
   	{

		EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchSetPortState: Function pointer setSwitchPortState is not null or PortCnt: %d is less than 2!",
				pDDB->pGlob->HWParams.Caps.PortCnt);
		Status = EDD_STS_ERR_NOT_SUPPORTED;

		LSA_UNUSED_ARG(pRQB);
   	}

    EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSwitchSetPortState(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSwitchFlushFDB                  +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_FLUSH_FILTERING_DB*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to NULL (not used)                                       +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles EDD_SRV_SWITCH_FLUSH_FILTERING_DB   +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSwitchFlushFDB(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    EDDS_LOCAL_DDB_PTR_TYPE     			   pDDB;
    EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE  pFlushFilterDB;

    pDDB  = pHDB->pDDB;
    EDDS_SWI_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSwitchFlushFDB(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    /* For function pointer flushSwitchFilteringDB only a PortCnt more than 1 makes sense*/
    if( (pDDB->pGlob->HWParams.Caps.PortCnt > 1) && EDDS_LL_AVAILABLE(pDDB,flushSwitchFilteringDB))
   	{
		LSA_UINT16 PortIdx;
		LSA_UINT16 Port;
		pFlushFilterDB = (EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE) pRQB->pParam;

		/* check PortCnt */
		if ((pFlushFilterDB->PortCnt < 1) || (pFlushFilterDB->PortCnt > EDD_CFG_MAX_PORT_CNT))
		{
			EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortCnt (%u)",pFlushFilterDB->PortCnt);
			Status = EDD_STS_ERR_PARAM;
		}

		if(EDD_STS_OK == Status)
		{
            /* check port status for every port in PortID array */
            /* range of PortCnt: 1..EDD_CFG_MAX_PORT_CNT        */
            for (PortIdx = 0 ; PortIdx < pFlushFilterDB->PortCnt ; PortIdx++)
            {
                /* PortID[] value range 1..EDD_CFG_MAX_PORT_CNT */
                Port = pFlushFilterDB->PortID[PortIdx];

                if ((Port < 1) ||
                    (Port > pDDB->pGlob->HWParams.Caps.PortCnt))
                {
                    EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortID (%u)",pFlushFilterDB->PortID[PortIdx]);
                    Status = EDD_STS_ERR_PARAM;
                    break;
                }

                if (EDD_PORT_STATE_BLOCKING != pDDB->pGlob->LinkStatus[Port].PortState &&
                    EDD_PORT_STATE_DISABLE  != pDDB->pGlob->LinkStatus[Port].PortState)
                {
                    EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortState for port (%u)",Port);
                    Status = EDD_STS_ERR_PARAM;
                    break;
                }
            }
		}

		if(EDD_STS_OK == Status)
		{
            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> asynchronous EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB: 0x%X)",pDDB);

            Status = EDDS_HandleHighRQB_byScheduler (pDDB, pRQB);
		}
	}
    else
	{
		EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB: Function pointer controlSwitchMulticastFwd is not null or PortCnt: %d is less than 2!",
				pDDB->pGlob->HWParams.Caps.PortCnt);
		Status = EDD_STS_ERR_NOT_SUPPORTED;
	}

    EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSwitchFlushFDB(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SwitchRequest                          +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_SET_PORT_STATE   +*/
/*+                                         EDD_SRV_SWITCH_FLUSH_FILTERING_DB*/
/*+                                         EDDS_SRV_SWITCH_SET_PORT_REDUNDANT*/
/*+                                         EDDS_SRV_SWITCH_SET_BRIDGE_MODE +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles EDDS switch services.               +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SwitchRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;

    pDDB     = pHDB->pDDB;
    EDDS_SWI_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_SwitchRequest(pRQB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pHDB);

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_SWITCH_SET_PORT_STATE:
            EDDS_SWI_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SWITCH_SET_PORT_STATE (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestSwitchSetPortState(pRQB,pHDB);
            break;
        case EDD_SRV_SWITCH_FLUSH_FILTERING_DB:
            EDDS_SWI_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SWITCH_FLUSH_FILTERING_DB (pRQB: 0x%X, pHDB: 0x%X)",pRQB,pHDB);
            Status = EDDS_RequestSwitchFlushFDB(pRQB,pHDB);
            break;
        default:
            EDDS_SWI_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_RequestFinish: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }


    if(EDD_STS_OK_PENDING != Status)
    {
        EDDS_RequestFinish(pHDB,pRQB,Status);
    }

    EDDS_SWI_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_SwitchRequest(), Status: 0x%X",
                           Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagCycleRequest                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Diag Cycle Request Timer function.                        +*/
/*+               This function is called after a Diag cycle timer-request  +*/
/*+               occured (triggered by EDDS_DO_DIAG_TIMER_REQUEST)         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagCycleRequest(pDDB: 0x%X)",
                           pDDB);

    if ( pDDB->pGlob->HWIsSetup ) /* if hardware not setup we do nothing */
    {
        EDDS_DiagFrameDropHandler(pDDB,pDDB->pGlob->DropStats.InDiscards + pDDB->pGlob->DropStats.OutDiscards);
    }

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagCycleRequest()");
}


/*****************************************************************************/
/*  end of file EDDS_GEN.C                                                   */
/*****************************************************************************/

