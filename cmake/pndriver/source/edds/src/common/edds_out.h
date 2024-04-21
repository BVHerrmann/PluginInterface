
#ifndef EDDS_OUT_H                    /* ----- reinclude-protection ----- */
#define EDDS_OUT_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_out.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  EDDS output macros framework. Have to be filled by systemadaption.       */
/*  This file declares prototypes with no function if macro is not defined   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  02.10.03    JS    added EDDS_MEMSET_UPPER                                */
/*  12.11.03    JS    #ifndef EDDS_MEMCOPY_UPPER (copy/paste error)          */
/*  05.02.03    JS    Optional MACRO-Call on tx-timeout for debugging        */
/*  05.03.04    JS    added EDDS_CSRT_CONSUMER_CYCLE_START_EVENT and         */
/*                    EDDS_CSRT_CONSUMER_CYCLE_END_EVENT                     */
/*  27.07.04    JS    EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE added to support */
/*                    middle-prio with consumer check                        */
/*  21.01.05    JS    added Rx-overload check support                        */
/*                    removed unused old ASRT functions.                     */
/*  04.10.05    JS    added LED-blink support                                */
/*  13.03.06    JS    added EDDS_DO_DIAG_TIMER_REQUEST                       */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
/*  30.01.09    JS    added EDDS_CHECK_AND_DECODE_MAUTYPE,EDDS_CODE_MAUTYPE  */
/*  17.01.11    MA    added EDDP_REQUEST_UPPER_DONE                          */
#endif
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Output-functions for System                                               */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  EDDS_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 EDDS_DETAIL_PTR_TYPE ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_GET_PATH_INFO
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PATH_INFO(
    LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_SYS_PTR_TYPE        EDDS_LOCAL_MEM_ATTR * sys_ptr_ptr,
    EDDS_DETAIL_PTR_TYPE   EDDS_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                             path
);
#endif


