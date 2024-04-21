#ifndef CM_ARGR_H
#define CM_ARGR_H

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
/*  F i l e               &F: cm_argr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the AR - Graph                                             */
/*                                                                           */
/*****************************************************************************/

#define CM_IANA_PNIO_UDP_UNICAST_PORT_NBO  CM_HTON16(0x8892) /* network byte order! see PNIO 61158-6 IANA_PNIO_UDP_UNICAST_PORT (formerly a const in ACP) */

/*------------------------------------------------------------------------------
// CM-AR-COM state machine
//----------------------------------------------------------------------------*/

enum cm_ar_com_state_enum {

	CM_AR_COM_INIT = 0,
	CM_AR_COM_IDLE = 1, /* has io_ar */

	CM_AR_COM_READY = 5, /* unknown or up */
	CM_AR_COM_DOWN  = 6

	/* next state == IDLE, see cm_ar_com_idle() */
};


/*------------------------------------------------------------------------------
// CM-AR-CR-XXX states
//----------------------------------------------------------------------------*/

enum cm_ar_cr_state_enum {

	CM_AR_CR_ALLOCATED =  0, /* has mem */

	/*** provider, consumer and alarms ***/

	/* prov:  IDLE, ERROR, PASSIVE, UP */
	/* cons:  IDLE, ERROR, PASSIVE, RESET, WF_UP, UP, DOWN */
	/* alarm: IDLE, ERROR, UP */
	/* mprov: IDLE, ERROR, PASSIVE, UP */
	/* mcons: IDLE, ERROR, PASSIVE, RESET, WF_UP, UP, DOWN */

	/* note: transition PASSIVE -> ERROR is possible => use acp_id as trigger for remove */

	CM_AR_CR_IDLE      =  1, /* has frame-id or alarm-ref */
	CM_AR_CR_ERROR     =  3, /* error on add => skip when removing; error on control => cm_ar_com_down() */
	CM_AR_CR_PASSIVE   =  4, /* after adding */
	CM_AR_CR_RESET     =  6, /* consumer only: resetting to unknown-state */
	CM_AR_CR_WF_UP     =  7, /* consumer only: after activation and reset */
	CM_AR_CR_UP        =  8,
	CM_AR_CR_DOWN      =  9  /* consumer only: received a miss.ind */
};


/*------------------------------------------------------------------------------
// CM-CL-AR things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

# include "cm_iclar.h"

#endif


/*------------------------------------------------------------------------------
// CM-SV-AR things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS

# include "cm_isvar_sr.h"
# include "cm_isvar.h"

#endif


/*------------------------------------------------------------------------------
// ARCQ - fixed size list
//
//	e.g: ProvAct, ConsAct, ProvSetDS, FramePas, AlarmRem
//----------------------------------------------------------------------------*/

#define CM_ARCQ_LIST_SIZE  5

struct cm_arcq_entry_tag {
	LSA_UINT8   opcode;
	LSA_BOOL    is_startup;
	LSA_UINT16  para1;
};

typedef struct cm_arcq_entry_tag * CM_ARCQ_ENTRY_PTR_TYPE;

typedef struct cm_arcq_list_tag {
	LSA_UINT8  curr;
	LSA_UINT8  next;
	struct cm_arcq_entry_tag  entry[CM_ARCQ_LIST_SIZE];
} CM_ARCQ_LIST_TYPE;

typedef struct cm_arcq_list_tag * CM_ARCQ_LIST_PTR_TYPE;


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_alarm_cr_tag * CM_AR_GRAPH_ALARM_CR_PTR_TYPE; /* forward */

typedef CM_ARCB_IR_INFO_TYPE * CM_AR_GRAPH_IR_INFO_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IR_INFO_SUB_TYPE * CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_SR_INFO_TYPE * CM_AR_GRAPH_SR_INFO_PTR_TYPE; /* only other mem-attrib */

/*------------------------------------------------------------------------------
// AR-Graph, RSInfoBlock
//----------------------------------------------------------------------------*/

typedef struct cm_argr_rs_info_tag {
	LSA_UINT16	rs_info_data_len;					/* 0 or length of rs_info_data */
	LSA_UINT8	rs_info_data[CM_REQ_RSINFO_SIZE];	/* RSInfoBlock */
} CM_AR_GRAPH_RS_INFO_TYPE;

