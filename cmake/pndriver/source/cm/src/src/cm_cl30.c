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
/*  F i l e               &F: cm_cl30.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for rpc / connect / release / control / lower_done       */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	630
#define CM_MODULE_ID		630

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/

#define CM_PUT_MACADR(dp_, offset_, mac_ptr_)	CM_MEMCPY (((dp_) + offset_), (mac_ptr_), 6)
#define CM_PUT_UUID(dp_, offset_, uuid_)		CM_PUT_UUID_HTON (dp_, offset_, uuid_)


/*----------------------------------------------------------------------------*/

#define cm_cl_calc_padding_length(len_)		CM_CAST_U16((4 - ((len_) % 4)) % 4)

/*----------------------------------------------------------------------------*/

static LSA_VOID
cm_cl_rpc_bind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
	);


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_to_addr_info (
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc,
	LSA_UINT16  error_code,
	LSA_UINT16 *lower_rsp,
	LSA_UINT16 *error_cause,
	LSA_UINT32 *pnio_status,
	LSA_UINT32 *nca_status
	)
{
	CLRPC_UPPER_CL_CALL_PTR_TYPE  call = rpc->args.cl.call;

	*lower_rsp = CM_RPC_GET_RESPONSE (rpc);
	*pnio_status = CM_PNIO_ERR_NONE;
	*nca_status = 0;

	switch (*lower_rsp)
	{
	case CLRPC_OK:
		switch (call->pnio_status)
		{
		case CM_PNIO_ERR_NONE:
			*error_cause = CM_ERROR_CAUSE_NONE;
			*pnio_status = CM_PNIO_ERR_NONE; /* good */
			break;

		default:
			*error_cause = CM_ERROR_CAUSE_PNIO_CONFIGURATION;

			if (error_code == CM_PNIO_ERR_CODE_CONNECT)
			{
				switch (call->pnio_status)
				{
				case CM_PNIO_FAULTY_Connect__CMRPC_OutOfARResources():
					*error_cause = CM_ERROR_CAUSE_OUT_OF_AR_RESOURCES;
					break;

				case CM_PNIO_FAULTY_Connect__CMRPC_OutOfProviderConsumerOrAlarmResources():
					*error_cause = CM_ERROR_CAUSE_OUT_OF_CR_RESOURCES;
					break;

				case CM_PNIO_FAULTY_IOCRBlockReq(8):
					*error_cause = CM_ERROR_CAUSE_OUT_OF_CR_RESOURCES;
					break;

				case CM_PNIO_FAULTY_Connect__CMRPC_PdevAlreadyOwned():
					*error_cause = CM_ERROR_CAUSE_PDEV_OWNERSHIP;
					break;

				case CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment():
					*error_cause = CM_ERROR_CAUSE_SYSTEM_REDUNDANCY;
					break;
				
				case CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment():
					*error_cause = CM_ERROR_CAUSE_SYSTEM_REDUNDANCY;
					break;

				default:
					break;
				}
			}

			*pnio_status = call->pnio_status;
			break;
		}
		break;

	case CLRPC_OK_CANCELLED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_APPLICATION_TIMEOUT);
		break;

	case CLRPC_ERR_REJECTED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_REJECTED);
		*nca_status  = call->nca_status;
		break;

	case CLRPC_ERR_FAULTED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_FAULTED);
		*nca_status  = call->nca_status;
		break;

	case CLRPC_ERR_TIMEOUT:
		*error_cause = CM_ERROR_CAUSE_RPC_TIMEOUT;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_TIMEOUT);
		break;

	case CLRPC_ERR_IN_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_IN_ARGS);
		break;

	case CLRPC_ERR_OUT_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_OUT_ARGS);
		break;

	case CLRPC_ERR_DECODE:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_DECODE);
		break;

	case CLRPC_ERR_PNIO_OUT_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_PNIO_OUT_ARGS);
		break;

	case CLRPC_ERR_OPCODE:
		CM_FATAL1(CM_RPC_GET_OPCODE(rpc));
		return;

	default:
		if (call->ndr_len < CLRPC_PNIO_OFFSET)
		{
			*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
			*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_PNIO_OUT_ARGS);
		}
		else
		{
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check: is this a possible case? if yes, do better code2")
#endif /* CM_MESSAGE */
			*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
			*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, 0xFF);
		}
		break;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_mod_state_check (
	LSA_UINT16  mod_state
	)
{
	switch (mod_state) {
	case CM_MOD_STATE_NO_MODULE:
	case CM_MOD_STATE_WRONG_MODULE:
	case CM_MOD_STATE_PROPER_MODULE:
	case CM_MOD_STATE_SUBSTITUTED_MODULE:
		return LSA_TRUE;

	default:
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "unknown mod_state(%u)", mod_state);
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_sub_state_check_nt (
	LSA_UINT16  mod_state,
	LSA_UINT16  sub_state
	)
{
	if ((sub_state & CM_SUB_STATE_FORMAT_MASK) != CM_SUB_STATE_FORMAT_1) {
		return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_ADD_INFO_MASK) {
	case CM_SUB_STATE_ADD_INFO_NONE:					break;
	case CM_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED:	break;
	default:
		/* CM_SUB_STATE_ADD_INFO_MASK: not checked (61158.v23) ??? */
		return LSA_FALSE;
	}

	/* CM_SUB_STATE_ADVICE_MASK: all values valid */

	/* CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK: all values valid */

	/* CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK: all values valid */

	/* CM_SUB_STATE_DIAG_INFO_MASK: all values valid */

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK) {
	case CM_SUB_STATE_AR_INFO_OWNED:					break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:		break;
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED:	break;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:		break;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:		break;
	default:
		return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:							break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:					break;
	case CM_SUB_STATE_IDENT_WRONG:						break;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:				break;
	default:
		return LSA_FALSE;
	}

	/***/

	switch (mod_state) {
	case CM_MOD_STATE_NO_MODULE:
		switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
		case CM_SUB_STATE_IDENT_NO_SUBMODULE:			break;/*AP00350631*/

		case CM_SUB_STATE_IDENT_OK:
		case CM_SUB_STATE_IDENT_SUBSTITUTE:
		case CM_SUB_STATE_IDENT_WRONG:
		default:
			return LSA_FALSE;
		}
		break;

	case CM_MOD_STATE_WRONG_MODULE:
		switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
		case CM_SUB_STATE_IDENT_WRONG:					break;/*AP00841926*/
		case CM_SUB_STATE_IDENT_NO_SUBMODULE:			break;/*AP00841926*/

		case CM_SUB_STATE_IDENT_OK:
		case CM_SUB_STATE_IDENT_SUBSTITUTE:
		default:
			return LSA_FALSE;
		}
		break;

	case CM_MOD_STATE_PROPER_MODULE:
	case CM_MOD_STATE_SUBSTITUTED_MODULE:
		break;

	default:
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_sub_state_check (
	LSA_UINT16  mod_state,
	LSA_UINT16  sub_state
	)
{
	LSA_BOOL ok = cm_sub_state_check_nt (mod_state, sub_state);

	if (!ok)
	{
		CM_CL_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "sub_state(0x%x) with wrong format / used mod_state(0x%x)", sub_state, mod_state
			);
	}

	return ok;
}


/*----------------------------------------------------------------------------*/
/* AP00219434 */
static LSA_UINT8
cm_cl_guess_dev_type (
	CM_RPC_MEM_U8_PTR_TYPE  dp,
	LSA_UINT32  left
	)
{
	LSA_UINT16  api_cnt;
	LSA_UINT16  cm_dp_assert_line = 0;

	cm_dp_assert (left >= CM_RSP_MDB_SIZE);

	api_cnt = CM_GET16_NTOH(dp, CM_RSP_MDB_number_of_apis);

	cm_dp_adjust (dp, left, CM_RSP_MDB_SIZE);

	while (api_cnt > 0) {
		LSA_UINT32  api;
		LSA_UINT16  mod_cnt;

		cm_dp_assert (left >= CM_RSP_API_SIZE);

		api     = CM_GET32_NTOH (dp, CM_RSP_API_api);
		mod_cnt = CM_GET16_NTOH (dp, CM_RSP_API_number_of_modules);

		LSA_UNUSED_ARG (api);

		cm_dp_adjust (dp, left, CM_RSP_API_SIZE);

		while (mod_cnt > 0) {
			LSA_UINT16  slot_nr, mod_state, sub_cnt;
			LSA_UINT32  mod_ident;

			cm_dp_assert (left >= CM_RSP_MOD_SIZE);

			slot_nr   = CM_GET16_NTOH (dp, CM_RSP_MOD_slot_nr);
			mod_ident = CM_GET32_NTOH (dp, CM_RSP_MOD_mod_ident);
			mod_state = CM_GET16_NTOH (dp, CM_RSP_MOD_mod_state);
			sub_cnt   = CM_GET16_NTOH (dp, CM_RSP_MOD_number_of_submodules);

			LSA_UNUSED_ARG (slot_nr);
			LSA_UNUSED_ARG (mod_ident);
			LSA_UNUSED_ARG (mod_state);

			cm_dp_adjust (dp, left, CM_RSP_MOD_SIZE);

			if (sub_cnt > 0) {
				LSA_UINT16  subslot_nr, sub_state;
				LSA_UINT32  sub_ident;

				cm_dp_assert (left >= CM_RSP_SUB_SIZE);

				subslot_nr = CM_GET16_NTOH (dp, CM_RSP_SUB_subslot_nr);
				sub_ident  = CM_GET32_NTOH (dp, CM_RSP_SUB_sub_ident);
				sub_state  = CM_GET16_NTOH (dp, CM_RSP_SUB_sub_state);

				LSA_UNUSED_ARG (subslot_nr);
				LSA_UNUSED_ARG (sub_ident);

				cm_dp_adjust (dp, left, CM_RSP_SUB_SIZE);

				switch (sub_state & CM_SUB_STATE_FORMAT_MASK) {
				case CM_SUB_STATE_FORMAT_0:
					CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_CHAT
						, "subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x) => CM_CL_DEV_TYPE_V2"
						, subslot_nr, sub_ident, sub_state
						);
					return CM_CL_DEV_TYPE_V2;

				case CM_SUB_STATE_FORMAT_1:
					CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_CHAT
						, "subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x) => CM_CL_DEV_TYPE_V3"
						, subslot_nr, sub_ident, sub_state
						);
					return CM_CL_DEV_TYPE_V3;

				default:
					CM_FATAL1 (sub_state);
					break;
				}
			}
			mod_cnt -= 1;
		}
		api_cnt -= 1;
	}
	return CM_CL_DEV_TYPE_UNKNOWN;

cm_dp_assert_failed:

	CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
		, "CM_CL_DEV_TYPE_UNKNWON or assertion in line(%u)"
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	return CM_CL_DEV_TYPE_UNKNOWN;
}


