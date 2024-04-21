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
/*  F i l e               &F: cm_arcb_helper.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Helper functions for CM_OPC_CL_DEVICE_ADD and CM_OPC_MC_DEVICE_ADD       */
/*                                                                           */
/*****************************************************************************/

#define CM_MODULE_ID       690
#define LTRC_ACT_MODUL_ID  690

#include "cm_inc.h"         /* note: not "cm_int.h"; this file shall be compilable with CM headers only */
#include "cm_arcb_helper.h" /* note: usage of arcb-helper is optional; thus not included in cm_inc.h */

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS


#define cm_is_null(ptr_)			(LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr_), LSA_NULL))
#define cm_is_not_null(ptr_)		(! cm_is_null(ptr_))


#define CM_ARCB_CAST_UPPER_MEM_PTR_PTR(val_)	((CM_UPPER_MEM_PTR_TYPE *)(val_))

/*lint -e(708) Info 708: union initialization */
static const LSA_USER_ID_TYPE arcb_null_user_id = {0}; /* see alloc-macros */


/*------------------------------------------------------------------------------
// copy a blob to a conditionally allocated destination
//----------------------------------------------------------------------------*/

static LSA_UINT16
cm_arcb_blob_alloc_copy (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_MEM_U8_PTR_TYPE	*dst,
	CM_UPPER_MEM_U8_PTR_TYPE	src,
	LSA_UINT16					len,

	LSA_BOOL make_copy /* see cm_arcb_free(), flag "made_copy_of_xxx" */
)
{
	if (make_copy)
	{
		CM_UPPER_MEM_PTR_TYPE cpy;

		CM_ALLOC_UPPER_MEM (& cpy, arcb_null_user_id, len, sys_ptr);
		if (cm_is_null (cpy))
		{
			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", len);
			*dst = LSA_NULL;
			return 0;
		}
		else
		{
			CM_MEMCPY (cpy, src, len);
			*dst = (CM_UPPER_MEM_U8_PTR_TYPE) cpy;
		}
	}
	else
	{
		*dst = src;
	}

	return len;
}


/*------------------------------------------------------------------------------
// build CM_CL_DEVICE_ADD_TYPE::dev_addr
//----------------------------------------------------------------------------*/

LSA_VOID
cm_arcb_set_nare (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_NARE_PTR_TYPE  nare,

	LSA_UINT32		properties,
	LSA_UINT16		device_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE
					device_name,
	LSA_UINT32		device_ip,
	LSA_UINT32		device_subnet_mask,
	LSA_UINT32		device_gateway_ip,

	LSA_BOOL make_copy_of_device_name /* see cm_arcb_free_nare(), flag "made_copy_of_device_name" */
	)
{
	CM_UPPER_MEM_U8_PTR_TYPE device_name_copy;

	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (nare));

	/* note: argument checks not here */

	if(make_copy_of_device_name)
	{
		CM_ALLOC_UPPER_MEM(CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& device_name_copy), arcb_null_user_id, device_name_length, sys_ptr);
		CM_ASSERT (cm_is_not_null(device_name_copy));

		CM_MEMCPY( device_name_copy, device_name, device_name_length);
		nare->device_name = device_name_copy;
	}
	else
	{
		nare->device_name = device_name;
	}

	nare->properties			= properties;
	nare->device_name_length	= device_name_length;
	nare->device_ip				= device_ip;
	nare->device_subnet_mask	= device_subnet_mask;
	nare->device_gateway_ip		= device_gateway_ip;

	CmListInitialize (& nare->device_alias_list);
}

/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE
cm_arcb_append_nare_alias (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_NARE_PTR_TYPE  nare,

	LSA_UINT16			alias_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE
						alias_name,
	LSA_BOOL make_copy_of_alias_name /* see cm_arcb_free_nare(), flag "made_copy_of_alias_name" */
	)
{
	CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE  alias;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (nare));

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& alias),
			arcb_null_user_id, sizeof (*alias), sys_ptr
		);
	if (cm_is_null (alias))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*alias));
		return LSA_NULL;
	}

	if (alias_name_length == 0)
	{
		alias->name = LSA_NULL;
	}
	else if(make_copy_of_alias_name)
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& alias->name),
				arcb_null_user_id, alias_name_length, sys_ptr
			);
		if (cm_is_null (alias->name))
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, alias, sys_ptr);
			CM_ASSERT (rc == LSA_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", alias_name_length);
			return LSA_NULL;
		}

		CM_MEMCPY (alias->name, alias_name, alias_name_length);
	}
	else
	{
		alias->name = alias_name;
	}

	alias->name_length = alias_name_length;

	CmListInsertTail (& nare->device_alias_list, & alias->link);

	return alias;
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_arcb_free_nare (
	LSA_SYS_PTR_TYPE sys_ptr,
	CM_UPPER_ARCB_NARE_PTR_TYPE nare,
	LSA_BOOL made_copy_of_device_name, /* see cm_arcb_set_nare(), flag "make_copy_of_device_name" */
	LSA_BOOL made_copy_of_alias_name  /* see cm_arcb_append_alias_name(), flag "make_copy_of_alias_name" */
	)
{
	CM_ASSERT (cm_is_not_null (nare));

	while (! CmListIsEmpty (& nare->device_alias_list))
	{
		CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE  alias;
		LSA_UINT16  rc;

		CmListRemoveHead (& nare->device_alias_list, alias, CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (alias));

		if(cm_is_not_null(alias->name) && made_copy_of_alias_name)
		{
			CM_FREE_UPPER_MEM (& rc, alias->name, sys_ptr);
			CM_ASSERT (rc == LSA_OK);
		}

		CM_FREE_UPPER_MEM (& rc, alias, sys_ptr);
		CM_ASSERT (rc == LSA_OK);
	}

	if (made_copy_of_device_name)
	{
		LSA_UINT16 rc;
		CM_FREE_UPPER_MEM(&rc, nare->device_name, sys_ptr);
		CM_ASSERT(rc == LSA_OK);
		nare->device_name = 0;
	}
}


/*------------------------------------------------------------------------------
// build CM_CL_DEVICE_ADD_TYPE::dev_rpc
//----------------------------------------------------------------------------*/

LSA_VOID
cm_arcb_set_rpc (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_RPC_PTR_TYPE  rpc,
	LSA_UINT16		vendor_id,
	LSA_UINT16		device_id,
	LSA_UINT16		instance,
	LSA_UINT32		max_supported_record_length
	)
{
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (rpc));

	/* note: argument checks not here */

	rpc->vendor_id			= vendor_id;
	rpc->device_id			= device_id;
	rpc->instance			= instance;

	rpc->max_supported_record_length	= max_supported_record_length;
}


