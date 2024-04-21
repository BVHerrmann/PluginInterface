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
/*  F i l e               &F: nare_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of NARE using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2300
#define PSI_MODULE_ID       2300 /* PSI_MODULE_ID_NARE_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_HD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_INT16  nare_enter = 0;

/*----------------------------------------------------------------------------*/

void nare_psi_startstop( int start )
{
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		rsp = nare_init();
		PSI_ASSERT( rsp == NARE_RSP_OK );
	}
	else
	{
		rsp = nare_undo_init();
		PSI_ASSERT( rsp == NARE_RSP_OK );
	}
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_GET_PATH_INFO
LSA_VOID NARE_GET_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE     * sys_ptr_ptr,
	NARE_DETAIL_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE      path )
{
	*ret_val_ptr = psi_nare_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef NARE_RELEASE_PATH_INFO
LSA_VOID NARE_RELEASE_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr,
	NARE_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_nare_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ALLOC_TIMER
LSA_VOID NARE_ALLOC_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * timer_id_ptr,
	LSA_UINT16   timer_type,
	LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, nare_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_START_TIMER
LSA_VOID NARE_START_TIMER(
	LSA_UINT16       * ret_val_ptr,
	LSA_UINT16         timer_id,
	LSA_USER_ID_TYPE   user_id,
	LSA_UINT16         time )
{
	PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_STOP_TIMER
LSA_VOID NARE_STOP_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FREE_TIMER
LSA_VOID NARE_FREE_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FATAL_ERROR
LSA_VOID NARE_FATAL_ERROR(
	LSA_UINT16                length,
	NARE_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "nare", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_RQB_ERROR
LSA_VOID NARE_RQB_ERROR(
	NARE_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_NARE, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ENTER
LSA_VOID NARE_ENTER( LSA_VOID )
{
	if ( nare_enter != 0 )
	{
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, PSI_MODULE_ID, 0 );
	}
	nare_enter += 1;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EXIT
LSA_VOID NARE_EXIT( LSA_VOID )
{
	nare_enter -= 1;

	if ( nare_enter != 0 )
	{
	    PSI_FATAL_COMP( LSA_COMP_ID_NARE, PSI_MODULE_ID, 0 );
	}
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

LSA_VOID NARE_DCP_UPPER_RQB_TO_LOWER_MEM(
	NARE_DCP_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	NARE_UPPER_RQB_MEM_PTR_TYPE   upper_rqb_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	*lower_mem_ptr_ptr = upper_rqb_mem_ptr;
}

/*----------------------------------------------------------------------------*/

LSA_VOID NARE_DCP_UPPER_TO_LOWER_MEM(
	NARE_DCP_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	NARE_UPPER_MEM_PTR_TYPE       upper_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	*lower_mem_ptr_ptr = upper_mem_ptr;
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_MEMSET_LOCAL
LSA_VOID NARE_MEMSET_LOCAL(
	NARE_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_MEMCOPY_COMMON
LSA_VOID NARE_MEMCOPY_COMMON(
	NARE_COMMON_MEM_PTR_TYPE pDst,
	NARE_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32               Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_REQUEST_UPPER_DONE
LSA_VOID NARE_REQUEST_UPPER_DONE(
	NARE_UPPER_CALLBACK_FCT_PTR_TYPE nare_request_upper_done_ptr,
	NARE_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)nare_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DO_TIMER_REQUEST
LSA_VOID NARE_DO_TIMER_REQUEST(
	NARE_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

LSA_VOID NARE_ARP_RESPONSE_FRAME_CONTROL(
	LSA_UINT16 NICId,
	LSA_BOOL   On )
{
	/* InterfaceID is uses as NICId [1..16], where value=0 is not allowed (is the global IF) */
	PSI_ASSERT( ( NICId > PSI_GLOBAL_PNIO_IF_NR ) && ( NICId <= PSI_MAX_PNIO_IF_NR ) );

	psi_hd_set_arp_on_state( NICId, On );
}

/*----------------------------------------------------------------------------*/

LSA_VOID NARE_ARP_RESPONSE_FRAME_OUT(
	LSA_UINT16                  NICId,
	NARE_EDD_LOWER_MEM_PTR_TYPE pFrame,
	LSA_UINT16                  FrameLen )
{
	/* no need to make a copy of the frame at sniffing */
	/* so NARE_ARP_RESPONSE_FRAME_OUT() is empty */
	LSA_UNUSED_ARG( NICId );
	LSA_UNUSED_ARG( pFrame );
	LSA_UNUSED_ARG( FrameLen );
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_OPEN_CHANNEL_LOWER
LSA_VOID NARE_DCP_OPEN_CHANNEL_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID NARE_DCP_CLOSE_CHANNEL_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_REQUEST_LOWER
LSA_VOID NARE_DCP_REQUEST_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"NARE_DCP_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		NARE_DCP_RQB_GET_HANDLE( lower_rqb_ptr ),
		NARE_DCP_RQB_GET_OPCODE( lower_rqb_ptr ) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_OPEN_CHANNEL_LOWER
LSA_VOID NARE_EDD_OPEN_CHANNEL_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_open_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID NARE_EDD_CLOSE_CHANNEL_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_close_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_REQUEST_LOWER
LSA_VOID NARE_EDD_REQUEST_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

    PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"NARE_EDD_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u) service(%#x)",
		lower_rqb_ptr,
		NARE_EDD_RQB_GET_HANDLE( lower_rqb_ptr ),
		NARE_EDD_RQB_GET_OPCODE( lower_rqb_ptr ),
		NARE_EDD_RQB_GET_SERVICE( lower_rqb_ptr ) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and request function for EDDx
	mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT)psi_edd_get_request_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_ALLOC_LOWER_TXMEM
LSA_VOID NARE_EDD_ALLOC_LOWER_TXMEM(
	NARE_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE              user_id,
	LSA_UINT16                    length,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
	LSA_UNUSED_ARG(user_id);

	psi_edd_alloc_nrt_tx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_NARE );

    PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_ALLOC_LOWER_TXMEM(): ptr(0x%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_EDD_FREE_LOWER_TXMEM
LSA_VOID NARE_EDD_FREE_LOWER_TXMEM(
	LSA_UINT16                  * ret_val_ptr,
	NARE_EDD_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

    PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_FREE_LOWER_TXMEM(): ptr(0x%08x)", lower_mem_ptr );

	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_NARE );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#endif // PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
