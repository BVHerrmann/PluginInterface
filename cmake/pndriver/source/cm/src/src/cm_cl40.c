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
/*  F i l e               &F: cm_cl40.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client ar statemachine                                                   */
/*                                                                           */
/*  client device statemachine                                               */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	640
#define CM_MODULE_ID		640

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

static LSA_UINT16  cm_cl_ar_fsu_with_at__hello_filter_replace (
	CM_AR_GRAPH_PTR_TYPE  argr
	);


/*------------------------------------------------------------------------------
// schedule-interface
//
//	cm_cl_schedule_init				init schedule-sm
//
//	in_schedule						::= (0/done) -> (1/added) -> (2/running)
//
//	cm_cl_schedule_add				queue a argr to the schedule-sm
//									(done) -> (added)		// NSU
//									(done) -> (running)		// ASU
//
//	cm_cl_schedule_done				notify the schedule-sm about finishing the job
//									(added) -> (done)
//									(running) -> (done)
//
//	cm_cl_schedule_timeout			callback for schedule-sm timer
//									calls cm_cl_arsm () for starting the job
//									(added) -> (running)
//
//	cm_cl_schedule_fsu				a fsu_hello.ind had arrived for this argr
//									calls cm_cl_arsm () for starting the job
//									(added) -> (running)
//
//	cm_cl_schedule_allowed			check all channel-requirements for starting the ar (note: no DINFO possible)
//	cm_cl_schedule_ar_allowed		check all ar-requirements for starting the ar
//									(except timing)
//
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_schedule_allowed (
	CM_CHANNEL_CONST_PTR_TYPE  channel
	)
{
		/* too many running ? */
	if (channel->usr.cl.schedule.running >= channel->usr.cl.schedule.max_running)
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "wf_running(%u) >= max_running(%u), wait"
			, channel->usr.cl.schedule.running
			, channel->usr.cl.schedule.max_running
			);
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_schedule_ar_allowed (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_CL_DOWN_INFO_PTR_TYPE  dinfo
	)
{
	LSA_UNUSED_ARG (argr);

	/* AP00496890: check only if link is up, else wait */

	/* CM_ERROR_CAUSE_LINK_DOWN */

	if (! cm_link_status_is_up (channel))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "link not up, must wait");
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_LNK_DOWN, CM_ERROR_CAUSE_LINK_DOWN, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
		return LSA_FALSE;
	}

	if (cm_pdusr_is_reset_to_factory (channel)  ||  (argr->cm.cl.dev->wf & CM_CL_DEV__IS_RESET_TO_FACTORY))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "the PDev is in state reset-to-factory, must wait");
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_RESET, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
		return LSA_FALSE;
	}


	if (cm_is_null (argr->cm.cl.mtd))
	{
		/* CM_ERROR_CAUSE_IOC_ABORTED */

		if (! cm_oha_ip_suite_is_notnull_nonconflicting_and_expected (channel))
		{
			CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "ip-suite not valid, must wait");
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
			return LSA_FALSE;
		}

		if (argr->cm.cl.dev->device_mode & CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_YES)
		{
			LSA_UINT32  local_ip, local_subnet_mask, local_gateway_ip;

			if (!cm_oha_get_ip_suite(channel, &local_ip, &local_subnet_mask, &local_gateway_ip) ||
				local_subnet_mask != argr->cm.cl.dev->dev_addr.device_subnet_mask)
			{
				CM_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "local_subnet_mask(0x%x) not identical with device_subnet_mask(0x%x), must wait"
					, local_subnet_mask, argr->cm.cl.dev->dev_addr.device_subnet_mask
					);
				CM_CL_DINFO_SET(dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
				return LSA_FALSE;
			}
		}

		if (! cm_oha_name_of_station_is_valid (channel))
		{
			CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "name-of-station not valid, must wait");
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_NAME, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
			return LSA_FALSE;
		}

		if (! cm_pdusr_is_pdev_stable (channel)) /* Single deployment: PDev may own a mtd */
		{
			CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "the PDev is not stable, must wait");
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_RESET, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
			return LSA_FALSE;
		}

		if (! cm_pdcl_pdev_is_ready_for_schedule (channel))
		{
			CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "the PDev has ARP, must wait");
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_RESET, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
			return LSA_FALSE;
		}
	}
	else
	{
		/* do addresstailor / machinetailor */
		if (! cm_cl_mp_tailor (channel, argr, argr->cm.cl.mtd, dinfo))
		{
			/* some needed tailoring information is missing, see CM_ERROR_CAUSE_MP_xxx*/
			return LSA_FALSE;
		}
		/* all alias / records tailored */
	}

	CM_CL_DINFO_SET2 (dinfo, CM_AR_REASON_NONE, CM_ERROR_CAUSE_NONE, CM_PNIO_ERR_NONE, 0);

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_schedule_timeout (
	CM_ONESHOT_PTR_TYPE  timeout,
	LSA_VOID_PTR_TYPE  void_channel
	)
{
	CM_CHANNEL_PTR_TYPE  channel = (CM_CHANNEL_PTR_TYPE)(void_channel);
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_LIST_ENTRY_PTR_TYPE  entry;
	CM_LIST_ENTRY_TYPE  schedule_list;

	LSA_UNUSED_ARG (timeout);
	CM_ASSERT (cm_is_not_null (channel));

		/* check ready CL-ARs */
	CmListInitialize (& schedule_list);
	CmListAppend (& schedule_list, & channel->usr.cl.schedule.ready2run_list);

	while (! CmListIsEmpty (& schedule_list))
	{
		CmListRemoveHead (& schedule_list, entry, CM_LIST_ENTRY_PTR_TYPE);

			/* calc real argr ptr */
		argr = CM_CONTAINING_STRUCT (entry, CM_AR_GRAPH_PTR_TYPE, cm.cl.schedule.link);

		if (cm_cl_schedule_allowed (channel))
		{
			if (argr->cm.cl.schedule.wait_ticks > 0)
			{
				argr->cm.cl.schedule.wait_ticks -= 1;
			}

			if (argr->cm.cl.schedule.wait_ticks == 0)
			{
				CM_CL_DOWN_INFO_TYPE  dinfo;

				CM_ASSERT (argr->cm.cl.schedule.in_schedule == 1/*added*/);

				if (cm_cl_schedule_ar_allowed (channel, argr, &dinfo))
				{
					argr->cm.cl.schedule.in_schedule = 2/*running*/;
					channel->usr.cl.schedule.running += 1;
					cm_cl_arsm (argr);
				}
				else if (cm_cl_ar_event_bad_is_different (argr, &dinfo))
				{
					CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
					cm_cl_arsm_down (argr, &dinfo);
				}
				else
				{
					argr->cm.cl.schedule.wait_ticks = 10; /*???*/
#ifdef CM_MESSAGE
					/*VV: schedule_add() sollte hier jetzt gehen, aber zuvor in_schedule richtig stellen */
#endif
					CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
					cm_cl_bindsm_set_unbound (argr, dinfo.reason_code, dinfo.error_cause);
					cm_cl_bindsm_trigger (argr, 0);
				}
			}
			else
			{
				CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
			}
		}
		else
		{
			CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
		}
	}

	if (! CmListIsEmpty (& channel->usr.cl.schedule.ready2run_list))
	{
		cm_oneshot_trigger (& channel->usr.cl.schedule.oneshot_timer, channel->usr.cl.schedule.timeout_msec);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_schedule_init (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  max_running,
	LSA_UINT16  wait_reconnect_sec,
	LSA_UINT16  schedule_timeout_msec
	)
{
	LSA_UINT32  tmp;

	cm_oneshot_init (& channel->usr.cl.schedule.oneshot_timer, cm_cl_schedule_timeout, channel);
	channel->usr.cl.schedule.running = 0;
	CmListInitialize (& channel->usr.cl.schedule.ready2run_list);

	channel->usr.cl.schedule.max_running = max_running;
	channel->usr.cl.schedule.timeout_msec = schedule_timeout_msec;
	tmp = ((LSA_UINT32)wait_reconnect_sec * 1000) / schedule_timeout_msec;
	if (tmp == 0)
	{
		tmp = 1;
	}
	channel->usr.cl.schedule.wait_reconnect_ticks = CM_CAST_U16(tmp);

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "max_running(%u) timeout_msec(%u) wait_reconnect_ticks(%u)"
		, channel->usr.cl.schedule.max_running
		, channel->usr.cl.schedule.timeout_msec
		, channel->usr.cl.schedule.wait_reconnect_ticks
		);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_schedule_add (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	CM_ASSERT (argr->cm.cl.schedule.in_schedule == 0/*done*/);

	if (argr->cm.cl.schedule.reschedule_ticks == 0) /*is ASU*/
	{
		CM_CL_DOWN_INFO_TYPE  dinfo;

		if (cm_cl_schedule_allowed (channel) && cm_cl_schedule_ar_allowed (channel, argr, &dinfo))
		{
			argr->cm.cl.schedule.in_schedule = 2/*running*/;
			channel->usr.cl.schedule.running += 1;

			CM_ASSERT (argr->cm.cl.state == CM_DST_WF_SCHEDULE);
			cm_cl_arsm (argr);
			return;
		}
		else
		{
			/* re-calculate reschedule_ticks and add do NSU */
			argr->cm.cl.schedule.reschedule_ticks = 2/*!*/;
		}
	}

	/* NSU */
	CM_ASSERT (argr->cm.cl.schedule.reschedule_ticks > 0);
	argr->cm.cl.schedule.wait_ticks = argr->cm.cl.schedule.reschedule_ticks;

	argr->cm.cl.schedule.in_schedule = 1/*added*/;

	if (CmListIsEmpty (& channel->usr.cl.schedule.ready2run_list))
	{
		CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
		cm_oneshot_trigger (& channel->usr.cl.schedule.oneshot_timer, channel->usr.cl.schedule.timeout_msec);
	}
	else
	{
		CmListInsertTail (& channel->usr.cl.schedule.ready2run_list, & argr->cm.cl.schedule.link);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_schedule_fsu (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	/*	note: a fsu-startup starts the device
	//		ignores 'max_running' but increments 'running'.
	//		so running could be > max_running
	*/

	CM_ASSERT (argr->cm.cl.state == CM_DST_WF_SCHEDULE);

	if (argr->cm.cl.schedule.in_schedule == 1/*added*/)
	{
		/* always true */
		if (cm_link_status_is_up (channel)) /* AP00496890: check only if link is up, else wait */
		{
			/* skip nare-phase, start connect-phase */

			argr->cm.cl.schedule.wait_ticks = 0;
			CmListRemoveEntry (& argr->cm.cl.schedule.link);
			argr->cm.cl.schedule.in_schedule = 2/*running*/;
			channel->usr.cl.schedule.running += 1;

			argr->cm.cl.state = CM_DST_NARE;
			cm_cl_bindsm_startup_optimized (argr, LSA_TRUE);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_schedule_done (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	if (argr->cm.cl.schedule.in_schedule == 1/*added*/)
	{
		argr->cm.cl.schedule.in_schedule = 0/*done*/;

		CM_ASSERT (argr->cm.cl.state == CM_DST_WF_SCHEDULE || argr->cm.cl.state == CM_DST_WF_DOWN);
		CmListRemoveEntry (& argr->cm.cl.schedule.link);
		if (CmListIsEmpty (& channel->usr.cl.schedule.ready2run_list))
		{
			cm_oneshot_stop (& channel->usr.cl.schedule.oneshot_timer);
		}
	}
	else if (argr->cm.cl.schedule.in_schedule == 2/*running*/)
	{
		argr->cm.cl.schedule.in_schedule = 0/*done*/;

		CM_ASSERT (channel->usr.cl.schedule.running > 0);
		channel->usr.cl.schedule.running -= 1; /* incremented by cm_cl_schedule_timeout() */
	}
}


/*------------------------------------------------------------------------------
// query functions
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
static CM_AR_GRAPH_PTR_TYPE
cm_cl_ar_from_device_ (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	LSA_BOOL  elog_if_not_found
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev;

	CM_ASSERT (cm_is_not_null (channel));

	dev = cm_cl_dev_from_device_nr (channel, device_nr);

	if (cm_is_not_null(dev) && cm_is_not_null (dev->argr))
	{
		//CM_ASSERT (argr->ar_nr == ar_nr);
		CM_ASSERT (dev->argr->device_nr == device_nr);
		return dev->argr;
		//}
	}

	if (elog_if_not_found)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "dev(%u) not found", device_nr);
	}
	else
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) not found", device_nr);
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_PTR_TYPE
cm_cl_ar_from_device_no_elog (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr
	)
{
	return cm_cl_ar_from_device_ (channel, device_nr, LSA_FALSE);
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_PTR_TYPE
cm_cl_ar_from_device (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr
	)
{
	return cm_cl_ar_from_device_ (channel, device_nr, LSA_TRUE);
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_PTR_TYPE
cm_cl_ar_from_ar_uuid (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CLRPC_UUID_CONST_PTR_TYPE  ar_uuid,
	LSA_BOOL  elog_if_not_found
	)
{
	LSA_UINT16  i;

	CM_ASSERT (cm_is_not_null (channel));

	for (i = 0; i < channel->usr.cl.dev_cnt; ++i)
	{
		CM_CL_DEVICE_PTR_TYPE  dev;

		dev = & channel->usr.cl.dev[i];

		if (dev->state == CM_CL_DEV_ST_NOI)
		{
			continue;
		}
		if (cm_is_not_null(dev->argr))
		{
			if (cm_uuid_equal (& dev->argr->ar.ar_uuid, ar_uuid))
			{
				CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
					, ar_uuid->time_low, ar_uuid->time_mid, ar_uuid->time_hi_and_version
					, ar_uuid->clock_seq_hi_and_reserved, ar_uuid->clock_seq_low
					, ar_uuid->node[0], ar_uuid->node[1], ar_uuid->node[2]
					, ar_uuid->node[3], ar_uuid->node[4], ar_uuid->node[5]
					);
				CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "ar_uuid found within dev(%u)", dev->argr->device_nr
					);
				return dev->argr;
			}
		}
	}

	if (elog_if_not_found)
	{
		CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) not found"
			, ar_uuid->time_low, ar_uuid->time_mid, ar_uuid->time_hi_and_version
			, ar_uuid->clock_seq_hi_and_reserved, ar_uuid->clock_seq_low
			, ar_uuid->node[0], ar_uuid->node[1], ar_uuid->node[2]
			, ar_uuid->node[3], ar_uuid->node[4], ar_uuid->node[5]
			);
	}
	else
	{
		CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) not found"
			, ar_uuid->time_low, ar_uuid->time_mid, ar_uuid->time_hi_and_version
			, ar_uuid->clock_seq_hi_and_reserved, ar_uuid->clock_seq_low
			, ar_uuid->node[0], ar_uuid->node[1], ar_uuid->node[2]
			, ar_uuid->node[3], ar_uuid->node[4], ar_uuid->node[5]
			);
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16		/* ret: count of argr */
cm_cl_dev_ar_count (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev,

	LSA_UINT16 * cnt_passive,
	LSA_UINT16 * cnt_wf_data,
	LSA_UINT16 * cnt_indata,
	LSA_UINT16 * cnt_down
	)
{
	LSA_UINT16 cnt, passive, wf_data, indata, down;


	cnt = passive = wf_data = indata = down = 0;

	if (cm_is_not_null(dev->argr))
	{
		cnt += 1;
		switch (dev->argr->cm.cl.usr_state)
		{
		case CM_CL_AR_PASSIVE:		passive += 1; break;
		case CM_CL_AR_WF_IN:		wf_data += 1; break;
		case CM_CL_AR_INDATA:		indata += 1; break;
		case CM_CL_AR_WF_DOWN:		down += 1; break;
		default:					CM_FATAL1(dev->argr->cm.cl.usr_state); break;
		}
	}

	if (cm_is_not_null (cnt_passive)) { *cnt_passive = passive; };
	if (cm_is_not_null (cnt_wf_data)) { *cnt_wf_data = wf_data; };
	if (cm_is_not_null (cnt_indata)) { *cnt_indata = indata; };
	if (cm_is_not_null (cnt_down)) { *cnt_down = down; };
	return cnt;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_ar_event_build_diff_list (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE  build_sub,
	CM_LIST_ENTRY_PTR_TYPE  diff_list
	)
{
	CM_UPPER_CL_DIFF_AP_PTR_TYPE  diff_ap;
	CM_UPPER_CL_DIFF_MODULE_PTR_TYPE  diff_mod;
	CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CmListInitialize (diff_list);

	for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				if (cm_is_null (build_sub)  ||  (build_sub == sub))
				{
					if (sub->cl.real_sub_state != CM_SUB_STATE_GOOD)
					{
						diff_ap = cm_pd_diff_ap_find_append (channel, diff_list, ap->api);
						diff_mod = cm_pd_diff_module_find_append (channel
							, & diff_ap->module_list
							, mod->slot_nr, mod->ist.mod_ident
							);
						diff_sub = cm_pd_diff_submodule_find_append (channel
							, & diff_mod->submodule_list
							, sub->subslot_nr, sub->cl.real_sub_state, sub->cl.real_sub_ident
							);
						CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "ar_event_build_diff_list: dev(%u) api(%u) slot(%u) subslot(0x%x) module_ident(0x%x) submodule_ident(0x%x) sub_state(0x%04x)"
							, argr->device_nr, ap->api, mod->slot_nr, sub->subslot_nr, mod->ist.mod_ident, sub->cl.real_sub_ident, sub->cl.real_sub_state
						);
						LSA_UNUSED_ARG (diff_sub);
					}
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
// ar-graph callback
//----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_ar_callback (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  response,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	switch (CM_RQB_GET_OPCODE (upper))
	{
	case CM_OPC_CL_RECORD_READ:
			/* ignore response, prm.read_state must fit */
		if (upper->args.cl.read->rec.record_index == CM_RECORD_INDEX_RPC_TRIGGER)
		{
			cm_cl_prm_read_rpc_trigger_cnf (argr, upper);
			return;
		}
		else
		{
				/* if running: retrigger next timeout */
			cm_cl_prm_read_retrigger (argr);
		}
		break;

	case CM_OPC_CL_RECORD_READ_QUERY:
	case CM_OPC_CL_RECORD_WRITE:
		{
				/* if running: retrigger next timeout */
			cm_cl_prm_read_retrigger (argr);
		}
		break;

	default:
		break;
	}

	cm_callback (argr->ar_com.channel, response, & upper);
}


/*------------------------------------------------------------------------------
//
// (NOI) -<add>-> (PASSIVE) -<act>-> (WF_DATA) -<pd>-> [PRE_DATA] -<id>-> (DATA)
//                  ^                  ^   |                  |              |
//                  |                  |  <ab>               <po>           <io>
//                  |                  |   |                  |              |
//                  |                  |   v                  |              |
//                  +-<pas>----------(WF_RSP)<--+-------------+--------------+
//                                     ^   |
//                                     +<r>+
//
//	<add> .. device-add
//	<act> .. device-control /activate
//	<pas> .. device-control /passivate.cnf
//
//	<ab> .. ar_abort.ind
//	<pd> .. [opt] pre_data.ind
//	<po> .. [opt] pre_offline.ind
//	<id> .. in_data.ind
//	<io> .. offline.ind
//	<r>  .. $any.rsp
//
//----------------------------------------------------------------------------*/
/*!-----------------------------------------------------------------------------
 *	Events to the CM-User
 *	\dot
 *	digraph {
 *	node [shape="ellipse", fontsize=8, style="filled", fillcolor="cyan", width=0.9];
 *	edge [fontsize=8];
 *	"SCHEDULE"		-> "WF-IN"			[label="schedule timeout"];
 *	"WF-IN"			-> "IN-DATA"		[label="InData.ind"];
 *	"WF-IN"			-> "WF-RTC3"		[label="PreData.ind"];
 *	"WF-IN"			-> "WF-RESPONSE"	[label="any down reason\n => Abort.ind, only if diagnosis is changed"];
 *	"WF-RESPONSE"	-> "WF-RESPONSE"	[label="any open rsp"];
 *	"WF-RESPONSE"	-> "SCHEDULE"		[label="no more open rsp"];
 *	"WF-RTC3"		-> "WF-RESPONSE"	[label="any down reason\n => PreOffline.ind"];
 *	"WF-RTC3"		-> "IN-DATA"		[label="InData.ind"];
 *	"IN-DATA"		-> "WF-RESPONSE"	[label="any down reason\n => Offline.ind"];
 *	}
 *	\enddot
 *----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_OPCODE_TYPE  notify
	)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_EVENT_PTR_TYPE  upper_event;
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	if (notify == CM_OPC_CL_AR_ABORT_IND)
	{
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_OFFLINE_IND))
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PRE_OFFLINE_IND);
			notify = CM_OPC_CL_AR_PRE_OFFLINE_IND;
		}
		if (! cm_cl_ar_event_bad_has_changed (argr))
		{
			/* nothing to do */
			return;
		}
	}

	upper = cm_cl_dev_event_get (argr->cm.cl.dev);
	if (cm_is_null (upper))
	{
		CM_FATAL1 (notify);
		return;
	}

	upper_event = upper->args.cl.event_ind;
	CM_ASSERT (cm_is_not_null (upper_event));

		/* data for all events */
	upper_event->device_nr		= argr->device_nr;
	upper_event->ar_nr			= argr->ar_nr;
	upper_event->session_key	= argr->session_key;

	switch (notify)
	{
	case CM_OPC_CL_AR_ABORT_IND:
		{
			CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE  error_cause = & upper_event->u.cl.ar_abort.error_cause;
			cm_cl_ar_event_get_abort_error_cause (argr, error_cause);
			cm_cl_ar_event_get_abort_ip (argr, & upper_event->u.cl.ar_abort.device_ip);

			CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) skey(%u) upper_event(%u/AR_ABORT_IND) reason(%u) error_cause(0x%x) device_ip(0x%x)" /* note: error_cause is BigEndian */
				, argr->device_nr, argr->session_key, notify
				, error_cause->reason_code, CM_NTOH16 (error_cause->error_cause), upper_event->u.cl.ar_abort.device_ip
				);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_AR_ABORT_RSP);
		}
		break;

	case CM_OPC_CL_AR_PRE_OFFLINE_IND:
		{
			CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE  error_cause = & upper_event->u.cl.ar_abort.error_cause;
			cm_cl_ar_event_get_abort_error_cause (argr, error_cause);
			cm_cl_ar_event_get_abort_ip (argr, & upper_event->u.cl.ar_abort.device_ip);

			CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) skey(%u) upper_event(%u/AR_PRE_OFFLINE_IND) reason(%u) error_cause(0x%x) device_ip(0x%x)"
				, argr->device_nr, argr->session_key, notify
				, error_cause->reason_code, CM_NTOH16 (error_cause->error_cause), upper_event->u.cl.ar_abort.device_ip
				);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRE_OFFLINE_RSP);
		}
		break;

	case CM_OPC_CL_AR_OFFLINE_IND:
		{
			CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE  error_cause = & upper_event->u.cl.ar_abort.error_cause;
			cm_cl_ar_event_get_abort_error_cause (argr, error_cause);
			cm_cl_ar_event_get_abort_ip (argr, & upper_event->u.cl.ar_abort.device_ip);

			CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) new-skey(%u) upper_event(%u/AR_OFFLINE_IND) reason(%u) error_cause(0x%x) device_ip(0x%x)"
				, argr->device_nr, argr->session_key, notify
				, error_cause->reason_code, CM_NTOH16 (error_cause->error_cause), upper_event->u.cl.ar_abort.device_ip
				);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_OFFLINE_RSP);
		}
		break;

	case CM_OPC_CL_AR_PRE_DATA_IND:
		{
			cm_cl_ar_event_build_diff_list (channel, argr, LSA_NULL
				, & upper_event->u.cl.in_data.diff_list
				);
			cm_pd_difflist_to_moddiffblock (channel, argr
				, & upper_event->u.cl.in_data.diff_list
				, & upper_event->u.cl.in_data.mod_diff_block_length
				, & upper_event->u.cl.in_data.mod_diff_block
				);

			cm_cl_ar_event_get_indata_ip (argr, & upper_event->u.cl.in_data.ip_suite);

			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) skey(%u) upper_event(%u/AR_PRE_DATA_IND)  ip-addr(0x%08x)"
				, argr->device_nr, argr->session_key, notify
				, upper_event->u.cl.in_data.ip_suite.device_ip
				);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRE_DATA_RSP);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_OFFLINE_IND);
		}
		break;

	case CM_OPC_CL_AR_IN_DATA_IND:
		{
			CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_OFFLINE_IND);

			cm_cl_ar_event_build_diff_list (channel, argr, LSA_NULL
				, & upper_event->u.cl.in_data.diff_list
				);
			cm_pd_difflist_to_moddiffblock (channel, argr
				, & upper_event->u.cl.in_data.diff_list
				, & upper_event->u.cl.in_data.mod_diff_block_length
				, & upper_event->u.cl.in_data.mod_diff_block
				);

			cm_cl_ar_event_get_indata_ip (argr, & upper_event->u.cl.in_data.ip_suite);

			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) skey(%u) upper_event(%u/AR_IN_DATA_IND)  ip-addr(0x%08x)"
				, argr->device_nr, argr->session_key, notify
				, upper_event->u.cl.in_data.ip_suite.device_ip
				);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_NEW_SESSION);

				/* we must wait for the normal user.rsp */
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_IN_DATA_RSP);

			CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_IN);
			argr->cm.cl.usr_state = CM_CL_AR_INDATA;
		}
		break;

	case CM_OPC_CL_AR_PRM_BEGIN_IND:
		{
			CM_UPPER_CL_PRM_BEGIN_PTR_TYPE prm_begin = &upper_event->u.cl.prm_begin;
			prm_begin->prm_mode = cm_cl_prm_mode (argr); /* CM_CL_PRMMODE_AR,CM_CL_PRMMODE_SUBMODULE,CM_CL_PRMMODE_COC */

			prm_begin->api        = argr->cm.cl.prm.submod.api;
			prm_begin->slot_nr    = argr->cm.cl.prm.submod.slot_nr;
			prm_begin->subslot_nr = argr->cm.cl.prm.submod.subslot_nr;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_BEGIN_RSP);

			switch (prm_begin->prm_mode)
			{
			case CM_CL_PRMMODE_AR:
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "PrmUpdate: dev(%u) skey(%u) upper_event(%u/AR_PRM_BEGIN_IND) AR"
					, argr->device_nr, argr->session_key, notify
					);
				break;

			case CM_CL_PRMMODE_SUBMODULE:
			case CM_CL_PRMMODE_COC:
				CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "PrmUpdate: dev(%u) skey(%u) upper_event(%u/AR_PRM_BEGIN_IND) prm_mode(0x%x) submod(%u/%u/0x%x)"
					, argr->device_nr, argr->session_key, notify, prm_begin->prm_mode
					, prm_begin->api, prm_begin->slot_nr, prm_begin->subslot_nr
					);
				break;
			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		break;

	case CM_OPC_CL_AR_PRM_INDATA_IND:
		{
			CM_UPPER_CL_PRM_INDATA_PTR_TYPE prm_indata = &upper_event->u.cl.prm_indata;

			prm_indata->prm_mode   = cm_cl_prm_mode (argr); /* CM_CL_PRMMODE_AR,CM_CL_PRMMODE_SUBMODULE,CM_CL_PRMMODE_COC */
			prm_indata->api        = argr->cm.cl.prm.submod.api;
			prm_indata->slot_nr    = argr->cm.cl.prm.submod.slot_nr;
			prm_indata->subslot_nr = argr->cm.cl.prm.submod.subslot_nr;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP);

			if(prm_indata->prm_mode == CM_CL_PRMMODE_AR)
			{
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "PrmUpdate: dev(%u) skey(%u) upper_event(%u/AR_PRM_INDATA_IND) AR"
					, argr->device_nr, argr->session_key, notify
					);

				cm_cl_ar_event_build_diff_list (channel, argr, LSA_NULL
					, & prm_indata->diff_list
					);
				cm_pd_difflist_to_moddiffblock (channel, argr
					, & prm_indata->diff_list
					, & prm_indata->mod_diff_block_length
					, & prm_indata->mod_diff_block
					);
			}
			else /* case CM_CL_PRMMODE_SUBMODULE or CM_CL_PRMMODE_COC */
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  build_sub;

				CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "PrmUpdate: dev(%u) skey(%u) upper_event(%u/AR_PRM_INDATA_IND) prm_mode(0x%x) submod(%u/%u/0x%x)"
					, argr->device_nr, argr->session_key, notify, prm_indata->prm_mode
					, prm_indata->api, prm_indata->slot_nr, prm_indata->subslot_nr
				);

				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & build_sub
					);
				CM_ASSERT (cm_is_not_null (build_sub));

				cm_cl_ar_event_build_diff_list (channel, argr, build_sub
					, & prm_indata->diff_list
					);

				cm_pd_difflist_to_moddiffblock (channel, argr
					, & prm_indata->diff_list
					, & prm_indata->mod_diff_block_length
					, & prm_indata->mod_diff_block
					);

			}
		}
		break;

	case CM_OPC_CL_AR_DATA_STATUS_IND: /* why not using ? */
	default:
		CM_FATAL ();
		return;
	}

	CM_RQB_SET_OPCODE (upper, notify);
	cm_callback (channel, CM_OK, & upper);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_prm_alarm_event (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_ALARM_PTR_TYPE alarm;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;

	CM_ASSERT (cm_is_not_null (argr->cm.cl.prm_alarm));
	CM_ASSERT (CM_RQB_GET_OPCODE (argr->cm.cl.prm_alarm) == CM_OPC_CL_ALARM_IND);

	upper = argr->cm.cl.prm_alarm;
	argr->cm.cl.prm_alarm = LSA_NULL;

	alarm = upper->args.cl.alarm;
	CM_ASSERT (CmListIsEmpty (& alarm->u.cl.diff_list));

	cm_ar_graph_find_api_slot_subslot (argr
		, alarm->api, alarm->slot_nr, alarm->subslot_nr
		, LSA_NULL, LSA_NULL, & sub
		);
	if (cm_is_null (sub)) { CM_FATAL(); /* pgm-error */ return; }

	switch (alarm->alarm_type)
	{
	case CM_ALARM_TYPE_RELEASED:
	case CM_ALARM_TYPE_PLUG:
		if (alarm->alarm_type == CM_ALARM_TYPE_RELEASED)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE, "AlarmEvent: dev(%u) alarm(RELEASED).ind"
				, argr->device_nr
				);
		}
		else
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE, "AlarmEvent: dev(%u) alarm(PLUG).ind"
				, argr->device_nr
				);
		}
		if (cm_is_not_null (sub))
		{
			cm_cl_ar_event_build_diff_list (channel, argr, sub, & alarm->u.cl.diff_list);
		}
		cm_pd_difflist_to_moddiffblock (channel, argr
			, & alarm->u.cl.diff_list
			, & alarm->u.cl.mod_diff_block_length, & alarm->u.cl.mod_diff_block
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF); /* see cm_cl_prm_trigger() */
		break;

	case CM_ALARM_TYPE_RETURN_OF_SUBMODULE:
		/* see CM_CL_DO_ALARM_ROSAL_IND */
		CM_FATAL1 (alarm->alarm_type);
		break;

	default:
		CM_FATAL1 (alarm->alarm_type);
		break;
	}

		/* notify cm-user */
	cm_cl_alarm_set_wait_for (argr, alarm->alarm_priority);
	cm_callback (channel, CM_OK, & upper);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_arsm_down (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_CL_DOWN_INFO_CONST_PTR_TYPE  dinfo
	)
{
	if (argr->cm.cl.in_down == 0)
	{
		argr->cm.cl.in_down = 1;

		CM_ASSERT (cm_is_not_null (dinfo));

		cm_cl_bindsm_set_unbound (argr, dinfo->reason_code, dinfo->error_cause);

		switch (argr->cm.cl.usr_state)
		{
		case CM_CL_AR_WF_DOWN:
			cm_cl_arsm (argr); /* trigger down */
			break;

		case CM_CL_AR_WF_IN:
		case CM_CL_AR_INDATA:
				/* called from cl_ar_com_down() */
			if (cm_ar_com_is_down (argr))
			{
				cm_cl_ar_event_set_bad (argr, dinfo);

				cm_ar_com_down_debug_data_fmt (argr, dinfo->module_id, dinfo->line_nr, dinfo->reason_code, dinfo->pnio_status);

				cm_cl_arsm (argr); /* set state to WF_DOWN */
			}

				/* my own down. e.g. rpc.cnf(-) */
			else
			{
				cm_cl_ar_event_set_bad (argr, dinfo);

				cm_ar_com_down_debug_data_fmt (argr, dinfo->module_id, dinfo->line_nr, dinfo->reason_code, dinfo->pnio_status);

				cm_ar_com_down2 (argr, dinfo->reason_code, 0/*no cyclce_counter*/);

				cm_cl_arsm (argr); /* set state to WF_DOWN */
			}
			break;

		case CM_CL_AR_PASSIVE:
			break;

		default:
			CM_ASSERT(0);
			break;
		}

		argr->cm.cl.in_down = 0;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_cancel_alarm_queues (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_CL_AR_ALARM_PTR_TYPE  a_prio
	)
{
	if (cm_is_not_null (a_prio->early_alarm))
	{
		cm_cl_alarm_provide (argr, a_prio->early_alarm);
		a_prio->early_alarm = LSA_NULL;
	}

	if (cm_is_not_null (a_prio->acp_ind))
	{
		cm_acp_request_lower (channel, ACP_OPC_ALARM_IND_RES_PROVIDE, 0/*unused*/, a_prio->acp_ind);
		a_prio->acp_ind = LSA_NULL;
	}

	while (! CmListIsEmpty (& a_prio->alarm_list))
	{
		CM_UPPER_RQB_PTR_TYPE  upper;
		CmListRemoveHead (& a_prio->alarm_list, upper, CM_UPPER_RQB_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (upper));
		cm_callback (channel, CM_ERR_SESSION, & upper);
	}
}

/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_cancel_queues (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cancel queued from dev(%u)"
		, argr->device_nr
		);

	cm_cl_schedule_done (argr);

	if (cm_is_not_null (argr->cm.cl.prm_alarm))
	{
		cm_cl_alarm_provide (argr, argr->cm.cl.prm_alarm);
		argr->cm.cl.prm_alarm = LSA_NULL;
	}

	if (cm_is_not_null (argr->cm.cl.rosal_alarm))
	{
		cm_cl_alarm_provide (argr, argr->cm.cl.rosal_alarm);
		argr->cm.cl.rosal_alarm = LSA_NULL;
	}

	CM_ASSERT (cm_is_null (argr->cm.cl.prm.prm_keep_upd));

	cm_cl_ar_cancel_alarm_queues (channel, argr, & argr->cm.cl.alarm[CM_ALARM_PRIORITY_LOW]);
	cm_cl_ar_cancel_alarm_queues (channel, argr, & argr->cm.cl.alarm[CM_ALARM_PRIORITY_HIGH]);
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_ar_attach_to_pd (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ar_iocr;
	LSA_UINT16  send_clock = 0;

	for (CmListForeach (ar_iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		if (ar_iocr->cm.adpt.send_clock > 0)
		{
			send_clock = ar_iocr->cm.adpt.send_clock;
			break;
		}
	}

	switch (cm_pdusr_attach (channel, send_clock))
	{
	case CM_OK:
	case CM_OK_PENDING:
		return LSA_TRUE;

	case CM_ERR_PARAM:
	case CM_ERR_ALLOC_LOWER:
	default:
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_ar_detach_from_pd (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	LSA_UNUSED_ARG (argr);

	cm_pdusr_detach (channel);

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_ar_check_abort (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
		/* user notify offline.ind is delayed to the last action, AP00215718 */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_NEW_SESSION))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_NEW_SESSION);

			/* moved from cm_cl_device_event() to here */
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_DOWN);

			/* build session_key of next session */
		CM_COUNTER_INCREMENT (argr->session_key);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_IND);
	}

		/* check cm - down while add */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_AR_ADD))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf cm_cl_ar_com_start_done()", argr->device_nr);
		return LSA_FALSE;
	}

		/* need net view: first stop provider, then send ERR-RTA-PDU aka alarm-close.req */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_FRAMES_PASSIVATE))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_FRAMES_PASSIVATE);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_FRAMES_PASSIVATE_DONE);

		cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_PASSIVATE, 0);
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_FRAMES_PASSIVATE_DONE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf frames passivate", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_PASSIVATE_AUX))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf prov passivate aux", argr->device_nr);
		return LSA_FALSE;
	}

		/* has moved to: so fast as possible, but after passivate frames */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_IND))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_OFFLINE_IND);

		cm_cl_ar_event (argr, CM_OPC_CL_AR_OFFLINE_IND);
	}

	/* checking CM_CL_WF_OFFLINE_RSP later, first start stop-alarms */

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_ALARMS_STOP))
	{
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF)) /* AP00427133 */
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-release.cnf prior to alarm-close", argr->device_nr);
			return LSA_FALSE;
		}

		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_ALARMS_STOP);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARMS_STOP_DONE);

		cm_arcq_request (argr, CM_ARCQ_OPC_ALARM_REMOVE, 0);
	}

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARMS_STOP_DONE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf alarms stop", argr->device_nr);
		return LSA_FALSE;
	}

		/* check user rsp */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_AR_ABORT_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf ar_abort.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_IN_DATA_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf in_data.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRE_DATA_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf pre_data.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_DATA_STATUS_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf apdu_status.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRE_OFFLINE_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf pre_offline.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_OFFLINE_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf offline.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_LOW_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf alarm-low.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_ALARM_HIGH_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf alarm-high.rsp", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf prm-indata.rsp", argr->device_nr);
		return LSA_FALSE;
	}

		/* check nare-calls */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf nare.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf nare_cancel.cnf", argr->device_nr);
		return LSA_FALSE;
	}

		/* check rpc-calls  */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-rebind.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-connect.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-release.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-prm-begin.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-prm-end.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-read/write.cnf", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CANCEL_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf rpc-cancel.cnf", argr->device_nr);
		return LSA_FALSE;
	}

		/* check prm-done */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf prm-done", argr->device_nr);
		return LSA_FALSE;
	}

		/* check cm - frame */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf prov activate", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE_AUX))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf prov activate aux", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf cons activate", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf cons activate + set2unk", argr->device_nr);
		return LSA_FALSE;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf plug alarm-ack.cnf", argr->device_nr);
		return LSA_FALSE;
	}

		/* check edd */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_HELLO_FILTER_REPLACE))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): wf hello-filter-replace", argr->device_nr);
		return LSA_FALSE;
	}

		/* check pdev */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PD_DETACH))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PD_DETACH);
		cm_cl_ar_detach_from_pd (channel, argr);
	}

		/* cancel upper, provide lower */
	cm_cl_ar_cancel_queues (channel, argr);

		/* prepare next schedule   VV(move to schedule_add()) */
	{
	if (argr->cm.cl.state == CM_DST_WF_DOWN)
	{
		LSA_UINT16  scan_cycle;

		scan_cycle =  CM_CAST_U16 (CM_CL_DEV_SCAN_CYCLE_10MS (argr->cm.cl.dev->device_mode) * 10);

		if (scan_cycle > 0)
		{
			argr->cm.cl.schedule.reschedule_ticks = 0; /*ASU*/
		}
		else
		{
			argr->cm.cl.schedule.reschedule_ticks = channel->usr.cl.schedule.wait_reconnect_ticks; /*NSU*/
		}
	}
	else
	{
#ifdef CM_MESSAGE
	/* TODO: diesen fall gibt es nicht mehr, check this and delete this */
#endif /* CM_MESSAGE */
		argr->cm.cl.schedule.reschedule_ticks = 1; /* NSU: prepare next activate: as fast as possible */
	}
	}

		/* all down */
	CM_ASSERT (argr->cm.cl.wait_for[0]  == 0);
	CM_ASSERT (argr->cm.cl.wait_for[1] == 0);

	cm_ar_com_idle (argr);

		/* argr like passive */
	argr->cm.cl.state = CM_DST_PASSIVE;
	argr->cm.cl.usr_state = CM_CL_AR_PASSIVE;

		/* all outstanding resources done */
	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_dev_indata_ind (
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_CL_TRACE_01 (dev->channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): INDATA", dev->device_nr);
	LSA_UNUSED_ARG (dev);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_dev_offline_ind (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): OFFLINE", dev->device_nr);

	if (cm_is_not_null (dev->upper_ctrl))
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (dev->upper_ctrl) == CM_OPC_CL_DEVICE_CONTROL);
		CM_ASSERT (dev->upper_ctrl->args.cl.dev_control->control_tag == CM_CL_DEVICE_CONTROL_PASSIVATE);

		cm_callback (channel, CM_OK, & dev->upper_ctrl);
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL /* return: true if ar is INDATA; no callback to cm_cl_devsm() */
cm_cl_ar_activate (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	switch (argr->cm.cl.state)
	{
	case CM_DST_PASSIVE:
			/* need by ABORT / DOWN */
		CM_ASSERT ((argr->cm.cl.wait_for[0] & ~(CM_CL_WF_NARE_CNF | CM_CL_WF_REBIND_CNF | CM_CL_WF_RPC_RECORD_CNF )) == 0);
		CM_ASSERT (argr->cm.cl.wait_for[1] == 0);

			/* need by CM-CL-AR */
		cm_cl_ar_reinit (argr);

			/* (re)schedule */
		argr->cm.cl.state = CM_DST_WF_SCHEDULE;
		argr->cm.cl.usr_state = CM_CL_AR_WF_IN;
		cm_cl_schedule_add (argr); /* wait for (re)connect */

		return LSA_FALSE;

	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_IN || argr->cm.cl.usr_state == CM_CL_AR_INDATA);
		return LSA_FALSE;

	case CM_DST_IN_DATA:
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_INDATA);
		return LSA_TRUE;

	case CM_DST_WF_DOWN:
