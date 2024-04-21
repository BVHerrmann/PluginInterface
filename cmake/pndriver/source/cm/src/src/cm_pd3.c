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
/*  F i l e               &F: cm_pd3.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  803
#define CM_MODULE_ID       803

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/
/*=== PD Is Ready ===========================================================*/
/*===========================================================================*/
LSA_BOOL
cm_pdusr_is_ready (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_CHANNEL_PTR_TYPE channel;

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (channel->state != CM_PD_READY) {

		CM_PD_TRACE_01(usr_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			,"cm-pd is not ready, state(%u)"
			, channel->state
			);
		return LSA_FALSE;
	}

	CM_PD_TRACE_00(usr_channel->trace_idx,LSA_TRACE_LEVEL_CHAT, "cm-pd is ready");
	return LSA_TRUE;
}


/*===========================================================================*/
/*=== PD SendClock ==========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdusr_is_parameterized (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 *send_clock,
	LSA_BOOL   *is_reduction_ratio_fixed
) {
	CM_CHANNEL_PTR_TYPE  channel;

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	if( channel->usr.pd.pdprm.is_parameterized ) {

		*send_clock = channel->usr.pd.clock.send_clock;

		*is_reduction_ratio_fixed = cm_pd_sendclock_prop_rr_is_fixed(channel);

		return LSA_TRUE;
	}
	else {

		*send_clock = 0;

		*is_reduction_ratio_fixed = LSA_FALSE;

		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pdusr_is_send_clock_fixed ( /* returns the send_clock if "fixed", 0 otherwise */
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_BOOL *is_reduction_ratio_fixed
) {
	CM_CHANNEL_PTR_TYPE channel;

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	*is_reduction_ratio_fixed = cm_pd_sendclock_prop_rr_is_fixed(channel);

	/***/

	if( cm_pd_sendclock_prop_sc_is_fixed (channel) ) {

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "yes, send_clock is fixed by sendclock_prop"
			);

		return channel->usr.pd.clock.send_clock;
	}
	else if( channel->usr.pd.clock.ar_cnt != 0 ) {

		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "yes, send_clock is fixed by an existing ar, ar_cnt(%u)"
			, channel->usr.pd.clock.ar_cnt
			);

		return channel->usr.pd.clock.send_clock;
	}
	else {

		CM_PD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no, send_clock is not fixed"
			);

		return 0; /* cm_pdusr_attach() with another send_clock will succeed */
	}
}



/*===========================================================================*/
/*=== get IR_DATA_UUID from PDEV ============================================*/
/*===========================================================================*/

CLRPC_UUID_PTR_TYPE
cm_pdusr_get_ir_data_uuid (
	CM_CHANNEL_PTR_TYPE  usr_channel
) {
	CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	return & pd_channel->usr.pd.reclist.prm_data.ir_data_uuid;
}


/*===========================================================================*/
/*=== ATTACH ==============================================*/
/*===========================================================================*/
LSA_UINT16
cm_pdusr_attach (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 send_clock
) {
	CM_CHANNEL_PTR_TYPE channel;
	LSA_UINT16 rc;

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: cm_pdusr_attach() so that it may be called with all EDDs.")
#endif /* CM_MESSAGE */

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (usr_channel->path_type == CM_PATH_TYPE_SERVER) {

		if (! channel->usr.pd.pdprm.is_parameterized) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "pdev not parameterized (no bug)"
				);
			/* SV may start with "phy power on" (no pdev remanence) */
		}

		rc = cm_pd_send_clock_set (channel, send_clock, cm_pd_sendclock_prop_get (channel)/*keep existing*/);
	}
	else { /* CL/MC (send-clock must be already prm-ed) */

		if (! channel->usr.pd.pdprm.is_parameterized) {
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "pdev not parameterized"
				);
			/* CL calls cm_pdusr_is_parameterized() when adding a device */
			/* MC calls cm_pdusr_is_parameterized() when adding a device */
			CM_FATAL(); /* a bug */
		}

		if (send_clock != channel->usr.pd.clock.send_clock) {
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "send_clock(%u) != pdev-send_clock(%u)"
				, send_clock, channel->usr.pd.clock.send_clock
				);
			rc = CM_ERR_PARAM;
		}
		else {
			rc = CM_OK;
		}
	}

	if (rc == CM_OK  ||  rc == CM_OK_PENDING) {

		channel->usr.pd.clock.ar_cnt += 1;

		if (channel->usr.pd.clock.ar_cnt == 0) {
			CM_FATAL(); /* a bug */
		}
	}
	else {

		/* not incrementing (not attached) */
	}

	CM_PD_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "send_clock(%u), ar_cnt(%u), rsp(%u)"
		, send_clock, channel->usr.pd.clock.ar_cnt, rc
		);

	return rc;
}

