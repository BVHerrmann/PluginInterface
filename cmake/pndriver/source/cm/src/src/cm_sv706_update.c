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
/*  F i l e               &F: cm_sv706_update.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, R1 Bridge                                                     */
/*                                                                           */
/*  - Update                                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7706
#define CM_MODULE_ID       7706

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_diag_to_diag_add_elem(
	CM_SV_DIAG_PTR_TYPE diag,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT16 diag_type,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE elem
); /* forward */

static LSA_BOOL
cm_sv_r1b_diag_list_to_message(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32* message_size,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_BOOL local_pdev /* TRUE: only local pdev, otherwise: all but remote-pdev */
); /* forward */

static LSA_VOID
cm_sv_r1b_add_alarm_to_message(
	CM_UPPER_RQB_CONST_PTR_TYPE rqb,
	LSA_UINT16 prio,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 * offset,
	LSA_UINT32 size
); /* forward */

static LSA_VOID
cm_sv_r1b_add_alarm_curr_to_message(
	CM_SV_R1_ALARM_CURR_PTR_TYPE alarm,
	LSA_UINT16 prio,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 * offset,
	LSA_UINT32 size
); /* forward */

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE BEGIN ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_begin(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(dev->r1b.upd.msg_begin_and_diag, &message_max);
	LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_UPDATE_BEGIN;

	CM_ASSERT(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE);

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) sending update-begin"
		, dev->device_nr
	);

	/* PASSIVE: sending update-begin to ACTIVE, payload: local pdev-diagnoses */

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_BEGIN == 0U); /* no header, body: pdev-diagnoses */

	if( !cm_sv_r1b_diag_list_to_message(dev, &message_size, message_body, message_max, LSA_TRUE) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) bridge_state(%u), PDEV diag is too big for bridge message"
			, dev->device_nr, dev->r1b.bridge_state
			);

		CM_FATAL();
	}

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_UPDATE_BEGIN, message_size, dev->r1b.upd.msg_begin_and_diag);
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){
	if(
		cm_is_null(dev)
		|| prefix != dev->r1b.upd.msg_begin_and_diag
	) {

		CM_FATAL();
	}
	else {

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_update_begin(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	/* ACTIVE: received update-begin from PASSIVE, payload: pdev-diagnoses */

	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) bridge_state(%u) not establishing-active"
			, dev->device_nr, dev->r1b.bridge_state
			);

		CM_FATAL(); /* R1 protocol error */
	}

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_BEGIN == 0U); /* no header, body: pdev-diagnoses */

	cm_sv_r1b_parse_and_change_diag(dev, message_size, message_body); /* get PDEV diag from PASSIVE */

	cm_callback(dev->channel, CM_OK, &rb);

	/* note: CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE only starts the update procedure; it does not wait until completion (see join-on-broken) */

	/***/

	cm_sv_r1b_update_diag(dev); /* send diagnoses to PASSIVE */

	cm_sv_r1b_update_connect(dev); /* send AR to PASSIVE */

	cm_sv_r1b_update_alarms(dev); /* send user and pdev alarms to PASSIVE */

	cm_sv_r1b_update_end(dev); /* send update-end to PASSIVE */
}

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE DIAG =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_diag(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(dev->r1b.upd.msg_begin_and_diag, &message_max);
	LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_UPDATE_DIAG;

	/* note: CM_SV_DEV_R1B_STATE_ESTABLISHED is for the case "diag-in-fragments" */
	CM_ASSERT(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED);

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) sending update-diag"
		, dev->device_nr
		);

	/* ACTIVE: sending diagnoses to PASSIVE */

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_DIAG == 0U); /* no header, body: diagnoses (pdev and/or user) */

	if( cm_sv_r1b_diag_list_to_message(dev, &message_size, message_body, message_max, LSA_FALSE) ) {

		if( dev->r1b.diag.update_pending ) {

			dev->r1b.diag.update_pending = LSA_FALSE;

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) posting diags in chunks: sending last"
				, dev->device_nr
				);
		}
	}
	else {

		dev->r1b.diag.update_pending = LSA_TRUE;

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) posting diags in chunks"
			, dev->device_nr
			);
	}

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_UPDATE_DIAG, message_size, dev->r1b.upd.msg_begin_and_diag);
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_diag(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		cm_is_null(dev)
		|| prefix != dev->r1b.upd.msg_begin_and_diag
	) {
		CM_FATAL();
	}
	else {

		/* note: CM_SV_DEV_R1B_STATE_ESTABLISHED is for the case "diag-in-fragments" */
		if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED ) {

			if(dev->r1b.diag.update_pending) {

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "dev(%u) posting diags in chunks: continuing"
					, dev->device_nr
					);

				cm_sv_r1b_update_diag(dev);
			}
		}
		else if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN) {

			/* join-on-broken: see cm_sv_r1b_join_broken() */
		}
		else {

			CM_FATAL(); /* a bug */
		}
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_update_diag(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	/* PASSIVE: received diagnoses from ACTIVE */

	/* note: CM_SV_DEV_R1B_STATE_ESTABLISHED is for the case "diag-in-fragments" */
	if( !(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) bridge_state(%u) not establishing-passive"
			, dev->device_nr, dev->r1b.bridge_state
			);

		CM_FATAL(); /* R1 protocol error */
	}

	/***/

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_DIAG == 0U); /* no header, body: diagnoses (pdev and/or user) */

	cm_sv_r1b_parse_and_change_diag(dev, message_size, message_body);

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE CONNECT ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_connect(
	CM_SV_DEVICE_PTR_TYPE dev
){
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);

	CM_ASSERT(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE);

	/* ACTIVE: sending AR to PASSIVE */

	if( arset->is_aborting ) {
		return; /* don't update if AR-set is aborting */
	}

	if( cm_is_not_null(arset->arr[0]) ) {

		CM_AR_GRAPH_PTR_TYPE argr = arset->arr[0];

		if(
			cm_is_not_null(arset->arr[1])
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) == 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_CONN_POSTED) != 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_CONN_DONE) != 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN) != 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED) != 0
		) {

			CM_FATAL(); /* a bug */
		}
		else {

			LSA_UINT32 message_size = 0;

			(void)cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_connect, &message_size); /* see cm_sv_r1b_post_connect() */

			if( cm_ar_com_is_down(argr) ) {
				return; /* don't update if AR is down */
			}

			/* note: update-connect re-uses the message-buffer of the AR */
			/* note: no DONE message, see cm_sv_r1b_handle_connect() */

			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_DO_DISCONN;
			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED; /* message-buffer is in use */

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) is sending AR to other head"
				, argr->device_nr
				);

			cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT, message_size, argr->cm.sv.r1b.msg_connect);

			/***/

			cm_sv_r1b_update_sub_state(argr);
		}
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_connect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){
	if(
		prefix != argr->cm.sv.r1b.msg_connect
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED) == 0
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED;

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED */
		}

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE SUBMODULE-STATE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED) != 0 ) {
		CM_FATAL(); /* bug */
	}
	else {

		LSA_UINT32 message_max;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_update_sub_state, &message_max);
		LSA_UINT32 message_size;

		message_size = cm_sv_r1b_build_sub_state_message(argr, message_body, message_max);

		argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED;

		cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE, message_size, argr->cm.sv.r1b.msg_update_sub_state);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED UPDATE SUBMODULE-STATE ===============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED) == 0
		|| prefix != argr->cm.sv.r1b.msg_update_sub_state
	) {

		CM_FATAL(); /* bug */
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED;

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE ALARMS ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_alarms(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_AR_GRAPH_PTR_TYPE prim;

	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(dev->r1b.upd.msg_alarms, &message_max);
	LSA_UINT32 offset = 0;

	CM_ASSERT(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE);

	/* ACTIVE: send alarm-sequence numbers and alarms to PASSIVE */

	if( arset->is_aborting ) {

		return; /* nothing to update */
	}

	if( cm_is_not_null(arset->arr[0]) ) {

		cm_sv_arset_check_primary(arset->arr[0]);
	}

	prim = arset->primary;

	/***/

	if( cm_is_not_null(arset->r1b.pd_alarm.orig) ) {

		/* note: pdev-alarm mirror-set before update-alarms */

		cm_sv_r1b_pdev_alarm_mirror_set(dev, arset->r1b.pd_alarm.orig);
	}

	/***/

	if(cm_is_not_null(prim)) { /* use ALMI of primary AR */

		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMI_PTR_TYPE almi = &prim->cm.sv.almi[prio];

			CM_PUT16_HTON(message_body, offset, almi->sequence_number);
			offset += 2;

			CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) prio(%u) sending sequence_number(%u) to other head (using ALMI of sk(0x%x))"
				, dev->device_nr
				, prio
				, almi->sequence_number
				, prim->session_key
				);
		}

		CM_ASSERT(offset == CM_SV_R1BMSG_SIZE_UPDATE_ALARMS);

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMI_PTR_TYPE almi = &prim->cm.sv.almi[prio];

			if(cm_is_not_null(almi->curr)) {

				cm_sv_r1b_add_alarm_to_message(almi->curr, prio, message_body, &offset, message_max);
			}
		}
	}
	else { /* use AR-set */

		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_PUT16_HTON(message_body, offset, arset->alarm[prio].sequence_number);
			offset += 2;

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) prio(%u) sending sequence_number(%u) to other head (using AR-set)"
				, dev->device_nr
				, prio
				, arset->alarm[prio].sequence_number
				);
		}

		CM_ASSERT(offset == CM_SV_R1BMSG_SIZE_UPDATE_ALARMS);

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

			if( alarm_curr->is_used ) {

				cm_sv_r1b_add_alarm_curr_to_message(alarm_curr, prio, message_body, &offset, message_max);
			}
		}
	}

	if( offset > message_max ) {
		CM_FATAL();
	}

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_UPDATE_ALARMS, offset, dev->r1b.upd.msg_alarms);
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_alarms(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		cm_is_null(dev)
		|| prefix != dev->r1b.upd.msg_alarms
	) {

		CM_FATAL();
	}
	else {

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_update_alarms(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	/* PASSIVE: alarms received from ACTIVE */

	if( message_size < CM_SV_R1BMSG_SIZE_UPDATE_ALARMS ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
		LSA_UINT32 offset = 0;

		/***/

		{
		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			arset->alarm[prio].sequence_number = CM_GET16_NTOH(message_body, offset);
			offset += 2;

			if( arset->r1b.alarm.curr[prio].is_used ) {
				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}
		}
		}

		CM_ASSERT(offset == CM_SV_R1BMSG_SIZE_UPDATE_ALARMS);

		/***/

		while( offset < message_size ) {

			if( offset + CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM > message_size ) {

				CM_FATAL(); /* R1 protocol error */
			}
			else {

				LSA_UINT16 prio              = CM_GET16_NTOH(message_body, offset +  0);
				LSA_UINT16 alarm_sequence    = CM_GET16_NTOH(message_body, offset +  2);
				LSA_UINT32 api               = CM_GET32_NTOH(message_body, offset +  4);
				LSA_UINT16 slot_nr           = CM_GET16_NTOH(message_body, offset +  8);
				LSA_UINT16 subslot_nr        = CM_GET16_NTOH(message_body, offset + 10);

				LSA_BOOL is_pull_module      = CM_GET16_NTOH(message_body, offset + 12) != 0 ? LSA_TRUE : LSA_FALSE;
				LSA_BOOL is_pdev_alarm       = CM_GET16_NTOH(message_body, offset + 14) != 0 ? LSA_TRUE : LSA_FALSE;

				LSA_UINT16 alarm_type        = CM_GET16_NTOH(message_body, offset + 16);
				LSA_UINT16 alarm_tag         = CM_GET16_NTOH(message_body, offset + 18);
				LSA_UINT16 alarm_data_length = CM_GET16_NTOH(message_body, offset + 20);

				CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM == 22U);

				offset += CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM;

				if(
					prio > 1
					|| alarm_sequence > 0x07FF
					|| offset + alarm_data_length > message_size
					|| alarm_data_length > sizeof(arset->r1b.alarm.curr[0].alarm_data)
				) {

					CM_FATAL(); /* R1 protocol error */
				}
				else {

					CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

					alarm_curr->is_used           = LSA_TRUE;
					alarm_curr->is_pdev_alarm     = is_pdev_alarm;
					alarm_curr->is_pull_module    = is_pull_module;

					alarm_curr->alarm_sequence    = alarm_sequence;

					alarm_curr->api               = api;
					alarm_curr->slot_nr           = slot_nr;
					alarm_curr->subslot_nr        = subslot_nr;

					alarm_curr->alarm_type        = alarm_type;
					alarm_curr->alarm_tag         = alarm_tag;
					alarm_curr->alarm_data_length = alarm_data_length;

					if( alarm_data_length > 0 ) {

						CM_MEMCPY(alarm_curr->alarm_data, message_body + offset, alarm_data_length);
					}

					offset += alarm_data_length;
				}
			}
		}

		if( offset != message_size ) {
			CM_FATAL(); /* R1 protocol error */
		}

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE UPDATE END ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_update_end(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	/* ACTIVE: sending UPDATE_DONE to PASSIVE */

	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE ) {

		CM_FATAL();
	}
	else {

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_UPDATE_END;

		CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_END == 0U); /* no parameters */

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_UPDATE_END, message_size, dev->r1b.upd.msg_end);

		/* establish bridge */

		dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_ESTABLISHED;

		/***/

		{
		LSA_UINT16 ar_nr;

		for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

			CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

			if( cm_is_not_null(argr) ) {

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_PULL_PLUG;

				cm_sv_ar_submodule_state_action(argr); /* trigger: cm_sv_r1b_update_end() */
			}
		}
		}

		cm_sv_r1b_pdev_A_next(dev); /* trigger: established */
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_update_end(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){
	if(
		cm_is_null(dev)
		|| prefix != dev->r1b.upd.msg_end
	) {

		CM_FATAL();
	}
	else {

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_update_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
){
	/* PASSIVE: got UPDATE_END from ACTIVE */

	if(
		dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE
		|| message_size != CM_SV_R1BMSG_SIZE_UPDATE_END
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong bridge_state(%u) or wrong message_size(%u))"
			, dev->device_nr, dev->r1b.bridge_state, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UNUSED_ARG(message_body); /* no parameters */

		CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_END == 0U);

		cm_callback(dev->channel, CM_OK, &rb);

		/* establish bridge */

		dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_ESTABLISHED;

		cm_sv_r1b_pdev_A_next(dev); /* trigger: established */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE DIAG HELPERS =================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_diag_to_diag_add_elem(
	CM_SV_DIAG_PTR_TYPE diag,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT16 diag_type,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE elem
) {
	LSA_VOID_PTR_TYPE body = (LSA_VOID_PTR_TYPE)( diag + 1 );

	elem->api = api;
	elem->slot_nr = slot_nr;
	elem->subslot_nr = subslot_nr;
	elem->diag_tag = diag->diag_tag;
	elem->channel_number = diag->channel_number;
	elem->channel_properties = diag->channel_properties;
	elem->diag_type = diag_type;

	switch( elem->diag_type ) { /* note: "diag_type" is private, must use normative constants for diag add element / bridge messages */

	case CM_SV_DIAG_TYPE_CHANNEL:
		elem->u.ch.channel_error_type = diag->channel_error_type_usi;
		break;

	case CM_SV_DIAG_TYPE_EXT_CHANNEL:
		if( diag->data_length != 2 + 4 ) {
			CM_FATAL();
		}

		elem->u.ext.channel_error_type     = diag->channel_error_type_usi;
		elem->u.ext.ext_channel_error_type = CM_GET16_NTOH(body, 0);
		elem->u.ext.ext_channel_add_value  = CM_GET32_NTOH(body, 2);
		break;

	case CM_SV_DIAG_TYPE_QUALIFIED:
		if( diag->data_length != 2 + 4 + 4 ) {
			CM_FATAL();
		}

		elem->u.qua.channel_error_type          = diag->channel_error_type_usi;
		elem->u.qua.ext_channel_error_type      = CM_GET16_NTOH(body, 0);
		elem->u.qua.ext_channel_add_value       = CM_GET32_NTOH(body, 2);
		elem->u.qua.qualified_channel_qualifier = CM_GET32_NTOH(body, 6);
		break;

	case CM_SV_DIAG_TYPE_GENERIC:
		elem->u.gen.info_tag = diag->channel_error_type_usi; /* UserStructureIdentifier */
		elem->u.gen.info_length = diag->data_length;
		elem->u.gen.info_data = (CM_COMMON_MEM_U8_PTR_TYPE)body;
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_r1b_diag_list_to_message(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32* message_size,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_BOOL local_pdev /* TRUE: only local pdev, otherwise: all but remote-pdev */
) {
	/* send diagnoses to other head - PASSIVE to ACTIVE starts UPDATE */

	CM_SV_AP_PTR_TYPE ap;

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) is sending diagnoses to other head"
		, dev->device_nr
		);

	for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

		CM_SV_SLOT_PTR_TYPE sl;

		for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

			CM_SV_SUBSLOT_PTR_TYPE su;

			for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

				if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, su->subslot_nr) ) {

						CM_LIST_ENTRY_PTR_TYPE diag_list = cm_pdsv_diag_read(dev->channel, sl->slot_nr, su->subslot_nr);

						CM_PD_DIAG_PTR_TYPE pd_diag;

						for( CmListEach(pd_diag, diag_list, CM_PD_DIAG_PTR_TYPE) ) {

							if( !cm_sv_r1b_marshal_pdev_diag(ap->api, sl->slot_nr, su->subslot_nr, pd_diag, message_body, size, message_size, LSA_TRUE /* add */) ) {

								return LSA_FALSE;
							}
						}
					}
				}
				else {

					if( ! local_pdev ) {

						LSA_UINT16 diag_type;

						for( diag_type = 0; diag_type < CM_SV_DIAG_TYPE_MAX; ++diag_type ) {

							CM_SV_DIAG_PTR_TYPE sv_diag;

							for( CmListEach(sv_diag, &su->diag_list[diag_type], CM_SV_DIAG_PTR_TYPE) ) {

								CM_SV_DIAG_ADD_ELEMENT_TYPE elem;

								if( !sv_diag->r1_mirrored ) {

									cm_sv_r1b_diag_to_diag_add_elem(sv_diag, ap->api, sl->slot_nr, su->subslot_nr, diag_type, &elem);

									if( cm_sv_r1b_marshal_diag(&elem, message_body, size, message_size, LSA_TRUE /* add */) ) {

										sv_diag->r1_mirrored = LSA_TRUE;
									}
									else {

										return LSA_FALSE;
									}
								}
								else {

									/* diag already sent to other -> do nothing */
								}
							}
						}
					}
				}
			}
		}
	}

	return LSA_TRUE;
}


