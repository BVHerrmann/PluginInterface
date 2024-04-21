#ifndef CM_LOW_H
#define CM_LOW_H

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
/*  F i l e               &F: cm_low.h                                  :F&  */
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
/*                                 types                                     */
/*===========================================================================*/

typedef CM_ACP_LOWER_RQB_TYPE  *CM_ACP_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_ACP_LOWER_RQB_TYPE  const *CM_ACP_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_EDD_LOWER_RQB_TYPE  *CM_EDD_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_EDD_LOWER_RQB_TYPE  const *CM_EDD_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

#if CM_CFG_USE_GSY
typedef CM_GSY_LOWER_RQB_TYPE  *CM_GSY_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_GSY_LOWER_RQB_TYPE  const *CM_GSY_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */
#endif

typedef CM_NARE_LOWER_RQB_TYPE  *CM_NARE_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_NARE_LOWER_RQB_TYPE  const *CM_NARE_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_OHA_LOWER_RQB_TYPE  *CM_OHA_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_OHA_LOWER_RQB_TYPE  const *CM_OHA_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

typedef CM_RPC_LOWER_RQB_TYPE  *CM_RPC_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_RPC_LOWER_RQB_TYPE  const *CM_RPC_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

#if CM_CFG_USE_MRP
typedef CM_MRP_LOWER_RQB_TYPE  *CM_MRP_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_MRP_LOWER_RQB_TYPE  const *CM_MRP_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */
#endif

#if CM_CFG_USE_POF
typedef CM_POF_LOWER_RQB_TYPE  *CM_POF_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */
typedef CM_POF_LOWER_RQB_TYPE  const *CM_POF_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */
#endif

/*----------------------------------------------------------------------------*/

