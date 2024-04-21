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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_out.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  EDDI output macros framework. Have to be filled by systemadaption.       */
/*  This file declares prototypes with no function if macro is not defined.  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_inc.h"
//#include "eddi_bsp_sys_int.h"

#if defined (EDDI_CFG_SOC)
#include "eddi_sys_soc_hw.h"
#else
#include "eddi_sys_ertec_hw.h"
#endif

#if defined (EDDI_CFG_ERTEC_200)
#include "eddi_sys_showlocation.h"
#endif

//#include "eddi_mau_types.h"

#if defined __cplusplus     //If C++ - compiler: Use C linkage
extern "C"
{
#endif

#define UNREFERENCED_PARAMETER(P)	{(void)(P);}


/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/

/*=============================================================================
 * function name:  EDDI_ALLOC_LOCAL_MEM()
 *
 * function:       allocate a local-memory
 *
 * parameters:     EDDI_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory available
 *                 LSA_UINT16                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_LOCAL_MEM( EDDI_LOCAL_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
                                                          LSA_UINT32                                      length )
{
    UNREFERENCED_PARAMETER(local_mem_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
}

/*=============================================================================
 * function name:  EDDI_FREE_LOCAL_MEM()
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 EDDI_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_LOCAL_MEM( LSA_UINT16               EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                         EDDI_LOCAL_MEM_PTR_TYPE                          local_mem_ptr )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(local_mem_ptr);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM()            +*/
/*+  Input/Output          :    EDDI_LOCAL_MEM_PTR_TYPE lower_mem_ptr_ptr   +*/
/*+                             LSA_UINT32              length              +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  local_mem_ptr_ptr  : Address of pointer for pointer to memoryspace     +*/
/*+  length             : Length of memory to allocate                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates length memory and returns the pointer in        +*/
/*+               lower_mem_ptr_ptr. NULL if no memory avail.               +*/
/*+               The memory-ptr has to be 8 Byte aligned                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// If identlical with ALLOC_LOCAL_MEM, the buffer has to be placed at a 64bit boundary.
// use of NON CACHED memory is mandatory !!!
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM( EDDI_SYS_HANDLE                                hSysDev,
                                                                    EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                                    LSA_UINT32                                     length,
                                                                    EDDI_USERMEMID_TYPE                            UserMemID )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(lower_mem_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
    UNREFERENCED_PARAMETER(UserMemID);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_FREE_DEV_SDRAM_ERTEC_MEM()             +*/
/*+  Input/Output          :    LSA_UINT16              ret_val_ptr         +*/
/*+                             EDDI_LOCAL_MEM_PTR_TYPE lower_mem_ptr       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ret_val_ptr        : Pointer to returnvalue                            +*/
/*+                                                                         +*/
/*+                       LSA_RET_OK                                        +*/
/*+                       LSA_RET_ERR_PARAM                                 +*/
/*+                                                                         +*/
/*+  local_mem_ptr      : Pointer to memory to free                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees previously allocated memory. We actually dont free  +*/
/*+               because we have no real memory-management. we only check  +*/
/*+               for NULL-Pointer.                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_DEV_SDRAM_ERTEC_MEM( EDDI_SYS_HANDLE                                hSysDev,
                                                                   LSA_UINT16             EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                                   EDDI_DEV_MEM_PTR_TYPE                          lower_mem_ptr,                                                                           
                                                                   EDDI_USERMEMID_TYPE                            UserMemID )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(lower_mem_ptr);
    UNREFERENCED_PARAMETER(UserMemID);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_ALLOC_DEV_SHARED_MEM()                 +*/
/*+  Input/Output          :    EDDI_LOCAL_MEM_PTR_TYPE  lower_mem_ptr_ptr   +*/
/*+                             LSA_UINT32              length              +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  local_mem_ptr_ptr  : Address of pointer for pointer to memoryspace     +*/
/*+  length             : Length of memory to allocate                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates length memory and returns the pointer in        +*/
/*+               lower_mem_ptr_ptr. NULL if no memory avail.               +*/
/*+               The memory-ptr has to be 8 Byte aligned                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// ERTEC can access the first 512MB directly over the PCI bus
// -> irrelevant for firmware
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_DEV_SHARED_MEM( EDDI_SYS_HANDLE                                hSysDev,
                                                               EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                               LSA_UINT32                                     length,                                                               
                                                               EDDI_USERMEMID_TYPE                            UserMemID )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(lower_mem_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
    UNREFERENCED_PARAMETER(UserMemID);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_FREE_DEV_SHARED_MEM()                  +*/
