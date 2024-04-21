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
/*  F i l e               &F: cm_acp1.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  alloc / free acp-RQBs                                                    */
/*  channel open                                                             */
/*  channel info                                                             */
/*  channel close                                                            */
/*  acp lower-done callback function                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20
#define CM_MODULE_ID       20

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*-----------------------------------------------------------------------------
//	work: alloc lower ACP-RQB that fits for all requests
//---------------------------------------------------------------------------*/
CM_ACP_LOWER_RQB_PTR_TYPE
cm_acp_alloc_rqb  (
	CM_CHANNEL_CONST_PTR_TYPE channel
) {
	LSA_UINT16 rc;
	CM_ACP_LOWER_MEM_PTR_TYPE lower_mem;
	CM_ACP_LOWER_RQB_PTR_TYPE lower_rqb_ptr;

	CM_ASSERT (cm_is_not_null(channel));

		/* alloc the RQB */
	CM_ACP_ALLOC_LOWER_RQB (&lower_rqb_ptr, cm_null_user_id, sizeof(CM_ACP_LOWER_RQB_TYPE), channel->sysptr);

	if (cm_is_null (lower_rqb_ptr)) {
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"h(%u) cannot alloc ACP RQB", channel->my_handle
			);
		return LSA_NULL;
	}

#if CM_DEBUG
	CM_ACP_SET_NEXT_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
	CM_ACP_SET_PREV_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
#endif

	/* alloc the buffer and attach it to the RQB */
	CM_ACP_ALLOC_LOWER_MEM (& lower_mem,
			cm_null_user_id, sizeof (ACP_ARGS_TYPE), channel->sysptr
		);
	if (cm_is_null (lower_mem)) {
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"h(%u) cannot alloc ACP MEM", channel->my_handle
			);
		CM_ACP_FREE_LOWER_RQB  (& rc, lower_rqb_ptr, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK); /* don't care in release version */
		return LSA_NULL;
	}

	CM_ACP_SET_ARGS (lower_rqb_ptr, (ACP_UPPER_ARGS_PTR_TYPE)lower_mem);

	CM_ACP_SET_OPCODE (lower_rqb_ptr, 0); /* sanity */

	return lower_rqb_ptr;
}


/*-----------------------------------------------------------------------------
//	work: free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID
cm_acp_free_rqb (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE lower_rqb_ptr
) {
	LSA_UINT16	rc;

	CM_ASSERT (cm_is_not_null(channel));
	CM_ASSERT (cm_is_not_null(lower_rqb_ptr));

	CM_ACP_FREE_LOWER_MEM (& rc, CM_ACP_GET_ARGS (lower_rqb_ptr), channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	CM_ACP_FREE_LOWER_RQB (& rc, lower_rqb_ptr, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);
}


/*-----------------------------------------------------------------------------
//	work: open the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_acp_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_CLIENT
		|| channel->path_type == CM_PATH_TYPE_SERVER
		);

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == ACP_INVALID_HANDLE);

	acp = cm_acp_alloc_rqb (channel);
	if (cm_is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, ACP_OPC_OPEN_CHANNEL);
	CM_ACP_SET_USER_ID_PTR (acp, LSA_NULL);

	acp->args->channel.open.handle = ACP_INVALID_HANDLE; /* sanity */
	acp->args->channel.open.handle_upper = channel->my_handle;
	acp->args->channel.open.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	acp->args->channel.open.acp_request_upper_done_ptr = cm_acp_request_lower_done;

	CM_ACP_OPEN_CHANNEL_LOWER (acp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_acp_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		channel->lower.handle = acp->args->channel.open.handle;
		CM_ASSERT (channel->lower.handle != ACP_INVALID_HANDLE);

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK) handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		/***/

		cm_channel_done (channel, CM_OK);

		/***/

		CM_ASSERT(channel->state == CM_CH_INFO);

		cm_acp_free_rqb (channel, acp);
	}
	else {
		CM_ASSERT (channel->lower.handle == ACP_INVALID_HANDLE); /* sanity */

		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)", channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}

