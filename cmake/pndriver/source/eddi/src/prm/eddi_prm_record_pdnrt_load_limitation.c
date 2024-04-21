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
/*  F i l e               &F: eddi_prm_record_pdnrt_load_limitation.c   :F&  */
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
/*  15.04.15    BV    initial version                                        */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"
#include "eddi_prm_record_pdnrt_load_limitation.h"
#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDNRT_LOAD_LIMITATION
#define LTRC_ACT_MODUL_ID  414

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDNRTFeedInLoadLimitation( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                         EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE_PTR_TYPE     pPDNRT_FillRec;


    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDNRTFeedInLoadLimitation->");

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDNRTFeedInLoadLimitation);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmWrite->record_data_length != EDDI_PRM_PDNRT_FILL_RECORD_LENGTH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDNRTFeedInLoadLimitation);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, EDDI_PRM_PDNRT_FILL_RECORD_LENGTH);
        return EDD_STS_ERR_PRM_DATA;
    }

    pPDNRT_FillRec = (EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE_PTR_TYPE)(void *)pPrmWrite->record_data;

    {
        LSA_UINT16 BlockTypeNetwork;
        LSA_UINT16 BlockLength;

        EDDI_PrmFctGetBlockTypeLength((LSA_UINT8 *)(LSA_VOID *)&pPDNRT_FillRec->BlockHeader, &BlockTypeNetwork, &BlockLength);
        if (BlockTypeNetwork != EDDI_PRM_BLOCKTYPE_PDNRT_FILL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDNRTFeedInLoadLimitation);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->BlockHeader.BlockType > ->:0x%X",
                              BlockTypeNetwork);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (BlockLength != EDDI_PRM_HEADER_BL_PDNRT_FILL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDNRTFeedInLoadLimitation);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->BlockHeader.BlockLength > ->:0x%X",
                              BlockLength);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDNRT_FillRec->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->BlockHeader.BlockVersionHigh > ->:0x%X :0x%X",
                          pPDNRT_FillRec->BlockHeader.BlockVersionHigh, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->BlockHeader.BlockVersionLow > ->:0x%X :0x%X",
                          pPDNRT_FillRec->BlockHeader.BlockVersionLow, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->BlockHeader.Padding1_1 != 0x00)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pPDNRT_FillRec->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->BlockHeader.Padding1_2 != 0x00)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDIRGlobalData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pPDNRT_FillRec->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->LoadLimitationActive > 1)
    {   //errror LoadLimitationActive
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_LoadLimitationActive_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_LoadLimitationActive);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->LoadLimitationActive > ->:0x%X",
                          pPDNRT_FillRec->LoadLimitationActive);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->IO_Configured > 1)
    {   // error IO_Configured
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IO_Configured_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_IO_Configured);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, pPDNRT_FillRec->IO_Configured > ->:0x%X",
                          pPDNRT_FillRec->IO_Configured);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->Reserved != 0)
    {   // error IO_Configured
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_Reserved);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, Invalid Reserved:0x%X",
                          pPDNRT_FillRec->Reserved);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNRT_FillRec->IO_Configured)
    {
        if (0 == pPDNRT_FillRec->LoadLimitationActive)
        {   // EDDI_ERR_INV_PRM_BLOCK_INVALID_PDNRTFeedInLoadLimitation
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PRM_BLOCK_INVALID_PDNRTFeedInLoadLimitation);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_LoadLimitationActive);
            EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, IO-Configured but FILL-inactive");
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }
    //else
    //{
    //    if ((pDDB->CRT.ProviderList.UsedEntries       > 0)  ||          // Provider vorhanden
    //        (pDDB->CRT.ConsumerList.UsedEntriesDFP    > 0)  ||          // Consumer vorhanden
    //        (pDDB->CRT.ConsumerList.UsedEntriesRTC123 > 0)   )
    //    {
    //        //error IO_Configured
    //    }
    //}
    if (!pPDNRT_FillRec->LoadLimitationActive  &&  (pDDB->HSYNCRole == EDDI_HSYNC_ROLE_APPL_SUPPORT))
    {   //errror LoadLimitationActive
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_LoadLimitationActive_PDNRTFeedInLoadLimitation);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_LoadLimitationActive);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNRTFeedInLoadLimitation, HSYNC_ROLE_APPL_SUPPORT requires LoadLimitationActive != 0");
        return EDD_STS_ERR_PRM_BLOCK;
    }


    pDDB->PRM.PDNrtLoadLimitation.pRecordSet_B->LoadLimitationActive = pPDNRT_FillRec->LoadLimitationActive ? LSA_TRUE : LSA_FALSE;
    pDDB->PRM.PDNrtLoadLimitation.pRecordSet_B->IO_Configuration     = pPDNRT_FillRec->IO_Configured        ? LSA_TRUE : LSA_FALSE;
    
    pDDB->PRM.PDNrtLoadLimitation.State_B = EDDI_PRM_WRITE_DONE;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdnrt_load_limitation.c                      */
/*****************************************************************************/

