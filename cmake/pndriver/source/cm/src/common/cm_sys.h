#ifndef CM_SYS_H
#define CM_SYS_H

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
/*  F i l e               &F: cm_sys.h                                  :F&  */
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

/*------------------------------------------------------------------------------
// cm details
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup cm-product-components-output-functions
******************************************************************************/
enum cm_path_type_enum {

	CM_PATH_TYPE_EDD       =  0,  /**< the system path to EDD */

#if CM_CFG_USE_MRP
	CM_PATH_TYPE_MRP       =  1,  /**< the system path to MRP */
#endif

#if CM_CFG_USE_GSY
	CM_PATH_TYPE_GSY       =  2,  /**< the system path to GSY */
#endif

#if CM_CFG_USE_POF
	CM_PATH_TYPE_POF       =  3,  /**< the system path to POF */
#endif

	CM_PATH_TYPE_OHA       =  4,  /**< the system path to OHA */

	CM_PATH_TYPE_5         =  5,  /**< defunct, was system path to REMA */

	CM_PATH_TYPE_NARE      =  6,  /**< the system path to NARE */

	CM_PATH_TYPE_RPC       =  7,  /**< the system path to CLRPC */

	CM_PATH_TYPE_PD        =  8,  /**< a user path to a CM-PD */

	CM_PATH_TYPE_CLIENT    =  9,  /**< a user path to a CM client */
	CM_PATH_TYPE_MULTICAST = 10,  /**< a user path to a MC-channel */
	CM_PATH_TYPE_SERVER    = 11,  /**< a user path to a CM server */

	CM_PATH_TYPE_ANY_MAX          /**< for array-size */
};

#define CM_PATH_TYPE_SYS_MAX  (CM_PATH_TYPE_RPC)  /**< ref to last system-channel, internally used */


#define CM_MAX_EDD  (EDD_CFG_MAX_INTERFACE_CNT)

#if CM_CFG_USE_MRP
#define CM_MAX_MRP  (EDD_CFG_MAX_INTERFACE_CNT)
#else
#define CM_MAX_MRP  (0)
#endif

#if CM_CFG_USE_GSY
#define CM_MAX_GSY  (EDD_CFG_MAX_INTERFACE_CNT)
#else
#define CM_MAX_GSY  (0)
#endif

#if CM_CFG_USE_POF
#define CM_MAX_POF  (EDD_CFG_MAX_INTERFACE_CNT)
#else
#define CM_MAX_POF  (0)
#endif

#define CM_MAX_OHA  (EDD_CFG_MAX_INTERFACE_CNT)

#define CM_MAX_NARE (EDD_CFG_MAX_INTERFACE_CNT)

#define CM_MAX_RPC  (EDD_CFG_MAX_INTERFACE_CNT)

#define CM_MAX_PD   (EDD_CFG_MAX_INTERFACE_CNT)