#define CM_EDD_GET_PPARAM(edd_rqb_ptr, edd_type)	((edd_type)EDD_RQB_GET_PPARAM(edd_rqb_ptr))

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.
@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_acp_request_lower_done (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.
@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_edd_request_lower_done (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


#if CM_CFG_USE_GSY
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.
@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_gsy_request_lower_done (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.
@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_nare_request_lower_done (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.

@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_oha_request_lower_done (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.

@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_rpc_request_lower_done (
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


#if CM_CFG_USE_MRP
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.

@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_mrp_request_lower_done (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif


#if CM_CFG_USE_POF
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Callback function for the responses of the lower layer.
@details
This function is also visible from the outside due to the system connection,
as described in [chapter 15](@ref cm-appendix-b), although they need not be in the "LSA theory".

All confirmations and indications of the lower layer are reported to CM through
this function.

The CM_FATAL function is called if an error situation cannot
be reported to the upper layer.

@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID
cm_pof_request_lower_done (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);
#endif


#if 0 /* not used */
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Asynchronous allocation of a lower-RQBr.
@details NOT USED

@param [in] user_id       user id
@param [in] length  alloc length
@param [in] lower_rqb_ptr Pointer to request block of the layer below
******************************************************************************/
LSA_VOID cm_lower_rqb(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    CM_LOWER_RQB_PTR_TYPE   lower_rqb_ptr
);
#endif


#if 0 /* not used */
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Asynchronous allocation of a lower-RQBr.
@details NOT USED

@param [in] user_id       user id
@param [in] length  alloc length
@param [in] lower_mem_ptr Pointer to memory of the layer below
******************************************************************************/
LSA_VOID cm_lower_mem(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    CM_LOWER_MEM_PTR_TYPE   lower_mem_ptr
);
#endif


/*====  out functions  =====*/


#ifndef CM_ACP_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_OPEN_CHANNEL_LOWER(
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_EDD_OPEN_CHANNEL_LOWER(
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_GSY_OPEN_CHANNEL_LOWER(
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#ifndef CM_NARE_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_NARE_OPEN_CHANNEL_LOWER(
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_OHA_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_OHA_OPEN_CHANNEL_LOWER(
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_RPC_OPEN_CHANNEL_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_MRP_OPEN_CHANNEL_LOWER(
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_OPEN_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Open a communication channel to lower layer.
@details
Sends the open-channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_POF_OPEN_CHANNEL_LOWER(
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif


#ifndef CM_ACP_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_CLOSE_CHANNEL_LOWER (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_EDD_CLOSE_CHANNEL_LOWER (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_GSY_CLOSE_CHANNEL_LOWER (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#ifndef CM_NARE_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_NARE_CLOSE_CHANNEL_LOWER (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_OHA_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_OHA_CLOSE_CHANNEL_LOWER (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_RPC_CLOSE_CHANNEL_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_MRP_CLOSE_CHANNEL_LOWER (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_CLOSE_CHANNEL_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Closes the communication channel of the lower layer.
@details
Sends the close channel RQB to the lower layer. Refer also to chapter CM channels.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_POF_CLOSE_CHANNEL_LOWER (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif


#ifndef CM_ACP_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_REQUEST_LOWER (
    CM_ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_EDD_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_EDD_REQUEST_LOWER (
    CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_GSY_REQUEST_LOWER (
    CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#ifndef CM_NARE_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_NARE_REQUEST_LOWER (
    CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_OHA_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_OHA_REQUEST_LOWER (
    CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#ifndef CM_RPC_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_RPC_REQUEST_LOWER(
    CM_RPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_MRP_REQUEST_LOWER (
    CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_REQUEST_LOWER
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Transfers an order to the lower layer.
@details
Send a request to a communication channel to lower layer.

@param [in] lower_rqb_ptr pointer to request block of the layer below
@param [in] sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_POF_REQUEST_LOWER (
    CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


#ifndef CM_ACP_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_ACP_ALLOC_LOWER_RQB(
    CM_ACP_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_EDD_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_EDD_ALLOC_LOWER_RQB(
    CM_EDD_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_GSY_ALLOC_LOWER_RQB(
    CM_GSY_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_NARE_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_NARE_ALLOC_LOWER_RQB(
    CM_NARE_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE             user_id,
    LSA_UINT16                   length,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef CM_OHA_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_OHA_ALLOC_LOWER_RQB(
    CM_OHA_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_RPC_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_RPC_ALLOC_LOWER_RQB(
    CM_RPC_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_MRP_ALLOC_LOWER_RQB(
    CM_MRP_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_ALLOC_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting a request block with the memory attributes of the lower layer.
@details
Alignment according to request of the processor, e.g., 32-bit in case of TriCore.

The memory should be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_rqb_ptr_ptr pointer to request block of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_POF_ALLOC_LOWER_RQB(
    CM_POF_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


#ifndef CM_ACP_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_ACP_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_ACP_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_EDD_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_EDD_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_EDD_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_EDD_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_GSY_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_GSY_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_GSY_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_NARE_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_NARE_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_NARE_FREE_LOWER_RQB(
    LSA_UINT16                 * ret_val_ptr,
    CM_NARE_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef CM_OHA_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_OHA_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_OHA_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_OHA_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_RPC_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_RCP_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_RPC_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_RPC_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_MRP_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_MRP_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_MRP_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_FREE_LOWER_RQB
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a request block requested with CM_POF_ALLOC_LOWER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).
Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_rqb_ptr pointer to request block of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_POF_FREE_LOWER_RQB(
    LSA_UINT16                * ret_val_ptr,
    CM_POF_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


#ifndef CM_ACP_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_ACP_ALLOC_LOWER_MEM(
    CM_ACP_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_ACP_ALLOC_ALARM_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer for alarms.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_alarm_ptr_ptr pointer to alarm of the layer below
@param [in]  length              alloc length
@param [in]  sys_ptr             system pointer
******************************************************************************/
LSA_VOID
CM_ACP_ALLOC_ALARM_MEM(
    CM_ACP_LOWER_MEM_PTR_TYPE * lower_alarm_ptr_ptr,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_EDD_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_EDD_ALLOC_LOWER_MEM(
    CM_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_GSY_ALLOC_LOWER_MEM(
    CM_GSY_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_NARE_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_NARE_ALLOC_LOWER_MEM(
    CM_NARE_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE             user_id,
    LSA_UINT16                   length,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef CM_OHA_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_OHA_ALLOC_LOWER_MEM(
    CM_OHA_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_RPC_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_RPC_ALLOC_LOWER_MEM(
    CM_RPC_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_MRP_ALLOC_LOWER_MEM(
    CM_MRP_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_ALLOC_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the lower layer.
@details
Alignment according the request of the processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface of the lower layer.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *lower_mem_ptr_ptr pointer to memory of the layer below
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
LSA_VOID
CM_POF_ALLOC_LOWER_MEM(
    CM_POF_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE            user_id,
    LSA_UINT16                  length,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


#ifndef CM_ACP_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_ACP_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_ACP_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_ACP_FREE_ALARM_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested with CM_ACP_ALLOC_ALARM_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_ACP_FREE_ALARM_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_ACP_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_EDD_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_EDD_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_EDD_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_EDD_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_GSY
#ifndef CM_GSY_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_GSY_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_GSY_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_GSY_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#ifndef CM_NARE_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_NARE_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_NARE_FREE_LOWER_MEM(
    LSA_UINT16                 * ret_val_ptr,
    CM_NARE_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

#ifndef CM_OHA_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_OHA_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_OHA_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_OHA_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#ifndef CM_RPC_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_RPC_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_RPC_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_RPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

#if CM_CFG_USE_MRP
#ifndef CM_MRP_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_MRP_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_MRP_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_MRP_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif

#if CM_CFG_USE_POF
#ifndef CM_POF_FREE_LOWER_MEM
/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a memory requested by CM_POF_ALLOC_LOWER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr  return value
@param [in]  lower_mem_ptr pointer to memory of the layer below
@param [in]  sys_ptr       system pointer
******************************************************************************/
LSA_VOID
CM_POF_FREE_LOWER_MEM(
    LSA_UINT16                * ret_val_ptr,
    CM_POF_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
);
#endif
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_LOW_H */
