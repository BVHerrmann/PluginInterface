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
/*  F i l e               &F: edds_dev.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Device and handle-management functions      */
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

#define LTRC_ACT_MODUL_ID  2
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_DEV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"            /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*=====  handle-functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_IsAnyHandleInUse                       +*/
/*+  Input/Output               EDDS_DDB_TYPE * const    pDDB               +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+  Result:      LSA_TRUE   : At least one handle is still in use.         +*/
/*+               LSA_FALSE  : No handle in use.                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check whether at least one handle is currenty in use.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_IsAnyHandleInUse(EDDS_DDB_TYPE * const pDDB)
{
    LSA_UINT32 Index = 0;
    LSA_BOOL Found = LSA_FALSE;
    EDDS_DDB_HDB_MGMT_PTR_TYPE pHDBMgmt = &pDDB->HDBMgmt;
    
    while ( (Index < pHDBMgmt->MaxHandleCnt) && ( ! Found ))
    {
        if ( pHDBMgmt->HDBTable[Index].InUse )
        {
            Found = LSA_TRUE;
        }

        Index++;
    }    

    return(Found);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleAcquire                          +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE    pDDB             +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE  * ppHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         Pointer to DDB.                                           +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK           : Free HDB available.                +*/
/*+               EDD_STS_ERR_RESOURCE : No more free HDB available.        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reserve a HDB.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 10/11/2014 ppHDB will only be set
//AD_DISCUSS 20/11/2014 ppHDB could be pointing to null
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleAcquire(EDDS_LOCAL_DDB_PTR_TYPE const pDDB, EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR *ppHDB)
{
    LSA_UINT32                 Index = 0;
    LSA_RESULT                 Status = EDD_STS_OK;
    EDDS_DDB_HDB_MGMT_PTR_TYPE pHDBMgmt = &pDDB->HDBMgmt;
    
    #if (EDDS_CFG_TRACE_MODE == 2)
    LSA_UINT32 TraceIdx = pDDB->pGlob->TraceIdx;
    #endif
    
    EDDS_UPPER_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN :EDDS_HandleAcquire()");
    
    if (pHDBMgmt->UsedHandleCnt >= pHDBMgmt->MaxHandleCnt)
    {
        Status = EDD_STS_ERR_RESOURCE;
    }
    else
    {
        while ( (Index < pHDBMgmt->MaxHandleCnt) && (pHDBMgmt->HDBTable[Index].InUse) )
        {
            Index++;
        }

        if (pHDBMgmt->HDBTable[Index].InUse)
        {
            Status = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            EDDS_LOCAL_HDB_PTR_TYPE pHDB = &pHDBMgmt->HDBTable[Index];
            
            pHDBMgmt->UsedHandleCnt++;

            EDDS_MEMSET_LOCAL(&pHDBMgmt->HDBTable[Index], 0, sizeof(EDDS_HDB_TYPE));
            
            pHDB->InUse    = LSA_TRUE;
            pHDB->HDBIndex = Index;
            *ppHDB = pHDB;
        }
    } 
    
    EDDS_UPPER_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "OUT:EDDS_HandleAcquire(*ppHDB: 0x%X, Index: %d), Status: 0x%X",
                        *ppHDB, Index, Status);

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleGetHDB                           +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE                    Handle   +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE            * ppHDB  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets pointer to handle-managment entry (HDB) for handle.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 10/11/2014 ppHDB will only be set
//AD_DISCUSS 20/11/2014 ppHDB could be pointing to null
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleGetHDB( EDD_HANDLE_LOWER_TYPE const LowerHandle,
                                                  EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppHDB)
{
    EDDS_LOCAL_HDB_PTR_TYPE const pHDB = (EDDS_LOCAL_HDB_PTR_TYPE)LowerHandle;
    LSA_RESULT Status = EDD_STS_ERR_PARAM; 
    
    if ( pHDB && (pHDB->InUse) )
    {
        if( EDDS_CFG_MAX_CHANNELS > pHDB->HDBIndex )
        {
            *ppHDB = pHDB; 
            Status = EDD_STS_OK;
        }
    }
        
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleRelease                          +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE                    Handle   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees channel handle.                                     +*/
/*+                                                                         +*/
/*+               If the handle is not in use, EDD_STS_ERR_SEQUENCE is      +*/
/*+               returned.                                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleRelease(EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT32 Index = 0;
    LSA_BOOL Found = LSA_FALSE;
    EDDS_DDB_HDB_MGMT_PTR_TYPE pHDBMgmt = &pHDB->pDDB->HDBMgmt;

    #if (EDDS_CFG_TRACE_MODE == 2)
    LSA_UINT32 TraceIdx = pHDB->pDDB->pGlob->TraceIdx;
    #endif
    
    EDDS_UPPER_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT, 
                        "IN :EDDS_HandleRelease(pHDB: 0x%X)",
                        pHDB);
        
    while ( (Index < pHDBMgmt->MaxHandleCnt) && ( ! Found ))
    {
        if ( pHDBMgmt->HDBTable[Index].InUse )
        {
            if ( LSA_HOST_PTR_ARE_EQUAL(&pHDBMgmt->HDBTable[Index], pHDB) )
            {
                pHDBMgmt->UsedHandleCnt--;
                pHDB->InUse = LSA_FALSE;
                
                Found = LSA_TRUE;
            }
        }

        Index++;
    } 
    
    if(!Found)
    {
        Status = EDD_STS_ERR_SEQUENCE;
    }

    EDDS_UPPER_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "OUT:EDDS_HandleRelease(), Status: 0x%X",
                        Status);

    return Status;
}

/*===========================================================================*/


