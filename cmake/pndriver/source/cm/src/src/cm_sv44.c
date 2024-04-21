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
/*  F i l e               &F: cm_sv44.c                                 :F&  */
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
/*  - CM-SV-AR state-machine                                                 */
/*    (abort, release, shutdown, detach)                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7440
#define CM_MODULE_ID       7440

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_frames_remove(
	CM_AR_GRAPH_PTR_TYPE argr
);

/*===========================================================================*/
/*=== AR ABORT ==============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_abort(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_ABORT_PTR_TYPE param = rb->args.sv.ar_abort;
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
	else if( cm_ar_com_is_down(argr) ) {

		rsp = CM_OK_CANCELLED; /* late, already aborting */
	}
	else {

		rsp = CM_OK;
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then abort */

	if( rsp == CM_OK && cm_is_not_null(argr) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) aborted by user"
			, argr->device_nr, argr->ar_nr
			);

		cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_ABORT, 0);
		cm_ar_com_down(argr, CM_AR_REASON_ABORT);
	}
}

/*===========================================================================*/
/*=== RELEASE INDICATION ====================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_rpc_release_indication(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_AR_GRAPH_PTR_TYPE argr;

	{
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;

	data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
	data_length = rpc->args.sv.call->ndr_len  - CLRPC_PNIO_OFFSET;

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) data_length(%u)"
		, dev->device_nr, data_length
		);

	/*
	 * IODReleaseReq ::= ReleaseBlock
	 *
	 * ReleaseBlock ::=
	 *   BlockHeader
	 *   , reserved (2 octets), ARUUID, SessionKey
	 *   , reserved (2 octets), ControlCommand, ControlBlockProperties
	 */

	if( CM_BLK_HEADER_SIZE > data_length ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "too few data"
			);
		return CM_PNIO_FAULTY_Release__CMRPC_ArgsLengthInvalid();
	}

	{
	LSA_UINT16 block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
	LSA_UINT16 block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
	LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( block_type != CM_BLOCK_TYPE_RELBLOCK_REQ ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong block-type"
			);
		return CM_PNIO_FAULTY_Release__CMRPC_UnknownBlocks();
	}

	if( block_length != data_length - CM_BLK_HEADER_TypLen_SIZE ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong block-length"
			);
		return CM_PNIO_FAULTY_Release__CMRPC_ArgsLengthInvalid();
	}

	if( (block_version & 0xFF00) != 0x0100/*V1.0*/ ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong block-version.high(0x%x)"
			, block_version
			);
		return CM_PNIO_FAULTY_ReleaseBlock(2); /* wrong block-version.high */
	}

	if( (block_version & 0x00FF) != 0x0000/*V1.0*/ ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong block-version.low(0x%x)"
			, block_version
			);
		return CM_PNIO_FAULTY_ReleaseBlock(3); /* wrong block-version.low */
	}
	}

	/***/

	{
	CLRPC_UUID_TYPE ar_uuid;
	LSA_UINT16 session_key;
	LSA_UINT16 control_command;

	if( CM_BLK_REL_SIZE != data_length ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "length/block-type mismatch"
			);
		return CM_PNIO_FAULTY_ReleaseBlock(1); /* length/block-type mismatch */
	}

	CM_GET_UUID_NTOH(&ar_uuid, data_ptr, CM_BLK_REL_ar_uuid);

	session_key = CM_GET16_NTOH(data_ptr, CM_BLK_REL_session_key);

	control_command = CM_GET16_NTOH(data_ptr, CM_BLK_REL_control_command);

	/* NOTE: "reserved" fields must not be checked */

	if( control_command != CM_CONTROL_COMMAND_RELEASE ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong control-command"
			);
		return CM_PNIO_FAULTY_ReleaseBlock(8); /* wrong control-command */
	}

	/***/

	argr = cm_sv_ar_lookup_by_aruuid(dev, &ar_uuid);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE /* not: _UNEXP */
			, "dev(%u) ARUUID not found"
			, dev->device_nr
			);

		return CM_PNIO_FAULTY_Release__CMRPC_ARUUIDUnknown();
	}

	if( session_key != argr->cm.sv.session_key_connect ) {
		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "wrong session key(%u) expected(%u)"
			, session_key, argr->cm.sv.session_key_connect
			);
		return CM_PNIO_FAULTY_ReleaseBlock(6); /* wrong session-key */
	}
	}

	/***/

	if( rpc->args.sv.call->host_ip != argr->ar.host_ip ) {
		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "host(%08X) not conn-host(%08X)"
			, CM_NTOH32(rpc->args.sv.call->host_ip)
			, CM_NTOH32(argr->ar.host_ip)
			);
		return CM_PNIO_FAULTY_Release__CMRPC_StateConflict(); /* unexpected ip-addr (V7.1i1.8, TIA 2679444) */
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.curr_rpc_ind) ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "already servicing a call (two clients on same host using the same ARUUID?)"
			);
		return CM_PNIO_FAULTY_Release__CMRPC_StateConflict(); /* unexpected call (V7.1i1.8, TIA 2679444) */
	}

	/***/

	if( cm_ar_com_is_down(argr) ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "already down (telling unknown aruuid)"
			);
		return CM_PNIO_FAULTY_Release__CMRPC_ARUUIDUnknown(); /* already down (V7.1i1.8, TIA 2679444) */
	}
	}

	/*
	 * note: we do not wait for abort-completion (since this would block the caller)
	 */

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) aborted by rpc-release"
		, argr->device_nr, argr->ar_nr
		);

	cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_REL, 0);
	cm_ar_com_down(argr, CM_AR_REASON_REL); /* release */

	/***/

	{
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;

	/*
	 * IODReleaseRes ::= ReleaseBlock ^ NULL
	 *
	 * In case of a negative response NULL shall be transmitted.
	 */

	data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
	data_length = rpc->args.sv.call->ndr_len; /* req-size == rsp-size */

	/*
	 * patch the block-type
	 * and the control-command
	 */

	CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_RELBLOCK_RSP);

	CM_PUT16_HTON(data_ptr, CM_BLK_REL_control_command, CM_CONTROL_COMMAND_DONE);

	cm_sv_rpc_call_response(dev, rpc, CLRPC_OK, CM_PNIO_ERR_NONE, data_length);
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== AR-COM DOWN ===========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_com_down(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x) r1_flags(0x%08x) reason(%u)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags, argr->cm.sv.r1_flags
		, argr->ar_com.down
		);

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) != 0 ) {

		CM_FATAL(); /* a bug in caller */
	}
	else if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_JOIN_DISC | CM_SV_AR_CNFL_DO_OWNERSHIP_DISC)) != 0 ) {

		CM_FATAL(); /* a bug */
	}
	else if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_DISCONN) != 0 ) { //R1 marker

		CM_FATAL(); /* a bug */
	}
	else { /* the AR is shutting down (note: after rpc-connect.rsp, see where cm_ar_com_ready() is called) */

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_DOWN_PHASE; /* note: check with CM_SV_AR_IS_DOWN() */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: replace cm_ar_com_is_down() with CM_SV_AR_IS_DOWN() [if appropriate; after ar-com-ready] checking CM_SV_AR_FL_DOWN_PHASE flag")
#endif /* CM_MESSAGE */

		/***/

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_JOIN_DISC; /* lock-out synchronous callbacks */

		/***/

		if(
			argr->cm.sv.r1_flags == 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 /* note: only original-AR makes an entry */
		) {

			LSA_UINT32 pnio_err = CM_PNIO_ERR_RTA_PROTOCOL(argr->ar_com.down); /* AP00352424: LogBookData does not contain the PNIOStatus */

			LSA_UINT32 entry_detail = (((LSA_UINT32)argr->ar_com.cycle_counter << 16) | (LSA_UINT16)argr->ar_com.down);

			cm_sv_logbook_write(argr, pnio_err, entry_detail);
		}

		/***/

		cm_sv_ar_cmi_timer_stop(argr);

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_DO_CTRL_RSP) != 0
			|| argr->cm.sv.curr_rpc_ind_is_parked /* TIA 1518690 */
		) {

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PBE_DO_CTRL_RSP;

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL;
				argr->cm.sv.curr_rpc_ind_is_parked = LSA_FALSE; /* TIA 1518690 */

				cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK, CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(), 0/*nice*/);
			}
		}

		/***/

		if( cm_is_not_null(argr->cm.sv.arset) ) {

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 && argr->cm.sv.arset->is_aborting ) { /* TIA 2253099 */

				/* explanation see cm_sv_opc_arset_abort() */
				argr->cm.sv.arset->is_aborting = LSA_FALSE;
				cm_sv_arset_check_primary(argr);
				argr->cm.sv.arset->is_aborting = LSA_TRUE; /* play it safe */
			}
			else {

				cm_sv_arset_check_primary(argr);
			}

			/***/

			if( cm_is_not_null(argr->cm.sv.curr_arset_trigger) ) {

				CM_UPPER_RQB_PTR_TYPE temp = argr->cm.sv.curr_arset_trigger;
				argr->cm.sv.curr_arset_trigger = LSA_NULL;

				cm_callback(argr->ar_com.channel, CM_OK_CANCELLED, &temp);
			}
		}

		/***/

		cm_sv_ar_almi_down(argr); /* note: disables pdev-alarms too */

		cm_sv_ar_almr_down(argr);

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_OWNERSHIP_RSP) != 0 ) {

			/* must serialize, only one resource, see cm_sv_ar_ownership_build() */

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_DO_OWNERSHIP_DISC; /* see cm_sv_opc_ar_ownership_response() */
		}
		else {

			cm_sv_ar_ownership_disc_indication(argr);
		}
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP INDICATION (DISCONNECT) ==================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_ownership_disc_indication(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_EARLY_ABORT) != 0
		|| cm_is_iosar_with_device_access(argr) /* easy-supervisor */
		|| ((argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0) //R1 marker
	) {

		cm_sv_ar_ownership_disc_response(argr); /* fake */
	}
	else {

		/* the ownership/disconnect indication shall be fired as soon as possible */
		/* however, this makes sense only if we do not trigger another action in parallel */

		{
		CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_ownership_build(argr, CM_SV_AR_CONTEXT_DISC);

		if( cm_is_null(event) ) {

			cm_sv_ar_ownership_disc_response(argr); /* fake */
		}
		else {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

			cm_callback(argr->ar_com.channel, CM_OK, &event);
		}
		}
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP RESPONSE (DISCONNECT) ====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_ownership_disc_response(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
		);

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_JOIN_DISC) == 0) {

		CM_FATAL();
	}
	else {

		if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_FRAMES_PAS | CM_SV_AR_CNFL_WF_ALARM_RMV | CM_SV_AR_CNFL_WF_PDEV_RELINQUISH | CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF | CM_SV_AR_CNFL_WF_IPSUITE_LOCK)) != 0 ) {

			CM_FATAL();
		}
		else {

			CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

			/***/

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_PAS;

			cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_PASSIVATE, 0);

			/***/

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_ALARM_RMV;

			cm_arcq_request(argr, CM_ARCQ_OPC_ALARM_REMOVE, 0);

			/***/

			if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

				/* pdev-relinquish is done unconditionally for ARs with pdev-submodules */
				/* see CM-PD: pdev-begin sets pdprm->sv_ar_nr, and pdev-relinquish resets it */
				/* pdev-relinquish takes effect only if pdprm->sv_ar_nr matches */
				/* S1: always, S2/R1: if not "late" (i.e., scenarios with takeover and/or AR abort) */

				CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_rel_rqb; /* note: cannot reuse the prmend-RQB */

				if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

					CM_FATAL();
				}
				else {

					CM_AR_GRAPH_PTR_TYPE partner_argr;

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PDEV_RELINQUISH;

					if( cm_is_not_null(argr->cm.sv.arset) ) { /* TIA 1892236 */

						CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

						if( arset->arr[0] == argr ) {

							partner_argr = arset->arr[1]; /* may be LSA_NULL*/
						}
						else if( arset->arr[1] == argr ) {

							partner_argr = arset->arr[0]; /* never LSA_NULL*/
						}
						else {

							partner_argr = LSA_NULL; /* unreachable */
						}
					}
					else {

						partner_argr = LSA_NULL;
					}

					if( cm_is_not_null(partner_argr) && ! cm_ar_com_is_down(partner_argr) ) {

						pd->args.pd.prm_relinquish->partner_session_key = partner_argr->session_key;
					}
					else {

						pd->args.pd.prm_relinquish->partner_session_key = 0;
					}

					cm_sv_pdev_request(channel, CM_OPC_PD_PRM_RELINQUISH_SERVER, argr->device_nr, argr->ar_nr, pd);
				}
			}

			/***/

			{
			CM_RPC_LOWER_RQB_PTR_TYPE rpc = LSA_NULL;

			if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) == (CM_SV_AR_FL_APPL_READY_REQ | 0) ) {

				rpc = argr->cm.sv.rebind_rqb; /* do cancel */
			}

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_RDY4RTC3_CNF) != 0 ) {

				rpc = argr->cm.sv.rebind_rqb; /* do cancel */
			}

			if( cm_is_not_null(rpc) ) { /* note: reusing rebind-rqb for cancel */

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "cancelling rpc-calls, rpc(%08X)", rpc
					);

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF;

				/***/

				if( CM_RPC_GET_OPCODE(rpc) != 0/*unused*/ ) {

					CM_FATAL(); /* a bug */
				}
				else {

					rpc->args.cl.cancel->client_id = cm_sv_get_rpc_client_id(argr);

					cm_rpc_request_lower(channel, rpc, CLRPC_OPC_CL_CANCEL, argr->device_nr, argr->ar_nr);
				}
			}
			}

			/***/

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_IS_IPSUITE_LOCKED) != 0 ) {

				argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_IS_IPSUITE_LOCKED;

				/***/

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_IPSUITE_LOCK;

				cm_oha_control_ip_suite(argr, OHA_UNLOCK_SET, argr->cm.sv.lock_rqb);
			}

			/***/

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN) != 0) { //R1 marker

				argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_DO_DISCONN;

				cm_sv_r1b_post_disconnect(argr);
			}

			/***/

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_JOIN_DISC;

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_JOIN_DISC */
		}
	}
}

