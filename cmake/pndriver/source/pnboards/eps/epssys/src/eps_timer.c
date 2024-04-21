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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_timer.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Timer Interface                                                      */
/*                                                                           */
/*****************************************************************************/

#include <string.h>
#include <errno.h>

#include <psi_inc.h>

#include <eps_rtos.h>
#include <eps_sys.h>
#include <eps_locks.h>

#include <eps_timer.h>
#include <eps_tasks.h>
#include <eps_trc.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdio.h>
#endif

#define LTRC_ACT_MODUL_ID 20038

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
// maximum count of sys timer available to allocate
// if you change this -> see posix2win.c -> P2W_TIMER_COUNT_MAX, change there too
// if you change this -> see epsTimerTests.cpp -> EPS_GTEST_SYSTIMER_ALLOCATE_TOO_MUCH_TIMERS, change there too
#define EPS_TIMER_MAX_SYS_TIMER     10

#define EPS_TIMER_GROUP0_BASE_TICK          10          // in ms
#define EPS_TIMER_GROUP1_BASE_TICK          1           // in ms

#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
#define EPS_TIMER_NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */
#endif

// #define EPS_TEST_MEASURE_PERFORMANCE_TGROUP0            // measure and trace processing time in timer group 0
// #define EPS_TEST_MEASURE_PERFORMANCE_TGROUP1            // measure and trace processing time in timer group 1

#if (defined(EPS_TEST_MEASURE_PERFORMANCE_TGROUP0) || defined (EPS_TEST_MEASURE_PERFORMANCE_TGROUP1))
#include <eps_statistics.h>
#endif

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
static void internal_timer_timeout_tgroup0( union sigval sig );
static void internal_timer_timeout_tgroup1( union sigval sig );
#endif

#ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0 
#if (EPS_CFG_USE_STATISTICS == 0)
#error "Use EPS_TEST_MEASURE_PERFORMANCE_TGROUP0 only in combination with EPS_CFG_USE_STATISTICS == 1!!!"
#endif
#endif

#ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1 
#if (EPS_CFG_USE_STATISTICS == 0)
#error "Use EPS_TEST_MEASURE_PERFORMANCE_TGROUP0 only in combination with EPS_CFG_USE_STATISTICS == 1!!!"
#endif
#endif

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------
typedef enum TimerStatus
{
    TS_FREE = 0,
    TS_RUNNING,
    TS_STOPPED
}TimerStatus;

typedef struct _TimerRecord
{
    struct _TimerRecord *  next;      
    struct _TimerRecord *  prior;
    TimerStatus           status;
    LSA_UINT16            timer_id;
    LSA_UINT16            timer_type;
    LSA_UINT16            time_base;
    EPS_TIMER_CBF         callback;
    LSA_USER_ID_TYPE      user_id;
    LSA_UINT32            target;     // value of 1ms timer tick at next timeout
    LSA_UINT32            preset;     // period - number of 1 ms timer ticks
}TimerRecord;

typedef enum {
    eTimerInit     = 0,
    eTimerRun      = 1,
    eTimerReqStop  = 2,
    eTimerStopped  = 3
}EPS_TIMER_STATE_TYPE;

typedef struct TimerState
{
    volatile EPS_TIMER_STATE_TYPE eState;
    LSA_UINT16            lock_id;
    LSA_UINT32            last_tick;
    LSA_UINT32            next_10ms_tick;
    LSA_UINT32            next_100ms_tick;
    LSA_UINT32            next_1s_tick;
    LSA_UINT32            next_10s_tick;
    LSA_UINT32            next_100s_tick;
    TimerRecord *         first1msTimer;
    TimerRecord *         first10msTimer;
    TimerRecord *         first100msTimer;
    TimerRecord *         first1sTimer;
    TimerRecord *         first10sTimer;
    TimerRecord *         first100sTimer;
    TimerRecord           timers[200];

}TimerState;

//lint -esym(826, g_Eps_Timer.state1.first*)
//lint -esym(826, g_Eps_Timer.state0.first*)
typedef struct
{
    LSA_BOOL                is_used;
    LSA_UINT16              sys_timer_id;
    timer_t                 sys_timer;
    LSA_UINT32              interval_ms;
    pthread_t               thread_id;
    EPS_SYS_TIMER_CBF       cbf;
}EPS_TIMER_SYS_TIMER_TYPE;

typedef struct
{
    LSA_BOOL                    is_running;                           // Initialized yes/no
    LSA_UINT16                  enter_exit;                           // Own reentrance lock
    EPS_TIMER_SYS_TIMER_TYPE    sysTimer[EPS_TIMER_MAX_SYS_TIMER];    // SysTimer list
}EPS_TIMER_SYS_TIMER_INSTANCE_TYPE;


typedef struct
{
    TimerState   state0; // ATTR_LOCATE_IN_DTCM
    TimerState   state1; // ATTR_LOCATE_IN_DTCM
    
    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    pthread_t           thread_id_tg0;
    pthread_t           thread_id_tg1;
    #else
    timer_t      tid0;
    timer_t      tid1;
    #endif // ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
} EPS_TIMER_TYPE;

static EPS_TIMER_TYPE g_Eps_Timer;

#ifdef _TEST
    LSA_UINT16 getEpsTimerMutex0(void) { return g_Eps_Timer.state0.lock_id; }
    LSA_UINT16 getEpsTimerMutex1(void) { return g_Eps_Timer.state1.lock_id; }
#endif //_TEST

#ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0
LSA_UINT32 stats_id_tgroup0;
#endif //EPS_TEST_MEASURE_PERFORMANCE_TGROUP0

#ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1
LSA_UINT32 stats_id_tgroup1;
#endif //EPS_TEST_MEASURE_PERFORMANCE_TGROUP1

// internal instance to handle the sys timers
static EPS_TIMER_SYS_TIMER_INSTANCE_TYPE g_Eps_sys_timer = {0};


//******************************************************************************
//  Function: internal_timer_fdtimer_handler
//
//  Description:
//
//      Thread-Function for fdtimer to trigger callback-function on timer-event
//
//  Parameters:
//
//      params       -- in:  struct with fd and callback (fdtimerHandlerParamsType)
//
//  Return:
//
//      None
//******************************************************************************
#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
void *internal_timer_fdtimer_handler_tg0(void * params)
{
    struct timespec t;
    union sigval    sig;
    const int       interval = EPS_TIMER_GROUP0_BASE_TICK * 1000 * 1000;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT, "internal_timer_fdtimer_handler_tg0()");

    eps_memset(&sig, 0, sizeof(sig));

    clock_gettime(CLOCK_MONOTONIC, &t);
    t.tv_nsec += interval;
    //handle overrun of nsec
    while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC)
    {
        t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
        t.tv_sec++;
    }

    while(1)
    {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

        internal_timer_timeout_tgroup0(sig);

        t.tv_nsec += interval;
        //handle overrun of nsec
        while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC) {
            t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
            t.tv_sec++;
        }
    }
}

void *internal_timer_fdtimer_handler_tg1(void * params)
{
    struct timespec t;
    union sigval    sig;
    const int       interval = EPS_TIMER_GROUP1_BASE_TICK * 1000 * 1000;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT, "internal_timer_fdtimer_handler_tg1()");

    eps_memset(&sig, 0, sizeof(sig));

    clock_gettime(CLOCK_MONOTONIC, &t);
    t.tv_nsec += interval;  
    //handle overrun of nsec
    while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC) {
        t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
        t.tv_sec++;
    }

    while(1)
    {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

        internal_timer_timeout_tgroup1(sig);

        t.tv_nsec += interval;
        //handle overrun of nsec
        while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC) {
            t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
            t.tv_sec++;
        }
    }
}

void *internal_sys_timer_handler(void * params)
{
    EPS_TIMER_SYS_TIMER_TYPE * sys_timer = (EPS_TIMER_SYS_TIMER_TYPE *) params;
    union sigval                 sig;
    struct timespec              t;

    EPS_ASSERT( sys_timer->interval_ms > 0 );

    EPS_ASSERT( sys_timer->cbf != LSA_NULL );

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,"internal_sys_timer_handler thread for systimer 0x%X started", sys_timer->sys_timer_id);

    clock_gettime(CLOCK_MONOTONIC, &t);
    t.tv_sec     +=  sys_timer->interval_ms / 1000;
    t.tv_nsec    += (sys_timer->interval_ms % 1000) * 1000000;
    //handle overrun of nsec
    while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC) {
            t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
            t.tv_sec++;
    }

    eps_memset(&sig, 0, sizeof(sig));

    while(1)
    {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

        sys_timer->cbf(sig);

        t.tv_sec     +=  sys_timer->interval_ms / 1000;
        t.tv_nsec    += (sys_timer->interval_ms % 1000) * 1000000;
        //handle overrun of nsec
        while (t.tv_nsec >= EPS_TIMER_NSEC_PER_SEC) {
            t.tv_nsec -= EPS_TIMER_NSEC_PER_SEC;
            t.tv_sec++;
        }
    }

    

    return NULL;
}
#endif // ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))