/*===========================================================================*/
/*=== R1-BRIDGE UPDATE ALARMS HELPERS =======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_add_alarm_to_message(
	CM_UPPER_RQB_CONST_PTR_TYPE rqb,
	LSA_UINT16 prio,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 * offset,
	LSA_UINT32 size
){
	CM_UPPER_ALARM_PTR_TYPE alarm = rqb->args.sv.ar_alarm_send;
	LSA_BOOL is_pdev_alarm = LSA_FALSE;

	if( CM_RQB_GET_OPCODE(rqb) == CM_OPC_SV_AR_ALARM_PDEV_GROUP || CM_RQB_GET_OPCODE(rqb) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL ) {

		is_pdev_alarm = LSA_TRUE;
	}

	if( prio != alarm->alarm_priority ) {
		CM_FATAL();
	}

	if( *offset + 22 + alarm->alarm_data_length > size ) {
		CM_FATAL();
	}

	CM_PUT16_HTON(message_body, *offset +  0, prio);
	CM_PUT16_HTON(message_body, *offset +  2, alarm->alarm_sequence);

	CM_PUT32_HTON(message_body, *offset +  4, alarm->api);
	CM_PUT16_HTON(message_body, *offset +  8, alarm->slot_nr);
	CM_PUT16_HTON(message_body, *offset + 10, alarm->subslot_nr);

	CM_PUT16_HTON(message_body, *offset + 12, cm_is_null(alarm->u.sv.void_ptr) ? 1 : 0); /* is pull module */
	CM_PUT16_HTON(message_body, *offset + 14, is_pdev_alarm ? 1 : 0);

	CM_PUT16_HTON(message_body, *offset + 16, alarm->alarm_type);
	CM_PUT16_HTON(message_body, *offset + 18, alarm->alarm_tag);
	CM_PUT16_HTON(message_body, *offset + 20, alarm->alarm_data_length);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM == 22U);

	if( alarm->alarm_data_length > 0 ) {

		CM_MEMCPY(message_body + *offset + 22U, alarm->alarm_data, alarm->alarm_data_length);
	}

	*offset += 22 + alarm->alarm_data_length;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_add_alarm_curr_to_message(
	CM_SV_R1_ALARM_CURR_PTR_TYPE alarm,
	LSA_UINT16 prio,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 * offset,
	LSA_UINT32 size
){
	if( *offset + 22 + alarm->alarm_data_length > size ) {
		CM_FATAL();
	}

	CM_PUT16_HTON(message_body, *offset + 0, prio);
	CM_PUT16_HTON(message_body, *offset + 2, alarm->alarm_sequence);

	CM_PUT32_HTON(message_body, *offset + 4, alarm->api);
	CM_PUT16_HTON(message_body, *offset + 8, alarm->slot_nr);
	CM_PUT16_HTON(message_body, *offset + 10, alarm->subslot_nr);

	CM_PUT16_HTON(message_body, *offset + 12, alarm->is_pull_module ? 1 : 0);
	CM_PUT16_HTON(message_body, *offset + 14, alarm->is_pdev_alarm ? 1 : 0);

	CM_PUT16_HTON(message_body, *offset + 16, alarm->alarm_type);
	CM_PUT16_HTON(message_body, *offset + 18, alarm->alarm_tag);
	CM_PUT16_HTON(message_body, *offset + 20, alarm->alarm_data_length);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM == 22U);

	if( alarm->alarm_data_length > 0 ) {

		CM_MEMCPY(message_body + *offset + 22U, alarm->alarm_data, alarm->alarm_data_length);
	}

	*offset += 22 + alarm->alarm_data_length;
}

/*===========================================================================*/
#else
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("***** not compiling server part (CM_CFG_MAX_SERVERS=" CM_STRINGIFY(CM_CFG_MAX_SERVERS) ")")
#endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
