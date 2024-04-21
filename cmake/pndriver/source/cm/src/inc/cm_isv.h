#ifndef CM_ISV_H
#define CM_ISV_H

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
/*  F i l e               &F: cm_isv.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - server things                                     */
/*                                                                           */
/*****************************************************************************/

struct cm_sv_device_tag; /* forward (for iterator) */

/*===========================================================================*/
/*=== CANDIDATE TYPE ========================================================*/
/*===========================================================================*/

typedef struct cm_sv_candidate_tag {

	LSA_UINT16 ar_nr;

	LSA_BOOL no_takeover;
	LSA_BOOL is_supervisor;

} CM_SV_CANDIDATE_TYPE;

/*===========================================================================*/
/*=== DIAG TYPE =============================================================*/
/*===========================================================================*/

typedef struct cm_sv_diag_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_diag_tag) link; /* first in struct */

	LSA_BOOL   r1_mirrored;

	LSA_UINT32 diag_tag;

	LSA_UINT16 channel_number;
	LSA_UINT16 channel_properties;
	LSA_UINT16 channel_error_type_usi; /* ChannelErrorType or UserStructureIdentifier */

	LSA_UINT16 data_length;

	/* dynamically allocated data[] follows */

} CM_SV_DIAG_TYPE;

typedef CM_SV_DIAG_TYPE *CM_SV_DIAG_PTR_TYPE;

/*===========================================================================*/
/*=== SUBSLOT TYPE ==========================================================*/
/*===========================================================================*/

#define CM_SV_SUBSLOT_IS_PDEV(subslot_nr)  (((subslot_nr) & 0xE000) == 0x8000) /* a pdev-submodule (0x8ipp/0x9jpp), see cm_sv_subslot_range_ok() */

#define CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr)  (CM_SV_SUBSLOT_IS_PDEV(subslot_nr) && ((subslot_nr) & 0xFF00) == (dev)->val_8i00) /* a local pdev-submodule, see cm_sv_subslot_range_ok() */

#define CM_SV_SUBSLOT_IS_PDEV_REMOTE(dev, subslot_nr)  (CM_SV_SUBSLOT_IS_PDEV(subslot_nr) && ((subslot_nr) & 0xFF00) != (dev)->val_8i00) /* a remote pdev-submodule */

#define CM_SV_SUBSLOT_IS_PDEV_INTERFACE(subslot_nr)  (CM_SV_SUBSLOT_IS_PDEV((subslot_nr)) && ((subslot_nr) & 0x00FF) == 0x0000) /* a pdev-interface */

#define CM_SV_SUBSLOT_IS_PDEV_PORT(subslot_nr)  (CM_SV_SUBSLOT_IS_PDEV((subslot_nr)) && ((subslot_nr) & 0x00FF) != 0x0000) /* a pdev-port */

#define CM_SV_SUBSLOT_IS_PLUGGED(su)  ((su)->sm_refcnt != 0)

typedef struct cm_sv_subslot_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_subslot_tag) link; /* link, first in struct! */

	LSA_UINT16 subslot_nr;

	LSA_UINT16 sm_refcnt; /* number of submodules (plugged into subslots) referencing this slot */

	LSA_UINT16 ar_refcnt; /* number of ARs referencing this subslot */

	/***/

	LSA_UINT32 sub_ident;

	LSA_UINT8 im0_bits; /* see cm_sv_record_marshal_IM0FilterData_all() */

	/***/

	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_diag_tag) diag_list[CM_SV_DIAG_TYPE_MAX]; /* list of CM_SV_DIAG_TYPE */

	/***/

	LSA_UINT16 owner_ar_nr; /* 0 = no owner */

	CM_SV_CANDIDATE_TYPE cand[CM_CFG_MAX_SV_ARS]; /* 0 to CM_CFG_MAX_SV_ARS candidates */

} CM_SV_SUBSLOT_TYPE;

typedef CM_SV_SUBSLOT_TYPE *CM_SV_SUBSLOT_PTR_TYPE;

typedef CM_SV_SUBSLOT_TYPE const *CM_SV_SUBSLOT_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== SLOT TYPE =============================================================*/
/*===========================================================================*/

#define CM_SV_SLOT_IS_PLUGGED(sl)  ((sl)->sm_refcnt != 0)

typedef struct cm_sv_slot_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_slot_tag) link; /* link, first in struct! */

	LSA_UINT16 slot_nr;

	LSA_UINT16 sm_refcnt; /* number of submodules (plugged into subslots) referencing this slot */

	LSA_UINT16 ar_refcnt; /* number of ARs referencing this slot */

	/***/

	LSA_UINT32 mod_ident;

	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_subslot_tag) subslot_list; /* sorted list of subslots */

} CM_SV_SLOT_TYPE;

typedef CM_SV_SLOT_TYPE *CM_SV_SLOT_PTR_TYPE;

typedef CM_SV_SLOT_TYPE const *CM_SV_SLOT_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== AP TYPE ===============================================================*/
/*===========================================================================*/

