#ifndef PSI_SYS_MEM_H_              /* ----- reinclude-protection ----- */
#define PSI_SYS_MEM_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: psi_sys_mem.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface for memory functions                                    */
/*                                                                           */
/*****************************************************************************/
/*----------------------------------------------------------------------------*/
/* local memory                                                               */
/*----------------------------------------------------------------------------*/

/* Memory Types for malloc statistics */
#define PSI_MTYPE_IGNORE            0   // reserved for all user allocs with no statistics
#define PSI_MTYPE_LOCAL_MEM         1
#define PSI_MTYPE_UPPER_RQB         2
#define PSI_MTYPE_UPPER_MEM         3
#define PSI_MTYPE_NRT_MEM           4   // NRT send/receive buffer
#define PSI_MTYPE_PI_MEM            5   // Process image memory (KRAM, IOCC, ...)
#define PSI_MTYPE_HIF_MEM           6

#define PSI_MTYPE_LOWER_RQB_ACP     7
#define PSI_MTYPE_LOWER_MEM_ACP     8
#define PSI_MTYPE_LOWER_RQB_CLRPC   9
#define PSI_MTYPE_LOWER_MEM_CLRPC   10
#define PSI_MTYPE_LOWER_RQB_DCP     11
#define PSI_MTYPE_LOWER_MEM_DCP     12
#define PSI_MTYPE_LOWER_RQB_EDD     13  // use same index for all lower EDDx
#define PSI_MTYPE_LOWER_MEM_EDD     14  // use same index for all lower EDDx
#define PSI_MTYPE_LOWER_RQB_GSY     15
#define PSI_MTYPE_LOWER_MEM_GSY     16
#define PSI_MTYPE_LOWER_RQB_LLDP    17
#define PSI_MTYPE_LOWER_MEM_LLDP    18
#define PSI_MTYPE_LOWER_RQB_MRP     19
#define PSI_MTYPE_LOWER_MEM_MRP     20
#define PSI_MTYPE_LOWER_RQB_NARE    21
#define PSI_MTYPE_LOWER_MEM_NARE    22
#define PSI_MTYPE_LOWER_RQB_OHA     23
#define PSI_MTYPE_LOWER_MEM_OHA     24
#define PSI_MTYPE_LOWER_RQB_DNS     25
#define PSI_MTYPE_LOWER_MEM_DNS     26
#define PSI_MTYPE_LOWER_RQB_POF     27
#define PSI_MTYPE_LOWER_MEM_POF     28
#define PSI_MTYPE_LOWER_RQB_SNMPX   29
#define PSI_MTYPE_LOWER_MEM_SNMPX   30
#define PSI_MTYPE_LOWER_RQB_SOCK    31
#define PSI_MTYPE_LOWER_MEM_SOCK    32
#define PSI_MTYPE_LOWER_RQB_IOH     33
#define PSI_MTYPE_LOWER_MEM_IOH     34
#define PSI_MTYPE_LOWER_RQB_PNTRC   35
#define PSI_MTYPE_LOWER_MEM_PNTRC   36
#define PSI_MTYPE_LOWER_RQB_HIF     37
#define PSI_MTYPE_LOWER_MEM_HIF     38
#define PSI_MTYPE_LOWER_RQB_HSA     39
#define PSI_MTYPE_LOWER_MEM_HSA     40
#define PSI_MTYPE_LOWER_RQB_PSI     41
#define PSI_MTYPE_LOWER_MEM_PSI     42
#define PSI_MTYPE_CRT_SLOW_MEM		43

#define PSI_MTYPE_HIF_RQB_MEM       44
#define PSI_MTYPE_HIF_NO_RQB_MEM    45

#define PSI_MTYPE_MAX               (PSI_MTYPE_HIF_NO_RQB_MEM + 1)

#ifndef PSI_ALLOC_LOCAL_MEM
LSA_VOID PSI_ALLOC_LOCAL_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          comp_id,
	LSA_UINT16          mem_type );
#endif

#ifndef PSI_FREE_LOCAL_MEM
LSA_VOID PSI_FREE_LOCAL_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          comp_id,
	LSA_UINT16          mem_type );
#endif

/*----------------------------------------------------------------------------*/
/* DEV memory pool (for one HD instance)                                      */
/*----------------------------------------------------------------------------*/