/*=============================================================================
 * function name:  EDDS_RELEASE_PATH_INFO
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
 *                 EDDS_DETAIL_PTR_TYPE detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_RELEASE_PATH_INFO
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RELEASE_PATH_INFO(
    LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    EDDS_DETAIL_PTR_TYPE detail_ptr
);
#endif


/*=============================================================================
 * function name:  EDDS_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     EDDS_UPPER_CALLBACK_FCT_PTR_TYPE
 *                                            edds_request_upper_done_ptr:
 *                     pointer to edds_request_upper_done_ptr, given by
 *                     edds_open_channel()
 *
 *                 EDDS_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef EDDS_REQUEST_UPPER_DONE
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_REQUEST_UPPER_DONE(
	EDD_UPPER_CALLBACK_FCT_PTR_TYPE     Cbf,
	EDD_UPPER_RQB_PTR_TYPE              upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                    sys_ptr
);
#endif


/*=============================================================================
 * function name:  EDDS_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     EDDS_LOCAL_MEM_PTR_TYPE ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_ALLOC_LOCAL_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_LOCAL_MEM(
    EDDS_LOCAL_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  EDDS_FREE_LOCAL_MEM
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 EDDS_LOCAL_MEM_PTR_TYPE local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FREE_LOCAL_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_LOCAL_MEM(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDDS_LOCAL_MEM_PTR_TYPE                          local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  EDDS_ALLOC_LOCAL_FAST_MEM
 *
 * function:       allocate a fast local-memory
 *
 * parameters:     EDDS_LOCAL_MEM_PTR_TYPE ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_ALLOC_LOCAL_FAST_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_LOCAL_FAST_MEM(
    EDDS_LOCAL_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  EDDS_FREE_LOCAL_FAST_MEM
 *
 * function:       frees local fast-memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 EDDS_LOCAL_MEM_PTR_TYPE local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FREE_LOCAL_FAST_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_LOCAL_FAST_MEM(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDDS_LOCAL_MEM_PTR_TYPE                          local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
 *
 * function:       allocate memory for receive transfer data which
 *                 must be accesseable by the ethernetcontroller (most likley
 *                 it must be an uncached area). The memory resides in upper
 *                 memory-space. The pyhsical address of the memory has
 *                 to be aligned depending on HW.
 *
 * parameters:     EDDS_SYS_HANDLE                 hSysDev: system handle
 *                 EDD_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-transfer-
 *                                                 memory-pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
    EDDS_SYS_HANDLE                               hSysDev,
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  EDDS_FREE_RX_TRANSFER_BUFFER_MEM
 *
 * function:       frees memory allocated with EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
 *
 * parameters:     EDDS_SYS_HANDLE                      hSysDev: system handle
 *                     ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 PREFIX_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            transfer memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
    EDDS_SYS_HANDLE                                  hSysDev,
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                           upper_mem_ptr
);
#endif

/*=============================================================================
 * function name:  EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
 *
 * function:       allocate memory for transmit transfer data which
 *                 must be accesseable by the ethernetcontroller (most likley
 *                 it must be an uncached area). The memory resides in upper
 *                 memory-space. The pyhsical address of the memory has
 *                 to be aligned depending on HW.
 *
 *                 This memory has to be DWORD-aligned!!
 *
 * parameters:     EDDS_SYS_HANDLE                 hSysDev: system handle
 *                 EDD_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-transfer-
 *                                                 memory-pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
    EDDS_SYS_HANDLE                               hSysDev,
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  EDDS_FREE_TX_TRANSFER_BUFFER_MEM
 *
 * function:       frees memory allocated with EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
 *
 * parameters:     EDDS_SYS_HANDLE                      hSysDev: system handle
 *                     ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 PREFIX_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            transfer memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
    EDDS_SYS_HANDLE                                  hSysDev,
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                           upper_mem_ptr
);
#endif

/*=============================================================================
 * function name:  EDDS_ALLOC_DEV_BUFFER_MEM
 *
 * function:       allocate memory for device data (e.g. descriptors) which
 *                 must be accesseable by the ethernetcontroller (most likley
 *                 it must be an uncached area). The memory resides in upper
 *                 memory-space. The pyhsical address of the memory has
 *                 to be aligned depending on HW.
 *
 * parameters:     EDDS_SYS_HANDLE                 hSysDev: system handle
 *                 EDD_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-transfer-
 *                                                 memory-pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                               hSysDev,
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif

/*=============================================================================
 * function name:  EDDS_FREE_DEV_BUFFER_MEM
 *
 * function:       frees memory allocated with EDDS_ALLOC_DEV_BUFFER_MEM
 *
 * parameters:     EDDS_SYS_HANDLE                 hSysDev: system handle
 *                     ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 PREFIX_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            transfer memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                               hSysDev,
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                           upper_mem_ptr
);
#endif


/*=============================================================================
 * function name:  EDDS_IOBUFFER_ALLOC_MNGM_MEM
 *
 * function:       Allocate memory from shared memory.
 *
 * parameters:     EDDS_SYS_HANDLE           system device handle
 *                 EDD_COMMON_MEM_PTR_TYPE * pointer to allocated memory
 *                 LSA_UINT32                Size
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_ALLOC_MNGM_MEM
LSA_VOID  EDDS_IOBUFFER_ALLOC_MNGM_MEM (
	EDDS_SYS_HANDLE           hSysDev,
	EDD_UPPER_MEM_PTR_TYPE *  pMem,
	LSA_UINT32                Size
);
#endif


/*=============================================================================
 * function name:  EDDS_IOBUFFER_FREE_MNGM_MEM
 *
 * function:       Free memory from shared memory.
 *
 * parameters:     EDDS_SYS_HANDLE          system device handle
 *                 EDD_COMMON_MEM_PTR_TYPE  memory to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_FREE_MNGM_MEM
LSA_VOID  EDDS_IOBUFFER_FREE_MNGM_MEM (
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       pMem
);
#endif


/*=============================================================================
 * function name:  EDDS_IOBUFFER_EXCHANGE
 *
 * function:       Atomic read of a pointer at a memory location and write
 *                 a new one.
 *
 * parameters:     EDDS_EXCHANGE_TYPE*   pointer adress
 *                 EDDS_EXCHANGE_TYPE    value to be exchanged
 *
 * return value:   The function returns the initial value pointed to by
 *                 pTarget.
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_EXCHANGE
EDDS_EXCHANGE_TYPE EDDS_IOBUFFER_EXCHANGE(
		   EDDS_EXCHANGE_TYPE volatile * pTarget,
		   EDDS_EXCHANGE_TYPE            Value
	   );
#endif

/*=============================================================================
 * function name:  EDDS_WAIT_US
 *
 * function:       Function waits one Count us
 *
 * parameters:     LSA_UINT16       Count
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_WAIT_US
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_WAIT_US(LSA_UINT16 Count);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR_LOCAL
 *
 * function:        convert local pointer to 32-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDDS_LOCAL_MEM_PTR_TYPE        Ptr
 *                  LSA_UINT32 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR_LOCAL(
    EDDS_SYS_HANDLE                     hSysDev,
    EDDS_LOCAL_MEM_PTR_TYPE             Ptr,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif


/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR32_DEV
 *
 * function:        convert upper device data pointer (e.g. descriptors)
 *                  to 32-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT32 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR32_DEV
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_DEV(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR64_DEV
 *
 * function:        convert upper device data pointer (e.g. descriptors)
 *                  to 64-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT64 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR64_DEV
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_DEV(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT64 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR32_TX
 *
 * function:        convert upper tx buffer pointer
 *                  to 32-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT32 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR32_TX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_TX(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR64_TX
 *
 * function:        convert upper tx buffer pointer
 *                  to 64-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT64 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR64_TX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_TX(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT64 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR32_RX
 *
 * function:        convert upper rx buffer pointer
 *                  to 32-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT32 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR32_RX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_RX(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:   EDDS_PTR_TO_ADDR64_RX
 *
 * function:        convert upper rx buffer pointer
 *                  to 64-Bit physical address
 *
 * parameters:      EDDS_SYS_HANDLE                hSysDev
 *                  EDD_UPPER_MEM_PTR_TYPE         Ptr
 *                  LSA_UINT64 EDDS_LOCAL_MEM_ATTR *pAddr
 *
 * return value:    LSA_VOID
 *===========================================================================*/
