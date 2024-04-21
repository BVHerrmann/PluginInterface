#ifndef SOCK_PLS_H
#define SOCK_PLS_H
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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_pls.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check plausibility                                                       */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*===========================================================================*/

#ifndef SOCK_CFG_INTERNICHE
#error SOCK_CFG_INTERNICHE must be defined in sock_cfg.h!
#elif SOCK_CFG_INTERNICHE < 0 || SOCK_CFG_INTERNICHE > 1
#error SOCK_CFG_INTERNICHE must be 0 or 1!
#endif

#ifndef SOCK_CFG_OPEN_BSD /* OBSD_ITGR */
#error SOCK_CFG_OPEN_BSD must be defined in sock_cfg.h!
#elif SOCK_CFG_OPEN_BSD < 0 || SOCK_CFG_OPEN_BSD > 1
#error SOCK_CFG_OPEN_BSD must be 0 or 1!
#endif

#ifndef SOCK_CFG_WINSOCK
#error SOCK_CFG_WINSOCK must be defined in sock_cfg.h!
#elif SOCK_CFG_WINSOCK < 0 || SOCK_CFG_WINSOCK > 1
#error SOCK_CFG_WINSOCK must be 0 or 1!
#endif

#ifndef SOCK_CFG_CUSTOM
#error SOCK_CFG_CUSTOM must be defined in sock_cfg.h!
#elif SOCK_CFG_CUSTOM < 0 || SOCK_CFG_CUSTOM > 1
#error SOCK_CFG_CUSTOM must be 0 or 1!
#endif

#if (SOCK_CFG_INTERNICHE + SOCK_CFG_OPEN_BSD + SOCK_CFG_WINSOCK + SOCK_CFG_CUSTOM) != 1
#error only one stack configuration can be enabled!
#endif

#if SOCK_CFG_OPEN_BSD
#ifdef PSI_CFG_MAX_SOCKETS /* sanity */
#if TCIP_CFG_MAX_SOCKETS != PSI_CFG_MAX_SOCKETS
#error TCIP_CFG_MAX_SOCKETS must be equal to PSI_CFG_MAX_SOCKETS!
#endif
#if TCIP_CFG_MAX_SOCKETS > 1024
#error for OpenBSD the number of TCIP_CFG_MAX_SOCKETS should not exceed 1024
#endif
#endif
#endif

/*===========================================================================*/

#ifdef SOCK_LOCAL_MEM_STATIC
#error SOCK_LOCAL_MEM_STATIC must not be defined
#endif

#ifndef SOCK_LOCAL_MEM_DYNAMIC
#error SOCK_LOCAL_MEM_DYNAMIC must be defined
#endif

/*===========================================================================*/

#ifdef SOCK_CLEAR_GLOBAL_DATA
#error SOCK_CLEAR_GLOBAL_DATA must not be defined
#endif

#ifndef SOCK_NO_CLEAR_GLOBAL_DATA
#error "SOCK_NO_CLEAR_GLOBAL_DATA" must be defined
#endif

/*===========================================================================*/

#ifdef SOCK_CFG_DNS_CLIENT
#error "SOCK_CFG_DNS_CLIENT" must not be defined
#endif

/*===========================================================================*/

#ifndef SOCK_CFG_TRACE_MODE
# error SOCK_CFG_TRACE_MODE not defined in file sock_cfg.h
#endif

#ifdef SOCKIF_BTRACE_SUPPORT
# error BTRACE not supported
#endif

/*===========================================================================*/

#ifndef SOCK_ASSERT
#error "SOCK_ASSERT()" not defined in sock_cfg.h!
#endif


#if defined SOCK_UPPER_IN_FCT_ATTR
#error "SOCK_UPPER_IN_FCT_ATTR" defined in file sock_cfg.h
#endif

#if defined SOCK_SYSTEM_IN_FCT_ATTR
#error "SOCK_SYSTEM_IN_FCT_ATTR" defined in file sock_cfg.h
#endif

#if defined SOCK_SYSTEM_OUT_FCT_ATTR
#error "SOCK_SYSTEM_OUT_FCT_ATTR" defined in file sock_cfg.h
#endif

#if defined SOCK_LOCAL_FCT_ATTR
#error "SOCK_LOCAL_FCT_ATTR" defined in file sock_cfg.h
#endif

#if defined SOCK_SYSTEM_MEM_ATTR
#error "SOCK_SYSTEM_MEM_ATTR" defined in file sock_cfg.h
#endif

#if defined SOCK_LOCAL_MEM_ATTR
#error "SOCK_LOCAL_MEM_ATTR" defined in file sock_cfg.h
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE" not defined in file lsa_cfg.h
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE" not defined in file lsa_cfg.h
#endif


