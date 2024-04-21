#ifndef CLRPC_USR_H
#define CLRPC_USR_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_usr.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*  Detailed documentation in ...\doc\LSA_CLRPC_LSA-Interface.doc       */
/*                                                                           */
/*****************************************************************************/

/*-----------------------------------------------------------------------------
// common types
//---------------------------------------------------------------------------*/

typedef LSA_UINT8 * CLRPC_UPPER_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 * CLRPC_COMMON_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 const * CLRPC_COMMON_MEM_U8_CONST_PTR_TYPE;

typedef LSA_VOID * CLRPC_UPPER_MEM_PTR_TYPE; /* pointer to upper-memory */

/*-----------------------------------------------------------------------------
// forward declaration
//---------------------------------------------------------------------------*/

typedef struct clrpc_rqb_tag * CLRPC_UPPER_RQB_PTR_TYPE;
typedef struct clrpc_rqb_tag const * CLRPC_UPPER_RQB_CONST_PTR_TYPE;
/*------------------------------------------------------------------------------
//	OPCODES
//	CLRPC_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup clrpc-opcode-overview Opcode overview
@ingroup clrpc-opcodes
@{
@brief Overview of all CLRPC opcodes.
******************************************************************************/
#define CLRPC_OPC_OPEN_CHANNEL				0x01 /**< see @ref CLRPC_OPC_OPEN_CHANNEL */
#define CLRPC_OPC_CLOSE_CHANNEL				0x02 /**< see @ref CLRPC_OPC_CLOSE_CHANNEL */
#define CLRPC_OPC_SET_INTERFACE_PARAM		0x03 /**< see @ref CLRPC_OPC_SET_INTERFACE_PARAM */
#define CLRPC_OPC_CLEAR_INTERFACE_PARAM		0x04 /**< see @ref CLRPC_OPC_CLEAR_INTERFACE_PARAM */
#define CLRPC_OPC_TIMER						0x05 /**< see @ref CLRPC_OPC_TIMER */

#define CLRPC_OPC_CL_BIND					0x10 /**< see @ref CLRPC_OPC_CL_BIND */
#define CLRPC_OPC_CL_REBIND					0x11 /**< see @ref CLRPC_OPC_CL_REBIND */
#define CLRPC_OPC_CL_UNBIND					0x12 /**< see @ref CLRPC_OPC_CL_UNBIND */
#define CLRPC_OPC_CL_CALL					0x13 /**< see @ref CLRPC_OPC_CL_CALL */
#define CLRPC_OPC_CL_CANCEL					0x14 /**< see @ref CLRPC_OPC_CL_CANCEL */
#define CLRPC_OPC_CL_QUERY_EPM_BIND			0x15 /**< don't use CLRPC_ANY_ARGS_TYPE see @ref CLRPC_OPC_CL_QUERY_EPM */
#define CLRPC_OPC_CL_QUERY_EPM_NEXT			0x16 /**< don't use CLRPC_ANY_ARGS_TYPE see @ref CLRPC_OPC_CL_QUERY_EPM */
#define CLRPC_OPC_CL_QUERY_EPM_BREAK		0x17 /**< don't use CLRPC_ANY_ARGS_TYPE see @ref CLRPC_OPC_CL_QUERY_EPM */
#define CLRPC_OPC_CL_QUERY_EPM_UNBIND		0x18 /**< don't use CLRPC_ANY_ARGS_TYPE see @ref CLRPC_OPC_CL_QUERY_EPM */

#define CLRPC_OPC_SV_REGISTER_IF_OBJ		0x20 /**< see @ref CLRPC_OPC_SV_REGISTER_IF_OBJ */
#define CLRPC_OPC_SV_UNREGISTER_IF_OBJ		0x21 /**< see @ref CLRPC_OPC_SV_UNREGISTER_IF_OBJ */
#define CLRPC_OPC_SV_reserved_22			0x22 /**< was _OBJECT_REGISTER */
#define CLRPC_OPC_SV_reserved_23			0x23 /**< was _OBJECT_UNREGISTER */
#define CLRPC_OPC_SV_CALL_IND_PROVIDE		0x24 /**< see @ref CLRPC_OPC_SV_CALL_IND_PROVIDE */
#define CLRPC_OPC_SV_CALL_INDICATION		0x25 /**< see @ref CLRPC_OPC_SV_CALL_INDICATION */
#define CLRPC_OPC_SV_CALL_RESPONSE			0x26 /**< see @ref CLRPC_OPC_SV_CALL_RESPONSE */

#define CLRPC_OPC_EPM_START					0x30 /**< see @ref CLRPC_OPC_EPM_START */
#define CLRPC_OPC_EPM_STOP					0x31 /**< see @ref CLRPC_OPC_EPM_STOP */

#define CLRPC_OPC_MAX						0x32 /**< Maximum of CLRPC Opcodes */
/** @} ***********************************************************************/

/*------------------------------------------------------------------------------
//	ERROR_CODES
//	CLRPC_RQB_TYPE::response
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup clrpc-group-response-codes Response Codes
@ingroup clrpc-group
@{
@brief CLRPC_RQB response codes
@details The following codes are used as response in CLRPC_RQB_TYPE::response.
******************************************************************************/
#define CLRPC_OK						LSA_RSP_OK                  /**< 0x01 */
#define CLRPC_OK_CANCELLED				(LSA_RSP_OK_OFFSET + 1)     /**< 0x41 */
#define CLRPC_OK_ACTIVE					(LSA_RSP_OK_OFFSET + 2)     /**< 0x42, internal use only */

#define CLRPC_ERR_RESOURCE				LSA_RSP_ERR_RESOURCE        /**< 0x84 */
#define CLRPC_ERR_PARAM					LSA_RSP_ERR_PARAM           /**< 0x86 */
#define CLRPC_ERR_SEQUENCE				LSA_RSP_ERR_SEQUENCE        /**< 0x87 */
#define CLRPC_ERR_SYS_PATH				LSA_RSP_ERR_SYS_PATH        /**< 0x91 */

enum
{
	CLRPC_ERR_OPCODE = LSA_RSP_ERR_OFFSET,
							/**< (0xc0/192) wrong opcode (server or client not configured?) */
	CLRPC_ERR_LOWER_LAYER,	/**< (0xc1/193) an error occurred in the lower layer */
	CLRPC_ERR_ALLOC_LOCAL,	/**< (0xc2/194) could not allocate local memory */
	CLRPC_ERR_ALLOC_LOWER,	/**< (0xc3/195) could not allocate lower memory */
	CLRPC_ERR_REJECTED,		/**< (0xc4/196) epm or server did reject the call, see "nca_status" */
	CLRPC_ERR_FAULTED,		/**< (0xc5/197) server had a fault while executing the call, see "nca_status" */
	CLRPC_ERR_TIMEOUT,		/**< (0xc6/198) epm or server did not respond within (rtx-max * rtx-timeout) */
	CLRPC_ERR_IN_ARGS,		/**< (0xc7/199) broadcast or maybe "ndr_data" too large, see channel-details */
	CLRPC_ERR_OUT_ARGS,		/**< (0xc8/200) server sent back more than "alloc_len" */
	CLRPC_ERR_DECODE,		/**< (0xc9/201) result of epm-query/break could not be decoded */
	CLRPC_ERR_PNIO_OUT_ARGS,/**< (0xca/202) out-args not "PN IO signature", too short or inconsistent */
	CLRPC_ERR_SERVER_BUSY	/**< (0xca/203) epm server is busy and does not accept any more calls - for internal use only! */

	/*** attention: max. 62 error codes ***/
};
/**@}*************************************************************************/

/*-----------------------------------------------------------------------------
// NCA status codes (incomplete list!)
// the additional information of the "nca_status" may help to
// track down a problem when CLRPC_ERR_xxx is not enough.
//---------------------------------------------------------------------------*/

/* DCE RPC NCA-status codes (see file ncastat.idl) */

#define CLRPC_NCA_COMM_FAILURE				0x1C010001 /* unable to get response from server */
#define CLRPC_NCA_OP_RANGE_ERR				0x1C010002 /* bad operation number */
#define CLRPC_NCA_UNKNOWN_IF				0x1C010003 /* unknown interface */
#define CLRPC_NCA_WRONG_BOOT_TIME			0x1C010006 /* client passed wrong boot time to server */
#define CLRPC_NCA_YOU_CRASHED				0x1C010009 /* a restartet server called back a client */
#define CLRPC_NCA_PROTO_ERROR				0x1C01000B /* someone messed up the protocol */
#define CLRPC_NCA_OUT_ARGS_TOO_BIG			0x1C010013 /* out-args are too big */
#define CLRPC_NCA_SERVER_TOO_BUSY			0x1C010014 /* server is too busy to handle call */

#define CLRPC_NCA_UNSPECIFIC_REJECT			0x1C000009
#define CLRPC_NCA_WHO_ARE_YOU_FAILED		0x1C00000B
#define CLRPC_NCA_MANAGER_NOT_ENTERED		0x1C00000C
#define CLRPC_NCA_FAULT_UNSPECIFIC			0x1C000012
#define CLRPC_NCA_FAULT_REMOTE_COMM_ERR		0x1C000013
#define CLRPC_NCA_CONTEXT_MISMATCH			0x1C00001A
#define CLRPC_NCA_FAULT_REMOTE_NO_MEMORY	0x1C00001B

