#ifndef TCIP_PNIO_CFG_H                       /* ----- reinclude-protection ----- */
#define TCIP_PNIO_CFG_H

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
/*  F i l e               &F: tcip_pnio_cfg.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration module                                                     */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*=== pcIOX configuration (do not copy/paste it) ============================*/
/*===========================================================================*/

void tcip_psi_startstop(int start);

#define TCIP_DEBUG  (PSI_DEBUG)

/* sado - internal performance tracing */
#define TCIP_INT_CFG_TRACE_ON 0 /* 1 enables performance trace */

/*===========================================================================*/
/*===========================================================================*/
/*----------------------------------------------------------------------------*/
/* interface to LSA trace                                                     */
/* 0: no traces                                                               */
/* 1: enable LSA Traces                                                       */
/* 2: enable LSA Idx Traces                                                   */
//----------------------------------------------------------------------------*/
#define TCIP_CFG_TRACE_MODE             (PSI_CFG_TRACE_MODE)

#ifndef TCIP_FILE_SYSTEM_EXTENSION
# define TCIP_FILE_SYSTEM_EXTENSION(module_id_)  /* Currently not supported */
#endif

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_COPY_ON_SEND   (PSI_CFG_TCIP_CFG_COPY_ON_SEND) //Using COPY IF for TCIP

#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
    #define TCIP_CFG_INTERNICHE     0
    #define TCIP_CFG_OPEN_BSD       1
#endif

#if (PSI_CFG_TCIP_STACK_INTERNICHE == 1)
    #define TCIP_CFG_INTERNICHE     1
    #define TCIP_CFG_OPEN_BSD       0
#endif

#if (PSI_CFG_TCIP_STACK_WINSOCK == 1)
    // Note: There is no TCIP_CFG_WINSOCK
    #define TCIP_CFG_INTERNICHE     0
    #define TCIP_CFG_OPEN_BSD       0
#endif

#if (PSI_CFG_TCIP_STACK_CUSTOM == 1)
    // Note: There is no TCIP_CFG_CUSTOM
    #define TCIP_CFG_INTERNICHE     0
    #define TCIP_CFG_OPEN_BSD       0
#endif 

/*===========================================================================*/
/*===========================================================================*/

#if defined(LSA_HOST_ENDIANESS_BIG)
#define TCIP_CFG_BIG_ENDIAN	 1 /* 0 == little endian, 1 == big endian */
#endif

#if defined(LSA_HOST_ENDIANESS_LITTLE)
#define TCIP_CFG_BIG_ENDIAN	 0
#endif

/*===========================================================================*/
/*===========================================================================*/
#define TCIP_CFG_MAX_SOCKETS            (PSI_CFG_MAX_SOCKETS)

#define TCIP_CFG_ENABLE_MULTICAST       (PSI_CFG_TCIP_CFG_ENABLE_MULTICAST)

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_QUEUE_CHECK_ENABLE  0 /* see macro QUEUE_CHECK */

#define TCIP_CFG_SOC_CHECK_ENABLE    0 /* see macro SOC_CHECK */

#define TCIP_CFG_SOC_CHECK_MORE      0 /* 1 enables SOC_CHECK in t_recv(), t_send(), t_select() */

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_SNMP_ON 1 /* enable SNMP services */

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_KEEPALIVE_IDLE_TIMEOUT		30 /* TCP Keep-Alive Idle Time in seconds; for default value undefine the parameter */
#define TCIP_CFG_KEEPALIVE_PROBE_INTERVAL	1  /* TCP Keep-Alive Interval in seconds; for default value undefine the parameter */
#define TCIP_CFG_KEEPALIVE_PROBE_COUNT		5  /* TCP Keep-Alive Probe Count; for default value undefine the parameter */

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_DEBUG
#ifdef _lint
#define TCIP_TEST_POINTER(ptr_) (ptr_) /* empty for Lint */
#else
#define TCIP_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */
#endif

/* check if condition holds or call fatal (programming error) */
#define TCIP_ASSERT(cond_) { if (!(cond_)) { TCIP_FATAL(); } } /*lint -unreachable */

#else

#define TCIP_TEST_POINTER(ptr_)  (ptr_) /* don't test */

#define TCIP_ASSERT(cond_) { /* empty-block in release version */ }

#endif

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*
 * TCIP-HEADER and TCIP-TRAILER
 */

#if ! defined(LSA_RQB_HEADER) || ! defined(LSA_RQB_TRAILER)
# error "LSA_RQB_HEADER or LSA_RQB_TRAILER not defined in lsa_cfg.h"
#endif


#define TCIP_RQB_HEADER \
	LSA_RQB_HEADER(TCIP_UPPER_RQB_PTR_TYPE) /* standard LSA-header comes first */ \
	/* end of TCIP_RQB_HEADER */


