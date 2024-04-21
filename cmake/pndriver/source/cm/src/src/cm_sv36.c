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
/*  F i l e               &F: cm_sv36.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Ownership things                                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7360
#define CM_MODULE_ID       7360

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== AR PLUG-GROUP ALARM SEND ==============================================*/
/*===========================================================================*/

static CM_UPPER_ALARM_PTR_TYPE
cm_sv_ar_plug_group_alarm_send(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_AP_CONST_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_UINT16 alarm_type
) {
	CM_UPPER_RQB_PTR_TYPE alarm;
	CM_UPPER_ALARM_PTR_TYPE param;

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) plug-group alarm_type(%u)"
		, argr->device_nr, argr->ar_nr, alarm_type
		);

	/***/

	if( alarm_type == CM_ALARM_TYPE_PULL_MODULE ) {

		ar_sub = LSA_NULL;

		if( (argr->ar.ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0 ) {

			alarm_type = CM_ALARM_TYPE_PULL; /* PULL and subslot_nr == 0 means PULL_MODULE */
		}
	}

	/***/

	if( cm_is_null(argr->cm.sv.plug_group_alarm_rqb) ) {
		CM_FATAL(); /* a bug */
	}

	alarm = argr->cm.sv.plug_group_alarm_rqb;
	argr->cm.sv.plug_group_alarm_rqb = LSA_NULL;

	/***/

	CM_RQB_SET_USER_ID_PTR(alarm, LSA_NULL);
	CM_RQB_SET_HANDLE(alarm, CM_INVALID_HANDLE);
	CM_RQB_SET_OPCODE(alarm, CM_OPC_SV_AR_ALARM_PLUG_GROUP);

	param = alarm->args.sv.ar_alarm_send;

	if( cm_is_null(param) ) {
		CM_FATAL(); /* a bug */
	}

	param->device_nr   = argr->device_nr;
	param->session_key = argr->session_key;
	param->ar_nr       = CM_SV_SESSION_KEY_TO_AR_NR(param->session_key); /* as documented */

	/*
	param->alarm_priority = ...
	*/

	param->alarm_type = alarm_type;

	param->api        = ar_ap->api;
	param->slot_nr    = ar_mod->slot_nr;

	if( cm_is_not_null(ar_sub) ) {
		param->subslot_nr = ar_sub->subslot_nr;
	}
	else {
		param->subslot_nr = 0; /* the module */
	}

	/*
	param->alarm_sequence = ...
	param->diag_channel_available = ...
	param->diag_generic_available = ...
	param->diag_submod_available = ...
	param->reserved = ...
	param->ar_diagnosis_state = ...
	param->mod_ident = ...
	param->sub_ident = ...
	*/

	param->alarm_tag         = 0;
	param->alarm_data_length = 0;
	param->alarm_data        = LSA_NULL;

	/*
	param->u.sv.void_ptr = ...
	*/

	/***/

	if( cm_sv_ar_almi_alarm_request(argr, alarm, ar_ap, ar_mod, ar_sub, LSA_NULL) != CM_OK ) {

		CM_FATAL(); /* a bug */
	}

	/***/

	return param; /* see caller */
}