/*------------------------------------------------------------------------------
// build CM_CL_DEVICE_ADD_TYPE::arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_ADD_PTR_TYPE
cm_arcb_new_io_ar (
	LSA_SYS_PTR_TYPE	sys_ptr,

	LSA_UINT16			device_nr,
	LSA_UINT16			ar_nr,
	LSA_UINT16			ar_type,
	CLRPC_UUID_TYPE		ar_uuid,
	LSA_UINT32			ar_properties,
	CLRPC_UUID_CONST_PTR_TYPE	ir_data_uuid, /* LSA_NULL or IRDataUUID */
	LSA_UINT8			sr_ar_number
	)
{
	CM_UPPER_ARCB_ADD_PTR_TYPE  ar;
	LSA_UNUSED_ARG (sys_ptr);
	LSA_UNUSED_ARG (device_nr); /* note: parameter is obsolete */

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ar),
			arcb_null_user_id, sizeof (*ar), sys_ptr
		);
	if (cm_is_null (ar))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ar));
		return LSA_NULL;
	}

	/* note: argument checks not here */

	ar->ar_nr               = ar_nr;
	ar->ar_type				= ar_type;
	ar->ar_uuid				= ar_uuid;
	ar->ar_properties		= ar_properties;

	ar->prm					= LSA_NULL;

	ar->ir_info				= LSA_NULL;

	if (cm_is_not_null(ir_data_uuid))
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ar->ir_info),
				arcb_null_user_id, sizeof (*ar->ir_info), sys_ptr
			);
		if (cm_is_null (ar->ir_info))
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, ar, sys_ptr);
			CM_ASSERT (rc == LSA_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ar->ir_info));
			return LSA_NULL;
		}

		ar->ir_info->ir_data_uuid = *ir_data_uuid;

		CmListInitialize (& ar->ir_info->ir_info_sub_list);
	}

	if (ar_type == CM_AR_TYPE_SINGLE_SYSRED)
	{
		/* update ARUUID::Selector bit 0-2 */
		ar->ar_uuid.node[5] = (ar->ar_uuid.node[5] & 0xF8) | sr_ar_number;
	}

	CmListInitialize (& ar->iocr_list);

	CmListInitialize (& ar->iocr_local_list);

	ar->alarm_cr			= LSA_NULL;

	CmListInitialize (& ar->ap_list);

	CmListInitialize (& ar->mcr_list);

	ar->sr_info				= LSA_NULL;

	ar->arvendor_blob.arvendor_data_len   = 0;
	ar->arvendor_blob.arvendor_data       = LSA_NULL;

	ar->submodule_properties_blob.submodule_properties_data_len = 0;
	ar->submodule_properties_blob.submodule_properties_data     = LSA_NULL;

	ar->rs_info.rs_info_data_len = 0;
	ar->rs_info.rs_info_data     = LSA_NULL;

	CmListInitialize (& ar->ar_prm_record_list);

	return ar;
}


/*------------------------------------------------------------------------------
// set the ar-vendor blob's data and length to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_arvendor (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					arvendor_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	arvendor_data,

	LSA_BOOL make_copy_of_arvendor_data /* see cm_arcb_free(), flag "made_copy_of_arvendor_data" */
	)
{
	if ( cm_is_not_null(arcb->arvendor_blob.arvendor_data) )
	{
		CM_MEM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP, "ARVendor data already added to arcb.");
		return LSA_NULL;
	}

	arcb->arvendor_blob.arvendor_data_len = cm_arcb_blob_alloc_copy (sys_ptr
		, &arcb->arvendor_blob.arvendor_data
		, arvendor_data, arvendor_data_len, make_copy_of_arvendor_data);

	return arcb->arvendor_blob.arvendor_data;
}


/*------------------------------------------------------------------------------
// append the SubmoduleProperties table to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_submodule_properties (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					submodule_properties_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	submodule_properties_data,

	LSA_BOOL make_copy_of_submodule_properties_data /* see cm_arcb_free(), flag "made_copy_of_submodule_properties_data" */
	)
{
	if ( cm_is_not_null(arcb->submodule_properties_blob.submodule_properties_data) )
	{
		CM_MEM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP, "SubmoduleProperties table already added to arcb.");
		return LSA_NULL;
	}

	arcb->submodule_properties_blob.submodule_properties_data_len = cm_arcb_blob_alloc_copy (sys_ptr
		, &arcb->submodule_properties_blob.submodule_properties_data
		, submodule_properties_data, submodule_properties_data_len
		, make_copy_of_submodule_properties_data);

	return arcb->submodule_properties_blob.submodule_properties_data;
}


/*------------------------------------------------------------------------------
// append the RSInfo block to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_rs_info (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					rs_info_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	rs_info_data,

	LSA_BOOL make_copy_of_rs_info_data /* see cm_arcb_free(), flag "made_copy_of_rs_info_data" */
	)
{
	if (cm_is_not_null(arcb->rs_info.rs_info_data))
	{
		CM_MEM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP, "RSInfo block already added to arcb.");
		return LSA_NULL;
	}

	arcb->rs_info.rs_info_data_len = cm_arcb_blob_alloc_copy (sys_ptr
		, &arcb->rs_info.rs_info_data
		, rs_info_data, rs_info_data_len
		, make_copy_of_rs_info_data);

	return arcb->rs_info.rs_info_data;
}


/*------------------------------------------------------------------------------
// append a new ar-related prm-record to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_arcb_append_prm_record (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT32					record_index,
	LSA_UINT16					record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	record_data,

	LSA_BOOL make_copy_of_record_data /* see cm_arcb_free(), flag "made_copy_of_record_data" */
	)
{
	CM_UPPER_ARCB_RECORD_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	if (make_copy_of_record_data)
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr->record_data),
				arcb_null_user_id, record_data_len, sys_ptr
			);
		if (cm_is_null (ptr->record_data))
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, ptr, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", record_data_len);
			return LSA_NULL;
		}

		CM_MEMCPY (ptr->record_data, record_data, record_data_len);
	}
	else
	{
		ptr->record_data = record_data;
	}

	ptr->record_index			= record_index;
	ptr->transfer_sequence		= 0; /* undefined transfer sequence, because ar-related-index always ordered by index */
	ptr->record_data_len		= record_data_len;

	CmListInsertTail (&arcb->ar_prm_record_list, &ptr->link);

	return ptr;
}


/*------------------------------------------------------------------------------
// append a new alarm cr to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_ALARM_CR_PTR_TYPE
cm_arcb_append_alarmcr (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,
		/* the alarm-cr */
	LSA_UINT32			alcr_properties,
	LSA_UINT16			timeout100ms,
	LSA_UINT16			retries
	)
{
	CM_UPPER_ARCB_ALARM_CR_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	/* note: argument checks not here */

	ptr->alcr_properties		= alcr_properties;
	ptr->timeout100ms			= timeout100ms;
	ptr->retries				= retries;

	arcb->alarm_cr = ptr;

	return ptr;
}


/*------------------------------------------------------------------------------
// append a SRInfoBlock to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_SR_INFO_PTR_TYPE
cm_arcb_append_sr_info (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,
		/* the sr_info */
	LSA_UINT16			RedundancyDataHoldFactor,
	LSA_UINT32			SRProperties
	)
{
	CM_UPPER_ARCB_SR_INFO_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));
	CM_ASSERT (cm_is_null (arcb->sr_info));

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	/* note: argument checks not here */

	ptr->rdh_factor		= RedundancyDataHoldFactor;
	ptr->sr_properties	= SRProperties;

	arcb->sr_info = ptr;

	return ptr;
}