/* cm_cl_device_enter_offline do 'restart' */
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_DOWN);
		return LSA_FALSE;

	default:
		CM_FATAL1 (argr->cm.cl.state);
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL	/* return: true if ar is_passive, no callback to cm_cl_devsm() */
cm_cl_ar_passivate (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  reason
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	switch (argr->cm.cl.state)
	{
	case CM_DST_PASSIVE:
		/* no callback */
		return LSA_TRUE;

	case CM_DST_WF_SCHEDULE:
		cm_cl_schedule_done (argr);
		CM_CL_DINFO_SET (&dinfo, reason, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(reason));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;

	case CM_DST_NARE:
		/* arsm does nare_cancel.req */
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
		cm_cl_schedule_done (argr);
		CM_CL_DINFO_SET (&dinfo, reason, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(reason));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;

	case CM_DST_COM_ACTIVATE:
	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:
		cm_cl_schedule_done (argr);
		if (reason == CM_AR_REASON_PAS  &&  argr->cm.cl.usr_state == CM_CL_AR_INDATA)
		{
			cm_cl_rpc_release (channel, argr);
		}
		CM_CL_DINFO_SET (&dinfo, reason, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(reason));
		cm_cl_arsm_down (argr, &dinfo);
		return LSA_FALSE;

	case CM_DST_WF_DOWN:
		/* only keep upper, done by cm_cl_device_enter_offline */
		return LSA_FALSE;

	default:
		CM_FATAL ();
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_UINT8
cm_cl_dev_ar_state (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev
	)
{
#ifdef CM_MESSAGE
	/* TODO: diese function ist ein streich-kanditat, VV: weniger States mehr Flags! */
#endif /* CM_MESSAGE */

	CM_AR_GRAPH_PTR_TYPE  argr = dev->argr;

	CM_CL_TRACE_03 (dev->channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): SINGLE usr_state(%u) ar_state(%u)"
		, argr->device_nr, argr->cm.cl.usr_state, argr->cm.cl.state
		);

	return argr->cm.cl.usr_state;
}


/*------------------------------------------------------------------------------
// cm_cl_dev - statemachine, handle all next-step of one device
//----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_devsm (
	CM_CL_DEVICE_PTR_TYPE  dev,
	LSA_UINT16  action,
	CM_AR_GRAPH_PTR_TYPE  action_argr
	)
{
	LSA_UINT8  ar_state;
	LSA_UINT32  all_act_wf;
	CM_CHANNEL_PTR_TYPE  channel = dev->channel;

	LSA_UNUSED_ARG (action);
	LSA_UNUSED_ARG (action_argr);

	CM_ASSERT (cm_is_not_null (dev));

	ar_state = cm_cl_dev_ar_state (dev);

	CM_CL_TRACE_06 (dev->channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): state(%u)  dev_ar_state(%u) wf(0x%x)  action(%u) from ar(%u)"
		, dev->device_nr, dev->state, ar_state, dev->wf
		, action, cm_is_not_null(action_argr) ? action_argr->ar_nr : 0
		);

	all_act_wf = CM_CL_DEV__WF_SINGLE_INDATA;


		/* reset-to-factor: listen to all-down */
	if (channel->usr.cl.upper_wf & CM_CUWF_PDEV_SET_DOWN_CNF)
	{
		cm_cl_pdev_set_down_check (channel);
	}

	switch (dev->state)
	{
	case CM_CL_DEV_ST_REMOVING:
		{
		LSA_BOOL  all_passive = LSA_TRUE;

		dev->wf &= ~all_act_wf;

		if (cm_is_not_null(dev->argr))
		{
			if (!cm_cl_ar_passivate (dev->argr, CM_AR_REASON_RMV))
			{
				all_passive = LSA_FALSE;
			}

			if (  CM_CL_WF_IS_SET (dev->argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF)
				||  CM_CL_WF_IS_SET (dev->argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF)
				||  CM_CL_WF_IS_SET (dev->argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
			{
				all_passive = LSA_FALSE;
			}
		}

		if (all_passive)
		{
			cm_cl_dev_offline_ind (channel, dev);
			cm_cl_device_add_rm_done (channel, dev, 0/*trigger*/, CM_OK);
		}
		else
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): removing: not all passive, must wait", dev->device_nr
				);
		}
		}
		break;

	case CM_CL_DEV_ST_PASSIVE:
		{
		LSA_BOOL  all_passive = LSA_TRUE;

		dev->wf &= ~all_act_wf;

		if (cm_is_not_null(dev->argr))
		{
			if (!cm_cl_ar_passivate (dev->argr, CM_AR_REASON_PAS))
			{
				all_passive = LSA_FALSE;
			}
		}

		if (all_passive)
		{
			cm_cl_dev_offline_ind (channel, dev);
		}
		else
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): passivating: not all passive, must wait", dev->device_nr
				);
		}

		}
		break;

	case CM_CL_DEV_ST_ACTIVE:
		switch (dev->wf & all_act_wf)
		{
/*----------------------------------------------------------------------------*/
			/* no cmd given => restart if passive */
		case 0:
			switch (ar_state)
			{
			case CM_CL_AR_PASSIVE:
				cm_cl_dev_activate (dev);
				break;

			case CM_CL_AR_WF_IN:
				cm_cl_dev_activate (dev);
				break;

			case CM_CL_AR_INDATA:
				cm_cl_dev_indata_ind (dev);
				break;

			case CM_CL_AR_WF_DOWN:
				break;

			default:
				CM_FATAL ();
				break;
			}
			return;

/*----------------------------------------------------------------------------*/
			/* single-ar */
		case CM_CL_DEV__WF_SINGLE_INDATA:

			switch (dev->argr->cm.cl.usr_state)
			{
			case CM_CL_AR_PASSIVE:
				cm_cl_ar_activate (dev->argr);
				break;

			case CM_CL_AR_WF_IN:
				/* waiting; who is calling ? */
				break;

			case CM_CL_AR_INDATA:
				dev->wf &= ~ CM_CL_DEV__WF_SINGLE_INDATA;
				cm_cl_dev_indata_ind (dev);
				break;

			case CM_CL_AR_WF_DOWN:
				/* keep wf */
				break;

			default:
				CM_FATAL1 (dev->argr->cm.cl.usr_state);
				break;
			}
			return;

/*----------------------------------------------------------------------------*/
			/* unsupported combination */
		default:
			CM_FATAL1 (dev->wf);
			break;
		}
		 /*lint -unreachable */
		break;

	default:
		CM_FATAL1 (dev->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_activate (
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_ASSERT (dev->state == CM_CL_DEV_ST_ACTIVE);
	dev->wf |= CM_CL_DEV__WF_SINGLE_INDATA;
	cm_cl_devsm (dev, 0, LSA_NULL);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_abort (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev,
	LSA_UINT8  reason_code
	)
{
	CM_CL_DOWN_INFO_TYPE  dinfo;

	if (reason_code == CM_AR_REASON_ABORT)
	{
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
	}
	else if (reason_code == CM_AR_REASON_PDEV)
	{
		/* down by PDInstanceTailorData, see cm_cl_schedule_ar_allowed() */
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_IN_PROGRESS, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
	}
	else if (reason_code == CM_AR_REASON_MTOT)
	{
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MTOT, CM_ERROR_CAUSE_SRD_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MTOT));
	}
	else
	{
		CM_FATAL1 (reason_code);
	}
	cm_cl_arsm_down (dev->argr, &dinfo);
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  cm_cl_ar_fsu_with_at__hello_filter_replace (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	if (cm_is_not_null(dev)  &&  dev->run_mode & CM_CL_RMODE__I_HELLO_FILTER_DYNAMIC)
	{
		if (dev->wf & CM_CL_DEV__HELLO_FILTER_NAME_CHANGED)
		{
			LSA_UINT16  rc;
			CM_CL_DOWN_INFO_TYPE  dinfo;

			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): hello-filter changed", dev->device_nr
				);

			/* note: a new mode EDD_DCP_HELLO_FILTER_REPLACE was not accepted because of
			//	complication within EDDP(-Hera).
			//	So the sequence _OFF and _ON is used.
			*/

			rc = cm_edd_set_dcp_hello_filter (dev->channel, EDD_DCP_HELLO_FILTER_OFF
				, CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr)
				, LSA_NULL, 0
				);
			if (rc != CM_OK_PENDING)
			{
				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
				cm_cl_arsm_down (argr, &dinfo);
				return 0;
			}

			rc = cm_edd_set_dcp_hello_filter (dev->channel, EDD_DCP_HELLO_FILTER_ON
				, CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr)
				, dev->dev_addr.tailored_device_name, dev->dev_addr.tailored_device_name_length
				);
			if (rc != CM_OK_PENDING)
			{
				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
				cm_cl_arsm_down (argr, &dinfo);
				return 0;
			}
			dev->wf |= CM_CL_DEV__WF_HELLO_FILTER_REPLACE;

			dev->wf &= ~CM_CL_DEV__HELLO_FILTER_NAME_CHANGED;

			return 1;
		}
	}
	return 0;
}


