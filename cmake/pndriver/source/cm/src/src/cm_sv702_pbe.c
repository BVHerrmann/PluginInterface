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
/*  F i l e               &F: cm_sv702_pbe.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, R1 Bridge                                                     */
/*                                                                           */
/*  - Prm Begin/End                                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7702
#define CM_MODULE_ID       7702

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== RPC CONTROL INDICATION (PrmBegin) =====================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_rpc_control_indication_prm_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT32 data_length;
	struct {
		LSA_UINT16 block_type;
		LSA_UINT16 block_length;
		LSA_UINT16 block_version;
		LSA_UINT16 reserved1;
		CLRPC_UUID_TYPE ar_uuid;
		LSA_UINT16 session_key;
		LSA_UINT16 reserved2;
		LSA_UINT16 command;
		LSA_UINT16 properties;
	} ctrl;

	data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
	data_length = rpc->args.sv.call->ndr_len  - CLRPC_PNIO_OFFSET;

	if( data_length < CM_BLK_HEADER_SIZE ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong block-length"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ArgsLengthInvalid();
	}

	ctrl.block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
	ctrl.block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
	ctrl.block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( ctrl.block_type != CM_BLOCK_TYPE_PRM_BEGIN_REQ ) {
		CM_FATAL(); /* see caller */
	}

	if( ctrl.block_length != 28 ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong block-length"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 1);
	}

	if( (ctrl.block_version & 0xFF00) != 0x0100/*V1.0*/ ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block-version.high(0x%x) not supported"
			, ctrl.block_version
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 2); /* wrong block-version.high */
	}

	if( (ctrl.block_version & 0x00FF) != 0x0000/*V1.0*/ ) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block-version.low(0x%x) not supported"
			, ctrl.block_version
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 3); /* wrong block-version.low */
	}

	ctrl.reserved1 = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_reserved1);

	if( (ctrl.reserved1) != 0 ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "padding1 != 0"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 4);
	}

	CM_GET_UUID_NTOH(&ctrl.ar_uuid, data_ptr, CM_BLK_CTRL_ar_uuid);

	ctrl.session_key = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_session_key);

	ctrl.reserved2 = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_reserved2_alarm_sequence_number);

	if( (ctrl.reserved2) != 0 ) {
		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "padding2 != 0"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 7);
	}

	ctrl.command = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_command);

	ctrl.properties = CM_GET16_NTOH(data_ptr, CM_BLK_CTRL_control_properties);

	/***/

	data_length -= CM_BLK_CTRL_SIZE;
	data_ptr    += CM_BLK_CTRL_SIZE;

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
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 6); /* wrong session-key, ignore */
	}

	if( ctrl.command != CM_CONTROL_COMMAND_PRM_BEGIN ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) prm-begin, wrong control-command(0x%x)"
			, dev->device_nr
			, ctrl.command
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 8); /* wrong control-command, ignore */
	}

	if( ctrl.properties != 0 ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) prm-end, wrong control-properties(0x%x)"
			, dev->device_nr
			, ctrl.properties
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 9); /* wrong control-properties, ignore */
	}

	/***/

	if( rpc->args.sv.call->host_ip != argr->ar.host_ip ) {
		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "host(%08X) not conn-host(%08X)"
			, CM_NTOH32(rpc->args.sv.call->host_ip)
			, CM_NTOH32(argr->ar.host_ip)
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict(); /* unexpected ip-addr */
	}

	if( cm_is_not_null(argr->cm.sv.curr_rpc_ind) ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "already servicing a call (two clients on same host using the same ARUUID?)"
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict(); /* unexpected call */
	}

	if( cm_ar_com_is_down(argr) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) already down (telling unknown aruuid)"
			, argr->device_nr, argr->ar_nr
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ARUUIDUnknown(); /* already down */
	}

	if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) prm-begin, not supported for easy-supervisor"
			, argr->device_nr, argr->ar_nr
			);

		return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_UnknownBlocks(); /* not supported for easy-supervisor */
	}

	if( cm_is_null(argr->cm.sv.arset) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) prm-begin, but not IOCARSR"
			, argr->device_nr, argr->ar_nr
			);
		return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_PrmBegin, 8); /* not IOCARSR, tell wrong control-command */
	}

	/***/

	{
	LSA_BOOL is_parked = cm_sv_rpc_park(argr, rpc, LSA_FALSE); /* TIA 1518690 */

	if( is_parked ) {

		return CM_PNIO_ERR_NONE;
	}
	}

	/***/

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev/ar(%u/%u) prm-begin, ar_flags(0x%08x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.ar_flags
		);

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) == (CM_SV_AR_FL_PRM_MASK & ~CM_SV_AR_FL_APPL_READY_CNF) ) {

			/* possible case: prm-begin.ind before appl-ready.cnf (e.g., because of re-run of appl-ready) */
			/* delaying is done via cm_sv_ar_submodule_state_action() */
		}
		else {

			return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict();
		}
	}

	if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_PREDATA_PHASE | CM_SV_AR_FL_NEW_MASK)) != 0 ) {

		/* no problem, see cm_sv_ar_almi_newdata_action() */
	}

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

		if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_APPL_READY_REQ | CM_SV_AR_FL_APPL_READY_CNF)) == (CM_SV_AR_FL_APPL_READY_REQ | 0) ) {

			/* IOC saw appl-ready.req but IOD did not see appl-ready.cnf yet (e.g., if the cnf got lost) */

			{
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) { /* always false (reset on appl-ready.req) */

							CM_FATAL(); /* a bug */
						}
					}
				}
			}
			}

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_DELAY; /* note: the CM_SV_AR_SMFL_PBE flags were reset on appl-ready.req */

			goto pbe_ok; /* note: CM_SV_AR_FL_PBE_PHASE is set here, and will be set again below */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: SRD, CM_SV_AR_FL_PBE_DELAY and the 'goto' here.")
