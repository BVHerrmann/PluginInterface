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
/*  F i l e               &F: sock_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SOCK using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2800
#define PSI_MODULE_ID       2800 /* PSI_MODULE_ID_SOCK_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

#include "sock_int.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/


#if (PSI_CFG_TCIP_STACK_INTERNICHE == 1) && (PSI_CFG_USE_TCIP == 1)
#include "snmpport.h"
#include "snmp_imp.h"
#elif (PSI_CFG_TCIP_STACK_OPEN_BSD == 1) && (PSI_CFG_USE_TCIP == 1)
/* OBSD_ITGR -- TODO: implement snmp_agt_parse */
int snmp_agt_parse(void * p, ...) { LSA_UNUSED_ARG(p); return 0; /* error */ } // quick and dirty
#endif

// Using WinSock
#if (PSI_CFG_TCIP_STACK_WINSOCK == 1)
#include <WinSock.h> // wegen u_char
/* note that ...\oha\test\mib2 has no header file with a protoype */
int snmp_agt_parse(u_char*, unsigned, u_char*, unsigned);
#endif

/*===========================================================================*/

#if (PSI_CFG_USE_TCIP == 0) || (!SOCK_INT_CFG_SOCKET_EVENTS) // NOT using TCIP or no events for socket used
static LSA_BOOL       sock_select_enabled = LSA_FALSE;
#endif

#if (PSI_CFG_USE_TCIP == 0) // NOT using TCIP
/*static*/ LSA_UINT16 sock_oneshot_TimerID;
/*static*/ LSA_VOID   sock_oneshot( LSA_UINT16 timer_id, LSA_USER_ID_TYPE user_id );
#endif

static LSA_UINT16 sock_enter_exit_id = PSI_LOCK_ID_INVALID;

/*===========================================================================*/
/*===  START/STOP  ==========================================================*/
/*===========================================================================*/

void sock_psi_startstop( int start )
{
	LSA_UINT32 result;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &sock_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		result = sock_init();
		PSI_ASSERT( LSA_RET_OK == result );

#if ( PSI_CFG_USE_TCIP == 0) || (!SOCK_INT_CFG_SOCKET_EVENTS) // NOT using TCIP or no events for socket used
		sock_select_enabled = LSA_TRUE; /* enable before timer-start (one-shot!) */
#endif

#if ( PSI_CFG_USE_TCIP == 0) // NOT using TCIP
		{
			LSA_UINT16          retVal;
	        LSA_USER_ID_TYPE    null_usr_id;

            PSI_INIT_USER_ID_UNION(null_usr_id);

            PSI_ALLOC_TIMER_TGROUP1( &retVal, &sock_oneshot_TimerID, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_10MS, sock_oneshot );
			PSI_ASSERT( LSA_RET_OK == retVal );

			PSI_START_TIMER( &retVal, sock_oneshot_TimerID, null_usr_id, 1 ); /* 10ms */
			PSI_ASSERT( LSA_RET_OK == retVal /*|| LSA_RET_OK_TIMER_RESTARTED == retVal*/);
		}
#endif
	}
	else
	{

#if ( PSI_CFG_USE_TCIP == 0) || (!SOCK_INT_CFG_SOCKET_EVENTS) // NOT using TCIP or no events for socket used
		sock_select_enabled = LSA_FALSE;
#endif

#if ( PSI_CFG_USE_TCIP == 0) // NOT using TCIP
		{
			LSA_UINT16 retVal;

			PSI_STOP_TIMER( &retVal, sock_oneshot_TimerID );
			PSI_ASSERT( LSA_RET_OK == retVal || LSA_RET_OK_TIMER_NOT_RUNNING == retVal);

			PSI_FREE_TIMER( &retVal, sock_oneshot_TimerID );
			PSI_ASSERT( LSA_RET_OK == retVal );
		}
#endif

        result = sock_undo_init();
        PSI_ASSERT(LSA_RET_OK == result);

		PSI_FREE_REENTRANCE_LOCK( &rsp, sock_enter_exit_id );
		PSI_ASSERT( rsp == LSA_RET_OK );

		sock_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*===========================================================================*/

#ifndef SOCK_GET_PATH_INFO
LSA_VOID SOCK_GET_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE     * sys_ptr_ptr,
	SOCK_DETAIL_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE      path )
{
	*ret_val_ptr = psi_sock_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef SOCK_RELEASE_PATH_INFO
LSA_VOID SOCK_RELEASE_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr,
	SOCK_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_sock_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_REQUEST_UPPER_DONE
LSA_VOID SOCK_REQUEST_UPPER_DONE(
	SOCK_UPPER_CALLBACK_FCT_PTR_TYPE sock_request_upper_done_ptr,
	SOCK_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)sock_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#if ( PSI_CFG_USE_TCIP == 0) // NOT using TCIP

// poll-timer for not TCIP platforms (i.E.: using WIN32 IP-Stack)

LSA_VOID sock_oneshot( LSA_UINT16 timer_id, LSA_USER_ID_TYPE user_id )
{
	PSI_ASSERT( timer_id == sock_oneshot_TimerID );

	// running in timer thread context

	if( sock_select_enabled )
	{
		LSA_UINT16 retVal;

		PSI_START_TIMER( &retVal, timer_id, user_id, 1 ); // 10ms
		PSI_ASSERT( LSA_RET_OK == retVal || LSA_RET_OK_TIMER_RESTARTED == retVal );

		SOCK_START_SELECT();
	}
}

#endif

/*===========================================================================*/

#if !SOCK_INT_CFG_SOCKET_EVENTS
#ifndef SOCK_START_SELECT
LSA_VOID SOCK_START_SELECT( LSA_VOID )
{
	if ( sock_select_enabled )
	{
		static SOCK_RQB_TYPE rqb = {0};

		// Post message to sock context
		psi_request_local( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_doselect, &rqb );
	}
}
#else
#error "by design a function!"
#endif
#endif // PSI_CFG_TCIP_STACK_OPEN_BSD

/*===========================================================================*/

#ifndef SOCK_ALLOC_TIMER
LSA_VOID SOCK_ALLOC_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * timer_id_ptr,
	LSA_UINT16   timer_type,
	LSA_UINT16   time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, sock_timeout );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_START_TIMER
