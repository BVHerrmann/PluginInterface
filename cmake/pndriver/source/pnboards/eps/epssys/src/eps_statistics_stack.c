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
/*  F i l e               &F: eps_statistics_stack.c                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS internal Statistics API for stack usage.                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20100
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include "eps_sys.h"
#if ( EPS_CFG_USE_STATISTICS == 1 )

#include "eps_trc.h"
#include "eps_locks.h"
#include "eps_statistics_stack.h"
#include "eps_tasks.h"

//#include "eps_posix_cfg_windows.h"
#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*-----------------------------------------------------------------*/
/* Defines                                                         */
/*-----------------------------------------------------------------*/
#define EPS_STATISTICS_STACK_TRACE_LVL                    LSA_TRACE_LEVEL_NOTE

#define EPS_STATISTICS_STACK_MAX_STATISTICS               20

#ifdef ADN_SYS_STACK_CHECK_MAGIC
#define MAGIC_PATTERN                                     ADN_SYS_STACK_CHECK_MAGIC
#else
#define MAGIC_PATTERN                                     0xdeadbeef
#endif

#define EPS_STATISTICS_STACK_GEAR_REDUCTION               20 // Stack-Statistic is only written in time = (Start-POLL-Thread-Time * this define)

/*-----------------------------------------------------------------*/
/* Types                                                           */
/*-----------------------------------------------------------------*/

typedef struct
{
    LSA_BOOL                is_used;
    LSA_CHAR                pName[25];
    LSA_UINT32              stack_start_address;
    LSA_UINT32              stack_size;
    LSA_UINT32              stack_max_used;
}EPS_STATISTICS_STACK_STATS_TYPE; 

typedef struct  
{ 
    LSA_BOOL                            is_running;                           
    LSA_UINT16                          enter_exit;                           
    EPS_STATISTICS_STACK_STATS_TYPE     stats[EPS_STATISTICS_STACK_MAX_STATISTICS]; 
    LSA_UINT32                          EpsStatisticsStackGearReduction;
    LSA_UINT32                          hThread;
    LSA_BOOL                            hThread_running;

} EPS_STATISTICS_STACK_INSTANCE_TYPE;

/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/
static EPS_STATISTICS_STACK_INSTANCE_TYPE g_Eps_statistics_stack = {0};

static LSA_VOID eps_statistics_stack_print_stats_short ( LSA_VOID );
//******************************************************************************
//  Function: eps_stastistics_stack_init
//******************************************************************************
LSA_VOID eps_statistics_stack_init ( LSA_VOID )
{
    LSA_UINT16        entry;
    LSA_RESPONSE_TYPE retVal;

    EPS_ASSERT( !g_Eps_statistics_stack.is_running );

    g_Eps_statistics_stack.EpsStatisticsStackGearReduction = 0;

    // alloc the critical section
    retVal = eps_alloc_critical_section(&g_Eps_statistics_stack.enter_exit, LSA_FALSE);
    EPS_ASSERT( retVal == LSA_RET_OK );

    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    {
        // set default values
        eps_memset(&g_Eps_statistics_stack.stats[entry], 0, sizeof(EPS_STATISTICS_STACK_STATS_TYPE));
    }

    g_Eps_statistics_stack.hThread_running = LSA_FALSE;

    g_Eps_statistics_stack.is_running = LSA_TRUE; // Now initialized   
}

//******************************************************************************
//  Function: eps_statistics_stack_undo_init
//******************************************************************************
LSA_VOID eps_statistics_stack_undo_init ( LSA_VOID )
{
    LSA_UINT16 entry;
    LSA_UINT16 retVal;

    EPS_ASSERT( g_Eps_statistics_stack.is_running );

    g_Eps_statistics_stack.EpsStatisticsStackGearReduction = 0;

    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    {
        // check all msgq are freed 
        EPS_ASSERT( !g_Eps_statistics_stack.stats[entry].is_used );
    }

    g_Eps_statistics_stack.is_running = LSA_FALSE;

    retVal = eps_free_critical_section(g_Eps_statistics_stack.enter_exit); 
    EPS_ASSERT(LSA_RET_OK == retVal);
}

