#ifndef SNMPX_CFG_H                      /* ----- reinclude-protection ----- */
#define SNMPX_CFG_H

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
/*  F i l e               &F: snmpx_cfg.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for SNMPX:                                                 */
/*  Defines constants, types and macros for SNMPX.                           */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/


void snmpx_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

#define SNMPX_CFG_TRACE_RQB_ERRORS      (PSI_CFG_SNMPX_CFG_TRACE_RQB_ERRORS)

/*---------------------------------------------------------------------------*/
/* The functionality must be defined (one of the following or both):         */
/*---------------------------------------------------------------------------*/
/* #define SNMPX_CFG_SNMP_MANAGER - snmp manager functionality               */
/*---------------------------------------------------------------------------*/
/* #define SNMPX_CFG_SNMP_AGENT - snmp agent functionality (not implemented) */
/*---------------------------------------------------------------------------*/

#ifdef PSI_CFG_SNMPX_CFG_SNMP_MANAGER
    #define SNMPX_CFG_SNMP_MANAGER
#else
    #error "SNMPX_CFG_SNMP_MANAGER snmp manager functionality must be defined"
#endif

#ifdef SNMPX_CFG_SNMP_AGENT
    #error "SNMPX_CFG_SNMP_AGENT snmp agent functionality not implemented"
#endif

/*---------------------------------------------------------------------------*/
/* Maximum number of user-channels (max. 32, because of handle-management)   */
/* is the maximum number of manageable sub agents (one per user channel)     */
/*---------------------------------------------------------------------------*/

#define SNMPX_CFG_MAX_CHANNELS           1

/*---------------------------------------------------------------------------*/
/* Maximum number of sessions for one manager (user) channel.                */
/* #define SNMPX_CFG_MAX_MANAGER_SESSION (default: 32)                       */
/*---------------------------------------------------------------------------*/

#define SNMPX_CFG_MAX_MANAGER_SESSIONS  (PSI_CFG_SNMPX_CFG_MAX_MANAGER_SESSIONS)

/* configure the integer data representation
     * the definition must reflect the hardware. there is no choice! */
/* If you have a system which stores most significant byte at the
 * lowest address of the word/doubleword:
*/

#if defined (LSA_HOST_ENDIANESS_BIG)
#define SNMPX_BIG_ENDIAN
#else
#undef SNMPX_BIG_ENDIAN
#endif

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*                                                                           */
/* The order of the parameters can be changed!                               */
/* The length of the RQB-HEADER must be 2/4-byte-aligned according to the    */
/* system!                                                                   */
/* If necessary, dummy-bytes must be added!                                  */
/* If necessary during system integration, additional parameters can be added*/
/*                                                                           */
/* For example:                                                              */
/*   LSA_COMP_ID_TYPE                comp_id;  for lsa-component-id          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define SNMPX_RQB_HEADER  LSA_RQB_HEADER(SNMPX_UPPER_RQB_PTR_TYPE)


/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define SNMPX_RQB_TRAILER
/* nothing by default */


/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define SNMPX_RQB_SET_NEXT_RQB_PTR      LSA_RQB_SET_NEXT_RQB_PTR
#define SNMPX_RQB_GET_NEXT_RQB_PTR      LSA_RQB_GET_NEXT_RQB_PTR
#define SNMPX_RQB_SET_PREV_RQB_PTR      LSA_RQB_SET_PREV_RQB_PTR
#define SNMPX_RQB_GET_PREV_RQB_PTR      LSA_RQB_GET_PREV_RQB_PTR
#define SNMPX_RQB_SET_OPCODE            LSA_RQB_SET_OPCODE
#define SNMPX_RQB_GET_OPCODE            LSA_RQB_GET_OPCODE
#define SNMPX_RQB_SET_HANDLE            LSA_RQB_SET_HANDLE
#define SNMPX_RQB_GET_HANDLE            LSA_RQB_GET_HANDLE
#define SNMPX_RQB_SET_USERID_UVAR32     LSA_RQB_SET_USER_ID_UVAR32
#define SNMPX_RQB_GET_USERID_UVAR32     LSA_RQB_GET_USER_ID_UVAR32
#define SNMPX_RQB_SET_USERID_UVAR16_1   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW
#define SNMPX_RQB_GET_USERID_UVAR16_1   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW
#define SNMPX_RQB_SET_USERID_UVAR16_2   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH
#define SNMPX_RQB_GET_USERID_UVAR16_2   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH
#define SNMPX_RQB_SET_RESPONSE          LSA_RQB_SET_RESPONSE
#define SNMPX_RQB_GET_RESPONSE          LSA_RQB_GET_RESPONSE
#define SNMPX_RQB_SET_USER_ID_PTR       LSA_RQB_SET_USER_ID_PTR
#define SNMPX_RQB_GET_USER_ID_PTR       LSA_RQB_GET_USER_ID_PTR


