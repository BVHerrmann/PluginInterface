
#ifndef SNMPX_SYS_H                      /* ----- reinclude-protection ----- */
#define SNMPX_SYS_H

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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_sys.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for snmpx               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for SNMPXFatalError())                             */
/*---------------------------------------------------------------------------*/

#define SNMPX_FATAL_ERROR_TYPE  LSA_UINT32

#define SNMPX_FATAL_ERR_NULL_PTR            (SNMPX_FATAL_ERROR_TYPE)  0x01
#define SNMPX_FATAL_ERR_MEM_FREE            (SNMPX_FATAL_ERROR_TYPE)  0x02
#define SNMPX_FATAL_ERR_RELEASE_PATH_INFO   (SNMPX_FATAL_ERROR_TYPE)  0x03
#define SNMPX_FATAL_ERR_INCONSISTENZ        (SNMPX_FATAL_ERROR_TYPE)  0x04
#define SNMPX_FATAL_ERR_HANDLE_INVALID      (SNMPX_FATAL_ERROR_TYPE)  0x06
#define SNMPX_FATAL_ERR_SOCK                (SNMPX_FATAL_ERROR_TYPE)  0x07
#define SNMPX_FATAL_ERR_USER                (SNMPX_FATAL_ERROR_TYPE)  0x08
#define SNMPX_FATAL_ERR_TIMER_RESOURCE      (SNMPX_FATAL_ERROR_TYPE)  0x0E
#define SNMPX_FATAL_ERR_RQB                 (SNMPX_FATAL_ERROR_TYPE)  0x0F

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
#define SNMPX_DETAIL_PTR_TYPE                 /* pointer to SNMPX_DETAIL */ \
struct snmpx_detail_tag       *

#define SNMPX_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag    *

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                   CDB (Channel Detail Block)                              */
/*                     (used by SNMPX_GET_PATH_INFO)                         */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* CDB - Channel Detail Block                                                */
/*      (used as detail-ptr in SNMPX_GET_PATH_INFO)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef union snmpx_path_params_tag
{
    /* Params for SNMPX_PATH_TYPE_AGENT User-Channel */
    struct
    {
        LSA_BOOL        Nothing;
    } Agent;

    /* Params for SNMPX_PATH_TYPE_SOCK Channel */
    /* NumOfRecv = Number of max. remote snmp manager stations, which can be */
    /* requested at the "same time". More remote snmp requests at the same   */
    /* time will be ignored.*/
    struct
    {
        LSA_UINT16      NumOfRecv;  /* receive-resources for SOCK (SNMP-Port) */
    } Sock;

    struct                          /* Params for SNMPX_PATH_TYPE_MANAGER User-Channel */
    {
        LSA_UINT16          NumOfRetries;     /* default: 2 retries */
        LSA_UINT16          TimeoutInterval;  /* default: 2 sec     */
    } Manager;

} SNMPX_PATH_PARAMS_TYPE;


/*---------------------------------------------------------------------------*/
/* Number of max. remote snmp manager stations, which can be requested at    */
/* the "same time". More remote snmp requests at same time will be ignored.  */
/*---------------------------------------------------------------------------*/


typedef struct snmpx_detail_tag
{
    LSA_UINT16                PathType; /* Type of Channel                   */
    SNMPX_PATH_PARAMS_TYPE    Params;   /* PathType specific Params.         */
} SNMPX_DETAIL_TYPE;

/* defines for PathType */

#define SNMPX_PATH_TYPE_AGENT   1   /* User-Channel of a SNMP sub agent      */
#define SNMPX_PATH_TYPE_MANAGER 2   /* User-Channel of a SNMP manager        */
#define SNMPX_PATH_TYPE_SOCK    3   /* System-Channel for SOCK */
#define SNMPX_PATH_TYPE_OHA     4   /* System-Channel for OHA */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                              external functions                           */
/*===========================================================================*/


/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/

/*====  in functions  =====*/
/*=============================================================================
 * function name:  snmpx_init
 *
 * function:       initialize SNMPX
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16 snmpx_init(LSA_VOID);

/*=============================================================================
 * function name:  snmpx_undo_init
 *
 * function:       undo the initialization of SNMPX
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16 snmpx_undo_init(LSA_VOID);


/*=============================================================================
 * function name:  snmpx_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID snmpx_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id);


/*****************************************************************************/
/*                                                                           */
/* Output-functions/macros                                                   */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  SNMPX_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 SNMPX_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_GET_PATH_INFO
LSA_VOID SNMPX_GET_PATH_INFO(
    LSA_UINT16            * ret_val_ptr,
    LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
    SNMPX_DETAIL_PTR_TYPE * detail_ptr_ptr,
    LSA_SYS_PATH_TYPE       path
);
#endif

