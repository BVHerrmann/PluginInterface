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
/*  F i l e               &F: cm_cl31.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for rpc / prm / records                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   631
#define CM_MODULE_ID        631

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


static LSA_BOOL
cm_cl_prm_record_write (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE cm
	);

static LSA_BOOL
cm_cl_prm_request_next (
	CM_AR_GRAPH_PTR_TYPE argr
	);

static LSA_VOID
cm_cl_record_read_write (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE argr
	);

static CM_UPPER_RQB_PTR_TYPE
cm_cl_prm_rqb_alloc (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	);

static LSA_VOID
cm_cl_prm_rqb_free (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE upper
	);

/*----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
// cm_cl_prm - interface
//
//		cm_cl_prm_request ()
//		cm_cl_prm_abort ()
//		=>  cm_cl_prm_start_cnf
//		=>	cm_cl_prm_begin_ind
//		=>	cm_cl_prm_end_req
//		=>	cm_cl_prm_up_ind
//		=>	cm_cl_prm_idle_ind
//		=>	cm_cl_prm_down_ind
//
//	prm-read:
//		cm_cl_prm_read_start ()		// for CMITimeout-retrigger
//		cm_cl_prm_read_stop ()
//		=> cm_cl_prm_read_timeout ()
//
//	prm-ardy
//		cm_cl_prm_ardy_timeout_start ()		// for Remote_Application_Ready_Timeout // for Advanced-PreDataInd
//		cm_cl_prm_ardy_timeout_stop ()
//		=> cm_cl_prm_ardy_timeout ()
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_sub_state_must_prmed (
	LSA_UINT16 sub_state
	)
{
	switch (sub_state & CM_SUB_STATE_IDENT_MASK)
	{
	case CM_SUB_STATE_IDENT_OK:                 break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:         break;
	case CM_SUB_STATE_IDENT_WRONG:              return LSA_FALSE;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:       return LSA_FALSE;
	default: CM_FATAL ();                       return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK)
	{
	case CM_SUB_STATE_AR_INFO_OWNED:                break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:   break;	/* AP00221324 */
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED: return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:  return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:  return LSA_FALSE;
	default: CM_FATAL ();                           return LSA_FALSE;
	}

	return LSA_TRUE;
}

