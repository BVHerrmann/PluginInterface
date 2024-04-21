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
/*  F i l e               &F: cm_sv47.c                                 :F&  */
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
/*  - alarm-send                                                             */
/*  - ALMI (alarm initiator state machine)                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7470
#define CM_MODULE_ID       7470

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#if CM_ALARM_PRIORITY_LOW != 0 || CM_ALARM_PRIORITY_HIGH != 1
# error "check this! prio is used as an array-index"
#endif

#if CM_ALARM_PRIORITY_LOW != ACP_ALARM_PRIORITY_LOW || CM_ALARM_PRIORITY_HIGH != ACP_ALARM_PRIORITY_HIGH
# error "check this! acp-prio is passed in as parameter"
#endif

/*===========================================================================*/
/*=== ALMI SEND MAX-LENGTH ==================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_ar_almi_send_max_length(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_type
) {
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE ar_alcr = argr->alarm_cr;
	LSA_UINT16 len;

	if( cm_is_null(ar_alcr) ) { /* easy-supervisor */

		len = 0;
	}
	else {

		/* note: max_alarm_data_length >= CM_ALARM_DATA_LENGTH_MIN, see cm_ar_graph_insert_alarm_cr() */

		len = (LSA_UINT16)(ar_alcr->max_alarm_data_len - CM_ALARM_OFFSET_DATA);

		if( CM_ALARM_IS_DIAG_GROUP(alarm_type) ) {

			len -= CM_ALARM_MAINTENANCEITEM_LENGTH;
		}
	}

	return len; /* max. length of AlarmItem::DATA */
}

/*===========================================================================*/
/*=== ALMI ALARM PRIORITY ===================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_ar_almi_alarm_prio(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_type
) {
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE alcr = argr->alarm_cr;
	LSA_UINT16 alarm_priority;

	if(
		cm_is_not_null(alcr) /* actually always true */
		&& (alcr->alcr_properties & CM_ALCR_PROP_PRIO_MASK) == CM_ALCR_PROP_PRIO_ONLY_LOW
	) {

		alarm_priority = CM_ALARM_PRIORITY_LOW;
	}
	else {

		/* PNIO-Spec V2.3, Part 6: process alarm prio high, others low (V6.0i2.xx, TIA 1166054) */

		if( alarm_type == CM_ALARM_TYPE_PROCESS ) {

			alarm_priority = CM_ALARM_PRIORITY_HIGH;
		}
		else {

			alarm_priority = CM_ALARM_PRIORITY_LOW;
		}
	}

	return alarm_priority;
}

/*===========================================================================*/

static LSA_UINT16
cm_sv_ar_almi_plug_group_prio(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	return cm_sv_ar_almi_alarm_prio(argr, CM_ALARM_TYPE_PLUG);
}

/*===========================================================================*/
/*=== ALMI INIT =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		almi->sequence_number = 0;

		almi->flags = CM_SV_AR_ALMI_IDLE;

		almi->is_enabled = LSA_FALSE;

		almi->set2unk_enabled = LSA_FALSE;

		almi->do_rtc3_action = LSA_FALSE;

		almi->do_new_data = LSA_FALSE;

		CmListInitialize(&almi->queue);

		almi->curr = LSA_NULL;

		/***/

		almi->acp_rqb = LSA_NULL;
		almi->acp_mem = LSA_NULL;
		almi->rpl_rqb = LSA_NULL;

		almi->r1b.msg_alarm_begin_end = LSA_NULL;
	}
}

/*===========================================================================*/
/*=== ALMI ALLOC ============================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_almi_alloc(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	LSA_UINT16 len = cm_sv_ar_almi_send_max_length(argr, 0/*!*/);

	if( len != 0 ) { /* i.e., not easy-supervisor */

		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

			/***/

			almi->acp_rqb = cm_acp_alloc_rqb(channel);

			if( cm_is_null(almi->acp_rqb) ) {

				return LSA_FALSE;
			}

			CM_ACP_SET_OPCODE(almi->acp_rqb, 0/*unused*/);

			/***/

			{
			CM_ACP_LOWER_MEM_PTR_TYPE lower_mem;

			CM_ACP_ALLOC_ALARM_MEM(&lower_mem, len, channel->sysptr);
			almi->acp_mem = (CM_COMMON_MEM_U8_PTR_TYPE)lower_mem;

			if( cm_is_null(almi->acp_mem) ) {

				return LSA_FALSE;
			}
			}

			/***/

			almi->rpl_rqb = cm_upper_alloc_rqb(channel);

			if( cm_is_null(almi->rpl_rqb) ) {

				return LSA_FALSE;
			}

			CM_RQB_SET_OPCODE(almi->rpl_rqb, 0/*unused*/);

			/***/

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

				almi->r1b.msg_alarm_begin_end = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1B_ALARM_BEGIN_END_MESSAGE_MAX);

				if( cm_is_null(almi->r1b.msg_alarm_begin_end) ) {

					return LSA_FALSE;
				}
			}
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALMI UNDO-INIT ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		CM_ASSERT(almi->flags == CM_SV_AR_ALMI_IDLE);

		CM_ASSERT(CmListIsEmpty(&almi->queue));

		CM_ASSERT(cm_is_null(almi->curr));

		/***/

		if( cm_is_not_null(almi->rpl_rqb) ) {

			CM_ASSERT(CM_RQB_GET_OPCODE(almi->rpl_rqb) == 0/*unused*/);
			cm_upper_free_rqb(argr->ar_com.channel, almi->rpl_rqb);
			almi->rpl_rqb = LSA_NULL;
		}

		if( cm_is_not_null(almi->acp_rqb) ) {

			CM_ASSERT(CM_ACP_GET_OPCODE(almi->acp_rqb) == 0/*unused*/);
			cm_acp_free_rqb(argr->ar_com.channel, almi->acp_rqb);
			almi->acp_rqb = LSA_NULL;
		}

		if( cm_is_not_null(almi->acp_mem) ) {

			LSA_UINT16 ret_val;
			CM_ACP_FREE_ALARM_MEM(&ret_val, almi->acp_mem, argr->ar_com.channel->sysptr);
			CM_ASSERT(ret_val == LSA_RET_OK);
			LSA_UNUSED_ARG(ret_val);

			almi->acp_mem = LSA_NULL;
		}

		if(cm_is_not_null(almi->r1b.msg_alarm_begin_end)){

			cm_sv_r1b_prefix_free(argr->ar_com.channel, almi->r1b.msg_alarm_begin_end);
			almi->r1b.msg_alarm_begin_end = LSA_NULL;
		}
	}
}