#define CM_MAX_CHANNELS \
	( CM_MAX_EDD \
	+ CM_MAX_MRP \
	+ CM_MAX_GSY \
	+ CM_MAX_POF \
	+ CM_MAX_OHA \
	+ CM_MAX_NARE \
	+ CM_MAX_RPC \
	+ CM_MAX_PD \
	+ CM_CFG_MAX_CLIENTS \
	+ CM_CFG_MAX_MULTICAST_CHANNELS \
	+ CM_CFG_MAX_SERVERS \
	)


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief The Details pointer is used to configure a CM channel. See GET_PATH_INFO().
@details The parameter multi_api_device was removed with V5.3i1.10 (AP01499644).
******************************************************************************/
typedef struct cm_detail_tag {

	LSA_UINT8  path_type; /**< see @ref cm_path_type_enum */

	LSA_UINT8  nic_id; /**< an ID that defines the NIC (network connector) at the lower end of the path */

	union cm_detail_path_type_tag {

		struct cm_detail_edd_tag {

			LSA_UINT8 nothing_so_far; /**< see CM_EDD_STATE_IND_RESOURCE_COUNT */

		} edd; /**< for CM_PATH_TYPE_EDD */

		struct cm_detail_mrp_tag {

			LSA_UINT8 nothing_so_far; /**< nothing so far*/
		} mrp; /**< for CM_PATH_TYPE_MRP */

		struct cm_detail_gsy_tag {

			LSA_UINT8 nothing_so_far; /**< nothing so far*/
		} gsy; /**< for CM_PATH_TYPE_GSY */

		struct cm_detail_pof_tag {

			LSA_UINT8 nothing_so_far; /**< nothing so far*/
		} pof; /**< for CM_PATH_TYPE_POF */

		struct cm_detail_oha_tag {

			LSA_UINT16  hello_resource_count;
			/**<
			 * number of hello-indication resources (OHA_OPC_HELLO_INDICATION)
			 * - IO Controller with 'fast startup' (see PNIO-Spec)
			 *     value := same as OHA_CDB_PATH_PARAMS_TYPE::Dcp.NumOfRecvFSU
			 *     note that if 'fast startup' is enabled, 'accelerated mode' (see u.nare) should be enabled too!
			 * - IO Controller without 'fast startup'
			 *     value := 0
			 * - IO Device:
			 *     value := 0
			 */
		} oha; /**< for CM_PATH_TYPE_OHA */

		struct cm_detail_nare_tag {

			LSA_UINT16 resolve_notify_resource_count;
			/**<
			 * number of resolve-notify-indication resources (NARE_OPC_RESOLVE_CONFIG_NOTIFY)
			 * - IO Controller with 'accelerated mode' (see PNIO-Spec)
			 *     value := same as CM_DETAIL_TYPE::u.cl.max_connects_per_second
			 * - IO Controller without 'accelerated mode'
			 *     value := 0
			 * - IO Device:
			 *     value := 0
			 */
		} nare; /**< for CM_PATH_TYPE_NARE */

		struct cm_detail_clrpc_tag {

			LSA_UINT16  recv_resource_count;
			/**<
			 * resources for the clrpc-client (see the clrpc-doc)
			 */

		} rpc; /**< for CM_PATH_TYPE_RPC */

		struct cm_detail_pd_tag {

			struct {

				LSA_UINT16  recv_resource_count;
				/**<
				 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
				 *
				 * @note alloc_len and call_resource_count are determined internally
				 */

				LSA_UINT16  send_resource_count;
				/**<
				 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
				 *
				 * @note alloc_len and call_resource_count are determined internally
				 */
			} epm; /**< the cm-pd creates a clrpc-server for the endpoint-mapper interface (AP00706078) */
		} pd; /**< for CM_PATH_TYPE_PD */

		struct cm_detail_cl_tag {

			struct {

				LSA_UINT32  alloc_len;
				LSA_UINT16  call_resource_count;
				LSA_UINT16  recv_resource_count;
				LSA_UINT16  send_resource_count;
				/**<
				 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
				 *
				 * note: the parameter server_udp_port was removed with V4.0i80.5 (AP00619205)
				 *       the port is now always selected by the ip-stack (server_udp_port = 0)
				 */
			} rpc; /**<
				   * each cm-client creates...
				   * - a clrpc-server for the IPNIO_Controller interface
				   * - and a clrpc-client for each AR (Connect, Release, ...)
				   */


			struct {

				LSA_UINT16 alarm_high_prio_ind_resource_count;
				LSA_UINT16 alarm_low_prio_ind_resource_count;
				LSA_UINT16 max_alarm_data_length; /**< must match with CM_OPC_CL_CREATE::max_alarm_data_length (see AP00342171) */
			} acp; /**< indication resources for acp (see the acp-doc) */

			LSA_UINT16 max_connects_per_second; /**< simultaneously running IPNIO-Connects. */
			LSA_UINT16 min_reconnect_timeout; /**< a failed connect waits "min_reconnect_timeout" before reconnecting. (in seconds) */
			LSA_UINT16 schedule_timeout; /**< a schedule decision is taken every "schedule_timeout" (in milliseconds (~200 msec)) */

		} cl; /**< for CM_PATH_TYPE_CLIENT */

		struct cm_detail_mc_tag {

			LSA_UINT8  nothing_so_far;
			/**	<
			 * indication resources for acp (see the acp-doc)
			 */
		} mc; /**< for CM_PATH_TYPE_MULTICAST */


		struct cm_detail_sv_tag {

			struct {

				LSA_UINT32  alloc_len;
				LSA_UINT16  call_resource_count;
				LSA_UINT16  recv_resource_count;
				LSA_UINT16  send_resource_count;
				/**<
				 * resources for the clrpc-server (see the clrpc-doc, registering interfaces)
				 *
				 * note: the parameter server_udp_port was removed with V4.0i80.5 (AP00619205)
				 *       the port is now always selected by the ip-stack (server_udp_port = 0)
				 */
			} rpc; /**<
					 * each cm-server creates...
					 * - a clrpc-server for the IPNIO_Device interface
					 * - and a clrpc-client for each AR (Appl-Ready callback)
					 */

			struct {

				LSA_UINT16 alarm_high_prio_ind_resource_count;
				LSA_UINT16 alarm_low_prio_ind_resource_count;
				LSA_UINT16 max_alarm_data_length;
			} acp; /**< indication resources for acp (see the acp-doc)	*/

		} sv; /**< for CM_PATH_TYPE_SERVER */
	} u; /**< path type union */

} CM_DETAIL_TYPE; /**< CM_DETAIL_TYPE, used for CM_GET_PATH_INFO() */

