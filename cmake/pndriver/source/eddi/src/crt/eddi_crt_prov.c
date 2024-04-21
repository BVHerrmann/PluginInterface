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
/*  F i l e               &F: eddi_crt_prov.c                           :F&  */
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
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*              19.03.2004  ZA    Created for New Provider-Tables and Entries*/
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "eddi_int.h"
#include "eddi_dev.h"

#if defined (EDDI_CFG_SYSRED_2PROC)
    #include "eddi_ext.h"
    #include "eddi_lock.h"
#endif

#include "eddi_crt_ext.h"
#include "eddi_ser_ext.h"
#include "eddi_crt_check.h"
#include "eddi_crt_com.h"
//#include "eddi_swi_ptp.h"
//#include "eddi_prm_record_pdir_data.h"
//#include "eddi_prm_record_common.h"
#include "eddi_crt_dfp.h"
#include "eddi_pool.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_prov.h"
#endif

#include "eddi_sync_ir.h"
#include "eddi_irt_ext.h"
#include "eddi_io_provctrl.h"

#if !defined(EDDI_INTCFG_PROV_BUFFER_IF_SINGLE)
#include "eddi_crt_prov3B.h"
//#include "eddi_pool.h"
#include "eddi_io_provctrl.h"

#endif

/*===========================================================================*/
/*                                #defines                                   */
/*===========================================================================*/
#define EDDI_MODULE_ID     M_ID_CRT_PROV
#define LTRC_ACT_MODUL_ID  105

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*            header  of "private" functions of CRTProvider-"Class"          */
/*===========================================================================*/

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderCheckData( EDDI_CRT_PROVIDER_PTR_TYPE             const pProvider,
                                                                 EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE    const pFrmHandler,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                                 LSA_BOOL                               const bCheckBuffOnly);

/*===========================================================================*/
/*             functions  CRTProvider-"Class"                                */
/*===========================================================================*/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderIsInitialized( EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderAddEvent( EDDI_CRT_PROVIDER_PTR_TYPE              const pProvider,
                                                                EDD_UPPER_RQB_PTR_TYPE                  const pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                 const pDDB,
                                                                LSA_UINT8                               const ListType,
                                                                LSA_UINT8                               const ProviderType );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeStatus( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      EDDI_CRT_PROVIDER_PTR_TYPE        const  pProvider,
                                                                      LSA_UINT8                         const  ProvDataStatus,
                                                                      LSA_UINT8                         const  ProvDataStatusMask,
                                                                      LSA_UINT8                         const  GroupDataStatus,
                                                                      LSA_BOOL                          const  bActivate );

static LSA_UINT8  EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderCalculateDataStatus( LSA_UINT8                    const CurrentDataStatus,
                                                                           LSA_UINT8                    const DataStatusMask,
                                                                           LSA_UINT8                    const NewDataStatus );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckProviderPhase ( EDDI_LOCAL_DDB_PTR_TYPE              const pDDB,
                                                                   EDDI_CONST_CRT_PROVIDER_PTR_TYPE     const pProvider );
                                                                   
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTProviderAddToTree( EDDI_CRT_PROVIDER_PTR_TYPE          const  pProvider,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                    LSA_UINT32                              *  pCWOffset,
                                                                    LSA_UINT8                           const  ListType );
                                                                    
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlPassivate( EDDI_CRT_PROVIDER_PTR_TYPE            pProvider,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                                         LSA_BOOL                       const  bRemoveACW,
                                                                         LSA_BOOL                       const  bRemoveReq,
                                                                         LSA_BOOL                     * const  pIndicate,
                                                                         EDD_UPPER_RQB_PTR_TYPE         const  pRQB);                                                                                                                   

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompleteProvFCW( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                EDDI_CONST_CRT_PROVIDER_PTR_TYPE        const  pProvider,
                                                                LSA_BOOL                                const  bDBPtrOnly);

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderASAdd ( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                  EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                  EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                  LSA_BOOL                                 const  bCheckOnly);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeAutoStop( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                  EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                  EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                  LSA_BOOL                                 const  bUseAutoStop );

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModePassivateAux( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                                      EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                                      EDD_UPPER_RQB_PTR_TYPE               const  pRQB,
                                                                                      LSA_UINT16                           const  Mode,
                                                                                      LSA_BOOL                           * const  pIndicate);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModePassivate( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                   EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                   EDD_UPPER_RQB_PTR_TYPE                   const  pRQB,
                                                                                   LSA_UINT8                                const  GroupDataStatus,
                                                                                   LSA_BOOL                                 const  bUseAutoStop,
                                                                                   LSA_BOOL                               * const  pIndicate);  

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivate( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                  EDDI_CRT_PROVIDER_PTR_TYPE                const  pProvider,
                                                                                  EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE  const  pRQBParam,
                                                                                  LSA_UINT8                                 const  GroupDataStatus,
                                                                                  LSA_BOOL                                  const  bUseAutoStop );
                                                                                  
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateCheckAndSet( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                             EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                             EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                             LSA_BOOL                                 const  bUseAutoStop);

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateSetOnlyParameters( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                                   EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                                   EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                                   LSA_BOOL                                 const  bUseAutoStop );

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog(  EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                                                    EDDI_CONST_CRT_PROVIDER_PTR_TYPE       const  pProvider);
                                                                                  
                                                                                  
/*===========================================================================*/
/*             functions  CRTProviderList-"Class"                            */
/*===========================================================================*/


/******************************************************************************
 *  Function:    EDDI_SRTProviderAddToTree()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SRTProviderAddToTree( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                    LSA_UINT32                      *  pCWOffset,
                                                                    LSA_UINT8                   const  ListType )
{
    LSA_RESULT  Status = EDD_STS_OK;
   
    switch (ListType)
    {
        case EDDI_LIST_TYPE_ACW:
        {
            if (!pProvider->pLowerCtrlACW)
            {
                Status = EDDI_RedTreeSrtProviderAdd(pDDB, &pProvider->LowerParams, &pProvider->pLowerCtrlACW, pProvider->pCWStored);
                if (   (EDD_STS_OK == Status)
                    && (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType) )
                {
                    pProvider->pCWStored = pProvider->pLowerCtrlACW->pCW;
                    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
                    pProvider->bHadBeenActive = LSA_TRUE;
                    #endif
                }
            }
            #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            //return ptr to acw also
            if (EDD_STS_OK == Status)
            {
                LSA_UINT32  const CWOffset_Local = (LSA_UINT32)(pProvider->pLowerCtrlACW->pCW) - (LSA_UINT32)pDDB->pKRam;

                if /* User got the pointer to acw already */
                   (EDDI_IO_XCW_OFFSET_FOLLOWS != *pProvider->pIOCW)
                {
                    if //ptrs are not equal?
                       (*pProvider->pIOCW != (CWOffset_Local | EDDI_IO_OFFSET_IS_ACW))
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SRTProviderAddToTree, pIOCW exists. ProvID:0x%X, pIOCW:0x%X CWOffset_Local:0x%X", 
                                          pProvider->ProviderId, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                        EDDI_Excp("EDDI_SRTProviderAddToTree, pIOCW exists!", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                        return Status;
                    }  
                }
                else
                {
                    *pCWOffset = (((LSA_UINT32)pProvider->pIOCW) - (LSA_UINT32)pDDB->pKRam);
                    *pProvider->pIOCW = (CWOffset_Local | EDDI_IO_OFFSET_IS_ACW);
                }
                //set buffer-dependant values in DDB and ACW
                EDDI_CRTProviderSetBuffer(pDDB, pProvider);
                
                pProvider->bXCW_DBInitialized = LSA_TRUE;  
            }
            else
            {
                *pCWOffset = EDD_DATAOFFSET_INVALID;

                //set buffer-dependant values in DDB and ACW
                EDDI_CRTProviderSetBuffer(pDDB, pProvider);
            }
            #else
            *pCWOffset = EDD_DATAOFFSET_INVALID;
            #endif

            break;
        }

        case EDDI_LIST_TYPE_FCW:
        {
            if /* AUX-Prov exists? */
               (pProvider->IRTtopCtrl.pAscProvider)
            {
                    //as the AUX-Prov is initialized first, it inherits the APDU-Buffer here
                    pProvider->IRTtopCtrl.pAscProvider->LowerParams.pKRAMDataBuffer = pProvider->LowerParams.pKRAMDataBuffer;
            
                //first add AUX-provider
                Status = EDDI_RedTreeSrtProviderAdd(pDDB, &(pProvider->IRTtopCtrl.pAscProvider->LowerParams), &(pProvider->IRTtopCtrl.pAscProvider->pLowerCtrlACW), 0 /*pCWStored*/ );
                #if !defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
                *pCWOffset = EDD_DATAOFFSET_INVALID;
                #endif

                //Rest will be done in PROVIDER_CONTROL
            }
            break;
        }
        default:
           return EDD_STS_ERR_PARAM;
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderAddEvent()
 *
 *  Description: Uses the RQB to initialise the pProvider and starts the enqueuing
 *               of the HW-substructures.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderAddEvent( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                   EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   LSA_UINT8                   const  ListType,
                                                                   LSA_UINT8                   const  ProviderType )
{
    LSA_RESULT                                   Status;
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_CRT_PROVIDER_PTR_TYPE                   pAUXProvider;
    EDD_RQB_CSRT_PROVIDER_ADD_TYPE               AUXProvParam;
    LSA_UINT32                                   ctr;
    
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderAddEvent->");

    //Check MRPD (Redundant Frames)
    Status = EDDI_CRTCheckForProviderRedundantFrames(pDDB, pProvider, pProvParam);
    if (Status != EDD_STS_OK)
    {
        pProvider->IRTtopCtrl.pAscProvider = (EDDI_CRT_PROVIDER_PTR_TYPE)0;
        return Status;
    }

    //allocate AUX-provider if necessary
    if (   (EDDI_RTC3_PROVIDER == ProviderType)
        && (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
    {
        //fill parameters for a simulated AUX_PROVIDER_ADD
        AUXProvParam                           = *pProvParam;  
        AUXProvParam.IOParams.BufferProperties = EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC;
        AUXProvParam.IOParams.DataOffset       = EDD_DATAOFFSET_UNDEFINED; //stays undefined
        AUXProvParam.Properties                = EDDI_CSRT_PROV_PROP_RTCLASS_AUX;

        Status = EDDI_CRTProviderListReserveFreeEntry(pDDB, &pAUXProvider, pDDB->pLocal_CRT, EDDI_RTC3_AUX_PROVIDER, LSA_FALSE);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderAddEvent, ERROR finding free Entry, Status:0x%X", Status);
            return Status;
        }

        // set references from Class3-prov. to AUX-prov and vv
        pProvider->IRTtopCtrl.pAscProvider     = pAUXProvider;
        pAUXProvider->IRTtopCtrl.pAscProvider  = pProvider;
        
        //Set MAC Address for AUX-Provider
        for (ctr = 0; ctr < EDD_MAC_ADDR_SIZE; ctr++)
        {
            pProvider->LowerParams.DstMAC.MacAdr[ctr] = 0xFF;
        }
        
        //init AUX-provider
        Status = EDDI_CRTProviderInit(pDDB, pAUXProvider, &AUXProvParam, EDDI_LIST_TYPE_ACW, EDDI_RTC3_AUX_PROVIDER);
        if (Status != EDD_STS_OK)
        {
            return Status;
        }
    }
    else
    {
        //Either Class1/2-Provider or MC-Class3-Provider
        pProvider->IRTtopCtrl.pAscProvider = (EDDI_CRT_PROVIDER_PTR_TYPE)0;
    }

    //init Class1,2,3-provider
    Status = EDDI_CRTProviderInit(pDDB, pProvider, (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)(pRQB->pParam), ListType, ProviderType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    //allocate a separate 4B-buffer
    //as the bufferproperties are not known yet, the 4B buffer has to be allocated even
    //for sync-providers!
    EDDI_MemGetPoolBuffer(pDDB->CRT.hPool.IOCWRef, (void * *)&pProvider->pIOCW);
    if (EDDI_NULL_PTR == pProvider->pIOCW)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderAddEvent, out of internal 4B-Buffer for 3BIF_SW -> xCWProviderCnt:0x%X", 
                          pDDB->CRT.MetaInfo.AcwProviderCnt + pDDB->CRT.MetaInfo.FcwProviderCnt);
        return EDD_STS_ERR_RESOURCE;
    }
    else
    {
        *pProvider->pIOCW = EDDI_IO_XCW_OFFSET_FOLLOWS;
    } 
    pProvParam->IOParams.CWOffset = (LSA_UINT32)(((LSA_UINT32)pProvider->pIOCW) - (LSA_UINT32)pDDB->pKRam);

    #else
    pProvParam->IOParams.CWOffset = EDD_DATAOFFSET_INVALID;
    #endif

    #if defined (EDDI_CFG_SYSRED_2PROC)
    pProvParam->IOParams.DataStatusOffset = pProvider->ProviderId; //Index into shadow array
    #else
    pProvParam->IOParams.DataStatusOffset = pProvider->LowerParams.DataStatusOffset;
    #endif
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlPassivate()
 *
 *  Description: 
 *
 *  Arguments:
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlPassivate( EDDI_CRT_PROVIDER_PTR_TYPE         pProvider,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                         LSA_BOOL                    const  bRemoveACW,
                                                                         LSA_BOOL                    const  bRemoveReq,
                                                                         LSA_BOOL                  * const  pIndicate,
                                                                         EDD_UPPER_RQB_PTR_TYPE      const  pRQB)
{
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderControlPassivate->ProvID:0x%X ProvActiveMask:0x%X", 
                      pProvider->ProviderId, pProvider->IRTtopCtrl.ProvActiveMask);

    if (EDDI_LIST_TYPE_ACW == pProvider->LowerParams.ListType)
    {
        if (   (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status)
            #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
            && (pProvider->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
            #endif
           )
        {
            #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            LSA_UINT32  length3b = pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    
            // Preserve 4 Byte-Alignment
            length3b = (length3b + 0x03) & ~0x00000003;
            #endif

            EDDI_RedTreeSrtProviderSetActivity(pProvider->pLowerCtrlACW, LSA_FALSE);
            pProvider->PendingIndEvent = 0;
            pProvider->Status = EDDI_CRT_PROV_STS_INACTIVE;
            
            if (0 == pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "EDDI_CRTProviderControlPassivate->ProvID:0x%X, Counter ProviderList.ActiveProvRTC12 already 0", pProvider->ProviderId);
                EDDI_Excp("EDDI_CRTProviderControlPassivate, Counter ProviderList.ActiveProvRTC12 already 0", EDDI_FATAL_ERR_EXCP,pProvider->ProviderId, 0);
                return;
            }
            else
            {
                pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12--;
            }
        }

        // Async removing of ACW. If passivating occurs during Remove.req, the ACW gets removed during pocessing the removal.
        if (   (EDDI_NULL_PTR != pProvider->pLowerCtrlACW) 
            && !bRemoveReq )
        {
            // Save context for later function "EDDI_CRTProviderPassivateEventFinish"
            if (pDDB->CRT.ContextProvRemove.InUse)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlPassivate (ContextProvRemove.InUse == TRUE). ProvID:0x%X", 
                            pProvider->ProviderId);
                EDDI_Excp("EDDI_CRTProviderControlPassivate (ContextProvRemove.InUse == TRUE)", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId,0);
                return;
            }
            else
            {
                pDDB->CRT.ContextProvRemove.InUse     = LSA_TRUE;
                pDDB->CRT.ContextProvRemove.pProvider = pProvider;
            }
            
            if (   bRemoveACW            
                #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
                && (pProvider->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
                #endif
                )
            {
                //permanent removal of ACW required, no reactivation allowed
                pProvider->pCWStored = (EDDI_SER_CCW_PTR_TYPE)0; //This will also result in deallocating the ACW in EDDI_CRTProviderRemoveEventFinish()
                pProvider->bActivateAllowed = LSA_FALSE;    //only action allowed now is "REMOVE"
            }
            EDDI_RedTreeSrtProviderACWRemovePart1(pDDB, pRQB, pProvider->pLowerCtrlACW);

            *pIndicate = LSA_FALSE; // No EDDI_RequestFinish yet !!
        }
        else if (   (pProvider->pCWStored)
                 && bRemoveACW            
                 #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
                 && (pProvider->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
                 #endif
                 )
        {
            //Usecase: Provider had been passivated normally before, and then a CONTROL.req(PASSIVATE+REMOVE_ACW) occurs
            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.AcwProvider, (EDDI_DEV_MEM_PTR_TYPE)(pProvider->pCWStored));

            if (0 == pDDB->pLocal_CRT->ProviderList.UsedACWs)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlPassivate: UsedACWs=0. ProvID:0x%X, UsedEntries:0x%X", 
                    pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
                EDDI_Excp("EDDI_CRTProviderControlPassivate: UsedACWs=0", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
                return;   
            }
    
            pDDB->pLocal_CRT->ProviderList.UsedACWs--;
            pProvider->pCWStored = (EDDI_SER_CCW_PTR_TYPE)0; 
            pProvider->bActivateAllowed = LSA_FALSE;    //only action allowed now is "REMOVE"
        }
    }
    else if (EDDI_LIST_TYPE_FCW == pProvider->LowerParams.ListType)
    {
        if (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status)
        {
            if (pProvider->IRTtopCtrl.ProvActiveMask & EDDI_PROV_ACTIVE_AUX_MAIN)
            {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlEventModePassivate. AUX should not be active. ProvID:0x%X", 
                              pProvider->ProviderId);
                    EDDI_Excp("EDDI_CRTProviderControlEventModePassivate. AUX should not be active.", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId,0);
                    return;
            }

            if (pProvider->IRTtopCtrl.ProvActiveMask & EDDI_PROV_ACTIVE_RTC3_MAIN)
            {
                if (EDDI_IS_DFP_PROV(pProvider))
                {
                    LSA_RESULT Status;
                    
                    EDDI_DFPProviderSetActivity(pDDB, pProvider, LSA_FALSE /*bActivate*/ );
                    
                    Status = EDDI_DFPProviderPassivate(pDDB, pProvider);
                    if (EDD_STS_OK != Status)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlPassivate EDDI_DFPProviderPassivate failed. ProviderId:0x%X", 
                                          pProvider->ProviderId);
                        EDDI_Excp("EDDI_CRTConsumerPassivate EDDI_DFPConsumerPassivate failed", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, 0);
                        return;
                    }
                                   
                    Status = EDDI_DFPProvPassPFFrame(pDDB, pProvider);
                    if (EDD_STS_OK != Status)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlPassivate EDDI_DFPProviderPassivate failed. ProviderId:0x%X", 
                                          pProvider->ProviderId);
                        EDDI_Excp("EDDI_CRTProviderControlPassivate EDDI_DFPProviderPassivate failed", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, 0);
                        return;
                    }
                }
                else
                {
                    //passivate class3-prov
                    EDDI_CRTProviderSetActivity(pDDB, pProvider, LSA_FALSE);
                    pProvider->PendingIndEvent = 0;
                    pProvider->Status = EDDI_CRT_PROV_STS_INACTIVE;

                    if (0 == pDDB->pLocal_CRT->ProviderList.ActiveProvRTC3)
                    {
                        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlPassivate, Counter ProviderList.ActiveProvRTC3 already 0");
                        EDDI_Excp("EDDI_CRTProviderControlPassivate, Counter ProviderList.ActiveProvRTC3 already 0", EDDI_FATAL_ERR_EXCP,pProvider->ProviderId, 0);
                        return;
                    }
                    else
                    {
                        pDDB->pLocal_CRT->ProviderList.ActiveProvRTC3--;
                    }

                    if (pProvider->pFrmHandler->UsedByUpper)
                    {
                        LSA_UINT32  FSOTerm;
                        LSA_UINT32  ClkCntStart;

                        //Remove first the x+1 Redundant framehandler, if exist
                        if (pProvider->pFrmHandler->pFrmHandlerRedundant)
                        {
                            //remove bindings between redundant fcws and provider
                            EDDI_IRTProviderRemove(pDDB, pProvider->pFrmHandler->pFrmHandlerRedundant);
                            //Get time by which the provider surely will not be accessed any more
                            pProvider->pFrmHandler->pFrmHandlerRedundant->UsedByUpper = LSA_FALSE;
                            //Do not kill pFrmHandler->pFrmHandlerRedundant (= EDDI_NULL_PTR), because you will destroy the associate redundant FrameID list
                            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderControlPassivate, FrameHandler->pFrmHandlerRedundant FREED: pROVId:0x%X FrameId:0x%X pFrmHandler:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId, (LSA_UINT32)pProvider->pFrmHandler);
                        }
                        //remove bindings between fcws and provider
                        EDDI_IRTProviderRemove(pDDB, pProvider->pFrmHandler);
                        
                        ClkCntStart = IO_R32(CLK_COUNT_VALUE);
                        //Get time by which the provider surely will not be accessed any more
                        do
                        {
                            //waste some time
                            FSOTerm = (80UL/EDDI_CRT_NS_PER_CLK_COUNT_TICK)*(pProvider->LowerParams.DataLen + EDDI_CRT_PROVIDER_HEADER_OVERHEAD);
                            pProvider->pFrmHandler->UsedByUpper        = LSA_FALSE;
                            //note: bXCW_DBInitialized shall not be set to FALSE, because the state is not to be changed until PROVIDER_REMOVE!
                        }
                        while ((IO_R32(CLK_COUNT_VALUE) - ClkCntStart) < FSOTerm);

                        pProvider->pFrmHandler = EDDI_NULL_PTR;

                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderControlPassivate, FrameHandler FREED: pROVId:0x%X FrameId:0x%X pFrmHandler:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId, (LSA_UINT32)pProvider->pFrmHandler);
                    }
                }
            }

            //now inform SM
            EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_PASSIVATE);
        }
    }
    else
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlPassivate, pProvider->LowerParams.ListType:0x%X", pProvider->LowerParams.ListType);
        EDDI_Excp("EDDI_CRTProviderControlPassivate, pProvider->LowerParams.ListType", EDDI_FATAL_ERR_EXCP, pProvider->LowerParams.ListType, 0);
        return;
    } 
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderRemoveEvent()
 *
 *  Description: Uses the RQB to remove the pProvider and starts the de-queuing
 *               of the HW-substructures.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderRemoveEvent( EDD_UPPER_RQB_PTR_TYPE         const  pRQB,
                                                              EDDI_CRT_PROVIDER_PTR_TYPE            pProvider,
                                                              EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                                              LSA_BOOL                    *  const  pIndicate )
{
    LSA_RESULT  Status = EDD_STS_OK;

    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderRemoveEvent->pProvider->Status:0x%X", pProvider->Status);

    *pIndicate = LSA_TRUE;

    // passivate Aux first, only if active
    if (pProvider->IRTtopCtrl.pAscProvider)
    {
        if ( EDDI_CRT_PROV_STS_ACTIVE == pProvider->IRTtopCtrl.pAscProvider->Status )
        {
            LSA_BOOL IndicateLocal;

            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderRemoveEvent, ProvID:0x%X is removed while being active.Passivating AUX.", pProvider->ProviderId);
            
            Status = EDDI_CRTProviderControlEventModePassivateAux( pDDB, pProvider, pRQB, 0 /*Mode*/, &IndicateLocal ); 
            if (EDD_STS_OK != Status ) 
            {
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderRemoveEvent, failed to execute the passivate (aux) cmd");
                return EDD_STS_ERR_PARAM;
            } 
        }
    }

    // passivate provider aswell, only if active
    if ( EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status )
    {
        LSA_BOOL IndicateLocal;

        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderRemoveEvent, ProvID:0x%X is removed while being active.Passivating.", pProvider->ProviderId);
        
        EDDI_CRTProviderControlPassivate(pProvider, pDDB, LSA_FALSE /*bRemoveACW*/, LSA_TRUE /*bRemoveReq*/, &IndicateLocal, pRQB);
    }

    Status = EDDI_CrtASCtrlSingleProviderList(pDDB, pProvider, LSA_TRUE /*RemoveASCoupling*/, LSA_FALSE /*DeactivateProvider*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "ERROR: EDDI_CRTProviderRemoveEvent -> Status:0x%X", Status);
    }

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        //Remove DFP DG Provider, or Packframe, if this is the last DG Provider
        Status = EDDI_DFPProvRemovePFHandler(pRQB, pProvider, pDDB, pIndicate);
        if (Status != EDD_STS_OK)
        {
            return Status;
        }
        EDDI_CRTProviderListUnReserveEntry(pProvider, pDDB->pLocal_CRT, LSA_TRUE /*bIsDFP*/);
    }
    else
    {
    switch (pProvider->LowerParams.ListType)
    {
        case EDDI_LIST_TYPE_FCW:
        {
            EDDI_CRT_PROVIDER_PTR_TYPE  const pAUXProvider = pProvider->IRTtopCtrl.pAscProvider;
        
            //Step 1: remove Class3-provider

            #if defined (EDDI_CFG_REV7)
            if (!pProvider->usingPAEA_Ram)
            {
                EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                             pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_REMOVE);
            }
            else
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderRemoveEvent: PAEA-APDU Pool --> deleting buffer 0x%X",
                                  (LSA_UINT32)pProvider->LowerParams.pKRAMDataBuffer);

                // Do nothing! Memory will be overwritten during the next Add-Provider() with the same ProviderID
            }
            #else
            EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                         pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_REMOVE);
            #endif

            #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            //dealloc reference to 4B-Buffer for 3BSW-IF
            //MRPD providers did not allocate a buffer!
            if (pProvider->pIOCW)
            {
                EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.IOCWRef, pProvider->pIOCW);
            }
            #endif

            //remove provider
            EDDI_CRTProviderListUnReserveEntry(pProvider, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);

            //now remove AUX-Provider
            if /* no AUX-Prov exists */
               (EDDI_NULL_PTR == pAUXProvider)
            {               
                break;
            }
            else
            {
                pProvider = pAUXProvider; //for fallthrough to next case
            }
        }
        //no break;
        //lint -fallthrough

        case EDDI_LIST_TYPE_ACW:
        {
            pProvider->Status = EDDI_CRT_PROV_STS_RESERVED;
            // Async removing of Provider
            if (EDDI_NULL_PTR != pProvider->pLowerCtrlACW)
            {
                // Save context for later function "EDDI_CRTProviderRemoveEventFinish"
                if (pDDB->CRT.ContextProvRemove.InUse)
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderRemoveEvent (ContextProvRemove.InUse == TRUE). ProvID:0x%X", 
                              pProvider->ProviderId);
                    EDDI_Excp("EDDI_CRTProviderRemoveEvent (ContextProvRemove.InUse == TRUE)", EDDI_FATAL_ERR_EXCP,  0,0);
                    return EDD_STS_ERR_EXCP;
                }
                else
                {
                    pDDB->CRT.ContextProvRemove.InUse     = LSA_TRUE;
                    pDDB->CRT.ContextProvRemove.pProvider = pProvider;
                }
            
                //permanent removal of ACW
                pProvider->pCWStored = (EDDI_SER_CCW_PTR_TYPE)0; //This will also result in deallocating the ACW in EDDI_CRTProviderRemoveEventFinish()
                EDDI_RedTreeSrtProviderACWRemovePart1(pDDB, pRQB, pProvider->pLowerCtrlACW);
                //EDDI_CRTCheckAddRemoveMirror will be called later in CallBack !
                *pIndicate = LSA_FALSE; // No EDDI_RequestFinish yet !!
            }
            else
            {
                if (pProvider->pCWStored)
                {
                    EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.AcwProvider, (EDDI_DEV_MEM_PTR_TYPE)(pProvider->pCWStored));

                    if (0 == pDDB->pLocal_CRT->ProviderList.UsedACWs)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderRemoveEvent: UsedACWs=0. ProvID:0x%X, UsedEntries:0x%X", 
                            pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
                        EDDI_Excp("EDDI_CRTProviderRemoveEvent: UsedACWs=0", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
                        return EDD_STS_ERR_EXCP;   
                    }
    
                    pDDB->pLocal_CRT->ProviderList.UsedACWs--;
                    pProvider->pCWStored = 0;
                }

                //CleanUp Context
                pProvider->pLowerCtrlACW              = EDDI_NULL_PTR;
                pDDB->CRT.ContextProvRemove.InUse     = LSA_FALSE;
                pDDB->CRT.ContextProvRemove.pProvider = EDDI_NULL_PTR;
                #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
                //dealloc reference to 4B-Buffer for 3BSW-IF
                if (pProvider->pIOCW)
                {
                    EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.IOCWRef, pProvider->pIOCW);
                }
                #endif
                EDDI_CRTProviderListUnReserveEntry(pProvider, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);
            }
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderRemoveEvent, ListTyp Error, ListType:0x%X", pProvider->LowerParams.ListType);
            EDDI_Excp("EDDI_CRTProviderRemoveEvent ListTyp Error: ListType:0x%X", EDDI_FATAL_ERR_EXCP, pProvider->LowerParams.ListType, 0);
            return EDD_STS_ERR_PARAM;   
        }
    }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderRemoveEventFinish()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderRemoveEventFinish( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_BOOL                 const  bPassivate)
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pProvider;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderRemoveEventFinish");

    //Restore Context
    if (   (!(pDDB->CRT.ContextProvRemove.InUse))
        || (pDDB->CRT.ContextProvRemove.pProvider == 0))
    {
        EDDI_Excp("EDDI_CRTProviderRemoveEventFinish, no valid ContextProvRemove found", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pProvider = pDDB->CRT.ContextProvRemove.pProvider;

    //Check Status-Conditions
    if (   ((EDDI_CRT_PROV_STS_RESERVED != pProvider->Status) && !bPassivate)
        || ((EDDI_CRT_PROV_STS_RESERVED == pProvider->Status) &&  bPassivate) )
    {
        EDDI_Excp("EDDI_CRTProviderRemoveEventFinish, invalid pProvider->Status", EDDI_FATAL_ERR_EXCP, pProvider->Status, 0);
        return;
    }

    EDDI_RedTreeSrtProviderRemovePart2(pDDB, pProvider->pLowerCtrlACW, &pProvider->LowerParams, (pProvider->pCWStored)?LSA_FALSE:LSA_TRUE /*bFreeACW*/);

    //reverse actions done during PROVIDER_ADD and ACTIVATE
    if (pProvider->LowerParams.ProviderType != EDDI_RTC3_AUX_PROVIDER)
    {
        #if defined (EDDI_CFG_REV7)
        if (!pProvider->usingPAEA_Ram)
        {
            EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                         pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_REMOVE);
        }
        #else
        EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                     pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_REMOVE);
        #endif
    }

    if (!bPassivate)
    {
        //reverse actions done during PROVIDER_ADD
        #if defined (EDDI_CFG_REV7)
        if (pProvider->LowerParams.ProviderType != EDDI_RTC3_AUX_PROVIDER)
        {
            if (pProvider->usingPAEA_Ram)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDDI_CRTProviderRemoveEventFinish: PAEA-APDU Pool --> deleting buffer 0x%X",
                                  (LSA_UINT32)pProvider->LowerParams.pKRAMDataBuffer);

                //Do nothing! Memory will be overwritten during the next Add-Provider() with the same ProviderID!
            }
        }
        #endif

        #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        //dealloc reference to 4B-Buffer for 3BSW-IF
        if (pProvider->pIOCW)
        {
            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.IOCWRef, pProvider->pIOCW);
        }
        #endif
    }

    //CleanUp Context
    pDDB->CRT.ContextProvRemove.InUse     = LSA_FALSE;
    pDDB->CRT.ContextProvRemove.pProvider = EDDI_NULL_PTR;
    pProvider->pLowerCtrlACW              = EDDI_NULL_PTR;
    
    if (0 == pDDB->pLocal_CRT->ProviderList.UsedACWs)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderRemoveEventFinish: UsedACWs=0. ProvID:0x%X, UsedEntries:0x%X", 
            pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
        EDDI_Excp("EDDI_CRTProviderRemoveEventFinish: UsedACWs=0", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pDDB->pLocal_CRT->ProviderList.UsedEntries);
    }
    
    //check if provider had not been passivated normally
    if (!pProvider->pCWStored)
    {
        pDDB->pLocal_CRT->ProviderList.UsedACWs--;
    }

    if (!bPassivate)
    {
        EDDI_CRTProviderListUnReserveEntry(pProvider, pDDB->pLocal_CRT, EDDI_IS_DFP_PROV(pProvider)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);

        //Update MaxUsedReduction
        if (pDDB->pLocal_CRT->ProviderList.UsedEntries == 0)
        {
            pDDB->pLocal_CRT->ProviderList.MaxUsedReduction = 0;
        }
    }
    else
    {
        if (EDDI_RTC3_AUX_PROVIDER == pProvider->LowerParams.ProviderType)
        {
            //now inform SM (it does not matter if AUX had been passivated before)
            EDDI_CRTProviderIRTtopSM(pDDB, pProvider->IRTtopCtrl.pAscProvider, EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX);

            //if RTC3-prov exists, passivate this now
            if (pProvider->IRTtopCtrl.pAscProvider)
            {
                EDDI_CRT_PROVIDER_PTR_TYPE                  const pRTC3Provider = pProvider->IRTtopCtrl.pAscProvider;
                LSA_BOOL                                          Indicate;

                EDDI_CRTProviderControlPassivate(pRTC3Provider, pDDB, LSA_FALSE /*bRemoveACW*/, LSA_FALSE /*bRemoveReq*/, &Indicate, pRQB);
                //ProvDataStatus has not to be taken care here (RTF)
            }
        }
    }

    EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/



