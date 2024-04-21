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
/*  F i l e               &F: cm_sv4.c                                  :F&  */
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
/*  - sv-argr alloc                                                          */
/*  - sv-argr free                                                           */
/*  - rpc call-indication and opnum-dispatching                              */
/*  - rpc call-response                                                      */
/*  - rpc call-request                                                       */
/*  - rpc call-confirmation                                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7400
#define CM_MODULE_ID       7400

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_argr_init(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_DEVICE_PTR_TYPE dev
); /* forward */

static LSA_VOID
cm_sv_argr_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_ar_cmi_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
); /* forward */

/*===========================================================================*/
/*=== ALLOC/FREE UPPER MEM ==================================================*/
/*===========================================================================*/

CM_UPPER_MEM_PTR_TYPE
cm_sv_upper_mem_alloc(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT32 length
) {
	CM_UPPER_MEM_PTR_TYPE mem;

	CM_ALLOC_UPPER_MEM(&mem, cm_null_user_id, length, channel->sysptr);

	if (cm_is_null (mem)) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper-mem");
		return LSA_NULL;
	}

	return mem;
}

/*===========================================================================*/

LSA_VOID
cm_sv_upper_mem_free(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_MEM_PTR_TYPE upper
) {
	LSA_UINT16  rc;

	CM_FREE_UPPER_MEM(&rc, upper, channel->sysptr);

	if(rc != LSA_RET_OK) {

		CM_FATAL();
	}
}

/*===========================================================================*/
/*=== ALLOC AND INIT AN AR-GRAPH ============================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_argr_alloc_and_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_AR_GRAPH_PTR_TYPE argr = cm_ar_graph_alloc(dev->channel, dev->device_nr, 0/*!*/);

	if( cm_is_not_null(argr) ) {

		argr->ar.ar_uuid = cm_uuid_nil_uuid; /* preset for cm_sv_logbook_write() in cm_sv_ar_free() */

		cm_sv_argr_init(argr, dev);
	}

	return argr;
}

/*===========================================================================*/
/*=== UNDO-INIT AND FREE AN AR-GRAPH ========================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_argr_undo_init_and_free(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 pdev_send_clock = 0;

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PD_ATTACHED) != 0 ) {

		cm_pdusr_detach(argr->ar_com.channel);

		/***/

		{
		LSA_BOOL is_rr_fixed;

		if( cm_pdusr_is_send_clock_fixed(argr->ar_com.channel, &is_rr_fixed) == 0 ) { /* not fixed now */

			LSA_UNUSED_ARG(is_rr_fixed);

			pdev_send_clock = argr->ar_com.ar_send_clock;
		}
		}
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_QF_OWNERSHIP) != 0 ) {

		cm_sv_ownership_relinquish(argr);
	}

	/***/

	cm_sv_ar_detach(argr);

	/***/

	cm_sv_argr_undo_init(argr);

	cm_arcq_free_ar_graph(argr);

	/***/

	return pdev_send_clock;
}

/*===========================================================================*/
/*=== INIT SERVER-SPECIFIC PART OF AR-GRAPH =================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_argr_init(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_DEVICE_PTR_TYPE dev
) {

	argr->cm.sv.dev = dev;

	argr->cm.sv.arset = LSA_NULL;

	argr->cm.sv.ar_flags = CM_SV_AR_FL_CONNECT_PHASE;

	argr->cm.sv.cn_flags = 0;

	argr->cm.sv.pd_flags = 0;

	argr->cm.sv.pr_appl_ready_cnt = 0;

	argr->cm.sv.plug_group_alarm = LSA_NULL;

	argr->cm.sv.nr_of_submodules = 0;

	argr->cm.sv.nr_of_pdev_submodules = 0;

	argr->cm.sv.nr_of_remote_pdev_submodules = 0;

	argr->cm.sv.local_pdev_if = LSA_NULL;

	argr->cm.sv.remote_pdev_if = LSA_NULL;

	argr->cm.sv.provider_sequence = 0;

	argr->cm.sv.sysred_IOCARSR_backup = LSA_FALSE;

	argr->cm.sv.net_data_length = 0;

	/***/

	argr->cm.sv.curr_rpc_ind = LSA_NULL;

	argr->cm.sv.curr_rpc_ind_is_parked = LSA_FALSE; /* TIA 1518690 */

	argr->cm.sv.curr_ar_ctrl = LSA_NULL;

	argr->cm.sv.curr_ar_pdevprm = LSA_NULL;

	argr->cm.sv.curr_arset_trigger = LSA_NULL;

	/***/

	cm_oneshot_init(&argr->cm.sv.cmi_timer, cm_sv_ar_cmi_timeout, argr);

	/***/

	argr->cm.sv.pi.is_enabled = LSA_FALSE;
	argr->cm.sv.pi.pi_target  = LSA_FALSE; /* default */
	argr->cm.sv.pi.pi_actual  = LSA_FALSE; /* no diff */
	argr->cm.sv.pi.pi_pending = LSA_FALSE;

	/***/

	argr->cm.sv.rec.seq_number_exp = 0;
	argr->cm.sv.rec.seq_number_pre = 0; /* was 0xFFFF up to V4.2i1.4 */

	argr->cm.sv.rec.iter_ptr = LSA_NULL;

	argr->cm.sv.rec.ardata_refcnt = 0;

	cm_sv_ARFSUDataAdjust_init(argr);

	argr->cm.sv.rec.is_rerun = LSA_FALSE;

	/***/

	cm_sv_ar_almi_init(argr);

	cm_sv_ar_almr_init(argr);

	/***/

	argr->cm.sv.lock_rqb = LSA_NULL;

	argr->cm.sv.rebind_rqb = LSA_NULL;

	argr->cm.sv.ar_connect_mem = LSA_NULL;

	argr->cm.sv.ar_ownership_mem = LSA_NULL;

	argr->cm.sv.ar_prm_end_mem = LSA_NULL;

	argr->cm.sv.ar_in_data_mem = LSA_NULL;

	argr->cm.sv.ar_rir_mem = LSA_NULL;

	argr->cm.sv.pdevprm_rdwr_rqb = LSA_NULL;

	argr->cm.sv.pdev_begin_rqb = LSA_NULL;

	argr->cm.sv.pdev_end_rqb = LSA_NULL;

	argr->cm.sv.pdev_rel_rqb = LSA_NULL;

	argr->cm.sv.applrdy_rqb = LSA_NULL;

	argr->cm.sv.plug_group_alarm_rqb = LSA_NULL;

	argr->cm.sv.rdy4rtc3_rqb = LSA_NULL;

	/***/

	cm_sv_ar_r1b_init(argr);
}