//******************************************************************************
//  Function: internal_timer_stop_tgroup1
//
//  Description:
//
//      Stop a timer and remove it from its timer list.
//
//  Parameters:
//
//      timer       -- in:  timer to be stopped and removed from the list
//
//  Return:
//
//      None
//******************************************************************************
static void internal_timer_stop_tgroup1(
    TimerRecord *   timer )
{
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE, "eps_timer internal_timer_stop_tgroup1 %d", timer->timer_id);

    if (NULL == timer->prior)
    {
        if (LSA_TIME_BASE_1MS == timer->time_base)
        {
            g_Eps_Timer.state1.first1msTimer = timer->next;
        }
        else /* LSA_TIME_BASE_10MS */
        {
            g_Eps_Timer.state1.first10msTimer = timer->next;
        }
    }
    else
    {
        (timer->prior)->next = timer->next;
    }
    if (NULL != timer->next)
    {
        (timer->next)->prior = timer->prior;
    }
    timer->status = TS_STOPPED;
}

//******************************************************************************
//  Function: internal_timer_stop_tgroup0
//
//  Description:
//
//      Stop a timer and remove it from its timer list.
//
//  Parameters:
//
//      timer       -- in:  timer to be stopped and removed from the list
//
//  Return:
//
//      None
//******************************************************************************
static void internal_timer_stop_tgroup0(
    TimerRecord *   timer )
{
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE, "eps_timer internal_timer_stop_tgroup0 %d", timer->timer_id);

    if (NULL == timer->prior)
    {
        if (LSA_TIME_BASE_100MS == timer->time_base)
        {
            g_Eps_Timer.state0.first100msTimer = timer->next;
        }
        else if (LSA_TIME_BASE_1S == timer->time_base)
        {
            g_Eps_Timer.state0.first1sTimer = timer->next;
        }
        else if (LSA_TIME_BASE_10S == timer->time_base)
        {
            g_Eps_Timer.state0.first10sTimer = timer->next;
        }
        else /* LSA_TIME_BASE_100S */
        {
            g_Eps_Timer.state0.first100sTimer = timer->next;
        }
    }
    else
    {
        (timer->prior)->next = timer->next;
    }
    if (NULL != timer->next)
    {
        (timer->next)->prior = timer->prior;
    }
    timer->status = TS_STOPPED;
}


//******************************************************************************
//  Function: eps_timer_start_tgroup0
//
//  Description:
//
//      This function will start timing on a timer allocated using
//      eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to start
//      user_id      -- in:   Caller's identifier for the timeout instance
//      timeVal      -- in:   timeout time in units of time_base at
//                            eps_timer_allocate
//
//  Return:
//
//      LSA_RET_OK                  The timer was stopped and has been started.
//      LSA_RET_OK_TIMER_RESTARTED  The timer was running and has been restarted
//      LSA_RET_ERR_PARAM           The function failed due to a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_start_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          timeVal )
{
    eps_enter_critical_section(g_Eps_Timer.state0.lock_id);

    EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_start_tgroup0 (%d, 0x%X, %d)",timer_id,user_id.void_ptr,timeVal);

    if ((timer_id < countof(g_Eps_Timer.state0.timers)) && (0 < timeVal))
    {
        TimerRecord * timer = &(g_Eps_Timer.state0.timers[timer_id]);

        if ((TS_STOPPED == timer->status) || (TS_RUNNING == timer->status))
        {
            TimerRecord ** anchor = NULL;

            EPS_ASSERT ( (timer->time_base == LSA_TIME_BASE_100MS) || (timer->time_base == LSA_TIME_BASE_1S) || (timer->time_base == LSA_TIME_BASE_10S) || (timer->time_base == LSA_TIME_BASE_100S) );

            timer->user_id = user_id;
            if (LSA_TIME_BASE_100MS == timer->time_base)
            {
                timer->preset = timeVal * 100;
                timer->target = g_Eps_Timer.state0.next_100ms_tick + timer->preset;
                anchor = (TimerRecord **)&g_Eps_Timer.state0.first100msTimer;
            }
            else if (LSA_TIME_BASE_1S == timer->time_base)
            {
                timer->preset = timeVal * 1000;
                timer->target = g_Eps_Timer.state0.next_1s_tick + timer->preset;
                anchor = (TimerRecord **)&g_Eps_Timer.state0.first1sTimer;
            }
            else if (LSA_TIME_BASE_10S == timer->time_base)
            {
                timer->preset = timeVal * 10000;
                timer->target = g_Eps_Timer.state0.next_10s_tick + timer->preset;
                anchor = (TimerRecord **)&g_Eps_Timer.state0.first10sTimer;
            }
            else /* LSA_TIME_BASE_100S */
            {
                timer->preset = timeVal * 100000;
                timer->target = g_Eps_Timer.state0.next_100s_tick + timer->preset;
                anchor = (TimerRecord **)&g_Eps_Timer.state0.first100sTimer;
            }

            if (TS_STOPPED == timer->status)
            {
                timer->prior = NULL;
                timer->next = *anchor;
                if (NULL != *anchor)
                {
                    (*anchor)->prior = timer;
                }
                *anchor = timer;
                timer->status = TS_RUNNING;
                g_Eps_Timer.state0.eState = eTimerRun;
                EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start_tgroup0 -- succeeded, timer started");
                eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
                return LSA_RET_OK;
            }

            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start_tgroup0 -- succeeded, timer restarted");
            eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

            //return LSA_RET_OK_TIMER_RESTARTED;
            return LSA_RET_OK; //@@@EDDI_StartTimer()?!
        }
    }

    if (timer_id >= countof(g_Eps_Timer.state0.timers))
    {
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,">< eps_timer_start_tgroup0 - TIMER INVALID");
        EPS_FATAL("eps_timer_start_tgroup0 - TIMER INVALID");
    }
    else
    {
        TimerRecord * timer = &(g_Eps_Timer.state0.timers[timer_id]);

        EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_start_tgroup0 (timer_id = %d, user_id.void_ptr = 0x%X, time = %d, Status = %d) -- failed, parameter error)",timer_id, user_id.void_ptr, timeVal, timer->status);
        eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
    }

    return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_stop_tgroup0