/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog()
 *
 *  Description: 
 *
 *  Arguments:     pDDB is used for trace
 *
 *  Return:      LSA_RESULT
 */

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog(  EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                                                    EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    //check value of watchdog register
    //in this case the use of watchdog is not allowed
    LSA_UINT32 const LocalRegApplWD = IO_R32(APP_WATCHDOG);

    if ( ( 0x00 == LocalRegApplWD ) || (0xFF == LocalRegApplWD ) )
    {
        // application watchdog is not in use
        return EDD_STS_OK;
    }
    else
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog: Usage of application watchdog is not allowed!, ProvID:0x%X, FrameID:0x%X, ApplWD:0x%X",
            pProvider->ProviderId, pProvider->LowerParams.FrameId, LocalRegApplWD);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Provider_Properties);

        EDDI_Excp("EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog: Usage of application watchdog is not allowed!", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, LocalRegApplWD);
        
        return EDD_STS_ERR_PARAM;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModeActivate()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
 
 
 
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateSetOnlyParameters( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                                   EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                                   EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                                   LSA_BOOL                                 const  bUseAutoStop )
{    
    //***********************************************************
    //in IIIB-SW-Mode: PREPARE FCW/ACW for activation
    //initialize provider-fcw
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_RESULT  Result = EDD_STS_OK;

    if (EDDI_CRT_PROV_STS_ACTIVE != pProvider->Status)
    {   
        //check for change in DataOffset (can only happen once from invalid->valid)!
        //provider gets activated ==> check parameters for dataoffset and BufferProperties
        //if there had been valid parameters given at PROVIDER_ADD, ignore parameters here!
        Result = EDDI_CRTProviderControlEventModeActivateCheckAndSet( pDDB, pProvider, pRQBParam, bUseAutoStop );
        if (EDD_STS_OK != Result)
        {
            //Details have already been set
            return (Result);
        }
        
        if (   (EDDI_RTC3_PROVIDER == pProvider->LowerParams.ProviderType)                                                          //RTC3-provider ...
            && (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))  //... AND ASYNC mode ...
            && (!pProvider->bXCW_DBInitialized)                                                                                     //... AND buffer not initialized yet ...
            && (!pProvider->bRedundantFrameID))                                                                                     //... AND no MRPD-Prov.
        {
            EDDI_IRT_FRM_HANDLER_PTR_TYPE  pFrmHandler; 
            LSA_UINT32                     pFCW;
        
            //Check for valid IRDATA for this provider
            pFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pProvider->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
            if (EDDI_NULL_PTR == pFrmHandler)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent no IRDATA found for ProvId:0x%X, FrameID:0x%X",
                    pProvider->ProviderId, pProvider->LowerParams.FrameId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                return EDD_STS_ERR_SEQUENCE;
            }
            else
            {
                //Check if IRDATA match to provider parameters
                Result = EDDI_CRTProviderCheckData(pProvider, pFrmHandler, pDDB, LSA_TRUE /*bCheckBuffOnly*/);
                if (EDD_STS_OK != Result)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent, IRDATA found but do not match. ProvID:0x%X FrameID:0x%X",
                                      pProvider->ProviderId, pProvider->LowerParams.FrameId);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                    return EDD_STS_ERR_PARAM;
                }
                else
                {
                    LSA_UINT32  CWOffset_Local;

                    pProvider->LowerParams.pKRAMDataBuffer = pDDB->pKRam + pProvider->LowerParams.DataOffset;

                    pFCW = EDDI_CRTCompleteProvFCW(pDDB, pProvider, LSA_TRUE /*bDBPtrOnly*/);
                    if (0 == pFCW)
                    {
                        EDDI_Excp("EDDI_CRTProviderControlEvent, no PDIRDATA found!", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->LowerParams.FrameId);
                        return EDD_STS_ERR_PARAM;
                    }  

                    CWOffset_Local = (LSA_UINT32)pFCW - (LSA_UINT32)pDDB->pKRam;
                    
                    //insert address of FCW into 4B-Buffer
                    if /* User got the pointer to acw already */
                       (EDDI_IO_XCW_OFFSET_FOLLOWS != *pProvider->pIOCW)
                    {
                        if  /* ptrs are not equal */
                            (*pProvider->pIOCW != (CWOffset_Local | EDDI_IO_OFFSET_IS_FCW))
                        {
                            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlEvent, pIOCW exists. ProvID:0x%X, pIOCW:0x%X CWOffset_Local:0x%X", 
                                              pProvider->ProviderId, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                            EDDI_Excp("EDDI_CRTProviderControlEvent, pIOCW exists!", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                            return Result;
                        }  
                    }
                    else
                    {
                        *pProvider->pIOCW = (CWOffset_Local | EDDI_IO_OFFSET_IS_FCW);
                    }

                    pProvider->bXCW_DBInitialized = LSA_TRUE;
                }
            }
        }
        else if ( (    (EDDI_RTC1_PROVIDER == pProvider->LowerParams.ProviderType)                                                  //RTC1-provider ...
                    || (EDDI_RTC2_PROVIDER == pProvider->LowerParams.ProviderType))                                                 //RTC2-provider ...
            && (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))  //... AND ASYNC mode ...
            && (!pProvider->bXCW_DBInitialized)                                                                                     //... AND buffer not initialized yet ...
            && (!pProvider->bRedundantFrameID))                                                                                     //... AND no MRPD-Prov.
        {        
            //***********************************************************
            //additional checks for activation
            //check CyclePhase, CyclePhaseSequence and CycleReductionRatio
            Result = EDDI_CRTCheckProviderPhase(pDDB, pProvider);
            if (EDD_STS_OK != Result)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_CRTCheckProviderPhase, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
                return Result;
            } 
                                          
            //Add Provider to Tree
            Result = EDDI_SRTProviderAddToTree(pProvider, pDDB, &pRQBParam->IOParams.CWOffset, EDDI_LIST_TYPE_ACW);
            if(EDD_STS_OK != Result)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_SRTProviderAddToTree, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AddToTree);
                return Result;
            }

            //Set buffer only once 
            EDDI_CRTProviderSetBuffer(pDDB, pProvider);
        }
    }
    return Result; 

    #else
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);
    LSA_UNUSED_ARG(bUseAutoStop);
    LSA_UNUSED_ARG(pRQBParam);
    return EDD_STS_OK;
    #endif
}                                                                                  
/*---------------------- end [subroutine] ---------------------------------*/
                                                                                  

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivateCheckAndSet( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                             EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                             EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                             LSA_BOOL                                 const  bUseAutoStop)
{
    LSA_RESULT                                       Result                     = EDD_STS_OK;
    LSA_UINT32                                const  BufferProperties           = (EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))?pRQBParam->IOParams.BufferProperties:pProvider->LowerParams.BufferProperties;
    LSA_UINT32                                const  DataOffset                 = (EDD_DATAOFFSET_UNDEFINED == pProvider->LowerParams.DataOffset)?pRQBParam->IOParams.DataOffset:pProvider->LowerParams.DataOffset;
    LSA_UINT16                                const  Mode                       = pRQBParam->Mode;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE          const  pCRTComp                   = pDDB->pLocal_CRT;
    EDDI_CRT_PROVIDER_PARAM_TYPE                     LowerParamsLocal           = pProvider->LowerParams;               //pProvider->LowerParams get updated if all checks are ok
    LSA_UINT16                                       CyclePhaseAUX = EDD_CYCLE_PHASE_UNDEFINED;
    LSA_UINT32                                       CyclePhaseSequenceAUX = EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
    LSA_UINT16                                       CycleReductionRatioAUX = EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;

    #if defined (EDDI_CFG_REV7)
    if (     (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
         &&  (EDD_CSRT_PROV_PROP_RTCLASS_3 == (pProvider->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
         #if defined (EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
         &&  (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
         #endif //(EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
       )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderControlEventModeActivateCheckAndSet: Sync Image and Legacy Mode and RTC3 is on SOC1/2 not allowed");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Provider_Properties);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    //check if activation is allowed here
    if (!pDDB->NRT.bIO_Configured)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderControlEventModeActivateCheckAndSet: activating Provider(ID:0x%X) when IO-Configuration is NO, is not allowed",
                         pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Provider_Properties);
        return EDD_STS_ERR_SERVICE;
    }

    if (!pDDB->NRT.bFeedInLoadLimitationActive)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                         "EDDI_CRTProviderControlEventModeActivateCheckAndSet: activating Provider(ID:0x%X) when LoadLimitationActive is FALSE, is not allowed",
                         pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Provider_Properties);
        return EDD_STS_ERR_SERVICE;
    }

    if (pRQBParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX)
    {
        if (!pProvider->bActivateAllowed)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
            return (EDD_STS_ERR_SEQUENCE);
        }
        if (EDD_STS_OK != (Result = EDDI_CRTProviderIRTtopSMStateCheck(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX)) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
            return Result;
        }
        
        if (pProvider->IRTtopCtrl.pAscProvider)
        {
            CyclePhaseAUX           = pProvider->IRTtopCtrl.pAscProvider->LowerParams.CyclePhase;
            CyclePhaseSequenceAUX   = pProvider->IRTtopCtrl.pAscProvider->LowerParams.CyclePhaseSequence;
            CycleReductionRatioAUX  = pProvider->IRTtopCtrl.pAscProvider->LowerParams.CycleReductionRatio;
        }
    }
    
    if (pRQBParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE)
    {
        if (!pProvider->bActivateAllowed)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
            return (EDD_STS_ERR_SEQUENCE);
        }
        if (EDD_STS_OK != (Result = EDDI_CRTProviderIRTtopSMStateCheck(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_ACTIVATE)) ) 
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
            return Result;
        }
    }

    //check for change in DataOffset (can only happen once from invalid->valid)!
    //provider gets activated ==> check parameters for dataoffset and BufferProperties
    //if there had been valid parameters given at PROVIDER_ADD, ignore parameters here!
    if (EDD_DATAOFFSET_UNDEFINED == LowerParamsLocal.DataOffset)
    {
        if (   (EDD_DATAOFFSET_UNDEFINED != pRQBParam->IOParams.DataOffset)
            && (EDDI_RTC3_AUX_PROVIDER != LowerParamsLocal.ProviderType))
        {
            if (!EDDI_IS_DFP_PROV(pProvider))
            {
                //change dataoffset
                Result = EDDI_CRTCheckOverlaps(pDDB, DataOffset, LowerParamsLocal.DataLen, LowerParamsLocal.ListType, LSA_TRUE, ((LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)));
                if (EDD_STS_OK != Result)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal DataOffset:0x%X Result:0x%X", DataOffset, Result);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
                    return EDD_STS_ERR_PARAM;
                }
            }
            LowerParamsLocal.DataOffset = pRQBParam->IOParams.DataOffset;
        }
    }
        
    //check for change in Buffer properties (can only happen once from invalid->valid)!
    if (EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
    {
        if (   (EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED != (pRQBParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
            && (EDDI_RTC3_AUX_PROVIDER != LowerParamsLocal.ProviderType))
        {
            if (!EDDI_CRTCheckBufferProperties(pDDB, LowerParamsLocal.ListType, BufferProperties, LSA_TRUE /*bIsProvider*/, LSA_FALSE /*bAllowUndefined*/))
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal DataOffset:0x%X Result:0x%X", DataOffset, Result);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
                return EDD_STS_ERR_PARAM;
            }
            LowerParamsLocal.BufferProperties = pRQBParam->IOParams.BufferProperties;
        }
    }

    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    //Check MRPD BufferProperties:
    //The 3B-IF cannot exchange the buffer-ptrs in multiple FCWs consistently. Therefor only SYNC images for MRPD providers are allowed.
    //A secret special mode is used for tests with PCIOX: it has to set "ASYNC" images, and use only the 2nd buffer of the 3!
    if (   (pProvider->bRedundantFrameID)
        && (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC != (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)))
    {
        #if defined (EDDI_CFG_ENABLE_MRPD_ASYNC_PROVIDER)         
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,"EDDI_CRTProviderControlEventModeActivateCheckAndSet, BufferProperties:0x%X of MRPD-Provider 0x%X with FrameID 0x%X is not SYNC.", 
            LowerParamsLocal.BufferProperties, pProvider->ProviderId, LowerParamsLocal.FrameId);
        #else
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderControlEventModeActivateCheckAndSet, BufferProperties:0x%X of MRPD-Provider 0x%X with FrameID 0x%X is not SYNC.", 
            LowerParamsLocal.BufferProperties, pProvider->ProviderId, LowerParamsLocal.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
        return EDD_STS_ERR_PARAM;
        #endif
    }
    #endif

    //Set CyclePhase if is undefined
    if (   (EDD_CYCLE_PHASE_UNDEFINED == LowerParamsLocal.CyclePhase) 
        && (   (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) 
            || (Mode & EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS)) )
    {
        LowerParamsLocal.CyclePhase = pRQBParam->CyclePhase;
    }
    
    //Set CyclePhaseSequence 
    if (   (EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == LowerParamsLocal.CyclePhaseSequence) 
        && (   (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) 
            || (Mode & EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS)) )
    {
        LowerParamsLocal.CyclePhaseSequence = pRQBParam->CyclePhaseSequence;
    }
    
    //Set CycleReductionRatio
    if (   (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == LowerParamsLocal.CycleReductionRatio)
        && (   (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) 
            || (Mode & EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS )) )
    {
        //set CRR for "normal" provider (RTC1/2/3)
        LowerParamsLocal.CycleReductionRatio = pRQBParam->CycleReductionRatio;
    }
   
    //Set values for AUX provider
    if (   (pProvider->IRTtopCtrl.pAscProvider) 
        && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) ) 
    {
        if (EDD_CYCLE_PHASE_UNDEFINED == CyclePhaseAUX)
        {
            CyclePhaseAUX = pRQBParam->CyclePhase;    
        }
        if (EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == CyclePhaseSequenceAUX)
        {
            CyclePhaseSequenceAUX = pRQBParam->CyclePhaseSequence;    
        }
        if (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == CycleReductionRatioAUX)
        { 
            //check if the reduction ratio of the class3-provider is binary
            if (EDDI_RedIsBinaryValue((LSA_UINT32)pRQBParam->CycleReductionRatio * (LSA_UINT32)pDDB->CycCount.Entity))
            {
                CycleReductionRatioAUX = EDDI_AUX_PROVIDER_RR;   //fixed RR
            }
            else
            {
                CycleReductionRatioAUX = pRQBParam->CycleReductionRatio;   //same RR as class3-provider
            }
        }

        //check cycle parameters for AUX provider
        if (   (EDD_CYCLE_PHASE_UNDEFINED != CyclePhaseAUX)
            && (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != CycleReductionRatioAUX)
            && (CyclePhaseAUX > CycleReductionRatioAUX) )
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderControlEventModeActivateCheckAndSet (AUX), illegal CyclePhase (0x%X)/ReductionRatio (0x%X)", 
                              CyclePhaseAUX, CycleReductionRatioAUX);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CycleReductionRatio);
            return EDD_STS_ERR_PARAM;
        }
    }

    //check cycle parameters
    if (   (EDD_CYCLE_PHASE_UNDEFINED != LowerParamsLocal.CyclePhase)
        && (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != LowerParamsLocal.CycleReductionRatio)
        && (LowerParamsLocal.CyclePhase > LowerParamsLocal.CycleReductionRatio) )
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal CyclePhase (0x%X)/ReductionRatio (0x%X)", 
                          LowerParamsLocal.CyclePhase, LowerParamsLocal.CycleReductionRatio);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CycleReductionRatio);
        return EDD_STS_ERR_PARAM;
    }

    //check dataoffset
    if (   (   (EDD_DATAOFFSET_UNDEFINED == LowerParamsLocal.DataOffset)
            || (EDD_DATAOFFSET_INVALID == LowerParamsLocal.DataOffset))    
        && (EDDI_RTC3_AUX_PROVIDER   != LowerParamsLocal.ProviderType))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal DataOffset:0x%X Result:0x%X", DataOffset, Result);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }
    
    if (   ((LowerParamsLocal.DataOffset + LowerParamsLocal.DataLen) > pDDB->ProcessImage.offset_ProcessImageEnd )
        && !EDDI_IS_DFP_PROV(pProvider) )
    {
        #ifdef EDDI_PRAGMA_MESSAGE
        #pragma message ("MultiEDDI")
        #endif /* EDDI_PRAGMA_MESSAGE */
        #if defined(EDDI_CFG_REV7)
        if (   !pProvider->usingPAEA_Ram
            || (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)) )
        #endif
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, (DataOffset(0x%X)+DataLen(0x%X)) exceeds offset_ProcessImageEnd(0x%X)", 
                              LowerParamsLocal.DataOffset, LowerParamsLocal.DataLen, pDDB->ProcessImage.offset_ProcessImageEnd);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
            return EDD_STS_ERR_PARAM;
        }
    }

    //check properties
    if (   (EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
        && (EDDI_RTC3_AUX_PROVIDER != LowerParamsLocal.ProviderType))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal BufferProperties:0x%X", BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
        return EDD_STS_ERR_PARAM;
    }
                  
    //if provider-as shall be activated, check this first (check only!)
    if ((!(( EDD_CONSUMER_ID_REMOVE_AS == pRQBParam->AutoStopConsumerID ) 
        || ( EDDI_CONSUMER_ID_INVALID == pRQBParam->AutoStopConsumerID ))) 
        && (Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP)
        && bUseAutoStop)
    {
        Result =  EDDI_CRTProviderASAdd(pDDB, pProvider, pRQBParam, LSA_TRUE /*bCheckOnly*/);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, ERROR call function: EDDI_CRTProviderASAdd, Result:0x%X", Result);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
            return EDD_STS_ERR_PARAM;    
        }      
    }

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        //Check an Set SFOffset, if necessary
        if (EDD_SFOFFSET_UNDEFINED == LowerParamsLocal.SFOffset)
        {
            LowerParamsLocal.SFOffset = pRQBParam->IOParams.SFOffset;        
        }
        
        if (   (EDD_SFOFFSET_UNDEFINED == LowerParamsLocal.SFOffset)
            || (EDD_SFOFFSET_INVALID   == LowerParamsLocal.SFOffset) )
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal DFP SFOffset:0x%X", LowerParamsLocal.SFOffset);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
            return EDD_STS_ERR_PARAM;     
        }
        
        //Check an Set SFPosition, if necessary
        if (EDD_SFPOSITION_UNDEFINED == LowerParamsLocal.SFPosition)
        {
            LowerParamsLocal.SFPosition = pRQBParam->IOParams.SFPosition;        
        }
        
        if (!EDDI_CRT_IS_DFP_PROV_SFPOSITION_IN_VALID_RANGE(pProvider))
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderControlEventModeActivateCheckAndSet, illegal DFP SFPosition:0x%X", LowerParamsLocal.SFPosition);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
            return EDD_STS_ERR_PARAM;     
        }  
    }

    //check DFP buffer properties and phase
    //the first activated DFP consumer determines the bufferproperties of the PF (in EDDI_DFPConsActPFFrame)  
    if (EDD_STS_OK != EDDI_DFPProviderCheckBufferProperties(pDDB, pProvider, &LowerParamsLocal))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderControlEvent, illegal DFP BufferProperties:0x%X", BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
        return EDD_STS_ERR_PARAM;
    }

    //Update LowerParams
    pProvider->LowerParams = LowerParamsLocal;
    #ifdef EDDI_PRAGMA_MESSAGE
    #pragma message ("MultiEDDI")
    #endif /* EDDI_PRAGMA_MESSAGE */
    #if defined(EDDI_CFG_REV7)
    if (pProvider->usingPAEA_Ram)
    {
        //using_PAEA_Ram and Bufferproperties of AUX will be set in EDDI_CRTProviderControlEventModeActivate
        if (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
        {
            //Bufferproperties changed from UNDEFINED/ASYNC to SYNC
            //==> change APDUStatus storage position, dellocation of statebuffer is not necessary for providers
            //DataStatusOffset should still be UNDEFINED
            pProvider->usingPAEA_Ram = LSA_FALSE;
        }
        else if (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (LowerParamsLocal.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
        {
            if (!EDDI_IS_DFP_PROV(pProvider))
            {
                //DataStatusOffset can be calculated now
                EDDI_CRT_PAEA_APDU_STATUS * const pPAEA_APDU = (EDDI_CRT_PAEA_APDU_STATUS *)(LSA_VOID *)pProvider->LowerParams.pKRAMDataBuffer;
                pProvider->LowerParams.DataStatusOffset = (LSA_UINT32)&pPAEA_APDU->APDU_Status.Detail.DataStatus - (LSA_UINT32)pDDB->pKRam;
            }
        }
    }
    #endif //(EDDI_CFG_REV7)

    //update max CycleReduction
    if (pCRTComp->ProviderList.MaxUsedReduction < LowerParamsLocal.CycleReductionRatio)
    {
        pCRTComp->ProviderList.MaxUsedReduction = LowerParamsLocal.CycleReductionRatio;
    }

    //set values for AUX-Provider
    if (   pProvider->IRTtopCtrl.pAscProvider
        && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) )
    {
        pProvider->IRTtopCtrl.pAscProvider->LowerParams.CyclePhase          = CyclePhaseAUX;
        pProvider->IRTtopCtrl.pAscProvider->LowerParams.CyclePhaseSequence  = CyclePhaseSequenceAUX;
        pProvider->IRTtopCtrl.pAscProvider->LowerParams.CycleReductionRatio = CycleReductionRatioAUX;
    }

    return Result;
}                                                                                  
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModeActivate()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeActivate( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                  EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                  EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                  LSA_UINT8                                const  GroupDataStatus,
                                                                                  LSA_BOOL                                 const  bUseAutoStop )
{
    LSA_UINT8                                 const  ListType                   = pProvider->LowerParams.ListType;
    LSA_UINT16                                const  Mode                       = pRQBParam->Mode;
    LSA_UINT8                                 const  ProvDataStatus             = pRQBParam->Status;
    LSA_UINT8                                 const  ProvDataStatusMask         = pRQBParam->Mask;
    LSA_RESULT                                       Result                     = EDD_STS_OK;
    LSA_UINT32                                const  BufferProperties           = (EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))?pRQBParam->IOParams.BufferProperties:pProvider->LowerParams.BufferProperties;
    LSA_UINT32                                const  DataOffset                 = (EDD_DATAOFFSET_UNDEFINED == pProvider->LowerParams.DataOffset)?pRQBParam->IOParams.DataOffset:pProvider->LowerParams.DataOffset;
    LSA_UINT32                                       pFCW;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                    pFrmHandler                = (EDDI_IRT_FRM_HANDLER_PTR_TYPE)EDDI_NULL_PTR;

    //***********************************************************
    //Step 1: set activity
    if (EDDI_CRT_PROV_STS_ACTIVE != pProvider->Status)
    {
        //check for change in DataOffset (can only happen once from invalid->valid)!
        //provider gets activated ==> check parameters for dataoffset and BufferProperties
        //if there had been valid parameters given at PROVIDER_ADD, ignore parameters here!
        Result = EDDI_CRTProviderControlEventModeActivateCheckAndSet( pDDB, pProvider, pRQBParam, bUseAutoStop);
        if (EDD_STS_OK != Result)
        {
            //Details have already been set
            return Result;
        }     
     
        //check value of watchdog register
        //in this case the use of watchdog is not allowed
        Result = EDDI_CRTProviderControlEventModeActivateCheckApplWatchdog( pDDB, pProvider );
        if (EDD_STS_OK != Result)
        {
            //Details have already been set
            return Result;
        }  

        //***********************************************************
        //additional checks for activation
        //check CyclePhase, CyclePhaseSequence and CycleReductionRatio
        if (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE)
        {
            Result = EDDI_CRTCheckProviderPhase(pDDB, pProvider);
            if (EDD_STS_OK != Result)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_CRTCheckProviderPhase, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
                return Result;
            } 
        }
        
        if (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX)
        {
            if (LSA_NULL == pProvider->IRTtopCtrl.pAscProvider)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_CRTCheckProviderPhase, no AUX-Provider available, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
                return Result;   
            }
            
            Result = EDDI_CRTCheckProviderPhase(pDDB, pProvider->IRTtopCtrl.pAscProvider);
            if (EDD_STS_OK != Result)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_CRTCheckProviderPhase, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
                return Result;
            }
        }

        //check ListType against FrameID. FrameID now has to be valid!
        if (EDDI_LIST_TYPE_ACW == pProvider->LowerParams.ListType)
        {
            if (EDD_FRAME_ID_UNDEFINED == pRQBParam->FrameID)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, UNDEFINED FrameID not allowed here ProvID:0x%X", pRQBParam->ProviderID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID);
                return EDD_STS_ERR_PARAM;
            }
            else if (EDD_STS_OK != EDDI_CRTCheckListType(pProvider->Properties, pRQBParam->FrameID, pProvider->LowerParams.ListType) )
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, FrameID 0x%X does not match ListType %d for ProvID 0x%X", 
                    pRQBParam->FrameID, pProvider->LowerParams.ListType, pRQBParam->ProviderID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID);
                return EDD_STS_ERR_PARAM;
            }

            pProvider->LowerParams.FrameId = pRQBParam->FrameID;
        }

        //final check of DstMac/DstIP. DstMac/DstIP now has to be valid!
        if (EDD_STS_OK != EDDI_CheckProviderMacAndIpAddr(pDDB, pProvider->Properties, &pRQBParam->DstMAC, &pRQBParam->DstIP, LSA_FALSE /*bUndefAllowed*/) )
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModeActivateCheckAndSet, DstMAC/DstIP check failed for ProvID 0x%X", 
                pRQBParam->ProviderID);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MAC_IP);
            return EDD_STS_ERR_PARAM;
        }
        else
        {
            EDDI_SetProviderMacAndIpAddr(pProvider, pProvider->Properties, &pRQBParam->DstMAC, &pRQBParam->DstIP);
        }

        //**********************************************************
        //Add RTC1/2-Providers to tree
        if (   (   (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX)
                && (EDDI_NULL_PTR == pProvider->IRTtopCtrl.pAscProvider->pLowerCtrlACW))
            || (EDDI_RTC3_PROVIDER != pProvider->LowerParams.ProviderType))
        {                                                          
            //always relate to main provider, function extracts the AUX-provider by itself
            Result = EDDI_SRTProviderAddToTree(pProvider, pDDB, &pRQBParam->IOParams.CWOffset, ListType);
            if(EDD_STS_OK != Result)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, illegal EDDI_SRTProviderAddToTree, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AddToTree);
                return Result;
            }
        }           
       
        //***********************************************************
        //set buffer-data on Class1/2-Providers
        if (   (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
            && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) )
        {
            if(pProvider->IRTtopCtrl.pAscProvider)
            {
                if (LSA_FALSE == pProvider->IRTtopCtrl.pAscProvider->bBufferParamsValid)
                {
                    // AUX buffer is always set, as it cannot be determined wether it is a re-activation
                    pProvider->IRTtopCtrl.pAscProvider->LowerParams.DataOffset       = DataOffset;
                    pProvider->IRTtopCtrl.pAscProvider->LowerParams.BufferProperties = BufferProperties;
                    #ifdef EDDI_PRAGMA_MESSAGE
                    #pragma message ("MultiEDDI")
                    #endif /* EDDI_PRAGMA_MESSAGE */
                    #if defined (EDDI_CFG_REV7)
                    pProvider->IRTtopCtrl.pAscProvider->usingPAEA_Ram                = pProvider->usingPAEA_Ram;
                    #endif
                    EDDI_CRTProviderSetBuffer(pDDB, pProvider->IRTtopCtrl.pAscProvider);
                }
            }
            else
            {
                EDDI_Excp("EDDI_CRTProviderControlEvent, pProvider->IRTtopCtrl.pAscProvider not valid, ProviderId:%i, pProvider ptr: 0x%X", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider);
                return EDD_STS_ERR_EXCP;
            }
        }
        else if (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE)
        {
            if (!EDDI_IS_DFP_PROV(pProvider))
            {
                if (EDDI_LIST_TYPE_FCW == ListType)
                {
                    //check for match with IRData
                    pFrmHandler = EDDI_SyncIrFindFrameHandler(pDDB, pProvider->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
                    if (EDDI_NULL_PTR == pFrmHandler)
                    {
                        //MC-prov cannot be activated without IRDATA being present!
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent no IRDATA found for ProvId:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                        return EDD_STS_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (!EDDI_DFPProviderIsPF(pProvider))
                        {
                            //Not allowed to activate the DFP-PackFrame by user
                            EDDI_DFPCheckFrameHandlerPF( pDDB, pProvider->ProviderId, &pFrmHandler);
                            if (EDDI_NULL_PTR == pFrmHandler)
                            {
                                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent activating a PFFrameID:0x%X for ProvId:0x%X is not allowed", pProvider->LowerParams.FrameId, pProvider->ProviderId);
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                                return EDD_STS_ERR_PARAM;
                            }
                        }

                        //Check if class3 Provider is active
                        if(   (EDDI_PROV_STATE_PASSIVE == pProvider->IRTtopCtrl.ProvState)
                           && (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status )
                           && (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent Prov is active.ProvId:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IIRTtopSM);
                            return EDD_STS_ERR_PARAM; 
                        }
        
                        //Check if IRDATA match to provider parameters
                        Result = EDDI_CRTProviderCheckData(pProvider, pFrmHandler, pDDB, LSA_FALSE /*bCheckBuffOnly*/);
                        if (EDD_STS_OK != Result)
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent, IRDATA found but do not match. ProvID:0x%X FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);

                            //MC-prov cannot be activated without IRDATA being present!
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                            return EDD_STS_ERR_PARAM;
                        }
                    }
                }
                if (!(pProvider->bBufferParamsValid))
                {
                    //rtc3 buffer is only set once (also completes the provider-fcw for usage in 3BIF-SW)
                    EDDI_CRTProviderSetBuffer(pDDB, pProvider);
                    //Mark for first activate
                    pProvider->bBufferParamsValid  = LSA_TRUE;
                }
            }
        }
        
        //***********************************************************
        //Step 2: set DataStatus before activating the provider
        if ((Mode & EDD_PROV_CONTROL_MODE_STATUS) && (EDDI_RTC3_PROVIDER != pProvider->LowerParams.ProviderType))
        {
            //Set new ProvDataStatus by respecting the related GroupDataStatus
            EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, ProvDataStatus, ProvDataStatusMask, GroupDataStatus, LSA_TRUE);
        }
        else if (!EDDI_DFPProviderIsPF(pProvider))
        {
            //Set new ProvDataStatus by respecting the related GroupDataStatus
            EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, 0 /*ProvDataStatus*/, 0 /*ProvDataStatusMask*/, GroupDataStatus, LSA_TRUE);
        }

        switch (ListType)
        {
            //Only for RTClass1/2 
            case EDDI_LIST_TYPE_ACW:
            {
                #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
                if (pProvider->LowerParams.RT_Type != EDDI_RT_TYPE_UDP)
                #endif
                {
                    //set DstMAC/DstIP before activating (ACW is known now)
                    EDDI_SERIniProviderXCWDstMacIP(pDDB, pProvider);

                    //set FrameID in real RTC1/2-Providers before activating
                    EDDI_SERIniProviderACWFrameID(pProvider);
                    EDDI_RedTreeSrtProviderSetActivity(pProvider->pLowerCtrlACW, LSA_TRUE);

                    if (EDDI_CRT_PROV_STS_ACTIVE != pProvider->Status)
                    {
                        pProvider->Status = EDDI_CRT_PROV_STS_ACTIVE;
                        pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12++;
                    }
                }

                break;
            }

            case EDDI_LIST_TYPE_FCW:
            {
                if (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) //Only for Class3-FCW
                {
                    if (EDDI_IS_DFP_PROV(pProvider))
                    {
                        //DFP Provider (DG)
                        Result = EDDI_DFPProviderActivate(pDDB, pProvider);
                        if (EDD_STS_OK != Result)
                        {
                            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, SFHandler does not match., ConsumerID:0x%X", pProvider->ProviderId);
                            //Details have already been set
                            return EDD_STS_ERR_PARAM;
                        }

                        //Step 2: set DataStatus before activating the provider
                        if (Mode & EDD_PROV_CONTROL_MODE_STATUS)
                        {
                            //Set new ProvDataStatus by respecting the related GroupDataStatus
                            EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, ProvDataStatus, ProvDataStatusMask, GroupDataStatus, LSA_TRUE);
                        }
                        else
                        {
                            //Set new ProvDataStatus by respecting the related GroupDataStatus
                            EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, 0 /*ProvDataStatus*/, 0 /*ProvDataStatusMask*/, GroupDataStatus, LSA_TRUE);
                        }
                        //activate class3-prov i.e. transfer the stored DS to DG.DS
                        EDDI_DFPProviderSetActivity(pDDB, pProvider, LSA_TRUE); //Nur den Datastatus vom Surragt entfernen. (DG.DataStatus.7 wechselt von 1 -> 0)
                        
                        pProvider->Status = EDDI_CRT_PROV_STS_ACTIVE;    // no increment of ActiveProvRTCxx (DFP)
            
                        EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_ACTIVATE);
                                                     
                        return Result;
                    }
                    else
                    {
                        //pFrmHandler is valid here (has been checked before)
                        if (EDDI_NULL_PTR == pFrmHandler)
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlEvent, pFrameHandler is 0. ProvID:0x%X, FrameID:0x%X", 
                                pProvider->ProviderId, pProvider->LowerParams.FrameId);
                            EDDI_Excp("EDDI_CRTProviderControlEvent, pFrameHandler is 0.", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->LowerParams.FrameId);
                            return (EDD_STS_ERR_PARAM);
                        }
                        
                        Result = EDDI_CheckAndSetProviderControlMacAddr(pDDB, pProvider, pFrmHandler);
                        if (EDD_STS_OK != Result)
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent, Invalid MAC-Addr. ProvID:0x%X FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MAC_IP);
                            return EDD_STS_ERR_PARAM;
                        }  

                        pProvider->pFrmHandler   = pFrmHandler;
                        pFrmHandler->UsedByUpper = LSA_TRUE;  //mark framehandler as used and initialized
                        
                        //initialize provider-fcw
                        #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
                        if (   (!pProvider->bRedundantFrameID)
                            && (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)) )
                        {
                            LSA_UINT32  length = pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

                            //Preserve 4 Byte-Alignment
                            length = (length + 0x03) & ~0x00000003;

                            #if defined (EDDI_CFG_REV7)
                            if (0 == pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
                            #endif
                            {
                                pProvider->LowerParams.pKRAMDataBuffer = pDDB->pKRam + pProvider->LowerParams.DataOffset;
                            }

                            //the user provided a set of 3 buffers, #0 is USER, #1 is DATA (set to DB0 in FCW), #2 is FREE
                            EDDI_IRTProviderAdd(pDDB,
                                                pFrmHandler,
                                                pProvider->LowerParams.DstMAC,
                                                (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)?(pProvider->LowerParams.pKRAMDataBuffer + length):(EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED,
                                                pProvider->LowerParams.ImageMode,
                                                &pFCW,
                                                pProvider->bXCW_DBInitialized);

                            if (!pProvider->bXCW_DBInitialized)
                            {
                                LSA_UINT32  CWOffset_Local;
                                CWOffset_Local = (LSA_UINT32)pFCW - (LSA_UINT32)pDDB->pKRam;
                                
                                //insert address of FCW into 4B-Buffer
                                if /* User got the pointer to acw already */
                                   (EDDI_IO_XCW_OFFSET_FOLLOWS != *pProvider->pIOCW)
                                {
                                    if  /* ptrs are not equal */
                                        (*pProvider->pIOCW != (CWOffset_Local | EDDI_IO_OFFSET_IS_FCW))
                                    {
                                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlEvent, pIOCW exists. ProvID:0x%X, pIOCW:0x%X CWOffset_Local:0x%X", 
                                                          pProvider->ProviderId, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                                        EDDI_Excp("EDDI_CRTProviderControlEvent, pIOCW exists!", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)pProvider->pIOCW, CWOffset_Local);
                                        return (Result);
                                    }  
                                }
                                else
                                {
                                    *pProvider->pIOCW = (CWOffset_Local | EDDI_IO_OFFSET_IS_FCW);
                                }

                                pProvider->bXCW_DBInitialized = LSA_TRUE;
                            }
                        }
                        else
                        #endif
                        {
                            EDDI_IRTProviderAdd(pDDB, pFrmHandler, pProvider->LowerParams.DstMAC, pProvider->LowerParams.pKRAMDataBuffer, pProvider->LowerParams.ImageMode, &pFCW, LSA_FALSE /*bXCW_DBInitialized*/);

                            if (pProvider->bRedundantFrameID)
                            {
                                if (pFrmHandler->pFrmHandlerRedundant)
                                {
                                    if ((pFrmHandler->FrameId + 1) != (pFrmHandler->pFrmHandlerRedundant->FrameId))
                                    {
                                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent, No x+1 MRPD Frame was found. ProvId:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderMRPD_NoFrameID);
                                        return EDD_STS_ERR_PARAM; 
                                    }
                                
                                    //Providers with FrameIDs x and x+1 share the same framebuffer
                                    EDDI_IRTProviderAdd(pDDB, pFrmHandler->pFrmHandlerRedundant, pProvider->LowerParams.DstMAC, pProvider->LowerParams.pKRAMDataBuffer, pProvider->LowerParams.ImageMode, &pFCW, LSA_FALSE /*bXCW_DBInitialized*/);
                                    pFrmHandler->pFrmHandlerRedundant->UsedByUpper = LSA_TRUE;  //mark framehandler as used and initialized
                                }
                                else
                                {
                                    if (EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType)
                                    {
                                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderControlEvent, No x+1 MRPD Frame was found. ProvId:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderMRPD_NoFrameID);
                                        return EDD_STS_ERR_PARAM; 
                                    }
                                }
                            }
                        }
                        EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_ACTIVATE);    
                    }
                }
                else if (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) //Only for Aux-FCW
                {
                     EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX); 
                }

                //now the aux/class3-manipulating flags are valid (set by SM)
                if (   (pProvider->IRTtopCtrl.ProvActiveMask & EDDI_PROV_ACTIVE_AUX_MAIN)
                    && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX))
                {
                    //set buffer on AUX-prov
                    EDDI_CRTProviderSetBuffer(pDDB, pProvider->IRTtopCtrl.pAscProvider);

                    //Step 2: set DataStatus before activating the provider
                    if (Mode & EDD_PROV_CONTROL_MODE_STATUS)
                    {
                        //Set new ProvDataStatus by respecting the related GroupDataStatus
                        EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, ProvDataStatus, ProvDataStatusMask, GroupDataStatus, LSA_TRUE);
                    }
                    else
                    {
                        //Set new ProvDataStatus by respecting the related GroupDataStatus
                        EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, 0 /*ProvDataStatus*/, 0 /*ProvDataStatusMask*/, GroupDataStatus, LSA_TRUE);
                    }
                    //activate AUX-prov
                    //set DstMAC/DstIP before activating (ACW is known now)
                    EDDI_SERIniProviderXCWDstMacIP(pDDB, pProvider->IRTtopCtrl.pAscProvider);

                    //set FrameID in AUX-Providers before activating
                    EDDI_SERIniProviderACWFrameID(pProvider->IRTtopCtrl.pAscProvider);
                    EDDI_RedTreeSrtProviderSetActivity(pProvider->IRTtopCtrl.pAscProvider->pLowerCtrlACW, LSA_TRUE);

                    if (EDDI_CRT_PROV_STS_ACTIVE != pProvider->IRTtopCtrl.pAscProvider->Status)
                    {
                        pProvider->IRTtopCtrl.pAscProvider->Status = EDDI_CRT_PROV_STS_ACTIVE;
                        pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12++;
                    }
                }

                if (   (pProvider->IRTtopCtrl.ProvActiveMask & EDDI_PROV_ACTIVE_RTC3_MAIN)
                    && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE))
                {
                    //set buffer on class3-prov
                    EDDI_CRTProviderSetBuffer(pDDB, pProvider);
                    //Step 2: set DataStatus before activating the provider
                    if (Mode & EDD_PROV_CONTROL_MODE_STATUS)
                    {
                        //Set new ProvDataStatus by respecting the related GroupDataStatus
                        EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, ProvDataStatus, ProvDataStatusMask, GroupDataStatus, LSA_TRUE);
                    }
                    else if (!EDDI_DFPProviderIsPF(pProvider))
                    {
                        //Set new ProvDataStatus by respecting the related GroupDataStatus
                        EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, 0 /*ProvDataStatus*/, 0 /*ProvDataStatusMask*/, GroupDataStatus, LSA_TRUE);
                    }
                    //DstMAC has already been set in all FCWs by EDDI_IRTProviderAdd
                    //activate class3-prov
                    EDDI_CRTProviderSetActivity(pDDB, pProvider, LSA_TRUE);

                    if (EDDI_CRT_PROV_STS_ACTIVE != pProvider->Status)
                    {
                        pProvider->Status = EDDI_CRT_PROV_STS_ACTIVE;
                        pDDB->pLocal_CRT->ProviderList.ActiveProvRTC3++;
                    }
                }
                break;
            }

            default:
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, pProvider->LowerParams.ListType:0x%X", ListType);
                EDDI_Excp("EDDI_CRTProviderControlEvent, pProvider->LowerParams.ListType", EDDI_FATAL_ERR_EXCP, ListType, 0);
                return EDD_STS_ERR_EXCP;
            }
        }
    } //end else //real activation

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModePassivate()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModePassivate( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                   EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                   EDD_UPPER_RQB_PTR_TYPE                   const  pRQB,
                                                                                   LSA_UINT8                                const  GroupDataStatus,
                                                                                   LSA_BOOL                                 const  bUseAutoStop,
                                                                                   LSA_BOOL                               * const  pIndicate)
{
    LSA_RESULT                                      Result    = EDD_STS_OK;
    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)pRQB->pParam;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pProvider->Locked)
    {
        if (pProvider->pRQB_PendingRemove != EDDI_NULL_PTR)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEventModePassivate, Provider(0x%X) is LOCKED (3B-IF HW). Waiting Service.REQ:0x%X", 
                pProvider->ProviderId, pProvider->pRQB_PendingRemove->Service);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
            return EDD_STS_ERR_SEQUENCE;
        }
        // Service will be confirmed when Provider will be unlocked
        pProvider->pRQB_PendingRemove = pRQB;
        *pIndicate                    = LSA_FALSE;
        return EDD_STS_OK;
    }
    #endif

    if (pProvider->IRTtopCtrl.pAscProvider)
    {
        Result = EDDI_CRTProviderControlEventModePassivateAux( pDDB, pProvider, pRQB, pRQBParam->Mode, pIndicate ); 
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        } 
    }

    if (*pIndicate)
    {
        //neither AUX with REMOVE_ACW nor REMOVE_ACW has been handled before
        //if bUseAutoStop is FALSE, passivate has been requested by consumer/provider-AS
        EDDI_CRTProviderControlPassivate(pProvider, pDDB, (pRQBParam->Mode & EDD_PROV_CONTROL_MODE_REMOVE_ACW)?LSA_TRUE:LSA_FALSE /*bRemoveACW*/, (LSA_FALSE == bUseAutoStop)?LSA_TRUE:LSA_FALSE /*bRemoveReq*/, pIndicate, pRQB);

        if //change requested in ProvDataStatus
            (pRQBParam->Mask && (pRQBParam->Mode & EDD_PROV_CONTROL_MODE_STATUS) )
        {
            //Set new ProvDataStatus by respecting the related GroupDataStatus
            EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, pRQBParam->Status, pRQBParam->Mask, GroupDataStatus, LSA_FALSE);
        } 
    }
    else
    {
        //RTC3-Prov will be passivated in EDDI_CRTProviderRemoveEventFinish
    }
    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModePassivateAux()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModePassivateAux( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                                      EDDI_CRT_PROVIDER_PTR_TYPE                const  pProvider,
                                                                                      EDD_UPPER_RQB_PTR_TYPE                    const  pRQB,
                                                                                      LSA_UINT16                                const  Mode,
                                                                                      LSA_BOOL                                * const  pIndicate)
{
    if (   (EDDI_LIST_TYPE_ACW == pProvider->LowerParams.ListType)
        || (!pProvider->IRTtopCtrl.pAscProvider))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, EDD_PROV_CONTROL_MODE_AUX_PASSIVATE not allowed here. ListType:0x%X", pProvider->LowerParams.ListType);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderType);
        return EDD_STS_ERR_PARAM;
    }    
    else
    {       
        if (EDDI_CRT_PROV_STS_ACTIVE == pProvider->IRTtopCtrl.pAscProvider->Status )
        {
            //passivate AUX-prov
            EDDI_CRTProviderPassivateAUX(pDDB, pProvider);

            if (!(Mode & EDD_PROV_CONTROL_MODE_REMOVE_ACW))            
            {
                //now inform SM
                EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX);
            }
        }    
            
        // Async removing of ACW
        if (Mode & EDD_PROV_CONTROL_MODE_REMOVE_ACW)            
        {
            /* Function can also be called by ProviderRemove, but in this case Mode will be 0! */
            EDDI_CRT_PROVIDER_PTR_TYPE const pAuxProvider = pProvider->IRTtopCtrl.pAscProvider;

            if(EDDI_NULL_PTR != pAuxProvider->pLowerCtrlACW)
            {
                pAuxProvider->bActivateAllowed = LSA_FALSE;     //only action allowed now is "REMOVE"
                pProvider->bActivateAllowed = LSA_FALSE;        
    
                // Save context for later function "EDDI_CRTProviderPassivateEventFinish"
                if (pDDB->CRT.ContextProvRemove.InUse)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderControlEventModePassivateAux (ContextProvRemove.InUse == TRUE). ProvID:0x%X, AUXProvID:0x%X", 
                        pProvider->ProviderId, pAuxProvider->ProviderId);
                    EDDI_Excp("EDDI_CRTProviderControlEventModePassivateAux (ContextProvRemove.InUse == TRUE)", EDDI_FATAL_ERR_EXCP, pAuxProvider->ProviderId, pAuxProvider->ProviderId);
                        return EDD_STS_ERR_EXCP;
                }
                else
                {
                    pDDB->CRT.ContextProvRemove.InUse     = LSA_TRUE;
                    pDDB->CRT.ContextProvRemove.pProvider = pAuxProvider;
                }
            
                EDDI_RedTreeSrtProviderACWRemovePart1(pDDB, pRQB, pAuxProvider->pLowerCtrlACW);
                *pIndicate = LSA_FALSE; // No EDDI_RequestFinish yet !!
            }
            else
            {
                //now inform SM
                EDDI_CRTProviderIRTtopSM(pDDB, pProvider, EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX);
            }
        }
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModeAutoStop()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeAutoStop( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                                  EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                                  EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                                  LSA_BOOL                                 const  bUseAutoStop )
{
    LSA_RESULT  Result = EDD_STS_OK;

    if ((!(( EDD_CONSUMER_ID_REMOVE_AS == pRQBParam->AutoStopConsumerID ) 
        || ( EDDI_CONSUMER_ID_INVALID == pRQBParam->AutoStopConsumerID ))) 
        && bUseAutoStop)
    {
        Result =  EDDI_CRTProviderASAdd(pDDB, pProvider, pRQBParam, LSA_FALSE /*bCheckOnly*/);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, ERROR call function: EDDI_CRTProviderASAdd, Status:0x%X", Result);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
            return EDD_STS_ERR_PARAM;    
        }      
    }
    else if (   (EDD_CONSUMER_ID_REMOVE_AS == pRQBParam->AutoStopConsumerID)
             && (bUseAutoStop))
    {        
        Result = EDDI_CrtASCtrlSingleProviderList(pDDB, pProvider, LSA_TRUE /*RemoveASCoupling*/, LSA_FALSE /*DeactivateProvider*/);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderControlEvent, ERROR call function: EDDI_CrtASAddSingleProviderToEndList, Status:0x%X", Result);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
            return EDD_STS_ERR_PARAM;    
        }      
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEventModeStatus()  //Helper Function
 *
 *  Description: Sets the Status of the specified Provider by respecting the
 *               related GroupDataStatus.
 *               Only DataStatus of KRAM will be set !
 *
 *               Function is only called at Provider-Active-State!
 *
 *  Arguments:   pProvider(THIS)    : The current Provider
 *               ProvDataStatus     : The new provider specific DataStatus
 *               Mask               : Masks the Bits of ProvDataStatus wich will take effect.
 *                                  : Hint: If GroupDataStatus only should take effect simply set Mask to 0,
 *               GroupDataStatus    : The group specific DataStatus wich has to be
 *                                    respected while changing ProviderStatus.
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEventModeStatus( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                               EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                               LSA_UINT8                   const  ProvDataStatus,
                                                                               LSA_UINT8                   const  ProvDataStatusMask,
                                                                               LSA_UINT8                   const  GroupDataStatus,
                                                                               LSA_BOOL                    const  bActivate )
{   
    EDDI_CRT_DATA_APDU_STATUS      *  pAPDU;
    LSA_UINT8                  const  OldLocalDataStatus = pProvider->LocalDataStatus;
    LSA_UINT8                         NewLocalDataStatus;
    LSA_UINT8                         MergedDataStatus;
    EDDI_CRT_PROVIDER_PTR_TYPE        pLocalProvider;

    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderControlEventModeStatus -> pProvider->Status:0x%X", pProvider->Status);

    //********************************************
    //supported combinations at RTC3-AUX-Provider:
    //********************************************
    //service             Mask    Activate
    //------------------------------------
    //ProvDataStatus      0       1
    //ProvDataStatus      1       1
    //ProvDataStatus      1       0
    //GroupDataStatus     0       1

    if //change requested in ProvDataStatus by mask
    (ProvDataStatusMask)
    {
        // First calculate the new local ProviderDataStatus without the GroupDataStatus
        NewLocalDataStatus = EDDI_CRTProviderCalculateDataStatus(OldLocalDataStatus, ProvDataStatusMask, ProvDataStatus);

        if //change requested in LocalDataStatus
        (OldLocalDataStatus != NewLocalDataStatus)
        {
            //update LocalDataStatus of this Provider
            pProvider->LocalDataStatus = NewLocalDataStatus;

            if //this Provider has an associated Provider
            (pProvider->IRTtopCtrl.pAscProvider)
            {
                //update LocalDataStatus of associated Provider too!
                pProvider->IRTtopCtrl.pAscProvider->LocalDataStatus = NewLocalDataStatus;
            }
        }

        //this provider is passive
        if ( !bActivate  && ( 
                (EDDI_RTC3_PROVIDER == pProvider->LowerParams.ProviderType) 
             || (EDDI_RTC3_AUX_PROVIDER == pProvider->LowerParams.ProviderType)))
        {
            if //this Provider has no associated Provider
            (!pProvider->IRTtopCtrl.pAscProvider)
            {
                return;
            }
            else //this Provider has an associated Provider
            {
                if //the associated provider is passive too
                (pProvider->IRTtopCtrl.pAscProvider->Status == EDDI_CRT_PROV_STS_INACTIVE)
                {
                    return;
                }
            }
        }
    }
    else //no change requested in ProvDataStatus by mask
    {
        NewLocalDataStatus = OldLocalDataStatus;
    }

    //Now merge LocalDataStatus with GroupDataStatus
    MergedDataStatus = (LSA_UINT8)(((GroupDataStatus | NewLocalDataStatus) & EDD_CSRT_DSTAT_WIN_MASK) | (GroupDataStatus & NewLocalDataStatus));

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        //For DFP subframes the buffer is always valid, as the packframe has to have been setup prior to manipulating the subframes
        EDDI_CRTDFPProviderSetDataStatus (pDDB, pProvider, MergedDataStatus);
    }
    else if (pProvider->bBufferParamsValid)
    {
        //this Provider has an associated Provider
        if (pProvider->IRTtopCtrl.pAscProvider)
        {
            pLocalProvider = pProvider->IRTtopCtrl.pAscProvider;
        }
        else
        {
            pLocalProvider = pProvider;
        }

        //Finally set the ProviderDataStatus respecting GroupDataStatus.
        //See also: edd_usr.h define EDD_CSRT_DSTAT_WIN_MASK
        //For a Provider we intentionaly use the UserDataBuffer here
        //to guaranty Consistency for SYNC-Buffered mode !!

        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        if (pLocalProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
        {
            pLocalProvider->Udp.pData->pAPDU->Detail.DataStatus = MergedDataStatus;
            return;
        }
        #endif

        if (EDD_CSRT_PROV_PROP_SYSRED & pProvider->Properties)
        {
            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->LowerParams.pKRAMDataBuffer;
            EDDI_ENTER_IO_S_INT(pDDB->Glob.InterfaceID);
            EDDI_CSRT_KEEP_DATASTATUS(MergedDataStatus, pAPDU, (EDD_CSRT_DSTAT_BIT_STATE + EDD_CSRT_DSTAT_BIT_REDUNDANCY));
        }

        //pay attention to sequence: 1st USER-Buffer, 2nd KRAM-Buffer
        #if defined (EDDI_CFG_REV6)
        if (pLocalProvider->LowerParams.ListType == EDDI_LIST_TYPE_ACW)
        {
            #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            {
                //use SW-3B-Modell
                //in eddi_ProviderBufferRequest_XChange, the apdu-status is copied again, but as this is
                //not synchronized with this function, we have to set the apdu-status here
                LSA_UINT32 length = pLocalProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

                //Preserve 4 Byte-Alignment
                length = (length + 0x03) & ~0x00000003;

                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pLocalProvider->LowerParams.pKRAMDataBuffer + length );
                pAPDU->Detail.DataStatus = MergedDataStatus;

                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pLocalProvider->LowerParams.pKRAMDataBuffer + (2*length) );
                pAPDU->Detail.DataStatus = MergedDataStatus;
            }
            #else
            //use 3B-Modell
            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->LowerParams.pKRAMDataBuffer1;
            pAPDU->Detail.DataStatus = MergedDataStatus;

            pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->LowerParams.pKRAMDataBuffer2;
            pAPDU->Detail.DataStatus = MergedDataStatus;
            #endif
        }
        else //EDDI_LIST_TYPE_FCW
        {
            #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            if (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
            {
                //use SW-3B-Modell
                //in edd_ProviderBufferRequest_XChange, the apdu-status is copied again, but as this is
                //not synchronized with this function, we have to set the apdu-status here
                LSA_UINT32 length = pLocalProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

                //Preserve 4 Byte-Alignment
                length = (length + 0x03) & ~0x00000003;

                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pLocalProvider->pUserDataBuffer + length );
                pAPDU->Detail.DataStatus = MergedDataStatus;

                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pLocalProvider->pUserDataBuffer + (2*length) );
                pAPDU->Detail.DataStatus = MergedDataStatus;
            }
            else
            #endif
            {
                //when in 1-buffer mode this points to pLocalProvider->LowerParams.pKRAMDataBuffer
                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->pUserDataBuffer;
                pAPDU->Detail.DataStatus = MergedDataStatus;
            }
        }
        #elif defined (EDDI_CFG_REV7)
        if (pLocalProvider->LowerParams.ListType == EDDI_LIST_TYPE_ACW)
        {
                //If using PAEA-IOC, there are no pDB1/pDB2
        }
        else //EDDI_LIST_TYPE_FCW
        {
            if (!pLocalProvider->usingPAEA_Ram)
            {
                //when in 1-buffer mode this points to pLocalProvider->LowerParams.pKRAMDataBuffer
                //this Provider has an associated Provider
                pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->pUserDataBuffer;
                pAPDU->Detail.DataStatus = MergedDataStatus;
            }
            else
            {
                //If using PAEA-IOC, no usage of pUserDataBuffer
            }
        }
        #else //EDDI_CFG_REV5
        //when in 1-buffer mode this points to pLocalProvider->LowerParams.pKRAMDataBuffer
        pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->pUserDataBuffer;
        pAPDU->Detail.DataStatus = MergedDataStatus;
        #endif

        pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pLocalProvider->LowerParams.pKRAMDataBuffer;
        pAPDU->Detail.DataStatus = MergedDataStatus;
        
        if (EDD_CSRT_PROV_PROP_SYSRED & pProvider->Properties)
        {
            EDDI_EXIT_IO_S_INT(pDDB->Glob.InterfaceID);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderControlEvent()
 *
 *  Description: Changes the ProviderDataStatus by respecting the related
 *               GroupDataStatus and the Activity-Value.
 *
 *  Arguments:   pProvider (THIS):   the current Provider
 *               bActivate:          bActivate == FALSE dominates the DataValid-Bit in ProvDataStatus.
 *               ProvDataStatus:     The wished lokal DataStatus (without influence
 *                                   of GroupDataStatus)
 *               ProvDataStatusMask: Masks the Bits in ProvDataStatus wich shall
 *                                   be changed.
 *               GroupDataStatus:    The DataStatus of the related Group.
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderControlEvent( EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                               EDD_UPPER_RQB_PTR_TYPE                   const  pRQB,
                                                               LSA_UINT8                                const  GroupDataStatus,
                                                               EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                               LSA_BOOL                                 const  bUseAutoStop,
                                                               LSA_BOOL                               * const  pIndicate)
{
    LSA_RESULT                                      Result = EDD_STS_OK;
    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)pRQB->pParam;
    LSA_UINT16                               const  Mode   = pRQBParam->Mode;
                                  
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderControlEvent->pProvider->Status:0x%X Mode:0x%X", pProvider->Status, Mode);
                              
    //ProviderStatus was already checked in EDDI_CRTProviderListGetEntry()!

    //******************************************************************************************************************
    // All buffer-relevant parameters are assumed to be valid beyond this point
    //******************************************************************************************************************
    if (Mode & EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS)
    {
        Result = EDDI_CRTProviderControlEventModeActivateSetOnlyParameters( pDDB, pProvider, pRQBParam, bUseAutoStop );
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        }
    }

    //******************************************************************************************************************
    // ACTIVATING a passive provider
    // PREPARING a passive provider for activation
    //******************************************************************************************************************
    if (   (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE)      //ACTIVATE RTC1/2/3
        || (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX))  //ACTIVATE AUX 
    {       
        Result = EDDI_CRTProviderControlEventModeActivate( pDDB, pProvider, pRQBParam, GroupDataStatus, bUseAutoStop ); 
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        }
    }

    //******************************************************************************************************************
    // PASSIVATE
    //******************************************************************************************************************
    if (Mode & EDD_PROV_CONTROL_MODE_PASSIVATE)  /* passivating requested */
    { 
        Result = EDDI_CRTProviderControlEventModePassivate( pDDB, pProvider, pRQB, GroupDataStatus, bUseAutoStop, pIndicate ); 
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        }
    }

    //******************************************************************************************************************
    // PASSIVATE AUX
    //******************************************************************************************************************
    if (Mode & EDD_PROV_CONTROL_MODE_PASSIVATE_AUX) /* passivating of the AUX-provider requested */
    {
        Result = EDDI_CRTProviderControlEventModePassivateAux( pDDB, pProvider, pRQB, pRQBParam->Mode, pIndicate ); 
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        }  
    }

    //******************************************************************************************************************
    // AutoStop-Management
    //******************************************************************************************************************
    if (Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP)
    {
        Result = EDDI_CRTProviderControlEventModeAutoStop( pDDB, pProvider, pRQBParam, bUseAutoStop); 
        if (EDD_STS_OK != Result ) 
        {
            //Details have already been set
            return Result;
        }      
    }
  
    //******************************************************************************************************************
    // Set Data Status solely
    //******************************************************************************************************************
    if //change requested in ProvDataStatus without activating or passivating
      (EDD_PROV_CONTROL_MODE_STATUS == ( Mode & (EDD_PROV_CONTROL_MODE_PASSIVATE + EDD_PROV_CONTROL_MODE_ACTIVATE + EDD_PROV_CONTROL_MODE_ACTIVATE_AUX + EDD_PROV_CONTROL_MODE_PASSIVATE_AUX + EDD_PROV_CONTROL_MODE_STATUS)) )
    {
        //Set new ProvDataStatus by respecting the related GroupDataStatus
        EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, pRQBParam->Status, pRQBParam->Mask, GroupDataStatus, ((EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status)?LSA_TRUE:LSA_FALSE));    
    }

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderControlEvent<-");
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderGetDataStatus()
 *
 *  Description: Return the DataStatus of the specified pProvider
 *
 *  Arguments:   pProvider(THIS) : the related Provider
 *               pDataStatus(OUT): The DataStatus of the Current Provider
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_SEQUENCE  else.
 */
