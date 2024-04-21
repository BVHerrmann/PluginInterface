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
/*  F i l e               &F: eddi_prm_record_pdir_data.c               :F&  */
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
/*  27.05.09    AH    initial version                                        */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"
#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_common.h"
#include "eddi_prm_req.h"
#include "eddi_crt_check.h"
#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"
//#include "eddi_ser_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDIR_DATA
#define LTRC_ACT_MODUL_ID  405

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/***************************************************************************/
/* Protos                                                                  */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  PDIRBeginEnd_DetermineClassType( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                        EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet );


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetPdirData()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_LOCAL_MEM_U8_PTR_TYPE                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirData( EDDI_LOCAL_MEM_U8_PTR_TYPE                 *  pRecord,
                                                         EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE         *  pPDIRData,
                                                         EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE         *  pPDIRDataNetworkFormat,
                                                         EDDI_LOCAL_DDB_PTR_TYPE             const     pDDB )
{
    EDDI_LOCAL_MEM_U8_PTR_TYPE const    pRecordStart    = *pRecord;
    LSA_RESULT                          Status          =  EDD_STS_OK;

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeader(pRecord, &pPDIRData->BlockHeader, &pPDIRDataNetworkFormat->BlockHeader, pDDB);

    if (pPDIRData->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_HEADER_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctGetPdirData, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          pPDIRData->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_HEADER_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //SlotNumber
    pPDIRData->SlotNumber = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pPDIRDataNetworkFormat->SlotNumber);

    //SubslotNumber;
    pPDIRData->SubslotNumber = EDDI_GET_U16(*pRecord);    
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pPDIRDataNetworkFormat->SubslotNumber);

    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(*pRecord, pRecordStart));
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetPdirGlobalDataExt()                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirGlobalDataExt( EDDI_LOCAL_MEM_U8_PTR_TYPE                     *  pRecord,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE      *  pPDIRGlobalDataExt,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE      *  pPDIRGlobalDataExtNetworkFormat,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB )
{                                                                                  
    LSA_UINT16                          i;
    EDDI_LOCAL_MEM_U8_PTR_TYPE const    pRecordStart    = *pRecord;
    LSA_RESULT                          Status          =  EDD_STS_OK;
//  LSA_UINT32                          err_offset_NumberOfPorts;

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeader(pRecord, &pPDIRGlobalDataExt->BlockHeader, &pPDIRGlobalDataExtNetworkFormat->BlockHeader, pDDB);

    if //BlockType of PDIRGlobalData invalid
       (pPDIRGlobalDataExt->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_GLOBAL_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctGetPdirGlobalDataExt, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          pPDIRGlobalDataExt->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_GLOBAL_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //IRDataID
    for (i=0; i < 16; i++)
    {
        pPDIRGlobalDataExt->IRDataID[i] = **pRecord; 
        EDDI_GET_U8_INCR(*pRecord, pPDIRGlobalDataExtNetworkFormat->IRDataID[i]);
    }

    //MaxBridgeDelay
    pPDIRGlobalDataExt->MaxBridgeDelay = EDDI_GET_U32(*pRecord);
    EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->MaxBridgeDelay);
    
    //NumberOfPorts
//  err_offset_NumberOfPorts          = *pRecord - pRecordStart;
    pPDIRGlobalDataExt->NumberOfPorts = EDDI_GET_U32(*pRecord);
    EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->NumberOfPorts);


    //check BlockVersionLow of PDIRGlobalData
    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA == pPDIRGlobalDataExt->BlockHeader.BlockVersionLow ||
        EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12 == pPDIRGlobalDataExt->BlockHeader.BlockVersionLow)
    {
        pDDB->PRM.PDIRData.pRecordSet_B->PortCnt_Extended    = pPDIRGlobalDataExt->NumberOfPorts;
        pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended = LSA_TRUE;  
        
        if (pDDB->PRM.PDIRData.pRecordSet_B->PortCnt_Extended > EDD_CFG_MAX_PORT_CNT)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRGlobalData);
//          EDDI_PRM_SET_ERR_OFFSET(pDDB, err_offset_NumberOfPorts);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_NumberOfPorts);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_PrmFctGetPdirGlobalDataExt, Invalid PortCnt_Extended:0x%X",
                              pDDB->PRM.PDIRData.pRecordSet_B->PortCnt_Extended);
            return EDD_STS_ERR_PRM_BLOCK;  
        }  
    }
    else
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctGetPdirGlobalDataExt, EDDI_ERR_INV_BlockVersionLow_PDIRGlobalData > ->:0x%X :0x%X",
                          pPDIRGlobalDataExt->BlockHeader.BlockVersionLow, EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    } 
       
    //MaxPortDelay
    for (i=0; i < pPDIRGlobalDataExt->NumberOfPorts; i++)
    {
        pPDIRGlobalDataExt->MaxPortDelay[i].Tx = EDDI_GET_U32(*pRecord);
        EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->MaxPortDelay[i].Tx);

        pPDIRGlobalDataExt->MaxPortDelay[i].Rx = EDDI_GET_U32(*pRecord);
        EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->MaxPortDelay[i].Rx);  

        if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12 == pPDIRGlobalDataExt->BlockHeader.BlockVersionLow) //PDIRGlobalData Version 1.2
        {
            pPDIRGlobalDataExt->MaxPortDelay[i].MaxLineRxDelay = EDDI_GET_U32(*pRecord);
            EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->MaxPortDelay[i].MaxLineRxDelay);

            pPDIRGlobalDataExt->MaxPortDelay[i].YellowTime = EDDI_GET_U32(*pRecord);
            EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRGlobalDataExtNetworkFormat->MaxPortDelay[i].YellowTime);
        }
    }

    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(*pRecord, pRecordStart));
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetPdirFrameData()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirFrameData( EDDI_LOCAL_MEM_U8_PTR_TYPE                  * pRecord,
                                                              EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE        * pFrameData,
                                                              EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE        * pFrameDataNetworkFormat,
                                                              EDDI_LOCAL_DDB_PTR_TYPE               const   pDDB  )               
{
    EDDI_LOCAL_MEM_U8_PTR_TYPE const    pRecordStart    = *pRecord;
    LSA_RESULT                          Status          =  EDD_STS_OK;

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeader(pRecord, &pFrameData->BlockHeader, &pFrameDataNetworkFormat->BlockHeader, pDDB);

    //BlockType of PDIRFrameData invalid
    if (pFrameData->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_FRAME_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctGetPdirFrameData, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          pFrameData->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_FRAME_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pFrameData->BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
    {                
        pFrameData->FrameDataProperties = EDDI_GET_U32(*pRecord);
        EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->FrameDataProperties); 

        if (EDDI_PRM_PDIR_FRAMEDATA_FORWARDING_MODE_RELATIVE_MODE == EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MODE) )  //Relative mode in EDDI is not allowed
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties_ForwardingMode);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_PrmFctGetPdirFrameData, Relative mode in EDDI is not allowed, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                              pFrameData->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_FRAME_DATA);
            return EDD_STS_ERR_PRM_BLOCK;
        }
        else if (EDDI_PRM_PDIR_FRAMEDATA_FORWARDING_MODE_ABSOLUTE_MODE == (pFrameData->FrameDataProperties & 1))  //Relative mode in EDDI is not allowed       
        {
            //nothing todo
        }
           
        #if !defined (EDDI_CFG_FRAG_ON)                                                         
        {
            LSA_UINT32  const  FragmentationMode = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FRAGMENTATION_MODE); 
            
            //Check Fragmentation
            if (   (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_128B == FragmentationMode) 
                || (EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_256B == FragmentationMode))        
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameProperties_Fragmentation);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties_FragmentationMode);
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_PrmFctGetPdirFrameData, Error: FrameDataProperties: Fragmention is ON, but the define EDDI_CFG_FRAG_ON is not set");
                return EDD_STS_ERR_PRM_BLOCK;
            }  
        }
        #endif 
    }

    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(*pRecord, pRecordStart));
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetPdirBeginEndData()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirBeginEndData( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                 EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE         * pPDIRBeginEndData,
                                                                 EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE         * pPDIRBeginEndDataNetworkFormat,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE                   const    pDDB )               
{  
    EDDI_LOCAL_MEM_U8_PTR_TYPE const    pRecordStart    = *pRecord;
    LSA_RESULT                          Status          =  EDD_STS_OK;

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeader(pRecord, &pPDIRBeginEndData->BlockHeader, &pPDIRBeginEndDataNetworkFormat->BlockHeader, pDDB);
    
    if //BlockType of PDIRBeginEndData invalid
       (pPDIRBeginEndData->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_BEGIN_END_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRBeginEndData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmFctGetPdirBeginEndData, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          pPDIRBeginEndData->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_BEGIN_END_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }
    
    pPDIRBeginEndData->RedGuardStartOfRedFrameID = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pPDIRBeginEndDataNetworkFormat->RedGuardStartOfRedFrameID);
    
    pPDIRBeginEndData->RedGuardEndOfRedFrameID = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pPDIRBeginEndDataNetworkFormat->RedGuardEndOfRedFrameID );
    
    pPDIRBeginEndData->NumberOfPorts = EDDI_GET_U32(*pRecord);
    EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pPDIRBeginEndDataNetworkFormat->NumberOfPorts);

    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(*pRecord, pRecordStart));
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetPdirFrameDataArray()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/                   
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirFrameDataArray( EDDI_LOCAL_MEM_U8_PTR_TYPE              * pRecord,
                                                                   EDDI_PRM_RECORD_FRAME_DATA_TYPE         * pFrameData,
                                                                   EDDI_PRM_RECORD_FRAME_DATA_TYPE         * pFrameDataNetworkFormat,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE          const    pDDB )               
{         
//    EDDI_LOCAL_MEM_U8_PTR_TYPE const    pRecordStart    = *pRecord;
    LSA_RESULT  const                   Status          =  EDD_STS_OK;
   
    LSA_UNUSED_ARG(pDDB);
   
    pFrameData->FrameSendOffset = EDDI_GET_U32(*pRecord);
    EDDI_GET_U32_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->FrameSendOffset);
    
    pFrameData->DataLength = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->DataLength);
    
    pFrameData->ReductionRatio = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->ReductionRatio);
    
    pFrameData->Phase = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->Phase);
    
    pFrameData->FrameID = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->FrameID);
    
    pFrameData->Ethertype = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pFrameDataNetworkFormat->Ethertype);
    
    pFrameData->UsrRxPort_0_4 = **pRecord;
    EDDI_GET_U8_INCR(*pRecord,  pFrameDataNetworkFormat->UsrRxPort_0_4);
    
    pFrameData->FrameDetails.Byte = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pFrameDataNetworkFormat->FrameDetails.Byte);
    
    pFrameData->NumberOfTxPortGroups = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pFrameDataNetworkFormat->NumberOfTxPortGroups);
    
    pFrameData->UsrTxPortGroupArray[0] = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pFrameDataNetworkFormat->UsrTxPortGroupArray[0]);
    
    pFrameData->Reserved[0] = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pFrameDataNetworkFormat->Reserved[0]);
    
    pFrameData->Reserved[1] = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pFrameDataNetworkFormat->Reserved[1]); 