#define CM_SV_AP_IS_PLUGGED(ap)  ((ap)->sm_refcnt != 0)

typedef struct cm_sv_ap_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_ap_tag) link; /* link, first in struct! */

	LSA_UINT32 api;

	LSA_UINT16 sm_refcnt; /* number of submodules (plugged into subslots) referencing this AP */

	LSA_UINT16 ar_refcnt; /* number of ARs referencing this AP */

	/***/

	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_slot_tag) slot_list; /* sorted list of slots */

} CM_SV_AP_TYPE;

typedef CM_SV_AP_TYPE *CM_SV_AP_PTR_TYPE;

typedef CM_SV_AP_TYPE const *CM_SV_AP_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== LOG ENTRY TYPE ========================================================*/
/*===========================================================================*/

typedef struct cm_sv_log_entry_tag {

	LSA_UINT64 local_time_stamp;

	CLRPC_UUID_TYPE ar_uuid;

	LSA_UINT32 pnio_status;
	LSA_UINT32 entry_detail;

} CM_SV_LOG_ENTRY_TYPE;

typedef CM_SV_LOG_ENTRY_TYPE *CM_SV_LOG_ENTRY_PTR_TYPE;

/*===========================================================================*/
/*=== ITERATOR TYPE =========================================================*/
/*===========================================================================*/

/*** defines for iterator flags ***/

#define CM_SV_ITER_SUB       0x0001 /* subslot, index 8xxx */
#define CM_SV_ITER_MOD       0x0002 /* slot, index Cxxx */
#define CM_SV_ITER_AR        0x0004 /* AR, index Exxx */
#define CM_SV_ITER_REAL_AP   0x0008 /* AP, index F0xx */
#define CM_SV_ITER_REAL_DEV  0x0010 /* device, index F8xx */
/***/
#define CM_SV_ITER__ONE_SUB  0x0100 /* next-sub is forbidden */
#define CM_SV_ITER__ONE_MOD  0x0200 /* next-mod is forbidden */
#define CM_SV_ITER__ONE_AP   0x0400 /* next-ap is forbidden */
#define CM_SV_ITER__ONE_AR   0x0800 /* next-ar is forbidden */
/***/
#define CM_SV_ITER__ALL_ARS  0x8000  /* setup ar-iterator */

typedef struct cm_sv_iterator_tag {

	/*CM_SV_DEVICE_PTR_TYPE*/struct cm_sv_device_tag * dev;

	CM_AR_GRAPH_PTR_TYPE argr;

	LSA_UINT16 flags; /* see CM_SV_ITER_SUB, ... */

	LSA_UINT32 rq_api;
	LSA_UINT16 rq_slot_nr;
	LSA_UINT16 rq_subslot_nr;

	LSA_UINT16 ar_nr;

	/*** device ("real") things ***/

	CM_SV_AP_PTR_TYPE ap;
	CM_SV_SLOT_PTR_TYPE slot;
	CM_SV_SUBSLOT_PTR_TYPE subslot;

	/*** ar-graph ("expected") things ***/

	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

	/*** pdev iterator things ***/

	struct {

		LSA_UINT16 state; /* see cm_pd_iter_pdev_enum (using the same constants as CM-PD) */
		LSA_UINT32 record_data_length;
		LSA_UINT32 offs_tot;
		LSA_UINT32 offs_blk;

		/* iterator is also used by ARData */

		CLRPC_UPPER_MEM_U8_PTR_TYPE record_data;
		CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;
		CM_AR_GRAPH_PTR_TYPE argr;
		LSA_BOOL v1dot1; /* LSA_TRUE use V1.1 else V1.0 */
	} pdev;

} CM_SV_ITERATOR_TYPE;

typedef CM_SV_ITERATOR_TYPE *CM_SV_ITERATOR_PTR_TYPE;

typedef CM_SV_ITERATOR_TYPE const *CM_SV_ITERATOR_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== DEVICE TYPE ===========================================================*/
/*===========================================================================*/

#define CM_CFG_MAX_SV_ARS_RTC123UDP  9 /* see cm_sv_dev_calc_provider_sequence(), AP01379821 (8->9 ARs) */

#define CM_CFG_MAX_SV_ARSETS_PER_DEVICE  1 /* currently only 1 AR-set */

#define CM_SV_DEV_STATE_ADDING    0
#define CM_SV_DEV_STATE_ADDED     1
#define CM_SV_DEV_STATE_REMOVING  2

#define CM_SV_DEV_IS_ACTIVE(dev)   ((dev)->is_active)  /* else passive */
#define CM_SV_DEV_IS_PARKING(dev)  ((dev)->is_parking) /* a special case of passive */

#define CM_SV_DEV_IS_PDEV_DEVICE(dev)  ((dev)->val_8i00 != 0) /* the device is the pdev-device (may contain 0x8ipp/0x9jpp-submodules) */

