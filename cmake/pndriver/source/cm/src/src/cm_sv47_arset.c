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
/*  F i l e               &F: cm_sv47_arset.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AR Alarm things                                               */
/*                                                                           */
/*  - system redundancy                                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7471
#define CM_MODULE_ID       7471

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== AR-SET ALARM STOP / PLUG-GROUP ========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_arset_alarm_stop_plug_group(
	CM_AR_GRAPH_PTR_TYPE argr, /* see cm_sv_arset_alarm_stop() */
	CM_UPPER_RQB_PTR_TYPE alarm,
	LSA_BOOL do_free
) {
	CM_UPPER_ALARM_PTR_TYPE param = alarm->args.sv.ar_alarm_send;

	if(
		param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE /* return-of-submodule is a user-alarm, opcode CM_OPC_SV_AR_ALARM_SEND */
		|| param != argr->cm.sv.plug_group_alarm
		|| cm_is_not_null(argr->cm.sv.plug_group_alarm_rqb)
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		/* reset the submodule state so that the alarm will be repeated on the new primary AR (AP01479479)
		 *
		 * must not use
		 * - cm_sv_ar_submodule_state_changed(...);
		 * - cm_sv_ar_almi_alarm_confirmation(...);
		 * because may recurse via cm_sv_ar_submodule_state_action() -> cm_sv_arset_check_primary() -> cm_sv_arset_alarm_stop().
		 */

		LSA_UINT16 st_told = CM_SUB_STATE_PULLED; /* restore to "owned" */

		if( param->alarm_type == CM_ALARM_TYPE_RELEASED ) {

			CM_SUB_STATE_SET_AR_INFO(st_told, CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED); /* restore to "not owned" */
		}

		if( cm_is_not_null(param->u.sv.void_ptr) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE)param->u.sv.void_ptr;

			LSA_BOOL aplrdy_cancelled = cm_sv_ar_submodule_state_reset(ar_sub, LSA_TRUE/*dirty*/, LSA_TRUE/*wrong*/);

			if( aplrdy_cancelled ) { /* TIA 2293689 */

				cm_sv_ar_appl_ready_cnt_decrement(argr);

				cm_sv_rpc_appl_ready_request(argr); /* trigger: cancellation of CM_SV_AR_SMFL_EX_APLRDY */
			}

			ar_sub->sv.st_told = st_told;
		}
		else { /* pull module, see cm_sv_ar_almi_alarm_request() */

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, 0, LSA_NULL, &ar_mod, LSA_NULL);

			if( cm_is_null(ar_mod) ) {

				CM_FATAL();
			}
			else {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					LSA_BOOL aplrdy_cancelled = cm_sv_ar_submodule_state_reset(ar_sub, LSA_TRUE/*dirty*/, LSA_TRUE/*wrong*/);

					if( aplrdy_cancelled ) {

						CM_FATAL();
					}

					ar_sub->sv.st_told = st_told;
				}
			}
		}

		/***/

		if( do_free ) {

			argr->cm.sv.plug_group_alarm = LSA_NULL;

			argr->cm.sv.plug_group_alarm_rqb = alarm;
		}
	}
}