/* check logic for set/reset of CM_SV_AR_FL_PBE_PDEV in case of CM_SV_AR_FL_PBE_DELAY */
/* refactor too: rerun could be checked stronger, CMRPC/StateConflict if not */
#endif
		}
		else {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev/ar(%u/%u) prm-begin-end already running (rerun)"
				, argr->device_nr, argr->ar_nr
				);

			cm_sv_rpc_control_response_prm_begin(argr, rpc); /* rerun */

			return CM_PNIO_ERR_NONE; /* done */
		}
	}
	else { /* no prm-begin-end */

pbe_ok:
		if( data_length == 0 ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "SubmoduleListBlock missing"
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_WrongBlockCount();
		}

		/***/

		{
		LSA_UINT8 code2 = 0;
		LSA_UINT16 nr_of_entries;
		LSA_UINT16 nr_of_pdev           = 0;
		LSA_UINT16 nr_of_pdev_if        = 0;
		LSA_UINT16 nr_of_remote_pdev    = 0;
		LSA_UINT16 nr_of_remote_pdev_if = 0;

		if( data_length < CM_BLK_HEADER_SIZE ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong block-length"
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_ArgsLengthInvalid();
		}

		ctrl.block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
		ctrl.block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
		ctrl.block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

		if( ctrl.block_type != CM_BLOCK_TYPE_SUBMODULE_LIST_REQ ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block not SubmoduleListBlock"
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_UnknownBlocks();
		}

		if( (ctrl.block_version & 0xFF00) != 0x0100/*V1.0*/ ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block-version.high(0x%x) not supported"
				, ctrl.block_version
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, 2); /* wrong block-version.high */
		}

		if( (ctrl.block_version & 0x00FF) != 0x0000/*V1.0*/ ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "block-version.low(0x%x) not supported"
				, ctrl.block_version
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, 3); /* wrong block-version.low */
		}

		if( ctrl.block_length != data_length - CM_BLK_HEADER_TypLen_SIZE ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "mismatch block-length/data_length"
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, 1); /* wrong block length */
		}

		if( data_length < CM_BLK_SUBMOD1_SIZE ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong block-length"
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, 1); /* wrong block length */
		}

		nr_of_entries = CM_GET16_NTOH(data_ptr, CM_BLK_SUBMOD1_number_of_entries);

		data_length -= CM_BLK_SUBMOD1_SIZE;
		data_ptr    += CM_BLK_SUBMOD1_SIZE;

		if( nr_of_entries * CM_BLK_SUBMOD2_SIZE != data_length ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "NumberOfEntries(%u) doesn't fit to BlockLength"
				, nr_of_entries
				);
			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, 4); /* wrong NumberOfEntries */
		}

		/***/

		if( nr_of_entries == 0 ) {

			code2 = 4; /* zero is not allowed */
		}
		else {

			LSA_UINT16 ilv;

			for (ilv = 0; ilv < nr_of_entries; ilv++) {

				LSA_UINT32 api        = CM_GET32_NTOH(data_ptr, CM_BLK_SUBMOD2_api);
				LSA_UINT16 slot_nr    = CM_GET16_NTOH(data_ptr, CM_BLK_SUBMOD2_slot_nr);
				LSA_UINT16 subslot_nr = CM_GET16_NTOH(data_ptr, CM_BLK_SUBMOD2_subslot_nr);

				CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				cm_ar_graph_find_api_slot_subslot (argr, api, slot_nr, subslot_nr, &ar_ap, &ar_mod, &ar_sub);

				if ( cm_is_null (ar_sub)) {

					if ( cm_is_null (ar_ap)) {
						code2 = 5; /* api not found */
					}
					else if ( cm_is_null (ar_mod)) {
						code2 = 6; /* slot not found */
					}
					else {
						code2 = 7; /* subslot not found */
					}
					break;
				}
				else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

					code2 = 7; /* subslot twice in list */
					break;
				}
				else {

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_PBE;

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, ar_sub->subslot_nr) ) {

						nr_of_pdev += 1;

						if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr)) {

							nr_of_pdev_if += 1;
						}
					}

					if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(dev, ar_sub->subslot_nr) ) {

						nr_of_remote_pdev += 1;

						if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr)) {

							nr_of_remote_pdev_if += 1;
						}
					}
				}

				data_length -= CM_BLK_SUBMOD2_SIZE;
				data_ptr    += CM_BLK_SUBMOD2_SIZE;
			}

			/***/

			if( code2 == 0 ) { /* no duplicates, all submodules are part of the AR */

				if( nr_of_entries == argr->cm.sv.nr_of_submodules ) { /* see CM-CL, CM_CL_PRMMODE_AR */

					if( nr_of_pdev != argr->cm.sv.nr_of_pdev_submodules && nr_of_remote_pdev != argr->cm.sv.nr_of_remote_pdev_submodules ) {

						code2 = 4; /* a bug */
					}
					else {

						/* ok, the AR */
					}
				}
				else if( dev->CIR_enabled ) { /* allow 1 submodule (TIA 906437 and 937536) */

					if( nr_of_entries == 1 ) {

						if( nr_of_pdev == 0 && nr_of_remote_pdev == 0 ) {

							/* ok, a normal submodule */
						}
						else if( nr_of_pdev == 1 && nr_of_pdev_if == 1 && nr_of_remote_pdev == 0 ) {

							if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) {

								CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
									, "PrmUpdate(IF local) but not COC"
									);

								code2 = 4; /* ar-properties wrong */
							}
							else {
								/* ok, local interface submodule (parameterization via CombinedObjectContainer) */
							}
						}
						else if( nr_of_remote_pdev == 1 && nr_of_remote_pdev_if == 1 && nr_of_pdev == 0) {

							if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) {

								CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
									, "PrmUpdate(IF remote) but not COC"
									);

								code2 = 4; /* ar-properties wrong */
							}
							else {
								/* ok, remote interface submodule (parameterization via CombinedObjectContainer) */
							}
						}
						else {

							code2 = 4; /* not the interface-submodule in list */
						}
					}
					else {

						code2 = 4; /* not 1 submodule in list */
					}
				}
				else {

					code2 = 4; /* not all submodules in list (and CIR not enabled) */
				}
			}
		}

		if( code2 == 0 ) {

			if (data_length != 0) {

				code2 = 1; /* a bug */
			}
		}

		if( code2 == 0 ) {

			LSA_UINT32 mdb_len = CM_RSP_MDB_SIZE;

			{
			LSA_UINT32 api_len = 0;
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				LSA_UINT32 mod_len = 0;
				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

					LSA_UINT32 sub_len = 0;
					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

							sub_len += CM_RSP_SUB_SIZE;
						}
					}

					mdb_len += sub_len;

					if( sub_len != 0 ) {

						mod_len += CM_RSP_MOD_SIZE;
					}
				}

				mdb_len += mod_len;

				if( mod_len != 0 ) {

					api_len += CM_RSP_API_SIZE;
				}
			}

			mdb_len += api_len;
			}

			if( 32 + mdb_len > rpc->args.sv.call->alloc_len ) {

				code2 = 4; /* report "too many subslots" */
			}
		}

		/***/

		if( code2 != 0 ) { /* undo */

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "parsing error: NumberOfEntries(%u) local-pdev(%u) remote-pdev(%u), code2(%u), ar_properties(0x%08x)"
				, nr_of_entries, nr_of_pdev, nr_of_remote_pdev, code2, argr->ar.ar_properties
				);

			/***/

			{
			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_PBE;
					}
				}
			}
			}

			return CM_PNIO_FAULTY_IODControlConnectPlug(CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList, code2);
		}
		}

		/***/

		if( cm_is_not_null(argr->cm.sv.arset) ) {

			cm_sv_arset_check_primary(argr); /* necessary to adjust pdev-ownership */
		}

		/***/

		{
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
		LSA_UINT32 count = 0;

		for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_PHASE;
						count += 1;

						if( CM_SV_SUBSLOT_IS_PDEV(ar_sub->subslot_nr) ) {

							argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_PDEV;
						}

						if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr) ) {

							if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, ar_sub->subslot_nr) ) {

								argr->cm.sv.pd_flags |= CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL;
							}

							if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(dev, ar_sub->subslot_nr) ) {

								argr->cm.sv.pd_flags |= CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE;
							}
						}

						/***/

						if( argr->cm.sv.pr_appl_ready_cnt != 0 ) {
							CM_FATAL();
						}

						cm_sv_ar_submodule_state_changed(argr, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr); /* note: cancels alarms */

						/***/

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0 ) { /* needs action */

							CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

							if( su->owner_ar_nr == argr->ar_nr && CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {

								ar_sub->sv.st_told = CM_SUB_STATE_GOOD;

								ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
							}
						}
					}
				}
			}
		}

		if( count != 1 ) { /* not PBE interface */

			argr->cm.sv.pd_flags &= ~(CM_SV_AR_PDFL_PBE_PDEV_IF_LOCAL | CM_SV_AR_PDFL_PBE_PDEV_IF_REMOTE);
		}
		}

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) == 0 ) {

			CM_FATAL(); /* a bug */
		}
		else if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_PBE_DO_OSHP_IND | CM_SV_AR_FL_PBE_DO_CTRL_RSP)) != 0 ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) prm-begin, ar_flags(0x%08x), plg_rel_sub(0x%08x), almi[0].flags(0x%04x)"
				, argr->device_nr, argr->ar_nr
				, argr->cm.sv.ar_flags, plg_rel_sub, argr->cm.sv.almi[0].flags
				);

			if( cm_is_not_null(plg_rel_sub) ) { /* prm-begin-end overlaps with plug or release alarm */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != CM_SV_AR_FL_EXP_PRM_END ) {

					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
					cm_ar_com_down(argr, CM_AR_REASON_PROT);

					return CM_PNIO_FAULTY_IODControlConnectPlug__CMRPC_StateConflict();
				}
				else {

					argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_MASK;

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_PLG_REL;

					if( (plg_rel_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

						plg_rel_sub->sv.sm_flags |= CM_SV_AR_SMFL_PBE_PLG_REL;
					}
				}
			}
			else if( cm_is_not_null(argr->cm.sv.plug_group_alarm) ) { /* plug-group alarm is not current but queued (AP01525350) */

				plg_rel_sub = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE )argr->cm.sv.plug_group_alarm->u.sv.void_ptr;

				if( cm_is_not_null(plg_rel_sub) ) { /* i.e., not pull-module */

					if( (plg_rel_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

						plg_rel_sub->sv.sm_flags |= CM_SV_AR_SMFL_PBE_PLG_REL; /* the following alarm parameterizes */
					}
				}
			}

			/***/

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_DO_OSHP_IND;

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_PBE_DO_CTRL_RSP;

			argr->cm.sv.curr_rpc_ind = rpc; /* pending */

			/***/

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

				cm_sv_ar_cmi_timer_stop(argr); /* trigger: rpc-prm-begin.ind */
			}

			/***/

			cm_sv_ar_submodule_state_action(argr); /* trigger: prm-begin (do ar-ownership.ind) */
		}

		return CM_PNIO_ERR_NONE; /* pending */
	}
}

