#ifndef CM_IPD_H
#define CM_IPD_H

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
/*  F i l e               &F: cm_ipd.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - physical device things                            */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*=== used by CMPD and CMCL =================================================*/
/*===========================================================================*/

#define CM_ALARM_SEQUENCE_UNDEF		0xFFFF /* valid alarm_sequence: 0..0x07FF */

#define CM_ALARM_SEQUENCE_NR_INCREMENT(seq_nr) {\
		(seq_nr) += 1; \
		if ((seq_nr) > 0x07FF) { (seq_nr) = 0; } \
	}

#define CM_IS_PDEV_SUBSLOT_NR(_subslot_nr)	(((_subslot_nr) & 0xE000) == 0x8000) /* true if 0x8ipp/0x9jpp */
#define CM_IS_PDEV_INTERFACE(_subslot_nr) (CM_IS_PDEV_SUBSLOT_NR (_subslot_nr) && ((_subslot_nr) & 0x00FF) == 0x0000) /* a pdev-interface */
#define CM_IS_PDEV_PORT(_subslot_nr) (CM_IS_PDEV_SUBSLOT_NR (_subslot_nr) && ((_subslot_nr) & 0x00FF) != 0x0000) /* a pdev-port */

#define CM_PDEV_TYPE_IS_COMPACT(_pdev_properties) ((_pdev_properties & CM_PDEV_PROP_TYPE_MASK) == CM_PDEV_PROP_TYPE_COMPACT)
#define CM_PDEV_TYPE_IS_MODULAR(_pdev_properties) ((_pdev_properties & CM_PDEV_PROP_TYPE_MASK) == CM_PDEV_PROP_TYPE_MODULAR)

#define CM_PDEV_STATE_IS_PULLED(_pdev_properties)	((_pdev_properties & CM_PDEV_PROP_STATE_MASK) == CM_PDEV_PROP_STATE_PULLED)
#define CM_PDEV_STATE_IS_PLUGGED(_pdev_properties)	((_pdev_properties & CM_PDEV_PROP_STATE_MASK) == CM_PDEV_PROP_STATE_PLUGGED)

#define CM_REDUNDANT_FRAME_ID_BASE(frame_id)		((0x0700 <= (frame_id) && (frame_id) <= 0x0FFF) ? ((frame_id) & 0x0FFE) : 0)

#define CM_PD_CURRENT_IS_DELETE(_channel) (_channel->state == CM_PD_DELETING\
		&&  cm_is_not_null (_channel->usr.pd.req.current)\
		&& CM_RQB_GET_OPCODE (_channel->usr.pd.req.current) == CM_OPC_PD_DELETE)

/*------------------------------------------------------------------------------
// for marshalling and parsing
//----------------------------------------------------------------------------*/

#define cm_dp_assert_set_line() {\
		cm_dp_assert_line = (LSA_UINT16)(__LINE__);\
	}

#define cm_dp_assert_set_error(err_) {\
		cm_dp_assert_error = err_;\
	}

#define cm_dp_goto_failed(err_) {\
		cm_dp_assert_set_line();\
		cm_dp_assert_set_error(err_);\
		/*lint -e(801) Use of goto is deprecated - request 1502507*/\
		goto cm_dp_assert_failed;\
	} /*lint -unreachable */

#define cm_dp_assert(cond_) {\
	if (! (cond_)) {\
		cm_dp_assert_set_line();\
		/*lint -e(801) Use of goto is deprecated - request 1502507*/ \
		goto cm_dp_assert_failed;\
	}}

#define cm_dp_assert2(cond_, err_) {\
	if (! (cond_)) {\
		cm_dp_assert_set_line();\
		cm_dp_assert_set_error(err_);\
		/*lint -e(801) Use of goto is deprecated - request 1502507*/ \
		goto cm_dp_assert_failed;\
	}}

#define cm_dp_adjust(dp_, left_, sizeof_) {\
	dp_ += (sizeof_); \
	left_ -= (sizeof_); \
	}

/*----------------------------------------------------------------------------*/
/* assert size of fields */

#if CM_DEBUG  &&  defined(_MSC_VER)

# define CM_SIZE_ASSERT_8(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT8) == sizeof (((type_*)0)->field_))

# define CM_SIZE_ASSERT_16(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT16) == sizeof (((type_*)0)->field_))

# define CM_SIZE_ASSERT_32(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT32) == sizeof (((type_*)0)->field_))

#else

# define CM_SIZE_ASSERT_8(type_, field_)
# define CM_SIZE_ASSERT_16(type_, field_)
# define CM_SIZE_ASSERT_32(type_, field_)

#endif


/*-----------------------------------------------------------------------------*/
#define CM_PD_PRM_DIAG_SET(prm_diag_, channel_error_type_, ext_channel_error_type_, port_id_, index_, offset_) {\
	(prm_diag_)->channel_error_type = channel_error_type_;\
	(prm_diag_)->ext_channel_error_type = ext_channel_error_type_;\
	(prm_diag_)->err_port_id = port_id_;\
	(prm_diag_)->err_index   = index_;\
	(prm_diag_)->err_offset  = offset_;\
}

#define CM_PD_PRM_DIAG_SET_WRITE_MISSING(prm_diag_, port_id_, index_) {\
	CM_PD_PRM_DIAG_SET( prm_diag_, 0x000F, 0x8001, port_id_, index_, 0);\
}

#define CM_PD_PRM_DIAG_SET_WRITE_FAULT(prm_diag_, port_id_, index_, offset_) {\
	CM_PD_PRM_DIAG_SET( prm_diag_, 0x0010, 0x8001, port_id_, index_, offset_);\
}

#define CM_PD_PRM_DIAG_SET_END_MISSING(prm_diag_, port_id_, index_) {\
	CM_PD_PRM_DIAG_SET( prm_diag_, 0x000F, 0x8002, port_id_, index_, 0);\
}

#define CM_PD_PRM_DIAG_SET_END_FAULT(prm_diag_, port_id_, index_, offset_) {\
	CM_PD_PRM_DIAG_SET( prm_diag_, 0x0010, 0x8002, port_id_, index_, offset_);\
}


/*---------------------------------------------------------------------------*/
/* PRM End Extension for Server */

#define CM_PD_PRM_END_PULLED    0x0001
#define CM_PD_PRM_END_WRONG     0x0002
#define CM_PD_PRM_END_LOCKED    0x0004
#define CM_PD_PRM_END_DIRTY     0x0008
#define CM_PD_PRM_END_WRITE     0x0010
#define CM_PD_PRM_END_RESERVED  0xFFE0 /* unused bits */

typedef struct cm_pd_ar_prm_end_elem_tag {

	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;

	LSA_UINT32 index;
	LSA_UINT32 offset;
	LSA_UINT16 flags;

} CM_PD_AR_PRM_END_ELEM_TYPE;

typedef CM_PD_AR_PRM_END_ELEM_TYPE * CM_PD_AR_PRM_END_ELEM_PTR_TYPE;

typedef struct cm_pd_ar_prm_end_tag { /* additional CMSV-Pdev-prm-end-args */

	/* important: if changed, must change CM_PD_AR_PRM_END_63P_TYPE too! */

	CLRPC_UUID_TYPE ir_data_uuid;

	LSA_UINT16 max_alarm_data_length;

	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr; /* 0: all (connect or PBE-ALL), 8i00/9j00: interface (PBE-IF), 8ipp/9jpp: port (plug-alarm) */

	LSA_UINT16 nr_of_elems; /* note: != 0 if CM-SV detected a problem */

	CM_PD_AR_PRM_END_ELEM_TYPE elem[1 + EDD_CFG_MAX_PORT_CNT];

} CM_PD_AR_PRM_END_TYPE;

typedef CM_PD_AR_PRM_END_TYPE * CM_PD_AR_PRM_END_PTR_TYPE;


