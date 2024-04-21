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
/*  F i l e               &F: eddi_crt_dfpprov.c                        :F&  */
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

#include "eddi_int.h"

#include "eddi_dev.h"

//#include "eddi_csrt_ext.h"
//#include "eddi_irt_ext.h"
#include "eddi_crt_ext.h"
//#include "eddi_ser_ext.h"

#include "eddi_crt_check.h"
//#include "eddi_ser_cmd.h"
//#include "eddi_crt_sb_sw_tree.h"
#include "eddi_crt_com.h"

#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_common.h"
//#include "eddi_prm_record_pdir_subframe_data.h"

//#include "eddi_time.h"

#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_CRT_DFPPROV
#define LTRC_ACT_MODUL_ID  113

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_DFP_ON)
/*--------------------------------------------------------------------------*
 * Local protos                                                             *
 *--------------------------------------------------------------------------*/
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvGetPF( EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                         EDDI_CRT_PROVIDER_PTR_TYPE          * const ppPFProvider,
                                                         EDDI_CONST_CRT_PROVIDER_PTR_TYPE      const pProvider );

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderCheckDGData( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                                      EDDI_CRT_PROVIDER_PTR_TYPE       const pProvider,
                                                                      EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE const pSubFrmHandler);

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_DFPProvBuildPackFrameInKRAM( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                      EDDI_CONST_CRT_PROVIDER_PTR_TYPE          const  pProvider,
                                                                      EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE  const  pParam,
                                                                      EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE          const  pSubFrmHandler);

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvActPFFrame(EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                             EDDI_CONST_CRT_PROVIDER_PTR_TYPE const pProvider,
                                                             EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE const pSubFrmHandler);

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvAddPFHandler(EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                               EDDI_CRT_PROVIDER_PTR_TYPE            const pProvider,
                                                               EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const pProvParam );
                                                      

/*--------------------------------------------------------------------------*
 * Providerhandling                                                         *
 *--------------------------------------------------------------------------*/
/*****************************************************************************
 *  Function:    EDDI_DFPProvAddCheckRQB()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
******************************************************************************/
//Checks if all conditions for a DFP frame are met
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ProviderType )
{
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;
    
    LSA_UNUSED_ARG(pDDB);

    if (   (EDDI_LIST_TYPE_FCW != ListType)
        || (EDDI_RTC3_PROVIDER != ProviderType)
        || (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) )
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProvAddCheckRQB, No DFP Provider: FrameID:0x%X, ListType:%d, ProviderType:%d, Properties:0x%X",
                          pProvParam->FrameID, ListType, ProviderType, pProvParam->Properties);
        return EDD_STS_ERR_PARAM;
    }
    
    if (   (pProvParam->DataLen < EDD_CSRT_DFP_DATALEN_MIN)
        || (pProvParam->DataLen > EDD_CSRT_DFP_DATALEN_MAX) )
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProvAddCheckRQB, invalid DataLen: FrameID:0x%X, DataLen:0x%X",
                          pProvParam->FrameID, pProvParam->DataLen);
        return EDD_STS_ERR_PARAM;
    }
    
    #if defined(EDDI_INTCFG_CONS_BUFFER_IF_3BHW)
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProvAddCheckRQB, 3BHW is not allowed");
    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
    return EDD_STS_ERR_PARAM;
    #endif


    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProvAddPFHandler()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