/*+  Input/Output          :    LSA_UINT16              ret_val_ptr         +*/
/*+                             EDDI_LOCAL_MEM_PTR_TYPE lower_mem_ptr       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ret_val_ptr        : Pointer to returnvalue                            +*/
/*+                                                                         +*/
/*+                       LSA_RET_OK                                        +*/
/*+                       LSA_RET_ERR_PARAM                                 +*/
/*+                                                                         +*/
/*+  local_mem_ptr      : Pointer to memory to free                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees previously allocated memory. We actually dont free  +*/
/*+               because we have no real memory-management. we only check  +*/
/*+               for NULL-Pointer.                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_DEV_SHARED_MEM( EDDI_SYS_HANDLE                     hSysDev,
                                                              LSA_UINT16  EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                              EDDI_DEV_MEM_PTR_TYPE               lower_mem_ptr,                                                             
                                                              EDDI_USERMEMID_TYPE                 UserMemID )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(lower_mem_ptr);
    UNREFERENCED_PARAMETER(UserMemID);
}


#if !defined EDDI_CFG_MAX_NR_PROVIDERS
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_IO_ALLOC_LOCAL_MEM()                   +*/
/*+  Input/Output          :    EDDI_LOCAL_MEM_PTR_TYPE local_mem_ptr_ptr   +*/
/*+                             LSA_UINT32              Length              +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  The function allocates "length" bytes of memory and returns a pointer  +*/
/*+  to this area. The memory area must be correctly aligned for all data   +*/
/*+  types (i.e. generally 32-bit aligned). If a NULL pointer returned,     +*/
/*+  no memory could be allocated. With this function, the EDDI allocates   +*/
/*+  internal management structures for the 3B-IF in SW and for the         +*/
/*+  systemredundancy functions.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IO_ALLOC_LOCAL_MEM (  EDDI_LOCAL_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
                                                              LSA_UINT32                                      length)
{
    UNREFERENCED_PARAMETER(local_mem_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_IO_FREE_LOCAL_MEM()                    +*/
/*+  Input/Output          :    EDDI_SYS_HANDLE         hSysDev             +*/
/*+                             LSA_UINT16 *            ret_val_ptr         +*/
/*+                             EDDI_LOCAL_MEM_PTR_TYPE local_mem_ptr_ptr   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: The function first releases memory allocated earlier with +*/
/*+  EDDI_IO_ALLOC_LOCAL_MEM. local_mem_ptr is the pointer returned with    +*/
/*   EDDI_IO_ALLOC_LOCAL_MEM.                                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IO_FREE_LOCAL_MEM ( EDDI_SYS_HANDLE                                  hSysDev,
                                                             LSA_UINT16                EDDI_LOCAL_MEM_ATTR  * ret_val_ptr,
                                                             EDDI_LOCAL_MEM_PTR_TYPE                          local_mem_ptr)
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(local_mem_ptr);
}
#endif

/*=============================================================================*/
/*        MEMSET-COPY-FUNCTIONS                                                */
/*=============================================================================*/

/*=============================================================================
 * function name:  EDDI_MEMSET()
 *
 * function:       fills memory with value
 *
 * parameters:    EDDI_LOCAL_MEM_PTR_TYPE         pMem
 *          LSA_UINT8                      Value
 *          LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_MEMSET( EDDI_LOCAL_MEM_PTR_TYPE  pMem,
                                                 LSA_UINT8                Value,
                                                 LSA_UINT32               Length )
{
    UNREFERENCED_PARAMETER(pMem);
    UNREFERENCED_PARAMETER(Value);
    UNREFERENCED_PARAMETER(Length);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_MEMCOPY()                              +*/
/*+  Input/Output               EDDI_LOCAL_MEM_PTR_TYPE pMem                +*/
/*+  Input/Output          :    LSA_UINT8               Value               +*/
/*+                             LSA_UINT                Size                +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pMem                 : Pointer to memory to initialize                 +*/
/*+  Value                : Value for init                                  +*/
/*+  Size                 : Number of bytes to init                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Inits memory with Value                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_MEMCOPY( EDDI_DEV_MEM_PTR_TYPE    pDest,
                                                  EDDI_LOCAL_MEM_PTR_TYPE  pSrc,
                                                  LSA_UINT                 Size )
{
    UNREFERENCED_PARAMETER(pDest);
    UNREFERENCED_PARAMETER(pSrc);
    UNREFERENCED_PARAMETER(Size);
}

/*****************************************************************************/
/*                                                                           */
/* Output-functions for System                                               */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  EDDI_GET_PATH_INFO()
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_RESULT              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 EDDI_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_GET_PATH_INFO( LSA_RESULT            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                        LSA_SYS_PTR_TYPE      EDDI_LOCAL_MEM_ATTR  *  sys_ptr_ptr,
                                                        EDDI_DETAIL_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  detail_ptr_ptr,
                                                        LSA_SYS_PATH_TYPE                             path )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(sys_ptr_ptr);
    UNREFERENCED_PARAMETER(detail_ptr_ptr);
    UNREFERENCED_PARAMETER(path);
}