typedef struct cm_pd_ar_prm_end_63P_tag { /* R1, "derived" from CM_PD_AR_PRM_END_TYPE (more elements) */

	/* note: begin of struct *is* CM_PD_AR_PRM_END_TYPE */

	CLRPC_UUID_TYPE ir_data_uuid;

	LSA_UINT16 max_alarm_data_length;

	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr; /* 0: all (connect or PBE-ALL), 8i00/9j00: interface (PBE-IF), 8ipp/9jpp: port (plug-alarm) */

	LSA_UINT16 nr_of_elems; /* note: != 0 if CM-SV detected a problem */

	CM_PD_AR_PRM_END_ELEM_TYPE elem[1 + 63]; /* see "nr_of_remote_pdev_submodules" */

#if  EDD_CFG_MAX_PORT_CNT > 63 /* see CM_PORT_UINT */
#error "check this!"
#endif

} CM_PD_AR_PRM_END_63P_TYPE;

typedef CM_PD_AR_PRM_END_63P_TYPE * CM_PD_AR_PRM_END_63P_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Record
//----------------------------------------------------------------------------*/
#  ifdef CM_MESSAGE
/* todo: rename to cm_list_record_tag */
#  endif /* CM_MESSAGE */

#define CM_TAILOR_DATA_LEN_NOT_TAILORED		0
#define CM_TAILOR_DATA_LEN_SKIP_RECORD		1

typedef struct cm_pd_record_tag {
	CM_LIST_ENTRY_TYPE			link; /* first in struct!  CL: link within submodule | PD: link within reclist */

	LSA_VOID_PTR_TYPE			backlink; /* CL: argr | PD: channel */
	CM_LIST_ENTRY_TYPE			prm_rec_link; /* CL: link within prm.prm_rec_list | link within prm.prm_multi_check_cnf_list */

	LSA_UINT32					api;
	LSA_UINT16					slot_nr;
	LSA_UINT16					subslot_nr;

	LSA_UINT32					record_index;
	LSA_UINT16					transfer_sequence;

	LSA_BOOL					mk_remanent;
	LSA_BOOL					is_local;

	LSA_UINT16					tailor_data_alloc_len;
	LSA_UINT16					tailor_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	tailor_data;

	LSA_UINT16					record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	record_data; /* request 1502507 (lint) */

	/*** nothing here, dynamic size ***/

} * CM_PD_RECORD_PTR_TYPE;

typedef struct cm_pd_record_tag const * CM_PD_RECORD_CONST_PTR_TYPE;


/*===========================================================================*/
/*=== DIAG ==================================================================*/
/*===========================================================================*/

#define CM_PD_DIAG_TAG_SUBCOMP		1
#define CM_PD_DIAG_TAG_PDUSER		2
#define CM_PD_DIAG_TAG_PRMERROR		3

typedef struct cm_pd_diag_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag) link; /* first in struct */

	LSA_BOOL    r1_mirrored;

	LSA_UINT16  diag_tag;		/* CM_PD_DIAG_TAG_ */

	LSA_UINT16  alarm_type;
	LSA_UINT16  channel_properties;
	LSA_UINT16  channel_error_type;
	LSA_UINT16  ext_channel_error_type;
	LSA_UINT32  ext_channel_add_value;

} CM_PD_DIAG_TYPE;

typedef CM_PD_DIAG_TYPE *CM_PD_DIAG_PTR_TYPE;

typedef CM_PD_DIAG_TYPE const *CM_PD_DIAG_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== ADDRESS-MAPPING =======================================================*/
/*===========================================================================*/

typedef struct cm_pd_port_addr_tag {

	/* logical address */
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;	    /* format 0x8ipp/0x9jpp */

	LSA_UINT8  pdev_properties; /* CM_PDEV_PROP_ */
	LSA_UINT8  im0_bits;        /* see CM_SV_IM0_BITS_NOTHING(0), ... */

	LSA_UINT32 mod_ident;
	LSA_UINT32 sub_ident;

} CM_PD_PORT_ADDR_TYPE;

/*===========================================================================*/
/*=== PORT ==================================================================*/
/*===========================================================================*/

typedef struct cm_pd_port_tag {

	LSA_UINT16  port_id; /* key of this entry, given by create */

	CM_PD_PORT_ADDR_TYPE  addr; /* address mapping, ident_numbers */

	struct {

		LSA_BOOL  update_allowed; /* see cm_pd_diag_update_start / cm_pd_diag_update_done */

		LSA_BOOL  server_in_sync; /* set to false if a pd sv race condition might occur TIA 2898601 */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list; /* list of CM_PD_DIAG_TYPE */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list_server; /* list of CM_PD_DIAG_TYPE */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list_user; /* list of CM_PD_DIAG_TYPE */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list_prm_sc; /* list of CM_PD_DIAG_TYPE */
																	/* diag occured while prm-sequence */

	} diag;

	struct {

		LSA_UINT16 sync_led_info; /* see cm_pd_sync_led_info_enum */
		LSA_BOOL   sync_led__is_rate_valid;
		LSA_UINT8  sync_led__rcv_sync_prio;
		LSA_UINT8  sync_led__local_prio;
		LSA_UINT8  sync_led__pll_state;

		LSA_UINT8  MRPRingPort; /* EDD_MRP_NO_RING_PORT etc. */
		/* interface: see EDDGetParams.MRPSupportedRole   port: see EDDGetPortParams.MRPRingPort */

		CM_UPPER_RQB_PTR_TYPE  link_status_ind; /* only one resource allowed per port_id, see CM_OPC_PD_LINK_STATUS_INFO */
	} misc;

} CM_PD_PORT_TYPE;

typedef CM_PD_PORT_TYPE *CM_PD_PORT_PTR_TYPE;

typedef CM_PD_PORT_TYPE const *CM_PD_PORT_CONST_PTR_TYPE;

typedef struct cm_pd_port_prm_data_tag {

	LSA_BOOL  do_irt;				/* index 0x802C: PDIRData */
	LSA_BOOL  do_mrp_record;		/* something is written to MRP */
	LSA_UINT16  scr_send_clock;		/* from SendClock-record */
	LSA_UINT16  scr_sendclock_prop;	/* from SendClock-record */

	CLRPC_UUID_TYPE  ir_data_uuid;	/* PDIRData.PDIRGlobalData.IRDataUUID */
} CM_PD_PORT_IF_PRM_DATA_TYPE;



/*------------------------------------------------------------------------------
// PDMasterTailorData, PDInstanceTailorData
//----------------------------------------------------------------------------*/

/*
//	PDMasterTailorData [ Index = 0x00017081, BlockType = 0x7081 ] ::=
//		BlockHeader, DeviceTailorProperties, DeviceGroup, NumberOfPortTailorInfos, [PortMasterTailorInfo*]
//
//	PortMasterTailorInfo ::=
//		LocalSlotNumber, LocalSubslotNumber, PortTailorProperties, PeerStationNumber, PeerSlotNumber, PeerSubslotNumber
*/

/* DeviceTailorProperties */
#define CM_PD_MTD_PROP_DEVICE_OPTIONAL	0x0001 /* DeviceMode(optional) // not valid for PDev */
#define CM_PD_MTD_PROP_NOS				0x0002 /* TailorNoS */
#define CM_PD_MTD_PROP_IP_SUITE			0x0004 /* TailorIPv4 */
#define CM_PD_MTD_PROP_SYNCDATA			0x0008 /* TailorPDSyncData */
#define CM_PD_MTD_PROP_MRP_DATA_ADJUST	0x0010 /* TailorPDInterfaceMrpDataAdjust */
#define CM_PD_MTD_PROP_MRP_DATA_CHECK	0x0020 /* TailorPDInterfaceMrpDataCheck */
#define CM_PD_MTD_PROP_IR_FRAME_DATA	0x0040 /* TailorPDIRFrameData */
#define CM_PD_MTD_PROP_RESERVED			0xFF80 /* reserved */
#define CM_PD_MTD_PROP_I_MRP_DATA_MASK	0x0030 /* internal mask: both mrp must set */
#define CM_PD_MTD_PROP_I_ANY_PEER_OPT	0x1000 /* internal bit: any peers is optional */
#define CM_PD_MTD_PROP_I_ANY_PEER_PROG	0x2000 /* internal bit: any peer is programmable */