/*===========================================================================*/
/*=== AR FRAMES PASSIVATE DONE ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_frames_passivate_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_FRAMES_PAS) == 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_FRAMES_PAS;

		/***/

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_FRAMES_PAS */
	}
}

/*===========================================================================*/
/*=== AR ALARM REMOVE DONE ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_alarm_remove_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_ALARM_RMV) == 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_ALARM_RMV;

		/***/

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_ALARM_RMV */
	}
}

/*===========================================================================*/
/*=== AR PDEV RESET ERROR INFO ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_pdev_reset_error_info(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_BOOL is_remote_pdev
) {
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap = cm_ar_graph_find_ap(argr, 0);

	if( cm_is_not_null(ar_ap) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				if( (is_remote_pdev && CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, ar_sub->subslot_nr) )
					|| (!is_remote_pdev && CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, ar_sub->subslot_nr) )
				) {

					ar_sub->sv.err_flags  = 0;
					ar_sub->sv.err_index  = 0;
					ar_sub->sv.err_offset = 0;
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== PDEV PRM BEGIN ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PRM_BEGIN) != 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_begin_rqb;

		if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_WF_PRM_BEGIN;

			cm_sv_ar_pdev_reset_error_info(argr, LSA_FALSE /*local pdev*/);

			pd->args.pd.prm_begin->sv.slot_nr    = slot_nr;
			pd->args.pd.prm_begin->sv.subslot_nr = subslot_nr;

			cm_sv_pdev_request(argr->ar_com.channel, CM_OPC_PD_PRM_BEGIN_SERVER, argr->device_nr, argr->ar_nr, pd);
		}
	}
}

