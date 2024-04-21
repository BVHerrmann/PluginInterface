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
/*  F i l e               &F: cm_cl13.c                                 :F&  */
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
/*  CM_OPC_CL_LOGBOOK_READ                                                   */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   613
#define CM_MODULE_ID        613

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_LOGBOOK_READ
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_logbook_read (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE lbr;
	LSA_UINT16 rc;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));

	lbr = rb->args.cl.logbook_read;

	if (lbr->entry_count > 0  &&  cm_is_null (lbr->entry))
	{
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (! (1 <= lbr->device_nr && lbr->device_nr <= channel->usr.cl.dev_cnt))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%d) wrong: valid(1..%d)", lbr->device_nr, channel->usr.cl.dev_cnt
			);
		cm_callback (channel, CM_ERR_PARAM, &rb);
		return;
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: logbook_read")
#endif /* CM_MESSAGE */

	CM_GET_CYCLE_COUNTER (&rc, channel->sysptr, &lbr->curr_time_stamp);
	lbr->entry_count = 0;

	if (rc == LSA_RET_OK)
	{
		cm_callback (channel, CM_OK, &rb);
	}
	else
	{
		cm_callback (channel, CM_ERR_LOWER_LAYER, &rb);
	}
}


/*------------------------------------------------------------------------------
// bindsm - handle orchestration of nare and rebind
//----------------------------------------------------------------------------*/

#define CM_CL_BINDSM_DO_IDENTIFY      0x0010	/* trigger DCP identify */
#define CM_CL_BINDSM_DO_SET           0x0020	/* trigger DCP set */
#define CM_CL_BINDSM_DO_TEST_IP       0x0040	/* trigger ARP, multiple IP usage test for NSU */
#define CM_CL_BINDSM_DO_REBIND        0x0080	/* run RPC rebind */
#define CM_CL_BINDSM_DO_TEST_IP_OSU   0x0100	/* trigger ARP, multiple IP usage test for ASU or FSU */
#define CM_CL_BINDSM_DO_IDENTIFY_FSU  0x0200	/* trigger DCP identify for FSU, multiple NoS */
#define CM_CL_BINDSM_DO_CANCEL_Q      0x0400	/* cancel pending reads */