/*----------------------------------------------------------------------------*/
/* AP00219434 */
static LSA_VOID
cm_ar_graph_set_ist_to_proper (
	CM_AR_GRAPH_CONST_PTR_TYPE	argr
	)
{
	CM_LIST_ENTRY_PTR_TYPE  inner_sub;

	for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
	{
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

		sub->mod->ist.mod_ident = sub->mod->soll.mod_ident;

		sub->cl.real_sub_state = CM_SUB_STATE_GOOD; /* default */
		sub->cl.real_sub_ident = sub->soll.sub_ident;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16
cm_cl_marshal_connect (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_MEM_U8_PTR_TYPE  data,
	LSA_UINT16  data_max,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	CM_RPC_MEM_U8_PTR_TYPE  dp, dp_len;
	LSA_UINT16  len_cnt;
	LSA_UINT32  left;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioap;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;
	LSA_UINT16  cm_dp_assert_line;

	LSA_UINT16  cmi_station_name_length;
	CM_UPPER_MEM_U8_PTR_TYPE  cmi_station_name;
	LSA_UINT16  cmi_max_alarm_data_length;

	CM_ASSERT (cm_is_not_null (argr));

	if ( ! cm_oha_get_name_of_station (channel, & cmi_station_name_length, & cmi_station_name))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "name_of_station not valid, changed in meantime?");
		return 0;
	}

	cmi_max_alarm_data_length	= channel->acp.max_alarm_data_length; /* alcr->max_alarm_data_len will store response */

	dp = data;
	left = data_max;

	/* 	see cm_arcb.h#IODConnectReq */

	/* --- ARBlockReq --- */
	cm_dp_assert (left >= CM_REQ_AR_SIZE);

	CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_AR_REQ);
	CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_CAST_U16(CM_REQ_AR_SIZE - CM_BLK_HEADER_TypLen_SIZE + cmi_station_name_length));
	CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

	CM_PUT16_HTON (dp, CM_REQ_AR_ar_type,					argr->ar.ar_type);
	CM_PUT_UUID   (dp, CM_REQ_AR_ar_uuid,					& argr->ar.ar_uuid);
	CM_PUT16_HTON (dp, CM_REQ_AR_session_key,				argr->cm.cl.connect_counter);
	CM_PUT_MACADR (dp, CM_REQ_AR_cmi_mac,					edd_params->MACAddr.MacAdr);
	CM_PUT_UUID   (dp, CM_REQ_AR_cmi_obj_uuid,				& argr->ar.cmi_obj_uuid);
	CM_PUT32_HTON (dp, CM_REQ_AR_ar_properties,				argr->ar.ar_properties);
	CM_PUT16_HTON (dp, CM_REQ_AR_cmi_activity_timeout100ms,	argr->ar.cmi_timeout100ms);
	CM_PUT16_HTON (dp, CM_REQ_AR_cmi_udp_port,				CM_NTOH16(argr->ar.cmi_udp_port));
	CM_PUT16_HTON (dp, CM_REQ_AR_cmi_name_length,			cmi_station_name_length);

	cm_dp_adjust (dp, left, CM_REQ_AR_SIZE);


	cm_dp_assert (left >= cmi_station_name_length);

	CM_MEMCPY (dp, cmi_station_name, cmi_station_name_length);

	cm_dp_adjust (dp, left, cmi_station_name_length);


	/* --- [IOCRBlockReq*] --- */
	{
		CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

		for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
		{
			/* IOCR 1 */
			cm_dp_assert (left >= CM_REQ_IOCR1_SIZE);

			dp_len = dp; /* keep for block length calculation */

			CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_IOCR_REQ);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	0);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_iocr_type,			iocr->iocr_type);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_iocr_ref,			iocr->iocr_ref);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_ethertype,			iocr->ethertype);
			CM_PUT32_HTON (dp, CM_REQ_IOCR1_iocr_properties,	iocr->iocr_properties);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_data_length,		iocr->data_length);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_frame_id,			iocr->frame_id);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_send_clock,			iocr->send_clock);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_reduction_ratio,	iocr->reduction_ratio);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_phase,				iocr->phase);
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_sequence,			0); /* AP01104502 */
			CM_PUT32_HTON (dp, CM_REQ_IOCR1_frame_send_offset,	iocr->frame_send_offset);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_watchdog_factor_legacy,	iocr->data_hold_factor); /* AP01304706 */
			CM_PUT16_HTON (dp, CM_REQ_IOCR1_data_hold_factor,	iocr->data_hold_factor);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_iocr_tag_header,	CM_CAST_U16(0xC000 | 0)); /* prio 6 + vlan_id=0 (TIA 208236) */
			CM_PUT_MACADR (dp, CM_REQ_IOCR1_iocr_multicast_mac,	& iocr->multicast_mac);

			CmListLength (& iocr->related_ap_list, & len_cnt);

			CM_PUT16_HTON (dp, CM_REQ_IOCR1_number_of_related_apis,	len_cnt);

			cm_dp_adjust (dp, left, CM_REQ_IOCR1_SIZE);

			/* IOCR 2 */
			for (CmListForeach(ioap, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE))
			{
				cm_dp_assert (left >= CM_REQ_IOCR2_SIZE);

				CM_PUT32_HTON (dp, CM_REQ_IOCR2_api, ioap->api);

				CmListLength (& ioap->related_iodata_objects_list, & len_cnt);

				CM_PUT16_HTON (dp, CM_REQ_IOCR2_number_of_related_iodata_objects,	len_cnt);

				cm_dp_adjust (dp, left, CM_REQ_IOCR2_SIZE);


				/* IOCR 3 */
				for (CmListForeach(iodata, & ioap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
				{
					cm_dp_assert (left >= CM_REQ_IOCR3_SIZE);

					CM_PUT16_HTON (dp, CM_REQ_IOCR3_slot_nr,		iodata->slot_nr);
					CM_PUT16_HTON (dp, CM_REQ_IOCR3_subslot_nr,		iodata->subslot_nr);
					CM_PUT16_HTON (dp, CM_REQ_IOCR3_frame_offset,	iodata->frame_offset);

					cm_dp_adjust (dp, left, CM_REQ_IOCR3_SIZE);
				}

				/* IOCR 4 */
				cm_dp_assert (left >= CM_REQ_IOCR4_SIZE);

				CmListLength (& ioap->related_iocs_list, & len_cnt);

				CM_PUT16_HTON (dp, CM_REQ_IOCR4_number_of_related_iocs, len_cnt);

				cm_dp_adjust (dp, left, CM_REQ_IOCR4_SIZE);


				/* IOCR 5 */
				for (CmListForeach(iocs, & ioap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE))
				{
					cm_dp_assert (left >= CM_REQ_IOCR5_SIZE);

					CM_PUT16_HTON (dp, CM_REQ_IOCR5_slot_nr,		iocs->slot_nr);
					CM_PUT16_HTON (dp, CM_REQ_IOCR5_subslot_nr,		iocs->subslot_nr);
					CM_PUT16_HTON (dp, CM_REQ_IOCR5_frame_offset,	iocs->frame_offset);

					cm_dp_adjust (dp, left, CM_REQ_IOCR5_SIZE);
				}
			}

			/* set block length */
			len_cnt = CM_CAST_U16(dp - dp_len);
			len_cnt -= CM_BLK_HEADER_TypLen_SIZE;
			CM_PUT16_HTON (dp_len, CM_BLK_HEADER_length, len_cnt);
		}
	}


	/* --- [MCRBlockReq*] --- */
	{
		CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

		for (CmListForeach (mcr, & argr->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE))
		{
			cm_dp_assert (left >= CM_REQ_MCR_SIZE);

			dp_len = dp; /* keep for block length calculation */

			CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_MCR_REQ);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	0);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

			CM_PUT16_HTON (dp, CM_REQ_MCR_iocr_ref,				mcr->iocr_ref);
			CM_PUT32_HTON (dp, CM_REQ_MCR_mcr_properties,		mcr->mcr_properties);
			CM_PUT16_HTON (dp, CM_REQ_MCR_mci_timeout_factor,	mcr->mci_timeout_100ms);
			CM_PUT8_HTON  (dp, CM_REQ_MCR_station_name_length,	mcr->provider_name_length);

			cm_dp_adjust (dp, left, CM_REQ_MCR_SIZE);

			cm_dp_assert (left >= mcr->provider_name_length);

			CM_MEMCPY (dp, mcr->provider_name, mcr->provider_name_length);

			cm_dp_adjust (dp, left, mcr->provider_name_length);

			/* set block length */
			len_cnt = CM_CAST_U16(dp - dp_len);
			len_cnt -= CM_BLK_HEADER_TypLen_SIZE;
			CM_PUT16_HTON (dp_len, CM_BLK_HEADER_length, len_cnt);
		}
	}


	/* --- [ExpectedSubmoduleBlockReq*] --- */
	{
		CM_AR_GRAPH_SLOT_NR_PTR_TYPE  unique;

		CM_RPC_MEM_U8_PTR_TYPE  dp_related;
		LSA_UINT16  nr_related;

		/* for all unique slot_nr */
		for (CmListForeach (unique, &argr->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE))
		{
			CM_AR_GRAPH_AP_PTR_TYPE  ap;

			/* unique->slot_nr  must(shall lt. IEC)  static for this block */

			/* CM_REQ_API */
			cm_dp_assert (left >= CM_REQ_API_SIZE);

			dp_len = dp; /* keep for block length calculation */
			nr_related = 0;

			CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_EXPSUBMOD_REQ);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	0);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

			dp_related = dp; /* keep for number_of_related_apis */

			CM_PUT16_HTON(dp, CM_REQ_API_number_of_related_apis, nr_related);

			cm_dp_adjust (dp, left, CM_REQ_API_SIZE);


				/* search all ap's which contains the unique->slot_nr */
			for (CmListForeach(ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
			{
				mod = cm_ar_graph_find_module (ap, unique->slot_nr);
				if (cm_is_null (mod))
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "ap(%u) does not contain slot_nr(%u)"
						, ap->api, unique->slot_nr
						);
					continue;
				}
				/* must only contained once, see InsertSorted */
				CM_ASSERT (unique->slot_nr == mod->slot_nr);

				nr_related += 1;
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "ap(%u) contains slot_nr(%u), nr_related(%u)"
					, ap->api, unique->slot_nr, nr_related
					);

				/* CM_REQ_MOD */
				cm_dp_assert (left >= CM_REQ_MOD_SIZE);

				CmListLength (& mod->submod_list, & len_cnt);

				CM_PUT32_HTON (dp, CM_REQ_MOD_api,					ap->api);
				CM_PUT16_HTON (dp, CM_REQ_MOD_slot_nr,				mod->slot_nr);
				CM_PUT32_HTON (dp, CM_REQ_MOD_mod_ident,			mod->soll.mod_ident);
				CM_PUT16_HTON (dp, CM_REQ_MOD_mod_properties,		mod->soll.mod_properties);
				CM_PUT16_HTON (dp, CM_REQ_MOD_number_of_submodules,	len_cnt);

				mod->ist.mod_ident = mod->soll.mod_ident;

				cm_dp_adjust (dp, left, CM_REQ_MOD_SIZE);

				/* CM_REQ_SUB */
				for (CmListForeach(sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					cm_dp_assert (left >= CM_REQ_SUB_SIZE);

					CM_PUT16_HTON (dp, CM_REQ_SUB_subslot_nr,		sub->subslot_nr);
					CM_PUT32_HTON (dp, CM_REQ_SUB_sub_ident,		sub->soll.sub_ident);
					CM_PUT16_HTON (dp, CM_REQ_SUB_sub_properties,	sub->soll.sub_properties);

					sub->cl.real_sub_state = CM_SUB_STATE_GOOD; /* default */
					sub->cl.real_sub_ident = sub->soll.sub_ident;

					cm_dp_adjust (dp, left, CM_REQ_SUB_SIZE);

					/* CM_REQ_DSC - INPUT */
					switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK)
					{
					case CM_SUB_PROP_TYPE_NO_DATA:
					case CM_SUB_PROP_TYPE_INPUT:
					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						cm_dp_assert (left >= CM_REQ_DSC_SIZE);

						CM_PUT16_HTON (dp, CM_REQ_DSC_data_description,	sub->descr[0].data_description);
						CM_PUT16_HTON (dp, CM_REQ_DSC_data_length,		sub->descr[0].data_length);
						CM_PUT8_HTON  (dp, CM_REQ_DSC_iops_length,		sub->descr[0].iops_length);
						CM_PUT8_HTON  (dp, CM_REQ_DSC_iocs_length,		sub->descr[0].iocs_length);

						cm_dp_adjust (dp, left, CM_REQ_DSC_SIZE);
						break;

					default:
						/* no input descr */
						break;
					}

					/* CM_REQ_DSC - OUTPUT */
					switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK)
					{
					case CM_SUB_PROP_TYPE_OUTPUT:
					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						cm_dp_assert (left >= CM_REQ_DSC_SIZE);

						CM_PUT16_HTON (dp, CM_REQ_DSC_data_description,	sub->descr[1].data_description);
						CM_PUT16_HTON (dp, CM_REQ_DSC_data_length,		sub->descr[1].data_length);
						CM_PUT8_HTON  (dp, CM_REQ_DSC_iops_length,		sub->descr[1].iops_length);
						CM_PUT8_HTON  (dp, CM_REQ_DSC_iocs_length,		sub->descr[1].iocs_length);

						cm_dp_adjust (dp, left, CM_REQ_DSC_SIZE);
						break;

					default:
						/* no output descr */
						break;
					}
				}
			}

			/* set number_of_related_apis */
			CM_PUT16_HTON (dp_related, CM_REQ_API_number_of_related_apis, nr_related);

			/* set block length */
			len_cnt = CM_CAST_U16(dp - dp_len);
			len_cnt -= CM_BLK_HEADER_TypLen_SIZE;
			CM_PUT16_HTON (dp_len, CM_BLK_HEADER_length, len_cnt);
		}
	}


	/* --- [AlarmCRBlockReq] --- */
	{
		CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alcr = argr->alarm_cr;

		cm_dp_assert (cm_is_not_null (alcr));

		/* CM_REQ_ALCR */
		cm_dp_assert (left >= CM_REQ_ALCR_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_ALCR_REQ);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_REQ_ALCR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

		CM_PUT16_HTON (dp, CM_REQ_ALCR_alcr_type,				alcr->alcr_type);
		CM_PUT16_HTON (dp, CM_REQ_ALCR_ethertype,				alcr->ethertype);
		CM_PUT32_HTON (dp, CM_REQ_ALCR_alcr_properties,			alcr->alcr_properties);
		CM_PUT16_HTON (dp, CM_REQ_ALCR_timeout100ms,			CM_CAST_U16(alcr->timeout100ms));
		CM_PUT16_HTON (dp, CM_REQ_ALCR_retries,					CM_CAST_U16(alcr->retries));
		CM_PUT16_HTON (dp, CM_REQ_ALCR_local_alarm_ref,			alcr->local_alarm_ref);
		CM_PUT16_HTON (dp, CM_REQ_ALCR_max_alarm_data_length,	cmi_max_alarm_data_length); /* alcr->max_alarm_data_len will store response */
		CM_PUT16_HTON (dp, CM_REQ_ALCR_alcr_tag_header_high,	CM_CAST_U16(0xC000 | 0)); /* prio 6 + vlan_id=0 (TIA 208236) */
		CM_PUT16_HTON (dp, CM_REQ_ALCR_alcr_tag_header_low,		CM_CAST_U16(0xA000 | 0)); /* prio 5 + vlan_id=0 (TIA 208236) */

		cm_dp_adjust (dp, left, CM_REQ_ALCR_SIZE);
	}


	/* --- [PrmServerBlock] --- */

	/* not yet */


	/* --- [ARRPCBlockReq] --- */

	/* not yet */


	/* --- [IRInfoBlock] --- */
	if (cm_is_not_null (argr->ir_info))
	{
		LSA_UINT16 subCount;

		CmListLength (&(argr->ir_info->ir_info_sub_list), &subCount);

		cm_dp_assert (left >= CM_REQ_IRINFO_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_IRINFOBLOCK);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_CAST_U16(CM_REQ_IRINFO_SIZE - CM_BLK_HEADER_TypLen_SIZE + subCount * CM_REQ_IRINFOSUB_SIZE));
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

		CM_PUT16_HTON (dp, CM_REQ_IRINFO_padding1,			0);
		CM_PUT_UUID   (dp, CM_REQ_IRINFO_ir_data_uuid,		& argr->ir_info->ir_data_uuid);
		CM_PUT16_HTON (dp, CM_REQ_IRINFO_padding2,			0);
		CM_PUT16_HTON (dp, CM_REQ_IRINFO_number_of_iocrs,	subCount);

		cm_dp_adjust (dp, left, CM_REQ_IRINFO_SIZE);

		if (0 != subCount)
		{
			CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE irInfoSub;

			for (CmListForeach(irInfoSub, &argr->ir_info->ir_info_sub_list, CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE))
			{
				cm_dp_assert (left >= CM_REQ_IRINFOSUB_SIZE);

				CM_PUT16_HTON (dp, CM_REQ_IRINFOSUB_iocr_reference,		irInfoSub->iocr_ref);
				CM_PUT16_HTON (dp, CM_REQ_IRINFOSUB_subframe_offset,	irInfoSub->subframe_offset);
				CM_PUT32_HTON (dp, CM_REQ_IRINFOSUB_subframe_data,		irInfoSub->subframe_data);

				cm_dp_adjust (dp, left, CM_REQ_IRINFOSUB_SIZE);
			}
		}
	}


	/* --- [SRInfoBlock] --- */
	if (cm_is_not_null (argr->sr_info))
	{
		cm_dp_assert (left >= CM_REQ_SRINFO_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_SRINFOBLOCK);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_REQ_SRINFO_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

		CM_PUT16_HTON (dp, CM_REQ_SRINFO_RedundancyDataHoldFactor,	argr->sr_info->rdh_factor);
		CM_PUT32_HTON (dp, CM_REQ_SRINFO_SRProperties,				argr->sr_info->sr_properties);

		cm_dp_adjust (dp, left, CM_REQ_SRINFO_SIZE);
	}


	/* --- [ARVendorBlockReq*] --- */
	{
		CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

		/* BlockHeader, APStructureIdentifier, API, [Data*], [Padding* a] */

		for (CmListForeach(arvendor, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
		{
			cm_dp_assert (left >= CM_REQ_ARVENDOR_SIZE);

			dp_len = dp; /* keep for block length calculation */

			CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_ARVENDORBLOCK_REQ);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	0);
			CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

			CM_PUT16_HTON (dp, CM_REQ_ARVENDOR_APStructureIdentifier,	arvendor->ap_structure_identifier);
			CM_PUT32_HTON (dp, CM_REQ_ARVENDOR_API,						arvendor->api);

			cm_dp_adjust (dp, left, CM_REQ_ARVENDOR_SIZE);

			cm_dp_assert (left >= arvendor->req_data_len);

			CM_MEMCPY (dp, arvendor->data, arvendor->req_data_len);

			cm_dp_adjust (dp, left, arvendor->req_data_len);

			/* a The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
			{
				/* [Data*] starts aligned, so [Padding* a] depends only on data_len */
				LSA_INT  padding = cm_cl_calc_padding_length (arvendor->req_data_len);
				while (padding > 0)
				{
					cm_dp_assert (left >= 1);
					CM_PUT8_HTON (dp, 0, 0);
					--padding;
					cm_dp_adjust (dp, left, 1);
				}
			}

			/* set block length */
			len_cnt = CM_CAST_U16(dp - dp_len);
			len_cnt -= CM_BLK_HEADER_TypLen_SIZE;
			CM_PUT16_HTON (dp_len, CM_BLK_HEADER_length, len_cnt);

			/* expect an ARVendorBlockRes */
			arvendor->res_data_len = 0;
		}
	}


	/* --- [ARFSUBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
	{
		CM_PD_RECORD_PTR_TYPE ptr;
		for (CmListForeach (ptr, &argr->cm.cl.ar_connect_prm_record_list, CM_PD_RECORD_PTR_TYPE))
		{
			if (ptr->record_index == 0xE050 /* ARFSUDataAdjust data */)
			{
				cm_dp_assert (left >= ptr->record_data_len);

				CM_MEMCPY (dp, ptr->record_data, ptr->record_data_len);

				CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_ARFSUBLOCK);
			/*	CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_GET16_NTOH (ptr->record_data, 2));*/
				CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

				cm_dp_adjust (dp, left, ptr->record_data_len);
			}
		}
	}


	/* --- [RSInfoBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
		&& cm_is_not_null (argr->rs_info)  &&  argr->rs_info->rs_info_data_len != 0)
	{
		cm_dp_assert (left >= argr->rs_info->rs_info_data_len);

		CM_MEMCPY (dp, argr->rs_info->rs_info_data, argr->rs_info->rs_info_data_len);

		cm_dp_adjust (dp, left, argr->rs_info->rs_info_data_len);
	}


	/* returns written data */
	return CM_CAST_U16(dp - data);

