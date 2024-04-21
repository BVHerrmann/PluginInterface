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
/*  F i l e               &F: cm_pd8.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MasterProjectAdaption for PDEV and CM-CL, common feature                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  808
#define CM_MODULE_ID       808

#include "cm_int.h"

/*----------------------------------------------------------------------------*/

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */

static CM_UPPER_RQB_PTR_TYPE
cm_pd_write_send_clock_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_PORT_CONST_PTR_TYPE port_if,
	LSA_BOOL is_prm_server
	);

static LSA_VOID
cm_pd_write_internal_free (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	);

#if CM_CFG_USE_MRP
static CM_UPPER_RQB_PTR_TYPE
cm_pd_write_mrp_interface_adjust_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_PORT_CONST_PTR_TYPE port_if,
	LSA_BOOL is_prm_server
	);

#endif


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pdusr_is_masterproject (
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get /*_nn*/ (usr_channel, CM_PATH_TYPE_PD);

	if (cm_is_not_null (pd_channel->usr.pd.reclist.mtd))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}


/*------------------------------------------------------------------------------
// cm_pd_list_record - functions
//	use upper mem, see 853634(use upper mem for records)
//
//	cm_pd_list_record_append()
//	cm_pd_list_record_lookup()
//	cm_pd_list_record_free()
//----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_list_record_append (
	CM_CHANNEL_PTR_TYPE channel,
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

	CM_PD_RECORD_PTR_TYPE     * rec_ptr_ptr	/* out */
	)
{
	CM_PD_RECORD_PTR_TYPE ptr;
	LSA_UINT32 alloc_size;

	*rec_ptr_ptr = LSA_NULL;

	/* argument check */
	alloc_size = sizeof (*ptr) + record_data_len;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&ptr), cm_null_user_id, alloc_size, channel->sysptr);
	if (cm_is_null (ptr))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "no mem, size(%u)", alloc_size);
		return 255;	/* no mem (for record) */
	}

	ptr->backlink = channel;
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
	ptr->record_data     = (CM_COMMON_MEM_U8_PTR_TYPE)(ptr + 1);/* request 1502507 (lint) */

	CM_MEMCPY (ptr->record_data, record_data, record_data_len);	/* dynamic size, see typedef */

	/***/

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "append-record to submod(%u/%u/0x%x) rec_index(0x%x) rec_len(%u)"
		, api, slot_nr, subslot_nr, record_index, record_data_len
		);

	CmListInsertTail (add_to_this_list, &ptr->link);/* NOTE: not sorted! */

	/***/

	*rec_ptr_ptr = ptr;
	return 0;	/* ok block_field */
}

/*----------------------------------------------------------------------------*/
static CM_PD_RECORD_PTR_TYPE
cm_pd_list_record_lookup (
	CM_LIST_ENTRY_CONST_PTR_TYPE from_this_list,

	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 record_index
	)
{
	CM_PD_RECORD_PTR_TYPE ptr;

	for (CmListForeach (ptr, from_this_list, CM_PD_RECORD_PTR_TYPE))
	{
		if (api == ptr->api && slot_nr == ptr->slot_nr && subslot_nr == ptr->subslot_nr && record_index == ptr->record_index)
		{
			return ptr;
		}
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_list_record_free (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec
	)
{
	LSA_UINT16 rc;

	if (cm_is_not_null (rec->tailor_data))
	{
		CM_FREE_UPPER_MEM (&rc, rec->tailor_data, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		rec->tailor_data           = LSA_NULL;
		rec->tailor_data_alloc_len = 0;
	}

	CM_FREE_UPPER_MEM (&rc, rec, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG (rc);
}


/*------------------------------------------------------------------------------
//	PD-RECLIST
//----------------------------------------------------------------------------*/

LSA_VOID
cm_pd_reclist_init (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_RECLIST_PTR_TYPE reclist = &channel->usr.pd.reclist;

	CmListInitialize (&reclist->regular_list);
	CmListInitialize (&reclist->empty_list);
	CmListInitialize (&reclist->rtf_list);
	CmListInitialize (&reclist->mixin_list);
	reclist->mtd            = LSA_NULL;
	reclist->is_running_mpa = LSA_FALSE;
	reclist->mpa_rb         = LSA_NULL;

	cm_pd_reclist_prm_begin (channel, 0, 0);

	cm_pd_reclist_clear (channel, CM_PD_RECLIST_MIXIN);	/* clear mixin records */

	reclist->is_valid = LSA_TRUE;	/*!*/
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_reclist_undo_init (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_REGULAR);
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_EMPTY);
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_RTF);
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_MIXIN);

#if CM_CFG_MAX_CLIENTS	/* see cm_pd_prm_write_internal () */
	cm_pd_mtd_free (&channel->usr.pd.reclist.mtd);
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_reclist_prm_begin (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
	)
{
	CM_PD_PRM_PTR_TYPE pdprm       = &channel->usr.pd.pdprm;
	CM_PD_RECLIST_PTR_TYPE reclist = &channel->usr.pd.reclist;

	reclist->is_valid  = LSA_FALSE;
	reclist->has_error = LSA_FALSE;

	if (subslot_nr == 0)/* Init; PD-Begin; Connect; PrmUpdate-AR */
	{
		pdprm->port = LSA_NULL;
	}
	else if (CM_IS_PDEV_INTERFACE (subslot_nr))	/* PrmUpdate-IF; Plug-IF */
	{
		pdprm->port = cm_pd_port_from_addr__nn (channel, slot_nr, subslot_nr);
	}
	else if (CM_IS_PDEV_PORT (subslot_nr))	/* Plug-Port */
	{
		pdprm->port = cm_pd_port_from_addr__nn (channel, slot_nr, subslot_nr);
		return;
	}
	else
	{
		CM_FATAL1 (subslot_nr);
		return;
	}

	reclist->prm_data.do_irt             = LSA_FALSE;
	reclist->prm_data.do_mrp_record      = LSA_FALSE;
	reclist->prm_data.scr_send_clock     = 0;	/* see cm_cmpd_prm_write() and cm_pd_reclist_add_default_rec() */
	reclist->prm_data.scr_sendclock_prop = 0;	/* see where scr_send_clock is set */
	reclist->prm_data.ir_data_uuid       = cm_uuid_nil_uuid;

	cm_pd_reclist_clear (channel, CM_PD_RECLIST_REGULAR);	/* clear normal records */

#if CM_CFG_MAX_CLIENTS	/* see cm_pd_prm_write_internal () */
	cm_pd_mtd_free (&reclist->mtd);
#endif
}


/*----------------------------------------------------------------------------*/
static CM_LIST_ENTRY_PTR_TYPE
cm_pd_reclist_get (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 list_type
	)
{
	switch (list_type)
	{
	case CM_PD_RECLIST_REGULAR:
		return &channel->usr.pd.reclist.regular_list;

	case CM_PD_RECLIST_EMPTY:
		return &channel->usr.pd.reclist.empty_list;

	case CM_PD_RECLIST_RTF:
		return &channel->usr.pd.reclist.rtf_list;

	case CM_PD_RECLIST_MIXIN:
		return &channel->usr.pd.reclist.mixin_list;

	default:
		CM_FATAL1 (list_type);
		return LSA_NULL;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_reclist_clear (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 list_type
	)
{
	CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get (channel, list_type);
	CM_PD_RECORD_PTR_TYPE rec;

	while (! CmListIsEmpty (reclist))
	{
		CmListRemoveHead (reclist, rec, CM_PD_RECORD_PTR_TYPE);

		cm_pd_list_record_free (channel, rec);
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_delete (
	/* TIA 1193013 */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 record_index
	)
{
	CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);

	if (cm_is_null (port))
	{
		return CM_ERR_PRM_PORTID;	/* not mapped */
	}
	else
	{
		CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get (channel, CM_PD_RECLIST_REGULAR);
		CM_PD_RECORD_PTR_TYPE rec;

		rec = cm_pd_list_record_lookup (reclist, 0 /*PDEV*/, slot_nr, subslot_nr, record_index);

		if (cm_is_not_null (rec))
		{
			CmListRemoveEntry (&rec->link);
			cm_pd_list_record_free (channel, rec);

			return CM_OK;
		}
		else
		{
			return CM_ERR_PRM_INDEX;/* not found */
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_write (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 list_type,
	CM_UPPER_RQB_CONST_PTR_TYPE rb
	)
{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;
	CM_LIST_ENTRY_PTR_TYPE reclist         = cm_pd_reclist_get (channel, list_type);
	LSA_UINT16 transfer_sequence           = 0;
	LSA_UINT32 api = 0;	/* PDEV */

	CM_PD_RECORD_PTR_TYPE rec;
	LSA_UINT16 rc;

	/* remove if exist, keep only latest record */
	rec = cm_pd_list_record_lookup (reclist, api, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index);
	if (cm_is_not_null (rec))
	{
		CmListRemoveEntry (&rec->link);
		cm_pd_list_record_free (channel, rec);
	}

	rc = cm_pd_list_record_append (channel, reclist
		, api, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
		, prm_rdwr->record_index, transfer_sequence
		, prm_rdwr->mk_remanent == CM_PD_MK_REMANENT_YES ? LSA_TRUE : LSA_FALSE, prm_rdwr->is_local == CM_PD_IS_LOCAL_YES ? LSA_TRUE : LSA_FALSE
		, prm_rdwr->record_data_length, prm_rdwr->record_data
		, &rec
		);

	if (rc != 0)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "reclist-write: failed, record_index(%u), list_type(%u)", prm_rdwr->record_index, list_type
			);
		return CM_ERR_ALLOC_LOCAL;
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_reset_to_factory (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_LIST_ENTRY_PTR_TYPE rec_list = cm_pd_reclist_get (channel, CM_PD_RECLIST_RTF);	/* ResetToFactory */
	CM_PD_RECORD_PTR_TYPE rec;

	for (CmListForeach (rec, rec_list, CM_PD_RECORD_PTR_TYPE))
	{
		if (rec->record_index == 0x00010000	/* SendClock */)
		{
			CM_PD_RECLIST_PTR_TYPE reclist = &channel->usr.pd.reclist;
			reclist->prm_data.scr_send_clock     = CM_GET16_NTOH (rec->record_data, 6);	/* SendClock from cm_pd_write_send_clock_alloc() */
			reclist->prm_data.scr_sendclock_prop = CM_GET16_NTOH (rec->record_data, 8);	/* SendClockProp from cm_pd_write_send_clock_alloc() */
			break;
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_add_ep_rec (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_LIST_ENTRY_PTR_TYPE rec_list = cm_pd_reclist_get (channel, CM_PD_RECLIST_EMPTY);
	CM_PD_RECORD_PTR_TYPE rec;

	for (CmListForeach (rec, rec_list, CM_PD_RECORD_PTR_TYPE))
	{
		if (rec->record_index == 0x00010000	/* SendClock */)
		{
			CM_PD_RECLIST_PTR_TYPE reclist = &channel->usr.pd.reclist;

			/* update the pre-allocated SendClock-record */

			CM_PUT16_HTON (rec->record_data, 6, reclist->prm_data.scr_send_clock);	/* SendClock from CM_PD_RECLIST_REGULAR */
			CM_PUT16_HTON (rec->record_data, 8, reclist->prm_data.scr_sendclock_prop);	/* SendClockProp from CM_PD_RECLIST_REGULAR */
			break;
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_add_default_rec (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_PRM_CONST_PTR_TYPE pdprm,
	LSA_BOOL is_prm_server,
	LSA_UINT8 set_mrp_off
	)
{
	CM_PD_PORT_PTR_TYPE port_if    = cm_pd_port_if__nn (channel);
	CM_PD_PORT_PTR_TYPE port       = cm_pd_port_from_addr__nn (channel, port_if->addr.slot_nr, port_if->addr.subslot_nr);
	CM_PD_RECLIST_PTR_TYPE reclist = &channel->usr.pd.reclist;
	LSA_UINT16 rsp                 = CM_OK;

	if (reclist->prm_data.scr_send_clock == 0)	/* no SendClock-record written, make default record */
	{
		CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_send_clock_alloc (channel, port_if, is_prm_server);

		if (cm_is_null (rb))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to alloc send clock rb"
				);
			return CM_ERR_RESOURCE;
		}

		rsp = cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb);

		if (rsp == CM_OK)
		{
			/* set scr_send_clock */
			rsp = cm_cmpd_prm_write (pdprm, port, rb->args.pd.prm_rdwr);
		}

		if (rsp != CM_OK)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to append send clock record"
				);
		}

		cm_pd_write_internal_free (channel, rb);
	}

#if CM_CFG_USE_MRP
	if (rsp == CM_OK
		&& (! reclist->prm_data.do_mrp_record /* if no MRP-record written */ &&  set_mrp_off == CM_SET_MRP_OFF_YES /* AP00824058: set MRPRole := OFF */))
	{
		CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_mrp_interface_adjust_alloc (channel, port_if, is_prm_server);

		if (cm_is_null (rb))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to alloc mrp adjust rb"
				);
			return CM_ERR_RESOURCE;
		}

		rsp = cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb);

		if (rsp == CM_OK)
		{
			/* set do_mrp_record */
			rsp = cm_cmpd_prm_write (pdprm, port, rb->args.pd.prm_rdwr);
		}

		if (rsp != CM_OK)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to append mrp adjust record"
				);
		}

		cm_pd_write_internal_free (channel, rb);
	}
#else
	LSA_UNUSED_ARG (set_mrp_off);
#endif

	return rsp;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_reclist_alloc_records (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_PORT_PTR_TYPE port_if = cm_pd_port_if__nn (channel);

	CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_send_clock_alloc (channel, port_if, LSA_FALSE);	/* default send clock */
	if (cm_is_null (rb))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to alloc send clock rb"
			);
		CM_FATAL ();
		return;
	}

	if (cm_pd_reclist_write (channel, CM_PD_RECLIST_EMPTY, rb) != CM_OK)/* used for empty parameterization */
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to append send clock record"
			);
		CM_FATAL ();
		return;
	}

	if (cm_pd_reclist_write (channel, CM_PD_RECLIST_RTF, rb) != CM_OK)	/* used for reset to factory */
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to append send clock record"
			);
		CM_FATAL ();
		return;
	}

	cm_pd_write_internal_free (channel, rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_reclist_set_error (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	if (! channel->usr.pd.reclist.has_error)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PRM_SC: reclist_set_error"
			);
		channel->usr.pd.reclist.has_error = LSA_TRUE;
	}
}

