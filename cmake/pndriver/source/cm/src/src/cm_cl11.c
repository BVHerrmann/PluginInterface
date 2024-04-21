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
/*  F i l e               &F: cm_cl11.c                                 :F&  */
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
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   611
#define CM_MODULE_ID        611

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
//  local definitions
//----------------------------------------------------------------------------*/

#if 0 /* AP01556507: disable ARVendorBlock */
#define cm_get_REQ_ARV_APStructureIdentifier(u16p_)     cm_get_u16  (u16p_, dp, CM_REQ_ARVENDOR, APStructureIdentifier)
#define cm_get_REQ_ARV_API(u32p_)                       cm_get_u32  (u32p_, dp, CM_REQ_ARVENDOR, API)
#endif


static LSA_VOID
cm_cl_dev_free_name_alias (
	CM_CL_DEVICE_PTR_TYPE dev
	);


/*------------------------------------------------------------------------------
//  dev helper
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_init (
	CM_CL_DEVICE_PTR_TYPE dev
	)
{
	dev->state     = CM_CL_DEV_ST_NOI;
	dev->device_nr = 0;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_dev_free (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CL_DEVICE_PTR_TYPE dev
	)
{
	CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) free", dev->device_nr);

	/* should freed earlier */
	CM_ASSERT (cm_is_null (dev->upper_add_rm));
	CM_ASSERT (cm_is_null (dev->upper_ctrl));
	CM_ASSERT (cm_is_null (dev->upper_apdu));

	CM_ASSERT (cm_is_null (dev->rpc_client_id));

	/* cancel indication resources */
	{
		CM_UPPER_RQB_PTR_TYPE rb;

		rb = cm_cl_dev_event_get (dev);
		while (cm_is_not_null (rb))
		{
			cm_callback (channel, CM_OK_CANCELLED, &rb);
			rb = cm_cl_dev_event_get (dev);
		}

		rb = cm_cl_dev_alarm_get (dev, CM_ALARM_PRIORITY_LOW);
		while (cm_is_not_null (rb))
		{
			cm_callback (channel, CM_OK_CANCELLED, &rb);
			rb = cm_cl_dev_alarm_get (dev, CM_ALARM_PRIORITY_LOW);
		}

		rb = cm_cl_dev_alarm_get (dev, CM_ALARM_PRIORITY_HIGH);
		while (cm_is_not_null (rb))
		{
			cm_callback (channel, CM_OK_CANCELLED, &rb);
			rb = cm_cl_dev_alarm_get (dev, CM_ALARM_PRIORITY_HIGH);
		}
	}

	/* remove ar_graph */
	if (cm_is_not_null (dev->argr))
	{
		cm_ar_graph_free (dev->argr);
	}

	/* remove device-station_name and alias-list*/
	cm_cl_dev_free_name_alias (dev);

	/* unregister fsu */
	if ((dev->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK) == CM_CL_DEVICE_MODE_IS_FSU_YES)
	{
		(LSA_VOID)cm_cl_fsu_count (channel, -1);
	}

	/* no instance */
	dev->state     = CM_CL_DEV_ST_NOI;
	dev->device_nr = 0;

	/* AP00989972: tell the channel state machine   VV: new callback function cm_cl_channel_trigger () */
	if (channel->usr.cl.upper_wf & CM_CUWF_PDEV_SET_DOWN_CNF)
	{
		cm_cl_pdev_set_down_check (channel);
	}
}


/*----------------------------------------------------------------------------*/
static CM_CL_DEVICE_PTR_TYPE
cm_cl_dev_add_device_nr (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 device_nr
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = LSA_NULL;
	LSA_UINT16 idev;

	if (! (1 <= device_nr && device_nr <= CM_CL_MAX_DEVICE_NR))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) not in valid range (1..%u)", device_nr, CM_CL_MAX_DEVICE_NR
			);
		return LSA_NULL;
	}
	for (idev = 0; idev < channel->usr.cl.dev_cnt; ++idev)
	{
		CM_CL_DEVICE_PTR_TYPE loop_dev = &channel->usr.cl.dev[idev];

		if (loop_dev->state == CM_CL_DEV_ST_NOI && loop_dev->device_nr == 0)
		{
			if (cm_is_null (dev))
			{
				dev = loop_dev;
			}
		}
		else
		{
			if (loop_dev->device_nr == device_nr)
			{
				CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u) already added", device_nr
					);
				return LSA_NULL;
			}
		}
	}
	if (cm_is_null (dev))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "not free dev-resource for dev(%u), max dev_cnt(%u)", device_nr, channel->usr.cl.dev_cnt
			);
		return LSA_NULL;
	}
	dev->channel = channel;
	return dev;
}


/*----------------------------------------------------------------------------*/
CM_CL_DEVICE_PTR_TYPE
cm_cl_dev_from_device_nr (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr
	)
{
	LSA_UINT16 idev;

	if (! (1 <= device_nr && device_nr <= CM_CL_MAX_DEVICE_NR))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) not in valid range (1..%u)", device_nr, CM_CL_MAX_DEVICE_NR
			);
		return LSA_NULL;
	}
	for (idev = 0; idev < channel->usr.cl.dev_cnt; ++idev)
	{
		CM_CL_DEVICE_PTR_TYPE dev = &channel->usr.cl.dev[idev];

		if (dev->device_nr == device_nr)
		{
			CM_ASSERT (
				dev->state == CM_CL_DEV_ST_PASSIVE
				|| dev->state == CM_CL_DEV_ST_ACTIVE
				|| dev->state == CM_CL_DEV_ST_REMOVING
				);
			return dev;
		}
	}

	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_CL_DEVICE_PTR_TYPE
cm_cl_dev_first (
	CM_CHANNEL_CONST_PTR_TYPE channel
	)
{
	LSA_UINT16 idev;

	for (idev = 0; idev < channel->usr.cl.dev_cnt; ++idev)
	{
		CM_CL_DEVICE_PTR_TYPE dev = &channel->usr.cl.dev[idev];

		if (dev->state != CM_CL_DEV_ST_NOI && dev->device_nr != 0)
		{
			return dev;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_CL_DEVICE_PTR_TYPE
cm_cl_dev_next (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_CL_DEVICE_CONST_PTR_TYPE dev
	)
{
	LSA_UINT16 idev;

	if (cm_is_null (dev))
	{
		return LSA_NULL;
	}

	for (idev = 0; idev < channel->usr.cl.dev_cnt; ++idev)
	{
		CM_CL_DEVICE_PTR_TYPE next_dev = &channel->usr.cl.dev[idev];

		if (cm_is_not_null (dev))
		{
			if (dev == next_dev)
			{
				dev = LSA_NULL;
			}
		}
		else
		{
			if (next_dev->state != CM_CL_DEV_ST_NOI && next_dev->device_nr != 0)
			{
				return next_dev;
			}
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_dev_alloc_name_alias (
	CM_CL_DEVICE_PTR_TYPE dev,
	CM_UPPER_CL_DEVICE_ADD_CONST_PTR_TYPE add,
	LSA_BOOL is_tailoring_needed
	)
{
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&dev->dev_addr.device_name), add->dev_addr.device_name_length);
	if (cm_is_null (dev->dev_addr.device_name))
	{
		CM_CL_TRACE_01 (dev->channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no dev_addr.device_name-memory, device_name_length(%u)"
			, add->dev_addr.device_name_length
			);
		return LSA_FALSE;
	}

	cm_station_name_copy (dev->dev_addr.device_name, add->dev_addr.device_name, add->dev_addr.device_name_length);
	dev->dev_addr.device_name_length = add->dev_addr.device_name_length;

	if (is_tailoring_needed)
	{
		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&dev->dev_addr.tailored_device_name), OHA_MAX_STATION_NAME_LEN);
		if (cm_is_null (dev->dev_addr.tailored_device_name))
		{
			LSA_UINT16 rc;
			CM_CL_TRACE_01 (dev->channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "no dev_addr.tailored_device_name-memory, device_name_length(%u)", OHA_MAX_STATION_NAME_LEN
				);
			CM_FREE_LOCAL_MEM (&rc, dev->dev_addr.device_name);
			dev->dev_addr.device_name = LSA_NULL;
			return LSA_FALSE;
		}
	}
	else
	{
		dev->dev_addr.tailored_device_name = LSA_NULL;
	}
	dev->dev_addr.tailored_device_name_length = 0;

	{
		CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE add_alias;
		CM_CL_NARE_ALIAS_PTR_TYPE dev_alias;

		CM_ASSERT (CmListIsEmpty (&dev->dev_addr.device_alias_list));

		for (CmListForeach (add_alias, &add->dev_addr.device_alias_list, CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE))
		{
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&dev_alias), sizeof (*dev_alias));
			if (cm_is_null (dev_alias))
			{
				cm_cl_dev_free_name_alias (dev);
				return LSA_FALSE;
			}

			if (is_tailoring_needed)
			{
				CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&dev_alias->name), OHA_MAX_STATION_NAME_LEN);
			}
			else
			{
				CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&dev_alias->name), add_alias->name_length);
			}
			if (cm_is_null (dev_alias->name))
			{
				LSA_UINT16 rc;
				CM_FREE_LOCAL_MEM (&rc, dev_alias);
				cm_cl_dev_free_name_alias (dev);
				return LSA_FALSE;
			}

			dev_alias->name_length = add_alias->name_length;

			cm_station_name_copy (dev_alias->name, add_alias->name, dev_alias->name_length);

			CmListInsertTail (&dev->dev_addr.device_alias_list, &dev_alias->link);
		}
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_dev_free_name_alias (
	CM_CL_DEVICE_PTR_TYPE dev
	)
{
	CM_CL_NARE_ALIAS_PTR_TYPE dev_alias;
	LSA_UINT16 rc;

	while (! CmListIsEmpty (&dev->dev_addr.device_alias_list))
	{
		CmListRemoveHead (&dev->dev_addr.device_alias_list, dev_alias, CM_CL_NARE_ALIAS_PTR_TYPE)
		CM_ASSERT (cm_is_not_null (dev_alias));

		CM_FREE_LOCAL_MEM (&rc, dev_alias->name);
		CM_ASSERT (rc == LSA_OK);

		CM_FREE_LOCAL_MEM (&rc, dev_alias);
		CM_ASSERT (rc == LSA_OK);
	}

	if (cm_is_not_null (dev->dev_addr.device_name))
	{
		CM_FREE_LOCAL_MEM (&rc, dev->dev_addr.device_name);
		CM_ASSERT (rc == LSA_RET_OK);
		dev->dev_addr.device_name = LSA_NULL;
	}

	if (cm_is_not_null (dev->dev_addr.tailored_device_name))
	{
		CM_FREE_LOCAL_MEM (&rc, dev->dev_addr.tailored_device_name);
		CM_ASSERT (rc == LSA_OK);
		dev->dev_addr.tailored_device_name = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_dev_build_provider_sequence (
	/* AP00542516 (stable sequence of provider frames) */
	CM_CL_DEVICE_CONST_PTR_TYPE dev
	)
{

#if CM_CFG_MAX_CLIENTS > 1
/* note that there is still only one client per NIC, no 'bits' are needed in the provider sequence */
#endif

#if CM_AR_IOCR_SEQUENCE_CL_COUNT < CM_CL_MAX_DEVICE_NR
# error "CM-CL device_nr must fit in range"
#endif

	/* edd.doc
	//	If a new provider has a CyclePhaseSequence which already exists in the sending list,
	//	the new provider is inserted always behind this existing CyclePhaseSequence.
	*/

#if CM_AR_IOCR_SEQUENCE_CL_COUNT != 0x0000ffff
# error "CM-CL wrong SEQUENCE_CL_COUNT"
#endif
	if (! (1 <= dev->device_nr /* && dev->device_nr <= CM_AR_IOCR_SEQUENCE_CL_COUNT*/))
	{
		return CM_PNIO_FAULTY_IOCRBlockReq (13 /*Sequence*/);
	}

	if (cm_is_not_null(dev->argr))
	{
		CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

		for (CmListForeach (iocr, &dev->argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))  /* iocr-list is ordered by iocr_ref */
		{
			iocr->cm.adpt.sequence = 0;

			/* calc for provider */
			if (iocr->cm.is_multicast || iocr->cm.is_consumer)
			{
				continue;
			}

			/* calc for RTClass1+2+UDP */
			if ((iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3)
			{
				continue;
			}

#if CM_AR_IOCR_SEQUENCE_CL_BASE == 0x10000000
			iocr->cm.adpt.sequence = CM_AR_IOCR_SEQUENCE_CL_BASE + dev->argr->device_nr - 1;
#else
# error "CM-CL wrong SEQUENCE_CL_BASE"
#endif
		}
	}

	return CM_PNIO_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_alarm_init (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	LSA_UINT16 i;

	for (i = 0; i < 2; ++i)
	{
/*		argr->cm.cl.alarm[i].sequence_nr		= reinit */
/*		argr->cm.cl.alarm[i].recv_sequence_nr	= reinit */
		CmListInitialize (&argr->cm.cl.alarm[i].alarm_list);

		argr->cm.cl.alarm[i].ack         = LSA_NULL;
		argr->cm.cl.alarm[i].ack_rqb     = LSA_NULL;
		argr->cm.cl.alarm[i].acp_ind     = LSA_NULL;
		argr->cm.cl.alarm[i].early_alarm = LSA_NULL;
	}

	argr->cm.cl.alarm[0].ack_rqb = cm_acp_alloc_rqb (argr->ar_com.channel);
	if (cm_is_null (argr->cm.cl.alarm[0].ack_rqb))
	{
		return LSA_FALSE;
	}

	argr->cm.cl.alarm[1].ack_rqb = cm_acp_alloc_rqb (argr->ar_com.channel);
	if (cm_is_null (argr->cm.cl.alarm[1].ack_rqb))
	{
		cm_acp_free_rqb (argr->ar_com.channel, argr->cm.cl.alarm[0].ack_rqb);
		argr->cm.cl.alarm[0].ack_rqb = LSA_NULL;
		return LSA_FALSE;
	}

/*	argr->cm.cl.alarm_enabled_state	= reinit */
/*	argr->cm.cl.station_problem		= reinit */

	return LSA_TRUE;
}
/*------------------------------------------------------------------------------
//  ar helper
//----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
// init client-struct of ar_graph, callback from cm_ar_graph_alloc()
//----------------------------------------------------------------------------*/
/* callback */ LSA_BOOL
cm_cl_ar_init (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	LSA_BOOL ok;

	CM_ASSERT (cm_is_not_null (argr));

	argr->session_key = 0;

	argr->cm.cl.dev = LSA_NULL;     /* caller does init */

	ok = cm_cl_alarm_init (argr);

	argr->cm.cl.usr_state = CM_CL_AR_PASSIVE;
	argr->cm.cl.state     = CM_DST_PASSIVE;

	cm_cl_prm_init (argr);

	argr->cm.cl.sumode = CM_SUMODE_UNDEFINED;

/*	argr->cm.cl.applrdy_state		= reinit */
	argr->cm.cl.in_down = 0;

	argr->cm.cl.wait_for[0] = 0;          /* e.g. see cm_cl_ar_check_abort(), cm_cl_bindsm_trigger () */
/*	argr->cm.cl.wait_for[1]			= reinit;  e.g. see cm_cl_ar_check_abort() */

	argr->cm.cl.connect_counter = 0;
/*	argr->cm.cl.record_sequence_nr			= reinit */

/*	argr->cm.cl.schedule.link				= link-field; see; cm_cl_schedule_add() */
/*	argr->cm.cl.schedule.wait_ticks			= calc; cm_cl_schedule_add() */
	argr->cm.cl.schedule.reschedule_ticks = 1;
	argr->cm.cl.schedule.in_schedule      = 0 /*done*/;
	argr->cm.cl.mtd                       = LSA_NULL;
	argr->cm.cl.sustat                    = LSA_NULL;

	cm_cl_ar_event_init (argr);

	CmListInitialize (&argr->cm.cl.ar_connect_prm_record_list);
	CmListInitialize (&argr->cm.cl.ar_prm_record_list);
/*	CmListInitialize (& argr->cm.cl.prm_list);  reinit */

	argr->cm.cl.prm_alarm   = LSA_NULL;
	argr->cm.cl.rosal_alarm = LSA_NULL;
	argr->cm.cl.rosal_park  = LSA_NULL;

	CmListInitialize (&argr->cm.cl.record_list);

	CmListInitialize (&argr->cm.cl.unique_slot_nr_list);

	cm_cl_ar_reinit (argr);

	return ok;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_reinit (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	LSA_UINT16 i;

	CM_ASSERT (cm_is_not_null (argr));

/*	CM_ASSERT (cm_is_not_null (argr->cm.cl.dev)); */

/*	argr->session_key = 0;	keep */

	for (i = 0; i < 2; ++i)
	{
		argr->cm.cl.alarm[i].send_sequence_nr = 0;
		argr->cm.cl.alarm[i].recv_sequence_nr = CM_ALARM_SEQUENCE_UNDEF;

		CM_ASSERT (CmListIsEmpty (&argr->cm.cl.alarm[i].alarm_list));
		CM_ASSERT (cm_is_null (argr->cm.cl.alarm[i].ack));
		CM_ASSERT (cm_is_null (argr->cm.cl.alarm[i].acp_ind));
		CM_ASSERT (cm_is_null (argr->cm.cl.alarm[i].early_alarm));
	}

	argr->cm.cl.alarm_enabled_state = CM_CL_ALARM_ENABLED_IGNORE;
	argr->cm.cl.station_problem     = LSA_FALSE;

/*	argr->cm.cl.usr_state			= CM_CL_AR_PASSIVE; keep */
/*	argr->cm.cl.state				= CM_DST_PASSIVE; keep */

	cm_cl_prm_init (argr);

	argr->cm.cl.applrdy_prev_state = CM_CL_APPL_READY_STATE_UP;
	argr->cm.cl.applrdy_state      = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

/* see cm_cl_ar_check_abort / cm_cl_device_enter_offline /cm_cl_bindsm_trigger */
	CM_ASSERT (((CM_CL_WF_NARE_CNF | CM_CL_WF_REBIND_CNF | CM_CL_WF_RPC_RECORD_CNF) & 0x1) == 0);
	argr->cm.cl.wait_for[0] &= CM_CL_WF_NARE_CNF | CM_CL_WF_REBIND_CNF | CM_CL_WF_RPC_RECORD_CNF;
	argr->cm.cl.wait_for[1]  = 0;

/*	argr->cm.cl.connect_counter				= keep */
	argr->cm.cl.record_sequence_nr = 0;

/*	argr->cm.cl.schedule.link		= link-field; see; cm_cl_schedule_add() */
/*	argr->cm.cl.schedule.wait_ticks	= calc; cm_cl_schedule_add() */
/*	CM_ASSERT (argr->cm.cl.schedule.reschedule_ticks > 0);  ASU == 0 */
	CM_ASSERT (argr->cm.cl.schedule.in_schedule == 0 /*done*/);

/*	CmListInitialize (& argr->cm.cl.ar_prm_record_list); do-not-reinit */

	CM_ASSERT (cm_is_null (argr->cm.cl.prm_alarm));
	CM_ASSERT (cm_is_null (argr->cm.cl.rosal_alarm));
/*  CM_ASSERT (CmListIsEmpty (& argr->cm.cl.record_list)); might be non-empty */

/*	argr->cm.cl.unique_slot_nr_list		= do-not-reinit */
}

/*----------------------------------------------------------------------------*/
/* callback */ LSA_VOID
cm_cl_ar_undo_init (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_ASSERT (argr->cm.cl.state == CM_DST_PASSIVE);

	/* should freed in cm_cl_device_enter_offline() */
	CM_ASSERT (cm_is_null (argr->cm.cl.prm_alarm));
	CM_ASSERT (cm_is_null (argr->cm.cl.rosal_alarm));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[0].early_alarm));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[1].early_alarm));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[0].ack));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[1].ack));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[0].acp_ind));
	CM_ASSERT (cm_is_null (argr->cm.cl.alarm[1].acp_ind));

	cm_acp_free_rqb (argr->ar_com.channel, argr->cm.cl.alarm[0].ack_rqb);
	cm_acp_free_rqb (argr->ar_com.channel, argr->cm.cl.alarm[1].ack_rqb);

	cm_ar_graph_free_list_local_mem (&argr->cm.cl.unique_slot_nr_list);

	cm_ar_graph_free_list_local_mem (&argr->cm.cl.ar_prm_record_list);

	cm_ar_graph_free_list_local_mem (&argr->cm.cl.ar_connect_prm_record_list);

	cm_pd_mtd_free (&argr->cm.cl.mtd);
	cm_cl_ar_sustat_free (&argr->cm.cl.sustat);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_sustat_init (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	LSA_UINT16 i;
	CM_CL_SUSTAT_DATA_PTR_TYPE sustat = argr->cm.cl.sustat;
	/* Clear the summary data */

	for (i = 0; i < CM_CL_SUSTAT_NUMBER_OF_DATASETS; ++i)
	{
		CM_CL_SUSTAT_RECORD_PTR_TYPE record = &sustat->record[i];
		CM_CL_SUSTAT_TIME_PTR_TYPE time     = &record->time;

		record->number_of_su     = 0;
		record->number_of_aborts = 0;

		record->min_time = 0;
		record->max_time = 0;

		time->nare    = 0;
		time->connect = 0;
		time->prm     = 0;
		time->ardy    = 0;
		time->indata  = 0;
	}

	cm_cl_ar_sustat_sm_reset (argr);
}