/*===========================================================================*/
/*=== PDEV PRM BEGIN DONE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_begin_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags
		);

	if(
		(argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PRM_BEGIN) == 0
		|| pd != argr->cm.sv.pdev_begin_rqb
		|| CM_RQB_GET_RESPONSE(pd) != CM_OK
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_WF_PRM_BEGIN;

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_FL_WF_PRM_BEGIN */
		}
		else {

			/* note: no explicit wait-state in prm-sequence; only a check in cm_sv_pd_prm_end_confirmation() */

			cm_sv_ar_submodule_state_action(argr); /* trigger: cm_sv_pd_prm_begin_done (TIA 2197506) */
		}
	}
}

/*===========================================================================*/
/*=== PDEV-OWNERSHIP RELINQUISHED ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_relinquish_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags
		);

	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PDEV_RELINQUISH) == 0
		|| pd != argr->cm.sv.pdev_rel_rqb
		|| CM_RQB_GET_RESPONSE(pd) != CM_OK /* note: relinquish does not fail */
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PDEV_RELINQUISH;

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		/***/

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PDEV_RELINQUISH */
	}
}

/*===========================================================================*/
/*=== RPC-CANCEL CONFIRMATION ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_cancel_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) rpc(%08X) rpc-rsp(%u)"
		, argr->device_nr, argr->ar_nr
		, rpc, CM_RPC_GET_RESPONSE(rpc)
		);

	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF) == 0
		|| rpc != argr->cm.sv.rebind_rqb
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF;

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

		/***/

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF */
	}
}

