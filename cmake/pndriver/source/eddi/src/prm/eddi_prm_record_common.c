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
/*  F i l e               &F: eddi_prm_record_common.c                  :F&  */
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
/*  Date        Who   What                                                   */
/*  27.05.09    AH    initial version                                        */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"

#include "eddi_prm_record_common.h"
#include "eddi_prm_req.h"

#include "eddi_prm_record_pdcontrol_pll.h"
#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_pdnc_data_check.h"
#include "eddi_prm_record_pdport_data_adjust.h"
#include "eddi_prm_record_pdnrt_load_limitation.h"
#include "eddi_prm_record_pdscf_data_check.h"
#include "eddi_prm_record_pdsync_data.h"
#include "eddi_prm_record_pdset_default_port_states.h"
#include "eddi_prm_record_pdtraceunit_control.h"
#include "eddi_prm_record_pdir_subframe_data.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_COMMON
#define LTRC_ACT_MODUL_ID  403

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckAndCopyRecord( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                    Result;
    EDD_UPPER_PRM_WRITE_PTR_TYPE  pPrmWrite;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "PrmCheckRecord->");

    pPrmWrite = (EDD_UPPER_PRM_WRITE_PTR_TYPE)pRQB->pParam;

    if (pPrmWrite->edd_port_id > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPrmWrite->edd_port_id > ->:0x%X", pPrmWrite->edd_port_id);
        return EDD_STS_ERR_PARAM;
    }

    if (   (EDD_PRM_PARAMS_ASSIGNED_REMOTE != pPrmWrite->Local)
        && (EDD_PRM_PARAMS_ASSIGNED_LOCALLY != pPrmWrite->Local) )
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_Mode);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, Local (%d): invalid value", pPrmWrite->Local);
        return EDD_STS_ERR_PARAM;
    }

    switch (pPrmWrite->record_index)
    {
        #if defined (EDDI_CFG_DFP_ON)
        case EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA:  //supports PDSetDefaultPortStates record 0x10001
        {
            Result = EDDI_PrmCheckAndCopyPdirSubframeData(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckePdirSubframeData, Result");
                return Result;
            }
            pDDB->PRM.PDIRSubFrameData.State_B = EDDI_PRM_WRITE_DONE;
            break;
        }
        #endif
         
        case EDDI_PRM_INDEX_PDNC_DATA_CHECK: //supports network component function mismatch record 0x8070
        {
            Result = EDDI_PrmCheckPDNCData(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNCData, Result > ->:0x%X", Result);
                return Result;
            }

            pDDB->PRM.PDNCDataCheck.SlotNumber    = pPrmWrite->slot_number;
            pDDB->PRM.PDNCDataCheck.SubSlotNumber = pPrmWrite->subslot_number;

            break;
        }

        case EDDI_PRM_INDEX_PDCONTROL_PLL:  //supports PLL_Ext record 0xB050
        {
            Result = EDDI_PrmCheckPDControlPLL(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, Result > ->:0x%X", Result);
                return Result;
            }
            break;
        }

        case EDDI_PRM_INDEX_PDTRACEUNIT_CONTROL: //supports PDTraceUnitControl record 0xB060
        {
            /* --------------------------------------------------------------------------*/
            /* PDTraceUnit Record                                                        */
            /* --------------------------------------------------------------------------*/
            Result = EDDI_PrmCheckPDTraceUnitControl(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, Result > ->:0x%X", Result);
                return Result;
            }
            break;
        }

        case EDDI_PRM_INDEX_PDSCF_DATA_CHECK: //supports send clock factor record 0x10000
        {
            Result = EDDI_PrmCheckPDSCFData(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, Result > ->:0x%X", Result);
                return Result;
            }

            pDDB->PRM.PDSCFDataCheck.SlotNumber     = pPrmWrite->slot_number;
            pDDB->PRM.PDSCFDataCheck.SubSlotNumber  = pPrmWrite->subslot_number;

            break;
        }

        case EDDI_PRM_INDEX_PDIR_DATA:  //supports PDIRData record 0x802C
        {
            pDDB->PRM.PDIRData.pRecordSet_B->bGlobalDataExtended  = LSA_FALSE;
            pDDB->PRM.PDIRData.pRecordSet_B->bBeginEndDataPresent = LSA_FALSE;
            pDDB->PRM.PDIRData.pRecordSet_B->PortCnt_Extended = 0;

            Result = EDDI_PrmCheckPDIRData(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDIRData, Result > ->:0x%X", Result);
                return Result;
            }

            pDDB->PRM.PDIRData.pRecordSet_B->PDIRDataRecordActLen = pPrmWrite->record_data_length;
            pDDB->PRM.PDIRData.State_B                            = EDDI_PRM_WRITE_DONE;

            break;
        }

        case EDDI_PRM_INDEX_PDSYNC_DATA: //supports PDSyncData (PTCPoverRTC) record 0x8020
        {
            pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount = 0;

            Result = EDDI_PrmCheckPDSyncData(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData, Result");
                return Result;
            }

            pDDB->PRM.PDSyncData.State_B = EDDI_PRM_WRITE_DONE;

            break;
        }

        case EDDI_PRM_INDEX_PDPORT_DATA_ADJUST: //supports PDPortDataAdjust record 0x802F
        {
            EDDI_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE  pPDPortDataAdjustSet;

            if (0 == pPrmWrite->edd_port_id)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                                  pPrmWrite->edd_port_id, 0);
                return EDD_STS_ERR_PRM_PORTID;
            }

            pPDPortDataAdjustSet = &pDDB->PRM.PDPortDataAdjust.RecordSet_B[pPrmWrite->edd_port_id - 1];

            pPDPortDataAdjustSet->MAUTypePresent = LSA_FALSE;
            pPDPortDataAdjustSet->MulticastBoundaryPresent = LSA_FALSE;
            pPDPortDataAdjustSet->DomainBoundaryPresent = LSA_FALSE;
            pPDPortDataAdjustSet->DCPBoundaryPresent = LSA_FALSE;
            pPDPortDataAdjustSet->PortStatePresent = LSA_FALSE;
            pDDB->PRM.PDPortDataAdjust.RecordState_B[pPrmWrite->edd_port_id - 1] = EDDI_PRM_NOT_VALID;

            //empty record
            if (pPrmWrite->record_data_length == EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_EMPTY)
            {
                return EDD_STS_OK;
            }

            Result = EDDI_PrmCheckPDPortDataAdjust((EDD_UPPER_PRM_WRITE_PTR_TYPE)pPrmWrite, (EDDI_LOCAL_DDB_PTR_TYPE)pDDB);
            if (EDD_STS_OK != Result)
            {
                pPDPortDataAdjustSet->MAUTypePresent = LSA_FALSE;
                pPDPortDataAdjustSet->MulticastBoundaryPresent = LSA_FALSE;
                pPDPortDataAdjustSet->DomainBoundaryPresent = LSA_FALSE;
                pPDPortDataAdjustSet->DCPBoundaryPresent = LSA_FALSE;
                pPDPortDataAdjustSet->PortStatePresent = LSA_FALSE;
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Result > ->:0x%X", Result);
                return Result;
            }

            EDDI_MemCopy(&pPDPortDataAdjustSet->PDPortDataAdjust_Max_Record_Net,
                         pPrmWrite->record_data,
                         pPrmWrite->record_data_length);

            pPDPortDataAdjustSet->record_data_length = pPrmWrite->record_data_length;

            pDDB->PRM.PDPortDataAdjust.RecordState_B[pPrmWrite->edd_port_id - 1] = EDDI_PRM_WRITE_DONE;

            break;
        }

        case EDDI_PRM_INDEX_PDINTERFACE_MRP_DATA_ADJUST: //supports PDInterfaceMrpDataAdjust record 0x8052
        {
            //empty record
            if (pPrmWrite->record_data_length == EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_EMPTY)
            {
                return EDD_STS_OK;
            }
            
            /* we only need the info that a write was done! */
            pDDB->PRM.PDInterfaceMrpDataAdjust.RecordState_B = EDDI_PRM_WRITE_DONE;
            break;
        }

        case EDDI_PRM_INDEX_PDPORT_MRP_DATA_ADJUST: //supports PDPortMrpDataAdjust record 0x8053
        {
            if (0 == pPrmWrite->edd_port_id)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                                  pPrmWrite->edd_port_id, 0);
                return EDD_STS_ERR_PRM_PORTID;
            }

            /* we only need the info that a write was done! */
            pDDB->PRM.PDPortMrpDataAdjust.RecordState_B[pPrmWrite->edd_port_id - 1] = EDDI_PRM_WRITE_DONE;
            break;
        }

        case EDDI_PRM_INDEX_PDSET_DEFAULT_PORT_STATES: //supports PDSetDefaultPortStates record 0x10001
        {
            Result = EDDI_PrmCheckPDSetDefaultPortStates(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Result");
                return Result;
            }

            break;
        }

        case EDDI_PRM_INDEX_PDIR_APPLICATION_DATA: //supports PDIRApplicationData record 0x10002
        {
            EDDI_LOCAL_MEM_U8_PTR_TYPE  pRecord = pPrmWrite->record_data;

            Result = EDDI_PrmCheckApplicationData(pPrmWrite, &pRecord, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckApplicationData, Result");
                return Result;
            }

            break;
        }

        case EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION: //supports PDNRTFeedInLoadLimitation record 0x10003
        {
            pDDB->PRM.PDNrtLoadLimitation.State_B = EDDI_PRM_NOT_VALID;

            Result = EDDI_PrmCheckPDNRTFeedInLoadLimitation(pPrmWrite, pDDB);
            if (EDD_STS_OK != Result)
            {
                EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, Result:0x%X", Result);
                return Result;
            }
        
            EDDI_MemCopy(&pDDB->PRM.PDNrtLoadLimitation.pRecordSet_B->RecordSet, pPrmWrite->record_data, pPrmWrite->record_data_length);
            pDDB->PRM.PDNrtLoadLimitation.State_B = EDDI_PRM_WRITE_DONE;

            break;
        }

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDD_STS_ERR_PRM_INDEX);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPrmWrite->record_index > ->:0x%X", pPrmWrite->record_index);
            return EDD_STS_ERR_PRM_INDEX;
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "PrmCheckRecord<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckApplicationData()                   */
/*                                                                         */
/* D e s c r i p t i o n: Checks the EDDI_PrmCheckApplicationData record   */
/*                        and fills up managment with data from record.    */
/*                                                                         */
/*                        This record is only allowed before first         */
/*                        COMMIT after startup!                            */
/*                                                                         */
/* A r g u m e n t s:     pPrmWrite  : Pointer to PRM WRITE RQB            */
/*                        pDDB       : Pointer to DDB                      */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/*                        EDD_STS_ERR_PRM_PORTID                           */
/*                        EDD_STS_ERR_PRM_DATA                             */
/*                        EDD_STS_ERR_PRM_INDEX                            */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckApplicationData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                               EDDI_LOCAL_MEM_U8_PTR_TYPE               * pRecord, 
                                                               EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_RESULT                                           Status;
    EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE           PDirApplicationData;
    EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE           PDirApplicationDataNetworkFormat;
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE BlockHeader; //BlockHeader from PDIRApplicationData cannot be passed to functions as compiler my not align it
    //EDDI_LOCAL_MEM_U8_PTR_TYPE                  pRecord = pPrmWrite->record_data;

    pDDB->PRM.PDirApplicationData.State_B = EDDI_PRM_NOT_VALID;

    //BlockHeader 
    Status = EDDI_PrmFctGetBlockHeaderWithoutPadding(pRecord, &BlockHeader, &PDirApplicationDataNetworkFormat.BlockHeader, pDDB);
    PDirApplicationData.BlockHeader.BlockLength     = BlockHeader.BlockLength;
    PDirApplicationData.BlockHeader.BlockType       = BlockHeader.BlockType;
    PDirApplicationData.BlockHeader.BlockVersionHigh= BlockHeader.BlockVersionHigh;
    PDirApplicationData.BlockHeader.BlockVersionLow = BlockHeader.BlockVersionLow;

    if //BlockType of PDIRGlobalData invalid
       (PDirApplicationData.BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PDIR_APPLICATION_DATA)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDirApplicationData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmCheckApplicationData, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
                          PDirApplicationData.BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PDIR_APPLICATION_DATA);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (PDirApplicationData.BlockHeader.BlockLength != (LSA_UINT16)(pPrmWrite->record_data_length - EDDI_PRM_BLOCK_WITHOUT_LENGTH))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckApplicationData, pBlockHeader->BlockLength > ->:0x%X :0x%X",
                          PDirApplicationData.BlockHeader.BlockLength, pPrmWrite->record_data_length - EDDI_PRM_BLOCK_WITHOUT_LENGTH);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (PDirApplicationData.BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDIRData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckApplicationData, pBlockHeader->BlockVersionHigh > ->:0x%X :0x%X",
                          PDirApplicationData.BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }
    
    //ConsumerCnt
    PDirApplicationData.ConsumerCnt = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, PDirApplicationDataNetworkFormat.ConsumerCnt);
    
    //ConsumerIODataLength
    PDirApplicationData.ConsumerIODataLength = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, PDirApplicationDataNetworkFormat.ConsumerIODataLength);

    //Reserved
    PDirApplicationData.Reserved = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, PDirApplicationDataNetworkFormat.Reserved);

    if (PDirApplicationData.Reserved != 0)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDirApplicationData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_OFFSET_Reserved);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_PrmCheckApplicationData, Invalid Reserved:0x%X", PDirApplicationData.Reserved);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pDDB->PRM.PDirApplicationData.RecordSet_B.ConsumerCnt                   = PDirApplicationData.ConsumerCnt;
    pDDB->PRM.PDirApplicationData.RecordSet_B.ConsumerIODataLength          = PDirApplicationData.ConsumerIODataLength;
    pDDB->PRM.PDirApplicationData.RecordSet_B.bCalculatedTransferEndValues  = LSA_FALSE;
    pDDB->PRM.PDirApplicationData.RecordSet_B.Reserved                      = PDirApplicationData.Reserved;
    pDDB->PRM.PDirApplicationData.State_B                                   = EDDI_PRM_WRITE_DONE;

    return Status;
}


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
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckIsIRTFlex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_BOOL   bPDPortDataAdjust = LSA_FALSE;
    LSA_UINT8  UsrPortIndex;
    
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        /* record was written */
        if (pDDB->PRM.PDPortDataAdjust.RecordState_B[UsrPortIndex] == EDDI_PRM_WRITE_DONE)
        {
            bPDPortDataAdjust = LSA_TRUE; 
        }
    }
   
    // IRTFLex = (No PDIRData) && (PDPortDataAdjust) && (PDSyncData)
    if (   (EDDI_PRM_WRITE_DONE != pDDB->PRM.PDIRData.State_B)
        && (bPDPortDataAdjust)
        && (EDDI_PRM_WRITE_DONE == pDDB->PRM.PDSyncData.State_B))
    {
        return LSA_TRUE;  
    }
    else
    {
        return LSA_FALSE;
    } 
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetBlockHeader()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_LOCAL_MEM_U8_PTR_TYPE                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetBlockHeader( EDDI_LOCAL_MEM_U8_PTR_TYPE       *  pRecord,
                                                            EDDI_PRM_RECORD_HEADER_TYPE      *  pBlockHeader,
                                                            EDDI_PRM_RECORD_HEADER_TYPE      *  pBlockHeaderNetworkFormat,
                                                            EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_RESULT  const  Status = EDD_STS_OK;

    LSA_UNUSED_ARG(pDDB);
                                                                                     
    pBlockHeader->BlockType = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pBlockHeaderNetworkFormat->BlockType);

    pBlockHeader->BlockLength = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pBlockHeaderNetworkFormat->BlockLength );

    pBlockHeader->BlockVersionHigh = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->BlockVersionHigh);

    pBlockHeader->BlockVersionLow= **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->BlockVersionLow);

    pBlockHeader->Padding1_1= **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->Padding1_1);

    pBlockHeader->Padding1_2= **pRecord; 
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->Padding1_2);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetBlockTypeLength()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetBlockTypeLength( LSA_UINT8   *  const  pBlockHeader,
                                                              LSA_UINT16  *  const  pBlockTypeNetwork,
                                                              LSA_UINT16  *  const  pBlockLength )
{
    LSA_UINT8  *  pTemp8 = pBlockHeader + EDDI_PRM_BLOCKHEADER_OFFSET_BlockType;

    *pBlockTypeNetwork = EDDI_GET_U16_KEEP_BYTEORDER(pTemp8);
    pTemp8 = pBlockHeader + EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength;
    *pBlockLength = EDDI_GET_U16(pTemp8);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmGetActiveIrtPortCount()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_PrmGetActiveIrtPortCount                    */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetActivePortCount( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB )  
{
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32         PortCnt    = 0;
    LSA_UINT32         UsrPortIndex;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {       
        if (   /* default settings      */
               /* use default (Autoneg) */
               /* use default (PowerOn) */
               (pDDB->PRM.PDPortDataAdjust.RecordState_B[UsrPortIndex] != EDDI_PRM_WRITE_DONE)

               /* if no PortState and no MAUType exist */
               /* use default (Autoneg) */
               /* use default (PowerOn) */
            || (   (!pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUTypePresent) 
                && (!pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].PortStatePresent))

               /* if only MauType -> Speed/Mode and PowerOn */
            || (pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUTypePresent)
           )
        {
            PortCnt++;
        }
    }

    return PortCnt;
}
/*---------------------- end [subroutine] ---------------------------------*/





