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
/*  F i l e               &F: acp_opc.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the various OPCODEs                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  5
#define ACP_MODULE_ID      LTRC_ACT_MODUL_ID

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)

/*------------------------------------------------------------------------------
//	free resource of one channel
//----------------------------------------------------------------------------*/
LSA_VOID
acp_free_resource (ACP_CHANNEL_CONST_PTR  channel)
{
	ACP_UPPER_RQB_PTR_TYPE  upper;

	while(!AcpListIsEmpty(&channel->alarm_provide[ACP_ALARM_PRIORITY_LOW])){
		AcpListRemoveHead(&channel->alarm_provide[ACP_ALARM_PRIORITY_LOW], upper, ACP_UPPER_RQB_PTR_TYPE);
		ACP_ASSERT (acp_is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, &upper);
	}

	while(!AcpListIsEmpty(&channel->alarm_provide[ACP_ALARM_PRIORITY_HIGH])){
		AcpListRemoveHead(&channel->alarm_provide[ACP_ALARM_PRIORITY_HIGH], upper, ACP_UPPER_RQB_PTR_TYPE);
		ACP_ASSERT (acp_is_not_null (upper));
		acp_callback_async (channel, ACP_OK_CANCEL, &upper);
	}

	if (channel->detailptr->channel_type == ACP_CHANNEL_TYPE_SYSTEM) {
		ACP_LOWER_RQB_PTR_TYPE	nrt;

		while (! AcpListIsEmpty (& channel->lower->q_or_send.nrt_send_list)) {
			AcpListRemoveHead (& channel->lower->q_or_send.nrt_send_list, nrt, ACP_LOWER_RQB_PTR_TYPE);
			ACP_ASSERT (acp_is_not_null (nrt));
			acp_nrt_send_free (channel->lower, & nrt);
		}
	}

	return;
}

/*------------------------------------------------------------------------------
//	requests the channel-info-data from EDD
//----------------------------------------------------------------------------*/
static LSA_UINT16
acp_edd_channel_info (ACP_CHANNEL_PTR  channel)
{
	ACP_LOWER_RQB_PTR_TYPE  lower;

	lower = acp_ll_alloc_rqb (channel->lower);
	if (acp_is_null (lower)) {
		return ACP_ERR_ALLOC_LOWER_RQB;
	}

	ACP_ASSERT (acp_is_not_null (ACP_LL_GET_PPARAM (lower, LSA_VOID_PTR_TYPE)));

	acp_ll_request (channel->lower, lower, EDD_SRV_GET_PARAMS, channel);
	return ACP_OK;
}


/*------------------------------------------------------------------------------
//	maps the edd_response-code to an acp-response-code
//----------------------------------------------------------------------------*/
LSA_UINT16
acp_rsp_from_edd_rsp (
	LSA_UINT16  edd_rsp
	)
{
	switch (edd_rsp) {
	case EDD_STS_OK:			return ACP_OK;
	case EDD_STS_OK_CANCEL:		return ACP_OK_CANCEL;
	case EDD_STS_ERR_PARAM:		return ACP_ERR_EDD_PARAM;
	case EDD_STS_ERR_RESOURCE:	return ACP_ERR_EDD_RESOURCE;
	default:					return ACP_ERR_LOWER_LAYER;
	}
}


/*------------------------------------------------------------------------------
//	handle request for Opcode ACP_OPC_CHANNEL_INFO
//----------------------------------------------------------------------------*/
LSA_VOID
acp_op_channel_info (
	ACP_CHANNEL_PTR  channel
	)
{
	LSA_UINT16 rc;

	rc = acp_edd_channel_info (channel);
	if (rc != ACP_OK) {
		acp_callback (channel, rc);
	}
}


/*------------------------------------------------------------------------------
//	handle response for Opcode ACP_OPC_CHANNEL_INFO
//----------------------------------------------------------------------------*/
LSA_VOID
acp_op_channel_info_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	)
{
	ACP_LL_SRT_PARAM_PTR_TYPE  edd_params;
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_CHANNEL_UPPER_INFO_PTR_TYPE  info;
	LSA_UINT16  rsp;

	rsp = ACP_LL_GET_RESPONSE (lower);
	edd_params = ACP_LL_GET_PPARAM (lower, ACP_LL_SRT_PARAM_PTR_TYPE);
	ACP_ASSERT (acp_is_not_null (edd_params));

	if (rsp != EDD_STS_OK) {
		ACP_UPPER_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rsp(%d)"
			, rsp
			);
		acp_ll_free_rqb (channel->lower, lower);
		acp_callback (channel, ACP_ERR_LOWER_LAYER);
		return;
	}

	upper = channel->upper;
	if (acp_is_null (upper)) {
		ACP_FATAL (channel);
		return;
	}

	info = & upper->args->channel.info;

	info->eth_interface_id			= (LSA_UINT16) edd_params->InterfaceID;

	ACP_MEMCPY (channel->mac_addr,	  edd_params->MACAddr.MacAdr, ACP_MAC_ADDR_SIZE);

	channel->trace_idx				= (LSA_UINT16) edd_params->TraceIdx;
	if (channel->lower->trace_idx == 0) {
		channel->lower->trace_idx	= (LSA_UINT16) edd_params->TraceIdx;
	}
	else {
		ACP_ASSERT (channel->lower->trace_idx == (LSA_UINT16)edd_params->TraceIdx);
	}

	info->alarm_ref_base			= channel->detailptr->type.user.alarm_ref_base;
	info->alarm_nr_of_ref			= channel->detailptr->type.user.alarm_nr_of_ref;

	acp_ll_free_rqb (channel->lower, lower);
	acp_callback (channel, ACP_OK);
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
