#ifndef CM_ARCB_HELPER_H
#define CM_ARCB_HELPER_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: cm_arcb_helper.h                          :F&  */
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

/*
 * note: cm_usr.h (via cm_inc.h) must be included first explicitly (the helper functions are an add-on)
 */


/**************************************************************************//**
@defgroup cm-arcb-helper-functions ARCB Helper-Functions
@ingroup CM_ARCB_TYPE
@brief CM includes helper-functions that assist in building the ARCB.
******************************************************************************/

/**************************************************************************//**
@defgroup cm-arcb-helper-functions-dev-addr CM_CL_DEVICE_ADD_TYPE::dev_addr
@ingroup cm-arcb-helper-functions
@brief Helper for CM_CL_DEVICE_ADD_TYPE::dev_addr
******************************************************************************/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-dev-addr
******************************************************************************/
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

	LSA_BOOL        make_copy_of_device_name
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-dev-addr
******************************************************************************/
CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE
cm_arcb_append_nare_alias (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_NARE_PTR_TYPE  nare,

	LSA_UINT16			alias_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE
						alias_name,
	LSA_BOOL make_copy_of_nare_alias
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-dev-addr
******************************************************************************/
LSA_VOID
cm_arcb_free_nare (
	LSA_SYS_PTR_TYPE sys_ptr,
	CM_UPPER_ARCB_NARE_PTR_TYPE nare,
	LSA_BOOL made_copy_of_device_name,
	LSA_BOOL made_copy_of_alias_name
);


/**************************************************************************//**
@defgroup cm-arcb-helper-functions-dev-rpc CM_CL_DEVICE_ADD_TYPE::dev_rpc
@ingroup cm-arcb-helper-functions
@brief Helper for CM_CL_DEVICE_ADD_TYPE::dev_rpc
******************************************************************************/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-dev-rpc
******************************************************************************/
LSA_VOID
cm_arcb_set_rpc (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_RPC_PTR_TYPE  rpc,
	LSA_UINT16		vendor_id,
	LSA_UINT16		device_id,
	LSA_UINT16		instance,
	LSA_UINT32		max_supported_record_length
);



/**************************************************************************//**
@defgroup cm-arcb-helper-functions-arcb CM_CL_DEVICE_ADD_TYPE::arcb
@ingroup cm-arcb-helper-functions
@brief Helper for CM_CL_DEVICE_ADD_TYPE::arcb
******************************************************************************/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_ADD_PTR_TYPE
cm_arcb_new_io_ar (
	LSA_SYS_PTR_TYPE	sys_ptr,

	LSA_UINT16			device_nr,
	LSA_UINT16			ar_nr,
	LSA_UINT16			ar_type,
	CLRPC_UUID_TYPE		ar_uuid,
	LSA_UINT32			ar_properties,
	CLRPC_UUID_CONST_PTR_TYPE	ir_data_uuid, /* LSA_NULL or IRDataUUID */
	LSA_UINT8			sr_ar_number  /* see 61158-6-10 ARUUID.Selector  //  if ar_type == CM_AR_TYPE_SINGLE_SYSRED: 1 .. 4;  else: 0 */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_arcb_append_prm_record (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT32					record_index, /* AR-specific */
	LSA_UINT16					record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	record_data,

	LSA_BOOL make_copy_of_record_data /* see cm_arcb_free(), flag "made_copy_of_record_data" */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_ALARM_CR_PTR_TYPE
cm_arcb_append_alarmcr (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT32			alcr_properties,
	LSA_UINT16			timeout100ms,
	LSA_UINT16			retries
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for CM_AR_TYPE_SINGLE_SYSRED
******************************************************************************/
CM_UPPER_ARCB_SR_INFO_PTR_TYPE
cm_arcb_append_sr_info (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT16			RedundancyDataHoldFactor,
	LSA_UINT32			SRProperties
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for ARVendorBlockReq
******************************************************************************/
CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_arvendor (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					arvendor_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	arvendor_data,

	LSA_BOOL make_copy_of_arvendor_data /* see cm_arcb_free(), flag "made_copy_of_arvendor_data" */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for SubmoduleProperties
******************************************************************************/
CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_submodule_properties (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					submodule_properties_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	submodule_properties_data,

	LSA_BOOL make_copy_of_submodule_properties_data /* see cm_arcb_free(), flag "made_copy_of_submodule_properties_data" */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for RSInfoBlock
******************************************************************************/
CM_UPPER_MEM_U8_PTR_TYPE
cm_arcb_append_rs_info (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT16					rs_info_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	rs_info_data,

	LSA_BOOL make_copy_of_rs_info_data /* see cm_arcb_free(), flag "made_copy_of_rs_info_data" */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_IOCR_PTR_TYPE
cm_arcb_append_iocr (
	LSA_SYS_PTR_TYPE	sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,

	LSA_UINT16			iocr_type,			/* IO CR Type, see defines CM_IOCR_TYPE_RESERVED_0, ... */
	LSA_UINT16			iocr_ref,			/* for reference */
	LSA_UINT32			iocr_properties,	/* see defines @ref CM_IOCR_PROP_RT_CLASS_MASK, ... */

	LSA_UINT16			data_length,		/* >= edd.min_frame_size && <= edd.max_frame_size */
	LSA_UINT16			frame_id,			/* frame_id, used by class3 */

	LSA_UINT16			send_clock,			/* send clock; must:1ms; range:125µs .. 4000µs, step:125µs */
	LSA_UINT16			reduction_ratio,	/* reduction ratio; must:1,2,4,8,16,32,128; optional:3,5,6,7,9,10,12,14,256,512 */
	LSA_UINT16			phase,				/* phase */
	LSA_UINT32			frame_send_offset,	/* */

	LSA_UINT16			data_hold_factor,	/* 3..16 */

	CM_MAC_TYPE			iocr_multicast_mac	/* multicast MAC address */
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE
cm_arcb_append_iodata_objects (
	LSA_SYS_PTR_TYPE		sys_ptr,
	CM_UPPER_ARCB_IOCR_PTR_TYPE	iocr,

	LSA_UINT32		api,
	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_IOCS_PTR_TYPE
cm_arcb_append_iocs (
	LSA_SYS_PTR_TYPE		sys_ptr,
	CM_UPPER_ARCB_IOCR_PTR_TYPE	iocr,

	LSA_UINT32		api,
	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for CM_AR_TYPE_SINGLE_RTC3 with dynamic frame packing (DFP)
******************************************************************************/
CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE
cm_arcb_append_iocr_ir_info_sub (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,

	LSA_UINT16					iocr_reference,
	LSA_UINT16					subframe_offset,	/* for EDDx */
	LSA_UINT32					subframe_data		/* for EDDx */
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
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
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for local send-clock and IO buffer settings.
******************************************************************************/
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
);


/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for local send-clock and IO buffer settings.
******************************************************************************/
LSA_VOID
cm_arcb_iocr_set_edd (
	CM_UPPER_ARCB_IOCR_PTR_TYPE  iocr,

	LSA_UINT32			edd_data_offset,	/* for EDDx */
	LSA_UINT32			edd_properties,		/* for EDDI */
	LSA_UINT16			edd_cr_number,		/* for EDDP and EDDS (see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254) */
	LSA_UINT8			edd_forward_mode,	/* for EDDP */
	LSA_UINT32			edd_ct_outdatastart	/* for EDDP */
);

/*----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_MODULE_PTR_TYPE
cm_arcb_append_module (
	LSA_SYS_PTR_TYPE		sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE	arcb,

	LSA_UINT32		api,			/* application process identifier */
	LSA_UINT16		slot_nr,		/* slot number */
	LSA_UINT16		mod_properties,	/* module properties */
	LSA_UINT32		mod_ident		/* module ident number */
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_SUBMODULE_PTR_TYPE
cm_arcb_append_submodule (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_MODULE_PTR_TYPE	module,

	LSA_UINT16		subslot_nr,				/* subslot number */
	LSA_UINT16		sub_properties,			/* submodule properties */
	LSA_UINT32		sub_ident,				/* submodule ident number */
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
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
******************************************************************************/
CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_arcb_append_record (
	LSA_SYS_PTR_TYPE			sys_ptr,
	CM_UPPER_ARCB_SUBMODULE_PTR_TYPE		submod,

	LSA_UINT32					record_index,
	LSA_UINT16					transfer_sequence,
	LSA_UINT16					record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE	record_data,

	LSA_BOOL make_copy_of_record_data /* see cm_arcb_free(), flag "made_copy_of_record_data" */
);


/*------------------------------------------------------------------------------
// calculate the iocr-fields "partial_length, partial_offset, apdu_status_length"
//	according to the given submodules
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions
@brief Helper for calculating the IO buffer size and offset
******************************************************************************/
LSA_BOOL		/* LSA_FALSE..some error in related list, don't use calculated fields */
cm_arcb_calc_iocr_buffer_size (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_BOOL  is_ioc,				/* LSA_TRUE: calc for CM-CL   LSA_FALSE: calc for CM-MC */
	LSA_UINT32  hardware_type		/* see CM_OPC_CL_CREATE::hardware_type */
);


/*------------------------------------------------------------------------------
// free the built ARCB
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup cm-arcb-helper-functions
@brief Free the built ARCB
******************************************************************************/
LSA_VOID
cm_arcb_free (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_ARCB_ADD_PTR_TYPE  arcb,
	LSA_BOOL  made_copy_of_record_data, /* see cm_arcb_append_record(), flag "make_copy_of_record_data" */
	LSA_BOOL  made_copy_of_provider_name, /* see cm_arcb_append_mcr(), flag "make_copy_of_provider_name" */
	LSA_BOOL  made_copy_of_arvendor_data, /* see cm_arcb_append_arvendor(), flag "make_copy_of_arvendor_data" */
	LSA_BOOL  made_copy_of_submodule_properties_data, /* see cm_arcb_append_submodule_properties(), flag "make_copy_of_submodule_properties_data" */
	LSA_BOOL  made_copy_of_rs_info_data  /* see cm_arcb_append_rs_info(), flag "make_copy_of_rs_info_data" */
);


#if CM_CFG_MAX_CLIENTS
/*****************************************************************************/
/* related to CM_OPC_CL_PRM_UPDATE                                           */
/**************************************************************************//**
@ingroup cm-arcb-helper-functions
@brief This function is used for adding or finding the prm_submod in the submodule list of the  PRM_UPDATE rqb.
@details
Subsequent calls to cm_prm_update_append_record() can be made to fill the record list, or, by not providing any records,
creating an empty parameterization for a submodule.

@pre
Prior to using this function, prm_update->prm_submod_list shall be initialized using a single invocation of
@code{.c}
	CmListInitialize (&prm_update->prm_submod_list);
@endcode
@attention A subsequent call to cm_prm_update_free() will release all memory allocated by all previous calls to cm_prm_update_append_or_find_submod().
******************************************************************************/
CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE
cm_prm_update_append_or_find_submod (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for CM_AR_TYPE_SINGLE_SYSRED
@details
This function can be used for adding records for prm_update.
@attention A subsequent call to cm_prm_update_free() will release all memory allocated by all previous calls to cm_prm_update_append_or_find_submod().
******************************************************************************/
CM_UPPER_ARCB_RECORD_PTR_TYPE
cm_prm_update_append_record (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE  prm_submod,
	LSA_UINT32  record_index,
	LSA_UINT16  transfer_sequence,
	LSA_UINT16  record_data_len,
	CM_UPPER_MEM_U8_PTR_TYPE  record_data,
	LSA_BOOL  make_copy_of_record_data /* see cm_prm_update_free(), flag "made_copy_of_record_data" */
);

/**************************************************************************//**
@ingroup cm-arcb-helper-functions-arcb
@brief Helper for CM_AR_TYPE_SINGLE_SYSRED
@details
Release all memory allocated with calls to cm_prm_update_append_or_find_submod() and cm_prm_update_append_record() in the PRM_UPDATE rqb.
******************************************************************************/
LSA_VOID
cm_prm_update_free (
	LSA_SYS_PTR_TYPE  sys_ptr,
	CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE  prm_update,
	LSA_BOOL  made_copy_of_record_data
);

#endif


/*------------------------------------------------------------------------------
// find IOCR, AP, module, submodule
//----------------------------------------------------------------------------*/

CM_UPPER_ARCB_IOCR_PTR_TYPE
cm_arcb_find_iocr (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_UINT16  iocr_ref
);

CM_UPPER_ARCB_AP_PTR_TYPE
cm_arcb_find_ap (
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE	arcb,
	LSA_UINT32  api
);

CM_UPPER_ARCB_MODULE_PTR_TYPE
cm_arcb_find_module (
	CM_UPPER_ARCB_AP_CONST_PTR_TYPE	ap,
	LSA_UINT16  slot_nr
);

CM_UPPER_ARCB_SUBMODULE_PTR_TYPE
cm_arcb_find_submodule (
	CM_UPPER_ARCB_MODULE_CONST_PTR_TYPE	mod,
	LSA_UINT16  subslot_nr
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ARCB_HELPER_H */