/*===========================================================================*/
/*=== AR-SET ALARM STOP =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_arset_alarm_stop(
	CM_AR_GRAPH_PTR_TYPE argr /* the "old" primary AR */
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

	if( arset->is_aborting ) {

		/* do nothing */
	}
	else {

		{
		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

			/***/

			if( cm_is_not_null(almi->curr) ) {

				CM_UPPER_RQB_PTR_TYPE curr = almi->curr;

				if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

					cm_sv_arset_alarm_stop_plug_group(argr, curr, LSA_FALSE);
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

					CM_UPPER_ALARM_PTR_TYPE param = curr->args.sv.ar_alarm_send;

					CM_UPPER_RQB_PTR_TYPE rpl = almi->rpl_rqb;

					CM_RQB_SET_OPCODE(rpl, CM_OPC_SV_AR_ALARM_RPL_INTERNAL);

					CM_RQB_SET_HANDLE(rpl, CM_INVALID_HANDLE);

					CM_RQB_SET_RESPONSE(rpl, CM_OK_PENDING); /* sanity */

					*rpl->args.sv.ar_alarm_send = *param;

					rpl->args.sv.ar_alarm_send->alarm_data_length = 0; /* sanity */
					rpl->args.sv.ar_alarm_send->alarm_data = LSA_NULL; /* sanity */

					/***/

					almi->curr = rpl; /* replace */

					CmListInsertHead(&almi->queue, (CM_LIST_ENTRY_PTR_TYPE)curr); /* push back */

					almi->sequence_number = param->alarm_sequence; /* restore, see too cm_sv_arset_alarm_cancel() */
				}
			}

			/***/

			arset->alarm[prio].sequence_number = almi->sequence_number;

			/***/

			{
			CM_UPPER_RQB_PTR_TYPE curr;

			for( CmListEach(curr, &almi->queue, CM_UPPER_RQB_PTR_TYPE) ) {

				if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

					CM_UPPER_RQB_PTR_TYPE temp = curr;

					curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

					CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);

					/***/

					cm_sv_arset_alarm_stop_plug_group(argr, temp, LSA_TRUE);
				}

				/***/

				if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL ) {

					CM_UPPER_RQB_PTR_TYPE temp = curr;

					curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

					CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);

					/***/

					if( temp != arset->r1b.pd_alarm.mirr ) {
						CM_FATAL();
					}

					/* just dequeue */
				}
			}
			}

			/***/

			CmListAppend(&almi->queue, &arset->alarm[prio].queue); /* append AR-set to ALMI */

			CmListAppend(&arset->alarm[prio].queue, &almi->queue); /* append ALMI to empty AR-set */
		}
		}
	}
}

/*===========================================================================*/
/*=== AR-SET ALARM RESTART ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_arset_alarm_restart(
	CM_AR_GRAPH_PTR_TYPE argr /* the new primary AR */
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

	{
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

		alarm_curr->is_used = LSA_FALSE;

		/***/

		argr->cm.sv.almi[prio].sequence_number = arset->alarm[prio].sequence_number;

		/***/

		while( ! CmListIsEmpty(&arset->alarm[prio].queue) ) {

			CM_UPPER_RQB_PTR_TYPE upper;

			CmListRemoveHead(&arset->alarm[prio].queue, upper, CM_UPPER_RQB_PTR_TYPE);

			/* note: need not redo cm_sv_ar_alarm_send_params_ok(), see consistency checks */
			/* note: need not redo cm_sv_ar_alarm_send_format_ok(), checks do not depend on the AR */

			{
			CM_UPPER_ALARM_PTR_TYPE param = upper->args.sv.ar_alarm_send;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			param->session_key = argr->session_key;
			param->ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(argr->session_key);

			cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

			if( cm_is_null(ar_sub) ) { /* submodule not in new AR (CIR only) */

				cm_sv_ar_almi_alarm_confirmation(argr, upper, CM_OK_CANCELLED);
			}
			else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) == 0 ) {

				cm_sv_ar_almi_alarm_confirmation(argr, upper, CM_OK_CANCELLED); /* not after "appl-ready feedback" from user */
			}
			else {

				param->u.sv.void_ptr = ar_sub;

				cm_sv_ar_almi_send_next(argr, prio, upper); /* trigger: arset-alarm-restart */
			}
			}
		}
	}
	}

	/***/

	cm_sv_ar_submodule_state_action(argr); /* trigger: arset-alarm-restart */
}