//******************************************************************************
//  Function: eps_statistics_stack_alloc_stats
//******************************************************************************
LSA_UINT32 eps_statistics_stack_register ( const LSA_CHAR * pName, LSA_UINT32 stackSize, LSA_UINT8* startAddress )
{    
    LSA_UINT16        entry;    
    LSA_UINT32        i;
    LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;   
    LSA_UINT32        stackEndAddr;

    EPS_ASSERT( g_Eps_statistics_stack.is_running );

    eps_enter_critical_section(g_Eps_statistics_stack.enter_exit);

    // get the next available internal message queue structure
    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    {    
        // prepare if not used
        if ( !g_Eps_statistics_stack.stats[entry].is_used )
        {
            for (i=0; (i<21) && (pName[i] != 0); i++)
            {
                g_Eps_statistics_stack.stats[entry].pName[i] = pName[i];
            }
            for(;i<25;i++)
            {
                g_Eps_statistics_stack.stats[entry].pName[i] = 0;
            }
            
            g_Eps_statistics_stack.stats[entry].stack_size = stackSize;
            g_Eps_statistics_stack.stats[entry].stack_start_address = (LSA_UINT32)startAddress;
            g_Eps_statistics_stack.stats[entry].stack_max_used = 0;
      
            g_Eps_statistics_stack.stats[entry].is_used = LSA_TRUE;            
                       
            retVal = LSA_RET_OK;
            break;
        }
    }

    EPS_ASSERT(entry != EPS_STATISTICS_STACK_MAX_STATISTICS);

    // prepare the stack with magic patterns 
    // (first and last four bytes are not padded, because adonis uses magic_pattern to notice stack overrun, too)
    stackEndAddr = g_Eps_statistics_stack.stats[entry].stack_start_address + g_Eps_statistics_stack.stats[entry].stack_size - 4;

    EPS_ASSERT(g_Eps_statistics_stack.stats[entry].stack_start_address != 0);
    EPS_ASSERT(g_Eps_statistics_stack.stats[entry].stack_size != 0);
    EPS_ASSERT(stackEndAddr > g_Eps_statistics_stack.stats[entry].stack_start_address);

    for(i = g_Eps_statistics_stack.stats[entry].stack_start_address + 4; i < stackEndAddr - 4; i+=4 )
    {
        *(LSA_UINT32*)i = MAGIC_PATTERN;
    }

    EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_NOTE, "eps statistics stack register (%s)", g_Eps_statistics_stack.stats[entry].pName );

    eps_exit_critical_section(g_Eps_statistics_stack.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_statistics_stack_free_stats
//******************************************************************************
LSA_UINT32 eps_statistics_stack_undo_register ( LSA_CHAR * pName, LSA_UINT8* startAddress )
{    
    LSA_RESPONSE_TYPE   retVal = LSA_RET_OK;
    LSA_UINT16          entry; 

    EPS_ASSERT( g_Eps_statistics_stack.is_running );

    eps_enter_critical_section(g_Eps_statistics_stack.enter_exit);

    // find correct entry
    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    {  
        if (( g_Eps_statistics_stack.stats[entry].is_used == LSA_TRUE ) &&
            (!eps_strcmp(pName, g_Eps_statistics_stack.stats[entry].pName)) &&
            (g_Eps_statistics_stack.stats[entry].stack_start_address == (LSA_UINT32)startAddress))
        {
            break;
        }
    }

    if ( entry < EPS_STATISTICS_STACK_MAX_STATISTICS )          // in range ?
    {
        EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_NOTE, "statistics stack undo register (%s) complete", g_Eps_statistics_stack.stats[entry].pName );

        eps_memset(&g_Eps_statistics_stack.stats[entry], 0, sizeof(EPS_STATISTICS_STACK_STATS_TYPE));    
        
        retVal = LSA_RET_OK;       
    }
    else
    {
        EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_WARN, "Thread (%s) not found for undo register", pName );

        retVal = LSA_RET_ERR_PARAM;
    }

    eps_exit_critical_section(g_Eps_statistics_stack.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_statistics_stack_capture_all
//******************************************************************************
static LSA_VOID eps_statistics_stack_capture_all( LSA_VOID )
{
    LSA_UINT16        entry;    
    LSA_UINT32        i;
    LSA_UINT32        stackEndAddr;

    EPS_ASSERT( g_Eps_statistics_stack.is_running );

    eps_enter_critical_section(g_Eps_statistics_stack.enter_exit);

    // get the next available internal message queue structure
    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    {    
        // if message queue is used
        if ( g_Eps_statistics_stack.stats[entry].is_used )
        {
            // calc stack end address
            stackEndAddr = g_Eps_statistics_stack.stats[entry].stack_start_address + g_Eps_statistics_stack.stats[entry].stack_size - 4;
            
            for(i = g_Eps_statistics_stack.stats[entry].stack_start_address + 4; i < stackEndAddr - 4; i+=4 )
            {
                if(*(LSA_UINT32*)i != MAGIC_PATTERN)
                {
                    g_Eps_statistics_stack.stats[entry].stack_max_used = (stackEndAddr + 8) - i;
                    break;
                }
            }

            if( !(g_Eps_statistics_stack.stats[entry].stack_max_used < g_Eps_statistics_stack.stats[entry].stack_size) )
            {
            	EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_FATAL, "Stack Overflow - Thread (%s):", g_Eps_statistics_stack.stats[entry].pName );

            	EPS_ASSERT( g_Eps_statistics_stack.stats[entry].stack_max_used < g_Eps_statistics_stack.stats[entry].stack_size );
            }
        }

    }
    
    eps_exit_critical_section(g_Eps_statistics_stack.enter_exit);
}

