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
/*  F i l e               &F: cm_sv48.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AR things                                                     */
/*                                                                           */
/*  - alarm-indication                                                       */
/*  - ALMR (alarm responder state machine)                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7480
#define CM_MODULE_ID       7480

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_almr_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio
); /* forward */

static LSA_VOID
cm_sv_ar_almr_reprovide(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

/*===========================================================================*/
/*=== ALMR INIT =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almr_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		almr->delay = LSA_NULL;

		almr->acp_ind = LSA_NULL;

		almr->sequence_number = 0xFFFF; /* AP01392486 */

		almr->wf_alm_rsp = LSA_FALSE;

		almr->wf_ack_req = LSA_FALSE;

		almr->curr_ack = LSA_NULL;

		/***/

		almr->acp_rqb = LSA_NULL;

		almr->ack_rqb = LSA_NULL;
	}
}

/*===========================================================================*/
/*=== ALMR ALLOC ============================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_almr_alloc(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( argr->cm.sv.nr_of_submodules != 0 ) { /* i.e., not easy-supervisor */

		CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

			/***/

			almr->acp_rqb = cm_acp_alloc_rqb(channel);

			if( cm_is_null(almr->acp_rqb) ) {

				return LSA_FALSE;
			}

			CM_ACP_SET_OPCODE(almr->acp_rqb, 0/*unused*/);

			/***/

			if( ! channel->usr.sv.enable_AlarmResponder ) { /* see cm_sv_ar_alarm_indication() */

				almr->ack_rqb = cm_upper_alloc_rqb(channel);

				if( cm_is_null(almr->ack_rqb) ) {

					return LSA_FALSE;
				}

				CM_RQB_SET_OPCODE(almr->ack_rqb, 0/*unused*/);

				/* note: with TIA 738691 (CM-CL) alarm-ack has changed, the cm-rqb is just a placeholder (content is don't care) */

				CM_RQB_SET_HANDLE(almr->ack_rqb, CM_INVALID_HANDLE); /* sanity */
				CM_MEMSET(almr->ack_rqb->args.sv.ar_alarm_ack, 0, sizeof(*almr->ack_rqb->args.sv.ar_alarm_ack)); /* sanity */
			}
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALMR UNDO-INIT ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almr_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		CM_ASSERT(cm_is_null(almr->delay));

		CM_ASSERT(cm_is_null(almr->acp_ind));

		CM_ASSERT( ! almr->wf_alm_rsp);

		/* almr->wf_ack_req .. may be set */

		CM_ASSERT(cm_is_null(almr->curr_ack));

		/***/

		if( cm_is_not_null(almr->acp_rqb) ) {

			CM_ASSERT(CM_ACP_GET_OPCODE(almr->acp_rqb) == 0/*unused*/);
			cm_acp_free_rqb(argr->ar_com.channel, almr->acp_rqb);
			almr->acp_rqb = LSA_NULL;
		}

		if( cm_is_not_null(almr->ack_rqb) ) {

			CM_ASSERT(CM_RQB_GET_OPCODE(almr->ack_rqb) == 0/*unused*/);
			cm_upper_free_rqb(argr->ar_com.channel, almr->ack_rqb);

			almr->ack_rqb = LSA_NULL;
		}
	}
}