/*----------------------------------------------------------------------------*/
static void cm_cl_arsm_set_down (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
		/* set states */
	argr->cm.cl.state		= CM_DST_WF_DOWN;
	argr->cm.cl.usr_state	= CM_CL_AR_WF_DOWN;

	cm_cl_ar_sustat_sm_reset (argr);

		/* clear no more needed flags */
	CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3);
	CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_PROV_PASSIVATE_AUX);
	CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);
	CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
	CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_ALARM_ROSAL_IND); /* for rosal_alarm see: cm_cl_ar_cancel_queues() */

		/* cancel outstanding NARE.req, see AP00970593 */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF)
	&&	! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF))
	{
		LSA_BOOL rc;

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF);

		rc = cm_nare_cancel (argr->ar_com.channel, argr->device_nr, argr->ar_nr, argr, LSA_NULL);
		if (! rc)
		{
			/* alloc error: must timeout the nare ident, set or test_ip req */
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF);
		}
	}

		/* cancel prm-statemachine */
	cm_cl_prm_abort (argr);

		/* cancel the optional rtc3-prm-read trigger. note: prior then clrpc-cancel */
	cm_cl_prm_read_stop (argr);

		/* cancel Remote_Application_Ready_Timeout */
	cm_cl_prm_ardy_timeout_stop (argr);

		/* cancel outstanding CLRPC.req, see AP00970593 */
	if (0
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF)
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF)
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF)
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF)
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF)
	||	CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
	{
		cm_cl_rpc_cancel (argr);
	}

		/* disable alarm handling */
	cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_IGNORE);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_arsm_set_indata (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	argr->cm.cl.state = CM_DST_IN_DATA;

	cm_cl_ar_sustat_sm_trigger(argr, CM_CL_SUSTAT_STATE_INDATA);

		/* when in prm_loop forward plug-alarm to cm-user */
	if (cm_is_not_null (argr->cm.cl.prm_alarm))
	{
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_INDATA);
		cm_cl_ar_prm_alarm_event (argr);
	}

		/* else tell cm-user in_data */
	if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
	{
		cm_cl_ar_event (argr, CM_OPC_CL_AR_IN_DATA_IND);
	}

		/* RTClass12: enable alarms */
	cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_FORWARD);

		/* check 'queued' apdu_status */
	cm_ar_com_cons_apdu_status_check_unicast (argr);

		/* tell device */
	cm_cl_devsm (argr->cm.cl.dev, CM_DST_IN_DATA, argr);
}