typedef CM_AR_GRAPH_RS_INFO_TYPE * CM_AR_GRAPH_RS_INFO_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Head
//----------------------------------------------------------------------------*/

typedef struct cm_argr_ar_tag {
	LSA_UINT32			host_ip;			/* network byte order, client: IP of IOD; server: IP of IOC */

	LSA_UINT16			ar_type;			/* see defines CM_AR_TYPE_SINGLE, ... */
	CLRPC_UUID_TYPE		ar_uuid;			/* ARUUID */

	CM_MAC_TYPE			eth_mac;			/* client: CMResponderMacAdd; server: CMInitiatorMacAdd */
	CLRPC_UUID_TYPE		cmi_obj_uuid;		/* CMIObjectUUID */
	LSA_UINT32			ar_properties;		/* see defines CM_AR_PROP_STATE_MASK, ... */
	LSA_UINT16			cmi_timeout100ms;	/* CMIActivityTimeout */
	LSA_UINT16			cmi_udp_port;		/* network byte order, InitiatorUDPRTPort */
	LSA_UINT16			cmr_udp_port;		/* network byte order, ResponderUDPRTPort */

	LSA_UINT8			cmi_station_name_length; /* length of cmi_station_name */
	CM_COMMON_MEM_U8_PTR_TYPE
						cmi_station_name;	/* not null-terminated! */
} CM_AR_GRAPH_AR_TYPE;


typedef struct cm_ar_graph_tag {
	CM_LIST_ENTRY_TYPE  link;

	LSA_UINT32  trace_idx; /* for "LSA index trace" */

	LSA_UINT16	device_nr;
	LSA_UINT16	ar_nr;
	LSA_UINT16  session_key;

	/***/

	struct cm_ar_com_tag
	{
		CM_CHANNEL_PTR_TYPE  channel; /* see alloc */
		LSA_UINT8  path_type; /* copy of channel->detailptr->path_type, see alloc */

		LSA_UINT16 state; /* see cm_ar_com_state_enum */

		LSA_UINT16 ar_send_clock;
		LSA_BOOL   is_power_of_two;
		LSA_BOOL   has_udp; /* TRUE if ar contains iocrs or alarmcrs with upd-transport (AP00583478) */
		LSA_UINT16 autostop_cons_id; /* EDD_CONS_ID_INVALID or ConsumerID for EDD_PROV_CONTROL_MODE_AUTOSTOP */

		/*
		 * variables for CM-AR-COM
		 */

		LSA_UINT8   down; /* see cm_ar_reason_enum (see ACP_OPC_ALARM_CLOSE::pnio_status_reason) */
		LSA_UINT16  cycle_counter;
		LSA_UINT32  down_pnio_status; /* for AP00678183, see cm_acp_alarm_indication() */
		ACP_ERROR_INFO_DATA_TYPE  down_debug_data; /* see VendorDeviceErrorInfo */

		LSA_BOOL cr_is_up;
		LSA_BOOL cr_is_rir;

		/***/

		LSA_UINT16 user_apdu_status; /* data-status indicated to user */
	} ar_com;

	struct cm_arcq_tag
	{
		CM_ARCQ_LIST_TYPE  list; /* list of arcq-opcodes, see cm_arcq_request() */
		LSA_UINT8  entered; /* reentrance check */
		LSA_UINT8  curr_opc;
		LSA_BOOL   do_reset; /* TRUE .. reset running (defer 'up' until after cnf) */
		CM_ACP_LOWER_RQB_PTR_TYPE  acp_rqb; /* pre-allocated ACP-RQB */
		CM_EDD_LOWER_RQB_PTR_TYPE  edd_rqb; /* pre-allocated EDD-RQB */
	} arcq;

	/***/

	union cm_cl_sv_mc_tag {

		LSA_UINT8 nihil; /* to make union non-empty for "compile as pdev only" (AP00565264) */

#if CM_CFG_MAX_CLIENTS
		CM_CL_ARGR_TYPE cl;
#endif

#if CM_CFG_MAX_SERVERS
		CM_SV_ARGR_TYPE sv;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		struct {
			struct cm_mc_device_tag *dev; /* note: CM_MC_DEVICE_PTR_TYPE not visible */
		} mc;
#endif
	} cm;

	/* IODConnectReq */

	CM_AR_GRAPH_AR_TYPE				ar;			/* ARBlockReq */

	CM_LIST_ENTRY_TYPE				io_cr_list;	/* [IOCRBlockReq*] */
												/* list of: CM_AR_GRAPH_IO_CR_PTR_TYPE */

	CM_AR_GRAPH_ALARM_CR_PTR_TYPE	alarm_cr;	/* [AlarmCRBlockReq] */

	CM_LIST_ENTRY_TYPE				ap_list;	/* [ExpectedSubmoduleBlockReq*] */
												/* list of: CM_AR_GRAPH_AP_PTR_TYPE / CM_AR_GRAPH_MODULE_PTR_TYPE / CM_AR_GRAPH_SUBMODULE_PTR_TYPE */

	/* prm-server not yet						// [PrmServerBlock] */

	CM_LIST_ENTRY_TYPE				mcr_list;	/* [MCRBlockReq*] */

	CM_AR_GRAPH_IR_INFO_PTR_TYPE	ir_info;	/* [IRInfoBlock] */

	CM_AR_GRAPH_SR_INFO_PTR_TYPE	sr_info;	/* [SRInfoBlock] */

	CM_LIST_ENTRY_TYPE				arvendor_list;	/* [ARVendorBlockReq*] */
												/* list of: CM_AR_GRAPH_ARVENDOR_PTR_TYPE */

	CM_AR_GRAPH_RS_INFO_PTR_TYPE	rs_info;	/* [RSInfoBlock] */
} * CM_AR_GRAPH_PTR_TYPE;