LSA_VOID
cm_cl_ar_sustat_sm_reset (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	if (argr->cm.cl.sustat)
	{
		argr->cm.cl.sustat->state     = CM_CL_SUSTAT_STATE_INITIAL;
		argr->cm.cl.sustat->timestamp = 0;
	}
}


LSA_VOID
cm_cl_ar_sustat_sm_trigger (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_CL_SUSTAT_STATE_TYPE trigger
	)
{
	if (argr->cm.cl.sustat)
	{
		CM_CL_SUSTAT_DATA_PTR_TYPE sustat = argr->cm.cl.sustat;
		CM_CL_SUSTAT_TIME_PTR_TYPE time   = &sustat->time;
		LSA_UINT64 ts                     = 0;
		LSA_UINT64 delta_t                = 0;

		CM_GET_TIMESTAMP (&ts);

		if (ts >= sustat->timestamp)
		{
			delta_t = ts - sustat->timestamp;
		}
		else
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "sustat_sm_trigger: dev(%u) resetting from state(%i) trigger(%i)"
				, argr->device_nr, sustat->state, trigger
				);
			cm_cl_ar_sustat_sm_reset (argr);
			return;
		}

		sustat->timestamp = ts;

		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "sustat_sm_trigger: dev(%u) state(%i) trigger(%i) timestamp(%lu) delta_t(%lu)"
			, argr->device_nr, sustat->state, trigger, sustat->timestamp, delta_t
			);

		switch (sustat->state)
		{
		case CM_CL_SUSTAT_STATE_INITIAL:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_ACTIVATE:
				sustat->state = CM_CL_SUSTAT_STATE_ACTIVATE;
				break;

			case CM_CL_SUSTAT_STATE_NARE_FSU:
				sustat->state = CM_CL_SUSTAT_STATE_ACTIVATE;
				cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_NARE_FSU);
				break;

			default:
				/* ignore */
				break;
			}
			/* ignore delta_t */
			break;

		case CM_CL_SUSTAT_STATE_ACTIVATE:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_NARE_FSU:
				sustat->su_type = CM_CL_SUSTAT_SUTYPE_FSU;
				time->nare = 0;
				sustat->state = CM_CL_SUSTAT_STATE_NARE;
				break;

			case CM_CL_SUSTAT_STATE_NARE_ASU:
				sustat->su_type = CM_CL_SUSTAT_SUTYPE_ASU;
				time->nare = delta_t;
				sustat->state = CM_CL_SUSTAT_STATE_NARE;
				break;

			case CM_CL_SUSTAT_STATE_NARE:
				sustat->su_type = CM_CL_SUSTAT_SUTYPE_OTHER;
				time->nare = delta_t;
				sustat->state = CM_CL_SUSTAT_STATE_NARE;
				break;

			default:
				/* ignore */
				break;
			}

			break;

		case CM_CL_SUSTAT_STATE_NARE:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_CONNECT:
				sustat->state = CM_CL_SUSTAT_STATE_CONNECT;
				time->connect = delta_t;
				break;

			default:
				/* ignore */
				break;
			}
			break;

		case CM_CL_SUSTAT_STATE_CONNECT:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_PRM:
				sustat->state = CM_CL_SUSTAT_STATE_PRM;
				time->prm     = delta_t;
				break;

			default:
				/* ignore */
				break;
			}
			break;

		case CM_CL_SUSTAT_STATE_PRM:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_ARDY:
				sustat->state = CM_CL_SUSTAT_STATE_ARDY;
				time->ardy    = delta_t;
				break;

			default:
				/* ignore */
				break;
			}
			break;

		case CM_CL_SUSTAT_STATE_ARDY:
			switch (trigger)
			{
			case CM_CL_SUSTAT_STATE_INDATA:
				{
					CM_CL_SUSTAT_RECORD_PTR_TYPE record = &sustat->record[sustat->su_type];
					LSA_UINT64 total;

					sustat->state = CM_CL_SUSTAT_STATE_INDATA;
					time->indata  = delta_t;

					total = time->nare + time->connect + time->prm + time->ardy + time->indata;
					if (record->max_time < total && total < 0xffffffff)
					{
						record->max_time = (LSA_UINT32) total;
					}

					if ((record->min_time > total || record->min_time == 0) && total < 0xffffffff)
					{
						record->min_time = (LSA_UINT32) total;
					}

					record->time.nare    += time->nare;
					record->time.connect += time->connect;
					record->time.prm     += time->prm;
					record->time.ardy    += time->ardy;
					record->time.indata  += time->indata;
					record->number_of_su += 1;
				}
				break;

			default:
				/* ignore */
				break;
			}
			break;

		case CM_CL_SUSTAT_STATE_INDATA:
			switch (trigger)
			{
			case CM_CL_SUSTAT_TRIGGER_LATE_ERR:
				sustat->record[sustat->su_type].number_of_aborts += 1;
				cm_cl_ar_sustat_sm_reset (argr);
				break;

			default:
				/* ignore */
				break;
			}
			break;

		case CM_CL_SUSTAT_TRIGGER_LATE_ERR:
		default:
			CM_FATAL1 (sustat->state);
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_sustat_free (
	CM_CL_SUSTAT_DATA_PTR_TYPE *sustat_ptr_ptr
	)
{
	LSA_UINT16 rc;

	if (cm_is_not_null (*sustat_ptr_ptr))
	{
		CM_FREE_LOCAL_MEM (&rc, *sustat_ptr_ptr);
		CM_ASSERT (rc == LSA_RET_OK);

		*sustat_ptr_ptr = LSA_NULL;
	}
}


/*------------------------------------------------------------------------------
// ar-abort helper
//----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_ar_event_init (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_MEMSET (&argr->cm.cl.ev.ip_suite, 0, sizeof(argr->cm.cl.ev.ip_suite));

	/* set good */
	argr->cm.cl.ev.current.reason_code = CM_AR_REASON_NONE;
	argr->cm.cl.ev.current.error_cause = CM_ERROR_CAUSE_NONE;
	argr->cm.cl.ev.current.pnio_status = CM_PNIO_ERR_NONE;

	/* set to not-changed */
	argr->cm.cl.ev.previous = argr->cm.cl.ev.current;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event_set_ip (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_MAC_CONST_PTR_TYPE device_mac_addr,
	LSA_UINT32 device_ip_addr,
	LSA_UINT32 device_subnet_mask,
	LSA_UINT32 device_gateway_ip
	)
{
	/* update event-data */
	argr->cm.cl.ev.ip_suite.device_mac         = *device_mac_addr;
	argr->cm.cl.ev.ip_suite.device_ip          = device_ip_addr;
	argr->cm.cl.ev.ip_suite.device_subnet_mask = device_subnet_mask;
	argr->cm.cl.ev.ip_suite.device_gateway_ip  = device_gateway_ip;

	/* update argr-data */
	argr->ar.eth_mac = *device_mac_addr;
	argr->ar.host_ip = device_ip_addr;

	CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): set dev_ip_addr(0x%x) dev_mac_addr(%02x-%02x-%02x-%02x-%02x-%02x)"
		, argr->device_nr
		, device_ip_addr
		, device_mac_addr->mac[0], device_mac_addr->mac[1], device_mac_addr->mac[2]
		, device_mac_addr->mac[3], device_mac_addr->mac[4], device_mac_addr->mac[5]
		);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event_set_bad (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_CL_DOWN_INFO_CONST_PTR_TYPE dinfo
	)
{
	argr->cm.cl.ev.current = *dinfo;
}