typedef CM_DETAIL_TYPE *CM_DETAIL_PTR_TYPE;

/*------------------------------------------------------------------------------
// cm init-parameters
//----------------------------------------------------------------------------*/

/** The type used for CM_INIT(). */
typedef struct cm_init_tag {

	LSA_UINT16 oneshot_lsa_time_base;   /**< oneshot-timer, see cm_oneshot_sys_init() */
	LSA_UINT16 oneshot_lsa_time_factor; /**< oneshot-timer, see cm_oneshot_sys_init() */

} CM_INIT_TYPE;

typedef CM_INIT_TYPE * CM_INIT_PTR_TYPE; /**< The pointer type used for CM_INIT(). */

typedef CM_INIT_TYPE const * CM_INIT_CONST_PTR_TYPE; /**< The const pointer type used for CM_INIT(). */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/

/**************************************************************************//**
@defgroup cm-product-components-input-functions Input functions
@ingroup cm-group
@brief The configuration options and the input functions are listed on this page according to [/1/](@ref cm-ref-no-1).

## Unused input functions

### cm_upper_rqb(LSA_USER_ID_TYPE user_id, LSA_UINT16 length, CM_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
Asynchronous allocation of an upper-RQB.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_rqb_ptr pointer to upper RQB

### cm_upper_mem(LSA_USER_ID_TYPE user_id, LSA_UINT16 length, CM_UPPER_MEM_PTR_TYPE upper_mem_ptr)
Asynchronous allocation of upper-memory.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_mem_ptr pointer to upper memory
******************************************************************************/


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Initialization by CM
@details
Must be called before calling the first open channel

Synchronous, according to [/1/](@ref cm-ref-no-1).

Initialization of the module.

A cyclic LSA timer with the specified resolution is allocated. CM_FATAL_ERROR() is called if the timer cannot be allocated.

CM derives internally oneshot-timers from this cyclic LSA timer.

Recommendation for lsa_time_base, lsa_time_factor:

lsa_time_base = LSA_TIME_BASE_100MS (100msec)
lsa_time_factor = 1.

@param [in] init Pointer to init structure
@return LSA_RET_OK
******************************************************************************/
LSA_UINT16 cm_init(
	CM_INIT_CONST_PTR_TYPE  init
);

/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief "Cleanup" of the CM components.
@details
De-initialization of the module. The timer allocated in cm_init is released again.

no params
@return LSA_RET_OK
******************************************************************************/
LSA_UINT16 cm_undo_init(
	LSA_VOID
);

/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function of the LSA timer.
@details
CM derives other Internal timers from this LSA timer.

Synchronizes the callback with the CM task through "Timer Mail" to itself.

Refer to output function @ref CM_REQUEST_LOCAL.

@param [in] timer_id
@param [in] user_id
@return LSA_RET_OK
******************************************************************************/
LSA_VOID cm_timeout(
	LSA_TIMER_ID_TYPE  timer_id,
	LSA_USER_ID_TYPE  user_id
);


/*=============================================================================
 * function name:  cm_fatal, cm_fatal1
 *
 * function:       undocumented, not in cm_int.h because of cm_cfg.c
 *===========================================================================*/
LSA_VOID
cm_fatal(
	LSA_UINT16 cm_module_id,
	LSA_INT line
);

#ifndef CM_FATAL /* AP00289268, note: returning from fatal is not allowed */
/*lint -function(exit, cm_fatal) */
#define CM_FATAL()  cm_fatal(CM_MODULE_ID, __LINE__) /*lint -unreachable */
#endif


LSA_VOID
cm_fatal1(
	LSA_UINT16 cm_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
);

#ifndef CM_FATAL1 /* AP00289268, note: returning from fatal is not allowed */
/*lint -function(exit, cm_fatal1) */
#define CM_FATAL1(ec_0)       cm_fatal1 (CM_MODULE_ID, __LINE__, ec_0) /*lint -unreachable */
#endif


