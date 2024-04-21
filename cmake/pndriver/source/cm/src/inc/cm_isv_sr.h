#ifndef CM_ISV_SR_H
#define CM_ISV_SR_H

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
/*  F i l e               &F: cm_isv_sr.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - system redundancy things                          */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*=== SRD THINGS ============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_arset_abort(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_arset_trigger(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_arset_check_primary(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_arset_is_primary_or_first(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

LSA_BOOL
cm_sv_arset_is_first(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

CM_AR_GRAPH_PTR_TYPE
cm_sv_arset_get_primary_or_first(
	CM_SV_ARSET_CONST_PTR_TYPE arset
);

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_sv_arset_alarm_stop(
	CM_AR_GRAPH_PTR_TYPE argr /* the "old" primary AR */
);

LSA_VOID
cm_sv_arset_alarm_restart(
	CM_AR_GRAPH_PTR_TYPE argr /* the new primary AR */
);

LSA_VOID
cm_sv_arset_alarm_cancel(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api, /* see usage */
	LSA_UINT16 slot_nr, /* see usage */
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub, /* LSA_NULL matches any alarm */
	LSA_BOOL is_r1_not_acked
);

LSA_UINT16
cm_sv_arset_alarm_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL * do_led_info_ptr /* LSA_NULL or out-param LSA_TRUE if A.R.P. changes */
);

LSA_UINT32
cm_sv_srd_consistency_check_argr(
	CM_AR_GRAPH_PTR_TYPE argr, /* the new AR */
	CM_AR_GRAPH_PTR_TYPE argr_ex, /* the existing AR */
	LSA_BOOL is_cir
);

/*===========================================================================*/
/*=== R1 THINGS =============================================================*/
/*===========================================================================*/

#define CM_SV_R1B_PREFIX_ALLOC_MAX 0xFFFFFFFF

CM_SV_R1BMSG_PREFIX_PTR_TYPE
cm_sv_r1b_prefix_alloc(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 max_body_size /* special case: CM_SV_R1B_PREFIX_ALLOC_MAX */
);

LSA_VOID
cm_sv_r1b_prefix_free(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

/***/

LSA_VOID
cm_sv_dev_r1b_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_BOOL
cm_sv_dev_r1b_alloc(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_dev_r1b_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_ar_r1b_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_BOOL
cm_sv_ar_r1b_prealloc(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_ar_r1b_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
);

/***/

LSA_VOID
cm_sv_opc_r1bridge_send_provide(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_r1bridge_send_rsp(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_r1bridge_received(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_opc_r1bridge_control(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/***/

LSA_VOID
cm_sv_r1b_join_broken(
	CM_SV_DEVICE_PTR_TYPE dev
);

/***/

CM_COMMON_MEM_U8_PTR_TYPE
cm_sv_r1b_get_message_body_from_prefix(
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix,
	LSA_UINT32 * message_max /* out-param (LSA_NULL: size will be checked in post function) */
);

LSA_VOID
cm_sv_r1b_send_res_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_post1( /* usage: device context */
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_post2( /* usage: AR context */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_post3( /* usage: AR/ALMI context */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_prio,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

/***/

LSA_VOID
cm_sv_r1b_post_connect_done_error( /* usage: representative-AR could not be created */
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UUID_CONST_PTR_TYPE ar_uuid,
	LSA_UINT32 pnio_err
);

LSA_VOID
cm_sv_r1b_posted_connect_done_error(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

/***/

LSA_BOOL
cm_sv_r1b_ownership_overlapping(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

/***/

LSA_VOID
cm_sv_r1b_cancel_arset( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_cancel_diag( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_r1b_post_connect(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_r1b_posted_connect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_connect(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL or not-LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size,
	CLRPC_UUID_CONST_PTR_TYPE ar_uuid,
	LSA_BOOL is_update /* TRUE if CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT */
);

/***/

LSA_VOID
cm_sv_r1b_posted_connect_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_connect_done(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_cancel_connect_done( /* called on bridge-broken */
	CM_AR_GRAPH_PTR_TYPE argr
);

/***/

LSA_UINT32
cm_sv_r1b_build_sub_state_message(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_COMMON_MEM_U8_PTR_TYPE sub_state,
	LSA_UINT32 buffer_size
);

LSA_VOID
cm_sv_r1b_post_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_prm_begin_end
);

LSA_VOID
cm_sv_r1b_posted_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_sub_state(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size,
	LSA_UINT16 message_numb /* CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT or CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE or CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END */
);

/***/

LSA_VOID
cm_sv_r1b_post_disconnect(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_r1b_posted_disconnect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_disconnect(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_post_disconnect_done(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_r1b_posted_disconnect_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_disconnect_done(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_cancel_disconnect_done( /* called on bridge-broken */
	CM_AR_GRAPH_PTR_TYPE argr
);

/***/

LSA_VOID
cm_sv_r1b_diag_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_r1b_diag_post_next(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_diag_changed(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_diag_changed(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_BOOL
cm_sv_r1b_diag_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_r1b_post_alarm_begin(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_SV_AR_ALMI_PTR_TYPE almi
);

LSA_VOID
cm_sv_r1b_posted_alarm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio, /* range-checked in caller */
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_alarm_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_pdev_alarm,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_handle_alarm_begin_plug_group(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_post_alarm_end(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_SV_AR_ALMI_PTR_TYPE almi
);

LSA_VOID
cm_sv_r1b_posted_alarm_end(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio, /* range-checked in caller */
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_alarm_end(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_pdev_alarm,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_handle_alarm_end_plug_group(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_BOOL
cm_sv_r1b_curr_alarm_matches(
	CM_SV_ARSET_PTR_TYPE arset,
	CM_UPPER_ALARM_CONST_PTR_TYPE al,
	LSA_UINT16 prio,
	LSA_UINT16 * sequence_number /* is updated if returns TRUE */
);

/***/

LSA_VOID
cm_sv_r1b_diag_replace_pdev(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_diag_replace_pdev(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_diag_replace_pdev(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_CONST_PTR_TYPE alarm
);

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
);

/***/

LSA_VOID
cm_sv_r1b_pdev_alarm_resubmit(
	CM_AR_GRAPH_PTR_TYPE argr
);

/***/

LSA_BOOL
cm_sv_r1b_marshal_diag(
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_UINT32 * offset,
	LSA_BOOL is_add
);

LSA_BOOL
cm_sv_r1b_marshal_pdev_diag(
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_PD_DIAG_CONST_PTR_TYPE diag,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_UINT32 * offset,
	LSA_BOOL is_add
);

LSA_VOID
cm_sv_r1b_parse_and_change_diag(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 message_size,
	CM_COMMON_MEM_U8_PTR_TYPE message_body
);

LSA_VOID
cm_sv_r1b_update_begin(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_handle_update_begin(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_posted_update_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_update_diag(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_handle_update_diag(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_posted_update_diag(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_update_connect(
	CM_SV_DEVICE_PTR_TYPE dev
);

/* note: handle update connect reuses cm_sv_r1b_handle_connect */

LSA_VOID
cm_sv_r1b_posted_update_connect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_update_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr
);

LSA_VOID
cm_sv_r1b_posted_update_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_update_alarms(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_update_alarms(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_update_alarms(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_update_end(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_handle_update_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_posted_update_end(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

/***/

LSA_UINT32
cm_sv_r1b_pdev_A_read_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data,
	LSA_UINT16 ar_nr
);

LSA_VOID
cm_sv_pd_prm_begin_remote(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr /* 0 .. Connect/PrmUpdate(IF)  else .. Plug/PrmUpdate */
);

LSA_VOID
cm_sv_pd_prm_begin_remote_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_VOID
cm_sv_r1b_pdev_A_request(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_VOID
cm_sv_r1b_pdev_A_next(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_pdev_A_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_read_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_write_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_begin_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_end_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_pdev_A_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_BOOL
cm_sv_r1b_pdev_A_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_r1b_pdev_B_handle_read_write_begin_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_r1b_pdev_B_next(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_pdev_B_done( /* called from cm_sv_pdev_request_done() */
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_CONST_PTR_TYPE pd
);

LSA_VOID
cm_sv_r1b_pdev_B_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_pdev_B_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_BOOL
cm_sv_r1b_pdev_B_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/***/

LSA_UINT16
cm_sv_r1b_stat_A_request(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 iocr_ref,
	CLRPC_UUID_PTR_TYPE ar_uuid,
	LSA_VOID_PTR_TYPE user_id
);

LSA_VOID
cm_sv_r1b_stat_A_next(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_stat_A_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat
);

LSA_VOID
cm_sv_r1b_stat_A_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_stat_A_handle_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

LSA_VOID
cm_sv_r1b_stat_A_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_BOOL
cm_sv_r1b_stat_A_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_r1b_stat_B_handle(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_sv_r1b_stat_B_next(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_stat_B_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 cycle_counter,
	LSA_UINT8 data_status,
	LSA_UINT8 transfer_status
);

LSA_VOID
cm_sv_r1b_stat_B_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_stat_B_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_BOOL
cm_sv_r1b_stat_B_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
);

/***/

LSA_VOID
cm_sv_r1b_post_reset_to_factory(
	CM_SV_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_sv_r1b_posted_reset_to_factory(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
);

LSA_VOID
cm_sv_r1b_handle_reset_to_factory(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
);

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ISV_SR_H */
