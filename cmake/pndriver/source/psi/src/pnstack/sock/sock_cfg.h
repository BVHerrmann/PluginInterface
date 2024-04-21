#ifndef SOCK_CFG_H
#define SOCK_CFG_H
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
/*  F i l e               &F: sock_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration for LSA to socket-Interface                                */
/*  Defines constants, types and macros for SOCK.                            */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/

void sock_psi_startstop( int start );

#define SOCK_DEBUG  PSI_DEBUG

/*==============================================================================*/

/* -------------------------------------------------------------------------------------*/
/* enables fixing the LSA_SOCK to interniche TCP IP with the lower layer EDD/ EDD-ERTEC */
/* -------------------------------------------------------------------------------------*/

/* Define one of the following switches here:
	SOCK_CFG_INTERNICHE InterNiche Stack
	SOCK_CFG_OPEN_BSD   OpenBSD Stack
	SOCK_CFG_WINSOCK    winsock
	SOCK_CFG_CUSTOM     other stack used to implement the SockIF_xx functions
-----------------------------------------------------------------------------*/

//Note: Only one switch of the folling is allowed:PSI_CFG_TCIP_STACK_INTERNICHE,  PSI_CFG_TCIP_STACK_OPEN_BSD, PSI_CFG_TCIP_STACK_WINSOCK, PSI_CFG_TCIP_STACK_CUSTOM. Sanity is checked in psi_pls.h

#if (PSI_CFG_TCIP_STACK_INTERNICHE == 1)
#define SOCK_CFG_INTERNICHE 1  // Activate Interniche IP-Stack
#define SOCK_CFG_OPEN_BSD   0 /* OBSD_ITGR -- switches for using OpenBSD Stack (AP01559744) */
#define SOCK_CFG_WINSOCK    0
#define SOCK_CFG_CUSTOM     0
/* note: SOCK_CFG_MAX_SOCKETS is derived internally from TCIP_CFG_MAX_SOCKETS */
#endif

#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1) /* OBSD_ITGR -- switches for using OpenBSD Stack (AP01559744) */
#define SOCK_CFG_INTERNICHE 0
#define SOCK_CFG_OPEN_BSD   1 /* OBSD_ITGR -- switches for using OpenBSD Stack (AP01559744) */
#define SOCK_CFG_WINSOCK    0
#define SOCK_CFG_CUSTOM     0
/* note: SOCK_CFG_MAX_SOCKETS is derived internally from TCIP_CFG_MAX_SOCKETS */
#endif

#if (PSI_CFG_TCIP_STACK_WINSOCK == 1)
#define SOCK_CFG_INTERNICHE 0
#define SOCK_CFG_OPEN_BSD   0 /* OBSD_ITGR -- switches for using OpenBSD Stack (AP01559744) */
#define SOCK_CFG_WINSOCK    1
#define SOCK_CFG_CUSTOM     0
#define SOCK_CFG_MAX_SOCKETS    (PSI_CFG_MAX_SOCKETS) /* configure as needed */
#endif

#if (PSI_CFG_TCIP_STACK_CUSTOM == 1)
#define SOCK_CFG_INTERNICHE 0
#define SOCK_CFG_OPEN_BSD   0
#define SOCK_CFG_WINSOCK    0
#define SOCK_CFG_CUSTOM     1 /* Custom stack -- use a tcip stack that is not part of the PNIO delivery. System adaption of PSI needs to implement some output macros to get SOCK to work! */
#endif


#if SOCK_CFG_INTERNICHE

/*=============================================================================
 *       socket functions for Version 1.9 (default) or 2.0
 * which supports IPV6
#define SOCK_CFG_INTERNICHE_V20 1
 *===========================================================================*/
#define SOCK_CFG_INTERNICHE_V20 1

/*=============================================================================
    // If SOCK_CFG_INTERNICHE_DEF_FD is 1, the  SOCKIF_FD_...
    // macros have to be defined
    // If SOCK_CFG_INTERNICHE_DEF_FD is 1, the necessary header files have to
    // be included !
 *===========================================================================*/
#define SOCK_CFG_INTERNICHE_DEF_FD 0

#endif /*SOCK_CFG_INTERNICHE*/

/*============================================================================*/

#if SOCK_CFG_OPEN_BSD
#define SOCK_CFG_CLUSTER_IP_SUPPORTED 1 /* Cluster IP is supported */
#else
#define SOCK_CFG_CLUSTER_IP_SUPPORTED 0 /* Cluster IP is not supported */
#endif

/*============================================================================*/

/*============================================================================*/
/*
 *       set local_mem static or dynamic:
 * static, don't use (no use of SOCK_ALLOC_LOCAL_MEM):
 * #define SOCK_LOCAL_MEM_STATIC
 * dynamic, must be used (use of SOCK_ALLOC_LOCAL_MEM):
 */
#define SOCK_LOCAL_MEM_DYNAMIC

