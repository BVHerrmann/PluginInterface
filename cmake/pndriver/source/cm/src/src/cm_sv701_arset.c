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
/*  F i l e               &F: cm_sv701_arset.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, SRD things                                                    */
/*                                                                           */
/*  - ARSet                                                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7701
#define CM_MODULE_ID       7701

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_arset_ownership_rearrange(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL do_takeover
); /* forward */

/*===========================================================================*/
/*=== AR-SET TRIGGER ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_arset_trigger(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_ARSET_TRIGGER_PTR_TYPE param = rb->args.sv.arset_trigger;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_EXP_USER_REQ);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( cm_is_null(argr->cm.sv.arset) ) {

		rsp = CM_ERR_PARAM; /* a user-bug (not CM_AR_TYPE_SINGLE_SYSRED)*/
	}
	else if( cm_is_not_null(argr->cm.sv.curr_arset_trigger) ) {

		rsp = CM_ERR_SEQUENCE; /* a user-bug */
	}
	else if (param->is_primary != CM_SV_IS_PRIMARY_NO && param->is_primary != CM_SV_IS_PRIMARY_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "is_primary(%u) is out of range"
			, param->is_primary
		);

		rsp = CM_ERR_PARAM; /* a user-bug */
	}
	else {

		CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u/%u) arset_trigger is_primary(%u) is_first(%u)"
			, argr->device_nr, argr->ar_nr
			, param->is_primary
			, (arset->first == argr)
			);

		cm_sv_arset_check_primary(argr);

		/***/

		if( param->is_primary == CM_SV_IS_PRIMARY_YES ) { /* AR is the new primary AR */

			if( arset->primary != argr ) {

				rsp = CM_OK_CANCELLED; /* late (other AR is primary or this AR is down) */
			}
			else { /* primary AR exists (and is not down) */

				if( argr->cm.sv.r1_flags != 0 ) {

					cm_sv_r1b_pdev_alarm_resubmit(argr);
				}

				arset->r1b.wait_for_primary_trigger = LSA_FALSE;

				cm_sv_arset_alarm_restart(arset->primary); /* trigger: opcode AR-set-trigger */

				rsp = CM_OK;
			}
		}
		else { /* AR no longer is the new primary AR */

			if( arset->primary == argr ) {

				rsp = CM_ERR_SEQUENCE; /* a user-bug */
			}
			else {

				if( argr->cm.sv.r1_flags != 0 ) {

					if( arset->first == argr ) { /* trigger(backup) for first AR means primary AR is on the other side */

						arset->first = LSA_NULL; /* the first AR is now a backup AR */

						cm_sv_arset_ownership_rearrange(argr, LSA_FALSE); /* TIA 1899476 */
					}

					cm_sv_arset_alarm_cancel(argr, 0, 0, LSA_NULL, LSA_TRUE);
				}

				if( cm_ar_com_is_down(argr) ) {

					rsp = CM_OK_CANCELLED; /* late */
				}
				else if( cm_sv_ar_almi_is_idle(argr) ) {

					argr->cm.sv.sysred_IOCARSR_backup = LSA_FALSE; /* reset */

					rsp = CM_OK;
				}
				else {

					argr->cm.sv.curr_arset_trigger = rb;

					rsp = CM_OK_PENDING;
				}
			}
		}
	}

	/***/

	if( rsp != CM_OK_PENDING ) {

		cm_callback(channel, rsp, &rb);
	}
}

