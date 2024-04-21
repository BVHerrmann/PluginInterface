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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_low.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the lower LSA interface                                       */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  4
#define ACP_MODULE_ID      LTRC_ACT_MODUL_ID

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/*------------------------------------------------------------------------------
//	alloc a lower request-block, with an 'one-size-fits-all' pParam
//----------------------------------------------------------------------------*/
ACP_LOWER_RQB_PTR_TYPE
acp_ll_alloc_rqb (
	ACP_LOWER_CONST_PTR			channel_lower
	)
{
	ACP_LOWER_RQB_PTR_TYPE  edd;
	LSA_UINT16	rc;

	ACP_ALLOC_LOWER_RQB (ACP_CAST_LOWER_RQB_PTR_PTR (& edd),
			acp_null_usr_id, sizeof(ACP_LOWER_RQB_TYPE), channel_lower->sysptr
		);
	if (acp_is_null (edd)) {
		ACP_LOWER_TRACE_00 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "!lower_rqb"
			);
		return LSA_NULL;
	}

	rc = sizeof (ACP_LL_USED_DATA_TYPE);

	ACP_ALLOC_LOWER_MEM (ACP_CAST_LOWER_MEM_PTR_PTR (& edd->pParam),
			acp_null_usr_id, rc, channel_lower->sysptr
		);
	if (acp_is_null (edd->pParam)) {
		ACP_LOWER_TRACE_00 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "!lower_mem"
			);
		ACP_FREE_LOWER_RQB (& rc, edd, channel_lower->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		return LSA_NULL;
	}
	return edd;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
acp_ll_request (
	ACP_LOWER_CONST_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  edd,
	EDD_SERVICE  service,
	LSA_VOID_PTR_TYPE  user_id
	)
{
	ACP_LL_SET_HANDLES (edd, channel_lower->handle_lower); /* TIA 2709127 */
	ACP_LL_SET_OPCODE (edd, EDD_OPC_REQUEST);
	ACP_LL_SET_SERVICE (edd, service);
	ACP_LL_SET_USERID_PTR (edd, user_id);

	ACP_ASSERT (
			/* not closing */
			channel_lower->count_of_open_channel != 0xFFFF
		||	/* closing and cancel */
			(channel_lower->count_of_open_channel == 0xFFFF  &&  service == EDD_SRV_NRT_CANCEL)
		);

	ACP_LOWER_TRACE_05 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW,
			">>> lower(%08x) h(%u) opc(%u) service(0x%x) user_id(%#x)",
			edd, ACP_LL_GET_HANDLE (edd),
			ACP_LL_GET_OPCODE (edd), ACP_LL_GET_SERVICE (edd),
			ACP_LL_GET_USERID_PTR (edd)
		);

	ACP_REQUEST_LOWER (edd, channel_lower->sysptr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
acp_ll_free_rqb (
	ACP_LOWER_CONST_PTR		channel_lower,
    ACP_LOWER_RQB_PTR_TYPE	lower_rqb_ptr
	)
{
	LSA_UINT16	rc;

	if (acp_is_not_null (ACP_LL_GET_PPARAM (lower_rqb_ptr, LSA_VOID_PTR_TYPE))) {
		ACP_FREE_LOWER_MEM (& rc, ACP_LL_GET_PPARAM (lower_rqb_ptr, ACP_LOWER_MEM_PTR_TYPE), channel_lower->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		ACP_LL_SET_PPARAM (lower_rqb_ptr, LSA_NULL);
	}
	ACP_FREE_LOWER_RQB (& rc, lower_rqb_ptr, channel_lower->sysptr);
	ACP_ASSERT (rc == LSA_RET_OK);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
acp_channel_open_done (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT32  rsp
	)
{
    ACP_LOWER_RQB_PTR_TYPE  edd;
	LSA_UINT16  i;

	ACP_ASSERT (channel->state == ACP_CHANNEL_STATE_RESERVED);
	ACP_ASSERT (channel->lower->count_of_open_channel >= 0xFF00);
	ACP_ASSERT (acp_is_not_null (channel->upper));
	ACP_ASSERT (ACP_RQB_GET_OPCODE (channel->upper) == ACP_OPC_OPEN_CHANNEL);
	ACP_ASSERT (channel->detailptr->channel_type == ACP_CHANNEL_TYPE_SYSTEM);

	if (rsp != ACP_OK) {
		ACP_LOWER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel.lower.nic_id(%d) cnt(0x%x) rsp(%d)"
			, channel->lower->nic_id
			, channel->lower->count_of_open_channel
			, rsp
			);

		channel->lower->count_of_open_channel = 0;
		channel->lower->nic_id = 0;
		channel->lower->handle_lower = 0;

			/* set result, ready with open, notify the user */
		channel->upper->args->channel.open.handle = ACP_INVALID_HANDLE;
		acp_callback_and_free_channel (channel, rsp, channel->upper);
		return;
	}
	else {
		ACP_LOWER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel.lower.nic_id(%d) cnt(0x%x) rsp(%d)"
			, channel->lower->nic_id
			, channel->lower->count_of_open_channel
			, rsp
			);
	}

		/* set result, ready with open */
	channel->lower->count_of_open_channel = 1;
	channel->upper->args->channel.open.handle = channel->my_handle;
	channel->state = ACP_CHANNEL_STATE_READY; /* channel is usable */

		/* providing RQBs for nrt-recv */
	for (i = 0; i < channel->detailptr->type.system.max_edd_nrt_recv_res; ++i) {
		edd = acp_nrt_recv_alloc (channel->lower);
		if (acp_is_null (edd)) {
			ACP_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_nrt_recv_res(%d)"
				, channel->detailptr->type.system.max_edd_nrt_recv_res
				);
			acp_close_channel (channel->upper);
			return;
		}
		acp_nrt_recv_provide (channel->lower, edd);
	}

		/* providing RQBs for RTA-nrt-send */
	for (i = 0; i < channel->detailptr->type.system.max_edd_nrt_send_res; ++i) {
		edd = acp_nrt_send_alloc (channel->lower, EDD_FRAME_BUFFER_LENGTH);
		if (acp_is_null (edd)) {
			ACP_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_nrt_send_res(%d)"
				, channel->detailptr->type.system.max_edd_nrt_send_res
				);
			acp_close_channel (channel->upper);
			return;
		}
		acp_rta_sm_q_or_send_provide (channel, edd);
	}

		/* all done, notify the user */
	acp_callback (channel, ACP_OK);
}

