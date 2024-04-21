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
/*  F i l e               &F: eps_statistics.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Statistics API                                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20099
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */
#include "eps_sys.h" // Definition of EPS_CFG_USE_STATISTICS and other headers
#if ( EPS_CFG_USE_STATISTICS == 1 )

#include "eps_trc.h"
#include "eps_locks.h"
#include "eps_statistics.h"
#include "eps_tasks.h"

#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
#include "pntrc_cfg.h"
#endif

//#include "eps_posix_cfg_windows.h"
#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*-----------------------------------------------------------------*/
/* Defines                                                         */
/*-----------------------------------------------------------------*/
#define EPS_STATISTICS_TRACE_LVL                    LSA_TRACE_LEVEL_NOTE

#define EPS_STATISTICS_MAX_STATISTICS               10
#define EPS_STATISTICS_MAX_VALUES_PER_STATISTIC     1000
#define EPS_STATISTICS_DISPERSION_FACTOR            10

#define EPS_STATISTICS_TRACE_GEAR_REDUCTION         10 // Statistic is only written in time = (Start-POLL-Thread-Time * this define)

/*-----------------------------------------------------------------*/
/* Types                                                           */
/*-----------------------------------------------------------------*/

typedef struct
{
    LSA_BOOL                is_used;
    LSA_UINT16              stats_id;
    LSA_CHAR                pName[25];
    LSA_UINT32              val_start[EPS_STATISTICS_MAX_VALUES_PER_STATISTIC];
    LSA_UINT32*             val_start_next_val_ptr;
    LSA_UINT32              val_start_count;
    LSA_UINT32              val_end[EPS_STATISTICS_MAX_VALUES_PER_STATISTIC];
    LSA_UINT32*             val_end_next_val_ptr;
    LSA_UINT32              val_end_count;
    LSA_UINT32              val_diffs[EPS_STATISTICS_MAX_VALUES_PER_STATISTIC];
    LSA_BOOL                buffer_filled;
}EPS_STATISTICS_STATS_TYPE; 

typedef struct  
{ 
    LSA_BOOL                        is_running;                           
    LSA_UINT16                      enter_exit;                           
    EPS_STATISTICS_STATS_TYPE       stats[EPS_STATISTICS_MAX_STATISTICS];  
    LSA_UINT32                      EpsStatisticsTraceGearReduction;
    LSA_UINT32                      hThread;
    LSA_BOOL                        hThread_running;
    struct timespec                 stats_time;
} EPS_STATISTICS_INSTANCE_TYPE;

/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/
static EPS_STATISTICS_INSTANCE_TYPE g_Eps_statistics = {0};

//******************************************************************************
//  Function: eps_statistics_calc_diffs
//******************************************************************************
static LSA_VOID eps_statistics_calc_diffs ( LSA_UINT32 stats_id )
{    
    LSA_UINT32 i;
    LSA_UINT32 end;
    LSA_UINT32 startValMod;
    LSA_UINT32 endValMod;
    EPS_ASSERT(stats_id < EPS_STATISTICS_MAX_STATISTICS);

    if(g_Eps_statistics.stats[stats_id].buffer_filled)
    {
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    }
    else
    {
        end = g_Eps_statistics.stats[stats_id].val_end_count;
    }

    if(g_Eps_statistics.stats[stats_id].val_start_count != 0)
    {
    	startValMod = g_Eps_statistics.stats[stats_id].val_start_count - 1;
    }
    else
    {
    	startValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }
    if(g_Eps_statistics.stats[stats_id].val_end_count != 0)
    {
    	endValMod = g_Eps_statistics.stats[stats_id].val_end_count - 1;
    }
    else
    {
    	endValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }

    for(i = 0; i < end; i++)
    {
        // skip the value which is not ready captured
        if ( !( (startValMod == i) && !(endValMod == i) ) )
        {
            g_Eps_statistics.stats[stats_id].val_diffs[i] = g_Eps_statistics.stats[stats_id].val_end[i] - g_Eps_statistics.stats[stats_id].val_start[i];
        }
    }
}