/*===========================================================================*/
/*=== ALMR ENABLE ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almr_enable(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	LSA_UINT16 prio;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	for( prio = 0; prio < 2; ++prio ) {

		cm_sv_ar_almr_next(argr, prio);
	}
}

/*===========================================================================*/
/*=== ALMR DOWN =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almr_down(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	LSA_UINT16 prio;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		if( cm_is_not_null(almr->delay) ) {

			CM_UPPER_RQB_PTR_TYPE upper = almr->delay;
			almr->delay = LSA_NULL;

			cm_sv_ar_almr_reprovide(argr->cm.sv.dev, upper);

			if( cm_is_null(almr->acp_ind) ) {
				CM_FATAL();
			}

			cm_acp_request_lower (argr->ar_com.channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, almr->acp_ind);
			almr->acp_ind = LSA_NULL;
		}
	}
}

/*===========================================================================*/
/*=== ALMR IS-IDLE ==========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_almr_is_idle(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		if( almr->wf_alm_rsp ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "prio(%u) alarm.RSP outstanding"
				, prio
				);
			return LSA_FALSE;
		}

		if( cm_is_not_null(almr->curr_ack) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "prio(%u) alarm-ack.CNF outstanding"
				, prio
				);
			return LSA_FALSE;
		}

		if( cm_is_not_null(almr->delay) ) {

			CM_FATAL(); /* a bug */
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALMR NEXT =============================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_almr_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio
) {
	CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u)"
		, argr->device_nr, argr->ar_nr
		, prio
		);

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "still in connect-phase"
		);
		return;
	}

	if( cm_is_not_null(almr->curr_ack) ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "alarm-ack running"
			);
		return;
	}

	if( cm_is_null(almr->delay) ) {

		/* nothing to do */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE upper = almr->delay;
		almr->delay = LSA_NULL;

		almr->wf_alm_rsp = LSA_TRUE;

		if( almr->wf_ack_req ) {
			CM_FATAL(); /* a bug */
		}

		cm_callback(argr->ar_com.channel, CM_OK, &upper);
	}
}

/*===========================================================================*/
/*=== ALARM INDICATION ======================================================*/
/*===========================================================================*/

CM_ACP_LOWER_RQB_PTR_TYPE
cm_sv_ar_alarm_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	ACP_UPPER_ALARM_DATA_PTR_TYPE al = &acp->args->alarm.data;

	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) prio(%u) alarm_type(%u) enable_AlarmResponder(%u)"
		, argr->device_nr, argr->ar_nr
		, prio
		, al->alarm_type
		, channel->usr.sv.enable_AlarmResponder
		);

	if( cm_ar_com_is_down(argr) ) {

		/* unreachable */
	}
	else if( prio > 1 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		/***/

		if(
			cm_is_not_null(almr->delay)
			|| almr->wf_alm_rsp
			|| almr->wf_ack_req
		) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "prio(%u) another alarm but delay(0x%x) or wf_alm_rsp(%u) or wf_ack_req(%u)"
				, prio
				, almr->delay
				, almr->wf_alm_rsp
				, almr->wf_ack_req
				);
			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT);
		}
		else {

			LSA_UINT16 al_sequence_number = (LSA_UINT16)(al->alarm_specifier & 0x07FF); /* see ACP_GET_ALARM_SPECIFIER() */

			if(
				almr->sequence_number == 0xFFFF   /* PNIO-Spec: The Alarm receiver shall accept an arbitrary value as start value. (AP01392486) */
				|| cm_is_not_null(argr->cm.sv.arset) /* PNIO-Spec: The SequenceNumber may start for each AR of an AR set with every switchover with an arbitrary value. (AP01392486; user must check, CM cannot) */
			) {

				almr->sequence_number = al_sequence_number;
			}

			if( al_sequence_number != almr->sequence_number ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "seq(%u) not expected(%u)"
					, al_sequence_number, almr->sequence_number
					);
				cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
				cm_ar_com_down(argr, CM_AR_REASON_PROT);
			}
			else {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				cm_ar_graph_find_api_slot_subslot(argr, al->api, al->slot_nr, al->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

				if( cm_is_null(ar_sub) ) {

					CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
						, "ap/sl/su(%u/%u/%u) not in AR"
						, al->api, al->slot_nr, al->subslot_nr
						);
					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
					cm_ar_com_down(argr, CM_AR_REASON_PROT);
				}
				else { /* accepting the alarm.ind */

					almr->sequence_number = (LSA_UINT16)((almr->sequence_number + 1) & 0x07FF); /* see ACP_GET_ALARM_SPECIFIER */

					/***/

					if( channel->usr.sv.enable_AlarmResponder ) {

						CM_UPPER_RQB_PTR_TYPE upper = cm_sv_dev_get_alarm_notnull(LSA_NULL, argr, prio, CM_OPC_SV_AR_ALARM_IND);

						cm_acp_alarm_copy_ind(upper->args.sv.ar_alarm_ind, acp, 0);

						almr->delay = upper;

						almr->acp_ind = acp; /* note: "user_data" is passed on to user, see cm_acp_alarm_copy_ind() */

						cm_sv_ar_almr_next(argr, prio);

						return LSA_NULL; /* in progress */
					}
					else { /* acknowledge internally */

						CM_UPPER_RQB_PTR_TYPE ack = almr->ack_rqb;
						CM_ACP_LOWER_RQB_PTR_TYPE ack_acp = almr->acp_rqb;

						if(
							cm_is_null(ack) || CM_RQB_GET_OPCODE(ack) != 0/*unused*/
							|| cm_is_null(ack_acp) || CM_ACP_GET_OPCODE(ack_acp) != 0/*unused*/
							|| cm_is_not_null(almr->curr_ack)
						) {

							CM_FATAL(); /* a bug */
						}
						else {

							cm_acp_alarm_copy_ack(ack_acp, al, LSA_NULL, CM_PNIO_ERR_AlarmTypeNotSupported);

							CM_RQB_SET_OPCODE(ack, CM_OPC_SV_AR_ALARM_ACK_INTERNAL); /* see note in cm_sv_ar_almr_alloc() */

							almr->curr_ack = ack;

							cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp);

							cm_acp_alarm_ack_send (argr, ack_acp);

							return LSA_NULL; /* handled */
						}
					}
				}
			}
		}
	}

	return acp; /* caller does reprovide */
}

