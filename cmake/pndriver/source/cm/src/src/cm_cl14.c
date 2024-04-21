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
/*  F i l e               &F: cm_cl14.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   614
#define CM_MODULE_ID        614

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_RECORD_READ
// handle opcode CM_OPC_CL_RECORD_READ_QUERY
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_opc_record_read (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_UPPER_CL_RECORD_READ_PTR_TYPE read;
	LSA_UINT32 local_ip_addr, local_subnet, local_gateway;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));
	CM_ASSERT (cm_is_not_null (rb->args.void_ptr));

	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = rb->args.cl.read;
	read->rec.cm_pnio_err = CM_PNIO_ERR_NONE;
	read->rec.ret_val_1   = 0;
	read->rec.ret_val_2   = 0;

	argr = cm_cl_ar_from_device (channel, read->device_nr);
	if (cm_is_null (argr))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: !dev(%d)", read->device_nr);
		return CM_ERR_PARAM;
	}

	if (argr->cm.cl.dev->wf & CM_CL_DEV__WF_ADD_DONE)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: device_add running");
		return CM_ERR_SEQUENCE;
	}

	if (argr->cm.cl.dev->wf & CM_CL_DEV__WF_REMOVE_DONE)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: device_remove running");
		return CM_ERR_SEQUENCE;
	}

	if (read->rec.data_length < CM_RECORD_OFFSET)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: !data_length(%d)", read->rec.data_length);
		return CM_ERR_PARAM;
	}

	/*	not checked:  read->rec.data_length > argr->cm.cl.dev->dev_rpc.max_supported_record_length
	//	this means:   the IOC may have a larger read buffer as the IOD can fill
	*/

	if (cm_is_null (read->rec.data))
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "record_read: !data-pointer with data_length(%d)", read->rec.data_length);
		return CM_ERR_PARAM;
	}

	if (! cm_oha_get_ip_suite (channel, &local_ip_addr, &local_subnet, &local_gateway))
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: have no local ip_addr");
		return CM_ERR_SEQUENCE;
	}

	if (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ)
	{
		read->rec.readquery_length = 0;
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_CL_RECORD_READ_QUERY);

		if (CM_RECORD_OFFSET + read->rec.readquery_length > read->rec.data_length)
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "record_read: CM_RECORD_OFFSET(%u) + rec.readquery_length(%u) > rec.data_length(%u)"
				, CM_RECORD_OFFSET, read->rec.readquery_length, read->rec.data_length
				);
			return CM_ERR_PARAM;
		}
	}

	if (read->rec.record_index == CM_RECORD_INDEX_RPC_TRIGGER) /*AP01304789*/
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: record_index(0x%x) not allowed", read->rec.record_index);
		read->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex (10);
		return CM_OK;
	}
	else if (read->rec.record_index < 0xF000)
	{
		switch (argr->cm.cl.state)
		{
		case CM_DST_COM_ACTIVATE:
		case CM_DST_PRM:
		case CM_DST_WF_APPL_READY_IND:
		case CM_DST_WF_NEW_DATA:
		case CM_DST_IN_DATA:
			break;

		case CM_DST_PASSIVE:
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: record_index(0x%x) not allowed, device passive"
				, read->rec.record_index);
			return CM_ERR_SESSION;

		case CM_DST_WF_SCHEDULE:
		case CM_DST_NARE:
		case CM_DST_CONNECT:
		case CM_DST_COM_START:
		case CM_DST_WF_DOWN:
		default:
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: dev(%u) state(%u) currently not readable"
				, read->device_nr, argr->cm.cl.state
				);

			return CM_ERR_ABORT;
		}
	}
	else if (cm_is_not_null (argr->cm.cl.mtd) && (argr->cm.cl.state == CM_DST_PASSIVE))
	{
		CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

		if (argr->cm.cl.mtd->properties & CM_PD_MTD_PROP_IP_SUITE)
		{
			LSA_UINT32 dev_ip = dev->dev_addr.device_ip;

			if (!cm_cl_mp_tailor_ipsuite (dev, local_ip_addr, local_subnet))
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_read: dev(%u) tailored IPv4 suite is invalid"
					, read->rec.record_index);
				return CM_ERR_ABORT;
			}

			if (dev_ip != dev->dev_addr.device_ip)
			{
				CM_CL_TRACE_01 (dev->channel->trace_idx , LSA_TRACE_LEVEL_NOTE, "record_read: dev(%u) IPv4 suite changed", argr->device_nr);
				dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
			}
		}

		if (argr->cm.cl.mtd->properties & CM_PD_MTD_PROP_NOS)
		{
			LSA_UINT8 dev_name[240];
			LSA_UINT16 dev_name_length;
			LSA_UINT16 ioc_name_length;
			CM_COMMON_MEM_U8_PTR_TYPE  ioc_name;

			if (! cm_oha_get_name_of_station (channel, &ioc_name_length, &ioc_name))
			{
				CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "record_read: dev(%u) NameOfStation adaption missing (PDev with invalid NoS)", argr->device_nr);
				return CM_ERR_SEQUENCE;
			}

			if (dev->dev_addr.device_name_length + 1 + ioc_name_length > 240)
			{
				CM_CL_TRACE_01 (dev->channel->trace_idx , LSA_TRACE_LEVEL_UNEXP, "record_read: dev(%u) length of tailored NameOfStation of IOD >240", argr->device_nr);
				return CM_ERR_ABORT;
			}

			dev_name_length = dev->dev_addr.tailored_device_name_length;
			CM_MEMCPY (dev_name, dev->dev_addr.tailored_device_name, dev->dev_addr.tailored_device_name_length);

			dev->dev_addr.tailored_device_name_length = cm_cl_mp_tailor_name (dev->dev_addr.tailored_device_name
				, dev->dev_addr.device_name, dev->dev_addr.device_name_length, ioc_name, ioc_name_length
			);

			if (dev_name_length != dev->dev_addr.tailored_device_name_length || CM_MEMCMP (dev_name, dev->dev_addr.tailored_device_name, dev_name_length) != 0)
			{
				CM_CL_TRACE_01 (dev->channel->trace_idx , LSA_TRACE_LEVEL_NOTE, "record_read: dev(%u) NoS changed", argr->device_nr);
				dev->bindsm.state = CM_CL_BINDSM_STATE_UNKNOWN;
			}
		}
	}


	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "record_read: dev(%u) index(0x%x) data_length(%u) query_length(%u)"
		, read->device_nr
		, read->rec.record_index
		, read->rec.data_length
		, read->rec.readquery_length
		);

	CmListInsertTail (&argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)rb);

	cm_cl_bindsm_read (argr);

	return CM_OK_PENDING;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32