/*=============================================================================
 * function name:  SNMPX_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system-pointer or
 *                                                      wrong detail-pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system-pointer
 *                 SNMPX_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_RELEASE_PATH_INFO
LSA_VOID SNMPX_RELEASE_PATH_INFO(
    LSA_UINT16            * ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    SNMPX_DETAIL_PTR_TYPE   detail_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated
 *                                         LSA_RET_ERR_NO_TIMER  no timer has
 *                                                               been allocated
 *                 LSA_TIMER_ID_TYPE * timer_id_ptr:
 *                           return value: pointer to id of timer
 *                 LSA_UINT16          timer_type:  LSA_TIMER_TYPE_ONE_SHOT or
 *                                                  LSA_TIMER_TYPE_CYCLIC
 *                 LSA_UINT16          time_base:   LSA_TIME_BASE_1MS,
 *                                                  LSA_TIME_BASE_10MS,
 *                                                  LSA_TIME_BASE_100MS,
 *                                                  LSA_TIME_BASE_1S,
 *                                                  LSA_TIME_BASE_10S or
 *                                                  LSA_TIME_BASE_100S
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_ALLOC_TIMER
LSA_VOID SNMPX_ALLOC_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE * timer_id_ptr,
    LSA_UINT16   timer_type,
    LSA_UINT16   time_base
);
#endif

/*=============================================================================
 * function name:  SNMPX_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn´t
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time
 *                                 system will call snmpx_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of prefix
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_START_TIMER
LSA_VOID SNMPX_START_TIMER(
    LSA_UINT16       * ret_val_ptr,
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id,
    LSA_UINT16         time
);
#endif

/*=============================================================================
 * function name:  SNMPX_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                    timer has been
 *                                                               stopped
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped
 *                                 LSA_RET_ERR_PARAM             timer hasn´t
 *                                                               stopped
 *                                                               because
 *                                                               of wrong
 *                                                               timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_STOP_TIMER
LSA_VOID SNMPX_STOP_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id
);
#endif

/*=============================================================================
 * function name:  SNMPX_FREE_TIMER
 *
 * function:       free a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_FREE_TIMER
LSA_VOID SNMPX_FREE_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id
);
#endif

/*=============================================================================
 * function name:  SNMPX_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 SNMPX_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_FATAL_ERROR
/*@noreturn@*/ LSA_VOID SNMPX_FATAL_ERROR(
    LSA_UINT16                 length,
    SNMPX_FATAL_ERROR_PTR_TYPE error_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_ENTER
 *
 * function:       Protect sequence within SNMPX from beeing interrupted.
 *                 (reentrance protection)
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_ENTER
LSA_VOID SNMPX_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  SNMPX_EXIT
 *
 * function:       Finishes SNMPX_ENTER-Sequence.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_EXIT
LSA_VOID SNMPX_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  SNMPX_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     SNMPX_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#define SNMPX_LOCAL_MEM_PTR_TYPE                /* pointer to local-memory */ \
LSA_VOID                     *

#ifndef SNMPX_ALLOC_LOCAL_MEM
LSA_VOID SNMPX_ALLOC_LOCAL_MEM(
    SNMPX_LOCAL_MEM_PTR_TYPE * local_mem_ptr_ptr,
    LSA_UINT32                 length
);
#endif

/*=============================================================================
 * function name:  SNMPX_FREE_LOCAL_MEM
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 SNMPX_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_FREE_LOCAL_MEM
LSA_VOID SNMPX_FREE_LOCAL_MEM(
    LSA_UINT16               * ret_val_ptr,
    SNMPX_LOCAL_MEM_PTR_TYPE   local_mem_ptr
);
#endif


LSA_EXTERN /*@noreturn@*/ LSA_VOID SNMPX_FatalError(
    SNMPX_FATAL_ERROR_TYPE   Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line);


/*=============================================================================
 * function name:  SNMPX_MEMSET
 *
 * function:       fills memory with value
 *
 * parameters:      SNMPX_LOCAL_MEM_PTR_TYPE       pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_MEMSET
LSA_VOID SNMPX_MEMSET(
    SNMPX_LOCAL_MEM_PTR_TYPE            pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  SNMPX_MEMCPY
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      SNMPX_COMMON_MEM_PTR_TYPE       pDst
 *                  SNMPX_COMMON_MEM_PTR_TYPE       pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_MEMCPY
LSA_VOID SNMPX_MEMCPY(
    SNMPX_COMMON_MEM_PTR_TYPE          pDst,
    SNMPX_COMMON_MEM_PTR_TYPE          pSrc,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  SNMPX_MEMMOVE
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      SNMPX_COMMON_MEM_PTR_TYPE      pDst
 *                  SNMPX_COMMON_MEM_PTR_TYPE      pSrc
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_MEMMOVE
LSA_VOID SNMPX_MEMMOVE(
    SNMPX_COMMON_MEM_PTR_TYPE          pDst,
    SNMPX_COMMON_MEM_PTR_TYPE          pSrc,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  SNMPX_MEMCMP
 *
 * function:       compares Length bytes of memory from pBuf1 with pBuf2
 *
 * parameters:      SNMPX_COMMON_MEM_PTR_TYPE       pBuf1
 *                  SNMPX_COMMON_MEM_PTR_TYPE       pBuf2
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_TRUE:  buffers are the same
 *                 LSA_FALSE: buffers differ
 *===========================================================================*/
#ifndef SNMPX_MEMCMP
LSA_BOOL SNMPX_MEMCMP(
    SNMPX_COMMON_MEM_PTR_TYPE          pBuf1,
    SNMPX_COMMON_MEM_PTR_TYPE          pBuf2,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  SNMPX_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper-RQB for local use
 *                 the memory has to be initialized with 0
 *
 * parameters:     SNMPX_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_ALLOC_UPPER_RQB_LOCAL
LSA_VOID SNMPX_ALLOC_UPPER_RQB_LOCAL(
    SNMPX_UPPER_RQB_PTR_TYPE * upper_rqb_ptr_ptr,
    LSA_UINT16                 length
);
#endif

/*=============================================================================
 * function name:  SNMPX_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper-RQB allocated with SNMPX_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                 SNMPX_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_FREE_UPPER_RQB_LOCAL
LSA_VOID SNMPX_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16               * ret_val_ptr,
    SNMPX_UPPER_RQB_PTR_TYPE   upper_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_ALLOC_UPPER_MEM
LSA_VOID
SNMPX_ALLOC_UPPER_MEM(
    SNMPX_UPPER_MEM_PTR_TYPE * upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_FREE_UPPER_MEM
LSA_VOID
SNMPX_FREE_UPPER_MEM(
    LSA_UINT16               * ret_val_ptr,
    SNMPX_UPPER_MEM_PTR_TYPE   upper_mem_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:    SNMPX_HTONS ... host to network byte order, short integer
 *                        _NTOHS ... network to host byte order, short integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_HTONS
LSA_UINT16
SNMPX_HTONS(
    LSA_UINT16 host_short
);
#endif

#ifndef SNMPX_NTOHS
LSA_UINT16
SNMPX_NTOHS(
    LSA_UINT16 network_short
);
#endif

/*=============================================================================
 * function name:    SNMPX_HTONL ... host to network byte order, long  integer
 *                        _NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_HTONL
LSA_UINT32
SNMPX_HTONL(
    LSA_UINT32 host_long
);
#endif

#ifndef SNMPX_NTOHL
LSA_UINT32
SNMPX_NTOHL(
    LSA_UINT32 network_long
);
#endif


/*=============================================================================
 * function name:  SNMPX_PUT 8,16,32 and SNMPX_GET 8,16,32
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef SNMPX_PUT8_HTON
LSA_VOID SNMPX_PUT8_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT8 val
);
#endif

#ifndef SNMPX_GET8_NTOH
LSA_UINT8 SNMPX_GET8_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

#ifndef SNMPX_PUT16_HTON
LSA_VOID SNMPX_PUT16_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT16 val
);
#endif

#ifndef SNMPX_GET16_NTOH
LSA_UINT16 SNMPX_GET16_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

#ifndef SNMPX_PUT32_HTON
LSA_VOID SNMPX_PUT32_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT32 val
);
#endif

#ifndef SNMPX_GET32_NTOH
LSA_UINT32 SNMPX_GET32_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

/*=============================================================================
 * function name:  SNMPX_DO_TIMER_REQUEST
 *
 * function:       Does a snmpx_system() request with the spezified RQB. This
 *                 function is called within SNMPX timer handler to signal
 *                 a Timer event to the main context of SNMPX.
 *                 Systemadaption has to call snmpx_system() with the specified
 *                 RQB. Before calling, the systemadaption should change to
 *                 taskstate.
 *
 * parameters:     SNMPX_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef SNMPX_DO_TIMER_REQUEST
LSA_VOID SNMPX_DO_TIMER_REQUEST(
    SNMPX_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  SNMPX_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE
 *                                         snmpx_request_upper_done_ptr:
 *                     pointer to snmpx_request_upper_done_ptr, given by
 *                     snmpx_open_channel()
 *
 *                 SNMPX_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE        sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef SNMPX_REQUEST_UPPER_DONE
LSA_VOID SNMPX_REQUEST_UPPER_DONE(
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE  snmpx_request_upper_done_ptr,
    SNMPX_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                   sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file SNMPX_SYS.H                                                  */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_SYS_H */