/*====  out functions  =====*/

/**************************************************************************//**
@defgroup cm-product-components-output-functions Output functions
@ingroup cm-group
@brief These are the output functions for CM.

## Unused output functions

These are synchronous, according to [/1/](@ref cm-ref-no-1).

### CM_LOWER_TO_UPPER_MEM(CM_UPPER_MEM_PTR_TYPE * upper_mem_ptr_ptr, CM_LOWER_MEM_PTR_TYPE lower_mem_ptr, LSA_SYS_PTR_TYPE sys_ptr)

Convert lower-memory to upper-memory.

@param [out] upper_mem_ptr_ptr upper-memory
@param [in]  lower_mem_ptr     lower-memory
@param [in]  sys_ptr           system pointer

### CM_UPPER_TO_LOWER_MEM(CM_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr, CM_UPPER_MEM_PTR_TYPE upper_mem_ptr, LSA_SYS_PTR_TYPE sys_ptr)
Convert upper-memory to lower-memory.

@param [out] lower_mem_ptr_ptr lower-memory
@param [in]  upper_mem_ptr     upper-memory
@param [in]  sys_ptr           system pointer


### CM_LOWER_TO_LOWER_MEM(CM_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr, CM_LOWER_MEM_PTR_TYPE lower_mem_ptr, LSA_SYS_PTR_TYPE src_sys_ptr, LSA_SYS_PTR_TYPE dst_sys_ptr)
Convert lower-memory to lower-memory.

@param [out] lower_mem_ptr_ptr lower-memory
@param [in]  lower_mem_ptr     lower-memory
@param [in]  src_sys_ptr       source system pointer
@param [in]  dst_sys_ptr       destination system pointer

### CM_COPY_COMMON_MEM(LSA_VOID * src_common_mem_ptr, LSA_VOID * dst_common_mem_ptr, LSA_UINT16 length)
Copy common memory. The memory areas of source and destination mustn't overlap.

@param [in]  src_common_mem_ptr source common mem pointer
@param [out] dst_common_mem_ptr destination common mem pointer
@param [in]  length             length

### CM_ENTER()
Set reentrance lock.

### CM_EXIT()
Cancel reentrance lock.
******************************************************************************/


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Get system-pointer and detail-pointer of a communication channel.
@details
An LSA_RET_OK must be returned in ret_val_ptr, otherwise CM calls CM_FATAL_ERROR(), refer to [/1/](@ref cm-ref-no-1) and CM_REQUEST_UPPER_DONE.

The Details pointer is used to configure a CM channel.

The System pointer is opaque for CM, and it is used by the functions of system connection.

The "path_type" defines the lower layer to be opened (refer to chapter @ref cm-channels ).

The "nic_id" is used to group together the system and user channels that belong to the same network connector (NIC). The "nic_id" is a "key value": All channels that contain the same value belong together. See too the FAQ below.

## FAQ for "nic_id" and "interface_id" (also written as NicID and InterfaceID)

<b>Q:</b> What is the "nic_id"? \n
<b>A:</b> The "nic_id" is a "key value": All channels that contain the same value belong together.

<b>Q:</b> What for is the "nic_id" used? \n
<b>A:</b> In a system with e.g. 2 Ethernet interfaces there are also 2 CM-EDD channels. These 2 CM-EDD channels have the same "path_type" but a different "nic_id".

<b>Q:</b> What is the "interface_id"? \n
<b>A:</b> The "interface_id" is the value that is printed on the casing (e.g. X5 -> interface_id = 5). It is a parameter of the EDD setup.

<b>Q:</b> Why are there 2 identifiers? \n
<b>A:</b> Because they are not the same thing. Usecase: 2 devices run the same firmware. On one device the interface is X1 and on the other it is X7. Having 2 identifiers makes the the LSA stack independent of the printed value (compile time values for the "nic_id" and runtime values for the "interface_id").

<b>Q:</b> Is there a common definition for the "nic_id" for all LSA components? \n
<b>A:</b> No! Each component defines the type and value range (and the name!). However, if the numbers selected are small enough, it should be possible to use them for all LSA components.

<b>Q:</b> Can the "interface_id" be used as the "nic_id"? \n
<b>A:</b> Yes, if it is known at compile time.

<b>Q:</b> Can the "nic_id" be used as the "interface_id"? \n
<b>A:</b> No!