#ifndef EDDS_PTR_TO_ADDR64_RX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_RX(
    EDDS_SYS_HANDLE                     hSysDev,
    EDD_UPPER_MEM_PTR_TYPE              Ptr,
    LSA_UINT64 EDDS_LOCAL_MEM_ATTR *    pAddr
);
#endif

/*=============================================================================
 * function name:  EDDS_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 EDDS_FATAL_ERROR_PTR_TYPE error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_FATAL_ERROR
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FATAL_ERROR(
    LSA_UINT16                length,
    EDDS_FATAL_ERROR_PTR_TYPE error_ptr
);
#endif

/*=============================================================================
 * function name:  EDDS_RQB_ERROR
 *
 * function:       notify a RQB-error in a user/system-request.
 *                 this function is called if there is a error (i.e. param)
 *                 within RQB which prevents notification of the caller
 *                 (i.e. missing call-back-function). the error-code is set
 *                 in "Status" in RQB. this error is typically a implementation
 *                 error.
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_RQB_ERROR
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RQB_ERROR(
    EDD_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  EDDS_MEMSET_LOCAL
 *
 * function:       fills memory with value
 *
 * parameters:      EDDS_LOCAL_MEM_PTR_TYPE        pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_MEMSET_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_LOCAL(
    EDDS_LOCAL_MEM_PTR_TYPE             pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  EDDS_MEMSET_UPPER
 *
 * function:       fills memory with value
 *
 * parameters:      EDD_UPPER_MEM_PTR_TYPE         pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_MEMSET_UPPER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_UPPER(
    EDD_UPPER_MEM_PTR_TYPE              pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  EDDS_MEMCOPY_UPPER
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      EDD_UPPER_MEM_PTR_TYPE         pDst
 *                  EDD_UPPER_MEM_PTR_TYPE         pSrc
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_MEMCOPY_UPPER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMCOPY_UPPER(
    EDD_UPPER_MEM_PTR_TYPE              pDst,
    EDD_UPPER_MEM_PTR_TYPE              pSrc,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER
 *
 * function:       Does a edds_system() request with the corresponding RQB. This
 *                 function is called within edds_scheduler to trigger RQB context,
 *                 after scheduler processed general requests.
 *                 (collect and signal all pending general request)
 *
 * parameters:     EDDS_SYS_HANDLE          hSysDev
 *                 EDD_UPPER_RQB_PTR_TYPE   pRQB
 *
 * prio:           RQB context
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
    EDDS_SYS_HANDLE         hSysDev,
    EDD_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  EDDS_DO_NRT_CANCEL_PART2
 *
 * function:       Does a edds_system() request with the specified RQB. This
 *                 function is called within EDDS to start part 2 of NRT-
 *                 CANCEL-Request.
 *                 Systemadaption has to call edds_system() with the specified
 *                 RQB.
 *
 * parameters:     EDDS_SYS_HANDLE          hSysDev
 *                 EDD_UPPER_RQB_PTR_TYPE   pRQB
 *
 * prio:           RQB context
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_DO_NRT_CANCEL_PART2
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_NRT_CANCEL_PART2(
    EDDS_SYS_HANDLE         hSysDev,
    EDD_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  EDDS_ALLOC_UPPER_RQB_LOCAL
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
#ifndef EDDS_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_UPPER_RQB_LOCAL(
    EDD_UPPER_RQB_PTR_TYPE  EDDS_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif


/*=============================================================================
 * function name:  EDDS_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper-RQB allocated with EDDS_ALLOC_UPPER_RQB_LOCAL
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
#ifndef EDDS_FREE_UPPER_RQB_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  EDDS_ALLOC_UPPER_MEM_LOCAL
 *
 * function:       allocate an upper-mem for local use
 *                 the memory has to be initialized with 0
 *
 * parameters:     EDD_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length:   length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_ALLOC_UPPER_MEM_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_UPPER_MEM_LOCAL(
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                    length
);
#endif


/*=============================================================================
 * function name:  EDDS_FREE_UPPER_MEM_LOCAL
 *
 * function:       free an upper-mem allocated with EDDS_ALLOC_UPPER_MEM_LOCAL
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
#ifndef EDDS_FREE_UPPER_MEM_LOCAL
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_UPPER_MEM_LOCAL(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                           upper_mem_ptr
);
#endif


/*=============================================================================
 * function name:  EDDS_ENTER
 *
 * function:       Protect sequence from beeing interrupted
 *
 * parameters:     EDDS_SYS_HANDLE
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_ENTER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ENTER(
        EDDS_SYS_HANDLE     hSysDev
);
#endif

/*=============================================================================
 * function name:  EDDS_EXIT
 *
 * function:       Finishes EDDS_ENTER-Sequence.
 *
 * parameters:     EDDS_SYS_HANDLE
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_EXIT
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_EXIT(
        EDDS_SYS_HANDLE     hSysDev
);
#endif

/*=============================================================================
 * function name:  EDDS_NTOHS
 *
 * function:       converts a UINT16 value from network to host
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/

/* Note: will be implemented within EDDS with general handling if not defined */
/*       by systemadaption.                                                  */