cm_dp_assert_failed:
	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);
	return 0;
}


/*----------------------------------------------------------------------------*/
/*	returns:
 *	0								.. Ok
 *	! is_appl_ready:
 *		CM_AR_REASON_xxx			.. Not Ok
 *	is_appl_ready:
 *		CM_PNIO_ERR_CODE_12(ErrorCode1, ErrorCode2)	.. Not Ok, see "Values of ErrorCode1 and ErrorCode2 for ErrorDecode with the value PNIO"
 */
static LSA_UINT16
cm_cl_rpc_connect_parse_response (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_MEM_U8_PTR_TYPE  dp_start,
	LSA_UINT32  dp_len,
	LSA_BOOL  is_appl_ready
	)
{
	LSA_UINT16  block_type, block_length, block_version;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  cm_dp_assert_line = 0;
	LSA_UINT16  cm_dp_assert_error = 0;
	LSA_BOOL  block_ar_seen = LSA_FALSE;
	LSA_BOOL  block_iocr_seen = LSA_FALSE;
	LSA_BOOL  block_alcr_seen = LSA_FALSE;
	LSA_BOOL  block_moddiff_seen = LSA_FALSE;
	LSA_BOOL  block_arserver_seen = LSA_FALSE;

	dp = dp_start;
	left = dp_len;
	block_type = 0xCCCC;

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("TODO: change all returns like ModuleDiffBlock does !")
#endif /* CM_MESSAGE */

	while (left > 0)
	{
		cm_dp_assert (left >= CM_BLK_HEADER_SIZE);

		block_type =	CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
		block_length =	CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
		block_version =	CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "BLOCK: type(0x%04x) len(%u) version(0x%04x)"
			, block_type, block_length, block_version
			);

		cm_dp_assert (block_length >= sizeof(block_version));
		cm_dp_assert (left >= (block_length - sizeof(block_version)));

		if (is_appl_ready)
		{
			if (block_type != CM_BLOCK_TYPE_MOD_DIFF)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "is_appl_ready and wrong block_type(0x%x)", block_type);
				return CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/);
			}
		}
		else if (! block_ar_seen)
		{
			if (block_type != CM_BLOCK_TYPE_AR_RSP)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "is_connect_rsp and wrong block_type(0x%x)", block_type);
				return CM_AR_REASON_PROT;
			}
		}

		switch (block_type)
		{
		case CM_BLOCK_TYPE_AR_RSP: /* ARBlockRes */
			{
			LSA_UINT16  ar_type, session_key;
			CLRPC_UUID_TYPE  ar_uuid;
			CM_MAC_TYPE  cmr_mac;
			LSA_UINT16  cmr_udp_port;

			if (block_ar_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARBlockRes: twice");
				return CM_AR_REASON_PROT;
			}
			block_ar_seen = LSA_TRUE;

			cm_dp_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_dp_assert (left >= CM_RSP_AR_SIZE);

			ar_type = CM_GET16_NTOH (dp, CM_RSP_AR_ar_type);
			CM_GET_UUID_NTOH (&ar_uuid, dp, CM_RSP_AR_ar_uuid);
			session_key = CM_GET16_NTOH (dp, CM_RSP_AR_session_key);
			CM_MEMCPY (&cmr_mac, dp + CM_RSP_AR_cmr_mac, CM_PDU_MAC_SIZE);
			cmr_udp_port = CM_GET16_NTOH (dp, CM_RSP_AR_cmr_udp_port);

			cm_dp_adjust (dp, left, CM_RSP_AR_SIZE);

			CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARBlockRes: ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
				, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
				, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
				, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
				, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
				);
			CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARBlockRes: type(%u) session_key(%u) cmr_mac(%02x-%02x-%02x-%02x-%02x-%02x) cmr_udp(%u)"
				, ar_type
				, session_key
				, cmr_mac.mac[0], cmr_mac.mac[1], cmr_mac.mac[2]
				, cmr_mac.mac[3], cmr_mac.mac[4], cmr_mac.mac[5]
				, cmr_udp_port
				);

			if (! cm_uuid_equal (& ar_uuid, & argr->ar.ar_uuid)) {
				CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARBlockRes: dev.ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) does not match"
					, argr->ar.ar_uuid.time_low, argr->ar.ar_uuid.time_mid, argr->ar.ar_uuid.time_hi_and_version
					, argr->ar.ar_uuid.clock_seq_hi_and_reserved, argr->ar.ar_uuid.clock_seq_low
					, argr->ar.ar_uuid.node[0], argr->ar.ar_uuid.node[1], argr->ar.ar_uuid.node[2]
					, argr->ar.ar_uuid.node[3], argr->ar.ar_uuid.node[4], argr->ar.ar_uuid.node[5]
					);
				return CM_AR_REASON_PROT;
			}

			if (session_key != argr->cm.cl.connect_counter) {
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARBlockRes: session_key(%u) != connect_counter(%u)"
					, session_key, argr->cm.cl.connect_counter
					);
				return CM_AR_REASON_PROT;
			}

			if (! cm_ar_graph_update_io_ar (argr, cmr_mac.mac, CM_HTON16(cmr_udp_port))) {
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARBlockRes: cannot cm_ar_graph_update_io_ar");
				return CM_AR_REASON_PROT;
			}
			}
			break;

		case CM_BLOCK_TYPE_IOCR_RSP:	/* IOCRBlockRes */
			{
			LSA_UINT16  iocr_type, iocr_ref, frame_id;

			block_iocr_seen = LSA_TRUE;

			cm_dp_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_dp_assert (left >= CM_RSP_IOCR_SIZE);

			iocr_type = CM_GET16_NTOH( dp, CM_RSP_IOCR_iocr_type);
			iocr_ref  = CM_GET16_NTOH( dp, CM_RSP_IOCR_iocr_ref);
			frame_id  = CM_GET16_NTOH( dp, CM_RSP_IOCR_frame_id);

			cm_dp_adjust (dp, left, CM_RSP_IOCR_SIZE);

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "IOCRBlockRes: type(%u) ref(%u) frame_id(%u)"
				, iocr_type, iocr_ref, frame_id
				);

			if (! cm_ar_graph_update_io_cr (argr, iocr_type, iocr_ref, frame_id)) {
				cm_dp_goto_failed (0);
			}
			}
			break;

		case CM_BLOCK_TYPE_MOD_DIFF: /* ModuleDiffBlock */
			{
			/*/	prm_mode \ is_appl_rdy	 FALSE								 TRUE
			//	 PRMMODE_CONNECT:		IODConnectRes						IOCControlReq(aka ApplRdy 4 Connect)
			//							 (exp-sub)							 (exp-sub)
			//	 PRMMODE_PLUG:												IOCControlReq(aka ApplRdy 4 Plug)
			//																 (one-sub/plug)
			//	 PRMMODE_AR:			IODControlRes(aka PrmBegin.rsp)		IOCControlReq(aka ApplRdy 4 PrmUpdate)
			//							 (exp-sub)							 (exp-sub)
			//	 PRMMODE_SUBMOD:		IODControlRes(aka PrmBegin.rsp)		IOCControlReq(aka ApplRdy 4 PrmUpdate)
			//							 (one-sub/upd)						 (one-sub/upd)
			*/
			LSA_UINT16  api_cnt;
			LSA_UINT8  dev_type;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE  one_sub = LSA_NULL;

			if (block_moddiff_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ModuleDiffBlock: twice");
				return CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/);
			}
			block_moddiff_seen = LSA_TRUE;

			switch (cm_cl_prm_mode(argr))
			{
			case CM_CL_PRMMODE_IDLE: /* ! note: IDLE means Connect, because PrmWrite is not started yet */
			case CM_CL_PRMMODE_CONNECT:
				one_sub = LSA_NULL;
				break;

			case CM_CL_PRMMODE_PLUG:
				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & one_sub
					);
				cm_dp_assert2 (cm_is_not_null (one_sub), CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				break;

			case CM_CL_PRMMODE_AR:
				one_sub = LSA_NULL;
				break;

			case CM_CL_PRMMODE_SUBMODULE:
				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & one_sub
					);
				cm_dp_assert2 (cm_is_not_null (one_sub), CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				break;

			case CM_CL_PRMMODE_COC:
				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & one_sub
					);
				cm_dp_assert2 (cm_is_not_null (one_sub), CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				break;

			default:
				cm_dp_goto_failed (CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
			}

			/* please note ArtsRQ:
			**  AP00219434: allow all submodule_state within appl_rdy.ind
			**              // but not connect.rsp/NO => appl_rdy.ind/PROPER/SUBST
			**              // accept all ident, even when differ from connect.rsp
			**	AP00350631: auto-convert submodule_state to new format (FormatIndicator == 1)
			**	AP00841926: never generate CM_MOD_STATE_WRONG_MODULE internally & checks according to 61158-6
			**	AP00893748: PDU-Check per AP00841926 ist beim Plug-Alarm zu streng
			**				// e.g. allow a PLUG.alarm followed by a ModDiffBlock with NO_MODULE
			**	AP00895528: allow number_of_modules == 0 (legacy,  version <= v2i7.13)
			**	AP00962427: IEPB Link: Control request wird abgelehnt (legacy,  version <= v2i7.14)
			**				// number_of_module == 1: mod_state == NO_MODULE: number_of_submodule == 1: sub_state == NO_SUBMOD
			*/

			cm_dp_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ModuleDiffBlock(2/*BlockVersionHigh*/));
			cm_dp_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ModuleDiffBlock(3/*BlockVersionLow*/));

			dev_type = cm_cl_guess_dev_type (dp, left);
			if (argr->cm.cl.dev->dev_type == CM_CL_DEV_TYPE_UNKNOWN) {
				argr->cm.cl.dev->dev_type = dev_type;
			}
			if (argr->cm.cl.dev->dev_type != CM_CL_DEV_TYPE_UNKNOWN
				&&  dev_type != CM_CL_DEV_TYPE_UNKNOWN)
			{
				cm_dp_assert2 (argr->cm.cl.dev->dev_type == dev_type, CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/));
			}

			cm_dp_assert2 (left >= CM_RSP_MDB_SIZE, CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

			api_cnt = CM_GET16_NTOH(dp, CM_RSP_MDB_number_of_apis);

			cm_dp_adjust (dp, left, CM_RSP_MDB_SIZE);

			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ModuleDiffBlock: api_cnt(%u)"
				, api_cnt
				);

			if (cm_is_not_null (one_sub))
			{
				cm_dp_assert2 (api_cnt == 1, CM_PNIO_ERR_ModuleDiffBlock(4/*NumberOfAPIs*/));
			}
			else
			{
				cm_dp_assert2 (api_cnt >= 1, CM_PNIO_ERR_ModuleDiffBlock(4/*NumberOfAPIs*/));
			}

			while (api_cnt > 0)
			{
				LSA_UINT32  api;
				LSA_UINT16  mod_cnt;
				CM_AR_GRAPH_AP_PTR_TYPE  ap;

				cm_dp_assert2 (left >= CM_RSP_API_SIZE, CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

				api     = CM_GET32_NTOH (dp, CM_RSP_API_api);
				mod_cnt = CM_GET16_NTOH (dp, CM_RSP_API_number_of_modules);

				cm_dp_adjust (dp, left, CM_RSP_API_SIZE);

				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, " API: api(%u) mod_cnt(%u)"
					, api, mod_cnt
					);

				ap = cm_ar_graph_find_ap (argr, api);
				cm_dp_assert2 (cm_is_not_null (ap), CM_PNIO_ERR_ModuleDiffBlock(5/*API*/));

				if (cm_is_not_null (one_sub))
				{
					cm_dp_assert2 (argr->cm.cl.prm.submod.api == api, CM_PNIO_ERR_ModuleDiffBlock(5/*API*/));
					cm_dp_assert2 (mod_cnt <= 1, CM_PNIO_ERR_ModuleDiffBlock(6/*NumberOfModules*/));
				}

				/* AP00895528: allow number_of_moduls == 0 (legacy, version <= v2i7.13) */

				while (mod_cnt > 0)
				{
					LSA_UINT16  slot_nr, mod_state, number_of_submodules, sub_cnt;
					LSA_UINT32  mod_ident;
					LSA_BOOL  mod_state_ok;
					CM_AR_GRAPH_MODULE_PTR_TYPE  mod;

					cm_dp_assert2 (left >= CM_RSP_MOD_SIZE, CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

					slot_nr				= CM_GET16_NTOH (dp, CM_RSP_MOD_slot_nr);
					mod_ident			= CM_GET32_NTOH (dp, CM_RSP_MOD_mod_ident);
					mod_state			= CM_GET16_NTOH (dp, CM_RSP_MOD_mod_state);
					number_of_submodules= CM_GET16_NTOH (dp, CM_RSP_MOD_number_of_submodules);

					cm_dp_adjust (dp, left, CM_RSP_MOD_SIZE);

					CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "  MOD: slot_nr(%u) mod_ident(0x%x) mod_state(%u) number_of_submodules(%u)"
						, slot_nr, mod_ident, mod_state, number_of_submodules
						);

					mod = cm_ar_graph_find_module (ap, slot_nr);
					cm_dp_assert2 (cm_is_not_null (mod), CM_PNIO_ERR_ModuleDiffBlock(7/*SlotNumber*/));

					if (cm_is_not_null (one_sub))
					{
						cm_dp_assert2 (argr->cm.cl.prm.submod.slot_nr == slot_nr, CM_PNIO_ERR_ModuleDiffBlock(7/*SlotNumber*/));
					}

					/* CM_PNIO_ERR_ModuleDiffBlock(8/ModuleIdentNumber/): not checked */

					mod_state_ok = cm_mod_state_check (mod_state);
					cm_dp_assert2 (mod_state_ok, CM_PNIO_ERR_ModuleDiffBlock(9/*ModuleState*/));

					if (mod_state == CM_MOD_STATE_NO_MODULE)
					{
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						if (number_of_submodules == 0)
						{
							for (CmListForeach(sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
							{
								if (cm_is_not_null (one_sub))
								{
									if (argr->cm.cl.prm.submod.subslot_nr == sub->subslot_nr)
									{
										sub->cl.real_sub_state = CM_SUB_STATE_PULLED;
									}
								}
								else
								{
									sub->cl.real_sub_state = CM_SUB_STATE_PULLED;
								}
							}
						}
						else /* legacy, version <= v2i7.14, see CM_OPC_SV_APPL_READY2(!), AP00962427 */
						{
							if (cm_is_not_null (one_sub))
							{
								cm_dp_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
							else
							{
								LSA_UINT16  exp_sub_count;
								CmListLength (& mod->submod_list, & exp_sub_count);
								cm_dp_assert2 (number_of_submodules == exp_sub_count, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
						}
					}
					else if (mod_state == CM_MOD_STATE_WRONG_MODULE)
					{
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						if (number_of_submodules == 0)
						{
							/* legacy, version <= v2i7.13 */
							for (CmListForeach(sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
							{
								if (cm_is_not_null (one_sub))
								{
									if (argr->cm.cl.prm.submod.subslot_nr == sub->subslot_nr)
									{
										sub->cl.real_sub_state = CM_SUB_STATE_WRONG;
									}
								}
								else
								{
									sub->cl.real_sub_state = CM_SUB_STATE_WRONG;
								}
							}
						}
						else
						{
							if (cm_is_not_null (one_sub))
							{
								cm_dp_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
							else
							{
								LSA_UINT16  exp_sub_count;
								CmListLength (& mod->submod_list, & exp_sub_count);
								cm_dp_assert2 (number_of_submodules == exp_sub_count, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
						}
					}
					else /* PROPER or SUBSTITUTED */
					{
						if (cm_is_not_null (one_sub))
						{
							cm_dp_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
						}
						else
						{
							cm_dp_assert2 (number_of_submodules >= 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
						}
					}

					sub_cnt = number_of_submodules;
					while (sub_cnt > 0)
					{
						LSA_UINT16  subslot_nr, sub_state;
						LSA_UINT32  sub_ident;
						LSA_BOOL  sub_state_ok;
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						cm_dp_assert2 (left >= CM_RSP_SUB_SIZE, CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

						subslot_nr= CM_GET16_NTOH (dp, CM_RSP_SUB_subslot_nr);
						sub_ident = CM_GET32_NTOH (dp, CM_RSP_SUB_sub_ident);
						sub_state = CM_GET16_NTOH (dp, CM_RSP_SUB_sub_state);

						cm_dp_adjust (dp, left, CM_RSP_SUB_SIZE);

						CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "   SUB: subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x)"
							, subslot_nr, sub_ident, sub_state
							);

						sub = cm_ar_graph_find_submodule (ap, mod, slot_nr, subslot_nr);
						cm_dp_assert2 (cm_is_not_null (sub), CM_PNIO_ERR_ModuleDiffBlock(11/*SubslotNumber*/));

						if (cm_is_not_null (one_sub))
						{
							cm_dp_assert2 (argr->cm.cl.prm.submod.subslot_nr == subslot_nr, CM_PNIO_ERR_ModuleDiffBlock(11/*SubslotNumber*/));
						}

						/* , CM_PNIO_ERR_ModuleDiffBlock(12/SubmoduleIdentNumber/): not checked */

						sub_state = cm_sub_state_to_format_1 (sub_state); /* to be compatible to CM V2 */
						sub_state_ok = cm_sub_state_check (mod_state, sub_state);
						cm_dp_assert2 (sub_state_ok, CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/));

						if (is_appl_ready)
						{
							LSA_BOOL prm_old = cm_cl_sub_state_must_prmed (sub->cl.real_sub_state);
							LSA_BOOL prm_new = cm_cl_sub_state_prmed_effect (sub_state);

							if (! prm_old && prm_new)
							{
								CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
									, "ModuleDiffBlock: slot_nr(%u) subslot_nr(0x%x) sub_state_old(%u) sub_state_new(%u), submodule wasn't prm but ApplRdy told us it should prm"
									, slot_nr, subslot_nr, sub->cl.real_sub_state, sub_state
									);
								return CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/);
							}
						}

						/***/

						sub->cl.real_sub_state = sub_state;
						sub->cl.real_sub_ident = sub_ident;

						sub_cnt -= 1;
					}

					/***/

					mod->ist.mod_ident = mod_ident;

					mod_cnt -= 1;
				}
				api_cnt -= 1;
			}
			}
			break;

		case CM_BLOCK_TYPE_ALCR_RSP: /* AlarmCRBlockRes */
			{
			LSA_UINT16  alcr_type, remote_alarm_ref, max_alarm_data_length;
			CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alcr;

			if (block_alcr_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "AlarmCRBlockRes: twice");
				return CM_AR_REASON_PROT;
			}
			block_alcr_seen = LSA_TRUE;

			cm_dp_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_dp_assert (left >= CM_RSP_ALCR_SIZE);

			alcr_type =				CM_GET16_NTOH (dp, CM_RSP_ALCR_alcr_type);
			remote_alarm_ref =		CM_GET16_NTOH (dp, CM_RSP_ALCR_local_alarm_ref);
			max_alarm_data_length =	CM_GET16_NTOH (dp, CM_RSP_ALCR_max_alarm_data_length);

			cm_dp_adjust (dp, left, CM_RSP_ALCR_SIZE);

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "AlarmCRBlockRes: type(%u) remote_alarm_ref(%u) alarm_data_len(%u)"
				, alcr_type, remote_alarm_ref, max_alarm_data_length
				);

			alcr = cm_ar_graph_find_alarm_cr (argr);
			cm_dp_assert (cm_is_not_null (alcr));

			alcr->remote_alarm_ref = remote_alarm_ref;
			alcr->max_alarm_data_len = max_alarm_data_length;

			cm_dp_assert (alcr->remote_alarm_ref != ACP_INVALID_ALARM_REF);
			cm_dp_assert (CM_ALARM_DATA_LENGTH_MIN <= alcr->max_alarm_data_len);
			cm_dp_assert (alcr->max_alarm_data_len <= CM_ALARM_DATA_LENGTH_MAX);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check and/or adapt with channel->acp.max_alarm_data_length")
#endif /* CM_MESSAGE */
			}
			break;

		case CM_BLOCK_TYPE_ARSERVERBLOCK:
			{
			LSA_UINT16  cmr_name_length, i, pad_length;

			if (block_arserver_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARServerBlock: twice");
				return CM_AR_REASON_PROT;
			}
			block_arserver_seen = LSA_TRUE;

			if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) == 0)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARServerBlock: only valid with ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED"
					);
				return CM_AR_REASON_PROT;
			}

			cm_dp_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_dp_assert (left >= CM_RSP_ARSERVER_SIZE);

			cmr_name_length = CM_GET16_NTOH (dp, CM_RSP_ARSERVER_cmr_name_length);

			cm_dp_adjust (dp, left, CM_RSP_ARSERVER_SIZE);

			/* CM_RSP_ARSERVER::cmr_name starts aligned */
			pad_length = cm_cl_calc_padding_length (cmr_name_length);

			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARServerBlock: cmr_name_len(%u) pad_len(%u)", cmr_name_length, pad_length
				);

			cm_dp_assert (block_length == sizeof(block_version) + sizeof(cmr_name_length) + cmr_name_length + pad_length);

			{
				LSA_UINT16               device_name_length;
				CM_UPPER_MEM_U8_PTR_TYPE device_name;

				if (cm_is_not_null(argr->cm.cl.mtd) && (argr->cm.cl.mtd->properties & CM_PD_MTD_PROP_NOS))
				{
					device_name_length = argr->cm.cl.dev->dev_addr.tailored_device_name_length;
					device_name = argr->cm.cl.dev->dev_addr.tailored_device_name;
				}
				else
				{
					device_name_length = argr->cm.cl.dev->dev_addr.device_name_length;
					device_name = argr->cm.cl.dev->dev_addr.device_name;
				}

				if (cmr_name_length != device_name_length)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "ARServerBlock: cmr_name_length(%u) != dev_name_length(%u)"
						, cmr_name_length, argr->cm.cl.dev->dev_addr.device_name_length
						);
					return CM_AR_REASON_PROT;
				}

				cm_dp_assert (left >= cmr_name_length);

				for (i = 0; i < device_name_length; ++i)
				{
					if (*(dp + i) != *(device_name + i))
					{
						CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "ARServerBlock: CMResponderStationName[%u](%c) != expected(%c)"
							, i, *(dp + i), *(device_name + i)
						);
						return CM_AR_REASON_PROT;
					}
				}
			}

			cm_dp_adjust (dp, left, cmr_name_length);

				/* [ Padding* a] */
			cm_dp_assert (left >= pad_length);
			cm_dp_adjust (dp, left, pad_length);
			}
			break;

		case CM_BLOCK_TYPE_ARVENDORBLOCK_RSP:
			{
			LSA_UINT16  ap_structure_identifier;
			LSA_UINT32  api;
			LSA_UINT16  data_and_padding_length;
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

			cm_dp_assert2 ((block_length % 4) == 0, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			cm_dp_assert2 (block_length + sizeof(block_type) + sizeof(block_length) <= 512/*see cm_ar_graph_connect_len()*/, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			cm_dp_assert2 (block_length >= sizeof(block_version) + sizeof(ap_structure_identifier) + sizeof(api), CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			data_and_padding_length = CM_CAST_U16 (block_length - sizeof(block_version) - sizeof(ap_structure_identifier) - sizeof(api));

			cm_dp_assert2 (block_version == 0x0100, CM_PNIO_ERR_ARVendorBlockRes(2/*BlockVersion*/));	/* only version 1.0 */

			cm_dp_assert2 (left >= CM_RSP_ARVENDOR_SIZE, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));

			ap_structure_identifier = CM_GET16_NTOH (dp, CM_RSP_ARVENDOR_APStructureIdentifier);
			api = CM_GET32_NTOH (dp, CM_RSP_ARVENDOR_API);

			cm_dp_adjust (dp, left, CM_RSP_ARVENDOR_SIZE);

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARVendorBlockRes: api(%u) ap_structure_identifier(%u) data_and_padding_length(%u)"
				, api, ap_structure_identifier, data_and_padding_length
				);

			arvendor = cm_ar_graph_find_arvendor (argr, api, ap_structure_identifier);
			cm_dp_assert2 (cm_is_not_null (arvendor), CM_PNIO_ERR_ARVendorBlockRes(4/*APStructureIdentifier*/));

			cm_dp_assert2 (left >= data_and_padding_length, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			/* ignore [Data*], [Padding*] */
			cm_dp_adjust (dp, left, data_and_padding_length);

			if (arvendor->res_data_len == 1)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARVendorBlockRes: twice");
				return CM_PNIO_ERR_ARVendorBlockRes(0/*BlockType*/);
			}
			else
			{
				arvendor->res_data_len = 1; /* see check for all ARVendorBlockRes */
			}
			}
			break;

		case CM_BLOCK_TYPE_PRMSRV_RSP:
		default:
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "IODConnectRes: unknown block_type(0x%x)", block_type);
			return CM_AR_REASON_PROT;
		}
	}

	if (! is_appl_ready)
	{
		if (! (block_ar_seen && block_iocr_seen && block_alcr_seen))
		{
			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "IODConnectRes: dev(%u): not all block_type seen, ar(%u) iocr(%u) alcr(%u)"
				, argr->device_nr
				, block_ar_seen, block_iocr_seen, block_alcr_seen
				);
			return CM_AR_REASON_PROT;
		}

		if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
		{
			if (! block_arserver_seen)
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "IODConnectRes: dev(%u): AdvancedStartup: not all block_type seen, arserver(%u)"
					, argr->device_nr
					, block_arserver_seen
					);
				return CM_AR_REASON_PROT;
			}
		}

		if (! CmListIsEmpty (&argr->arvendor_list))
		{
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

			for (CmListForeach(arvendor, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
			{
				if (arvendor->res_data_len == 0)
				{
					CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "IODConnectRes: dev(%u): ARVendorBlockRes not found for api(%u) and ap_structure_identifier(%u)"
						, argr->device_nr
						, arvendor->api, arvendor->ap_structure_identifier
						);
					return CM_PNIO_ERR_ARVendorBlockRes(0/*BlockType*/);
				}
			}
		}
	}

	return 0/*CM_AR_REASON_NONE*/;