LSA_VOID SOCK_START_TIMER(
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

/*===========================================================================*/

#ifndef SOCK_STOP_TIMER
LSA_VOID SOCK_STOP_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_FREE_TIMER
LSA_VOID SOCK_FREE_TIMER(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_ENTER
LSA_VOID SOCK_ENTER( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( sock_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_EXIT
LSA_VOID SOCK_EXIT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( sock_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_INTERLOCKED_EXCHANGE
SOCK_EXCHANGE_TYPE SOCK_INTERLOCKED_EXCHANGE(
	SOCK_EXCHANGE_TYPE volatile *ptr,
	SOCK_EXCHANGE_TYPE val )
{
	return( PSI_EXCHANGE_LONG( ptr, val ) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_FATAL_ERROR
LSA_VOID SOCK_FATAL_ERROR(
	LSA_UINT16                length,
	SOCK_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "sock", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_REQUEST_LOCAL
LSA_VOID SOCK_REQUEST_LOCAL(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMSET
LSA_VOID SOCK_MEMSET(
	LSA_VOID   * pMem,
	LSA_UINT8    Value,
	LSA_UINT32   Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMCPY
LSA_VOID SOCK_MEMCPY(
	LSA_VOID   * pDst,
	LSA_VOID   * pSrc,
	LSA_UINT32   Length )
{
	PSI_MEMCPY( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMMOVE
LSA_VOID SOCK_MEMMOVE(
	LSA_VOID      * pDst,
	LSA_VOID      * pSrc,
	LSA_UINT32      Length )
{
	PSI_MEMMOVE( pDst, pSrc, (LSA_UINT)Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMCMP
LSA_BOOL SOCK_MEMCMP(
	LSA_VOID        *  pBuf1,
	LSA_VOID        *  pBuf2,
	LSA_UINT32         Length )
{
	// Note: In case of differ LSA_TRUE as result is expected, not LSA_FALSE (see sock_sys.h)
	return ( PSI_MEMCMP(pBuf1, pBuf2, (LSA_UINT)Length) == 0 ? LSA_FALSE : LSA_TRUE );
    //lint --e(818) Pointer parameter 'pBuf1' / 'pBuf2' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_STRLEN
LSA_UINT32 SOCK_STRLEN(
	const LSA_CHAR * str )
{
	return ( (LSA_UINT32)PSI_STRLEN( (LSA_VOID_PTR_TYPE)str) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#if ( PSI_CFG_USE_TCIP == 1)
#ifndef SOCK_SNMP_AGT_PARSE
// This implementation uses snmp_agt_parse from the interniche code
LSA_VOID SOCK_SNMP_AGT_PARSE(
	LSA_UINT8  * precv_buffer,
	LSA_UINT     len,
	LSA_UINT8  * psnmp_buffer,
	LSA_UINT     snmp_len,
	LSA_UINT16 * pret_val )
{
	PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT,
		"SOCK_SNMP_AGT_PARSE(): RecBuffer=(%p) Size:(%u) SNMP-Buffer=(%p) Size:(%u)",
		precv_buffer, len, psnmp_buffer, snmp_len );

	*pret_val = (LSA_UINT16)snmp_agt_parse( precv_buffer, len, psnmp_buffer, snmp_len );
}
#else
#error "by design a function!"
#endif
#else // PSI_CFG_USE_TCIP == 0
// Call the output function PSI_SOCK_SNMP_AGT_PARSE. A systemadaptation with a custom IP stack must implement this function. See LSA_SOCK_USR_SPEC_en.doc
LSA_VOID SOCK_SNMP_AGT_PARSE(
    LSA_UINT8  * precv_buffer,
    LSA_UINT     len,
    LSA_UINT8  * psnmp_buffer,
    LSA_UINT     snmp_len,
    LSA_UINT16 * pret_val )
{
    PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT,
        "SOCK_SNMP_AGT_PARSE(): RecBuffer=(%p) Size:(%u) SNMP-Buffer=(%p) Size:(%u)",
        precv_buffer, len, psnmp_buffer, snmp_len );

    *pret_val = PSI_SOCK_SNMP_AGT_PARSE( precv_buffer, len, psnmp_buffer, snmp_len);
}
#endif

#endif // PSI_CFG_USE_LD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
