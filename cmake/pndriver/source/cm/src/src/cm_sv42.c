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
/*  F i l e               &F: cm_sv42.c                                 :F&  */
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
/*    (prm-end, appl-ready, in-data)                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7420
#define CM_MODULE_ID       7420

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_pdevprm_indication(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_pd_prm_end_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE plg_rel_sub,
	LSA_BOOL is_prm_begin_end,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE local_pdev_if
); /* forward */

static LSA_VOID
cm_sv_pd_prm_end_remote_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE plg_rel_sub,
	LSA_BOOL is_prm_begin_end,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE remote_pdev_if
); /* forward */

static CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_prm_end_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL is_prm_begin_end
); /* forward */

static LSA_VOID
cm_sv_ar_prov_cons_on_prmend(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_ar_set2unk_enpras(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_ar_in_data_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL do_empty
); /* forward */

/*===========================================================================*/
/*=== RPC CONTROL INDICATION (PrmEnd) =======================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_rpc_control_indication(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;
	struct {
		LSA_UINT16 block_type;
		LSA_UINT16 block_length;
		LSA_UINT16 block_version;
		/***/
		CLRPC_UUID_TYPE ar_uuid;
		LSA_UINT16 session_key;
		LSA_UINT16 alarm_sequence_number;
		LSA_UINT16 command;
		/*LSA_UINT16 properties;*/
	} ctrl;
	LSA_UINT8 code1;
	CM_AR_GRAPH_PTR_TYPE argr;

	data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
	data_length = rpc->args.sv.call->ndr_len  - CLRPC_PNIO_OFFSET;

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) data_length(%u)"
		, dev->device_nr, data_length
		);

	/*
	 * IODControlReq ::=
	 *   ControlBlockConnect ^ ControlBlockPlug
	 *
	 * ControlBlockConnect ::=
	 *   BlockHeader
	 *   , reserved (2 octets), ARUUID, SessionKey
	 *   , reserved (2 octets), ControlCommand, ControlBlockProperties
	 *
	 * ControlBlockPlug ::=
	 *   BlockHeader
	 *   , reserved (2 octets), ARUUID, SessionKey
	 *   , AlarmSequenceNumber, ControlCommand, ControlBlockProperties
	 *
	 * The field AlarmSequenceNumber shall contain the value of the sub-field
	 * AlarmSpecifier.SequenceNumber of the corresponding AlarmNotification-PDU.
	 */

	if( CM_BLK_HEADER_SIZE > data_length ) {

		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "too few data"
			);
		/* cannot decide if connect or plug! */
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ArgsLengthInvalid();
	}

	ctrl.block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
	ctrl.block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
	ctrl.block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( ctrl.block_type == CM_BLOCK_TYPE_PRM_BEGIN_REQ ) {

		return cm_sv_rpc_control_indication_prm_begin(dev, rpc);
	}

	switch( ctrl.block_type ) {

	case CM_BLOCK_TYPE_IODBLOCK_CONN_REQ:
		code1 = CM_PNIO_FAULTY_IODControl_Code1_Connect;
		break;

	case CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ:
		code1 = CM_PNIO_FAULTY_IODControl_Code1_Plug;
		break;

	default:
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block-type(0x%x) neither iodblock-conn-req nor iodblock-plug-req"
			, ctrl.block_type
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_UnknownBlocks();
	}

	if( ctrl.block_length != data_length - CM_BLK_HEADER_TypLen_SIZE ) {

		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong block-length"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ArgsLengthInvalid();
	}

	if( (ctrl.block_version & 0xFF00) != 0x0100/*V1.0*/ ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block-version.high(0x%x) not supported"
			, ctrl.block_version
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 2); /* wrong block-version.high */
	}

	if( (ctrl.block_version & 0x00FF) != 0x0000/*V1.0*/ ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block-version.low(0x%x) not supported"
			, ctrl.block_version
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 3); /* wrong block-version.low */
	}

	/***/

	if( CM_BLK_CTRL_SIZE != data_length ) {

		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/block-type mismatch"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 1); /* wrong block-length */
	}

	/*
	reserved1 = not checked
	*/

	CM_GET_UUID_NTOH(&ctrl.ar_uuid, data_ptr, CM_BLK_CTRL_ar_uuid);

	ctrl.session_key = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_session_key);

	ctrl.alarm_sequence_number = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_reserved2_alarm_sequence_number);

	ctrl.command = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_command);

	/*ctrl.properties = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_properties);*/

	/***/

	argr = cm_sv_ar_lookup_by_aruuid(dev, &ctrl.ar_uuid);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ARUUID not found"
			, dev->device_nr
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown();
	}

	if( ctrl.session_key != argr->cm.sv.session_key_connect ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) wrong session key(%u) expected(%u)"
			, dev->device_nr
			, ctrl.session_key
			, argr->cm.sv.session_key_connect
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 6); /* wrong session-key, ignore */
	}

	if( ctrl.block_type == CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ ) {

		if( ctrl.alarm_sequence_number > 0x07FF ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) prm-end, alarm_seq(%u) too big"
				, dev->device_nr
				, ctrl.alarm_sequence_number
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 7); /* wrong alarm-seq, ignore */
		}
	}
	else {

		/* reserved2 = not checked (for compatibility, it was not checked formerly) */
	}

	if( ctrl.command != CM_CONTROL_COMMAND_PRM_END ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) prm-end, wrong control-command(0x%x)"
			, dev->device_nr
			, ctrl.command
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 8); /* wrong control-command, ignore */
	}

#if 0 /* ctrl.properties are no longer checked */
	if( ctrl.properties != ... ) {
		/*
		 * definition changed with WD61158-6-10_v21_i323:
		 * Bit 0-15 ... shall not be tested at the receiving side
		 */
	}
#endif

	/***/

	if( rpc->args.sv.call->host_ip != argr->ar.host_ip ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "host(%08X) not conn-host(%08X)"
			, CM_NTOH32(rpc->args.sv.call->host_ip)
			, CM_NTOH32(argr->ar.host_ip)
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict(); /* unexpected ip-addr (V7.1i1.8, TIA 2679444) */
	}

	if( cm_is_not_null(argr->cm.sv.curr_rpc_ind) ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "already servicing a call (two clients on same host using the same ARUUID?)"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict(); /* unexpected call (V7.1i1.8, TIA 2679444) */
	}

	if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) easy-supervisor does not expect PrmEnd"
			, argr->device_nr, argr->ar_nr
			);

		/*
		 * NOTE: without this if(), the misbehaving easy-supervisor would see "ok" (see rerun below).
		 *       we prefer to see an error indicating that something is wrong.
		 */

		return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 5); /* device-access does not use prm-end, tell wrong aruuid */
	}

	if( cm_ar_com_is_down(argr) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) already down (telling unknown aruuid)"
			, argr->device_nr, argr->ar_nr
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(); /* already down (V7.1i1.8, TIA 2679444) */
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_EXP_PRM_END) == 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "prm-end unexpected, assuming rerun (e.g. response was lost, retransmission after appl-ready)"
			);
		goto rerun;
	}

	if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_PDEVPRM_RSP | CM_SV_AR_CNFL_WF_PD_PRM_END_CNF)) != 0 ) {

		/* unreachable, "curr_rpc_ind" still running (see check above) */
	}

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_END_IND) != 0 ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "prm-end already seen, assuming rerun (e.g. response was lost, retransmission before appl-ready)"
			);
		goto rerun;
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

		if( code1 != CM_PNIO_FAULTY_IODControl_Code1_Connect ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block-type(0x%x) not iodblock-conn-req"
				, ctrl.block_type
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 0); /* wrong block-type */
		}
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

		if( code1 != CM_PNIO_FAULTY_IODControl_Code1_Connect ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block-type(0x%x) not iodblock-conn-req"
				, ctrl.block_type
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 0); /* wrong block-type */
		}
	}
	else { /* data-phase */

		CM_UPPER_ALARM_PTR_TYPE plg_rel_alarm;

		if( code1 != CM_PNIO_FAULTY_IODControl_Code1_Plug ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block-type(0x%x) not iodblock-plug-req"
				, ctrl.block_type
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 0); /* wrong block-type */
		}

		plg_rel_alarm = cm_sv_ar_almi_prm_end_indication(argr, ctrl.alarm_sequence_number);

		if( cm_is_null(plg_rel_alarm) ) {

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT);

			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 5); /* protocol-violation, tell wrong aruuid */
		}
	}

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

		cm_sv_ar_cmi_timer_stop(argr); /* trigger: rpc-prm-end.ind */

		/***/

		if( CM_SV_AR_IS_RTCLASS3(argr) ) {

			if( CM_SV_AR_IS_ADVANCED(argr) ) {

				/* do nothing */
			}
			else {

				argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_RETRIGGER_TIMER;

				/* more after pdev-commit, see in cm_sv_ar_prov_cons_on_prmend() */
			}
		}
		else {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DO_CONS_12UDP) != 0 ) {

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_DO_CONS_12UDP;

				/* compare to cm_sv_rpc_record_dispatch() */

				if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) != 0 ) {

					CM_FATAL();
				}
				else {

					if( CM_SV_AR_IS_ADVANCED(argr) ) {

						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
						cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE, 0);
					}
					else {

						argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_RETRIGGER_TIMER; /* consumer DHT takes over */

						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
						cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
					}
				}
			}
		}
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) { /* TIA 2329293 */

		cm_sv_arset_check_primary(argr);

		if(
			(argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_BACKUP) != 0 /* started in "backup" (i.e., possibly no pd-prm-begin), TIA 2329293 */
			&& argr == argr->cm.sv.arset->primary /* but ended in "primary" (must not do pd-prm-end) */
		) {

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT);

			return CM_PNIO_FAULTY_IODControlConnectPlug(code1, 5); /* protocol-violation, tell wrong aruuid */
		}
	}

	/***/

	if(
		(argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != CM_SV_AR_FL_EXP_PRM_END
		|| (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_ZCNT) != 0
		|| argr->cm.sv.pr_appl_ready_cnt != 0
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE prm_end_rqb;

		argr->cm.sv.curr_rpc_ind = rpc; /* pending */
		rpc = LSA_NULL; /* sanity */

		argr->cm.sv.pr_appl_ready_cnt += 1; /* one for cm_sv_opc_ar_prm_end_response() */

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			if( cm_sv_ar_is_pdev_owner(argr) ) {

				if( dev->PDevPrmInd_enabled ) {

					cm_sv_ar_pdevprm_indication(argr);

					prm_end_rqb = LSA_NULL;
				}
				else {

					if( cm_is_not_null(argr->cm.sv.arset) && ! cm_sv_arset_is_first(argr) ) {

						/* case: first AR dies before prm-end of backup AR; backup AR becomes pdev-owner; records were ignored... */

						CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "dev/ar(%u/%u) not first AR, not signalling prm-end to cm-pd"
							, argr->device_nr, argr->ar_nr
							);

						prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_FALSE); /* connect-phase (rpc-control.ind) */
					}
					else { /* normal AR or SR-AR "first AR" */

						cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), connect-phase */

						if(
							argr->cm.sv.r1_flags != 0
							&& dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
						) {

							cm_sv_pd_prm_end_remote_request(argr, LSA_NULL, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), connect-phase */
						}

						prm_end_rqb = LSA_NULL;
					}
				}
			}
			else {

				prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_FALSE); /* connect-phase (rpc-control.ind) */
			}
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

			LSA_BOOL is_parked = cm_sv_rpc_park(argr, rpc, LSA_FALSE); /* TIA 1518690 */

			if( is_parked ) {

				return CM_PNIO_ERR_NONE;
			}

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) != 0 ) {

				/* note: before COC (V6.0i4x, TIA 1092258) the PDEV was not reparameterized (CRC64 to detect changed parameters, AP01238541) */

				if( cm_sv_ar_is_pdev_owner(argr) ) {

					if( dev->PDevPrmInd_enabled ) {

						cm_sv_ar_pdevprm_indication(argr); /* supported since V7.0i2.05 (TIA 2227906) */

						prm_end_rqb = LSA_NULL;
					}
					else {

						if( cm_is_not_null(argr->cm.sv.arset) && argr->cm.sv.arset->primary != argr ) {

							CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
								, "dev/ar(%u/%u) PBE but not primary AR, not signalling prm-end to cm-pd"
								, argr->device_nr, argr->ar_nr
								);

							prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* prm-begin-end (PDEV but not primary AR) */
						}
						else {

							if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL) != 0 ) {

								cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_TRUE, argr->cm.sv.local_pdev_if); /* trigger: cm_sv_rpc_control_indication(), prm-begin-end */
							}
							else if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE) != 0 ) {

								if(
									argr->cm.sv.r1_flags != 0
									&& dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
								) {

									cm_sv_pd_prm_end_remote_request(argr, LSA_NULL, LSA_TRUE, argr->cm.sv.remote_pdev_if); /* trigger: cm_sv_rpc_control_indication(), prm-begin-end */
								}
							}
							else { /* PrmUpdate(ALL) */

								cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_TRUE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), prm-begin-end */

								if(
									argr->cm.sv.r1_flags != 0
									&& dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
								) {

									cm_sv_pd_prm_end_remote_request(argr, LSA_NULL, LSA_TRUE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), prm-begin-end */
								}
							}

							prm_end_rqb = LSA_NULL;
						}
					}
				}
				else {

					prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* prm-begin-end (PDEV but not pdev-owner) */
				}
			}
			else {

				prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* prm-begin-end (not PDEV) */
			}
		}
		else { /* data-phase */

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

			if( cm_is_not_null(plg_rel_sub) ) {

				LSA_BOOL is_parked = cm_sv_rpc_park(argr, rpc, LSA_FALSE); /* TIA 1518690 */

				if( is_parked ) {

					return CM_PNIO_ERR_NONE;
				}

				if( dev->PDevPrmInd_enabled ) {
					/* the pdev-interface is not pulled-and-plugged */
				}

				if( CM_SV_SUBSLOT_IS_PDEV(plg_rel_sub->subslot_nr) ) {

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, plg_rel_sub->subslot_nr) ) {

						if( CM_SV_SUBSLOT_IS_PDEV_PORT(plg_rel_sub->subslot_nr) ) {

							cm_sv_pd_prm_end_request(argr, plg_rel_sub, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), plug-phase */
						}
						else {

							if(
								argr->cm.sv.r1_flags != 0
								&& CM_SV_SUBSLOT_IS_PDEV_INTERFACE(plg_rel_sub->subslot_nr)
							) { /* TIA 2037122 */

								cm_sv_pd_prm_end_request(argr, plg_rel_sub, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), plug-phase */
							}
							else {

								CM_FATAL(); /* a bug */
							}
						}

						prm_end_rqb = LSA_NULL;
					}
					else { /* remote pdev */

						if(
							argr->cm.sv.r1_flags != 0
							&& dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
						) {

							cm_sv_pd_prm_end_remote_request(argr, plg_rel_sub, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_rpc_control_indication(), plug-phase */

							prm_end_rqb = LSA_NULL;
						}
						else {

							prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-phase (and bridge-broken) */
						}
					}
				}
				else { /* normal submodule */

					prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-phase */
				}
			}
			else {

				prm_end_rqb = LSA_NULL;

				CM_FATAL();
			}
		}

		/***/

		if( cm_is_not_null(prm_end_rqb) ) {

			cm_sv_opc_ar_prm_end_response(argr->ar_com.channel, prm_end_rqb); /* fake */
		}
	}

	return CM_PNIO_ERR_NONE;

	/***/