cm_dp_assert_failed:
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "IODConnectRes: parse response failed, assert_error(0x%x) sourcecode-line(%u) current-block_type(0x%x), current-parse-offset(%u)"
		, cm_dp_assert_error, cm_dp_assert_line, block_type, (LSA_UINT32)dp - (LSA_UINT32)dp_start
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);
	return cm_dp_assert_error;
}


/*------------------------------------------------------------------------------
// bind an rpc-client-handle to the device->rpc_client_id
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_rpc_bind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_BIND_CNF);
	rpc = cm_rpc_alloc_rqb (channel);
	if (cm_is_null (rpc)) {
		return CM_ERR_ALLOC_LOWER;
	}

	rpc->args.cl.bind->client_id		= CM_RPC_INVALID_CLIENT_ID; /* out */

	if (dev->argr->ar.host_ip == 0)
	{
		rpc->args.cl.bind->host_ip = CM_HTON32 (0x7F000001); /* anything, 127.0.0.1 */
	}
	else
	{
		rpc->args.cl.bind->host_ip = dev->argr->ar.host_ip;
	}
	rpc->args.cl.bind->udp_port			= 0;

	cm_rpc_get_IPNIO_Device(
		& rpc->args.cl.bind->if_uuid,
		& rpc->args.cl.bind->if_version_major,
		& rpc->args.cl.bind->if_version_minor,
		LSA_NULL, LSA_NULL
		);

	cm_rpc_make_object_uuid (& rpc->args.cl.bind->object_uuid,
			dev->dev_rpc.vendor_id, dev->dev_rpc.device_id, dev->dev_rpc.instance
		);

	dev->bindsm.rpc = rpc;

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_BIND, dev->device_nr, 0);

	return CM_OK_PENDING;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_rpc_bind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
	)
{
	CM_ASSERT (cm_is_not_null (rpc));
	CM_ASSERT (dev->wf & CM_CL_DEV__WF_BIND_CNF);

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): wf(0x%x) rpc-rsp(%u)"
		, dev->device_nr, dev->wf, CM_RPC_GET_RESPONSE (rpc)
		);

	if (CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK)
	{
		dev->rpc_client_id = rpc->args.cl.bind->client_id;
		cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_BIND_CNF, CM_OK);
	}
	else
	{
		dev->rpc_client_id = CM_RPC_INVALID_CLIENT_ID;
		cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_BIND_CNF, CM_ERR_LOWER_LAYER);
	}
}