/*===========================================================================*/
/*=== ALMI ENABLE ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_enable(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL enable_set2unk
) {
	LSA_UINT16 prio;

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) enable_set2unk(%u)"
		, argr->device_nr, argr->ar_nr
		, enable_set2unk
		);

	/***/

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		if( almi->is_enabled || almi->set2unk_enabled ) {

			CM_FATAL();
		}
		else {

			almi->is_enabled = LSA_TRUE;

			if( enable_set2unk ) {

				almi->set2unk_enabled = LSA_TRUE;
			}
		}

		cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: almi-enable */
	}
}

/*===========================================================================*/
/*=== ALMI DOWN =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_down(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	cm_sv_ar_almi_cancel(argr, 0, 0, LSA_NULL/*any*/);

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		if( almi->flags != CM_SV_AR_ALMI_IDLE ) {

			cm_sv_ar_almi_done(argr, prio);
		}
	}
}

/*===========================================================================*/
/*=== ALMI IS-IDLE ==========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_almi_is_idle(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		if( almi->flags != CM_SV_AR_ALMI_IDLE ) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev/ar(%u/%u) prio(%u) not idle, almi-flags(0x%x)"
				, argr->device_nr, argr->ar_nr
				, prio
				, almi->flags
				);
			return LSA_FALSE;
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALMI PLUG-OR-RELEASE ==================================================*/
/*===========================================================================*/

CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_sv_ar_almi_plug_or_release(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr);
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) != 0 ) {

		if( cm_is_not_null(almi->curr) ) { /* always true */

			return (CM_AR_GRAPH_SUBMODULE_PTR_TYPE)almi->curr->args.sv.ar_alarm_send->u.sv.void_ptr;
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== ALMI CANCEL ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_cancel(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api, /* see usage */
	LSA_UINT16 slot_nr, /* see usage */
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub /* LSA_NULL matches any alarm */
) {
	CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) ar_sub(0x%x) ap/sl/su(%u/%u/%u)"
		, argr->device_nr, argr->ar_nr
		, ar_sub
		, api, slot_nr, (ar_sub ? ar_sub->subslot_nr : 0)
		);

	/***/

	{
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

		/***/

		if( cm_is_not_null(almi->curr) ) { /* AP01280175, AP01270869 */

			CM_UPPER_RQB_PTR_TYPE curr = almi->curr;
			CM_UPPER_ALARM_PTR_TYPE param = curr->args.sv.ar_alarm_send;

			if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

				/* do nothing (must not cancel a running plug-group alarm; see CM_SV_AR_FL_PRM_MASK and CM_SV_AR_FL_NEW_MASK) */
			}
			else if( ! (cm_is_null(ar_sub) || param->u.sv.void_ptr == (LSA_VOID_PTR_TYPE)ar_sub) ) {

				/* do nothing */
			}
			else if( cm_is_null(almi->rpl_rqb) ) {

				CM_FATAL(); /* a bug */
			}
			else if( curr == almi->rpl_rqb ) {

				/* do nothing (already replaced) */
			}
			else if( CM_RQB_GET_OPCODE(almi->rpl_rqb) != 0/*unused*/ ) {

				CM_FATAL(); /* a bug */
			}
			else { /* replace "curr" by an internal RQB */

				CM_UPPER_RQB_PTR_TYPE rpl = almi->rpl_rqb;

				CM_RQB_SET_OPCODE(rpl, CM_OPC_SV_AR_ALARM_RPL_INTERNAL);

				CM_RQB_SET_HANDLE(rpl, CM_INVALID_HANDLE);

				CM_RQB_SET_RESPONSE(rpl, CM_OK_PENDING); /* sanity */

				*rpl->args.sv.ar_alarm_send = *param;

				rpl->args.sv.ar_alarm_send->alarm_data_length = 0; /* sanity */
				rpl->args.sv.ar_alarm_send->alarm_data = LSA_NULL; /* sanity */

				/***/

				almi->curr = rpl; /* replace */

				cm_sv_ar_almi_alarm_confirmation(argr, curr, CM_OK_CANCELLED);
			}
		}

		/***/

		{
		CM_UPPER_RQB_PTR_TYPE curr;

		for( CmListEach(curr, &almi->queue, CM_UPPER_RQB_PTR_TYPE) ) {

			LSA_BOOL do_cancel;

			if( cm_is_null(ar_sub) ) {

				do_cancel = LSA_TRUE; /* cancel "any" alarm */
			}
			else if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

				/* must not cancel a queued plug-group alarm; see ar_sub->sv.st_told = ... */
				/* note that return-of-submodule is part of the CM_ALARM_PLUG_GROUP but it is initiated by the user (CM_OPC_SV_AR_ALARM_SEND) */

				do_cancel = LSA_FALSE;
			}
			else { /* not plug-group */

				if( curr->args.sv.ar_alarm_send->u.sv.void_ptr == (LSA_VOID_PTR_TYPE)ar_sub ) {

					do_cancel = LSA_TRUE; /* cancel "this" alarm */
				}
				else {

					do_cancel = LSA_FALSE;
				}
			}

			if( do_cancel ) {

				CM_UPPER_RQB_PTR_TYPE temp = curr;

				curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

				CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);

				/***/

				cm_sv_ar_almi_alarm_confirmation(argr, temp, CM_OK_CANCELLED);
			}
		}
		}
	}
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) && cm_is_not_null(ar_sub) ) {

		cm_sv_arset_alarm_cancel(argr, api, slot_nr, ar_sub, LSA_FALSE);
	}
}