/*===========================================================================*/
/*=== PRE-ALLOCATE ==========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_argr_prealloc(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

	/***/

	{
	CM_LOCAL_MEM_PTR_TYPE local_mem;
	CM_SV_ITERATOR_PTR_TYPE iter_ptr;

	CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*iter_ptr));
	iter_ptr = (CM_SV_ITERATOR_PTR_TYPE)local_mem;

	if( cm_is_null(iter_ptr) ) {

		return LSA_FALSE;
	}

	iter_ptr->pdev.state = CM_PD_ITER_PDEV_IDLE; /* sic! using the CM-PD enumeration */

	argr->cm.sv.rec.iter_ptr = iter_ptr;
	}

	/***/

	if( ! cm_sv_ar_almi_alloc(argr) ) {

		return LSA_FALSE;
	}

	if( ! cm_sv_ar_almr_alloc(argr) ) {

		return LSA_FALSE;
	}

	/***/

	{
	CM_OHA_LOWER_RQB_PTR_TYPE oha;

	oha = cm_oha_alloc_rqb(channel);

	if( cm_is_null(oha) ) {

		return LSA_FALSE;
	}
	else {

		CM_OHA_LOWER_MEM_PTR_TYPE lower_mem;
		OHA_UPPER_ADDR_INFO_PTR_TYPE pAddrInfo;

		CM_OHA_ALLOC_LOWER_MEM(&lower_mem, cm_null_user_id, sizeof(*pAddrInfo), channel->sysptr);
		pAddrInfo = (OHA_UPPER_ADDR_INFO_PTR_TYPE)lower_mem;

		if( cm_is_null(pAddrInfo) ) {

			cm_oha_free_rqb(channel, oha);

			return LSA_FALSE;
		}
		else {

			pAddrInfo->NoS.pName = LSA_NULL; /* sanity */

			oha->args.DcpControl.pAddrInfo = pAddrInfo;

			CM_OHA_SET_OPCODE(oha, 0/*unused*/);

			argr->cm.sv.lock_rqb = oha;
		}
	}
	}

	/***/

	argr->cm.sv.ar_connect_mem = cm_sv_ar_connect_alloc(argr);

	if( cm_is_null(argr->cm.sv.ar_connect_mem) ) {

		return LSA_FALSE;
	}

	/***/

	if( argr->cm.sv.nr_of_submodules != 0 ) { /* i.e., not easy-supervisor */

		{
		CM_RPC_LOWER_RQB_PTR_TYPE rpc;

		rpc = cm_rpc_alloc_rqb(channel);

		if( cm_is_null(rpc) ) {

			return LSA_FALSE;
		}

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

		argr->cm.sv.rebind_rqb = rpc;
		}

		/***/

		argr->cm.sv.ar_ownership_mem = cm_sv_ar_ownership_alloc(argr);

		if( cm_is_null(argr->cm.sv.ar_ownership_mem) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.ar_prm_end_mem = cm_sv_ar_prm_end_alloc(argr);

		if( cm_is_null(argr->cm.sv.ar_prm_end_mem) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.ar_in_data_mem = cm_sv_ar_in_data_alloc(argr);

		if( cm_is_null(argr->cm.sv.ar_in_data_mem) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.ar_rir_mem = cm_sv_ar_rir_alloc(argr);

		if( cm_is_null(argr->cm.sv.ar_rir_mem) ) {

			return LSA_FALSE;
		}

		/***/

		if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

			CM_UPPER_RQB_PTR_TYPE pd;

			if (argr->cm.sv.dev->PDevPrmInd_enabled) {

				pd = cm_upper_alloc_rqb(channel);

				if( cm_is_null(pd) ) {

					return LSA_FALSE;
				}

				CM_RQB_SET_OPCODE(pd, 0/*unused*/);

				argr->cm.sv.pdevprm_rdwr_rqb = pd;
			}

			/***/

			pd = cm_upper_alloc_rqb(channel);

			if( cm_is_null(pd) ) {

				return LSA_FALSE;
			}

			CM_RQB_SET_OPCODE(pd, 0/*unused*/);

			argr->cm.sv.pdev_begin_rqb = pd;

			/***/

			pd = cm_upper_alloc_rqb(channel);

			if( cm_is_null(pd) ) {

				return LSA_FALSE;
			}

			CM_RQB_SET_OPCODE(pd, 0/*unused*/);

			argr->cm.sv.pdev_end_rqb = pd;

			/***/

			pd = cm_upper_alloc_rqb(channel);

			if( cm_is_null(pd) ) {

				return LSA_FALSE;
			}

			CM_RQB_SET_OPCODE(pd, 0/*unused*/);

			argr->cm.sv.pdev_rel_rqb = pd;
		}

		/***/

		if( CM_SV_AR_IS_RTCLASS3(argr) && ! CM_SV_AR_IS_ADVANCED(argr) ) {

			LSA_UINT16 len = CLRPC_PNIO_OFFSET + CM_BLK_CTRL_SIZE;
			CM_RPC_LOWER_RQB_PTR_TYPE rpc;

			rpc = cm_rpc_alloc_call_rqb(channel, len);

			if( cm_is_null(rpc) ) {

				return LSA_FALSE;
			}

			CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

			argr->cm.sv.rdy4rtc3_rqb = rpc;
		}

		/***/

		{
		LSA_UINT32 ndr_length = CLRPC_PNIO_OFFSET + CM_BLK_CTRL_SIZE;
		LSA_UINT32 mdb_length = CM_RSP_MDB_SIZE;
		CM_RPC_LOWER_RQB_PTR_TYPE rpc;

		{
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

		for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			mdb_length += CM_RSP_API_SIZE;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				mdb_length += CM_RSP_MOD_SIZE;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					mdb_length += CM_RSP_SUB_SIZE;
				}
			}
		}
		}

		ndr_length += mdb_length;

		if( ndr_length > 0xFFFF ) { /* always false (see cast below, AP01382164) */

			rpc = LSA_NULL;
		}
		else {

			rpc = cm_rpc_alloc_call_rqb(channel, (LSA_UINT16)ndr_length);
		}

		if( cm_is_null(rpc) ) {

			return LSA_FALSE;
		}

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

		argr->cm.sv.applrdy_rqb = rpc;
		}

		/***/

		argr->cm.sv.plug_group_alarm_rqb = cm_upper_alloc_rqb(channel);

		if( cm_is_null(argr->cm.sv.plug_group_alarm_rqb) ) {

			return LSA_FALSE;
		}

		/***/

		if( (argr->cm.sv.r1_flags != 0) ) {

			if( !cm_sv_ar_r1b_prealloc(argr) ) {

				return LSA_FALSE;
			}
		}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== UNDO-INIT SERVER-SPECIFIC PART OF AR-GRAPH ============================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_argr_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

	/***/

	CM_ASSERT(cm_is_null(argr->cm.sv.plug_group_alarm));

	/***/

	argr->cm.sv.local_pdev_if = LSA_NULL;

	argr->cm.sv.remote_pdev_if = LSA_NULL;

	/***/

	CM_ASSERT(cm_is_null(argr->cm.sv.curr_rpc_ind));

	CM_ASSERT(cm_is_null(argr->cm.sv.curr_ar_ctrl));

	CM_ASSERT(cm_is_null(argr->cm.sv.curr_ar_pdevprm));

	CM_ASSERT(cm_is_null(argr->cm.sv.curr_arset_trigger));

	/***/

	CM_ASSERT( ! cm_oneshot_is_running(&argr->cm.sv.cmi_timer));

	/***/

	CM_ASSERT( ! argr->cm.sv.pi.pi_pending);

	/***/

	if( cm_is_not_null(argr->cm.sv.rec.iter_ptr) ) {

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, argr->cm.sv.rec.iter_ptr);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}

		argr->cm.sv.rec.iter_ptr = LSA_NULL;
	}

	CM_ASSERT(argr->cm.sv.rec.ardata_refcnt == 0);

	cm_sv_ARFSUDataAdjust_remove(argr);

	/***/

	cm_sv_ar_almi_undo_init(argr);

	cm_sv_ar_almr_undo_init(argr);

	/***/

	if( cm_is_not_null(argr->cm.sv.lock_rqb) ) {

		CM_OHA_LOWER_RQB_PTR_TYPE oha = argr->cm.sv.lock_rqb;
		argr->cm.sv.lock_rqb = LSA_NULL;

		CM_ASSERT(CM_OHA_GET_OPCODE(oha) == 0/*unused*/);

		if( cm_is_not_null(oha->args.DcpControl.pAddrInfo) ) {

			CM_OHA_LOWER_MEM_PTR_TYPE mem = oha->args.DcpControl.pAddrInfo;
			oha->args.DcpControl.pAddrInfo = LSA_NULL;

			{
			LSA_UINT16 ret_val;
			CM_OHA_FREE_LOWER_MEM(&ret_val, mem, channel->sysptr);
			LSA_UNUSED_ARG(ret_val);
			}
		}

		cm_oha_free_rqb(channel, oha);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.rebind_rqb) ) {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.rebind_rqb;
		argr->cm.sv.rebind_rqb = LSA_NULL;

		CM_ASSERT(CM_RPC_GET_OPCODE(rpc) == 0/*unused*/);

		cm_rpc_free_rqb(channel, rpc);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.ar_connect_mem) ) {

		cm_sv_upper_mem_free(channel, argr->cm.sv.ar_connect_mem);
		argr->cm.sv.ar_connect_mem = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.ar_ownership_mem) ) {

		cm_sv_upper_mem_free(channel, argr->cm.sv.ar_ownership_mem);
		argr->cm.sv.ar_ownership_mem = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.ar_prm_end_mem) ) {

		cm_sv_upper_mem_free(channel, argr->cm.sv.ar_prm_end_mem);
		argr->cm.sv.ar_prm_end_mem = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.ar_in_data_mem) ) {

		cm_sv_upper_mem_free(channel, argr->cm.sv.ar_in_data_mem);
		argr->cm.sv.ar_in_data_mem = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.ar_rir_mem) ) {

		cm_sv_upper_mem_free(channel, argr->cm.sv.ar_rir_mem);
		argr->cm.sv.ar_rir_mem = LSA_NULL;
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.pdevprm_rdwr_rqb) ) {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdevprm_rdwr_rqb;
		argr->cm.sv.pdevprm_rdwr_rqb = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(pd) == 0/*unused*/);

		cm_upper_free_rqb(channel, pd);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.pdev_begin_rqb) ) {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_begin_rqb;
		argr->cm.sv.pdev_begin_rqb = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(pd) == 0/*unused*/);

		cm_upper_free_rqb(channel, pd);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.pdev_end_rqb) ) {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_end_rqb;
		argr->cm.sv.pdev_end_rqb = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(pd) == 0/*unused*/);

		cm_upper_free_rqb(channel, pd);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.pdev_rel_rqb) ) {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_rel_rqb;
		argr->cm.sv.pdev_rel_rqb = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(pd) == 0/*unused*/);

		cm_upper_free_rqb(channel, pd);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.rdy4rtc3_rqb) ) {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.rdy4rtc3_rqb;
		argr->cm.sv.rdy4rtc3_rqb = LSA_NULL;

		CM_ASSERT(CM_RPC_GET_OPCODE(rpc) == 0/*unused*/);

		CM_RPC_SET_OPCODE(rpc, CLRPC_OPC_CL_CALL); /* see cm_rpc_free_call_rqb() */

		cm_rpc_free_call_rqb(channel, rpc);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.applrdy_rqb) ) {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.applrdy_rqb;
		argr->cm.sv.applrdy_rqb = LSA_NULL;

		CM_ASSERT(CM_RPC_GET_OPCODE(rpc) == 0/*unused*/);

		CM_RPC_SET_OPCODE(rpc, CLRPC_OPC_CL_CALL); /* see cm_rpc_free_call_rqb() */

		cm_rpc_free_call_rqb(channel, rpc);
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.plug_group_alarm_rqb) ) {

		CM_UPPER_RQB_PTR_TYPE alarm = argr->cm.sv.plug_group_alarm_rqb;
		argr->cm.sv.plug_group_alarm_rqb = LSA_NULL;

		cm_upper_free_rqb(channel, alarm);
	}

	/***/

	cm_sv_ar_r1b_undo_init(argr);

}