/*===========================================================================*/
/*=== RPC CONTROL RESPONSE (PrmBegin) =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_control_response_prm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;

	data_ptr    = rpc->args.sv.call->ndr_data  + CLRPC_PNIO_OFFSET;
	data_length = rpc->args.sv.call->alloc_len - CLRPC_PNIO_OFFSET;

	if( data_length < 32 ) {

		CM_FATAL(); /* unreachable */
	}
	else {

		LSA_UINT32 written;

		/* block header */
		CM_PUT16_HTON(data_ptr, 0, CM_BLOCK_TYPE_PRM_BEGIN_RSP);
		CM_PUT16_HTON(data_ptr, 2, CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON(data_ptr, 4, 0x0100/*V1.0*/);

		/* control block */
		CM_PUT16_HTON(data_ptr,     6, 0); /* padding */
		CM_PUT_UUID_HTON(data_ptr,  8, &argr->ar.ar_uuid);
		CM_PUT16_HTON(data_ptr,    24, argr->cm.sv.session_key_connect);
		CM_PUT16_HTON(data_ptr,    26, 0); /* padding */
		CM_PUT16_HTON(data_ptr,    28, CM_CONTROL_COMMAND_DONE);
		CM_PUT16_HTON(data_ptr,    30, 0); /* control block properties */

		written = 32;

		if( ! cm_sv_marshal_module_diff_block(argr, LSA_NULL, CM_SV_MDB_CONTEXT_PBE_BRES, data_ptr, data_length, &written) ) {

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT); /* unreachable */
		}

		cm_sv_rpc_call_response(argr->cm.sv.dev, rpc, CLRPC_OK, CM_PNIO_ERR_NONE, CLRPC_PNIO_OFFSET + written);
	}

	/***/

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

		if( cm_ar_com_is_down(argr) ) {

			/* do nothing, timer is reused for other things */
		}
		else {

			cm_sv_ar_cmi_timer_trigger(argr); /* trigger: rpc-prm-begin.rsp */
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
