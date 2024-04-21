#ifndef ACP_SYS_H
#define ACP_SYS_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_sys.h                                 :F&  */
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
/*                       constants for detail-pointer                        */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define ACP_LOWER_MEM_PTR_TYPE               /* pointer to lower-memory */ \
    LSA_VOID                     *

#define ACP_LOCAL_MEM_PTR_TYPE               /* pointer to local-memory */ \
    LSA_VOID                     *

#define ACP_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag   *


/*------------------------------------------------------------------------------
// acp_init_tag
//----------------------------------------------------------------------------*/

typedef struct acp_init_tag
{
	LSA_UINT16	rta_time_base;		/**< Time base of the LSA timer, see LSA_TIME_BASE_xxx */
	LSA_UINT16  rta_time_factor;	/**< Factor of the LSA timer */
} ACP_INIT_TYPE;

typedef struct acp_init_tag * ACP_INIT_PTR_TYPE;
typedef struct acp_init_tag const* ACP_INIT_CONST_PTR_TYPE;


/*------------------------------------------------------------------------------
// acp_detail_tag
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup acp-group-output-macros
******************************************************************************/
enum acp_channel_type_enum {
	ACP_CHANNEL_TYPE_SYSTEM		= 1, /**< system channel type */
	ACP_CHANNEL_TYPE_USER		= 2  /**< user channel type */
};

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Detail pointer is used to configure an ACP instance.
@details
@par nic_id
	This number identifies the network interface. \n
	It is used to combine several ACP instances of the same network interface.
	All ACP instances that are assigned the same **nic_id** use the same network interface.\n
	Refer to @ref acp-functions-instances-system-user-channel and @ref acp-other-system-connection-sys-path.

@par channel_type
	Identifies the type of the channel. Refer to @ref acp_channel_type_enum

## The following is applicable to channel_type == ACP_CHANNEL_TYPE_SYSTEM:
@par type.system.max_edd_nrt_send_res
	Specifies the number of NRT and send resources. These resources form the pool common to all user channels
	for transmitting the RTA protocol elements.
@par type.system.max_edd_nrt_recv_res
	Specifies the number of NRT reception resources for the lower layer. These resources form the pool common
	to all user channels for receiving RTA protocol elements.

## The following is applicable to channel_type == ACP_CHANNEL_TYPE_USER:
@par type.user.alarm_ref_base
	Specifies the first available alarm instance reference.
@par type.user.nr_of_alarm_ref
	Specifies the number of assigned references.\n
	The used alarm instance references must correspond to the following condition:\n
	alarm_ref_base  <=  used_alarm_ref  <  (alarm_ref_base + nr_of_alarm_ref)

@note
	One AlarmProvider/AlarmConsumer having the 'high' priority and one AlarmProvider/AlarmConsumer
	having the 'low' priority are addressed corresponding to each alarm reference.
******************************************************************************/
typedef struct acp_detail_tag
{
	LSA_UINT16	nic_id;					/**< reference to the network device which must be used  */
	LSA_UINT16	channel_type;			/**< see @ref acp_channel_type_enum */

	union acp_detail_union {

	struct acp_detail_sys_tag {

		LSA_UINT16  max_edd_nrt_send_res;		/**< count of send resouces, used by edd.nrt.send */
		LSA_UINT16  max_edd_nrt_recv_res;		/**< count of recv resouces, used by edd.nrt.recv */

	}  system;

	struct acp_detail_usr_tag {

			/* alarm */
		LSA_UINT16	alarm_ref_base;			/**< first available alarm-reference number */
		LSA_UINT16	alarm_nr_of_ref;		/**< number of available reference numbers */

	}  user;
	} type;
} ACP_DETAIL_TYPE;

typedef struct acp_detail_tag * ACP_DETAIL_PTR_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Initialization of the ACP component
@details
Basic initialization of the module. Must be called once during restart.

LSA function, synchronous