#define CM_SV_DEV_GET_ARSET(dev)  (&(dev)->explicit_ar.arset[0]) /* note: no parameter "arset_nr", see CM_CFG_MAX_SV_ARSETS_PER_DEVICE */

typedef struct cm_sv_device_tag {

	CM_CHANNEL_PTR_TYPE channel; /* backlink to the channel */
	LSA_UINT32 trace_idx;        /* for "LSA index trace" */

	LSA_UINT16 device_nr;

	LSA_UINT8 state; /* see CM_SV_DEV_STATE_ADDING, ... */

	LSA_BOOL is_active;

	LSA_BOOL is_parking;

	/***/

	CM_UPPER_RQB_PTR_TYPE curr; /* current request */
	CM_UPPER_RQB_PTR_TYPE read; /* current read request */

	/***/

	LSA_UINT16 vendor_id;
	LSA_UINT16 device_id;
	LSA_UINT16 instance;

	LSA_UINT16 max_slot_nr;
	LSA_UINT16 max_subslot_nr;
	LSA_UINT16 max_nr_of_subslots; /* of one AR (AP00774491) */
	LSA_UINT16 alarm_send_max_length; /* AP01232054 */

	LSA_UINT16 val_8i00; /* 0 or 0x8i00 (0x9j00) if "contains_pdev", see AP00995373 */
	LSA_UINT16 nr_of_pdev_mappings; /* valid if val_8i00 != 0 (additional info) */
	LSA_BOOL   central_pdev; /* AP01066848 */
	LSA_UINT16 pdev_owner_ar_nr;

	LSA_UINT16 write_multiple_parallelism; /* AP00786836 */

	LSA_BOOL SRD_enabled;
	LSA_BOOL CIR_enabled;
	LSA_BOOL R1_enabled;
	LSA_UINT32 R1_bridge_max;

	LSA_BOOL PDevPrmInd_enabled; /* TIA 1193013 */

	LSA_BOOL skip_check_100Mbps_FDX; /* TIA 1963788 */

	LSA_UINT16 min_device_interval_31250ns; /* AP01451871 */

	/* TIA 1453731 */
	LSA_UINT16 gsd_MaxInputLength;  /* see GSDML/MaxInputLength */
	LSA_UINT16 gsd_MaxOutputLength; /* see GSDML/MaxOutputLength */
	LSA_UINT16 gsd_MaxDataLength;   /* see GSDML/MaxDataLength or calculated as defined in GSDML */
	LSA_UINT16 gsd_NumberOfInputCR;             /* see GSDML/NumberOfInputCR */
	LSA_UINT16 gsd_NumberOfOutputCR;            /* see GSDML/NumberOfOutputCR */
	LSA_UINT16 gsd_NumberOfAdditionalInputCR;   /* see GSDML/NumberOfAdditionalInputCR */
	LSA_UINT16 gsd_NumberOfAdditionalOutputCR;  /* see GSDML/NumberOfAdditionalOutputCR */
	LSA_UINT16 gsd_NumberOfMulticastProviderCR; /* see GSDML/NumberOfAdditionalMulticastProviderCR (note that "Additional" is misleading) */
	LSA_UINT16 gsd_NumberOfMulticastConsumerCR; /* see GSDML/NumberOfMulticastConsumerCR */

	LSA_BOOL gsd_SharedDeviceSupported; /* see GSDML/SharedDeviceSupported (TIA 1587591) */

	LSA_BOOL gsd_ReportingSystemSupported; /* see GSDML/ReportingSystem (TIA 1645525) */

	LSA_BOOL gsd_AssetManagementSupported; /* see GSDML/attribute to be defined yet (TIA 1722142) */

	struct {

		LSA_UINT16 nr_of_event_resources;
		LSA_UINT16 nr_of_alarm_resources;

		CM_RPC_LOWER_RQB_PTR_TYPE rpc_rqb; /* for register/unregister and bind/unbind */
		CM_LIST_ENTRY_TYPE rpc_callQ;      /* call.ind resources */
		LSA_UINT16 rpc_server_id;          /* see cm_sv_rpc_register_if_obj_done() */

		CM_LIST_ENTRY_TYPE eventQ; /* event resources */

		CM_LIST_ENTRY_TYPE alarmQ; /* alarm resources */

		LSA_VOID_PTR_TYPE clid_arr[1 + CM_CFG_MAX_SV_ARS]; /* index = ar_nr, a pre-allocated rpc-client for each AR */

		LSA_UINT16 skey_arr[1 + CM_CFG_MAX_SV_ARS]; /* index = ar_nr, incremented on each connect - must not use the "foreign key" of the RPC! */

	} res;

	/***/

	CM_LIST_TYPED_ENTRY_TYPE(cm_sv_ap_tag) ap_list; /* list of APs */

	struct {

		LSA_UINT16 max_ARs_IOC;
		LSA_UINT16 max_ARs_IOS;
		LSA_UINT16 max_ARs_DAC;
		LSA_UINT16 max_ARs_IOX; /* = IOC + IOS */
		LSA_UINT16 max_ARs_TOT; /* = IOC + IOS + DAC = total */

		LSA_UINT16 ar_cnt; /* number of ARs in argr_arr[] */

		CM_AR_GRAPH_PTR_TYPE argr_arr[1 + CM_CFG_MAX_SV_ARS]; /* index = ar_nr */

		struct { /* AP00619993 (stable sequence of provider frames) */

			LSA_UINT16 cnt;

			CM_AR_GRAPH_PTR_TYPE arr[CM_CFG_MAX_SV_ARS]; /* index = 0 .. sort.cnt */

		} sort;

		CM_SV_ARSET_TYPE arset[CM_CFG_MAX_SV_ARSETS_PER_DEVICE]; /* index = (arset_nr - 1), AR-set (system redundancy) */

	} explicit_ar;

	struct {

		CM_LIST_ENTRY_TYPE queue;

		CM_RPC_LOWER_RQB_PTR_TYPE rpc;

		CM_SV_ITERATOR_TYPE iter; /* an iterator for the F8xx records */

	} implicit_ar;

	struct {

		LSA_UINT32 wrt_idx; /* next writable position */

		LSA_UINT32 log_cnt; /* number of entries */

		CM_SV_LOG_ENTRY_TYPE buf[CM_CFG_MAX_SV_LOG_ENTRIES];

	} logbook;

	struct {

		CM_LIST_ENTRY_TYPE info_list;

	} led;

	CM_SV_DEVICE_R1B_TYPE r1b;

} CM_SV_DEVICE_TYPE;