/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_ar_event_bad_is_different (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_CL_DOWN_INFO_CONST_PTR_TYPE dinfo
	)
{
	if (argr->cm.cl.ev.previous.reason_code != dinfo->reason_code
		||  argr->cm.cl.ev.previous.error_cause != dinfo->error_cause
		||  argr->cm.cl.ev.previous.pnio_status != dinfo->pnio_status)
	{
		return LSA_TRUE;
	}

#ifdef CM_MESSAGE
#if 0/*TODO: check*/
# pragma CM_MESSAGE("warning: TODO: more difference? current docu say yes, real needed?")

	if (argr->cm.cl.ev.previous.data2 != argr->cm.cl.ev.current.data2
		||  ...
		||  argr->cm.cl.ev.previous.mac_addr.mac[5] != argr->cm.cl.ev.current.mac_addr.mac[5]
		)
	{
		return LSA_TRUE;
	}
#endif
#endif /* CM_MESSAGE */

	/* note to usecase: indicate if ErrorCase is the same but ip-address differ
	// ErrorCause(_MP_IPv4_INVALID / ip-addr-1) => ErrorCause(MP_IPv4_INVALID / ip-addr-2)
	// between this two ErrorCauses there is always an ErrorCause(MP_IN_PROGRESS)
	*/

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_ar_event_bad_has_changed (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	return cm_cl_ar_event_bad_is_different (argr, &argr->cm.cl.ev.current);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event_get_abort_error_cause (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE error_cause
	)
{
	LSA_UINT16 i;
	CM_CL_DOWN_INFO_PTR_TYPE dinfo = &argr->cm.cl.ev.current;

	/* host-endianess */
	error_cause->reason_code = dinfo->reason_code;
	error_cause->padding     = 0;

	/* big-endianess */
	error_cause->error_cause = CM_HTON16 (dinfo->error_cause);

	switch (dinfo->error_cause)
	{
	case CM_ERROR_CAUSE_IP_MULTIPLE:
	case CM_ERROR_CAUSE_NOS_MULTIPLE:
	case CM_ERROR_CAUSE_IP_SET_ABORTED:
		CM_PUT32_HTON (error_cause->additional_info, 0, dinfo->pnio_status);
		CM_MEMCPY (&error_cause->additional_info[4], dinfo->mac_addr.mac, 6);
		i = 10;
		break;

	case CM_ERROR_CAUSE_LINK_DOWN:
	case CM_ERROR_CAUSE_MAU_MISMATCH:
	case CM_ERROR_CAUSE_PNIO_CONFIGURATION:
	case CM_ERROR_CAUSE_RPC_TIMEOUT:
	case CM_ERROR_CAUSE_IOD_NOT_FOUND:
	case CM_ERROR_CAUSE_OUT_OF_AR_RESOURCES:
	case CM_ERROR_CAUSE_NOS_SET_ABORTED:
	case CM_ERROR_CAUSE_IOC_ABORTED:
	case CM_ERROR_CAUSE_SRD_ABORTED:
	case CM_ERROR_CAUSE_VENDORID_ABORTED:
	case CM_ERROR_CAUSE_OUT_OF_CR_RESOURCES:
	case CM_ERROR_CAUSE_PDEV_OWNERSHIP:
	case CM_ERROR_CAUSE_SYSTEM_REDUNDANCY:
		CM_PUT32_HTON (error_cause->additional_info, 0, dinfo->pnio_status);
		i = 4;
		break;

	case CM_ERROR_CAUSE_RPC_ERROR:
	case CM_ERROR_CAUSE_IOD_ABORTED:
		CM_PUT32_HTON (error_cause->additional_info, 0, dinfo->pnio_status);
		CM_PUT32_HTON (error_cause->additional_info, 4, dinfo->data2);
		i = 8;
		break;

	case CM_ERROR_CAUSE_MISS:
		{
			LSA_UINT16 data2 = (LSA_UINT16)dinfo->data2;
			CM_PUT32_HTON (error_cause->additional_info, 0, dinfo->pnio_status);
			CM_PUT16_HTON (error_cause->additional_info, 4, data2);
			i = 6;
		}
		break;

	case CM_ERROR_CAUSE_MP_IN_PROGRESS:
	case CM_ERROR_CAUSE_MP_NOS_MISSING:
	case CM_ERROR_CAUSE_MP_IPv4_MISSING:
	case CM_ERROR_CAUSE_MP_NOS_INVALID:
	case CM_ERROR_CAUSE_MP_IPv4_INVALID:
	case CM_ERROR_CAUSE_MP_TAILORING_MISSING:
	case CM_ERROR_CAUSE_MP_PDEV_ERROR:
	case CM_ERROR_CAUSE_MP_DEVICE_DISABLED:
	case CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE:
		CM_PUT32_HTON (error_cause->additional_info, 0, dinfo->pnio_status);
		i = 4;
		break;

	default:
		CM_FATAL1 (error_cause->error_cause);
		return;
	}

	for (; i < sizeof(error_cause->additional_info); ++i)
	{
		error_cause->additional_info[i] = 0;
	}

	/* set to nothing-to-do */
	argr->cm.cl.ev.previous = argr->cm.cl.ev.current;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event_get_abort_ip (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32                 *device_ip
	)
{
	/* note: "argr->cm.cl.ev.ip_suite.device_ip" is the resolved-ip-addr */
	LSA_UINT32 nare_prop = (argr->cm.cl.dev->dev_addr.properties & CM_NARE_MODE_MASK);
	LSA_UINT32 ip        = 0;

	/* if device is a part of a masterproject and needs ip-addr-tailoring */
	if (cm_is_not_null (argr->cm.cl.mtd)  &&  (argr->cm.cl.mtd->properties & CM_PD_MTD_PROP_IP_SUITE))
	{
		LSA_UINT32 ioc_ipaddr, ioc_netmask, ioc_gateway;

		if (cm_oha_get_ip_suite (argr->ar_com.channel, &ioc_ipaddr, &ioc_netmask, &ioc_gateway))
		{
			LSA_UNUSED_ARG (ioc_gateway);
			if (cm_cl_mp_tailor_ipsuite (argr->cm.cl.dev, ioc_ipaddr, ioc_netmask))
			{
				ip = argr->cm.cl.dev->dev_addr.device_ip; /* tailoring sets the given-ip */
			}
		}
	}
	else if (nare_prop == CM_NARE_MODE_DCP_CONFIG_IP)
	{
		ip = argr->cm.cl.dev->dev_addr.device_ip; /* given-ip */
	}

	*device_ip = ip;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_ar_event_get_indata_ip (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_CL_IP_SUITE_PTR_TYPE ip_suite
	)
{
	*ip_suite = argr->cm.cl.ev.ip_suite;

	/* set good */
	argr->cm.cl.ev.current.reason_code = CM_AR_REASON_NONE;
	argr->cm.cl.ev.current.error_cause = CM_ERROR_CAUSE_NONE;
	argr->cm.cl.ev.current.pnio_status = CM_PNIO_ERR_NONE;

	/* set to not-changed */
	argr->cm.cl.ev.previous = argr->cm.cl.ev.current;
}


/*------------------------------------------------------------------------------
// check the ARFSUDataAdjust Block for consistency
//----------------------------------------------------------------------------*/
static LSA_BOOL
cm_ar_graph_check_ARFSUDataAdjust (
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size /* AP01034170 */
	)
{
	LSA_UINT32 offs;
	LSA_UINT16 block_type;
	LSA_UINT16 block_length;
	LSA_UINT32 block_count;

	/*
	 * ARFSUDataAdjust ::=
	 *   BlockHeader, [Padding*] a, { [FSParameterBlock], [FastStartUpBlock] } b
	 *
	 * a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
	 * b At least one optional block shall be existing.
	 *
	 *
	 * FSParameterBlock ::=
	 *   BlockHeader, [Padding*] a, FSParameterMode, FSParameterUUID, [Padding*] a
	 *
	 * FastStartUpBlock ::=
	 *   BlockHeader, [Padding*] a, Data*, [Padding*] a
	 */

	offs = 0;

	if (offs + 2 + 2 > size)
	{
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "size(%u) too short", size
			);
		return LSA_FALSE;
	}

	block_type   = CM_GET16_NTOH (data, offs + 0);
	block_length = CM_GET16_NTOH (data, offs + 2);

	if (offs + 2 + 2 + block_length != size)
	{
		CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
			, "block_type(0x%x), block_length(%u) not consistent with size(%u)"
			, block_type, block_length, size
			);
		return LSA_FALSE;
	}

	if (block_type != CM_BLOCK_TYPE_ARFSUDATAADJUST || block_length < 1 + 1 /*version*/ + 2 /*padding*/)
	{
		CM_CL_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "block_type(0x%x) wrong or block_length(%u) too short"
			, block_type, block_length
			);
		return LSA_FALSE;
	}

	offs += 2 + 2 + 1 + 1 /*version*/ + 2 /*padding*/;

	/***/

	block_count = 0;

	while (offs < size)
	{
		/*
		 * note: it was decided (24.9.2007) that the block_type is only range-checked here
		 *       thus more optional blocks may be defined without having to change this code
		 */
		if (offs + 2 + 2 > size)
		{
			CM_CL_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP
				, "too short for another optional block"
				);
			return LSA_FALSE;
		}

		block_type   = CM_GET16_NTOH (data, offs + 0);
		block_length = CM_GET16_NTOH (data, offs + 2);

		if (! (
				(block_type >= 0x0600 && block_type <= 0x060F) /* the range assigned to fast startup */
				&& block_type != CM_BLOCK_TYPE_ARFSUDATAADJUST /* but not the "outer" block */
				))
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x) wrong", block_type
				);
			return LSA_FALSE;
		}

		if (offs + 2 + 2 + block_length > size)
		{
			CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x), block_length(%u) not consistent with size(%u)"
				, block_type, block_length, size
				);
			return LSA_FALSE;
		}

		if (block_length < 1 + 1 /*version*/ || (block_length % 4) != 0)
		{
			CM_CL_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x), block_length(%u) too short or not 0 mod 4"
				, block_type, block_length
				);
			return LSA_FALSE;
		}

		/***/

		offs += 2 + 2 + block_length;

		block_count += 1;
	}

	if (block_count < 1)
	{
		CM_CL_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP
			, "not 'at least one optional block'"
			);
		return LSA_FALSE;
	}

	/***/

	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// build the unique slot_nr list
//----------------------------------------------------------------------------*/
static LSA_BOOL
cm_list_cmp__argr_slot_nr__slot_nr (
	CM_LIST_ENTRY_PTR_TYPE ListEntry,
	CM_LIST_ENTRY_PTR_TYPE NewEntry
	)
{
	CM_AR_GRAPH_SLOT_NR_PTR_TYPE p = (CM_AR_GRAPH_SLOT_NR_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_SLOT_NR_PTR_TYPE e = (CM_AR_GRAPH_SLOT_NR_PTR_TYPE) NewEntry;

	return p->slot_nr < e->slot_nr;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_ar_graph_build_unique_slot_nr (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_AR_GRAPH_AP_PTR_TYPE ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE mod;
	CM_AR_GRAPH_SLOT_NR_PTR_TYPE ptr;

	/* re-build */
	if (! CmListIsEmpty (&argr->cm.cl.unique_slot_nr_list))
	{
		cm_ar_graph_free_list_local_mem (&argr->cm.cl.unique_slot_nr_list);
	}

	CM_ASSERT (CmListIsEmpty (&argr->cm.cl.unique_slot_nr_list));

	/* build a list with all available slot_nr's */
	for (CmListForeach (ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			for (CmListForeach (ptr, &argr->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE))
			{
				if (ptr->slot_nr == mod->slot_nr)
				{
					break; /* for */
				}
			}

			/* if slot_nr not in list */
			if (cm_is_null (ptr))
			{
				/* add this new slot_nr */
				CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&ptr), sizeof (*ptr));
				if (cm_is_null (ptr))
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "no local-mem, size(%u)", sizeof(*ptr));
					return LSA_FALSE;
				}
				ptr->slot_nr = mod->slot_nr;
				CmListInsertSorted (&argr->cm.cl.unique_slot_nr_list, &ptr->link, cm_list_cmp__argr_slot_nr__slot_nr);
			}
		}
	}
	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// insert ARVendorBlock
//----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_ar_graph_insert_arvendor (
	CM_AR_GRAPH_PTR_TYPE argr,

	LSA_UINT16 req_blob_len,
	CM_COMMON_MEM_U8_PTR_TYPE req_blob
	)
{
#if 1 /* AP01556507: disable ARVendorBlock */
	CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARVendorBlock: not supported (AP01556507)");
	LSA_UNUSED_ARG (argr);
	LSA_UNUSED_ARG (req_blob_len);
	LSA_UNUSED_ARG (req_blob);
	return CM_PNIO_FAULTY_ARVendorBlockReq (0);
#else
	CM_COMMON_MEM_U8_PTR_TYPE dp;
	LSA_UINT32 left;
	LSA_UINT16 cm_dp_assert_line  = 0;
	LSA_UINT32 cm_dp_assert_error = 0;

	if (req_blob_len == 0 || cm_is_null (req_blob))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARVendorBlock: no vendor blocks");
		return CM_PNIO_FAULTY_ARVendorBlockReq (0); /* param-error */
	}

	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) == 0)
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARVendorBlock: not allowed if StartupMode == Legacy");
		return CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
	}

	/***/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("TODO: change all returns like ModuleDiffBlock does !")
#endif /* CM_MESSAGE */

	left = req_blob_len;
	dp   = req_blob;

	while (left > 0)
	{
		LSA_UINT16 block_type, block_length, block_version;
		LSA_UINT16 ap_structure_identifier;
		LSA_UINT32 api;

		/***/

		cm_dp_assert2 (left >= sizeof (CM_BLK_HEADER)
			, CM_PNIO_FAULTY_ARVendorBlockReq (1));  /* too few data left */

		cm_get_BLOCK_HEADER_type (&block_type);
		cm_get_BLOCK_HEADER_length (&block_length);
		cm_get_BLOCK_HEADER_version (&block_version);

		cm_dp_adjust (dp, left, sizeof (CM_BLK_HEADER));

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "BLOCK: type(0x%04x) len(%u) version(0x%04x)"
			, block_type, block_length, block_version
			);

		cm_dp_assert2 (block_type == CM_BLOCK_TYPE_ARVENDORBLOCK_REQ
			, CM_PNIO_FAULTY_ARVendorBlockReq (0));                                  /* wrong BlockType   */

		cm_dp_assert2 (block_length >= sizeof(block_version) + sizeof (CM_REQ_ARVENDOR)
			, CM_PNIO_FAULTY_ARVendorBlockReq (1));                                  /* invalid BlockLength */

		cm_dp_assert2 (left >= (block_length - sizeof(block_version))
			, CM_PNIO_FAULTY_ARVendorBlockReq (1));                                  /* too few data left */

		cm_dp_assert2 ((block_version & 0xFF00) == 0x0100
			, CM_PNIO_FAULTY_ARVendorBlockReq (2));                                 /* wrong BlockVersionHigh */

		cm_dp_assert2 ((block_version & 0x00FF) == 0x0000
			, CM_PNIO_FAULTY_ARVendorBlockReq (3));                                 /* wrong BlockVersionLow */

		/***/

		cm_dp_assert2 (left >= sizeof (CM_REQ_ARVENDOR)
			, CM_PNIO_FAULTY_ARVendorBlockReq (1));                                  /* too few data left */

		cm_get_REQ_ARV_APStructureIdentifier (&ap_structure_identifier);
		cm_get_REQ_ARV_API (&api);

		cm_dp_adjust (dp, left, sizeof (CM_REQ_ARVENDOR));

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ARVendorBlockReq: ap_structure_identifier(%u), api(%u)"
			, ap_structure_identifier, api
			);

		/***/

		{
			LSA_UINT16 data_len;
			LSA_UINT16 block_field;

			data_len = block_length - (sizeof(block_version) + sizeof (CM_REQ_ARVENDOR));

			cm_dp_assert2 (left >= data_len
				, CM_PNIO_FAULTY_ARVendorBlockReq (1)); /* too few data left */

			block_field = cm_ar_graph_insert_arvendor (argr
				, api, ap_structure_identifier, data_len, dp, 0
				);

			cm_dp_adjust (dp, left, data_len);

			if (block_field != 0)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "could not cm_ar_graph_insert_arvendor, field(%u)"
					, block_field
					);

				switch (block_field)
				{
				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
				default:
					return CM_PNIO_FAULTY_ARVendorBlockReq (block_field);
				}
			}
		}
	} /* while (left > 0) */

	return CM_PNIO_ERR_NONE;

