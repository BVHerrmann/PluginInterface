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
/*  F i l e               &F: cm_pd5.c                                  :F&  */
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

#define LTRC_ACT_MODUL_ID  805
#define CM_MODULE_ID       805

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

static LSA_UINT16
cm_pd_diag_equalize (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_ALARM_PTR_TYPE  param,
	LSA_BOOL  with_header,
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list,
	CM_LIST_ENTRY_PTR_TYPE  merge_to
);

static LSA_UINT16  cm_pd_diag_check_add_or_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  nr_of_elems,
	CM_PD_SUBMOD_DIAG_TYPE  *diag_elems,
	LSA_UINT16  chpr_specifier
	);


/*===========================================================================*/
/*=== alarm initiator init ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_almi_init (
	CM_CHANNEL_PTR_TYPE	channel
	)
{
#if CM_CFG_MAX_SERVERS

	channel->usr.pd.almi.server_allowed = LSA_FALSE;
	channel->usr.pd.almi.server_max_alarm_data_length = 0; /* sanity */

	{
		CM_UPPER_RQB_PTR_TYPE  alarm;
		alarm = cm_upper_alloc_rqb (channel);

		if (cm_is_null(alarm))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "no mem, cm_upper_alloc_rqb()");
			CM_FATAL ();
		}

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& alarm->args.sv.ar_alarm_send->alarm_data)
			, cm_null_user_id, CM_ALARM_DATA_LENGTH_MAX, channel->sysptr);
		if (cm_is_null (alarm->args.pd.alarm->alarm_data))
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "no mem, max_alarm_data_length(%u)"
				, CM_ALARM_DATA_LENGTH_MAX);
			CM_FATAL ();
		}
		channel->usr.pd.almi.server_alarm = alarm;
	}
#endif

	channel->usr.pd.almi.user_allowed = LSA_FALSE; /*AP00942517; was true*/
	channel->usr.pd.almi.user_sequence_nr = 0;

	channel->usr.pd.almi.appl_ready_ind_pending = CM_ERR_NOT_YET; /* TFS 1061676: was 0 */
	channel->usr.pd.almi.appl_ready_rsp_pending = LSA_FALSE;
}