/*------------------------------------------------------------------------------
// cm_cl_ar - statemachine, handle all next-step of one ar-graph
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_arsm (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel;
	LSA_BOOL  is_down;
	LSA_UINT16  trigger_down;

	CM_ASSERT (cm_is_not_null (argr));
	CM_ASSERT (cm_is_not_null (argr->ar_com.channel));
	channel = argr->ar_com.channel;

	is_down			= cm_ar_com_is_down (argr);
	trigger_down	= is_down  &&  (argr->cm.cl.usr_state != CM_CL_AR_WF_DOWN);

	if (trigger_down)
	{
		LSA_UINT16  old_ustate = argr->cm.cl.usr_state;

		CM_CL_TRACE_10 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "[cm_cl-arsm: dev(%u): goes down.reason(%u): skey(%u) arState(%u) usrState(%u) #con(%u) wf0(0x%x) wf1(0x%x) appl(%u) prm(%u)"
			, argr->device_nr, argr->ar_com.down
			, argr->session_key, argr->cm.cl.state, argr->cm.cl.usr_state
			, argr->cm.cl.connect_counter, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			, argr->cm.cl.applrdy_state, argr->cm.cl.prm.state
			);

		cm_cl_arsm_set_down (argr);
		cm_cl_control_set_nos_assignment (channel);

		if (old_ustate == CM_CL_AR_INDATA)
		{
			/* CM_OPC_CL_AR_OFFLINE_IND: is done within cm_cl_ar_check_abort() because of frame-passivate first */
		}
		else if (old_ustate == CM_CL_AR_WF_IN)
		{
			cm_cl_ar_event (argr, CM_OPC_CL_AR_ABORT_IND); /* CM_OPC_CL_AR_PRE_OFFLINE_IND is event-magic */
		}
		else
		{
			CM_FATAL1(old_ustate);
		}
	}
	else if (is_down)
	{
		CM_CL_TRACE_10 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "[cm_cl-arsm: dev(%u): is down.reason(%u): skey(%u) arState(%u) usrState(%u) #con(%u) wf0(0x%x) wf1(0x%x) appl(%u) prm(%u)"
			, argr->device_nr, argr->ar_com.down
			, argr->session_key, argr->cm.cl.state, argr->cm.cl.usr_state
			, argr->cm.cl.connect_counter, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			, argr->cm.cl.applrdy_state, argr->cm.cl.prm.state
			);
	}
	else
	{
		CM_CL_TRACE_10 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "[cm_cl-arsm: dev(%u): down.reason(%u): skey(%u) arState(%u) usrState(%u) #con(%u) wf0(0x%x) wf1(0x%x) appl(%u) prm(%u)"
			, argr->device_nr, argr->ar_com.down
			, argr->session_key, argr->cm.cl.state, argr->cm.cl.usr_state
			, argr->cm.cl.connect_counter, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			, argr->cm.cl.applrdy_state, argr->cm.cl.prm.state
			);
	}

	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_SCHEDULE:
		CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_IN);

		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u)  STARTUP /nare-resolve"
			, argr->device_nr
		);

		/* usecase: if at & fsu & nos-changed: set hello-filter */
		if (cm_cl_ar_fsu_with_at__hello_filter_replace (argr) != 0)
		{
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_HELLO_FILTER_REPLACE);
		}

		argr->cm.cl.state = CM_DST_NARE;

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_ACTIVATE);

		cm_cl_bindsm_startup (argr);

		break;

	case CM_DST_NARE:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_HELLO_FILTER_REPLACE))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): waiting for HELLO_FILTER_REPLACE", argr->device_nr
			);
			break;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): waiting for REBIND.CNF", argr->device_nr
			);
			break;
		}

		cm_cl_prm_init (argr);

		argr->cm.cl.state = CM_DST_CONNECT;

		cm_cl_control_set_nos_assignment (channel);

			/* set DO flags */
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);

		cm_cl_rpc_connect (channel, argr);
		break;

	case CM_DST_CONNECT:
		argr->cm.cl.state = CM_DST_COM_START;

		/* attach to CM-PD */ /*AP00998330*/
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PD_DETACH);
		if (!cm_cl_ar_attach_to_pd (channel, argr))
		{
			CM_CL_DOWN_INFO_TYPE		dinfo;
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u): can't attach send_clock to CM-PD, go down"
				, argr->device_nr
			);

			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PD_DETACH);
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_LNK_MODE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_IRDATA));
			cm_cl_arsm_down (argr, &dinfo);
			break;
		}

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_AR_ADD);
		cm_arcq_request (argr, CM_ARCQ_OPC_ALARM_ADD, 0);
		break;

	case CM_DST_COM_START:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_AR_ADD))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): waiting for AR_ADD", argr->device_nr
				);
			break;
		}

		if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
		{
			argr->cm.cl.state = CM_DST_COM_ACTIVATE;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
			cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, CM_ARCQ_PROVIDER_SET_STATION_OK);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE);
			cm_arcq_request (argr, CM_ARCQ_OPC_CONS_ACTIVATE, 0);
		}
		else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
		{
			argr->cm.cl.state = CM_DST_PRM;
			cm_cl_prm_request (argr, CM_CL_PRMMODE_CONNECT, (CM_UPPER_RQB_PTR_TYPE)argr);
		}
		else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
		{
			argr->cm.cl.state = CM_DST_COM_ACTIVATE;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
			cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, CM_ARCQ_PROVIDER_SET_STATION_OK);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK);
			cm_arcq_request (argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
		}
		else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
		{
			argr->cm.cl.state = CM_DST_COM_ACTIVATE;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE_AUX);
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PROV_PASSIVATE_AUX);
			cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE_AUX, CM_ARCQ_PROVIDER_SET_STATION_OK);
		}
		break;

	case CM_DST_COM_ACTIVATE:
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for PROV-Activate", argr->device_nr);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE_AUX))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for PROV-Activate-Aux", argr->device_nr);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for CONS-Activate+Set2Unk", argr->device_nr);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for CONS-Activate", argr->device_nr);
			break;
		}

		argr->cm.cl.state = CM_DST_PRM;
		cm_cl_prm_request (argr, CM_CL_PRMMODE_CONNECT, (CM_UPPER_RQB_PTR_TYPE)argr);
		break;

