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
/*  F i l e               &F: tcip_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of TCIP using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2900
#define PSI_MODULE_ID       2900 /* PSI_MODULE_ID_TCIP_CFG */

#include "psi_int.h"

#if ((PSI_CFG_USE_TCIP == 1) && (PSI_CFG_USE_LD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#if TCIP_CFG_INTERNICHE

#include "tcip_iniche_cfg.h"

#elif TCIP_CFG_OPEN_BSD	/* OBSD_ITGR */

#include "tcip_obsd_cfg.h"
#include <obsd_kernel_BSDStackPort.h>
#include <obsd_platform_osal.h>

#ifdef EPS_PLF
#include <eps_tasks.h>
#else
#include <pcpnio_sys.h>
#endif

/*----------------------------------------------------------------------------*/
#define OBSD_PAGE_SIZE          PSI_CFG_TCIP_CFG_OBSD_PAGE_SIZE
#define OBSD_NUMBER_OF_PAGES    PSI_CFG_TCIP_CFG_OBSD_NUMBER_OF_PAGES

/*----------------------------------------------------------------------------*/
/* OpenBSD Memory                                                             */
/*----------------------------------------------------------------------------*/
#define ZERO_FLAG 0x0008 /* if flag is set, memory is initialized with 0 */

#ifdef OBSD_MEMORY_TRACING
/*----------------------------------------------------------------------------*/
/* OpenBSD -- List of allocated OpenBSD Memory, has to be freed at shutdown   */
/*----------------------------------------------------------------------------*/
struct obsd_mem_entry {
	void *                  allocated;
	struct obsd_mem_entry * next;
};

static struct obsd_mem_entry * OBSD_ALLOCATED_MEM_LIST;
#endif

#endif // TCIP_CFG_OPEN_BSD

/*----------------------------------------------------------------------------*/
#if TCIP_CFG_INTERNICHE
    #define TCIP_MEMORY_POOL_SIZE       0x3E8000UL                              // size of TCIP memory pool = 4MB (4000 * 1024 Byte)
#elif TCIP_CFG_OPEN_BSD
    #define TCIP_MEMORY_POOL_SIZE       0x1F4000UL                              // size of TCIP memory pool = 2MB (2000 * 1024 Byte)
    #define OBSD_PAGE_MEMORY_POOL_SIZE  (OBSD_PAGE_SIZE * OBSD_NUMBER_OF_PAGES) // size of OpenBSD Page memory pool
#endif

static LSA_VOID_PTR_TYPE tcip_memory_pool_base_ptr      = 0;    // base ptr to allocated TCIP memory pool

#if TCIP_CFG_OPEN_BSD
static LSA_VOID_PTR_TYPE obsd_page_memory_pool_base_ptr = 0;    // base ptr to allocated OpenBSD Page memory pool
#endif

/*----------------------------------------------------------------------------*/
#if TCIP_CFG_INTERNICHE
static LSA_UINT16 tcip_enter_exit_interniche_id = PSI_LOCK_ID_INVALID;
#endif

/*----------------------------------------------------------------------------*/

void tcip_psi_startstop( int start )
{
    LSA_UINT16 retVal;

	if ( start )
	{
        #if TCIP_CFG_INTERNICHE
        retVal = 0;
		PSI_ALLOC_REENTRANCE_LOCK( &retVal, &tcip_enter_exit_interniche_id );
		PSI_ASSERT( retVal == LSA_RET_OK );
        #endif

	    // allocate TCIP memory pool
        {
            LSA_UINT32       const size        = TCIP_MEMORY_POOL_SIZE;
            LSA_USER_ID_TYPE       null_usr_id;
            PSI_INIT_USER_ID_UNION(null_usr_id);
            PSI_ALLOC_LOCAL_MEM( &tcip_memory_pool_base_ptr, null_usr_id, size, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM ); // allocate TCIP memory pool from HEAP
            retVal = 0;
            PSI_CREATE_TCIP_MEM_POOL( &retVal, tcip_memory_pool_base_ptr, size );
		    PSI_ASSERT( retVal == LSA_RET_OK );
        }

		retVal = tcip_init();
		PSI_ASSERT( retVal == LSA_RET_OK );
	}
	else // stop
	{
		retVal = tcip_undo_init();
		PSI_ASSERT( retVal == LSA_RET_OK );

	    // free TCIP memory pool
        {
            retVal = 0;
            PSI_DELETE_TCIP_MEM_POOL( &retVal );
		    PSI_ASSERT( retVal == LSA_RET_OK );
            retVal = 0;
            PSI_FREE_LOCAL_MEM( &retVal, tcip_memory_pool_base_ptr, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM ); // free TCIP memory pool from HEAP
            PSI_ASSERT( retVal == LSA_RET_OK );
            tcip_memory_pool_base_ptr = 0;
        }

        #if TCIP_CFG_INTERNICHE
        retVal = 0;
		PSI_FREE_REENTRANCE_LOCK( &retVal, tcip_enter_exit_interniche_id );
		PSI_ASSERT( retVal == LSA_RET_OK );
		tcip_enter_exit_interniche_id = PSI_LOCK_ID_INVALID;
        #endif
	}
}