static LSA_VOID eps_statistics_stack_thread( LSA_UINT32 uParam, void *arg )
{
    LSA_UNUSED_ARG(uParam);
    LSA_UNUSED_ARG(arg);

    g_Eps_statistics_stack.EpsStatisticsStackGearReduction++;

    if ((g_Eps_statistics_stack.EpsStatisticsStackGearReduction % EPS_STATISTICS_STACK_GEAR_REDUCTION) == 0)
    {
        // log values
        eps_statistics_stack_capture_all();

        // trace values
        eps_statistics_stack_print_stats_short();
    }
}

//******************************************************************************
//  Function: eps_statistics_stack_start_cyclic
//******************************************************************************
LSA_VOID eps_statistics_stack_cyclic_start( LSA_UINT32 cycle_time_ms )
{
    g_Eps_statistics_stack.hThread = eps_tasks_start_poll_thread("EPS_STAT_STACK", EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedRR, cycle_time_ms /*ms*/, eps_statistics_stack_thread, 0, (LSA_VOID*)NULL, eRUN_ON_1ST_CORE ); 
	EPS_ASSERT(g_Eps_statistics_stack.hThread != 0);
	g_Eps_statistics_stack.hThread_running = LSA_TRUE;
}

//******************************************************************************
//  Function: eps_statistics_stack_stop_cyclic
//******************************************************************************
LSA_VOID eps_statistics_stack_cyclic_stop( LSA_VOID )
{
    if(g_Eps_statistics_stack.hThread_running == LSA_TRUE)
	    eps_tasks_stop_poll_thread(g_Eps_statistics_stack.hThread); 
    g_Eps_statistics_stack.hThread_running = LSA_FALSE;
}

//******************************************************************************
//  Function: eps_statistics_stack_print_stats_short
//******************************************************************************
static LSA_VOID eps_statistics_stack_print_stats_short ( LSA_VOID )
{   
    LSA_UINT32 entry;
    LSA_UINT32 percentF;
    LSA_UINT32 percentC;
    float percent = 0;

    EPS_ASSERT( g_Eps_statistics_stack.is_running );
    
    eps_enter_critical_section(g_Eps_statistics_stack.enter_exit);

     // get the next available internal message queue structure
    for ( entry = 0; entry < EPS_STATISTICS_STACK_MAX_STATISTICS; entry++ )
    { 
        // if message queue is used
        if ( g_Eps_statistics_stack.stats[entry].is_used )
        {
            EPS_SYSTEM_TRACE_STRING( 0, EPS_STATISTICS_STACK_TRACE_LVL, "Stack Usage - Thread (%s):", g_Eps_statistics_stack.stats[entry].pName );
            EPS_SYSTEM_TRACE_00( 0, EPS_STATISTICS_STACK_TRACE_LVL, "_    Size       Used max      Free      Used (Precentage)");
            
            percent = (float)((float)g_Eps_statistics_stack.stats[entry].stack_max_used*100.0/(float)g_Eps_statistics_stack.stats[entry].stack_size);
            percentF = (LSA_UINT32)percent;
            percentC = (LSA_UINT32)((percent - (float)percentF) *10);

            EPS_SYSTEM_TRACE_05( 0, EPS_STATISTICS_STACK_TRACE_LVL, "%10d%10d%15d%6d.%01d", 
                g_Eps_statistics_stack.stats[entry].stack_size, 
                g_Eps_statistics_stack.stats[entry].stack_max_used, 
                g_Eps_statistics_stack.stats[entry].stack_size - g_Eps_statistics_stack.stats[entry].stack_max_used, 
                percentF, percentC);
        }
    }

    eps_exit_critical_section(g_Eps_statistics_stack.enter_exit);
}

#endif // ( EPS_CFG_USE_STATISTICS == 1 )
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