/*****************************************************************************/
/* plausibility check of general attributes                                  */
/*****************************************************************************/
#if !defined SOCK_LOCAL_MEM_STATIC && !defined SOCK_LOCAL_MEM_DYNAMIC
#error "SOCK_LOCAL_MEM_STATIC" or "SOCK_LOCAL_MEM_DYNAMIC" is missing in file sock_cfg.h!
#endif

#if defined SOCK_LOCAL_MEM_STATIC && defined SOCK_LOCAL_MEM_DYNAMIC
#error Define "SOCK_LOCAL_MEM_STATIC" or "SOCK_LOCAL_MEM_DYNAMIC" in file sock_cfg.h!
#endif

#if !defined SOCK_CLEAR_GLOBAL_DATA && !defined SOCK_NO_CLEAR_GLOBAL_DATA
#error "SOCK_CLEAR_GLOBAL_DATA" or "SOCK_NO_CLEAR_GLOBAL_DATA" is missing in file sock_cfg.h!
#endif

#if defined SOCK_CLEAR_GLOBAL_DATA && defined SOCK_NO_CLEAR_GLOBAL_DATA
#error Define "SOCK_CLEAR_GLOBAL_DATA" or "SOCK_NO_CLEAR_GLOBAL_DATA" in file sock_cfg.h!
#endif


/*****************************************************************************/
/* check presence of defines                                                 */
/*****************************************************************************/

#if !defined SOCK_CFG_UDP_ON
#error "SOCK_CFG_UDP_ON" not defined in file sock_cfg.h!
#endif

#if !defined SOCK_CFG_ENABLE_MULTICASTS
#error "SOCK_CFG_ENABLE_MULTICASTS" is not defined in file sock_cfg.h!
#endif

#if SOCK_CFG_ENABLE_MULTICASTS
#if (SOCK_CFG_INTERNICHE == 0) && (SOCK_CFG_OPEN_BSD == 0)
#error SOCK_CFG_ENABLE_MULTICAST must be 0 for this configuration!
#endif
#if TCIP_CFG_ENABLE_MULTICAST == 0
#error TCIP_CFG_ENABLE_MULTICAST must be 1 for this configuration!
#endif
#endif

#if !defined SOCK_CFG_BIND_MULTICAST_SOCKETS_TO_ANY
#error "SOCK_CFG_BIND_MULTICAST_SOCKETS_TO_ANY" is not defined in file sock_cfg.h!
#endif

#if !defined SOCK_CFG_SNMP_ON
#error "SOCK_CFG_SNMP_ON" not defined in file sock_cfg.h!
#endif

#if !defined SOCK_CFG_MAX_CHANNELS
#error "SOCK_CFG_MAX_CHANNELS" not defined in file sock_cfg.h!
#endif

#if !SOCK_CFG_INTERNICHE && !SOCK_CFG_OPEN_BSD /* OBSD_ITGR -- new switch for Open BSD */
#if !defined SOCK_CFG_MAX_SOCKETS
#error "SOCK_CFG_MAX_SOCKETS" not defined in file sock_cfg.h!
#endif
#endif

#if !defined SOCK_CFG_MAX_INTERFACE_CNT
#error "SOCK_CFG_MAX_INTERFACE_CNT" not defined in file sock_cfg.h!
#endif

#if !defined SOCK_CFG_CLUSTER_IP_SUPPORTED
#error "SOCK_CFG_CLUSTER_IP_SUPPORTED" not defined in file sock_cfg.h!
#endif

#if SOCK_CFG_CLUSTER_IP_SUPPORTED && !SOCK_CFG_OPEN_BSD && !SOCK_CFG_CUSTOM
#error "SOCK_CFG_CLUSTER_IP_SUPPORTED" cannot be 1 for this configuration!
#endif
/*****************************************************************************/
/* obsolete defines                                                          */
/*****************************************************************************/

#ifdef SOCK_CFG_DO_TIMER_SYNCHRONIZATION
#error SOCK_CFG_DO_TIMER_SYNCHRONIZATION is obsolete and has no effect!
#endif

#ifdef SOCK_CFG_MAX_CONNECTIONS
#error SOCK_CFG_MAX_CONNECTIONS is obsolete and has no effect!
#endif

#if defined SOCK_CFG_ENABLE_SOCKET_EVENTS
#error "SOCK_CFG_ENABLE_SOCKET_EVENTS" is obsolete and has no effect
#endif

#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD

#if defined SOCK_CFG_SNMP_BUFFER_LEN
#error "SOCK_CFG_SNMP_BUFFER_LEN" is obsolete and has no effect
#endif

#if defined SOCK_CFG_SNMP_PORTNUMBER
#error "SOCK_CFG_SNMP_PORTNUMBER" is obsolete and has no effect
#endif

#endif

/*===========================================================================*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of SOCK_PLS_H */