/*------------------------------------------------------------------------------
//	entry-function for the lower layer.
//		the channel is acquire via the field user_id, and not via the lsa-handle
//		this is nesessary, because more than one upper layer can use the same
//		lower layer (system-channel - behaviour).
//----------------------------------------------------------------------------*/
LSA_VOID
acp_request_lower_done (
    ACP_LOWER_RQB_PTR_TYPE  lower		/* in.: the response */
	)
{
	ACP_CHANNEL_PTR  channel;
	LSA_UINT32  rsp;
	LSA_VOID_PTR_TYPE  user_id;
	ACP_LOWER_PTR  channel_lower;

	ACP_LL_OPEN_CHANNEL_PTR_TYPE  edd_open;

	if (acp_is_null (lower)) {
		ACP_FATAL (0);
		return;
	}

	channel = acp_channel_from_handle (ACP_LL_GET_HANDLE (lower));
	if (acp_is_null (channel)) {
		ACP_FATAL (ACP_LL_GET_HANDLE (lower));
		return;
	}

		/* keeps lower state for callback */
	rsp = acp_rsp_from_edd_rsp (ACP_LL_GET_RESPONSE (lower));

	if (rsp == ACP_OK || rsp == ACP_OK_CANCEL) {
		ACP_LOWER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW,
				"<<< lower(%08x) h(%u) opc(%d) service(0x%x) rsp(0x%x)",
				lower, ACP_LL_GET_HANDLE (lower),
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower),
				ACP_LL_GET_RESPONSE (lower)
			);
	}
	else {
		ACP_LOWER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP,
				"<<< lower(%08x) h(%u) opc(%d) service(0x%x) rsp(0x%x)",
				lower, ACP_LL_GET_HANDLE (lower),
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower),
				ACP_LL_GET_RESPONSE (lower)
			);
	}

		/* check for all opcodes / services */
	user_id = ACP_LL_GET_USERID_PTR (lower);
	if (acp_is_null (user_id)) {
		ACP_LOWER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "user_id of RQB(0x%08x) is null", lower);
		ACP_FATAL (user_id);
		return; /* never */
	}

	if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_REQUEST) {

		LSA_VOID_PTR_TYPE param = ACP_LL_GET_PPARAM (lower, LSA_VOID_PTR_TYPE);
		if (acp_is_null (param)) {
			ACP_FATAL ((LSA_UINT32)lower);
			return;
		}

	switch (ACP_LL_GET_SERVICE(lower)) {
/*----------------------------------------------------------------------------*/
	case EDD_SRV_GET_PARAMS:
		channel = (ACP_CHANNEL_PTR) user_id;

		acp_op_channel_info_done (channel, lower);
		break;

/*------------------------------------------------------------------------------
// ALARM / RTA
//----------------------------------------------------------------------------*/

	case EDD_SRV_NRT_SEND:
		{
		acp_nrt_send_done (lower, user_id);
		}
		break;

	case EDD_SRV_NRT_RECV:
		{
		channel_lower = (ACP_LOWER_PTR) user_id;

		if (channel_lower->count_of_open_channel == 0xFFFF		/* channel closing */
		||	ACP_LL_GET_RESPONSE (lower) == EDD_STS_OK_CANCEL
		) {
			acp_nrt_recv_free (channel_lower, & lower);
		}
		else {
			acp_nrt_recv_indication (channel_lower, lower);
		}
		}
		break;

	case EDD_SRV_NRT_CANCEL:
		{
		ACP_LL_NRT_CANCEL_PTR_TYPE  nrt_cancel;
		LSA_UINT16  rc;

		channel = (ACP_CHANNEL_PTR) user_id;

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFFFF);
		rsp = rsp; /* ignore */

		nrt_cancel = ACP_LL_GET_PPARAM (lower, ACP_LL_NRT_CANCEL_PTR_TYPE);
		ACP_ASSERT (acp_is_not_null (nrt_cancel));
		ACP_FREE_LOWER_MEM (& rc, nrt_cancel, channel->sysptr);
		ACP_ASSERT (rc == LSA_RET_OK);
		ACP_LL_SET_PPARAM (lower, LSA_NULL);

		ACP_LL_SET_OPCODE (lower, EDD_OPC_CLOSE_CHANNEL);
		ACP_LL_SET_SERVICE (lower, 0);
		ACP_LL_SET_USERID_PTR (lower, channel);
		ACP_LL_SET_HANDLES (lower, channel->lower->handle_lower); /* TIA 2709127 */

			/* real close is done in callback */
		ACP_CLOSE_CHANNEL_LOWER (lower, channel->lower->sysptr);
		}
		break;


/*----------------------------------------------------------------------------*/
	default:
		{
		ACP_LOWER_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled service(%d) rsp(%d)",
				ACP_LL_GET_SERVICE (lower), ACP_LL_GET_RESPONSE (lower)
			);
		ACP_FATAL (ACP_LL_GET_SERVICE (lower));
		}
		break;
	}
	}
