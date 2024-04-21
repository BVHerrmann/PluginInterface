#ifndef CM_PDU_H
#define CM_PDU_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_pdu.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile: PDU definitions                                     */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by cm_int.h
 */

/*------------------------------------------------------------------------------
// PDU UUID
//----------------------------------------------------------------------------*/

#define CM_PDU_UUID_SIZE (16U)

/*------------------------------------------------------------------------------
// PDU MAC
//----------------------------------------------------------------------------*/

#define CM_PDU_MAC_SIZE (6U)

/*------------------------------------------------------------------------------
// PDU Constants
//----------------------------------------------------------------------------*/

#define CM_RECORD_INDEX_RPC_TRIGGER     0xFBFF /* Trigger index for the RPC connection monitoring */


enum cm_block_type_enum {
	CM_BLOCK_TYPE_NONE				= 0x0000,

	CM_BLOCK_TYPE_ALARM_HIGH		= 0x0001, /* AlarmNotification High, USAGE: AlarmNotification-PDU */
	CM_BLOCK_TYPE_ALARM_LOW			= 0x0002, /* AlarmNotification Low, USAGE: AlarmNotification-PDU */
	CM_BLOCK_TYPE_ALARM_ACK_HIGH	= 0x8001, /* Alarm Ack High, USAGE: AlarmAck-PDU */
	CM_BLOCK_TYPE_ALARM_ACK_LOW		= 0x8002, /* Alarm Ack Low, USAGE: AlarmAck-PDU */

	CM_BLOCK_TYPE_WRITE_REQ			= 0x0008, /* WriteRecordReq, USAGE: RecordDataWrite */
	CM_BLOCK_TYPE_READ_REQ			= 0x0009, /* ReadRecordReq, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_WRITE_RSP			= 0x8008, /* WriteRecordRes, USAGE: RecordDataWrite */
	CM_BLOCK_TYPE_READ_RSP			= 0x8009, /* ReadRecordRes, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_DIAGNOSIS			= 0x0010, /* DiagnosisBlock, USAGE: RecordDataRead, AlarmNotification-PDU */

	CM_BLOCK_TYPE_EXP_IDENT			= 0x0012, /* ExpectedIdentificationDataBlock, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_REAL_IDENT		= 0x0013, /* RealIdentificationData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_SUBST_VALUE		= 0x0014, /* SubstituteValue, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_IODATA_INP		= 0x0015, /* RecordInputDataObjectElement, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_IODATA_OUT		= 0x0016, /* RecordOutputDataObjectElement, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_AR_DATA			= 0x0018, /* ARData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_LOG_DATA			= 0x0019, /* LogBookData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_API_DATA			= 0x001A, /* APIData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_SRL_DATA			= 0x001B, /* SRLData, USAGE: RecordDataRead.ARDataInfo.SRLData */
	CM_BLOCK_TYPE_I_M_0				= 0x0020, /* I&M0, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_I_M_1				= 0x0021, /* I&M1, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_2				= 0x0022, /* I&M2, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_3				= 0x0023, /* I&M3, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_4				= 0x0024, /* I&M4, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_5				= 0x0025, /* I&M5, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_I_M0FilterDataSub	= 0x0030, /* I&M0FilterDataSubmodule, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M0FilterDataMod	= 0x0031, /* I&M0FilterDataModule, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M0FilterDataDev	= 0x0032, /* I&M0FilterDataDevice, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M_5_Data		= 0x0034, /* I&M5Data, USAGE: RecordDataRead*/

	CM_BLOCK_TYPE_AR_REQ			= 0x0101, /* ARBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_IOCR_REQ			= 0x0102, /* IOCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ALCR_REQ			= 0x0103, /* AlarmCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_EXPSUBMOD_REQ		= 0x0104, /* ExpectedSubmoduleBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_PRMSRV_REQ		= 0x0105, /* PrmServerBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_MCR_REQ			= 0x0106, /* MCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARRPCBLOCK_REQ	= 0x0107, /* ARRPCBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARVENDORBLOCK_REQ	= 0x0108, /* ArVendorBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_IRINFOBLOCK		= 0x0109, /* IRInfoBlock, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_SRINFOBLOCK		= 0x010A, /* SRInfoBlock, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARFSUBLOCK		= 0x010B, /* ARFSUBlock, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_RSINFOBLOCK		= 0x010C, /* RSInfoBlock, USAGE: IODConnectReq */

	CM_BLOCK_TYPE_AR_RSP			= 0x8101, /* ARBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_IOCR_RSP			= 0x8102, /* IOCRBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ALCR_RSP			= 0x8103, /* AlarmCRBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_MOD_DIFF			= 0x8104, /* ModuleDiffBlock, USAGE: IODConnectRes, RecordDataRead,IOCControlReq,IOSControlReq */
	CM_BLOCK_TYPE_PRMSRV_RSP		= 0x8105, /* PrmServerBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARSERVERBLOCK		= 0x8106, /* ARServerBlock, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARRPCBLOCK_RSP	= 0x8107, /* ARRPCBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARVENDORBLOCK_RSP	= 0x8108, /* ArVendorBlockRes, USAGE: IODConnectRes */

