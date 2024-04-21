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
/*  F i l e               &F: eddi_crt_com.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
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
//#include "eddi_ext.h"

#include "eddi_crt_com.h"
#include "eddi_crt_check.h"
#include "eddi_crt_ext.h"
//#include "eddi_swi_ext.h"
#include "eddi_ser_ext.h"
//#include "eddi_crt_sb_sw_tree.h"

#include "eddi_swi_ptp.h"
#include "eddi_prm_record_pdir_data.h"
#include "eddi_crt_dfp.h"

#if !defined(EDDI_INTCFG_PROV_BUFFER_IF_SINGLE)
#include "eddi_crt_prov3B.h"
#endif

#if defined (EDDI_CFG_REV7)
#include "eddi_pool.h"     //If using PAEA-RAM
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_prov.h"
#endif

#define EDDI_MODULE_ID     M_ID_CRT_COM
#define LTRC_ACT_MODUL_ID  102

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************
 *  Function:    EDDI_CRTProviderListUnReserveEntry()
 *
 *  Description: Searches the next free Entry in the Providerlist.
 *               A Entry is treated as free if its Status == EDDI_CRT_PROV_STS_NOTUSED.
 *               (The Provider-Status will be changed to EDDI_CRT_PROV_STS_RESERVED this function.)
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListUnReserveEntry( EDDI_CRT_PROVIDER_PTR_TYPE        const  pProvider,
                                                                   EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT,
                                                                   LSA_BOOL                          const  bIsDfp )
{
    EDDI_CRT_PROVIDER_LIST_TYPE  *  const  pProvList         = &pCRT->ProviderList;
    LSA_INT32                              LastIndex;
    LSA_INT32                              FirstSearchIndex;
    LSA_INT32                              ctr;
    LSA_BOOL                               bLastIndexChanged = LSA_FALSE;

    #if defined (EDDI_CFG_SYSRED_2PROC)
    EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB = pCRT->pHDB->pDDB;

    if (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)
    {
        LSA_UINT16  i;

        if (pProvider->ProviderId == pDDB->CRT.ProviderList.MinSysRedProviderID)
        {
            if (pProvider->ProviderId == pDDB->CRT.ProviderList.MaxSysRedProviderID)     // was last SysRedProvider
            {
                pDDB->CRT.ProviderList.MinSysRedProviderID  = 0xFFFF;
                pDDB->CRT.ProviderList.MaxSysRedProviderID  = 0;
            }
            else
            {
                // Update MinSysRedProviderID
                for (i = pDDB->CRT.ProviderList.MinSysRedProviderID + 1; i <= pDDB->CRT.ProviderList.MaxSysRedProviderID; ++i)
                {
                    EDDI_CRT_PROVIDER_TYPE  *  const  pLocalProvider = &pProvList->pEntry[i];

                    if (   (pLocalProvider->Status    != EDDI_CRT_PROV_STS_NOTUSED)
                        && (pLocalProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED) )
                    {
                        pDDB->CRT.ProviderList.MinSysRedProviderID = i;
                        break;
                    }
                }
            }
        }
        else if (pProvider->ProviderId == pDDB->CRT.ProviderList.MaxSysRedProviderID)
        {
            // Update MaxSysRedProviderID
            for (i = pDDB->CRT.ProviderList.MaxSysRedProviderID - 1; i >= pDDB->CRT.ProviderList.MinSysRedProviderID; --i)
            {
                EDDI_CRT_PROVIDER_TYPE  *  const  pLocalProvider = &pProvList->pEntry[i];

                if (   (pLocalProvider->Status    != EDDI_CRT_PROV_STS_NOTUSED)
                    && (pLocalProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED) )
                {
                    pDDB->CRT.ProviderList.MaxSysRedProviderID = i;
                    break;
                }
            }
        }
    }
    #endif

    if (EDDI_LIST_TYPE_FCW == pProvider->LowerParams.ListType)
    {
        if (bIsDfp)
        {
            FirstSearchIndex = (LSA_INT32)pProvList->LastIndexDFP;
            LastIndex = (LSA_INT32)pCRT->MetaInfo.DFPIDStartProv;
        }
        else
        {
            FirstSearchIndex = (LSA_INT32)pProvList->LastIndexRTC3;
            LastIndex = (LSA_INT32)pCRT->MetaInfo.RTC3IDStartProv;
        }
    }
    else
    {
        FirstSearchIndex = (LSA_INT32)pProvList->LastIndexRTC12;
        LastIndex = (LSA_INT32)pCRT->MetaInfo.RTC12IDStartProv;
    }

    if (pProvider->Status == EDDI_CRT_PROV_STS_NOTUSED)
    {
        EDDI_Excp("EDDI_CRTProviderListUnReserveEntry, Invalid Provider-Status:", EDDI_FATAL_ERR_EXCP, pProvider->Status, 0);
        return;
    }

    pProvider->Status = EDDI_CRT_PROV_STS_NOTUSED;
    pProvider->bXCW_DBInitialized = LSA_FALSE;

    pProvList->UsedEntries--;

    // Update LastIndex
    for (ctr = FirstSearchIndex; ctr>LastIndex; ctr--)
    {
        EDDI_CRT_PROVIDER_TYPE  *  const  pLocalProvider = &pProvList->pEntry[ctr];
        if (pLocalProvider ->Status != EDDI_CRT_PROV_STS_NOTUSED)
        {
            LastIndex = ctr;
            bLastIndexChanged = LSA_TRUE;
            //leave for-loop
            break;
        }
    }

    if (   (0 == pProvList->UsedEntries)
        && (pProvList->UsedACWs != 0) )
    {
        EDDI_Excp("EDDI_CRTConsumerListUnReserveEntry: UsedACWs!=0. ", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, 0);
        return;
    }

    if (bLastIndexChanged)
    {
        if (EDDI_LIST_TYPE_FCW == pProvider->LowerParams.ListType)
        {
            if (bIsDfp)
            {
                pProvList->LastIndexDFP = (LSA_UINT32)LastIndex;
            }
            else
            {
                pProvList->LastIndexRTC3 = (LSA_UINT32)LastIndex;
            }
        }
        else
        {
            pProvList->LastIndexRTC12 = (LSA_UINT32)LastIndex;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListReserveFreeEntry()
 *
 *  Description: Searches the next free Entry in the Providerlist.
 *               A Entry is treated as free if its Status == EDDI_CRT_PROV_STS_NOTUSED.
 *               (The Provider-Status will be changed to EDDI_CRT_PROV_STS_RESERVED within this function.)
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_LIMIT_REACHED else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListReserveFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       EDDI_CRT_PROVIDER_PTR_TYPE            *  const  ppProvider,  //OUT
                                                                       EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE         const  pCRT,
                                                                       LSA_UINT8                                const  ProviderType,
                                                                       LSA_BOOL                                 const  bIsDfp )
{
    LSA_UINT32                             ctr;
    LSA_UINT32                             IdBeginSearchFree;
    LSA_UINT32                             IdEndSearchFree;
    LSA_UINT32                          *  pLastIndex;
    EDDI_CRT_PROVIDER_LIST_TYPE  *  const  pProvList               = &pCRT->ProviderList;

    //    *************************
    //    *    ACW    *    FCW    *
    //    *************************
    //the Provider-List is segmented into 2 classes: ACW + FCW!
    //- do not change without checking EDDI_CRTCreateProviderList()!
    //- do not change without checking EDDI_SetDMACtrlIntern()!

    switch (ProviderType)
    {
        case EDDI_RTC3_PROVIDER:  //=>FCW
        {
            //set limits for searching a free Provider-Entry in FCW-class
            if (bIsDfp)
            {
                //set limits for searching a free provider entry in FCW-class
                IdBeginSearchFree = pCRT->MetaInfo.DFPIDStartProv;
                IdEndSearchFree   = pCRT->MetaInfo.DFPIDEndProv;
                pLastIndex        = &pProvList->LastIndexDFP;
            }
            else
            {
                //set limits for searching a free provider entry in FCW-class
                IdBeginSearchFree = pCRT->MetaInfo.RTC3IDStartProv;
                IdEndSearchFree   = pCRT->MetaInfo.RTC3IDEndProv;
                pLastIndex        = &pProvList->LastIndexRTC3;
            }

            break;
        }
        default:  //=>ACW/AUX-Provider
        {
            //set limits for searching a free Provider-Entry in ACW-class
            IdBeginSearchFree   = pCRT->MetaInfo.RTC12IDStartProv;
            IdEndSearchFree     = pCRT->MetaInfo.RTC12IDEndProv;
            pLastIndex          = &pProvList->LastIndexRTC12;
        }
    }

    //search for a free Provider-Entry
    for (ctr = IdBeginSearchFree; ctr < IdEndSearchFree; ctr++)
    {
        EDDI_CRT_PROVIDER_TYPE  *  const  pProvider = &pProvList->pEntry[ctr];

        if (pProvider->Status == EDDI_CRT_PROV_STS_NOTUSED)
        {
            EDDI_MEMSET(pProvider, 0, sizeof(EDDI_CRT_PROVIDER_TYPE));

            pProvider->Status               = EDDI_CRT_PROV_STS_RESERVED;
            pProvider->ProviderId           = (LSA_UINT16)ctr;
            if (ctr > *pLastIndex)
            {
                *pLastIndex = ctr;
            }
            pProvList->UsedEntries++;
            *ppProvider = pProvider;

            return EDD_STS_OK;
        }
    }

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                      "EDDI_CRTProviderListReserveFreeEntry<-ERROR no free Provider found. ProviderType:0x%X MaxEntries for this ProviderType:0x%X",
                      ProviderType, (IdEndSearchFree - IdBeginSearchFree));

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_ERR_LIMIT_REACHED;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTConsumerListReserveFreeEntry()
 *
 *  Description:Searches the next free Entry in the Consumerlist.
 *              A Entry is treated as free if its Status == EDDI_CRT_CONS_STS_NOTUSED.
 *              (The Consumer-Status will be changed to EDDI_CRT_CONS_STS_RESERVED within this function.)
 *
 *  Arguments:  pConsList:      reference to Consumerlist
 *              ppConsumer(OUT):a free Consumer found in the list.
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_LIMIT_REACHED else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListReserveFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                       EDDI_CRT_CONSUMER_PTR_TYPE        *  const  ppConsumer,  //OUT
                                                                       EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE     const  pCRT,
                                                                       LSA_UINT8                            const  ListType,
                                                                       LSA_BOOL                             const  bIsDfp )
{
    LSA_UINT32                             ctr;
    LSA_UINT32                             IdBeginSearchFree;
    LSA_UINT32                             IdEndSearchFree;
    LSA_UINT32                          *  pLastIndex;
    EDDI_CRT_CONSUMER_LIST_TYPE  *  const  pConsList = &pCRT->ConsumerList;

    //    *************************
    //    *    ACW    *    FCW    *
    //    *************************
    //the Consumer-List is segmented into 3 classes: ACW, FCW and DFP!
    //- do not change without checking EDDI_CRTCreateConsumerList()!
    //- do not change without checking EDDI_CRTConsumerListFindByFrameId()!
    //- do not change without checking EDDI_SetDMACtrlIntern()!

    if (  (EDDI_LIST_TYPE_FCW == ListType)
        && bIsDfp)
    {
        //set limits for searching a free Consumer-Entry in DFP-class
        IdBeginSearchFree = pCRT->MetaInfo.DFPIDStart;
        IdEndSearchFree   = pCRT->MetaInfo.DFPIDEnd;
        pLastIndex        = &pConsList->LastIndexDFP;
    }
    else
    {
        if (pCRT->CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_CRIT)
        {
            //check for ConsID limits with small SCFs
            if ((pConsList->LastIndexRTC123+1) >= EDDICrtMaxConsSmallSCF[pCRT->CycleBaseFactor])
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerListReserveFreeEntry, ERROR only %i Consumers for SCF %i allowed",
                                  EDDICrtMaxConsSmallSCF[pCRT->CycleBaseFactor], pCRT->CycleBaseFactor);
                return EDD_STS_ERR_LIMIT_REACHED;
            }
        }

        //set limits for searching a free Consumer-Entry in ACW/FCW-class
        IdBeginSearchFree = pCRT->MetaInfo.RTC123IDStart;
        IdEndSearchFree   = pCRT->MetaInfo.RTC123IDEnd;
        pLastIndex        = &pConsList->LastIndexRTC123;
    }

    //search for a free Consumer-Entry
    for (ctr = IdBeginSearchFree; ctr < IdEndSearchFree; ctr++)
    {
        EDDI_CRT_CONSUMER_TYPE  *  const  pConsumer = &pConsList->pEntry[ctr];
        EDDI_CRT_CONS_SB_TYPE   *  const  pSBEntry  = pConsumer->pSB;

        if (pSBEntry->Status == EDDI_CRT_CONS_STS_NOTUSED)
        {
            EDDI_MEMSET(pConsumer, 0, sizeof(EDDI_CRT_CONSUMER_TYPE));

            pConsumer->ConsumerId           = (LSA_UINT16)ctr;                  
            //restore reference!                                      
            pConsumer->pSB                  = pSBEntry;                         
            pSBEntry->Status                = EDDI_CRT_CONS_STS_RESERVED;
            pConsumer->LowerParams.ListType = ListType;

            if (ctr > *pLastIndex)
            {
                *pLastIndex = ctr;
            }
    
            if (  (EDDI_LIST_TYPE_FCW == ListType)
                && bIsDfp)
            {
                pConsList->UsedEntriesDFP++;
            }
            else
            {
                pConsList->UsedEntriesRTC123++;
            }
            
            *ppConsumer = pConsumer;
            /*EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
            "EDDI_CRTConsumerListReserveFreeEntry, free Consumer found. ListType:0x%X ConsumerID:0x%X",
            ListType, ctr);*/

            return EDD_STS_OK;
        }
    }

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                      "EDDI_CRTConsumerListReserveFreeEntry, ERROR no free Consumer found. ListType:0x%X MaxEntries for this ListType:0x%X",
                      ListType, (IdEndSearchFree - IdBeginSearchFree));

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_ERR_LIMIT_REACHED;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderInit()  //HelperFunction only
 *
 *  Description: Uses the RQB to initializes the pProvider.
 *               Checks if Params are all in a good Range
 *               Sets the pDataBuffer relativ to the OUT_DATA_BASE-Image.
 *               The Paramblock of Requestblock will also be changed ! ->pBuffer !
 *
 *  Arguments:   pProvider (THIS): the Pointer of Provider
 *               (the ProviderID must already be initialized !)
 *               pRQB      (IN/OUT): the Pointer to the RQB containing all params
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderInit( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                       EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                       EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE const  pProvParam,
                                                       LSA_UINT8                            const  ListType,
                                                       LSA_UINT8                            const  ProviderType )
{
    LSA_RESULT                        Status = EDD_STS_OK;
    #if defined (EDDI_CFG_REV7)
    EDDI_CRT_PAEA_TYPE         const  PAEAType   = pDDB->CRT.PAEA_Para;
    EDDI_CRT_PAEA_APDU_STATUS      *  pPAEA_APDU = EDDI_NULL_PTR;
    #endif

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderInit->");

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    pProvider->Locked             = LSA_FALSE;
    pProvider->pRQB_PendingRemove = EDDI_NULL_PTR;
    #endif

    // now init provider with the given parameters
    pProvParam->ProviderID = pProvider->ProviderId;

    //AutoStop Provider init
    pProvider->AutoStopConsumerID    = EDDI_CONSUMER_ID_INVALID;
    pProvider->pNextAutoStopProv     = EDDI_NULL_PTR;

    pProvider->bBufferParamsValid    = LSA_FALSE;

    if (EDD_CSRT_PROV_PROP_RTCLASS_UDP == (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
    {
        pProvider->LowerParams.RT_Type = EDDI_RT_TYPE_UDP;
    }
    else
    {
        pProvider->LowerParams.RT_Type = EDDI_RT_TYPE_NORMAL;
    }

    pProvider->Properties                   = pProvParam->Properties;
    pProvider->LocalDataStatus              = EDD_CSRT_DSTAT_LOCAL_INIT_VALUE;
    EDDI_CSRT_SET_DATASTATUS_STATE(pProvider->LocalDataStatus, (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)?EDD_CSRT_DSTAT_BIT_STATE_BACKUP:EDD_CSRT_DSTAT_BIT_STATE_PRIMARY); //SYSRED providers start in state BACKUP
    pProvider->GroupId                      = pProvParam->GroupID;
    pProvider->UpperUserId                  = pProvParam->UserID;
    pProvider->LowerParams.FrameId          = pProvParam->FrameID;
    pProvider->pFrmHandler                  = EDDI_NULL_PTR;
    pProvider->bXCW_DBInitialized           = LSA_FALSE;
    pProvider->pLowerCtrlACW                = EDDI_NULL_PTR;
    pProvider->pCWStored                    = EDDI_NULL_PTR;
    pProvider->PendingIndEvent              = 0;
    pProvider->LowerParams.DataLen          = pProvParam->DataLen;
    pProvider->LowerParams.DataOffset       = pProvParam->IOParams.DataOffset;
    pProvider->LowerParams.BufferProperties = pProvParam->IOParams.BufferProperties;
    pProvider->IRTtopCtrl.ProvActiveMask    = (EDDI_RTC3_PROVIDER == pProvider->LowerParams.ProviderType)?0:EDDI_PROV_ACTIVE_CLASS12_MAIN;
    pProvider->IRTtopCtrl.ProvState         = EDDI_PROV_STATE_PASSIVE;
    pProvider->LowerParams.DataStatusOffset = EDD_DATAOFFSET_INVALID;    //will only change to a valid value for SOC w. PAEARAM
    pProvider->bActivateAllowed             = LSA_TRUE;
    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    pProvider->bHadBeenActive               = LSA_FALSE;
    #endif
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    pProvider->pIOCW                        = (LSA_UINT32 *)0;
    #endif

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        //init DG-consumer
        Status = EDDI_DFPProviderInit(pDDB, pProvider, pProvParam);
        if (EDD_STS_OK != Status)
        {
            return Status;
        }
    }
    else
    {  
        #if defined (EDDI_CFG_REV7)
        if (0 != PAEAType.PAEA_BaseAdr)
        {
            if (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) )
            {
                //AUX provider needs to be in KRAM also!
                pProvider->usingPAEA_Ram = LSA_FALSE;
            }
            else
            {
                //Set to TRUE and alloc apdu-memory, even if properties are UNDEFINED.
                //If they get set to SYNC during ACTIVATE, the buffer will be deallocated again
                pProvider->usingPAEA_Ram = LSA_TRUE;

                if (EDDI_RTC3_AUX_PROVIDER != ProviderType)
                {
                    //Place PAEA-APDU element to memory-pool according to its ID.
                    Status = EDDI_MemGetApduBuffer(pProvider->ProviderId, pDDB, &pPAEA_APDU, EDDI_MEM_APDU_PROVIDER_BUFFER);
                    if (EDD_STS_OK != Status)
                    {
                        return Status;
                    }
                
                    //Under IOC, the APDU is not reachable for the user
                    pProvider->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pPAEA_APDU;
                    if (     (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC != (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
                          && (EDDI_RTC3_PROVIDER                 != ProviderType)
                       )
                    {
                        pProvider->LowerParams.DataStatusOffset = (LSA_UINT32)&pPAEA_APDU->APDU_Status.Detail.DataStatus - (LSA_UINT32)pDDB->pKRam;
                    }

                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTProviderInit, PAEA-APDU Pool --> getting buffer 0x%X", (LSA_UINT32)pPAEA_APDU);
                    EDDI_CSRT_SET_DATASTATUS_STATE(pProvider->LocalDataStatus, (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)?EDD_CSRT_DSTAT_BIT_STATE_BACKUP:EDD_CSRT_DSTAT_BIT_STATE_PRIMARY); //SYSRED providers start in state BACKUP
                    EDDI_CRTCompResetAPDUStatus(pDDB, (EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pPAEA_APDU, pProvider->LocalDataStatus, (LSA_UINT8)0, LSA_FALSE /*bSysRed*/);

                    //Set user data application pointer to PAEA-RAM (offset only)
                    pPAEA_APDU->pPAEA_Data = EDDI_HOST2IRTE32(pProvider->LowerParams.DataOffset);

                    #if defined (EDDI_CFG_SYSRED_2PROC)
                        if (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)
                        {
                            // adjust min and max Provider ID
                            if (pProvider->ProviderId < pDDB->CRT.ProviderList.MinSysRedProviderID)
                            {
                                pDDB->CRT.ProviderList.MinSysRedProviderID = pProvider->ProviderId;
                            }

                            if (pProvider->ProviderId > pDDB->CRT.ProviderList.MaxSysRedProviderID)
                            {
                                pDDB->CRT.ProviderList.MaxSysRedProviderID = pProvider->ProviderId;
                            }
                        }
                    #endif
                    
                }
                else        
                {
                    //AUX-Prov shares the APDU-Buffer with the Class3-Prov.
                    pProvider->LowerParams.pKRAMDataBuffer = pProvider->IRTtopCtrl.pAscProvider->LowerParams.pKRAMDataBuffer;
                }
            }
        }
        else
        {
            pProvider->usingPAEA_Ram = LSA_FALSE;

            //Using 3B-Interface with APDU in user controlled space
            //now done in EDDI_CRTProviderSetBuffer
        }
        #else
        //now done in EDDI_CRTProviderSetBuffer
        #endif

    }

    pProvider->LowerParams.ImageMode           = EDDI_IMAGE_MODE_UNDEFINED;
    pProvider->LowerParams.CycleReductionRatio = pProvParam->CycleReductionRatio;   //same RR as class3-provider

    if (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pProvParam->CycleReductionRatio)
    {
        if (EDDI_RTC3_AUX_PROVIDER == ProviderType)
        {
            //check if the reduction ratio of the class3-provider is binary
            if (EDDI_RedIsBinaryValue((LSA_UINT32)pProvParam->CycleReductionRatio * (LSA_UINT32)pDDB->CycCount.Entity))
            {
                pProvider->LowerParams.CycleReductionRatio = EDDI_AUX_PROVIDER_RR;   //fixed RR
            }
        }
        else
        {
            //update max CycleReduction
            if (pDDB->CRT.ProviderList.MaxUsedReduction < pProvider->LowerParams.CycleReductionRatio)
            {
                pDDB->CRT.ProviderList.MaxUsedReduction = pProvider->LowerParams.CycleReductionRatio;
            }
        }
    }
    pProvider->LowerParams.CyclePhase          = pProvParam->CyclePhase;
    pProvider->LowerParams.CyclePhaseSequence  = pProvParam->CyclePhaseSequence;
    pProvider->LowerParams.ListType            = ListType;  
    pProvider->LowerParams.ProviderType        = ProviderType;

    switch (ListType)
    {
        case EDDI_LIST_TYPE_FCW:
        {
            //image mode gets set in SetBuffer
            break;
        }

        case EDDI_LIST_TYPE_ACW:
        {
            #if defined (EDDI_CFG_REV7)
            if (   (0 != PAEAType.PAEA_BaseAdr)
                || (EDDI_RTC3_AUX_PROVIDER == ProviderType) )
            {
                pProvider->LowerParams.ImageMode = EDDI_IMAGE_MODE_ASYNC;
            }
            else
            {
                pProvider->LowerParams.ImageMode = EDDI_IMAGE_MODE_SYNC;
            }
            #else
            pProvider->LowerParams.ImageMode = EDDI_IMAGE_MODE_ASYNC;
            #endif

            #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
            if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
            {
                Status = EDDI_RtoProvIni(pProvider, pDDB);
                return Status;
            }
        #endif

            break;
        }

        default:
        {
            EDDI_Excp("EDDI_CRTProviderInit", EDDI_FATAL_ERR_EXCP, ListType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    //update ProviderStatus
    pProvider->Status = EDDI_CRT_PROV_STS_INACTIVE;

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTProvInit()                               */
/*                                                                         */
/* D e s c r i p t i o n: function is called for ACWs and FCWs             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProvInit( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                 EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                 LSA_UINT8                   const  ProviderType )
{
    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTProvInit->pProvParam->DataOffset:0x%X", pProvider->LowerParams.DataOffset);

    #if defined (EDDI_CFG_REV7)
    //SOC1/2 does not support DMA -> 3B-Interface or PAEA
    if (pProvider->usingPAEA_Ram)
    {
        //Pointing to user data in PAEA-RAM
        //pProvider->pUserDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(pDDB->CRT.PAEA_Para.PAEA_BaseAdr +
        //                                                         pProvParam->DataOffset);
        //Pointing to NULL
        pProvider->pUserDataBuffer = LSA_NULL;

        LSA_UNUSED_ARG(ProviderType); //satisfy lint!
    }
    #else
    if (   (ProviderType == EDDI_RTC3_PROVIDER)       //=>FCW
        || (ProviderType == EDDI_RTC3_AUX_PROVIDER))  //=>ACW
    {
        //update pUserDataBuffer of this Provider
        pProvider->pUserDataBuffer = pDDB->ProcessImage.pUserDMA + pProvider->LowerParams.DataOffset;

        if //this Provider has no associated Provider
           (!pProvider->IRTtopCtrl.pAscProvider)
        {
            EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->pUserDataBuffer, pProvider->LocalDataStatus, (LSA_UINT8)0, LSA_FALSE /*bSysRed*/);
        }
        else //this Provider has an associated Provider
        {
            //calc pointer to associated Provider
            EDDI_CRT_PROVIDER_PTR_TYPE  const  pAssociatedProvider = pProvider->IRTtopCtrl.pAscProvider;

            //update pUserDataBuffer of associated Provider too!
            pAssociatedProvider->pUserDataBuffer = pProvider->pUserDataBuffer;
        }
    }
    #endif
    else
    {
        //no DMA -> User-Puffer nur im KRAM
        pProvider->pUserDataBuffer = pProvider->LowerParams.pKRAMDataBuffer;
    }

    // we must *not* reset the FrameMemory !!. This must be done by the User.
    // we only reset the APDU-Status;
    // set CycleNumber to 0

    if //   (this Provider has no associated Provider (applies to RCT1/2 providers too!))
       //OR (it has one that is not active yet)
       (   (!pProvider->IRTtopCtrl.pAscProvider)
	    || (   (pProvider->IRTtopCtrl.pAscProvider)
		    && (EDDI_CRT_PROV_STS_ACTIVE != pProvider->IRTtopCtrl.pAscProvider->Status)))
    {
        EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer, pProvider->LocalDataStatus, (LSA_UINT8)0, (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)?LSA_TRUE:LSA_FALSE /*bSysRed*/);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTConsumerListUnReserveEntry()
 *
 *  Description: Searches the next free Entry in the Providerlist.
 *               A Entry is treated as free if its Status == EDDI_CRT_PROV_STS_NOTUSED.
 *               (The Provider-Status will be changed to EDDI_CRT_PROV_STS_RESERVED within this function.)
 *
 *  Arguments:   pProvList:      reference to Providerlist
 *               ppProvider(OUT):a free provider found in the list.
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListUnReserveEntry( EDDI_CRT_CONSUMER_PTR_TYPE        const  pConsumer,
                                                                   EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT,
                                                                   LSA_BOOL                          const  bIsDfp )
{
    EDDI_CRT_CONSUMER_LIST_TYPE  *  const  pConsList = &pCRT->ConsumerList;
    LSA_INT32                              NewLastIndex;
    LSA_INT32                              LastIndex;
    LSA_INT32                              FirstSearchIndex;
    LSA_INT32                              ctr;
    LSA_BOOL                               bLastIndexChanged = LSA_FALSE;
    LSA_UINT8                       const  ListType = pConsumer->LowerParams.ListType;

    if (  (EDDI_LIST_TYPE_FCW == ListType)
        && bIsDfp)
    {
        FirstSearchIndex = (LSA_INT32)pConsList->LastIndexDFP;
        LastIndex = (LSA_INT32)pCRT->MetaInfo.DFPIDStart;
        NewLastIndex     = LastIndex;
    }
    else
    {
        FirstSearchIndex = (LSA_INT32)pConsList->LastIndexRTC123;
        LastIndex = (LSA_INT32)pCRT->MetaInfo.RTC123IDStart;
        NewLastIndex     = LastIndex;
    }

    if (pConsumer->pSB->Status == EDDI_CRT_CONS_STS_NOTUSED)
    {
        EDDI_Excp("EDDI_CRTConsumerListUnReserveEntry", EDDI_FATAL_ERR_EXCP, pConsumer->pSB->Status, 0);
        return;
    }

    // Update nr of consumers
    if (  (EDDI_LIST_TYPE_FCW == ListType)
        && bIsDfp)
    {
      pConsList->UsedEntriesDFP--;
    }
    else
    {
      pConsList->UsedEntriesRTC123--;
    }
    
    // Set new Status
    pConsumer->pSB->Status          = EDDI_CRT_CONS_STS_NOTUSED;
    pConsumer->LowerParams.ListType = EDDI_LIST_TYPE_UNDEFINED;

    // Update LastIndex
    for (ctr = FirstSearchIndex; ctr>=LastIndex; ctr--)
    {
        if (EDDI_CRT_CONS_STS_NOTUSED == pConsList->pSBEntry[ctr].Status)
        {
            NewLastIndex = ctr;
            bLastIndexChanged = LSA_TRUE;
        }
        else
        {
            //highest ConsumerID reached -> leave for-loop
            break;
        }
    }

    if (   (0 == pConsList->UsedEntriesRTC123)
        && (pConsList->UsedACWs != 0) )
    {
        EDDI_Excp("EDDI_CRTConsumerListUnReserveEntry: UsedACWs!=0. ", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, 0);
        return;
    }

    if (bLastIndexChanged)
    {
        if (  (EDDI_LIST_TYPE_FCW == ListType)
            && bIsDfp)
        {
            pConsList->LastIndexDFP = (LSA_UINT32)NewLastIndex;
        }
        else
        {
            pConsList->LastIndexRTC123 = (LSA_UINT32)NewLastIndex;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCheckForProviderRedundantFrames()
 *
 *  Description: Check if the Provider is redundant
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckForProviderRedundantFrames(       EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                                                EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                                                EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam )
{
    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCheckForProviderRedundantFrames-> FrameID:0x%X", pProvParam->FrameID);

    //Check MRPD Frame ID
    if(    (pProvParam->FrameID  >= EDD_SRT_FRAMEID_IRT_REDUNDANT_START)
        && (pProvParam->FrameID  <= EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP))
    {
        if (EDD_CSRT_PROV_PROP_RTCLASS_3 != (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckForProviderRedundantFrames, ERROR: MRPD works only with Class3 Provider -> FrameId:0x%X,  pProvParam->Properties:0x%X", pProvParam->FrameID, pProvParam->Properties);
            return EDD_STS_ERR_PARAM;
        }
     
        if (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckForProviderRedundantFrames, ERROR: MRPD dosn't work with 2CR (Legacy) Provider -> FrameId:0x%X,  pProvParam->Properties:0x%X", pProvParam->FrameID, pProvParam->Properties);
            return EDD_STS_ERR_PARAM;
        }
                
        //Check if Frame ID is even        
        if (1 == (pProvParam->FrameID & 1) )
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckForProviderRedundantFrames, ERROR: The FrameId:0x%X must be even.", pProvParam->FrameID);
            return EDD_STS_ERR_PARAM;  
        } 
                      
        #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW) && !defined(EDDI_CFG_ENABLE_MRPD_ASYNC_PROVIDER) && defined (EDDI_CFG_REV6)
        if ((pProvParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) != EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC)
        {
			if ((pProvParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) != EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED)
			{
				EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckForProviderRedundantFrames, ERROR: No redundant frames allowed for 3BIF-SW FrameId:0x%X", pProvParam->FrameID);
				return EDD_STS_ERR_PARAM;   
			}
        }
        #endif
        //Mark the Provider as redundant
        pProvider->bRedundantFrameID = LSA_TRUE;    
    }
    else
    {
        //Mark the Provider as non-redundant
        pProvider->bRedundantFrameID = LSA_FALSE;
    }
    
    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK; 
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCheckForConsumerRedundantFrames()
 *
 *  Description: Check if the Consumer is redundant
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckForConsumerRedundantFrames( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                                          EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,     //!! not pointing to const Data
                                                                          EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam )
{
    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTCheckForConsumerRedundantFrames-> FrameID:0x%X", pConsParam->FrameID);
    
    if(    (pConsParam->FrameID  >= EDD_SRT_FRAMEID_IRT_REDUNDANT_START)
        && (pConsParam->FrameID  <= EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP))
    {       
        if (EDD_CSRT_CONS_PROP_RTCLASS_3 != (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListAddEntry, ERROR: MRPD works only with Class3 Consumer -> FrameId:0x%X, pProvParam->Properties:0x%X", pConsParam->FrameID, pConsParam->Properties);
            return EDD_STS_ERR_PARAM;
        }

        if (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerListAddEntry, ERROR: MRPD dosn't work with 2CR (Legacy) Consumer -> FrameId:0x%X,  pProvParam->Properties:0x%X", pConsParam->FrameID, pConsParam->Properties);
            return EDD_STS_ERR_PARAM;
        }
                
        //Check if Frame ID is even        
        if (1 == (pConsParam->FrameID & 1) )
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerListAddEntry, ERROR: The FrameId:0x%X must be even.", pConsParam->FrameID);
            return EDD_STS_ERR_PARAM;  
        }  
                
        //Mark the Consumer as redundant
        pConsumer->bRedundantFrameID = LSA_TRUE;    
    }
    else
    {
        //Mark the Provider as non-redundant
        pConsumer->bRedundantFrameID = LSA_FALSE;
    }
    
    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CheckAndSetProviderControlMacAddr()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAndSetProviderControlMacAddr( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                                         EDDI_CRT_PROVIDER_PTR_TYPE           const  pProvider,
                                                                         EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler)
{
    EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * const  pFrameData           = &pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord->PDIRFrameData; //pFrameData must be valid here
    LSA_UINT32                              const  FrameDataProperties  = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD);
    LSA_UINT8                               const  BlockVersionLow      = pDDB->PRM.PDIRData.pRecordSet_A->pLocalPDIRDataRecord->PDIRFrameData.BlockHeader.BlockVersionLow;      

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAndSetProviderControlMacAddr->"); 

    //Set the MAC Address
    if (EDDI_RTC3_PROVIDER == pProvider->LowerParams.ProviderType)
    {
        //no legacy 
        if (!(pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            //no MC-CR
            if (!(pProvider->Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR))
            {
                if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V10 == BlockVersionLow )
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAndSetProviderControlMacAddr, ERROR: PDIRFrameData is V1.0, needed v1.1");
                    return EDD_STS_ERR_PARAM;   
                }
                else if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == BlockVersionLow )
                {
                    if (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CheckAndSetProviderControlMacAddr, Multicast Fast Forwarding is not allowed with Lagacy mode. ProvID:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                        return EDD_STS_ERR_PARAM;    
                    }
                    
                    #if defined (EDDI_CFG_DFP_ON)
                    //special handling until frameproperties are handled correctly by TIAPortal
                    EDDI_SetMACAddrRTC3(pDDB, FrameDataProperties, pProvider->LowerParams.DstMAC.MacAdr, pFrmHandler->FrameId, pProvider->bIsPFProvider);
                    #else
                    EDDI_SetMACAddrRTC3(pDDB, FrameDataProperties, pProvider->LowerParams.DstMAC.MacAdr, pFrmHandler->FrameId, LSA_FALSE /*bIsDfp*/);
                    #endif
                }
            }
            else if (pProvider->Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR) // with MC-CR
            {
                if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V10 == BlockVersionLow )
                {
                    //Check the MC-CR against from user first
                    if(!EDDI_CRTIsMACMulticastAddr(&pProvider->LowerParams.DstMAC))
                    {
                        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Multicast Address");
                        return EDD_STS_ERR_PARAM;   
                    }
                    //EDDI_SetRTClass3MultiCastMACAddr(pProvider->LowerParams.DstMAC.MacAdr);
                }               
                else if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == BlockVersionLow )
                {
                    //Check the MC-CR against from user
                    if (   (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_LEGACY     == FrameDataProperties)
                        || (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_RT_CLASS_3 == FrameDataProperties))  
                    {
                        if(!EDDI_CRTIsMACMulticastAddr(&pProvider->LowerParams.DstMAC))
                        {
                            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Multicast Address");
                            return EDD_STS_ERR_PARAM;   
                        }
                        //EDDI_SetRTClass3MultiCastMACAddr(pProvider->LowerParams.DstMAC.MacAdr);
                    }
                    else if (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataProperties)
                    {
                        EDD_MAC_ADR_TYPE  DstMAC;
                        LSA_UINT32        Cnt;
                        
                        EDDI_SetFastForwardingMulticastMACAdd(DstMAC.MacAdr, pFrmHandler->FrameId);  
                        
                        //Check if the User gave the same FFW MAC Address
                        for (Cnt = 0; Cnt < EDD_MAC_ADDR_SIZE; Cnt++)
                        {
                            if (DstMAC.MacAdr[Cnt] != pProvider->LowerParams.DstMAC.MacAdr[Cnt])
                            {
                                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid FFW MAC Address from user");
                                return EDD_STS_ERR_PARAM; 
                            }
                        }    
                    } 
                } 
            }
        }
        else if (pProvider->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) //legacy  
        {
            //no MC-CR
            if (!(pProvider->Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR))
            {
                if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V10 == BlockVersionLow )
                {
                    //Do nothing, take added DST-MAC 
                }
                else if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == BlockVersionLow )
                {
                    if (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_LEGACY != FrameDataProperties)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_CheckAndSetProviderControlMacAddr, Only Legacy with PDIRFrameData V1.1 allowed. ProvID:0x%X, FrameID:0x%X", pProvider->ProviderId, pProvider->LowerParams.FrameId);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
                        return EDD_STS_ERR_PARAM;    
                    }   
                }    
            }
            else
            {
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAndSetProviderControlMacAddr, ERROR: Legacy-Mode with MC-CR is not allowed");
                return EDD_STS_ERR_PARAM;  
            }
        }
    }
    else
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAndSetProviderControlMacAddr, ERROR: Only RTC3 is allowed, pProvider->Properties:0x%X", pProvider->Properties);
        return EDD_STS_ERR_PARAM;  
    }

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAndSetProviderControlMacAddr <-"); 
 
    return EDD_STS_OK;
}                                                    
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SetRTClass3MultiCastMACAddr()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetRTClass3MultiCastMACAddr( LSA_UINT8  *  MacAdr)
{                               
    LSA_UINT8                Index;
    EDD_MAC_ADR_TYPE  const  DstMacAdressRTC3 = { EDDI_MAC_RT_3_DA_BEGIN };
    
    for (Index = 0; Index < EDD_MAC_ADDR_SIZE; Index++)
    {
        MacAdr[Index] = DstMacAdressRTC3.MacAdr[Index];
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SetFastForwardingMulticastMACAdd()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetFastForwardingMulticastMACAdd( LSA_UINT8       *  MacAdr,
                                                                      LSA_UINT16  const  FrameID)
{                               
    LSA_UINT8  Index;
    
    // use FastForwarding-MultiCast-DstMAC
    EDD_MAC_ADR_TYPE  const  DstMacAdr   = { EDDI_MAC_PROFINET_FF };
    LSA_UINT16        const  MaskFrameID = 0xFFFF;
    LSA_UINT8                MacAdrTemp;

    for (Index=0; Index < EDD_MAC_ADDR_SIZE; Index++)
    {
        MacAdr[Index] = DstMacAdr.MacAdr[Index];
    }

    // MacAdr[0]    : FrameID(11:8) + 0011
    MacAdrTemp = EDDI_GET_BIT_VALUE(FrameID, MaskFrameID, 8);
    EDDI_SHIFT_VALUE_LEFT(MacAdrTemp, 4);  
    MacAdr[0] |= MacAdrTemp;

    // MacAdr[1]    : FrameID(7:0)
    MacAdrTemp = (LSA_UINT8)EDDI_GET_BIT_VALUE(FrameID, MaskFrameID, 0);
    MacAdr[1]  = MacAdrTemp;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SetMACAddrRTC3()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetMACAddrRTC3( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB, 
                                                    LSA_UINT32                 const  FrameDataProperties,
                                                    LSA_UINT8                      *  MacAdr,
                                                    LSA_UINT16                 const  FrameID,
                                                    LSA_BOOL                   const  bIsDFP)
{                                  
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMACAddrRTC3->");
    
    if (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataProperties)  //&& DFP   
    {                    
        EDDI_SetFastForwardingMulticastMACAdd(MacAdr, FrameID);
    }
    else if (  (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_RT_CLASS_3 == FrameDataProperties)
             || bIsDFP)  //&& DFP
    {
        EDDI_SetRTClass3MultiCastMACAddr(MacAdr); 
    }
    else 
    {
        //Take dst-mac from provider-Add
    }
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SetMACAddrRTC3<-");
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetProviderAddMacAndIpAddr()                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetProviderMacAndIpAddr(       EDDI_CRT_PROVIDER_PTR_TYPE    const  pProvider,
                                                                   LSA_UINT16                    const  Properties,
                                                             const EDD_MAC_ADR_TYPE            * const  pDstMAC,
                                                             const EDD_IP_ADR_TYPE             * const  pDstIP)
{
    LSA_UINT32  ctr;
    
    if (EDDI_CSRT_PROV_PROP_RTCLASS_AUX == Properties)
    {
        //Do nothing with Aux-Provider
        return;
    }

    //Copy MAC Address from User to the LowerParams
    for (ctr = 0; ctr < EDD_MAC_ADDR_SIZE; ctr++)
    {
        pProvider->LowerParams.DstMAC.MacAdr[ctr] = pDstMAC->MacAdr[ctr];
    }
    
    //Set IP Address for UDP Provider
    if (EDD_CSRT_PROV_PROP_RTCLASS_UDP == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
    {
        pProvider->LowerParams.UPD_DstIPAdr.b.IP[0] = pDstIP->IPAdr[0];
        pProvider->LowerParams.UPD_DstIPAdr.b.IP[1] = pDstIP->IPAdr[1];
        pProvider->LowerParams.UPD_DstIPAdr.b.IP[2] = pDstIP->IPAdr[2];
        pProvider->LowerParams.UPD_DstIPAdr.b.IP[3] = pDstIP->IPAdr[3];
    }
    
    if (EDD_CSRT_PROV_PROP_RTCLASS_3 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
    {
        //no legacy 
        if (!(Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            //no MC-CR
            if (!(Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR))
            {
                for (ctr = 0; ctr < EDD_MAC_ADDR_SIZE; ctr++)
                {
                    pProvider->LowerParams.DstMAC.MacAdr[ctr] = 0;
                }
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTCheckListType()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_CRTCheckListType( LSA_UINT16  const  Properties, 
                                   LSA_UINT16  const  FrameID,
                                   LSA_UINT8   const  ListType)
{
    LSA_UINT16  const  Prop = Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK; 

    if (EDDI_LIST_TYPE_ACW == ListType)
    {
        if //RT-CLASS1-FrameID-range
           (   (FrameID >= EDDI_SRT_FRAMEID_RT_CLASS1_START)
            && (FrameID <= EDDI_SRT_FRAMEID_RT_CLASS1_STOP)
            && (   (EDD_CSRT_CONS_PROP_RTCLASS_1 == Prop)   || (EDD_CSRT_PROV_PROP_RTCLASS_1 == Prop)
                || (EDD_CSRT_CONS_PROP_RTCLASS_UDP == Prop) || (EDD_CSRT_PROV_PROP_RTCLASS_UDP == Prop)))
        {
            return EDD_STS_OK;
        }
        else if //RT-CLASS2-FrameID-range
                (   ((FrameID >= EDDI_SRT_FRAMEID_RT_CLASS2_START) && (FrameID <= EDDI_SRT_FRAMEID_RT_CLASS2_STOP))
                 && ((EDD_CSRT_CONS_PROP_RTCLASS_2 == Prop) || (EDD_CSRT_PROV_PROP_RTCLASS_2 == Prop)) )
        {
            return EDD_STS_OK;
        }
    }

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckProviderAddMacAndIpAddr()              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckProviderMacAndIpAddr(       EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                       LSA_UINT16                  const  Properties,
                                                                 const EDD_MAC_ADR_TYPE          * const  pDstMAC,
                                                                 const EDD_IP_ADR_TYPE           * const  pDstIP,
                                                                       LSA_BOOL                    const  bUndefAllowed)
{   
    //Check and Set MAC-Address
    if (EDD_CSRT_PROV_PROP_RTCLASS_3 != (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
    {
        //***********************************************
        //RTC1/2: DstMac has to be valid on ACTIVATE only
        //***********************************************
        if (!(Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            if (!bUndefAllowed)
            {
                //no MC-CR
                if (!(Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR))
                {
                    if (   (EDD_CSRT_PROV_PROP_RTCLASS_1 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
                        || (EDD_CSRT_PROV_PROP_RTCLASS_2 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
                       )
                    {
                        LSA_UINT8  i;
                        LSA_UINT16 Sum = 0;

                        for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
                        {
                            Sum += (LSA_UINT16)pDstMAC->MacAdr[i];
                        }

                        //Check for Unicast MAC Address
                        if(   EDDI_CRTIsMACMulticastAddr(pDstMAC)
                           || (0 == Sum) )
                        {
                            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Unicast Address");
                            return EDD_STS_ERR_PARAM;   
                        }
                        
                    }
                    else if (EDD_CSRT_PROV_PROP_RTCLASS_UDP == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))  //UDP Provider
                    {
                        //Check for Unicast IP Address
                        if(EDDI_CRTIsIPMulticastAddr(pDstIP))
                        {
                            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Unicast Address");
                            return EDD_STS_ERR_PARAM;   
                        }
                    }
                }
                else if (Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR) // with MC-CR 
                {
                
                    if (   (EDD_CSRT_PROV_PROP_RTCLASS_1 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
                        || (EDD_CSRT_PROV_PROP_RTCLASS_2 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
                       )
                    {
                        //Check for Multicast MAC Address               
                        if(!EDDI_CRTIsMACMulticastAddr(pDstMAC))
                        {
                            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Multicast Address");
                            return EDD_STS_ERR_PARAM; 
                        }
                    }
                    else if (EDD_CSRT_PROV_PROP_RTCLASS_UDP == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))  //UDP Provider
                    {
                        //Check for Multicast IP Address               
                        if(!EDDI_CRTIsIPMulticastAddr(pDstIP))
                        {
                            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Multicast Address");
                            return EDD_STS_ERR_PARAM; 
                        }
                    } 
                }
            }
        }
        else if (Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Legacy-Mode with RTC1 or RTC2 is not allowed");
            return EDD_STS_ERR_PARAM;  
        }
    }
    else if (EDD_CSRT_PROV_PROP_RTCLASS_3 == (Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
    {
        //************************************************
        //RTC3: DstMac has to be valid on ADD and ACTIVATE
        //************************************************
        //no legacy 
        if (!(Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            //no MC-CR
            if (Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR)
            {
                //Check for Multicast IP Address               
                if(!EDDI_CRTIsMACMulticastAddr(pDstMAC))
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Multicast Address");
                    return EDD_STS_ERR_PARAM; 
                } 
            }
        }
        else if (Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) //legacy  
        {
            //no MC-CR
            if (!(Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR))
            {
                //Check for Unicast MAC Address
                if(EDDI_CRTIsMACMulticastAddr(pDstMAC))
                {
                    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Not valid Unicast Address");
                    return EDD_STS_ERR_PARAM;   
                }
            }
            else if (Properties & EDD_CSRT_PROV_PROP_MODE_MC_CR)
            {
                EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderInit, ERROR: Legacy-Mode with MC-CR is not allowed");
                return EDD_STS_ERR_PARAM;  
            }
        }
    }
    
    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTIsIPMulticastAddr()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTIsIPMulticastAddr( const EDD_IP_ADR_TYPE  * const  pIP)
{
    //is IP-Adress valid
    if (  ((pIP->IPAdr[0] == 0x0)
        && (pIP->IPAdr[1] == 0x0)
        && (pIP->IPAdr[2] == 0x0)
        && (pIP->IPAdr[3] == 0x0))
        || (pIP->IPAdr[0] &  0xE0) )
    {
        return LSA_TRUE;   //Multicast
    }
    else
    {
        return LSA_FALSE;  //Unicast  
    } 
}                                                 
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTIsMACMulticastAddr()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTIsMACMulticastAddr( const EDD_MAC_ADR_TYPE * const  pMAC )
{
    if (pMAC->MacAdr[0] & 0x1) 
    {
         return LSA_TRUE;      //multicast
    }
    else
    {
        return LSA_FALSE;      //unicast
    } 
} 
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_GetMaxProviderParams()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      LSA_RESULT
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GetMaxProviderParams( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB, 
                                                            LSA_UINT32              *  const ProviderCyclePhaseMaxCnt_Real,
                                                            LSA_UINT32              *  const ProviderCyclePhaseMaxByteCnt_Real)
{                               
    LSA_UINT32  ProvMaxCnt;
    
    if (0 == pDDB->PM.PortMap.PortCnt)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_GetMaxProviderParams, illegalPortCnt:0x%X", pDDB->PM.PortMap.PortCnt);
        return EDD_STS_ERR_PARAM;
    }

    if /* base cycle < 1ms */
       (pDDB->CycCount.Entity < EDDI_CRT_CYCLE_BASE_FACTOR_1MS)
    {
        // for CycleBaseFactor = 1/2/4/8/16
        *ProviderCyclePhaseMaxByteCnt_Real = (pDDB->CycCount.Entity * EDDI_CRT_CYCLE_LENGTH_GRANULARITY * EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_MULTIPLIER * 10UL) / EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_DIVISION_FACTOR;
    }
    else
    {
        // for CycleBaseFactor = 32/64/128/256/512/1024
        *ProviderCyclePhaseMaxByteCnt_Real = EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_32 * EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_MULTIPLIER;
    }

    #if (EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT == 0)
    //user setting for "no checks"
    ProvMaxCnt = 0;
    
    #else
    //Calc max nr of providers per cycle, take the minimum of these limits:
    //Limit 1: SRT_FCW_Count: SRT_FCWs per Cycle, Nr has to be shared for all ports
    //Limit 2: pDDB->CRT.CfgPara.SRT.ProviderCyclePhaseMaxCnt provided by user
    //Limit 3: Theoretical max. per cycle based on 64 providers per 1ms
    ProvMaxCnt = EDDI_MIN((pDDB->SRT.SRT_FCW_Count / pDDB->PM.PortMap.PortCnt), (EDDI_CRT_PROVIDER_DEFAULT_PER_1MS * pDDB->CycCount.Entity)/EDDI_CRT_CYCLE_BASE_FACTOR_1MS);
    ProvMaxCnt = EDDI_MIN(ProvMaxCnt, pDDB->pConstValues->ProviderCyclePhaseMaxCnt);
    
    if (0 == ProvMaxCnt)
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_GetMaxProviderParams ERROR, CyclePhaseMaxCnt=0: In:ProviderCyclePhaseMaxCnt:%d; In:Entity:%d; Out:CyclePhaseMaxByteCnt:%d", 
                          pDDB->pConstValues->ProviderCyclePhaseMaxCnt, pDDB->CycCount.Entity, *ProviderCyclePhaseMaxByteCnt_Real);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    *ProviderCyclePhaseMaxCnt_Real = ProvMaxCnt;
    EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_GetMaxProviderParams, In:ProviderCyclePhaseMaxCnt:%d; Out:CyclePhaseMaxCnt:%d CyclePhaseMaxByteCnt:%d", 
                      pDDB->pConstValues->ProviderCyclePhaseMaxCnt, ProvMaxCnt, *ProviderCyclePhaseMaxByteCnt_Real);
                                        
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTSetImageMode()
 *
 *  Description: 
 *
 *  Arguments:
 *
 *  Return:      
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTSetImageMode( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB, 
                                                     void                  *  const pConsProv,
                                                     LSA_BOOL                 const bIsProv)
{                               
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTSetImageMode->"); 
    
    if (bIsProv)
    {
        EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider = (EDDI_CRT_PROVIDER_PTR_TYPE)pConsProv;
        //evaluate properties
        #if defined (EDDI_CFG_REV7)
        if (pProvider->usingPAEA_Ram)
        {
            pProvider->LowerParams.ImageMode  =
                (LSA_UINT8)(((pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC) ?
                            EDDI_IMAGE_MODE_SYNC:
                            EDDI_IMAGE_MODE_ASYNC);
        }
        else
        {
            pProvider->LowerParams.ImageMode  =
                (LSA_UINT8)(((pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC) ?
                            EDDI_IMAGE_MODE_SYNC:
                            EDDI_IMAGE_MODE_STAND_ALONE);
        }
        #elif defined (EDDI_CFG_REV6)
        pProvider->LowerParams.ImageMode  =
            (LSA_UINT8)(((pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC ) ?
                        EDDI_IMAGE_MODE_SYNC:
                        EDDI_IMAGE_MODE_STAND_ALONE);
        #else
        pProvider->LowerParams.ImageMode  =
            (LSA_UINT8)(((pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC) ?
                        EDDI_IMAGE_MODE_SYNC:
                        EDDI_IMAGE_MODE_ASYNC);
        #endif
    }
    else
    {
        EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer = (EDDI_CRT_CONSUMER_PTR_TYPE)pConsProv;
        
        #if defined (EDDI_CFG_REV7)
        if (pConsumer->usingPAEA_Ram)
        {
            pConsumer->LowerParams.ImageMode =
                (LSA_UINT8)(((pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC)?
                            EDDI_IMAGE_MODE_SYNC:
                            EDDI_IMAGE_MODE_ASYNC);
        }
        else
        {
            pConsumer->LowerParams.ImageMode =
                (LSA_UINT8)(((pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC)?
                            EDDI_IMAGE_MODE_SYNC:
                            EDDI_IMAGE_MODE_STAND_ALONE);
        }
        #elif defined (EDDI_CFG_REV6)
        pConsumer->LowerParams.ImageMode =
            (LSA_UINT8)(((pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC)?
                        EDDI_IMAGE_MODE_SYNC:
                        EDDI_IMAGE_MODE_STAND_ALONE);
        #else
        pConsumer->LowerParams.ImageMode =
            (LSA_UINT8)(((pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) == (LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC)?
                        EDDI_IMAGE_MODE_SYNC:
                        EDDI_IMAGE_MODE_ASYNC);
        #endif
    }
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTSetImageMode<-"); 
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_com.c                                               */
/*****************************************************************************/