//
//  Description:
//
//      This function will stop timing on a timer started using eps_timer_start.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to stop
//
//  Return:
//
//      LSA_RET_OK                    The timer was stopped.
//      LSA_RET_OK_TIMER_NOT_RUNNING  The timer was not running
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_stop_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id )
{
    eps_enter_critical_section(g_Eps_Timer.state0.lock_id);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_stop_tgroup0 (%d)",timer_id);

    if (timer_id < countof(g_Eps_Timer.state0.timers))
    {
        TimerRecord * timer = &(g_Eps_Timer.state0.timers[timer_id]);

        if (TS_STOPPED == timer->status)
        {
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop_tgroup0 -- succeeded, timer was not running");
            eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
            return LSA_RET_OK_TIMER_NOT_RUNNING;
        }
        else if (TS_RUNNING == timer->status)
        {
            internal_timer_stop_tgroup0(timer);
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop_tgroup0 -- succeeded");
            eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
            return LSA_RET_OK;
        }
    }

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_stop_tgroup0 (%d) -- failed, parameter error",timer_id);

    eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

    return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_free_tgroup0
//
//  Description:
//
//      This function will release a timer allocated using eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was released.
//      LSA_RET_ERR_TIMER_IS_RUNNING  The timer is running and was not released
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_free_tgroup0(
    LSA_TIMER_ID_TYPE   timer_id )
{
    eps_enter_critical_section(g_Eps_Timer.state0.lock_id);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_free_tgroup0 (%d)",timer_id);

    if (timer_id < countof(g_Eps_Timer.state0.timers))
    {
        TimerRecord * timer = &(g_Eps_Timer.state0.timers[timer_id]);

        if (TS_STOPPED == timer->status)
        {
            timer->status = TS_FREE;
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_free_tgroup0 -- succeeded");
            eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
            EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_free_tgroup0 - timer_id = %d", timer_id);
            return LSA_RET_OK;
        }
        else if (TS_RUNNING == timer->status)
        {
            EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free_tgroup0 (%d) -- failed, timer is running",timer_id);
            eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
            return LSA_RET_ERR_TIMER_IS_RUNNING;
        }
    }

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free_tgroup0 (%d) -- failed, parameter error",timer_id);

    eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

    return LSA_RET_ERR_PARAM;
}


//******************************************************************************
//  Function: eps_timer_start_tgroup1
//
//  Description:
//
//      This function will start timing on a timer allocated using
//      eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to start
//      user_id      -- in:   Caller's identifier for the timeout instance
//      timeUnits    -- in:   timeout time in units of time_base at
//                            eps_timer_allocate
//
//  Return:
//
//      LSA_RET_OK                  The timer was stopped and has been started.
//      LSA_RET_OK_TIMER_RESTARTED  The timer was running and has been restarted
//      LSA_RET_ERR_PARAM           The function failed due to a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_start_tgroup1(
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          timeUnits )
{
    eps_enter_critical_section(g_Eps_Timer.state1.lock_id);

    EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_start_tgroup1 (%d, 0x%X, %d)",timer_id,user_id.void_ptr,timeUnits);

    if ( ((timer_id & 0x7FFF) < countof(g_Eps_Timer.state1.timers)) && (0 < timeUnits) )
    {
        TimerRecord * timer = &(g_Eps_Timer.state1.timers[timer_id & 0x7FFF]);

        if ((TS_STOPPED == timer->status) || (TS_RUNNING == timer->status))
        {
            TimerRecord ** anchor = NULL;

            EPS_ASSERT( (timer->time_base == LSA_TIME_BASE_1MS) || (timer->time_base == LSA_TIME_BASE_10MS) );

            timer->user_id = user_id;
            if (LSA_TIME_BASE_1MS == timer->time_base)
            {
                timer->preset = timeUnits;
                timer->target = g_Eps_Timer.state1.last_tick + timer->preset + 1;
                anchor = (TimerRecord **)&g_Eps_Timer.state1.first1msTimer;
            }
            else /* LSA_TIME_BASE 10MS */
            {
                timer->preset = timeUnits * 10;
                timer->target = g_Eps_Timer.state1.next_10ms_tick + timer->preset;
                anchor = (TimerRecord **)&g_Eps_Timer.state1.first10msTimer;
            }

            if (TS_STOPPED == timer->status)
            {
                timer->prior = NULL;
                timer->next = *anchor;
                if (NULL != *anchor)
                {
                    (*anchor)->prior = timer;
                }
                *anchor = timer;
                timer->status = TS_RUNNING;
                g_Eps_Timer.state1.eState = eTimerRun;
                EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start_tgroup1 -- succeeded, timer started");
                eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
                return LSA_RET_OK;
            }

            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_start_tgroup1 -- succeeded, timer restarted");
            eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

            //return LSA_RET_OK_TIMER_RESTARTED;
            return LSA_RET_OK; //@@@EDDI_StartTimer()?!
        }
    }

    if(timer_id >= countof(g_Eps_Timer.state1.timers))
    {
        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,">< eps_timer_start_tgroup1 - TIMER INVALID");
        return LSA_RET_ERR_RESOURCE;
    }
    else
    {
        TimerRecord * timer = &(g_Eps_Timer.state1.timers[timer_id & 0x7FFF]);

        EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_start_tgroup1 (timer_id = %d, user_id.void_ptr = 0x%X, timeUnits = %d, Status = %d) -- failed, parameter error)",timer_id, user_id.void_ptr, timeUnits, timer->status);
        eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
        return LSA_RET_ERR_PARAM;
    }
}

//******************************************************************************
//  Function: eps_timer_stop_tgroup1
//
//  Description:
//
//      This function will stop timing on a timer started using eps_timer_start.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to stop
//
//  Return:
//
//      LSA_RET_OK                    The timer was stopped.
//      LSA_RET_OK_TIMER_NOT_RUNNING  The timer was not running
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_stop_tgroup1(
    LSA_TIMER_ID_TYPE   timer_id )
{
    eps_enter_critical_section(g_Eps_Timer.state1.lock_id);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_stop_tgroup1 (%d)",timer_id);

    if ((timer_id & 0x7FFF) < countof(g_Eps_Timer.state1.timers))
    {
        TimerRecord * timer = &(g_Eps_Timer.state1.timers[timer_id & 0x7FFF]);

        if (TS_STOPPED == timer->status)
        {
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop_tgroup1 -- succeeded, timer was not running");
            eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
            return LSA_RET_OK_TIMER_NOT_RUNNING;
        }
        else if (TS_RUNNING == timer->status)
        {
            internal_timer_stop_tgroup1(timer);
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_stop_tgroup1 -- succeeded");
            eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
            return LSA_RET_OK;
        }
    }

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,">< eps_timer_stop_tgroup1 (%d) -- failed, parameter error",timer_id);

    eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

    return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_free_tgroup1
//
//  Description:
//
//      This function will release a timer allocated using eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was released.
//      LSA_RET_ERR_TIMER_IS_RUNNING  The timer is running and was not released
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
static LSA_UINT16 eps_timer_free_tgroup1(
    LSA_TIMER_ID_TYPE   timer_id )
{
    eps_enter_critical_section(g_Eps_Timer.state1.lock_id);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_free_tgroup1 (%d)",timer_id);

    if ((timer_id & 0x7FFF) < countof(g_Eps_Timer.state1.timers))
    {
        TimerRecord * timer = &(g_Eps_Timer.state1.timers[(timer_id & 0x7FFF)]);

        if (TS_STOPPED == timer->status)
        {
            timer->status = TS_FREE;
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_free_tgroup1 -- succeeded");
            eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

            EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_free_tgroup1 - timer_id = %d", timer_id);

            return LSA_RET_OK;
        }
        else if (TS_RUNNING == timer->status)
        {
            EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free_tgroup1 (%d) -- failed, timer is running",timer_id);
            eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
            return LSA_RET_ERR_TIMER_IS_RUNNING;
        }
    }

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_free_tgroup1 (%d) -- failed, parameter error",timer_id);

    eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

    return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: internal_timer_timeout_tgroup0
//
//  Description:
//
//      Implement the time-keeping algorithm. The implementation assumes a small
//      number of timers. It runs in IRQ state0 in 1 ms tick.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
//#pragma ghs nothumb
static void internal_timer_timeout_tgroup0( union sigval sig )
{
    TimerRecord * timer;
    uint32_t      cur_tick;
    struct   itimerspec    its;
    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    int retVal;
    #endif
    LSA_UINT16 retValFree;
    LSA_UNUSED_ARG(sig);

    // if Shutdown is started, ignore this function
    if (g_Eps_Timer.state0.eState == eTimerReqStop)
    {
        eps_memset( &its, 0, sizeof(its) );

        #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
        retVal = EPS_POSIX_TIMER_SETTIME( (timer_t)g_Eps_Timer.tid0, 0, &its, NULL ); // the timer disarm itself
        EPS_ASSERT(0 == retVal);
        #endif

        // Other thread is cleaning up this thread
        // the cleanup thread waits until this thread is stopped
        // and cleans this thread afterwards.
        g_Eps_Timer.state0.eState = eTimerStopped;

        return;
    }

    if ((g_Eps_Timer.state0.eState == eTimerStopped) || (g_Eps_Timer.state0.eState == eTimerInit))
    {
    	return; // timer can be triggered one time again after delete(eThStopped) and while init(eThInit)
    }

    eps_enter_critical_section(g_Eps_Timer.state0.lock_id);

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0
    eps_statistics_capture_start_value(stats_id_tgroup0);
    #endif

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup0");

    cur_tick = g_Eps_Timer.state0.last_tick + EPS_TIMER_GROUP0_BASE_TICK;
    g_Eps_Timer.state0.last_tick = cur_tick;

    if ( !PSI_FATAL_ERROR_OCCURED() )
    {
        if (cur_tick == g_Eps_Timer.state0.next_100ms_tick)
        {
            g_Eps_Timer.state0.next_100ms_tick = cur_tick + 100;
            timer = g_Eps_Timer.state0.first100msTimer;
            while (NULL != timer)
            {
                if (cur_tick == timer->target)
                {
                    EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                    timer->target = cur_tick + timer->preset;
                    if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                    {
                        (*timer->callback) (timer->timer_id, timer->user_id);
                        continue;
                    }
                    internal_timer_stop_tgroup0(timer);
                    (*timer->callback) (timer->timer_id, timer->user_id);
                }
                timer = timer->next;
            }

            if (cur_tick == g_Eps_Timer.state0.next_1s_tick)
            {
                g_Eps_Timer.state0.next_1s_tick = cur_tick + 1000;
                timer = g_Eps_Timer.state0.first1sTimer;
                while (NULL != timer)
                {
                    if (cur_tick == timer->target)
                    {
                        EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                        timer->target = cur_tick + timer->preset;
                        if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                        {
                            (*timer->callback) (timer->timer_id, timer->user_id);
                            continue;
                        }
                        internal_timer_stop_tgroup0(timer);
                        (*timer->callback) (timer->timer_id, timer->user_id);
                    }
                    timer = timer->next;
                }
            }

            if (cur_tick == g_Eps_Timer.state0.next_10s_tick)
            {
                g_Eps_Timer.state0.next_10s_tick = cur_tick + 10000;
                timer = g_Eps_Timer.state0.first10sTimer;
                while (NULL != timer)
                {
                    if (cur_tick == timer->target)
                    {
                        EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                        timer->target = cur_tick + timer->preset;
                        if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                        {
                            (*timer->callback) (timer->timer_id, timer->user_id);
                            continue;
                        }
                        internal_timer_stop_tgroup0(timer);
                        (*timer->callback) (timer->timer_id, timer->user_id);
                    }
                    timer = timer->next;
                }
            }

            if (cur_tick == g_Eps_Timer.state0.next_100s_tick)
            {
                g_Eps_Timer.state0.next_100s_tick = cur_tick + 100000;
                timer = g_Eps_Timer.state0.first100sTimer;
                while (NULL != timer)
                {
                    if (cur_tick == timer->target)
                    {
                        EPS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,"eps_timer0 calling 0x%X(%d, 0x%X)",timer->callback,timer->timer_id,timer->user_id.void_ptr);

                        timer->target = cur_tick + timer->preset;
                        if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                        {
                            (*timer->callback) (timer->timer_id, timer->user_id);
                            continue;
                        }
                        internal_timer_stop_tgroup0(timer);
                        (*timer->callback) (timer->timer_id, timer->user_id);
                    }
                    timer = timer->next;
                }
            }
        }
    }
    else
    {
        LSA_UINT32 i;

        EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer internal_timer_timeout_tgroup0 - else case");

        for (i = 0; i < countof(g_Eps_Timer.state0.timers); i++)
	    {
            if(TS_FREE != g_Eps_Timer.state0.timers[i].status)
            {
                if (TS_RUNNING == g_Eps_Timer.state0.timers[i].status)
                {
                    internal_timer_stop_tgroup0(&g_Eps_Timer.state0.timers[i]);
                }
                retValFree = eps_timer_free_tgroup0(g_Eps_Timer.state0.timers[i].timer_id);
                EPS_ASSERT(retValFree == LSA_RET_OK);
            }
	    }

        eps_timer_undo_init_tgroup0();
    }

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer internal_timer_timeout_tgroup0");

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0
    eps_statistics_capture_end_value(stats_id_tgroup0);
    #endif

    eps_exit_critical_section(g_Eps_Timer.state0.lock_id);
}
//#pragma ghs thumb