rerun:

	{
	LSA_UINT16 block_type;

	data_ptr = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;

	if( code1 == CM_PNIO_FAULTY_IODControl_Code1_Connect ) { /* do not use CM_SV_AR_FL_CONNECT_PHASE in case of rerun! */

		block_type = CM_BLOCK_TYPE_IODBLOCK_CONN_RSP;
	}
	else {

		block_type = CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP;
	}

	CM_PUT16_HTON(data_ptr
		, CM_BLK_HEADER_type
		, block_type
		);

	CM_PUT16_HTON(data_ptr
		, CM_BLK_CTRL_control_command
		, CM_CONTROL_COMMAND_DONE
		);

	cm_sv_rpc_call_response(dev, rpc, CLRPC_OK
		, CM_PNIO_ERR_NONE
		, CLRPC_PNIO_OFFSET + CM_BLK_CTRL_SIZE
		);
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== AR PDEVPRM INDICATION =================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_pdevprm_indication(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PDEVPRM_RSP) != 0
		|| cm_is_not_null(argr->cm.sv.curr_ar_pdevprm)
		|| argr->cm.sv.pr_appl_ready_cnt != 1
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pdevprm_rqb = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_PDEVPRM_IND);

		pdevprm_rqb->args.sv.ar_event->u.sv.ar_pdevprm = 0; /* sanity */

		/***/

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PDEVPRM_RSP;

		cm_callback(argr->ar_com.channel, CM_OK, &pdevprm_rqb);
	}
}

/*===========================================================================*/
/*=== AR PDEVPRM RESPONSE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_pdevprm_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_PDEVPRM_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else if( cm_is_not_null(argr->cm.sv.curr_ar_pdevprm) ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) curr_ar_pdevprm(0x%08x) cm-user did not wait for pdevprm-read/write.cnf (cannot report)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.curr_ar_pdevprm
			);

		CM_FATAL(); /* cm-user did not wait for pdevprm-read/write.cnf (cannot report) */
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) cn_flags(0x%08x) pdevprm.rsp"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags
			);

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PDEVPRM_RSP;

		/***/

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		if( argr->cm.sv.pr_appl_ready_cnt != 1 ) {
			CM_FATAL(); /* a bug */
		}

		if( cm_ar_com_is_down(argr) ) {

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

				cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK, CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(), 0/*nice*/);
			}

			/***/

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PDEVPRM_RSP and curr_rpc_ind == LSA_NULL (prm-end) */
		}
		else {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

				if( cm_sv_ar_is_pdev_owner(argr) ) {

					cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_FALSE, LSA_NULL); /* trigger: cm_sv_opc_ar_pdevprm_response() */
				}
				else {

					CM_FATAL(); /* a bug */
				}
			}
			else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

				if( cm_is_not_null(argr->cm.sv.arset) ) { /* note: no checks for first/primary/backup */

					if( cm_sv_ar_is_pdev_owner(argr) ) {

						if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL) != 0 ) {

							cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_TRUE, argr->cm.sv.local_pdev_if); /* trigger: cm_sv_opc_ar_pdevprm_response() */
						}
						else {

							cm_sv_pd_prm_end_request(argr, LSA_NULL, LSA_TRUE, LSA_NULL); /* trigger: cm_sv_opc_ar_pdevprm_response() */
						}
					}
					else { /* TIA 2384054 switchover between pdevprm.ind and pdevprm.rsp */

						CM_UPPER_RQB_PTR_TYPE prm_end_rqb;

						prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* prm-begin-end (PDEV but not pdev-owner) */

						if( cm_is_not_null(prm_end_rqb) ) {

							cm_sv_opc_ar_prm_end_response(argr->ar_com.channel, prm_end_rqb); /* fake */
						}
					}
				}
				else {

					CM_FATAL(); /* unreachable, see cm_sv_rpc_control_indication() */
				}
			}
			else { /* data-phase */

				CM_FATAL();
			}
		}
	}
}

