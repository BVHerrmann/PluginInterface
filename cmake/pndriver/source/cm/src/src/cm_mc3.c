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
/*  F i l e               &F: cm_mc3.c                                  :F&  */
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
#define LTRC_ACT_MODUL_ID	83
#define CM_MODULE_ID		83

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_MULTICAST_CHANNELS

/*------------------------------------------------------------------------------
//	helper
//----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_dev_event_provide (
	CM_MC_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
){
	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_MC_DEVICE_PROVIDE_EVENT);
	CmListInsertTail (& dev->event_q, (CM_LIST_ENTRY_PTR_TYPE)rb);
}

/*----------------------------------------------------------------------------*/
CM_UPPER_RQB_PTR_TYPE
cm_mc_dev_event_get (
	CM_MC_DEVICE_CONST_PTR_TYPE dev
){
	CM_UPPER_RQB_PTR_TYPE rb = LSA_NULL;
	if (! CmListIsEmpty(& dev->event_q) ) {
		CmListRemoveHead(& dev->event_q, rb, CM_UPPER_RQB_PTR_TYPE);
	}
	return rb;
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_DEVICE_PROVIDE_EVENT
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_opc_device_provide_event (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
){
	CM_UPPER_EVENT_PTR_TYPE  event = rb->args.mc.event_ind;
	CM_MC_DEVICE_PTR_TYPE dev;

	dev = cm_mc_dev_from_device_nr (channel, event->device_nr);

	if (cm_is_null (dev))
	{
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (dev->state != CM_MST_PASSIVE)
	{
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	cm_mc_dev_event_provide (dev, rb);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_addr_info_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_EVENT_PTR_TYPE  event = rb->args.mc.event_ind;
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, event->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "dev(%d) not found", event->device_nr);
		CM_FATAL();
		return;
	}

	CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%d): dev(%d) state(%d) wait_for(0x%x)"
		, channel->my_handle, dev->device_nr, dev->state, dev->wait_for
		);

	if (dev->wait_for & CM_MWF__ADDR_INFO_RSP) {
		dev->wait_for &= ~CM_MWF__ADDR_INFO_RSP;
	}
	else {
		CM_FATAL();
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_MC_DEVICE_PROVIDE_EVENT);
	cm_mc_dev_event_provide (dev, rb);

		/* kick state-machine */
	cm_mc_device_sm (dev);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_EVENT_AR_IN_DATA_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_in_data_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_EVENT_PTR_TYPE  event = rb->args.mc.event_ind;
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, event->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "dev(%d) not found", event->device_nr);
		CM_FATAL();
		return;
	}

	CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%d): dev(%d) state(%d) wait_for(0x%x)"
		, channel->my_handle, dev->device_nr, dev->state, dev->wait_for
		);

	if (dev->wait_for & CM_MWF__IN_DATA_RSP) {
		dev->wait_for &= ~CM_MWF__IN_DATA_RSP;
	}
	else {
		CM_FATAL();
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_MC_DEVICE_PROVIDE_EVENT);
	cm_mc_dev_event_provide (dev, rb);

		/* kick state-machine */
	cm_mc_device_sm (dev);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_EVENT_AR_OFFLINE_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_offline_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_EVENT_PTR_TYPE  event = rb->args.mc.event_ind;
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, event->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "dev(%d) not found", event->device_nr);
		CM_FATAL();
		return;
	}

	CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%d): dev(%d) state(%d) wait_for(0x%x)"
		, channel->my_handle, dev->device_nr, dev->state, dev->wait_for
		);

	if (dev->wait_for & CM_MWF__OFFLINE_RSP) {
		dev->wait_for &= ~CM_MWF__OFFLINE_RSP;
	}
	else {
		CM_FATAL();
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_MC_DEVICE_PROVIDE_EVENT);
	cm_mc_dev_event_provide (dev, rb);

		/* kick state-machine */
	cm_mc_device_sm (dev);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_EVENT_APDU_STATUS_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_apdu_status_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_EVENT_PTR_TYPE  event = rb->args.mc.event_ind;
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, event->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "dev(%d) not found", event->device_nr);
		CM_FATAL();
		return;
	}

	CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%d): dev(%d) state(%d) wait_for(0x%x)"
		, channel->my_handle, dev->device_nr, dev->state, dev->wait_for
		);

	if (dev->wait_for & CM_MWF__APDU_STATUS_RSP) {
		dev->wait_for &= ~CM_MWF__APDU_STATUS_RSP;
	}
	else {
		CM_FATAL();
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_MC_DEVICE_PROVIDE_EVENT);
	cm_mc_dev_event_provide (dev, rb);

	if (cm_is_null(dev->mcons)) {
		CM_FATAL();
	}
	else if( cm_ar_com_is_down(dev->ar_graph) ) {
		cm_mc_device_sm (dev);
	}
	else {
		/* check delayed apdu_ind */
		cm_mc_ar_mcons_data_status_check (dev->ar_graph, dev->mcons);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_mcons_data_status_check (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE  iocr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	if( cm_ar_com_is_down(argr) ) {
		/* late */
	}
	else if ((dev->wait_for & CM_MWF__IS_IN_DATA) == 0) {
		/* early */
	}
	else if ((dev->wait_for & CM_MWF__APDU_STATUS_RSP) != 0) {
		/* defer */
	}
	else {

		LSA_UINT16  apdu_delta = CM_CAST_U16((~ argr->ar_com.user_apdu_status) & iocr->cm.cons_data_status);

		CM_MC_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) data-status: old(0x%x) new(0x%x) diff(0x%x)"
			, dev->device_nr
			, argr->ar_com.user_apdu_status
			, iocr->cm.cons_data_status
			, apdu_delta
			);

		if (apdu_delta != 0) {

			CM_UPPER_RQB_PTR_TYPE  upper = cm_mc_dev_event_get (dev);

			if (cm_is_null (upper)) {
				CM_FATAL(); /* a bug */
			}
			else {

				CM_UPPER_EVENT_PTR_TYPE  event = upper->args.mc.event_ind;

				CM_RQB_SET_OPCODE (upper, CM_OPC_MC_EVENT_APDU_STATUS_IND);

				event->device_nr = dev->device_nr;
				event->ar_nr = argr->ar_nr;
				event->session_key = argr->session_key;
				event->u.mc.apdu_status = apdu_delta;

				dev->wait_for |= CM_MWF__APDU_STATUS_RSP;

				cm_callback (dev->channel, CM_OK, & upper);

				/***/

				argr->ar_com.user_apdu_status = iocr->cm.cons_data_status; /* told */
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_ident(
	CM_MC_DEVICE_PTR_TYPE  dev
) {
	CM_AR_GRAPH_MCR_PTR_TYPE  ar_mcr;

	CM_MC_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)", dev->device_nr
		);

	/***/

	ar_mcr = cm_ar_graph_find_mcr(dev->ar_graph, dev->mcons->iocr_ref);

	if(
		cm_is_null(ar_mcr) /* should not happen, see consistency checks */
		|| cm_is_null(dev->nare_ident)
		|| (dev->wait_for & CM_MWF__NARE_CNF) != 0
	) {
		CM_FATAL();
	}

	/***/

	if( (ar_mcr->mcr_properties & CM_MCR_PROP_PROTOCOL_MASK) != CM_MCR_PROP_PROTOCOL_DCP ) {
		/* unreachable, see cm_ar_graph_insert_mcr() */
	}

	dev->wait_for |= CM_MWF__NARE_CNF;
	if (!cm_nare_ident (dev->channel, ar_mcr->provider_name_length, ar_mcr->provider_name, 0 /* timeout */
		, dev->device_nr, 0 /* ar_nr */, 0 /* notify_id */
		, dev /* cancel_request_id */, LSA_NULL /* no alias list */, dev->nare_ident))
	{
		CM_FATAL ();
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_ident_done(
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
) {
	CM_MC_DEVICE_PTR_TYPE dev;

	dev = cm_mc_dev_from_device_nr(channel, device_nr);

	if(
		cm_is_null(dev)
		|| cm_is_null(dev->mcons)
		|| nare != dev->nare_ident
		|| (dev->wait_for & CM_MWF__NARE_CNF) == 0
		|| (dev->wait_for & CM_MWF__DO_PRE_DATA) != 0
	) {
		CM_FATAL();
	}

	dev->wait_for &= ~CM_MWF__NARE_CNF;

	/***/

	if(
		cm_ar_com_is_down(dev->ar_graph)
		|| (dev->wait_for & CM_MWF__NARE_CANCEL) != 0
	) {

		cm_mc_device_sm(dev);
	}
	else if( CM_NARE_GET_RESPONSE(nare) != NARE_RSP_OK ) {

		CM_MC_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) resolve failed, nare-rsp(%u) "
			, dev->device_nr
			, CM_NARE_GET_RESPONSE(nare)
			);

		cm_ar_com_down_debug_data_fmt(dev->ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_NARE, 0);
		cm_ar_com_down(dev->ar_graph, CM_AR_REASON_NARE);
	}
	else { /* cnf(+) */

		CM_MAC_TYPE mac;
		LSA_UINT32  ip;

		CM_ASSERT (sizeof (nare->args.Ident.MACAddr) == sizeof (mac.mac));

		CM_MEMCPY (mac.mac, nare->args.Ident.MACAddr, sizeof (mac.mac));

		CM_ASSERT (sizeof (nare->args.Ident.IPAddr) == sizeof (ip));

		CM_MEMCPY (&ip, nare->args.Ident.IPAddr, sizeof (ip));

		CM_MC_TRACE_08(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) resolved mac(%02x-%02x-%02x-%02x-%02x-%02x) ip(%#08x)"
			, dev->device_nr
			, mac.mac[0], mac.mac[1], mac.mac[2]
			, mac.mac[3], mac.mac[4], mac.mac[5]
			, CM_NTOH32(ip)
			);

		/***/

		dev->ar_graph->ar.eth_mac = mac;
		dev->ar_graph->ar.host_ip = ip;

		dev->wait_for |= CM_MWF__DO_PRE_DATA;

		cm_mc_device_sm(dev);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_pause(
	CM_MC_DEVICE_PTR_TYPE  dev
) {
	CM_AR_GRAPH_MCR_PTR_TYPE  ar_mcr;
	LSA_UINT16 pause_msec;

	ar_mcr = cm_ar_graph_find_mcr(dev->ar_graph, dev->mcons->iocr_ref);

	if(
		cm_is_null(ar_mcr) /* should not happen, see consistency checks */
		|| (dev->wait_for & CM_MWF__NARE_PAUSE) != 0
	) {
		CM_FATAL();
	}

	pause_msec = CM_CAST_U16(CM_MCR_PROP_GET_FACTOR_1SEC(ar_mcr->mcr_properties) * 1000/*msec*/);

	CM_MC_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) pausing for %u msec"
		, dev->device_nr
		, pause_msec
	);

	dev->wait_for |= CM_MWF__NARE_PAUSE;

	cm_oneshot_trigger(&dev->mcons_timer, pause_msec);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
) {
	CM_MC_DEVICE_PTR_TYPE  dev = (CM_MC_DEVICE_PTR_TYPE)user_ptr;

	CM_ASSERT(CM_CONTAINING_STRUCT(oneshot, CM_MC_DEVICE_PTR_TYPE, mcons_timer) == dev);
	LSA_UNUSED_ARG(oneshot);

	CM_MC_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)", dev->device_nr
		);

	/***/

	if( (dev->wait_for & CM_MWF__NARE_PAUSE) == 0 ) {
		CM_FATAL();
	}

	dev->wait_for &= ~CM_MWF__NARE_PAUSE;

	/***/

	if( cm_ar_com_is_down(dev->ar_graph) ) { /* note: always false */
		cm_mc_device_sm(dev);
	}
	else {
		dev->wait_for |= CM_MWF__DO_START;
		cm_mc_device_sm(dev);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_ident_cancel(
	CM_MC_DEVICE_PTR_TYPE dev
){
	if(
		(dev->wait_for & CM_MWF__NARE_CNF) == 0
		|| (dev->wait_for & CM_MWF__NARE_PAUSE) != 0
		|| (dev->wait_for & CM_MWF__NARE_CANCEL) != 0
		|| cm_is_null (dev->nare_cancel)
	) {
		CM_FATAL();
	}

	/***/

	CM_MC_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) cancelling nare-request"
		, dev->device_nr
		);

	dev->wait_for |= CM_MWF__NARE_CANCEL;

	if( ! cm_nare_cancel(dev->channel, dev->device_nr, 0, dev, dev->nare_cancel) ) {
		CM_FATAL();
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_mcons_ident_cancel_done(
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	CM_NARE_LOWER_RQB_CONST_PTR_TYPE nare
) {
	CM_MC_DEVICE_PTR_TYPE dev;

	dev = cm_mc_dev_from_device_nr(channel, device_nr);

	if(
		cm_is_null(dev)
		|| cm_is_null(dev->mcons)
		|| nare != dev->nare_cancel
		|| (dev->wait_for & CM_MWF__NARE_CNF) != 0 /* see NARE documentation */
		|| (dev->wait_for & CM_MWF__NARE_CANCEL) == 0
	) {
		CM_FATAL();
	}

	dev->wait_for &= ~CM_MWF__NARE_CANCEL;

	/***/

	CM_MC_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u) nare-cancelled, rsp(%u)"
		, dev->device_nr, CM_NARE_GET_RESPONSE(nare)
		);

	cm_mc_device_sm(dev);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_mcons_activate_done(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_MC_DEVICE_PTR_TYPE dev = argr->cm.mc.dev;

	CM_MC_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) activate+set2unk done, waiting for up"
		, dev->device_nr
		);

	if( (dev->wait_for & CM_MWF__FRAMES_ACT_CNF) == 0 ) {
		CM_FATAL();
	}

	dev->wait_for &= ~CM_MWF__FRAMES_ACT_CNF;

	if( cm_ar_com_is_down(argr) ) {
		cm_mc_device_sm(dev);
	}
	else {
		if( dev->mcons->cm.cr_state != CM_AR_CR_WF_UP ) {
			CM_FATAL();
		}

		/* note: ar_com.user_apdu_status is reinitialized in cm_ar_com_idle() */

		/* waiting for cm_mc_ar_mcons_up() */
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_mcons_up (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	CM_MC_TRACE_03 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%d) state(%d) wait_for(0x%x)"
		, dev->device_nr, dev->state, dev->wait_for
		);

	if( cm_ar_com_is_down(argr) ) {
		/* late */
	}
	else if( (dev->wait_for & CM_MWF__DO_IN_DATA) != 0 ) {
		CM_FATAL();
	}
	else {
		dev->wait_for |= CM_MWF__DO_IN_DATA;
		cm_mc_device_sm (dev);
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