/*------------------------------------------------------------------------------
// append a new io cr to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IOCR_PTR_TYPE
cm_arcb_append_iocr (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT16			iocr_type,			/* IO CR Type, see defines CM_IOCR_TYPE_RESERVED_0, ... */
	LSA_UINT16			iocr_ref,			/* for reference */
	LSA_UINT32			iocr_properties,	/* see defines CM_IOCR_PROP_RT_CLASS_MASK, ... */

	LSA_UINT16			data_length,		/* >= edd.min_frame_size && <= edd.max_frame_size */
	LSA_UINT16			frame_id,			/* frame_id, used by class3 */

	LSA_UINT16			send_clock,			/* send clock; must:1ms; range:125µs .. 4000µs, step:125µs */
	LSA_UINT16			reduction_ratio,	/* reduction ratio; must:1,2,4,8,16,32,128; optional:3,5,6,7,9,10,12,14,256,512 */
	LSA_UINT16			phase,				/* phase */
	LSA_UINT32			frame_send_offset,	/* */

	LSA_UINT16			data_hold_factor,	/* 3..16 */

	CM_MAC_TYPE			iocr_multicast_mac	/* multicast MAC address */
	)
{
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	/* note: argument checks not here */

		/* append a new element */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& iocr),
			arcb_null_user_id, sizeof (*iocr), sys_ptr
		);
	if (cm_is_null (iocr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*iocr));
		return LSA_NULL;
	}

	iocr->iocr_type			= iocr_type;
	iocr->iocr_ref			= iocr_ref;
	iocr->iocr_properties	= iocr_properties;

	iocr->data_length		= data_length;		/* 0 to max. 1472 (real user data length of the CR) */
	iocr->frame_id			= frame_id;

	iocr->send_clock		= send_clock;		/* Send Clock (phase = f(clock, ratio)) */
	iocr->reduction_ratio	= reduction_ratio;	/* Reduction ratio */
	iocr->phase				= phase;			/* Phase */
	iocr->frame_send_offset	= frame_send_offset;

	iocr->data_hold_factor	= data_hold_factor;

	iocr->iocr_multicast_mac= iocr_multicast_mac;

	CmListInitialize (& iocr->related_ioapi_list);

	CM_ASSERT (EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID == EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID);

	cm_arcb_iocr_set_edd (iocr
		, EDD_DATAOFFSET_INVALID, EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID
		, EDD_CRNUMBER_UNDEFINED, EDD_FORWARDMODE_UNDEFINED, 0
		);

	iocr->apdu_status_length= 0;
	iocr->partial_offset	= 0;
	iocr->partial_length	= 0;
	iocr->subframe_data		= 0;
	iocr->subframe_offset	= 0;

	/***/

	CmListInsertTail (& arcb->iocr_list, & iocr->link);

	return iocr;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE
cm_arcb_append_iocr_ir_info_sub (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,

	LSA_UINT16					iocr_reference,
	LSA_UINT16					subframe_offset,	/* for EDDx */
	LSA_UINT32					subframe_data		/* for EDDx */
	)
{
	CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE irInfoSubPtr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));
	CM_ASSERT (cm_is_not_null (arcb->ir_info));

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& irInfoSubPtr),
			arcb_null_user_id, sizeof (*irInfoSubPtr), sys_ptr
		);

	if (cm_is_null (irInfoSubPtr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*irInfoSubPtr));
		return LSA_NULL;
	}

	irInfoSubPtr->iocr_ref			= iocr_reference;
	irInfoSubPtr->subframe_offset	= subframe_offset;
	irInfoSubPtr->subframe_data		= subframe_data;

	CmListInsertTail (& arcb->ir_info->ir_info_sub_list, & irInfoSubPtr->link);

	return irInfoSubPtr;
}


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_arcb_iocr_set_edd (
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr,

	LSA_UINT32			edd_data_offset,	/* for EDDx */
	LSA_UINT32			edd_properties,		/* for EDDI */
	LSA_UINT16			edd_cr_number,		/* for EDDP and EDDS (see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254) */
	LSA_UINT8			edd_forward_mode,	/* for EDDP */
	LSA_UINT32			edd_ct_outdatastart	/* for EDDP */
	)
{
	CM_ASSERT (cm_is_not_null (iocr));

	iocr->edd_data_offset		= edd_data_offset;
	iocr->edd_properties		= edd_properties;
	iocr->edd_cr_number			= edd_cr_number;
	iocr->edd_forward_mode		= edd_forward_mode;
	iocr->edd_ct_outdatastart	= edd_ct_outdatastart;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IOCR_PTR_TYPE
cm_arcb_find_iocr (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE  arcb,
	LSA_UINT16  iocr_ref
	)
{
	CM_UPPER_ARCB_IOCR_PTR_TYPE  p;

	for (CmListForeach (p, & arcb->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
	{
		if (p->iocr_ref == iocr_ref)
		{
			return p;
		}
	}
	CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT, "iocr_ref(%d) not found", iocr_ref);
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/

static CM_UPPER_ARCB_IOAPI_PTR_TYPE
cm_arcb_iocr_find_ioapi (
	CM_UPPER_ARCB_IOCR_CONST_PTR_TYPE  iocr,
	LSA_UINT32  api
	)
{
	CM_UPPER_ARCB_IOAPI_PTR_TYPE  ap;

	for (CmListForeach (ap, & iocr->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
	{
		if (ap->api == api)
		{
			return ap;
		}
	}
	CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT, "api(%d) not found", api);
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/

static CM_UPPER_ARCB_IOAPI_PTR_TYPE
cm_arcb_iocr_insert_ioapi (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr,
	LSA_UINT32  api
	)
{
	CM_UPPER_ARCB_IOAPI_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (iocr));

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	CmListInitialize (& ptr->related_iodata_objects_list);
	CmListInitialize (& ptr->related_iocs_list);

	ptr->api = api;

	CmListInsertTail (& iocr->related_ioapi_list, & ptr->link); /* note: not sorted */

	return ptr;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE
cm_arcb_append_iodata_objects (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr,

	LSA_UINT32		api,
	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset
	)
{
	CM_UPPER_ARCB_IOAPI_PTR_TYPE  ioap;
	CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (iocr));

	/* note: argument checks not here */

		/* append a new api or use the existing one */
	ioap = cm_arcb_iocr_find_ioapi (iocr, api);
	if (cm_is_null (ioap))
	{
		ioap = cm_arcb_iocr_insert_ioapi (sys_ptr, iocr, api);
	}
	if (cm_is_null (ioap))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, api(%d)", api);
		return LSA_NULL;
	}

		/* append a new element */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	ptr->slot_nr		= slot_nr;
	ptr->subslot_nr		= subslot_nr;
	ptr->frame_offset	= frame_offset;

	CmListInsertTail (& ioap->related_iodata_objects_list, & ptr->link); /* note: not sorted */

	return ptr;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IOCS_PTR_TYPE
cm_arcb_append_iocs (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr,

	LSA_UINT32		api,
	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset
	)
{
	CM_UPPER_ARCB_IOAPI_PTR_TYPE  ioap;
	CM_UPPER_ARCB_IOCS_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (iocr));

	/* note: argument checks not here */

		/* append a new api or use the existing one */
	ioap = cm_arcb_iocr_find_ioapi (iocr, api);
	if (cm_is_null (ioap))
	{
		ioap = cm_arcb_iocr_insert_ioapi (sys_ptr, iocr, api);
	}
	if (cm_is_null (ioap))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, api(%d)", api);
		return LSA_NULL;
	}

		/* append a new element */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	ptr->slot_nr		= slot_nr;
	ptr->subslot_nr		= subslot_nr;
	ptr->frame_offset	= frame_offset;

	CmListInsertTail (& ioap->related_iocs_list, & ptr->link); /* note: not sorted */

	return ptr;
}

/*------------------------------------------------------------------------------
// append a new io cr local adaption to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE
cm_arcb_append_iocr_local (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT16			iocr_ref,

	LSA_UINT16			send_clock,
	LSA_UINT16			reduction_ratio,
	LSA_UINT16			phase,
	LSA_UINT16			data_hold_factor,	/* 0..use data_hold_factor from refered iocr, else..use as defined */
	LSA_UINT32			frame_send_offset
	)
{
	CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE  iocr_local;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	/* note: argument checks not here */

		/* append a new element */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& iocr_local),
			arcb_null_user_id, sizeof (*iocr_local), sys_ptr
		);
	if (cm_is_null (iocr_local))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*iocr_local));
		return LSA_NULL;
	}

	iocr_local->iocr_ref			= iocr_ref;

	iocr_local->send_clock			= send_clock;
	iocr_local->reduction_ratio		= reduction_ratio;
	iocr_local->phase				= phase;

	iocr_local->data_hold_factor	= data_hold_factor;

	iocr_local->frame_send_offset	= frame_send_offset;

	/***/

	CmListInsertTail (& arcb->iocr_local_list, & iocr_local->link);

	return iocr_local;
}


