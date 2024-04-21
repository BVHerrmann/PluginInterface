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
/*  F i l e               &F: cm_sv22.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Device things                                                 */
/*                                                                           */
/*  - device remove                                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7220
#define CM_MODULE_ID       7220

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_unregister_if_obj(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
); /* forward */

static LSA_VOID
cm_sv_rpc_unbind(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
); /* forward */

static LSA_VOID
cm_sv_dev_finalize(
	CM_SV_DEVICE_PTR_TYPE dev
); /* forward */

/*===========================================================================*/
/*=== DEVICE REMOVE =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_remove(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DEVICE_REMOVE_PTR_TYPE param = rb->args.sv.dev_remove;
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
	else if( cm_is_not_null(dev->curr) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) another request still running, opc(%u)"
			, dev->device_nr, CM_RQB_GET_OPCODE(dev->curr)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( dev->R1_enabled && ! (dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_UNKNOWN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) R1 bridge_state(%u) still established"
			, dev->device_nr, dev->r1b.bridge_state
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u)"
			, dev->device_nr
			);

		rsp = CM_OK;
	}

	/***/

	if( rsp != CM_OK || cm_is_null(dev) ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		CM_ASSERT(dev->state == CM_SV_DEV_STATE_ADDED);

		/***/

		if( dev->R1_enabled && cm_is_not_null(dev->r1b.control.bridge_broken_rqb) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) R1 bridge-control(broken) is in progress"
				, dev->device_nr
				);
		}

		/***/

		dev->is_active = LSA_FALSE;

		dev->is_parking = LSA_FALSE;

		/* cancel led-info before aborting the ARs */

		cm_sv_dev_led_info_cancel(dev);

		/* abort the ARs before removing the submodules */

		cm_sv_ar_abort_all(dev, CM_AR_REASON_RMV); /* note: may call cm_sv_dev_done() */

		/* remove all subslots, slots and APs (they are referenced by a "submodule" or an AR or both) */

		{
		CM_SV_AP_PTR_TYPE ap;

		for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

			CM_SV_SLOT_PTR_TYPE sl;

			for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su;

				for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

					su->sm_refcnt = 1; /* tricky: see detach */

					su = cm_sv_subslot_detach(su, LSA_TRUE);
				}

				sl->sm_refcnt = 1; /* tricky, see detach */

				sl = cm_sv_slot_detach(sl, LSA_TRUE);
			}

			ap->sm_refcnt = 1; /* tricky, see detach */

			ap = cm_sv_ap_detach(ap, LSA_TRUE);
		}
		}

		/***/

		dev->curr = rb; /* pending, set after possible calls to cm_sv_dev_done() */

		dev->state = CM_SV_DEV_STATE_REMOVING;

		cm_sv_dev_done(dev); /* trigger: remove device */
	}
}

/*===========================================================================*/
/*=== DEVICE DONE ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_done(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) dev-state(%u) expl(%u) impl(%08X) curr-opc(%u) bridge_broken_rqb(%08x)"
		, dev->device_nr
		, dev->state, dev->explicit_ar.ar_cnt, dev->implicit_ar.rpc
		, cm_is_null(dev->curr) ? 0 : CM_RQB_GET_OPCODE(dev->curr)
		, dev->r1b.control.bridge_broken_rqb
		);

	if( CM_SV_DEV_IS_ACTIVE(dev) ) {

		/* still active */
	}
	else if( dev->explicit_ar.ar_cnt != 0 ) {

		/* not all explicit ARs gone */
	}
	else if( cm_is_not_null(dev->implicit_ar.rpc) ) {

		/* not all implicit ARs gone */
	}
	else if( dev->R1_enabled && cm_is_not_null(dev->r1b.control.bridge_broken_rqb) ) {

		/* R1 bridge-control(broken) in progress */
	}
	else if( cm_is_null(dev->curr) ) {

		/* nothing pending */
	}
	else {

		if( CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

			cm_pdsv_occupant_set_reset(dev->channel, LSA_FALSE); /* TIA 213234 */
		}

		switch( CM_RQB_GET_OPCODE(dev->curr) ) {

		case CM_OPC_SV_DEVICE_CONTROL:
			cm_callback(dev->channel, CM_OK, &dev->curr);
			break;

		case CM_OPC_SV_DEVICE_REMOVE:
			CM_ASSERT(dev->state == CM_SV_DEV_STATE_REMOVING);
			cm_sv_rpc_unregister_if_obj(dev, dev->res.rpc_rqb);
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}
}

/*===========================================================================*/
/*=== RPC UNREGISTER ========================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_unregister_if_obj(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-unregister server_id(%u)"
		, dev->device_nr
		, dev->res.rpc_server_id
		);

	rpc->args.sv.unreg_if_obj->server_id = dev->res.rpc_server_id;

	cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_SV_UNREGISTER_IF_OBJ, dev->device_nr, 0);
}

/*===========================================================================*/
/*=== RPC UNREGISTER DONE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_unregister_if_obj_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-rsp(%u)"
		, dev->device_nr, CM_RPC_GET_RESPONSE(rpc)
		);

	if(
		rpc != dev->res.rpc_rqb
		|| CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
	) {

		CM_FATAL(); /* could register, so why not unregister? */
	}
	else {

		dev->res.rpc_server_id = CLRPC_INVALID_SERVER_ID; /* sanity */

		if( dev->explicit_ar.max_ARs_IOX != 0 ) {

			cm_sv_rpc_unbind(dev, 1, rpc);
		}
		else { /* TIA 1063199, TIA 1063197 (V5.4) */

			cm_sv_dev_finalize(dev);
		}
	}
}

/*===========================================================================*/
/*=== UNBIND ================================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_unbind(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-unbind(%u/%u)"
		, dev->device_nr, ar_nr, dev->explicit_ar.max_ARs_IOX
		);

	if( ! (ar_nr >= 1 && ar_nr <= dev->explicit_ar.max_ARs_IOX) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		rpc->args.cl.unbind->client_id = dev->res.clid_arr[ar_nr]; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

		dev->res.clid_arr[ar_nr] = CM_RPC_INVALID_CLIENT_ID; /* sanity */

		cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_CL_UNBIND, dev->device_nr, ar_nr);
	}
}

/*===========================================================================*/
/*=== UNBIND DONE ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_unbind_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-rsp(%u)"
		, dev->device_nr, CM_RPC_GET_RESPONSE(rpc)
		);

	if(
		rpc != dev->res.rpc_rqb
		|| CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
	) {

		CM_FATAL(); /* could bind, so why not unbind? */
	}
	else {

		if( ar_nr < dev->explicit_ar.max_ARs_IOX ) { /* see cm_sv_rpc_bind_done() */

			ar_nr += 1;

			cm_sv_rpc_unbind(dev, ar_nr, rpc);
		}
		else {

			cm_sv_dev_finalize(dev);
		}
	}
}

/*===========================================================================*/
/*=== DEVICE FINALIZE =======================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_dev_finalize(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel; /* save */

	CM_UPPER_RQB_PTR_TYPE rb = dev->curr; /* save */
	dev->curr = LSA_NULL;

	CM_ASSERT(dev->state == CM_SV_DEV_STATE_REMOVING);

	cm_sv_dev_res_cancel(dev);

	cm_sv_dev_unlink_and_free(dev);

	cm_callback(channel, CM_OK, &rb);
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