LSA_BOOL
cm_cl_sub_state_prmed_effect (
	LSA_UINT16 sub_state
	)
{
	switch (sub_state & CM_SUB_STATE_IDENT_MASK)
	{
	case CM_SUB_STATE_IDENT_OK:                 break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:         break;
	case CM_SUB_STATE_IDENT_WRONG:              return LSA_FALSE;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:       return LSA_FALSE;
	default: CM_FATAL ();                       return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK)
	{
	case CM_SUB_STATE_AR_INFO_OWNED:                break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:   return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED: return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:  return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:  return LSA_FALSE;
	default: CM_FATAL ();                           return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_init (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CL_AR_PRM_PTR_TYPE prm = &argr->cm.cl.prm;

	prm->state         = CM_CL_PRM_STATE_DOWN;
	prm->prm_seq       = CM_CL_PRMMODE_IDLE;
	prm->prm_seq_next  = CM_CL_PRMMODE_IDLE;
	prm->prm_seq_next2 = CM_CL_PRMMODE_IDLE;

	CmListInitialize        (&prm->prm_rec_list);
	CmListInitialize        (&prm->prm_submod_list);
	CmListInitialize        (&prm->prm_multi_check_cnf_list);

	prm->prm_keep_plug = LSA_NULL;
	prm->prm_keep_upd  = LSA_NULL;

	prm->submod.api        = 0;
	prm->submod.slot_nr    = 0;
	prm->submod.subslot_nr = 0;

	prm->read_state   = 0;
	prm->read_timeout = 0;
	cm_oneshot_init (&prm->read_oneshot, cm_cl_prm_read_timeout, argr);

	prm->ardy_timer        = 0;
	prm->ardy_timeout_msec = 0;
	cm_oneshot_init (&prm->ardy_oneshot, cm_cl_prm_ardy_timeout, argr);
}


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_prm_mode (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	return argr->cm.cl.prm.prm_seq;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_abort (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	if (argr->cm.cl.prm.state != CM_CL_PRM_STATE_DOWN)
	{
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_WF_DOWN;

		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		/* check prm-down.ind */
		cm_cl_prm_trigger (argr, 0);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_request (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prm_mode,
	CM_UPPER_RQB_PTR_TYPE rqb
	)
{
	CM_ASSERT (cm_cl_prm_request_can_queue (argr, prm_mode));

	switch (prm_mode)
	{
	case CM_CL_PRMMODE_CONNECT:
		CM_ASSERT (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE);
		CM_ASSERT ((LSA_VOID_PTR_TYPE)argr == (LSA_VOID_PTR_TYPE)rqb);
		argr->cm.cl.prm.prm_keep_upd = rqb;
		break;

	case CM_CL_PRMMODE_PLUG:
		argr->cm.cl.prm.prm_keep_plug = rqb;
		break;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		argr->cm.cl.prm.prm_keep_upd = rqb;
		break;

	case CM_CL_PRMMODE_IDLE:
	default:
		CM_FATAL ();
		return;
	}

	if (argr->cm.cl.prm.prm_seq_next == CM_CL_PRMMODE_IDLE)
	{
		argr->cm.cl.prm.prm_seq_next = (LSA_UINT8)prm_mode;
	}
	else if (argr->cm.cl.prm.prm_seq_next2 == CM_CL_PRMMODE_IDLE)
	{
		/* use case: connect -> plug -> prm_update || connect -> prm_update -> plug */
		argr->cm.cl.prm.prm_seq_next2 = (LSA_UINT8)prm_mode;
	}
	else
	{
		CM_FATAL ();
		return;
	}

	if (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE)
	{
		cm_cl_prm_request_next (argr);
	}
	else
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PrmUpdate: dev(%u) request queued, current-prmmode(%u) next-prmmode(%u) next2-prmmode(%u), keep_upd(0x%x) keep_plug(0x%x)"
			, argr->device_nr
			, cm_cl_prm_mode (argr), argr->cm.cl.prm.prm_seq_next, argr->cm.cl.prm.prm_seq_next2
			, argr->cm.cl.prm.prm_keep_upd, argr->cm.cl.prm.prm_keep_plug
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_cl_prm_request_can_queue (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prm_mode
	)
{
	switch (prm_mode)
	{
	case CM_CL_PRMMODE_CONNECT:
		CM_ASSERT (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE);
		if (cm_is_not_null (argr->cm.cl.prm.prm_keep_plug))
		{
			return LSA_FALSE;
		}
		if (cm_is_not_null (argr->cm.cl.prm.prm_keep_upd))
		{
			return LSA_FALSE;
		}
		break;

	case CM_CL_PRMMODE_PLUG:
		if (cm_is_not_null (argr->cm.cl.prm.prm_keep_plug))
		{
			return LSA_FALSE;
		}
		break;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		if (cm_is_not_null (argr->cm.cl.prm.prm_keep_upd))
		{
			return LSA_FALSE;
		}
		break;

	default:
		CM_FATAL ();
		return LSA_FALSE;
	}
	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_prm_request_next (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	LSA_UINT16 prev_prmmode = cm_cl_prm_mode (argr);

	argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_IDLE;

	switch (prev_prmmode)
	{
	case CM_CL_PRMMODE_PLUG:
#ifdef CM_MESSAGE
		/* @todo: here: synchronize with ar-alarm-event() eg remove .prm_alarm */
#endif /* CM_MESSAGE */
		CM_ASSERT (cm_is_not_null (argr->cm.cl.prm.prm_keep_plug));
		argr->cm.cl.prm.prm_keep_plug = LSA_NULL;
		break;

	default:
		break;
	}

	if (argr->cm.cl.prm.prm_seq_next != CM_CL_PRMMODE_IDLE)
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: dev(%u): check NEXT, do next-prmmode(%u), next2-prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.prm.prm_seq_next, argr->cm.cl.prm.prm_seq_next2
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);

		/* start next prm */
		cm_cl_prm_trigger (argr, CM_CL_DO_PRM_START);
		return LSA_TRUE;
	}

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u): check NEXT, IDLE, prev-prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr
		, prev_prmmode, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL		/* return (ListEntry->field < NewEntry->field); */
cm_list_cmp__prm_record__api_slot_subslot_index_transfer (
	CM_LIST_ENTRY_PTR_TYPE ListEntry,
	CM_LIST_ENTRY_PTR_TYPE NewEntry
	)
{
#define cm_index_is_ar_related(idx_)            (0xe000 <= (idx_) && (idx_) <= 0xebff)
#define cm_index_is_normative(idx_)             ((idx_) >= 0x8000 && ! (0xb000 <= (idx_) && (idx_) <= 0xbfff))
#define cm_transfer_sequence_is_defined(ts_)    ((ts_) != 0)

	LSA_UINT32 existing_api;
	LSA_UINT16 existing_slot_nr;
	LSA_UINT16 existing_subslot_nr;
	LSA_UINT32 existing_record_index;
	LSA_UINT32 existing_transfer_sequence;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE existing_coc_ap;

	LSA_UINT32 new_entry_api;
	LSA_UINT16 new_entry_slot_nr;
	LSA_UINT16 new_entry_subslot_nr;
	LSA_UINT32 new_entry_record_index;
	LSA_UINT32 new_entry_transfer_sequence;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE new_entry_coc_ap;

	{
		CM_PD_RECORD_PTR_TYPE existing  = (CM_PD_RECORD_PTR_TYPE) ListEntry;
		CM_PD_RECORD_PTR_TYPE new_entry = (CM_PD_RECORD_PTR_TYPE) NewEntry;

		existing  = CM_CONTAINING_STRUCT (existing,  CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */
		new_entry = CM_CONTAINING_STRUCT (new_entry, CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */

		/* Sorting Order ::= /61158510 Version 2.3 Ed. 2 from 2013-01 -- updated per eMail/2013-06-25/ and AP01553245/
		// a	- AR related data records defined by IEC 61158-6-10 (index is between 0xE000 and 0xEBFF) shall be transferred first in ascending order according to their index.
		// b	- Within the IO Device, all APIs are processed in ascending order.
		// c	- Within each API, all slots carrying a module are processed in ascending order.
		// d	- Within each module, all subslots carrying a submodule are processed in ascending order.
		// e	- Within each submodule,
		// f		- data records defined by IEC 61158-6-10 (index is between 0x8000 and 0xAFFF) will always be transferred first in ascending order according to their index. #AP01553245
		// g		- manufacturer and profile specific data records (index is between 0x0000 and 0x7FFF resp. between 0xB000 and 0xBFFF) will follow afterwards				#AP01553245
		// h			- according to their attribute @TransferSequence as given in the devices GSD,
		// i			- or if the attribute @TransferSequence is not stated, in ascending order according to their index.
		//
		//	Data records having an index between 0xC000 and 0xDFFF or between 0xECFF and 0xFFFF are always read-only according to IEC 61158-6-10 and IEC 61784-2.
		//	Therefore they do not appear within the transfer sequence.
		//	=> interpretation: read-only index are transferred like IEC data records, see f
		//
		//	@TransferSequence is not stated ::= (see GSDML Version 2.31  Date June 2013)
		//		"If this attribute is set to 0 (which is the default) then the transfer sequence is in ascending order according to their index."
		//
		// ASOM h:
		//				The transmission sequence is
		//				- defined by the attribute DataRecords­TransferSequence if present; derived from the GSD attribute ..\ParameterRecordDataItem\@Transfer resp. ..\F_ParameterRecordData­Item\@TransferSequence,
		//				  The content of the attribute is an array of OMS_UINT32 stored in big endian format. Each array element specifies the index of a particular data record to be transmitted.
		//				  The number of elements is given implicitly by the length of the OMS_BLOB / 4.
		// ASOM i:
		//				- otherwise in ascending order according to their index.
		//
		*/

		/*	CombinedObjectContainer :: /FDIS 61158-5-10_v23ed2MU2_FINAL__CBPG6_TO_PNO_CO__Rev4.pdf/
		//		The transport of the parameters for the submodules of a combined object is done by writing
		//		the CO Container to its access point; addressed by API, slot, subslot and index. The CO
		//		Container holds the parameterization for all submodules; for this it is substructured in
		//		subblocks; each having an address for the submodule (API, slot and subslot), an index for the
		//		data record to write and the data itself. The CO Container shall only contain data records
		//		assigned to submodules of the combined object. The records shall be sorted according to the
		//		transfer sequence defined in chapter 7.5.2.1.
		*/

		existing_api               = existing->api;
		existing_slot_nr           = existing->slot_nr;
		existing_subslot_nr        = existing->subslot_nr;
		existing_record_index      = existing->record_index;
		existing_transfer_sequence = existing->transfer_sequence;
		existing_coc_ap            = cm_cl_ar_graph_record_find_coc_access_point (existing);

		new_entry_api               = new_entry->api;
		new_entry_slot_nr           = new_entry->slot_nr;
		new_entry_subslot_nr        = new_entry->subslot_nr;
		new_entry_record_index      = new_entry->record_index;
		new_entry_transfer_sequence = new_entry->transfer_sequence;
		new_entry_coc_ap            = cm_cl_ar_graph_record_find_coc_access_point (new_entry);

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: this doesn't work with multiple COC-APs")
#endif /* CM_MESSAGE */

		if (cm_is_not_null (existing_coc_ap) &&  cm_is_null (new_entry_coc_ap))
		{
			existing_api        = existing_coc_ap->mod->ap->api;
			existing_slot_nr    = existing_coc_ap->mod->slot_nr;
			existing_subslot_nr = existing_coc_ap->subslot_nr;
		}
		if (cm_is_null (existing_coc_ap) &&  cm_is_not_null (new_entry_coc_ap))
		{
			new_entry_api        = new_entry_coc_ap->mod->ap->api;
			new_entry_slot_nr    = new_entry_coc_ap->mod->slot_nr;
			new_entry_subslot_nr = new_entry_coc_ap->subslot_nr;
		}
		/* both records inside or outside of the COC: use the record api/slot/subslot */
	}

	if (! cm_index_is_ar_related (existing_record_index) && cm_index_is_ar_related (new_entry_record_index))
	{
		return LSA_FALSE;	/*a, new first */
	}
	else if (cm_index_is_ar_related (existing_record_index) && ! cm_index_is_ar_related (new_entry_record_index))
	{
		return LSA_TRUE;/*a, existing first */
	}
	else if (cm_index_is_ar_related (existing_record_index) && cm_index_is_ar_related (new_entry_record_index))
	{
		return existing_record_index <= new_entry_record_index;	/*a*/
	}
	else
	{
		if (existing_api == new_entry_api)
		{
			if (existing_slot_nr == new_entry_slot_nr)
			{
				if (existing_subslot_nr == new_entry_subslot_nr)/*e*/
				{
					if (! cm_index_is_normative (existing_record_index) && cm_index_is_normative (new_entry_record_index))
					{
						return LSA_FALSE;	/*f, new first */
					}
					else if (cm_index_is_normative (existing_record_index) && ! cm_index_is_normative (new_entry_record_index))
					{
						return LSA_TRUE;/*f, existing first */
					}
					else if (cm_index_is_normative (existing_record_index) && cm_index_is_normative (new_entry_record_index))
					{
						return existing_record_index <= new_entry_record_index;	/*f*/
					}
					else
					{
						if (! cm_transfer_sequence_is_defined (existing_transfer_sequence) &&
							cm_transfer_sequence_is_defined (new_entry_transfer_sequence))
						{
							return LSA_FALSE;	/*h, new first */
						}
						else if (cm_transfer_sequence_is_defined (existing_transfer_sequence) &&
								 ! cm_transfer_sequence_is_defined (new_entry_transfer_sequence))
						{
							return LSA_TRUE;/*h, existing first */
						}
						else if (cm_transfer_sequence_is_defined (existing_transfer_sequence) &&
								 cm_transfer_sequence_is_defined (new_entry_transfer_sequence))
						{
							if (existing_transfer_sequence == new_entry_transfer_sequence)	/* illegal case, not checked earlier */
							{
								/* see GSDML: The TransferSequence attribute shall be unique within a submodule.
								//	The first transferred ParameterRecordData object shall have this attribute set to "1",
								//	the following objects shall have this attribute incremented in direct sequence (without gaps). */
								return existing_record_index <= new_entry_record_index;	/* within transfer-sequence order by record-index */
							}
							else
							{
								return existing_transfer_sequence <= new_entry_transfer_sequence;	/*h*/
							}
						}
						else
						{
							return existing_record_index <= new_entry_record_index;	/*i*/
						}
					}
				}
				else
				{
					return existing_subslot_nr <= new_entry_subslot_nr;	/*d*/
				}
			}
			else
			{
				return existing_slot_nr <= new_entry_slot_nr;	/*c*/
			}
		}
		else
		{
			return existing_api <= new_entry_api;	/*b*/
		}
	}
#undef cm_index_is_ar_related
#undef cm_index_is_normative
#undef cm_transfer_sequence_is_defined
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_record_list_build (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 prm_mode
	)
{
	CM_AR_GRAPH_AP_PTR_TYPE ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub = LSA_NULL;

	CM_ASSERT (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_IDLE);

	argr->cm.cl.prm.submod.api        = 0;
	argr->cm.cl.prm.submod.slot_nr    = 0;
	argr->cm.cl.prm.submod.subslot_nr = 0;

	/* collect prm-records to prm_list */

	CM_ASSERT (CmListIsEmpty (&argr->cm.cl.prm.prm_rec_list));

	CmListInitialize (&argr->cm.cl.prm.prm_submod_list);

	switch (prm_mode)
	{
	case CM_CL_PRMMODE_CONNECT:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_CONNECT;

		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) prm-connect"
			, argr->device_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

		/* all ar-related prm-records */
		{
			CM_PD_RECORD_PTR_TYPE rec;

			for (CmListForeach (rec, &argr->cm.cl.ar_prm_record_list, CM_PD_RECORD_PTR_TYPE))
			{
				CmListInsertSorted (&argr->cm.cl.prm.prm_rec_list, &rec->prm_rec_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
			}
		}

		/* all prm-related submodules */
		for (CmListForeach (ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					/* prm_mode connect means after IODConnectRes.ModuleDiffBlock, so sub_state is set correctly */
					if (cm_cl_sub_state_must_prmed (sub->cl.real_sub_state))
					{
						CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
					}
					else if ((sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) != 0)
					{
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap = cm_cl_ar_graph_record_find_sub_coc_ap (argr, ap->api, mod->slot_nr,
							sub->subslot_nr);

						if (cm_is_not_null (sub_coc_ap) && cm_cl_sub_state_must_prmed (sub_coc_ap->cl.real_sub_state))
						{
							CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
						}
					}
				}
			}
		}

		break;

	case CM_CL_PRMMODE_PLUG:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_PLUG;

		{
			CM_UPPER_ALARM_PTR_TYPE alarm = argr->cm.cl.prm.prm_keep_plug->args.cl.alarm;

			argr->cm.cl.prm.submod.api        = alarm->api;
			argr->cm.cl.prm.submod.slot_nr    = alarm->slot_nr;
			argr->cm.cl.prm.submod.subslot_nr = alarm->subslot_nr;

			cm_ar_graph_find_api_slot_subslot (argr
				, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
				, LSA_NULL, &mod, &sub
				);
			if (cm_is_null (sub))
			{
				CM_FATAL ();
			}

			/* moved from cm_cl_alarm_indication() */
			{
				/* PLUG means: submodule does fit */
				mod->ist.mod_ident     = alarm->mod_ident;
				sub->cl.real_sub_ident = alarm->sub_ident;
			}
		}

		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) prm-plug-alarm subslot(%u/%u/0x%x)"
			, argr->device_nr
			, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_PLUG;

		sub->cl.real_sub_state = CM_SUB_STATE_GOOD;
		if (sub->cl.real_sub_ident != sub->soll.sub_ident)	/* AP00219532 */
		{
			CM_SUB_STATE_SET_IDENT (sub->cl.real_sub_state, CM_SUB_STATE_IDENT_SUBSTITUTE);
		}

		/* plugged COC AP or member */
		if ((sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) != 0)
		{
			if ((sub->cl.sub_coc_properties & CM_SUB_COC_IS_ACCESS_POINT) != 0)
			{
				/* COC AP: all submodule of this coc-group */
				CM_AR_GRAPH_AP_PTR_TYPE ap_coc;
				CM_AR_GRAPH_MODULE_PTR_TYPE mod_coc;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc;
				LSA_UINT16 coc_prop_groupid = (sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK);
				for (CmListForeach (ap_coc, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
				{
					for (CmListForeach (mod_coc, &ap_coc->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
					{
						for (CmListForeach (sub_coc, &mod_coc->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
						{
							if (coc_prop_groupid == (sub_coc->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK))
							{
								CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub_coc->cl.submod_link);
							}
						}
					}
				}
			}
			else
			{
				/* COC member: do not prm */
			}
		}
		/* plugged submodule */
		else
		{
			CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
		}
		break;

	case CM_CL_PRMMODE_AR:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_AR;

		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) prm-update ar"
			, argr->device_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

		/* all prm-related submodules */
		/* ! sync with cm_cl_rpc_prm_begin() */
		for (CmListForeach (ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					sub->cl.real_sub_state = CM_SUB_STATE_GOOD;

					CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
				}
			}
		}
		break;

	case CM_CL_PRMMODE_SUBMODULE:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_SUBMODULE;

		{
			CM_UPPER_CL_PRM_UPDATE_PTR_TYPE prm_update = argr->cm.cl.prm.prm_keep_upd->args.cl.prm_update;
			CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE prm_submod;

			for (CmListForeach (prm_submod, &prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
			{
				cm_ar_graph_find_api_slot_subslot (argr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					, LSA_NULL, LSA_NULL, &sub
					);

				CM_ASSERT (cm_is_not_null (sub));

				CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "PrmUpdate: START: dev(%u) prm-update submodule; subslot(%u/%u/0x%x) submodule.state(0x%x)"
					, argr->device_nr
					, sub->mod->ap->api, sub->mod->slot_nr, sub->subslot_nr
					, sub->cl.real_sub_state
					);

				sub->cl.real_sub_state = CM_SUB_STATE_GOOD;

				argr->cm.cl.prm.submod.api        = prm_submod->api;
				argr->cm.cl.prm.submod.slot_nr    = prm_submod->slot_nr;
				argr->cm.cl.prm.submod.subslot_nr = prm_submod->subslot_nr;

				CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
			}

			cm_cl_prm_update_record_list_activate (argr, prm_update);
		}

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

		break;

	case CM_CL_PRMMODE_COC:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_COC;

		{
			CM_UPPER_CL_PRM_UPDATE_PTR_TYPE prm_update = argr->cm.cl.prm.prm_keep_upd->args.cl.prm_update;
			CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE prm_submod;

			for (CmListForeach (prm_submod, &prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
			{
				cm_ar_graph_find_api_slot_subslot (argr
					, prm_submod->api, prm_submod->slot_nr, prm_submod->subslot_nr
					, LSA_NULL, LSA_NULL, &sub
					);

				CM_ASSERT (cm_is_not_null (sub));

				CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "PrmUpdate: START: dev(%u) prm-update coc; subslot(%u/%u/0x%x) submodule.state(0x%x)"
					, argr->device_nr
					, sub->mod->ap->api, sub->mod->slot_nr, sub->subslot_nr
					, sub->cl.real_sub_state
					);

				if ((sub->cl.sub_coc_properties & CM_SUB_COC_IS_ACCESS_POINT) != 0)
				{
					sub->cl.real_sub_state = CM_SUB_STATE_GOOD;

					argr->cm.cl.prm.submod.api        = prm_submod->api;
					argr->cm.cl.prm.submod.slot_nr    = prm_submod->slot_nr;
					argr->cm.cl.prm.submod.subslot_nr = prm_submod->subslot_nr;
				}
				CmListInsertTail (&argr->cm.cl.prm.prm_submod_list, &sub->cl.submod_link);
			}

			cm_cl_prm_update_record_list_activate (argr, prm_update);
		}

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

		break;

	default:
		CM_FATAL ();
		return;
	}


	{
		CM_LIST_ENTRY_PTR_TYPE inner_sub;
		CM_PD_RECORD_PTR_TYPE rec;

		for (CmListForeach (inner_sub, &argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
		{
			sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

			for (CmListForeach (rec, &sub->cl.prm_record_list, CM_PD_RECORD_PTR_TYPE))
			{
				if (rec->tailor_data_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
				{
					CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "PrmUpdate: dev(%u) skip tailored record-index(0x%x) for subslot(%u/%u/0x%x)"
						, argr->device_nr, rec->record_index
						, rec->api, rec->slot_nr, rec->subslot_nr
						);
				}
				else
				{
					CmListInsertSorted (&argr->cm.cl.prm.prm_rec_list, &rec->prm_rec_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
				}
			}
		}

#if 0/* trace record-order, see record_index, transfer_sequence, CombinedObjectContainer */
		{
			CM_PD_RECORD_PTR_TYPE i_rec;
			for (CmListForeach (i_rec, &argr->cm.cl.prm.prm_rec_list, CM_PD_RECORD_PTR_TYPE))
			{
				CM_PD_RECORD_PTR_TYPE rec = CM_CONTAINING_STRUCT (i_rec, CM_PD_RECORD_PTR_TYPE, prm_rec_link);
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub;
				cm_ar_graph_find_api_slot_subslot (argr, rec->api, rec->slot_nr, rec->subslot_nr, LSA_NULL, LSA_NULL, &sub);
				if (cm_is_not_null (sub) &&  (sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) != 0)
				{
					CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap = cm_cl_ar_graph_record_find_coc_access_point (rec);
					CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "OrderOfRecord: submod(%u/%u/0x%x) COC-submod(%u/%u/0x%x) index(0x%08x) transfer_seq(0x%04x) COC-prop(0x%x)"
						, sub_coc_ap->mod->ap->api, sub_coc_ap->mod->slot_nr, sub_coc_ap->subslot_nr
						, rec->api, rec->slot_nr, rec->subslot_nr
						, rec->record_index, rec->transfer_sequence, sub->cl.sub_coc_properties
						);
				}
				else
				{
					CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "OrderOfRecord: submod(%u/%u/0x%x) index(0x%08x) transfer_seq(0x%04x)"
						, rec->api, rec->slot_nr, rec->subslot_nr, rec->record_index, rec->transfer_sequence
						);
				}
			}
		}
#endif
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_record_list_update (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
	case CM_CL_PRMMODE_COC:
		{
			LSA_BOOL again = LSA_TRUE;

			while (again)
			{
				CM_LIST_ENTRY_PTR_TYPE inner_sub;

				again = LSA_FALSE;

				for (CmListForeach (inner_sub, &argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
				{
					CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

					if (! cm_cl_sub_state_must_prmed (sub->cl.real_sub_state))
					{
						if ((sub->cl.sub_coc_properties & CM_SUB_COC_GROUPID_MASK) != 0)
						{
							CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap = cm_cl_ar_graph_record_find_sub_coc_ap (argr, sub->mod->ap->api,
								sub->mod->slot_nr, sub->subslot_nr);

							if (cm_is_not_null (sub_coc_ap) && cm_cl_sub_state_must_prmed (sub_coc_ap->cl.real_sub_state))
							{
								continue;
							}
						}

						CmListRemoveEntry (inner_sub);
						again = LSA_TRUE;
						break;	/*for*/
					}
				}
			}
		}


		CmListInitialize (&argr->cm.cl.prm.prm_rec_list);
		{
			CM_LIST_ENTRY_PTR_TYPE inner_sub;
			CM_PD_RECORD_PTR_TYPE rec;

			for (CmListForeach (inner_sub, &argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

				for (CmListForeach (rec, &sub->cl.prm_record_list, CM_PD_RECORD_PTR_TYPE))
				{
					/*AP01312556*/
					CmListInsertSorted (&argr->cm.cl.prm.prm_rec_list, &rec->prm_rec_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
				}
			}
		}
		return;

	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	default:
		CM_FATAL ();
		return;
	}
}


/*------------------------------------------------------------------------------
// the new prm-statemachine
//----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_prm_trigger (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 done
	)
{
	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PrmUpdate: dev(%u) prm-trigger(0x%x) prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, done, cm_cl_prm_mode (argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	/* do done */
	switch (done)
	{
	case 0:	/* trigger only */
		break;

	case CM_CL_DO_PRM_START:/* start */
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_PRM;
		if (! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START))	/* prm-plug & prm-upd started at the same time */
		{
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);
		}
		break;

	case CM_CL_WF_PRM_BEGIN_RSP:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_BEGIN_RSP);
		break;

	case CM_CL_WF_RPC_PRM_WRITE_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF);
		break;

	case CM_CL_WF_PRM_APPL_RDY_IND:
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND))	/* see prm-abort */
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

			cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_ARDY);
		}
		break;

	case CM_CL_WF_RPC_RECORD_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);
		break;

	case CM_CL_WF_PRM_INDATA_RSP:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP);
		break;

	case CM_CL_WF_PRM_PLUG_ACK_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF);
		break;

	default:
		CM_FATAL1 (done);
		break;
	}

	/* common wait-for */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_BEGIN_RSP))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf prm-begin.rsp", argr->device_nr);
		return;
	}
	/* not common:  CM_CL_WF_PRM_INDATA_RSP => see cm_cl_ar_check_abort() */

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc prm-begin.cnf", argr->device_nr);
		return;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc prm-end.cnf", argr->device_nr);
		return;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc prm-write.cnf", argr->device_nr);
		return;
	}

	/* do next */
	switch (argr->cm.cl.prm.state)
	{
	case CM_CL_PRM_STATE_DOWN:
		if (argr->cm.cl.state == CM_DST_PASSIVE)
		{
			/* start next read write using implicit read */

			cm_cl_record_read_write (argr->ar_com.channel, argr);
		}
		if (argr->cm.cl.state == CM_DST_NARE)
		{
			/* read might have blocked startup */

			cm_cl_bindsm_trigger (argr, 0);
		}
		else if (argr->cm.cl.state == CM_DST_WF_DOWN)
		{
			cm_cl_arsm (argr);
		}
		break;

	case CM_CL_PRM_STATE_PRM:

		/* do start */
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START))
		{
			if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc user-read/write.cnf", argr->device_nr);
				return;
			}
			if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf prm-indata.rsp", argr->device_nr);
				return;
			}

			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);

			CM_ASSERT (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_IDLE);

			switch (argr->cm.cl.prm.prm_seq_next)
			{
			case CM_CL_PRMMODE_CONNECT:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_CONNECT);
				break;

			case CM_CL_PRMMODE_AR:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_AR);
				break;

			case CM_CL_PRMMODE_SUBMODULE:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_SUBMODULE);
				break;

			case CM_CL_PRMMODE_COC:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_COC);
				break;

			case CM_CL_PRMMODE_PLUG:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_PLUG);
				break;

			default:
				CM_FATAL ();
				return;
			}

			/* callback needed by prm-update.req */
			cm_cl_prm_start_cnf (argr);

			argr->cm.cl.prm.prm_seq_next  = argr->cm.cl.prm.prm_seq_next2;
			argr->cm.cl.prm.prm_seq_next2 = CM_CL_PRMMODE_IDLE;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE);	/* wait flag for arsm() */

			cm_cl_prm_begin_ind (argr);
			cm_cl_prm_trigger (argr, 0);
			/* wait for prm-begin.rsp */
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN))
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);

			cm_cl_rpc_prm_begin (argr);
			return;
		}

		/* writing the next prm_record from dev->prm_list */
		if (! CmListIsEmpty (&argr->cm.cl.prm.prm_rec_list))
		{
			CM_UPPER_RQB_PTR_TYPE upper = cm_cl_prm_rqb_alloc (argr);
			if (cm_is_null (upper))
			{
				CM_CL_DOWN_INFO_TYPE dinfo;
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PrmUpdate: dev(%u) ar(%u) no mem", argr->device_nr, argr->ar_nr);

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
				cm_cl_arsm_down (argr, &dinfo);

				return;
			}
			if (! cm_cl_prm_record_write (argr, upper))
			{
				CM_CL_DOWN_INFO_TYPE dinfo;

				cm_cl_prm_rqb_free (argr, upper);

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_WRITE));
				cm_cl_arsm_down (argr, &dinfo);

				return;
			}
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END))
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);

			cm_cl_prm_end_req (argr);
			cm_cl_rpc_prm_end (argr);
			return;
		}

		argr->cm.cl.prm.state = CM_CL_PRM_STATE_UP;
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE);
		cm_cl_prm_up_ind (argr);

		cm_cl_prm_trigger (argr, 0);
		break;

	case CM_CL_PRM_STATE_UP:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc prm-appl-rdy.ind", argr->device_nr);

			/* start next record-read/write */
			cm_cl_record_read_write (argr->ar_com.channel, argr);
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf prm-indata.rsp", argr->device_nr);

			/* start next record-read/write */
			cm_cl_record_read_write (argr->ar_com.channel, argr);
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_PLUG_ACK_CNF))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf plug alarm-ack.cnf", argr->device_nr);

			/* start next record-read/write */
			cm_cl_record_read_write (argr->ar_com.channel, argr);
			return;
		}

		if (cm_cl_prm_request_next (argr))
		{	/* next prm started */
			return;
		}

		cm_cl_prm_idle_ind (argr);

		/* start next record-read/write */
		cm_cl_record_read_write (argr->ar_com.channel, argr);
		break;

	case CM_CL_PRM_STATE_WF_DOWN:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf prm-indata.rsp", argr->device_nr);
			return;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
		{
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) wf rpc user-read/write.cnf", argr->device_nr);
			return;
		}

		/* no more wf-flag */
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_DOWN;
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE);/* up -> prm_abort() -> down.ind */
		if (cm_is_not_null (argr->cm.cl.prm.prm_keep_upd))
		{
			cm_cl_prm_update_record_list_free (argr, argr->cm.cl.prm.prm_keep_upd->args.cl.prm_update);
			cm_callback (argr->ar_com.channel, CM_OK_CANCELLED, &argr->cm.cl.prm.prm_keep_upd);
		}

		cm_cl_prm_down_ind (argr);
		break;

	default:
		CM_FATAL1 (argr->cm.cl.prm.state);
		return;
	}
}