/*===========================================================================*/
/*=== AR PDEV SET PRM RES ===================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_pdev_set_prm_end_elems(
	CM_PD_AR_PRM_END_PTR_TYPE prm_end_args_local,
	CM_PD_AR_PRM_END_63P_PTR_TYPE prm_end_args_remote, /* R1 */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE plg_rel_sub /* LSA_NULL: all pdev submodules, else: this pdev submodule */
) {
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap = cm_ar_graph_find_ap(argr, 0);
	CM_PD_AR_PRM_END_63P_PTR_TYPE prm_end_args;
	LSA_UINT32 max_nr_of_elems;

	if( cm_is_not_null(prm_end_args_local) ) {

		prm_end_args = (CM_PD_AR_PRM_END_63P_PTR_TYPE)prm_end_args_local; /* the 63P struct contains the "local" struct */

		max_nr_of_elems = argr->cm.sv.nr_of_pdev_submodules;

		if( max_nr_of_elems > (sizeof(prm_end_args_local->elem) / sizeof(prm_end_args_local->elem[0])) ) {
			CM_FATAL();
		}
	}
	else if( cm_is_not_null(prm_end_args_remote) ) {

		prm_end_args = prm_end_args_remote;

		max_nr_of_elems = argr->cm.sv.nr_of_remote_pdev_submodules;

		if( max_nr_of_elems > (sizeof(prm_end_args_remote->elem) / sizeof(prm_end_args_remote->elem[0])) ) {
			CM_FATAL();
		}
	}
	else {

		CM_FATAL();
		return;
	}

	if( max_nr_of_elems > (sizeof(prm_end_args->elem) / sizeof(prm_end_args->elem[0])) ) {
		CM_FATAL();
	}

	prm_end_args->nr_of_elems = 0;

	if( cm_is_not_null(ar_ap) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				if( (cm_is_not_null(prm_end_args_remote) && CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, su->subslot_nr) )
					|| (cm_is_not_null(prm_end_args_local) && CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, su->subslot_nr) )
				) {

					if( cm_is_null(plg_rel_sub) || ar_sub == plg_rel_sub ) {

						LSA_UINT16 prm_end_flags = 0;

						if( ! CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

							prm_end_flags |= CM_PD_PRM_END_PULLED;
						}

						if( su->owner_ar_nr != argr->ar_nr ) {

							prm_end_flags |= CM_PD_PRM_END_LOCKED;
						}

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_WRONG) != 0 ) {

							prm_end_flags |= CM_PD_PRM_END_WRONG; /* note: wrong may follow from locked */
						}

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_ARP) != 0 ) {

							/* unreachable (function runs before user-appl-ready) */
						}

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) {

							prm_end_flags |= CM_PD_PRM_END_DIRTY;
						}

						if( (ar_sub->sv.err_flags & CM_PD_PRM_END_WRITE) != 0 ) {

							prm_end_flags |= CM_PD_PRM_END_WRITE; /* err_index and err_offset are valid */
						}

						/***/

						if( prm_end_args->nr_of_elems >= max_nr_of_elems ) {

							CM_FATAL();
						}
						else {

							if(prm_end_flags != 0) {

								CM_PD_AR_PRM_END_ELEM_PTR_TYPE elem = &prm_end_args->elem[prm_end_args->nr_of_elems];

								elem->slot_nr    = ar_mod->slot_nr;
								elem->subslot_nr = ar_sub->subslot_nr;
								elem->flags      = prm_end_flags;
								elem->index      = ar_sub->sv.err_index;
								elem->offset     = ar_sub->sv.err_offset;

								prm_end_args->nr_of_elems += 1;
							}
						}
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== PD PRM-END REQUEST ====================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_pd_prm_end_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE plg_rel_sub,
	LSA_BOOL is_prm_begin_end,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE local_pdev_if /* only used for PBE-IF*/
) {
	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PD_PRM_END_CNF) != 0
		|| argr->cm.sv.pr_appl_ready_cnt != 1
	) {

		CM_FATAL();
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.pdev_end_rqb;

		if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_UPPER_PD_PRM_END_PTR_TYPE pd_prm_end = pd->args.pd.prm_end;
			CM_PD_AR_PRM_END_PTR_TYPE prm_end_args = &argr->cm.sv.prm_end_args;

			pd_prm_end->set_mrp_off = argr->cm.sv.set_mrp_off ? CM_SET_MRP_OFF_YES : CM_SET_MRP_OFF_NO; /* AP00824058 */
			pd_prm_end->sv.void_ptr = prm_end_args;

			if( cm_is_not_null(argr->ir_info) ) {

				prm_end_args->ir_data_uuid = argr->ir_info->ir_data_uuid;
			}
			else {

				prm_end_args->ir_data_uuid = cm_uuid_nil_uuid; /* note: NIL if not RTC3 advanced mode */
			}

			prm_end_args->max_alarm_data_length = cm_sv_ar_almi_send_max_length(argr, CM_ALARM_TYPE_DIAGNOSIS);

			if( cm_is_not_null(plg_rel_sub) ) {

				prm_end_args->slot_nr    = plg_rel_sub->mod->slot_nr;
				prm_end_args->subslot_nr = plg_rel_sub->subslot_nr; /* a local port */
			}
			else if( is_prm_begin_end ) {

				if( cm_is_not_null(local_pdev_if) ) {

					prm_end_args->slot_nr    = local_pdev_if->mod->slot_nr;
					prm_end_args->subslot_nr = local_pdev_if->subslot_nr; /* PBE-IF */
				}
				else {

					prm_end_args->slot_nr    = 0;
					prm_end_args->subslot_nr = 0; /* PBE-ALL */
				}
			}
			else {

				prm_end_args->slot_nr    = 0;
				prm_end_args->subslot_nr = 0; /* connect (all) */
			}

			cm_sv_ar_pdev_set_prm_end_elems(prm_end_args, LSA_NULL, argr, plg_rel_sub); /* local PDEV */

			/***/

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PD_PRM_END_CNF;

			cm_sv_pdev_request(argr->ar_com.channel, CM_OPC_PD_PRM_END_SERVER, argr->device_nr, argr->ar_nr, pd);
		}
	}
}

/*===========================================================================*/
/*=== PD PRM-END REMOTE REQUEST =============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_pd_prm_end_remote_request(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE plg_rel_sub,
	LSA_BOOL is_prm_begin_end,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE remote_pdev_if /* only used for PBE-IF*/
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

	CM_ASSERT(dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED); /* see callers */

	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_END_REMOTE) != 0
		|| argr->cm.sv.pr_appl_ready_cnt != 1
		|| (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_REMOTE_PDEV_INTERFACE) == 0 /* see cm_sv_argr_build() */
	) {

		CM_FATAL();
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.r1b.remote_pdev_end_rqb;

		if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_UPPER_PD_PRM_END_PTR_TYPE pd_prm_end = pd->args.pd.prm_end;
			CM_PD_AR_PRM_END_63P_PTR_TYPE prm_end_args = argr->cm.sv.r1b.remote_prm_end_args;

			pd_prm_end->set_mrp_off = CM_SET_MRP_OFF_NO; /* see cm_sv_ar_ownership_conn_response(), TIA 1434352 */
			pd_prm_end->sv.void_ptr = (LSA_VOID_PTR_TYPE)prm_end_args;

			if( cm_is_not_null(argr->ir_info) ) {
				CM_FATAL();
			}

			prm_end_args->ir_data_uuid = cm_uuid_nil_uuid;

			prm_end_args->max_alarm_data_length = cm_sv_ar_almi_send_max_length(argr, CM_ALARM_TYPE_DIAGNOSIS);

			if( cm_is_not_null(plg_rel_sub) ) {

				prm_end_args->slot_nr    = plg_rel_sub->mod->slot_nr;
				prm_end_args->subslot_nr = plg_rel_sub->subslot_nr;
			}
			else if( is_prm_begin_end ) {

				if( cm_is_not_null(remote_pdev_if) ) {

					prm_end_args->slot_nr    = remote_pdev_if->mod->slot_nr;
					prm_end_args->subslot_nr = remote_pdev_if->subslot_nr; /* PBE-IF */
				}
				else {

					prm_end_args->slot_nr    = 0;
					prm_end_args->subslot_nr = 0; /* PBE-ALL */
				}
			}
			else {

				prm_end_args->slot_nr    = 0;
				prm_end_args->subslot_nr = 0; /* connect (all) */
			}

			cm_sv_ar_pdev_set_prm_end_elems(LSA_NULL, prm_end_args, argr, plg_rel_sub); /* remote PDEV */

			/***/

			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_PRM_END_REMOTE;

			cm_sv_r1b_pdev_A_request(dev, CM_OPC_PD_PRM_END_SERVER, argr->ar_nr, pd); /* prm-end */
		}
	}
}

/*===========================================================================*/
/*=== ABORT ARs WITH "IR-PROBLEM" ===========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_dev_abort_ir_problem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {
	CLRPC_UUID_PTR_TYPE pdev_ir_data_uuid = cm_pdusr_get_ir_data_uuid(dev->channel);

	/* if PDEV-IRDataUUID != NIL must abort all ARs with IR-problem and same IRDataUUID */

	if( ! cm_uuid_is_nil(pdev_ir_data_uuid) ) {

		LSA_UINT16 ar_nr;

		for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

			CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

			if( cm_is_not_null(argr) ) {

				if( CM_SV_AR_IS_RTCLASS3(argr) && CM_SV_AR_IS_ADVANCED(argr) && cm_is_not_null(argr->ir_info) ) {

					if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

						if( cm_uuid_equal(&argr->ir_info->ir_data_uuid, pdev_ir_data_uuid) ) {

							if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

								/* skip, this AR has a PO-problem */
							}
							else {

								cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PDEV, 0);
								cm_ar_com_down(argr, CM_AR_REASON_PDEV);
							}
						}
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== PD PRM SET PRMED-FLAG =================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_pd_prm_set_prmed_flag(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub,
	LSA_BOOL is_remote_pdev,
	LSA_BOOL is_prm_begin_end
) {
	LSA_UINT16 smfl_mask = (CM_SV_AR_SMFL_DIRTY | CM_SV_AR_SMFL_WRONG | CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP);

	if( cm_is_not_null(ar_sub) ) { /* PBE-IF, Plug-IF, Plug-Port */

		if( ! CM_SV_SUBSLOT_IS_PDEV(ar_sub->subslot_nr) ) {
			CM_FATAL(); /* a bug */
		}

		if( is_prm_begin_end && (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) == 0 ) {
			CM_FATAL(); /* a bug */
		}

		if( (ar_sub->sv.sm_flags & smfl_mask) == 0
			&& CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
			&& ar_sub->sv.real_su->owner_ar_nr == argr->ar_nr
		) {
			ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED; /* parameterized */
		}
	}
	else {

		CM_AR_GRAPH_AP_PTR_TYPE ar_ap = cm_ar_graph_find_ap(argr, 0);

		if( cm_is_not_null(ar_ap) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					if( is_remote_pdev ) {

						if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, ar_sub->subslot_nr) ) {

							if( is_prm_begin_end && (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) == 0 ) {
								CM_FATAL(); /* a bug */
							}

							if( (ar_sub->sv.sm_flags & smfl_mask) == 0
								&& CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
								&& ar_sub->sv.real_su->owner_ar_nr == argr->ar_nr
							) {
								ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED; /* parameterized */
							}
						}
					}
					else { /* local */

						if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, ar_sub->subslot_nr) ) {

							if( is_prm_begin_end && (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) == 0 ) {
								CM_FATAL(); /* a bug */
							}

							if( (ar_sub->sv.sm_flags & smfl_mask) == 0
								&& CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
								&& ar_sub->sv.real_su->owner_ar_nr == argr->ar_nr
							) {
								ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED; /* parameterized */
							}
						}
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== PD PRM-END CONFIRMATION ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_end_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	LSA_UINT16 pd_rsp = CM_RQB_GET_RESPONSE(pd);

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) ar_flags(0x%08x) rsp(%u)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags, pd_rsp
		);

	if(
		(argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PRM_BEGIN) != 0 /* note: no explicit wait-state for prm-begin */
		|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PD_PRM_END_CNF) == 0
		|| pd != argr->cm.sv.pdev_end_rqb
		|| pd->args.pd.prm_end->sv.void_ptr != (LSA_VOID_PTR_TYPE)&argr->cm.sv.prm_end_args
		|| argr->cm.sv.pr_appl_ready_cnt != 1
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT16 pp = (argr->cm.sv.prm_end_args.subslot_nr & 0x00FF); /* != 0 if port */

		if( pp != 0 ) { /* port */

			if( pd_rsp != CM_OK ) {

				CM_FATAL(); /* a bug in CM-PD */
			}
		}
		else { /* "all" or interface */

			if( pd_rsp != CM_OK ) { /* not committed */

				CM_ASSERT(pd_rsp == CM_ERR_PRM_CONSISTENCY);

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_PDEV_INVALID; /* note: the empty parameterization is active */
			}
			else { /* committed */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) != 0 ) { /* TIA 1355739 */

					argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PDEV_INVALID;
				}

				/* note: if AR-IRDataUUID != NIL then PDEV-IRDataUUID == AR-IRDataUUID, see consistency check in CM_OPC_PD_PRM_END_SERVER */

				cm_sv_dev_abort_ir_problem(argr->cm.sv.dev);
			}
		}

		/***/

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PD_PRM_END_CNF; /* note: reset after cm_sv_ar_abort_ir_problem() */

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_END_REMOTE) != 0 ) { //R1 marker (join pd-prm-end-remote)
				return;
			}

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

				cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK, CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(), 0/*nice*/);
			}

			/***/

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PD_PRM_END_CNF and curr_rpc_ind == LSA_NULL (prm-end) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE prm_end_rqb;

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

				CM_ASSERT(pp == 0);

				cm_sv_pd_prm_set_prmed_flag(argr, LSA_NULL, LSA_FALSE/*local*/, LSA_FALSE); /* connect, local */

				prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_FALSE); /* connect-phase (pd-prm-end.cnf) */
			}
			else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

				/* note: before COC (V6.0i4x, TIA 1092258) the PDEV was not reparameterized (CRC64 to detect changed parameters, AP01238541) */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) != 0 ) {

					CM_ASSERT(pp == 0);

					if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL) != 0 ) {
						cm_sv_pd_prm_set_prmed_flag(argr, argr->cm.sv.local_pdev_if, LSA_FALSE/*local*/, LSA_TRUE); /* PBE-IF, local */
					}
					else {
						cm_sv_pd_prm_set_prmed_flag(argr, LSA_NULL, LSA_FALSE/*local*/, LSA_TRUE); /* PBE-ALL, local */
					}

					prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* prm-begin-end (pd-prm-end.cnf) */
				}
				else {

					prm_end_rqb = LSA_NULL;

					CM_FATAL(); /* a bug */
				}
			}
			else { /* data-phase */

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

				if( cm_is_not_null(plg_rel_sub) ) {

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, plg_rel_sub->subslot_nr) ) {

						if( CM_SV_SUBSLOT_IS_PDEV_PORT(plg_rel_sub->subslot_nr) ) {

							cm_sv_pd_prm_set_prmed_flag(argr, plg_rel_sub, LSA_FALSE/*local*/, LSA_FALSE); /* plug-port, local */

							prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-port (pd-prm-end.cnf) */
						}
						else {

							if(
								argr->cm.sv.r1_flags != 0
								&& CM_SV_SUBSLOT_IS_PDEV_INTERFACE(plg_rel_sub->subslot_nr)
							) { /* TIA 2037122 */

								cm_sv_pd_prm_set_prmed_flag(argr, plg_rel_sub, LSA_FALSE/*local*/, LSA_FALSE); /* plug-interface local */

								prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-interface (pd-prm-end.cnf) */
							}
							else {

								prm_end_rqb = LSA_NULL;

								CM_FATAL(); /* a bug */
							}
						}
					}
					else { /* normal submodule or remote pdev */

						prm_end_rqb = LSA_NULL;

						CM_FATAL(); /* a bug */
					}
				}
				else {

					prm_end_rqb = LSA_NULL;

					CM_FATAL(); /* a bug */
				}
			}

			/***/

			if( cm_is_not_null(prm_end_rqb) ) {

				cm_sv_opc_ar_prm_end_response(argr->ar_com.channel, prm_end_rqb); /* fake */
			}
		}
	}
}

