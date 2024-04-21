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
/*  F i l e               &F: cm_cl32.c                                 :F&  */
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
/*  CM_OPC_CL_PRM_UPDATE                                                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	632
#define CM_MODULE_ID		632

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_prm_begin (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  alloc_len;
	LSA_UINT16  cm_dp_assert_line = 0;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	LSA_UINT16  sublist_cnt = 0;

	switch (argr->cm.cl.prm.prm_seq)
	{
	case CM_CL_PRMMODE_COC:
		sublist_cnt = 1;
		break;
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		CmListLength (& argr->cm.cl.prm.prm_submod_list, & sublist_cnt);
		break;
	default:
		CM_FATAL (); /* a bug */
		break;
	}

		/* req: (ControlBlockConnect, SubmoduleListBlock) */
	alloc_len = CM_CAST_U16((CLRPC_PNIO_OFFSET
		+ CM_BLK_CTRL_SIZE
		+ CM_BLK_SUBMOD1_SIZE + sublist_cnt * CM_BLK_SUBMOD2_SIZE))
		;

	{	/* cnf: (ControlBlockConnect, [ModuleDiffBlock]) */
		LSA_UINT16  alloc_cnf = CM_CAST_U16((CLRPC_PNIO_OFFSET
			+ CM_BLK_CTRL_SIZE
			/* worst case (each submodule in another API and module) */
			+ CM_RSP_MDB_SIZE + sublist_cnt * (CM_RSP_API_SIZE + CM_RSP_MOD_SIZE + CM_RSP_SUB_SIZE)))
			;
		if (alloc_cnf > alloc_len)
		{
			alloc_len = alloc_cnf;
		}
	}

	rpc = cm_rpc_alloc_call_rqb (channel, alloc_len);
	if (cm_is_null (rpc))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "dev(%u): no mem, do down, prm_mode(%u)", argr->device_nr, cm_cl_prm_mode (argr));

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);

		return;
	}

	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONTROL;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->ndr_len			= 0;
	rpc->args.cl.call->ndr_data[0]		= 0;

		/* marshal */
	{
		CM_RPC_MEM_U8_PTR_TYPE  dp;
		LSA_UINT32  left;

		dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		left = alloc_len - CLRPC_PNIO_OFFSET;

		cm_dp_assert (left >= CM_BLK_CTRL_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_PRM_BEGIN_REQ);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	CM_BLK_CTRL_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

		CM_PUT16_HTON(dp,    CM_BLK_CTRL_reserved1, 0);
		CM_PUT_UUID_HTON(dp, CM_BLK_CTRL_ar_uuid, & argr->ar.ar_uuid);
		CM_PUT16_HTON(dp,    CM_BLK_CTRL_session_key, argr->cm.cl.connect_counter);
		CM_PUT16_HTON(dp,    CM_BLK_CTRL_reserved2_alarm_sequence_number, 0);
		CM_PUT16_HTON(dp,    CM_BLK_CTRL_control_command, CM_CONTROL_COMMAND_PRM_BEGIN);
		CM_PUT16_HTON(dp,    CM_BLK_CTRL_control_properties, 0);	/* fixed data */

		cm_dp_adjust (dp, left, CM_BLK_CTRL_SIZE);

		cm_dp_assert (left >= CM_BLK_SUBMOD1_SIZE);

		CM_PUT16_HTON (dp, CM_BLK_HEADER_type,		CM_BLOCK_TYPE_SUBMODULE_LIST_REQ);
		CM_PUT16_HTON (dp, CM_BLK_HEADER_length,	(LSA_UINT16)(CM_BLK_SUBMOD1_SIZE - CM_BLK_HEADER_TypLen_SIZE + sublist_cnt * CM_BLK_SUBMOD2_SIZE));
		CM_PUT16_HTON (dp, CM_BLK_HEADER_version,	0x0100);

		CM_PUT16_HTON (dp, CM_BLK_SUBMOD1_number_of_entries, sublist_cnt);

		cm_dp_adjust (dp, left, CM_BLK_SUBMOD1_SIZE);

		if (sublist_cnt > 0)
		{
			CM_LIST_ENTRY_PTR_TYPE  inner_sub;
			LSA_UINT32  api;
			LSA_UINT16  slot_nr;
			LSA_UINT16  subslot_nr;

			for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
			{
				sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

				if (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_COC)
				{
					if ((sub->cl.sub_coc_properties & CM_SUB_COC_IS_ACCESS_POINT) == 0)
					{
						continue;
					}
				}

				api        = sub->mod->ap->api;
				slot_nr    = sub->mod->slot_nr;
				subslot_nr = sub->subslot_nr;
				{
					cm_dp_assert (left >= CM_BLK_SUBMOD2_SIZE);

					CM_PUT32_HTON (dp, CM_BLK_SUBMOD2_api,        api);
					CM_PUT16_HTON (dp, CM_BLK_SUBMOD2_slot_nr,    slot_nr);
					CM_PUT16_HTON (dp, CM_BLK_SUBMOD2_subslot_nr, subslot_nr);

					cm_dp_adjust (dp, left, CM_BLK_SUBMOD2_SIZE);
				}
			}
		}

		/* left > 0: space for cnf */

		rpc->args.cl.call->ndr_len = CM_CAST_U16(dp - rpc->args.cl.call->ndr_data);
	}

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
	return;

cm_dp_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_dp_assert_line
		);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	cm_rpc_free_call_rqb (channel, rpc);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_prm_end (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	LSA_UINT16  prm_seq = cm_cl_prm_mode (argr);

	switch (prm_seq)
	{
	case CM_CL_PRMMODE_CONNECT:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_PLUG:
		{
		LSA_UINT16  alarm_seq_nr = argr->cm.cl.prm_alarm->args.cl.alarm->alarm_sequence;

		cm_cl_rpc_control (channel, argr, LSA_TRUE, alarm_seq_nr);
		}
		break;

	case CM_CL_PRMMODE_AR:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_SUBMODULE:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	case CM_CL_PRMMODE_COC:
		cm_cl_rpc_control (channel, argr, LSA_FALSE, 0);
		break;

	default:
		CM_FATAL();
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