/*----------------------------------------------------------------------------*/
CM_PD_RECORD_PTR_TYPE
cm_pd_reclist_get_next (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec,
	LSA_UINT16 list_type
	)
{
	CM_LIST_ENTRY_PTR_TYPE list = cm_pd_reclist_get (channel, list_type);

	if (list_type == CM_PD_RECLIST_RTF  &&  cm_is_not_null (rec) && rec->record_index == 0x00010003)/* TIA 1702410 */
	{
		return LSA_NULL;
	}

	if (cm_is_null (rec))
	{
		rec = CmListFirst (list, CM_PD_RECORD_PTR_TYPE);
	}
	else
	{
		rec = CmListNext (list, &rec->link, CM_PD_RECORD_PTR_TYPE);
	}

	while (cm_is_not_null (rec))
	{
		if (rec->tailor_data_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
		{
			rec = CmListNext (list, &rec->link, CM_PD_RECORD_PTR_TYPE);
		}
		else
		{
			return rec;
		}
	}

	if (list_type == CM_PD_RECLIST_RTF  &&  cm_is_null (rec))	/* TIA 1702410 */
	{
		CM_LIST_ENTRY_PTR_TYPE mixin_list = cm_pd_reclist_get (channel, CM_PD_RECLIST_MIXIN);
		for (CmListForeach (rec, mixin_list, CM_PD_RECORD_PTR_TYPE))
		{
			if (rec->record_index == 0x00010003 && rec->tailor_data_len != CM_TAILOR_DATA_LEN_SKIP_RECORD)
			{
				return rec;
			}
		}
	}

	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
CM_PD_RECORD_PTR_TYPE
cm_pd_reclist_read (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 list_type,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 record_index
	)
{
	CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get (channel, list_type);
	CM_PD_RECORD_PTR_TYPE rec;

	rec = cm_pd_list_record_lookup (reclist, 0 /*PDEV*/, slot_nr, subslot_nr, record_index);

	return rec;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_write_internal_free (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	LSA_UINT16 rc;

	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_INTERNAL_OPC_PD_INTERNAL)
	CM_ASSERT (CM_RQB_GET_HANDLE (rb) == CM_INVALID_HANDLE);

	CM_FREE_UPPER_MEM (&rc, rb->args.pd.prm_rdwr->record_data, channel->sysptr);
	CM_ASSERT (rc == CM_OK);

	cm_upper_free_rqb (channel, rb);
}

/*----------------------------------------------------------------------------*/
static CM_UPPER_RQB_PTR_TYPE
cm_pd_write_send_clock_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_PORT_CONST_PTR_TYPE port_if,
	LSA_BOOL is_prm_server
	)
{
	CM_UPPER_RQB_PTR_TYPE rb;
	LSA_UINT16 record_data_length;
	CM_UPPER_MEM_PTR_TYPE record_data;
	LSA_UINT16 send_clock;
	LSA_UINT16 sendclock_prop;


	rb = cm_upper_alloc_rqb (channel);
	if (cm_is_null (rb))
	{
		CM_FATAL ();/* no mem */
		return LSA_NULL;
	}

	/* SendClock ::= BlockHeader, SendClockFactor, SendClockProperties, Reserved, [Data*] */

	record_data_length = 2 + 2 + 1 + 1 + 2 + 2 + 2;

	CM_ALLOC_UPPER_MEM (&record_data, cm_null_user_id, record_data_length, channel->sysptr);
	if (cm_is_null (record_data))
	{
		CM_FATAL ();/* no mem */
		return LSA_NULL;
	}

	if (is_prm_server)
	{

		/* note: the SendClock-record does not come via RPC */
		CM_ASSERT (channel->usr.pd.clock.send_clock != 0);
		send_clock     = channel->usr.pd.clock.send_clock;	/* must be self-fulfilling, see cm_pdusr_attach() */
		sendclock_prop = cm_pd_sendclock_prop_get (channel);/* must keep what the startup-parameterization said */
	}
	else
	{

		/* TIA 612541 (V5.3i2.15) updates AP00914550 and AP01353701
		 *
		 * if the user does not write a SendClock-record, the following rules apply:
		 *
		 * EDDS: SC=FIX, RR=VAR
		 * - SC=FIX because the send-clock cannot be changed at runtime
		 * - RR=VAR because otherwise an AR with another send-clock cannot be established (e.g., PDEV has SC=32ms and the AR has SC=1ms and RR=32)
		 * - note that this contradicts AP00914550 (no automatic RR-adaption)
		 *
		 * EDDI/EDDP: SC=VAR, RR=FIX
		 * - SC=VAR because the send-clock can be changed at runtime
		 * - RR=FIX because automatic RR-adaption is not desired (AP00914550, V4.2i2.4)
		 *
		 */

		if (! cm_edd_can_set_send_clock (channel))
		{
			send_clock     = channel->usr.pd.clock.send_clock;	/* unchanged */
			sendclock_prop = CM_SENDCLOCK_PROP_SC_FIX_RR_VAR;	/* TIA 612541 (was SC_FIX_RR_FIX) */
		}
		else
		{
			send_clock     = 32;/* default to 1 ms */
			sendclock_prop = CM_SENDCLOCK_PROP_SC_VAR_RR_FIX;
		}
	}

	CM_PUT16_HTON (record_data, 0, CM_BLOCK_TYPE_SEND_CLOCK);
	CM_PUT16_HTON (record_data, 2, CM_CAST_U16 (record_data_length - (2 + 2)));
	CM_PUT16_HTON (record_data, 4, 0x0100);
	CM_PUT16_HTON (record_data, 6, send_clock);
	CM_PUT16_HTON (record_data, 8, sendclock_prop);
	CM_PUT16_HTON (record_data, 10, 0x0000);

	{
		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

		prm_rdwr->slot_nr            = port_if->addr.slot_nr;
		prm_rdwr->subslot_nr         = port_if->addr.subslot_nr;
		prm_rdwr->record_index       = 0x00010000;
		prm_rdwr->record_data_length = record_data_length;
		prm_rdwr->record_data        = CM_CAST_COMMON_MEM_U8_PTR (record_data);

		CM_RQB_SET_HANDLE (rb, CM_INVALID_HANDLE);
		CM_RQB_SET_OPCODE (rb, CM_INTERNAL_OPC_PD_INTERNAL);

		if (is_prm_server)
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_YES;
		}
		else
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_NO;
		}

		prm_rdwr->is_local = CM_PD_IS_LOCAL_YES;
		prm_rdwr->cls      = cm_record_index_classify (channel, 0 /*!*/, prm_rdwr->record_index, CM_VENDOR_ID_DONTCARE);
	}

	return rb;
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
static CM_UPPER_RQB_PTR_TYPE
cm_pd_write_mrp_interface_adjust_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_PORT_CONST_PTR_TYPE port_if,
	LSA_BOOL is_prm_server
	)
{
	CM_UPPER_RQB_PTR_TYPE rb;
	LSA_UINT16 record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE record_data;
	CLRPC_UUID_TYPE mrp_uuid;

	/***/

	rb = cm_upper_alloc_rqb (channel);
	if (cm_is_null (rb))
	{
		CM_FATAL ();/* no mem */
		return LSA_NULL;
	}

	record_data_length = 2 + 2 + 1 + 1 + 1 + 1 + 16 + 2 + 2	/*pad*/ + 1 + 17 + 2 /*pad*/;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&record_data)
		, cm_null_user_id, record_data_length, channel->sysptr
		);
	if (cm_is_null (record_data))
	{
		CM_FATAL ();/* no mem */
		return LSA_NULL;
	}

	/* PDInterfaceMrpDataAdjust ::=
	**	BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Role, [Padding*] a
	**	, MRP_LengthDomainName_u8, MRP_DomainName, [Padding*] a, ...
	**
	**  a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
	**
	**	MRP_Role             .. OFF ==0
	**	MRP_DomainName       .. default-mrpdomain
	**	MRP_LengthDomainName .. 17
	*/

	CM_MEMSET (&mrp_uuid, 0xFF, sizeof (mrp_uuid));

	CM_PUT16_HTON (record_data, 0, CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
	CM_PUT16_HTON (record_data, 2, CM_CAST_U16 (record_data_length - (2 + 2)));
	CM_PUT16_HTON (record_data, 4, 0x0100);
	CM_PUT16_HTON (record_data, 6, 0x0000);

	CM_PUT_UUID_HTON (record_data, 8, &mrp_uuid);
	CM_PUT16_HTON (record_data, 24, 0);
	CM_PUT16_HTON (record_data, 26, 0);
	CM_PUT8_HTON (record_data, 28, 17);
	CM_MEMCPY (record_data + 29, CM_CAST_LOCAL_MEM_PTR ("default-mrpdomain"), 17);
	CM_PUT16_HTON (record_data, 46, 0);

	{
		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

		prm_rdwr->slot_nr            = port_if->addr.slot_nr;
		prm_rdwr->subslot_nr         = port_if->addr.subslot_nr;
		prm_rdwr->record_index       = 0x8052;	/* PDInterfaceMrpDataAdjust */
		prm_rdwr->record_data_length = record_data_length;
		prm_rdwr->record_data        = record_data;

		CM_RQB_SET_HANDLE (rb, CM_INVALID_HANDLE);
		CM_RQB_SET_OPCODE (rb, CM_INTERNAL_OPC_PD_INTERNAL);

		if (is_prm_server)
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_YES;
		}
		else
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_NO;
		}

		prm_rdwr->is_local = CM_PD_IS_LOCAL_YES;
		prm_rdwr->cls      = cm_record_index_classify (channel, 0 /*!*/, prm_rdwr->record_index, CM_VENDOR_ID_DONTCARE);
	}

	return rb;
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_reclist_tailor (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = channel->usr.pd.reclist.mtd;

	if (cm_is_not_null (mtd))
	{
		CM_PD_PORT_PTR_TYPE port_if    = cm_pd_port_if__nn (channel);
		CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get (channel, CM_PD_RECLIST_REGULAR);
		LSA_UINT32 api                 = 0;	/* PDEV */
		LSA_UINT16 slot_nr             = port_if->addr.slot_nr;
		LSA_UINT16 subslot_nr          = port_if->addr.subslot_nr;

		CM_PD_RECORD_PTR_TYPE rec_8052;
		CM_COMMON_MEM_U8_PTR_TYPE ioc_nos;
		LSA_UINT16 ioc_nos_len, rsp;

		{
			CM_PD_RECORD_PTR_TYPE rec;
			for (CmListForeach (rec, reclist, CM_PD_RECORD_PTR_TYPE))
			{
				rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;
			}
		}

		if (! cm_oha_get_name_of_station (channel, &ioc_nos_len, &ioc_nos))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "Controller NoS missing");
			return CM_ERR_SEQUENCE;
		}

		{
			CM_PD_RECORD_PTR_TYPE rec;

			for (CmListForeach (rec, reclist, CM_PD_RECORD_PTR_TYPE))
			{
				if (rec->record_index == 0x802b	/* PDPortDataCheck */)
				{
					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}

		{
			LSA_UINT16 pdirdata_td_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

			if (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)
			{
				CM_PD_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x802C	/* PDIRData */);

				rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
				if (rsp != CM_OK)
				{
					return rsp;
				}
				pdirdata_td_len = rec->tailor_data_len;
			}

			if (mtd->properties & CM_PD_MTD_PROP_SYNCDATA)
			{
				CM_PD_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x802d	/* PDSyncData */);
				if (pdirdata_td_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
				{
					CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dropping PDSyncData");
					rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
				}
				else if (mtd->properties & CM_PD_MTD_PROP_NOS)
				{
					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}

		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
		{
			rec_8052 = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x8052 /* PDInterfaceMrpDataAdjust */);

			rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec_8052, LSA_NULL, ioc_nos, ioc_nos_len);
			if (rsp != CM_OK)
			{
				return rsp;
			}
		}
		else
		{
			rec_8052 = LSA_NULL;
		}

		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
		{
			CM_PD_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x8051	/* PDInterfaceMrpDataCheck */);

			rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, rec_8052, ioc_nos, ioc_nos_len);
			if (rsp != CM_OK)
			{
				return rsp;
			}
		}

		{
			LSA_UINT16 index;

			for (index = 0; index < mtd->port_cnt; index++)
			{
				CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[index];

				if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
				{
					CM_PD_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup (reclist
						, api, mtdp->local_slot_nr, mtdp->local_subslot_nr, 0x8053	/* PDPortMrpDataAdjust */
						);

					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, rec_8052, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_reclist_validate_and_prealloc_tailoring_records (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd,
	CM_PD_PRM_DIAG_PTR_TYPE prm_diag
	)
{
	CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get (channel, CM_PD_RECLIST_REGULAR);
	CM_PD_PORT_PTR_TYPE port_if = cm_pd_port_if__nn (channel);
	LSA_UINT32 api = 0;						/* PDEV */
	LSA_UINT16 slot_nr = port_if->addr.slot_nr;
	LSA_UINT16 subslot_nr = port_if->addr.subslot_nr;
	CM_PD_RECORD_PTR_TYPE rec, rec_8052;

	for (CmListForeach (rec, reclist, CM_PD_RECORD_PTR_TYPE))
	{
		if (cm_pd_mpa_prm_parse_record (channel, mtd, rec) != CM_OK)
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr__nn (channel, rec->slot_nr, rec->subslot_nr);

			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ParseError, record-index(0x%x)", rec->record_index);
			CM_PD_PRM_DIAG_SET_WRITE_FAULT (prm_diag, port->port_id, rec->record_index, 0);
			return LSA_FALSE;
		}
	}

	/* CM_PD_MTD_PROP_NOS: PDPortDataCheck is not checked, since configurations without a check record are possible */

	if (mtd->properties & CM_PD_MTD_PROP_SYNCDATA)
	{
		rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x802D /* PDSyncData */);

		if (cm_is_null (rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDSyncData record not found");
			CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x802D);
			return LSA_FALSE;
		}
	}

	if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
	{
		rec      = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x8052 /* PDInterfaceMrpDataAdjust */);
		rec_8052 = rec;

		if (cm_is_null (rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
			CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x8052);
			return LSA_FALSE;
		}
	}
	else
	{
		rec_8052 = LSA_NULL;
	}

	if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
	{
		rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x8051 /* PDInterfaceMrpDataCheck */);
		if (cm_is_null (rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataCheck record not found");
			CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x8051);
			return LSA_FALSE;
		}
		if (cm_is_null (rec_8052))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
			CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x8052);
			return LSA_FALSE;
		}
	}

	if ((mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA))
	{
		rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x802C /* PDIRData */);
		if (cm_is_null (rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDIRData record not found");
			CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x802C);
			return LSA_FALSE;
		}
	}

	{
		LSA_UINT16 index;

		for (index = 0; index < mtd->port_cnt; index++)
		{
			CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[index];

			if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
			{
				rec = cm_pd_list_record_lookup (reclist, api, mtdp->local_slot_nr, mtdp->local_subslot_nr, 0x8053 /* PDPortMrpDataAdjust */);
				if (cm_is_null (rec))
				{
					CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr__nn (channel, mtdp->local_slot_nr, mtdp->local_subslot_nr);

					CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDPortMrpDataAdjust record not found, port-index(%u)", index);
					CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, port->port_id, 0x8053);
					return LSA_FALSE;
				}
				if (cm_is_null (rec_8052))
				{
					CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
					CM_PD_PRM_DIAG_SET_END_MISSING (prm_diag, 0, 0x8052);
					return LSA_FALSE;
				}
			}
		}
	}

	/* prealloc tailor_data */
	for (CmListForeach (rec, reclist, CM_PD_RECORD_PTR_TYPE))
	{
		if (cm_pd_mpa_prm_prealloc_tailor_data (channel, rec) != CM_OK)
		{
			CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr__nn (channel, rec->slot_nr, rec->subslot_nr);

			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDev prealloc tailor data failed, record_index(0x%x)", rec->record_index);

			CM_PD_PRM_DIAG_SET_WRITE_MISSING (prm_diag, port->port_id, rec->record_index);
			return LSA_FALSE;
		}
	}

	return LSA_TRUE;/* Ok */
}