typedef struct cm_ar_graph_tag  const * CM_AR_GRAPH_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// AR-Graph, IOCR (iodata, iocs, ioapi, iocr)
//----------------------------------------------------------------------------*/

/* general send order: CM-CL, then CM-SV, then CM-MC (see AP00542516) extended by AP01379821 (sequence 16->32 bits) */
/* CL: 0x1000dddd */
/* SV: 0x20ddaaaa */
/* MC: 0x3000dddd */
#define CM_AR_IOCR_SEQUENCE_CL_BASE		0x10000000
#define CM_AR_IOCR_SEQUENCE_CL_COUNT	0x0000ffff
#define CM_AR_IOCR_SEQUENCE_SV_BASE		0x20000000
#define CM_AR_IOCR_SEQUENCE_SV_COUNT	0x01000000 /* 24 bits */
#define CM_AR_IOCR_SEQUENCE_MC_BASE		0x30000000
#define CM_AR_IOCR_SEQUENCE_MC_COUNT	0x0000ffff


typedef CM_ARCB_IOCS_TYPE CM_AR_GRAPH_IOCS_TYPE; /* the same */
typedef CM_ARCB_IOCS_TYPE * CM_AR_GRAPH_IOCS_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IODATA_OBJECT_TYPE CM_AR_GRAPH_IODATA_OBJECT_TYPE; /* the same */
typedef CM_ARCB_IODATA_OBJECT_TYPE * CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IOAPI_TYPE CM_AR_GRAPH_IOAPI_TYPE; /* the same */
typedef CM_ARCB_IOAPI_TYPE * CM_AR_GRAPH_IOAPI_PTR_TYPE; /* only other mem-attrib */
typedef CM_ARCB_IOAPI_TYPE const * CM_AR_GRAPH_IOAPI_CONST_PTR_TYPE; /* only other mem-attrib */