/*-----------------------------------------------------------------------------
//	work: get the channel info
//---------------------------------------------------------------------------*/
LSA_VOID
cm_acp_channel_info (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->state == CM_CH_INFO);

	acp = cm_acp_alloc_rqb (channel);
	if (cm_is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	cm_acp_request_lower (channel, ACP_OPC_CHANNEL_INFO, channel, acp);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_acp_channel_info_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_ASSERT (channel->state == CM_CH_INFO);

	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK)", channel->my_handle
			);

		if( acp->args->channel.info.eth_interface_id != edd_params->InterfaceID ) {
			CM_FATAL (); /* wrong configuration */
		}

		channel->lower.info.acp.alarm_ref_base = acp->args->channel.info.alarm_ref_base;
		channel->lower.info.acp.alarm_nr_of_ref = acp->args->channel.info.alarm_nr_of_ref;

		cm_acp_free_rqb (channel, acp);

		/***/

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: move ranges to a function, call it from the channel machine")*/
#endif /* CM_MESSAGE */

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_ALARM_REF],
			channel->lower.info.acp.alarm_ref_base,
			channel->lower.info.acp.alarm_nr_of_ref
			)
		) {
			cm_channel_done (channel, CM_ERR_ALLOC_LOCAL);
			return;
		}

		/***/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_SERVERS
		{
		LSA_UINT16 max_al_hi;
		LSA_UINT16 max_al_lo;
		LSA_UINT16 i;

		switch( channel->path_type ) {
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			max_al_hi = channel->detailptr->u.cl.acp.alarm_high_prio_ind_resource_count;
			max_al_lo = channel->detailptr->u.cl.acp.alarm_low_prio_ind_resource_count;
			break;
#endif

#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			max_al_hi = channel->detailptr->u.sv.acp.alarm_high_prio_ind_resource_count;
			max_al_lo = channel->detailptr->u.sv.acp.alarm_low_prio_ind_resource_count;
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			return; /* unreachable */
		}

#if ACP_ALARM_PRIORITY_LOW != CM_ALARM_PRIORITY_LOW || ACP_ALARM_PRIORITY_HIGH != CM_ALARM_PRIORITY_HIGH
#error "check this!"
#endif

		for( i = 0; i < max_al_hi; ++i ) {
			CM_ACP_LOWER_RQB_PTR_TYPE acp_alarm_ind;
			CM_LOCAL_MEM_PTR_TYPE local_mem;

			acp_alarm_ind = cm_acp_alloc_rqb(channel);
			if (cm_is_null(acp_alarm_ind)) {
				CM_FATAL();
			}

			CM_ACP_ALLOC_ALARM_MEM (&local_mem,
					channel->acp.max_alarm_data_length, channel->sysptr
				);
			if (cm_is_null (local_mem)) {
				CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "h(%u) cannot alloc ACP ALARM MEM(%u)"
					, channel->my_handle, channel->acp.max_alarm_data_length
					);
				CM_FATAL ();
			}

			acp_alarm_ind->args->alarm.data.user_max_length = channel->acp.max_alarm_data_length;
			acp_alarm_ind->args->alarm.data.user_data = (ACP_UPPER_MEM_U8_PTR_TYPE)local_mem;
			acp_alarm_ind->args->alarm.data.priority = ACP_ALARM_PRIORITY_HIGH;

			cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp_alarm_ind);
		}

		for( i = 0; i < max_al_lo; ++i ) {
			CM_ACP_LOWER_RQB_PTR_TYPE acp_alarm_ind;
			CM_LOCAL_MEM_PTR_TYPE local_mem;

			acp_alarm_ind = cm_acp_alloc_rqb(channel);
			if (cm_is_null(acp_alarm_ind)) {
				CM_FATAL();
			}

			CM_ACP_ALLOC_ALARM_MEM (&local_mem,
					channel->acp.max_alarm_data_length, channel->sysptr
				);
			if (cm_is_null (local_mem)) {
				CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "h(%u) cannot alloc ACP ALARM MEM(%u)"
					, channel->my_handle, channel->acp.max_alarm_data_length
					);
				CM_FATAL ();
			}

			acp_alarm_ind->args->alarm.data.user_max_length = channel->acp.max_alarm_data_length;
			acp_alarm_ind->args->alarm.data.user_data = (ACP_UPPER_MEM_U8_PTR_TYPE)local_mem;
			acp_alarm_ind->args->alarm.data.priority = ACP_ALARM_PRIORITY_LOW;

			cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp_alarm_ind);
		}
		}
#endif

		/***/

		cm_channel_done (channel, CM_OK);
	}
	else {
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)"
			, channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}