/*===========================================================================*/
/*=== DETACH ================================================================*/
/*===========================================================================*/
LSA_VOID
cm_pdusr_detach (
	CM_CHANNEL_PTR_TYPE usr_channel
) {
	CM_CHANNEL_PTR_TYPE channel;

	/* context-switch from CM-CL/MC/SV to CM-PD */

	channel = cm_peer_channel_get(usr_channel, CM_PATH_TYPE_PD);

	/***/

	if (channel->usr.pd.clock.ar_cnt < 1) {
		CM_FATAL (); /* a bug */
	}

	channel->usr.pd.clock.ar_cnt -= 1;
}

/*===========================================================================*/
/*=== CM-PD SET SEND-CLOCK ==================================================*/
/*===========================================================================*/

LSA_UINT16		/* CM_OK | CM_OK_PENDING | CM_ERR_xxx */
cm_pd_send_clock_set (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock,
	LSA_UINT16  sendclock_prop
) {
	LSA_UINT16  rc;

	if (send_clock == 0) {

		CM_FATAL();
		rc = CM_ERR_PARAM; /* unreachable */
	}
	else if (send_clock == channel->usr.pd.clock.send_clock) { /* same send-clock */

		if (sendclock_prop != channel->usr.pd.clock.sendclock_prop) {

			channel->usr.pd.clock.sendclock_prop = sendclock_prop; /*AP01565798*/
		}

		if (channel->usr.pd.clock.is_pending) {

			rc = CM_OK_PENDING;
		}
		else {

			rc = CM_OK;
		}
	}
	else { /* new send-clock */

		if (channel->usr.pd.clock.is_pending) {

			CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot set new send_clock(%u), old is_pending(%u)"
				, send_clock, channel->usr.pd.clock.is_pending
				);

			rc = CM_ERR_PARAM;
		}
		else if (channel->usr.pd.clock.ar_cnt != 0) {

			CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "send_clock is in use, ar_cnt(%u)"
				, channel->usr.pd.clock.ar_cnt
				);

			rc = CM_ERR_PARAM;
		}
		else if (! cm_edd_can_set_send_clock (channel)) {

			CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "EDDS cannot set send_clock(%u), fixed to send_clock(%u)"
				, send_clock, channel->usr.pd.clock.send_clock
				);

			rc = CM_ERR_PARAM;
		}
		else {

			if( cm_edd_set_send_clock(channel, send_clock) ) {

				channel->usr.pd.clock.send_clock = send_clock; /* change PDEV-SendClock */
				channel->usr.pd.clock.sendclock_prop = sendclock_prop; /*AP01565798*/
				channel->usr.pd.clock.is_pending = LSA_TRUE;

				rc = CM_OK_PENDING;
			}
			else {

				CM_PD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cannot cm_edd_set_send_clock()"
					);

				rc = CM_ERR_ALLOC_LOWER;
			}
		}
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "send_clock(%u) rc(%u)", send_clock, rc);
	return rc;
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_set_send_clock_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_response,
	LSA_UINT16 send_clock
) {
	if( ! channel->usr.pd.clock.is_pending ) {
		CM_FATAL();
	}

	channel->usr.pd.clock.is_pending = LSA_FALSE;

	/***/

	if (edd_response != EDD_STS_OK) {

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "set send-clock(%u) failed, edd-rsp(%u)"
			, send_clock, edd_response
			);

		send_clock = 0; /* send-clock is not set */
		channel->usr.pd.clock.send_clock = 0; /* send-clock is not set */
	}
	else {

		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"set send-clock(%u) succeeded"
			, send_clock
			);

		CM_ASSERT(channel->usr.pd.clock.send_clock == send_clock); /* confirm changed PDEV-SendClock */
	}

	/* announce send-clock changed */

	cm_cmpd_sendclock_changed(channel, send_clock);

#if CM_CFG_MAX_SERVERS
	{
	CM_CHANNEL_PTR_TYPE sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);
	if (cm_is_not_null (sv_channel)) {
		cm_sv_sendclock_changed(sv_channel, send_clock);
	}
	}
