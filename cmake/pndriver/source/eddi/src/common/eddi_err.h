#ifndef EDDI_ERR_H              //reinclude-protection
#define EDDI_ERR_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_err.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Version of prefix                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  21.09.07    JS    added DCPBoundary error codes                          */
/*  06.02.15    TH    refactor AdjustPortState to AdjustLinkState            */
/*                                                                           */
/*****************************************************************************/

/* Detail ERROR Codes -> rqb->err.Error */

/*** Common ***/
#define EDDI_ERR_PARA_pBuffer_NULL                       0x00000001UL
#define EDDI_ERR_INVALID_PortID                          0x00000002UL
#define EDDI_ERR_INVALID_Mode                            0x00000004UL
#define EDDI_ERR_INVALD_OPCODE                           0x00000006UL
#define EDDI_ERR_INVALD_HANDLE                           0x00000007UL
#define EDDI_ERR_pParam_NULL                             0x00000008UL
#define EDDI_ERR_SEQUENCE                                0x00000009UL
#define EDDI_ERR_Cbf_NULL                                0x0000000aUL
#define EDDI_ERR_NO_FREE_HANDLE                          0x0000000bUL
#define EDDI_ERR_PATH_INFO_PROBLEM                       0x0000000cUL
#define EDDI_ERR_PATH_INFO_pDetail_NULL                  0x0000000dUL
#define EDDI_ERR_DDB_PROBLEM                             0x0000000eUL
#define EDDI_ERR_USE_NRT_COMP                            0x0000000fUL
#define EDDI_ERR_USE_CRT_COMP                            0x00000010UL
#define EDDI_ERR_USE_SYNC_COMP                           0x00000011UL
#define EDDI_ERR_USE_SWITCH_COMP                         0x00000013UL
#define EDDI_ERR_FILTER_NOT_FREE                         0x00000014UL
#define EDDI_ERR_USE_PRM_COMP                            0x00000015UL

#define EDDI_ERR_INV_GroupID                             0x00000016UL
#define EDDI_ERR_PORT_STATUS                             0x00000017UL
#define EDDI_ERR_TIMER                                   0x00000018UL
#define EDDI_ERR_ENTRY_NOT_EXIST                         0x00000019UL
#define EDDI_ERR_FROM_FUNCTION                           0x0000001aUL
#define EDDI_ERR_INVALID_TYPE                            0x0000001bUL
#define EDDI_ERR_INVALID_PARAM                           0x0000001cUL
#define EDDI_ERR_NOT_SUPPORTED_FOR_REV5                  0x0000001dUL
#define EDDI_ERR_PORT_SPEEDMODE                          0x0000001eUL
#define EDDI_ERR_MAC_ADDRGROUP                           0x0000001fUL
#define EDDI_ERR_ID_MODEARRAY                            0x00000020UL
#define EDDI_ERR_ID_MODECNT                              0x00000021UL
#define EDDI_ERR_BITMASK_GROUP                           0x00000022UL
#define EDDI_ERR_BITMASK_PORTCNT                         0x00000023UL
#define EDDI_ERR_ADDRLOW_SIZE                            0x00000024UL
#define EDDI_ERR_WRONG_MODE                              0x00000025UL
#define EDDI_ERR_ONOFF_DURATION_DIFFER                   0x00000027UL
#define EDDI_ERR_ONOFF_DURATION_RANGE                    0x00000028UL
#define EDDI_ERR_TOTAL_DURATION_RANGE                    0x00000030UL
#define EDDI_ERR_MAC_ADDRPRIO                            0x00000031UL
#define EDDI_ERR_HSYNC_SRC_PORTID                        0x00000032UL

#define EDDI_ERR_INTERVAL_OVERLAP_IRT_ISRT               0x00000036UL
#define EDDI_ERR_TRANSCEIVER_NOT_DEFINED                 0x00000038UL
#define EDDI_ERR_PHYPOWER_NOT_DEFINED                    0x00000039UL
#define EDDI_ERR_PHY_CAPABILITIES                        0x0000003CUL

#if defined (EDDI_CFG_REV7)
#define EDDI_ERR_SWITCH_GROUP_ID                         0x0000003DUL
#define EDDI_ERR_GROUPING_NOT_SUPPORTED                  0x0000003EUL
#endif

/*** NRT   ***/
#define EDDI_ERR_RxInUse_SEE_EDD_RQB_CMP_NRT_INI_TYPE    0x00000051UL
#define EDDI_ERR_INVALID_SEND_LENGTH                     0x00000052UL
#define EDDI_ERR_TxInUse_SEE_EDD_RQB_CMP_NRT_INI_TYPE    0x00000053UL
#define EDDI_ERR_pFilterTLV_NULL                         0x00000054UL
#define EDDI_ERR_DCP_Filter_ALL                          0x00000055UL
#define EDDI_ERR_DCP_Filter_NOS                          0x00000056UL
#define EDDI_ERR_DCP_Filter_LEN                          0x00000057UL
#if defined (EDDI_CFG_FRAG_ON)
#define EDDI_ERR_FRAG_CLEANUP_ACTIVE                     0x00000058UL
#endif

