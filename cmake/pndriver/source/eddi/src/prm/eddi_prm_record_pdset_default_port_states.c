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
/*  F i l e               &F: eddi_prm_record_pdset_default_port_states.c :F&  */
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
#include "eddi_prm_record_pdset_default_port_states.h"
//#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDSET_DEFAULT_PORT_STATES
#define LTRC_ACT_MODUL_ID  409

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_PrmCheckPDSetDefaultPortStates()            */
/*                                                                         */
/* D e s c r i p t i o n: Checks the PDSetDefaultPortStates record         */
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDSetDefaultPortStates( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_RESULT                 Status;
    EDD_UPPER_MEM_U8_PTR_TYPE  pRecordData;
    LSA_UINT16                 BlockType;
    LSA_UINT16                 BlockLength;
    LSA_UINT8                  VersionHigh;
    LSA_UINT8                  VersionLow;
    LSA_UINT8                  Pad1;
    LSA_UINT8                  Pad2;
    LSA_UINT32                 i;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDSetDefaultPortStates->");

    Status      = EDD_STS_OK;
    pRecordData = pPrmWrite->record_data;
    BlockLength = 0;
    pDDB->PRM.PDSetDefaultPortStates.RecordState_B = EDDI_PRM_NOT_VALID;

    /* Set all entrys to not present*/
    for (i=0; i<EDD_CFG_MAX_PORT_CNT; i++)
    {
        pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[i].PortStatePresent = LSA_FALSE;
    }

    /*---------------------------------------------------------------------------*/
    /*  EDDI_PRM_INDEX_PDSET_DEFAULT_PORT_STATES, record_index:0x10001           */
    /*---------------------------------------------------------------------------*/
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    (                                                                      */
    /*      LSA_UINT8           PortId                                      1    */
    /*      LSA_UINT8           PortState                                   1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*    )*                                                                     */
    /*                                                                           */
    /* PortID: 1..255 (a valid PortId)                                           */
    /* each PortId must be a present Port and only appears once within record!   */
    /* Multiple PortId settings may be present within record.                    */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/

    if (pDDB->PRM.bFirstCommitExecuted) /* request only allowed before first commit */
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDSetPortStates);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Request invalid. First COMMIT already done.");
        Status = EDD_STS_ERR_PRM_INDEX; /* special case we use Index error here */
    }
    else
    {
        if (pPrmWrite->edd_port_id != 0 )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDSetPortStates);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid edd_port_id :0x%X :0x%X",
                              pPrmWrite->edd_port_id, 0);
            Status = EDD_STS_ERR_PRM_PORTID;
        }
        else
        {
            if (pPrmWrite->record_data_length < sizeof (EDDI_PRM_RECORD_HEADER_TYPE))
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDSetDefaultPortStates);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid record_data_length :0x%X :0x%X",
                                  pPrmWrite->record_data_length, sizeof (EDDI_PRM_RECORD_HEADER_TYPE));
                Status = EDD_STS_ERR_PRM_DATA;
            }
            else
            {
                LSA_UINT32  ErrOffset = EDDI_PRM_ERR_INVALID_OFFSET;

                /* --------------------------------*/
                /* Record Header                   */
                /* --------------------------------*/

                EDDI_GET_U16_INCR(pRecordData,BlockType);
                EDDI_GET_U16_INCR(pRecordData,BlockLength);
                VersionHigh = *pRecordData++;
                VersionLow  = *pRecordData++;
                Pad1 = *pRecordData++;
                Pad2 = *pRecordData++;

                if (BlockLength == (pPrmWrite->record_data_length - EDDI_PRM_BLOCK_WITHOUT_LENGTH))
                {
                    if ( BlockType   != EDDI_NTOHS(EDDI_PRM_BLOCKTYPE_PD_SET_DEFAULT_PORT_STATES))
                    {
                        ErrOffset = EDDI_PRM_BLOCKHEADER_OFFSET_BlockType;
                    }
                    else if ( Pad1 != 0)
                    {
                        ErrOffset = EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1;
                    }
                    else if ( Pad2 != 0)
                    {
                        ErrOffset = EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2;
                    }
                    else if ( VersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
                    {
                        ErrOffset = EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh;
                    }
                    else if ( VersionLow  != EDDI_PRM_BLOCK_VERSION_LOW)
                    {
                        ErrOffset = EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow;
                    }

                    if (EDDI_PRM_ERR_INVALID_OFFSET != ErrOffset)
                    {
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockHeader_PDSetDefaultPortStates);
                        EDDI_PRM_SET_ERR_OFFSET(pDDB, ErrOffset);
                        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid BlockHeader.");
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }
                else
                {

                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDSetDefaultPortStates);
                    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid BlockLength :0x%X :0x%X",
                                      BlockLength, pPrmWrite->record_data_length);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
        }
    }

    /*---------------------------------------------------------------------------*/
    /* if header ok. continue with content..                                     */
    /*---------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        LSA_UINT8  PortId;    /* User PortId 1..x */
        LSA_UINT8  PortState;

        BlockLength -= (sizeof(EDDI_PRM_RECORD_HEADER_TYPE) - EDDI_PRM_BLOCK_WITHOUT_LENGTH); /* remaining length behind paddings */

        EDDI_PRM_ADD_ERR_OFFSET(pDDB, sizeof(EDDI_PRM_RECORD_HEADER_TYPE));

        while (BlockLength >= EDDI_PRM_BLOCK_WITHOUT_LENGTH) /* PortId, PortState, Pad1, Pad2 = 4 Bytes */
        {
            PortId    = *pRecordData++;
            PortState = *pRecordData++;
            Pad1      = *pRecordData++;
            Pad2      = *pRecordData++;
            BlockLength -= EDDI_PRM_BLOCK_WITHOUT_LENGTH;

            /* check for valid PortId */
            if ((PortId > 0) && (PortId <= pDDB->PM.PortMap.PortCnt))
            {
                /* check for duplicate entry */
                if ( ! pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[PortId-1].PortStatePresent )
                {
                    /* check for valid PortState */
                    if (( PortState == EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_DISABLE ) ||
                        ( PortState == EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_BLOCKING))
                    {
                        /* all ok. store values.. */
                        pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[PortId-1].PortStatePresent = LSA_TRUE;
                        pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[PortId-1].PortState        = PortState;

                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDSetDefaultPortStates: PortID: %d, PortState: %d",PortId,PortState);
                    }
                    else
                    {
                        /* Invalid PortState! */
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortState_PDSetDefaultPortStates);
                        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_OFFSET_PortState);
                        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid PortState: 0x%X :0x%X",
                                          PortState, PortState);
                        Status = EDD_STS_ERR_PRM_BLOCK;
                    }
                }
                else
                {
                    /* PortState for PortID already set within record! */
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_MultiplePortId_PDSetDefaultPortStates);
                    EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_OFFSET_PortID);
                    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Multiple PortId: 0x%X :0x%X",
                                      PortId, PortId);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else
            {
                /* invalid PortId */
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortId_PDSetDefaultPortStates);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_OFFSET_PortID);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid PortId: 0x%X :0x%X",
                                  PortId, PortId);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
            EDDI_PRM_ADD_ERR_OFFSET(pDDB, EDDI_PRM_BLOCK_WITHOUT_LENGTH);
        }

        /* if ok check if Blocklength matches (no bytes left) */
        if ((Status == EDD_STS_OK) && (BlockLength != 0))
        {
            /* invalid BlockLength (dont matches content) */
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDSetDefaultPortStates);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSetDefaultPortStates, Invalid BlockLength:0x%X :0x%X",
                              pPrmWrite->record_data_length, BlockLength);
            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    }

    /*---------------------------------------------------------------------------*/
    /* on error clear content. if ok set WRITE_DONE.                             */
    /*---------------------------------------------------------------------------*/
    if (Status != EDD_STS_OK)
    {
        for (i=0; i<EDD_CFG_MAX_PORT_CNT; i++)
        {
            pDDB->PRM.PDSetDefaultPortStates.RecordSet_B[i].PortStatePresent = LSA_FALSE;
        }
    }
    else
    {
        pDDB->PRM.PDSetDefaultPortStates.RecordState_B = EDDI_PRM_WRITE_DONE;
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDSetDefaultPortStates<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdset_default_port_states.c                  */
/*****************************************************************************/