/*===========================================================================*/
/*=====  DDB functions                                                   ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CreateDDB                              +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE * ppDDB             +*/
/*+                             LSA_UINT32                TxBufferCnt (>0)  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+  TxBufferCnt  : Number of Tx-Buffers to manage                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an DDB structure and initializes it. The new DDB+*/
/*+               is chained into the global DDB-info structure and a       +*/
/*+               pointer to the new DDB is returned.                       +*/
/*+                                                                         +*/
/*+               EDDS_DevInfoIni() has to be called before using this funct.+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_RESOURCE is returned, if alloc fails or no    +*/
/*+               more devices supported.                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



//ppDDB will be set
//AD_DISCUSS 20/11/2014 ppDDB could be pointing to null
LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CreateDDB(
    EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB,
    EDDS_UPPER_DPB_PTR_TO_CONST_TYPE   const            pDPB)
{

    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    LSA_UINT32                      Size;
    LSA_UINT16                      FreeStat;
    LSA_RESULT                      Result;

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CreateDDB(ppDDB: 0x%X)",
                           ppDDB);


    Result = EDD_STS_OK;
    pDDB   = LSA_NULL;
    pGlob  = LSA_NULL;

    /*-----------------------------------------------------------------------*/
    /* alloc for  DDB-type                                                   */
    /*-----------------------------------------------------------------------*/

        EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pDDB), sizeof(EDDS_DDB_TYPE));

        if ( LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pDDB,0,sizeof(EDDS_DDB_TYPE));

    /*-----------------------------------------------------------------------*/
    /* alloc for global DDB-type parts                                       */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {

        Size =  sizeof(EDDS_DEVICE_GLOB_TYPE);

        #ifdef EDDS_CFG_GLOB_FAST_MEM
        EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob), Size);
        #else
        EDDS_ALLOC_LOCAL_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob, Size);
        #endif

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pGlob,0,Size);
    }
    EDDS_IS_VALID_PTR(pGlob);

    /*-----------------------------------------------------------------------*/
    /* alloc for  pTxInfo                                                    */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Size = sizeof(EDDS_TX_INFO_TYPE)* (pDPB->TxBufferCnt);

        EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob->pTxInfo), Size);

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->pTxInfo, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            EDDS_MEMSET_LOCAL(pGlob->pTxInfo,0,Size);
            pGlob->TxBuffCnt           = pDPB->TxBufferCnt;
        }
    }

    /*-----------------------------------------------------------------------*/
    /* alloc ressources for CopyInterface                                    */
    /*-----------------------------------------------------------------------*/

    if ( (EDD_STS_OK == Result) && (EDDS_DPB_DO_NOT_USE_ZERO_COPY_IF == pDPB->ZeroCopyInterface) )
    {
        Size =  EDD_FRAME_BUFFER_LENGTH * (pDPB->TxBufferCnt);

        // we must use hSysDev from pDPB here (pDDB->hSysDev is not initialized yet)!
        EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(pDPB->hSysDev,((EDD_UPPER_MEM_PTR_TYPE *)&pGlob->pTxTransferMem), Size);
    }
    pGlob->ZeroCopyInterface   = (EDDS_DPB_USE_ZERO_COPY_IF == pDPB->ZeroCopyInterface)?LSA_TRUE:LSA_FALSE;

    /*-----------------------------------------------------------------------*/
    /* alloc for  MC-MAC Management                                          */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Size =  sizeof(EDDS_MC_MAC_INFO_TYPE);

        EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob->pMCInfo), Size);

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->pMCInfo, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pGlob->pMCInfo,0,Size);
    }

    /*-----------------------------------------------------------------------*/
    /* Allocate PRM Structures                                               */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Result = EDDS_PrmAlloc(&pGlob->Prm);
    }

    /*-----------------------------------------------------------------------*/
    /* on scuccess we initialize the internal RQBs with fixed values.        */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        /*-------------------------------------------------------------------*/
        /* Init structure (all parts are already memset to 0, so no init to 0*/
        /* is required!)                                                     */
        /*-------------------------------------------------------------------*/

        EDDS_IS_VALID_PTR(pDDB);
        pDDB->pNext         = LSA_NULL;
        pDDB->pPrev         = LSA_NULL;
        pDDB->hDDB          = (EDDS_HANDLE)pDDB;
        pDDB->pGlob         = pGlob;
        pDDB->pNRT          = LSA_NULL;  /* so NRT-Component present yet */
        pDDB->pSRT          = LSA_NULL;  /* so SRT-Component present yet */

        *ppDDB = pDDB;
    }


    if ( Result == EDD_STS_OK )
    {
        /*---------------------------------------------------------------*/
        /* Init RQB scheduler queues (set to zero in EDDS_CreateDDB):    */
        /* Set internal RQB for trigger (from scheduler to RQB context)  */
        /*---------------------------------------------------------------*/
        pGlob->GeneralRequestFinishedTrigger.pRQB = EDDS_AllocInternalRQB();
        pGlob->FilterARPFrameTrigger.pRQB = EDDS_AllocInternalRQB();
        pGlob->FilterDCPFrameTrigger.pRQB = EDDS_AllocInternalRQB();
        pGlob->FilterDCPHELLOFrameTrigger.pRQB = EDDS_AllocInternalRQB();
        pGlob->triggerSchedulerRequest.pRQB = EDDS_AllocInternalRQB();
        pGlob->SchedulerEventsDone.pRQB = EDDS_AllocInternalRQB();

        if(  (LSA_HOST_PTR_ARE_EQUAL(pGlob->GeneralRequestFinishedTrigger.pRQB, LSA_NULL))
          || (LSA_HOST_PTR_ARE_EQUAL(pGlob->FilterARPFrameTrigger.pRQB, LSA_NULL))
          || (LSA_HOST_PTR_ARE_EQUAL(pGlob->FilterDCPFrameTrigger.pRQB, LSA_NULL))
          || (LSA_HOST_PTR_ARE_EQUAL(pGlob->FilterDCPHELLOFrameTrigger.pRQB, LSA_NULL))
          || (LSA_HOST_PTR_ARE_EQUAL(pGlob->triggerSchedulerRequest.pRQB, LSA_NULL))
          || (LSA_HOST_PTR_ARE_EQUAL(pGlob->triggerSchedulerRequest.pRQB, LSA_NULL))
          )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            pGlob->GeneralRequestFinishedTriggerPending = LSA_FALSE;
            EDDS_SetupInternalRQBParams(&pGlob->GeneralRequestFinishedTrigger,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_GEN_REQUEST_PROCESSED, 0);

            pGlob->FilterARPFrameTriggerPending = LSA_FALSE;
            EDDS_SetupInternalRQBParams(&pGlob->FilterARPFrameTrigger,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_FILTER_ARP_FRAME, 0);

            pGlob->FilterDCPFrameTriggerPending = LSA_FALSE;
            EDDS_SetupInternalRQBParams(&pGlob->FilterDCPFrameTrigger,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_FILTER_DCP_FRAME, 0);

            pGlob->FilterDCPHELLOFrameTriggerPending = LSA_FALSE;
            EDDS_SetupInternalRQBParams(&pGlob->FilterDCPHELLOFrameTrigger,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_FILTER_DCP_HELLO_FRAME, 0);

            pGlob->triggerSchedulerRequestPending = LSA_FALSE;
            pGlob->triggerSchedulerHighPriorRequestPending = LSA_FALSE;
            EDDS_SetupInternalRQBParams(&pGlob->triggerSchedulerRequest,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_SCHEDULER, 0);

            pGlob->SchedulerEventsDonePending = EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__NOTHING;
            EDDS_SetupInternalRQBParams(&pGlob->SchedulerEventsDone,
                                        EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                                        pDDB,
                                        EDDS_TRIGGER_SCHEDULER_EVENTS_DONE, 0);
        }
    }

    /*-----------------------------------------------------------------------*/
    /* on error we free all allocs                                           */
    /*-----------------------------------------------------------------------*/

    if (Result != EDD_STS_OK)
    {
        if (!LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL))
        {
            if (!LSA_HOST_PTR_ARE_EQUAL(pGlob->pTxInfo, LSA_NULL))
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat, pGlob->pTxInfo);
                EDDS_FREE_CHECK(FreeStat);
            }

            if (!LSA_HOST_PTR_ARE_EQUAL(pGlob->pMCInfo, LSA_NULL))
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat, pGlob->pMCInfo);
                EDDS_FREE_CHECK(FreeStat);
            }

            EDDS_PrmFree(&pGlob->Prm);