/*----------------------------------------------------------------------------*/

#if TCIP_INT_CFG_TRACE_ON /* CPU Messung */
//#include "cm_inc.h"
#define PNTRC_HELPER_GET_HIGH_VALUE(_val) ((_val >> 32) & 0xFFFFFFFF)
#define PNTRC_HELPER_GET_LOW_VALUE(_val) (_val & 0xFFFFFFFF)

#ifndef TCIP_GET_TIMESTAMP
LSA_VOID TCIP_GET_TIMESTAMP(
	LSA_UINT64 * current_time_stamp_in_us )
{
	/* map to CM's macro */
	static LSA_UINT64 old_value = 0;

	*current_time_stamp_in_us = PSI_GET_TICKS_100NS() / 10;

    if ((PNTRC_HELPER_GET_LOW_VALUE(*current_time_stamp_in_us) & 0xF0000000) != (PNTRC_HELPER_GET_LOW_VALUE(old_value) & 0xF0000000))
    {
        LSA_UINT32  cul, pal;
	    cul =  PNTRC_HELPER_GET_LOW_VALUE(*current_time_stamp_in_us);
	    pal = PNTRC_HELPER_GET_LOW_VALUE(old_value);
        PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_LOW, "TCIP_GET_TIMESTAMP(): Timer low 0xF0000000 value changed cur(low: 0x%08x), past(low: 0x%08x)", cul, pal );
    }

    if (PNTRC_HELPER_GET_HIGH_VALUE(*current_time_stamp_in_us) != PNTRC_HELPER_GET_HIGH_VALUE(old_value))
    {
        LSA_UINT32  cuh, pah;
	    cuh =  PNTRC_HELPER_GET_HIGH_VALUE(*current_time_stamp_in_us);
	    pah = PNTRC_HELPER_GET_HIGH_VALUE(old_value);
        PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_LOW, "TCIP_GET_TIMESTAMP(): Timer high value changed cur(hi: 0x%08x), past(hi: 0x%08x)", cuh, pah );
    }

	if (old_value > *current_time_stamp_in_us)
	{
        LSA_UINT32  pah, pal, cuh, cul;

        pal = PNTRC_HELPER_GET_LOW_VALUE(old_value);
        pah = PNTRC_HELPER_GET_HIGH_VALUE(old_value);
        cul = PNTRC_HELPER_GET_LOW_VALUE(*current_time_stamp_in_us);
        cuh = PNTRC_HELPER_GET_HIGH_VALUE(*current_time_stamp_in_us);

        PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_FATAL, "TCIP_GET_TIMESTAMP(): Timer overflow! past(hi: 0x%08x, low: 0x%08x) > cur(hi: 0x%08x, low: 0x%08x)",
            pah, pal, cuh, cul );
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
	}

	old_value = *current_time_stamp_in_us;
}
#endif
#endif