cm_dp_assert_failed:
	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "IODConnectReq: parse ARVendorBlock failed, assert_error(0x%x) sourcecode-line(%u), current-parse-offset(%u)"
		, cm_dp_assert_error, cm_dp_assert_line, (LSA_UINT32)dp - (LSA_UINT32)req_blob
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);
	return cm_dp_assert_error;
#endif
}


/*------------------------------------------------------------------------------
// insert SubmoduleProperties block
//----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_ar_graph_insert_submodule_properties (
	CM_AR_GRAPH_PTR_TYPE argr,

	LSA_UINT16 req_blob_len,
	CM_COMMON_MEM_U8_PTR_TYPE req_blob
	)
{
	CM_COMMON_MEM_U8_PTR_TYPE dp = req_blob;
	LSA_UINT32 left = req_blob_len;
	LSA_UINT16 cm_dp_assert_line = 0;
	LSA_UINT32 cm_dp_assert_error = 0;
	LSA_UINT16 block_type, block_length, block_version;
	LSA_UINT16 number_of_apis, adj, n_api, reserved16_1, reserved16_2;

	cm_dp_assert2 ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* CoC/SubmoduleProperties require advanced startup mode */

	cm_dp_assert2 (left >= CM_BLK_HEADER_SIZE
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* not enough data left */

	block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
	block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
	block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

	cm_dp_assert2 (block_type == 0x3110
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* wrong BlockType */
	cm_dp_assert2 (block_length == left - (2 + 2)
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* wrong BlockLength */
	cm_dp_assert2 ((block_version & 0xFF00) == 0x0100
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* wrong BlockVersionHigh */
	cm_dp_assert2 ((block_version & 0x00FF) == 0x0000
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* wrong BlockVersionLow */

	cm_dp_adjust (dp, left, CM_BLK_HEADER_SIZE);

	adj = 2;
	cm_dp_assert2 (left >= adj
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* not enough data left */

	number_of_apis = CM_GET16_NTOH (dp, 0);

	cm_dp_assert2 (number_of_apis >= 1
		, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* too small */

	cm_dp_adjust (dp, left, adj);

	for (n_api = 0; n_api < number_of_apis; n_api++)
	{
		LSA_UINT32 api;
		LSA_UINT16 number_of_slots, n_slot;

		adj = 4 + 2 + 2;
		cm_dp_assert2 (left >= adj
			, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* not enough data left */

		api             = CM_GET32_NTOH (dp, 0);  /* checked later */
		reserved16_1    = CM_GET16_NTOH (dp, 4);
		number_of_slots = CM_GET16_NTOH (dp, 6);

		cm_dp_assert2 (reserved16_1 == 0x0
			, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null*/
		cm_dp_assert2 (number_of_slots >= 1
			, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* too small */

		cm_dp_adjust (dp, left, adj);

		for (n_slot = 0; n_slot < number_of_slots; n_slot++)
		{
			LSA_UINT16 slot, number_of_subslots, n_subslot;

			adj = 2 + 2 + 2 + 2;
			cm_dp_assert2 (left >= adj
				, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* not enough data left */

			slot               = CM_GET16_NTOH (dp, 0);
			reserved16_1       = CM_GET16_NTOH (dp, 2);
			reserved16_2       = CM_GET16_NTOH (dp, 4);
			number_of_subslots = CM_GET16_NTOH (dp, 6);

			cm_dp_assert2 (slot <= 0x7FFF
				, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* invalid slot number */
			cm_dp_assert2 (reserved16_1 == 0
				, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null*/
			cm_dp_assert2 (reserved16_2 == 0
				, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null*/
			cm_dp_assert2 (number_of_subslots >= 1
				, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* too small */

			cm_dp_adjust (dp, left, adj);

			for (n_subslot = 0; n_subslot < number_of_subslots; n_subslot++)
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE submodule;
				LSA_UINT16 subslot, coc;
				LSA_UINT32 reserved32_1, reserved32_2;

				adj = 2 + 2 + 4 + 4;
				cm_dp_assert2 (left >= adj
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* not enough data left */

				subslot      = CM_GET16_NTOH (dp, 0);
				coc          = CM_GET16_NTOH (dp, 2);
				reserved32_1 = CM_GET32_NTOH (dp, 4);
				reserved32_2 = CM_GET32_NTOH (dp, 8);

				cm_dp_assert2 (subslot <= 0x9FFF
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* invalid subslot number */

				cm_ar_graph_find_api_slot_subslot (argr, api, slot, subslot, LSA_NULL, LSA_NULL, &submodule);
				cm_dp_assert2 (cm_is_not_null (submodule)
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* submodule not found */

				if ((coc & CM_SUB_COC_GROUPID_MASK) == 0)
				{
					cm_dp_assert2 ((coc & CM_SUB_COC_IS_ACCESS_POINT) == 0
						, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* access point not in a coc group */
				}
				else
				{
					if ((argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0)
					{
						argr->ar.ar_properties |= CM_AR_PROP_COMBINED_OBJECT_CONTAINER;
					}
				}

				cm_dp_assert2 ((coc & CM_SUB_COC_RESERVED) == 0
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null */

				cm_dp_assert2 (reserved32_1 == 0
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null */

				cm_dp_assert2 (reserved32_2 == 0
					, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* reserved not null */

				cm_dp_adjust (dp, left, adj);

				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "IODConnectReq: SubmoduleProperties (0x%02x) api(%u)/slot(%u)/subslot(0x%x)"
					, coc, api, slot, subslot
					);
				submodule->cl.sub_coc_properties = coc;
			}
		}
	}

	/* note: consistency check will done in cm_cl_ar_graph_check_coc_properties() */

	cm_dp_assert2 (left == 0, CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ());  /* no padding */

	return CM_PNIO_ERR_NONE;

cm_dp_assert_failed:
	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "IODConnectReq: parse SubmoduleProperties table failed, assert_error(0x%x) sourcecode-line(%u), current-parse-offset(%u)"
		, cm_dp_assert_error, cm_dp_assert_line, (LSA_UINT32)dp - (LSA_UINT32)req_blob
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);
	return cm_dp_assert_error;
}


/*------------------------------------------------------------------------------
// insert RSInfo block
//----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_ar_graph_insert_rs_info (
	CM_AR_GRAPH_PTR_TYPE argr,

	LSA_UINT16 req_blob_len,
	CM_COMMON_MEM_U8_PTR_TYPE req_blob
	)
{
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) == 0)
	{
		CM_AR_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cm_cl_ar_graph_insert_rs_info: RSInfo block requires advanced startup"
			);
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();  /* RSInfo block requires advanced startup mode */
	}

	{
		LSA_UINT16 block_field;

		if (! cm_ar_graph_insert_rs_info (argr, req_blob, req_blob_len, &block_field))
		{
			switch (block_field)
			{
			case 255:
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			default:
				return CM_PNIO_FAULTY_RSInfoBlock (block_field);
			}
		}
	}

	return CM_PNIO_ERR_NONE; /* ok */
}


/*------------------------------------------------------------------------------
// append a record to *any* ar_graph-list
//	use local mem for record and record-data
//	use special allocation method to free this lists with cm_ar_graph_free_list_local_mem()
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_list_record_append (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_LIST_ENTRY_PTR_TYPE add_to_this_list,

	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 record_index,
	LSA_UINT16 transfer_sequence,
	LSA_BOOL mk_remanent,
	LSA_BOOL is_local,
	LSA_UINT32 record_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE record_data,

	CM_PD_RECORD_PTR_TYPE     * rec_ptr_ptr /* out */
	)
{
	CM_PD_RECORD_PTR_TYPE ptr;
	LSA_UINT32 alloc_size;

	*rec_ptr_ptr = LSA_NULL;

	/* argument check */
	alloc_size = sizeof (*ptr) + record_data_len;

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&ptr), alloc_size);

	if (cm_is_null (ptr))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "no local-mem, size(%u)", alloc_size);
		return 255; /* no mem (for record) */
	}

	ptr->backlink = argr;
	CmListInitialize (&ptr->prm_rec_link);

	ptr->api        = api;
	ptr->slot_nr    = slot_nr;
	ptr->subslot_nr = subslot_nr;

	ptr->record_index      = record_index;
	ptr->transfer_sequence = transfer_sequence;

	ptr->mk_remanent = mk_remanent;
	ptr->is_local    = is_local;

	ptr->tailor_data_alloc_len = 0;
	ptr->tailor_data_len       = CM_TAILOR_DATA_LEN_NOT_TAILORED;
	ptr->tailor_data           = LSA_NULL;

	ptr->record_data_len = (LSA_UINT16)record_data_len;
	ptr->record_data     = (CM_COMMON_MEM_U8_PTR_TYPE)(ptr + 1);        /* request 1502507 (lint) */

	CM_MEMCPY (ptr->record_data, record_data, record_data_len); /* dynamic size, see typedef */

	/***/

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "append-record to submod(%u/%u/0x%x) rec_index(0x%x) rec_len(%u)"
		, api, slot_nr, subslot_nr, record_index, record_data_len
		);

	CmListInsertTail (add_to_this_list, &ptr->link); /* NOTE: not sorted! */

	/***/

	*rec_ptr_ptr = ptr;
	return 0; /* ok block_field */
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_cl_ar_graph_record_find_sub_coc_ap (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
	)
{
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub;

	cm_ar_graph_find_api_slot_subslot (argr, api, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &sub);

	if (cm_is_not_null (sub)  &&  (sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) != 0)
	{
		CM_AR_GRAPH_AP_PTR_TYPE ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE mod;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap;
		LSA_INT32 coc_prop = (CM_SUB_COC_IS_ACCESS_POINT | (sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK));

		for (CmListForeach (ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub_coc_ap, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					if (coc_prop == (sub_coc_ap->cl.sub_coc_properties & (CM_SUB_COC_IS_ACCESS_POINT | CM_SUB_COC_GROUPID_MASK)))
					{
						return sub_coc_ap;
					}
				}
			}
		}
	}

	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_cl_ar_graph_record_find_coc_access_point (
	CM_PD_RECORD_CONST_PTR_TYPE rec
	)
{
	return cm_cl_ar_graph_record_find_sub_coc_ap ((CM_AR_GRAPH_PTR_TYPE)rec->backlink
		, rec->api, rec->slot_nr, rec->subslot_nr
		);
}


/*------------------------------------------------------------------------------
// convert an ARCB_ADD (see CM_OPC_CL_DEVICE_ADD) to an internal AR_GRAPH
//----------------------------------------------------------------------------*/
static LSA_UINT32
cm_ar_graph_from_arcb_add (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CL_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE arcb,
	LSA_UINT16 local_clock,
	LSA_BOOL is_reduction_ratio_fixed,
	CM_AR_GRAPH_PTR_TYPE *ar_graph
	)
{
	CM_MAC_TYPE null_mac = { {0, 0, 0, 0, 0, 0} };
	CM_AR_GRAPH_PTR_TYPE argr;
	CLRPC_UUID_TYPE cmi_object_uuid;
	LSA_UINT16 block_field;

	LSA_UNUSED_ARG (is_reduction_ratio_fixed);

	cm_rpc_make_object_uuid (&cmi_object_uuid
		, channel->usr.cl.vendor_id, channel->usr.cl.device_id, channel->usr.cl.instance
		);

	*ar_graph = cm_ar_graph_alloc (channel, dev->device_nr, arcb->ar_nr);
	if (cm_is_null (*ar_graph))
	{
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
	}

	argr = *ar_graph;

	block_field = cm_ar_graph_insert_io_ar (argr
		, dev->dev_addr.device_ip, arcb->ar_type, &arcb->ar_uuid
		, null_mac.mac, &cmi_object_uuid
		, arcb->ar_properties, dev->cmi_timeout100ms, 0 /* see below (search for cmi_udp_port) */
		, 0, LSA_NULL
		);
	if (block_field != 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "io_ar.block_field(%u)", block_field);
		switch (block_field)
		{
		case 255: /* no mem */
			return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
		default:
			return CM_PNIO_FAULTY_ARBlockReq (block_field);
		}
	}

	/* IRInfoBlock */
	if (cm_is_not_null (arcb->ir_info))
	{
		CLRPC_UUID_PTR_TYPE ir_data_uuid;

		if (! (argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3 && (argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0))  /*AP01203539*/
		{
			CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ir_info-block but the AR has no RTC3 and advanced-startup flag");
			return CM_PNIO_FAULTY_IRInfoBlock (0);
		}

		ir_data_uuid = cm_pdusr_get_ir_data_uuid (argr->ar_com.channel);

		if (! cm_uuid_equal (&arcb->ir_info->ir_data_uuid, ir_data_uuid))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ir_info.block_field(%u)", 5 /*IRDATAUUID is faulty*/);
			return CM_PNIO_FAULTY_IRInfoBlock (5 /*IRDATAUUID is faulty*/);

		}

		block_field = cm_ar_graph_insert_ir_info (argr, &arcb->ir_info->ir_data_uuid, 0);
		if (block_field == 0)
		{
			CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE irInfoSub;

			for (CmListForeach (irInfoSub, &arcb->ir_info->ir_info_sub_list, CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE))
			{
				block_field = cm_ar_graph_insert_ir_info_sub (argr
					, irInfoSub->iocr_ref, irInfoSub->subframe_offset, irInfoSub->subframe_data
					);
				if (block_field != 0)
				{
					break;
				}
			}
		}
		if (block_field != 0)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ir_info.block_field(%u)", block_field);
			switch (block_field)
			{
			case 255: /* no mem */
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			default:
				return CM_PNIO_FAULTY_IRInfoBlock (block_field);
			}
		}
	}

	/* SRInfoBlock */
	if (cm_is_not_null (arcb->sr_info))
	{
		block_field = cm_ar_graph_insert_sr_info (argr
			, arcb->sr_info->rdh_factor, arcb->sr_info->sr_properties, 0
			);
		if (block_field != 0)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "sr_info.block_field(%u)", block_field);
			switch (block_field)
			{
			case 255: /* no mem */
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			default:
				return CM_PNIO_FAULTY_SRInfoBlock (block_field);
			}
		}
	}

	/* ARVendorBlock */
	if (arcb->arvendor_blob.arvendor_data_len != 0)
	{
		LSA_UINT32 pnio_err;

		pnio_err = cm_cl_ar_graph_insert_arvendor (argr, arcb->arvendor_blob.arvendor_data_len, arcb->arvendor_blob.arvendor_data);

		if (pnio_err != 0)
		{
			return pnio_err;
		}
	}

	/* PrmServerBlock */
	if (cm_is_not_null (arcb->prm))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PrmServerBlock not supported");
		return CM_PNIO_FAULTY_PrmServerReqBlockReq (0); /* prm-block not supported */
	}


	/* ar-related prm_records */
	{
		CM_UPPER_ARCB_RECORD_PTR_TYPE cb_rec;
		CM_PD_RECORD_PTR_TYPE rec;
		LSA_UINT16 e050_cnt = 0;

		block_field = 0;

		for (CmListForeach (cb_rec, &arcb->ar_prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
		{
			LSA_UINT16 record_index;

			if (! (0xe000 <= cb_rec->record_index && cb_rec->record_index <= 0xefff))
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "Faulty AR specific index(0x%x)", cb_rec->record_index);
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			}

			record_index = (LSA_UINT16)cb_rec->record_index;

			if ((arcb->ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0) /* AP01314661 */
			{
				if (record_index == 0xE050) /* ARFSUDataAdjust data */
				{
					if (cm_ar_graph_check_ARFSUDataAdjust (cb_rec->record_data, cb_rec->record_data_len))
					{
						block_field = cm_cl_list_record_append (argr, &argr->cm.cl.ar_connect_prm_record_list, 0, 0, 0
							, record_index, cb_rec->transfer_sequence, LSA_FALSE, LSA_FALSE
							, cb_rec->record_data_len, cb_rec->record_data, &rec
							);
						e050_cnt += 1;
					}
					else
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARFSUDataAdjust.block_length(%u) mismatch", cb_rec->record_data_len);
						return CM_PNIO_FAULTY_ARFSUBlock (1);
					}
				}
				else
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ar-specific index(0x%x) not allowed", record_index);
					return CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
				}
			}
			else
			{
				block_field = cm_cl_list_record_append (argr, &argr->cm.cl.ar_prm_record_list, 0, 0, 0
					, record_index, cb_rec->transfer_sequence, LSA_FALSE, LSA_FALSE
					, cb_rec->record_data_len, cb_rec->record_data, &rec
					);
			}

			if (e050_cnt >= 2)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARFSUDataAdjust twice");
				return CM_PNIO_FAULTY_ARFSUBlock (0);
			}
			if (block_field != 0)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ar-record.block_field(%u)", block_field);
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			}
		}
	}


	/* IO-CR blocks */
	{
		CM_UPPER_ARCB_IOCR_PTR_TYPE cb_iocr;
		CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

		for (CmListForeach (cb_iocr, &arcb->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
		{
			CM_UPPER_ARCB_IOAPI_PTR_TYPE cb_ioapi;
			CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE cb_iodata;
			CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE iodata;
			CM_UPPER_ARCB_IOCS_PTR_TYPE cb_iocs;
			CM_AR_GRAPH_IOCS_PTR_TYPE iocs;
			LSA_UINT16 data_length;

			data_length = cb_iocr->data_length;

			block_field = cm_ar_graph_insert_io_cr (argr
				, cb_iocr->iocr_type, cb_iocr->iocr_ref
				, 0x8892, cb_iocr->iocr_properties
				, data_length, cb_iocr->frame_id
				, cb_iocr->send_clock, cb_iocr->reduction_ratio, cb_iocr->phase
				, cb_iocr->frame_send_offset
				, cb_iocr->data_hold_factor /*AP01304706*/, cb_iocr->data_hold_factor
				, 0xC000 /* TIA 946498 */
				, cb_iocr->iocr_multicast_mac.mac
				, &iocr
				);
			if (block_field == 0)
			{
				block_field = cm_cl_ar_graph_set_iocr_edd_data (iocr
					, cb_iocr->edd_data_offset, cb_iocr->edd_properties
					, cb_iocr->edd_cr_number, cb_iocr->edd_forward_mode, cb_iocr->edd_ct_outdatastart
					, cb_iocr->partial_length, cb_iocr->partial_offset, cb_iocr->apdu_status_length
					, cb_iocr->subframe_data, cb_iocr->subframe_offset
					);
			}
			if (block_field != 0)
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "io_cr.block_field(%u) iocr_ref(%u)", block_field, cb_iocr->iocr_ref);
				switch (block_field)
				{
				case 255: /* no mem */
				case 254: /* no provider- or consumer-resource */
				case 253: /* unreachable */
					return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
				default:
					return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
				}
			}

			CM_ASSERT (cm_is_not_null (iocr));

			/* prepare cnf */
			cb_iocr->frame_id = iocr->frame_id;     /* cnf ! */
			/*
			// class1 & 2:	frame_id only valid for consumer
			// class3:		frame_id must be planned
			*/

			/* see cm_arcb_device_add_set_iocr_cnf() */
			cb_iocr->edd_apdustatus_offset = EDD_DATAOFFSET_INVALID;
			cb_iocr->edd_cw_offset         = EDD_DATAOFFSET_INVALID;
			cb_iocr->edd_datastatus_offset = EDD_DATAOFFSET_INVALID;


			for (CmListForeach (cb_ioapi, &cb_iocr->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
			{
				CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioap;

				block_field = cm_argr_insert_ioap (iocr, cb_ioapi->api, &ar_ioap);
				if (block_field != 0)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ioap.block_field(%u)", block_field);
					switch (block_field)
					{
					case 255: /* no mem */
						return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
					default:
						return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
					}
				}
				CM_ASSERT (cm_is_not_null (ar_ioap));

				for (CmListForeach (cb_iodata, &cb_ioapi->related_iodata_objects_list, CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE))
				{
					block_field = cm_argr_insert_iodata_objects (ar_ioap
						, cb_iodata->slot_nr, cb_iodata->subslot_nr, cb_iodata->frame_offset, &iodata
						);
					if (block_field != 0)
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iodata.block_field(%u)", block_field);
						switch (block_field)
						{
						case 255: /* no mem */
							return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
						default:
							return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
						}
					}
					CM_ASSERT (cm_is_not_null (iodata));
				}

				for (CmListForeach (cb_iocs, &cb_ioapi->related_iocs_list, CM_UPPER_ARCB_IOCS_PTR_TYPE))
				{
					block_field = cm_argr_insert_iocs (ar_ioap
						, cb_iocs->slot_nr, cb_iocs->subslot_nr, cb_iocs->frame_offset, &iocs
						);
					if (block_field != 0)
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iocs.block_field(%u)", block_field);
						switch (block_field)
						{
						case 255: /* no mem */
							return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
						default:
							return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
						}
					}
					CM_ASSERT (cm_is_not_null (iocs));
				}
			}
		}
	}

	/* IO-CR local-adaption blocks */
	{
		CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE cb_iocr_local;
		LSA_UINT16 adpt_send_clock = 0;

		for (CmListForeach (cb_iocr_local, &arcb->iocr_local_list, CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE))
		{
			block_field = cm_cl_ar_graph_set_iocr_adaption (argr, cb_iocr_local->iocr_ref, local_clock
				, cb_iocr_local->send_clock, cb_iocr_local->reduction_ratio, cb_iocr_local->phase
				, cb_iocr_local->frame_send_offset, cb_iocr_local->data_hold_factor
				);
			if (block_field != 0)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iocr_adaption.block_field(%u)", block_field);
				return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
			}
			if (adpt_send_clock == 0)
			{
				adpt_send_clock = cb_iocr_local->send_clock;
			}
		}

		/* AP01009553: if no adaptation is given, take the iocr-data as adaptation */
		if (adpt_send_clock == 0)
		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;
			for (CmListForeach (iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
			{
				block_field = cm_cl_ar_graph_set_iocr_adaption (argr, iocr->iocr_ref, local_clock
					, iocr->send_clock, iocr->reduction_ratio, iocr->phase
					, iocr->frame_send_offset, iocr->data_hold_factor
					);
				if (block_field != 0)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "adaptation to self failed: iocr_adaption.block_field(%u)", block_field);
					return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
				}
				if (adpt_send_clock == 0)
				{
					adpt_send_clock = iocr->send_clock;
				}
			}
		}

		/* check: are all IOCR adapted? */
		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

			for (CmListForeach (iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
			{
				if (iocr->cm.adpt.send_clock == 0)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "adaptation of iocr_ref(%u) missing", iocr->iocr_ref);
					return CM_PNIO_FAULTY_IOCRBlockReq (10);  /* adpt.send_clock not set */
				}
				if (iocr->cm.adpt.send_clock != adpt_send_clock)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "adaptation of iocr_ref(%u) differ to other adaption", iocr->iocr_ref);
					return CM_PNIO_FAULTY_IOCRBlockReq (10);  /* adpt.send_clock differ */
				}
			}

			argr->ar_com.ar_send_clock = adpt_send_clock; /* same as local_clock, see cm_cl_ar_graph_set_iocr_adaption() */
		}
	}

	/* M-CR blocks */
	{
		CM_UPPER_ARCB_MCR_PTR_TYPE cb_mcr;
		CM_AR_GRAPH_MCR_PTR_TYPE mcr;

		for (CmListForeach (cb_mcr, &arcb->mcr_list, CM_UPPER_ARCB_MCR_PTR_TYPE))
		{
			block_field = cm_ar_graph_insert_mcr (argr
				, cb_mcr->iocr_ref, cb_mcr->mcr_properties, cb_mcr->mci_timeout_100ms
				, cb_mcr->provider_name_length, cb_mcr->provider_name
				, &mcr
				);
			if (block_field != 0)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "mcr.block_field(%u)", block_field);
				switch (block_field)
				{
				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
				default:
					return CM_PNIO_FAULTY_MCRBlockReq (block_field);
				}
			}
			CM_ASSERT (cm_is_not_null (mcr));
		}
	}

	/* alarm block */
	if (cm_is_null (arcb->alarm_cr))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "AlarmCRBlock doesn't exist");
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
	}
	else
	{
		CM_AR_GRAPH_ALARM_CR_PTR_TYPE al_cr;

		block_field = cm_ar_graph_insert_alarm_cr (argr
			, CM_ALCR_TYPE_ALARM, 0x8892, arcb->alarm_cr->alcr_properties
			, arcb->alarm_cr->timeout100ms, arcb->alarm_cr->retries
			, ACP_INVALID_ALARM_REF/*remote alarm_ref*/
			, channel->acp.max_alarm_data_length
			, 0xC000 /* TIA 946498 */, 0xA000 /* TIA 946498 */
			, &al_cr
			);
		if (block_field != 0)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "alarm_cr.block_field(%u)", block_field);
			switch (block_field)
			{
			case 255: /* no mem */
			case 254: /* no alarm-ref */
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			default:
				return CM_PNIO_FAULTY_AlarmCRBlockReq (block_field);
			}
		}
		CM_ASSERT (cm_is_not_null (al_cr));
	}


	/* api / slot / subslot */
	{
		CM_UPPER_ARCB_AP_PTR_TYPE cb_ap;
		CM_AR_GRAPH_AP_PTR_TYPE ap;

		CM_UPPER_ARCB_MODULE_PTR_TYPE cb_mod;
		CM_AR_GRAPH_MODULE_PTR_TYPE mod;

		CM_UPPER_ARCB_SUBMODULE_PTR_TYPE cb_submod;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE submod;

		CM_UPPER_ARCB_RECORD_PTR_TYPE cb_rec;
		CM_PD_RECORD_PTR_TYPE rec;

		for (CmListForeach (cb_ap, &arcb->ap_list, CM_UPPER_ARCB_AP_PTR_TYPE))
		{
			/* no APs checked in cm_ar_graph_check_consistency() */

			for (CmListForeach (cb_mod, &cb_ap->module_list, CM_UPPER_ARCB_MODULE_PTR_TYPE))
			{
				block_field = cm_ar_graph_insert_ap_module (argr
					, cb_ap->api, cb_mod->slot_nr, cb_mod->mod_ident, cb_mod->mod_properties
					, LSA_NULL, &ap, &mod
					);
				if (block_field != 0)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ap.block_field(%u)", block_field);
					switch (block_field)
					{
					case 255: /* no mem */
						return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
					default:
						return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (block_field);
					}
				}
				CM_ASSERT (cm_is_not_null (mod));

				for (CmListForeach (cb_submod, &cb_mod->submodule_list, CM_UPPER_ARCB_SUBMODULE_PTR_TYPE))
				{
					LSA_UINT16 cnt = 0;
					LSA_UINT16 tmp;

					block_field = cm_ar_graph_insert_submodule (argr, mod
						, cb_submod->subslot_nr, cb_submod->sub_ident, cb_submod->sub_properties
						, channel->usr.cl.IOXS_Required
						, &submod
						);
					if (block_field != 0)
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "mod.block_field(%u)", block_field);
						switch (block_field)
						{
						case 255: /* no mem */
							return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
						default:
							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (block_field);
						}
					}

					tmp = CM_CAST_U16 (cb_submod->sub_properties & CM_SUB_PROP_TYPE_MASK);
					switch (tmp)
					{
					case CM_SUB_PROP_TYPE_NO_DATA:
					case CM_SUB_PROP_TYPE_INPUT:
					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						++cnt;
						block_field = cm_ar_graph_set_data_descr (submod
							, 0 /* 0..input   1..output */, cb_submod->descr[0].data_description
							, cb_submod->descr[0].data_length, cb_submod->descr[0].iops_length, cb_submod->descr[0].iocs_length
							);
						if (block_field != 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submod.block_field(%u)", block_field);
							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (block_field);
						}
						break;

					default:
						/* nothing */
						break;
					}

					switch (tmp)
					{
					case CM_SUB_PROP_TYPE_OUTPUT:
					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						++cnt;
						block_field = cm_ar_graph_set_data_descr (submod
							, 1 /* 0..input   1..output */, cb_submod->descr[1].data_description
							, cb_submod->descr[1].data_length, cb_submod->descr[1].iops_length, cb_submod->descr[1].iocs_length
							);
						if (block_field != 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submod.block_field(%u)", block_field);
							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (block_field);
						}
						break;

					default:
						/* nothing */
						break;
					}

					if (cnt == 0)
					{
						CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "wrong SubmoduleProperties.Type(0x%x), submod.block_field(12)", tmp
							);
						return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (12); /* wrong SubmoduleProperties */
					}
				}
			}
		}

		/* SubmoduleProperties table */
		if (arcb->submodule_properties_blob.submodule_properties_data_len != 0)
		{
			LSA_UINT32 pnio_err;

			pnio_err = cm_cl_ar_graph_insert_submodule_properties (argr
				, arcb->submodule_properties_blob.submodule_properties_data_len
				, arcb->submodule_properties_blob.submodule_properties_data
				);

			if (pnio_err != 0)
			{
				return pnio_err;
			}
		}

		/* RSInfo block */
		if (arcb->rs_info.rs_info_data_len != 0)
		{
			LSA_UINT32 pnio_err;

			pnio_err = cm_cl_ar_graph_insert_rs_info (argr,
				arcb->rs_info.rs_info_data_len,
				arcb->rs_info.rs_info_data);

			if (pnio_err != 0)
			{
				return pnio_err;
			}
		}

		for (CmListForeach (cb_ap, &arcb->ap_list, CM_UPPER_ARCB_AP_PTR_TYPE))
		{
			for (CmListForeach (cb_mod, &cb_ap->module_list, CM_UPPER_ARCB_MODULE_PTR_TYPE))
			{
				for (CmListForeach (cb_submod, &cb_mod->submodule_list, CM_UPPER_ARCB_SUBMODULE_PTR_TYPE))
				{
					cm_ar_graph_find_api_slot_subslot (argr, cb_ap->api, cb_mod->slot_nr, cb_submod->subslot_nr, &ap, &mod, &submod);
					if (cm_is_null (submod))
					{
						CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submodule(%u/%u/0x%x) not found !", cb_ap->api, cb_mod->slot_nr,
							cb_submod->subslot_nr);
						return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource (); /* submodule not found */
					}

					/* now record data */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: if shared not prm-records allowed")
#endif /* CM_MESSAGE */
					for (CmListForeach (cb_rec, &cb_submod->u.cl.prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
					{
						if ((submod->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) == 0)
						{
							if (cb_rec->record_data_len + CM_RECORD_OFFSET > dev->dev_rpc.max_supported_record_length)
							{
								CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submodule-record.block_length(%u)", cb_rec->record_data_len);
								return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
							}
						}
						else
						{
							if (cb_rec->record_data_len + CM_RECORD_OFFSET + CM_CL_PDU_COC_HEADER > dev->dev_rpc.max_supported_record_length)
							{
								CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "coc-submodule-record.block_length(%u)",
									cb_rec->record_data_len);
								return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
							}
						}

						if (cb_rec->record_index == 0x00017081 /* PDMasterTailorData */)
						{
							argr->cm.cl.mtd = cm_pd_mtd_parse (channel, dev->device_nr, cb_rec->record_data_len, cb_rec->record_data);
							if (cm_is_null (argr->cm.cl.mtd))
							{
								return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
							}
						}
						else if (cb_rec->record_index > 0xFFFF
								 ||  cb_rec->record_index == 0xE040/* WriteMultiple */
								 ||  cb_rec->record_index == 0x80B0 /* CombinedObjectContainer */)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "invalid record_index(0x%x)", cb_rec->record_index);
							return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
						}
						else
						{
							block_field = cm_cl_list_record_append (argr, &submod->cl.prm_record_list
								, ap->api, mod->slot_nr, submod->subslot_nr
								, cb_rec->record_index, cb_rec->transfer_sequence, LSA_FALSE, LSA_FALSE, cb_rec->record_data_len, cb_rec->record_data
								, &rec
								);
							if (block_field != 0)
							{
								CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submodule-record.block_field(%u)", block_field);
								return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
							}
						}
					}
				}
			}
		}
	}


	CM_ASSERT (argr->ar.cmi_udp_port == 0); /* see cm_ar_graph_insert_io_ar() above */

	if (argr->ar_com.has_udp)
	{
		argr->ar.cmi_udp_port = CM_IANA_PNIO_UDP_UNICAST_PORT_NBO;
	}
	else
	{
		argr->ar.cmi_udp_port = CM_HTON16 (0x8892); /* AP00583478 */
	}

	{
		LSA_UINT32 pnio_err;

		pnio_err = cm_ar_graph_check_consistency (argr);

		if (pnio_err != CM_PNIO_ERR_NONE)
		{
			if (CM_PNIO_ERR_FETCH_CODE_1 (pnio_err) == 64 /*CMRPC*/)
			{
				pnio_err = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			}
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "return from cm_ar_graph_check_consistency:pnio_err(0x%x)", pnio_err);
			return pnio_err;
		}

		pnio_err = cm_cl_ar_graph_check_coc_properties (argr);

		if (pnio_err != CM_PNIO_ERR_NONE)
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "return from cm_cl_ar_graph_check_coc_properties:pnio_err(0x%x)", pnio_err
				);
			return pnio_err;
		}
	}

	/* pre-build */
	if (! cm_ar_graph_build_unique_slot_nr (argr))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "can't build unique_slot_nr_list");
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
	}

	/* calc startup mode */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
	{
		if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3)
		{
			argr->cm.cl.sumode = CM_SUMODE_AdvRTClass3;
		}
		else /* CM_AR_TYPE_SINGLE, CM_AR_TYPE_SINGLE_SYSRED */
		{
			argr->cm.cl.sumode = CM_SUMODE_AdvRT;
		}
	}
	else
	{
		if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3)
		{
			argr->cm.cl.sumode = CM_SUMODE_LegacyRTClass3;
		}
		else /* CM_AR_TYPE_SINGLE */
		{
			argr->cm.cl.sumode = CM_SUMODE_LegacyRT;
		}
	}

