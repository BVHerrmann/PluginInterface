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
/*  F i l e               &F: cm_pd1.c                                  :F&  */
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

#define LTRC_ACT_MODUL_ID  801
#define CM_MODULE_ID       801

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

static LSA_VOID
cm_pd_epm_start (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_MEM_U8_PTR_TYPE annotation /* char[] of size CLRPC_MAX_ANNOTATION_SIZE */
	);

static LSA_BOOL
cm_pd_epm_stop (
	CM_CHANNEL_PTR_TYPE channel
	);

static LSA_VOID
cm_pd_create_im5_record (
	CM_CHANNEL_PTR_TYPE pd_channel,
	CM_UPPER_OEM_DATA_PTR_TYPE oem_data
	);

/*===========================================================================*/
/*=== PD-CHANNEL INIT =======================================================*/
/*===========================================================================*/

static LSA_VOID
cm_pd_channel_init (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	/***/

	{
		CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(channel);
		CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);

		CM_PD_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "registering pd(%u) with edd(%u), initial send_clock(%u)"
			, channel->my_handle
			, edd_channel->my_handle
			, edd_params->CycleBaseFactor
			);

		if (cm_is_not_null (edd_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL();
		}

		edd_channel->lower.channel[channel->path_type] = channel; /* see prm-indication */

		/***/

		channel->usr.pd.port_count = edd_params->MaxPortCnt; /* see alloc of channel->usr.pd.port_array_ptr below */

		channel->usr.pd.clock.send_clock     = edd_params->CycleBaseFactor; /* set initial PDEV-SendClock */
		channel->usr.pd.clock.ar_cnt         = 0;
		channel->usr.pd.clock.is_pending     = LSA_FALSE;
		channel->usr.pd.clock.sendclock_prop = cm_edd_default_sendclock_prop(channel);
	}

	/***/

	{
		CM_RESERVED_INTERVAL_IF_RED_PTR_TYPE rsv = &channel->usr.pd.rsv_interval_red;

		rsv->RxEndNs                = 0;
		rsv->RxLocalTransferEndNs   = 0;
		rsv->TxEndNs                = 0;
		rsv->TxLocalTransferStartNs = 0;
		rsv->TxLocalTransferEndNs   = 0;
	}

	/***/

	if (! cm_edd_send_clock_range_ok_hw (channel, channel->usr.pd.clock.send_clock))
	{
		CM_FATAL (); /* wrong configuration */
	}

	/***/

