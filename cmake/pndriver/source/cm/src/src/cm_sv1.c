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
/*  F i l e               &F: cm_sv1.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Server things                                                 */
/*                                                                           */
/*  - server create                                                          */
/*  - server delete                                                          */
/*  - server control                                                         */
/*  - rpc lower done                                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7100
#define CM_MODULE_ID       7100

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("CM_CFG_MAX_SERVERS           = " CM_STRINGIFY(CM_CFG_MAX_SERVERS))
#pragma CM_MESSAGE("CM_CFG_MAX_SV_DEVICES        = " CM_STRINGIFY(CM_CFG_MAX_SV_DEVICES))
#pragma CM_MESSAGE("CM_CFG_MAX_SV_LOG_ENTRIES    = " CM_STRINGIFY(CM_CFG_MAX_SV_LOG_ENTRIES))
#pragma CM_MESSAGE("CM_CFG_MAX_SV_ARS            = " CM_STRINGIFY(CM_CFG_MAX_SV_ARS))
#pragma CM_MESSAGE("CM_CFG_MAX_SV_ARS_RTC123UDP  = " CM_STRINGIFY(CM_CFG_MAX_SV_ARS_RTC123UDP))
#pragma CM_MESSAGE("CM_CFG_SV_AR_HAS_MCONS       = " CM_STRINGIFY(CM_CFG_SV_AR_HAS_MCONS))
#endif /* CM_MESSAGE */

