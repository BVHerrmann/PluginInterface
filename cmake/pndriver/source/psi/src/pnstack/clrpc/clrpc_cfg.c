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
/*  F i l e               &F: clrpc_cfg.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CLRPC using PSI.                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1100
#define PSI_MODULE_ID       1100 /* PSI_MODULE_ID_CLRPC_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

void clrpc_psi_startstop( int start )
{
	if ( start )
	{
		static CLRPC_INIT_TYPE clrpc_init_param; // not defined by LSA if necessarily static. static is safe.
		LSA_UINT16             rc;

		clrpc_init_param.oneshot_lsa_time_base   = LSA_TIME_BASE_100MS;
		clrpc_init_param.oneshot_lsa_time_factor = 1;

		rc = clrpc_init( &clrpc_init_param );
		PSI_ASSERT( rc == LSA_RET_OK );
	}
	else
	{
		LSA_UINT16 rc;

		rc = clrpc_undo_init();
		PSI_ASSERT( rc == LSA_RET_OK );
	}
}

/*-----------------------------------------------------------------------------
//	System Layer (prototypes in clrpc_sys.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_GET_PATH_INFO
LSA_VOID CLRPC_GET_PATH_INFO(
	LSA_UINT16            * ret_val_ptr,
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	CLRPC_DETAIL_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       path )
{
	*ret_val_ptr = psi_clrpc_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_RELEASE_PATH_INFO
LSA_VOID CLRPC_RELEASE_PATH_INFO(
	LSA_UINT16            * ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	CLRPC_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_clrpc_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_TIMER
LSA_VOID CLRPC_ALLOC_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * timer_id_ptr,
	LSA_UINT16   timer_type,
	LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr,  timer_id_ptr,  timer_type, time_base, clrpc_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_START_TIMER
LSA_VOID CLRPC_START_TIMER(
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

#ifndef CLRPC_STOP_TIMER
LSA_VOID CLRPC_STOP_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_TIMER
LSA_VOID CLRPC_FREE_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifdef CLRPC_ENTER
#error "who defined it? not used!"
#endif

#ifdef CLRPC_EXIT
#error "who defined it? not used!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_FATAL_ERROR
LSA_VOID CLRPC_FATAL_ERROR(
	LSA_UINT16                 length,
	CLRPC_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "clrpc", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*-----------------------------------------------------------------------------
//	Upper Layer (prototypes in clrpc_usr.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_REQUEST_UPPER_DONE
LSA_VOID CLRPC_REQUEST_UPPER_DONE(
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE  clrpc_request_upper_done_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                   sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)clrpc_request_upper_done_ptr, (struct psi_header*)upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_REQUEST_LOCAL
LSA_VOID CLRPC_REQUEST_LOCAL(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, (LSA_VOID_PTR_TYPE)upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*-----------------------------------------------------------------------------
//	Lower Layer (prototypes in clrpc_low.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_OPEN_CHANNEL_LOWER
LSA_VOID CLRPC_OPEN_CHANNEL_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != 0 );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel,(LSA_VOID_PTR_TYPE) lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_CLOSE_CHANNEL_LOWER
LSA_VOID CLRPC_CLOSE_CHANNEL_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != 0 );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, (LSA_VOID_PTR_TYPE)lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_REQUEST_LOWER
LSA_VOID CLRPC_REQUEST_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != 0 );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, (LSA_VOID_PTR_TYPE)lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#endif // PSI_CFG_USE_LD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