******************************************************************************/
//searches if a pProvider for a packframe with this FrameID has been set yet, if not, then add it
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvAddPFHandler( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                   EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                                   EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider;
    LSA_RESULT                  Result;

    //check if a PF has to be added
    //1. find PF by FrameID
    EDDI_CRTProviderListFindByFrameId(pDDB, &pDDB->CRT.ProviderList, &pDDB->CRT, pProvParam, EDDI_LIST_TYPE_FCW, &pPFProvider, LSA_FALSE /*bIsDFP*/);

    //2. Add PF if needed
    if (pPFProvider == EDDI_NULL_PTR)
    {
        EDD_RQB_TYPE                    RQBLocal;
        EDD_RQB_CSRT_PROVIDER_ADD_TYPE  ProvParamLocal;

        //first DG provider ==> add PF
        ProvParamLocal.FrameID                      = pProvParam->FrameID;
        ProvParamLocal.Properties                   = EDD_CSRT_PROV_PROP_RTCLASS_3;
        ProvParamLocal.DataLen                      = EDD_CSRT_DATALEN_MIN;   //will be changed before activate!
        ProvParamLocal.CycleReductionRatio          = pProvParam->CycleReductionRatio;
        ProvParamLocal.GroupID                      = 0;
        ProvParamLocal.CyclePhase                   = EDD_CYCLE_PHASE_UNDEFINED;
        ProvParamLocal.CyclePhaseSequence           = EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
        ProvParamLocal.UserID                       = 0xFFFFFFFF;
        ProvParamLocal.ProviderID                   = 0xCCCC;
        
        ProvParamLocal.IOParams.BufferProperties    = pProvParam->IOParams.BufferProperties;                  
        ProvParamLocal.IOParams.DataOffset          = EDD_DATAOFFSET_UNDEFINED;
        
        ProvParamLocal.IOParams.CWOffset            = EDD_DATAOFFSET_INVALID; 
 
        //Note: ProvParamLocal.DstMAC will be filled up in control activate
 
        RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&ProvParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_PROVIDER_ADD;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);
        
        Result = EDDI_CRTProviderListAddEntry(&RQBLocal, pDDB);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPProvAddPFHandler, ERROR: Adding PF failed. FrameID:0x%X, ProviderID:0x%X",
                              pProvider->LowerParams.FrameId, pProvider->ProviderId);
            return EDD_STS_ERR_PARAM;
        }
        else
        {
            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPProvAddPFHandler, PF Added. SFPos:%d, FrameID:0x%X, ProviderID:0x%X, PFProviderId:0x%X",
                              pProvider->LowerParams.SFPosition, pProvider->LowerParams.FrameId, pProvider->ProviderId, ProvParamLocal.ProviderID);
            
            //Get PFProvider by ProviderID
            if (EDD_STS_OK != EDDI_CRTProviderListGetEntry(pDDB, &pPFProvider, &pDDB->pLocal_CRT->ProviderList, ProvParamLocal.ProviderID))
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                                  "EDDI_DFPProvAddPFHandler, ERROR: Could not get pPFProvider, ProviderID:0x%X, pFProviderID:0x%X",
                                  pProvider->ProviderId, ProvParamLocal.ProviderID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
                EDDI_Excp("EDDI_DFPProvAddPFHandler, ERROR: Could not get pPFProvider", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, ProvParamLocal.ProviderID);
                return EDD_STS_ERR_PARAM;
            }

            //Init PFProvider
            pPFProvider->bIsPFProvider = LSA_TRUE;                              
            pPFProvider->DGActiveCtr  = 0;                            
            pPFProvider->DGPassiveCtr = 0;
            pPFProvider->GroupId        = EDD_CFG_CSRT_MAX_PROVIDER_GROUP; /* set to max+1, to skip the packframe in service EDD_SRV_SRT_PROVIDER_SET_STATE */
        }
    }
    else
    {
        //PF exists
        if (!pPFProvider->bIsPFProvider)
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                              "EDDI_DFPProvAddPFHandler, ERROR: Prov is no PF. FrameID:0x%X, ProviderID:0x%X pFProviderID:0x%X",
                              pProvider->LowerParams.FrameId, pProvider->ProviderId, pProvider->PFProviderId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
            EDDI_Excp("EDDI_DFPProvAddPFHandler, ERROR: Prov is no PF", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->PFProviderId);
            return EDD_STS_ERR_PARAM;
        }

        EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_DFPProvAddPFHandler, Adding DG, PF exists. SFPos:%d, FrameID:0x%X, ProviderID:0x%X, PFProviderId:0x%X, PassDGs:%d, ActDGs:%d",
                          pProvider->LowerParams.SFPosition, pProvider->LowerParams.FrameId, pProvider->ProviderId, pPFProvider->ProviderId, pPFProvider->DGPassiveCtr, pPFProvider->DGActiveCtr);

    }
    
    //check for underrun/overrun
    if (0xFF == pPFProvider->DGPassiveCtr)
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
            "EDDI_DFPProvAddPFHandler, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFProviderID:0x%X, FrameID:0x%X", 
                          pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr, pPFProvider->ProviderId, pPFProvider->LowerParams.FrameId);
        EDDI_Excp("EDDI_DFPProvAddPFHandler, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr);
    }
    
    pPFProvider->DGPassiveCtr++;
    pProvider->PFProviderId = pPFProvider->ProviderId;
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProvBuildPackFrameInKRAM()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 *
 *  Example:
 *
 *  HeaderChecksum (SFCRC16 = 0) 
 *  Datagramm 1: Header
 *  Datagramm 1: C_SDU
 *  Datagramm 1: CRC (SFCRC16 = 0)
 *  Datagramm 2: Header
 *  Datagramm 2: C_SDU
 *  Datagramm 2: CRC (SFCRC16 = 0)
 *  Datagramm 3: Header
 *  Datagramm 3: C_SDU
 *  Datagramm 3: CRC (SFCRC16 = 0)
 *  Datagramm 4: Header
 *  Datagramm 4: C_SDU
 *  Datagramm 4: CRC (SFCRC16 = 0)
 *  Terminator (SFEndDelimiter)
 ******************************************************************************/