#define EDDI_ERR_INVALID_Priority                        0x0000005BUL
#define EDDI_ERR_INVALID_TxFrmGroup                      0x0000005CUL
#define EDDI_ERR_INVALID_HSyncRole                       0x0000005DUL
#define EDDI_ERR_INVALID_HSyncPortID                     0x0000005EUL
#define EDDI_ERR_INVALID_HSYNC_SEND_CLOCK_FACTOR         0x0000005FUL

/*** GEN   ***/
// EDD_SRV_SET_REMOTE_PORT_STATE
#define EDDI_ERR_INVALID_TopoState                       0x00000080UL
#define EDDI_ERR_INVALID_RemoteState                     0x00000081UL
#define EDDI_ERR_INVALID_SetupPhy                        0x00000082UL

/*** SWITCH ***/
#define EDDI_ERR_SWI_TOO_MUCH_USER                       0x00000100UL

/*** CRT ***/
#define EDDI_ERR_CRT_ALREADY_IN_USE                      0x00000150UL

/*** SB    ***/
//#define EDDI_ERR_SB_INIT_NO_MEM2                       0x00000200UL
#define EDDI_ERR_SB_UNEXPECTED_EVENT_START               0x00000201UL
#define EDDI_ERR_SB_UNEXPECTED_EVENT_STOP                0x00000202UL
#define EDDI_ERR_SB_UNEXPECTED_EVENT_SB_CHANGED          0x00000203UL
#define EDDI_ERR_SB_UNEXPECTED_EVENT_NEW_IND_RESOURCE    0x00000204UL
//#define EDDI_ERR_SB_NO_NEW_IND_RESOURCE                0x00000205UL
#define EDDI_ERR_SB_UNKNOWN_EVENT                        0x00000206UL

/*** CRT   ***/
#define EDDI_ERR_CRT_FrmHandler                          0x00000300UL
#define EDDI_ERR_CRT_HandlerType                         0x00000301UL
#define EDDI_ERR_CRT_UsedByUpper                         0x00000302UL
#define EDDI_ERR_INV_ReductionRatio                      0x00000303UL
#define EDDI_ERR_INV_ListType                            0x00000304UL
#define EDDI_ERR_INV_DataOffset                          0x00000305UL
//#define EDDI_ERR_INV_ProviderCnt                       0x00000306UL
#define EDDI_ERR_INV_DataLen                             0x00000307UL
#define EDDI_ERR_DataLen_too_big                         0x00000309UL
#define EDDI_ERR_INV_CycleReductionRatio                 0x0000030aUL
#define EDDI_ERR_Bandwidth                               0x0000030bUL
#define EDDI_ERR_INV_FrameSendOffset                     0x0000030eUL
#define EDDI_ERR_INV_IRT_XRTProperties                   0x0000030dUL

#define EDDI_ERR_INV_PartialDataLen                      0x0000030eUL

#define EDDI_ERR_OverLap                                 0x0000030fUL
#define EDDI_ERR_NO_LOCAL_IP_SET                         0x00000310UL
#define EDDI_ERR_INV_EDDProperties                       0x00000311UL
#define EDDI_ERR_INV_Mode                                0x00000312UL

#define EDDI_ERR_INV_DataHoldFactor                      0x00000313UL
#define EDDI_ERR_INV_DataHoldFactor2                     0x00000314UL
#define EDDI_ERR_INV_Alignment                           0x00000315UL
//#define EDDI_ERR_INV_FrameID2                            0x00000316UL
#define EDDI_ERR_FrameID_used                            0x00000317UL

#define EDDI_ERR_INV_DMA_IN_DATA                         0x00000318UL
#define EDDI_ERR_INV_DMA_OUT_DATA                        0x00000319UL
#define EDDI_ERR_INV_DMA_ALIGN                           0x0000031aUL
#define EDDI_ERR_INV_DMA_MEMMODE                         0x0000031cUL

#define EDDI_ERR_INV_DMA_IMAGE_MODE                      0x0000031fUL

#define EDDI_ERR_INV_PartialDataOffset                   0x00000320UL

#define EDDI_ERR_INV_ProviderType                        0x00000321UL
#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
#define EDDI_ERR_INV_ConsumerType                        0x00000322UL
#endif
#define EDDI_ERR_INV_ProviderDataStatusMask              0x00000323UL
#define EDDI_ERR_INV_ForwardingMode                      0x00000324UL
#define EDDI_ERR_INV_DFPParams                           0x00000325UL
#define EDDI_ERR_INV_DFPFrameID                          0x00000326UL
#define EDDI_ERR_INV_DFPProvConsID                       0x00000327UL
#define EDDI_ERR_INV_DFPSFOffset                         0x00000328UL

#define EDDI_ERR_Cons_Prov_Entries                       0x00000334UL
#define EDDI_ERR_DMA_UserBufferSize                      0x00000335UL

#define EDDI_ERR_CycleReductionRatio_Binary              0x00000337UL
#define EDDI_ERR_CycleReductionRatio_To_Long             0x00000338UL
#define EDDI_ERR_CyclePhase_To_Long                      0x00000339UL
#define EDDI_ERR_CyclePhase_NULL                         0x0000033AUL