/*------------------------------------------------------------------------------
// rebind the device->rpc_client_id to the dev_addr
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_rebind (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc = dev->bindsm.rpc;

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) rpc_client_id(0x%x)"
		, argr->device_nr
		, dev->rpc_client_id
		);

	rpc->args.cl.bind->client_id		= dev->rpc_client_id;

	rpc->args.cl.bind->host_ip			= argr->ar.host_ip;
	rpc->args.cl.bind->udp_port			= 0;

	cm_rpc_get_IPNIO_Device(
		&rpc->args.cl.bind->if_uuid,
		&rpc->args.cl.bind->if_version_major,
		&rpc->args.cl.bind->if_version_minor,
		LSA_NULL, LSA_NULL
		);

	cm_rpc_make_object_uuid (& rpc->args.cl.bind->object_uuid
		, dev->dev_rpc.vendor_id, dev->dev_rpc.device_id, dev->dev_rpc.instance
		);

	cm_rpc_request_lower (argr->ar_com.channel, rpc, CLRPC_OPC_CL_REBIND, argr->device_nr, argr->ar_nr);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_rpc_rebind_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
	)
{
	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) rsp(%u) rpc_client_id(0x%x)"
		, argr->device_nr
		, CM_RPC_GET_RESPONSE (rpc)
		, rpc->args.cl.bind->client_id
		);

	if (CM_RPC_GET_RESPONSE (rpc) != CLRPC_OK)
	{
		/* docu: should never fail */
		CM_FATAL1(CM_RPC_GET_RESPONSE (rpc));
	}

	cm_cl_bindsm_trigger (argr, CM_CL_WF_REBIND_CNF);
}