/*============================================================================*/
/*       clear global data:
 * clear, don't use (SOCK_DATA_TYPE sock_data = {0};)
 * #define SOCK_CLEAR_GLOBAL_DATA
 * or no clear:
 */
#define SOCK_NO_CLEAR_GLOBAL_DATA

/*============================================================================*/

/*  Die SOCK_Komponente unterstützt UDP-Dienste     */
#define SOCK_CFG_UDP_ON 1 /* 0 or 1 */

/*============================================================================*/
#define SOCK_CFG_ENABLE_MULTICASTS (PSI_CFG_SOCK_CFG_ENABLE_MULTICASTS)

/* use ANY binding for multicast sockets */
#define SOCK_CFG_BIND_MULTICAST_SOCKETS_TO_ANY 1 /* 0 or 1 */

/*============================================================================*/

/* Die SOCK_Komponente unterstützt SNMP-Dienste */
#define SOCK_CFG_SNMP_ON          1      /* used by OHA (refer to OHA_SOCK_SNMP_OFF) */

/*============================================================================*/

/* SOCK soll DNS - Anfragen unterstützen */
/*    #define SOCK_CFG_DNS_CLIENT 1 */
/* SOCK unterstützt keine DNS - Anfragen  */
#define SOCK_CFG_DNS_CLIENT 0 /* not yet supported */

/*============================================================================*/
/*============================================================================*/

/* Maximum amount of LSA channels (number of SOCK_CHANNEL data structures) */
#define SOCK_CFG_MAX_CHANNELS (PSI_CFG_SOCK_CFG_MAX_CHANNELS)

/* maxímum number of supported interfaces */
#define SOCK_CFG_MAX_INTERFACE_CNT (PSI_CFG_SOCK_CFG_MAX_INTERFACE_CNT)

/*===========================================================================*/
/*                          basic attributes for sock                        */
/*===========================================================================*/


/*===========================================================================*/
/*                      code- and data-attributes for sock                   */
/*===========================================================================*/


/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/




/****************************************************************************/
/* Macro-Definitionen                                                       */
/****************************************************************************/

#if SOCK_DEBUG

#ifdef _lint
#define SOCK_TEST_POINTER(ptr_)  (ptr_) /* empty for Lint */
#else
#define SOCK_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */
#endif

#else

#define SOCK_TEST_POINTER(ptr_)  (ptr_) /* don't test */

#endif

/*--------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                  */
/*--------------------------------------------------------------------------*/

#ifdef  LSA_RQB_HEADER
#define SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)    LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)    LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)

#define SOCK_RQB_SET_OPCODE(rb_, val_)          LSA_RQB_SET_OPCODE(rb_, val_)
#define SOCK_RQB_GET_OPCODE(rb_)                LSA_RQB_GET_OPCODE(rb_)

#define SOCK_RQB_SET_HANDLE(rb_, val_)          LSA_RQB_SET_HANDLE(rb_, val_)
#define SOCK_RQB_GET_HANDLE(rb_)                LSA_RQB_GET_HANDLE(rb_)

#define SOCK_RQB_SET_RESPONSE(rb_, val_)        LSA_RQB_SET_RESPONSE(rb_, val_)
#define SOCK_RQB_GET_RESPONSE(rb_)              LSA_RQB_GET_RESPONSE(rb_)

#define SOCK_RQB_GET_NEXT_RQB_PTR(rb_)          LSA_RQB_GET_NEXT_RQB_PTR(rb_)
#define SOCK_RQB_GET_PREV_RQB_PTR(rb_)          LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define SOCK_RQB_SET_USER_ID_PTR(_pRQB, _Value) LSA_RQB_SET_USER_ID_PTR(_pRQB, _Value)
#define SOCK_RQB_GET_USER_ID_PTR(_pRQB)         LSA_RQB_GET_USER_ID_PTR(_pRQB)

#else
#error "How this? LSA_RQB_HEADER must be befined at this point"
#endif


/*------------------------------------------------------------------------------
* interface to LSA trace                                                     
* 0: no traces                                                               
* 1: enable LSA Traces                                                       
* 2: enable LSA Idx Traces                                                   
//----------------------------------------------------------------------------*/
#define SOCK_CFG_TRACE_MODE             (PSI_CFG_TRACE_MODE)

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef SOCK_FILE_SYSTEM_EXTENSION
# define SOCK_FILE_SYSTEM_EXTENSION(module_id_)	/* Currently not supported */
#endif


/*===========================================================================*/
/*                                types for sock                             */
/*===========================================================================*/

#define SOCK_EXCHANGE_TYPE long

/*=============================================================================
 *      sock common request block type
=============================================================================*/

#define SOCK_RQB_HEADER   LSA_RQB_HEADER(SOCK_UPPER_RQB_PTR_TYPE)

#define SOCK_RQB_TRAILER  /* nothing by default */


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*- alloc mem macros----------------------------------------------------------*/
#define SOCK_ALLOC_LOCAL_MEM(local_mem_ptr_ptr, length)     PSI_ALLOC_LOCAL_MEM((local_mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM)

#define SOCK_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif 
