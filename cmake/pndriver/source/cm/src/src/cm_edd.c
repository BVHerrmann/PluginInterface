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
/*  F i l e               &F: cm_edd.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EDD things                                                               */
/*                                                                           */
/*  - alloc EDD-RQB                                                          */
/*  - free EDD-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	40
#define CM_MODULE_ID		40

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

static LSA_VOID
cm_edd_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd /* provide: LSA_NULL */
);

static LSA_BOOL
cm_edd_state_indication_provide (
	CM_CHANNEL_PTR_TYPE channel
);

/*===========================================================================*/
/*=== EDD CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID
cm_edd_channel_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	/*
	channel->lower.info.edd.xxx ... checked in cm_edd_channel_info_done()
	*/

	cm_edd_link_init(channel);

	/* provide some PRM indication resources (must do it in context of CM-EDD, not CM-PD) */

	cm_edd_prm_indication(channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16
cm_edd_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	/* NOTE: caller wipes out channel->usr.xxx when returning CM_OK */

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	if( cm_is_not_null(channel->lower.channel[CM_PATH_TYPE_PD]) ) {
		CM_FATAL(); /* cm-pd not unlinked */
	}

	cm_edd_link_undo_init (channel);

	return CM_OK;
}

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_edd_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	channel = channel->lower.channel[CM_PATH_TYPE_EDD];

	if( cm_is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	get channel-params
//---------------------------------------------------------------------------*/

CM_EDD_CHANNEL_PARAMS_PTR_TYPE
cm_edd_channel_params(
	CM_CHANNEL_PTR_TYPE channel
) {
	channel = cm_edd_channel_get(channel);

	return &channel->lower.info.edd;
}

/*-----------------------------------------------------------------------------
//	can set send-clock
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_can_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

	switch( (edd_params->HardwareType & EDD_HW_TYPE_USED_MSK) ) {

	case EDD_HW_TYPE_USED_STANDARD_MAC:
		return LSA_FALSE;

	case EDD_HW_TYPE_USED_ERTEC_400:
	case EDD_HW_TYPE_USED_ERTEC_200:
	case EDD_HW_TYPE_USED_SOC:
	case EDD_HW_TYPE_USED_ERTEC_200P:
	case EDD_HW_TYPE_USED_HERA:
		return LSA_TRUE;

	default:
		CM_FATAL(); /* unknown hardware, update! */
		return LSA_FALSE;
	}
}

/*-----------------------------------------------------------------------------
//	default sendclock prop
//---------------------------------------------------------------------------*/

LSA_UINT16
cm_edd_default_sendclock_prop(
	CM_CHANNEL_PTR_TYPE channel
) {

	LSA_UINT16 properties = 0;
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

	/* see TIA 612541 */

	switch ((edd_params->HardwareType & EDD_HW_TYPE_USED_MSK)) {

	case EDD_HW_TYPE_USED_STANDARD_MAC:
		properties = CM_SENDCLOCK_PROP_SC_FIX_RR_VAR;
		return properties;

	case EDD_HW_TYPE_USED_ERTEC_400:
	case EDD_HW_TYPE_USED_ERTEC_200:
	case EDD_HW_TYPE_USED_SOC:
		properties = CM_SENDCLOCK_PROP_SC_VAR_RR_FIX;
		return properties;

	case EDD_HW_TYPE_USED_ERTEC_200P:
	case EDD_HW_TYPE_USED_HERA:
		properties = CM_SENDCLOCK_PROP_SC_VAR_RR_FIX;
		return properties;

	default:
		CM_FATAL(); /* unknown hardware, update! */
		return properties;
	}
}



/*-----------------------------------------------------------------------------
//	can "partial data access" (consumer-iocrs)
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_can_partial_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	LSA_UINT32 mask;
	LSA_UINT32 supp;

	switch( rt_class ) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
		mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_MSK;
		supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT;
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_MSK;
		supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_SUPPORT;
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		mask = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_MSK;
		supp = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_SUPPORT;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return LSA_FALSE; /* unreachable */
	}

	if( (edd_params->HardwareType & mask) == supp ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	EDDI BUG: max consumer partial data length
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_bug_consumer_partial_data_length_allowed ( /* 1415794 */
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE iocr
	)
{
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

	switch( (edd_params->HardwareType & EDD_HW_TYPE_USED_MSK) )
	{
	case EDD_HW_TYPE_USED_ERTEC_200:
	case EDD_HW_TYPE_USED_SOC:
		if (iocr->data_length > 40)
		{
			return LSA_FALSE;
		}
		else
		{
			LSA_UINT16  p_len = iocr->cm.partial_offset + iocr->cm.partial_length;

			if (36 <= p_len && p_len <= 39) /* see eddi_crt_cons.h / EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_xxx */
			{
				return LSA_FALSE;
			}
		}
		break;

	case EDD_HW_TYPE_USED_STANDARD_MAC:
	case EDD_HW_TYPE_USED_ERTEC_400:
	case EDD_HW_TYPE_USED_ERTEC_200P:
	case EDD_HW_TYPE_USED_HERA:
		break;

	default:
		CM_FATAL(); /* unknown hardware, update! */
		break;
	}

	return LSA_TRUE;
}


/*-----------------------------------------------------------------------------
//	can "autopadding" (provider-iocrs)
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_can_autopadding(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	LSA_UINT32 mask;
	LSA_UINT32 supp;

	switch( rt_class ) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
		mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_MSK;
		supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_SUPPORT;
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_MSK;
		supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT;
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		mask = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_MSK;
		supp = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_SUPPORT;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return LSA_FALSE; /* unreachable */
	}

	if( (edd_params->HardwareType & mask) == supp ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	get length of APDUStatus (0 if APDUStatus is not in front of the C_SDU)
//---------------------------------------------------------------------------*/

LSA_UINT16
cm_edd_get_apdu_status_length(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	LSA_UINT16 apdu_status_length;

	if( (edd_params->HardwareType & EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE_MSK) == EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE ) {

		apdu_status_length = 0; /* APDUStatus is in the KRAM, C_SDU is in the PAEARAM */
	}
	else {

		apdu_status_length = 4; /* APDUStatus and C_SDU are in the KRAM (first APDUStatus, then C_SDU) */
	}

	return apdu_status_length;
}

/*-----------------------------------------------------------------------------
//	can "time-sync slave"
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_can_time_sync_slave(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd = cm_edd_channel_params(channel);

	if( (edd->HardwareType & EDD_HW_TYPE_FEATURE_TIME_SLAVE_MSK) == EDD_HW_TYPE_FEATURE_TIME_SLAVE_SUPPORT ) {

		return LSA_TRUE; /* note: implies TimeASE */
	}

	return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	EDD is-HERA