/*----------------------------------------------------------------------------*/
/* from this state we have to differ between connect-phase, plug-phase and prm-update-phase */

	case CM_DST_PRM: /* means post PrmEnd.cnf, for pre PrmEnd.req see cm_cl_prm_end_req() */

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for PRM_DONE", argr->device_nr);
			break;
		}

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_CONNECT:

			cm_cl_schedule_done (argr);

			/* see also cm_cl_prm_end_req() */

			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				if (! cm_ar_com_is_up (argr))
				{
						/* start CMI-RetriggerTimeout */
					cm_cl_prm_read_start (argr);
				}
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
					/* start CMI-RetriggerTimeout */
				CM_ASSERT (! cm_ar_com_is_up (argr));
				cm_cl_prm_read_start (argr);

				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE);
				cm_arcq_request (argr, CM_ARCQ_OPC_CONS_ACTIVATE, 0);
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK);
				cm_arcq_request (argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
			}

			if (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP)
			{
				cm_cl_prm_ardy_timeout_start (argr, CM_CL_ARDY_TIMEOUT__RemoteApplReady);
			}

			argr->cm.cl.state = CM_DST_WF_APPL_READY_IND;
			cm_cl_arsm (argr); /* appl-ready recv in the meantime ? */
			break;

		case CM_CL_PRMMODE_PLUG:

			if (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP)
			{
				cm_cl_prm_ardy_timeout_start (argr, CM_CL_ARDY_TIMEOUT__RemoteApplReady);
			}

			argr->cm.cl.state = CM_DST_WF_APPL_READY_IND;
			cm_cl_arsm (argr); /* appl-ready recv in the meantime ? */
			break;

		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:

			if (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP)
			{
				cm_cl_prm_ardy_timeout_start (argr, CM_CL_ARDY_TIMEOUT__RemoteApplReady);
			}

			argr->cm.cl.state = CM_DST_WF_APPL_READY_IND;
			cm_cl_arsm (argr); /* appl-ready recv in the meantime ? */
			break;

		default:
			CM_ASSERT(0);
			break;
		}
		break;

