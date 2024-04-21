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
/*  F i l e               &F: cm_cl15.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  opcodes for client                                                       */
/*                                                                           */
/*  CM_OPC_CL_DEVICE_PROVIDE_EVENT                                           */
/*  CM_OPC_CL_DEVICE_PROVIDE_ALARM                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	615
#define CM_MODULE_ID		615

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*------------------------------------------------------------------------------
//	helper
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_event_provide (
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_DEVICE_PROVIDE_EVENT);

	CmListInsertTail (& dev->event_q, (CM_LIST_ENTRY_PTR_TYPE)rb);
}

/*----------------------------------------------------------------------------*/
CM_UPPER_RQB_PTR_TYPE
cm_cl_dev_event_get (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev
	)
{
	CM_UPPER_RQB_PTR_TYPE  rb = LSA_NULL;
	if (! CmListIsEmpty (& dev->event_q))
	{
		CmListRemoveHead (& dev->event_q, rb, CM_UPPER_RQB_PTR_TYPE);
	}
	return rb;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_alarm_provide (
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{

	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_DEVICE_PROVIDE_ALARM);

	rb->args.cl.alarm->alarm_priority = CM_ALARM_PRIORITY_LOW; /* sanity */

	CmListInsertTail (& dev->alarm_q, (CM_LIST_ENTRY_PTR_TYPE)rb);
}

/*----------------------------------------------------------------------------*/
CM_UPPER_RQB_PTR_TYPE
cm_cl_dev_alarm_get (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev,
	LSA_UINT16  prio
	)
{
	CM_UPPER_RQB_PTR_TYPE  rb = LSA_NULL;
	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	if (! CmListIsEmpty (& dev->alarm_q))
	{
		CmListRemoveHead (& dev->alarm_q, rb, CM_UPPER_RQB_PTR_TYPE);

		rb->args.cl.alarm->alarm_priority = prio;
	}
	return rb;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_calc_resource_count (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev,
	LSA_UINT16 *nr_of_events,
	LSA_UINT16 *nr_of_alarms
	)
{
	CM_AR_GRAPH_PTR_TYPE  argr = dev->argr;

	*nr_of_events = 2; /* InData + (Abort | PreOffline | Offline) */

	if ((argr->cm.cl.sumode & CM_SUMODE_IsAdvanced)
	||	(argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3))
	{
		*nr_of_events += 1; /* PreData */
	}
	if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED)
	{
		*nr_of_events += 1; /* PrmBegin | PrmInData */
	}
	if (! (dev->run_mode & CM_CL_RMODE_DISABLE_DATA_STATUS_IND))
	{
		*nr_of_events += 1; /* DataStatus */
	}


	if ((argr->alarm_cr->alcr_properties & CM_ALCR_PROP_PRIO_MASK) == CM_ALCR_PROP_PRIO_ONLY_LOW)
	{
		*nr_of_alarms = 1; /* low */
	}
	else
	{
		*nr_of_alarms = 2; /* high + low */
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_DEVICE_PROVIDE_EVENT
// handle opcode CM_OPC_CL_DEVICE_PROVIDE_ALARM
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_device_provide_resource (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev;
	LSA_UINT16  device_nr;

	switch (CM_RQB_GET_OPCODE(rb))
	{
	case CM_OPC_CL_DEVICE_PROVIDE_EVENT:
		device_nr = rb->args.cl.event_ind->device_nr;
		break;

	case CM_OPC_CL_DEVICE_PROVIDE_ALARM:
		device_nr = rb->args.cl.alarm->device_nr;
		break;

	default:
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	dev = cm_cl_dev_from_device_nr (channel, device_nr);
	if (cm_is_null (dev))
	{
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (dev->state != CM_CL_DEV_ST_PASSIVE)
	{
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	switch (CM_RQB_GET_OPCODE(rb))
	{
	case CM_OPC_CL_DEVICE_PROVIDE_EVENT:
		cm_cl_dev_event_provide (dev, rb);
		break;

	case CM_OPC_CL_DEVICE_PROVIDE_ALARM:
		cm_cl_dev_alarm_provide (dev, rb);
		break;

	default:
		break;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_ABORT_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_abort_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d)", upper_event->device_nr);
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

	/* no session_key check: a newer session is possible */

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_AR_ABORT_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:
		/* only provide */
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		CM_FATAL1 (argr->cm.cl.state);
		break;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_PRE_DATA_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_pre_data_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

		/* free event-memory */
	cm_pd_diff_list_free (channel, & upper_event->u.cl.in_data.diff_list);
	if (upper_event->u.cl.in_data.mod_diff_block_length > 0)
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.cl.in_data.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
	}

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr)) {
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "!dev(%d) ", upper_event->device_nr);
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

	/* no session_key check: a newer session is possible (see down) */

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRE_DATA_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_WF_NEW_DATA:
		cm_cl_arsm (argr);
		break;

	case CM_DST_IN_DATA:
/*
//		cm_cl_alarm_set_enabled_state (channel, argr, CM_CL_ALARM_ENABLED_FORWARD);
//		cm_cl_arsm (argr);
*/
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
		/* plug-alarm running */
		break;

	case CM_DST_PASSIVE:
	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
	default:
		CM_FATAL ();
		return;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_PRE_OFFLINE_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_pre_offline_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr))
	{
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

		/* old session */
	if (argr->session_key != upper_event->session_key)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "offline_counter(%d) != session_key(%d)"
			, upper_event->session_key, argr->session_key
			);
		CM_FATAL();
		return;
	}

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRE_OFFLINE_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_PROT));
		cm_cl_arsm_down (argr, &dinfo);
		break;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_IN_DATA_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_in_data_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

		/* free event-memory */
	cm_pd_diff_list_free (channel, & upper_event->u.cl.in_data.diff_list);
	if (upper_event->u.cl.in_data.mod_diff_block_length > 0)
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.cl.in_data.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
	}

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr)) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", upper_event->device_nr, upper_event->ar_nr);
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

	/* no session_key check: a newer session is possible */

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_IN_DATA_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_PASSIVE:
	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:
		/* only provide */
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		CM_FATAL ();
		return;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_OFFLINE_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_offline_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);

	if (cm_is_null (argr)) {
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

		/* old session */
	if (argr->session_key != upper_event->session_key) {
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "offline_counter(%d) != session_key(%d)"
			, upper_event->session_key, argr->session_key
			);
		CM_FATAL();
		return;
	}

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_OFFLINE_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_PASSIVE:
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_PROT));
		cm_cl_arsm_down (argr, &dinfo);
		break;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_DATA_STATUS_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_data_status_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_AR_GRAPH_PTR_TYPE  argr;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));

	upper_event = rb->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr)) {
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_DATA_STATUS_RSP);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	case CM_DST_IN_DATA:
	default:
		if (argr->cm.cl.usr_state == CM_CL_AR_INDATA) {
			cm_ar_com_cons_apdu_status_check_unicast (argr);
		}
		else {
			/* only provide */
		}
		break;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_ALARM_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_alarm_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_UPPER_ALARM_PTR_TYPE  alarm = rb->args.cl.alarm;

	CM_AR_GRAPH_PTR_TYPE  argr = cm_cl_ar_from_device (channel, alarm->device_nr);
	if (cm_is_null (argr)) {
		CM_FATAL();
	}
	CM_ASSERT (argr->ar_nr == alarm->ar_nr);

	/* free diff-list, initialise on every alarm_type, see cm_cl_acp_alarm_indication() */
	cm_pd_diff_list_free (channel, & alarm->u.cl.diff_list);
	if (alarm->u.cl.mod_diff_block_length > 0)
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, alarm->u.cl.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
	}

	/* TIA 2085965: move clear to acp-alarm-ack.cnf */
	if (alarm->alarm_priority == CM_ALARM_PRIORITY_LOW)
	{
		CM_ASSERT(CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP));
	}
	else if (alarm->alarm_priority == CM_ALARM_PRIORITY_HIGH)
	{
		CM_ASSERT(CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP));
	}
	else
	{
		CM_FATAL();
	}

	/* TIA: 1101169 */
	if(alarm->cm_pnio_err == CM_PNIO_ERR_AlarmAck_IOCARSR_backup
		&& (argr->cm.cl.dev->device_properties & CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM) == 0)
	{
		CM_CL_DOWN_INFO_TYPE  dinfo;
		CM_CL_DINFO_SET(&dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_SRD_ABORTED, CM_PNIO_ERR_AlarmAck_IOCARSR_backup);
		cm_cl_arsm_down(argr, &dinfo);
	}

	if (cm_ar_com_is_down(argr))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "Alarm.rsp: dev(%u): ar is down"
			, argr->device_nr
			);

		cm_cl_alarm_clear_wait_for(argr, alarm->alarm_priority);
		cm_cl_alarm_provide (argr, rb);

		cm_cl_arsm (argr);
	}
	else
	{
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
		CM_CL_AR_ALARM_PTR_TYPE  a_prio = & argr->cm.cl.alarm[alarm->alarm_priority];
		LSA_BOOL  valid;

		if (cm_is_not_null (a_prio->ack)) {
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "Alarm.rsp: dev(%u) ar(%u): ack running", alarm->device_nr, alarm->ar_nr);
			CM_FATAL();
		}

		if (cm_cl_alarm_is_device_alarm(argr, alarm->api, alarm->slot_nr, alarm->subslot_nr
			, alarm->mod_ident, alarm->sub_ident, alarm->alarm_type, &valid))
		{
			if (! valid)
			{
				CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "Alarm.rsp: dev(%u) ar(%u): invalid: alarm_type(0x%x) api(%u) slot(%u) subslot(0x%x) rs_info(0x%x)"
					, alarm->device_nr, alarm->ar_nr
					, alarm->alarm_type, alarm->api, alarm->slot_nr, alarm->subslot_nr, argr->rs_info
					);
				CM_FATAL();
			}
		}
		else if (! cm_cl_alarm_param_ok (argr, alarm->alarm_type
			, alarm->api, alarm->slot_nr, alarm->subslot_nr, &sub))
		{
			CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "Alarm.rsp: dev(%u) ar(%u): !found: alarm_type(0x%x) api(%u) slot(%u) subslot(0x%x)",
				alarm->device_nr, alarm->ar_nr,
				alarm->alarm_type, alarm->api, alarm->slot_nr, alarm->subslot_nr
				);
			CM_FATAL();
		}

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "Alarm.rsp: dev(%u) ar(%u): alarm-ack-send: alarm_type(0x%x)"
			, alarm->device_nr, alarm->ar_nr, alarm->alarm_type
			);

		cm_acp_alarm_copy_ack (a_prio->ack_rqb, LSA_NULL, alarm, alarm->cm_pnio_err);
		a_prio->ack = a_prio->ack_rqb;

		cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, a_prio->acp_ind);
		a_prio->acp_ind = LSA_NULL;

		cm_cl_alarm_provide (argr, rb);

		cm_acp_alarm_ack_send (argr, a_prio->ack_rqb);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_ALARM_SEND
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_alarm_send (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
)
{
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_UPPER_ALARM_PTR_TYPE  ad; /* common alarm data struct */
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	ad = rb->args.cl.alarm_send;	/* a common data struct pointer */
	CM_ASSERT (cm_is_not_null (ad));

		/* check para */
	argr = cm_cl_ar_from_device (channel, ad->device_nr);
	if (cm_is_null (argr)) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)",  ad->device_nr, ad->ar_nr);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}
	CM_ASSERT (argr->ar_nr == ad->ar_nr);

	if (cm_alarm_type_is_internal (ad->alarm_type)) {
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!alarm_type(%d)",  ad->alarm_type);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

		/* set alarm_priority according to alarm_type and alcr_property */
	{
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE alcr = argr->alarm_cr;

	if (cm_is_not_null (alcr) /* should be always true */
		&& (alcr->alcr_properties & CM_ALCR_PROP_PRIO_MASK) == CM_ALCR_PROP_PRIO_ONLY_LOW)
	{
		ad->alarm_priority = CM_ALARM_PRIORITY_LOW;
	}
	else {

		/* PNIO-Spec V2.3, Part 6: process alarm prio high, others low (V6.0i2.xx, TIA 1166054) */

		if (ad->alarm_type == CM_ALARM_TYPE_PROCESS)
		{
			ad->alarm_priority = CM_ALARM_PRIORITY_HIGH;
		}
		else
		{
			ad->alarm_priority = CM_ALARM_PRIORITY_LOW;
		}
	}
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: check ad->alarm_data_length against alcr->max_data_length")
#endif /* CM_MESSAGE */

	cm_ar_graph_find_api_slot_subslot (argr,
			ad->api, ad->slot_nr, ad->subslot_nr,
			& ap, & mod, & sub
		);
	if (cm_is_null (sub)) {
		CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u):  !found: api(%d) slot(%d) subslot(%d)",
				ad->device_nr, ad->ar_nr,
				ad->api, ad->slot_nr, ad->subslot_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	ad->mod_ident = mod->soll.mod_ident;
	ad->sub_ident = sub->soll.sub_ident;


	if (argr->session_key != ad->session_key) {
		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev.session_key(%d) != session_key(%d): dev(%d) ar(%d)",
				argr->session_key, ad->session_key, ad->device_nr, ad->ar_nr
			);
		cm_callback (channel, CM_ERR_SESSION, & rb);
		return;
	}

	if (argr->cm.cl.usr_state != CM_CL_AR_INDATA) {
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!IN_DATA, usr_state(%d), dev(%d) ar(%d)",
				argr->cm.cl.usr_state, ad->device_nr, ad->ar_nr
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	ad->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO;
	ad->diag_generic_available = CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO;
	ad->diag_submod_available = CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;
	ad->ar_diagnosis_state = CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO;

		/* kick alarm-send-machine */
	if (CmListIsEmpty (& argr->cm.cl.alarm[ad->alarm_priority].alarm_list)) {

		CmListInsertHead (& argr->cm.cl.alarm[ad->alarm_priority].alarm_list,
				(CM_LIST_ENTRY_PTR_TYPE)rb
			);
		cm_cl_alarm_send_next (channel, argr, ad->alarm_priority);
		return;
	}

		/* alarm-send-machine is running, simple add to queue */
	CmListInsertTail (& argr->cm.cl.alarm[ad->alarm_priority].alarm_list,
			(CM_LIST_ENTRY_PTR_TYPE)rb
		);

	return;
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