/*===========================================================================*/
/*=== ALMI ALARM REQUEST ====================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_ar_almi_alarm_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_AR_GRAPH_AP_CONST_PTR_TYPE ar_ap, /* LSA_NULL if Reporting System (TIA 1645641) */
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod, /* LSA_NULL if Reporting System (TIA 1645641) */
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub, /* LSA_NULL if pull module or Reporting System (TIA 1645641) */
	LSA_BOOL * do_led_info_ptr /* LSA_NULL or out-param LSA_TRUE if A.R.P. changes */
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_send;
	LSA_UINT16 prio = cm_sv_ar_almi_alarm_prio(argr, param->alarm_type);

	CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) alarm_type(%u) prio(%u) ap/sl/su(%u/%u/%u)"
		, argr->device_nr, argr->ar_nr
		, param->alarm_type, prio
		, param->api, param->slot_nr, param->subslot_nr
		);

	/*
	 * note: cm_ar_com_is_down() is handled further down because of system redundancy
	 */

	if( cm_is_not_null(ar_mod) ) {

		param->mod_ident = ar_mod->sv.real_sl->mod_ident;
	}
	else {

		param->mod_ident = 0; /* Reporting System (TIA 1645641) */
	}

	if( cm_is_not_null(ar_sub) ) {

		param->sub_ident = ar_sub->sv.real_su->sub_ident;
	}
	else {

		param->sub_ident = 0; /* pull module or Reporting System (TIA 1645641) */
	}

	/***/

	param->ar_diagnosis_state = CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO; /* see cm_sv_ar_almi_send_next() below */

	param->reserved = 0;

	param->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO;
	param->diag_generic_available = CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO;
	param->diag_submod_available  = CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;

	param->maintenance_status = 0;

	/***/

	param->alarm_priority = prio;

	param->alarm_sequence = 0xFFFF; /* see cm_sv_ar_almi_send_next() and cm_sv_arset_alarm_cancel() */

	param->cm_pnio_err = CM_PNIO_ERR_AlarmAndWrongSubmoduleState; /* sanity */

	param->u.sv.void_ptr = ar_sub; /* LSA_NULL if pull module or Reporting System (TIA 1645641) */

	/*
	 * plug-group alarms are special
	 */

	if( CM_RQB_GET_OPCODE(rb) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

		/* must not check a plug-group alarm; see ar_sub->sv.st_told = ... */
		/* note that return-of-submodule is part of the CM_ALARM_PLUG_GROUP but it is initiated by the user (CM_OPC_SV_AR_ALARM_SEND) */

		if( cm_ar_com_is_down(argr) ) {

			CM_FATAL(); /* a bug, see caller */
		}
		else {

			cm_sv_ar_almi_send_next(argr, prio, rb); /* trigger: alarm request (plug-group alarm) */
		}

		return CM_OK; /* pending */
	}

	/*
	 * check if physically plugged and determine the diag-bits
	 */

	if( cm_is_not_null(ar_ap) && cm_is_not_null(ar_mod) && cm_is_not_null(ar_sub) ) {

		if( ! CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su) ) {

			return CM_OK_CANCELLED; /* not plugged */
		}

		/***/

		if( CM_ALARM_IS_DIAG_GROUP(param->alarm_type) ) {

			/*
			 * PNIO-Spec, AlarmSpecifier.SubmoduleDiagnosisState, Usage within AlarmType
			 * PNIO-Spec, AlarmSpecifier.ChannelDiagnosis, Usage within AlarmType
			 * PNIO-Spec, AlarmSpecifier.ManufacturerSpecificDiagnosis, Usage within AlarmType
			 * PNIO-Spec, table AlarmType, column "Attached to the Diagnosis ASE"
			 */

			if(
				param->alarm_type == CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS
				&& param->alarm_data_length == 0
			) {

				/* note: bits must be zero */
			}
			else {

				/* note: set bits now, not later when sending */

				param->diag_submod_available = cm_sv_diag_maint_qual(argr->cm.sv.dev
					, ar_ap->sv.real_ap, ar_mod->sv.real_sl, ar_sub->sv.real_su
					, &param->diag_channel_available
					, &param->diag_generic_available
					, &param->maintenance_status
					) ? CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;

#if CM_CFG_SV_AR_HAS_MCONS
				if( ar_sub->out_via_mcr ) { /* is a m-consumer-submodule */

					LSA_UINT16 ext_err = cm_sv_mcons_diag_value(argr, LSA_NULL, ar_ap, ar_mod, ar_sub);

					if( ext_err != 0 ) { /* has (appearing) diagnosis */

						param->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES;
						param->diag_submod_available  = CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES;
					}
				}
#endif
			}
		}
	}

	/*
	 * system redundancy AR is special
	 */

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		LSA_UINT16 rc = cm_sv_arset_alarm_request(argr, rb, ar_sub, do_led_info_ptr);

		if( rc != CM_OK ) {

			if( rc == CM_OK_PENDING ) {

				return CM_OK; /* enqueued in AR-set */
			}

			return rc; /* e.g. first-AR or late */
		}
		else {

			/* proceed like "normal AR" */
		}
	}

	/*
	 * normal AR (not system-redundancy)
	 */

	if( cm_ar_com_is_down(argr) ) {

		return CM_OK_CANCELLED; /* late */
	}
	else {

		if( cm_is_not_null(ar_sub) ) { /* always true (because not plug-group) */

			if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) == 0 ) {

				/* note: CM_SV_AR_SMFL_IS_ARP is not a no-go, see explanation in AP01054520 */

				return CM_OK_CANCELLED; /* not after "appl-ready feedback" from user */
			}

			if( param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE ) {

				if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_ARP) != 0 ) { /* CM_SV_AR_SMFL_IS_PRMED but a.r.p. */

					ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_ARP; /* a return-of-submodule alarm resets the A.R.P. flag (AP01054520) */

					if( cm_is_not_null(do_led_info_ptr) ) {

						*do_led_info_ptr = LSA_TRUE;
					}
				}
			}
		}

		cm_sv_ar_almi_send_next(argr, prio, rb); /* trigger: alarm request (not plug-group alarm) */

		return CM_OK; /* pending */
	}
}

