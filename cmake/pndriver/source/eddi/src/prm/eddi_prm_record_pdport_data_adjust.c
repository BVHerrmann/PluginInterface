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
/*  F i l e               &F: eddi_prm_record_pdport_data_adjust.c      :F&  */
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
/*  06.02.15    TH    refactor AdjustPortState to AdjustLinkState            */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"
#include "eddi_prm_record_pdport_data_adjust.h"
#include "eddi_prm_record_pdir_data.h"
#include "eddi_prm_record_common.h"
#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDPORT_DATA_ADJUST
#define LTRC_ACT_MODUL_ID  407

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************
 * F u n c t i o n:       EDDI_PrmCheckPDPortDataAdjust()
 *
 * D e s c r i p t i o n:
 *
 * PNIO-Definition of PDPortDataAdjust:
 *
 * BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, { [AdjustDomainBoundary],
 * [AdjustMulticastBoundary], [AdjustMAUType ^ pAdjustLinkState],
 * [AdjustPeerToPeerBoundary], [AdjustDCPBoundary], [PreambleLength] }
 *
 * A r g u m e n t s:
 *
 * Return Value:
 *
 ***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDPortDataAdjust( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_UINT8                                        Local;
    EDDI_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE        pPDPortDataAdjustSet;
    EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_PTR_TYPE  pPDPortAdjustMax;
    LSA_RESULT                                       Result;
    LSA_UINT16                                       Temp16;    

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDPortDataAdjust->");

    pPDPortDataAdjustSet = &pDDB->PRM.PDPortDataAdjust.RecordSet_B[pPrmWrite->edd_port_id - 1];

    pPDPortAdjustMax = (EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_PTR_TYPE)(void *)pPrmWrite->record_data;

    /******* Check PDPortData ******/

    //wrong length
    if (   (pPrmWrite->record_data_length < EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_EMPTY)
        || (pPrmWrite->record_data_length > sizeof(EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE)) ) 
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDPortDataAdjust);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, 0);
        return EDD_STS_ERR_PRM_DATA;
    }

    {
        LSA_UINT16 BlockTypeNetwork;
        LSA_UINT16 BlockLength;

        EDDI_PrmFctGetBlockTypeLength((LSA_UINT8 *)(LSA_VOID *)&pPDPortAdjustMax->BlockHeader, &BlockTypeNetwork, &BlockLength);
        if (BlockTypeNetwork != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDPortDataAdjust);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid BlockType; -> BlockType:0x%X ExpectedBlockType:0x%X",
                              BlockTypeNetwork, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (BlockLength != (pPrmWrite->record_data_length - EDDI_PRM_BLOCK_WITHOUT_LENGTH)  )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDPortDataAdjust);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid BlockLength; -> BlockLength:0x%X ExpectedBlockLength:0x%X",
                              BlockLength, EDDI_PRM_H_BL_PDPORT_DATA_ADJUST);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDPortAdjustMax->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid BlockVersion; -> BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pPDPortAdjustMax->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDPortAdjustMax->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pPDPortAdjustMax->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDPortAdjustMax->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pPDPortAdjustMax->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDPortAdjustMax->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pPDPortAdjustMax->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    EDDI_GET_U16Offset(pPDPortAdjustMax, EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SlotNummer, Temp16);
    if (pPrmWrite->slot_number != Temp16)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustSlotNumber_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SlotNummer);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid slot_number; -> slot_number:0x%X SlotNumber:0x%X",
                          pPrmWrite->slot_number, Temp16);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pPDPortDataAdjustSet->SlotNumber = pPrmWrite->slot_number;

    EDDI_GET_U16Offset(pPDPortAdjustMax, EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SubSlotNummer, Temp16);
    if (pPrmWrite->subslot_number != Temp16)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustSubSlotNumber_PDPortDataAdjust);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SubSlotNummer);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, Invalid subslot_number; -> subslot_number:0x%X SubslotNummer:0x%X",
                          pPrmWrite->subslot_number, Temp16);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pPDPortDataAdjustSet->SubSlotNumber = pPrmWrite->subslot_number;

    Local = pPrmWrite->Local;

    /* If LesserCheckQuality we verify the same way as Local.. so set Local */
    if (pDDB->PRM.LesserCheckQuality)
    {
        Local = EDD_PRM_PARAMS_ASSIGNED_LOCALLY;
    }

    // Check all possible Subblocks
    {
        LSA_UINT32                  const  MaxSubBlockCount = 6;
        LSA_UINT32                         iSubBlock        = 0;
        LSA_UINT32                         LocalRecord[(sizeof(EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE)+3)/4];
        EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pLocalRecord = (EDDI_LOCAL_MEM_U8_PTR_TYPE)(LSA_VOID *)&LocalRecord[0];
        EDDI_LOCAL_MEM_U8_PTR_TYPE         pBytePtr;
        EDDI_LOCAL_MEM_U8_PTR_TYPE         pEnd;

        // start of subblock is after PDPortDataAdjust - Header
        pBytePtr = pLocalRecord;
        pEnd = pLocalRecord + pPrmWrite->record_data_length;
        EDDI_MemCopy(&LocalRecord[0], pPrmWrite->record_data, pPrmWrite->record_data_length);

        pBytePtr += 12;
        EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_BlockHeader1);

        for (;;)
        {
            EDDI_PRM_RECORD_HEADER_TYPE  *  const  pSubBlockHeader = (EDDI_PRM_RECORD_HEADER_TYPE *)(void*)pBytePtr;

            if ((LSA_UINT32)((pBytePtr + EDDI_NTOHS(pSubBlockHeader->BlockLength) + EDDI_PRM_BLOCK_WITHOUT_LENGTH)) > (LSA_UINT32)pEnd)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SubBlockTooBig_PDPortDataAdjust);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, pBytePtr - record_data:0x%X record_data_length:0x%X",
                                  (LSA_UINT32)pBytePtr - (LSA_UINT32)pPrmWrite->record_data, pPrmWrite->record_data_length);
                return EDD_STS_ERR_PRM_BLOCK;
            }

            switch (pSubBlockHeader->BlockType)
            {
                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH:
                {
                    Result = EDDI_CheckPreambleLength((EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE *)(void *)pBytePtr, pDDB);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;
                }

                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MULTICAST_BOUNDARY:
                {
                    Result = EDDI_CheckAdjustMulticastBoundary((EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE *)(void *)pBytePtr,
                                                               (LSA_UINT16)(pPrmWrite->edd_port_id - 1),
                                                               pDDB);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;
                }


                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY:
                {
                    Result = EDDI_CheckAdjustDomainBoundary((EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE *)(void *)pBytePtr,
                                                            (LSA_UINT16)(pPrmWrite->edd_port_id - 1),
                                                            pDDB);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;                                    
                }

                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MAU_TYPE:
                {
                    Result = EDDI_CheckAdjustMAUType((EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE *)(void *)pBytePtr,
                                                     (LSA_UINT16)(pPrmWrite->edd_port_id - 1),
                                                     Local,
                                                     pDDB);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;
                }

                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_LINK_STATE:
                {
                    Result = EDDI_CheckAdjustLinkState((EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE *)(void *)pBytePtr,
                                                       (LSA_UINT16)(pPrmWrite->edd_port_id - 1),
                                                       pDDB);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;
                }

                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PEER_TP_PEER_BOUNDARY:
                {
                    // ignore content
                    break;
                } 

                case EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DCP_BOUNDARY:
                {
                    Result = EDDI_CheckAdjustDCPBoundary((EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE *)(void *)pBytePtr,
                                                         (LSA_UINT16)(pPrmWrite->edd_port_id - 1),
                                                         pDDB);
                    pDDB->PRM.PrmDetailErr.EDDI_PRM_ADJUST_PORT_STATE_Offset[(LSA_UINT16)(pPrmWrite->edd_port_id - 1)] = EDDI_POINTER_BYTE_DISTANCE(pBytePtr, pPrmWrite->record_data) +
                                                                                                                         EDDI_PRM_RECORD_ADJUST_PORT_STATE_OFFSET_PortState;
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                    break;
                }
                
                default:
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDPortDataAdjust);
                    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_PrmCheckPDPortDataAdjust, EDDI_ERR_INV_BlockType_PDPortDataAdjust -> BlockType:0x%X record_data_length:0x%X",
                                      pSubBlockHeader->BlockType, pPrmWrite->record_data_length);
                    return EDD_STS_ERR_PRM_BLOCK;
                }
            }

            iSubBlock++;
            if (iSubBlock > MaxSubBlockCount)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_TooManySubBlocks_PDPortDataAdjust);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDPortDataAdjust, TooManySubBlocks, iSubBlock:0x%X MaxSubBlockCount:0x%X",
                                  iSubBlock, MaxSubBlockCount);
                return EDD_STS_ERR_PRM_BLOCK;
            }

            // Goto next Block
            pBytePtr =   pBytePtr   + EDDI_NTOHS(pSubBlockHeader->BlockLength) + EDDI_PRM_BLOCK_WITHOUT_LENGTH;
            EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_NTOHS(pSubBlockHeader->BlockLength) + EDDI_PRM_BLOCK_WITHOUT_LENGTH);

            if (pBytePtr == pEnd)
            {
                // OK: end of block reached --> exit loop
                return EDD_STS_OK;
            }
        }
    }
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPreambleLength( const EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE  *  const  pAdjustPreambleLength,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE                         const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPreambleLength->");

    if (pAdjustPreambleLength->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid BlockLen;  -> BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH:0x%X",
                          pAdjustPreambleLength->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustPreambleLength->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustPreambleLength);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustPreambleLength->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustPreambleLength->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid BlockVersion; -> pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustPreambleLength->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustPreambleLength->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustPreambleLength->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustPreambleLength->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid Padding1_1 1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustPreambleLength->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustPreambleLength->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid Padding1_2 1; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustPreambleLength->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if ((EDDI_NTOHS(pAdjustPreambleLength->Reserved) & 1) != 0x0)   // Only allowed 0, IRTE donsn't support short (1 Byte) preamble
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_ReservedPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_OFFSET_Reserved);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength: Error IRTE donsn't support short (1 Byte) preamble.");
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustPreambleLength->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustPropertiesPreambleLength);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckPreambleLength Invalid AdjustProperties; -> AdjustProperties:0x%X :0x%X",
                          pAdjustPreambleLength->AdjustProperties, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }
    
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckPreambleLength<-");
    
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustMulticastBoundary( const EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE  *  const  pAdjustBoundary,
                                                                          LSA_UINT16                                         const  UsrPortIndex,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE                            const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustMulticastBoundary->");

    if (pAdjustBoundary->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DTATA_ADJUST_MULTICAST_BOUNDARY)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid BlockLen;  -> BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PDPortDataAdjust_Boundary:0x%X",
                          pAdjustBoundary->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DTATA_ADJUST_MULTICAST_BOUNDARY);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustBoundary->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MULTICAST_BOUNDARY)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustMulticastBoundary);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustBoundary->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MULTICAST_BOUNDARY);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustBoundary->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid BlockVersion; -> pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid Padding1_1 1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid Padding1_2 1; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid Padding1_1 2; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid Padding1_2 2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustProperties_AdjustMulticastBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMulticastBoundary Invalid AdjustProperties; -> AdjustProperties:0x%X :0x%X",
                          pAdjustBoundary->AdjustProperties, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MulticastBoundaryPresent = LSA_TRUE;
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MulticastBoundary        = EDDI_NTOHL(pAdjustBoundary->MulticastBoundary);
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MulticastBoundaryAP      = EDDI_NTOHS(pAdjustBoundary->AdjustProperties);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustMulticastBoundary<-");

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustDomainBoundary( const EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE  *  const  pAdjustBoundary,
                                                                       LSA_UINT16                                      const  UsrPortIndex,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE                         const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustDomainBoundary->");

    if (pAdjustBoundary->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid BlockLen;  -> BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PDPortDataAdjust_Boundary:0x%X",
                          pAdjustBoundary->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustBoundary->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustDomainBoundary);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustBoundary->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustBoundary->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid BlockVersion; -> pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW_ADJUST_DOMAIN_BOUNDARY)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionLow, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid Padding1_1 1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid Padding1_2 1; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid Padding1_1 2; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid Padding1_2 2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustProperties_AdjustDomainBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDomainBoundary, Invalid AdjustProperties; -> AdjustProperties:0x%X :0x%X",
                          pAdjustBoundary->AdjustProperties, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DomainBoundaryPresent = LSA_TRUE;

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DomainBoundaryIngress = EDDI_NTOHL(pAdjustBoundary->DomainBoundaryIngress);
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DomainBoundaryEgress  = EDDI_NTOHL(pAdjustBoundary->DomainBoundaryEgress);
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DomainBoundaryAP      = EDDI_NTOHS(pAdjustBoundary->AdjustProperties);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustDomainBoundary<-");

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustLinkState( const EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE  *  const  pAdjustLinkState,
                                                                  LSA_UINT16                                 const  UsrPortIndex,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustLinkState->");

    if (pAdjustLinkState->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_LINK_STATE)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid BlockLen;  -> BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_LINK_STATE:0x%X",
                          pAdjustLinkState->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_LINK_STATE);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustLinkState->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_LINK_STATE)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustLinkState);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid BlockType; -> pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustLinkState->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_LINK_STATE);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustLinkState->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid BlockVersion; -> pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustLinkState->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustLinkState->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustLinkState->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustLinkState->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustLinkState->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustLinkState->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustLinkState->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustLinkState->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustProperties_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_PORT_STATE_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid AdjustProperties; -> AdjustProperties:0x%X :0x%X",
                          pAdjustLinkState->AdjustProperties, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (EDDI_NTOHS(pAdjustLinkState->PortState) != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PORTE_STATE_DOWN) //down
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortState_AdjustLinkState);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_PORT_STATE_OFFSET_PortState);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustLinkState, Invalid PortState; -> PortState:0x%X :0x%X",
                          pAdjustLinkState->PortState, 0x0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].PortStatePresent = LSA_TRUE;

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].PortState        = EDDI_NTOHS(pAdjustLinkState->PortState);

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].PortStateAP      = EDDI_NTOHS(pAdjustLinkState->AdjustProperties);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustLinkState<-");

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustMAUType( const EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE  *  const  pAdjustMAUType,
                                                                LSA_UINT16                               const  UsrPortIndex,
                                                                LSA_UINT8                                const  Local,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB )
{
    LSA_UNUSED_ARG(Local);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustMAUType->");

    if (pAdjustMAUType->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_MAU_TYPE)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid BlockLen, BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_MAU_TYPE:0x%X",
                          pAdjustMAUType->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_MAU_TYPE);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustMAUType->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MAU_TYPE)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustMAUType);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid BlockType, pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustMAUType->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_LINK_STATE);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustMAUType->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid BlockVersion, pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustMAUType->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustMAUType->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid BlockVersion, BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustMAUType->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustMAUType->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid Padding1_1, Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustMAUType->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustMAUType->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid Padding1_2, Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustMAUType->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustMAUType->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustProperties_AdjustMAUType);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_MAU_TYPE_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustMAUType, Invalid AdjustProperties, AdjustProperties:0x%X ExpectedAdjustProperties:0x%X",
                          pAdjustMAUType->AdjustProperties, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //only store MAUType, evaluation will be done during PRM_END - done
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUTypePresent = LSA_TRUE;

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUType        = EDDI_NTOHS(pAdjustMAUType->MAUType);

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].MAUTypeAP      = EDDI_NTOHS(pAdjustMAUType->AdjustProperties);

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].LinkStat       = EDD_LINK_UNKNOWN;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustMAUType<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckAdjustDCPBoundary()                    */
/*                                                                         */
/* D e s c r i p t i o n: Check the DCPBoundary record. If ok stores       */
/*                        values to record set B                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDD_STS_OK                                       */
/*                        EDD_STS_ERR_PRM_BLOCK                            */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustDCPBoundary( const EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE  *  const  pAdjustBoundary,
                                                                    LSA_UINT16                                   const  UsrPortIndex,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustDCPBoundary->");

    if (pAdjustBoundary->BlockHeader.BlockLength != EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DCP_BOUNDARY)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid BlockLen, BlockHeader.BlockLength:0x%X EDDI_PRM_H_BL_PDPortDataAdjust_Boundary:0x%X",
                          pAdjustBoundary->BlockHeader.BlockLength, EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DCP_BOUNDARY);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    //if (pAdjustBoundary->BlockHeader.BlockType != EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DCP_BOUNDARY)
    //{
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_AdjustDCPBoundary);
    //    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
    //    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid BlockType, pBlockHeader->BlockType:0x%X Expected:0x%X",
    //                      pAdjustBoundary->BlockHeader.BlockType, EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DCP_BOUNDARY);
    //    return EDD_STS_ERR_PRM_BLOCK;
    //}

    if (pAdjustBoundary->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid BlockVersion, pBlockHeader->BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid BlockVersion, BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pAdjustBoundary->BlockHeader.BlockVersionLow, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_1_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid Padding1_1 1, Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->BlockHeader.Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockPadding1_2_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid Padding1_2 1, Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_1 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid Padding1_1 2, Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pAdjustBoundary->Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->Padding1_2 != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid Padding1_2 2, Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pAdjustBoundary->Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pAdjustBoundary->AdjustProperties != 0x00)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_AdjustProperties_AdjustDCPBoundary);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_AdjustProperties);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CheckAdjustDCPBoundary, Invalid AdjustProperties, AdjustProperties:0x%X",
                          pAdjustBoundary->AdjustProperties);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DCPBoundaryPresent = LSA_TRUE;
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DCPBoundary        = EDDI_NTOHL(pAdjustBoundary->DCPBoundary);
    pDDB->PRM.PDPortDataAdjust.RecordSet_B[UsrPortIndex].DCPBoundaryAP      = EDDI_NTOHS(pAdjustBoundary->AdjustProperties);

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckAdjustDCPBoundary<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetLinkStatusForFastForwarding()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SetLinkStatusForFastForwarding( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT                               const  Status              = EDD_STS_OK;
    LSA_UINT8                                       HwPortIndex;
    LSA_UINT32                               const  PortMapCnt          = pDDB->PM.PortMap.PortCnt;
    EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  *  const  pFrameData          = &pDDB->PRM.PDIRData.pRecordSet_B->pLocalPDIRDataRecord->PDIRFrameData;
    LSA_UINT32                                      PhyEnabledPortCount = 0;
    
    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11 == pFrameData->BlockHeader.BlockVersionLow) //BlockVersionLow = 1
    { 
        LSA_UINT32  const  FrameDataProperties = EDDI_GetBitField32NoSwap(pFrameData->FrameDataProperties, EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD);

        if (EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF == FrameDataProperties)
        {
            //Get enabled Port and set the LinkStatus to down on each port which has phy status disabled
            for (HwPortIndex = 0; HwPortIndex < PortMapCnt; HwPortIndex++)
            {               
                if (pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PhyStatus == EDD_PHY_STATUS_DISABLED)
                {
                    pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus = EDD_LINK_DOWN;
                }
            }

            PhyEnabledPortCount = EDDI_PrmGetActivePortCount(pDDB);

            //Check for enabled Ports. Note: Only one enabled port is allowed!
            if (PhyEnabledPortCount != 1)
            {
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetLinkStatusForFastForwarding, for Fast Forwarding is only 1 Port allowed to enable, Port's enabled:0x%X Result:0x%X", PhyEnabledPortCount, Status);
                EDDI_PRM_SET_DETAIL_ERR(&pDDB->PRM.PrmDetailErr, EDD_PRM_ERR_PORT_ID_DEFAULT,
                                                                 EDDI_PRM_INDEX_PDIR_DATA,
                                                                 EDD_PRM_ERR_OFFSET_DEFAULT,
                                                                 EDD_PRM_ERR_FAULT_DEFAULT);
                return EDD_STS_ERR_PRM_CONSISTENCY;
            }
        }
    }
    
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdport_data_adjust.c                         */
/*****************************************************************************/

