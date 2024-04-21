#ifndef CLRPC_LOW_H
#define CLRPC_LOW_H

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
/*  F i l e               &F: clrpc_low.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Lower-Layer interface                                                    */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef CLRPC_LOWER_RQB_TYPE *CLRPC_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

/*===========================================================================*/

/* sock doesn't define the pointers */

typedef SOCK_RQB_ARGS_CHANNEL_TYPE	 *CLRPC_LL_OPEN_CHANNEL_PTR_TYPE;
typedef SOCK_RQB_ARGS_UDP_OPEN_TYPE	 *CLRPC_LL_UDP_OPEN_PTR_TYPE;
typedef SOCK_RQB_ARGS_CLOSE_TYPE	 *CLRPC_LL_UDP_CLOSE_PTR_TYPE;
typedef SOCK_RQB_ARGS_DATA_TYPE		 *CLRPC_LL_UDP_DATA_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Callback function for responses from the lower layer.
@details
Confirmation of clrpc_open_channel(), clrpc_close_channel() and clrpc_request().

The client and server subcomponents each have their own callback function as they respectively open their own channel to the lower layer.

Due to the system connection described in chapter @ref clrpc-architecture-constraints-connection,
this function can be viewed from outside, although they need not be as per LSA theory.

All results and indications of the lower layer are sent to CLPRC via this function.

If the RQB cannot be analyzed, the output function CLRPC_FATAL1() is called (e.g.: NULL pointer).
If an error situation cannot be reported to the upper layer, the output function CLRPC_FATAL_ERROR() is called.

@cond INTERNAL
For implementation details refer [/5/](@ref clrpc-ref-no-5) and [/6/](@ref clrpc-ref-no-6).
@endcond

@param lower_rqb_ptr  pointer to the request block of lower layer
******************************************************************************/
LSA_VOID clrpc_request_lower_done(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/*=============================================================================
 * function name:  clrpc_lower_rqb
 *
 * function:       asynchronous allocation of a lower-RQB
 *
 * NOTE: not used
 *
 *===========================================================================*/

#if 0
LSA_VOID clrpc_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    CLRPC_LOWER_RQB_PTR_TYPE   lower_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  clrpc_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * NOTE: not used
 *
 *===========================================================================*/

#if 0
LSA_VOID clrpc_lower_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    CLRPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr
);
#endif

/*====  out functions  =====*/

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Opens the communication channel to the lower layer (SOCK).
@details
Asynchronous. As per LSA draft.

Sends the Open-Channel RQB to the lower layer (and there sets comp_id).

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef CLRPC_OPEN_CHANNEL_LOWER
LSA_VOID CLRPC_OPEN_CHANNEL_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Closes the communication channel to the lower layer.
@details
Asynchronous. As per LSA draft.

Sends the Close Channel RQB to the lower layer (and there sets comp_id).

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system pointer
******************************************************************************/
#ifndef CLRPC_CLOSE_CHANNEL_LOWER
LSA_VOID CLRPC_CLOSE_CHANNEL_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Transfers an order to the lower layer.
@details
Asynchronous. As per LSA draft.

Sends the RQB to the lower layer (and there sets comp_id).

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system pointer
******************************************************************************/
#ifndef CLRPC_REQUEST_LOWER
LSA_VOID CLRPC_REQUEST_LOWER(
    CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Request for a request block with storage attributes of the lower layer.
@details
Synchronous. As per LSA draft.

Alignment based on request from processor. E.g. 32 Bit for TriCore.

The storage must be accessible from the upper layer interface of the lower layer.

@param [out] lower_rqb_ptr_ptr pointer to lower-RQB
@param [in]  user_id           user id
@param [in]  length            alloc length
@param [in]  sys_ptr           system pointer
******************************************************************************/
#ifndef CLRPC_ALLOC_LOWER_RQB
LSA_VOID CLRPC_ALLOC_LOWER_RQB(
    CLRPC_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of a request block, requested with @ref CLRPC_ALLOC_LOWER_RQB.
@details
Synchronous. As per LSA draft.

@param [out] ret_val_ptr    return value
@param [in]  lower_rqb_ptr  pointer to the request block of the layer below
@param [in]  sys_ptr        system pointer
******************************************************************************/
#ifndef CLRPC_FREE_LOWER_RQB
LSA_VOID CLRPC_FREE_LOWER_RQB(
    LSA_UINT16               *  ret_val_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE    lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Request for storage with storage attributes of the lower layer.
@details
Synchronous. As per LSA draft.

Alignment based on processor requirements. E.g. 32-Bit for TriCore.

The storage must be accessible from the upper layer interface of the lower layer.

@param [out] lower_mem_ptr_ptr  pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CLRPC_ALLOC_LOWER_MEM
LSA_VOID CLRPC_ALLOC_LOWER_MEM(
    CLRPC_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup clrpc-group-output-functions
@brief Release of memory, requested with CLRPC_ALLOC_LOWER_MEM.
@details
Synchronous. As per LSA draft.

@param [out] ret_val_ptr   return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
#ifndef CLRPC_FREE_LOWER_MEM
LSA_VOID CLRPC_FREE_LOWER_MEM(
    LSA_UINT16               * ret_val_ptr,
    CLRPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_LOW_H */