//******************************************************************************
//  Function: eps_timer_init_tgroup0
//
//  Description:
//
//      This function will initialize rhe IRQ-based timer subsystem.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      LSA_RET_OK            The timer subsystem was initialized successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of resources
//******************************************************************************
LSA_UINT16 eps_timer_init_tgroup0( LSA_VOID )
{
    pthread_attr_t           thread_attr;
    struct sched_param       sched_param;
    struct sigevent          event;
    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    struct itimerspec        timeout;
    #endif //#if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    int                      ret;
    LSA_UINT16               i;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup0 - begin");

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0
    eps_statistics_alloc_stats(&stats_id_tgroup0, "TimerGroup_0");
    #endif

    // initialize the state0 record
    eps_memset( &g_Eps_Timer.state0, 0, sizeof(g_Eps_Timer.state0) );

    ret = eps_alloc_critical_section( &g_Eps_Timer.state0.lock_id, LSA_FALSE );
    if (ret != LSA_RET_OK)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    g_Eps_Timer.state0.next_100ms_tick  = 100;
    g_Eps_Timer.state0.next_1s_tick     = 1000;
    g_Eps_Timer.state0.next_10s_tick    = 10000;
    g_Eps_Timer.state0.next_100s_tick   = 100000;
    for (i = 0; i < countof(g_Eps_Timer.state0.timers); i++)
    {
        g_Eps_Timer.state0.timers[i].timer_id = i;
    }

    // initialize the "thread" attributes (we don't use a thread)
    eps_memset( &thread_attr, 0, sizeof( thread_attr ) );
    ret = EPS_POSIX_PTHREAD_ATTR_INIT( &thread_attr );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Enable explicitly to set individual scheduling parameters, otherwise they are taken from this thread
    ret = EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &thread_attr, PTHREAD_EXPLICIT_SCHED );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Set scheduling policy to FIFO
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY( &thread_attr, SCHED_FIFO );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Set scheduling priority
    eps_memset( &sched_param, 0, sizeof( sched_param ) );
    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_TIMER;
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &thread_attr, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    //set thread name
    ret = pthread_attr_setname( &thread_attr, "TIMER_TGROUP0" );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif
  
    // fill event data and create the timer
    eps_memset( &event, 0, sizeof( event ) );
    event.sigev_notify = SIGEV_THREAD;

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)) // TODO -> move SIGNO to Posix-IF
    event.sigev_signo = SIGRTMIN;
    #else
    event.sigev_signo = SIGRT15;
    #endif

    event.sigev_notify_function   = internal_timer_timeout_tgroup0;
    event.sigev_notify_attributes = &thread_attr;
    event.sigev_value.sival_ptr   = NULL;

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    //create handler thread for tg0 with same parameters as timer thread
    ret = EPS_POSIX_PTHREAD_CREATE(&g_Eps_Timer.thread_id_tg0, &thread_attr, &internal_timer_fdtimer_handler_tg0, NULL);
    EPS_POSIX_PTHREAD_SETNAME_NP(g_Eps_Timer.thread_id_tg0, "TIMER_TGROUP0");
    #else
    ret = EPS_POSIX_TIMER_CREATE( CLOCK_MONOTONIC, &event, (timer_t*)&g_Eps_Timer.tid0 );
    #endif // ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))

    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))

    // start the timer - EPS_TIMER_GROUP0_BASE_TICK ms cyclic timeout
    timeout.it_value.tv_sec     = 0; // EPS_TIMER_GROUP0_BASE_TICK / 1000 --> evaluates to 0
    timeout.it_value.tv_nsec    = 1000000 * (EPS_TIMER_GROUP0_BASE_TICK % 1000);
    timeout.it_interval.tv_sec  = 0; // EPS_TIMER_GROUP0_BASE_TICK / 1000 --> evaluates to 0
    timeout.it_interval.tv_nsec = 1000000 * (EPS_TIMER_GROUP0_BASE_TICK % 1000);

    ret = EPS_POSIX_TIMER_SETTIME( (timer_t)g_Eps_Timer.tid0, 0, &timeout, NULL );
    #endif //((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_init_tgroup0 - done");

    return LSA_RET_OK;
}

LSA_VOID eps_timer_undo_init_tgroup0( LSA_VOID )
{
	LSA_UINT32 i;
    LSA_RESPONSE_TYPE retValFree;
    int retVal;
    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
	void *res;
    #endif //#if !((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup0 - Timer-State=%u", g_Eps_Timer.state0.eState);

    // Tell the timeout function to stop it's work.
    g_Eps_Timer.state0.eState = eTimerReqStop;

    // Synchronize with other thread/timer wait until it is stopped
    while (g_Eps_Timer.state0.eState != eTimerStopped)
    {
        eps_tasks_sleep(1);
    }

    eps_enter_critical_section( g_Eps_Timer.state0.lock_id );
	for (i = 0; i < countof(g_Eps_Timer.state0.timers); i++)
	{
		EPS_ASSERT(g_Eps_Timer.state0.timers[i].status == TS_FREE);
	}

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    retVal = EPS_POSIX_PTHREAD_CANCEL(g_Eps_Timer.thread_id_tg0);
    EPS_ASSERT(0 == retVal);
    retVal = EPS_POSIX_PTHREAD_JOIN(g_Eps_Timer.thread_id_tg0, &res);
    EPS_ASSERT(0 == retVal);
    EPS_ASSERT(res == PTHREAD_CANCELED);

    g_Eps_Timer.thread_id_tg0 = 0;
    #else
	retVal = EPS_POSIX_TIMER_DELETE( (timer_t)g_Eps_Timer.tid0 );
    EPS_ASSERT(0 == retVal);
    #endif
    eps_exit_critical_section(g_Eps_Timer.state0.lock_id );

    // do the actual clean up after timer is stopped!
    retValFree = eps_free_critical_section( g_Eps_Timer.state0.lock_id );
    EPS_ASSERT(retValFree == LSA_RET_OK);
    g_Eps_Timer.state0.lock_id = 0;

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP0
    eps_statistics_free_stats(stats_id_tgroup0);
    #endif

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_undo_init_tgroup0 - Timer-State=%u", g_Eps_Timer.state0.eState);
}

