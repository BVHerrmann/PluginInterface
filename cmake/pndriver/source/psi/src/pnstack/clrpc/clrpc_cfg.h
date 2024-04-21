#ifndef CLRPC_CFG_H
#define CLRPC_CFG_H

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
/*  F i l e               &F: clrpc_cfg.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CLRPC using PSI.                        */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "clrpc_inc.h"
 */

void clrpc_psi_startstop (int start);

#ifndef LSA_HOST_ENDIANESS_LITTLE
# error "LSA_HOST_ENDIANESS_LITTLE not defined?"
#endif

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define CLRPC_DEBUG PSI_DEBUG

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*===========================================================================*/
/* CLRPC-header and CLRPC-trailer                                            */
/*===========================================================================*/

#if ! defined(LSA_RQB_HEADER) || ! defined(LSA_RQB_TRAILER)
# error "LSA_RQB_HEADER or LSA_RQB_TRAILER not defined in lsa_cfg.h"
#endif


#define CLRPC_RQB_HEADER \
	LSA_RQB_HEADER(CLRPC_UPPER_RQB_PTR_TYPE) /* standard LSA-header comes first */ \
	/* end of CLRPC_RQB_HEADER */


#define CLRPC_RQB_TRAILER \
	LSA_RQB_TRAILER /* standard LSA-trailer comes last */ \
	/* end of CLRPC_RQB_TRAILER */


#define CLRPC_RQB_SET_NEXT_RQB_PTR(rb_, val_)		LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define CLRPC_RQB_GET_NEXT_RQB_PTR(rb_)				LSA_RQB_GET_NEXT_RQB_PTR(rb_)

#define CLRPC_RQB_SET_PREV_RQB_PTR(rb_, val_)		LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define CLRPC_RQB_GET_PREV_RQB_PTR(rb_)				LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define CLRPC_RQB_SET_OPCODE(rb_, val_)				LSA_RQB_SET_OPCODE(rb_, val_)
#define CLRPC_RQB_GET_OPCODE(rb_)					LSA_RQB_GET_OPCODE(rb_)

#define CLRPC_RQB_SET_HANDLE(rb_, val_)				LSA_RQB_SET_HANDLE(rb_, val_)
#define CLRPC_RQB_GET_HANDLE(rb_)					LSA_RQB_GET_HANDLE(rb_)

#define CLRPC_RQB_SET_RESPONSE(rb_, val_)			LSA_RQB_SET_RESPONSE(rb_, val_)
#define CLRPC_RQB_GET_RESPONSE(rb_)					LSA_RQB_GET_RESPONSE(rb_)

#define CLRPC_RQB_SET_USER_ID_PTR(rb_, val_)		LSA_RQB_SET_USER_ID_PTR(rb_, val_)
#define CLRPC_RQB_GET_USER_ID_PTR(rb_)				LSA_RQB_GET_USER_ID_PTR(rb_)


/*===========================================================================*/
/* LOWER LAYER MACROS                                                        */
/*===========================================================================*/

#define CLRPC_LL_INVALID_HANDLE ((LSA_HANDLE_TYPE)-1) /*check this - NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */
#define CLRPC_LL_INVALID_SOCKET ((LSA_INT32)-1) /*check this - NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

#define CLRPC_LOWER_RQB_TYPE  SOCK_RQB_TYPE

#define CLRPC_LL_SET_NEXT_RQB_PTR(rb_, val_)	SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define CLRPC_LL_SET_PREV_RQB_PTR(rb_, val_)	SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define CLRPC_LL_SET_OPCODE(rb_, val_)			SOCK_RQB_SET_OPCODE(rb_, val_)
#define CLRPC_LL_SET_HANDLE(rb_, val_)			SOCK_RQB_SET_HANDLE(rb_, val_)
#define CLRPC_LL_SET_RESPONSE(rb_, val_)		SOCK_RQB_SET_RESPONSE(rb_, val_)
#define CLRPC_LL_SET_USER_ID_PTR(rb_, val_)		SOCK_RQB_SET_USER_ID_PTR(rb_, val_)