/*===========================================================================*/
/*=== CMI TIMER =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_cmi_timer_trigger(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	cm_oneshot_trigger(&argr->cm.sv.cmi_timer, argr->ar.cmi_timeout100ms * 100);

	CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cmi-timeout triggered"
		);
}

/*===========================================================================*/

LSA_VOID
cm_sv_ar_cmi_timer_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	cm_oneshot_stop(&argr->cm.sv.cmi_timer);

	CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cmi-timeout stopped"
		);
}

/*===========================================================================*/

static LSA_VOID
cm_sv_ar_cmi_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
) {
	CM_AR_GRAPH_PTR_TYPE argr = (CM_AR_GRAPH_PTR_TYPE)user_ptr;

	CM_ASSERT(argr == CM_CONTAINING_STRUCT(oneshot, CM_AR_GRAPH_PTR_TYPE, cm.sv.cmi_timer));
	LSA_UNUSED_ARG(oneshot);

	CM_SV_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "dev(%u) ar(%u) cmi-timeout", argr->device_nr, argr->ar_nr);
	{
		LSA_UINT32  data1 = argr->device_nr;
		data1 <<= 16;
		data1 |= argr->ar_nr;
		cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_TIMER, data1);
	}
	cm_ar_com_down(argr, CM_AR_REASON_TIMER);
}