typedef CM_SV_DEVICE_TYPE *CM_SV_DEVICE_PTR_TYPE;

typedef CM_SV_DEVICE_TYPE const *CM_SV_DEVICE_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== CHANNEL TYPE ==========================================================*/
/*===========================================================================*/

typedef struct cm_sv_channel_tag {

	/*
	 * the states of the state machine CM-SV
	 * are defined in cm_int.h "in-line" with
	 * the states of CM-CH
	 */

	CM_UPPER_RQB_PTR_TYPE curr; /* current working upper request block */

	/***/

	LSA_BOOL can_RecordDataReadQuery;

	LSA_BOOL can_ARFSUDataAdjust;

	LSA_BOOL enable_AlarmResponder;

	LSA_BOOL enable_DataStatusIndication;

	LSA_BOOL IOXS_Required; /* TIA 1218229 */

	LSA_UINT16 pdev_device_nr; /* the device that contains the pdev-submodules */

	LSA_BOOL tell_all_ARs_gone; /* tell CM-PD that all (explicit) ARs of all devices are gone */

	/***/

	CM_SV_DEVICE_PTR_TYPE dev[1/*unused*/ + CM_CFG_MAX_SV_DEVICES]; /* index = device_nr */

	/***/

	CM_EDD_LOWER_RQB_PTR_TYPE edd_rqb; /* for global provider-control */

} CM_SV_CHANNEL_TYPE;

/*===========================================================================*/
/*=== SERVER THINGS =========================================================*/
/*===========================================================================*/

CM_UPPER_MEM_PTR_TYPE
cm_sv_upper_mem_alloc(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT32 length
);