//Build PAckframe in KRAM
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvBuildPackFrameInKRAM( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                         EDDI_CONST_CRT_PROVIDER_PTR_TYPE          const  pProvider,
                                                                         EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE  const  pParam,
                                                                         EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE          const  pSubFrmHandler )
{
    LSA_UINT8                              *  pBuffer;
    LSA_UINT8                              *  pSrc;
    LSA_UINT8                                 TempHeader[EDDI_DFP_PNIO_HEADER_SIZE];
    LSA_UINT8                                 TempIndex = 0;
    EDD_MAC_ADR_TYPE                          DstMAC;
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  const  pPFHandler                                = pSubFrmHandler->pPF;    
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE          pSFhandler                                = pPFHandler->pSF;
    EDDI_DFP_APDU_TYPE                        DfpApdu;
    LSA_UINT32                                PAEARAMMult;
               
                                                                   
    #if defined (EDDI_CFG_REV7)
    //Do no use "usingPAEA" here, as it might not be valid yet!
    if (pProvider->usingPAEA_Ram)
    {
        PAEARAMMult = 4;
        pBuffer = (LSA_UINT8*)(pDDB->CRT.PAEA_Para.PAEA_BaseAdr + (PAEARAMMult * (pParam->IOParams.DataOffset)));
    }
    else
    {
        PAEARAMMult = 1;
        pBuffer = (LSA_UINT8*)((LSA_UINT32)pDDB->pKRam + pParam->IOParams.DataOffset + sizeof(EDD_CSRT_APDU_STATUS_TYPE));
    }
    #else
    #error "not allowed"
    #endif  
    
    //Build the DA-MacAdress
    EDDI_SetMACAddrRTC3(pDDB, EDDI_PrmGetFrameDataProperties(pDDB, LSA_FALSE /*bIsRecordSet_B*/, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD), DstMAC.MacAdr, pPFHandler->FrameID, LSA_TRUE /*bIsDfp*/); //DA
    
    //Build Temp Header with SA, DA, Ethertype and FrameID to calc the header CRC checksum
    //Copy DA
    EDDI_MemCopy(TempHeader, DstMAC.MacAdr, EDD_MAC_ADDR_SIZE);
    //Copy SA          
    TempIndex += EDD_MAC_ADDR_SIZE;  
    EDDI_MemCopy(&TempHeader[TempIndex], pDDB->Glob.LLHandle.xRT.MACAddressSrc.MacAdr, EDD_MAC_ADDR_SIZE);
    
    //Copy Ethertype   
    TempIndex += EDD_MAC_ADDR_SIZE;
    TempHeader[TempIndex] = (LSA_UINT8)(EDDI_ETHERTYPE >> 8);
    TempIndex++;
    TempHeader[TempIndex] = (LSA_UINT8)(EDDI_ETHERTYPE & 0xFF);

    //Copy FrameID
    TempIndex++;
    TempHeader[TempIndex] = (LSA_UINT8)(pPFHandler->FrameID >> 8);
    TempIndex++;
    TempHeader[TempIndex] = (LSA_UINT8)(pPFHandler->FrameID & 0xFF);
    
    //Copy Header-CRC
    *pBuffer = 0; //2nd byte
    pBuffer += PAEARAMMult;
    
    *pBuffer = 0; //1st byte
    pBuffer += PAEARAMMult;
    
    while (pSFhandler)
    {
        DfpApdu.SFPosition   = pSFhandler->SFPosition;
        DfpApdu.SFLen        = pSFhandler->Len;
        DfpApdu.SFCycleCtr   = 0;
        DfpApdu.SFDataStatus = EDDI_DS_BIT7_SURROGAT;  //Bit 7 is set as Surrugat 
        pSrc = (LSA_UINT8 *)(LSA_VOID *)&DfpApdu;

        //Copy DFP APDU
        for (TempIndex = 0; TempIndex<sizeof(EDDI_DFP_APDU_TYPE); TempIndex++)
        {
            *pBuffer = *pSrc++;
            pBuffer += PAEARAMMult;
        }

        //Do nothing with C_SDU, because it's user specific data, do not touch
        pBuffer += (PAEARAMMult * pSFhandler->Len);
       
        //Set DG-CRC 
        *pBuffer = 0; //1st byte
        pBuffer += PAEARAMMult;
        *pBuffer = 0; //2nd byte
        pBuffer += PAEARAMMult;

        //Get next Element
        pSFhandler = pSFhandler->pNext;
    }
    
    //Copy Delimiter at the End
    *pBuffer = 0; //1st byte
    pBuffer += PAEARAMMult;
    *pBuffer = 0; //2nd byte
}
/*---------------------- end [subroutine] ---------------------------------*/

                                                      
/******************************************************************************
 *  Function:    EDDI_DFPProvActPFFrame()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for activating a packframe are met
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvActPFFrame( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                 EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                                 EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  const  pSubFrmHandler )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider;
    LSA_RESULT                  Result;          

    EDDI_DFPProvGetPF(pDDB, &pPFProvider, pProvider);
    
    //if the PF has active DGs, then it had been activated before 
    if (!pPFProvider->DGActiveCtr)
    {
        EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE              ParamLocal;
        EDDI_PRM_PF_DSCR_TYPE                  * const  pPFHandler = pSubFrmHandler->pPF;
        LSA_BOOL                                        Indicate = LSA_TRUE;
        EDD_RQB_TYPE                                    RQBLocal;

        ParamLocal.IOParams.DataOffset     = pProvider->LowerParams.DataOffset - pProvider->LowerParams.SFOffset;
        pPFProvider->LowerParams.ImageMode = pProvider->LowerParams.ImageMode;

        //Build PackFrame in KRAM
        EDDI_DFPProvBuildPackFrameInKRAM(pDDB, pProvider, &ParamLocal, pSubFrmHandler);
       
        //activate PF
        ParamLocal.ProviderID                   = pPFProvider->ProviderId;
        ParamLocal.Mode                         = EDD_PROV_CONTROL_MODE_ACTIVATE;
        
        //Copy params from first DG of this PackFrame!
        ParamLocal.CycleReductionRatio          = pProvider->LowerParams.CycleReductionRatio;
        ParamLocal.CyclePhase                   = pProvider->LowerParams.CyclePhase;
        ParamLocal.IOParams.BufferProperties    = pProvider->LowerParams.BufferProperties;

        ParamLocal.IOParams.CWOffset            = EDD_DATAOFFSET_INVALID;       

        //calc final framelength
        #if defined (EDDI_CFG_REV7)
        pPFProvider->LowerParams.DataLen = pPFHandler->FrameLen + ((EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))?sizeof(EDDI_CRT_DATA_APDU_STATUS):0);
        #else
        pPFProvider->LowerParams.DataLen = pPFHandler->FrameLen + sizeof(EDDI_CRT_DATA_APDU_STATUS); 
        #endif

        RQBLocal.pParam  = (EDD_UPPER_MEM_PTR_TYPE)&ParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_PROVIDER_CONTROL;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);

        Result = EDDI_CRTProviderControlEvent(pPFProvider, &RQBLocal, 0, pDDB, LSA_FALSE /*bUseAutoStop (irrelevant here)*/, &Indicate);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPProvActPFFrame, ERROR: EDDI_CRTProviderControlEvent failed. PFProviderId:0x%X Status:0x%X",
                              pPFProvider->ProviderId, Result);
            //Details have already been set
            return Result;
        }
        else
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPProvActPFFrame, PF activated. PFProviderId:0x%X Status:0x%X",
                              pPFProvider->ProviderId, Result);
        }
        
        //complete PF handler
        pPFHandler->ConsProvID = pPFProvider->ProviderId;

        pPFProvider->pSubFrmHandler = (EDDI_PRM_SF_DSCR_TYPE *)(void *)pPFHandler;
    }
    else
    {
        //check the DG-dataoffset against the dataoffset of the PF
        if (pPFProvider->LowerParams.DataOffset != (pProvider->LowerParams.DataOffset - pProvider->LowerParams.SFOffset))
        {
            EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPProvActPFFrame, ERROR: DataOffset does not match. ProviderId:0x%X, FrameID:0x%X, PFDataOffset:0x%X, DGDataOffset:0x%X, DGSFOffset:0x%X, DGSFPosition:0x%X",
                              pProvider->ProviderId, pProvider->LowerParams.FrameId, pPFProvider->LowerParams.DataOffset, pProvider->LowerParams.DataOffset, pProvider->LowerParams.SFOffset, pProvider->LowerParams.SFPosition);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
            return EDD_STS_ERR_PARAM;
        }
    }

    pPFProvider->DGActiveCtr++;
    pPFProvider->DGPassiveCtr--;
    EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_DFPProvActPFFrame, DG:%d PFFrameID:0x%X, PFProvID:0x%X, PassCtr:%d, ActCtr:%d", 
                      pProvider->LowerParams.SFPosition, pPFProvider->LowerParams.FrameId, pPFProvider->ProviderId, pPFProvider->DGPassiveCtr, pPFProvider->DGActiveCtr);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProvPassPFFrame()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for passivating a packframe are met, and passivate it
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvPassPFFrame( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                          EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider;
    LSA_RESULT                  Result;          
     
    EDDI_DFPProvGetPF(pDDB, &pPFProvider, pProvider);

    //check for underrun/overrun
    if (   (0 == pPFProvider->DGActiveCtr)
        || (0xFF == pPFProvider->DGPassiveCtr) )
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPProvPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFProviderID:0x%X, FrameID:0x%X", 
                          pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr, pPFProvider->ProviderId, pPFProvider->LowerParams.FrameId);
        EDDI_Excp("EDDI_DFPProvPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr);
    }

    pPFProvider->DGActiveCtr--;
    pPFProvider->DGPassiveCtr++;
    EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_DFPProvPassPFFrame, DG:%d PFFrameID:0x%X, PFProvID:0x%X, PassCtr:%d, ActCtr:%d", 
                      pProvider->LowerParams.SFPosition, pPFProvider->LowerParams.FrameId, pPFProvider->ProviderId, pPFProvider->DGPassiveCtr, pPFProvider->DGActiveCtr);

    //if the PF now has passive DGs only, then passivate it 
    if (!pPFProvider->DGActiveCtr)
    {
        EDD_RQB_TYPE                        RQBLocal;
        EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE  ParamLocal;
        LSA_BOOL                            Indicate = LSA_TRUE;

        //passivate PF
        ParamLocal.ProviderID = pPFProvider->ProviderId;
        ParamLocal.Mode       = EDD_PROV_CONTROL_MODE_PASSIVATE;

        RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&ParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_PROVIDER_CONTROL;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);

        Result = EDDI_CRTProviderControlEvent(pPFProvider, &RQBLocal, 0, pDDB, LSA_FALSE /*bUseAutoStop (irrelevant here)*/, &Indicate);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPProvPassPFFrame, ERROR: EDDI_CRTProviderControlEvent failed. PFProviderId:0x%X Status:0x%X",
                              pPFProvider->ProviderId, Result);
            return (Result);
        }
        else
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPProvPassPFFrame, PF passivated. PFProviderId:0x%X Status:0x%X",
                              pPFProvider->ProviderId, Result);
        }

        //Reset all parameters, they get inherited from 1st DG being activated
        pPFProvider->pSubFrmHandler                   = EDDI_NULL_PTR;
        pPFProvider->LowerParams.BufferProperties     = EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED;
        pPFProvider->LowerParams.CyclePhase           = EDD_CYCLE_PHASE_UNDEFINED;
        pPFProvider->LowerParams.CycleReductionRatio  = EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
        pPFProvider->LowerParams.DataOffset           = EDD_DATAOFFSET_UNDEFINED;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProvRemovePFHandler()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for removing a packframe are met
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvRemovePFHandler( EDD_UPPER_RQB_PTR_TYPE              const  pRQB,
                                                            EDDI_CONST_CRT_PROVIDER_PTR_TYPE    const  pProvider,
                                                            EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                            LSA_BOOL                         *  const  pIndicate )
{
    if (EDDI_IS_DFP_PROV(pProvider))
    {
        EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider;
        LSA_RESULT                  Status;          
    
        //Get PF Provider
        EDDI_DFPProvGetPF(pDDB, &pPFProvider, pProvider);
        
        //check for underrun/overrun
        if (   (0 == pPFProvider->DGPassiveCtr)
            || (pProvider->Status != EDDI_CRT_PROV_STS_INACTIVE) )
        {
            EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                "EDDI_DFPConsPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFProviderId:0x%X, FrameID:0x%X, Status:%d", 
                              pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr, pPFProvider->ProviderId, pPFProvider->LowerParams.FrameId, pPFProvider->Status);
            EDDI_Excp("EDDI_DFPConsPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFProvider->DGActiveCtr, pPFProvider->DGPassiveCtr);
        }
        
        pPFProvider->DGPassiveCtr--;   //DG gets removed from PF

        EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
            "EDDI_DFPProvRemovePFHandler, removing DG:%d from FrameID:0x%X, PFProvID:0x%X, PassCtr:%d, ActCtr:%d", 
            pProvider->LowerParams.SFPosition, pPFProvider->LowerParams.FrameId, pPFProvider->ProviderId, pPFProvider->DGPassiveCtr, pPFProvider->DGActiveCtr);

        if (    (0 == pPFProvider->DGActiveCtr)
            &&  (0 == pPFProvider->DGPassiveCtr) )
        {
            //remove PF
            Status = EDDI_CRTProviderRemoveEvent(pRQB, pPFProvider, pDDB, pIndicate); //Attention: contains EDDI_CRTProviderListUnReserveEntry!
            if (Status != EDD_STS_OK)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPProvRemovePFHandler, ERROR removing PF Provider, Status:0x%X, ProviderID:0x%X pFProviderID:0x%X", 
                                  Status, pProvider->ProviderId, pProvider->PFProviderId);
                return Status;
            }
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderInit()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//initializes a DFP-provider
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                       EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam)
{                                                               
    LSA_RESULT  Result;
    
    pProvider->pSubFrmHandler           = EDDI_NULL_PTR;  
    pProvider->LowerParams.SFPosition   = pProvParam->IOParams.SFPosition;
    pProvider->LowerParams.SFOffset     = pProvParam->IOParams.SFOffset;
    pProvider->DGActiveCtr              = EDDI_DFP_INVALID_DG_CTR;
    pProvider->DGPassiveCtr             = EDDI_DFP_INVALID_DG_CTR;
    pProvider->PFProviderId             = EDD_PROV_ID_INVALID;
    pProvider->bIsPFProvider            = LSA_FALSE;
    
    pProvider->LowerParams.DataOffset   = pProvParam->IOParams.DataOffset;
    pProvider->LowerParams.DataLen      = pProvParam->DataLen;       

    if (  (0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
       && (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC != (pProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) ) )
    {
       pProvider->usingPAEA_Ram = LSA_TRUE;
    }
    else
    {
       pProvider->usingPAEA_Ram = LSA_FALSE;
    }

    //check if a PF has to be added, or just enter the PF-ProvID
    Result = EDDI_DFPProvAddPFHandler(pDDB, pProvider, pProvParam);
    if (EDD_STS_OK != Result)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProviderInit, EDDI_DFPProvAddPFHandler failed. ProvID:0x%X, FrameID:0x%X",
                          pProvider->ProviderId,pProvParam->FrameID);
        return Result;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
   
   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDFPProviderSetDataStatus( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const  pProvider,
                                                                 LSA_UINT8                          const  DGDataStatus )
{
    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTDFPProvider DGDataStatus:0x%X", DGDataStatus);

    //Set only DataStaus if the DG is active
    if (EDDI_CRT_PROV_STS_ACTIVE == pProvider->Status)
    {
        //Set initial or precalculated DataStatus
        LSA_UINT8 * const pSFDataStatus = pProvider->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFDataStatus * (pProvider->usingPAEA_Ram?4:1));

        *pSFDataStatus = DGDataStatus;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CRTDFPProviderSetDataStatus<-");
    LSA_UNUSED_ARG(pDDB);
}                                                                         
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderCheckDGData()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks a subframe against the PDIRSubframeData prior to activation
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderCheckDGData(EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                                     EDDI_CRT_PROVIDER_PTR_TYPE        const pProvider,
                                                                     EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  const pSubFrmHandler)
{
    EDDI_PRM_PF_DSCR_TYPE * const pPFHandler = pSubFrmHandler->pPF;
    LSA_UINT32                    iDataElem;
    LSA_UINT32                    iDataElemCount = 0;
    
    
    if (!pPFHandler->bIsProv)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPProviderCheckDGData, no provider. ProviderID:0x%X, FrameID:0x%X",
                          pProvider->ProviderId, pProvider->LowerParams.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_HandlerType);                           
        return EDD_STS_ERR_PARAM;
    }

    if (pSubFrmHandler->UsedByUpper)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPProviderCheckDGData, LSA_FALSE |= UsedByUpper. ProviderID:0x%X, FrameID:0x%X",
                          pProvider->ProviderId, pProvider->LowerParams.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_UsedByUpper);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!(EDDI_NULL_PTR == pProvider->pSubFrmHandler))
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPProviderCheckDGData, no EDDI_NULL_PTR, ProviderID:0x%X, FrameID:0x%X, pSubFrmHandler 0x%X", 
                          pProvider->ProviderId, pProvider->LowerParams.FrameId, pProvider->pSubFrmHandler);
        EDDI_Excp("EDDI_CRTProviderCheckData, pProvider->pFrmHandler", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->LowerParams.FrameId);
        return EDD_STS_ERR_EXCP;
    }

    if (pSubFrmHandler->Len != pProvider->LowerParams.DataLen)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPProviderCheckDGData, DataLength -> pProvider->LowerParams.DataLen:0x%X pFrmHandler->pIrFrameDataElem[iElem]->DataLength:0x%X",
                          pProvider->LowerParams.DataLen, pSubFrmHandler->Len);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDataLen);
        return EDD_STS_ERR_PARAM;
    }

    for (iDataElem = 0; iDataElem < pDDB->PM.PortMap.PortCnt; iDataElem++)
    {        
        if (EDDI_NULL_PTR == pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem])
        {
            //do nothing
            continue;
        }
        
        if (pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->FrameID == pProvider->LowerParams.FrameId)
        {
            if (pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->ReductionRatio != pProvider->LowerParams.CycleReductionRatio)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPProviderCheckDGData, ReductionRatio -> pProvider->LowerParams.CycleReductionRatio:0x%X pFrmHandler->pIrFrameDataElem[0x%X]->ReductionRatio:0x%X",
                                  pProvider->LowerParams.CycleReductionRatio, iDataElem, pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->ReductionRatio);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                return EDD_STS_ERR_PARAM;
            }

            //non-legacy providers take their phase directly from the PDIRData
            pProvider->LowerParams.CyclePhase = pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->Phase;

            if (pPFHandler->FrameID != pProvider->LowerParams.FrameId)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPProviderCheckDGData, FrameId -> pProvider->LowerParams.FrameId:0x%X pFrmHandler->pIrFrameDataElem[0x%X]->FrameID:0x%X",
                                  pProvider->LowerParams.FrameId, iDataElem, pPFHandler->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
                return EDD_STS_ERR_PARAM;
            }
            
            iDataElemCount++;
        }
    }

    if (iDataElemCount == 0)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPProviderCheckDGData, No pFrmHandler->pIrFrameDataElem with same FrameID was found, pProvider->LowerParams.FrameId:0x%X pFrmHandler->pIrFrameDataElem[iElem]->FrameID:0x%X",
                          pProvider->LowerParams.FrameId, pPFHandler->FrameID);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderPassivate()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//Passivate a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    //deactivate Provider
    //remove bindings to subframehandler
    if (!pProvider->pSubFrmHandler)
    {
        //UsedByUpper does not have to be checked (is always paired with pFrmHandler)
        //no framehandler present.
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_DFPProviderPassivate, EDDI_NULL_PTR, pProvider->pSubFrmHandler, ProvID:0x%X", 
                          pProvider->ProviderId);
        return EDD_STS_ERR_PARAM;
    }
        
    pProvider->pSubFrmHandler->UsedByUpper = LSA_FALSE;
    pProvider->pSubFrmHandler              = EDDI_NULL_PTR;
    pProvider->Status                      = EDDI_CRT_PROV_STS_INACTIVE;    // no decrement of ActiveProvRTCxx (DFP)
    
    EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_DFPProviderPassivate, SubFrameHandler FREED: ProvId:0x%X DG/FrameId:%d/0x%X pSubFrmHandler:0x%X", 
                      pProvider->ProviderId, pProvider->LowerParams.SFPosition, pProvider->LowerParams.FrameId, (LSA_UINT32)pProvider->pSubFrmHandler);

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderCheckBufferProperties()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//check if the buffer properties of DGProvider and PFProvider match
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                         EDDI_CONST_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                                         EDDI_CRT_PROVIDER_PARAM_TYPE const * const  pLowerParamsLocal)
{
    if (EDDI_IS_DFP_PROV(pProvider))
    {
        EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider;
    
        EDDI_DFPProvGetPF(pDDB, &pPFProvider, pProvider);

        //if the PFProvider has properties already, they have to match the properties of the DGProvider. 
        if (   (   ((pPFProvider->LowerParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK) != EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED)
            && (pPFProvider->LowerParams.BufferProperties != pLowerParamsLocal->BufferProperties))
            || (   (pPFProvider->LowerParams.CycleReductionRatio != EDD_CYCLE_REDUCTION_RATIO_UNDEFINED)
                && (pPFProvider->LowerParams.CycleReductionRatio != pLowerParamsLocal->CycleReductionRatio))
            || (   (pPFProvider->LowerParams.CyclePhase          != EDD_CYCLE_PHASE_UNDEFINED)
                && (pPFProvider->LowerParams.CyclePhase          != pLowerParamsLocal->CyclePhase)))
        {
            EDDI_CRT_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPCheckBufferProperties, ERROR: Properties/CRR/CyclePhase do not match, ProviderID:0x%X, Prop:0x%X/CRR:0x%X/CyclePhase:0x%X / pFProviderID:0x%X, Prop:0x%X/CRR:0x%X/CyclePhase:0x%X",
                              pProvider->ProviderId,         pLowerParamsLocal->BufferProperties,       pLowerParamsLocal->CycleReductionRatio,       pLowerParamsLocal->CyclePhase,
                              pProvider->PFProviderId, pPFProvider->LowerParams.BufferProperties, pPFProvider->LowerParams.CycleReductionRatio, pPFProvider->LowerParams.CyclePhase);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderActivate()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if provider is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//activates a DG provider
LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{                                                         
    LSA_RESULT                             Status;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE const pSubFrmHandler = EDDI_SyncIrFindSubFrameHandler(pDDB, (LSA_VOID *)pProvider, LSA_TRUE /*bIsProv*/);

    if (EDDI_NULL_PTR == pSubFrmHandler)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPProviderActivate, SubFrameHandler not found. ProviderID:0x%X", pProvider->ProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
        return EDD_STS_ERR_SEQUENCE;
    }
    else
    {
        //SFIRDATA found and valid!
        Status = EDDI_DFPProviderCheckDGData(pDDB, pProvider, pSubFrmHandler);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPProviderActivate, SFHandler does not match., ProviderID:0x%X", pProvider->ProviderId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
            return EDD_STS_ERR_PARAM;
        }
        else
        {                    
            //set buffer-data for DFP-Provider
            EDDI_CRTSetImageMode(pDDB, pProvider, LSA_TRUE /*bIsProv*/);

            //if this is the first DG of a PF being activated, activate the PF also 
            Status =  EDDI_DFPProvActPFFrame(pDDB, pProvider, pSubFrmHandler);
            if(EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPProviderActivate, illegal EDDI_DFPProvActPFFrame, ProviderID:0x%X", pProvider->ProviderId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                return Status;
            }
            
            pProvider->pSubFrmHandler   = pSubFrmHandler;
            pSubFrmHandler->UsedByUpper = LSA_TRUE;  //mark framehandler as used and initialized 

            #if defined (EDDI_CFG_REV7)
            if (pProvider->usingPAEA_Ram)
            {
                //PAEARAM used
                pProvider->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE) pDDB->CRT.PAEA_Para.PAEA_BaseAdr + (4 *(pProvider->LowerParams.DataOffset - sizeof(EDDI_DFP_APDU_TYPE)));
            }
            else
            {
                //no PAEARAM or SYNC Provider
                pProvider->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(LSA_UINT32)pDDB->pKRam + pProvider->LowerParams.DataOffset - sizeof(EDDI_DFP_APDU_TYPE);
            }
            #else
            #error "not allowed here"
            #endif
            
            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPProviderActivate, FrameHandler USED, ProvId:0x%X DG/FrameId:%d/0x%X pFrmHandler:0x%X", 
                              pProvider->ProviderId, pProvider->LowerParams.SFPosition, pProvider->LowerParams.FrameId, (LSA_UINT32)pSubFrmHandler);
        }
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProvGetPF()
 *
 *  Arguments:
 *
 *  Return:
 ******************************************************************************/