#endif
}


/*===========================================================================*/
#if CM_CFG_MAX_SERVERS
/*===========================================================================*/

LSA_BOOL
cm_pdsv_mapping_ok (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);
	CM_PD_PORT_PTR_TYPE port;

	/* edd_port_id, mod_ident and sub_ident are no longer checked, see comment in caller (CM V4.2i1.2, AP00827094) */

	port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);

	if( cm_is_null(port) ) {

		return LSA_FALSE; /* mapping does not exist */
	}

	return LSA_TRUE;
}


/*===========================================================================*/
/*=== OCCUPANT IS-NOBODY ====================================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdsv_occupant_is_nobody ( /* called before cm_pdsv_occupant_set_reset() */
	CM_CHANNEL_PTR_TYPE sv_channel
) {
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	if( cm_is_not_null(channel) && channel->usr.pd.pdprm.occupant == CM_PD_OCCUPANT_NOBODY ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== OCCUPANT SET-RESET ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_pdsv_occupant_set_reset (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_BOOL do_set /* true..set, false..reset */
) {
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	if( cm_is_null(channel) ) {

		CM_FATAL();
	}
	else {

		CM_PD_PRM_PTR_TYPE  pdprm = &channel->usr.pd.pdprm;

		if( do_set ) {

			if( pdprm->occupant == CM_PD_OCCUPANT_NOBODY ) {

				pdprm->occupant = CM_PD_OCCUPANT_SV;
				pdprm->sv_ar_nr = 0;
			}
			else {

				CM_FATAL();
			}
		}
		else { /* reset */

			if( pdprm->occupant == CM_PD_OCCUPANT_SV ) {

				pdprm->occupant = CM_PD_OCCUPANT_NOBODY;

				if( pdprm->sv_ar_nr == CM_SV_R1B_PDEV_B_AR_NR ) { //R1 marker (special case)

					pdprm->sv_ar_nr = 0; /* note: injecting "relinquish" on bridge-broken is difficult */
				}

				if( pdprm->sv_ar_nr != 0 ) { /* see CM_OPC_PD_PRM_RELINQUISH_SERVER */

					CM_FATAL();
				}

				channel->usr.pd.almi.server_allowed = LSA_FALSE; /* see special case PDEV_B above */
				channel->usr.pd.almi.server_max_alarm_data_length = 0;
			}
			else if( pdprm->occupant == CM_PD_OCCUPANT_NOBODY ) {

				/* ok, sequence device-passivate, device-remove */
			}
			else {

				CM_FATAL();
			}
		}
	}
}

/*===========================================================================*/
/*=== PRM SERVER BEGIN ======================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_prm_begin_server (
	CM_CHANNEL_PTR_TYPE pd_channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_PRM_PTR_TYPE pdprm = &pd_channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_BEGIN_PTR_TYPE prm_begin_args = rb->args.pd.prm_begin;
	LSA_UINT16 ar_nr = CM_EDD_GET_USER_ID_AR(rb); /* see cm_sv_pdev_request_done() */

	if (pdprm->occupant != CM_PD_OCCUPANT_SV /* always false */  ||  ar_nr == 0)
	{
		CM_FATAL();
	}

	CM_PD_TRACE_02 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PD-begin-server: slot_nr(%u) subslot_nr(0x%x)", prm_begin_args->sv.slot_nr, prm_begin_args->sv.subslot_nr);

	if (pdprm->sv_ar_nr != 0)
	{
		/* note: between prm-begin and prm-relinquish */
		if (pdprm->sv_ar_nr != ar_nr)
		{
			/* case: new primary AR interrupts pdev-prm of old primary AR */
		}
	}

	pdprm->sv_ar_nr = ar_nr;

	pd_channel->usr.pd.almi.server_allowed = LSA_FALSE; /* disable alarms */
	pd_channel->usr.pd.almi.server_max_alarm_data_length = 0;

	/***/

	if (! pd_channel->usr.pd.reclist.is_valid)
	{
		/* note: between prm-begin and prm-end */
		/* case: new primary AR interrupts pdev-prm of old primary AR */
	}

	cm_pd_reclist_prm_begin (pd_channel, prm_begin_args->sv.slot_nr, prm_begin_args->sv.subslot_nr);

	/***/

	cm_pd_req_callback (pd_channel, CM_OK, rb);
}


/*===========================================================================*/
/*=== PRM SERVER END ========================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_pd_prm_end_server_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_AR_PRM_END_PTR_TYPE prm_end_args
) {
	LSA_BOOL  check_ok = LSA_TRUE;
	LSA_BOOL  interface_ok = LSA_TRUE;
	LSA_UINT16  i;

	for (i = 0; i < prm_end_args->nr_of_elems; i++)
	{
		CM_PD_AR_PRM_END_ELEM_PTR_TYPE elem = &prm_end_args->elem[i];

		if (elem->flags == 0)
		{
			CM_FATAL(); /* A bug */
		}
		else
		{
			CM_PD_PRM_DIAG_TYPE  prm_diag;
			CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr__nn (channel, elem->slot_nr, elem->subslot_nr);

			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"PD-End: slot_nr(%u) subslot_nr(0x%x) flags(%u)",
				elem->slot_nr, elem->subslot_nr, elem->flags);

			if (elem->flags & CM_PD_PRM_END_WRITE)
			{
				CM_PD_PRM_DIAG_SET_WRITE_FAULT (&prm_diag, port->port_id, elem->index, elem->offset);
				cm_pd_prm_diag_add (&channel->usr.pd.pdprm, &prm_diag);
				check_ok = LSA_FALSE;
			}

			if (CM_PDEV_TYPE_IS_COMPACT(port->addr.pdev_properties))
			{
				if (elem->flags & (CM_PD_PRM_END_WRONG | CM_PD_PRM_END_LOCKED | CM_PD_PRM_END_DIRTY))
				{
					check_ok = LSA_FALSE;

					if (port->port_id == 0)
					{
						interface_ok = LSA_FALSE;
					}
				}
				else if (elem->flags & CM_PD_PRM_END_PULLED)
				{
					CM_FATAL(); /* A bug */
				}
			}
			else /* modular */
			{
				if (elem->flags & (CM_PD_PRM_END_LOCKED))
				{
					check_ok = LSA_FALSE;

					if (port->port_id == 0)
					{
						interface_ok = LSA_FALSE;
					}
				}
			}
		}
	}

	/* if RQB::IRDataUUID == NIL ignore, else do not commit if not equal to PDIRGlobalData::IRDataUUID */
	if (interface_ok && !cm_uuid_is_nil (&prm_end_args->ir_data_uuid))
	{
		CM_PD_RECLIST_PTR_TYPE  reclist = &channel->usr.pd.reclist;

		if (!cm_uuid_equal (&prm_end_args->ir_data_uuid, &reclist->prm_data.ir_data_uuid))
		{
			CM_PD_PRM_DIAG_TYPE prm_diag;

			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PD-end failed, IRDataUUID mismatch");

			CM_PD_PRM_DIAG_SET_END_FAULT (&prm_diag, 0, 0x802c, 20);
			cm_pd_prm_diag_add (&channel->usr.pd.pdprm, &prm_diag);
			check_ok = LSA_FALSE;
		}
	}

	return check_ok;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_end_server (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_END_PTR_TYPE prm_end = rb->args.pd.prm_end;
	CM_PD_AR_PRM_END_PTR_TYPE prm_end_args;

	if (cm_is_null (prm_end))
	{
		CM_FATAL();
		return;
	}

	prm_end_args = (CM_PD_AR_PRM_END_PTR_TYPE)prm_end->sv.void_ptr;

	if (cm_is_null (prm_end_args))
	{
		CM_FATAL();
		return;
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PD-end-server: slot_nr(%u) subslot_nr(0x%x)", prm_end_args->slot_nr, prm_end_args->subslot_nr);

	{
		LSA_UINT16 ar_nr = CM_EDD_GET_USER_ID_AR(rb); /* see cm_sv_pdev_request_done() */
		CM_UPPER_RQB_PTR_TYPE tmp = cm_pd_req_queue_remove_current (channel); /* see cm_pd_req_callback() */

		if (pdprm->occupant != CM_PD_OCCUPANT_SV  ||  ar_nr == 0  ||  ! (tmp == rb))
		{
			CM_FATAL();
		}
		else
		{
			if (ar_nr != pdprm->sv_ar_nr || (channel->usr.pd.rtf.wf_sm & CM_PD_RTF_WF__IS_RTF))
			{
				/* late, do nothing */
				cm_pd_req_callback (channel, CM_ERR_PRM_CONSISTENCY, rb); /* see cm_sv_pd_prm_end_confirmation() */
				return;
			}

			/* must not reset "sv_ar_nr" here see cm_pd_prm_relinquish_server() */
		}
	}

	if (channel->usr.pd.reclist.is_valid)
	{
		CM_FATAL();
	}

	if (cm_pd_reclist_add_default_rec (channel, pdprm, LSA_TRUE, rb->args.pd.prm_end->set_mrp_off) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}

	if (cm_pd_reclist_add_ep_rec (channel) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}

	cm_pd_alarm_user_enable (pdprm->channel, LSA_FALSE);

	channel->usr.pd.reclist.is_valid = LSA_TRUE;

	rb->args.pd.prm_end->is_master_project = CM_IS_MASTER_PROJECT_NO;

	if (cm_is_not_null (pdprm->end))
	{
		CM_FATAL();
	}
	else
	{
		pdprm->end = rb; /* pending (PD-end.cnf is used to signal PD-appl-ready) */

		if (! cm_pd_prm_end_server_check (channel, prm_end_args))
		{
			cm_pd_reclist_set_error (channel);
		}

		/* check: same as prm-begin */
		if (prm_end_args->subslot_nr == 0)
		{
			if (! cm_is_null (pdprm->port))
			{
				CM_FATAL();
			}
		}
		else
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr (channel, prm_end_args->slot_nr, prm_end_args->subslot_nr);

			if (pdprm->port != port)
			{
				CM_FATAL();
			}
		}

		cm_pd_config_start (channel, CM_PD_CONFIG_PRM_SV); /* note: inserts CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER into queue */
	}
}


