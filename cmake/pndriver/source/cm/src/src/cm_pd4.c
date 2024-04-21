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
/*  F i l e               &F: cm_pd4.c                                  :F&  */
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

#define LTRC_ACT_MODUL_ID  804
#define CM_MODULE_ID       804

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_pd_im0_bits_ok (
	LSA_UINT8  im0_bits,
	LSA_BOOL  has_im5_record
)
{
#if CM_CFG_MAX_SERVERS
	if (
		! cm_sv_im0_bits_ok (im0_bits, has_im5_record)
		|| (im0_bits & CM_SV_IM0_BITS_R1_REMOTE) != 0 /* TIA 1915400 */
		)
	{
		return LSA_FALSE;
	}
#else /* note: server-defines not applicable */
	LSA_UNUSED_ARG(has_im5_record);
	if (im0_bits != 0)
	{
		return LSA_FALSE;
	}
#endif

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_port_addr_map_check (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  map_count,
	CM_UPPER_PD_MAP_PTR_TYPE  map_array,
	LSA_BOOL  has_im5_record
) {
	LSA_UINT16  port_id;
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	LSA_UINT16  interface_id = (LSA_UINT16)((LSA_UINT8)(edd_params->InterfaceID - 1) << 8);
	LSA_UINT16  interface_nr = 0;

	if (map_count != (1 + channel->usr.pd.port_count))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			,"map-check: map_count(%u) != 1 + edd.port_count(%u))"
			, map_count, channel->usr.pd.port_count
			);
		return CM_ERR_PARAM;
	}


	/* example: X408-2 https://www.automation.siemens.com/bilddb/download.aspx?reqInsID=716367
	//	slot_nr 0
	//	subslot_nr 0x8000 ;; interface
	//	slot_nr 5
	//	subslot_nr  0x8001
	//	subslot_nr  0x8002
	//	slot_nr 6
	//	subslot_nr  0x8001
	//	subslot_nr  0x8002
	//	slot_nr 8
	//	subslot_nr 0x8001
	//	subslot_nr 0x8002
	//	subslot_nr 0x8003
	//	subslot_nr 0x8004
	*/

	for (port_id = 0; port_id < map_count; ++port_id)
	{
		if (port_id == 0)
		{
			CM_UPPER_PD_MAP_PTR_TYPE  interface_map = &map_array[0];

			if ((   !CM_IS_PDEV_SUBSLOT_NR(interface_map->subslot_nr))
				||  ((interface_map->subslot_nr & 0x0F00) != interface_id)
				||  ((interface_map->subslot_nr & 0x00FF) != 0)
				||  ((interface_map->pdev_properties & CM_PDEV_PROP_RESERVED_CREATE_MASK) != 0)
				||  CM_PDEV_TYPE_IS_MODULAR(interface_map->pdev_properties)
				||  (CM_PDEV_TYPE_IS_COMPACT(interface_map->pdev_properties) && CM_PDEV_STATE_IS_PULLED(interface_map->pdev_properties))
				||  (! cm_pd_im0_bits_ok (interface_map->im0_bits, has_im5_record))
				)
			{
				CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "map-check: port_id(0), subslot_nr(0x%x), pdev_properties(0x%x) or im0_bits(0x%x) do not match"
					, interface_map->subslot_nr, interface_map->pdev_properties, interface_map->im0_bits
					);
				return CM_ERR_PARAM;
			}

			/* Li part of 0xLipp, has to be equal for all ports */
			interface_nr = interface_map->subslot_nr & 0xFF00;
		}
		else
		{
			CM_UPPER_PD_MAP_PTR_TYPE  map = (map_array + port_id);

			/* format: 0xLipp, note: pp != port_id is ok (AP00774150) */

			if ((   (map->subslot_nr & 0xFF00) != interface_nr)
				||  ((map->subslot_nr & 0x00FF) == 0)
				||  ((map->pdev_properties & CM_PDEV_PROP_RESERVED_CREATE_MASK) != 0)
				||  (CM_PDEV_TYPE_IS_COMPACT (map->pdev_properties)  &&  CM_PDEV_STATE_IS_PULLED (map->pdev_properties))
				||  (! cm_pd_im0_bits_ok (map->im0_bits, has_im5_record))
				)
			{

				CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "map-check: port_id(%u), subslot_nr(0x%x), pdev_properties(0x%x) or im0_bits(0x%x) do not match"
					, port_id, map->subslot_nr, map->pdev_properties, map->im0_bits
					);
				return CM_ERR_PARAM;
			}

			{
				LSA_UINT16  port_id_2;

				for (port_id_2 = 0; port_id_2 < port_id; ++port_id_2)
				{
					CM_UPPER_PD_MAP_PTR_TYPE  map_2 = (map_array + port_id_2);

					if (map->slot_nr == map_2->slot_nr  &&  map->subslot_nr == map_2->subslot_nr)
					{
						CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
							, "map-check: port_id(%u) slot_nr(%u) subslot_nr(0x%x) mapped also to port_id_2(%u)"
							, port_id, map->slot_nr, map->subslot_nr, port_id_2
							);
						return CM_ERR_PARAM;
					}

					if (map->slot_nr == map_2->slot_nr
						&&  (CM_PDEV_STATE_IS_PLUGGED (map->pdev_properties))
						&&  (CM_PDEV_STATE_IS_PLUGGED (map_2->pdev_properties))
						&&  map->mod_ident != map_2->mod_ident)
					{
						CM_PD_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
							, "map-check: port_id(%u) slot_nr(%u) subslot_nr(0x%x) and port_id(%u) slot_nr(%u) subslot_nr(0x%x) differ in mod_ident (0x%x) != (0x%x)"
							, port_id, map->slot_nr, map->subslot_nr
							, port_id_2, map_2->slot_nr, map_2->subslot_nr
							, map->mod_ident, map_2->mod_ident
							);
						return CM_ERR_PARAM;
					}
				}
			}
		}
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_port_addr_map (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16  port_id,
	CM_UPPER_PD_MAP_CONST_PTR_TYPE  map
) {
	CM_PD_PORT_PTR_TYPE  port;

	CM_PD_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "addr-map: port_id(%u) map to slot(%u) subslot(0x%x)"
		, port_id, map->slot_nr, map->subslot_nr
		);

	port = cm_pd_port_from_id__nn (channel, port_id);

	CM_ASSERT (port->addr.subslot_nr == 0);
	CM_ASSERT (map->subslot_nr != 0);
	CM_ASSERT (port->port_id == port_id);

	port->addr.slot_nr			= map->slot_nr;
	port->addr.subslot_nr		= map->subslot_nr;
	port->addr.pdev_properties	= map->pdev_properties;
	port->addr.im0_bits			= map->im0_bits; /* note: currently not checked by CM-PD */

	if (CM_PDEV_STATE_IS_PLUGGED (port->addr.pdev_properties))
	{
		port->addr.mod_ident	= map->mod_ident;
		port->addr.sub_ident	= map->sub_ident;
	}
	else
	{
		port->addr.mod_ident	= 0;
		port->addr.sub_ident	= 0;
	}
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_port_addr_unmap (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16  port_id
) {
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, port_id);

	port->addr.slot_nr		= 0;
	port->addr.subslot_nr	= 0;
	port->addr.mod_ident	= 0;
	port->addr.sub_ident	= 0;
}