/*------------------------------------------------------------------------------
// rpc-unbind the rpc-client-handle
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_rpc_unbind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_CONST_PTR_TYPE  dev
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) wf(0x%x) rpc_client_id(0x%x)"
		, dev->device_nr
		, dev->wf
		, dev->rpc_client_id
		);

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_UNBIND_CNF);

	CM_ASSERT (cm_is_not_null (dev->bindsm.rpc));

	rpc = dev->bindsm.rpc;
	rpc->args.cl.unbind->client_id = dev->rpc_client_id;

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_UNBIND, dev->device_nr, 0
		);

	return CM_OK_PENDING;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_rpc_unbind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	LSA_UINT16  rpc_rsp
	)
{
	LSA_UINT16  cm_rsp;

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_UNBIND_CNF);
/*
	dev->wf &= ~ CM_CL_DEV__WF_UNBIND_CNF;

	must handled in the forwarwed functions
*/

	if (rpc_rsp != CLRPC_OK) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) rsp(%u) from rpc, set to invalid_client_id"
			, dev->device_nr, rpc_rsp
			);
		cm_rsp = CM_ERR_LOWER_LAYER;
	}
	else {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) rpc_rsp(%u)"
			, dev->device_nr, rpc_rsp
			);
		cm_rsp = CM_OK;
	}

	dev->rpc_client_id = CM_RPC_INVALID_CLIENT_ID;

	cm_rpc_free_rqb (channel, dev->bindsm.rpc);
	dev->bindsm.rpc = LSA_NULL;

	cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_UNBIND_CNF, cm_rsp);
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT32
cm_ar_graph_connect_len (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	LSA_UINT16  cmi_station_name_length = 0;

	LSA_UINT32  len_req;
	LSA_UINT32  len_cnf;

	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioap;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;

	CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

	CM_AR_GRAPH_SLOT_NR_PTR_TYPE  unique;
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

	{	CM_UPPER_MEM_U8_PTR_TYPE  cmi_station_name;
		if ( ! cm_oha_get_name_of_station (channel, & cmi_station_name_length, & cmi_station_name))
		{
			return 0;
		}
	}

	/*----------------------------------------------------------------------------
	//	see cm_arcb.h#IODConnectReq
	*/

	/* --- ARBlockReq --- */
	len_req = CLRPC_PNIO_OFFSET;
	len_req += CM_REQ_AR_SIZE;
	len_req += cmi_station_name_length;

	/* --- [IOCRBlockReq*] --- */
	for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		len_req += CM_REQ_IOCR1_SIZE;

		for (CmListForeach (ioap, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE))
		{
			len_req += CM_REQ_IOCR2_SIZE;

			for (CmListForeach (iodata, & ioap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				len_req += CM_REQ_IOCR3_SIZE;
			}

			len_req += CM_REQ_IOCR4_SIZE;

			for (CmListForeach (iocs, & ioap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE))
			{
				len_req += CM_REQ_IOCR5_SIZE;
			}
		}
	}

	/* --- [AlarmCRBlockReq] --- */
	len_req += CM_REQ_ALCR_SIZE;

	/* --- [ExpectedSubmoduleBlockReq*] --- */
	/* for all unique slot_nr */
	for (CmListForeach(unique, &argr->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE))
	{
		len_req += CM_REQ_API_SIZE;

			/* search all ap's which contains the unique->slot_nr */
		for (CmListForeach(ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			mod = cm_ar_graph_find_module (ap, unique->slot_nr);
			if (cm_is_null (mod)) {
				continue;
			}
			len_req += CM_REQ_MOD_SIZE;

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				len_req += CM_REQ_SUB_SIZE;

				switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
				case CM_SUB_PROP_TYPE_OUTPUT:
					len_req += CM_REQ_DSC_SIZE;
					break;

				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					len_req += CM_REQ_DSC_SIZE;
					len_req += CM_REQ_DSC_SIZE;
					break;

				default:
					CM_FATAL ();
					break;
				}
			}
		}
	}

	/* --- [PrmServerBlock] --- // not yet */

	/* --- [MCRBlockReq*] --- */
	for (CmListForeach (mcr, & argr->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE))
	{
		len_req += CM_REQ_MCR_SIZE;
		len_req += mcr->provider_name_length;
	}

	/* --- [ARRPCBlockReq] --- // not possible */

	/* --- [IRInfoBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
		&& cm_is_not_null (argr->ir_info))
	{
		LSA_UINT16 subCount = 0;

		CmListLength (&(argr->ir_info->ir_info_sub_list), &subCount);

		len_req += CM_REQ_IRINFO_SIZE;
		len_req += (subCount * CM_REQ_IRINFOSUB_SIZE);
	}

	/* --- [SRInfoBlock] --- */
	if (cm_is_not_null (argr->sr_info))
	{
		len_req += CM_REQ_SRINFO_SIZE;
	}

	/* --- [ARVendorBlockReq*] --- */
	for (CmListForeach (arvendor, & argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		len_req += CM_REQ_ARVENDOR_SIZE;
		len_req += arvendor->req_data_len;
		len_req += cm_cl_calc_padding_length (arvendor->req_data_len);
	}

	/* --- [ARFSUBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
	{
		CM_PD_RECORD_PTR_TYPE ptr;
		for (CmListForeach(ptr, &argr->cm.cl.ar_connect_prm_record_list, CM_PD_RECORD_PTR_TYPE))
		{
			len_req += ptr->record_data_len;
		}
	}

	/* --- [RSInfoBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
		&& cm_is_not_null (argr->rs_info))
	{
		len_req += argr->rs_info->rs_info_data_len;
	}

	/*----------------------------------------------------------------------------
	//	IODConnectRes ::=
	//		ARBlockRes,
	//			{ [IOCRBlockRes*], [AlarmCRBlockRes], [ModuleDiffBlock]
	//			, [ARRPCBlockRes], ARServerBlockRes,  [ARVendorBlockRes*]
	//			}
	*/
	len_cnf = CLRPC_PNIO_OFFSET;

	/* --- ARBlockRes --- */
	len_cnf += CM_RSP_AR_SIZE;

	/* --- [IOCRBlockRes*] --- */
	for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		len_cnf += CM_RSP_IOCR_SIZE;
	}

	/* --- [AlarmCRBlockRes] --- */
	len_cnf += CM_RSP_ALCR_SIZE;

	/* --- [ModuleDiffBlock] --- */
	/* max-len .. for every submodule one entry */
	/* BlockHeader, NumberOfAPIs,
		(API, NumberOfModules,
			(SlotNumber, ModuleIdentNumber, ModuleState, NumberOfSubmodules,
				(SubslotNumber, SubmoduleIdentNumber, SubmoduleState)*)*)*

		note: cm-server assume optimum hierarchy size
	*/
	len_cnf += CM_RSP_MDB_SIZE;

	for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		len_cnf += CM_RSP_API_SIZE;

		for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			len_cnf += CM_RSP_MOD_SIZE;

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				len_cnf += CM_RSP_SUB_SIZE;
			}
		}
	}

	/* --- [ARRPCBlockRes] --- // not possible */

	/* --- [ARServerBlockRes] --- */
	if (argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED)
	{
		len_cnf += CM_RSP_ARSERVER_SIZE;
		len_cnf += 240; /* max StationNameLength */
	}

	/* --- [ARVendorBlockRes*] --- */
	/* see part2: "At least an ARVendorBlockRes size of 512 octets for each used ARVendorBlockReq at the connect request" */
	for (CmListForeach (arvendor, & argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		len_cnf += 512;
	}

	/*----------------------------------------------------------------------------*/
	if (len_req > len_cnf)
	{
		return len_req;
	}
	return len_cnf;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_connect (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  marshalled_len;
	LSA_UINT32  alloc_len;
	CM_CL_DEVICE_PTR_TYPE  dev;
	CM_CL_DOWN_INFO_TYPE  dinfo;

/* VV: use oha_channel.oha.nos.length */
	alloc_len = cm_ar_graph_connect_len (channel, argr);
	if (alloc_len >= 0xFFFF || alloc_len == 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "alloc_len(%u) >= 0xFFFF", alloc_len);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	CM_COUNTER_INCREMENT (argr->cm.cl.connect_counter);

	dev = argr->cm.cl.dev;
	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rpc_client_id(%08x) alloc_len(%u)"
		, argr->device_nr, dev->rpc_client_id, alloc_len
		);

	rpc = cm_rpc_alloc_call_rqb (channel, (LSA_UINT16)alloc_len);
	if (cm_is_null (rpc)) {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!rpc");

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* marshal to PNIO-net-Format */
	marshalled_len = cm_cl_marshal_connect (channel
		, rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET
		, (LSA_UINT16) (alloc_len - CLRPC_PNIO_OFFSET)
		, argr
		);
	CM_ASSERT (0U + CLRPC_PNIO_OFFSET + marshalled_len <= alloc_len);
	if (marshalled_len == 0)
	{
		cm_rpc_free_call_rqb (channel, rpc);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}


		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONNECT;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->ndr_len			= CLRPC_PNIO_OFFSET + marshalled_len;
/*	rpc->args.cl.call->ndr_data[0]		= done with marshal */

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_rpc_connect_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
	)
{
	LSA_UINT16  err12;
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	CLRPC_UPPER_CL_CALL_PTR_TYPE  call = rpc->args.cl.call;

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF);

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_CONNECT
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rpc-connect-done, rsp(%u) nca_status(%u) ndr_len(%u) pnio_status(0x%x)"
		, argr->device_nr
		, lower_rsp, nca_status, call->ndr_len, pnio_status
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): cm_cl_rpc_connect_done: pnio_status(0x%x)"
			, argr->device_nr, pnio_status
			);
		CM_CL_DINFO_SET2  (&dinfo, CM_AR_REASON_CONNECT, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

#ifdef CM_MESSAGE
/* VV: should return (part of) pnio_status like ModuleDiffBlock does */
#endif /* CM_MESSAGE */

	err12 = cm_cl_rpc_connect_parse_response (argr, call->ndr_data + CLRPC_PNIO_OFFSET, call->ndr_len - CLRPC_PNIO_OFFSET, LSA_FALSE);
	if (err12 != 0)
	{
		if (CM_PNIO_ERR_FETCH_CODE_1 (err12) == 0) {
			pnio_status = CM_PNIO_FAULTY_BlockType (CM_PNIO_ERR_CODE_CONNECT); /* compatible */
		}
		else {
			pnio_status = CM_PNIO_FAULTY_IODConnectRes (err12);
		}
		CM_CL_DINFO_SET  (&dinfo, CM_AR_REASON_CONNECT, CM_ERROR_CAUSE_PNIO_CONFIGURATION, pnio_status);
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* trigger next step */

	cm_cl_ar_sustat_sm_trigger(argr, CM_CL_SUSTAT_STATE_CONNECT);

	cm_cl_arsm (argr);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_appl_ready_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,

	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	CM_RPC_MEM_U8_PTR_TYPE  dp,
	LSA_INT32  left,

	LSA_UINT16  block_type,
	LSA_UINT16  control_properties,
	LSA_UINT16  alarm_sequence_number
	)
{
	LSA_UINT16  rc;

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_PRM: /* PrmEnd.cnf not seen yet */
	case CM_DST_WF_APPL_READY_IND:
		{
		LSA_UINT16  prmmode = cm_cl_prm_mode(argr);
		LSA_BOOL  ar_is_up = cm_ar_com_is_up(argr);
		LSA_BOOL  start_predata_timer = LSA_FALSE; /* 900548(RT AdvancedStartup does a needlessly PRE_INDATA.IND) / decision: also for Advanced-IRT */

		if (argr->cm.cl.applrdy_state == CM_CL_APPL_READY_STATE_UP)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "CL-ARDY.ind: dev(%u): rerun in state(%u) prmmode(%u)"
				, argr->device_nr, argr->cm.cl.state, prmmode
				);
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
			return;
		}

		if (block_type == CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ
			&&  (	prmmode == CM_CL_PRMMODE_CONNECT
				||  prmmode == CM_CL_PRMMODE_AR
				||  prmmode == CM_CL_PRMMODE_SUBMODULE
				||  prmmode == CM_CL_PRMMODE_COC))
		{
			if (argr->cm.cl.applrdy_prev_state == CM_CL_APPL_READY_STATE_UNKNOWN_PLUG)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "CL-ARDY.ind: dev(%u): rerun in state(%u) prmmode(%u) applrdy_prev_state(%u) "
					, argr->device_nr, argr->cm.cl.state, prmmode, argr->cm.cl.applrdy_prev_state
					);

				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
				return;
			}
			else
			{
				CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "CL-ARDY.ind: dev(%u): block_type(0x%x) and prmmode(%u) mismatch, state(%u) wf0(0x%x) wf1(0x%x)"
					, argr->device_nr, block_type, prmmode
					, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
					);
				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (0/*BlockType*/)));
				return;
			}
		}
		else if (block_type == CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ  && prmmode == CM_CL_PRMMODE_PLUG)
		{
			if (argr->cm.cl.applrdy_prev_state == CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "CL-ARDY.ind: dev(%u): rerun in state(%u) prmmode(%u) applrdy_prev_state(%u) "
					, argr->device_nr, argr->cm.cl.state, prmmode, argr->cm.cl.applrdy_prev_state
					);

				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
				return;
			}
			else
			{
				CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "CL-ARDY.ind: dev(%u): block_type(0x%x) and prmmode(%u) mismatch, state(%u) wf0(0x%x) wf1(0x%x)"
					, argr->device_nr, block_type, prmmode
					, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
					);
				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (0/*BlockType*/)));
				return;
			}
		}


		CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "CL-ARDY.ind: dev(%u):  state(%u) applrdy_state(%u) wf0(0x%x) wf1(0x%x) prmmode(%u) block_type(0x%x) ar_is_up(%u)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.applrdy_state
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1], prmmode, block_type, ar_is_up
			);

		switch (cm_cl_prm_mode (argr))
		{
		case CM_CL_PRMMODE_CONNECT:
				/* check this: how long must we support V2 (means SubmoduleState.FormatIndicator == 0) ? */
				/* use this function also within cm_cl_marshal_connect() */
			if (argr->cm.cl.dev->dev_type == CM_CL_DEV_TYPE_V3)
			{
				cm_ar_graph_set_ist_to_proper (argr);
			}

			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT
			||	argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3
			||	argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				if ((control_properties & CM_CONTROL_PROP_READY_FOR_RTC3) != 0)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "CL-ARDY.ind: dev(%u): ControlBlockProperties not 0", argr->device_nr);
					cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (9/*ControlBlockProperties*/)));
					return;
				}
			}

			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				if (! ar_is_up)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "CL-ARDY.ind: dev(%u):  AR not up, start PreData-Timeout", argr->device_nr
						);
					start_predata_timer = LSA_TRUE;
				}
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
				/* our CMSV has implemented norm part 6 Figure A.5 – Principle of the data evaluation during startup (Special case: Isochronous mode application)
				// this means ar_is_up is never true during ApplReady.ind */
				if (! ar_is_up)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "CL-ARDY.ind: dev(%u):  AR not up, start PreData-Timeout", argr->device_nr
						);
						start_predata_timer = LSA_TRUE;
				}
				else
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "CL-ARDY.ind: dev(%u):  AR up, no PreData.ind nesessary", argr->device_nr
						);
				}
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				if ((control_properties & CM_CONTROL_PROP_READY_FOR_RTC3) == 0)
				{
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3);
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
				}
			}
			break;

		case CM_CL_PRMMODE_PLUG:
			if (cm_is_not_null (argr->cm.cl.prm_alarm) /* may happen in case of rerun */
				&&	argr->cm.cl.prm_alarm->args.cl.alarm->alarm_sequence != alarm_sequence_number)
			{
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "CL-ARDY.ind: dev(%u):  prm.alarm_seq_nr(%u) != appl_rdy.alarm_seq_nr(%u)"
					, argr->device_nr
					, argr->cm.cl.prm_alarm->args.cl.alarm->alarm_sequence, alarm_sequence_number
					);
				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockPlug (7/*alarm_sequence*/)));
				return;
			}
			break;

		case CM_CL_PRMMODE_AR:
			cm_ar_graph_set_ist_to_proper (argr); /* 1078181 */
			break;

		case CM_CL_PRMMODE_SUBMODULE:
			{	/* 1078179, same as 1078181 but for one submodule */
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & sub
					);
				CM_ASSERT (cm_is_not_null (sub));
				sub->cl.real_sub_state = CM_SUB_STATE_GOOD;
				sub->cl.real_sub_ident = sub->soll.sub_ident;
			}
			break;

		case CM_CL_PRMMODE_COC:
			{	/* 1078179, same as 1078181 but for one COC ap */
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & sub
					);
				CM_ASSERT (cm_is_not_null (sub));
				sub->cl.real_sub_state = CM_SUB_STATE_GOOD;
				sub->cl.real_sub_ident = sub->soll.sub_ident;
			}
			break;

		default:
			CM_FATAL();
			break;
		}

		rc = cm_cl_rpc_connect_parse_response (argr, dp, (LSA_UINT32)left, LSA_TRUE);
		if (rc != 0)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "CL-ARDY.ind: dev(%u): cannot parse response, rc(%u) prmmode(%u)"
				, argr->device_nr, rc, prmmode
				);
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (rc));
			return;
		}

		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);

		cm_cl_prm_ardy_timeout_stop (argr);

		argr->cm.cl.applrdy_prev_state = argr->cm.cl.applrdy_state;
		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UP;

		if (start_predata_timer)
		{
			cm_cl_prm_ardy_timeout_start (argr, CM_CL_ARDY_TIMEOUT__PreDataInd);
		}
		}

		if (argr->cm.cl.state == CM_DST_PRM)
		{
			/* cm_cl_arsm(): too early, only keep ready state */
		}
		if (argr->cm.cl.state == CM_DST_WF_APPL_READY_IND)
		{
			cm_cl_arsm (argr);
		}
		break;

	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "CL-ARDY.ind: dev(%u): rerun in state(%u)"
			, argr->device_nr, argr->cm.cl.state
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		break;

	case CM_DST_PASSIVE:
	default:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "CL-ARDY.ind: dev(%u): wrong state(%u) wf0(0x%x) wf1(0x%x) or go passive in meantime, reject rpc"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		if (block_type == CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ) {
			cm_pnio_c_control_rsp (channel
				, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect(6/*SessionKey*/))
				);
		}
		else {
			cm_pnio_c_control_rsp (channel
				, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockPlug(6/*SessionKey*/))
				);
		}
		break;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_ready_for_rtc3_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT16  block_type
	)
{
	LSA_UNUSED_ARG (block_type);

	switch (argr->cm.cl.usr_state)
	{
	case CM_CL_AR_INDATA:
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): rerun in state(%u)"
			, argr->device_nr, argr->cm.cl.state
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		return;

	case CM_CL_AR_PASSIVE:
	case CM_CL_AR_WF_DOWN:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): go passive in meantime, reject rpc; state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/)));
		return;

	case CM_CL_AR_WF_IN:
		break;

	default:
		CM_FATAL1 (argr->cm.cl.usr_state);
		return;
	}


	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): ReadyForRTClass3: state(%u) applrdy_state(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, argr->cm.cl.state, argr->cm.cl.applrdy_state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): got ReadyForRTClass3 in state(%u): wf0(0x%x) wf1(0x%x)"
			, argr->device_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3);

		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		rpc = LSA_NULL;
	}
	else
	{
		/* in case of rerun */
		CM_ASSERT (cm_is_not_null (rpc));
	}

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_SCHEDULE: /* case: down -> re-schedule -> rtc3.ind  note: may occur because session_nr will increment later in cm_cl_rpc_connect() */
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
		if (cm_is_not_null (rpc)) /* too late, wrong session */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/)));
		}
		break;

	case CM_DST_PRM: /* case: plug.ind -> prm_write.req -> rtc3.ind & delayed PrmEnd.cnf*/
	case CM_DST_WF_APPL_READY_IND:
		if (cm_is_not_null (rpc)) /* rerun */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		}
		/* do not call cm_cl_arsm () */
		break;

	case CM_DST_WF_NEW_DATA:
		if (cm_is_not_null (rpc)) /* rerun */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		}
		/* trigger */
		cm_cl_arsm (argr);
		break;

	default:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u): state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_FATAL1 (argr->cm.cl.state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_rpc_prepare_control_rsp (
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc,
	LSA_UINT16  block_type
	)
{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;

		/* set rpc.control.rsp to done */
	dp = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.sv.call->ndr_len - CLRPC_PNIO_OFFSET;

	switch (block_type) {
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
		CM_PUT16_HTON(dp, CM_BLK_HEADER_type, CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP);
		break;
	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		CM_PUT16_HTON(dp, CM_BLK_HEADER_type, CM_BLOCK_TYPE_IOXBLOCK_PLUG_RSP);
		break;
	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		CM_PUT16_HTON(dp, CM_BLK_HEADER_type, CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_RSP);
		break;
	default:
		CM_ASSERT (0); /* checked previously */
		return;
	}

		/* marshal answer */
	CM_PUT16_HTON (dp, CM_BLK_CTRL_control_command, CM_CONTROL_COMMAND_DONE);
	cm_dp_adjust (dp, left, CM_BLK_CTRL_SIZE);
	LSA_UNUSED_ARG (left); /* lint */

	rpc->args.sv.call->ndr_len = (LSA_UINT32)(dp - rpc->args.sv.call->ndr_data);
}


/*------------------------------------------------------------------------------
// handle incoming rpc_call::pnio_c_control
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pnio_c_control_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  cm_dp_assert_line = 0;
	LSA_UINT16  cm_dp_assert_error = 0;
	LSA_UINT16  device_nr = 0;
	LSA_UINT16  ar_nr = 0;

	LSA_UINT16  block_type, block_length, block_version;
	CLRPC_UUID_TYPE  ar_uuid;
	CM_AR_GRAPH_PTR_TYPE  argr = LSA_NULL;
	LSA_UINT16  session_key, alarm_sequence_number, control_command, control_properties;

	dp = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.sv.call->ndr_len - CLRPC_PNIO_OFFSET;

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "IOXControl.ind: rpc-rqb(0x%x) opnum(%u) ndr_len(%u)"
		, rpc, rpc->args.sv.call->opnum, rpc->args.sv.call->ndr_len
		);

		/* parse block-header */
	block_type = 0;
	cm_dp_assert2 (left >= CM_BLK_HEADER_SIZE, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));

	block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
	block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
	block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

	switch (block_type)
	{
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		cm_dp_assert2 (block_length == 28, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		cm_dp_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ControlBlockConnect (2/*BlockVersionHigh*/));
		cm_dp_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ControlBlockConnect (3/*BlockVersionLow*/));

			/* parse control-block */
		cm_dp_assert2 (left >= CM_BLK_CTRL_SIZE, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		cm_dp_assert2 (block_length == 28, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		cm_dp_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ControlBlockPlug (2/*BlockVersionHigh*/));
		cm_dp_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ControlBlockPlug (3/*BlockVersionLow*/));

			/* parse control-block */
		cm_dp_assert2 (left >= CM_BLK_CTRL_SIZE, CM_PNIO_ERR_ControlBlockPlug (1/*BlockLength*/));
		break;

	default:
		cm_dp_goto_failed (CM_PNIO_ERR_ControlBlockConnect (0/*BlockType*/)); /* BlockConnect */
	}

	/* left is checked above */

	CM_GET_UUID_NTOH (& ar_uuid, dp, CM_BLK_CTRL_ar_uuid);
	session_key				= CM_GET16_NTOH (dp, CM_BLK_CTRL_session_key);
	alarm_sequence_number	= CM_GET16_NTOH (dp, CM_BLK_CTRL_reserved2_alarm_sequence_number);
	control_command			= CM_GET16_NTOH (dp, CM_BLK_CTRL_control_command);
	control_properties		= CM_GET16_NTOH (dp, CM_BLK_CTRL_control_properties);

	CM_PUT16_HTON (dp, CM_BLK_CTRL_control_command, 0);	/* !!! set response */

	cm_dp_adjust (dp, left, CM_BLK_CTRL_SIZE);

	argr = cm_cl_ar_from_ar_uuid (channel, & ar_uuid, LSA_TRUE);
	if (cm_is_not_null (argr))
	{
		device_nr = argr->device_nr;
		ar_nr = argr->ar_nr;
	}

	CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "IOXControl.ind: ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
		, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
		, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
		, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
		);
	CM_CL_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "IOXControl.ind: dev(%u) ar(%u) session_key(%u) block_type(0x%x) ctrl_command(0x%x) ctrl_prop(0x%x) alarm_seq_nr(%u)"
		, device_nr, ar_nr, session_key, block_type, control_command, control_properties, alarm_sequence_number
		);

	switch (block_type)
	{
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
		{
		cm_dp_assert2 (cm_is_not_null (argr), CM_PNIO_ERR_ControlBlockConnect (5/*ARUUID*/));
		cm_dp_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/));
		cm_dp_assert2 (control_command == CM_CONTROL_COMMAND_APPLICATION_READY, CM_PNIO_ERR_ControlBlockConnect (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_appl_ready_ind (channel, argr
			, rpc, dp, CM_CAST_INT32(left)
			, block_type, control_properties, alarm_sequence_number
			);
		}
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		{
		cm_dp_assert2 (cm_is_not_null (argr), CM_PNIO_ERR_ControlBlockPlug (5/*ARUUID*/));
		cm_dp_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockPlug (6/*SessionKey*/));
		cm_dp_assert2 (control_command == CM_CONTROL_COMMAND_APPLICATION_READY, CM_PNIO_ERR_ControlBlockPlug (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_appl_ready_ind (channel, argr
			, rpc, dp, CM_CAST_INT32(left)
			, block_type, control_properties, alarm_sequence_number
			);
		}
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		{
		cm_dp_assert2 (cm_is_not_null (argr), CM_PNIO_ERR_ControlBlockConnect (5/*ARUUID*/));
		cm_dp_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/));
		cm_dp_assert2 (control_command == CM_CONTROL_COMMAND_READY_FOR_RTC3, CM_PNIO_ERR_ControlBlockConnect (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_ready_for_rtc3_ind (channel, argr, rpc, block_type);
		}
		break;

	default:
		CM_ASSERT (0); /* checked previously */
		return;
	}

	return;

cm_dp_assert_failed:
	{
	LSA_UINT32  pnio_err = CM_PNIO_FAULTY_IOXControlRes (cm_dp_assert_error);
	CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "IOXControl.ind: dev(%u) ar(%u)  assertion in line(%u): block_type(0x%x) pnio_err(0x%x) left(%u)"
		, device_nr, ar_nr, cm_dp_assert_line, block_type, pnio_err, left
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);
	cm_pnio_c_control_rsp (channel, argr, rpc, pnio_err);
	}
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pnio_c_control_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT32  pnio_code
	)
{
	LSA_UINT32  trace_idx = cm_is_not_null (argr) ? argr->trace_idx : 0;

	CM_ASSERT (pnio_code == CM_PNIO_ERR_NONE || (pnio_code & 0xFFFFFF00) != 0);

		/* ok response */
	if (pnio_code == CM_PNIO_ERR_NONE) {
		rpc->args.sv.call->pnio_status = CM_PNIO_ERR_NONE;
		CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
	}

		/* error response */
	else {
		rpc->args.sv.call->pnio_status = pnio_code;

		LSA_UNUSED_ARG (trace_idx);
		CM_CL_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_UNEXP, "pnio_status(0x%x)", rpc->args.sv.call->pnio_status);

		rpc->args.sv.call->ndr_len = CLRPC_PNIO_OFFSET;
		CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0);
	}
}