	CM_BLOCK_TYPE_IODBLOCK_CONN_REQ	= 0x0110, /* IODBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IODControlReq(Prm End.req) */
	CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ	= 0x0111, /* IODBlockReq, shall only be used in conjunction with a plug alarm event, USAGE: IODControlReq(Prm End.req) */

	CM_BLOCK_TYPE_IODBLOCK_CONN_RSP	= 0x8110, /* IODBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IODControlRes(Prm End.res) */
	CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP	= 0x8111, /* IODBlockRes, shall only be used in conjunction with a plug alarm event, USAGE: IODControlRes(Prm End.res) */

	CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ	= 0x0112, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Application Ready.req) */
	CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ	= 0x0113, /* IOXBlockReq, shall only be used in conjunction with a plug alarm event, USAGE: IOCControlReq, IOSControlReq(Application Ready.req) */

	CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP	= 0x8112, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Application Ready.res) */
	CM_BLOCK_TYPE_IOXBLOCK_PLUG_RSP	= 0x8113, /* IOXBlockRes, shall only be used in conjunction with a plug alarm event, USAGE: IOCControlRes IOSControlRes(Application Ready.res) */

	CM_BLOCK_TYPE_RELBLOCK_REQ		= 0x0114, /* ReleaseBlockReq, USAGE: IODReleaseReq */
	CM_BLOCK_TYPE_RELBLOCK_RSP		= 0x8114, /* ReleaseBlockRes, USAGE: IODReleaseRes */

#if 0/*reserved for future use*/
	CM_BLOCK_TYPE_IOXBLOCK_R4COMP_REQ = 0x0116, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Ready for Companion.req) */
	CM_BLOCK_TYPE_IOXBLOCK_R4COMP_RSP = 0x8116, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Ready for Companion.res) */
#endif

	CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ = 0x0117, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Ready for RTClass3.req) */
	CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_RSP = 0x8117, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Ready for RTClass3.res) */

	CM_BLOCK_TYPE_PRM_BEGIN_REQ       = 0x0118, /* IODBlockReq, USAGE: IODControlReq(Prm Begin.req) */
	CM_BLOCK_TYPE_PRM_BEGIN_RSP       = 0x8118, /* IODBlockRes, USAGE: IODControlRes(Prm Begin.rsp) */

	CM_BLOCK_TYPE_SUBMODULE_LIST_REQ  = 0x0119, /* SubmoduleListBlock, USAGE: IODControlReq(Prm Begin.req) */

	CM_BLOCK_TYPE_PD_PORT_DATA_CHECK        = 0x0200, /* PDPortDataCheck,  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_PDEVDATA               = 0x0201, /* PDevData,         USAGE: RecordDataRead */
	CM_BLOCK_TYPE_PD_PORT_DATA_ADJUST       = 0x0202, /* PDPortDataAdjust  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_SYNC_DATA              = 0x0203, /* PDSyncData,       USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_ISOCHRONOUS               = 0x0204, /* Isochr.ModeData,  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_DATA                = 0x0205, /* PDIRData,         USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA         = 0x0206, /* PDIRGlobalData,   USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_FRAME_DATA          = 0x0207, /* PDIRFrameData,    USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA      = 0x0208, /* PDIRBeginEndData, USAGE: RecordDataWrite/Read */

	CM_BLOCK_TYPE_PD_ADJ_DOMAIN_BOUNDARY    = 0x0209, /* adjusting DomainBoundary, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_CHK_PEERS              = 0x020A, /* checking Peers, PDPortDataCheck */
	CM_BLOCK_TYPE_PD_CHK_LINE_DELAY         = 0x020B, /* checking LineDelayFactor, PDPortDataCheck */
	CM_BLOCK_TYPE_PD_CHK_MAUTYPE            = 0x020C, /* checking MAUType, PDPortDataCheck */

	CM_BLOCK_TYPE_PD_ADJ_MAUTYPE            = 0x020E, /* adjusting MAUType, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_PORT_DATA_REAL         = 0x020F, /* PDPortDataReal, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_PD_ADJ_MULTICAST_BOUNDARY = 0x0210, /* adjusting MulticastBoundary, PDPortDataAdjust */

	CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ      = 0x0211, /* adjusting MRP_Role,      PDInterfaceMrpDataAdjust */
	CM_BLOCK_TYPE_PD_MRP_INTERFACE_CHECK	= 0x0213, /* PDInterfaceMrpDataCheck, USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_MRP_PORT_ADJ			= 0x0214, /* PDPortMrpDataAdjust,     USAGE: RecordDataWrite/Read */

	CM_BLOCK_TYPE_PD_ADJ_LINK_STATE         = 0x021B, /* adjusting LinkState, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_CHK_LINK_STATE         = 0x021C, /* checking LinkState, PDPortDataCheck */

	CM_BLOCK_TYPE_PD_CHK_SYNC_DIFFERENCE	= 0x021E, /* checking SyncDifference, PDPortDataCheck */
	CM_BLOCK_TYPE_PD_CHK_MAUTYPE_DIFFERENCE = 0x021F, /* checking MAUTypeDifference, PDPortDataCheck */

	CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH	= 0x0226, /* adjusting PreambleLength, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_CHK_MAUTYPE_EXTENSION	= 0x0227, /* checking MAUTypeExtension, PDPortDataCheck */

	CM_BLOCK_TYPE_MULTIPLE_BLOCK_HEADER = 0x0400, /* Sub block for MultipleBlockHeader, RecordDataRead */
	CM_BLOCK_TYPE_COCONTAINERCONTENT    = 0x0401, /* CombinedObjectContainer.COContainerContent, RecordDataWrite */

	CM_BLOCK_TYPE_READQUERY             = 0x0500, /* RecordDataReadQuery */

	CM_BLOCK_TYPE_FSHELLOBLOCK          = 0x0600, /* FSHelloBlock, PDInterfaceFSUDataAdjust */
	CM_BLOCK_TYPE_FSPARAMETERBLOCK      = 0x0601, /* FSParameterBlock, ARFSUDataAdjust */
	CM_BLOCK_TYPE_FSUDATAADJUST         = 0x0608, /* PDInterfaceFSUDataAdjust, RecordDataRead RecordDataWrite */
	CM_BLOCK_TYPE_ARFSUDATAADJUST       = 0x0609, /* ARFSUDataAdjust, RecordDataRead RecordDataWrite */

	CM_BLOCK_TYPE_MAINTENANCE           = 0x0F00, /* PROFInet IO - Maintenance Integration.ppt */

	CM_BLOCK_TYPE_PD_SYNC_PLL			= 0x7000, /* PDSyncPLL (Index 0x00011020) */
	CM_BLOCK_TYPE_SEND_CLOCK            = 0xF000, /* SendClock (Index 0x00010000) */

	CM_BLOCK_TYPE_RESERVED              = 0xFFFF
};

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