#ifndef PSI_CREATE_DEV_MEM_POOL
LSA_VOID PSI_CREATE_DEV_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT           * pool_handle_ptr );
#endif

#ifndef PSI_DELETE_DEV_MEM_POOL
LSA_VOID PSI_DELETE_DEV_MEM_POOL(
	LSA_UINT16        * ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT             pool_handle );
#endif

#ifndef PSI_DEV_ALLOC_MEM
LSA_VOID PSI_DEV_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_DEV_FREE_MEM
LSA_VOID PSI_DEV_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

/*----------------------------------------------------------------------------*/
/* NRT memory pool (for one HD instance)                                      */
/*----------------------------------------------------------------------------*/

// NRT memory pool types available. Only used for create/delete.
// no enum to ease PSI function definitions.
#define PSI_HD_NRT_TX_MEM                   0   // NRT TX memory (SharedMem, SDRam)
#define PSI_HD_NRT_RX_MEM                   1   // NRT RX memory (SharedMem, SDRam)
#define PSI_HD_NRT_EDDI_DMACW_DEFAULT       2
#define PSI_HD_NRT_EDDI_TX_LOW_FRAG         3
#define PSI_HD_NRT_EDDI_TX_MID_FRAG         4
#define PSI_HD_NRT_EDDI_TX_HIGH_FRAG        5
#define PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG     6
#define PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG    7
#define PSI_HD_NRT_MAX_MEMPOOLS             8

#ifndef PSI_CREATE_NRT_MEM_POOL
LSA_VOID PSI_CREATE_NRT_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT *           pool_handle_ptr,
	LSA_UINT8           cp_mem_nrt_type );
#endif

#ifndef PSI_DELETE_NRT_MEM_POOL
LSA_VOID PSI_DELETE_NRT_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT             pool_handle,
	LSA_UINT8           cp_mem_nrt_type );
#endif

#ifndef PSI_NRT_ALLOC_TX_MEM
LSA_VOID PSI_NRT_ALLOC_TX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_FREE_TX_MEM
LSA_VOID PSI_NRT_FREE_TX_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_ALLOC_RX_MEM
LSA_VOID PSI_NRT_ALLOC_RX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_FREE_RX_MEM
LSA_VOID PSI_NRT_FREE_RX_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

/*----------------------------------------------------------------------------*/
/* PI memory pool                                                             */
/*----------------------------------------------------------------------------*/

#ifndef PSI_CREATE_PI_MEM_POOL
LSA_VOID PSI_CREATE_PI_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT *           pool_handle_ptr );
#endif

#ifndef PSI_DELETE_PI_MEM_POOL
LSA_VOID PSI_DELETE_PI_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT             pool_handle );
#endif

#ifndef PSI_PI_ALLOC_MEM
LSA_VOID PSI_PI_ALLOC_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_BOOL            is_provider );
#endif

#ifndef PSI_PI_FREE_MEM
LSA_VOID PSI_PI_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle);
#endif

/*----------------------------------------------------------------------------*/
/* TCIP memory pool                                                           */
/*----------------------------------------------------------------------------*/

#if (PSI_CFG_USE_TCIP == 1)

#ifndef PSI_CREATE_TCIP_MEM_POOL
LSA_VOID PSI_CREATE_TCIP_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length );
#endif

#ifndef PSI_DELETE_TCIP_MEM_POOL
LSA_VOID PSI_DELETE_TCIP_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr);
#endif

#ifndef PSI_TCIP_ALLOC_MEM
LSA_VOID PSI_TCIP_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_TCIP_FREE_MEM
LSA_VOID PSI_TCIP_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr );
#endif

#endif //PSI_CFG_USE_TCIP

/*----------------------------------------------------------------------------*/
/* OpenBSD page-memory pool (AP01559183)                                      */
/*----------------------------------------------------------------------------*/

#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)

#ifndef PSI_CREATE_OBSD_MEM_POOL
LSA_VOID PSI_CREATE_OBSD_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length );
#endif

#ifndef PSI_DELETE_OBSD_MEM_POOL
LSA_VOID PSI_DELETE_OBSD_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr );
#endif

#ifndef PSI_OBSD_ALLOC_MEM
LSA_VOID PSI_OBSD_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_OBSD_FREE_MEM
LSA_VOID PSI_OBSD_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr );
#endif

#endif // PSI_CFG_TCIP_STACK_OPEN_BSD


/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PSI_SYS_H_ */