#ifdef EDDS_CFG_GLOB_FAST_MEM
            EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pGlob);
#else
            EDDS_FREE_LOCAL_MEM(&FreeStat,pGlob);
#endif
            EDDS_FREE_CHECK(FreeStat);

            EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pDDB);
            EDDS_FREE_CHECK(FreeStat);
        }
    }

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CreateDDB(), Status: 0x%X",
                           Result);

    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GetDDB                                 +*/
/*+  Input/Output          :    EDDS_HANDLE               hDDB              +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE * ppDDB             +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hDDB         : Valid DDB-Handle                                        +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets the DDB associated with the handle.                  +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM    is returned, if no DDB was found.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 10/11/2014 ppDDB will be set/allocated within this function, a null ptr would lead to EDD_STS_ERR_PARAM
//AD_DISCUSS 20/11/2014 better solution define a macro to capsule the check
LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_GetDDB(EDDS_HANDLE const hDDB, EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB)
{

    if (LSA_HOST_PTR_ARE_EQUAL((EDDS_LOCAL_DDB_PTR_TYPE)hDDB, LSA_NULL))
        return EDD_STS_ERR_PARAM;

    *ppDDB = (EDDS_LOCAL_DDB_PTR_TYPE) hDDB;
    return EDD_STS_OK;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CloseDDB                               +*/