/*===========================================================================*/
/*=== SERVER OPCODE DISPATCHER ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_dispatch(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {

	if( channel->state != CM_SV_CREATED ) {

		switch( CM_RQB_GET_OPCODE(rb) ) {

		case CM_OPC_SV_CREATE:
			/* channel-state will be checked in cm_sv_opc_create() */
			break;

		default:
			CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "opc(%u) channel-state(%u) not CM_SV_CREATED"
				, CM_RQB_GET_OPCODE(rb), channel->state
				);
			cm_callback(channel, CM_ERR_SEQUENCE, &rb); /* CM-SV not created */
			return;
		}
	}

	if( cm_is_null(rb->args.void_ptr) ) {

		if( CM_RQB_GET_OPCODE(rb) == CM_OPC_SV_DELETE ) {

			/* ok, has no arguments */
		}
		else {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "opc(%u) needs arguments"
				, CM_RQB_GET_OPCODE(rb)
				);
			cm_callback(channel, CM_ERR_PARAM, & rb);
			return;
		}
	}

	/***/

	switch( CM_RQB_GET_OPCODE(rb) ) {

	case CM_OPC_SV_CREATE:
		cm_sv_opc_create(channel, rb);
		break;

	case CM_OPC_SV_DELETE:
		cm_sv_opc_delete(channel, rb);
		break;

	case CM_OPC_SV_CONTROL:
		cm_sv_opc_control(channel, rb);
		break;

	/***/

	case CM_OPC_SV_DEVICE_ADD:
		cm_sv_opc_device_add(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_REMOVE:
		cm_sv_opc_device_remove(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_CONTROL:
		cm_sv_opc_device_control(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_LED_INFO:
		cm_sv_opc_device_led_info(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_PROVIDE_EVENT:
		cm_sv_opc_device_provide_event(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_PROVIDE_ALARM:
		cm_sv_opc_device_provide_alarm(channel, rb);
		break;

	case CM_OPC_SV_DEVICE_READ:
		cm_sv_opc_device_read(channel, rb);
		break;

	/***/

	case CM_OPC_SV_R1BRIDGE_SEND_PROVIDE:
		cm_sv_opc_r1bridge_send_provide(channel, rb);
		break;

	case CM_OPC_SV_R1BRIDGE_SEND_RSP:
		cm_sv_opc_r1bridge_send_rsp(channel, rb);
		break;

	case CM_OPC_SV_R1BRIDGE_RECEIVED:
		cm_sv_opc_r1bridge_received(channel, rb);
		break;

	case CM_OPC_SV_R1BRIDGE_CONTROL:
		cm_sv_opc_r1bridge_control(channel, rb);
		break;

	/***/

	case CM_OPC_SV_SUBMODULE_ADD:
		cm_sv_opc_submodule_add(channel, rb);
		break;

	case CM_OPC_SV_SUBMODULE_REMOVE:
		cm_sv_opc_submodule_remove(channel, rb);
		break;

	case CM_OPC_SV_DIAG_ADD:
		cm_sv_opc_diag_add(channel, rb);
		break;

	case CM_OPC_SV_DIAG_REMOVE:
		cm_sv_opc_diag_remove(channel, rb);
		break;

	case CM_OPC_SV_DIAG_ADD2: /* AP01467031 */
		cm_sv_opc_diag_change(channel, rb, LSA_TRUE);
		break;

	case CM_OPC_SV_DIAG_REMOVE2: /* AP01467031 */
		cm_sv_opc_diag_change(channel, rb, LSA_FALSE);
		break;

	/***/

	case CM_OPC_SV_ARSET_TRIGGER:
		cm_sv_opc_arset_trigger(channel, rb);
		break;

	case CM_OPC_SV_ARSET_ABORT:
		cm_sv_opc_arset_abort(channel, rb);
		break;

	/***/

	case CM_OPC_SV_AR_APPL_READY:
		cm_sv_opc_ar_appl_ready_user(channel, rb);
		break;

	case CM_OPC_SV_AR_CONTROL:
		cm_sv_opc_ar_control(channel, rb);
		break;

	case CM_OPC_SV_AR_ABORT:
		cm_sv_opc_ar_abort(channel, rb);
		break;

	case CM_OPC_SV_AR_RIR_ACK:
		cm_sv_opc_ar_rir_ack(channel, rb);
		break;

	/***/

	case CM_OPC_SV_AR_ALARM_SEND:
		cm_sv_opc_ar_alarm_send(channel, rb);
		break;

	case CM_OPC_SV_AR_ALARM_PDEV_GROUP:
		cm_sv_opc_ar_alarm_pdev_group_ind(channel, rb);
		break;

	/***/

	case CM_OPC_SV_AR_ALARM_RSP:
		cm_sv_opc_ar_alarm_response(channel, rb);
		break;

	case CM_OPC_SV_AR_ALARM_ACK:
		cm_sv_opc_ar_alarm_ack(channel, rb);
		break;

	/***/

	case CM_OPC_SV_AR_CONNECT_RSP:
		cm_sv_opc_ar_connect_response(channel, rb);
		break;

	case CM_OPC_SV_AR_DISCONNECT_RSP:
		cm_sv_opc_ar_disconnect_response(channel, rb);
		break;

	case CM_OPC_SV_AR_OWNERSHIP_RSP:
		cm_sv_opc_ar_ownership_response(channel, rb);
		break;

	case CM_OPC_SV_AR_PRM_END_RSP:
		cm_sv_opc_ar_prm_end_response(channel, rb);
		break;

	case CM_OPC_SV_AR_IN_DATA_RSP:
		cm_sv_opc_ar_in_data_response(channel, rb);
		break;

	case CM_OPC_SV_AR_RIR_RSP:
		cm_sv_opc_ar_rir_response(channel, rb);
		break;

	case CM_OPC_SV_AR_READ_RSP:
		cm_sv_opc_ar_record_response(channel, rb, LSA_TRUE/*read*/);
		break;

	case CM_OPC_SV_AR_WRITE_RSP:
		cm_sv_opc_ar_record_response(channel, rb, LSA_FALSE/*write*/);
		break;

	case CM_OPC_SV_AR_DATA_STATUS_RSP:
		cm_sv_opc_ar_data_status_response(channel, rb);
		break;

#if CM_CFG_SV_AR_HAS_MCONS
	case CM_OPC_SV_AR_MCONS_RSP:
		cm_sv_opc_ar_mcons_response(channel, rb);
		break;
#endif

	case CM_OPC_SV_AR_PDEVPRM_RSP:
		cm_sv_opc_ar_pdevprm_response(channel, rb);
		break;

	case CM_OPC_SV_AR_PDEVPRM_READ:
		cm_sv_opc_ar_pdevprm_rdwr(channel, rb, LSA_TRUE);
		break;

	case CM_OPC_SV_AR_PDEVPRM_WRITE:
		cm_sv_opc_ar_pdevprm_rdwr(channel, rb, LSA_FALSE);
		break;

	case CM_OPC_PD_PRM_BEGIN_SERVER:
	case CM_OPC_PD_PRM_READ_SERVER:
	case CM_OPC_PD_PRM_WRITE_SERVER:
	case CM_OPC_PD_PRM_END_SERVER:
	case CM_OPC_PD_PRM_RELINQUISH_SERVER:
		cm_sv_pdev_request_done(channel, rb);
		break;

	default:
		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unhandled opcode(%u), path-type(%u)"
			, CM_RQB_GET_OPCODE(rb), channel->path_type
			);

		CM_FATAL();
		break;
	}
}

/*===========================================================================*/
/*=== SERVER INIT ===========================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_server_init(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_SV_CREATE_CONST_PTR_TYPE param
) {
	CM_SV_CHANNEL_TYPE *sv = &channel->usr.sv;

	/***/

	sv->curr = LSA_NULL;

	/***/

	sv->can_RecordDataReadQuery = param->can_RecordDataReadQuery == CM_SV_RECORD_DATA_READ_QUERY_YES ? LSA_TRUE : LSA_FALSE;

	sv->can_ARFSUDataAdjust = param->can_ARFSUDataAdjust == CM_SV_AR_FSU_DATA_ADJUST_YES ? LSA_TRUE : LSA_FALSE;

	sv->enable_AlarmResponder = param->enable_AlarmResponder == CM_SV_ALARM_RESPONDER_YES ? LSA_TRUE : LSA_FALSE;

	sv->enable_DataStatusIndication = param->enable_DataStatusIndication == CM_SV_DATA_STATUS_INDICATION_YES ? LSA_TRUE : LSA_FALSE;

	sv->IOXS_Required = param->IOXS_Required == CM_IOXS_REQUIRED_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1218229 */

	sv->pdev_device_nr = 0;

	sv->tell_all_ARs_gone = LSA_FALSE;

	/***/

	{
	LSA_UINT16 device_nr;

	for( device_nr = 0/*!*/; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

		sv->dev[device_nr] = LSA_NULL;
	}
	}

	/***/

	sv->edd_rqb = cm_edd_alloc_rqb(channel, EDD_SRV_SRT_PROVIDER_SET_STATE); /* see check below */

	/***/

	if( cm_is_null(sv->edd_rqb) ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc edd-rqb"
			);

		return LSA_FALSE; /* caller must call cm_sv_server_undo_init() */
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== SERVER UNDO-INIT ======================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_server_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_SV_CHANNEL_TYPE *sv = &channel->usr.sv;

	/***/

	CM_ASSERT(cm_is_null(sv->curr));

	CM_ASSERT(sv->pdev_device_nr == 0);

	CM_ASSERT(sv->tell_all_ARs_gone == LSA_FALSE);

	/***/

	if( cm_is_not_null(sv->edd_rqb) ) {

		cm_edd_free_rqb(channel, sv->edd_rqb);
	}
}

/*===========================================================================*/
/*=== SERVER CREATE =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_create(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_CREATE_PTR_TYPE param = rb->args.sv.create;
	LSA_UINT16 rsp;

	/***/

	if( channel->state != CM_CH_READY ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel-state(%u) not CM_CH_READY"
			, channel->state
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( ! cm_pdusr_is_ready(channel) ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "CM-PD not ready"
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cm-sv(%u) connected to cm-pd(%u), trace_idx(%u)"
			, channel->my_handle
			, cm_peer_channel_get(channel, CM_PATH_TYPE_PD)->my_handle
			, channel->trace_idx
			);

		if( ! cm_sv_server_init(channel, param) ) {

			cm_sv_server_undo_init(channel);

			rsp = CM_ERR_ALLOC_LOWER;
		}
		else {

			rsp = CM_OK;
		}
	}

	if ( CM_OK == rsp ) {
		if (param->can_RecordDataReadQuery != CM_SV_RECORD_DATA_READ_QUERY_NO && param->can_RecordDataReadQuery != CM_SV_RECORD_DATA_READ_QUERY_YES) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "can_RecordDataReadQuery(%u) out of range"
				, param->can_RecordDataReadQuery
			);

			rsp = CM_ERR_PARAM;
		}

		if (param->can_ARFSUDataAdjust != CM_SV_AR_FSU_DATA_ADJUST_NO && param->can_ARFSUDataAdjust != CM_SV_AR_FSU_DATA_ADJUST_YES) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "can_ARFSUDataAdjust(%u) out of range"
				, param->can_ARFSUDataAdjust
			);

			rsp = CM_ERR_PARAM;
		}

		if (param->enable_AlarmResponder != CM_SV_ALARM_RESPONDER_NO && param->enable_AlarmResponder != CM_SV_ALARM_RESPONDER_YES) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "enable_AlarmResponder(%u) out of range"
				, param->enable_AlarmResponder
			);

			rsp = CM_ERR_PARAM;
		}

		if (param->enable_DataStatusIndication != CM_SV_DATA_STATUS_INDICATION_NO && param->enable_DataStatusIndication != CM_SV_DATA_STATUS_INDICATION_YES) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "enable_DataStatusIndication(%u) out of range"
				, param->enable_DataStatusIndication
			);

			rsp = CM_ERR_PARAM;
		}

		if (param->IOXS_Required != CM_IOXS_REQUIRED_NO && param->IOXS_Required != CM_IOXS_REQUIRED_YES) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "IOXS_Required(%u) out of range"
				, param->IOXS_Required
			);

			rsp = CM_ERR_PARAM;
		}
	}

	/***/

	if( rsp != CM_OK ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		channel->state = CM_SV_CREATING;

		/* get channel infos (#201512) */

		{
		CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

		rb->args.sv.create->hardware_type = edd_params->HardwareType;
		rb->args.sv.create->interface_id  = edd_params->InterfaceID;
		}

		/***/

		channel->usr.sv.curr = rb; /* pending */

		/* reset the global provider state to its default */

		{
		CM_EDD_LOWER_RQB_PTR_TYPE edd = channel->usr.sv.edd_rqb;
		EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE edd_glob = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE);

		edd_glob->GroupID = CM_PROVIDER_GROUP_IOD;
		edd_glob->Status  = EDD_CSRT_DSTAT_BIT_STOP_RUN;
		edd_glob->Mask    = EDD_CSRT_DSTAT_BIT_STOP_RUN;

		cm_edd_request_lower(channel, EDD_SRV_SRT_PROVIDER_SET_STATE, channel, edd);
		}

		/* see cm_sv_acp_global_provider_control_done() */
	}
}