//Get the PFProv for a DGProv
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvGetPF(EDDI_LOCAL_DDB_PTR_TYPE             const pDDB,
                                                        EDDI_CRT_PROVIDER_PTR_TYPE        * const ppPFProvider,
                                                        EDDI_CONST_CRT_PROVIDER_PTR_TYPE    const pProvider )
{
    EDDI_CRT_PROVIDER_PTR_TYPE  pPFProvider = LSA_NULL;
    LSA_RESULT                  Status;
    
    Status = EDDI_CRTProviderListGetEntry(pDDB, &pPFProvider, &pDDB->pLocal_CRT->ProviderList, pProvider->PFProviderId);
    
    //Get PF Provider
    if (EDD_STS_OK != Status)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPProvGetPF, ERROR: Could not get pProvider, ProviderID:0x%X pFProviderID:0x%X",
                          pProvider->ProviderId, pProvider->PFProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
        EDDI_Excp("EDDI_DFPProvGetPF, ERROR: Could not get pProvider", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->PFProviderId);
    }

    if (!pPFProvider->bIsPFProvider)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPProvGetPF, ERROR: Prov is no PF. ProviderID:0x%X pFProviderID:0x%X",
                          pProvider->ProviderId, pProvider->PFProviderId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);
        EDDI_Excp("EDDI_DFPProvGetPF, ERROR: Prov is no PF", EDDI_FATAL_ERR_EXCP, pProvider->ProviderId, pProvider->PFProviderId);
    }
    
    *ppPFProvider = pPFProvider;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_DFPProviderSetActivity()
 *
 *  Arguments:
 *
 *  Return:
 ******************************************************************************/