/*------------------------------------------------------------------------------
// append a new MCR to an existing arcb
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_MCR_PTR_TYPE
cm_arcb_append_mcr (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT16		iocr_ref,
	LSA_UINT32		mcr_properties,
	LSA_UINT16		mci_timeout_100ms,
	LSA_UINT16		provider_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE
					provider_name,

	LSA_BOOL make_copy_of_provider_name /* see cm_arcb_free(), flag "made_copy_of_provider_name" */
	)
{
	CM_UPPER_ARCB_MCR_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	/* note: argument checks not here */

	if( provider_name_length == 0 ) /* see copy */
	{
		CM_MEM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP, "!length");
		return LSA_NULL;
	}

		/* append a new element */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	if (make_copy_of_provider_name)
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr->provider_name),
				arcb_null_user_id, provider_name_length, sys_ptr
			);
		if (cm_is_null (ptr->provider_name))
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, ptr, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, provider_name_length(%d)", provider_name_length);
			return LSA_NULL;
		}

		CM_MEMCPY (ptr->provider_name, provider_name, provider_name_length);
	}
	else
	{
		ptr->provider_name = provider_name;
	}

	CmListInitialize (& ptr->link);

	ptr->iocr_ref = iocr_ref;
	ptr->mcr_properties = mcr_properties;
	ptr->mci_timeout_100ms = mci_timeout_100ms;
	ptr->provider_name_length = provider_name_length;

	CmListInsertTail (& arcb->mcr_list, &ptr->link); /* note: not sorted */

	return ptr;
}


/*------------------------------------------------------------------------------
// find AP, module, submodule
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_AP_PTR_TYPE
cm_arcb_find_ap (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_UINT32  api
	)
{
	CM_UPPER_ARCB_AP_PTR_TYPE  ap;

	for (CmListForeach(ap, & arcb->ap_list, CM_UPPER_ARCB_AP_PTR_TYPE))
	{
		if (ap->api == api)
		{
			return ap;
		}
	}
	CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT, "api(%d) not found", api);
	return LSA_NULL;
}

CM_UPPER_ARCB_MODULE_PTR_TYPE
cm_arcb_find_module (
	CM_UPPER_ARCB_AP_CONST_PTR_TYPE	ap,
	LSA_UINT16  slot_nr
	)
{
	CM_UPPER_ARCB_MODULE_PTR_TYPE  mod;

	for (CmListForeach(mod, & ap->module_list, CM_UPPER_ARCB_MODULE_PTR_TYPE))
	{
		if (mod->slot_nr == slot_nr)
		{
			return mod;
		}
	}
	CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT, "module with slot_nr(%d) not found", slot_nr);
	return LSA_NULL;
}

CM_UPPER_ARCB_SUBMODULE_PTR_TYPE
cm_arcb_find_submodule (
	CM_UPPER_ARCB_MODULE_CONST_PTR_TYPE	mod,
	LSA_UINT16  subslot_nr
	)
{
	CM_UPPER_ARCB_SUBMODULE_PTR_TYPE  sub;

	for (CmListForeach(sub, & mod->submodule_list, CM_UPPER_ARCB_SUBMODULE_PTR_TYPE))
	{
		if (sub->subslot_nr == subslot_nr)
		{
			return sub;
		}
	}
	CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT, "submodule with subslot_nr(0x%x) not found", subslot_nr);
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// append new api / module / submodule to an existing arcb
//----------------------------------------------------------------------------*/

static CM_UPPER_ARCB_AP_PTR_TYPE
cm_arcb_insert_ap (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,
	LSA_UINT32  api
	)
{
	CM_UPPER_ARCB_AP_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	CmListInitialize (& ptr->module_list);
	ptr->api = api;

	CmListInsertTail (& arcb->ap_list, & ptr->link); /* note: not sorted */

	return ptr;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_MODULE_PTR_TYPE
cm_arcb_append_module (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT32		api,			/* application process identifier */
	LSA_UINT16		slot_nr,		/* slot number */
	LSA_UINT16		mod_properties,	/* Module Properties */
	LSA_UINT32		mod_ident		/* Module_Ident_Number */
	)
{
	CM_UPPER_ARCB_AP_PTR_TYPE  ap;
	CM_UPPER_ARCB_MODULE_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (arcb));

	/* note: argument checks not here */

		/* append a new api or use the existing one */
	ap = cm_arcb_find_ap (arcb, api);
	if (cm_is_null (ap))
	{
		ap = cm_arcb_insert_ap (sys_ptr, arcb, api);
	}
	if (cm_is_null (ap))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, api(%d)", api);
		return LSA_NULL;
	}

		/* append a new module */
	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	CmListInitialize (& ptr->submodule_list);

	ptr->slot_nr		= slot_nr;
	ptr->mod_properties	= mod_properties;
	ptr->mod_ident		= mod_ident;

	CmListInsertTail (& ap->module_list, & ptr->link);

	return ptr;
}


/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_SUBMODULE_PTR_TYPE
cm_arcb_append_submodule (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_MODULE_PTR_TYPE	module,

	LSA_UINT16		subslot_nr,				/* Subslot_Number */
	LSA_UINT16		sub_properties,			/* Submodule Properties */
	LSA_UINT32		sub_ident,				/* Submodule_Ident_Number */
		/* for INPUT or NO_DATA (or INPUT_OUTPUT) */
	LSA_UINT16		in_data_description,	/* Data Description */
	LSA_UINT16		in_data_length,			/* Data Length, without IOPS */
	LSA_UINT8		in_iops_length,			/* Length IOPS 1..n */
	LSA_UINT8		in_iocs_length,			/* Length IOCS 1..m */
		/* for OUTPUT (or INPUT_OUTPUT) */
	LSA_UINT16		out_data_description,	/* Data Description */
	LSA_UINT16		out_data_length,		/* Data Length, without IOPS */
	LSA_UINT8		out_iops_length,		/* Length IOPS 1..n */
	LSA_UINT8		out_iocs_length			/* Length IOCS 1..m */
	)
{
	CM_UPPER_ARCB_SUBMODULE_PTR_TYPE ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (module));

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	ptr->subslot_nr			= subslot_nr;
	ptr->sub_properties		= sub_properties;
	ptr->sub_ident			= sub_ident;

	sub_properties &= CM_SUB_PROP_TYPE_MASK;

	switch (sub_properties) {
	case CM_SUB_PROP_TYPE_NO_DATA:
	case CM_SUB_PROP_TYPE_INPUT:
	case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
		ptr->descr[0].data_description	= in_data_description;
		ptr->descr[0].data_length		= in_data_length;
		ptr->descr[0].iops_length		= in_iops_length;
		ptr->descr[0].iocs_length		= in_iocs_length;
		break;

	default:
		ptr->descr[0].data_length		= 0xFFFF; /* invalid */
		break;
	}

	switch (sub_properties) {
	case CM_SUB_PROP_TYPE_OUTPUT:
	case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
		ptr->descr[1].data_description	= out_data_description;
		ptr->descr[1].data_length		= out_data_length;
		ptr->descr[1].iops_length		= out_iops_length;
		ptr->descr[1].iocs_length		= out_iocs_length;
		break;

	default:
		ptr->descr[1].data_length		= 0xFFFF;
		break;
	}

	CmListInitialize (& ptr->u.cl.prm_record_list);

	CmListInsertTail (& module->submodule_list, & ptr->link);

	return ptr;
}