#ifndef TCIP_GET_PATH_INFO
LSA_VOID TCIP_GET_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE     * sys_ptr_ptr,
	TCIP_DETAIL_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE      path )
{
	*ret_val_ptr = psi_tcip_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef TCIP_RELEASE_PATH_INFO
LSA_VOID TCIP_RELEASE_PATH_INFO(
	LSA_UINT16           * ret_val_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr,
	TCIP_DETAIL_PTR_TYPE   detail_ptr )
{
	*ret_val_ptr = psi_tcip_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	System Layer (prototypes in tcip_sys.h)
//----------------------------------------------------------------------------*/

#ifndef TCIP_ALLOC_LOCAL_MEM
LSA_VOID TCIP_ALLOC_LOCAL_MEM(
	TCIP_LOCAL_MEM_PTR_TYPE * local_mem_ptr_ptr,
	LSA_UINT16                length )
{
    PSI_TCIP_ALLOC_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, (LSA_UINT32)length );
	if ( *local_mem_ptr_ptr == LSA_NULL )
	{
	    PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_WARN, "TCIP_ALLOC_LOCAL_MEM(): No more memory" );
    }
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FREE_LOCAL_MEM
LSA_VOID TCIP_FREE_LOCAL_MEM(
	LSA_UINT16              * ret_val_ptr,
	TCIP_LOCAL_MEM_PTR_TYPE   local_mem_ptr )
{
    PSI_TCIP_FREE_MEM( ret_val_ptr, local_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMSET
LSA_VOID TCIP_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT val,
	LSA_UINT len )
{
	PSI_MEMSET( ptr, val, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMCPY
LSA_VOID TCIP_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len )
{
	PSI_MEMCPY( dst, src, len );
    //lint --e(818) Pointer parameter 'src' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#if TCIP_CFG_INTERNICHE
#ifndef TCIP_MEMMOVE
LSA_VOID TCIP_MEMMOVE(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len )
{
	PSI_MEMMOVE( dst, src, len );
    //lint --e(818) Pointer parameter 'src' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMCMP
LSA_INT TCIP_MEMCMP(
	LSA_VOID_PTR_TYPE buf1,
	LSA_VOID_PTR_TYPE buf2,
	LSA_UINT len )
{
	return ( PSI_MEMCMP( buf1, buf2, len ) );
    //lint --e(818) Pointer parameter 'buf1' / 'buf2' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif
#endif // TCIP_CFG_INTERNICHE

/*----------------------------------------------------------------------------*/

#ifndef TCIP_ALLOC_TIMER
LSA_VOID TCIP_ALLOC_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE * timer_id_ptr,
	LSA_UINT16          timer_type,
	LSA_UINT16          time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, tcip_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_START_TIMER
LSA_VOID TCIP_START_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time )
{
	PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_STOP_TIMER
LSA_VOID TCIP_STOP_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FREE_TIMER
LSA_VOID TCIP_FREE_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_ARP_RESPONSE_RECEIVED
LSA_VOID TCIP_ARP_RESPONSE_RECEIVED(
	LSA_UINT16                 group_id, /* must have the same value as NARE_CDB_PATH_PARAMS_TYPE::NICId */
	EDD_UPPER_MEM_U8_PTR_TYPE  frame_buf,
	LSA_UINT16                 frame_len )
{
	// NOTE must be empty, because the ARP sniffing is done on HD level (see nare_cfg.c)
	LSA_UNUSED_ARG( group_id );
	LSA_UNUSED_ARG( frame_buf );
	LSA_UNUSED_ARG( frame_len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FATAL_ERROR
LSA_VOID TCIP_FATAL_ERROR(
	LSA_UINT16                length,
	TCIP_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "tcip", length, error_ptr );
}
#else
#pragma PSI_MESSAGE("TCIP_FATAL_ERROR redefined by compiler switch to: " TCIP_STRINGIFY(TCIP_FATAL_ERROR))
#endif

/*------------------------------------------------------------------------------
//	Upper Layer (prototypes in tcip_usr.h)
//----------------------------------------------------------------------------*/

#ifndef TCIP_REQUEST_UPPER_DONE
LSA_VOID TCIP_REQUEST_UPPER_DONE(
	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE tcip_request_upper_done_ptr,
	TCIP_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)tcip_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_REQUEST_LOCAL
LSA_VOID TCIP_REQUEST_LOCAL(
	TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_TCIP, (PSI_REQUEST_FCT)tcip_request, upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#if TCIP_CFG_INTERNICHE

#ifndef LOCK_NET_RESOURCE
void LOCK_NET_RESOURCE( int res )
{
	LSA_UNUSED_ARG( res );
	PSI_ENTER_REENTRANCE_LOCK( tcip_enter_exit_interniche_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef UNLOCK_NET_RESOURCE
void UNLOCK_NET_RESOURCE( int res )
{
	LSA_UNUSED_ARG( res );
	PSI_EXIT_REENTRANCE_LOCK( tcip_enter_exit_interniche_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

void tcp_sleep( void * event )
{
	/* see nsref_complete.htm, chapter "Net Resource Method" */
	PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, (LSA_UINT32)event ); /* must not be called for NBIO */
}

/*---------------------------------------------------------------------------*/

void tcp_wakeup( void * _event )
{
	/* see nsref_complete.htm, chapter "Net Resource Method" */
	LSA_UNUSED_ARG(_event);  /* is called by stack although NBIO is used */
}

#endif // TCIP_CFG_INTERNICHE

/*===========================================================================*/

#ifndef TCIP_EXCHANGE_LONG
long TCIP_EXCHANGE_LONG(
	long volatile * ptr,
	long            val )
{
	return( PSI_EXCHANGE_LONG( ptr, val ) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_OPEN_CHANNEL_LOWER
LSA_VOID TCIP_EDD_OPEN_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )  // Using HIF HD
    if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and open function for EDDx
	    LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_open_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID TCIP_EDD_CLOSE_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )  // Using HIF HD
    if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and close function for EDDx
	    LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_close_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_REQUEST_LOWER
LSA_VOID TCIP_EDD_REQUEST_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )  // Using HIF HD
	if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
        // post the message to HIF HD (EDDx is located on HD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    }
    else
    #endif
	{
	    // get the mailbox ID and request function for EDDx
	    LSA_UINT16      const mbx_id = psi_edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT const p_func = psi_edd_get_request_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT16                    length,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
    LSA_USER_ID_TYPE    null_usr_id;

    PSI_INIT_USER_ID_UNION(null_usr_id);

    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
        PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    PSI_ASSERT( pSys != LSA_NULL );

        switch (pSys->hd_runs_on_level_ld)
        {
            case PSI_HD_RUNS_ON_LEVEL_LD_NO:
            {
	            PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
                break;
            }
            case PSI_HD_RUNS_ON_LEVEL_LD_YES:
            {
	            psi_edd_alloc_nrt_rx_mem( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_TCIP );
                break;
            }
            default:
            {
		        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM(): hd_runs_on_level_ld(%u) is invalid",
                    pSys->hd_runs_on_level_ld );
                *lower_mem_ptr_ptr = LSA_NULL;
                PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
            }
        }
	}
    #endif
}
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM(
	LSA_UINT16                  * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
	    PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    PSI_ASSERT( pSys != LSA_NULL );

        switch (pSys->hd_runs_on_level_ld)
        {
            case PSI_HD_RUNS_ON_LEVEL_LD_NO:
            {
	    	    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
                break;
            }
            case PSI_HD_RUNS_ON_LEVEL_LD_YES:
            {
                psi_edd_free_nrt_rx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP );
                break;
            }
            default:
            {
		        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM(): hd_runs_on_level_ld(%u) is invalid",
                    pSys->hd_runs_on_level_ld );
                *((LSA_VOID_PTR_TYPE*)lower_mem_ptr) = LSA_NULL;
                PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
            }
        }
	}
    #endif
}
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT16                    length,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
    LSA_USER_ID_TYPE    null_usr_id;

    PSI_INIT_USER_ID_UNION(null_usr_id);

    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
        PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    PSI_ASSERT( pSys != LSA_NULL );

        switch (pSys->hd_runs_on_level_ld)
        {
            case PSI_HD_RUNS_ON_LEVEL_LD_NO:
            {
	            PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
                break;
            }
            case PSI_HD_RUNS_ON_LEVEL_LD_YES:
            {
	            psi_edd_alloc_nrt_tx_mem( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_TCIP );
                break;
            }
            default:
            {
		        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM(): hd_runs_on_level_ld(%u) is invalid",
                    pSys->hd_runs_on_level_ld );
                *lower_mem_ptr_ptr = LSA_NULL;
                PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
            }
        }
	}
    #endif
}
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM(
    LSA_UINT16                  * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr )
{
    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
	    PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    PSI_ASSERT( pSys != LSA_NULL );

        switch (pSys->hd_runs_on_level_ld)
        {
            case PSI_HD_RUNS_ON_LEVEL_LD_NO:
            {
	    	    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
                break;
            }
            case PSI_HD_RUNS_ON_LEVEL_LD_YES:
            {
                psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP );
                break;
            }
            default:
            {
		        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM(): hd_runs_on_level_ld(%u) is invalid",
                    pSys->hd_runs_on_level_ld );
                *((LSA_VOID_PTR_TYPE*)lower_mem_ptr) = LSA_NULL;
                PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
            }
        }
	}
    #endif
}
#endif