/*===========================================================================*/
/*=== AR DISCONNECT INDICATION JOIN =========================================*/
/*===========================================================================*/

#define CM_SV_DISC_CN_FLAGS        0x0001
#define CM_SV_DISC_AR_FLAGS        0x0002
#define CM_SV_DISC_CURR_RPC_IND    0x0004
#define CM_SV_DISC_CURR_AR_PDEV    0x0008
#define CM_SV_DISC_CURR_AR_CTRL    0x0010
#define CM_SV_DISC_ARDATA_REFCNT   0x0020
#define CM_SV_DISC_PI_CTRL_CNF     0x0040
#define CM_SV_DISC_APPL_READY_CNF  0x0080
#define CM_SV_DISC_NEW_DATA_CNF    0x0100
#define CM_SV_DISC_ALMI_IDLE       0x0200
#define CM_SV_DISC_ALMR_IDLE       0x0400
#define CM_SV_DISC_R1_FLAGS        0x0800

LSA_VOID
cm_sv_ar_disconnect_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) == 0 ) {

		/* do nothing (sanity, unreachable, see callers) */
	}
	else { /* AP01367655: wait for all user-responses and lower-confirmations before signalling disconnect.ind (V5.2i4.8) */

		const LSA_UINT32 cnfl_wf_mask =
			( CM_SV_AR_CNFL_WF_UNPARK
			| CM_SV_AR_CNFL_WF_SEND_CLOCK
			| CM_SV_AR_CNFL_WF_FRAMES_ADD
			| CM_SV_AR_CNFL_WF_IPSUITE_LOCK /* also used for "unlock" */
			| CM_SV_AR_CNFL_WF_JOIN_CONN
			| CM_SV_AR_CNFL_WF_CONN_RSP
			| CM_SV_AR_CNFL_WF_REBIND
			| CM_SV_AR_CNFL_WF_ALARM_ADD
			/***/
			| CM_SV_AR_CNFL_WF_JOIN_OSHP
			| CM_SV_AR_CNFL_WF_OWNERSHIP_RSP
			| CM_SV_AR_CNFL_WF_PROV_ACT
			| CM_SV_AR_CNFL_WF_CONS_ACT
			/***/
			| CM_SV_AR_CNFL_WF_RECORD_RSP
			| CM_SV_AR_CNFL_WF_PDEVPRM_RSP
			| CM_SV_AR_CNFL_WF_PRMEND_RSP
			| CM_SV_AR_CNFL_WF_PD_PRM_END_CNF
			| CM_SV_AR_CNFL_WF_RIR_RSP
			| CM_SV_AR_CNFL_WF_INDATA_RSP
			| CM_SV_AR_CNFL_WF_DATA_STATUS_RSP
			/***/
			| CM_SV_AR_CNFL_WF_JOIN_DISC
			| CM_SV_AR_CNFL_WF_FRAMES_PAS
			| CM_SV_AR_CNFL_WF_ALARM_RMV
			| CM_SV_AR_CNFL_WF_PDEV_RELINQUISH
			| CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF
			| CM_SV_AR_CNFL_WF_DISC_RSP
			| CM_SV_AR_CNFL_WF_FRAMES_RMV
			);

		const LSA_UINT32 arfl_wf_mask =
			( CM_SV_AR_FL_WF_PROV_PAS_AUX
			| CM_SV_AR_FL_WF_RDY4RTC3_CNF
			| CM_SV_AR_FL_WF_PRM_BEGIN
			);

		const LSA_UINT32 r1fl_wf_mask =
			( CM_SV_AR_R1FL_WF_CONN_POSTED //R1 VV: if set here it is a bug. make fatal. same for all flags before com-ready.
			| CM_SV_AR_R1FL_EX_CONN_DONE
			| CM_SV_AR_R1FL_WF_CONN_DONE_POSTED
			| CM_SV_AR_R1FL_WF_DISCONN_POSTED
			| CM_SV_AR_R1FL_EX_DISCONN_DONE
			| CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED
			| CM_SV_AR_R1FL_WF_SUBSTATE_POSTED
			| CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE
			| CM_SV_AR_R1FL_WF_PRM_END_REMOTE
			| CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED
			| CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED
			);

		LSA_UINT16 disc = 0;

		if( (argr->cm.sv.cn_flags & cnfl_wf_mask) != 0 ) {
			disc |= CM_SV_DISC_CN_FLAGS;
		}

		if( (argr->cm.sv.ar_flags & arfl_wf_mask) != 0 ) {
			disc |= CM_SV_DISC_AR_FLAGS;
		}

		if( cm_is_not_null(argr->cm.sv.curr_rpc_ind) ) {
			disc |= CM_SV_DISC_CURR_RPC_IND;
		}

		if( cm_is_not_null(argr->cm.sv.curr_ar_pdevprm) ) {
			disc |= CM_SV_DISC_CURR_AR_PDEV;
		}

		if( cm_is_not_null(argr->cm.sv.curr_ar_ctrl) ) {
			disc |= CM_SV_DISC_CURR_AR_CTRL;
		}

		if( argr->cm.sv.rec.ardata_refcnt != 0 ) {
			disc |= CM_SV_DISC_ARDATA_REFCNT;
		}

		if( argr->cm.sv.pi.pi_pending ) {
			disc |= CM_SV_DISC_PI_CTRL_CNF;
		}

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) == (CM_SV_AR_FL_APPL_READY_REQ | 0) ) {
			disc |= CM_SV_DISC_APPL_READY_CNF;
		}

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_NEW_DATA_REQ | CM_SV_AR_FL_NEW_DATA_CNF)) == (CM_SV_AR_FL_NEW_DATA_REQ | 0) ) {
			disc |= CM_SV_DISC_NEW_DATA_CNF;
		}

		if( ! cm_sv_ar_almi_is_idle(argr) ) {
			disc |= CM_SV_DISC_ALMI_IDLE;
		}

		if( ! cm_sv_ar_almr_is_idle(argr) ) {
			disc |= CM_SV_DISC_ALMR_IDLE;
		}

		if( (argr->cm.sv.r1_flags & r1fl_wf_mask) != 0 ) {
			disc |= CM_SV_DISC_R1_FLAGS;
		}

		/***/

		if( disc != 0 ) {

			CM_SV_TRACE_10(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) disc(0x%04x), cn_flags(0x%08x)/wf(0x%08x), ar_flags(0x%08x)/wf(0x%08x), r1_flags(0x%08x)/wf(0x%08x), ardata_refcnt(%u)"
				, argr->device_nr, argr->ar_nr
				, disc
				, argr->cm.sv.cn_flags, (argr->cm.sv.cn_flags & cnfl_wf_mask)
				, argr->cm.sv.ar_flags, (argr->cm.sv.ar_flags & arfl_wf_mask)
				, argr->cm.sv.r1_flags, (argr->cm.sv.r1_flags & r1fl_wf_mask)
				, argr->cm.sv.rec.ardata_refcnt
				);
		}
		else { /* joined */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) joined for disconnect"
				, argr->device_nr, argr->ar_nr
				);

			/***/

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker

				cm_sv_r1b_post_disconnect_done(argr);
			}
			else if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_EARLY_ABORT) != 0 ) {

				cm_sv_frames_remove(argr);
			}
			else {

				CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_disconnect = argr->cm.sv.ar_connect_mem; /* reusing the connect.ind (is not-null) */

				ar_disconnect->disconnect.reason_code = argr->ar_com.down;

				ar_disconnect->disconnect.reason_pnio_status = argr->ar_com.down_pnio_status;

				/* ar_disconnect->disconnect.was_in_data is set in cm_sv_opc_ar_in_data_response() (TIA 1918234) */

				/***/

				if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_DISC_RSP) != 0 ) {

					CM_FATAL();
				}
				else {

					CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_DISCONNECT_IND);

					event->args.sv.ar_event->u.sv.ar_disconnect = ar_disconnect;

					/***/

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_DISC_RSP;

					cm_callback(argr->ar_com.channel, CM_OK, &event);
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== AR DISCONNECT RESPONSE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_disconnect_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_DISC_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_DISC_RSP;

		/***/

		rb->args.sv.ar_event->u.sv.ar_disconnect = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_EXP_USER_REQ; /* after ar-disconnect.rsp (user saw the disc.ind) */

		/***/

		cm_sv_frames_remove(argr);
	}
}

/*===========================================================================*/
/*=== AR FRAMES REMOVE ======================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_frames_remove(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_FRAMES_RMV) != 0 ) {

		CM_FATAL();
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) removing frames, cn_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags
			);

		if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {
			CM_FATAL(); /* a bug */
		}

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_RMV;

		cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_REMOVE, 0);
	}
}

/*===========================================================================*/
/*=== AR FRAMES REMOVE DONE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_frames_remove_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags
		, argr->cm.sv.ar_flags
		);

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_FRAMES_RMV) == 0 ) {

		CM_FATAL();
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev; /* save */

		/***/

		cm_sv_ar_free(argr, CM_PNIO_ERR_NONE, CM_AR_REASON_NONE);

		cm_sv_dev_led_info_eval(dev); /* trigger: ar-graph freed (expected-config gone, m-consumers gone) */
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