/*------------------------------------------------------------------------------
// internal opc record_read
//----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_rpc_record_read (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_CONST_PTR_TYPE upper
	)
{
	CM_UPPER_CL_RECORD_READ_PTR_TYPE read;
	CM_CL_DEVICE_PTR_TYPE dev     = argr->cm.cl.dev;
	CM_RPC_LOWER_RQB_PTR_TYPE rpc = dev->bindsm.rpc;
	CM_RPC_MEM_U8_PTR_TYPE dp;
	CLRPC_UUID_TYPE target_uuid = argr->ar.ar_uuid;	/* implicit read only */
	LSA_UINT32 dp_left;
	LSA_BOOL do_read_implicit;

	if (argr->cm.cl.state == CM_DST_WF_APPL_READY_IND || argr->cm.cl.state == CM_DST_WF_NEW_DATA || argr->cm.cl.state == CM_DST_IN_DATA)
	{
		do_read_implicit = LSA_FALSE;
	}
	else
	{
		do_read_implicit = LSA_TRUE;
	}

	CM_ASSERT (cm_is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = upper->args.cl.read;

	CM_ASSERT (CM_RECORD_OFFSET + read->rec.readquery_length <= read->rec.data_length);

	CM_ASSERT (cm_is_not_null (dev->rpc_client_id));
	CM_ASSERT (cm_is_not_null (rpc));

	/* fill rpc-call */
	rpc->args.cl.call->client_id = dev->rpc_client_id;

	if (do_read_implicit)
	{
		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rec-read-impl(%u): rpc-client(%08x) opnum(%u) submodule(%u/%u/%u) rec_index(0x%x)"
			, dev->device_nr, rpc->args.cl.call->client_id, rpc->args.cl.call->opnum
			, read->rec.api, read->rec.slot_nr, read->rec.subslot_nr, read->rec.record_index
		);
		CM_CL_TRACE_12 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rec-read-impl(%u): target_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, dev->device_nr
			, target_uuid.time_low, target_uuid.time_mid, target_uuid.time_hi_and_version
			, target_uuid.clock_seq_hi_and_reserved, target_uuid.clock_seq_low
			, target_uuid.node[0], target_uuid.node[1], target_uuid.node[2]
			, target_uuid.node[3], target_uuid.node[4], target_uuid.node[5]
		);

		rpc->args.cl.call->opnum = CLRPC_PNIO_OPNUM_READ_IMPLICIT;
	}
	else
	{
		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) slot(%u) subslot(0x%x) index(0x%x) sequence(%u) length(%u) query_length(%u)"
			, argr->device_nr
			, read->rec.slot_nr, read->rec.subslot_nr, read->rec.record_index
			, read->rec.sequence_nr, read->rec.data_length, read->rec.readquery_length
			);
		rpc->args.cl.call->opnum = CLRPC_PNIO_OPNUM_READ;
	}

	rpc->args.cl.call->semantics        = CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature   = 1;/* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec = CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->alloc_len = read->rec.data_length;
	rpc->args.cl.call->ndr_len   = CM_RECORD_OFFSET + read->rec.readquery_length;
	rpc->args.cl.call->ndr_data  = read->rec.data;			/* zero copy */

	/* marshal args to PNIO-net-format */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;

	dp_left = CM_RECORD_OFFSET - CLRPC_PNIO_OFFSET;

	CM_PUT16_HTON (dp, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_READ_REQ);
	CM_PUT16_HTON (dp, CM_BLK_HEADER_length,  CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	CM_PUT16_HTON (dp, CM_BLK_HEADER_version, 0x0100);

	if (do_read_implicit)
	{
		CM_PUT16_HTON (dp, CM_BLK_RDWR_seq_number, 0);	/* const */
		CM_PUT_UUID_HTON (dp, CM_BLK_RDWR_ar_uuid, &cm_uuid_nil_uuid);
	}
	else
	{
		read->rec.sequence_nr = argr->cm.cl.record_sequence_nr;
		CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

		CM_PUT16_HTON (dp, CM_BLK_RDWR_seq_number, read->rec.sequence_nr);
		CM_PUT_UUID_HTON (dp, CM_BLK_RDWR_ar_uuid, &argr->ar.ar_uuid);
	}

	CM_PUT32_HTON (dp, CM_BLK_RDWR_api, read->rec.api);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_slot_nr, read->rec.slot_nr);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_subslot_nr, read->rec.subslot_nr);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_padding_1, 0);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_index, read->rec.record_index);
	CM_PUT32_HTON (dp, CM_BLK_RDWR_record_data_length, read->rec.data_length - CM_RECORD_OFFSET);

	if (do_read_implicit)
	{
		CM_PUT_UUID_HTON (dp, CM_BLK_RDWR_req_rd_implicit_target_ar_uuid, &target_uuid);
		CM_MEMSET (dp + CM_BLK_RDWR_req_rd_implicit_padding_2, 0, 8);
	}
	else
	{
		CM_MEMSET (dp + CM_BLK_RDWR_req_rd_explicit_padding_2, 0, 24);
	}