/*=============================================================================
 * function name:  EDDI_RELEASE_PATH_INFO()
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_RESULT      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system-pointer or
 *                                                      wrong detail-pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system-pointer
 *                 EDDI_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_RELEASE_PATH_INFO( LSA_RESULT            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                            LSA_SYS_PTR_TYPE                              sys_ptr,
                                                            EDDI_DETAIL_PTR_TYPE                          detail_ptr )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(sys_ptr);
    UNREFERENCED_PARAMETER(detail_ptr);
}

/*=============================================================================
 * function name:  EDDI_FATAL_ERROR()
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                 length:     number of bytes
 *                 EDDI_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                        LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FATAL_ERROR( LSA_UINT16                 length,
                                                      EDDI_FATAL_ERROR_PTR_TYPE  error_ptr )
{
    UNREFERENCED_PARAMETER(length);
    UNREFERENCED_PARAMETER(error_ptr);
}

/*=============================================================================*/
/*        TIMER-FUNCTIONS                                                      */
/*=============================================================================*/

/*=============================================================================
 * function name:  EDDI_ALLOC_TIMER()
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_RESULT  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated
 *                                         LSA_RET_ERR_NO_TIMER  no timer has
 *                                                               been allocated
 *                 LSA_TIMER_ID_TYPE  ...  *  timer_id_ptr:
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
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_TIMER( LSA_RESULT         EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                      LSA_TIMER_ID_TYPE  EDDI_LOCAL_MEM_ATTR  *  timer_id_ptr,
                                                      LSA_UINT16                                 timer_type,
                                                      LSA_UINT16                                 time_base )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(timer_id_ptr);
    UNREFERENCED_PARAMETER(timer_type);
    UNREFERENCED_PARAMETER(time_base);
}

/*=============================================================================
 * function name:  EDDI_START_TIMER()
 *
 * function:       start a timer
 *
 * parameters:     LSA_RESULT  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn´t
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time
 *                                 system will call eddi_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of prefix
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_START_TIMER( LSA_RESULT         EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                      LSA_TIMER_ID_TYPE                          timer_id,
                                                      LSA_USER_ID_TYPE                           user_id,
                                                      LSA_UINT16                                 time )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(timer_id);
    UNREFERENCED_PARAMETER(user_id);
    UNREFERENCED_PARAMETER(time);
}

/*=============================================================================
 * function name:  EDDI_STOP_TIMER()
 *
 * function:       stop a timer
 *
 * parameters:     LSA_RESULT  ...  *  ret_val_ptr:
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
 *                 LSA_TIMER_ID_TYPE         timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_STOP_TIMER( LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                     LSA_TIMER_ID_TYPE                   timer_id )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(timer_id);
}

/*=============================================================================
 * function name:  EDDI_FREE_TIMER()
 *
 * function:       free a timer
 *
 * parameters:     LSA_RESULT  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_TIMER_ID_TYPE               timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_TIMER( LSA_RESULT         EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                     LSA_TIMER_ID_TYPE                          timer_id )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(timer_id);
}

/*=============================================================================*/
/*        GLOBAL-FUNCTIONS                                                     */
/*=============================================================================*/

/*=============================================================================
 * function name:  EDDI_RQB_ERROR()
 *
 * function:       notify a RQB-error in a user/system-request.
 *                 this function is called if there is an error (i.e. param)
 *                 within RQB which prevents notification of the caller
 *                 (i.e. missing call-back-function). the error-code is set
 *                 in "Status" in RQB. this error is typically an implementation
 *                 error.
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_RQB_ERROR( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(pRQB);
}

#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
/*=============================================================================
 * function name:  EDDI_DO_EV_INTERRUPT_PRIO1_AUX()
 *
 * function:       ...
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO1_AUX( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(pRQB);
}
#endif

/*=============================================================================
 * function name:  EDDI_DO_EV_INTERRUPT_PRIO2_ORG()
 *
 * function:       ...
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO2_ORG( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(pRQB);
}

/*=============================================================================
 * function name:  EDDI_DO_EV_INTERRUPT_PRIO3_REST()
 *
 * function:       ...
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO3_REST( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(pRQB);
}

/*=============================================================================
 * function name:  EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW()
 *
 * function:       ...
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(pRQB);
}

/*=============================================================================
 * function name:  EDDI_SII_USER_INTERRUPT_HANDLING_STARTED()
 *
 * function:       SII announces the start of the SII USER interrupt handling in User-Int-Share-Mode
 *
 * parameters:     EDDI_HANDLE  hDDB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( EDDI_HANDLE  hDDB )
{
    UNREFERENCED_PARAMETER(hDDB);
}

/*=============================================================================
 * function name:  EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED()
 *
 * function:       SII announces the stop of the SII USER interrupt handling in User-Int-Share-Mode
 *
 * parameters:     EDDI_HANDLE  hDDB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED( EDDI_HANDLE  hDDB )
{
    UNREFERENCED_PARAMETER(hDDB);
}

/*=============================================================================
ATTENTION: Ab hier sind es KEINE EDDI Ausgangsmakros, d.h. diese Funktionen werden intern vom EDDI
           nicht benötigt - diese Macros werden normalerweise von den ueberlagerten Schichten genutzt!
*/