/* PortTailorProperties */
#define CM_PD_MTD_PEER_PROP_PEER_MASK			0x0003 /* PeerMode */
#define CM_PD_MTD_PEER_PROP_PEER_FIXED			0x0000 /* PeerMode Fixed peer to mandatory neighbor */
#define CM_PD_MTD_PEER_PROP_PEER_OPT			0x0001 /* PeerMode Fixed peer to optional neighbor */
#define CM_PD_MTD_PEER_PROP_PEER_PROG			0x0002 /* PeerMode Programmable peer */
#define CM_PD_MTD_PEER_PROP_PEER_ANY			0x0003 /* PeerMode Any peer */
#define CM_PD_MTD_PEER_PROP_PORT_CODING			0x0004 /* NameOfPortCoding */
#define CM_PD_MTD_PEER_PROP_RED_PORT			0x0008 /* TailorREDPort */
#define CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST		0x0010 /* TailorPDPortMrpDataAdjust */
#define CM_PD_MTD_PEER_PROP_RESERVED			0xFFE0 /* reserved */
#define CM_PD_MTD_PEER_PROP_I_PARSED			0x1000 /* internal bit: PDPortDataCheck parsed */

/* set_by_itd */
#define CM_PD_MTD_SET_IS_TAILORED				0x0001 /* PDInstanceTailorData was written */
#define CM_PD_MTD_SET_DEVICE_ENABLED			0x0002 /* enabled by .DeviceInstanceTailorData.StationNumber */

typedef struct cm_pd_mtd_port_tag
{
	struct cm_pd_master_tailor_data_tag	*mtd;	/* backlink to MasterTailorData */

	LSA_UINT16  local_station_nr;		/* local StationNumber */
	LSA_UINT16  local_slot_nr;			/* PortMasterTailorData.LocalSlotNumber */
	LSA_UINT16  local_subslot_nr;		/* PortMasterTailorData.LocalSubslotNumber */

	LSA_UINT16  properties;				/* PortMasterTailorData.PortTailorProperties */

	LSA_UINT16  peer_station_nr;		/* PortMasterTailorData.PeerStationNumber  or  PDInstanceTailorData...PeerPort.StationNumber */
	LSA_UINT16  peer_slot_nr;			/* PortMasterTailorData.PeerSlotNumber     or  PDInstanceTailorData...PeerPort.SlotNumber */
	LSA_UINT16  peer_subslot_nr;		/* PortMasterTailorData.PeerSubslotNumber  or  PDInstanceTailorData...PeerPort.SubslotNumber */

	LSA_UINT32  check_linedelay;		/* parsed PDPortDataCheck.CheckLineDelay  0..reserved/unknown */
	LSA_UINT32  max_line_rx_delay;		/* parsed PDIRData.PDIRGlobalData.MaxLineRxDelay  0..unknown */

	/* following fields are valid after call of cm_pd_mtdp_get_peer() */
	LSA_UINT16					real_station_nr;		/* 0xFFFF .. this port has no peer */
	struct cm_pd_mtd_port_tag *	real_mtdp;				/* LSA_NULL .. real_station_nr not added or real_station_nr/slot/subslot not found */
	LSA_UINT16					real_properties;		/* neighbour properties */
	LSA_UINT32					real_max_line_rx_delay;	/* tailored MaxLineRXDelay;  0 .. no neighbour or green neighbour */
} CM_PD_MTD_PORT_TYPE;

typedef CM_PD_MTD_PORT_TYPE * CM_PD_MTD_PORT_PTR_TYPE;

typedef CM_PD_MTD_PORT_TYPE const * CM_PD_MTD_PORT_CONST_PTR_TYPE;

typedef struct cm_pd_master_tailor_data_tag
{
	LSA_UINT16				properties;				/* != 0 if PDMasterTailorData is written */
	LSA_UINT16				port_cnt;				/* count of PortMasterTailorData*/
	CM_PD_MTD_PORT_PTR_TYPE	ports;					/* PortMasterTailorData */
	LSA_UINT16				set_by_itd;				/* infos given by PDInstanceTailorData */
	LSA_UINT16				pdirglobaldata_version;	/* version of the PDIRGlobalData  0..unknown/not existing */
	LSA_UINT32				max_bridge_delay;		/* PDIRData.PDIRGlobalData.MaxBridgeDelay */
	LSA_UINT16				frame_id[2];			/* consumer/provider frame_id */
} CM_PD_MASTER_TAILOR_DATA_TYPE;

typedef CM_PD_MASTER_TAILOR_DATA_TYPE * CM_PD_MASTER_TAILOR_DATA_PTR_TYPE;

typedef CM_PD_MASTER_TAILOR_DATA_TYPE const * CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE;

/*
//	PDInstanceTailorData [ Index = 0xB080, BlockType = 0x7080 ] ::=
//		BlockHeader, Padding, Padding, DeviceInstanceTailorData, [ PortInstanceTailorData ]
//
//	DeviceInstanceTailorData ::=	# enable optional devices
//		DeviceGroup, NumberOfDevices, (StationNumber, Padding, Padding)*
//
//	PortInstanceTailorData ::		# peer programable ports, use enabled or mandatory devices
//		Padding, Padding, NumberOfInterconnections, (PeerPort1, PeerPort2)*
//
//	PeerPort1 ::=
//		StationNumber, SlotNumber, SubslotNumber, Padding, Padding
//
//	PeerPort2 ::=
//		StationNumber, SlotNumber, SubslotNumber, Padding, Padding
*/

typedef struct cm_pd_port_instance_peer_tag
{
	/* PeerPort1 */
	LSA_UINT16		device_nr_1;
	LSA_UINT16		slot_nr_1;
	LSA_UINT16		subslot_nr_1;
	/* PeerPort2 */
	LSA_UINT16		device_nr_2;
	LSA_UINT16		slot_nr_2;
	LSA_UINT16		subslot_nr_2;
} CM_PD_PORT_INSTANCE_PEER_TYPE;

typedef CM_PD_PORT_INSTANCE_PEER_TYPE * CM_PD_PORT_INSTANCE_PEER_PTR_TYPE;

typedef struct cm_pd_instance_tailor_data_tag
{
	LSA_UINT16							nr_of_devices;		/* NumberOfDevices */
	LSA_UINT16							*devices;			/* enable these devices */
	LSA_UINT16							nr_of_peers;		/* NumberOfInterconnections */
	CM_PD_PORT_INSTANCE_PEER_PTR_TYPE	peers;				/* connect these pdev-ports */
} CM_PD_INSTANCE_TAILOR_DATA_TYPE;

typedef CM_PD_INSTANCE_TAILOR_DATA_TYPE * CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE;

typedef CM_PD_INSTANCE_TAILOR_DATA_TYPE const * CM_PD_INSTANCE_TAILOR_DATA_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== PD-USER CHANNEL-TYPE ==================================================*/
/*===========================================================================*/

enum cm_pd_occupant_enum
{
	CM_PD_OCCUPANT_NOBODY = 0,
	CM_PD_OCCUPANT_USER   = 1,
	CM_PD_OCCUPANT_SV     = 2
};

enum cm_sub_comp_id_enum
{
	CM_PD_SCID_EDD		= (CM_REC_IDX_CMPD_EDD),			/* 1 */
#if CM_CFG_USE_MRP
	CM_PD_SCID_MRP		= (CM_REC_IDX_CMPD_MRP),			/* 2 */
#endif
#if CM_CFG_USE_GSY
	CM_PD_SCID_GSY		= (CM_REC_IDX_CMPD_GSY),			/* 3 */
#endif
	CM_PD_SCID_0x04		= (CM_REC_IDX_CMPD_0x04),			/* 4 (defunct, was GSY_TIME) */
#if CM_CFG_USE_POF
	CM_PD_SCID_POF		= (CM_REC_IDX_CMPD_POF),			/* 5 */
#endif
	CM_PD_SCID_OHA		= (CM_REC_IDX_CMPD_OHA),			/* 6 */
	CM_PD_SCID_BBUF		= (CM_REC_IDX_CMPD_BBUF),			/* 7 */
	CM_PD_SCID_CMPD		= (CM_REC_IDX_CMPD_PD),				/* 8 */

	CM_PD_SCID_MAX		= (CM_REC_IDX_CMPD_PD + 1)
};