/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmFctGetBlockHeaderWithoutPadding()        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetBlockHeaderWithoutPadding( EDDI_LOCAL_MEM_U8_PTR_TYPE                            * pRecord,
                                                                          EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE  * pBlockHeader,
                                                                          EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE           * pBlockHeaderNetworkFormat,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE                     const    pDDB)           
{   
    LSA_RESULT  const  Status = EDD_STS_OK;
      
    LSA_UNUSED_ARG(pDDB);
                                                                                     
    pBlockHeader->BlockType = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pBlockHeaderNetworkFormat->BlockType);
    
    pBlockHeader->BlockLength = EDDI_GET_U16(*pRecord);
    EDDI_GET_U16_INCR_NO_SWAP(*pRecord, pBlockHeaderNetworkFormat->BlockLength );

    pBlockHeader->BlockVersionHigh = **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->BlockVersionHigh);

    pBlockHeader->BlockVersionLow= **pRecord;
    EDDI_GET_U8_INCR(*pRecord, pBlockHeaderNetworkFormat->BlockVersionLow);
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_CFG_DFP_ON)                                                                  
/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmGetCycleBaseFactor()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_PrmGetCycleBaseFactor                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetCycleBaseFactor( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{   
    return pDDB->CRT.CycleBaseFactor;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmGetFrameDataProperties()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_PrmGetFrameDataProperties                   */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetFrameDataProperties( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_BOOL                 const  bIsRecordSet_B,
                                                                 LSA_UINT32                      BitHigh,
                                                                 LSA_UINT32                      BitLow )
{
    LSA_UINT32  FrameDataProperties = 0;
    
    EDDI_PRM_RECORD_IRT_PTR_TYPE   pRecordSet = pDDB->PRM.PDIRData.pRecordSet_A;
    
    if(bIsRecordSet_B)
    {
        pRecordSet = pDDB->PRM.PDIRData.pRecordSet_B;
    }

        
    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pRecordSet->pLocalPDIRDataRecord->PDIRFrameData.BlockHeader.BlockVersionLow )
    {
        FrameDataProperties = EDDI_GetBitField32NoSwap(pRecordSet->pLocalPDIRDataRecord->PDIRFrameData.FrameDataProperties, (LSA_UINT32)BitHigh, (LSA_UINT32)BitLow);
    }
    
    return FrameDataProperties;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //defined (EDDI_CFG_DFP_ON)

#if defined (EDDI_CFG_FRAG_ON)
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmGetFragSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{   
    return pDDB->PRM.PDIRData.pRecordSet_B->NrtFrag.FragSize;
}

#endif //defined (EDDI_CFG_FRAG_ON)

/*****************************************************************************/
/*  end of file eddi_prm_record_common.c                                     */
/*****************************************************************************/