/*===========================================================================*/
/*=== ALARM RESPONSE ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_ind;
	LSA_UINT16 prio = param->alarm_priority;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, 0);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else if( prio > 1 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) prio(%u) > 1"
			, argr->device_nr, argr->ar_nr
			, prio
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		if( ! almr->wf_alm_rsp ) {

			CM_FATAL(); /* a user-bug */
		}
		else {

			almr->wf_alm_rsp = LSA_FALSE;

			/***/

			if( almr->wf_ack_req || cm_is_null(almr->acp_ind) ) {

				CM_FATAL(); /* a bug */
			}
			else {

				/* only when alarm.rsp is done before ack.req we need only one */
				/* alarm-resource per priority. see the documentation. */

				almr->wf_ack_req = LSA_TRUE;

				cm_sv_ar_almr_reprovide(argr->cm.sv.dev, rb);

				cm_acp_request_lower (argr->ar_com.channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, almr->acp_ind);
				almr->acp_ind = LSA_NULL;

				/***/

				if( cm_ar_com_is_down(argr) ) {

					cm_sv_ar_disconnect_indication_join(argr); /* trigger: ALMR alarm.rsp */
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== ALMR REPROVIDE ========================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_almr_reprovide(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_ind; /* see cm_sv_ar_alarm_indication() */

	param->alarm_data = LSA_NULL;
	param->alarm_data_length = 0;

	cm_sv_dev_reprovide_alarm(dev, rb);
}

/*===========================================================================*/
/*=== ALARM-ACK SEND ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_ack(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_ack;
	LSA_UINT16 prio = param->alarm_priority;
	CM_ACP_LOWER_RQB_PTR_TYPE acp = LSA_NULL;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_EXP_USER_REQ);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( prio > 1 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) prio(%u) > 1"
			, argr->device_nr, argr->ar_nr
			, prio
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		if( cm_is_not_null(almr->curr_ack) ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev/ar(%u/%u) prio(%u) alarm-ack.REQ already running"
				, argr->device_nr, argr->ar_nr
				, prio
				);

			rsp = CM_ERR_SEQUENCE;
		}
		else if( ! almr->wf_ack_req ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev/ar(%u/%u) prio(%u) alarm-ack.REQ before alarm.RSP"
				, argr->device_nr, argr->ar_nr
				, prio
				);

			rsp = CM_ERR_SEQUENCE;
		}
		else { /* note: "wf_ack_req" implies "channel->usr.sv.enable_AlarmResponder" */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: ALMR check 'wf_ack_req && submod-exists' could be stronger (checking submod-pointer; abort if mismatch).")
#endif

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = LSA_NULL;

			cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

			if( cm_is_null(ar_sub)  ) {

				CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev/ar(%u/%u) prio(%u) ap/sl/su(%u/%u/%u) not in AR"
					, argr->device_nr, argr->ar_nr
					, prio
					, param->api, param->slot_nr, param->subslot_nr
					);

				rsp = CM_ERR_PARAM;
			}
			else {

				/* note: every alarm must be acknowledged (no dependency on "pulled" or "not owned") */

				CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "dev/ar(%u/%u) prio(%u) alarm-ack.req"
					, argr->device_nr, argr->ar_nr
					, prio
					);

				almr->wf_ack_req = LSA_FALSE;

				/***/

				if( cm_ar_com_is_down(argr) ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "already down"
						);

					rsp = CM_OK_CANCELLED; /* late */
				}
				else {

					acp = almr->acp_rqb;

					if( cm_is_null(acp) || CM_ACP_GET_OPCODE(acp) != 0/*unused*/ ) {

						CM_FATAL(); /* a bug */

						rsp = CM_ERR_RESOURCE; /* unreachable */
					}
					else {

						cm_acp_alarm_copy_ack(acp, LSA_NULL, param, param->cm_pnio_err);

						almr->curr_ack = rb; /* pending */

						rsp = CM_OK_PENDING;
					}
				}
			}
		}
	}

	/***/

	if( rsp != CM_OK_PENDING || cm_is_null(acp) || cm_is_null(argr) ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		cm_acp_alarm_ack_send(argr, acp);
	}
}