/*+  Input/Output               EDDS_LOCAL_DDB_PTR_TYPE    pDDB             +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB to be closed                             +*/
/*+                                                                         +*/
/*+  Result:      ---                                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an DDB. The DDB is unqueued and the memory is freed+*/
/*+                                                                         +*/
/*+               A fatal error is signaled, if hDDB is invalid             +*/
/*+                                                                         +*/
/*+               A fatal error is signaled, if no DDB is present.          +*/
/*+                                                                         +*/
/*+               Note: This function does not check for open handles for   +*/
/*+                     this device!!                                       +*/
/*+                     Does not check for open components!                 +*/
/*+                                                                         +*/
/*+               Important: No internal RQB must be in use !!! not checked.+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 10/11/2014 pDDB will be checked in EDDS_GetDDB
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_CloseDDB(EDDS_HANDLE hDDB)
{

    LSA_RESULT              Status;
    LSA_UINT16              FreeStat;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32              TraceIdx;

    pDDB    = LSA_NULL;  /* to prevent compiler warning */

    Status = EDDS_GetDDB(hDDB, &pDDB);

    /* check if already TraceIdx can be accessed */
    if((EDD_STS_OK != Status) || LSA_HOST_PTR_ARE_EQUAL(0, pDDB) || LSA_HOST_PTR_ARE_EQUAL(0, pDDB->pGlob))
    {
        /* TraceIdx cannot be accessed */
        TraceIdx = EDDS_UNDEF_TRACE_IDX;
    }
    else
    {
        TraceIdx = pDDB->pGlob->TraceIdx;
    }

    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CloseDDB(hDDB: 0x%X)",
                           hDDB);

    if ( Status == EDD_STS_OK )
    {
        /*-----------------------------------------------------------------------*/
        /* Free global structure within DDB                                      */
        /*-----------------------------------------------------------------------*/
        EDDS_IS_VALID_PTR(pDDB);

        if ( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob, LSA_NULL) )
        {
            EDDS_FreeInternalRQB(&pDDB->pGlob->SchedulerEventsDone);
            pDDB->pGlob->SchedulerEventsDonePending = EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__NOTHING;

            EDDS_FreeInternalRQB(&pDDB->pGlob->triggerSchedulerRequest);
            pDDB->pGlob->triggerSchedulerRequestPending = LSA_FALSE;
            pDDB->pGlob->triggerSchedulerHighPriorRequestPending = LSA_FALSE;

            EDDS_FreeInternalRQB(&pDDB->pGlob->GeneralRequestFinishedTrigger);
            pDDB->pGlob->GeneralRequestFinishedTriggerPending = LSA_FALSE;

            EDDS_FreeInternalRQB(&pDDB->pGlob->FilterARPFrameTrigger);
            pDDB->pGlob->FilterARPFrameTrigger.pRQB = LSA_NULL;
            pDDB->pGlob->FilterARPFrameTriggerPending = LSA_FALSE;

            EDDS_FreeInternalRQB(&pDDB->pGlob->FilterDCPFrameTrigger);
            pDDB->pGlob->FilterDCPFrameTrigger.pRQB = LSA_NULL;
            pDDB->pGlob->FilterDCPFrameTriggerPending = LSA_FALSE;

            EDDS_FreeInternalRQB(&pDDB->pGlob->FilterDCPHELLOFrameTrigger);
            pDDB->pGlob->FilterDCPHELLOFrameTrigger.pRQB = LSA_NULL;
            pDDB->pGlob->FilterDCPHELLOFrameTriggerPending = LSA_FALSE;

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pTxInfo, LSA_NULL) )
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat,pDDB->pGlob->pTxInfo);
                EDDS_FREE_CHECK(FreeStat);
            }

            if ( (!pDDB->pGlob->ZeroCopyInterface) && (! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pTxTransferMem, LSA_NULL)) )
            {
                EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&FreeStat,pDDB->pGlob->pTxTransferMem);
                EDDS_FREE_CHECK(FreeStat);
            }

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pMCInfo, LSA_NULL) )
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat,pDDB->pGlob->pMCInfo);
                EDDS_FREE_CHECK(FreeStat);
            }

            EDDS_PrmFree(&pDDB->pGlob->Prm);

            #ifdef EDDS_CFG_GLOB_FAST_MEM
            EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pDDB->pGlob);
            #else
            EDDS_FREE_LOCAL_MEM(&FreeStat, pDDB->pGlob);
            #endif

            pDDB->pGlob = LSA_NULL;

            /* -----------------------------------------------------------------*/
            /* what can we do if free fails ?? Its fatal, because it should not */
            /* occur, but we also could ignore it, because its not a problem for*/
            /* us. Currently we signal an error and continue                    */
            /* -----------------------------------------------------------------*/

            EDDS_FREE_CHECK(FreeStat);
        }


        EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pDDB);

        /* ---------------------------------------------------------------------*/
        /* what can we do if free fails ?? Its fatal, because it should not     */
        /* occur, but we also could ignore it, because its not a problem for us */
        /* Currently we signal an error and continue                            */
        /* ---------------------------------------------------------------------*/

        EDDS_FREE_CHECK(FreeStat);

    }
    else
    {
        EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_FATAL,
                             "EDDS_CloseDDB: Invalid hDDB: 0x%X",
                             hDDB);

        EDDS_FatalError(EDDS_FATAL_ERR_CLOSE_DDB,
                        EDDS_MODULE_ID,
                        __LINE__);
    }


    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CloseDDB(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);
}


/*****************************************************************************/
/*  end of file edds_dev.c                                                   */
/*****************************************************************************/
