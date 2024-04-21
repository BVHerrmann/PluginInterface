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
/*  F i l e               &F: cm_cl12.c                                 :F&  */
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
/*  CM_OPC_CL_AR_PRM_BEGIN_RSP                                               */
/*  CM_OPC_CL_AR_PRM_INDATA_RSP                                              */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	612
#define CM_MODULE_ID		612

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*----------------------------------------------------------------------------*/
static  LSA_BOOL
cm_cl_prm_update_record_list_prealloc (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update
	)
{
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;

	for (CmListForeach (prm_submod, & prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		CM_UPPER_ARCB_RECORD_PTR_TYPE  arcb_rec;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

		/* argument check */

		cm_ar_graph_find_api_slot_subslot (argr
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			, LSA_NULL, LSA_NULL, & sub
			);

		CM_ASSERT (cm_is_not_null (sub));

		CM_ASSERT (CmListIsEmpty (& sub->cl.prm_update_record_list));

		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PrmUpdate: dev(%u)  Request:  prm_mode(0x%x)  submodule(%u/%u/%u)"
			, argr->device_nr, prm_update->prm_mode
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			);

		for (CmListForeach (arcb_rec, & prm_submod->prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
		{
			CM_PD_RECORD_PTR_TYPE  rec;
			LSA_UINT16  block_field;

			block_field = cm_cl_list_record_append (argr, & sub->cl.prm_update_record_list
				, sub->mod->ap->api, sub->mod->slot_nr, sub->subslot_nr
				, arcb_rec->record_index, arcb_rec->transfer_sequence, LSA_FALSE, LSA_FALSE, arcb_rec->record_data_len, arcb_rec->record_data
				, & rec
				);

			if (block_field != 0) /* only 'no mem' */
			{
				return LSA_FALSE;
			}
		}
	}

	return LSA_TRUE;
}


LSA_VOID
cm_cl_prm_update_record_list_free (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update
	)
{
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;

	for (CmListForeach (prm_submod, & prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

		cm_ar_graph_find_api_slot_subslot (argr
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			, LSA_NULL, LSA_NULL, & sub
			);

		CM_ASSERT (cm_is_not_null (sub));

		cm_ar_graph_free_list_local_mem (& sub->cl.prm_update_record_list);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_update_record_list_activate (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update
	)
{
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;

	for (CmListForeach (prm_submod, & prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

		cm_ar_graph_find_api_slot_subslot (argr
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			, LSA_NULL, LSA_NULL, & sub
			);

		CM_ASSERT (cm_is_not_null (sub));

		cm_ar_graph_free_list_local_mem (& sub->cl.prm_record_list);

		CmListAppend (& sub->cl.prm_record_list, & sub->cl.prm_update_record_list);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_PRM_UPDATE
//----------------------------------------------------------------------------*/
static CM_AR_GRAPH_SUBMODULE_PTR_TYPE
cm_cl_prm_update_find_submodule (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_SUBMODULE_CONST_PTR_TYPE  prm_submod
	)
{
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = LSA_NULL;
	CM_AR_GRAPH_AP_PTR_TYPE  ap = LSA_NULL;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod = LSA_NULL;

	cm_ar_graph_find_api_slot_subslot (argr
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			, & ap, & mod, & sub
			);

	if (cm_is_null (sub))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) submodule(%u/%u/%u) not found"
			, argr->device_nr
			, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
			);
		return LSA_NULL;
	}

	return sub;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_cl_prm_update_check (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update
	)
{

	/*
	* Die adressierte AR muss eine COC Gruppe haben
* Alle übergebenen Submodule müssen zur selben COC-Gruppe gehören
* Die Submodulemenge von AR und PrmUpdate  muss identisch sein
* Jedes Submodule enthält eine Liste der neuen PrmRecord
* Eine leere Liste heißt ablöschen während der PrmPhase
* Eine nicht-leere Liste heißt ersetzen während der PrmPhase
*/

	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;
	CM_AR_GRAPH_AP_PTR_TYPE ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	LSA_UINT16  submod_count = 0;
	LSA_UINT16  coc_group   = 0;
	LSA_UINT16  coc_group_count;

	if (argr->ar.ar_type != CM_AR_TYPE_SINGLE_SYSRED)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) only allowed with CM_AR_TYPE_SINGLE_SYSRED", argr->device_nr
			);
		return CM_ERR_PARAM;
	}

	switch (prm_update->prm_mode)
	{
	case CM_CL_PRMMODE_AR:
		return CM_OK;

	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		break;

	default:
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "PrmUpdate: wrong prm_mode(0x%x)", prm_update->prm_mode);
		return CM_ERR_PARAM;
	}


	for (CmListForeach (prm_submod, &prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		LSA_UINT16  local_coc_group;

		sub = cm_cl_prm_update_find_submodule (argr, prm_submod);

		if (cm_is_null (sub))
		{
			return CM_ERR_PARAM;
		}

		local_coc_group = sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK;

		if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE)
		{
			if (submod_count != 0)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PrmUpdate:Submodule: dev(%u) pdev-submodule(%u/%u/%u), only one submodule allowed"
					, argr->device_nr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					);
				return CM_ERR_PARAM;
			}

			if (CM_IS_PDEV_SUBSLOT_NR(sub->subslot_nr))
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PrmUpdate:Submodule: dev(%u) pdev-submodule(%u/%u/%u), pdev not allowed"
					, argr->device_nr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					);
				return CM_ERR_PARAM;
			}

			if (local_coc_group != 0)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PrmUpdate:Submodule: dev(%u) pdev-submodule(%u/%u/%u), coc not allowed"
					, argr->device_nr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					);
				return CM_ERR_PARAM;
			}
		}
		else if (prm_update->prm_mode == CM_CL_PRMMODE_COC)
		{
			if (!CM_IS_PDEV_SUBSLOT_NR(sub->subslot_nr))
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PrmUpdate:COC: dev(%u) submodule(%u/%u/%u) is no pdev submodule"
					, argr->device_nr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
				);
				return CM_ERR_PARAM;
			}

			if (local_coc_group == 0)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PrmUpdate:COC: dev(%u) pdev-submodule(%u/%u/%u), only coc groups allowed"
					, argr->device_nr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					);
				return CM_ERR_PARAM;
			}

			if (coc_group != 0)
			{
				if (coc_group != local_coc_group)
				{
					CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "PrmUpdate:COC: dev(%u) submodule(%u/%u/%u), only a single coc_group(%u/%u) allowed"
						, argr->device_nr
						, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
						, (sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK), coc_group
						);
					return CM_ERR_PARAM;
				}
			}
			else
			{
				coc_group = local_coc_group;
			}
		}

		{
			CM_UPPER_ARCB_RECORD_PTR_TYPE  arcb_rec;

			for (CmListForeach (arcb_rec, & prm_submod->prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
			{
				if (arcb_rec->record_index >  0xFFFF
				||  arcb_rec->record_index == 0xE040 /* WriteMultiple */
				||	arcb_rec->record_index == 0x80B0 /* CombinedObjectContainer */)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "PrmUpdate: dev(%u) index(0x%x) not allowed"
						, argr->device_nr, arcb_rec->record_index
						);
					return CM_ERR_PARAM;
				}
			}
		}

		submod_count++;
	}

	if (submod_count == 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) no submodules passed"
			, argr->device_nr
		);
		return CM_ERR_PARAM;
	}

	if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE)
	{
		return CM_OK;
	}

	if (coc_group == 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) passed submodules are not members of a coc group"
			, argr->device_nr
		);
		return CM_ERR_PARAM;
	}

	coc_group_count = 0;
	for (CmListForeach (ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			for (CmListForeach (sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				if ((sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) == coc_group)
				{
					coc_group_count++;
				}
			}
		}
	}

	if (submod_count != coc_group_count)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) passed nr(%u) of submodules != nr(%u) of submodules in ar"
			, argr->device_nr
			, submod_count, coc_group_count
		);
		return CM_ERR_PARAM;
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_prm_update (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update = rb->args.cl.prm_update;
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16  rsp;

	if (channel->state != CM_CL_READY)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PrmUpdate: wrong channel.state(%u)", channel->state);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

		/* param check */

	argr = cm_cl_ar_from_device (channel, prm_update->device_nr);
	if (cm_is_null (argr))
	{
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}


	rsp = cm_cl_prm_update_check (argr, prm_update);
	if (rsp != CM_OK)
	{
		cm_callback (channel, rsp, & rb);
		return;
	}

	if (prm_update->prm_mode == CM_CL_PRMMODE_AR  &&  ! CmListIsEmpty (&prm_update->prm_submod_list))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) prm_submod_list must be empty"
			, argr->device_nr
			);
		cm_callback(channel, CM_ERR_PARAM, &rb);
		return;
	}

	if (prm_update->prm_mode == CM_CL_PRMMODE_COC  &&  cm_is_not_null (argr->cm.cl.mtd))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) PrmUpdate and PDMasterTailorData not allowed"
			, argr->device_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (! cm_cl_prm_request_can_queue (argr, prm_update->prm_mode))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PrmUpdate: dev(%u) a previous PrmUpdate exist, arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE  ||  prm_update->prm_mode == CM_CL_PRMMODE_COC)
	{
		if (! cm_cl_prm_update_record_list_prealloc (argr, prm_update))
		{
			CM_CL_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PrmUpdate:  dev(%u) no mem for prm-record-update-list"
				, argr->device_nr
				);
			cm_cl_prm_update_record_list_free (argr, prm_update);
			cm_callback (channel, CM_ERR_RESOURCE, & rb);
			return;
		}
	}

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "PrmUpdate: dev(%u)  Request:prm_mode(0x%x)  and current:prm_mode(%u)"
		, argr->device_nr, prm_update->prm_mode, cm_cl_prm_mode (argr)
		);

		/* kick state machine */
	switch (argr->cm.cl.state) {
	case CM_DST_PASSIVE:
	case CM_DST_WF_DOWN:

	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:

			/* only update submodule-rec-list */
		if (prm_update->prm_mode == CM_CL_PRMMODE_SUBMODULE ||  prm_update->prm_mode == CM_CL_PRMMODE_COC)
		{
			cm_cl_prm_update_record_list_activate (argr, prm_update);
		}

		cm_callback (channel, CM_OK, & rb);
		break;

	case CM_DST_PRM:
	case CM_DST_WF_APPL_READY_IND:
	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:

		cm_cl_prm_request (argr, prm_update->prm_mode/*CM_CL_PRMMODE_AR or CM_CL_PRMMODE_SUBMODULE or CM_CL_PRMMODE_COC*/, rb);
		break;

	default:
		CM_FATAL1(argr->cm.cl.state);
		break;
	}
}