/*===========================================================================*/
/*=== ALMI SEND NEXT ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_send_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_UPPER_RQB_PTR_TYPE upper
) {
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u)"
		, argr->device_nr, argr->ar_nr
		, prio
		);

	/***/

	if( cm_is_not_null(upper) ) {

		CM_RQB_SET_RESPONSE(upper, CM_OK_PENDING); /* sanity */

		CmListInsertTail(&almi->queue, (CM_LIST_ENTRY_PTR_TYPE)upper);

		upper = LSA_NULL; /* sanity */
	}

	/***/

	if( ! almi->is_enabled ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ALMI not yet enabled"
			);
		return;
	}

	if( cm_is_not_null(almi->curr) || almi->flags != CM_SV_AR_ALMI_IDLE ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "alarm in progress or rtc3-action running"
			);
		return;
	}

	/*** ALMI is idle ***/

	if( almi->do_rtc3_action ) {

		almi->do_rtc3_action = LSA_FALSE;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			/* do nothing */
		}
		else {

			almi->flags = CM_SV_AR_ALMI_RTC3_ACTION;

			cm_sv_ar_end_of_predata_phase(argr);

			/***/

			if( ! CM_SV_AR_IS_RTCLASS3(argr) ) {

				cm_sv_ar_almi_rtc3_action_done(argr); /* AP01256809, note: indirect recursion */
			}
			else if( CM_SV_AR_IS_ADVANCED(argr) ) {

				if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PROV_ACT) != 0 ) {

					CM_FATAL();
				}
				else {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PROV_ACT;

					cm_arcq_request(argr, CM_ARCQ_OPC_PROV_ACTIVATE, 0); /* note: with config-provider-autostop */
				}
			}
			else {

				cm_sv_rpc_ready4rtclass3_request(argr);
			}

			/***/

			return; /* see cm_sv_ar_almi_rtc3_action_done() */
		}
	}

	/*** ALMI is idle ***/

	if( almi->do_new_data && almi->set2unk_enabled ) {

		almi->do_new_data = LSA_FALSE;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			/* do nothing */
		}
		else {

			almi->flags = CM_SV_AR_ALMI_NWDT_ACTION;

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) != 0 ) {

				CM_FATAL();
			}
			else {

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_REQ;

				cm_arcq_request(argr, CM_ARCQ_OPC_CONS_SET2UNK, 0);
			}

			return; /* see cm_sv_ar_almi_done() */
		}
	}

	/*** ALMI is idle ***/

	if( CmListIsEmpty(&almi->queue) ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "alarm-queue empty"
			);

		if( cm_is_not_null(argr->cm.sv.curr_arset_trigger) ) {

			if( cm_sv_ar_almi_is_idle(argr) ) {

				CM_UPPER_RQB_PTR_TYPE temp = argr->cm.sv.curr_arset_trigger;
				argr->cm.sv.curr_arset_trigger = LSA_NULL;

				argr->cm.sv.sysred_IOCARSR_backup = LSA_FALSE; /* reset */

				cm_callback(argr->ar_com.channel, CM_OK, &temp);
			}
		}
	}
	else if( cm_ar_com_is_down(argr) ) {

		/* do nothing */
	}
	else { /* send next alarm */

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* AP01525350 */

			if( argr->cm.sv.dev->CIR_enabled ) {

				CM_UPPER_RQB_PTR_TYPE next = CmListFirst(&almi->queue, CM_UPPER_RQB_PTR_TYPE);

				if( cm_is_not_null(next) ) { /* always true */

					CM_UPPER_ALARM_PTR_TYPE param = next->args.sv.ar_alarm_send;

					if( CM_ALARM_IS_PLUG_GROUP(param->alarm_type) ) { /* not: CM_OPC_SV_AR_ALARM_PLUG_GROUP (because of return-of-submodule) */

						CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "PBE running (delaying next alarm; CM_ALARM_IS_PLUG_GROUP)"
							);
						return; /* see cm_sv_ar_almi_newdata_action() */
					}
					else {

						if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_REQ) == 0 ) {

							CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE)param->u.sv.void_ptr;

							if( cm_is_not_null(ar_sub) ) { /* always true */

								if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

									/* user said appl-ready, must delay until after appl-ready-rpc.CNF */

									CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
										, "PBE running (delaying next alarm; before appl-ready-rpc.REQ)"
										);
									return; /* see cm_sv_ar_almi_newdata_action() */
								}
							}
						}
						else { /* cm_sv_marshal_module_diff_block() did reset CM_SV_AR_SMFL_PBE, thus submodule-knowledge gone */

							if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_CNF) == 0 ) {

								/* must delay until after appl-ready-rpc */

								CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
									, "PBE running (delaying next alarm; between appl-ready-rpc.REQ/CNF)"
									);
								return; /* see cm_sv_ar_almi_newdata_action() */
							}
						}
					}
				}
			}
			else {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "PBE running (delaying next alarm; not CIR_enabled)"
					);
				return; /* see cm_sv_ar_almi_newdata_action() */
			}
		}

		/***/

		if( (almi->flags & (CM_SV_AR_ALMI_ALARM_REQ | CM_SV_AR_ALMI_ALARM_CNF | CM_SV_AR_ALMI_WF_R1_ALARM_BEGIN_POSTED | CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED)) != 0 ) {

			CM_FATAL();
		}
		else {

			/*
			 * note: at the time of cm_sv_ar_almi_alarm_request() the submodule state was ok
			 *       and on changes of the submodule states, pending alarms are cancelled
			 */

			CM_UPPER_ALARM_PTR_TYPE param;

			CmListRemoveHead(&almi->queue, almi->curr, CM_UPPER_RQB_PTR_TYPE);

			param = almi->curr->args.sv.ar_alarm_send;

			/***/

			param->alarm_sequence = almi->sequence_number; /* assign right before sending (see cancelation) */

			almi->sequence_number = (LSA_UINT16)((almi->sequence_number + 1) & 0x07FF); /* see ACP_GET_ALARM_SPECIFIER */

			/***/

			if( CM_ALARM_IS_PLUG_GROUP(param->alarm_type) ) {

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != 0 ) {

					CM_FATAL();
				}
				else {

					switch( param->alarm_type ) {

					case CM_ALARM_TYPE_PLUG:
					case CM_ALARM_TYPE_RELEASED:
						almi->flags |= CM_SV_AR_ALMI_DO_SET2UNK;
						almi->flags |= CM_SV_AR_ALMI_IS_PLG_REL;
						argr->cm.sv.ar_flags |= CM_SV_AR_FL_EXP_PRM_END;

						if( CM_SV_SUBSLOT_IS_PDEV(param->subslot_nr) ) { /* local or remote pdev */

							if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, param->subslot_nr) ) {

								if( CM_SV_SUBSLOT_IS_PDEV_PORT(param->subslot_nr) ) {

									cm_sv_pd_prm_begin(argr, param->slot_nr, param->subslot_nr); /* Plug (local port) */
								}
								else {

									if(
										argr->cm.sv.r1_flags != 0
										&& CM_SV_SUBSLOT_IS_PDEV_INTERFACE(param->subslot_nr)
									) {

										/* TIA 2037122
										* - First AR is established
										* - Second device starts and AR comes
										* - Second AR becomes primary
										* - Plug alarms for the (local) PDEV need to be sent because there are no alarms on a First AR
										*/

										CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
											, "dev/ar(%u/%u) plug local PDEV interface (ap/sl/su(%u/%u/%05u))"
											, argr->device_nr, argr->ar_nr, param->api, param->slot_nr, param->subslot_nr
											);

										cm_sv_pd_prm_begin(argr, param->slot_nr, param->subslot_nr); /* Plug (local interface) */
									}
									else {

										CM_FATAL(); /* a bug */
									}
								}
							}
							else { /* remote pdev */

								if(
									argr->cm.sv.r1_flags != 0
									&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
								) {

									cm_sv_pd_prm_begin_remote(argr, param->slot_nr, param->subslot_nr); /* Plug (remote interface or port) */
								}
							}
						}
						break;

					case CM_ALARM_TYPE_RETURN_OF_SUBMODULE:
						almi->flags |= CM_SV_AR_ALMI_DO_SET2UNK;
						break;

					default:
						break;
					}
				}
			}

			if(
				(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0
				&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
			) {

				cm_sv_r1b_post_alarm_begin(argr, prio, almi);
			}
			else {

				/* R1: cm_sv_r1b_update_alarms() will do "begin" when the other head comes */

				almi->flags |= CM_SV_AR_ALMI_ALARM_REQ;
				cm_sv_ar_almi_send_next_continued(argr, almi);
			}
		}
	}
}