LSA_VOID
cm_sv_upper_mem_free(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_MEM_PTR_TYPE upper
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_opc_dispatch(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_sv_opc_create(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_delete(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_control(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_acp_global_provider_control_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_CONST_PTR_TYPE edd
);

LSA_VOID
cm_sv_rpc_lower_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*===========================================================================*/
/*=== DEVICE THINGS =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_add(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_control(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_led_info(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_provide_event(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_provide_alarm(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_read(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_device_read_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_DEVICE_PTR_TYPE dev
);

/*---------------------------------------------------------------------------*/

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_get_added(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr
);

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_lookup(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr
);

/*---------------------------------------------------------------------------*/

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_alloc_and_link(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_SV_DEVICE_ADD_CONST_PTR_TYPE param
);

LSA_VOID
cm_sv_dev_unlink_and_free(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_dev_res_init(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 max_ars_IOC,
	LSA_UINT16 max_ars_IOS,
	LSA_UINT16 max_ars_DAC,
	LSA_UINT16 write_multiple_parallelism /* >= 1, see cm_sv_dev_params_ok() */
);

LSA_VOID
cm_sv_dev_res_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_dev_reprovide_event(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
);

CM_UPPER_RQB_PTR_TYPE
cm_sv_dev_get_event_notnull(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* don't care if argr != LSA_NULL */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_OPCODE_TYPE cm_opcode
);

LSA_VOID
cm_sv_dev_reprovide_alarm(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
);

CM_UPPER_RQB_PTR_TYPE
cm_sv_dev_get_alarm_notnull(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* don't care if argr != LSA_NULL */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio, /* AP01025495 */
	LSA_OPCODE_TYPE cm_opcode
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_register_if_obj_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_unregister_if_obj_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_bind_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_unbind_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_dev_led_info_eval(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

LSA_VOID
cm_sv_dev_led_info_cancel(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_dev_done(
	CM_SV_DEVICE_PTR_TYPE dev
);

/*===========================================================================*/
/*=== AP/MODULE/SUBMODULE/DIAGNOSIS THINGS ==================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_submodule_or_diag_changed_trigger(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

LSA_VOID
cm_sv_opc_submodule_add(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_submodule_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_diag_add(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_diag_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_diag_change( /* AP01467031 */
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_add
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_im0_bits_ok( /* note: called too from CM-PD */
	LSA_UINT8 im0_bits,
	LSA_BOOL cm_pd_has_im5_record
);

LSA_UINT16
cm_sv_submodule_add_elem(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_user
);

LSA_UINT16
cm_sv_submodule_remove_elem(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_user
);

LSA_UINT16
cm_sv_diag_add_elem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
);

LSA_UINT16
cm_sv_diag_remove_elem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_REMOVE_ELEMENT_CONST_PTR_TYPE elem
);

LSA_UINT16
cm_sv_diag_change_elem( /* AP01467031 */
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_add,
	LSA_BOOL is_user
);

LSA_BOOL
cm_sv_diag_delete(
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT32 diag_tag, /* 0 = all */
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
);

/*---------------------------------------------------------------------------*/

CM_SV_AP_PTR_TYPE
cm_sv_ap_lookup(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 api
);

CM_SV_AP_PTR_TYPE
cm_sv_ap_attach(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT32 api,
	LSA_BOOL from_submodule
);

CM_SV_AP_PTR_TYPE
cm_sv_ap_detach(
	CM_SV_AP_PTR_TYPE ap,
	LSA_BOOL from_submodule
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_slot_range_ok(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 slot_nr
);

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_lookup(
	CM_SV_AP_CONST_PTR_TYPE ap,
	LSA_UINT16 slot_nr
);

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_attach(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_AP_PTR_TYPE ap,
	LSA_UINT16 slot_nr,
	LSA_BOOL from_submodule
);

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_detach(
	CM_SV_SLOT_PTR_TYPE sl,
	LSA_BOOL from_submodule
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_subslot_range_ok(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_lookup(
	CM_SV_SLOT_CONST_PTR_TYPE sl,
	LSA_UINT16 subslot_nr
);

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_attach(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_SLOT_PTR_TYPE sl,
	LSA_UINT16 subslot_nr,
	LSA_BOOL from_submodule
);

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_detach(
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_BOOL from_submodule
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_diag_maint_qual(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_AP_CONST_PTR_TYPE ap,
	CM_SV_SLOT_CONST_PTR_TYPE sl,
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT8  *cha_ptr, /* Possible value is CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES or CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO */
	LSA_UINT8  *gen_ptr, /* Possible value is CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES or CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO */
	LSA_UINT32 *mnt_ptr
);

/*===========================================================================*/
/*=== EXPLICIT/IMPLICIT-AR THINGS ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_explicit_ar_init(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 max_ars_IOC,
	LSA_UINT16 max_ars_IOS,
	LSA_UINT16 max_ars_DAC
);

LSA_VOID
cm_sv_explicit_ar_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_implicit_ar_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_implicit_ar_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE
cm_sv_get_rpc_client_id(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_ar_alloc(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_ARSET_PTR_TYPE arset,
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_free(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 pnio_err,
	LSA_UINT32 entry_detail
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup1(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup2(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 ar_nr
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup3(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 session_key,
	LSA_UINT32 cn_flag /* 0 or CM_SV_AR_CNFL_EXP_USER_REQ, CM_SV_AR_CNFL_WF_CONN_RSP, ... */
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup_by_aruuid(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CLRPC_UUID_CONST_PTR_TYPE uuid
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_abort_all(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT8 reason
);

LSA_VOID
cm_sv_sendclock_changed(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 send_clock
);

LSA_BOOL
cm_sv_pdev_Rtf_NoS_IpSuite(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
);

/*===========================================================================*/
/*=== LOGBOOK THINGS ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_logbook_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_logbook_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_logbook_write(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 pnio_err,
	LSA_UINT32 entry_detail
);

LSA_BOOL
cm_sv_logbook_has_entries(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

LSA_UINT16
cm_sv_logbook_marshal(
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size
);

/*===========================================================================*/
/*=== AR-GRAPH OWNERSHIP THINGS =============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_attach(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_detach(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ownership_queue_for(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ownership_relinquish(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_UINT16
cm_sv_ownership_action(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT16 cand_session_key
);

/*===========================================================================*/
/*=== AR-GRAPH THINGS =======================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_argr_alloc_and_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_UINT16
cm_sv_argr_undo_init_and_free(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_ar_is_pdev_owner(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_BOOL
cm_sv_rpc_check_implicit_ar( /* check the rpc and complete it if tagged as "completed" */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_rpc_next_implicit_ar(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_rpc_check_explicit_ar( /* check the rpc and complete it if tagged as "completed" */
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_call_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_call_response(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_UINT16 clrpc_response, /* CLRPC_OK or CLRPC_ERR_FAULTED */
	LSA_UINT32 pnio_status,    /* used if CLRPC_OK */
	LSA_UINT32 ndr_length      /* including CLRPC_PNIO_OFFSET */
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_call_request(
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_ready4rtclass3,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
);

LSA_VOID
cm_sv_rpc_call_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_ready4rtclass3,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_ready4rtclass3_request(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_rpc_ready4rtclass3_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_UINT32
cm_sv_rpc_connect_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_connect_response_join(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_UINT32
cm_sv_argr_build(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_COMMON_MEM_U8_PTR_TYPE r1_data_ptr, /* used if rpc==NULL */
	LSA_UINT32 r1_data_length              /* used if rpc==NULL */
);

LSA_BOOL
cm_sv_argr_prealloc(
	CM_AR_GRAPH_PTR_TYPE argr
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_unpark(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 reason
);

LSA_VOID
cm_sv_ar_sendclock_changed(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 send_clock
);

LSA_VOID
cm_sv_ar_control_ip_suite_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_OHA_LOWER_RQB_PTR_TYPE oha
);

CM_UPPER_SV_AR_CONNECT_PTR_TYPE
cm_sv_ar_connect_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_connect_build_notnull(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

LSA_VOID
cm_sv_opc_ar_connect_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_BOOL
cm_sv_ar_connect_update(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect
);

LSA_VOID
cm_sv_ar_connect_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*----------------------------------------------------------------------------*/

CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE
cm_sv_ar_ownership_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_ownership_build(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 ar_context
);

LSA_VOID
cm_sv_opc_ar_ownership_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

CM_UPPER_SV_AR_PRM_END_PTR_TYPE
cm_sv_ar_prm_end_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

CM_UPPER_SV_AR_IN_DATA_PTR_TYPE
cm_sv_ar_in_data_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

CM_UPPER_SV_AR_RIR_PTR_TYPE
cm_sv_ar_rir_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ar_submodule_state_reset(
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL is_dirty,
	LSA_BOOL is_wrong
);

LSA_VOID
cm_sv_ar_submodule_state_changed(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_VOID
cm_sv_ar_submodule_state_action(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_rebind_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_UINT16
cm_sv_get_submodule_state_ident( /* returns NO_SUBMODULE or WRONG or OK (not SUBSTITUTE) */
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
);

LSA_UINT16
cm_sv_get_submodule_state(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* LSA_NULL if "without diag" */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_AP_CONST_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
);

#define CM_SV_MDB_CONTEXT_READ       0 /* context = read-record */
#define CM_SV_MDB_CONTEXT_CONN_CRES  1 /* context = connect-phase / connect.rsp */
#define CM_SV_MDB_CONTEXT_CONN_ARDY  2 /* context = connect-phase / appl-ready.req */
#define CM_SV_MDB_CONTEXT_PLUG_ARDY  3 /* context = plug-phase / appl-ready.req */
#define CM_SV_MDB_CONTEXT_PBE_BRES   4 /* context = prm-begin-end / prm-begin.rsp */
#define CM_SV_MDB_CONTEXT_PBE_ARDY   5 /* context = prm-begin-end / appl-ready.req */

LSA_BOOL
cm_sv_marshal_module_diff_block(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_ALARM_CONST_PTR_TYPE plug_alarm,
	LSA_UINT32 mdb_context, /* see CM_SV_MDB_CONTEXT_READ, ... */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
);

LSA_UINT32
cm_sv_build_connect_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_UINT32
cm_sv_rpc_control_indication(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_UINT32
cm_sv_rpc_control_indication_prm_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_control_response_prm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_opc_ar_pdevprm_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_ar_pdevprm_rdwr(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_read
);

LSA_VOID
cm_sv_ar_pdev_reset_error_info(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_BOOL is_remote_pdev
);

LSA_VOID
cm_sv_pd_prm_end_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_VOID
cm_sv_opc_ar_prm_end_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_pd_prm_end_remote_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_end_of_predata_phase(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_opc_ar_in_data_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_ar_rir_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_ar_rir_ack(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_opc_ar_appl_ready_user(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_ar_appl_ready_cnt_decrement(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_rpc_appl_ready_request(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_rpc_appl_ready_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_rpc_cancel_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

/*---------------------------------------------------------------------------*/

LSA_UINT32
cm_sv_rpc_release_indication(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
);

LSA_VOID
cm_sv_opc_ar_abort(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_ar_disconnect_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_pd_prm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr /* 0 .. Connect/PrmUpdate(IF)  else .. Plug/PrmUpdate */
);

LSA_VOID
cm_sv_pd_prm_begin_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_VOID
cm_sv_pd_prm_relinquish_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

/*===========================================================================*/
/*=== RECORD THINGS =========================================================*/
/*===========================================================================*/

typedef struct cm_sv_record_params_tag {

	/* rpc opnum things */

	LSA_BOOL is_read;
	LSA_BOOL is_read_implicit;

	/* parsed things */

	LSA_UINT16 seq_number;

	CLRPC_UUID_TYPE ar_uuid;

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;
	LSA_UINT16 padding1;
	LSA_UINT16 index;

	LSA_UINT32 record_data_length;

	CLRPC_UPPER_MEM_U8_PTR_TYPE target_ar_uuid_ptr;

	LSA_UINT32 readquery_length;

	/* looked-up things */

	LSA_UINT32 cls; /* see cm_record_index_classify() */

	CM_AR_GRAPH_PTR_TYPE argr;

	CM_SV_AP_PTR_TYPE ap;

} CM_SV_RECORD_PARAMS_TYPE;

typedef CM_SV_RECORD_PARAMS_TYPE *CM_SV_RECORD_PARAMS_PTR_TYPE;

typedef CM_SV_RECORD_PARAMS_TYPE const *CM_SV_RECORD_PARAMS_CONST_PTR_TYPE;

/*---------------------------------------------------------------------------*/

LSA_UINT32
cm_sv_iter_setup(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	LSA_UINT16 flags,
	LSA_INT need_AR /* true: need "argr" (expected config) */
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_rpc_record_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_BOOL is_read_implicit
);

LSA_VOID
cm_sv_rpc_mark_as_record_malformed(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_UINT32 pnio_err
);

LSA_VOID
cm_sv_rpc_mark_as_record_failed(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
);

LSA_VOID
cm_sv_rpc_mark_as_record_succeeded(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length
);

LSA_UINT32
cm_sv_record_check_range_and_context( /* only called for writes */
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_RECORD_PARAMS_PTR_TYPE req,
	LSA_BOOL is_COC
);

LSA_BOOL
cm_sv_rpc_park( /* TIA 1518690 */
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_write /* true if write, false if prm-end or prm-begin (TIA 2231336) */
);

LSA_VOID
cm_sv_rpc_unpark( /* TIA 1518690 */
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_record_update_header(
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
);

LSA_BOOL
cm_sv_ar_record_indicate(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_CONST_PTR_TYPE argr, /* LSA_NULL if implicit-ar */
	LSA_BOOL via_CombinedObjectContainer, /* write: record transferred via COC (TIA 1582512) */
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data,
	LSA_UINT32 *pnio_err
);

LSA_VOID
cm_sv_opc_ar_record_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_read
);

LSA_UINT32
cm_sv_record_read_normative(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_SV_SUBSLOT_CONST_PTR_TYPE su
);

LSA_VOID
cm_sv_frame_get_apdu_status_done(
	LSA_VOID_PTR_TYPE user_id,
	LSA_UINT16 cycle_counter,
	LSA_UINT8 data_status,
	LSA_UINT8 transfer_status
);

LSA_UINT32
cm_sv_record_read_pdev_next(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_VOID
cm_sv_record_write_embedded_update(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
);

LSA_VOID
cm_sv_record_write_embedded_next(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_pdev_record_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* LSA_NULL if implicit-ar */
	LSA_BOOL is_read,
	CM_UPPER_RQB_PTR_TYPE pd
);

/*===========================================================================*/
/*=== AR-COM THINGS =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_cmi_timer_trigger(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID
cm_sv_ar_cmi_timer_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*---------------------------------------------------------------------------*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_arcq_request_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 arcq_opcode,
	LSA_UINT16 arcq_para1
);

LSA_VOID
cm_sv_ar_frames_add_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_alarm_add_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_prov_activate_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_prov_activate_aux_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_prov_ioparams_set_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_cons_activate_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_set2unk_enpras_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_received_in_red_indication(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_new_data_indication(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_com_down(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_prov_passivate_aux_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_ownership_disc_indication(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_ownership_disc_response(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_disconnect_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_frames_passivate_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_alarm_remove_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_frames_remove_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*===========================================================================*/
/*=== AR-GRAPH DATA-STATUS THINGS ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_control(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_ar_data_status_set_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_problem_indicator_eval(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_problem_indicator_mirror(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_request
);

/*===========================================================================*/

LSA_BOOL
cm_sv_ar_data_status_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 data_status_curr,
	LSA_UINT16 data_status_prev
);

LSA_VOID
cm_sv_opc_ar_data_status_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/*===========================================================================*/
/*=== AR-GRAPH ALMI (ALARM INITIATOR) THINGS ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_send(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_UINT16
cm_sv_ar_almi_send_max_length(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_type
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_almi_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ar_almi_alloc(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_enable(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL enable_set2unk
);

LSA_VOID
cm_sv_ar_almi_down(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ar_almi_is_idle(
	CM_AR_GRAPH_PTR_TYPE argr
);

CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_sv_ar_almi_plug_or_release(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_cancel(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api, /* see usage */
	LSA_UINT16 slot_nr, /* see usage */
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub /* LSA_NULL matches any alarm */
);

LSA_UINT16
cm_sv_ar_almi_alarm_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_AR_GRAPH_AP_CONST_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub, /* LSA_NULL if pull module */
	LSA_BOOL * do_led_info_ptr /* LSA_NULL or out-param LSA_TRUE if A.R.P. changes */
);

LSA_VOID
cm_sv_ar_almi_send_next_continued(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_AR_ALMI_CONST_PTR_TYPE almi
);

LSA_VOID
cm_sv_ar_almi_send_done(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_sv_ar_almi_ack_indication(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16 idx/*prio*/,
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp
);

LSA_VOID
cm_sv_ar_almi_alarm_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE upper,
	LSA_UINT16 rsp
);

LSA_VOID
cm_sv_ar_almi_send_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_UPPER_RQB_PTR_TYPE upper
);

LSA_VOID
cm_sv_ar_almi_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio
);

LSA_UINT16
cm_sv_ar_almi_alarm_prio(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_type
);

/*---------------------------------------------------------------------------*/

CM_UPPER_ALARM_PTR_TYPE
cm_sv_ar_almi_prm_end_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 alarm_sequence_number
);

LSA_VOID
cm_sv_ar_almi_pbe(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
);

LSA_VOID
cm_sv_ar_almi_set2unk_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_plug_group_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_almi_rtc3_action(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_rtc3_action_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almi_newdata_action(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*===========================================================================*/
/*=== AR-GRAPH ALMR (ALARM RESPONDER) THINGS ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_ar_alarm_ack(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_ar_almr_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ar_almr_alloc(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almr_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_almr_down(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL
cm_sv_ar_almr_is_idle(
	CM_AR_GRAPH_PTR_TYPE argr
);

CM_ACP_LOWER_RQB_PTR_TYPE
cm_sv_ar_alarm_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
);

LSA_VOID
cm_sv_ar_alarm_ack_send_done(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_sv_ar_almr_enable(
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/
/*=== AR-GRAPH M-CONSUMER THINGS ============================================*/
/*===========================================================================*/

#if CM_CFG_SV_AR_HAS_MCONS

LSA_BOOL
cm_sv_mcons_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE io_cr,
	LSA_OPCODE_TYPE cm_opcode,
	LSA_UINT16 nare_rsp, /* see NARE_RSP_xxx */
	CM_MAC_PTR_TYPE source_mac, /* ethernet mac-addr or error-mac */
	LSA_UINT32 source_ip, /* ip-addr or error-ip */
	LSA_UINT16 data_status_delta
);

LSA_VOID
cm_sv_opc_ar_mcons_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_UINT16
cm_sv_mcons_diag_value(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr,
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub
);

#endif

/*===========================================================================*/
/*=== PHYSICAL DEVICE THINGS ================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_pdev_read_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data, /* the clrpc-buffer! */
	LSA_UINT16 ar_nr
);

/*===========================================================================*/

LSA_VOID
cm_sv_pdev_diag_changed(
	CM_CHANNEL_PTR_TYPE channel /* the pd-channel */
);

LSA_VOID
cm_sv_opc_ar_alarm_pdev_group_ind(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE alarm
);

LSA_VOID
cm_sv_pdev_request(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_pdev_request_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_pdev_callback(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_UINT16 rsp
);

LSA_VOID
cm_sv_pdev_write_failed( /* AP00581494 */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 cls,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 index,
	LSA_UINT32 offset
);

LSA_UINT32
cm_sv_pdev_CombinedObjectContainer_write(
	CM_AR_GRAPH_PTR_TYPE argr,
	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data,
	CM_SV_RECORD_PARAMS_PTR_TYPE req
);

LSA_BOOL
cm_sv_pdev_CombinedObjectContainer_next(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_pdev_pull_submodule(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_UINT16
cm_sv_pdev_plug_submodule(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 mod_ident,
	LSA_UINT32 sub_ident,
	LSA_UINT8  im0_bits
);

LSA_VOID
cm_sv_pdev_set_im0_bits(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT8  im0_bits
);

/*===========================================================================*/
/*=== ARFSUDataAdjust THINGS ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ARFSUDataAdjust_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ARFSUDataAdjust_remove(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ARFSUDataAdjust_check(
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_BOOL is_ARFSUBlock /* AP01034170 */
);

LSA_BOOL
cm_sv_ARFSUDataAdjust_store(
	CM_AR_GRAPH_PTR_TYPE argr,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_BOOL is_ARFSUBlock /* AP01034170 */
);

LSA_UINT32
cm_sv_ARFSUDataAdjust_read(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
);

/*===========================================================================*/
/*=== I&M1-4 THINGS =========================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_IM1234_check( /* AP01338246 */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT16 index
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ISV_H */