#if CM_DEBUG
	cm_ar_graph_dump (argr);
#endif

	return CM_PNIO_ERR_NONE;
}


/*------------------------------------------------------------------------------
//  opcode handler
//----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_DEVICE_ADD
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_device_add (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_UPPER_CL_DEVICE_ADD_PTR_TYPE add;
	CM_CL_DEVICE_PTR_TYPE dev;
	CM_UPPER_ARCB_ADD_PTR_TYPE arcb = LSA_NULL;
	LSA_UINT16 local_clock;
	LSA_BOOL is_reduction_ratio_fixed;
	LSA_UINT32 device_ip = 0;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd;

	/* sequence check */
	if (channel->state != CM_CL_READY)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "wrong ch.state(%u)", channel->state);
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	if (! cm_pdusr_is_parameterized (channel, &local_clock, &is_reduction_ratio_fixed))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "the PDEV is not parameterized");
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	if (cm_pdusr_is_reset_to_factory (channel))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "the PDEV is in state reset-to-factory, must wait");
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	add              = rb->args.cl.dev_add;
	add->pnio_status = CM_PNIO_ERR_NONE;

	dev = cm_cl_dev_add_device_nr (channel, add->device_nr);
	if (cm_is_null (dev))
	{
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
		cm_callback (channel, CM_ERR_RESOURCE, &rb);
		return;
	}
	CM_ASSERT (dev->state == CM_CL_DEV_ST_NOI  &&  dev->device_nr == 0);

	/* check params */
	if ((add->device_mode & CM_CL_DEVICE_MODE_RESERVED_MASK) != 0)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "a reserved bit was set, device_mode(0x%x)", add->device_mode
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	{
		LSA_UINT scan_cycle = CM_CL_DEV_SCAN_CYCLE_10MS (add->device_mode);

		if (scan_cycle != 0)
		{
			if (! (10 <= scan_cycle && scan_cycle <= 200))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "scan cylce not in range, !(10 <= %u <= 200), device_mode(0x%x)"
					, scan_cycle, add->device_mode
					);
				add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}
		}
	}

	switch (add->device_mode & CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_MASK)
	{
	case CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_NO:
		break;

	case CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_YES:
		if (add->dev_addr.device_gateway_ip == 0)
		{
			add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		}
		break;

	default:
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		break;
	}

	if (add->pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "invalid default router settings: device_ip(0x%x), subnet_mask(0x%x), gateway_ip(0x%x), device_mode(0x%x)"
			, add->dev_addr.device_ip, add->dev_addr.device_subnet_mask, add->dev_addr.device_gateway_ip, add->device_mode
			);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	switch (add->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK)
	{
	case CM_CL_DEVICE_MODE_IS_FSU_NO:
	default:
		break;

	case CM_CL_DEVICE_MODE_IS_FSU_YES:

		if (cm_cl_fsu_count (channel, 0) >= CM_CFG_MAX_CL_FSU_DEVICES)
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "too many fsu-devices, CM_CFG_MAX_CL_FSU_DEVICES(%u)", CM_CFG_MAX_CL_FSU_DEVICES
				);
			add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource ();
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;/*AP00544039*/
		}
		break;
	}

	/* CM_CL_DEVICE_MODE_IS_ACCELERATED_MASK:			all entries are valid */

	/* CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_MASK:		all entries are valid */

	/* CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_MASK:			all entries are valid */

	/* CM_CL_DEVICE_MODE_NOS_OVERWRITE_MASK:			all entries are valid */

	/***/

	/* CM_CL_DEVICE_PROPERTIES_MULTIPLE_WRITE:			all entries are valid */

	/* CM_CL_DEVICE_PROPERTIES_SYSTEM_REDUNDANCY_MASK:	all entries are valid */

	/* CM_CL_DEVICE_PROPERTIES_CIR_MASK:				all entries are valid */

	/* CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM:	see check below */

	if ((add->device_properties & CM_CL_DEVICE_PROPERTIES_RESERVED_MASK) != 0)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "a reserved bit was set, device_properties(0x%x)", add->device_properties
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if ((add->run_mode & CM_CL_RMODE_RESERVED_MASK) != 0)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "a reserved bit was set, run_mode(0x%x)", add->run_mode
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	/* CM_CL_RMODE_DISABLE_DATA_STATUS_IND: all entries are valid */

	if ((add->dev_addr.properties & CM_NARE_MODE_RESERVED_MASK) != 0)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "a reserved bit was set, dev_addr.properties(0x%x)", add->dev_addr.properties
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	/* CM_NARE_MODE_DONTCARE_MASK: all entries are valid */

	/* CM_NARE_MODE_MASK: all entries are valid */

	/* CM_NARE_MODE_SETIP_MASK: all entries are valid */

	if (! cm_is_valid_station_name (add->dev_addr.device_name_length, add->dev_addr.device_name))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "invalid device_name, device_name_length(%u)"
			, add->dev_addr.device_name_length
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CTLDINA_DCPNoRealStationName ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	/* add->dev_rpc.write_multiple_is_supported: true and false is valid */

	if (add->dev_rpc.max_supported_record_length == 0)
	{
		/*	RpcMustRecvFragSize ::= 1464
		**
		**	((1464 - 80/RpcHeader/) * 3/fragments/) == sizeof(NDRDataRequest) == 4152
		**
		**	(4152 - 20/PnioOffset/) == sizeof(PROFINETIOServiceReqPDU) == 4132
		**
		**	(4152 - 20/PnioOffset/ - 64/IODWriteReqHeader/) == 4068
		**
		**	(20/PnioOffset/ + 64/IODWriteReqHeader/) == CM_RECORD_OFFSET
		*/
		add->dev_rpc.max_supported_record_length = 4152;

		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "set max_supported_record_length to default(%u)", add->dev_rpc.max_supported_record_length
			);
	}

	if (add->dev_rpc.max_supported_record_length <= CM_RECORD_OFFSET
		||  add->dev_rpc.max_supported_record_length > (0xFFFF + CM_RECORD_OFFSET))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "invalid max_supported_record_length(%u)", add->dev_rpc.max_supported_record_length
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	/* currently supports:
	//	- a single-ar
	*/
	if (cm_is_not_null(add->arcb))
	{
		arcb = add->arcb;
		if (! CM_AR_IS_SINGLE (arcb->ar_properties))
		{
			CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) ar(%u): and ar_properties(0x%x) mismatch", add->device_nr, arcb->ar_nr, arcb->ar_properties
				);
			add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (9); /* ARProperties */
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}
		if ((arcb->ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) != 0)
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) ar(%u): ARProperties.CombinedObjectContainer must not be set", add->device_nr, arcb->ar_nr);
			add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (9); /* ARProperties */
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}
		if (! (1 <= arcb->ar_nr && arcb->ar_nr <= 255))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) ar(%u): only ar_nr 1..255 allowed", add->device_nr, arcb->ar_nr);
			add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (255); /* no mem */
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}
	}
	else
	{
		/* note: AP00542516 (stable order of frames for RT, RT* and IRTflex)
		**	if there is more than one ar, then the ar-list must be sorted by ar_nr
		*/
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%d): No AR found!", add->device_nr);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

