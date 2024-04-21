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
/*  F i l e               &F: cm_sv32.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Slot things                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7320
#define CM_MODULE_ID       7320

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== SLOT RANGE-CHECK ======================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_slot_range_ok(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 slot_nr
) {

	if( slot_nr <= dev->max_slot_nr ) {

		return LSA_TRUE; /* ok */
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== SLOT LOOKUP ===========================================================*/
/*===========================================================================*/

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_lookup(
	CM_SV_AP_CONST_PTR_TYPE ap,
	LSA_UINT16 slot_nr
) {
	CM_SV_SLOT_PTR_TYPE sl;

	for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

		if( sl->slot_nr == slot_nr ) {

			return sl;
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== SLOT COMPARE-FUNCTION =================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_list_cmp__slot__slot_nr(
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
) {
	CM_SV_SLOT_PTR_TYPE p = (CM_SV_SLOT_PTR_TYPE)ListEntry;
	CM_SV_SLOT_PTR_TYPE e = (CM_SV_SLOT_PTR_TYPE)NewEntry;

	CM_ASSERT(p->slot_nr != e->slot_nr); /* no duplicates, tested before insertion */

	return p->slot_nr < e->slot_nr;
}

/*===========================================================================*/
/*=== SLOT ALLOC ============================================================*/
/*===========================================================================*/

static CM_SV_SLOT_PTR_TYPE
cm_sv_slot_alloc(
	CM_SV_AP_PTR_TYPE ap,
	LSA_UINT16 slot_nr
) {
	CM_SV_SLOT_PTR_TYPE sl = cm_sv_slot_lookup(ap, slot_nr);

	if( cm_is_null(sl) ) {

		CM_LOCAL_MEM_PTR_TYPE local_mem;

		CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*sl));
		sl = (CM_SV_SLOT_PTR_TYPE)local_mem;

		if( cm_is_null(sl) ) {

			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "cannot alloc local-mem"
				);
		}
		else {

			sl->slot_nr = slot_nr;

			sl->ar_refcnt = 0;

			sl->sm_refcnt = 0;

			/***/

			sl->mod_ident = 0; /* sanity */

			CmListInitialize(&sl->subslot_list);

			/***/

			CmListInsertSorted(&ap->slot_list, &sl->link, cm_list_cmp__slot__slot_nr);
		}
	}

	/***/

	return sl;
}

/*===========================================================================*/
/*=== SLOT FREE-IF-UNREFERENCED =============================================*/
/*===========================================================================*/

static CM_SV_SLOT_PTR_TYPE
cm_sv_slot_free_if_unreferenced(
	CM_SV_SLOT_PTR_TYPE sl
) {
	CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "slot(%u) sm_refcnt(%u) ar_refcnt(%u)"
		, sl->slot_nr, sl->sm_refcnt, sl->ar_refcnt
		);

	if( sl->sm_refcnt == 0 && sl->ar_refcnt == 0 ) {

		CM_SV_SLOT_PTR_TYPE temp = sl;

		sl = CmListPrevOrHead(&sl->link, CM_SV_SLOT_PTR_TYPE);
		/* see where called in a loop, CmListNext is the next operation */

		CmListRemoveEntry(&temp->link);

		/***/

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "slot(%u) unreferenced, unlink and free"
			, temp->slot_nr
			);

		CM_ASSERT(CmListIsEmpty(&temp->subslot_list)); /* see cm_sv_detach() */

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, temp);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}
	}

	CM_ASSERT(cm_is_not_null(sl));

	return sl;
}

/*===========================================================================*/
/*=== SLOT ATTACH ===========================================================*/
/*===========================================================================*/

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_attach(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_AP_PTR_TYPE ap,
	LSA_UINT16 slot_nr,
	LSA_BOOL from_submodule
) {
	CM_SV_SLOT_PTR_TYPE sl = cm_sv_slot_alloc(ap, slot_nr);

	if( cm_is_not_null(sl) ) {

		if( from_submodule ) {

			if( sl->sm_refcnt >= 0xFFFF ) {
				CM_FATAL(); /* a bug */
			}

			sl->sm_refcnt += 1;

			CM_ASSERT(CM_SV_SLOT_IS_PLUGGED(sl)); /* consistency check */
		}
		else {

			if( sl->ar_refcnt >= dev->explicit_ar.max_ARs_TOT ) {
				CM_FATAL(); /* a bug */
			}

			sl->ar_refcnt += 1;
		}
	}

	return sl;
}

/*===========================================================================*/
/*=== SLOT DETACH ===========================================================*/
/*===========================================================================*/

CM_SV_SLOT_PTR_TYPE
cm_sv_slot_detach(
	CM_SV_SLOT_PTR_TYPE sl,
	LSA_BOOL from_submodule
) {

	if( from_submodule ) {

		if( sl->sm_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		sl->sm_refcnt -= 1;
	}
	else {

		if( sl->ar_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		sl->ar_refcnt -= 1;
	}

	sl = cm_sv_slot_free_if_unreferenced(sl);

	return sl;
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