/*===========================================================================*/
/*=== PRM RELINQUISH ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_prm_relinquish_server (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_PD_PRM_PTR_TYPE pdprm = &pd_channel->usr.pd.pdprm;
	LSA_UINT16 ar_nr = CM_EDD_GET_USER_ID_AR(rb); /* see cm_sv_pdev_request_done() */

	if(
		pdprm->occupant != CM_PD_OCCUPANT_SV /* always false */
		|| ar_nr == 0
	) {

		CM_FATAL();
	}
	else {

		if( ar_nr != pdprm->sv_ar_nr ) {

			/* late, do nothing */
			cm_pd_req_callback (pd_channel, CM_OK, rb);
		}
		else {

			LSA_UINT16  port_id;

			pdprm->sv_ar_nr = 0;

			if( rb->args.pd.prm_relinquish->partner_session_key != 0 ) { /* TIA 1892236 */

				/* don't disable the alarms (this AR is an SR-AR and its partner is alive and will process the alarms) */
			}
			else {

				pd_channel->usr.pd.almi.server_allowed = LSA_FALSE;
				pd_channel->usr.pd.almi.server_max_alarm_data_length = 0; /* sanity */
			}

			if( ! pd_channel->usr.pd.reclist.is_valid ) { /* AR aborted before prm-end */

				/* setup for next parameterization or occupant-change; compare to cm_pd_reclist_init() */
				cm_pd_reclist_prm_begin (pd_channel, 0, 0);
				pd_channel->usr.pd.reclist.is_valid = LSA_TRUE;
			}

			cm_pd_req_callback (pd_channel, CM_OK, rb);

			/* TIA 1412642: remove all subcomponent diagnosis entries */
			for (port_id = 0; port_id <= pd_channel->usr.pd.port_count; ++port_id) {

				CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (pd_channel, port_id);

				cm_pd_diag_update_start (pd_channel, port);

				cm_pd_diag_list_clear (& port->diag.diag_list, CM_PD_DIAG_TAG_PRMERROR);

				cm_pd_diag_update_done (pd_channel, port);
			}
		}
	}
}