/*------------------------------------------------------------------------------
// append a new record to an existing submodule
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_arcb_append_record (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_SUBMODULE_PTR_TYPE		submodule,

	LSA_UINT32					record_index,
	LSA_UINT16					transfer_sequence,
	LSA_UINT16					record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	record_data,

	LSA_BOOL make_copy_of_record_data /* see cm_arcb_free(), flag "made_copy_of_record_data" */
	)
{
	CM_UPPER_ARCB_RECORD_PTR_TYPE  ptr;
	LSA_UNUSED_ARG (sys_ptr);

	CM_ASSERT (cm_is_not_null (submodule));

	/* note: argument checks not here */

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr),
			arcb_null_user_id, sizeof (*ptr), sys_ptr
		);
	if (cm_is_null (ptr))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*ptr));
		return LSA_NULL;
	}

	if (make_copy_of_record_data)
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (& ptr->record_data),
				arcb_null_user_id, record_data_len, sys_ptr
			);
		if (cm_is_null (ptr->record_data))
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, ptr, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", record_data_len);
			return LSA_NULL;
		}

		CM_MEMCPY (ptr->record_data, record_data, record_data_len);
	}
	else
	{
		ptr->record_data = record_data;
	}

	ptr->record_index			= record_index;
	ptr->transfer_sequence		= transfer_sequence;
	ptr->record_data_len		= record_data_len;

	CmListInsertTail (& submodule->u.cl.prm_record_list, &ptr->link);

	return ptr;
}


/*------------------------------------------------------------------------------
// calculate the iocr-fields "partial_length, partial_offset, apdu_status_length"
//	according to the given submodules
//----------------------------------------------------------------------------*/