//---------------------------------------------------------------------------*/

LSA_BOOL
cm_edd_is_HERA(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params (channel);

	if ((edd_params->HardwareType & EDD_HW_TYPE_USED_MSK) == EDD_HW_TYPE_USED_HERA)
	{
		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*-----------------------------------------------------------------------------
//	alloc lower EDD-RQB that fits for all requests
//---------------------------------------------------------------------------*/
CM_EDD_LOWER_RQB_PTR_TYPE
cm_edd_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	EDD_SERVICE					service /* use 0 for open- and close-channel */
)
{
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
	LSA_UINT16 length = 0;

	CM_EDD_LOWER_MEM_PTR_TYPE lower_mem;
	CM_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr;

		/* alloc the RQB */
	CM_EDD_ALLOC_LOWER_RQB (&lower_rqb_ptr, cm_null_user_id, sizeof (CM_EDD_LOWER_RQB_TYPE), edd_channel->sysptr);

	if (cm_is_null (lower_rqb_ptr)) {
		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc EDD RQB, len(%u)"
			, channel->my_handle, sizeof (CM_EDD_LOWER_RQB_TYPE)
			);
		return LSA_NULL;
	}

	switch( service ) {

	case 0: /* special case, one for all, see open/close/prm */
		length = LSA_MAX(length, sizeof(EDD_RQB_OPEN_CHANNEL_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_GET_LINK_STATUS_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_GET_PARAMS_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_SENDCLOCK_CHANGE_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_CHANGE_PORT_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_INDICATION_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_PREPARE_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_WRITE_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_END_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_COMMIT_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_PRM_READ_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE));
		break;

	case EDD_SRV_SRT_TYPE: /* see cm_arcq_init, needed for reusing request */
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_CONSUMER_ADD_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_PROVIDER_ADD_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_GET_APDU_STATUS_TYPE));
		length = LSA_MAX(length, sizeof(EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE));
		break;

	case EDD_SRV_GET_LINK_STATUS:
		length = sizeof(EDD_RQB_GET_LINK_STATUS_TYPE);
		break;

	case EDD_SRV_GET_PARAMS:
		length = sizeof(EDD_RQB_GET_PARAMS_TYPE);
		break;

	case EDD_SRV_GET_PORT_PARAMS:
		length = sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE);
		break;

	case EDD_SRV_SENDCLOCK_CHANGE:
		length = sizeof(EDD_RQB_SENDCLOCK_CHANGE_TYPE);
		break;

	case EDD_SRV_PRM_CHANGE_PORT:
		length = sizeof(EDD_RQB_PRM_CHANGE_PORT_TYPE);
		break;

	case EDD_SRV_PRM_INDICATION:
		length = sizeof(EDD_RQB_PRM_INDICATION_TYPE);
		break;

	case EDD_SRV_PRM_PREPARE:
		length = sizeof(EDD_RQB_PRM_PREPARE_TYPE);
		break;

	case EDD_SRV_PRM_WRITE:
		length = sizeof(EDD_RQB_PRM_WRITE_TYPE);
		break;

	case EDD_SRV_PRM_END:
		length = sizeof(EDD_RQB_PRM_END_TYPE);
		break;

	case EDD_SRV_PRM_COMMIT:
		length = sizeof(EDD_RQB_PRM_COMMIT_TYPE);
		break;

	case EDD_SRV_PRM_READ:
		length = sizeof(EDD_RQB_PRM_READ_TYPE);
		break;

	case EDD_SRV_NRT_SET_DCP_HELLO_FILTER:
		length = sizeof(EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE);
		break;

	case EDD_SRV_LINK_STATUS_IND_PROVIDE:
	case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT: /* uses the same type */
		length = sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE);
		break;

	case EDD_SRV_SRT_IND_PROVIDE:
		length = sizeof(EDD_RQB_CSRT_IND_PROVIDE_TYPE);
		break;

	case EDD_SRV_SRT_PROVIDER_SET_STATE:
		length = sizeof(EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE);
		break;

	case EDD_SRV_SRT_GET_APDU_STATUS:
		length = sizeof(EDD_RQB_CSRT_GET_APDU_STATUS_TYPE);
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}

	CM_EDD_ALLOC_LOWER_MEM (&lower_mem
		, cm_null_user_id, length, edd_channel->sysptr
		);

	if (cm_is_null (lower_mem)) {
		LSA_UINT16  rc;

		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc EDD MEM, len(%u)"
			, channel->my_handle, length
			);

		CM_EDD_FREE_LOWER_RQB (& rc, lower_rqb_ptr, edd_channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG(rc);

		return LSA_NULL;
	}

	EDD_RQB_SET_PPARAM(lower_rqb_ptr, lower_mem);