/*=============================================================================
 * function name:  EDDS_NTOHL
 *
 * function:       converts a UINT32 value from network to host
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/

/* Note: will be implemented within EDDS with general handling if not defined */
/*       by systemadaption.                                                  */

/*=============================================================================
 * function name:  EDDS_HTONS
 *
 * function:       converts a UINT16 value from host to network
 *
 * parameters:     LSA_UINT16
 *
 * return value:   LSA_UINT16
 *===========================================================================*/

/* Note: will be implemented within EDDS with general handling if not defined */
/*       by systemadaption.                                                  */

/*=============================================================================
 * function name:  EDDS_HTONL
 *
 * function:       converts a UINT32 value from host to network
 *
 * parameters:     LSA_UINT32
 *
 * return value:   LSA_UINT32
 *===========================================================================*/

/* Note: will be implemented within EDDS with general handling if not defined */
/*       by systemadaption.                                                  */


/*=============================================================================
 * function name:  EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT
 *
 * function:       Called just before the consumer check is done.
 *                 timer_id is the timer_id used for this consumer check cycle.
 *                 Each EDDS-device uses its own timer for consumer check
 *
 *                 With this function the systemadaption can verify the time
 *                 elapsed since end of last consumer check cycle. if the time
 *                 is too short (back2back-situation) it can skip this check.
 *
 *                 The Systemadaption uses TimerValue to verify if enough
 *                 time is elapsed since last call. CycleTime specifies the
 *                 time the consumer check is base (a constant value from
 *                 init. eg. 3000000 for 3ms). If the time elapsed is ok, the
 *                 systemadaption actualizes TimerValue with the current Value
 *                 and sets Run to LSA_TRUE. If not enough time is elapsed,
 *                 it sets Run to LSA_FALSE and leaves TimerValue unchanged.
 *
 * parameters:     LSA_UINT16                              timer_id
 *                 LSA_UINT32                              CycleTime
 *                 LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
 *                 LSA_UINT8     EDDS_LOCAL_MEM_ATTR *     pRun
 *
 *                 timer_id    : Timer-ID of timer used for this cycle.
 *                               (only for reference, may not be needed)
 *                 pTimerValue : In : Last- TimerValue
 *                               Out: New TimerValue
 *                 CylceTime   : CycleTime in ns for consumer check
 *                 pRun        : Out: EDDS_CSRT_CYCLE_JITTER_CHECK_DO_CONS_CHECK  : Do consumer Check
 *                                    EDDS_CSRT_CYCLE_JITTER_CHECK_DONT_DO_CONS_CHECK : Dont do consumer check
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT(
    LSA_UINT16                              timer_id,
    LSA_UINT32                              CycleTime,
    LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue,
    LSA_UINT8     EDDS_LOCAL_MEM_ATTR *     pRun
);
#endif
#endif

/*=============================================================================
 * function name:  EDDS_CSRT_CONSUMER_CYCLE_START
 *
 * function:       Called after the consumer timer is started.
 *                 The timer_id was allocated previously for this consumer
 *                 check cycle and can be used to identify the cycle time.
 *
 *                 This macro will be called  after the consumer timer
 *                 is started. The Systemadaption setup the TimerValue
 *                 with the first timer value.
 *
 *
 * parameters:     LSA_UINT16                              timer_id
 *                 LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_START
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_START(
    LSA_UINT16                              timer_id,
    LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
);
#endif
#endif

/*=============================================================================
 * function name:  EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END
 *
 * function:       Called after the consumer check is done.
 *                 The timer_id was allocated previously for this consumer
 *                 check cycle and can be used to identify the cycle time.
 *
 *                 This Function is only used if
 *                 EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END is defined.
 *
 *                 Systemadaption can update the TimerValue with the current
 *                 Timer. So the Systemadaption can verify the timer since
 *                 end of consumer check instead of start later on.
 *
 * parameters:     LSA_UINT16                              timer_id
 *                 LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END
#ifndef EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END(
    LSA_UINT16                              timer_id,
    LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
);
#endif
#endif
#endif

/*=============================================================================
 * function name:  EDDS_CHECK_AND_DECODE_MAUTYPE
 *
 * function:       Checks if the physics (PHYType) of MAUType is supported and
 *                 returns speed and mode of the MAUType.
 *
 *                 Note: This macro maybe used within the LLIF! It is not part
 *                       of the EDDS Core.
 *
 * parameters:              ...         *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  unknown/unsupported
 *                                                      MAUType
 *                 EDDS_SYS_HANDLE           hSysDev       : system handle           (in)
 *                 EDDS_HANDLE               hDDB          : Handle of Device        (in)
 *                 LSA_UINT32                PortID        : PortID of Port (1..x)   (in)
 *                 LSA_UINT16                MAUType       : MAUType (see IEC)       (in)
 *
 *                 EDDS_LOCAL_MEM_U8_PTR_TYPE pSpeedMode: pointer to SpeedMode       (out)
 *
 *                                                 EDD_LINK_100MBIT_HALF
 *                                                 EDD_LINK_100MBIT_FULL
 *                                                 EDD_LINK_10MBIT_HALF
 *                                                 EDD_LINK_10MBIT_FULL
 *                                                 EDD_LINK_1GBIT_HALF
 *                                                 EDD_LINK_1GBIT_FULL
 *                                                 EDD_LINK_10GBIT_HALF
 *                                                 EDD_LINK_10GBIT_FULL
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_CHECK_AND_DECODE_MAUTYPE
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CHECK_AND_DECODE_MAUTYPE(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDDS_SYS_HANDLE                                  hSysDev,
    EDDS_HANDLE                                      hDDB,
    LSA_UINT32                                       PortID,
    LSA_UINT16                                       MAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE                       pSpeedMode
);
#endif

/*=============================================================================
 * function name:  EDDS_GET_PHY_PARAMS
 *
 * function:       Get PHY Parameters (MAUType,MediaType etc) for a given
 *                 speed and mode. If the Link is down, speed and mode will be
 *                 EDD_LINK_UNKNOWN and MAUType will be returned as EDD_MAUTYPE_UNKNOWN.
 *
 *                 Note: This macro maybe used within the LLIF! It is not part
 *                       of the EDDS Core. If used it will typically be used on
 *                       linkchange.
 *                 Note: The speed/modes/capabilites etc. supported depends on
 *                       PHY Hardware!
 *
 * parameters:
 *                 EDDS_SYS_HANDLE                 hSysDev       : system handle              (in)
 *                 EDDS_HANDLE                     hDDB          : Handle of Device           (in)
 *
 *                 LSA_UINT32                      PortID        : PortID of Port (1..x)      (in)
 *
 *                 LSA_UINT8                       Speed         : Speed of Port              (in)
 *                                                 EDD_LINK_UNKNOWN
 *                                                 EDD_LINK_SPEED_10
 *                                                 EDD_LINK_SPEED_100
 *                                                 EDD_LINK_SPEED_1000
 *                                                 EDD_LINK_SPEED_10000
 *
 *                 LSA_UINT8                       Mode          : Mode of Port               (in)
 *                                                 EDD_LINK_UNKNOWN
 *                                                 EDD_LINK_MODE_HALF
 *                                                 EDD_LINK_MODE_FULL
 *
 *                 EDDS_LOCAL_MEM_U16_PTR_TYPE     pMAUType       : MAUType (see IEC)         (out)
 *                                                 EDD_MAUTYPE_UNKNOWN (=0): if unknown
 *
 *                 EDDS_LOCAL_MEM_U8_PTR_TYPE      pMediaType    : MediaType (see IEC)        (out)
 *                                                 EDD_MEDIATYPE_UNKNOWN (=0): if unknown
 *                                                 EDD_MEDIATYPE_COPPER_CABLE
 *                                                 EDD_MEDIATYPE_FIBER_OPTIC_CABLE
 *                                                 EDD_MEDIATYPE_RADIO_COMMUNICATION
 *
 *                 EDDS_LOCAL_MEM_U8_PTR_TYPE      pIsPOF        : IsPOF
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef EDDS_GET_PHY_PARAMS
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PHY_PARAMS(
    EDDS_SYS_HANDLE                     hSysDev,
    EDDS_HANDLE                         hDDB,
    LSA_UINT32                          PortID,
    LSA_UINT8                           Speed,
    LSA_UINT8                           Mode,
    EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pIsPOF
);
#endif

#ifdef EDDS_CPU_CACHE_LINE_SIZE

/** Synchronization of CPU/system caches and external memory.
  * @note For performance reasons, these functions should be implemented as a MACRO.
  * @{
  */