/*===========================================================================*/
/*=== AR-SET ABORT ==========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_arset_abort(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_ARSET_ABORT_PTR_TYPE param = rb->args.sv.arset_abort;
	CM_SV_DEVICE_PTR_TYPE dev;
	LSA_UINT16 rsp;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) is not added"
			, param->device_nr
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( param->arset_nr < 1 || param->arset_nr > CM_CFG_MAX_SV_ARSETS_PER_DEVICE ) {

		rsp = CM_ERR_PARAM; /* out of range */
	}
	else {

#if CM_CFG_MAX_SV_ARSETS_PER_DEVICE != 1
#error "check this!"
#endif

		CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);

		if( cm_is_not_null(arset->abort_rqb) ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) arset(%u) another request still running"
				, dev->device_nr, param->arset_nr
				);

			rsp = CM_ERR_SEQUENCE;
		}
		else { /* RDHT expired, abort all ARs of the AR-set */

			LSA_BOOL r1_repr = LSA_FALSE;

			{
			LSA_UINT16 idx;

			for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

				CM_AR_GRAPH_PTR_TYPE argr = arset->arr[idx];

				if( cm_is_not_null(argr) ) {

					if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

						r1_repr = LSA_TRUE; /* see below */
						continue;
					}

					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_RDHT, 0);
					cm_ar_com_down(argr, CM_AR_REASON_RDHT);
				}
			}
			}

			if( r1_repr && ! arset->is_aborting ) { /* TIA 2253099 */

				/* note:
				 * cm_sv_ar_com_down() does cm_sv_arset_check_primary()
				 * cm_sv_arset_check_primary() detects "all_down"
				 * "all_down" sets "is_aborting" and does cm_sv_arset_alarm_cancel()
				 *
				 * for S2 after the loop above "all_down" was detected and "is_aborting" is true.
				 * [because of e.g. MISS "all_down" may have happened before entering the loop. but now it is for sure.]
				 *
				 * R1 is different:
				 * only the original-AR (or bridge-broken) is allowed to abort the representative-AR
				 * thus we only set "is_aborting" here (so that the "is-late" logic works)
				 * and wait until the original-AR (or bridge-broken) removes the representative-AR
				 * - problem: because the flag is set, the "all_down" down logic is unreachable
				 * - solution: a little trick in cm_sv_ar_com_down(), see there.
				 */

				arset->is_aborting = LSA_TRUE;
			}

			if( arset->is_aborting ) {

				arset->abort_rqb = rb;

				rsp = CM_OK_PENDING;
			}
			else {

				rsp = CM_OK_CANCELLED; /* nothing to abort */
			}
		}
	}

	/***/

	if( rsp != CM_OK_PENDING ) {

		cm_callback(channel, rsp, &rb);
	}
}