enum cm_sub_comp_bf_enum
{
	CM_PD_SCBF_EDD		= (0x01 << (CM_PD_SCID_EDD)),		/* 0x002 */
#if CM_CFG_USE_MRP
	CM_PD_SCBF_MRP		= (0x01 << (CM_PD_SCID_MRP)),		/* 0x004 */
#endif
#if CM_CFG_USE_GSY
	CM_PD_SCBF_GSY		= (0x01 << (CM_PD_SCID_GSY)),		/* 0x008 */
#endif
#if CM_CFG_USE_POF
	CM_PD_SCBF_POF		= (0x01 << (CM_PD_SCID_POF)),		/* 0x020 */
#endif
	CM_PD_SCBF_OHA		= (0x01 << (CM_PD_SCID_OHA)),		/* 0x040 */
	CM_PD_SCBF_CMPD		= (0x01 << (CM_PD_SCID_CMPD)),		/* 0x100 */

	/* special */
	CM_PD_SCBF_EDD_LINK_STATUS		= 0x0800
};

enum cm_pd_prm_state_enum
{
	CM_PD_PRM_STATE_WF_PREPARE		= 1,
	CM_PD_PRM_STATE_WORKING			= 2,
	CM_PD_PRM_STATE_WF_WRITE		= 3,
	CM_PD_PRM_STATE_WF_END			= 5,
	CM_PD_PRM_STATE_WF_COMMIT		= 6,
	CM_PD_PRM_STATE_WF_SC_WORKING	= 7
};

enum cm_pd_wait_for_enum
{
	CM_PD_PRM_DO_PREPARE		= 0x0001,
	CM_PD_PRM_DO_WRITE			= 0x0002,
	CM_PD_PRM_DO_END			= 0x0004,
	CM_PD_PRM_DO_COMMIT			= 0x0008,
	CM_PD_PRM_DO_FIRST_IND		= 0x0010,

	CM_PD_PRM_WF_PREPARE_CNF	= 0x0100,
	CM_PD_PRM_WF_WRITE_CNF		= 0x0200,
	CM_PD_PRM_WF_END_CNF		= 0x0400,
	CM_PD_PRM_WF_COMMIT_CNF		= 0x0800,
	CM_PD_PRM_WF_FIRST_IND		= 0x1000,

	CM_PD_PRM_WF_READ_CNF		= 0x2000,
	CM_PD_PRM_IS_RECLIST_WRITE	= 0x4000,
	CM_PD_PRM_reserved			= 0x8000
};

typedef struct cm_pd_prm_diag_tag {
	LSA_UINT16				channel_error_type;		/* PRM_WRITE: 0x0010; PRM_END: 0x000F, 0x0010 */
	LSA_UINT16				ext_channel_error_type;	/* PRM_WRITE: 0x8001; PRM_END: 0x8002 */
	LSA_UINT16				err_port_id;
	LSA_UINT32				err_index;
	LSA_UINT32				err_offset;
} CM_PD_PRM_DIAG_TYPE;

typedef CM_PD_PRM_DIAG_TYPE * CM_PD_PRM_DIAG_PTR_TYPE;

typedef struct cm_pd_prm_tag
{
	CM_CHANNEL_PTR_TYPE	channel;	/* backlink */
	LSA_UINT32			trace_idx;
	LSA_UINT8			state;		/* see cm_pd_prm_state_enum */
	LSA_BOOL			is_parameterized; /* true if at least once parameterized successfully */
	LSA_BOOL			is_reset_to_factory; /* is parameterized because of ResetToFactory */

	LSA_UINT8			occupant; /* see cm_pd_occupant_enum, see cm_pd_prm_begin_user() and cm_pdsv_occupant_set_reset() */
	LSA_UINT16			sv_ar_nr; /* used if CM_PD_OCCUPANT_SV, set on "begin", reset on "relinquish" */

	LSA_RESPONSE_TYPE	last_appl_ready_ind; /* response of last indication */

	LSA_UINT32			wait_cnf;	/* wait for current cm_pd_config_start() */
	LSA_UINT16			wait_cnf_next;

	LSA_UINT16			wait_for;	/* wait for current working command */
	LSA_UINT16			wait_sc;	/* wait for subcomponents with above command */
	LSA_UINT16			wait_sc_ind;/* wait for subcomponents first indication */

	LSA_RESPONSE_TYPE	response;	/* summary of subcomponents response, mapped to cm-err */

	LSA_UINT16			arp;		/* application ready pending, if sc_write.cnf(-) or sc_end.cnf(-) */
	CM_PORT_UINT		wait_first_ind[CM_PD_SCID_MAX];
									/* wait for first indication of subcomponent and port */
									/* every written port must fire one indication */
									/* bit = ((CM_PORT_UINT)1 << port_id) */

	CM_UPPER_RQB_PTR_TYPE		pd_config_rqb;	/* internal, used by cm_pd_config_xxx() */

	CM_PD_RECORD_PTR_TYPE		write;
	CM_UPPER_RQB_PTR_TYPE		write_nonprm;	/* keep upper requests */

	CM_UPPER_RQB_PTR_TYPE		end;

	CM_UPPER_RQB_PTR_TYPE	read;

	CM_PD_PORT_PTR_TYPE		port; /* NULL .. Prm-ALL; else Prm-Port, see CM_PD_CONFIG_PRM_SV */

} CM_PD_PRM_TYPE;

typedef CM_PD_PRM_TYPE * CM_PD_PRM_PTR_TYPE;

typedef CM_PD_PRM_TYPE const * CM_PD_PRM_CONST_PTR_TYPE;

typedef struct cm_pd_reclist_tag
{
	LSA_BOOL										is_valid;				/* LSA_TRUE: if not in prm sequence   LSA_FALSE: between PrmBegin and PrmEnd */
	LSA_BOOL										has_error;				/* LSA_TRUE: if error occured during user prm sequence */
	LSA_UINT32										faulty_record_index;	/* first faulty record index */
	CM_PD_PORT_IF_PRM_DATA_TYPE						prm_data;
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	regular_list;		/* normal records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	empty_list;			/* empty parameterization */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	rtf_list;			/* ResetToFactory */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	mixin_list;			/* locally written, mixin to remote records */

	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE				mtd;				/* parsed PDMasterTailorData */
/* must not keep
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE				itd;				// parsed PDInstanceTailorData */
	LSA_BOOL										is_running_mpa;		/* LSA_TRUE: a PDInstanceTailorData is 'running' */
	CM_UPPER_RQB_PTR_TYPE							mpa_rb;				/* if is_running_mpa stored PDInstanceTailorData RQB, else LSA_NULL */
} CM_PD_RECLIST_TYPE;

typedef CM_PD_RECLIST_TYPE * CM_PD_RECLIST_PTR_TYPE;


enum cm_pd_iter_pdev_enum {

	CM_PD_ITER_PDEV_IDLE = 0,

	/* PDevData (0xF831) */
	CM_PD_ITER_PDEV_BlockHeaderPDev,
	CM_PD_ITER_PDEV_IRData,
	CM_PD_ITER_PDEV_PDSyncDataClock,
	CM_PD_ITER_PDEV_PDIRSubframeData,
	CM_PD_ITER_PDEV_PDevDataComplete,

	/* PDRealData (0xF841) */
	CM_PD_ITER_PDEV_MultipleBlockHeaderReal,
	CM_PD_ITER_PDEV_PDPortDataReal,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataReal,
	CM_PD_ITER_PDEV_PDPortMrpDataReal,
	CM_PD_ITER_PDEV_PDPortFODataReal,
	CM_PD_ITER_PDEV_PDInterfaceDataReal,
	CM_PD_ITER_PDEV_PDPortStatistic,
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
	CM_PD_ITER_PDEV_PDPortMrpIcDataReal,
#endif
	CM_PD_ITER_PDEV_PDRealDataComplete,
	CM_PD_ITER_PDEV_PDRealDataInvalidated,