/*------------------------------------------------------------------------------
//	handle opcode CM_OPC_CL_AR_PRM_BEGIN_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_prm_begin_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event = rb->args.cl.event_ind;
	CM_AR_GRAPH_PTR_TYPE  argr;

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", upper_event->device_nr, upper_event->ar_nr);
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): Prm-Begin.rsp, wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

		/* kick state-machine */
	cm_cl_prm_trigger (argr, CM_CL_WF_PRM_BEGIN_RSP);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_AR_PRM_INDATA_RSP
//----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_opc_ar_prm_indata_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event = rb->args.cl.event_ind;
	CM_AR_GRAPH_PTR_TYPE  argr;

	argr = cm_cl_ar_from_device (channel, upper_event->device_nr);
	if (cm_is_null (argr))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!dev(%d) !ar(%d)", upper_event->device_nr, upper_event->ar_nr);
		CM_FATAL();
		return;
	}
	CM_ASSERT (argr->ar_nr == upper_event->ar_nr);


	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): Prm-InData.rsp, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	/* free event-memory */
	cm_pd_diff_list_free (channel, & upper_event->u.cl.prm_indata.diff_list);
	if (upper_event->u.cl.prm_indata.mod_diff_block_length > 0)
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.cl.prm_indata.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
	}

		/* provide again */
	CM_RQB_SET_OPCODE (rb, CM_OPC_CL_DEVICE_PROVIDE_EVENT);
	cm_cl_dev_event_provide (argr->cm.cl.dev, rb);

	cm_cl_prm_trigger (argr, CM_CL_WF_PRM_INDATA_RSP);

	if (cm_is_not_null (argr->cm.cl.rosal_park)) /* TIA 2838473 */
	{
		CM_ACP_LOWER_RQB_PTR_TYPE acp = argr->cm.cl.rosal_park;
		argr->cm.cl.rosal_park = LSA_NULL;

		cm_cl_acp_alarm_indication (argr, acp->args->alarm.data.priority, acp);
	}

		/* kick state-machine */
	switch (argr->cm.cl.state) {
	case CM_DST_WF_NEW_DATA:
		if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
		{
			cm_cl_arsm (argr);
		}
		break;

	case CM_DST_WF_DOWN:
		cm_cl_arsm (argr);
		break;

	default:
		/* only provide */
		break;
	}
}


