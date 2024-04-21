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
/*  F i l e               &F: edds_mc_filter.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SW filtering of multicast ethernet frames        */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  13
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_MC_FILTER */


/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*****************************************************************************/
/*                                                                           */
/* Extern variables                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Extern functions                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Globlal variables                                                         */
/*                                                                           */
/*****************************************************************************/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_Init                        +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+  Results    :         ----                                              +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      initializes the sw filter of multicast mac addresses +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Init(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB)
{
    EDDS_MC_MAC_INFO_PTR_TYPE pMCInfo;

    pMCInfo = pDDB->pGlob->pMCInfo;

    EDDS_MEMSET_LOCAL(pMCInfo->McMac,0,(EDDS_MAX_MC_MAC_CNT * sizeof(EDDS_MC_MAC_TYPE)));
    pMCInfo->cntEnabledMcMac = 0;
    pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_NONE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_Statemachine                +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             EDDS_MC_MAC_FILTER_TRIGGER_TYPE Trigger     +*/
/*+  Result                :    ---                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  Trigger    : current trigger of statemachine                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:    statemachine for sw filtering of mc mac frames         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Statemachine(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB,
    EDDS_MC_MAC_FILTER_TRIGGER_TYPE const        Trigger)
{
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CORE_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_McSWFilter_Statemachine(pDDB: 0x%X)",
                           pDDB);

    switch (pDDB->pGlob->pMCInfo->McMacFilterState)
    {
        case EDDS_MC_MAC_FILTER_STATE_NONE:
        {
            switch(Trigger)
            {
                case EDDS_MC_MAC_FILTER_TRIGGER_DISABLE:
                {
                    /* nothing to do */
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_ENABLE:
                {
                    pDDB->pGlob->pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_PAUSED;
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_START:
                {
                    /* nothing to do */
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_STOP:
                {
                    /* nothing to do */
                    break;
                }
                default:
                {
                    /* no valid trigger */
                    EDDS_CORE_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                "EDDS_McSWFilter_Statemachine: unknown trigger(pDDB: 0x%X - state: 0x%X - trigger: 0x%X)",
                                pDDB,pDDB->pGlob->pMCInfo->McMacFilterState,Trigger);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE, EDDS_MODULE_ID, __LINE__);
                    break;
                }
            }
            break;
        }
        case EDDS_MC_MAC_FILTER_STATE_PAUSED:
        {
            switch(Trigger)
            {
                case EDDS_MC_MAC_FILTER_TRIGGER_DISABLE:
                {
                    pDDB->pGlob->pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_NONE;
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_ENABLE:
                {
                    /* nothing to do */
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_START:
                {
                    pDDB->pGlob->pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_ACTIVE;
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_STOP:
                {
                    /* nothing to do */
                    break;
                }
                default:
                {
                    /* no valid trigger */
                    EDDS_CORE_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                "EDDS_McSWFilter_Statemachine: unknown trigger(pDDB: 0x%X - state: 0x%X - trigger: 0x%X)",
                                pDDB,pDDB->pGlob->pMCInfo->McMacFilterState,Trigger);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE, EDDS_MODULE_ID, __LINE__);
                    break;
                }
            }
            break;
        }
        case EDDS_MC_MAC_FILTER_STATE_ACTIVE:
        {
            switch(Trigger)
            {
                case EDDS_MC_MAC_FILTER_TRIGGER_DISABLE:
                {
                    pDDB->pGlob->pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_NONE;
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_ENABLE:
                {
                    /* nothing to do */
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_START:
                {
                    /* nothing to do */
                    break;
                }
                case EDDS_MC_MAC_FILTER_TRIGGER_STOP:
                {
                    pDDB->pGlob->pMCInfo->McMacFilterState = EDDS_MC_MAC_FILTER_STATE_PAUSED;
                    break;
                }
                default:
                {
                    /* no valid trigger */
                    EDDS_CORE_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                "EDDS_McSWFilter_Statemachine: unknown trigger(pDDB: 0x%X - state: 0x%X - trigger: 0x%X)",
                                pDDB,pDDB->pGlob->pMCInfo->McMacFilterState,Trigger);
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE, EDDS_MODULE_ID, __LINE__);
                    break;
                }
            }
            break;
        }
        default:
        {
            /* no valid state */
            EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_FATAL,
                        "EDDS_McSWFilter_Statemachine: unknown state(pDDB: 0x%X - state: 0x%X)",
                        pDDB,pDDB->pGlob->pMCInfo->McMacFilterState);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE, EDDS_MODULE_ID, __LINE__);
            break;
        }
    }

    EDDS_CORE_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_McSWFilter_Statemachine()");

    LSA_UNUSED_ARG(TraceIdx);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_IsEnabled                   +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE                                                  +*/