#if CM_CFG_USE_MRP
	{
		CM_CHANNEL_PTR_TYPE mrp_channel = cm_mrp_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "registering pd(%u) with mrp(%u)"
			, channel->my_handle
			, mrp_channel->my_handle
			);

		if (cm_is_not_null (mrp_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		mrp_channel->lower.channel[channel->path_type] = channel; /* see prm-indication */
	}
#endif

	/***/

#if CM_CFG_USE_GSY
	{
		CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "registering pd(%u) with gsy(%u)"
			, channel->my_handle
			, gsy_channel->my_handle
			);

		if (cm_is_not_null (gsy_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		gsy_channel->lower.channel[channel->path_type] = channel; /* see prm-indication */
	}
#endif

	/***/

#if CM_CFG_USE_POF
	{
		CM_CHANNEL_PTR_TYPE pof_channel = cm_pof_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "registering pd(%u) with pof(%u)"
			, channel->my_handle
			, pof_channel->my_handle
			);

		if (cm_is_not_null (pof_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		pof_channel->lower.channel[channel->path_type] = channel; /* see prm-indication */
	}
#endif

	/***/

	{
		CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "registering pd(%u) with oha(%u)"
			, channel->my_handle
			, oha_channel->my_handle
			);

		if (cm_is_not_null (oha_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		oha_channel->lower.channel[channel->path_type] = channel; /* see prm-indication */
	}

	/***/

	CmListInitialize (&channel->usr.pd.req.queue);
	channel->usr.pd.req.current         = LSA_NULL;
	channel->usr.pd.req.iter.pdev.state = CM_PD_ITER_PDEV_IDLE;
	channel->usr.pd.req.is_busy         = LSA_FALSE;
	channel->usr.pd.req.alarm_ind_count = 0;
	channel->usr.pd.req.wait_for        = 0;

	channel->usr.pd.req.multi_write.is_running = LSA_FALSE;

	CmListInitialize (&channel->usr.pd.req.eventQ);
	CmListInitialize (&channel->usr.pd.req.alarmQ);

	channel->usr.pd.pullplug.wait_for  = 0;
	channel->usr.pd.pullplug.wait_done = 0;

	/***/

	if (channel->usr.pd.port_count == 0)
	{
		CM_FATAL (); /* a bug */
	}

	/* alloc array of PD ports (interface + EDD::MaxPortCnt) */
	{
		LSA_UINT32 alloc_len = sizeof(*channel->usr.pd.port_array_ptr) * (1 /*interface*/ + channel->usr.pd.port_count);

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&channel->usr.pd.port_array_ptr), alloc_len);

		if (cm_is_null (channel->usr.pd.port_array_ptr))
		{
			CM_FATAL (); /* FATAL@alloc, can make non-fatal? */
		}
	}

	/* init PD ports */
	{
		LSA_UINT16 port_id;

		for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
		{

			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

			port->port_id = port_id; /* assign port_id */

			cm_pd_port_init (port); /* initialize the rest */
		}
	}

	/***/

	cm_pd_prm_init (channel, &channel->usr.pd.pdprm);

	/***/

	cm_pd_reclist_init (channel);

	/***/

	cm_pd_almi_init (channel);

	/***/

	CmListInitialize (&channel->usr.pd.sync.led_info_list);

	channel->usr.pd.sync.tell_oha               = LSA_FALSE;
	channel->usr.pd.sync.oha_ptcp_write_pending = LSA_FALSE;


	/***/

	channel->usr.pd.rpc.server_id              = CLRPC_INVALID_SERVER_ID;
	channel->usr.pd.rpc.im5_record_data_length = 0;
	channel->usr.pd.rpc.im5_record_data        = LSA_NULL;
	channel->usr.pd.rpc.oem_is_siemens         = LSA_FALSE;

	/***/

	channel->usr.pd.rtf.wf_sm = 0;
}


/*===========================================================================*/
/*=== PD-CHANNEL UNDO-INIT ==================================================*/
/*===========================================================================*/

static LSA_VOID
cm_pd_channel_undo_init (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_ASSERT(channel->state == CM_PD_DELETING);

	CM_ASSERT (channel->usr.pd.clock.ar_cnt == 0);
	CM_ASSERT (channel->usr.pd.clock.is_pending == LSA_FALSE);

	CM_ASSERT (channel->usr.pd.rpc.server_id == CLRPC_INVALID_SERVER_ID);

	CM_ASSERT (channel->usr.pd.pdprm.occupant == CM_PD_OCCUPANT_NOBODY);

	/* unmap on pd_delete */
	{
		LSA_UINT16 port_id;

		for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
		{
			cm_pd_port_addr_unmap (channel, port_id);
		}
	}

	/* cancel */
	while (! CmListIsEmpty (&channel->usr.pd.sync.led_info_list))
	{
		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead (&channel->usr.pd.sync.led_info_list, rb, CM_UPPER_RQB_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (rb));

		cm_callback (channel, CM_OK_CANCELLED, &rb);
	}

	for (;;)
	{
		CM_UPPER_RQB_PTR_TYPE ev = cm_pd_event_get (channel);
		if (cm_is_null (ev))
		{
			break; /* for */
		}
		cm_callback (channel, CM_OK_CANCELLED, &ev);
	}

	for (;;)
	{
		CM_UPPER_RQB_PTR_TYPE alarm = cm_pd_alarm_get (channel);
		if (cm_is_null (alarm))
		{
			break; /* for */
		}
		cm_callback (channel, CM_OK_CANCELLED, &alarm);
	}

	/*CM_ASSERT(channel->usr.pd.sync.tell_oha == don't care);*/
	CM_ASSERT (channel->usr.pd.sync.oha_ptcp_write_pending == LSA_FALSE);

	/***/

	CM_ASSERT (channel->usr.pd.rtf.wf_sm == 0);

	/***/

	cm_pd_prm_undo_init (&channel->usr.pd.pdprm);

	/***/

	cm_pd_reclist_undo_init (channel);

	/***/

	cm_pd_almi_undo_init (channel);

	/* undo_init PD ports */
	{
		LSA_UINT16 port_id;

		for (port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id)
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

			CM_ASSERT (port->port_id == port_id);

			if (cm_is_not_null (port->misc.link_status_ind))
			{
				cm_callback (channel, CM_OK_CANCELLED, &port->misc.link_status_ind);
			}

			cm_pd_port_undo_init (port);
		}
	}

	/* free array of PD ports (interface + EDD::MaxPortCnt) */
	{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM (&ret_val, channel->usr.pd.port_array_ptr);
		CM_ASSERT (ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG (ret_val);
	}

	channel->usr.pd.port_count = 0; /* sanity */

	/***/

	{
		CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregistering pd(%u) from edd(%u)"
			, channel->my_handle
			, edd_channel->my_handle
			);

		if (cm_is_null (edd_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		edd_channel->lower.channel[channel->path_type] = LSA_NULL; /* see cm_pd_channel_init() */
	}

	/***/

#if CM_CFG_USE_MRP
	{ /* AP00324661 */
		CM_CHANNEL_PTR_TYPE mrp_channel = cm_mrp_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregistering pd(%u) from mrp(%u)"
			, channel->my_handle
			, mrp_channel->my_handle
			);

		if (cm_is_null (mrp_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		mrp_channel->lower.channel[channel->path_type] = LSA_NULL; /* see cm_pd_channel_init() */
	}
#endif

	/***/

#if CM_CFG_USE_GSY
	{
		CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregistering pd(%u) from gsy(%u)"
			, channel->my_handle
			, gsy_channel->my_handle
			);

		if (cm_is_null (gsy_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		gsy_channel->lower.channel[channel->path_type] = LSA_NULL; /* see cm_pd_channel_init() */
	}
#endif

	/***/

#if CM_CFG_USE_POF
	{
		CM_CHANNEL_PTR_TYPE pof_channel = cm_pof_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregistering pd(%u) from pof(%u)"
			, channel->my_handle, pof_channel->my_handle
			);

		if (cm_is_null (pof_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		pof_channel->lower.channel[channel->path_type] = LSA_NULL; /* see cm_pd_channel_init() */
	}
#endif

	/***/

	{
		CM_CHANNEL_PTR_TYPE oha_channel = cm_oha_channel_get (channel);

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "unregistering pd(%u) from oha(%u)"
			, channel->my_handle, oha_channel->my_handle
			);

		if (cm_is_null (oha_channel->lower.channel[channel->path_type]))
		{
			CM_FATAL ();
		}

		oha_channel->lower.channel[channel->path_type] = LSA_NULL; /* see cm_pd_channel_init() */
	}

	/***/

	return;
}


/*===========================================================================*/
/*=== PD-CHANNEL REQUESTS ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_request (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	if (cm_is_null (rb->args.void_ptr)) /* args must be there always - with some exceptions */
	{
		switch (CM_RQB_GET_OPCODE (rb))
		{
		case CM_OPC_PD_DELETE:
		case CM_OPC_PD_PRM_BEGIN:
			break;

		default:
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "no args");
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}
	}

	/***/

	switch (CM_RQB_GET_OPCODE (rb))
	{
	case CM_OPC_PD_PROVIDE_EVENT:
		if (channel->state == CM_PD_READY)
		{
			cm_pd_event_provide (channel, rb);
		}
		else
		{
			cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		}
		return;

	case CM_OPC_PD_PROVIDE_ALARM:
		if (channel->state == CM_PD_READY)
		{
			rb->args.pd.alarm->alarm_data_length = 0; /* "don't care", see docu */
			rb->args.pd.alarm->alarm_data        = LSA_NULL;

			if (rb->args.pd.alarm->alarm_priority != CM_ALARM_PRIORITY_LOW)
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "prio out of range");
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}

			cm_pd_alarm_provide (channel, rb);
			cm_pd_alarm_provide_event (channel);
		}
		else
		{
			cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		}
		return;

#if CM_CFG_MAX_SERVERS
	case CM_OPC_SV_AR_ALARM_PDEV_GROUP:
		cm_pd_opc_sv_ar_alarm_pdev_group_rsp (channel, rb);
		return;
#endif

	case CM_OPC_PD_CREATE:
		{
			CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-create: rqb(0x%x) channel_state(0x%x) h(%u) max_alarm_data_length(%u)"
				, rb, channel->state, channel->my_handle
				, rb->args.pd.create->max_alarm_data_length
				);

			if (channel->state == CM_CH_READY)
			{
				cm_pd_channel_init(channel);
				channel->state = CM_PD_STARTING;
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_DELETE:
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-delete: rqb(0x%x) channel_state(0x%x) h(%u)"
				, rb, channel->state, channel->my_handle
				);

			if (channel->state == CM_PD_READY)
			{
				channel->state = CM_PD_DELETING;
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_REMAP:
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-remap: rqb(0x%x) channel_state(0x%x) map_count(%u)"
				, rb, channel->state, rb->args.pd.remap->map_count
				);

			if (channel->state == CM_PD_READY)
			{
				channel->state = CM_PD_REMAPPING;
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_LINK_STATUS_INFO:
		if (channel->state == CM_PD_READY)
		{
			cm_pd_opc_link_status_info(channel, rb);
		}
		else
		{
			cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		}
		return;

	case CM_OPC_PD_SYNC_LED_INFO:
		if (channel->state == CM_PD_READY)
		{
			cm_pd_opc_sync_led_info(channel, rb);
		}
		else
		{
			cm_callback(channel, CM_ERR_SEQUENCE, &rb);
		}
		return;

	case CM_OPC_PD_SUBMODULE_CONTROL:
		{
			CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE submodule_control = rb->args.pd.submodule_control;
			LSA_UNUSED_ARG (submodule_control);

			CM_PD_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-submodule-control: rqb(0x%x) channel_state(0x%x) slot_nr(%u) subslot_nr(0x%x) pdev_properties(0x%x) mod_ident(0x%x) sub_ident(0x%x) im0_bits(0x%x)"
				, rb, channel->state
				, submodule_control->slot_nr, submodule_control->subslot_nr
				, submodule_control->pdev_properties, submodule_control->mod_ident
				, submodule_control->sub_ident, submodule_control->im0_bits
				);

			if (channel->state == CM_PD_READY)
			{
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_DIAG_ADD:
		{
			CM_UPPER_PD_DIAG_ADD_PTR_TYPE diag_add = rb->args.pd.diag_add;
			LSA_UNUSED_ARG (diag_add);

			CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PDDiagAdd: rqb(0x%x) nr_of_elem(%u), first: subslot(%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x) add_value(0x%x)"
				, rb, diag_add->nr_of_elems, diag_add->elem[0].slot_nr, diag_add->elem[0].subslot_nr
				, diag_add->elem[0].channel_error_type, diag_add->elem[0].ext_channel_error_type, diag_add->elem[0].ext_channel_add_value
				);

			if (channel->state == CM_PD_READY)
			{
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_DIAG_REMOVE:
		{
			CM_UPPER_PD_DIAG_REMOVE_PTR_TYPE diag_rm = rb->args.pd.diag_remove;
			LSA_UNUSED_ARG (diag_rm);

			CM_PD_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PDDiagRemove: rqb(0x%x) nr_of_elem(%u), first: subslot(%u/0x%x) ch_err(0x%x) ext_ch_err(0x%x)"
				, rb, diag_rm->nr_of_elems, diag_rm->elem[0].slot_nr, diag_rm->elem[0].subslot_nr
				, diag_rm->elem[0].channel_error_type, diag_rm->elem[0].ext_channel_error_type
				);

			if (channel->state == CM_PD_READY)
			{
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_PRM_READ:
		{
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

			CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-read: rqb(0x%x) channel_state(0x%x) slot_nr(%u) subslot_nr(0x%x) index(0x%x)"
				, rb, channel->state
				, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index
				);

			if (channel->state == CM_PD_READY)
			{
				prm_rdwr->is_local = CM_PD_IS_LOCAL_YES;
				prm_rdwr->cls      = cm_record_index_classify (channel, 0 /*!*/, prm_rdwr->record_index, CM_VENDOR_ID_SIEMENS /*!*/);

				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_PRM_BEGIN:
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-prepare: rqb(0x%x) channel_state(0x%x)"
				, rb, channel->state
				);

			if (channel->state == CM_PD_READY)
			{
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_PRM_WRITE:
		{
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

			CM_PD_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-write: rqb(0x%x) channel_state(0x%x) record_index(0x%x) slot(%u) subslot(0x%x) record_data_length(%u)"
				, rb, channel->state
				, prm_rdwr->record_index, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
				, prm_rdwr->record_data_length
				);

			if (channel->state == CM_PD_READY)
			{
				prm_rdwr->is_local = CM_PD_IS_LOCAL_YES;
				prm_rdwr->cls      = cm_record_index_classify (channel, 0 /*!*/, prm_rdwr->record_index, CM_VENDOR_ID_SIEMENS /*!*/);

				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_PRM_END:
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "PD-prm-end: rqb(0x%x) channel_state(0x%x)"
				, rb, channel->state
				);

			if (channel->state == CM_PD_READY)
			{
				cm_pd_req_queue_add (channel, rb);
			}
			else
			{
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			}
		}
		return;

	case CM_OPC_PD_EVENT_APPL_READY_RSP:
		cm_pd_opc_appl_ready_rsp (channel, rb);
		return;

	case CM_OPC_PD_ALARM_RSP:
		cm_pd_opc_alarm_rsp (channel, rb);
		return;

#if CM_CFG_USE_GSY
	case CM_OPC_PD_PTCP_MASTER_CONTROL:
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PD-ptcp-master-control: rqb(0x%x) mode(%u)"
			, rb, rb->args.pd.ptcp_master_control->mode
			);

		if (channel->state == CM_PD_READY)
		{
			cm_pd_req_queue_add (channel, rb);
		}
		else
		{
			cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		}
		return;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_OPC_PD_PRM_BEGIN_SERVER:
	case CM_OPC_PD_PRM_READ_SERVER:
	case CM_OPC_PD_PRM_WRITE_SERVER:
	case CM_OPC_PD_PRM_END_SERVER:
	case CM_OPC_PD_PRM_RELINQUISH_SERVER:
		if (channel->state == CM_PD_READY)
		{
			CM_CHANNEL_PTR_TYPE sv_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_SERVER);

			if (cm_is_null (sv_channel))
			{
				CM_FATAL (); /* a bug */
			}

			CM_RQB_SET_HANDLE (rb, sv_channel->my_handle); /* prepare for callback */
			cm_pd_req_queue_add (channel, rb);
		}
		else
		{
			cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		}
		return;
#endif

#if CM_CFG_MAX_CLIENTS
	case CM_OPC_PD_PRM_READ_CLIENT:
		cm_pnio_c_read_implicit_rsp (channel /*PD*/, rb);
		return;
#endif

	default:
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unhandled opcode(%u), path-type(%u)"
			, CM_RQB_GET_OPCODE (rb), channel->path_type
			);
		CM_FATAL1 ((LSA_UINT32)rb);
		return;
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_CREATE
//----------------------------------------------------------------------------*/

static LSA_VOID
cm_pd_create (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_UPPER_PD_CREATE_PTR_TYPE create = rb->args.pd.create;
	LSA_UINT16 port_id;

	CM_ASSERT (cm_is_not_null (create));
	CM_ASSERT (channel->state == CM_PD_STARTING);
	CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.pd.req.current) == CM_OPC_PD_CREATE);

	CM_ASSERT (sizeof(create->annotation) == CLRPC_MAX_ANNOTATION_SIZE);

	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "CreatePD: rqb(0x%x), h(%u) max_alarm_data_length(%u)"
		, rb, channel->my_handle
		, create->max_alarm_data_length
		);

	/***/
	if (! (CM_ALARM_DATA_LENGTH_MIN <= create->max_alarm_data_length
		   && create->max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX))
	{
		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "CreatePD: !(%u <= max_alarm_data_length(%u) <= %u)"
			, CM_ALARM_DATA_LENGTH_MIN, create->max_alarm_data_length, CM_ALARM_DATA_LENGTH_MAX
			);

		goto err_param;
	}

	if (cm_is_not_null (create->oem_data))
	{
		if (create->oem_data->vendor_id == 0 || create->oem_data->device_id == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "CreatePD: OEMData not valid: VendorID(0x%04x), DeviceID(0x%04x), must not be '0'"
				, create->oem_data->vendor_id, create->oem_data->device_id
				);

			goto err_param;
		}
		if (! (create->oem_data->SWRevisionPrefix == 'V'
			   || create->oem_data->SWRevisionPrefix == 'R'
			   || create->oem_data->SWRevisionPrefix == 'P'
			   || create->oem_data->SWRevisionPrefix == 'U'
			   || create->oem_data->SWRevisionPrefix == 'T'))
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "CreatePD: OEMData not valid: SWRevisionPrefix('%c'), must be one of ('V', 'R', 'P', 'U', 'T')"
				, create->oem_data->SWRevisionPrefix
				);

			goto err_param;
		}
	}

	{
		LSA_BOOL has_im5_record = (cm_is_not_null (create->oem_data) ? LSA_TRUE : LSA_FALSE);
		LSA_UINT16 rc           = cm_pd_port_addr_map_check (channel, create->map_count, &create->map[0], has_im5_record);
		if (rc != CM_OK)
		{
			goto err_param;
		}
	}

	/***/

	channel->acp.max_alarm_data_length = create->max_alarm_data_length; /* re-using the acp-structure */

	/***/

	/* add port mapping */

	for (port_id = 0; port_id < create->map_count; ++port_id)
	{
		CM_UPPER_PD_MAP_PTR_TYPE map = &create->map[port_id];

		cm_pd_port_addr_map (channel, port_id, map);
	}

	cm_pd_reclist_alloc_records (channel);

	cm_pd_create_im5_record (channel, create->oem_data);

	cm_pd_pullplug_map_remap (channel, CM_PD_PULLPLUG__MAP, 0);

	return;