//******************************************************************************
//  Function: eps_statistics_get_min
//******************************************************************************
static LSA_UINT32 eps_statistics_get_min ( LSA_UINT32 stats_id )
{    
    LSA_UINT32 i;
    LSA_UINT32 min = 0xFFFFFFFF;
    LSA_UINT32 end;
    LSA_UINT32 startValMod;
    LSA_UINT32 endValMod;

    if(g_Eps_statistics.stats[stats_id].buffer_filled)
    {
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    }
    else
    {
        end = g_Eps_statistics.stats[stats_id].val_end_count;
    }

    if(g_Eps_statistics.stats[stats_id].val_start_count != 0)
    {
    	startValMod = g_Eps_statistics.stats[stats_id].val_start_count - 1;
    }
    else
    {
    	startValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }
    if(g_Eps_statistics.stats[stats_id].val_end_count != 0)
    {
    	endValMod = g_Eps_statistics.stats[stats_id].val_end_count - 1;
    }
    else
    {
    	endValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }

    for(i = 0; i < end; i++)
    {
        // skip the value which is not ready captured
    	if ( !( (startValMod == i) && !(endValMod == i) ) )
        {
            if( g_Eps_statistics.stats[stats_id].val_diffs[i] < min)
            {
                min = g_Eps_statistics.stats[stats_id].val_diffs[i];
            }
        }
    }

    return min;
}

//******************************************************************************
//  Function: eps_statistics_get_max
//******************************************************************************
static LSA_UINT32 eps_statistics_get_max ( LSA_UINT32 stats_id )
{    
    LSA_UINT32 i;
    LSA_UINT32 max = 0;
    LSA_UINT32 end;
    LSA_UINT32 startValMod;
    LSA_UINT32 endValMod;
    
    if(g_Eps_statistics.stats[stats_id].buffer_filled)
    {
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    }
    else
    {
        end = g_Eps_statistics.stats[stats_id].val_end_count;
    }

    if(g_Eps_statistics.stats[stats_id].val_start_count != 0)
    {
    	startValMod = g_Eps_statistics.stats[stats_id].val_start_count - 1;
    }
    else
    {
    	startValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }
    if(g_Eps_statistics.stats[stats_id].val_end_count != 0)
    {
    	endValMod = g_Eps_statistics.stats[stats_id].val_end_count - 1;
    }
    else
    {
    	endValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }

    for(i = 0; i < end; i++)
    {
        // skip the value which is not ready captured
    	if ( !( (startValMod == i) && !(endValMod == i) ) )
        {
            if( g_Eps_statistics.stats[stats_id].val_diffs[i] > max)
            {
                max = g_Eps_statistics.stats[stats_id].val_diffs[i];
            }
        }
    }

    return max;
}

//******************************************************************************
//  Function: eps_statistics_get_avg
//******************************************************************************
static LSA_UINT32 eps_statistics_get_avg ( LSA_UINT32 stats_id )
{    
    LSA_UINT32 i;
    LSA_UINT32 avg = 0;
    LSA_UINT32 end;
    LSA_BOOL skippedValue = LSA_FALSE;
    LSA_UINT32 startValMod;
    LSA_UINT32 endValMod;

    if(g_Eps_statistics.stats[stats_id].buffer_filled)
    {
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    }
    else
    {
        end = g_Eps_statistics.stats[stats_id].val_end_count;
    }

    if(g_Eps_statistics.stats[stats_id].val_start_count != 0)
    {
    	startValMod = g_Eps_statistics.stats[stats_id].val_start_count - 1;
    }
    else
    {
    	startValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }
    if(g_Eps_statistics.stats[stats_id].val_end_count != 0)
    {
    	endValMod = g_Eps_statistics.stats[stats_id].val_end_count - 1;
    }
    else
    {
    	endValMod = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1;
    }

    for(i = 0; i < end; i++)
    {
        // skip the value which is not ready captured
    	if ( !( (startValMod == i) && !(endValMod == i) ) )
        {
            skippedValue = LSA_TRUE;
            avg += g_Eps_statistics.stats[stats_id].val_diffs[i];
        }
    }

    if( end > 1 )
    {
        // skip the value which is not ready captured
        if ( skippedValue )
        {
            end--;
        }

        avg = avg / end;
    }
    else
    {
        avg = 0;
    }

    return avg;
}