/*===========================================================================*/
/*=== ALARM RSP =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_opc_sv_ar_alarm_pdev_group_rsp (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE alarm
) {
	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_pd_opc_sv_ar_alarm_pdev_group_rsp : rqb(0x%X) slot_nr(%u) subslot_nr(0x%x)"
		, alarm, alarm->args.sv.ar_alarm_send->slot_nr, alarm->args.sv.ar_alarm_send->subslot_nr
		);

	if (channel->state != CM_PD_READY)
	{
		CM_FATAL ();
	}
	else if (cm_is_not_null(channel->usr.pd.almi.server_alarm))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unexpected: cm_pd_opc_sv_ar_alarm_pdev_group_rsp() but don't waiting for"
			);

		CM_FATAL ();
	}
	else
	{
		channel->usr.pd.almi.server_alarm = alarm;

		cm_pd_diag_sv_update_check (channel);
	}
}

/*===========================================================================*/
/*=== DIAG, MAINTENANCE AND QUALIFIED =======================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdsv_diag_maint_qual (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 *maint_status_ptr
) {
	CM_CHANNEL_PTR_TYPE channel;
	CM_PD_PORT_PTR_TYPE port;
	LSA_BOOL diag_status;

	CM_PD_TRACE_03(sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"IN : slot(%u) subslot(%u) h(%u)"
			, slot_nr, subslot_nr, sv_channel->my_handle
			);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	port = cm_pd_port_from_addr__nn (channel, slot_nr, subslot_nr);

	cm_pd_diag_maint_qual (port, & diag_status, maint_status_ptr);

	return diag_status;
}


/*===========================================================================*/
/*=== DIAG READ =============================================================*/
/*===========================================================================*/