/*===========================================================================*/
/*=== AR-SET OWNERSHIP SWAP =================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_arset_ownership_swap( /* called from cm_sv_arset_ownership_rearrange() */
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT16 front_idx,
	CM_AR_GRAPH_PTR_TYPE front_argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE front_sub,
	LSA_UINT16 back_idx,
	CM_AR_GRAPH_CONST_PTR_TYPE back_argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE back_sub
) {

	CM_SV_TRACE_05(front_argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "ownership-swap begin: subslot_nr(0x%x) front_argr(0x%x) back_argr(0x%x) su_owner(0x%x) front_told(0x%x)"
		, su->subslot_nr
		, front_argr->ar_nr, back_argr->ar_nr
		, su->owner_ar_nr, front_sub->sv.st_told
		);

	/* swap places */

	{
	CM_SV_CANDIDATE_TYPE tmp = su->cand[front_idx];
	su->cand[front_idx] = su->cand[back_idx];
	su->cand[back_idx] = tmp;
	}

	/* transfer ownership */

	if( su->owner_ar_nr == back_argr->ar_nr ) {

		CM_ASSERT(back_idx == 0 && su->cand[0].ar_nr == front_argr->ar_nr);

		su->owner_ar_nr = su->cand[0].ar_nr;

		if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

			CM_SV_DEVICE_PTR_TYPE dev = front_argr->cm.sv.dev;

			if( dev->pdev_owner_ar_nr == back_argr->ar_nr ) {

				dev->pdev_owner_ar_nr = su->owner_ar_nr;
			}
		}
	}

	if( su->owner_ar_nr == 0 && back_argr->cm.sv.r1_flags != 0 && front_argr->cm.sv.r1_flags != 0 ) { //R1 marker (pdev ownership)
		/* note: parallel AR establishment, one AR is "before appl-ready", happens before ownership.rsp */
		if( back_argr->cm.sv.dev->pdev_owner_ar_nr == back_argr->ar_nr ) {
			back_argr->cm.sv.dev->pdev_owner_ar_nr = front_argr->ar_nr;
		}
	}

	/* transfer submodule state
	 *
	 * note: "back" is the old front,
	 *       and the old front is no longer the owner
	 */

	{
	LSA_UINT16 bk_flags = back_sub->sv.sm_flags; /* shortcut */
	LSA_UINT16 fr_flags = front_sub->sv.sm_flags; /* keep AR-specific flags (AP01507502) */

	fr_flags &= ~CM_SV_AR_SMFL_TNA;
	fr_flags &= ~CM_SV_AR_SMFL_DIRTY;
	fr_flags &= ~CM_SV_AR_SMFL_OSHP;
	fr_flags &= ~CM_SV_AR_SMFL_WRONG;
	fr_flags &= ~CM_SV_AR_SMFL_IS_PRMED;
	fr_flags &= ~CM_SV_AR_SMFL_IS_ARP;
	fr_flags &= ~CM_SV_AR_SMFL_EX_NEWDTA;
	fr_flags &= ~CM_SV_AR_SMFL_R1_PP;

	if( (bk_flags & CM_SV_AR_SMFL_TNA) != 0 ) {

		fr_flags |= CM_SV_AR_SMFL_TNA;
	}

	if( (bk_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) {

		fr_flags |= CM_SV_AR_SMFL_DIRTY;
		fr_flags |= CM_SV_AR_SMFL_OSHP;
		fr_flags |= CM_SV_AR_SMFL_WRONG;
	}

	if( (bk_flags & CM_SV_AR_SMFL_WRONG) != 0 ) {

		fr_flags |= CM_SV_AR_SMFL_WRONG;
	}

	if( (bk_flags & CM_SV_AR_SMFL_EX_APLRDY) != 0 ) {

		/* don't cancel, user has to signal appl-ready (most likely the RQB is already under way) */
	}

	if( (bk_flags & CM_SV_AR_SMFL_IS_PRMED) != 0 ) {

		fr_flags |= CM_SV_AR_SMFL_IS_PRMED;

		if( (bk_flags & CM_SV_AR_SMFL_IS_ARP) != 0 ) {

			fr_flags |= CM_SV_AR_SMFL_IS_ARP;
		}

		if( (bk_flags & CM_SV_AR_SMFL_EX_NEWDTA) != 0 ) {

			/* the new primary AR has to reparameterize the submodule anyway */
		}
	}

	back_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_EX_NEWDTA; /* cancel new-data.ind in "back" */
	back_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_R1_PP;     /* alarm sequence interrupted by primary change */

	front_sub->sv.st_told  = back_sub->sv.st_told;
	front_sub->sv.sm_flags = fr_flags;

	if( (front_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) {

		front_argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_PULL_PLUG;
	}
	}

	CM_SV_TRACE_05(front_argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "ownership-swap end: subslot_nr(0x%x) front_argr(0x%x) back_argr(0x%x) su_owner(0x%x) front_told(0x%x)"
		, su->subslot_nr
		, front_argr->ar_nr, back_argr->ar_nr
		, su->owner_ar_nr, front_sub->sv.st_told
		);
}

/*===========================================================================*/
/*=== AR-SET OWNERSHIP REARRANGE ============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_arset_ownership_rearrange(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL do_takeover /* LSA_TRUE if primary AR comes, LSA_FALSE if AR runs down */
) {

	/* rearrange the candidate queue of all submodules */

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev/ar(%u/%u) arset(%u) ownership_rearrange do_takeover(%u)"
		, argr->device_nr, argr->ar_nr, argr->cm.sv.arset->arset_nr, do_takeover
		);

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				LSA_UINT16 argr_idx  = CM_CFG_MAX_SV_ARS;
				LSA_UINT16 other_idx = CM_CFG_MAX_SV_ARS;

				CM_AR_GRAPH_PTR_TYPE other_argr = LSA_NULL;

				{
				LSA_UINT16 cand_idx;

				for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

					if( su->cand[cand_idx].ar_nr == 0 ) {

						break; /* tail */
					}
					else {

						CM_AR_GRAPH_PTR_TYPE cand_argr = cm_sv_ar_lookup2(argr->cm.sv.dev, su->cand[cand_idx].ar_nr);

						if( cm_is_not_null(cand_argr) ) { /* always true */

							if( cand_argr == argr ) {

								argr_idx = cand_idx;
							}
							else { /* another AR */

								if( cand_argr->cm.sv.arset == argr->cm.sv.arset ) { /* same AR-set */

									if( other_idx == CM_CFG_MAX_SV_ARS ) {

										if( do_takeover ) { /* note: this AR is not down */

											other_idx = cand_idx;

											other_argr = cand_argr;
										}
										else {

											if( (cand_argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) != 0 ) {

												/* skip (an AR that is down must not become owner) */
											}
											else {

												other_idx = cand_idx;

												other_argr = cand_argr;
											}
										}
									}
								}
							}
						}
					}
				}
				}

				if( other_idx >= CM_CFG_MAX_SV_ARS ) {

					/* no other AR of the same AR-set found */
				}
				else if( cm_is_null(other_argr) || argr_idx >= CM_CFG_MAX_SV_ARS ) {

					CM_FATAL(); /* a bug */
				}
				else {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE other_sub;

					cm_ar_graph_find_api_slot_subslot(other_argr, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr, LSA_NULL, LSA_NULL, &other_sub);

					if( cm_is_null(other_sub) ) {

						CM_FATAL();
					}
					else {

						if( do_takeover ) {

							if( argr_idx > other_idx ) { /* move "argr" to the front */

								cm_sv_arset_ownership_swap(su, argr_idx, argr, ar_sub, other_idx, other_argr, other_sub);
							}
						}
						else { /* no takeover (AR is running down) */

							if( argr_idx < other_idx ) { /* move "other" to front (i.e., move "argr" to the back) */

								cm_sv_arset_ownership_swap(su, other_idx, other_argr, other_sub, argr_idx, argr, ar_sub);
							}
						}
					}
				}
			}
		}
	}
	}
}

