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
/*  F i l e               &F: eddi_prm_record_pdir_subframe_data.c      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-global functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  26.10.09    AH    initial version                                        */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"
#include "eddi_prm_record_pdir_subframe_data.h"
#include "eddi_prm_record_common.h"
//#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

//#include "eddi_pool.h"

//#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDIR_SUBFRAME_DATA
#define LTRC_ACT_MODUL_ID  413

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_DFP_ON)
/*

    Structure of PDIRSubframeData
    ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
     _________________
    |PDIRSubframeData |
     ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
         _________________
        | SubframeBlock 1 |
         ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | SubframeData 1 |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | ...            |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | SubframeData n |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
         ________________
        | ...            |
         ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
         _________________
        | SubframeBlock n |
         ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | SubframeData 1 |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | ...            |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
             ________________
            | SubframeData n |
             ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯  

*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCalculateMaxSubframeLength()             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCalculateMaxSubframeLength( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB )
{
    LSA_UINT32                        maxSubframeLength;
    LSA_UINT32                        maxSubframeDG;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCalculateMaxSubframeLength->");

    /* max. dfp consumer / provider = max. rtc3 consumer / provider */
    maxSubframeDG = pDDB->CRT.MetaInfo.FcwConsumerCnt + pDDB->CRT.MetaInfo.FcwProviderCnt;

    /* record header + NumberOfSubframeBlocks */
    maxSubframeLength = sizeof(EDDI_PRM_RECORD_HEADER_TYPE) + sizeof(LSA_UINT16 /*NumberOfSubframeBlocks*/);
    
    maxSubframeLength += 
        /* max subframes */
        (EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MAX * 
        /* subframe header */
        ( sizeof(EDDI_PRM_RECORD_HEADER_TYPE) + sizeof(LSA_UINT16/*FrameID*/) + sizeof(LSA_UINT32/*SFIOCRProperties*/) + 
        /* data * max DG */
        (sizeof(LSA_UINT32/*SubframeData[0]*/) * maxSubframeDG) )); 

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCalculateMaxSubframeLength<-");

    return maxSubframeLength;
}