#define EDDI_ERR_INV_ConsumerID                          0x00000340UL
#define EDDI_ERR_INV_Unknown_Consumer_State              0x00000341UL
#define EDDI_ERR_INV_IRDATA_do_not_match                 0x00000342UL
#define EDDI_ERR_INV_ProviderID                          0x00000343UL

#if defined (EDDI_CFG_REV7)
#define EDDI_ERR_CRT_ONLY_SUPPORTED_BY_PAEA              0x00000350UL
#endif

#define EDDI_ERR_CRT_APDU_INDIRECTION                    0x00000351UL
                                                                     
#define EDDI_ERR_INV_MAC_IP                              0x00000352UL
#define EDDI_ERR_INV_IIRTtopSM                           0x00000353UL

#define EDDI_ERR_INV_CyclePhaseSequence                  0x00000354UL
#define EDDI_ERR_INV_CyclePhase                          0x00000355UL

#define EDDI_ERR_INV_Provider_Properties                 0x00000356UL
#define EDDI_ERR_INV_Consumer_Properties                 0x00000357UL

/*** SYNC   ***/
#define EDDI_ERR_INV_FrameDataLen                        0x10000001UL
//#define EDDI_ERR_INV_GlobalDataLen                       0x10000002UL
#define EDDI_ERR_INV_CyclePhase_Min                      0x10000005UL
#define EDDI_ERR_INV_CyclePhase_Max                      0x10000006UL
#define EDDI_ERR_INV_CycleLength_TriggerDeadline         0x10000007UL

// SYNC Parsing of RecordData Parameter Errors //
#define EDDI_ERR_INV_Phase                               0x11000006UL
#define EDDI_ERR_INV_FrameID                             0x11000007UL
#define EDDI_ERR_INV_DataElem_FrameID                    0x1100000fUL
#define EDDI_ERR_INV_FrameDetails                        0x1100000eUL

// SYNC Parsing of RecordData Consistency Errors //
#define EDDI_ERR_INV_RxTxPortSettings                    0x12000001UL // RxPort and TxPortGroupArray[0] mismatch.
// This may also happen if same FrameId is used in multiple FrameElements
// and we got a mismatch inbetween these FrameElements.
// e.g. a TxPort-Bit was used twice.
#define EDDI_ERR_INV_TooManyElements                     0x12000002UL // Too many IRFrameDataElements.

// SYNC SyncControl
//#define EDDI_ERR_INV_No_SyncData                       0x13000000UL
#define EDDI_ERR_INV_SyncProperties_Role                 0x13000001UL

// PRM Consistency-Check
#define EDDI_ERR_INV_MismatchExtRxPorts                  0x14000002UL // Mismatch between external RxPorts of two IrFrmDataElements
#define EDDI_ERR_INV_MismatchExtTxPorts                  0x14000003UL // Mismatch between external TxPorts of two IrFrmDataElements
#define EDDI_ERR_INV_MismatchDatalength                  0x14000004UL // Mismatch between Datalength       of two IrFrmDataElements
#define EDDI_ERR_INV_MismatchEthertype                   0x14000005UL // Mismatch between Ethertype        of two IrFrmDataElements
#define EDDI_ERR_INV_MismatchReductionRatio              0x14000006UL // Mismatch between ReductionRatio   of two IrFrmDataElements
#define EDDI_ERR_INV_MismatchPhase                       0x14000007UL // Mismatch between Phase            of two IrFrmDataElements

#define EDDI_ERR_INV_SndClkChTransitionRunning           0x15000001UL // A SendClockChange is already running and must be finished first.
#define EDDI_ERR_INV_ConsumerNotRemoved                  0x15000002UL // All Provider must be removed before SendclockChange

#define EDDI_ERR_INV_ProviderNotRemoved                  0x15000003UL // All Consumer must be removed before SendclockChange

#define EDDI_ERR_INV_PRM_Max_Data_Length                 0x15000004UL

#define EDDI_ERR_INV_ConsumerIsActive                    0x15000005UL // All Provider must be removed before SendclockChange
#define EDDI_ERR_INV_ProviderIsActive                    0x15000006UL // All Consumer must be removed before SendclockChange

#define EDDI_ERR_INV_AddToTree                           0x15000007UL // All Consumer must be removed before SendclockChange
#define EDDI_ERR_INV_ProviderMRPD_NoFrameID              0x16000000UL // No FrameID + 1 was found
#define EDDI_ERR_INV_ConsumerMRPD_NoFrameID              0x16000001UL // No FrameID + 1 was found

#define EDDI_ERR_INV_ProviderMRPD_MoreTheOneSendPort     0x16000002UL // No FrameID + 1 was found
#define EDDI_ERR_INV_ConsumerMRPD_MoreTheOneSendPort     0x16000003UL // No FrameID + 1 was found

#define EDDI_ERR_INV_ConsumerScoreboardInit              0x16000004UL // No FrameID + 1 was found
#define EDDI_ERR_INV_TooSmallCBFWithHSyncApplication     0x16000005UL // For HSyncApplicationSupport the CBF has to be at least 8

// PRM-Modul