/*===========================================================================*/
/* LOWER LAYER RQB MACROS to SOCK                                            */
/*===========================================================================*/

#define SNMPX_SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)   SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define SNMPX_SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)   SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define SNMPX_SOCK_RQB_SET_OPCODE(rb_, val_)         SOCK_RQB_SET_OPCODE(rb_, val_)
#define SNMPX_SOCK_RQB_SET_HANDLE(rb_, val_)         SOCK_RQB_SET_HANDLE(rb_, val_)
#define SNMPX_SOCK_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define SNMPX_SOCK_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, val_)
#define SNMPX_SOCK_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(rb_, val_) LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(rb_, val_)
#define SNMPX_SOCK_RQB_SET_USERID_UVAR32(rb_, val_)  SOCK_RQB_SET_USERID_UVAR32(rb_, val_)
#define SNMPX_SOCK_RQB_SET_RESPONSE(rb_, val_)       SOCK_RQB_SET_RESPONSE(rb_, val_)

#define SNMPX_SOCK_RQB_GET_NEXT_RQB_PTR(rb_)         SOCK_RQB_GET_NEXT_RQB_PTR(rb_)
#define SNMPX_SOCK_RQB_GET_PREV_RQB_PTR(rb_)         SOCK_RQB_GET_PREV_RQB_PTR(rb_)
#define SNMPX_SOCK_RQB_GET_OPCODE(rb_)               SOCK_RQB_GET_OPCODE(rb_)
#define SNMPX_SOCK_RQB_GET_HANDLE(rb_)               SOCK_RQB_GET_HANDLE(rb_)
#define SNMPX_SOCK_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_)  LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_)
#define SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(rb_) LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(rb_)
#define SNMPX_SOCK_RQB_GET_USERID_UVAR32(rb_)        SOCK_RQB_GET_USERID_UVAR32(rb_)
#define SNMPX_SOCK_RQB_GET_RESPONSE(rb_)             SOCK_RQB_GET_RESPONSE(rb_)


/*===========================================================================*/
/* LOWER LAYER RQB MACROS to OHA                                             */
/*===========================================================================*/

#define SNMPX_OHA_RQB_SET_NEXT_RQB_PTR(rb_, val_)   OHA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define SNMPX_OHA_RQB_SET_PREV_RQB_PTR(rb_, val_)   OHA_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define SNMPX_OHA_RQB_SET_USER_ID_PTR(rb_, val_)    OHA_RQB_SET_USER_ID_PTR(rb_, val_)
#define SNMPX_OHA_RQB_SET_OPCODE(rb_, val_)         OHA_RQB_SET_OPCODE(rb_, val_)
#define SNMPX_OHA_RQB_SET_HANDLE(rb_, val_)         OHA_RQB_SET_HANDLE(rb_, val_)
#define SNMPX_OHA_RQB_SET_RESPONSE(rb_, val_)       OHA_RQB_SET_RESPONSE(rb_, val_)

#define SNMPX_OHA_RQB_GET_USER_ID_PTR(rb_)          OHA_RQB_GET_USER_ID_PTR(rb_)
#define SNMPX_OHA_RQB_GET_OPCODE(rb_)               OHA_RQB_GET_OPCODE(rb_)
#define SNMPX_OHA_RQB_GET_HANDLE(rb_)               OHA_RQB_GET_HANDLE(rb_)
#define SNMPX_OHA_RQB_GET_RESPONSE(rb_)             OHA_RQB_GET_RESPONSE(rb_)