/*
 * defines CM_AR_TYPE_SINGLE, ... see cm_arcb.h
 */

/*
 * defines CM_IOCR_TYPE_RESERVED_0, ... see cm_arcb.h
 */

/*
 * defines CM_ALCR_TYPE_RESERVED_0, ... see cm_arcb.h
 */

/*------------------------------------------------------------------------------
// BlockHeader ::=
//   BlockType, BlockLength, BlockVersionHigh, BlockVersionLow
//----------------------------------------------------------------------------*/

#define CM_BLK_HEADER_type		0 /* 0 [u16] BlockType; see cm_block_type_enum */
#define CM_BLK_HEADER_length	2 /* 1 [u16] BlockLength; block-length without 'type' and 'length' */
#define CM_BLK_HEADER_version	4 /* 2 [u16] BlockVersionHigh, BlockVersionLow; high=major, low=minor */

#define CM_BLK_HEADER_SIZE			(6U)
#define CM_BLK_HEADER_TypLen_SIZE	(4U)


/*------------------------------------------------------------------------------
// ARBlockReq ::=
//   BlockHeader,
//   ARType, ARUUID, SessionKey,
//   CMInitiatorMacAdd, CMInitiatorObjectUUID,
//   ARProperties,
//   CMInitiatorActivityTimeout, UDPCMInitiatorPort,
//   StationNameLength, CMInitiatorStationName
//----------------------------------------------------------------------------*/

#define CM_REQ_AR_BlockHeader				0  /*  0 BlockHeader */
#define CM_REQ_AR_ar_type					6  /*  4 [u16] ARType; see defines CM_AR_TYPE_SINGLE, ... */
#define CM_REQ_AR_ar_uuid					8  /*  5 [uid] ARUUID */
#define CM_REQ_AR_session_key				24 /*  6 [u16] SessionKey */
#define CM_REQ_AR_cmi_mac					26 /*  7 [mac] CMInitiatorMacAdd */
#define CM_REQ_AR_cmi_obj_uuid				32 /*  8 [uid] CMInitiatorObjectUUID */
#define CM_REQ_AR_ar_properties				48 /*  9 [u32] ARProperties; see defines CM_AR_PROP_STATE_MASK, ... */
#define CM_REQ_AR_cmi_activity_timeout100ms	52 /* 10 [u16] CMInitiatorActivityTimeout */
#define CM_REQ_AR_cmi_udp_port				54 /* 11 [u16] InitiatorUDPRTPort */
#define CM_REQ_AR_cmi_name_length			56 /* 12 [u16] StationNameLength */

#define CM_REQ_AR_SIZE						(58U) /* size including BlockHeader, excluding CMInitiatorStationName */


/*------------------------------------------------------------------------------
// ARBlockRes ::=
//   BlockHeader,
//   ARType, ARUUID, SessionKey, CMResponderMacAdd, UDPCMResponderPort
//----------------------------------------------------------------------------*/