// CloseChannel
#define EDDI_ERR_INV_PDSyncData_PDIRData_Valid                   0x16000000UL

// IndicationProvide
#define EDDI_ERR_INV_PortID_PRM_Indication                       0x16000010UL

// EDDIPrm_Read
#define EDDI_ERR_INV_WrongIndex_EDDGenPrmRead                    0x16000011UL
#define EDDI_ERR_INV_PortIDgreaterPortMapCnt_EDDGenPrmRead       0x16000012UL

// EDDIPrm_ReadPDNCDataCheck
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDNCDataCheck       0x16000020UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDNCDataCheck          0x16000021UL
#define EDDI_ERR_INV_SlotNumber_ReadPDNCDataCheck                0x16000022UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDNCDataCheck             0x16000023UL

// EDDIPrm_ReadPDSCFDataCheck
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDSCFDataCheck      0x16000030UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDSCFDataCheck         0x16000031UL
#define EDDI_ERR_INV_SlotNumber_ReadPDSCFData                    0x16000032UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDSCFData                 0x16000033UL

// EDDIPrm_ReadPDControlPLL
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDControlPll        0x16000040UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDControlPll           0x16000041UL
#define EDDI_ERR_INV_Mode_ReadPDControlPll                       0x16000042UL

// EDDIPrm_ReadPDTraceUnitControl
#define EDDI_ERR_INV_PortID_ReadPDTraceUnitControl               0x16000050UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDTraceUnitControl     0x16000051UL

// EDDIPrm_ReadPDTraceUnitData
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDTraceUnitData     0x16000060UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDTraceUnitData        0x16000061UL
#define EDDI_ERR_INV_TraceUnitInUse_ReadPDTraceUnitData          0x16000062UL

// EDDIPrm_ReadPDSyncTraceRTCData
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDSyncTraceRTCData  0x16000070UL

// EDDIPrm_ReadPDIRData
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDIRData            0x16000080UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDIRData               0x16000081UL
#define EDDI_ERR_INV_SlotNumber_ReadPDIRData                     0x16000082UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDIRData                  0x16000083UL

// EDDIPrm_ReadPDSyncData
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDSyncData          0x16000090UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDSyncData             0x16000091UL
#define EDDI_ERR_INV_SlotNumber_ReadPDSyncData                   0x16000092UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDSyncData                0x16000093UL

// EDDIPrm_ReadPDSyncData
#define EDDI_ERR_INV_PortID_ReadPDPortDataAdjust                 0x160000A0UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDPortDataAdjust       0x160000A1UL
#define EDDI_ERR_INV_SlotNumber_ReadPDPortDataAdjust             0x160000A2UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDPortDataAdjust          0x160000A3UL

// EDDI_PrmReadPDPortStatistic
#define EDDI_ERR_INV_RecordDataLength_ReadPDStatistics           0x160000B0UL

#if defined (EDDI_CFG_DFP_ON)
// EDDI_PrmReadPDIRSubframeData
#define EDDI_ERR_INV_PortID_NotInterface_ReadPDIRSubframeData    0x160000C0UL
#define EDDI_ERR_INV_RecordDataLength_ReadPDIRDataSubframeData   0x160000C1UL
#define EDDI_ERR_INV_SlotNumber_ReadPDIRDataSubframeData         0x160000C2UL
#define EDDI_ERR_INV_SubSlotNumber_ReadPDIRDataSubframeData      0x160000C3UL
#endif

// PRM StateMachine

// eddi_PrmState_Start
#define EDDI_ERR_INV_Commit_Write_End_eddiPrmStateStart           0x16000100UL
#define EDDI_ERR_INV_Default_eddiPrmStateStart                    0x16000101UL

// eddi_PrmState_Prepare
#define EDDI_ERR_INV_Commit_eddiPrmStatePrepare                   0x16000110UL
#define EDDI_ERR_INV_Default_eddiPrmStatePrepare                  0x16000111UL

// eddi_PrmState_Write
#define EDDI_ERR_INV_Commit_eddiPrmStateWrite                     0x16000120UL
#define EDDI_ERR_INV_Default_eddiPrmStateWrite                    0x16000121UL

// eddi_PrmState_End
#define EDDI_ERR_INV_Write_End_eddiPrmStateEnd                    0x16000130UL
#define EDDI_ERR_INV_Default_eddiPrmStateEnd                      0x16000131UL

// eddi_prm_check_Consistency
#define EDDI_ERR_INV_PDSCFDataCheck_SendClockFactor               0x16000140UL
#define EDDI_ERR_INV_PDSyncData_SendClockFactor                   0x16000141UL
#define EDDI_ERR_INV_PDIRDataInUse_eddiPrmCheckConsistency        0x16000142UL
#define EDDI_ERR_INV_PDIRData_PDSyncData_eddiPrmCheckConsistency  0x16000143UL
#define EDDI_ERR_INV_CntSpeedMode_Adjust_eddiPrmCheckConsistency  0x16000144UL
#define EDDI_ERR_INV_AllPowerDown_Adjust_eddiPrmCheckConsistency  0x16000145UL
#define EDDI_ERR_INV_PDSyncData_ReservedIntervalEnd               0x16000146UL
#define EDDI_ERR_INV_PDPortMrpDataAdjust                          0x16000147UL
#define EDDI_ERR_INV_PDSetDefaultPortStates                       0x16000148UL
#define EDDI_ERR_INV_PDIRSubFrameData                             0x16000149UL
#define EDDI_ERR_INV_PDirApplicationData                          0x1600014AUL