/*===========================================================================*/
/*=== alarm initiator undo init =============================================*/
/*===========================================================================*/
LSA_VOID
cm_pd_almi_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
#if CM_CFG_MAX_SERVERS
	CM_UPPER_RQB_PTR_TYPE  alarm = channel->usr.pd.almi.server_alarm;

	if (cm_is_not_null (alarm))
	{
		if (cm_is_not_null (alarm->args.sv.ar_alarm_send->alarm_data))
		{
			LSA_UINT16  rc;

			CM_FREE_UPPER_MEM (& rc, alarm->args.sv.ar_alarm_send->alarm_data, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		cm_upper_free_rqb (channel, alarm);

		channel->usr.pd.almi.server_alarm = LSA_NULL;
	}
#else
	LSA_UNUSED_ARG (channel);
#endif
}


/*===========================================================================*/
/*=== build alarm ===========================================================*/
/*===========================================================================*/
static LSA_VOID
cm_pd_alarm_build (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_ALARM_PTR_TYPE  param,
	LSA_BOOL  with_header,
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_PD_DIAG_CONST_PTR_TYPE  diag
) {
	CM_UPPER_MEM_U8_PTR_TYPE  alarm_data;
	LSA_UINT16  alarm_data_len;
	LSA_UINT16  block_length_offset;
	LSA_BOOL diag_status;

	switch (diag->channel_properties & CM_CHPR_SPEC_MASK)
	{
	case CM_CHPR_SPEC_APP:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"appearing-alarm_type(%u): with_header(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, with_header, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	case CM_CHPR_SPEC_DIS:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"disappearing-alarm_type(%u): with_header(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, with_header, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	case CM_CHPR_SPEC_DBO:
		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			,"disappearing-but-other-remain-alarm_type(%u): with_header(%u) submodule(0/%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x)"
			, diag->alarm_type
			, with_header, port->addr.slot_nr, port->addr.subslot_nr
			, diag->channel_error_type, diag->ext_channel_error_type
			, diag->channel_properties
			);
		break;

	default:
		CM_FATAL1 (diag->channel_properties);
		return;
	}

	/***/

	CM_ASSERT (cm_is_not_null (param));
	CM_ASSERT (cm_is_not_null (param->alarm_data));

	alarm_data     = param->alarm_data;
	alarm_data_len = 0;

	/***/

	param->device_nr              = 0;  /* user: don't evaluate */
	param->ar_nr                  = 0;  /* user: don't evaluate */
	param->session_key            = 0;  /* user: don't evaluate */

	param->api                    = 0;
	param->slot_nr                = port->addr.slot_nr;
	param->subslot_nr             = port->addr.subslot_nr;

	param->alarm_type             = diag->alarm_type;

	param->alarm_priority         = CM_ALARM_PRIORITY_LOW;
	param->alarm_sequence         = 0;			/* don't care */
	param->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO;		/* channel diagnosis available */
	param->diag_generic_available = CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO;		/* don't care */
	param->diag_submod_available  = CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;	/* diagnosis available*/
	param->ar_diagnosis_state     = CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO;			/* don't care */
	param->reserved               = 0;			/* don't care */
	param->mod_ident              = port->addr.mod_ident;	/* AP00329730 */
	param->sub_ident              = port->addr.sub_ident;	/* AP00329730 */
	param->cm_pnio_err            = 0;			/* don't care */

	cm_pd_diag_maint_qual (port, &diag_status, & param->maintenance_status);
	param->diag_submod_available = diag_status ? CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;

	if (param->diag_submod_available == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES)
	{
		param->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES;
	}

	/*
	//	AlarmNotification-PDU ::=
	//		BlockHeader, AlarmType, API, SlotNumber, SubslotNumber
	//		, ModuleIdentNumber, SubmoduleIdentNumber, AlarmSpecifier
	//		, [MaintenanceItem], [AlarmItem]
	//
	//	MaintenanceItem ::=
	//		BlockType(0x0F00), BlockLength, BlockVersion(1.0), ReservedU16, MaintenanceStatusU32
	//
	//	AlarmItem ::=
	//		UserStructureIdentifier(=0x8002), ExtChannelDiagnosisData*
	//
	//	ExtChannelDiagnosisData ::=
	//		ChannelNumber(0x8000), ChannelProperties, ChannelErrorType, ExtChannelErrorType, ExtChannelAddValue
	*/

	/* if CM-PD-alarm: simulate alarm-PDU-Header */

	if (with_header)
	{
		LSA_UINT16 alarm_specifier;

		param->alarm_sequence = channel->usr.pd.almi.user_sequence_nr; /* AP00632258 */
		CM_ALARM_SEQUENCE_NR_INCREMENT (channel->usr.pd.almi.user_sequence_nr);

		ACP_SET_ALARM_SPECIFIER(& alarm_specifier,
			param->alarm_sequence,
			(param->diag_channel_available == CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES),
			(param->diag_generic_available == CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES),
			(param->diag_submod_available == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES),
			(param->ar_diagnosis_state == CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES)
		);

		CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0002); /* ACP_ALARM_BLOCK_REQUEST_LOW */
		alarm_data_len += 2;

		block_length_offset = alarm_data_len;
		CM_PUT16_HTON (alarm_data, alarm_data_len, 0xCCCC); /* block_length, see below */
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0100); /* ACP_ALARM_BLOCK_VERSION */
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->alarm_type);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->api);
		alarm_data_len += 4;

		CM_PUT16_HTON (alarm_data, alarm_data_len, param->slot_nr);
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, param->subslot_nr);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->mod_ident);
		alarm_data_len += 4;

		CM_PUT32_HTON (alarm_data, alarm_data_len, param->sub_ident);
		alarm_data_len += 4;

		CM_PUT16_HTON (alarm_data, alarm_data_len, alarm_specifier);
		alarm_data_len += 2;

		if (param->maintenance_status != 0)
		{
			param->alarm_tag = CM_ALARM_USI_MAINTENANCE;

			CM_PUT16_HTON (alarm_data, alarm_data_len, CM_ALARM_USI_MAINTENANCE);
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, CM_BLOCK_TYPE_MAINTENANCE);
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 2+2+4); /* block_length */
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0x0100); /* block_version */
			alarm_data_len += 2;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0); /* reserved */
			alarm_data_len += 2;

			CM_PUT32_HTON (alarm_data, alarm_data_len, param->maintenance_status);
			alarm_data_len += 4;

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8002); /* UserStructureIdentifier::ExtChannelDiagnosis */
			alarm_data_len += 2;
		}
		else
		{
			param->alarm_tag = 0x8002; /* UserStructureIdentifier::ExtChannelDiagnosis */

			CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8002); /* UserStructureIdentifier::ExtChannelDiagnosis */
			alarm_data_len += 2;
		}
	}
	else
	{
		block_length_offset = 0;
		param->alarm_tag = 0x8002; /* UserStructureIdentifier::ExtChannelDiagnosis */
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("REVIEW: when to use ChannelProperties.Specifier 'All subsequent disappears'?")
	/* see TIA 1737456 */
#endif /* CM_MESSAGE */

	if (port->diag.server_in_sync || with_header)
	{
		/*
		//	ExtChannelDiagnosisData ::=
		//		ChannelNumber(0x8000), ChannelProperties, ChannelErrorType, ExtChannelErrorType, ExtChannelAddValue
		*/

		CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8000/* ChannelNumber - Submodule */);
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->channel_properties);
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->channel_error_type);
		alarm_data_len += 2;

		CM_PUT16_HTON (alarm_data, alarm_data_len, diag->ext_channel_error_type);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, diag->ext_channel_add_value);
		alarm_data_len += 4;
	}

	/***/

	if (alarm_data_len > max_alarm_data_length)
	{
		CM_FATAL1 (alarm_data_len); /* caller must check */
	}

	if (block_length_offset != 0)
	{
		CM_PUT16_HTON (alarm_data, block_length_offset, CM_CAST_U16(alarm_data_len - (2+2))); /* block without (block_type,block_length) */
	}

	param->alarm_data_length = alarm_data_len;
}


/*===========================================================================*/
/*=== Alarm Provide =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_alarm_provide_event (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	if (channel->usr.pd.req.alarm_ind_count == 0)
	{
		if (CM_PD_CURRENT_IS_DELETE (channel))
		{
			cm_pd_delete (channel, channel->usr.pd.req.current);
			return;
		}
	}

	if (channel->usr.pd.req.wait_for & CM_PD_WF_PULLPLUG_ALARM_RES)
	{
		channel->usr.pd.req.wait_for &= ~CM_PD_WF_PULLPLUG_ALARM_RES;

		cm_pd_submodule_control(channel, channel->usr.pd.req.current);

		return;
	}

	if (channel->usr.pd.almi.user_allowed)
	{
		LSA_UINT16  i;

		for (i = 0; i <= channel->usr.pd.port_count; ++i)
		{
			CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, i);

			cm_pd_diag_update_check (channel, port);
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_alarm_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_ASSERT (
		CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PROVIDE_ALARM
		|| CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_ALARM_IND
		|| CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_ALARM_RSP
	);
	CM_ASSERT (rb->args.pd.alarm->alarm_priority == CM_ALARM_PRIORITY_LOW);


	if (cm_is_not_null (rb->args.pd.alarm->alarm_data))
	{
		LSA_UINT16 rc;

			/* must be alloced buffer from alarm_ind */
		CM_FREE_UPPER_MEM (& rc, rb->args.pd.alarm->alarm_data, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);

		rb->args.pd.alarm->alarm_data = LSA_NULL;
		rb->args.pd.alarm->alarm_data_length = 0;
	}


	CM_RQB_SET_OPCODE(rb, CM_OPC_PD_PROVIDE_ALARM);

	CmListInsertTail (& channel->usr.pd.req.alarmQ, (CM_LIST_ENTRY_PTR_TYPE)rb);
}

/*----------------------------------------------------------------------------*/