#if CM_DEBUG
	CM_EDD_SET_NEXT_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
	CM_EDD_SET_PREV_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
#endif

	return lower_rqb_ptr;
}


/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
/*lint -e{818} symbol lower_rqb_ptr Pointer parameter 'Symbol' (Location) could be declared ptr to const request 1524437*/
LSA_VOID
cm_edd_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
	LSA_UINT16	rc;

	CM_ASSERT (cm_is_not_null(lower_rqb_ptr));
	CM_ASSERT (cm_is_not_null(lower_rqb_ptr->pParam));

	CM_EDD_FREE_LOWER_MEM (
		& rc,
		lower_rqb_ptr->pParam,
		edd_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);

	CM_EDD_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		edd_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);

	LSA_UNUSED_ARG(rc);
}

/*-----------------------------------------------------------------------------
// get the preallocated RQB, allocate one if necessary
//---------------------------------------------------------------------------*/
CM_EDD_LOWER_RQB_PTR_TYPE
cm_edd_get_rqb (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_CHANNEL_PTR_TYPE  edd_channel = cm_edd_channel_get(channel);
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;

	if (cm_is_null (edd_channel->lower.prealloc.edd))
	{
		edd = cm_edd_alloc_rqb (channel, 0/*open*/);
	}
	else
	{
		edd = edd_channel->lower.prealloc.edd;
		edd_channel->lower.prealloc.edd = LSA_NULL;
	}

	return edd;
}


/*-----------------------------------------------------------------------------
// release the preallocated RQB, free surplus RQBs
//--------------------------------------------------------------------------*/
LSA_VOID
cm_edd_release_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
	)
{
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get (channel);

	if (cm_is_not_null (edd_channel->lower.prealloc.edd))
	{
		cm_edd_free_rqb (edd_channel, edd);
	}
	else
	{
		edd_channel->lower.prealloc.edd = edd;
	}
}

/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_edd_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_EDD);

	CM_ASSERT (cm_edd_channel_get(channel) == channel);

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.edd_handle == LSA_NULL); /* TIA 2709127 */

	/***/

	channel->lower.prealloc.edd = LSA_NULL;

	edd = cm_edd_get_rqb (channel);

	if (cm_is_null (edd)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	{
	EDD_UPPER_OPEN_CHANNEL_PTR_TYPE args = CM_EDD_GET_PPARAM(edd, EDD_UPPER_OPEN_CHANNEL_PTR_TYPE);

	args->HandleLower = LSA_NULL; /* TIA 2709127 */
	args->HandleUpper = channel->my_handle;
	args->SysPath = channel->usr.ch.curr->args.channel.open->sys_path;
	args->Cbf = cm_edd_request_lower_done;
	}

	CM_EDD_SET_USER_ID_PTR (edd, channel);

	CM_EDD_SET_HANDLES (edd, LSA_NULL); /* TIA 2709127 */
	CM_EDD_SET_OPCODE (edd, EDD_OPC_OPEN_CHANNEL, 0);

	CM_EDD_OPEN_CHANNEL_LOWER (edd, channel->sysptr);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_edd_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
)
{
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_EDD_GET_RESPONSE (edd) == EDD_STS_OK) {

		EDD_UPPER_OPEN_CHANNEL_PTR_TYPE args = CM_EDD_GET_PPARAM(edd, EDD_UPPER_OPEN_CHANNEL_PTR_TYPE);

		channel->lower.edd_handle = args->HandleLower; /* TIA 2709127 */

		cm_edd_release_rqb(channel, edd);

		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) handle(%u)"
			, channel->my_handle, channel->lower.edd_handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.edd_handle == LSA_NULL); /* TIA 2709127 */

		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) edd-rsp(%u)"
			, channel->my_handle, CM_EDD_GET_RESPONSE (edd)
			);

		rc = CM_ERR_LOWER_LAYER;

		cm_edd_free_rqb(channel, edd);
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_edd_channel_close (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_EDD);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	edd = cm_edd_get_rqb (channel);

	CM_ASSERT (cm_is_not_null(edd));

	CM_EDD_SET_USER_ID_HANDLE_DEV_AR (edd, channel->my_handle, 0, 0);

	CM_EDD_SET_HANDLES (edd, channel->lower.edd_handle); /* TIA 2709127 */
	CM_EDD_SET_OPCODE (edd, EDD_OPC_CLOSE_CHANNEL, 0);

	CM_EDD_CLOSE_CHANNEL_LOWER (edd, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_edd_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_EDD_GET_RESPONSE (edd);

	if (rc == EDD_STS_OK) {

		cm_edd_free_rqb (channel, edd);

		channel->lower.edd_handle = LSA_NULL; /* TIA 2709127 */
		channel->lower.prealloc.edd = LSA_NULL;

		CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate edd-handle, channel is not closed! */

		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) edd-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== EDD INFO ==============================================================*/
/*===========================================================================*/

LSA_VOID
cm_edd_channel_info(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;

	CM_ASSERT(channel->state == CM_CH_INFO);

	edd = cm_edd_get_rqb(channel);

	if( cm_is_null(edd) ) {
		cm_channel_done(channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	cm_edd_request_lower(channel, EDD_SRV_GET_PARAMS, 0, edd);
}

/*===========================================================================*/

static LSA_VOID
cm_edd_channel_info_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_INFO);

	rc = CM_EDD_GET_RESPONSE (edd);

	CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "h(%u) edd-rsp(%u)", channel->my_handle, rc);

	if (rc == EDD_STS_OK) {

		CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

		{
		EDD_UPPER_GET_PARAMS_PTR_TYPE args = CM_EDD_GET_PPARAM(edd, EDD_UPPER_GET_PARAMS_PTR_TYPE);

		channel->trace_idx = args->TraceIdx; /* note: all values valid */

		/***/

		if( args->InterfaceID < 1 || args->InterfaceID > 16 ) {
			CM_FATAL (); /* wrong configuration */
		}

		if( args->MaxPortCnt < 1 || args->MaxPortCnt > EDD_CFG_MAX_PORT_CNT ) {
			CM_FATAL (); /* wrong configuration */
		}

		if( (args->MACAddr.MacAdr[0] & 0x01) != 0 ) {
			CM_FATAL (); /* wrong configuration */
		}

		if( args->CycleBaseFactor == 0 ) {
			CM_FATAL(); /* wrong configuration */
		}

#if CM_CFG_USE_ACP
		if (args->ConsumerCntClass1 > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_1,
					args->ConsumerFrameIDBaseClass1
			) ) {
				CM_FATAL (); /* wrong configuration */
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_1,
					CM_CAST_U16(args->ConsumerFrameIDBaseClass1 + args->ConsumerCntClass1 - 1)
			) ) {
				CM_FATAL (); /* wrong configuration */
			}
		}

		if (args->ConsumerCntClass2 > 0) {

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_2,
					args->ConsumerFrameIDBaseClass2
			) ) {
				CM_FATAL (); /* wrong configuration */
			}

			if( ! cm_is_frame_id_within_normative_range(
					LSA_FALSE,
					LSA_FALSE,
					CM_IOCR_PROP_RT_CLASS_2,
					CM_CAST_U16(args->ConsumerFrameIDBaseClass2 + args->ConsumerCntClass2 - 1)
			) ) {
				CM_FATAL (); /* wrong configuration */
			}
		}

		if( args->ConsumerCntClass3 > 0 ) {

			if( args->ConsumerFrameIDBaseClass3 != 0x0100 ) { /* const, well-known to IRT planning algorithm */
				CM_FATAL(); /* wrong configuration */
			}
		}

		if( args->ConsumerCntClass3 > 0 || args->ProviderCntClass3 > 0 ) {

			if( (args->HardwareType & EDD_HW_TYPE_FEATURE_TIMESTAMP_MSK) != EDD_HW_TYPE_FEATURE_TIMESTAMP_SUPPORT ) {
				CM_FATAL(); /* a bug in EDD, IRT needs synchronization (which needs timestamping) */
			}
		}