/*===========================================================================*/
/*=== ALMI SEND NEXT CONTINUED ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_send_next_continued(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_AR_ALMI_CONST_PTR_TYPE almi
) {
	CM_UPPER_ALARM_PTR_TYPE param = almi->curr->args.sv.ar_alarm_send;

	/***/

	if( CM_ALARM_IS_DIAG_GROUP(param->alarm_type) ) {

		/*
		 * PNIO-Spec, AlarmSpecifier.ARDiagnosisState, Usage within AlarmType
		 * PNIO-Spec, table AlarmType, column "Attached to the Diagnosis ASE"
		 *
		 * history: the global data-status (channel->acp.station_problem) must not effect this bit (AP00349091).
		 */

		if( argr->cm.sv.pi.pi_target ) {

			param->ar_diagnosis_state = CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES;
		}
	}

	CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "alarm_type(%u) ap/sl/su(%u/%u/%u) ar_diagnosis_state(%u) maintenance_status(%u) set2unk_enabled(%u)"
		, param->alarm_type
		, param->api, param->slot_nr, param->subslot_nr
		, param->ar_diagnosis_state, param->maintenance_status
		, almi->set2unk_enabled
		);

	/***/

	if(
		cm_is_null(almi->acp_rqb)
		|| CM_ACP_GET_OPCODE(almi->acp_rqb) != 0/*unused*/
		|| cm_is_null(almi->acp_mem)
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_COMMON_MEM_U8_PTR_TYPE dat = almi->acp_mem;
		LSA_UINT16 tag = param->alarm_tag;
		LSA_UINT16 len = param->alarm_data_length;

		if( param->maintenance_status != 0 ) { /* insert maintenance-block */

			/*
			 * BlockType(0x0F00), BlockLength, BlockVersion(1.0), ReservedU16, MaintenanceStatusU32
			 *
			 * NOTE: the maintenance-block is included in CM_ALARM_DATA_LENGTH_MIN (MaxAlarmDataLength)
			 */

#if CM_ALARM_MAINTENANCEITEM_LENGTH != (2+2+1+1 + 2+4 + 2/*USI*/)
#error "check this!"
#endif

			CM_PUT16_HTON(dat, 0, CM_BLOCK_TYPE_MAINTENANCE);
			CM_PUT16_HTON(dat, 2, 1+1+2+4);
			CM_PUT16_HTON(dat, 4, 0x0100);

			CM_PUT16_HTON(dat, 6, 0); /* reserved */
			CM_PUT32_HTON(dat, 8, param->maintenance_status); /* maintenance status */

			CM_PUT16_HTON(dat, 12+0, tag);

			tag = CM_ALARM_USI_MAINTENANCE; /* note: maybe things would have been easier without CM_ALARM_TYPE::alarm_tag */

			len += CM_ALARM_MAINTENANCEITEM_LENGTH;
			dat += CM_ALARM_MAINTENANCEITEM_LENGTH;
		}

		if( cm_is_not_null(param->alarm_data) ) {

			CM_MEMCPY(dat, param->alarm_data, param->alarm_data_length);
		}

		/* note: always sending a copy of "alarm_data" because of system redundancy (AP01208170) and alarm cancellation (AP01280175) */
		cm_acp_alarm_send(argr, param, tag, len, almi->acp_mem, almi->acp_rqb);
	}
}

/*===========================================================================*/
/*=== ALMI SEND DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_send_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	LSA_UINT16 prio = acp->args->alarm.data.priority;
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u) acp-rsp(%u) ar_flags(0x%08x) almi-flags(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prio
		, CM_ACP_GET_RESPONSE(acp)
		, argr->cm.sv.ar_flags
		, almi->flags
		);

	/***/

	if( acp != almi->acp_rqb || acp->args->alarm.data.user_data != almi->acp_mem ) {

		CM_FATAL(); /* a bug */
	}
	else if( cm_is_null(almi->curr) ) {

		CM_FATAL(); /* a bug */
	}
	else if( (almi->flags & (CM_SV_AR_ALMI_ALARM_REQ | CM_SV_AR_ALMI_ALARM_CNF)) != (CM_SV_AR_ALMI_ALARM_REQ | 0) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		if( CM_ACP_GET_RESPONSE(acp) != ACP_OK ) {

			/* do nothing, see cm_ar_com_down() in caller */
		}

		CM_ACP_SET_OPCODE(acp, 0/*unused*/);

		/***/

		almi->flags |= CM_SV_AR_ALMI_ALARM_CNF;

		cm_sv_ar_almi_done(argr, prio);

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: ALMI alarm-send.cnf */
		}
	}
}

/*===========================================================================*/
/*=== ALMI PRM-END INDICATION ===============================================*/
/*===========================================================================*/