//    EDDI_PRM_ADD_ERR_OFFSET(pDDB, *pRecord - pRecordStart);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopyPDIRData()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyPDIRData( EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE      *  pPDIRDataDst,
                                                        EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE      *  pPDIRDataSrc,
                                                        LSA_BOOL                           const  bGlobalDataExtended )               
{         
    EDDI_MemCopy(&pPDIRDataDst->BlockHeader, &pPDIRDataSrc->BlockHeader, sizeof(EDDI_PRM_RECORD_HEADER_TYPE)); 
    
    EDDI_COPY_MISALIGNED_U16(pPDIRDataDst, pPDIRDataSrc, EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SlotNumber);
    EDDI_COPY_MISALIGNED_U16(pPDIRDataDst, pPDIRDataSrc, EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SubslotNumber);
    
    if (bGlobalDataExtended)
    {
        EDDI_PrmFctCopyGlobalDataExtended(&pPDIRDataDst->PDIRGlobalDataExt, &pPDIRDataSrc->PDIRGlobalDataExt);    
    }

    EDDI_PrmFctCopyPDIRFrameData(&pPDIRDataDst->PDIRFrameData, &pPDIRDataSrc->PDIRFrameData);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopyPDIRFrameData()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyPDIRFrameData( EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * pPDIRFrameDataDst,
                                                             EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * pPDIRFrameDataSrc )               
{    
    EDDI_MemCopy(&pPDIRFrameDataDst->BlockHeader, &pPDIRFrameDataSrc->BlockHeader, sizeof(EDDI_PRM_RECORD_HEADER_TYPE)); 
    EDDI_COPY_MISALIGNED_U32(pPDIRFrameDataDst, pPDIRFrameDataSrc, EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctCopyGlobalDataExtended()              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/                   
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyGlobalDataExtended( EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE  * pGlobalDataExtendedDst,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE  * pGlobalDataExtendedSrc )               
{    
    LSA_UINT16  i;

    //PDIRGlobalDataExt //BlockHeader  
    EDDI_MemCopy(&pGlobalDataExtendedDst->BlockHeader, &pGlobalDataExtendedSrc->BlockHeader, sizeof(EDDI_PRM_RECORD_HEADER_TYPE)); 

    //IRDataID
    for (i=0; i < 16; i++)
    {      
        EDDI_MemCopy(&pGlobalDataExtendedDst->IRDataID[i], &pGlobalDataExtendedSrc->IRDataID[i], sizeof(pGlobalDataExtendedSrc->IRDataID[0]));  
    }

    //MaxBridgeDelay
    EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_MaxBridgeDelay), 
                 EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_MaxBridgeDelay), 
                 sizeof(pGlobalDataExtendedSrc->MaxBridgeDelay)); 
    
    //NumberOfPorts
    EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_NumberOfPorts), 
                 EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_NumberOfPorts), 
                 sizeof(pGlobalDataExtendedSrc->NumberOfPorts)); 
       
    //MaxPortDelay
    for (i=0; i < pGlobalDataExtendedSrc->NumberOfPorts; i++)
    {
        EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Tx), 
                     EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Tx), 
                     sizeof(pGlobalDataExtendedSrc->MaxPortDelay[i].Tx)); 
        EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Rx), 
                     EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Rx), 
                     sizeof(pGlobalDataExtendedSrc->MaxPortDelay[i].Rx)); 

        if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12 == pGlobalDataExtendedSrc->BlockHeader.BlockVersionLow) //PDIRGlobalData Version 1.2
        {           
            EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_MaxLineRxDelay), 
                         EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_MaxLineRxDelay), 
                         sizeof(pGlobalDataExtendedSrc->MaxPortDelay[i].MaxLineRxDelay)); 
            EDDI_MemCopy(EDDI_BUILD_BYTEPTR(pGlobalDataExtendedDst, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_YellowTime), 
                         EDDI_BUILD_BYTEPTR(pGlobalDataExtendedSrc, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay+(i*EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay)+EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_YellowTime), 
                         sizeof(pGlobalDataExtendedSrc->MaxPortDelay[i].YellowTime)); 
        }
    }  
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckPDIRData()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDIRDataMaxPortDelays( EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB,
                                                               const EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE *  const  pPDIRDataGlobalExt,
                                                                     LSA_BOOL                                     const  bGlobalDataExtended,
                                                                     LSA_UINT32                                   const  UsrPortIndex,
                                                                     LSA_UINT32                                *  const  pErrOffset)
{

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRDataMaxPortDelays->");

    if (   bGlobalDataExtended
        && UsrPortIndex < pPDIRDataGlobalExt->NumberOfPorts)
    {
        LSA_UINT32  const HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if ((( !pDDB->Glob.bDisableMaxPortDelayCheck)     &&
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Rx < pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortRxDelay )) ||    //temporarily_disabled_lint !e961
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Rx > EDDI_PRM_MAX_PORT_RX_DELAY )                                     ||    //temporarily_disabled_lint !e961
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Rx == 0 ) )                                                                 //temporarily_disabled_lint !e961
        {
            *pErrOffset = EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay + UsrPortIndex * EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay + EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Rx;
            EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "EDDI_PrmCheckPDIRData, Invalid MaxPortRxDelay, MaxPortRxDelay:0x%X ExpectedPortRxDelay:0x%X UsrPortIndex:%u HwPortIndex:%u",
                                pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Rx, pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortRxDelay, UsrPortIndex, HwPortIndex);
            return EDD_STS_ERR_PARAM;
        }

        if ((( !pDDB->Glob.bDisableMaxPortDelayCheck)     &&
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Tx < pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortTxDelay )) ||    //temporarily_disabled_lint !e961
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Tx > EDDI_PRM_MAX_PORT_TX_DELAY )                                     ||    //temporarily_disabled_lint !e961
                ( pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Tx == 0 ) )                                                                 //temporarily_disabled_lint !e961
        {
            *pErrOffset = EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay + UsrPortIndex * EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay + EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Tx;
            EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "EDDI_PrmCheckPDIRData, Invalid MaxPortTxDelay, MaxPortTxDelay:0x%X ExpectedPortTxDelay:0x%X UsrPortIndex:%u HwPortIndex:%u",
                                pPDIRDataGlobalExt->MaxPortDelay[UsrPortIndex].Tx, pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortTxDelay, UsrPortIndex, HwPortIndex);
            return EDD_STS_ERR_PARAM;
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRDataMaxPortDelays<-");

    *pErrOffset = 0;
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckPDIRData()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDIRData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PRM_RECORD_FRAME_DATA_TYPE           * pCurrentFrameDataElementLocal;
    LSA_UINT16                                  NumberOfElements;
    LSA_UINT32                                  NumberOfElementsCorr;
    LSA_UINT32                                  ElementItem;
    LSA_RESULT                                  Result;
    EDDI_PRM_RECORD_IRT_PTR_TYPE                pRecordSet_B;
    EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE          PDIRData;
    EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE          PDIRDataNetworkFormat;
    EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE    PDIRBeginEndData;
    EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE    PDIRBeginEndDataNetworkFormat;
    LSA_BOOL                                    bPDIRBeginEndData;
    EDDI_LOCAL_MEM_U8_PTR_TYPE                  pPDIRBeginEndData;
    EDDI_LOCAL_MEM_U8_PTR_TYPE                  pRecord;
    LSA_UINT32                                  BeginEndBlockHeaderOffset;
    LSA_UINT32                                  BlockHeaderOffset;
    LSA_UINT32                                  FrameDataBlockHeaderOffset;
    LSA_UINT32                                  GlobalBlockHeaderOffset;
    LSA_RESULT                                  Status = EDD_STS_OK;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRData->");

    pDDB->PRM.PDIRData.edd_port_id = pPrmWrite->edd_port_id;

    //Clear records
    EDDI_MEMSET(&PDIRData, 0, sizeof(EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE));
    EDDI_MEMSET(&PDIRDataNetworkFormat, 0, sizeof(EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE));

    pRecord  = pPrmWrite->record_data;

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDIRData);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, pPrmWrite->edd_port_id:0x%X",
                          pPrmWrite->edd_port_id);
        return EDD_STS_ERR_PRM_PORTID;
    }

    //---------------------------------------------------------------------
    //  Get and Check Pdirdata 
    //---------------------------------------------------------------------
    BlockHeaderOffset   = 0;
    Status              = EDDI_PrmFctGetPdirData(&pRecord, &PDIRData, &PDIRDataNetworkFormat, pDDB);

    if (EDD_STS_OK != Status)
    {
        return Status;
    }

    //---------------------------------------------------------------------
    //  Get and Check Pdirglobaldataext 
    //---------------------------------------------------------------------
    GlobalBlockHeaderOffset = EDDI_POINTER_BYTE_DISTANCE(pRecord, pPrmWrite->record_data);
    Status = EDDI_PrmFctGetPdirGlobalDataExt(&pRecord, &PDIRData.PDIRGlobalDataExt, &PDIRDataNetworkFormat.PDIRGlobalDataExt, pDDB);
    if (EDD_STS_OK != Status)
    {
        return Status;
    }

    //---------------------------------------------------------------------    
    //  Get and Check Pdirframedata 
    //---------------------------------------------------------------------
    FrameDataBlockHeaderOffset = EDDI_POINTER_BYTE_DISTANCE(pRecord, pPrmWrite->record_data);
    Status = EDDI_PrmFctGetPdirFrameData(&pRecord, &PDIRData.PDIRFrameData, &PDIRDataNetworkFormat.PDIRFrameData, pDDB);
    if (EDD_STS_OK != Status)
    {
        return Status;
    }
    
    pPDIRBeginEndData = pRecord;
    bPDIRBeginEndData = LSA_FALSE; //default: not present
    
    //Jump to the Next Record
    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == PDIRData.PDIRFrameData.BlockHeader.BlockVersionLow) //BlockVersionLow = 1
    {
        pRecord += PDIRData.PDIRFrameData.BlockHeader.BlockLength - (EDDI_PRM_BLOCK_WITHOUT_LENGTH + sizeof(PDIRData.PDIRFrameData.FrameDataProperties)); 
    }
    else
    {
        pRecord += PDIRData.PDIRFrameData.BlockHeader.BlockLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH;    
    }

    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pRecord, pPDIRBeginEndData));

    //check BlockVersionLow of PDIRData
    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_DATA == PDIRData.BlockHeader.BlockVersionLow)
    {
        //Only the new Version (Version 1) of PDIRData contains PDIRBeginEndData.
        BeginEndBlockHeaderOffset = EDDI_POINTER_BYTE_DISTANCE(pRecord, pPrmWrite->record_data);
        Status = EDDI_PrmFctGetPdirBeginEndData(&pRecord, &PDIRBeginEndData, &PDIRBeginEndDataNetworkFormat, pDDB);
        if (EDD_STS_OK != Status)
        {
            return Status;
        }
        bPDIRBeginEndData = LSA_TRUE;
    } 
    else
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDIRData);
        EDDI_PRM_RESET_ERR_OFFSET(pDDB, BlockHeaderOffset);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);

        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmCheckPDIRData, EDDI_ERR_INV_BlockVersionLow_PDIRData:0x%X EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_DATA:0x%X",
                          PDIRData.BlockHeader.BlockVersionLow, EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //---------------------------------------------------------------------
    //  PDIRData
    //---------------------------------------------------------------------
    EDDI_PRM_RESET_ERR_OFFSET(pDDB, BlockHeaderOffset);
    Result = EDDI_CheckPDIRDataBlockHeader(&PDIRData.BlockHeader, pDDB, pPrmWrite->record_data_length);
    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result:0x%X", Result);
        return Result;
    }

    if (pPrmWrite->slot_number != PDIRData.SlotNumber)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SlotNumber_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SlotNumber);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Invalid slot_number, slot_number:0x%X SlotNummer:0x%X",
                          pPrmWrite->slot_number, PDIRData.SlotNumber);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPrmWrite->subslot_number != PDIRData.SubslotNumber)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SubSlotNumber_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SubslotNumber);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Invalid subslot_number, subslot_number:0x%X SubslotNummer:0x%X",
                          pPrmWrite->subslot_number, PDIRData.SubslotNumber);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //---------------------------------------------------------------------
    // PDIRGlobal
    //---------------------------------------------------------------------
    EDDI_PRM_RESET_ERR_OFFSET(pDDB, GlobalBlockHeaderOffset);
    Result = EDDI_CheckPDIRGlobalDataBlockHeader(&PDIRData.PDIRGlobalDataExt.BlockHeader, pDDB);
    
    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result:0x%X", Result);
        return Result;
    }

    if (PDIRData.PDIRGlobalDataExt.IRDataID[0])  // not checked
    {
    }

    if (pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended)
    {
        LSA_UINT32  const  PDIRGlobalDataMaxBridgeDelay = PDIRData.PDIRGlobalDataExt.MaxBridgeDelay;

        if (   (PDIRGlobalDataMaxBridgeDelay < pDDB->Glob.MaxBridgeDelay)
               || (PDIRGlobalDataMaxBridgeDelay > EDDI_PRM_MAX_BRIDGE_DELAY)
               || (PDIRGlobalDataMaxBridgeDelay == 0))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MaxBridgeDelay_PDIRData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_MaxBridgeDelay);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_PrmCheckPDIRData, Invalid MaxBridgeDelay, MaxBridgeDelay:0x%X ExpectedMaxBridgeDelay:0x%X",
                              PDIRGlobalDataMaxBridgeDelay, pDDB->Glob.MaxBridgeDelay);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (PDIRData.PDIRGlobalDataExt.NumberOfPorts > pDDB->PM.PortMap.PortCnt)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortNumber_PDIRData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_NumberOfPorts);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_PrmCheckPDIRData, Invalid NumberOfPorts, NumberOfPorts:0x%X PortMap.PortCnt:0x%X",
                              PDIRData.PDIRGlobalDataExt.NumberOfPorts, pDDB->PM.PortMap.PortCnt);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    //Check the length of PDIRData
    if ( pPrmWrite->record_data_length > EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT ||
         ( PDIRData.PDIRGlobalDataExt.BlockHeader.BlockVersionLow == EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12 && 
           pPrmWrite->record_data_length > (EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT + sizeof(LSA_UINT32)) 
         )
       )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDIRData);
        //No EDDI_PRM_SET_ERR_OFFSET(pDDB, );
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Invalid record_data_length, record_data_length:0x%X EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT:0x%X",
                          pPrmWrite->record_data_length, EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT);
        return EDD_STS_ERR_PRM_DATA;
    }

    //---------------------------------------------------------------------
    // PDIRFrameData BlockHeader
    //---------------------------------------------------------------------
    EDDI_PRM_RESET_ERR_OFFSET(pDDB, FrameDataBlockHeaderOffset);
    Result = EDDI_CheckPDIRFrameDataBlockHeader(&PDIRData.PDIRFrameData, pDDB);
    if (EDD_STS_OK != Result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result:0x%X", Result);
        return Result;
    }

    pRecordSet_B = pDDB->PRM.PDIRData.pRecordSet_B;

    //Delete old Framehandler
    EDDI_SyncIrIrRecordCleanUp(pDDB, pRecordSet_B);

    //---------------------------------------------------------------------
    // PDIRBeginEnd BlockHeader
    // PDIRBeginEnd parser and builder
    //---------------------------------------------------------------------

    if (bPDIRBeginEndData) //temporarily_disabled_lint !e774   
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRData, PDIRBeginEnd present");
 
        EDDI_PRM_RESET_ERR_OFFSET(pDDB, BeginEndBlockHeaderOffset);
        Result = EDDI_CheckPDIRBeginEndBlockHeader(&PDIRBeginEndData.BlockHeader,pDDB);

        if (EDD_STS_OK != Result)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result ->:0x%X", Result);
            return Result;
        }

        Result = EDDI_PDIRBeginEndParse(&PDIRBeginEndData, pRecord, pDDB);

        if (EDD_STS_OK != Result)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result ->:0x%X", Result);
            return Result;
        }

        /* PDIRBeginEndData present */
        pRecordSet_B->bBeginEndDataPresent = LSA_TRUE;
    }
    else
    {                               
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRData, PDIRBeginEnd NOT present.");

        /* PDIRBeginEndData not present */
        pRecordSet_B->bBeginEndDataPresent = LSA_FALSE;
    }

    //---------------------------------------------------------------------
    // Copy incoming IRGlobalDataBlock to local Buffers. 
    //---------------------------------------------------------------------
    EDDI_MemCopy(pRecordSet_B->pNetWorkPDIRDataRecord, pPrmWrite->record_data, EDDI_NTOHS(PDIRDataNetworkFormat.BlockHeader.BlockLength) + EDDI_PRM_BLOCK_WITHOUT_LENGTH); 
    EDDI_PrmFctCopyPDIRData(pRecordSet_B->pLocalPDIRDataRecord, &PDIRData, pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended);
    
    //---------------------------------------------------------------------
    // FrameDataBlock
    //---------------------------------------------------------------------
    // Check Length of incoming IRFrameDataBlock

    NumberOfElements = (LSA_UINT16)((LSA_UINT16)(pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData.BlockHeader.BlockLength));

    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V10 == PDIRData.PDIRFrameData.BlockHeader.BlockVersionLow ) //BlockVersionLow = 0
    {
        NumberOfElements = (NumberOfElements - EDDI_PRM_BLOCK_WITHOUT_LENGTH) / sizeof(EDDI_PRM_RECORD_FRAME_DATA_TYPE);
    }
    else if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == PDIRData.PDIRFrameData.BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
    {
        NumberOfElements = ( NumberOfElements - (EDDI_PRM_BLOCK_WITHOUT_LENGTH + sizeof(PDIRData.PDIRFrameData.FrameDataProperties)) ) / sizeof(EDDI_PRM_RECORD_FRAME_DATA_TYPE);
    }
    
    pDDB->PRM.PDIRData.pRecordSet_B->NumberOfFrameDataElements = NumberOfElements;

    //for all Elements
    pRecord = pPDIRBeginEndData;
    pCurrentFrameDataElementLocal = &pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData.PDIRFrameData[0];

    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckPDIRData, NumberOfElements: 0x%X", NumberOfElements);

    //for (ElementItem = 0; ElementItem < NumberOfElements; ElementItem++)
    ElementItem = 0;
    NumberOfElementsCorr = 0; 
     
    //Copy Framedata   
    while (ElementItem < (NumberOfElements - NumberOfElementsCorr))
    {      
        EDDI_PRM_RESET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pRecord, pPrmWrite->record_data));
        Status = EDDI_PrmFctGetPdirFrameDataArray(&pRecord, &PDIRData.PDIRFrameData.PDIRFrameData[0],&PDIRDataNetworkFormat.PDIRFrameData.PDIRFrameData[0], pDDB);              
        //copy to pRecordSet_B

        EDDI_MemCopy(pCurrentFrameDataElementLocal, &PDIRData.PDIRFrameData.PDIRFrameData[0], EDDI_PRM_LENGTH_FRAME_DATA);

        Result = EDDI_PDIRFrameDataCheckElement(pDDB, &PDIRData.PDIRFrameData.PDIRFrameData[0]);

        if (EDD_STS_OK != Result)
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result ->:0x%X", Result);
            return Result;
        }
               
        EDDI_PRM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_PrmCheckPDIRData, ElementNr: %d: FrameID: 0x%X,UsrRxPort_0_4: 0x%X, NumberOfTxPortGroups: 0x%X, UsrTxPortGroupArray: 0x%X" 
                          , ElementItem 
                          , PDIRData.PDIRFrameData.PDIRFrameData[0].FrameID
                          , PDIRData.PDIRFrameData.PDIRFrameData[0].UsrRxPort_0_4
                          , PDIRData.PDIRFrameData.PDIRFrameData[0].NumberOfTxPortGroups
                          , PDIRData.PDIRFrameData.PDIRFrameData[0].UsrTxPortGroupArray[0]);

        /* Special handling with Secondary RTSync - Frame.                            */
        /* We substitute the FrameID of a secondary RTSync frame with a Dummy-FrameID */
        /* within the local-Element so we got no conflict with primary FrameID (which */
        /* is the same. This is only done within the local structure!                 */

        #if defined (EDDI_CFG_NO_SYNCINRED)
        //check for SyncFrame and Syncmaster
        if (   (EDDI_GetBitField8Bit(PDIRData.PDIRFrameData.PDIRFrameData[0].FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME) == SYNC_FRAME_DETAILS_NO_SYNC_FRAME) /* no syncframe */
            || (0 != PDIRData.PDIRFrameData.PDIRFrameData[0].UsrRxPort_0_4) )                                                                                                          /* no reception ==> no syncmaster */        
        {
            //no Syncframe
            pCurrentFrameDataElementLocal = EDDI_CheckGetNextFrameDataElement(pCurrentFrameDataElementLocal);
            ElementItem++;
        }
        else
        {
            //skip syncframe, keep pCurrentFrameDataElementLocal
            NumberOfElementsCorr++;
        }
        
        #else
        EDDI_PDIRFrameCheckAndSubstituteSecondaryRTSync(pCurrentFrameDataElementLocal);
        pCurrentFrameDataElementLocal = EDDI_CheckGetNextFrameDataElement(pCurrentFrameDataElementLocal);
        ElementItem++;
        #endif
    }
    EDDI_PRM_RESET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pRecord, pPrmWrite->record_data));

    //---------------------------------------------------------------------
    // Building FrameHandlers out of incoming Data
    //---------------------------------------------------------------------

    // Do complex check while building FrameHandler
    if (!EDDI_SyncBuildAllFrameHandler(pDDB,
                                       NumberOfElements,
                                       &pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData.PDIRFrameData[0],
                                       pRecordSet_B))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PRM_BuildAllFrameHandler, Sync_BuildFrameHandler failed");

        EDDI_SyncIrIrRecordCleanUp(pDDB, pRecordSet_B);
        return EDD_STS_ERR_PARAM;
    }

    /* --------------------------------------------------------------------------*/
    /* Check if we have enough pool resources for FCWs                           */
    /* Note: Must be called after successful EDDI_SyncBuildAllFrameHandler() because */
    /*       Metadata collected within EDDI_SyncBuildAllFrameHandler() used by check */
    /* --------------------------------------------------------------------------*/

    if (!EDDI_SyncIrCheckPoolResources(pDDB,pRecordSet_B))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PRM_BuildAllFrameHandler, EDDI_SyncIrCheckPoolResources failed");

        EDDI_SyncIrIrRecordCleanUp(pDDB, pRecordSet_B);
        /* Note: We use EDD_STS_ERR_PARAM because "out of resources" is not supported as PRM-Error */
        /*       CM knows about this special case and will not threat a PARM-Error as fatal        */
        return EDD_STS_ERR_PARAM;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDIRData<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PDIRFrameCheckAndSubstituteSecondaryRTSync()*/