/*----------------------------------------------------------------------------*/
	else if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_OPEN_CHANNEL) {
		channel = (ACP_CHANNEL_PTR) user_id;
		edd_open = ACP_LL_GET_PPARAM (lower, ACP_LL_OPEN_CHANNEL_PTR_TYPE);
		if (acp_is_null (edd_open)) {
			ACP_FATAL ((LSA_UINT32)lower);
			return;
		}

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFF00);

			/* set result */
		channel->lower->handle_lower = edd_open->HandleLower; /* TIA 2709127 */
		acp_ll_free_rqb (channel->lower, lower);

			/* confirm to upper */
		acp_channel_open_done (channel, rsp);
		return;
	}
/*----------------------------------------------------------------------------*/
	else if (ACP_LL_GET_OPCODE (lower) == EDD_OPC_CLOSE_CHANNEL) {
		channel = (ACP_CHANNEL_PTR) user_id;

		ACP_ASSERT (channel->lower->count_of_open_channel == 0xFFFF);

		if (rsp == ACP_OK) {
			channel->lower->nic_id = 0;
			channel->lower->count_of_open_channel = 0;
			channel->lower->handle_lower = 0;

			acp_ll_free_rqb (channel->lower, lower);

				/* free all provided resourcen */
			acp_free_resource (channel);

			if (ACP_RQB_GET_OPCODE (channel->upper) == ACP_OPC_OPEN_CHANNEL) {
				ACP_LOWER_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "no resource on open_channel"
					);
				rsp = ACP_ERR_RESOURCE;
			}

				/* fin */
			acp_callback_and_free_channel (channel, rsp, channel->upper);
		}
		else {
			channel->lower->count_of_open_channel = 1; /* see acp_close_channel */
			acp_ll_free_rqb (channel->lower, lower);
			acp_callback (channel, rsp); /* report bad result */
		}
		return;
	}
/*----------------------------------------------------------------------------*/
	else {
		ACP_LOWER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%d) service(%d) rsp(%d)",
				ACP_LL_GET_OPCODE (lower), ACP_LL_GET_SERVICE (lower), ACP_LL_GET_RESPONSE (lower)
			);
		ACP_FATAL (ACP_LL_GET_OPCODE (lower));
		return;
	}

	return;
}


#if 0 /* LSA-function, not used */
/*-----------------------------------------------------------------------------
// LSA-function, not used
//---------------------------------------------------------------------------*/
LSA_VOID acp_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr)
{
	user_id = user_id;
	length  = length;
	lower_rqb_ptr = lower_rqb_ptr;
	return;
}
/*-----------------------------------------------------------------------------
// LSA-function, not used
//---------------------------------------------------------------------------*/
LSA_VOID acp_lower_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_MEM_PTR_TYPE     lower_mem_ptr)
{
	user_id = user_id;
	length  = length;
	lower_mem_ptr = lower_mem_ptr;
	return;
}
#endif


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