Ad "u.sv.alloc_len": See @ref CM_OPC_CL_DEVICE_ADD.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr    return value
@param [out] sys_ptr_ptr    system pointer
@param [out] detail_ptr_ptr detail pointer
@param [in]  path           LSA path
******************************************************************************/
#ifndef CM_GET_PATH_INFO
LSA_VOID
CM_GET_PATH_INFO(
	LSA_UINT16          *  ret_val_ptr,
	LSA_SYS_PTR_TYPE    *  sys_ptr_ptr,
	CM_DETAIL_PTR_TYPE  *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                        path
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of information, requested with CM_GET_PATH_INFO.
@details
Release system-pointer and detail-pointer of a communication channel.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr return value
@param [in]  sys_ptr     system pointer
@param [in]  detail_ptr  detail pointer
******************************************************************************/
#ifndef CM_RELEASE_PATH_INFO
LSA_VOID
CM_RELEASE_PATH_INFO(
	LSA_UINT16         * ret_val_ptr,
	LSA_SYS_PTR_TYPE     sys_ptr,
	CM_DETAIL_PTR_TYPE   detail_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory that is internally required by CM.
@details
Alignment, according to request of processor, e.g., 32-bit for TriCore.

The memory must be accessible within CM.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] local_mem_ptr_ptr requested memory
@param [in]  length            alloc length
******************************************************************************/
#ifndef CM_ALLOC_LOCAL_MEM
LSA_VOID
CM_ALLOC_LOCAL_MEM(
	CM_LOCAL_MEM_PTR_TYPE * local_mem_ptr_ptr,
	LSA_UINT32              length
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a requested memory with CM_ALLOC_LOCAL_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr   return value
@param [in]  local_mem_ptr memory to be freed
******************************************************************************/
#ifndef CM_FREE_LOCAL_MEM
LSA_VOID
CM_FREE_LOCAL_MEM(
	LSA_UINT16            * ret_val_ptr,
	CM_LOCAL_MEM_PTR_TYPE   local_mem_ptr
);
#endif


#if 0 /* not used, #ifndef CM_LOWER_TO_UPPER_MEM */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Convert lower-memory to upper-memory.
@details Not used. Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] upper_mem_ptr_ptr upper-memory
@param [in]  lower_mem_ptr     lower-memory
@param [in]  sys_ptr           system pointer
******************************************************************************/
LSA_VOID CM_LOWER_TO_UPPER_MEM(
	CM_UPPER_MEM_PTR_TYPE * upper_mem_ptr_ptr,
	CM_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

#if 0 /* not used, #ifndef CM_UPPER_TO_LOWER_MEM */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Convert upper-memory to lower-memory.
@details Not used. Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] lower_mem_ptr_ptr lower-memory
@param [in]  upper_mem_ptr     upper-memory
@param [in]  sys_ptr           system pointer
******************************************************************************/
LSA_VOID CM_UPPER_TO_LOWER_MEM(
	CM_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	CM_UPPER_MEM_PTR_TYPE   upper_mem_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr
);
#endif


#if 0 /* not used, #ifndef CM_LOWER_TO_LOWER_MEM */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Convert lower-memory to lower-memory.
@details
Not used. Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out]lower_mem_ptr_ptr lower-memory
@param [in] lower_mem_ptr     lower-memory
@param [in] src_sys_ptr       source system pointer
@param [in] dst_sys_ptr       destination system pointer
******************************************************************************/
LSA_VOID CM_LOWER_TO_LOWER_MEM(
	CM_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	CM_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE        src_sys_ptr,
	LSA_SYS_PTR_TYPE        dst_sys_ptr
);
#endif


#if 0 /* not used, #ifndef CM_COPY_COMMON_MEM */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Copy common memory.
@details
The memory areas of source and destination mustn't overlap.

Not used. Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in]  src_common_mem_ptr source common mem pointer
@param [out] dst_common_mem_ptr destination common mem pointer
@param [in]  length             length
******************************************************************************/
LSA_VOID CM_COPY_COMMON_MEM(
	LSA_VOID   * src_common_mem_ptr,
	LSA_VOID   * dst_common_mem_ptr,
	LSA_UINT16   length
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting an LSA timer.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr  return value
@param [out] timer_id_ptr timer id
@param [in]  timer_type   timer type
@param [in]  time_base    timer base
******************************************************************************/
#ifndef CM_ALLOC_TIMER
LSA_VOID
CM_ALLOC_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE * timer_id_ptr,
	LSA_UINT16          timer_type,
	LSA_UINT16          time_base
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Starting the LSA timers.
@details
The input function cm_timeout is called after the expiry of the time.

An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
@param [in]  user_id     user id
@param [in]  time        time
******************************************************************************/
#ifndef CM_START_TIMER
LSA_VOID
CM_START_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Stopping the LSA timer.
@details
An LSA timer can be triggered again after "stop" (e.g., if already in a mailbox, etc).

An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
******************************************************************************/
#ifndef CM_STOP_TIMER
LSA_VOID
CM_STOP_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a requested LSA timer with CM_ALLOC_TIMER.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] ret_val_ptr return value
@param [in]  timer_id    timer id
******************************************************************************/
#ifndef CM_FREE_TIMER
LSA_VOID
CM_FREE_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id
);
#endif