/*------------------------------------------------------------------------------
// release an AR
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_release (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  ndr_len;
	LSA_UINT16  cm_dp_assert_line = 0;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	ndr_len = CLRPC_PNIO_OFFSET + CM_BLK_REL_SIZE;

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u): cm_cl_rpc_release, rpc_client_id(%08x) opnum(%u) ndr_len(%u)"
		, argr->device_nr
		, dev->rpc_client_id
		, CLRPC_PNIO_OPNUM_RELEASE
		, ndr_len
		);

	rpc = cm_rpc_alloc_call_rqb (channel, ndr_len);
	if (cm_is_null (rpc))
	{
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* marshal to PNIO-Net-Format */
	{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  release_command, release_properties;

	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = ndr_len - CLRPC_PNIO_OFFSET;

	cm_dp_assert (left >= CM_BLK_REL_SIZE);

	CM_PUT16_HTON(dp, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_RELBLOCK_REQ);
	CM_PUT16_HTON(dp, CM_BLK_HEADER_length,  CM_BLK_REL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	CM_PUT16_HTON(dp, CM_BLK_HEADER_version, 0x0100); /* fixed V1.0 because not in CM_ARCB_PDU_VERSION_TYPE */

	release_command = CM_CONTROL_COMMAND_RELEASE;
	release_properties = 0;

	CM_PUT16_HTON (dp, CM_BLK_REL_reserved1,			0);
	CM_PUT_UUID_HTON (dp, CM_BLK_REL_ar_uuid,			& argr->ar.ar_uuid);
	CM_PUT16_HTON (dp, CM_BLK_REL_session_key,			argr->cm.cl.connect_counter);
	CM_PUT16_HTON (dp, CM_BLK_REL_reserved2_alarm_sequence_number, 0);
	CM_PUT16_HTON (dp, CM_BLK_REL_control_command,		release_command);
	CM_PUT16_HTON (dp, CM_BLK_REL_control_properties,	release_properties);

	cm_dp_adjust (dp, left, CM_BLK_REL_SIZE);
	CM_ASSERT (left == 0);
	LSA_UNUSED_ARG (dp); /* lint */
	LSA_UNUSED_ARG (left); /* lint */
	}

		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_RELEASE;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->ndr_len			= ndr_len;
/*	rpc->args.cl.call->ndr_data[0]		= see marshal */

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
	return;

cm_dp_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	cm_rpc_free_call_rqb (channel, rpc);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_rpc_release_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
)
{
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF);

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_RELEASE
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): cm_cl_rpc_release_done, pnio_status(0x%x)"
			, argr->device_nr, pnio_status
			);
	}
	else
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): cm_cl_rpc_release_done."
			, argr->device_nr
			);
	}

	/* already down, see ar_passivate() */
	cm_cl_arsm (argr);
}