/*----------------------------------------------------------------------------*/

	dev->state = CM_CL_DEV_ST_PASSIVE;

	dev->device_nr         = add->device_nr;
	dev->device_mode       = add->device_mode;
	dev->device_properties = add->device_properties;
	dev->run_mode          = add->run_mode;
	dev->cmi_timeout100ms  = add->cmi_activity_timeout100ms;

	dev->dev_addr.properties = add->dev_addr.properties;
	dev->dev_addr.device_name_length          = 0;
	dev->dev_addr.device_name                 = LSA_NULL;
	dev->dev_addr.tailored_device_name_length = 0;
	dev->dev_addr.tailored_device_name        = LSA_NULL;
	dev->dev_addr.device_ip                   = add->dev_addr.device_ip;
	dev->dev_addr.device_subnet_mask          = add->dev_addr.device_subnet_mask;
	dev->dev_addr.device_gateway_ip           = add->dev_addr.device_gateway_ip;
	CmListInitialize (&dev->dev_addr.device_alias_list);

	dev->dev_rpc = add->dev_rpc;

	dev->argr = LSA_NULL;

	dev->dev_type = CM_CL_DEV_TYPE_UNKNOWN;

	dev->wf = CM_CL_DEV_NOTHING_TODO;

	CmListInitialize (&dev->event_q);
	CmListInitialize (&dev->alarm_q);

	dev->upper_add_rm = LSA_NULL;
	dev->upper_ctrl   = LSA_NULL;
	dev->upper_apdu   = LSA_NULL;

	dev->rpc_client_id = LSA_NULL;

	if ((add->dev_addr.properties & CM_NARE_MODE_MASK) == CM_NARE_MODE_DCP_CONFIG_IP)
	{
		dev->bindsm.state = CM_CL_BINDSM_STATE_IDENTIFIED;
	}
	else
	{
		dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
	}
	dev->bindsm.wait_for[0] = 0;
	dev->bindsm.response = CM_OK;
	dev->bindsm.schedule_count = 0;
	dev->bindsm.force_nsu = LSA_FALSE;
	dev->bindsm.rpc = LSA_NULL;


	if ((dev->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK) == CM_CL_DEVICE_MODE_IS_FSU_YES)
	{
		(LSA_VOID)cm_cl_fsu_count (channel, 1);
	}

	/* now we are prepared to call cm_cl_dev_free() */