/*----------------------------------------------------------------------------*/

	case CM_DST_WF_APPL_READY_IND:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): state(%u): waiting for PROV-Activate", argr->device_nr, argr->cm.cl.state);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): state(%u): waiting for CONS-Activate", argr->device_nr, argr->cm.cl.state);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): state(%u): waiting for CONS-Activate+Set2Unknown", argr->device_nr, argr->cm.cl.state);
			break;
		}
		if (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): state(%u): waiting for ApplReady.ind, applrdy_state(%u)", argr->device_nr, argr->cm.cl.state, argr->cm.cl.applrdy_state);
			break;
		}

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_CONNECT:
				/* with ApplReady.ind the link state must match */
			if (! cm_link_status_is_100Mbps_FDX_or_more(channel))
			{
				CM_CL_DOWN_INFO_TYPE  dinfo;

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_LNK_MODE, CM_ERROR_CAUSE_MAU_MISMATCH, CM_PNIO_FAULTY_MAC(0/*ARProperties*/));
				cm_cl_arsm_down (argr, &dinfo);
				break;
			}

				/* legacy: ApplReady.ind tell us to send send a PreData.ind (formerly RTC3Follows.ind) / enable alarm / records enabled since prm-end */
				/* advanced: if AR is not up, ApplReady.ind starts PreDataTimeout */
			if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND)) /* Legacy PreData.ind */
			{
				CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);

					/* no RiR yet => must send PreData.ind to enable alarm handling */
				cm_cl_ar_event (argr, CM_OPC_CL_AR_PRE_DATA_IND);

				cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_FORWARD);
			}

			if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND))
			{
				cm_cl_prm_trigger (argr, CM_CL_WF_PRM_APPL_RDY_IND);
				/* => CM_CL_PRMMODE_IDLE, see cm_cl_prm_idle_ind() */
				/* => or may start a new CM_CL_PRMMODE_xxx */
			}
			else
			{
				CM_FATAL(/*pgm-error*/);
			}
			break;

		case CM_CL_PRMMODE_IDLE:
			argr->cm.cl.state = CM_DST_WF_NEW_DATA;
			cm_cl_arsm (argr); /* cons received in the meantime ? */
			break;

		case CM_CL_PRMMODE_PLUG:
		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:
			if (argr->cm.cl.usr_state == CM_CL_AR_INDATA)
			{
				/* /// rosal: check if rosal has already done. is this possible? yes if we have 2 priorities */
				if (! (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK)))
				{
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK);
					cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK, 0);
				}
				else
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "PrmUpdate: dev(%u) prm_mode(%u) set2unk is already set, skip", argr->device_nr, cm_cl_prm_mode(argr)
						);
				}
			}

			argr->cm.cl.state = CM_DST_WF_NEW_DATA;
			cm_cl_arsm (argr); /* again recv in the meantime ? */
			break;

		default:
			CM_ASSERT(0);
			break;
		}
		break;