/*===========================================================================*/
/*=== CM-SV-ARCQ REQUEST DONE ===============================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_arcq_request_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 arcq_opcode,
	LSA_UINT16 arcq_para1
) {
	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) arcq_opcode(%u), cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, arcq_opcode
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
		);

	switch( arcq_opcode ) {

	case CM_ARCQ_OPC_FRAMES_ADD:
		cm_sv_ar_frames_add_done(argr);
		break;

	case CM_ARCQ_OPC_ALARM_ADD:
		cm_sv_ar_alarm_add_done(argr);
		break;

	case CM_ARCQ_OPC_PROV_ACTIVATE:
		cm_sv_ar_prov_activate_done(argr);
		break;

	case CM_ARCQ_OPC_PROV_ACTIVATE_AUX:
		cm_sv_ar_prov_activate_aux_done(argr);
		break;

	case CM_ARCQ_OPC_PROV_IOPARAMS_SET:
		cm_sv_ar_prov_ioparams_set_done(argr);
		break;

	case CM_ARCQ_OPC_CONS_ACTIVATE:
	case CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK:
		cm_sv_ar_cons_activate_done(argr);
		break;

	case CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS:
		cm_sv_ar_set2unk_enpras_done(argr);
		break;

	case CM_ARCQ_OPC_CONS_SET2UNK:
		cm_sv_ar_almi_set2unk_done(argr);
		break;

	case CM_ARCQ_OPC_PROV_PASSIVATE_AUX:
		cm_sv_ar_prov_passivate_aux_done(argr);
		break;

	case CM_ARCQ_OPC_FRAMES_PASSIVATE:
		cm_sv_ar_frames_passivate_done(argr);
		break;

	case CM_ARCQ_OPC_ALARM_REMOVE:
		cm_sv_ar_alarm_remove_done(argr);
		break;

	case CM_ARCQ_OPC_FRAMES_REMOVE:
		cm_sv_frames_remove_done(argr);
		argr = LSA_NULL; /* the ar-graph is gone! */
		break;

	case CM_ARCQ_OPC_PROV_DATASTATUS_SET: /* note: setting apdu-status is ok or fatal, see cm_acp_frame_control_done() */
		{
		LSA_UINT16 acp_mode = arcq_para1;

		if( (acp_mode & (CM_ARCQ_PROVIDER_SET_STATION_OK | CM_ARCQ_PROVIDER_SET_STATION_FAILURE)) != 0 ) {
			cm_sv_ar_problem_indicator_mirror(argr, LSA_FALSE/*cnf*/);
		}
		else {
			cm_sv_ar_data_status_set_done(argr);
		}
		}
		break;

	default:
		CM_FATAL1(arcq_opcode);
		return LSA_NULL;
	}

	return argr;
}

