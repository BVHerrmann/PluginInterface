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
/*  F i l e               &F: gsy_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of GSY using PSI.                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1800
#define PSI_MODULE_ID       1800 /* PSI_MODULE_ID_GSY_CFG */

#include "psi_int.h"

#if ((PSI_CFG_USE_GSY == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/

void gsy_psi_startstop( int start )
{
	LSA_UINT16 rc;

	if ( start ) 
	{
		rc = gsy_init();
		PSI_ASSERT( rc == LSA_RET_OK );
	}
	else 
	{
		rc = gsy_undo_init();
		PSI_ASSERT( rc == LSA_RET_OK );
	}
}

/*----------------------------------------------------------------------------*/

#ifndef GSY_GET_PATH_INFO
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_PATH_INFO(
	LSA_UINT16           GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE     GSY_LOCAL_MEM_ATTR *  sys_ptr_ptr,
	GSY_DETAIL_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_gsy_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef GSY_RELEASE_PATH_INFO
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_RELEASE_PATH_INFO(
	LSA_UINT16           GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE                           sys_ptr,
	GSY_DETAIL_PTR_TYPE                        detail_ptr )
{
	*ret_val_ptr = psi_gsy_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_ALLOC_TIMER
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_TIMER(
	LSA_UINT16 GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 GSY_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                      timer_type,
	LSA_UINT16                      time_base )
{
    if ((time_base == LSA_TIME_BASE_1MS) || (time_base == LSA_TIME_BASE_10MS))
    {
        PSI_ALLOC_TIMER_TGROUP1( ret_val_ptr, timer_id_ptr, timer_type, time_base, gsy_timeout );
    }
    else
    {
        PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, gsy_timeout );
    }
}
#else
#error "by design a function!"
#endif

#ifndef GSY_START_TIMER
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_START_TIMER(
	LSA_UINT16       GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                            timer_id,
	LSA_USER_ID_TYPE                      user_id,
	LSA_UINT16                            time )
{
    PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time  );
}
#else
#error "by design a function!"
#endif