cm_cl_record_write_index_check (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write
	)
{
	CM_AR_GRAPH_AP_PTR_TYPE ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub;

	LSA_UNUSED_ARG (write);

	if (write->rec.record_index < 0x8000)
	{
		/* user specific */
	}
	else if (write->rec.record_index < 0xC000)
	{
		/* Subslot specific */
		cm_ar_graph_find_api_slot_subslot (argr, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr, &ap, &mod, &sub);
		if (cm_is_null (ap))
		{
			return CM_PNIO_FAULTY_Record__InvalidArea (0);
		}
		else if (cm_is_null (mod))
		{
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot (1);
		}
		else if (cm_is_null (sub))
		{
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot (2);
		}
	}
	else if (write->rec.record_index < 0xE000)
	{
		/* Slot specific */
		cm_ar_graph_find_api_slot_subslot (argr, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr, &ap, &mod, LSA_NULL);

		if (cm_is_null (ap))
		{
			return CM_PNIO_FAULTY_Record__InvalidArea (1);
		}
		else if (cm_is_null (mod))
		{
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot (3);
		}
	}
	else if (write->rec.record_index < 0xF000)
	{
		/* AR specific */
	}
	else if (write->rec.record_index < 0xF800)
	{
		/* API specific */
		cm_ar_graph_find_api_slot_subslot (argr, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr, &ap, LSA_NULL, LSA_NULL);

		if (cm_is_null (ap))
		{
			return CM_PNIO_FAULTY_Record__InvalidArea (2);
		}
	}
	else
	{
		/* device specific */
		LSA_UINT32 idx = write->rec.record_index;
		CM_ASSERT (idx <= 0xFFFF);

		LSA_UNUSED_ARG (idx);
	}

	return CM_PNIO_ERR_NONE;
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_CL_RECORD_WRITE
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_cl_opc_record_write (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
	)
{
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE write;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (rb));
	CM_ASSERT (cm_is_not_null (rb->args.void_ptr));

	write = rb->args.cl.write;
	write->rec.cm_pnio_err = CM_PNIO_ERR_NONE;
	write->rec.ret_val_1   = 0;
	write->rec.ret_val_2   = 0;

	argr = cm_cl_ar_from_device (channel, write->device_nr);
	if (cm_is_null (argr))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) not found", write->device_nr);
		return CM_ERR_PARAM;
	}

	if (argr->session_key != write->session_key)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) : session_key(%u) != ar.session_key(%u)",
			write->device_nr, write->session_key, argr->session_key
			);
		return CM_ERR_SESSION;
	}

	if (! (argr->cm.cl.state == CM_DST_IN_DATA  ||  argr->cm.cl.state == CM_DST_WF_NEW_DATA /*v2.3*/))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "dev(%u) : !IN_DATA, arState(%d) usrState(%u)",
			write->device_nr, argr->cm.cl.state, argr->cm.cl.usr_state
			);
		return CM_ERR_SEQUENCE;
	}

	write->rec.cm_pnio_err = cm_cl_record_write_index_check (argr, write);

	if (write->rec.cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);

		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) : !allowed, record_index(0x%x) api(%u) slot(%u) subslot(0x%x) pnio_err(0x%x)"
			, write->device_nr, write->rec.record_index
			, write->rec.api, write->rec.slot_nr, write->rec.subslot_nr, write->rec.cm_pnio_err
			);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK /*!*/;
	}

	if (write->rec.record_index == 0xE040 /* WriteMultiple */  ||  write->rec.record_index == 0x80B0 /* CombinedObjectContainer */)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR, "record_index(0x%x) not allowed", write->rec.record_index);
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex (10);
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK /*!*/;
	}

	if (write->rec.data_length > argr->cm.cl.dev->dev_rpc.max_supported_record_length)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u):  data_length(%u) > max_supported_record_length(%u)"
			, write->device_nr
			, write->rec.data_length, argr->cm.cl.dev->dev_rpc.max_supported_record_length
			);
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__WriteLengthError (0);
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK /*!*/;
	}

	if (write->rec.data_length < CM_RECORD_OFFSET)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) :  data_length(%u) < CM_RECORD_OFFSET(%u)"
			, write->device_nr
			, write->rec.data_length, CM_RECORD_OFFSET
			);
		write->rec.data_length = CM_RECORD_OFFSET;
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record__WriteLengthError (0);
		write->rec.cm_pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite (write->rec.cm_pnio_err);
		cm_callback (channel, CM_OK, &rb);
		return CM_OK /*!*/;
	}

	CM_ASSERT (argr->cm.cl.dev->rpc_client_id != CM_RPC_INVALID_CLIENT_ID);

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) : record-write: index(0x%x)"
		, write->device_nr, write->rec.record_index
		);

	CmListInsertTail (&argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)rb);

	cm_cl_prm_trigger (argr, 0);

	return CM_OK;
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
