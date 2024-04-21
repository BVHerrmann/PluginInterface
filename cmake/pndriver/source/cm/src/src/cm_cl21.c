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
/*  F i l e               &F: cm_cl21.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for acp                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	621
#define CM_MODULE_ID		621

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

#define CM_CL_ALPMI_INIT			0x00
#define CM_CL_ALPMI_SEND_DONE		0x01
#define CM_CL_ALPMI_ACK_DONE		0x02


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_acp_global_provider_control (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  apdu_status
)
{
	EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE global_prov;
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;

	edd = cm_edd_alloc_rqb (channel, EDD_SRV_SRT_PROVIDER_SET_STATE);
	if (cm_is_null (edd)) {
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!edd-rqb");
		return CM_ERR_ALLOC_LOWER;
	}

	global_prov = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE);

	global_prov->Mask = 0;
	global_prov->Status = 0;
	global_prov->GroupID = CM_PROVIDER_GROUP_IOC;

	if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_STOP)) {
		global_prov->Mask |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}
	else if ((apdu_status & CM_CL_CONTROL_APDU_STATUS_RUN)) {
		global_prov->Mask	|= EDD_CSRT_DSTAT_BIT_STOP_RUN;
		global_prov->Status |= EDD_CSRT_DSTAT_BIT_STOP_RUN;
	}

	cm_edd_request_lower (channel, EDD_SRV_SRT_PROVIDER_SET_STATE, channel, edd);
	return CM_OK;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_acp_global_provider_control_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
)
{
	LSA_UINT16  rc;

	rc = CM_EDD_GET_RESPONSE (edd);
	CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "state(%u) rsp(%u)"
		, channel->state
		, rc
		);
	cm_edd_free_rqb (channel, edd);

	if (channel->state == CM_CL_GLOBAL_CTRL)
	{
		if (rc == EDD_STS_OK)
		{
			/*
			 * transitions CM-CL::GLOBAL_CTRL --> CM-CL::REG_IFC
			 */
			channel->state = CM_CL_REG_IFC;
			cm_cl_rpc_register_if_obj (channel);
			/* => cm_cl_rpc_register_if_obj_done() */
		}
		else
		{
			/*
			 * transition CM-CL::GLOBAL_CTRL --> CM-CH::READY
			 */
			channel->state = CM_CH_READY;
			cm_cl_client_free (channel);
			cm_callback (channel, CM_ERR_LOWER_LAYER, & channel->usr.cl.upper);
		}
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.cl.upper_apdu) == CM_OPC_CL_CONTROL);
		cm_callback (channel,
				CM_CAST_U16 ((rc == EDD_STS_OK) ? CM_OK : CM_ERR_LOWER_LAYER),
				& channel->usr.cl.upper_apdu
			);

		if (cm_is_not_null (channel->usr.cl.upper))
		{
			cm_cl_delete_done (channel, 0/*event*/);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_provide (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	upper->args.cl.alarm->alarm_data = LSA_NULL;
	upper->args.cl.alarm->alarm_data_length = 0;

	CM_RQB_SET_OPCODE (upper, CM_OPC_CL_DEVICE_PROVIDE_ALARM);
	cm_cl_dev_alarm_provide (argr->cm.cl.dev, upper);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_set_enabled_state (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  enabled_state
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	switch (enabled_state) {
	case CM_CL_ALARM_ENABLED_IGNORE:
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): alarm_enabled_state(IGNORE)", argr->device_nr
			);
		argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_IGNORE;
		break;

	case CM_CL_ALARM_ENABLED_DELAY:
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): alarm_enabled_state(DELAY)", argr->device_nr
			);
		argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_DELAY;
		break;

	case CM_CL_ALARM_ENABLED_FORWARD:
		if (argr->cm.cl.alarm_enabled_state != CM_CL_ALARM_ENABLED_FORWARD)
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u): alarm_enabled_state(FORWARD), was(%u)", argr->device_nr, argr->cm.cl.alarm_enabled_state
				);

			argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_FORWARD;

				/* check queued alarms */
			cm_cl_alarm_indication (channel, argr, CM_ALARM_PRIORITY_HIGH);
			cm_cl_alarm_indication (channel, argr, CM_ALARM_PRIORITY_LOW);
		}
		break;

	default:
		CM_FATAL1( enabled_state);
		return;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_mcm_alarm_append_affected_submods (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_RQB_CONST_PTR_TYPE  upper,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  common_diag_info
	)
{
/*	for all mcr containing alarm->api, alarm->slot, alarm->sub:
//		foreach ioapi of this mcr:
//			foreach iodata of this ioapi:
//				append ioapi->api, iodata->slot, iodata->subslot to diff_list
*/

	CM_AR_GRAPH_IO_CR_PTR_TYPE   mcr;
	CM_UPPER_ALARM_PTR_TYPE alarm;

	alarm = upper->args.cl.alarm;
	CM_ASSERT (CmListIsEmpty (& alarm->u.cl.diff_list));

	CM_ASSERT (alarm->alarm_type == CM_ALARM_TYPE_MCR_MISMATCH);
	CM_ASSERT (common_diag_info == CM_SUB_STATE_DIAG_INFO_AVAILABLE
		|| common_diag_info == CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
		);

	for (CmListForeach (mcr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {
		CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi;
		CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
		LSA_BOOL  do_append;

		switch (mcr->iocr_type) {
		case CM_IOCR_TYPE_MULTICAST_PROVIDER:
		case CM_IOCR_TYPE_MULTICAST_CONSUMER:
			break;
		default:
			continue;
		}

		do_append = LSA_FALSE;
		for (CmListForeach (ioapi, & mcr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
			if (ioapi->api != api) {
				continue;
			}
			for (CmListForeach (iodata, & ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				if (iodata->slot_nr == slot_nr && iodata->subslot_nr == subslot_nr) {
					do_append = LSA_TRUE;
					break;
				}
			}
		}
		if (! do_append) {
			continue;
		}

		for (CmListForeach (ioapi, & mcr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE))
		{
			for (CmListForeach (iodata, & ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				CM_AR_GRAPH_AP_PTR_TYPE  ap;
				CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

				cm_ar_graph_find_api_slot_subslot (argr,
						ioapi->api, iodata->slot_nr, iodata->subslot_nr,
						& ap, & mod, & sub
					);

				if (cm_is_not_null (sub))
				{
					CM_UPPER_CL_DIFF_AP_PTR_TYPE  diff_ap;
					CM_UPPER_CL_DIFF_MODULE_PTR_TYPE  diff_mod;
					CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

					CM_ASSERT (cm_is_not_null (ap)); CM_ASSERT (cm_is_not_null (mod));

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check: really? setting diag-state from alarm to -all- related submodules")
#endif /* CM_MESSAGE */
					CM_SUB_STATE_SET_DIAG_INFO (sub->cl.real_sub_state, common_diag_info);

					diff_ap = cm_pd_diff_ap_find_append (channel, & alarm->u.cl.diff_list,
							ap->api
						);
					diff_mod = cm_pd_diff_module_find_append (channel, & diff_ap->module_list,
							mod->slot_nr, mod->ist.mod_ident
						);
					diff_sub = cm_pd_diff_submodule_find_append (channel, & diff_mod->submodule_list,
							sub->subslot_nr, sub->cl.real_sub_state, sub->cl.real_sub_ident
						);
					LSA_UNUSED_ARG (diff_sub);
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_alarm_param_ok (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	LSA_UINT16  alarm_type,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  *sub
)
{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;

	cm_ar_graph_find_api_slot_subslot (argr, api, slot_nr, subslot_nr, &ap, &mod, sub);

	if (cm_is_null (ap))
	{
		return LSA_FALSE;
	}

	if (cm_is_null (mod))
	{
		return LSA_FALSE;
	}

	switch (alarm_type)
	{
	case CM_ALARM_TYPE_PULL:
		if ((argr->ar.ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0)
		{
			if (cm_is_null (*sub)) {
				if (subslot_nr != 0) {
					return LSA_FALSE;
				}
				/* is a pull_module */
			}
			else if (subslot_nr == 0)
			{
				return LSA_FALSE; /* see cm_ar_graph_insert_submodule() */
			}
		}
		else
		{
			if (cm_is_null (*sub)) {
				return LSA_FALSE;
			}
		}
		break;

	case CM_ALARM_TYPE_PULL_MODULE:
		if ((argr->ar.ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0)
		{
			return LSA_FALSE;
		}
		/* subslot_nr not checked */
		break;

	default:
		if (cm_is_null (*sub)) {
			return LSA_FALSE;
		}
		break;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_alarm_type_and_submod_state_matches (
	LSA_UINT16  alarm_type,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE  sub
)
{
	LSA_UINT32  always_true;

	always_true = CM_ALARM_PLUG_GROUP | (1UL << CM_ALARM_TYPE_MCR_MISMATCH);

	if ((1UL << alarm_type) & always_true) {
		return LSA_TRUE;
	}

	CM_ASSERT (cm_is_not_null (sub));
	switch (sub->cl.real_sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:
	case CM_SUB_STATE_IDENT_SUBSTITUTE:
		return LSA_TRUE;

	case CM_SUB_STATE_IDENT_WRONG:
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:
	default:
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_alarm_set_sub_state (
	CM_UPPER_ALARM_CONST_PTR_TYPE  alarm,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub
	)
{
		/*
		 * PNIO-Spec, AlarmSpecifier.XXXDiagnosis, Usage within AlarmType
		 *
		 * Diagnosis, Redundancy, multicast communication mismatch, Port data change notification,
		 * Sync data change notification, isochronous mode problem notification,
		 * Network component problem notification, Time data changed notification
		 */

		if (alarm->diag_submod_available == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES) {
			CM_SUB_STATE_SET_DIAG_INFO (sub->cl.real_sub_state, CM_SUB_STATE_DIAG_INFO_AVAILABLE);
		}
		else {
			CM_SUB_STATE_SET_DIAG_INFO (sub->cl.real_sub_state, CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE);
		}

		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_REQUIRED) {
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK))
				| CM_SUB_STATE_MAINTENANCE_REQUIRED
				;
		}
		else {
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK))
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				;
		}

		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_DEMANDED) {
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK))
				| CM_SUB_STATE_MAINTENANCE_DEMANDED
				;
		}
		else {
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK))
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				;
		}

#if 0 /* future requirement, see TIA 1376282 */
		if (alarm->maintenance_status & CM_MAINTENANCE_STATUS_XXX) {
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_ADVICE_MASK))
				| CM_SUB_STATE_ADVICE_AVAILABLE
				;
		}
		else
#endif
		{
			sub->cl.real_sub_state = (sub->cl.real_sub_state & (~CM_SUB_STATE_ADVICE_MASK))
				| CM_SUB_STATE_ADVICE_NOT_AVAILABLE
				;
		}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_set_wait_for (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  alarm_priority
	)
{
	if (alarm_priority == CM_ALARM_PRIORITY_LOW)
	{
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP);
	}
	else
	{
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_clear_wait_for(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  alarm_priority
)
{
	if (alarm_priority == CM_ALARM_PRIORITY_LOW)
	{
		CM_CL_WF_CLEAR(argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP);
	}
	else {
		CM_CL_WF_CLEAR(argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP);
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_alarm_is_device_alarm (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	LSA_UINT32  api,
	LSA_UINT16  slot,
	LSA_UINT16  subslot,
	LSA_UINT32	module_ident,
	LSA_UINT32  submodule_ident,
	LSA_UINT16  alarm_type,
	LSA_BOOL  *valid
)
{
	*valid = LSA_FALSE;

	if (api == 0  &&  slot == 0x8000  &&  subslot == 0)
	{
		if (cm_is_not_null (argr->rs_info)
			&&  module_ident == 0
			&&  submodule_ident == 0
			&&  alarm_type == CM_ALARM_TYPE_STATUS )
		{
			*valid = LSA_TRUE;
		}

		return LSA_TRUE;
	}

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
/* please note
**	AP00219532: deliver submodule_state to cm-user
**  AP00841926: never generate CM_MOD_STATE_WRONG_MODULE internally
*/
LSA_VOID
cm_cl_alarm_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  alarm_prio
)
{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_ALARM_PTR_TYPE  alarm;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	LSA_BOOL  is_valid;

	upper = argr->cm.cl.alarm[alarm_prio].early_alarm;
	if (cm_is_null (upper))
	{
		/* no stored alarm */
		return;
	}

	alarm = upper->args.cl.alarm;

	switch (argr->cm.cl.alarm_enabled_state) {
	case CM_CL_ALARM_ENABLED_IGNORE:
		/* abort, unreachable see cm_cl_acp_alarm_indication() */
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "AlarmEvent: dev(%u) alarm_enabled_state(ignore)  alarm_type(0x%x) arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, alarm->alarm_type
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
		cm_cl_arsm_down (argr, &dinfo);
		return;

	case CM_CL_ALARM_ENABLED_DELAY:
		if (! cm_ar_com_is_up (argr)
		&&  argr->cm.cl.state == CM_DST_WF_NEW_DATA
		&&	cm_cl_prm_mode(argr) == CM_CL_PRMMODE_IDLE/*! means connect in this state */
		&&	(argr->cm.cl.sumode == CM_SUMODE_AdvRT || argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "AlarmEvent: dev(%u) alarm_enable_state(delay) alarm_type(0x%x), send PreData.ind to enable Alarm-handling"
				, argr->device_nr, alarm->alarm_type
				);
			/* trigger PreData.ind, trigger does CM_CL_ALARM_ENABLED_FORWARD */
			cm_cl_prm_ardy_timeout_stop (argr);
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
			cm_cl_arsm (argr);
		}
		else
		{
			/* keep alarm, because too early */
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "AlarmEvent: dev(%u) alarm_enable_state(delay) alarm_type(0x%x)"
				, argr->device_nr, alarm->alarm_type
				);
		}
		return;

	case CM_CL_ALARM_ENABLED_FORWARD:
		argr->cm.cl.alarm[alarm_prio].early_alarm = LSA_NULL; /* handle alarm */
		break;

	default:
		CM_FATAL1(argr->cm.cl.alarm_enabled_state);
		return;
	}

	/* checks are done in cm_cl_acp_alarm_indication() */

	CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "AlarmEvent: dev(%u) alarm_type(0x%x) api(%u) slot(%u=0x%x) subslot(%u=0x%x)  diag[channel(%u) generic(%u) submod(%u)]  maintenance(%x)"
		, argr->device_nr
		, alarm->alarm_type, alarm->api
		, alarm->slot_nr, alarm->mod_ident, alarm->subslot_nr, alarm->sub_ident
		, alarm->diag_channel_available, alarm->diag_generic_available, alarm->diag_submod_available
		, alarm->maintenance_status
		);


	if (cm_cl_alarm_is_device_alarm (argr, alarm->api, alarm->slot_nr, alarm->subslot_nr, alarm->mod_ident, alarm->sub_ident, alarm->alarm_type, &is_valid))
	{
		CM_ASSERT (is_valid);

		/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		return;
	}

	cm_ar_graph_find_api_slot_subslot (argr, alarm->api, alarm->slot_nr, alarm->subslot_nr, & ap, & mod, & sub);

	/* handle alarm */
	switch (alarm->alarm_type)
	{
	case CM_ALARM_TYPE_DIAGNOSIS:
	case CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS:
	case CM_ALARM_TYPE_MEDIA_REDUNDANCY:
/*	case CM_ALARM_TYPE_MCR_MISMATCH: see below */
	case CM_ALARM_TYPE_PORT_DATA_CHANGED:
	case CM_ALARM_TYPE_SYNC_DATA_CHANGED:
	case CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM:
	case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM:
	case CM_ALARM_TYPE_TIME_DATA_CHANGED:
	case CM_ALARM_TYPE_DFP_PROBLEM:
	case CM_ALARM_TYPE_MRPD_PROBLEM:
	case CM_ALARM_TYPE_MULTIPLE_INTERFACE:
			/* handle diag_info, see AlarmSpecifier */
		cm_cl_alarm_set_sub_state (alarm, sub);

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_MCR_MISMATCH:
			/* handle diag_info, see AlarmSpecifier */
		cm_cl_alarm_set_sub_state (alarm, sub);

		cm_cl_mcm_alarm_append_affected_submods (channel,
				argr, upper, ap->api, mod->slot_nr, sub->subslot_nr,
				CM_CAST_U16(sub->cl.real_sub_state & CM_SUB_STATE_DIAG_INFO_MASK)
			);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE ("TODO: check: should alarm->api, alarm->slot_nr, alarm->subslot_nr set to 0")
#endif /* CM_MESSAGE */
			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PULL:

			/* subslot != 0 .. pull submodule */
		if (cm_is_not_null (sub)) {
			/* mod->ist.mod_state = mod->ist.mod_state;  keep: more than one submod possible */

			sub->cl.real_sub_state = CM_SUB_STATE_PULLED;
			sub->cl.real_sub_ident = 0;
		}

			/* subslot == 0 .. pull module => pull all submodule */
			/* take care: plug alarm recv only for one subslot,
						so (maybe) we must update mod_state to */
		else {
			mod->ist.mod_ident = 0;

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				sub->cl.real_sub_state = CM_SUB_STATE_PULLED;
				sub->cl.real_sub_ident = 0;
			}
		}

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PULL_MODULE:

		mod->ist.mod_ident = 0;

		for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
		{
			sub->cl.real_sub_state = CM_SUB_STATE_PULLED;
			sub->cl.real_sub_ident = 0;
		}

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR:

			/* no more the owner of this submodule */
		sub->cl.real_sub_state = CM_SUB_STATE_PULLED;	/* review: or only set_ar_info */
		CM_SUB_STATE_SET_AR_INFO (sub->cl.real_sub_state, CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR);

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PLUG:
	case CM_ALARM_TYPE_RELEASED:

		/* mod_ident and sub_(ident|state) is set with cm_cl_prm_record_list_build(), see queueing of cm_cl_prm_request() */

			/* keep plug-alarm */
			/* notify cm-user is done on transition WF_NEW_DATA -> IN_DATA */
		CM_ASSERT (cm_is_null (argr->cm.cl.prm_alarm));
		argr->cm.cl.prm_alarm = upper;

			/* start prm-machine */
		/* argr->cm.cl.state = CM_DST_PRM; */
		cm_cl_prm_request (argr, CM_CL_PRMMODE_PLUG, upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_PLUG_WRONG:

		mod->ist.mod_ident = alarm->mod_ident;

		sub->cl.real_sub_ident = alarm->sub_ident;
		sub->cl.real_sub_state = CM_SUB_STATE_WRONG;

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;

/*----------------------------------------------------------------------------*/
	case CM_ALARM_TYPE_RETURN_OF_SUBMODULE:

		sub->cl.real_sub_state &= ~CM_SUB_STATE_AR_INFO_APPL_READY_PENDING;
		sub->cl.real_sub_state |=  CM_SUB_STATE_AR_INFO_OWNED;

		if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "AlarmEvent: dev(%u) alarm(RETURN-OF-SUBMODULE).ind with PreIndata", argr->device_nr
				);
			/* notify cm-user */
			cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
			cm_callback (channel, CM_OK, & upper);
		}
		else
		{
			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "AlarmEvent: dev(%u) alarm(RETURN-OF-SUBMODULE).ind, Start, arState(%u) wf0(0x%x) wf1(0x%x)"
				, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
				);

			if (cm_is_not_null (argr->cm.cl.rosal_alarm))
			{	/* unreachable */
				argr->cm.cl.alarm[alarm_prio].early_alarm = upper; /* undo */
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "AlarmEvent: dev(%u) alarm(RETURN-OF-SUBMODULE).ind, PROTOCOL-VIOLATION, arState(%u) wf0(0x%x) wf1(0x%x)"
					, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
					);
				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_PROT));
				cm_cl_arsm_down (argr, &dinfo);
				return;
			}

			/* do not: cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority); */

			argr->cm.cl.rosal_alarm = upper;
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_ALARM_ROSAL_IND);

			if (! (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK)
			||     CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS)))
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK);
				cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK, 0);
			}
			else
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "AlarmEvent: dev(%u) prm_mode(%u) set2unk is already set, skip", argr->device_nr, cm_cl_prm_mode(argr)
					);
			}
		}
		break;