typedef struct cm_ar_graph_io_cr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	struct {
		CM_AR_GRAPH_PTR_TYPE  backlink; /* backlink to the ar_graph */

		LSA_BOOL  is_consumer;  /* true if iocr_type is a consumer (iocr_type has view of controller!) */
		LSA_BOOL  is_multicast; /* true if iocr_type is a m-provider or m-consumer */

		LSA_UINT8  cr_state; /* see cm_ar_cr_state_enum, state machine CM-AR-CR-XXX */
		LSA_BOOL   is_rir;   /* for RTClass3: is received in red */

		/* buffer handling */

		LSA_UINT16  acp_id;

		LSA_UINT32  edd_properties;      /* for EDDI */
		LSA_UINT32  edd_data_offset;     /* for EDDI, EDDP */
		LSA_UINT32  edd_ds_offset;       /* for EDDx, DataStatusOffset */
		LSA_UINT16  edd_cr_number;       /* for EDDP, EDDS */
		LSA_UINT8   edd_forward_mode;    /* for EDDP */
		LSA_UINT32  edd_ct_outdatastart; /* for EDDP */

		LSA_UINT16  apdu_status_length; /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  partial_length;     /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  partial_offset;     /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  subframe_data;      /* see cm_ar_graph_check_ir_info_block(), cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  subframe_offset;    /* see cm_ar_graph_check_ir_info_block(), cm_ar_graph_check_iocrs_against_expsubmods() */

		LSA_UINT32  edd_cw_offset; /* pointer to ControlWord, only provider (see AP00618087)*/
		LSA_UINT32  edd_as_offset; /* pointer to APDUStatus, only consumer (see AP00630092) */

		/* local adaption */

		struct {
			LSA_UINT16	send_clock; /* 0..no adaption set, see cm_cl_ar_graph_set_iocr_adaption(), see cm_sv_ar_connect_indication_continued() */
			LSA_UINT16	reduction_ratio;
			LSA_UINT16	phase;
			LSA_UINT32	sequence; /* AP01379821 (16->32 bits) */
			LSA_UINT16  data_hold_factor;
		} adpt;

		LSA_UINT16 cons_data_status; /* cons: see cm_..._ind() prov: 0 */

#if CM_CFG_MAX_SERVERS
		struct {
			LSA_USER_ID_TYPE iocr_user_id;
#if CM_CFG_SV_AR_HAS_MCONS
			LSA_UINT16 data_status_ind; /* data_status indicated to the user */
#endif
			LSA_UINT16 sum_iodata; /* TIA 2612940 */
			LSA_UINT16 sum_ioxs;   /* TIA 2612940 */
		} sv;
#endif

	} cm;

	/***/

	LSA_UINT16			iocr_type;		/* see defines CM_IOCR_TYPE_RESERVED_0, ... */
	LSA_UINT16			iocr_ref;		/* IOCRReference */
	LSA_UINT16			ethertype;		/* LT */
	LSA_UINT32			iocr_properties;/* see defines CM_IOCR_PROP_RT_CLASS_MASK, ... */

	LSA_UINT16			data_length;
	LSA_UINT16			frame_id;

	LSA_UINT16			send_clock;
	LSA_UINT16			reduction_ratio;
	LSA_UINT16			phase;
	LSA_UINT32			frame_send_offset;

	LSA_UINT16			data_hold_factor;

	CM_MAC_TYPE			multicast_mac;

	/***/

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag) related_ap_list; /* list of CM_AR_GRAPH_IOAPI_TYPE */

} * CM_AR_GRAPH_IO_CR_PTR_TYPE;

typedef struct cm_ar_graph_io_cr_tag const * CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// AR-Graph, ALARM-CR
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_alarm_cr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	struct {
		CM_AR_GRAPH_PTR_TYPE  backlink; /* backlink to the ar_graph */

		LSA_UINT16  cr_state; /* see cm_ar_cr_state_enum, state machine CM-AR-CR-XXX */
	} cm;

	/***/

	LSA_UINT16			alcr_type;       /* see defines CM_ALCR_TYPE_RESERVED_0, ... */
	LSA_UINT32			alcr_properties; /* see defines CM_ALCR_PROP_PRIO_MASK, ... */

	LSA_UINT8			timeout100ms;
	LSA_UINT8			retries;

	LSA_UINT16			local_alarm_ref;

	LSA_UINT16			max_alarm_data_len; /* see acp-info */

	LSA_UINT16			ethertype; /* LT */

	/* rsp */
	LSA_UINT16			remote_alarm_ref;

} CM_AR_GRAPH_ALARM_TYPE;

/* typedef ... CM_AR_GRAPH_ALARM_CR_PTR_TYPE see above */