CM_UPPER_ALARM_PTR_TYPE
cm_sv_ar_almi_prm_end_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 alarm_sequence_number
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr);
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];
	CM_UPPER_ALARM_PTR_TYPE param;

	CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u) alarm-seq(%u) ar_flags(0x%08x) almi-flags(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prio
		, alarm_sequence_number
		, argr->cm.sv.ar_flags
		, almi->flags
		);

	/***/

	if( cm_is_null(almi->curr) ) {

		param = LSA_NULL; /* actually a bug */
	}
	else {

		param = almi->curr->args.sv.ar_alarm_send;

		if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) == 0 || param != argr->cm.sv.plug_group_alarm ) {

			param = LSA_NULL; /* actually a bug */
		}
		else if( (almi->flags & CM_SV_AR_ALMI_ALARM_ACK) != 0 ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "prm-end.ind after alarm-ack"
				);
			param = LSA_NULL; /* protocol-violation */
		}
		else if( alarm_sequence_number != param->alarm_sequence ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong alarm-seq(%u), expected(%u)"
				, alarm_sequence_number, param->alarm_sequence
				);
			param = LSA_NULL; /* protocol-violation */
		}
		else {

			/* ok */
		}
	}

	/***/

	return param;
}

/*===========================================================================*/
/*=== ALMI PRM-BEGIN-END ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_pbe(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
) {

	if( argr->cm.sv.pr_appl_ready_cnt == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

			if( almi->flags != CM_SV_AR_ALMI_IDLE && cm_is_not_null(almi->curr) ) {

				if( almi->curr->args.sv.ar_alarm_send->u.sv.void_ptr == (LSA_VOID_PTR_TYPE)ar_sub ) {

					/* AP01193236: delay appl-ready after alarm-ack of current alarm; exceptions: plug or release alarm */

					if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) == 0 ) { /* not plug/release */

						if( (almi->flags & CM_SV_AR_ALMI_ALARM_ACK) == 0 ) { /* alarm-ack not yet seen (AP01525350) */

							if( (almi->flags & CM_SV_AR_ALMI_DO_ARDY) == 0 ) { /* always true */

								almi->flags |= CM_SV_AR_ALMI_DO_ARDY;

								argr->cm.sv.pr_appl_ready_cnt += 1;
							}
						}
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== ALMI ALARM-ACK INDICATION =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_ack_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE acp
) {
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];
	LSA_UINT8 reason;

	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u) ar_flags(0x%08x) almi-flags(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prio
		, argr->cm.sv.ar_flags
		, almi->flags
		);

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		cm_sv_arset_check_primary(argr);
	}

	/***/

	if( cm_ar_com_is_down(argr) ) {

		reason = CM_AR_REASON_PROT; /* actually unreachable */
	}
	else if( (almi->flags & CM_SV_AR_ALMI_ALARM_REQ) == 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "unexpected alarm-ack"
			);
		reason = CM_AR_REASON_PROT;
	}
	else if( (almi->flags & CM_SV_AR_ALMI_ALARM_ACK) != 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "alarm-ack already seen"
			);
		reason = CM_AR_REASON_PROT;
	}
	else if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) != 0 && (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_REQ) == 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "alarm-ack.ind before appl-ready.req"
			);
		reason = CM_AR_REASON_PROT;
	}
	else {

		CM_UPPER_ALARM_PTR_TYPE param = almi->curr->args.sv.ar_alarm_send;

		if( ! cm_acp_alarm_ack_matches(param, acp, &param->cm_pnio_err) ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "alarm-ack does not match"
				);
			reason = CM_AR_REASON_PROT;
		}
		else { /* accept */

			reason = CM_AR_REASON_NONE;

			if( param->cm_pnio_err != CM_PNIO_ERR_NONE ) {

				if( param->cm_pnio_err == CM_PNIO_ERR_AlarmAck_IOCARSR_backup ) { /* TIA 209760 */

					if( cm_is_null(argr->cm.sv.arset) ) {

						reason = CM_AR_REASON_PROT; /* ack-code is for IOCARSR only */
					}
					else if( argr == argr->cm.sv.arset->primary ) {

						argr->cm.sv.sysred_IOCARSR_backup = LSA_TRUE;

						cm_sv_arset_check_primary(argr); /* now backup */
					}
					else {

						/* already backup */
					}
				}
				else {

					/*
					 * must abort if the AR gets inconsistent
					 * - this is true for configuration changes (alarms of the plug-group)
					 * - this is not true e.g. for the pdev-alarms (the diagnosis is still stored in cm-pd)
					 */

					if( CM_ALARM_IS_PLUG_GROUP(param->alarm_type) ) {

						if( param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE ) {

							/* return-of-submodule is part of the CM_ALARM_PLUG_GROUP but it is initiated by the user */
						}
						else {

							CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "alarm-ack.ind for alarm_type(%u) with cm_pnio_err(0x%x)"
								, param->alarm_type, param->cm_pnio_err
								);
							reason = CM_AR_REASON_PROT;
						}
					}
				}
			}
		}
	}

	/***/

	if( reason != CM_AR_REASON_NONE ) {

		cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, reason, 0);
		cm_ar_com_down(argr, reason);
	}
	else {

		almi->flags |= CM_SV_AR_ALMI_ALARM_ACK;

		/***/

		if( (almi->flags & CM_SV_AR_ALMI_DO_SET2UNK) != 0 ) {

			if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) != 0 ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE)almi->curr->args.sv.ar_alarm_send->u.sv.void_ptr;

				if( cm_is_not_null(ar_sub) ) { /* always true */

					if( (ar_sub->sv.sm_flags & (CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP)) == (CM_SV_AR_SMFL_IS_PRMED | 0) ) { /* owned and ok/subst and "not A.R.P." */

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_EX_NEWDTA;
					}
				}
			}
			else {

				/* no in-data.ind for the return-of-submodule alarm (user uses the alarm.cnf) */
			}

			if( almi->set2unk_enabled ) {

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) != 0 ) {

					CM_FATAL();
				}
				else {

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_REQ;

					cm_arcq_request(argr, CM_ARCQ_OPC_CONS_SET2UNK, 0);
				}
			}
		}

		/***/

		if( (almi->flags & CM_SV_AR_ALMI_DO_ARDY) != 0 ) {

			almi->flags &= ~CM_SV_AR_ALMI_DO_ARDY;

			cm_sv_ar_appl_ready_cnt_decrement(argr); /* see AP01193236 */

			cm_sv_rpc_appl_ready_request(argr); /* trigger: alarm-ack.ind */
		}

		/***/

		if( cm_is_not_null(argr->cm.sv.arset) ) { //R1 VV: make a function (SRD, alarm-ack in primary)

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

			if( arset->primary == argr ) { /* alarm-ack in state "primary" */

				/* history: search for TIA 1493405 if you wonder why "st_told" is not set here */

				if(
					(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0
					&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
				) {

					if( (almi->flags & CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED) != 0 ) {

						CM_FATAL();
					}
					else {

						cm_sv_r1b_post_alarm_end(argr, prio, almi);
					}

					/***/

					if( CM_RQB_GET_OPCODE(almi->curr) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL ) { //R1 VV: make a function (pd-alarm mirror)

						CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

						r1pd_alarm->mirr_is_completed = LSA_TRUE;
					}
				}
			}
		}

		/***/

		cm_sv_ar_almi_done(argr, prio);
	}
}

