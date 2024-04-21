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
/*  F i l e               &F: eps_trace_timer_if.c                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Trace Timer Interface			                                     */
/*                                                                           */
/*    - eps_trace_timer_if_register() : registers a timer to administration  */
/*                                                                           */
/*    - eps_trace_timer_start()     : starts the registered timer            */
/*    - eps_trace_timer_stop()      : stops the registered timer             */
/*    - eps_trace_timer_init()      : initialization of TT_IF                */
/*    - eps_trace_timer_undo_init() : uninstalls the registered timer        */
/*    - eps_trace_timer_get_time()  : reads the time of registered timer     */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20077
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/** Includes for the Trace Timer Interface
 */
#include <eps_rtos.h>           /* OS for Thread Api (open(), ...)  */
#include <eps_sys.h>            /* Types / Prototypes / Funcs    */
#include <eps_trc.h>            /* Tracing                       */
#include <eps_trace_timer_if.h> /* Trace Timer Interface         */
#include <eps_pndrvif.h>        /* Defines ex. EPS_PN_DRV_RET_OK   */


/** Global definitions for the Trace Timer Interface
 */
static EPS_TRACE_TIMER_IF_TYPE     g_EpsTraceTimerIf;
static EPS_TRACE_TIMER_IF_PTR_TYPE g_pTraceTimerIf = LSA_NULL;


/** Implementation (functions) of the Trace Timer Interface
 */
//----------------------------------------------------------------------------------------------------
/**
 * Registers the Timer Modul in administration of EPS Trace Timer Interface.
 * 
 * @param [in] pTtIf     - pointer to the structure that contains the function pointers of the implementation
 * @return LSA_VOID
 */
LSA_VOID   eps_trace_timer_if_register (EPS_TRACE_TIMER_IF_CONST_PTR_TYPE pTtIf)
{
    /// check function-IN-parameter and all function-pointers of interface
    EPS_ASSERT(pTtIf            != LSA_NULL);
    EPS_ASSERT(pTtIf->start     != LSA_NULL);
    EPS_ASSERT(pTtIf->stop      != LSA_NULL);
    EPS_ASSERT(pTtIf->get_time  != LSA_NULL);
    EPS_ASSERT(pTtIf->uninstall != LSA_NULL);
    
    

    /// ? is Trace Timer Interface already in use ?
    if (g_pTraceTimerIf != LSA_NULL)            /* Trace Timer Interface installed */
    {
        EPS_FATAL("eps_trace_timer_if_register() is called but is already in use");
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_trace_timer_if_register() - registered timer driver");
        /// Trace Timer Interface is unused -> Register new Trace Timer Interface
        g_pTraceTimerIf = &g_EpsTraceTimerIf;   /* Init pointer to Global Instance */
        *g_pTraceTimerIf = *pTtIf;              /* Install Trace Timer Interface   */
    }
}


//----------------------------------------------------------------------------------------------------
/**
 * Starts the Trace Timer.
 * 
 * @param [in] LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID   eps_trace_timer_start ()
{
    // locals
    LSA_UINT16  retVal;
   

    /// ? is Trace Timer Interface in use ?
    if (g_pTraceTimerIf != LSA_NULL)                /* Trace Timer Interface installed */
    {
        /// Trace Timer Interface is in use -> timer start()
        retVal = g_pTraceTimerIf->start();      /* start the timer */
        EPS_ASSERT(retVal == 0);
    }
    else
    {
        EPS_FATAL("eps_trace_timer_start() is called but is not in use");
    }
}

//----------------------------------------------------------------------------------------------------
/**
 * Stops the Trace Timer.
 * 
 * @param [in] LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID   eps_trace_timer_stop ()
{
    // locals
    LSA_UINT16  retVal;

    /// ? is Trace Timer Interface in use ?
    if (g_pTraceTimerIf != LSA_NULL)            /* Trace Timer Interface installed */
    {
        /// Trace Timer Interface is in use -> timer start()
        retVal = g_pTraceTimerIf->stop();                /* stop the timer */
        EPS_ASSERT(retVal == 0);
    }
    else
    {
        EPS_FATAL("eps_trace_timer_stop() is called but is not in use");
    }
}

//----------------------------------------------------------------------------------------------------
/**
 * Undo initialization of Trace Timer Interface.
 * 
 * @param [in] LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID   eps_trace_timer_init ()
{
    /// nothing to do
}

//----------------------------------------------------------------------------------------------------
/**
 * De-Registers the Timer Modul in administration of EPS Trace Timer Interface.
 * 
 * @param [in] LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID   eps_trace_timer_undo_init ()
{
    // locals
    LSA_UINT16  retVal;

    /// ? is Trace Timer Interface in use ?
    if (g_pTraceTimerIf != LSA_NULL)            /* Trace Timer Interface installed */
    {
        /// Trace Timer Interface is in use -> uninstall() and delete Trace Timer Module
        retVal = g_pTraceTimerIf->uninstall();  /* uninstall the timer */
        EPS_ASSERT(retVal == EPS_PN_DRV_RET_OK);
      
        g_pTraceTimerIf = LSA_NULL;             /* delete Trace Timer Interface */
    }
    else
    {
        EPS_FATAL("eps_trace_timer_undo_init() is called but is not in use");
    }
}

//----------------------------------------------------------------------------------------------------
/**
 * Returns the actual Trace Time.
 * 
 * @param [in] LSA_VOID
 * @return LSA_UINT64   - if Trace Timer Interface registered, then return a 64 Bit Time
 *                        if Trace Timer Interface unregistered, then return 0
 */
LSA_UINT64 eps_trace_timer_get_time ()
{
    // locals
    LSA_UINT64  currentTime = 0;

    
    /// ? is Trace Timer Interface registered ?
    if (g_pTraceTimerIf != LSA_NULL)
    {
        /// Trace Timer Interface is registered, so read currentTime
        currentTime = g_pTraceTimerIf->get_time();

        return (currentTime);
    }
    else
    {
        /// Trace Timer Interface is not registered -> return (0)
        return (currentTime); /* return 0, bcs no Trace Timer Interface registered */
    }
}
