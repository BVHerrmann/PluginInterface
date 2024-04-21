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
/*  F i l e               &F: eddi_prm_record_pdnc_data_check.c         :F&  */
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
#include "eddi_prm_record_pdnc_data_check.h"
//#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDNC_DATA_CHECK
#define LTRC_ACT_MODUL_ID  406

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDNCData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE  *  pPDNCDataCheckRec;
    EDDI_PRM_PDNC_DATA_CHECK              *  pPDNC;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDNCData->");

    pPDNC              = &pDDB->PRM.PDNCDataCheck;

    pPDNC->edd_port_id = pPrmWrite->edd_port_id;

    pPDNCDataCheckRec  = pPDNC->pB;

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDNCDataCheck);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNCData, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmWrite->record_data_length != EDDI_PRM_LENGTH_PDNC_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDNCDataCheck);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDNCData, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, 0);
        return EDD_STS_ERR_PRM_DATA;
    }

    EDDI_MemCopy(pPDNCDataCheckRec, pPrmWrite->record_data, pPrmWrite->record_data_length);

    if (pPDNCDataCheckRec->Header.BlockType != EDDI_PRM_BLOCKTYPE_PDNC_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.BlockType > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.BlockType, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNCDataCheckRec->Header.BlockLength != EDDI_PRM_H_BL_PDNC_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.BlockLength > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.BlockLength, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNCDataCheckRec->Header.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.BlockVersionHigh > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.BlockVersionHigh, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNCDataCheckRec->Header.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.BlockVersionLow > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.BlockVersionLow, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNCDataCheckRec->Header.Padding1_1 != 0)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.Padding1_1 > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.Padding1_1, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDNCDataCheckRec->Header.Padding1_2 != 0)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDNCDataCheck);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNCDataCheckRec->Header.Padding1_2 > ->:0x%X :0x%X",
                          pPDNCDataCheckRec->Header.Padding1_2, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pPDNC->DropBudget_B.DoCheckRequired = EDDI_HTONL(pPDNCDataCheckRec->MaintenanceRequiredDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_CHECK;
    pPDNC->DropBudget_B.DoCheckDemanded = EDDI_HTONL(pPDNCDataCheckRec->MaintenanceDemandedDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_CHECK;
    pPDNC->DropBudget_B.DoCheckError    = EDDI_HTONL(pPDNCDataCheckRec->ErrorDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_CHECK;

    pPDNC->DropBudget_B.DoCheck = pPDNC->DropBudget_B.DoCheckRequired | pPDNC->DropBudget_B.DoCheckDemanded | pPDNC->DropBudget_B.DoCheckError;

    // only check ErrorBudgetType if Enable-Bit is ON
    if (pPDNC->DropBudget_B.DoCheckError)
    {
        pPDNC->DropBudget_B.ErrorDropBudget = EDDI_HTONL(pPDNCDataCheckRec->ErrorDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_VALUE;
        if ((pPDNC->DropBudget_B.ErrorDropBudget == 0) || (pPDNC->DropBudget_B.ErrorDropBudget > 0x03E7))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ErrorDropBudget_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_ErrorDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNC->DropBudget_B.ErrorDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.ErrorDropBudget, 0);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    // only check MaintenanceDemandedDropBudget if Enable-Bit is ON
    if (pPDNC->DropBudget_B.DoCheckDemanded)
    {
        pPDNC->DropBudget_B.MaintenanceDemandedDropBudget = EDDI_HTONL(pPDNCDataCheckRec->MaintenanceDemandedDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_VALUE;
        if ((pPDNC->DropBudget_B.MaintenanceDemandedDropBudget == 0) || (pPDNC->DropBudget_B.MaintenanceDemandedDropBudget > 0x03E7))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MDDropBudget_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceDemandedDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNC->DropBudget_B.MaintenanceDemandedDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.MaintenanceDemandedDropBudget, 0);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    // only check MaintenanceRequiredDropBudget if Enable-Bit is ON
    if (pPDNC->DropBudget_B.DoCheckRequired)
    {
        pPDNC->DropBudget_B.MaintenanceRequiredDropBudget = EDDI_HTONL(pPDNCDataCheckRec->MaintenanceRequiredDropBudget.Data) & EDDI_PRM_DEF_MASK_DROP_BUDGET_VALUE;
        if ((pPDNC->DropBudget_B.MaintenanceRequiredDropBudget == 0) || (pPDNC->DropBudget_B.MaintenanceRequiredDropBudget > 0x03E7)) // -> S. 457
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MRDropBudget_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceRequiredDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNC->DropBudget_B.MaintenanceRequiredDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.MaintenanceRequiredDropBudget, 0);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDNC->DropBudget_B.DoCheckError && pPDNC->DropBudget_B.DoCheckDemanded)
    {
        if (pPDNC->DropBudget_B.MaintenanceDemandedDropBudget >= pPDNC->DropBudget_B.ErrorDropBudget)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MDgreaterError_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceDemandedDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "PrmCheckRecord, pPDNC->DropBudget_B.MaintenanceDemandedDropBudget >= pPDNC->DropBudget_B.ErrorDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.MaintenanceDemandedDropBudget, pPDNC->DropBudget_B.ErrorDropBudget);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDNC->DropBudget_B.DoCheckRequired && pPDNC->DropBudget_B.DoCheckDemanded)
    {
        if (pPDNC->DropBudget_B.MaintenanceRequiredDropBudget >= pPDNC->DropBudget_B.MaintenanceDemandedDropBudget)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MRgreaterMD_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceRequiredDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "PrmCheckRecord, pPDNC->DropBudget_B.MaintenanceRequiredDropBudget >= pPDNC->DropBudget_B.MaintenanceDemandedDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.MaintenanceRequiredDropBudget, pPDNC->DropBudget_B.MaintenanceDemandedDropBudget);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    // only if DoCheckDemanded == 0
    if (pPDNC->DropBudget_B.DoCheckRequired && pPDNC->DropBudget_B.DoCheckError)
    {
        if (pPDNC->DropBudget_B.MaintenanceRequiredDropBudget >= pPDNC->DropBudget_B.ErrorDropBudget)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MRgreaterError_PDNCDataCheck);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceRequiredDropBudget);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "PrmCheckRecord, pPDNC->DropBudget_B.MaintenanceRequiredDropBudget >= pPDNC->DropBudget_B.ErrorDropBudget > ->:0x%X :0x%X",
                              pPDNC->DropBudget_B.MaintenanceRequiredDropBudget, pPDNC->DropBudget_B.ErrorDropBudget);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    pPDNC->State_B = EDDI_PRM_WRITE_DONE;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDNCData<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdnc_data_check.c                            */
/*****************************************************************************/

