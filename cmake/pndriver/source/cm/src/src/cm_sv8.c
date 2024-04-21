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
/*  F i l e               &F: cm_sv8.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Record things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7800
#define CM_MODULE_ID       7800

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_iter_ar_next(
	CM_SV_ITERATOR_PTR_TYPE iter
); /* forward */

static LSA_UINT32
cm_sv_record_read_pdev_first(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
); /* forward */

/*===========================================================================*/
/*=== ITERATOR / SETUP ======================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_iter_setup(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	LSA_UINT16 flags,
	LSA_INT need_AR /* true: need "argr" (expected config) */
) {

#if CM_DEBUG
	CM_MEMSET(iter, 0xCC, sizeof(*iter));
#endif

	/***/

	CM_ASSERT(
		flags == CM_SV_ITER_SUB
		|| flags == CM_SV_ITER_MOD
		|| flags == CM_SV_ITER_AR
		|| flags == CM_SV_ITER_REAL_AP
		|| flags == CM_SV_ITER_REAL_DEV
	);

	/***/

	if( need_AR ) {

		flags |= CM_SV_ITER__ONE_AR;

		if( cm_is_null(req->argr) ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) index(0x%x) needs AR"
				, dev->device_nr
				, req->index
				);
			return CM_PNIO_FAULTY_Record__InvalidParameter(12); /* index needs target_uuid (code2?) */
		}
	}

	if( (flags & CM_SV_ITER_SUB) ) {
		flags |= CM_SV_ITER__ONE_SUB;
		flags |= CM_SV_ITER_MOD; /* implied by one-sub */
	}

	if( (flags & CM_SV_ITER_MOD) ) {
		flags |= CM_SV_ITER__ONE_MOD;
		flags |= CM_SV_ITER__ONE_AP; /* implied by one-mod */
	}

	if( (flags & CM_SV_ITER_REAL_AP) ) {
		flags |= CM_SV_ITER__ONE_AP;
		flags |= CM_SV_ITER__ALL_ARS;
	}

	if( (flags & CM_SV_ITER_REAL_DEV) ) {
		CM_ASSERT(cm_is_null(req->ap)); /* see caller */
		flags |= CM_SV_ITER__ALL_ARS;
	}

	/***/

	iter->dev = dev;

	iter->argr = req->argr;

	iter->flags = flags;

	iter->rq_api        = req->api;
	iter->rq_slot_nr    = req->slot_nr;
	iter->rq_subslot_nr = req->subslot_nr;

	iter->ar_nr = 0; /* sanity, iter->argr is the "primary key" */

	/***/

	CM_ASSERT(
		cm_is_null(req->ap)
		|| req->ap->api == req->api /* see caller */
	);

	CM_ASSERT(
		(iter->flags & CM_SV_ITER__ONE_AP) == 0
		|| cm_is_not_null(req->ap)
	);

	iter->ap      = req->ap; /* LSA_NULL if index >= 0xF800 (API shall be ignored) */
	iter->slot    = LSA_NULL;
	iter->subslot = LSA_NULL;

	/***/

	iter->ar_ap  = LSA_NULL;
	iter->ar_mod = LSA_NULL;
	iter->ar_sub = LSA_NULL;

	/***/

	if( (iter->flags & CM_SV_ITER__ALL_ARS) ) {

		iter->argr = LSA_NULL; /* sanity, see cm_sv_iter_ar_first() */
	}

#if 0 /* AP00424001 - assertion below is wrong for easy-supervisor (assignment not necessary) */
	if( cm_is_not_null(iter->argr) ) {
		if( cm_is_not_null(iter->ap) ) {
			iter->ar_ap = cm_ar_graph_find_ap(iter->argr, iter->ap->api);
			CM_ASSERT(cm_is_not_null(iter->ar_ap)); /* must exist (see connect) */
		}
	}
