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
/*  F i l e               &F: cm_mc1.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  multicast things                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	81
#define CM_MODULE_ID		81

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_MULTICAST_CHANNELS


static LSA_VOID
cm_mc_create (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

static LSA_VOID
cm_mc_acp_global_provider_control (
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd,
	LSA_UINT16  apdu_tag
);

static LSA_VOID
cm_mc_delete (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

static LSA_VOID
cm_mc_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_request (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	if( channel->state != CM_MC_CREATED )
	{
		switch( CM_RQB_GET_OPCODE(rb) )
		{
		case CM_OPC_MC_CREATE:
			/* channel-state will be checked in cm_mc_create() */
			break;

		default:
			CM_MC_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "opc(%u) channel-state(%u) not CM_MC_CREATED"
				, CM_RQB_GET_OPCODE(rb), channel->state
				);
			cm_callback(channel, CM_ERR_SEQUENCE, &rb); /* CM-MC not created */
			return;
		}
	}

	if (cm_is_null(rb->args.void_ptr)) /* args must be there always - with some exceptions */
	{
		switch( CM_RQB_GET_OPCODE(rb) )
		{
		case CM_OPC_MC_DELETE:
			break;

		default:
			CM_MC_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "no args");
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}
	}

	/***/

	switch( CM_RQB_GET_OPCODE(rb) )
	{
	case CM_OPC_MC_CREATE:
		cm_mc_create (channel, rb);
		return;
	case CM_OPC_MC_DELETE:
		cm_mc_delete (channel, rb);
		return;
	case CM_OPC_MC_CONTROL:
		cm_mc_control (channel, rb);
		return;

	case CM_OPC_MC_DEVICE_ADD:
		cm_mc_device_add (channel, rb);
		return;
	case CM_OPC_MC_DEVICE_REMOVE:
		cm_mc_device_remove (channel, rb);
		return;
	case CM_OPC_MC_DEVICE_CONTROL:
		cm_mc_device_control (channel, rb);
		return;
	case CM_OPC_MC_DEVICE_PROVIDE_EVENT:
		cm_mc_opc_device_provide_event (channel, rb);
		return;

	case CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP:
		cm_mc_ar_addr_info_rsp (channel, rb);
		return;
	case CM_OPC_MC_EVENT_AR_IN_DATA_RSP:
		cm_mc_ar_in_data_rsp (channel, rb);
		return;
	case CM_OPC_MC_EVENT_AR_OFFLINE_RSP:
		cm_mc_ar_offline_rsp (channel, rb);
		return;
	case CM_OPC_MC_EVENT_APDU_STATUS_RSP:
		cm_mc_apdu_status_rsp (channel, rb);
		return;

	default:
		CM_MC_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unhandled opcode(%u), path-type(%u)"
			, CM_RQB_GET_OPCODE(rb), channel->path_type
			);

		CM_FATAL();
		/*lint -unreachable */
		return;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_mc_channel_init (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_CONST_PTR_TYPE  rb
	)
{
	channel->usr.mc.upper = LSA_NULL;
	channel->usr.mc.upper_apdu = LSA_NULL;

	channel->usr.mc.dev_arr = LSA_NULL;
	channel->usr.mc.dev_cnt = 0;

	channel->usr.mc.wait_for = 0;

	channel->usr.mc.edd_rqb = LSA_NULL;

	/***/

	if (! (1 <= rb->args.mc.create->max_devices  &&  rb->args.mc.create->max_devices <= CM_MC_MAX_DEVICE_NR))
	{
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "!(1 <= max_devices(%u) <= CM_MC_MAX_DEVICE_NR(%u))"
			, rb->args.mc.create->max_devices, CM_MC_MAX_DEVICE_NR
			);
		return CM_ERR_PARAM;
	}

	{
	LSA_UINT32 length = sizeof (channel->usr.mc.dev_arr[0]) * rb->args.mc.create->max_devices;
	CM_LOCAL_MEM_PTR_TYPE mem;

	CM_ALLOC_LOCAL_MEM (&mem, length);
	if (cm_is_null (mem)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "can't alloc local mem, length(%u)"
			, length
			);
		return CM_ERR_ALLOC_LOCAL;
	}

	channel->usr.mc.dev_arr = (CM_MC_DEVICE_PTR_TYPE *)mem;
	channel->usr.mc.dev_cnt = rb->args.mc.create->max_devices;
	}

	{
	LSA_UINT16  idev;

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev) {
		channel->usr.mc.dev_arr[idev] = LSA_NULL;
	}
	}

	/***/

	channel->usr.mc.edd_rqb = cm_edd_alloc_rqb (channel, EDD_SRV_SRT_PROVIDER_SET_STATE);

	if (cm_is_null (channel->usr.mc.edd_rqb)) {
		CM_MC_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "can't alloc edd-rqb"
			);
		return CM_ERR_ALLOC_LOCAL;
	}

	/***/

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_channel_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	if (channel->state != CM_MC_DELETING) {
		CM_FATAL(); /* see caller */
	}

	if (cm_is_not_null (channel->usr.mc.dev_arr)) {

		{
		LSA_UINT16  rc;
		CM_FREE_LOCAL_MEM (& rc, channel->usr.mc.dev_arr);
		if (rc != LSA_RET_OK) {
			CM_FATAL1 (rc);
			return;
		}
		}

		channel->usr.mc.dev_arr = LSA_NULL;
		channel->usr.mc.dev_cnt = 0;
	}

	CM_ASSERT(cm_is_null(channel->usr.mc.upper));

	CM_ASSERT(cm_is_null(channel->usr.mc.upper_apdu));

	if (cm_is_not_null (channel->usr.mc.edd_rqb)) {
		cm_edd_free_rqb (channel, channel->usr.mc.edd_rqb);
		channel->usr.mc.edd_rqb = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_create (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
		/* sequence check */
	if (channel->state != CM_CH_READY)
	{
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel(%d), wrong state(%d)"
			, channel->my_handle, channel->state
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if( ! cm_pdusr_is_ready(channel) )
	{
		CM_MC_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD not ready (PD::CREATE not run?)"
			);
		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "connected to pd(%u)"
		, cm_peer_channel_get(channel, CM_PATH_TYPE_PD)->my_handle
		);

	/***/

	{
	LSA_UINT16 rc;
	rc = cm_mc_channel_init (channel, rb);
	if (rc != CM_OK) {
		cm_callback (channel, rc, &rb);
		return;
	}
	}

		/* get channel infos (TIA 201512) */
	{
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	rb->args.mc.create->hardware_type = edd_params->HardwareType;
	rb->args.mc.create->interface_id  = edd_params->InterfaceID;
	}

		/* keep needed things */
	channel->usr.mc.upper = rb; /* keep */

		/* transition CM-CH::READY --> CM-MC::GLOBAL_CTRL */
	channel->state = CM_MC_GLOBAL_CTRL;

	cm_mc_acp_global_provider_control (channel, channel->usr.mc.edd_rqb, CM_CL_CONTROL_APDU_STATUS_RUN);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_acp_global_provider_control (
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd,
	LSA_UINT16  apdu_status
	)
{
	EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE global_prov;

	CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "channel(%d) state(%d) apdu_status(0x%x)"
		, channel->my_handle, channel->state, apdu_status
		);

	global_prov = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE);

	global_prov->GroupID = CM_PROVIDER_GROUP_IOM;
	global_prov->Status  = 0;
	global_prov->Mask    = 0;

	if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_STOP)) {
		global_prov->Mask |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}
	else if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_RUN)) {
		global_prov->Status |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
		global_prov->Mask	|= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}

	// apdu_status for station is now per device. See CM_CL_AR_CONTROL_STATION_OK and CM_CL_AR_CONTROL_STATION_PROBLEM

	cm_edd_request_lower (channel, EDD_SRV_SRT_PROVIDER_SET_STATE, channel, edd);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_acp_global_provider_control_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_CONST_PTR_TYPE  edd
	)
{
	if (
		CM_EDD_GET_RESPONSE (edd) != EDD_STS_OK
		|| edd != channel->usr.mc.edd_rqb
	) {
		CM_FATAL(); /* a bug */
	}

	if (channel->state == CM_MC_GLOBAL_CTRL) {

		if(
			cm_is_null(channel->usr.mc.upper)
			|| CM_RQB_GET_OPCODE(channel->usr.mc.upper) != CM_OPC_MC_CREATE
		) {
			CM_FATAL(); /* a bug */
		}

			/* transition CM-MC::GLOBAL_CTRL --> CM-MC::CREATED */
		channel->state = CM_MC_CREATED;
		cm_callback (channel, CM_OK, & channel->usr.mc.upper);
	}
	else {

		if(
			cm_is_null(channel->usr.mc.upper_apdu)
			|| CM_RQB_GET_OPCODE(channel->usr.mc.upper_apdu) != CM_OPC_MC_CONTROL
		) {
			CM_FATAL(); /* a bug */
		}

		cm_callback (channel, CM_OK, & channel->usr.mc.upper_apdu);

			/* complete transition CM-MC::DELETING --> CM-CH::READY, see cm_mc_delete() */
		if (cm_is_not_null (channel->usr.mc.upper)) {

			CM_UPPER_RQB_PTR_TYPE rb = channel->usr.mc.upper;
			channel->usr.mc.upper = LSA_NULL;

			cm_mc_channel_undo_init(channel);

			channel->state = CM_CH_READY;

			CM_ASSERT (CM_RQB_GET_OPCODE(rb) == CM_OPC_MC_DELETE);
			cm_callback (channel, CM_OK, &rb);
		}
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_delete (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	if (cm_is_not_null (channel->usr.mc.upper)) {
		cm_callback (channel, CM_ERR_SEQUENCE, & rb); /* unreachable */
		return;
	}

		/* is there one device ? */
	{
	LSA_UINT16  idev;
	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev) {

		CM_MC_DEVICE_PTR_TYPE  dev = channel->usr.mc.dev_arr[idev];

		if (cm_is_not_null (dev)) {
			CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) in use, idev(%u)"
				, dev->device_nr, idev
				);
			cm_callback (channel, CM_ERR_SEQUENCE, & rb);
			return;
		}
	}
	}

	if (channel->usr.mc.wait_for != 0) {
		CM_FATAL(); /* unreachable (because no device) */
	}

		/* transition CM-MC::CREATED --> CM-MC::DELETING */
	channel->state = CM_MC_DELETING;

		/* wait if one global_control is runnning */
	if (cm_is_not_null (channel->usr.mc.upper_apdu)) {

			/* keep needed things */
		channel->usr.mc.upper = rb; /* keep */

			/* complete in cm_mc_acp_global_provider_control_done() */
		return;
	}

	cm_mc_channel_undo_init(channel);

		/* transition CM-MC::DELETING --> CM-CH::READY */
	channel->state = CM_CH_READY;

	cm_callback (channel, CM_OK, & rb);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "channel(%d) control_tag(%d) apdu_status(0x%x)"
		, channel->my_handle
		, rb->args.mc.control->control_tag
		, channel->usr.mc.upper_apdu
		);

	switch (rb->args.mc.control->control_tag) {
	case CM_CL_CONTROL_PASSIVATE:
	case CM_CL_CONTROL_ACTIVATE:
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "channel(%d) !control_tag(%d)"
			, channel->my_handle, rb->args.mc.control->control_tag
			);
		cm_callback (channel, CM_ERR_PARAM, & rb); /* CM_ERR_NOT_YET */
		return;

	case CM_CL_CONTROL_APDU_STATUS:

		if (cm_is_not_null (channel->usr.mc.upper_apdu)) {
			CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "channel(%d), another apdu pending, rqb(0x%x)",
					channel->my_handle, channel->usr.mc.upper_apdu
				);
			cm_callback (channel, CM_ERR_SEQUENCE, & rb);
			return;
		}

		channel->usr.mc.upper_apdu = rb; /* keep */

		cm_mc_acp_global_provider_control (channel, channel->usr.mc.edd_rqb, rb->args.mc.control->apdu_status);
		return;

	default:
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "channel(%d) !control_tag(%d)"
			, channel->my_handle, rb->args.mc.control->control_tag
			);
		cm_callback (channel , CM_ERR_PARAM, & rb);
		return;
	}
}