// eddi_prm_check_PDIR_PDSync_Consistency
// #define EDDI_ERR_INV_NoIrDataForSyncFrame_PDIRPDSyncConsistency  0x16000150UL
#define EDDI_ERR_INV_SyncProperties_PDIRPDSyncConsistency         0x16000151UL
#define EDDI_ERR_INV_RoleIrDataMismatch_A_PDIRPDSyncConsistency   0x16000152UL
#define EDDI_ERR_INV_RoleIrDataMismatch_B_PDIRPDSyncConsistency   0x16000153UL
#define EDDI_ERR_INV_RoleIrDataMismatchSec_PDIRPDSyncConsistency  0x16000154UL
#if defined (EDDI_CFG_ERTEC_400)
#define EDDI_ERR_INV_SyncPropertiesSec_PDIRPDSyncConsistency      0x16000155UL
#endif

// PRMCheck - Write

// PDPortDataAdjust
#define EDDI_ERR_INV_RecordDataLength_PDPortDataAdjust            0x18000000UL
#define EDDI_ERR_INV_BlockType_PDPortDataAdjust                   0x18000001UL
#define EDDI_ERR_INV_BlockLength_PDPortDataAdjust                 0x18000002UL
#define EDDI_ERR_INV_BlockVersionHigh_PDPortDataAdjust            0x18000003UL
#define EDDI_ERR_INV_BlockVersionLow_PDPortDataAdjust             0x18000004UL
#define EDDI_ERR_INV_AdjustMAUType_PDPortDataAdjust               0x18000005UL
#define EDDI_ERR_INV_AdjustSlotNumber_PDPortDataAdjust            0x18000006UL
#define EDDI_ERR_INV_AdjustSubSlotNumber_PDPortDataAdjust         0x18000007UL
#define EDDI_ERR_INV_Padding1_1_PDPortDataAdjust                  0x18000008UL
#define EDDI_ERR_INV_Padding1_2_PDPortDataAdjust                  0x18000009UL
#define EDDI_ERR_INV_TooManySubBlocks_PDPortDataAdjust            0x1800000AUL
#define EDDI_ERR_INV_SubBlockTooBig_PDPortDataAdjust              0x1800000BUL
                                                               
#define EDDI_ERR_INV_BlockLength_AdjustMulticastBoundary          0x18000010UL
#define EDDI_ERR_INV_BlockType_AdjustMulticastBoundary            0x18000011UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustMulticastBoundary     0x18000012UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustMulticastBoundary      0x18000013UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustMulticastBoundary      0x18000014UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustMulticastBoundary      0x18000015UL
#define EDDI_ERR_INV_Padding1_1_AdjustMulticastBoundary           0x18000016UL
#define EDDI_ERR_INV_Padding1_2_AdjustMulticastBoundary           0x18000017UL
#define EDDI_ERR_INV_AdjustProperties_AdjustMulticastBoundary     0x18000017UL
                                                               
#define EDDI_ERR_INV_BlockType_AdjustMAUType                      0x18000020UL
#define EDDI_ERR_INV_BlockLength_AdjustMAUType                    0x18000021UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustMAUType               0x18000022UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustMAUType                0x18000023UL
#define EDDI_ERR_INV_AdjustProperties_AdjustMAUType               0x18000024UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustMAUType                0x18000025UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustMAUType                0x18000026UL
                                                               
#define EDDI_ERR_INV_BlockType_AdjustLinkState                    0x18000030UL
#define EDDI_ERR_INV_BlockLength_AdjustLinkState                  0x18000031UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustLinkState             0x18000032UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustLinkState              0x18000033UL
#define EDDI_ERR_INV_AdjustProperties_AdjustLinkState             0x18000034UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustLinkState              0x18000035UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustLinkState              0x18000036UL
#define EDDI_ERR_INV_PortState_AdjustLinkState                    0x18000037UL
                                                               
#define EDDI_ERR_INV_BlockLength_AdjustDomainBoundary             0x18000040UL
#define EDDI_ERR_INV_BlockType_AdjustDomainBoundary               0x18000041UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustDomainBoundary        0x18000042UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustDomainBoundary         0x18000043UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustDomainBoundary         0x18000044UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustDomainBoundary         0x18000045UL
#define EDDI_ERR_INV_Padding1_1_AdjustDomainBoundary              0x18000046UL
#define EDDI_ERR_INV_Padding1_2_AdjustDomainBoundary              0x18000047UL
#define EDDI_ERR_INV_AdjustProperties_AdjustDomainBoundary        0x18000047UL
                                                               