err_param:
	channel->state = CM_CH_READY;
	cm_pd_req_callback (channel, CM_ERR_PARAM, channel->usr.pd.req.current);
	return;
}

/*------------------------------------------------------------------------------*/
LSA_VOID
cm_pd_pullplug_map_remap (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 wait_done,
	LSA_UINT16 port_id  /* start = 0 */
	)
{
	CM_PD_PORT_PTR_TYPE port;

	for (;;)
	{
		port_id += 1;
		port     = cm_pd_port_from_id (channel, port_id);

		if (port)
		{
			if (CM_PDEV_STATE_IS_PULLED (port->addr.pdev_properties))
			{
				cm_pd_pullplug_sm_start (channel, port, wait_done);
				return;
			}
		}
		else
		{
			break;
		}
	}

	if (wait_done == CM_PD_PULLPLUG__MAP)
	{
		CM_UPPER_PD_CREATE_PTR_TYPE create = channel->usr.pd.req.current->args.pd.create;

		CM_ASSERT (cm_is_not_null (create));

		cm_pd_epm_start (channel, create->annotation);
	}
	else if (wait_done == CM_PD_PULLPLUG__REMAP)
	{
		cm_pd_req_callback (channel, CM_OK, channel->usr.pd.req.current);
	}
	else
	{
		CM_FATAL (); /* a bug */
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_epm_start (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_MEM_U8_PTR_TYPE annotation /* char[] of size CLRPC_MAX_ANNOTATION_SIZE */
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;

	CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "starting EPM");

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);
	CM_ASSERT (channel->state == CM_PD_STARTING);

	/***/

	rpc = cm_rpc_alloc_rqb (channel);

	if (cm_is_null (rpc))
	{

		CM_FATAL ();
		return;
	}

	/***/

	{
		CLRPC_UPPER_EPM_START_PTR_TYPE args = rpc->args.epm.start;

		args->server_id = 0; /* sanity, out-parameter */
		args->udp_port  = 0; /* sanity, out-parameter */

		args->recv_resource_count = channel->detailptr->u.pd.epm.recv_resource_count;
		args->send_resource_count = channel->detailptr->u.pd.epm.send_resource_count;

		CM_ASSERT (sizeof(args->annotation) == CLRPC_MAX_ANNOTATION_SIZE);

		CM_MEMCPY (args->annotation, annotation, sizeof(args->annotation)); /* copy all */
		args->annotation[sizeof(args->annotation) - 1] = 0; /* and make it zero-terminated */
	}

	CM_ASSERT (channel->usr.pd.rpc.server_id == CLRPC_INVALID_SERVER_ID);

	channel->usr.pd.rpc.server_id = CLRPC_INVALID_SERVER_ID;

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_EPM_START, 0, 0);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_rpc_epm_start_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 rpc_response
	)
{
	CM_UPPER_OEM_DATA_PTR_TYPE oem_data = channel->usr.pd.req.current->args.pd.create->oem_data;

	if (rpc_response != CLRPC_OK)
	{

		CM_FATAL1 (rpc_response); /* should not happen */
	}

	if (cm_is_not_null (oem_data))
	{

		cm_oha_set_ident (channel, oem_data);
	}
	else
	{

		cm_pd_oha_set_ident_done (channel, OHA_OK); /* fake */
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_oha_set_ident_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 oha_response
	)
{
	if (oha_response != OHA_OK)
	{

		CM_FATAL1 (oha_response); /* must succeed */
	}

	cm_pd_get_port_params_done (channel, LSA_NULL /*start*/);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_get_port_params_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_PTR_TYPE edd
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel;
	LSA_BOOL done = LSA_FALSE;

	if (cm_is_null (edd)) /*start*/
	{
		pd_channel = channel;
		{
			CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params (pd_channel);

			/* init: MRPRingPort == EDD_MRP_NO_RING_PORT */

			if (edd_params->MRPSupportedRole == 0 /*not supported*/)
			{
				done = LSA_TRUE;
			}
			else
			{
				CM_PD_PORT_PTR_TYPE port_if = cm_pd_port_from_id__nn (pd_channel, 0 /*interface*/);

				port_if->misc.MRPRingPort = EDD_MRP_RING_PORT; /* interface supports MRP */

				edd = cm_edd_get_rqb (pd_channel);

				if (cm_is_null (edd))
				{
					CM_FATAL ();
				}
				else
				{
					EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE);

					param->PortID = 1;
					cm_edd_request_lower (pd_channel, EDD_SRV_GET_PORT_PARAMS, pd_channel, edd);
				}
			}
		}
	}
	else
	{
		EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE);

		pd_channel = (CM_CHANNEL_PTR_TYPE) CM_EDD_GET_USER_ID_PTR (edd);

		if (CM_EDD_GET_RESPONSE (edd) == EDD_STS_OK)
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (pd_channel, param->PortID);

			if (param->MRPRingPort != EDD_MRP_NO_RING_PORT)
			{
				port->misc.MRPRingPort = param->MRPRingPort;
			}

			if (param->PortID < pd_channel->usr.pd.port_count)
			{
				param->PortID += 1;
				cm_edd_request_lower (pd_channel, EDD_SRV_GET_PORT_PARAMS, pd_channel, edd);
			}
			else
			{
				cm_edd_release_rqb (pd_channel, edd);
				done = LSA_TRUE;
			}
		}
		else
		{
			CM_FATAL ();
		}
	}

	if (done)
	{
		CM_ASSERT (pd_channel->state == CM_PD_STARTING);
		pd_channel->state = CM_PD_READY;

		// Default resource counts
		pd_channel->usr.pd.req.current->args.pd.create->nr_of_event_resources = 1;
		pd_channel->usr.pd.req.current->args.pd.create->nr_of_alarm_resources = 1;

		CM_ASSERT (CM_RQB_GET_OPCODE (pd_channel->usr.pd.req.current) == CM_OPC_PD_CREATE);
		cm_pd_req_callback (pd_channel, CM_OK, pd_channel->usr.pd.req.current);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_DELETE
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_delete (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_ASSERT(channel->state == CM_PD_DELETING);

	{
		LSA_UINT8 pta[3] = {CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_SERVER, CM_PATH_TYPE_MULTICAST};
		LSA_INT i;

		for (i = 0; i < 3; ++i)
		{
			CM_CHANNEL_PTR_TYPE pt_channel = cm_peer_channel_get (channel, pta[i]);
			if (cm_is_not_null (pt_channel))
			{
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "channel with path-type(%u) must not exist"
					, pta[i]
					);

				channel->state = CM_PD_READY;
				cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
				return;
			}
		}
	}

	/* check for owner of PDev */
	if (channel->usr.pd.pdprm.occupant != CM_PD_OCCUPANT_NOBODY)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ownership not released from occupant(%u)", channel->usr.pd.pdprm.occupant
			);

		channel->state = CM_PD_READY;
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	/* check for pending appl_ready_rsp */
	if (channel->usr.pd.almi.appl_ready_rsp_pending)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "waiting for pending appl_ready_rsp"
			);
		return;
	}

	/* wait */
	if (channel->usr.pd.req.alarm_ind_count > 0)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "waiting for alarm_ind_count(%u) == 0"
			, channel->usr.pd.req.alarm_ind_count
			);
		return;
	}