/*=============================================================================
 * function name:  EDDI_ALLOC_UPPER_RQB_LOCAL()
 *
 * function:       allocate an upper-RQB for local use
 *                 the memory has to be initialized with 0
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_UPPER_RQB_LOCAL( EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  upper_rqb_ptr_ptr,
                                                                LSA_UINT16                                      length )
{
    UNREFERENCED_PARAMETER(upper_rqb_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
}

/*=============================================================================
 * function name:  EDDI_FREE_UPPER_RQB_LOCAL()
 *
 * function:       free an upper-RQB allocated with EDDI_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                 EDD_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_UPPER_RQB_LOCAL( LSA_UINT16              EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                               EDD_UPPER_RQB_PTR_TYPE                          upper_rqb_ptr )
{
    UNREFERENCED_PARAMETER(upper_rqb_ptr);
    UNREFERENCED_PARAMETER(ret_val_ptr);
}

/*=============================================================================
 * function name:  EDDI_ALLOC_UPPER_MEM_LOCAL()
 *
 * function:       allocate an upper-mem for local use
 *                 the memory has to be initialized with 0
 *
 * parameters:     EDD_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ALLOC_UPPER_MEM_LOCAL( EDD_UPPER_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  upper_mem_ptr_ptr,
                                                                LSA_UINT16                                      length )
{
    UNREFERENCED_PARAMETER(upper_mem_ptr_ptr);
    UNREFERENCED_PARAMETER(length);
}

/*=============================================================================
 * function name:  EDDI_FREE_UPPER_MEM_LOCAL()
 *
 * function:       free an upper-mem allocated with EDDI_ALLOC_UPPER_MEM_LOCAL
 *
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-mem or
 *                 EDD_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                         memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_FREE_UPPER_MEM_LOCAL( LSA_UINT16              EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                               EDD_UPPER_MEM_PTR_TYPE                          upper_mem_ptr )
{
    UNREFERENCED_PARAMETER(upper_mem_ptr);
    UNREFERENCED_PARAMETER(ret_val_ptr);
}

/*=============================================================================
 * function name:  EDDI_ENTER_COM()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_COM( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_COM()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_COM( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_REST()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_REST( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_REST()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_REST( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_SYNC()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SYNC( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_SYNC()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SYNC( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_IO()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_IO( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_IO()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_IO( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_IO_KRAM()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_IO_KRAM( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_IO_KRAM()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_IO_KRAM( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_BUFF_EXCH
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_BUFF_EXCH( LSA_VOID )
{
}


/*=============================================================================
 * function name:  EDDI_EXIT_BUFF_EXCH
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_BUFF_EXCH( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_APPLSYNC()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_APPLSYNC( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_APPLSYNC()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_APPLSYNC( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_SND_CHA0()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SND_CHA0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_SND_CHA0()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SND_CHA0 ( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_SND_CHB0()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SND_CHB0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_SND_CHB0()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SND_CHB0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_RCV_CHA0()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_RCV_CHA0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_RCV_CHA0()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_RCV_CHA0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_RCV_CHB0()
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_RCV_CHB0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_RCV_CHB0()
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_RCV_CHB0( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_CRITICAL()
 *
 * function:       set lock for a critical section
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_CRITICAL( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_CRITICAL()
 *
 * function:       cancel lock for a critical section
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_CRITICAL( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_SII_CRITICAL()
 *
 * function:       set lock for a critical section in SII
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SII_CRITICAL( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_SII_CRITICAL()
 *
 * function:       cancel lock for a critical section in SII
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SII_CRITICAL( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_ENTER_I2C()
 *
 * function:       Entry into a protected I2C area
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_I2C( LSA_VOID )
{
}

/*=============================================================================
 * function name:  EDDI_EXIT_I2C()
 *
 * function:       Exit from the protected I2C area
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_I2C( LSA_VOID )
{
}

#if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)
/*=============================================================================
 * function name:  EDDI_SII_APPLSYNC_NEWCYCLE()
 *
 * function:       SII announces a New-Cycle-Interrupt to Appl Sync
 *
 * parameters:     EDDI_HANDLE  hDDB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SII_APPLSYNC_NEWCYCLE( EDDI_HANDLE  hDDB )
{
    UNREFERENCED_PARAMETER(hDDB);
}
#endif

/*=============================================================================
 * function name:  EDDI_SIGNAL_SENDCLOCK_CHANGE()
 *
 * function:       get NewCycle reduction from system adaption
 *                 - Function is called during
 *                   1. EDDI_SRV_DEV_COMP_INI
 *                   2. EDD_SRV_SENDCLOCK_CHANGE
 *
 *                 -> !! eddi_interrupt(g_SyshDDB, EDDI_INT_newCycle) !!
 *
 * parameter:       LSA_UINT32         CycleBaseFactor
 *                    -> Cycle Time(us) = (CycleBaseFactor * 3125) / 100
 *                    -> Cycle Time(us) * NewCycleReduction(Returnvalue) <= 32000 ( 32ms ) !!!!
 *
 * parameter:       EDDI_SYS_HANDLE     hSysDev
 *                    -> selects the device
 *                    -> if you have only one hardware, the parameter can be ignored
 *
 * parameter:       LSA_UINT8           Mode
 *                    -> see EDDI_SENDCLOCK_CHANGE_xxx in eddi_usr.h
 *
 * return value:    LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SIGNAL_SENDCLOCK_CHANGE( EDDI_SYS_HANDLE  hSysDev,
                                                                  LSA_UINT32       CycleBaseFactor,
                                                                  LSA_UINT8        Mode )
{
    UNREFERENCED_PARAMETER(hSysDev);

    EDDI_SysSetSendClockChange(CycleBaseFactor);

    UNREFERENCED_PARAMETER(Mode);
}

/*=============================================================================
 * function name:  EDDI_LL_XPLL_SETPLLMODE_OUT()
 *
 * function:       initializes a GPIO for PLL-Out/In
 *
 *
 * parameters:
 *  - hDDB          = Handle
 *  - pIRTE         = Ptr to the IRTE-Reg-Base
 *  - location      = location: LOC_ERTEC400
 *                              LOC_ERTEC200
 *                              LOC_SOC1
 *                              LOC_SOC2
 *
 *  - pllmode       = EDDI_PRM_DEF_PLL_MODE_OFF
 *                    EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT
 *                    EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN
 *
 * return value:   LSA_RESULT (EDD_STS_OK or EDD_STS_ERR_PARAM)
 *===========================================================================*/