#define CM_RSP_AR_BlockHeader	0  /* 0 BlockHeader */
#define CM_RSP_AR_ar_type		6  /* 4 [u16] ARType; see defines CM_AR_TYPE_SINGLE, ... */
#define CM_RSP_AR_ar_uuid		8  /* 5 [uid] ARUUID */
#define CM_RSP_AR_session_key	24 /* 6 [u16] SessionKey */
#define CM_RSP_AR_cmr_mac		26 /* 7 [mac] CMResponderMacAdd */
#define CM_RSP_AR_cmr_udp_port	32 /* 8 [u16] ResponderUDPRTPort */

#define CM_RSP_AR_SIZE			(34U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// IOCRBlockReq ::=
//   BlockHeader,
//   IOCRType, IOCRReference, LT, IOCRProperties,
//   DataLength, FrameID,
//   SendClock, ReductionRatio, Phase, Sequence, FrameSendOffset,
//   WatchdogFactorLegacy, DataHoldFactor,
//   IOCRTagHeader, IOCRMulticastMACAdd,
//   NumberOfRelatedAPIs,
//   ( API,
//     NumberOfRelatedIODataObjects,
//     ( Slotnumber, SubslotNumber, IODataObjectFrameOffset
//     )*,
//     NumberOfRelatedIOCS,
//     ( Slotnumber, SubslotNumber, IOCSFrameOffset
//     )*
//   )*
//----------------------------------------------------------------------------*/

#define CM_REQ_IOCR1_BlockHeader			0  /*  0 BlockHeader */
#define CM_REQ_IOCR1_iocr_type				6  /*  4 [u16] IOCRType; see defines CM_IOCR_TYPE_RESERVED_0, ... */
#define CM_REQ_IOCR1_iocr_ref				8  /*  5 [u16] IOCRReference */
#define CM_REQ_IOCR1_ethertype				10 /*  6 [u16] LT */
#define CM_REQ_IOCR1_iocr_properties		12 /*  7 [u32] IOCRProperties; see defines CM_IOCR_PROP_RT_CLASS_MASK, ... */
#define CM_REQ_IOCR1_data_length			16 /*  8 [u16] DataLength; >= edd.min_frame_size && <= edd.max_frame_size */
#define CM_REQ_IOCR1_frame_id				18 /*  9 [u16] FrameID */
#define CM_REQ_IOCR1_send_clock				20 /* 10 [u16] SendClock */
#define CM_REQ_IOCR1_reduction_ratio		22 /* 11 [u16] ReductionRatio */
#define CM_REQ_IOCR1_phase					24 /* 12 [u16] Phase */
#define CM_REQ_IOCR1_sequence				26 /* 13 [u16] Sequence */
#define CM_REQ_IOCR1_frame_send_offset		28 /* 14 [u32] FrameSendOffset; timebase 1ns */
#define CM_REQ_IOCR1_watchdog_factor_legacy	32 /* 15 [u16] WatchdogFactorLegacy; legacy, must be the same as data_hold_factor (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12) */
#define CM_REQ_IOCR1_data_hold_factor		34 /* 16 [u16] DataHoldFactor */
#define CM_REQ_IOCR1_iocr_tag_header		36 /* 17 [u16] IOCRTagHeader */
#define CM_REQ_IOCR1_iocr_multicast_mac		38 /* 18 [mac] IOCRMulticastMACAdd */
#define CM_REQ_IOCR1_number_of_related_apis	44 /* 19 [u16] NumberOfRelatedAPIs */

#define CM_REQ_IOCR1_SIZE					(46U) /* size including BlockHeader */

/* part 2 follows */

#define CM_REQ_IOCR2_api								0 /* [u32] API */
#define CM_REQ_IOCR2_number_of_related_iodata_objects	4 /* [u16] NumberOfRelatedIODataObjects */

#define CM_REQ_IOCR2_SIZE								(6U)

/* part 3 follows (related io data object) */

#define CM_REQ_IOCR3_slot_nr		0 /* [u16] Slotnumber */
#define CM_REQ_IOCR3_subslot_nr		2 /* [u16] SubslotNumber */
#define CM_REQ_IOCR3_frame_offset	4 /* [u16] IODataObjectFrameOffset */

#define CM_REQ_IOCR3_SIZE			(6U)

/* part 4 follows */

#define CM_REQ_IOCR4_number_of_related_iocs	0 /* [u16] NumberOfRelatedIOCS */

#define CM_REQ_IOCR4_SIZE					(2U)

/* part 5 follows (related iocs) */

#define CM_REQ_IOCR5_slot_nr		0 /* [u16] Slotnumber */
#define CM_REQ_IOCR5_subslot_nr		2 /* [u16] SubslotNumber */
#define CM_REQ_IOCR5_frame_offset	4 /* [u16] IOCSFrameOffset */

#define CM_REQ_IOCR5_SIZE			(6U)


/*------------------------------------------------------------------------------
// IOCRBlockRes ::=
//   BlockHeader, IOCRType, IOCRReference, FrameID
//----------------------------------------------------------------------------*/

#define CM_RSP_IOCR_BlockHeader	0  /* 0 BlockHeader */
#define CM_RSP_IOCR_iocr_type	6  /* 4 [u16] IOCRType; see defines CM_IOCR_TYPE_RESERVED_0, ... */
#define CM_RSP_IOCR_iocr_ref	8  /* 5 [u16] IOCRReference */
#define CM_RSP_IOCR_frame_id	10 /* 6 [u16] FrameID */