/*------------------------------------------------------------------------------
// handle irt-port transition to "run"
//----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_device_any_port_irt_run(
	CM_CHANNEL_CONST_PTR_TYPE channel
) {
	CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
		"h(%d): channel.state(%d) any port reached irt-run",
		channel->my_handle, channel->state
	);

	if( channel->state == CM_MC_CREATED ) {

		LSA_UINT16 idev;

		for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev) {

			CM_MC_DEVICE_PTR_TYPE dev = channel->usr.mc.dev_arr[idev];

			if ( cm_is_not_null(dev) ) {

				if ( cm_is_not_null(dev->mcons) ) {

					if ((dev->wait_for & CM_MWF__WF_IRT_RUN) != 0) {

						dev->wait_for &= ~CM_MWF__WF_IRT_RUN;

						dev->wait_for |= CM_MWF__DO_RESOLVE;

						cm_mc_device_sm(dev);
					}
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
// handle RtfNosIpChanged
//----------------------------------------------------------------------------*/

LSA_BOOL
cm_mc_pdev_set_down_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
	)
{
	LSA_BOOL    all_reset = LSA_TRUE;
	LSA_UINT16  idev;

	if (channel->state != CM_MC_CREATED) /* too late */
	{
		return  LSA_TRUE;
	}

	if( (channel->usr.mc.wait_for & CM_MC_CWF__RESET_TO_FACTORY_CNF) != 0 ) {
		CM_FATAL(); /* a bug (CM-PD did not wait) */
	}

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev)
	{
		CM_MC_DEVICE_PTR_TYPE  dev = channel->usr.mc.dev_arr[idev];

		if (cm_is_not_null (dev)) {

			if (reason != CM_AR_REASON_DCP_RESET) {
				/* do nothing (note: is not a MasterProject, see cm_mc_device_add()) */
			}
			else if( (dev->wait_for & CM_MWF__IS_RESET_TO_FACTORY) != 0 ) {
				/* do nothing (already RTF) */
			}
			else {
				dev->wait_for |= CM_MWF__IS_RESET_TO_FACTORY;
				dev->wait_for |= CM_MWF__DO_FRAME_RTF_REQ;

				all_reset = LSA_FALSE;

				if( cm_ar_com_is_down(dev->ar_graph) ) {
					cm_mc_device_sm(dev);
				}
				else {
					cm_ar_com_down_debug_data_fmt(dev->ar_graph, CM_MODULE_ID, __LINE__, reason, 0);
					cm_ar_com_down(dev->ar_graph, reason);
				}
			}
		}
	}

	if (all_reset) {
		return LSA_TRUE; /* ready for pdev-rtf */
	}
	else {
		channel->usr.mc.wait_for |= CM_MC_CWF__RESET_TO_FACTORY_CNF;
		return LSA_FALSE; /* not all down */
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_pdev_set_down_check (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	LSA_UINT16  idev;

	if( (channel->usr.mc.wait_for & CM_MC_CWF__RESET_TO_FACTORY_CNF) == 0 ) {
		CM_FATAL();
	}

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev)
	{
		CM_MC_DEVICE_PTR_TYPE  dev = channel->usr.mc.dev_arr[idev];

		if (cm_is_not_null (dev)) {

			if ((dev->wait_for & (CM_MWF__DO_FRAME_RTF_REQ | CM_MWF__WF_FRAME_RTF_CNF)) != 0) {

				CM_MC_TRACE_03 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE,
					"dev(%d) state(%d) wait_for(0x%x) rtf running, must wait!",
					dev->device_nr, dev->state, dev->wait_for
					);
				return;
			}
		}
	}

	channel->usr.mc.wait_for &= ~CM_MC_CWF__RESET_TO_FACTORY_CNF;

	{
	CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_PD);

	cm_pd_rtf_nos_ip_changed_done (pd_channel, CM_PD_RTF_WF__MC_DOWN);
	}
}


/*----------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling multicast channel part (CM_CFG_MAX_MULTICAST_CHANNELS=" CM_STRINGIFY(CM_CFG_MAX_MULTICAST_CHANNELS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