/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_ALARM_RSP
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_opc_alarm_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_ALARM_PTR_TYPE param;

	param = rb->args.pd.alarm;

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PD-alarm-rsp: rqb(0x%x) slot_nr(%u) subslot_nr(0x%x) alarm_type(%u) prio(%u)"
		, rb
		, param->slot_nr, param->subslot_nr, param->alarm_type, param->alarm_priority
		);
	LSA_UNUSED_ARG (param);

	/***/

	CM_ASSERT (channel->usr.pd.req.alarm_ind_count > 0);
	channel->usr.pd.req.alarm_ind_count -= 1;

	cm_pd_alarm_provide (channel, rb);

	cm_pd_alarm_provide_event (channel);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_EVENT_APPL_READY_RSP
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_opc_appl_ready_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_EVENT_PTR_TYPE  upper_event;

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PD-appl-ready-rsp: rqb(0x%x)", rb
		);

	upper_event = rb->args.pd.event;

	cm_pd_diff_list_free (channel, & upper_event->u.pd.appl_ready.diff_list);

	if (cm_is_not_null (upper_event->u.pd.appl_ready.mod_diff_block))
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.pd.appl_ready.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
		upper_event->u.pd.appl_ready.mod_diff_block = LSA_NULL;
	}

	if (cm_is_not_null (upper_event->u.pd.appl_ready.diag_block)) /* AP01427283 */
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.pd.appl_ready.diag_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
		upper_event->u.pd.appl_ready.diag_block = LSA_NULL;
	}

	channel->usr.pd.almi.appl_ready_rsp_pending = LSA_FALSE;
	CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PROVIDE_EVENT);
	cm_pd_event_provide (channel, rb);

	if (CM_PD_CURRENT_IS_DELETE (channel))
	{
		channel->usr.pd.almi.appl_ready_ind_pending = CM_ERR_NOT_YET;  /* Sanity */
		cm_pd_delete (channel, channel->usr.pd.req.current);
	}
	else if (channel->usr.pd.almi.appl_ready_ind_pending != CM_ERR_NOT_YET) /* retrigger appl_ready_ind */
	{
		LSA_UINT16 rsp = channel->usr.pd.almi.appl_ready_ind_pending;

		channel->usr.pd.almi.appl_ready_ind_pending = CM_ERR_NOT_YET;

		cm_pd_callback_appl_ready(channel, rsp);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_LINK_STATUS_INFO
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_opc_link_status_info (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_PORT_PTR_TYPE  port;

	port = cm_pd_port_from_id (channel, rb->args.pd.link_status->port_id);
	if (cm_is_null (port))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "LinkStatusInfo: port_id(%u) doesn't exist", rb->args.pd.link_status->port_id
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (cm_is_not_null (port->misc.link_status_ind))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "LinkStatusInfo: port_id(%u) has already a indication resource, rb(%08x)"
			, rb->args.pd.link_status->port_id, port->misc.link_status_ind
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	port->misc.link_status_ind = rb;

	cm_pd_link_status_trigger_user (channel, port->port_id);
}


/*------------------------------------------------------------------------------
// common for CM-PD and CM-CL
//----------------------------------------------------------------------------*/

CM_UPPER_PD_DIFF_AP_PTR_TYPE
cm_pd_diff_ap_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) *head,
	LSA_UINT32  api
	)
{
	CM_UPPER_PD_DIFF_AP_PTR_TYPE  diff_ap;

	for (CmListForeach (diff_ap, head, CM_UPPER_PD_DIFF_AP_PTR_TYPE)) {
		if (diff_ap->api == api) {
			return diff_ap;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_ap)
		, cm_null_user_id, sizeof (*diff_ap), channel->sysptr
		);
	if (cm_is_null (diff_ap)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_ap->api = api;
	CmListInitialize (& diff_ap->module_list);

	CmListInsertTail (head, & diff_ap->link);
	return diff_ap;
}


/*----------------------------------------------------------------------------*/
CM_UPPER_PD_DIFF_MODULE_PTR_TYPE
cm_pd_diff_module_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) *head,
	LSA_UINT16  slot_nr,
	LSA_UINT32  mod_ident
	)
{
	CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  diff_mod;

	for (CmListForeach (diff_mod, head, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE)) {
		if (diff_mod->slot_nr == slot_nr) {
			return diff_mod;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_mod)
		, cm_null_user_id, sizeof (*diff_mod), channel->sysptr
		);
	if (cm_is_null (diff_mod)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_mod->slot_nr = slot_nr;
	diff_mod->mod_ident = mod_ident;

	CmListInitialize (& diff_mod->submodule_list);

	CmListInsertTail (head, & diff_mod->link);
	return diff_mod;
}

/*----------------------------------------------------------------------------*/
CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE
cm_pd_diff_submodule_find_append (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) *head,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  sub_state,
	LSA_UINT32  sub_ident
	)
{
	CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

	for (CmListForeach (diff_sub, head, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE)) {
		if (diff_sub->subslot_nr == subslot_nr) {
			return diff_sub;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_sub)
		, cm_null_user_id, sizeof (*diff_sub), channel->sysptr
		);
	if (cm_is_null (diff_sub)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_sub->subslot_nr = subslot_nr;
	diff_sub->sub_state = sub_state;
	diff_sub->sub_ident = sub_ident;

	CmListInsertTail (head, & diff_sub->link);
	return diff_sub;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_diff_list_free (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) const *head
	)
{
	CM_UPPER_PD_DIFF_AP_PTR_TYPE  diff_ap;
	CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  diff_mod;
	CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  diff_sub;
	LSA_UINT16  rc;

	while (! CmListIsEmpty (head)) {
		CmListRemoveHead (head, diff_ap, CM_UPPER_PD_DIFF_AP_PTR_TYPE);

		while (! CmListIsEmpty (& diff_ap->module_list)) {
			CmListRemoveHead (& diff_ap->module_list, diff_mod, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE);

			while (! CmListIsEmpty (& diff_mod->submodule_list)) {
				CmListRemoveHead (& diff_mod->submodule_list, diff_sub, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE);

				CM_FREE_UPPER_MEM (& rc, diff_sub, channel->sysptr);
				CM_ASSERT (rc == LSA_RET_OK);
			}

			CM_FREE_UPPER_MEM (& rc, diff_mod, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_UPPER_MEM (& rc, diff_ap, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
	}
}


/*------------------------------------------------------------------------------
//	ModuleDiffBlock ::=
//		BlockHeader
//		, NumberOfAPIs, (API
//		, NumberOfModules, (SlotNumber, ModuleIdentNumber a, ModuleState b
//		, NumberOfSubmodules, [(SubslotNumber, SubmoduleIdentNumber a, SubmoduleState)*])*)*
//
//	a Real identification data
//	b If ModuleState=NO_MODULE then NumberOfSubmodules shall be zero. The subsequent
//	part shall be omitted. For all other ModuleState only the SubmoduleState shall be used to
//	decide whether the submodule shall be parametrized.
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_difflist_to_moddiffblock (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  ar_graph_void_ptr, /* PD: LSA_NULL, CL: argr */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) const *diff_list,
	LSA_UINT16 *mod_diff_block_length,
	CM_UPPER_MEM_PTR_TYPE *mod_diff_block
	)
{
	LSA_UINT16  state;
	CM_UPPER_MEM_U8_PTR_TYPE  dp = LSA_NULL; /* = LSA_NULL because of warning C4701: potentially uninitialized local variable 'dp' used */
	LSA_UINT16  cm_dp_assert_line;
	LSA_UINT32  len = 0; /* = 0 because of warning C4701: potentially uninitialized local variable 'len' used */
	LSA_UINT32  left;
	LSA_UINT16  ap_count;

	*mod_diff_block_length = 0;
	*mod_diff_block = LSA_NULL;

	CmListLength (diff_list, & ap_count);
	if (ap_count == 0)
	{
		return;
	}

	for (state = 0; state <= 1; ++state)
	{
		LSA_BOOL  do_calc_len;

		CM_UPPER_PD_DIFF_AP_PTR_TYPE  ap;

		if (state == 0)
		{
			do_calc_len = LSA_TRUE;
			len = 0;
			left = 0;
		}
		else
		{
			do_calc_len = LSA_FALSE;

			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& dp)
				, cm_null_user_id, len, channel->sysptr
				);
			if (cm_is_null (dp)) {
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot alloc upper mem, len(%u)", len);
				CM_FATAL();
				return;
			}
			*mod_diff_block_length = (LSA_UINT16)len;
			*mod_diff_block = dp;

			left = len;
		}

		if (do_calc_len)
		{
			len += CM_RSP_MDB_SIZE;
		}
		else
		{
			CM_ASSERT (cm_is_not_null(dp));

			cm_dp_assert (left >= CM_RSP_MDB_SIZE);

			/* BlockLength ::= This field shall contain the number of octets without counting the fields BlockType and BlockLength. */

			CM_PUT16_HTON(dp, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_MOD_DIFF);
			CM_PUT16_HTON(dp, CM_BLK_HEADER_length,  CM_CAST_U16(len - CM_BLK_HEADER_TypLen_SIZE));
			CM_PUT16_HTON(dp, CM_BLK_HEADER_version, 0x0100);

			CM_PUT16_HTON(dp, CM_RSP_MDB_number_of_apis, ap_count);

			cm_dp_adjust  (dp, left, CM_RSP_MDB_SIZE);
		}

		for (CmListForeach (ap, diff_list, CM_UPPER_PD_DIFF_AP_PTR_TYPE))
		{
			CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  mod;
			LSA_UINT16  mod_count;

			CmListLength (& ap->module_list, & mod_count);

			if (do_calc_len)
			{
				len += CM_RSP_API_SIZE;
			}
			else
			{
				CM_ASSERT (cm_is_not_null(dp));

				cm_dp_assert (left >= CM_RSP_API_SIZE);

				CM_PUT32_HTON(dp, CM_RSP_API_api,               ap->api);
				CM_PUT16_HTON(dp, CM_RSP_API_number_of_modules, mod_count);

				cm_dp_adjust  (dp, left, CM_RSP_API_SIZE);
			}

			for (CmListForeach (mod, & ap->module_list, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE))
			{
				CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  sub;
				LSA_UINT16  sub_count;
				LSA_UINT16  mod_state;

				if (cm_is_null (ar_graph_void_ptr)) /* PDEV */
				{
					mod_state = CM_MOD_STATE_PROPER_MODULE;
					CmListLength (& mod->submodule_list, & sub_count);
				}
				else
				{
#if CM_CFG_MAX_CLIENTS

					CM_AR_GRAPH_PTR_TYPE ar_graph = (CM_AR_GRAPH_PTR_TYPE)ar_graph_void_ptr;

					CM_AR_GRAPH_MODULE_PTR_TYPE  ag_mod;
					LSA_UINT16  no_sub_count, ag_sub_count;

					no_sub_count = 0;
					for (CmListForeach (sub, & mod->submodule_list, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE))
					{
						if ((sub->sub_state & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_NO_SUBMODULE)
						{
							no_sub_count += 1;
						}
					}

					cm_ar_graph_find_api_slot_subslot (ar_graph
						, ap->api, mod->slot_nr, 0, LSA_NULL, & ag_mod, LSA_NULL
						);
					CM_ASSERT (cm_is_not_null (ag_mod));

					CmListLength (& ag_mod->submod_list, & ag_sub_count);

					if (ag_sub_count == no_sub_count) /* means: all submodule have IDENT_NO_SUBMODULE */
					{
						sub_count = 0;
						mod_state = CM_MOD_STATE_NO_MODULE;
					}
					else
					{
						CmListLength (& mod->submodule_list, & sub_count); /* must report all submodule */

						mod_state = CM_MOD_STATE_PROPER_MODULE;
						if (ag_mod->soll.mod_ident != mod->mod_ident)
						{
							mod_state = CM_MOD_STATE_SUBSTITUTED_MODULE;
						}
					}
#else
					mod_state = CM_MOD_STATE_PROPER_MODULE;
					CmListLength (& mod->submodule_list, & sub_count);
					CM_FATAL();
#endif
				}

				if (do_calc_len)
				{
					len += CM_RSP_MOD_SIZE;
				}
				else
				{
					CM_ASSERT (cm_is_not_null(dp));

					cm_dp_assert (left >= CM_RSP_MOD_SIZE);

					CM_PUT16_HTON(dp, CM_RSP_MOD_slot_nr,				mod->slot_nr);
					CM_PUT32_HTON(dp, CM_RSP_MOD_mod_ident,				mod->mod_ident);
					CM_PUT16_HTON(dp, CM_RSP_MOD_mod_state,				mod_state);
					CM_PUT16_HTON(dp, CM_RSP_MOD_number_of_submodules,	sub_count);

					cm_dp_adjust  (dp, left, CM_RSP_MOD_SIZE);
				}

				if (sub_count > 0)
				{
					for (CmListForeach (sub, & mod->submodule_list, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE))
					{
						if (do_calc_len)
						{
							len += CM_RSP_SUB_SIZE;
						}
						else
						{
							CM_ASSERT (cm_is_not_null(dp));

							cm_dp_assert (left >= CM_RSP_SUB_SIZE);

							CM_PUT16_HTON(dp, CM_RSP_SUB_subslot_nr, sub->subslot_nr);
							CM_PUT32_HTON(dp, CM_RSP_SUB_sub_ident,  sub->sub_ident);
							CM_PUT16_HTON(dp, CM_RSP_SUB_sub_state,  sub->sub_state);

							cm_dp_adjust  (dp, left, CM_RSP_SUB_SIZE);
						}
					}
				}
			}
		}
	}
	return;

cm_dp_assert_failed:
	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
		, "write of module_diff_block failed, line(%u), len(%u) left(%u)"
		, cm_dp_assert_line, len, left
		);
	CM_FATAL1 (cm_dp_assert_line);
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_diag_for_appl_ready( /* AP01427283 */
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16  *diag_block_length,
	CM_UPPER_MEM_PTR_TYPE *diag_block
) {

	CM_UPPER_MEM_PTR_TYPE  mem = LSA_NULL;
	LSA_UINT32  len = 0;
	LSA_UINT32  offs = 0;

	{
		LSA_UINT16  port_id;

		/* compare to cm_pd_diag_usr_copy_to_user_diag() and cm_pd_prm_read_user() */

		for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id )
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn(channel, port_id);
			LSA_UINT32 tmp = 0;
			CM_PD_DIAG_PTR_TYPE diag;

			for( CmListForeach(diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE) )
			{
				tmp += 2+2+2 + 2+4; /* ExtChannelDiagnosisData */
			}

			if( tmp != 0 )
			{ /* port has diagnoses */
				len += 2+2+1+1 + 4+2+2 + 2+2+2 + tmp; /* block-header, diagnosis-header, and diagnoses */
			}
		}
	}

	if( len != 0 )
	{
		CM_ALLOC_UPPER_MEM(&mem, cm_null_user_id, len, channel->sysptr);

		if( cm_is_null(mem) )
		{
			CM_FATAL();
		}
		else
		{
			LSA_UINT16 port_id;

			for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id )
			{
				CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn(channel, port_id);

				LSA_BOOL ok = cm_pd_diag_marshal_8000(
					port->addr.slot_nr, port->addr.subslot_nr,
					(CM_COMMON_MEM_U8_PTR_TYPE)mem, len, &offs,
					&port->diag.diag_list,
					0x800C);

				if( ! ok )
				{
					CM_FATAL(); /* a bug in length calculation */
				}
			}
		}
	}

	CM_ASSERT(offs == len);

	*diag_block_length = (LSA_UINT16)offs;
	*diag_block = mem;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