	/* PDExpectedData (0xF842) */
	CM_PD_ITER_PDEV_MultipleBlockHeaderExpected,
	CM_PD_ITER_PDEV_PDPortDataCheck,
	CM_PD_ITER_PDEV_PDPortDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataCheck,
	CM_PD_ITER_PDEV_PDPortMrpDataAdjust,
	CM_PD_ITER_PDEV_PDPortFODataAdjust,
	CM_PD_ITER_PDEV_PDPortFODataCheck,
	CM_PD_ITER_PDEV_PDNCDataCheck,
	CM_PD_ITER_PDEV_PDInterfaceFSUDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceAdjust,
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
	CM_PD_ITER_PDEV_PDPortMrpIcDataAdjust,
	CM_PD_ITER_PDEV_PDPortMrpIcDataCheck,
#endif
	CM_PD_ITER_PDEV_PDExpectedDataComplete,
	CM_PD_ITER_PDEV_PDExpectedDataInvalidated,

	/* ARData */
	CM_PD_ITER_PDEV_ARData_Setup,
	CM_PD_ITER_PDEV_ARData_GetAPDUStatus,
	CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done
};

typedef struct cm_pd_iter_tag {

	struct { /* similar to CM_SV_ITERATOR_TYPE::pdev */

		LSA_UINT16 state; /* see cm_pd_iter_pdev_enum */

		CM_PD_PRM_RDWR_TYPE org; /* original request */

		LSA_UINT32 offs_tot;
		LSA_UINT32 offs_blk;

	} pdev;

	LSA_UINT16 port_id;

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;

} CM_PD_ITER_TYPE;

typedef CM_PD_ITER_TYPE * CM_PD_ITER_PTR_TYPE;

typedef struct cm_pd_multi_write_tag
{
	LSA_BOOL  is_running;
	LSA_BOOL  all_records_done;
	LSA_BOOL  mk_remanent;
	LSA_BOOL  is_local;
	CM_UPPER_RQB_PTR_TYPE  internal_rqb;
	CM_UPPER_MEM_U8_PTR_TYPE  data;
	LSA_UINT32  data_length;
	LSA_UINT32  data_offs;
	LSA_RESPONSE_TYPE  rsp; /* summary of subcomponents response, mapped to cm-err */

} CM_PD_MULTI_WRITE_TYPE;

typedef CM_PD_MULTI_WRITE_TYPE * CM_PD_MULTI_WRITE_PTR_TYPE;

typedef struct cm_pd_rtf_tag
{
	LSA_UINT32	wf_sm;		/* ResetToFactory, NoS or IPSuite changed - wait_for - statemachine, see CM_PD_RTF_WF__xxx */
} CM_PD_RTF_TYPE;

typedef CM_PD_RTF_TYPE * CM_PD_RTF_PTR_TYPE;

#define CM_PD_WF_PULLPLUG_ALARM_RES  0x00000001

typedef struct cm_pd_pullplug_tag
{
	LSA_UINT8				wait_for;
	LSA_UINT8				wait_done;
	LSA_UINT16				wait_sc;
	CM_UPPER_RQB_PTR_TYPE	alarm;
	CM_PD_PORT_PTR_TYPE		port;
} CM_PD_PULLPLUG_TYPE;

typedef CM_PD_PULLPLUG_TYPE * CM_PD_PULLPLUG_PTR_TYPE;