//******************************************************************************
//  Function: eps_timer_allocate_tgroup0
//
//  Description:
//
//      This function will allocate an IRQ-based timer.
//
//  Parameters:
//
//      timer_id_ptr -- out:  points to returned value of timer's identifier
//      timer_type   -- in:   Type of timer -
//
//                                  LSA_TIMER_TYPE_ONE_SHOT or
//                                  LSA_TIMER_TYPE_CYCLIC
//
//      time_base    -- in:   Units of preset at start --
//
//                                  LSA_TIME_BASE_1MS
//                                  LSA_TIME_BASE_10MS,
//                                  LSA_TIME_BASE_100MS or
//                                  LSA_TIME_BASE_1S
//
//      callback     -- in:   pointer to function called on timeout.
//
//                              NOTE: Callback function is called in IRQ state.
//
//  Return:
//
//      LSA_RET_OK            The timer was allocated successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of timer resources
//      LSA_RET_ERR_PARAM     The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_allocate_tgroup0(
    LSA_TIMER_ID_TYPE * timer_id_ptr,
    LSA_UINT16          timer_type,
    LSA_UINT16          time_base,
    EPS_TIMER_CBF       callback )
{
    LSA_UINT16 i;

    eps_enter_critical_section(g_Eps_Timer.state0.lock_id);

    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_allocate_tgroup0 (0x%X, %d, %d, 0x%X)",timer_id_ptr,timer_type,time_base,callback);

    if ( ( (timer_type == LSA_TIMER_TYPE_ONE_SHOT) ||
           (timer_type == LSA_TIMER_TYPE_CYCLIC) ) &&
           ( (time_base == LSA_TIME_BASE_100MS) ||
             (time_base == LSA_TIME_BASE_1S) ||
             (time_base == LSA_TIME_BASE_10S) ||
             (time_base == LSA_TIME_BASE_100S) ) &&
           (NULL != callback) )
    {
        for (i = 0; i < countof(g_Eps_Timer.state0.timers); i++)
        {
            if (TS_FREE == g_Eps_Timer.state0.timers[i].status)
            {
                TimerRecord * timer = &(g_Eps_Timer.state0.timers[i]);

                timer->prior = NULL;
                timer->next = NULL;
                timer->status = TS_STOPPED;
                timer->timer_type = timer_type;
                timer->time_base = time_base;
                timer->callback = callback;
                eps_memset(&(timer->user_id), 0, sizeof(timer->user_id));
                timer->target = 0;
                timer->preset = 0;
                *timer_id_ptr = timer->timer_id;
                EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_allocate_tgroup0 -- success, ID = %d",timer->timer_id);

                eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

                EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_allocate_tgroup0 - timer_id = %d", timer->timer_id);

                return LSA_RET_OK;
            }
        }

        EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,"eps_timer_allocate_tgroup0 (0x%X, %d, %d, 0x%X) -- failed, no timers available",timer_id_ptr,timer_type,time_base,callback);

        eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_allocate_tgroup0 (0x%X, %d, %d, 0x%X) -- failed, parameter error",timer_id_ptr,timer_type,time_base,callback);

    eps_exit_critical_section(g_Eps_Timer.state0.lock_id);

    return LSA_RET_ERR_PARAM;
}

//******************************************************************************
//  Function: eps_timer_start
//
//  Description:
//
//      This function will start timing on a timer allocated using
//      eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to start
//      user_id      -- in:   Caller's identifier for the timeout instance
//      timeUnits    -- in:   timeout time in units of time_base at
//                            eps_timer_allocate
//
//  Return:
//
//      LSA_RET_OK                  The timer was stopped and has been started.
//      LSA_RET_OK_TIMER_RESTARTED  The timer was running and has been restarted
//      LSA_RET_ERR_PARAM           The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_start(
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          timeUnits)
{
    if (timer_id < 0x8000)
    {
        return ( eps_timer_start_tgroup0(timer_id, user_id, timeUnits) );
    }
    else
    {
        return ( eps_timer_start_tgroup1(timer_id, user_id, timeUnits) );
    }
}

//******************************************************************************
//  Function: eps_timer_stop
//
//  Description:
//
//      This function will stop timing on a timer started using eps_timer_start.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to stop
//
//  Return:
//
//      LSA_RET_OK                    The timer was stopped.
//      LSA_RET_OK_TIMER_NOT_RUNNING  The timer was not running
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_stop(
    LSA_TIMER_ID_TYPE   timer_id)
{
    if (timer_id < 0x8000)
    {
        return ( eps_timer_stop_tgroup0(timer_id) );
    }
    else
    {
        return ( eps_timer_stop_tgroup1(timer_id) );
    }
}

//******************************************************************************
//  Function: eps_timer_free
//
//  Description:
//
//      This function will release a timer allocated using eps_timer_allocate.
//
//  Parameters:
//
//      timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was released.
//      LSA_RET_ERR_TIMER_IS_RUNNING  The timer is running and was not released
//      LSA_RET_ERR_PARAM             There was a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_free(
    LSA_TIMER_ID_TYPE   timer_id)
{
    if (timer_id < 0x8000)
    {
        return ( eps_timer_free_tgroup0(timer_id) );
    }
    else
    {
        return ( eps_timer_free_tgroup1(timer_id) );
    }
}


//******************************************************************************
//  Function: internal_timer_timeout_tgroup1
//
//  Description:
//
//      Implement the time-keeping algorithm. The implementation assumes a small
//      number of timers. It runs in IRQ state1 in 1 ms tick.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
//#pragma ghs nothumb
static void internal_timer_timeout_tgroup1(union sigval sig)
{
    TimerRecord * timer;
    uint32_t      cur_tick;
    struct   itimerspec    its;
    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    int         retVal;
    #endif //#if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    LSA_UINT16  retValFree;
    LSA_UNUSED_ARG(sig);

    // if Shutdown is started, ignore this function
    if (g_Eps_Timer.state1.eState == eTimerReqStop)
    {
        eps_memset(&its, 0, sizeof(its));

        #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
        retVal = EPS_POSIX_TIMER_SETTIME((timer_t)g_Eps_Timer.tid1, 0, &its, NULL); // the timer disarm itself
        EPS_ASSERT(retVal == 0);
        #endif

                                                               // Other thread is cleaning up this thread
                                                               // the cleanup thread waits until this thread is stopped
                                                               // and cleans this thread afterwards.
        g_Eps_Timer.state1.eState = eTimerStopped;

        return;
    }

    if ((g_Eps_Timer.state1.eState == eTimerStopped) || (g_Eps_Timer.state1.eState == eTimerInit))
    {
        return; // timer can be triggered one time again after delete(eThStopped) and while init(eThInit)
    }

    eps_enter_critical_section(g_Eps_Timer.state1.lock_id);

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1
    eps_statistics_capture_start_value(stats_id_tgroup1);
    #endif

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> eps_timer internal_timer_timeout_tgroup1");

    cur_tick = g_Eps_Timer.state1.last_tick + EPS_TIMER_GROUP1_BASE_TICK;
    g_Eps_Timer.state1.last_tick = cur_tick;

    if (!PSI_FATAL_ERROR_OCCURED())
    {
        timer = g_Eps_Timer.state1.first1msTimer;
        while (NULL != timer)
        {
            if (cur_tick == timer->target)
            {
                EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "eps_timer1 calling 0x%X(%d, 0x%X)", timer->callback, timer->timer_id, timer->user_id.void_ptr);

                timer->target = cur_tick + timer->preset;
                if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                {
                    (*timer->callback) (timer->timer_id, timer->user_id);
                    continue;
                }
                internal_timer_stop_tgroup1(timer);
                (*timer->callback) (timer->timer_id, timer->user_id);
            }
            timer = timer->next;
        }

        if (cur_tick == g_Eps_Timer.state1.next_10ms_tick)
        {
            g_Eps_Timer.state1.next_10ms_tick = cur_tick + 10;
            timer = g_Eps_Timer.state1.first10msTimer;
            while (NULL != timer)
            {
                if (cur_tick == timer->target)
                {
                    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "eps_timer1 calling 0x%X(%d, 0x%X)", timer->callback, timer->timer_id, timer->user_id.void_ptr);

                    timer->target = cur_tick + timer->preset;
                    if (timer->timer_type == LSA_TIMER_TYPE_CYCLIC)
                    {
                        (*timer->callback) (timer->timer_id, timer->user_id);
                        continue;
                    }
                    internal_timer_stop_tgroup1(timer);
                    (*timer->callback) (timer->timer_id, timer->user_id);
                }
                timer = timer->next;
            }
        }
    }
    else
    {
        LSA_UINT32 i;

        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> eps_timer internal_timer_timeout_tgroup1 - else case");

        for (i = 0; i < countof(g_Eps_Timer.state1.timers); i++)
        {
            if (TS_FREE != g_Eps_Timer.state1.timers[i].status)
            {
                if (TS_RUNNING == g_Eps_Timer.state1.timers[i].status)
                {
                    internal_timer_stop_tgroup1(&g_Eps_Timer.state1.timers[i]);
                }
                retValFree = eps_timer_free_tgroup1(g_Eps_Timer.state1.timers[i].timer_id);
                EPS_ASSERT(LSA_RET_OK == retValFree);
            }
        }

        eps_timer_undo_init_tgroup1();
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< eps_timer internal_timer_timeout_tgroup1");

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1
    eps_statistics_capture_end_value(stats_id_tgroup1);
    #endif

    eps_exit_critical_section(g_Eps_Timer.state1.lock_id);
}
//#pragma ghs thumb