LSA_VOID EDDI_DFPProviderSetActivity(EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                     EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const pProvider,
                                     LSA_BOOL                           const bActivate ) 
{
    LSA_UINT8 * const pSFDataStatus = pProvider->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFDataStatus * (pProvider->usingPAEA_Ram?4:1));

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DFPProviderSetActivity->");
    
    if (bActivate)
    {
        //Set initial or precalculated DataStatus
        LSA_UINT8 const GroupDataStatus = pDDB->pLocal_CRT->ProviderList.GroupDataStatus[pProvider->GroupId];
        LSA_UINT8 const MergedDataStatus = (LSA_UINT8)(((GroupDataStatus | pProvider->LocalDataStatus) & EDD_CSRT_DSTAT_WIN_MASK) | (GroupDataStatus & pProvider->LocalDataStatus));

        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_DFPProviderSetActivity, DG.DS FrameID/SFPos:0x%X/%d 0x%X->0x%X",
            pProvider->LowerParams.FrameId, pProvider->LowerParams.SFPosition, *pSFDataStatus, MergedDataStatus);

        *pSFDataStatus = MergedDataStatus;
    }
    else
    {
        //Set DG as surrogat
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_DFPProviderSetActivity, DG.DS FrameID/SFPos:0x%X/%d 0x%X->SURR",
            pProvider->LowerParams.FrameId, pProvider->LowerParams.SFPosition, *pSFDataStatus);
        *pSFDataStatus = EDDI_DS_BIT7_SURROGAT;
    }
           
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DFPProviderSetActivity<-");
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPProviderSetActivity()
 *
 *  Arguments:
 *
 *  Return:
 ******************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                      LSA_UINT8                       * const  pSFDataStatus)
{
    LSA_UINT8 * const pSFDataStatusLocal = pProvider->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFDataStatus * (pProvider->usingPAEA_Ram?4:1));

    LSA_UNUSED_ARG(pDDB);

    *pSFDataStatus = *pSFDataStatusLocal;
}                                                      
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerIsPF()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if consumer is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//checks if a consumer is a packframe
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderIsPF( EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    return (pProvider->bIsPFProvider);
}
/*---------------------- end [subroutine] ---------------------------------*/