/*----------------------------------------------------------------------------*/
	default:
		CM_ASSERT (! CM_ALARM_IS_DIAG_GROUP(alarm->alarm_type));

			/* notify cm-user */
		cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
		cm_callback (channel, CM_OK, & upper);
		break;
	}
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_alarm_maintenance_is_consistence (
	LSA_UINT16  user_tag,
	LSA_UINT16  user_length,
	CM_ACP_LOWER_MEM_PTR_TYPE  user_data,
	LSA_UINT32               * maintenance_ptr
	)
{
	*maintenance_ptr = 0;

	if (user_length == 0) {
		/* maintenance not possible */
		return LSA_TRUE;
	}

	if (user_tag != CM_ALARM_USI_MAINTENANCE) {
		/* no maintenance prefix */
		return LSA_TRUE;
	}

	if (user_length < 2+2+2+2+4) {
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "wrong size(%u), must >= 2+2+2+2+4", user_length
			);
		return LSA_FALSE;
	}

	{
	CM_ACP_MEM_U8_PTR_TYPE  dp;

	LSA_UINT16  block_type, block_length, block_version, reserved;
	LSA_UINT32  maintenance;

	dp = (CM_ACP_MEM_U8_PTR_TYPE)user_data + CM_ALARM_OFFSET_DATA;

	block_type		= CM_GET16_NTOH (dp, 0);
	block_length	= CM_GET16_NTOH (dp, 2);
	block_version	= CM_GET16_NTOH (dp, 4);
	reserved		= CM_GET16_NTOH (dp, 6);
	maintenance		= CM_GET32_NTOH (dp, 8);

	if (block_type != CM_BLOCK_TYPE_MAINTENANCE  || block_length != 8
	|| block_version != 0x0100  ||  reserved != 0
	|| (maintenance & CM_MAINTENANCE_STATUS_RESERVED) != 0 /* since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11 */
	) {
		CM_CL_TRACE_05 (0, LSA_TRACE_LEVEL_UNEXP
			, "maintenance-block-mismatch: something wrong: block_type(0x%x) block_length(%u) block_version(0x%x) res(%u) maintenance(0x%x)"
			, block_type, block_length, block_version, reserved
			, maintenance
			);
		return LSA_FALSE;
	}

	*maintenance_ptr = maintenance;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_alarm_type_is_supported (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  alarm_type
)
{
	LSA_UINT32  alarm_bit;

	if (alarm_type < 0x0020) {

		if (alarm_type == 0x0000) {
			return LSA_FALSE;
		}

		alarm_bit = (1UL << alarm_type);

		if ((channel->usr.cl.alarm_reject & alarm_bit) != 0) {
			return LSA_FALSE;
		}
	}
	else if (alarm_type < 0x0080) { /* manufacturer specific, use bit 0 */

#if CM_ALARM_MANUFACTURER_SPECIFIC_GROUP != 1UL
#error "check this"
#endif

		if ((channel->usr.cl.alarm_reject & CM_ALARM_MANUFACTURER_SPECIFIC_GROUP) != 0) {
			return LSA_FALSE;
		}
	}
	else if (alarm_type < 0x0100) { /* profiles */

		/* ok */
	}
	else { /* reserved */

		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
/*lint -e{818} symbol acp_al Pointer parameter 'Symbol' (Location) could be declared ptr to const request 1524437*/
static  LSA_BOOL
cm_cl_prm_update_reject_alarm (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al
	)
{
	if (acp_al->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE)
	{
		/* CM_CL_PRMMODE_AR */
		if (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_AR) /*AP01528984*/
		{
			return LSA_TRUE; /* reject: rosal overlap's with prm-update(ar) */
		}
		if (argr->cm.cl.prm.prm_seq_next == CM_CL_PRMMODE_AR)
		{
			/* AP01253553: this means, ROSAL is old, cancel it */  /* taken from cm_cl_prm_request_can_queue() */
			return LSA_TRUE;
		}
		if (argr->cm.cl.prm.prm_seq_next2 == CM_CL_PRMMODE_AR)
		{
			return LSA_TRUE;
		}

		/* CM_CL_PRMMODE_SUBMODULE, CM_CL_PRMMODE_COC */
		if (    argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_SUBMODULE  /* reject: rosal overlap's with prm-update */
			||  argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_COC)
		{
			if (    argr->cm.cl.prm.submod.api        == acp_al->api
				&&  argr->cm.cl.prm.submod.slot_nr    == acp_al->slot_nr
				&&  argr->cm.cl.prm.submod.subslot_nr == acp_al->subslot_nr)
			{
				return LSA_TRUE;
			}
		}
		else if (   argr->cm.cl.prm.prm_seq_next  == CM_CL_PRMMODE_SUBMODULE /* reject: rosal is old */
				||  argr->cm.cl.prm.prm_seq_next2 == CM_CL_PRMMODE_SUBMODULE
				||  argr->cm.cl.prm.prm_seq_next  == CM_CL_PRMMODE_COC  //COC: Test rosal
				||  argr->cm.cl.prm.prm_seq_next2 == CM_CL_PRMMODE_COC)
		{
			CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  sub;

			for (CmListForeach (sub, &argr->cm.cl.prm.prm_keep_upd->args.cl.prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
			{
				if (    sub->api        == acp_al->api
					&&  sub->slot_nr    == acp_al->slot_nr
					&&  sub->subslot_nr == acp_al->subslot_nr)
				{
					return LSA_TRUE;
				}
			}
		}
	}

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
CM_ACP_LOWER_RQB_PTR_TYPE
cm_cl_acp_alarm_indication (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
	)
{
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al = &acp->args->alarm.data;
	LSA_UINT32  pnio_err_do_reject;
	LSA_UINT16  next_recv_sequence_nr;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	LSA_UINT32  maintenance_status = 0;

	LSA_UINT16 alarm_sequence = (LSA_UINT16)(acp_al->alarm_specifier & 0x07FF); /* see ACP_GET_ALARM_SPECIFIER() */

	CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) skey(%u) alarm_type(0x%x) prio(%u) al_seq_nr(%u) api(%u) slot(%u) subslot(%u)"
		, argr->device_nr
		, argr->session_key, acp_al->alarm_type, prio, alarm_sequence
		, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
		);

	if (cm_ar_com_is_down (argr))
	{
		/* unreachable */
	}
	else if (prio == CM_ALARM_PRIORITY_HIGH  &&  (argr->alarm_cr->alcr_properties & CM_ALCR_PROP_PRIO_MASK) == CM_ALCR_PROP_PRIO_ONLY_LOW)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u): alarm_indication: CM_ALARM_PRIORITY_HIGH and alcr_properties wants CM_ALCR_PROP_PRIO_ONLY_LOW"
			, argr->device_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_PROT));
		cm_cl_arsm_down (argr, &dinfo);
	}
	else
	{
		/* check expected sequence_nr */
		if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED)
		{
			argr->cm.cl.alarm[prio].recv_sequence_nr = alarm_sequence; /* SRD, v2.3_Ed4: The SequenceNumber may start for each AR of an AR set with _every switchover_ with an arbitrary value. */
		}
		else if (argr->cm.cl.alarm[prio].recv_sequence_nr == CM_ALARM_SEQUENCE_UNDEF)
		{
			argr->cm.cl.alarm[prio].recv_sequence_nr = alarm_sequence; /* AP01334245, Rev16: The Alarm receiver shall accept an arbitrary value as start value */
		}

		if (argr->cm.cl.alarm[prio].recv_sequence_nr != alarm_sequence)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) expected.seq_nr(%u) != recv.seq_nr(%u)"
				, argr->device_nr
				, argr->cm.cl.alarm[prio].recv_sequence_nr, alarm_sequence
				);

			CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT), argr->cm.cl.alarm[prio].recv_sequence_nr);
			cm_cl_arsm_down (argr, &dinfo);
		}
		else
		{
			next_recv_sequence_nr = argr->cm.cl.alarm[prio].recv_sequence_nr;
			CM_ALARM_SEQUENCE_NR_INCREMENT (next_recv_sequence_nr);

			/* keep check, an alarm is running */
			if (cm_is_not_null (argr->cm.cl.alarm[prio].early_alarm)
				/*AP01287831*/
				|| (prio == CM_ALARM_PRIORITY_LOW   &&  CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP))
				|| (prio == CM_ALARM_PRIORITY_HIGH  &&  CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP))
				/* todo: check rosal_alarm; check prm_alarm; ACHTUNG!prio; wurde bisher auch nicht geprüft */)
			{
				CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u) previous alarm running, protocol fault; alarm-prio(%u) seq(%u) wf0(0x%x) wf1(0x%x)"
					, argr->device_nr
					, prio, alarm_sequence, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
				);
				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
				cm_cl_arsm_down (argr, &dinfo);
			}
			else if (argr->cm.cl.alarm_enabled_state == CM_CL_ALARM_ENABLED_IGNORE)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u) currently in alarm_state(IGNORE), arState(%u) wf0(0x%x) wf1(0x%x)"
					, argr->device_nr
					, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
				);
				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
				cm_cl_arsm_down (argr, &dinfo);
			}
			else if (! cm_cl_alarm_maintenance_is_consistence (acp_al->user_tag, acp_al->user_length, acp_al->user_data, &maintenance_status))
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u): maintenace-alarm not consistence, alarm_type(0x%x)"
					, argr->device_nr, acp_al->alarm_type
					);

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
				cm_cl_arsm_down (argr, &dinfo);
			}
			else if (
				(acp_al->alarm_type == CM_ALARM_TYPE_PLUG  &&  ! cm_cl_prm_request_can_queue (argr, CM_CL_PRMMODE_PLUG))
			||	(acp_al->alarm_type == CM_ALARM_TYPE_RELEASED  &&  ! cm_cl_prm_request_can_queue (argr, CM_CL_PRMMODE_PLUG)))
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u): sequence error: cann't queue alarm_type(0x%x), wf0(0x%x) wf1(0x%x)"
					, argr->device_nr, acp_al->alarm_type
					, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
				);

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
				cm_cl_arsm_down (argr, &dinfo);
			}
			else
			{
				LSA_BOOL  valid;

				/* Check for a device alarm */
				if (cm_cl_alarm_is_device_alarm (argr, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
					, acp_al->module_ident, acp_al->submodule_ident, acp_al->alarm_type, &valid))
				{
					if (! valid)
					{
						CM_CL_TRACE_08(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "dev(%u): invalid alarm_type(0x%x) api(%u) slot(%u) subslot(0x%x) module_ident(0x%x) submodule_ident(0x%x) rs_info(0x%x)"
							, argr->device_nr
							, acp_al->alarm_type, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
							, acp_al->module_ident, acp_al->submodule_ident, argr->rs_info
						);
						pnio_err_do_reject = CM_PNIO_ERR_AlarmTypeNotSupported;
					}
					else
					{
						pnio_err_do_reject = CM_PNIO_ERR_NONE;
					}
				}
				else
				{
					CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = LSA_NULL;

					/* check if alarm is related to ar */
					if (! cm_cl_alarm_param_ok (argr, acp_al->alarm_type
						, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr, &sub))
					{
						CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "dev(%u): unknown submodule: alarm_type(0x%x) api(%u) slot(%u) subslot(%u) module_ident(0x%x) submodule_ident(0x%x)"
							, argr->device_nr
							, acp_al->alarm_type, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
							, acp_al->module_ident, acp_al->submodule_ident
							);

						CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT));
						cm_cl_arsm_down (argr, &dinfo);
						return acp;
					}
					else
					{
						/* check if alarm is to be rejected */
						if (! cm_cl_alarm_type_is_supported (argr->ar_com.channel, acp_al->alarm_type))
						{
							CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "dev(%u): internal reject, reject alarm_type(0x%x)  alarm_reject(0x%x)"
								, argr->device_nr
								, acp_al->alarm_type, argr->ar_com.channel->usr.cl.alarm_reject
								);
							pnio_err_do_reject = CM_PNIO_ERR_AlarmTypeNotSupported;
						}
						/* check if submodule.state is properly / substituted */
						else if (! cm_cl_alarm_type_and_submod_state_matches (acp_al->alarm_type, sub))
						{
							CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "dev(%u): alarm_type(0x%x) and submod.state(0x%x) doesn't match, submodule(%u/%u/%u)"
								, argr->device_nr
								, acp_al->alarm_type, cm_is_not_null (sub) ? sub->cl.real_sub_state : 0xFFFF
								, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
								);
							pnio_err_do_reject = CM_PNIO_ERR_AlarmAndWrongSubmoduleState;
						}
						/* check if ROSAL does overlap with PrmUpdate */
						else if (cm_cl_prm_update_reject_alarm (argr, acp_al))
						{
							if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP) && cm_is_null (argr->cm.cl.rosal_park)) /* TIA 2838473 */
							{
								CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
									, "dev(%u): alarm_type(0x%x) submodule(%u/%u/%u), parking, wf0(0x%x) wf1(0x%x)"
									, argr->device_nr, acp_al->alarm_type
									, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
									, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
								);
								argr->cm.cl.rosal_park = acp;
								return LSA_NULL;
							}
							else
							{
								CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
									, "dev(%u): alarm_type(0x%x) submodule(%u/%u/%u), rejected because of running/queued PrmUpdate-Sequence, wf0(0x%x) wf1(0x%x)"
									, argr->device_nr, acp_al->alarm_type
									, acp_al->api, acp_al->slot_nr, acp_al->subslot_nr
									, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
								);
								pnio_err_do_reject = CM_PNIO_ERR_AlarmAndWrongSubmoduleState;
							}
						}
						else
						{
							pnio_err_do_reject = CM_PNIO_ERR_NONE;
						}
					}
				}
				/* prepare next expected */
				argr->cm.cl.alarm[prio].recv_sequence_nr = next_recv_sequence_nr;

				/* prepare alarm.ind to user */
				if (pnio_err_do_reject == CM_PNIO_ERR_NONE)
				{
					CM_UPPER_RQB_PTR_TYPE  upper;
					CM_UPPER_ALARM_PTR_TYPE  alarm;

					/* get upper */
					upper = cm_cl_dev_alarm_get (argr->cm.cl.dev, prio);
					if (cm_is_null (upper))
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "dev(%u) !upper-rqb", argr->device_nr
							);

						CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
						cm_cl_arsm_down (argr, &dinfo);
					}
					else
					{
						CM_RQB_SET_OPCODE (upper, CM_OPC_CL_ALARM_IND);

						alarm = upper->args.cl.alarm;

						alarm->device_nr	= argr->device_nr;
						alarm->ar_nr		= argr->ar_nr;
						alarm->session_key	= argr->session_key;

						CmListInitialize (& alarm->u.cl.diff_list);
						alarm->u.cl.mod_diff_block_length = 0;
						alarm->u.cl.mod_diff_block = LSA_NULL;

						cm_acp_alarm_copy_ind (upper->args.cl.alarm, acp, maintenance_status);

						CM_ASSERT (cm_is_null (argr->cm.cl.alarm[prio].early_alarm));
						argr->cm.cl.alarm[prio].early_alarm = upper;
						CM_ASSERT (cm_is_null (argr->cm.cl.alarm[prio].acp_ind));
						argr->cm.cl.alarm[prio].acp_ind = acp; /* keep acp-alarm until cm-alarm.rsp */

						cm_cl_alarm_indication (argr->ar_com.channel, argr, prio);
						return LSA_NULL;
					}
				}
				/* reject alarm.ind */
				else
				{
					if (cm_is_not_null (argr->cm.cl.alarm[prio].ack))
					{
						CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "dev(%u): a ack is running, alarm_type(0x%x)"
							, argr->device_nr, argr->cm.cl.alarm[prio].ack->args->alarm.data.alarm_type
							);

						CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT), pnio_err_do_reject);
						cm_cl_arsm_down (argr, &dinfo);
					}
					else
					{
						cm_acp_alarm_copy_ack (argr->cm.cl.alarm[prio].ack_rqb, acp_al, LSA_NULL, pnio_err_do_reject);
						argr->cm.cl.alarm[prio].ack = argr->cm.cl.alarm[prio].ack_rqb;

						/* first re-provide, then send alarm-ack */
						cm_acp_request_lower (argr->ar_com.channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, acp);
						cm_cl_alarm_set_wait_for (argr, prio);
						cm_acp_alarm_ack_send (argr, argr->cm.cl.alarm[prio].ack_rqb);
						return LSA_NULL;
					}
				}
			}
		}
	}

	return acp;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_send_next (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio
	)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): prio(%u) upper(%08x)"
		, argr->device_nr
		, prio
		, upper
		);

	if (cm_is_null (upper)) {
		return;
	}

	CM_RQB_SET_RESPONSE (upper, CM_CL_ALPMI_INIT); /* state of alarm_send.rqb */

	acp = cm_acp_alloc_rqb (channel);
	if (cm_is_null (acp)) {
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALSND));
		cm_cl_arsm_down (argr, &dinfo);
		/* cancel alarm-send-queue in cm_cl_ar_com_down */
		return;
	}

	CM_ASSERT (upper->args.cl.alarm_send->alarm_priority == prio);

	upper->args.cl.alarm_send->alarm_sequence = argr->cm.cl.alarm[prio].send_sequence_nr;
	CM_ALARM_SEQUENCE_NR_INCREMENT (argr->cm.cl.alarm[prio].send_sequence_nr);

	if (CM_ALARM_IS_DIAG_GROUP(upper->args.cl.alarm_send->alarm_type))
	{
		upper->args.cl.alarm_send->ar_diagnosis_state = argr->cm.cl.station_problem ? CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO;
	}

	cm_acp_alarm_send (argr, upper->args.cl.alarm_send
		, upper->args.cl.alarm_send->alarm_tag
		, upper->args.cl.alarm_send->alarm_data_length
		, upper->args.cl.alarm_send->alarm_data
		, acp
		);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  prio;
	LSA_UINT16  rsp;
	LSA_UINT16  state;

	CM_ASSERT (cm_is_not_null (argr));
	CM_ASSERT (cm_is_not_null (acp));

	rsp = CM_ACP_GET_RESPONSE (acp);
	prio = acp->args->alarm.data.priority;
	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	if (cm_is_null (upper)) {
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper, acp(0x%08x)", acp);
		cm_acp_free_rqb (channel, acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALSND));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	cm_acp_free_rqb (channel, acp);

	state = CM_RQB_GET_RESPONSE (upper);
	state = state | CM_CL_ALPMI_SEND_DONE;
	CM_RQB_SET_RESPONSE (upper, state);

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rsp(%u) upper(%08x) state(%x)"
		, argr->device_nr
		, rsp
		, upper
		, state
		);

		/* case: good */
	if (rsp == ACP_OK) {
		if ((state & CM_CL_ALPMI_SEND_DONE)  &&  (state & CM_CL_ALPMI_ACK_DONE)) {
			CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);

			cm_callback (channel, CM_OK, & upper);

				/* kick-next */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: dev should decide, not iter here")
#endif /* CM_MESSAGE */
			cm_cl_alarm_send_next (channel, argr, prio);
			return;
		}
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "wf ack.done"
			);
		return;
	}

		/* case: bad */
		/* ar goes down */
	CM_ASSERT (cm_ar_com_is_down (argr));

	CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rsp(%u) upper(0x%08x)", rsp, upper);

	if (rsp == ACP_ERR_ASRT  ||  rsp == ACP_OK_CANCEL) {
		cm_callback (channel, CM_ERR_SESSION, & upper);
	}
	else {
		cm_callback (channel, CM_ERR_LOWER_LAYER, & upper);
	}
	/* cancel alarm-send-queue in cm_cl_ar_com_down */
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_ack_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prio,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  state;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_ASSERT (cm_is_not_null (argr));
	CM_ASSERT (cm_is_not_null (acp));

	CM_ASSERT (prio == CM_ALARM_PRIORITY_LOW || prio == CM_ALARM_PRIORITY_HIGH);

	upper = CmListFirst (& argr->cm.cl.alarm[prio].alarm_list, CM_UPPER_RQB_PTR_TYPE);
	if (cm_is_null (upper)) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper, acp(0x%08x)", acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALACK));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	if (! cm_acp_alarm_ack_matches (upper->args.cl.alarm, acp, &upper->args.cl.alarm->cm_pnio_err)) {
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!matches, acp(0x%08x)", acp);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ALACK));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	state = CM_RQB_GET_RESPONSE (upper);
	state = state | CM_CL_ALPMI_ACK_DONE;
	CM_RQB_SET_RESPONSE (upper, state);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): upper(%08x) state(%x)"
		, argr->device_nr
		, upper
		, state
		);

	if ((state & CM_CL_ALPMI_SEND_DONE)  &&  (state & CM_CL_ALPMI_ACK_DONE)) {

		CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);
		cm_callback (channel, CM_OK, & upper);

				/* kick-next */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: dev should decide, not iter here")
#endif /* CM_MESSAGE */
		cm_cl_alarm_send_next (channel, argr, prio);
	}
	else {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, ".cnf-sequence-bug: wf send.done"
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_alarm_ack_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp
)
{
	LSA_UINT16  prio;

	CM_ASSERT (cm_is_not_null (argr));
	CM_ASSERT (cm_is_not_null (acp));

	prio = acp->args->alarm.data.priority;
	argr->cm.cl.alarm[prio].ack = LSA_NULL;

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "rsp(%u) prio(%u)", CM_ACP_GET_RESPONSE (acp), prio);

	cm_cl_alarm_clear_wait_for(argr, prio); /* TIA 2093279: move clear to acp-alarm-ack.cnf */

	if (cm_ar_com_is_down(argr))
	{
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF);
		cm_cl_arsm(argr);
	}
	else
	{
		/* check for queued alarm.ind */
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF))
		{
			cm_cl_prm_trigger (argr, CM_CL_WF_PRM_PLUG_ACK_CNF);
		}
		cm_cl_alarm_indication (channel, argr, prio);
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
