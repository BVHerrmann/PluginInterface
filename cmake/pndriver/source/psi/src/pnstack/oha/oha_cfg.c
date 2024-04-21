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
/*  F i l e               &F: oha_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of OHA using PSI.                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2400
#define PSI_MODULE_ID       2400 /* PSI_MODULE_ID_OHA_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_UINT16 oha_enter_exit_id = PSI_LOCK_ID_INVALID;
static LSA_UINT64 oha_sys_uptime_startup_10ms = 0; /* startup time of oha (used for e.g. snmpx) */

/*----------------------------------------------------------------------------*/

void oha_psi_startstop( int start )
{
	LSA_UINT16        rc;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &oha_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		oha_sys_uptime_startup_10ms = PSI_GET_TICKS_100NS() / 100000; /*Calculate from 100 ns to 10 milliseconds ticks, don't lose percision here -> divide with UIN64 type! */

		rc = oha_init();
		PSI_ASSERT( rc == OHA_OK );
	}
	else
	{
		rc = oha_undo_init();
		PSI_ASSERT( rc == OHA_OK );

		PSI_FREE_REENTRANCE_LOCK( &rsp, oha_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		oha_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*----------------------------------------------------------------------------*/

#ifndef OHA_GET_PATH_INFO
LSA_VOID OHA_GET_PATH_INFO(
	LSA_UINT16              *  ret_val_ptr,
	LSA_SYS_PTR_TYPE        *  sys_ptr_ptr,
	OHA_DETAIL_PTR_TYPE     *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE          path )
{
	*ret_val_ptr = psi_oha_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_RELEASE_PATH_INFO
LSA_VOID OHA_RELEASE_PATH_INFO(
	LSA_UINT16          * ret_val_ptr,
	LSA_SYS_PTR_TYPE      sys_ptr,
	OHA_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_oha_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ALLOC_TIMER
LSA_VOID OHA_ALLOC_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * timer_id_ptr,
	LSA_UINT16   timer_type,
	LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, oha_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_START_TIMER
LSA_VOID OHA_START_TIMER(
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

#ifndef OHA_STOP_TIMER
LSA_VOID OHA_STOP_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_FREE_TIMER
LSA_VOID OHA_FREE_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_DO_TIMER_REQUEST
LSA_VOID OHA_DO_TIMER_REQUEST(
	OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_FATAL_ERROR
LSA_VOID OHA_FATAL_ERROR(
	LSA_UINT16               length,
	OHA_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "oha", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ENTER
LSA_VOID OHA_ENTER( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( oha_enter_exit_id );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EXIT
LSA_VOID OHA_EXIT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( oha_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_DCP_OPEN_CHANNEL_LOWER
LSA_VOID OHA_DCP_OPEN_CHANNEL_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower== LSA_COMP_ID_DCP );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (DCP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to DCP (without HIF DCP is located in same system)
	    psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_DCP_CLOSE_CHANNEL_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (DCP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to DCP (without HIF DCP is located in same system)
	    psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_REQUEST_LOWER
LSA_VOID OHA_DCP_REQUEST_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_DCP_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_DCP_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_DCP_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
        // post the message to HIF HD (DCP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to DCP (without HIF DCP is located in same system)
	    psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_EDD_OPEN_CHANNEL_LOWER
LSA_VOID OHA_EDD_OPEN_CHANNEL_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and open function for EDDx
	    LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_open_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_EDD_CLOSE_CHANNEL_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and close function for EDDx
	    LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_close_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_REQUEST_LOWER
LSA_VOID OHA_EDD_REQUEST_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_EDD_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u) service(%u)",
		lower_rqb_ptr,
		OHA_EDD_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_EDD_RQB_GET_OPCODE(lower_rqb_ptr),
		OHA_EDD_RQB_GET_SERVICE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and request function for EDDx
    	LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_request_fct( pSys->comp_id_lower );

    	// post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID OHA_SOCK_OPEN_CHANNEL_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_SOCK_CLOSE_CHANNEL_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_REQUEST_LOWER
LSA_VOID OHA_SOCK_REQUEST_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_SOCK_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_SOCK_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_SOCK_RQB_GET_OPCODE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef OHA_CFG_USE_DNS

#ifndef OHA_DNS_OPEN_CHANNEL_LOWER
LSA_VOID OHA_DNS_OPEN_CHANNEL_LOWER(
    OHA_DNS_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
)
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DNS );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_open_channel, lower_rqb_ptr );
}
#else
# error "by design a function"
#endif

#ifndef OHA_DNS_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_DNS_CLOSE_CHANNEL_LOWER(
    OHA_DNS_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
)
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DNS );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_close_channel, lower_rqb_ptr );
}
#else
# error "by design a function"
#endif

#ifndef OHA_DNS_REQUEST_LOWER
LSA_VOID OHA_DNS_REQUEST_LOWER(
    OHA_DNS_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE            sys_ptr
)
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DNS );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_DNS_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_DNS_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_DNS_RQB_GET_OPCODE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_request, lower_rqb_ptr );
}
#else
# error "by design a function"
#endif