/*+               LSA_FALSE                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      evaluates if the sw filtering of mc mac is enabled   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsEnabled(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB)
{
    LSA_BOOL isEnabled;

    isEnabled = LSA_FALSE;

    if(EDDS_MC_MAC_FILTER_STATE_NONE != pDDB->pGlob->pMCInfo->McMacFilterState)
    {
        isEnabled = LSA_TRUE;
    }
    return isEnabled;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_IsActive                    +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE                                                  +*/
/*+               LSA_FALSE                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      evaluates if the sw filtering of mc mac is active    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsActive(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB)
{
    LSA_BOOL isActive;

    isActive = LSA_FALSE;

    if(EDDS_MC_MAC_FILTER_STATE_ACTIVE == pDDB->pGlob->pMCInfo->McMacFilterState)
    {
        isActive = LSA_TRUE;
    }
    return isActive;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_InsertMcMac                 +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             EDD_MAC_ADR_TYPE mcMAC                      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  mcMAC      : multicast mac address                                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      inserts the assigned multicast mac address into the  +*/
/*+                            sw filter                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_InsertMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
    EDD_MAC_ADR_TYPE mcMAC)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK;

    /* todo: add a better filtering algo (current: linear search) */
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(mcMAC);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_DeleteMcMac                 +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             EDD_MAC_ADR_TYPE mcMAC                      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  mcMAC      : multicast mac address                                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      removes the assigned multicast mac address from the  +*/
/*+                            sw filter                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_DeleteMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB,
    EDD_MAC_ADR_TYPE mcMAC)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK;

    /* todo: add a better filtering algo (current: linear search) */
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(mcMAC);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_Reset                       +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+  Results    :         ----                                              +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      resets the sw filter of multicast mac addresses      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Reset(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB)
{
    EDDS_MC_MAC_INFO_PTR_TYPE pMCInfo;

    pMCInfo = pDDB->pGlob->pMCInfo;

    /* todo: add a better filtering algo (current: linear search) */
    EDDS_MEMSET_LOCAL(pMCInfo->McMac,0,(EDDS_MAX_MC_MAC_CNT * sizeof(EDDS_MC_MAC_TYPE)));
    pMCInfo->cntEnabledMcMac = 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_McSWFilter_IsEnabledMcMac              +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             LSA_UINT8* pMACAddr                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  pMACAddr    : MAC address as byte array                                +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE                                                  +*/
/*+               LSA_FALSE                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:      evaluates if the assigned mac address                +*/
/*+                        is an enabled multicast mac address              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsEnabledMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const   pDDB,
    LSA_UINT8 const *               pMACAddr)
{
    EDDS_MC_MAC_INFO_PTR_TYPE pMCInfo;
    LSA_UINT32 index;
    LSA_BOOL mcMacIsEnabled;

    pMCInfo = pDDB->pGlob->pMCInfo;
    mcMacIsEnabled = LSA_FALSE;

    /* todo: add a better filtering algo (current: linear search)*/

    for(index = 0 ; index < EDDS_MAX_MC_MAC_CNT; index++)
    {
        if(pMCInfo->McMac[index].Cnt)
        {
            LSA_UINT8* pCompareMAC = pMCInfo->McMac[index].MAC.MacAdr;
            if( (pCompareMAC[5] == pMACAddr[5]) &&
                        (pCompareMAC[4] == pMACAddr[4]) &&
                        (pCompareMAC[3] == pMACAddr[3]) &&
                        (pCompareMAC[2] == pMACAddr[2]) &&
                        (pCompareMAC[1] == pMACAddr[1]) &&
                        (pCompareMAC[0] == pMACAddr[0]))
            {
                mcMacIsEnabled = LSA_TRUE;
            }
        }
    }


    return mcMacIsEnabled;
}