/*----------------------------------------------------------------------------*/

	{
		CM_AR_GRAPH_PTR_TYPE argr;
		CM_ASSERT (cm_is_not_null (arcb));

		argr = cm_cl_ar_from_device_no_elog (channel, add->device_nr);
		if (cm_is_not_null (argr))
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) already exists", argr->device_nr);
			add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (5 /*ARUUID*/);
			cm_cl_dev_free (channel, dev);
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}

		if (arcb->ar_type == CM_AR_TYPE_SINGLE_SYSRED)
		{
			if (! ((1 <= (arcb->ar_uuid.node[5] & 0x07)) && ((arcb->ar_uuid.node[5] & 0x07) <= 4)))
			{
				CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARUUID::Selector bit 0-2 not in range (1-4)");
				add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (5 /*ARUUID*/);
				cm_cl_dev_free (channel, dev);
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}
			else if (((arcb->ar_uuid.node[5] & 0x18) >> 3) != 2)
			{
				CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARUUID::Selector bit 3-4 not 2");
				add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (5 /*ARUUID*/);
				cm_cl_dev_free (channel, dev);
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}
			else if ((arcb->ar_uuid.node[5] & 0xE0) != 0  ||  (arcb->ar_uuid.node[4] != 0))
			{
				CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARUUID::Selector bit 5-7 not zero");
				add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (5 /*ARUUID*/);
				cm_cl_dev_free (channel, dev);
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}
		}
		else if ((add->device_properties & CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM) != 0)
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "device_properties(0x%x) is only allowed for SRD", add->device_properties
				);
			add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict ();
			cm_cl_dev_free (channel, dev);
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}

		argr = cm_cl_ar_from_ar_uuid (channel, &arcb->ar_uuid, LSA_FALSE);
		if (cm_is_not_null (argr))
		{
			CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
				, arcb->ar_uuid.time_low, arcb->ar_uuid.time_mid, arcb->ar_uuid.time_hi_and_version
				, arcb->ar_uuid.clock_seq_hi_and_reserved, arcb->ar_uuid.clock_seq_low
				, arcb->ar_uuid.node[0], arcb->ar_uuid.node[1], arcb->ar_uuid.node[2]
				, arcb->ar_uuid.node[3], arcb->ar_uuid.node[4], arcb->ar_uuid.node[5]
				);
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "ar_uuid already exists within dev(%u)", argr->device_nr
				);
			add->pnio_status = CM_PNIO_FAULTY_ARBlockReq (5 /*ARUUID*/);
			cm_cl_dev_free (channel, dev);
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}

		/* ar_graph alloc */
		add->pnio_status = cm_ar_graph_from_arcb_add (channel, dev, arcb, local_clock, is_reduction_ratio_fixed, &argr);

		if (add->pnio_status != CM_PNIO_ERR_NONE)
		{
			CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "pnio_err(0x%x) on alloc ar_graph, dev(%u) ar(%u)", add->pnio_status, dev->device_nr, arcb->ar_nr
				);
			if (cm_is_not_null (argr))
			{
				cm_ar_graph_free (argr);
			}
			cm_cl_dev_free (channel, dev);
			cm_callback (channel, CM_ERR_PARAM, &rb);
			return;
		}

		argr->session_key = 0;
		argr->cm.cl.dev   = dev;

		dev->argr=argr;

		if (((dev->device_mode & CM_CL_DEVICE_MODE_IS_ACCELERATED_MASK) == CM_CL_DEVICE_MODE_IS_ACCELERATED_YES)
			|| ((dev->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK) == CM_CL_DEVICE_MODE_IS_FSU_YES))
		{
			/* start up statistic */
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&argr->cm.cl.sustat), sizeof (*argr->cm.cl.sustat));
			if (cm_is_null (argr->cm.cl.sustat))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "alloc sustat: dev(%u) ar(%u)", dev->device_nr, arcb->ar_nr
					);
				cm_cl_dev_free (channel, dev);
				cm_callback (channel, CM_ERR_ALLOC_LOCAL, &rb);
				return;
			}
			cm_cl_ar_sustat_init (argr);
		}
		else
		{
			argr->cm.cl.sustat = LSA_NULL;
		}

		mtd = argr->cm.cl.mtd;

		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /*PDev*/);

			if (cm_is_not_null (mtd) || cm_is_not_null (pd_mtd))
			{
				if (cm_is_null (mtd))
				{
					CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PDMasterTailorData missing for dev(%u)", dev->device_nr);
					add->pnio_status = CM_PNIO_FAULTY_Connect__CTLDINA_DCPNoRealStationName ();
				}
				else if (cm_is_null (pd_mtd))
				{
					CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PDMasterTailorData missing for pdev");
					add->pnio_status = CM_PNIO_FAULTY_Connect__CTLDINA_DCPNoRealStationName ();
				}
				else
				{
					add->pnio_status = cm_cl_mp_ar_graph_check_consistency (dev, argr, mtd);
				}

				if (add->pnio_status != CM_PNIO_ERR_NONE)
				{
					cm_cl_dev_free (channel, dev);
					cm_callback (channel, CM_ERR_PARAM, &rb);
					return;
				}
			}
		}
		cm_cl_ar_event_get_abort_ip (argr, &device_ip);
	}

	if (! CmListIsEmpty (&add->dev_addr.device_alias_list))
	{
		CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE alias;

		for (CmListForeach (alias, &add->dev_addr.device_alias_list, CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE))
		{
			LSA_BOOL rc;

			if (cm_is_null (mtd))
			{
				rc = cm_is_valid_alias_name (alias->name_length, alias->name);
			}
			else
			{
				rc = cm_cl_mp_is_valid_alias_name (alias, mtd);
			}

			if (! rc)
			{
				CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "invalid alias_name, alias_name_length(%u)", alias->name_length);
				add->pnio_status = CM_PNIO_FAULTY_Connect__CTLDINA_DCPNoRealStationName ();
				cm_cl_dev_free (channel, dev);
				cm_callback (channel, CM_ERR_PARAM, &rb);
				return;
			}
		}
	}
	/* Boolean argument to function - request 1524437*/
	if (! /*lint -e(730) */ cm_cl_dev_alloc_name_alias (dev, add, cm_is_not_null (mtd)))
	{
		add->pnio_status = CM_PNIO_FAULTY_Connect__CTLDINA_DCPNoRealStationName ();
		cm_cl_dev_free (channel, dev);
		cm_callback (channel, CM_ERR_ALLOC_LOCAL, &rb);
		return;
	}

/*----------------------------------------------------------------------------*/

	add->pnio_status = cm_cl_dev_build_provider_sequence (dev);
	if (add->pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u): pnio_err(0x%x) on build provider sequence", dev->device_nr,
			add->pnio_status);
		cm_cl_dev_free (channel, dev);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

/*----------------------------------------------------------------------------*/

	add->is_master_project = cm_is_not_null (mtd) ? CM_IS_MASTER_PROJECT_YES : CM_IS_MASTER_PROJECT_NO;
	add->device_ip         = device_ip;

	cm_cl_dev_calc_resource_count (dev, &add->nr_of_event_resources, &add->nr_of_alarm_resources);

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u): added, needed resources event_cnt(%u) alarm_cnt(%u)"
		, add->device_nr, add->nr_of_event_resources, add->nr_of_alarm_resources
		);

/*----------------------------------------------------------------------------*/

	CM_RQB_SET_RESPONSE (rb, CM_OK);
	dev->upper_add_rm = rb;

	dev->wf |= CM_CL_DEV__WF_ADD_DONE;
	dev->wf |= CM_CL_DEV__DO_BIND_REQ;
	dev->wf |= CM_CL_DEV__DO_FRAME_ADD_REQ;

	if ((dev->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK) == CM_CL_DEVICE_MODE_IS_FSU_YES)
	{
		dev->wf |= CM_CL_DEV__DO_SET_HELLO_FILTER_REQ;

		if (cm_is_not_null (mtd) && (mtd->properties & CM_PD_MTD_PROP_NOS))
		{
			dev->run_mode |= CM_CL_RMODE__I_HELLO_FILTER_DYNAMIC;

			/* need inital set-hello-filter with first activate */
			dev->wf |= CM_CL_DEV__HELLO_FILTER_NAME_CHANGED;
		}
	}

	cm_cl_device_add_rm_done (channel, dev, 0 /*trigger*/, CM_OK);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_device_add_set_iocr_cnf (
	CM_UPPER_RQB_CONST_PTR_TYPE upper_add,
	CM_CL_DEVICE_CONST_PTR_TYPE dev
	)
{
	if (dev->argr->ar_nr == upper_add->args.cl.dev_add->arcb->ar_nr)
	{
		cm_arcb_device_add_set_iocr_cnf (dev->argr, upper_add->args.cl.dev_add->arcb);
	}
}


/*------------------------------------------------------------------------------
//  Notify pdev if the newly added device belongs to a Master Project
//----------------------------------------------------------------------------*/

static LSA_VOID
cm_cl_device_add_notify_master_project (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CL_DEVICE_CONST_PTR_TYPE dev
	)
{
	if (cm_is_not_null(dev->argr) && cm_is_not_null (dev->argr->cm.cl.mtd))
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = dev->argr->cm.cl.mtd;
		LSA_UINT16 i;

		for (i = 0; i < mtd->port_cnt; i++)
		{
			if (mtd->ports[i].peer_station_nr == 0)
			{
				cm_pdcl_master_tailor_device_added (channel);
				return;
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_device_add_rm_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_CL_DEVICE_PTR_TYPE dev,
	LSA_UINT32 done,
	LSA_UINT16 cm_rsp
	)
{
	LSA_UINT16 rc;

	CM_ASSERT (done == 0 || (dev->wf & done) != 0);

	if (cm_rsp == CM_OK)
	{
		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "cm_device_add_sm: dev(%u) wf(0x%x), done(0x%x) with cm_rsp(%u)"
			, dev->device_nr, dev->wf, done, cm_rsp
			);
	}
	else
	{
		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cm_device_add_sm: dev(%u) wf(0x%x), done(0x%x) with cm_rsp(%u)"
			, dev->device_nr, dev->wf, done, cm_rsp
			);
		if (CM_RQB_GET_RESPONSE (dev->upper_add_rm) == CM_OK)
		{
			CM_RQB_SET_RESPONSE (dev->upper_add_rm, cm_rsp);
		}
	}

	/* confirmation done */

	if (done == CM_CL_DEV__WF_BIND_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_BIND_CNF;

		if (cm_rsp != CM_OK)
		{
			dev->wf &= ~CM_CL_DEV__DO_FRAME_ADD_REQ;
			dev->wf &= ~CM_CL_DEV__DO_SET_HELLO_FILTER_REQ;
		}
	}
	else if (done == CM_CL_DEV__WF_SET_HELLO_FILTER_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_SET_HELLO_FILTER_CNF;

		if (cm_rsp != CM_OK)
		{
			dev->wf &= ~CM_CL_DEV__DO_FRAME_ADD_REQ;
			dev->wf |= CM_CL_DEV__DO_UNBIND_REQ;
		}
	}
	else if (done == CM_CL_DEV__WF_FRAME_ADD_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_FRAME_ADD_CNF;

		if (cm_rsp != CM_OK)
		{
			dev->wf |= CM_CL_DEV__DO_FRAME_REMOVE_REQ; /*AP01337261*/
			dev->wf |= CM_CL_DEV__DO_CLEAR_HELLO_FILTER_REQ;
			dev->wf |= CM_CL_DEV__DO_UNBIND_REQ;
		}
	}
	else if (done == CM_CL_DEV__WF_FRAME_REMOVE_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_FRAME_REMOVE_CNF;
	}
	else if (done == CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF;
	}
	else if (done == CM_CL_DEV__WF_UNBIND_CNF)
	{
		dev->wf &= ~CM_CL_DEV__WF_UNBIND_CNF;
	}
	else
	{
		CM_ASSERT (done == 0);
	}

	/* check wait_for confirmation */

	if ((dev->wf & CM_CL_DEV__WF_BIND_CNF) != 0
		||  (dev->wf & CM_CL_DEV__WF_UNBIND_CNF) != 0
		||  (dev->wf & CM_CL_DEV__WF_SET_HELLO_FILTER_CNF) != 0
		||  (dev->wf & CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF) != 0
		||  (dev->wf & CM_CL_DEV__WF_FRAME_ADD_CNF) != 0
		||  (dev->wf & CM_CL_DEV__WF_FRAME_REMOVE_CNF) != 0)
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "device_add_rm_sm: dev(%u) must wf(0x%x)"
			, dev->device_nr, dev->wf
			);
		return;
	}

	/* do next request */

	if (dev->wf & CM_CL_DEV__DO_BIND_REQ)
	{
		dev->wf &= ~CM_CL_DEV__DO_BIND_REQ;
		dev->wf |= CM_CL_DEV__WF_BIND_CNF;

		rc = cm_cl_rpc_bind (channel, dev);
		if (rc != CM_OK_PENDING)
		{
			cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_BIND_CNF, CM_ERR_ALLOC_LOWER);
		}
		return;
	}
	else if (dev->wf & CM_CL_DEV__DO_SET_HELLO_FILTER_REQ)
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		dev->wf &= ~CM_CL_DEV__DO_SET_HELLO_FILTER_REQ;
		dev->wf |= CM_CL_DEV__WF_SET_HELLO_FILTER_CNF;

		rc = cm_edd_set_dcp_hello_filter (channel, EDD_DCP_HELLO_FILTER_ON
			, CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr), dev->dev_addr.device_name, dev->dev_addr.device_name_length
			);
		if (rc != CM_OK_PENDING)
		{
			cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_SET_HELLO_FILTER_CNF, CM_ERR_ALLOC_LOWER);
		}
		return;
	}
	else if (dev->wf & CM_CL_DEV__DO_FRAME_ADD_REQ)
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		dev->wf &= ~CM_CL_DEV__DO_FRAME_ADD_REQ;
		dev->wf |= CM_CL_DEV__WF_FRAME_ADD_CNF;

		cm_arcq_request (argr, CM_ARCQ_OPC_FRAMES_ADD, 0);
		return;
	}
	else if (dev->wf & CM_CL_DEV__DO_FRAME_REMOVE_REQ)
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		dev->wf &= ~CM_CL_DEV__DO_FRAME_REMOVE_REQ;
		dev->wf |= CM_CL_DEV__WF_FRAME_REMOVE_CNF;

		cm_arcq_request (argr, CM_ARCQ_OPC_FRAMES_REMOVE, 0);
		return;
	}
	else if (dev->wf & CM_CL_DEV__DO_CLEAR_HELLO_FILTER_REQ)
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		dev->wf &= ~CM_CL_DEV__DO_CLEAR_HELLO_FILTER_REQ;
		dev->wf |= CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF;

		rc = cm_edd_set_dcp_hello_filter (channel, EDD_DCP_HELLO_FILTER_OFF
			, CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr), LSA_NULL, 0
			);
		if (rc != CM_OK_PENDING)
		{
			CM_FATAL1 (rc); /* set works, but unset not ... */
		}
		return;
	}
	else if (dev->wf & CM_CL_DEV__DO_UNBIND_REQ)
	{
		dev->wf &= ~CM_CL_DEV__DO_UNBIND_REQ;

		dev->wf |= CM_CL_DEV__WF_UNBIND_CNF;
		rc       = cm_cl_rpc_unbind (channel, dev);
		if (rc != CM_OK_PENDING)
		{
			CM_FATAL1 (rc); /* bind works, but unbind not ... */
		}
		return;
	}

	/* do finish opcode */

	if (dev->wf & CM_CL_DEV__WF_ADD_DONE)
	{
		CM_UPPER_RQB_PTR_TYPE upper_add = dev->upper_add_rm;
		CM_ASSERT (cm_is_not_null (upper_add));
		CM_ASSERT (CM_RQB_GET_OPCODE (upper_add) == CM_OPC_CL_DEVICE_ADD);

		dev->upper_add_rm = LSA_NULL;
		dev->wf          &= ~CM_CL_DEV__WF_ADD_DONE;

		cm_rsp = CM_RQB_GET_RESPONSE (upper_add);
		if (cm_rsp != CM_OK)
		{
			cm_cl_dev_free (channel, dev);
			CM_ASSERT (dev->state == CM_CL_DEV_ST_NOI);

			/* device_add.cnf(-) */
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) not added, cm-rsp(%u)", dev->device_nr, cm_rsp
				);
			cm_callback (channel, cm_rsp, &upper_add);
			return;
		}

		CM_ASSERT (dev->rpc_client_id != CM_RPC_INVALID_CLIENT_ID);

		cm_cl_device_add_set_iocr_cnf (upper_add, dev);

		/* device_add.cnf(+) */
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) added", dev->device_nr);
		cm_callback (channel, CM_OK, &upper_add);

		cm_cl_device_add_notify_master_project (channel, dev);
	}
	else if (dev->wf & CM_CL_DEV__WF_REMOVE_DONE)
	{
		CM_UPPER_RQB_PTR_TYPE upper_rm = dev->upper_add_rm;
		CM_ASSERT (cm_is_not_null (upper_rm));
		CM_ASSERT (CM_RQB_GET_OPCODE (upper_rm) == CM_OPC_CL_DEVICE_REMOVE);

		dev->wf &= ~CM_CL_DEV__IS_RESET_TO_FACTORY;
		dev->wf &= ~CM_CL_DEV__HELLO_FILTER_NAME_CHANGED;

		dev->upper_add_rm = LSA_NULL;
		dev->wf          &= ~CM_CL_DEV__WF_REMOVE_DONE;

		CM_ASSERT (dev->rpc_client_id == CM_RPC_INVALID_CLIENT_ID);

		cm_cl_dev_free (channel, dev);
		CM_ASSERT (dev->state == CM_CL_DEV_ST_NOI);
		CM_ASSERT (dev->wf == 0);

		/* device_remove.cnf(+) */
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) removed", dev->device_nr);
		cm_callback (channel, CM_OK, &upper_rm);
	}
	else
	{
		CM_FATAL1 (dev->wf);
	}
	return;
}