#ifndef EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE
/** CPU has modified descriptor memory and external device wants to read it.
  * This function should write back all cache lines that hold the addressed memory (no need to invalidate them).
 */
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(
         EDDS_SYS_HANDLE               hSysDev,
         LSA_VOID_PTR_TYPE             addr,
         LSA_UINT                      size
         );
#endif

#ifndef EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE
 /** External device modified descriptor memory, CPU wants to read it.
  *  This function should invalidate all cache lines (no write-back!) that hold the addressed memory.
  *  The CPU will reload them from external memory (initiated by an read access).
  */
 LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE(
         EDDS_SYS_HANDLE               hSysDev,
         LSA_VOID_PTR_TYPE             addr,
         LSA_UINT                      size
         );
#endif

#ifndef EDDS_TX_MEM_DMA_SYNC_TO_DEVICE
/** CPU has modified tx transfer memory and external device wants to read it.
  * This function should write back all cache lines that hold the addressed memory (no need to invalidate them).
 */
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_TX_MEM_DMA_SYNC_TO_DEVICE(
         EDDS_SYS_HANDLE               hSysDev,
         LSA_VOID_PTR_TYPE             addr,
         LSA_UINT                      size
         );
#endif

#ifndef EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE
 /** External device modified rx transfer memory, CPU wants to read it.
  *  This function should invalidate all cache lines (no write-back!) that hold the addressed memory.
  *  The CPU will reload them from external memory (initiated by an read access).
  */
 LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE(
         EDDS_SYS_HANDLE               hSysDev,
         LSA_VOID_PTR_TYPE             addr,
         LSA_UINT                      size
         );