#define CM_RSP_IOCR_SIZE		(12U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// MCRBlockReq ::=
//   BlockHeader,
//   IOCRReference, AddressResolutionProperties,
//   MCITimeoutFactor, StationNameLength, ProviderStationName, [Padding]*
//----------------------------------------------------------------------------*/

#define CM_REQ_MCR_BlockHeader			0  /* 0 BlockHeader */
#define CM_REQ_MCR_iocr_ref				6  /* 4 [u16] IOCRReference */
#define CM_REQ_MCR_mcr_properties		8  /* 5 [u32] AddressResolutionProperties */
#define CM_REQ_MCR_mci_timeout_factor	12 /* 6 [u16] MCITimeoutFactor */
#define CM_REQ_MCR_station_name_length	14 /* 7 [u16] StationNameLength */

#define CM_REQ_MCR_SIZE					(16U) /* size including BlockHeader, excluding ProviderStationName/Padding */


/*------------------------------------------------------------------------------
// IRInfoBlock ::=
//   BlockHeader, Padding* a, IRDataUUID, Padding* a,
//     NumberOfIOCRs b, [(IOCRReference, SubframeOffset, SubframeData)*], [Padding*] c
//
// a The number of Padding octets shall be 2.
// b This field shall be coded as zero if no DFP is used.
// c The number of padding octets shall be adapted to make the block Unsigned32 aligned.
//----------------------------------------------------------------------------*/

#define CM_REQ_IRINFO_BlockHeader		0  /* BlockHeader */
#define CM_REQ_IRINFO_padding1			6  /* [u16] Padding; 2 bytes */
#define CM_REQ_IRINFO_ir_data_uuid		8  /* [uid] IRDataUUID */
#define CM_REQ_IRINFO_padding2			24 /* [u16] Padding; 2 bytes */
#define CM_REQ_IRINFO_number_of_iocrs	26 /* [u16] NumberOfIOCRs; This field shall be coded as zero if no DFP is used */

#define CM_REQ_IRINFO_SIZE				(28U) /* size including BlockHeader */


#define CM_REQ_IRINFOSUB_iocr_reference		0 /* [u16] IOCRReference */
#define CM_REQ_IRINFOSUB_subframe_offset	2 /* [u16] SubframeOffset */
#define CM_REQ_IRINFOSUB_subframe_data		4 /* [u32] SubframeData */

#define CM_REQ_IRINFOSUB_SIZE				(8U)


/*------------------------------------------------------------------------------
// SRInfoBlock ::=
//   BlockHeader, RedundancyDataHoldFactor, SRProperties
//----------------------------------------------------------------------------*/

#define CM_REQ_SRINFO_BlockHeader				0 /* BlockHeader */
#define CM_REQ_SRINFO_RedundancyDataHoldFactor	6 /* [u16] RedundancyDataHoldFactor */
#define CM_REQ_SRINFO_SRProperties				8 /* [u32] SRProperties */

#define CM_REQ_SRINFO_SIZE						(12U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// RSInfoBlock ::=
//   BlockHeader, Padding, RSProperties
//----------------------------------------------------------------------------*/

#define CM_REQ_RSINFO_BlockType			0 /* #0 BlockType */
#define CM_REQ_RSINFO_BlockLength		2 /* #1 BlockLength */
#define CM_REQ_RSINFO_BlockVersionHigh	4 /* #2 BlockVersionHigh */
#define CM_REQ_RSINFO_BlockVersionLow	5 /* #3 BlockVersionLow */
#define CM_REQ_RSINFO_Padding1			6 /* #4 [u16] Padding */
#define CM_REQ_RSINFO_RSProperties		8 /* #5 [u32] RSProperties */

#define CM_REQ_RSINFO_SIZE				(12U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// ARVendorBlockReq ::=
//   BlockHeader,
//   APStructureIdentifier, API, [Data*], [Padding*]
//----------------------------------------------------------------------------*/

#define CM_REQ_ARVENDOR_BlockHeader				0 /* BlockHeader */
#define CM_REQ_ARVENDOR_APStructureIdentifier	6 /* [u16] APStructureIdentifier */
#define CM_REQ_ARVENDOR_API						8 /* [u32] API */

#define CM_REQ_ARVENDOR_SIZE					(12U) /* size including BlockHeader, excluding Data/Padding */

/*------------------------------------------------------------------------------
// ARVendorBlockRes ::=
//   BlockHeader,
//   APStructureIdentifier, API, [Data*], [Padding*]
//----------------------------------------------------------------------------*/

#define CM_RSP_ARVENDOR_BlockHeader				0 /* BlockHeader */
#define CM_RSP_ARVENDOR_APStructureIdentifier	6 /* [u16] APStructureIdentifier */
#define CM_RSP_ARVENDOR_API						8 /* [u32] API */

#define CM_RSP_ARVENDOR_SIZE					(12U) /* size including BlockHeader, excluding Data/Padding */


