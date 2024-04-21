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
/*  F i l e               &F: cm_sv37.c                                 :F&  */
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

#define LTRC_ACT_MODUL_ID  7370
#define CM_MODULE_ID       7370

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== ATTACH EXPECTED-CFG TO REAL-CFG =======================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_attach(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	/*
	 * attach "expected config" to "real config"
	 * and vivify the "real config" if not yet plugged
	 */

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_SV_AP_PTR_TYPE ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		ap = cm_sv_ap_attach(dev, ar_ap->api, LSA_FALSE);

		if( cm_is_null(ap) ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot cm_sv_ap_attach"
				);

			return LSA_FALSE;
		}

		ar_ap->sv.real_ap = ap; /* attached */

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_SV_SLOT_PTR_TYPE sl;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			sl = cm_sv_slot_attach(dev, ap, ar_mod->slot_nr, LSA_FALSE);

			if( cm_is_null(sl) ) {
				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cannot cm_sv_slot_attach"
					);

				return LSA_FALSE;
			}

			ar_mod->sv.real_sl = sl; /* attached */

			/***/

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su;

				su = cm_sv_subslot_attach(dev, sl, ar_sub->subslot_nr, LSA_FALSE);

				if( cm_is_null(su) ) {
					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "cannot cm_sv_subslot_attach"
						);

					return LSA_FALSE;
				}

				ar_sub->sv.real_su = su; /* attached */

				/*
				 * note: su->ar_refcnt == 0 .NE. ownership-queue empty
				 *       because of sequence relinquish and then detach
				 */
			}
		}
	}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== DETACH EXPECTED-CFG FROM REAL-CFG =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_detach(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	/*
	 * detach the "expected config" from "real config"
	 */

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_SV_AP_PTR_TYPE ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		ap = ar_ap->sv.real_ap; /* TIA 1159626 */

		ar_ap->sv.real_ap = LSA_NULL; /* sanity */

		if( cm_is_null(ap) ) {

			continue;
		}

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_SV_SLOT_PTR_TYPE sl;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			sl = ar_mod->sv.real_sl; /* TIA 1159626 */

			ar_mod->sv.real_sl = LSA_NULL; /* sanity */

			if( cm_is_null(sl) ) {

				continue;
			}

			/***/

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su;

				su = ar_sub->sv.real_su; /* TIA 1159626 */

				ar_sub->sv.real_su = LSA_NULL; /* sanity */

				if( cm_is_null(su) ) {

					continue;
				}

				/***/

				{
				LSA_UINT16 cand_idx;

				for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

					if( su->cand[cand_idx].ar_nr == 0 ) {
						break; /* last */
					}

					/* must not be found, see cm_sv_subslot_ownership_relinquish() */

					CM_ASSERT(su->cand[cand_idx].ar_nr != argr->ar_nr);
				}
				}

				/***/

				cm_sv_subslot_detach(su, LSA_FALSE);
			}

			/***/

			cm_sv_slot_detach(sl, LSA_FALSE);
		}

		/***/

		cm_sv_ap_detach(ap, LSA_FALSE);
	}
	}
}

