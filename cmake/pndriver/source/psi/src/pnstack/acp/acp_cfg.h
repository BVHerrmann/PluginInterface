#ifndef ACP_CFG_H
#define ACP_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: acp_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component ACP                                  */
/*  Using the PSI framework.                                              */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "acp_inc.h"
 */

LSA_VOID acp_psi_startstop (LSA_INT start);

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define ACP_DEBUG  PSI_DEBUG

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/


/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// the lower layer
//----------------------------------------------------------------------------*/

#define ACP_LOWER_RQB_TYPE						EDD_RQB_TYPE

#define ACP_LL_GET_NEXT_RQB_PTR(pRQB)			EDD_RQB_GET_NEXT_RQB_PTR (pRQB)
#define ACP_LL_GET_PREV_RQB_PTR(pRQB)			EDD_RQB_GET_PREV_RQB_PTR (pRQB)

#define ACP_LL_SET_OPCODE(pRQB, value)			EDD_RQB_SET_OPCODE (pRQB, value)
#define ACP_LL_GET_OPCODE(pRQB)					EDD_RQB_GET_OPCODE (pRQB)

#define ACP_LL_SET_SERVICE(pRQB, value)			EDD_RQB_SET_SERVICE (pRQB, value)
#define ACP_LL_GET_SERVICE(pRQB)				EDD_RQB_GET_SERVICE (pRQB)

#define ACP_LL_SET_HANDLE(pRQB, value)			EDD_RQB_SET_HANDLE (pRQB, value)
#define ACP_LL_GET_HANDLE(pRQB)					EDD_RQB_GET_HANDLE (pRQB)

#define ACP_LL_SET_RESPONSE(pRQB, value)		EDD_RQB_SET_RESPONSE (pRQB, value)
#define ACP_LL_GET_RESPONSE(pRQB)				EDD_RQB_GET_RESPONSE (pRQB)

#define ACP_LL_SET_USERID_PTR(pRQB, value)		EDD_RQB_SET_USERID_PTR (pRQB, value)
#define ACP_LL_GET_USERID_PTR(pRQB)				EDD_RQB_GET_USERID_PTR (pRQB)

#define ACP_LL_SET_PPARAM(pRQB, value)			EDD_RQB_SET_PPARAM (pRQB, value)
#define ACP_LL_GET_PPARAM(pRQB, type)			((type)(EDD_RQB_GET_PPARAM (pRQB)))


/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define ACP_RQB_HEADER						LSA_RQB_HEADER(ACP_UPPER_RQB_PTR_TYPE)

#define ACP_RQB_TRAILER						LSA_RQB_TRAILER

#define ACP_RQB_SET_NEXT_RQB_PTR(rb,v)		LSA_RQB_SET_NEXT_RQB_PTR(rb,v)
#define ACP_RQB_SET_PREV_RQB_PTR(rb,v)		LSA_RQB_SET_PREV_RQB_PTR(rb,v)
#define ACP_RQB_SET_OPCODE(rb,v)			LSA_RQB_SET_OPCODE(rb,v)
#define ACP_RQB_SET_HANDLE(rb,v)			LSA_RQB_SET_HANDLE(rb,v)
#define ACP_RQB_SET_RESPONSE(rb,v)			LSA_RQB_SET_RESPONSE(rb,v)
#define ACP_RQB_SET_USER_ID_PTR(rb,v)		LSA_RQB_SET_USER_ID_PTR(rb,v)

#define ACP_RQB_GET_NEXT_RQB_PTR(rb)		LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define ACP_RQB_GET_PREV_RQB_PTR(rb)		LSA_RQB_GET_PREV_RQB_PTR(rb)
#define ACP_RQB_GET_OPCODE(rb)				LSA_RQB_GET_OPCODE(rb)
#define ACP_RQB_GET_HANDLE(rb)				LSA_RQB_GET_HANDLE(rb)
#define ACP_RQB_GET_RESPONSE(rb)			LSA_RQB_GET_RESPONSE(rb)
#define ACP_RQB_GET_USER_ID_PTR(rb)			LSA_RQB_GET_USER_ID_PTR(rb)


/*------------------------------------------------------------------------------
// maximum number of open LSA channels
//----------------------------------------------------------------------------*/

/* System / UserIOC / UserIOD */
#define ACP_CFG_MAX_CHANNELS  (1 + PSI_CFG_USE_IOC + PSI_CFG_USE_IOD)*(EDD_CFG_MAX_INTERFACE_CNT)

/*------------------------------------------------------------------------------
// enable / disable cast-warnings
//----------------------------------------------------------------------------*/

# define ACP_CFG_CAST_WARN  0

/*------------------------------------------------------------------------------
// enable / disable ACP_ASSERT
//----------------------------------------------------------------------------*/

#if ACP_DEBUG

	/* check for condition: internal programming error */

# define ACP_ASSERT(cond) \
	if (! (cond)) { ACP_FATAL (0); }
#else

	/* no more programming error exists :-) */
# define ACP_ASSERT(cond)

#endif


/*------------------------------------------------------------------------------
// ACP_STATIC_ASSERT
//  Does an ASSERT at compile time.
//  With Visual Studio 2010 you can use:   static_assert(expr_, msg_)
//  with C11 you can use:                  _Static_assert(expr_, msg_)
//  with cpp magic you can use:            { extern char _compile_time_assert[expr_]; }
//  else use runtime assert:               ACP_ASSERT(expr_)
//----------------------------------------------------------------------------*/

#ifndef ACP_STATIC_ASSERT
/* define ACP_STATIC_ASSERT(expr_, msg_)    static_assert(expr_, msg_) */
#  define ACP_STATIC_ASSERT(expr_, msg_)    ACP_ASSERT(expr_) /* use runtime assert */
#endif


/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef ACP_FILE_SYSTEM_EXTENSION
# define ACP_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif


/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace [default]
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/

#define ACP_CFG_TRACE_MODE  PSI_CFG_TRACE_MODE

/*- alloc mem macros----------------------------------------------------------*/

#define ACP_ALLOC_UPPER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_RQB)
#define ACP_ALLOC_UPPER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_MEM)
#define ACP_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_RQB_EDD)
#define ACP_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_MEM_EDD)
#define ACP_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM)

#define ACP_FREE_UPPER_RQB(ret_val_ptr, upper_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_RQB)
#define ACP_FREE_UPPER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_MEM)
#define ACP_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_RQB_EDD)
#define ACP_FREE_LOWER_MEM(ret_val_ptr, lower_mem_ptr, sys_ptr )    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_mem_ptr), (sys_ptr), LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_MEM_EDD)
#define ACP_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)              PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_CFG_H */