#if CM_DEBUG
	cm_dp_adjust (dp, dp_left, CM_BLK_RDWR_SIZE);
	CM_ASSERT (dp_left == 0);
	LSA_UNUSED_ARG (dp);/* lint */
#endif

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	dev->bindsm.schedule_count = 0;
	cm_rpc_request_lower (argr->ar_com.channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_pnio_status_abort_ar (
	LSA_UINT32 pnio_status
	)
{
	LSA_UINT8 error_decode;

	if (pnio_status == CM_PNIO_ERR_NONE)
	{
		return LSA_FALSE;
	}

	/* 2010-07-26: An applicative error code is not a protocol error and therefore no AR-abortion. */
	error_decode = CM_PNIO_ERR_FETCH_DECODE (pnio_status);
	if (error_decode == CM_PNIO_ERR_DECODE_PNIORW)
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// internal opc record write
//----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_rpc_record_write (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_RQB_CONST_PTR_TYPE upper
	)
{
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write;
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;
	CM_RPC_MEM_U8_PTR_TYPE dp;
	LSA_UINT32 dp_left;

	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	CM_ASSERT (cm_is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)

	write = upper->args.cl.write;

	rpc = argr->cm.cl.dev->bindsm.rpc;

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rpc_record_write(0x%x) submodule(%u/%u/0x%x) index(0x%x)"
		, argr->device_nr, rpc
		, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
		, write->rec.record_index
		);

	/* fill rpc-call */
	rpc->args.cl.call->client_id        = dev->rpc_client_id;
	rpc->args.cl.call->opnum            = CLRPC_PNIO_OPNUM_WRITE;
	rpc->args.cl.call->semantics        = CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature   = 1;/* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec = CM_REMOTE_APPLICATION_TIMEOUT_1SEC;

	rpc->args.cl.call->alloc_len = write->rec.data_length;
	rpc->args.cl.call->ndr_len   = write->rec.data_length;
	rpc->args.cl.call->ndr_data  = write->rec.data;			/* zero copy */

	/* marshal args to PNIO-net-format */
	dp      = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = CM_RECORD_OFFSET;

	CM_PUT16_HTON (dp, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_WRITE_REQ);
	CM_PUT16_HTON (dp, CM_BLK_HEADER_length,  CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	CM_PUT16_HTON (dp, CM_BLK_HEADER_version, 0x0100);

	CM_PUT16_HTON (dp, CM_BLK_RDWR_seq_number,          write->rec.sequence_nr);
	CM_PUT_UUID_HTON (dp, CM_BLK_RDWR_ar_uuid,          &argr->ar.ar_uuid);
	CM_PUT32_HTON (dp, CM_BLK_RDWR_api,                 write->rec.api);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_slot_nr,             write->rec.slot_nr);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_subslot_nr,          write->rec.subslot_nr);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_padding_1,           0);
	CM_PUT16_HTON (dp, CM_BLK_RDWR_index,               write->rec.record_index);
	CM_PUT32_HTON (dp, CM_BLK_RDWR_record_data_length,  write->rec.data_length - CM_RECORD_OFFSET);
	CM_MEMSET (dp + CM_BLK_RDWR_req_wr_padding_2,       0, 24);

#if CM_DEBUG
	cm_dp_adjust (dp, dp_left, CM_BLK_RDWR_SIZE);
	LSA_UNUSED_ARG (dp);/* lint */
	LSA_UNUSED_ARG (dp_left);	/* lint */
#endif

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_CALL, argr->device_nr, argr->ar_nr);
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_cl_rpc_record_write_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_UPPER_RQB_CONST_PTR_TYPE upper
	)
{
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write = upper->args.cl.write;
	CM_RPC_MEM_U8_PTR_TYPE dp;
	LSA_UINT32 dp_left;
	LSA_UINT16 cm_dp_assert_line;
	CM_CL_DOWN_INFO_TYPE dinfo;

	LSA_UINT16 block_type, block_length, block_version;
	CLRPC_UUID_TYPE ar_uuid;
	LSA_UINT16 slot_nr, subslot_nr, index, seq_number;
	LSA_UINT32 api, record_data_length;
	LSA_UINT16 lower_rsp, error_cause;
	LSA_UINT32 pnio_status, nca_status, pnio_status_2;
	LSA_UINT16 padding_1;

	LSA_UNUSED_ARG (channel);
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE);

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_WRITE
		, &lower_rsp, &error_cause, &pnio_status, &nca_status
		);

	if (cm_pnio_status_abort_ar (pnio_status))
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u): rpc_record_write_done(0x%x) failed, error_cause(%u) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr, rpc, error_cause, pnio_status, nca_status
			);
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_WRITE, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}
	/* pnio_status other then NONE are application errors */

	/* enough data ? */
	cm_dp_assert (rpc->args.cl.call->ndr_len >= CM_RECORD_OFFSET /*CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE*/);

	dp      = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	cm_dp_assert (dp_left >= CM_BLK_RDWR_SIZE);

	block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
	block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
	block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

	cm_dp_assert (block_type == CM_BLOCK_TYPE_WRITE_RSP);
	cm_dp_assert (block_length == 60);
	cm_dp_assert (block_version == 0x0100);

	seq_number = CM_GET16_NTOH (dp, CM_BLK_RDWR_seq_number);
	CM_GET_UUID_NTOH (&ar_uuid, dp, CM_BLK_RDWR_ar_uuid);
	api                = CM_GET32_NTOH (dp, CM_BLK_RDWR_api);
	slot_nr            = CM_GET16_NTOH (dp, CM_BLK_RDWR_slot_nr);
	subslot_nr         = CM_GET16_NTOH (dp, CM_BLK_RDWR_subslot_nr);
	padding_1          = CM_GET16_NTOH (dp, CM_BLK_RDWR_padding_1);
	index              = CM_GET16_NTOH (dp, CM_BLK_RDWR_index);
	record_data_length = CM_GET32_NTOH (dp, CM_BLK_RDWR_record_data_length);

	write->rec.ret_val_1 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_wr_additional_value_1);
	write->rec.ret_val_2 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_wr_additional_value_2);
	pnio_status_2        = CM_GET32_NTOH (dp, CM_BLK_RDWR_rsp_wr_pnio_status);

	cm_dp_adjust (dp, dp_left, CM_BLK_RDWR_SIZE);

	cm_dp_assert (seq_number == write->rec.sequence_nr);	/* ??? not checked, see FDIS 61158-6-10_v23ed2MU2_FINAL__CBPG6_TO_PNO_CO__Rev5.pdf */
	cm_dp_assert (cm_uuid_equal (&ar_uuid, &argr->ar.ar_uuid));
	cm_dp_assert (padding_1 == 0);
	cm_dp_assert (index == write->rec.record_index);
	/* record_data_length: not checked */
	/* ret_val_1 and ret_val_2: not checked */
	cm_dp_assert (pnio_status_2 == 0 || pnio_status_2 == pnio_status);
	/* RWPadding: not checked */

	if (index == 0xE040)
	{
		LSA_UINT16 multi_sequence_nr = write->rec.sequence_nr;

		while (dp_left > 0)
		{
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap;

			/* fetch written record */
			CM_PD_RECORD_PTR_TYPE rec = CmListFirst (&argr->cm.cl.prm.prm_multi_check_cnf_list, CM_PD_RECORD_PTR_TYPE);
			cm_dp_assert (cm_is_not_null (rec));
			rec = CM_CONTAINING_STRUCT (rec, CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */
			CmListRemoveEntry (&rec->prm_rec_link);	/* remove record from prm_multi_check_cnf_list */

			sub_coc_ap = cm_cl_ar_graph_record_find_coc_access_point (rec);

			multi_sequence_nr += 1;

			cm_dp_assert (dp_left >= CM_BLK_RDWR_SIZE);

			block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
			block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
			block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

			cm_dp_assert (block_type == CM_BLOCK_TYPE_WRITE_RSP);
			cm_dp_assert (block_length == 60);
			cm_dp_assert (block_version == 0x0100);

			seq_number = CM_GET16_NTOH (dp, CM_BLK_RDWR_seq_number);
			CM_GET_UUID_NTOH (&ar_uuid, dp, CM_BLK_RDWR_ar_uuid);
			api                = CM_GET32_NTOH (dp, CM_BLK_RDWR_api);
			slot_nr            = CM_GET16_NTOH (dp, CM_BLK_RDWR_slot_nr);
			subslot_nr         = CM_GET16_NTOH (dp, CM_BLK_RDWR_subslot_nr);
			padding_1          = CM_GET16_NTOH (dp, CM_BLK_RDWR_padding_1);
			index              = CM_GET16_NTOH (dp, CM_BLK_RDWR_index);
			record_data_length = CM_GET32_NTOH (dp, CM_BLK_RDWR_record_data_length);

			write->rec.ret_val_1 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_wr_additional_value_1);
			write->rec.ret_val_2 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_wr_additional_value_2);
			pnio_status_2        = CM_GET32_NTOH (dp, CM_BLK_RDWR_rsp_wr_pnio_status);

			cm_dp_adjust (dp, dp_left, CM_BLK_RDWR_SIZE);

			cm_dp_assert (seq_number == multi_sequence_nr);	/* ??? not checked, see FDIS 61158-6-10_v23ed2MU2_FINAL__CBPG6_TO_PNO_CO__Rev5.pdf */
			cm_dp_assert (cm_uuid_equal (&ar_uuid, &argr->ar.ar_uuid));
			cm_dp_assert (padding_1 == 0);

			if (cm_is_null (sub_coc_ap))
			{
				cm_dp_assert (api == rec->api);
				cm_dp_assert (slot_nr == rec->slot_nr);
				cm_dp_assert (subslot_nr == rec->subslot_nr);
				cm_dp_assert (index == rec->record_index);
			}
			else
			{
				cm_dp_assert (api == sub_coc_ap->mod->ap->api);
				cm_dp_assert (slot_nr == sub_coc_ap->mod->slot_nr);
				cm_dp_assert (subslot_nr == sub_coc_ap->subslot_nr);
				cm_dp_assert (index == 0x80B0 /* CombinedObjectContainer */);
			}

			/* record_data_length: not checked */
			/* ret_val_1 and ret_val_2: not checked */
			/* RWPadding: not checked */

			if (cm_pnio_status_abort_ar (pnio_status_2))
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u): rpc_record_write_done(0x%x) failed, multi_sequence_nr(%u) pnio_status(0x%x)"
					, argr->device_nr, rpc, multi_sequence_nr, pnio_status_2
					);
				CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_WRITE, CM_ERROR_CAUSE_IOC_ABORTED, pnio_status_2, 0);
				cm_cl_arsm_down (argr, &dinfo);
				return CM_ERR_LOWER_LAYER;
			}

			CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): rpc_record_write_done(0x%x) multi_sequence_nr(%u) submodule(%u/%u/0x%x) index(0x%x)"
				, argr->device_nr, rpc, multi_sequence_nr
				, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
				, write->rec.record_index
				);
		}

		cm_dp_assert (CmListIsEmpty (&argr->cm.cl.prm.prm_multi_check_cnf_list));
	}
	else
	{
		cm_dp_assert (dp_left == 0);

		cm_dp_assert (api == write->rec.api);
		cm_dp_assert (slot_nr == write->rec.slot_nr);
		cm_dp_assert (subslot_nr == write->rec.subslot_nr);
	}


	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rpc_record_write_done(0x%x) submodule(%u/%u/0x%x) index(0x%x)"
		, argr->device_nr, rpc
		, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
		, write->rec.record_index
		);

	write->rec.cm_pnio_err = pnio_status;
	write->rec.data_length = CM_RECORD_OFFSET + record_data_length;
	return CM_OK;

cm_dp_assert_failed:
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): rpc_record_write_done(0x%x) failed, index(%d) failed-line(%u)"
		, argr->device_nr, rpc, write->rec.record_index
		, cm_dp_assert_line
		);
	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_WRITE));
	cm_cl_arsm_down (argr, &dinfo);
	return CM_ERR_LOWER_LAYER;
}