/*===========================================================================*/
/*=== QUEUE FOR OWNERSHIP (SUBSLOT) =========================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_subslot_ownership_queue_for( /* called from cm_sv_ownership_queue_for() */
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT16 ar_nr,
	LSA_BOOL is_supervisor,
	LSA_BOOL no_takeover
) {
	LSA_UINT16 cand_idx;
	LSA_BOOL add_info;

	CM_SV_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
		, "subslot(%u) ar_nr(%u) is_supervisor(%u) no_takeover(%u)"
		, su->subslot_nr, ar_nr
		, is_supervisor, no_takeover
		);

	for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

		if( su->cand[cand_idx].ar_nr == 0 ) {
			break; /* this is the tail */
		}

		if( su->cand[cand_idx].ar_nr == ar_nr ) {
			CM_FATAL(); /* no duplicates */
		}
	}

	if( cand_idx >= CM_CFG_MAX_SV_ARS ) {
		CM_FATAL(); /* a bug in cm_sv_ar_alloc() */
	}

	/* insert-tail */

	su->cand[cand_idx].ar_nr         = ar_nr; /* used */
	su->cand[cand_idx].no_takeover   = no_takeover;
	su->cand[cand_idx].is_supervisor = is_supervisor;

	/*
	 * IOC: queues at the tail
	 * IOS: jumps the queue as far as possible
	 */

	add_info = LSA_FALSE;

	if( is_supervisor ) {

		for( ; cand_idx > 0; --cand_idx ) { /* while "cand_idx" has a predecessor do swap ... */

			LSA_UINT16 pred_idx = cand_idx - 1;

			if( su->cand[pred_idx].is_supervisor ) {

				add_info = LSA_TRUE; /* takeover not allowed */

				break; /* stop: predecessor is another IOS */
			}
			else if( pred_idx == 0 ) {

				if( su->cand[pred_idx].no_takeover ) {

					add_info = LSA_TRUE; /* takeover not allowed */
				}

				break; /* stop: predecessor is the queue-head */
			}
			else { /* swap */

				CM_SV_CANDIDATE_TYPE tmp = su->cand[pred_idx];
				su->cand[pred_idx] = su->cand[cand_idx];
				su->cand[cand_idx] = tmp;
			}
		}
	}

	return add_info;
}

/*===========================================================================*/
/*=== QUEUE FOR OWNERSHIP (AR) ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ownership_queue_for(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 pdev_first = 0;
	LSA_BOOL is_supervisor;
	LSA_BOOL no_takeover;

	/*
	 * note: called only if cm_sv_attach() succeeded
	 */

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_QF_OWNERSHIP) != 0 ) {
		CM_FATAL();
	}

	argr->cm.sv.ar_flags |= CM_SV_AR_FL_QF_OWNERSHIP;

	/***/

	if( argr->ar.ar_type == CM_AR_TYPE_SUPERVISOR ) {
		is_supervisor = LSA_TRUE;
	}
	else {
		is_supervisor = LSA_FALSE;
	}

	if( is_supervisor ) {
		no_takeover = LSA_TRUE; /* IOS implies "takeover not allowed" */
	}
	else if( (argr->ar.ar_properties & CM_AR_PROP_SUPERVISOR_TAKEOVER_ALLOWED) != 0 ) {
		no_takeover = LSA_FALSE;
	}
	else {
		no_takeover = LSA_TRUE;
	}

	/***/

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) is_supervisor(%u) no_takeover(%u)"
		, argr->device_nr, argr->ar_nr
		, is_supervisor, no_takeover
		);

	/***/

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) is_supervisor(%u) no_takeover(%u)"
					, argr->device_nr, argr->ar_nr
					, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
					, is_supervisor, no_takeover
					);

				if( cm_sv_subslot_ownership_queue_for(ar_sub->sv.real_su, argr->ar_nr, is_supervisor, no_takeover) ) {

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_TNA; /* takeover not allowed (note: this flag is never reset!) */
				}

				/***/

				if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, ar_sub->subslot_nr) ) {

					/* note: only checking "local" is ok too for R1-ARs since an R1-AR contains both PDEVs. */

					if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr) ) {

						/* note: a PDEV-AR always contains the interface, see cm_sv_argr_build() */

						if( pdev_first == 0 ) {

							pdev_first = ar_sub->sv.real_su->cand[0].ar_nr;
						}
					}
				}
			}
		}
	}
	}

	/***/

	if( pdev_first == argr->ar_nr ) {

		CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

		if( dev->pdev_owner_ar_nr != 0 ) {

			CM_FATAL();
		}
		else if( dev->central_pdev ) {

			/* do nothing */
		}
		else {

			dev->pdev_owner_ar_nr = argr->ar_nr; /* set the pdev-owner */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev/ar(%u/%u) is the pdev-owner"
				, argr->device_nr, argr->ar_nr
				);
		}
	}
}

