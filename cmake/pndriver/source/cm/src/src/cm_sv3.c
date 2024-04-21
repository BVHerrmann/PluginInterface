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
/*  F i l e               &F: cm_sv3.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Real-config things                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7300
#define CM_MODULE_ID       7300

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== SUBMODULE OR DIAG CHANGED TRIGGER =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_submodule_or_diag_changed_trigger(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) ) {

			cm_sv_ar_problem_indicator_eval(argr); /* trigger: callers of cm_sv_dev_submodule_or_diag_changed_trigger() */

			cm_sv_ar_submodule_state_action(argr); /* trigger: callers of cm_sv_dev_submodule_or_diag_changed_trigger() */
		}
	}

	/***/

	cm_sv_dev_led_info_eval(dev); /* trigger: callers of cm_sv_dev_submodule_or_diag_changed_trigger() */
}

/*===========================================================================*/
/*=== SUBMODULE ADD ALL ELEMENTS ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_submodule_add(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_SUBMODULE_ADD_PTR_TYPE param = rb->args.sv.submod_add;
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
	else if( param->nr_of_elems == 0 ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) nr_of_elems(%u) is zero"
			, dev->device_nr, param->nr_of_elems
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 idx;

		rsp = CM_OK;

		for( idx = 0; idx < param->nr_of_elems; ++idx ) {

			CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_submodule_add_elem(dev, elem, LSA_TRUE);

			if( elem->response != CM_OK ) {

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) elem[%u] rsp(%u) is not CM_OK"
					, dev->device_nr, idx, elem->response
					);

				rsp = CM_ERR_ELEM;
			}
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( cm_is_not_null(dev) ) {

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* submodule-add (note: if is pdev, it may have already some diagnoses) */
	}
}

/*===========================================================================*/
/*=== SUBMODULE REMOVE ALL ELEMENTS =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_submodule_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_SUBMODULE_REMOVE_PTR_TYPE param = rb->args.sv.submod_remove;
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
	else if( param->nr_of_elems == 0 ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) nr_of_elems(%u) is zero"
			, dev->device_nr, param->nr_of_elems
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 idx;

		rsp = CM_OK;

		for( idx = 0; idx < param->nr_of_elems; ++idx ) {

			CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_submodule_remove_elem(dev, elem, LSA_TRUE);

			if( elem->response != CM_OK ) {

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) elem[%u] rsp(%u) is not CM_OK"
					, dev->device_nr, idx, elem->response
					);

				rsp = CM_ERR_ELEM;
			}
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( cm_is_not_null(dev) ) {

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* submodule-remove (implies: diag-delete-all) */
	}
}

/*===========================================================================*/
/*=== PDEV Control Submodule ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_pull_submodule(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {

	if( channel->state != CM_SV_CREATED ) {

		/* too early or too late */
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_get_added(channel, channel->usr.sv.pdev_device_nr);

		if( cm_is_null(dev) ) {

			/* too early or too late */
		}
		else {

			CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE elem;
			LSA_UINT16 rsp;

			elem.api        =  0; /* PDEV: api is 0 */
			elem.slot_nr    = slot_nr;
			elem.subslot_nr = subslot_nr;

			rsp = cm_sv_submodule_remove_elem(dev, &elem, LSA_FALSE);

			if(rsp != CM_OK) {

				CM_FATAL(); /* a bug */
			}
			else {

				cm_sv_dev_submodule_or_diag_changed_trigger(dev);
			}
		}
	}
}

/*===========================================================================*/

LSA_UINT16
cm_sv_pdev_plug_submodule(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 mod_ident,
	LSA_UINT32 sub_ident,
	LSA_UINT8  im0_bits
) {
	LSA_UINT16 rsp;

	if( channel->state != CM_SV_CREATED ) {

		rsp = CM_OK; /* too early or too late */
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_get_added(channel, channel->usr.sv.pdev_device_nr);

		if( cm_is_null(dev) ) {

			rsp = CM_OK; /* too early or too late */
		}
		else {

			CM_SV_SUBMODULE_ADD_ELEMENT_TYPE elem;

			elem.api        = 0; /* PDEV: api is 0 */
			elem.slot_nr    = slot_nr;
			elem.subslot_nr = subslot_nr;
			elem.mod_ident  = mod_ident;
			elem.sub_ident  = sub_ident;
			elem.im0_bits   = im0_bits;

			rsp = cm_sv_submodule_add_elem(dev, &elem, LSA_FALSE);

			if(rsp == CM_OK) {

				cm_sv_dev_submodule_or_diag_changed_trigger(dev);
			}
		}
	}

	return rsp;
}

