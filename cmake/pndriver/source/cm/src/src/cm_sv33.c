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
/*  F i l e               &F: cm_sv33.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Subslot things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7330
#define CM_MODULE_ID       7330

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== SUBSLOT RANGE-CHECK ===================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_subslot_range_ok(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {

	if( subslot_nr <= dev->max_subslot_nr ) {

		return LSA_TRUE; /* ok (subslot_nr 0 is allowed due to AP00517543) */
	}

	if( CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

		/* PNIO-Spec: The Subslot 0x8000 - 0x8FFF shall exist only in API 0 (of the pdev-device) */
		/* history: AP00284316 (not testing the 'i') was revoked */

		if(
			api == 0
			&& CM_SV_SUBSLOT_IS_PDEV(subslot_nr)
		) {

			if( (subslot_nr & 0xFF00) == dev->val_8i00 ) {

				CM_ASSERT(CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr));

				if( cm_pdsv_mapping_ok(dev->channel, slot_nr, subslot_nr) ) {

					return LSA_TRUE; /* ok, subsequently: pdev-submodule <=> CM_SV_SUBSLOT_IS_PDEV() */
				}
			}

			if( dev->R1_enabled ) { //R1 marker ("other pdev is range-ok")

				if( (subslot_nr & 0xF000) == ((dev->val_8i00 ^ 0x1000) & 0xF000) ) {

					CM_ASSERT(!CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr));

					return LSA_TRUE; /* ok (other PDEV) */
				}
			}
		}
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== SUBSLOT LOOKUP ========================================================*/
/*===========================================================================*/

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_lookup(
	CM_SV_SLOT_CONST_PTR_TYPE sl,
	LSA_UINT16 subslot_nr
) {
	CM_SV_SUBSLOT_PTR_TYPE su;

	for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

		if( su->subslot_nr == subslot_nr ) {

			return su;
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== SUBSLOT COMPARE-FUNCTION ==============================================*/
/*===========================================================================*/

static LSA_BOOL
cm_list_cmp__subslot__subslot_nr(
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
) {
	CM_SV_SUBSLOT_PTR_TYPE p = (CM_SV_SUBSLOT_PTR_TYPE)ListEntry;
	CM_SV_SUBSLOT_PTR_TYPE e = (CM_SV_SUBSLOT_PTR_TYPE)NewEntry;

	CM_ASSERT(p->subslot_nr != e->subslot_nr); /* no duplicates, tested before insertion */

	return p->subslot_nr < e->subslot_nr;
}

/*===========================================================================*/
/*=== SUBSLOT ALLOC =========================================================*/
/*===========================================================================*/

static CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_alloc(
	CM_SV_SLOT_PTR_TYPE sl,
	LSA_UINT16 subslot_nr
) {
	CM_SV_SUBSLOT_PTR_TYPE su = cm_sv_subslot_lookup(sl, subslot_nr);

	if( cm_is_null(su) ) {

		CM_LOCAL_MEM_PTR_TYPE local_mem;

		CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*su));
		su = (CM_SV_SUBSLOT_PTR_TYPE)local_mem;

		if( cm_is_null(su) ) {

			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "cannot alloc local-mem"
				);
		}
		else {

			su->subslot_nr = subslot_nr;

			su->sm_refcnt = 0;

			su->ar_refcnt = 0;

			/***/

			su->sub_ident = 0; /* sanity */

			su->im0_bits = CM_SV_IM0_BITS_NOTHING; /* sanity */

			/***/

			{
			LSA_UINT16 diag_index;

			for( diag_index = 0; diag_index < CM_SV_DIAG_TYPE_MAX; ++diag_index ) {

				CmListInitialize(&su->diag_list[diag_index]);
			}
			}

			/***/

			su->owner_ar_nr = 0;

			{
			LSA_UINT16 cand_idx;

			for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

				su->cand[cand_idx].ar_nr = 0; /* not owned */
			}
			}

			/***/

			CmListInsertSorted(&sl->subslot_list, &su->link, cm_list_cmp__subslot__subslot_nr);
		}
	}

	/***/

	return su;
}

/*===========================================================================*/
/*=== SUBSLOT FREE-IF-UNREFERENCED ==========================================*/
/*===========================================================================*/

static CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_free_if_unreferenced(
	CM_SV_SUBSLOT_PTR_TYPE su
) {
	CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "subslot(%u) sm_refcnt(%u) ar_refcnt(%u)"
		, su->subslot_nr, su->sm_refcnt, su->ar_refcnt
		);

	if( su->sm_refcnt == 0 && su->ar_refcnt == 0 ) {

		CM_SV_SUBSLOT_PTR_TYPE temp = su;

		su = CmListPrevOrHead(&su->link, CM_SV_SUBSLOT_PTR_TYPE);
		/* see where called in a loop, CmListNext is the next operation */

		CmListRemoveEntry(&temp->link);

		/***/

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "subslot(%u) unreferenced, unlink and free"
			, temp->subslot_nr
			);

		{
		LSA_UINT16 diag_index;

		for( diag_index = 0; diag_index < CM_SV_DIAG_TYPE_MAX; ++diag_index ) {

			CM_ASSERT(CmListIsEmpty(&temp->diag_list[diag_index]));
		}
		}

		CM_ASSERT(temp->owner_ar_nr == 0);

		{
		LSA_UINT16 cand_idx;

		for( cand_idx = 0; cand_idx < CM_CFG_MAX_SV_ARS; ++cand_idx ) {

			CM_ASSERT(temp->cand[cand_idx].ar_nr == 0);
		}
		}

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, temp);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}
	}

	CM_ASSERT(cm_is_not_null(su));

	return su;
}

/*===========================================================================*/
/*=== SUBSLOT ATTACH ========================================================*/
/*===========================================================================*/

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_attach(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_SLOT_PTR_TYPE sl,
	LSA_UINT16 subslot_nr,
	LSA_BOOL from_submodule
) {
	CM_SV_SUBSLOT_PTR_TYPE su = cm_sv_subslot_alloc(sl, subslot_nr);

	if( cm_is_not_null(su) ) {

		if( from_submodule ) {

			if( su->sm_refcnt >= 3/*!*/ ) {
				CM_FATAL(); /* a bug */
			}

			su->sm_refcnt += 1;

			CM_ASSERT(CM_SV_SUBSLOT_IS_PLUGGED(su)); /* consistency check */
		}
		else {

			if( su->ar_refcnt >= dev->explicit_ar.max_ARs_TOT ) {
				CM_FATAL(); /* a bug */
			}

			su->ar_refcnt += 1;
		}
	}

	return su;
}

/*===========================================================================*/
/*=== SUBSLOT DETACH ========================================================*/
/*===========================================================================*/