Note:
Dynamic data has not been necessary up till now.\n
The size of the handle table is specified during the translation time with\n
\#define ACP_CFG_MAX_CHANNELS   xxx.

The initialization values of a LSA timer are required.\n
Refer to @ref acp-interfaces-initptr

@param [in] init Pointer to init structure
@return LSA_RET_OK
******************************************************************************/
LSA_UINT16 acp_init (
    ACP_INIT_CONST_PTR_TYPE  init
);

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Closing the ACP component
@details LSA function, synchronous

@return LSA_RET_OK
******************************************************************************/
LSA_UINT16 acp_undo_init (
    LSA_VOID
);

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Callback function for the System Connection.
@details
LSA function, synchronous

LSA timer events take place in any convenient task context.\n
ACP synchronizes these events by sending a mail to the ACP task. The internal opcode ACP_OPC_INTERNAL_TIMER is used.

Refer also to @ref ACP_REQUEST_LOCAL

@param [in] timer_id Return value of ACP_ALLOC_TIMER.
@param [in] user_id The user_id specified along with ACP_START_TIMER.
******************************************************************************/
LSA_VOID acp_timeout (
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id
);


/*====  out functions  =====*/

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Get system-pointer and detail-pointer of a communication channel
@details
Synchronous.

@param [out] ret_val_ptr    return value: LSA_RET_OK, LSA_RET_ERR_SYS_PATH
@param [out] sys_ptr_ptr    return value: pointer to system-pointer
@param [out] detail_ptr_ptr return value: pointer to detail-pointer
@param [in] path            path of a communication channel
******************************************************************************/
#ifndef ACP_GET_PATH_INFO
LSA_VOID ACP_GET_PATH_INFO (
    LSA_UINT16            *  ret_val_ptr,
    LSA_SYS_PTR_TYPE      *  sys_ptr_ptr,
    ACP_DETAIL_PTR_TYPE   *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                          path
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Release system-pointer and detail-pointer of a communication channel
@details
Synchronous.

@param [out] ret_val_ptr    LSA_RET_OK: ok\n
                            LSA_RET_ERR_PARAM: no release because of wrong system-pointer or wrong detail-pointer
@param [in] sys_ptr         system-pointer
@param [in] detail_ptr      detail-pointer
******************************************************************************/
#ifndef ACP_RELEASE_PATH_INFO
LSA_VOID ACP_RELEASE_PATH_INFO (
    LSA_UINT16  *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr,
    ACP_DETAIL_PTR_TYPE               detail_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocates memory for internal requirement.
@details
Synchronous.

@param [out] local_mem_ptr_ptr  pointer to local-memory-pointer
@param [in] length              length of memory
******************************************************************************/
#ifndef ACP_ALLOC_LOCAL_MEM
LSA_VOID ACP_ALLOC_LOCAL_MEM (
    ACP_LOCAL_MEM_PTR_TYPE  *  local_mem_ptr_ptr,
    LSA_UINT32                 length
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Frees the requested memory.
@details
Synchronous.

@param [out] ret_val_ptr    LSA_RET_OK: ok\n
                            LSA_RET_ERR_PARAM: no deallocation because of wrong pointer to local-memory
@param [in] local_mem_ptr   pointer to local-memory
******************************************************************************/
#ifndef ACP_FREE_LOCAL_MEM
LSA_VOID ACP_FREE_LOCAL_MEM (
    LSA_UINT16  *  ret_val_ptr,
    ACP_LOCAL_MEM_PTR_TYPE            local_mem_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocates a new LSA timer.
@details
Synchronous.

@param [out] ret_val_ptr    LSA_RET_OK: timer has been allocated\n
                            LSA_RET_ERR_NO_TIMER: no timer has been allocated
@param [out] timer_id_ptr   pointer to id of timer
@param [in] timer_type      LSA_TIMER_TYPE_ONE_SHOT or\n
                            LSA_TIMER_TYPE_CYCLIC
@param [in] time_base       LSA_TIME_BASE_1MS,\n
                            LSA_TIME_BASE_10MS,\n
                            LSA_TIME_BASE_100MS,\n
                            LSA_TIME_BASE_1S,\n
                            LSA_TIME_BASE_10S or\n
                            LSA_TIME_BASE_100S
******************************************************************************/
#ifndef ACP_ALLOC_TIMER
LSA_VOID ACP_ALLOC_TIMER (
    LSA_UINT16         *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  *  timer_id_ptr,
    LSA_UINT16            timer_type,
    LSA_UINT16            time_base
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Starts a LSA timer.
@details
Synchronous.

System will call acp_timeout() after expiration of the running time.

@param [out] ret_val_ptr    LSA_RET_OK: timer has been started\n
                            LSA_RET_OK_TIMER_RESTARTED: timer has been restarted\n
                            LSA_RET_ERR_PARAM: timer has not been started because of wrong timer-id
@param [in] timer_id        id of timer
@param [in] user_id         id of acp
@param [in] time            running time
******************************************************************************/
#ifndef ACP_START_TIMER
LSA_VOID ACP_START_TIMER (
    LSA_UINT16         *  ret_val_ptr,
    LSA_TIMER_ID_TYPE     timer_id,
    LSA_USER_ID_TYPE      user_id,
    LSA_UINT16            time
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Stops a LSA timer.
@details
Synchronous.

@param [out] ret_val_ptr    LSA_RET_OK: timer has been stopped\n
                            LSA_RET_OK_TIMER_NOT_RUNNING: timer was not running\n
                            LSA_RET_ERR_PARAM: timer has not been stopped because of wrong timer-id
@param [in] timer_id        id of timer
******************************************************************************/
#ifndef ACP_STOP_TIMER
LSA_VOID ACP_STOP_TIMER (
    LSA_UINT16        *  ret_val_ptr,
    LSA_TIMER_ID_TYPE    timer_id
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Frees a LSA timer.
@details
Frees a requested LSA timer with ACP_ALLOC_TIMER.

Synchronous.

@param [out] ret_val_ptr    LSA_RET_OK: timer has been deallocated\n
                            LSA_RET_ERR_TIMER_IS_RUNNING: because timer is running timer has not been deallocated\n
                            LSA_RET_ERR_PARAM: no deallocation because of wrong timer-id
@param [in] timer_id        id of timer
******************************************************************************/
#ifndef ACP_FREE_TIMER
LSA_VOID ACP_FREE_TIMER (
    LSA_UINT16        *  ret_val_ptr,
    LSA_TIMER_ID_TYPE    timer_id
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Set reentrance lock.
@details
Starts a non-interruptible code section.

Synchronous
******************************************************************************/
#ifndef ACP_ENTER
LSA_VOID ACP_ENTER (
    LSA_VOID
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Cancel reentrance lock.
@details
Exits a non-interruptible code section.

Synchronous
******************************************************************************/
#ifndef ACP_EXIT
LSA_VOID ACP_EXIT (
    LSA_VOID
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Reports a fatal error.
@details
No return from this function. The task is closed.

Synchronous

ACP_FATAL_ERROR_PTR_TYPE is a pointer to a LSA_FATAL_ERROR_TYPE structure.

## LSA_FATAL_ERROR_TYPE ##

@code
typedef struct lsa_fatal_error_tag
{
LSA_UINT16         lsa_component_id;        // LSA_COMP_ID_*
LSA_UINT16         module_id;               // *_MODULE_ID
LSA_UINT16         line;                    // line of file
LSA_UINT16         error_data_length;       // length in bytes of the
                                            // memory area addressed by
                                            // error_data_ptr
LSA_UINT32         error_code[4];
LSA_VOID_PTR_TYPE  error_data_ptr;
} LSA_FATAL_ERROR_TYPE;
@endcode

@param [in] length      number of bytes
@param [in] error_ptr   pointer to LSA_FATAL_ERROR
******************************************************************************/
#ifndef ACP_FATAL_ERROR
LSA_VOID ACP_FATAL_ERROR (
    LSA_UINT16                length,
    ACP_FATAL_ERROR_PTR_TYPE  error_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Returns a finished request to the upper layer.
@details
This macros is called by the ACP if a user request (RQB) from calling acp_open_channel(), acp_request() or acp_close_channel() is finished.
The ACP does not call the callback function provided by acp_open_channel() directly but calls this output macro instead.\n
Synchronous

@param [in] acp_request_upper_done_ptr  pointer to acp_request_upper_done_ptr, given by acp_open_channel()
@param [in] upper_rqb_ptr               pointer to upper-RQB
@param [in] sys_ptr                     system-pointer
******************************************************************************/
#ifndef ACP_REQUEST_UPPER_DONE
LSA_VOID ACP_REQUEST_UPPER_DONE (
    ACP_UPPER_CALLBACK_FCT_PTR_TYPE  acp_request_upper_done_ptr,
    ACP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif


/*===========================================================================*/
/* non-LSA-predefined             prototyping                                */
/*===========================================================================*/

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Auxiliary function for System Connection.
@details
No LSA function, synchronous

Fills up the global error structure acp_glob_fatal_error and calls the output macro ACP_FATAL_ERROR.

Is not called directly by ACP. Can be used by the System Connection.

Refer also to @ref ACP_FATAL_ERROR, @ref acp-interfaces-cfg-header-trailer

@param [in] module_id        Refer to \#define ACP_MODULE_ID per file
@param [in] line             Refer to \__LINE\__
@param [in] error_code_0     Can be used by System Connection.
@param [in] error_code_1     Can be used by System Connection.
@param [in] error_code_2     Can be used by System Connection.
@param [in] error_code_3     Can be used by System Connection.
@param [in] error_data_len   Can be used by System Connection.
@param [in] error_data       Can be used by System Connection.
******************************************************************************/
LSA_VOID
acp_fatal_error (
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data
	);


/*------------------------------------------------------------------------------
// ASRT within ACP
// need NRT
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocates an NRT send buffer for the lower layer.
@details
Synchronous

ACP always requests EDD_FRAME_BUFFER_LENGTH.\n
The physical address of the receiving buffer must satisfy special alignment and size requirements (hardware dependent)
and must be located in a memory area that is accessible to the Ethernet controller. For more important information about
the position and size of the receiving buffer, please refer to [/3/](@ref acp-ref-no-3) Chapter "Send buffer".

@attention  Important: The send buffer must, independently of the length, be at least 64-byte large.
			This is required by EDD for the padding of frames.

@param [out] lower_mem_ptr_ptr  pointer to lower memory pointer
@param [in] length              length
@param [in] sys_ptr             system-pointer
******************************************************************************/
#ifndef ACP_ALLOC_NRT_SEND_MEM
LSA_VOID
ACP_ALLOC_NRT_SEND_MEM (
    ACP_LOWER_MEM_PTR_TYPE  *  lower_mem_ptr_ptr,
    LSA_UINT32                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Frees the requested NRT send buffer.
@details
Synchronous

@param [out] ret_val_ptr    LSA_RET_OK: ok
@param [in] lower_mem_ptr   pointer to lower memory
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_FREE_NRT_SEND_MEM
LSA_VOID
ACP_FREE_NRT_SEND_MEM (
    LSA_UINT16             *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE    lower_mem_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Requests an NRT receiving buffer for the lower layer.
@details
Synchronous

ACP always requests EDD_FRAME_BUFFER_LENGTH.\n
The physical address of the receiving buffer must satisfy special alignment and size requirements (hardware-dependent)
and must be located in a memory zone that can be accessed by the Ethernet controller.\n
For further, important information about the position and size of the receiving buffer,
please refer to [/3/](@ref acp-ref-no-3) Chapter "Reception".


@param [out] lower_mem_ptr_ptr  pointer to lower memory pointer
@param [in] length              length
@param [in] sys_ptr             system-pointer
******************************************************************************/
#ifndef ACP_ALLOC_NRT_RECV_MEM
LSA_VOID
ACP_ALLOC_NRT_RECV_MEM (
    ACP_LOWER_MEM_PTR_TYPE  *  lower_mem_ptr_ptr,
    LSA_UINT32                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Releasing the requested NRT reception buffer.
@details
Synchronous

@param [out] ret_val_ptr    LSA_RET_OK: ok
@param [in] lower_mem_ptr   pointer to lower memory
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_FREE_NRT_RECV_MEM
LSA_VOID
ACP_FREE_NRT_RECV_MEM (
    LSA_UINT16             *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE    lower_mem_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif


/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Macro for standard C-lib function
@details
Corresponds to the 'memcpy' of the standard C library.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [out] dst    destination pointer
@param [in] src     source pointer
@param [in] length  length
******************************************************************************/
#ifndef ACP_MEMCPY
LSA_VOID
ACP_MEMCPY (
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           length
);
#endif


/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Put and convert 8-bit values from host to network byte order.
@details
Converts an 'unsigned integer' of size 8-bit from the host format into the network format ("big endian").\n
Writes the converted 'unsigned integer' to the 'base + offset' position in a buffer.\n
This position can be a randomly “aligned” address.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [out] base   base
@param [in] offset  offset
@param [in] val     value
******************************************************************************/
#ifndef ACP_PUT8_HTON
LSA_VOID
ACP_PUT8_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT8          val
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Get and convert 8-bit values from network to host byte order.
@details
Reads an 'unsigned integer' of size 8-bit from the buffer position 'base + offset'.\n
This position can be a randomly aligned address.\n
Converts this 'unsigned integer' from the network format ("big endian") to the host format.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [in] base    base
@param [in] offset  offset
@return             converted value
******************************************************************************/
#ifndef ACP_GET8_NTOH
LSA_UINT8
ACP_GET8_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Put and convert 16-bit values from host to network byte order.
@details
Converts an 'unsigned integer' of size 16-bit from the host format into the network format ("big endian").\n
Writes the converted 'unsigned integer' to the 'base + offset' position in a buffer.\n
This position can be a randomly “aligned” address.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [out] base   base
@param [in] offset  offset
@param [in] val     value
******************************************************************************/
#ifndef ACP_PUT16_HTON
LSA_VOID
ACP_PUT16_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT16         val
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Get and convert 16-bit values from network to host byte order.
@details
Reads an 'unsigned integer' of size 16-bit from the buffer position 'base + offset'.\n
This position can be a randomly aligned address.\n
Converts this 'unsigned integer' from the network format ("big endian") to the host format.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [in] base    base
@param [in] offset  offset
@return             converted value
******************************************************************************/
#ifndef ACP_GET16_NTOH
LSA_UINT16
ACP_GET16_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Put and convert 32-bit values from host to network byte order.
@details
Converts an 'unsigned integer' of size 32-bit from the host format into the network format ("big endian").\n
Writes the converted 'unsigned integer' to the 'base + offset' position in a buffer.\n
This position can be a randomly “aligned” address.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [out] base   base
@param [in] offset  offset
@param [in] val     value
******************************************************************************/
#ifndef ACP_PUT32_HTON
LSA_VOID
ACP_PUT32_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset,
	LSA_UINT32         val
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Get and convert 32-bit values from network to host byte order.
@details
Reads an 'unsigned integer' of size 32-bit from the buffer position 'base + offset'.\n
This position can be a randomly aligned address.\n
Converts this 'unsigned integer' from the network format ("big endian") to the host format.

WE WILL IMPLEMENT variants for mem-attribs when needed. All known platforms are "flat".

@param [in] base    base
@param [in] offset  offset
@return             converted value
******************************************************************************/
#ifndef ACP_GET32_NTOH
LSA_UINT32
ACP_GET32_NTOH ( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE  base,
	LSA_INT            offset
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_SYS_H */
