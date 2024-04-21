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
/*  F i l e               &F: cm_acp3.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-AR-ACP                                                                */
/*  - alarm things                                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  22
#define CM_MODULE_ID       22

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_alarm_type_is_internal ( /* user is not allowed to send this alarm-type */
	LSA_UINT16  alarm_type
)
{
	LSA_UINT32  alarm_bit;

	if (alarm_type < 0x0020) {

		switch( alarm_type ) {

		case 0x0000: /* reserved */
			return LSA_TRUE;

		case CM_ALARM_TYPE_MCR_MISMATCH:              /* internal, from m-consumer */
		case CM_ALARM_TYPE_MEDIA_REDUNDANCY:          /* internal, from CM-PD */
		case CM_ALARM_TYPE_PORT_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_SYNC_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM: /* internal, from CM-PD */
		case CM_ALARM_TYPE_TIME_DATA_CHANGED:         /* internal, from CM-PD */
		case CM_ALARM_TYPE_DFP_PROBLEM:               /* internal, from CM-PD */
		case CM_ALARM_TYPE_MRPD_PROBLEM:              /* internal, from CM-PD */
		case CM_ALARM_TYPE_MULTIPLE_INTERFACE:        /* internal, from CM-PD */
			return LSA_TRUE;

		default:
			break;
		}

		alarm_bit = (1UL << alarm_type);

		if ((alarm_bit & CM_ALARM_PLUG_GROUP) != 0) {
			return LSA_TRUE;
		}
	}
	else if (alarm_type < 0x0100) {
		/* manufacturer or profile specific */
	}
	else {
		/* reserved */
		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
/*lint -e{818} symbol acp_al Pointer parameter 'Symbol' (Location) could be declared ptr to const request 1524437*/
LSA_VOID
cm_acp_alarm_copy_ack(
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp,        /* destination (alarm-ack request) */
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al, /* source (ACP alarm indication) */
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,        /* source (CM alarm response) */
	LSA_UINT32 cm_pnio_err
)
{
	ACP_UPPER_ALARM_DATA_PTR_TYPE  ack = &acp->args->alarm.data;

	if( cm_is_not_null(acp_al) )
	{
		ack->priority        = acp_al->priority;
		ack->api             = acp_al->api;
		ack->alarm_type      = acp_al->alarm_type;
		ack->slot_nr         = acp_al->slot_nr;
		ack->subslot_nr      = acp_al->subslot_nr;
		ack->alarm_specifier = acp_al->alarm_specifier;
	}
	else if( cm_is_not_null(cm_al) )
	{
		ack->priority   = cm_al->alarm_priority;
		ack->api        = cm_al->api;
		ack->alarm_type = cm_al->alarm_type;
		ack->slot_nr    = cm_al->slot_nr;
		ack->subslot_nr = cm_al->subslot_nr;

		ACP_SET_ALARM_SPECIFIER (&ack->alarm_specifier,
				cm_al->alarm_sequence,
				(cm_al->diag_channel_available == CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES),
				(cm_al->diag_generic_available == CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES),
				(cm_al->diag_submod_available  == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES),
				(cm_al->ar_diagnosis_state     == CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES)
			);
	}
	else
	{
		CM_FATAL();
	}

	ack->pnio_status = cm_pnio_err;

	/* rest is don't care for an alarm-ack */

	ack->module_ident = 0;
	ack->submodule_ident = 0;
	ack->user_tag = 0;
	ack->user_max_length = 0;
	ack->user_length = 0;
	ack->user_data = LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_send (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,
	LSA_UINT16  upper_alarm_tag,
	LSA_UINT16  upper_alarm_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  upper_alarm_data,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_ACP_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "alarm_send:alarm_cr(%08x) local_alarm_ref(%u)"
		, argr->alarm_cr, argr->alarm_cr->local_alarm_ref
		);

	cm_acp_alarm_copy_ack(acp, LSA_NULL, cm_al, 0);

	acp->args->alarm.data.module_ident		= cm_al->mod_ident;
	acp->args->alarm.data.submodule_ident	= cm_al->sub_ident;

	acp->args->alarm.data.user_tag			= upper_alarm_tag;         /* not: cm_al->alarm_... */
	acp->args->alarm.data.user_max_length	= upper_alarm_data_length; /* not: argr->ar_com.channel->acp.max_alarm_data_length */
	acp->args->alarm.data.user_length		= upper_alarm_data_length; /* not: cm_al->alarm_... */
	acp->args->alarm.data.user_data			= upper_alarm_data;        /* not: cm_al->alarm_... */

	/***/

	acp->args->alarm.data.alarm_ref			= argr->alarm_cr->local_alarm_ref;
	acp->args->alarm.data.usr_id			= 0; /* unused */

	cm_acp_request_lower (argr->ar_com.channel, ACP_OPC_ALARM_SEND, argr, acp);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	if (cm_is_null (argr))
	{
		CM_FATAL ();
		return;
	}

	if (CM_ACP_GET_RESPONSE (acp) != ACP_OK)
	{
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_WARN, "rsp(%u)", CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALSND, CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down(argr, CM_AR_REASON_ALSND);
	}

	switch (channel->path_type)
	{
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_alarm_send_done (channel, argr, acp);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_ar_almi_send_done (argr, acp);
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_ack_send (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_ACP_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "alarm_cr(%08x) local_alarm_ref(%u)"
		, argr->alarm_cr, argr->alarm_cr->local_alarm_ref
		);

	acp->args->alarm.data.alarm_ref = argr->alarm_cr->local_alarm_ref;
	acp->args->alarm.data.usr_id    = 0; /* unused */

	cm_acp_request_lower (argr->ar_com.channel, ACP_OPC_ALARM_ACK_SEND, argr, acp);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_ack_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	if (cm_is_null (argr))
	{
		CM_FATAL ();
		return;
	}

	if (CM_ACP_GET_RESPONSE (acp) != ACP_OK)
	{
		CM_ACP_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_WARN, "rsp(%u)", CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALACK, CM_ACP_GET_RESPONSE (acp));
		cm_ar_com_down(argr, CM_AR_REASON_ALACK);
	}

	switch (channel->path_type)
	{
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_alarm_ack_send_done (channel, argr, acp);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_ar_alarm_ack_send_done (argr, acp);
		break;

#endif
	default:
		CM_FATAL(); /* a bug */
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al = & acp->args->alarm.data;
	LSA_UINT16  prio = acp_al->priority;
	CM_AR_GRAPH_PTR_TYPE  ar_graph;

#if CM_ALARM_PRIORITY_LOW != 0 || CM_ALARM_PRIORITY_HIGH != 1
# error "check this!"
#endif

	ar_graph = (CM_AR_GRAPH_PTR_TYPE)acp_al->usr_id;

	if (cm_is_null (ar_graph) || prio > 1)
	{
		CM_FATAL ();
		return; /* unreachable */
	}

	/***/

	if( cm_ar_com_is_down (ar_graph) )
	{
		/* do nothing */
	}
	else if (CM_ACP_GET_RESPONSE (acp) != ACP_OK)
	{
		CM_ACP_TRACE_02 (ar_graph->trace_idx, LSA_TRACE_LEVEL_WARN
			, "alarm_indication: !ok, acp-rsp(%u) pnio_status(0x%08x)"
			, CM_ACP_GET_RESPONSE (acp), acp_al->pnio_status
			);

		if (CM_ACP_GET_RESPONSE (acp) == ACP_ERR_ASRT) /* AP00678183 */
		{
			/*
			 * a) ACP got an alarm but could not send the asrt-ack (pnio_status is 0)
			 * b) ACP could not parse the asrt-data (pnio_status is 0)
			 * c) ACP got an asrt-error (pnio_status is taken from the asrt-error)
			 */
			if (ar_graph->ar_com.down == CM_AR_REASON_NONE) { /* should make a cm_ar_com_down3()... */
				ar_graph->ar_com.down_pnio_status = acp_al->pnio_status;
			}
		}

		cm_ar_com_down_debug_data_fmt (ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_ASRT, ar_graph->ar_com.down_pnio_status);
		cm_ar_com_down (ar_graph, CM_AR_REASON_ASRT);
	}
	else if (acp_al->user_length == 0xFFFF) /* truncated */
	{
		CM_ACP_TRACE_01 (ar_graph->trace_idx, LSA_TRACE_LEVEL_WARN
			, "alarm data truncated, more data than user_max_length(%u)", acp_al->user_max_length
			);
		cm_ar_com_down_debug_data_fmt (ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_ALLEN, acp_al->user_max_length);
		cm_ar_com_down(ar_graph, CM_AR_REASON_ALLEN);
	}
	else
	{
		switch (channel->path_type)
		{
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			acp = cm_cl_acp_alarm_indication (ar_graph, prio, acp);
			break;
#endif

#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			acp = cm_sv_ar_alarm_indication (ar_graph, prio, acp);
			break;
#endif

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}

	if (cm_is_not_null(acp))
	{
		/* reprovide acp rqb */
		cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_copy_ind (
	CM_UPPER_ALARM_PTR_TYPE  cm_al, /* destination */
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp, /* source (ACP alarm indication) */
	LSA_UINT32  maintenance_status
)
{
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al = & acp->args->alarm.data;

	/*
	cm_al->alarm_priority = set in caller
	*/

	cm_al->api				= acp_al->api;
	cm_al->slot_nr			= acp_al->slot_nr;
	cm_al->subslot_nr		= acp_al->subslot_nr;

	cm_al->alarm_type		= acp_al->alarm_type;

	cm_al->alarm_sequence         = acp_al->alarm_specifier  & 0x07FF;
	cm_al->diag_channel_available = (acp_al->alarm_specifier & 0x0800) ? CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO;
	cm_al->diag_generic_available = (acp_al->alarm_specifier & 0x1000) ? CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO;
	cm_al->diag_submod_available  = (acp_al->alarm_specifier & 0x2000) ? CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;
	cm_al->ar_diagnosis_state     = (acp_al->alarm_specifier & 0x8000) ? CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO;
	cm_al->reserved			= 0;

	cm_al->mod_ident		= acp_al->module_ident;
	cm_al->sub_ident		= acp_al->submodule_ident;

	/***/

	cm_al->cm_pnio_err		= CM_PNIO_ERR_MAKE (0xff, 0xff, 0xff, 0xff); /* docu says: do not evaluate */

	cm_al->maintenance_status = maintenance_status;

	/***/

	cm_al->alarm_tag		= acp_al->user_tag;

	if (acp_al->user_length == 0) {
		cm_al->alarm_data_length = CM_ALARM_OFFSET_USI;
	}
	else {
		cm_al->alarm_data_length = CM_CAST_U16 (CM_ALARM_OFFSET_DATA + acp_al->user_length);
	}

	cm_al->alarm_data		= acp_al->user_data; /* copy pointer to cm-rqb */

	/***/

	CM_ACP_TRACE_07(0, LSA_TRACE_LEVEL_CHAT,
		"dev/ar(%u/%u) al_type(%u) api(%u) slot(%u) subslot(%u) alarm_tag(%u)",
		cm_al->device_nr, cm_al->ar_nr,
		cm_al->alarm_type,
		cm_al->api, cm_al->slot_nr, cm_al->subslot_nr,
		cm_al->alarm_tag
		);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_acp_alarm_ack_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al = & acp->args->alarm.data;
	LSA_UINT16  prio = acp_al->priority;
	CM_AR_GRAPH_PTR_TYPE  argr;

#if CM_ALARM_PRIORITY_LOW != 0 || CM_ALARM_PRIORITY_HIGH != 1
# error "check this!"
#endif

	argr = (CM_AR_GRAPH_PTR_TYPE)acp_al->usr_id;

	if (cm_is_null (argr) || prio > 1 || CM_ACP_GET_RESPONSE (acp) != ACP_OK)
	{
		CM_FATAL ();
		return; /* unreachable */
	}

	/***/

	if( cm_ar_com_is_down (argr) )
	{
		/* do nothing */
	}
	else
	{
		switch (channel->path_type) {
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			cm_cl_alarm_ack_indication (channel, argr, prio, acp);
			break;
#endif
#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			cm_sv_ar_almi_ack_indication (argr, prio, acp);
			break;
#endif
		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}

	cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp);
}

/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_acp_alarm_ack_matches (
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp,
	LSA_UINT32 * cm_pnio_err /* result from "acp" */
)
{
	LSA_UINT16  alarm_specifier;

	CM_ASSERT (CM_ACP_GET_RESPONSE (acp) == ACP_OK);

	*cm_pnio_err = acp->args->alarm.data.pnio_status;

	CM_ACP_TRACE_07(0, LSA_TRACE_LEVEL_CHAT
			, "acp-rsp(%u) type(%u) api(%u) slot(%u) sub(%u) seq(%u) acp-pnio_status(%#x)"
			, CM_ACP_GET_RESPONSE (acp)
			, cm_al->alarm_type, cm_al->api, cm_al->slot_nr, cm_al->subslot_nr
			, cm_al->alarm_sequence, *cm_pnio_err
		);

		/* if header doesn't match: protocol-error */
	ACP_SET_ALARM_SPECIFIER (& alarm_specifier,
			cm_al->alarm_sequence,
			(cm_al->diag_channel_available == CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES),
			(cm_al->diag_generic_available == CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES),
			(cm_al->diag_submod_available == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES),
			(cm_al->ar_diagnosis_state == CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES)
		);

	if (! (cm_al->alarm_type == acp->args->alarm.data.alarm_type
		&&	cm_al->api == acp->args->alarm.data.api
		&&	cm_al->slot_nr == acp->args->alarm.data.slot_nr
		&&	cm_al->subslot_nr == acp->args->alarm.data.subslot_nr
		&&	alarm_specifier == acp->args->alarm.data.alarm_specifier))
	{
		CM_ACP_TRACE_10 (0, LSA_TRACE_LEVEL_UNEXP,
				"alarm-header doesn't match: type(%u/%u) api(%u/%u) slot(%u/%u) subslot(%u/%u) al_spec(0x%x/0x%x)",
				cm_al->alarm_type, acp->args->alarm.data.alarm_type,
				cm_al->api, acp->args->alarm.data.api,
				cm_al->slot_nr, acp->args->alarm.data.slot_nr,
				cm_al->subslot_nr, acp->args->alarm.data.subslot_nr,
				alarm_specifier, acp->args->alarm.data.alarm_specifier
			);
		return LSA_FALSE;
	}

	CM_ACP_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT, "ok");
	return LSA_TRUE;
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