LSA_VOID
cm_sv_pdev_set_im0_bits(
	CM_CHANNEL_CONST_PTR_TYPE channel, /* the sv-channel */
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT8  im0_bits
) {
	if( channel->state != CM_SV_CREATED ) {

		/* too early or too late */
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_get_added(channel, channel->usr.sv.pdev_device_nr);

		if( cm_is_null(dev) ) {

			/* too early or too late */
		}
		else {

			CM_SV_AP_PTR_TYPE ap;
			CM_SV_SLOT_PTR_TYPE sl;
			CM_SV_SUBSLOT_PTR_TYPE su;

			ap = cm_sv_ap_lookup(dev, 0);

			sl = cm_is_null(ap) ? LSA_NULL : cm_sv_slot_lookup(ap, slot_nr);

			su = cm_is_null(sl) ? LSA_NULL : cm_sv_subslot_lookup(sl, subslot_nr);

			if(cm_is_null(su)) {

				CM_FATAL(); /* a bug */
			}
			else {

				CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "dev(%u) ap/sl/su(%u/%u/%u) im0_bits(0x%x/0x%x)"
					, dev->device_nr
					, 0, slot_nr, subslot_nr
					, su->im0_bits, im0_bits
					);

				su->im0_bits = im0_bits;
			}
		}
	}
}

/*===========================================================================*/
/*=== DIAG ADD ALL ELEMENTS =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_diag_add(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DIAG_ADD_PTR_TYPE param = rb->args.sv.diag_add;
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
	else if( dev->R1_enabled ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) not supported for R1"
			, dev->device_nr
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( param->nr_of_elems == 0 ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) nr_of_elems(%u) is zero"
			, dev->device_nr, param->nr_of_elems
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 idx;

		rsp = CM_OK;

		for( idx = 0; idx < param->nr_of_elems; ++idx ) {

			CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_diag_add_elem(dev, elem);

			if( ! (elem->response == CM_OK || elem->response == CM_OK_REPLACED) ) {

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) elem[%u] rsp(%u) is not CM_OK or CM_OK_REPLACED"
					, dev->device_nr, idx, elem->response
					);

				rsp = CM_ERR_ELEM;
			}
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( cm_is_not_null(dev) ) {

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* diag-add */
	}
}

/*===========================================================================*/
/*=== DIAG REMOVE ALL ELEMENTS ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_diag_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DIAG_REMOVE_PTR_TYPE param = rb->args.sv.diag_remove;
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
	else if( param->nr_of_elems == 0 ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) nr_of_elems(%u) is zero"
			, dev->device_nr, param->nr_of_elems
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 idx;

		rsp = CM_OK;

		for( idx = 0; idx < param->nr_of_elems; ++idx ) {

			CM_UPPER_SV_DIAG_REMOVE_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_diag_remove_elem(dev, elem);

			if( elem->response != CM_OK ) {

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) elem[%u] rsp(%u) is not CM_OK"
					, dev->device_nr, idx, elem->response
					);

				rsp = CM_ERR_ELEM;
			}
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( cm_is_not_null(dev) ) {

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* diag-remove */
	}
}

/*===========================================================================*/
/*=== DIAG CHANGE ALL ELEMENTS ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_diag_change( /* AP01467031 */
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_add
) {
	CM_UPPER_SV_DIAG_ADD_PTR_TYPE param = rb->args.sv.diag_add; /* note: remove2 uses the "diag_add" too (only the keys) */
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
	else if( param->nr_of_elems == 0 ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) nr_of_elems(%u) is zero"
			, dev->device_nr, param->nr_of_elems
			);

		rsp = CM_ERR_PARAM;
	}
	else {

		LSA_UINT16 idx;

		rsp = CM_OK;

		for( idx = 0; idx < param->nr_of_elems; ++idx ) {

			CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_diag_change_elem(dev, elem, is_add, LSA_TRUE /*is_user*/);

			if( is_add ) {

				if( ! (elem->response == CM_OK || elem->response == CM_OK_REPLACED) ) {

					CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) ADD elem[%u] rsp(%u) is not CM_OK or CM_OK_REPLACED"
						, dev->device_nr, idx, elem->response
						);

					rsp = CM_ERR_ELEM;
				}
			}
			else { /* remove */

				if( elem->response != CM_OK ) {

					CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) RMV elem[%u] rsp(%u) is not CM_OK"
						, dev->device_nr, idx, elem->response
						);

					rsp = CM_ERR_ELEM;
				}
			}
		}

		if( dev->R1_enabled
			&& dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
		) {

			CM_RQB_SET_RESPONSE(rb, rsp);

			cm_sv_r1b_diag_post(dev, rb);

			rsp = CM_OK_PENDING;
		}
	}

	/***/

	if( rsp != CM_OK_PENDING ) {

		cm_callback(channel, rsp, &rb); /* call back first, then trigger */
	}

	if( cm_is_not_null(dev) ) {

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* diag-change */
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