/*===========================================================================*/
/*     OpenBSD Functions                                                     */
/*===========================================================================*/
#if TCIP_CFG_OPEN_BSD

/*---------------------------------------------------------------------------*/
/*  Helper Functions for List of allocated memory                            */
/*---------------------------------------------------------------------------*/
#ifdef OBSD_MEMORY_TRACING
void obsd_memory_init( void )
{
	OBSD_ALLOCATED_MEM_LIST = 0;
}

void obsd_insert_node( struct obsd_mem_entry * * list_head_ptr, void * allocated )
{
	struct obsd_mem_entry * new_entry   = 0;

	PSI_OBSD_ALLOC_MEM( (LSA_VOID_PTR_TYPE *)&new_entry, sizeof(struct obsd_mem_entry) );
	
	new_entry->allocated = allocated;
	new_entry->next      = 0;

	if (*list_head_ptr == 0)
	{
		*list_head_ptr = new_entry;
	}
	else
	{
		new_entry->next = *list_head_ptr;
		*list_head_ptr = new_entry;
	}
}

void obsd_delete_node( struct obsd_mem_entry * * list_head_ptr, void * allocated )
{
	LSA_UINT16              retVal;
	struct obsd_mem_entry * curr_node = *list_head_ptr;
	struct obsd_mem_entry * prev_node = LSA_NULL;

	while (curr_node != 0)
	{
		if (curr_node->allocated == allocated)
		{
			if (curr_node == *list_head_ptr)
			{
				*list_head_ptr = curr_node->next;
			}
			else
			{
				prev_node->next = curr_node->next;
			}

            retVal = 0;
			PSI_OBSD_FREE_MEM( &retVal, curr_node );
			PSI_ASSERT( retVal == LSA_RET_OK );
			return;
		}
		else
		{
			prev_node = curr_node;
			curr_node = curr_node->next;
		}
	}
}