/*===========================================================================*/
/*=== ALMI SET-TO-UNKNOWN DONE ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_set2unk_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) != CM_SV_AR_FL_NEW_DATA_REQ ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_CNF;

		/***/

		cm_sv_ar_almi_plug_group_done(argr); /* trigger: CM_SV_AR_FL_NEW_DATA_CNF */

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_FL_NEW_DATA_CNF */
		}
	}
}

/*===========================================================================*/
/*=== ALMI PLUG-GROUP DONE (PRM-END, APPL-READY, NEW-DATA) ==================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_plug_group_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr);

	cm_sv_ar_almi_done(argr, prio);
}

/*===========================================================================*/
/*=== ALMI DONE =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio
) {
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) prio(%u) ar_flags(0x%08x) almi-flags(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prio
		, argr->cm.sv.ar_flags
		, almi->flags
		);

	/***/

	if( (almi->flags & CM_SV_AR_ALMI_RTC3_ACTION) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) prio(%u) rtc3-action pending"
			, argr->device_nr, argr->ar_nr, prio
			);
		return;
	}

	if( (almi->flags & CM_SV_AR_ALMI_NWDT_ACTION) != 0 ) {

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_NEW_DATA_REQ | CM_SV_AR_FL_NEW_DATA_CNF)) == (CM_SV_AR_FL_NEW_DATA_REQ | 0) ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "set2unk.cnf pending"
				);
			return;
		}

		if( cm_ar_com_is_down(argr) ) {

			almi->flags = CM_SV_AR_ALMI_IDLE;
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) == CM_SV_AR_FL_NEW_MASK ) {

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_NEW_MASK;

			almi->flags = CM_SV_AR_ALMI_IDLE;

			cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: new-data-action done */

			return;
		}
		else {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "new-data.ind pending"
				);
			return;
		}
	}

	if( (almi->flags & CM_SV_AR_ALMI_ALARM_REQ) == 0 ) {

		/* note: almi-done may be called more than once if the AR aborts */

		return;
	}

	/* check local actions */

	if( (almi->flags & CM_SV_AR_ALMI_WF_R1_ALARM_BEGIN_POSTED) != 0  ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "r1-alarm-begin pending"
			);
		return;
	}

	if( (almi->flags & CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED) != 0  ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "r1-alarm-end pending"
			);
		return;
	}

	if( (almi->flags & CM_SV_AR_ALMI_ALARM_CNF) == 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "alarm.cnf pending"
			);
		return;
	}

	if( (almi->flags & CM_SV_AR_ALMI_DO_SET2UNK) != 0 ) { /* look at the ar_flags only if "plug-prio" */

		if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) != 0 ) {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PLG_REL) != 0 ) { /* prm-flags belong to pbe-phase (TIA 2368168) */

				if( cm_ar_com_is_down(argr) ) {

					/* note: in order to keep the change small we reset the ALMI flags here */
					/*       the effect is: not changing the flags of CM_SV_AR_FL_PRM_MASK and CM_SV_AR_FL_NEW_MASK below and not to "do_unpark" */

					almi->flags &= ~CM_SV_AR_ALMI_IS_PLG_REL;
					almi->flags &= ~CM_SV_AR_ALMI_DO_SET2UNK;

					/* continue, see below (alarm will be completed with CM_OK_CANCELLED) */
				}
				else {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "plug/release alarm pending because overlapping with prm-begin-end"
						);
					return;
				}
			}
			else {

				if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_PRM_END_IND | CM_SV_AR_FL_PRM_END_RSP)) == (CM_SV_AR_FL_PRM_END_IND | 0) ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "prm-end.rsp pending"
						);
					return;
				}

				if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) == (CM_SV_AR_FL_APPL_READY_REQ | 0) ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "appl-ready.cnf pending"
						);
					return;
				}
			}
		}

		if( almi->set2unk_enabled ) {

			if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_NEW_DATA_REQ | CM_SV_AR_FL_NEW_DATA_CNF)) == (CM_SV_AR_FL_NEW_DATA_REQ | 0) ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "set2unk.cnf pending"
					);
				return;
			}
		}
	}

	/* check remote actions (which may not come) */

	{
	LSA_UINT16 rsp;

	if( cm_ar_com_is_down(argr) ) {

		rsp = CM_OK_CANCELLED; /* failed */
	}
	else {

		if( (almi->flags & CM_SV_AR_ALMI_ALARM_ACK) == 0 ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "alarm-ack.ind pending"
				);
			return;
		}

		if( (almi->flags & CM_SV_AR_ALMI_DO_SET2UNK) != 0 ) { /* look at the ar_flags only if "plug-prio" */

			if( almi->set2unk_enabled ) {

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_DATA_IND) == 0 ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "new-data.ind pending"
						);
					return;
				}
			}
		}

		rsp = CM_OK; /* success */
	}

	/* all local and remote actions done */

	if( cm_is_null(almi->curr) ) {

		CM_FATAL();
	}
	else {

		LSA_BOOL do_unpark = LSA_FALSE;

		CM_UPPER_RQB_PTR_TYPE upper = almi->curr;
		almi->curr = LSA_NULL;

		if( (almi->flags & CM_SV_AR_ALMI_IS_PLG_REL) != 0 ) { /* reset prm-flags */

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_MASK;

			do_unpark = LSA_TRUE;
		}

		if( (almi->flags & CM_SV_AR_ALMI_DO_SET2UNK) != 0 ) { /* reset newdata-flags */

			if( almi->set2unk_enabled ) {

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_NEW_MASK;
			}
		}

		almi->flags = CM_SV_AR_ALMI_IDLE;

		cm_sv_ar_almi_alarm_confirmation(argr, upper, rsp);

		/***/

		if( do_unpark ) { /* TIA 1518690 */

			cm_sv_rpc_unpark(argr);
		}

		if( upper == argr->cm.sv.plug_group_alarm_rqb ) { /* TIA 2158678 */

			/* note: must "unpark" before "state_action" (more details see TIA 2158678) */
			/* note: CM_OPC_SV_AR_ALARM_PLUG_GROUP is only completed in this function */
			/* note: moving "unpark" before "confirmation" seems to be wrong (because PrmBegin cancels alarms) */

			cm_sv_ar_submodule_state_action(argr); /* trigger: almi-complete plug-group */
		}
	}
	}

	/***/

	cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: alarm confirmed */
}