#define EDDI_ERR_INV_BlockLength_AdjustDCPBoundary                0x18000050UL
#define EDDI_ERR_INV_BlockType_AdjustDCPBoundary                  0x18000051UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustDCPBoundary           0x18000052UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustDCPBoundary            0x18000053UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustDCPBoundary            0x18000054UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustDCPBoundary            0x18000055UL
#define EDDI_ERR_INV_Padding1_1_AdjustDCPBoundary                 0x18000056UL
#define EDDI_ERR_INV_Padding1_2_AdjustDCPBoundary                 0x18000057UL
#define EDDI_ERR_INV_AdjustProperties_AdjustDCPBoundary           0x18000057UL

#define EDDI_ERR_INV_BlockLength_AdjustPreambleLength             0x18000060UL
#define EDDI_ERR_INV_BlockType_AdjustPreambleLength               0x18000061UL
#define EDDI_ERR_INV_BlockVersionHigh_AdjustPreambleLength        0x18000062UL
#define EDDI_ERR_INV_BlockVersionLow_AdjustPreambleLength         0x18000063UL
#define EDDI_ERR_INV_BlockPadding1_1_AdjustPreambleLength         0x18000064UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustPreambleLength         0x18000065UL
#define EDDI_ERR_INV_BlockPadding1_1_ReservedPreambleLength       0x18000064UL
#define EDDI_ERR_INV_BlockPadding1_2_AdjustPropertiesPreambleLength 0x18000065UL

// PDInterfaceMrpDataAdjust
#define EDDI_ERR_INV_RecordDataLength_PDInterfaceMrpDataAdjust   0x22000000UL
#define EDDI_ERR_INV_BlockType_PDInterfaceMrpDataAdjust          0x22000001UL
#define EDDI_ERR_INV_BlockLength_PDInterfaceMrpDataAdjust        0x22000002UL
#define EDDI_ERR_INV_BlockVersionHigh_PDInterfaceMrpDataAdjust   0x22000003UL
#define EDDI_ERR_INV_BlockVersionLow_PDInterfaceMrpDataAdjust    0x22000004UL
#define EDDI_ERR_INV_Padding1_1_PDInterfaceMrpDataAdjust         0x22000008UL
#define EDDI_ERR_INV_Padding1_2_PDInterfaceMrpDataAdjust         0x22000009UL
#define EDDI_ERR_INV_MrpRole_PDInterfaceMrpDataAdjust            0x22000010UL

// PDSyncData
#define EDDI_ERR_INV_PortID_NotInterface_PDSyncData              0x19000000UL
#define EDDI_ERR_INV_RecordDataLength_PDSyncData                 0x19000001UL
#define EDDI_ERR_INV_BlockType_PDSyncData                        0x19000002UL
#define EDDI_ERR_INV_BlockLength_PDSyncData                      0x19000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDSyncData                 0x19000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDSyncData                  0x19000005UL
#define EDDI_ERR_INV_PTCP_MasterPriority1_PDSyncData             0x19000006UL
#define EDDI_ERR_INV_PTCP_MasterPriority2_PDSyncData             0x19000007UL
#define EDDI_ERR_INV_Padding1_1_PDSyncData                       0x19000008UL
#define EDDI_ERR_INV_Padding1_2_PDSyncData                       0x19000009UL
#define EDDI_ERR_INV_SendClockFactor_PDSyncData                  0x1900000AUL
#define EDDI_ERR_INV_ReservedIntervalBegin_PDSyncData            0x1900000BUL
#define EDDI_ERR_INV_PTCP_PTCPTakeoverTimeoutFactor_PDSyncData   0x1900000CUL
#define EDDI_ERR_INV_PLLWindow_PDSyncData                        0x1900000DUL
#define EDDI_ERR_INV_PTCPTimeoutFactor_PDSyncData                0x1900000FUL
#define EDDI_ERR_INV_SyncProperties_Role_PDSyncData              0x19000010UL
#define EDDI_ERR_INV_SyncProperties_Class_PDSyncID               0x19000011UL
#define EDDI_ERR_INV_PTCPSubdomainNameLength_PDSyncData          0x19000012UL
#define EDDI_ERR_INV_Padding_1_PDSyncData                        0x19000013UL
#define EDDI_ERR_INV_PTCPMasterStartupTime_PDSyncData            0x19000014UL

// PDIRData
#define EDDI_ERR_INV_PortID_NotInterface_PDIRData                0x1A000000UL
#define EDDI_ERR_INV_RecordDataLength_PDIRData                   0x1A000001UL
#define EDDI_ERR_INV_BlockType_PDIRData                          0x1A000002UL
#define EDDI_ERR_INV_BlockLength_PDIRData                        0x1A000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDIRData                   0x1A000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDIRData                    0x1A000005UL
#define EDDI_ERR_INV_SlotNumber_PDIRData                         0x1A000006UL
#define EDDI_ERR_INV_SubSlotNumber_PDIRData                      0x1A000007UL
#define EDDI_ERR_INV_Padding1_1_PDIRData                         0x1A000008UL
#define EDDI_ERR_INV_Padding1_2_PDIRData                         0x1A000009UL
#define EDDI_ERR_INV_MaxBridgeDelay_PDIRData                     0x1A00000AUL
#define EDDI_ERR_INV_PortNumber_PDIRData                         0x1A00000BUL
#define EDDI_ERR_INV_MaxPortDelayRx_PDIRData                     0x1A00000CUL
#define EDDI_ERR_INV_MaxPortDelayTx_PDIRData                     0x1A00000DUL