#if CM_CFG_MAX_SERVERS
	if (cm_is_null (channel->usr.pd.almi.server_alarm))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "waiting for pending server alarm"
			);
		CM_FATAL ();
	}
#endif

	if (channel->usr.pd.sync.oha_ptcp_write_pending)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "waiting for finishing sync.oha_ptcp_write_pending"
			);
		return;
	}

	cm_pd_alarm_user_enable (channel, LSA_FALSE);

	CM_ASSERT (channel->state == CM_PD_DELETING);

	/* set to not-prmed */
	channel->usr.pd.pdprm.is_parameterized = LSA_FALSE;

	if (cm_pd_epm_stop (channel))
	{

		/* wait-for cm_pd_rpc_epm_stop_done() */
	}
	else
	{

		CM_FATAL (); /* FATAL@rpc-alloc: can make non-fatal? */
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_pd_epm_stop (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;

	CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_WARN, "stopping EPM");

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);
	CM_ASSERT (channel->state == CM_PD_DELETING);

	/* set to not-prmed */
	channel->usr.pd.pdprm.is_parameterized = LSA_FALSE;

	/***/

	rpc = cm_rpc_alloc_rqb (channel);

	if (cm_is_null (rpc))
	{

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "could not alloc rpc-mem");

		return LSA_FALSE;
	}

	/***/

	CM_ASSERT (channel->usr.pd.rpc.server_id != CLRPC_INVALID_SERVER_ID);

	rpc->args.epm.stop->server_id = channel->usr.pd.rpc.server_id;

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_EPM_STOP, 0, 0);

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_rpc_epm_stop_done (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_ASSERT (channel->state == CM_PD_DELETING);

	/* set to not-prmed */
	channel->usr.pd.pdprm.is_parameterized = LSA_FALSE;

	if (cm_is_not_null (channel->usr.pd.rpc.im5_record_data))
	{
		LSA_UINT16 rc;
		CM_FREE_UPPER_MEM (&rc, channel->usr.pd.rpc.im5_record_data, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
		channel->usr.pd.rpc.im5_record_data = LSA_NULL;
	}

	cm_pd_channel_undo_init(channel);

	channel->state = CM_CH_READY;

	cm_pd_req_callback (channel, CM_OK, channel->usr.pd.req.current);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_REMAP
//----------------------------------------------------------------------------*/

static LSA_VOID
cm_pd_remap (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_UPPER_PD_REMAP_PTR_TYPE remap = rb->args.pd.remap;

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PD-Remap: rqb(0x%x) map_count(%u)"
		, rb, rb->args.pd.remap->map_count
		);

	channel->state = CM_PD_READY;

	{
		LSA_UINT8 pta[3] = {CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_SERVER, CM_PATH_TYPE_MULTICAST};
		LSA_INT i;

		for (i = 0; i < 3; ++i)
		{
			CM_CHANNEL_PTR_TYPE pt_channel = cm_peer_channel_get (channel, pta[i]);
			if (cm_is_not_null (pt_channel))
			{
				if (pt_channel->state != CM_CH_READY)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "PD-remap: existing channel with path-type(%u) must have the state(ready), not state(%u)"
						, pta[i], pt_channel->state
						);
					cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
					return;
				}
			}
		}
	}

	{
		LSA_UINT16 rc = cm_pd_port_addr_map_check (channel, remap->map_count, &remap->map[0], cm_pdusr_exists_im5_record (channel));
		if (rc != CM_OK)
		{
			cm_pd_req_callback (channel, rc, rb);
			return;
		}
	}

#if 1//test needed, see below
	{
		LSA_UINT16 port_id;

		for (port_id = 0; port_id < remap->map_count; ++port_id)
		{
			CM_UPPER_PD_MAP_PTR_TYPE map = &remap->map[port_id];
			CM_PD_PORT_PTR_TYPE port     = cm_pd_port_from_id__nn (channel, port_id);

			if (CM_PDEV_STATE_IS_PULLED (map->pdev_properties) && CM_PDEV_STATE_IS_PULLED (port->addr.pdev_properties))
			{
				continue;
			}
			if (CM_PDEV_STATE_IS_PLUGGED (map->pdev_properties) && CM_PDEV_STATE_IS_PLUGGED (port->addr.pdev_properties))
			{
				continue;
			}

			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-remap: changing pulled / plugged state not allowed, port_id(%u)", port_id
				);
			cm_pd_req_callback (channel, CM_ERR_PARAM, rb);
			return;
		}
	}
#endif

	/* disable alarms until PdApplReady.ind */
	cm_pd_alarm_user_enable (channel, LSA_FALSE);


	/* add port mapping */
	{
		LSA_UINT16 port_id;

		for (port_id = 0; port_id < remap->map_count; ++port_id)
		{
			CM_UPPER_PD_MAP_PTR_TYPE map = &remap->map[port_id];

			cm_pd_port_addr_unmap (channel, port_id);
			cm_pd_port_addr_map (channel, port_id, map);
		}
	}

	/* set to not-prmed */
	{
		channel->usr.pd.pdprm.is_parameterized = LSA_FALSE;
	}

	cm_pd_pullplug_map_remap (channel, CM_PD_PULLPLUG__REMAP, 0);
	/*/// test needed: script PdevRemap allows only valid test cases */
}


/*===========================================================================*/
/*=== REQUEST QUEUE =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_req_queue_add (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CmListInsertTail (&channel->usr.pd.req.queue, (CM_LIST_ENTRY_PTR_TYPE)rb);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PD request added, rqb(0x%08x) with cm-opc(%u)", rb, CM_RQB_GET_OPCODE (rb)
		);

	cm_pd_req_queue_next (channel);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_req_queue_next (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_UPPER_RQB_PTR_TYPE current;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	/***/

	if (channel->usr.pd.req.is_busy)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "PD request-loop is_busy");
		return;
	}

	channel->usr.pd.req.is_busy = LSA_TRUE;

	while (channel->usr.pd.req.is_busy)
	{

		if (cm_is_not_null (channel->usr.pd.req.current))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "PD request rqb(0x%08x) with cm-opc(%u) is in progress"
				, channel->usr.pd.req.current
				, CM_RQB_GET_OPCODE (channel->usr.pd.req.current)
				);
			channel->usr.pd.req.is_busy = LSA_FALSE;
			continue;
		}

		if (CmListIsEmpty (&channel->usr.pd.req.queue))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "PD request-queue empty");
			channel->usr.pd.req.is_busy = LSA_FALSE;
			continue;
		}

		CmListRemoveHead (&channel->usr.pd.req.queue, current, CM_UPPER_RQB_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (current));

		channel->usr.pd.req.current = current;

		switch (CM_RQB_GET_OPCODE (current))
		{

		case CM_OPC_PD_CREATE:
			cm_pd_create (channel, current);
			break;

		case CM_OPC_PD_DELETE:
			cm_pd_delete (channel, current);
			break;

		case CM_OPC_PD_REMAP:
			cm_pd_remap (channel, current);
			break;

		case CM_OPC_PD_SUBMODULE_CONTROL:
			cm_pd_submodule_control (channel, current);
			break;

		case CM_OPC_PD_DIAG_ADD:
			cm_pd_user_diag_add (channel, current);
			break;

		case CM_OPC_PD_DIAG_REMOVE:
			cm_pd_user_diag_remove (channel, current);
			break;

		case CM_OPC_PD_PRM_READ:
			cm_pd_prm_read_user (channel, current);
			break;

		case CM_OPC_PD_PRM_READ_CLIENT:
			cm_pd_prm_read_user (channel, current);
			break;

		case CM_OPC_PD_PRM_READ_SERVER:
			cm_pd_prm_read_internal (channel, current);
			break;

		case CM_OPC_PD_PRM_BEGIN:
			cm_pd_prm_begin_user (channel, current);
			break;

#if CM_CFG_MAX_SERVERS
		case CM_OPC_PD_PRM_BEGIN_SERVER:
			cm_pd_prm_begin_server (channel, current);
			break;
#endif

		case CM_OPC_PD_PRM_WRITE:
			if (current->args.pd.prm_rdwr->record_index == 0xF8000000 /*MultiplePDev*/)
			{
				cm_pd_prm_write_multiple (channel, current);
			}
			else
			{
				cm_pd_prm_write_user (channel, current, LSA_FALSE);
			}
			break;

#if CM_CFG_MAX_SERVERS
		case CM_OPC_PD_PRM_WRITE_SERVER:
			cm_pd_prm_write_user (channel, current, LSA_TRUE);
			break;
#endif

		case CM_OPC_PD_PRM_END:
			cm_pd_prm_end_user (channel, current);
			break;

#if CM_CFG_MAX_SERVERS
		case CM_OPC_PD_PRM_END_SERVER:
			cm_pd_prm_end_server (channel, current);
			break;
#endif

#if CM_CFG_MAX_SERVERS
		case CM_OPC_PD_PRM_RELINQUISH_SERVER:
			cm_pd_prm_relinquish_server (channel, current);
			break;
#endif

#if CM_CFG_USE_GSY
		case CM_OPC_PD_PTCP_MASTER_CONTROL:
			{
				LSA_UINT16 rc;
				rc = cm_gsy_ptcp_master_control (channel, current->args.pd.ptcp_master_control);
				if (rc != CM_OK_PENDING)
				{
					cm_pd_req_callback (channel, rc, current);
				}
				break;
			}
#endif

		case CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER:
			cm_pd_config_trigger (&channel->usr.pd.pdprm, CM_PD_CONFIG_START);
			break;

		default:
			CM_FATAL1 (CM_RQB_GET_OPCODE (current));
			return;
		}
	}

	CM_ASSERT (! channel->usr.pd.req.is_busy);
}

/*----------------------------------------------------------------------------*/