/*------------------------------------------------------------------------------
// AR-Graph, AP
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_ap_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	/***/

	LSA_UINT32				api; /* application process identifier */

	CM_LIST_ENTRY_TYPE		mod_list;

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_AP_PTR_TYPE*/ struct cm_sv_ap_tag * real_ap; /* pointer to the real config, see cm_sv_attach() */
	} sv;
#endif

} * CM_AR_GRAPH_AP_PTR_TYPE;

typedef struct cm_ar_graph_ap_tag const * CM_AR_GRAPH_AP_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// AR-Graph, Module
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_module_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	CM_AR_GRAPH_AP_PTR_TYPE  ap; /* parent */

	/***/

	LSA_UINT16				slot_nr;		/* Slot_Number */

	struct cm_arg_module_soll_tag {
		LSA_UINT32			mod_ident;
		LSA_UINT16			mod_properties;	/* see defines CM_MOD_PROP_UNCHECKED_0_7, ... */
	} soll;

#if CM_CFG_MAX_CLIENTS /* note: CM-SV uses sv.real_sl->mod_ident */
	struct cm_arg_module_ist_tag {
		LSA_UINT32			mod_ident;
	} ist;
#endif

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_SLOT_PTR_TYPE*/ struct cm_sv_slot_tag * real_sl; /* pointer to the real config, see cm_sv_attach() */
	} sv;
#endif

	CM_LIST_ENTRY_TYPE		submod_list;

} * CM_AR_GRAPH_MODULE_PTR_TYPE;

typedef struct cm_ar_graph_module_tag const * CM_AR_GRAPH_MODULE_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// AR-Graph, Submodule
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_submodule_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	CM_AR_GRAPH_MODULE_PTR_TYPE  mod; /* parent */

	/***/

	LSA_UINT16			subslot_nr;		/* ascending order */

	struct cm_arg_sub_soll_tag {
		LSA_UINT32		sub_ident;
		LSA_UINT16		sub_properties;	/* see defines CM_SUB_PROP_TYPE_MASK, ... */
	} soll;

	/* submodule data description */
	struct cm_argr_submodule_descr_tag {
		LSA_UINT16		data_description;	/* Data Description */
		LSA_UINT16		data_length;		/* Data Length, without IOPS */

		LSA_UINT8		iops_length;		/* Length IOPS 1..n */
		LSA_UINT8		iocs_length;		/* Length IOCS 1..m */

	} descr[2];								/* index: 0..input 1..output */

	LSA_BOOL	inp_via_mcr;	/* is a m-provider-submodule (inputs are written to m-provider cr) */
	LSA_BOOL	out_via_mcr;	/* is a m-consumer-submodule (outputs are read from m-consumer cr) */

#if CM_CFG_MAX_CLIENTS
	struct cm_arg_sub_cl_tag {
		LSA_UINT32			real_sub_ident;
		LSA_UINT16			real_sub_state;			/* see cm_sub_state_enum, CM-SV: see comment in "struct sv" below */
		LSA_UINT16			sub_coc_properties;		/* combined object container, see cm_sub_coc_properties_enum */

		CM_LIST_ENTRY_TYPE	submod_link;			/* link within prm.prm_submod_list */
		CM_LIST_ENTRY_TYPE	prm_record_list;		/* list for submodule-prm-records */
		CM_LIST_ENTRY_TYPE	prm_update_record_list;	/* list for updated submodule-prm-records */
	} cl;
#endif

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_SUBSLOT_PTR_TYPE*/ struct cm_sv_subslot_tag * real_su; /* pointer to the real config, see cm_sv_attach() */
		LSA_UINT16 sm_flags; /* see CM_SV_AR_SMFL_TNA, ... */
		LSA_UINT16 st_told;  /* submodule state that was sent to CM-CL */

		/* only for pdev-submodules */
		LSA_UINT16 err_flags;
		LSA_UINT32 err_index;
		LSA_UINT32 err_offset;
	} sv;
#endif

} * CM_AR_GRAPH_SUBMODULE_PTR_TYPE;