/*------------------------------------------------------------------------------
// ARServerBlock ::=
//	BlockHeader, StationNameLength, CMResponderStationName, [Padding* a]
//
// a The number of Padding octets shall ensure Unsigned32 alignment.
//----------------------------------------------------------------------------*/

#define CM_RSP_ARSERVER_BlockHeader		0 /* BlockHeader */
#define CM_RSP_ARSERVER_cmr_name_length	6 /* [u16] StationNameLength */

#define CM_RSP_ARSERVER_SIZE			(8U) /* size including BlockHeader, excluding CMResponderStationName/Padding */


/*------------------------------------------------------------------------------
// ExpectedSubmoduleBlockReq ::=
//   BlockHeader,
//   NumberOfRelatedAPIs,
//   ( API, SlotNumber, ModuleIdentNumber, ModuleProperties,
//     NumberOfSubmodules,
//     ( SubslotNumber, SubmoduleIdentNumber, SubmoduleProperties,
//       <one or two, depending on the properties>
//       ( DataDescription, SubmoduleDataLength, LengthIOPS, LengthIOCS
//       )*
//     )*
//   )*
//----------------------------------------------------------------------------*/

#define CM_REQ_API_BlockHeader				0 /* BlockHeader */
#define CM_REQ_API_number_of_related_apis	6 /* [u16] NumberOfRelatedAPIs */

#define CM_REQ_API_SIZE						(8U) /* size including BlockHeader */

/* part 2 follows */

#define CM_REQ_MOD_api					0  /* [u32] API */
#define CM_REQ_MOD_slot_nr				4  /* [u16] SlotNumber */
#define CM_REQ_MOD_mod_ident			6  /* [u32] ModuleIdentNumber */
#define CM_REQ_MOD_mod_properties		10 /* [u16] ModuleProperties; see defines CM_MOD_PROP_UNCHECKED_0_7, ... */
#define CM_REQ_MOD_number_of_submodules	12 /* [u16] NumberOfSubmodules */

#define CM_REQ_MOD_SIZE					(14U)

/* part 3 follows */

#define CM_REQ_SUB_subslot_nr			0 /* [u16] SubslotNumber */
#define CM_REQ_SUB_sub_ident			2 /* [u32] SubmoduleIdentNumber */
#define CM_REQ_SUB_sub_properties		6 /* [u16] SubmoduleProperties; see defines CM_SUB_PROP_TYPE_MASK, ... */

#define CM_REQ_SUB_SIZE					(8U)

/* part 4 follows */

#define CM_REQ_DSC_data_description		0 /* [u16] DataDescription */
#define CM_REQ_DSC_data_length			2 /* [u16] SubmoduleDataLength */
#define CM_REQ_DSC_iops_length			4 /* [u8] LengthIOPS */
#define CM_REQ_DSC_iocs_length			5 /* [u8] LengthIOCS */

#define CM_REQ_DSC_SIZE					(6U)


/*------------------------------------------------------------------------------
// ModuleDiffBlock ::=
//   BlockHeader,
//   NumberOfAPIs,
//   ( API,
//     NumberOfModules
//     ( SlotNumber, ModuleIdentNumber, ModuleState
//       NumberOfSubmodules,
//       ( SubslotNumber, SubmoduleIdentNumber, SubmoduleState
//       )*
//     )*
//   )*
//----------------------------------------------------------------------------*/

#define CM_RSP_MDB_BlockHeader			0 /* BlockHeader */
#define CM_RSP_MDB_number_of_apis		6 /* [u16] NumberOfAPIs */

#define CM_RSP_MDB_SIZE					(8U) /* size including BlockHeader */

/* part 2 follows */

#define CM_RSP_API_api					0 /* [u32] API */
#define CM_RSP_API_number_of_modules	4 /* [u16] NumberOfModules */

#define CM_RSP_API_SIZE					(6U)

/* part 3 follows */

#define CM_RSP_MOD_slot_nr				0 /* [u16] SlotNumber */
#define CM_RSP_MOD_mod_ident			2 /* [u32] ModuleIdentNumber */
#define CM_RSP_MOD_mod_state			6 /* [u16] ModuleState */
#define CM_RSP_MOD_number_of_submodules	8 /* [u16] NumberOfSubmodules */

#define CM_RSP_MOD_SIZE					(10U)

/* part 4 follows */

#define CM_RSP_SUB_subslot_nr			0 /* [u16] SubslotNumber */
#define CM_RSP_SUB_sub_ident			2 /* [u32] SubmoduleIdentNumber */
#define CM_RSP_SUB_sub_state			6 /* [u16] SubmoduleState */

#define CM_RSP_SUB_SIZE					(8U)


/*------------------------------------------------------------------------------
// AlarmCRBlockReq ::=
//   BlockHeader,
//   AlarmCRType, LT, AlarmCRProperties,
//   RTATimeoutFactor, RTARetries,
//   LocalAlarmReference, MaxAlarmDataLength,
//   AlarmCRTagHeaderHigh, AlarmCRTagHeaderLow
//----------------------------------------------------------------------------*/