/*===========================================================================*/
/*=== PD PRM-END REMOTE CONFIRMATION ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_end_remote_confirmation( //R1 VV: move parts to "PDEV_A", make a single DONE function with flag local/remote
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	LSA_UINT16 pd_rsp = CM_RQB_GET_RESPONSE(pd);

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
	, "dev/ar(%u/%u) r1_flags(0x%08x) pd-rsp(%u)"
	, argr->device_nr, argr->ar_nr
	, argr->cm.sv.r1_flags, pd_rsp
	);

	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE) != 0
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_END_REMOTE) == 0
		|| pd != argr->cm.sv.r1b.remote_pdev_end_rqb
		|| pd->args.pd.prm_end->sv.void_ptr != argr->cm.sv.r1b.remote_prm_end_args
		|| argr->cm.sv.pr_appl_ready_cnt != 1
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_PRM_END_REMOTE;

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		/***/

		if( (argr->cm.sv.r1b.remote_prm_end_args->subslot_nr & 0x00FF) == 0x0000 ) { /* connect or PBE-ALL or PBE-IF */

			if( pd_rsp != CM_OK ) {
				argr->cm.sv.pd_flags |= CM_SV_AR_PDFL_REMOTE_PDEV_PRM_FAULT;
			}
			else {
				argr->cm.sv.pd_flags &= ~CM_SV_AR_PDFL_REMOTE_PDEV_PRM_FAULT;
			}
		}

		/***/

		if( cm_ar_com_is_down(argr) ) {

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PD_PRM_END_CNF) != 0 ) { //R1 marker (join pd-prm-end-remote)
				return;
			}

			//R1 VV: make a function "cancel prm-end-rpc"
			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

				cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK, CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(), 0/*nice*/);
			}

			/***/

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_PRM_END_REMOTE and curr_rpc_ind == LSA_NULL (prm-end) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE prm_end_rqb;

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

				cm_sv_pd_prm_set_prmed_flag(argr, LSA_NULL, LSA_TRUE/*remote*/, LSA_FALSE); /* connect-phase (pd-prm-end-remote.cnf) */

				prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_FALSE); /* connect-phase (pd-prm-end-remote.cnf) */
			}
			else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end (pd-prm-end-remote.cnf) */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) != 0 ) {

					if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE) != 0 ) {
						cm_sv_pd_prm_set_prmed_flag(argr, argr->cm.sv.remote_pdev_if, LSA_TRUE/*remote*/, LSA_TRUE); /* PBE-IF, remote */
					}
					else {
						cm_sv_pd_prm_set_prmed_flag(argr, LSA_NULL, LSA_TRUE/*remote*/, LSA_TRUE); /* PBE-ALL, remote */
					}

					prm_end_rqb = cm_sv_ar_prm_end_indication(argr, LSA_NULL, LSA_TRUE); /* pbe-phase (pd-prm-end-remote.cnf) */
				}
				else {

					prm_end_rqb = LSA_NULL;

					CM_FATAL(); /* a bug */
				}
			}
			else { /* data-phase */

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

				if( cm_is_not_null(plg_rel_sub) ) {

					if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, plg_rel_sub->subslot_nr) ) {

						if( CM_SV_SUBSLOT_IS_PDEV_PORT(plg_rel_sub->subslot_nr) ) {

							cm_sv_pd_prm_set_prmed_flag(argr, plg_rel_sub, LSA_TRUE/*remote*/, LSA_FALSE); /* plug port, remote */

							prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-port (pd-prm-end-remote.cnf) */
						}
						else {

							cm_sv_pd_prm_set_prmed_flag(argr, plg_rel_sub, LSA_TRUE/*remote*/, LSA_FALSE); /* plug IF, remote */

							prm_end_rqb = cm_sv_ar_prm_end_indication(argr, plg_rel_sub, LSA_FALSE); /* plug-interface (pd-prm-end-remote.cnf) */
						}
					}
					else { /* normal submodule or local pdev */

						prm_end_rqb = LSA_NULL;

						CM_FATAL(); /* a bug */
					}
				}
				else {

					prm_end_rqb = LSA_NULL;

					CM_FATAL(); /* a bug */
				}
			}

			/***/

			if( cm_is_not_null(prm_end_rqb) ) {

				cm_sv_opc_ar_prm_end_response(argr->ar_com.channel, prm_end_rqb); /* fake */
			}
		}
	}
}

/*===========================================================================*/
/*=== AR PRM-END INDICATION =================================================*/
/*===========================================================================*/

static CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_prm_end_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub,
	LSA_BOOL is_prm_begin_end
) {
	LSA_BOOL is_arset_backup = LSA_FALSE;
	CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end;

	/***/

	//R1 VV: make a join-function that calls this function.

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PD_PRM_END_CNF) != 0 ) {
		return LSA_NULL;
	}

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_END_REMOTE) != 0 ) {
		return LSA_NULL;
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		cm_sv_arset_check_primary(argr);

		if(
			! cm_sv_arset_is_primary_or_first(argr)
			|| (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_BACKUP) != 0 /* TIA 2329293 */
		) {

			is_arset_backup = LSA_TRUE;
		}
	}

	/***/

	if(
		(argr->cm.sv.ar_flags & (CM_SV_AR_FL_PRM_END_IND | CM_SV_AR_FL_PRM_END_RSP)) != 0
		|| argr->cm.sv.pr_appl_ready_cnt != 1
		|| cm_is_null(argr->cm.sv.ar_prm_end_mem)
	) {

		CM_FATAL();
	}

	argr->cm.sv.ar_flags |= CM_SV_AR_FL_PRM_END_IND;

	/***/

	ar_prm_end = argr->cm.sv.ar_prm_end_mem;

	if( cm_is_null(plg_rel_sub) ) { /* connect-phase or prm-begin-end */

		if( is_prm_begin_end ) {
			ar_prm_end->ar_context = CM_SV_AR_CONTEXT_PLUG;
		}
		else {
			ar_prm_end->ar_context = CM_SV_AR_CONTEXT_CONN;
		}

		ar_prm_end->nr_of_elems = 0;

		{
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

		for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					if( is_prm_begin_end ) {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) == 0 ) {

							continue; /* skip */
						}

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE_PLG_REL) != 0 ) { /* AP01525387 */

							continue; /* skip */
						}
					}

					/***/

					if(
						! CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
						|| ar_sub->sv.real_su->owner_ar_nr != argr->ar_nr
						|| (ar_sub->sv.sm_flags & (CM_SV_AR_SMFL_WRONG | CM_SV_AR_SMFL_DIRTY)) != 0
					) {

						/* exclude */
					}
					else if( is_arset_backup ) {

						/* not: cm_sv_ar_submodule_state_changed() -- would do pull/plug on new primary AR */

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_CIR) != 0 ) {

							/* CIR: kind of "pulled", see cm_sv_marshal_module_diff_block() */
						}
						else {

							ar_sub->sv.sm_flags |= (CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP);
						}

						/* exclude */
					}
					else {

						CM_UPPER_SV_AR_PRM_END_ELEMENT_PTR_TYPE elem = &ar_prm_end->elem[ar_prm_end->nr_of_elems];

						elem->api        = ar_ap->api;
						elem->slot_nr    = ar_mod->slot_nr;
						elem->subslot_nr = ar_sub->subslot_nr;

						elem->appl_ready_follows = CM_SV_APPL_READY_FOLLOWS_NO; /* sanity */
						elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES;  /* sanity */

						if( CM_SV_SUBSLOT_IS_PDEV(elem->subslot_nr) ) { /* report result of pdev-prm (V6.0i2.1x, TIA 1193013) */

							elem->appl_ready_follows = CM_SV_APPL_READY_FOLLOWS_NO; /* see documentation */

							if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, elem->subslot_nr) ) {

								if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PDEV_INVALID) != 0 ) {
									elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES; /* note: info only (TIA 1403121) */
								}
								else {
									elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_NO; /* note: info only (TIA 1403121) */
								}
							}
							else { /* remote pdev */

								if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_REMOTE_PDEV_PRM_FAULT) != 0 ) {
									elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES; /* note: info only (TIA 1403121) */
								}
								else {
									elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_NO; /* note: info only (TIA 1403121) */
								}
							}
						}

						ar_prm_end->nr_of_elems += 1;

						/***/

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WF_PRMEND;
					}
				}
			}
		}
		}

		CM_ASSERT(ar_prm_end->nr_of_elems <= argr->cm.sv.nr_of_submodules);
	}
	else { /* plug-phase */

		ar_prm_end->ar_context = CM_SV_AR_CONTEXT_PLUG;

		ar_prm_end->nr_of_elems = 0;

		if( cm_is_not_null(plg_rel_sub) ) { /* always true */

			if(
				! CM_SV_SUBSLOT_IS_PLUGGED(plg_rel_sub->sv.real_su)
				|| plg_rel_sub->sv.real_su->owner_ar_nr != argr->ar_nr
				|| (plg_rel_sub->sv.sm_flags & (CM_SV_AR_SMFL_WRONG | CM_SV_AR_SMFL_DIRTY)) != 0
			) {

				/* exclude */
			}
			else if( is_arset_backup ) {

				/* not: cm_sv_ar_submodule_state_changed() -- repetition of plug-alarm is not done here */

				plg_rel_sub->sv.sm_flags |= (CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP);

				/* exclude */
			}
			else {

				CM_UPPER_SV_AR_PRM_END_ELEMENT_PTR_TYPE elem = &ar_prm_end->elem[ar_prm_end->nr_of_elems];

				elem->api        = plg_rel_sub->mod->ap->api;
				elem->slot_nr    = plg_rel_sub->mod->slot_nr;
				elem->subslot_nr = plg_rel_sub->subslot_nr;

				elem->appl_ready_follows = CM_SV_APPL_READY_FOLLOWS_NO; /* sanity */
				elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES;  /* sanity */

				if( CM_SV_SUBSLOT_IS_PDEV(elem->subslot_nr) ) { /* report result of pdev-prm (V6.0i2.1x, TIA 1193013) */

					elem->appl_ready_follows = CM_SV_APPL_READY_FOLLOWS_NO; /* see documentation */

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, elem->subslot_nr) ) {

						if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PDEV_INVALID) != 0 ) {
							elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES; /* note: info only (TIA 1403121) */
						}
						else {
							elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_NO; /* note: info only (TIA 1403121) */
						}
					}
					else { /* remote pdev */

						if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_REMOTE_PDEV_PRM_FAULT) != 0 ) {
							elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_YES; /* note: info only (TIA 1403121) */
						}
						else {
							elem->appl_ready_pending = CM_SV_APPL_READY_PENDING_NO; /* note: info only (TIA 1403121) */
						}
					}
				}

				ar_prm_end->nr_of_elems += 1;

				/***/

				plg_rel_sub->sv.sm_flags |= CM_SV_AR_SMFL_WF_PRMEND;
			}
		}

		CM_ASSERT(ar_prm_end->nr_of_elems <= 1);
	}

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PRMEND_RSP) != 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PRMEND_RSP;

		/***/

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) prm-end.ind"
			, argr->device_nr, argr->ar_nr
			);

		{
		CM_UPPER_RQB_PTR_TYPE prm_end_rqb = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_PRM_END_IND);

		prm_end_rqb->args.sv.ar_event->u.sv.ar_prm_end = ar_prm_end;

		if( ar_prm_end->nr_of_elems == 0 ) {

			if( ar_prm_end->ar_context == CM_SV_AR_CONTEXT_CONN ) {

				/* prm-end in connect-phase always (AP01503040, V5.1i5.62) */
			}
			else {

				return prm_end_rqb;
			}
		}

		cm_callback(argr->ar_com.channel, CM_OK, &prm_end_rqb);
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AR PRM-END UPDATE =====================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_prm_end_update(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end
) {
	LSA_UINT16 idx;

	for( idx = 0; idx < ar_prm_end->nr_of_elems; ++idx ) {

		CM_UPPER_SV_AR_PRM_END_ELEMENT_PTR_TYPE elem = &ar_prm_end->elem[idx];

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		cm_ar_graph_find_api_slot_subslot(argr, elem->api, elem->slot_nr, elem->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

		if( cm_is_null(ar_sub) ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "ap/sl/su(%u/%u/%u) not in AR"
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		}
		else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_WF_PRMEND) == 0 ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "ap/sl/su(%u/%u/%u) not wf-prm-end"
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		}
		else {

			ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_WF_PRMEND;

			/***/

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ap/sl/su(%u/%u/%u) arp-follows(%u) arp(%u)"
				, elem->api, elem->slot_nr, elem->subslot_nr
				, elem->appl_ready_follows
				, elem->appl_ready_pending
				);

			if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) {

				/* do nothing */
			}
			else {

				if( CM_SV_SUBSLOT_IS_PDEV(elem->subslot_nr) ) { /* see cm_sv_ar_prm_end_indication() (TIA 1193013) */

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_EX_APLRDY) != 0 ) {
						CM_FATAL();
					}

					if( elem->appl_ready_follows == CM_SV_APPL_READY_FOLLOWS_YES ) {

						CM_FATAL(); /* a user-bug, see documentation "must be unchanged in the response" (TIA 1323409) */
					}

					/* note: CM_SV_AR_SMFL_IS_PRMED is set in cm_sv_pd_prm_set_prmed_flag() */
				}
				else {

					if( (ar_sub->sv.sm_flags & (CM_SV_AR_SMFL_EX_APLRDY | CM_SV_AR_SMFL_IS_ARP | CM_SV_AR_SMFL_IS_PRMED)) != 0 ) {
						CM_FATAL();
					}

					if( elem->appl_ready_follows == CM_SV_APPL_READY_FOLLOWS_YES ) {

						argr->cm.sv.pr_appl_ready_cnt += 1;

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_EX_APLRDY;
					}
					else {

						if( elem->appl_ready_pending == CM_SV_APPL_READY_PENDING_YES ) {

							ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_ARP;
						}

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED; /* parameterized */
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== AR PRM-END RESPONSE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_prm_end_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_PRMEND_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else if(
		(argr->cm.sv.ar_flags & (CM_SV_AR_FL_PRM_END_IND | CM_SV_AR_FL_PRM_END_RSP)) != (CM_SV_AR_FL_PRM_END_IND | 0)
		|| param->u.sv.ar_prm_end != argr->cm.sv.ar_prm_end_mem
		|| argr->cm.sv.pr_appl_ready_cnt != 1
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PRMEND_RSP;

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_PRM_END_RSP;

		argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_BACKUP; /* TIA 2329293 */

		/***/

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) prm-end.rsp, cn_flags(0x%08x) ar_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
			);

		/***/

		cm_sv_ar_prm_end_update(argr, param->u.sv.ar_prm_end);

		/***/

		param->u.sv.ar_prm_end = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		rb    = LSA_NULL; /* sanity */
		param = LSA_NULL; /* sanity */

		/***/

		{
		CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
		argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

		if( cm_is_null(rpc) ) {

			CM_FATAL();
		}
		else { /* note: respond ok even if down (was not-down on prm-end.ind) */

			CM_UPPER_MEM_U8_PTR_TYPE data_ptr = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;
			LSA_UINT16 block_type;

			/*
			 * IODControlRes ::= ControlBlockConnect ^ ControlBlockPlug ^ NULL
			 *
			 * In case of a negative response NULL shall be transmitted.
			 */

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

				block_type = CM_BLOCK_TYPE_IODBLOCK_CONN_RSP;
			}
			else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

				block_type = CM_BLOCK_TYPE_IODBLOCK_CONN_RSP;
			}
			else {

				block_type = CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP;
			}

			CM_PUT16_HTON(data_ptr
				, CM_BLK_HEADER_type
				, block_type
				);

			CM_PUT16_HTON(data_ptr
				, CM_BLK_CTRL_control_command
				, CM_CONTROL_COMMAND_DONE
				);

			cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK
				, CM_PNIO_ERR_NONE
				, CLRPC_PNIO_OFFSET + CM_BLK_CTRL_SIZE
				);
			}
		}

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

			if( cm_ar_com_is_down(argr) ) {

				/* do nothing, timer is reused for other things */
			}
			else {

				cm_sv_ar_cmi_timer_trigger(argr); /* trigger: rpc-prm-end.rsp */
			}
		}

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			if( ! cm_ar_com_is_down(argr) ) {

				cm_sv_ar_prov_cons_on_prmend(argr);
			}
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

			{
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

							cm_sv_ar_almi_pbe(argr, ar_sub); /* AP01193236 */
						}
					}
				}
			}
			}
		}
		else { /* data-phase */

			cm_sv_ar_almi_plug_group_done(argr); /* trigger: CM_SV_AR_FL_PRM_END_RSP */
		}

		/***/

		cm_sv_ar_appl_ready_cnt_decrement(argr); /* see cm_sv_rpc_control_indication() */

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PRMEND_RSP and curr_rpc_ind == LSA_NULL (prm-end) */
		}
		else {

			cm_sv_rpc_appl_ready_request(argr); /* trigger: ar-prm-end.rsp */
		}
	}
}

/*===========================================================================*/
/*=== PROVIDER AND CONSUMER ON PRM-END ======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_prov_cons_on_prmend(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* note: connect-phase only */

	if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_AR_PROBLEM | CM_SV_AR_FL_PDEV_INVALID)) != 0 ) {

		/* see too cm_sv_ar_sendclock_changed() */

		if( CM_SV_AR_IS_RTCLASS3(argr) ) {

			if( CM_SV_AR_IS_ADVANCED(argr) ) {

				/* do nothing, the PDEV-submodules are SO-locked or A.R.P., the AR is in rpc-trigger-mode */
			}
			else {

				/* must activate the "green" consumer, the PDEV-submodules are SO-locked or A.R.P. */
				/* note that we cannot activate the "green" consumer only (see too AP01205330) */
				/* note that an IRData-record may exist and may or may not contain this frame-id */
				/* so the AR either stays in "green" or aborts here or cm_sv_ar_received_in_red_indication() will ignore the false signal */

				if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) != 0 ) {

					CM_FATAL();
				}
				else {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
					cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
				}
			}
		}
		else {

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) != 0 ) {

				/* possible, search for CM_SV_AR_FL_DO_CONS_12UDP */
			}

			/* do nothing */
		}
	}
	else { /* neither AR-problem nor PDEV-problem */

		if( CM_SV_AR_IS_RTCLASS3(argr) ) {

			if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_PROV_ACT | CM_SV_AR_CNFL_WF_CONS_ACT)) != 0 ) {

				CM_FATAL();
			}
			else {

				if( CM_SV_AR_IS_ADVANCED(argr) ) {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PROV_ACT; /* sic! */
					cm_arcq_request(argr, CM_ARCQ_OPC_PROV_IOPARAMS_SET, 0);

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
					cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE, 0);
				}
				else {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PROV_ACT;
					cm_arcq_request(argr, CM_ARCQ_OPC_PROV_ACTIVATE, 0); /* note: with config-provider-autostop */

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
					cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
				}
			}
		}
		else {

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) != 0 ) {

				/* possible, search for CM_SV_AR_FL_DO_CONS_12UDP */
			}

			/* do nothing */
		}
	}
}

/*===========================================================================*/
/*=== CONSUMER ACTIVATE DONE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_cons_activate_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) == 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_CONS_ACT;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_CONS_ACT */
		}
		else {

			cm_sv_rpc_appl_ready_request(argr); /* trigger: CM_SV_AR_CNFL_WF_CONS_ACT */
		}
	}
}