#endif /* DNS */


/*----------------------------------------------------------------------------*/

#ifdef OHA_CFG_USE_SNMPX

#ifndef OHA_SNMPX_OPEN_CHANNEL_LOWER
LSA_VOID OHA_SNMPX_OPEN_CHANNEL_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_SNMPX_CLOSE_CHANNEL_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_REQUEST_LOWER
LSA_VOID OHA_SNMPX_REQUEST_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_SNMPX_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_SNMPX_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_SNMPX_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#endif /* SNMPX */

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_NARE

#ifndef OHA_NARE_OPEN_CHANNEL_LOWER
LSA_VOID OHA_NARE_OPEN_CHANNEL_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
        // post the message to HIF HD (NARE is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to NARE (without HIF NARE is located in same system)
	    psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_NARE_CLOSE_CHANNEL_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (NARE is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to NARE (without HIF NARE is located in same system)
	    psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_REQUEST_LOWER
LSA_VOID OHA_NARE_REQUEST_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_NARE_REQUEST_LOWER(): rqb(0x%08x) h(%d) opc(%d)",
		lower_rqb_ptr,
		OHA_NARE_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_NARE_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (NARE is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to NARE (without HIF NARE is located in same system)
	    psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#endif  /* NARE */

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_MRP

#ifndef OHA_MRP_OPEN_CHANNEL_LOWER
LSA_VOID OHA_MRP_OPEN_CHANNEL_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (MRP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to MRP (without HIF MRP is located in same system)
	    psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_open_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_MRP_CLOSE_CHANNEL_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (MRP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to MRP (without HIF MRP is located in same system)
	    psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_close_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_REQUEST_LOWER
LSA_VOID OHA_MRP_REQUEST_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_MRP_REQUEST_LOWER(): rqb(0x%08x) h(%d) opc(%d)",
		lower_rqb_ptr,
		OHA_MRP_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_MRP_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (MRP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to MRP (without HIF MRP is located in same system)
	    psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_request, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#endif  /* MRP */

/*----------------------------------------------------------------------------*/

#ifndef OHA_LLDP_OPEN_CHANNEL_LOWER
LSA_VOID OHA_LLDP_OPEN_CHANNEL_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (LLDP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to LLDP (without HIF LLDP is located in same system)
	    psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_open_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_LLDP_CLOSE_CHANNEL_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (LLDP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to LLDP (without HIF LLDP is located in same system)
	    psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_close_channel, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_REQUEST_LOWER
LSA_VOID OHA_LLDP_REQUEST_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_LLDP_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_LLDP_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_LLDP_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
    if( pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO )
    {
	    // post the message to HIF HD (LLDP is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
    {
	    // post the message to LLDP (without HIF LLDP is located in same system)
	    psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_request, lower_rqb_ptr );
    }
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_MEMSET
LSA_VOID OHA_MEMSET(
	OHA_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef OHA_REQUEST_UPPER_DONE
LSA_VOID OHA_REQUEST_UPPER_DONE(
	OHA_UPPER_CALLBACK_FCT_PTR_TYPE  oha_request_upper_done_ptr,
	OHA_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)oha_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_MEMCPY
LSA_VOID OHA_MEMCPY(
	OHA_COMMON_MEM_PTR_TYPE pDst,
	OHA_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32              Length )
{
	PSI_MEMCPY( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MEMMOVE
LSA_VOID OHA_MEMMOVE(
	OHA_COMMON_MEM_PTR_TYPE  pDst,
	OHA_COMMON_MEM_PTR_TYPE  pSrc,
	LSA_UINT32               Length )
{
	PSI_MEMMOVE( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MEMCMP
LSA_BOOL OHA_MEMCMP(
	OHA_COMMON_MEM_PTR_TYPE pBuf1,
	OHA_COMMON_MEM_PTR_TYPE pBuf2,
	LSA_UINT32              Length )
{
	return( PSI_MEMCMP( pBuf1, pBuf2, (LSA_UINT)Length ) == 0 ? LSA_TRUE : LSA_FALSE );
    //lint --e(818) Pointer parameter 'pBuf1' / 'pBuf2' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:    OHA_HTONS ... host to network byte order, short integer
 *                      _NTOHS ... network to host byte order, short integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONS
LSA_UINT16 OHA_HTONS(
	LSA_UINT16 host_short )
{
    #ifndef LSA_HOST_ENDIANESS_BIG
	PSI_SWAP_U16(&host_short);
    #endif
	return host_short;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NTOHS
LSA_UINT16 OHA_NTOHS(
	LSA_UINT16 network_short )
{
    #ifndef LSA_HOST_ENDIANESS_BIG
	PSI_SWAP_U16(&network_short);
    #endif
	return network_short;
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:    OHA_HTONL ... host to network byte order, long  integer
 *                      _NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONL
LSA_UINT32 OHA_HTONL(
	LSA_UINT32 host_long )
{
    #ifndef LSA_HOST_ENDIANESS_BIG
	PSI_SWAP_U32(&host_long);
    #endif
	return host_long;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NTOHL
LSA_UINT32 OHA_NTOHL(
	LSA_UINT32 network_long )
{
    #ifndef LSA_HOST_ENDIANESS_BIG
	PSI_SWAP_U32(&network_long);
    #endif
	return network_long;
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//  read data from PDU / write data to PDU
//----------------------------------------------------------------------------*/

#ifndef OHA_PUT8_HTON
LSA_VOID OHA_PUT8_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT8         val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr[offset] = val;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET8_NTOH
LSA_UINT8 OHA_GET8_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	return ( ptr[offset] );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_PUT16_HTON
LSA_VOID OHA_PUT16_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT16        val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	val = PSI_HTON16( val );
	PSI_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET16_NTOH
LSA_UINT16 OHA_GET16_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT16                 val = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	PSI_MEMCPY( &val, ptr, sizeof(val) );
	val = PSI_NTOH16( val );
	return val;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_PUT32_HTON
LSA_VOID OHA_PUT32_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT32        val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	val = PSI_HTON32( val );
	PSI_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET32_NTOH
LSA_UINT32 OHA_GET32_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT32  val = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	PSI_MEMCPY( &val, ptr, sizeof(val) );
	val = PSI_NTOH32( val );
	return val;
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  OHA_GET_SYS_UPTIME
 *
 * function:       The OHA_GET_SYS_UPTIME function retrieves the number of 10 milliseconds
 *                 that have elapsed since the system was started.
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_GET_SYS_UPTIME
LSA_UINT32 OHA_GET_SYS_UPTIME( LSA_VOID )
{
    LSA_UINT64 const dwStart = PSI_GET_TICKS_100NS() / 100000;  /* Calculate from 100 ns to 10 milliseconds ticks, don't lose percision here -> divide with UIN64 type! */
    return ( (LSA_UINT32)(dwStart - oha_sys_uptime_startup_10ms));
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++  DCP-Server  +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_IND
 * function:       Trace incoming SET Requests
 *                 Called before calling "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 *  ReqPtr:     -> SET request data
 *                 LSA_INT      ReqLen:     Length of request
 * return value:   LSA_UINT8   Reason:      The first Result-field of the
 *                                          SET-Response will be set to this value.
 *
 *  OHA_DCP_REASON_OK (no error)        Positive response, accept a SET and deliver parameters
 *  OHA_DCP_REASON_NO_DATABASE          Option not supported
 *  OHA_DCP_REASON_NO_DATASET           uboption not supported or no dataset available
 *  OHA_DCP_REASON_DS_NOT_SET (default) Suboption not set (local reasons)
 *  OHA_DCP_REASON_NO_RESOURCE          (temporary) resource error (server)
 *  OHA_DCP_REASON_NOT_POSSIBLE         SET not possible (local reasons)
 *  OHA_DCP_REASON_NOT_APPLICABLE       SET not possible (application operation)
 *
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_IND
LSA_UINT8 OHA_REPORT_DCP_SET_IND(
	LSA_UINT16                          NicId,
	OHA_UPPER_DCP_INDICATION_PTR_TYPE   pDcpInd,
	LSA_UINT8                         * ReqPtr,
	LSA_INT                             ReqLen )
{
	LSA_UNUSED_ARG( NicId );
	LSA_UNUSED_ARG( pDcpInd );
	LSA_UNUSED_ARG( ReqPtr );
	LSA_UNUSED_ARG( ReqLen );

	return OHA_DCP_REASON_OK;
}
#endif

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_RES
 * function:       Trace End of SET Request
 *                 Called after calling all "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 * RspPtr:      -> SET response data
 *                 LSA_INT     RspLen:      Length of response
 * return value:   -
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_RES
LSA_VOID OHA_REPORT_DCP_SET_RES(
	LSA_UINT16                     NicId,
	OHA_DCP_INDICATION_TYPE        DcpInd,
	LSA_UINT8                    * RspPtr,
	LSA_INT                        RspLen )
{
	LSA_UNUSED_ARG( NicId );
	LSA_UNUSED_ARG( DcpInd );
	LSA_UNUSED_ARG( RspPtr );
	LSA_UNUSED_ARG( RspLen );
}
#endif

#endif // PSI_CFG_USE_LD_COMP