/*===========================================================================*/
/* BTRACE - Support                                                          */
/*===========================================================================*/

#ifndef SNMPX_FILE_SYSTEM_EXTENSION
#define SNMPX_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/
/*----------------------------------------------------------------------------*/
/* interface to LSA trace                                                     */
/* 0: no traces                                                               */
/* 1: enable LSA Traces                                                       */
/* 2: enable LSA Idx Traces                                                   */
//----------------------------------------------------------------------------*/
#define SNMPX_CFG_TRACE_MODE            (PSI_CFG_TRACE_MODE)

/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then
 * the SNMPX trace macros can be defined here. On default they are empty.
 *
 * See also files snmpx_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/

/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If a a memory-free call error should result in an fatal-error call        */
/* with SNMPX_FATAL_ERR_MEM_FREE define SNMPX_CFG_FREE_ERROR_IS_FATAL.       */
/* If not memory-free errors will be ignored.                                */
/*---------------------------------------------------------------------------*/

#define SNMPX_CFG_FREE_ERROR_IS_FATAL

/*---------------------------------------------------------------------------*/
/* define to enable spezial additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/

#if PSI_DEBUG
#define SNMPX_CFG_DEBUG_ASSERT
#endif

/*----------------------------------------------------------------------------*/
/*  Assert                                                                    */
/*                                                                            */
/*  ==> MUST BE EMPTY in Release-Version!                                     */
/*                                                                            */
/*  check if condition holds or call fatal error otherwise (programming error)*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifdef SNMPX_CFG_DEBUG_ASSERT
#define SNMPX_ASSERT(Cond_)                             \
{                                                       \
    if( ! (Cond_) )                                     \
    {                                                   \
        SNMPX_FatalError( SNMPX_FATAL_ERR_INCONSISTENZ, \
                          SNMPX_MODULE_ID,              \
                          __LINE__);                    \
    }                                                   \
}

#define SNMPX_ASSERT_NULL_PTR(pPtr) \
{                                                       \
    if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )        \
    {                                                   \
        SNMPX_FatalError( SNMPX_FATAL_ERR_NULL_PTR,     \
                          SNMPX_MODULE_ID,              \
                          __LINE__);                    \
    }                                                   \
}

#define SNMPX_ASSERT_FALSE(Cond_)                       \
{                                                       \
    if( ! (Cond_) )                                     \
    {                                                   \
        SNMPX_FatalError( SNMPX_FATAL_ERR_INCONSISTENZ, \
                          SNMPX_MODULE_ID,              \
                          __LINE__);                    \
    }                                                   \
}
#else
#define SNMPX_ASSERT(Condition) {}
#define SNMPX_ASSERT_NULL_PTR(pPtr) {}
#define SNMPX_ASSERT_FALSE(Condition) {}
#endif

/*===========================================================================*/
/*                          macros/function                                  */
/*===========================================================================*/

/*- alloc mem macros----------------------------------------------------------*/
#define SNMPX_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_UPPER_RQB)
#define SNMPX_ALLOC_UPPER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)        PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_UPPER_MEM)
#define SNMPX_SOCK_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)   PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_RQB_SOCK)
#define SNMPX_SOCK_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)   PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_MEM_SOCK)
#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
#define SNMPX_OHA_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_RQB_OHA)
#define SNMPX_OHA_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_MEM_OHA)
#endif
#define SNMPX_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                          PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM)

#define SNMPX_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)              PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_UPPER_RQB)
#define SNMPX_FREE_UPPER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)           PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_UPPER_MEM)
#define SNMPX_SOCK_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)      PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_RQB_SOCK)
#define SNMPX_SOCK_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)      PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_MEM_SOCK)
#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
#define SNMPX_OHA_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_RQB_OHA)
#define SNMPX_OHA_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_SNMPX, PSI_MTYPE_LOWER_MEM_OHA)
#endif
#define SNMPX_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)                    PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM)


/*****************************************************************************/
/*  end of file snmpx_cfg.h                                                  */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_CFG_H */