#else //if defined (EDDI_CFG_DFP_ON)

/* #########################################################################################################*/
/* Dummy-Functions                                                                                          */
/* #########################################################################################################*/
//Checks if all conditions for a DFP frame are met
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ProviderType )
{
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;
    
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(ListType);
    LSA_UNUSED_ARG(ProviderType);

    if (EDDI_IS_DFP_PROV(pProvParam))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProvAddCheckRQB, DFP not supported. FrameID:0x%X, Properties:0x%X",
                          pProvParam->FrameID, pProvParam->Properties);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//checks if the conditions for passivating a packframe are met, and passivate it
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvPassPFFrame(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                       EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    LSA_UNUSED_ARG(pDDB);

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        return EDD_STS_ERR_PARAM;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//checks if the conditions for removing a packframe are met
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvRemovePFHandler( EDD_UPPER_RQB_PTR_TYPE              const  pRQB,
                                                            EDDI_CONST_CRT_PROVIDER_PTR_TYPE    const  pProvider,
                                                            EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                            LSA_BOOL                         *  const  pIndicate )
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIndicate);
    LSA_UNUSED_ARG(pRQB);

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        return EDD_STS_ERR_PARAM;
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//initializes a DFP-provider
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                       EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam)
{                                                               
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);
    LSA_UNUSED_ARG(pProvParam);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/
    