/*----------------------------------------------------------------------------*/

	case CM_DST_WF_NEW_DATA:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for CONS-Set2Unknown", argr->device_nr);
			break;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for CONS-Set2Unknown+EnableProviderAutostop", argr->device_nr);
			break;
		}

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_CONNECT: /* should never occur */
			CM_FATAL(/*pgm-error*/);
			break;

		case CM_CL_PRMMODE_IDLE: /* note: sequence CM_CL_PRMMODE_CONNECT -> trigger(ApplRdy) -> CM_CL_PRMMODE_IDLE */

			if (! cm_ar_com_is_up (argr))
			{
				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND)) /* Advanced PreData.ind */
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);

						/* no Again.ind yet => must send PreData.ind to enable alarm handling */
					cm_cl_ar_event (argr, CM_OPC_CL_AR_PRE_DATA_IND);

					cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_FORWARD);
				}

				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for AGAIN.ind", argr->device_nr);
				break;
			}
			else
			{
				cm_cl_prm_ardy_timeout_stop (argr);
			}

			CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_WF_IN);

			/*----------------------------------------------------------------*/
			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS))
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS);
					cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS, 0);
					break;
				}

				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u)  IN_DATA /AdvRT", argr->device_nr
					);
				argr->cm.cl.state = CM_DST_IN_DATA;
				cm_cl_arsm_set_indata (argr);
			}
			/*----------------------------------------------------------------*/
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
				if (! cm_ar_com_is_rir (argr))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev(%u): AdvRTClass3 waiting for ReceivedInRed.ind", argr->device_nr
						);
					break;
				}

				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS))
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS);
					cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS, 0);
					break;
				}

				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u)  IN_DATA /AdvRTClass3", argr->device_nr
					);
				argr->cm.cl.state = CM_DST_IN_DATA;
				cm_cl_arsm_set_indata (argr);
			}
			/*----------------------------------------------------------------*/
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS))
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS);
					cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS, 0);
					break;
				}

				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u)  IN_DATA /LegacyRT", argr->device_nr
					);
				argr->cm.cl.state = CM_DST_IN_DATA;
				cm_cl_arsm_set_indata (argr);
			}
			/*----------------------------------------------------------------*/
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev(%u): LegacyRTClass3 waiting for ReadyForRTClass3.ind", argr->device_nr
						);
					break;
				}

				if (! cm_ar_com_is_rir (argr))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev(%u): LegacyRTClass3 waiting for ReceivedInRed.ind", argr->device_nr
						);
					break;
				}

				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PROV_PASSIVATE_AUX))
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PROV_PASSIVATE_AUX);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_PASSIVATE_AUX);
					cm_arcq_request (argr, CM_ARCQ_OPC_PROV_PASSIVATE_AUX, 0);
				}
				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_PASSIVATE_AUX))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
						, "dev(%u): LegacyRTClass3 waiting for PROV-Passivate-Aux.cnf", argr->device_nr
						);
					break;
				}

				if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS))
				{
					CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_CONS_SET2UNK_ENPRAS);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS);
					cm_arcq_request (argr, CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS, 0);
					break;
				}

				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u)  IN_DATA /LegacyRTClass3", argr->device_nr
					);
				argr->cm.cl.state = CM_DST_IN_DATA;
				cm_cl_arsm_set_indata (argr);
			}
			break;

		case CM_CL_PRMMODE_PLUG:
			if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
			{
				if (cm_is_not_null (argr->cm.cl.prm_alarm))
				{
					cm_cl_ar_prm_alarm_event (argr); /* forward pending prm-alarm */
				}

				cm_cl_prm_trigger (argr, CM_CL_WF_PRM_APPL_RDY_IND);
				/* => cm_cl_prm_idle_ind() trigger arsm() */
			}
			else
			{
				CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_INDATA);

				if (! cm_ar_com_is_up (argr))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
						, "dev(%u): Alarm pending: waiting for AGAIN.ind", argr->device_nr
						);
					break;
				}

				if (cm_is_not_null (argr->cm.cl.prm_alarm))
				{
					cm_cl_ar_prm_alarm_event (argr); /* forward pending prm-alarm */
				}

				cm_cl_prm_trigger (argr, CM_CL_WF_PRM_APPL_RDY_IND);
				/* => cm_cl_prm_idle_ind() set state = CM_DST_IN_DATA */
				/*/// VV: set state here and then trigger queued prm_request() */
			}
			break;

		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
		case CM_CL_PRMMODE_COC:
			if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
			{
				if (! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
				{
					cm_cl_ar_event (argr, CM_OPC_CL_AR_PRM_INDATA_IND);
					cm_cl_prm_trigger (argr, CM_CL_WF_PRM_APPL_RDY_IND);
					/* => cm_cl_prm_idle_ind() trigger arsm() */
				}
			}
			else
			{
				CM_ASSERT (argr->cm.cl.usr_state == CM_CL_AR_INDATA);

				if (! cm_ar_com_is_up (argr))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
						, "dev(%u): PrmUpdate pending: waiting for AGAIN.ind", argr->device_nr
						);
					break;
				}
				cm_cl_ar_event (argr, CM_OPC_CL_AR_PRM_INDATA_IND);
				cm_cl_prm_trigger (argr, CM_CL_WF_PRM_APPL_RDY_IND);
				/* => cm_cl_prm_idle_ind() set state = CM_DST_IN_DATA */
			}
			break;

		default:
			CM_FATAL(/*pgm-error*/);
			break;
		}
		break;

/*----------------------------------------------------------------------------*/

	case CM_DST_IN_DATA:
		/* only "trigger_down" */

		/*/// ROSAL breakpoint test */
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): waiting for CONS-Set2Unknown", argr->device_nr);
			break;
		}

		cm_cl_prm_trigger (argr, 0); /* pending reads */
		break;

/*----------------------------------------------------------------------------*/

	case CM_DST_WF_DOWN:
		if (cm_cl_ar_check_abort (channel, argr))
		{
			cm_cl_bindsm_trigger (argr, 0); /* tell bindsm */

			cm_cl_devsm (argr->cm.cl.dev, CM_DST_PASSIVE, argr); /* tell device */
		}
		break;

/*----------------------------------------------------------------------------*/

	default:
		CM_FATAL (/*unknown state*/);
		break;
	}

	if (is_down)
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "]cm_cl-arsm: dev(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
	}
	else
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "]cm_cl-arsm: dev(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_start_done (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_AR_ADD);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_ALARMS_STOP);
	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_FRAMES_PASSIVATE);

	if (cm_ar_com_is_down (argr))
	{
		/* if we are down, cm_cl_ar_com_down() is immediately fireing */
		CM_ASSERT (argr->cm.cl.state == CM_DST_COM_START || argr->cm.cl.state == CM_DST_WF_DOWN);
		cm_cl_arsm (argr); /* AP01298977 */
	}
	else
	{
		CM_ASSERT (argr->cm.cl.state == CM_DST_COM_START);
		cm_cl_arsm (argr);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_frame_passivate_aux_done (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PROV_PASSIVATE_AUX);

	switch (argr->cm.cl.state)
	{
	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_IN_DATA:
		break;

	case CM_DST_WF_NEW_DATA:
		cm_cl_arsm (argr);
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		CM_FATAL ();
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_up (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
		/* rosal-statemachine */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_ALARM_ROSAL_IND))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "AlarmEvent: dev(%u) alarm(RETURN-OF-SUBMODULE).ind, UP, arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);

		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_ALARM_ROSAL_IND);

		if (cm_is_not_null (argr->cm.cl.rosal_alarm)) /* always true */
		{
			cm_cl_alarm_set_wait_for (argr, argr->cm.cl.rosal_alarm->args.cl.alarm->alarm_priority);
			cm_callback (argr->ar_com.channel, CM_OK, & argr->cm.cl.rosal_alarm);
		}
	}

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_APPL_READY_IND:
		cm_cl_prm_read_stop (argr);
		break;

	case CM_DST_WF_NEW_DATA:
		cm_cl_prm_read_stop (argr);
		cm_cl_arsm (argr);
		break;

	case CM_DST_WF_DOWN:
		/* down for other reason than frame-miss / alarm-error */
		cm_cl_arsm (argr);
		break;

	default:
		/* keep only state, other must check */
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_rtc3_is_running (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_NEW_DATA:
		cm_cl_arsm (argr);
		break;

	case CM_DST_WF_DOWN:
		/* down for other reason than frame-miss / alarm-error */
		cm_cl_arsm (argr);
		break;

	default:
		/* keep only state, other must check */
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_down (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_DOWN_INFO_TYPE  dinfo;

	if (argr->ar_com.down == CM_AR_REASON_MISS)
	{
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_MISS, CM_ERROR_CAUSE_MISS, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MISS), argr->ar_com.cycle_counter);

	}
	else if (argr->ar_com.down == CM_AR_REASON_ASRT)
	{
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_ASRT, CM_ERROR_CAUSE_IOD_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ASRT), argr->ar_com.down_pnio_status);
	}
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("warning: TODO: check, gibt hier noch mehr spezialfälle zu beachten?")
#endif /* CM_MESSAGE */
	else
	{
		CM_CL_DINFO_SET (&dinfo, argr->ar_com.down, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(argr->ar_com.down));
	}

	cm_cl_arsm_down (argr, &dinfo);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_reset_done (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_COM_ACTIVATE:
	case CM_DST_WF_APPL_READY_IND:
		cm_cl_arsm (argr);
		break;

	case CM_DST_WF_NEW_DATA:
		if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3)
		{
#ifdef CM_MESSAGE
//TODO: check: ist das hier notwendig?
//Notwendig für ARs ohne CM_OPC_CL_AR_PRE_DATA_IND ?
#endif
			cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_FORWARD);
		}
		cm_cl_arsm (argr);
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	case CM_DST_PRM:
	case CM_DST_IN_DATA:
		/* CM_ALARM_TYPE_RETURN_OF_SUBMODULE */
		break;

	default:
		CM_FATAL1 (argr->cm.cl.state);
		break;
	}
}


#ifdef CM_MESSAGE
# pragma CM_MESSAGE("warning: TODO: remove #if 1//SRD")
/* sobald das setzen des DataStatus nicht mehr mit einen Request erledigt wird */
#endif /* CM_MESSAGE */