/*!!! example for implementation only !!!*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_XPLL_SETPLLMODE_OUT( LSA_RESULT       *  result,
                                                                 EDDI_SYS_HANDLE     hSysDev,
                                                                 LSA_UINT32          pIRTE,
                                                                 LSA_UINT32          location,
                                                                 LSA_UINT16          pllmode )
{
    #if defined (EDDI_CFG_SOC)
    *result = EDDI_CpSocSetPllPort(hSysDev, pIRTE, location, pllmode);
    #else
    *result = EDDI_CpErtecSetPllPort(hSysDev, pIRTE, location, pllmode);
    #endif
}

/*=============================================================================
 * function name:  EDDI_LL_GET_PHY_PARAMS()
 *
 * function:       Obtains the Phy-parameters from the system integration
 *
 *
 * parameters:
 *  - hDDB          = Handle
 *  - HwPortIndex   = HW-Port, 0..n
 *  - Speed         = LinkSpeed: EDD_LINK_UNKNOWN
 *                               EDD_LINK_SPEED_10
 *                               EDD_LINK_SPEED_100
 *                               EDD_LINK_SPEED_1000
 *                               EDD_LINK_SPEED_10000
 *  - Mode          = LinkMode:  EDD_LINK_UNKNOWN
 *                               EDD_LINK_MODE_HALF
 *                               EDD_LINK_MODE_FULL
 *
 * return value:
 * - AutonegCapAdvertised = see EDD_AUTONEG_CAP_xxx in EDD_USR.H
 * - AutonegMappingCap    = see EDD_LINK_xxx in EDD_USR.H
 * - MAUType              = MAU-Type according to IEC, not checked in EDDI
 * - MediaType            = Media-Type according to IEC, not checked in EDDI
 * - IsPOF                = If optical, set for a "real" POF port but cleared for an optical, but non-POF port,
 * - TransceiverType      = Type of the optical transceiver
 *===========================================================================*/