#define CLRPC_LL_GET_NEXT_RQB_PTR(rb_)			SOCK_RQB_GET_NEXT_RQB_PTR(rb_)
#define CLRPC_LL_GET_PREV_RQB_PTR(rb_)			SOCK_RQB_GET_PREV_RQB_PTR(rb_)
#define CLRPC_LL_GET_OPCODE(rb_)				SOCK_RQB_GET_OPCODE(rb_)
#define CLRPC_LL_GET_HANDLE(rb_)				SOCK_RQB_GET_HANDLE(rb_)
#define CLRPC_LL_GET_RESPONSE(rb_)				SOCK_RQB_GET_RESPONSE(rb_)
#define CLRPC_LL_GET_USER_ID_PTR(rb_)			SOCK_RQB_GET_USER_ID_PTR(rb_)


/*=============================================================================
 *       set local_mem static or dynamic:
 * static (no use of CLRPC_ALLOC_LOCAL_MEM):
 * #define CLRPC_LOCAL_MEM_STATIC
 * or dynamic (use of CLRPC_ALLOC_LOCAL_MEM):
 * #define CLRPC_LOCAL_MEM_DYNAMIC
 *===========================================================================*/

#define CLRPC_LOCAL_MEM_DYNAMIC /* don't change. there is no choice! */


/*=============================================================================
 *       clear global data:
 * clear (CLRPC_DATA_TYPE clrpc_data = {0};):
 * #define CLRPC_CLEAR_GLOBAL_DATA
 * or no clear:
 * #define CLRPC_NO_CLEAR_GLOBAL_DATA
 *===========================================================================*/

#define CLRPC_NO_CLEAR_GLOBAL_DATA /* don't change. there is no choice! */


/*===========================================================================*/
/* configurable things                                                       */
/*===========================================================================*/

#define CLRPC_CFG_MAX_CONTEXTS                      (PSI_CFG_CLRPC_CFG_MAX_CONTEXTS)

	/* 1 = use casts to avoid some warnings
	//-----------------------------------------------------------*/
#define CLRPC_CFG_USE_CASTS                         (PSI_CFG_CLRPC_CFG_USE_CASTS)

	/* configure the number of communication channels
	//-----------------------------------------------------------*/
#define CLRPC_CFG_MAX_CHANNELS                      (PSI_CFG_CLRPC_CFG_MAX_CHANNELS) 

	/* configure the client part
	//-----------------------------------------------------------*/
#define CLRPC_CFG_MAX_CLIENTS                       (PSI_CFG_CLRPC_CFG_MAX_CLIENTS)

	/* configure the server part
	// max number of interfaces
	//	3: Endpoint-Mapper-If, IPNIO-Controller-If, IPNIO-Server-If
	//-----------------------------------------------------------*/
#define CLRPC_CFG_MAX_SERVERS                       (PSI_CFG_CLRPC_CFG_MAX_SERVERS)


	/* configure the server part
	// max number of 'workers' (for all interfaces)
	//  e.g. _MAX_CL_DEVICES + _MAX_SV_DEVICES * (_MAX_AR + _MAX_SV_READ_IMPL)
	//-----------------------------------------------------------*/
#define CLRPC_CFG_MAX_SCALLS                        (PSI_CFG_CLRPC_CFG_MAX_SCALLS)


	/* configure the server part
	// max number of read implicit per object_uuid
	//	valid for all interfaces with pnio-signature
	//	0: no read implicit allowed
	//	n: n parallel read implicits are allowed on every registered object-uuid
	//-----------------------------------------------------------*/
#define CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID    (PSI_CFG_CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID)

	/* configure the integer data representation
	 * the definition must reflect the hardware. there is no choice!
	//-----------------------------------------------------------*/
#ifndef CLRPC_CFG_DREP_INT

#ifdef LSA_HOST_ENDIANESS_LITTLE
#define CLRPC_CFG_DREP_INT  CLRPC_DREP_INT_LITTLE /* see clrpc_usr.h, CLRPC_DREP_INT_XXX */
#endif