#ifndef GSY_STOP_TIMER
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_STOP_TIMER(
	LSA_UINT16 GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                      timer_id )
{
    PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef GSY_FREE_TIMER
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_FREE_TIMER(
	LSA_UINT16 GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                      timer_id )
{
    PSI_FREE_TIMER( ret_val_ptr, timer_id  );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_FATAL_ERROR
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_FATAL_ERROR(
	LSA_UINT16                length,
	GSY_FATAL_ERROR_PTR_TYPE  error_ptr )
{
	PSI_FATAL_ERROR( "gsy", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_ENTER
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_ENTER( LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

#ifndef GSY_EXIT
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_EXIT( LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_RQB_ERROR
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_RQB_ERROR(
	GSY_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_GSY, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	Upper Layer
//----------------------------------------------------------------------------*/

#ifndef GSY_REQUEST_UPPER_DONE
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_REQUEST_UPPER_DONE(
	GSY_UPPER_CALLBACK_FCT_PTR_TYPE  gsy_request_upper_done_ptr,
	GSY_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)gsy_request_upper_done_ptr, upper_rqb_ptr, sys_ptr);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_DO_TIMEOUT_EXTERN
#error "cannot do that because mails may be posted to the MM-timer thread"
#endif

#ifndef GSY_DO_TIMER_REQUEST
LSA_VOID GSY_SYSTEM_OUT_FCT_ATTR GSY_DO_TIMER_REQUEST(
	GSY_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef GSY_COPY_COMMON_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_COPY_COMMON_MEM(
	LSA_VOID  GSY_COMMON_MEM_ATTR   *src_common_mem_ptr,
	LSA_VOID  GSY_COMMON_MEM_ATTR   *dst_common_mem_ptr,
	LSA_UINT16                      length )
{
	PSI_MEMCPY( dst_common_mem_ptr, src_common_mem_ptr, length );
    //lint --e(818) Pointer parameter 'src_common_mem_ptr' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef GSY_COPY_UPPER_TO_LOCAL_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR	GSY_COPY_UPPER_TO_LOCAL_MEM(
	GSY_UPPER_MEM_PTR_TYPE     src,
	GSY_LOCAL_MEM_PTR_TYPE     dst,
	LSA_UINT16                 len,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	PSI_MEMCPY( dst, src, len );
    //lint --e(818) Pointer parameter 'src' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef GSY_MEMSET_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_MEMSET_LOCAL(
	GSY_LOCAL_MEM_PTR_TYPE              pMem,
	LSA_UINT8                           Value,
	LSA_UINT32                          Length )
{
	PSI_MEMSET (pMem, Value, Length);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_OPEN_CHANNEL_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_OPEN_CHANNEL_LOWER(
	GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	LSA_UINT16             mbx_id;
	PSI_REQUEST_FCT        p_func;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_GSY, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_open_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_CLOSE_CHANNEL_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_CLOSE_CHANNEL_LOWER(
	GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	LSA_UINT16             mbx_id;
	PSI_REQUEST_FCT        p_func;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_GSY, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_close_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_REQUEST_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_REQUEST_LOWER(
	GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	LSA_UINT16             mbx_id;
	PSI_REQUEST_FCT        p_func;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"GSY_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u) service(%u)",
		lower_rqb_ptr,
		GSY_LOWER_RQB_HANDLE_GET( lower_rqb_ptr ),
		GSY_LOWER_RQB_OPCODE_GET( lower_rqb_ptr ),
		GSY_LOWER_RQB_SERVICE_GET( lower_rqb_ptr ) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and request function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_GSY, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_request_fct( pSys->comp_id_lower );

	if (mbx_id == PSI_MBX_ID_EDDI_NRT_ORG)
	{
		psi_request_direct_start( mbx_id, p_func, lower_rqb_ptr );
	}
	else
	{
		psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_ALLOC_LOWER_TXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_TXMEM(
	GSY_LOWER_TXMEM_PTR_TYPE GSY_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	LSA_UNUSED_ARG(user_id);

	psi_edd_alloc_nrt_tx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_GSY );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "GSY_ALLOC_LOWER_TXMEM(): ptr(0x%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef GSY_FREE_LOWER_TXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_TXMEM(
	LSA_UINT16               GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	GSY_LOWER_TXMEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "GSY_FREE_LOWER_TXMEM(): ptr(0x%08x)", lower_mem_ptr );

	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_GSY );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_ALLOC_LOWER_RXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_RXMEM(
	GSY_LOWER_RXMEM_PTR_TYPE GSY_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

    if (length != EDD_FRAME_BUFFER_LENGTH)
    {
		PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_ALLOC_LOWER_RXMEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *lower_mem_ptr_ptr = LSA_NULL;
	    LSA_UNUSED_ARG( user_id );
        PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    }

	psi_edd_alloc_nrt_rx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_GSY );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "GSY_ALLOC_LOWER_RXMEM(): ptr(0x%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef GSY_FREE_LOWER_RXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_RXMEM(
	LSA_UINT16               GSY_LOCAL_MEM_ATTR * ret_val_ptr,
	GSY_LOWER_RXMEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	PSI_SYSTEM_TRACE_01( pSys->trace_idx,LSA_TRACE_LEVEL_CHAT, "GSY_FREE_LOWER_RXMEM(): ptr(0x%08x)", lower_mem_ptr );

	psi_edd_free_nrt_rx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_GSY );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_MEMSET_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_MEMSET_UPPER(
	GSY_UPPER_MEM_PTR_TYPE  pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef GSY_COPY_LOCAL_TO_UPPER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_COPY_LOCAL_TO_UPPER_MEM(
	GSY_LOCAL_MEM_PTR_TYPE  src_ptr,
	GSY_UPPER_MEM_PTR_TYPE  dst_ptr,
	LSA_UINT16              length,
	LSA_SYS_PTR_TYPE	    sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	PSI_MEMCPY( dst_ptr, src_ptr, length );
    //lint --e(818) Pointer parameter 'src_ptr' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_GSY && PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