CM_UPPER_RQB_PTR_TYPE
cm_pd_req_queue_remove_current (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_UPPER_RQB_PTR_TYPE current;

	current = channel->usr.pd.req.current;
	channel->usr.pd.req.current = LSA_NULL;

	return current;
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_req_callback (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	/* handle services not stored in .req.current */
	switch (CM_RQB_GET_OPCODE (rb))
	{
	case CM_OPC_PD_EVENT_APPL_READY_IND:
		cm_callback (channel, response, &rb);
		cm_pd_req_queue_next (channel);
		return;

#if CM_CFG_MAX_SERVERS
	case CM_OPC_SV_AR_ALARM_PDEV_GROUP:
#endif
	case CM_OPC_PD_PRM_END_SERVER: /* xxx_PRM_END_SERVER do NOT finish ownership of pdprm */
		/* indication for INTERNAL opc */
		CM_RQB_SET_RESPONSE (rb, response);
		/* note: Handle was set in cm_pd_request() */

		CM_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) f-h(%u)"
			, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb), channel->my_handle
			);

		CM_REQUEST_LOCAL (rb);
		cm_pd_req_queue_next (channel);
		return;

	default:
		break;
	}

	if (channel->usr.pd.req.iter.pdev.state != CM_PD_ITER_PDEV_IDLE)
	{
		if (! (CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_READ ||  CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_READ_CLIENT))
		{
			CM_FATAL ();
		}

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cm_pd_req_callback: iterator pdev.state(%u) response(%u)"
			, channel->usr.pd.req.iter.pdev.state, response
			);

		CM_RQB_SET_RESPONSE (rb, response);

		/***/

		{
			LSA_UINT32 pnio_err;

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("PD-F831/41/42: tail-recursion (for those completed in prm-read), avoidable")
/* breakpoint setzen und callstack betrachten */
#endif /* CM_MESSAGE */

			pnio_err = cm_pd_queue_iterator_next (channel, rb);

			if (pnio_err != CM_PNIO_ERR_NONE)
			{

				if (channel->usr.pd.req.iter.pdev.state != CM_PD_ITER_PDEV_IDLE)
				{
					CM_FATAL ();
				}

				channel->usr.pd.req.iter.pdev.org.record_data_length = 0; /* update */

				*channel->usr.pd.req.current->args.pd.prm_rdwr = channel->usr.pd.req.iter.pdev.org; /* restore */

				/***/

				if ((pnio_err & 0xFFFFFF00) == CM_PNIO_FAULTY_Record__BufferTooSmall (0))
				{

					response = CM_ERR_PRM_DATA;
				}
				else
				{

					response = CM_ERR_PRM_OTHER;
				}

				/***/

				/* see case CM_OPC_PD_PRM_READ below */
			}
			else
			{

				return; /* pending or completed */
			}
		}
	}


	switch (CM_RQB_GET_OPCODE (rb))
	{
	case CM_OPC_PD_CREATE:
	case CM_OPC_PD_REMAP:
		CM_ASSERT(rb == channel->usr.pd.req.current);
		cm_callback(channel, response, &channel->usr.pd.req.current);
		break;

	case CM_OPC_PD_DELETE:
		CM_ASSERT(rb == channel->usr.pd.req.current);
		cm_callback(channel, response, &channel->usr.pd.req.current);
		return;

	case CM_OPC_PD_SUBMODULE_CONTROL:
	case CM_OPC_PD_DIAG_ADD:
	case CM_OPC_PD_DIAG_REMOVE:
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_callback (channel, response, &channel->usr.pd.req.current);
		break;

	case CM_OPC_PD_PRM_READ_CLIENT:
		/* callback for INTERNAL opc */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		CM_RQB_SET_RESPONSE (rb, response);
		CM_REQUEST_LOCAL (rb);
		channel->usr.pd.req.current = LSA_NULL;
		break;

	case CM_OPC_PD_PRM_BEGIN_SERVER:
	case CM_OPC_PD_PRM_READ_SERVER:
	case CM_OPC_PD_PRM_WRITE_SERVER:
	case CM_OPC_PD_PRM_RELINQUISH_SERVER:
		/* callback for INTERNAL opc */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		CM_RQB_SET_RESPONSE (rb, response);
		/* note: Handle was set in cm_pd_request() */

		CM_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) f-h(%u)"
			, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb), channel->my_handle
			);

		CM_REQUEST_LOCAL (rb);
		channel->usr.pd.req.current = LSA_NULL;
		break;

	case CM_OPC_PD_PRM_READ:
	case CM_OPC_PD_PRM_WRITE:

		/* case WriteMultiple */
		{
			if (CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_WRITE
				&&  channel->usr.pd.req.multi_write.is_running)
			{
				CM_ASSERT (rb == channel->usr.pd.req.multi_write.internal_rqb);

				/* keep original response */
				cm_pd_prm_write_multiple_done (channel, response);
				return; /*! no break !*/
			}
		}

		switch (response)
		{
		case CM_OK:
			{
				LSA_UINT32 cm_pnio_err = rb->args.pd.prm_rdwr->cm_pnio_err;

				if (cm_pnio_err != CM_PNIO_ERR_NONE
					&& CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_WRITE
					&&  CM_PNIO_ERR_FETCH_CODE (cm_pnio_err) != CM_PNIO_ERR_CODE_WRITE)
				{
					CM_ASSERT (CM_PNIO_ERR_FETCH_CODE (cm_pnio_err) == CM_PNIO_ERR_CODE_READ);
					rb->args.pd.prm_rdwr->cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (cm_pnio_err);

					CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "write: changed error code to write, PNIOStatus(0x%x)", cm_pnio_err
						);
				}
			}
			break;

		case CM_ERR_PARAM:
		case CM_ERR_SEQUENCE:
		case CM_ERR_ALLOC_UPPER: /* PRM_WRITE(Multiple) */
			/* keep */
			break;

		case CM_ERR_NOT_ACCESSIBLE: /* mapping does not exist */
		case CM_ERR_PRM_INDEX:
		case CM_ERR_PRM_PORTID:
		case CM_ERR_PRM_DATA:
		case CM_ERR_PRM_BLOCK:
		case CM_ERR_PRM_CONSISTENCY: /* note for AP00633079: should only occur with PrmEnd */
		case CM_ERR_PRM_OTHER:
		case CM_ERR_PRM_VERSION:
		case CM_ERR_PRM_ACCESS_DENIED:
			/* map to PNIOStatus */
			{
				LSA_UINT32 cm_pnio_err;

				if (CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_READ)
				{
					cm_pnio_err = cm_pd_response_to_pnio_err (response, LSA_TRUE);

					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH /*AP00586614:no error trace if mrp is not configured*/
						, "read: map response(%d) to PNIOStatus(0x%x)"
						, response, cm_pnio_err
						);
				}
				else
				{
					cm_pnio_err = cm_pd_response_to_pnio_err (response, LSA_FALSE);

					CM_ASSERT (CM_PNIO_ERR_FETCH_CODE (cm_pnio_err) == CM_PNIO_ERR_CODE_READ);
					cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (cm_pnio_err);

					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "write: map response(%d) to PNIOStatus(0x%x)"
						, response, cm_pnio_err
						);
				}
				rb->args.pd.prm_rdwr->cm_pnio_err        = cm_pnio_err;
				rb->args.pd.prm_rdwr->record_data_length = 0;
				response = CM_OK;
			}
			break;

		default:
			CM_FATAL1 (response);
			break;
		}

		/* user request finished -> ready for the next one */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_callback (channel, response, &channel->usr.pd.req.current);
		break;

	case CM_OPC_PD_PRM_BEGIN:
		/* user request finished -> ready for the next one */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_callback (channel, response, &channel->usr.pd.req.current);
		break;

	case CM_OPC_PD_PRM_END:
		/* user request finished -> ready for the next one */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_callback (channel, response, &channel->usr.pd.req.current);
		break;

	case CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER:
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_pd_config_callback (&channel->usr.pd.pdprm, response, channel->usr.pd.req.current);
		channel->usr.pd.req.current = LSA_NULL;
		break;

	case CM_OPC_PD_PTCP_MASTER_CONTROL:
		/* user request finished -> ready for the next one */
		CM_ASSERT (rb == channel->usr.pd.req.current);
		cm_callback (channel, response, &channel->usr.pd.req.current);
		break;

	default:
		CM_FATAL ();
		return;
	}

	/***/

	/* ready to get next request from queue */
	cm_pd_req_queue_next (channel);
}


/*===========================================================================*/
/*=== SYNC LED-INFO CHECK ===================================================*/
/*===========================================================================*/