/*===========================================================================*/
/*=== AR SUBMODULE STATE RESET ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_submodule_state_reset(
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL is_dirty,
	LSA_BOOL is_wrong
) {
	LSA_UINT16 tmp_flags = ar_sub->sv.sm_flags; /* save, see below */

	if( is_dirty ) {

		ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_DIRTY;  /* needs action */
		ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_OSHP;   /* do an ownership.ind if plugged */

		ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_R1_PP; /* alarm sequence interrupted -> reset flag */
	}
	else { /* clean (ar-ownership.rsp in connect-context) */

		ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
		ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_OSHP;
	}

	/***/

	if( is_wrong || is_dirty ) {

		ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WRONG;
	}
	else {

		ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_WRONG;
	}

	/***/

	ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_EX_APLRDY; /* cancel, see below */
	ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_PRMED;  /* reset */
	ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_ARP;    /* reset */
	ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_EX_NEWDTA; /* cancel */

	/***/

	if( (tmp_flags & CM_SV_AR_SMFL_EX_APLRDY) != 0 ) {

		return LSA_TRUE; /* cancelled, see argr->cm.sv.pr_appl_ready_cnt */
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== AR SUBMODULE STATE CHANGED ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_submodule_state_changed(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

	cm_ar_graph_find_api_slot_subslot(argr, api, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

	if( cm_is_not_null(ar_sub) ) { /* always true */

		LSA_BOOL aplrdy_cancelled = cm_sv_ar_submodule_state_reset(ar_sub, LSA_TRUE/*dirty*/, LSA_TRUE/*wrong*/);

		cm_sv_ar_almi_cancel(argr, api, slot_nr, ar_sub);

		/***/

		if( aplrdy_cancelled ) {

			cm_sv_ar_appl_ready_cnt_decrement(argr);

			cm_sv_rpc_appl_ready_request(argr); /* trigger: cancellation of CM_SV_AR_SMFL_EX_APLRDY */
		}

		/***/

		CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-FLG: marked as dirty/wrong/not-prmed"
			, argr->device_nr, argr->ar_nr
			, api, slot_nr, subslot_nr
			);

		/***/

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_PULL_PLUG;
	}
}

/*===========================================================================*/
/*=== AR SUBMODULE STATE ACTION =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_submodule_state_action(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DO_PULL_PLUG) == 0 ) {

		return; /* idle */
	}

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

		return; /* early */
	}

	if( cm_ar_com_is_down(argr) ) {

		return; /* late */
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

			return; /* wait */
		}

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_DELAY) != 0 ) {

			return; /* wait */
		}

		if(
			(argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PRM_BEGIN) != 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE) != 0
		) {

			/* TIA 2197506
			*- When R1-Bridge is established remote submodules are plugged.
			*- AR-Primary triggers PLUG-alarms for the remote submodules.
			*- PD_BEGIN_REMOTE is triggered (R1-Bridge is slow!).
			*- Controller does PrmBegin.
			*- Ownership.IND/RSP leads to another PD_BEGIN_REMOTE.
			*- Must wait because there is only one resource for PD_BEGIN_REMOTE.
			*/

			return; /* wait */
		}

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_DO_OSHP_IND) != 0 ) {

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PBE_DO_OSHP_IND;

			if( cm_is_not_null(argr->cm.sv.arset) ) { /* TIA 2329293 */

				/* a buggy IOC did PrimaryReq but startet PrmBegin without waiting for PrimaryAck from the device */
				/* "backup" on PrmBegin -> no ownership.ind and no pd-prm-begin */
				/* "primary" on PrmEnd -> pd-prm-end complained because of missing pd-prm-begin */

				cm_sv_arset_check_primary(argr);

				if( argr != argr->cm.sv.arset->primary ) {

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_PRM_BACKUP;
				}
			}

			{
			CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_ownership_build(argr, CM_SV_AR_CONTEXT_PLUG);

			if( cm_is_not_null(event) ) {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

				cm_callback(argr->ar_com.channel, CM_OK, &event);

				return; /* wait */
			}
			}
		}

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_DO_CTRL_RSP) != 0 ) { /* see cm_sv_ar_com_down() too */

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PBE_DO_CTRL_RSP;

			/***/

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != 0 ) {

				CM_FATAL(); /* a bug */
			}
			else {

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_EXP_PRM_END; /* note: compare to plug-alarm */

				/* note: CM_SV_AR_FL_PRM_BACKUP must be set before cm_sv_ar_ownership_build() */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) != 0 ) {

					if( cm_sv_ar_is_pdev_owner(argr) ) { /* may be false */

						/* note: the flag CM_SV_AR_FL_PDEV_INVALID is reset in cm_sv_pd_prm_end_confirmation() */

						if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL) != 0 ) {

							cm_sv_pd_prm_begin(argr, argr->cm.sv.local_pdev_if->mod->slot_nr, argr->cm.sv.local_pdev_if->subslot_nr);
						}
						else if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE) != 0 ) {

							if(
								argr->cm.sv.r1_flags != 0
								&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
							) {

								cm_sv_pd_prm_begin_remote(argr, argr->cm.sv.remote_pdev_if->mod->slot_nr, argr->cm.sv.remote_pdev_if->subslot_nr);
							}
						}
						else { /* PrmUpdate(ALL) */

							cm_sv_pd_prm_begin(argr, 0, 0); /* PrmUpdate(ALL) */

							if(
								argr->cm.sv.r1_flags != 0
								&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
							) {

								cm_sv_pd_prm_begin_remote(argr, 0, 0); /* PrmUpdate(ALL) */
							}
						}
					}
				}

				if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

					CM_FATAL();
				}
				else {

					CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
					argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

					cm_sv_rpc_control_response_prm_begin(argr, rpc);
				}
			}
		}

		return; /* prm-begin-end has precedence */
	}

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

		return; /* busy */
	}

	if( cm_is_not_null(argr->cm.sv.plug_group_alarm) ) {

		return; /* busy */
	}

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		cm_sv_arset_check_primary(argr);

		if( argr != argr->cm.sv.arset->primary ) {

			return; /* no alarms if not primary (kind of late) */
		}

		if( argr->cm.sv.r1_flags != 0 ) { //R1 VV: beautify this scenario by means of a device-flag

			/* case: plug-remote in state broken -> control-established comes soon */

			if( argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE || argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE ) {

				return; /* wait for update-end */
			}
		}
	}

	/***/

	{
	CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_ownership_build(argr, CM_SV_AR_CONTEXT_PULLPDEV);

	if( cm_is_not_null(event) ) {

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

		cm_callback(argr->ar_com.channel, CM_OK, &event);

		return;
	}
	}

	{
	CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_ownership_build(argr, CM_SV_AR_CONTEXT_PLUG);

	if( cm_is_not_null(event) ) {

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

		cm_callback(argr->ar_com.channel, CM_OK, &event);

		/* not: return -- keep on pulling */
	}
	}

	/***/

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) { /* needs action */

					CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;
					LSA_UINT16 alarm_type;

					/* see PNIO-Spec, Part 5, state machines RSMSM and OWNSM */
					/* principle: one kind of "unavailable" is not replaced by another kind of "unavailable" */

					if( argr->ar_nr != su->owner_ar_nr ) {

						if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

							continue; /* su->owner_ar_nr not yet determined (V6.1i1.3x, TIA 2002617) */
						}

						/***/

						if( CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {

							alarm_type = CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR; /* even if told pulled or wrong, see OWNSM */
						}
						else {

							alarm_type = 0; /* no alarm, still unavailable */
						}
					}
					else if( CM_SV_SUBSLOT_IS_PLUGGED(su) ) { /* owned and plugged */

						if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

							continue; /* see above: ident-check pending, but can keep on pulling */
						}

						/***/

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_WRONG) != 0 ) {

							if( CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {

								alarm_type = CM_ALARM_TYPE_PLUG_WRONG;
							}
							else {

								alarm_type = 0; /* no alarm, still unavailable */
							}
						}
						else { /* ok or substitute */

							if( CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {

								alarm_type = CM_ALARM_TYPE_PLUG;
							}
							else {

								alarm_type = CM_ALARM_TYPE_RELEASED;
							}

							/***/

							if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, ar_sub->subslot_nr) ) { //R1 marker

								if( argr->cm.sv.dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED ) {

									/* case: plug-remote in state broken -> control-established comes soon */

									continue; /* see cm_sv_r1b_update_end() */
								}

								/***/

								if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REMOTE_RTF) != 0 ) {

									alarm_type = CM_ALARM_TYPE_RELEASED; /* TIA 1240352 */
								}
							}
						}
					}
					else { /* owned and pulled */

						if( CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {

							if( CM_SV_SLOT_IS_PLUGGED(ar_mod->sv.real_sl) ) {

								alarm_type = CM_ALARM_TYPE_PULL;
							}
							else { /* module is pulled */

								alarm_type = CM_ALARM_TYPE_PULL_MODULE;
							}
						}
						else {

							alarm_type = 0; /* no alarm, still unavailable */
						}
					}

					/***/

					CM_SV_TRACE_10(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
						, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-ACT: is-owner(%u) plugged(%u) wrong(%u) told-owned(%u) -> alarm_type(%u)"
						, argr->device_nr, argr->ar_nr
						, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
						, (argr->ar_nr == su->owner_ar_nr)
						, CM_SV_SUBSLOT_IS_PLUGGED(su)
						, ((ar_sub->sv.sm_flags & CM_SV_AR_SMFL_WRONG) != 0)
						, CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told)
						, alarm_type
						);

					/***/

					if( alarm_type != 0 ) {

						LSA_UINT16 sub_state;

						if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, ar_sub->subslot_nr) ) {

							if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) { /* if no COC then abort the AR */

								cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_COC, 0);
								cm_ar_com_down(argr, CM_AR_REASON_COC);

								return; /* stop */
							}
						}

						if( alarm_type == CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR ) {

							sub_state = ((ar_sub->sv.st_told & ~CM_SUB_STATE_AR_INFO_MASK) | CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR);
						}
						else if( alarm_type == CM_ALARM_TYPE_PLUG || alarm_type == CM_ALARM_TYPE_RELEASED ) {

							sub_state = CM_SUB_STATE_GOOD; /* note: "subst" is transferred in the ModuleDiffBlock */
						}
						else if( alarm_type == CM_ALARM_TYPE_PLUG_WRONG ) {

							sub_state = CM_SUB_STATE_WRONG;
						}
						else {

							sub_state = CM_SUB_STATE_PULLED;
						}

						/***/

						/*
						 * history: SR-AR and "st_told"
						 *
						 * in case of an SR-AR an alarm is completed only if acked on the primary-AR
						 * the task TIA 1493405 suggested to set "st_told" later on alarm-ack.ind (and only if still primary-AR).
						 * the intention was to simplifiy things. however, it turned out that this was not the case.
						 * - cm_sv_arset_alarm_stop_plug_group() was still necessary in order to repeat the prm-sequence on the new primary AR.
						 * - pull module was still a special case.
						 */

						if( alarm_type == CM_ALARM_TYPE_PULL_MODULE ) {

							for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

								ar_sub->sv.st_told = sub_state;

								ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
							}
						}
						else {

							ar_sub->sv.st_told = sub_state;

							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
						}

						/***/

						argr->cm.sv.plug_group_alarm = cm_sv_ar_plug_group_alarm_send(argr, ar_ap, ar_mod, ar_sub, alarm_type);

						return; /* pending */
					}
					else {

						ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
					}
				}
			}
		}
	}
	}

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

		return; /* see above: ident-check pending, but can keep on pulling */
	}

	if( argr->cm.sv.r1_flags != 0 ) { //R1 marker (RTF)

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_REMOTE_RTF;
	}

	argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_DO_PULL_PLUG;
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