/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_pdev_is_ready_for_tailoring (
	/* note: the pdev of a non series machine is always ready */
	CM_PD_PRM_CONST_PTR_TYPE pdprm
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel           = pdprm->channel;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (pd_channel, 0 /*PDev*/);

	if (cm_is_not_null (pd_mtd))
	{
		/* temporary: RtfNosIpChanged is not ready yet */
		if (pd_channel->usr.pd.rtf.wf_sm != 0)
		{
			return LSA_FALSE;
		}

		/* temporary: PDInstanceTailorData running */
		if (pd_channel->usr.pd.reclist.is_running_mpa)
		{
			return LSA_FALSE;
		}

		if (pd_mtd->properties & CM_PD_MTD_PROP_IP_SUITE)
		{
			LSA_UINT32 ip_addr, subnet, gateway;

			if (! cm_oha_get_ip_suite (pd_channel, &ip_addr, &subnet, &gateway))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & CM_PD_MTD_PROP_NOS)
		{
			LSA_UINT16 cmi_station_name_length;
			CM_UPPER_MEM_U8_PTR_TYPE cmi_station_name;

			if (! cm_oha_get_name_of_station (pd_channel, &cmi_station_name_length, &cmi_station_name))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & (CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			if (! (pd_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & (CM_PD_MTD_PROP_NOS | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			if (! cm_pd_mtd_neighbour_exists (pd_channel, pd_mtd))
			{
				return LSA_FALSE;
			}
		}
	}
	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// _mpa_ MasterProjectAdaption
//----------------------------------------------------------------------------*/

static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDIRData (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd,
	CM_PD_RECORD_PTR_TYPE rec
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp = rec->record_data;
	LSA_UINT32 left = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left = rec->tailor_data_alloc_len;
	LSA_UINT32 adj, number_of_ports;
	LSA_UINT16 block_type, block_len, block_version, pdirdata_block_version;

	/* PDIRData :=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData,
	//				PDIRFrameData, PDIRBeginEndData
	*/
	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
	cm_dp_assert (left >= adj);
	cm_dp_assert (wr_left >= left);

	block_type             = CM_GET16_NTOH (dp, 0);
	block_len              = CM_GET16_NTOH (dp, 2);
	pdirdata_block_version = CM_GET16_NTOH (dp, 4);

	cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_IR_DATA);
	cm_dp_assert (block_len == rec->record_data_len - (2 + 2));
	cm_dp_assert (pdirdata_block_version == 0x0100 || pdirdata_block_version == 0x0101);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_dp_adjust (dp, left, adj);
	cm_dp_adjust (wr_dp, wr_left, adj);

	{
		/*	PDIRGlobalData ( BlockVersionLow = 1) :=
		//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts,
		//		(MaxPortTxDelay, MaxPortRxDelay)
		//	PDIRGlobalData (BlockVersionLow = 2) :=
		//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts,
		//		(MaxPortTxDelay, MaxPortRxDelay, MaxLineRxDelay, YellowTime)*
		*/

		adj = 2 + 2 + 1 + 1 + 1 + 1 + 16 + 4 + 4;
		cm_dp_assert (left >= adj);

		block_type      = CM_GET16_NTOH (dp, 0);
		block_len       = CM_GET16_NTOH (dp, 2);
		block_version   = CM_GET16_NTOH (dp, 4);
		number_of_ports = CM_GET32_NTOH (dp, 28);

		cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA);
		cm_dp_assert (block_len <= left - (2 + 2));
		if (block_version == 0x0101)
		{
			adj = 2 + 2 + block_len;/* no tailoring */
		}
		else
		{
			cm_dp_assert (block_version == 0x0102)
		}
		cm_dp_assert (1 <= number_of_ports && number_of_ports <= 0xFF);

		cm_dp_assert (left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_dp_adjust (dp, left, adj);
		cm_dp_adjust (wr_dp, wr_left, adj);

		if (block_version == 0x0102)
		{
			LSA_UINT16 p;

			adj = 4 + 4 + 4 + 4;

			for (p = 0; p < number_of_ports; ++p)
			{
				CM_PD_MTD_PORT_PTR_TYPE mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, p);

				cm_dp_assert (left >= adj);

				CM_MEMCPY (wr_dp, dp, adj);

				if (cm_is_not_null (mtdp) && ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT))
				{
					CM_PUT32_HTON (wr_dp, 8, mtdp->real_max_line_rx_delay);
				}

				cm_dp_adjust (dp, left, adj);
				cm_dp_adjust (wr_dp, wr_left, adj);
			}
		}
	}

	{
		/* PDIRFrameData (BlockVersionLow = 0) :=
		//		BlockHeader, Padding, Padding
		//		,   (FrameSendOffset, DataLength, ReductionRatio, Phase, FrameID, EtherType, RxPort, FrameDetails, TxPortGroup, [Padding*] )*
		//
		// PDIRFrameData (BlockVersionLow = 1) :=
		//		BlockHeader, Padding, Padding, FrameDataProperties
		//		, [ (FrameSendOffset, DataLength, ReductionRatio, Phase, FrameID, EtherType, RxPort, FrameDetails, TxPortGroup, [Padding*] )* ]
		*/

		adj = 2 + 2;
		cm_dp_assert (left >= adj);

		block_type = CM_GET16_NTOH (dp, 0);
		block_len  = CM_GET16_NTOH (dp, 2);

		cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_IR_FRAME_DATA);
		cm_dp_assert (CM_CAST_U32 (block_len + (2 + 2)) <= left);

		if (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)
		{
			CM_COMMON_MEM_U8_PTR_TYPE blocklen_dp = wr_dp;
			LSA_UINT32 block_left;
			LSA_UINT32 frame_count = 0;

			adj += 1 + 1 + 1 + 1;
			cm_dp_assert (left >= adj);

			block_version = CM_GET16_NTOH (dp, 4);

			if (block_version == 0x0101)
			{
				adj += 4;	/* FrameDataProperties */
			}
			else
			{
				cm_dp_assert (block_version == 0x0100);
			}

			cm_dp_assert (left >= adj);
			cm_dp_assert (wr_left >= adj);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_dp_adjust (dp, left, adj);
			cm_dp_adjust (wr_dp, wr_left, adj);

			cm_dp_assert (block_len >= adj - (2 + 2));
			block_left = block_len - (adj - (2 + 2));

			while (block_left > 0)
			{
				LSA_UINT8 number_of_txportgroups;
				LSA_UINT16 frame_id;
				/* FrameSendOffset(0), DataLength(4), ReductionRatio(6), Phase(8),
				// FrameID(10), EtherType(12), RxPort(14), FrameDetails(15), NumberOfTxPortGroups(16)
				*/

				adj = 4 + 2 + 2 + 2 + 2 + 2 + 1 + 1 + 1;
				/* TxPortGroupArray [Padding*] */
				cm_dp_assert (left >= adj);

				frame_id               = CM_GET16_NTOH (dp, 10);
				number_of_txportgroups = CM_GET8_NTOH (dp, 16);

				adj += number_of_txportgroups;
				adj += (4 - adj % 4) % 4;

				cm_dp_assert (left >= adj);

				if (cm_pdusr_mpa_tailor_frame_id (channel, frame_id))
				{
					cm_dp_adjust (dp, left, adj);	/* skip frame_id */
				}
				else
				{
					CM_MEMCPY (wr_dp, dp, adj);

					cm_dp_adjust (dp, left, adj);
					cm_dp_adjust (wr_dp, wr_left, adj);
					frame_count++;
				}
				cm_dp_assert (block_left >= adj);
				block_left -= adj;
			}

			if (frame_count == 0)
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dropping PDIRData");
				rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
				return CM_OK;
			}

			CM_PUT16_HTON (blocklen_dp, 2, CM_CAST_U16 (wr_dp - blocklen_dp - (2 + 2)));
		}
		else
		{
			adj += block_len;

			cm_dp_assert (left >= adj);
			cm_dp_assert (wr_left >= adj);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_dp_adjust (dp, left, adj);
			cm_dp_adjust (wr_dp, wr_left, adj);
		}
	}

	if (left > 0)
	{
		cm_dp_assert (pdirdata_block_version == 0x0101);/* PDIRBeginEndData */
		{
			/* PDIRBeginEndData := BlockHeader, Padding, Padding, RedGuard, NumberOfPorts, (NumberOfAssignments,
			//						(TXBeginEndAssignment, RXBeginEndAssignment)*, NumberOfPhases,
			//						(TXPhaseAssignment, RXPhaseAssignment)*)*
			*/

			LSA_UINT16 p;
			CM_COMMON_MEM_U8_PTR_TYPE blocklen_dp = wr_dp;


			/* BlockHeader, Padding, Padding, RedGuard, NumberOfPorts */

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2 + 4;

			cm_dp_assert (left >= adj);
			cm_dp_assert (wr_left >= adj);

			block_type      = CM_GET16_NTOH (dp, 0);
			block_len       = CM_GET16_NTOH (dp, 2);
			block_version   = CM_GET16_NTOH (dp, 4);
			number_of_ports = CM_GET32_NTOH (dp, 12);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA);
			cm_dp_assert (block_len == left - (2 + 2));
			cm_dp_assert (block_version == 0x0100);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_dp_adjust (dp, left, adj);
			cm_dp_adjust (wr_dp, wr_left, adj);

			for (p = 0; p < number_of_ports; ++p)
			{
				LSA_UINT32 number_of_assignments, number_of_phases;
				CM_PD_MTD_PORT_PTR_TYPE mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, p);

				number_of_assignments = CM_GET32_NTOH (dp, 0);

				adj = 4 + number_of_assignments * ((4 + 4 + 4) + (4 + 4 + 4));

				cm_dp_assert (wr_left >= adj);

				number_of_phases = CM_GET32_NTOH (dp, adj);

				adj += 4 + number_of_phases * (2 + 2);

				cm_dp_assert (wr_left >= adj);

				/* NumberOfAssignments, (TXBeginEndAssignment, RXBeginEndAssignment)*, NumberOfPhases,
				//						(TXPhaseAssignment, RXPhaseAssignment)*
				*/

				/*	TXBeginEndAssignment := RedOrangePeriodBegin, OrangePeriodBegin, GreenPeriodBegin
				//	RXBeginEndAssignment := RedOrangePeriodBegin, OrangePeriodBegin, GreenPeriodBegin
				//	TXPhaseAssignment := PhaseAssignment
				//	RXPhaseAssignment := PhaseAssignment
				*/

				cm_pd_mtdp_get_peer (channel, mtdp);

				if (cm_is_not_null (mtdp)
					&& (mtdp->properties & CM_PD_MTD_PEER_PROP_RED_PORT)
					&& (mtdp->real_station_nr == 0xFFFF
						|| (cm_is_not_null (mtdp->real_mtdp) && mtdp->real_mtdp->mtd->pdirglobaldata_version == 0)))
				{
					LSA_UINT32 wr_adj = 4 + ((4 + 4 + 4) + (4 + 4 + 4)) + 4 + (2 + 2);

					cm_dp_assert (wr_left >= wr_adj);

					CM_PUT32_HTON (wr_dp, 0, 0x01);
					CM_PUT32_HTON (wr_dp, 4, 0x00);
					CM_PUT32_HTON (wr_dp, 8, 0x00);
					CM_PUT32_HTON (wr_dp, 12, 0x00);
					CM_PUT32_HTON (wr_dp, 16, 0x00);
					CM_PUT32_HTON (wr_dp, 20, 0x00);
					CM_PUT32_HTON (wr_dp, 24, 0x00);

					CM_PUT32_HTON (wr_dp, 28, 0x01);
					CM_PUT16_HTON (wr_dp, 32, 0x00);
					CM_PUT16_HTON (wr_dp, 34, 0x00);

					cm_dp_adjust (wr_dp, wr_left, wr_adj);
				}
				else
				{
					CM_MEMCPY (wr_dp, dp, adj);

					cm_dp_adjust (wr_dp, wr_left, adj);
				}

				cm_dp_adjust (dp, left, adj);
			}

			CM_PUT16_HTON (blocklen_dp, 2, CM_CAST_U16 (wr_dp - blocklen_dp - (2 + 2)));
		}
	}

	cm_dp_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16 (wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16 (rec->tailor_data_len - (2 + 2)));

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDIRData Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT8
cm_pd_mpa_put_port_name (
	CM_COMMON_MEM_U8_PTR_TYPE dp,
	LSA_UINT16 properties,
	LSA_UINT16 subslot_nr,
	LSA_UINT16 slot_nr
	)
{
	LSA_UINT8 port_nr = subslot_nr & 0x00FF;
	LSA_UINT8 p_len   = 8;

	if (properties & CM_PD_MTD_PEER_PROP_PORT_CODING)
	{
		p_len = 14;
	}

	if (cm_is_null (dp))
	{
		return p_len;
	}

	CM_PUT8_HTON (dp, 0, 'p');
	CM_PUT8_HTON (dp, 1, 'o');
	CM_PUT8_HTON (dp, 2, 'r');
	CM_PUT8_HTON (dp, 3, 't');
	CM_PUT8_HTON (dp, 4, '-');
	CM_PUT8_HTON (dp, 5, '0');	/* see CM_PORT_UINT / EDD_CFG_MAX_PORT_CNT */
	CM_PUT8_HTON (dp, 6, '0' + port_nr / 10);
	CM_PUT8_HTON (dp, 7, '0' + port_nr % 10);

	if (p_len == 14)
	{
		CM_PUT8_HTON (dp, 8, '-');
		CM_PUT8_HTON (dp, 9, '0' + CM_CAST_U8 (slot_nr / 10000));
		slot_nr %= 10000;
		CM_PUT8_HTON (dp, 10, '0' + CM_CAST_U8 (slot_nr / 1000));
		slot_nr %= 1000;
		CM_PUT8_HTON (dp, 11, '0' + CM_CAST_U8 (slot_nr / 100));
		slot_nr %= 100;
		CM_PUT8_HTON (dp, 12, '0' + CM_CAST_U8 (slot_nr / 10));
		slot_nr %= 10;
		CM_PUT8_HTON (dp, 13, '0' + CM_CAST_U8 (slot_nr));
	}

	return p_len;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDPortDataCheck (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd,
	CM_PD_MTD_PORT_PTR_TYPE start_mtdp,	/* this port */
	CM_PD_RECORD_PTR_TYPE rec,
	CM_COMMON_MEM_U8_PTR_TYPE ioc_nos,
	LSA_UINT16 ioc_nos_len	/* 0 .. do not tailor ioc_nos */
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp    = rec->record_data;
	LSA_UINT32 left                 = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left              = rec->tailor_data_alloc_len;
	LSA_UINT32 adj;
	LSA_UINT32 max_checklinedelay = 0;

	cm_pd_mtdp_get_peer (channel, start_mtdp);

	if (start_mtdp->real_station_nr == 0xFFFF)	/* TIA 1794695, TIA 1795576: any ports and alternative peers need follow up */
	{
		rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
		return CM_OK;
	}

	if (cm_is_not_null (start_mtdp->real_mtdp))
	{
		CM_PD_MTD_PORT_PTR_TYPE peer_real_mtdp = cm_pdusr_mpa_lookup_mtdp (channel
			, start_mtdp->real_mtdp->peer_station_nr, start_mtdp->real_mtdp->peer_slot_nr, start_mtdp->real_mtdp->peer_subslot_nr, LSA_NULL
			);

		if (start_mtdp->check_linedelay != 0 && cm_is_not_null (peer_real_mtdp) && peer_real_mtdp->check_linedelay != 0)
		{
			max_checklinedelay = LSA_MAX (start_mtdp->check_linedelay, peer_real_mtdp->check_linedelay);
		}
	}

	/*	PDPortDataCheck ::=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
	//		, { [CheckPeers], [CheckLineDelay], [CheckMAUType], [CheckLinkState], [CheckSyncDifference], [CheckMAUTypeDifference], [CheckMAUTypeExtension] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
	cm_dp_assert (left >= adj);
	cm_dp_assert (wr_left >= adj);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_dp_adjust (dp, left, adj);
	cm_dp_adjust (wr_dp, wr_left, adj);

	{
		/*	CheckPeers		::= BlockHeader, NumberOfPeers, (LengthPeerPortName, PeerPortName, LengthPeerStationName, PeerStationName)*, [Padding*]
		//	CheckLineDelay	::= BlockHeader, Padding, Padding, LineDelay
		//	CheckMAUType	::= BlockHeader, MAUType
		//	CheckLinkState	::= BlockHeader, LinkState
		//	CheckSyncDifference	::= BlockHeader, CheckSyncMode
		//	CheckMAUTypeDifference	::= BlockHeader, MAUTypeMode
		//	CheckMAUTypeExtension	::= BlockHeader, MAUTypeExtension
		*/
		while (left > 0)
		{
			LSA_UINT16 block_type, block_length;

			adj = 2 + 2;
			cm_dp_assert (left >= adj);
			cm_dp_assert (wr_left >= adj);

			block_type   = CM_GET16_NTOH (dp, 0);
			block_length = CM_GET16_NTOH (dp, 2);

			switch (block_type)
			{
			case CM_BLOCK_TYPE_PD_CHK_LINE_DELAY:
				adj += block_length;

				cm_dp_assert (left >= adj);

				if (max_checklinedelay == 0)/* skip sub-block */
				{
					cm_dp_adjust (dp, left, adj);
				}
				else
				{
					cm_dp_assert (wr_left >= adj);

					CM_MEMCPY (wr_dp, dp, adj);
					CM_PUT32_HTON (wr_dp, 8, max_checklinedelay);

					cm_dp_adjust (dp, left, adj);
					cm_dp_adjust (wr_dp, wr_left, adj);
				}
				break;

			case CM_BLOCK_TYPE_PD_CHK_PEERS:
				adj += block_length;

				cm_dp_assert (left >= adj);

				if (cm_is_null (start_mtdp->real_mtdp))	/* skip sub-block */
				{
					cm_dp_adjust (dp, left, adj);
				}
				else
				{
					CM_COMMON_MEM_U8_PTR_TYPE cp_wr_dp = wr_dp;

					/* BlockHeader + NrOfPeers(1) */
					{
						cm_dp_assert (wr_left >= 2 + 2 + 1 + 1 + 1);

						CM_MEMCPY (wr_dp, dp, 2 + 2 + 1 + 1 + 1);

						cm_dp_adjust (dp, left, adj);
						cm_dp_adjust (wr_dp, wr_left, 2 + 2 + 1 + 1 + 1);
					}

					/* LengthPeerPortName, PeerPortName */
					{
						LSA_UINT8 p_len = cm_pd_mpa_put_port_name (LSA_NULL
							, start_mtdp->real_properties, start_mtdp->real_mtdp->local_subslot_nr, start_mtdp->real_mtdp->local_slot_nr
							);

						adj = 1 + p_len;

						cm_dp_assert (wr_left >= adj);

						CM_PUT8_HTON (wr_dp, 0, p_len /*LengthPeerPortName*/);
						p_len = cm_pd_mpa_put_port_name (wr_dp + 1
							, start_mtdp->real_properties, start_mtdp->real_mtdp->local_subslot_nr, start_mtdp->real_mtdp->local_slot_nr
							);
						cm_dp_assert (adj > p_len);

						cm_dp_adjust (wr_dp, wr_left, adj);
					}

					/* LengthPeerStationName, PeerStationName */
					{
						CM_COMMON_MEM_U8_PTR_TYPE iod_nos = LSA_NULL;
						CM_COMMON_MEM_U8_PTR_TYPE p_len_dp;
						LSA_UINT16 iod_nos_len = 0;
						LSA_UINT16 p_len       = 0;

						if (start_mtdp->real_station_nr > 0)
						{
							CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (channel);

							if (cm_is_not_null (cl_channel))
							{
								CM_CL_DEVICE_PTR_TYPE dev = cm_cl_dev_from_device_nr (cl_channel, start_mtdp->real_station_nr);

								if (cm_is_not_null (dev))
								{
									iod_nos     = dev->dev_addr.device_name;
									iod_nos_len = dev->dev_addr.device_name_length;
								}
							}
						}

						p_len_dp = wr_dp;

						cm_dp_assert (wr_left >= 1);
						CM_PUT8_HTON (wr_dp, 0, 0);
						cm_dp_adjust (wr_dp, wr_left, 1);

						if (iod_nos_len > 0)
						{
							cm_dp_assert (wr_left >= iod_nos_len);
							CM_MEMCPY (wr_dp, iod_nos, iod_nos_len);
							cm_dp_adjust (wr_dp, wr_left, iod_nos_len);
							p_len += iod_nos_len;

							if (ioc_nos_len > 0 && (mtd->properties & CM_PD_MTD_PROP_NOS))
							{
								cm_dp_assert (wr_left >= 1);
								CM_PUT8_HTON (wr_dp, 0, '.');
								cm_dp_adjust (wr_dp, wr_left, 1);
								p_len += 1;
							}
						}

						if (ioc_nos_len > 0 && (start_mtdp->real_station_nr == 0 || (mtd->properties & CM_PD_MTD_PROP_NOS)))
						{
							cm_dp_assert (wr_left >= ioc_nos_len);
							CM_MEMCPY (wr_dp, ioc_nos, ioc_nos_len);
							cm_dp_adjust (wr_dp, wr_left, ioc_nos_len);
							p_len += ioc_nos_len;
						}

						cm_dp_assert (p_len <= 240);
						CM_PUT8_HTON (p_len_dp, 0, CM_CAST_U8 (p_len));
					}

					/* Padding */
					while (((wr_dp - cp_wr_dp) & 3) != 0)
					{
						cm_dp_assert (wr_left >= 1);
						CM_PUT8_HTON (wr_dp, 0, 0);
						cm_dp_adjust (wr_dp, wr_left, 1);
					}

					/* CheckPeers.BlockHeader.BlockLength */
					CM_PUT16_HTON (cp_wr_dp, 2, CM_CAST_U16 (wr_dp - cp_wr_dp - (2 + 2)));
				}
				break;

			case CM_BLOCK_TYPE_PD_CHK_MAUTYPE:
			case CM_BLOCK_TYPE_PD_CHK_LINK_STATE:
			case CM_BLOCK_TYPE_PD_CHK_SYNC_DIFFERENCE:
			case CM_BLOCK_TYPE_PD_CHK_MAUTYPE_DIFFERENCE:
			case CM_BLOCK_TYPE_PD_CHK_MAUTYPE_EXTENSION:
			default:
				adj += block_length;

				cm_dp_assert (left >= adj);
				cm_dp_assert (wr_left >= adj);

				CM_MEMCPY (wr_dp, dp, adj);

				cm_dp_adjust (dp, left, adj);
				cm_dp_adjust (wr_dp, wr_left, adj);
				break;
			}
		}

		cm_dp_assert (left == 0);
	}

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16 (wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16 (rec->tailor_data_len - (2 + 2)));

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortDataCheck Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDPortDataAdjust (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MTD_PORT_PTR_TYPE start_mtdp,	/* this port */
	CM_PD_RECORD_PTR_TYPE rec
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp    = rec->record_data;
	LSA_UINT32 left                 = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left              = rec->tailor_data_alloc_len;
	LSA_UINT32 adj;

	/* PDPortDataAdjust ::=
	//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
	//		, { [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState], [AdjustPeerToPeerBoundary]
	//		, [AdjustDCPBoundary], [AdjustPreambleLength] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	cm_pd_mtdp_get_peer (channel, start_mtdp);

	if (cm_is_not_null (start_mtdp->real_mtdp) && (start_mtdp->real_mtdp->mtd->pdirglobaldata_version > 0))
	{
		return CM_OK;
	}

	adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
	cm_dp_assert (left >= adj);
	cm_dp_assert (wr_left >= adj);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_dp_adjust (dp, left, adj);
	cm_dp_adjust (wr_dp, wr_left, adj);

	{
		/*	AdjustPreambleLength	::= BlockHeader, Padding, Padding, PreambleLength, AdjustProperties, [Padding*]
		//	other					::= BlockHeader, Data*
		*/

		while (left > 0)
		{
			LSA_UINT16 block_type, block_length;

			adj = 2 + 2;
			cm_dp_assert (left >= adj);
			cm_dp_assert (wr_left >= adj);

			block_type   = CM_GET16_NTOH (dp, 0);
			block_length = CM_GET16_NTOH (dp, 2);

			switch (block_type)
			{
			case CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH:
				adj += block_length;

				cm_dp_assert (left >= adj);
				cm_dp_assert (wr_left >= adj);

				CM_MEMCPY (wr_dp, dp, adj);

				CM_PUT16_HTON (wr_dp, 2 + 2 + 1 + 1 + 1 + 1, 0);/* PreambleLength.Length: Seven octets Preamble shall be used */

				cm_dp_adjust (dp, left, adj);
				cm_dp_adjust (wr_dp, wr_left, adj);

				break;

			default:
				adj += block_length;

				cm_dp_assert (left >= adj);
				cm_dp_assert (wr_left >= adj);

				CM_MEMCPY (wr_dp, dp, adj);

				cm_dp_adjust (dp, left, adj);
				cm_dp_adjust (wr_dp, wr_left, adj);
				break;
			}
		}
	}

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16 (wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16 (rec->tailor_data_len - (2 + 2)));

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDSyncData (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_COMMON_MEM_U8_PTR_TYPE ioc_nos,
	LSA_UINT16 ioc_nos_len
	)
{
	CM_COMMON_MEM_U8_PTR_TYPE dp = rec->tailor_data;
	/*lint --e{713} Loss of precision request 1524437*/
	LSA_UINT32 offs = 0;

	LSA_UINT32 offs_ri_begin, offs_ri_end, offs_pllwindow, offs_syncsendfactor, offs_sendclock;
	LSA_UINT32 offs_timeout, offs_subdomainname_length, offs_subdomainname, offs_subdomainuuid;

	LSA_UINT16 block_length, domain_name_length_new;
	LSA_UINT8 domain_name_length;
	LSA_UINT8 subdomain_uuid[CM_MD5_DIGEST_LENGTH];

	CM_MD5_CTX_TYPE ctx;

	LSA_UNUSED_ARG (channel);

	/*
	PDSyncData :=
	    BlockHeader, Padding, Padding, PTCP_SubdomainUUID, ReservedIntervalBegin, ReservedIntervalEnd, PLLWindow, SyncSendFactor,
	    SendClockFactor, PTCPTimeoutFactor, PTCPTakeoverTimeoutFactor, PTCPMasterStartupTime, SyncProperties, PTCP_MasterPriority1,
	    PTCP_MasterPriority2, PTCPLengthSubdomainName, PTCPSubdomainName, [Padding*]
	*/

	offs += 2 + 2 + 1 + 1;	/* BlockHeader */
	offs += 1;		/* Padding */
	offs += 1;		/* Padding */
	offs_subdomainuuid        = offs;
	offs                     += 16;	/* PTCP_SubdomainUUID */
	offs_ri_begin             = offs;
	offs                     += 4;	/* ReservedIntervalBegin */
	offs_ri_end               = offs;
	offs                     += 4;	/* ReservedIntervalEnd */
	offs_pllwindow            = offs;
	offs                     += 4;	/* PLLWindow */
	offs_syncsendfactor       = offs;
	offs                     += 4;	/* SyncSendFactor */
	offs_sendclock            = offs;
	offs                     += 2;	/* SendClockFactor */
	offs_timeout              = offs;
	offs                     += 2;	/* PTCPTimeoutFactor */
	offs                     += 2;	/* PTCPTakeoverTimeoutFactor */
	offs                     += 2;	/* PTCPMasterStartupTime */
	offs                     += 2;	/* SyncProperties */
	offs                     += 1;	/* PTCP_MasterPriority1 */
	offs                     += 1;	/* PTCP_MasterPriority2 */
	offs_subdomainname_length = offs;

	if (offs + 1 > rec->record_data_len)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDSyncData, Length of Record failed");
		return CM_ERR_PRM_DATA;
	}

	domain_name_length     = CM_GET8_NTOH (rec->record_data, offs_subdomainname_length);
	domain_name_length_new = domain_name_length + 1 + ioc_nos_len;

	if (domain_name_length_new > 240)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "Tailored subdomain name would exceed 240 characters"
			);
		return CM_ERR_PRM_BLOCK;
	}

	CM_MEMCPY (dp, rec->record_data, offs_subdomainname_length);

	CM_PUT8_HTON (dp, offs, CM_CAST_U8 (domain_name_length_new));	/* PTCPLengthSubdomainName */
	offs              += 1;
	offs_subdomainname = offs;

	if (offs + domain_name_length > rec->record_data_len)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDSyncData, Length of Record failed");
		return CM_ERR_PRM_DATA;
	}

	CM_MEMCPY (dp + offs, rec->record_data + offs, domain_name_length);	/* PTCPSubdomainName */
	offs += domain_name_length;

	CM_PUT8_HTON (dp, offs, '.');
	offs += 1;

	CM_MEMCPY (dp + offs, ioc_nos, ioc_nos_len);/* NoS controller */
	offs += ioc_nos_len;

	/* PTCP_SubdomainUUID := MD5 { ReservedIntervalBegin, ReservedIntervalEnd, PLLWindow, SyncSendFactor, SendClockFactor, PTCPTimeoutFactor, PTCPSubdomainName } */

	cm_md5_init (&ctx);
	cm_md5_update (&ctx, rec->record_data + offs_ri_begin, 4);
	cm_md5_update (&ctx, rec->record_data + offs_ri_end, 4);
	cm_md5_update (&ctx, rec->record_data + offs_pllwindow, 4);
	cm_md5_update (&ctx, rec->record_data + offs_syncsendfactor, 4);
	cm_md5_update (&ctx, rec->record_data + offs_sendclock, 2);
	cm_md5_update (&ctx, rec->record_data + offs_timeout, 2);
	cm_md5_update (&ctx, dp + offs_subdomainname, domain_name_length + 1 + ioc_nos_len);
	cm_md5_final (subdomain_uuid, &ctx);

	CM_MEMCPY (dp + offs_subdomainuuid, subdomain_uuid, CM_MD5_DIGEST_LENGTH);	/* PTCP_SubdomainUUID */

	while ((offs & 3) != 0)
	{
		CM_PUT8_HTON (dp, offs, 0);	/* Padding */
		offs += 1;
	}

	block_length = CM_CAST_U16 (offs - (2 + 2));
	CM_PUT16_HTON (dp, 2, block_length);/* BlockLength */

	CM_ASSERT (offs <= 0xFFFF);
	rec->tailor_data_len = (LSA_UINT16)offs;

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static CM_COMMON_MEM_U8_PTR_TYPE
cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (
	CM_PD_RECORD_CONST_PTR_TYPE rec_8052,
	LSA_UINT16 rec_block_version,
	LSA_UINT8 rec_mrp_instance
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp = rec_8052->tailor_data;
	LSA_INT32 left               = rec_8052->tailor_data_len;

	LSA_INT32 adj;
	LSA_UINT16 block_type, block_length, block_version;
	LSA_UINT8 nr_of_entries, mrp_instance;

	adj = 2 + 2 + 1 + 1 + 1 + 1;
	cm_dp_assert (left >= adj);

	block_type    = CM_GET16_NTOH (dp, 0);
	block_length  = CM_GET16_NTOH (dp, 2);
	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH (dp, 7);

	cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
	cm_dp_assert (2 + 2 + block_length == left);
	cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
	cm_dp_assert (block_version == rec_block_version);

	if (block_version == 0x0100)
	{
		cm_dp_assert (nr_of_entries == 0);

		cm_dp_adjust (dp, left, adj);

		cm_dp_assert (left >= 16);
		return dp;
	}
	else
	{
		cm_dp_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_dp_adjust (dp, left, adj);

		while (left > 0)
		{
			adj = 2 + 2 + 1 + 1 + 1 + 1;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			mrp_instance  = CM_GET8_NTOH (dp, 7);

			cm_dp_assert (block_type == 0x0231 /*MrpInstance-DataAdjustBlock*/);
			cm_dp_assert (left >= 2 + 2 + block_length);
			cm_dp_assert (block_version == 0x0100);
			cm_dp_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);

			if (mrp_instance == rec_mrp_instance)
			{
				cm_dp_adjust (dp, left, adj);

				cm_dp_assert (left >= 16);
				return dp;
			}

			cm_dp_adjust (dp, left, 2 + 2 + block_length);
		}

		return LSA_NULL;
	}

