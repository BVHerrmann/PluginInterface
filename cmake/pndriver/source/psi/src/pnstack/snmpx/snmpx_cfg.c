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
/*  F i l e               &F: snmpx_cfg.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SNMPX using PSI.                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2700
#define PSI_MODULE_ID       2700 /* PSI_MODULE_ID_SNMPX_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_UINT16 snmpx_enter_exit_id = PSI_LOCK_ID_INVALID;

/*----------------------------------------------------------------------------*/

void snmpx_psi_startstop( int start )
{
	LSA_UINT16        rc;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &snmpx_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		rc = snmpx_init();
		PSI_ASSERT(rc == SNMPX_OK);
	}
	else
	{
		rc = snmpx_undo_init();
		PSI_ASSERT(rc == SNMPX_OK);

		PSI_FREE_REENTRANCE_LOCK( &rsp, snmpx_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		snmpx_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_GET_PATH_INFO
LSA_VOID SNMPX_GET_PATH_INFO(
	LSA_UINT16            * ret_val_ptr,
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	SNMPX_DETAIL_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       path )
{
	*ret_val_ptr = psi_snmpx_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_RELEASE_PATH_INFO
LSA_VOID SNMPX_RELEASE_PATH_INFO(
	LSA_UINT16            * ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	SNMPX_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_snmpx_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_ALLOC_TIMER
LSA_VOID SNMPX_ALLOC_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE * timer_id_ptr,
    LSA_UINT16   timer_type,
    LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, snmpx_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_START_TIMER
LSA_VOID SNMPX_START_TIMER(
    LSA_UINT16       * ret_val_ptr,
    LSA_TIMER_ID_TYPE  timer_id,
    LSA_USER_ID_TYPE   user_id,
    LSA_UINT16         time )
{
	PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_STOP_TIMER
LSA_VOID SNMPX_STOP_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_FREE_TIMER
LSA_VOID SNMPX_FREE_TIMER(
    LSA_UINT16 * ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_DO_TIMER_REQUEST
LSA_VOID SNMPX_DO_TIMER_REQUEST(
    SNMPX_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_FATAL_ERROR
LSA_VOID SNMPX_FATAL_ERROR(
	LSA_UINT16                 length,
	SNMPX_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "snmpx", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_ENTER
LSA_VOID SNMPX_ENTER( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( snmpx_enter_exit_id );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_EXIT
LSA_VOID SNMPX_EXIT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( snmpx_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID SNMPX_SOCK_OPEN_CHANNEL_LOWER(
	SNMPX_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID SNMPX_SOCK_CLOSE_CHANNEL_LOWER(
	SNMPX_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_SOCK_REQUEST_LOWER
LSA_VOID SNMPX_SOCK_REQUEST_LOWER(
	SNMPX_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
		"SNMPX_SOCK_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		SNMPX_SOCK_RQB_GET_HANDLE(lower_rqb_ptr),
		SNMPX_SOCK_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_MEMSET
LSA_VOID SNMPX_MEMSET(
	SNMPX_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_REQUEST_UPPER_DONE
LSA_VOID SNMPX_REQUEST_UPPER_DONE(
	SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE snmpx_request_upper_done_ptr,
	SNMPX_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                  sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)snmpx_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef SNMPX_MEMCPY
LSA_VOID SNMPX_MEMCPY(
	SNMPX_COMMON_MEM_PTR_TYPE pDst,
	SNMPX_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32                Length )
{
	PSI_MEMCPY( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_MEMMOVE
LSA_VOID SNMPX_MEMMOVE(
	SNMPX_COMMON_MEM_PTR_TYPE pDst,
	SNMPX_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32                Length )
{
	PSI_MEMMOVE( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_MEMCMP
LSA_BOOL SNMPX_MEMCMP(
	SNMPX_COMMON_MEM_PTR_TYPE pBuf1,
	SNMPX_COMMON_MEM_PTR_TYPE pBuf2,
	LSA_UINT32                Length )
{
	return( PSI_MEMCMP(pBuf1, pBuf2, (LSA_UINT)Length) == 0 ? LSA_TRUE : LSA_FALSE );
    //lint --e(818) Pointer parameter 'pBuf1' / 'pBuf2' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:    SNMPX_HTONS ... host to network byte order, short integer
 *                      _NTOHS ... network to host byte order, short integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_HTONS
LSA_UINT16 SNMPX_HTONS(
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

#ifndef SNMPX_NTOHS
LSA_UINT16 SNMPX_NTOHS(
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
 * function name:    SNMPX_HTONL ... host to network byte order, long  integer
 *                      _NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_SNMPX_Detailspec.doc
 *
 *===========================================================================*/
#ifndef SNMPX_HTONL
LSA_UINT32 SNMPX_HTONL(
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

#ifndef SNMPX_NTOHL
LSA_UINT32 SNMPX_NTOHL(
	LSA_UINT32 network_long )
{
    #ifndef LSA_HOST_ENDIANESS_BIG
	PSI_SWAP_U32( &network_long );
    #endif
	return network_long;
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
    read data from PDU / write data to PDU
  ----------------------------------------------------------------------------*/

#ifndef SNMPX_PUT8_HTON
LSA_VOID SNMPX_PUT8_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT8         val )
{
	LSA_UINT8 *ptr = base;
	ptr[offset] = val;
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_GET8_NTOH
LSA_UINT8 SNMPX_GET8_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT8 *ptr = base;
	return( ptr[offset] );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_PUT16_HTON
LSA_VOID SNMPX_PUT16_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT16        val )
{
	LSA_UINT8 *ptr = base;
	ptr += offset;
	val = PSI_HTON16 (val);
	PSI_MEMCPY (ptr, & val, sizeof(val));
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_GET16_NTOH
LSA_UINT16 SNMPX_GET16_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT16  val = 0;
	LSA_UINT8 *ptr = base;
	ptr += offset;
	PSI_MEMCPY (& val, ptr, sizeof(val));
	val = PSI_NTOH16 ( val );
	return val;
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_PUT32_HTON
LSA_VOID SNMPX_PUT32_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT32        val )
{
	LSA_UINT8 *ptr = base;
	ptr += offset;
	val = PSI_HTON32 (val);
	PSI_MEMCPY (ptr, & val, sizeof(val));
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_GET32_NTOH
LSA_UINT32 SNMPX_GET32_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT32  val = 0;
	LSA_UINT8 *ptr = base;
	ptr += offset;
	PSI_MEMCPY (& val, ptr, sizeof(val));
	val = PSI_NTOH32 (val);
	return val;
}
#else
#error "by design a function!"
#endif

/* ===================================================================================================== */
#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)

#ifndef SNMPX_OHA_OPEN_CHANNEL_LOWER
LSA_VOID SNMPX_OHA_OPEN_CHANNEL_LOWER(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_OHA_CLOSE_CHANNEL_LOWER
LSA_VOID SNMPX_OHA_CLOSE_CHANNEL_LOWER(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef SNMPX_OHA_REQUEST_LOWER
LSA_VOID SNMPX_OHA_REQUEST_LOWER(
    SNMPX_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
		"SNMPX_OHA_REQUEST_LOWER(): rqb(0x%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		SNMPX_OHA_RQB_GET_HANDLE(lower_rqb_ptr),
		SNMPX_OHA_RQB_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_TCIP_STACK_OPEN_BSD
/* ===================================================================================================== */

#endif // PSI_CFG_USE_LD_COMP

/*****************************************************************************/
/*  end of file snmpx_cfg.c                                                  */
/*****************************************************************************/