typedef struct cm_ar_graph_submodule_tag const * CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// AR-Graph, Multicast CR
//----------------------------------------------------------------------------*/
typedef struct cm_ar_graph_mcr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT16		iocr_ref;
	LSA_UINT32		mcr_properties;	/* see defines CM_MCR_PROP_PROTOCOL_MASK, ... */
	LSA_UINT16		mci_timeout_100ms;
	LSA_UINT8		provider_name_length;
	CM_UPPER_MEM_U8_PTR_TYPE
					provider_name;

} * CM_AR_GRAPH_MCR_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, ARVendorBlock
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_arvendor_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT32		api;
	LSA_UINT16		ap_structure_identifier;
	LSA_UINT16		req_data_len;
	LSA_UINT16		res_data_len;

	LSA_UINT8		data[1];
	/*** nothing here, dynamic size ***/

} * CM_AR_GRAPH_ARVENDOR_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Unique Slot_nr
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_slot_nr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT16			slot_nr;
} * CM_AR_GRAPH_SLOT_NR_PTR_TYPE;


/*------------------------------------------------------------------------------
// utilities
//----------------------------------------------------------------------------*/

LSA_BOOL
cm_is_valid_port_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_CONST_PTR_TYPE name
);

LSA_BOOL
cm_is_valid_station_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_CONST_PTR_TYPE name
);

LSA_BOOL
cm_is_valid_alias_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_CONST_PTR_TYPE name
);

LSA_VOID
cm_station_name_copy (
	CM_COMMON_MEM_U8_PTR_TYPE to_station_name,
	CM_COMMON_MEM_U8_CONST_PTR_TYPE from_station_name,
	LSA_UINT16  station_name_length
);

LSA_BOOL
cm_is_iosar_with_device_access(
	CM_AR_GRAPH_CONST_PTR_TYPE ar_graph
);

LSA_BOOL
cm_is_frame_id_within_normative_range (
	LSA_BOOL				is_consumer,
	LSA_BOOL				is_multicast,
	LSA_UINT32				rt_class,
	LSA_UINT16				frame_id
);

/*------------------------------------------------------------------------------
// build the ar-graph
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_PTR_TYPE
cm_ar_graph_alloc(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
);

LSA_VOID
cm_ar_graph_free (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
);

LSA_VOID
cm_ar_graph_free_list_local_mem(
	CM_LIST_ENTRY_CONST_PTR_TYPE	list
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32				host_ip, /* network byte order */

	LSA_UINT16				ar_type,
	const CLRPC_UUID_TYPE * const ar_uuid,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmi_mac,

	CLRPC_UUID_CONST_PTR_TYPE		cmi_obj_uuid,

	LSA_UINT32				ar_properties,
	LSA_UINT16				cmi_timeout100ms,
	LSA_UINT16				cmi_udp_port, /* network byte order */

	LSA_UINT16/*!*/			cmi_station_name_length,

	CM_COMMON_MEM_U8_CONST_PTR_TYPE
							cmi_station_name
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS
LSA_BOOL
cm_ar_graph_update_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmr_mac,

	LSA_UINT16				cmr_udp_port /* network byte order */
);
#endif

#if CM_CFG_MAX_SERVERS
LSA_BOOL
cm_sv_ar_graph_check_io_ar(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);
#endif