#ifdef LSA_HOST_ENDIANESS_BIG
#define CLRPC_CFG_DREP_INT  CLRPC_DREP_INT_BIG /* see clrpc_usr.h, CLRPC_DREP_INT_XXX */
#endif

#endif

	/* configure the character data representation
	 * the definition must reflect the hardware. there is no choice!
	//-----------------------------------------------------------*/
#ifndef CLRPC_CFG_DREP_CHAR
#define CLRPC_CFG_DREP_CHAR  CLRPC_DREP_CHAR_ASCII /* see clrpc_usr.h, CLRPC_DREP_CHAR_XXX */
#endif

	/* configure the floating-point data representation
	 * the definition must reflect the hardware. there is no choice!
	//-----------------------------------------------------------*/
#ifndef CLRPC_CFG_DREP_FLOAT
#define CLRPC_CFG_DREP_FLOAT  CLRPC_DREP_FLOAT_IEEE /* see clrpc_usr.h, CLRPC_DREP_FLOAT_XXX */
#endif


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*------------------------------------------------------------------------------
//	Assert
//
//  ==> must be empty in Release-Version!
//----------------------------------------------------------------------------*/

#if CLRPC_DEBUG

	/* check if condition holds or call fatal (programming error) */
#define CLRPC_ASSERT(cond_) { \
	if (!(cond_)) { \
		CLRPC_FATAL1(0); \
	} \
	}

#else

#define CLRPC_ASSERT(cond_) { \
	/* empty-block in release version */ \
	}

#endif


/*------------------------------------------------------------------------------
// Test a pointer
//
//  ==> must be empty in Release-Version!
//----------------------------------------------------------------------------*/

#if CLRPC_DEBUG

#define CLRPC_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */

#define CLRPC_TEST_ALIGN2(ptr_)   (PSI_TEST_ALIGN2(ptr_)) /* test for alignment */
#define CLRPC_TEST_ALIGN4(ptr_)   (PSI_TEST_ALIGN4(ptr_)) /* test for alignment */

#else

#define CLRPC_TEST_POINTER(ptr_)  (ptr_) /* no test */

#define CLRPC_TEST_ALIGN2(ptr_)   (ptr_) /* no test */
#define CLRPC_TEST_ALIGN4(ptr_)   (ptr_) /* no test */

#endif


/*------------------------------------------------------------------------------
//	System Layer (prototypes in clrpc_sys.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in clrpc_cfg.c as an example
 */

/*------------------------------------------------------------------------------
//	Upper Layer (prototypes in clrpc_usr.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in clrpc_cfg.c as an example
 */

/*------------------------------------------------------------------------------
//	Lower Layer (prototypes in clrpc_low.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in clrpc_cfg.c as an example
 */

/*------------------------------------------------------------------------------
//	Library Functions (prototypes in clrpc_lib.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in clrpc_cfg_lib.c as an example
 */

 /*------------------------------------------------------------------------------
 //	alloc mem macros
 //----------------------------------------------------------------------------*/
#define CLRPC_ALLOC_UPPER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_RQB)
#define CLRPC_ALLOC_UPPER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_MEM)
#define CLRPC_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_RQB_SOCK)
#define CLRPC_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_MEM_SOCK)
#define CLRPC_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                      PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM)

#define CLRPC_FREE_UPPER_RQB(ret_val_ptr, upper_rqb_ptr, sys_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_RQB)
#define CLRPC_FREE_UPPER_MEM(ret_val_ptr, mem_ptr, sys_ptr)             PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_MEM)
#define CLRPC_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_RQB_SOCK)
#define CLRPC_FREE_LOWER_MEM(ret_val_ptr, lower_mem_ptr, sys_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_mem_ptr), (sys_ptr), LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_MEM_SOCK)
#define CLRPC_FREE_LOCAL_MEM(ret_val_ptr, mem_ptr)                      PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM)

/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 = no LSA trace
//	1 = LSA trace
//  2 = LSA indexed trace
//----------------------------------------------------------------------------*/
#define CLRPC_CFG_TRACE_MODE        (PSI_CFG_TRACE_MODE)

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef CLRPC_FILE_SYSTEM_EXTENSION
# define CLRPC_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_CFG_H */