/*===========================================================================*/
/*=== AR IS PDEV-OWNER ======================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_is_pdev_owner(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( argr->ar_nr == argr->cm.sv.dev->pdev_owner_ar_nr ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== RPC MARK AS PENDING ===================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_mark_as_pending(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	/* reusing the field "response" as pending-flag */

	CM_RPC_SET_RESPONSE(rpc, LSA_RSP_UNUSED/*pending*/);
}

/*===========================================================================*/
/*=== RPC MARK AS RECORD-MALFORMED ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_mark_as_record_malformed(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_UINT32 pnio_err
) {

	rpc->args.sv.call->ndr_len = CLRPC_PNIO_OFFSET;

	rpc->args.sv.call->pnio_status = pnio_err;

	CM_RPC_SET_RESPONSE(rpc, CLRPC_OK);
}

/*===========================================================================*/
/*=== RPC MARK AS RECORD-FAILED =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_mark_as_record_failed(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
) {
	/*
	 * NOTE: must marshal a block even if "pnio-error" because of ret_val_1 and ret_val_2
	 */

	if( CM_RPC_GET_RESPONSE(rpc) != LSA_RSP_UNUSED/*pending*/ ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_TRACE_05(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "rpc(%08X) opnum(%u) failed, pnio_err(0x%x) v1(%u) v2(%u)"
			, rpc, rpc->args.sv.call->opnum
			, pnio_err, ret_val_1, ret_val_2
			);

		/***/

		cm_sv_record_update_header(
			rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET,
			is_read,
			CM_RECORD_OFFSET,
			pnio_err, ret_val_1, ret_val_2
			);

		/***/

		rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET;

		rpc->args.sv.call->pnio_status = pnio_err;

		CM_RPC_SET_RESPONSE(rpc, CLRPC_OK);
	}
}

/*===========================================================================*/
/*=== RPC MARK AS RECORD-SUCCEEDED ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_mark_as_record_succeeded(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length
) {
	if( CM_RPC_GET_RESPONSE(rpc) != LSA_RSP_UNUSED/*pending*/ ) {

		CM_FATAL(); /* a bug */
	}
	else if(
		ndr_length < CM_RECORD_OFFSET /* see callers */
		|| ndr_length > rpc->args.sv.call->alloc_len
	) {

		CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
			, "not CM_RECORD_OFFSET(%u) <= ndr_length(%u) <= alloc_len(%u)"
			, CM_RECORD_OFFSET
			, ndr_length
			, rpc->args.sv.call->alloc_len
			);

		CM_FATAL(); /* not as documented (possibly a buffer-overflow) */
	}
	else {

		CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "rpc(%08X) opnum(%u) succeeded, ndr_length(%u)"
			, rpc, rpc->args.sv.call->opnum
			, ndr_length
			);

		if( is_read ) {
			/* ok, all values allowed */
		}
		else if( ndr_length == CM_RECORD_OFFSET ) {
			/* ok, body of a write-rsp is empty */
		}
		else { /* body of a write-multiple.rsp contains an "array" of record-headers */
			CM_ASSERT(((ndr_length - CM_RECORD_OFFSET) % CM_RECORD_HEADER_SIZE) == 0);
		}

		/***/

		cm_sv_record_update_header(
			rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET,
			is_read,
			ndr_length,
			CM_PNIO_ERR_NONE, 0, 0
			);

		/***/

		rpc->args.sv.call->ndr_len = ndr_length;

		rpc->args.sv.call->pnio_status = CM_PNIO_ERR_NONE;

		CM_RPC_SET_RESPONSE(rpc, CLRPC_OK);
	}
}

/*===========================================================================*/
/*=== READ-IMPLICIT =========================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_read_implicit_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	/* note that the number of parallel reads "via implicit-ar" is limited in CLRPC */

	CmListInsertTail(&dev->implicit_ar.queue, (CM_LIST_ENTRY_PTR_TYPE)rpc);

	if( cm_is_not_null(dev->implicit_ar.rpc) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "another read-implicit(%08X) is already running, queueing rpc(%08X)"
			, dev->implicit_ar.rpc
			, rpc
			);
	}
	else {

		cm_sv_rpc_next_implicit_ar(dev);
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_rpc_next_implicit_ar(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( cm_is_not_null(dev->implicit_ar.rpc) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		while( ! CmListIsEmpty(&dev->implicit_ar.queue) ) {

			CM_RPC_LOWER_RQB_PTR_TYPE rpc;

			CmListRemoveHead(&dev->implicit_ar.queue, rpc, CM_RPC_LOWER_RQB_PTR_TYPE);

			dev->implicit_ar.rpc = rpc; /* pending */

			/*
			 * note: device can be "not active" here (passivated/removed while calls in queue)
			 *
			 * special handling for faster shutdown seems not to be useful because
			 * - queue is short (CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID)
			 * - cm-internal calls don't take long
			 * - user did passivate/remove and thus will "shortcut" the calls
			 * - would produce difficult test-cases
			 */

			cm_sv_rpc_record_indication(dev, rpc, LSA_TRUE/*read*/, LSA_TRUE/*implicit*/);

			if( cm_sv_rpc_check_implicit_ar(dev) ) {

				if( cm_is_not_null(dev->implicit_ar.rpc) ) {
					CM_FATAL(); /* a bug */
				}
			}
			else {

				return; /* still pending */
			}
		}

		/***/

		cm_sv_dev_done(dev); /* trigger: all implicit ARs gone */
	}
}

/*===========================================================================*/

LSA_BOOL
cm_sv_rpc_check_implicit_ar( /* check the rpc and complete it if tagged as "completed" */
	CM_SV_DEVICE_PTR_TYPE dev
) {
	/* called from:
	 * - cm_sv_rpc_next_implicit_ar()
	 * - cm_sv_opc_ar_record_response()
	 * - cm_sv_pdev_record_done()
	 * - cm_sv_frame_get_apdu_status_done()
	 */

	 if( cm_is_null(dev->implicit_ar.rpc) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc = dev->implicit_ar.rpc;

		if( CM_RPC_GET_RESPONSE(rpc) == LSA_RSP_UNUSED/*pending*/ ) {

			/* not yet completed */
		}
		else {

			dev->implicit_ar.rpc = LSA_NULL; /* done */

			/***/

			CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "responding rpc(%08X) opnum(%u) with rpc-rsp(%u), pnio_status(0x%x), ndr_len(%u)"
				, rpc, rpc->args.sv.call->opnum
				, CM_RPC_GET_RESPONSE(rpc)
				, rpc->args.sv.call->pnio_status, rpc->args.sv.call->ndr_len
				);

			if( CM_RPC_GET_USER_ID_AR(rpc) == 255 ) { /* see cm_sv_opc_device_read */

				cm_sv_opc_device_read_done(dev->channel, rpc, dev);
			}
			else {

				cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, dev->device_nr, 0);
			}

			/***/

			return LSA_TRUE; /* done */
		}
	}

	/***/

	return LSA_FALSE; /* still pending */
}