//******************************************************************************
//  Function: eps_stastistics_init
//******************************************************************************
LSA_VOID eps_statistics_init ( LSA_VOID )
{
    LSA_UINT16        entry;
    LSA_RESPONSE_TYPE retVal;

    EPS_ASSERT( !g_Eps_statistics.is_running );

    g_Eps_statistics.EpsStatisticsTraceGearReduction = 0;

    // alloc the critical section
    retVal = eps_alloc_critical_section(&g_Eps_statistics.enter_exit, LSA_FALSE);
    EPS_ASSERT( retVal == LSA_RET_OK );

    for ( entry = 0; entry < EPS_STATISTICS_MAX_STATISTICS; entry++ )
    {
        // set default values
        eps_memset(&g_Eps_statistics.stats[entry], 0, sizeof(EPS_STATISTICS_STATS_TYPE));
    }

    g_Eps_statistics.hThread_running = LSA_FALSE;
    g_Eps_statistics.is_running = LSA_TRUE; // Now initialized   
}

//******************************************************************************
//  Function: eps_statistics_undo_init
//******************************************************************************
LSA_VOID eps_statistics_undo_init ( LSA_VOID )
{
    LSA_UINT16 entry;
    LSA_RESPONSE_TYPE retVal;
    EPS_ASSERT( g_Eps_statistics.is_running );

    g_Eps_statistics.EpsStatisticsTraceGearReduction = 0;

    for ( entry = 0; entry < EPS_STATISTICS_MAX_STATISTICS; entry++ )
    {
        // check all msgq are freed 
        EPS_ASSERT( !g_Eps_statistics.stats[entry].is_used );
    }

    g_Eps_statistics.is_running = LSA_FALSE;

    retVal = eps_free_critical_section(g_Eps_statistics.enter_exit);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

//******************************************************************************
//  Function: eps_statistics_alloc_stats
//******************************************************************************
LSA_UINT32 eps_statistics_alloc_stats ( LSA_UINT32 *stats_id_ptr, const LSA_CHAR * pName )
{    
    LSA_UINT16        entry;    
    LSA_UINT32        i;
    LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;   

    EPS_ASSERT( stats_id_ptr != LSA_NULL );
    EPS_ASSERT( g_Eps_statistics.is_running );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

    // get the next available internal message queue structure
    for ( entry = 0; entry < EPS_STATISTICS_MAX_STATISTICS; entry++ )
    {    
        if ( !g_Eps_statistics.stats[entry].is_used )
        {
            g_Eps_statistics.stats[entry].val_start_next_val_ptr = &g_Eps_statistics.stats[entry].val_start[0];
            g_Eps_statistics.stats[entry].val_end_next_val_ptr = &g_Eps_statistics.stats[entry].val_end[0];

            for (i=0; (i<21) && (pName[i] != 0); i++)
            {
                g_Eps_statistics.stats[entry].pName[i] = pName[i];
            }
            for(;i<25;i++)
            {
                g_Eps_statistics.stats[entry].pName[i] = 0;
            }
            
            g_Eps_statistics.stats[entry].stats_id = entry;
      
            g_Eps_statistics.stats[entry].is_used = LSA_TRUE;            
            
            g_Eps_statistics.stats[entry].buffer_filled = LSA_FALSE;
            
            *stats_id_ptr = entry;
            
            retVal = LSA_RET_OK;
            break;
        }
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_statistics_free_stats
//******************************************************************************
LSA_UINT32 eps_statistics_free_stats ( LSA_UINT32 stats_id )
{    
    LSA_RESPONSE_TYPE   retVal = LSA_RET_OK;

    EPS_ASSERT( g_Eps_statistics.is_running );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

    if ( ( stats_id < EPS_STATISTICS_MAX_STATISTICS ) &&          // in range ?
         ( g_Eps_statistics.stats[stats_id].is_used == LSA_TRUE ))  // allocated ?
    {
        eps_memset(&g_Eps_statistics.stats[stats_id], 0, sizeof(EPS_STATISTICS_STATS_TYPE));    
        
        retVal = LSA_RET_OK;       
    }
    else
    {
        retVal = LSA_RET_ERR_PARAM;
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_statistics_capture_start_value
//******************************************************************************
LSA_VOID eps_statistics_capture_start_value ( LSA_UINT32 stats_id )
{    
    LSA_UINT32  timeInUs;    
    int retVal;
    EPS_ASSERT( g_Eps_statistics.is_running );
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].is_used );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

	// Ueberpruefung ob Reihenfolge der calls passt ( Start -> End -> Start -> End usw...)
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].val_start_count == g_Eps_statistics.stats[stats_id].val_end_count);

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	timeInUs = (LSA_UINT32)pntrc_get_trace_timer();
	#else
    retVal = EPS_POSIX_CLOCK_GETTIME(CLOCK_MONOTONIC, &g_Eps_statistics.stats_time);
    EPS_ASSERT(0 == retVal);

    // convert to us
    timeInUs  = (LSA_UINT32)(g_Eps_statistics.stats_time.tv_nsec / 1000.0);
    timeInUs  += (LSA_UINT32)(g_Eps_statistics.stats_time.tv_sec * 1000000.0);
	#endif

    *g_Eps_statistics.stats[stats_id].val_start_next_val_ptr = timeInUs;
    
    g_Eps_statistics.stats[stats_id].val_start_next_val_ptr++;
    g_Eps_statistics.stats[stats_id].val_start_count++;
    
    // wrap around if buffer is full
    if(g_Eps_statistics.stats[stats_id].val_start_count == EPS_STATISTICS_MAX_VALUES_PER_STATISTIC)
    {
        g_Eps_statistics.stats[stats_id].val_start_next_val_ptr = g_Eps_statistics.stats[stats_id].val_start;
        g_Eps_statistics.stats[stats_id].val_start_count = 0;
        g_Eps_statistics.stats[stats_id].buffer_filled = LSA_TRUE;
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);
}

//******************************************************************************
//  Function: eps_statistics_capture_end_value
//******************************************************************************
LSA_VOID eps_statistics_capture_end_value ( LSA_UINT32 stats_id )
{    
    LSA_UINT32 timeInUs;
    int retVal;
    EPS_ASSERT( g_Eps_statistics.is_running );
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].is_used );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	timeInUs = (LSA_UINT32)pntrc_get_trace_timer();
	#else
    retVal = EPS_POSIX_CLOCK_GETTIME(CLOCK_MONOTONIC, &g_Eps_statistics.stats_time);
    EPS_ASSERT(0 == retVal);

    // convert to us
    timeInUs  = (LSA_UINT32)(g_Eps_statistics.stats_time.tv_nsec / 1000.0);
    timeInUs  += (LSA_UINT32)(g_Eps_statistics.stats_time.tv_sec * 1000000.0);
	#endif

    *g_Eps_statistics.stats[stats_id].val_end_next_val_ptr = timeInUs;
    
    g_Eps_statistics.stats[stats_id].val_end_next_val_ptr++;
    g_Eps_statistics.stats[stats_id].val_end_count++;

    // wrap around if buffer is full
    if(g_Eps_statistics.stats[stats_id].val_end_count == EPS_STATISTICS_MAX_VALUES_PER_STATISTIC)
    {
        g_Eps_statistics.stats[stats_id].val_end_next_val_ptr = g_Eps_statistics.stats[stats_id].val_end;
        g_Eps_statistics.stats[stats_id].val_end_count = 0;
        g_Eps_statistics.stats[stats_id].buffer_filled = LSA_TRUE;
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);
}