#endif

#else	/* EDDS_CACHE_LINE_SIZE */
#undef EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE
#undef EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE
#undef EDDS_TX_MEM_DMA_SYNC_TO_DEVICE
#undef EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE
#define EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(hSysDev,addr,size)
#define EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE(hSysDev,addr,size)
#define EDDS_TX_MEM_DMA_SYNC_TO_DEVICE(hSysDev,addr,size)
#define EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE(hSysDev,addr,size)
#endif  /* EDDS_CACHE_LINE_SIZE */

/*****************************************************************************/
/*  end of file EDDS_OUT.H                                                   */
/*****************************************************************************/


/*=============================================================================
 * function name:  EDDS_NRT_SEND_HOOK
 *
 * function:       This macro is called by EDDS just before a NRT send RQB from a
 *                 EDD_SRV_NRT_SEND (not EDD_SRV_NRT_SEND_TIMESTAMP or EDD_SRV_SYNC_SEND!)
 *                 request is queued for sending (i.e. transferred to LLIF). The macro gets the
 *                 RQB and can modify the framebuffer content referenced by pPuffer within the
 *                 RQB. All parameters within the RQB shall not be changed!
 *                 If the hook is not needed, the macro shall be defined “empty” (typical usecase)
 *
 * parameters:     EDDS_SYS_HANDLE         hSysDev
 *                 EDD_UPPER_RQB_PTR_TYPE  pRQB
 *
 * prio:           LOW
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_NRT_SEND_HOOK
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_NRT_SEND_HOOK (
    EDDS_SYS_HANDLE         hSysDev,
    EDD_UPPER_RQB_PTR_TYPE  pRQB);
#endif

/*=============================================================================
 * function name:  EDDS_GET_SYSTEM_TIME_NS
 *
 * function:
 *
 * parameters:     EDDS_SYS_HANDLE hSysDev
 *
 * prio:           xxx
 *
 * return value:   LSA_UINT64
 *===========================================================================*/