/*-----------------------------------------------------------------------------
// DREP (data representation) type and codes
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@addtogroup clrpc-opcodes-server-call
@{
******************************************************************************/
typedef struct
{
	LSA_UINT8 drep_char;
	LSA_UINT8 drep_int;
	LSA_UINT8 drep_float;
	LSA_UINT8 drep_reserverd;
} CLRPC_DREP_TYPE;

	/* ==> never change the values: see clrpc_int.h */

#define CLRPC_DREP_CHAR_ASCII	(0 << 0)
#define CLRPC_DREP_CHAR_EBCDIC	(1 << 0)

#define CLRPC_DREP_INT_BIG		(0 << 4)
#define CLRPC_DREP_INT_LITTLE	(1 << 4)

#define CLRPC_DREP_FLOAT_IEEE	(0 << 0)
#define CLRPC_DREP_FLOAT_VAX	(1 << 0)
#define CLRPC_DREP_FLOAT_CRAY	(2 << 0)
#define CLRPC_DREP_FLOAT_IBM	(3 << 0)

/*-----------------------------------------------------------------------------
// constants for call semantics
//---------------------------------------------------------------------------*/


enum clrpc_semantics_enum
{
	CLRPC_SEMANTICS_AT_MOST_ONCE = 0, /* the default */
	CLRPC_SEMANTICS_IDEMPOTENT,
	CLRPC_SEMANTICS_MAYBE,
	CLRPC_SEMANTICS_BROADCAST,
	CLRPC_SEMANTICS_BROADCAST_MAYBE,
	CLRPC_SEMANTICS_MAX /* for range check */
};
/**@}*************************************************************************/

/*------------------------------------------------------------------------------
// PN IO signature
// (see clrpc_pdu.h)
//----------------------------------------------------------------------------*/

#define CLRPC_PNIO_OFFSET (4 + 4 + 4+4+4) /* offset for "user octets" in ndr_data */

/*
 * NOTE: the coding of the 'pnio_status' is application-specific
 */

#define CLRPC_PNIO_OPNUM_CONNECT       0
#define CLRPC_PNIO_OPNUM_RELEASE       1
#define CLRPC_PNIO_OPNUM_READ          2
#define CLRPC_PNIO_OPNUM_WRITE         3
#define CLRPC_PNIO_OPNUM_CONTROL       4
#define CLRPC_PNIO_OPNUM_READ_IMPLICIT 5
#define CLRPC_PNIO_OPNUM_MAX           CLRPC_PNIO_OPNUM_READ_IMPLICIT /* the last opnum, see CLRPC_SV_REGISTER_IF_OBJ_TYPE */

/*------------------------------------------------------------------------------
// size and format string for pnio-annotation (see CLRPC_OPC_SV_REGISTER_IF_OBJ / CLRPC_OPC_EPM_START)
// intended usage: sprintf()
//
// RPCAnnotation ::=
//  DeviceType, Blank, OrderID, Blank, HWRevision, Blank,
//  SWRevisionPrefix, SWRevision, EndTerm
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@addtogroup clrpc-opcodes
@{
******************************************************************************/
#define CLRPC_MAX_ANNOTATION_SIZE (63+1) /**< size is defined by DCE RPC, see ep.idl */

#define CLRPC_PNIO_ANNOTATION_FORMAT   "%-25.25s %-20.20s %5u %c%3u%3u%3u" /**< pnio annotation format */

#define CLRPC_PNIO_ANNOTATION(sDeviceType, sOrderID, uHWRevision, cSWRevisionPrefix, uSWRevision1, uSWRevision2, uSWRevision3) \
	CLRPC_PNIO_ANNOTATION_FORMAT, sDeviceType, sOrderID, uHWRevision, cSWRevisionPrefix, uSWRevision1, uSWRevision2, uSWRevision3
/**@}*************************************************************************/

/**************************************************************************//**
@defgroup clrpc-opcodes-channel 0x01-0x04 Channel Opcodes
@ingroup clrpc-opcodes
@brief CLRPC channel specific opcodes
******************************************************************************/

/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define CLRPC_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(/*ATTR*/,
CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE)(
	CLRPC_UPPER_RQB_PTR_TYPE rb
);

/**************************************************************************//**
@defgroup CLRPC_OPC_OPEN_CHANNEL 0x01 CLRPC_OPC_OPEN_CHANNEL
@ingroup clrpc-opcodes-channel
@{
@brief Generates and initializes a new CLRPC instance.
@details
Refer to clrpc_open_channel().

## RQB

Parameter name        | R/C     | Description
----------------------|---------|------------------------------------
rqb.opcode            | REQ     | CLRPC_OPC_OPEN_CHANNEL
rqb.handle            | REQ     | Don't care
|                     | CNF     | Channel Handle of the caller (Upper Layer)
rqb.response          | CNF     | Result of the request.
|                     |         | CLRPC_OK \n The channel is opened.
|                     |         | CLRPC_ERR_RESOURCE \n No available channel resource (refer [CLRPC_CFG_MAX_CHANNELS](@ref clrpc-anchor-clrpc-cfg-max-channels))
|                     |         | CLRPC_ERR_SYS_PATH \n Invalid Sys-Path.
|                     |         | CLRPC_ERR_ALLOC_LOWER \n No RQB could be allocated to open the lower layer.
|                     |         | CLRPC_ERR_LOWER_LAYER \n The channel to the lower layer could not be opened.
rqb.args.channel.open | REQ/CNF | Refer to @ref CLRPC_OPEN_CHANNEL_TYPE

## Example
@snippet "code examples/clrpc_opc_open_channel_example.c" clrpc-opc-open-channel-snippet
******************************************************************************/
typedef struct
{
	LSA_HANDLE_TYPE		handle;
	/**< ## CNF
	 * Handle of generated CLRPC instance. This handle is to be given during future calls.
	 */
	LSA_HANDLE_TYPE		handle_upper;
	/**< ## REQ
	 * channel-handle of user, every confirmation of a request set this handle to rqb.handle
	 */
	LSA_SYS_PATH_TYPE	sys_path;
	/**< ## REQ
	 * Sys-Path of channel as per LSA. Also refer to @ref CLRPC_GET_PATH_INFO
	 */
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE
						clrpc_request_upper_done_ptr;
	/**< ## REQ
	 * Pointer to the callback function, which is called as response to a request. This function is also called during an indication. \n
	 * As soon as the communication channel is opened, this callback function can be called at any time by CLRPC.
	 * The user is responsible for ensuring that the code sections that are not to be interrupted by the CLRPC are made non-interruptible or to take other suitable measures.
	 */
} CLRPC_OPEN_CHANNEL_TYPE;