/*===========================================================================*/
/*=== AR-SET CHECK PRIMARY ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_arset_check_primary(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
	LSA_BOOL did_alarm_stop = LSA_FALSE;
	CM_AR_GRAPH_PTR_TYPE prim;

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) != 0 ) {

		/* note: may come here more than once - but once from cm_sv_ar_com_down() */

		if( argr == arset->first ) {

			arset->first = LSA_NULL;
		}

		/***/

		if( arset->primary == argr ) { /* TIA 1488371 */

			cm_sv_arset_alarm_stop(arset->primary);

			did_alarm_stop = LSA_TRUE;
		}

		cm_sv_arset_ownership_rearrange(argr, LSA_FALSE);

		/***/

		if( ! arset->is_aborting ) {

			LSA_BOOL all_down = LSA_TRUE;

			{
			LSA_UINT16 idx;

			for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

				CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[idx];

				if( cm_is_not_null(temp_argr) ) {

					if( (temp_argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) == 0 ) { /* not down */

						all_down = LSA_FALSE;
					}
				}
			}
			}

			if( all_down ) {

				arset->is_aborting = LSA_TRUE;

				cm_sv_arset_alarm_cancel(argr, 0, 0, LSA_NULL, LSA_FALSE);

				/***/

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev(%u) arset(%u) all ARs are down"
					, argr->device_nr, arset->arset_nr
					);
			}
		}
	}

	/***/

	{
	LSA_UINT16 session_key = CM_GET_PRIMARY_AR(arset->arset_nr, argr->ar_com.channel->sysptr);

	if( session_key == 0 ) {

		prim = LSA_NULL; /* no primary AR */
	}
	else { /* search the AR-set */

		prim = LSA_NULL;

		{
		LSA_UINT16 idx;

		for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

			CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[idx];

			if( cm_is_not_null(temp_argr) && temp_argr->session_key == session_key ) {

				prim = temp_argr; /* found */
				break;
			}
		}
		}

		if( cm_is_null(prim) ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) arset(%u) session_key(0x%x) not found"
				, argr->device_nr, arset->arset_nr
				, session_key
				);

			CM_FATAL(); /* not found (a user-bug) */
		}
		else {

			if( (prim->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

				CM_FATAL(); /* representative-AR is not allowed (a user-bug) */
			}
		}
	}
	}

	/***/

	if( cm_is_not_null(prim) ) {

		if( (prim->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) != 0 ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) arset(%u) prim(0x%x) is down, ignored (user did not see disconnect yet)"
				, argr->device_nr, arset->arset_nr
				, prim->session_key
				);

			prim = LSA_NULL; /* user did not see disconnect yet */
		}
		else if( prim->cm.sv.sysred_IOCARSR_backup ) { /* TIA 209760 */

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) arset(%u) prim(0x%x) is AlarmAck(IOCARSR backup)"
				, argr->device_nr, arset->arset_nr
				, prim->session_key
				);

			prim = LSA_NULL; /* is AlarmAck(IOCARSR backup), alarm-ack overtook the backup-request in the O-CR */
		}
		else {

			/* ok */
		}
	}

	/***/

	if( argr->cm.sv.r1_flags != 0 ) {

		if( cm_is_null(prim) ) {

			arset->r1b.wait_for_primary_trigger = LSA_TRUE; /* no primary-AR thus waiting for primary-AR */
		}
	}

	/***/

	if( cm_is_null(arset->primary) ) { /* primary AR did not exist */

		if( cm_is_null(prim) ) {

			/* do nothing (no change) */
		}
		else {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) arset(%u) prim(0x%x) comes"
				, argr->device_nr, arset->arset_nr
				, prim->session_key
				);

			arset->first = LSA_NULL; /* the primary AR replaces the first AR */

			arset->primary = prim; /* primary AR comes */

			cm_sv_arset_ownership_rearrange(prim, LSA_TRUE);

			/* note: cm_sv_arset_alarm_restart() see cm_sv_opc_arset_trigger() */
		}
	}
	else { /* primary AR existed */

		if( cm_is_null(prim) ) {

			CM_AR_GRAPH_PTR_TYPE old = arset->primary;

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) arset(%u) prim(0x%x) gone"
				, argr->device_nr, arset->arset_nr
				, arset->primary->session_key
				);

			if( ! did_alarm_stop ) { /* see above */

				cm_sv_arset_alarm_stop(arset->primary);
			}

			arset->primary = LSA_NULL; /* primary AR gone */

			if( old->cm.sv.r1_flags != 0 ) {

				cm_sv_arset_ownership_rearrange(old, LSA_FALSE);
			}
		}
		else if( arset->primary != prim ) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) arset(%u) prim(0x%x) changed, old(0x%x)"
				, argr->device_nr, arset->arset_nr
				, prim->session_key, arset->primary->session_key
				);

			cm_sv_arset_alarm_stop(arset->primary);

			arset->primary = prim; /* primary AR changed */

			cm_sv_arset_ownership_rearrange(prim, LSA_TRUE);

			/* note: cm_sv_arset_alarm_restart() see cm_sv_opc_arset_trigger() */
		}
		else {

			/* do nothing (no change) */
		}
	}
}

/*===========================================================================*/
/*=== AR-SET IS PRIMARY OR FIRST ============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_arset_is_primary_or_first(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( argr == argr->cm.sv.arset->primary ) {

		return LSA_TRUE;
	}

	if( argr == argr->cm.sv.arset->first ) {

		return LSA_TRUE;
	}

	return LSA_FALSE; /* neither primary AR nor "first" */
}

/*===========================================================================*/
/*=== AR-SET IS FIRST =======================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_arset_is_first(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( argr == argr->cm.sv.arset->first ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== AR-SET GET PRIMARY OR FIRST ===========================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_arset_get_primary_or_first(
	CM_SV_ARSET_CONST_PTR_TYPE arset
) {

	if( cm_is_not_null(arset->primary) ) {

		return arset->primary;
	}

	return arset->first; /* note: may be LSA_NULL */
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