CM_SV_SUBSLOT_PTR_TYPE
cm_sv_subslot_detach(
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_BOOL from_submodule
) {

	if( from_submodule ) {

		/* note: sm_refcnt is 2 if adding twice */

		if( su->sm_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		su->sm_refcnt -= 1;

		/***/

		if( su->sm_refcnt == 0 ) {

			cm_sv_diag_delete(su, 0/*all*/, LSA_NULL);
		}
	}
	else {

		if( su->ar_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		su->ar_refcnt -= 1;
	}

	su = cm_sv_subslot_free_if_unreferenced(su);

	return su;
}

/*===========================================================================*/
/*=== SUBMODULE IM0_BITS CHECK ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_im0_bits_ok( /* note: called too from CM-PD */
	LSA_UINT8 im0_bits,
	LSA_BOOL cm_pd_has_im5_record
) {

	if(
		(im0_bits & CM_SV_IM0_BITS_RESERVED) != 0
		|| ((im0_bits & CM_SV_IM0_BITS_MODULE) != 0 && (im0_bits & CM_SV_IM0_BITS_SUBMODULE) == 0)
		|| ((im0_bits & CM_SV_IM0_BITS_DEVICE) != 0 && (im0_bits & CM_SV_IM0_BITS_SUBMODULE) == 0)
	) {

		return LSA_FALSE;
	}

	if( (im0_bits & CM_SV_IM0_BITS_IM5) != 0 ) {

		/* note: CM_SV_IM0_BITS_IM5 is not a modifier of CM_SV_IM0_BITS_SUBMODULE (TIA 1650262) */

		if( (im0_bits & CM_SV_IM0_BITS_R1_REMOTE) != 0 ) {

			/* no check here, the I&M5 record is handled by the other head */
		}
		else {

			/* note: cannot call cm_pdusr_exists_im5_record() here because of caller CM_OPC_PD_CREATE */

			if( ! cm_pd_has_im5_record ) {

				return LSA_FALSE; /* no CM_PD_CREATE_TYPE::oem_data */
			}
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== SUBMODULE PARAMETER CHECK =============================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_submodule_params_ok(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_CONST_PTR_TYPE elem
) {
	if( ! (
		cm_sv_slot_range_ok(dev, elem->slot_nr)
		&& cm_sv_subslot_range_ok(dev, elem->api, elem->slot_nr, elem->subslot_nr))
	) {

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) slot_nr or subslot_nr out of range (or not mapped in pdev)"
			, dev->device_nr
			, elem->api, elem->slot_nr, elem->subslot_nr
			);

		return LSA_FALSE;
	}

	/***/

	if( CM_SV_SUBSLOT_IS_PDEV(elem->subslot_nr) ) {

		/* do nothing, the pdev-mapping is already checked cm_sv_subslot_range_ok() */

		/* note: with the feature "pull/plug pdev-submodules" the range check is self-fulfilling */
	}

	/***/

	if(
		! cm_sv_im0_bits_ok(elem->im0_bits, cm_pdusr_exists_im5_record(dev->channel))
		|| ((elem->im0_bits & CM_SV_IM0_BITS_R1_REMOTE) != 0 && ! dev->R1_enabled) /* TIA 1915400 */
	) {

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) invalid im0_bits(0x%02x)"
			, dev->device_nr
			, elem->api, elem->slot_nr, elem->subslot_nr
			, elem->im0_bits
			);

		return LSA_FALSE;
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== SUBMODULE ADD ONE ELEMENT =============================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_submodule_add_elem(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_user
) {
	CM_SV_AP_PTR_TYPE ap      = LSA_NULL;
	CM_SV_SLOT_PTR_TYPE sl    = LSA_NULL;
	CM_SV_SUBSLOT_PTR_TYPE su = LSA_NULL;
	LSA_UINT16 rsp;

	CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u)"
		, dev->device_nr
		, elem->api, elem->slot_nr, elem->subslot_nr
		);

	if( ! cm_sv_submodule_params_ok(dev, elem) ) {

		rsp = CM_ERR_PARAM;
	}
	else {

		ap = cm_sv_ap_attach(dev, elem->api, LSA_TRUE);

		sl = cm_is_null(ap) ? LSA_NULL : cm_sv_slot_attach(dev, ap, elem->slot_nr, LSA_TRUE);

		su = cm_is_null(sl) ? LSA_NULL : cm_sv_subslot_attach(dev, sl, elem->subslot_nr, LSA_TRUE);

		if( cm_is_null(su) ) {

			rsp = CM_ERR_ALLOC_LOCAL;
		}
		else if( su->sm_refcnt > 1 ) { /* tricky */

			CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) ap/sl/su(%u/%u/%u) is already plugged"
				, dev->device_nr
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			rsp = CM_ERR_SEQUENCE;
		}
		else { /* new */

			CM_ASSERT(CM_SV_SUBSLOT_IS_PLUGGED(su));

			{
			LSA_UINT16 diag_index;

			for( diag_index = 0; diag_index < CM_SV_DIAG_TYPE_MAX; ++diag_index ) {

				CM_ASSERT(CmListIsEmpty(&su->diag_list[diag_index]));
			}
			}

			rsp = CM_OK;
		}
	}

	if( rsp == CM_OK && cm_is_not_null(sl) && cm_is_not_null(su) ){

		if( sl->sm_refcnt > 1 && elem->mod_ident != sl->mod_ident ) {

			CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) ap/sl/su(%u/%u/%u) mod_ident(%u != %u)"
				, dev->device_nr
				, elem->api, elem->slot_nr, elem->subslot_nr
				, elem->mod_ident, sl->mod_ident
				);

			rsp = CM_ERR_PARAM;
		}
		else if( is_user && CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, su->subslot_nr) ) { /* note: for symmetry reasons not part of cm_sv_submodule_params_ok() */

			CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) ap/sl/su(%u/%u/%u) is local pdev-submodule"
				, dev->device_nr
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			rsp = CM_ERR_PARAM;
		}
		else {

			sl->mod_ident = elem->mod_ident; /* see consistency-check above */

			su->sub_ident = elem->sub_ident;

			su->im0_bits  = elem->im0_bits;

			rsp = CM_OK;

			/***/

			{
			CM_AR_GRAPH_PTR_TYPE cand_argr = (su->cand[0].ar_nr != 0 ? cm_sv_ar_lookup2(dev, su->cand[0].ar_nr) : LSA_NULL);

			if( cm_is_not_null(cand_argr) && cm_is_not_null(ap) ) {

				cm_sv_ar_submodule_state_changed(cand_argr, ap->api, sl->slot_nr, su->subslot_nr); /* submodule-add */
			}
			}
		}
	}

	if( rsp != CM_OK ) {

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) could not add, rsp(%u)"
			, dev->device_nr
			, elem->api, elem->slot_nr, elem->subslot_nr
			, rsp
			);

		if( cm_is_not_null(su) ) {

			cm_sv_subslot_detach(su, LSA_TRUE);
		}

		if( cm_is_not_null(sl) ) {

			cm_sv_slot_detach(sl, LSA_TRUE);
		}

		if( cm_is_not_null(ap) ) {

			cm_sv_ap_detach(ap, LSA_TRUE);
		}
	}

	return rsp;
}