void obsd_delete_mem_ptr( void * allocated )
{
	obsd_delete_node( &OBSD_ALLOCATED_MEM_LIST, allocated );
}

void obsd_add_mem_ptr( void * allocated )
{
	obsd_insert_node( &OBSD_ALLOCATED_MEM_LIST, allocated );
}

void obsd_memory_deinit( void )
{
	struct obsd_mem_entry * curr_entry = OBSD_ALLOCATED_MEM_LIST;
	LSA_UINT16              retVal;

	if (curr_entry == 0) PSI_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No OBSD-Pointer left ;-)");

	while (curr_entry != 0)
	{
		void * allocated = curr_entry->allocated;
		PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "OBSD-Pointer left: 0x%x", allocated);
        retVal = 0;
		PSI_OBSD_FREE_MEM( &retVal, allocated );
		PSI_ASSERT( retVal == LSA_RET_OK );

		curr_entry = curr_entry->next;
		obsd_delete_node( &OBSD_ALLOCATED_MEM_LIST, allocated );
	}

	OBSD_ALLOCATED_MEM_LIST = 0;
}

#endif
/*---------------------------------------------------------------------------*/
/*  Startup/Shutdown                                                         */
/*---------------------------------------------------------------------------*/
void OSAL_Init( void )
{
	LSA_RESPONSE_TYPE   retVal;
    LSA_USER_ID_TYPE    null_usr_id;

    PSI_INIT_USER_ID_UNION(null_usr_id);

    /* Page Memory */
    PSI_ALLOC_LOCAL_MEM( &obsd_page_memory_pool_base_ptr, null_usr_id, OBSD_PAGE_MEMORY_POOL_SIZE, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM ); // allocate Page Memory from HEAP

    retVal = 0;
    PSI_CREATE_OBSD_MEM_POOL( &retVal, obsd_page_memory_pool_base_ptr, OBSD_PAGE_MEMORY_POOL_SIZE );
	PSI_ASSERT( retVal == LSA_RET_OK );

#ifdef OBSD_MEMORY_TRACING
	/* List of allocated memory */
	obsd_memory_init();
#endif
}