//******************************************************************************
//  Function: eps_timer_init_tgroup1
//
//  Description:
//
//      This function will initialize rhe IRQ-based timer subsystem.
//
//  Parameters:
//
//      None
//
//  Return:
//
//      LSA_RET_OK            The timer subsystem was initialized successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of resources
//******************************************************************************
LSA_UINT16 eps_timer_init_tgroup1( LSA_VOID )
{
    pthread_attr_t     thread_attr;
    struct sched_param sched_param;
    struct sigevent    event;
    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    struct itimerspec  timeout;
    #endif //#if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    int                ret;
    LSA_UINT16         i;

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_init_tgroup1 - begin");

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1
    eps_statistics_alloc_stats(&stats_id_tgroup1, "TimerGroup_1");
    #endif

    // initialize the state1 record
    eps_memset( &g_Eps_Timer.state1, 0, sizeof(g_Eps_Timer.state1) );

    ret = eps_alloc_critical_section( &g_Eps_Timer.state1.lock_id, LSA_FALSE );
    if (ret != LSA_RET_OK)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    g_Eps_Timer.state1.next_10ms_tick 	= 10;

    for (i = 0; i < countof(g_Eps_Timer.state1.timers); i++)
    {
        // timer_id has to be unique. timer_group0 and timer_group1 may not use the same timer_ids.
        // 0x8000 is used to distinguish between timer_ids from group0 and group1
        g_Eps_Timer.state1.timers[i].timer_id = i | 0x8000;
    }

    // initialize the "thread" attributes (we don't use a thread)
    eps_memset( &thread_attr, 0, sizeof( thread_attr ) );
    ret = EPS_POSIX_PTHREAD_ATTR_INIT( &thread_attr );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Enable explicitly to set individual scheduling parameters, otherwise they are taken from this thread
    ret = EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &thread_attr, PTHREAD_EXPLICIT_SCHED );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Set scheduling policy to FIFO
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY( &thread_attr, SCHED_FIFO );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    // Set scheduling priority
    eps_memset( &sched_param, 0, sizeof( sched_param ) );
    sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_TIMER;
    ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &thread_attr, &sched_param );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    //set thread name
    ret = pthread_attr_setname( &thread_attr, "TIMER_TGROUP1" );
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }
    #endif

    // fill event data and create the timer
    eps_memset( &event, 0, sizeof( event ) );
    event.sigev_notify = SIGEV_THREAD;

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    event.sigev_signo = SIGRTMIN;
    #else
    event.sigev_signo = SIGRT15;
    #endif

    event.sigev_notify_function   = internal_timer_timeout_tgroup1;
    event.sigev_notify_attributes = &thread_attr;
    event.sigev_value.sival_ptr   = NULL;

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    ret = EPS_POSIX_PTHREAD_CREATE(&g_Eps_Timer.thread_id_tg1, &thread_attr, &internal_timer_fdtimer_handler_tg1, NULL);
    EPS_POSIX_PTHREAD_SETNAME_NP(g_Eps_Timer.thread_id_tg1, "TIMER_TGROUP1");
    #else
    ret = EPS_POSIX_TIMER_CREATE( CLOCK_MONOTONIC, &event, (timer_t*)&g_Eps_Timer.tid1 );
    #endif // ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }


    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    // start the timer - EPS_TIMER_GROUP1_BASE_TICK ms cyclic timeout
    timeout.it_value.tv_sec     = 0; // EPS_TIMER_GROUP1_BASE_TICK / 1000 --> evaluates to 0
    timeout.it_value.tv_nsec    = 1000000 * (EPS_TIMER_GROUP1_BASE_TICK % 1000);
    timeout.it_interval.tv_sec  = 0; // EPS_TIMER_GROUP1_BASE_TICK / 1000 --> evaluates to 0
    timeout.it_interval.tv_nsec = 1000000 * (EPS_TIMER_GROUP1_BASE_TICK % 1000);
    ret = EPS_POSIX_TIMER_SETTIME( (timer_t)g_Eps_Timer.tid1, 0, &timeout, NULL );
    #endif //((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
    if (ret)
    {
        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_init_tgroup1 - done");

    return LSA_RET_OK;
}

LSA_VOID eps_timer_undo_init_tgroup1( LSA_VOID )
{
	LSA_UINT32      i;
    LSA_RESPONSE_TYPE retValFree;
    int retVal;
    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
	void *res;
    #endif //#if !((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_undo_init_tgroup1 - Timer-State=%u", g_Eps_Timer.state1.eState);

    // Tell the timeout function to stop it's work.
    g_Eps_Timer.state1.eState = eTimerReqStop;

    // Synchronize with other thread/timer wait until it is stopped
    while(g_Eps_Timer.state1.eState != eTimerStopped)
    {
        eps_tasks_sleep(1);
    }

    eps_enter_critical_section( g_Eps_Timer.state1.lock_id );
	for (i = 0; i < countof(g_Eps_Timer.state1.timers); i++)
	{
		EPS_ASSERT(g_Eps_Timer.state1.timers[i].status == TS_FREE);
	}

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    retVal = EPS_POSIX_PTHREAD_CANCEL(g_Eps_Timer.thread_id_tg1);
    EPS_ASSERT(0 == retVal);
    retVal = EPS_POSIX_PTHREAD_JOIN(g_Eps_Timer.thread_id_tg1, &res);
    EPS_ASSERT(0 == retVal);
    EPS_ASSERT(res == PTHREAD_CANCELED);
    g_Eps_Timer.thread_id_tg1 = 0;
    #else
    retVal = EPS_POSIX_TIMER_DELETE( (timer_t)g_Eps_Timer.tid1 );
    EPS_ASSERT(0 == retVal);
    #endif
    eps_exit_critical_section(g_Eps_Timer.state1.lock_id );

    // do the actual clean up after timer is stopped!
    retValFree = eps_free_critical_section( g_Eps_Timer.state1.lock_id );
    EPS_ASSERT(retValFree == LSA_RET_OK);
    g_Eps_Timer.state1.lock_id = 0;

    #ifdef EPS_TEST_MEASURE_PERFORMANCE_TGROUP1
    eps_statistics_free_stats(stats_id_tgroup1);
    #endif

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_undo_init_tgroup1 - Timer-State=%u", g_Eps_Timer.state1.eState);
}