#define TCIP_RQB_TRAILER \
	LSA_RQB_TRAILER /* standard LSA-trailer comes last */ \
	/* end of TCIP_RQB_TRAILER */


#define TCIP_RQB_SET_NEXT_RQB_PTR(rb_, val_)	LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define TCIP_RQB_GET_NEXT_RQB_PTR(rb_)			LSA_RQB_GET_NEXT_RQB_PTR(rb_)

#define TCIP_RQB_SET_PREV_RQB_PTR(rb_, val_)	LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define TCIP_RQB_GET_PREV_RQB_PTR(rb_)			LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define TCIP_RQB_SET_OPCODE(rb_, val_)			LSA_RQB_SET_OPCODE(rb_, val_)
#define TCIP_RQB_GET_OPCODE(rb_)				LSA_RQB_GET_OPCODE(rb_)

#define TCIP_RQB_SET_HANDLE(rb_, val_)			LSA_RQB_SET_HANDLE(rb_, val_)
#define TCIP_RQB_GET_HANDLE(rb_)				LSA_RQB_GET_HANDLE(rb_)

#define	TCIP_RQB_SET_USER_ID(rb_, val_)			LSA_RQB_SET_USER_ID(rb_, val_)
#define	TCIP_RQB_GET_USER_ID(rb_)				LSA_RQB_GET_USER_ID(rb_)

#define TCIP_RQB_SET_USER_ID_PTR(rb_, val_)		LSA_RQB_SET_USER_ID_PTR(rb_, val_)
#define TCIP_RQB_GET_USER_ID_PTR(rb_)			LSA_RQB_GET_USER_ID_PTR(rb_)

#define TCIP_RQB_SET_RESPONSE(rqb_, v_)			LSA_RQB_SET_RESPONSE(rqb_, v_)
#define TCIP_RQB_GET_RESPONSE(rqb_)				LSA_RQB_GET_RESPONSE(rqb_)

/*===========================================================================*/
/*===========================================================================*/

/*
 * lower layer macros for EDD
 */

#define TCIP_EDD_LOWER_RQB_TYPE	EDD_RQB_TYPE

#define TCIP_EDD_SET_NEXT_RQB_PTR(rb_, val_)	EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define TCIP_EDD_SET_PREV_RQB_PTR(rb_, val_)	EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define TCIP_EDD_SET_OPCODE(rb_, opc_, svc_)	{ EDD_RQB_SET_OPCODE(rb_, opc_); EDD_RQB_SET_SERVICE(rb_, svc_); }
#define TCIP_EDD_SET_HANDLE(rb_, val_)			EDD_RQB_SET_HANDLE(rb_, val_)
#define TCIP_EDD_SET_USER_ID_PTR(rb_, val_)		EDD_RQB_SET_USERID_PTR(rb_, val_)
#define TCIP_EDD_SET_RESPONSE(rb_, val_)		EDD_RQB_SET_RESPONSE(rb_, val_)

#define TCIP_EDD_GET_NEXT_RQB_PTR(rb_)			EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define TCIP_EDD_GET_PREV_RQB_PTR(rb_)			EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define TCIP_EDD_GET_OPCODE(rb_)				EDD_RQB_GET_OPCODE(rb_)
#define TCIP_EDD_GET_SERVICE(rb_)				EDD_RQB_GET_SERVICE(rb_)
#define TCIP_EDD_GET_HANDLE(rb_)				EDD_RQB_GET_HANDLE(rb_)
#define TCIP_EDD_GET_USER_ID_PTR(rb_)			EDD_RQB_GET_USERID_PTR(rb_)
#define TCIP_EDD_GET_RESPONSE(rb_)				EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*- alloc mem macros----------------------------------------------------------*/

#define TCIP_ALLOC_UPPER_RQB(upper_rqb_ptr_ptr, user_id, length, sys_ptr)       PSI_ALLOC_LOCAL_MEM((upper_rqb_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_UPPER_RQB)
#define TCIP_EDD_ALLOC_LOWER_RQB(lower_rqb_ptr_ptr, user_id, length, sys_ptr)   PSI_ALLOC_LOCAL_MEM((lower_rqb_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_RQB_EDD)
#define TCIP_EDD_ALLOC_LOWER_MEM(lower_mem_ptr_ptr, user_id, length, sys_ptr)   PSI_ALLOC_LOCAL_MEM((lower_mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_MEM_EDD)

#define TCIP_FREE_UPPER_RQB(ret_val_ptr, upper_rqb_ptr, sys_ptr)                PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_UPPER_RQB)
#define TCIP_EDD_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)            PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_RQB_EDD)
#define TCIP_EDD_FREE_LOWER_MEM(ret_val_ptr, lower_mem_ptr, sys_ptr)            PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_mem_ptr), (sys_ptr), LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_MEM_EDD)


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_CFG_H (reinclude protection) */