/*----------------------------------------------------------------------------*/
static CM_UPPER_RQB_PTR_TYPE
cm_cl_prm_rqb_alloc (
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	CM_CHANNEL_PTR_TYPE channel = argr->cm.cl.dev->channel;
	CM_UPPER_RQB_PTR_TYPE upper = cm_upper_alloc_rqb (channel);

	if (cm_is_not_null (upper))
	{
		CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write = upper->args.cl.write;
		LSA_UINT32 data_length = argr->cm.cl.dev->dev_rpc.max_supported_record_length;

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&write->rec.data)
			, cm_null_user_id, data_length, channel->sysptr
			);
		if (cm_is_not_null (write->rec.data))
		{
			write->rec.data_length = data_length;	/* keep alloc length ! */
			CM_RQB_SET_OPCODE (upper, CM_OPC_CL_RECORD_WRITE);
			return upper;
		}
		cm_upper_free_rqb (channel, upper);
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_prm_rqb_free (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE upper
	)
{
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	LSA_UINT16 rc;

	if (CM_RQB_GET_OPCODE (upper) ==  CM_OPC_CL_RECORD_WRITE)
	{
		if (cm_is_not_null (upper->args.cl.write->rec.data))/* may be null (AP01457314) */
		{
			CM_FREE_UPPER_MEM (&rc, upper->args.cl.write->rec.data, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			upper->args.cl.write->rec.data = LSA_NULL;
		}
	}
	else
	{
		CM_FATAL ();
	}
	cm_upper_free_rqb (channel, upper);
}


/*------------------------------------------------------------------------------
//
//	IODWriteMultipleReq	::= IODWriteHeader[idx:0xE040], (IODWriteHeader(idx), RecordDataWrite, [Padding] b)*
//	IODWriteHeader		::= BlockHeader, SeqNumber, ARUUID, API, SlotNumber, SubslotNumber, Padding*, Index, RecordDataLength, RWPadding*
//
//	RecordDataWrite[idx:0x80B0]:
//	CombinedObjectContainer ::= (COContainerContent, [Padding]* a)*
//	COContainerContent		::= BlockHeader, Padding, Padding, API, Slot, Subslot, Padding, Padding, Index,   COContainerBlock
//	COContainerBlock		::= RecordDataWrite
*/

/* #define CM_CL_PDU_COC_HEADER	(2+2+2 + 1+1 + 4 + 2 + 2 + 1+1 + 2)  => cm_icl.h */

/* (len_ % 4): 0=>0  1=>3  2=>2  3=>1 */
#define cm_cl_next_padding(len_)  CM_CAST_U16 ((4 - ((len_) % 4)) % 4)

/*----------------------------------------------------------------------------*/

#define CM_CL_PRM_RECORD_LEN(rec_)   CM_CAST_U16 (rec_->tailor_data_len > 0 ? rec_->tailor_data_len : rec_->record_data_len)
#define CM_CL_PRM_RECORD_DATA(rec_)  (rec_->tailor_data_len > 0 ? rec_->tailor_data : rec_->record_data)

/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_prm_record_coc_copy (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_COMMON_MEM_U8_PTR_TYPE   *p_dp,
	LSA_UINT32  *p_dp_left,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE sub_coc_ap
	)
{
	CM_COMMON_MEM_U8_PTR_TYPE dp = *p_dp;
	LSA_UINT32 dp_left           = *p_dp_left;
	LSA_UINT16 cm_dp_assert_line;
	LSA_BOOL one_more_rec = LSA_TRUE;
	LSA_INT next_padding  = 0;

	while (one_more_rec)
	{
		LSA_UINT16 len = CM_CAST_U16 (CM_CL_PDU_COC_HEADER + CM_CL_PRM_RECORD_LEN (rec) - (2 + 2));

		/* optional padding to the next begin */
		for (; next_padding > 0; next_padding -= 1)
		{
			cm_dp_assert (dp_left >= 1);
			CM_PUT8_HTON  (dp, 0, 0);
			cm_dp_adjust (dp, dp_left, 1);
		}

		cm_dp_assert (dp_left >= CM_CL_PDU_COC_HEADER);

		/* COContainer-Header: BlockHeader, Padding, Padding, API, Slot, Subslot, Padding, Padding, Index */
		CM_PUT16_HTON (dp,  0, CM_BLOCK_TYPE_COCONTAINERCONTENT);
		CM_PUT16_HTON (dp,  2, len);
		CM_PUT16_HTON (dp,  4, 0x0100);
		CM_PUT8_HTON  (dp,  6, 0);
		CM_PUT8_HTON  (dp,  7, 0);
		CM_PUT32_HTON (dp,  8, rec->api);
		CM_PUT16_HTON (dp, 12, rec->slot_nr);
		CM_PUT16_HTON (dp, 14, rec->subslot_nr);
		CM_PUT8_HTON  (dp, 16, 0);
		CM_PUT8_HTON  (dp, 17, 0);
		CM_PUT16_HTON (dp, 18, (LSA_UINT16)rec->record_index);

		cm_dp_adjust (dp, dp_left, CM_CL_PDU_COC_HEADER)


		/* COContainerBlock */
		cm_dp_assert (dp_left >= CM_CL_PRM_RECORD_LEN (rec));
		CM_MEMCPY (dp, CM_CL_PRM_RECORD_DATA (rec), CM_CL_PRM_RECORD_LEN (rec));
		cm_dp_adjust (dp, dp_left, CM_CL_PRM_RECORD_LEN (rec))


		CmListRemoveEntry (&rec->prm_rec_link);	/* remove record from prm_rec_list */


		/* check for next coc record */
		{
			one_more_rec = LSA_FALSE;

			rec = CmListFirst (&argr->cm.cl.prm.prm_rec_list, CM_PD_RECORD_PTR_TYPE);
			if (cm_is_not_null (rec))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE next_sub_coc_ap;
				LSA_UINT32 curr_len = CM_CAST_U32 (dp - *p_dp);

				next_padding = cm_cl_next_padding (curr_len);

				rec             = CM_CONTAINING_STRUCT (rec, CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */
				next_sub_coc_ap = cm_cl_ar_graph_record_find_coc_access_point (rec);

				if ((sub_coc_ap == next_sub_coc_ap)
					&&  dp_left >= CM_CAST_U32 (next_padding + CM_CL_PDU_COC_HEADER + CM_CL_PRM_RECORD_LEN (rec)))
				{
					one_more_rec = LSA_TRUE;
				}
			}
		}
	}
	*p_dp      = dp;
	*p_dp_left = dp_left;
	return LSA_TRUE;

cm_dp_assert_failed:
	/* this is a programmers error */
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u): PrmRecordWrite, cm_cl_prm_record_coc_copy, line(%u)"
		, argr->device_nr, cm_dp_assert_line
		);
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_prm_record_multi_copy (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_COMMON_MEM_U8_PTR_TYPE *p_dp,
	LSA_UINT32  *p_dp_left,
	CM_PD_RECORD_PTR_TYPE rec,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE sub_coc_ap
	)
{
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_COMMON_MEM_U8_PTR_TYPE dp = *p_dp;
	LSA_UINT32 dp_left           = *p_dp_left;
	LSA_UINT16 cm_dp_assert_line = 0;
	LSA_BOOL one_more_rec        = LSA_TRUE;
	LSA_INT next_padding         = 0;

	CmListInitialize (&argr->cm.cl.prm.prm_multi_check_cnf_list);

	while (one_more_rec)
	{
		CM_COMMON_MEM_U8_PTR_TYPE dp_hdr;

		/* optional padding to the next begin */
		for (; next_padding > 0; next_padding -= 1)
		{
			cm_dp_assert (dp_left >= 1);
			CM_PUT8_HTON  (dp, 0, 0);
			cm_dp_adjust (dp, dp_left, 1);
		}

		cm_dp_assert (dp_left >= CM_RECORD_HEADER_SIZE);

		/* IODWriteHeader ::= BlockHeader, SeqNumber, ARUUID, API, SlotNumber, SubslotNumber, Padding*, Index, RecordDataLength, RWPadding* */
		CM_PUT16_HTON (dp,  0, CM_BLOCK_TYPE_WRITE_REQ);
		CM_PUT16_HTON (dp,  2, (CM_RECORD_HEADER_SIZE - (2 + 2)));
		CM_PUT16_HTON (dp,  4, 0x0100);
		CM_PUT16_HTON (dp,  6, argr->cm.cl.record_sequence_nr);
		CM_PUT_UUID_HTON (dp,  8, &argr->ar.ar_uuid);
/*		CM_PUT32_HTON (dp, 24, api); */
/*		CM_PUT16_HTON (dp, 28, slot_nr); */
/*		CM_PUT16_HTON (dp, 30, subslot_nr); */
		CM_PUT16_HTON (dp, 32, 0 /*Padding*/);
/*		CM_PUT16_HTON (dp, 34, record_index); */
/*		CM_PUT32_HTON (dp, 36, data_length); */
		CM_MEMSET     (dp + 40, 0, 24);

		CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

		dp_hdr = dp;/* keep */
		cm_dp_adjust (dp, dp_left, CM_RECORD_HEADER_SIZE)

		if (cm_is_not_null (sub_coc_ap))
		{
			CM_COMMON_MEM_U8_PTR_TYPE dp_calc_len = dp;

			CM_PUT32_HTON (dp_hdr, 24, sub_coc_ap->mod->ap->api);
			CM_PUT16_HTON (dp_hdr, 28, sub_coc_ap->mod->slot_nr);
			CM_PUT16_HTON (dp_hdr, 30, sub_coc_ap->subslot_nr);
			CM_PUT16_HTON (dp_hdr, 34, 0x80B0 /* CombinedObjectContainer */);

			/* RecordDataWrite */
			if (! cm_cl_prm_record_coc_copy (argr, &dp, &dp_left, rec, sub_coc_ap))
			{
				goto cm_dp_assert_failed;
			}

			{
				LSA_UINT32 coc_len = CM_CAST_U32 (dp - dp_calc_len);
				CM_PUT32_HTON (dp_hdr, 36, coc_len);
			}
		}
		else
		{
			CM_PUT32_HTON (dp_hdr, 24, rec->api);
			CM_PUT16_HTON (dp_hdr, 28, rec->slot_nr);
			CM_PUT16_HTON (dp_hdr, 30, rec->subslot_nr);
			CM_PUT16_HTON (dp_hdr, 34, (LSA_UINT16)rec->record_index);

			/* RecordDataWrite */
			cm_dp_assert (dp_left >= CM_CL_PRM_RECORD_LEN (rec));
			CM_MEMCPY (dp, CM_CL_PRM_RECORD_DATA (rec), CM_CL_PRM_RECORD_LEN (rec));
			cm_dp_adjust (dp, dp_left, CM_CL_PRM_RECORD_LEN (rec))

			CmListRemoveEntry (&rec->prm_rec_link);	/* remove record from prm_rec_list */

			CM_PUT32_HTON (dp_hdr, 36, CM_CL_PRM_RECORD_LEN (rec));
		}

		/* insert rec to prm.multi_rec_check_list */
		CmListInsertTail (&argr->cm.cl.prm.prm_multi_check_cnf_list, &rec->prm_rec_link);

		/* check for next record */
		{
			one_more_rec = LSA_FALSE;

			rec = CmListFirst (&argr->cm.cl.prm.prm_rec_list, CM_PD_RECORD_PTR_TYPE);
			if (cm_is_not_null (rec))
			{
				LSA_UINT32 curr_len = CM_CAST_U32 (dp - *p_dp);

				next_padding = cm_cl_next_padding (curr_len);

				rec        = CM_CONTAINING_STRUCT (rec, CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */
				sub_coc_ap = cm_cl_ar_graph_record_find_coc_access_point (rec);

				if (cm_is_not_null (sub_coc_ap))
				{
					if (dp_left >= (CM_CAST_U16 (next_padding) + CM_RECORD_HEADER_SIZE + CM_CL_PDU_COC_HEADER + CM_CL_PRM_RECORD_LEN (rec)))
					{
						one_more_rec = LSA_TRUE;
					}
				}
				else
				{
					if (dp_left >= (CM_CAST_U16 (next_padding) + CM_RECORD_HEADER_SIZE + CM_CL_PRM_RECORD_LEN (rec)))
					{
						one_more_rec = LSA_TRUE;
					}
				}
			}
		}
	}
	*p_dp      = dp;
	*p_dp_left = dp_left;
	return LSA_TRUE;

cm_dp_assert_failed:
	/* this is a programmers error */
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u): PrmRecordWrite, cm_cl_prm_record_coc_copy, line(%u)"
		, argr->device_nr, cm_dp_assert_line
		);
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_prm_record_write (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE cm
	)
{
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_CHANNEL_PTR_TYPE channel             = argr->ar_com.channel;
	CM_CL_DEVICE_PTR_TYPE dev               = argr->cm.cl.dev;
	LSA_UINT16 cm_dp_assert_line            = 0;
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write = cm->args.cl.write;

	CM_COMMON_MEM_U8_PTR_TYPE dp;
	LSA_UINT32 dp_left;
	CM_PD_RECORD_PTR_TYPE rec;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub_coc_ap;
	LSA_BOOL do_write_multiple;
	LSA_BOOL write_multiple_is_supported = ((dev->device_properties & CM_CL_DEVICE_PROPERTIES_MULTIPLE_WRITE) != 0);

	CM_ASSERT (argr->cm.cl.prm.state == CM_CL_PRM_STATE_PRM);
	CM_ASSERT (! CmListIsEmpty (&argr->cm.cl.prm.prm_rec_list));

	cm_dp_assert (write->rec.data_length > CM_RECORD_OFFSET);
	dp      = write->rec.data + CM_RECORD_OFFSET;
	dp_left = write->rec.data_length - CM_RECORD_OFFSET;

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: test write-multiple with AP00299791")
#endif /* CM_MESSAGE */

	/* fetch first prm-record */
	rec = CmListFirst (&argr->cm.cl.prm.prm_rec_list, CM_PD_RECORD_PTR_TYPE);
	CM_ASSERT (cm_is_not_null (rec));
	rec = CM_CONTAINING_STRUCT (rec, CM_PD_RECORD_PTR_TYPE, prm_rec_link);	/* calc real record ptr */

	sub_coc_ap = cm_cl_ar_graph_record_find_coc_access_point (rec);

	do_write_multiple = LSA_FALSE;
	if (write_multiple_is_supported)
	{
		if (cm_is_null (sub_coc_ap))
		{
			if (dp_left >= CM_RECORD_HEADER_SIZE + CM_CL_PRM_RECORD_LEN (rec))
			{
				do_write_multiple = LSA_TRUE;
			}
		}
		else
		{
			if (dp_left >= CM_RECORD_HEADER_SIZE + CM_CL_PDU_COC_HEADER + CM_CL_PRM_RECORD_LEN (rec))
			{
				do_write_multiple = LSA_TRUE;
			}
		}
	}

	write->device_nr       = argr->device_nr;
	write->rec.sequence_nr = argr->cm.cl.record_sequence_nr;
	write->rec.ret_val_1   = 0;
	write->rec.ret_val_2   = 0;

	CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

	if (do_write_multiple)
	{
		write->rec.api          = 0xFFFFFFFF;	/* don't care */
		write->rec.slot_nr      = 0xFFFF;	/* don't care */
		write->rec.subslot_nr   = 0xFFFF;	/* don't care */
		write->rec.record_index = 0xE040;	/* IODWriteMultipleReq */

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): PrmRecordWrite: multiple-record,  max_supported_record_length(%u)"
			, argr->device_nr, dev->dev_rpc.max_supported_record_length
			);

		if (! cm_cl_prm_record_multi_copy (argr, &dp, &dp_left, rec, sub_coc_ap))
		{
			goto cm_dp_assert_failed;
		}

		write->rec.data_length = CM_CAST_U32 (dp - write->rec.data);
	}
	else
	{
		if (cm_is_not_null (sub_coc_ap))
		{
			write->rec.api          = sub_coc_ap->mod->ap->api;
			write->rec.slot_nr      = sub_coc_ap->mod->slot_nr;
			write->rec.subslot_nr   = sub_coc_ap->subslot_nr;
			write->rec.record_index = 0x80B0;	/* CombinedObjectContainer */

			CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) : PrmRecordWrite: single CombinedObjectContainer,  coc-submodule(%u/%u/0x%x) max_supported_record_length(%u)"
				, write->device_nr
				, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
				, dev->dev_rpc.max_supported_record_length
				);

			if (! cm_cl_prm_record_coc_copy (argr, &dp, &dp_left, rec, sub_coc_ap))
			{
				goto cm_dp_assert_failed;
			}

			write->rec.data_length = CM_CAST_U32 (dp - write->rec.data);
		}
		else
		{
			write->rec.api          = rec->api;
			write->rec.slot_nr      = rec->slot_nr;
			write->rec.subslot_nr   = rec->subslot_nr;
			write->rec.record_index = (LSA_UINT16)rec->record_index;

			cm_dp_assert (dp_left >= CM_CL_PRM_RECORD_LEN (rec));
			CM_MEMCPY (dp, CM_CL_PRM_RECORD_DATA (rec), CM_CL_PRM_RECORD_LEN (rec));
			cm_dp_adjust (dp, dp_left, CM_CL_PRM_RECORD_LEN (rec))

			CmListRemoveEntry (&rec->prm_rec_link);	/* remove record from prm_rec_list */

			write->rec.data_length = CM_CAST_U32 (dp - write->rec.data);

			CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) : PrmRecordWrite: single prm-record,  submodule(%u/%u/0x%x) index(0x%x) data_len(%u)"
				, write->device_nr
				, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr
				, write->rec.record_index, write->rec.data_length
				);
		}
	}

	cm_cl_rpc_record_write (channel, argr, cm);

	/* wf rpc_record_write_done */
	/* keep upper in record_list */
	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF);
	CmListInsertHead (&argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)cm);

	return LSA_TRUE;