/*static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderGetDataStatus( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider, LSA_UINT8  *  pDataStatus )
{
    //Check forbidden States
    if (pProvider->Status == EDDI_CRT_PROV_STS_NOTUSED || pProvider->Status == EDDI_CRT_PROV_STS_RESERVED)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    *pDataStatus = *(pProvider->pLocalDataBuffer+SER10_DATA_STATUS_BYTE_OFF);
    return EDD_STS_OK;
} */
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCreateProviderList()
 *
 *  Description: Constructor for the ProviderList
 *               Advice: ProviderList can also be initialized with
 *                       maxProviderCount == 0 and
 *                       maxGroupCount    == 0.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCreateProviderList( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CRT_PROVIDER_LIST_PTR_TYPE  const  pProvList,
                                                           LSA_UINT32                       const  maxAcwProviderCount,
                                                           LSA_UINT32                       const  maxFcwProviderCount,
                                                           LSA_UINT16                       const  maxGroupCount )
{
    LSA_UINT32                     Ctr;
    LSA_UINT32                     alloc_size;
    EDDI_CRT_PROVIDER_TYPE      *  pProvider;
    LSA_UINT32              const  maxProviderCount = maxAcwProviderCount + maxFcwProviderCount
    #if defined (EDDI_CFG_DFP_ON)
    + maxFcwProviderCount;
    #else
    ;
    #endif

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCreateProviderList -> maxAcwProviderCount, maxFcwProviderCount:0x%X 0:0x%X",
                      maxAcwProviderCount, maxFcwProviderCount);

    if (   ((maxProviderCount == 0) && (maxGroupCount))
        || ((maxProviderCount)      && (maxGroupCount == 0)))
    {
        EDDI_Excp("EDDI_CRTCreateProviderList ERROR Inconsistent Number of maxProviderCount and maxGroupCount", EDDI_FATAL_ERR_EXCP, maxProviderCount, maxGroupCount);
        return;
    }

    if (maxGroupCount > EDD_CFG_CSRT_MAX_PROVIDER_GROUP)
    {
        EDDI_Excp("EDDI_CRTCreateProviderList ERROR MaxGroups > EDD_CFG_CSRT_MAX_PROVIDER_GROUP", EDDI_FATAL_ERR_EXCP, maxGroupCount, EDD_CFG_CSRT_MAX_PROVIDER_GROUP);
        return;
    }

    if (maxProviderCount > (0xFFFF / sizeof(EDDI_CRT_PROVIDER_PTR_TYPE)))
    {
        EDDI_Excp("EDDI_CRTCreateProviderList ERROR maxProviderCount is too big!", EDDI_FATAL_ERR_EXCP, maxProviderCount, 0);
        return;
    }

    pProvList->MaxEntries      = maxProviderCount;
    pProvList->UsedEntries     = 0;
    pProvList->UsedACWs        = 0;
    pProvList->MaxGroups       = maxGroupCount;

    pProvList->ActiveProvRTC12 = 0;
    pProvList->ActiveProvRTC3  = 0;

    pProvList->LastIndexRTC12  = pDDB->CRT.MetaInfo.RTC12IDStartProv;
    pProvList->LastIndexRTC3   = pDDB->CRT.MetaInfo.RTC3IDStartProv;
    pProvList->LastIndexDFP    = pDDB->CRT.MetaInfo.DFPIDStartProv;

    #if defined (EDDI_CFG_SYSRED_2PROC)
        pDDB->CRT.ProviderList.LastSetDS_ChangeCount                 = 0xFFFFFFFF;
        pDDB->CRT.ProviderList.MinSysRedProviderID                   = 0xFFFF;
        pDDB->CRT.ProviderList.MaxSysRedProviderID                   = 0;
        pDDB->CRT.ProviderList.SysRedPoll.bUsed                      = LSA_FALSE;
        pDDB->CRT.ProviderList.SysRedPoll.Para                       = (LSA_UINT32) NULL;
        pDDB->CRT.ProviderList.SysRedPoll.IntRQB.internal_context    = &pDDB->CRT.ProviderList.SysRedPoll;
        pDDB->CRT.ProviderList.SysRedPoll.IntRQB.internal_context_1  = (LSA_UINT32)pDDB;
        pDDB->CRT.ProviderList.SysRedPoll.IntRQB.pParam              = (LSA_UINT32) 0;
        EDD_RQB_SET_OPCODE( &pDDB->CRT.ProviderList.SysRedPoll.IntRQB, EDD_OPC_REQUEST);
        EDD_RQB_SET_SERVICE(&pDDB->CRT.ProviderList.SysRedPoll.IntRQB, EDDI_SRV_EV_SYSRED_POLL);
    #endif

    if (maxProviderCount)
    {
        //Allocate Provider-Pointer-Array
        alloc_size = maxProviderCount * sizeof(EDDI_CRT_PROVIDER_TYPE);
        EDDI_AllocLocalMem((void**)&pProvider, alloc_size);
        if (EDDI_NULL_PTR == pProvider)
        {
            EDDI_Excp("EDDI_CRTCreateProviderList ERROR while allocating Provider-Array", EDDI_FATAL_ERR_EXCP,  0, 0);
            return;
        }

        pProvList->pEntry = pProvider;

        //reset all ACW-Provider-Entries
        for (Ctr = pDDB->CRT.MetaInfo.RTC12IDStartProv; Ctr < pDDB->CRT.MetaInfo.RTC12IDEndProv; Ctr++)
        {
            pProvider->Status               = EDDI_CRT_PROV_STS_NOTUSED;
            pProvider->ProviderId           = (LSA_UINT16)Ctr;
            pProvider->LowerParams.ListType = EDDI_LIST_TYPE_ACW;
            #if defined (EDDI_CFG_SYSRED_2PROC)
            pProvider->LatestDataStatusOrder_LE   = 0xFFFFFFFF;
            #endif
            pProvider++;
        }

        //reset all FCW-Provider-Entries
        for (Ctr = pDDB->CRT.MetaInfo.RTC3IDStartProv; Ctr < pDDB->CRT.MetaInfo.RTC3IDEndProv; Ctr++)
        {
            pProvider->Status               = EDDI_CRT_PROV_STS_NOTUSED;
            pProvider->ProviderId           = (LSA_UINT16)Ctr;
            pProvider->LowerParams.ListType = EDDI_LIST_TYPE_FCW;
            #if defined (EDDI_CFG_SYSRED_2PROC)
            pProvider->LatestDataStatusOrder_LE   = 0xFFFFFFFF;
            #endif
            pProvider++;
        }

        //reset all DFP-Provider-Entries
        for (Ctr = pDDB->CRT.MetaInfo.DFPIDStartProv; Ctr < pDDB->CRT.MetaInfo.DFPIDEndProv; Ctr++)
        {
            pProvider->Status               = EDDI_CRT_PROV_STS_NOTUSED;
            pProvider->ProviderId           = (LSA_UINT16)Ctr;
            pProvider->LowerParams.ListType = EDDI_LIST_TYPE_FCW;
            #if defined (EDDI_CFG_SYSRED_2PROC)
            pProvider->LatestDataStatusOrder_LE   = 0xFFFFFFFF;
            #endif
            pProvider++;
        }

        //init GroupDataStatus
        for (Ctr = 0; Ctr < maxGroupCount; Ctr++)
        {
            pProvList->GroupDataStatus[Ctr] = EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTDeleteProviderList()
 *
 *  Description: Destructor for the ProviderList
 *
 *  Arguments:   pProvList: reference to listObject to be deleted
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDeleteProviderList( EDDI_CRT_PROVIDER_LIST_PTR_TYPE  const  pProvList )
{
    LSA_UINT16  u16Status;

    if (!(pProvList->pEntry == EDDI_NULL_PTR))
    {
        EDDI_FREE_LOCAL_MEM(&u16Status, pProvList->pEntry);

        if (u16Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_CRTDeleteProviderList", EDDI_FATAL_ERR_EXCP, u16Status, pProvList->MaxEntries);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListAddEntry()
 *
 *  Description: Adds a new Provider to the List and calls the CRTProviderAdd-Function
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_LIMIT_REACHED else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListAddEntry( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT                                   Status, Status2;
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam   = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;
    LSA_UINT8                                    ListType;
    LSA_UINT8                                    ProviderType;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE      const  pCRTComp     = pDDB->pLocal_CRT;
    EDDI_CRT_PROVIDER_PTR_TYPE                   pProvider;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderListAddEntry->");

    ListType     = EDDI_CRTGetListType(pProvParam->Properties, pProvParam->FrameID);
    ProviderType = EDDI_CRTGetProviderType(pDDB, pRQB, ListType);

    if (ProviderType == EDDI_RT_PROVIDER_TYPE_INVALID)
    {
        return EDD_STS_ERR_PARAM;
    }
    else if (ProviderType == EDDI_UDP_PROVIDER)
    {
        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        return (EDDI_RtoProviderListAddEntry (pRQB, pDDB));
        #elif !defined (EDDI_CFG_REV7)
        return (EDD_STS_ERR_PARAM);
        #endif
    }

    Status = EDDI_CRTProviderAddCheckRQB(pRQB, pDDB, ListType, ProviderType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    //Search for a free ProviderEntry in ProviderList
    //Here a provider-entry is found for a class1/2/3-provider, but not for the AUX-provider
    Status = EDDI_CRTProviderListReserveFreeEntry(pDDB, &pProvider, pCRTComp, ProviderType, EDDI_IS_DFP_PROV(pProvParam)?LSA_TRUE:LSA_FALSE /*bIsDfp*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListAddEntry, ERROR finding free Entry, Status:0x%X", Status);
        return Status;
    }
    
    Status = EDDI_CRTProviderAddEvent(pProvider, pRQB, pDDB, ListType, ProviderType);
    if (Status != EDD_STS_OK)
    {
        LSA_BOOL  pIndicate = LSA_FALSE;
        
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListAddEntry, ERROR adding CRTProvider, Status:0x%X", Status);

        //check if already an associated AUX-provider exists
        if (pProvider->IRTtopCtrl.pAscProvider)
        {
            EDDI_CRTProviderListUnReserveEntry(pProvider->IRTtopCtrl.pAscProvider, pCRTComp, LSA_FALSE /*bIsDFP*/);
        }
        #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        //dealloc reference to 4B-Buffer for 3BSW-IF
        else if (pProvider->pIOCW)
        {
            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.IOCWRef, pProvider->pIOCW);
        }
        #endif

        //checks if the conditions for removing a packframe are met
        Status2 = EDDI_DFPProvRemovePFHandler(pRQB, pProvider, pDDB, &pIndicate);
        if (Status2 != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                              "EDDI_CRTProviderListAddEntry, ERROR EDDI_DFPProvRemovePFHandler, Status:0x%X ProviderId:0x%X", 
                              Status2, pProvider->ProviderId);
            EDDI_Excp("EDDI_CRTProviderListAddEntry, ERROR EDDI_DFPProvRemovePFHandler",
                      EDDI_FATAL_ERR_EXCP, Status2, pProvider->ProviderId);
        }  
        
        //Statebuffer does not need to be deallocated (no real "allocation" for providers)
                
        EDDI_CRTProviderListUnReserveEntry(pProvider, pCRTComp, EDDI_IS_DFP_PROV(pProvParam)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);
        return Status;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListRemoveEntry()
 *
 *  Description: Removes a Provider to the List and calls the CRTProviderRemove-Function
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_RESOURCE else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListRemoveEntry( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                  LSA_BOOL                 *  const  pIndicate )
{
    EDDI_CRT_PROVIDER_LIST_TYPE              *  const  pProvList = &pDDB->pLocal_CRT->ProviderList;
    LSA_RESULT                                         Status;
    EDDI_CRT_PROVIDER_PTR_TYPE                         pProvider;
    EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE     const  pRemParam = (EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE)pRQB->pParam;

    // get free ProviderID

    *pIndicate = LSA_TRUE;

    Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, pProvList, pRemParam->ProviderID);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListRemoveEntry, EDDI_CRTProviderListGetEntry, pRemParam->ProviderID:0x%X", pRemParam->ProviderID);
        return Status;
    }

    #if defined (EDDI_CFG_RTC3REMOVE_WITHOUT_PASSIVATING)
    //RTC3-AUX providers cannot be automatically passivated before being removed (indicates SM error in CM!)
    if (pProvider->IRTtopCtrl.pAscProvider)
    #else    
    //RTC3 providers cannot be removed without being passivated
    if (EDDI_PROV_STATE_RED_ACTIVE == pProvider->IRTtopCtrl.ProvState)
    #endif
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListRemoveEntry, pProvider is active. ERROR getting Entry, pRemParam->pProvider:0x%X", pRemParam->ProviderID);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderIsActive);
        return EDD_STS_ERR_SEQUENCE;     
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pProvider->Locked)
    {
        if (pProvider->pRQB_PendingRemove != EDDI_NULL_PTR)
        {
            return EDD_STS_ERR_SEQUENCE;
        }
        // Service will be confirmed when Provider will be unlocked
        pProvider->pRQB_PendingRemove = pRQB;
        *pIndicate                    = LSA_FALSE;
        return EDD_STS_OK;
    }
    #endif

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        (void)EDDI_RtoProviderRemoveEvent(pProvider, pIndicate);
        return EDD_STS_OK;
    }
    #endif

    Status = EDDI_CRTProviderRemoveEvent(pRQB, pProvider, pDDB, pIndicate);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListRemoveEntry, EDDI_CRTProviderRemoveEvent -> Status:0x%X", Status);
        return Status;
    }

    // Update MaxUsedReduction
    if (pProvList->UsedEntries == 0)
    {
        pProvList->MaxUsedReduction = 0;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListSetGroupStatus()
 *
 *  Description: Sets all Provider-Data-States of one Provider-Group.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListSetGroupStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                                            Ctr;
    LSA_UINT8                                             GroupDataStatusMask;
    LSA_UINT8                                      const  AllowedMaskBitsForGroups = EDD_CSRT_DSTAT_GLOBAL_MASK_ALL;
    EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE     const  pParam                   = (EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_CRT_PROVIDER_LIST_TYPE                 *  const  pProvList                = &pDDB->pLocal_CRT->ProviderList;
    LSA_UINT8                                      const  OldGroupDataStatus       = pProvList->GroupDataStatus[pParam->GroupID];
    LSA_UINT8                                             NewGroupDataStatus;

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    {
        LSA_RESULT  const  Status = EDDI_RtoProviderListSetGroupStatus(pRQB, pDDB);

        if (Status != EDD_STS_OK)
        {
            return Status;
        }
    }
    #endif

    if (pParam->GroupID >= pProvList->MaxGroups)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListSetGroupStatus ERROR: Parameter pParam->GroupId too big -> pParam->GroupID:0x%X pProvList->MaxGroups:0x%X", pParam->GroupID, pProvList->MaxGroups);
        EDDI_SET_DETAIL_ERR(EDDI_RQB_DDB(pRQB), EDDI_ERR_INV_GroupID);
        return EDD_STS_ERR_PARAM;
    }

    GroupDataStatusMask = pParam->Mask;
    if (GroupDataStatusMask & (~AllowedMaskBitsForGroups))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListSetGroupStatus WARNING: One or more masked Bits don't effect GroupDataStatus. -> Mask:0x%X AllowedMaskBitsForGroups:0x%X", GroupDataStatusMask, AllowedMaskBitsForGroups);
        EDDI_SET_DETAIL_ERR(EDDI_RQB_DDB(pRQB), EDDI_ERR_INV_ProviderDataStatusMask);
        return EDD_STS_ERR_PARAM;
    }

    //calc new GroupDataStatus
    NewGroupDataStatus = EDDI_CRTProviderCalculateDataStatus(OldGroupDataStatus, GroupDataStatusMask, pParam->Status);

    if //change in GroupDataStatus requested?
       (OldGroupDataStatus != NewGroupDataStatus)
    {
        pProvList->GroupDataStatus[pParam->GroupID] = NewGroupDataStatus;

        for (Ctr = 0; Ctr < pProvList->MaxEntries; Ctr++)
        {
            EDDI_CRT_PROVIDER_TYPE  *  const  pProvider = &pProvList->pEntry[Ctr];

            if (   (pProvider->Status  == EDDI_CRT_PROV_STS_ACTIVE)  //do not change!
                && (pProvider->GroupId == pParam->GroupID))
            {
                //set a single ProviderDataStatus by respecting the new GroupDataStatus
                EDDI_CRTProviderControlEventModeStatus(pDDB, pProvider, (LSA_UINT8)0, (LSA_UINT8)0, NewGroupDataStatus, LSA_TRUE);
            }
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListCtrl()
 *
 *  Description: Uses the RQB to set Data-States of one Provider and its Activity
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListCtrl( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                           LSA_BOOL              *  const  pIndicate)
{
    LSA_RESULT                                          Status;
    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE     const  pParam    = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_CRT_PROVIDER_LIST_TYPE               *  const  pProvList = &pDDB->pLocal_CRT->ProviderList;
    EDDI_CRT_PROVIDER_PTR_TYPE                          pProvider;
    LSA_UINT8                                           GroupDataStatus;
    LSA_UINT16                                   const  Mode = pParam->Mode;
        
    *pIndicate = LSA_TRUE;

    //Check EDDProperties
    if (EDD_DATAOFFSET_INVALID == pParam->IOParams.DataOffset)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListCtrl, EDD_DATAOFFSET_INVALID.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    //Check BufferProperties
    if (EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID == (pParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderListCtrl, EDD_BUFFER_PROPERTIES_INVALID.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    //get the Provider
    Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, pProvList, pParam->ProviderID);
    if (EDD_STS_OK != Status)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListCtrl, ERROR: Could not get pProvider. -> pParam->ProviderID:0x%X pProvList->MaxEntries:0x%X", pParam->ProviderID, pProvList->MaxEntries);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
        return EDD_STS_ERR_PARAM;
    }
  
    //CM passivate the AS-Provider again
    if (    (pProvider->Status == EDDI_CRT_PROV_STS_AS_INACTIVE)
         && (!(Mode & EDD_PROV_CONTROL_MODE_PASSIVATE))
       )
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderListCtrl, ERROR: The Provider was 'inactive' from Privider-Autostop. Next allowed operation is only passivate. Mode:0x%X, ProviderID:0x%X",Mode, pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderIsActive);
        return EDD_STS_ERR_SEQUENCE;
    }
    
    if (EDDI_CRT_PROV_STS_AS_INACTIVE == pProvider->Status)
    {
        //Set the Provider-Status back, to Passivate the Provider 
        pProvider->Status = EDDI_CRT_PROV_STS_ACTIVE;  // nothing to do 
    }

    //Check if the user wants to control the DFP-PackFrame
    if (EDDI_DFPProviderIsPF(pProvider))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderListCtrl, Not allowed to control (Mode:0x%X) a DFP-Packframe (ProviderID:0x%X) by user",Mode, pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_APDU_INDIRECTION);
        return EDD_STS_ERR_PARAM;
    }
  
    if (   ( (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) && (Mode & EDD_PROV_CONTROL_MODE_PASSIVATE) )                              //1
        || ( (Mode & EDD_PROV_CONTROL_MODE_PASSIVATE_AUX) && (Mode != EDD_PROV_CONTROL_MODE_PASSIVATE_AUX) )                    //2
        || ( (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) && !(pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) )    //3
        || ( (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE) && (Mode & EDD_PROV_CONTROL_MODE_ACTIVATE_AUX) )                           //4
        )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListCtrl, invalid combination in Mode-field:0x%X", pParam->Mode);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
        return EDD_STS_ERR_PARAM;
    }
       
    //only the allowed APDUStatus-bits can be manipulated
    if ((pParam->Mask | pParam->Status) & (~EDD_CSRT_DSTAT_LOCAL_MASK_ALL))
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListCtrl, Mask/Status contain forbidden bits. ProvID:0x%X Mask:0x%X Status:0x%X", 
            pParam->ProviderID, pParam->Mask, pParam->Status);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
        return EDD_STS_ERR_PARAM;
    }
      
    //get the GroupDataStatus
    GroupDataStatus = pProvList->GroupDataStatus[pProvider->GroupId];
        
    Status = EDDI_CRTProviderControlEvent(pProvider, pRQB, GroupDataStatus, pDDB, LSA_TRUE /*bUseAutoStop*/, pIndicate);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListGetEntry()
 *
 *  Description: returns the Provider from the specified ProviderId.
 *               Remark: Only initialized providers will be returned.
 *
 *  Arguments:   *pProvider(OUT) : return Value
 *               pProvList (THIS): the Pointer of Provider
 *               ProvId          : the Id of the Provider
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListGetEntry( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                               EDDI_CRT_PROVIDER_PTR_TYPE       *  const  ppProvider, //OUT
                                                               EDDI_CRT_PROVIDER_LIST_PTR_TYPE     const  pProvList,
                                                               LSA_UINT16                          const  ProvId )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider = &pProvList->pEntry[ProvId];

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderListGetEntry->");

    if (ProvId >= pProvList->MaxEntries)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderListGetEntry NOTE_LOW: Parameter ProvId not valid. -> ProvId:0x%X", ProvId);
        return EDD_STS_ERR_PARAM;
    }

    if (pProvider->LowerParams.ProviderType == EDDI_RTC3_AUX_PROVIDER)
    {
       EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderListGetEntry NOTE_LOW: EDDI_RTC3_AUX_PROVIDER not allowed. -> ProvId:0x%X", ProvId);
       return EDD_STS_ERR_PARAM;
    }
    
    //Check ProviderStatus
    if (   (pProvider->Status != EDDI_CRT_PROV_STS_INACTIVE)
        && (pProvider->Status != EDDI_CRT_PROV_STS_ACTIVE)
        && (pProvider->Status != EDDI_CRT_PROV_STS_AS_INACTIVE)
       )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderListGetEntry NOTE_LOW: ProvId not in Use. -> ProvId:0x%X", ProvId);
        return EDD_STS_ERR_PARAM;
    }

    *ppProvider = pProvider;

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListChangePhase()
 *
 *  Description: Changes the Phase of a SRT-Provider
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListChangePhase( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                  LSA_BOOL                 *  const  pIndicate )
{
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UNUSED_ARG(pRQB);
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIndicate);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListChangePhase, Not allowed with 3B-IF in SW.");
    return (EDD_STS_ERR_SERVICE);    
    #else

    EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE  const  pParam                = (EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    LSA_UINT16                                     const  NewCyclePhase         = pParam->CyclePhase;
    LSA_UINT32                                     const  NewCyclePhaseSequence = pParam->CyclePhaseSequence;
    LSA_UINT16                                            CycleReductionRatio;
    EDDI_CRT_PROVIDER_PTR_TYPE                            pProvider;
    LSA_RESULT                                            Status;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderListChangePhase->");

    *pIndicate = LSA_TRUE;

    Status = EDDI_CRTProviderListGetEntry(pDDB, &pProvider, &pDDB->CRT.ProviderList, pParam->ProviderID);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    CycleReductionRatio = pProvider->LowerParams.CycleReductionRatio;

    // Only allowed action is "remove"?
    if (!pProvider->bActivateAllowed)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTProviderListChangePhase, after RTF for ProvID 0x%X only REMOVE is allowed", 
                            pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode);
        return EDD_STS_ERR_SEQUENCE;
    }

    // Provider had never been activated before?
    if (   !(pProvider->pLowerCtrlACW) 
        && !(pProvider->pCWStored) )
    {
        if (pProvider->Status != EDDI_CRT_PROV_STS_INACTIVE)
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderListChangePhase, pLowerCtrlACW/pCWStored=0 but status (0x%X) of ProvID/FrameID 0x%X/0x%X not INACTIVE", 
                pProvider->Status, pProvider->ProviderId, pProvider->LowerParams.FrameId);
            EDDI_Excp("EDDI_CRTProviderListChangePhase, pLowerCtrlACW/pCWStored=0 but status not INACTIVE", EDDI_FATAL_ERR_EXCP, pProvider->Status, pProvider->ProviderId);
            return EDD_STS_ERR_EXCP;
        }

        if (   (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != CycleReductionRatio)
            && (NewCyclePhase > CycleReductionRatio) )
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListChangePhase, illegal CyclePhase (0x%X)/ReductionRatio (0x%X)", 
                              NewCyclePhase, CycleReductionRatio);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CycleReductionRatio);
            return EDD_STS_ERR_PARAM;
        }
        else
        {
            pProvider->LowerParams.CyclePhase = NewCyclePhase;  
            pProvider->LowerParams.CyclePhaseSequence = NewCyclePhaseSequence;  
        }
    }
    else
    {
        switch (pProvider->LowerParams.ProviderType)
        {
            case EDDI_RTC1_PROVIDER:
            case EDDI_RTC2_PROVIDER:
            {
                break;
            }
            default:
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListChangePhase, invalid ProviderType; ProviderID, ProviderType, ->:0x%X :0x%X", pParam->ProviderID, pProvider->LowerParams.ProviderType);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderType);
                return EDD_STS_ERR_PARAM;
            }
        }

        #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
        //do not change the following codelines without checking "EDDI_FIXED_ACW_PROVIDER_SEQUENCE_NOT_NECESSARY"!
        if (   (pProvider->LowerParams.CyclePhaseSequence != EDDI_TREE_GET_LAST_OF_SEQ)
            || (NewCyclePhaseSequence                     != EDDI_TREE_GET_LAST_OF_SEQ))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListChangePhase, old or new CyclePhaseSequence not equal EDDI_TREE_GET_LAST_OF_SEQ, old and new CyclePhaseSequence:0x%X,0x%X", pProvider->LowerParams.CyclePhaseSequence, NewCyclePhaseSequence);
            return EDD_STS_ERR_PARAM;
        }
        #endif

        if (CycleReductionRatio <= 1)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListChangePhase, Reduction has only one Phase, CycleReductionRatio:0x%X", CycleReductionRatio);
            return EDD_STS_ERR_PARAM;
        }

        #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
        if (pProvider->LowerParams.CyclePhase == NewCyclePhase)
        #else
        if (   (pProvider->LowerParams.CyclePhase         == NewCyclePhase)
            && (pProvider->LowerParams.CyclePhaseSequence == NewCyclePhaseSequence))
        #endif
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderListChangePhase, No Change needed, NewCyclePhase:0x%X", NewCyclePhase);
            return EDD_STS_OK;
        }

        //Check if there is still enough Bandwith in DestinationPhase
        //allow overplaning for AUX-providers
        if (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType)
        {
            if (EDD_STS_OK != EDDI_CRTCheckClass12Bandwidth(pDDB, pDDB->pLocal_CRT, CycleReductionRatio, NewCyclePhase, (LSA_UINT32)pProvider->LowerParams.DataLen))
            {
                return EDD_STS_ERR_PARAM;
            }
        }

        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
        {
            Status = EDDI_RtoProviderChangePhase(pDDB, pProvider, NewCyclePhase, NewCyclePhaseSequence);
            return Status;
        }
        #endif

        //Changing Phase asynchronously!
        Status = EDDI_RedTreeSrtProviderChPhasePart1(pDDB, pRQB, pProvider, NewCyclePhase, NewCyclePhaseSequence, pIndicate);
    }
    
    return Status;
    #endif //defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*               "private" Helper-Functions of CRTProviderList-"Class"       */