typedef struct cm_pd_channel_tag {
	/*
	 * the states of the state machine CM-PD
	 * are defined in cm_int.h "in-line" with
	 * the states of CM-CH
	 */

	struct {
		CM_LIST_TYPED_ENTRY_TYPE(cm_rqb_tag)  queue;
		CM_UPPER_RQB_PTR_TYPE  current;
		CM_PD_ITER_TYPE  iter;
		LSA_BOOL   is_busy;
		LSA_UINT16  alarm_ind_count;/* CM_OPC_PD_ALARM_IND */
		LSA_UINT32  wait_for;
		CM_PD_MULTI_WRITE_TYPE  multi_write;

		CM_LIST_TYPED_ENTRY_TYPE(cm_rqb_tag)  eventQ;	/* pool of notification resources */
		CM_LIST_TYPED_ENTRY_TYPE(cm_rqb_tag)  alarmQ;	/* pool of alarm resources */
	} req;

	/***/

	LSA_UINT16			port_count;
	CM_PD_PORT_PTR_TYPE	port_array_ptr;	/* pointer to array of ports (interface is port_id = 0) */ /* 1 <= port_id <= port_count */

	struct {
		LSA_UINT16  send_clock;     /* current SendClockFactor */
		LSA_UINT16  ar_cnt;         /* number of ARs using this send-clock */
		LSA_BOOL    is_pending;     /* LSA_TRUE: wait for set-send-clock.CNF */
		LSA_UINT16  sendclock_prop; /* see cm_sendclock_prop_enum */
	} clock;

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red;

	CM_PD_PRM_TYPE  pdprm;

	CM_PD_RECLIST_TYPE  reclist;

	struct {
#if CM_CFG_MAX_SERVERS
		LSA_BOOL				server_allowed;
		LSA_UINT16				server_max_alarm_data_length;
		CM_UPPER_RQB_PTR_TYPE	server_alarm; /* prealloced alarm rqb */
#endif

		LSA_BOOL	user_allowed;
		LSA_UINT16	user_sequence_nr;

		LSA_UINT16	appl_ready_ind_pending; /* appl ready indication waiting for resources */
		LSA_BOOL	appl_ready_rsp_pending; /* appl ready rsp is pending */
	} almi;

	struct {
		CM_LIST_ENTRY_TYPE led_info_list;

		LSA_BOOL tell_oha; /* LSA_TRUE .. must tell OHA the new sync state */
		LSA_BOOL oha_ptcp_write_pending;

	} sync;

	struct {
		LSA_UINT16	server_id;			/* server-id of registered clrpc-endpoint-mapper */

		LSA_UINT16  im5_record_data_length; /* see I&M5 */
		CM_UPPER_MEM_U8_PTR_TYPE  im5_record_data;
		LSA_BOOL	oem_is_siemens;		/* is this a Siemens PN-Stack? */
	} rpc;

	CM_PD_RTF_TYPE		rtf; /* ResetToFactory, NoS or IPSuite changed */

	CM_PD_PULLPLUG_TYPE	pullplug; /* Submodule pull and plug */

} CM_PD_CHANNEL_TYPE;

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_request (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_BOOL
cm_pdusr_is_ready (
	CM_CHANNEL_PTR_TYPE usr_channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_req_queue_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_req_queue_next (
	CM_CHANNEL_PTR_TYPE  channel
);

CM_UPPER_RQB_PTR_TYPE
cm_pd_req_queue_remove_current (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_pd_req_callback (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  response,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_delete(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_rpc_epm_start_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16 rpc_response
);

LSA_VOID
cm_pd_rpc_epm_stop_done (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pd_oha_set_ident_done(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 oha_response
);

LSA_VOID
cm_pd_get_port_params_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_pdusr_is_parameterized (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 *send_clock,
	LSA_BOOL   *is_reduction_ratio_fixed
);

LSA_UINT16
cm_pdusr_is_send_clock_fixed ( /* returns the send_clock if "fixed", 0 otherwise */
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_BOOL *is_reduction_ratio_fixed
);

CLRPC_UUID_PTR_TYPE
cm_pdusr_get_ir_data_uuid (
	CM_CHANNEL_PTR_TYPE  usr_channel
);

LSA_UINT16
cm_pdusr_attach (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 send_clock
);

LSA_VOID
cm_pdusr_detach (
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL
cm_pdusr_is_pdev_stable ( /* true if user is allowed to start up */
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL
cm_pdusr_rtf_is_running(
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL
cm_pdusr_is_any_port_irt_run(
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL
cm_pdusr_exists_im5_record (
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL
cm_pdusr_get_im5_record (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT32  max_im5_rec_data_length,
	LSA_UINT16 * written_length,
	CM_COMMON_MEM_U8_PTR_TYPE  im5_rec_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_port_init (
	CM_PD_PORT_PTR_TYPE port
);

LSA_VOID
cm_pd_port_undo_init (
	CM_PD_PORT_CONST_PTR_TYPE port
);

CM_PD_PORT_PTR_TYPE
cm_pd_port_from_id (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 port_id
);

CM_PD_PORT_PTR_TYPE
cm_pd_port_from_id__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 port_id
);

CM_PD_PORT_PTR_TYPE
cm_pd_port_if__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

CM_PD_PORT_PTR_TYPE
cm_pd_port_from_addr (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

CM_PD_PORT_PTR_TYPE
cm_pd_port_from_addr__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_pd_im0_bits_ok (
	LSA_UINT8  im0_bits,
	LSA_BOOL  has_im5_record
);

LSA_UINT16
cm_pd_port_addr_map_check (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  map_count,
	CM_UPPER_PD_MAP_PTR_TYPE  map_array,
	LSA_BOOL  has_im5_record
);

LSA_VOID
cm_pd_port_addr_map (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  port_id,
	CM_UPPER_PD_MAP_CONST_PTR_TYPE  map
);

LSA_VOID
cm_pd_port_addr_unmap (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  port_id
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_link_status_changed (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT8  status,
	LSA_UINT8  prev_status
);


LSA_VOID
cm_pd_link_status_trigger_user (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  port_id
);

/*----------------------------------------------------------------------------*/

#define CM_PD_RTF_WF__START_DONE			0x0001
#define CM_PD_RTF_WF__IS_RTF				0x0002
#define CM_PD_RTF_WF__IS_NOS_IP				0x0004

#define CM_PD_RTF_WF__SV_DOWN				0x0010
#define CM_PD_RTF_WF__CL_DOWN				0x0020
#define CM_PD_RTF_WF__MC_DOWN				0x0040

#define CM_PD_RTF_WF__DO_DELAYED_RTF		0x0100

LSA_VOID
cm_pd_rtf_nos_ip_changed (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
);

LSA_VOID
cm_pd_rtf_nos_ip_changed_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  wf_done
);

LSA_BOOL
cm_pdusr_is_reset_to_factory (
	CM_CHANNEL_PTR_TYPE usr_channel
);

/*----------------------------------------------------------------------------*/

LSA_UINT16		/* CM_OK | CM_OK_PENDING | CM_ERR_xxx */
cm_pd_send_clock_set (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock,
	LSA_UINT16  sendclock_prop
);

LSA_VOID
cm_pd_set_send_clock_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_response,
	LSA_UINT16 send_clock
);

LSA_UINT16
cm_pd_sendclock_prop_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
);

LSA_BOOL
cm_pd_sendclock_prop_sc_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_BOOL
cm_pd_sendclock_prop_rr_is_fixed (
	CM_CHANNEL_CONST_PTR_TYPE  channel
);


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_prm_read_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_prm_read_internal(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_UINT32
cm_pd_queue_iterator_next(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_UINT16
cm_pd_prm_read_PDIOSystemInfo (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  edd_port_id,
	LSA_UINT32 *record_data_length,
	CM_UPPER_MEM_PTR_TYPE  record_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_prm_begin_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_prm_write_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_server /* true if CM_OPC_PD_PRM_WRITE_SERVER */
);


LSA_VOID
cm_pd_prm_write_nonprm (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_prm_write_PDInstanceTailorData (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_prm_write_PDInstance_TailorData_trigger (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_pd_prm_read_StartupStatisticsData (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32 *record_data_length,
	CM_UPPER_MEM_PTR_TYPE  record_data
);

LSA_UINT16
cm_pd_prm_write_StartupStatisticsData (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pd_prm_write_nonprm_done (
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_prm_write_multiple(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_prm_end_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_prm_diag_add (
	CM_PD_PRM_CONST_PTR_TYPE	pdprm,
	CM_PD_PRM_DIAG_PTR_TYPE		prm_diag
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_prm_write_multiple_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_RESPONSE_TYPE  response
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_opc_appl_ready_rsp(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_opc_alarm_rsp(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_pd_opc_sync_led_info(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_opc_link_status_info (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_sc_map_response (
	LSA_UINT16			lsa_comp_id,
	LSA_UINT16			sc_response
	);

LSA_UINT32
cm_pd_response_to_pnio_err (		/* if CM_OK: ret CM_PNIO_ERR_NONE  else: see CM_PNIO_ERR_MAKE() */
	LSA_RESPONSE_TYPE  rsp,			/* see cm_response_enum */
	LSA_BOOL  is_read
	);

LSA_VOID
cm_pd_prm_init (
	CM_CHANNEL_PTR_TYPE		channel,
	CM_PD_PRM_PTR_TYPE		pdprm
	);

LSA_VOID
cm_pd_prm_undo_init (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

LSA_UINT16		/* CM_ERR_xxx:sync-error  CM_OK_PENDING:wait-for-lower-cnf */
cm_pd_prm_sc_read (
	CM_PD_PRM_PTR_TYPE		pdprm,
	CM_UPPER_RQB_PTR_TYPE	read,
	LSA_UINT16				port_id
	);

LSA_VOID
cm_pd_prm_sc_read_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				response,
	LSA_UINT32				record_data_length
	);

/**/

LSA_VOID
cm_pd_prm_set_arp (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				rsp,
	LSA_UINT32				record_index,
	LSA_UINT16				wait_scbf_done
	);

/*----------------------------------------------------------------------------*/

CM_UPPER_PD_DIFF_AP_PTR_TYPE
cm_pd_diff_ap_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) *head,
	LSA_UINT32  api
);

CM_UPPER_PD_DIFF_MODULE_PTR_TYPE
cm_pd_diff_module_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) *head,
	LSA_UINT16  slot_nr,
	LSA_UINT32  mod_ident
);

CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE
cm_pd_diff_submodule_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) *head,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  sub_state,
	LSA_UINT32  sub_ident
);

LSA_VOID
cm_pd_diff_list_free (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) const *head
);

/* --- */

LSA_VOID
cm_pd_difflist_to_moddiffblock (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  ar_graph_void_ptr, /* PD: LSA_NULL, CL: argr */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) const *diff_list,
	LSA_UINT16 *mod_diff_block_length,
	CM_UPPER_MEM_PTR_TYPE *mod_diff_block
);

LSA_VOID
cm_pd_diag_for_appl_ready( /* AP01427283 */
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 *diag_block_length,
	CM_UPPER_MEM_PTR_TYPE *diag_block
);


/*----------------------------------------------------------------------------*/
#define CM_PD_PULLPLUG__MAP				0x0001
#define CM_PD_PULLPLUG__REMAP			0x0002
#define CM_PD_PULLPLUG__PULL			0x0004
#define CM_PD_PULLPLUG__PLUG			0x0008

#define CM_PD_PULLPLUG__DO_PULL			0x0010

#define CM_PD_PULLPLUG__DO_PLUG_PREPARE	0x0020
#define CM_PD_PULLPLUG__DO_PLUG_COMMIT	0x0040

/* see usr.h
#define CM_PDEV_PROP_TYPE_MASK				0x01
#define CM_PDEV_PROP_TYPE_COMPACT			0x00
#define CM_PDEV_PROP_TYPE_MODULAR			0x01

#define CM_PDEV_PROP_STATE_MASK				0x02
#define CM_PDEV_PROP_STATE_PULLED			0x00
#define CM_PDEV_PROP_STATE_PLUGGED			0x02

#define CM_PDEV_PROP_CHANGE_PROFIENERGY		0x04

#define CM_PDEV_PROP_RESERVED_CREATE_MASK	0xFC
#define CM_PDEV_PROP_RESERVED_CONTROL_MASK	0xF9
*/

#define CM_PDEV_PROP_DEFAULT_MASK			0x80
#define CM_PDEV_PROP_DEFAULT_DISABLED		0x00
#define CM_PDEV_PROP_DEFAULT_ENABLED		0x80

LSA_VOID
cm_pd_submodule_control (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_edd_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
);

#ifdef CM_CFG_USE_MRP
LSA_VOID
cm_mrp_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
);
#endif

#ifdef CM_CFG_USE_GSY
LSA_VOID
cm_gsy_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
);
#endif

#ifdef CM_CFG_USE_POF
LSA_VOID
cm_pof_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
);
#endif

LSA_VOID
cm_oha_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
);

#if CM_CFG_MAX_SERVERS
LSA_VOID
cm_pd_pullplug_sv_pull_done (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pd_pullplug_sv_plug_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  state_ok
);
#endif

LSA_VOID
cm_pd_pullplug_map_remap (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  wait_done,
	LSA_UINT16  port_id /* start = 0 */
);

LSA_VOID
cm_pd_pullplug_sm_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,
	LSA_UINT8  action
);

LSA_VOID
cm_pd_pullplug_sm_trigger (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  wait_sc_done,
	LSA_UINT8  portparams_not_applicable
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS

LSA_BOOL
cm_pdsv_mapping_ok (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_BOOL
cm_pdsv_occupant_is_nobody ( /* called before cm_pdsv_occupant_set_reset() */
	CM_CHANNEL_PTR_TYPE sv_channel
);

LSA_VOID
cm_pdsv_occupant_set_reset (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_BOOL do_set /* true..set, false..reset */
);

LSA_VOID
cm_pd_prm_begin_server (
	CM_CHANNEL_PTR_TYPE pd_channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_prm_end_server (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_prm_relinquish_server (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_opc_sv_ar_alarm_pdev_group_rsp (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE alarm
);

LSA_BOOL
cm_pdsv_diag_maint_qual (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 *maint_status_ptr
);

CM_LIST_ENTRY_PTR_TYPE
cm_pdsv_diag_read (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_UINT16
cm_pdsv_get_interface_subslot_nr(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 * map_count /* see CM_PD_CREATE_TYPE::map_count */
);

LSA_BOOL
cm_pdsv_port_from_id(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16  port_id,
	LSA_UINT16* slot_nr,        /* out */
	LSA_UINT16* subslot_nr,     /* out */
	LSA_UINT32* mod_ident,      /* out */
	LSA_UINT32* sub_ident,      /* out */
	LSA_UINT8*  im0_bits,       /* out */
	LSA_UINT16* pdev_properties /* out */
);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_VOID
cm_pdcl_request(
	CM_CHANNEL_PTR_TYPE cl_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_BOOL
cm_pdcl_pdev_is_ready_for_schedule (
	CM_CHANNEL_PTR_TYPE  cl_channel
);

LSA_VOID
cm_pdcl_master_tailor_device_added (
	CM_CHANNEL_PTR_TYPE  cl_channel
);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_almi_init (
	CM_CHANNEL_PTR_TYPE	channel
);

LSA_VOID
cm_pd_almi_undo_init (
	CM_CHANNEL_PTR_TYPE	channel
);

LSA_VOID
cm_pd_alarm_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

CM_UPPER_RQB_PTR_TYPE
cm_pd_alarm_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
);

LSA_VOID
cm_pd_alarm_provide_event (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pd_alarm_user_enable (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  user_enable
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_event_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

CM_UPPER_RQB_PTR_TYPE
cm_pd_event_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_oha_write_ptcp_trigger(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CHANNEL_PTR_TYPE gsy_channel
);

LSA_VOID
cm_pd_oha_write_ptcp_data_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16 oha_response
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY
LSA_VOID
cm_pd_sync_state_changed(
	CM_CHANNEL_PTR_TYPE channel,
	CM_CHANNEL_PTR_TYPE gsy_channel /* current values in ->usr.gsy.xxx */
);
#endif

LSA_VOID
cm_pd_irt_port_status_changed(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id,
	LSA_UINT8 irt_port_status
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_user_diag_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_pd_user_diag_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/***/

LSA_VOID
cm_pd_diag_update_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

LSA_VOID
cm_pd_diag_update_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

LSA_VOID
cm_pd_diag_update_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_fill__nn (
	LSA_VOID_PTR_TYPE  pre_allocated,	/* 0 .. alloc  else .. &CM_PD_DIAG_TYPE */
	LSA_UINT16 diag_tag,				/* CM_PD_DIAG_TAG_ */
	LSA_UINT16 alarm_type,				/* 0 .. AlarmType from ChannelErrorType */
	LSA_UINT16 channel_properties,
	LSA_UINT16 channel_error_type,
	LSA_UINT16 ext_channel_error_type,
	LSA_UINT32 ext_channel_add_value
);

CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_add (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag
);

CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_lookup (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list,
	CM_PD_DIAG_CONST_PTR_TYPE  diag
);

LSA_VOID
cm_pd_diag_elem_remove (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag
);

LSA_BOOL
cm_pd_diag_elem_has_equal_add_value (
	CM_PD_DIAG_CONST_PTR_TYPE  diag_1,
	CM_PD_DIAG_CONST_PTR_TYPE  diag_2
);


LSA_VOID
cm_pd_diag_sc_update (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value
);

/***/

LSA_VOID
cm_pd_diag_list_clear (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list_to_clear,
	LSA_UINT16  diag_tag
);

LSA_VOID
cm_pd_diag_list_move (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list_from,
	CM_LIST_ENTRY_PTR_TYPE  diag_list_to
);

LSA_VOID
cm_pd_diag_maint_qual (
	CM_PD_PORT_PTR_TYPE  port,
	LSA_BOOL   *diag_status_ptr,
	LSA_UINT32 *maint_status_ptr
);

/***/

LSA_VOID
cm_pd_diag_usr_copy_to_user_diag (
	CM_CHANNEL_CONST_PTR_TYPE  channel
);

LSA_VOID
cm_pd_diag_sv_copy_to_server_diag (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_PD_PORT_CONST_PTR_TYPE  trigger_port
);

LSA_VOID
cm_pd_diag_sv_update_check (
	CM_CHANNEL_PTR_TYPE  channel
);

/***/

LSA_BOOL
cm_pd_diag_marshal_8000(
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_COMMON_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr,
	CM_LIST_ENTRY_CONST_PTR_TYPE ext_phys_diag_list,
	LSA_UINT16 index
);


/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_cmpd_prm_write (
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	CM_PD_PORT_CONST_PTR_TYPE  port,
	CM_UPPER_PD_PRM_RDWR_CONST_PTR_TYPE  prm_rdwr
);

LSA_VOID
cm_cmpd_sendclock_changed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
);

LSA_VOID
cm_pd_prm_set_prm_port_flags (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT8  *port_flags,
	LSA_UINT32  size,
	LSA_UINT8  port_flag_size,
	LSA_UINT8  flag
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_edd_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID
cm_pd_edd_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID
cm_pd_edd_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID
cm_pd_edd_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID
cm_pd_edd_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	EDD_UPPER_PRM_INDICATION_PTR_TYPE ind
	);

LSA_VOID
cm_pd_edd_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID
cm_pd_edd_prm_link_status_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);



/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_MRP

LSA_VOID
cm_pd_mrp_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_MRP_LOWER_RQB_CONST_PTR_TYPE		mrp
	);

LSA_VOID
cm_pd_mrp_prm_write_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_MRP_LOWER_RQB_CONST_PTR_TYPE		mrp
	);

LSA_VOID
cm_pd_mrp_prm_end_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_MRP_LOWER_RQB_CONST_PTR_TYPE		mrp
	);

LSA_VOID
cm_pd_mrp_prm_commit_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_MRP_LOWER_RQB_CONST_PTR_TYPE		mrp
	);

LSA_VOID
cm_pd_mrp_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	MRP_PRM_INDICATION_PTR_TYPE	ind
	);

LSA_VOID
cm_pd_mrp_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

LSA_VOID
cm_pd_gsy_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_GSY_LOWER_RQB_CONST_PTR_TYPE		gsy
	);

LSA_VOID
cm_pd_gsy_prm_write_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_GSY_LOWER_RQB_CONST_PTR_TYPE		gsy
	);

LSA_VOID
cm_pd_gsy_prm_end_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_GSY_LOWER_RQB_CONST_PTR_TYPE		gsy
	);

LSA_VOID
cm_pd_gsy_prm_commit_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_GSY_LOWER_RQB_CONST_PTR_TYPE		gsy
	);

LSA_VOID
cm_pd_gsy_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	GSY_RQB_PRM_INDICATION_PROVIDE_TYPE const *ind
	);

LSA_VOID
cm_pd_gsy_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

LSA_VOID
cm_pd_gsy_ptcp_master_control_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_POF

LSA_VOID
cm_pd_pof_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_POF_LOWER_RQB_CONST_PTR_TYPE		pof
	);

LSA_VOID
cm_pd_pof_prm_write_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_POF_LOWER_RQB_CONST_PTR_TYPE		pof
	);

LSA_VOID
cm_pd_pof_prm_end_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_POF_LOWER_RQB_CONST_PTR_TYPE		pof
	);

LSA_VOID
cm_pd_pof_prm_commit_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_POF_LOWER_RQB_CONST_PTR_TYPE		pof
	);

LSA_VOID
cm_pd_pof_prm_indication_done (
	CM_CHANNEL_PTR_TYPE					channel,
	POF_UPPER_PRM_INDICATION_PTR_TYPE	ind
	);

LSA_VOID
cm_pd_pof_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_oha_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_OHA_LOWER_RQB_CONST_PTR_TYPE		oha
	);

LSA_VOID
cm_pd_oha_prm_write_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_OHA_LOWER_RQB_CONST_PTR_TYPE		oha
	);

