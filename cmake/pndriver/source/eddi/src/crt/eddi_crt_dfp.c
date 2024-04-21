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
/*  F i l e               &F: eddi_crt_dfp.c                            :F&  */
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
/*  10.05.2011  A.H.  Removed CRC Calculation for HCS                        */
/*                    (PNIO Version 05.01.00.00_00.01.07.01)                 */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

//#include "eddi_csrt_ext.h"
//#include "eddi_irt_ext.h"
//#include "eddi_crt_ext.h"
//#include "eddi_ser_ext.h"

//#include "eddi_crt_check.h"
//#include "eddi_ser_cmd.h"
//#include "eddi_crt_sb_sw_tree.h"
#include "eddi_crt_com.h"

#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_common.h"

#include "eddi_crt_dfp.h"

#include "eddi_pool.h"
#include "eddi_prm_record_pdir_subframe_data.h"

#include "eddi_prm_state.h"
 
#define EDDI_MODULE_ID     M_ID_CRT_DFPCOM
#define LTRC_ACT_MODUL_ID  115

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_DFP_ON)
static LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_DFPInitPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB,
                                                                    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE                    pPackFrmHandler,
                                                                    EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE  const  pPackFrmData );
                                                                    
static LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_DFPInitSubFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                   EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE         pSubFrmHandler,
                                                                   LSA_UINT32                               SubframeData);
   
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPCalcPackFrameLength( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                   EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE         pPackFrmHandler );
                                                                   
                                                                  
/*****************************************************************************
 *  Function:    EDDI_SyncIrFindSubFrameHandler()
 *
 *  Arguments:
 *
 *  Return:      EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE
******************************************************************************/  
EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  EDDI_LOCAL_FCT_ATTR EDDI_SyncIrFindSubFrameHandler(       EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                                      const LSA_VOID *                  const  pConsProv,
                                                                                            LSA_BOOL                    const  bIsProv)
{
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  pLocalPackFrmData;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE   pLocalSubFrmData;
    LSA_UINT16                         const FrameID    = bIsProv?((EDDI_CRT_PROVIDER_TYPE *)pConsProv)->LowerParams.FrameId:((EDDI_CRT_CONSUMER_TYPE *)pConsProv)->LowerParams.FrameId;
    LSA_UINT16                         const SFPosition = bIsProv?((EDDI_CRT_PROVIDER_TYPE *)pConsProv)->LowerParams.SFPosition:((EDDI_CRT_CONSUMER_TYPE *)pConsProv)->LowerParams.SFPosition;
    LSA_UINT32                         const SFOffset   = bIsProv?((EDDI_CRT_PROVIDER_TYPE *)pConsProv)->LowerParams.SFOffset:((EDDI_CRT_CONSUMER_TYPE *)pConsProv)->LowerParams.SFOffset;
    LSA_UINT32                               SFOffsetCalc = sizeof(LSA_UINT16)+sizeof(EDDI_DFP_APDU_TYPE); //HdrCRC+sizeofDGAPDUSTatus
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncIrFindSubFrameHandler->");

    //Get at first the PackFrameHandler
    pLocalPackFrmData = EDDI_DFPFindPackFrameHandler(pDDB, FrameID);
    
    if (pLocalPackFrmData)
    {
        pLocalSubFrmData = pLocalPackFrmData->pSF;
        while (pLocalSubFrmData)
        {
            if (pLocalSubFrmData->SFPosition == SFPosition) 
            {
                if (pLocalPackFrmData->bIsProv == bIsProv)
                {
                    if (SFOffset != SFOffsetCalc)
                    {
                        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrFindSubFrameHandler, SFOffset:0x%X and SFOffsetCalc:0x%X is not the same", SFOffset, SFOffsetCalc);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPSFOffset);
                        return EDDI_NULL_PTR;
                    }
                    return (pLocalSubFrmData);
                }
                else
                {
                    EDDI_SYNC_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncIrFindSubFrameHandler, ProvID:0x%X, SFPosition:0x%X, SFOffset:0x%X, SFOffsetCalc:0x%X is not the same", pLocalPackFrmData->ConsProvID,SFPosition,SFOffset,SFOffsetCalc);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPProvConsID);
                    return EDDI_NULL_PTR;
                }
            }
            SFOffsetCalc = SFOffsetCalc + pLocalSubFrmData->Len + sizeof(LSA_UINT16)+sizeof(EDDI_DFP_APDU_TYPE);
            pLocalSubFrmData = pLocalSubFrmData->pNext;        
        }
    }
    EDDI_SYNC_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncIrFindSubFrameHandler, No SyncIrFindSubFrameHandler found (bIsProv:0x%X), FrameID:0x%X, SFPosition:0x%X, SFOffset:0x%X, SFOffsetCalc:0x%X", bIsProv, FrameID, SFPosition, SFOffset, SFOffsetCalc);
    return EDDI_NULL_PTR;                                                                                  
}
/*---------------------- end [subroutine] ---------------------------------*/
                                                                                      
                                                              
/*****************************************************************************
 *  Function:    EDDI_CRTDFPFindSubFrameHandler()
 *
 *  Arguments:
 *
 *  Return:      EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE
******************************************************************************/
EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  EDDI_LOCAL_FCT_ATTR   EDDI_DFPFindPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                                       LSA_UINT16                             const  FrameId)
{
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  pLocalPackFrmData = (EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE)pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pFirstPackFrame;
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPFindPackFrameHandler->");
    
    if (pLocalPackFrmData)
    {
        do
        {
            if (FrameId == pLocalPackFrmData->FrameID)
            {
                return pLocalPackFrmData;
            }
            
            //Get the Next one
            pLocalPackFrmData = pLocalPackFrmData->pNext;
        }
        while (pLocalPackFrmData);

        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_DFPFindPackFrameHandler, No PackFrameHandler found (FrameId:0x%X)", FrameId);
    }
    else
    {
        EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_DFPFindPackFrameHandler, pFirstPackFrame is 0(FrameId:0x%X)", FrameId);
    }

    return EDDI_NULL_PTR;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPBuildAllPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                   EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  const  pRecordSet )
{
    LSA_UINT16                                          PackFrameIndex;
    LSA_UINT16                                          iDGElement;
    LSA_UINT32                                          SubframeDataCount;                   
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE                   pPackFrmData            = EDDI_NULL_PTR;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE                    pDGFrmData              = EDDI_NULL_PTR;
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE                   pLastPackFrmData        = EDDI_NULL_PTR;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE                    pLastDGFrmData          = EDDI_NULL_PTR;
    EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE        pSubframeBlock          = EDDI_NULL_PTR;
    LSA_UINT16                                   const  NumberOfSubframeBlocks  = pRecordSet->pLocalPDirSubframeDataRecord->NumberOfSubframeBlocks;
    LSA_UINT32                                          PackFrameCount[2]       = {0, 0};
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                       pLocalFrmHandler        = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pFirst;
    LSA_UINT32                                          iDataElemCount          = 0;         
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DFPBuildAllPackFrameHandler->");
           
    //Get First PackFrmData
    pSubframeBlock = pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord->SubframeBlock;

    //1. Check first for max Packframes (In and Out)
    if (NumberOfSubframeBlocks > (2 * EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL))  // x * 2 for in and out frames
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPBuildAllPackFrameHandler, More Subframes was available in PRM Data (given Subframes:0x%X ) and Max (Subframes: 0x%X)", NumberOfSubframeBlocks, (2 * EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL));
        return EDD_STS_ERR_PRM_CONSISTENCY; 
    }

    for (PackFrameIndex = 0; PackFrameIndex < NumberOfSubframeBlocks; PackFrameIndex++)  //PackFrames ++
    {     
        if (PackFrameCount[0] > EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, EDDI supported Max: 0x%X DFP-Consumer. (given: 0x%X)", EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL, PackFrameCount[0]);
            return EDD_STS_ERR_PRM_CONSISTENCY; 
        }
        
        if (PackFrameCount[1] > EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, EDDI supported Max: 0x%X DFP-Provider. (given: 0x%X)", EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL, PackFrameCount[1]);
            return EDD_STS_ERR_PRM_CONSISTENCY; 
        }

        pLocalFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pFirst;

        while (pLocalFrmHandler)
        {
            EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pLocalFrmHandler->pIrFrameDataElem[0];

            if (pFrmData)
            {                                      
                if (pLocalFrmHandler->FrameId == pSubframeBlock->FrameID)
                {                
                    iDataElemCount++;
                    
                    if (EDDI_SYNC_IRT_CONSUMER == pLocalFrmHandler->HandlerType)
                    {
                        PackFrameCount[0]++;
                        break;
                    }
                    
                    if (EDDI_SYNC_IRT_PROVIDER == pLocalFrmHandler->HandlerType)
                    {
                        PackFrameCount[1]++;
                        break;
                    }                               
                } 
            } 
            pLocalFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(LSA_VOID *)pLocalFrmHandler->QueueLink.next_ptr; 
        }

        // Get Next Element
        pSubframeBlock = (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)(((LSA_UINT8 *)(void *)pSubframeBlock) + pSubframeBlock->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH);   
    }
       
    if (iDataElemCount != NumberOfSubframeBlocks)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPBuildAllPackFrameHandler, NumberOfSubframeBlocks is wrong (given NumberOfSubframeBlocks:0x%X ) and Count (NumberOfSubframeBlocks: 0x%X)", NumberOfSubframeBlocks, iDataElemCount);
        return EDD_STS_ERR_PRM_CONSISTENCY; 
    }

    pSubframeBlock = pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord->SubframeBlock;

    //2. Build DFP-Packframes and DG's
    for (PackFrameIndex = 0; PackFrameIndex < NumberOfSubframeBlocks; PackFrameIndex++)   //PackFrames ++
    {     
        //Ignore FrameID x+1 (MRPD with DFP)
        if (   (pSubframeBlock->FrameID >= EDD_SRT_FRAMEID_IRT_REDUNDANT_START) 
            && (pSubframeBlock->FrameID <= EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP)
            && (0 != (pSubframeBlock->FrameID & 1))
            )
        {
            //do nothing  
        }
        else
        {
            //Setup new FrameHandler       
            EDDI_MemGetPoolBuffer(pRecordSet->hPackFrameHandler, (LSA_VOID * *)&pPackFrmData);
            if (!pPackFrmData)
            {
                EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPBuildAllPackFrameHandler, No more Memory for FrameHandler");
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TooManyElements);
                return EDD_STS_ERR_PRM_BLOCK;
            }

            if (pLastPackFrmData)
            {
                //Save Last Packframe as next one
                pLastPackFrmData->pNext = pPackFrmData;
            }

            //Save Last Packframe Ptr
            pLastPackFrmData = pPackFrmData;

            //Init FrameHandler
            if (!EDDI_DFPInitPackFrameHandler(pDDB, pPackFrmData, pSubframeBlock))
            {
                // Free  in Pool
                EDDI_MemFreePoolBuffer(pRecordSet->hPackFrameHandler, (LSA_VOID *)pPackFrmData);
                return EDD_STS_ERR_PRM_BLOCK;
            }

             //Calc SubframeData
            SubframeDataCount =  EDDI_PrmCalcSubframeCount(pSubframeBlock);
             
            //Add 1st element as anchor
            if (0 == PackFrameIndex )
            {
                pRecordSet->pFirstPackFrame = pPackFrmData;
            }
         
            pLastDGFrmData = EDDI_NULL_PTR;
            
            //Get DG and add it 
            for (iDGElement = 0; iDGElement < SubframeDataCount; iDGElement++)   //DG = SubFrames ++
            {
                //Setup new FrameHandler
                EDDI_MemGetPoolBuffer(pRecordSet->hSubFrameHandler, (LSA_VOID * *)&pDGFrmData);
                if (!pDGFrmData)
                {
                    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPBuildAllPackFrameHandler, No more Memory for SubFrameHandler");
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TooManyElements);
                    return EDD_STS_ERR_PRM_BLOCK;
                }

                if (0 == iDGElement)
                {
                    //Set Link to 1st Subframe (DG)
                    pPackFrmData->pSF = pDGFrmData; 
                }
                                    
                //Save Last DG as next one
                if (pLastDGFrmData)
                {
                    pLastDGFrmData->pNext = pDGFrmData;
                }

                //Save Last DG Ptr
                pLastDGFrmData = pDGFrmData;

                //Init FrameHandler
                if (!EDDI_DFPInitSubFrameHandler(pDDB, pDGFrmData, pSubframeBlock->SubframeData[iDGElement]))
                {
                    // Free  in Pool
                    EDDI_MemFreePoolBuffer(pRecordSet->hSubFrameHandler, (LSA_VOID *)pDGFrmData);
                    return EDD_STS_ERR_PRM_BLOCK;
                }               
                pDGFrmData->pPF = pPackFrmData; 
            }
           
            if (pDGFrmData)
            {
                //Delete next Pointer
                pDGFrmData->pNext = EDDI_NULL_PTR;
            }
            
            //Calc PackFrameLength
            EDDI_DFPCalcPackFrameLength(pDDB, pLastPackFrmData);
        }
        // Get Next Element
        pSubframeBlock = (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)(((LSA_UINT8 *)(void *)pSubframeBlock) + pSubframeBlock->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH);   
    }
    
    if (pPackFrmData)
    {
        //Set pPackFrmData at last
        pPackFrmData->pNext = EDDI_NULL_PTR;
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DFPBuildAllPackFrameHandler<-");

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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPPackFrameHandlerCleanUp( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  const  pRecordSet )
{
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  pPackFrmData      = pRecordSet->pFirstPackFrame; //Get 1st PackFrame
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  pSavePackFrmData  = pRecordSet->pFirstPackFrame;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE   pDGFrmData        = EDDI_NULL_PTR; 
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE   pSaveDGFrmData    = EDDI_NULL_PTR; 

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPPackFrameHandlerCleanUp->");
           
    while (pPackFrmData)
    {
        //Get the 1st SubFrame in PackframeData
        pDGFrmData = pPackFrmData->pSF;
        
        while (pDGFrmData)
        {     
            //Free in Pool
            pSaveDGFrmData = pDGFrmData->pNext;
            EDDI_MemFreePoolBuffer(pRecordSet->hSubFrameHandler, (LSA_VOID *)pDGFrmData);    
            pDGFrmData = pSaveDGFrmData;
        }
        
        //Free  in Pool
        pSavePackFrmData = pPackFrmData->pNext;
        EDDI_MemFreePoolBuffer(pRecordSet->hPackFrameHandler, (LSA_VOID *)pPackFrmData);
        pPackFrmData = pSavePackFrmData;  
    }
   
    pRecordSet->pFirstPackFrame  = EDDI_NULL_PTR;
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DFPPackFrameHandlerCleanUp<-");
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPdirPFDataInitComponent()                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDFPDataInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32            maxSubframeLength;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PrmPdirPFDataInitComponent");

    maxSubframeLength = EDDI_PrmCalculateMaxSubframeLength(pDDB);

    pDDB->PRM.PDIRSubFrameData.pRecordSet_A = &pDDB->PRM.PDIRSubFrameData.RecordSet_A;
    pDDB->PRM.PDIRSubFrameData.pRecordSet_B = &pDDB->PRM.PDIRSubFrameData.RecordSet_B;

    //******************************************************************************************************************
    // Alloc PF and SF Structures
    //******************************************************************************************************************
    
    //************************************************************************************
    // PackFrame  
    // hPackFrameHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hPackFrameHandler = 0x0UL;
    (LSA_VOID)EDDI_MemIniPool( EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL * 2,            // x * 2 = for in and out frames 
                           (LSA_UINT32)sizeof(EDDI_PRM_PF_DSCR_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hPackFrameHandler ); // OUT*/

    //hPFHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hPackFrameHandler = 0x0UL;
    (LSA_VOID)EDDI_MemIniPool( EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL * 2,            // x * 2 = for in and out frames 
                           (LSA_UINT32)sizeof(EDDI_PRM_PF_DSCR_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hPackFrameHandler ); // OUT */

    //************************************************************************************
    // SubFrame  
    // hSubFrameHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hSubFrameHandler= 0x0UL;
    (LSA_VOID)EDDI_MemIniPool( (pDDB->CRT.MetaInfo.FcwConsumerCnt + pDDB->CRT.MetaInfo.FcwProviderCnt),       
                           (LSA_UINT32)sizeof(EDDI_PRM_SF_DSCR_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hSubFrameHandler ); // OUT*/

    //hSFHandler Contains Ressources for all FramesHandlers
    pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hSubFrameHandler = 0x0UL;
    (LSA_VOID)EDDI_MemIniPool( (pDDB->CRT.MetaInfo.FcwConsumerCnt + pDDB->CRT.MetaInfo.FcwProviderCnt),       
                           (LSA_UINT32)sizeof(EDDI_PRM_SF_DSCR_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hSubFrameHandler ); // OUT */

    //******************************************************************************************************************
    // Alloc PRM Records Structures
    //******************************************************************************************************************
    
    pDDB->PRM.PDIRSubFrameData.pRecordSet_A->PDirSubFrameDataRecordActLen = 0;
    
    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pLocalPDirSubframeDataRecord, maxSubframeLength);
    if (EDDI_NULL_PTR == pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pLocalPDirSubframeDataRecord)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pLocalPDIRPackframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pNetWorkPDirSubframeDataRecord, maxSubframeLength);
    if (EDDI_NULL_PTR ==  pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pNetWorkPDirSubframeDataRecord)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pNetWorkPDIRPackframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    
    pDDB->PRM.PDIRSubFrameData.pRecordSet_B->PDirSubFrameDataRecordActLen = 0;
    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord, maxSubframeLength);
    if (EDDI_NULL_PTR == pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDIRPackframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDirSubframeDataRecord, maxSubframeLength);
    if (EDDI_NULL_PTR ==  pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDirSubframeDataRecord)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDIRPackframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    
    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp, maxSubframeLength);
    if (EDDI_NULL_PTR ==  pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_AllocLocalMem((LSA_VOID **)&pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp, maxSubframeLength);
    if (EDDI_NULL_PTR ==  pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp)
    {
        EDDI_Excp("EDDI_PrmPdirPFDataInitComponent Alloc pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pLocalPDirSubframeDataRecord, (LSA_UINT8)0, maxSubframeLength);
    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord, (LSA_UINT8)0, maxSubframeLength);

    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pNetWorkPDirSubframeDataRecord, (LSA_UINT8)0, maxSubframeLength);
    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDirSubframeDataRecord, (LSA_UINT8)0, maxSubframeLength);
 
    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp, (LSA_UINT8)0, maxSubframeLength);
    EDDI_MemSet(pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp, (LSA_UINT8)0, maxSubframeLength);

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PrmPdirPFDataInitComponent, Initialisation OK");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_EDDI_PrmPdirPFFreeMemory()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPRelComponent(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16  ret_val;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EDDI_PrmPdirPFFreeMemory");

    //******************************************************************************************************************
    // Close MemPools
    //******************************************************************************************************************
    EDDI_MemClosePool(pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hPackFrameHandler);
    EDDI_MemClosePool(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hPackFrameHandler);
    
    EDDI_MemClosePool(pDDB->PRM.PDIRSubFrameData.pRecordSet_A->hSubFrameHandler);
    EDDI_MemClosePool(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->hSubFrameHandler);   

    //******************************************************************************************************************
    // Deallocate PRM Structures
    //******************************************************************************************************************
    pDDB->PRM.PDIRSubFrameData.pRecordSet_A->PDirSubFrameDataRecordActLen = 0;
    pDDB->PRM.PDIRSubFrameData.pRecordSet_B->PDirSubFrameDataRecordActLen = 0;
    
    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pPDIRSubframeDataTemp", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    } 

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pPDIRSubframeDataNetworkFormatTemp", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    } 

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pNetWorkPDirSubframeDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pRecordSet_A->pNetWorkPDirSubframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pRecordSet_A->pLocalPDirSubframeDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pRecordSet_A->pLocalPDirSubframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDirSubframeDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pRecordSet_B->pNetWorkPDirSubframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord);
    if (EDD_STS_OK != ret_val)
    {
        EDDI_Excp("EDDI_EDDI_PrmPdirPFFreeMemory pRecordSet_B->pLocalPDirSubframeDataRecord", EDDI_FATAL_ERR_EXCP, 0, ret_val);
        return;
    }

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_EDDI_PrmPdirPFFreeMemory");
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_DFPInitPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB,
                                                                     EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE                    pPackFrmHandler,
                                                                     EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE  const  pPackFrmData )
{   
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPInitPackFrameHandler->");

    EDDI_MemSet(pPackFrmHandler, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE));

    pPackFrmHandler->NrOfSubFrames = (LSA_UINT8)EDDI_PrmCalcSubframeCount(pPackFrmData);
    pPackFrmHandler->FrameID = pPackFrmData->FrameID;
    pPackFrmHandler->SFProperties = pPackFrmData->SFIOCRProperties;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPInitPackFrameHandler<-");

    LSA_UNUSED_ARG(pDDB);
    return LSA_TRUE;
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
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPCalcPackFrameLength( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                   EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE         pPackFrmHandler ) 
{
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  pLocalDG     = pPackFrmHandler->pSF;
    LSA_UINT32                        SumDGLength  = 0;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPCalcPackFrameLength->");

    //Calc PackFrameLength
    while (pLocalDG)
    {
        SumDGLength += pLocalDG->Len + EDDI_DFP_DG_HEADER_LENGTH + EDDI_DFP_DG_CRC_LENGTH;
        pLocalDG = pLocalDG->pNext;
    }
     
    pPackFrmHandler->FrameLen = (LSA_UINT16)(EDDI_DFP_PACKFRM_HEADER_CHECKSUM_LENGTH + SumDGLength + EDDI_DFP_PACKFRM_SF_END_DELIMITER_LENGTH); 
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPCalcPackFrameLength<-");
    LSA_UNUSED_ARG(pDDB);
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_DFPInitSubFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE         pSubFrmHandler,
                                                                    LSA_UINT32                               SubframeData )
{
    LSA_UINT32  Value;
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPInitSubFrameHandler->");

    EDDI_MemSet(pSubFrmHandler, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE));

    //Check Position
    Value = EDDI_GetBitField32NoSwap(SubframeData, EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION);
    if (   (Value >= EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MAX) 
        && (Value <= EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MIN) ) 
    {
        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Position:0x%X. Expected: from 0x%X to 0x%X", Value, EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MIN, EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MAX);
        return LSA_FALSE; 
    }
    pSubFrmHandler->SFPosition = (LSA_UINT8)Value;
    
    //Data Lenghth
    Value = EDDI_GetBitField32NoSwap(SubframeData, EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH);
    if (   (Value >= EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MAX) 
        && (Value <= EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MIN) ) 
    {
        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmFctCheckSubframeBlock, Data Lenghth:0x%X. Expected: from 0x%X to 0x%X", Value, EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MIN, EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MAX);
        return LSA_FALSE; 
    }
    pSubFrmHandler->Len = (LSA_UINT8)Value;

    pSubFrmHandler->UsedByUpper = LSA_FALSE;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPInitSubFrameHandler<-");

    LSA_UNUSED_ARG(pDDB);
    return LSA_TRUE;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPCheckPackFrameAgainstIrtFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                               EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  const  pPackFrame,
                                                                               EDDI_IRT_FRM_HANDLER_PTR_TYPE      const  pFrmHandler )
{
    EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE              pLocalPackFrame  = pPackFrame;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                  pLocalFrmHandler = pFrmHandler;
    LSA_BOOL                                       Found            = LSA_FALSE;

    LSA_UINT32                                     iDataElemCount   = 0;
    LSA_UINT32                                     iDataElem;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler->");

    //Not allowed: More than one active IRT-ports
    if (   (   (pDDB->PRM.PDIRData.pRecordSet_B->MetaData.IrtAllPortUsrTxCnt > 1)
            || (pDDB->PRM.PDIRData.pRecordSet_B->MetaData.IrtAllPortUsrRxCnt > 1))
        && (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == EDDI_PrmGetFrameDataProperties(pDDB, LSA_TRUE /*bIsRecordSet_B*/, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD)))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDDI_PRM_INDEX_PDIR_DATA,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         EDD_PRM_ERR_FAULT_DEFAULT);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, More then one IRT-Port is activate");
        return EDD_STS_ERR_PRM_CONSISTENCY; 
    }
    
    while (pLocalPackFrame)
    {
        Found = LSA_FALSE;
        pLocalFrmHandler = pFrmHandler;

        while (pLocalFrmHandler)
        {
            for (iDataElem = 0; iDataElem < pDDB->PM.PortMap.PortCnt; iDataElem++)
            {
                EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pLocalFrmHandler->pIrFrameDataElem[iDataElem];

                if (pFrmData)
                {
                    LSA_UINT32  const  TxPortCount = EDDI_PRMGetPortCountFromTxPortGroupArray(pFrmData->UsrTxPortGroupArray[0]);
                 
                    if ((EDDI_SYNC_IRT_PROVIDER == pLocalFrmHandler->HandlerType) && (TxPortCount != 1))
                    {
                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, ERROR: -> The redundant provider FrameID(0x%X) has TxPortCount(%u) in PDIRData. TxPortGroupArray=0x%X", pFrmData->FrameID, TxPortCount, pFrmData->UsrTxPortGroupArray[0]);
                        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                            EDDI_PRM_INDEX_PDIR_DATA,
                                                                            EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                            EDD_PRM_ERR_FAULT_DEFAULT);
                        return EDD_STS_ERR_PRM_CONSISTENCY; 
                    }
                                       
                    if (pLocalFrmHandler->FrameId == pLocalPackFrame->FrameID)
                    {                
                        iDataElemCount++;
                        
                        if (    (EDD_CSRT_DATALEN_MIN == pFrmData->DataLength)   
                             && (pLocalPackFrame->FrameLen > pFrmData->DataLength)) 
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, ERROR: CSF_SDU-DataLength:0x%X > PDIRData.DataLength:0x%X", pLocalPackFrame->FrameLen, pFrmData->DataLength);
                            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                             EDD_PRM_ERR_INDEX_DEFAULT,
                                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                             EDD_PRM_ERR_FAULT_DEFAULT);
                            return EDD_STS_ERR_PRM_CONSISTENCY; 
                        }
                        
                        if (    (pFrmData->DataLength > EDD_CSRT_DATALEN_MIN)   
                             && (pLocalPackFrame->FrameLen != pFrmData->DataLength)) 
                        {
                            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, ERROR: CSF_SDU-DataLength:0x%X != PDIRData.DataLength:0x%X", pLocalPackFrame->FrameLen, pFrmData->DataLength);
                            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                             EDD_PRM_ERR_INDEX_DEFAULT,
                                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                             EDD_PRM_ERR_FAULT_DEFAULT);
                            return EDD_STS_ERR_PRM_CONSISTENCY; 
                        }
                                        
                        //Associate pFrameHandler with PackFrame
                        pLocalPackFrame->pFrameHandler = pLocalFrmHandler;    
                        
                        switch (pLocalFrmHandler->HandlerType)
                        {
                            case EDDI_SYNC_IRT_CONSUMER:
                            {
                                LSA_UINT32  const  Value = EDDI_GetBitField32NoSwap(pLocalPackFrame->SFProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION);

                                if (EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION_INBOUND != Value)
                                {
                                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                     EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, Invalid DFP Direction, Stored DFP Direction:0x%X", Value);
                                    return EDD_STS_ERR_PRM_CONSISTENCY; 
                                }

                                pLocalPackFrame->bIsProv = LSA_FALSE;
                                break;
                            }
                            case EDDI_SYNC_IRT_PROVIDER:
                            {
                                LSA_UINT32  Value = EDDI_GetBitField32NoSwap(pLocalPackFrame->SFProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION);
                                
                                if (EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION_OUTBOUND != Value)
                                {
                                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                     EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, Invalid DFP Direction, Stored DFP Direction:0x%X", Value);
                                    return EDD_STS_ERR_PRM_CONSISTENCY; 
                                }
                                
                                Value = EDDI_GetBitField32NoSwap(pLocalPackFrame->SFProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_SFCRC16);

                                //Check SFCRC16
                                if (0x0 != Value ) 
                                {
                                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                                    EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                     EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA,
                                                                                     EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                     EDD_PRM_ERR_FAULT_DEFAULT);
                                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, Invalid SFCRC16:0x%X. Expected:0x0", Value);
                                    return EDD_STS_ERR_PRM_CONSISTENCY; 
                                }
         
                                pLocalPackFrame->bIsProv = LSA_TRUE;
                                break;
                            }
                            case EDDI_SYNC_IRT_INVALID_HANDLER:
                            case EDDI_SYNC_IRT_FORWARDER:
                            case EDDI_SYNC_IRT_FWD_CONSUMER:
                            default:
                            {
                                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRSubFrameData);
                                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                                 EDD_PRM_ERR_INDEX_DEFAULT,
                                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, Invalid pLocalFrmHandler->HandlerType:0x%X, FrameID: 0x%X", pLocalFrmHandler->HandlerType, pLocalFrmHandler->FrameId);
                                return EDD_STS_ERR_PRM_CONSISTENCY;
                            }
                        }                                
                        Found = LSA_TRUE;
                    } 
                } 
            }
            pLocalFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(LSA_VOID *)pLocalFrmHandler->QueueLink.next_ptr; 
        }
    
        if (LSA_FALSE == Found)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PDIRSubFrameData);
            EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                             EDD_PRM_ERR_INDEX_DEFAULT,
                                                             EDD_PRM_ERR_OFFSET_DEFAULT,
                                                             EDD_PRM_ERR_FAULT_DEFAULT);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, No equal FrameId found");
            return EDD_STS_ERR_PRM_CONSISTENCY;
        }
        pLocalPackFrame = pLocalPackFrame->pNext; 
    }
    
    if (iDataElemCount == 0)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler, No valid IrtFrameHandler was found, pLocalFrmHandler->FrameId:0x%X, pLocalPackFrame->FrameID:0x%X",
                          pFrmHandler->FrameId, pPackFrame->FrameID);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
        EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                         EDD_PRM_ERR_INDEX_DEFAULT,
                                                         EDD_PRM_ERR_OFFSET_DEFAULT,
                                                         EDD_PRM_ERR_FAULT_DEFAULT);
        return EDD_STS_ERR_PRM_CONSISTENCY;
    }
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPCheckPackFrameAgainstIrtFrameHandler<-");

    return EDD_STS_OK;
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
    LSA_UNUSED_ARG(ProvConsId);

    if (*pFrmHandler)
    {         
        EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE const pPackFrame = EDDI_DFPFindPackFrameHandler(pDDB, (*pFrmHandler)->FrameId);

        if (pPackFrame)
        {      
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                              "EDDI_DFPCheckFrameHandlerPF, ERROR: Not allowed to activate a Packframe by user, FrameId:0x%X, ProvConsID:0x%X",
                              (*pFrmHandler)->FrameId, ProvConsId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderID);    
            
            *pFrmHandler = LSA_NULL;  
        }
    }
    LSA_UNUSED_ARG(pDDB);
}   
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_crt_dfp.c                                               */
/*****************************************************************************/