/*===========================================================================*/
/*=== SERVER DELETE =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_delete(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	LSA_UINT16 rsp;

	/* note: channel->state is checked in caller */

	if( cm_is_not_null(channel->usr.sv.curr) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "another request still running, opc(%u)"
			, CM_RQB_GET_OPCODE(channel->usr.sv.curr)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		LSA_UINT16 device_nr;

		rsp = CM_OK;

		for( device_nr = 1; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

			CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

			if( cm_is_not_null(dev) ) {

				CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) not yet removed"
					, device_nr
					);

				rsp = CM_ERR_SEQUENCE;
				/*do not break*/
			}
		}
	}

	/***/

	if( rsp == CM_OK ) {

		channel->state = CM_SV_DELETING; /* note: was a wait-state before AP01271744 */

		cm_sv_server_undo_init(channel);

		channel->state = CM_CH_READY;
	}

	cm_callback(channel, rsp, &rb);
}

/*===========================================================================*/
/*=== SERVER CONTROL ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_control(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_CONTROL_PTR_TYPE param = rb->args.sv.control;
	LSA_UINT8 edd_status = 0;
	LSA_UINT8 edd_mask = 0;
	LSA_UINT16 rsp;

	/* note: channel->state is checked in caller */

	if( cm_is_not_null(channel->usr.sv.curr) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "another request still running, opc(%u)"
			, CM_RQB_GET_OPCODE(channel->usr.sv.curr)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		if( (param->cmd & CM_SV_CONTROL_CMD_DATA_STATUS_STOP) != 0 ) {

			edd_status |= 0;
			edd_mask   |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
		}
		else if( (param->cmd & CM_SV_CONTROL_CMD_DATA_STATUS_RUN) != 0 ) {

			edd_status |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
			edd_mask   |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
		}

		/* note: "station ok/failure" is controlled per AR, not globally (AP00349091) */

		if( edd_mask == 0 ) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "invalid cmd(0x%04x)"
				, param->cmd
				);

			rsp = CM_ERR_PARAM;
		}
		else {

			rsp = CM_OK;
		}
	}

	/***/

	if( rsp != CM_OK ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		channel->usr.sv.curr = rb; /* pending */

		/***/

		{
		CM_EDD_LOWER_RQB_PTR_TYPE edd = channel->usr.sv.edd_rqb;
		EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE edd_glob = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE);

		edd_glob->GroupID = CM_PROVIDER_GROUP_IOD;
		edd_glob->Status  = edd_status;
		edd_glob->Mask    = edd_mask;

		cm_edd_request_lower(channel, EDD_SRV_SRT_PROVIDER_SET_STATE, channel, edd);
		}
	}
}