/*===========================================================================*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTProviderCalculateDataStatus()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT8  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderCalculateDataStatus( LSA_UINT8  const  CurrentDataStatus,
                                                                             LSA_UINT8  const  DataStatusMask,
                                                                             LSA_UINT8  const  NewDataStatus )
{
    return (LSA_UINT8)((CurrentDataStatus & ~(DataStatusMask)) | (NewDataStatus & DataStatusMask));
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTProviderCheckData()
 *
 *  Description:Uses the RQB to remove the pProvider and starts the de-queuing
 *              of the HW-substructures.
 *
 *  Arguments:  pProvider (THIS): the related Provider
 *              pRQB:
 *              pDDB:
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_PARAM else.
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderCheckData( EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                    LSA_BOOL                             const  bCheckBuffOnly )
{
    LSA_UINT32  iElem;
    LSA_UINT32  TxPortCnt = 0; 
    LSA_UINT32  UsrPortIndex = 0;  
        
    if (EDDI_SYNC_IRT_PROVIDER != pFrmHandler->HandlerType)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData EDDI_SYNC_IRT_PROVIDER <> pFrmHandler->HandlerType -> pFrmHandler->HandlerType:0x%X", pFrmHandler->HandlerType);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_HandlerType);
        return EDD_STS_ERR_PARAM;
    }

    if (pFrmHandler->UsedByUpper)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData ProvID:0x%X, FrameID:0x%X: FrmHandler->UsedByUpper=TRUE", 
            pProvider->ProviderId, pProvider->LowerParams.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_UsedByUpper);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!(EDDI_NULL_PTR == pProvider->pFrmHandler))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData EDDI_NULL_PTR, pProvider->pFrmHandler");
        EDDI_Excp("EDDI_CRTProviderCheckData, pProvider->pFrmHandler", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    for (iElem = 0; iElem < pDDB->PM.PortMap.PortCnt; iElem++)
    {
        if (!(EDDI_NULL_PTR == pFrmHandler->pIrFrameDataElem[iElem]))
        {
            if (!bCheckBuffOnly)
            {
                #if defined (EDDI_CFG_REV7)
                if //autopadding is used
                (   (pProvider->LowerParams.DataLen < EDDI_IRT_DATALEN_MIN)
                    && (pDDB->NRT.PaddingEnabled))
                {
                    if (   (pFrmHandler->pIrFrameDataElem[iElem]->DataLength < pProvider->LowerParams.DataLen)
                           || (pFrmHandler->pIrFrameDataElem[iElem]->DataLength < EDDI_IRT_DATALEN_MIN))
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData DataLength -> DataLen:0x%X DataLength:0x%X", pProvider->LowerParams.DataLen, pFrmHandler->pIrFrameDataElem[iElem]->DataLength);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDataLen);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                else
                #endif
                {
                    if (pFrmHandler->pIrFrameDataElem[iElem]->DataLength != pProvider->LowerParams.DataLen)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData DataLength -> DataLen:0x%X DataLength:0x%X", pProvider->LowerParams.DataLen, pFrmHandler->pIrFrameDataElem[iElem]->DataLength);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDataLen);
                        return EDD_STS_ERR_PARAM;
                    }
                }

                if (pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio != pProvider->LowerParams.CycleReductionRatio)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData ReductionRatio -> CycleReductionRatio:0x%X ReductionRatio:0x%X", pProvider->LowerParams.CycleReductionRatio, pFrmHandler->pIrFrameDataElem[iElem]->ReductionRatio);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                    return EDD_STS_ERR_PARAM;
                }

                if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pProvider->LowerParams.CycleReductionRatio, 
                                                               pProvider->LowerParams.CyclePhase, (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)?LSA_TRUE:LSA_FALSE /*bCheckPhase*/) )
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                    return EDD_STS_ERR_PARAM;
                }

                if (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
                {
                    if (pFrmHandler->pIrFrameDataElem[iElem]->Phase != pProvider->LowerParams.CyclePhase)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData Phase -> CyclePhase:0x%X Phase:0x%X", pProvider->LowerParams.CyclePhase, pFrmHandler->pIrFrameDataElem[iElem]->Phase);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Phase);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                else
                {
                    //non-legacy providers take their phase directly from the PDIRData. A MC-Prov can have multiple phases, so it is undefined
                    pProvider->LowerParams.CyclePhase = EDD_CYCLE_PHASE_UNDEFINED;
                }
            } //end if (!bCheckBuffOnly)

            if (pFrmHandler->pIrFrameDataElem[iElem]->FrameID != pProvider->LowerParams.FrameId)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckData FrameId -> FrameId:0x%X FrameID:0x%X", pProvider->LowerParams.FrameId, pFrmHandler->pIrFrameDataElem[iElem]->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID);
                return EDD_STS_ERR_PARAM;
            }

            //get TX-port for this provider
            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                // UsrPortIndex ist Absicht -> Vergleich auf User Sicht in pFrmData
                if (EDDI_IRTIsTxPortSet(pFrmHandler->pIrFrameDataElem[iElem], pDDB->PM.PortMap.PortCnt, UsrPortIndex))
                {
                    TxPortCnt++;   
                }
            }
        }
    }

    if (   (0 == TxPortCnt) 
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW) 
        || (   (TxPortCnt > 1)                      /* No MC-CCC allowed...        */
            && (!pProvider->bRedundantFrameID))     /* ... except redundant frames */
    #endif
       ) 
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDetails);
        return EDD_STS_ERR_PARAM;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTProviderSetBuffer()
 *
 *  Description:Set all paramaters in DDB and ACW that are related to buffer-offset
 *              and buffer-length.
 *
 *  Arguments:  pProvider (THIS): the related Provider
 *              pRQB:
 *              pDDB:
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_PARAM else.
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderSetBuffer( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const pProvider)
{
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32  length3b = pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    
    // Preserve 4 Byte-Alignment
    length3b = (length3b + 0x03) & ~0x00000003;
    #endif

    if (pProvider->bBufferParamsValid)
    {
        //Do nothing, because the provider has been set up before
        return;
    }

    //Set 1st KRAM-Databuffer (independent of listtype)
    #if defined (EDDI_CFG_REV7)
    if (!pProvider->usingPAEA_Ram)
    {
        if (EDD_DATAOFFSET_UNDEFINED == pProvider->LowerParams.DataOffset)
        {
            pProvider->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)EDD_DATAOFFSET_UNDEFINED;
        }
        else
        {
            pProvider->LowerParams.pKRAMDataBuffer = pDDB->pKRam + pProvider->LowerParams.DataOffset;
        }
    }
    else
    {
        EDDI_CRT_PAEA_APDU_STATUS  *  const  pPAEA_APDU = (EDDI_CRT_PAEA_APDU_STATUS *)((void *)pProvider->LowerParams.pKRAMDataBuffer);

        // Set user data application pointer to PAEA-RAM (offset only)
        pPAEA_APDU->pPAEA_Data = EDDI_HOST2IRTE32(pProvider->LowerParams.DataOffset);
    }
    #else
    if (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)
    {
        pProvider->LowerParams.pKRAMDataBuffer = pDDB->pKRam + pProvider->LowerParams.DataOffset;
    }
    #endif

    //---- Class1/2 ------
    if (EDDI_LIST_TYPE_ACW == pProvider->LowerParams.ListType)
    {
        //Init Provider-Data
        if (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)
        {
            #if defined (EDDI_CFG_REV6)
            if (pProvider->LowerParams.ProviderType == EDDI_RTC3_AUX_PROVIDER)
            {
                EDDI_CRTProvInit(pProvider, pDDB, pProvider->LowerParams.ProviderType);
            }
            else
            {
                EDDI_CRTProvInit3B(pDDB, pProvider);
            }
            #elif defined (EDDI_CFG_REV7)
            if (!pProvider->usingPAEA_Ram)
            {
                if (EDDI_RTC3_AUX_PROVIDER == pProvider->LowerParams.ProviderType)
                {
                    EDDI_CRTProvInit(pProvider, pDDB, pProvider->LowerParams.ProviderType);
                }
                else
                {
                    EDDI_CRTProvInit(pProvider, pDDB, pProvider->LowerParams.ProviderType);
                }
            }
            else
            {
                //nothing to do, PAEA-APDU was already reset earlier
            }
            #else //EDDI_CFG_REV5
            EDDI_CRTProvInit(pProvider, pDDB, pProvider->LowerParams.ProviderType);
            #endif
        }

        //init ACW
        #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        {
            EDDI_SER10_ACW_SND_TYPE AcwSndLocal;

            //get acw contents
            DEV32_MEMCOPY((LSA_VOID *)&AcwSndLocal, (LSA_VOID *)&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd, sizeof(AcwSndLocal));

            if (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)
            {
                if (EDDI_RTC3_AUX_PROVIDER == pProvider->LowerParams.ProviderType)
                {
                    //3 buffers are not necessary for AUX, use only buffer #0
                    EDDI_SetBitField32(&AcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                                        DEV_kram_adr_to_asic_register((pProvider->LowerParams.pKRAMDataBuffer), pDDB)); /* 21 Bit */
                }
                else
                {
                    if  /* buffer-ptr has not already been initialized by Mode=EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS */
                        (!pProvider->bXCW_DBInitialized)
                    {
                        //the user provided a set of 3 buffers, #0 is USER, #1 is DATA (set to DB0 in ACW), #2 is FREE
                        EDDI_SetBitField32(&AcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                                            DEV_kram_adr_to_asic_register((pProvider->LowerParams.pKRAMDataBuffer + length3b), pDDB)); /* 21 Bit */
                    }
                }
            }
            else
            {
                EDDI_SetBitField32(&AcwSndLocal.Hw34.Value.U32_0, EDDI_SER10_LLx_3B_IF_BIT__pDB1,
                                    EDDI_IO_OFFSET_INVALID); /* 21 Bit */
            }

            EDDI_SetBitField32(&AcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Sync,
                                (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);   /*  1 Bit */

            EDDI_SetBitField32(&AcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Async,
                                (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0);  /*  1 Bit */

            //write back acw contents
            DEV32_MEMCOPY((LSA_VOID *)&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd, (LSA_VOID *)&AcwSndLocal, sizeof(AcwSndLocal));
        }
        #else
        EDDI_SetBitField32(&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                            DEV_kram_adr_to_asic_register(pProvider->LowerParams.pKRAMDataBuffer, pDDB)); /* 21 Bit */

        EDDI_SetBitField32(&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Sync,
                            (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);   /*  1 Bit */

        EDDI_SetBitField32(&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Async,
                            (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0);  /*  1 Bit */
        #ifdef EDDI_PRAGMA_MESSAGE
        #pragma message ("MultiEDDI")
        #endif /* EDDI_PRAGMA_MESSAGE */
        #if defined (EDDI_CFG_REV7)
        //The AUX prov. for a SYNC-RTC3 prov. shall not reside in PAEARam!
        EDDI_SetBitField32(&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__EXT, (pProvider->usingPAEA_Ram)?1:0); /*  1 Bit */
        #endif

        #endif
    }
    //------- Class3 -------
    else if (EDDI_LIST_TYPE_FCW == pProvider->LowerParams.ListType)
    {
        EDDI_CRTSetImageMode(pDDB, pProvider, LSA_TRUE/*bIsProv*/);

        //Init Provider-Data
        if (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)
        {
            EDDI_CRTProvInit(pProvider, pDDB, pProvider->LowerParams.ProviderType);

            #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            if (   (!pProvider->bRedundantFrameID)
                && (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)))
            {
                //Reset APDUStatus of other buffers as well
                EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer + length3b,   pProvider->LocalDataStatus, (LSA_UINT8)0, LSA_FALSE /*bSysRed always FALSE on RTC3 */);
                EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer + 2*length3b, pProvider->LocalDataStatus, (LSA_UINT8)0, LSA_FALSE /*bSysRed always FALSE on RTC3 */);
            }
            #endif
            
            if (!EDDI_CRTCompleteProvFCW(pDDB, pProvider, LSA_FALSE /*bDBPtrOnly*/)) { }
        }
    }

    //reserve frame-memory in mirror (for checking)
    if (   (EDD_DATAOFFSET_UNDEFINED != pProvider->LowerParams.DataOffset)
        && (pProvider->LowerParams.ProviderType != EDDI_RTC3_AUX_PROVIDER))
    {
        #if defined (EDDI_CFG_REV7)
        if (!pProvider->usingPAEA_Ram)
        #endif
        {
            EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset, pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_ADD);
        }
    }

    //Mark for first SetBuffer
    pProvider->bBufferParamsValid = LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTProviderCheckDataLenClass12()
 *
 *  Description:Check bufferlength.
 *
 *  Arguments:
 *  Return:     errorcode
 */
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderCheckDataLenClass12( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      LSA_UINT16               const  DataLen,
                                                                      LSA_UINT16               const  CycleReductionRatio,
                                                                      LSA_UINT16               const  CyclePhase,
                                                                      LSA_UINT8                const  ProviderType )
{
    // if Padding is not enabled for all Ports --> check with EDD_CSRT_DATALEN_MIN;
    if (!pDDB->NRT.PaddingEnabled)
    {
        if (DataLen < ((EDDI_UDP_PROVIDER == ProviderType)?EDD_CSRT_UDP_DATALEN_MIN:EDD_CSRT_DATALEN_MIN))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderCheckDataLenClass12, Parameter pProvParam->DataLen too small; DataLen, EDD_CSRT_DATALEN_MIN, ->:0x%X :0x%X",
                              DataLen, EDD_CSRT_DATALEN_MIN );
            return EDDI_ERR_INV_DataLen;
        }
    }
                                                                                                        
    if (DataLen > ((EDDI_UDP_PROVIDER == ProviderType)?EDD_CSRT_UDP_DATALEN_MAX:EDD_CSRT_DATALEN_MAX))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderCheckDataLenClass12, Parameter pProvParam->DataLen too big; DataLen, EDD_CSRT_DATALEN_MAX, ->:0x%X :0x%X", DataLen, EDD_CSRT_DATALEN_MAX);
        return EDDI_ERR_DataLen_too_big;
    }

    // Check if there is still enough Bandwidth to add a new Provider to this Phase
    // allow overplaning for AUX-providers
    if (EDDI_RTC3_AUX_PROVIDER != ProviderType)
    {
        if (EDD_STS_OK != EDDI_CRTCheckClass12Bandwidth(pDDB, pDDB->pLocal_CRT, CycleReductionRatio, CyclePhase, (LSA_UINT32)DataLen))
        {
            return EDDI_ERR_Bandwidth;
        }
    }

    return 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtASRemoveAllProviderFromList()
 *
 *  Description: Remove all Provider from the AutoStop list and passivate each
 *               provider
 *
 *  Arguments:   pConsumer (THIS): the related pConsumer
 *               pRQB:
 *               pDDB:
 *               bRemoveASCoupling:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CrtASCtrlAllProviderList( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                LSA_BOOL                          const  bRemoveASCoupling,
                                                                LSA_BOOL                          const  bDeactivateProvider )
{
    LSA_RESULT                  Status        = EDD_STS_OK;
    EDDI_CRT_PROVIDER_PTR_TYPE  pProviderCurr = EDDI_NULL_PTR;
    EDDI_CRT_PROVIDER_PTR_TYPE  pProviderPrev = EDDI_NULL_PTR;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CrtASCtrlAllProviderList->bRemoveASCoupling:0x%X bDeactivateProvider:0x%X",
                      bRemoveASCoupling, bDeactivateProvider);

    if (pConsumer->pNextAutoStopProv!=EDDI_NULL_PTR)
    {
        //get 1st provider from consumer
        pProviderCurr = pConsumer->pNextAutoStopProv;
       
        while (pProviderCurr != EDDI_NULL_PTR)
        {
            pProviderPrev = pProviderCurr;
            //get next ptr
            pProviderCurr = pProviderCurr->pNextAutoStopProv;

            //Deactivate single provider
            Status = EDDI_CrtASCtrlSingleProviderList(pDDB, pProviderPrev, bRemoveASCoupling, bDeactivateProvider);
            if (Status != EDD_STS_OK)
            {
                break;
            }
            
            // Reset pProvider->pNextAutoStopProv and AutoStopConsumerID 
            if (bRemoveASCoupling)
            {
                pProviderPrev->pNextAutoStopProv  = EDDI_NULL_PTR;
                pProviderPrev->AutoStopConsumerID = EDD_CONSUMER_ID_REMOVE_AS;
            }
        }
    }
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtASRemoveSingleProviderFromList()
 *
 *  Description: Remove the related provider from the AutoStop list and passivate 
 *               provider it.
 *
 *  Arguments:   pProvider (THIS): the related pProvider
 *               pRQB:
 *               pDDB:
 *               bRemove:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CrtASCtrlSingleProviderList( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                   LSA_BOOL                    const  bRemoveASCoupling,
                                                                   LSA_BOOL                    const  bDeactivateProvider )
{
    LSA_RESULT                                    Status = EDD_STS_OK;
    EDDI_CRT_CONSUMER_LIST_TYPE         *  const  pConsList = &pDDB->pLocal_CRT->ConsumerList;
    EDDI_CRT_CONSUMER_PTR_TYPE                    pConsumer;
    EDDI_CRT_PROVIDER_PTR_TYPE                    pProviderCurr = pProvider;
    EDDI_CRT_PROVIDER_PTR_TYPE                    pProviderPrev = pProvider;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CrtASCtrlSingleProviderList->ProvID:0x%X from ConsID:0x%X", pProvider->ProviderId, pProvider->AutoStopConsumerID);

    if (   ( EDD_CONSUMER_ID_REMOVE_AS != pProvider->AutoStopConsumerID )
        && ( EDDI_CONSUMER_ID_INVALID  != pProvider->AutoStopConsumerID ))
    {
        Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, pConsList, pProvider->AutoStopConsumerID);

        if (Status != EDD_STS_OK)                                                                            
        {
            LSA_UNUSED_ARG(pProviderPrev); //satisfy lint!

            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CrtASRemoveSingleProvider, ERROR EDDI_CRTConsumerListGetEntry -> Status:0x%X", Status);
            return Status;
        }
        
        if (pConsumer->pNextAutoStopProv != EDDI_NULL_PTR)
        {
            /*Deactivate Provider*/
            if (bDeactivateProvider)
            {               
                if (pProvider->Status != EDDI_CRT_PROV_STS_AS_INACTIVE)
                {
                    LSA_UINT8                           GroupDataStatus;
                    LSA_BOOL                            Indicate = LSA_TRUE;
                    EDD_RQB_TYPE                        RQBLocal;
                    EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE  RQBParam;

                    GroupDataStatus = pDDB->pLocal_CRT->ProviderList.GroupDataStatus[pProvider->GroupId];
                    RQBParam.ProviderID = pProvider->ProviderId;
                    RQBParam.Mask = 0;
                    RQBParam.Mode = EDD_PROV_CONTROL_MODE_PASSIVATE;
                    RQBParam.Status = 0;

                    RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&RQBParam;
                    RQBLocal.Service = EDD_SRV_SRT_PROVIDER_CONTROL;
                    EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);

                        Status = EDDI_CRTProviderControlEvent(pProvider, &RQBLocal, GroupDataStatus, pDDB, LSA_FALSE, &Indicate);
                    if (Status != EDD_STS_OK)
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CrtASCtrlSingleProviderList, ERROR EDDI_CRTProviderControlEvent, Status:0x%X", Status);
                        return Status;
                    }
                }
            }

            /*Remove the AutoStop coupling*/
            if (bRemoveASCoupling)
            {
                //Do nothing if the consumer has no provider
                if (pConsumer->pNextAutoStopProv!=EDDI_NULL_PTR)
                {
                    //copy ptr to 1st provider
                    pProviderCurr = pConsumer->pNextAutoStopProv;
            
                    if (pProviderCurr->ProviderId == pProvider->ProviderId)  //if only one provider exist
                    {
                        //remove  1st
                        pProviderCurr->AutoStopConsumerID = EDDI_CONSUMER_ID_INVALID;
                        pConsumer->pNextAutoStopProv = pProviderCurr->pNextAutoStopProv;
                    }
                    else
                    {
                        while (pProviderCurr->pNextAutoStopProv != EDDI_NULL_PTR)
                        {
                            pProviderPrev = pProviderCurr;
                            //get next ptr
                            pProviderCurr = pProviderCurr->pNextAutoStopProv;
                            if (pProviderCurr->ProviderId == pProvider->ProviderId)
                            {
                                //copy ptr to the next provider
                                pProviderPrev->pNextAutoStopProv = pProviderCurr->pNextAutoStopProv;
                                pProviderCurr->AutoStopConsumerID = EDDI_CONSUMER_ID_INVALID;
                                pProviderCurr->pNextAutoStopProv = EDDI_NULL_PTR;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CrtASCtrlSingleProviderList (No AutoStop: Consumer ID is 'RemoveAS or Invalid' ) ProvID:0x%X from ConsID:0x%X", pProvider->ProviderId, pProvider->AutoStopConsumerID);
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtASAddSingleProviderToEndList()
 *
 *  Description: Add a Provider to the AutoStop list 
 *
 *  Arguments:   pProvider (THIS): the related pProvider
 *               pConsumer (THIS): the related pConsumer
 *
 */
void  EDDI_LOCAL_FCT_ATTR  EDDI_CrtASAddSingleProviderToEndList( EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                 EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider )
{
    EDDI_CRT_PROVIDER_PTR_TYPE         pLocProvider = EDDI_NULL_PTR;

    if (pConsumer->pNextAutoStopProv == EDDI_NULL_PTR) //if no provider exist
    {
        pConsumer->pNextAutoStopProv = pProvider;
    }
    else                
    {
        pLocProvider = pConsumer->pNextAutoStopProv;

        while (pLocProvider->pNextAutoStopProv != EDDI_NULL_PTR)
        {
            //get ptr from the next provider
            pLocProvider = pLocProvider->pNextAutoStopProv;
        }

        if (pLocProvider->pNextAutoStopProv == EDDI_NULL_PTR) //found last entry
        {
            //copy ptr to last pNextAutoStopProv
            pLocProvider->pNextAutoStopProv = pProvider;
        }
    }

    //set end of provider list
    pProvider->pNextAutoStopProv = EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CrtASIsProviderInList()
 *
 *  Description: Search a Provider in a AutoStop list. If a provider is exist with the 
                 same providerID in the list, then return LSA_TRUE. Otherwise
                 return LSA_FALSE;
 *
 *  Arguments:   pProvider (THIS): the related pProvider
 *               pConsumer (THIS): the related pConsumer
 *
 *  Return:      LSA_TRUE if found the same providerID,
 *               LSA_FALSE else.
 */
LSA_BOOL  EDDI_LOCAL_FCT_ATTR EDDI_CrtASIsProviderInList( EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer,
                                                          EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const  pProvider )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pLocProvider = EDDI_NULL_PTR;

    pLocProvider = pConsumer->pNextAutoStopProv;

    while (pLocProvider != EDDI_NULL_PTR)
    {
        if (pLocProvider->ProviderId == pProvider->ProviderId)
        {
            return LSA_TRUE;
        }
        
        //get ptr from the next provider
        pLocProvider = pLocProvider->pNextAutoStopProv;
    }

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderIRTtopSM()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderIRTtopSM( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                         EDDI_CRT_PROVIDER_PTR_TYPE        const  pProvider, 
                                                         EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE  const  Trigger )
{
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProviderIRTtopSM -> ProvID:0x%X Trigger:0x%X", pProvider->ProviderId, Trigger);

    if (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
    {
        //******************************************
        //Legacy-Mode
        //******************************************
        switch (pProvider->IRTtopCtrl.ProvState)
        {
            //******************************************
            //Passive state: AUX passive, Class3 passive
            case EDDI_PROV_STATE_PASSIVE:
            {
                switch (Trigger)
                {                                 
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        //AUX has been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_PASSIVE to STATE_WF_CLASS3_TX with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_AUX_MAIN;  //user/event-actions affect nothing.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_WF_CLASS3_TX;
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        //do nothing
                        break;
                    }
                    
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }


            //**************************************************************************
            //Active, irdata present, but RTC3PSM not in RUN yet: AUX active, Class3 passive
            case EDDI_PROV_STATE_WF_CLASS3_TX:
            {
                switch (Trigger)
                {
                    //no break;
                    //temporarily_disabled_lint -fallthrough
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        //AUX has been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_WF_CLASS3_TX to STATE_PASSIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_NONE;  //user/event-actions affect nothing.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_PASSIVE;
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    {
                        //leave AUX-provider on, switch Class3-provider on
                        //class3-prov has already been prepared during PROVIDER_CONTROL and has to be switched on only
                        EDDI_CRTProviderSetActivity(pDDB, pProvider, LSA_TRUE /*bActivate*/);

                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_WF_CLASS3_TX to STATE_RED_GREEN_ACTIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_RTC3_MAIN + EDDI_PROV_ACTIVE_AUX_MAIN;  //user/event-actions affect both providers.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_RED_GREEN_ACTIVE;
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }

            //**************************************************************************
            //Active, irdata present, RTC3PSM RUN, legacy mode: AUX active, Class3 active
            case EDDI_PROV_STATE_RED_GREEN_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    {
                        //AUX and Class3-Prov have been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_RED_GREEN_ACTIVE to STATE_PASSIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_NONE;  //user/event-actions affect nothing.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_PASSIVE;
              
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        //AUX has been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_RED_GREEN_ACTIVE to STATE_RED_ACTIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_RTC3_MAIN;  //user/event-actions affect only Class3-Prov.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_RED_ACTIVE;
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }

            //***************************************************************************
            //Active, irdata present, RTC3PSM RUN: Final state: AUX passive, Class3 active
            case EDDI_PROV_STATE_RED_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    {
                        //AUX and Class3-Prov have been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_RED_ACTIVE to STATE_PASSIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_NONE;  //user/event-actions affect nothing.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_PASSIVE;
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }
            default:
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM STATE ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                EDDI_Excp("EDDI_CRTProviderIRTtopSM STATE ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                return;
            }
        }
    }
    else
    {
        //******************************************
        //Non-Legacy-Mode
        //******************************************
        switch (pProvider->IRTtopCtrl.ProvState)
        {
            //******************************************
            //Passive state: AUX passive, Class3 passive
            case EDDI_PROV_STATE_PASSIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    {
                        //Only the Class3-prov will be activated AFTER calling this function
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_PASSIVE to STATE_RED_ACTIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_RTC3_MAIN;    //user/event-actions only affect the AUX-Prov.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_RED_ACTIVE;
                        break;
                    }
  
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        //do nothing
                        break;
                    }

                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }

            //***************************************************************************
            //Active, irdata present, RTC3PSM RUN: Final state: AUX passive, Class3 active
            case EDDI_PROV_STATE_RED_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    {
                        //Class3-Prov has been passivated before by the calling fct!
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "#### IRTTOP ProvSM: Transition from STATE_RED_ACTIVE to STATE_PASSIVE with trigger 0x%X", Trigger);
                        pProvider->IRTtopCtrl.ProvActiveMask = EDDI_PROV_ACTIVE_NONE;  //user/event-actions affect nothing.
                        pProvider->IRTtopCtrl.ProvState      = EDDI_PROV_STATE_PASSIVE;
              
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    {
                        //do nothing
                        break;
                    }
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                        EDDI_Excp("EDDI_CRTProviderIRTtopSM TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                        return;
                    }
                }
                break;
            }

            case EDDI_PROV_STATE_WF_CLASS3_TX:
            case EDDI_PROV_STATE_RED_GREEN_ACTIVE:
            default:
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM STATE ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                EDDI_Excp("EDDI_CRTProviderIRTtopSM STATE ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
                return;
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderIRTtopSM()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:
 */
LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderIRTtopSMStateCheck( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                                    EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider, 
                                                                    EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE  const  Trigger )
{
    LSA_RESULT  Result = EDD_STS_OK;

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderIRTtopSMStateCheck -> ProvID:0x%X Trigger:0x%X", pProvider->ProviderId, Trigger);

    if (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
    {
        //******************************************
        //Legacy-Mode
        //******************************************
        switch (pProvider->IRTtopCtrl.ProvState)
        {
            //******************************************
            //Passive state: AUX passive, Class3 passive
            case EDDI_PROV_STATE_PASSIVE:
            {
                switch (Trigger)
                {                                 
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        //do nothing
                        break;
                    }
                    
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }

            //**************************************************************************
            //Active, irdata present, but RTC3PSM not in RUN yet: AUX active, Class3 passive
            case EDDI_PROV_STATE_WF_CLASS3_TX:
            {
                switch (Trigger)
                {
                    //no break;
                    //temporarily_disabled_lint -fallthrough
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }

            //**************************************************************************
            //Active, irdata present, RTC3PSM RUN, legacy mode: AUX active, Class3 active
            case EDDI_PROV_STATE_RED_GREEN_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }

            //***************************************************************************
            //Active, irdata present, RTC3PSM RUN: Final state: AUX passive, Class3 active
            case EDDI_PROV_STATE_RED_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }
            default:
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTProviderIRTtopSM STATE ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
                EDDI_Excp("EDDI_CRTProviderIRTtopSM STATE ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState);
            }
        }
    }
    else
    {
        //******************************************
        //Non-Legacy-Mode
        //******************************************
        switch (pProvider->IRTtopCtrl.ProvState)
        {
            //******************************************
            //Passive state: AUX passive, Class3 passive
            case EDDI_PROV_STATE_PASSIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    {
                        //do nothing
                        break;
                    }

                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }

            //***************************************************************************
            //Active, irdata present, RTC3PSM RUN: Final state: AUX passive, Class3 active
            case EDDI_PROV_STATE_RED_ACTIVE:
            {
                switch (Trigger)
                {
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE:
                    case EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX:
                    case EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX:
                    {
                        break;
                    }
                    default:
                    {
                       Result = EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }

            case EDDI_PROV_STATE_WF_CLASS3_TX:
            case EDDI_PROV_STATE_RED_GREEN_ACTIVE:
            default:
            {
                Result = EDD_STS_ERR_PARAM;
            }
        }
    }

    if (EDD_STS_OK != Result)
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderIRTtopSMCheckState TRIGGER ERROR -> ProvID:0x%X State:0x%X Trigger:0x%X ", pProvider->ProviderId, pProvider->IRTtopCtrl.ProvState, Trigger);
    }

    LSA_UNUSED_ARG(pDDB);
    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderPassivateAUX()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:
 */
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderPassivateAUX( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                           EDDI_CONST_CRT_PROVIDER_PTR_TYPE const pProvider )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  const  pAscProvider = pProvider->IRTtopCtrl.pAscProvider;

    if (pAscProvider->IRTtopCtrl.pAscProvider == pProvider)
    {
        EDDI_RedTreeSrtProviderSetActivity(pAscProvider->pLowerCtrlACW, LSA_FALSE);
        pAscProvider->PendingIndEvent = 0;

        if (EDDI_CRT_PROV_STS_ACTIVE == pAscProvider->Status)
        {
            if (0 == pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "EDDI_CRTProviderPassivateAUX-ProviderID(0x%X), Counter ProviderList.ActiveProvRTC12 already 0",
                                  pProvider->ProviderId);
                EDDI_Excp("EDDI_CRTProviderPassivateAUX, Counter ProviderList.ActiveProvRTC12 already 0", EDDI_FATAL_ERR_EXCP,pProvider->ProviderId, 0);
                return;
            }
            else
            {
                pDDB->pLocal_CRT->ProviderList.ActiveProvRTC12--;
            }
        }

        pAscProvider->Status = EDDI_CRT_PROV_STS_INACTIVE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderSetActivity()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderSetActivity( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                            EDDI_CONST_CRT_PROVIDER_PTR_TYPE    const  pProvider, 
                                                            LSA_BOOL                            const  bActivate )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler = pProvider->pFrmHandler; // EDDI_SyncIrFindFrameHandler(pProvider->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
    EDDI_TREE_ELEM_PTR_TYPE               pTreeElem;
    LSA_UINT8                             HwPortIndex;

    if (EDDI_NULL_PTR == pFrmHandler)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,"EDDI_CRTProviderSetActivity, EDDI_NULL_PTR == pFrmHandler -> ProvID:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
        EDDI_Excp("EDDI_CRTProviderIRTtopSM, TRIGGER ERROR", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->LowerParams.FrameId);
        return;
    }

    if (!bActivate)
    {
        //passivate all FCWs of this provider
        for (HwPortIndex=0; HwPortIndex<EDDI_MAX_IRTE_PORT_CNT; HwPortIndex++)
        {  
            pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pIRTSndEl[HwPortIndex]);
            if (pTreeElem)
            {
                EDDI_RedTreeIrtSetTxActivity(pTreeElem, bActivate);
            }

            if (pFrmHandler->pFrmHandlerRedundant)
            {
                pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pFrmHandlerRedundant->pIRTSndEl[HwPortIndex]);
                if (pTreeElem)
                {
                    EDDI_RedTreeIrtSetTxActivity(pTreeElem, bActivate);
                }
            }
        }
    }
    else
    {
        //Prov-Sm has to be RED_ACTIVE or RED_GREEN_ACTIVE
        if (pProvider->IRTtopCtrl.ProvActiveMask & EDDI_PROV_ACTIVE_RTC3_MAIN)
        {
            //set activity for all fcws whose RTC3PSM is RUN 
            for (HwPortIndex=0; HwPortIndex<EDDI_MAX_IRTE_PORT_CNT; HwPortIndex++)
            {  
                pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pIRTSndEl[HwPortIndex]);
                EDDI_CRTSetActivityFCW(pDDB, pTreeElem, bActivate, HwPortIndex);

                if (pFrmHandler->pFrmHandlerRedundant)
                {
                    pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pFrmHandlerRedundant->pIRTSndEl[HwPortIndex]);
                    EDDI_CRTSetActivityFCW(pDDB, pTreeElem, bActivate, HwPortIndex);
                }
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTSetActivityForAllFCWs()
 *
 *  Description: Activate/Deactivate all FCWS in RcvFCWs for a given HwPortIndex.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSetActivityFCW( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_TREE_ELEM_PTR_TYPE  const  pTreeElem,
                                                       LSA_BOOL                 const  bActivate,
                                                       LSA_UINT8                const  HwPortIndex )
{
    if (pTreeElem)
    {
        EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex];

        if (   (EDDI_RT_CLASS3_STATE_RXsTX  == pRtClass3_Machine->RtClass3_OutputState)
            || (EDDI_RT_CLASS3_STATE_RXsTXs == pRtClass3_Machine->RtClass3_OutputState))
        {
            EDDI_RedTreeIrtSetTxActivity(pTreeElem, bActivate);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTSetActivityAllFWD()
 *
 *  Description: Activate/Deactivate all Forwarding-Ports in RcvFCWs for a given HwPortIndex.
 *
 *  Arguments:   
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSetActivityAllFWD( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bActivate )
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE             pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pDDB->PRM.PDIRData.pRecordSet_A->FrameHandlerQueue.pFirst;
    LSA_UINT8                                 RcvHwPortIndex; 
    EDDI_TREE_ELEM_PTR_TYPE                   pTreeElem;
    EDDI_SER10_FCW_RCV_FORWARDER_TYPE      *  pFcwRcvForwarder;
    LSA_UINT32                                FWDMask;
    LSA_UINT32                         const  HwPortMask = (1UL<<HwPortIndex);

    if (HwPortIndex >= EDDI_MAX_IRTE_PORT_CNT)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTSetActivityAllFWD ERROR Invalid HwPortIndex -> HwPortIndex:0x%X", HwPortIndex);
        EDDI_Excp("EDDI_CRTSetActivityAllFWD ERROR Invalid HwPortIndex", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
        return;
    }

    //walk through all frameid
    while (pFrmHandler)
    {
        if (   (EDDI_SYNC_IRT_FORWARDER    == pFrmHandler->HandlerType)
            || (EDDI_SYNC_IRT_FWD_CONSUMER == pFrmHandler->HandlerType))
        {
            if /* frame is not the sync-frame (never gets modified) */
               (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) != EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
            {
                //check all RCVPorts
                for (RcvHwPortIndex=0; RcvHwPortIndex<EDDI_MAX_IRTE_PORT_CNT; RcvHwPortIndex++)
                {
                    pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pIRTRcvEl[RcvHwPortIndex]);
                    if (pTreeElem)
                    {
                        //check if this fcw is forwarding to HwPortIndex
                        if (pTreeElem->HwTxPortMask & HwPortMask)
                        {
                            //activate/deactivate forwarding to the given HwPortIndex
                            pFcwRcvForwarder = &pTreeElem->pCW->FcwAcw.FcwRcvForwarder;
                            FWDMask = EDDI_GetBitField32(pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort);
                            if (bActivate)
                            {
                                FWDMask |= HwPortMask;
                            }
                            else
                            {
                                FWDMask &= (~HwPortMask);
                            }
                            EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort, FWDMask);
                        }
                    }
                }
            }
        }
        else if (EDDI_SYNC_IRT_PROVIDER == pFrmHandler->HandlerType)
        {
            pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler->pIRTSndEl[HwPortIndex]);
            //Activate only if FCW(s) is/are used and initialized
            //Passivate always
            if (   (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) != EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
                && (   (pTreeElem && !bActivate)
                    || (pTreeElem && pFrmHandler->UsedByUpper)))
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTSetActivityAllFWD: pTreeElem->pCW 0x%X", (LSA_INT32) pTreeElem->pCW);
                EDDI_RedTreeIrtSetTxActivity(pTreeElem, bActivate);
            }
        }

        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCheckProviderPhase()
 *
 *  Description: Check for valid CyclePhase, CyclePhaseSequence and CycleReductionRatio 
 *
 *  Arguments:   
 *
 *  Return:      
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckProviderPhase( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                             EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const  pProvider )
{
    LSA_RESULT  const  Status   = EDD_STS_OK;
    LSA_UINT8   const  ListType = pProvider->LowerParams.ListType;
    
    //check CyclePhase
    if (   (EDD_CYCLE_PHASE_UNDEFINED == pProvider->LowerParams.CyclePhase)
        && (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType)
        && (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckProviderPhase, illegal CyclePhase:0x%X", pProvider->LowerParams.CyclePhase);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhase);
        return EDD_STS_ERR_PARAM;
    }
    //check CyclePhaseSequence
    else if (   (EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == pProvider->LowerParams.CyclePhaseSequence)
             && (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType)
             && (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckProviderPhase, illegal CyclePhaseSequence:0x%X", pProvider->LowerParams.CyclePhaseSequence);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CyclePhaseSequence);
        return EDD_STS_ERR_PARAM;
    }
    //check CycleReductionRatio
    else if (   (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pProvider->LowerParams.CycleReductionRatio)
             && (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckProviderPhase, illegal CycleReductionRatio:0x%X", pProvider->LowerParams.CycleReductionRatio);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_CycleReductionRatio);
        return EDD_STS_ERR_PARAM;
    }
    else if (EDDI_RTC3_AUX_PROVIDER != pProvider->LowerParams.ProviderType)
    {
        switch (ListType)
        {
            case EDDI_LIST_TYPE_ACW:
            {
                //Pathload arrays are updated when allocating a provider-ACW. Checks checking for 
                //(current load + 1) are only allowed if no ACW had been allocated yet.
                //an allocated ACW also shows that this provider had been checked at the previous activate
                if (EDDI_NULL_PTR == pProvider->pLowerCtrlACW)
                {
                    LSA_UINT32  errorcode;

                    if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB, pDDB->pLocal_CRT->SrtRedTree.MaxReduction, pProvider->LowerParams.CycleReductionRatio, pProvider->LowerParams.CyclePhase, LSA_FALSE /*bUndefAllowed*/))
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckProviderPhase, illegal CycleReductionRatio:0x%X", pProvider->LowerParams.CycleReductionRatio);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                        return EDD_STS_ERR_PARAM;
                    }
                    
                    //Check phase load
                    errorcode = EDDI_CRTProviderCheckDataLenClass12(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.CycleReductionRatio, pProvider->LowerParams.CyclePhase, pProvider->LowerParams.ProviderType);
                    if (0 != errorcode)
                    {
                        EDDI_SET_DETAIL_ERR(pDDB, errorcode);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }
            case EDDI_LIST_TYPE_FCW:
            {
                if (!EDDI_IS_DFP_PROV(pProvider))
                {
                    if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pProvider->LowerParams.CycleReductionRatio, 
                                                               pProvider->LowerParams.CyclePhase, (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)?LSA_TRUE:LSA_FALSE /*bCheckPhase*/))
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTCheckProviderPhase, illegal CycleReductionRatio:0x%X", pProvider->LowerParams.CycleReductionRatio);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                break;
            }
            default:
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckProviderPhase, Invalid List-Type:0x%X", ListType);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
                return EDD_STS_ERR_PARAM;
            }
        }
    }
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCompleteProvFCW()
 *
 *  Description: Complete the buffer-dependant values in all FCWs of a provider 
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCompleteProvFCW( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                  EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                  LSA_BOOL                          const  bDBPtrOnly )
{
    EDDI_SER10_FCW_SND_TYPE           FcwSndLocal;
    EDDI_SER10_FCW_SND_TYPE        *  pFcwSndKRAM = (EDDI_SER10_FCW_SND_TYPE *)EDDI_NULL_PTR;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE     pFrmHandler[2] = {(EDDI_IRT_FRM_HANDLER_TYPE *)0, (EDDI_IRT_FRM_HANDLER_TYPE *)0};
    LSA_UINT32                        UsrPortIndex;
    LSA_UINT8                         i;

    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32  length3b = pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    
    // Preserve 4 Byte-Alignment
    length3b = (length3b + 0x03) & ~0x00000003;
    #endif
        
    //find fcw
    pFrmHandler[0] = EDDI_SyncIrFindFrameHandler(pDDB, pProvider->LowerParams.FrameId, pDDB->PRM.PDIRData.pRecordSet_A);
    if (EDDI_NULL_PTR == pFrmHandler[0])
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CRTCompleteProvFCW EDDI_NULL_PTR == pFrmHandler, ProvID:0x%X, FrameID:0x%X",
            pProvider->ProviderId, pProvider->LowerParams.FrameId);
        return (0);
    }

    //pFrmHandler[0]: Main FCW Frame
    //pFrmHandler[1]: Associated Redunandant FCW Frame
    /*if (EDDI_IS_DFP_PROV(pProvider))
    {
        //MRPD Associated Redunandant (x+1) not allowed with DFP  
        pFrmHandler[1] = (EDDI_IRT_FRM_HANDLER_TYPE *)0;        
    }
    else*/
    {
        pFrmHandler[1] = (pFrmHandler[0]->pFrmHandlerRedundant)?pFrmHandler[0]->pFrmHandlerRedundant:(EDDI_IRT_FRM_HANDLER_TYPE *)0;
    }

    for (i=0; i<2; i++)
    {
        if (pFrmHandler[i])
        {
            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB,UsrPortIndex);

                if (pFrmHandler[i]->pIRTSndEl[HwPortIndex])
                {
                    EDDI_TREE_ELEM_PTR_TYPE  const  pTreeElem = ((EDDI_TREE_ELEM_PTR_TYPE)(void *)pFrmHandler[i]->pIRTSndEl[HwPortIndex]);

                    pFcwSndKRAM = &pTreeElem->pCW->FcwAcw.FcwSnd;

                    //get fcw contents
                    DEV32_MEMCOPY((LSA_VOID *)&FcwSndLocal, (LSA_VOID *)pFcwSndKRAM, sizeof(FcwSndLocal));

                    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
                    if (   (EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))  //async buffer AND ...
                        && (!pProvider->bRedundantFrameID)                                                                                      // ... AND no redundant frame AND ...
                        && (!pProvider->bXCW_DBInitialized) )                                                                                   // ... buffer-ptr has not already been initialized by Mode=EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS
                    {
                        EDDI_SetBitField32(&FcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__pDB0, DEV_kram_adr_to_asic_register(pProvider->LowerParams.pKRAMDataBuffer + length3b, pDDB)); /* 21 Bit */
                    }
                    else if (!pProvider->bXCW_DBInitialized)
                    #endif
                    {
                        EDDI_SetBitField32(&FcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__pDB0,
                                            DEV_kram_adr_to_asic_register(pProvider->LowerParams.pKRAMDataBuffer, pDDB)); /* 21 Bit */
                    }

                    if  /* not only the DB-Ptr shall be set */
                        (!bDBPtrOnly)
                    {
                        #if defined (EDDI_CFG_REV7)
                        // change DataLen from IRData-Value (>=40B) to real value (may also be less than 40B)
                        // plausibility has been already checked in EDDI_CRTProviderCheckData
                        EDDI_SetBitField32(&FcwSndLocal.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, (pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS)));    
                        EDDI_SetBitField32(&FcwSndLocal.Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, (pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS))>>3);    
                        EDDI_SetBitField32(&FcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__EXT, (pProvider->usingPAEA_Ram)?1:0); /*  1 Bit */
                        #endif

                        EDDI_SetBitField32(&FcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Sync, (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);    /*  1 Bit */
                        EDDI_SetBitField32(&FcwSndLocal.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Async, (pProvider->LowerParams.ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0); /*  1 Bit */
                    }
                    //write back fcw contents
                    DEV32_MEMCOPY((LSA_VOID *)pFcwSndKRAM, (LSA_VOID *)&FcwSndLocal,  sizeof(FcwSndLocal));
                }
            }
        }
    }
    //only the ptr to the last found FCW is returned. This will work, because
    //this feature is used only for IIIB_IF in SW, without MRPD!
    return (LSA_UINT32)pFcwSndKRAM;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderASAdd()
 *
 *  Description: check all conditions for adding a provider to a consumers AS-list
 *               and add the provider to this list 
 *
 *  Arguments:   
 *
 *  Return:      
 */
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderASAdd ( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                 EDDI_CRT_PROVIDER_PTR_TYPE               const  pProvider,
                                                                 EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const  pRQBParam,
                                                                 LSA_BOOL                                 const  bCheckOnly )
{
EDDI_CRT_CONSUMER_PTR_TYPE  pConsumer;
LSA_RESULT                  Status;

    //get the Consumer
    Status = EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, &pDDB->pLocal_CRT->ConsumerList, pRQBParam->AutoStopConsumerID);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderASCtrl, ERROR EDDI_CRTConsumerListGetEntry, Status:0x%X", Status);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
        return EDD_STS_ERR_PARAM;
    } 
    else if (   (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_INACTIVE)
             || (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_ACTIVE))
    {
        if ( EDDI_CONSUMER_ID_INVALID != pRQBParam->AutoStopConsumerID)
        {
            if (!EDDI_CrtASIsProviderInList(pConsumer, pProvider))
            {
                if (!bCheckOnly)
                {
                    EDDI_CrtASAddSingleProviderToEndList(pConsumer, pProvider);
                    pProvider->AutoStopConsumerID = pRQBParam->AutoStopConsumerID;
                }
            }
            else if (pProvider->AutoStopConsumerID == pRQBParam->AutoStopConsumerID)
            {
                //Same ConsID ==> No Error
            }
            else
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderASCtrl, Invalid consumer ID:0x%X", pRQBParam->AutoStopConsumerID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
                return EDD_STS_ERR_PARAM; 
            }                                                                     
        }
        else
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderASCtrl, Illegal consumer state:0x%X", pConsumer->pSB->Status);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Unknown_Consumer_State);
            return EDD_STS_ERR_PARAM;    
        }
    }
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTProviderIsInitialized()
 *
 *  Description:Checks if the pProvider is initialized
 *
 *  Arguments:  pProvider   (THIS): the related pProvider
 *
 *  Return:     LSA_TRUE   if pProvider is Active,
 *              LSA_FALSE  else
 */
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderIsInitialized( EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider )
{
    if ((pProvider->Status != EDDI_CRT_PROV_STS_NOTUSED) && (pProvider->Status != EDDI_CRT_PROV_STS_RESERVED))
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderASAdd()
 *
 *  Description: check all conditions for adding a provider to a consumers AS-list
 *               and add the provider to this list 
 *
 *  Arguments:   
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListFindByFrameId( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                  EDDI_CRT_PROVIDER_LIST_PTR_TYPE       const  pProvList,
                                                                  EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE      const  pCRT,
                                                                  EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam,
                                                                  LSA_UINT8                             const  ListType,
                                                                  EDDI_CRT_PROVIDER_PTR_TYPE         *  const  ppRetProvider,
                                                                  LSA_BOOL                              const  bIsDfp )                                                                 
{
    LSA_UINT32  ctr, IdBegin, IdEnd;
    LSA_UINT16  const FrameId = pProvParam->FrameID;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderListFindByFrameId->");

    if (ListType == EDDI_LIST_TYPE_ACW)
    {
        IdBegin = pCRT->MetaInfo.RTC12IDStartProv;
        IdEnd   = pProvList->LastIndexRTC12 + 1;
    }
    else //ListType == EDDI_LIST_TYPE_FCW
    {
        if (bIsDfp)
        {
            IdBegin = pCRT->MetaInfo.DFPIDStartProv;
            IdEnd   = pProvList->LastIndexDFP + 1;
        }
        else
        {
            IdBegin = pCRT->MetaInfo.RTC3IDStartProv;
            IdEnd   = pProvList->LastIndexRTC3 + 1;
        }
    }

    //search for an initialized Provider with the same FrameID with the same ListType
    for (ctr = IdBegin; ctr < IdEnd; ctr++)
    {
        EDDI_CRT_PROVIDER_TYPE  *  const  pProvider = &pProvList->pEntry[ctr];

        if ((FrameId == pProvider->LowerParams.FrameId) && (EDDI_CRTProviderIsInitialized(pProvider)))
        {
            if (!bIsDfp)
            {
                *ppRetProvider = pProvider;
                return;
            }
            else
            {
                //check if SFPosition matches
                if (   (pProvider->LowerParams.SFPosition != EDD_SFPOSITION_UNDEFINED)
                    && (pProvider->LowerParams.SFPosition == pProvParam->IOParams.SFPosition) )
                {
                    *ppRetProvider = pProvider;
                    return;
                }
            }
        }
    }

    *ppRetProvider = EDDI_NULL_PTR;
    LSA_UNUSED_ARG(pDDB);
}                                                                                                                    
/*---------------------- end [subroutine] ---------------------------------*/                                                                  

#if defined (EDDI_CFG_SYSRED_2PROC)

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_SysRed_Provider_MergeDS(EDDI_HANDLE  const  hDDB)
{
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;
    LSA_RESULT               Status;

    Status = EDDI_GetDDB(hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eddi_SysRed_Provider_MergeDS, EDDI_GetDDB() failed with Status: 0x%X", Status);
        EDDI_Excp("eddi_SysRed_Provider_MergeDS, EDDI_GetDDB() failed with Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    pDDB->CRT.ProviderList.LastSetDS_ChangeCount = pDDB->pGSharedRAM->ChangeCount;

    if (pDDB->CRT.ProviderList.MinSysRedProviderID <= pDDB->CRT.ProviderList.MaxSysRedProviderID)
    {
        LSA_UINT16 ctr;
        EDDI_CRT_PROVIDER_LIST_TYPE   const  pProvList = pDDB->CRT.ProviderList;

        for (ctr = pDDB->CRT.ProviderList.MinSysRedProviderID; ctr <= pDDB->CRT.ProviderList.MaxSysRedProviderID; ++ctr)
        {
            EDDI_CRT_PROVIDER_PTR_TYPE    const  pProvider = &pProvList.pEntry[ctr];
            EDDI_SET_DS_ORDER                    DS_Order;

            DS_Order.SetDS_OrderCombined = pDDB->pGSharedRAM->DataStatusShadow[pProvider->ProviderId];

            if (   (pProvider->ProviderId == ctr)
                && (pProvider->Properties               &  EDD_CSRT_PROV_PROP_SYSRED)
                && (pProvider->Status                   != EDDI_CRT_PROV_STS_NOTUSED)
                && (pProvider->LatestDataStatusOrder_LE != DS_Order.SetDS_OrderCombined))
            {
                pProvider->LatestDataStatusOrder_LE = DS_Order.SetDS_OrderCombined;
                EDDI_LE2H_L(DS_Order.SetDS_OrderCombined);
                {
                    EDDI_CRT_DATA_APDU_STATUS * pAPDU            = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pProvider->LowerParams.pKRAMDataBuffer;
                    //LSA_UINT8                 MergedDataStatus = pAPDU->Detail.DataStatus & ~(EDD_CSRT_DSTAT_BIT_STATE+EDD_CSRT_DSTAT_BIT_REDUNDANCY);
                    LSA_UINT8                   MergedDataStatus = pAPDU->Detail.DataStatus & ~(DS_Order.SetDS_Order.Mask);

                    MergedDataStatus |= ((DS_Order.SetDS_Order.Status) & (DS_Order.SetDS_Order.Mask));
                    pAPDU->Detail.DataStatus = MergedDataStatus;

                    DS_Order.SetDS_Order.Status = MergedDataStatus;
                    DS_Order.SetDS_Order.Mask   = 0;

                    EDDI_H2LE_L(DS_Order.SetDS_OrderCombined);
                    pDDB->pGSharedRAM->DataStatusAcknowledge[pProvider->ProviderId] = DS_Order.SetDS_OrderCombined;
                }
            }
        }
    }
}


/***************************************************************************/
/* F u n c t i o n:       EDDI_SysRedPoll()                                */
/*                                                                         */
/* D e s c r i p t i o n: executes EDDI-event within eddi_request() after  */
/*                        context-switch by DO-macro                       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SysRedPoll( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_SHED_OBJ_TYPE  *  const  pShedObj = (EDDI_SHED_OBJ_TYPE *)pRQB->internal_context;
    EDDI_DDB_TYPE       *  const  pDDB     = (EDDI_DDB_TYPE *)     pRQB->internal_context_1;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SysRedPoll->");

    EDDI_ENTER_REST_S();
    //check if device is not closing and does not wait for internal RQBs to stop
    if (!pDDB->Glob.pCloseDevicePendingRQB)
    {
        eddi_SysRed_Provider_MergeDS(pDDB->hDDB);
    }
    pShedObj->bUsed = LSA_FALSE;
    EDDI_EXIT_REST_S();
}
#endif



/*****************************************************************************/
/*  end of file eddi_crt_prov.c                                              */
/*****************************************************************************/
