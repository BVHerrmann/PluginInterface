
#ifndef SNMPX_LOW_H                        /* ----- reinclude-protection ----- */
#define SNMPX_LOW_H

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
/*  F i l e               &F: snmpx_low.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower component interface                                                */
/*  Defines constants, types, macros and prototyping for SNMPX               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/



/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                   LOWER LAYER MACROS to SOCK                              */
/*===========================================================================*/

/* SOCK-RQB */
typedef SOCK_RQB_TYPE    SNMPX_SOCK_LOWER_RQB_TYPE;
typedef SNMPX_SOCK_LOWER_RQB_TYPE   *  SNMPX_SOCK_LOWER_RQB_PTR_TYPE;

/* sock doesn't define the pointers */

typedef SOCK_RQB_ARGS_CHANNEL_TYPE      *SNMPX_SOCK_OPEN_CHANNEL_PTR_TYPE;
typedef SOCK_RQB_ARGS_UDP_OPEN_TYPE     *SNMPX_SOCK_UDP_OPEN_PTR_TYPE;
typedef SOCK_RQB_ARGS_CLOSE_TYPE        *SNMPX_SOCK_UDP_CLOSE_PTR_TYPE;
typedef SOCK_RQB_ARGS_DATA_TYPE         *SNMPX_SOCK_UDP_DATA_PTR_TYPE;
/* typedef SOCK_RQB_ARGS_GETHOSTBYNAME_TYPE *SNMPX_SOCK_GETHOSTBYNAME_PTR_TYPE; */
typedef SOCK_RQB_ARGS_SNMP_RECEIVE_TYPE *SNMPX_SOCK_SNMP_RECEIVE_PTR_TYPE;

/* OHA-RQB */
typedef OHA_RQB_TYPE SNMPX_OHA_LOWER_RQB_TYPE;
typedef SNMPX_OHA_LOWER_RQB_TYPE *SNMPX_OHA_LOWER_RQB_PTR_TYPE;
typedef OHA_CHANNEL_TYPE         *SNMPX_OHA_OPEN_CHANNEL_PTR_TYPE;
typedef LSA_VOID                 *SNMPX_OHA_LOWER_MEM_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/



/*===========================================================================*/
/*====                     in functions from SOCK                        ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  snmpx_sock_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of SNMPX-lower request to XXX:
 *
 * parameters:     SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB   pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see sock_open_channel(), sock_close_channel()
 *                    respectively sock_request()
 *===========================================================================*/
LSA_VOID snmpx_sock_request_lower_done(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB
);

/*===========================================================================*/
/*====               out functions to SOCK                               ====*/
/*===========================================================================*/

/*=============================================================================
 * function name:  SNMPX_SOCK_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to SOCK lower layer
 *
 * parameters:     SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     LSA_USER_ID_TYPE   user-id:       id of prefix
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of prefix
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(ATTR, *_request_upper_done_ptr)
 *                                    (SNMPX_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
 *
 * RQB-return values via callback-function:
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *                        response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef SNMPX_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID SNMPX_SOCK_OPEN_CHANNEL_LOWER(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to SOCK lower layer
 *
 * parameters:     SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of prefix
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
#ifndef SNMPX_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID SNMPX_SOCK_CLOSE_CHANNEL_LOWER(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to SOCK lower layer
 *
 * parameters:     SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
#ifndef SNMPX_SOCK_REQUEST_LOWER
LSA_VOID SNMPX_SOCK_REQUEST_LOWER(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for SOCK synchronously
 *
 * parameters:     SNMPX_SOCK_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_SOCK_ALLOC_LOWER_RQB
LSA_VOID SNMPX_SOCK_ALLOC_LOWER_RQB(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                user_id,
    LSA_UINT16                      length,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from XXX
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *                 SNMPX_SOCK_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_SOCK_FREE_LOWER_RQB
LSA_VOID SNMPX_SOCK_FREE_LOWER_RQB(
    LSA_UINT16                    * ret_val_ptr,
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for EDD synchronously
 *
 * parameters:     SNMPX_SOCK_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_SOCK_ALLOC_LOWER_MEM
LSA_VOID SNMPX_SOCK_ALLOC_LOWER_MEM(
    SNMPX_SOCK_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                user_id,
    LSA_UINT16                      length,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_SOCK_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for SOCK
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 SNMPX_SOCK_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_SOCK_FREE_LOWER_MEM
LSA_VOID SNMPX_SOCK_FREE_LOWER_MEM(
    LSA_UINT16                    * ret_val_ptr,
    SNMPX_SOCK_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif


/*=============================================================================
 * function name:  SNMPX_OHA_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory for EDD synchronously
 *
 * parameters:     SNMPX_OHA_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_ALLOC_LOWER_MEM
LSA_VOID SNMPX_OHA_ALLOC_LOWER_MEM(
    SNMPX_OHA_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE               user_id,
    LSA_UINT16                     length,
    LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_OHA_FREE_LOWER_MEM
 *
 * function:       free a lower-memory for OHA
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system-pointer
 *                 SNMPX_OHA_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_FREE_LOWER_MEM
LSA_VOID SNMPX_OHA_FREE_LOWER_MEM(
    LSA_UINT16                   * ret_val_ptr,
    SNMPX_OHA_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
    LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  snmpx_oha_request_lower_done
 *
 * function:       callback-function
 *                 confirmation of SNMPX-lower request to XXX:
 *
 * parameters:     SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB  pointer to lower-
 *                                                     RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-return values, see oha_open_channel(), oha_close_channel()
 *                    respectively oha_request()
 *===========================================================================*/
LSA_VOID snmpx_oha_request_lower_done(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB
);

/*=============================================================================
 * function name:  SNMPX_OHA_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB for OHA synchronously
 *
 * parameters:     SNMPX_OHA_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 failed.
 *                 LSA_USER_ID_TYPE                   user_id:  not used
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_ALLOC_LOWER_RQB
LSA_VOID SNMPX_OHA_ALLOC_LOWER_RQB(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE               user_id,
    LSA_UINT16                     length,
    LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_OHA_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB from XXX
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-RQB or
 *                                                      wrong system-pointer
 *          SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:pointer to lower-RQB
 *                             LSA_SYS_PTR_TYPE sys_ptr:system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_FREE_LOWER_RQB
LSA_VOID SNMPX_OHA_FREE_LOWER_RQB(
    LSA_UINT16                   * ret_val_ptr,
    SNMPX_OHA_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
    LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_OHA_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to OHA lower layer
 *
 * parameters:     SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_OPEN_CHANNEL_LOWER
LSA_VOID SNMPX_OHA_OPEN_CHANNEL_LOWER(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_OHA_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to OHA lower layer
 *
 * parameters:     SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_CLOSE_CHANNEL_LOWER
LSA_VOID SNMPX_OHA_CLOSE_CHANNEL_LOWER(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*=============================================================================
 * function name:  SNMPX_OHA_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to OHA lower layer
 *
 * parameters:     SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                              RQB
 *                 LSA_SYS_PTR_TYPE             sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef SNMPX_OHA_REQUEST_LOWER
LSA_VOID SNMPX_OHA_REQUEST_LOWER(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file SNMPX_LOW.H                                                  */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_LOW_H */