CM_UPPER_RQB_PTR_TYPE
cm_pd_alarm_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
	)
{
	if (CmListIsEmpty (& channel->usr.pd.req.alarmQ))
	{
		return LSA_NULL;
	}
	else
	{
		CM_UPPER_RQB_PTR_TYPE  rb;

		CmListRemoveHead (& channel->usr.pd.req.alarmQ, rb, CM_UPPER_RQB_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (rb));

		return rb;
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_alarm_user_enable (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  user_enable
) {
	if (user_enable)
	{
		if (channel->state == CM_PD_DELETING)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "want PD-user alarm.ind but also want to close, do not enable"
				);
		}
		else if (channel->state == CM_PD_REMAPPING)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "want PD-user alarm.ind but also want to remap, do not enable"
				);
		}
		else
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "enable PD-user alarm.ind");
			channel->usr.pd.almi.user_allowed = LSA_TRUE;
		}
		cm_pd_alarm_provide_event (channel);
	}
	else
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "disable PD-user alarm.ind"
			);

		channel->usr.pd.almi.user_allowed = LSA_FALSE;
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_diag_usr_copy_to_user_diag (
	CM_CHANNEL_CONST_PTR_TYPE  channel
) {
	LSA_UINT16  port_id;

	for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
	{
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, port_id);
		CM_PD_DIAG_PTR_TYPE  diag;

		while (! CmListIsEmpty (& port->diag.diag_list_user))
		{
			diag = CmListFirst (& port->diag.diag_list_user, CM_PD_DIAG_PTR_TYPE);
			cm_pd_diag_elem_remove (& port->diag.diag_list_user, diag);
		}

		if (CM_PDEV_STATE_IS_PLUGGED (port->addr.pdev_properties))
		{
			for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE))
			{
				CM_PD_DIAG_PTR_TYPE  diag_new = cm_pd_diag_elem_fill__nn (0/*alloc*/, diag->diag_tag
					, diag->alarm_type, diag->channel_properties
					, diag->channel_error_type, diag->ext_channel_error_type, diag->ext_channel_add_value
					);
				cm_pd_diag_elem_add (& port->diag.diag_list_user, diag_new);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_diag_sv_copy_to_server_diag (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_PD_PORT_CONST_PTR_TYPE  trigger_port
) {
	LSA_UINT16 port_id;

	for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
	{
		CM_PD_DIAG_PTR_TYPE diag;
		CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

		if (cm_is_null (trigger_port)  ||  (trigger_port == port))
		{
			while( !CmListIsEmpty(&port->diag.diag_list_server) )
			{
				diag = CmListFirst(&port->diag.diag_list_server, CM_PD_DIAG_PTR_TYPE);
				cm_pd_diag_elem_remove (&port->diag.diag_list_server, diag);
			}

			for( CmListForeach(diag, &port->diag.diag_list, CM_PD_DIAG_PTR_TYPE) )
			{
				CM_PD_DIAG_PTR_TYPE  diag_new = cm_pd_diag_elem_fill__nn(0/*alloc*/, diag->diag_tag
					, diag->alarm_type, diag->channel_properties
					, diag->channel_error_type, diag->ext_channel_error_type, diag->ext_channel_add_value
					);
				cm_pd_diag_elem_add (&port->diag.diag_list_server, diag_new);
			}
		}
	}
}


#if CM_CFG_MAX_SERVERS
/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_sv_update_check (
	CM_CHANNEL_PTR_TYPE  channel
) {
	LSA_UINT16 port_id;

	if (channel->usr.pd.almi.server_allowed)
	{
		/* AP00678581 */
		for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

			cm_pd_diag_update_check (channel, port);
		}
	}
}
#endif

/*===========================================================================*/
/*=== DIAG LIST =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_diag_update_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	LSA_UNUSED_ARG (channel);

	if (port->diag.update_allowed)
	{
		port->diag.update_allowed = LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_update_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
) {
	LSA_UNUSED_ARG (channel);

	if (! port->diag.update_allowed)
	{
#if CM_DEBUG
		CM_PD_DIAG_PTR_TYPE  diag;
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "diag_update_done for port(%u)", port->port_id
			);
		for (CmListForeach (diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE))
		{
			CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "   alarm_type(%u) ch_err(0x%x) ext_ch_err(0x%x) ch_prop(0x%x) add_value(0x%x)"
				, diag->alarm_type
				, diag->channel_error_type, diag->ext_channel_error_type
				, diag->channel_properties
				, diag->ext_channel_add_value
				);
		}
#endif

		port->diag.update_allowed = LSA_TRUE;

		/***/
#if CM_CFG_MAX_SERVERS
		cm_sv_pdev_diag_changed(channel);
