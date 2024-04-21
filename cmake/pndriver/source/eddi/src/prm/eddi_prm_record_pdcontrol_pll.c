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
/*  F i l e               &F: eddi_prm_record_pdcontrol_pll.c           :F&  */
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
#include "eddi_ext.h"
#include "eddi_prm_record_pdcontrol_pll.h"
#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDCONTROL_PLL
#define LTRC_ACT_MODUL_ID  404

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDControlPLL( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                            EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE_PTR_TYPE     pPDControlPLLRec;
    EDDI_PRM_PDCONTROL_PLL                       *  pPDPLL;
    LSA_RESULT                                     Result;
    LSA_UINT16                                     Temp16;    

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDControlPLL->");

    if (!pDDB->PRM.PDControlPLL.PDControlPllActive)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode_PDControlPll);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPrmWrite->PDControlPllActive > ->:0x%X :0x%X",
                          pDDB->PRM.PDControlPLL.PDControlPllActive, 0);
        return EDD_STS_ERR_PRM_INDEX;
    }

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDControlPll);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    if (pPrmWrite->record_data_length != EDDI_PRM_LENGTH_PD_CONTROL_PLL)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDControlPll);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPrmWrite->record_data_length > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, EDDI_PRM_LENGTH_PDSCF_DATA_MAX_CHECK);
        return EDD_STS_ERR_PRM_DATA;
    }

    pPDControlPLLRec = (EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE_PTR_TYPE)(void *)pPrmWrite->record_data;

    {
        LSA_UINT16 BlockTypeNetwork;
        LSA_UINT16 BlockLength;

        EDDI_PrmFctGetBlockTypeLength((LSA_UINT8 *)(LSA_VOID *)&pPDControlPLLRec->BlockHeader, &BlockTypeNetwork, &BlockLength);
        if (BlockTypeNetwork != EDDI_PRM_BLOCKTYPE_PD_CONTROL_PLL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDControlPll);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPDControlPLLRec->BlockHeader.BlockType > ->:0x%X",
                              BlockTypeNetwork);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if (BlockLength != EDDI_PRM_HEADER_BL_PD_CONTROL_PLL)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDControlPll);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPDControlPLLRec->BlockHeader.BlockLength > ->:0x%X",
                              BlockLength);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDControlPLLRec->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDControlPll);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPDControlPLLRec->BlockHeader.BlockVersionHigh > ->:0x%X :0x%X",
                          pPDControlPLLRec->BlockHeader.BlockVersionHigh, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDControlPLLRec->BlockHeader.BlockVersionLow != EDDI_PRM_BLOCK_VERSION_LOW)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDControlPll);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, pPDControlPLLRec->BlockHeader.BlockVersionLow > ->:0x%X :0x%X",
                          pPDControlPLLRec->BlockHeader.BlockVersionLow, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    EDDI_GET_U16Offset(pPDControlPLLRec, EDDI_PRM_RECORD_PDCONTROL_PLL_OFFSET_PLL_Mode, Temp16);
    switch (Temp16)
    {
        case EDDI_PRM_DEF_PLL_MODE_OFF:
        case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT:
        case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN:
        {
        } break;

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode_PDControlPll);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDCONTROL_PLL_OFFSET_PLL_Mode);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, PLL_Mode:0x%X", Temp16);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    pPDPLL           = &pDDB->PRM.PDControlPLL;

    pPDControlPLLRec = &pPDPLL->RecordSet_A;

    EDDI_MemCopy(pPDControlPLLRec, pPrmWrite->record_data, (LSA_UINT32) sizeof(EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE));
    //From here on, pPDControlPLLRec points to aligned values!

    Result = EDDI_XPLLSetPLLMode(EDDI_NTOHS(pPDControlPLLRec->PLL_Mode), pDDB);
    if (EDD_STS_OK != Result)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Mode_PDControlPll);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDCONTROL_PLL_OFFSET_PLL_Mode);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDControlPLL, EDDI_XPLLSetPLLMode, PLL_Mode:0x%X, Result:0x%X", pPDControlPLLRec->PLL_Mode, Result);
        return Result;
    }

    pPDPLL->State_A = EDDI_PRM_WRITE_DONE;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDControlPLL<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdcontrol_pll.c                              */
/*****************************************************************************/

