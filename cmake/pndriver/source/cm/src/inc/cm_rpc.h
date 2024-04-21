#ifndef CM_RPC_H
#define CM_RPC_H

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
/*  F i l e               &F: cm_rpc.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile: RPC things                                          */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by cm_int.h
 */

/*------------------------------------------------------------------------------
//	UUID utility functions
//----------------------------------------------------------------------------*/
extern const CLRPC_UUID_TYPE  cm_uuid_nil_uuid;

LSA_BOOL
cm_uuid_equal (
	const CLRPC_UUID_TYPE * const uuid1,
	const CLRPC_UUID_TYPE * const uuid2
);

LSA_BOOL
cm_uuid_is_nil (
	const CLRPC_UUID_TYPE * const uuid
);


/*------------------------------------------------------------------------------
//	PNIO interface
//----------------------------------------------------------------------------*/

/*
 * note: Opnums see CLRPC_PNIO_OPNUM_CONNECT, etc.
 */

LSA_VOID
cm_rpc_get_IPNIO_Controller(
	CLRPC_UUID_TYPE *uuid,
	LSA_UINT16      *version_major,
	LSA_UINT16      *version_minor,
	LSA_UINT16      *opnum_max,     /* may be LSA_NULL */
	LSA_UINT16      *pnio_signature /* may be LSA_NULL */
);

LSA_VOID
cm_rpc_get_IPNIO_Device(
	CLRPC_UUID_TYPE *uuid,
	LSA_UINT16      *version_major,
	LSA_UINT16      *version_minor,
	LSA_UINT16      *opnum_max,     /* may be LSA_NULL */
	LSA_UINT16      *pnio_signature /* may be LSA_NULL */
);

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

LSA_VOID
cm_rpc_make_object_uuid(
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_UINT16 vendor_id,
	LSA_UINT16 device_id,
	LSA_UINT16 instance
);

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_rpc_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

CM_RPC_LOWER_RQB_PTR_TYPE
cm_rpc_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_rpc_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

CM_RPC_LOWER_RQB_PTR_TYPE
cm_rpc_alloc_call_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  len
);

LSA_VOID
cm_rpc_free_call_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

LSA_VOID
cm_rpc_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_rpc_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);


LSA_VOID
cm_rpc_request_lower (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_OPCODE_TYPE  opcode,
	LSA_UINT16  dev_nr,
	LSA_UINT16  ar_nr
);

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

LSA_VOID
cm_rpc_set_interface_param (
	CM_CHANNEL_PTR_TYPE  channel
);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_RPC_H */