#endif

		/***/
		cm_pd_diag_update_check (channel, port);
	 }
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_pd_diag_update_all_done (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	LSA_UINT16  port_id;
	for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
	{
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, port_id);

		if (! port->diag.update_allowed)
		{
			cm_pd_diag_update_done (channel, port);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_update_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
	)
{
	if (! port->diag.update_allowed)
	{
		return;
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: argument which tell us the checking part: sv, pd or both; see caller")
#endif /* CM_MESSAGE */


#if CM_CFG_MAX_SERVERS
	if (channel->usr.pd.pdprm.occupant == CM_PD_OCCUPANT_SV)
	{
		CM_CHANNEL_PTR_TYPE  sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);

		if (cm_is_null (sv_channel)) { CM_FATAL(); }

		if (channel->usr.pd.almi.server_allowed)
		{
			if (cm_is_null (channel->usr.pd.almi.server_alarm))
			{
				LSA_UINT16 pd_diag_count, sv_diag_count;

				CmListLength (&port->diag.diag_list, &pd_diag_count);
				CmListLength (&port->diag.diag_list_server, &sv_diag_count);

				if (sv_diag_count == 0 && pd_diag_count > 0)
				{
					port->diag.server_in_sync = LSA_FALSE;
				}

				CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "server_allowed for PDevAlarm.ind, but waiting for cm_pd_opc_sv_ar_alarm_pdev_group_rsp(), pd_diag_count(%u), sv_diag_count(%u), server_in_sync(%u)"
					, pd_diag_count, sv_diag_count, port->diag.server_in_sync
					);
			}
			else
			{
				LSA_UINT16  rc;
				CM_UPPER_RQB_PTR_TYPE  alarm = channel->usr.pd.almi.server_alarm;

				alarm->args.sv.ar_alarm_send->alarm_data_length = 0; /* but alarm_data != 0 */

				rc = cm_pd_diag_equalize (channel
					, alarm->args.sv.ar_alarm_send, LSA_FALSE, channel->usr.pd.almi.server_max_alarm_data_length
					, port, &port->diag.diag_list, &port->diag.diag_list_server
					);

				if( rc == CM_OK )
				{
					channel->usr.pd.almi.server_alarm = LSA_NULL;

					CM_RQB_SET_USER_ID_PTR(alarm, LSA_NULL); /* not used */
					CM_RQB_SET_OPCODE (alarm, CM_OPC_SV_AR_ALARM_PDEV_GROUP);
					CM_RQB_SET_HANDLE(alarm, sv_channel->my_handle);

					cm_pd_req_callback(channel, CM_OK, alarm);
				}
				else
				{
					/* no difference found */
					CM_ASSERT (rc == CM_OK_CANCELLED);
				}
			}
		}
	}
#endif

	if (channel->usr.pd.almi.user_allowed)
	{
		if (CM_PDEV_STATE_IS_PULLED (port->addr.pdev_properties))
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "port(%u): port is pulled", port->port_id);
			return;
		}
		else
		{
			CM_UPPER_RQB_PTR_TYPE  alarm = cm_pd_alarm_get (channel);

			if (cm_is_not_null (alarm))
			{
				CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& alarm->args.pd.alarm->alarm_data)
					, cm_null_user_id, channel->acp.max_alarm_data_length, channel->sysptr
					);
				if (cm_is_null (alarm->args.pd.alarm->alarm_data))
				{
					CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "no alarm_mem, size(%u)", channel->acp.max_alarm_data_length);
					cm_pd_alarm_provide (channel, alarm);
					alarm = LSA_NULL;
				}
			}

			if (cm_is_null (alarm))
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW, "no indication resource for alarm to user");
			}
			else
			{
				LSA_UINT16  rc;

				alarm->args.pd.alarm->alarm_data_length = 0; /* but alarm_data != 0 */

				rc = cm_pd_diag_equalize (channel
					, alarm->args.pd.alarm, LSA_TRUE, channel->acp.max_alarm_data_length
					, port, & port->diag.diag_list, & port->diag.diag_list_user
					);

				if (rc == CM_OK)
				{
					channel->usr.pd.req.alarm_ind_count += 1;

					CM_RQB_SET_OPCODE (alarm, CM_OPC_PD_ALARM_IND);

					cm_callback (channel, CM_OK, & alarm);
					/* see cm_pd_alarm_provide() */
				}
				else
				{
					CM_ASSERT (rc == CM_OK_CANCELLED);
					CM_FREE_UPPER_MEM (& rc, alarm->args.pd.alarm->alarm_data, channel->sysptr);
					CM_ASSERT (rc == LSA_RET_OK);
					alarm->args.pd.alarm->alarm_data = LSA_NULL;
					cm_pd_alarm_provide (channel, alarm);
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
*	\brief update the merge_to list until this list is equal to diag_list
*
*	\param alarm	[var] internal alarm-rqb for sending an alarm
*	\param port		the current port
*
*	\return
*		CM_OK			one merge had done, one alarm is ready to send
*		CM_OK_CANCELED	the lists are equal, not alarm must be send
*/
static LSA_UINT16
cm_pd_diag_equalize (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_ALARM_PTR_TYPE  param,
	LSA_BOOL  with_header,
	LSA_UINT16  max_alarm_data_length,
	CM_PD_PORT_PTR_TYPE  port,
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list, /**< a list with all added diagnosis elementes */
	CM_LIST_ENTRY_PTR_TYPE  merge_to_list /**< this list must be equal to the diag_list */
) {
	CM_PD_DIAG_PTR_TYPE  i_diag;

	CM_PD_DIAG_PTR_TYPE  add_to_merge = LSA_NULL;
	CM_PD_DIAG_PTR_TYPE  remove_from_merge = LSA_NULL;

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: Sammelmeldung, see 22. conclave")
/*
Sammelmeldung:
Die Subkomponenten liefern alle Aenderungen in EINER Indikation an den CMPD.
Der CMPD gene-riert aus dieser Indikation einen ChannelErrorType-richtigen(AlarmType)
Alarm abhaengig von der Kapazitaet des Transportkanals. Sollten mehrere ChannelErrorTypes
in dieser Indication geliefert werden, so werden auch mehrere ChannelErrorType-richtige
Alarme geliefert.
Allgemein ist dabei eine Zusammenfassung der Diagnoseobjekte ueber mehrere Indikations zur
Re-duzierung der Alarmlast sinnvoll.
*/
#endif /* CM_MESSAGE */

	for (CmListForeach (i_diag, diag_list, CM_PD_DIAG_PTR_TYPE))
	{
		CM_PD_DIAG_PTR_TYPE  e = cm_pd_diag_elem_lookup (merge_to_list, i_diag);

		if (cm_is_not_null (e))
		{
			/* previously merged and send */

			if (cm_pd_diag_elem_has_equal_add_value (i_diag, e))
			{
				/* ok, nothing to add+send */
			}
			else
			{
				/* add_value has changed, must add+send again */
				add_to_merge = i_diag;
				break;
			}
		}
		else
		{
			add_to_merge = i_diag;
			break;
		}
	}

	if (cm_is_null (add_to_merge))
	{
		for (CmListForeach (i_diag, merge_to_list, CM_PD_DIAG_PTR_TYPE))
		{
			CM_PD_DIAG_PTR_TYPE  e = cm_pd_diag_elem_lookup (diag_list, i_diag);

			if (cm_is_null (e))
			{
				remove_from_merge = i_diag;
				break;
			}
		}
	}

	if (!with_header /* server, TIA 2898601 */)
	{
		if (!port->diag.server_in_sync)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "port(%u): not !server_in_sync", port->port_id);

			if (cm_is_null (add_to_merge) && cm_is_null (remove_from_merge))
			{
					/* both lists are empty but a glitch has happened */
				CM_PD_DIAG_TYPE disappear;

				disappear.alarm_type = CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS;
				disappear.channel_properties = CM_CHPR_SPEC_DIS;
				disappear.channel_error_type = 0;
				disappear.ext_channel_error_type = 0;
				disappear.ext_channel_add_value = 0;

				cm_pd_alarm_build (channel, param, with_header, max_alarm_data_length, port, &disappear);

				port->diag.server_in_sync = LSA_TRUE;
				return CM_OK;
			}
			else
			{
				port->diag.server_in_sync = LSA_TRUE;
			}
		}
	}

	if (cm_is_not_null (add_to_merge))
	{
		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "port(%u): add_to_merge(0x%x/0x%x)"
			, port->port_id, add_to_merge->channel_error_type, add_to_merge->ext_channel_error_type
			);

			/* alarm add_to_merge */
		cm_pd_alarm_build (channel, param, with_header, max_alarm_data_length, port, add_to_merge);

			/* add_to_merge */
		{
			CM_PD_DIAG_PTR_TYPE diag_new = cm_pd_diag_elem_fill__nn (0/*alloc*/, add_to_merge->diag_tag
				, add_to_merge->alarm_type, add_to_merge->channel_properties
				, add_to_merge->channel_error_type, add_to_merge->ext_channel_error_type
				, add_to_merge->ext_channel_add_value
				);
			CM_PD_DIAG_PTR_TYPE  e = cm_pd_diag_elem_lookup (merge_to_list, diag_new);
			if (cm_is_not_null (e)) /* only add_value has changed */
			{
				cm_pd_diag_elem_remove (merge_to_list, e);
			}
			cm_pd_diag_elem_add (merge_to_list, diag_new);
		}

		return CM_OK;
	}

	else if (cm_is_not_null (remove_from_merge))
	{
		CM_PD_DIAG_PTR_TYPE  more_diag = LSA_NULL;

		for (CmListForeach (more_diag, diag_list, CM_PD_DIAG_PTR_TYPE))
		{
			if ((more_diag->channel_properties & CM_CHPR_MAINT_MASK) == (remove_from_merge->channel_properties & CM_CHPR_MAINT_MASK))
			{
				/* same maintenance mask found => disappears but other remain */
				break;
			}
		}

		/* alarm remove_from_merge + DISAPPEARING */
		if (cm_is_null (more_diag))
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): remove_from_merge(0x%x/0x%x)  disappear"
				, port->port_id
				, remove_from_merge->channel_error_type
				, remove_from_merge->ext_channel_error_type
				);

			remove_from_merge->channel_properties = CM_CAST_U16 (
				(remove_from_merge->channel_properties & ~ CM_CHPR_SPEC_MASK)
				| CM_CHPR_SPEC_DIS
				);
		}

			/* alarm remove_from_merge + DISAPPEARING but other errors remain [AP00627740] */
		else
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "port(%u): remove_from_merge(0x%x/0x%x)  disappear but other remain"
				, port->port_id
				, remove_from_merge->channel_error_type
				, remove_from_merge->ext_channel_error_type
				);

			remove_from_merge->channel_properties = CM_CAST_U16 (
				(remove_from_merge->channel_properties & ~ CM_CHPR_SPEC_MASK)
				| CM_CHPR_SPEC_DBO
				);
		}

		cm_pd_alarm_build (channel, param, with_header, max_alarm_data_length, port, remove_from_merge);

			/* remove_from_merge */
		cm_pd_diag_elem_remove (merge_to_list, remove_from_merge);

		return CM_OK;
	}

	else
	{
		/* list's are equal */
		return CM_OK_CANCELLED;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_sc_update (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value
	)
{
	LSA_BOOL  appearing;

	LSA_UNUSED_ARG (channel);

	switch (channel_properties & CM_CHPR_SPEC_MASK) {
	case CM_CHPR_SPEC_APP:
		appearing = LSA_TRUE;
		break;

	case CM_CHPR_SPEC_DIS:
		appearing = LSA_FALSE;
		break;

	case CM_CHPR_SPEC_RSVD:
	case CM_CHPR_SPEC_DBO:
	default:
		CM_FATAL1 (channel_properties);
		return;
	}

	if (appearing)
	{
		/* alloc and set appearing diagnosis */
		CM_PD_DIAG_PTR_TYPE  diag_new = cm_pd_diag_elem_fill__nn (0/*alloc*/, CM_PD_DIAG_TAG_SUBCOMP
			, 0, channel_properties
			, channel_error_type, ext_channel_error_type, ext_channel_add_value
			);

		CM_PD_DIAG_PTR_TYPE  diag_old = cm_pd_diag_elem_lookup (& port->diag.diag_list, diag_new);

		CM_PD_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "port(%u): appearing: alarm_type(%u)  ch_prop(0x%x) ch_err_type(0x%x) ext_ch_err_type(0x%x) ext_ch_add_value(0x%x)"
			, port->port_id, diag_new->alarm_type
			, diag_new->channel_properties, diag_new->channel_error_type
			, diag_new->ext_channel_error_type, diag_new->ext_channel_add_value
			);

		if (cm_is_not_null (diag_old))
		{
			cm_pd_diag_elem_remove (& port->diag.diag_list, diag_old);
		}

		/* insert new diagnosis */
		cm_pd_diag_elem_add (& port->diag.diag_list, diag_new);
	}

	else /* disappearing */
	{
		CM_PD_DIAG_TYPE  diag_elem;

		CM_PD_DIAG_PTR_TYPE  diag_new = cm_pd_diag_elem_fill__nn (&diag_elem, CM_PD_DIAG_TAG_SUBCOMP
			, 0, channel_properties
			, channel_error_type, ext_channel_error_type, ext_channel_add_value
			);

		CM_PD_DIAG_PTR_TYPE  diag_old = cm_pd_diag_elem_lookup (& port->diag.diag_list, diag_new);

		CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "port(%u): disappearing: alarm_type(%u) ch_prop(0x%x) ch_err_type(0x%x) ext_ch_err_type(0x%x) ext_ch_add_value(0x%x) wait_done(0x%x)"
			, port->port_id, diag_new->alarm_type
			, diag_new->channel_properties, diag_new->channel_error_type
			, diag_new->ext_channel_error_type, diag_new->ext_channel_add_value
			, channel->usr.pd.pullplug.wait_done
			);

		if (cm_is_not_null (diag_old))
		{
			cm_pd_diag_elem_remove (& port->diag.diag_list, diag_old);
		}
		else
		{
			CM_FATAL(); /* old does not exist */ /* a bug in the subcomponent */
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_list_clear (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list_to_clear,
	LSA_UINT16  diag_tag /* 0 .. all entrys   n .. all entries with this diag_tag */
	)
{
	if (diag_tag == 0)
	{
		while (! CmListIsEmpty (diag_list_to_clear))
		{
			CM_PD_DIAG_PTR_TYPE diag = CmListFirst (diag_list_to_clear, CM_PD_DIAG_PTR_TYPE);
			cm_pd_diag_elem_remove (diag_list_to_clear, diag);
		}
	}
	else
	{
		CM_PD_DIAG_PTR_TYPE diag;
		LSA_BOOL one_found = LSA_TRUE;

		while (one_found)
		{
			one_found = LSA_FALSE;

			for (CmListForeach (diag, diag_list_to_clear, CM_PD_DIAG_PTR_TYPE))
			{
				if (diag->diag_tag == diag_tag)
				{
					cm_pd_diag_elem_remove (diag_list_to_clear, diag);
					one_found = LSA_TRUE;
					break; /* for */
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_list_move (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list_from,
	CM_LIST_ENTRY_PTR_TYPE  diag_list_to
	)
{
	while (! CmListIsEmpty (diag_list_from))
	{
		CM_PD_DIAG_PTR_TYPE diag;

		CmListRemoveHead (diag_list_from, diag, CM_PD_DIAG_PTR_TYPE);

		CM_ASSERT(cm_is_not_null (diag));

		CmListInsertTail (diag_list_to, & diag->link);
	}
}


#ifdef CM_MESSAGE
# pragma CM_MESSAGE("REFACTOR: make a clear-diag_list for User and Server too (now many while/remove loops)")
/*search for loops with cm_pd_diag_elem_remove*/
#endif /* CM_MESSAGE */


/*---------------------------------------------------------------------------*/
CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_fill__nn (
	LSA_VOID_PTR_TYPE  pre_allocated,	/* 0 .. alloc  else .. &CM_PD_DIAG_TYPE */
	LSA_UINT16 diag_tag,				/* CM_PD_DIAG_TAG_ */
	LSA_UINT16 alarm_type,				/* 0 .. AlarmType from ChannelErrorType */
	LSA_UINT16 channel_properties,
	LSA_UINT16 channel_error_type,
	LSA_UINT16 ext_channel_error_type,
	LSA_UINT32 ext_channel_add_value
	)
{
	CM_PD_DIAG_PTR_TYPE  diag = (CM_PD_DIAG_PTR_TYPE)pre_allocated;

	CM_ASSERT (diag_tag == CM_PD_DIAG_TAG_SUBCOMP || diag_tag == CM_PD_DIAG_TAG_PDUSER || diag_tag == CM_PD_DIAG_TAG_PRMERROR);

	if (alarm_type == 0)
	{
		switch (channel_error_type) {
		case 0x8000: /* Data transmission impossible */
			alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
			break;

		case 0x8001: /* Remote mismatch */
			alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
			break;

		case 0x8002: /* Media redundancy mismatch */
			alarm_type = CM_ALARM_TYPE_MEDIA_REDUNDANCY;
			break;

		case 0x8003: /* Sync mismatch */
			alarm_type = CM_ALARM_TYPE_SYNC_DATA_CHANGED;
			break;

		case 0x8007: /* Fiber optic mismatch */
			alarm_type = CM_ALARM_TYPE_PORT_DATA_CHANGED;
			break;

		case 0x8008: /* Network component function mismatch */
			alarm_type = CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM;
			break;

		case 0x8009: /* Time mismatch */
			alarm_type = CM_ALARM_TYPE_TIME_DATA_CHANGED;
			break;

		case 0x800B: /* Media redundancy with planned duplication mismatch */
			alarm_type = CM_ALARM_TYPE_MRPD_PROBLEM;
			break;

		/* note: update cm_alarm_type_is_internal() when adding new alarm types */
		case 0x800D: /* Multiple interface mismatch */
			alarm_type = CM_ALARM_TYPE_MULTIPLE_INTERFACE;
			break;

		case 0x800A: /* Dynamic frame packing function mismatch */
			alarm_type = CM_ALARM_TYPE_DFP_PROBLEM;
			break;

		case 0x8004: /* IsochronousMode mismatch */
		case 0x8005: /* Multicast CR mismatch */
		case 0x8006: /* reserved */
		default:
			CM_FATAL1(channel_error_type);
			return 0;
		}
	}

	if (cm_is_null (diag))
	{
		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(&diag), sizeof(*diag));
	}
	if (cm_is_null (diag))
	{
		CM_FATAL();
		return 0;
	}
	else
	{
		CmListInitialize (&diag->link);

		if ((channel_properties & (CM_CHPR_DIR_MASK | CM_CHPR_ACCU_MASK)) != 0)
		{
			CM_PD_TRACE_04 (0/*!*/, LSA_TRACE_LEVEL_UNEXP
				, "warning: usage of not allowed ChannelProperies, diag_tag(%u) ChProp(0x%x) ChErrTyp(0x%x) ExChErrTyp(0x%x)"
				, diag_tag, channel_properties, channel_error_type, ext_channel_error_type
				);
			channel_properties &= ~ (CM_CHPR_DIR_MASK | CM_CHPR_ACCU_MASK);
		}

		diag->diag_tag = diag_tag;
		diag->alarm_type = alarm_type;
		diag->channel_properties = channel_properties;
		diag->channel_error_type = channel_error_type;
		diag->ext_channel_error_type = ext_channel_error_type;
		diag->ext_channel_add_value = ext_channel_add_value;
	}
	return diag;
}


/*---------------------------------------------------------------------------*/
CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_lookup (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list,
	CM_PD_DIAG_CONST_PTR_TYPE  diag
	)
{
	CM_PD_DIAG_PTR_TYPE  iter_diag;

	/* key == AlarmType + ChannelError + ExtChannelError + ChannelProperties.Maintenance */

	/* TFS 1405836 ChannelProperties.Direction is always 0 because of ChannelNumber 0x8000 */

	for (CmListForeach (iter_diag, diag_list, CM_PD_DIAG_PTR_TYPE))
	{
		if (iter_diag->alarm_type != diag->alarm_type)
		{
			continue;
		}
		if (iter_diag->channel_error_type != diag->channel_error_type)
		{
			continue;
		}
		if (iter_diag->ext_channel_error_type != diag->ext_channel_error_type)
		{
			continue;
		}
		if ((iter_diag->channel_properties & CM_CHPR_MAINT_MASK) != (diag->channel_properties & CM_CHPR_MAINT_MASK))
		{
			continue;
		}
		return iter_diag;
	}
	return LSA_NULL; /* not found */
}


/*---------------------------------------------------------------------------*/
CM_PD_DIAG_PTR_TYPE
cm_pd_diag_elem_add (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag
	)
{
#if CM_DEBUG
	{
		CM_PD_DIAG_PTR_TYPE iter_diag;
		for (CmListForeach (iter_diag, diag_list, CM_PD_DIAG_PTR_TYPE)) {
			if (iter_diag == diag) { break; }
		}
		CM_ASSERT (cm_is_null (iter_diag));
	}
#endif

	CmListInsertTail (diag_list, & diag->link);

	return diag;
}


/*---------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_elem_remove (
	CM_LIST_ENTRY_CONST_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag
	)
{
	if (cm_is_not_null (diag))
	{
		if (cm_is_not_null (diag_list))
		{
#if CM_DEBUG
			CM_PD_DIAG_PTR_TYPE iter_diag;
			for (CmListForeach (iter_diag, diag_list, CM_PD_DIAG_PTR_TYPE)) {
				if (iter_diag == diag) { break; }
			}
			CM_ASSERT (cm_is_not_null (iter_diag));
#endif
			CmListRemoveEntry (& diag->link);
		}

		{
			LSA_UINT16 rc;
			CM_FREE_LOCAL_MEM (& rc, diag);
			CM_ASSERT (rc == LSA_RET_OK);
			LSA_UNUSED_ARG (rc);
		}
	}
}


/*---------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_diag_elem_has_equal_add_value (
	CM_PD_DIAG_CONST_PTR_TYPE  diag_1,
	CM_PD_DIAG_CONST_PTR_TYPE  diag_2
	)
{
	if (diag_1->ext_channel_add_value == diag_2->ext_channel_add_value)
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diag_maint_qual (
	CM_PD_PORT_PTR_TYPE  port,
	LSA_BOOL  *diag_status_ptr,
	LSA_UINT32 *maint_status_ptr
) {
	CM_LIST_ENTRY_PTR_TYPE diag_list;
	CM_PD_DIAG_PTR_TYPE diag;
	LSA_BOOL diag_status;
	LSA_UINT32 maint_status;

	diag_list = &port->diag.diag_list;

	CM_ASSERT(cm_is_not_null (diag_list));

	diag_status  = LSA_FALSE;
	maint_status = 0;

	for( CmListEach(diag, diag_list, CM_PD_DIAG_PTR_TYPE) ) {

		if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

			switch( (diag->channel_properties & CM_CHPR_MAINT_MASK) ) {

			case CM_CHPR_MAINT_ERR:
				diag_status = LSA_TRUE;
				break;

			case CM_CHPR_MAINT_REQ:
				maint_status |= CM_MAINTENANCE_STATUS_REQUIRED;
				break;

			case CM_CHPR_MAINT_DEM:
				maint_status |= CM_MAINTENANCE_STATUS_DEMANDED;
				break;

			case CM_CHPR_MAINT_QUA:
				CM_FATAL(); /* the pdev does not have qualified diagnoses (22. conclave) */
				break;

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		else {

			/* don't want to know */
		}
	}

	/***/

	*diag_status_ptr = diag_status;
	*maint_status_ptr = maint_status;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  cm_pd_diag_check_add_or_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  nr_of_elems,
	CM_PD_SUBMOD_DIAG_TYPE  *diag_elems,
	LSA_UINT16  chpr_specifier
	)
{
	LSA_RESPONSE_TYPE err_rsp = CM_OK;

	LSA_UINT16  i;

	/* prepare rollback */
	for (i = 0; i < nr_of_elems; ++i)
	{
		CM_PD_SUBMOD_DIAG_TYPE  *sm_diag = diag_elems + i;
		sm_diag->void_ptr = LSA_NULL;
	}

	/* check param */
	if (nr_of_elems == 0)
	{
		return CM_ERR_PARAM;
	}

	for (i = 0; i < nr_of_elems; ++i)
	{
		CM_PD_SUBMOD_DIAG_TYPE *sm_diag = diag_elems + i;
		CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr (channel, sm_diag->slot_nr, sm_diag->subslot_nr);

		if (cm_is_null (port))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDDiagCheck: subslot(%u/0x%x) not found", sm_diag->slot_nr, sm_diag->subslot_nr
				);
			return CM_ERR_PARAM;
		}

		switch (sm_diag->channel_properties & CM_CHPR_TYPE_MASK)
		{
		case CM_CHPR_TYPE_OTHER:
		case CM_CHPR_TYPE_1:
		case CM_CHPR_TYPE_2:
		case CM_CHPR_TYPE_4:
		case CM_CHPR_TYPE_8:
		case CM_CHPR_TYPE_16:
		case CM_CHPR_TYPE_32:
		case CM_CHPR_TYPE_64:
			break;
		default:
			err_rsp = CM_ERR_PARAM;
		}

		if ((sm_diag->channel_properties &  CM_CHPR_ACCU_MASK) != CM_CHPR_ACCU_NO)
		{
			err_rsp = CM_ERR_PARAM;
		}

		if ((sm_diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_QUA)
		{
			err_rsp = CM_ERR_PARAM;
		}

		if ((sm_diag->channel_properties &  CM_CHPR_SPEC_MASK) != chpr_specifier)
		{
			err_rsp = CM_ERR_PARAM;
		}

		switch (sm_diag->channel_properties & CM_CHPR_DIR_MASK)
		{
		case CM_CHPR_DIR_OTHER:
		case CM_CHPR_DIR_IN:
		case CM_CHPR_DIR_OUT:
		case CM_CHPR_DIR_INOUT:
			break;
		default:
			err_rsp = CM_ERR_PARAM;
		}

		if (err_rsp != CM_OK)
		{
			CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDDiagCheck: subslot(%u/0x%x), faulty channel_properties(0x%x), chpr_specifier(0x%x)"
				, sm_diag->slot_nr, sm_diag->subslot_nr, sm_diag->channel_properties, chpr_specifier
				);
			return CM_ERR_PARAM;
		}

		if (! (1 <= sm_diag->channel_error_type && sm_diag->channel_error_type <= 0x7FFF))
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDDiagCheck: subslot(%u/0x%x), faulty channel_error_type(0x%x)"
				, sm_diag->slot_nr, sm_diag->subslot_nr, sm_diag->channel_error_type
				);
			return CM_ERR_PARAM;
		}

		if (! (port->addr.pdev_properties & CM_PDEV_PROP_STATE_PLUGGED))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PDDiagCheck: subslot(%u/0x%x) not plugged", sm_diag->slot_nr, sm_diag->subslot_nr
				);
			return CM_ERR_SEQUENCE;
		}
	}

	return CM_OK;
}