CM_LIST_ENTRY_PTR_TYPE
cm_pdsv_diag_read (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_PD_PORT_PTR_TYPE port;
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03(sv_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		,"IN : slot(%u) subslot(%u) h(%u)"
		, slot_nr, subslot_nr, sv_channel->my_handle
		);

	/* context-switch from CM-SV to CM-PD */

	CM_ASSERT(sv_channel->path_type == CM_PATH_TYPE_SERVER);
	channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);

	/***/

	port = cm_pd_port_from_addr__nn (channel, slot_nr, subslot_nr);

	return &(port->diag.diag_list);
}


/*===========================================================================*/
/*=== Pull/Plug =============================================================*/
/*===========================================================================*/

LSA_UINT16
cm_pdsv_get_interface_subslot_nr(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 * map_count /* see CM_PD_CREATE_TYPE::map_count */
){
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);
	CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn(channel, 0);

	*map_count = 1 + channel->usr.pd.port_count; /* see cm_pd_port_addr_map_check() */

	return port->addr.subslot_nr;
}

LSA_BOOL
cm_pdsv_port_from_id(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16  port_id,
	LSA_UINT16* slot_nr,        /* out */
	LSA_UINT16* subslot_nr,     /* out */
	LSA_UINT32* mod_ident,      /* out */
	LSA_UINT32* sub_ident,      /* out */
	LSA_UINT8*  im0_bits,       /* out */
	LSA_UINT16* pdev_properties /* out */
){
	CM_CHANNEL_PTR_TYPE channel = cm_peer_channel_get(sv_channel, CM_PATH_TYPE_PD);
	CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id(channel, port_id);

	if(cm_is_null(port)) {

		return LSA_FALSE;
	}

	*slot_nr    = port->addr.slot_nr;
	*subslot_nr = port->addr.subslot_nr;
	*mod_ident  = port->addr.mod_ident;
	*sub_ident  = port->addr.sub_ident;
	*im0_bits   = port->addr.im0_bits;
	*pdev_properties = port->addr.pdev_properties;

	return LSA_TRUE;
}

/*===========================================================================*/
#endif /* CM_CFG_MAX_SERVERS */
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*=== PDCL REQUEST ==========================================================*/
/*===========================================================================*/
#if CM_CFG_MAX_CLIENTS

LSA_VOID
cm_pdcl_request(
	CM_CHANNEL_PTR_TYPE cl_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_CHANNEL_PTR_TYPE channel;

	CM_PD_TRACE_03 (cl_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			,"cm_pdcl_request: dev(%u) ar(%u) cm-opc(%u)"
			, device_nr, ar_nr, opcode
			);

	/* context-switch from CM-CL to CM-PD */

	CM_ASSERT(cl_channel->path_type == CM_PATH_TYPE_CLIENT);

	channel = cm_peer_channel_get(cl_channel, CM_PATH_TYPE_PD);

	/***/

	CM_EDD_SET_USER_ID_HANDLE_DEV_AR (rb, cl_channel->my_handle, device_nr, ar_nr);

	CM_RQB_SET_HANDLE(rb, channel->my_handle);

	CM_RQB_SET_OPCODE(rb, opcode);

	/***/

	switch( opcode ) {

	case CM_OPC_PD_PRM_READ_CLIENT:
		break;

	default:
		CM_FATAL();
		break;
	}

	cm_pd_req_queue_add (channel, rb);
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