#ifndef EDDS_GET_SYSTEM_TIME_NS
LSA_UINT64 EDDS_GET_SYSTEM_TIME_NS(
        EDDS_SYS_HANDLE hSysDev
);
#endif

/*=============================================================================
 * function name:  EDDS_NOTIFY_FILL_SETTINGS
 *
 * function:       Notify system adaption for settings of Feed-In
 *                 Load Limitation (FILL).
 *
 *                 FeedInLoadLimitationActive : 0: INACTIVE, 1: ACTIVE
 *                 IO_Configured              : 0: NO, 1: YES
 *
 * parameters:     EDDS_SYS_HANDLE hSysDev
 *                 LSA_UINT8       FeedInLoadLimitationActive
 *                 LSA_UINT8       IO_Configured
 *
 * prio:           RQB context
 *
 * return value:   EDD_RSP      EDD_STS_OK_PENDING - The scheduler will be triggered 
 *                              with the macro EDDS_DO_NOTIFY_SCHEDULER in case of
 *                              an NRT-Snd event. NRT-Send/Rcv RQBs will be finished
 *                              in RQB context.
 *
 *                              EDD_STS_OK - The scheduler will be called immediately 
 *                              from the RQB context in case of an NRT-Snd event. 
 *                              NRT-Send/Rcv RQBs will be finished in Scheduler context.
 *
 *===========================================================================*/