/*===========================================================================*/
/*=== PD-User: PDDiagAdd ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_user_diag_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	LSA_UINT16  i;

	CM_UPPER_PD_DIAG_ADD_PTR_TYPE  diag_add = rb->args.pd.diag_add;
	LSA_UINT16  rsp = cm_pd_diag_check_add_or_remove (channel, diag_add->nr_of_elems, &diag_add->elem[0], CM_CHPR_SPEC_APP);

	if (rsp != CM_OK)
	{
		cm_pd_req_callback(channel, rsp, rb);
		return;
	}

	/* prealloc one diag entry */
	for (i = 0; i < diag_add->nr_of_elems; ++i)
	{
		CM_PD_SUBMOD_DIAG_TYPE *sm_diag = & diag_add->elem[i];

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&sm_diag->void_ptr), sizeof(CM_PD_DIAG_TYPE));
		if (cm_is_null (sm_diag->void_ptr))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDDiagAdd: subslot(%u/0x%x) alloc failed", sm_diag->slot_nr, sm_diag->subslot_nr
				);
			rsp = CM_ERR_ALLOC_LOCAL;
		}
	}

	if (rsp != CM_OK)
	{
		for (i = 0; i < diag_add->nr_of_elems; ++i)
		{
			CM_PD_SUBMOD_DIAG_TYPE *sm_diag = & diag_add->elem[i];

			if (cm_is_not_null (sm_diag->void_ptr))
			{
				LSA_UINT16  rc;
				CM_FREE_LOCAL_MEM (&rc, sm_diag->void_ptr);
				LSA_UNUSED_ARG (rc);
				sm_diag->void_ptr = LSA_NULL;
			}
		}

		cm_pd_req_callback(channel, rsp, rb);
		return;
	}


	for (i = 0; i < diag_add->nr_of_elems; ++i)
	{
		CM_PD_SUBMOD_DIAG_TYPE  *sm_diag = & diag_add->elem[i];
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr__nn (channel, sm_diag->slot_nr, sm_diag->subslot_nr);
		CM_PD_DIAG_PTR_TYPE  diag;
		CM_PD_DIAG_PTR_TYPE  diag_lookup;

		cm_pd_diag_update_start (channel, port);

		diag = cm_pd_diag_elem_fill__nn (sm_diag->void_ptr, CM_PD_DIAG_TAG_PDUSER
			, CM_ALARM_TYPE_DIAGNOSIS, sm_diag->channel_properties
			, sm_diag->channel_error_type, sm_diag->ext_channel_error_type, sm_diag->ext_channel_add_value
			);

		diag_lookup = cm_pd_diag_elem_lookup (&port->diag.diag_list, diag);

		if (cm_is_not_null (diag_lookup))
		{
			cm_pd_diag_elem_remove (&port->diag.diag_list, diag_lookup);
			cm_pd_diag_elem_add (&port->diag.diag_list, diag);
		}
		else
		{
			cm_pd_diag_elem_add (&port->diag.diag_list, diag);
		}

		sm_diag->void_ptr = 0;
	}

	/* first: callback */
	cm_pd_req_callback (channel, CM_OK, rb);

	/* second: trigger alarm.ind */
	cm_pd_diag_update_all_done (channel);
}


