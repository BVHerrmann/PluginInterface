#ifndef ACP_LOW_H
#define ACP_LOW_H

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
/*  F i l e               &F: acp_low.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Lower component interface                                                */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define ACP_LOWER_RQB_PTR_TYPE                  /* pointer to lower-RQB */ \
    ACP_LOWER_RQB_TYPE     *

#define ACP_LOWER_RQB_CONST_PTR_TYPE	ACP_LOWER_RQB_PTR_TYPE	const

typedef EDD_RQB_OPEN_CHANNEL_TYPE					* ACP_LL_OPEN_CHANNEL_PTR_TYPE;
typedef EDD_RQB_GET_PARAMS_TYPE						* ACP_LL_SRT_PARAM_PTR_TYPE;

typedef EDD_RQB_NRT_SEND_TYPE					  * ACP_LL_NRT_SEND_PTR_TYPE;
typedef EDD_RQB_NRT_RECV_TYPE					  * ACP_LL_NRT_RECV_PTR_TYPE;
typedef EDD_RQB_NRT_CANCEL_TYPE					  * ACP_LL_NRT_CANCEL_PTR_TYPE;


typedef union {
	EDD_RQB_OPEN_CHANNEL_TYPE  ch_op;
	EDD_RQB_GET_PARAMS_TYPE  ch_para;

	EDD_RQB_NRT_SEND_TYPE  nrt_send;
	EDD_RQB_NRT_RECV_TYPE  nrt_recv;
	EDD_RQB_NRT_CANCEL_TYPE  nrt_cancel;

} ACP_LL_USED_DATA_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Callback function of ACP for the lower layer.
@details
LSA function, synchronous

This is the callback function of the lower LSA layer of ACP (LOWER LAYER). The pointer to this function is also specified in ACP_OPEN_CHANNEL_LOWER.\n
All results and indications of the lower layer are reported through this function.

Request: All

@param lower_rqb_ptr Pointer on request block of the lower layer
******************************************************************************/
LSA_VOID acp_request_lower_done (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

#if 0 /* LSA-function, not used */
/*=============================================================================
 * function name:  acp_lower_rqb
 *
 * function:       asynchronous allocation of a lower-RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of RQB
 *                 ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID acp_lower_rqb (
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  acp_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of memory
 *                 ACP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID acp_lower_mem (
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_LOWER_MEM_PTR_TYPE     lower_mem_ptr
);
#endif

/*====  out functions  =====*/

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Opens a channel with the lower layer.
@details
Asynchronous

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_OPEN_CHANNEL_LOWER
LSA_VOID ACP_OPEN_CHANNEL_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Closes the channel with the lower layer.
@details
Asynchronous

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_CLOSE_CHANNEL_LOWER
LSA_VOID ACP_CLOSE_CHANNEL_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Transfers an asynchronous request to the lower layer.
@details
Asynchronous

@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_REQUEST_LOWER
LSA_VOID ACP_REQUEST_LOWER (
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocates a request block for the lower layer.
@details
Synchronous

@param [out] lower_rqb_ptr_ptr  pointer to lower-RQB-pointer or\n
                                LSA_NULL: The allocation will be asynchronous, see acp_lower_rqb().
@param [in] user_id             id of acp
@param [in] length              length of RQB
@param [in] sys_ptr             system-pointer
******************************************************************************/
#ifndef ACP_ALLOC_LOWER_RQB
LSA_VOID ACP_ALLOC_LOWER_RQB (
    ACP_LOWER_RQB_PTR_TYPE  *  lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT32                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Frees the requested request block.
@details
Synchronous

@param [out] ret_val_ptr    LSA_RET_OK: ok\n
                            LSA_RET_ERR_PARAM: no deallocation because of
                            wrong pointer to lower-RQB or wrong system-pointer
@param [in] lower_rqb_ptr   pointer to lower-RQB
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_FREE_LOWER_RQB
LSA_VOID ACP_FREE_LOWER_RQB (
    LSA_UINT16              *  ret_val_ptr,
    ACP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocates memory for the lower layer.
@details
Synchronous

@param [out] lower_mem_ptr_ptr  pointer to lower-memory-pointer or \n
                                LSA_NULL: the allocation will be asynchronous, see acp_lower_mem().
@param [in] user_id             id of acp
@param [in] length              length of memory
@param [in] sys_ptr             system-pointer
******************************************************************************/
#ifndef ACP_ALLOC_LOWER_MEM
LSA_VOID ACP_ALLOC_LOWER_MEM (
    ACP_LOWER_MEM_PTR_TYPE  *  lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT32                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Frees the requested memory.
@details
Synchronous

@param [out] ret_val_ptr    LSA_RET_OK: ok\n
                            LSA_RET_ERR_PARAM: no deallocation because of wrong pointer lower-memory or wrong system-pointer
@param [in] lower_mem_ptr   pointer to lower-memory
@param [in] sys_ptr         system-pointer
******************************************************************************/
#ifndef ACP_FREE_LOWER_MEM
LSA_VOID ACP_FREE_LOWER_MEM (
    LSA_UINT16              *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE     lower_mem_ptr,
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
#endif  /* of ACP_LOW_H */