//******************************************************************************
//  Function: eps_statistics_delete_last_start_value
//******************************************************************************
LSA_VOID eps_statistics_delete_last_start_value ( LSA_UINT32 stats_id )
{    
    EPS_ASSERT( g_Eps_statistics.is_running );
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].is_used );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);     
    
    // undo wrap around if buffer was full
    if(g_Eps_statistics.stats[stats_id].val_start_count == 0)
    {
        g_Eps_statistics.stats[stats_id].val_start_next_val_ptr = &g_Eps_statistics.stats[stats_id].val_start[EPS_STATISTICS_MAX_VALUES_PER_STATISTIC -1];
        g_Eps_statistics.stats[stats_id].val_start_count = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC - 1;
        g_Eps_statistics.stats[stats_id].buffer_filled = LSA_FALSE;
    }
    else
    {
        g_Eps_statistics.stats[stats_id].val_start_next_val_ptr--;
        g_Eps_statistics.stats[stats_id].val_start_count--;       
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);
}

static LSA_VOID eps_statistics_thread( LSA_UINT32 uParam, void *arg )
{
    LSA_UINT32 i;

    LSA_UNUSED_ARG(uParam);
    LSA_UNUSED_ARG(arg);

    g_Eps_statistics.EpsStatisticsTraceGearReduction++;

    if ((g_Eps_statistics.EpsStatisticsTraceGearReduction % EPS_STATISTICS_TRACE_GEAR_REDUCTION) == 0)
    {
        // trace values
        for (i = 0;i < EPS_STATISTICS_MAX_STATISTICS;i++)
        {
            if (g_Eps_statistics.stats[i].is_used)
            {
                eps_statistics_print_stats_short(i);
            }
        }
    }
}