typedef CLRPC_OPEN_CHANNEL_TYPE * CLRPC_UPPER_OPEN_CHANNEL_PTR_TYPE; /**< The pointer type used for CM_OPC_OPEN_CHANNEL. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CLOSE_CHANNEL 0x02 CLRPC_OPC_CLOSE_CHANNEL
@ingroup clrpc-opcodes-channel
@{
@brief Closes an (opened) communication channel.
@details
Refer to clrpc_close_channel().

## RQB

Parameter name          | R/C     | Description
------------------------|---------|------------------------------------
rqb.opcode              | REQ     | CLRPC_OPC_CLOSE_CHANNEL
rqb.handle              | REQ     | Channel Handle
rqb.response            | CNF     | Result of the request.
|                       |         | CLRPC_OK \n The channel is closed.
|                       |         | CLRPC_ERR_SEQUENCE \n Clients or servers are still available.
|                       |         | No RQB was allocated to close the lower layer
|                       |         | CLRPC_ERR_LOWER_LAYER \n The channel to the lower layer could not be closed.
|rqb.args.channel.close | REQ/CNF | LSA_NULL \n no parameters.

## Example
@snippet "code examples/clrpc_opc_close_channel_example.c" clrpc-opc-close-channel-snippet
******************************************************************************/
typedef LSA_VOID_PTR_TYPE * CLRPC_UPPER_CLOSE_CHANNEL_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CLOSE_CHANNEL. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SET_INTERFACE_PARAM
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_SET_INTERFACE_PARAM 0x03 CLRPC_OPC_SET_INTERFACE_PARAM
@ingroup clrpc-opcodes-channel
@{
@brief Additional parameters for the channel -for example the MAC address- will be set.
@details
If the user requests for client functionality, a socket to the lower layer will be opened and the receive resources will be provided.
Only for channels with client functionality the MAC address may be 0.0.0.0.

The interface parameters must have been set for each channel before any of the following requests is given: \n
CLRPC_OPC_CL_BIND, CLRPC_OPC_SV_REGISTER_IF_OBJ, CLRPC_OPC_CL_QUERY_EPM_BIND or CLRPC_OPC_EPM_START.

## RQB

Parameter name      | R/C   | Description
--------------------|-------|------------------------------------
rqb.opcode          | REQ   | CLRPC_OPC_SET_INTERFACE_PARAM
rqb.handle          | REQ   | Channel Handle
rqb.response        | CNF   | Result of the request.
|                   |       | CLRPC_OK \n A client instance was successfully generated and "bound".
|                   |       | CLRPC_ERR_SYS_PATH \n Opcode for this channel  path_type not specified.
|                   |       | CLRPC_ERR_SEQUENCE \n Channel not opened or interface parameters are already set.
|                   |       | CLRPC_ERR_PARAM \n Invalid parameter.
|                   |       | CLRPC_ERR_ALLOC_LOWER \n RQB needed to open the socket or the receive-resources could not be allocated.
|                   |       | CLRPC_ERR_LOWER_LAYER \n The socket could not be opened.
|rqb.args.channel.set_interface_param | REQ/CNF | Refer to CLRPC_SET_INTERFACE_PARAM_TYPE.
******************************************************************************/
typedef struct clrpc_set_interface_param_tag
{
	LSA_UINT32	trace_idx;
	/**< ## REQ
	 * Used by LSA indexed trace.
	 * @see CLRPC_CFG_TRACE_MODE
	 */

	LSA_UINT8	mac_addr[6];
	/**< ## REQ
	 * MAC address for this channel. Needed to generate UUIDs or 0 if not known.
	 */

	LSA_UINT16  client_receive_resources;
	/**< ## REQ
	 * Number of receive resources for the client, \n
	 * 0 ==> no socket will be opend for client functions, no client functionality wanted
	 */
} CLRPC_SET_INTERFACE_PARAM_TYPE;

typedef CLRPC_SET_INTERFACE_PARAM_TYPE * CLRPC_UPPER_SET_INTERFACE_PARAM_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_SET_INTERFACE_PARAM. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CLEAR_INTERFACE_PARAM
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CLEAR_INTERFACE_PARAM 0x04 CLRPC_OPC_CLEAR_INTERFACE_PARAM
@ingroup clrpc-opcodes-channel
@{
@brief Undo of CLRPC_OPC_SET_INTERFACE_PARAM.
@details
The parameters will be cleared and an opened socket will be closed.

These actions are also done implicitly during the CLOSE_CHANNEL-Request.


## RQB

Parameter name      | R/C   | Description
--------------------|-------|------------------------------------
rqb.opcode          | REQ   | CLRPC_OPC_CLEAR_INTERFACE_PARAM
rqb.handle          | REQ   | Channel Handle
rqb.response        | CNF   | Result of the request.
|                   |       | CLRPC_OK \n The parameters were successfully cleared.
|                   |       | CLRPC_ERR_SEQUENCE \n Client or server functions are still active.
|                   |       | CLRPC_ERR_PARAM \n Invalid parameter or handle.
|rqb.args.channel.clear_interface_param | REQ/CNF | LSA_NULL \n No parameter
******************************************************************************/
typedef LSA_VOID_PTR_TYPE * CLRPC_UPPER_CLEAR_INTERFACE_PARAM_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CLEAR_INTERFACE_PARAM. */
/**@}*************************************************************************/


/**************************************************************************//**
@internal
@defgroup CLRPC_OPC_TIMER 0x05 CLRPC_OPC_TIMER
@ingroup clrpc-opcodes
@brief Only internally used.
@details
The synchronization of LSA-Timer with the CLRPC task is done.

Refer to @ref clrpc-architecture-constraints-connection-lsa-timer
@endinternal
******************************************************************************/


/**************************************************************************//**
@defgroup clrpc-opcodes-client 0x10-0x19 Client Opcodes
@ingroup clrpc-opcodes
@brief CLRPC client specific opcodes.
******************************************************************************/

/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_BIND
// OPCODE: CLRPC_OPC_CL_REBIND
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CL_BIND 0x10 CLRPC_OPC_CL_BIND
@ingroup clrpc-opcodes-client
@{
@brief Generates a client instance and "binds" it.
@details
It generates a client instance and links this to the specified host, port and interface UUID and interface version and object UUID.
This operation is local, no network activity.

If 0 is entered for the port ("partially bound"), the first call is sent to the endpoint mapper port of the given host.

Upon receiving the first response, the client instance is "fully bound". This means:
All following calls go directly to the server and are not directed via the endpoint mapper.

If it was "partially" at the beginning, the complete connection can become "partially" again as the server may fail.
During a restart a new port number could be assigned to it.


## RQB

Parameter name      | R/C     | Description
--------------------|---------|------------------------------------
rqb.opcode          | REQ     | CLRPC_OPC_CL_BIND
rqb.handle          | REQ     | Channel Handle
rqb.response        | CNF     | Result of the request.
|                   |         | CLRPC_OK \n A client instance is generated and "bound".
|                   |         | CLRPC_ERR_OPCODE \n No client configured. \n (refer to [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients))
|                   |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel  path_type not specified.
|                   |         | CLRPC_ERR_RESOURCE \n The maximum number of clients is limited by [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients).
|                   |         | CLRPC_ERR_ALLOC_LOCAL \n Client instance could not be allocated.
|                   |         | CLRPC_ERR_ALLOC_LOWER \n Sending resources or RQB to open the socket have not been allocated.
|                   |         | CLRPC_ERR_LOWER_LAYER \n The socket could not be opened.
|                   |         | CLRPC_ERR_PARAM \n IP address invalid.
|                   |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state.
|rqb.args.cl.bind   | REQ/CNF | refer to CLRPC_CL_BIND_TYPE
******************************************************************************/
#define CLRPC_INVALID_CLIENT_ID  LSA_NULL

typedef struct
{
	LSA_VOID_PTR_TYPE	client_id;
	/**<
	 * first in struct!
	 * ## CLRPC_OPC_CL_BIND: CNF
	 * ID of the generated client instance
	 *
	 * ## CLRPC_OPC_CL_REBIND: REQ
	 * ID of client instance to be used
	 */

	LSA_UINT32			host_ip;
	/**< ## REQ
	 * IP address of the server that provides the given interface during the subsequent call.\n
	 * Network byte order! see @ref CLRPC_HTONL()
	 */

	LSA_UINT16			udp_port;
	/**< ## REQ
	 * The ("well known") UDP Port Number of the server or 0 = port number of the endpoint-mapper from the channel-details. \n
	 * Network byte order! see @ref CLRPC_HTONS()
	 */

	CLRPC_UUID_TYPE		if_uuid;
	/**< ## REQ
	 * The UUID of the interface
	 */

	LSA_UINT16			if_version_major;
	/**< ## REQ
	 * The major version of the interface
	 */

	LSA_UINT16			if_version_minor;
	/**< ## REQ
	 * The minor version of the interface
	 */

	CLRPC_UUID_TYPE		object_uuid;
	/**< ## REQ
	 * Object UUID
	 */

	/* AP01309324: timeout_msec deleted, CLRPC_RTX_TIMEOUT_MSEC will be used now */
} CLRPC_CL_BIND_TYPE;

typedef CLRPC_CL_BIND_TYPE * CLRPC_UPPER_CL_BIND_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CL_BIND. */

typedef CLRPC_CL_BIND_TYPE const * CLRPC_UPPER_CL_BIND_CONST_PTR_TYPE; /**< The const pointer type used for CLRPC_OPC_CL_BIND. */
/**@}*************************************************************************/


/**************************************************************************//**
@defgroup CLRPC_OPC_CL_REBIND 0x11 CLRPC_OPC_CL_REBIND
@ingroup clrpc-opcodes-client
@brief Rebinds an existing client instance.
@details
Changes host and / or port and / or interface UUID/version and / or Object UUID of a client instance.
This operation is local, no network activity. All subsequent "calls" via this instance go to the modified host or port.

Rebind is functionally the same as the sequence Unbind - Bind, but prevents the release and new allocation of the client instance.

Rebind uses the same parameter structure as Bind (refer to @ref CLRPC_CL_BIND_TYPE).
The only difference is that for Rebind, the Client-ID is an IN-Parameter.

Before calling a Rebind request, you do not need to wait for the completion of a previous call that is still running!

## RQB

Parameter name             | R/C     | Description
---------------------------|---------|------------------------------------
rqb.opcode                 | REQ     | CLRPC_OPC_CL_REBIND
rqb.handle                 | REQ     | Channel Handle
rqb.response               | CNF     | Result of the request.
|                          |         | CLRPC_OK \n The (existing) client instance was newly "bound".
|                          |         | CLRPC_ERR_OPCODE \n No client configured. \n (refer to [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients))
|                          |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel  path_type not specified.
|                          |         | CLRPC_ERR_PARAM \n IP address invalid.
|                          |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state.
|                          |         | CLRPC_ERR_PARAM \n IP address invalid
|rqb.args.cl.bind          | REQ/CNF | refer to CLRPC_CL_BIND_TYPE
******************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_UNBIND
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CL_UNBIND 0x12 CLRPC_OPC_CL_UNBIND
@ingroup clrpc-opcodes-client
@{
@brief Releases a client instance.
@details
Before calling an Unbind request, you do not need to wait for the completion of a previous call that is still running!

## RQB

Parameter name      | R/C     | Description
--------------------|---------|------------------------------------
rqb.opcode          | REQ     | CLRPC_OPC_CL_UNBIND
rqb.handle          | REQ     | Channel Handle
rqb.response        | CNF     | Result of the request.
|                   |         | CLRPC_OK \n The results of the client instance are again released. The client handle is no longer valid.
|                   |         | CLRPC_ERR_OPCODE \n No client configured. \n (refer to [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients))
|                   |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel  path_type not specified.
|                   |         | CLRPC_ERR_PARAM \n Invalid client handle
|                   |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state.
|rqb.args.cl.unbind | REQ/CNF | refer to CLRPC_CL_UNBIND_TYPE
******************************************************************************/
typedef struct
{
	LSA_VOID_PTR_TYPE	client_id;
	/**<
	 * first in struct!
	 * ## REQ
	 * ID of client instance to be released.
	 * ## CNF
	 * 0
	 */
} CLRPC_CL_UNBIND_TYPE;

typedef CLRPC_CL_UNBIND_TYPE * CLRPC_UPPER_CL_UNBIND_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CL_UNBIND. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_CALL
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CL_CALL 0x13 CLRPC_OPC_CL_CALL
@ingroup clrpc-opcodes-client
@{
@brief Executes a "remote procedure call" on the host previously given in "Bind".
@details
Before calling a Call request, you do not need to wait for the completion of a previous call or rebind that is still running!

@note
If a call gives the Response == CLRPC_ERR_TIMEOUT, a Rebind must be carried out before a fresh call.
In the rare(st) cases (Keyword: Bindings cached on server side), interruption of power, Activity Wrap Around) the server can seem dead,
as it unfortunately sees all packages as "old" and silently discards them (at the latest after 2^31 repeats, it would again respond...).

##PN IO Signature

In order to call a function with a PN IO Signature, the flag "pnio_signature" is to be set to 1.

"pnio_status" is an OUT parameter. CLRPC only transports this value. Coding is to be done by the user.

@see @ref clrpc-architecture-behavior-signature

## RQB

Parameter name      | R/C     | Description
--------------------|---------|------------------------------------
rqb.opcode          | REQ     | CLRPC_OPC_CL_CALL
rqb.handle          | REQ     | Channel Handle
rqb.response        | CNF     | Result of the request.
|                   |         | CLRPC_OK \n The call was successfully completed.
|                   |         | CLRPC_OK_CANCELLED \n The call was cancelled (refer @ref CLRPC_OPC_CL_CANCEL) or the "Application Timeout" occurred (refer Parameter CLRPC_CL_CALL_TYPE::appl_timeout1sec). \n Possibly the call was partially or completely carried out on the server side!
|                   |         | CLRPC_ERR_OPCODE \n No clients configured. (refer to [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients))
|                   |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel-path_type not specified.
|                   |         | CLRPC_ERR_PARAM \n Invalid client handle or invalid semantics.
|                   |         | CLRPC_ERR_SEQUENCE \n Call occurred after "unbind".
|                   |         | CLRPC_ERR_REJECTED \n The Endpoint Mapper or server rejected the call. Refer CLRPC_CL_CALL_TYPE::nca_status for more details. \n The call was not carried out (even partially) at the server side.
|                   |         | CLRPC_ERR_FAULTED \n The Endpoint Mapper or the server rejected the call. Refer 'nca_status' for more details. \n The call was already partially executed at the server side. \n Exception: CLRPC_NCA_MANAGER_NOT_ENTERED
|                   |         | CLRPC_ERR_TIMEOUT \n The Endpoint Mapper or the server did not respond in a timely manner (refer channel details).
|                   |         | CLRPC_ERR_IN_ARGS \n  CLRPC_CL_CALL_TYPE::ndr_len is too large. During broadcast or Maybe Semantic, the "IN-Args" must go fragmented. Refer Channel Details 'send_data_size' (Default is 1384 = 1464 - 80).
|rqb.args.cl.call   | REQ/CNF | refer to CLRPC_CL_CALL_TYPE

## Example
@snippet "code examples/clrpc_opc_cl_call_example.c" clrpc-opc-cl-call-snippet
******************************************************************************/
typedef struct
{
	LSA_VOID_PTR_TYPE	client_id;
	/**< first in struct!
	 * ## REQ
	 * ID of the client instance
	 */

	LSA_UINT16			opnum;
	/**< ## REQ
	 * The function to be executed \n
	 * (Functions are lexicographically ordered in the IDL, starting with 0)
	 */

	LSA_UINT16			semantics;
	/**< ## REQ
	 * Possible values:
	 * - CLRPC_SEMANTICS_AT_MOST_ONCE
	 * - CLRPC_SEMANTICS_IDEMPOTENT
	 *
	 * forbidden (see AP00969270):
	 * - CLRPC_SEMANTICS_MAYBE
	 * - CLRPC_SEMANTICS_BROADCAST
	 * - CLRPC_SEMANTICS_BROADCAST_MAYBE
	 */

	LSA_UINT16			pnio_signature;
	/**< ## REQ
	 * 1=called function has PN IO Signature \n
	 * 0=otherwise
	 *
	 * Refer to @ref clrpc-architecture-behavior-signature
	 */

	LSA_UINT16			appl_timeout1sec;
	/**< ## REQ
	 * 0 = Default (300sec)\n
	 * The server must respond within the time frame or the call will be cancelled.
	 * ## CNF
	 * CLRPC converts this parameter from seconds into "pings"
	 */

	LSA_UINT32			nca_status;
	/**< ## CNF
	 * The NCA status codes defined by DCE RPC (OSF Sources, file ncastat.idl).
	 * Valid if the response == CLRPC_ERR_REJECTED or CLRPC_ERR_FAULTED.
	 *
	 * The most important ones are listed in clrpc_usr.h (CLRPC_NCA_XXX). A CLRPC server only provides these!
	 */

	LSA_UINT32			pnio_status;
	/**< ## CNF
	 * PN IO status code (opaque for CLRPC)
	 *
	 * User defined status, not evaluated by CLRPC. Valid if response == CLRPC_OK
	 * @see @ref clrpc-architecture-behavior-signature
	 */

	CLRPC_DREP_TYPE		drep;
	/**< ## CNF
	 * Data Representation of CNF arguments in "ndr_data". Valid if response == CLRPC_OK.
	 *
	 * Possible values:
	 * - CLRPC_DREP_CHAR_ASCII
	 * - CLRPC_DREP_CHAR_EBCDIC
	 * - CLRPC_DREP_INT_BIG
	 * - CLRPC_DREP_INT_LITTLE
	 */

	LSA_UINT32			alloc_len;
	/**< ## REQ
	 * The size of the allocated storage. Refer @ref ndr_data \n
	 * (= max (IN-Args, OUT-Args)).
	 * @see @ref clrpc-architecture-behavior-signature, args_max.
	 * @see @ref CLRPC_OPC_SV_REGISTER_IF_OBJ, %alloc_len.
	 */

	LSA_UINT32			ndr_len;
	/**<
	 * ## REQ
	 * The size of data to be sent. @see @ref clrpc-architecture-behavior-signature, args_len.
	 *
	 * ## CNF
	 * The size of received data. Valid if response == CLRPC_OK.
	 */

	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data;
	/**< buffer with NDR-encoded data,  must be at least 4 byte aligned
	 * ## REQ
	 * Pointer to the storage of the variable 'alloc_len', in which the first 'ndr_len' bytes with usage data (NDR-coded!) are entered.
	 * These IN arguments are sent to the server.
	 * ## CNF
	 * These OUT arguments were received from the server. Valid if response == CLRPC_OK.
	 */
} CLRPC_CL_CALL_TYPE;

typedef CLRPC_CL_CALL_TYPE * CLRPC_UPPER_CL_CALL_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CL_CALL. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_CANCEL
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CL_CANCEL 0x14 CLRPC_OPC_CL_CANCEL
@ingroup clrpc-opcodes-client
@{
@brief Cancels the currently running call.
@details
A cancel can be uniquely assigned to a call only if the user awaits the end of the cancel and the previous call before withdrawing a new call.

Background: Until the cancel request is processed, a specific time frame is over (Task switching, Mailbox, etc.).
Meanwhile, the server could have just responded or the operation could have timed out.

## RQB

Parameter name      | R/C     | Description
--------------------|---------|------------------------------------
rqb.opcode          | REQ     | CLRPC_OPC_CL_CANCEL
rqb.handle          | REQ     | Channel Handle
rqb.response        | CNF     | Result of the request.
|                   |         | CLRPC_OK \n Ok, but no further call may have been found, that could be cancelled.
|                   |         | CLRPC_OK_CANCELLED \n The cancelled call will also be terminated with CLRPC_OK_CANCELLED "in the near future".
|                   |         | CLRPC_ERR_OPCODE \n No clients configured. (refer [CLRPC_CFG_MAX_CLIENTS](@ref clrpc-anchor-clrpc-cfg-max-clients))
|                   |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel-path_type not specified.
|                   |         | CLRPC_ERR_PARAM \n Invalid client handle.
|                   |         | CLRPC_ERR_SEQUENCE \n Call occurred after "unbind".
|rqb.args.cl.call   | REQ/CNF | refer to CLRPC_CL_CANCEL_TYPE
******************************************************************************/
typedef struct
{
	LSA_VOID_PTR_TYPE	client_id;
	/**< first in struct!
	 * ## REQ
	 * ID of client instance.
	 */
} CLRPC_CL_CANCEL_TYPE;

typedef CLRPC_CL_CANCEL_TYPE * CLRPC_UPPER_CL_CANCEL_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CL_CANCEL. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_BIND
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_NEXT
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_BREAK
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_UNBIND
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_CL_QUERY_EPM 0x15 CLRPC_OPC_CL_QUERY_EPM_BIND / 0x16 NEXT / 0x17 BREAK / 0x18 UNBIND
@ingroup clrpc-opcodes-client
@{
@brief Queries on the Endpoint Mapper database
@details
Due to the non-trivial marshalling and un/de-marshalling of calls of the Endpoint Mapper Functions "lookup" and "lookup_handle_free",
this functionality was "pre-wired" in CLRPC, so to say.

The four following Opcodes enable the queries on the Endpoint Mapper database:
- CLRPC_OPC_CL_QUERY_EPM_BIND
- CLRPC_OPC_CL_QUERY_EPM_NEXT
- CLRPC_OPC_CL_QUERY_EPM_BREAK
- CLRPC_OPC_CL_QUERY_EPM_UNBIND

In addition to the response codes provided during "bind", "call" and "unbind", the following codes could occur:

Parameter name         | R/C     | Description
-----------------------|---------|------------------------------------
rqb.response           | CNF     | Result of the request.
|                      |         | CLRPC_ERR_DECODE \n The response of the Endpoint Mapper query could not be decoded.
|                      |         | CLRPC_ERR_SEQUENCE \n The Opcode was not called in the documented sequence.
rqb.args.cl.query_epm  | REQ/CNF | refer to CLRPC_CL_QUERY_EPM_TYPE

The query uses the mechanism "Context Handle", due to which a specific call sequence needs to be adhered to.

Non-LSA Pseudo code:
@code
	BIND()
	do {
		entry = NEXT()
		if(  entry->valid ) {
			... evaluate
			if( have-read-enough ) {
				BREAK()
				break;
			}
		}
	} while( entry->more )
	UNBIND()
@endcode

The following code examples show the relation between CLRPC LSA-API and the Microsoft / DCE API.

@par CLRPC:
@snippet "code examples/clrpc_relation_lsa_dce_clrpc.c" clrpc-relation-lsa-dce-clrpc-snippet

@par Microsoft / DCE:
@snippet "code examples/clrpc_relation_lsa_dce_microsoft.c" clrpc-relation-lsa-dce-microsoft-snippet


The Query-EPM-Type contains the "state" for the iteration over the Endpoint Mapper database.

The calls are to be executed in the sequence, given in the example above.

The query is enclosed in a CLRPC_OPC_CL_QUERY_EPM_BIND / UNBIND pair.

Bind() generates a new client instance and initializes the iterator. The IP address of the target host is to be provided.
Enter 0 for the port. Default value 0 is entered for the timeout parameter.

Unbind() releases the client instance again.

Using CLRPC_OPC_CL_QUERY_EPM_NEXT, the Endpoint Mapper database can be successively queried. As CLRPC only uses UDP as transport protocol, only UDP entries are decoded.
Due to technical implementation reasons, however, all queries are responded to. The flag "epm_entry.valid" must be verified. If "Not-valid", this entry is to be skipped.
The end of the query is marked by using the flag "epm_entry.more".

If the query shall be interrupted before "epm_entry.more == FALSE", the server must be informed by using  CLRPC_OPC_CL_QUERY_EPM_BREAK,
so that it can also releases its resources (if this is  not done, the resources remain reserved for 5 minutes).
******************************************************************************/
#define CLRPC_QUERY_EPM_NDR_DATA_SIZE 256 /**< see clrpc_int.h, too */

/** @cond INTERNAL */
enum clrpc_query_epm_state_enum
{
	CLRPC_QUERY_EPM_BIND = 0,
	CLRPC_QUERY_EPM_LOOKUP_FIRST,
	CLRPC_QUERY_EPM_LOOKUP_NEXT,
	CLRPC_QUERY_EPM_LOOKUP_FREE,
	CLRPC_QUERY_EPM_UNBIND,
	CLRPC_QUERY_EPM_STOP
}; /* used internally only! */
/** @endcond */

typedef struct
{
	union
	{
		CLRPC_CL_BIND_TYPE bind;
		CLRPC_CL_CALL_TYPE call;
		CLRPC_CL_UNBIND_TYPE unbind;
	} u; /**< first in struct! */

	/*** a user-supplied buffer for the query ***/

	LSA_UINT32 alloc_len;
	/**<
	 * ## REQ
	 * actual length of ndr_data, must be at least CLRPC_QUERY_EPM_NDR_DATA_SIZE bytes in size
	 */

	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data;
	/**< ## REQ / CNF
	 * buffer with NDR-encoded data
	 * must be at least 4 byte aligned
	 */

	/*** the result ***/

	struct
	{
		LSA_UINT16 valid; /**< != 0 if the entry is valid */
		LSA_UINT16 more;  /**< != 0 if there are more entries */
		/***/
		CLRPC_UUID_TYPE object_uuid;
		CLRPC_UUID_TYPE if_uuid;
		LSA_UINT16 if_vers_major;
		LSA_UINT16 if_vers_minor;
		LSA_UINT32 host_ip;  /**< network byte order! see CLRPC_NTOHL() */
		LSA_UINT16 udp_port; /**< network byte order! see CLRPC_NTOHS() */

		LSA_UINT8 annotation[CLRPC_MAX_ANNOTATION_SIZE];
	} epm_entry;
	/**< ## CNF
	 * the result
	 */

	/*** the rest is for "internal" use only ***/

	LSA_UINT16 state; /**< see clrpc_query_epm_state_enum */

	CLRPC_UUID_TYPE ctx_uuid; /**< context-handle UUID */

} CLRPC_CL_QUERY_EPM_TYPE;

typedef CLRPC_CL_QUERY_EPM_TYPE * CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_CL_QUERY_EPM_*. */
/**@}*************************************************************************/


/**************************************************************************//**
@defgroup clrpc-opcodes-server 0x20-0x29 Server Opcodes
@ingroup clrpc-opcodes
@brief CLRPC server specific opcodes.
******************************************************************************/

/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_REGISTER_IF_OBJ
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_SV_REGISTER_IF_OBJ 0x20 CLRPC_OPC_SV_REGISTER_IF_OBJ
@ingroup clrpc-opcodes-server
@{
@brief Creates a server instance for the given interface-object-uuid pair.
@details

## RQB

Parameter name          | R/C     | Description
------------------------|---------|------------------------------------
rqb.opcode              | REQ     | CLRPC_OPC_SV_REGISTER_IF_OBJ
rqb.handle              | REQ     | Channel Handle
rqb.response            | CNF     | Result of the request.
|                       |         | CLRPC_OK
|                       |         | CLRPC_ERR_OPCODE \n No server configured. ([CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                       |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel path_type not specified.
|                       |         | CLRPC_ERR_RESOURCE \n No more resources for register interface / object uuid ([CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                       |         | CLRPC_ERR_PARAM
|                       |         | CLRPC_ERR_SEQUENCE \n The interface / object uuid was already registered.
|                       |         | CLRPC_ERR_ALLOC_LOWER
|                       |         | CLRPC_ERR_LOWER_LAYER \n SOCK_OPC_UDP_OPEN gave an error.
|                       |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state
|rqb.args.sv.reg_if_obj | REQ/CNF | refer to CLRPC_SV_REGISTER_IF_OBJ_TYPE

To interface: if_uuid, if_version_major, if_version_minor: \n
If the call does not match the given interface, the call will be rejected with the error code CLRPC_NCA_UNKNOWN_IF (= nca_s_unk_if).

Procedure for a received RPC call to identify an interface-object-uuid pair:
1. Is the received Interface UUID registered?  If yes: next step
2. Is the received Version registered?         If yes: next step
3. Is the received object UUID registered?     If yes: use this interface-object-uuid
4. Is the NIL object UUID registered?          If yes: use this interface-object-uuid
5. Is the received Interface UUID registered?  If yes: next step
6. Is there a compatible Version registered?   If yes: next step
7. Is the received object UUID registered?     If yes: use this interface-object-uuid
8. Is the NIL object UUID registered?          If yes: use this interface-object-uuid
9. Else: reject the call with error code CLRPC_NCA_UNKNOWN_IF
******************************************************************************/
#define CLRPC_INVALID_SERVER_ID			0xFFFF

typedef struct clrpc_sv_register_if_obj_tag
{
	LSA_UINT16			server_id;
	/**< ## CNF
	 * The ID of the generated server instance if response is ok.
	 *
	 * This ID identifies an interface-uuid / object-uuid pair. This is needed to unregister this pair.
	 * In addition this id is given by the CLRPC_OPC_SV_CALL_INDICATION.
	 */

	CLRPC_UUID_TYPE		if_uuid;
	/**< ## REQ
	 * The UUID of the interface to be registered.
	 */

	LSA_UINT16			if_version_major;
	/**< ## REQ
	 * The major version of the interface to be registered.
	 */

	LSA_UINT16			if_version_minor;
	/**< ## REQ
	 * The minor version of the interface to be registered.
	 */

	LSA_UINT16			udp_port;
	/**< ## REQ
	 * >0..use this udp-port, 0..sock select a free one
	 * ## CNF
	 * The port number assigned by UDP stack given in network order.
	 */

	LSA_UINT32			alloc_len;
	/**< ## REQ
	 * If pnio_signature == 0: \n
	 * The size of storage to be allocated. (= max(REQ-Args, CNF-Args) of all functions of the interface). See CALL_INDICATION.
	 *
	 * If pnio_signatur == 1: \n
	 * The required size for each individual CLRPC_OPC_SV_CALL_INDICATION is read from the first fragment and corresponds to the size of the array 'args',
	 * refer @ref clrpc-architecture-behavior-signature, size_is(args_max).
	 * If the required size is larger than 'alloc_len', the required size is limited to 'alloc_len'.
	 *
	 * Must be <= 0xFFFF.
	 *
	 * If the client sends more data than allowed by alloc_len, this call will be rejected with error code CLRPC_NCA_FAULT_REMOTE_NO_MEMORY (= nca_s_fault_remote_no_memory).
	 */

	LSA_UINT16			opnum_max;
	/**< ## REQ
	 * The largest Opnum of this interface. \n
	 * For opnum check, 0 <= opnum <= opnum_max
	 *
	 * If the client sends an 'opnum' that is bigger than 'opnum_max', this call will be rejected with the error code CLRPC_NCA_OP_RANGE_ERR (= nca_s_op_rng_error).
	 */

	LSA_UINT16			pnio_signature;
	/**< ## REQ
	 * 1 .. All functions of this interface have the "PN IO Signature"
	 *
	 * 0 .. Otherwise
	 */

	LSA_UINT16			recv_resource_count;
	/**< ## REQ
	 * Number of RQBs, which are provided to the subordinated SOCK layer as receive buffer.
	 * Must be > 0.
	 */

	LSA_UINT16			send_resource_count;
	/**< ## REQ
	 * Maximum number of RQBs that have been pre-allocated and made available for send orders.
	 * Must be > 0.
	 */

/* if_obj_type */
	CLRPC_UUID_TYPE		object_uuid;
	/**< ## REQ
	 * The Object UUID to be released within this interface.
	 *
	 * If all Object UUID are to be received by this interface, the NIL-UUID must be registered.
	 */

	LSA_VOID_PTR_TYPE	usr_id;
	/**< ## REQ
	 * This usr_id will be given to the @see CLRPC_OPC_SV_CALL_INDICATION, which is filtered by this interface-object-uuid pair.
	 */

	LSA_UINT8			annotation[CLRPC_MAX_ANNOTATION_SIZE];
	/**< ## REQ
	 * The textual note that is returned by the endpoint-mapper for this interface and objectuuid.
	 * See @ref CLRPC_PNIO_ANNOTATION_FORMAT too.
	 */
} CLRPC_SV_REGISTER_IF_OBJ_TYPE;

typedef CLRPC_SV_REGISTER_IF_OBJ_TYPE * CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_SV_REGISTER_IF_OBJ. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_UNREGISTER_IF_OBJ
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_SV_UNREGISTER_IF_OBJ 0x21 CLRPC_OPC_SV_UNREGISTER_IF_OBJ
@ingroup clrpc-opcodes-server
@{
@brief Releases the resources of an interface-uuid server.
@details
@note the user must wait at least one second during unregister/register of the same interface.
To understand this, relate it with the output macro for Boot Time.

## RQB

Parameter name              | R/C     | Description
----------------------------|---------|------------------------------------
rqb.opcode                  | REQ     | CLRPC_OPC_SV_REGISTER_IF_OBJ
rqb.handle                  | REQ     | Channel Handle
rqb.response                | CNF     | Result of the request.
|                           |         | CLRPC_OK
|                           |         | CLRPC_ERR_OPCODE \n No server configured. (refer [CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                           |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel path_type not specified.
|                           |         | CLRPC_ERR_PARAM
|                           |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment, wrong state
|rqb.args.sv.unreg_if_obj   | REQ/CNF | refer to CLRPC_SV_UNREGISTER_IF_OBJ_TYPE
******************************************************************************/
typedef struct clrpc_sv_unregister_if_obj_tag
{
	LSA_UINT16		server_id;
	/**< ## REQ
	 * The ID for the server instance to be released.
	 */
} CLRPC_SV_UNREGISTER_IF_OBJ_TYPE;

typedef CLRPC_SV_UNREGISTER_IF_OBJ_TYPE * CLRPC_UPPER_SV_UNREGISTER_IF_OBJ_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_SV_UNREGISTER. */
/**@}*************************************************************************/


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_CALL_IND_PROVIDE
// INDICATION: CLRPC_OPC_SV_CALL_INDICATION
// OPCODE: CLRPC_OPC_SV_CALL_RESPONSE
//---------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup clrpc-opcodes-server-call 0x24-0x-26 CLRPC_OPC_SV_CALL_*
@ingroup clrpc-opcodes-server
@{
@brief CLRPC_OPC_SV_CALL_* specific opcodes.
@details
Using the functionalities PROVIDE.REQ, CALL.IND and RESPONSE.REQ, a "local procedure call" is executed on the server side (which is "remote" from the client's view).

All 3 LSA OpCodes use the same RQB layout and serve the same purpose.

If the channel is closed, all RQBs that were "provided" will be returned with the response code "cancelled".

@see
- @ref CLRPC_OPC_SV_CALL_IND_PROVIDE
- @ref CLRPC_OPC_SV_CALL_INDICATION
- @ref CLRPC_OPC_SV_CALL_RESPONSE
******************************************************************************/
#define CLRPC_PNIO_STATUS_INVALID  0xFFFFFFFF /**< PNIO status invalid */

/** The type used for CLRPC_OPC_SV_CALL_*. @note If an opcode is not quoted in the description of a data field, the opcode does not use this data field. */
typedef struct
{
	LSA_UINT16					server_id;
	/**<
	 * ## CLRPC_OPC_SV_CALL_IND_PROVIDE: REQ
	 * The ID of the server instance.
	 * Return of CLRPC_OPC_SV_REGISTER_IF_OBJ.
	 *
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * The ID of the server instance.
	 */

	LSA_VOID_PTR_TYPE			scall_id;
	/**<
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * ID of the server call instance. Refer CLRPC_OPC_SV_CALL_RESPONSE.
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Unchanged date of CALL_INDICATION.
	 */

	LSA_VOID_PTR_TYPE			usr_id;
	/**< ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * The 'usr_id', that was given during registration.
	 */

	LSA_UINT32					host_ip;
	/**< network byte order! see CLRPC_NTOHL()
	 *
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * IP address of client (if the server wants to recall the client).
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Unchanged date of CALL_INDICATION or 0.
	 */

	LSA_UINT16					udp_port;
	/**< network byte order! see CLRPC_NTOHS()
	 *
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * UDP port number of client (only serves for information).
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Unchanged date of CALL_INDICATION or 0.
	 */

	LSA_UINT16					opnum;
	/**< ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * The function to be carried out.
	 */

	LSA_UINT16					semantics;
	/**<
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * Refer to clrpc_semantics_enum in module @ref clrpc-opcodes-server-call \n
	 * AP00969270: not possible at the moment are:
	 * - MAYBE
	 * - BROADCAST
	 * - BROADCAST_MAYBE
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Unchanged date of CALL_INDICATION.
	 */

	CLRPC_DREP_TYPE				drep;
	/**< ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * Data representation of IN arguments in CLRPC_SV_CALL_TYPE::ndr_data.
	 * For possible values refer to @ref clrpc-opcodes-server-call.
	 */

	CLRPC_UUID_TYPE				object_id;
	/**< ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * The object ID of the object to be used.
	 */

	LSA_UINT32					alloc_len;
	/**<
	 * ## CLRPC_OPC_SV_CALL_IND_PROVIDE: REQ
	 * 0
	 *
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * Refer @ref CLRPC_OPC_SV_REGISTER_IF_OBJ.
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Unchanged date of CALL_INDICATION.
	 */

	LSA_UINT32					ndr_len;
	/**<
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * The size of the received data.
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * The size of RSP arguments. Must be less than or equal to @ref CLRPC_SV_CALL_TYPE::alloc_len.
	 */

	CLRPC_UPPER_MEM_U8_PTR_TYPE	ndr_data;
	/**< @note buffer is at least 4 byte aligned
	 *
	 * ## CLRPC_OPC_SV_CALL_IND_PROVIDE: REQ
	 * LSA_NULL
	 *
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * Pointer to a storage area of the size @ref CLRPC_SV_CALL_TYPE::alloc_len, in which the first @ref CLRPC_SV_CALL_TYPE::ndr_len Bytes contain the IND arguments.
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * Pointer to the storage location of the variable 'alloc_len', in which the first 'len' bytes are populated with the RSP arguments.
	 */

	/* if interface registered with "PN IO signature" */
	LSA_UINT32					pnio_status;
	/**<
	 * ## CLRPC_OPC_SV_CALL_INDICATION: IND
	 * Don't Care. Contains CLRPC_PNIO_STATUS_INVALID. Refer @ref CLRPC_OPC_SV_CALL_RESPONSE.
	 *
	 * ## CLRPC_OPC_SV_CALL_RESPONSE: RSP
	 * The user does coding. \n A value is predefined: @ref CLRPC_PNIO_STATUS_INVALID.
	 */

} CLRPC_SV_CALL_TYPE;

typedef CLRPC_SV_CALL_TYPE * CLRPC_UPPER_SV_CALL_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_SV_CALL_*. */
/**@}*************************************************************************/


/**************************************************************************//**
@defgroup CLRPC_OPC_SV_CALL_IND_PROVIDE 0x24 CLRPC_OPC_SV_CALL_IND_PROVIDE
@ingroup clrpc-opcodes-server-call
@details
@see @ref clrpc-opcodes-server-call

## RQB

Parameter name          | R/C     | Description
------------------------|---------|------------------------------------
rqb.opcode              | REQ     | CLRPC_OPC_SV_CALL_INDICATION
rqb.handle              | REQ     | Channel Handle
rqb.response            | CNF     | Result of the request.
|                       |         | CLRPC_OK
|                       |         | CLRPC_OK_CANCELLED
|                       |         | CLRPC_ERR_OPCODE \n No server configured. (refer [CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                       |         | CLRPC_ERR_SYS_PATH \n Opcode for this channel path_type not specified.
|                       |         | CLRPC_ERR_PARAM
|                       |         | CLRPC_ERR_SEQUENCE
rqb.args.sv.call        | REQ/CNF | refer to @ref CLRPC_SV_CALL_TYPE
******************************************************************************/

/**************************************************************************//**
@defgroup CLRPC_OPC_SV_CALL_INDICATION 0x25 CLRPC_OPC_SV_CALL_INDICATION
@ingroup clrpc-opcodes-server-call
@brief
@details
@see @ref clrpc-opcodes-server-call

Behavior for new incoming call: \n
In case of a new call, if the response of the 'old call' is still at the CLRPC user (not yet responded with @ref CLRPC_OPC_SV_CALL_RESPONSE),
the new call is buffered. After response the old call will be discarded and the new call with @ref CLRPC_OPC_SV_CALL_INDICATION will be transmitted to the user.

## RQB

Parameter name          | I/R    | Description
------------------------|--------|------------------------------------
rqb.opcode              | IND    | CLRPC_OPC_SV_CALL_INDICATION
rqb.handle              | IND    | Channel Handle
rqb.response            | IND    | CLRPC_OK
rqb.args.sv.call        | IND    | refer to @ref CLRPC_SV_CALL_TYPE
******************************************************************************/

/**************************************************************************//**
@defgroup CLRPC_OPC_SV_CALL_RESPONSE 0x26 CLRPC_OPC_SV_CALL_RESPONSE
@ingroup clrpc-opcodes-server-call
@brief
@details
@see @ref clrpc-opcodes-server-call

If the value @ref CLRPC_ERR_REJECTED is given in the response, instead of a response,
an RPC fault will be sent to the client with the error code CLRPC_NCA_FAULT_UNSPECIFIC (= nca_s_unspec_reject).

## RQB

Parameter name          | I/R    | Description
------------------------|--------|------------------------------------
rqb.opcode              | RSP    | CLRPC_OPC_SV_CALL_RESPONSE
rqb.handle              | RSP    | Channel Handle
rqb.response            | RSP    | CLRPC_OK
|                       |        | CLRPC_ERR_REJECTED \n A fault packet with NCA status CLRPC_NCA_FAULT_UNSPECIFIC is sent to the client.
|                       |        | CLRPC_ERR_FAULTED \n A fault packet with NCA status CLRPC_NCA_FAULT_UNSPECIFIC is sent to the client.
rqb.args.sv.call        | RSP    | refer to @ref CLRPC_SV_CALL_TYPE
******************************************************************************/


/**************************************************************************//**
@defgroup clrpc-opcodes-epm 0x30-0x31 EPM Opcodes
@ingroup clrpc-opcodes
@brief CLRPC EPM specific opcodes.
******************************************************************************/

/*------------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_EPM_START
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_EPM_START 0x30 CLRPC_OPC_EPM_START
@ingroup clrpc-opcodes-epm
@{
@brief Starts the Endpoint Mapper.
@details
This Opcode is allowed in both channel types. The EPM start is always executed in channel type @ref CLRPC_PATH_TYPE_SYSTEM.

The Opcode can be called as often as required. The number of calls is monitored by a Ref-counter. Attention: Only the parameters of the first call take effect!

Intended usage: only by the LSA-component CM (PDEV).

## Implementation detail:

Procedure for the first start:
1. Registers the Endpoint mapper Interface. \n
   Also refer @ref CLRPC_OPC_SV_REGISTER_IF_OBJ.
2. Allocates 'recv_resource_count' receive resources and makes them available to the lower layer.
3. Allocates 'recv_resource_count' indication resources to be able to receive EPM queries, also refer @ref CLRPC_OPC_SV_CALL_IND_PROVIDE.
4. Allocates 'send_resource_count' send resources for response to EPM queries.
5. epm-ref-counter := 1

Procedure for nth start:
1. epm-ref-counter := +1

EPM Interface ID: uuid(e1af8308-5d1f-11c9-91a4-08002b14a0fa), version(3.0)

Ad 'CLRPC_OPC_SV_REGISTER_IF_OBJ \::alloc_len', refer \#define CLRPC_EPM_ALLOC_LEN 216


## RQB

Parameter name      | R/C     | Description
--------------------|---------|------------------------------------
rqb.opcode          | REQ     | CLRPC_OPC_EPM_START
rqb.handle          | REQ     | Channel Handle
rqb.response        | CNF     | Result of the request.
|                   |         | CLRPC_OK
|                   |         | CLRPC_ERR_OPCODE \n No server configured. (refer [CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                   |         | CLRPC_ERR_PARAM \n Parameter not consistent, originate from the detail pointer
|                   |         | CLRPC_ERR_LOWER_LAYER \n Error from lower layer
|                   |         | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state
|                   |         | CLRPC_ERR_RESOURCE
|                   |         | CLRPC_ERR_ALLOC_LOCAL
|                   |         | CLRPC_ERR_ALLOC_LOWER
|rqb.args.epm.start | REQ/CNF | refer to CLRPC_EPM_START_TYPE
******************************************************************************/
typedef struct
{
	LSA_UINT16		server_id;
	/**< ## CNF
	 * The ID of generated Endpoint mapper server instance if response is ok.
	 */

	LSA_UINT16		udp_port;
	/**< ## CNF
	 * UDP port number (in network byte order) of the endpoint mapper.
	 */

	LSA_UINT16		recv_resource_count;
	/**< ## REQ
	 * Number of RQBs that use the SOCK as input buffer. \n
	 * Must be > 0. \n
	 * Provide recv_count recv resources to sock.
	 */

	LSA_UINT16		send_resource_count;
	/**< ## REQ
	 * Maximum number of RQBs that have been pre-allocated and made available for send orders. \n
	 * Must be > 0. \n
	 * All scalls use send_count common send resources.
	 */

	LSA_UINT8		annotation[CLRPC_MAX_ANNOTATION_SIZE];
	/**< ## REQ
	 * The textual note that is returned for the EPM's interface / Object UUID. See @ref CLRPC_PNIO_ANNOTATION_FORMAT too.
	 */
} CLRPC_EPM_START_TYPE;

typedef CLRPC_EPM_START_TYPE * CLRPC_UPPER_EPM_START_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_EPM_START. */
/**@}*************************************************************************/


/*------------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_EPM_STOP
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup CLRPC_OPC_EPM_STOP 0x31 CLRPC_OPC_EPM_STOP
@ingroup clrpc-opcodes-epm
@{
@brief Stops the Endpoint Mapper.
@details
This Opcode is allowed in both ChannelTypes. The EPM stop is executed in ChannelType @ref CLRPC_PATH_TYPE_SYSTEM.

Decrements the epm-ref-counter if > 1.

For epm-ref-counter == 1, the resources of the Endpoint Mapper Server instance are released and epm-ref-counter is set to 0.

@note Between Register/Unregister/Register of the same interface, the user must wait at least one second. Otherwise no instance modification can be determined via Boot Time.

## RQB

Parameter name      | R/C    | Description
--------------------|--------|------------------------------------
rqb.opcode          | REQ    | CLRPC_OPC_EPM_STOP
rqb.handle          | REQ    | Channel Handle
rqb.response        | CNF    | Result of the request.
|                   |        | CLRPC_OK
|                   |        | CLRPC_ERR_OPCODE \n No servers configured. (refer [CLRPC_CFG_MAX_SERVERS](@ref clrpc-anchor-clrpc-cfg-max-servers))
|                   |        | CLRPC_ERR_SEQUENCE \n No Endpoint mapper started.
|                   |        | CLRP_ERR_PARAM \n server_id does not match the Endpoint Mapper Server_id.
|                   |        | CLRPC_ERR_LOWER_LAYER \n Error from lower layer
|                   |        | CLRPC_ERR_ALLOC_LOWER
|                   |        | CLRPC_ERR_SEQUENCE \n Opcode not allowed in the moment , wrong state
|rqb.args.epm.stop  | REQ    | see below, refer to @ref CLRPC_EPM_STOP_TYPE
|.server_id         | REQ    | The ID of the Endpoint mapper Server instance to be released.
******************************************************************************/
typedef CLRPC_SV_UNREGISTER_IF_OBJ_TYPE CLRPC_EPM_STOP_TYPE;

typedef CLRPC_EPM_STOP_TYPE * CLRPC_UPPER_EPM_STOP_PTR_TYPE; /**< The pointer type used for CLRPC_OPC_EPM_STOP. */
/**@}*************************************************************************/


/*------------------------------------------------------------------------------
// CLRPC_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@defgroup clrpc-group-rqb CLRPC RQBs
@ingroup clrpc-group
@{
For more details please refer to @ref clrpc-architecture-external-interfaces-exported-rqb
******************************************************************************/
#define CLRPC_RQB_SET_ARGS(rqb_,val_)		{(rqb_)->args.void_ptr = (val_);}
#define CLRPC_RQB_GET_ARGS(rqb_)			((rqb_)->args.void_ptr)

typedef struct clrpc_rqb_tag
{
	CLRPC_RQB_HEADER /**< see @ref clrpc-group-cfg-header-trailer */

	union
	{ /* a union of pointers for type-safety */

		union
		{
			CLRPC_UPPER_OPEN_CHANNEL_PTR_TYPE	        open;
			CLRPC_UPPER_CLOSE_CHANNEL_PTR_TYPE	        close;
			CLRPC_UPPER_SET_INTERFACE_PARAM_PTR_TYPE	set_interface_param;
			CLRPC_UPPER_CLEAR_INTERFACE_PARAM_PTR_TYPE	clear_interface_param;
		} channel;

		union
		{
			CLRPC_UPPER_CL_BIND_PTR_TYPE		bind;
			CLRPC_UPPER_CL_UNBIND_PTR_TYPE		unbind;
			CLRPC_UPPER_CL_CALL_PTR_TYPE		call;
			CLRPC_UPPER_CL_CANCEL_PTR_TYPE		cancel;
			CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE	query_epm;
		} cl; /**< don't forget to update 'ANY_ARGS' */

		union
		{
			CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE		reg_if_obj;
			CLRPC_UPPER_SV_UNREGISTER_IF_OBJ_PTR_TYPE	unreg_if_obj;
			CLRPC_UPPER_SV_CALL_PTR_TYPE				call;
		} sv; /**< don't forget to update 'ANY_ARGS' */

		union
		{
			CLRPC_UPPER_EPM_START_PTR_TYPE	start;
			CLRPC_UPPER_EPM_STOP_PTR_TYPE	stop;
		} epm; /**< don't forget to update 'ANY_ARGS' */

		LSA_VOID * void_ptr; /**< for set/get macro */

	} args; /**< var: arguments corresponding to the opcode */

	CLRPC_RQB_TRAILER /**< see @ref clrpc-group-cfg-header-trailer */
} CLRPC_RQB_TYPE;
/**@}*************************************************************************/


/*------------------------------------------------------------------------------
// UNION of all types
//----------------------------------------------------------------------------*/

typedef union clrpc_any_args_tag
{ /* may be useful: one for all... */

	union
	{
		CLRPC_OPEN_CHANNEL_TYPE			open;
		/* close: no parameters */
		CLRPC_SET_INTERFACE_PARAM_TYPE	set_interface_param;
		/* clear_interface_param: no parameters */
	} channel;

	union
	{
		CLRPC_CL_BIND_TYPE				bind;
		CLRPC_CL_UNBIND_TYPE			unbind;
		CLRPC_CL_CALL_TYPE				call;
		CLRPC_CL_CANCEL_TYPE			cancel;
		CLRPC_CL_QUERY_EPM_TYPE			query_epm;
	} cl;

	union
	{
		CLRPC_SV_REGISTER_IF_OBJ_TYPE	reg_if_obj;
		CLRPC_SV_UNREGISTER_IF_OBJ_TYPE	unreg_if_obj;
		CLRPC_SV_CALL_TYPE				call;
	} sv;

	union
	{
		CLRPC_EPM_START_TYPE			start;
		CLRPC_EPM_STOP_TYPE				stop;
	} epm;

} CLRPC_ANY_ARGS_TYPE;

typedef CLRPC_ANY_ARGS_TYPE * CLRPC_UPPER_ANY_ARGS_PTR_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Opens a new communication channel for the component CLRPC.
@details
Asynchronous. As per LSA draft.

Generates and initializes a new CLRPC instance.

A callback function is to be added in the request block. All finished requests for this channel are responded via this callback function.

CLRPC does not call the callback function directly but calls the output macro @ref CLRPC_REQUEST_UPPER_DONE().

Calls the output function @ref CLRPC_GET_PATH_INFO to get information of the configuration of the channel.

If the RQB cannot be analyzed, the output function CLRPC_UPPER_RQB_ERROR() is called (e.g.: NULL pointer).
If an error situation cannot be reported to the upper layer, the output function @ref CLRPC_FATAL_ERROR() is called.

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@see CLRPC_OPC_OPEN_CHANNEL

@param upper_rqb_ptr Pointer to the request block
******************************************************************************/
LSA_VOID clrpc_open_channel(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Close a communication channel.
@details
Asynchronous. As per LSA draft.

Releases the local memory requested with clrpc_open_channel().

Before closing, all the request blocks that were prepared for indications are responded with the response code @ref CLRPC_OK_CANCELLED by the server,
refer Opcode: @ref CLRPC_OPC_SV_CALL_IND_PROVIDE

Gives an error, if all clients and server registers have not been withdrawn.

If the RQB could not be analyzed, the output function CLRPC_UPPER_RQB_ERROR() is called (e.g.: NULL Pointer).
If an error situation is not reported to the upper layer, the output function @ref CLRPC_FATAL_ERROR() is called.

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@see CLRPC_OPC_CLOSE_CHANNEL

@param upper_rqb_ptr Pointer to the request block
******************************************************************************/
LSA_VOID clrpc_close_channel(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Processing of a request for CLRPC client or  server.
@details
Asynchronous. As per LSA draft.

The construction of the RQBs and the opcodes are explained in chapter @ref clrpc-architecture-external-interfaces-exported

Receives a request block from the higher level for processing.

Forwards the RQB to the client or server for processing based on the entered opcodes.

Client or server enters the result in the request block. The response is sent asynchronous using the callback function,
made known to clrpc_open_channel() to the caller.

If the RQB could not be analyzed, the output function CLRPC_UPPER_RQB_ERROR() is called (e.g.: NULL Pointer).
If an error situation is not reported to the upper layer, the output function CLRPC_FATAL_ERROR() is called.

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

## Example
@snippet "code examples/clrpc_opc_register_example.c" clrpc-opc-register-snippet

@param upper_rqb_ptr Pointer to the request block
******************************************************************************/
LSA_VOID clrpc_request(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  clrpc_upper_rqb
 *
 * function:       asynchronous allocation of an upper-RQB
 *
 * documentation:  none, not used
 *
 *===========================================================================*/
LSA_VOID clrpc_upper_rqb(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT16                length,
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);



/*=============================================================================
 * function name:  clrpc_upper_mem
 *
 * function:       asynchronous allocation of an upper-memory
 *
 * documentation:  none, not used
 *
 *===========================================================================*/
LSA_VOID clrpc_upper_mem(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT32                length,
	CLRPC_UPPER_MEM_PTR_TYPE  upper_mem_ptr
);


/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Request for a request block with the storage attributes of the upper layer.
@details
Synchronous. As per LSA draft.

Alignment based on processor requirements. E.g. 32-Bit for TriCore.

The storage must be accessible by the upper interface.

@param [out] upper_rqb_ptr_ptr  pointer to upper RQB
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CLRPC_ALLOC_UPPER_RQB
LSA_VOID CLRPC_ALLOC_UPPER_RQB(
	CLRPC_UPPER_RQB_PTR_TYPE * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT16                 length,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of a request block, requested with CLRPC_ALLOC_UPPER_RQB.
@details
Synchronous. As per LSA draft.

@param [out] ret_val_ptr   return value
@param [in]  upper_rqb_ptr upper RQB pointer
@param [in]  sys_ptr       system pointer
******************************************************************************/
#ifndef CLRPC_FREE_UPPER_RQB
LSA_VOID CLRPC_FREE_UPPER_RQB(
	LSA_UINT16               *  ret_val_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE    upper_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Request for storage with the storage attributes of the upper layer.
@details
Synchronous. As per LSA draft.

Alignment based on processor requirements. E.g. 32-Bit for TriCore.

The storage must be accessible from the upper interface.

@param [out] upper_mem_ptr_ptr  pointer to upper memory pointer
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CLRPC_ALLOC_UPPER_MEM
LSA_VOID CLRPC_ALLOC_UPPER_MEM(
	CLRPC_UPPER_MEM_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT32                 length,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of storage, requested with @ref CLRPC_ALLOC_UPPER_MEM.
@details
Synchronous. As per LSA draft.

@param [out] ret_val_ptr    return value
@param [in]  upper_mem_ptr  upper memory pointer
@param [in]  sys_ptr        system pointer
******************************************************************************/
#ifndef CLRPC_FREE_UPPER_MEM
LSA_VOID CLRPC_FREE_UPPER_MEM(
	LSA_UINT16               * ret_val_ptr,
	CLRPC_UPPER_MEM_PTR_TYPE   upper_mem_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief "Sending" of a RQB to itself (CLRPC).
@details
The LSA timer calls clrpc_timeout(), which sends a (static) RQB to itself using this function.

This ensures that the timer derived in this manner runs in task context.

@param [in]  upper_rqb_ptr upper rqb pointer
******************************************************************************/
#ifndef CLRPC_REQUEST_LOCAL
LSA_VOID CLRPC_REQUEST_LOCAL(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_USR_H */