cm_dp_assert_failed:
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u): PrmRecordWrite failed, reason-line(%u)"
		, argr->device_nr, cm_dp_assert_line
		);
	return LSA_FALSE;
}

#undef CM_CL_PRM_RECORD_LEN
#undef CM_CL_PRM_RECORD_DATA


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_record_read_write (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_UPPER_RQB_PTR_TYPE upper;
	LSA_UINT16 prm_cnt = 0, rec_cnt = 0;

	CmListLength (&argr->cm.cl.record_list, &rec_cnt);
	CmListLength (&argr->cm.cl.prm.prm_rec_list, &prm_cnt);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)  prm_state(%u)  wf1(0x%x)  prm_cnt(%u) rec_cnt(%u)"
		, argr->device_nr, argr->cm.cl.prm.state, argr->cm.cl.wait_for[1], prm_cnt, rec_cnt
		);

	switch (argr->cm.cl.prm.state)
	{
	case CM_CL_PRM_STATE_WF_DOWN:
		/* no action */
		break;

	case CM_CL_PRM_STATE_PRM:
		/* no next action */
		break;

	case CM_CL_PRM_STATE_DOWN:
	case CM_CL_PRM_STATE_UP:
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
		{
			if (rec_cnt > 0)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "CL-RecordRW: dev(%u) prm_state(UP) wf rpc user-read/write.cnf"
					, argr->device_nr
					);
			}
			return;
		}

		/* start next user-record */
		while (! CmListIsEmpty (&argr->cm.cl.record_list))
		{
			upper = CmListFirst (&argr->cm.cl.record_list, CM_UPPER_RQB_PTR_TYPE);
			CM_ASSERT (cm_is_not_null (upper));

			cm_cl_prm_read_retrigger (argr);/* simulate prm_read_stop() */

			if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
				||  CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY)
			{
				if (cm_cl_bindsm_is_identified (argr))
				{
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);

					cm_cl_rpc_record_read (argr, upper);
				}
				return;
			}
			else if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)
			{
				if (argr->cm.cl.prm.state == CM_CL_PRM_STATE_DOWN)
				{
					return;
				}
				else
				{
					/* increment sequence_nr */
					upper->args.cl.write->rec.sequence_nr = argr->cm.cl.record_sequence_nr;
					CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);

					cm_cl_rpc_record_write (channel, argr, upper);
					return;
				}
			}
			else
			{
				CM_FATAL1 (CM_RQB_GET_OPCODE (upper));
				return;
			}
		}
		/* CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u): no (more) user-record", argr->device_nr); */
		return;

	default:
		CM_FATAL1 (argr->cm.cl.prm.state);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_read_done (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
	)
{
	CM_UPPER_RQB_PTR_TYPE upper;
	CM_UPPER_CL_RECORD_READ_PTR_TYPE read;
	LSA_UINT32 pnio_status, nca_status;
	LSA_UINT16 lower_rsp, error_cause;

	LSA_UINT16 rc = CM_OK;

	if (CmListIsEmpty (&argr->cm.cl.record_list))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!no record waiting");
		return;
	}

	CmListRemoveHead (&argr->cm.cl.record_list, upper, CM_UPPER_RQB_PTR_TYPE);
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = upper->args.cl.read;

	CM_ASSERT (read->rec.data == rpc->args.cl.call->ndr_data);	/* zero copy */

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_READ
		, &lower_rsp, &error_cause, &pnio_status, &nca_status
		);

	read->rec.cm_pnio_err = pnio_status;

	if (cm_pnio_status_abort_ar (pnio_status))
	{
		CM_CL_DOWN_INFO_TYPE dinfo;
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cm_cl_rpc_record_read_done: dev(%u) ndr_len(%d) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr
			, rpc->args.cl.call->ndr_len, lower_rsp, pnio_status, nca_status
			);
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_READ, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);

		if (lower_rsp == CLRPC_ERR_TIMEOUT)
		{
			rc = CM_ERR_TIMEOUT;
		}
		else
		{
			rc = CM_ERR_LOWER_RPC;
		}
	}

	if (rc == CM_OK)
	{
		/* enough data ? */
		if (rpc->args.cl.call->ndr_len < CM_RECORD_OFFSET)
		{
			read->rec.cm_pnio_err = CM_PNIO_FAULTY_RTA (CM_AR_REASON_PROT);
		}
		else
		{
			CM_RPC_MEM_U8_PTR_TYPE dp;
			LSA_UINT16 block_type, block_length, block_version;
			CLRPC_UUID_TYPE ar_uuid;
			CLRPC_UUID_TYPE req_ar_uuid;
			LSA_UINT16 slot_nr, subslot_nr, index, seq_number;
			LSA_UINT32 api, record_data_length;
			LSA_UINT16 padding_1;

			if (rpc->args.cl.call->opnum == CLRPC_PNIO_OPNUM_READ_IMPLICIT)
			{
				req_ar_uuid = cm_uuid_nil_uuid;
			}
			else
			{
				req_ar_uuid = argr->ar.ar_uuid;
			}

			dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;

			block_type    = CM_GET16_NTOH (dp, CM_BLK_HEADER_type);
			block_length  = CM_GET16_NTOH (dp, CM_BLK_HEADER_length);
			block_version = CM_GET16_NTOH (dp, CM_BLK_HEADER_version);

			seq_number = CM_GET16_NTOH (dp, CM_BLK_RDWR_seq_number);/* not checked */
			LSA_UNUSED_ARG (seq_number);
			CM_GET_UUID_NTOH (&ar_uuid, dp, CM_BLK_RDWR_ar_uuid);
			api                = CM_GET32_NTOH (dp, CM_BLK_RDWR_api);
			slot_nr            = CM_GET16_NTOH (dp, CM_BLK_RDWR_slot_nr);
			subslot_nr         = CM_GET16_NTOH (dp, CM_BLK_RDWR_subslot_nr);
			padding_1          = CM_GET16_NTOH (dp, CM_BLK_RDWR_padding_1);
			index              = CM_GET16_NTOH (dp, CM_BLK_RDWR_index);
			record_data_length = CM_GET32_NTOH (dp, CM_BLK_RDWR_record_data_length);

			read->rec.ret_val_1 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_rd_wr_additional_value_1);
			read->rec.ret_val_2 = CM_GET16_NTOH (dp, CM_BLK_RDWR_rsp_rd_wr_additional_value_2);

			if (block_type != CM_BLOCK_TYPE_READ_RSP
				|| block_length != 60
				|| block_version != 0x0100
				|| ! cm_uuid_equal (&ar_uuid, &req_ar_uuid)
				|| api != read->rec.api
				|| slot_nr != read->rec.slot_nr
				|| subslot_nr != read->rec.subslot_nr
				|| padding_1 != 0
				|| index != read->rec.record_index
				|| record_data_length + CM_RECORD_OFFSET < CM_RECORD_OFFSET	/* wrap around check; does this work with int == u64 ? */
				|| record_data_length + CM_RECORD_OFFSET > rpc->args.cl.call->ndr_len
				)
			{
				CM_CL_DOWN_INFO_TYPE dinfo;
				CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cm_cl_rpc_read_done: dev(%u) parse error: block.type(0x%x).version(0x%x) slot(%d) subslot(%d) index(%d) length(%d) rv1(%d) rv2(%d) ndr_len(%d) padding_1(%d)"
					, argr->device_nr
					, block_type, block_version
					, slot_nr, subslot_nr, index
					, record_data_length, read->rec.ret_val_1, read->rec.ret_val_2
					, rpc->args.cl.call->ndr_len, padding_1
					);
				read->rec.cm_pnio_err = CM_PNIO_FAULTY_RTA (CM_AR_REASON_PROT);

				CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_READ));

				cm_cl_arsm_down (argr, &dinfo);

				rc = CM_ERR_LOWER_RPC;
			}
			else
			{
				/* good, everything goes ok */
				read->rec.data_length = CM_RECORD_OFFSET + record_data_length;
			}
		}
	}

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_cl_rpc_read_done: dev(%u) ndr_len(%d) rsp(%d) pnio_status(0x%x) nca_status(0x%x) read_implicit(%d)"
		, argr->device_nr
		, rpc->args.cl.call->ndr_len, rc, read->rec.cm_pnio_err, nca_status
		, rpc->args.cl.call->opnum == CLRPC_PNIO_OPNUM_READ_IMPLICIT ? 1 : 0
		);

	cm_cl_ar_callback (argr, rc, upper);

	cm_cl_prm_trigger (argr, CM_CL_WF_RPC_RECORD_CNF);

	if (! cm_cl_bindsm_is_identified (argr))
	{
		cm_cl_bindsm_trigger (argr, 0);	/* cleanup */
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_rpc_write_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
	)
{
	CM_UPPER_RQB_PTR_TYPE upper;
	LSA_UINT16 rc;
	LSA_BOOL was_prm;

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
	{
		was_prm = LSA_FALSE;
	}
	else if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF))
	{
		was_prm = LSA_TRUE;
	}
	else
	{
		CM_FATAL ();
		return;
	}

	if (CmListIsEmpty (&argr->cm.cl.record_list))
	{
		CM_FATAL ();/* no record waiting */
		return;
	}

	CmListRemoveHead (&argr->cm.cl.record_list, upper, CM_UPPER_RQB_PTR_TYPE);

	if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)
	{
		rc = cm_cl_rpc_record_write_done (channel, argr, rpc, upper);
	}
	else
	{
		CM_FATAL1 (CM_RQB_GET_OPCODE (upper));
		return;
	}

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev.prm_state(%d) rc(%d)"
		, argr->cm.cl.prm.state
		, rc
		);

	if (was_prm)
	{
		/* free internal upper */
		cm_cl_prm_rqb_free (argr, upper);

		cm_cl_prm_trigger (argr, CM_CL_WF_RPC_PRM_WRITE_CNF);
	}
	else
	{
		/* normal record-write cnf */
		cm_callback (channel, rc, &upper);

		cm_cl_prm_trigger (argr, CM_CL_WF_RPC_RECORD_CNF);
	}
}


