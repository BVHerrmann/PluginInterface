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
/*  F i l e               &F: cm_sv45.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AR things                                                     */
/*                                                                           */
/*  - data-status control                                                    */
/*  - problem-indicator state-machine                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7450
#define CM_MODULE_ID       7450

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== AR CONTROL ============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_control(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_CONTROL_PTR_TYPE param = rb->args.sv.ar_control;
	LSA_UINT16 acp_mode = 0;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_EXP_USER_REQ);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( cm_is_not_null(argr->cm.sv.curr_ar_ctrl) ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) another request still running, opc(%u)"
			, argr->device_nr, argr->ar_nr
			, CM_RQB_GET_OPCODE(argr->cm.sv.curr_ar_ctrl)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( (param->cmd & ~CM_SV_AR_CONTROL_CMD_MASK) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) invalid cmd(0x%04x)"
			, argr->device_nr, argr->ar_nr
			, param->cmd
			);

		rsp = CM_ERR_PARAM;
	}
	else if( cm_ar_com_is_down(argr) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) already down"
			, argr->device_nr, argr->ar_nr
			);

		rsp = CM_OK_CANCELLED; /* late, do nothing because the providers are being stopped */
	}
	else {

		/* note: STATION_OK/FAILURE is controlled by CM, see cm_sv_ar_problem_indicator_mirror() */

		/* note: PRIMARY/BACKUP is controlled by the user, see system redundancy (SRD) */

		/* note: RUN/STOP can be controlled only globally (since V5.1) */

		/*
		// acp_mode = ...
		// argr->cm.sv.curr_ar_ctrl = rb; // pending
		// rsp = CM_OK_PENDING;
		*/

		rsp = CM_ERR_PARAM;
	}

	/***/

	if( rsp != CM_OK_PENDING || cm_is_null(argr) ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) acp_mode(0x%04x)"
			, argr->device_nr, argr->ar_nr
			, acp_mode
			);

		cm_arcq_request(argr, CM_ARCQ_OPC_PROV_DATASTATUS_SET, acp_mode);
	}
}

/*===========================================================================*/
/*=== SET DATA-STATUS DONE ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_data_status_set_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( cm_is_null(argr->cm.sv.curr_ar_ctrl) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb = argr->cm.sv.curr_ar_ctrl;
		argr->cm.sv.curr_ar_ctrl = LSA_NULL;

		cm_callback(argr->ar_com.channel, CM_OK, &rb);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: curr_ar_ctrl == LSA_NULL (set DataStatus done) */
		}
	}
}

/*===========================================================================*/
/*=== PROBLEM-INDICATOR CONTROL THINGS ======================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_problem_indicator_yes(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_UINT16 sysred_ar_nr = 0;

	if( cm_is_not_null(argr->cm.sv.arset) ) { /* AP01214172: owner if any AR of the AR-set is owner */

		CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

#if CM_SV_ARSET_MAX_ARS != 2
#error "check this!"
#endif

		if( arset->arr[0] == argr ) { /* other AR is [1] */
			if( cm_is_not_null(arset->arr[1]) ) {
				sysred_ar_nr = arset->arr[1]->ar_nr;
			}
		}
		else if( arset->arr[1] == argr ) { /* other AR is [0] */
			if( cm_is_not_null(arset->arr[0]) ) {
				sysred_ar_nr = arset->arr[0]->ar_nr;
			}
		}
		else {
			/* unreachable */
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

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				if(
					su->owner_ar_nr == argr->ar_nr
					|| (sysred_ar_nr != 0 && su->owner_ar_nr == sysred_ar_nr)
				) {

					if( cm_sv_diag_maint_qual(argr->cm.sv.dev, ar_ap->sv.real_ap, ar_mod->sv.real_sl, su, LSA_NULL, LSA_NULL, LSA_NULL) ) {

						CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) has diag-appearing"
							, argr->device_nr, argr->ar_nr
							, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
							);

						return LSA_TRUE;
					}
				}
			}
		}
	}
	}

	/***/

#if CM_CFG_SV_AR_HAS_MCONS
	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		if( ar_iocr->cm.is_aux ) { /* necessary? aux-iocrs are unicast */
			continue;
		}

		if( ar_iocr->cm.is_consumer && ar_iocr->cm.is_multicast ) {

			LSA_UINT16 ext_err;

			ext_err = cm_sv_mcons_diag_value(argr, ar_iocr, LSA_NULL, LSA_NULL, LSA_NULL);

			if( ext_err != 0 ) { /* has (appearing) diagnosis */

				CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "dev/ar(%u/%u) has mcons(%08X) with ext_err(%u)"
					, argr->device_nr, argr->ar_nr
					, ar_iocr, ext_err
					);

				return LSA_TRUE;
			}
		}
	}
	}
#endif

	/***/

	return LSA_FALSE;
}

/*===========================================================================*/

LSA_VOID
cm_sv_ar_problem_indicator_mirror(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_request
) {
	/*
	 * mirror the state of the problem-indicator
	 * to all provider-crs of the ar.
	 */

	CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) is_req(%u) pend(%u)  actual(%u) target(%u)"
		, argr->device_nr, argr->ar_nr
		, is_request, argr->cm.sv.pi.pi_pending
		, argr->cm.sv.pi.pi_actual, argr->cm.sv.pi.pi_target
		);

	/***/

	if( ! is_request ) { /* confirmation */

		if( ! argr->cm.sv.pi.pi_pending ) {
			CM_FATAL(); /* a bug */
		}

		argr->cm.sv.pi.pi_pending = LSA_FALSE;
	}

	/***/

	if( cm_ar_com_is_down(argr) ) {

		if( is_request ) {

			/* do nothing */
		}
		else {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: pi_pending == LSA_FALSE */
		}
	}
	else if( argr->cm.sv.pi.pi_pending ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) pi-update is running"
			, argr->device_nr, argr->ar_nr
			);
	}
	else if( argr->cm.sv.pi.pi_actual == argr->cm.sv.pi.pi_target ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) no pi-diff"
			, argr->device_nr, argr->ar_nr
			);
	}
	else { /* actual not target */

		LSA_UINT16 acp_mode;

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) PI-DIFF actual(%u) target(%u)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.pi.pi_actual, argr->cm.sv.pi.pi_target
			);

		argr->cm.sv.pi.pi_actual  = argr->cm.sv.pi.pi_target; /* update */
		argr->cm.sv.pi.pi_pending = LSA_TRUE; /* mirroring running */

		if( argr->cm.sv.pi.pi_actual ) {
			acp_mode = CM_ARCQ_PROVIDER_SET_STATION_FAILURE;
		}
		else {
			acp_mode = CM_ARCQ_PROVIDER_SET_STATION_OK;
		}

		cm_arcq_request(argr, CM_ARCQ_OPC_PROV_DATASTATUS_SET, acp_mode);
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_ar_problem_indicator_eval(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) enabled(%u)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.pi.is_enabled
		);

	if( argr->cm.sv.pi.is_enabled ) {

		/* "down" is handled below */

		argr->cm.sv.pi.pi_target = cm_sv_ar_problem_indicator_yes(argr);

		cm_sv_ar_problem_indicator_mirror(argr, LSA_TRUE/*req*/);
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
