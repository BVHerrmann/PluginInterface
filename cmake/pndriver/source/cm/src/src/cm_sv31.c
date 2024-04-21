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
/*  F i l e               &F: cm_sv31.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AP things                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7310
#define CM_MODULE_ID       7310

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== AP LOOKUP =============================================================*/
/*===========================================================================*/

CM_SV_AP_PTR_TYPE
cm_sv_ap_lookup(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 api
) {
	CM_SV_AP_PTR_TYPE ap;

	for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

		if( ap->api == api ) {

			return ap;
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AP COMPARE-FUNCTION ===================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_list_cmp__ap__api(
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
) {
	CM_SV_AP_PTR_TYPE p = (CM_SV_AP_PTR_TYPE)ListEntry;
	CM_SV_AP_PTR_TYPE e = (CM_SV_AP_PTR_TYPE)NewEntry;

	CM_ASSERT(p->api != e->api); /* no duplicates, tested before insertion */

	return p->api < e->api;
}

/*===========================================================================*/
/*=== AP ALLOC ==============================================================*/
/*===========================================================================*/

static CM_SV_AP_PTR_TYPE
cm_sv_ap_alloc(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT32 api
) {
	CM_SV_AP_PTR_TYPE ap = cm_sv_ap_lookup(dev, api);

	if( cm_is_null(ap) ) {

		CM_LOCAL_MEM_PTR_TYPE local_mem;

		CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*ap));
		ap = (CM_SV_AP_PTR_TYPE)local_mem;

		if( cm_is_null(ap) ) {

			CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "cannot alloc local-mem"
				);
		}
		else {

			ap->api = api;

			ap->sm_refcnt = 0;

			ap->ar_refcnt = 0;

			/***/

			CmListInitialize(&ap->slot_list);

			/***/

			CmListInsertSorted(&dev->ap_list, &ap->link, cm_list_cmp__ap__api);
		}
	}

	/***/

	return ap;
}

/*===========================================================================*/
/*=== AP FREE-IF-UNREFERENCED ===============================================*/
/*===========================================================================*/

static CM_SV_AP_PTR_TYPE
cm_sv_ap_free_if_unreferenced(
	CM_SV_AP_PTR_TYPE ap
) {
	CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "ap(%u) sm_refcnt(%u) ar_refcnt(%u)"
		, ap->api, ap->sm_refcnt, ap->ar_refcnt
		);

	if( ap->sm_refcnt == 0 && ap->ar_refcnt == 0 ) {

		CM_SV_AP_PTR_TYPE temp = ap;

		ap = CmListPrevOrHead(&ap->link, CM_SV_AP_PTR_TYPE);
		/* see where called in a loop, CmListNext is the next operation */

		CmListRemoveEntry(&temp->link);

		/***/

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "ap(%u) unreferenced, unlink and free"
			, temp->api
			);

		CM_ASSERT(CmListIsEmpty(&temp->slot_list)); /* see cm_sv_detach() */

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, temp);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}
	}

	CM_ASSERT(cm_is_not_null(ap));

	return ap;
}

/*===========================================================================*/
/*=== AP ATTACH =============================================================*/
/*===========================================================================*/

CM_SV_AP_PTR_TYPE
cm_sv_ap_attach(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT32 api,
	LSA_BOOL from_submodule
) {
	CM_SV_AP_PTR_TYPE ap = cm_sv_ap_alloc(dev, api);

	if( cm_is_not_null(ap) ) {

		if( from_submodule ) {

			if( ap->sm_refcnt >= 0xFFFF ) {
				CM_FATAL(); /* a bug */
			}

			ap->sm_refcnt += 1;

			CM_ASSERT(CM_SV_AP_IS_PLUGGED(ap)); /* consistency check */
		}
		else {

			if( ap->ar_refcnt >= dev->explicit_ar.max_ARs_TOT ) {
				CM_FATAL(); /* a bug */
			}

			ap->ar_refcnt += 1;
		}
	}

	return ap;
}

/*===========================================================================*/
/*=== AP DETACH =============================================================*/
/*===========================================================================*/

CM_SV_AP_PTR_TYPE
cm_sv_ap_detach(
	CM_SV_AP_PTR_TYPE ap,
	LSA_BOOL from_submodule
) {

	if( from_submodule ) {

		if( ap->sm_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		ap->sm_refcnt -= 1;
	}
	else {

		if( ap->ar_refcnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		ap->ar_refcnt -= 1;
	}

	ap = cm_sv_ap_free_if_unreferenced(ap);

	return ap;
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