/*===========================================================================*/
/*=== APPLICATION-READY FROM USER (ELEMENT) =================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_ar_appl_ready_user_elem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_AR_APPL_READY_ELEMENT_CONST_PTR_TYPE elem
) {
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	CM_SV_TRACE_07(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) sk(0x%x) ap/sl/su(%u/%u/%u) a.r.p.(%u)"
		, dev->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(elem->session_key)
		, elem->session_key
		, elem->api, elem->slot_nr, elem->subslot_nr
		, elem->appl_ready_pending
		);

	/***/

	argr = cm_sv_ar_lookup3(dev->channel, dev->device_nr, elem->session_key, CM_SV_AR_CNFL_EXP_USER_REQ);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, dev->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(elem->session_key)
			, elem->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		cm_ar_graph_find_api_slot_subslot(argr, elem->api, elem->slot_nr, elem->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

		if( cm_is_null(ar_sub) ) {

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) not in AR"
				, argr->device_nr, argr->ar_nr
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			rsp = CM_ERR_PARAM;
		}
		else if( elem->appl_ready_pending != CM_SV_APPL_READY_PENDING_NO && elem->appl_ready_pending != CM_SV_APPL_READY_PENDING_YES ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "appl_ready_pending(%u) is out of range"
				, elem->appl_ready_pending
			);

			rsp = CM_ERR_PARAM;
		}
		else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_EX_APLRDY) == 0 ) {

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) not expecting appl-ready"
				, argr->device_nr, argr->ar_nr
				, elem->api, elem->slot_nr, elem->subslot_nr
				);

			rsp = CM_OK_CANCELLED;
		}
		else { /* expecting appl-ready */

			ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_EX_APLRDY;

			cm_sv_ar_appl_ready_cnt_decrement(argr);

			/* cm_sv_rpc_appl_ready_request() is triggered in caller */

			/***/

			if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) {

				/* do nothing */
			}
			else {

				if( elem->appl_ready_pending == CM_SV_APPL_READY_PENDING_YES ) {

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_ARP;
				}

				ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED; /* parameterized */
			}

			/***/

			if( cm_ar_com_is_down(argr) ) {

				rsp = CM_OK_CANCELLED;
			}
			else {

				rsp = CM_OK;
			}
		}
	}

	/***/

	return rsp;
}

/*===========================================================================*/
/*=== APPLICATION-READY FROM USER ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_appl_ready_user(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_APPL_READY_PTR_TYPE param = rb->args.sv.ar_appl_ready;
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

			CM_UPPER_SV_AR_APPL_READY_ELEMENT_PTR_TYPE elem = &param->elem[idx];

			elem->response = cm_sv_ar_appl_ready_user_elem(dev, elem);

			if( ! (elem->response == CM_OK || elem->response == CM_OK_CANCELLED) ) {

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) elem[%u] rsp(%u) is not CM_OK or CM_OK_CANCELLED"
					, dev->device_nr, idx, elem->response
					);

				rsp = CM_ERR_ELEM;
			}
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( cm_is_not_null(dev) ) {

		/* trigger appl-ready callback */

		{
		LSA_UINT16 ar_nr;

		for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

			CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

			if( cm_is_not_null(argr) ) {

				if( cm_ar_com_is_down(argr) ) {

					/* do nothing */
				}
				else {

					cm_sv_rpc_appl_ready_request(argr); /* trigger: ar-appl-ready.req */
				}
			}
		}
		}
	}
}

/*===========================================================================*/
/*=== APPL-READY COUNTER DECREMENT ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_appl_ready_cnt_decrement(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( argr->cm.sv.pr_appl_ready_cnt < 1 || (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_ZCNT) != 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.pr_appl_ready_cnt -= 1;

		if( argr->cm.sv.pr_appl_ready_cnt == 0 ) {

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_APPL_READY_ZCNT;

			/* cm_sv_rpc_appl_ready_request() is triggered in caller */
		}
	}
}

/*===========================================================================*/
/*=== RPC APPL-READY REQUEST ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_appl_ready_request(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

	/***/

	if( cm_ar_com_is_down(argr) ) {

		return; /* actually unreachable */
	}

	/***/

	if(
		(argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_PROV_ACT | CM_SV_AR_CNFL_WF_CONS_ACT)) != 0
		|| argr->cm.sv.pr_appl_ready_cnt != 0
	) {

		CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) prov-act.cnf or ioparams-set.cnf or cons-act.cnf or pr_appl_ready_cnt(%u) pending, cn_flags(0x%08x) ar_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.pr_appl_ready_cnt
			, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
			);
		return;
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_ZCNT) == 0 ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) not CM_SV_AR_FL_APPL_READY_ZCNT"
			, argr->device_nr, argr->ar_nr
			);
		return;
	}

	argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_APPL_READY_ZCNT;

	/***/

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) cm-user and cm-pd said appl-ready, cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
		);

	/*
	 * parameterization is done and diagnoses are added (if any),
	 * thus start mirroring the ar-diagnosis-state to the data-status.
	 */

	if( ! argr->cm.sv.pi.is_enabled ) {

		argr->cm.sv.pi.is_enabled = LSA_TRUE; /* now enabled */

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) pi-mirroring enabled"
			, argr->device_nr, argr->ar_nr
			);

		cm_sv_ar_problem_indicator_eval(argr); /* trigger: enable (appl-ready) */
	}

	/***/

	cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: rpc-appl-ready.req (connect-phase, plug-phase) */

	if( argr->cm.sv.dev->skip_check_100Mbps_FDX ) { /* TIA 1963788 */
		/* do nothing */
	}
	else {
		/*
		 * AP00496890: postpone data rate and duplexity check after prm-commit
		 *
		 * At the time of writing (30.08.2007) the following problem exists:
		 * When switching from "autoneg" to "fixed", EDD does a link-down. This seems
		 * to be necessary because of a thing called "parallel detection".
		 *
		 * As long as this is so, the AR is aborted. But the next try will work.
		 *
		 * note: cm-pd did EDD_SRV_GET_LINK_STATUS before confirming CM_OPC_PD_PRM_END_SERVER.
		 */

		if( ! cm_link_status_is_100Mbps_FDX_or_more(channel) ) { /* see PNIO-Spec Part6, CMDEV, PdevCheckFailed */

			/* note: the comment in AP01403295 explains why this check is necessary */

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "link is not 100 Mb/s fdx or more"
				);
			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_LNK_MODE, 0);
			cm_ar_com_down(argr, CM_AR_REASON_LNK_MODE);
			return;
		}
	}

	/*
	 * marshal the control-block
	 *
	 * IOXControlReq ::= ControlBlockConnect ^ ControlBlockPlug, [ModuleDiffBlock]
	 * The field ModuleDiffBlock shall only be present if an Application Ready has to signal an error.
	 *
	 * ControlBlockConnect ::=
	 *   BlockHeader,
	 *   reserved (2 octets), ARUUID, SessionKey,
	 *   reserved (2 octets), ControlCommand, ControlBlockProperties
	 *
	 * ControlBlockPlug ::=
	 *   BlockHeader
	 *   reserved (2 octets), ARUUID, SessionKey,
	 *   AlarmSequenceNumber, ControlCommand, ControlBlockProperties
	 */

	{
	CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.applrdy_rqb;

	if( cm_is_null(rpc) || CM_RPC_GET_OPCODE(rpc) != 0/*unused*/ ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_MEM_U8_PTR_TYPE data_ptr = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		CM_UPPER_ALARM_PTR_TYPE plug_alarm;
		LSA_UINT16 block_type;
		LSA_UINT16 seq;
		LSA_UINT16 prop;
		LSA_UINT32 mdb_context;

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			plug_alarm = LSA_NULL;

			block_type = CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ;

			seq = 0; /* reserved */

			prop = 0;

			mdb_context = CM_SV_MDB_CONTEXT_CONN_ARDY;

			/***/

			if( CM_SV_AR_IS_RTCLASS3(argr) ) {

				prop &= ~CM_CONTROL_PROP_READY_FOR_RTC3; /* always "not yet ready" here, see AP00738250 (RTC3 startup adapted) */
			}
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

			plug_alarm = LSA_NULL;

			block_type = CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ;

			seq = 0; /* reserved */

			prop = 0;

			mdb_context = CM_SV_MDB_CONTEXT_PBE_ARDY;
		}
		else { /* data-phase */

			plug_alarm = argr->cm.sv.plug_group_alarm;

			if( cm_is_null(plug_alarm) ) {
				CM_FATAL();
			}

			block_type = CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ;

			seq = plug_alarm->alarm_sequence;

			prop = 0;

			mdb_context = CM_SV_MDB_CONTEXT_PLUG_ARDY; /* however, the key is "plug_alarm" */
		}

		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type,    block_type);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length,  CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

		/***/

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_reserved1, 0);

		CM_PUT_UUID_HTON(data_ptr, CM_BLK_CTRL_ar_uuid, &argr->ar.ar_uuid);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_session_key, argr->cm.sv.session_key_connect);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_reserved2_alarm_sequence_number, seq);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_control_command, CM_CONTROL_COMMAND_APPLICATION_READY);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_control_properties, prop);

		data_ptr += CM_BLK_CTRL_SIZE;

		/***/

		{
		LSA_UINT32 ndr_length = CM_CAST_U32(data_ptr - rpc->args.cl.call->ndr_data);
		LSA_UINT32 data_length = rpc->args.cl.call->alloc_len - ndr_length;
		LSA_UINT32 written = 0;
		LSA_BOOL ok;

		ok = cm_sv_marshal_module_diff_block(argr, plug_alarm, mdb_context, data_ptr, data_length, &written);

		if( ! ok ) {

			CM_FATAL(); /* a bug */
		}

		data_ptr += written;

		/***/

		ndr_length = CM_CAST_U32(data_ptr - rpc->args.cl.call->ndr_data);

		if( ndr_length > rpc->args.cl.call->alloc_len ) {

			CM_FATAL();
		}
		else {

			rpc->args.cl.call->client_id        = cm_sv_get_rpc_client_id(argr);

			rpc->args.cl.call->opnum            = CLRPC_PNIO_OPNUM_CONTROL;
			rpc->args.cl.call->semantics        = CLRPC_SEMANTICS_IDEMPOTENT;
			rpc->args.cl.call->pnio_signature   = 1; /* see cm_rpc_get_IPNIO_Controller() */
			rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

			rpc->args.cl.call->ndr_len          = ndr_length;
		}
		}

		/***/

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) != 0 ) {

			CM_FATAL();
		}
		else {

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_APPL_READY_REQ;

			cm_sv_rpc_call_request(channel, rpc, LSA_FALSE/*appl-ready*/, argr->device_nr, argr->ar_nr);

			/***/

			if( cm_is_not_null(argr->cm.sv.arset) && (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) { //R1 marker

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

					if( cm_sv_arset_is_first(argr) ) { /* first-AR only (TIA 1722257) */

						cm_sv_r1b_post_sub_state(argr, LSA_FALSE);
					}
				}
				else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

					if( cm_sv_arset_is_primary_or_first(argr) ) { /* TIA 1899476 */

						cm_sv_r1b_post_sub_state(argr, LSA_TRUE);
					}
				}
				else { /* data-phase */

					/* do nothing (status is updated with cm_sv_r1b_handle_alarm_end_plug_group()) */
				}
			}
		}
	}
	}
}