static LSA_VOID
cm_pd_sync_led_info_check (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE curr
	)
{
	if (cm_is_null (curr))   /* check the whole list */

	{
		curr = CmListFirst (&channel->usr.pd.sync.led_info_list, CM_UPPER_RQB_PTR_TYPE);
	}
	else   /* check only this (new) request */

	{
		CmListInsertTail (&channel->usr.pd.sync.led_info_list, (CM_LIST_ENTRY_PTR_TYPE)curr);
	}

	for (
		/* curr = already set-up */
		; cm_is_not_null (curr)
		; curr = CmListNext (&channel->usr.pd.sync.led_info_list, (CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE)
		)
	{
		CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, curr->args.pd.sync_led_info->edd_port_id);

		if (
			curr->args.pd.sync_led_info->sync_led_info    != port->misc.sync_led_info
			|| (curr->args.pd.sync_led_info->is_rate_valid == CM_PD_IS_RATE_VALID_YES ? LSA_TRUE : LSA_FALSE) != port->misc.sync_led__is_rate_valid
			|| curr->args.pd.sync_led_info->rcv_sync_prio != port->misc.sync_led__rcv_sync_prio
			|| curr->args.pd.sync_led_info->local_prio    != port->misc.sync_led__local_prio
			|| curr->args.pd.sync_led_info->pll_state     != port->misc.sync_led__pll_state
			)
		{

			CM_UPPER_RQB_PTR_TYPE temp = curr; /* has delta */

			curr = CmListPrevOrHead ((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

			CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)temp);

			/* update */

			temp->args.pd.sync_led_info->sync_led_info = port->misc.sync_led_info;
			temp->args.pd.sync_led_info->is_rate_valid = port->misc.sync_led__is_rate_valid ? CM_PD_IS_RATE_VALID_YES : CM_PD_IS_RATE_VALID_NO;
			temp->args.pd.sync_led_info->rcv_sync_prio = port->misc.sync_led__rcv_sync_prio;
			temp->args.pd.sync_led_info->local_prio    = port->misc.sync_led__local_prio;
			temp->args.pd.sync_led_info->pll_state     = port->misc.sync_led__pll_state;

			CM_PD_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "port(%u) sync_led_info(%u) rate_valid(%u) rcv_sync_prio(%u) local_prio(%u) pll_state(%u) changed"
				, temp->args.pd.sync_led_info->edd_port_id
				, temp->args.pd.sync_led_info->sync_led_info
				, temp->args.pd.sync_led_info->is_rate_valid
				, temp->args.pd.sync_led_info->rcv_sync_prio
				, temp->args.pd.sync_led_info->local_prio
				, temp->args.pd.sync_led_info->pll_state
				);

			cm_callback (channel, CM_OK, &temp);
		}
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_SYNC_LED_INFO
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_opc_sync_led_info (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	if (rb->args.pd.sync_led_info->edd_port_id > channel->usr.pd.port_count)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "edd_port_id(%u) out of range"
			, rb->args.pd.sync_led_info->edd_port_id
			);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (rb->args.pd.sync_led_info->sync_led_info >= CM_PD_SYNC_LED_INFO_MAX)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "sync_info(%u) out of range"
			, rb->args.pd.sync_led_info->sync_led_info
			);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (rb->args.pd.sync_led_info->is_rate_valid != CM_PD_IS_RATE_VALID_NO && rb->args.pd.sync_led_info->is_rate_valid != CM_PD_IS_RATE_VALID_YES)
	{
		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "is_rate_valid(%u) out of range"
			, rb->args.pd.sync_led_info->is_rate_valid
		);
		cm_callback(channel, CM_ERR_PARAM, &rb);
		return;
	}

	/***/

	cm_pd_sync_led_info_check (channel, rb); /* trigger: opcode */
}


/*===========================================================================*/
/*=== OHA WRITE PTCP-DATA TRIGGER ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_oha_write_ptcp_trigger (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CHANNEL_PTR_TYPE gsy_channel
	)
{
	if (channel->usr.pd.sync.oha_ptcp_write_pending)
	{

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "oha-ptcp-write pending"
			);

		/* see cm_pd_oha_write_ptcp_data_done() */
	}
	else if (channel->usr.pd.pdprm.state == CM_PD_PRM_STATE_WF_COMMIT)
	{

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "commit running (deferring update)"
			);

		/* see cm_pd_prm_sc_commit_done() */
	}
	else if (! channel->usr.pd.sync.tell_oha)
	{

		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "no (more) differences"
			);
	}
	else   /* must tell OHA */

	{
		channel->usr.pd.sync.tell_oha = LSA_FALSE;

#if CM_CFG_USE_GSY
		if (cm_is_null (gsy_channel))
		{

			gsy_channel = cm_gsy_channel_get (channel);

			if (cm_is_null (gsy_channel))
			{
				CM_FATAL ();
			}
		}

		if (gsy_channel->usr.gsy.do_sync_clock)
		{

			cm_oha_write_ptcp_data (channel
				, gsy_channel->usr.gsy.do_sync_clock
				, gsy_channel->usr.gsy.is_sync
				, &gsy_channel->usr.gsy.master_mac_addr.MacAdr
				);

			channel->usr.pd.sync.oha_ptcp_write_pending = LSA_TRUE;
		}
#else
		if (cm_is_null (gsy_channel))
		{
			/*
			 * note: do_sync_clock == LSA_FALSE because CM_CFG_USE_GSY == 0; see comment below
			 */
		}
#endif
		else
		{
			/*
			 * do_sync_clock == LSA_FALSE is now signalled indirectly via the prm-sequence
			 *
			 * all sync-records are duplicated to OHA
			 * thus OHA knows whether we are synchronizing or not
			 *
			 * note: without the if() OHA returns an ERR_SEQUENCE in OHA_RequestPtcpWriteData()
			 *       (user feedback: no error-trace if it is not an error)
			 */
		}
	}
}

/*===========================================================================*/
/*=== OHA WRITE PTCP-DATA DONE ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_oha_write_ptcp_data_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 oha_response
	)
{
	if (channel->state != CM_PD_READY
		&& channel->state != CM_PD_DELETING)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "state(%u) not pd-ready or deleting"
			, channel->state
			);
		CM_FATAL (); /* a bug */
	}
	else if (! channel->usr.pd.sync.oha_ptcp_write_pending)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "oha-write.cnf not pending"
			);
		CM_FATAL (); /* a bug */
	}
	else
	{
		channel->usr.pd.sync.oha_ptcp_write_pending = LSA_FALSE;

		if (CM_PD_CURRENT_IS_DELETE (channel))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "pd_delete is queued"
				);

			cm_pd_delete (channel, channel->usr.pd.req.current);
		}
		else
		{
			if (oha_response != OHA_OK)
			{
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "oha-write failed, oha-rsp(%u)"
					, oha_response
					);
				/* ignore */
			}

			cm_pd_oha_write_ptcp_trigger (channel, LSA_NULL);
		}
	}
}

/*===========================================================================*/
/*=== GSY SYNC-STATE CHANGED ================================================*/
/*===========================================================================*/

#if CM_CFG_USE_GSY
LSA_VOID
cm_pd_sync_state_changed (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CHANNEL_PTR_TYPE gsy_channel /* current values in ->usr.gsy.xxx */
	)
{
	CM_PD_PORT_PTR_TYPE port_if = cm_pd_port_from_id__nn (channel, 0 /*interface*/);

	if (channel->state != CM_PD_READY)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}

	CM_PD_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "has_syncdata(%u) do_sync_clock(%u) is_sync(%u) rate_valid(%u) rcv_sync_prio(%u) local_prio(%u) pll_state(%u)"
		, gsy_channel->usr.gsy.subdomain
		, gsy_channel->usr.gsy.do_sync_clock
		, gsy_channel->usr.gsy.is_sync
		, gsy_channel->usr.gsy.is_rate_valid
		, gsy_channel->usr.gsy.rcv_sync_prio
		, gsy_channel->usr.gsy.local_prio
		, gsy_channel->usr.gsy.pll_state
		);

	/***/

	if (gsy_channel->usr.gsy.do_sync_clock)
	{

		if (gsy_channel->usr.gsy.is_sync)
		{

			port_if->misc.sync_led_info = CM_PD_SYNC_LED_INFO_SYNC;
		}
		else
		{

			port_if->misc.sync_led_info = CM_PD_SYNC_LED_INFO_NOT_SYNC;
		}

		port_if->misc.sync_led__is_rate_valid = gsy_channel->usr.gsy.is_rate_valid;
		port_if->misc.sync_led__rcv_sync_prio = gsy_channel->usr.gsy.rcv_sync_prio;
		port_if->misc.sync_led__local_prio    = gsy_channel->usr.gsy.local_prio;
	}
	else
	{

		port_if->misc.sync_led_info = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;

		port_if->misc.sync_led__is_rate_valid = LSA_FALSE;
		port_if->misc.sync_led__rcv_sync_prio = 0;
		port_if->misc.sync_led__local_prio    = 0;
	}

	/***/

	port_if->misc.sync_led__pll_state = gsy_channel->usr.gsy.pll_state;

	/***/

	cm_pd_sync_led_info_check (channel, LSA_NULL); /* trigger: sync-state changed */

	/***/

	channel->usr.pd.sync.tell_oha = LSA_TRUE;

	cm_pd_oha_write_ptcp_trigger (channel, gsy_channel);
}
#endif

/*===========================================================================*/
/*=== EDD IRT-PORT-STATUS CHANGED ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_irt_port_status_changed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id,
	LSA_UINT8 irt_port_status
	)
{
	CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);
	LSA_UINT16 sync_led_info;

	if (channel->state != CM_PD_READY)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}

	switch (irt_port_status)
	{

	case EDD_IRT_NOT_SUPPORTED:
		sync_led_info = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;
		break;

	case EDD_IRT_PORT_INACTIVE:
	case EDD_IRT_PORT_ACTIVE_TX_UP:
		sync_led_info = CM_PD_SYNC_LED_INFO_NOT_SYNC;
		break;

	case EDD_IRT_PORT_ACTIVE_TX_RX:
		sync_led_info = CM_PD_SYNC_LED_INFO_SYNC;
		break;

	default:
		CM_FATAL (); /* a bug in EDD */
		return;
	}

	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "port(%u) sync_led_info old(%u) new(%u)"
		, port_id, port->misc.sync_led_info, sync_led_info
		);

	if (port->misc.sync_led_info != sync_led_info)
	{

		port->misc.sync_led_info = sync_led_info;

		cm_pd_sync_led_info_check (channel, LSA_NULL); /* trigger: RTC3 port-state changed */

#if CM_CFG_MAX_MULTICAST_CHANNELS
		if (port->misc.sync_led_info == CM_PD_SYNC_LED_INFO_SYNC)
		{

			CM_CHANNEL_PTR_TYPE mc_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_MULTICAST);

			if (cm_is_not_null (mc_channel))
			{
				cm_mc_device_any_port_irt_run (mc_channel);
			}
		}
#endif
	}
}