/*------------------------------------------------------------------------------
//	PRM-Read - retrigger CMSV-CMITimeout
//----------------------------------------------------------------------------*/

#define CM_CL_PRM_READ_NONE     0
#define CM_CL_PRM_READ_RUNNING  1


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_start (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	LSA_UINT32 read_timeout;

	/*
	//	see	CMInitiatorTriggerTimeoutFactor = CMInitiatorActivityTimeoutFactor DIV 3
	//	and Table CMInitiatorTriggerTimeoutFactor
	//	| 33 .. 333 | With a time base of 100 ms |
	//
	*/
	read_timeout  = argr->ar.cmi_timeout100ms * 100;/* msec */
	read_timeout /= 3;
	if (! (33 * 100 <= read_timeout && read_timeout <= 333 * 100))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u): prm-read: resize read_timeout(%u) to 3300 msec"
			, argr->device_nr, read_timeout
			);
		read_timeout = 33 * 100;/* msec */
	}
	argr->cm.cl.prm.read_timeout = read_timeout;	/* msec */

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): prm-read: starting"
		, argr->device_nr
		);

	CM_ASSERT (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE);
	argr->cm.cl.prm.read_state = CM_CL_PRM_READ_RUNNING;

	cm_cl_prm_read_retrigger (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_retrigger (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_RUNNING)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u): prm-read: retrigger read-record with timeout(%u)"
			, argr->device_nr, argr->cm.cl.prm.read_timeout
			);
		cm_oneshot_trigger (&argr->cm.cl.prm.read_oneshot, argr->cm.cl.prm.read_timeout);
	}
	else
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): prm-read: retrigger stopped in meantime"
			, argr->device_nr
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_timeout (
	CM_ONESHOT_PTR_TYPE timeout,
	LSA_VOID_PTR_TYPE void_argr
	)
{
	CM_AR_GRAPH_PTR_TYPE argr = (CM_AR_GRAPH_PTR_TYPE)void_argr;

	LSA_UNUSED_ARG (timeout);

	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): prm-read: timeout stopped in meantime"
			, argr->device_nr
			);
		return;	/* timeout to late */
	}

	cm_cl_prm_read_rpc_trigger_req (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_rpc_trigger_req (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CHANNEL_PTR_TYPE channel = argr->cm.cl.dev->channel;
	CM_UPPER_RQB_PTR_TYPE upper;
	LSA_UINT32 alloc_len;
	CM_COMMON_MEM_U8_PTR_TYPE ndr_data;
	CM_CL_DOWN_INFO_TYPE dinfo;

	CM_ASSERT (cm_is_not_null (channel));

	/* build the read-record */
	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u): prm-read: start rpc-read of rpc-trigger record"
		, argr->device_nr
		);

	upper = cm_upper_alloc_rqb (channel);
	if (cm_is_null (upper))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u): prm-read: no cm_upper_alloc_rqb()"
			, argr->device_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	alloc_len = CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE + 0 /*2009-12-03: this is a record with no body*/;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (&ndr_data), cm_null_user_id, alloc_len, channel->sysptr);
	if (cm_is_null (ndr_data))
	{
		cm_upper_free_rqb (channel, upper);
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u): prm-read: no mem for ndr_data, alloc_len(%u)"
			, argr->device_nr, alloc_len
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	CM_RQB_SET_OPCODE (upper,           CM_OPC_CL_RECORD_READ);

	upper->args.cl.read->device_nr = argr->device_nr;

	upper->args.cl.read->rec.api              = 0;
	upper->args.cl.read->rec.slot_nr          = 0;
	upper->args.cl.read->rec.subslot_nr       = 0;
	upper->args.cl.read->rec.record_index     = CM_RECORD_INDEX_RPC_TRIGGER;
	upper->args.cl.read->rec.sequence_nr      = 0;
	upper->args.cl.read->rec.data_length      = alloc_len;
	upper->args.cl.read->rec.data             = ndr_data;
	upper->args.cl.read->rec.cm_pnio_err      = 0;
	upper->args.cl.read->rec.ret_val_1        = 0;
	upper->args.cl.read->rec.ret_val_2        = 0;
	upper->args.cl.read->rec.readquery_length = 0;

	/* all prepared, now let trigger read-write-statemachine */
	CmListInsertTail (&argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)upper);
	cm_cl_prm_trigger (argr, 0);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_rpc_trigger_cnf (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE upper
	)
{
	LSA_UINT16 rc;

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): prm-read: rpc-trigger record done"
		, argr->device_nr
		);

	CM_FREE_UPPER_MEM (&rc, upper->args.cl.read->rec.data, argr->ar_com.channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	cm_upper_free_rqb (argr->ar_com.channel, upper);

	/* retrigger next timeout */
	cm_cl_prm_read_retrigger (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_read_stop (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_RUNNING)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u): prm-read: stopping"
			, argr->device_nr
			);

		argr->cm.cl.prm.read_state = CM_CL_PRM_READ_NONE;
		cm_oneshot_stop (&argr->cm.cl.prm.read_oneshot);
	}

	CM_ASSERT (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE);
}


/*------------------------------------------------------------------------------
//	ApplReadyTimer, used for two timeouts
//	__RemoteApplReady		.. between PrmEnd and ApplRdy
//	__PreDataInd			.. between ApplRdy and PreData.ind
//----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_prm_ardy_timeout_start (
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 which_one
	)
{
	switch (which_one)
	{
	case CM_CL_ARDY_TIMEOUT__RemoteApplReady:
		/*
		//	Remote_Application_Ready_Timeout (Value <= 300 sec)  see part-2
		//	  The parameter Remote_Application_Ready_Timeout represents the deadline between
		//	  IODControlRes (ControlCommand.PrmEnd) and IOXControlReq (ControlCommand.ApplicationReady).
		*/
		CM_ASSERT (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP);

		argr->cm.cl.prm.ardy_timer        = which_one;
		argr->cm.cl.prm.ardy_timeout_msec = 300 * 1000;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): ardy-timer: starting remote_ardy_timeout(%u)"
			, argr->device_nr, argr->cm.cl.prm.ardy_timeout_msec
			);

		cm_oneshot_trigger (&argr->cm.cl.prm.ardy_oneshot, argr->cm.cl.prm.ardy_timeout_msec);
		break;

	case CM_CL_ARDY_TIMEOUT__PreDataInd:
		CM_ASSERT (argr->cm.cl.applrdy_state == CM_CL_APPL_READY_STATE_UP);

		argr->cm.cl.prm.ardy_timer        = which_one;
		argr->cm.cl.prm.ardy_timeout_msec = 3000;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): ardy-timer: starting, predata_timeout(%u)"
			, argr->device_nr, argr->cm.cl.prm.ardy_timeout_msec
			);

		cm_oneshot_trigger (&argr->cm.cl.prm.ardy_oneshot, argr->cm.cl.prm.ardy_timeout_msec);
		break;

	default:
		CM_FATAL ();
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_ardy_timeout (
	CM_ONESHOT_PTR_TYPE timeout,
	LSA_VOID_PTR_TYPE void_argr
	)
{
	CM_AR_GRAPH_PTR_TYPE argr = (CM_AR_GRAPH_PTR_TYPE)void_argr;
	LSA_UINT16 ardy_timer     = argr->cm.cl.prm.ardy_timer;

	LSA_UNUSED_ARG (timeout);

	argr->cm.cl.prm.ardy_timer = 0;

	switch (ardy_timer)
	{
	case CM_CL_ARDY_TIMEOUT__RemoteApplReady:
		{
			CM_CL_DOWN_INFO_TYPE dinfo;

			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u): ardy-timer: remote_application_ready_timeout"
				, argr->device_nr
				);

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_ARDY, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_ARDY));
			cm_cl_arsm_down (argr, &dinfo);
		}
		break;

	case CM_CL_ARDY_TIMEOUT__PreDataInd:
		{
			LSA_BOOL is_up   = cm_ar_com_is_up (argr);
			LSA_BOOL is_down = cm_ar_com_is_down (argr);

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u): ardy-timer: if !is_up(%u) && !is_down(%u) do PreData.ind"
				, argr->device_nr, is_up, is_down
				);

			if (! is_up  &&  ! is_down)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
				cm_cl_arsm (argr);
			}
			else
			{
				/* too late, but not state switch yet, e.g. CM_CL_PRMMODE_PLUG */
			}
		}
		break;

	default:
		CM_FATAL1 (argr->cm.cl.prm.ardy_timer);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_prm_ardy_timeout_stop (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	argr->cm.cl.prm.ardy_timer = 0;

	if (cm_oneshot_is_running (&argr->cm.cl.prm.ardy_oneshot))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): ardy-timer: stopped, remaining ticks(%u)"
			, argr->device_nr, argr->cm.cl.prm.ardy_oneshot.ticks
			);

		cm_oneshot_stop (&argr->cm.cl.prm.ardy_oneshot);
	}
}


/*------------------------------------------------------------------------------
//	UUID_IO_ControllerInterface :: rpc implicit read
//----------------------------------------------------------------------------*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: read-implicit (make own file, etc)")
#endif /* CM_MESSAGE */

typedef struct cm_cl_record_params_tag {

	/* rpc opnum things */

	LSA_BOOL is_read;
	LSA_BOOL is_read_implicit;

	/* parsed things */

	LSA_UINT16 seq_number;

	CLRPC_UUID_TYPE ar_uuid;

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;
	LSA_UINT16 padding1;
	LSA_UINT16 index;

	LSA_UINT32 record_data_length;

	CLRPC_UPPER_MEM_U8_PTR_TYPE target_ar_uuid_ptr;

} CM_CL_RECORD_PARAMS_TYPE;

typedef CM_CL_RECORD_PARAMS_TYPE *CM_CL_RECORD_PARAMS_PTR_TYPE;