/*===========================================================================*/
/*=== EXPLICIT-AR ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_check_explicit_ar( /* check the rpc and complete it if tagged as "completed" */
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* called from:
	 * - cm_sv_rpc_record_indication()
	 * - cm_sv_opc_ar_record_response()
	 * - cm_sv_pdev_record_done()
	 * - cm_sv_frame_get_apdu_status_done()
	 */

	if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;

		if( CM_RPC_GET_RESPONSE(rpc) == LSA_RSP_UNUSED/*pending*/ ) {

			/* not yet completed */
		}
		else if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RECORD_RSP) != 0 ) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.curr_rpc_ind = LSA_NULL; /* done */

			/***/

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "responding rpc(%08X) opnum(%u) with rpc-rsp(%u), pnio_status(0x%x), ndr_len(%u)"
				, rpc, rpc->args.sv.call->opnum
				, CM_RPC_GET_RESPONSE(rpc)
				, rpc->args.sv.call->pnio_status, rpc->args.sv.call->ndr_len
				);

			cm_rpc_request_lower(argr->ar_com.channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, argr->device_nr, 0);

			/***/

			if( cm_ar_com_is_down(argr) ) {

				/* note: timer is reused for other things */

				cm_sv_ar_disconnect_indication_join(argr); /* trigger: curr_rpc_ind == LSA_NULL (record read/write) */
			}
			else if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

				cm_sv_ar_cmi_timer_trigger(argr); /* trigger: rpc-read-write.rsp */
			}
			else {

				/* do nothing */
			}
		}
	}
}

/*===========================================================================*/
/*=== DEVICE READ ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_read( /* AP01467948 */
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;
	CM_SV_DEVICE_PTR_TYPE dev;
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr;
	CM_UPPER_SV_DEVICE_READ_PTR_TYPE read;
	CLRPC_UPPER_SV_CALL_PTR_TYPE call;

	read = rb->args.sv.dev_read;

	dev = cm_sv_dev_get_added(channel, read->device_nr);
	if( cm_is_null(dev) ) {
		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	if( cm_is_not_null(dev->read) ) {
		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	if(
		cm_is_null(read->data)
		|| read->data_length < CM_RECORD_OFFSET
		|| read->record_index > 0xFFFF /* see cast below */
	) {
		cm_callback(channel, CM_ERR_PARAM, &rb);
		return;
	}

	rpc = cm_rpc_alloc_rqb (channel);
	if( cm_is_null(rpc) ) {
		cm_callback(channel, CM_ERR_ALLOC_LOWER, &rb);
		return;
	}

	call = rpc->args.sv.call; /* shortcut */

	call->alloc_len           = read->data_length;
	call->ndr_len             = CM_RECORD_OFFSET;
	call->ndr_data            = read->data;
	call->pnio_status         = CM_PNIO_ERR_NONE;

	/* sanity */
	call->opnum               = CLRPC_PNIO_OPNUM_READ_IMPLICIT;
	call->semantics           = CLRPC_SEMANTICS_IDEMPOTENT;
	call->server_id           = 0;
	call->scall_id            = LSA_NULL;
	call->usr_id              = LSA_NULL;
	call->host_ip             = 0;
	call->udp_port            = 0;
	call->drep.drep_char      = 0;
	call->drep.drep_float     = 0;
	call->drep.drep_int       = 0;
	call->drep.drep_reserverd = 0;
	call->object_id           = cm_uuid_nil_uuid; /* not: clrpc_uuid_nil_uuid */

	/* build read request block */

	data_ptr = call->ndr_data + CLRPC_PNIO_OFFSET;

	CM_PUT16_HTON(data_ptr,    CM_BLK_HEADER_type,    CM_BLOCK_TYPE_READ_REQ);
	CM_PUT16_HTON(data_ptr,    CM_BLK_HEADER_length,  CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	CM_PUT16_HTON(data_ptr,    CM_BLK_HEADER_version, 0x0100/*V1.0*/);

	CM_PUT16_HTON(data_ptr,    CM_BLK_RDWR_seq_number, 0);
	CM_PUT_UUID_HTON(data_ptr, CM_BLK_RDWR_ar_uuid, &cm_uuid_nil_uuid);
	CM_PUT32_HTON(data_ptr,    CM_BLK_RDWR_api, read->api);
	CM_PUT16_HTON(data_ptr,    CM_BLK_RDWR_slot_nr, read->slot_nr);
	CM_PUT16_HTON(data_ptr,    CM_BLK_RDWR_subslot_nr, read->subslot_nr);
	CM_PUT16_HTON(data_ptr,    CM_BLK_RDWR_padding_1, 0);
	CM_PUT16_HTON(data_ptr,    CM_BLK_RDWR_index, (LSA_UINT16)read->record_index);
	CM_PUT32_HTON(data_ptr,    CM_BLK_RDWR_record_data_length, read->data_length - CM_RECORD_OFFSET);
	CM_PUT_UUID_HTON(data_ptr, CM_BLK_RDWR_req_rd_implicit_target_ar_uuid, &read->target_ar_uuid);

	CM_RPC_SET_OPCODE(rpc, CLRPC_OPC_MAX);
	CM_RPC_SET_RESPONSE(rpc, LSA_RSP_UNUSED/*pending*/);
	CM_RPC_SET_USER_ID_HANDLE_DEV_AR(rpc, CM_INVALID_HANDLE, dev->device_nr, 255);

	dev->read = rb;

	cm_sv_rpc_read_implicit_indication(dev, rpc); /* indicate simulated request, checked in cm_sv_rpc_check_implicit_ar */
}

/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_read_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK ) { /* Always false */

		dev->read->args.sv.dev_read->data_length = CM_RECORD_OFFSET;
		dev->read->args.sv.dev_read->pnio_status = CM_PNIO_FAULTY_Record__FeatureNotSupported(0);
	}
	else {

		dev->read->args.sv.dev_read->data_length = rpc->args.sv.call->ndr_len;
		dev->read->args.sv.dev_read->pnio_status = rpc->args.sv.call->pnio_status;
	}

	cm_rpc_free_rqb(channel, rpc);

	cm_callback(channel, CM_OK, &dev->read);
}