#endif

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== ITERATOR / APS ========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_iter_ap_first(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_ASSERT(cm_is_not_null(iter->dev));

	if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		if( (iter->flags & CM_SV_ITER__ONE_AP) ) {

			iter->ar_ap = cm_ar_graph_find_ap(iter->argr, iter->rq_api);

			if( cm_is_null(iter->ar_ap) ) { /* api not withing ar-context */

				if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

					CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
						, "api(%u) not in AR, using real-config", iter->rq_api
						);

					goto use_real_config; /* AP00431609, use "real config" */
				}
				else if( (iter->flags & CM_SV_ITER__ONE_MOD) ) {

					if( cm_is_iosar_with_device_access(iter->argr) ) { /* easy-supervisor */

						CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
							, "api(%u) not in AR, using real-config", iter->rq_api
							);

						goto use_real_config; /* AP00431609, use "real config" */
					}
					else { /* note: semantics "which subslots" undefined */

						CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
							, "api(%u) not in AR, using empty-config", iter->rq_api
							);

						/* AP00431609: return Ok / 0 Bytes, not CM_PNIO_FAULTY_Record__AccessDenied(6) */
					}
				}
				else { /* note: unreachable, see cm_sv_record_check_range() and E000 iterator-setup */

					CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
						, "api(%u) not in AR", iter->rq_api
						);

					return CM_PNIO_FAULTY_Record__AccessDenied(6); /* unreachable */
				}
			}
		}
		else { /* all */

			iter->ar_ap = CmListFirst(&iter->argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE);

			if( cm_is_null(iter->ar_ap) ) {

				/* e.g. easy-supervisor (has no expected config) */
			}
		}

		/* sync the ap */

		if( cm_is_not_null(iter->ar_ap) ) {

			iter->ap = iter->ar_ap->sv.real_ap; /* must exist (see vivification) */
		}
		else {

			iter->ap = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

use_real_config:

		CM_ASSERT(cm_is_null(iter->ar_ap));

		if( (iter->flags & CM_SV_ITER__ONE_AP) ) {

			iter->ap = cm_sv_ap_lookup(iter->dev, iter->rq_api);

			if( cm_is_null(iter->ap) ) {

				/* assume: not yet added */
			}
		}
		else { /* all */

			iter->ap = CmListFirst(&iter->dev->ap_list, CM_SV_AP_PTR_TYPE);
		}
	}

	/***/

	iter->slot    = LSA_NULL; /* sanity */
	iter->subslot = LSA_NULL; /* sanity */

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_iter_ap_valid(
	CM_SV_ITERATOR_CONST_PTR_TYPE iter
) {
	CM_ASSERT(
		cm_is_null(iter->ar_ap)
		|| cm_is_not_null(iter->ap)
	);

	if( cm_is_null(iter->ap) ) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_iter_ap_next(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	if( (iter->flags & CM_SV_ITER__ONE_AP) ) {

		iter->ap = LSA_NULL;
		iter->ar_ap = LSA_NULL;
	}
	else if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		CM_ASSERT(cm_is_not_null(iter->ar_ap));

		iter->ar_ap = CmListNext(&iter->argr->ap_list, &iter->ar_ap->link, CM_AR_GRAPH_AP_PTR_TYPE);

		/* sync the ap */

		if( cm_is_not_null(iter->ar_ap) ) {

			iter->ap = iter->ar_ap->sv.real_ap; /* must exist (see vivification) */
		}
		else {

			iter->ap = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

		CM_ASSERT(cm_is_not_null(iter->ap));

		iter->ap = CmListNext(&iter->dev->ap_list, &iter->ap->link, CM_SV_AP_PTR_TYPE);
	}
}

/*===========================================================================*/
/*=== ITERATOR / SLOTS, MODULES =============================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_iter_mod_first(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/

	CM_ASSERT(cm_is_not_null(iter->ap));

	if( (iter->flags & CM_SV_ITER__ONE_MOD) ) {

		CM_ASSERT((iter->flags & CM_SV_ITER__ONE_AP));

		if( ! cm_sv_slot_range_ok(iter->dev, iter->rq_slot_nr) ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "slot(%u) out of range", iter->rq_slot_nr
				);
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(7); /* slot out-of-range */
		}
	}
	else {

		/*don't: CM_ASSERT(iter->rq_slot_nr == 0);*/
	}

	if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		if( cm_is_null(iter->ar_ap) ) {

			goto use_real_config; /* AP00431609, use "real config" */
		}

		CM_ASSERT(cm_is_not_null(iter->ar_ap));

		if( (iter->flags & CM_SV_ITER__ONE_MOD) ) {

			iter->ar_mod = cm_ar_graph_find_module(iter->ar_ap, iter->rq_slot_nr);

			if( cm_is_null(iter->ar_mod) ) { /* slot not withing ar-context */

				if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

					CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
						, "slot(%u) not in AR, using real-config", iter->rq_slot_nr
						);

					goto use_real_config; /* AP00431609, use "real config" */
				}
				else { /* note: semantics "which subslots" undefined */

					CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
						, "slot(%u) not in AR, using empty-config", iter->rq_slot_nr
						);

					/* AP00431609: return Ok / 0 Bytes, not CM_PNIO_FAULTY_Record__InvalidSlotSubslot(7) */
				}
			}
		}
		else { /* all modules (and submodules) */

			iter->ar_mod = CmListFirst(&iter->ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE);

			if( cm_is_null(iter->ar_mod) ) {

				/* strange AR, but ok in this context */
			}
		}

		/* sync the slot */

		if( cm_is_not_null(iter->ar_mod) ) {

			iter->slot = iter->ar_mod->sv.real_sl; /* must exist (see vivification) */
		}
		else {

			iter->slot = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

use_real_config:

		CM_ASSERT(cm_is_null(iter->ar_mod));

		if( (iter->flags & CM_SV_ITER__ONE_MOD) ) {

			iter->slot = cm_sv_slot_lookup(iter->ap, iter->rq_slot_nr);

			if( cm_is_null(iter->slot) ) {

				/* assume: not yet plugged, since range is ok */
			}
		}
		else { /* all modules (and submodules) */

			iter->slot = CmListFirst(&iter->ap->slot_list, CM_SV_SLOT_PTR_TYPE);
		}
	}

	/***/

	iter->subslot = LSA_NULL; /* sanity */

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_iter_mod_valid(
	CM_SV_ITERATOR_CONST_PTR_TYPE iter
) {
	CM_ASSERT(
		cm_is_null(iter->ar_mod)
		|| cm_is_not_null(iter->slot)
	);

	if( cm_is_null(iter->slot) ) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_iter_mod_next(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	CM_ASSERT(cm_is_not_null(iter->ap));

	if( (iter->flags & CM_SV_ITER__ONE_MOD) ) {

		iter->slot = LSA_NULL;
		iter->ar_mod = LSA_NULL;
	}
	else if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		CM_ASSERT(cm_is_not_null(iter->ar_ap));
		CM_ASSERT(cm_is_not_null(iter->ar_mod));

		iter->ar_mod = CmListNext(&iter->ar_ap->mod_list, &iter->ar_mod->link, CM_AR_GRAPH_MODULE_PTR_TYPE);

		/* sync the slot */

		if( cm_is_not_null(iter->ar_mod) ) {

			iter->slot = iter->ar_mod->sv.real_sl; /* must exist (see vivification) */
		}
		else {

			iter->slot = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

		CM_ASSERT(cm_is_not_null(iter->slot));

		iter->slot = CmListNext(&iter->ap->slot_list, &iter->slot->link, CM_SV_SLOT_PTR_TYPE);
	}
}

/*===========================================================================*/
/*=== ITERATOR / SUBSLOTS, SUBMODULES =======================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_iter_sub_first(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_ASSERT(cm_is_not_null(iter->ap));
	CM_ASSERT(cm_is_not_null(iter->slot));

	if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

		CM_ASSERT((iter->flags & CM_SV_ITER__ONE_MOD));

		if( ! cm_sv_subslot_range_ok(iter->dev, iter->ap->api, iter->slot->slot_nr, iter->rq_subslot_nr) ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "subslot(%u) out of range", iter->rq_subslot_nr
				);
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(8); /* subslot out-of-range */
		}
	}
	else {

		/*don't: CM_ASSERT(iter->rq_subslot_nr == 0);*/
	}

	if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		if( cm_is_null(iter->ar_mod) ) {

			goto use_real_config; /* AP00431609, use "real config" */
		}

		CM_ASSERT(cm_is_not_null(iter->ar_ap));
		CM_ASSERT(cm_is_not_null(iter->ar_mod));

		if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

			iter->ar_sub = cm_ar_graph_find_submodule(LSA_NULL, iter->ar_mod, 0, iter->rq_subslot_nr);

			if( cm_is_null(iter->ar_sub) ) { /* subslot not withing ar-context */

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
					, "subslot(%u) not in AR, using real-config", iter->rq_subslot_nr
					);

				goto use_real_config; /* AP00431609, use "real config" */
			}
		}
		else { /* all submodules */

			iter->ar_sub = CmListFirst(&iter->ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE);

			if( cm_is_null(iter->ar_sub) ) {

				/* strange AR, but ok in this context */
			}
		}

		/* sync the subslot */

		if( cm_is_not_null(iter->ar_sub) ) {

			iter->subslot = iter->ar_sub->sv.real_su; /* must exist (see vivification) */
		}
		else {

			iter->subslot = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

use_real_config:

		CM_ASSERT(cm_is_null(iter->ar_sub));

		if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

			iter->subslot = cm_sv_subslot_lookup(iter->slot, iter->rq_subslot_nr);

			if( cm_is_null(iter->subslot) ) {

				/* assume: not yet plugged, since range is ok */
			}
		}
		else { /* all subslots */

			iter->subslot = CmListFirst(&iter->slot->subslot_list, CM_SV_SUBSLOT_PTR_TYPE);
		}
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_iter_sub_valid(
	CM_SV_ITERATOR_CONST_PTR_TYPE iter
) {
	/* not true: CM_ASSERT(CM_SV_SLOT_IS_PLUGGED(iter->slot)); */

	CM_ASSERT(
		cm_is_null(iter->ar_sub)
		|| cm_is_not_null(iter->subslot)
	);

	if( cm_is_null(iter->subslot) ) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_iter_sub_next(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_ASSERT(cm_is_not_null(iter->ap));
	CM_ASSERT(cm_is_not_null(iter->slot));

	if( (iter->flags & CM_SV_ITER__ONE_SUB) ) {

		iter->subslot = LSA_NULL;
		iter->ar_sub  = LSA_NULL;
	}
	else if( cm_is_not_null(iter->argr) ) { /* iterate over "expected config" */

		if( cm_is_null(iter->ar_mod) ) {

			goto use_real_config; /* AP00431609, use "real config" */
		}

		CM_ASSERT(cm_is_not_null(iter->ar_ap));
		CM_ASSERT(cm_is_not_null(iter->ar_mod));
		CM_ASSERT(cm_is_not_null(iter->ar_sub));

		iter->ar_sub = CmListNext(&iter->ar_mod->submod_list, &iter->ar_sub->link, CM_AR_GRAPH_SUBMODULE_PTR_TYPE);

		/* sync the subslot */

		if( cm_is_not_null(iter->ar_sub) ) {

			iter->subslot = iter->ar_sub->sv.real_su; /* must exist (see vivification) */
		}
		else {

			iter->subslot = LSA_NULL;
		}
	}
	else { /* iterate over "real config" */

use_real_config:

		CM_ASSERT(cm_is_not_null(iter->subslot));

		iter->subslot = CmListNext(&iter->slot->subslot_list, &iter->subslot->link, CM_SV_SUBSLOT_PTR_TYPE);
	}
}

/*===========================================================================*/
/*=== ITERATOR / ARS ========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_iter_ar_first(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	if( (iter->flags & CM_SV_ITER__ALL_ARS) ) {

		iter->ar_nr = 0;

		cm_sv_iter_ar_next(iter); /* AP01226390, code changed */
	}
	else {

		CM_ASSERT(cm_is_not_null(iter->argr));
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_iter_ar_valid(
	CM_SV_ITERATOR_CONST_PTR_TYPE iter
) {
	if( cm_is_null(iter->argr) ) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_iter_ar_next(
	CM_SV_ITERATOR_PTR_TYPE iter
) {
	CM_ASSERT((iter->flags & CM_SV_ITER__ONE_AR) == 0);
	CM_ASSERT((iter->flags & CM_SV_ITER__ALL_ARS) != 0);

	/* note: called too from cm_sv_iter_ar_first() */

	for( ++iter->ar_nr; iter->ar_nr <= iter->dev->explicit_ar.max_ARs_TOT; ++iter->ar_nr ) {

		iter->argr = cm_sv_ar_lookup2(iter->dev, iter->ar_nr);

		if( cm_is_not_null(iter->argr) ) {

			if( (iter->flags & CM_SV_ITER__ONE_AP) ) {

				iter->ar_ap = cm_ar_graph_find_ap(iter->argr, iter->ap->api);

				if( cm_is_null(iter->ar_ap) ) {
					continue;
				}
			}

			return;
		}
	}

	iter->argr = LSA_NULL;
}

/*===========================================================================*/
/*=== MARSHAL EXPECTED IDENT ================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_expected_ident(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 offs_blklen;

	/*
	 * ExpectedIdentificationData ::=
	 *   BlockHeader,
	 *   NumberOfAPIs, <---------------- with BlockVersion 1.1
	 *   ( API, <----------------------- with BlockVersion 1.1
	 *     NumberOfSlots,
	 *     ( SlotNumber, ModuleIdentNumber,
	 *       NumberOfSubslots,
	 *       ( SubslotNumber, SubmoduleIdentNumber
	 *       )*
	 *     )*
	 *   )*  <-------------------------- with BlockVersion 1.1
	 */

	if( offs + 2+2+1+1 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_EXP_IDENT);
	offs += 2;
	offs_blklen = offs; /* BlockLength written below */
	offs += 2;
	CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
	offs += 1+1;

	/***/

	{
	LSA_UINT32 offs_nr_of_apis = offs;
	LSA_UINT16 api_cnt;
	LSA_UINT32 pnio_err;

	if( offs + 2 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	api_cnt = 0; /* NumberOfAPIs */
	offs += 2;

	pnio_err = cm_sv_iter_ap_first(iter);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "could not cm_sv_iter_ap_first, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	for(
		; cm_sv_iter_ap_valid(iter)
		; cm_sv_iter_ap_next(iter)
	) {

		LSA_UINT32 offs_api = offs;

		if( cm_is_null(iter->ar_ap) ) { /* may be LSA_NULL due to AP00431609 */

			continue; /* note: unreachable, see cm_sv_record_check_range() and E000 iterator-setup */
		}

		CM_ASSERT(iter->ap->api == iter->ar_ap->api);

		if( offs + 4 + 2 > size ) {

			return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
		}

		CM_PUT32_HTON(data, offs, iter->ap->api);
		offs += 4;

		{
		LSA_UINT32 offs_nr_of_slots = offs;
		LSA_UINT16 mod_cnt;

		mod_cnt = 0; /* NumberOfSlots */
		offs += 2;

		pnio_err = cm_sv_iter_mod_first(iter);

		if( pnio_err != CM_PNIO_ERR_NONE ) {
			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "could not cm_sv_iter_mod_first, pnio_err(0x%x)", pnio_err
				);
			return pnio_err;
		}

		for(
			; cm_sv_iter_mod_valid(iter)
			; cm_sv_iter_mod_next(iter)
		) {

			if( cm_is_null(iter->ar_mod) ) { /* may be LSA_NULL due to AP00431609 */

				continue; /* slot not withing ar-context */
			}

			CM_ASSERT(iter->slot->slot_nr == iter->ar_mod->slot_nr);

			if( offs + 2+4 + 2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
			offs += 2;
			CM_PUT32_HTON(data, offs, iter->ar_mod->soll.mod_ident);
			offs += 4;

			{
			LSA_UINT32 offs_nr_of_subslots = offs;
			LSA_UINT16 sub_cnt;

			sub_cnt = 0; /* NumberOfSubslots */
			offs += 2;

			pnio_err = cm_sv_iter_sub_first(iter);

			if( pnio_err != CM_PNIO_ERR_NONE ) {
				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "could not cm_sv_iter_sub_first, pnio_err(0x%x)", pnio_err
					);
				return pnio_err;
			}

			for(
				; cm_sv_iter_sub_valid(iter)
				; cm_sv_iter_sub_next(iter)
			) {

				if( cm_is_null(iter->ar_sub) ) { /* may be LSA_NULL due to AP00431609 */

					continue; /* subslot not withing ar-context */
				}

				CM_ASSERT(iter->subslot->subslot_nr == iter->ar_sub->subslot_nr);

				if( offs + 2+4 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
				offs += 2;
				CM_PUT32_HTON(data, offs, iter->ar_sub->soll.sub_ident);
				offs += 4;

				/***/

				sub_cnt += 1;
			}

			CM_PUT16_HTON(data, offs_nr_of_subslots, sub_cnt);
			}

			/***/

			mod_cnt += 1;
		}

		CM_PUT16_HTON(data, offs_nr_of_slots, mod_cnt);

		/***/

		if( mod_cnt != 0 ) {

			api_cnt += 1;
		}
		else {

			offs = offs_api; /* rollback */
		}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( api_cnt != 0 ) {

		CM_PUT16_HTON(data, offs_nr_of_apis, api_cnt);

		*offs_ptr = offs; /* update */
	}
	else {

		/* don't update */
	}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL REAL IDENT ====================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_real_ident(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 offs_blklen;

	/*
	 * RealIdentificationData ::=
	 *   BlockHeader,
	 *   NumberOfAPIs, <---------------- with BlockVersion 1.1
	 *   ( API, <----------------------- with BlockVersion 1.1
	 *     NumberOfSlots,
	 *     ( SlotNumber, ModuleIdentNumber,
	 *       NumberOfSubslots,
	 *       ( SubslotNumber, SubmoduleIdentNumber
	 *       )*
	 *     )*
	 *   )*  <-------------------------- with BlockVersion 1.1
	 */

	if( offs + 2+2+1+1 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_REAL_IDENT);
	offs += 2;
	offs_blklen = offs; /* BlockLength written below */
	offs += 2;
	CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
	offs += 1+1;

	/***/

	{
	LSA_UINT32 offs_nr_of_apis = offs;
	LSA_UINT16 api_cnt;
	LSA_UINT32 pnio_err;

	if( offs + 2 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	api_cnt = 0; /* NumberOfAPIs */
	offs += 2;

	pnio_err = cm_sv_iter_ap_first(iter);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "could not cm_sv_iter_ap_first, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	for(
		; cm_sv_iter_ap_valid(iter)
		; cm_sv_iter_ap_next(iter)
	) {

		LSA_UINT32 offs_api = offs;

		if( ! CM_SV_AP_IS_PLUGGED(iter->ap) ) {
			continue;
		}

		if( offs + 4 + 2 > size ) {

			return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
		}

		CM_PUT32_HTON(data, offs, iter->ap->api);
		offs += 4;

		{
		LSA_UINT32 offs_nr_of_slots = offs;
		LSA_UINT16 mod_cnt;

		mod_cnt = 0; /* NumberOfSlots */
		offs += 2;

		pnio_err = cm_sv_iter_mod_first(iter);

		if( pnio_err != CM_PNIO_ERR_NONE ) {
			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "could not cm_sv_iter_mod_first, pnio_err(0x%x)", pnio_err
				);
			return pnio_err;
		}

		for(
			; cm_sv_iter_mod_valid(iter)
			; cm_sv_iter_mod_next(iter)
		) {

			if( ! CM_SV_SLOT_IS_PLUGGED(iter->slot) ) {
				continue;
			}

			if( offs + 2+4 + 2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
			offs += 2;
			CM_PUT32_HTON(data, offs, iter->slot->mod_ident);
			offs += 4;

			{
			LSA_UINT32 offs_nr_of_subslots = offs;
			LSA_UINT16 sub_cnt;

			sub_cnt = 0; /* NumberOfSubslots */
			offs += 2;

			pnio_err = cm_sv_iter_sub_first(iter);

			if( pnio_err != CM_PNIO_ERR_NONE ) {
				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "could not cm_sv_iter_sub_first, pnio_err(0x%x)", pnio_err
					);
				return pnio_err;
			}

			for(
				; cm_sv_iter_sub_valid(iter)
				; cm_sv_iter_sub_next(iter)
			) {

				if( ! CM_SV_SUBSLOT_IS_PLUGGED(iter->subslot) ) {
					continue;
				}

				if( offs + 2+4 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
				offs += 2;
				CM_PUT32_HTON(data, offs, iter->subslot->sub_ident);
				offs += 4;

				/***/

				sub_cnt += 1;
			}

			CM_PUT16_HTON(data, offs_nr_of_subslots, sub_cnt);
			}

			/***/

			mod_cnt += 1;
		}

		CM_PUT16_HTON(data, offs_nr_of_slots, mod_cnt);

		/***/

		if( mod_cnt != 0 ) {

			api_cnt += 1;
		}
		else {

			offs = offs_api; /* rollback */
		}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( api_cnt != 0 ) {

		CM_PUT16_HTON(data, offs_nr_of_apis, api_cnt);

		*offs_ptr = offs; /* update */
	}
	else {

		/* don't update */
	}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL DIAG ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_diag(
	CM_SV_ITERATOR_PTR_TYPE iter,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr,
	LSA_UINT16 index
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 pnio_err;

	/***/

	pnio_err = cm_sv_iter_ap_first(iter);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "could not cm_sv_iter_ap_first, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	for(
		; cm_sv_iter_ap_valid(iter)
		; cm_sv_iter_ap_next(iter)
	) {

		if( ! CM_SV_AP_IS_PLUGGED(iter->ap) ) {
			continue;
		}

		pnio_err = cm_sv_iter_mod_first(iter);

		if( pnio_err != CM_PNIO_ERR_NONE ) {
			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "could not cm_sv_iter_mod_first, pnio_err(0x%x)", pnio_err
				);
			return pnio_err;
		}

		for(
			; cm_sv_iter_mod_valid(iter)
			; cm_sv_iter_mod_next(iter)
		) {

			if( ! CM_SV_SLOT_IS_PLUGGED(iter->slot) ) {
				continue;
			}

			pnio_err = cm_sv_iter_sub_first(iter);

			if( pnio_err != CM_PNIO_ERR_NONE ) {
				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "could not cm_sv_iter_sub_first, pnio_err(0x%x)", pnio_err
					);
				return pnio_err;
			}

			for(
				; cm_sv_iter_sub_valid(iter)
				; cm_sv_iter_sub_next(iter)
			) {

				if( ! CM_SV_SUBSLOT_IS_PLUGGED(iter->subslot) ) {
					continue;
				}

				/*
				 * DiagnosisData ::=
				 *   BlockHeader,
				 *   API, <------------- with BlockVersion 1.1
				 *   ChannelDiagnosis
				 *   ^ ManufacturerSpecificDiagnosis
				 *   ^ ExtChannelDiagnosis
				 *   ^ QualifiedChannelDiagnosis
				 */

				CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
					, "marshalling diag for ap/sl/su(%u/%u/%u)"
					, iter->ap->api, iter->slot->slot_nr, iter->subslot->subslot_nr
					);

				/*
				 * CHANNEL DIAGNOSIS
				 */

				{
				CM_LIST_ENTRY_PTR_TYPE ch_diag_list = &iter->subslot->diag_list[CM_SV_DIAG_TYPE_CHANNEL];

				if( ! CmListIsEmpty(ch_diag_list) ) {

					LSA_UINT32 offs_block;
					LSA_UINT32 offs_chpr_sum;

					/*
					 * ChannelDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber(0x8000), ChannelProperties a), UserStructureIdentifier(0x8000),
					 *   ChannelDiagnosisData*
					 *
					 * a) The field ChannelProperties.Type, the field ChannelProperties.Direction,
					 *    the field ChannelProperties.Maintenance shall be set to zero.
					 *    The field ChannelProperties.Specifier shall be set to appear if at least
					 *    one ChannelProperties.Specifier in the ChannelDiagnosisData is set to
					 *    appear in conjunction with ChannelProperties.Maintenance(=diagnosis).
					 *    Else, the field ChannelProperties.Specifier shall be set to disappear.
					 */

					if( offs + 2+2+1+1 + 4+2+2 + 2+2+2 > size ) {

						return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
					}

					offs_block = offs;

					CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
					offs += 2;
					CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
					offs += 1+1;

					CM_PUT32_HTON(data, offs, iter->ap->api);
					offs += 4;
					CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
					offs += 2;
					CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
					offs += 2;

					CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
					offs += 2;
					offs_chpr_sum = offs; /* ChannelProperties are written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x8000/*ChannelDiagnosis*/);
					offs += 2;

					/***/

					{
					LSA_UINT32 diag_cnt;
					LSA_UINT16 chpr_sum;

					diag_cnt = 0;
					chpr_sum = CM_CHPR_SPEC_DIS;

					/*
					 * ChannelDiagnosisData ::=
					 *   ChannelNumber, ChannelProperties, ChannelErrorType
					 */

					if( cm_is_not_null(ch_diag_list) ) { /* always true, for symmetry-reasons! */

						CM_SV_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, ch_diag_list, CM_SV_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								CM_ASSERT(diag->data_length == 0);

								if( offs + 2+2+2 > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								CM_PUT16_HTON(data, offs, diag->channel_number);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type_usi);
								offs += 2;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					CM_PUT16_HTON(data, offs_chpr_sum, chpr_sum);

					/***/

					if( diag_cnt != 0 ) {

						LSA_UINT32 blklen = offs - (offs_block + (2+2));

						if( blklen > 0xFFFF ) {
							CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
								, "cannot store blklen(%u)", blklen
								);

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11); /* buffer too small (code2?) */
						}

						CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
					}
					else { /* rollback */

						offs = offs_block;
					}
					}
				}
				}

				/*
				 * EXTENDED CHANNEL DIAGNOSIS
				 */

				{
				CM_LIST_ENTRY_PTR_TYPE ext_real_diag_list;
				CM_LIST_ENTRY_PTR_TYPE ext_phys_diag_list;
				CM_SV_DIAG_PTR_TYPE ext_mcons_diag_entry;
#if CM_CFG_SV_AR_HAS_MCONS
				CM_SV_DIAG_TYPE ext_mcons_diag; /* here, not within an inner block! */
#endif

				/***/

				ext_real_diag_list = &iter->subslot->diag_list[CM_SV_DIAG_TYPE_EXT_CHANNEL];

				if( CmListIsEmpty(ext_real_diag_list) ) {

					ext_real_diag_list = LSA_NULL;
				}

				/***/

				if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(iter->dev, iter->subslot->subslot_nr) ) {

					ext_phys_diag_list = cm_pdsv_diag_read(iter->dev->channel, iter->slot->slot_nr, iter->subslot->subslot_nr);

					if( CmListIsEmpty(ext_phys_diag_list) ) {

						ext_phys_diag_list = LSA_NULL;
					}
				}
				else {

					ext_phys_diag_list = LSA_NULL;
				}

				/***/

				ext_mcons_diag_entry = LSA_NULL;

#if CM_CFG_SV_AR_HAS_MCONS
				if(
					cm_is_not_null(iter->argr)
					&& cm_is_not_null(iter->ar_ap)  /* note: could assert only */
					&& cm_is_not_null(iter->ar_mod) /* note: could assert only */
					&& cm_is_not_null(iter->ar_sub) /* note: could assert only */
					&& iter->ar_sub->out_via_mcr /* is a m-consumer-submodule */
				) {
					LSA_UINT16 ext_err;

					ext_err = cm_sv_mcons_diag_value(iter->argr, LSA_NULL, iter->ar_ap, iter->ar_mod, iter->ar_sub);

					CM_ASSERT(
						ext_err == 0 /* Multicast Consumer CR Okay */
						|| ext_err == 0x8000 /* Multicast Consumer CR timed out */
						|| ext_err == 0x8001 /* Address Resolution Failed */
						|| ext_err == 0x8002 /* Multicast Consumer CR Okay with M-Provider Status Mismatch */
					);

					ext_mcons_diag.channel_number = 0x8000/*Submodule*/;

					if( ext_err != 0 ) { /* has (appearing) diagnosis */

						ext_mcons_diag.channel_properties = (CM_CHPR_MAINT_ERR | CM_CHPR_SPEC_APP);

						ext_mcons_diag.channel_error_type_usi = 0x8005/*Multicast CR Mismatch*/;

						ext_mcons_diag.data_length = ext_err; /* tricky */

						/***/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: Komm.diagnose, wenn keine ar/target dann current owner oder gar nichts?")
#endif /* CM_MESSAGE */
						ext_mcons_diag_entry = &ext_mcons_diag;
					}
					else { /* there is no disappearing ext-channel dignosis */

						CM_ASSERT(cm_is_null(ext_mcons_diag_entry));
					}
				}
#endif

				/***/

				if(
					cm_is_not_null(ext_real_diag_list)
					|| cm_is_not_null(ext_phys_diag_list)
					|| cm_is_not_null(ext_mcons_diag_entry)
				) {

					LSA_UINT32 offs_block;
					LSA_UINT32 offs_chpr_sum;

					/*
					 * ExtChannelDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber(0x8000), ChannelProperties a), UserStructureIdentifier(0x8002),
					 *   ExtChannelDiagnosisData *
					 *
					 * a) The field ChannelProperties.Type, the field ChannelProperties.Direction,
					 *    the field ChannelProperties.Maintenance shall be set to zero.
					 *    The field ChannelProperties.Specifier shall be set to appear if at least
					 *    one ChannelProperties.Specifier in the ExtChannelDiagnosisData is set to
					 *    appear in conjunction with ChannelProperties.Maintenance(=diagnosis).
					 *    Else, the field ChannelProperties.Specifier shall be set to disappear.
					 */

					if( offs + 2+2+1+1 + 4+2+2 + 2+2+2 > size ) {

						return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
					}

					offs_block = offs;

					CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
					offs += 2;
					CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
					offs += 1+1;

					CM_PUT32_HTON(data, offs, iter->ap->api);
					offs += 4;
					CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
					offs += 2;
					CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
					offs += 2;

					CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
					offs += 2;
					offs_chpr_sum = offs; /* ChannelProperties are written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x8002/*ExtChannelDiagnosis*/);
					offs += 2;

					/***/

					{
					LSA_UINT32 diag_cnt;
					LSA_UINT16 chpr_sum;

					diag_cnt = 0;
					chpr_sum = CM_CHPR_SPEC_DIS;

					/*
					 * ExtChannelDiagnosisData ::=
					 *   ChannelNumber, ChannelProperties, ChannelErrorType,
					 *   ExtChannelErrorType, ExtChannelAddValue
					 */

					if( cm_is_not_null(ext_real_diag_list) ) {

						CM_SV_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, ext_real_diag_list, CM_SV_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + diag->data_length > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								CM_PUT16_HTON(data, offs, diag->channel_number);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type_usi);
								offs += 2;

								CM_ASSERT(diag->data_length == 2+4);

								CM_MEMCPY(data + offs, (diag + 1), diag->data_length); /* see cm_sv_diag_append() */
								offs += diag->data_length;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					/***/

					if( cm_is_not_null(ext_phys_diag_list) ) {

						CM_PD_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, ext_phys_diag_list, CM_PD_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + 2+4 > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type);
								offs += 2;

								CM_PUT16_HTON(data, offs, diag->ext_channel_error_type);
								offs += 2;
								CM_PUT32_HTON(data, offs, diag->ext_channel_add_value);
								offs += 4;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					/***/

					if( cm_is_not_null(ext_mcons_diag_entry) ) {

						CM_SV_DIAG_PTR_TYPE diag;

						{ /* indentation for symmetry-reasons! */

							diag = ext_mcons_diag_entry;

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + 2+4 > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								CM_PUT16_HTON(data, offs, diag->channel_number);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type_usi);
								offs += 2;

								CM_PUT16_HTON(data, offs, diag->data_length); /* see above! */
								offs += 2;
								CM_PUT32_HTON(data, offs, 0);
								offs += 4;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					/* finally */

					CM_PUT16_HTON(data, offs_chpr_sum, chpr_sum);

					/***/

					if( diag_cnt != 0 ) {

						LSA_UINT32 blklen = offs - (offs_block + (2+2));

						if( blklen > 0xFFFF ) {
							CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
								, "cannot store blklen(%u)", blklen
								);

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11); /* buffer too small (code2?) */
						}

						CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
					}
					else { /* rollback */

						offs = offs_block;
					}
					}
				}
				}

				/*
				 * QUALIFIED CHANNEL DIAGNOSIS
				 */

				{
				CM_LIST_ENTRY_PTR_TYPE qua_diag_list = &iter->subslot->diag_list[CM_SV_DIAG_TYPE_QUALIFIED];

				if( ! CmListIsEmpty(qua_diag_list) ) {

					LSA_UINT32 offs_block;
					LSA_UINT32 offs_chpr_sum;

					/*
					 * QualifiedChannelDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber(0x8000), ChannelProperties a, UserStructureIdentifier(0x8003),
					 *   QualifiedChannelDiagnosisData *
					 *
					 * a The field ChannelProperties.Type, the field ChannelProperties.Direction,
					 *   the field ChannelProperties.Maintenance shall be set to zero.
					 *   The field ChannelProperties.Specifier shall be set to appear if at least
					 *   one ChannelProperties.Specifier in the QualifiedChannelDiagnosisData is set to
					 *   appear in conjunction with ChannelProperties.Maintenance(=diagnosis).
					 *   Else, the field ChannelProperties.Specifier shall be set to disappear.
					 */

					if( offs + 2+2+1+1 + 4+2+2 + 2+2+2 > size ) {

						return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
					}

					offs_block = offs;

					CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
					offs += 2;
					CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
					offs += 1+1;

					CM_PUT32_HTON(data, offs, iter->ap->api);
					offs += 4;
					CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
					offs += 2;
					CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
					offs += 2;

					CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
					offs += 2;
					offs_chpr_sum = offs; /* ChannelProperties are written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x8003/*QualifiedChannelDiagnosis*/);
					offs += 2;

					/***/

					{
					LSA_UINT32 diag_cnt;
					LSA_UINT16 chpr_sum;

					diag_cnt = 0;
					chpr_sum = CM_CHPR_SPEC_DIS;

					/*
					 * QualifiedChannelDiagnosisData ::=
					 *   ChannelNumber, ChannelProperties, ChannelErrorType,
					 *   ExtChannelErrorType, ExtChannelAddValue, QualifiedChannelQualifier
					 */

					if( cm_is_not_null(qua_diag_list) ) { /* always true, for symmetry-reasons! */

						CM_SV_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, qua_diag_list, CM_SV_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + diag->data_length > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								CM_PUT16_HTON(data, offs, diag->channel_number);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type_usi);
								offs += 2;

								CM_ASSERT(diag->data_length == 2+4+4);

								CM_MEMCPY(data + offs, (diag + 1), diag->data_length); /* see cm_sv_diag_append() */
								offs += diag->data_length;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					CM_PUT16_HTON(data, offs_chpr_sum, chpr_sum);

					/***/

					if( diag_cnt != 0 ) {

						LSA_UINT32 blklen = offs - (offs_block + (2+2));

						if( blklen > 0xFFFF ) {
							CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
								, "cannot store blklen(%u)", blklen
								);

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11); /* buffer too small (code2?) */
						}

						CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
					}
					else { /* rollback */

						offs = offs_block;
					}
					}
				}
				}

				/*
				 * GENERIC DIAGNOSIS
				 */

				{
				CM_LIST_ENTRY_PTR_TYPE gen_diag_list = &iter->subslot->diag_list[CM_SV_DIAG_TYPE_GENERIC];

				{ /* indentation for symmetry-reasons! */

					/*
					 * ManufacturerSpecificDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber, ChannelProperties, UserStructureIdentifier,
					 *   Data*
					 */

					if( cm_is_not_null(gen_diag_list) ) { /* always true, for symmetry-reasons! */

						CM_SV_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, gen_diag_list, CM_SV_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_TRUE, diag->channel_properties) ) {

								LSA_UINT32 offs_block;

								if( offs + 2+2+1+1 + 4+2+2 + 2+2+2 + diag->data_length > size ) {

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
								}

								offs_block = offs;

								CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
								offs += 2;
								CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
								offs += 2;
								CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/); /* only V1.1 as of V5.3i1.10 (AP01499644) */
								offs += 1+1;

								CM_PUT32_HTON(data, offs, iter->ap->api);
								offs += 4;
								CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
								offs += 2;
								CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
								offs += 2;

								CM_PUT16_HTON(data, offs, diag->channel_number);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type_usi);
								offs += 2;

								CM_ASSERT(diag->channel_error_type_usi < 0x8000);

								CM_MEMCPY(data + offs, (diag + 1), diag->data_length); /* see cm_sv_diag_append_add() */
								offs += diag->data_length;

								/***/

								{
								LSA_UINT32 blklen = offs - (offs_block + (2+2));

								if( blklen > 0xFFFF ) {
									CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
										, "cannot store blklen(%u)", blklen
										);

									return CM_PNIO_FAULTY_Record__BufferTooSmall(11);  /* buffer too small (code2?) */
								}

								CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
								}
							}
						}
					}
				}
				}
			}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	*offs_ptr = offs; /* update */

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL ARData next ===================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_ARData_next(
	CM_SV_ITERATOR_PTR_TYPE iter,
	LSA_UINT16 cycle_counter,
	LSA_UINT8 data_status,
	LSA_UINT8 transfer_status
) {
	CM_SV_DEVICE_PTR_TYPE dev = iter->dev;
	CM_AR_GRAPH_PTR_TYPE temp_argr; /* not within "for IOAR" loop (Green Hills, warning "bypassing initialization", AP01299574) */
	CM_AR_GRAPH_IO_CR_PTR_TYPE temp_iocr; /* see temp_argr above */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data;
	LSA_UINT32 size;
	LSA_UINT32 offs;
	LSA_UINT32 offs_blklen;
	LSA_UINT32 offs_nr_of_ars;

	/***/

	data = iter->pdev.record_data;
	size = iter->pdev.record_data_length;
	offs = iter->pdev.offs_tot;

	offs_blklen    = CM_RECORD_OFFSET + 2;
	offs_nr_of_ars = CM_RECORD_OFFSET + 2+2+1+1;

	/***/

	if( iter->pdev.state == CM_PD_ITER_PDEV_ARData_Setup ) { /* first call */

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) state(%u) offs/len(%u/%u) first_call"
			, dev->device_nr, cm_is_null(iter->pdev.argr) ? 0 : iter->pdev.argr->ar_nr
			, iter->pdev.state
			, offs, size
			);

		if( offs != CM_RECORD_OFFSET ) {
			CM_FATAL();
		}
	}
	else if( iter->pdev.state == CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done ) { /* confirmation (callback from acp) */

		CM_SV_TRACE_08(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) state(%u) offs/len(%u/%u) cycle_counter(0x%x) data_status(0x%x) transfer_status(0x%x)"
			, dev->device_nr, cm_is_null(iter->pdev.argr) ? 0 : iter->pdev.argr->ar_nr
			, iter->pdev.state
			, offs, size
			, cycle_counter, data_status, transfer_status
			);

		/*lint -e(801) Use of goto is deprecated - request 1502507*/
		goto _confirmation;
	}
	else {

		CM_FATAL();
	}

	/*
	 * ARData V1.0 ::=
	 *   BlockHeader,
	 *   NumberOfARs,
	 *   ( ARUUID, ARType, ARProperties,
	 *     CMInitiatorObjectUUID, StationNameLength, CMInitiatorStationName,
	 *     NumberOfIOCRs,
	 *     ( IOCRType, IOCRProperties, FrameID,
	 *       APDUStatus(=CycleCounter, DataStatus, TransferStatus),
	 *       InitiatorUDPRTPort, ResponderUDPRTPort
	 *     )*,
	 *     AlarmCRType, LocalAlarmReference, RemoteAlarmReference,
	 *     ParameterServerObjectUUID, StationNameLength, ParameterServerStationName,
	 *     NumberOfAPIs,
	 *       API*
	 *   )*
	 *
	 * ARData V1.1 ::=
	 *   BlockHeader,
	 *   NumberOfARs,
	 *   ( ARUUID, CMInitiatorObjectUUID, ParameterServerObjectUUID,
	 *     ARProperties, ARType,
	 *     AlarmCRType, LocalAlarmReference, RemoteAlarmReference,
	 *     InitiatorUDPRTPort, ResponderUDPRTPort,
	 *     StationNameLength, CMInitiatorStationName, [Padding*],
	 *     StationNameLength, [ParameterServerStationName], [Padding*],
	 *     NumberOfIOCRs, [Padding*],
	 *     ( IOCRProperties, IOCRType,
	 *       FrameID, APDU_Status
	 *     )*,
	 *     NumberOfAPIs, [Padding*],
	 *       API*,
	 *     ARDataInfo
	 *   )*
	 *
	 * ARDataInfo ::=
	 *   NumberOfEntries, [Padding*],
	 *   { [IRInfoBlock], [SRInfoBlock],
	 *     [ARVendorBlockReq*], [ARVendorBlockRes*],
	 *     [ARFSUBlock], [SRLData], [RSInfoBlock]
	 *   }
	 *
	 * ad [Padding*]: The number of padding octets shall be adapted to make the following field Unsigned32 aligned.
	 */

	if( offs + 2+2+1+1 + 2 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_AR_DATA);
	offs += 2;
	CM_ASSERT(offs_blklen == offs); /* BlockLength written below */
	offs += 2;
	if( iter->pdev.v1dot1 ) {
		CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/);
	}
	else {
		CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
	}
	offs += 1+1;

	/***/

	{
	LSA_UINT32 pnio_err;

	CM_ASSERT(offs_nr_of_ars == offs);
	CM_PUT16_HTON(data, offs, 0); /* NumberOfARs */
	offs += 2;

	pnio_err = cm_sv_iter_ar_first(iter);

	if( pnio_err != CM_PNIO_ERR_NONE ) { /* always false */

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}
	}

	for(
		; cm_sv_iter_ar_valid(iter)
		; cm_sv_iter_ar_next(iter)
	) {

		temp_argr = iter->argr;

		/* note: the AR may be down, see IOCR handling below */

		if( iter->pdev.v1dot1 ) {

			CM_AR_GRAPH_ALARM_CR_PTR_TYPE alcr = temp_argr->alarm_cr;
			LSA_UINT16 alcr_type;
			LSA_UINT16 local_alarm_ref;
			LSA_UINT16 remote_alarm_ref;

			if( cm_is_null(alcr) ) { /* easy-supervisor */

				alcr_type        = 0;
				local_alarm_ref  = 0;
				remote_alarm_ref = 0;
			}
			else {

				alcr_type        = alcr->alcr_type;
				local_alarm_ref  = alcr->local_alarm_ref;
				remote_alarm_ref = alcr->remote_alarm_ref;
			}

			if( offs + 16+16+16 + 4+2 + 2+2+2 + 2+2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT_UUID_HTON(data, offs, &temp_argr->ar.ar_uuid);
			offs += 16;
			CM_PUT_UUID_HTON(data, offs, &temp_argr->ar.cmi_obj_uuid);
			offs += 16;
			CM_PUT_UUID_HTON(data, offs, &cm_uuid_nil_uuid/*prm_obj_uuid*/);
			offs += 16;

			CM_PUT32_HTON(data, offs, temp_argr->ar.ar_properties);
			offs += 4;
			CM_PUT16_HTON(data, offs, temp_argr->ar.ar_type);
			offs += 2;

			CM_PUT16_HTON(data, offs, alcr_type);
			offs += 2;
			CM_PUT16_HTON(data, offs, local_alarm_ref);
			offs += 2;
			CM_PUT16_HTON(data, offs, remote_alarm_ref);
			offs += 2;

			CM_PUT16_HTON(data, offs, CM_NTOH16(temp_argr->ar.cmi_udp_port));
			offs += 2;
			CM_PUT16_HTON(data, offs, CM_NTOH16(temp_argr->ar.cmr_udp_port));
			offs += 2;

			if( offs + 2+temp_argr->ar.cmi_station_name_length > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, temp_argr->ar.cmi_station_name_length);
			offs += 2;
			CM_MEMCPY(data + offs, temp_argr->ar.cmi_station_name, temp_argr->ar.cmi_station_name_length);
			offs += temp_argr->ar.cmi_station_name_length;

			while( (offs & 3) != 0 ) { /* padding */

				if( offs + 1 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT8_HTON(data, offs, 0);
				offs += 1;
			}

			if( offs + 2 + 0/*prm_server_name_length*/ > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, 0/*prm_server_name_length*/);
			offs += 2;
#if 0 /* PrmServerBlock not yet supported */
			CM_MEMCPY(data + offs, prm_server_name, prm_server_name_length);
			offs += prm_server_name_length;
#endif

			while( (offs & 3) != 0 ) { /* padding */

				if( offs + 1 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT8_HTON(data, offs, 0);
				offs += 1;
			}
		}
		else { /* V1.0 */

			if( offs + 16+2+4 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT_UUID_HTON(data, offs, &temp_argr->ar.ar_uuid);
			offs += 16;

			CM_PUT16_HTON(data, offs, temp_argr->ar.ar_type);
			offs += 2;

			CM_PUT32_HTON(data, offs, temp_argr->ar.ar_properties);
			offs += 4;

			if( offs + 16 + 2+temp_argr->ar.cmi_station_name_length > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT_UUID_HTON(data, offs, &temp_argr->ar.cmi_obj_uuid);
			offs += 16;

			CM_PUT16_HTON(data, offs, temp_argr->ar.cmi_station_name_length);
			offs += 2;
			CM_MEMCPY(data + offs, temp_argr->ar.cmi_station_name, temp_argr->ar.cmi_station_name_length);
			offs += temp_argr->ar.cmi_station_name_length;
		}

		/***/

		{
		LSA_UINT16 number_of_iocrs = 0;

		for( CmListEach(temp_iocr, &temp_argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {
			number_of_iocrs += 1;
		}

		if( iter->pdev.v1dot1 ) {

			if( offs + 2+2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, number_of_iocrs);
			offs += 2;
			CM_PUT16_HTON(data, offs, 0); /* padding */
			offs += 2;
		}
		else { /* V1.0 */

			if( offs + 2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, number_of_iocrs);
			offs += 2;
		}
		}

		for( CmListEach(temp_iocr, &temp_argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			/* AP00351571: APDUStatus shall be read from EDD (V3.11i9) */

			iter->pdev.state = CM_PD_ITER_PDEV_ARData_GetAPDUStatus;
			iter->pdev.offs_tot = offs;
			iter->pdev.iocr = temp_iocr; /* save */

			{
			LSA_UINT16 rc;

			if( cm_ar_com_is_down(temp_argr) ) {

				rc = CM_OK_CANCELLED; /* cannot lock the AR */
			}
			else {

				if( (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

					rc = cm_sv_r1b_stat_A_request(dev, temp_iocr->iocr_ref, &temp_argr->ar.ar_uuid, iter);
				}
				else {

					rc = cm_acp_frame_get_apdu_status(dev->channel, temp_iocr, iter);
				}
			}

			if( rc != CM_OK_PENDING ) {

				iter->pdev.state = CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done;

				cycle_counter   = 0;
				data_status     = 0;
				transfer_status = 0;
			}
			else {

				if( temp_argr->cm.sv.rec.ardata_refcnt >= (CM_CFG_MAX_SV_ARS + 1/*impl.ar*/) ) {

					CM_FATAL(); /* a bug */
				}
				else {

					temp_argr->cm.sv.rec.ardata_refcnt += 1; /* lock the AR, unlock see cm_sv_frame_get_apdu_status_done() */
				}

				return CM_PNIO_ERR_NONE; /* wait for confirmation */
			}
			}

_confirmation: /* confirmation is back from acp */

			temp_argr = iter->argr; /* restore */
			temp_iocr = iter->pdev.iocr; /* restore */

			if(
				iter->pdev.state != CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done
				|| cm_is_null(temp_argr)
				|| cm_is_null(temp_iocr)
			) {

				CM_FATAL();
			}

			/***/

			if( cm_is_not_null(iter->pdev.argr) ) { /* explicit-ar: abort iteration if down */

				if( cm_ar_com_is_down(iter->pdev.argr) ) {

					return CM_PNIO_FAULTY_Record__CMRPC_ARUUIDUnknown();
				}
			}

			/***/

			if( iter->pdev.v1dot1 ) {

				if( offs + 4+2+2 + 2+1+1 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT32_HTON(data, offs, temp_iocr->iocr_properties);
				offs += 4;
				CM_PUT16_HTON(data, offs, temp_iocr->iocr_type);
				offs += 2;
				CM_PUT16_HTON(data, offs, temp_iocr->frame_id);
				offs += 2;

				CM_PUT16_HTON(data, offs, cycle_counter);
				offs += 2;
				CM_PUT8_HTON(data, offs, data_status);
				offs += 1;
				CM_PUT8_HTON(data, offs, transfer_status);
				offs += 1;
			}
			else { /* V1.0 */

				if( offs + 2+4+2 + 2+1+1 + 2+2 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT16_HTON(data, offs, temp_iocr->iocr_type);
				offs += 2;
				CM_PUT32_HTON(data, offs, temp_iocr->iocr_properties);
				offs += 4;
				CM_PUT16_HTON(data, offs, temp_iocr->frame_id);
				offs += 2;

				CM_PUT16_HTON(data, offs, cycle_counter);
				offs += 2;
				CM_PUT8_HTON(data, offs, data_status);
				offs += 1;
				CM_PUT8_HTON(data, offs, transfer_status);
				offs += 1;

				/* history: the udp-port was once in the IOCRBlock, then it was moved
				 *          to the ARBlock, but this coding was not changed...
				 */

				CM_PUT16_HTON(data, offs, CM_NTOH16(temp_argr->ar.cmi_udp_port));
				offs += 2;
				CM_PUT16_HTON(data, offs, CM_NTOH16(temp_argr->ar.cmr_udp_port));
				offs += 2;
			}
		}

		/***/

		if( iter->pdev.v1dot1 ) {

			LSA_UINT16 number_of_apis = 0;
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &temp_argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {
				number_of_apis += 1;
			}

			if( offs + 2+2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, number_of_apis);
			offs += 2;
			CM_PUT16_HTON(data, offs, 0); /* padding */
			offs += 2;

			for( CmListEach(ar_ap, &temp_argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				if( offs + 4 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT32_HTON(data, offs, ar_ap->api);
				offs += 4;
			}

			/* ARDataInfo */

			{
			LSA_UINT16 number_of_entries = 0;
			LSA_VOID_PTR_TYPE ir_ptr = LSA_NULL;
			LSA_VOID_PTR_TYPE sr_ptr = LSA_NULL;
			LSA_VOID_PTR_TYPE fs_ptr = LSA_NULL;
			CM_AR_GRAPH_RS_INFO_PTR_TYPE rs_ptr = LSA_NULL; /* TIA 1651851 */

			if( cm_is_not_null(temp_argr->ir_info) ) {

				ir_ptr = temp_argr->ir_info + 1; /* see cm_sv_build_ir_info() */

				number_of_entries += 1; /* IRInfoBlock */
			}

			if( cm_is_not_null(temp_argr->sr_info) ) {

				sr_ptr = temp_argr->sr_info + 1; /* see cm_sv_build_sr_info() */

				number_of_entries += 1; /* SRInfoBlock */
			}

			{
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

			for( CmListEach(ar_vend, &temp_argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

				if( ar_vend->req_data_len != 0 ) { /* always true */

					number_of_entries += 1; /* ARVendorBlockReq (AP01354956) */
				}

				if( ar_vend->res_data_len != 0 ) { /* not always true, see cm_sv_ar_connect_update() */

					number_of_entries += 1; /* ARVendorBlockRes (AP01354956) */
				}
			}
			}

			if( CM_SV_AR_IS_ADVANCED(temp_argr) && temp_argr->cm.sv.rec.ARFSUDataAdjust_len != 0 ) {

				if( cm_is_not_null(temp_argr->cm.sv.rec.ARFSUDataAdjust_ptr) ) { /* always true */

					fs_ptr = temp_argr->cm.sv.rec.ARFSUDataAdjust_ptr;

					number_of_entries += 1; /* ARFSUBlock */
				}
			}

			if( iter->dev->R1_enabled ) {

				number_of_entries += 1; /* SRLData (present if R-NAP) */
			}

			if( cm_is_not_null(temp_argr->rs_info) ) {

				rs_ptr = temp_argr->rs_info; /* see cm_sv_build_rs_info() */

				number_of_entries += 1; /* RSInfoBlock */
			}

			if( offs + 2+2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, number_of_entries);
			offs += 2;
			CM_PUT16_HTON(data, offs, 0); /* padding */
			offs += 2;

			if( cm_is_not_null(ir_ptr) ) {

				LSA_UINT16 ir_len = 2+2 + CM_GET16_NTOH(ir_ptr, 2);

				if( offs + ir_len > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_MEMCPY(data + offs, ir_ptr, ir_len);
				offs += ir_len;
			}

			if( cm_is_not_null(sr_ptr) ) {

				LSA_UINT16 sr_len = 2+2 + CM_GET16_NTOH(sr_ptr, 2);

				if( offs + sr_len > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_MEMCPY(data + offs, sr_ptr, sr_len);
				offs += sr_len;
			}

			{
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

			for( CmListEach(ar_vend, &temp_argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

				if( ar_vend->req_data_len != 0 ) { /* always true */

					if( offs + ar_vend->req_data_len > size ) {

						return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
					}

					CM_MEMCPY(data + offs, ar_vend->data, ar_vend->req_data_len);
					offs += ar_vend->req_data_len;
				}
			}

			for( CmListEach(ar_vend, &temp_argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

				if( ar_vend->res_data_len != 0 ) { /* not always true, see cm_sv_ar_connect_update() */

					if( offs + ar_vend->res_data_len > size ) {

						return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
					}

					CM_MEMCPY(data + offs, ar_vend->data + ar_vend->req_data_len, ar_vend->res_data_len);
					offs += ar_vend->res_data_len;
				}
			}
			}

			if( cm_is_not_null(fs_ptr) ) {

				LSA_UINT16 fs_len = temp_argr->cm.sv.rec.ARFSUDataAdjust_len;

				if( offs + fs_len > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_MEMCPY(data + offs, fs_ptr, fs_len);
				offs += fs_len;
			}

			if( iter->dev->R1_enabled ) {

				LSA_UINT16 RedundancyInfo;

				/* PNIO-Spec:
				 * SubslotNumber 0x8xxx ... mounted left or up into the rack
				 * SubslotNumber 0x9xxx ... mounted right or down into the rack.
				 * Bit 0: RedundancyInfo.EndPoint1 ... The position of the delivering node in a rack is the left or above one.
				 * Bit 1: RedundancyInfo.EndPoint2 ... The position of the delivering node in a rack is the right or below one.
				 * Bit 2 – 15: RedundancyInfo.reserved
				 */

				if( (iter->dev->val_8i00 & 0xF000) == 0x8000 ) {
					RedundancyInfo = 0x0001;
				}
				else {
					RedundancyInfo = 0x0002;
				}

				if( (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {
					RedundancyInfo ^= 0x0003; /* flip */
				}

				/* SRLData ::= BlockHeader, RedundancyInfo (U16), [Padding*] (none) */

				if( offs + 2+2+1+1 + 2 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_SRL_DATA);
				offs += 2;
				CM_PUT16_HTON(data, offs, 1+1 + 2);
				offs += 2;
				CM_PUT16_HTON(data, offs, 0x0100);
				offs += 1+1;
				CM_PUT16_HTON(data, offs, RedundancyInfo);
				offs += 2;
			}

			if( cm_is_not_null(rs_ptr) ) {

				if( offs + rs_ptr->rs_info_data_len > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_MEMCPY(data + offs, rs_ptr->rs_info_data, rs_ptr->rs_info_data_len);
				offs += rs_ptr->rs_info_data_len;
			}
			}
		}
		else { /* V1.0 */

			{
			CM_AR_GRAPH_ALARM_CR_PTR_TYPE alcr = temp_argr->alarm_cr;
			LSA_UINT16 alcr_type;
			LSA_UINT16 local_alarm_ref;
			LSA_UINT16 remote_alarm_ref;

			if( cm_is_null(alcr) ) { /* easy-supervisor */

				alcr_type        = 0;
				local_alarm_ref  = 0;
				remote_alarm_ref = 0;
			}
			else {

				alcr_type        = alcr->alcr_type;
				local_alarm_ref  = alcr->local_alarm_ref;
				remote_alarm_ref = alcr->remote_alarm_ref;
			}

			if( offs + 2+2+2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, alcr_type);
			offs += 2;

			CM_PUT16_HTON(data, offs, local_alarm_ref);
			offs += 2;

			CM_PUT16_HTON(data, offs, remote_alarm_ref);
			offs += 2;
			}

			/***/

			if( offs + 16 + 2+0/*prm_server_name_length*/ > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT_UUID_HTON(data, offs, &cm_uuid_nil_uuid/*prm_obj_uuid*/);
			offs += 16;

			CM_PUT16_HTON(data, offs, 0/*prm_server_name_length*/);
			offs += 2;
#if 0 /* PrmServerBlock not yet supported */
			CM_MEMCPY(data + offs, prm_server_name, prm_server_name_length);
			offs += prm_server_name_length;
#endif

			/***/

			{
			LSA_UINT16 number_of_apis = 0;
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &temp_argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {
				number_of_apis += 1;
			}

			if( offs + 2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, number_of_apis);
			offs += 2;

			for( CmListEach(ar_ap, &temp_argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				if( offs + 4 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT32_HTON(data, offs, ar_ap->api);
				offs += 4;
			}
			}
		}

		/***/

		{
		LSA_UINT16 nr;
		nr = CM_GET16_NTOH(data, offs_nr_of_ars);
		nr += 1;
		CM_PUT16_HTON(data, offs_nr_of_ars, nr);
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( CM_GET16_NTOH(data, offs_nr_of_ars) == 0 ) {

		offs = CM_RECORD_OFFSET; /* undo (see CM_PD_ITER_PDEV_ARData_Setup) */
	}

	iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* reset */

	/***/

	CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) offs/len(%u/%u) pnio_err(0x%x)"
		, dev->device_nr, cm_is_null(iter->pdev.argr) ? 0 : iter->pdev.argr->ar_nr
		, offs, size
		, CM_PNIO_ERR_NONE
		);

	if( cm_is_null(iter->pdev.argr) ) { /* implicit-ar */

		cm_sv_rpc_mark_as_record_succeeded(dev->implicit_ar.rpc, LSA_TRUE/*read*/, offs);
	}
	else { /* explicit-ar */

		cm_sv_rpc_mark_as_record_succeeded(iter->pdev.argr->cm.sv.curr_rpc_ind, LSA_TRUE/*read*/, offs);
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL ARData (helper: Get-APDUStatus-done) ==========================*/
/*===========================================================================*/

LSA_VOID
cm_sv_frame_get_apdu_status_done(
	LSA_VOID_PTR_TYPE user_id,
	LSA_UINT16 cycle_counter,
	LSA_UINT8 data_status,
	LSA_UINT8 transfer_status
) {
	CM_SV_ITERATOR_PTR_TYPE iter = (CM_SV_ITERATOR_PTR_TYPE)user_id;

	if(
		cm_is_null(iter)
		|| iter->pdev.state != CM_PD_ITER_PDEV_ARData_GetAPDUStatus
	) {

		CM_FATAL(); /* a bug */
	}
	else if(
		cm_is_not_null(iter->dev)
		&& iter->dev->R1_enabled
		&& iter == iter->dev->r1b.stat_B.iter
	) {

		cm_sv_r1b_stat_B_post(iter->dev, cycle_counter, data_status, transfer_status);
	}
	else {

		CM_AR_GRAPH_PTR_TYPE locked_argr = iter->argr; /* note: the locked AR was "not down" on request, but may be "down" here */
		LSA_UINT32 pnio_err;

		iter->pdev.state = CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done;

		pnio_err = cm_sv_record_marshal_ARData_next(iter, cycle_counter, data_status, transfer_status);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_SV_TRACE_05(iter->dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev/ar(%u/%u) offs/len(%u/%u) pnio_err(0x%x)"
				, iter->dev->device_nr, cm_is_null(iter->pdev.argr) ? 0 : iter->pdev.argr->ar_nr
				, iter->pdev.offs_tot, iter->pdev.record_data_length
				, pnio_err
				);

			iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* reset */
		}

		/***/

		if( cm_is_null(iter->pdev.argr) ) { /* implicit-ar */

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				cm_sv_rpc_mark_as_record_failed(iter->dev->implicit_ar.rpc, LSA_TRUE/*read*/, pnio_err, 0, 0);
			}

			/***/

			if( cm_sv_rpc_check_implicit_ar(iter->dev) ) {

				cm_sv_rpc_next_implicit_ar(iter->dev);
			}
		}
		else { /* explicit-ar */

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				cm_sv_rpc_mark_as_record_failed(iter->pdev.argr->cm.sv.curr_rpc_ind, LSA_TRUE/*read*/, pnio_err, 0, 0);
			}

			/***/

			cm_sv_rpc_check_explicit_ar(iter->pdev.argr);
		}

		/***/

		if(
			cm_is_null(locked_argr)
			|| locked_argr->cm.sv.rec.ardata_refcnt < 1
		) {

			CM_FATAL(); /* a bug */
		}
		else {

			locked_argr->cm.sv.rec.ardata_refcnt -= 1;

			if( locked_argr->cm.sv.rec.ardata_refcnt == 0 ) {

				if( cm_ar_com_is_down(locked_argr) ) {

					cm_sv_ar_disconnect_indication_join(locked_argr); /* trigger: ardata_refcnt == 0 */
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== MARSHAL ARData first ==================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_ARData_first(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	LSA_UINT16 flags, /* CM_SV_ITER_REAL_AP or CM_SV_ITER_REAL_DEV */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 const *offs_ptr
) {
	LSA_BOOL v1dot1 = LSA_FALSE;
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_SV_ITERATOR_PTR_TYPE iter;
	LSA_UINT32 pnio_err;

	/***/

	if( req->is_read_implicit ) { /* implicit-ar */

		argr = LSA_NULL; /* running on implicit-ar */

		iter = &dev->implicit_ar.iter;
	}
	else if( cm_is_not_null(req->argr) ) { /* explicit-ar */

		argr = req->argr; /* running on explicit-ar */

		iter = argr->cm.sv.rec.iter_ptr; /* exists, see cm_sv_argr_init() */
	}
	else {

		return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) { /* TIA 1160172 (V6.0i1.5x) obsoletes AP01198903 (which excluded "DAC") */

		CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(temp_argr) ) {

			if( CM_SV_AR_IS_ADVANCED(temp_argr) ) {

				v1dot1 = LSA_TRUE; /* use V1.1 if there are advanced mode ARs (AP01198903) */
				break;
			}
		}
	}
	}

	/***/

	pnio_err = cm_sv_iter_setup(iter, dev, req, flags, 0/*false*/);

	if( pnio_err == CM_PNIO_ERR_NONE ) {

		iter->pdev.state = CM_PD_ITER_PDEV_ARData_Setup;

		iter->pdev.argr = argr; /* LSA_NULL if running on implicit-ar */

		iter->pdev.v1dot1 = v1dot1;

		iter->pdev.record_data = data;

		iter->pdev.record_data_length = size;

		iter->pdev.offs_tot = *offs_ptr;

		iter->pdev.iocr = LSA_NULL; /* sanity */

		pnio_err = cm_sv_record_marshal_ARData_next(iter, 0, 0, 0);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev/ar(%u/%u) offs/len(%u/%u) pnio_err(0x%x)"
				, dev->device_nr, cm_is_null(iter->pdev.argr) ? 0 : iter->pdev.argr->ar_nr
				, iter->pdev.offs_tot, iter->pdev.record_data_length
				, pnio_err
				);

			iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* reset */
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL APIData =======================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_APIData(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 offs_blklen;

	/*
	 * APIData ::=
	 *   BlockHeader,
	 *   NumberOfAPIs, API*
	 */

	if( offs + 2+2+1+1 + 2 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_API_DATA);
	offs += 2;
	offs_blklen = offs; /* BlockLength written below */
	offs += 2;
	CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
	offs += 1+1;

	{
	LSA_UINT32 offs_nr_of_apis = offs;
	LSA_UINT16 api_cnt;

	api_cnt = 0; /* NumberOfAPIs will be written below */
	offs += 2;

	/* note: we do not use the iterator for this because it is too simple */

	{
	CM_SV_AP_PTR_TYPE ap;

	for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

		if( ! CM_SV_AP_IS_PLUGGED(ap) ) {
			continue;
		}

		if( offs + 4 > size ) {

			return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
		}

		CM_PUT32_HTON(data, offs, ap->api);
		offs += 4;

		api_cnt += 1;
	}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( api_cnt != 0 ) {

		CM_PUT16_HTON(data, offs_nr_of_apis, api_cnt);

		*offs_ptr = offs; /* update */
	}
	else {

		/* don't update */
	}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL LogBookData ===================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_LogBookData(
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 offs_blklen;

	/*
	 * LogBookData ::=
	 *   BlockHeader,
	 *   ActualLocalTimeStamp,
	 *   NumberOfLogEntries, (
	 *     LocalTimeStamp, ARUUID, PNIOStatus, EntryDetail
	 *   )*
	 */

	if( !cm_sv_logbook_has_entries(dev) ) { /* TIA 1525219 */

		return CM_PNIO_ERR_NONE;
	}

	if( offs + 2+2+1+1 + 8 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_LOG_DATA);
	offs += 2;
	offs_blklen = offs; /* BlockLength written below */
	offs += 2;
	CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
	offs += 1+1;

	{
	LSA_UINT16 ret_val;
	LSA_UINT64 actual_time_stamp;

	CM_GET_CYCLE_COUNTER(&ret_val, dev->channel->sysptr, &actual_time_stamp);

	if( ret_val != LSA_RET_OK ) {

		actual_time_stamp = 0;
	}

	CM_PUT32_HTON(data, offs, (LSA_UINT32)(actual_time_stamp >> 32));
	offs += 4;
	CM_PUT32_HTON(data, offs, (LSA_UINT32)(actual_time_stamp >> 0));
	offs += 4;
	}

	/***/

	{
	LSA_UINT32 offs_nr_of_entries;
	LSA_UINT32 eff_size;
	LSA_UINT16 nr_of_entries;

	if( offs + 2 + CM_PDU_LOG_ENTRY_SIZE > size ) {

		/*
		 * note: at least one entry has to fit into the buffer
		 *       or else "NumberOfLogEntries" == 0 may be misleading
		 *       if the only the BlockHeader can be marshalled
		 */

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	offs_nr_of_entries = offs; /* NumberOfLogEntries written below */
	offs += 2;

	CM_ASSERT(
		(1+1 + 8 + 2) + CM_PDU_LOG_ENTRY_SIZE * CM_CFG_MAX_SV_LOG_ENTRIES <= 0xFFFF /* see cm_pls.h */
	);

	eff_size = offs - (1+1 + 8 + 2) + 0xFFFF; /* effective size, BlockLength is 16 bit! */

	if( eff_size > size ) {
		eff_size = size;
	}

	nr_of_entries = cm_sv_logbook_marshal(dev, data + offs, eff_size - offs);

	CM_PUT16_HTON(data, offs_nr_of_entries, nr_of_entries); /* NumberOfLogEntries */

	offs += (CM_PDU_LOG_ENTRY_SIZE * nr_of_entries);
	}

	/***/

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	*offs_ptr = offs; /* update */

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL I&M0FilterData ================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_IM0FilterData_block(
	CM_SV_ITERATOR_PTR_TYPE iter,
	LSA_UINT8 im0_flag,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT16 block_type;
	LSA_UINT32 offs_blklen;

	/*
	 * I&M0FilterDataSubmodule | I&M0FilterDataModule | I&M0FilterDataDevice ::=
	 *   BlockHeader,
	 *   NumberOfAPIs, (
	 *     API,
	 *     NumberOfModules, (
	 *       SlotNumber, ModuleIdentNumber,
	 *       NumberOfSubmodules, (
	 *         SubslotNumber, SubmoduleIdentNumber
	 *       )*
	 *     )*
	 *   )*
	 */

	if( offs + 2+2+1+1 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	switch( im0_flag ) {

	case CM_SV_IM0_BITS_SUBMODULE:
		block_type = CM_BLOCK_TYPE_I_M0FilterDataSub;
		break;

	case CM_SV_IM0_BITS_MODULE:
		block_type = CM_BLOCK_TYPE_I_M0FilterDataMod;
		break;

	case CM_SV_IM0_BITS_DEVICE:
		block_type = CM_BLOCK_TYPE_I_M0FilterDataDev;
		break;

	default:
		return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	CM_PUT16_HTON(data, offs, block_type);
	offs += 2;
	offs_blklen = offs; /* BlockLength written below */
	offs += 2;
	CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/); /* note: exp/real-ident has the same coding but has V1.1 */
	offs += 1+1;

	/***/

	{
	LSA_UINT32 offs_nr_of_apis = offs;
	LSA_UINT16 api_cnt;
	LSA_UINT32 pnio_err;

	if( offs + 2 > size ) {

		return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
	}

	api_cnt = 0; /* NumberOfAPIs */
	offs += 2;

	pnio_err = cm_sv_iter_ap_first(iter);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "could not cm_sv_iter_ap_first, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	for(
		; cm_sv_iter_ap_valid(iter)
		; cm_sv_iter_ap_next(iter)
	) {

		LSA_UINT32 offs_api = offs;

		if( ! CM_SV_AP_IS_PLUGGED(iter->ap) ) {
			continue;
		}

		if( offs + 4 + 2 > size ) {

			return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
		}

		CM_PUT32_HTON(data, offs, iter->ap->api);
		offs += 4;

		{
		LSA_UINT32 offs_nr_of_slots = offs;
		LSA_UINT16 mod_cnt;

		mod_cnt = 0; /* NumberOfSlots */
		offs += 2;

		pnio_err = cm_sv_iter_mod_first(iter);

		if( pnio_err != CM_PNIO_ERR_NONE ) {
			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "could not cm_sv_iter_mod_first, pnio_err(0x%x)", pnio_err
				);
			return pnio_err;
		}

		for(
			; cm_sv_iter_mod_valid(iter)
			; cm_sv_iter_mod_next(iter)
		) {

			LSA_UINT32 offs_mod = offs;

			if( ! CM_SV_SLOT_IS_PLUGGED(iter->slot) ) {
				continue;
			}

			if( offs + 2+4 + 2 > size ) {

				return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
			}

			CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
			offs += 2;
			CM_PUT32_HTON(data, offs, iter->slot->mod_ident);
			offs += 4;

			{
			LSA_UINT32 offs_nr_of_subslots = offs;
			LSA_UINT16 sub_cnt;

			sub_cnt = 0; /* NumberOfSubslots */
			offs += 2;

			pnio_err = cm_sv_iter_sub_first(iter);

			if( pnio_err != CM_PNIO_ERR_NONE ) {
				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "could not cm_sv_iter_sub_first, pnio_err(0x%x)", pnio_err
					);
				return pnio_err;
			}

			for(
				; cm_sv_iter_sub_valid(iter)
				; cm_sv_iter_sub_next(iter)
			) {

				if( ! CM_SV_SUBSLOT_IS_PLUGGED(iter->subslot) ) {
					continue;
				}

				if( (iter->subslot->im0_bits & im0_flag) == 0 ) {
					continue; /* skip */
				}

				if( offs + 2+4 > size ) {

					return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
				}

				CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
				offs += 2;
				CM_PUT32_HTON(data, offs, iter->subslot->sub_ident);
				offs += 4;

				/***/

				sub_cnt += 1;
			}

			CM_PUT16_HTON(data, offs_nr_of_subslots, sub_cnt);

			/***/

			if( sub_cnt != 0 ) {

				mod_cnt += 1;
			}
			else {

				offs = offs_mod; /* rollback */
			}
			}
		}

		CM_PUT16_HTON(data, offs_nr_of_slots, mod_cnt);

		/***/

		if( mod_cnt != 0 ) {

			api_cnt += 1;
		}
		else {

			offs = offs_api; /* rollback */
		}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	{
	LSA_UINT32 blklen = offs - (offs_blklen + 2/*BlockLength*/);
	CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);
	}

	/***/

	if( api_cnt != 0 ) {

		CM_PUT16_HTON(data, offs_nr_of_apis, api_cnt);

		*offs_ptr = offs; /* update */
	}
	else {

		/* don't update */
	}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_IM0FilterData_all(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_ITERATOR_TYPE iter;
	LSA_UINT32 pnio_err;

	/*
	 * I&M0FilterData ::=
	 *   { I&M0FilterDataSubmodule a,
	 *     [I&M0FilterDataModule] b,
	 *     [I&M0FilterDataDevice] c
	 *   }
	 *
	 * a Shall contain all submodules with discrete IMData
	 * b Shall, if exists, contain only the module reference
	 * c Shall, if exists, contain only the device reference
	 */

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_REAL_DEV, 0/*false*/);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "could not cm_sv_iter_setup, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	pnio_err = cm_sv_record_marshal_IM0FilterData_block(&iter, CM_SV_IM0_BITS_SUBMODULE, data, size, offs_ptr);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "could not CM_SV_IM0_BITS_SUBMODULE, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_REAL_DEV, 0/*false*/);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_FATAL(); /* a bug, it worked before */
	}

	pnio_err = cm_sv_record_marshal_IM0FilterData_block(&iter, CM_SV_IM0_BITS_MODULE, data, size, offs_ptr);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "could not CM_SV_IM0_BITS_MODULE, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_REAL_DEV, 0/*false*/);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_FATAL(); /* a bug, it worked before */
	}

	pnio_err = cm_sv_record_marshal_IM0FilterData_block(&iter, CM_SV_IM0_BITS_DEVICE, data, size, offs_ptr);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "could not CM_SV_IM0_BITS_DEVICE, pnio_err(0x%x)", pnio_err
			);
		return pnio_err;
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== MARSHAL I&M5 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_IM5( /* TIA 1092255 */
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_SUBSLOT_CONST_PTR_TYPE su,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 pnio_err;

	if(
		cm_is_null(su) /* always false */
		|| (su->im0_bits & CM_SV_IM0_BITS_IM5) == 0
	) {

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* submodule does not provide I&M5 record */
	}
	else {

		LSA_UINT16 written = 0;

		size -= *offs_ptr;
		data += *offs_ptr;

		if( ! cm_pdusr_get_im5_record(dev->channel, size, &written, data) ) {

			pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(0);
		}
		else {

			*offs_ptr += written;

			pnio_err = CM_PNIO_ERR_NONE;
		}
	}

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL 8000 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_8000(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_ITERATOR_TYPE iter;
	LSA_UINT32 pnio_err;

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_SUB, CM_CAST_INT(req->index == 0x8000)); /* lint waring 730  request 1524437 */

	if( pnio_err == CM_PNIO_ERR_NONE ) {

		switch( req->index ) {

		case 0x8000: /* ExpectedIdentificationData */
			pnio_err = cm_sv_record_marshal_expected_ident(&iter, data, size, offs_ptr);
			break;

		case 0x8001: /* RealIdentificationData */
			pnio_err = cm_sv_record_marshal_real_ident(&iter, data, size, offs_ptr);
			break;

		case 0x800A: /* Diagnosis in channel coding */
		case 0x800B: /* Diagnosis in all codings */
		case 0x800C: /* Diagnosis, Maintenance, Qualified and Status */
		case 0x8010: /* Maintenance required in channel coding */
		case 0x8011: /* Maintenance demanded in channel coding */
		case 0x8012: /* Maintenance required in all codings */
		case 0x8013: /* Maintenance demanded in all codings */
			pnio_err = cm_sv_record_marshal_diag(&iter, data, size, offs_ptr, req->index);
			break;

		default:
			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
			break;
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL C000 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_C000(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_ITERATOR_TYPE iter;
	LSA_UINT32 pnio_err;

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_MOD, CM_CAST_INT(req->index == 0xC000)); /* lint waring 730  request 1524437 */

	if( pnio_err == CM_PNIO_ERR_NONE ) {

		switch( req->index ) {

		case 0xC000: /* ExpectedIdentificationData */
			pnio_err = cm_sv_record_marshal_expected_ident(&iter, data, size, offs_ptr);
			break;

		case 0xC001: /* RealIdentificationData */
			pnio_err = cm_sv_record_marshal_real_ident(&iter, data, size, offs_ptr);
			break;

		case 0xC00A: /* Diagnosis in channel coding */
		case 0xC00B: /* Diagnosis in all codings */
		case 0xC00C: /* Diagnosis, Maintenance, Qualified and Status */
		case 0xC010: /* Maintenance required in channel coding */
		case 0xC011: /* Maintenance demanded in channel coding */
		case 0xC012: /* Maintenance required in all codings */
		case 0xC013: /* Maintenance demanded in all codings */
			pnio_err = cm_sv_record_marshal_diag(&iter, data, size, offs_ptr, req->index);
			break;

		default:
			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
			break;
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL E000 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_E000(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_ITERATOR_TYPE iter;
	LSA_UINT32 pnio_err;

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_AR, 1/*true*/);

	if( pnio_err == CM_PNIO_ERR_NONE ) {

		switch( req->index ) {

		case 0xE000: /* ExpectedIdentificationData */
			pnio_err = cm_sv_record_marshal_expected_ident(&iter, data, size, offs_ptr);
			break;

		case 0xE001: /* RealIdentificationData */
			pnio_err = cm_sv_record_marshal_real_ident(&iter, data, size, offs_ptr);
			break;

		case 0xE002: /* ModuleDiffBlock */
			{
			LSA_BOOL ok = cm_sv_marshal_module_diff_block(iter.argr, LSA_NULL, CM_SV_MDB_CONTEXT_READ, data, size, offs_ptr);

			if( ! ok ) {

				pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11); /* 23.01.2006 */
			}
			}
			break;

		case 0xE00A: /* Diagnosis in channel coding */
		case 0xE00B: /* Diagnosis in all codings */
		case 0xE00C: /* Diagnosis, Maintenance, Qualified and Status */
		case 0xE010: /* Maintenance required in channel coding */
		case 0xE011: /* Maintenance demanded in channel coding */
		case 0xE012: /* Maintenance required in all codings */
		case 0xE013: /* Maintenance demanded in all codings */
			pnio_err = cm_sv_record_marshal_diag(&iter, data, size, offs_ptr, req->index);
			break;

		default:
			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
			break;
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL F000 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_F000(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_ITERATOR_TYPE iter;
	LSA_UINT32 pnio_err;

	/***/

	pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_REAL_AP, 0/*false*/);

	if( pnio_err == CM_PNIO_ERR_NONE ) {

		switch( req->index ) {

		case 0xF000: /* RealIdentificationData */
			pnio_err = cm_sv_record_marshal_real_ident(&iter, data, size, offs_ptr);
			break;

		case 0xF00A: /* Diagnosis in channel coding */
		case 0xF00B: /* Diagnosis in all codings */
		case 0xF00C: /* Diagnosis, Maintenance, Qualified and Status */
		case 0xF010: /* Maintenance required in channel coding */
		case 0xF011: /* Maintenance demanded in channel coding */
		case 0xF012: /* Maintenance required in all codings */
		case 0xF013: /* Maintenance demanded in all codings */
			pnio_err = cm_sv_record_marshal_diag(&iter, data, size, offs_ptr, req->index);
			break;

		/* note: 0xF020 ARData handled in caller */

		default:
			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
			break;
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== MARSHAL F800 ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_marshal_F800(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	LSA_UINT32 pnio_err;

	/***/

	switch( req->index ) {

	case 0xF80C: /* Diagnosis, Maintenance, Qualified and Status */
		{
		CM_SV_ITERATOR_TYPE iter;

		pnio_err = cm_sv_iter_setup(&iter, dev, req, CM_SV_ITER_REAL_DEV, 0/*false*/);

		if( pnio_err == CM_PNIO_ERR_NONE ) {

			pnio_err = cm_sv_record_marshal_diag(&iter, data, size, offs_ptr, req->index);
		}
		}
		break;

	/* note: 0xF820 ARData handled in caller */

	case 0xF821: /* APIData */
		pnio_err = cm_sv_record_marshal_APIData(dev, data, size, offs_ptr);
		break;

	case 0xF830: /* LogBookData */
		pnio_err = cm_sv_record_marshal_LogBookData(dev, data, size, offs_ptr);
		break;

	/* note: 0xF831 PDevData handled in caller */

	case 0xF840: /* I&M0FilterData */
		pnio_err = cm_sv_record_marshal_IM0FilterData_all(dev, req, data, size, offs_ptr);
		break;

	/* note: 0xF841 PDRealData handled in caller */
	/* note: 0xF842 PDExpectedData handled in caller */

	case 0xFBFF: /* Trigger index for the RPC connection monitoring */
		pnio_err = CM_PNIO_ERR_NONE; /* return an empty block */
		break;

	default:
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
		break;
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== READ NORMATIVE RECORDS ================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_record_read_normative(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_SV_SUBSLOT_CONST_PTR_TYPE su
) {
	CLRPC_UPPER_MEM_U8_PTR_TYPE data;
	LSA_UINT32 size;
	LSA_UINT32 written;
	LSA_UINT32 pnio_err;
	LSA_BOOL is_iter;

	/***/

	data = rpc->args.sv.call->ndr_data;

	size = req->record_data_length/*!*/;

	if( size > rpc->args.sv.call->alloc_len ) {
		CM_FATAL(); /* a bug */
	}

	written = CM_RECORD_OFFSET; /* starting offset */

	is_iter = LSA_FALSE; /* see "iterators" ARData etc. */

	/***/

	if( (req->cls & CM_REC_IDX_8000) != 0 ) {

		if( req->index == 0xAFF5/*I&M5*/ ) {

			pnio_err = cm_sv_record_marshal_IM5(dev, su, data, size, &written);
		}
		else {

			pnio_err = cm_sv_record_marshal_8000(dev, req, data, size, &written);
		}
	}
	else if( (req->cls & CM_REC_IDX_C000) != 0 ) {

		pnio_err = cm_sv_record_marshal_C000(dev, req, data, size, &written);
	}
	else if( (req->cls & CM_REC_IDX_E000) != 0 ) {

		pnio_err = cm_sv_record_marshal_E000(dev, req, data, size, &written);
	}
	else if( (req->cls & CM_REC_IDX_F000) != 0 ) {

		if( req->index == 0xF020 ) { /* ARData of one API */

			is_iter = LSA_TRUE;
			pnio_err = cm_sv_record_marshal_ARData_first(dev, req, CM_SV_ITER_REAL_AP, data, size, &written);
		}
		else {

			pnio_err = cm_sv_record_marshal_F000(dev, req, data, size, &written);
		}
	}
	else if( (req->cls & CM_REC_IDX_F800) != 0 ) {

		switch( req->index ) {

		case 0xF820: /* ARData of one device */
			is_iter = LSA_TRUE;
			pnio_err = cm_sv_record_marshal_ARData_first(dev, req, CM_SV_ITER_REAL_DEV, data, size, &written);
			break;

		case 0xF831: /* PDevData */
		case 0xF841: /* PDRealData */
		case 0xF842: /* PDExpectedData */
			is_iter = LSA_TRUE;
			pnio_err = cm_sv_record_read_pdev_first(dev, req);
			break;

		default:
			pnio_err = cm_sv_record_marshal_F800(dev, req, data, size, &written);
			break;
		}
	}
	else {

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	if( written < CM_RECORD_OFFSET || written > size ) {

		CM_FATAL(); /* a bug in the marshalling */
	}
	else if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) marshalling index(0x%x) failed, pnio_err(0x%x)"
			, dev->device_nr
			, req->index, pnio_err
			);
	}
	else { /* success (or pending) */

		if( is_iter ) {

			/* iterators do cm_sv_rpc_mark_as_record_succeeded() themselves */
		}
		else {

			cm_sv_rpc_mark_as_record_succeeded(rpc, LSA_TRUE/*read*/, written);
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== PDEV RECORD ITERATOR ==================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_read_pdev_first(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
) {
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_SV_ITERATOR_PTR_TYPE iter;
	LSA_UINT32 pnio_err;

	if( req->is_read_implicit ) { /* implicit-ar */

		argr = LSA_NULL; /* running on implicit-ar */

		iter = &dev->implicit_ar.iter;
	}
	else if( cm_is_not_null(req->argr) ) { /* explicit-ar */

		argr = req->argr; /* running on explicit-ar */

		iter = argr->cm.sv.rec.iter_ptr; /* exists, see cm_sv_argr_init() */
	}
	else {

		return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	pnio_err = cm_sv_iter_setup(iter, dev, req, CM_SV_ITER_REAL_DEV, 0/*false*/);

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		CM_FATAL(); /* a bug */
	}

	iter->pdev.record_data_length = req->record_data_length;
	iter->pdev.offs_tot = 0; /* sanity */

	switch( req->index ) {

	case 0xF831: /* PDevData */
		iter->pdev.state = CM_PD_ITER_PDEV_BlockHeaderPDev;
		break;

	case 0xF841: /* PDRealData */
		iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderReal;
		break;

	case 0xF842: /* PDExpectedData */
		iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderExpected;
		break;

	default:
		return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	pnio_err = cm_sv_record_read_pdev_next(dev, argr, LSA_NULL);

	return pnio_err;
}

/*===========================================================================*/

LSA_UINT32
cm_sv_record_read_pdev_next(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;
	CM_SV_ITERATOR_PTR_TYPE iter;

	if( cm_is_null(argr) ) { /* implicit-ar */

		rpc = dev->implicit_ar.rpc;

		if( cm_is_null(rpc) ) {
			CM_FATAL(); /* a bug */
			return 0;
		}

		iter = &dev->implicit_ar.iter;
	}
	else { /* explicit-ar */

		rpc = argr->cm.sv.curr_rpc_ind;

		if( cm_is_null(rpc) ) {
			CM_FATAL(); /* a bug */
			return 0;
		}

		iter = argr->cm.sv.rec.iter_ptr;

		/***/

		if( cm_ar_com_is_down(argr) ) { /* abort iteration */

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev/ar(%u/%u) already down, aborting iteration"
				, argr->device_nr, argr->ar_nr
				);

			iter->pdev.state = CM_PD_ITER_PDEV_IDLE;

			return CM_PNIO_FAULTY_Record__CMRPC_ARUUIDUnknown();
		}
	}

	CM_SV_TRACE_07(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) state(%u) offs/len(%u/%u) pd(%08X) pd-rsp(%u)"
		, dev->device_nr, cm_is_null(argr) ? 0 : argr->ar_nr
		, iter->pdev.state
		, iter->pdev.offs_tot, iter->pdev.record_data_length
		, pd
		, cm_is_null(pd) ? 0 : CM_RQB_GET_RESPONSE(pd)
		);

	if( cm_is_null(pd) ) { /* first call */

		if( iter->pdev.offs_tot != 0 ) {
			CM_FATAL(); /* a bug, see caller */
		}

		iter->pdev.offs_tot = CM_RECORD_OFFSET;
	}
	else if( iter->pdev.state == CM_PD_ITER_PDEV_PDRealDataInvalidated ) {

		iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderReal;
		iter->pdev.offs_tot = CM_RECORD_OFFSET; /* rollback */
	}
	else if( iter->pdev.state == CM_PD_ITER_PDEV_PDExpectedDataInvalidated ) {

		iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderExpected;
		iter->pdev.offs_tot = CM_RECORD_OFFSET; /* rollback */
	}
	else { /* confirmation (callback from cm-pd) */

		goto _confirmation;
	}

	/***/

	if( cm_sv_iter_ap_first(iter) != CM_PNIO_ERR_NONE ) {
		CM_FATAL(); /* a bug */
	}

	for(
		; cm_sv_iter_ap_valid(iter)
		; cm_sv_iter_ap_next(iter)
	) {

		if( ! CM_SV_AP_IS_PLUGGED(iter->ap) ) {
			continue;
		}

		/***/

		if( cm_sv_iter_mod_first(iter) != CM_PNIO_ERR_NONE ) {
			CM_FATAL(); /* a bug */
		}

		for(
			; cm_sv_iter_mod_valid(iter)
			; cm_sv_iter_mod_next(iter)
		) {

			if( ! CM_SV_SLOT_IS_PLUGGED(iter->slot) ) {
				continue;
			}

			/***/

			if( cm_sv_iter_sub_first(iter) != CM_PNIO_ERR_NONE ) {
				CM_FATAL(); /* a bug */
			}

			for(
				; cm_sv_iter_sub_valid(iter)
				; cm_sv_iter_sub_next(iter)
			) {

				if( ! CM_SV_SUBSLOT_IS_PLUGGED(iter->subslot) ) {
					continue;
				}

				if( ! CM_SV_SUBSLOT_IS_PDEV(iter->subslot->subslot_nr) ) {
					continue;
				}

				if(
					iter->pdev.state == CM_PD_ITER_PDEV_BlockHeaderPDev /* PDevData (0xF831) */
					&& ! CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, iter->subslot->subslot_nr) /* note: only local PDEV (TIA 1240456) */
				) {
					continue;
				}

				/***/

				for(;;) { /* for each block */

					CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "pdev-iter: ap/sl/su(%u/%u/%u) state(%u) offs/len(%u/%u)"
						, iter->ap->api, iter->slot->slot_nr, iter->subslot->subslot_nr
						, iter->pdev.state, iter->pdev.offs_tot, iter->pdev.record_data_length
						);

					{
					LSA_UINT32 pnio_err = CM_PNIO_ERR_NONE; /* assume ok */
					LSA_UINT16 index;

					switch( iter->pdev.state ) {

					/* PDevData (0xF831) */

					case CM_PD_ITER_PDEV_BlockHeaderPDev:

						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = rpc->args.sv.call->ndr_data;
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 > iter->pdev.record_data_length ) {

							CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_PD_PDEVDATA);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* length is written below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					case CM_PD_ITER_PDEV_IRData:
						index = 0x802C;
						break;

					case CM_PD_ITER_PDEV_PDSyncDataClock:
						index = 0x802D;
						break;

					case CM_PD_ITER_PDEV_PDIRSubframeData:
						index = 0x8020;
						break;

					/* Header for PDRealData and PDExpectedData */

					case CM_PD_ITER_PDEV_MultipleBlockHeaderReal:
					case CM_PD_ITER_PDEV_MultipleBlockHeaderExpected:
						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = rpc->args.sv.call->ndr_data;
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 + 4+2+2 > iter->pdev.record_data_length ) {

							CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_MULTIPLE_BLOCK_HEADER);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* patched below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						CM_PUT32_HTON(data, offs, iter->ap->api);
						offs += 4;
						CM_PUT16_HTON(data, offs, iter->slot->slot_nr);
						offs += 2;
						CM_PUT16_HTON(data, offs, iter->subslot->subslot_nr);
						offs += 2;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					/* PDRealData (0xF841) */

					case CM_PD_ITER_PDEV_PDPortDataReal:
						index = 0x802A;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataReal:
						index = 0x8050;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataReal:
						index = 0x8054;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataReal:
						index = 0x8060; /* note: special handling removed with TIA 1337858 (V6.0i2.xx) */
						break;
					case CM_PD_ITER_PDEV_PDInterfaceDataReal:
						index = 0x8080;
						break;
					case CM_PD_ITER_PDEV_PDPortStatistic:
						index = 0x8072; /* note: PDPortStatistic is for the interface too */
						break;
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
					case CM_PD_ITER_PDEV_PDPortMrpIcDataReal:
						index = 0x8057;
						break;
#endif

					/* PDExpectedData (0xF842) */

					case CM_PD_ITER_PDEV_PDPortDataCheck:
						index = 0x802B;
						break;
					case CM_PD_ITER_PDEV_PDPortDataAdjust:
						index = 0x802F;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataAdjust:
						index = 0x8052;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataCheck:
						index = 0x8051;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataAdjust:
						index = 0x8053;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataAdjust:
						index = 0x8062;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataCheck:
						index = 0x8061;
						break;
					case CM_PD_ITER_PDEV_PDNCDataCheck:
						index = 0x8070;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceFSUDataAdjust:
						index = 0x8090;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceAdjust:
						index = 0x8071;
						break;
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
					case CM_PD_ITER_PDEV_PDPortMrpIcDataAdjust:
						index = 0x8055;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpIcDataCheck:
						index = 0x8056;
						break;
#endif

					/***/

					default:
						CM_FATAL(); /* a bug */
						return 0;
					}

					/***/

					if( pnio_err == CM_PNIO_ERR_NONE ) {

						LSA_UINT32 cls = cm_record_index_classify(dev->channel, iter->ap->api, index, CM_VENDOR_ID_DONTCARE);

						/* AP00431325, avoid reads that will fail anyway */

						if( (cls & CM_REC_IDX_READ) == 0 ) {

							goto _next; /* e.g. POF not configured */
						}

						if( (iter->subslot->subslot_nr & 0x00FF) == 0x0000 ) { /* is an interface-submodule, see cm_pdsv_range_ok() */

							if( (cls & CM_REC_IDX_INTERFACE) == 0 ) {

								goto _next;
							}
						}
						else { /* is a port-submodule */

							if( (cls & CM_REC_IDX_PORT) == 0 ) {

								goto _next;
							}
						}

						/***/

						{
						CM_SV_RECORD_PARAMS_TYPE req;
						LSA_UINT16 ar_nr;

						req.is_read = LSA_TRUE;
						req.is_read_implicit = 0; /* don't care */

						req.api        = iter->ap->api;
						req.slot_nr    = iter->slot->slot_nr;
						req.subslot_nr = iter->subslot->subslot_nr;
						req.index      = index;

						req.record_data_length = iter->pdev.record_data_length - iter->pdev.offs_tot + CM_RECORD_OFFSET;

						req.cls  = cls;
						req.argr = argr;

						if( cm_is_null(argr) ) {

							ar_nr = 0;
						}
						else {

							ar_nr = argr->ar_nr;
						}

						pnio_err = cm_sv_pdev_read_write(
							dev
							, &req
							, rpc->args.sv.call->ndr_data + iter->pdev.offs_tot - CM_RECORD_OFFSET /* pass-in the clrpc-buffer! */
							, ar_nr
							);
						}
					}

					if( pnio_err != CM_PNIO_ERR_NONE ) { /* abort iteration */

						iter->pdev.state = CM_PD_ITER_PDEV_IDLE;
					}

					return pnio_err; /* CM_PNIO_ERR_NONE .. wait for confirmation */
					}

_confirmation: /* confirmation is back from cm-pd */

					if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

						CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "reading index(0x%x) from sl/su(%u/0x%x) failed, pd-rsp(%u)"
							, pd->args.pd.prm_rdwr->record_index
							, pd->args.pd.prm_rdwr->slot_nr
							, pd->args.pd.prm_rdwr->subslot_nr
							, CM_RQB_GET_RESPONSE(pd)
							);

						if( CM_RQB_GET_RESPONSE(pd) == CM_ERR_PRM_DATA ) { /* data-length too short */

							CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.record_data_length, iter->pdev.state
								);

							iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* abort iteration */

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
						}
						else {

							/* ignored (e.g. index cannot be read from a port, ...) */
						}
					}
					else {

						iter->pdev.offs_tot += pd->args.pd.prm_rdwr->record_data_length;

						if( iter->pdev.offs_tot > iter->pdev.record_data_length ) {
							CM_FATAL(); /* a bug */
						}
					}

					/***/

_next:
					iter->pdev.state += 1; /* next */

					switch( iter->pdev.state ) {

					case CM_PD_ITER_PDEV_PDevDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									rpc->args.sv.call->ndr_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_BlockHeaderPDev; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDRealDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									rpc->args.sv.call->ndr_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderReal; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDExpectedDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									rpc->args.sv.call->ndr_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderExpected; /* setup for next subslot */
						break;

					default:
						continue; /* read next index */
					}

					break; /* forever-loop */
				}
			}
		}
	}

	/***/

	iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* reset */

	if( cm_is_null(argr) ) { /* implicit-ar */

		cm_sv_rpc_mark_as_record_succeeded(dev->implicit_ar.rpc, LSA_TRUE/*read*/, iter->pdev.offs_tot);
	}
	else { /* explicit-ar */

		cm_sv_rpc_mark_as_record_succeeded(argr->cm.sv.curr_rpc_ind, LSA_TRUE/*read*/, iter->pdev.offs_tot);
	}

	return CM_PNIO_ERR_NONE; /* completed */
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