/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_BOOL  plug,
	LSA_UINT16  alarm_seq_nr
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  ndr_len;
	LSA_UINT16  cm_dp_assert_line = 0;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UINT16  block_type;
	LSA_UINT16  control_cmd = CM_CONTROL_COMMAND_PRM_END;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	ndr_len = CLRPC_PNIO_OFFSET + CM_BLK_CTRL_SIZE;

	rpc = cm_rpc_alloc_call_rqb (channel, ndr_len);
	if (cm_is_null (rpc))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): no mem, do down, plug(%u) alarm_seq_nr(%u)"
			, argr->device_nr, plug, alarm_seq_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONTROL;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->ndr_len			= ndr_len;
	rpc->args.cl.call->ndr_data[0]		= 0;

		/* ? two functions */
	if (plug) {
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) PrmEnd(Plug).req rpc_client_id(%08x) opnum(%u) control_cmd(%u) ndr_len(%u)"
			, argr->device_nr
			, dev->rpc_client_id, CLRPC_PNIO_OPNUM_CONTROL, control_cmd, ndr_len
			);

		block_type = CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ;
	}
	else {
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) PrmEnd(Connect).req rpc_client_id(%08x) opnum(%u) control_cmd(%u) ndr_len(%u)"
			, argr->device_nr
			, dev->rpc_client_id, CLRPC_PNIO_OPNUM_CONTROL, control_cmd, ndr_len
			);

		block_type = CM_BLOCK_TYPE_IODBLOCK_CONN_REQ;
		alarm_seq_nr = 0;
	}

		/* marshal */
	{
		CM_RPC_MEM_U8_PTR_TYPE  dp;
		LSA_UINT32  left;

		dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		left = ndr_len - CLRPC_PNIO_OFFSET;

		cm_dp_assert (left >= CM_BLK_CTRL_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,    block_type);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,  CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version, 0x0100);

		CM_PUT16_HTON (dp, CM_BLK_CTRL_reserved1, 0);
		CM_PUT_UUID_HTON (dp, CM_BLK_CTRL_ar_uuid, & argr->ar.ar_uuid);
		CM_PUT16_HTON (dp, CM_BLK_CTRL_session_key, argr->cm.cl.connect_counter);
		CM_PUT16_HTON (dp, CM_BLK_CTRL_reserved2_alarm_sequence_number, alarm_seq_nr);
		CM_PUT16_HTON (dp, CM_BLK_CTRL_control_command, control_cmd);
		CM_PUT16_HTON (dp, CM_BLK_CTRL_control_properties, 0);	/* fixed data */

		cm_dp_adjust (dp, left, CM_BLK_CTRL_SIZE);
		CM_ASSERT (left == 0);
		LSA_UNUSED_ARG (dp); /* lint */
		LSA_UNUSED_ARG (left); /* lint */
	}

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
	return;

cm_dp_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	cm_rpc_free_call_rqb (channel, rpc);
	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_rpc_control_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE  rpc
	)
{
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	LSA_UINT16  cm_dp_assert_line = 0;
	LSA_UINT16  cm_dp_assert_error = 0;
	LSA_BOOL  is_prm_begin = LSA_FALSE;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UINT16  block_type, block_length, block_version;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	CLRPC_UUID_TYPE  ar_uuid;
	LSA_UINT16  session_key, control_cmd, control_properties;

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_CONTROL
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rsp(%u) pnio_status(0x%x) nca_status(0x%x) ndr_len(%u)"
		, argr->device_nr
		, lower_rsp, pnio_status, nca_status, rpc->args.cl.call->ndr_len
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): cm_cl_rpc_control_done: pnio_status(0x%x) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, pnio_status
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF); /* one of this two is set .. */
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

		CM_CL_DINFO_SET2  (&dinfo, CM_AR_REASON_CONTROL, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);

		cm_cl_prm_trigger (argr, 0);
		return;
	}


	/* parse answer */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	cm_dp_assert (left >= CM_BLK_HEADER_SIZE);

	block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
	block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
	block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

	cm_dp_assert (block_length == CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	cm_dp_assert (block_version == 0x0100); /* only version 1.0 */

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF);

		cm_cl_ar_sustat_sm_trigger(argr, CM_CL_SUSTAT_STATE_PRM);

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_CONNECT:
			cm_dp_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_CONN_RSP);
			break;

		case CM_CL_PRMMODE_PLUG:
			cm_dp_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP);
			break;

		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:
			cm_dp_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_CONN_RSP);
			break;

		default:
			cm_dp_goto_failed (0/*pgm error*/);
		}
	}
	else if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

		is_prm_begin = LSA_TRUE;

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:
			cm_dp_assert (block_type == CM_BLOCK_TYPE_PRM_BEGIN_RSP);
			break;

		case CM_CL_PRMMODE_PLUG:
		case CM_CL_PRMMODE_CONNECT:
		default:
			cm_dp_goto_failed (0/*pgm error*/);
		}
	}
	else
	{
		cm_dp_goto_failed (0/*pgm error*/);
	}


	cm_dp_assert (left >= CM_BLK_CTRL_SIZE);

	CM_GET_UUID_NTOH (& ar_uuid, dp, CM_BLK_CTRL_ar_uuid);
	session_key			= CM_GET16_NTOH (dp, CM_BLK_CTRL_session_key);
	control_cmd			= CM_GET16_NTOH (dp, CM_BLK_CTRL_control_command);
	control_properties	= CM_GET16_NTOH (dp, CM_BLK_CTRL_control_properties);

	cm_dp_adjust (dp, left, CM_BLK_CTRL_SIZE);

	cm_dp_assert (cm_uuid_equal (& ar_uuid, & argr->ar.ar_uuid));
	cm_dp_assert (session_key == argr->cm.cl.connect_counter);
	cm_dp_assert (control_cmd == CM_CONTROL_COMMAND_DONE);
	LSA_UNUSED_ARG (control_properties); /* shall set and not be tested */

	CM_CL_TRACE_12 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "control_cmd(%x) ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, control_cmd
		, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
		, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
		, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
		, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
		);

	if (is_prm_begin)
	{
		LSA_UINT16  rc = 1;

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:
			/*optional ModuleDiffBlock*/
			rc = cm_cl_rpc_connect_parse_response (argr, dp, left, LSA_TRUE/*only ModuleDiffBlock*/);
			if (rc != 0)
			{
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u): PrmBegin.Rsp: cannot parse response ModuleDiffBlock, rc(%u) prmmode(%u)"
					, argr->device_nr, rc, cm_cl_prm_mode(argr)
					);
				cm_dp_assert (rc == 0);
			}
			/* update prm-record list */
			cm_cl_prm_record_list_update (argr);
			break;

		case CM_CL_PRMMODE_PLUG:
		case CM_CL_PRMMODE_CONNECT:
		default:
			cm_dp_goto_failed (0/*pgm error*/);
		}
	}

		/* trigger state-machine */
	cm_cl_prm_trigger (argr, 0/*trigger*/);

	return;

cm_dp_assert_failed:

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u), assertion failed in line(%u), ar goes down"
		, argr->device_nr
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_error);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	CM_CL_DINFO_SET_lnr (&dinfo, cm_dp_assert_line);
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*------------------------------------------------------------------------------
// cancel any call
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_cancel (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	rpc = cm_rpc_alloc_rqb (channel);
	if (cm_is_null (rpc))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): no rpc-rqb-mem for rpc-cancel, ignore this, must timeout calls"
			, argr->device_nr
			);
		return;
	}

	rpc->args.cl.cancel->client_id = argr->cm.cl.dev->rpc_client_id;

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CANCEL_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CANCEL, argr->device_nr, argr->ar_nr);
}

/*----------------------------------------------------------------------------*/

static  LSA_VOID
cm_cl_rpc_cancel_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	cm_rpc_free_rqb (argr->ar_com.channel, rpc);

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_CANCEL_CNF);
	cm_cl_arsm (argr);
}





/*------------------------------------------------------------------------------
// client rpc lower done
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_lower_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
)
{
	CM_CL_DEVICE_PTR_TYPE  dev;
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16  device_nr;

	if (CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK || CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK_CANCELLED) {
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rqb(0x%08x) rpc-opc(%u) rsp(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			);
	}
	else {
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rqb(0x%08x) rpc-opc(%u) rsp(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			);
	}

	switch (CM_RPC_GET_OPCODE (rpc))
	{
	case CLRPC_OPC_SV_REGISTER_IF_OBJ:
		cm_cl_rpc_register_if_obj_done (channel, rpc);
		break;

	case CLRPC_OPC_SV_UNREGISTER_IF_OBJ:
		cm_cl_rpc_unregister_done (channel, rpc);
		break;

	case CLRPC_OPC_SV_CALL_IND_PROVIDE:
		CM_ASSERT (CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK_CANCELLED); /* what else? */
		CM_ASSERT (channel->state == CM_CL_UNREGISTER);
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "call-ind-res cancelled, freeing"
			);
		cm_rpc_free_rqb(channel, rpc);
		break;

	case CLRPC_OPC_SV_CALL_INDICATION:
		CM_ASSERT (CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK);

		/* sanity-check the call-semantics */

		if( rpc->args.sv.call->semantics != CLRPC_SEMANTICS_IDEMPOTENT ) {
			/* caller uses wrong IDL file? */
			if( rpc->args.sv.call->semantics != CLRPC_SEMANTICS_AT_MOST_ONCE ) {
				CM_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "rpc-call.ind: opcum(%u) semantics(%u) not idempotent"
					, rpc->args.sv.call->opnum
					, rpc->args.sv.call->semantics
					);
				CM_RPC_SET_RESPONSE (rpc, CLRPC_ERR_FAULTED); /* not CLRPC_ERR_REJECTED, MS-RPC will retry endlessly! */
				cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
				break;
			}
			else {
				CM_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "rpc-call.ind: semantics(%u) not idempotent, at-most-once is ok too"
					, rpc->args.sv.call->semantics
					);
			}
		}

			/* .ind from client to server calls */
		switch (rpc->args.sv.call->opnum) {
		case CLRPC_PNIO_OPNUM_CONTROL:
			/* RTF or MPT running: AR already down or gone, call is late (similar to device-remove) */
			cm_pnio_c_control_ind (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
			/* RTF or MPT running: read does not interfere with RTF or MPT */
			cm_pnio_c_read_implicit_ind (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_CONNECT:
		case CLRPC_PNIO_OPNUM_RELEASE:
		case CLRPC_PNIO_OPNUM_READ:
		case CLRPC_PNIO_OPNUM_WRITE:
		default:
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "opnum(%u) not handled, rejecting", rpc->args.sv.call->opnum);
			CM_RPC_SET_RESPONSE (rpc, CLRPC_ERR_FAULTED); /* not CLRPC_ERR_REJECTED, MS-RPC will retry endlessly! */
			cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
			break;
		}
		break;

	case CLRPC_OPC_CL_BIND:
		device_nr = CM_RPC_GET_USER_ID_DEV (rpc);

		dev = cm_cl_dev_from_device_nr (channel, device_nr);
		if (cm_is_null (dev))
		{
			CM_FATAL ();
			break;
		}

		cm_cl_rpc_bind_done (channel, dev, rpc);
		break;

	case CLRPC_OPC_CL_REBIND:
		device_nr = CM_RPC_GET_USER_ID_DEV (rpc);

		argr = cm_cl_ar_from_device (channel, device_nr);

		if (cm_is_null (argr) || argr->ar_nr != CM_RPC_GET_USER_ID_AR (rpc))
		{
			CM_FATAL (); /* should not happen */
			break;
		}

		cm_cl_rpc_rebind_done (argr, rpc);
		break;

	case CLRPC_OPC_CL_UNBIND:
		device_nr = CM_RPC_GET_USER_ID_DEV (rpc);

		dev = cm_cl_dev_from_device_nr (channel, device_nr);
		if (cm_is_null (dev))
		{
			CM_FATAL ();
			break;
		}

		cm_cl_rpc_unbind_done (channel, dev, CM_RPC_GET_RESPONSE (rpc));
		break;

	case CLRPC_OPC_CL_CALL:
		device_nr = CM_RPC_GET_USER_ID_DEV (rpc);

		argr = cm_cl_ar_from_device (channel, device_nr);

		if (cm_is_null (argr) || argr->ar_nr != CM_RPC_GET_USER_ID_AR (rpc))
		{
			CM_FATAL (); /* should not happen */
			break;
		}

		/* .cnf from server to client calls, see .IDL of PNIO */
		switch (rpc->args.cl.call->opnum)
		{
		case CLRPC_PNIO_OPNUM_CONNECT:
			cm_cl_rpc_connect_done (argr, rpc);
			cm_rpc_free_call_rqb (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_RELEASE:
			cm_cl_rpc_release_done (argr, rpc);
			cm_rpc_free_call_rqb (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_READ:
		case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
			cm_cl_rpc_read_done (argr, rpc);
			break;

		case CLRPC_PNIO_OPNUM_WRITE:
			cm_cl_rpc_write_done (channel, argr, rpc);
			break;

		case CLRPC_PNIO_OPNUM_CONTROL:
			cm_cl_rpc_control_done (argr, rpc);
			cm_rpc_free_call_rqb (channel, rpc);
			break;

		default:
			CM_FATAL (); /* a bug in CLRPC */
			break;
		}
		break;

	case CLRPC_OPC_CL_CANCEL:
		argr = cm_cl_ar_from_device (channel,
				CM_RPC_GET_USER_ID_DEV (rpc)
			);

		if (cm_is_null (argr) || argr->ar_nr != CM_RPC_GET_USER_ID_AR (rpc))
		{
			CM_FATAL (); /* should not happen */
			break;
		}

		cm_cl_rpc_cancel_done (argr, rpc);
		break;

	default:
		CM_FATAL (); /* should not happen */
		break;
	}
}


/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