#endif

		/***/

		edd_params->InterfaceID                = (LSA_UINT16)args->InterfaceID; /* note: shortened to U16 to fit CM */
		edd_params->HardwareType               = args->HardwareType;
		edd_params->MACAddr                    = args->MACAddr;
		edd_params->MaxPortCnt                 = args->MaxPortCnt;
		edd_params->MRPSupportedRole           = args->MRPSupportedRole;
		edd_params->CycleBaseFactor            = args->CycleBaseFactor;
		edd_params->ConsumerCntClass3          = args->ConsumerCntClass3;
		edd_params->ConsumerFrameIDBaseClass2  = args->ConsumerFrameIDBaseClass2;
		edd_params->ConsumerCntClass2          = args->ConsumerCntClass2;
		edd_params->ConsumerFrameIDBaseClass1  = args->ConsumerFrameIDBaseClass1;
		edd_params->ConsumerCntClass1          = args->ConsumerCntClass1;
		edd_params->ConsumerCntClass12Combined = (args->ConsumerCntClass12Combined == EDD_CONSUMERCNT_CLASS12_COMBINED) ? LSA_TRUE : LSA_FALSE;
		edd_params->ProviderCnt                = args->ProviderCnt;
		edd_params->ProviderCntClass3          = args->ProviderCntClass3;
		}

		/***/

		if(
			edd_params->ProviderCnt != 0
			|| edd_params->ProviderCntClass3 != 0
			|| edd_params->ConsumerCntClass1 != 0
			|| edd_params->ConsumerCntClass2 != 0
			|| edd_params->ConsumerCntClass3 != 0
		) {
			/* TIA 1066222: provide only if "CRT configured" (see EDD-details: UseCRT/UseCSRT/UseXRT) */

			if( ! cm_edd_state_indication_provide(channel) ) {

				rc = CM_ERR_RESOURCE;
				goto channel_info_fail;
			}
		}

		/***/

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_1_UNICAST],
			edd_params->ConsumerFrameIDBaseClass1,
			edd_params->ConsumerCntClass1
			)
		) {

				rc = CM_ERR_ALLOC_LOCAL;
				goto channel_info_fail;
		}

		/* note: CM_RANGE_ID_CLASS_1_MULTICAST is not supported */

		/***/

		if (! cm_range_alloc(
			&channel->res.id_range[CM_RANGE_ID_CLASS_2_UNICAST],
			edd_params->ConsumerFrameIDBaseClass2,
			edd_params->ConsumerCntClass2
			)
		) {

			rc = CM_ERR_ALLOC_LOCAL;
			goto channel_info_fail;
		}

		/* note: CM_RANGE_ID_CLASS_2_MULTICAST is not supported */

		/***/

		cm_edd_release_rqb (channel, edd);
		cm_channel_done (channel, CM_OK);
		return;
	}
	else {

		rc = CM_ERR_LOWER_LAYER;
	}