#define CM_REQ_ALCR_BlockHeader				0  /* 0  BlockHeader */
#define CM_REQ_ALCR_alcr_type				6  /* 4  [u16] AlarmCRType */
#define CM_REQ_ALCR_ethertype				8  /* 5  [u16] LT */
#define CM_REQ_ALCR_alcr_properties			10 /* 6  [u32] AlarmCRProperties */
#define CM_REQ_ALCR_timeout100ms			14 /* 7  [u16] RTATimeoutFactor */
#define CM_REQ_ALCR_retries					16 /* 8  [u16] RTARetries */
#define CM_REQ_ALCR_local_alarm_ref			18 /* 9  [u16] LocalAlarmReference */
#define CM_REQ_ALCR_max_alarm_data_length	20 /* 10 [u16] MaxAlarmDataLength */
#define CM_REQ_ALCR_alcr_tag_header_high	22 /* 11 [u16] AlarmCRTagHeaderHigh; VLAN */
#define CM_REQ_ALCR_alcr_tag_header_low		24 /* 12 [u16] AlarmCRTagHeaderLow; VLAN */

#define CM_REQ_ALCR_SIZE					(26U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// AlarmCRBlockRes ::=
//   BlockHeader,
//   AlarmCRType, LocalAlarmReference, MaxAlarmDataLength
//----------------------------------------------------------------------------*/

#define CM_RSP_ALCR_BlockHeader				0  /* BlockHeader */
#define CM_RSP_ALCR_alcr_type				6  /* [u16] AlarmCRType */
#define CM_RSP_ALCR_local_alarm_ref			8  /* [u16] LocalAlarmReference; local: view from sender of PDU */
#define CM_RSP_ALCR_max_alarm_data_length	10 /* [u16] MaxAlarmDataLength */

#define CM_RSP_ALCR_SIZE					(12U) /* size including BlockHeader */


/*------------------------------------------------------------------------------
// PrmServerBlock ::=
//   BlockHeader,
//   ParameterServerObjectUUID, ParameterServerProperties,
//   CMInitiatorActivityTimeout,
//   StationNameLength, ParameterServerStationName
//----------------------------------------------------------------------------*/

/* PrmServerBlock not yet supported */


/*------------------------------------------------------------------------------
// ControlBlockConnect ::=
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ControlBlockPlug ::=
//   BlockHeader
//   reserved (2 octets), ARUUID, SessionKey,
//   AlarmSequenceNumber, ControlCommand, ControlBlockProperties
//
// ControlBlockRFC ::= ... Ready for Companion
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ControlBlockRTC ::= ... Ready for RTClass3
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ReleaseBlock ::=
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//----------------------------------------------------------------------------*/

#define CM_CONTROL_COMMAND_PRM_END             0x0001
#define CM_CONTROL_COMMAND_APPLICATION_READY   0x0002
#define CM_CONTROL_COMMAND_RELEASE             0x0004
#define CM_CONTROL_COMMAND_DONE                0x0008
#if 0/*reserved for future use*/
#define CM_CONTROL_COMMAND_READY_FOR_COMPANION 0x0010
#endif
#define CM_CONTROL_COMMAND_READY_FOR_RTC3      0x0020
#define CM_CONTROL_COMMAND_PRM_BEGIN		   0x0040

#if 0/*reserved for future use*/
#define CM_CONTROL_PROP_READY_FOR_RFC          0x0001 /* used with CM_CONTROL_COMMAND_APPLICATION_READY */
#endif
#define CM_CONTROL_PROP_READY_FOR_RTC3         0x0002 /* used with CM_CONTROL_COMMAND_APPLICATION_READY */


#define CM_BLK_CTRL_BlockHeader						0  /* BlockHeader */
#define CM_BLK_CTRL_reserved1						6  /* [u16] reserved (2 octets) */
#define CM_BLK_CTRL_ar_uuid							8  /* [uid] ARUUID */
#define CM_BLK_CTRL_session_key						24 /* [u16] SessionKey */
#define CM_BLK_CTRL_reserved2_alarm_sequence_number	26 /* [u16] reserved (2 octets) or AlarmSequenceNumber */
#define CM_BLK_CTRL_control_command					28 /* [u16] ControlCommand; see CM_CONTROL_COMMAND_PRM_END, etc. above */
#define CM_BLK_CTRL_control_properties				30 /* [u16] ControlBlockProperties */

#define CM_BLK_CTRL_SIZE							(32U) /* size including BlockHeader */


#define CM_BLK_REL_BlockHeader						CM_BLK_CTRL_BlockHeader
#define CM_BLK_REL_reserved1						CM_BLK_CTRL_reserved1
#define CM_BLK_REL_ar_uuid							CM_BLK_CTRL_ar_uuid
#define CM_BLK_REL_session_key						CM_BLK_CTRL_session_key
#define CM_BLK_REL_reserved2_alarm_sequence_number	CM_BLK_CTRL_reserved2_alarm_sequence_number
#define CM_BLK_REL_control_command					CM_BLK_CTRL_control_command
#define CM_BLK_REL_control_properties				CM_BLK_CTRL_control_properties