/*!!! example for implementation only !!!*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_GET_PHY_PARAMS( EDDI_SYS_HANDLE     hSysDev,
                                                            LSA_UINT32          HwPortIndex,
                                                            LSA_UINT8           Speed,
                                                            LSA_UINT8           Mode,
                                                            LSA_UINT32       *  pAutonegCapAdvertised,
                                                            LSA_UINT32       *  pAutonegMappingCap,
                                                            LSA_UINT16       *  pMAUType,
                                                            LSA_UINT8        *  pMediaType,
                                                            LSA_UINT8        *  pIsPOF,
                                                            LSA_UINT8        *  pFXTransceiverType )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(HwPortIndex);
    UNREFERENCED_PARAMETER(Speed);
    UNREFERENCED_PARAMETER(Mode);
    UNREFERENCED_PARAMETER(pAutonegCapAdvertised);
    UNREFERENCED_PARAMETER(pAutonegMappingCap);
    UNREFERENCED_PARAMETER(pMAUType);
    UNREFERENCED_PARAMETER(pMediaType);
    UNREFERENCED_PARAMETER(pIsPOF);
    UNREFERENCED_PARAMETER(pFXTransceiverType);
}

/*=============================================================================
 * function name:  EDDI_LL_CHECK_PHY_PARAMS()
 *
 * function:       Is called to make the system integration check the Phy-parameters
 *
 *
 * parameters:
 *  - hDDB                 = Handle
 *  - HwPortIndex          = HW-Port, 0..n
 *  - AutonegCapAdvertised = see EDD_AUTONEG_CAP_xxx in EDDI_USR.H, obtained before with EDDI_LL_GET_PHY_PARAMS
 *  - AutonegMappingCap    = see EDD_LINK_xxx in EDDI_USR.H, obtained before with EDDI_LL_GET_PHY_PARAMS
 *  - MAUType              = MAU-Type according to IEC, obtained before with EDDI_LL_GET_PHY_PARAMS
 *
 * return value:
 *  - Speed         = LinkSpeed: EDD_LINK_UNKNOWN
 *                               EDD_LINK_SPEED_10
 *                               EDD_LINK_SPEED_100
 *                               EDD_LINK_SPEED_1000
 *                               EDD_LINK_SPEED_10000
 *  - Mode          = LinkMode:  EDD_LINK_UNKNOWN
 *                               EDD_LINK_MODE_HALF
 *                               EDD_LINK_MODE_FULL
 *===========================================================================*/
/*!!! example for implementation only !!!*/
LSA_RESULT  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_CHECK_PHY_PARAMS( EDDI_SYS_HANDLE     hSysDev,
                                                                LSA_UINT32          HwPortIndex,
                                                                LSA_UINT32          AutonegCapAdvertised,
                                                                LSA_UINT32          AutonegMappingCap,
                                                                LSA_UINT16          MAUType,
                                                                LSA_UINT8        *  pSpeed,
                                                                LSA_UINT8        *  pMode )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(HwPortIndex);
    UNREFERENCED_PARAMETER(AutonegCapAdvertised);
    UNREFERENCED_PARAMETER(AutonegMappingCap);
    UNREFERENCED_PARAMETER(MAUType);
    UNREFERENCED_PARAMETER(pSpeed);
    UNREFERENCED_PARAMETER(pMode);

    return LSA_OK;
}

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM) || defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC) || defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC) || defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI) || defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)