cm_dp_assert_failed:
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
		, "Record8052, lookup MRP_DomainUUID, mrp_instance(%u), error in line(%u): record-offset(%u)"
		, rec_mrp_instance, cm_dp_assert_line, ((left > rec_8052->tailor_data_len) ? 0 : (rec_8052->tailor_data_len - left))
		);

	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataCheck (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_PD_RECORD_CONST_PTR_TYPE rec_8052
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp    = rec->record_data;
	LSA_UINT32 left                 = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left              = rec->tailor_data_alloc_len;

	LSA_UINT32 adj;
	LSA_UINT16 block_version;
	LSA_UINT8 nr_of_entries;

	/*	PDInterface-MrpDataCheck [BlockVersionLow = 0]  ::=
	//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Check
	//
	//	PDInterface-MrpDataCheck [BlockVersionLow = 1]  ::=
	//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataCheckBlock*
	//
	//	MrpInstance-DataCheckBlock [BlockType = 0x0233] ::=
	//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID, MRP_Check
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2 + 2 + 1 + 1 + 1 + 1;
	cm_dp_assert (left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH (dp, 7);

	cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);

	if (block_version == 0x0100)
	{
		cm_dp_assert (nr_of_entries == 0);
	}
	else
	{
		cm_dp_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_dp_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_dp_adjust (dp, left, adj);
		cm_dp_adjust (wr_dp, wr_left, adj);
	}

	while (left > 0)
	{
		CM_COMMON_MEM_U8_PTR_TYPE uuid;
		LSA_UINT8 mrp_instance;

		adj = 2 + 2 + 1 + 1 + 1 + 1 + 16 + 4;

		cm_dp_assert (left >= adj);
		cm_dp_assert (wr_left >= adj);

		mrp_instance = CM_GET8_NTOH (dp, 7);

		cm_dp_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);

		CM_MEMCPY (wr_dp, dp, adj);

		uuid = cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (rec_8052, block_version, mrp_instance);
		cm_dp_assert (cm_is_not_null (uuid));

		CM_MEMCPY (wr_dp + 2 + 2 + 1 + 1 + 1 + 1, uuid, 16);

		cm_dp_adjust (dp, left, adj);
		cm_dp_adjust (wr_dp, wr_left, adj);
	}

	cm_dp_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = rec->record_data_len;

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInterfaceMrpDataCheck Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDPortMrpDataAdjust (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_PD_RECORD_CONST_PTR_TYPE rec_8052
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp    = rec->record_data;
	LSA_UINT32 left                 = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left              = rec->tailor_data_alloc_len;

	LSA_UINT32 adj;
	LSA_UINT16 block_version;
	LSA_UINT8 mrp_instance;
	CM_COMMON_MEM_U8_PTR_TYPE uuid;

	/*	PDPortMrpDataAdjust [BlockVersionLow = 0]  ::=
	//		BlockHeader, Padding, Padding, MRP_DomainUUID
	//
	//	PDPortMrpDataAdjust [BlockVersionLow = 1]  ::=
	//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2 + 2 + 1 + 1 + 1 + 1;
	cm_dp_assert (left >= adj);
	cm_dp_assert (wr_left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	mrp_instance  = CM_GET8_NTOH (dp, 7);

	cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
	cm_dp_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_dp_adjust (wr_dp, wr_left, adj);


	adj = 16;
	cm_dp_assert (wr_left >= adj);

	uuid = cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (rec_8052, block_version, mrp_instance);
	cm_dp_assert (cm_is_not_null (uuid));

	CM_MEMCPY (wr_dp, uuid, adj);

	cm_dp_adjust (wr_dp, wr_left, adj);
	LSA_UNUSED_ARG (wr_dp);	/* lint */
	LSA_UNUSED_ARG (wr_left);	/* lint */

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = rec->record_data_len;

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortMrpDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataAdjust (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_COMMON_MEM_U8_PTR_TYPE ioc_nos,
	LSA_UINT16 ioc_nos_len
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp    = rec->record_data;
	LSA_UINT32 left                 = rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE wr_dp = rec->tailor_data;
	LSA_UINT32 wr_left              = rec->tailor_data_alloc_len;
	LSA_UINT32 adj;
	LSA_UINT16 block_version;
	LSA_UINT8 nr_of_entries, name_length;

	/*	PDInterface-MrpDataAdjust [BlockVersionLow = 0] ::=
	//		BlockHeader, Padding, Padding
	//		, MRP_DomainUUID, MRP_Role, [Padding*] a, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
	//
	//	PDInterface-MrpDataAdjust [BlockVersionLow = 1] ::=
	//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataAdjustBlock*
	//
	//	MrpInstance-DataAdjustBlock [BlockType = 0x0231] ::=
	//		BlockHeader, Padding, MRP_Instance
	//		, MRP_DomainUUID, MRP_Role, [Padding*] a, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2 + 2 + 1 + 1 + 1 + 1;
	cm_dp_assert (left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH (dp, 7);

	cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);

	if (block_version == 0x0100)
	{
		cm_dp_assert (nr_of_entries == 0);

		nr_of_entries += 1;
	}
	else
	{
		cm_dp_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_dp_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_dp_adjust (dp, left, adj);
		cm_dp_adjust (wr_dp, wr_left, adj);
	}

	while (nr_of_entries > 0)
	{
		CM_COMMON_MEM_U8_PTR_TYPE block_dp;
		CM_COMMON_MEM_U8_PTR_TYPE wr_block_dp, wr_uuid_dp, wr_name_dp;
		LSA_UINT16 block_type;
		LSA_UINT32 block_left;

		block_dp    = dp;
		wr_block_dp = wr_dp;

		adj        = 2 + 2 + 1 + 1 + 1 + 1;
		wr_uuid_dp = wr_block_dp + adj;

		adj += 16 + 2 + 2 + 1;
		cm_dp_assert (left >= adj);
		cm_dp_assert (wr_left >= adj);

		block_type  = CM_GET16_NTOH (dp, 0);
		block_left  = 2 + 2 + CM_GET16_NTOH (dp, 2 /* block_length */);
		name_length = CM_GET8_NTOH (dp, adj - 1);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_dp_assert ((block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ    &&  block_version == 0x0100)
			|| (block_type == 0x0231 /*MrpInstanceDataAdjustBlock*/ && block_version == 0x0101));
		cm_dp_assert (block_left <= left);
		cm_dp_assert (name_length > 0);
		cm_dp_assert (name_length + 1 + ioc_nos_len <= 240);

		CM_PUT8_HTON (wr_dp, adj - 1, CM_CAST_U8 (name_length + 1 + ioc_nos_len));

		cm_dp_adjust (dp, left, adj);
		block_left -= adj;
		cm_dp_adjust (wr_dp, wr_left, adj);

		cm_dp_assert (left >= name_length);
		cm_dp_assert (block_left >= name_length);
		cm_dp_assert (wr_left >= CM_CAST_U8 (name_length + 1 + ioc_nos_len));

		wr_name_dp = wr_dp;
		CM_MEMCPY (wr_dp, dp, name_length);
		CM_PUT8_HTON (wr_dp, name_length, '.');
		CM_MEMCPY (wr_dp + name_length + 1, ioc_nos, ioc_nos_len);

		cm_dp_adjust (dp, left, name_length);
		block_left -= name_length;
		cm_dp_adjust (wr_dp, wr_left, name_length + 1 + ioc_nos_len);

		while (((dp - block_dp) & 0x3) != 0)
		{
			cm_dp_assert (left >= 1);
			cm_dp_assert (block_left >= 1);
			cm_dp_adjust (dp, left, 1);
			block_left -= 1;
		}

		while (((wr_dp - wr_block_dp) & 0x3) != 0)
		{
			cm_dp_assert (wr_left >= 1);
			CM_PUT8_HTON (wr_dp, 0, 0);
			cm_dp_adjust (wr_dp, wr_left, 1);
		}

		/* { [(MrpManagerParams) ^ (MrpClientParams)] } */
		adj = block_left;
		cm_dp_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_dp_adjust (dp, left, adj);
		cm_dp_adjust (wr_dp, wr_left, adj);

		/* calc md5 of MRP_DomainName */
		{
			CM_MD5_CTX_TYPE ctx;
			LSA_UINT8 uuid[CM_MD5_DIGEST_LENGTH];

			cm_md5_init (&ctx);
			cm_md5_update (&ctx, wr_name_dp, name_length + 1 + ioc_nos_len);
			cm_md5_final (uuid, &ctx);

			CM_MEMCPY (wr_uuid_dp, uuid, CM_MD5_DIGEST_LENGTH);
		}

		/* BlockHeader.BlockLength */
		CM_PUT16_HTON (wr_block_dp, 2, CM_CAST_U16 (wr_dp - wr_block_dp - (2 + 2)));

		nr_of_entries -= 1;
	}

	cm_dp_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16 (wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16 (rec->tailor_data_len - (2 + 2)));

	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInterfaceMrpDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_dp_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_mpa_prm_write_tailor_data (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_PD_RECORD_CONST_PTR_TYPE rec_8052,
	CM_COMMON_MEM_U8_PTR_TYPE ioc_nos,
	LSA_UINT16 ioc_nos_len
	)
{
	LSA_UINT16 rsp = CM_OK;

	switch (rec->record_index)
	{
	case 0x802b:/* PDPortDataCheck */
		{
			CM_PD_MTD_PORT_PTR_TYPE start_mtdp = LSA_NULL;

			if ((mtd->properties & CM_PD_MTD_PROP_NOS)
				|| (mtd->properties & (CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG)))
			{
				start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);
			}
			if (cm_is_not_null (start_mtdp))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortDataCheck (channel, mtd, start_mtdp, rec, ioc_nos, ioc_nos_len);
			}
		}
		break;

	case 0x802f:/* PDPortDataAdjust */
		{
			CM_PD_MTD_PORT_PTR_TYPE start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			if (cm_is_not_null (start_mtdp) && (start_mtdp->properties & CM_PD_MTD_PEER_PROP_RED_PORT))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortDataAdjust (channel, start_mtdp, rec);
			}
		}
		break;

	case 0x802d:/* PDSyncData */
		if ((mtd->properties & CM_PD_MTD_PROP_SYNCDATA) && (mtd->properties & CM_PD_MTD_PROP_NOS))
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDSyncData (channel, rec, ioc_nos, ioc_nos_len);
		}
		break;

	case 0x8051:/* PDInterfaceMrpDataCheck */
		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataCheck (channel, rec, rec_8052);
		}
		break;

	case 0x8052:/* PDInterfaceMrpDataAdjust */
		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataAdjust (channel, rec, ioc_nos, ioc_nos_len);
		}
		break;

	case 0x8053:/* PDPortMrpDataAdjust */
		{
			CM_PD_MTD_PORT_PTR_TYPE start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			if (cm_is_not_null (start_mtdp) && (start_mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortMrpDataAdjust (channel, rec, rec_8052);
			}
		}
		break;

	case 0x802C:/* PDIRData */
		/* bits are checked inside */
		rsp = cm_pd_mpa_prm_write_tailor_data_PDIRData (channel, mtd, rec);
		break;

	default:
		return CM_OK;
	}

	if (rec->tailor_data_len > rec->tailor_data_alloc_len)
	{
		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "tailor_data_len(%u) > tailor_data_alloc_len(%u), record_index(%u)"
			, rec->tailor_data_len, rec->tailor_data_alloc_len, rec->record_index
			);
		CM_FATAL ();
	}

	if (rsp != CM_OK)
	{
		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "Failed to write tailor data, subslot(%u/%u/0x%x) record-index(0x%x)", rec->api, rec->slot_nr, rec->subslot_nr, rec->record_index
			);
	}

	return rsp;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_mpa_prm_parse_record (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd,
	CM_PD_RECORD_CONST_PTR_TYPE rec
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp = rec->record_data;
	LSA_UINT32 left              = rec->record_data_len;
	LSA_UINT32 adj;

	/* see SPH Table 1 Data record compendium */

	switch (rec->record_index)
	{
	case 0x802b:/* PDPortDataCheck */
		{
			/*	PDPortDataCheck ::=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
			//		, { [CheckPeers], [CheckLineDelay], [CheckMAUType], [CheckLinkState], [CheckSyncDifference], [CheckMAUTypeDifference], [CheckMAUTypeExtension] }
			*/
			CM_PD_MTD_PORT_PTR_TYPE mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			/* cm_is_null() means an "any peer" */

			if (cm_is_not_null (mtdp))
			{
				LSA_UINT16 block_type, block_length, block_version, pad, slot_nr, subslot_nr;

				adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
				cm_dp_assert (left >= adj);

				block_type    = CM_GET16_NTOH (dp, 0);
				block_length  = CM_GET16_NTOH (dp, 2);
				block_version = CM_GET16_NTOH (dp, 4);
				pad           = CM_GET16_NTOH (dp, 6);
				slot_nr       = CM_GET16_NTOH (dp, 8);
				subslot_nr    = CM_GET16_NTOH (dp, 10);

				cm_dp_adjust (dp, left, adj);

				cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_PORT_DATA_CHECK);
				cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
				cm_dp_assert (block_version == 0x0100);
				cm_dp_assert (pad == 0);
				cm_dp_assert (slot_nr == rec->slot_nr);
				cm_dp_assert (subslot_nr == rec->subslot_nr);

				/*	CheckPeers		::= BlockHeader, NumberOfPeers, (LengthPeerPortName, PeerPortName, LengthPeerStationName, PeerStationName)*, [Padding*]
				//	CheckLineDelay	::= BlockHeader, Padding, Padding, LineDelay
				//	CheckMAUType	::= BlockHeader, MAUType
				//	CheckLinkState	::= BlockHeader, LinkState
				//	CheckSyncDifference	::= BlockHeader, CheckSyncMode
				//	CheckMAUTypeDifference	::= BlockHeader, MAUTypeMode
				//	CheckMAUTypeExtension	::= BlockHeader, MAUTypeExtension
				*/
				while (left > 0)
				{
					LSA_UINT16 nr_of_peers;
					LSA_UINT32 linedelay;

					adj = 2 + 2;
					cm_dp_assert (left >= adj);

					block_type   = CM_GET16_NTOH (dp, 0);
					block_length = CM_GET16_NTOH (dp, 2);

					cm_dp_adjust (dp, left, adj);

					cm_dp_assert (left >= block_length);

					switch (block_type)
					{
					case CM_BLOCK_TYPE_PD_CHK_PEERS:
						cm_dp_assert (block_length >= 1 + 1 + 1);

						block_version = CM_GET16_NTOH (dp, 0);
						nr_of_peers   = CM_GET8_NTOH (dp, 2);

						cm_dp_assert (block_version == 0x0100);
						cm_dp_assert (nr_of_peers == 1);
						break;

					case CM_BLOCK_TYPE_PD_CHK_LINE_DELAY:
						cm_dp_assert (block_length == 1 + 1 + 1 + 1 + 4);

						block_version = CM_GET16_NTOH (dp, 0);
						pad           = CM_GET16_NTOH (dp, 2);
						linedelay     = CM_GET32_NTOH (dp, 4);

						cm_dp_assert (block_version == 0x0100);
						cm_dp_assert (pad == 0);
						cm_dp_assert (linedelay != 0);
						cm_dp_assert ((linedelay & 0x80000000) == 0);	/* LineDelay.FormatIndicator */

						mtdp->check_linedelay = linedelay;
						break;

					default:
						/* ANY, with version ANY */
						break;
					}

					cm_dp_adjust (dp, left, block_length);
				}

				cm_dp_assert (left == 0);

				mtdp->properties |= CM_PD_MTD_PEER_PROP_I_PARSED;
			}
		}
		break;

	case 0x802c:/* PDIRData */
		{
			LSA_UINT16 block_type, block_length, block_version, pad, slot_nr, subslot_nr;
			LSA_UINT32 max_bridge_delay, nr_of_ports;

			/*	PDIRData [VersionLow == 1]  ::=
			//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, PDIRFrameData, PDIRBeginEndData
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET16_NTOH (dp, 6);
			slot_nr       = CM_GET16_NTOH (dp, 8);
			subslot_nr    = CM_GET16_NTOH (dp, 10);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_IR_DATA);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x0101);
			cm_dp_assert (pad == 0);
			cm_dp_assert (slot_nr == rec->slot_nr);
			cm_dp_assert (subslot_nr == rec->subslot_nr);

			while (left > 0)
			{
				adj = 2 + 2;
				cm_dp_assert (left >= adj);

				block_type   = CM_GET16_NTOH (dp, 0);
				block_length = CM_GET16_NTOH (dp, 2);

				cm_dp_adjust (dp, left, adj);

				cm_dp_assert (left >= block_length);

				switch (block_type)
				{
				case CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA:
					/*	PDIRGlobalData with BlockVersionLow = 1  ::=
					//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, (MaxPortTxDelay, MaxPortRxDelay)*
					//
					//	PDIRGlobalData with BlockVersionLow = 2  ::=
					//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, (MaxPortTxDelay, MaxPortRxDelay, MaxLineRxDelay, YellowTime)*
					*/
					adj = 1 + 1 + 1 + 1 + 16 + 4 + 4;
					cm_dp_assert (block_length >= adj);

					block_version    = CM_GET16_NTOH (dp, 0);
					pad              = CM_GET16_NTOH (dp, 2);
					max_bridge_delay = CM_GET32_NTOH (dp, 20);
					nr_of_ports      = CM_GET32_NTOH (dp, 24);

					cm_dp_assert (block_version == 0x0101 || block_version == 0x0102);
					cm_dp_assert (pad == 0);
					cm_dp_assert (1 <= max_bridge_delay && max_bridge_delay <= 0x0000FFFF);
					cm_dp_assert (1 <= nr_of_ports && nr_of_ports <= 0x000000FF);

					mtd->pdirglobaldata_version = block_version;
					mtd->max_bridge_delay       = max_bridge_delay;

					if (block_version == 0x0101)
					{
						cm_dp_adjust (dp, left, block_length);

						/* calculate later as f(LineDelay), see cm_pd_mpa_tailor_pre_calc_tailor_data() */
					}
					else
					{
						LSA_UINT16 i;
						LSA_UINT32 max_line_rx_delay;

						cm_dp_adjust (dp, left, adj);

						adj = 4 + 4 + 4 + 4;
						for (i = 0; i < nr_of_ports; ++i)
						{
							CM_PD_MTD_PORT_PTR_TYPE mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, i);

							cm_dp_assert (left >= adj);

							max_line_rx_delay = CM_GET32_NTOH (dp, 8);

							cm_dp_adjust (dp, left, adj);
							cm_dp_assert (/* 0x00000000 <= max_line_rx_delay  && */ max_line_rx_delay <= 0x3B9AC9FF);
							/* 0 .. Unknown, No delay, forward as soon as possible */

							if (cm_is_not_null (mtdp))
							{
								mtdp->max_line_rx_delay = max_line_rx_delay;
							}
						}
					}
					break;

				case CM_BLOCK_TYPE_PD_IR_FRAME_DATA:
					/*	PDIRFrameData [VersionLow == 0]  ::=
					//		BlockHeader, Padding, Padding, (FrameSendOffset, ...
					//
					//	PDIRFrameData [VersionLow == 1]  ::=
					//		BlockHeader, Padding, Padding, FrameDataProperties, [ ...
					*/
					cm_dp_assert (block_length >= 1 + 1 + 1 + 1 + 4);

					block_version = CM_GET16_NTOH (dp, 0);
					pad           = CM_GET16_NTOH (dp, 2);

					cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
					cm_dp_assert (pad == 0);

					cm_dp_adjust (dp, left, block_length);
					break;

				case CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA:
					/*	PDIRBeginEndData [VersionLow == 0]  ::=
					//		BlockHeader, Padding, Padding, RedGuard, NumberOfPorts, (...
					*/
					cm_dp_assert (block_length >= 1 + 1 + 1 + 1 + 2 + 2 + 4);

					block_version = CM_GET16_NTOH (dp, 0);
					pad           = CM_GET16_NTOH (dp, 2);

					cm_dp_assert (block_version == 0x0100);

					cm_dp_adjust (dp, left, block_length);
					break;

				default:
					/* ANY, with version ANY */
					cm_dp_adjust (dp, left, block_length);
					break;
				}
			}
		}
		break;

	case 0x802d:/* PDSyncData */
		{
			LSA_UINT16 block_type, block_length, block_version, pad;

			/* PDSyncData [BlockVersionLow = 2] ::=
			//		BlockHeader, Padding, Padding, PTCP_SubdomainUUID, ...
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 16;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET16_NTOH (dp, 6);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_SYNC_DATA);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x102);
			cm_dp_assert (pad == 0);
		}
		break;

	case 0x802f:/* PDPortDataAdjust */
		{
			LSA_UINT16 block_type, block_length, block_version, pad, slot_nr, subslot_nr;

			/* PDPortDataAdjust ::=
			//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
			//		, { [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState], [AdjustPeerToPeerBoundary]
			//		, [AdjustDCPBoundary], [AdjustPreambleLength] }
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET16_NTOH (dp, 6);
			slot_nr       = CM_GET16_NTOH (dp, 8);
			subslot_nr    = CM_GET16_NTOH (dp, 10);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_PORT_DATA_ADJUST);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x0100);
			cm_dp_assert (pad == 0);
			cm_dp_assert (slot_nr == rec->slot_nr);
			cm_dp_assert (subslot_nr == rec->subslot_nr);

			while (left > 0)
			{
				adj = 2 + 2;
				cm_dp_assert (left >= adj);

				block_type   = CM_GET16_NTOH (dp, 0);
				block_length = CM_GET16_NTOH (dp, 2);

				cm_dp_adjust (dp, left, adj);

				cm_dp_assert (left >= block_length);

				switch (block_type)
				{
				case CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH:
					/*	AdjustPreambleLength	::= BlockHeader, Padding, Padding, PreambleLength, AdjustProperties, [Padding*] a
					*/
					cm_dp_assert (block_length == 1 + 1 + 1 + 1 + 2 + 2);

					block_version = CM_GET16_NTOH (dp, 0);

					cm_dp_assert (block_version == 0x0100);
					break;

				default:
					/* ANY, with version ANY */
					break;
				}

				cm_dp_adjust (dp, left, block_length);
			}
		}
		break;

	case 0x8051:/* PDInterfaceMrpDataCheck */
		{
			LSA_UINT16 block_type, block_length, block_version, pad, nr_of_entries;

			/* PDInterface-MrpDataCheck [BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Check
			//
			//	PDInterface-MrpDataCheck [BlockVersionLow = 1]  ::=
			//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataCheckBlock*
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 16 + 4;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET8_NTOH (dp, 6);
			nr_of_entries = CM_GET8_NTOH (dp, 7);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_CHECK);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
			cm_dp_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_dp_assert (nr_of_entries == 0);
			}
			else
			{
				cm_dp_assert (1 <= nr_of_entries && nr_of_entries <= 127);
			}
		}
		break;

	case 0x8052:/* PDInterfaceMrpDataAdjust */
		{
			LSA_UINT16 block_type, block_length, block_version, pad, nr_of_entries;

			/* PDInterface-MrpDataAdjust [BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Role, [Padding*] a
			//		, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
			//
			//	PDInterface-MrpDataAdjust [BlockVersionLow = 1] ::=
			//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataAdjustBlock*
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 16 + 2 + 2 + 1;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET8_NTOH (dp, 6);
			nr_of_entries = CM_GET8_NTOH (dp, 7);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
			cm_dp_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_dp_assert (nr_of_entries == 0);
			}
			else
			{
				cm_dp_assert (1 <= nr_of_entries && nr_of_entries <= 127);
			}
		}
		break;

	case 0x8053:/* PDPortMrpDataAdjust */
		{
			LSA_UINT16 block_type, block_length, block_version, pad, instance;

			/* PDPortMrpDataAdjust with BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID
			//
			//	PDPortMrpDataAdjust [BlockVersionLow = 1]  ::=
			//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID
			*/

			adj = 2 + 2 + 1 + 1 + 1 + 1 + 16;
			cm_dp_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			pad           = CM_GET8_NTOH (dp, 6);
			instance      = CM_GET8_NTOH (dp, 7);

			cm_dp_adjust (dp, left, adj);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_PD_MRP_PORT_ADJ);
			cm_dp_assert (block_length + 2 + 2 == rec->record_data_len);
			cm_dp_assert (block_version == 0x0100 || block_version == 0x0101);
			cm_dp_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_dp_assert (instance == 0);
			}
			else
			{
				cm_dp_assert (/*0 <= instance &&*/ instance <= 126);
			}
		}
		break;

	default:
		break;
	}
	return CM_OK;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "Parse Error in line(%u): index(0x%x) slot(%u) subslot(0x%x), record-offset(%u)"
		, cm_dp_assert_line, rec->record_index, rec->slot_nr, rec->subslot_nr, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_pd_mpa_prm_prealloc_tailor_data (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_PD_RECORD_PTR_TYPE rec
	)
{
	LSA_UINT32 alloc_length;

	switch (rec->record_index)
	{
	case 0x802b:/* PDPortDataCheck */
		alloc_length = rec->record_data_len + 240;	/* NrOfPeers has to be exactly 1, no need to parse CheckPeers */
		break;

	case 0x802f:/* PDPortDataAdjust */
		alloc_length = rec->record_data_len;
		break;

	case 0x802d:/* PDSyncData */
		alloc_length = rec->record_data_len + 240;	/* PTCPSubdomainName */
		break;

	case 0x8051:/* PDInterfaceMrpDataCheck */
		alloc_length = rec->record_data_len;
		break;

	case 0x8052:/* PDInterfaceMrpDataAdjust */
		if (rec->record_data_len > 2 + 2 + 1 + 1 + 1 + 1)
		{
			LSA_UINT16 block_version = CM_GET16_NTOH (rec->record_data, 4);
			LSA_UINT8 nr_of_entries  = CM_GET8_NTOH (rec->record_data, 7);

			if (block_version == 0x0100)
			{
				alloc_length = rec->record_data_len + 240;	/* MRP_DomainName */
			}
			else if (block_version == 0x0101)
			{
				if (1 <= nr_of_entries  && nr_of_entries <= 127)
				{
					alloc_length = rec->record_data_len + nr_of_entries * 240;	/* MRP_DomainName */
				}
				else
				{
					return CM_ERR_RESOURCE;
				}
			}
			else
			{
				return CM_ERR_RESOURCE;
			}
		}
		else
		{
			return CM_ERR_RESOURCE;
		}
		break;

	case 0x8053:/* PDPortMrpDataAdjust */
		alloc_length = rec->record_data_len;
		break;

	case 0x802C:/* PDIRData */
		alloc_length = rec->record_data_len;
		break;

	default:
		alloc_length = 0;
		break;
	}

	if (alloc_length > 0)
	{
		if (alloc_length > 0xFFFF)
		{
			return CM_ERR_RESOURCE;
		}
		if (cm_is_not_null (channel))
		{
			/* usecase cmpd */
			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&rec->tailor_data), cm_null_user_id, alloc_length, channel->sysptr);
		}
		else
		{
			/* usecase cmcl */
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&rec->tailor_data), alloc_length);
		}
		if (cm_is_null (rec->tailor_data))
		{
			return CM_ERR_RESOURCE;
		}
		rec->tailor_data_alloc_len = CM_CAST_U16 (alloc_length);
		rec->tailor_data_len       = CM_TAILOR_DATA_LEN_NOT_TAILORED;
	}
	else
	{
		rec->tailor_data           = LSA_NULL;
		rec->tailor_data_alloc_len = 0;
		rec->tailor_data_len       = CM_TAILOR_DATA_LEN_NOT_TAILORED;
	}

	return CM_OK;
}