/*===========================================================================*/
/*=== GLOBAL PROVIDER-CONTROL DONE ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_acp_global_provider_control_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_CONST_PTR_TYPE edd
) {
	CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "channel-state(%u) edd-rsp(%u)"
		, channel->state, CM_EDD_GET_RESPONSE(edd)
		);

	if(
		channel->usr.sv.edd_rqb != edd
		|| CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK
	) {

		CM_FATAL(); /* how could it fail? */
	}
	else {

		switch( CM_RQB_GET_OPCODE(channel->usr.sv.curr) ) {

		case CM_OPC_SV_CREATE:
			CM_ASSERT(channel->state == CM_SV_CREATING);
			channel->state = CM_SV_CREATED;
			cm_callback(channel, CM_OK, &channel->usr.sv.curr);
			break;

		case CM_OPC_SV_CONTROL:
			cm_callback(channel, CM_OK, &channel->usr.sv.curr);
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}
}

/*===========================================================================*/
/*=== RPC LOWER DONE ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_lower_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	LSA_UINT16 device_nr = CM_RPC_GET_USER_ID_DEV(rpc); /* see CM_RPC_SET_USER_ID_HANDLE_DEV_AR() in cm_rpc_request_lower() */
	LSA_UINT8 ar_nr = CM_RPC_GET_USER_ID_AR(rpc);

	/***/

	if( device_nr == 0 ) { /* opcodes for "no device" (implying "no AR") */

		CM_FATAL(); /* a bug */
	}
	else { /* opcodes for "valid device" */

		if( channel->state != CM_SV_CREATED ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

			if( cm_is_null(dev) ) {

				CM_FATAL(); /* a bug */
			}
			else if( ar_nr == 0 ) {

				switch( CM_RPC_GET_OPCODE(rpc) ) {

				case CLRPC_OPC_SV_REGISTER_IF_OBJ:
					cm_sv_rpc_register_if_obj_done(dev, rpc);
					break;

				case CLRPC_OPC_SV_UNREGISTER_IF_OBJ:
					cm_sv_rpc_unregister_if_obj_done(dev, rpc);
					break;

				case CLRPC_OPC_SV_CALL_IND_PROVIDE:
					if( dev->state != CM_SV_DEV_STATE_REMOVING ) {
						CM_FATAL(); /* a bug */
					}
					else {
						CM_ASSERT(CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK_CANCELLED);
						CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_CHAT, "call-ind-rqb cancelled");
						CmListInsertTail(&dev->res.rpc_callQ, (CM_LIST_ENTRY_PTR_TYPE)rpc);
					}
					break;

				case CLRPC_OPC_SV_CALL_INDICATION:
					if( CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK ) {
						CM_FATAL(); /* not as documented */
					}
					else {
						cm_sv_rpc_call_indication(dev, rpc);
					}
					break;

				default:
					CM_FATAL();
					break;
				}
			}
			else if( ar_nr < 0x7F ) {

#if CM_CFG_MAX_SV_ARS > 0x7F
# error "check this if-then-else"
#endif

				CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_null(argr) ) {

					switch( CM_RPC_GET_OPCODE(rpc) ) {

					case CLRPC_OPC_CL_BIND: /* note: device is not active */
						cm_sv_rpc_bind_done(dev, ar_nr, rpc);
						break;

					case CLRPC_OPC_CL_UNBIND: /* note: device is not active */
						cm_sv_rpc_unbind_done(dev, ar_nr, rpc);
						break;

					default:
						CM_FATAL();
						break;
					}
				}
				else {

					switch( CM_RPC_GET_OPCODE(rpc) ) {

					case CLRPC_OPC_CL_REBIND:
						cm_sv_rpc_rebind_done(argr, rpc);
						break;

					case CLRPC_OPC_CL_CALL:
						cm_sv_rpc_call_confirmation(argr, LSA_FALSE, rpc);
						break;

					case CLRPC_OPC_CL_CANCEL:
						cm_sv_rpc_cancel_confirmation(argr, rpc);
						break;

					default:
						CM_FATAL();
						break;
					}
				}
			}
			else { /* see "ar_nr |= 0x80" in cm_sv_rpc_call_request() */

				CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, CM_CAST_U16(ar_nr & 0x7F));

				if( cm_is_null(argr) ) {

					CM_FATAL();
				}
				else if( CM_RPC_GET_OPCODE(rpc) != CLRPC_OPC_CL_CALL ) {

					CM_FATAL();
				}
				else {

					cm_sv_rpc_call_confirmation(argr, LSA_TRUE, rpc);
				}
			}
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