static LSA_VOID
cm_cl_bindsm_cancel_queue (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;
	CM_UPPER_RQB_PTR_TYPE upper, next;

	upper = CmListFirst (&argr->cm.cl.record_list, CM_UPPER_RQB_PTR_TYPE);
	if (cm_is_null (upper))
	{
		dev->bindsm.schedule_count = 0;
	}
	else
	{
		dev->bindsm.schedule_count++;
	}

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
		, "bindsm_cancel_queue: dev(%u) rsp(%u/0x%x) schedule_count(%u)"
		, dev->device_nr, dev->bindsm.response, dev->bindsm.response, dev->bindsm.schedule_count
	);

	for (; cm_is_not_null (upper); upper = next)
	{
		next = CmListNext (&argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE) upper, CM_UPPER_RQB_PTR_TYPE);

		if ((CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ || CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY)
			&& dev->bindsm.response != CM_OK_CANCELLED /* cancel all on device remove */
			&& dev->bindsm.schedule_count <= 1 /* keep first queued read for one AR schedule cycle to skip temporary failures */
			&& upper->args.cl.read->rec.record_index >= 0xF000
			)
		{
			/* keep queued reads for index >= 0xF000 */
		}
		else
		{
			dev->bindsm.schedule_count = 0;

			CmListRemoveEntry ((CM_LIST_ENTRY_PTR_TYPE)upper);
			cm_cl_ar_callback (argr, dev->bindsm.response, upper);
		}
	}

	dev->bindsm.response = CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_startup (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "bindsm_startup: dev(%u) state(%u) ar_state(%u)"
		, dev->device_nr, dev->bindsm.state, argr->cm.cl.state
		);

	dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;

	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU);

	CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY);
	CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP);

	cm_cl_bindsm_trigger (argr, 0);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_startup_optimized (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_fsu
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "bindsm_startup_optimized: dev(%u) wf(0x%x) is_fsu(%u) ar_state(%u) wf0(0x%x)"
		, dev->device_nr, dev->bindsm.wait_for[0], is_fsu, argr->cm.cl.state, argr->cm.cl.wait_for[0]
		);

	dev->bindsm.state = CM_CL_BINDSM_STATE_IDENTIFIED;

	if (! CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU))
	{
		CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP);

		CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU);
		CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_REBIND);

		if (is_fsu)
		{
			CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU);
		}

		cm_cl_bindsm_trigger (argr, 0);
	}
	else if ( ! is_fsu)  /* TIA 3227491, late DCP_Identify.ind or DCP_Hello.req */
	{
		CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_read (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	LSA_UINT32 list_length;
	CmListLength (&argr->cm.cl.record_list, &list_length);

	if (dev->bindsm.state == CM_CL_BINDSM_STATE_IDENTIFIED)
	{
		if (list_length == 1)
		{
			cm_cl_prm_trigger (argr, 0);
		}
	}
	else if (list_length > 0
			 && dev->state == CM_CL_DEV_ST_PASSIVE
			 && ! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF)
			 && ! CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY))
	{
		CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY);
		cm_cl_bindsm_trigger (argr, 0);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_set_unbound (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT8 reason_code,
	LSA_UINT16 error_cause
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;
	LSA_UINT32 list_length;
	LSA_UINT16 rsp = CM_ERR_ABORT;

	CmListLength (&argr->cm.cl.record_list, &list_length);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "bindsm_set_unbound: dev(%u) state(%u) queue_length(%u) reason_code(%u) error_cause(%u)"
		, argr->device_nr, dev->bindsm.state, list_length, reason_code, error_cause);

	/* set identified state to unknown and return codes other than CM_ERR_ABORT */
	switch (reason_code)
	{
	case CM_AR_REASON_FRAME:
	case CM_AR_REASON_MISS:
	case CM_AR_REASON_ALARM:
	case CM_AR_REASON_ALSND:
	case CM_AR_REASON_ALACK:
	case CM_AR_REASON_ALLEN:
	case CM_AR_REASON_ASRT:
	case CM_AR_REASON_PAS:
	case CM_AR_REASON_PROT:
	case CM_AR_REASON_IRDATA:
	case CM_AR_REASON_PDEV:
	case CM_AR_REASON_LNK_MODE:
	case CM_AR_REASON_ARDY:
	case CM_AR_REASON_MTOT:
		break; /* queued reads are triggered prior to starting the AR */

	case CM_AR_REASON_RMV:
		dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
		rsp               = CM_OK_CANCELLED;
		break;

	case CM_AR_REASON_MEM:
	case CM_AR_REASON_NARE:
	case CM_AR_REASON_LNK_DOWN:
		dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
		rsp               = CM_ERR_LOWER_NARE;
		break;

	case CM_AR_REASON_ABORT:
		if (error_cause == CM_ERROR_CAUSE_IOC_ABORTED || error_cause == CM_ERROR_CAUSE_MP_IN_PROGRESS || error_cause == CM_ERROR_CAUSE_SRD_ABORTED)
		{
			/* user aborted */
		}
		else
		{
			dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
		}
		break;

	case CM_AR_REASON_CONNECT:
	case CM_AR_REASON_READ:
	case CM_AR_REASON_WRITE:
	case CM_AR_REASON_CONTROL:
		if (error_cause == CM_ERROR_CAUSE_RPC_TIMEOUT)
		{
			/* RPC timeout requires rebind */
			dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
			rsp               = CM_ERR_TIMEOUT;
		}
		else
		{
			/* other RPC errors do not require a rebind */
			rsp = CM_ERR_LOWER_RPC;
		}
		break;

	case CM_AR_REASON_IPSUITE:
		dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
		if (error_cause == CM_ERROR_CAUSE_IOC_ABORTED)
		{
			/* user has not set a valid IP suite for the controller */
			rsp = CM_ERR_SEQUENCE;
		}
		break;

	case CM_AR_REASON_DCP_NAME:
	case CM_AR_REASON_DCP_RESET:
	default:
		dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
		break;
	}

	if (dev->bindsm.response == CM_OK)	/* only first abort reason evaluated */
	{
		CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_CANCEL_Q);
		dev->bindsm.response = rsp;
	}
	else if (rsp == CM_OK_CANCELLED)
	{
		dev->bindsm.response = CM_OK_CANCELLED;
	}

	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_SET);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_REBIND);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU);
	CM_CL_WF_CLEAR_IF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU);
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_bindsm_is_identified (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	if (argr->cm.cl.dev->bindsm.state == CM_CL_BINDSM_STATE_IDENTIFIED)
	{
		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_nare_set (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 mode
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "bindsm_nare_set: dev(%u) mode(%u)"
		, argr->device_nr, mode);

	CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_SET);
	dev->bindsm.nare_mode = mode;

	cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_nare_error (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_CL_DOWN_INFO_CONST_PTR_TYPE dinfo
	)
{
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "bindsm_nare_error: dev(%u) error_cause(%u)"
		, argr->device_nr, (cm_is_null (dinfo) ? 0 : dinfo->error_cause)
		);

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);

	cm_cl_arsm_down (argr, dinfo);

	cm_cl_bindsm_trigger (argr, 0);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_bindsm_trigger (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 done
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;
	LSA_UINT32 list_length;
	CmListLength (&argr->cm.cl.record_list, &list_length);

	CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "[bindsm_trigger: dev(%u) state(%u) wf(0x%x) done(0x%x) wf0(0x%x) queue_length(%u) dev_state(%u) ar_state(%u) schedule_count(%u)"
		, argr->device_nr, dev->bindsm.state, dev->bindsm.wait_for[0]
		, done, argr->cm.cl.wait_for[0]
		, list_length, dev->state, argr->cm.cl.state, dev->bindsm.schedule_count
		);

	switch (done)
	{
	case 0:
		/* trigger */
		break;

	case CM_CL_WF_NARE_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
		if (dev->state == CM_CL_DEV_ST_REMOVING)
		{
			/* removing, late */
		}
		else if (argr->cm.cl.state != CM_DST_NARE && argr->cm.cl.state != CM_DST_PASSIVE)
		{
			/* late */
		}
		else if (dev->bindsm.state == CM_CL_BINDSM_STATE_UNKNOWN)
		{
			/* rebind only once for each identify */
			dev->bindsm.state = CM_CL_BINDSM_STATE_IDENTIFIED;
			CM_CL_WF_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_REBIND);
		}
		break;

	case CM_CL_WF_REBIND_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF);
		break;

	default:
		CM_FATAL ();
	}

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF) || CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "]bindsm_trigger: dev(%u) pending rpc_record.cnf"
			, dev->device_nr
			);
		return;
	}
	else
	{
		if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_CANCEL_Q))
		{
			CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_CANCEL_Q);
			cm_cl_bindsm_cancel_queue (argr);
		}
	}

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "]bindsm_trigger: dev(%u) pending rebind.cnf"
			, dev->device_nr
			);
		return;
	}

	if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU) || CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU))
	{
		/* OSU */
		CM_ASSERT (! CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP));
	}
	else if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "]bindsm_trigger: dev(%u) pending nare.cnf"
			, dev->device_nr
			);
		return;
	}

	switch (dev->bindsm.state)
	{
	case CM_CL_BINDSM_STATE_UNKNOWN:
		if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY))
		{
			CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
			cm_cl_nare_ident (argr);
			break;
		}

		switch (argr->cm.cl.state)
		{
		case CM_DST_PASSIVE:
			switch (dev->state)
			{
			case CM_CL_DEV_ST_REMOVING:
				cm_cl_devsm (dev, 0, LSA_NULL);
				break;

			case CM_CL_DEV_ST_PASSIVE:
				cm_cl_bindsm_read (argr);
				break;

			case CM_CL_DEV_ST_ACTIVE:
				/* do nothing, wait for scheduler */
				break;

			default:
				CM_FATAL1 (dev->state);
				break;
			}
			break;

		case CM_DST_WF_DOWN:
			cm_cl_arsm (argr);
			break;

		default:
			break;
		}

		break;

	case CM_CL_BINDSM_STATE_IDENTIFIED:
		if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_SET))
		{
			CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_SET);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
			cm_cl_nare_set (argr);
			break;
		}

		if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_REBIND))
		{
			CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_REBIND);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF);
			cm_cl_rpc_rebind (argr);
			break;
		}

		if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP))
		{
			CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
			cm_cl_nare_test_ip (argr);
			break;
		}

		if (! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF))
		{
			if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU))
			{
				CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_TEST_IP_OSU);

				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
				cm_cl_nare_test_ip (argr);
			}
			else if (CM_CL_WF_IS_SET (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU))
			{
				CM_CL_WF_CLEAR (dev->bindsm.wait_for, CM_CL_BINDSM_DO_IDENTIFY_FSU);

				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF);
				cm_cl_nare_ident (argr);
			}
		}

		switch (argr->cm.cl.state)
		{
		case CM_DST_PASSIVE:
		case CM_DST_COM_ACTIVATE:
		case CM_DST_PRM:
		case CM_DST_WF_APPL_READY_IND:
		case CM_DST_WF_NEW_DATA:
		case CM_DST_IN_DATA:
			if (list_length > 0)
			{
				cm_cl_prm_trigger (argr, 0);
			}
			break;

		case CM_DST_NARE:
		case CM_DST_WF_DOWN:
			cm_cl_arsm (argr);
			break;

		case CM_DST_WF_SCHEDULE:
		case CM_DST_CONNECT:
		case CM_DST_COM_START:
		default:
			break;
		}

		break;

	default:
		CM_FATAL1 (dev->bindsm.state);
	}

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "]bindsm_trigger: dev(%u) state(%u) wf(0x%x) done(0x%x) wf0(0x%x)"
		, argr->device_nr, dev->bindsm.state, dev->bindsm.wait_for[0]
		, done, argr->cm.cl.wait_for[0]
		);
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