#if 0 /* not used, #ifndef CM_ENTER */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Set reentrance lock.
@details Not used.
******************************************************************************/
LSA_VOID CM_ENTER(
	LSA_VOID
);
#endif


#if 0 /* not used, #ifndef CM_EXIT */
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Cancel reentrance lock.
@details Not used.
******************************************************************************/
LSA_VOID CM_EXIT(
	LSA_VOID
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Reporting a fatal error.
@details
No return from this function.

Synchronous, according to [/1/](@ref cm-ref-no-1).

## LSA_FATAL_ERROR_TYPE

@code
typedef struct lsa_fatal_error_tag
{
LSA_UINT16         lsa_component_id;
LSA_UINT16         module_id;
LSA_UINT16         line;
LSA_UINT16         error_data_length;
LSA_UINT32         error_code[4];
LSA_VOID_PTR_TYPE  error_data_ptr;
} LSA_FATAL_ERROR_TYPE;
@endcode

@param [in] length    length
@param [in] error_ptr error
******************************************************************************/
#ifndef CM_FATAL_ERROR
LSA_VOID
CM_FATAL_ERROR(
	LSA_UINT16               length,
	CM_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Returns a finished request to the upper layer.
@details
The request is handled. The RQB has to be sent back to the originator.
This macro is called by the CM if a request (RQB) from calling cm_open_channel(), cm_request() or cm_close_channel() is finished.
The CM does not call the callback function provided by cm_open_channel() directly, but calls this output macro instead.

The system adaption has to call the upper callback function to finish the request.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] cm_request_upper_done_ptr pointer to cm_request_upper_done_ptr, given by cm_open_channel()
@param [in] upper_rqb_ptr             pointer to upper-RQB
@param [in] sys_ptr                   system pointer
******************************************************************************/
#ifndef CM_REQUEST_UPPER_DONE
LSA_VOID
CM_REQUEST_UPPER_DONE(
	CM_UPPER_CALLBACK_FCT_PTR_TYPE  cm_request_upper_done_ptr,
	CM_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Read the 64 bit cycle counter to get the current time stamp in micro seconds since startup or since a specific day (e.g. 1.1.1970).
@details
Every call should give an ascending timestamp.
In the worst case, the next call should show the same value as before.

There is no overflow expected (within the next years).

The granularity of the value must be microsecond.

If there is no microsecond resolution is available, a timer with a lower resolution might be used.

@internal
The difference between 2 timestamps is used to measure the fast startup, see StartupStatisticsRecord.
@endinternal

@param [out] ret_val_ptr         return value
@param [in]  sys_ptr             system pointer
@param [in]  *cycle_counter_ptr  cycle counter
******************************************************************************/
#ifndef CM_GET_CYCLE_COUNTER
LSA_VOID
CM_GET_CYCLE_COUNTER(
	LSA_UINT16       * ret_val_ptr,
	LSA_SYS_PTR_TYPE   sys_ptr,
	LSA_UINT64       * cycle_counter_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Get the primary AR of an AR-set.
@details
The system adaption must call the eddi_ProviderGetPrimaryAR() or eddp_ProviderGetPrimaryAR() EDD function.

For prototype, refer to file .../cfg/cm_sys.h

For macros, refer to file .../cfg/cm_cfg.h

For examples, refer to file .../cfg/cm_cfg.c (not an official part of CM!)

Usage see chapter @ref cm-server-channels-sysred.

@param [in] arset_nr arset number
@param [in] sys_ptr  system pointer
******************************************************************************/
#ifndef CM_GET_PRIMARY_AR
LSA_UINT16
CM_GET_PRIMARY_AR(
	LSA_UINT16        arset_nr,
	LSA_SYS_PTR_TYPE  sys_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_SYS_H */