/*===========================================================================*/
/*=== AR-SET ALARM CANCEL ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_arset_alarm_cancel(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api, /* see usage */
	LSA_UINT16 slot_nr, /* see usage */
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub, /* LSA_NULL matches any alarm */
	LSA_BOOL is_r1_not_acked
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

	{
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		{
		CM_UPPER_RQB_PTR_TYPE curr;

		for( CmListEach(curr, &arset->alarm[prio].queue, CM_UPPER_RQB_PTR_TYPE) ) {

			CM_UPPER_ALARM_PTR_TYPE param = curr->args.sv.ar_alarm_send;
			LSA_BOOL do_cancel;

			if( cm_is_null(ar_sub) ) {

				do_cancel = LSA_TRUE; /* cancel "any" alarm */
			}
			else if( param->api == api && param->slot_nr == slot_nr && param->subslot_nr == ar_sub->subslot_nr ) {

				do_cancel = LSA_TRUE; /* cancel "this" alarm */
			}
			else {

				do_cancel = LSA_FALSE;
			}

			if( do_cancel ) {

				CM_UPPER_RQB_PTR_TYPE temp = curr;

				curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

				CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);

				/***/

				if( CM_RQB_GET_OPCODE(temp) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

					CM_FATAL();
				}
				else {

					if( param->alarm_sequence != 0xFFFF ) { /* advance AR-set sequence number if cancelling the alarm that was ALMI::curr (AP01320211) */

						arset->alarm[prio].sequence_number = (LSA_UINT16)((arset->alarm[prio].sequence_number + 1) & 0x07FF);
					}

					param->session_key = argr->session_key;
					param->ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(argr->session_key);

					if( argr->cm.sv.r1_flags != 0 ) {

						if( CM_RQB_GET_OPCODE(temp) == CM_OPC_SV_AR_ALARM_PDEV_GROUP ) {

							if( temp != arset->r1b.pd_alarm.orig ) {
								CM_FATAL();
							}

							if( !arset->is_aborting ) {

								/* keep the original alarm for cm_sv_r1b_pdev_alarm_resubmit() */
								continue;
							}
						}

						if( CM_RQB_GET_OPCODE(temp) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL ) {

							if( temp != arset->r1b.pd_alarm.mirr ) {
								CM_FATAL();
							}

							/* not: cm_sv_ar_almi_alarm_confirmation(), see cm_sv_r1b_handle_pdev_alarm_mirror_clr() */
							continue;
						}
					}

					if( is_r1_not_acked ) {

						cm_sv_ar_almi_alarm_confirmation(argr, temp, CM_OK_R1_NOT_ACKED);
					}
					else {

						cm_sv_ar_almi_alarm_confirmation(argr, temp, CM_OK_CANCELLED);
					}
				}
			}
		}
		}
	}
	}
}

/*===========================================================================*/
/*=== AR-SET ALARM REQUEST ==================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_arset_alarm_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL * do_led_info_ptr /* LSA_NULL or out-param LSA_TRUE if A.R.P. changes */
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
	LSA_UINT16 rc;

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {
		CM_FATAL(); /* a bug */
	}

	cm_sv_arset_check_primary(argr);

	if( arset->is_aborting ) {

		rc = CM_OK_CANCELLED; /* late */
	}
	else if( cm_is_not_null(arset->first) ) { /* TIA 535510 */

		rc = CM_OK_CANCELLED; /* see PNIO-Spec, Special case: No AR of the ARset was ever in ARstate == Primary – e.g. startup */
	}
	else if(
		cm_is_null(arset->primary)   /* no primary AR, enqueue in AR-set */
		|| argr != arset->primary /* alarm.req (async) on the way to CM, then primary AR changed (sync); enqueue in AR-set (AP01315412) */
		|| arset->r1b.wait_for_primary_trigger
	) {
		CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_send;
		LSA_UINT16 prio = cm_sv_ar_almi_alarm_prio(argr, param->alarm_type);
		LSA_UINT16 sequence_number = 0; /* sanity */

		if( cm_is_not_null(ar_sub) ) { /* always true (because not plug-group) */

			if( param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE ) {

				if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_ARP) != 0 ) { /* CM_SV_AR_SMFL_IS_PRMED but a.r.p. */

					ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_ARP; /* a return-of-submodule alarm resets the A.R.P. flag (AP01054520) */

					if( cm_is_not_null(do_led_info_ptr) ) {

						*do_led_info_ptr = LSA_TRUE;
					}
				}
			}
		}

		/***/

		param->u.sv.void_ptr = LSA_NULL; /* sanity */

		CM_RQB_SET_RESPONSE(rb, CM_OK_PENDING); /* sanity */

		/***/

		if(
			(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0
			&& cm_sv_r1b_curr_alarm_matches(arset, param, prio, &sequence_number)
		) {

			arset->alarm[prio].sequence_number = sequence_number;

			CmListInsertHead(&arset->alarm[prio].queue, (CM_LIST_ENTRY_PTR_TYPE)rb);
		}
		else {

			CmListInsertTail(&arset->alarm[prio].queue, (CM_LIST_ENTRY_PTR_TYPE)rb);
		}

		rc = CM_OK_PENDING; /* pending (as tail) in AR-set, cm_sv_opc_arset_trigger() follows */
	}
	else {

		rc = CM_OK; /* AR is primary, proceed */
	}

	return rc;
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