/*===========================================================================*/
/*=== IS ANY PORT IRT RUN ===================================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdusr_is_any_port_irt_run (
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel;
	LSA_UINT16 port_id;

	pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	if (cm_is_null (pd_channel))
	{
		return LSA_FALSE; /* unreachable */
	}

	for (port_id = 1; port_id <= pd_channel->usr.pd.port_count; ++port_id)
	{

		CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (pd_channel, port_id);

		if (port->misc.sync_led_info == CM_PD_SYNC_LED_INFO_SYNC)
		{
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== INTERNAL LINK-STATUS CHANGED ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_link_status_changed (
	CM_CHANNEL_PTR_TYPE pd_channel,
	LSA_UINT8 status,
	LSA_UINT8 prev_status
	)
{
	LSA_BOOL delta;
	LSA_BOOL is_up;
	LSA_BOOL is_transient;

	CM_ASSERT (pd_channel->path_type == CM_PATH_TYPE_PD);

	if (pd_channel->state != CM_PD_READY)
	{
		CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, pd_channel->state
			);
		return;
	}

	if (status == EDD_LINK_UP)
	{
		is_up        = LSA_TRUE;
		is_transient = LSA_FALSE; /* up is per definiton not transient */
		if (prev_status == EDD_LINK_UP)
		{
			delta = LSA_FALSE; /* no delta (speed/mode changed) */
		}
		else
		{
			delta = LSA_TRUE; /* delta: not-up --> up */
		}
	}
	else if (status == EDD_LINK_UP_CLOSED)
	{
		is_up        = LSA_FALSE;
		is_transient = LSA_TRUE; /* UP_CLOSED is transient (MRP ring-reconfiguration takes some 100ms) */
		if (prev_status == EDD_LINK_UP_CLOSED)
		{
			delta = LSA_FALSE; /* no delta (speed/mode changed) */
		}
		else
		{
			delta = LSA_TRUE; /* delta: not-closed --> closed */
		}
	}
	else /* down/disabled */
	{
		is_up        = LSA_FALSE;
		is_transient = LSA_FALSE;

		if (prev_status == EDD_LINK_UP)
		{
			delta = LSA_TRUE; /* delta: up --> down/disabled */
		}
		else if (prev_status == EDD_LINK_UP_CLOSED)
		{
			delta = LSA_TRUE; /* delta: closed --> down/disabled */
		}
		else /* unknown/down/disabled */
		{
			delta = LSA_FALSE; /* no delta (unknown/down/disabled --> down/disabled or speed/mode changed) */
		}
	}

	if (delta)
	{
		CM_ASSERT (is_up == cm_link_status_is_up (pd_channel)); /* only valid for auto-port */

		CM_PD_TRACE_02 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "pd-link-delta: now is_up(%u) is_transient(%u)"
			, is_up, is_transient
			);
		LSA_UNUSED_ARG (is_up);
		LSA_UNUSED_ARG (is_transient);

#if CM_CFG_MAX_CLIENTS
		/*
		 * note: client uses schedule-timeout for checking link-state up
		 */

		/*
		 * note: do not cm_cl_ar_set_to_down(LINK_DOWN), wait for miss.ind instead
		 */
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		/*
		 * do nothing (V6.0i1.xx, refactoring TIA 201616; CM-MC uses "irt port status changed")
		 */
#endif

#if CM_CFG_MAX_SERVERS
		/*
		 * do nothing (due to AP00831818, AP00774495, AP00774501; cm_sv_link_status_changed() removed with V5.3i1.xx)
		 */
#endif
	}
}


/*===========================================================================*/
/*=== USER LINK-STATUS CHANGED (CM_OPC_PD_LINK_STATUS_INFO) ====================*/
/*===========================================================================*/

#if CM_PORT_ID_AUTO != EDD_PORT_ID_AUTO \
	|| CM_LINK_STATUS_UNKNOWN     != EDD_LINK_UNKNOWN \
	|| CM_LINK_STATUS_UP          != EDD_LINK_UP \
	|| CM_LINK_STATUS_DOWN        != EDD_LINK_DOWN \
	|| CM_LINK_STATUS_UP_CLOSED   != EDD_LINK_UP_CLOSED \
	|| CM_LINK_STATUS_UP_DISABLED != EDD_LINK_UP_DISABLED \
	|| CM_LINK_SPEED_UNKNOWN      != EDD_LINK_UNKNOWN \
	|| CM_LINK_SPEED_10_M         != EDD_LINK_SPEED_10 \
	|| CM_LINK_SPEED_100_M        != EDD_LINK_SPEED_100 \
	|| CM_LINK_SPEED_1_G          != EDD_LINK_SPEED_1000 \
	|| CM_LINK_SPEED_10_G         != EDD_LINK_SPEED_10000 \
	|| CM_LINK_MODE_UNKNOWN       != EDD_LINK_UNKNOWN \
	|| CM_LINK_MODE_HALF_DUPLEX   != EDD_LINK_MODE_HALF \
	|| CM_LINK_MODE_FULL_DUPLEX   != EDD_LINK_MODE_FULL \
	|| CM_LINK_AUTONEG_UNKNOWN    != EDD_AUTONEG_UNKNOWN \
	|| CM_LINK_AUTONEG_ON         != EDD_AUTONEG_ON \
	|| CM_LINK_AUTONEG_OFF        != EDD_AUTONEG_OFF
#error "who changed that? see comparison CM_LINK_xxx vs. EDD_LINK_xxx below"
#endif

LSA_VOID
cm_pd_link_status_trigger_user (
	CM_CHANNEL_PTR_TYPE pd_channel,
	LSA_UINT16 port_id
	)
{
	CM_CHANNEL_PTR_TYPE edd_channel;
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params;
	CM_PD_PORT_PTR_TYPE port;

	CM_ASSERT (pd_channel->path_type == CM_PATH_TYPE_PD);

	edd_channel = cm_edd_channel_get (pd_channel); /* fatal if not exists */
	edd_params  = cm_edd_channel_params (pd_channel);

	port = cm_pd_port_from_id__nn (pd_channel, port_id); /* fatal if port_id not exists */

	if (cm_is_not_null (port->misc.link_status_ind))
	{
		CM_UPPER_PD_LINK_STATUS_INFO_PTR_TYPE param = port->misc.link_status_ind->args.pd.link_status;
		CM_EDD_PORT_STATUS_PTR_TYPE ps              = &edd_channel->usr.edd.ports[port_id];

		if (param->status != ps->status || param->speed != ps->speed || param->mode != ps->mode || param->autoneg != ps->autoneg)
		{
			param->interface_id = edd_params->InterfaceID;

			CM_ASSERT (param->port_id == port_id);

			param->status  = ps->status;
			param->speed   = ps->speed;
			param->mode    = ps->mode;
			param->autoneg = ps->autoneg;

			cm_callback (pd_channel, CM_OK, &port->misc.link_status_ind);
		}
	}
}

/*===========================================================================*/
/*=== ResetToFactory or NameOfStation or IpSuite changed ====================*/
/*===========================================================================*/

LSA_VOID
cm_pd_rtf_nos_ip_changed (
	CM_CHANNEL_PTR_TYPE pd_channel,
	LSA_UINT8 reason  /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
	)
{
	CM_PD_RTF_PTR_TYPE rtf = &pd_channel->usr.pd.rtf;

	/* was reset-to-factory, V4.1i3.1, AP00738256 */

	if (pd_channel->state != CM_PD_READY)
	{
		CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "RtfNosIpChanged: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, pd_channel->state
			);
		return;
	}

	CM_PD_TRACE_02 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "RtfNosIpChanged: start with reason(%u), wf_sm(0x%x)", reason, rtf->wf_sm
		);

	if (reason == CM_AR_REASON_DCP_RESET)
	{
		/* set state to ResetToFactory */
		pd_channel->usr.pd.pdprm.is_reset_to_factory = LSA_TRUE;

		if ((rtf->wf_sm & CM_PD_RTF_WF__IS_NOS_IP) != 0)
		{
			CM_PD_TRACE_01 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "RtfNosIpChanged: delay reset-to-factory, wf_sm(0x%x)", rtf->wf_sm
				);
			rtf->wf_sm |= CM_PD_RTF_WF__DO_DELAYED_RTF;
			return;
		}
	}

	if (rtf->wf_sm != 0)
	{
		CM_PD_TRACE_00 (pd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "RtfNosIpChanged: already running, ignoring second trigger"
			);
		return;
	}

	/* setup wf-flags */
	if (reason == CM_AR_REASON_DCP_RESET)
	{
		rtf->wf_sm = CM_PD_RTF_WF__IS_RTF | CM_PD_RTF_WF__START_DONE;
	}
	else
	{
		rtf->wf_sm = CM_PD_RTF_WF__IS_NOS_IP | CM_PD_RTF_WF__START_DONE;
	}

#if CM_CFG_MAX_SERVERS
	{
		CM_CHANNEL_PTR_TYPE sv_channel = cm_peer_channel_get (pd_channel, CM_PATH_TYPE_SERVER);

		if (cm_is_not_null (sv_channel))
		{
			LSA_BOOL all_down = cm_sv_pdev_Rtf_NoS_IpSuite (sv_channel, reason);

			if (! all_down)
			{
				rtf->wf_sm |= CM_PD_RTF_WF__SV_DOWN;
			}
		}
	}
#endif

#if CM_CFG_MAX_CLIENTS
	{
		CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (pd_channel);

		if (cm_is_not_null (cl_channel))
		{
			LSA_BOOL all_down;

			rtf->wf_sm |= CM_PD_RTF_WF__CL_DOWN;

			all_down = cm_cl_pdev_set_down_ind (cl_channel, reason);
			if (all_down)
			{
				rtf->wf_sm &= ~CM_PD_RTF_WF__CL_DOWN;
			}
		}
	}
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	{
		CM_CHANNEL_PTR_TYPE mc_channel = cm_peer_channel_get (pd_channel, CM_PATH_TYPE_MULTICAST);

		if (cm_is_not_null (mc_channel))
		{
			LSA_BOOL all_down = cm_mc_pdev_set_down_ind (mc_channel, reason);

			if (! all_down)
			{
				rtf->wf_sm |= CM_PD_RTF_WF__MC_DOWN;
			}
		}
	}