/*===========================================================================*/
/*=== RPC APPL-READY CONFIRMATION ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_appl_ready_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags
		);

	if(
		(argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) != (CM_SV_AR_FL_APPL_READY_REQ | 0)
		|| rpc != argr->cm.sv.applrdy_rqb
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT16 expected_block_type;
		LSA_UINT8 reason;

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			expected_block_type = CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP;
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

			expected_block_type = CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP;
		}
		else { /* data-phase */

			expected_block_type = CM_BLOCK_TYPE_IOXBLOCK_PLUG_RSP;
		}

		/*
		 * IOXControlRes ::= ControlBlockConnect ^ ControlBlockPlug ^ NULL
		 *
		 * In case of a negative response NULL shall be transmitted.
		 *
		 * ControlBlockConnect ::=
		 *   BlockHeader
		 *   , reserved (2 octets), ARUUID, SessionKey
		 *   , reserved (2 octets), ControlCommand, ControlBlockProperties
		 *
		 * ControlBlockPlug ::=
		 *   BlockHeader
		 *   , reserved (2 octets), ARUUID, SessionKey
		 *   , AlarmSequenceNumber, ControlCommand, ControlBlockProperties
		 */

		if(
			CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
			|| rpc->args.cl.call->pnio_status != CM_PNIO_ERR_NONE
		) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "call failed, rpc-rsp(%u) nca_status(0x%x) pnio_status(0x%x)"
				, CM_RPC_GET_RESPONSE(rpc)
				, rpc->args.cl.call->nca_status
				, rpc->args.cl.call->pnio_status
				);

			reason = CM_AR_REASON_CONTROL; /* CNF(-) */
		}
		else {

			CM_COMMON_MEM_U8_PTR_TYPE data_ptr = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in cm_sv_rpc_call_confirmation() */
			LSA_UINT32 data_length             = rpc->args.cl.call->ndr_len  - CLRPC_PNIO_OFFSET;

			if( CM_BLK_CTRL_SIZE != data_length ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong data_length(%u)"
					, data_length
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type) != expected_block_type ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_type, expected:0x%x"
					, expected_block_type
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length) != CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_length"
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version) != 0x0100 ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_version"
					);
				reason = CM_AR_REASON_PROT;
			}
			else {

				if( CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_command) != CM_CONTROL_COMMAND_DONE ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "control-command not 'done'"
						);
					reason = CM_AR_REASON_PROT;
				}
				else {

					/* the rest should be unchanged and is not checked. */

					reason = CM_AR_REASON_NONE;
				}
			}
		}

		/***/

		if( reason != CM_AR_REASON_NONE ) {

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, reason, 0);
			cm_ar_com_down(argr, reason);
		}

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_APPL_READY_CNF; /* set flag after possible calls to cm_ar_com_down() */

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) == 0 ) {

				cm_sv_ar_almi_plug_group_done(argr); /* trigger: CM_SV_AR_FL_APPL_READY_CNF (AR is down) */
			}

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_FL_APPL_READY_CNF */
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != CM_SV_AR_FL_PRM_MASK ) {

				CM_FATAL();
			}
			else { /* parameterization completed */

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_MASK;

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_CONNECT_PHASE; /* end of connect-phase (rpc-applready.cnf) */

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_PREDATA_PHASE; /* begin of predata-phase (rpc-applready.cnf) */

				/***/

				cm_sv_ar_almr_enable(argr); /* maybe an alarm-indication was deferred */

				cm_sv_ar_submodule_state_action(argr); /* trigger: end of connect-phase */

				/***/

				if( CM_SV_AR_IS_ADVANCED(argr) || CM_SV_AR_IS_RTCLASS3(argr) ) {

					cm_sv_ar_almi_enable(argr, LSA_FALSE);

					cm_sv_ar_set2unk_enpras(argr);
				}
				else {

					/* cm_sv_ar_almi_enable() not here! */

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_RECEIVED_IN_RED; /* fake, see cm_sv_ar_new_data_indication() too */

					cm_sv_ar_set2unk_enpras(argr);
				}

				/***/

				cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: end of connect-phase */
			}
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != CM_SV_AR_FL_PRM_MASK ) {

				CM_FATAL();
			}
			else { /* parameterization completed */

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_MASK;

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_DELAY) != 0 ) {

					argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PBE_DELAY;
				}
				else {

					argr->cm.sv.ar_flags &= ~(CM_SV_AR_FL_PBE_PHASE | CM_SV_AR_FL_PBE_PDEV); /* end of prm-begin-end (appl-ready.cnf) */

					argr->cm.sv.pd_flags &= ~(CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL | CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE);
				}

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PLG_REL) != 0 ) {

					argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PBE_PLG_REL;

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_EXP_PRM_END;

					{ /* TIA 1518690 pd-begin after CM_SV_AR_FL_PBE_PLG_REL */
					CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

					if( cm_is_not_null(plg_rel_sub) ) {

						/* cm_sv_ar_almi_send_next() did cm_sv_pd_prm_begin[remote] */
						/* then PrmUpdate came in (overlapping the plug-alarm) */
						/* cm_sv_ar_submodule_state_action() did cm_sv_pd_prm_begin[remote] */
						/* when continuing the plug-alarm, must redo the action of cm_sv_ar_almi_send_next() */

						if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, plg_rel_sub->subslot_nr) ) {

							cm_sv_pd_prm_begin(argr, plg_rel_sub->mod->slot_nr, plg_rel_sub->subslot_nr);
						}

						if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, plg_rel_sub->subslot_nr) ) {

							cm_sv_pd_prm_begin_remote(argr, plg_rel_sub->mod->slot_nr, plg_rel_sub->subslot_nr);
						}
					}
					}
				}

				/***/

				cm_sv_rpc_unpark(argr); /* TIA 1518690 */

				/***/

				cm_sv_ar_almi_newdata_action(argr);

				/***/

				cm_sv_ar_submodule_state_action(argr); /* trigger: end of prm-begin-end */
			}
		}
		else { /* data-phase */

			cm_sv_ar_almi_plug_group_done(argr); /* trigger: CM_SV_AR_FL_APPL_READY_CNF */
		}
	}
}

/*===========================================================================*/
/*=== AR RECEIVED-IN-RED INDICATION =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_received_in_red_indication(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
		);

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_RECEIVED_IN_RED) == 0 ) {

		/* see too cm_sv_ar_sendclock_changed() and cm_sv_ar_prov_cons_on_prmend() */

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_AR_PROBLEM | CM_SV_AR_FL_PDEV_INVALID)) != 0 ) {

			if( CM_SV_AR_IS_RTCLASS3(argr) ) {

				return; /* must ignore the false signal, strange testcase (e.g., RTC2-PDEV-AR with PDIRData + RTC3-AR legacy) */
			}
		}

		/***/

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_RECEIVED_IN_RED;

		cm_sv_ar_set2unk_enpras(argr);
	}
	else {

		/* ignore (actually unreachable) */
	}
}

/*===========================================================================*/
/*=== AR SET-TO-UNKNOWN WITH ENABLE-PROVIDER-AUTOSTOP =======================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_set2unk_enpras(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_PREDATA_PHASE | CM_SV_AR_FL_RECEIVED_IN_RED)) != (CM_SV_AR_FL_PREDATA_PHASE | CM_SV_AR_FL_RECEIVED_IN_RED) ) {

		/* wait */
	}
	else {

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) != 0 ) {

			CM_FATAL();
		}
		else {

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

				argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_RETRIGGER_TIMER;

				cm_sv_ar_cmi_timer_stop(argr); /* trigger: appl-ready.cnf and received-in-red */
			}

			/***/

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_REQ;

			cm_arcq_request(argr, CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS, 0);
		}
	}
}

/*===========================================================================*/
/*=== AR SET-TO-UNKNOWN WITH ENABLE-PROVIDER-AUTOSTOP DONE ==================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_set2unk_enpras_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PREDATA_PHASE) == 0 ) {

		CM_FATAL();
	}
	else { /* predata-phase */

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_MASK) != CM_SV_AR_FL_NEW_DATA_REQ ) {

			CM_FATAL();
		}
		else {

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_CNF;

			/***/

			if( cm_ar_com_is_down(argr) ) {

				cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_FL_NEW_DATA_CNF */
			}
		}
	}
}

/*===========================================================================*/
/*=== AR NEW-DATA INDICATION ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_new_data_indication(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
		);

	/***/

	if( cm_is_iosar_with_device_access(argr) ) {
	
		return;  /* TIA 2944578 */
	}

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_RECEIVED_IN_RED) == 0 ) {

		if( CM_SV_AR_IS_ADVANCED(argr) && ! CM_SV_AR_IS_RTCLASS3(argr) ) {

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

				argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_RETRIGGER_TIMER; /* consumer DHT takes over */

				cm_sv_ar_cmi_timer_stop(argr); /* trigger: new-data.ind (advanced, 12UDP) */
			}

			cm_sv_ar_received_in_red_indication(argr); /* fake */
		}
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_DATA_REQ) == 0 ) {

		/* new-data.ind not requested (first new-data.ind after activation) */
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_DATA_CNF) == 0 ) {

		/* set2unk.req still running, ignored */
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_RECEIVED_IN_RED) == 0 ) {

		/* new-data.ind before received-in-red, ignored */
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_NEW_DATA_IND) != 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_NEW_DATA_IND;

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			CM_FATAL(); /* unreachable */
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PREDATA_PHASE) != 0 ) {

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PROV_PAS_AUX) != 0 ) {

				CM_FATAL();
			}
			else {

				if( CM_SV_AR_IS_RTCLASS3(argr) ) {

					if( CM_SV_AR_IS_ADVANCED(argr) ) {

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_WF_PROV_PAS_AUX;
						cm_sv_ar_prov_passivate_aux_done(argr); /* fake */
					}
					else {

						CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
							, "dev/ar(%u/%u) passivating aux"
							, argr->device_nr, argr->ar_nr
							);

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_WF_PROV_PAS_AUX;
						cm_arcq_request(argr, CM_ARCQ_OPC_PROV_PASSIVATE_AUX, 0);
					}
				}
				else {

					if( CM_SV_AR_IS_ADVANCED(argr) ) {

						cm_sv_ar_almi_rtc3_action(argr); /* AP01256809 */
					}
					else {

						cm_sv_ar_end_of_predata_phase(argr);
					}
				}
			}
		}
		else { /* prm-begin-end or data-phase */

			cm_sv_ar_almi_plug_group_done(argr); /* trigger: CM_SV_AR_FL_NEW_DATA_IND */

			cm_sv_ar_in_data_indication(argr, LSA_FALSE);
		}
	}
}