/*===========================================================================*/
/*=== ALARM ACK SEND DONE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_alarm_ack_send_done (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	LSA_UINT16 prio = acp->args->alarm.data.priority;

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) prio(%u) acp-rsp(%u)"
		, argr->device_nr, argr->ar_nr
		, prio
		, CM_ACP_GET_RESPONSE(acp)
		);

	/* note: if acp-rsp != ACP_OK, the AR was already aborted with CM_AR_REASON_ALACK in cm_acp_alarm_ack_send_done() */

	if( prio > 1 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_AR_ALMR_PTR_TYPE almr = &argr->cm.sv.almr[prio];

		if( cm_is_null(almr->curr_ack) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE upper = almr->curr_ack;
			almr->curr_ack = LSA_NULL;

			if( CM_RQB_GET_OPCODE(upper) == CM_OPC_SV_AR_ALARM_ACK_INTERNAL ) {

				if( upper != almr->ack_rqb ) {

					CM_FATAL();
				}
				else {

					CM_RQB_SET_OPCODE(upper, 0/*unused*/);
				}
			}
			else {

				LSA_UINT16 rsp;

				if( CM_ACP_GET_RESPONSE(acp) == ACP_OK ) {

					rsp = CM_OK;
				}
				else { /* ACP_OK_CANCEL(?), ACP_ERR_ASRT */

					rsp = CM_OK_CANCELLED;
				}

				cm_callback(argr->ar_com.channel, rsp, &upper);
			}

			/***/

			if( acp != almr->acp_rqb ) {

				CM_FATAL();
			}
			else {

				CM_ACP_SET_OPCODE(acp, 0/*unused*/);
			}

			/***/

			if( cm_ar_com_is_down(argr) ) { /* case: miss.ind before alarm-ack.cnf */

				cm_sv_ar_disconnect_indication_join(argr); /* trigger: ALMR alarm-ack-send.cnf */
			}
			else {

				cm_sv_ar_almr_next(argr, prio);
			}
		}
	}
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