/*===========================================================================*/
/*=== RPC CALL INDICATION ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_call_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CLRPC_UPPER_SV_CALL_PTR_TYPE param = rpc->args.sv.call;
	LSA_BOOL ok;
	LSA_UINT32 pnio_err;

	/***/

	CM_SV_TRACE_09(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u) rpc(%08X), from(%08X:%u) opnum(%u) semantics(%u) drepInt(0x%x), ndr_len(%u+%u)"
		, dev->device_nr
		, rpc
		, CM_NTOH32(param->host_ip)
		, CM_NTOH16(param->udp_port)
		, param->opnum
		, param->semantics
		, param->drep.drep_int
		, CLRPC_PNIO_OFFSET
		, param->ndr_len - CLRPC_PNIO_OFFSET
		);

	/*
	 * ip-addresses and port-numbers are checked in CLRPC
	 * - fragments with source ip-address 0.0.0.0 are dropped
	 * - fragments with source ip-address 255.255.255.255 are dropped
	 * - fragments with source port-number 0 are dropped
	 */

	/*
	 * sanity-check the ndr-parameters
	 */

	if(
		param->ndr_len < CLRPC_PNIO_OFFSET
		|| param->ndr_len > param->alloc_len
		|| cm_is_null(param->ndr_data)
		|| ((LSA_UINT32)param->ndr_data & 3) != 0
	) {
		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "ndr_len(%u) not within CLRPC_PNIO_OFFSET(%u) and alloc_len(%u) or ndr_data(0x%x) is null-ptr or not long-aligned"
			, param->ndr_len
			, CLRPC_PNIO_OFFSET
			, param->alloc_len
			, param->ndr_data
			);
		CM_FATAL (); /* a bug in CLRPC (see pnio-signature) */
		return;
	}

	/*
	 * check the call-semantics
	 */

	if( param->semantics != CLRPC_SEMANTICS_IDEMPOTENT ) {

		if( param->semantics == CLRPC_SEMANTICS_AT_MOST_ONCE ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "semantics(%u) not idempotent, at-most-once is ok too"
				, param->semantics
				);
		}
		else {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "semantics(%u) not idempotent (and not at-most-once)"
				, param->semantics
				);

			cm_sv_rpc_call_response(dev, rpc, CLRPC_ERR_FAULTED, 0, 0/*nice*/); /* not: CLRPC_ERR_REJECTED, MS-RPC will retry endlessly! */
			return;
		}
	}

	/***/

	if( CM_SV_DEV_IS_ACTIVE(dev) ) {

		ok = LSA_TRUE; /* active */
	}
	else {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u) is passive, opnum(%u) is_parking(%u)"
			, dev->device_nr
			, param->opnum, dev->is_parking
			);

		if( param->opnum == CLRPC_PNIO_OPNUM_CONNECT && CM_SV_DEV_IS_PARKING(dev) ) {

			ok = LSA_TRUE; /* connect while in parking mode */
		}
		else {

			ok = LSA_FALSE; /* passive */
		}
	}

	/***/

	cm_sv_rpc_mark_as_pending(rpc);

	/***/

	switch( param->opnum ) {

	case CLRPC_PNIO_OPNUM_CONNECT:

		if( ok ) {

			/* RTF running: PDEV is waiting for "all gone", must not establish another AR */

			if( cm_pdusr_rtf_is_running(dev->channel) ) { /* see cm_sv_pdev_Rtf_NoS_IpSuite() */

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u) RTF running"
					, dev->device_nr
					);

				ok = LSA_FALSE; /* RTF running */
			}
		}

		if( ok ) {

			if( dev->central_pdev ) {

				/* MPT pending: possible for an iDevice with central-pdev */
				/* - ARs with PDEV: no problem, pdev-submodules are SO-locked */
				/* - RT-AR without PDEV (shared RT-AR): no problem */
				/* - IRT-AR without PDEV (shared IRT-AR): no problem because no machine tailoring in V5.3 */
			}
			else {

				/* MPT pending: not possible, MPT is not a feature of a normal device or an iDevices with decentral-pdev */
			}
		}

		if( ok ) {

			/* "PDEV committing" is NOT a reason to reject another AR (shared AR or easy-supervisor) */
		}

		if( ok ) {

			if(
				dev->R1_enabled
				&& ! (dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED)
			) {

				ok = LSA_FALSE; //R1 marker (reject rpc-connect if bridge not broken/established)
			}
		}

		if( ! ok ) {

			pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfARResources();
		}
		else {

			pnio_err = cm_sv_rpc_connect_indication(dev, rpc);
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_CONNECT);
		}
		break;

	case CLRPC_PNIO_OPNUM_RELEASE:

		if( ! ok ) {

			pnio_err = CM_PNIO_FAULTY_Release__CMRPC_OutOfARResources();
		}
		else {

			/* RTF running: AR already down or gone, no special handling needed */
			/* MPT pending: no special handling needed */

			pnio_err = cm_sv_rpc_release_indication(dev, rpc);
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_RELEASE);
		}
		break;

	case CLRPC_PNIO_OPNUM_READ:

		if( ! ok ) {

			pnio_err = CM_PNIO_FAULTY_Record__CMRPC_OutOfARResources();
		}
		else {

			/* RTF running: AR already down or gone, no special handling needed */
			/* MPT pending: no special handling needed */

			cm_sv_rpc_record_indication(dev, rpc, LSA_TRUE/*read*/, LSA_FALSE/*explicit*/);

			pnio_err = CM_PNIO_ERR_NONE; /* handled */
		}
		break;

	case CLRPC_PNIO_OPNUM_WRITE:

		if( ! ok ) {

			LSA_UINT32 tmp = CM_PNIO_FAULTY_Record__CMRPC_OutOfARResources();

			pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(tmp);
		}
		else {

			/* RTF running: AR already down or gone, no special handling needed */
			/* MPT pending: no special handling needed */

			cm_sv_rpc_record_indication(dev, rpc, LSA_FALSE/*write*/, LSA_FALSE/*explicit*/);

			pnio_err = CM_PNIO_ERR_NONE; /* handled */
		}
		break;

	case CLRPC_PNIO_OPNUM_CONTROL:

		if( ! ok ) {

			pnio_err = CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_OutOfARResources();
		}
		else {

			/* RTF running: AR already down or gone, no special handling needed */
			/* MPT pending: no special handling needed */

			pnio_err = cm_sv_rpc_control_indication(dev, rpc);
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_CONTROL);
		}
		break;

	case CLRPC_PNIO_OPNUM_READ_IMPLICIT:

		if( ! ok ) {

			pnio_err = CM_PNIO_FAULTY_Record__CMRPC_OutOfARResources();
		}
		else {

			/* RTF running: read does not interfere with RTF */
			/* MPT pending: read does not interfere with MPT */

			cm_sv_rpc_read_implicit_indication(dev, rpc);

			pnio_err = CM_PNIO_ERR_NONE; /* handled */
		}
		break;

	default:
		CM_FATAL(); /* unreachable, see cm_rpc_register() */
		return;
	}

	/***/

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		cm_sv_rpc_call_response(dev, rpc, CLRPC_OK, pnio_err, 0/*nice*/);
	}
	else {

		/* pending or handled */
	}
}

