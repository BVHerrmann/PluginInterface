#ifndef TCIP_LOW_H
#define TCIP_LOW_H

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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_low.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Lower-Layer interface                                                    */
/*                                                                           */
/*****************************************************************************/

#define TCIP_EDD_GET_PPARAM(edd_rqb_ptr, edd_type)	((edd_type)EDD_RQB_GET_PPARAM(edd_rqb_ptr))


/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

typedef TCIP_EDD_LOWER_RQB_TYPE * TCIP_EDD_LOWER_RQB_PTR_TYPE; /* pointer to lower-RQB */

typedef TCIP_EDD_LOWER_RQB_TYPE const * TCIP_EDD_LOWER_RQB_CONST_PTR_TYPE; /* pointer to lower-RQB */

typedef EDD_RQB_NRT_SET_ARP_FILTER_TYPE * TCIP_EDD_RQB_SET_ARP_FILTER_PTR_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/* check availability of send rqb on EDD ARP channel, see 1824771 */
int tcip_is_send_resource_available(const void* ifp);

/*=============================================================================
 * function name:  tcip_edd_request_lower_done
 *
 * function:       lower done callback-function
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID
tcip_edd_request_lower_done(
	TCIP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  TCIP_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_OPEN_CHANNEL_LOWER
LSA_VOID
TCIP_EDD_OPEN_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE			 sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID
TCIP_EDD_CLOSE_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE		lower_rqb_ptr,
	LSA_SYS_PTR_TYPE				sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_REQUEST_LOWER
LSA_VOID
TCIP_EDD_REQUEST_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_ALLOC_LOWER_RQB
LSA_VOID
TCIP_EDD_ALLOC_LOWER_RQB(
	TCIP_EDD_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE              user_id,
	LSA_UINT16                    length,
	LSA_SYS_PTR_TYPE              sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_FREE_LOWER_RQB
 *
 * function:       free a lower-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_FREE_LOWER_RQB
LSA_VOID
TCIP_EDD_FREE_LOWER_RQB(
	LSA_UINT16                  * ret_val_ptr,
	TCIP_EDD_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
	LSA_SYS_PTR_TYPE			  sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_ALLOC_LOWER_MEM
LSA_VOID
TCIP_EDD_ALLOC_LOWER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE			  user_id,
	LSA_UINT16					  length,
	LSA_SYS_PTR_TYPE			  sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_FREE_LOWER_MEM
 *
 * function:       free a lower-memory
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#ifndef TCIP_EDD_FREE_LOWER_MEM
LSA_VOID
TCIP_EDD_FREE_LOWER_MEM(
	LSA_UINT16                 	* ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE	  lower_mem_ptr,
	LSA_SYS_PTR_TYPE			  sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM
 *
 * function:       allocate frame buffer from receive buffer pool
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID
TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT16                    length,
	LSA_SYS_PTR_TYPE              sys_ptr

);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM
 *
 * function:       return frame buffer to receive buffer pool
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID
TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM(
	LSA_UINT16                   * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE    lower_mem_ptr,
	LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM
 *
 * function:       allocate frame buffer from send buffer pool
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID
TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE  * lower_mem_ptr_ptr,
	LSA_UINT16                     length,
	LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*=============================================================================
 * function name:  TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM
 *
 * function:       return frame buffer to send buffer pool
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID
TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM(
	LSA_UINT16                   * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE    lower_mem_ptr,
	LSA_SYS_PTR_TYPE               sys_ptr
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_LOW_H */