/*------------------------------------------------------------------------------
//	PDMasterTailorData
//----------------------------------------------------------------------------*/

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE
cm_pd_mtd_parse (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 local_device_nr,
	LSA_UINT32 record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE record_data
	)
{
	LSA_UINT16 block_type, block_length, block_version, tailor_properties, dev_group, port_cnt;
	LSA_UINT16 rc;
	LSA_UINT16 mtdp_index;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd;
	CM_PD_MTD_PORT_PTR_TYPE ports;
	LSA_UINT16 count_of_fixed;

	LSA_UNUSED_ARG (channel);

	if (record_data_length < 0UL + 2 + 2 + 1 + 1 + 2 + 2 + 2)	/* size without any ports */
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDMasterTailorData: record_data_length(%u)", record_data_length);
		return LSA_NULL;
	}

	block_type        = CM_GET16_NTOH (record_data, 0);
	block_length      = CM_GET16_NTOH (record_data, 2);
	block_version     = CM_GET16_NTOH (record_data, 4);
	tailor_properties = CM_GET16_NTOH (record_data, 6);
	dev_group         = CM_GET16_NTOH (record_data, 8);
	port_cnt          = CM_GET16_NTOH (record_data, 10);

	if (record_data_length < 0UL + 2 + 2 + 1 + 1 + 2 + 2 + 2 + (port_cnt * (2 + 2 + 2 + 2 + 2 + 2)))/* size including ports */
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDMasterTailorData: record_data_length(%u)", record_data_length);
		return LSA_NULL;
	}

	if (block_length + 2 + 2 != (LSA_INT)record_data_length
		|| block_type != 0x7081	/*BlockType(PDMasterTailorData)*/
		|| block_version != 0x0100
		|| dev_group != 0)
	{
		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PDMasterTailorData: block_length(%u) block_type(0x%x) block_version(0x%x) dev_group(0x%x)"
			, block_length, block_type, block_version, dev_group
			);
		return LSA_NULL;
	}

	if ((tailor_properties & CM_PD_MTD_PROP_RESERVED) != 0)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PDMasterTailorData: reserved tailor_properties(0x%x) set", tailor_properties
			);
		return LSA_NULL;
	}

	/* both flags are required CM_PD_MTD_PROP_MRP_DATA_ADJUST + CM_PD_MTD_PROP_MRP_DATA_CHECK */
	if ((tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) != 0 &&
		(tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) != CM_PD_MTD_PROP_I_MRP_DATA_MASK)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PDMasterTailorData: mrp data config invalid, tailor_properties (0x%x)", tailor_properties
			);
		return LSA_NULL;
	}

	count_of_fixed = 0;

	if (port_cnt > 0)
	{
		LSA_UINT16 offset = 2 + 2 + 1 + 1 + 2 + 2 + 2;

		LSA_BOOL mtdp_valid = LSA_TRUE;

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&ports), sizeof (*ports) * port_cnt);
		if (cm_is_null (ports))
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "no local-mem, size(%u)", sizeof (*ports) * port_cnt);
			return LSA_NULL;
		}

		for (mtdp_index = 0; mtdp_index < port_cnt; mtdp_index++)
		{
			CM_PD_MTD_PORT_PTR_TYPE mtdp = &ports[mtdp_index];

			mtdp->local_station_nr = local_device_nr;	/* backlink */
			mtdp->local_slot_nr    = CM_GET16_NTOH (record_data, offset + 0);
			mtdp->local_subslot_nr = CM_GET16_NTOH (record_data, offset + 2);
			mtdp->properties       = CM_GET16_NTOH (record_data, offset + 4);
			mtdp->peer_station_nr  = CM_GET16_NTOH (record_data, offset + 6);
			mtdp->peer_slot_nr     = CM_GET16_NTOH (record_data, offset + 8);
			mtdp->peer_subslot_nr  = CM_GET16_NTOH (record_data, offset + 10);

			mtdp->check_linedelay   = 0;
			mtdp->max_line_rx_delay = 0;

			mtdp->real_station_nr        = 0xFFFF;
			mtdp->real_mtdp              = LSA_NULL;
			mtdp->real_properties        = 0;
			mtdp->real_max_line_rx_delay = 0;

			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_FIXED
				|| (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
			{
				if (mtdp->peer_station_nr > CM_CL_MAX_DEVICE_NR)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_station_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (mtdp->peer_slot_nr > 0x7FFF)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (! CM_IS_PDEV_SUBSLOT_NR (mtdp->peer_subslot_nr))
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				count_of_fixed += 1;
				if (tailor_properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL  &&  count_of_fixed > 2)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: for optional devices at most 2 fixed/optional peers allowed, index(0x%x) properties(0x%x)"
						, mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

			}

			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
				|| (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_ANY)
			{
				if (mtdp->peer_station_nr != 0xFFFF)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_station_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (mtdp->peer_slot_nr != 0xFFFF)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (mtdp->peer_subslot_nr != 0xFFFF)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: peer_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}
			}

			if (mtdp->local_slot_nr > 0x7FFF)
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "PortMasterTailorData: local_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
					);
				mtdp_valid = LSA_FALSE;
			}

			if (! CM_IS_PDEV_SUBSLOT_NR (mtdp->local_subslot_nr))
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "PortMasterTailorData: local_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
					);
				mtdp_valid = LSA_FALSE;
			}

			if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
			{
				if ((tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) == 0)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: tailor properties flag TailorPDInterfaceMrpDataAdjust is required, index(0x%x) properties(0x%x)"
						, mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}
			}

			if (! mtdp_valid)
			{
				CM_FREE_LOCAL_MEM (&rc, ports);
				CM_ASSERT (rc == LSA_RET_OK);
				return LSA_NULL;
			}

			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
			{
				tailor_properties |= CM_PD_MTD_PROP_I_ANY_PEER_OPT;
			}
			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG)
			{
				tailor_properties |= CM_PD_MTD_PROP_I_ANY_PEER_PROG;
			}

			offset += 2 + 2 + 2 + 2 + 2 + 2;
		}
	}
	else
	{
		ports = LSA_NULL;
	}

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&mtd), sizeof (*mtd));
	if (cm_is_null (mtd))
	{
		if (cm_is_not_null (ports))
		{
			CM_FREE_LOCAL_MEM (&rc, ports);
		}
		return LSA_NULL;
	}

	mtd->properties             = tailor_properties;
	mtd->port_cnt               = port_cnt;
	mtd->ports                  = ports;
	mtd->set_by_itd             = 0;
	mtd->pdirglobaldata_version = 0;
	mtd->max_bridge_delay       = 0;
	mtd->frame_id[0]            = 0;
	mtd->frame_id[1]            = 0;

	for (mtdp_index = 0; mtdp_index < port_cnt; mtdp_index++)
	{
		mtd->ports[mtdp_index].mtd = mtd;
	}

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PDMasterTailorData: successfully parsed: tailor_properties(0x%x)", tailor_properties
		);

	return mtd;
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE
cm_pd_mtd_lookup_mtdp (
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
	)
{
	LSA_INT i;
	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

		if (mtdp->local_slot_nr == slot_nr && mtdp->local_subslot_nr == subslot_nr)
		{
			return mtdp;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE
cm_pd_mtd_lookup_mtdp_from_index (
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd,
	LSA_UINT16 index	/* 0 .. (port_cnt - 1) */
	)
{
	LSA_INT i;
	LSA_UINT16 subslot_nr = 0x8000 + ((index + 1) & 0xFF);

	/* SPHi13: 4.4.7.5 Modular switches */

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

		if ((mtdp->local_subslot_nr & 0xF0FF) == subslot_nr)
		{
			return mtdp;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_mtd_neighbour_exists (
	CM_CHANNEL_PTR_TYPE usr_channel,
	CM_PD_MASTER_TAILOR_DATA_CONST_PTR_TYPE mtd
	)
{
	LSA_INT i;

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

		{
			cm_pd_mtdp_get_peer (usr_channel, mtdp);

			if (mtdp->real_station_nr != 0xFFFF && cm_is_null (mtdp->real_mtdp))
			{
				CM_PD_TRACE_02 (usr_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "PreCheck-MTD: dev(%u) needs not (yet) existing neighbour-dev(%u)", mtdp->local_station_nr, mtdp->real_station_nr
					);
				return LSA_FALSE;
			}
		}
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mtd_reset (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd
	)
{
	LSA_INT i;

	mtd->set_by_itd = CM_PD_MTD_SET_IS_TAILORED;

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

		if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG)
		{
			mtdp->peer_station_nr = 0xFFFF;
			mtdp->peer_slot_nr    = 0xFFFF;
			mtdp->peer_subslot_nr = 0xFFFF;
		}

		mtdp->real_station_nr        = 0xFFFF;
		mtdp->real_mtdp              = LSA_NULL;
		mtdp->real_properties        = 0;
		mtdp->real_max_line_rx_delay = 0;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mtd_set (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd,
	LSA_UINT16 set
	)
{
	CM_ASSERT (cm_is_not_null (mtd));

	if (set == CM_PD_MTD_SET_DEVICE_ENABLED)
	{
		mtd->set_by_itd |= CM_PD_MTD_SET_DEVICE_ENABLED;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mtd_peer (
	CM_PD_MTD_PORT_PTR_TYPE mtdp_1,
	CM_PD_MTD_PORT_PTR_TYPE mtdp_2
	)
{
	/* note: do not implizit enable station_nr */

	mtdp_1->peer_station_nr = mtdp_2->local_station_nr;
	mtdp_1->peer_slot_nr    = mtdp_2->local_slot_nr;
	mtdp_1->peer_subslot_nr = mtdp_2->local_subslot_nr;

	mtdp_2->peer_station_nr = mtdp_1->local_station_nr;
	mtdp_2->peer_slot_nr    = mtdp_1->local_slot_nr;
	mtdp_2->peer_subslot_nr = mtdp_1->local_subslot_nr;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mpa_set_real_peer_port_data (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MTD_PORT_PTR_TYPE mtdp
	)
{
	CM_PD_MTD_PORT_PTR_TYPE next_mtdp = mtdp;
	LSA_UINT32 bridged_line_rx_delay  = 0;

	mtdp->real_station_nr        = 0xFFFF;	/* no peer */
	mtdp->real_mtdp              = LSA_NULL;/* not added */
	mtdp->real_properties        = 0;
	mtdp->real_max_line_rx_delay = 0;

	for (;;)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE peer_mtd;
		CM_PD_MTD_PORT_PTR_TYPE peer_mtdp;

		mtdp->real_station_nr = 0xFFFF;

		switch (next_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK)
		{
		case CM_PD_MTD_PEER_PROP_PEER_FIXED:
			CM_ASSERT (next_mtdp->peer_station_nr != 0xFFFF);	/* else a bug, see cm_pd_mtd_parse() */
			break;

		case CM_PD_MTD_PEER_PROP_PEER_OPT:
			CM_ASSERT (next_mtdp->peer_station_nr != 0xFFFF);	/* else a bug, see cm_pd_mtd_parse() */
			break;

		case CM_PD_MTD_PEER_PROP_PEER_PROG:
			/* no range check */
			break;

		case CM_PD_MTD_PEER_PROP_PEER_ANY:
			CM_ASSERT (next_mtdp->peer_station_nr == 0xFFFF);	/* else a bug, see cm_pd_mtd_parse() */
			break;

		default:
			CM_FATAL ();
			break;
		}

		if (next_mtdp->peer_station_nr == 0xFFFF)
		{
			mtdp->real_station_nr = 0xFFFF;	/* no peer */
			return;
		}

		mtdp->real_station_nr = next_mtdp->peer_station_nr;

		peer_mtd = cm_pdusr_mpa_lookup_mtd (channel, next_mtdp->peer_station_nr);

		if (cm_is_null (peer_mtd))
		{
			return;	/* adaption missing, device not added */
		}

		if ((peer_mtd->properties & (CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT))
			&& ! (peer_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
		{
			return;	/* adaption missing, instance tailor data not written */
		}

		peer_mtdp = cm_pd_mtd_lookup_mtdp (peer_mtd, next_mtdp->peer_slot_nr, next_mtdp->peer_subslot_nr);

		if (cm_is_null (peer_mtdp))
		{
			CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "PDInstanceTailorData: local_station(%u), peer_station_nr(%u) peer_slot_nr(%u) peer_subslot_nr(0x%x) not found"
				, next_mtdp->local_station_nr, next_mtdp->peer_station_nr, next_mtdp->peer_slot_nr, next_mtdp->peer_subslot_nr
				);
			return;	/* adaption missing, device not added or MasterTailorData pointing to nonexisting slot/subslot */
		}

		if ((next_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT
			&& (peer_mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED) == 0)
		{
			/* bridged device, find next_mtdp along the path */
			LSA_UINT16 i;

			bridged_line_rx_delay += peer_mtd->max_bridge_delay + next_mtdp->max_line_rx_delay;

			for (i = 0; i < peer_mtd->port_cnt; i++)
			{
				next_mtdp = &peer_mtd->ports[i];

				if (peer_mtdp == next_mtdp)
				{
					continue;
				}

				/* note: optional devices allows only "2 fixed neighbours" (mandatory or optional) */
				if ((next_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_FIXED
					|| (next_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
				{
					break;
				}
			}

			if (i == peer_mtd->port_cnt)
			{
				mtdp->real_station_nr = 0xFFFF;
				return;	/* adaption not necessary, no neighbour */
			}
			/* next_mtdp found, next while-loop */
		}
		else
		{
			CM_ASSERT (mtdp->real_station_nr != 0xFFFF);
			mtdp->real_mtdp       = peer_mtdp;
			mtdp->real_properties = peer_mtdp->properties;

			if (next_mtdp->max_line_rx_delay > 0)
			{
				mtdp->real_max_line_rx_delay = next_mtdp->max_line_rx_delay + bridged_line_rx_delay;
			}
			return;
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_mtdp_get_peer (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_MTD_PORT_PTR_TYPE mtdp
	)
{
	if (cm_is_null (mtdp))
	{
		return;
	}

	switch (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK)
	{
	case CM_PD_MTD_PEER_PROP_PEER_FIXED:
	case CM_PD_MTD_PEER_PROP_PEER_OPT:
	case CM_PD_MTD_PEER_PROP_PEER_PROG:
		/*		if (cm_is_null (mtdp->real_mtdp))		TIA-1602513 real_mtdp is a pointer from an other device, if device is removed this pointer is invalid */
		{
			cm_pd_mpa_set_real_peer_port_data (channel, mtdp);
		}
		break;

	case CM_PD_MTD_PEER_PROP_PEER_ANY:
		mtdp->real_station_nr = 0xFFFF;
		mtdp->real_mtdp       = LSA_NULL;
		break;

	default:
		CM_FATAL ();/* a bug */
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_mtd_free (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE * mtd_ptr_ptr
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = *mtd_ptr_ptr;
	LSA_UINT16 rc;

	if (cm_is_not_null (mtd))
	{
		if (cm_is_not_null (mtd->ports))
		{
			CM_FREE_LOCAL_MEM (&rc, mtd->ports);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_LOCAL_MEM (&rc, mtd);
		CM_ASSERT (rc == LSA_RET_OK);

		*mtd_ptr_ptr = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_pdusr_mpa_is_device_added (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 device_nr
	)
{
	if (device_nr == 0)	/* controller is always added */
	{
		return LSA_TRUE;
	}
	else
	{
		CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (usr_channel);

		if (cm_is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE dev = cm_cl_dev_from_device_nr (cl_channel, device_nr);
			if (cm_is_not_null (dev))
			{
				return LSA_TRUE;
			}
		}

		return LSA_FALSE;
	}
}

/*----------------------------------------------------------------------------*/
CM_PD_MASTER_TAILOR_DATA_PTR_TYPE
cm_pdusr_mpa_lookup_mtd (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 device_nr
	)
{
	if (device_nr == 0)
	{
		CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);
		return pd_channel->usr.pd.reclist.mtd;
	}
	else
	{
		CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (usr_channel);

		if (cm_is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE dev = cm_cl_dev_from_device_nr (cl_channel, device_nr);
			if (cm_is_not_null (dev))
			{
				if (cm_is_not_null (dev->argr))
				{
					return dev->argr->cm.cl.mtd;
				}
			}
		}
		return LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE
cm_pdusr_mpa_lookup_mtdp (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE *mtd_
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (usr_channel, device_nr);
	CM_PD_MTD_PORT_PTR_TYPE mtdp;

	if (cm_is_null (mtd))
	{
		mtdp = LSA_NULL;
	}
	else
	{
		mtdp = cm_pd_mtd_lookup_mtdp (mtd, slot_nr, subslot_nr);
		if (cm_is_null (mtdp))
		{
			mtd = LSA_NULL;
		}
	}
	if (cm_is_not_null (mtd_))
	{
		*mtd_ = mtd;
	}
	return mtdp;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pdusr_mpa_tailor_frame_id (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 rtc3_frame_id
	)
{
	CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (usr_channel);

	if (cm_is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE dev;

		if (CM_REDUNDANT_FRAME_ID_BASE (rtc3_frame_id) != 0)
		{
			rtc3_frame_id = CM_REDUNDANT_FRAME_ID_BASE (rtc3_frame_id);
		}

		for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);
			if (cm_is_not_null (mtd))
			{
				if (mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL
					&&  mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED
					&& ! (mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED))
				{
					if (mtd->frame_id[0] == rtc3_frame_id || mtd->frame_id[1] == rtc3_frame_id)
					{
						return LSA_TRUE;
					}
				}
			}
		}
	}
	return LSA_FALSE;
}


/*------------------------------------------------------------------------------
//	read record: PDIOSystemInfo
//----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_prm_read_PDIOSystemInfo (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_port_id,
	LSA_UINT32  *record_data_length,
	CM_UPPER_MEM_PTR_TYPE record_data
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;
	CM_CHANNEL_PTR_TYPE cl_channel;
	LSA_UINT32 left = *record_data_length;
	LSA_UINT16 device_nr, device_nr_max;
	LSA_UINT32 adj;

	if (edd_port_id != 0 /*interface*/)
	{
		return CM_ERR_PRM_INDEX;
	}

	cl_channel = cm_cl_channel_get (channel);

	if (cm_is_not_null (cl_channel))
	{
		device_nr_max = CM_CL_MAX_DEVICE_NR;
	}
	else
	{
		device_nr_max = 0;
	}

	/*	PDIOSystemInfo ::= BlockHeader, IOSystemProperties, NumberOfDevices, (StationNumber, DeviceProperties)*
	//	IOSystemProperties
	//		Bit 0: HasOptionalRTC3Devices
	//	DeviceProperties
	//		Bit 0: IsPartOfAddressTailoring
	//		Bit 1: IsOptional
	//		Bit 2: HasFixedPortToOptionalNeighbor
	//		Bit 3: HasProgrammablePeer
	*/

	{
		LSA_UINT16 number_of_devices   = 0;
		LSA_UINT16 iosystem_properties = 0;

		adj = 2 + 2 + 1 + 1 + 2 + 2;
		cm_dp_assert (left >= adj);

		CM_PUT16_HTON (dp, 0, 0x7082 /* BlockType: PDIOSystemInfo */);
		CM_PUT16_HTON (dp, 2, 0xCCCC);
		CM_PUT16_HTON (dp, 4, 0x0100);

		CM_PUT16_HTON (dp, 6, 0xCCCC);
		CM_PUT16_HTON (dp, 8, 0xCCCC);

		cm_dp_adjust (dp, left, adj);

		for (device_nr = 0; device_nr <= device_nr_max; ++device_nr)
		{
			if (cm_pdusr_mpa_is_device_added (channel, device_nr))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (channel, device_nr);
				LSA_UINT16 device_properties          = 0;

				if (cm_is_not_null (mtd))
				{
					device_properties |= (mtd->properties & CM_PD_MTD_PROP_NOS) ? 0x01 : 0;
					device_properties |= (mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) ? 0x02 : 0;
					device_properties |= (mtd->properties & CM_PD_MTD_PROP_I_ANY_PEER_OPT) ? 0x04 : 0;
					device_properties |= (mtd->properties & CM_PD_MTD_PROP_I_ANY_PEER_PROG) ? 0x08 : 0;

					iosystem_properties |= ((device_nr == 0) && (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)) ? 0x0001 : 0;
				}

				number_of_devices += 1;

				cm_dp_assert (left > 2 + 2);

				CM_PUT16_HTON (dp, 0, device_nr);
				CM_PUT16_HTON (dp, 2, device_properties);

				cm_dp_adjust (dp, left, 2 + 2);
			}
		}

		dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;

		*record_data_length -= left;
		CM_PUT16_HTON (dp, 2, CM_CAST_U16 (*record_data_length) - (2 + 2));
		CM_PUT16_HTON (dp, 6, iosystem_properties);
		CM_PUT16_HTON (dp, 8, number_of_devices);
	}

	return CM_OK;

cm_dp_assert_failed:
	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDIOSystemInfo: failed, buffer to small, line(%u), given-data_length(%u)"
		, cm_dp_assert_line, record_data_length
		);
	return CM_ERR_PRM_DATA;
}


/*------------------------------------------------------------------------------
//	PDInstanceTailorData
//----------------------------------------------------------------------------*/

CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE
cm_pd_itd_parse (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE record_data,
	LSA_UINT32  *pnio_err
	)
{
	CM_UPPER_MEM_U8_PTR_TYPE dp = record_data;
	LSA_UINT32 left             = record_data_length;
	LSA_UINT16 cm_dp_assert_line;
	LSA_UINT32 cm_dp_assert_error;
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE itd = LSA_NULL;

	/* see PDInstanceTailorData */

	LSA_UINT16 nr_of_devices, nr_of_peers;

	{
		LSA_UINT16 block_type, block_length, block_version, pad1, pad2, device_group;

		cm_dp_assert2 (left >= 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2, CM_PNIO_FAULTY_PDInstanceTailor (0));

		block_type    = CM_GET16_NTOH (dp, 0);
		block_length  = CM_GET16_NTOH (dp, 2);
		block_version = CM_GET16_NTOH (dp, 4);
		pad1          = CM_GET8_NTOH (dp, 6);
		pad2          = CM_GET8_NTOH (dp, 7);

		device_group  = CM_GET16_NTOH (dp, 8);
		nr_of_devices = CM_GET16_NTOH (dp, 10);

		cm_dp_adjust (dp, left, 2 + 2 + 1 + 1 + 1 + 1 + 2 + 2);

		cm_dp_assert2 (block_type == 0x7080, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (CM_CAST_U32 (block_length + 2 + 2) == record_data_length, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (block_version == 0x0100, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (pad1 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (pad2 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (device_group == 0, CM_PNIO_FAULTY_PDInstanceTailor (1));
		cm_dp_assert2 (CM_CAST_U32 (nr_of_devices * (2 + 1 + 1)) <= left, CM_PNIO_FAULTY_PDInstanceTailor (0));
	}

	{
		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&itd), sizeof (*itd));
		cm_dp_assert2 (cm_is_not_null (itd), CM_PNIO_FAULTY_PDInstanceTailor (0));

		itd->nr_of_devices = nr_of_devices;
		itd->devices       = LSA_NULL;
		itd->nr_of_peers   = 0;
		itd->peers         = LSA_NULL;

		if (nr_of_devices > 0)
		{
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&itd->devices), nr_of_devices * sizeof (LSA_UINT16));
			cm_dp_assert2 (cm_is_not_null (itd->devices), CM_PNIO_FAULTY_PDInstanceTailor (0));
		}
	}

	{
		LSA_UINT16 i;

		for (i = 0; i < nr_of_devices; ++i)
		{
			LSA_UINT16 device_nr, pad1, pad2;

			cm_dp_assert2 (left >= 2 + 1 + 1, CM_PNIO_FAULTY_PDInstanceTailor (0));

			device_nr = CM_GET16_NTOH (dp, 0);
			pad1      = CM_GET8_NTOH (dp, 2);
			pad2      = CM_GET8_NTOH (dp, 3);

			cm_dp_adjust (dp, left, 2 + 1 + 1);

			cm_dp_assert2 (/*0 <= device_nr &&*/ device_nr <= CM_CL_MAX_DEVICE_NR, CM_PNIO_FAULTY_PDInstanceTailor (2));
			cm_dp_assert2 (pad1 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
			cm_dp_assert2 (pad2 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));

			itd->devices[i] = device_nr;
		}
	}

	if (left > 0)
	{
		LSA_UINT16 pad1, pad2;

		cm_dp_assert2 (left >= 1 + 1 + 2, CM_PNIO_FAULTY_PDInstanceTailor (0));

		pad1        = CM_GET8_NTOH (dp, 0);
		pad2        = CM_GET8_NTOH (dp, 1);
		nr_of_peers = CM_GET16_NTOH (dp, 2);

		cm_dp_adjust (dp, left, 1 + 1 + 2);

		cm_dp_assert2 (pad1 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (pad2 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (nr_of_peers != 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
		cm_dp_assert2 (CM_CAST_U32 (nr_of_peers * ((2 + 2 + 2 + 1 + 1) + (2 + 2 + 2 + 1 + 1))) <= left, CM_PNIO_FAULTY_PDInstanceTailor (0));

		itd->nr_of_peers = nr_of_peers;

		if (nr_of_peers > 0)
		{
			LSA_UINT16 i;

			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&itd->peers), nr_of_peers * sizeof (CM_PD_PORT_INSTANCE_PEER_TYPE));
			cm_dp_assert2 (cm_is_not_null (itd->peers), CM_PNIO_FAULTY_PDInstanceTailor (0));

			for (i = 0; i < nr_of_peers; ++i)
			{
				LSA_UINT16 device_nr_1, slot_nr_1, subslot_nr_1, device_nr_2, slot_nr_2, subslot_nr_2;
				LSA_UINT16 pad1_1, pad2_1, pad1_2, pad2_2;

				cm_dp_assert2 (left >= ((2 + 2 + 2 + 1 + 1) + (2 + 2 + 2 + 1 + 1)), CM_PNIO_FAULTY_PDInstanceTailor (0));

				device_nr_1  = CM_GET16_NTOH (dp, 0);
				slot_nr_1    = CM_GET16_NTOH (dp, 2);
				subslot_nr_1 = CM_GET16_NTOH (dp, 4);
				pad1_1       = CM_GET8_NTOH (dp, 6);
				pad2_1       = CM_GET8_NTOH (dp, 7);

				device_nr_2  = CM_GET16_NTOH (dp, 8);
				slot_nr_2    = CM_GET16_NTOH (dp, 10);
				subslot_nr_2 = CM_GET16_NTOH (dp, 12);
				pad1_2       = CM_GET8_NTOH (dp, 14);
				pad2_2       = CM_GET8_NTOH (dp, 15);

				cm_dp_adjust (dp, left, ((2 + 2 + 2 + 1 + 1) + (2 + 2 + 2 + 1 + 1)));

				cm_dp_assert2 (/*0 <= device_nr_1 &&*/ device_nr_1 <= CM_CL_MAX_DEVICE_NR, CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (/*0 <= slot_nr_1 &&*/ slot_nr_1 <= 0x7FFF, CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (CM_IS_PDEV_SUBSLOT_NR (subslot_nr_1), CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (pad1_1 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
				cm_dp_assert2 (pad2_1 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));

				cm_dp_assert2 (/*0 <= device_nr_2 &&*/ device_nr_2 <= CM_CL_MAX_DEVICE_NR, CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (/*0 <= slot_nr_2 &&*/ slot_nr_2 <= 0x7FFF, CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (CM_IS_PDEV_SUBSLOT_NR (subslot_nr_2), CM_PNIO_FAULTY_PDInstanceTailor (3));
				cm_dp_assert2 (pad1_2 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));
				cm_dp_assert2 (pad2_2 == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));

				cm_dp_assert2 (device_nr_1 != device_nr_2, CM_PNIO_FAULTY_PDInstanceTailor (3));

				itd->peers[i].device_nr_1  = device_nr_1;
				itd->peers[i].slot_nr_1    = slot_nr_1;
				itd->peers[i].subslot_nr_1 = subslot_nr_1;
				itd->peers[i].device_nr_2  = device_nr_2;
				itd->peers[i].slot_nr_2    = slot_nr_2;
				itd->peers[i].subslot_nr_2 = subslot_nr_2;
			}
		}
	}

	cm_dp_assert2 (left == 0, CM_PNIO_FAULTY_PDInstanceTailor (0));

	*pnio_err = CM_PNIO_ERR_NONE;
	return itd;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInstanceTailortData: parsing failed, line(%u), record-offset(%u) ErrorCode(0x%x)"
		, cm_dp_assert_line, (dp - record_data), cm_dp_assert_error
		);
	cm_pd_itd_free (&itd);
	*pnio_err = cm_dp_assert_error;
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_itd_free (
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE *itd
	)
{
	if (cm_is_not_null (*itd))
	{
		LSA_UINT16 rc;

		if (cm_is_not_null ((*itd)->devices))
		{
			CM_FREE_LOCAL_MEM (&rc, (*itd)->devices);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		if (cm_is_not_null ((*itd)->peers))
		{
			CM_FREE_LOCAL_MEM (&rc, (*itd)->peers);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_LOCAL_MEM (&rc, (*itd));
		CM_ASSERT (rc == LSA_RET_OK);

		*itd = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32
cm_pd_itd_parsed_data_has_error (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_INSTANCE_TAILOR_DATA_CONST_PTR_TYPE itd
	)
{
	LSA_UINT16 cm_dp_assert_line;
	LSA_UINT32 cm_dp_assert_error;
	LSA_INT i;
	CM_CHANNEL_PTR_TYPE cl_channel           = cm_cl_channel_get (channel);
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /* PDev */);

	cm_dp_assert2 (cm_is_not_null (cl_channel), CM_PNIO_FAULTY_PDInstanceTailor (2)	/* no device found */);

	cm_dp_assert2 (cm_is_not_null (pd_mtd), CM_PNIO_FAULTY_Record__AccessDenied (2)	/* PDMasterTailorData not available */);

	/* itd => dev */
	for (i = 0; i < itd->nr_of_devices; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, itd->devices[i]);

		cm_dp_assert2 (cm_is_not_null (mtd), CM_PNIO_FAULTY_PDInstanceTailor (2) /* device/mtd not found */);
		cm_dp_assert2 ((mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL), CM_PNIO_FAULTY_PDInstanceTailor (2) /* enabled device not optional */);
	}

	for (i = 0; i < itd->nr_of_peers; ++i)
	{
		CM_PD_PORT_INSTANCE_PEER_PTR_TYPE itdp = &itd->peers[i];
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd_1, mtd_2;
		CM_PD_MTD_PORT_PTR_TYPE mtdp_1 = cm_pdusr_mpa_lookup_mtdp (cl_channel, itdp->device_nr_1, itdp->slot_nr_1, itdp->subslot_nr_1, &mtd_1);
		CM_PD_MTD_PORT_PTR_TYPE mtdp_2 = cm_pdusr_mpa_lookup_mtdp (cl_channel, itdp->device_nr_2, itdp->slot_nr_2, itdp->subslot_nr_2, &mtd_2);

		cm_dp_assert2 (cm_is_not_null (mtdp_1), CM_PNIO_FAULTY_PDInstanceTailor (3)	/* device/slot/subslot not found */);
		cm_dp_assert2 (cm_is_not_null (mtdp_2), CM_PNIO_FAULTY_PDInstanceTailor (3)	/* device/slot/subslot not found */);

		cm_dp_assert2 ((mtdp_1->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
			, CM_PNIO_FAULTY_PDInstanceTailor (3)/* peer-propertie not programable */
			)
		cm_dp_assert2 ((mtdp_2->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
			, CM_PNIO_FAULTY_PDInstanceTailor (3)	/* peer-propertie not programable */
			)

		{
			LSA_INT p;
			LSA_BOOL found_1, found_2;

			found_1 = (mtd_1->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) ? LSA_FALSE : LSA_TRUE /* mandatory */;
			found_2 = (mtd_2->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) ? LSA_FALSE : LSA_TRUE /* mandatory */;

			for (p = 0; p < itd->nr_of_devices; ++p)
			{
				if (itd->devices[p] == itdp->device_nr_1)
				{
					found_1 = LSA_TRUE;
				}
				if (itd->devices[p] == itdp->device_nr_2)
				{
					found_2 = LSA_TRUE;
				}
			}
			cm_dp_assert2 (found_1, CM_PNIO_FAULTY_PDInstanceTailor (3)	/* programed device_1 disabled */);
			cm_dp_assert2 (found_2, CM_PNIO_FAULTY_PDInstanceTailor (3)	/* programed device_2 disabled */);
		}
	}

	/* pdev */
	{
		LSA_INT p;
		for (p = 0; p < pd_mtd->port_cnt; ++p)
		{
			CM_PD_MTD_PORT_PTR_TYPE mtdp = &pd_mtd->ports[p];

			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE peer_mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, mtdp->peer_station_nr);

				cm_dp_assert2 (cm_is_not_null (peer_mtd), CM_PNIO_FAULTY_PDInstanceTailor (2) /* optional peer-device/mtd not found */);
			}
		}
	}

	/* dev */
	{
		CM_CL_DEVICE_PTR_TYPE dev;

		for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (cm_is_not_null (mtd))
			{
				LSA_INT p;
				for (p = 0; p < mtd->port_cnt; ++p)
				{
					CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[p];

					if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
					{
						CM_PD_MASTER_TAILOR_DATA_PTR_TYPE peer_mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, mtdp->peer_station_nr);

						cm_dp_assert2 (cm_is_not_null (peer_mtd), CM_PNIO_FAULTY_PDInstanceTailor (2) /* optional peer-device/mtd not found */);
					}
				}
			}
		}
	}

	return CM_PNIO_ERR_NONE;

cm_dp_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_dp_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInstanceTailortData: checking failed, line(%u), ErrorCode2(0x%x)", cm_dp_assert_line, cm_dp_assert_error
		);
	return cm_dp_assert_error;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32
cm_pd_mpa_tailor_abort_cmcl_and_set_running (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /*PDev*/);
	CM_CHANNEL_PTR_TYPE cl_channel           = cm_cl_channel_get (channel);
	LSA_UINT16 abort_properties              = CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG;
	LSA_BOOL abort_all = LSA_FALSE;

	/* part 1: check if any PDev or dev is concerned */
	{
		LSA_BOOL abort_mtd = LSA_FALSE;

		if (cm_is_not_null (pd_mtd))
		{
			if ((pd_mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA) != 0)
			{
				abort_all = LSA_TRUE;
				abort_mtd = LSA_TRUE;
			}
			else if ((pd_mtd->properties & abort_properties) != 0)
			{
				abort_mtd = LSA_TRUE;
			}
		}

		if (! abort_mtd && cm_is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE dev;
			for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);
				if (cm_is_not_null (mtd) && ((mtd->properties & abort_properties) != 0))
				{
					abort_mtd = LSA_TRUE;
					break;
				}
			}
		}

		if (! abort_mtd)
		{
			return CM_PNIO_FAULTY_PDInstanceTailor (0);	/* no mtd abort-reason exists */
		}
	}

	/* part 2: set PDInstanceTailorData running to prevent restart of devices */
	{
		channel->usr.pd.reclist.is_running_mpa = LSA_TRUE;
	}

	/* part 3: reset and abort */
	{
		if (cm_is_not_null (pd_mtd) && (abort_all || (pd_mtd->properties & abort_properties) != 0))
		{
			cm_pd_mtd_reset (pd_mtd);
		}
		if (cm_is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE dev;
			for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

				if (abort_all)
				{
					if (cm_is_not_null (mtd))
					{
						cm_pd_mtd_reset (mtd);
					}
					cm_cl_dev_abort (dev, CM_AR_REASON_PDEV);
				}
				else if (cm_is_not_null (mtd) && (mtd->properties & abort_properties) != 0)
				{
					cm_pd_mtd_reset (mtd);
					cm_cl_dev_abort (dev, CM_AR_REASON_PDEV);
				}
			}
		}
	}

	return CM_PNIO_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mpa_tailor_instance_tailor_data (
	CM_CHANNEL_PTR_TYPE channel,
	CM_PD_INSTANCE_TAILOR_DATA_CONST_PTR_TYPE itd
	)
{
	LSA_INT i;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /*PDev*/);

	cm_pd_mtd_set (pd_mtd, CM_PD_MTD_SET_DEVICE_ENABLED);

	for (i = 0; i < itd->nr_of_devices; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (channel, itd->devices[i]);

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PDInstanceTailorData: enable dev(%u)", itd->devices[i]
			);
		cm_pd_mtd_set (mtd, CM_PD_MTD_SET_DEVICE_ENABLED);
	}

	for (i = 0; i < itd->nr_of_peers; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd_1;
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd_2;
		CM_PD_PORT_INSTANCE_PEER_PTR_TYPE itdp = &itd->peers[i];
		CM_PD_MTD_PORT_PTR_TYPE mtdp_1         = cm_pdusr_mpa_lookup_mtdp (channel, itdp->device_nr_1, itdp->slot_nr_1, itdp->subslot_nr_1, &mtd_1);
		CM_PD_MTD_PORT_PTR_TYPE mtdp_2         = cm_pdusr_mpa_lookup_mtdp (channel, itdp->device_nr_2, itdp->slot_nr_2, itdp->subslot_nr_2, &mtd_2);

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PDInstanceTailorData: peer dev(%u) subslot(0x%x)  to  dev(%u) subslot(0x%x)"
			, itdp->device_nr_1, itdp->subslot_nr_1, itdp->device_nr_2, itdp->subslot_nr_2
			);

		cm_pd_mtd_peer (mtdp_1, mtdp_2);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_mpa_tailor_pre_calc_tailor_data (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /*PDev*/);
	CM_CHANNEL_PTR_TYPE cl_channel           = cm_cl_channel_get (channel);
	LSA_UINT16 i;

	/* pre-calc max_line_rx_delay for PDIRGlobalData [BlockVersionLow = 1] */
	if (cm_is_not_null (pd_mtd))
	{
		if (pd_mtd->pdirglobaldata_version == 0x0101)
		{
			for (i = 0; i < pd_mtd->port_cnt; ++i)
			{
				CM_PD_MTD_PORT_PTR_TYPE mtdp = &pd_mtd->ports[i];

				mtdp->max_line_rx_delay = 1000 + mtdp->check_linedelay - 50;
			}
		}
	}
	if (cm_is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE dev;

		for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (cm_is_not_null (mtd) && (mtd->pdirglobaldata_version == 0x0101))
			{
				for (i = 0; i < mtd->port_cnt; ++i)
				{
					CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

					mtdp->max_line_rx_delay = 1000 + mtdp->check_linedelay - 50;
				}
			}
		}
	}

	if (cm_is_not_null (pd_mtd))
	{
		for (i = 0; i < pd_mtd->port_cnt; ++i)
		{
			cm_pd_mpa_set_real_peer_port_data (channel, &pd_mtd->ports[i]);
		}
	}

	if (cm_is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE dev;

		for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (cm_is_not_null (mtd)
				&& ((mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) == 0 || mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED))
			{
				for (i = 0; i < mtd->port_cnt; ++i)
				{
					cm_pd_mpa_set_real_peer_port_data (channel, &mtd->ports[i]);
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_write_PDInstanceTailorData (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE mpa_rb
	)
{
	CM_PD_RECLIST_PTR_TYPE reclist          = &channel->usr.pd.reclist;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr  = mpa_rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE port_if             = cm_pd_port_if__nn (channel);
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE itd = LSA_NULL;

	/*	parse record PDInstaceTailorData
	//	check parsed info, e.g. device_nr exists
	//	note: no cnf(-) allowed after this step
	//	set schedule-abort-reason (is_running_mpa)
	//		abort all IODs targed by PDInstanceTailorData
	//		reset IODs (and PDev) to PDInstanceTailorData unknown
	//		set new PDInstanceTailorData
	//		pre-calc some information needed by cm_pd_mpa_prm_write_tailor_data()
	//		prm-trigger to PDev if targed by PDInstanceTailorData
	//	clear schedule-abort-reason
	//	PDInstaceTailorData.cnf(+)
	*/

	CM_ASSERT (prm_rdwr->cm_pnio_err == CM_PNIO_ERR_NONE);

	/* additional parameter check */
	if (port_if->addr.slot_nr != prm_rdwr->slot_nr || port_if->addr.subslot_nr != prm_rdwr->subslot_nr)
	{
		prm_rdwr->cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidSlotSubslot (0);
	}
	else if (reclist->is_running_mpa)
	{
		prm_rdwr->cm_pnio_err = CM_PNIO_FAULTY_PDInstanceTailor (0);
	}
	else
	{
		itd = cm_pd_itd_parse (channel, prm_rdwr->record_data_length, prm_rdwr->record_data, &prm_rdwr->cm_pnio_err);

		if (cm_is_not_null (itd))
		{
			prm_rdwr->cm_pnio_err = cm_pd_itd_parsed_data_has_error (channel, itd);

			if (prm_rdwr->cm_pnio_err == CM_PNIO_ERR_NONE)
			{
				prm_rdwr->cm_pnio_err = cm_pd_mpa_tailor_abort_cmcl_and_set_running (channel);

				if (prm_rdwr->cm_pnio_err == CM_PNIO_ERR_NONE)
				{
					CM_ASSERT (reclist->is_running_mpa);/* no more cnf(-) occur */

					cm_pd_mpa_tailor_instance_tailor_data (channel, itd);

					cm_pd_mpa_tailor_pre_calc_tailor_data (channel);

					channel->usr.pd.reclist.mpa_rb = mpa_rb;/* store for callback */

					cm_pd_prm_write_PDInstance_TailorData_trigger (channel);

					cm_pd_itd_free (&itd);
					return;
				}
			}
		}
	}

	if (cm_is_not_null (itd))
	{
		cm_pd_itd_free (&itd);
	}
	cm_pd_req_callback (channel, CM_OK, mpa_rb);/* OK but cm_pnio_err */
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_write_PDInstance_TailorData_trigger (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	if (channel->usr.pd.reclist.is_running_mpa)
	{
		CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (channel);

		if (cm_is_not_null (cl_channel))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /*PDev*/);
			LSA_UINT16 abort_properties              = CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT |
													   CM_PD_MTD_PROP_I_ANY_PEER_PROG;
			LSA_BOOL abort_all = LSA_FALSE;
			CM_CL_DEVICE_PTR_TYPE dev;

			if (cm_is_not_null (pd_mtd) && ((pd_mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA) != 0))
			{
				abort_all = LSA_TRUE;
			}

			for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

				if (abort_all || (cm_is_not_null (mtd) && (mtd->properties & abort_properties) != 0))
				{
					CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

					if (cm_is_not_null (argr))
					{
						if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_FRAMES_PASSIVATE) ||
							CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_FRAMES_PASSIVATE_DONE))
						{
							CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev (%u) not yet passivated", dev->device_nr);
							return;
						}
					}
				}
			}
		}

		/* trigger a new PDev-parameterization */
		cm_pd_config_start (channel, CM_PD_CONFIG_MACHINE_TAILOR);

		{
			CM_UPPER_RQB_PTR_TYPE mpa_rb = channel->usr.pd.reclist.mpa_rb;

			channel->usr.pd.reclist.mpa_rb = LSA_NULL;

			channel->usr.pd.reclist.is_running_mpa = LSA_FALSE;

			cm_pd_req_callback (channel, CM_OK, mpa_rb);
		}
	}
}

/*------------------------------------------------------------------------------
//	StartupStatisticsData
//----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_prm_read_StartupStatisticsData (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 *record_data_length,
	CM_UPPER_MEM_PTR_TYPE record_data
	)
{
	LSA_UINT16 cm_dp_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;
	CM_CHANNEL_PTR_TYPE cl_channel;
	CM_CL_DEVICE_PTR_TYPE dev;
	LSA_UINT32 left = *record_data_length;
	LSA_UINT32 adj;
	LSA_UINT16 number_of_sudev = 0;

	cl_channel = cm_cl_channel_get (channel);
	if (cm_is_null (cl_channel))
	{
		/* ERROR: Could not find the peer channel */
		return CM_ERR_PRM_INDEX;
	}

	/*
	StartupStatisticsData	::=	BlockHeader, NumberOfSUStatisticDevice, SUStatisticDevice*

	SUStatisticDevice		::=	DeviceNr, Padding*6,
	FSUStatisticData, ASUStatisticData, OtherStatisticData

	FSUStatisticData,
	ASUStatisticData,
	OtherStatisticData
	::=	NumberOfSU, NumberOfAbort, MinTime, MaxTime,
	TotalTime,
	NareTime, ConnectTime, PrmTime, ArdyTime, InDataTime
	*/

	adj = 2 + 2 + 1 + 1 + 2;
	cm_dp_assert (left >= adj);

	/* BlockHeader */
	CM_PUT16_HTON (dp, 0, 0x708F);		/* BlockType: StartupStatisticsData */
	CM_PUT16_HTON (dp, 2, 0xCCCC);		/* placeholder for block-length without 'type' and 'length', is filled at the end */
	CM_PUT16_HTON (dp, 4, 0x0100);		/* version, high=major, low=minor */

	CM_PUT16_HTON (dp, 6, 0xCCCC);		/* placeholder for NumberOfSUStatisticDevice, is filled at the end */

	cm_dp_adjust (dp, left, adj);

	for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		if (cm_is_not_null (argr) && cm_is_not_null (argr->cm.cl.sustat))
		{
			CM_CL_SUSTAT_DATA_PTR_TYPE sustat = argr->cm.cl.sustat;			/* shortcut */
			LSA_UINT16 i;
			number_of_sudev += 1;

			cm_dp_assert (left > 2 + 2 + 4);

			CM_PUT16_HTON (dp, 0, argr->cm.cl.dev->device_nr);	/* DeviceNr */
			CM_PUT16_HTON (dp, 2, 0);							/* 2x padding */
			CM_PUT32_HTON (dp, 4, 0);							/* 4x padding */

			cm_dp_adjust (dp, left, 2 + 2 + 4);

			for (i = 0; i < CM_CL_SUSTAT_NUMBER_OF_DATASETS; ++i)
			{
				CM_CL_SUSTAT_RECORD_PTR_TYPE sum = &sustat->record[i];
				CM_CL_SUSTAT_TIME_PTR_TYPE time  = &sum->time;
				LSA_UINT64 total_time            = (time->nare + time->connect + time->prm + time->ardy + time->indata);

				cm_dp_assert (left > 4 + 4 + 4 + 4 + 8 + 8 + 8 + 8 + 8 + 8);

				CM_PUT32_HTON (dp, 0, sum->number_of_su);		/* number of startups */
				CM_PUT32_HTON (dp, 4, sum->number_of_aborts);	/* number of aborts */

				CM_PUT32_HTON (dp, 8, sum->min_time);			/* min time */
				CM_PUT32_HTON (dp, 12, sum->max_time);			/* max time */

				CM_PUT64_HTON (dp, 16, total_time);				/* total time */
				CM_PUT64_HTON (dp, 24, time->nare);						/* nare time */
				CM_PUT64_HTON (dp, 32, time->connect);					/* connect time */
				CM_PUT64_HTON (dp, 40, time->prm);						/* prm time */
				CM_PUT64_HTON (dp, 48, time->ardy);						/* ardy time */
				CM_PUT64_HTON (dp, 56, time->indata);					/* indata time */

				cm_dp_adjust (dp, left, 4 + 4 + 4 + 4 + 8 + 8 + 8 + 8 + 8 + 8);
			}
		}
	}

	*record_data_length -= left;

	/* fill the placeholders */
	dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;
	CM_PUT16_HTON (dp, 2, CM_CAST_U16 (*record_data_length) - (2 + 2));	/* block-length without 'type' and 'length' */
	CM_PUT16_HTON (dp, 6, number_of_sudev);	/* NumberOfSUStatisticDevice */

	return CM_OK;

cm_dp_assert_failed:
	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "StartupStatistic: failed, buffer to small, line(%u), given-data_length(%u)"
		, cm_dp_assert_line, record_data_length
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_pd_prm_write_StartupStatisticsData (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_CHANNEL_PTR_TYPE cl_channel = cm_cl_channel_get (channel);
	CM_CL_DEVICE_PTR_TYPE dev;

	if (cm_is_null (cl_channel))
	{
		/* ERROR: Could not find the peer channel */
		return CM_ERR_PRM_INDEX;
	}

	for (dev = cm_cl_dev_first (cl_channel); cm_is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
	{
		CM_AR_GRAPH_PTR_TYPE argr = dev->argr;

		if (cm_is_not_null (argr) && cm_is_not_null (argr->cm.cl.sustat))
		{
			/* reset statistical data */
			cm_cl_ar_sustat_init (argr);
		}
	}

	return CM_OK;
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