/*=============================================================================
 * function name:  EDDI_LL_LED_BLINK_BEGIN()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before the LED(s) start to blink.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *                 This function is only called by EDDI if compiler switch 
 *                 EDDI_CFG_PHY_BLINK_EXTERNAL_XXX is set.
 *
 * parameters:     hSysDev:         selects the device
 *                 HwPortIndex:     Hardware Port Index
 *                 PhyTransceiver:  selects the PHY
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_LL_LED_BLINK_BEGIN
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_BEGIN( EDDI_SYS_HANDLE            hSysDev,
                                                             LSA_UINT32                 HwPortIndex,
                                                             EDDI_PHY_TRANSCEIVER_TYPE  PhyTransceiver )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(HwPortIndex);
    UNREFERENCED_PARAMETER(PhyTransceiver);

    #if defined (EDDI_CFG_ERTEC_200)
    if (PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC)
    {
        EDDI_SetPortModeEB200(HwPortIndex, LSA_TRUE);
    }
    #endif
}
#endif

/*=============================================================================
 * function name:  EDDI_LL_LED_BLINK_SET_MODE()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *                 This function is only called by EDDI if compiler switch 
 *                 EDDI_CFG_PHY_BLINK_EXTERNAL_XXX is set.
 *
 * parameters:     hSysDev:         selects the device
 *                 HwPortIndex      Hardware port index
 *                 PhyTransceiver:  selects the PHY
 *                 LEDMode          EDDI_LED_MODE_ON / EDDI_LED_MODE_OFF
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_LL_LED_BLINK_SET_MODE
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_SET_MODE( EDDI_SYS_HANDLE            hSysDev,
                                                                LSA_UINT32                 HwPortIndex,
                                                                EDDI_PHY_TRANSCEIVER_TYPE  PhyTransceiver,
                                                                LSA_UINT16                 LEDMode )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(HwPortIndex);
    UNREFERENCED_PARAMETER(PhyTransceiver);
    UNREFERENCED_PARAMETER(LEDMode);

    #if defined (EDDI_CFG_ERTEC_200)
    if (PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC)
    {
        EDDI_SetLEDOnOffEB200(HwPortIndex, LEDMode);
    }
    #endif
}
#endif

/*=============================================================================
 * function name:  EDDI_LL_LED_BLINK_END()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() after blinking of LED(s) has finished.
 *                 Can be used e.g. to re-enable the link/activity LED function of
 *                 a PHY if necessary.
 *                 This function is only called by EDDI if compiler switch 
 *                 EDDI_CFG_PHY_BLINK_EXTERNAL_XXX is set.
 *
 * parameters:     hSysDev:         selects the device
 *                 HwPortIndex:     Hardware Port Index
 *                 PhyTransceiver:  selects the PHY
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_LL_LED_BLINK_END
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_END( EDDI_SYS_HANDLE            hSysDev,
                                                           LSA_UINT32                 HwPortIndex,
                                                           EDDI_PHY_TRANSCEIVER_TYPE  PhyTransceiver )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(HwPortIndex);
    UNREFERENCED_PARAMETER(PhyTransceiver);

    #if defined (EDDI_CFG_ERTEC_200)
    if (PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC)
    {
        EDDI_SetPortModeEB200(HwPortIndex, LSA_FALSE);
    }
    #endif
}
#endif

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_XXX

/*=============================================================================
 * function name:  EDDI_WAIT_10_NS()
 *
 * function:       This function waits 10 ns * ticks_10ns.
 *
 * parameters:     hSysDev:    selects the device
 *                 ticks_10ns: waiting-time in units of 10ns
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_WAIT_10_NS( EDDI_SYS_HANDLE    hSysDev,
                                                     LSA_UINT32         ticks_10ns )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(ticks_10ns);
}

/*============================================================================= 
 * function name:  EDDI_REQUEST_UPPER_DONE()
 * 
 * function:       returns a finished request to the upper layer 
 * 
 * parameters:     EDDI_UPPER_CALLBACK_FCT_PTR_TYPE  
 *                     Cbf: 
 *                     pointer to eddi_request_upper_done_ptr, given by 
 *                     eddi_open_channel() 
 * 
 *                 EDD_UPPER_RQB_PTR_TYPE     upper_rqb_ptr: pointer to upper-RQB 
 *                 LSA_SYS_PTR_TYPE           sys_ptr:       system-pointer
 * 
 * return value:   LSA_VOID 
 * 
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_REQUEST_UPPER_DONE( EDD_UPPER_CALLBACK_FCT_PTR_TYPE  Cbf,    
                                                          	 EDD_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,    
                                                        	 LSA_SYS_PTR_TYPE                 sys_ptr )
{
    UNREFERENCED_PARAMETER(sys_ptr);

    Cbf(upper_rqb_ptr);
}

/*=============================================================================
 * function name:  EDDI_I2C_SCL_LOW_HIGHZ()
 *
 * function:       Sets the SCL signal of the selected I2C device to level low or highz
 *                 Set SCL to highz level: set SCL to input => set to Tristate => high level is set via pull up resistor
 *                 Set SCL to low level:  set SCL to output => set SCL to low level
 *
 *                 If the output of SCL-GPIO is initialized with 0 and not changed by other functions,
 *                 this function only has to switch between input (HIGHZ) and output(LOW).
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_I2C_SCL_LOW_HIGHZ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SCL_LOW_HIGHZ( EDDI_SYS_HANDLE	 const  hSysDev,
                                                           LSA_UINT8         const  Level )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(Level);
}
#endif

/*=============================================================================
 * function name:  EDDI_I2C_SDA_LOW_HIGHZ()
 *
 * function:       Sets the SDA signal of the selected I2C device to level low or highz
 *                 Set SDA to highz level: set SDA to input => set to Tristate => high level is set via pull up resistor
 *                 Set SDA to low level:  set SDA to output => set SDA to low level
 *
 *                 If the output of SDA-GPIO is initialized with 0 and not changed by other functions, 
 *                 this function only has to switch between input (HIGHZ) and output(LOW).
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_I2C_SDA_LOW_HIGHZ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SDA_LOW_HIGHZ( EDDI_SYS_HANDLE	 const  hSysDev,
                                                           LSA_UINT8         const  Level )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(Level);
}
#endif

/*=============================================================================
 * function name:  EDDI_I2C_SDA_READ()
 *
 * function:       Reads the SDA signal of the selected I2C device (0 or 1)
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_I2C_SDA_READ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SDA_READ( EDDI_SYS_HANDLE	                  const  hSysDev,
                                                       LSA_UINT8  EDDI_LOCAL_MEM_ATTR  *  const  value_ptr )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(value_ptr);
}
#endif

/*=============================================================================
 * function name:  EDDI_I2C_SELECT()
 *
 * function:       Selects an I2C port for the following I2C transfers
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_I2C_SELECT
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR 	EDDI_I2C_SELECT(LSA_UINT8  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
		                                             EDDI_SYS_HANDLE			       const  hSysDev,
		                                             LSA_UINT16                        const  PortId,
		                                             LSA_UINT16          			   const  I2CMuxSelect )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(PortId);
    UNREFERENCED_PARAMETER(I2CMuxSelect);
}
#endif

/*=============================================================================
 * function name:  EDDI_LL_I2C_WRITE_OFFSET_SOC()
 *
 * function:       Writes n bytes to the I2C device via SOC-I2C-HW
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if defined (EDDI_CFG_REV7)
#ifndef EDDI_LL_I2C_WRITE_OFFSET_SOC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_I2C_WRITE_OFFSET_SOC( LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
	                                                              EDDI_SYS_HANDLE			          const  hSysDev,
	                                                              LSA_UINT8			   	              const  I2CDevAddr,
	                                                              LSA_UINT8			   	              const  I2COffsetCnt,
	                                                              LSA_UINT8			   	              const  I2COffset1,
	                                                              LSA_UINT8			   	              const  I2COffset2,
	                                                              LSA_UINT32                          const  Size,
	                                                              LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(I2CDevAddr);
    UNREFERENCED_PARAMETER(I2COffsetCnt);
    UNREFERENCED_PARAMETER(I2COffset1);
    UNREFERENCED_PARAMETER(I2COffset2);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(pBuf);
}
#endif
#endif

/*=============================================================================
 * function name:  EDDI_LL_I2C_READ_OFFSET_SOC()
 *
 * function:       Reads n bytes from the I2C device via SOC-I2C-HW
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if defined (EDDI_CFG_REV7)
#ifndef EDDI_LL_I2C_READ_OFFSET_SOC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_I2C_READ_OFFSET_SOC( LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
	                                                             EDDI_SYS_HANDLE			         const  hSysDev,
	                                                             LSA_UINT8			   	             const  I2CDevAddr,
	                                                             LSA_UINT8			   	             const  I2COffsetCnt,
	                                                             LSA_UINT8			   	             const  I2COffset1,
	                                                             LSA_UINT8			   	             const  I2COffset2,
	                                                             LSA_UINT32                          const  Size,
	                                                             LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
    UNREFERENCED_PARAMETER(ret_val_ptr);
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(I2CDevAddr);
    UNREFERENCED_PARAMETER(I2COffsetCnt);
    UNREFERENCED_PARAMETER(I2COffset1);
    UNREFERENCED_PARAMETER(I2COffset2);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(pBuf);
}
#endif
#endif

/*=============================================================================
 * function name:  EDDI_DETECTED_RECEIVE_LIMIT()
 *
 * function:       If receive-limit is reached, then the eddi would trigger this makro
 *
 * parameters:     hDDB:         Device handle  
 *                 hSysDev       Sys Handle                
 *                 NRTChannel:   EDDI_NRT_CHANEL_A_IF_0,         
 *                               EDDI_NRT_CHANEL_B_IF_0,              
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_DETECTED_RECEIVE_LIMIT
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DETECTED_RECEIVE_LIMIT( const  EDDI_HANDLE  const  hDDB,
                                                                 EDDI_SYS_HANDLE            hSysDev,
                                                                 LSA_UINT32                 NRTChannel )
{
    UNREFERENCED_PARAMETER(hDDB);
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(NRTChannel);
}
#endif

/*=============================================================================
 * function name:  EDDI_NRT_SEND_HOOK()
 *
 * function:       This macro is called by EDDI just before a NRT send RQB 
 *                 from a EDD_SRV_NRT_SEND request is queued in the DMACW table 
 *                 for sending.
 *                 The macro gets the RQB and can modify the framebuffer 
 *                 content referenced by pPuffer within the RQB. 
 *                 All parameters within the RQB shall not be changed! 
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDI_NRT_SEND_HOOK
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_NRT_SEND_HOOK( EDDI_SYS_HANDLE         hSysDev,
                                                        EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    UNREFERENCED_PARAMETER(hSysDev);
    UNREFERENCED_PARAMETER(pRQB);
}
#endif



#if defined __cplusplus
}
#endif


/*****************************************************************************/
/*  end of file eddi_out.c                                                   */
/*****************************************************************************/