/*----------------------------------------------------------------------------*/
/*lint -e{818} symbol sf Pointer parameter 'Symbol' (Location) could be declared ptr to const request 1524437*/
LSA_VOID
cm_cl_edd_set_dcp_hello_filter_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_rsp,
	EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE sf
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = cm_cl_dev_from_device_nr (channel, CM_CL_U32_GET_DEVICE_NR (sf->UserID));

	LSA_UNUSED_ARG (sf->Mode);

	if (cm_is_null (dev))
	{
		CM_FATAL1 (CM_CL_U32_GET_DEVICE_NR (sf->UserID));
	}
	else if (dev->wf & CM_CL_DEV__WF_HELLO_FILTER_REPLACE)
	{
		CM_AR_GRAPH_PTR_TYPE argr = cm_cl_ar_from_device (channel, dev->device_nr);

		if (cm_is_null (argr) || edd_rsp != EDD_STS_OK)
		{
			CM_FATAL1 (CM_CL_U32_GET_AR_NR (sf->UserID));
		}
		CM_ASSERT (argr->ar_nr == CM_CL_U32_GET_AR_NR (sf->UserID));
		switch (sf->Mode)
		{
		case EDD_DCP_HELLO_FILTER_OFF:
			CM_ASSERT (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_HELLO_FILTER_REPLACE));
			break;

		case EDD_DCP_HELLO_FILTER_ON:

			dev->wf &= ~CM_CL_DEV__WF_HELLO_FILTER_REPLACE;
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_HELLO_FILTER_REPLACE);

			if (! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF))
			{
				cm_cl_arsm (argr);
			}
			break;

		default:
			CM_FATAL1 (sf->Mode);
			break;
		}
	}
	/* device-add -remove */
	else
	{
		switch (sf->Mode)
		{
		case EDD_DCP_HELLO_FILTER_OFF:
			if (edd_rsp != EDD_STS_OK)
			{
				cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF, CM_ERR_LOWER_LAYER); /* TIA 2717970: was CM_FATAL() */
			}
			else
			{
				cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_CLEAR_HELLO_FILTER_CNF, CM_OK);
			}
			break;

		case EDD_DCP_HELLO_FILTER_ON:
			if (edd_rsp == EDD_STS_OK)
			{
				cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_SET_HELLO_FILTER_CNF, CM_OK);
			}
			else
			{
				cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_SET_HELLO_FILTER_CNF, CM_ERR_LOWER_LAYER);
			}
			break;

		default:
			CM_FATAL1 (sf->Mode);
			break;
		}
	}
}

/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_DEVICE_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_device_remove (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_CL_DEVICE_PTR_TYPE dev;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));

	CM_ASSERT (cm_is_not_null (rb->args.void_ptr));

	dev = cm_cl_dev_from_device_nr (channel, rb->args.cl.dev_remove->device_nr);
	if (cm_is_null (dev))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", rb->args.cl.dev_remove->device_nr,
			1 /*rb->args.cl.dev_remove->ar_nr*/);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (dev->state == CM_CL_DEV_ST_NOI)
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%d): wrong state(%d)", dev->device_nr, dev->state);
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	if (cm_is_not_null (dev->upper_add_rm))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "upper is running, opcode(%d)", CM_RQB_GET_OPCODE (dev->upper_add_rm));
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	switch (dev->state)
	{
	case CM_CL_DEV_ST_ACTIVE:
	case CM_CL_DEV_ST_PASSIVE:
		dev->state = CM_CL_DEV_ST_REMOVING;
		break;

	case CM_CL_DEV_ST_REMOVING:
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "removing is running, opcode(%d)", CM_RQB_GET_OPCODE (dev->upper_add_rm));
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;

	case CM_CL_DEV_ST_NOI:
	default:
		CM_FATAL1 (dev->state);
		return;
	}

	CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_WARN, "dev(%d) wf(%x)", dev->device_nr, dev->wf);

	/* start dev-sm */
	dev->upper_add_rm = rb; /* keep */

	dev->wf |= CM_CL_DEV__WF_REMOVE_DONE;
	dev->wf |= CM_CL_DEV__DO_FRAME_REMOVE_REQ;
	dev->wf |= CM_CL_DEV__DO_UNBIND_REQ;
	if ((dev->device_mode & CM_CL_DEVICE_MODE_IS_FSU_MASK) == CM_CL_DEVICE_MODE_IS_FSU_YES)
	{
		dev->wf |= CM_CL_DEV__DO_CLEAR_HELLO_FILTER_REQ;
	}

	cm_cl_bindsm_set_unbound (dev->argr, CM_AR_REASON_RMV, CM_ERROR_CAUSE_IOC_ABORTED);
	cm_cl_bindsm_trigger (dev->argr, 0);
	cm_cl_devsm (dev, 0, LSA_NULL);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_DEVICE_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_device_control (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_CL_DEVICE_PTR_TYPE dev;
	CM_UPPER_CL_DEVICE_CONTROL_PTR_TYPE ctrl;
	LSA_UINT8 ar_state;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));
	CM_ASSERT (cm_is_not_null (rb->args.void_ptr));
	ctrl = rb->args.cl.dev_control;

	dev = cm_cl_dev_from_device_nr (channel, ctrl->device_nr);
	if (cm_is_null (dev))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): not found", ctrl->device_nr
			);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (dev->state == CM_CL_DEV_ST_NOI)
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u): wrong state(%u)"
			, dev->device_nr, dev->state
			);
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	ar_state = cm_cl_dev_ar_state (dev);

	/* locked after 'queueing' device_remove */
	if (cm_is_not_null (dev->upper_add_rm))
	{
		CM_ASSERT (dev->state == CM_CL_DEV_ST_REMOVING);
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u): DeviceRemove is running, cancel DeviceControl", dev->device_nr);
		cm_callback (channel, CM_ERR_SEQUENCE, &rb);
		return;
	}

	CM_ASSERT (dev->state == CM_CL_DEV_ST_ACTIVE || dev->state == CM_CL_DEV_ST_PASSIVE);

	switch (ctrl->control_tag)
	{
	case CM_CL_DEVICE_CONTROL_ACTIVATE:

		if (dev->wf & CM_CL_DEV__IS_RESET_TO_FACTORY)
		{
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u): device is ResetToFactory, only DeviceRemove allowed", dev->device_nr
				);
			cm_callback (channel, CM_ERR_SEQUENCE, &rb);
			return;
		}

		if (ar_state == CM_CL_AR_PASSIVE)
		{
			LSA_UINT16 needed_events, needed_alarms;
			LSA_UINT16 provided_events, provided_alarms;

			cm_cl_dev_calc_resource_count (dev, &needed_events, &needed_alarms);
			CmListLength (&dev->event_q, &provided_events);
			CmListLength (&dev->alarm_q, &provided_alarms);

			if (provided_events < needed_events || provided_alarms < needed_alarms)
			{
				CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u): too few resources provided", dev->device_nr
					);
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
				return;
			}
		}

		if (cm_is_not_null (dev->upper_ctrl))
		{
			CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u): DeviceActivate, a DeviceControl is running, control_tag(%u), cancelled this rqb(%08x)"
				, dev->device_nr
				, dev->upper_ctrl->args.cl.dev_control->control_tag, dev->upper_ctrl
				);
			cm_callback (channel, CM_OK_CANCELLED, &dev->upper_ctrl);
		}
		dev->upper_ctrl = rb;

		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u):  DeviceActivate, ar_state(%u)", dev->device_nr, ar_state
			);

		switch (ar_state)
		{
		case CM_CL_AR_PASSIVE:
			dev->state = CM_CL_DEV_ST_ACTIVE;
			cm_cl_dev_activate (dev);
			cm_callback (channel, CM_OK, &dev->upper_ctrl);
			break;

		case CM_CL_AR_WF_IN:
		case CM_CL_AR_INDATA:
			CM_ASSERT (dev->state == CM_CL_DEV_ST_ACTIVE);
			cm_callback (channel, CM_OK, &dev->upper_ctrl);
			break;

		case CM_CL_AR_WF_DOWN:
			dev->state = CM_CL_DEV_ST_ACTIVE;
			/* activation is done with cm_cl_devsm() */
			cm_callback (channel, CM_OK, &dev->upper_ctrl);
			break;

		default:
			CM_ASSERT (0);
			break;
		}
		break;

	case CM_CL_DEVICE_CONTROL_PASSIVATE:
		if (cm_is_not_null (dev->upper_ctrl))
		{
			CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u): DevicePassivate, a DeviceControl is running, control_tag(%u), cancelled this rqb(%08x)"
				, dev->device_nr, dev->upper_ctrl->args.cl.dev_control->control_tag
				, dev->upper_ctrl
				);
			cm_callback (channel, CM_OK_CANCELLED, &dev->upper_ctrl);
		}
		dev->upper_ctrl = rb;

		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u):  DevicePassivate, ar_state(%u)", dev->device_nr, ar_state
			);

		switch (ar_state)
		{
		case CM_CL_AR_PASSIVE:
			dev->state = CM_CL_DEV_ST_PASSIVE;
			cm_cl_devsm (dev, 0, LSA_NULL);
			/* => cm_cl_dev_offline_ind() */
			break;

		case CM_CL_AR_WF_IN:
		case CM_CL_AR_INDATA:
			dev->state = CM_CL_DEV_ST_PASSIVE;
			cm_cl_devsm (dev, 0, LSA_NULL);
			/* => cm_cl_dev_offline_ind() */
			break;

		case CM_CL_AR_WF_DOWN:
			dev->state = CM_CL_DEV_ST_PASSIVE;
			/* keep */
			/* => cm_cl_dev_offline_ind() */
			break;

		default:
			CM_ASSERT (0);
			break;
		}
		break;

#if 1//SRD
	case CM_CL_DEVICE_CONTROL_APDU_STATUS:
		{
			LSA_UINT16 acp_state;

			if ((ctrl->apdu_status & CM_CL_AR_CONTROL_STATION_OK) && (ctrl->apdu_status & CM_CL_AR_CONTROL_STATION_PROBLEM))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u): DeviceControl: using wrong apdu_status(0x%x)"
					, dev->device_nr, ctrl->apdu_status
					);
				cm_callback (channel, CM_ERR_PARAM, &rb);
				break;
			}

			if (ar_state != CM_CL_AR_INDATA)
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u): no IN_DATA, dev_ar_state(%d)"
					, dev->device_nr, cm_cl_dev_ar_state (dev)
					);
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
				break;
			}

			if (cm_is_not_null (dev->upper_apdu))
			{
				CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u): DeviceSetDataStatus is running, rqb(%08x), apdu-status(%d)"
					, dev->device_nr, dev->upper_apdu, dev->upper_apdu->args.cl.dev_control->apdu_status
					);
				cm_callback (channel, CM_ERR_SEQUENCE, &rb);
				return;
			}
			dev->upper_apdu = rb; /* keep */

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("check: store apdu for internal restart?")
#endif /* CM_MESSAGE */

			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u): DeviceSetDataStatus apdu-status(0x%x)", dev->device_nr, ctrl->apdu_status
				);

			acp_state = 0;

			if (ctrl->apdu_status & CM_CL_AR_CONTROL_STATION_OK)
			{
				acp_state |= CM_ARCQ_PROVIDER_SET_STATION_OK;
			}
			else if (ctrl->apdu_status & CM_CL_AR_CONTROL_STATION_PROBLEM)
			{
				acp_state |= CM_ARCQ_PROVIDER_SET_STATION_FAILURE;
			}

			cm_cl_ar_com_apdu_status_control (dev->argr, acp_state);
		}
		break;
#else
#endif

	case CM_CL_DEVICE_CONTROL_ABORT:
	case CM_CL_DEVICE_CONTROL_ABORT_MTOT:

		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): DeviceAbort (%u)", dev->device_nr
			, ctrl->control_tag
			);

		switch (ar_state)
		{
		case CM_CL_AR_PASSIVE:
		case CM_CL_AR_WF_DOWN:
			cm_callback (channel, CM_OK, &rb);
			break;

		case CM_CL_AR_WF_IN:
		case CM_CL_AR_INDATA:
			cm_callback (channel, CM_OK, &rb);

			if (ctrl->control_tag ==  CM_CL_DEVICE_CONTROL_ABORT)
			{
				cm_cl_dev_abort (dev, CM_AR_REASON_ABORT);
			}
			else
			{
				cm_cl_dev_abort (dev, CM_AR_REASON_MTOT);
			}
			break;

		default:
			CM_ASSERT (0);
			break;
		}
		break;

	default:
		/* or param error */
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!control_tag(0x%x)", ctrl->control_tag);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		break;
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