/*===========================================================================*/
/*=== ALMI ALARM CONFIRMATION ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_alarm_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE upper,
	LSA_UINT16 rsp
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	CM_UPPER_ALARM_PTR_TYPE param = upper->args.sv.ar_alarm_send;

	CM_SV_TRACE_10(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) alarm_type(%u) prio(%u) alarm_seq(%u) ap/sl/su(%u/%u/%u) rsp(%u) opc(%u)"
		, argr->device_nr, argr->ar_nr
		, param->alarm_type, param->alarm_priority, param->alarm_sequence
		, param->api, param->slot_nr, param->subslot_nr
		, rsp
		, CM_RQB_GET_OPCODE(upper)
		);

	/***/

	switch( CM_RQB_GET_OPCODE(upper) ) {

	case CM_OPC_SV_AR_ALARM_SEND:
		cm_callback(channel, rsp, &upper);
		break;

	case CM_OPC_SV_AR_ALARM_PDEV_GROUP:
		if( cm_is_not_null(argr->cm.sv.arset) && argr->cm.sv.r1_flags != 0 ) {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
			CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

			if( r1pd_alarm->orig != upper ) {

				CM_FATAL(); /* a bug */
			}

			CM_RQB_SET_RESPONSE(upper, rsp);

			if( arset->primary == argr ) { //R1 VV: move this code to cm_sv_ar_almi_ack_indication()

				r1pd_alarm->orig_is_completed = LSA_TRUE;

				if( argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED ) {

					cm_sv_r1b_pdev_alarm_mirror_clr(argr->cm.sv.dev); /* see cm_sv_r1b_posted_pdev_alarm_mirror_clr() */
				}
				else {

					r1pd_alarm->wf_clr_posted = LSA_TRUE;

					cm_sv_r1b_posted_pdev_alarm_mirror_clr(argr->cm.sv.dev, r1pd_alarm->msg_mirror_clr); /* fake */
				}
			}
		}
		else { /* non-R1 case */

			cm_sv_pdev_callback(channel, upper, rsp);
		}
		break;

	case CM_OPC_SV_AR_ALARM_PLUG_GROUP:
		if(
			param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE /* return-of-submodule is a user-alarm, opcode CM_OPC_SV_AR_ALARM_SEND */
			|| param != argr->cm.sv.plug_group_alarm
			|| cm_is_not_null(argr->cm.sv.plug_group_alarm_rqb)
		) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.plug_group_alarm = LSA_NULL;

			LSA_UNUSED_ARG(rsp); /* note: if cnf(-) then the AR is aborting */

			argr->cm.sv.plug_group_alarm_rqb = upper;

			/* note: cm_sv_ar_submodule_state_action() is called in cm_sv_ar_almi_done() (TIA 2158678) */
		}
		break;

	case CM_OPC_SV_AR_ALARM_RPL_INTERNAL:
		if( upper != argr->cm.sv.almi[param->alarm_priority].rpl_rqb ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_RQB_SET_OPCODE(upper, 0/*unused*/);
		}
		break;

	case CM_OPC_SV_AR_ALARM_R1PD_INTERNAL:
		if(
			cm_is_not_null(argr->cm.sv.arset)
			&& upper == argr->cm.sv.arset->r1b.pd_alarm.mirr
		) {

			break; /* see cm_sv_r1b_handle_pdev_alarm_mirror_clr() */
		}
		else {

			CM_FATAL(); /* a bug (pre-allocated) */
		}
		break;

#if CM_CFG_SV_AR_HAS_MCONS
#error "this code is out of date (m-consumers are currently not supported). review backpressing."
	case CM_OPC_SV_AR_ALARM_MCONS:
		CM_ASSERT(param->alarm_type == CM_ALARM_TYPE_MCR_MISMATCH);
		if( cm_is_not_null(param->alarm_data) ) {
			cm_sv_upper_mem_free(channel, param->alarm_data);
		}
		LSA_UNUSED_ARG(rsp);
		cm_sv_internal_alarm_free(channel, upper);
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}

/*===========================================================================*/
/*=== ALMI RTC3 ACTION ======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_rtc3_action(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr); /* must be the plug-group prio, see AP00738250 (RTC3 startup adapted) */
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	if( almi->do_rtc3_action ) {

		CM_FATAL();
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) prio(%u) scheduling rtc3-action (rpc or prov-act)"
			, argr->device_nr, argr->ar_nr
			, prio
			);

		almi->do_rtc3_action = LSA_TRUE;

		cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: rtc3-action request */
	}
}

/*===========================================================================*/
/*=== ALMI RTC3 ACTION DONE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_rtc3_action_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr);
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) prio(%u) ar_flags(0x%08x) almi-flags(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prio
		, argr->cm.sv.ar_flags
		, almi->flags
		);

	if( almi->flags != CM_SV_AR_ALMI_RTC3_ACTION ) {

		CM_FATAL();
	}
	else {

		almi->flags = CM_SV_AR_ALMI_IDLE;

		/***/

		almi->set2unk_enabled = LSA_TRUE;

		cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: rtc3-action done (i.e., CM_SV_AR_ALMI_IDLE) */

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: rtc3-action done (i.e., CM_SV_AR_ALMI_IDLE) */
		}
	}
}

/*===========================================================================*/
/*=== ALMI NEW-DATA ACTION ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_almi_newdata_action(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 prio = cm_sv_ar_almi_plug_group_prio(argr);
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	if( almi->do_new_data ) {

		/* do nothing, already scheduled */
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) prio(%u) scheduling new-data"
			, argr->device_nr, argr->ar_nr
			, prio
			);

		almi->do_new_data = LSA_TRUE;

		cm_sv_ar_almi_send_next(argr, prio, LSA_NULL); /* trigger: new-data-action request */
	}

	cm_sv_ar_almi_send_next(argr, 1, LSA_NULL); /* trigger: end of pbe-phase (V6.1i1.33, TIA 2036823) */
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