LSA_VOID
cm_pd_oha_prm_end_done (
	CM_CHANNEL_PTR_TYPE					channel,
	CM_OHA_LOWER_RQB_CONST_PTR_TYPE		oha
	);

LSA_VOID
cm_pd_oha_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_CONST_PTR_TYPE	oha
	);

LSA_VOID
cm_pd_oha_prm_indication_done (
	CM_CHANNEL_PTR_TYPE						channel,
	OHA_UPPER_PRM_INDICATION_PTR_TYPE		ind
	);

LSA_VOID
cm_pd_oha_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

/*----------------------------------------------------------------------------*/
/* PDMasterTailorData */

#define CM_PD_CONFIG_START				0x00FF
#define CM_PD_CONFIG_PRM_USR			0x0001

#if CM_CFG_MAX_SERVERS
#define CM_PD_CONFIG_PRM_SV				0x0002
#endif

#define CM_PD_CONFIG_NOS_IP				0x0004
#define CM_PD_CONFIG_RESET_TO_FACTORY	0x0008
#define CM_PD_CONFIG_CL_DEVICE_ADD		0x0010
#define CM_PD_CONFIG_MACHINE_TAILOR		0x0020

/* starts a PDev parameterization */
LSA_VOID
cm_pd_config_start (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg
	);