/*===========================================================================*/
/*=== RELINQUISH OWNERSHIP (SUBSLOT) ========================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_subslot_ownership_relinquish( /* called from cm_sv_ownership_relinquish() */
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT16 ar_nr
) {
	LSA_UINT16 cand_idx;
	LSA_BOOL is_head_leaving;

	CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
		, "ar(%u) subslot(%u)"
		, ar_nr, su->subslot_nr
		);

	if( ar_nr == 0 ) {
		CM_FATAL(); /* a bug */
	}

	for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

		if( su->cand[cand_idx].ar_nr == ar_nr ) {
			break; /* found */
		}
	}

	if( cand_idx >= CM_CFG_MAX_SV_ARS ) {
		CM_FATAL(); /* a bug, must be found */
	}

	/***/

	if( su->owner_ar_nr == su->cand[cand_idx].ar_nr ) { /* owner is leaving */

		if( cand_idx != 0 ) {
			CM_FATAL(); /* a bug, must be the queue-head */
		}

		su->owner_ar_nr = 0;
	}

	if( cand_idx == 0 ) { /* head-of-queue is leaving (AP00927492 updates AP00826555) */

		is_head_leaving = LSA_TRUE;
	}
	else {

		is_head_leaving = LSA_FALSE;
	}

	/* shift down the rest */

	for( ; cand_idx < CM_CFG_MAX_SV_ARS - 1; ++cand_idx ) {

		su->cand[cand_idx] = su->cand[cand_idx + 1]; /* copy the whole struct! */

		if( su->cand[cand_idx].ar_nr == 0 ) {
			break;
		}
	}

	su->cand[CM_CFG_MAX_SV_ARS - 1].ar_nr = 0;

	/***/

	return is_head_leaving;
}

/*===========================================================================*/
/*=== RELINQUISH OWNERSHIP (AR) =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ownership_relinquish(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	LSA_BOOL abort_pdev_ARs = LSA_FALSE;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u)"
		, argr->device_nr, argr->ar_nr
		);

	/***/

	if( cm_sv_ar_is_pdev_owner(argr) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u/%u) pdev-owner is gone"
			, argr->device_nr, argr->ar_nr
			);

		dev->pdev_owner_ar_nr = 0; /* reset the pdev-owner */
	}

	/***/

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-REL: owner(%u)"
					, argr->device_nr, argr->ar_nr
					, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
					, su->owner_ar_nr
					);

				if( cm_sv_subslot_ownership_relinquish(su, argr->ar_nr) ) { /* head-of-queue is leaving */

					if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

						/* must abort other PDEV-ARs if the first PDEV-candidate (rather than the PDEV-owner) leaves
						 *
						 * AR1 queues for ownership and becomes the PDEV-owner.
						 * AR2 queues for ownership and sees a PO-problem.
						 * AR1 relinquishes ownership and signals "down" to AR2.
						 * Until AR2 relinquishes ownership it takes a while.
						 * In the meantime AR3 queues for ownership and sees a PO-problem because of AR2.
						 *
						 * When AR2 relinquishes ownership, AR3 must be aborted too.
						 * It is not sufficient to abort the PDEV-ARs only when the PDEV-owner leaves.
						 */

						abort_pdev_ARs = LSA_TRUE;
					}
					else {

						CM_AR_GRAPH_PTR_TYPE cand_argr = (su->cand[0].ar_nr != 0 ? cm_sv_ar_lookup2(dev, su->cand[0].ar_nr) : LSA_NULL);

						if( cm_is_not_null(cand_argr) ) {

							cm_sv_ar_submodule_state_changed(cand_argr, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr); /* ownership-relinquish (head-of-queue is leaving) */
						}
					}
				}
			}
		}
	}
	}

	/***/

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(temp_argr) ) {

			if( temp_argr != argr ) { /* not this */

				if( temp_argr->cm.sv.nr_of_pdev_submodules != 0 && abort_pdev_ARs ) {

					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PDEV, 0);
					cm_ar_com_down(temp_argr, CM_AR_REASON_PDEV); /* AP00914416 */
				}

				cm_sv_ar_problem_indicator_eval(temp_argr); /* trigger: cm_sv_ownership_relinquish() */

				cm_sv_ar_submodule_state_action(temp_argr); /* trigger: cm_sv_ownership_relinquish() */
			}
		}
	}
	}
}