#if 1//SRD
/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_com_apdu_status_control (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  acp_mode
	)
{
	cm_arcq_request (argr, CM_ARCQ_OPC_PROV_DATASTATUS_SET, acp_mode);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID
cm_cl_ar_com_apdu_status_control_done (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_ASSERT (cm_is_not_null (argr->cm.cl.dev->upper_apdu));
	CM_ASSERT (CM_RQB_GET_OPCODE (argr->cm.cl.dev->upper_apdu) == CM_OPC_CL_DEVICE_CONTROL);

	cm_callback (argr->ar_com.channel, CM_OK, &argr->cm.cl.dev->upper_apdu);
}
#else
#endif


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_ar_com_apdu_status_ind (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  apdu_delta
	)
{
	CM_UPPER_RQB_PTR_TYPE  upper;

	if ((argr->cm.cl.dev->run_mode & CM_CL_RMODE_DISABLE_DATA_STATUS_IND) != 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "DataStatus.ind disabled, keep apdu_delta(0x%x)", apdu_delta);
		return LSA_FALSE;
	}

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_DATA_STATUS_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW, "!too fast, waiting 4 .rsp, keep apdu_delta(0x%x)", apdu_delta);
		return LSA_FALSE;
	}

	if (argr->cm.cl.usr_state != CM_CL_AR_INDATA)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "too early, waiting for in_data, state(%u) usr_state(%u) apdu_delta(0x%x)"
			, argr->cm.cl.state, argr->cm.cl.usr_state, apdu_delta
			);
		return LSA_FALSE;
	}

	upper = cm_cl_dev_event_get (argr->cm.cl.dev);
	if (cm_is_null (upper))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!ind resource, keep apdu_delta(0x%x)", apdu_delta);
		return LSA_FALSE;
	}

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_DATA_STATUS_RSP);

	upper->args.cl.event_ind->device_nr = argr->device_nr;
	upper->args.cl.event_ind->ar_nr = argr->ar_nr;
	upper->args.cl.event_ind->session_key = argr->session_key;
	upper->args.cl.event_ind->u.cl.data_status = apdu_delta;

	CM_RQB_SET_OPCODE (upper, CM_OPC_CL_AR_DATA_STATUS_IND);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u): DataStatus.ind:  skey(%u)  apdu_delta(0x%04x)"
		, argr->device_nr, argr->session_key, apdu_delta
		);
	cm_callback (argr->ar_com.channel, CM_OK, & upper);

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_dev_is_ready_to_reset_to_factory (
	CM_CL_DEVICE_CONST_PTR_TYPE  dev
	)
{
	switch (dev->state)
	{
	case CM_CL_DEV_ST_NOI:
		return LSA_TRUE;

	case CM_CL_DEV_ST_REMOVING:
		return LSA_FALSE; /* must trigger RTF-callback if done, AP00989972 */

	case CM_CL_DEV_ST_PASSIVE: /* AP01166567 */
	case CM_CL_DEV_ST_ACTIVE:
		if (cm_is_not_null(dev->argr))
		{
			switch (dev->argr->cm.cl.state)
			{
			case CM_DST_PASSIVE:
				break;

			case CM_DST_WF_SCHEDULE:
				break;

			default:
				return LSA_FALSE;
			}
		}
		return LSA_TRUE;

	default:
		CM_FATAL1(dev->state);
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_pdev_set_down_ind (
	CM_CHANNEL_PTR_TYPE channel, /* state always CM_CL_READY, see caller */
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev;
	LSA_BOOL  all_passive = LSA_TRUE;
	LSA_BOOL  all_removed = LSA_TRUE;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	CM_CL_DOWN_INFO_TYPE  dinfo_mp;

	CM_CL_DINFO_SET (&dinfo, reason, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_CTLDINA(0/*No DCP active / No Link*/));
	CM_CL_DINFO_SET (&dinfo_mp, reason, CM_ERROR_CAUSE_MP_IN_PROGRESS, CM_PNIO_FAULTY_RTA(reason));

	for (dev = cm_cl_dev_first (channel); cm_is_not_null(dev); dev = cm_cl_dev_next (channel, dev))
	{
		switch (dev->state)
		{
		case CM_CL_DEV_ST_PASSIVE:
			all_removed = LSA_FALSE;

			if (reason == CM_AR_REASON_DCP_RESET)
			{
				dev->wf |= CM_CL_DEV__IS_RESET_TO_FACTORY;
				dev->wf |= CM_CL_DEV__DO_FRAME_RTF_REQ;
			}
			if (dev->run_mode & CM_CL_RMODE__I_HELLO_FILTER_DYNAMIC)
			{
				if (reason == CM_AR_REASON_DCP_RESET || reason == CM_AR_REASON_DCP_NAME)
				{
					dev->wf |= CM_CL_DEV__HELLO_FILTER_NAME_CHANGED;
				}
			}
			break;

		case CM_CL_DEV_ST_ACTIVE:
			all_removed = LSA_FALSE;
			all_passive = LSA_FALSE;

			if (reason == CM_AR_REASON_DCP_RESET)
			{
				dev->wf |= CM_CL_DEV__IS_RESET_TO_FACTORY;
				dev->wf |= CM_CL_DEV__DO_FRAME_RTF_REQ;

				dev->state = CM_CL_DEV_ST_PASSIVE;
			}
			if (dev->run_mode & CM_CL_RMODE__I_HELLO_FILTER_DYNAMIC)
			{
				if (reason == CM_AR_REASON_DCP_RESET || reason == CM_AR_REASON_DCP_NAME)
				{
					dev->wf |= CM_CL_DEV__HELLO_FILTER_NAME_CHANGED;
				}
			}

			if (cm_is_not_null(dev->argr))
			{
				if (cm_is_not_null (dev->argr->cm.cl.mtd)) /* is_masterproject */
				{
					cm_cl_arsm_down (dev->argr, &dinfo_mp);
				}
				else
				{
					cm_cl_arsm_down (dev->argr, &dinfo);
				}
			}
			break;

		case CM_CL_DEV_ST_NOI:
		case CM_CL_DEV_ST_REMOVING:
		default:
			break;
		}
	}

	if (all_removed)
	{
		return LSA_TRUE; /* ready for pdev-rtf */
	}

	channel->usr.cl.upper_wf |= CM_CUWF_PDEV_SET_DOWN_CNF;

	if (all_passive)
	{
		/* must trigger */
		cm_cl_pdev_set_down_check (channel);
		return LSA_FALSE;
	}

	/* trigger is done by e.g. cm_cl_devsm() */
	return LSA_FALSE; /* not all_down */
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_pdev_set_down_check (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_CL_DEVICE_PTR_TYPE  dev;

	CM_ASSERT (channel->usr.cl.upper_wf & CM_CUWF_PDEV_SET_DOWN_CNF);

	for (dev = cm_cl_dev_first (channel); cm_is_not_null(dev); dev = cm_cl_dev_next (channel, dev))
	{
		if (! cm_cl_dev_is_ready_to_reset_to_factory (dev))
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "cl-RtfNosIpChanged: dev(%u) not ready for rtf, must wait", dev->device_nr
				);
			return; /* not all_down */
		}
	}

	for (dev = cm_cl_dev_first (channel); cm_is_not_null(dev); dev = cm_cl_dev_next (channel, dev))
	{
		if (dev->wf & CM_CL_DEV__WF_FRAME_RTF_CNF)
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "cl-RtfNosIpChanged: dev(%u) must wait for set-frame-rtf", dev->device_nr
				);
			return;
		}

		if (dev->wf & CM_CL_DEV__DO_FRAME_RTF_REQ)
		{
			dev->wf &= ~CM_CL_DEV__DO_FRAME_RTF_REQ;
			dev->wf |=  CM_CL_DEV__WF_FRAME_RTF_CNF;

			cm_arcq_request (dev->argr, CM_ARCQ_OPC_FRAMES_RTF, 0);
			return;
		}
	}


	{
	CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_PD);

	channel->usr.cl.upper_wf &= ~CM_CUWF_PDEV_SET_DOWN_CNF;

	cm_pd_rtf_nos_ip_changed_done (pd_channel, CM_PD_RTF_WF__CL_DOWN); /* cm_pdcl_pdev_is_ready_for_schedule() possibly becomes ready */
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  arcq_para1
	)
{
	CM_ASSERT (argr->ar_com.path_type == CM_PATH_TYPE_CLIENT);

	switch (arcq_opcode)
	{
	case CM_ARCQ_OPC_FRAMES_ADD:
		if (cm_ar_com_is_down (argr))
		{
			cm_cl_device_add_rm_done (argr->ar_com.channel
				, argr->cm.cl.dev, CM_CL_DEV__WF_FRAME_ADD_CNF, CM_ERR_LOWER_LAYER
				);
		}
		else
		{
			cm_cl_device_add_rm_done (argr->ar_com.channel
				, argr->cm.cl.dev, CM_CL_DEV__WF_FRAME_ADD_CNF, CM_OK
				);
		}
		break;

	case CM_ARCQ_OPC_FRAMES_RTF:
		argr->cm.cl.dev->wf &= ~ CM_CL_DEV__WF_FRAME_RTF_CNF;
		cm_cl_pdev_set_down_check (argr->ar_com.channel);
		break;

	case CM_ARCQ_OPC_ALARM_ADD:
		cm_ar_com_ready (argr); /* enables ar_com-down-indication */
		cm_cl_ar_com_start_done (argr);
		break;

	case CM_ARCQ_OPC_PROV_ACTIVATE:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
		cm_cl_arsm (argr);
		break;

	case CM_ARCQ_OPC_PROV_ACTIVATE_AUX:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE_AUX);
		cm_cl_arsm (argr);
		break;

	case CM_ARCQ_OPC_CONS_ACTIVATE:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE);
		cm_cl_arsm (argr);
		break;

	case CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_CONS_ACTIVATE_SET2UNK);
		cm_cl_ar_com_reset_done (argr);
		break;

	case CM_ARCQ_OPC_CONS_SET2UNK:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK);
		cm_cl_ar_com_reset_done (argr);
		break;

	case CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_CONS_SET2UNK_ENPRAS);
		cm_cl_ar_com_reset_done (argr);
		break;

	case CM_ARCQ_OPC_PROV_PASSIVATE_AUX:
		cm_cl_ar_com_frame_passivate_aux_done(argr);
		break;

	case CM_ARCQ_OPC_FRAMES_PASSIVATE:
		{
			CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get (argr->ar_com.channel, CM_PATH_TYPE_PD);
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_FRAMES_PASSIVATE_DONE);
			cm_pd_prm_write_PDInstance_TailorData_trigger (pd_channel);
			cm_cl_arsm (argr);
		}
		break;

	case CM_ARCQ_OPC_ALARM_REMOVE:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_ALARMS_STOP_DONE);
		cm_cl_arsm (argr);
		break;

	case CM_ARCQ_OPC_FRAMES_REMOVE:
		cm_cl_device_add_rm_done (argr->ar_com.channel
			, argr->cm.cl.dev, CM_CL_DEV__WF_FRAME_REMOVE_CNF, CM_OK
			);
		break;

	case CM_ARCQ_OPC_PROV_DATASTATUS_SET:

		/* note: setting apdu-status is ok or fatal, see cm_acp_frame_control_done() */
		{
		LSA_UINT16 acp_mode = arcq_para1;

		if (acp_mode & CM_ARCQ_PROVIDER_SET_STATION_OK) {
			argr->cm.cl.station_problem = LSA_FALSE;
			CM_CL_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "station_problem(%u)", argr->cm.cl.station_problem
				);
		}
		else if (acp_mode & CM_ARCQ_PROVIDER_SET_STATION_FAILURE) {
			argr->cm.cl.station_problem = LSA_TRUE;
			CM_CL_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "station_problem(%u)", argr->cm.cl.station_problem
				);
		}
#if 1//SRD
		cm_cl_ar_com_apdu_status_control_done(argr);
#else
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("error: CM_ARCQ_OPC_PROV_DATASTATUS_SET kann man (bald) löschen!")
#endif /* CM_MESSAGE */
#endif
		}
		break;

	default:
		CM_FATAL1 (arcq_opcode);
		return;
	}
}


/*----------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