#endif

	/* trigger */
	cm_pd_rtf_nos_ip_changed_done (pd_channel, CM_PD_RTF_WF__START_DONE);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_rtf_nos_ip_changed_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 wf_done
	)
{
	CM_PD_RTF_PTR_TYPE rtf = &channel->usr.pd.rtf;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "RtfNosIpChanged: wf_sm(0x%x) wf_done(0x%x)"
		, rtf->wf_sm, wf_done
		);

	if ((rtf->wf_sm & (CM_PD_RTF_WF__IS_RTF | CM_PD_RTF_WF__IS_NOS_IP)) == 0)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "RtfNosIpChanged: not running, wf_sm(0x%x)"
			, rtf->wf_sm
			);
		CM_FATAL (); /* a bug */
	}

	if ((rtf->wf_sm & wf_done) == 0)
	{
		CM_FATAL (); /* a bug */
	}
	rtf->wf_sm &= ~wf_done;

	/* wait-for pd/sv/cl/mc done */
	if (rtf->wf_sm & (CM_PD_RTF_WF__START_DONE | CM_PD_RTF_WF__SV_DOWN | CM_PD_RTF_WF__CL_DOWN | CM_PD_RTF_WF__MC_DOWN))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "RtfNosIpChanged: wf_done(0x%x),  must wait for an other done, rtf_wf_sm(0x%x)"
			, wf_done, rtf->wf_sm
			);
		return;
	}

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "RtfNosIpChanged: all down, wf_sm(0x%x)", rtf->wf_sm
		);

	if (rtf->wf_sm & CM_PD_RTF_WF__IS_NOS_IP)
	{
		rtf->wf_sm &= ~CM_PD_RTF_WF__IS_NOS_IP;

		cm_pd_config_start (channel, CM_PD_CONFIG_NOS_IP);
	}
	else if (rtf->wf_sm & CM_PD_RTF_WF__IS_RTF)
	{
		rtf->wf_sm &= ~CM_PD_RTF_WF__IS_RTF;

		cm_pd_config_start (channel, CM_PD_CONFIG_RESET_TO_FACTORY);
	}

	if (rtf->wf_sm & CM_PD_RTF_WF__DO_DELAYED_RTF)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "RtfNosIpChanged: continue delayed reset-to-factory"
			);
		rtf->wf_sm &= ~CM_PD_RTF_WF__DO_DELAYED_RTF;

		if (rtf->wf_sm != 0)
		{
			CM_FATAL ();
		}

		cm_pd_rtf_nos_ip_changed (channel, CM_AR_REASON_DCP_RESET);
	}
	else if (rtf->wf_sm != 0)
	{
		CM_FATAL ();
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pdusr_is_reset_to_factory (
	/* true: PDev is in state reset-to-factory */
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	return pd_channel->usr.pd.pdprm.is_reset_to_factory;
}

/*===========================================================================*/
/*=== IS PDEV STABLE ========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdusr_is_pdev_stable (
	/* true if user is allowed to start up */
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	/* temporary: RtfNosIpChanged is not ready yet */
	if (pd_channel->usr.pd.rtf.wf_sm != 0)
	{
		return LSA_FALSE;
	}

	/* tailoring not finished */
	if (pd_channel->usr.pd.pdprm.wait_cnf != 0 || pd_channel->usr.pd.pdprm.wait_cnf_next != 0)
	{
		return LSA_FALSE;
	}

	if (pd_channel->usr.pd.reclist.is_running_mpa)
	{
		return LSA_FALSE;
	}

	if (pd_channel->usr.pd.pdprm.last_appl_ready_ind == CM_OK_ADAPTION_NEEDED)
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== RTF-IS-RUNNING ========================================================*/
/*===========================================================================*/

LSA_BOOL
cm_pdusr_rtf_is_running (
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	if (cm_is_null (pd_channel))
	{
		CM_FATAL ();
	}
	else
	{
		/* RTF is running if between start and end:
		 * start: cm_cl_pdev_set_down_ind(), cm_mc_pdev_set_down_ind(), cm_sv_pdev_Rtf_NoS_IpSuite()
		 *   working: cm_pd_rtf_nos_ip_changed_done()
		 *   working: cm_pd_config_start(RTF)
		 *   working: cm_pd_prm_sc_prepare()
		 *   working: cm_pd_prm_sc_commit_done()
		 *   working: appl-ready.ind (not waiting for .rsp)
		 * end: PDEV parameterization complete
		 */

		if ((pd_channel->usr.pd.rtf.wf_sm & (CM_PD_RTF_WF__IS_RTF | CM_PD_RTF_WF__DO_DELAYED_RTF)) != 0)
		{
			return LSA_TRUE;
		}

		if ((pd_channel->usr.pd.pdprm.wait_cnf_next & CM_PD_CONFIG_RESET_TO_FACTORY) != 0)
		{
			return LSA_TRUE;
		}

		if ((pd_channel->usr.pd.pdprm.wait_cnf & CM_PD_CONFIG_RESET_TO_FACTORY) != 0)
		{
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== I&M5 ==================================================================*/
/*===========================================================================*/

static LSA_VOID
cm_pd_create_im5_record (
	CM_CHANNEL_PTR_TYPE pd_channel,
	CM_UPPER_OEM_DATA_PTR_TYPE oem_data
	)
{
	if (cm_is_null (oem_data))
	{
		pd_channel->usr.pd.rpc.im5_record_data_length = 0;
		pd_channel->usr.pd.rpc.im5_record_data        = LSA_NULL;
		pd_channel->usr.pd.rpc.oem_is_siemens         = LSA_FALSE;
	}
	else
	{
		LSA_UINT16 im5data_len = 2 + 2 + 2 + 64 + 64 + 1 + 1 + 16 + 2 + 1 + 1 + 1 + 1;
		LSA_UINT16 alloc_len   = 2 + 2 + 2 + 2 + im5data_len;

		pd_channel->usr.pd.rpc.oem_is_siemens = (oem_data->vendor_id == CM_VENDOR_ID_SIEMENS) ? LSA_TRUE : LSA_FALSE;

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&pd_channel->usr.pd.rpc.im5_record_data)
			, cm_null_user_id, alloc_len, pd_channel->sysptr
			);
		if (cm_is_null (pd_channel->usr.pd.rpc.im5_record_data))
		{
			CM_FATAL ();
		}
		else
		{
			CM_UPPER_MEM_U8_PTR_TYPE dp = pd_channel->usr.pd.rpc.im5_record_data;
			LSA_UINT16 left             = alloc_len;

			pd_channel->usr.pd.rpc.im5_record_data_length = alloc_len;

			CM_PUT16_HTON (dp, 0, CM_BLOCK_TYPE_I_M_5);
			CM_PUT16_HTON (dp, 2, CM_CAST_U16 (alloc_len - (2 + 2)));
			CM_PUT16_HTON (dp, 4, 0x0100);

			CM_PUT16_HTON (dp, 6, 1);

			cm_dp_adjust (dp, left, 2 + 2 + 2 + 2);

			CM_PUT16_HTON (dp, 0, CM_BLOCK_TYPE_I_M_5_Data);
			CM_PUT16_HTON (dp, 2, CM_CAST_U16 (im5data_len - (2 + 2)));
			CM_PUT16_HTON (dp, 4, 0x0100);

			cm_dp_adjust (dp, left, 2 + 2 + 2);

			CM_MEMCPY (dp, oem_data->IM_Annotation, 64);
			cm_dp_adjust (dp, left, 64);

			CM_MEMCPY (dp, oem_data->IM_OrderID, 64);
			cm_dp_adjust (dp, left, 64);

			CM_PUT16_HTON (dp, 0, oem_data->vendor_id);
			cm_dp_adjust (dp, left, 2);

			CM_MEMCPY (dp, oem_data->IM_Serial_Number, 16);
			cm_dp_adjust (dp, left, 16);

			CM_PUT16_HTON (dp, 0, oem_data->IM_Hardware_Revision);
			CM_PUT8_HTON  (dp, 2, oem_data->SWRevisionPrefix);
			CM_PUT8_HTON  (dp, 3, oem_data->IM_SWRevision_Functional_Enhancement);
			CM_PUT8_HTON  (dp, 4, oem_data->IM_SWRevision_Bug_Fix);
			CM_PUT8_HTON  (dp, 5, oem_data->IM_SWRevision_Internal_Change);
			cm_dp_adjust (dp, left, 2 + 1 + 1 + 1 + 1);

			CM_ASSERT (left == 0);
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_pdusr_exists_im5_record (
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	if (pd_channel->usr.pd.rpc.im5_record_data_length != 0)
	{
		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_pdusr_get_im5_record (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT32 max_im5_rec_data_length,
	LSA_UINT16  * written_length,
	CM_COMMON_MEM_U8_PTR_TYPE im5_rec_data
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);

	if (max_im5_rec_data_length >= pd_channel->usr.pd.rpc.im5_record_data_length)
	{
		if (pd_channel->usr.pd.rpc.im5_record_data_length != 0)
		{
			CM_MEMCPY (im5_rec_data, pd_channel->usr.pd.rpc.im5_record_data, pd_channel->usr.pd.rpc.im5_record_data_length);
		}

		*written_length = pd_channel->usr.pd.rpc.im5_record_data_length;
		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}

/*===========================================================================*/
/*=== PTCP MASTER CONTROL ===================================================*/
/*===========================================================================*/

#if CM_CFG_USE_GSY
LSA_VOID
cm_pd_gsy_ptcp_master_control_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
	)
{
	LSA_UINT16 response;

	switch (CM_GSY_GET_RESPONSE (gsy))
	{
	case GSY_RSP_OK:
		response = CM_OK;
		break;

	default:
		response = CM_ERR_LOWER_LAYER;
		channel->usr.pd.req.current->args.pd.ptcp_master_control->gsy_response = CM_GSY_GET_RESPONSE (gsy);
		break;
	}

	cm_gsy_free_rqb (channel, gsy);

	cm_pd_req_callback (channel, response, channel->usr.pd.req.current);
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
