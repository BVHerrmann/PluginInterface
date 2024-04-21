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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component PSI.                      */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32099
#define LTRC_ACT_MODUL_ID  32099
#include "eps_sys.h"
#include "eps_rtos.h"
//lint --e(537) Repeated include file - but if we remove it here, we get compile errors.
#include "psi_trc.h"
#include "eps_locks.h"
#include "eps_cp_mem.h"
#include "eps_cp_hw.h"
#include "eps_events.h"
#include "eps_isr.h"

#include "eps_plf.h"
#include "eps_app.h"    // EPS APP prototypes
#include "eps_tasks.h"
#if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
#include "eps_caching.h"
#endif

#include "eps_timer.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

#define EPS_IR_LOCK_MAGIC 0x5454

/*---------------------------------------------------------------------------*/

LSA_EXTERN LSA_INT eps_vsprintf( LSA_UINT8 * buffer, const char * format, va_list argptr );
#if (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
LSA_EXTERN volatile ertec_timers_t * adn_ertec_timers;
LSA_EXTERN uint32_t adn_ertec_system_timer_reload_value;
#endif

/*===========================================================================*/
/*                 Timer                                                     */
/*===========================================================================*/

#ifndef PSI_ALLOC_TIMER_TGROUP0
LSA_VOID PSI_ALLOC_TIMER_TGROUP0(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout )
{
    if( (time_base != LSA_TIME_BASE_100MS) && (time_base != LSA_TIME_BASE_1S) && (time_base != LSA_TIME_BASE_10S) && (time_base != LSA_TIME_BASE_100S) )
    {
        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR,
            "PSI_ALLOC_TIMER_TGROUP0 used with wrong TIME_BASE. TIME_BASE (%d)", time_base );

        *ret_val_ptr = LSA_RET_ERR_PARAM;
        return;
    }

   *ret_val_ptr = eps_timer_allocate_tgroup0( timer_id_ptr, timer_type, time_base, psi_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_ALLOC_TIMER_TGROUP1
LSA_VOID PSI_ALLOC_TIMER_TGROUP1(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout )
{
    if( (time_base != LSA_TIME_BASE_1MS) && (time_base != LSA_TIME_BASE_10MS) )
    {
        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR,
            "PSI_ALLOC_TIMER_TGROUP1 used with wrong TIME_BASE. TIME_BASE (%d)", time_base );
        *ret_val_ptr = LSA_RET_ERR_PARAM;
        return;
    }

   *ret_val_ptr = eps_timer_allocate_tgroup1( timer_id_ptr, timer_type, time_base, psi_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_START_TIMER
LSA_VOID PSI_START_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          timeVal )
{
	*ret_val_ptr = eps_timer_start( timer_id, user_id, timeVal );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STOP_TIMER
LSA_VOID PSI_STOP_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	*ret_val_ptr = eps_timer_stop( timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_TIMER
LSA_VOID PSI_FREE_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id )
{
	*ret_val_ptr = eps_timer_free( timer_id );
}
#else
#error "by design a function!"
#endif


/*----------------------------------------------------------------------------*/
/*  Ticks count                                                               */
/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_1MS
LSA_UINT32 PSI_GET_TICKS_1MS( LSA_VOID )
{
    return eps_get_ticks_1ms();
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_100NS
LSA_UINT64 PSI_GET_TICKS_100NS( LSA_VOID )
{
    return eps_get_ticks_100ns();
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                 Reentrance Locks                                          */
/*===========================================================================*/

#ifndef PSI_ALLOC_REENTRANCE_LOCK
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * lock_handle_ptr )
{
	*ret_val_ptr = eps_alloc_critical_section( lock_handle_ptr, LSA_FALSE );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_REENTRANCE_LOCK_PRIO_PROTECTED
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK_PRIO_PROTECTED(
    LSA_UINT16 * ret_val_ptr,
    LSA_UINT16 * lock_handle_ptr )
{
    *ret_val_ptr = eps_alloc_critical_section_prio_protected( lock_handle_ptr, LSA_FALSE );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_REENTRANCE_LOCK
LSA_VOID PSI_FREE_REENTRANCE_LOCK(
	 LSA_UINT16 * ret_val_ptr,
	 LSA_UINT16   lock_handle )
{
	*ret_val_ptr = eps_free_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_ENTER_REENTRANCE_LOCK
LSA_VOID PSI_ENTER_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	eps_enter_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXIT_REENTRANCE_LOCK
LSA_VOID PSI_EXIT_REENTRANCE_LOCK( LSA_UINT16 lock_handle )
{
	eps_exit_critical_section( lock_handle );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     IR Locks                                              */
/*===========================================================================*/

#ifndef PSI_ALLOC_IR_LOCK
LSA_VOID PSI_ALLOC_IR_LOCK(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16 * lock_handle_ptr )
{
    /* IR-Lock is currently only possible in Adonis, for other os we use prio ceiling mutex (thread lock) */
    #if defined(EPS_USE_RTOS_ADONIS)
	*lock_handle_ptr = EPS_IR_LOCK_MAGIC;
	*ret_val_ptr     = LSA_RET_OK;
    #else
    *ret_val_ptr = eps_alloc_critical_section_prio_ceiling( lock_handle_ptr, LSA_FALSE );
    #endif // (EPS_USE_RTOS_ADONIS)
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_IR_LOCK
LSA_VOID PSI_FREE_IR_LOCK(
	 LSA_UINT16 * ret_val_ptr,
	 LSA_UINT16   lock_handle )
{
    #if defined(EPS_USE_RTOS_ADONIS)
	EPS_ASSERT( lock_handle == EPS_IR_LOCK_MAGIC );
	*ret_val_ptr = LSA_RET_OK;
    #else
    PSI_FREE_REENTRANCE_LOCK(ret_val_ptr, lock_handle);
    #endif // (EPS_USE_RTOS_ADONIS)
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_ENTER_IR_LOCK
LSA_VOID PSI_ENTER_IR_LOCK( LSA_UINT16 lock_handle )
{
    /* IR-Lock is currently only possible in Adonis, for other os we use prio ceiling mutex (thread lock) */
    #if defined(EPS_USE_RTOS_ADONIS)
	EPS_ASSERT( lock_handle == EPS_IR_LOCK_MAGIC );
	EPS_PLF_DISABLE_INTERRUPTS();
    #else
    PSI_ENTER_REENTRANCE_LOCK(lock_handle);
    #endif // (EPS_USE_RTOS_ADONIS)
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXIT_IR_LOCK
LSA_VOID PSI_EXIT_IR_LOCK( LSA_UINT16 lock_handle )
{
    #if defined(EPS_USE_RTOS_ADONIS)
	EPS_ASSERT( lock_handle == EPS_IR_LOCK_MAGIC );
	EPS_PLF_ENABLE_INTERRUPTS();
    #else
    PSI_EXIT_REENTRANCE_LOCK(lock_handle);
    #endif // (EPS_USE_RTOS_ADONIS)
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
/* Blocking Waits                                                             */
/*----------------------------------------------------------------------------*/

typedef struct psi_blocking_wait_type
{
	LSA_UINT16  bw_handle;

} /*PSI_BLOCKING_WAIT_TYPE,*/ * PSI_BLOCKING_WAIT_PTR_TYPE;

#ifndef PSI_ALLOC_BLOCKING_WAIT
LSA_VOID PSI_ALLOC_BLOCKING_WAIT(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

	*ret_val_ptr = eps_alloc_event( &p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_FREE_BLOCKING_WAIT
LSA_VOID PSI_FREE_BLOCKING_WAIT(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

	*ret_val_ptr = eps_free_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SET_BLOCKING_WAIT
LSA_VOID PSI_SET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

	eps_reset_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_RESET_BLOCKING_WAIT
LSA_VOID PSI_RESET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

	eps_set_event( p_wait_ptr->bw_handle );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_WAIT_BLOCKING_WAIT
LSA_VOID PSI_WAIT_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type )
{
    LSA_RESPONSE_TYPE retVal;
	PSI_BLOCKING_WAIT_PTR_TYPE p_wait_ptr = (PSI_BLOCKING_WAIT_PTR_TYPE)bw_type;
	PSI_ASSERT( p_wait_ptr );

    retVal = eps_wait_event( p_wait_ptr->bw_handle, PSI_EVENT_NO_TIMEOUT );  // wait forever
    PSI_ASSERT( LSA_RET_OK == retVal );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_WAIT_NS
LSA_VOID PSI_WAIT_NS(
    LSA_UINT64 uTimeNs )
{
    #if (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
    if (uTimeNs <= (LSA_UINT64)500000) // special wait implementation till 500us
    {
        LSA_UINT32 const WaitTimeUs  = (((LSA_UINT32)uTimeNs + 900UL) / 1000UL) + 1UL; // assumption: 100ns software runtime
        LSA_UINT32 const StartTimeUs = adn_ertec_timers->tim0;
        LSA_UINT32       CurrentTimeUs;
        LSA_UINT32       TargetTimeUs;

        // This wait function is implemented for a reload value of 1000 and a time resolution of 1us.
        // With other values modifications are necessary!
	    PSI_ASSERT( 1000 == adn_ertec_system_timer_reload_value );

        // timer0:  runs backwards from 1000...0
        //          time resolution = 1us
        // example: requested wait time = 1500ns
        //          wait 3 timer ticks (* 1us)
        //          real wait time = 2000...2999ns (+ software runtime)

        if // overflow occurs during wait?
           (WaitTimeUs >= StartTimeUs)
        {
            TargetTimeUs = 1000UL - (WaitTimeUs - StartTimeUs);

            // active wait
            do
            {
                CurrentTimeUs = adn_ertec_timers->tim0;
            } while ((CurrentTimeUs <= StartTimeUs) || (CurrentTimeUs > TargetTimeUs));
        }
        else // no overflow occurs
        {
            TargetTimeUs = StartTimeUs - WaitTimeUs;

            // active wait
            do
            {
                CurrentTimeUs = adn_ertec_timers->tim0;
            } while (CurrentTimeUs > TargetTimeUs);
        }
    }
    else // standard wait implementation above 500us
    {
        eps_tasks_nswait( uTimeNs );
    }
    #else
    eps_tasks_nswait( uTimeNs );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
/* Cache                                                                      */
/*----------------------------------------------------------------------------*/

#if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )

#ifndef PSI_CACHE_SYNC
LSA_VOID PSI_CACHE_SYNC(
    LSA_VOID_PTR_TYPE   basePtr,
    LSA_UINT32          length )
{
    eps_cache_sync( basePtr, length );
	if(eps_cache_cmp_uncached(basePtr, length))
    {
        PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
            "PSI_CACHE_SYNC after eps_cache_sync eps_cache_cmp_uncached != 0 ptr(0x%08x), length(%u)", basePtr, length );
        PSI_FATAL(0);
    }
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CACHE_INV
LSA_VOID PSI_CACHE_INV(
    LSA_VOID_PTR_TYPE basePtr,
    LSA_UINT32        length )
{
    eps_cache_inv( basePtr, length );
	if(eps_cache_cmp_uncached(basePtr, length))
	{
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"PSI_CACHE_INV after eps_cache_inv eps_cache_cmp_uncached != 0 ptr(0x%08x), length(%u)", basePtr, length );
		PSI_FATAL(0);
	}
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CACHE_WB
LSA_VOID PSI_CACHE_WB(
    LSA_VOID_PTR_TYPE basePtr,
    LSA_UINT32        length )
{
    eps_cache_wb( basePtr, length );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CACHE_WB_INV
LSA_VOID PSI_CACHE_WB_INV(
    LSA_VOID_PTR_TYPE basePtr,
    LSA_UINT32        length )
{
    eps_cache_wb_inv( basePtr, length );
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_NRT_CACHE_SYNC

/*===========================================================================*/
/*                     DEV memory pool                                       */
/*===========================================================================*/
#if (EPS_PLF != EPS_PLF_SOC_MIPS)
#ifndef PSI_CREATE_DEV_MEM_POOL
LSA_VOID PSI_CREATE_DEV_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT           * pool_handle_ptr )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

   *pool_handle_ptr = eps_cp_mem_create_dev_pool( pSys->hd_nr, (LSA_UINT8 *)basePtr, length );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_DEV_MEM_POOL
LSA_VOID PSI_DELETE_DEV_MEM_POOL(
	LSA_UINT16     * ret_val_ptr,
	PSI_SYS_HANDLE   sys_handle,
	LSA_INT          pool_handle )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

    eps_cp_mem_delete_dev_pool( pSys->hd_nr, pool_handle );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DEV_ALLOC_MEM
LSA_VOID PSI_DEV_ALLOC_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_dev_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DEV_FREE_MEM
LSA_VOID PSI_DEV_FREE_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_dev_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif
#endif //(EPS_PLF != EPS_PLF_SOC_MIPS)
/*===========================================================================*/
/*                     NRT memory pool                                       */
/*===========================================================================*/

#ifndef PSI_CREATE_NRT_MEM_POOL
LSA_VOID PSI_CREATE_NRT_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT           * pool_handle_ptr,
    LSA_UINT8           cp_mem_nrt_type )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

   *pool_handle_ptr = eps_cp_mem_create_nrt_pool( pSys->hd_nr, (LSA_UINT8 *)basePtr, length, cp_mem_nrt_type );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_NRT_MEM_POOL
LSA_VOID PSI_DELETE_NRT_MEM_POOL(
	LSA_UINT16     * ret_val_ptr,
	PSI_SYS_HANDLE   sys_handle,
	LSA_INT          pool_handle,
    LSA_UINT8        cp_mem_nrt_type )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

    eps_cp_mem_delete_nrt_pool( pSys->hd_nr, pool_handle, cp_mem_nrt_type );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_ALLOC_TX_MEM
LSA_VOID PSI_NRT_ALLOC_TX_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_nrt_tx_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_FREE_TX_MEM
LSA_VOID PSI_NRT_FREE_TX_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_nrt_tx_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_ALLOC_RX_MEM
LSA_VOID PSI_NRT_ALLOC_RX_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_nrt_rx_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_NRT_FREE_RX_MEM
LSA_VOID PSI_NRT_FREE_RX_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_nrt_rx_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#if ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))
/*===========================================================================*/
/*                     PI memory pool                                        */
/*===========================================================================*/

#ifndef PSI_CREATE_PI_MEM_POOL

/** Systemadaption for creating mempools
 * 
 * Depending on the hardware device type and the hd_nr a special
 * memset function is set to p_memset_fct pointer. With this information
 * the mempool is created by calling eps_cp_mem_create_pi_pool.
 *  
 * @param basePtr Base address of memory block
 * @param length Size of memory block [bytes]
 * @param sys_handle EPS system information (EPS_SYS_PTR_TYPE)
 * @param pool_handle_ptr handle to adress allocated pool
 */
LSA_VOID PSI_CREATE_PI_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT           * pool_handle_ptr )
{
	LSA_UINT32                    gran_size;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_MEMSET_FCT                p_memset_fct = LSA_NULL;
	EPS_SYS_PTR_TYPE        const pSys         = (EPS_SYS_PTR_TYPE)sys_handle;

	PSI_ASSERT( pSys );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board );

	if ( p_board->edd_type == LSA_COMP_ID_EDDI )
	{
		gran_size = 0x08; // Default for KRAM

		if ( p_board->eddi.device_type == EDD_HW_TYPE_USED_SOC ) // SOC1 board ?
		{
			switch ( pSys->hd_nr )
			{
				case 1: 
				{
					// set special memset function
					p_memset_fct = eps_cp_mem_hd1_paea_memset;
				}
				break;
				case 2: 
				{
					// set special memset function
					p_memset_fct = eps_cp_mem_hd2_paea_memset;
				}
				break;
				case 3: 
				{
					// set special memset function
					p_memset_fct = eps_cp_mem_hd3_paea_memset;
				}
				break;
				case 4: 
				{
					// set special memset function
					p_memset_fct = eps_cp_mem_hd4_paea_memset;
				}
				break;
				default:
				{
					EPS_FATAL( "PSI_CREATE_PI_MEM_POOL(): Creating MEM_Pool: No supported HD-Number." );
				}
				break;
			}
		}
		else
		{
			// use standard memset function
			p_memset_fct = eps_memset;
		}
	}
	else
	{
		// use standard memset function
		p_memset_fct = eps_memset;
		gran_size    = 0x20;
	}

	*pool_handle_ptr = eps_cp_mem_create_pi_pool( pSys->hd_nr, (LSA_UINT8 *)basePtr, length, gran_size, p_memset_fct );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_PI_MEM_POOL
LSA_VOID PSI_DELETE_PI_MEM_POOL(
	LSA_UINT16     * ret_val_ptr,
	PSI_SYS_HANDLE   sys_handle,
	LSA_INT          pool_handle )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

	eps_cp_mem_delete_pi_pool( pSys->hd_nr, pool_handle );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_PI_ALLOC_MEM
LSA_VOID PSI_PI_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_BOOL            is_provider )
{
	*mem_ptr_ptr = eps_cp_mem_pi_alloc( length, is_provider, pool_handle, LSA_COMP_ID_IOH );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_PI_FREE_MEM
LSA_VOID PSI_PI_FREE_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle )
{
	eps_cp_mem_pi_free( mem_ptr, pool_handle, LSA_COMP_ID_IOH );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif
#endif //(PSI_CFG_USE_IOH == 1)  && (PSI_CFG_USE_LD_COMP == 1)

/*----------------------------------------------------------------------------*/
/* TCIP memory pool                                                           */
/*----------------------------------------------------------------------------*/

#if (( PSI_CFG_USE_TCIP == 1 ) && (PSI_CFG_USE_LD_COMP == 1))

#ifndef PSI_CREATE_TCIP_MEM_POOL
LSA_VOID PSI_CREATE_TCIP_MEM_POOL(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length )
{
    eps_cp_mem_create_tcip_pool( (LSA_UINT8 *)basePtr, length );

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_TCIP_MEM_POOL
LSA_VOID PSI_DELETE_TCIP_MEM_POOL(
	LSA_UINT16 * ret_val_ptr )
{
    eps_cp_mem_delete_tcip_pool();

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_TCIP_ALLOC_MEM
LSA_VOID PSI_TCIP_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length )
{
	*mem_ptr_ptr = eps_cp_mem_tcip_alloc( length );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_TCIP_FREE_MEM
LSA_VOID PSI_TCIP_FREE_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr )
{
	eps_cp_mem_tcip_free( (LSA_UINT8 *)mem_ptr );

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_TCIP && PSI_CFG_USE_LD_COMP

/*===========================================================================*/
/* OpenBSD Page memory pool                                                  */
/*===========================================================================*/

#if ( PSI_CFG_TCIP_STACK_OPEN_BSD == 1 )

#ifndef PSI_CREATE_OBSD_MEM_POOL
LSA_VOID PSI_CREATE_OBSD_MEM_POOL(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length )
{
	eps_cp_mem_create_obsd_pool( (LSA_UINT8 *)basePtr, length );

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_OBSD_MEM_POOL
LSA_VOID PSI_DELETE_OBSD_MEM_POOL(
	LSA_UINT16 * ret_val_ptr )
{
	eps_cp_mem_delete_obsd_pool();

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_OBSD_ALLOC_MEM
LSA_VOID PSI_OBSD_ALLOC_MEM(
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length )
{
	*mem_ptr_ptr = eps_cp_mem_obsd_alloc( length );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_OBSD_FREE_MEM
LSA_VOID PSI_OBSD_FREE_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr )
{
	eps_cp_mem_obsd_free( (LSA_UINT8 *)mem_ptr );

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_TCIP_STACK_OPEN_BSD

/*===========================================================================*/
/*                    stdlib                                                 */
/*===========================================================================*/

#ifndef PSI_MEMSET
LSA_VOID PSI_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT           val,
	LSA_UINT          len )
{
	eps_memset( ptr, val, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMCPY
LSA_VOID PSI_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	const LSA_VOID_PTR_TYPE src,
	LSA_UINT          len )
{
	eps_memcpy( dst, src, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMCMP
LSA_INT PSI_MEMCMP(
	const LSA_VOID_PTR_TYPE pBuf1,
	const LSA_VOID_PTR_TYPE pBuf2,
	LSA_UINT          Length )
{
	return ( eps_memcmp( pBuf1, pBuf2, (LSA_UINT)Length ) );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_MEMMOVE
LSA_VOID PSI_MEMMOVE(
	LSA_VOID_PTR_TYPE dst,
	const LSA_VOID_PTR_TYPE src,
	LSA_UINT          len )
{
	eps_memmove( dst, src, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STRCPY
LSA_VOID PSI_STRCPY(
	LSA_VOID_PTR_TYPE dst,
	const LSA_VOID_PTR_TYPE src )
{
	eps_strcpy( dst, src );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_STRLEN
LSA_UINT PSI_STRLEN(
	const LSA_VOID_PTR_TYPE str )
{
	return ( eps_strlen( str ) );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SPRINTF
LSA_INT PSI_SPRINTF(
	LSA_UINT8 * str,
	LSA_CHAR  * fmt,
	... )
{
	LSA_INT ret;
	va_list argptr;

    va_start( argptr, fmt );
	ret = eps_vsprintf( str, fmt, argptr );
    //lint --e(10) --e(534) --e(40) Undeclared identifier '__crt_va_end' - Doing an include of <vadefs.h> and <stdarg.h> doesn't help
	va_end( argptr );

	return ret;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_SSCANF_UUID
LSA_INT PSI_SSCANF_UUID(
	const LSA_UINT8  * uuid_string,
	const LSA_CHAR   * fmt,
	LSA_UINT32 * time_low,
	LSA_INT    * time_mid,
	LSA_INT    * time_hi_and_version,
	LSA_INT    * clock_seq_hi_and_reserved,
	LSA_INT    * clock_seq_low,
	LSA_INT    * node0,
	LSA_INT    * node1,
	LSA_INT    * node2,
	LSA_INT    * node3,
	LSA_INT    * node4,
	LSA_INT    * node5,
	LSA_INT    * read_count )
{
	LSA_INT ret;

	ret = sscanf( (const char *)uuid_string, fmt,
		time_low,
		time_mid,
		time_hi_and_version,
		clock_seq_hi_and_reserved,
		clock_seq_low,
		node0, node1, node2, node3, node4, node5,
		read_count );

	return ret;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_LOG_10
LSA_VOID PSI_LOG_10(
	LSA_UINT32 const Arg_in_ppm,
	LSA_INT32      * pResult_in_ppm )
{
	// coding from docu
	double LogOut;

	LogOut = log10( (double)Arg_in_ppm );

	LogOut *= 1000000.0; /* ppm, parts per million */

	*pResult_in_ppm = (LSA_INT32)LogOut;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_POW_10
LSA_VOID PSI_POW_10(
	LSA_INT16  const numerator,
	LSA_UINT16 const denominator,
	LSA_UINT32     * pResult )
{
	// coding from docu
	*pResult = (LSA_UINT32)pow(10.0, (double)numerator/(double)denominator );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EXCHANGE_LONG
long PSI_EXCHANGE_LONG(
	long volatile * long_ptr,
	long            val )
{
	return ( EPS_PLF_EXCHANGE_LONG( long_ptr, val ) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   FATAL / ERROR                                                           */
/*===========================================================================*/

#ifndef PSI_FATAL_ERROR
LSA_VOID PSI_FATAL_ERROR(
	LSA_CHAR                 * comp,
	LSA_UINT16                 length,
	PSI_FATAL_ERROR_PTR_TYPE   error_ptr )
{
    EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_LSA_FATAL, EPS_EXIT_CODE_LSA_FATAL, comp, length, error_ptr, 0, 0, 0, 0);
}
#else
#error "by design a function!"
#endif

#ifndef PSI_RQB_ERROR
LSA_VOID PSI_RQB_ERROR(
	LSA_UINT16        comp_id,
	LSA_UINT16        comp_id_lower,
	LSA_VOID_PTR_TYPE rqb_ptr )
{
	PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_FATAL,
		"RQB_ERROR occured, compId(%u/%#x) compId-Lower(%u/%#x) ptr(0x%08x)",
		comp_id, comp_id,
		comp_id_lower, comp_id_lower,
		rqb_ptr );

	EPS_FATAL( "RQB_ERROR occured" );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_FATAL_ERROR_OCCURED
LSA_BOOL PSI_FATAL_ERROR_OCCURED( LSA_VOID )
{
	EPS_ASSERT( g_pEpsData != LSA_NULL );

	return ( g_pEpsData->bEpsInFatal );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   HW param adaption                                                       */
/*===========================================================================*/

#ifndef PSI_GET_HD_PARAM
LSA_VOID PSI_GET_HD_PARAM(
	LSA_UINT16             *        ret_val_ptr,
    PSI_HD_INPUT_CONST_PTR_TYPE     hd_ptr,
	PSI_HD_SYS_ID_CONST_PTR_TYPE    hd_sys_id_ptr,
	PSI_HD_PARAM_PTR_TYPE           hd_param_ptr )
{
    PSI_ASSERT( hd_sys_id_ptr != LSA_NULL );
	PSI_ASSERT( hd_param_ptr  != LSA_NULL );

    // check and getting information for HD over core system (using driver)
	eps_hw_get_hd_params( hd_ptr->hd_id, hd_ptr->hd_runs_on_level_ld, hd_sys_id_ptr, hd_param_ptr );

	*ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_LD_GET_HD_PARAM
LSA_VOID PSI_LD_GET_HD_PARAM(
    LSA_UINT16             *        ret_val_ptr,
    LSA_UINT16                      hd_id,
    PSI_HD_PARAM_PTR_TYPE           hd_param_ptr)
{
    PSI_ASSERT(hd_param_ptr != LSA_NULL);

    // get stored information for HD
    eps_hw_set_hd_param(hd_id, hd_param_ptr);

    *ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_FREE_HD_PARAM
LSA_VOID PSI_FREE_HD_PARAM(
	LSA_UINT16 * ret_val_ptr,
	LSA_UINT16   hd_id )
{
	// check and getting information for HD over core system (using driver)
	eps_hw_free_hd_params( hd_id );

	*ret_val_ptr = PSI_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_LD_CLOSED_HD
LSA_VOID PSI_LD_CLOSED_HD(
	LSA_UINT16 hd_id )
{
	eps_hw_close_hd( hd_id );	
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*   PNIO IR adaption                                                        */
/*===========================================================================*/
#if (PSI_CFG_USE_HD_COMP == 1) 

#ifndef PSI_HD_ENABLE_EVENT
LSA_VOID PSI_HD_ENABLE_EVENT(
	PSI_SYS_HANDLE sys_handle )
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)sys_handle;
    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "PSI_HD_ENABLE_EVENT() - enable events for hd_id = %d", pSys->hd_nr );
	eps_enable_pnio_event( sys_handle );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_HD_DISABLE_EVENT
LSA_VOID PSI_HD_DISABLE_EVENT(
	PSI_SYS_HANDLE sys_handle )
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)sys_handle;
    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "PSI_HD_DISABLE_EVENT() - disable events for hd_id = %d", pSys->hd_nr );
	eps_disable_pnio_event( sys_handle );
}
#else
#error "by design a function!"
#endif

#endif // (PSI_CFG_USE_HD_COMP == 1) 

/*===========================================================================*/

#ifndef PSI_THREAD_READY
LSA_VOID PSI_THREAD_READY( LSA_VOID_PTR_TYPE arg )
{
	eps_tasks_signal_psi_thread_ready( arg );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_THREAD_STOPPED
LSA_VOID PSI_THREAD_STOPPED( LSA_VOID_PTR_TYPE arg )
{
	eps_tasks_signal_psi_thread_stopped( arg );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*                     CRT slow memory pool                                  */
/*===========================================================================*/
#if ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))

#ifndef PSI_CREATE_CRT_SLOW_MEM_POOL
LSA_VOID PSI_CREATE_CRT_SLOW_MEM_POOL(
	LSA_VOID_PTR_TYPE   basePtr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT           * pool_handle_ptr,
    LSA_UINT8           cp_mem_crt_slow_type )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

   *pool_handle_ptr = eps_cp_mem_create_crt_slow_pool( pSys->hd_nr, (LSA_UINT8 *)basePtr, length, cp_mem_crt_slow_type );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_DELETE_CRT_SLOW_MEM_POOL
LSA_VOID PSI_DELETE_CRT_SLOW_MEM_POOL(
	LSA_UINT16     * ret_val_ptr,
	PSI_SYS_HANDLE   sys_handle,
	LSA_INT          pool_handle,
    LSA_UINT8        cp_mem_crt_slow_type )
{
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_handle;
	PSI_ASSERT( pSys );

    eps_cp_mem_delete_crt_slow_pool( pSys->hd_nr, pool_handle, cp_mem_crt_slow_type );
	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_ALLOC_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_crt_slow_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_ALLOC_TX_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_TX_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_crt_slow_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_ALLOC_RX_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_RX_MEM(
    LSA_VOID_PTR_TYPE * mem_ptr_ptr,
    LSA_UINT32          length,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    *mem_ptr_ptr = eps_cp_mem_crt_slow_alloc( length, pool_handle, comp_id );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_FREE_MEM
LSA_VOID PSI_CRT_SLOW_FREE_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_crt_slow_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_FREE_TX_MEM
LSA_VOID PSI_CRT_SLOW_FREE_TX_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_crt_slow_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_CRT_SLOW_FREE_RX_MEM
LSA_VOID PSI_CRT_SLOW_FREE_RX_MEM(
    LSA_UINT16        * ret_val_ptr,
    LSA_VOID_PTR_TYPE   mem_ptr,
    LSA_INT             pool_handle,
    LSA_UINT16          comp_id )
{
    eps_cp_mem_crt_slow_free( mem_ptr, pool_handle, comp_id );
    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif
#endif //(PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