/*===========================================================================*/
/*=== OWNERSHIP ACTION ======================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_ownership_action(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT16 cand_session_key
) {
	LSA_UINT16 prev_ar_nr = 0;

	/***/

	if( ! (cand_session_key == 0 || cand_session_key == argr->session_key) ) {

		CM_FATAL(); /* user messed up the ownership.rsp */
	}
	else {

#if CM_CFG_MAX_SV_ARS >= 2 /* cand[1] is within bounds */

		if(
			su->cand[1].ar_nr == argr->ar_nr
			&& su->cand[1].is_supervisor
		) {

			/* supervisor-takeover, see cm_sv_ar_ownership_build() */
			/* note: cand_ar_nr == 0 does not mean "do not takeover", it may mean "so-locked" */

			if(
				su->cand[0].no_takeover
				|| CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) /* pdev-submodules are different */
			) {

				if( cand_session_key != 0 ) {

					CM_FATAL(); /* a bug, see cm_sv_ar_ownership_build() */
				}
			}
			else { /* takeover */

				su->owner_ar_nr = 0;

				prev_ar_nr = su->cand[0].ar_nr;

				/***/

				{
				CM_SV_CANDIDATE_TYPE tmp = su->cand[0];
				su->cand[0] = su->cand[1];
				su->cand[1] = tmp;
				}

				/***/

#if CM_CFG_MAX_SV_ARS > 2 /* #if added to avoid LINT warning "loop not entered" (AP01235721) */
				{
				LSA_UINT16 cand_idx;

				for( cand_idx = 2; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

					LSA_UINT16 pred_idx = cand_idx - 1;

					if( su->cand[cand_idx].ar_nr == 0 ) {

						break;
					}
					else if( ! su->cand[cand_idx].is_supervisor ) {

						break;
					}
					else { /* swap */

						CM_SV_CANDIDATE_TYPE tmp = su->cand[pred_idx];
						su->cand[pred_idx] = su->cand[cand_idx];
						su->cand[cand_idx] = tmp;
					}
				}
				}
#endif
			}
		}

#endif

		/***/

		if( cand_session_key == 0 ) { /* user said "superordinated locked" or ownership was not offered (connect-context) */

			if( su->owner_ar_nr == argr->ar_nr ) {

				su->owner_ar_nr = 0; /* now locked (AP01273403) */
			}
		}
		else { /* user said "owned" */

			LSA_UINT16 cand_ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(cand_session_key);

			if( su->cand[0].ar_nr != cand_ar_nr ) {

				if( cm_is_not_null(argr->cm.sv.arset) ) { /* AP01270899 */

					CM_AR_GRAPH_PTR_TYPE cand_argr = cm_sv_ar_lookup2(argr->cm.sv.dev, su->cand[0].ar_nr);

					if( cm_is_null(cand_argr) || cand_argr->cm.sv.arset != argr->cm.sv.arset ) {

						CM_FATAL();
					}
					else {

						/* do nothing (other AR of AR-set took over) */
					}
				}
				else {

					CM_FATAL(); /* a bug */
				}
			}
			else {

				su->owner_ar_nr = cand_ar_nr; /* now owned! */
			}
		}
	}

	/***/

	return prev_ar_nr;
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
