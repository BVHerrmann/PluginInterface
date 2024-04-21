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
/*  F i l e               &F: eddi_prm_record_pdsync_data.c             :F&  */
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
#include "eddi_prm_record_pdsync_data.h"
#include "eddi_prm_req.h"
//#include "eddi_crt_check.h"
//#include "eddi_sync_ir.h"
//#include "eddi_swi_ext.h"
//#include "eddi_trac_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PRM_PDSYNC_DATA
#define LTRC_ACT_MODUL_ID  410

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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDSyncData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                          EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_UINT32                            i;
    EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  pPDSyncDataNetWork;
    EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  pPDSyncDataLocal;
    EDDI_LOCAL_MEM_U8_PTR_TYPE            pPDSyncData_Padding_BytePtr;

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDSyncData->");

    pDDB->PRM.PDSyncData.edd_port_id = pPrmWrite->edd_port_id;

    pPDSyncData_Padding_BytePtr      = pPrmWrite->record_data;

    if (0 != pPrmWrite->edd_port_id)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PortID_NotInterface_PDSyncData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData, pPrmWrite->edd_port_id > ->:0x%X :0x%X",
                          pPrmWrite->edd_port_id, 0);
        return EDD_STS_ERR_PRM_PORTID;
    }

    //no need for memcopy here, only access to uint8
    pPDSyncDataNetWork = (EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE)(void *)pPrmWrite->record_data;

    if (EDDI_PRM_BLOCK_VERSION_LOW_PD_SYNC_DATA != pPDSyncDataNetWork->BlockHeader.BlockVersionLow)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionLow_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid BlockVersion; -> BlockVersionLow:0x%X ExpectedBlockVersionLow:0x%X",
                          pPDSyncDataNetWork->BlockHeader.BlockVersionLow, 0x02);
        return EDD_STS_ERR_PRM_BLOCK;
    }
    else
    {
        pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount                              =  3 - (pPDSyncDataNetWork->PTCPLengthSubdomainName % 4);
        pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_PTCPSubdomainNameLength  =  pPDSyncDataNetWork->PTCPLengthSubdomainName;
        pPDSyncData_Padding_BytePtr                                                  += (pPrmWrite->record_data_length -
                                                                                         pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount);
        pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All                      =  EDDI_PRM_LENGTH_PDSYNC_DATA_FIX +
                                                                                        pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount +
                                                                                        pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_PTCPSubdomainNameLength;
    }

    if (pPrmWrite->record_data_length != pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_RecordDataLength_PDSyncData);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData, pPrmWrite->record_data_length, PDSyncDataLength_All > ->:0x%X :0x%X",
                          pPrmWrite->record_data_length, pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All);
        return EDD_STS_ERR_PRM_DATA;
    }

    {
        LSA_UINT16 BlockTypeNetwork;
        LSA_UINT16 BlockLength;

        EDDI_PrmFctGetBlockTypeLength((LSA_UINT8 *)(LSA_VOID *)&pPDSyncDataNetWork->BlockHeader, &BlockTypeNetwork, &BlockLength);
        if (BlockTypeNetwork != EDDI_PRM_BLOCKTYPE_PD_SYNC_DATA)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockType_PDSyncData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockType);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid BlockType; -> BlockType:0x%X ExpectedBlockType:0x%X",
                              BlockTypeNetwork, EDDI_PRM_BLOCKTYPE_PD_SYNC_DATA);
            return EDD_STS_ERR_PRM_BLOCK;
        }

        if ((pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All - EDDI_PRM_BLOCK_WITHOUT_LENGTH) != BlockLength)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockLength_PDSyncData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid BlockLength; -> BlockLength:0x%X ExpectedBlockLength:0x%X",
                              BlockLength, pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (pPDSyncDataNetWork->BlockHeader.BlockVersionHigh != EDDI_PRM_BLOCK_VERSION_HIGH)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_BlockVersionHigh_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid BlockVersion; -> BlockVersionHigh:0x%X ExpectedBlockVersionHigh:0x%X",
                          pPDSyncDataNetWork->BlockHeader.BlockVersionHigh, 0x01);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataNetWork->BlockHeader.Padding1_1 != 0x00)   // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_1_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid Padding1_1; -> Padding1_1:0x%X ExpectedPadding1_1:0x%X",
                          pPDSyncDataNetWork->BlockHeader.Padding1_1, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataNetWork->BlockHeader.Padding1_2 != 0x00)  // check padding for 0
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding1_2_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid Padding1_2; -> Padding1_2:0x%X ExpectedPadding1_2:0x%X",
                          pPDSyncDataNetWork->BlockHeader.Padding1_2, 0x00);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    // Copy incoming SyncDataBlock to local Buffers
    EDDI_MemCopy(pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord,
                 pPDSyncDataNetWork,
                 pDDB->PRM.PDSyncData.pRecordSet_B->PDSyncDataLength_All - pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount);

    EDDI_MemCopy(pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord->Padding_1,
                 pPDSyncData_Padding_BytePtr,
                 pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount);

    EDDI_MemCopy(pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord,
                 pDDB->PRM.PDSyncData.pRecordSet_B->pNetWorkPDSyncDataRecord,
                 sizeof(EDDI_PRM_RECORD_PDSYNC_DATA_TYPE));

    //PDSyncDataData
    //pDDB->PRM.PrmDetailErr.EDDI_PRM_PDSYNC_DATA_Record_Offset = 0;
    pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_ReservedIntervalEnd_OFFSET  = EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_ReservedIntervalEnd;
    pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_PTCP_MasterPriority1_OFFSET = EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCP_MasterPriority1;
    pDDB->PRM.PrmDetailErr.EDDI_PRM_RECORD_PDSYNC_DATA_SyncProperties_OFFSET       = EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties;

    EDDI_PRMPDSyncDataRecordNetWorkToLocal(pDDB, pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord);

    pPDSyncDataLocal = pDDB->PRM.PDSyncData.pRecordSet_B->pLocalPDSyncDataRecord;

    if (   (pPDSyncDataLocal->SendClockFactor < EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MIN)
        || (pPDSyncDataLocal->SendClockFactor > EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MAX))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SendClockFactor_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SendClockFactor);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData SendClockFactor -> SendClockFactor:0x%X 0:0x%X",
                          pPDSyncDataLocal->SendClockFactor, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataLocal->ReservedIntervalBegin != 0)
    {
        // In this version ReservedIntervalBegin can not be moved !
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReservedIntervalBegin_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_ReservedIntervalBegin);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> ReservedIntervalBegin:0x%X 0:0x%X",
                          pPDSyncDataLocal->ReservedIntervalBegin, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (   (pPDSyncDataLocal->PLLWindow == 0)
           || (pPDSyncDataLocal->PLLWindow > (pPDSyncDataLocal->SendClockFactor * (LSA_UINT32)EDDI_CRT_CYCLE_LENGTH_GRANULARITY * 5UL)))
    {
        // iSRT + IRT may not run at the same time in this version.
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PLLWindow_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PLLWindow);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PLLWindow:0x%X MaximumValue:0x%X",
                          pPDSyncDataLocal->PLLWindow, pPDSyncDataLocal->SendClockFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY * 5);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataLocal->PTPSubDomainID[0] || pPDSyncDataLocal->PTCPSubdomainName[0])    // not checked
    {
    }

    if (pPDSyncDataLocal->SyncSendFactor)  // not checked
    {
    }

    if (EDDI_GetBF16Host (pPDSyncDataLocal->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__ROLE) == SYNC_PROP_ROLE_EXTERNAL_SYNC)
    {
        switch (pPDSyncDataLocal->PTCP_MasterPriority1)
        {
            case EDDI_PRM_PDSYNCDATA_PTCP_SLAVE:
                break;

            default:
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCP_MasterPriority1_PDSyncData);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCP_MasterPriority1:0x%X 0:0x%X",
                                  pPDSyncDataLocal->PTCP_MasterPriority1, 0);
                return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (EDDI_GetBF16Host (pPDSyncDataLocal->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__ROLE) == SYNC_PROP_ROLE_CLOCK_MASTER)
    {
        switch (pPDSyncDataLocal->PTCP_MasterPriority1)
        {
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER:

            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_1:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_2:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_3:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_4:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_5:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_6:
            case EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_7:

            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_1:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_2:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_3:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_4:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_5:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_6:
            case EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_7:
                break;

            default:
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCP_MasterPriority1_PDSyncData);
                EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties);
                EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCP_MasterPriority1:0x%X 0:0x%X",
                                  pPDSyncDataLocal->PTCP_MasterPriority1, 0);
                return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    if (EDDI_PRM_PDSYNCDATA_PTCP_MASTERPRIORITY2_DEFAULT != pPDSyncDataLocal->PTCP_MasterPriority2)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCP_MasterPriority2_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCP_MasterPriority2);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCP_MasterPriority2:0x%X 0:0x%X",
                          pPDSyncDataLocal->PTCP_MasterPriority2, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataLocal->PTCPTakeoverTimeoutFactor >= EDDI_PRM_PDSYNCDATA_PTCP_TAKEOVER_TIMEOUT_FACTOR_MAX)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCP_PTCPTakeoverTimeoutFactor_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPTakeoverTimeoutFactor);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCPTakeoverTimeoutFactor:0x%X 0:0x%X",
                          pPDSyncDataLocal->PTCPTakeoverTimeoutFactor, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataLocal->PTCPTimeoutFactor > EDDI_PRM_PDSYNCDATA_PTCP_TIMEOUT_FACTOR_MAX)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCPTimeoutFactor_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPTimeoutFactor);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCPTimeoutFactor:0x%X 0:0x%X",
                          pPDSyncDataLocal->PTCPTimeoutFactor, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (pPDSyncDataLocal->PTCPMasterStartupTime > EDDI_PRM_PDSYNCDATA_PTCP_MASTER_STARTUP_TIME_MAX)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCPMasterStartupTime_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPMasterStartupTime);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCPMasterStartupTime:0x%X 0:0x%X",
                          pPDSyncDataLocal->PTCPMasterStartupTime, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (   (EDDI_GetBF16Host (pPDSyncDataLocal->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__ROLE) != SYNC_PROP_ROLE_CLOCK_MASTER )
           && (EDDI_GetBF16Host (pPDSyncDataLocal->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__ROLE) != SYNC_PROP_ROLE_EXTERNAL_SYNC))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncProperties_Role_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Role, Invalid SyncProperties -> SyncProperties.Value16:0x%X 0:0x%X",
                          pPDSyncDataLocal->SyncProperties.Value16, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (EDDI_GetBF16Host (pPDSyncDataLocal->SyncProperties.Value16, S_PRM_RECORD_PROPERTIES_BIT__SYNC_ID) > EDDI_PRM_PDSYNCDATA_SYNC_ID_MAX)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_SyncProperties_Class_PDSyncID);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData SyncClass Invalid SyncProperties, -> SyncProperties.Value16:0x%X 0:0x%X",
                          pPDSyncDataLocal->SyncProperties.Value16, 0);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    if (   (pPDSyncDataLocal->PTCPLengthSubdomainName == 0)
           || (pPDSyncDataLocal->PTCPLengthSubdomainName > EDDI_PRM_PDSYNCDATA_PTCP_SUBDOMAIN_NAME_LENGTH_MAX))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PTCPSubdomainNameLength_PDSyncData);
        EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPLengthSubdomainName);
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData -> PTCPLengthSubdomainName:0x%X 0:0x%X",
                          pPDSyncDataLocal->PTCPLengthSubdomainName, EDDI_PRM_PDSYNCDATA_PTCP_SUBDOMAIN_NAME_LENGTH_MAX);
        return EDD_STS_ERR_PRM_BLOCK;
    }

    for (i = 0; i < pDDB->PRM.PDSyncData.pRecordSet_B->PaddingCount; i++)
    {
        if (pPDSyncDataLocal->Padding_1[i] != 0x00)   // check padding for 0
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Padding_1_PDSyncData);
            EDDI_PRM_SET_ERR_OFFSET(pDDB, EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_Padding_1 + i);
            EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_PrmCheckPDSyncData Invalid Padding_1; -> Padding_1[i]:0x%X ExpectedPadding_1:0x%X",
                              pPDSyncDataLocal->Padding_1[i], 0x00);
            return EDD_STS_ERR_PRM_BLOCK;
        }
    }

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PrmCheckPDSyncData<-");

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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PRMPDSyncDataRecordNetWorkToLocal( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                                       EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  const  PDSyncDataRecord )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PRMPDSyncDataRecordNetWorkToLocal->");

    #if defined (EDDI_CFG_BIG_ENDIAN)
    if (PDSyncDataRecord)
    {
    }
    #else
    PDSyncDataRecord->BlockHeader.BlockType       = EDDI_NTOHS(PDSyncDataRecord->BlockHeader.BlockType);
    PDSyncDataRecord->BlockHeader.BlockLength     = EDDI_NTOHS(PDSyncDataRecord->BlockHeader.BlockLength);
    PDSyncDataRecord->ReservedIntervalBegin       = EDDI_NTOHL(PDSyncDataRecord->ReservedIntervalBegin);
    PDSyncDataRecord->ReservedIntervalEnd         = EDDI_NTOHL(PDSyncDataRecord->ReservedIntervalEnd);
    PDSyncDataRecord->PLLWindow                   = EDDI_NTOHL(PDSyncDataRecord->PLLWindow);
    PDSyncDataRecord->SyncSendFactor              = EDDI_NTOHL(PDSyncDataRecord->SyncSendFactor);
    PDSyncDataRecord->SendClockFactor             = EDDI_NTOHS(PDSyncDataRecord->SendClockFactor);
    PDSyncDataRecord->SyncProperties.Value16      = EDDI_NTOHS(PDSyncDataRecord->SyncProperties.Value16);
    PDSyncDataRecord->PTCPTimeoutFactor           = EDDI_NTOHS(PDSyncDataRecord->PTCPTimeoutFactor);
    PDSyncDataRecord->PTCPTakeoverTimeoutFactor   = EDDI_NTOHS(PDSyncDataRecord->PTCPTakeoverTimeoutFactor);
    PDSyncDataRecord->PTCPMasterStartupTime       = EDDI_NTOHS(PDSyncDataRecord->PTCPMasterStartupTime);
    #endif

    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_PRMPDSyncDataRecordNetWorkToLocal<-");
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_prm_record_pdsync_data.c                                */
/*****************************************************************************/