/*===========================================================================*/
/*=== ITER SUBSLOT INVALIDATE ===============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_iter_subslot_invalidate(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CM_SV_SUBSLOT_CONST_PTR_TYPE su_invalid
) {
	if(
		cm_is_not_null(iter)
		&& iter->pdev.state != CM_PD_ITER_PDEV_IDLE
		&& iter->subslot == su_invalid
	) {

		LSA_UINT16 state = iter->pdev.state;

		if( CM_PD_ITER_PDEV_MultipleBlockHeaderReal <= state && state <= CM_PD_ITER_PDEV_PDRealDataComplete ) {

			iter->pdev.state = CM_PD_ITER_PDEV_PDRealDataInvalidated;
		}
		else if( CM_PD_ITER_PDEV_MultipleBlockHeaderExpected <= state && state <= CM_PD_ITER_PDEV_PDExpectedDataComplete ) {

			iter->pdev.state = CM_PD_ITER_PDEV_PDExpectedDataInvalidated;
		}
		else {

			CM_FATAL(); /* a bug */
		}
	}
	else {

		/* Nothing to do. */
	}
}

/*===========================================================================*/
/*=== SUBMODULE REMOVE ONE ELEMENT ==========================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_submodule_remove_elem(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_user
) {
	CM_SV_AP_PTR_TYPE ap;
	CM_SV_SLOT_PTR_TYPE sl;
	CM_SV_SUBSLOT_PTR_TYPE su;
	LSA_UINT16 rsp;

	CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u)"
		, dev->device_nr
		, elem->api, elem->slot_nr, elem->subslot_nr
		);

	ap = cm_sv_ap_lookup(dev, elem->api);

	sl = cm_is_null(ap) ? LSA_NULL : cm_sv_slot_lookup(ap, elem->slot_nr);

	su = cm_is_null(sl) ? LSA_NULL : cm_sv_subslot_lookup(sl, elem->subslot_nr);

	if( cm_is_null(ap) || cm_is_null(sl) || cm_is_null(su) || ! CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) is not plugged"
			, dev->device_nr
			, elem->api, elem->slot_nr, elem->subslot_nr
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( is_user && CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, su->subslot_nr) ) {

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) is local pdev-submodule"
			, dev->device_nr
			, elem->api, elem->slot_nr, elem->subslot_nr
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 ar_refcnt = su->ar_refcnt; /* save before cm_sv_subslot_detach() */
		CM_SV_SUBSLOT_PTR_TYPE next_su;

		/*
		su->sub_ident = not erased because the value is needed for the pull-alarm
		sl->mod_ident = not erased because the value is needed for the pull-alarm
		*/

		next_su = cm_sv_subslot_detach(su, LSA_TRUE);

		cm_sv_slot_detach(sl, LSA_TRUE);

		cm_sv_ap_detach(ap, LSA_TRUE);

		/*** must do the actions afterwards ***/

		if( ar_refcnt != 0 ) { /* referenced by a candidate, thus ap/sl/su still valid */

			CM_AR_GRAPH_PTR_TYPE cand_argr = (su->cand[0].ar_nr != 0 ? cm_sv_ar_lookup2(dev, su->cand[0].ar_nr) : LSA_NULL);

			if( cm_is_not_null(cand_argr) && cm_is_not_null(ap) && cm_is_not_null(sl) ) {

				cm_sv_ar_submodule_state_changed(cand_argr, ap->api, sl->slot_nr, su->subslot_nr); /* submodule-remove */

				if( !is_user ) { /* i.e. pdev-submodule */

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					cm_ar_graph_find_api_slot_subslot(cand_argr, ap->api, sl->slot_nr, su->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

					if( cm_is_not_null(ar_sub) ) { /* always true */

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_PULLPDEV;
					}
				}
			}
		}

		/***/

		if( next_su != su ) { /* subslot detached */

			LSA_UINT16 ar_nr;

			for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

				CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_not_null(temp_argr) ) {

					cm_sv_iter_subslot_invalidate(temp_argr->cm.sv.rec.iter_ptr, su);
				}
			}

			cm_sv_iter_subslot_invalidate(&dev->implicit_ar.iter, su);
		}

		rsp = CM_OK;
	}

	return rsp;
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
