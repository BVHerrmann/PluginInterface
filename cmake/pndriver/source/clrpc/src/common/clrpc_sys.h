#ifndef CLRPC_SYS_H
#define CLRPC_SYS_H

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
/*  F i l e               &F: clrpc_sys.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct clrpc_detail_tag    *CLRPC_DETAIL_PTR_TYPE;      /* pointer to CLRPC_DETAIL    */

typedef struct lsa_fatal_error_tag *CLRPC_FATAL_ERROR_PTR_TYPE; /* pointer to LSA_FATAL_ERROR */

typedef LSA_VOID * CLRPC_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID * CLRPC_LOCAL_MEM_PTR_TYPE; /* pointer to local-memory */

/*------------------------------------------------------------------------------
// clrpc details
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@addtogroup clrpc-group-output-functions
@brief The Details pointer is used to configure a CLRPC channel. See CLRPC_GET_PATH_INFO().
@{
******************************************************************************/

#define CLRPC_PATH_TYPE_SYSTEM		1 /**< system path type */
#define CLRPC_PATH_TYPE_USER		2 /**< user path type */

typedef struct clrpc_detail_tag
{

	LSA_UINT16  path_type; /**< see CLRPC_PATH_TYPE_xxx */

	/* note: the parameter ep_mapper_port was removed with V4.0i80.2 (AP00619205), value is now always CLRPC_EPM_DEFAULT_PORT */

	union
	{
#if CLRPC_CFG_MAX_CLIENTS
		struct
		{ /* client details */
			LSA_UINT8  nothing_so_far; /* details were deleted, AP01309324 */
			/* values send_data_size, rtxTimeout and rtxMax were deleted,
				used values are CLRPC_MUST_RECV_BODY_SIZE, CLRPC_RTX_TIMEOUT_MSEC and CLRPC_MAX_RTX_RETRY */
		} cl;
#endif

#if CLRPC_CFG_MAX_SERVERS /* EPM is a special server */
		struct
		{ /* endpoint-mapper details */

			LSA_UINT8  nothing_so_far; /* details were moved to CM-PD with V4.1i3 (AP00706078) */

		} epm;
#endif
	} u; /**< details were deleted or moved, see clrpc_sys.h */

} CLRPC_DETAIL_TYPE; /**< Type used for CLRPC_GET_PATH_INFO */
/**@}*************************************************************************/


/*------------------------------------------------------------------------------
// clrpc init-parameters
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@addtogroup clrpc-group-input-functions
@{
******************************************************************************/
typedef struct clrpc_init_tag {

	LSA_UINT16 oneshot_lsa_time_base;
	/**<
	 * LSA_TIME_BASE_XXX \n
	 * Determines the resolution of timeouts along with _time_factor. \n
	 * Must be at least 2 (better 3) times faster than the shortest timeout (1 second as per PNIO). \n
	 * Allowed values:
	 * - LSA_TIME_BASE_100MS .... Recommended
	 * - LSA_TIME_BASE_10MS
	 * - LSA_TIME_BASE_1MS ...... Not meaningful
	 */

	LSA_UINT16 oneshot_lsa_time_factor;
	/**<
	 * Refer _time_base.
	 */

} CLRPC_INIT_TYPE, * CLRPC_INIT_PTR_TYPE;

typedef CLRPC_INIT_TYPE const * CLRPC_INIT_CONST_PTR_TYPE;
/**@}***********************************************************************/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/



/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Initialization of CLRPC.
@details
Must be called before calling the first open channel.

Synchronous. As per LSA draft.

Initialization of the module.

Initialization of the channel table and the subcomponents of client or server.

If (base * factor) is greater than 500 milliseconds, CLRPC_FATAL() is called. The standardized RPC timeouts cannot be accurately mapped with this setting.

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@param [in] init refer to @ref CLRPC_INIT_TYPE
@return          LSA_RET_OK
******************************************************************************/
LSA_UINT16 clrpc_init(
	CLRPC_INIT_CONST_PTR_TYPE init
);

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief "Cleanup" of the component CLRPC.
@details
Releases the storage allocated in clrpc_init().

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@return          LSA_RET_OK
******************************************************************************/
LSA_UINT16 clrpc_undo_init(
    LSA_VOID
);

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Callback function for the LSA timer.
@details
CLRPC derives other internal timers from the LSA timer.