LSA_VOID
cm_pd_config_trigger (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT32  wf_done
	);

LSA_VOID
cm_pd_config_callback (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT16  rsp,
	CM_UPPER_RQB_PTR_TYPE  rb
	);

/* callback */
LSA_VOID
cm_pd_config_done (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg,
	LSA_UINT16  rsp
	);

LSA_VOID
cm_pd_prepare_appl_ready (
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	LSA_RESPONSE_TYPE  rsp,
	CM_UPPER_RQB_CONST_PTR_TYPE  appl_ready_ind
	);

LSA_VOID
cm_pd_callback_appl_ready (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_RESPONSE_TYPE rsp
	);

LSA_BOOL
cm_pdusr_is_masterproject (
	CM_CHANNEL_PTR_TYPE  usr_channel
	);

LSA_VOID
cm_pd_reclist_init (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID
cm_pd_reclist_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID
cm_pd_reclist_prm_begin (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr /* 0 .. Connect/PrmUpdate(IF)  else .. Plug/PrmUpdate */
	);

LSA_VOID
cm_pd_reclist_alloc_records (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID
cm_pd_reclist_clear (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type
	);

LSA_UINT16
cm_pd_reclist_write (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type,
	CM_UPPER_RQB_CONST_PTR_TYPE  rb
	);

LSA_UINT16
cm_pd_reclist_add_default_rec (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	LSA_BOOL  is_prm_server,
	LSA_UINT8 set_mrp_off /* Possible value is CM_SET_MRP_OFF_NO or CM_SET_MRP_OFF_YES */
	);

LSA_UINT16
cm_pd_reclist_reset_to_factory (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_UINT16
cm_pd_reclist_add_ep_rec (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID
cm_pd_reclist_set_error (
	CM_CHANNEL_PTR_TYPE  channel
);

#define CM_PD_RECLIST_REGULAR  0	/* records for the usual or tailored parameterization */
#define CM_PD_RECLIST_EMPTY    1	/* records for the empty parameterization */
#define CM_PD_RECLIST_RTF      2	/* records for ResetToFactory parameterization*/
#define CM_PD_RECLIST_MIXIN    3	/* locally written, mixin to remote records */

CM_PD_RECORD_PTR_TYPE
cm_pd_reclist_get_next (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_RECORD_PTR_TYPE  rec,
	LSA_UINT16  list_type
	);

/*----------------------------------------------------------------------------*/

CM_PD_RECORD_PTR_TYPE
cm_pd_reclist_read (
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			list_type,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_reclist_delete ( /* TIA 1193013 */
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_list_record_append (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_LIST_ENTRY_PTR_TYPE		add_to_this_list,

	LSA_UINT32					api,
	LSA_UINT16					slot_nr,
	LSA_UINT16					subslot_nr,
	LSA_UINT32					record_index,
	LSA_UINT16					transfer_sequence,
	LSA_BOOL					mk_remanent,
	LSA_BOOL					is_local,
	LSA_UINT32					record_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE	record_data,

	CM_PD_RECORD_PTR_TYPE * rec_ptr_ptr /* out */
	);

LSA_VOID
cm_pd_list_record_free (
	CM_CHANNEL_CONST_PTR_TYPE		channel,
	CM_PD_RECORD_PTR_TYPE	rec
	);

#if CM_CFG_MAX_CLIENTS

LSA_UINT16
cm_pd_reclist_tailor (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_BOOL
cm_pd_reclist_validate_and_prealloc_tailoring_records (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_PD_PRM_DIAG_PTR_TYPE diag
	);

LSA_BOOL
cm_pd_pdev_is_ready_for_tailoring ( /* note: the pdev of a non series machine is always ready */
	CM_PD_PRM_CONST_PTR_TYPE  pdprm
	);

LSA_UINT16
cm_pd_mpa_prm_parse_record (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_PD_RECORD_CONST_PTR_TYPE  rec
	);

LSA_UINT16
cm_pd_mpa_prm_prealloc_tailor_data (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_PD_RECORD_PTR_TYPE  rec
	);

LSA_UINT16
cm_pd_mpa_prm_write_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE  mtd,
	CM_PD_RECORD_PTR_TYPE  rec,
	CM_PD_RECORD_CONST_PTR_TYPE  rec_8052,
	CM_COMMON_MEM_U8_PTR_TYPE  nos_cl,
	LSA_UINT16  nos_cl_len
	);

LSA_UINT8
cm_pd_mpa_put_port_name (
	CM_COMMON_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  properties, /* 0 or CM_PD_MTD_PEER_PROP_PORT_CODING */
	LSA_UINT16  subslot_nr,
	LSA_UINT16  slot_nr
	);

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE
cm_pd_mtd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  device_nr, /* backlink */
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data
	);

CM_PD_MTD_PORT_PTR_TYPE
cm_pd_mtd_lookup_mtdp (
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE  mtd,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
	);

CM_PD_MTD_PORT_PTR_TYPE
cm_pd_mtd_lookup_mtdp_from_index (
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE  mtd,
	LSA_UINT16  index /* 0 .. (port_cnt - 1) */
	);

LSA_BOOL
cm_pd_mtd_neighbour_exists (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE  mtd
	);

LSA_VOID
cm_pd_mtd_free (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE *mtd
	);

CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE
cm_pd_itd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data,
	LSA_UINT32  *pnio_err
	);

LSA_VOID
cm_pd_itd_free (
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE *itd
	);

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE
cm_pdusr_mpa_lookup_mtd (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16 device_nr
	);

CM_PD_MTD_PORT_PTR_TYPE
cm_pdusr_mpa_lookup_mtdp (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE *mtd_
	);

LSA_BOOL
cm_pdusr_mpa_tailor_frame_id (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  rtc3_frame_id
	);

LSA_VOID
cm_pd_mtdp_get_peer (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MTD_PORT_PTR_TYPE  mtdp
	);

#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_IPD_H */