/*------------------------------------------------------------------------------
//	handle callbacks from the prm-statemachine
//----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_start_cnf (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
		argr->cm.cl.prm.prm_keep_upd = LSA_NULL; /* cm_cl_prm_request() set to argr-pointer, to occupy prm-sm and distinguish between PRM-Connect and PRM-Update */
		return;

	case CM_CL_PRMMODE_PLUG:
		return;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		CM_ASSERT (CM_RQB_GET_OPCODE(argr->cm.cl.prm.prm_keep_upd) == CM_OPC_CL_PRM_UPDATE);

		cm_callback (argr->ar_com.channel, CM_OK, & argr->cm.cl.prm.prm_keep_upd);
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_begin_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): Prm-Begin.ind, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
		argr->cm.cl.state = CM_DST_PRM;
		return;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		argr->cm.cl.state = CM_DST_PRM;
		cm_cl_ar_event (argr, CM_OPC_CL_AR_PRM_BEGIN_IND);
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_end_req (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): pre Prm-End.req, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:

			/* aktivate provider with PrmEnd.req */
		if (argr->cm.cl.state == CM_DST_PRM)
		{
			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
				cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, CM_ARCQ_PROVIDER_SET_STATION_OK);
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PROV_ACTIVATE);
				cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, CM_ARCQ_PROVIDER_SET_STATION_OK);
			}

				/* ApplReady.ind may recv earlier then PrmEnd.cnf, so we should accept alarm here */
			cm_cl_alarm_set_enabled_state (argr, CM_CL_ALARM_ENABLED_DELAY);
		}
		return;

	case CM_CL_PRMMODE_PLUG:
		return;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_up_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): Prm-UP.ind (aka Prm-End.cnf), prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		if (argr->cm.cl.state == CM_DST_PRM)
		{
			cm_cl_arsm (argr);
		}
		return;

	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_idle_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): IDLE,  prmmode(Idle) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_IDLE:
		/*AP01292310*/
		if (argr->cm.cl.usr_state == CM_CL_AR_WF_IN)
		{
			/* argr->cm.cl.state = keep state */
			cm_cl_arsm (argr); /* continue connect */
		}
		else
		{
			argr->cm.cl.state = CM_DST_IN_DATA;
			/* cm_cl_arsm (argr); * no trigger */
		}
		return;

	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
	default:
		CM_ASSERT(0);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_down_ind (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): Prm-DOWN.ind, prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, cm_cl_prm_mode(argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	cm_cl_arsm (argr);
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
