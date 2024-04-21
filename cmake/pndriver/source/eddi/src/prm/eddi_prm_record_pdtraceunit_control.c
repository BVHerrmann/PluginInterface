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
/*  F i l e               &F: eddi_prm_record_pdtraceunit_control.c     :F&  */
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
/*                                                                           */
/*  27.05.09    AH    initial version                                        */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_ext.h"
#include "eddi_prm_record_pdtraceunit_control.h"
#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDTRACEUNIT_CONTROL
#define LTRC_ACT_MODUL_ID  411

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDTraceUnitControl( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_UINT32                                        HwPortIndex;
    EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_PTR_TYPE     pPDTraceUnitControlRec;
    EDDI_PRM_PDTRACE_UNIT_CONTROL                  *  pPDTraceUnit;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDTraceUnitControl->");

    if (0 == pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_PDTraceUnitControl);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmWrite->record_data_length != EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDTraceUnitControl);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL);
        return EDD_STS_ERR_PRM_DATA;
    }

    pPDTraceUnitControlRec = (EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_PTR_TYPE)(void *)pPrmWrite->record_data;

    {
        LSA_UINT16 BlockTypeNetwork;
        LSA_UINT16 BlockLength;

        EDDI_PrmFctGetBlockTypeLength((LSA_UINT8 *)(LSA_VOID *)&pPDTraceUnitControlRec->BlockHeader, &BlockTypeNetwork, &BlockLength);
        if (BlockTypeNetwork != EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_CONTROL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDTraceUnitControl);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPDTraceUnitControlRec->BlockHeader.BlockType > ->:0x%X",
                              BlockTypeNetwork);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (BlockLength != EDDI_PRM_HEADER_BL_PD_TRACE_UNIT_CONTROL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDTraceUnitControl);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPDTraceUnitControlRec->BlockHeader.BlockLength > ->:0x%X",
                              BlockLength);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDTraceUnitControlRec->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDTraceUnitControl);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPDTraceUnitControlRec->BlockHeader.BlockVersionHigh > ->:0x%X :0x%X",
                          pPDTraceUnitControlRec->BlockHeader.BlockVersionHigh, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDTraceUnitControlRec->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDTraceUnitControl);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, pPDTraceUnitControlRec->BlockHeader.BlockVersionLow > ->:0x%X :0x%X",
                          pPDTraceUnitControlRec->BlockHeader.BlockVersionLow, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    switch (pDDB->ERTEC_Version.Location)
    {
        case EDDI_LOC_FPGA_XC2_V8000:
        case EDDI_LOC_ERTEC200:
        case EDDI_LOC_ERTEC400:
        case EDDI_LOC_SOC1:
        case EDDI_LOC_SOC2:
        {
            LSA_UINT32  Temp32;
            EDDI_GET_U32Offset(pPDTraceUnitControlRec, EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_OFFSET_DiagnosticEnable, Temp32);
            if (Temp32 >= EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_IP_HEADER_FRAME_LENGTH_ERR)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DiagnosticEnable_PDTraceUnitControl);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_OFFSET_DiagnosticEnable);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl, ERTEC_Version:0x%X DiagnosticEnable:0x%X",
                                  pDDB->ERTEC_Version.Location, Temp32);
                return EDD_STS_ERR_PRM_BLOCK;
            }
        }
        break;

        default:
        {
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDTraceUnitControl pDDB->ERTEC_Version.Location:0x%X", pDDB->ERTEC_Version.Location);
            EDDI_Excp("EDDI_PrmCheckPDTraceUnitControl pDDB->ERTEC_Version.Location", EDDI_FATAL_ERR_EXCP, pDDB->ERTEC_Version.Location, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    pPDTraceUnit = &pDDB->PRM.PDTraceUnitControl;
    pPDTraceUnit = &pDDB->PRM.PDTraceUnitControl;

    pPDTraceUnitControlRec = &pPDTraceUnit->RecordSet_A[pPrmWrite->edd_port_id - 1];

    EDDI_MemCopy(pPDTraceUnitControlRec, pPrmWrite->record_data, (LSA_UINT32) sizeof(EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE));

    HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, pPrmWrite->edd_port_id - 1);

    //set filter for traceunit
    EDDI_TRACEPRMSetFilterTraceunit(HwPortIndex, EDDI_NTOHL(pPDTraceUnit->RecordSet_A[pPrmWrite->edd_port_id - 1].DiagnosticEnable), pDDB);

    pPDTraceUnit->State_A[pPrmWrite->edd_port_id - 1] = EDDI_PRM_WRITE_DONE;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDTraceUnitControl<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdtraceunit_control.c                        */
/*****************************************************************************/