/*----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_record_header_parse (
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_CL_RECORD_PARAMS_PTR_TYPE req
	)
{
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_len;

	/***/

	#if CLRPC_PNIO_OFFSET + CM_BLK_RDWR_SIZE != CM_RECORD_OFFSET
	#error "check this!"
	#endif

	/***/

	CM_ASSERT (rpc->args.sv.call->ndr_len >= CLRPC_PNIO_OFFSET);/* see caller */

	data_ptr = rpc->args.sv.call->ndr_data;
	data_len = rpc->args.sv.call->ndr_len;

	if (req->is_read)
	{

		if (data_len != CM_RECORD_OFFSET)
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
				, "read, wrong data_len(%u)"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid ();	/* read, wrong length */
		}
	}
	else	/* write */

	{
		CM_ASSERT (! req->is_read_implicit);/* impossible */

		if (data_len < CM_RECORD_OFFSET)
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
				, "write, data_len(%u) too short"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid ();	/* write, too few data */
		}
	}

	data_ptr += CLRPC_PNIO_OFFSET;
	data_len -= CLRPC_PNIO_OFFSET;

	/***/

	{
		LSA_UINT16 block_type;
		LSA_UINT16 block_length;
		LSA_UINT16 block_version;

		block_type    = CM_GET16_NTOH (data_ptr, CM_BLK_HEADER_type);
		block_length  = CM_GET16_NTOH (data_ptr, CM_BLK_HEADER_length);
		block_version = CM_GET16_NTOH (data_ptr, CM_BLK_HEADER_version);

		/***/

		if (req->is_read)
		{

			if (block_type != CM_BLOCK_TYPE_READ_REQ)
			{
				CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
					, "read, wrong block_type(%u)"
					, block_type
					);
				return CM_PNIO_FAULTY_Record__Block (0);/* read, wrong block-type */
			}
		}
		else/* write */

		{
			if (block_type != CM_BLOCK_TYPE_WRITE_REQ)
			{
				CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
					, "write, wrong block_type(%u)"
					, block_type
					);
				return CM_PNIO_FAULTY_Record__Block (0);/* write, wrong block-type */
			}
		}

		if (block_length != CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE)
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
				, "wrong block_length(%u)"
				, block_length
				);
			return CM_PNIO_FAULTY_Record__Block (1);/* wrong block-length*/
		}

		if ((block_version & 0xFF00) != 0x0100 /*V1.0*/)
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "wrong block_version-high(%u)", block_version);
			return CM_PNIO_FAULTY_Record__Block (2);/* wrong block-version-high */
		}
		if ((block_version & 0x00FF) != 0x0000 /*V1.0*/)
		{
			CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "wrong block_version-low(%u)", block_version);
			return CM_PNIO_FAULTY_Record__Block (3);/* wrong block-version-low */
		}
	}

	/***/

	req->seq_number = CM_GET16_NTOH (data_ptr, CM_BLK_RDWR_seq_number);

	CM_GET_UUID_NTOH (&req->ar_uuid, data_ptr, CM_BLK_RDWR_ar_uuid);

	req->api        = CM_GET32_NTOH (data_ptr, CM_BLK_RDWR_api);
	req->slot_nr    = CM_GET16_NTOH (data_ptr, CM_BLK_RDWR_slot_nr);
	req->subslot_nr = CM_GET16_NTOH (data_ptr, CM_BLK_RDWR_subslot_nr);

	req->padding1 = CM_GET16_NTOH (data_ptr, CM_BLK_RDWR_padding_1);
	req->index    = CM_GET16_NTOH (data_ptr, CM_BLK_RDWR_index);

	req->record_data_length = CM_GET32_NTOH (data_ptr, CM_BLK_RDWR_record_data_length);

	req->target_ar_uuid_ptr = data_ptr + CM_BLK_RDWR_req_rd_implicit_target_ar_uuid;

	data_ptr += CM_BLK_RDWR_SIZE;
	data_len -= CM_BLK_RDWR_SIZE;

	LSA_UNUSED_ARG (data_ptr);	/* lint */
	LSA_UNUSED_ARG (data_len);	/* lint */

	/***/

	if (req->padding1 != 0)
	{

		/* NOTE: 'index' maybe be expanded to 32 bit in the future */
		/* to allow this, we have to check the padding (the HighWord) for zero */

		CM_CL_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP
			, "padding_1 not zero"
			);
		return CM_PNIO_FAULTY_Record__InvalidRange (9);	/* padding_1 not zero */
	}

	/***/

	req->record_data_length += CM_RECORD_OFFSET;/* include the rpc- and record-header, see cm_sv_record_event_indicate() */

	if (req->record_data_length < CM_RECORD_OFFSET)		/* wrap around at 4GB */

	{
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "record-length(%u) too big"
			, req->record_data_length - CM_RECORD_OFFSET
			);
		return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid ();	/* record-length inconsistent with ndr-length */
	}

	if (req->is_read)
	{

		if (req->record_data_length > rpc->args.sv.call->alloc_len)
		{

			/* we must accept this record-read, but we need to shorten the length indicated to the user */

			CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_NOTE_HIGH/* not LEVEL_UNEXP */
				, "read, %u + length(%u) > alloc_len(%u), shortening the request"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->alloc_len
				);

			req->record_data_length = rpc->args.sv.call->alloc_len;
		}
	}
	else	/* write */

	{
		if (req->record_data_length != rpc->args.sv.call->ndr_len)
		{

			CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
				, "write, %u + length(%u) != ndr_len(%u), inconsistency"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->ndr_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid ();	/* write, record-length inconsistent with ndr-length */
		}
	}

	/***/

	return CM_PNIO_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_record_header_update (
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
	)
{
	/***/

	CM_ASSERT (CM_RECORD_OFFSET == CLRPC_PNIO_OFFSET + CM_BLK_RDWR_SIZE);

	/***/

	if (is_read)
	{
		CM_PUT16_HTON (header_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_READ_RSP);
	}
	else
	{
		CM_PUT16_HTON (header_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_WRITE_RSP);
	}

	CM_PUT16_HTON (header_ptr, CM_BLK_HEADER_length, CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE);

	CM_PUT16_HTON (header_ptr, CM_BLK_HEADER_version, 0x0100 /*V1.0*/);

	/*
	 * update the length and additional-values, the rest remains unchanged
	 */

	CM_PUT32_HTON (
		header_ptr
		, CM_BLK_RDWR_record_data_length
		, ndr_length - CM_RECORD_OFFSET
		);

	CM_PUT16_HTON (
		header_ptr
		, CM_BLK_RDWR_rsp_rd_wr_additional_value_1
		, ret_val_1
		);

	CM_PUT16_HTON (
		header_ptr
		, CM_BLK_RDWR_rsp_rd_wr_additional_value_2
		, ret_val_2
		);

	/*
	 * write-rsp: update the pnio-status
	 */

	if (! is_read)
	{

		CM_PUT32_HTON (
			header_ptr
			, CM_BLK_RDWR_rsp_wr_pnio_status
			, pnio_err
			);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16
cm_cl_read_impl_ind_search (
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
	)
{
	LSA_UINT16 i;
	CM_RPC_LOWER_RQB_PTR_TYPE *rind;

	/* rpc == LSA_NULL: search new one
	// rpc != LSA_NULL: search existing one
	*/
	for (i = 0; i < CM_CFG_MAX_CL_IMPLICIT_READ_IND; ++i)
	{
		rind = &channel->usr.cl.ipnio_controller__read_implicit[i];
		if (*rind == rpc)
		{
			return CM_CAST_U16 (i + 1);
		}
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static CM_RPC_LOWER_RQB_PTR_TYPE *
cm_cl_read_impl_ind_from_idx (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 idx
	)
{
	if (1 <= idx && idx <= CM_CFG_MAX_CL_IMPLICIT_READ_IND)
	{
		return &channel->usr.cl.ipnio_controller__read_implicit[idx - 1];
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pnio_c_read_implicit_ind (
	CM_CHANNEL_PTR_TYPE channel,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
	)
{
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	LSA_UINT16 ridx;
	CM_RPC_LOWER_RQB_PTR_TYPE *rind;
	CM_CL_RECORD_PARAMS_TYPE req;
	LSA_UINT32 pnio_err;
	LSA_UINT32 cls;
	LSA_UINT32 cls_allowed;

	/* param check */

	req.is_read          = LSA_TRUE;
	req.is_read_implicit = LSA_TRUE;

	pnio_err = cm_cl_record_header_parse (rpc, &req);
	if (pnio_err != CM_PNIO_ERR_NONE)
	{
		rpc->args.sv.call->ndr_len     = CLRPC_PNIO_OFFSET;
		rpc->args.sv.call->pnio_status = pnio_err;

		CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0);	/* is-a-response: no need to set user_id */
		return;
	}

	if (channel->state != CM_CL_READY)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "channel.state(%u) != Ready, ignore read-implicit"
			, channel->state
			);
		pnio_err = CM_PNIO_FAULTY_Record__ResourceBusy (0);
		goto complete;
	}

	ridx = cm_cl_read_impl_ind_search (channel, LSA_NULL);
	if (ridx == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "another read-implicit is still running"
			);
		pnio_err = CM_PNIO_FAULTY_Record__ResourceBusy (0);
		goto complete;
	}


	cls = cm_record_index_classify (channel, req.api, req.index, channel->usr.cl.vendor_id);

	cls_allowed = CM_REC_IDX_SIMATIC;

	if ((cls & cls_allowed) != 0  &&  (cls & CM_REC_IDX_READ) != 0)
	{
		CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_PD);
		CM_PD_PORT_PTR_TYPE port;

		if ((cls & CM_REC_IDX_8000) != 0)
		{
			switch (req.index)
			{
			case 0xB000:/* GSY: Sync-Log, RTA SyncID 0 */
			case 0xB001:/* GSY: Sync-Log, RTA SyncID 1 */
			case 0xB050:/* EDD: Ext_PLL control (for RTA+RTC SyncID 0) */
			case 0xB051:/* GSY: Ext_PLL control (for RTA SyncID 1) */
			case 0xB061:/* EDD: Trace-Unit data */
			case 0xB070:/* OHA: PDInterfaceAdditionalInfo */
				port = cm_pd_port_from_id (pd_channel, 0 /*interface*/);
				break;

			case 0xB060:/* EDD: Trace-Unit control */
			default:
				port = cm_pd_port_from_addr (pd_channel, req.slot_nr, req.subslot_nr);
				break;
			}
		}
		else
		{
			port = cm_pd_port_from_id (pd_channel, 0 /*interface*/);
		}

		if (cm_is_null (port))
		{
			pnio_err = CM_PNIO_FAULTY_Record__FeatureNotSupported (0);
			goto complete;
		}
		else if (port->addr.subslot_nr == 0)	/* not (yet) mapped */
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "interface not (yet) mapped, slot(%u) subslot(0x%x)"
				, req.slot_nr, req.subslot_nr
				);
			pnio_err = CM_PNIO_FAULTY_Record__FeatureNotSupported (0);	/* not telling InvalidSlotSubslot because params are ignored... */
			goto complete;
		}
		else
		{
			req.api        = 0;
			req.slot_nr    = port->addr.slot_nr;
			req.subslot_nr = port->addr.subslot_nr;
		}
	}
	else
	{
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unknown index(0x%x) cls(0x%x) cls_allowed(0x%x)"
			, req.index, cls, cls_allowed
			);
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex (0);
		goto complete;
	}

	CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rpc-read-implicit.ind: rpc(0x%08x) opnum(%d) ndr_len(%d)  index(0x%x) slot(%u) subslot(0x%x)"
		, rpc, rpc->args.sv.call->opnum, rpc->args.sv.call->ndr_len
		, req.index, req.slot_nr, req.subslot_nr
		);

	{
		CM_UPPER_RQB_PTR_TYPE pd;

		pd = cm_upper_alloc_rqb (channel);
		if (cm_is_null (pd))
		{
			CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "no mem"
				);
			pnio_err = CM_PNIO_FAULTY_Record__AccessDenied (255);	/* no mem */
			goto complete;
		}

		/***/

		{
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

			prm_rdwr->slot_nr            = req.slot_nr;
			prm_rdwr->subslot_nr         = req.subslot_nr;
			prm_rdwr->record_index       = req.index;
			prm_rdwr->record_data_length = req.record_data_length - CM_RECORD_OFFSET;
			prm_rdwr->record_data        = rpc->args.sv.call->ndr_data + CM_RECORD_OFFSET;

			prm_rdwr->is_local = CM_PD_IS_LOCAL_NO;
			prm_rdwr->cls      = cls;
		}


		rind = cm_cl_read_impl_ind_from_idx (channel, ridx);
		CM_ASSERT (cm_is_not_null (rind));
		CM_ASSERT (cm_is_null (*rind));
		*rind = rpc;/* keep */

		cm_pdcl_request (channel, CM_OPC_PD_PRM_READ_CLIENT, ridx, 0, pd);
	}

	return;	/* pending */

	/***/

complete:

	CM_ASSERT (pnio_err != CM_PNIO_ERR_NONE);

	rpc->args.sv.call->ndr_len     = CM_RECORD_OFFSET;
	rpc->args.sv.call->pnio_status = pnio_err;

	cm_cl_record_header_update (rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET
		, LSA_TRUE	/*read*/
		, rpc->args.sv.call->ndr_len
		, pnio_err, 0, 0
		);

	CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0);	/* is-a-response: no need to set user_id */
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pnio_c_read_implicit_rsp (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE pd
	)
{
	CM_RPC_LOWER_RQB_PTR_TYPE *rind;
	CM_RPC_LOWER_RQB_PTR_TYPE rpc;
	LSA_UINT32 pnio_err;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);
	if (cm_is_null (channel))
	{
		CM_FATAL ();
		return;
	}

	rind = cm_cl_read_impl_ind_from_idx (channel, CM_EDD_GET_USER_ID_DEV (pd));
	if (cm_is_null (rind) || cm_is_null (*rind))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "cannot find ridx(0x%x)", CM_EDD_GET_USER_ID_DEV (pd)
			);
		CM_FATAL ();
		return;
	}

	rpc   = *rind;
	*rind = LSA_NULL;

	if (CM_RQB_GET_RESPONSE (pd) != CM_OK)
	{
		rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET;

		pnio_err = cm_pd_response_to_pnio_err (CM_RQB_GET_RESPONSE (pd), LSA_TRUE);
	}
	else
	{
		rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET + pd->args.pd.prm_rdwr->record_data_length;

		pnio_err = CM_PNIO_ERR_NONE;
	}

	cm_cl_record_header_update (rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET
		, LSA_TRUE	/*read*/
		, rpc->args.sv.call->ndr_len
		, pnio_err, 0, 0
		);

	rpc->args.sv.call->pnio_status = pnio_err;

	CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0);	/* is-a-response: no need to set user_id */

	cm_upper_free_rqb (channel, pd);
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