/*                                                                         */
/* D e s c r i p t i o n: Checks if the FrameID is FrameID of RT_SYNC      */
/*                        If so and this is the secondary master we        */
/*                        substitute the FrameID against a internal used   */
/*                        Dummy-FrameID so we can handle both frames       */
/*                        with the FrameHandler independent.               */
/*                                                                         */
/* A r g u m e n t s:     pFrmData. Points to LOCAL frameelement buffer!   */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRFrameCheckAndSubstituteSecondaryRTSync( EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData )
{
    if (   (pFrmData->FrameID == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
        && (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME) == SYNC_FRAME_DETAILS_SECONDARY_SYNC_FRAME))
    {
        //substitute FrameID
        pFrmData->FrameID = (LSA_UINT16)EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckPDIRGlobalDataBlockHeader()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRGlobalDataBlockHeader( const EDDI_PRM_RECORD_HEADER_TYPE  *  const  pBlockHeader,
                                                                            EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB )
{
    LSA_UINT16  locBlockLength = 0; 

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRGlobalDataBlockHeader->");

    if (pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended)
    {
        if (pBlockHeader->BlockVersionLow == EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA)
        {
            locBlockLength = 2 * sizeof(LSA_UINT32);
        }
        if (pBlockHeader->BlockVersionLow == EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12)
        {
            locBlockLength = 4 * sizeof(LSA_UINT32);
        }
       
        if (pBlockHeader->BlockLength != ((EDDI_PRM_LENGTH_PDIR_GLOBAL_DATA - EDDI_PRM_BLOCK_WITHOUT_LENGTH) +
                                           EDDI_PRM_PDIRDATA_LENGTH_GLOBAL_EXT_WITHOUT_PORT +
                                          (pDDB->PRM.PDIRData.pRecordSet_B->PortCnt_Extended * locBlockLength)))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRGlobalData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CheckPDIRGlobalDataBlockHeader Invalid PDIRGlobalDataBlockLen;  -> pBlockHeader->BlockLength:0x%X EDDI_PRM_H_BL_PDIR_GLOBAL_DATA:0x%X",
                              pBlockHeader->BlockLength, EDDI_PRM_H_BL_PDIR_GLOBAL_DATA);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }
    else
    {
        if (pBlockHeader->BlockLength != EDDI_PRM_H_BL_PDIR_GLOBAL_DATA)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRGlobalData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CheckPDIRGlobalDataBlockHeader Invalid PDIRGlobalDataBlockLen;  -> pBlockHeader->BlockLength:0x%X EDDI_PRM_H_BL_PDIR_GLOBAL_DATA:0x%X",
                              pBlockHeader->BlockLength, EDDI_PRM_H_BL_PDIR_GLOBAL_DATA);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    //BlockType is already checked!

    if (pBlockHeader->BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CheckPDIRGlobalDataBlockHeader Invalid BlockVersion; -> pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pBlockHeader->BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //BlockVersionLow is already checked!

    if (pBlockHeader->Padding1_1 != 0x00)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRGlobalDataBlockHeader Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pBlockHeader->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pBlockHeader->Padding1_2 != 0x00)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRGlobalDataBlockHeader Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pBlockHeader->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRGlobalDataBlockHeader<-");

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRDataBlockHeader( const EDDI_PRM_RECORD_HEADER_TYPE  *  const  pBlockHeader,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                                      LSA_UINT32                      const  RecordDataLength )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRDataBlockHeader->");

    if (pBlockHeader->BlockLength != (LSA_UINT16)(RecordDataLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRDataBlockHeader, pBlockHeader->BlockLength > ->:0x%X :0x%X",
                          pBlockHeader->BlockLength, RecordDataLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //BlockType is already checked!

    if (pBlockHeader->BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRDataBlockHeader, pBlockHeader->BlockVersionHigh > ->:0x%X :0x%X",
                          pBlockHeader->BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //BlockVersionLow is already checked!

    if (pBlockHeader->Padding1_1 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRDataBlockHeader Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pBlockHeader->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pBlockHeader->Padding1_2 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRDataBlockHeader Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pBlockHeader->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRDataBlockHeader<-");

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
 LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRFrameDataBlockHeader( const EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  *  const  pPDIRFrameData,
                                                                            EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB )
{
    LSA_UINT16  L_BlockLength;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRFrameDataBlockHeader->");

    L_BlockLength = pPDIRFrameData->BlockHeader.BlockLength - EDDI_PRM_BLOCK_WITHOUT_LENGTH;

    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pPDIRFrameData->BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
    {
        L_BlockLength -= sizeof(pPDIRFrameData->FrameDataProperties);
    }

    if (    (L_BlockLength < EDDI_PRM_LENGTH_FRAME_DATA)
         || (L_BlockLength > (EDDI_SYNC_MAX_NUMBER_OF_FRAME_ELEMENTS * EDDI_PRM_LENGTH_FRAME_DATA)) 
       )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader, pBlockHeader->BlockLength > ->:0x%X :0x%X",
                          pPDIRFrameData->BlockHeader.BlockLength, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if ((((LSA_UINT16)(L_BlockLength)) % EDDI_PRM_LENGTH_FRAME_DATA) != 0)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_NumberOfElements_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader, pBlockHeader->BlockLength > ->:0x%X :0x%X",
                          pPDIRFrameData->BlockHeader.BlockLength, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //BlockType is already checked!

    if (pPDIRFrameData->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader, pBlockHeader->BlockVersionHigh > ->:0x%X :0x%X",
                          pPDIRFrameData->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (   (pPDIRFrameData->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
        && (pPDIRFrameData->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader , pBlockHeader->BlockVersionLow > ->:0x%X :0x%X",
                          pPDIRFrameData->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDIRFrameData->BlockHeader.Padding1_1 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pPDIRFrameData->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_DATA;
    }

    if (pPDIRFrameData->BlockHeader.Padding1_2 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pPDIRFrameData->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRFrameDataBlockHeader<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckPDIRBeginEndBlockHeader()              */
/*                                                                         */
/* D e s c r i p t i o n: Checks the Block Header of PDIRBeginEnd Block    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRBeginEndBlockHeader( const EDDI_PRM_RECORD_HEADER_TYPE  *  const  pBlockHeader,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB )
{
    LSA_UINT16  L_BlockLength;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRBeginEndBlockHeader->");

    L_BlockLength = pBlockHeader->BlockLength;

    if (L_BlockLength < (sizeof(EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE) - EDDI_PRM_BLOCK_WITHOUT_LENGTH))  /* Header (version+padding + NumberOfPorts field */
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRFrameDataBlockHeader, pBlockHeader->BlockLength > ->:0x%X :0x%X",
                          pBlockHeader->BlockLength, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //BlockType is already checked!
    if (pBlockHeader->BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRBeginEndBlockHeader, pBlockHeader->BlockVersionHigh > ->:0x%X :0x%X",
                          pBlockHeader->BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pBlockHeader->BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRBeginEndBlockHeader , pBlockHeader->BlockVersionLow > ->:0x%X :0x%X",
                          pBlockHeader->BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pBlockHeader->Padding1_1 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRBeginEndBlockHeader Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pBlockHeader->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_DATA;
    }

    if (pBlockHeader->Padding1_2 != 0)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPDIRBeginEndBlockHeader Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pBlockHeader->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPDIRBeginEndBlockHeader<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*********************************************************************************/
/* F u n c t i o n:       EDDI_PDIRBeginEndValidate()                            */
/*                                                                               */
/* D e s c r i p t i o n: Validates the BeginEndData coming from PRM             */
/*                        for Value-Range and consistency for this port          */
/*                        and build up Groupmanagment                            */
/*                                                                               */
/* Type:                                                                         */
/*                                                                               */
/* Type with TX:                                                                 */
/*                                                                               */
/* ORANGE     : RedOrangePeriodBegin = OrangePeriodBegin < GreenPeriodBegin      */
/* RED        : RedOrangePeriodBegin < OrangePeriodBegin = GreenPeriodBegin      */
/* NONE       : RedOrangePeriodBegin = OrangePeriodBegin = GreenPeriodBegin  = 0 */
/*                                                                               */
/* Type with RX (Rx has no ORANGE phase)                                         */
/*                                                                               */
/* RED        : RedOrangePeriodBegin < OrangePeriodBegin = GreenPeriodBegin      */
/* NONE       : RedOrangePeriodBegin = OrangePeriodBegin = GreenPeriodBegin  = 0 */
/*                                                                               */
/*                                                                               */
/* Rules:                                                                        */
/*                                                                               */
/* - Rx has no ORANGE phase                                                      */
/* - ORANGE tx-Phases must begin with 0 (OrangePeriodBegin = 0)                  */
/* - We only support EDDI_IRT_MAX_ORANGE_END_CNT different ORANGE                */
/*   (with RED_ORANGE and ORANGE tx-Phases)                                      */
/*   Note: this is over all ports! here we only check for one Port!              */
/*         addition check needed outside                                         */
/* - We only support EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT different RED begin per   */
/*   rx/tx phases (with RED_ORANGE and RED phases)                               */
/* - We only support EDDI_IRT_MAX_RED_ORANGE_END_CNT different RED end per       */
/*   rx/tx phases (with RED_ORANGE and RED phases)                               */
/*                                                                               */
/* Return Value:           EDD_STS_OK                                            */
/*                         EDD_STS_ERR_PRM_BLOCK                                 */
/*********************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndValidate( EDDI_LOCAL_DDB_PTR_TYPE                               const  pDDB, 
                                                            EDDI_IRT_BEGIN_END_DATA_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  pBeginEndData )
{
    LSA_RESULT  Status;
    LSA_UINT32  i, j, Max;
    LSA_BOOL    Found;
    LSA_UINT32  RedOrangePeriodBeginAssignment,OrangePeriodBeginAssignment,GreenPeriodBeginAssignment;
    LSA_UINT32  RxRedOrangePeriodBeginMin,TxRedOrangePeriodBeginMin;
    LSA_UINT32  ErrOffsetAccumulated  = 0;
    LSA_UINT32  ErrOffset;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate->");

    Status = EDD_STS_OK;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: NumberOfAssignments: %d (0x%X)",pBeginEndData->NumberOfAssignments,pBeginEndData->NumberOfAssignments);
    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: NumberOfPhases     : %d (0x%X)",pBeginEndData->NumberOfPhases,pBeginEndData->NumberOfPhases);

    /* check for limits of timevalues */
    for (i=0; i<pBeginEndData->NumberOfAssignments; i++)
    {
        ErrOffset = EDDI_PRM_ERR_INVALID_OFFSET;

        EDDI_PRM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: NumberOfAssignments: %d, Tx.RedOrangePeriodBegin: %d, Tx.OrangePeriodBegin: %d, Tx.GreenPeriodBegin: %d, Rx.RedOrangePeriodBegin: %d, Rx.OrangePeriodBegin: %d, Rx.GreenPeriodBegin: %d",
                          i,
                          pBeginEndData->Assignment[i].Tx.RedOrangePeriodBegin,
                          pBeginEndData->Assignment[i].Tx.OrangePeriodBegin,
                          pBeginEndData->Assignment[i].Tx.GreenPeriodBegin,
                          pBeginEndData->Assignment[i].Rx.RedOrangePeriodBegin,
                          pBeginEndData->Assignment[i].Rx.OrangePeriodBegin,
                          pBeginEndData->Assignment[i].Rx.GreenPeriodBegin );

        /* check limits */
        if      ( pBeginEndData->Assignment[i].Tx.RedOrangePeriodBegin  > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Tx_RedOrangePeriodBegin;
        }
        else if ( pBeginEndData->Assignment[i].Tx.OrangePeriodBegin     > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Tx_OrangePeriodBegin;
        }
        else if ( pBeginEndData->Assignment[i].Tx.GreenPeriodBegin      > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Tx_GreenPeriodBegin;
        }
        else if ( pBeginEndData->Assignment[i].Rx.RedOrangePeriodBegin  > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Rx_RedOrangePeriodBegin;
        }
        else if ( pBeginEndData->Assignment[i].Rx.OrangePeriodBegin     > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Rx_OrangePeriodBegin;
        }
        else if ( pBeginEndData->Assignment[i].Rx.GreenPeriodBegin      > EDDI_IRT_MAX_PERIOD_TIME )
        {
            ErrOffset = EDDI_IRT_BEGIN_END_OFFSET_Rx_GreenPeriodBegin;
        }
        
        if (EDDI_PRM_ERR_INVALID_OFFSET != ErrOffset)
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, Invalid PeriodTime.");

            if (EDD_STS_OK == Status)
            {
                EDDI_PRM_SET_ERR_OFFSET(pDDB, ErrOffsetAccumulated + ErrOffset);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
        ErrOffsetAccumulated += sizeof(EDDI_IRT_TXRX_BEGIN_END_TYPE);
    }

    i = 0;

    /* Note: GroupsInfo already set to 0 outside */

    RxRedOrangePeriodBeginMin = 0xFFFFFFFFL;     /* start with max */
    TxRedOrangePeriodBeginMin = 0xFFFFFFFFL;     /* start with max */

    ErrOffsetAccumulated += sizeof(LSA_UINT32);  // NumberOfPhases

    /* check PhaseAssignments and buildUp Groups*/
    while ((Status == EDD_STS_OK) && (i < pBeginEndData->NumberOfPhases))
    {
        ErrOffset = EDDI_PRM_ERR_INVALID_OFFSET;
        /* ----------------------------------------------------------------------*/
        /* Check PhaseAssignments                                                */
        /* ----------------------------------------------------------------------*/

        EDDI_PRM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d, Tx.RedOrangePeriodBeginAssignment: %d, Tx.OrangePeriodBeginAssignment: %d, Tx.GreenPeriodBeginAssignment: %d, Rx.RedOrangePeriodBeginAssignment: %d, Rx.OrangePeriodBeginAssignment: %d, Rx.GreenPeriodBeginAssignment: %d",
                          i,
                          pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginAssignment,
                          pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodBeginAssignment,
                          pBeginEndData->PhaseAssignment[i].Tx.GreenPeriodBeginAssignment,
                          pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginAssignment,
                          pBeginEndData->PhaseAssignment[i].Rx.OrangePeriodBeginAssignment,
                          pBeginEndData->PhaseAssignment[i].Rx.GreenPeriodBeginAssignment);


        if      (pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginAssignment  >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_RedOrangePeriodBeginAssignment;
        }
        else if (pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodBeginAssignment     >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_OrangePeriodBeginAssignment;
        }
        else if (pBeginEndData->PhaseAssignment[i].Tx.GreenPeriodBeginAssignment      >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_GreenPeriodBeginAssignment;
        }
        else if (pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginAssignment  >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_RedOrangePeriodBeginAssignment;
        }
        else if (pBeginEndData->PhaseAssignment[i].Rx.OrangePeriodBeginAssignment     >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_OrangePeriodBeginAssignment;
        }
        else if (pBeginEndData->PhaseAssignment[i].Rx.GreenPeriodBeginAssignment      >= pBeginEndData->NumberOfAssignments )
        {
            ErrOffset = EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_GreenPeriodBeginAssignment;
        }


        if (EDDI_PRM_ERR_INVALID_OFFSET != ErrOffset)
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate: Invalid PhaseAssignment (Assignment dont exist). Assigment: %d (0x%X)",i,i);
            if (EDD_STS_OK == Status)
            {
                EDDI_PRM_SET_ERR_OFFSET(pDDB, ErrOffsetAccumulated + ErrOffset);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
        else
        {
            RedOrangePeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginAssignment;
            OrangePeriodBeginAssignment    = pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodBeginAssignment;
            GreenPeriodBeginAssignment     = pBeginEndData->PhaseAssignment[i].Tx.GreenPeriodBeginAssignment;
            
            /* -------------------------------------------------------------------- */
            /* Any configuration with a RED and an ORANGE period is not supported   */
            /*  1) RedOrangePeriodBegin < OrangePeriodBegin < GreenPeriodBegin      */
            /*  2)     RedOrangePeriodBegin = OrangePeriodBegin < GreenPeriodBegin  */
            /*     and RedOrangePeriodBegin < OrangePeriodBegin = GreenPeriodBegin  */
            /* -------------------------------------------------------------------- */

            if ( 
                  /*Case 1)*/
                  ( ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin   <  pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin  )  && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin < pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin))
               || 
                  /*Case 2)*/
                  (
                    (( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin  == pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin  )  && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin < pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin ))
                    && 
                    (( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin  <  pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin  )  && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin == pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin ))
                  )
               )
            {
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, Error: Red or Orange Phase is not allowed");
                Status = EDD_STS_ERR_PRM_BLOCK;
            }


            /* -------------------------------------------------------------------- */
            /* check Tx combination per phase..(RED_ORANGE,ORANGE,RED or NONE)      */
            /* 1) RedOrangePeriodBegin = OrangePeriodBegin < GreenPeriodBegin       */
            /* 2) RedOrangePeriodBegin < OrangePeriodBegin = GreenPeriodBegin       */
            /* 3) RedOrangePeriodBegin = OrangePeriodBegin = GreenPeriodBegin  = 0  */
            /* All other cases -> error                                             */
            /* -------------------------------------------------------------------- */

            if (EDD_STS_OK == Status)
            {
                if (    ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin == pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin  ) 
                     && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin < pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin ))
                {
                    /* case 1). OK */
                    pBeginEndData->PhaseAssignment[i].Tx.Type = EDDI_IRT_PHASE_TYPE_ORANGE;
                    pBeginEndData->bAtLeastOrangePresent = LSA_TRUE;

                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d. Tx is ORANGE",i);

                    /* ORANGE-Tx Phases must start at 0 ! */
                    if (pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin != 0)
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate: ORANGE-Tx-Phase, but OrangePeriodBegin is not 0. Phase: %d, Begin: %d",i,pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }
                else
                {
                    if ( ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin  < pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin  ) &&
                         ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin       == pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin ))
                    {
                        /* case 2). OK */
                        pBeginEndData->PhaseAssignment[i].Tx.Type = EDDI_IRT_PHASE_TYPE_RED;
                        pBeginEndData->bAtLeastRedPresent = LSA_TRUE;
                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d. Tx is RED",i);
                    }
                    else
                    {
                        if (   ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin == 0 ) 
                            && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin       == 0 ) 
                            && ( pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin         == 0 ))
                        {
                            /* case 3). OK */
                            pBeginEndData->PhaseAssignment[i].Tx.Type = EDDI_IRT_PHASE_TYPE_NONE;
                            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d. Tx has no reserved period",i);
                        }
                        else
                        {
                            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, Invalid TxPeriodTime combination.");
                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                    }
                }
            
                /* --------------------------------------------------------------------*/
                /* check Rx combination per Phase! (RED or NONE)                       */
                /* 3) RedOrangePeriodBegin < OrangePeriodBegin = GreenPeriodBegin      */
                /* 4) RedOrangePeriodBegin = OrangePeriodBegin = GreenPeriodBegin  = 0 */
                /* All other cases -> error                                            */
                /* --------------------------------------------------------------------*/

                RedOrangePeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginAssignment;
                OrangePeriodBeginAssignment    = pBeginEndData->PhaseAssignment[i].Rx.OrangePeriodBeginAssignment;
                GreenPeriodBeginAssignment     = pBeginEndData->PhaseAssignment[i].Rx.GreenPeriodBeginAssignment;

                if (    ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Rx.RedOrangePeriodBegin  <  pBeginEndData->Assignment[OrangePeriodBeginAssignment].Rx.OrangePeriodBegin) 
                     && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Rx.OrangePeriodBegin        == pBeginEndData->Assignment[GreenPeriodBeginAssignment].Rx.GreenPeriodBegin ))
                {
                    /* case 3). OK */
                    pBeginEndData->PhaseAssignment[i].Rx.Type = EDDI_IRT_PHASE_TYPE_RED;
                    pBeginEndData->bAtLeastRedPresent = LSA_TRUE;
                    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d. Rx is RED",i);
                }
                else
                {
                    if (    ( pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Rx.RedOrangePeriodBegin == 0 ) 
                         && ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Rx.OrangePeriodBegin       == 0 ) 
                         && ( pBeginEndData->Assignment[GreenPeriodBeginAssignment].Rx.GreenPeriodBegin         == 0 ))
                    {
                        /* case 4). OK */
                        pBeginEndData->PhaseAssignment[i].Rx.Type = EDDI_IRT_PHASE_TYPE_NONE;
                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d. Rx has no reserved period",i);
                    }
                    else
                    {
                        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, Invalid RxPeriodTime combination.");
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }

            } /* else */
        }

        /* ----------------------------------------------------------------------*/
        /* Build RxRedOrangePeriodBegin GroupsInfo for RED phases                */
        /* The GroupInfo hold all used different times <> 0.                     */
        /* We only support a limited number of different times!                  */
        /* If a entry is 0 it is not used. We fill up from bottom to top so if   */
        /* we see a 0 we have reached the top off used elements.                 */
        /* ----------------------------------------------------------------------*/

        RedOrangePeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginAssignment;
        pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginGroup = 0; /* default */

        if (( Status == EDD_STS_OK) &&
            ( pBeginEndData->PhaseAssignment[i].Rx.Type == EDDI_IRT_PHASE_TYPE_RED))
        {
            if (pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Rx.RedOrangePeriodBegin) /* Begin <> 0 */
            {
                Found = LSA_FALSE;
                j = 0;

                /* is 0 already used as starttime? */
                if ( pBeginEndData->Groups.RxRedOrangePeriodBegin0Present )
                {
                    Max  = EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT;
                    Max -= 1; // 2 line assignment just to satisfying lint
                }
                else
                {
                    Max = EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT;
                }

                while ((j<Max) && (!Found))
                {
                    if ( pBeginEndData->Groups.RxRedOrangePeriodBegin[j] == 0 ) /* free entry */
                    {
                        /* Store this one */
                        pBeginEndData->Groups.RxRedOrangePeriodBegin[j] = pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Rx.RedOrangePeriodBegin;
                        pBeginEndData->Groups.RxRedOrangePeriodBeginGroupCnt++;
                        pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginGroup = j+1;

                        /* check if this is a new MIN value. if so store it */
                        if (pBeginEndData->Groups.RxRedOrangePeriodBegin[j] < RxRedOrangePeriodBeginMin)
                        {
                            pBeginEndData->Groups.RxRedOrangePeriodBeginMin = pBeginEndData->Groups.RxRedOrangePeriodBegin[j];
                            RxRedOrangePeriodBeginMin = pBeginEndData->Groups.RxRedOrangePeriodBegin[j];
                        }

                        Found = LSA_TRUE;

                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Grp: RxRedOrangePeriodBegin[%d]    : 0x%X",j,pBeginEndData->Groups.RxRedOrangePeriodBegin[j]);
                    }
                    else
                    {
                        if (pBeginEndData->Groups.RxRedOrangePeriodBegin[j] == pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Rx.RedOrangePeriodBegin)
                        {
                            /* Found this value */
                            Found = LSA_TRUE;
                            pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginGroup = j+1;
                        }
                    }
                    j++;
                }

                if (!Found)
                {
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, More than 1 RxRedOrangePeriodBeginAssignments (<>0).");
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else
            {
                /* check if we have reached our maximum of possible start-values      */
                /* we only support EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT different values incl. 0 */

                if ( pBeginEndData->Groups.RxRedOrangePeriodBeginGroupCnt == EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT )
                {
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, More than 1 RxRedOrangePeriodBeginAssignments (==0).");
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
                else
                {
                    pBeginEndData->Groups.RxRedOrangePeriodBegin0Present = LSA_TRUE;
                    RxRedOrangePeriodBeginMin = 0;
                }
            }
        }

        /* ----------------------------------------------------------------------*/
        /* Build TxRedOrangePeriodBegin GroupsInfo for RED Phases                      */
        /* The GroupInfo hold all used different times <> 0.                     */
        /* We only support a limited number of different times!                  */
        /* If a entry is 0 it is not used. We fill up from bottom to top so if   */
        /* we see a 0 we have reached the top off used elements.                 */
        /* ----------------------------------------------------------------------*/

        RedOrangePeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginAssignment;
        pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginGroup = 0; /* default */

        if (   (Status == EDD_STS_OK) 
            && (pBeginEndData->PhaseAssignment[i].Tx.Type == EDDI_IRT_PHASE_TYPE_RED))
        {

            if (pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin) /* Begin <> 0 */
            {
                Found = LSA_FALSE;
                j = 0;

                /* is 0 already used as starttime? */
                if ( pBeginEndData->Groups.TxRedOrangePeriodBegin0Present )
                {
                    Max  = EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT;
                    Max -= 1; // 2 line assignment just to satisfying lint
                }
                else
                {
                    Max = EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT;
                }

                while ((j<Max) && (!Found))
                {
                    if ( pBeginEndData->Groups.TxRedOrangePeriodBegin[j] == 0 ) /* free entry */
                    {
                        /* Store this one */
                        pBeginEndData->Groups.TxRedOrangePeriodBegin[j] = pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin;
                        pBeginEndData->Groups.TxRedOrangePeriodBeginGroupCnt++;
                        pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginGroup = j+1;
                        Found = LSA_TRUE;

                        /* check if this is a new MIN value. if so store it */
                        if ( pBeginEndData->Groups.TxRedOrangePeriodBegin[j] < TxRedOrangePeriodBeginMin)
                        {
                            pBeginEndData->Groups.TxRedOrangePeriodBeginMin = pBeginEndData->Groups.TxRedOrangePeriodBegin[j];
                            TxRedOrangePeriodBeginMin = pBeginEndData->Groups.TxRedOrangePeriodBegin[j];
                        }

                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Grp: TxRedOrangePeriodBegin[%d]    : 0x%X",j,pBeginEndData->Groups.TxRedOrangePeriodBegin[j]);

                    }
                    else
                    {
                        if (pBeginEndData->Groups.TxRedOrangePeriodBegin[j] == pBeginEndData->Assignment[RedOrangePeriodBeginAssignment].Tx.RedOrangePeriodBegin )
                        {
                            /* Found this value */
                            Found = LSA_TRUE;
                            pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginGroup = j+1;
                        }
                    }
                    j++;
                }

                if (!Found)
                {
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, More than 1 TxRedOrangePeriodBeginAssignments (<>0).");
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else
            {
                /* check if we have reached our maximum of possible start-values      */
                /* we only support EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT different values incl. 0 */

                if (pBeginEndData->Groups.TxRedOrangePeriodBeginGroupCnt == EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT)
                {
                    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate, More than 1 RxRedOrangePeriodBeginAssignments (==0).");
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
                else
                {
                    pBeginEndData->Groups.TxRedOrangePeriodBegin0Present = LSA_TRUE;
                    TxRedOrangePeriodBeginMin = 0;
                }
            }

        }
        /* ----------------------------------------------------------------------*/
        /* Build RxRedPeriodEndGroup GroupsInfo for RED Phases                   */
        /*                                                                       */
        /* RxRedOrangePeriodEnd := RxGreenPeriodBegin if a RED-Phase is present else */
        /*                   not present.                                        */
        /*                                                                       */
        /* The GroupInfo hold all used different times <> 0.                     */
        /* We only support a limited number of different times!                  */
        /* If a entry is 0 it is not used. We fill up from bottom to top so if   */
        /* we see a 0 we have reached the top off used elements.                 */
        /* ----------------------------------------------------------------------*/

        GreenPeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Rx.GreenPeriodBeginAssignment;
        pBeginEndData->PhaseAssignment[i].Rx.RedPeriodEndGroup       = 0; /* default   */

        if (( Status == EDD_STS_OK) &&
            (
                ( pBeginEndData->PhaseAssignment[i].Rx.Type == EDDI_IRT_PHASE_TYPE_RED )
            ) &&
            ( pBeginEndData->Assignment[GreenPeriodBeginAssignment].Rx.GreenPeriodBegin)) /* End <> 0. must be */
        {
            LSA_UINT32  const  RxGreenPeriodBeginLocal = pBeginEndData->Assignment[GreenPeriodBeginAssignment].Rx.GreenPeriodBegin;

            Found = LSA_FALSE;
            j = 0;
            while ((j<EDDI_IRT_MAX_RED_ORANGE_END_CNT) && (!Found))
            {
                if (pBeginEndData->Groups.RxRedOrangePeriodEnd[j] == 0) /* free entry */
                {
                    /* Store this one */
                    pBeginEndData->Groups.RxRedOrangePeriodEnd[j] = RxGreenPeriodBeginLocal;
                    pBeginEndData->Groups.RxRedPeriodEndGroupCnt++;
                    if (RxGreenPeriodBeginLocal > pBeginEndData->Groups.RxRedPeriodEndMax)
                    {
                        pBeginEndData->Groups.RxRedPeriodEndMax = RxGreenPeriodBeginLocal;
                    }
                    pBeginEndData->PhaseAssignment[i].Rx.RedPeriodEndGroup = j+1;
                    Found = LSA_TRUE;

                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Grp: RxRedOrangePeriodEnd[%d]      : 0x%X",j,pBeginEndData->Groups.RxRedOrangePeriodEnd[j] );
                }
                else
                {
                    if (pBeginEndData->Groups.RxRedOrangePeriodEnd[j] == RxGreenPeriodBeginLocal)
                    {
                        /* Found this value */
                        Found = LSA_TRUE;
                        pBeginEndData->PhaseAssignment[i].Rx.RedPeriodEndGroup = j+1;
                    }
                }
                j++;
            }

            if (!Found)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate: Too many different RxGreenPeriodBeginAssignments. Allowed:%d",EDDI_IRT_MAX_RED_ORANGE_END_CNT);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }

        /* ----------------------------------------------------------------------*/
        /* Build TxOrangePeriodEnd GroupsInfo for phases with ORANGE part        */
        /* The GroupInfo hold all used different times <> 0.                     */
        /* We only support a limited number of different times!                  */
        /* If a entry is 0 it is not used. We fill up from bottom to top so if   */
        /* we see a 0 we have reached the top off used elements.                 */
        /* ----------------------------------------------------------------------*/

        GreenPeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Tx.GreenPeriodBeginAssignment;
        pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodEndGroup = 0; /* default */

        if (     ( Status == EDD_STS_OK) 
             &&  ( pBeginEndData->PhaseAssignment[i].Tx.Type == EDDI_IRT_PHASE_TYPE_ORANGE ) 
             &&  ( pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin)) /* End <> 0. must be */
        {
            Found = LSA_FALSE;
            j = 0;
            while ((j<EDDI_IRT_MAX_ORANGE_END_CNT) && (!Found))
            {
                if (pBeginEndData->Groups.TxOrangePeriodEnd[j] == 0) /* free entry */
                {
                    /* Store this one */
                    pBeginEndData->Groups.TxOrangePeriodEnd[j] = pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin;
                    pBeginEndData->Groups.TxOrangePeriodEndGroupCnt++;
                    pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodEndGroup = j+1;
                    Found = LSA_TRUE;

                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Grp: TxOrangePeriodEnd[%d]   : 0x%X",j,pBeginEndData->Groups.TxOrangePeriodEnd[j]);
                }
                else
                {
                    if (pBeginEndData->Groups.TxOrangePeriodEnd[j] == pBeginEndData->Assignment[GreenPeriodBeginAssignment].Tx.GreenPeriodBegin)
                    {
                        /* Found this value */
                        Found = LSA_TRUE;
                        pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodEndGroup = j+1;
                    }
                }
                j++;
            }

            if (!Found)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate: Too many different TxGreenPeriodBeginAssignments. Allowed:%d",EDDI_IRT_MAX_ORANGE_END_CNT);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }

        /* ----------------------------------------------------------------------*/
        /* Build TxRedOrangePeriodEnd GroupsInfo for RED phases                        */
        /*                                                                       */
        /* TxRedOrangePeriodEnd := TxOrangePeriodBegin if a RED-Phase is present else  */
        /*                   not present.                                        */
        /*                                                                       */
        /* The GroupInfo hold all used different times <> 0.                     */
        /* We only support a limited number of different times!                  */
        /* If a entry is 0 it is not used. We fill up from bottom to top so if   */
        /* we see a 0 we have reached the top off used elements.                 */
        /* ----------------------------------------------------------------------*/

        OrangePeriodBeginAssignment = pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodBeginAssignment;
        pBeginEndData->PhaseAssignment[i].Tx.RedPeriodEndGroup = 0; /* default */

        if (
                ( Status == EDD_STS_OK) 
            &&  ( pBeginEndData->PhaseAssignment[i].Tx.Type == EDDI_IRT_PHASE_TYPE_RED )
            &&  ( pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin)) /* <> 0. must be */
        {
            LSA_UINT32  const  TxOrangePeriodBeginLocal = pBeginEndData->Assignment[OrangePeriodBeginAssignment].Tx.OrangePeriodBegin;

            Found = LSA_FALSE;
            j = 0;
            while ((j<EDDI_IRT_MAX_RED_ORANGE_END_CNT) && (!Found))
            {
                if (pBeginEndData->Groups.TxRedOrangePeriodEnd[j] == 0 ) /* free entry */
                {
                    /* Store this one */
                    pBeginEndData->Groups.TxRedOrangePeriodEnd[j] = TxOrangePeriodBeginLocal;
                    pBeginEndData->Groups.TxRedPeriodEndGroupCnt++;
                    if (TxOrangePeriodBeginLocal > pBeginEndData->Groups.TxRedPeriodEndMax)
                    {
                        pBeginEndData->Groups.TxRedPeriodEndMax      = TxOrangePeriodBeginLocal;
                        pBeginEndData->Groups.TxRedPeriodEndMaxIndex = j;
                    }
                    pBeginEndData->PhaseAssignment[i].Tx.RedPeriodEndGroup = j+1;
                    Found = LSA_TRUE;

                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Grp: TxRedOrangePeriodEnd[%d]      : 0x%X",j,pBeginEndData->Groups.TxRedOrangePeriodEnd[j]);
                }
                else
                {
                    if (pBeginEndData->Groups.TxRedOrangePeriodEnd[j] == TxOrangePeriodBeginLocal)
                    {
                        /* Found this value */
                        Found = LSA_TRUE;
                        pBeginEndData->PhaseAssignment[i].Tx.RedPeriodEndGroup = j+1;
                    }
                }
                j++;
            }

            if (!Found)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidate: Too many different TxOrangePeriodBeginAssignments. Allowed:%d",EDDI_IRT_MAX_RED_ORANGE_END_CNT);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }

        if (Status == EDD_STS_OK)
        {
            EDDI_PRM_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: Phase: %d, Tx.RedOrangePeriodBeginGrp %d, Tx.RedPeriodEndGrp %d, Tx.OrangePeriodEndGrp %d, Rx.RedOrangePeriodBeginGrp %d, Rx.RedPeriodEndGrp %d" ,
                              i+1,
                              pBeginEndData->PhaseAssignment[i].Tx.RedOrangePeriodBeginGroup,
                              pBeginEndData->PhaseAssignment[i].Tx.RedPeriodEndGroup,
                              pBeginEndData->PhaseAssignment[i].Tx.OrangePeriodEndGroup,
                              pBeginEndData->PhaseAssignment[i].Rx.RedOrangePeriodBeginGroup,
                              pBeginEndData->PhaseAssignment[i].Rx.RedPeriodEndGroup);
        }

        /* ----------------------------------------------------------------------*/

        i++;
        ErrOffsetAccumulated += sizeof(LSA_UINT32);  // sizeof Phase information
    } /* while */

    if (Status == EDD_STS_OK)
    {
        EDDI_PRM_TRACE_11(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate: TxRedPeriod: BeginGroupCnt: %d, EndGroupCnt: %d, TxOrangePeriodEndGroupCnt: %d, RxRedPeriod: BeginGroupCnt: %d  EndGroupCnt: %d, TxRedOrangePeriodBegin0Present: %d, RxRedOrangePeriodBegin0Present: %d, TxRedOrangePeriodBeginMin: %d, RxRedOrangePeriodBeginMin: %d, TxRedPeriodEndMax: %d, RxRedPeriodEndMax: %d",
                          pBeginEndData->Groups.TxRedOrangePeriodBeginGroupCnt,
                          pBeginEndData->Groups.TxRedPeriodEndGroupCnt,
                          pBeginEndData->Groups.TxOrangePeriodEndGroupCnt,
                          pBeginEndData->Groups.RxRedOrangePeriodBeginGroupCnt,
                          pBeginEndData->Groups.RxRedPeriodEndGroupCnt,
                          pBeginEndData->Groups.TxRedOrangePeriodBegin0Present,
                          pBeginEndData->Groups.RxRedOrangePeriodBegin0Present,
                          pBeginEndData->Groups.TxRedOrangePeriodBeginMin,
                          pBeginEndData->Groups.RxRedOrangePeriodBeginMin,
                          pBeginEndData->Groups.TxRedPeriodEndMax,
                          pBeginEndData->Groups.RxRedPeriodEndMax);
    }


    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidate, < Status: %d (0x%X)", Status, Status);

    LSA_UNUSED_ARG(pDDB);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       PDIRBeginEnd_DetermineClassType()                */
/*                                                                         */
/* D e s c r i p t i o n: Determines the total Class Type over all Ports   */
/*                        and directions and stores it within record-set   */
/*                        management "BeginEndIRTType"                     */
/*                                                                         */
/*                        Prio:   EDDI_IRT_PHASE_TYPE_RED  or              */
/*                                EDDI_IRT_PHASE_TYPE_ORANGE               */
/*                                EDDI_IRT_PHASE_TYPE_NONE                 */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/***************************************************************************/
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  PDIRBeginEnd_DetermineClassType( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                       EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_RESULT      Status;
    LSA_UINT32      i, UsrPortIndex;
    LSA_UINT32      Type;
    LSA_BOOL        Red,Orange;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "PDIRBeginEnd_DetermineClassType->");

    Status       = EDD_STS_OK;
    UsrPortIndex = 0;
    Type         = EDDI_IRT_PHASE_TYPE_NONE;

    Red    = LSA_FALSE;
    Orange = LSA_FALSE;

    /* loop over all Ports */
    while ((Status == EDD_STS_OK) && (UsrPortIndex < EDD_CFG_MAX_PORT_CNT))
    {
        /* Assignment present ? */
        if (pRecordSet->BeginEndData[UsrPortIndex].bPresent)
        {
            for ( i= 0; i<pRecordSet->BeginEndData[UsrPortIndex].NumberOfPhases; i++)
            {
                /* check Tx-Type */
                switch (pRecordSet->BeginEndData[UsrPortIndex].PhaseAssignment[i].Tx.Type)
                {
                    case EDDI_IRT_PHASE_TYPE_RED:
                        Red    = LSA_TRUE;
                        break;
                    case EDDI_IRT_PHASE_TYPE_ORANGE:
                        Orange = LSA_TRUE;
                        break;
                    case EDDI_IRT_PHASE_TYPE_NONE:
                    default:
                        break;
                }

                /* check Rx-Type */
                switch (pRecordSet->BeginEndData[UsrPortIndex].PhaseAssignment[i].Rx.Type)
                {
                    case EDDI_IRT_PHASE_TYPE_RED:
                        Red    = LSA_TRUE;
                        break;
                    case EDDI_IRT_PHASE_TYPE_ORANGE:
                        Orange = LSA_TRUE;
                        break;
                    case EDDI_IRT_PHASE_TYPE_NONE:
                    default:
                        break;
                }
            }
        }

        UsrPortIndex++;
    }

    /* Determine total Type */
    if (Red)
    {
        Type = EDDI_IRT_PHASE_TYPE_RED;
    }
    if (Orange)
    {
        Type = EDDI_IRT_PHASE_TYPE_ORANGE;
    }

    pRecordSet->BeginEndIRTType = Type;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "PDIRBeginEnd_DetermineClassType: Type: %d (0x%X)", Type, Type);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PDIRBeginEndValidateOrangeEnds()            */
/*                                                                         */
/* D e s c r i p t i o n: Validates if we only have                        */
/*                        EDDI_IRT_MAX_ORANGE_END_CNT differnt end times   */
/*                        for ORANGE phases over all ports                 */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndValidateOrangeEnds( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB, 
                                                                      EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet )
{
    LSA_RESULT      Status;
    LSA_UINT32      OrangeEndCnt,i,j,UsrPortIndex;
    LSA_BOOL        Found;
    LSA_UINT32      OrangeEnd[EDDI_IRT_MAX_ORANGE_END_CNT];

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidateOrangeEnds->");

    Status       = EDD_STS_OK;
    UsrPortIndex = 0;
    OrangeEndCnt = 0;

    for (i=0; i<EDDI_IRT_MAX_ORANGE_END_CNT; i++)
    {
        OrangeEnd[i] = 0xFFFF;
    }

    /* loop over all Ports */
    while ((Status == EDD_STS_OK) && (UsrPortIndex < EDD_CFG_MAX_PORT_CNT))
    {
        /* Assignment present ? */
        if (pRecordSet->BeginEndData[UsrPortIndex].bPresent)
        {
            j = 0;
            /* loop over all ORANGE-End Groupentrys */
            while (( Status == EDD_STS_OK ) &&
                   ( j < pRecordSet->BeginEndData[UsrPortIndex].Groups.TxOrangePeriodEndGroupCnt))
            {
                i = 0;
                Found = LSA_FALSE;
                /* search for already used End-Values within OrangeEnd array */
                while ((i< OrangeEndCnt) && (!Found))
                {
                    if (pRecordSet->BeginEndData[UsrPortIndex].Groups.TxOrangePeriodEnd[j] == OrangeEnd[i] )
                    {
                        Found = LSA_TRUE;
                    }
                    i++;
                }

                /* if no entry was found  */
                if (! Found)
                {
                    if ( i >= EDDI_IRT_MAX_ORANGE_END_CNT )
                    {
                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndValidateOrangeEnds: Too many different ORANGE end times. (only %d allowed)", EDDI_IRT_MAX_ORANGE_END_CNT);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                    else
                    {
                        /* Store new entry */
                        OrangeEnd[i] = pRecordSet->BeginEndData[UsrPortIndex].Groups.TxOrangePeriodEnd[j];
                        OrangeEndCnt = i+1;
                    }
                }

                j++;
            }
        }

        UsrPortIndex++;
    }

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndValidateOrangeEnds, < Status: %d (0x%X)", Status, Status);

    LSA_UNUSED_ARG(pDDB);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PDIRBeginEndParse()                         */
/*                                                                         */
/* D e s c r i p t i o n: Parses the PDIRBeginEndData Block and build      */
/*                        up internal structure block with data.           */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndParse( const EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE  *  const  pBlockBeginEnd,
                                                               EDDI_LOCAL_MEM_U8_PTR_TYPE                          pRecord,
                                                               EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB )
{
    LSA_RESULT                    Status;
    LSA_BOOL                      bRedPhasePresent = LSA_FALSE;
    LSA_UINT16                    BlockLength;
    LSA_UINT32                    RemainingBlockLength,Length;
    EDD_UPPER_MEM_U8_PTR_TYPE     pCurrentPtr;
    LSA_UINT32                    UsrPortIndex,NumberOfPorts;
    LSA_UINT32                    NumberOfAssignments,AssignmentCnt;
    LSA_UINT32                    i;
    LSA_UINT16                    Help1,Help2;
    EDDI_PRM_RECORD_IRT_PTR_TYPE  pRecordSet_B;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndParse->");

    Status = EDD_STS_OK;

    pRecordSet_B = pDDB->PRM.PDIRData.pRecordSet_B;

    /*---------------------------------------------------------------------------*/
    /*   PDIRBeginEndData                                                        */
    /*---------------------------------------------------------------------------*/
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          RedGuard.StartOfRedFrameID                    2    */
    /*    LSA_UINT16          RedGuard.EndOfRedFrameID                      2    */
    /*    LSA_UINT32          NumberOfPorts                                 4    */
    /*                                                                           */
    /*    [NumberOfPorts]                                                        */
    /*      LSA_UINT32        NumberOfAssignments                                */
    /*      [NumberOfAssignments]                                                */
    /*       {                                                                   */
    /*        LSA_UINT32      RedOrangePeriodBegin                               */
    /*        LSA_UINT32      OrangePeriodBegin                                  */
    /*        LSA_UINT32      GreenPeriodBegin      (1)                          */
    /*       } TxBeginEndAssignment                                              */
    /*                                                                           */
    /*       {                                                                   */
    /*        LSA_UINT32      RedOrangePeriodBegin                               */
    /*        LSA_UINT32      OrangePeriodBegin                                  */
    /*        LSA_UINT32      GreenPeriodBegin      (1)                          */
    /*       } RxBeginEndAssignment                                              */
    /*      ]                                                                    */
    /*                                                                           */
    /*      LSA_UINT32        NumberOfPhases                                     */
    /*      [NumberOfPhases]                                                     */
    /*        LSA_UINT16      TxPhaseAssignment                                  */
    /*        LSA_UINT16      RxPhaseAssignment                                  */
    /*      ]                                                                    */
    /*    ]                                                                      */
    /*                                                                           */
    /* (1) actually renamed to GreenPeriodBegin whithin IEC spec after           */
    /*     implementation                                                        */
    /*---------------------------------------------------------------------------*/

    BlockLength = pBlockBeginEnd->BlockHeader.BlockLength;

    pRecordSet_B->RedGuard.StartOfRedFrameID = pBlockBeginEnd->RedGuardStartOfRedFrameID;
    pRecordSet_B->RedGuard.EndOfRedFrameID   = pBlockBeginEnd->RedGuardEndOfRedFrameID;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndParse: StartOfRedFrameID: 0x%X, EndOfRedFrameID: 0x%X",pRecordSet_B->RedGuard.StartOfRedFrameID,pRecordSet_B->RedGuard.EndOfRedFrameID);

    if ((pRecordSet_B->RedGuard.StartOfRedFrameID < EDDI_IRT_RED_GUARD_MIN_RED_FRAME_ID) ||
        (pRecordSet_B->RedGuard.StartOfRedFrameID > EDDI_IRT_RED_GUARD_MAX_RED_FRAME_ID)  )
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid Min/Max RedGuard FrameID. StartOfRedFrameID: 0x%X",pRecordSet_B->RedGuard.StartOfRedFrameID);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_RedGuardStartOfRedFrameID);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    else if ((pRecordSet_B->RedGuard.EndOfRedFrameID < EDDI_IRT_RED_GUARD_MIN_RED_FRAME_ID) ||
        (pRecordSet_B->RedGuard.EndOfRedFrameID > EDDI_IRT_RED_GUARD_MAX_RED_FRAME_ID)  )
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid Min/Max RedGuard FrameID. EndOfRedFrameID: 0x%X",pRecordSet_B->RedGuard.EndOfRedFrameID);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_RedGuardEndOfRedFrameID);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    NumberOfPorts = pBlockBeginEnd->NumberOfPorts;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndParse: NumberOfPorts: %d (0x%X)",NumberOfPorts,NumberOfPorts);

    /* We only support a limited number of Ports */
    if ((NumberOfPorts > pDDB->PM.PortMap.PortCnt) ||
        (NumberOfPorts == 0))
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid NumberOfPorts. %d (0x%X)",NumberOfPorts,NumberOfPorts);
        if (EDD_STS_OK == Status)
        {
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_NumberOfPorts);
        }
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    /* Clear Management. will also set "Present" to LSA_FALSE */
    EDDI_MEMSET(pRecordSet_B->BeginEndData,(LSA_UINT8)0x00, sizeof(pRecordSet_B->BeginEndData));

    UsrPortIndex          = 0;
    RemainingBlockLength  = (BlockLength + EDDI_PRM_BLOCK_WITHOUT_LENGTH) - sizeof(EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE); /* remaining Recordlength      */
    pCurrentPtr           = pRecord; //(EDD_UPPER_MEM_U8_PTR_TYPE)(pBlockBeginEnd) + 16;                /* points behind NumberOfPorts */
    EDDI_PRM_ADD_ERR_OFFSET(pDDB, sizeof(EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE));

    /* Loop over all Ports */
    while ((Status == EDD_STS_OK) && (UsrPortIndex < NumberOfPorts))
    {
        if (RemainingBlockLength > sizeof(LSA_UINT32))  /* at least sizeof NumberOfAssignments */
        {
            EDDI_GET_U32_INCR(pCurrentPtr, NumberOfAssignments);

            if ((NumberOfAssignments == 0) || (NumberOfAssignments > EDDI_IRT_MAX_BEGIN_END_ASSIGNMENTS))
            {
                /* Note the EDD does only support the mandatory (=5) Assignments */
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid NumberOfAssignments.  %d (0x%X)",NumberOfAssignments,NumberOfAssignments);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT32));
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
            else
            {
                /* We have assignments for this Port */
                pRecordSet_B->BeginEndData[UsrPortIndex].bPresent = LSA_TRUE;

                /* calculate expected Blocklength */

                Length =   sizeof (LSA_UINT32)                                   /* NumberOfAssigments      */
                           + (NumberOfAssignments  * (sizeof (LSA_UINT32) * 6 )) /* Tx/RxBeginEndAssignment */
                           + sizeof (LSA_UINT32);                                /* NumberOfPhases          */

                if (RemainingBlockLength >= Length)
                {
                    LSA_UINT32  NumberOfPhases;

                    AssignmentCnt = 0;
                    RemainingBlockLength -= Length;

                    /* ------------------------------------------------------------------*/
                    /* Parse Assignments                                                 */
                    /* ------------------------------------------------------------------*/
                    pRecordSet_B->BeginEndData[UsrPortIndex].NumberOfAssignments = NumberOfAssignments;

                    while ((Status == EDD_STS_OK) && (AssignmentCnt < NumberOfAssignments))
                    {
                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.RedOrangePeriodBegin);    //temporarily_disabled_lint !e961
                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.OrangePeriodBegin);       //temporarily_disabled_lint !e961
                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.GreenPeriodBegin);        //temporarily_disabled_lint !e961

                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.RedOrangePeriodBegin);    //temporarily_disabled_lint !e961
                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.OrangePeriodBegin);       //temporarily_disabled_lint !e961
                        EDDI_GET_U32_INCR(pCurrentPtr,pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.GreenPeriodBegin);        //temporarily_disabled_lint !e961

                        #if !defined (EDDI_PRM_CFG_ALLOW_REDSHIFT)
                        if (   pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.RedOrangePeriodBegin
                            || pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.RedOrangePeriodBegin)
                        {
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: RedOrangePeriodBegin <> 0. UsrPortIndex:%d AssignmentCnt:%d", 
                                              UsrPortIndex, AssignmentCnt);
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: RedOrangePeriodBegin <> 0. Tx.RedOrangePeriodBegin:%d Rx.RedOrangePeriodBegin:%d", 
                                              pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.RedOrangePeriodBegin,
                                              pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.RedOrangePeriodBegin);
                            if (pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.RedOrangePeriodBegin)
                            {
                                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT32) * 6);
                            }
                            else
                            {
                                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT32) * 3);
                            }

                            Status = EDD_STS_ERR_PRM_BLOCK;
                            //continue parsing to reveal all errors in SDB
                        }
                        #endif

                        /*---------------------------------------------------------------------------*/
                        /* check for RED phase                                                       */
                        /*---------------------------------------------------------------------------*/
                        if (   (pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.RedOrangePeriodBegin < pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Tx.OrangePeriodBegin)
                            || (pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.RedOrangePeriodBegin < pRecordSet_B->BeginEndData[UsrPortIndex].Assignment[AssignmentCnt].Rx.OrangePeriodBegin))
                        {
                            bRedPhasePresent = LSA_TRUE;
                        }

                        AssignmentCnt++;
                    }

                    /* get NumberOf Phases */
                    EDDI_GET_U32_INCR(pCurrentPtr,NumberOfPhases);

                    if (EDD_STS_OK == Status)
                    {
                        if ((NumberOfPhases != 1 ) &&
                            (NumberOfPhases != 2 ) &&
                            (NumberOfPhases != 4 ) &&
                            (NumberOfPhases != 8 ) &&
                            (NumberOfPhases != EDDI_IRT_MAX_PHASE_ASSIGNMENTS)) /* check for valid Number of Phases */
                        {
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid NumberOfPhases.  %d (0x%X)",NumberOfPhases,NumberOfPhases);
                            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT32));
                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else
                        {
                            /* calculate expected Blocklength */

                            Length = NumberOfPhases * (sizeof (LSA_UINT16) * 2 ); /* Tx/RxPhaseAssignments   */

                            if (RemainingBlockLength >= Length)
                            {
                                RemainingBlockLength -= Length;

                                i = 0;
                                /* ------------------------------------------------------------------*/
                                /* Parse PhaseAssignments.                                           */
                                /* ------------------------------------------------------------------*/

                                pRecordSet_B->BeginEndData[UsrPortIndex].NumberOfPhases = NumberOfPhases;

                                while ((Status == EDD_STS_OK) && (i < NumberOfPhases))
                                {
                                    EDDI_GET_U16_INCR(pCurrentPtr,Help1); /* TxPhaseAssignment is BitCoded */
                                    EDDI_GET_U16_INCR(pCurrentPtr,Help2); /* RxPhaseAssignment is BitCoded */

                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Tx.RedOrangePeriodBeginAssignment  = (LSA_UINT8)( Help1 & 0x000F);
                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Tx.OrangePeriodBeginAssignment     = (LSA_UINT8)((Help1 & 0x00F0) >> 4);
                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Tx.GreenPeriodBeginAssignment      = (LSA_UINT8)((Help1 & 0x0F00) >> 8);

                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Rx.RedOrangePeriodBeginAssignment  = (LSA_UINT8)( Help2 & 0x000F);
                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Rx.OrangePeriodBeginAssignment     = (LSA_UINT8)((Help2 & 0x00F0) >> 4);
                                    pRecordSet_B->BeginEndData[UsrPortIndex].PhaseAssignment[i].Rx.GreenPeriodBeginAssignment      = (LSA_UINT8)((Help2 & 0x0F00) >> 8);

                                    if (Help1 & 0xF000) /* Bits 12-15 have to be 0 */
                                    {
                                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid PhaseAssignment 2. 0x%X",Help1);
                                        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT16) * 2);
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                    else if (Help2 & 0xF000) /* Bits 12-15 have to be 0 */
                                    {
                                        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid PhaseAssignment 2. 0x%X",Help2);
                                        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord) - sizeof(LSA_UINT16));
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }

                                    i++;
                                } /* while */
                            } /* if */
                            else
                            {
                                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid BlockLength. Expected: %d, Remaining: %d",Length,RemainingBlockLength);
//                              EDDI_PRM_SET_ERR_OFFSET(pDDB, ???);
                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        } /* else */
                    } //(EDD_STS_OK == Status)

                    /* ------------------------------------------------------------------*/
                    /* Validate parameters for this port (value and references)          */
                    /* ------------------------------------------------------------------*/

                    if (Status == EDD_STS_OK)
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Parse_PDIRBeginEnd: Validate_PDIRBeginEnd for Port: %d (0x%X)",UsrPortIndex,UsrPortIndex);
                        Status = EDDI_PDIRBeginEndValidate(pDDB, &pRecordSet_B->BeginEndData[UsrPortIndex]);
                    }
                }
                else
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid BlockLength. Expected: %d, Remaining: %d",Length,RemainingBlockLength);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }

            UsrPortIndex++;
        }
        else
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid BlockLength.  %d,%d",BlockLength,RemainingBlockLength);
            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    }
    EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_POINTER_BYTE_DISTANCE(pCurrentPtr, pRecord));

    if (( Status == EDD_STS_OK ) &&
        ( RemainingBlockLength != 0 ))
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: Invalid BlockLength. To Long by %d (0x%X)",RemainingBlockLength,RemainingBlockLength);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }
    else if (LSA_FALSE == bRedPhasePresent)  /* verify, that there is at least one red phase in the configuration */
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: No RED phase present");
        Status = EDD_STS_ERR_PRM_BLOCK;
    }
    else if (EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole)
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRBeginEndParse: NO RED phase is allowed for HSyncApplications");
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    /*---------------------------------------------------------------------------*/
    /* check for RED_ORANGE/ORANGE phases if we only have one OrangeEnd Time over*/
    /* all Ports!                                                                */
    /*---------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        PDIRBeginEnd_DetermineClassType(pDDB, pRecordSet_B);
        Status = EDDI_PDIRBeginEndValidateOrangeEnds(pDDB, pRecordSet_B);
    }

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRBeginEndParse, < Status: %d (0x%X)", Status, Status);

    return Status;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRFrameDataCheckElement( EDDI_LOCAL_DDB_PTR_TYPE                    pDDB,
                                                           const EDDI_PRM_RECORD_FRAME_DATA_TYPE  *  const  pFrmData )
{
    LSA_UINT8                       SupportedTxPortBits;
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    EDDI_SYNC_IRT_FRM_HANDLER_ENUM  FrameHandlerType;
    #endif

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRFrameDataCheckElement->");

    if (pFrmData->Ethertype != EDDI_PRM_xRT_TAG)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Ethertype_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_Ethertype);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->Ethertype > ->:0x%X :0x%X",
                          pFrmData->Ethertype, EDDI_xRT_TAG);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if ((pFrmData->DataLength > EDDI_IRT_DATALEN_MAX) || (pFrmData->DataLength < EDDI_IRT_DATALEN_MIN ))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLength_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
        EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X, allowed min :0x%X, max :0x%X",
                          pFrmData->DataLength, EDDI_IRT_DATALEN_MIN, EDDI_IRT_DATALEN_MAX);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //check lower-limit for FrameSendOffset
    if (pFrmData->FrameSendOffset < EDDI_MIN_FCW_START_TIME_NS)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameSendOffset_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameSendOffset);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->FrameSendOffset > ->:0x%X :0x%X",
                          pFrmData->FrameSendOffset, EDDI_MIN_FCW_START_TIME_NS);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pFrmData->ReductionRatio, pFrmData->Phase, LSA_TRUE /*bCheckPhase*/))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_ReductionRatio);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, ReductionRatio, Phase > ->:0x%X :0x%X",
                          pFrmData->ReductionRatio, pFrmData->Phase);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (   (pFrmData->FrameID != EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
        && ((pFrmData->FrameID > EDD_SRT_FRAMEID_IRT_STOP) || (pFrmData->FrameID < EDD_SRT_FRAMEID_IRT_START)))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameID);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->FrameID > ->:0x%X :0x%X",
                          pFrmData->FrameID, 0x0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //0 = Lokal, 1= 1.Port, 2= 2.Port, ...
    if (pFrmData->UsrRxPort_0_4 > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RxPort_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_UsrRxPort_0_4);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->RxPort > ->:0x%X :0x%X",
                          pFrmData->UsrRxPort_0_4, pDDB->PM.PortMap.PortCnt);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_RESERVED)) // not checked
    {
    }
    
    // Special Check for IrtSyncFrames with FrameId ==  0x80
    if (pFrmData->FrameID == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
    {
        if (   (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME) != SYNC_FRAME_DETAILS_PRIMARY_SYNC_FRAME)
            && (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME) != SYNC_FRAME_DETAILS_SECONDARY_SYNC_FRAME))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDetails_SyncFrame_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameDetails);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->FrameDetails.Bit.SyncFrame > ->:0x%X",
                              EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME));
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (pFrmData->DataLength != EDDI_SYNC_FRAME_PAYLOAD_SIZE)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncFrameDataLength_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X :0x%X",
                              pFrmData->DataLength, EDDI_SYNC_FRAME_PAYLOAD_SIZE);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }
    else
    {
        //no Sync-Frames
        if (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME) != SYNC_FRAME_DETAILS_NO_SYNC_FRAME)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDetails_SyncFrame_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameDetails);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->FrameDetails.Bit.SyncFrame > ->:0x%X",
                              EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME));
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (   (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET)
            != SYNC_FRAME_DETAILS_RX_TX_TIME)
        && (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET)
            != SYNC_FRAME_DETAILS_IRT_PHASE_BEGIN)
        && (EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET)
            != SYNC_FRAME_DETAILS_IRT_PHASE_END))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDetails_FrameOffset_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameDetails);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->FrameDetails.Bit.MeaningFrameSendOffset > ->:0x%X", 
                          EDDI_GetBitField8Bit(pFrmData->FrameDetails.Byte, EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET));
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pFrmData->Reserved[0]) //not checked
    {
    }

    if ((pFrmData->NumberOfTxPortGroups > 0x21) || (!(pFrmData->NumberOfTxPortGroups % 2)))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_NumberOfTxPortGroups_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_NumberOfTxPortGroups);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->NumberOfTxPortGroups > ->:0x%X", pFrmData->NumberOfTxPortGroups);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //0 = Lokal, 1= 1.Port, 2= 2.Port, ...

    SupportedTxPortBits = (LSA_UINT8)(EDDI_SYNC_TXPORTS_EXTERNAL | EDDI_SYNC_TXPORT_LOCAL);

    //Check if unsupported Bits are set in TxPortGroupArray
    if (pFrmData->UsrTxPortGroupArray[0] & (~SupportedTxPortBits))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TxPortGroupArray_PDIRFrameData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_UsrTxPortGroupArray);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->NumberOfTxPortGroups > ->:0x%X :0x%X",
                          pFrmData->UsrTxPortGroupArray[0], SupportedTxPortBits);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    #if defined (EDD_MESSAGE)
    /* --------------------------------------------------------------------------*/
    /*  IRTE-IP Bug - Only if we have a CONSUMER or CONSUMER_FWD                 */
    /*                The bug occurs if the ERTEC copys data of Class3-Frames    */
    /*                to the KRAM and this is finished at the same time a new    */
    /*                RT-Frame was received. To prevent this situation we limit  */
    /*                the frame length of Class3 Frames so we are ready with     */
    /*                copy just in time and prevent the situation.               */
    /* --------------------------------------------------------------------------*/
    #endif

    #if defined (EDDI_CFG_REV5)

    FrameHandlerType = EDDI_SyncDecodeFrameHandlerType(pDDB,pFrmData);

    if (   (FrameHandlerType == EDDI_SYNC_IRT_CONSUMER)
        || (FrameHandlerType == EDDI_SYNC_IRT_FWD_CONSUMER))
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PDIRFrameDataCheckElement, LenCheck: FrameId, DataLength:0x%X :%d",
                          pFrmData->FrameID,pFrmData->DataLength);

        #if defined (EDDI_CFG_MAX_IR_DATA_LENGTH)
        if ( ( EDDI_PRM_IRTE_IP_BUG_REV5_DEBUG_VERSION == pDDB->ERTEC_Version.Debug ) &&
             ( ( pFrmData->DataLength + sizeof(EDDI_CRT_DATA_APDU_STATUS) ) > EDDI_CFG_MAX_IR_DATA_LENGTH ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLength_REV5_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X :0x%X",
                              pFrmData->DataLength, EDDI_CFG_MAX_IR_DATA_LENGTH);
            return EDD_STS_ERR_PRM_VERSION_CONFLICT;
        }
        #elif !defined (EDDI_CFG_MAX_IR_DATA_LENGTH)
        if ( ( EDDI_PRM_IRTE_IP_BUG_REV5_DEBUG_VERSION == pDDB->ERTEC_Version.Debug ) &&
             ( ( pFrmData->DataLength + sizeof(EDDI_CRT_DATA_APDU_STATUS) ) > EDDI_IRT_DATALEN_MAX_REV5_DEBUG_0 ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLength_REV5_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X :0x%X",
                              pFrmData->DataLength, EDDI_IRT_DATALEN_MAX_REV5_DEBUG_0);
            return EDD_STS_ERR_PRM_VERSION_CONFLICT;
        }
        #endif
    }
    #endif

    #if defined (EDDI_CFG_REV6)

    FrameHandlerType = EDDI_SyncDecodeFrameHandlerType(pDDB,pFrmData);

    if (   (FrameHandlerType == EDDI_SYNC_IRT_CONSUMER)
        || (FrameHandlerType == EDDI_SYNC_IRT_FWD_CONSUMER))
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PDIRFrameDataCheckElement, LenCheck: FrameId, DataLength:0x%X :%d",
                          pFrmData->FrameID,pFrmData->DataLength);

        #if defined (EDDI_CFG_MAX_IR_DATA_LENGTH)
        if ( ( EDDI_PRM_IRTE_IP_BUG_REV6_DEBUG_VERSION >= pDDB->ERTEC_Version.Debug ) &&
             ( ( pFrmData->DataLength + sizeof(EDDI_CRT_DATA_APDU_STATUS) ) > EDDI_CFG_MAX_IR_DATA_LENGTH ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLength_REV6_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X :0x%X",
                              pFrmData->DataLength, EDDI_CFG_MAX_IR_DATA_LENGTH);
            return EDD_STS_ERR_PRM_VERSION_CONFLICT;
        }
        #elif (!defined (EDDI_CFG_MAX_IR_DATA_LENGTH))
        if ( ( EDDI_PRM_IRTE_IP_BUG_REV6_DEBUG_VERSION >= pDDB->ERTEC_Version.Debug ) &&
             ( ( pFrmData->DataLength + sizeof(EDDI_CRT_DATA_APDU_STATUS) ) > EDDI_IRT_DATALEN_MAX_REV6_DEBUG_0 ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLength_REV6_PDIRFrameData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PDIRFrameDataCheckElement, pFrmData->DataLength > ->:0x%X :0x%X",
                              pFrmData->DataLength, EDDI_IRT_DATALEN_MAX_REV6_DEBUG_0);
            return EDD_STS_ERR_PRM_VERSION_CONFLICT;
        }
        #endif
    }
    #endif

    /* --------------------------------------------------------------------------*/

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PDIRFrameDataCheckElement<-");

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
EDDI_PRM_RECORD_FRAME_DATA_TYPE  *  EDDI_LOCAL_FCT_ATTR  EDDI_CheckGetNextFrameDataElement( EDDI_PRM_RECORD_FRAME_DATA_TYPE  *  const  pFrmData )
{
    LSA_UINT32     Elementlength;
    LSA_UINT32     Padding;
    LSA_UINT32     Rest;
    LSA_UINT8   *  pNext;

    Elementlength = 17 + pFrmData->NumberOfTxPortGroups; // TODO rename  17 to define

    Rest = Elementlength % 4;

    //Calculate Padding
    if (Rest)
    {
        Padding = 4 - Rest;
    }
    else
    {
        Padding = 0;
    }

    pNext = ((LSA_UINT8 *)(void *)pFrmData) + Padding + Elementlength;

    return (EDDI_PRM_RECORD_FRAME_DATA_TYPE *)(void *)pNext;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckReductionRatioIsPowerOf2()           */
/*                                                                         */
/* D e s c r i p t i o n: Check if the ReductionRatio is a power of 2      */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_TRUE:  is a power of two                     */
/*                        LSA_FALSE: is no power of two                    */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckReductionRatioIsPowerOf2( LSA_UINT32 const ReductionRatio)
{
    LSA_UINT32 Help = ReductionRatio;
    LSA_BOOL   Status = LSA_TRUE;

    if (Help)
    {
        while (! (Help & 1)) /* till first bit is on bit position 0 */
        {
            Help >>= 1;
        }

        if ( Help != 1 ) /* only bit 0 shall be set ! */
        {
            Status = LSA_FALSE;
        }
    }
    else
    {
        Status = LSA_FALSE;
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesAddFrameToList()      */
/*                                                                         */
/* D e s c r i p t i o n: Adds the FrmData element to the Node list        */
/*                        referenced by NodeIndex if frame fits.           */
/*                                                                         */
/*                        We check if the frame is within red-phase on     */
/*                        this port.                                       */
/*                                                                         */
/*                        We check if the frame overlaps with existing     */
/*                        frames within this list.                         */
/*                                                                         */
/*                        Notes: We work with record set B and need        */
/*                               PDIRBeginEndData to be setup within       */
/*                               Groups info!                              */
/*                                                                         */
/* A r g u m e n t s:     pDDB      : pointer to DDB                       */
/*                        UsrPortIdx: UsePortIndex (0..x)                  */
/*                        NodeIndex : NodeIndex into NodeMangament         */
/*                        pFrmData  : pointer to PDIRFrame Data record     */
/*                        bIsRx     : LSA_TRUE: Rx, LSA_FALSE: Tx          */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesAddFrameToList( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                            LSA_UINT32                           const  UsrPortIdx,
                                                                            LSA_UINT32                           const  NodeIndex,
                                                                            EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData,
                                                                            LSA_BOOL                             const  bIsRx )
{
    LSA_RESULT                             Status;
    EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE  pValidate;
    EDDI_PRM_IRT_NODE_LIST_PTR_TYPE        pNodeElement;
    EDDI_PRM_IRT_ELEM_PTR_TYPE             pFrameElement;
    LSA_UINT32                             StartTimeNs,EndTimeNs,EndTimeNsWithGap;
    LSA_UINT32                             Length;
    LSA_UINT32                             MaxBridgeDelay;
    EDDI_PRM_RECORD_IRT_PTR_TYPE           pRecordSet;
    LSA_UINT32                             AssignIdx,PhaseIdx;

    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddFrameToList, > ->UsrPortIdx: %d, NodeIndex: %d",UsrPortIdx,NodeIndex);

    Status    = EDD_STS_OK;
    pValidate = pDDB->PRM.PDIRData.pValidate;

    if (bIsRx)
    {
        pNodeElement = &pValidate->NodeArray[UsrPortIdx][NodeIndex].Rx;
    }
    else
    {
        pNodeElement = &pValidate->NodeArray[UsrPortIdx][NodeIndex].Tx;
    }

    /* ---------------------------------------------------------------------------*/
    /* Calculation of Start/End-Time:                                             */
    /*                                                                            */
    /* bIsRx == TRUE (e.g.Receiption of a CONSUMER/FORWARDER/CONSUMER_FORWARDER)  */
    /*                                                                            */
    /* StartTimeNs = FrameSendOffset - MaxBridgeDelay                             */
    /* EndTimeNs   = StartTime + (8(PRE) + FrameLength + 4(FCS))* 80              */
    /* EndTimeNsWithGap  = EndTimeNs + GAP                                        */
    /*                                                                            */
    /* bIsRx == FALSE (e.g. Sending of a FORWARDER/CONSUMER_FORWARDER/PROVIDER)   */
    /*                                                                            */
    /* StartTimeNs = FrameSendOffset                                              */
    /* EndTimeNs   = StartTime + (8(PRE) + FrameLength + 4(FCS))* 80              */
    /* EndTimeNsWithGap  = EndTimeNs + GAP                                        */
    /*                                                                            */
    /* EndTimeNs and EndTimeNsWithGap only valid for 100Mbit connection!          */
    /* GAP: The minimal GAP between two RT_CLASS3 Frames is 1120ns                */
    /*                                                                            */
    /* ---------------------------------------------------------------------------*/

    if (pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended)
    {
        MaxBridgeDelay = pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord->PDIRGlobalDataExt.MaxBridgeDelay;
    }
    else
    {
        MaxBridgeDelay = pDDB->Glob.MaxBridgeDelay; /* EDD-configured Max-Bridge Delay*/
    }

    StartTimeNs = pFrmData->FrameSendOffset;

    if (bIsRx) /* we need a receive-time */
    {
        if (MaxBridgeDelay <= StartTimeNs)
        {
            StartTimeNs -= MaxBridgeDelay;
        }
        else
        {
            EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesAddFrameToList, Rx: FrameSendOffset/MaxBridgeDelay mismatch. FrameId: 0x%X, StartTimeNs: %d, MaxBrDel: %d", pFrmData->FrameID, StartTimeNs, MaxBridgeDelay);
            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    }

    /*-----------------------------------------------------------------------------*/
    /* Framelength incl. Preamble/Startdelemitter                                  */
    /*-----------------------------------------------------------------------------*/
    Length = (    7                                    // Preamble  
                + 1                                    // Startdelimitter  
                + 6                                    // DA
                + 6                                    // SA
                + 2                                    // EtherType
                + 2                                    // FrameID             
                + pFrmData->DataLength                 // Data
                + sizeof(EDDI_CRT_DATA_APDU_STATUS)    // APDU-Status
                + 4                                    // FCS 
             );

    /* according to PNIO-Norm:                                */
    /* The minimal GAP between two RT_CLASS3 Frames is 1120ns */

    EndTimeNs         = StartTimeNs + (Length * 80);      /* for 100MBit! */
    EndTimeNsWithGap  = EndTimeNs + PRM_RT_CLASS3_GAP_NS; /* for 100MBit! */

    EDDI_PRM_TRACE_10(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. FrameID: 0x%X, UsrPortId: %d, DataLen: %d, SendOffset: %d, StartNs: %d, EndTimeNs: %d, Ratio: %d, EndTimeNsGap: %d, Phase: %d, bIsRx: %d ",
                      pFrmData->FrameID,
                      UsrPortIdx+1,
                      pFrmData->DataLength,
                      pFrmData->FrameSendOffset,
                      StartTimeNs,
                      EndTimeNs,
                      pFrmData->ReductionRatio,
                      EndTimeNsWithGap,
                      pFrmData->Phase,
                      bIsRx);

    /*-----------------------------------------------------------------------------*/
    /* Now we have start/end times and we first check for border of red phase      */
    /* on this port                                                                */
    /* ----------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        LSA_UINT32  RedPeriodStartMin,RedPeriodEndMax;

        pRecordSet = pDDB->PRM.PDIRData.pRecordSet_B;

        if (( pRecordSet->bBeginEndDataPresent) &&             /* beginEnd data present */
            ( pRecordSet->BeginEndData[UsrPortIdx].bPresent))  /* PortData present      */
        {
            PhaseIdx = pFrmData->Phase-1; /* 1.. x. Not Phase must be 1..x not 0!*/

            /* if we dont have enough phase assignments we reuse peridically */
            if (PhaseIdx >= pRecordSet->BeginEndData[UsrPortIdx].NumberOfPhases)
            {
                PhaseIdx = PhaseIdx % pRecordSet->BeginEndData[UsrPortIdx].NumberOfPhases;
            }

            /* receiption? */
            if (bIsRx)
            {
                /* Do we have a red phase ?*/
                if ( pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Rx.Type == EDDI_IRT_PHASE_TYPE_RED )
                {
                    /* check Rx RedOrangePeriodBegin time      */
                    /* Get the Time from the assignment  */
                    AssignIdx         = pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Rx.RedOrangePeriodBeginAssignment;
                    RedPeriodStartMin = pRecordSet->BeginEndData[UsrPortIdx].Assignment[AssignIdx].Rx.RedOrangePeriodBegin;

                    if (StartTimeNs < RedPeriodStartMin )
                    {
                        EDDI_PRM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. RxStartTime below red period. FrameID  : 0x%X, PortId: %d, EndTimeNs: %d, RedPeriodEndNs: %d, Phase: %d",pFrmData->FrameID, UsrPortIdx+1, StartTimeNs, RedPeriodStartMin, pFrmData->Phase);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }

                    /* check Rx RedPeriodEnd Max time    */
                    /* Get the Time from the assignment. Because we are RED or RED_ORANGE the end is OrangePeriodBegin */
                    AssignIdx       = pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Rx.OrangePeriodBeginAssignment;
                    RedPeriodEndMax = pRecordSet->BeginEndData[UsrPortIdx].Assignment[AssignIdx].Rx.OrangePeriodBegin;

                    /* Note: The RedPeriodEnd Time is set to the FrameSendOffset of the last frame   */
                    /*       and does not include Bridgedelay and the length of the frame!           */
                    /*       So we dont have to check against the EndTimeNs!                         */
                    if (pFrmData->FrameSendOffset > RedPeriodEndMax )
                    {
                        EDDI_PRM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. RxEndTime above red period. FrameID: 0x%X, PortId: %d, FSO: %d, RedPeriodEndNs: %d, Phase    : %d",pFrmData->FrameID,UsrPortIdx+1, pFrmData->FrameSendOffset,RedPeriodEndMax, pFrmData->Phase);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }
                else
                {
                    /* invalid Type. no RED Phase present ! */
                    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. No RED Rx-Phase present for Port/Phase. FrameID  : 0x%X, PortId: %d, Phase: %d,",pFrmData->FrameID,UsrPortIdx+1, pFrmData->Phase);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else /* tx */
            {
                /* Do we have a red phase ?*/
                if ( pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Tx.Type == EDDI_IRT_PHASE_TYPE_RED )
                {
                    /* check Tx RedOrangePeriodBegin Min time */
                    /* Get the Time from the assignment  */
                    AssignIdx         = pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Tx.RedOrangePeriodBeginAssignment;
                    RedPeriodStartMin = pRecordSet->BeginEndData[UsrPortIdx].Assignment[AssignIdx].Tx.RedOrangePeriodBegin;

                    if (StartTimeNs < RedPeriodStartMin)
                    {
                        EDDI_PRM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. TxStartTime below red period. FrameID  : 0x%X, PortId: %d, EndTimeNs: %d, RedPeriodEndNs: %d, Phase: %d",pFrmData->FrameID,UsrPortIdx+1, StartTimeNs,RedPeriodStartMin, pFrmData->Phase);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }

                    /* check Tx RedPeriodEnd Max time */
                    /* Get the Time from the assignment. Because we are RED or RED_ORANGE the end is OrangePeriodBegin */
                    AssignIdx         = pRecordSet->BeginEndData[UsrPortIdx].PhaseAssignment[PhaseIdx].Tx.OrangePeriodBeginAssignment;
                    RedPeriodEndMax   = pRecordSet->BeginEndData[UsrPortIdx].Assignment[AssignIdx].Tx.OrangePeriodBegin;

                    /* Note: The RedPeriodEnd Time is set to the FrameSendOffset of the last frame   */
                    /*       and does not include the length of the frame!                           */
                    /*       So we dont have to check against the EndTimeNs!                         */

                    if (pFrmData->FrameSendOffset > RedPeriodEndMax)
                    {
                        EDDI_PRM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_PrmPDIRCheckFrameTimesAddFrameToList. TxEndTime above red period. FrameID: 0x%X, UsrPortIdx: %d, pFrmData->FrameSendOffset: %d, RedPeriodEndMax: %d, pFrmData->Phase: %d",
                                          pFrmData->FrameID, 
                                          UsrPortIdx+1, 
                                          pFrmData->FrameSendOffset,
                                          RedPeriodEndMax, 
                                          pFrmData->Phase);
                                          
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }
                else
                {
                    /* invalid Type. no RED Phase present ! */
                    EDDI_PRM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. No RED Tx-Phase present for Port/Phase. FrameID  : 0x%X, PortId: %d, Phase: %d",pFrmData->FrameID,UsrPortIdx+1, pFrmData->Phase);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
        }
        else
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. No BeginEnd or PortData present. Can not check RED phase borders! PortId: %d",UsrPortIdx-1);
        }
    }

    /* ----------------------------------------------------------------------------*/
    /* Now we can search the phase list for a valid gap and queue this element into*/
    /* the phase list                                                              */
    /* ----------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        if (pValidate->ElemFreeCnt) /* still free elements within management? */
        {
            pFrameElement = &pValidate->ElemArray[pValidate->ElemUsedCnt];
            pValidate->ElemUsedCnt++;
            pValidate->ElemFreeCnt--;

            /* setup Times for Element */
            pFrameElement->StartTimeNs      = StartTimeNs;
            pFrameElement->EndTimeNsWithGap = EndTimeNsWithGap;
            pFrameElement->pFrmData         = pFrmData;

            if (pNodeElement->Queue.Count) /* already elements queued.. */
            {
                EDDI_PRM_IRT_ELEM_PTR_TYPE pPrev,pCur;
                LSA_BOOL                  Found;

                pPrev = LSA_NULL;
                pCur  = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pFirst;
                Found = LSA_FALSE;

                while (( Status == EDD_STS_OK ) && ( ! Found ))
                {
                    /* only if the frame begins before current one, or we have no more current*/
                    if (LSA_HOST_PTR_ARE_EQUAL(pCur,LSA_NULL) || (StartTimeNs < pCur->StartTimeNs))
                    {
                        /* first check if we fit into this gap  */
                        /* e.g. starts after end of prev, and ends before start of current */
                        if ( (LSA_HOST_PTR_ARE_EQUAL(pPrev,LSA_NULL) || ( StartTimeNs      >= pPrev->EndTimeNsWithGap )) &&
                             (LSA_HOST_PTR_ARE_EQUAL(pCur,LSA_NULL)  || ( EndTimeNsWithGap <= pCur->StartTimeNs       )))
                        {
                            Found = LSA_TRUE;

                            // queue element here */
                            pFrameElement->QueueLink.prev_ptr = (EDDI_QUEUE_ELEMENT_PTR_TYPE)(void *)pPrev;
                            pFrameElement->QueueLink.next_ptr = (EDDI_QUEUE_ELEMENT_PTR_TYPE)(void *)pCur;

                            if (!LSA_HOST_PTR_ARE_EQUAL(pPrev,LSA_NULL))
                            {
                                pPrev->QueueLink.next_ptr  = &pFrameElement->QueueLink;
                                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. Gap Found. Put behind FrameId: 0x%X (NodeIndex: %d)",pPrev->pFrmData->FrameID,NodeIndex);
                            }
                            else
                            {
                                /* new element put at begin */
                                pNodeElement->Queue.pFirst = &pFrameElement->QueueLink;
                                pNodeElement->StartTimeNs  = StartTimeNs;
                                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. Gap Found. First element. (NodeIndex: %d)",NodeIndex);
                            }

                            if (!LSA_HOST_PTR_ARE_EQUAL(pCur,LSA_NULL))
                            {
                                pCur->QueueLink.prev_ptr  = &pFrameElement->QueueLink;
                            }
                            else
                            {
                                /* new element put at end */
                                pNodeElement->Queue.pLast        = &pFrameElement->QueueLink;
                                pNodeElement->EndTimeNsWithGap   = EndTimeNsWithGap;
                            }

                            pNodeElement->Queue.Count++;

                        }
                        else
                        {
                            /* Note: At least one of the following two overlaps must be present! */

                            /* check for Overlapping at begin (prev with new one) */
                            if ((!LSA_HOST_PTR_ARE_EQUAL(pPrev,LSA_NULL)) && (StartTimeNs < pPrev->EndTimeNsWithGap))
                            {
                                EDDI_PRM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. Overlapping occured at begin. FrameID1: 0x%X, FrameID2: 0x%X, Offset1: %d, Offset2: %d, PortID: %d, StartimeNs: %d, Phase: %d",pFrmData->FrameID,pPrev->pFrmData->FrameID, pFrmData->FrameSendOffset,pPrev->pFrmData->FrameSendOffset, UsrPortIdx+1,StartTimeNs, pFrmData->Phase);
                               Status = EDD_STS_ERR_PRM_BLOCK;
                            }

                            /* check for Overlapping at end (new one with current)*/
                            if ((!LSA_HOST_PTR_ARE_EQUAL(pCur,LSA_NULL)) && (EndTimeNsWithGap > pCur->StartTimeNs))
                            {
                                EDDI_PRM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. Overlapping occured at end. FrameID1: 0x%X, FrameID2: 0x%X, Offset1: %d, Offset2: %d, PortID: %d, EndTimeNsGap: %d, Phase: %d",pFrmData->FrameID,pCur->pFrmData->FrameID, pFrmData->FrameSendOffset,pCur->pFrmData->FrameSendOffset, UsrPortIdx+1,EndTimeNsWithGap, pFrmData->Phase);
                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                    }

                    /* if no error and not found advance to next frame */
                    if ((Status == EDD_STS_OK) && (!Found))
                    {
                        /* we can not advance behind the end.. should not occur!*/
                        if (LSA_HOST_PTR_ARE_EQUAL(pCur,LSA_NULL))
                        {
                            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesAddFrameToList, List inconsistency.");
                            EDDI_Excp("EDDI_PrmPDIRCheckFrameTimesAddFrameToList, List inconsistency.", EDDI_FATAL_ERR_EXCP,0,0);
                            return(EDD_STS_ERR_PRM_BLOCK);
                        }
                        else
                        {
                            /* advance to next */
                            pPrev = pCur;
                            pCur  = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pCur->QueueLink.next_ptr;
                        }
                    }
                } /* while */
            }
            else
            {
                /* this is the first element. Just queue it */
                pNodeElement->StartTimeNs      = StartTimeNs;
                pNodeElement->EndTimeNsWithGap = EndTimeNsWithGap;
                EDDI_QueueAddToEnd(&pNodeElement->Queue, &pFrameElement->QueueLink);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_PrmPDIRCheckFrameTimesAddFrameToList. First element. FrameId: 0x%X, NodeIndex: %d,",pFrmData->FrameID,NodeIndex);
            }
        }
        else
        {
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesAddFrameToList, No more free elements. Cannot check.");
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddFrameToList<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesAddElement()          */
/*                                                                         */
/* D e s c r i p t i o n: Adds the FrmData Element from IRData record      */
/*                        to the phase list within node management used    */
/*                        for validation of start/end - time               */
/*                                                                         */
/*                        We check if the frame is within red-phase on     */
/*                        this port.                                       */
/*                                                                         */
/*                        We check if the frame overlaps with existing     */
/*                        frames within its reduction and phase            */
/*                                                                         */
/*                        Notes: We work with record set B and need        */
/*                               PDIRBeginEndData to be setup within       */
/*                               Groups info!                              */
/*                                                                         */
/* A r g u m e n t s:     pDDB      : pointer to DDB                       */
/*                        pFrmData  : pointer to PDIRFrame Data record     */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesAddElement( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData )
{
    LSA_RESULT  Status;
    LSA_UINT32  NodeIndex;
    LSA_UINT32  UsrPortIdx;
    LSA_UINT8   TxPortsExt;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddElement->");

    Status = EDD_STS_OK;

    /* ------------------------------------------------------------------------*/
    /* we only support a limit max-reduction ratio for test because of limited */
    /* management size! and only power of 2. (e.g. 1,2,4,8..)                  */
    /* ------------------------------------------------------------------------*/

    if ( EDDI_PrmCheckReductionRatioIsPowerOf2(pFrmData->ReductionRatio) &&
         (pFrmData->ReductionRatio <= EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX ))
    {
        /* calculate branch of Node Management: */
        NodeIndex = (pFrmData->ReductionRatio-1) + (pFrmData->Phase-1);

        if (NodeIndex >= EDDI_PRM_RECORD_IRT_VAL_NODE_CNT)
        {
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_PrmPDIRCheckFrameTimesAddElement: InvalidIndex.:0x%X :0x%X",NodeIndex,NodeIndex);
            EDDI_Excp("EDDI_PrmPDIRCheckFrameTimesAddElement: InvalidIndex.",EDDI_FATAL_ERR_EXCP,0,0);
            return EDD_STS_ERR_EXCP;
        }

        /* -----------------------------------------------------------------------*/
        /* First check the UsrRxPort for Rx                                       */
        /* usrPortIdx = 0 is local Port and has not to be checked. Only external  */
        /* Ports will be checked!                                                 */
        /* -----------------------------------------------------------------------*/

        UsrPortIdx = pFrmData->UsrRxPort_0_4;

        if ((UsrPortIdx != 0) && (UsrPortIdx <= pDDB->PM.PortMap.PortCnt))
        {
            UsrPortIdx--;
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddElement, Rx check: FrameId: 0x%X, PortID: 0x%X", pFrmData->FrameID, UsrPortIdx+1);

            Status = EDDI_PrmPDIRCheckFrameTimesAddFrameToList(pDDB, UsrPortIdx, NodeIndex, pFrmData, LSA_TRUE);

            if (Status != EDD_STS_OK)
            {
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesAddElement, Rx overlapping.: FrameId: 0x%X, PortID: 0x%X", pFrmData->FrameID, UsrPortIdx+1);
            }
        }

        /* -----------------------------------------------------------------------*/
        /* Now we check for all Tx-Ports used by element.                         */
        /* There maybe more then one Tx-Port!                                     */
        /* -----------------------------------------------------------------------*/

        UsrPortIdx = 0;

        /* Bit0 = local, Bit1 = UsrPort 1 etc */
        TxPortsExt = (LSA_UINT8)(pFrmData->UsrTxPortGroupArray[0] & EDDI_SYNC_TXPORTS_EXTERNAL);

        while ((Status == EDD_STS_OK) && (UsrPortIdx < pDDB->PM.PortMap.PortCnt))
        {
            TxPortsExt >>=1;

            if (TxPortsExt & 1) /* is this UsrPort used ? */
            {
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddElement, Tx check: FrameId: 0x%X, PortID: 0x%X", pFrmData->FrameID, UsrPortIdx+1);

                Status = EDDI_PrmPDIRCheckFrameTimesAddFrameToList(pDDB, UsrPortIdx, NodeIndex, pFrmData, LSA_FALSE);

                if (Status != EDD_STS_OK )
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesAddElement, Tx overlapping.: FrameId: 0x%X, PortID: 0x%X", pFrmData->FrameID, UsrPortIdx+1);
                }
            }

            UsrPortIdx++;
        }
    }
    else
    {
        /* we dont support Ratio > EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX */
        /* ignore this frame (cannot be checked!)                          */
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_PrmPDIRCheckFrameTimesAddElement, Unsuported ReductionRatio: 0x%X :0x%X", pFrmData->ReductionRatio, pFrmData->ReductionRatio);
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesAddElement<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesCheckBranches()       */
/*                                                                         */
/* D e s c r i p t i o n: Called after all elements added to the node      */
/*                        management. We check here if all phaselists of   */
/*                        each reduction wont overlap.                     */
/*                                                                         */
/*                        For this we check all possible combinations of   */
/*                        phase list. This are maxreduction ratio          */
/*                        combinations.                                    */
/*                                                                         */
/*                        e.g. with a max. reduction ratio of 4 we have    */
/*                        the following combinations:                      */
/*                                                                         */
/*                        x.y : x = reduction ratio, y = phase             */
/*                                                                         */
/*                        1: 1.1 2.1 4.1                                   */
/*                        2: 1.1 2.2 4.2                                   */
/*                        3: 1.1 2.1 4.3                                   */
/*                        4: 1.1 2.2 4.4                                   */
/*                                                                         */
/*                        Each phase has its own begin/end times.          */
/*                        We check the end/begin times between 1.y and 2.y */
/*                        and 2.y and 4.y for the 4 combinations.          */
/*                                                                         */
/*                        The check is done for every Port!                */
/*                                                                         */
/* A r g u m e n t s:     pDDB      : pointer to DDB                       */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesCheckBranches( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                             Status;
    EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE  pValidate;
    LSA_UINT32                             NodeIndex;
    LSA_UINT32                             UsrPortIdx;
    LSA_UINT32                             ReductionRatio;
    LSA_UINT32                             MaxPhase;
    LSA_UINT32                             TxTimeEnd,RxTimeEnd;
    LSA_UINT32                             TxLastFrameID, RxLastFrameID;
    EDDI_PRM_IRT_NODE_LIST_PTR_TYPE        pNodeElement;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches->");

    Status      = EDD_STS_OK;
    pValidate   = pDDB->PRM.PDIRData.pValidate;

    UsrPortIdx     = 0;

    /* --------------------------------------------------------------------------*/
    /* Loop over all ports                                                       */
    /* --------------------------------------------------------------------------*/
    while ((Status == EDD_STS_OK) && (UsrPortIdx < pDDB->PM.PortMap.PortCnt))
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: PortID: %d",UsrPortIdx+1);

        MaxPhase = 1; /* goes 1,2,4..EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX */

        #ifdef EDDI_CFG_PDIRDATA_TRACE_TREE
        /* ------------------------------------------------------------------------*/
        /* for debugging. trace each branch of the tree                            */
        /* ------------------------------------------------------------------------*/
        {
            LSA_UINT32                 i;
            EDDI_PRM_IRT_ELEM_PTR_TYPE  pFrameElement;
            LSA_UINT32                 Ratio;
            LSA_UINT32                 Phase;

            Ratio = 1;
            for (i=0; i<EDDI_PRM_RECORD_IRT_VAL_NODE_CNT; i++)
            {
                if ( i == (Ratio*2)-1)
                {
                    Ratio *= 2;
                }

                Phase = i+2-Ratio;

                pNodeElement  = &pValidate->NodeArray[UsrPortIdx][i].Tx;
                pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pFirst;

                if (pNodeElement->Queue.Count)
                {
                    EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Tx-Branchlist for Ratio: %d, Phase: %d, Start: %d, End: %d",Ratio,Phase,pNodeElement->StartTimeNs,pNodeElement->EndTimeNsWithGap);
                    while (! LSA_HOST_PTR_ARE_EQUAL(pFrameElement,LSA_NULL))
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: - FrameID: 0x%X (Start: %d)",pFrameElement->pFrmData->FrameID,pFrameElement->StartTimeNs);
                        pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pFrameElement->QueueLink.next_ptr;
                    }
                }
                else
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Tx-Branchlist for Ratio: %d, Phase: %d is empty",Ratio,Phase);
                }
            }

            Ratio = 1;
            for (i=0; i<EDDI_PRM_RECORD_IRT_VAL_NODE_CNT; i++)
            {
                if (i == (Ratio*2)-1)
                {
                    Ratio *= 2;
                }

                Phase = i+2-Ratio;

                pNodeElement  = &pValidate->NodeArray[UsrPortIdx][i].Rx;
                pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pFirst;

                if (pNodeElement->Queue.Count)
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Rx-Branchlist for Ratio: %d, Phase: %d",Ratio,Phase);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Start: %d, End: %d",pNodeElement->StartTimeNs,pNodeElement->EndTimeNsWithGap);

                    while (!LSA_HOST_PTR_ARE_EQUAL(pFrameElement, LSA_NULL))
                    {
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: - FrameID: 0x%X (Start: %d)",pFrameElement->pFrmData->FrameID,pFrameElement->StartTimeNs);
                        pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pFrameElement->QueueLink.next_ptr;
                    }
                }
                else
                {
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Rx-Branchlist for Ratio: %d, Phase: %d is empty",Ratio,Phase);
                }
            }
        }
        #endif //EDDI_CFG_PDIRDATA_TRACE_TREE

        /* ------------------------------------------------------------------------*/
        /* Loop over all possible combinations (=max Ratio)                        */
        /* e.g.                                                                    */
        /*                         1: 1.1 2.1 4.1 .. 16.1                          */
        /*                         2: 1.1 2.2 4.2 ..                               */
        /*                         3: 1.1 2.1 4.3 ..                               */
        /*                         4: 1.1 2.2 4.4 ..                               */
        /*                         :                                               */
        /*                        16  1.1 2.2 4.4 ...16.16                         */
        /*                                                                         */
        /* Note: Within a ratio there maybe no frames! also ratio 1 may not contain*/
        /*       frames!                                                           */
        /* ------------------------------------------------------------------------*/
        while ((Status == EDD_STS_OK) && (MaxPhase <= EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX))
        {

            RxTimeEnd   = 0; /* as long as no first element is prsent */
            TxTimeEnd   = 0; /* as long as no first element is prsent */

            TxLastFrameID = 0;
            RxLastFrameID = 0;

            ReductionRatio = 1; /* goes 1,2,4..EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX */

            /* ----------------------------------------------------------------------*/
            /* combine all branches (ratios) (= number of ratios)                    */
            /* ----------------------------------------------------------------------*/
            while ((Status == EDD_STS_OK) && (ReductionRatio <= EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX))
            {
                /* calculate branch(=index) of Node Management:    */
                /* we get the branches like  1.1, 2.2, 4.2, 8.6 .. */
                /* we start for 1.x and go upwards                 */

                /* The maximum per ReductionRatio is ReductionRatio! so wie have to % */
                NodeIndex = (ReductionRatio-1) + ((MaxPhase-1) % ReductionRatio);

                //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: ReductionRatio: %d, NodeIndex: %d",ReductionRatio,NodeIndex);

                /* ----------------------------------------------------------------------*/
                /* Check Tx-Borders. EndTime of last branch with start time of this one  */
                /* ----------------------------------------------------------------------*/
                pNodeElement = &pValidate->NodeArray[UsrPortIdx][NodeIndex].Tx;

                if (pNodeElement->Queue.Count) /* at least a frame present within phase? */
                {
                    EDDI_PRM_IRT_ELEM_PTR_TYPE  pFrameElement;

                    /* Check if the Starttime of this Branch is not below the previous      */
                    /* EndTime                                                              */

                    if (TxTimeEnd != 0) /* not the first element? */
                    {
                        pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pFirst;

                        if (pNodeElement->StartTimeNs < TxTimeEnd )
                        {
                            EDDI_PRM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Tx end/begin overlapping: from FrameID 0x%X to FrameID 0x%X, PortId: %d, MaxPhase: %d, NodeIndex : %d, Ratio: %d, EndTimLast: %d, BeginTime: %d"
                                              ,TxLastFrameID,pFrameElement->pFrmData->FrameID,UsrPortIdx+1,MaxPhase,NodeIndex, ReductionRatio, TxTimeEnd, pNodeElement->StartTimeNs);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else
                        {
                            #if defined (EDDI_CFG_PDIRDATA_TRACE_TREE)
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Tx end/begin check OK from FrameID 0x%X to FrameID: 0x%X",TxLastFrameID,pFrameElement->pFrmData->FrameID);
                            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Tx end/begin-check OK.Ratio: %d, MaxPhase: %d",ReductionRatio,MaxPhase);
                            #endif
                        }
                    }

                    TxTimeEnd = pNodeElement->EndTimeNsWithGap; /* this is the new end for next check */

                    pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pLast;
                    TxLastFrameID = pFrameElement->pFrmData->FrameID; /* save FrameId for Trace */
                }

                /* ----------------------------------------------------------------------*/
                /* Check Rx-Borders. EndTime of last branch with start time of this one  */
                /* ----------------------------------------------------------------------*/
                pNodeElement = &pValidate->NodeArray[UsrPortIdx][NodeIndex].Rx;

                if (pNodeElement->Queue.Count) /* at least a frame present within phase? */
                {
                    EDDI_PRM_IRT_ELEM_PTR_TYPE  pFrameElement;

                    /* Check if the Starttime of this Branch is not below the previous      */
                    /* EndTime                                                              */

                    if (RxTimeEnd != 0)  /* not the first element? */
                    {
                        pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pFirst;

                        if (pNodeElement->StartTimeNs < RxTimeEnd)
                        {
                            EDDI_PRM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Rx end/begin overlapping: from FrameID 0x%X to FrameID 0x%X,  PortId     : %d, MaxPhase: %d, NodeIndex  : %d, Ratio: %d, EndTimeLast: %d, BeginTime: %d",
                                              RxLastFrameID,pFrameElement->pFrmData->FrameID, UsrPortIdx+1,MaxPhase, NodeIndex, ReductionRatio, RxTimeEnd, pNodeElement->StartTimeNs);
                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        else
                        {
                            #if defined (EDDI_CFG_PDIRDATA_TRACE_TREE)
                            EDDI_PRM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches: Rx end/begin check OK from FrameID 0x%X to FrameID: 0x%X, Ratio: %d, MaxPhase: %d",RxLastFrameID,pFrameElement->pFrmData->FrameID, ReductionRatio,MaxPhase);
                            #endif
                        }
                    }

                    RxTimeEnd = pNodeElement->EndTimeNsWithGap; /* this is the new endime for next check */

                    pFrameElement = (EDDI_PRM_IRT_ELEM_PTR_TYPE)(void *)pNodeElement->Queue.pLast;
                    RxLastFrameID = pFrameElement->pFrmData->FrameID; /* save FrameId for Trace */
                }

                ReductionRatio *= 2; /* Ratio goes 1, 2, 4, 8.. */
            }

            MaxPhase++;
        }

        UsrPortIdx++;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesCheckBranches<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesSetup()               */
/*                                                                         */
/* D e s c r i p t i o n: Setup management for FrameTime check.            */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesSetup( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE  pValidate;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesSetup->");

    pValidate = pDDB->PRM.PDIRData.pValidate;

    if (LSA_HOST_PTR_ARE_EQUAL(pValidate, LSA_NULL))
    {
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_PrmPDIRCheckFrameTimesSetup, Null-Ptr.");
        EDDI_Excp("EDDI_PrmPDIRCheckFrameTimesSetup, Null-Ptr!",EDDI_FATAL_ERR_NULL_PTR,0,0);
        return;
    }

    /* Clear Management. We only clear NodeArray to speed it up */
    EDDI_MEMSET(pValidate->NodeArray, (LSA_UINT8)0x00, sizeof(pValidate->NodeArray));
    pValidate->ElemFreeCnt = EDDI_IRT_MAX_TREE_ELEMENTS;
    pValidate->ElemUsedCnt = 0;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesSetup<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesInit()                */
/*                                                                         */
/* D e s c r i p t i o n: Initialize management for FrameTime check.       */
/*                        Allocs memory                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesInit->");

    EDDI_AllocLocalMem((void * *)&pDDB->PRM.PDIRData.pValidate,sizeof(*pDDB->PRM.PDIRData.pValidate));

    if (LSA_HOST_PTR_ARE_EQUAL(pDDB->PRM.PDIRData.pValidate, LSA_NULL))
    {
        EDDI_Excp("EDDI_PrmPDIRCheckFrameTimesInit, Alloc local memory failed!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_MemSet(pDDB->PRM.PDIRData.pValidate, (LSA_UINT8)0,sizeof(*pDDB->PRM.PDIRData.pValidate));

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesInit<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmPDIRCheckFrameTimesExit()                */
/*                                                                         */
/* D e s c r i p t i o n: Frees management for FrameTime check.            */
/*                                                                         */
/* A r g u m e n t s:     pDDB      : pointer to DDB                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesExit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16  ret_val;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesExit->");

    if (!LSA_HOST_PTR_ARE_EQUAL(pDDB->PRM.PDIRData.pValidate, LSA_NULL))
    {
        EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->PRM.PDIRData.pValidate);

        if (EDD_STS_OK != ret_val)
        {
            EDDI_Excp("EDDI_PrmPDIRCheckFrameTimesExit, EDDI_FREE_LOCAL_MEM failed!", EDDI_FATAL_ERR_EXCP, 0, ret_val);
            return;
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmPDIRCheckFrameTimesExit<-");
}
/*---------------------- end [subroutine] ---------------------------------*/





/***************************************************************************/
/* F u n c t i o n: EDDI_CheckFrameDataProperties                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckFrameDataProperties( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  *  const  pFrameData = &pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData;
    EDDI_IRT_FRM_HANDLER_PTR_TYPE                   pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pDDB->PRM.PDIRData.pRecordSet_B->FrameHandlerQueue.pFirst;
    LSA_BOOL                                        bRedundantFrameID = LSA_FALSE;
    LSA_UINT32                               const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                               const  PortEnableCount = EDDI_PrmGetActivePortCount(pDDB);
                       
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CheckFrameDataProperties ->");
    
    LSA_UNUSED_ARG(PortMapCnt);

    while (EDDI_NULL_PTR != pFrmHandler)
    {
        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pFrmHandler->pIrFrameDataElem[0];
             
        bRedundantFrameID = LSA_FALSE;
               
        if (   (pFrmData->FrameID >= EDD_SRT_FRAMEID_IRT_REDUNDANT_START) 
            && (pFrmData->FrameID <= EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP))
        {
            bRedundantFrameID = LSA_TRUE;    
        }
 
        if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pFrameData->BlockHeader.BlockVersionLow ) //BlockVersionLow = 1
        { 
            LSA_UINT32  const  FrameDataMulticastAdd   = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD);
                    
            if (   (bRedundantFrameID)
                && (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_LEGACY == FrameDataMulticastAdd )
                && (PortEnableCount > 0)  ) 
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDD_PRM_ERR_INDEX_DEFAULT,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CheckFrameDataProperties, Error: FrameDataProperties LEGACY is allowed for Redundant Frames with Portcount > 0, pFrmData->FrameID->:0x%X, PortMapCnt->:0x%X", pFrmData->FrameID, PortMapCnt);
                return EDD_STS_ERR_PRM_BLOCK;
            }  
            
            //Check for Redundant Frames
            if (   (bRedundantFrameID)
                && (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataMulticastAdd )
                && (PortEnableCount != 1)  ) 
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDD_PRM_ERR_INDEX_DEFAULT,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CheckFrameDataProperties, Error: Only RT_CLASS_3 is allowed for Redundant Frames, pFrmData->FrameID->:0x%X, PortMapCnt->:0x%X", pFrmData->FrameID, PortMapCnt);
                return EDD_STS_ERR_PRM_BLOCK;
            } 
            
            //Check for NonRedundant Frames
            if (   (!bRedundantFrameID)
                && (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataMulticastAdd )
                && (PortEnableCount != 1)  ) 
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDIRFrameData);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDD_PRM_ERR_INDEX_DEFAULT,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CheckFrameDataProperties, Error: FrameDataProperties: FastForwarding is allowed for NonRedundant Frames with Portcount > 0, pFrmData->FrameID->:0x%X, PortMapCnt->:0x%X", pFrmData->FrameID, PortMapCnt);
                return EDD_STS_ERR_PRM_BLOCK;   
            }      
        }
                
        pFrmHandler = (EDDI_IRT_FRM_HANDLER_TYPE *)(void *)pFrmHandler->QueueLink.next_ptr;
    }
    
    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CheckFrameDataProperties <-");
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdir_data.c                                  */
/*****************************************************************************/