channel_info_fail:
	cm_edd_free_rqb (channel, edd);
	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== STATE INDICATION ======================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_edd_state_indication_provide (
	CM_CHANNEL_PTR_TYPE  channel
) {
	LSA_UINT32 max_edd_ind = CM_EDD_STATE_IND_RESOURCE_COUNT;
	LSA_UINT32 i;

	for (i = 0; i < max_edd_ind; ++i) {
		CM_EDD_LOWER_RQB_PTR_TYPE eddInd;

		eddInd = cm_edd_alloc_rqb(channel, EDD_SRV_SRT_IND_PROVIDE);

		if (cm_is_null (eddInd)) {
			CM_EDD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "!edd-rqb, max_edd_indication_res(%d)"
				, max_edd_ind
				);

			return LSA_FALSE;
		}

		cm_edd_request_lower(channel, EDD_SRV_SRT_IND_PROVIDE, 0, eddInd);
	}

	return LSA_TRUE;
}

/*===========================================================================*/
#if CM_CFG_USE_ACP
static  LSA_VOID
cm_edd_state_indication_done (
	CM_CHANNEL_PTR_TYPE  channel,
    CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	if(channel->state >= CM_CH_CLOSE)
	{
		CM_ASSERT(
				CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK
				|| CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK_CANCEL
				);
		cm_edd_free_rqb (channel, edd);
	}
	else if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK)
	{
		CM_EDD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "!!undefined response(%d)"
			, CM_EDD_GET_RESPONSE(edd)
			);
		CM_FATAL1 (CM_EDD_GET_RESPONSE(edd));
	}
	else
	{
		EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE edd_ind = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE);
		LSA_UINT16 i;

		for(i = 0; i < edd_ind->Count; i++) {

			EDD_UPPER_CSRT_IND_DATA_PTR_TYPE data = &edd_ind->Data[i];

			cm_acp_frame_indication(data, edd_ind->DebugInfo);
		}

		cm_edd_request_lower(channel, EDD_SRV_SRT_IND_PROVIDE, 0, edd); /* Reprovide */
	}
}
#endif

/*===========================================================================*/
/*=== SENDCLOCK: RANGE-CHECK ================================================*/
/*===========================================================================*/

LSA_BOOL
cm_edd_send_clock_range_ok_hw (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock
	)
{
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

	switch (edd_params->HardwareType & EDD_HW_TYPE_USED_MSK)
	{
	case EDD_HW_TYPE_USED_STANDARD_MAC:
		switch (send_clock)
		{
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
			return LSA_TRUE;

		default:
			break;
		}
		break;

	case EDD_HW_TYPE_USED_ERTEC_400:
	case EDD_HW_TYPE_USED_ERTEC_200:
	case EDD_HW_TYPE_USED_SOC:
	case EDD_HW_TYPE_USED_ERTEC_200P:
	case EDD_HW_TYPE_USED_HERA:
		switch (send_clock)
		{
		case 1:
		case 2:
		case 3:
		case 5:
		case 7:
			/* 1389513: quote "1,2,3,5,7 are disabled until further notice" */
			break;

		default:
			if (send_clock >= 1 && send_clock <= 128)
			{
				return LSA_TRUE;
			}
			break;
		}
		break;

	default:
		CM_FATAL(); /* unknown hardware, update! */
		break;
	}

	CM_EDD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "send_clock_range_ok_hw: send_clock(%u) not allowed", send_clock);

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== EDD SYNC_SENDCLOCK_CHANGE =============================================*/
/*===========================================================================*/

LSA_BOOL
cm_edd_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;
	EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE args;

	/*
	 * note: StandardMAC EDD (>= V3.12) supports this service too
	 */

	edd = cm_edd_get_rqb(channel);

	if( cm_is_null(edd) ) {

		return LSA_FALSE;
	}

	args = CM_EDD_GET_PPARAM(edd, EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE);

	/***/

	if( send_clock == 0 ) {

		CM_ASSERT (channel->state == CM_CH_CLOSE);

		args->CycleBaseFactor = 0; /* special */
	}
	else {

		args->CycleBaseFactor = send_clock;
	}

	cm_edd_request_lower(channel, EDD_SRV_SENDCLOCK_CHANGE, channel, edd);

	return LSA_TRUE; /* pending */
}