//Passivate a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

//activates a DG provider
LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{                                                         
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);         

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

//activates a DG provider
LSA_VOID EDDI_DFPProviderSetActivity(EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                     EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const pPFProvider,
                                     LSA_BOOL                           const bActivate ) 
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pPFProvider);
    LSA_UNUSED_ARG(bActivate);
}
/*---------------------- end [subroutine] ---------------------------------*/

//check if the buffer properties of DGProvider and PFProvider match
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                         EDDI_CONST_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                                         EDDI_CRT_PROVIDER_PARAM_TYPE const * const  pLowerParamsLocal)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pLowerParamsLocal);

    if (EDDI_IS_DFP_PROV(pProvider))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPProviderCheckBufferProperties, DFP not supported. FrameID:0x%X, Properties:0x%X",
                          pProvider->LowerParams.FrameId, pProvider->Properties);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDFPProviderSetDataStatus( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const  pProvider,
                                                                 LSA_UINT8                          const  DGDataStatus )
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);
    LSA_UNUSED_ARG(DGDataStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                      LSA_UINT8                       * const  pSFDataStatus)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pProvider);
    LSA_UNUSED_ARG(pSFDataStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPCheckFrameHandlerPF()
 *
 *  Arguments:
 *
 *  Return:     
 ******************************************************************************/
LSA_VOID EDDI_DFPCheckFrameHandlerPF( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                      LSA_UINT16                     const  ProvConsId,
                                      EDDI_IRT_FRM_HANDLER_PTR_TYPE      *  pFrmHandler)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(ProvConsId);
    LSA_UNUSED_ARG(pFrmHandler);
}
/*---------------------- end [subroutine] ---------------------------------*/

                                             
/******************************************************************************
 *  Function:    EDDI_DFPConsumerIsPF()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if consumer is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//checks if a consumer is a packframe
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderIsPF( EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider )
{
    LSA_UNUSED_ARG(pProvider);
    return LSA_FALSE;
}

#endif //if defined (EDDI_CFG_DFP_ON)


/*****************************************************************************/
/*  end of file eddi_crt_dfpprov.c                                           */
/*****************************************************************************/