#define EDDI_ERR_INV_BlockType_PDIRGlobalData                    0x1A000011UL
#define EDDI_ERR_INV_BlockLength_PDIRGlobalData                  0x1A000012UL
#define EDDI_ERR_INV_BlockVersionHigh_PDIRGlobalData             0x1A000013UL
#define EDDI_ERR_INV_BlockVersionLow_PDIRGlobalData              0x1A000014UL
#define EDDI_ERR_INV_Padding1_1_PDIRGlobalData                   0x1A000015UL
#define EDDI_ERR_INV_Padding1_2_PDIRGlobalData                   0x1A000016UL

#define EDDI_ERR_INV_BlockType_PDIRFrameData                     0x1A000021UL
#define EDDI_ERR_INV_BlockLength_PDIRFrameData                   0x1A000022UL
#define EDDI_ERR_INV_BlockVersionHigh_PDIRFrameData              0x1A000023UL
#define EDDI_ERR_INV_BlockVersionLow_PDIRFrameData               0x1A000024UL
#define EDDI_ERR_INV_Padding1_1_PDIRFrameData                    0x1A000025UL
#define EDDI_ERR_INV_Padding1_2_PDIRFrameData                    0x1A000026UL
#define EDDI_ERR_INV_NumberOfElements_PDIRFrameData              0x1A000027UL

#define EDDI_ERR_INV_Ethertype_PDIRFrameData                     0x1A000030UL
#define EDDI_ERR_INV_DataLength_PDIRFrameData                    0x1A000031UL
#define EDDI_ERR_INV_FrameSendOffset_PDIRFrameData               0x1A000032UL
#define EDDI_ERR_INV_ReductionRatio_PDIRFrameData                0x1A000034UL
#define EDDI_ERR_INV_FrameID_PDIRFrameData                       0x1A000035UL
#define EDDI_ERR_INV_RxPort_PDIRFrameData                        0x1A000036UL
#define EDDI_ERR_INV_FrameDetails_SyncFrame_PDIRFrameData        0x1A000037UL
#define EDDI_ERR_INV_SyncFrameDataLength_PDIRFrameData           0x1A000038UL
#define EDDI_ERR_INV_FrameDetails_FrameOffset_PDIRFrameData      0x1A000039UL
#define EDDI_ERR_INV_NumberOfTxPortGroups_PDIRFrameData          0x1A00003AUL
#define EDDI_ERR_INV_TxPortGroupArray_PDIRFrameData              0x1A00003BUL
#define EDDI_ERR_INV_FrameProperties_Fragmentation                0x1A00003CUL

#if defined (EDDI_CFG_REV5)
#define EDDI_ERR_INV_DataLength_REV5_PDIRFrameData               0x1A00003CUL
#elif defined (EDDI_CFG_REV6)
#define EDDI_ERR_INV_DataLength_REV6_PDIRFrameData               0x1A00003DUL
#endif

#define EDDI_ERR_INV_BlockType_PDIRBeginEndData                  0x1A000041UL

// PDSCFData
#define EDDI_ERR_INV_PortID_NotInterface_PDSCFData               0x1B000000UL
#define EDDI_ERR_INV_RecordDataLength_PDSCFData                  0x1B000001UL
#define EDDI_ERR_INV_BlockType_PDSCFData                         0x1B000002UL
#define EDDI_ERR_INV_BlockLength_PDSCFData                       0x1B000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDSCFData                  0x1B000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDSCFData                   0x1B000005UL
#define EDDI_ERR_INV_SendClockFactor_PDSCFData                   0x1B000006UL

// PDTraceUnitControl
#define EDDI_ERR_INV_PortID_PDTraceUnitControl                   0x1E000000UL
#define EDDI_ERR_INV_RecordDataLength_PDTraceUnitControl         0x1E000001UL
#define EDDI_ERR_INV_BlockType_PDTraceUnitControl                0x1E000002UL
#define EDDI_ERR_INV_BlockLength_PDTraceUnitControl              0x1E000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDTraceUnitControl         0x1E000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDTraceUnitControl          0x1E000005UL
#define EDDI_ERR_INV_DiagnosticEnable_PDTraceUnitControl         0x1E000006UL

// PDControlPLL
#define EDDI_ERR_INV_PortID_NotInterface_PDControlPll            0x1F000000UL
#define EDDI_ERR_INV_RecordDataLength_PDControlPll               0x1F000001UL
#define EDDI_ERR_INV_BlockType_PDControlPll                      0x1F000002UL
#define EDDI_ERR_INV_BlockLength_PDControlPll                    0x1F000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDControlPll               0x1F000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDControlPll                0x1F000005UL
#define EDDI_ERR_INV_Mode_PDControlPll                           0x1F000006UL