//******************************************************************************
//  Function: eps_timer_allocate_tgroup1
//
//  Description:
//
//      This function will allocate an IRQ-based timer.
//
//  Parameters:
//
//      timer_id_ptr -- out:  points to returned value of timer's identifier
//      timer_type   -- in:   Type of timer -
//
//                                  LSA_TIMER_TYPE_ONE_SHOT or
//                                  LSA_TIMER_TYPE_CYCLIC
//
//      time_base    -- in:   Units of preset at start --
//
//                                  LSA_TIME_BASE_1MS
//                                  LSA_TIME_BASE_10MS,
//                                  LSA_TIME_BASE_100MS or
//                                  LSA_TIME_BASE_1S
//
//      callback     -- in:   pointer to function called on timeout.
//
//                              NOTE: Callback function is called in IRQ state1.
//
//  Return:
//
//      LSA_RET_OK            The timer was allocated successfully.
//      LSA_RET_ERR_NO_TIMER  The function failed due to lack of timer resources
//      LSA_RET_ERR_PARAM     The function failed due to a parameter error
//******************************************************************************
LSA_UINT16 eps_timer_allocate_tgroup1(
    LSA_TIMER_ID_TYPE * timer_id_ptr,
    LSA_UINT16          timer_type,
    LSA_UINT16          time_base,
    EPS_TIMER_CBF       callback )
{
    LSA_UINT16 i;

    eps_enter_critical_section(g_Eps_Timer.state1.lock_id);

    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,">> eps_timer_allocate_tgroup1 (0x%X, %d, %d, 0x%X)",timer_id_ptr,timer_type,time_base,callback);

    if ( ( (timer_type == LSA_TIMER_TYPE_ONE_SHOT) ||
           (timer_type == LSA_TIMER_TYPE_CYCLIC) ) &&
           ( (time_base == LSA_TIME_BASE_1MS) ||
             (time_base == LSA_TIME_BASE_10MS) ) &&
           (NULL != callback) )
    {
        for (i = 0; i < countof(g_Eps_Timer.state1.timers); i++)
        {
            if (TS_FREE == g_Eps_Timer.state1.timers[i].status)
            {
                TimerRecord * timer = &(g_Eps_Timer.state1.timers[i]);

                timer->prior = NULL;
                timer->next = NULL;
                timer->status = TS_STOPPED;
                timer->timer_type = timer_type;
                timer->time_base = time_base;
                timer->callback = callback;
                eps_memset(&(timer->user_id), 0, sizeof(timer->user_id));
                timer->target = 0;
                timer->preset = 0;
                *timer_id_ptr = timer->timer_id;
                EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"<< eps_timer_allocate_tgroup1 -- success, ID = %d",timer->timer_id);

                eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

                EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">> eps_timer_allocate_tgroup1 - timer_id = %d", timer->timer_id);

                return LSA_RET_OK;
            }
        }

        EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,"eps_timer_allocate_tgroup1 (0x%X, %d, %d, 0x%X) -- failed, no timers available",timer_id_ptr,timer_type,time_base,callback);

        eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

        return LSA_RET_ERR_NO_TIMER;
    }

    EPS_SYSTEM_TRACE_04(0,LSA_TRACE_LEVEL_WARN,">< eps_timer_allocate_tgroup1 (0x%X, %d, %d, 0x%X) -- failed, parameter error",timer_id_ptr,timer_type,time_base,callback);

    eps_exit_critical_section(g_Eps_Timer.state1.lock_id);

    return LSA_RET_ERR_PARAM;
}

//------------------------------------------------------------------------------
// Implementation - Additional timer functions
//------------------------------------------------------------------------------

//******************************************************************************
//  Function: eps_timer_init_sys_timer
//
//  Description:
//
//      Initialize the eps system timer interface
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_timer_init_sys_timer ( LSA_VOID )
{
    LSA_UINT16        entry;
    LSA_RESPONSE_TYPE retVal;

    // not initialized yet?
    EPS_ASSERT( !g_Eps_sys_timer.is_running );

    // allocate the critical section for the sys timer
    retVal = eps_alloc_critical_section(&g_Eps_sys_timer.enter_exit, LSA_FALSE);
    EPS_ASSERT( retVal == LSA_RET_OK );

    // initialize the structures to default values
    for ( entry = 0; entry < EPS_TIMER_MAX_SYS_TIMER; entry++ )
    {
        g_Eps_sys_timer.sysTimer[entry].is_used       = LSA_FALSE;
        g_Eps_sys_timer.sysTimer[entry].sys_timer_id  = 0;
        g_Eps_sys_timer.sysTimer[entry].sys_timer     = 0;
        g_Eps_sys_timer.sysTimer[entry].interval_ms   = 0;
        g_Eps_sys_timer.sysTimer[entry].thread_id     = 0;
        g_Eps_sys_timer.sysTimer[entry].cbf           = LSA_NULL;
    }

    g_Eps_sys_timer.is_running = LSA_TRUE; // Now initialized

    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_timer_init_sys_timer(): setup done, max sys_timer(%u)", EPS_TIMER_MAX_SYS_TIMER );
}

//******************************************************************************
//  Function: eps_timer_undo_init_sys_timer
//
//  Description:
//
//      Deinitialize the eps sys timer
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_timer_undo_init_sys_timer ( LSA_VOID )
{
    LSA_UINT16 entry;
    LSA_RESPONSE_TYPE retValFree;

    // check if eps sys timer are initialized
    EPS_ASSERT( g_Eps_sys_timer.is_running );

    for ( entry = 0; entry < EPS_TIMER_MAX_SYS_TIMER; entry++ )
    {
        // check all sys timer are freed
        EPS_ASSERT( !g_Eps_sys_timer.sysTimer[entry].is_used );
    }

    g_Eps_sys_timer.is_running = LSA_FALSE;

    retValFree = eps_free_critical_section(g_Eps_sys_timer.enter_exit);
    EPS_ASSERT(retValFree == LSA_RET_OK);

    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_timer_undo_init_sys_timer(): cleanup done, max sys_timer(%u)", EPS_TIMER_MAX_SYS_TIMER );
}