Synchronizes the callback with the CLRPC task using the "timer mail" to itself.

Refer output function @ref CLRPC_REQUEST_LOCAL

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@param [in] timer_id timer id
@param [in] user_id  user id
******************************************************************************/
LSA_VOID clrpc_timeout(
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id
);


/*=============================================================================
 * function name:  clrpc_get_sock_handle
 *
 * function:       undocumented, for testing only
 *===========================================================================*/
LSA_HANDLE_TYPE
clrpc_get_sock_handle (
    LSA_HANDLE_TYPE  clrpc_handle
);


/*====  out functions  =====*/

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Get system-pointer and detail-pointer of a communication channel
@details
Synchronous. As per LSA draft.

The detail pointer is used to configure a CLRPC instance.

The system pointer for CLRPC is "opaque" and is used by the functions of the system connection.

@note if this function returns a "ret_val" other than LSA_RET_OK, then CLRPC throws a fatal. This is a precondition for CLRPC_REQUEST_UPPER_DONE.

@note see also the picture in @ref clrpc-architecture-constraints-channel-structure

@see @ref CLRPC_DETAIL_TYPE

@param [out] ret_val_ptr    return value
@param [out] sys_ptr_ptr    system pointer
@param [out] detail_ptr_ptr detail pointer
@param [in]  path           LSA path
******************************************************************************/
#ifndef CLRPC_GET_PATH_INFO
LSA_VOID CLRPC_GET_PATH_INFO(
    LSA_UINT16            *  ret_val_ptr,
    LSA_SYS_PTR_TYPE      *  sys_ptr_ptr,
    CLRPC_DETAIL_PTR_TYPE *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE        path
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release system-pointer and detail-pointer of a communication channel.
@details
Synchronous. As per LSA draft.

Release of the information, requested with @ref CLRPC_GET_PATH_INFO.

@param [out] ret_val_ptr return value
@param [in]  sys_ptr     system pointer
@param [in]  detail_ptr  detail pointer
******************************************************************************/
#ifndef CLRPC_RELEASE_PATH_INFO
LSA_VOID CLRPC_RELEASE_PATH_INFO(
    LSA_UINT16             * ret_val_ptr,
    LSA_SYS_PTR_TYPE         sys_ptr,
    CLRPC_DETAIL_PTR_TYPE    detail_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Request for storage that is internally required by CLRPC.
@details
Synchronous. As per LSA draft.

The storage must be accessible internally in CLRPC.

@param [out] local_mem_ptr_ptr pointer to requested memory
@param [in]  length            alloc length
******************************************************************************/
#ifndef CLRPC_ALLOC_LOCAL_MEM
LSA_VOID CLRPC_ALLOC_LOCAL_MEM(
    CLRPC_LOCAL_MEM_PTR_TYPE * local_mem_ptr_ptr,
    LSA_UINT32                 length
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of storage requested with @ref CLRPC_ALLOC_LOCAL_MEM.
@details
Synchronous. As per LSA draft.

@param [out] ret_val_ptr   return value
@param [in]  local_mem_ptr memory to be freed
******************************************************************************/
#ifndef CLRPC_FREE_LOCAL_MEM
LSA_VOID CLRPC_FREE_LOCAL_MEM(
    LSA_UINT16               *  ret_val_ptr,
    CLRPC_LOCAL_MEM_PTR_TYPE    local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_LOWER_TO_UPPER_MEM
LSA_VOID CLRPC_LOWER_TO_UPPER_MEM(
    CLRPC_UPPER_MEM_PTR_TYPE * upper_mem_ptr_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_UPPER_TO_LOWER_MEM
LSA_VOID CLRPC_UPPER_TO_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    CLRPC_UPPER_MEM_PTR_TYPE   upper_mem_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower-memory to lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_LOWER_TO_LOWER_MEM
LSA_VOID CLRPC_LOWER_TO_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE           src_sys_ptr,
    LSA_SYS_PTR_TYPE           dst_sys_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  CLRPC_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * NOTE: not used
 *
 *===========================================================================*/
#if 0 /* not used */
#ifndef CLRPC_COPY_COMMON_MEM
LSA_VOID CLRPC_COPY_COMMON_MEM(
    LSA_VOID * src_common_mem_ptr,
    LSA_VOID * dst_common_mem_ptr,
    LSA_UINT16 length
);
#endif
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Allocate a timer
@details
Request for an LSA Timer.

Synchronous. As per LSA draft.

@param [out] ret_val_ptr   return value
@param [out] timer_id_ptr  pointer to id of timer
@param [in]  timer_type    LSA timer type
@param [in]  time_base     LSA timer base
******************************************************************************/
#ifndef CLRPC_ALLOC_TIMER
LSA_VOID CLRPC_ALLOC_TIMER(
    LSA_UINT16        * ret_val_ptr,
    LSA_TIMER_ID_TYPE * timer_id_ptr,
    LSA_UINT16          timer_type,
    LSA_UINT16          time_base
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Start of LSA Timer.
@details
Synchronous. As per LSA draft.

At the end of the timeframe, the input function clrpc_timeout() is called.

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
@param [in]  user_id     user id
@param [in]  time        LSA timer base
******************************************************************************/
#ifndef CLRPC_START_TIMER
LSA_VOID CLRPC_START_TIMER(
    LSA_UINT16        * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          time
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Stopping the LSA timer.
@details
Synchronous. As per LSA draft.

An LSA timer can be triggered once again after “stop” (e.g. if already in a mailbox, etc).

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
******************************************************************************/
#ifndef CLRPC_STOP_TIMER
LSA_VOID CLRPC_STOP_TIMER(
    LSA_UINT16      * ret_val_ptr,
    LSA_TIMER_ID_TYPE timer_id
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of an LSA timer requested with @ref CLRPC_ALLOC_TIMER.
@details
Synchronous. As per LSA draft.

An LSA timer can be triggered once again after "stop" (e.g. if already in a mailbox, etc).

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
******************************************************************************/
#ifndef CLRPC_FREE_TIMER
LSA_VOID CLRPC_FREE_TIMER(
    LSA_UINT16        * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id
);
#endif

/**************************************************************************//**
@defgroup clrpc-group-output-functions-clrpc-enter CLRPC_ENTER
@ingroup clrpc-group-output-functions
@brief Set reentrance lock
@details
Synchronous. As per LSA draft.

Entry in an area protected from interrupts.
@note not used
@see @ref clrpc-architecture-constraints-connection-enter-exit
******************************************************************************/
#if 0
#ifndef CLRPC_ENTER
LSA_VOID CLRPC_ENTER(
    LSA_VOID
);
#endif
#endif

/**************************************************************************//**
@defgroup clrpc-group-output-functions-clrpc-exit CLRPC_EXIT
@ingroup clrpc-group-output-functions
@brief Cancel reentrance lock
@details
Synchronous. As per LSA draft.

Leaving an area protected from interrupts.
@note not used
@see @ref clrpc-architecture-constraints-connection-enter-exit
******************************************************************************/
#if 0
#ifndef CLRPC_EXIT
LSA_VOID CLRPC_EXIT(
    LSA_VOID
);
#endif
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Reporting a fatal error.
@details
Synchronous. As per LSA draft.

No return from this function.

## LSA_FATAL_ERROR_TYPE

@code
typedef struct lsa_fatal_error_tag
{
	LSA_UINT16         lsa_component_id;    // LSA_COMP_ID_
	LSA_UINT16         module_id;           // *_MODULE_ID
	LSA_UINT16         line;                // line of file
	LSA_UINT16         error_data_length;   // length in bytes of the
											// memory area addressed by
											// error_data_ptr
	LSA_UINT32         error_code[4];
	LSA_VOID_PTR_TYPE  error_data_ptr;
} LSA_FATAL_ERROR_TYPE;
@endcode

@param [in] length     length
@param [in] error_ptr  error pointer
******************************************************************************/
#ifndef CLRPC_FATAL_ERROR
LSA_VOID CLRPC_FATAL_ERROR(
    LSA_UINT16                  length,
    CLRPC_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/*=============================================================================
 * function name:  clrpc_fatal1
 *
 * function:       undocumented, not in clrpc_int.h because of clrpc_cfg.c
 *===========================================================================*/
LSA_VOID
clrpc_fatal1(
	LSA_UINT16 clrpc_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

/*lint -function(exit, clrpc_fatal1) */
#ifndef CLRPC_FATAL1 /* AP00289268, note: returning from fatal is not allowed */
#define CLRPC_FATAL1(ec_0)  clrpc_fatal1 (CLRPC_MODULE_ID, __LINE__, ec_0) /*lint -unreachable */
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Returns a finished request to the upper layer.
@details
Synchronous. As per LSA draft.

The request is handled. The RQB has to be sent back to the originator.

This macro is called by the CLRPC if a user channel request (RQB) from calling clrpc_open_channel(), clrpc_request() or clrpc_close_channel() is finished.
The CLRPC does not directly call the callback function provided by clrpc_open_channel(), but calls this output macro instead.

The system adaption has to call the upper callback function to finish the request.

@param [in] clrpc_request_upper_done_ptr pointer to clrpc_request_upper_done_ptr, given by clrpc_open_channel()
@param [in] upper_rqb_ptr                pointer to upper-RQB
@param [in] sys_ptr                      system pointer
******************************************************************************/
#ifndef CLRPC_REQUEST_UPPER_DONE
LSA_VOID CLRPC_REQUEST_UPPER_DONE(
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE  clrpc_request_upper_done_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                   sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Get number of 100 ns ticks from system adaptation.
@details
Synchronous.

This counter/timer is used for several things. CLRPC uses it instead of a normal Timer like a UNIX-Timer for UUIDs
and for the boot time of a server and instead of a random value to initialize the clocksequence of the UUIDs.
This counter/timer starts with 0 and should be a quick hardware timer to count the number of 100ns ticks after power on.

The time in a uuid or the server boot time cannot be interpreted as normal time because this counter does not have any reference to a set watch.

DCE RPC only examines the value for being identical or different.

@param [out] ret_val_ptr       pointer for return value
                               - LSA_RET_OK no error
                               - LSA_RET_RESOURCE counter could not be read
@param [out] counter_value_ptr pointer for value of 100ns-counter
******************************************************************************/
#ifndef CLRPC_GET_100NS_COUNTER
LSA_VOID CLRPC_GET_100NS_COUNTER(
    LSA_UINT16 * ret_val_ptr,
    LSA_UINT64 * counter_value_ptr
);
#endif


/*=============================================================================
 * function name:  CLRPC_PUT8,16,32,UUID and CLRPC_GET8,16,32,UUID
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifdef  CLRPC_PUT8_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT8_ALIGNED(ptr, val) { \
	*(LSA_UINT8 *)(ptr) = (LSA_UINT8)(val); \
	}
#endif

#ifdef  CLRPC_GET8_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET8_ALIGNED(ptr) ( \
	*(LSA_UINT8 *)(ptr) \
	)
#endif

#ifdef  CLRPC_PUT16_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT16_ALIGNED(ptr, val) { \
	*(LSA_UINT16 *)(CLRPC_TEST_ALIGN2(ptr)) = (LSA_UINT16)(val); \
	}
#endif

#ifdef  CLRPC_GET16_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET16_ALIGNED(ptr) ( \
	*(LSA_UINT16 *)(CLRPC_TEST_ALIGN2(ptr)) \
	)
#endif

#ifdef  CLRPC_PUT32_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT32_ALIGNED(ptr, val) { \
	*(LSA_UINT32 *)(CLRPC_TEST_ALIGN4(ptr)) = (LSA_UINT32)(val); \
	}
#endif

#ifdef  CLRPC_GET32_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET32_ALIGNED(ptr) ( \
	*(LSA_UINT32 *)(CLRPC_TEST_ALIGN4(ptr)) \
	)
#endif

#ifdef  CLRPC_PUT_UUID_ALIGNED
#error "why defined?"
#else
#define CLRPC_PUT_UUID_ALIGNED(ptr, uuid_ptr) { \
	*(CLRPC_UUID_TYPE *)(CLRPC_TEST_ALIGN4(ptr)) = *(/*no-cast*/uuid_ptr); \
	}
#endif

#ifdef  CLRPC_GET_UUID_ALIGNED
#error "why defined?"
#else
#define CLRPC_GET_UUID_ALIGNED(uuid_ptr, ptr) { \
	*(uuid_ptr) = *(CLRPC_UUID_TYPE *)(CLRPC_TEST_ALIGN4(ptr)); \
	}
#endif

/*=============================================================================
 * function name:  CLRPC_SWAP_U16, CLRPC_SWAP_U32 and CLRPC_SWAP_UUID
 *
 * function:       swap "in place"
 *                 between "little" and "big" endian or vice versa
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

/**************************************************************************//**
@defgroup clrpc-group-output-functions-swap CLRPC_SWAP_U16 / _U32 / _UUID
@ingroup clrpc-group-output-functions
@brief "In Place" conversion between "MSB first" and "LSA first" and vice versa.
@details
@note These functions on an MSB-first machine are not of same significance as the CLRPC_HTONS/L macros.

These functions are mapped in the system connection to the corresponding CPU optimized LSA macros.

Prototype refer file.../cfg/clrpc_lib.h

Macros refer file.../cfg/clrpc_cfg.h

Examples refer file.../cfg/clrpc_cfg_lib.c (which is not official part of CLRPC!)
******************************************************************************/

/**************************************************************************//**
@ingroup clrpc-group-output-functions-swap
@brief Swap "in place" between "little" and "big" endian or vice versa, short integer

@param [out] short_ptr pointer to a short integer
******************************************************************************/
#ifndef CLRPC_SWAP_U16
LSA_VOID
CLRPC_SWAP_U16(
	LSA_UINT16 * short_ptr /* correct alignment! */
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-swap
@brief Swap "in place" between "little" and "big" endian or vice versa, long integer

@param [inout] long_ptr pointer to a long integer
******************************************************************************/
#ifndef CLRPC_SWAP_U32
LSA_VOID
CLRPC_SWAP_U32(
	LSA_UINT32 * long_ptr /* correct alignment! */
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-swap
@brief Swap "in place" between "little" and "big" endian or vice versa, UUID

@param [inout] uuid_ptr pointer to an UUID
******************************************************************************/
#ifndef CLRPC_SWAP_UUID
LSA_VOID
CLRPC_SWAP_UUID(
	CLRPC_UUID_PTR_TYPE uuid_ptr /* correct alignment! */
);
#endif


/**************************************************************************//**
@defgroup clrpc-group-output-functions-byte-order CLRPC_HTONS / _HTONL / _NTOHS / _NTOHL
@ingroup clrpc-group-output-functions
@brief Converting between "network byte order" and "host byte order"
@details
These functions are mapped in the system connection onto the corresponding CPU optimized LSA macros.

@note on an MSB first machine, these macros are left empty.

Prototypes refer file .../cfg/clrpc_lib.h

Macros refer file.../cfg/clrpc_cfg.h

Example refers file.../cfg/clrpc_cfg_lib.c (which is not official part of CLRPC!)
******************************************************************************/

/**************************************************************************//**
@ingroup clrpc-group-output-functions-byte-order
@brief Converts host to network byte order, short integer

@param [in] host_short host value
@return                converted value
******************************************************************************/
#ifndef CLRPC_HTONS
LSA_UINT16
CLRPC_HTONS(
	LSA_UINT16 host_short
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-byte-order
@brief Converts host to network byte order, long integer

@param [in] host_long host value
@return               converted value
******************************************************************************/
#ifndef CLRPC_HTONL
LSA_UINT32
CLRPC_HTONL(
	LSA_UINT32 host_long
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-byte-order
@brief Converts network to host byte order, short integer

@param [in] network_short network value
@return                   converted value
******************************************************************************/
#ifndef CLRPC_NTOHS
LSA_UINT16
CLRPC_NTOHS(
	LSA_UINT16 network_short
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-byte-order
@brief Converts network to host byte order, long integer

@param [in] network_long network value
@return                  converted value
******************************************************************************/
#ifndef CLRPC_NTOHL
LSA_UINT32
CLRPC_NTOHL(
	LSA_UINT32 network_long
);
#endif


/**************************************************************************//**
@defgroup clrpc-group-output-functions-access-misaligned-data CLRPC_ PUT/GET 16/UUID _LITTLE
@ingroup clrpc-group-output-functions
@brief Access functions to "misaligned" data.
@details
Macros for memory access without alignment requirements.
Important: WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

These functions are mapped to the corresponding CPU optimized LSA macros in the system connection.

Prototypes refer file.../cfg/clrpc_lib.h

Macros refer file.../cfg/clrpc_cfg.h

Examples refer file.../cfg/clrpc_cfg_lib.c (which is not official part of CLRPC!)
******************************************************************************/

/**************************************************************************//**
@ingroup clrpc-group-output-functions-access-misaligned-data
@brief Macro for memory access: put, little endian byte order

@param [out] ptr pointer for memory access
@param [in]  val value
******************************************************************************/
#ifndef CLRPC_PUT16_LITTLE
LSA_VOID
CLRPC_PUT16_LITTLE(
	LSA_VOID_PTR_TYPE ptr,
	LSA_UINT16 val
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-access-misaligned-data
@brief Macro for memory access: get, little endian byte order

@param [in] ptr pointer for memory access
@return         converted value
******************************************************************************/
#ifndef CLRPC_GET16_LITTLE
LSA_UINT16
CLRPC_GET16_LITTLE(
	LSA_VOID_PTR_TYPE ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-access-misaligned-data
@brief Macro for memory access: put, convert into little endian byte order

@param [out] ptr      pointer
@param [in]  uuid_ptr uuid pointer, correct alignment
******************************************************************************/
#ifndef CLRPC_PUT_UUID_LITTLE
LSA_VOID
CLRPC_PUT_UUID_LITTLE(
	LSA_VOID_PTR_TYPE ptr,
	CLRPC_UUID_PTR_TYPE uuid_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-access-misaligned-data
@brief Macro for memory access: get, convert from little endian byte order

@param [out] uuid_ptr uuid pointer, correct alignment!
@param [in]  ptr pointer for memory access
******************************************************************************/
#ifndef CLRPC_GET_UUID_LITTLE
LSA_VOID
CLRPC_GET_UUID_LITTLE(
	CLRPC_UUID_PTR_TYPE uuid_ptr,
	LSA_VOID_PTR_TYPE ptr
);
#endif

/**************************************************************************//**
@defgroup clrpc-group-output-functions-standard-c-lib "Standard C-Lib"
@ingroup clrpc-group-output-functions
@brief Macros for standard C-lib functions
@details
LSA modules are "self-contained". Even functions of standard C-Lib can only be addressed via macros.

The functionality corresponds to the corresponding C-Lib functions (memset, memcpy, etc.).

Prototypes refer file.../cfg/clrpc_lib.h

Macros refer file.../cfg/clrpc_cfg.h

Examples refer file.../cfg/clrpc_cfg_lib.c (which is not official part of CLRPC!)

Important: WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat"
******************************************************************************/

/**************************************************************************//**
@ingroup clrpc-group-output-functions-standard-c-lib
@brief Corresponds to the 'memset' function of standard C-Lib

@param [out] ptr destination pointer
@param [in]  val value to be set
@param [in]  len number of bytes to be set to the value
******************************************************************************/
#ifndef CLRPC_MEMSET
LSA_VOID
CLRPC_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT val,
	LSA_UINT len
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions-standard-c-lib
@brief Corresponds to the 'memcpy' function of standard C-Lib

@param [out] dst destination pointer
@param [in]  src pointer to source of data
@param [in]  len number of bytes to copy
******************************************************************************/
#ifndef CLRPC_MEMCPY
LSA_VOID
CLRPC_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_SYS_H */