static LSA_BOOL
cm_arcb_calc_can_from_iocr_properties (
	LSA_UINT32  hardware_type,
	LSA_UINT32	iocr_properties,
	LSA_BOOL	*can_partial_data,
	LSA_BOOL	*can_autopadding,
	LSA_BOOL	*is_udp
	)
{
	LSA_UINT32  mask_p, supp_p, mask_a, supp_a;

	switch (iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)
	{
	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
		mask_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_MSK;
		supp_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT;

		mask_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_MSK;
		supp_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_SUPPORT;

		*is_udp = LSA_FALSE;
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		mask_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_MSK;
		supp_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_SUPPORT;

		mask_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_MSK;
		supp_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT;

		*is_udp = LSA_FALSE;
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		mask_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_MSK;
		supp_p = EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_SUPPORT;

		mask_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_MSK;
		supp_a = EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_SUPPORT;

		*is_udp = LSA_TRUE;
		break;

	default:
		/* no fatal here */
		return LSA_FALSE;
	}

	if( (hardware_type & mask_p) == supp_p )
	{
		*can_partial_data = LSA_TRUE;
	}
	else
	{
		*can_partial_data = LSA_FALSE;
	}

	if( (hardware_type & mask_a) == supp_a )
	{
		*can_autopadding = LSA_TRUE;
	}
	else
	{
		*can_autopadding = LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL		/* LSA_FALSE..some error in related list, don't use calculated fields */
cm_arcb_calc_iocr_buffer_size_i (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_BOOL  is_ioc,				/* LSA_TRUE: calc for CM-CL   LSA_FALSE: calc for CM-MC */
	LSA_UINT32  hardware_type		/* see CM_OPC_CL_CREATE::hardware_type and CM_OPC_MC_CREATE::hardware_type */
	)
{
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr;
	LSA_BOOL consumer_bug;

	if ((hardware_type & EDD_HW_TYPE_USED_MSK) == EDD_HW_TYPE_USED_ERTEC_200
	||	(hardware_type & EDD_HW_TYPE_USED_MSK) == EDD_HW_TYPE_USED_SOC)
	{
		consumer_bug = LSA_TRUE;
	}
	else
	{
		consumer_bug = LSA_FALSE;
	}

	for (CmListForeach (iocr, & arcb->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
	{
		CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE  ir_info_sub;
		LSA_UINT16  offset_adjust;
		CM_UPPER_ARCB_IOAPI_PTR_TYPE  ioap;
		LSA_UINT16  iocr_dir;

			/* no-partial, no-autopad, no-dfp, multicast */
		iocr->partial_length  = iocr->data_length;
		iocr->partial_offset  = 0;
		iocr->subframe_data   = 0;
		iocr->subframe_offset = 0;

		if( (hardware_type & EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE_MSK) == EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE )
		{
			iocr->apdu_status_length = 0;
		}
		else
		{
			iocr->apdu_status_length = 4;/*sizeof(APDUStatus)*/
		}

			/* search related ir_info_sub */
		if (cm_is_not_null (arcb->ir_info)  &&  ! CmListIsEmpty (& arcb->ir_info->ir_info_sub_list)) /* means is_dfp */
		{
			for (CmListForeach (ir_info_sub, & arcb->ir_info->ir_info_sub_list, CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE))
			{
				if (ir_info_sub->iocr_ref == iocr->iocr_ref)
				{
					break;
				}
			}
			if (cm_is_null (ir_info_sub))
			{
				return LSA_FALSE; /* related ir_info_sub not found */
			}
		}
		else
		{
			ir_info_sub = LSA_NULL;
		}

		switch( iocr->iocr_type ) /* note: cannot use is_consumer here */
		{
		case CM_IOCR_TYPE_INPUT:
			if (is_ioc)
			{
				iocr_dir = 0;
			}
			else
			{
				return LSA_FALSE; /* IOM has no input-cr */
			}
			break;

		case CM_IOCR_TYPE_OUTPUT:
			if (is_ioc)
			{
				iocr_dir = 1;
			}
			else
			{
				return LSA_FALSE; /* IOM has no output-cr */
			}
			break;

		case CM_IOCR_TYPE_MULTICAST_PROVIDER:
		case CM_IOCR_TYPE_MULTICAST_CONSUMER:
			if (is_ioc)
			{
				iocr->partial_length = 0; /* IOC doesn't use multicast-cr */
				iocr->apdu_status_length = 0;
				continue;
			}
			else
			{
				continue; /* IOM: onyl full frame allowed */
			}

		default:
			return LSA_FALSE;
		}

		CM_ASSERT (is_ioc); /* only IOC here */

		if (cm_is_null (ir_info_sub)) /* normal frame */
		{
			LSA_UINT16  used_from, used_to;

			used_from = iocr->data_length;
			used_to   = 0;

			for (CmListForeach (ioap, & iocr->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
			{
				CM_UPPER_ARCB_AP_PTR_TYPE  ap;
				CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE  iodata;
				CM_UPPER_ARCB_IOCS_PTR_TYPE  iocs;

				ap = cm_arcb_find_ap (arcb, ioap->api);
				if (cm_is_null (ap))
				{
					return LSA_FALSE; /* ap not found */
				}

				for (CmListForeach (iodata, & ioap->related_iodata_objects_list, CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE))
				{
					CM_UPPER_ARCB_MODULE_PTR_TYPE  mod;
					CM_UPPER_ARCB_SUBMODULE_PTR_TYPE  sub;
					LSA_UINT16  descr_dir, s_from, s_to;
					LSA_BOOL  reduce_iodata, reduce_ioxs;

					mod = cm_arcb_find_module (ap, iodata->slot_nr);
					if (cm_is_null (mod))
					{
						return LSA_FALSE; /* mod not found */
					}
					sub = cm_arcb_find_submodule (mod, iodata->subslot_nr);
					if (cm_is_null (sub))
					{
						return LSA_FALSE; /* submod not found */
					}

					switch (sub->sub_properties & CM_SUB_PROP_TYPE_MASK)
					{
					case CM_SUB_PROP_TYPE_NO_DATA:
					case CM_SUB_PROP_TYPE_INPUT:
						descr_dir = 0;
						break;

					case CM_SUB_PROP_TYPE_OUTPUT:
						descr_dir = 1;
						break;

					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						descr_dir = iocr_dir;
						break;

					default:
						return LSA_FALSE; /* unknown submod-properties */
					}

					s_from = iodata->frame_offset;
					s_to   = iodata->frame_offset;

					reduce_iodata = LSA_FALSE;
					if( descr_dir == 0 )
					{
						if( (sub->sub_properties & CM_SUB_PROP_REDUCE_INP_LEN_MASK) == CM_SUB_PROP_REDUCE_INP_LEN_YES )
						{
							reduce_iodata = LSA_TRUE;
						}
					}
					else
					{
						if( (sub->sub_properties & CM_SUB_PROP_REDUCE_OUT_LEN_MASK) == CM_SUB_PROP_REDUCE_OUT_LEN_YES )
						{
							reduce_iodata = LSA_TRUE;
						}
					}

					if (! reduce_iodata)
					{
						s_to += sub->descr[descr_dir].data_length;
					}

					reduce_ioxs = LSA_FALSE;
					if( (sub->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES )
					{
						reduce_ioxs = LSA_TRUE;
					}

					if( ! reduce_ioxs )
					{
						s_to += sub->descr[descr_dir].iops_length;
					}

					if (used_from > s_from)
					{
						used_from = s_from;
					}
					if (used_to < s_to)
					{
						used_to = s_to;
					}
				}

				for (CmListForeach (iocs, & ioap->related_iocs_list, CM_UPPER_ARCB_IOCS_PTR_TYPE))
				{
					CM_UPPER_ARCB_MODULE_PTR_TYPE  mod;
					CM_UPPER_ARCB_SUBMODULE_PTR_TYPE  sub;
					LSA_UINT16  descr_dir, s_from, s_to;
					LSA_BOOL  reduce_ioxs;

					mod = cm_arcb_find_module (ap, iocs->slot_nr);
					if (cm_is_null (mod))
					{
						return LSA_FALSE; /* mod not found */
					}
					sub = cm_arcb_find_submodule (mod, iocs->subslot_nr);
					if (cm_is_null (sub))
					{
						return LSA_FALSE; /* submod not found */
					}

					switch (sub->sub_properties & CM_SUB_PROP_TYPE_MASK)
					{
					case CM_SUB_PROP_TYPE_NO_DATA:
					case CM_SUB_PROP_TYPE_INPUT:
						descr_dir = 0;
						break;

					case CM_SUB_PROP_TYPE_OUTPUT:
						descr_dir = 1;
						break;

					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						if (iocr_dir == 0)
						{
							descr_dir = 1; /*!*/
						}
						else
						{
							descr_dir = 0; /*!*/
						}
						break;

					default:
						return LSA_FALSE; /* unknown submod-properties */
					}

					s_from = iocs->frame_offset;
					s_to   = iocs->frame_offset;

					reduce_ioxs = LSA_FALSE;
					if( (sub->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES )
					{
						reduce_ioxs = LSA_TRUE;
					}

					if( ! reduce_ioxs )
					{
						s_to += sub->descr[descr_dir].iocs_length;
					}

					if (used_from > s_from)
					{
						used_from = s_from;
					}
					if (used_to < s_to)
					{
						used_to = s_to;
					}
				}
			}

			if (used_from > used_to)
			{
				return LSA_FALSE; /* strange related offsets */
			}

			/***/

			{
			LSA_BOOL  can_partial_data, can_autopadding, is_udp;

			if (! cm_arcb_calc_can_from_iocr_properties (hardware_type, iocr->iocr_properties, &can_partial_data, &can_autopadding, &is_udp))
			{
				return LSA_FALSE;
			}

			switch (iocr->iocr_type)
			{
			case CM_IOCR_TYPE_INPUT:
				/* "partial data access" for consumers */
				if (can_partial_data)
				{
					if ((used_to - used_from) > 0)
					{
						iocr->partial_length = used_to - used_from;
						iocr->partial_offset = used_from;
					}
					else
					{
						iocr->partial_length = 1; /* "partial data access" and application doesn't read any data */
						iocr->partial_offset = used_from;
					}

					/* partial access not allowed, 1415794 */
					if (consumer_bug)
					{
						/* see EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_xxx */
						/* used_to == iocr->partial_offset + iocr->partial_length */
						if (iocr->data_length > 40
						||  (36 <= used_to && used_to <= 39))

						{
							iocr->partial_length  = iocr->data_length;
							iocr->partial_offset  = 0;
						}
					}
				}
				break;

			case CM_IOCR_TYPE_OUTPUT:
				/* "autopadding" for providers */
				if (can_autopadding)
				{
					if ((  is_udp  &&  iocr->data_length == CM_PNIO_C_SDU_MIN_LENGTH_UDP)
					||	(! is_udp  &&  iocr->data_length == CM_PNIO_C_SDU_MIN_LENGTH_RT ))
					{
						if (used_to > 0)
						{
							iocr->partial_length = used_to;
						}
						else
						{
							iocr->partial_length = 1; /* "autopadding" and application doesn't write any data */
						}
						iocr->partial_offset = 0;
					}
				}
				break;

			case CM_IOCR_TYPE_MULTICAST_CONSUMER:
			case CM_IOCR_TYPE_MULTICAST_PROVIDER:
			default:
				CM_FATAL1(iocr->iocr_type); /* see continue */
				return LSA_FALSE;
			}
			}

			offset_adjust = iocr->partial_offset;
		}
		else /* DFP */
		{
			LSA_UINT16 subframe_length = (ir_info_sub->subframe_data >> 8) & 0xFF;
			LSA_UINT32 edd_hw_type_used = (hardware_type & EDD_HW_TYPE_USED_MSK);

			{
			LSA_UINT16  data_length = iocr->data_length - (2+2);	/* see checking rule */

			if (ir_info_sub->subframe_offset < 6  ||  ir_info_sub->subframe_offset >= data_length)
			{
				return LSA_FALSE;
			}
			if (subframe_length == 0 || ir_info_sub->subframe_offset + subframe_length  > data_length)
			{
				return LSA_FALSE;
			}
			}

			iocr->partial_offset = 0;

			if (edd_hw_type_used == EDD_HW_TYPE_USED_SOC) /* packframe */
			{
				iocr->partial_length = iocr->data_length; /* i.e. CSF_SDU + RTCPadding */

				offset_adjust = 0;
			}
			else if (edd_hw_type_used == EDD_HW_TYPE_USED_ERTEC_200P) /* subframe */
			{
				iocr->partial_length = subframe_length; /* size of Subframe::C_SDU */

				offset_adjust = ir_info_sub->subframe_offset;
			}
			else if (edd_hw_type_used == EDD_HW_TYPE_USED_HERA)
			{
				if (iocr->iocr_type == CM_IOCR_TYPE_INPUT)
				{
					/// HERA DFP-CONSUMER (subframe)
					/// SeqID(2) + Subframe::C_SDU(SFLen) + SFPosLenCcDs(4) + SeqID(2)
					/// HERA-ALLOC --> adds SeqID at the front and SFPosLenCcDs+SeqID at the end
					/// CM ----------> adds nothing to "alloc size"
					/// HERA-COPY ---> copies Subframe::C_SDU, no HERA-specific offset-adjustments necessary

					iocr->partial_length = subframe_length; /* size of Subframe::C_SDU */

					offset_adjust = ir_info_sub->subframe_offset;
				}
				else
				{
					/// HERA DFP-PROVIDER (packframe without header-SFCRC16)
					/// SeqID(2) + DS(1) + <<SUBFRAME* + TERM(2) + [RTCPadding]>> + SeqID(2)
					/// HERA-ALLOC --> adds SeqID+DS at the front and SeqID at the end
					/// CM ----------> adds nothing to "alloc size"
					/// HERA-COPY ---> copies <<...>> which has size (iocr->data_length - size of header-SFCRC16)
					/// CM must adjust offsets by 2 (header-SFCRC16)

					iocr->partial_length = iocr->data_length; /* i.e. CSF_SDU + RTCPadding (including header-SFCRC! see EDD interface) */

					offset_adjust = 2; /* adjust offsets by size of header-SFCRC16 */
					/// user-buffer is of size "HERA-COPY copies ..." --> iocr->partial_length - 2
				}
			}
			else
			{
				return LSA_FALSE; /* hardware doesn't support DFPs */
			}

			iocr->subframe_data   = ir_info_sub->subframe_data & 0xFF7F;
			iocr->subframe_offset = ir_info_sub->subframe_offset;
		}

			/* calculate the application offset for each io data object */
		for (CmListForeach (ioap, & iocr->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
		{
			CM_UPPER_ARCB_AP_PTR_TYPE  ap;
			CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE  iodata;
			CM_UPPER_ARCB_IOCS_PTR_TYPE  iocs;

			ap = cm_arcb_find_ap (arcb, ioap->api);
			if (cm_is_null (ap))
			{
				return LSA_FALSE; /* ap not found */
			}

			for (CmListForeach (iodata, & ioap->related_iodata_objects_list, CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE))
			{
				if (! (iodata->frame_offset >= offset_adjust))
				{
					return LSA_FALSE; /* config error */
				}
				iodata->application_offset = iodata->frame_offset - offset_adjust;
			}

			for (CmListForeach (iocs, & ioap->related_iocs_list, CM_UPPER_ARCB_IOCS_PTR_TYPE))
			{
				if (! (iocs->frame_offset >= offset_adjust))
				{
					return LSA_FALSE; /* config error */
				}
				iocs->application_offset = iocs->frame_offset - offset_adjust;
			}
		}
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL		/* LSA_FALSE..some error in related list, don't use calculated fields */
cm_arcb_calc_iocr_buffer_size (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_BOOL  is_ioc,				/* LSA_TRUE: calc for CM-CL   LSA_FALSE: calc for CM-MC */
	LSA_UINT32  hardware_type		/* see CM_OPC_CL_CREATE::hardware_type */
)
{
	/* request 1502507 (pc-lint) */
	LSA_BOOL ok = cm_arcb_calc_iocr_buffer_size_i (arcb, is_ioc, hardware_type);

	if (!ok)
	{
		CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr;

		for (CmListForeach (iocr, & arcb->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
		{
			iocr->apdu_status_length	= 0;
			iocr->partial_length		= 0;
			iocr->partial_offset		= 0;
			iocr->subframe_data			= 0;
			iocr->subframe_offset		= 0;
		}
	}
	return ok;
}
/*------------------------------------------------------------------------------
// free the arcb
//----------------------------------------------------------------------------*/

static LSA_VOID
cm_arcb_free_list(
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_LIST_ENTRY_CONST_PTR_TYPE  head
	)
{
	LSA_UNUSED_ARG (sys_ptr);

	while( ! CmListIsEmpty(head) )
	{
		CM_LIST_ENTRY_PTR_TYPE tmp;
		LSA_UINT16 rc;
		CmListRemoveHead(head, tmp, CM_LIST_ENTRY_PTR_TYPE);

		CM_FREE_UPPER_MEM (& rc, tmp, sys_ptr);
		CM_ASSERT(rc == LSA_RET_OK);
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_arcb_free (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,
	LSA_BOOL  made_copy_of_record_data, /* see cm_arcb_append_record(), flag "make_copy_of_record_data" */
	LSA_BOOL  made_copy_of_provider_name, /* see cm_arcb_append_mcr(), flag "make_copy_of_provider_name" */
	LSA_BOOL  made_copy_of_arvendor_data, /* see cm_arcb_append_arvendor(), flag "make_copy_of_arvendor_data" */
	LSA_BOOL  made_copy_of_submodule_properties_data, /* see cm_arcb_append_submodule_properties(), flag "make_copy_of_submodule_properties_data" */
	LSA_BOOL  made_copy_of_rs_info_data  /* see cm_arcb_append_rs_info(), flag "make_copy_of_rs_info_data" */
	)
{
	/*** traverse the graph an free everything ***/

	if (cm_is_not_null (arcb->prm))
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (& rc, arcb->prm, sys_ptr);
		CM_ASSERT (rc == LSA_RET_OK);
		arcb->prm = LSA_NULL;
	}

	{
		CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr;

		for (CmListForeach (iocr, &arcb->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
		{
			CM_UPPER_ARCB_IOAPI_PTR_TYPE  ioap;

			for (CmListForeach (ioap, & iocr->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
			{
				cm_arcb_free_list (sys_ptr, & ioap->related_iodata_objects_list);
				cm_arcb_free_list (sys_ptr, & ioap->related_iocs_list);
			}
			cm_arcb_free_list (sys_ptr, & iocr->related_ioapi_list);
		}
	}

	cm_arcb_free_list(sys_ptr, & arcb->iocr_list);

	cm_arcb_free_list(sys_ptr, & arcb->iocr_local_list);

	if (cm_is_not_null (arcb->alarm_cr))
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (& rc, arcb->alarm_cr, sys_ptr);
		CM_ASSERT (rc == LSA_RET_OK);
		arcb->alarm_cr = LSA_NULL;
	}

	if (cm_is_not_null (arcb->ir_info))
	{
		LSA_UINT16  rc;
		cm_arcb_free_list(sys_ptr, & arcb->ir_info->ir_info_sub_list);
		CM_FREE_UPPER_MEM (& rc, arcb->ir_info, sys_ptr);
		CM_ASSERT (rc == LSA_RET_OK);
		arcb->ir_info = LSA_NULL;
	}

	if (cm_is_not_null (arcb->sr_info))
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (& rc, arcb->sr_info, sys_ptr);
		CM_ASSERT (rc == LSA_RET_OK);
		arcb->sr_info = LSA_NULL;
	}

	if (cm_is_not_null (arcb->arvendor_blob.arvendor_data))
	{
		if (made_copy_of_arvendor_data)
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, arcb->arvendor_blob.arvendor_data, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		arcb->arvendor_blob.arvendor_data_len = 0;
		arcb->arvendor_blob.arvendor_data     = LSA_NULL;
	}

	if (cm_is_not_null (arcb->submodule_properties_blob.submodule_properties_data))
	{
		if (made_copy_of_submodule_properties_data)
		{
			LSA_UINT16  rc;
			CM_FREE_UPPER_MEM (& rc, arcb->submodule_properties_blob.submodule_properties_data, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		arcb->submodule_properties_blob.submodule_properties_data_len = 0;
		arcb->submodule_properties_blob.submodule_properties_data     = LSA_NULL;
	}

	if (cm_is_not_null (arcb->rs_info.rs_info_data))
	{
		LSA_UINT16  rc;

		if (made_copy_of_rs_info_data)
		{
			CM_FREE_UPPER_MEM (& rc, arcb->rs_info.rs_info_data, sys_ptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		arcb->rs_info.rs_info_data_len = 0;
		arcb->rs_info.rs_info_data     = LSA_NULL;
	}

	{	/* for each AP */
		CM_UPPER_ARCB_AP_PTR_TYPE  ap;

		for (CmListForeach (ap, &arcb->ap_list, CM_UPPER_ARCB_AP_PTR_TYPE))
		{
			/* for each MODULE */
			CM_UPPER_ARCB_MODULE_PTR_TYPE  mod;

			for (CmListForeach (mod, &ap->module_list, CM_UPPER_ARCB_MODULE_PTR_TYPE))
			{
				/* for each SUBMODULE */
				CM_UPPER_ARCB_SUBMODULE_PTR_TYPE  sub;

				for (CmListForeach (sub, &mod->submodule_list, CM_UPPER_ARCB_SUBMODULE_PTR_TYPE))
				{
					/* for each RECORD */
					CM_UPPER_ARCB_RECORD_PTR_TYPE  rec;

					if (made_copy_of_record_data)
					{
						for (CmListForeach (rec, &sub->u.cl.prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
						{
							if (cm_is_not_null (rec->record_data))
							{
								LSA_UINT16  rc;
								CM_FREE_UPPER_MEM (& rc, rec->record_data, sys_ptr);
								CM_ASSERT (rc == LSA_RET_OK);
								rec->record_data = LSA_NULL;
							}
						}
					}

					cm_arcb_free_list (sys_ptr, & sub->u.cl.prm_record_list);
				}

				cm_arcb_free_list (sys_ptr, & mod->submodule_list);
			}

			cm_arcb_free_list (sys_ptr, & ap->module_list);
		}

		cm_arcb_free_list (sys_ptr, & arcb->ap_list);
	}

	if (made_copy_of_provider_name)
	{
		CM_UPPER_ARCB_MCR_PTR_TYPE mcr;

		for (CmListForeach (mcr, &arcb->mcr_list, CM_UPPER_ARCB_MCR_PTR_TYPE))
		{
			if (cm_is_not_null (mcr->provider_name))
			{
				LSA_UINT16 rc;
				CM_FREE_UPPER_MEM (& rc, mcr->provider_name, sys_ptr);
				CM_ASSERT (rc == LSA_RET_OK);
				mcr->provider_name = LSA_NULL;
			}
		}
	}

	cm_arcb_free_list (sys_ptr, & arcb->mcr_list);

	if (made_copy_of_record_data)
	{
		CM_UPPER_ARCB_RECORD_PTR_TYPE  rec;

		for (CmListForeach (rec, &arcb->ar_prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
		{
			if (cm_is_not_null (rec->record_data))
			{
				LSA_UINT16  rc;
				CM_FREE_UPPER_MEM (& rc, rec->record_data, sys_ptr);
				CM_ASSERT (rc == LSA_RET_OK);
				rec->record_data = LSA_NULL;
			}
		}
	}
	cm_arcb_free_list (sys_ptr, &arcb->ar_prm_record_list);

	{
		LSA_UINT16 rc;
		CM_FREE_UPPER_MEM (& rc, arcb, sys_ptr);
		CM_ASSERT (rc == LSA_RET_OK);
	}
}


/*------------------------------------------------------------------------------
// related to CM_OPC_CL_PRM_UPDATE
//----------------------------------------------------------------------------*/

CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE
cm_prm_update_append_or_find_submod (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
	)
{
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;

	for (CmListForeach(prm_submod, &prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		if (prm_submod->api == api  &&  prm_submod->slot_nr == slot_nr  &&  prm_submod->subslot_nr == subslot_nr)
		{
			return prm_submod;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR(&prm_submod), arcb_null_user_id, sizeof (*prm_submod), sys_ptr);

	if (!prm_submod)
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*prm_submod));
		return LSA_NULL;
	}

	prm_submod->api = api;
	prm_submod->slot_nr = slot_nr;
	prm_submod->subslot_nr = subslot_nr;

	CmListInitialize (&prm_submod->prm_record_list);

	CmListInsertTail (&prm_update->prm_submod_list, &prm_submod->link);

	return prm_submod;
}

/*----------------------------------------------------------------------------*/

CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_prm_update_append_record (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod,
	LSA_UINT32  record_index,
	LSA_UINT16  transfer_sequence,
	LSA_UINT16  record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE  record_data,
	LSA_BOOL  make_copy_of_record_data /* see cm_prm_update_free(), flag "made_copy_of_record_data" */
	)
{
	CM_UPPER_ARCB_RECORD_PTR_TYPE  prm_record;

	if (record_data_len == 0)
	{
		CM_MEM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP, "record_data_len(0) == 0");
		return LSA_NULL;
	}

	CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (&prm_record), arcb_null_user_id, sizeof (*prm_record), sys_ptr);

	if (cm_is_null (prm_record))
	{
		CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", sizeof(*prm_record));
		return LSA_NULL;
	}

	prm_record->record_index = record_index;
	prm_record->transfer_sequence = transfer_sequence;
	prm_record->record_data_len = record_data_len;

	if (make_copy_of_record_data)
	{
		CM_ALLOC_UPPER_MEM (CM_ARCB_CAST_UPPER_MEM_PTR_PTR (&prm_record->record_data), arcb_null_user_id, record_data_len, sys_ptr);

		if (cm_is_null (prm_record->record_data))
		{
			LSA_UINT16  rc;

			CM_FREE_UPPER_MEM (& rc, prm_record, sys_ptr);
			CM_ASSERT (rc == LSA_OK);

			CM_MEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "!mem, size(%d)", record_data_len);
			return LSA_NULL;
		}

		CM_MEMCPY (prm_record->record_data, record_data, record_data_len);
	}
	else
	{
		prm_record->record_data = record_data;
	}

	CmListInsertTail (&prm_submod->prm_record_list, &prm_record->link);

	return prm_record;
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_prm_update_free (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update,
	LSA_BOOL  made_copy_of_record_data
	)
{
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod;

	for (CmListForeach (prm_submod, &prm_update->prm_submod_list, CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE))
	{
		if (made_copy_of_record_data)
		{
			CM_UPPER_ARCB_RECORD_PTR_TYPE  prm_record;

			for (CmListForeach (prm_record, &prm_submod->prm_record_list, CM_UPPER_ARCB_RECORD_PTR_TYPE))
			{
				if (cm_is_not_null(prm_record->record_data))
				{
					LSA_UINT16  rc;
					CM_FREE_UPPER_MEM (& rc, prm_record->record_data, sys_ptr);
					CM_ASSERT (rc == LSA_RET_OK);
				}
			}
		}

		cm_arcb_free_list (sys_ptr, &prm_submod->prm_record_list);
	}

	cm_arcb_free_list (sys_ptr, &prm_update->prm_submod_list);
}


/*---------------------------------------------------------------------------*/
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