/*===========================================================================*/
/*=== PD-User: PDDiagRemove =================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_user_diag_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	LSA_UINT16  i;

	CM_UPPER_PD_DIAG_REMOVE_PTR_TYPE  diag_remove = rb->args.pd.diag_remove;
	LSA_UINT16  rsp = cm_pd_diag_check_add_or_remove (channel, diag_remove->nr_of_elems, &diag_remove->elem[0], CM_CHPR_SPEC_DIS);

	if (rsp != CM_OK)
	{
		cm_pd_req_callback(channel, rsp, rb);
		return;
	}

	for (i = 0; i < diag_remove->nr_of_elems; ++i)
	{
		CM_PD_SUBMOD_DIAG_TYPE  *sm_diag = & diag_remove->elem[i];
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr__nn (channel, sm_diag->slot_nr, sm_diag->subslot_nr);

		CM_PD_DIAG_TYPE  diag_elem;
		CM_PD_DIAG_PTR_TYPE  diag;
		CM_PD_DIAG_PTR_TYPE  diag_lookup;

		cm_pd_diag_update_start (channel, port);

		diag = cm_pd_diag_elem_fill__nn (&diag_elem, CM_PD_DIAG_TAG_PDUSER
			, CM_ALARM_TYPE_DIAGNOSIS, sm_diag->channel_properties
			, sm_diag->channel_error_type, sm_diag->ext_channel_error_type, sm_diag->ext_channel_add_value
			);

		diag_lookup = cm_pd_diag_elem_lookup (&port->diag.diag_list, diag);

		if (cm_is_not_null (diag_lookup))
		{
			cm_pd_diag_elem_remove (&port->diag.diag_list, diag_lookup);
		}
	}

	/* first: callback */
	cm_pd_req_callback (channel, CM_OK, rb);

	/* second: trigger alarm.ind */
	cm_pd_diag_update_all_done (channel);
}


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