/*===========================================================================*/
/*=== AUX-PROVIDER PASSIVATE DONE ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_prov_passivate_aux_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_PROV_PAS_AUX) == 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_WF_PROV_PAS_AUX;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_FL_WF_PROV_PAS_AUX */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_RIR_IND);
			CM_UPPER_SV_AR_RIR_PTR_TYPE ar_rir = argr->cm.sv.ar_rir_mem;

			if( cm_is_null(ar_rir) ) {
				CM_FATAL();
			}

			event->args.sv.ar_event->u.sv.ar_rir = ar_rir;

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(argr->ar_com.channel, CM_PATH_TYPE_PD);

			ar_rir->rsv_interval_red = pd_channel->usr.pd.rsv_interval_red; /* AP00964333 and AP01317720 */
			}

			/***/

			ar_rir->nr_of_elems = 0;

			{
			CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

			for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

				CM_UPPER_SV_AR_RIR_ELEMENT_PTR_TYPE elem = &ar_rir->elem[ar_rir->nr_of_elems];

				elem->iocr_user_id = ar_iocr->cm.sv.iocr_user_id;

				ar_rir->nr_of_elems += 1;
			}
			}

			/***/

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RIR_RSP) != 0 ) {

				CM_FATAL();
			}
			else {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RIR_RSP;

				cm_callback(argr->ar_com.channel, CM_ERR_RESOURCE/*!*/, &event);
			}
		}
	}
}

/*===========================================================================*/
/*=== AR RIR RESPONSE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_rir_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_RIR_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else if (param->u.sv.ar_rir != argr->cm.sv.ar_rir_mem) {

		CM_FATAL();
	}
	else {

		LSA_UINT16 rsp = CM_RQB_GET_RESPONSE(rb);

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
			);

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RIR_RSP;

		param->u.sv.ar_rir = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_EX_RIR_ACK) != 0 ) {

			CM_FATAL();
		}
		else {

			if( cm_ar_com_is_down(argr) ) {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EX_RIR_ACK; /* or else CM_ERR_SEQUENCE in cm_sv_opc_ar_rir_ack() */

				cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_RIR_RSP */
			}
			else if( rsp == CM_OK ) { /* usecase "immediate acknowledge" */

				cm_sv_ar_almi_rtc3_action(argr);
			}
			else {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EX_RIR_ACK;
			}
		}
	}
}

/*===========================================================================*/
/*=== AR RIR ACKNOWLEDGE ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_rir_ack(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_RIR_ACK_PTR_TYPE param = rb->args.sv.ar_rir_ack;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_EX_RIR_ACK);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) cn_flags(0x%08x) ar_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
			);

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_EX_RIR_ACK;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev/ar(%u/%u) already down"
				, argr->device_nr, argr->ar_nr
				);

			rsp = CM_OK_CANCELLED; /* late, do nothing */
		}
		else {

			rsp = CM_OK;
		}
	}

	/***/

	cm_callback(channel, rsp, &rb); /* call back first, then trigger */

	if( rsp == CM_OK && cm_is_not_null(argr) ) {

		cm_sv_ar_almi_rtc3_action(argr);
	}
}

/*===========================================================================*/
/*=== AR END OF PREDATA-PHASE ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_end_of_predata_phase(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_NEW_MASK | CM_SV_AR_FL_PREDATA_PHASE)) != (CM_SV_AR_FL_NEW_MASK | CM_SV_AR_FL_PREDATA_PHASE) ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.ar_flags &= ~(CM_SV_AR_FL_NEW_MASK | CM_SV_AR_FL_PREDATA_PHASE); /* end of predata-phase */

		/***/

		if( cm_ar_com_is_down(argr) ) {

			/* late, do nothing */
		}
		else {

			cm_ar_com_cons_apdu_status_check_unicast(argr); /* start data-status indications, see cm_sv_ar_data_status_indication() */

			/* note: starting data-status indications before in-data indication because otherwise the user would begin always with non-current (default) values */

			/***/

			if( ! (CM_SV_AR_IS_ADVANCED(argr) || CM_SV_AR_IS_RTCLASS3(argr)) ) {

				cm_sv_ar_almi_enable(argr, LSA_TRUE);
			}

			/***/

			cm_sv_ar_in_data_indication(argr, LSA_TRUE);
		}
	}
}

/*===========================================================================*/
/*=== AR IN-DATA INDICATION =================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_in_data_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL do_empty
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_INDATA_RSP) != 0 ) {

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_IN_DATA_IND; /* see cm_sv_opc_ar_in_data_response() */
	}
	else {

		CM_UPPER_SV_AR_IN_DATA_PTR_TYPE ar_in_data = argr->cm.sv.ar_in_data_mem;

		if( cm_is_null(ar_in_data) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			LSA_BOOL is_arset_backup = LSA_FALSE;

			if( cm_is_not_null(argr->cm.sv.arset) ) {

				cm_sv_arset_check_primary(argr);

				if( ! cm_sv_arset_is_primary_or_first(argr) ) {

					is_arset_backup = LSA_TRUE;
				}
			}

			/***/

			ar_in_data->nr_of_elems = 0;

			{
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_EX_NEWDTA) != 0 ) {

							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_EX_NEWDTA;

							if( is_arset_backup ) {

								/* exclude (AP01320211) */
							}
							else {

								CM_UPPER_SV_AR_IN_DATA_ELEMENT_PTR_TYPE elem = &ar_in_data->elem[ar_in_data->nr_of_elems];

								elem->api        = ar_ap->api;
								elem->slot_nr    = ar_mod->slot_nr;
								elem->subslot_nr = ar_sub->subslot_nr;

								ar_in_data->nr_of_elems += 1;
							}
						}
					}
				}
			}
			}

			CM_ASSERT(ar_in_data->nr_of_elems <= argr->cm.sv.nr_of_submodules);

			/***/

			if( ar_in_data->nr_of_elems != 0 || do_empty ) {

				CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_IN_DATA_IND);

				event->args.sv.ar_event->u.sv.ar_in_data = ar_in_data;

				/***/

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_INDATA_RSP;

				cm_callback(argr->ar_com.channel, CM_OK, &event);
			}
			else {

				/* do nothing */
			}
		}
	}
}

/*===========================================================================*/
/*=== AR IN-DATA RESPONSE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_in_data_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_INDATA_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else if( param->u.sv.ar_in_data != argr->cm.sv.ar_in_data_mem ) {

		CM_FATAL();
	}
	else {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) ar_flags(0x%08x) in-data.rsp"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.ar_flags
			);

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_INDATA_RSP;

		/***/

		param->u.sv.ar_in_data = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		if( cm_is_not_null(argr->cm.sv.ar_connect_mem) ) { /* always true */

			/* by design not done in cm_sv_ar_in_data_indication() - because of LED-info trigger */

			if( !(argr->cm.sv.ar_connect_mem->disconnect.was_in_data == CM_SV_WAS_IN_DATA_YES) ) { /* TIA 1918234 */

				argr->cm.sv.ar_connect_mem->disconnect.was_in_data = CM_SV_WAS_IN_DATA_YES;

				cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: cm_sv_opc_ar_in_data_response() - AR was_in_data */
			}
		}

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_INDATA_RSP */
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DO_IN_DATA_IND) != 0 ) {

			argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_DO_IN_DATA_IND;

			cm_sv_ar_in_data_indication(argr, LSA_FALSE);
		}
		else {

			/* all done */
		}
	}
}

/*===========================================================================*/
/*=== RPC READY-FOR-RTCLASS3 REQUEST ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_ready4rtclass3_request(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.rdy4rtc3_rqb;

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) doing ready-for-rtclass3 callback"
		, argr->device_nr, argr->ar_nr
		);

	if(
		(argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_RDY4RTC3_CNF) != 0
		|| cm_is_null(rpc)
		|| CM_RPC_GET_OPCODE(rpc) != 0/*unused*/
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

		CM_UPPER_MEM_U8_PTR_TYPE data_ptr = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;

		/*
		 * ControlBlockRTC ::=
		 *   BlockHeader,
		 *   reserved (2 octets), ARUUID, SessionKey,
		 *   reserved (2 octets), ControlCommand, ControlBlockProperties
		 */

		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length,  CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

		/***/

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_reserved1, 0);

		CM_PUT_UUID_HTON(data_ptr, CM_BLK_CTRL_ar_uuid, &argr->ar.ar_uuid);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_session_key, argr->cm.sv.session_key_connect);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_reserved2_alarm_sequence_number, 0/*reserved*/);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_control_command, CM_CONTROL_COMMAND_READY_FOR_RTC3);

		CM_PUT16_HTON(data_ptr, CM_BLK_CTRL_control_properties, 0);

		data_ptr += CM_BLK_CTRL_SIZE;

		/***/

		{
		LSA_UINT32 ndr_length = CM_CAST_U32(data_ptr - rpc->args.cl.call->ndr_data);

		if( ndr_length > rpc->args.cl.call->alloc_len ) {

			CM_FATAL(); /* a bug */
		}
		else {

			rpc->args.cl.call->client_id        = cm_sv_get_rpc_client_id(argr);

			rpc->args.cl.call->opnum            = CLRPC_PNIO_OPNUM_CONTROL;
			rpc->args.cl.call->semantics        = CLRPC_SEMANTICS_IDEMPOTENT;
			rpc->args.cl.call->pnio_signature   = 1; /* see cm_rpc_get_IPNIO_Controller() */
			rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

			rpc->args.cl.call->ndr_len          = ndr_length;
		}
		}

		/***/

		argr->cm.sv.ar_flags |= CM_SV_AR_FL_WF_RDY4RTC3_CNF;

		cm_sv_rpc_call_request(channel, rpc, LSA_TRUE/*ready4rtclass3*/, argr->device_nr, argr->ar_nr);
	}
}

/*===========================================================================*/
/*=== RPC READY-FOR-RTCLASS3 CONFIRMATION ===================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_ready4rtclass3_confirmation(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags
		);

	if(
		(argr->cm.sv.ar_flags & CM_SV_AR_FL_WF_RDY4RTC3_CNF) == 0
		|| rpc != argr->cm.sv.rdy4rtc3_rqb
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT8 reason;

		argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_WF_RDY4RTC3_CNF;

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/); /* note: accessing rpc->args.cl.call after this line (which is ok) */

		/***/

		if(
			CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
			|| rpc->args.cl.call->pnio_status != CM_PNIO_ERR_NONE
		) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "call failed, rpc-rsp(%u) nca_status(0x%x) pnio_status(0x%x)"
				, CM_RPC_GET_RESPONSE(rpc)
				, rpc->args.cl.call->nca_status
				, rpc->args.cl.call->pnio_status
				);
			reason = CM_AR_REASON_CONTROL; /* CNF(-) */
		}
		else {

			CM_COMMON_MEM_U8_PTR_TYPE data_ptr = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in cm_sv_rpc_call_confirmation() */
			LSA_UINT32 data_length             = rpc->args.cl.call->ndr_len  - CLRPC_PNIO_OFFSET;

			if( CM_BLK_CTRL_SIZE != data_length ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong data_length(%u)"
					, data_length
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type) != CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_RSP ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_type"
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length) != CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_length"
					);
				reason = CM_AR_REASON_PROT;
			}
			else if( CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version) != 0x0100 ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_version"
					);
				reason = CM_AR_REASON_PROT;
			}
			else {

				if( CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_command) != CM_CONTROL_COMMAND_DONE ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "control-command not 'done'"
						);
					reason = CM_AR_REASON_PROT;
				}
				else {

					/* the rest should be unchanged and is not checked. */

					reason = CM_AR_REASON_NONE;
				}
			}
		}

		/***/

		if( reason != CM_AR_REASON_NONE ) {

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, reason, 0);
			cm_ar_com_down(argr, reason);
		}

		cm_sv_ar_almi_rtc3_action_done(argr);
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