//******************************************************************************
//  Function: eps_timer_allocate_sys_timer
//
//  Description:
//
//      This function will allocate a timer from the operating system
//
//  Parameters:
//
//      sys_timer_id_ptr  -- in/out:   timer identifier
//      pTimerName        -- in    :   Name of the timer
//      lTimerCycleMs     -- in    :   Timer cycle in ms
//      callback_function -- in    :   callback function called at time out
//
//  Return:
//
//      LSA_RET_OK                    The timer was created
//      LSA_RET_ERR_NO_TIMER          No timer available
//******************************************************************************
LSA_UINT16 eps_timer_allocate_sys_timer(LSA_UINT16*         sys_timer_id_ptr,
                                        const LSA_CHAR*     pTimerName,
                                        LSA_UINT32          lTimerCycleMs,
                                        EPS_SYS_TIMER_CBF   callback_function,
                                        LSA_VOID*           pArgs)
{
    LSA_UINT16        entry;
    LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_NO_TIMER;

    // initialized and return pointer is ok?
    EPS_ASSERT( sys_timer_id_ptr != LSA_NULL );
    EPS_ASSERT( g_Eps_sys_timer.is_running );

    eps_enter_critical_section(g_Eps_sys_timer.enter_exit);

    // check if another sys timer is available
    for ( entry = 0; entry < EPS_TIMER_MAX_SYS_TIMER; entry++ )
    {
        if ( !g_Eps_sys_timer.sysTimer[entry].is_used )
        {
            pthread_attr_t     thread_attr;
            struct sched_param sched_param;
            struct sigevent    event;
            int                ret;
            timer_t            internalSysTimerId = 0;
            #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
            timer_t*           internalSysTimerIdPtr = &internalSysTimerId;
            struct itimerspec  timeout;
            #endif
        
            EPS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_allocate_sys_timer - begin");        
       
            // initialize the "thread" attributes (we don't use a thread)
            eps_memset( &thread_attr, 0, sizeof( thread_attr ) );
            ret = EPS_POSIX_PTHREAD_ATTR_INIT( &thread_attr );
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }
            
            // Enable explicitly to set individual scheduling parameters, otherwise they are taken from this thread
            ret = EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &thread_attr, PTHREAD_EXPLICIT_SCHED );
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }

            // Set scheduling policy to FIFO
            ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY( &thread_attr, SCHED_FIFO );
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }

            // Set scheduling priority
            eps_memset( &sched_param, 0, sizeof( sched_param ) );
            sched_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE;
            ret = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &thread_attr, &sched_param );
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }

            #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
            //set thread name
            ret = pthread_attr_setname( &thread_attr, pTimerName );
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }
            #endif

            // fill event data and create the timer
            eps_memset( &event, 0, sizeof( event ) );
            event.sigev_notify = SIGEV_THREAD;

            #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)) // TODO -> move SIGNO to Posix-IF
            event.sigev_signo = SIGRTMIN;
            #else
            event.sigev_signo = SIGRT15;
            #endif

            event.sigev_notify_function   = callback_function;
            event.sigev_notify_attributes = &thread_attr;
            event.sigev_value.sival_ptr   = pArgs;
            #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)) // TODO -> move SIGNO to Posix-IF
            #else
            ret = EPS_POSIX_TIMER_CREATE( CLOCK_MONOTONIC, &event, internalSysTimerIdPtr );
            #endif
            
            if (ret)
            {
                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }

            #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1)) // TODO -> move SIGNO to Posix-IF
            // start the timer - calculate the cyclic timeout
            timeout.it_value.tv_sec     =  lTimerCycleMs / 1000;
            timeout.it_value.tv_nsec    = (lTimerCycleMs % 1000) * 1000000;
            timeout.it_interval.tv_sec  =  lTimerCycleMs / 1000;
            timeout.it_interval.tv_nsec = (lTimerCycleMs % 1000) * 1000000;
            
            ret = EPS_POSIX_TIMER_SETTIME( internalSysTimerId, 0, &timeout, NULL );
            
            if (ret)
            {
                // delete the sys timer
                ret = EPS_POSIX_TIMER_DELETE( internalSysTimerId );
                if (ret)
                {
                    eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                    return LSA_RET_ERR_NO_TIMER;
                }

                eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                return LSA_RET_ERR_NO_TIMER;
            }   
            #endif     
                       
            g_Eps_sys_timer.sysTimer[entry].sys_timer_id  = entry;
            g_Eps_sys_timer.sysTimer[entry].sys_timer     = internalSysTimerId;
            g_Eps_sys_timer.sysTimer[entry].cbf           = callback_function;
            g_Eps_sys_timer.sysTimer[entry].interval_ms   = lTimerCycleMs;
            g_Eps_sys_timer.sysTimer[entry].thread_id     = 0;
            g_Eps_sys_timer.sysTimer[entry].is_used       = LSA_TRUE;
            
            *sys_timer_id_ptr = entry;

            #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)) 
            ret = EPS_POSIX_PTHREAD_CREATE(&g_Eps_sys_timer.sysTimer[entry].thread_id, &thread_attr, internal_sys_timer_handler, &g_Eps_sys_timer.sysTimer[entry]);
            EPS_POSIX_PTHREAD_SETNAME_NP(g_Eps_sys_timer.sysTimer[entry].thread_id, pTimerName);
            if (ret)
                {
                    eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
                    // set internal values to default
                    g_Eps_sys_timer.sysTimer[entry].sys_timer      = 0;
                    g_Eps_sys_timer.sysTimer[entry].sys_timer_id   = 0;
                    g_Eps_sys_timer.sysTimer[entry].cbf            = LSA_NULL;
                    g_Eps_sys_timer.sysTimer[entry].interval_ms    = 0;
                    g_Eps_sys_timer.sysTimer[entry].thread_id      = 0;
                    g_Eps_sys_timer.sysTimer[entry].is_used        = LSA_FALSE;
                    return LSA_RET_ERR_NO_TIMER;
                } 
            #endif
            
            EPS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_allocate_sys_timer - done: sys_timer_id_ptr: (0x%X) lTimerCycleMs: (%d)", *sys_timer_id_ptr, lTimerCycleMs);

            retVal = LSA_RET_OK;
            break;
        }
    }

    eps_exit_critical_section(g_Eps_sys_timer.enter_exit);

    return (retVal);
}

//******************************************************************************
//  Function: eps_timer_change_sys_timer
//
//  Description:
//
//      This function will change the configuration of a timer from
//      the operating system
//
//  Parameters:
//
//      sys_timer_id     -- in:   timer identifier of timer to change
//      lTimerCycleMs    -- in:   new timer cycle in ms
//
//  Return:
//
//      LSA_RET_OK                    The timer was changed
//      LSA_RET_ERR_NO_TIMER          No timer available
//******************************************************************************
LSA_RESPONSE_TYPE eps_timer_change_sys_timer(LSA_UINT16    sys_timer_id,
                                             LSA_UINT32    lTimerCycleMs)
{
    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1)) 
    struct itimerspec  timeout;
    int                ret;
    #endif //#if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1)) 

    EPS_ASSERT(g_Eps_sys_timer.is_running );
    EPS_ASSERT(sys_timer_id < EPS_TIMER_MAX_SYS_TIMER );
    EPS_ASSERT(g_Eps_sys_timer.sysTimer[sys_timer_id].is_used );

    eps_enter_critical_section(g_Eps_sys_timer.enter_exit);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_change_sys_timer - begin: sys_timer_id (0x%X)", sys_timer_id);


    g_Eps_sys_timer.sysTimer[sys_timer_id].interval_ms   = lTimerCycleMs;

    #if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1)) 
     
    // calculate the new cyclic timeout and activate it
    timeout.it_value.tv_sec     =  lTimerCycleMs / 1000;
    timeout.it_value.tv_nsec    = (lTimerCycleMs % 1000) * 1000000;
    timeout.it_interval.tv_sec  =  lTimerCycleMs / 1000;
    timeout.it_interval.tv_nsec = (lTimerCycleMs % 1000) * 1000000;

    ret = EPS_POSIX_TIMER_SETTIME( g_Eps_sys_timer.sysTimer[sys_timer_id].sys_timer, 0, &timeout, NULL );
    if (ret)
    {
        eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
        return LSA_RET_ERR_NO_TIMER;
    }   
    #endif //#if ((EPS_PLF != EPS_PLF_LINUX_X86) && (EPS_PLF != EPS_PLF_LINUX_IOT2000) && (EPS_PLF != EPS_PLF_LINUX_SOC1)) 
    
    eps_exit_critical_section(g_Eps_sys_timer.enter_exit);

    EPS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_change_sys_timer - done: timer_id_ptr (0x%X) new-lTimerCycleMs (%d)", sys_timer_id, lTimerCycleMs);

    return LSA_RET_OK;
}

//******************************************************************************
//  Function: eps_timer_free_sys_timer
//
//  Description:
//
//      This function will free a timer from the operating system
//
//  Parameters:
//
//      sys_timer_id     -- in:   timer identifier of timer to release
//
//  Return:
//
//      LSA_RET_OK                    The timer was deleted
//      LSA_RET_ERR_NO_TIMER          No timer was deleted
//******************************************************************************
LSA_RESPONSE_TYPE eps_timer_free_sys_timer( LSA_UINT16  sys_timer_id )
{
    int ret;

    EPS_ASSERT( g_Eps_sys_timer.is_running );
    EPS_ASSERT( g_Eps_sys_timer.sysTimer[sys_timer_id].is_used );

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,">> eps_timer_free_sys_timer - begin: sys_timer_id (0x%X)", sys_timer_id);

    eps_enter_critical_section(g_Eps_sys_timer.enter_exit);

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)) 
    ret = EPS_POSIX_PTHREAD_CANCEL(g_Eps_sys_timer.sysTimer[sys_timer_id].thread_id);
    #else
    // delete the sys timer
    ret = EPS_POSIX_TIMER_DELETE(g_Eps_sys_timer.sysTimer[sys_timer_id].sys_timer );
    #endif
    if (ret)
    {
        eps_exit_critical_section(g_Eps_sys_timer.enter_exit);
        return LSA_RET_ERR_NO_TIMER;
    }

    // set internal values to default
    g_Eps_sys_timer.sysTimer[sys_timer_id].sys_timer      = 0;
    g_Eps_sys_timer.sysTimer[sys_timer_id].sys_timer_id   = 0;
    g_Eps_sys_timer.sysTimer[sys_timer_id].cbf            = LSA_NULL;
    g_Eps_sys_timer.sysTimer[sys_timer_id].interval_ms    = 0;
    g_Eps_sys_timer.sysTimer[sys_timer_id].thread_id      = 0;
    g_Eps_sys_timer.sysTimer[sys_timer_id].is_used        = LSA_FALSE;

    eps_exit_critical_section(g_Eps_sys_timer.enter_exit);

    EPS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"<< eps_timer_free_sys_timer - done: sys_timer_id (0x%X)", sys_timer_id);

    return LSA_RET_OK;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