#ifndef EDDS_NOTIFY_FILL_SETTINGS
EDD_RSP EDDS_NOTIFY_FILL_SETTINGS(
        EDDS_SYS_HANDLE hSysDev,
        LSA_UINT8 FeedInLoadLimitationActive,
        LSA_UINT8 IO_Configured
);
#endif


/*=============================================================================
 * function name:  EDDS_DO_NOTIFY_SCHEDULER
 *
 * function:       This macro indicates system adaption to call the edds_scheduler.
 *                 Currently EDDS calls this macro when edds_scheduler should process
 *                 one or more Requests.
 *                 System adaption may trigger edds_scheduler (intermediateCall = LSA_TRUE)
 *                 by using an indirection (e.g. Message to edds_scheduler thread).
 *
 *                 Attention:
 *                 It is not allowed to call edds_scheduler() directly in the output macro!
 *
 * parameters:     EDDS_SYS_HANDLE         hSysDev
 *
 * prio:           LOW
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_DO_NOTIFY_SCHEDULER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_NOTIFY_SCHEDULER (
    EDDS_SYS_HANDLE         hSysDev);
#endif


/*=============================================================================
 * function name:  EDDS_DO_RETRIGGER_SCHEDULER
 *
 * function:       This macro indicates system adaption to call the edds_scheduler again.
 *                 Currently EDDS Scheduler calls this macro when some high priority events
 *                 should be finished in another edds_scheduler call.
 *                 System adaption should call the edds_scheduler (intermediateCall = LSA_TRUE)
 *                 by using an indirection (e.g. Message to edds_scheduler thread).
 *
 *                 Attention:
 *                 It is not allowed to call edds_scheduler() directly in the output macro!
 *
 * parameters:     EDDS_SYS_HANDLE         hSysDev
 *
 * prio:           LOW
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_DO_RETRIGGER_SCHEDULER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_RETRIGGER_SCHEDULER (
    EDDS_SYS_HANDLE         hSysDev);
#endif


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_OUT_H */