#define CM_BLK_REL_SIZE								CM_BLK_CTRL_SIZE


/*------------------------------------------------------------------------------
// SubmoduleListBlock ::=
//   BlockHeader, NumberOfEntries, (API, SlotNumber, SubslotNumber)*
//----------------------------------------------------------------------------*/

#define CM_BLK_SUBMOD1_BlockHeader			0 /* BlockHeader */
#define CM_BLK_SUBMOD1_number_of_entries	6 /* [u16] NumberOfEntries */

#define CM_BLK_SUBMOD1_SIZE					(8U) /* size including BlockHeader */

/* part 2 follows */

#define CM_BLK_SUBMOD2_api					0 /* [u32] API */
#define CM_BLK_SUBMOD2_slot_nr				4 /* [u16] SlotNumber */
#define CM_BLK_SUBMOD2_subslot_nr			6 /* [u16] SubslotNumber */

#define CM_BLK_SUBMOD2_SIZE					(8U)


/*------------------------------------------------------------------------------
// IODReadReq ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   [TargetARUUID], RWPadding*
//
// IODReadRes ::=
//   IODReadHeader, RecordDataRead
//
// IODReadHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   AdditionalValue1, AdditionalValue2, RWPadding*
//
//
// IODWriteReq ::=
//   IODWriteHeader, RecordDataWrite
//
// IODWriteMultipleReq ::=
//   IODWriteHeader(Index=0xE040),
//   ( IODWriteHeader, RecordDataWrite, Padding* )*
//
// IODWriteHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   RWPadding*
//
// IODWriteRes ::=
//   IODWriteResHeader
//
// IODWriteMultipleRes ::=
//   IODWriteResHeader(Index=0xE040),
//   ( IODWriteResHeader )*
//
// IODWriteResHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   AdditionalValue1, AdditionalValue2,
//   PNIOStatus, RWPadding*
//
//----------------------------------------------------------------------------*/

#define CM_BLK_RDWR_BlockHeader						0  /* BlockHeader */
#define CM_BLK_RDWR_seq_number						6  /* [u16] SeqNumber */
#define CM_BLK_RDWR_ar_uuid							8  /* [uid] ARUUID */
#define CM_BLK_RDWR_api								24 /* [u32] API */
#define CM_BLK_RDWR_slot_nr							28 /* [u16] SlotNumber */
#define CM_BLK_RDWR_subslot_nr						30 /* [u16] SubslotNumber */
#define CM_BLK_RDWR_padding_1						32 /* [u16] Padding */
#define CM_BLK_RDWR_index							34 /* [u16] Index */
#define CM_BLK_RDWR_record_data_length				36 /* [u32] RecordDataLength */

#define CM_BLK_RDWR_req_rd_explicit_padding_2		40 /* padding_2[24]; so that the header-size is 64 */

#define CM_BLK_RDWR_req_rd_implicit_target_ar_uuid	40 /* [uid] TargetARUUID */
#define CM_BLK_RDWR_req_rd_implicit_padding_2		56 /* padding_2[8]; so that the header-size is 64 */

#define CM_BLK_RDWR_req_wr_padding_2				40 /* padding_2[24]; so that the header-size is 64 */

#define CM_BLK_RDWR_rsp_rd_wr_additional_value_1	40 /* [u16] AdditionalValue1 */
#define CM_BLK_RDWR_rsp_rd_wr_additional_value_2	42 /* [u16] AdditionalValue2 */
#define CM_BLK_RDWR_rsp_rd_wr_padding_2				44 /* padding_2[20]; so that the header-size is 64 */

#define CM_BLK_RDWR_rsp_wr_additional_value_1		40 /* [u16] AdditionalValue1 */
#define CM_BLK_RDWR_rsp_wr_additional_value_2		42 /* [u16] AdditionalValue2 */
#define CM_BLK_RDWR_rsp_wr_pnio_status				44 /* [u32] PNIOStatus */
#define CM_BLK_RDWR_rsp_wr_padding_2				48 /* padding_2[16]; so that the header-size is 64 */

#define CM_BLK_RDWR_SIZE							(64U) /* size including BlockHeader */

#if CM_BLK_RDWR_SIZE != CM_RECORD_HEADER_SIZE
#error "check this!"
#endif

/*------------------------------------------------------------------------------
// LogEntry ::=
//   LocalTimeStamp, ARUUID, PNIOStatus, EntryDetail
//----------------------------------------------------------------------------*/

#define CM_PDU_LOG_ENTRY_local_time_stamp_high	0  /* [u32] LocalTimeStamp (High) */
#define CM_PDU_LOG_ENTRY_local_time_stamp_low	4  /* [u32] LocalTimeStamp (Low) */
#define CM_PDU_LOG_ENTRY_ar_uuid				8  /* [uid] ARUUID */
#define CM_PDU_LOG_ENTRY_pnio_status			24 /* [u32] PNIOStatus */
#define CM_PDU_LOG_ENTRY_entry_detail			28 /* [u32] EntryDetail */

#define CM_PDU_LOG_ENTRY_SIZE					(32U)


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_PDU_H */