/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT
cm_edd_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
) {
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	CM_PORT_UINT mask;

	mask = ((CM_PORT_UINT)2 << edd_params->MaxPortCnt) - 1;

	return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID
cm_edd_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd /* provide: LSA_NULL */
) {
	if( cm_is_null(edd) ) { /* provide */

		CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
		CM_PORT_UINT provide_mask = 0;
		LSA_UINT16 port_id;

		for( port_id = 0; port_id <= edd_params->MaxPortCnt; ++port_id ) {

			LSA_UINT16 max = 2/*!*/;
			LSA_UINT16 cnt;

			for( cnt = 0; cnt < max; ++cnt ) {

				edd = cm_edd_alloc_rqb(channel, EDD_SRV_PRM_INDICATION);

				if( cm_is_null(edd) ) {
					CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot edd-alloc");
					CM_FATAL();
				}

				{
				EDD_UPPER_PRM_INDICATION_PTR_TYPE ind = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_INDICATION_PTR_TYPE);

				ind->edd_port_id = port_id; /* in-paramter! */

				ind->diag_cnt = 0; /* sanity (an out-paramter!) */
				}

				cm_edd_request_lower(channel, EDD_SRV_PRM_INDICATION, 0, edd);
			}

			provide_mask |= ((CM_PORT_UINT)1 << port_id);
		}

		if( provide_mask != cm_edd_prm_provide_mask(channel) ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* handle indication and do re-provide */

		EDD_UPPER_PRM_INDICATION_PTR_TYPE ind = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_INDICATION_PTR_TYPE);

		CM_ASSERT(cm_is_not_null(ind));

		CM_EDD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "edd-prm.IND edd_port_id(%u) diag_cnt(%u)"
			, ind->edd_port_id
			, ind->diag_cnt
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK
				|| CM_EDD_GET_RESPONSE(edd) == EDD_STS_OK_CANCEL
				);

			cm_edd_free_rqb(channel, edd);
		}
		else if( CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( ind->diag_cnt > EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES ) {
				CM_FATAL();
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( cm_is_null(pd_channel) )
			{
				CM_EDD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "edd-prm.IND but no pd-channel, ignored");
			}
			else
			{
				cm_pd_edd_prm_indication_done(pd_channel, ind);
			}
			}

			/***/

			/*
			ind->edd_port_id = unchanged (an in-paramter!)
			*/

			ind->diag_cnt = 0; /* sanity (an out-paramter!) */

			cm_edd_request_lower(channel, EDD_SRV_PRM_INDICATION, 0, edd); /* re-provide */
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_prm_prepare (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_PREPARE_PTR_TYPE  param;

	CM_ASSERT (cm_is_not_null(edd));

	param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_PREPARE_PTR_TYPE);

	param->dummy = 1;

	cm_edd_request_lower (channel, EDD_SRV_PRM_PREPARE, channel, edd);
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_BOOL				is_local,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_WRITE_PTR_TYPE  param;

	CM_ASSERT (cm_is_not_null(edd));

	param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_WRITE_PTR_TYPE);

	param->Local = (is_local) ? EDD_PRM_PARAMS_ASSIGNED_LOCALLY : EDD_PRM_PARAMS_ASSIGNED_REMOTE;
	param->edd_port_id = edd_port_id;
	param->slot_number = slot_number;
	param->subslot_number = subslot_number;
	param->record_index = record_index;
	param->record_data_length = record_data_length;

	/* record_data used as reference, check this memory attr */
	param->record_data = CM_CAST_EDD_MEM_U8_PTR (record_data);

	cm_edd_request_lower (channel, EDD_SRV_PRM_WRITE, channel, edd);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_edd_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_READ_PTR_TYPE  param;

	if (cm_is_null (edd)) {
		return CM_ERR_ALLOC_LOWER;
	}

	param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_READ_PTR_TYPE);

	param->edd_port_id = edd_port_id;
	param->slot_number = slot_number;
	param->subslot_number = subslot_number;
	param->record_index = record_index;
	param->record_data_length = record_data_length;

	/* record_data used as reference, check this */
	param->record_data = CM_CAST_EDD_MEM_U8_PTR (record_data);

	cm_edd_request_lower (channel, EDD_SRV_PRM_READ, channel, edd);
	return CM_OK;
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_prm_end (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL  is_local
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_END_PTR_TYPE  param;

	CM_ASSERT (cm_is_not_null(edd));

	param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_END_PTR_TYPE);

	param->Local = (is_local) ? EDD_PRM_PARAMS_ASSIGNED_LOCALLY : EDD_PRM_PARAMS_ASSIGNED_REMOTE;

#if EDD_PRM_PORT_IS_COMPACT != 0 || EDD_PRM_PORT_IS_MODULAR != 1
#error "EDD_PRM_PORT_IS_COMPACT/EDD_PRM_PORT_IS_MODULAR are not 0/1"
#endif

	cm_pd_prm_set_prm_port_flags (channel, param->IsModularPort
		, sizeof (param->IsModularPort), sizeof (param->IsModularPort[0])
		, CM_PDEV_PROP_TYPE_MODULAR);

	cm_edd_request_lower (channel, EDD_SRV_PRM_END, channel, edd);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_prm_commit (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_COMMIT_PTR_TYPE  param;

	CM_ASSERT (cm_is_not_null(edd));

	param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_COMMIT_PTR_TYPE);

#if EDD_PRM_APPLY_RECORD_PORT_PARAMS != 0 || EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != 1
#error "EDD_PRM_APPLY_RECORD_PORT_PARAMS/EDD_PRM_APPLY_DEFAULT_PORT_PARAMS are not 0/1"
#endif

	cm_pd_prm_set_prm_port_flags (channel, param->ApplyDefaultPortparams
		, sizeof (param->ApplyDefaultPortparams), sizeof (param->ApplyDefaultPortparams[0])
		, CM_PDEV_PROP_DEFAULT_ENABLED
		);

	/* sanity: set out-params to zero */
	param->RsvIntervalRed.RxEndNs = 0;
	param->RsvIntervalRed.RxLocalTransferEndNs = 0;
	param->RsvIntervalRed.TxEndNs = 0;
	param->RsvIntervalRed.TxLocalTransferStartNs = 0;
	param->RsvIntervalRed.TxLocalTransferEndNs = 0;

	cm_edd_request_lower (channel, EDD_SRV_PRM_COMMIT, channel, edd);
}


/*===========================================================================*/
/*=== EDD_SRV_NRT_SET_DCP_HELLO_FILTER ======================================*/
/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS

LSA_UINT16
cm_edd_set_dcp_hello_filter (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8  mode,
	LSA_UINT32  user_id,
	EDD_UPPER_MEM_U8_PTR_TYPE  name_of_station,
	LSA_UINT32 name_of_station_length
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb(channel);

	EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE  sf;

	if( cm_is_null(edd) ) {
		return CM_ERR_ALLOC_LOWER;
	}

	sf = CM_EDD_GET_PPARAM (edd, EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE);

	sf->Mode = mode;
	sf->UserID = user_id;
	sf->pNameOfStation = name_of_station;
	sf->NameOfStationLen = name_of_station_length;

	cm_edd_request_lower(channel, EDD_SRV_NRT_SET_DCP_HELLO_FILTER, channel, edd);

	return CM_OK_PENDING;
}

#endif

/*===========================================================================*/
/*=== GET-LINK-STATUS =======================================================*/
/*===========================================================================*/

LSA_BOOL
cm_edd_get_link_status(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb(channel);

	if( cm_is_null(edd) ) {

		return LSA_FALSE;
	}
	else {

		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_GET_LINK_STATUS_PTR_TYPE);

		param->PortID = EDD_PORT_ID_AUTO;

		cm_edd_request_lower(channel, EDD_SRV_GET_LINK_STATUS, channel, edd);

		return LSA_TRUE;
	}
}

/*===========================================================================*/
/*=== PULL/PLUG =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_edd_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
	)
{
	CM_EDD_LOWER_RQB_PTR_TYPE  edd = cm_edd_get_rqb (channel);

	EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE  param;

	CM_ASSERT (cm_is_not_null(edd));

	param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE);

	param->PortID = channel->usr.pd.pullplug.port->port_id;
	param->ModuleState = submodule_state;

#if EDD_PRM_APPLY_RECORD_PORT_PARAMS != 0 || EDD_PRM_APPLY_DEFAULT_PORT_PARAMS != 1
#error "EDD_PRM_APPLY_RECORD_PORT_PARAMS/EDD_PRM_APPLY_DEFAULT_PORT_PARAMS are not 0/1"
#endif

	param->ApplyDefaultPortparams = apply_default_portparams;

	cm_edd_request_lower (channel, EDD_SRV_PRM_CHANGE_PORT, channel, edd);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_edd_pullplug_change_port_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE  param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_CHANGE_PORT_PTR_TYPE);
	LSA_UINT8  notApplicable = param->PortparamsNotApplicable;

	if (cm_pd_sc_map_response(LSA_COMP_ID_EDD, CM_EDD_GET_RESPONSE (edd)) != CM_OK)
	{
		CM_FATAL1 (CM_EDD_GET_RESPONSE (edd));
	}

	cm_edd_release_rqb (channel, edd);

#if EDD_PRM_PORT_PARAMS_APPLICABLE != 0 || EDD_PRM_PORT_PARAMS_NOT_APPLICABLE != 1
#error "EDD_PRM_PORT_PARAMS_APPLICABLE/EDD_PRM_PORT_PARAMS_NOT_APPLICABLE are not 0/1"
#endif

	cm_pd_pullplug_sm_trigger (channel, CM_PD_SCBF_EDD, notApplicable);
}

/*===========================================================================*/
/*=== EDD REQUEST LOWER =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_edd_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	EDD_SERVICE service,
	LSA_VOID_PTR_TYPE user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE edd
) {
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);

	CM_EDD_SET_USER_ID_PTR (edd, user_id);

	CM_EDD_SET_HANDLES (edd, edd_channel->lower.edd_handle); /* TIA 2709127 */
	CM_EDD_SET_OPCODE (edd, EDD_OPC_REQUEST, service);

	CM_EDD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> edd-rqb(0x%08x) edd-svc(0x%x) edd-userid(0x%x)"
		, edd, CM_EDD_GET_SERVICE (edd)
		, CM_EDD_GET_USER_ID_PTR (edd)
		);

	CM_EDD_REQUEST_LOWER (edd, edd_channel->sysptr);
}