/*===========================================================================*/
/*=== RPC CALL RESPONSE =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_call_response(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_UINT16 clrpc_response, /* CLRPC_OK or CLRPC_ERR_FAULTED */
	LSA_UINT32 pnio_status,    /* used if CLRPC_OK */
	LSA_UINT32 ndr_length      /* including CLRPC_PNIO_OFFSET and PNIO header! */
) {

	CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "rpc(%08X) opnum(%u) rpc-rsp(%u) pnio_status(0x%x) ndr_length(%u)"
		, rpc
		, rpc->args.sv.call->opnum
		, clrpc_response
		, pnio_status
		, ndr_length
		);

	/***/

	CM_ASSERT(
		clrpc_response == CLRPC_OK
		|| clrpc_response == CLRPC_ERR_FAULTED
	);

	if( ndr_length == 0 ) {
		/* nice */
		ndr_length = CLRPC_PNIO_OFFSET;
	}

	if(
		ndr_length < CLRPC_PNIO_OFFSET
		|| ndr_length > rpc->args.sv.call->alloc_len
	) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "ndr_length out-of-range (user?)"
			);

		CM_FATAL(); /* cannot report */
	}

	/***/

	rpc->args.sv.call->ndr_len = ndr_length;

	rpc->args.sv.call->pnio_status = pnio_status;

	CM_RPC_SET_RESPONSE(rpc, clrpc_response);

	cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, dev->device_nr, 0);
}

/*===========================================================================*/
/*=== RPC CALL REQUEST ======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_call_request(
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_ready4rtclass3,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
) {

#if CM_CFG_MAX_SV_ARS > 0x7F
# error "CM_CFG_MAX_SV_ARS must not be greater than 0x7F"
#endif

	if( is_ready4rtclass3 ) {

		ar_nr |= 0x80; /* see cm_sv_rpc_lower_done() */
	}

	cm_rpc_request_lower(channel, rpc, CLRPC_OPC_CL_CALL, device_nr, ar_nr);
}

/*===========================================================================*/
/*=== RPC CALL CONFIRMATION =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_call_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_ready4rtclass3,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u), rpc-rsp(%u)"
		, argr->device_nr, argr->ar_nr, CM_RPC_GET_RESPONSE(rpc)
		);

	/***/

	if(
		CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK
		&& rpc->args.cl.call->ndr_len < CLRPC_PNIO_OFFSET
	) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "ndr_len < CLRPC_PNIO_OFFSET"
			);
		CM_FATAL (); /* a bug in CLRPC, see "pnio-signature" */
	}
	else {

		/* note: dispatching by looking into the response PDU could be wrong */

		if( is_ready4rtclass3 ) {

			cm_sv_rpc_ready4rtclass3_confirmation(argr, rpc);
		}
		else {

			cm_sv_rpc_appl_ready_confirmation(argr, rpc);
		}
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