void OSAL_Deinit( void )
{
	LSA_RESPONSE_TYPE retVal;

#ifdef OBSD_MEMORY_TRACING
	/* List of allocated memory */
	obsd_memory_deinit();
#endif

	/* Page Memory */
    retVal = 0;
	PSI_DELETE_OBSD_MEM_POOL( &retVal );
	PSI_ASSERT( retVal == LSA_RET_OK );

    retVal = 0;
    PSI_FREE_LOCAL_MEM( &retVal, obsd_page_memory_pool_base_ptr, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM ); // free Page Memory from HEAP
    PSI_ASSERT( retVal == LSA_RET_OK );
    obsd_page_memory_pool_base_ptr = 0;
}

/*---------------------------------------------------------------------------*/
/*  Page Memory                                                              */
/*---------------------------------------------------------------------------*/

/* Allocate Page from Page Memory and store location in mlist */
void * OSAL_km_alloc( unsigned int Size )
{
    void * pPage = 0;

    #if (OBSD_PAGE_SIZE != 4096)
    #error "OBSD_PAGE_SIZE not 4K but TCIP_OBSD_ALLOC_PAGE_MEM is defined/documented to 4K alignment"
    #endif

	if ( (Size % OBSD_PAGE_SIZE) == 0 ) // km_alloc only supports allocation of multiple of OBSD_PAGE_SIZE
	{
		PSI_OBSD_ALLOC_MEM( &pPage, Size );
		if ( pPage == LSA_NULL)
		{
		    PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_WARN, "OSAL_km_alloc(): OpenBSD: No more page memory" );
        }
	}
	else
	{
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_ERROR, "OSAL_km_alloc(): OpenBSD: cannot alloc a Size that is not a multiple of OBSD_PAGE_SIZE" );
	}

	return pPage;
}

/* Free Page from Page Memory and delete location from mlist */
void OSAL_km_free( void * pMem, unsigned int Size )
{
	LSA_UINT16 retVal = 0;

	LSA_UNUSED_ARG( Size );

	PSI_OBSD_FREE_MEM( &retVal, pMem );
	PSI_ASSERT( retVal == LSA_RET_OK );
}

/*---------------------------------------------------------------------------*/
/*  Standard Memory                                                          */
/*---------------------------------------------------------------------------*/

void * OSAL_kern_malloc( unsigned long size, int type, int flags )
{
	TCIP_LOCAL_MEM_PTR_TYPE ptr = LSA_NULL;

	LSA_UNUSED_ARG( type );

	if ( size > 0xFFFF )
	{
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"OSAL_kern_malloc(): OpenBSD: invalid size > 0xFFFF" );
	    PSI_FATAL_COMP( LSA_COMP_ID_TCIP, PSI_MODULE_ID, 0 );
	}

    PSI_TCIP_ALLOC_MEM( (LSA_VOID_PTR_TYPE *)&ptr, (LSA_UINT32)size );

	if (ptr != LSA_NULL)
	{
		if (flags & ZERO_FLAG)
		{
			PSI_MEMSET( ptr, 0, size );
		}
#ifdef OBSD_MEMORY_TRACING
#if OBSD_MEMORY_TRACING_DETAILS != 0
	PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "OSAL_kern_malloc(): 0x%x", ptr);
#endif
	obsd_add_mem_ptr( ptr );
#endif
	}
	else
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_WARN, "OSAL_kern_malloc(): OpenBSD: No more memory" );
    }

	return ptr;
}

void OSAL_kern_free( void * addr, int type )
{
	LSA_UINT16 result = 0;

	LSA_UNUSED_ARG( type );

    PSI_TCIP_FREE_MEM( &result, addr );
	PSI_ASSERT( result == LSA_RET_OK );

#ifdef OBSD_MEMORY_TRACING
#if OBSD_MEMORY_TRACING_DETAILS != 0
	PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "OSAL_kern_free(): 0x%x", addr);
#endif
	obsd_delete_mem_ptr( addr );
#endif

}

#endif // TCIP_CFG_OPEN_BSD

#endif // PSI_CFG_USE_TCIP && PSI_CFG_USE_LD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