/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckAndCopyPdirSubframeData()           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckAndCopyPdirSubframeData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    //EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE  PDIRSubframeData;
    //EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE  PDIRSubframeDataNetworkFormat; 
    
    LSA_UINT8*                               pPDIRSubframeData;
    LSA_UINT8*                               pPDIRSubframeDataNetworkFormat;

    LSA_UINT32                                 maxSubframeLength;
    
    EDDI_LOCAL_MEM_U8_PTR_TYPE               pRecord;
    EDDI_LOCAL_MEM_U8_PTR_TYPE               pRecordStart;
    LSA_RESULT                               Status                            = EDD_STS_OK;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckAndCopyPdirSubframeData->");

    pDDB->PRM.PDIRSubFrameData.edd_port_id = pPrmWrite->edd_port_id;
    pDDB->PRM.PDIRSubFrameData.SlotNumber = pPrmWrite->slot_number;
    pDDB->PRM.PDIRSubFrameData.SubSlotNumber = pPrmWrite->subslot_number;
    pDDB->PRM.PDIRSubFrameData.RecordSet_B.PDirSubFrameDataRecordActLen = pPrmWrite->record_data_length;

    pPDIRSubframeData = pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataTemp;
    pPDIRSubframeDataNetworkFormat = pDDB->PRM.PDIRSubFrameData.pPDIRSubframeDataNetworkFormatTemp;

    maxSubframeLength = EDDI_PrmCalculateMaxSubframeLength(pDDB);

    //Clear records
    EDDI_MEMSET((EDDI_LOCAL_MEM_PTR_TYPE)(pPDIRSubframeData), 0, maxSubframeLength);
    EDDI_MEMSET((EDDI_LOCAL_MEM_PTR_TYPE)(pPDIRSubframeDataNetworkFormat), 0, maxSubframeLength);
   
    pRecord  = pPrmWrite->record_data;
    pRecordStart = pRecord;

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDIRData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckAndCopyPdirSubframeData, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    //---------------------------------------------------------------------
    //  Check if DFP is allowed at all 
    //---------------------------------------------------------------------
    if (0 == pDDB->FeatureSupport.MaxDFPFrames)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_PRM_INDEX);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckAndCopyPdirSubframeData, DFP support is switched off in DPB!");
        return EDD_STS_ERR_PRM_INDEX;
    }

       
    //---------------------------------------------------------------------
    //  Check SubframeBlock 
    //---------------------------------------------------------------------
    Status = EDDI_PrmFctCheckPDIRSubframeData(&pRecord, (EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE)(LSA_VOID *)pPDIRSubframeData, (EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE)(LSA_VOID *)pPDIRSubframeDataNetworkFormat, pDDB);
    if (EDD_STS_OK != Status)
    {  
        return Status;
    }
    
    //---------------------------------------------------------------------
    //  Copy SubframeBlock 
    //---------------------------------------------------------------------
    EDDI_PrmFctCopySubframeData(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pLocalPDirSubframeDataRecord, (EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE)(LSA_VOID *)pPDIRSubframeData);
    
    //Copy NetworkFormat
    EDDI_MemCopy(pDDB->PRM.PDIRSubFrameData.pRecordSet_B->pNetWorkPDirSubframeDataRecord, pRecordStart, pDDB->PRM.PDIRSubFrameData.RecordSet_B.PDirSubFrameDataRecordActLen); 

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckAndCopyPdirSubframeData<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCheckSubframeBlock()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_LOCAL_MEM_U8_PTR_TYPE                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCheckPDIRSubframeData( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                   EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        PDIRSubframeData,
                                                                   EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        PDIRSubframeDataNetworkFormat,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB  )               
{  
    LSA_RESULT                                Status                            = EDD_STS_OK;
    LSA_UINT16                                Index;
    LSA_UINT8                              *  pSubframeBlock;
    LSA_UINT8                              *  pNetworkFormatSubframeBlock;
    LSA_UINT32                                BlockLength;
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE BlockHeader; //BlockHeader from PDIRApplicationData cannot be passed to functions as compiler my not align it

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmFctCheckPDIRSubframeData->");
    
    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeaderWithoutPadding(pRecord, &BlockHeader, &PDIRSubframeDataNetworkFormat->BlockHeader, pDDB);
    PDIRSubframeData->BlockHeader.BlockLength     = BlockHeader.BlockLength;
    PDIRSubframeData->BlockHeader.BlockType       = BlockHeader.BlockType;
    PDIRSubframeData->BlockHeader.BlockVersionHigh= BlockHeader.BlockVersionHigh;
    PDIRSubframeData->BlockHeader.BlockVersionLow = BlockHeader.BlockVersionLow;

    if (PDIRSubframeData->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);

        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctCheckSubframeBlock, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          PDIRSubframeData->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_BLOCK);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //NumberOfSubframeBlocks
    PDIRSubframeData->NumberOfSubframeBlocks = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, PDIRSubframeDataNetworkFormat->NumberOfSubframeBlocks);
    
    if (  (  (PDIRSubframeData->NumberOfSubframeBlocks >= EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MAX)
          && (PDIRSubframeData->NumberOfSubframeBlocks <= EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MIN))
       )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_OFFSET_NumberOfSubframeBlocks);
        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctCheckSubframeBlock, Invalid NumberOfSubframeBlocks:0x%X. Expected:0x%X to 0x%X",
                          PDIRSubframeData->NumberOfSubframeBlocks, EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MIN, EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MAX);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //---------------------------------------------------------------------
    //  Check CheckSubframeBlock (Packframe)
    //---------------------------------------------------------------------
    pSubframeBlock = (LSA_UINT8*)(LSA_VOID *)PDIRSubframeData->SubframeBlock;
    pNetworkFormatSubframeBlock = (LSA_UINT8*)(LSA_VOID *)PDIRSubframeDataNetworkFormat->SubframeBlock;
    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_OFFSET_START_FIRST_SubframeBlock);
    
    for (Index = 0; Index < PDIRSubframeData->NumberOfSubframeBlocks; Index++)
    {                      
        Status = EDDI_PrmFctCheckSubframeBlock(pRecord, (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)pSubframeBlock, (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)pNetworkFormatSubframeBlock, pDDB);
        if (EDD_STS_OK != Status)
        {
            return Status;
        }
        BlockLength                 = ((EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)pSubframeBlock)->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH;
        
        pSubframeBlock              += BlockLength;
        pNetworkFormatSubframeBlock += BlockLength; 
    }
    
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmFctCheckPDIRSubframeData<-");
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCheckSubframeBlock()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_LOCAL_MEM_U8_PTR_TYPE                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCheckSubframeBlock( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE       pPDIRSubframeBlock,
                                                                EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE       pPDIRSubframeBlockNetworkFormat,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB  )               
{  
    LSA_RESULT  Status             = EDD_STS_OK;
    LSA_UINT32  SubframeDataCount; 
    LSA_UINT32  Value;
    LSA_UINT16  Index;
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE BlockHeader; //BlockHeader from PDIRApplicationData cannot be passed to functions as compiler my not align it

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmFctCheckSubframeBlock->");

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeaderWithoutPadding(pRecord, &BlockHeader, &pPDIRSubframeBlockNetworkFormat->BlockHeader, pDDB);
    pPDIRSubframeBlock->BlockHeader.BlockLength     = BlockHeader.BlockLength;
    pPDIRSubframeBlock->BlockHeader.BlockType       = BlockHeader.BlockType;
    pPDIRSubframeBlock->BlockHeader.BlockVersionHigh= BlockHeader.BlockVersionHigh;
    pPDIRSubframeBlock->BlockHeader.BlockVersionLow = BlockHeader.BlockVersionLow;

    if (pPDIRSubframeBlock->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_BLOCK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctCheckSubframeBlock, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          pPDIRSubframeBlock->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_BLOCK);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //FrameID
    pPDIRSubframeBlock->FrameID = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pPDIRSubframeBlockNetworkFormat->FrameID);

    //SFIOCRProperties
    pPDIRSubframeBlock->SFIOCRProperties = EDDI_GET_U32(*pRecord);
    EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRSubframeBlockNetworkFormat->SFIOCRProperties);

    //Check DFP-Mode
    Value = EDDI_GetBitField32NoSwap(pPDIRSubframeBlock->SFIOCRProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_MODE);
    if (0x00 != Value ) 
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DFP_MODE);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Invalid DFPMode:0x%X. Expected:0x00", Value);
        return EDD_STS_ERR_PRM_BLOCK; 
    }
    //Check DFPRedundantPath Layout
    Value = EDDI_GetBitField32NoSwap(pPDIRSubframeBlock->SFIOCRProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_REDUNDANT_PATH_LAYOUT);
    if (0x0 != Value ) 
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DFP_RED_PATH_LAYOUT);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Invalid DFPRedundantPath Layout:0x%X. Expected:0x0", Value);
        return EDD_STS_ERR_PRM_BLOCK; 
    }

    //Check DistributedWatchDogFactor
    Value = EDDI_GetBitField32NoSwap(pPDIRSubframeBlock->SFIOCRProperties, EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DISTRIBUTED_WATCH_DOG_FACTOR);
    if ((EDDI_PRM_RECORD_SFIOCR_PROPERTIES_MIN_DISTRIBUTED_WATCH_DOG_FACTOR > Value) || 
            ( EDDI_PRM_RECORD_SFIOCR_PROPERTIES_MAX_DISTRIBUTED_WATCH_DOG_FACTOR < Value) )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DistributedWatchDogFactor);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Invalid DistributedWatchDogFactor :0x%X.(FrameID: 0x%X)",
                        Value, pPDIRSubframeBlock->FrameID);
        return EDD_STS_ERR_PRM_BLOCK; 
    }

    //Calc SubframeData
    SubframeDataCount = EDDI_PrmCalcSubframeCount(pPDIRSubframeBlock); 
    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_START_FIRST_SubframeData);
         
    //Check SubframeBlock elements
    for (Index = 0; Index < SubframeDataCount; Index++)
    {
        //Check DG Elements        
        pPDIRSubframeBlock->SubframeData[Index] = EDDI_GET_U32(*pRecord);    
        EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRSubframeBlockNetworkFormat->SubframeData[Index]); 
        
       //Check Position
        Value = EDDI_GetBitField32NoSwap(pPDIRSubframeBlock->SubframeData[Index], EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION);
        if (   (Value >= EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MAX) 
            && (Value <= EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MIN) ) 
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_SubframeData_Position);
            EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Position:0x%X. Expected: from 0x%X to 0x%X", Value, EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MIN, EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MAX);
            return EDD_STS_ERR_PRM_BLOCK; 
        }

        //Data Lenghth
        Value = EDDI_GetBitField32NoSwap(pPDIRSubframeBlock->SubframeData[Index], EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH);
        if (   (Value >= EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MAX) 
            && (Value <= EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MIN) ) 
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_SubframeData_Length);
            EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmFctCheckSubframeBlock, Data Lenghth:0x%X. Expected: from 0x%X to 0x%X", Value, EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MIN, EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MAX);
            return EDD_STS_ERR_PRM_BLOCK; 
        }
        EDDI_PRM_ADD_ERR_OFFSET(pDDB, sizeof(LSA_UINT32));
    }
    
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmFctCheckSubframeBlock<-");
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopySubframeData()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopySubframeData( EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        pPDIRSubframeDataDst,
                                                            EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        pPDIRSubframeDataSrc)               
{               
    EDDI_MemCopy(&pPDIRSubframeDataDst->BlockHeader, &pPDIRSubframeDataSrc->BlockHeader, sizeof(EDDI_PRM_RECORD_HEADER_TYPE)); 
    EDDI_MemCopy(&pPDIRSubframeDataDst->NumberOfSubframeBlocks, &pPDIRSubframeDataSrc->NumberOfSubframeBlocks, sizeof(pPDIRSubframeDataSrc->NumberOfSubframeBlocks)); 
    
    EDDI_PrmFctCopySubframeBlock(pPDIRSubframeDataDst->SubframeBlock, pPDIRSubframeDataSrc->SubframeBlock, pPDIRSubframeDataSrc->NumberOfSubframeBlocks);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopySubframeBlock()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopySubframeBlock( EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE         pPDIRSubframeBlockDataDst,
                                                             EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE         pPDIRSubframeBlockDataSrc,
                                                             LSA_UINT32                                    const  NumberOfSubframeBlocks)               
{         
    LSA_UINT32  Index;
    LSA_UINT32  SubIndex;
    
    for (Index=0; Index<NumberOfSubframeBlocks; Index++)
    {
        LSA_UINT32  const  SubframeDataCount =  EDDI_PrmCalcSubframeCount(pPDIRSubframeBlockDataSrc);
  
        EDDI_MemCopy(&pPDIRSubframeBlockDataDst->BlockHeader, &pPDIRSubframeBlockDataSrc->BlockHeader, sizeof(EDDI_PRM_RECORD_HEADER_TYPE));     
        EDDI_MemCopy(&pPDIRSubframeBlockDataDst->FrameID, &pPDIRSubframeBlockDataSrc->FrameID, sizeof(pPDIRSubframeBlockDataDst->FrameID)); 
        EDDI_MemCopy(&pPDIRSubframeBlockDataDst->SFIOCRProperties, &pPDIRSubframeBlockDataSrc->SFIOCRProperties, sizeof(pPDIRSubframeBlockDataDst->SFIOCRProperties));  
        
        for (SubIndex=0; SubIndex<SubframeDataCount; SubIndex++)
        {
            EDDI_MemCopy(&pPDIRSubframeBlockDataDst->SubframeData[SubIndex], &pPDIRSubframeBlockDataSrc->SubframeData[SubIndex], sizeof(pPDIRSubframeBlockDataSrc->SubframeData[0])); 
        } 
        //Get next pPDIRSubframeBlockDataSrc
        pPDIRSubframeBlockDataSrc = (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)((LSA_UINT8*)(void *)(pPDIRSubframeBlockDataSrc) + pPDIRSubframeBlockDataSrc->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH);
        pPDIRSubframeBlockDataDst = (EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE)(LSA_VOID *)((LSA_UINT8*)(void *)(pPDIRSubframeBlockDataDst) + pPDIRSubframeBlockDataDst->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopySubframeBlock()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCalcSubframeCount( EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE  const  pPDIRSubframeBlockData )  
{
    LSA_UINT32  const  SubframeDataCount = ((pPDIRSubframeBlockData->BlockHeader.BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH) - (sizeof(pPDIRSubframeBlockData->BlockHeader) + sizeof(pPDIRSubframeBlockData->SFIOCRProperties))) / sizeof(pPDIRSubframeBlockData->SubframeData[0]);

    return SubframeDataCount;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif


/*****************************************************************************/
/*  end of file eddi_prm_record_pdir_subframe_data.c                         */
/*****************************************************************************/