//******************************************************************************
//  Function: eps_statistics_stack_start_cyclic
//******************************************************************************
LSA_VOID eps_statistics_trace_cyclic_start( LSA_UINT32 cycle_time_ms )
{
    g_Eps_statistics.hThread = eps_tasks_start_poll_thread("EPS_STAT", EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedRR, cycle_time_ms /*ms*/, eps_statistics_thread, 0, (LSA_VOID*)NULL, eRUN_ON_1ST_CORE );
	EPS_ASSERT(g_Eps_statistics.hThread != 0);

	g_Eps_statistics.hThread_running = LSA_TRUE;
}

//******************************************************************************
//  Function: eps_statistics_stack_stop_cyclic
//******************************************************************************
LSA_VOID eps_statistics_trace_cyclic_stop( LSA_VOID )
{
    if (g_Eps_statistics.hThread_running == LSA_TRUE)
    {
        eps_tasks_stop_poll_thread(g_Eps_statistics.hThread);
    }

    g_Eps_statistics.hThread_running = LSA_FALSE;
}

//******************************************************************************
//  Function: eps_statistics_print_stats_short
//******************************************************************************
LSA_VOID eps_statistics_print_stats_short ( LSA_UINT32 stats_id )
{  
    LSA_UINT32 min, max, avg;
    LSA_UINT32 diff_minmax, disp_block_size;
    LSA_UINT32 disp_table[EPS_STATISTICS_DISPERSION_FACTOR] = {0};
    LSA_UINT32 disp_value_table_range_start[EPS_STATISTICS_DISPERSION_FACTOR] = { 0 };
    LSA_UINT32 disp_value_table_range_end[EPS_STATISTICS_DISPERSION_FACTOR] = { 0 };
    LSA_UINT32 i, j;
    LSA_UINT32 percent_uint;
    LSA_UINT32 percent_uint_com;
    LSA_UINT32 end;
    LSA_UINT32 dispersionFactor;

    EPS_ASSERT( g_Eps_statistics.is_running );
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].is_used );

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

    // skip if no values available
    if(!g_Eps_statistics.stats[stats_id].buffer_filled && g_Eps_statistics.stats[stats_id].val_end_count == 0 )
    {
        eps_exit_critical_section(g_Eps_statistics.enter_exit);
        return;
    }

    eps_statistics_calc_diffs(stats_id);

    min = eps_statistics_get_min(stats_id);
    max = eps_statistics_get_max(stats_id);
    avg = eps_statistics_get_avg(stats_id);
    
    EPS_SYSTEM_TRACE_STRING( 0, EPS_STATISTICS_TRACE_LVL, "Statistics Trace: (%s) - All values in us", g_Eps_statistics.stats[stats_id].pName );
    EPS_SYSTEM_TRACE_03( 0, EPS_STATISTICS_TRACE_LVL, "Min: %d   Max: %d    Avg: %d", min, max, avg);
    
    diff_minmax = (LSA_UINT32)(max - min);

    if(diff_minmax < EPS_STATISTICS_DISPERSION_FACTOR)
    {
        dispersionFactor = diff_minmax;
        
        // if min and max value are equal
        if(dispersionFactor == 0)
            dispersionFactor = 1;
    }
    else
    {
        dispersionFactor = EPS_STATISTICS_DISPERSION_FACTOR;
    }    

    disp_block_size = diff_minmax / dispersionFactor + 1;

    for(i = 0; i < dispersionFactor; i++)
    {
        disp_value_table_range_start[i] = min + i*disp_block_size;
        disp_value_table_range_end[i] = min + (i+1)*disp_block_size - 1;
    }

    // correction for the last value
    if (disp_value_table_range_end[dispersionFactor - 1] < max)
    {
        disp_value_table_range_end[dispersionFactor - 1] = max;
    }

    if(g_Eps_statistics.stats[stats_id].buffer_filled)
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    else
        end = g_Eps_statistics.stats[stats_id].val_end_count;

    for(i = 0; i < end; i++)
    {
        for(j = 0; j < dispersionFactor; j++)
        {
            // skip the value which is not ready captured
            if ( !( ((g_Eps_statistics.stats[stats_id].val_start_count -1) == i) && !((g_Eps_statistics.stats[stats_id].val_end_count - 1) == i) ) )
            {
                if(     (g_Eps_statistics.stats[stats_id].val_diffs[i] >= disp_value_table_range_start[j])
                    &&  (g_Eps_statistics.stats[stats_id].val_diffs[i] <= disp_value_table_range_end[j]) )
                {
                    disp_table[j] ++;
                    break;
                }
            }
        }
    }

    EPS_SYSTEM_TRACE_00( 0, EPS_STATISTICS_TRACE_LVL, "_          ValueRange       Calls     Percentage");

    for(i = 0; i < dispersionFactor; i++)
    {
        float percent;

        percent = (float)(((disp_table[i] * 1.0) / end) * 100.0);

        percent_uint = (LSA_UINT32)percent;
        percent_uint_com = (LSA_UINT32)((percent - percent_uint)*10);
        EPS_SYSTEM_TRACE_05( 0, EPS_STATISTICS_TRACE_LVL, "%10d-%10d%12d%15d.%01d", disp_value_table_range_start[i], disp_value_table_range_end[i], disp_table[i], percent_uint, percent_uint_com);
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);
}