/*===========================================================================*/
/*=== EDD REQUEST LOWER DONE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_edd_request_lower_done (
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	CM_CHANNEL_PTR_TYPE  channel;

	if( cm_is_null(edd) ) {

		CM_EDD_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"cm_is_null(edd)"
			);

		CM_FATAL ();
		return;
	}

	channel = cm_channel_from_handle (CM_EDD_GET_HANDLE(edd));

	if( cm_is_null(channel) ) {

		CM_EDD_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"cm_is_null(channel), edd-svc(0x%x)",
			CM_EDD_GET_SERVICE (edd)
			);

		CM_FATAL ();
		return;
	}

	/***/

	switch (CM_EDD_GET_RESPONSE (edd)) {
	case EDD_STS_OK:
	case EDD_STS_OK_CANCEL:
		CM_EDD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< edd-rqb(0x%08x) edd-svc(0x%x) edd-rsp(%u) edd-userid(0x%x)"
			, edd, CM_EDD_GET_SERVICE (edd), CM_EDD_GET_RESPONSE (edd)
			, CM_EDD_GET_USER_ID_PTR (edd)
			);
		break;

	case EDD_STS_ERR_PRM_INDEX:
		CM_EDD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "<<< edd-rqb(0x%08x) edd-svc(0x%x) edd-rsp(%u) edd-userid(0x%x)"
			, edd, CM_EDD_GET_SERVICE (edd), CM_EDD_GET_RESPONSE (edd)
			, CM_EDD_GET_USER_ID_PTR (edd)
			);
		break;

	default:
		CM_EDD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< edd-rqb(0x%08x) edd-svc(0x%x) edd-rsp(%u) edd-userid(0x%x)"
			, edd, CM_EDD_GET_SERVICE (edd), CM_EDD_GET_RESPONSE (edd)
			, CM_EDD_GET_USER_ID_PTR (edd)
			);
		break;
	}

	/***/

	if( CM_EDD_GET_OPCODE(edd) == EDD_OPC_REQUEST ) { /* fast path */

		LSA_VOID_PTR_TYPE user_id = (LSA_VOID_PTR_TYPE)CM_EDD_GET_USER_ID_PTR(edd); /* 0 or "channel" or "iocr" or "iter", see callers of cm_edd_request_lower() */

		switch( CM_EDD_GET_SERVICE(edd) ) {

		/* CRT-Services (0x2001..0x20FF) */

#if CM_CFG_USE_ACP
		case EDD_SRV_SRT_PROVIDER_ADD:
		case EDD_SRV_SRT_CONSUMER_ADD:
			cm_acp_frame_add_done (user_id, edd);
			break;

		case EDD_SRV_SRT_PROVIDER_REMOVE:
		case EDD_SRV_SRT_CONSUMER_REMOVE:
			cm_acp_frame_remove_done (user_id, edd);
			break;

		case EDD_SRV_SRT_PROVIDER_CONTROL:
		case EDD_SRV_SRT_CONSUMER_CONTROL:
			cm_acp_frame_control_done (user_id, edd);
			break;

		case EDD_SRV_SRT_PROVIDER_SET_STATE:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			switch (channel->path_type) {

#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				cm_cl_acp_global_provider_control_done (channel, edd);
				break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
			case CM_PATH_TYPE_MULTICAST:
				cm_mc_acp_global_provider_control_done (channel, edd);
				break;
#endif
#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				cm_sv_acp_global_provider_control_done (channel, edd);
				break;
#endif
			default:
				CM_FATAL(); /* a bug */
				break;
			}
			break;

#if CM_CFG_MAX_SERVERS
		case EDD_SRV_SRT_GET_APDU_STATUS:
			cm_acp_frame_get_apdu_status_done (channel, user_id, edd);
			break;
#endif

		case EDD_SRV_SRT_IND_PROVIDE:
			cm_edd_state_indication_done (channel, edd);
			break;
#endif

		/* GENERAL-Services (0x2300..0x23FF) */

		case EDD_SRV_GET_PARAMS:
			cm_edd_channel_info_done (channel, edd);
			break;

		case EDD_SRV_GET_LINK_STATUS:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_link_status_done (channel, edd);
			break;

		case EDD_SRV_LINK_STATUS_IND_PROVIDE:
		case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
			cm_edd_link_indication (channel, edd);
			break;

		case EDD_SRV_GET_PORT_PARAMS:
			cm_pd_get_port_params_done (channel, edd);
			break;

		case EDD_SRV_SENDCLOCK_CHANGE:
			if( channel == (CM_CHANNEL_PTR_TYPE)user_id ) {

				CM_ASSERT (channel->path_type == CM_PATH_TYPE_EDD);

				if( CM_EDD_GET_RESPONSE (edd) != EDD_STS_OK ) { /* "set-send-clock 0" before closing the edd-channel */

					CM_FATAL();
				}
				else {

					cm_edd_release_rqb (channel, edd);

					CM_EDD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "h(%u) set-sendclock(0) done"
						, channel->my_handle
						);

					cm_edd_channel_close (channel);
				}
			}
			else {

				LSA_UINT16 rsp = CM_EDD_GET_RESPONSE (edd);
				EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE);
				LSA_UINT16 send_clock = param->CycleBaseFactor;

				channel = (CM_CHANNEL_PTR_TYPE)user_id;

				CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

				cm_edd_release_rqb (channel, edd);

				cm_pd_set_send_clock_done (channel, rsp, send_clock);
			}
			break;

		/* PRM-Services (0x2700..0x27FF) */

		case EDD_SRV_PRM_INDICATION:
			cm_edd_prm_indication (channel, edd);
			break;

		case EDD_SRV_PRM_PREPARE:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_prepare_done (channel, edd);
			break;

		case EDD_SRV_PRM_WRITE:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_write_done (channel, edd);
			break;

		case EDD_SRV_PRM_END:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_end_done (channel, edd);
			break;

		case EDD_SRV_PRM_COMMIT:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_commit_done (channel, edd);
			break;

		case EDD_SRV_PRM_READ:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_pd_edd_prm_read_done (channel, edd);
			break;

		case EDD_SRV_PRM_CHANGE_PORT:
			channel = (CM_CHANNEL_PTR_TYPE)user_id;
			cm_edd_pullplug_change_port_done (channel, edd);
			break;

		/* NRT-Filter services */

#if CM_CFG_MAX_CLIENTS
		case EDD_SRV_NRT_SET_DCP_HELLO_FILTER:
			{
			EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE  sf;

			sf = CM_EDD_GET_PPARAM (edd, EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE);

			channel = (CM_CHANNEL_PTR_TYPE)user_id;

			cm_cl_edd_set_dcp_hello_filter_done (channel, CM_EDD_GET_RESPONSE (edd), sf);

			cm_edd_release_rqb (channel, edd);
			}
			break;
#endif

		default:
			CM_EDD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled service(%u)",
				CM_EDD_GET_SERVICE (edd)
				);
			CM_FATAL ();
			break;
		}
	}
	else {

		switch (CM_EDD_GET_OPCODE (edd)) {

		case EDD_OPC_OPEN_CHANNEL:
			cm_edd_channel_open_done (channel, edd);
			break;

		case EDD_OPC_CLOSE_CHANNEL:
			cm_edd_channel_close_done (channel, edd);
			break;

		default:
			CM_EDD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_EDD_GET_OPCODE (edd)
				);
			CM_FATAL ();
			break;
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