/*------------------------------------------------------------------------------
// work: common request
//----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_request_lower(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, opcode);
	CM_ACP_SET_USER_ID_PTR (acp, user_id);

	CM_ACP_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> acp-rqb(0x%08x) acp-opc(%u) f-h(%u)"
		, acp, CM_ACP_GET_OPCODE (acp), channel->my_handle
		);

	CM_ACP_REQUEST_LOWER (acp, channel->sysptr);
}


/*-----------------------------------------------------------------------------
//	work: close the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_acp_channel_close (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_CLIENT
		|| channel->path_type == CM_PATH_TYPE_SERVER
		);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	acp = cm_acp_alloc_rqb (channel);
	if (cm_is_null (acp)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_ACP_SET_HANDLE (acp, channel->lower.handle);
	CM_ACP_SET_OPCODE (acp, ACP_OPC_CLOSE_CHANNEL);
	CM_ACP_SET_USER_ID_PTR (acp, LSA_NULL);

	CM_ACP_CLOSE_CHANNEL_LOWER (acp, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_acp_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	if (CM_ACP_GET_RESPONSE (acp) == ACP_OK) {

		channel->lower.handle = ACP_INVALID_HANDLE;

		CM_ACP_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) rsp(OK)", channel->my_handle
			);
		cm_acp_free_rqb (channel, acp);
		cm_channel_done (channel, CM_OK);
	}
	else {
		/* don't invalidate acp-handle, channel is not closed! */
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) rsp(%u)"
			, channel->my_handle, CM_ACP_GET_RESPONSE (acp)
			);
		cm_acp_free_rqb (channel, acp);
		cm_channel_done (channel, CM_ERR_LOWER_LAYER);
	}
}

/*===========================================================================*/
/*=== ACP LOWER DONE ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_request_lower_done(
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_CHANNEL_PTR_TYPE  channel;

	if( cm_is_null(acp) )
	{
		CM_ACP_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "cm_is_null(acp)"
			);
		CM_FATAL ();
		return;
	}

	channel = cm_channel_from_handle(CM_ACP_GET_HANDLE(acp));
	if( cm_is_null(channel) )
	{
		CM_ACP_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "invalid handle(%u)", CM_ACP_GET_HANDLE(acp)
			);
		CM_FATAL ();
		return;
	}

	switch (CM_ACP_GET_RESPONSE(acp)) {
	case ACP_OK:
	case ACP_OK_CANCEL:
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;

	case ACP_ERR_ASRT: /* AP00300519: warn only */
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;

	default:
		CM_ACP_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "<<< acp-rqb(0x%08x) acp-opc(%u) acp-rsp(%u) t-h(%u)"
			, acp, CM_ACP_GET_OPCODE(acp), CM_ACP_GET_RESPONSE(acp)
			, channel->my_handle
			);
		break;
	}

	switch (CM_ACP_GET_OPCODE (acp)) {
	case ACP_OPC_OPEN_CHANNEL:
		cm_acp_channel_open_done (channel, acp);
		return;

	case ACP_OPC_CLOSE_CHANNEL:
		cm_acp_channel_close_done (channel, acp);
		return;

	case ACP_OPC_CHANNEL_INFO:
		cm_acp_channel_info_done (channel, acp);
		return;

	case ACP_OPC_ALARM_OPEN:
	case ACP_OPC_UDP_ALARM_OPEN:
		cm_acp_alarm_add_done(channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	case ACP_OPC_ALARM_IND_RES_PROVIDE:
		if(
			channel->state != CM_CH_CLOSE
			|| CM_ACP_GET_RESPONSE(acp) != ACP_OK_CANCEL
		) {
			CM_FATAL(); /* a bug */
		}
		else {

			if( cm_is_null(acp->args->alarm.data.user_data) ) {
				CM_FATAL(); /* a bug */
			}
			else {
				LSA_UINT16 rc;
				CM_ACP_FREE_ALARM_MEM(&rc, acp->args->alarm.data.user_data, channel->sysptr);
				CM_ASSERT(rc == LSA_RET_OK);
				LSA_UNUSED_ARG(rc);
			}

			cm_acp_free_rqb (channel, acp);
		}
		return;

	case ACP_OPC_ALARM_SEND:
		cm_acp_alarm_send_done (channel, (CM_AR_GRAPH_PTR_TYPE)CM_ACP_GET_USER_ID_PTR(acp), acp);
		return;

	case ACP_OPC_ALARM_INDICATION:
		cm_acp_alarm_indication(channel, acp);
		return;

	case ACP_OPC_ALARM_ACK_SEND:
		cm_acp_alarm_ack_send_done (channel, (CM_AR_GRAPH_PTR_TYPE)CM_ACP_GET_USER_ID_PTR(acp), acp);
		return;

	case ACP_OPC_ALARM_ACK_INDICATION:
		cm_acp_alarm_ack_indication (channel, acp);
		return;

	case ACP_OPC_ALARM_CLOSE:
		cm_acp_alarm_remove_done(channel, CM_CAST_ARGR_ALARMCR_PTR(CM_ACP_GET_USER_ID_PTR(acp)), acp);
		return;

	default:
		CM_FATAL (); /* should not happen */
		return;
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