// PDNCDataCheck
#define EDDI_ERR_INV_PortID_NotInterface_PDNCDataCheck           0x20000000UL
#define EDDI_ERR_INV_RecordDataLength_PDNCDataCheck              0x20000001UL
#define EDDI_ERR_INV_BlockType_PDNCDataCheck                     0x20000002UL
#define EDDI_ERR_INV_BlockLength_PDNCDataCheck                   0x20000003UL
#define EDDI_ERR_INV_BlockVersionHigh_PDNCDataCheck              0x20000004UL
#define EDDI_ERR_INV_BlockVersionLow_PDNCDataCheck               0x20000005UL
#define EDDI_ERR_INV_Padding1_1_PDNCDataCheck                    0x20000006UL
#define EDDI_ERR_INV_Padding1_2_PDNCDataCheck                    0x20000007UL
#define EDDI_ERR_INV_ErrorDropBudget_PDNCDataCheck               0x20000008UL
#define EDDI_ERR_INV_MDDropBudget_PDNCDataCheck                  0x20000009UL
#define EDDI_ERR_INV_MRDropBudget_PDNCDataCheck                  0x2000000AUL
#define EDDI_ERR_INV_MDgreaterError_PDNCDataCheck                0x2000000BUL
#define EDDI_ERR_INV_MRgreaterError_PDNCDataCheck                0x2000000CUL
#define EDDI_ERR_INV_MRgreaterMD_PDNCDataCheck                   0x2000000DUL

// PDSetDefaultPortStates
#define EDDI_ERR_INV_PortID_NotInterface_PDSetPortStates         0x21000000UL
#define EDDI_ERR_INV_RecordDataLength_PDSetDefaultPortStates     0x21000001UL
#define EDDI_ERR_INV_BlockHeader_PDSetDefaultPortStates          0x21000002UL
#define EDDI_ERR_INV_BlockLength_PDSetDefaultPortStates          0x21000003UL
#define EDDI_ERR_INV_PortState_PDSetDefaultPortStates            0x21000004UL
#define EDDI_ERR_INV_MultiplePortId_PDSetDefaultPortStates       0x21000005UL
#define EDDI_ERR_INV_PortId_PDSetDefaultPortStates               0x21000006UL

// PDirApplicationData
#define EDDI_ERR_INV_PortID_NotInterface_PDirApplicationData     0x22000000UL
#define EDDI_ERR_INV_RecordDataLength_PDirApplicationData        0x22000001UL
#define EDDI_ERR_INV_BlockType_PDirApplicationData               0x22000002UL
#define EDDI_ERR_INV_BlockHeader_PDirApplicationData             0x22000003UL
#define EDDI_ERR_INV_BlockLength_PDirApplicationData             0x22000004UL

// PDNRTFeedInLoadLimitation
#define EDDI_ERR_INV_PortID_NotInterface_PDNRTFeedInLoadLimitation  0x23000000UL
#define EDDI_ERR_INV_RecordDataLength_PDNRTFeedInLoadLimitation     0x23000001UL
#define EDDI_ERR_INV_BlockType_PDNRTFeedInLoadLimitation            0x23000002UL
#define EDDI_ERR_INV_BlockHeader_PDNRTFeedInLoadLimitation          0x23000003UL
#define EDDI_ERR_INV_BlockLength_PDNRTFeedInLoadLimitation          0x23000004UL
#define EDDI_ERR_INV_BlockVersionHigh_PDNRTFeedInLoadLimitation     0x23000005UL
#define EDDI_ERR_INV_BlockVersionLow_PDNRTFeedInLoadLimitation      0x23000006UL
#define EDDI_ERR_INV_Padding1_1_PDNRTFeedInLoadLimitation           0x23000007UL
#define EDDI_ERR_INV_Padding1_2_PDNRTFeedInLoadLimitation           0x23000008UL
#define EDDI_ERR_INV_LoadLimitationActive_PDNRTFeedInLoadLimitation 0x23000009UL
#define EDDI_ERR_INV_IO_Configured_PDNRTFeedInLoadLimitation        0x2300000AUL
#define EDDI_ERR_INV_PRM_BLOCK_INVALID_PDNRTFeedInLoadLimitation    0x2300000BUL
  

#if defined (EDDI_CFG_REV5)
#define EDDI_ERR_NRT_TS_TIMEOUT                                  0x18000000UL
#define EDDI_ERR_NRT_TS_LINK_DOWN                                0x18000001UL
#define EDDI_ERR_NRT_TS_STATE                                    0x18000002UL
#define EDDI_ERR_NRT_TS_INTERFACE                                0x18000003UL
#endif
#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define EDDI_ERR_NRT_TS_MIRROR_MODE                              0x18000004UL
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE
#if defined (EDDI_ERR_INV_PDPortMrpDataAdjust) || defined (EDDI_ERR_INV_FrameTimeOutofBounds_PDIRFrameData) //satisfy lint!
#endif

#if defined (EDDI_CFG_REV5)
#if defined (EDDI_ERR_NRT_TS_TIMEOUT) || defined (EDDI_ERR_NRT_TS_LINK_DOWN) || defined (EDDI_ERR_NRT_TS_STATE) //satisfy lint!
#endif
#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_ERR_H


/*****************************************************************************/
/*  end of file eddi_err.h                                                   */
/*****************************************************************************/
