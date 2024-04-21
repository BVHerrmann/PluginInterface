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
/*  F i l e               &F: eddi_prm_record_pdscf_data_check.c        :F&  */
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
#include "eddi_prm_record_pdscf_data_check.h"
//#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDSCF_DATA_CHECK
#define LTRC_ACT_MODUL_ID  408

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDSCFData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                         EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE  *  pPDSCFDataCheckRec;
    EDDI_PRM_PDSCF_DATA_CHECK              *  pPDSCF;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDNCData->");

    pPDSCF              = &pDDB->PRM.PDSCFDataCheck;
    pPDSCF->edd_port_id = pPrmWrite->edd_port_id;
    pPDSCFDataCheckRec  = pPDSCF->pB;

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDSCFData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (  (pPrmWrite->record_data_length < EDDI_PRM_LENGTH_PDSCF_DATA_MIN_CHECK)
        ||(pPrmWrite->record_data_length > EDDI_PRM_LENGTH_PDSCF_DATA_MAX_CHECK))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDSCFData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, EDDI_PRM_LENGTH_PDSCF_DATA_MAX_CHECK);
        return EDD_STS_ERR_PRM_DATA;
    }

    pPDSCF->RecordSize  = pPrmWrite->record_data_length;

    EDDI_MemCopy(pPDSCFDataCheckRec, pPrmWrite->record_data, pPrmWrite->record_data_length);

    if (pPDSCFDataCheckRec->BlockType != EDDI_PRM_BLOCKTYPE_PDSCF_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDSCFData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPDSCFDataCheckRec->BlockType > ->:0x%X :0x%X",
                          pPDSCFDataCheckRec->BlockType, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSCFDataCheckRec->BlockLength < EDDI_PRM_H_BL_PDSCF_DATA_CHECK)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDSCFData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPDSCFDataCheckRec->BlockLength > ->:0x%X :0x%X",
                          pPDSCFDataCheckRec->BlockLength, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSCFDataCheckRec->BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDSCFData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPDSCFDataCheckRec->BlockVersionHigh > ->:0x%X :0x%X",
                          pPDSCFDataCheckRec->BlockVersionHigh, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSCFDataCheckRec->BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDSCFData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPDSCFDataCheckRec->BlockVersionLow > ->:0x%X :0x%X",
                          pPDSCFDataCheckRec->BlockVersionLow, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    // check SendClockFactor (1 <= SCF <= 128)
    if (   (EDDI_NTOHS(pPDSCFDataCheckRec->SendClockFactor ) < EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MIN)
        || (EDDI_NTOHS(pPDSCFDataCheckRec->SendClockFactor ) > EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MAX))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SendClockFactor_PDSCFData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSCF_DATA_CHECK_OFFSET_SendClockFactor);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSCFData, pPDSCFDataCheckRec->SendClockFactor > ->:0x%X :0x%X",
                          EDDI_NTOHS(pPDSCFDataCheckRec->SendClockFactor), 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    pPDSCF->State_B = EDDI_PRM_WRITE_DONE;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDNCData<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdscf_data_check.c                           */
/*****************************************************************************/