LSA_VOID
cm_ar_graph_update_autostop_cons_id (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_ir_info (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CLRPC_UUID_CONST_PTR_TYPE		ir_data_uuid,
	LSA_UINT16				alloc_more /* for CM-SV, else 0 */
);

LSA_UINT16
cm_ar_graph_insert_ir_info_sub (
	CM_AR_GRAPH_CONST_PTR_TYPE	ar_graph,

	LSA_UINT16				iocr_ref,
	LSA_UINT16				subframe_offset,
	LSA_UINT32				subframe_data
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_ar_graph_reduction_ratio_range_ok (
	LSA_UINT16  reduction_ratio,
	LSA_BOOL  is_power_of_two,
	LSA_UINT32  rt_class
);

LSA_UINT16
cm_ar_graph_insert_io_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,

	LSA_UINT16  iocr_type,
	LSA_UINT16  iocr_ref,
	LSA_UINT16  ethertype,
	LSA_UINT32  iocr_properties,

	LSA_UINT16  data_length,
	LSA_UINT16  frame_id,

	LSA_UINT16  send_clock,
	LSA_UINT16  reduction_ratio,
	LSA_UINT16  phase,
	LSA_UINT32  frame_send_offset,

	LSA_UINT16  watchdog_factor_legacy, /* CM-CL/CM-MC: pass in data_hold_factor, CM-SV: pass in value from PDU (AP01304706) */
	LSA_UINT16  data_hold_factor,

	LSA_UINT16  iocr_tag_header, /* only for provider, don't care for consumer */

	CM_COMMON_MEM_U8_PTR_TYPE  mac_addr,

	CM_AR_GRAPH_IO_CR_PTR_TYPE *iocr_ptr_ptr /* out */
);

#if CM_CFG_MAX_CLIENTS  ||  CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT16
cm_cl_ar_graph_set_iocr_edd_data (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ptr,

	LSA_UINT32		edd_data_offset,
	LSA_UINT32		edd_properties,
	LSA_UINT16		edd_cr_number,
	LSA_UINT8		edd_forward_mode,
	LSA_UINT32		edd_ct_outdatastar,

	LSA_UINT16		partial_length,
	LSA_UINT16		partial_offset,
	LSA_UINT16		apdu_status_length,
	LSA_UINT16		subframe_data,
	LSA_UINT16		subframe_offset
);

LSA_UINT16
cm_cl_ar_graph_set_iocr_adaption (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT16		iocr_ref,
	LSA_UINT16		local_clock,

	LSA_UINT16		send_clock,
	LSA_UINT16		reduction_ratio,
	LSA_UINT16		phase,
	LSA_UINT32		frame_send_offset,
	LSA_UINT16		data_hold_factor
);

#endif

#if CM_CFG_MAX_CLIENTS
LSA_BOOL
cm_ar_graph_update_io_cr (
	CM_AR_GRAPH_CONST_PTR_TYPE	ar_graph,

	LSA_UINT16			iocr_type,
	LSA_UINT16			iocr_ref,
	LSA_UINT16			frame_id
);
#endif

LSA_UINT16
cm_argr_insert_ioap (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,

	LSA_UINT32		api,

	CM_AR_GRAPH_IOAPI_PTR_TYPE *ioapi_ptr_ptr /* out */
);

LSA_UINT16
cm_argr_insert_iodata_objects (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE *iodata_ptr_ptr
);

LSA_UINT16
cm_argr_insert_iocs (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IOCS_PTR_TYPE *iocs_ptr_ptr
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_alarm_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16				alcr_type,
	LSA_UINT16				ethertype,
	LSA_UINT32				alcr_properties,

	LSA_UINT16				timeout100ms,
	LSA_UINT16				retries,

	LSA_UINT16				alarm_ref,
	LSA_UINT16				max_alarm_data_length,

	LSA_UINT16				alcr_tag_header_high,
	LSA_UINT16				alcr_tag_header_low,

	CM_AR_GRAPH_ALARM_CR_PTR_TYPE *alcr_ptr_ptr /* out */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_ap_module (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32	api,

	LSA_UINT16	slot_nr,
	LSA_UINT32	mod_ident,
	LSA_UINT16	mod_properties,

	LSA_UINT16  const *exp_slot_nr_ptr, /* LSA_NULL or expected slot_nr */

	CM_AR_GRAPH_AP_PTR_TYPE *ap_ptr_ptr, /* out */
	CM_AR_GRAPH_MODULE_PTR_TYPE *mod_ptr_ptr /* out */
);

LSA_UINT16
cm_ar_graph_insert_submodule (
	CM_AR_GRAPH_CONST_PTR_TYPE   argr,
	CM_AR_GRAPH_MODULE_PTR_TYPE  module,

	LSA_UINT16		subslot_nr,
	LSA_UINT32		sub_ident,
	LSA_UINT16		sub_properties,
	LSA_BOOL		IOXS_Required,

	CM_AR_GRAPH_SUBMODULE_PTR_TYPE *sub_ptr_ptr /* out */
);

LSA_UINT16
cm_ar_graph_set_data_descr (
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  submod,

	LSA_UINT16		direction, /* 0..input, 1..output, 2..use-description */

	LSA_UINT16		data_description,
	LSA_UINT16		data_length,
	LSA_UINT8		iops_length,
	LSA_UINT8		iocs_length
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_mcr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16		iocr_ref,
	LSA_UINT32		mcr_properties,	/* see defines CM_MCR_PROP_PROTOCOL_MASK, ... */
	LSA_UINT16		mci_timeout_100ms,
	LSA_UINT16/*!*/	provider_name_length,
	CM_UPPER_MEM_U8_CONST_PTR_TYPE				provider_name,
	CM_AR_GRAPH_MCR_PTR_TYPE *mcr_ptr_ptr	/* out */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_sr_info (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16	rdh_factor,
	LSA_UINT32	sr_properties,
	LSA_UINT16	alloc_more /* for CM-SV, else 0 */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_ar_graph_insert_arvendor (
	CM_AR_GRAPH_PTR_TYPE  argr,

	LSA_UINT32					api,
	LSA_UINT16					ap_structure_identifier,
	LSA_UINT16					req_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE	req_data,
	LSA_UINT16					res_alloc_len
);

CM_AR_GRAPH_ARVENDOR_PTR_TYPE
cm_ar_graph_find_arvendor (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	LSA_UINT32  api,
	LSA_UINT16  ap_structure_identifier
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_ar_graph_insert_rs_info (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_COMMON_MEM_U8_PTR_TYPE  rs_info_block,
	LSA_UINT32  rs_info_block_len,
	LSA_UINT16  *block_field
);

/*----------------------------------------------------------------------------*/

LSA_UINT32
cm_ar_graph_check_consistency(
	CM_AR_GRAPH_PTR_TYPE ar_graph
);

LSA_UINT16
cm_ar_graph_check_iocr_against_mcr (
	CM_AR_GRAPH_CONST_PTR_TYPE ar_graph
);
LSA_UINT16
cm_ar_graph_check_mcr_against_iocr (
	CM_AR_GRAPH_CONST_PTR_TYPE ar_graph
);


/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_UINT32
cm_cl_ar_graph_check_coc_properties (
	CM_AR_GRAPH_CONST_PTR_TYPE ar_graph
);

#endif


/*------------------------------------------------------------------------------
// find within the ar_graph
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_IO_CR_PTR_TYPE
cm_ar_graph_find_io_cr (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT16  io_cr_ref
);

CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE
cm_ar_graph_find_ir_info_sub (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
);

CM_AR_GRAPH_IOAPI_PTR_TYPE
cm_ar_graph_find_ioapi (
	CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE  iocr,
	LSA_UINT32  api
);

CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE
cm_ar_graph_find_iodata (
	CM_AR_GRAPH_IOAPI_CONST_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

CM_AR_GRAPH_IOCS_PTR_TYPE
cm_ar_graph_find_iocs (
	CM_AR_GRAPH_IOAPI_CONST_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

CM_AR_GRAPH_ALARM_CR_PTR_TYPE
cm_ar_graph_find_alarm_cr (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph
);

CM_AR_GRAPH_AP_PTR_TYPE
cm_ar_graph_find_ap (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT32  api
);

CM_AR_GRAPH_MODULE_PTR_TYPE
cm_ar_graph_find_module (
	CM_AR_GRAPH_AP_CONST_PTR_TYPE  ar_ap,
	LSA_UINT16  slot_nr
);

CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_ar_graph_find_submodule (
	CM_AR_GRAPH_AP_CONST_PTR_TYPE  ar_ap,
	CM_AR_GRAPH_MODULE_PTR_TYPE  ar_mod, /* in: mod or slot_nr must specified */
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

LSA_VOID
cm_ar_graph_find_api_slot_subslot (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_AR_GRAPH_AP_PTR_TYPE * ap_pp,
	CM_AR_GRAPH_MODULE_PTR_TYPE * mod_pp,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE * sub_pp
);

CM_AR_GRAPH_MCR_PTR_TYPE
cm_ar_graph_find_mcr (
	CM_AR_GRAPH_CONST_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
);

/*------------------------------------------------------------------------------
// ar-graph helper
//----------------------------------------------------------------------------*/

#if CM_DEBUG

LSA_VOID
cm_ar_graph_dump (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
);

#endif


/*------------------------------------------------------------------------------
// arcb helper
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_VOID
cm_arcb_device_add_set_iocr_cnf (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE  cb_ar
	);

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ARGR_H */