//******************************************************************************
//  Function: eps_statistics_print_stats_full
//******************************************************************************
LSA_VOID eps_statistics_print_stats_full ( LSA_UINT32 stats_id )
{   
    LSA_UINT32 i;
    LSA_UINT32 end;
    
    EPS_ASSERT( g_Eps_statistics.is_running );
    EPS_ASSERT( g_Eps_statistics.stats[stats_id].is_used );

    eps_statistics_print_stats_short(stats_id);

    eps_enter_critical_section(g_Eps_statistics.enter_exit);

    EPS_SYSTEM_TRACE_STRING( 0, EPS_STATISTICS_TRACE_LVL, "Statistics Trace: (%s)  - All values in us", g_Eps_statistics.stats[stats_id].pName );
    EPS_SYSTEM_TRACE_00( 0, EPS_STATISTICS_TRACE_LVL, "_    Entry     Start       End      Diff");

    if(g_Eps_statistics.stats[stats_id].buffer_filled)
        end = EPS_STATISTICS_MAX_VALUES_PER_STATISTIC;
    else
        end = g_Eps_statistics.stats[stats_id].val_end_count;

    for(i = 0; i < end; i++)
    {
        EPS_SYSTEM_TRACE_04( 0, EPS_STATISTICS_TRACE_LVL, "%10d%10d%10d%10d", i, g_Eps_statistics.stats[stats_id].val_start[i], g_Eps_statistics.stats[stats_id].val_end[i], g_Eps_statistics.stats[stats_id].val_diffs[i]);
    }

    eps_exit_critical_section(g_Eps_statistics.enter_exit);
}

#endif // #if ( EPS_CFG_USE_STATISTICS == 1 )
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
