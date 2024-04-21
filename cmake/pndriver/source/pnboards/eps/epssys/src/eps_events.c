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
/*  F i l e               &F: eps_events.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Event Api                                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20004
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include "eps_sys.h"
#include "eps_trc.h"
#include "eps_locks.h"
#include "eps_events.h"

#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*-----------------------------------------------------------------*/
/* Defines                                                         */
/*-----------------------------------------------------------------*/



/*-----------------------------------------------------------------*/
/* Types                                                           */
/*-----------------------------------------------------------------*/

typedef struct
 {
	LSA_BOOL             is_used;
	LSA_UINT16           sema_id;
	sem_t                event_sema;
 }EPS_EVENT_ENTRY_TYPE;


typedef struct  
 { 
	LSA_BOOL                is_running;                           // Initialized yes/no
	LSA_UINT16              enter_exit;                           // Own reentrance lock
	EPS_EVENT_ENTRY_TYPE    events[EPS_WAIT_EVENT_MAX+1];         // Wait event list
} EPS_WAIT_EVENT_INSTANCE_TYPE;


/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/

static EPS_WAIT_EVENT_INSTANCE_TYPE g_Eps_events = {0};
//lint -esym(457, g_Eps_events)
/*-----------------------------------------------------------------*/
/* Implementation of eps events (Wait events)                      */
/*-----------------------------------------------------------------*/

LSA_VOID eps_events_init( LSA_VOID )
{
	LSA_UINT16        entry;
	LSA_RESPONSE_TYPE retVal;

	EPS_ASSERT( !g_Eps_events.is_running );

	retVal = eps_alloc_critical_section(&g_Eps_events.enter_exit, LSA_FALSE);
	EPS_ASSERT( retVal == LSA_RET_OK );

	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
	    g_Eps_events.events[entry].is_used = LSA_FALSE;
	    g_Eps_events.events[entry].sema_id = 0;
	}

	g_Eps_events.is_running = LSA_TRUE; // Now initialized
}

/*-----------------------------------------------------------------*/
LSA_VOID eps_events_undo_init( LSA_VOID )
{
	LSA_UINT16 entry;
    LSA_UINT16 retVal;

	EPS_ASSERT(g_Eps_events.is_running );

	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
		// check all user wait events are freed 
		EPS_ASSERT( !g_Eps_events.events[entry].is_used );
	}

	g_Eps_events.is_running = LSA_FALSE;

	retVal = eps_free_critical_section(g_Eps_events.enter_exit);
    EPS_ASSERT( LSA_RET_OK == retVal );

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
	"eps_events_undo_init(): locks cleanup done, max locks(%u)", 
	EPS_WAIT_EVENT_MAX );
}

/*-----------------------------------------------------------------*/
/* Implementation of event adaption                                */
/*-----------------------------------------------------------------*/

LSA_RESPONSE_TYPE eps_alloc_event( LSA_UINT16 *event_id_ptr)
{
	LSA_UINT16        entry;
	LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;
	int retValSem;

	EPS_ASSERT( event_id_ptr != LSA_NULL );
	EPS_ASSERT( g_Eps_events.is_running );

	eps_enter_critical_section(g_Eps_events.enter_exit);

	// Note: 0 is reserved for EPS sys wait event
	for ( entry = 0; entry <= EPS_WAIT_EVENT_MAX; entry++ )
	{
    
		if ( !g_Eps_events.events[entry].is_used )
		{
			retValSem =  EPS_POSIX_SEM_INIT(&g_Eps_events.events[entry].event_sema, 0, 0);
			EPS_ASSERT(retValSem == 0);

			g_Eps_events.events[entry].sema_id = entry;
      
			g_Eps_events.events[entry].is_used = LSA_TRUE;

			*event_id_ptr = entry;

			retVal = LSA_RET_OK;
			break;
		}
	}

	eps_exit_critical_section(g_Eps_events.enter_exit);

	return ( retVal );
}

/*-----------------------------------------------------------------*/

LSA_RESPONSE_TYPE eps_free_event( LSA_UINT16 event_id )
{
	LSA_RESPONSE_TYPE retVal = LSA_RET_OK;
	int retValSem;

	EPS_ASSERT( g_Eps_events.is_running );

	eps_enter_critical_section(g_Eps_events.enter_exit);

	// Note: 0 is reserved for EPS sys wait event
	if ( g_Eps_events.events[event_id].is_used == LSA_TRUE )  // allocated ?
	{

		retValSem = EPS_POSIX_SEM_DESTROY(&g_Eps_events.events[event_id].event_sema);
		EPS_ASSERT( retValSem == 0 );
		g_Eps_events.events[event_id].sema_id = 0;

		g_Eps_events.events[event_id].is_used = LSA_FALSE;
    
		retVal = LSA_RET_OK;
	}
	else
	{
		retVal = LSA_RET_ERR_PARAM;
	}

	eps_exit_critical_section(g_Eps_events.enter_exit);

	return ( retVal );
}

/*-----------------------------------------------------------------*/
LSA_VOID eps_set_event( LSA_UINT16 event_id )
{
	int rc = -1;

	EPS_ASSERT( g_Eps_events.is_running );
	EPS_ASSERT( event_id <= EPS_WAIT_EVENT_MAX );  // in range
  
	// unlock a semaphore 
	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_POST(&g_Eps_events.events[event_id].event_sema);
	}

}

/*-----------------------------------------------------------------*/
LSA_VOID eps_reset_event( LSA_UINT16 event_id )
{
	int rc = -1;

	EPS_ASSERT( g_Eps_events.is_running );
	EPS_ASSERT( event_id <= EPS_WAIT_EVENT_MAX );   // in range

	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_DESTROY(&g_Eps_events.events[event_id].event_sema);
	}
  
	rc = -1;
	while (!(rc == 0))
	{
		rc = EPS_POSIX_SEM_INIT(&g_Eps_events.events[event_id].event_sema,0,0);
	}
}

/*-----------------------------------------------------------------*/
LSA_RESPONSE_TYPE eps_wait_event( LSA_UINT16 event_id, LSA_UINT16 wait_sec )
{
    LSA_RESPONSE_TYPE retVal = LSA_RET_OK;
	int rc;
  
	EPS_ASSERT( g_Eps_events.is_running );
	EPS_ASSERT( event_id <= EPS_WAIT_EVENT_MAX );  // in range

	if  (PSI_EVENT_NO_TIMEOUT == wait_sec)
	    // wait forever
	{
		rc = EPS_POSIX_SEM_WAIT(&g_Eps_events.events[event_id].event_sema);
	}
	else
	    // wait with timeout
	{
	    struct timespec WaitSpec;
        int retValGetTime;
	    //Attention: tv_sec and tv_nsec are of type long!
        retValGetTime = EPS_POSIX_CLOCK_GETTIME(CLOCK_REALTIME, &WaitSpec);
        EPS_ASSERT( 0 == retValGetTime );
	    WaitSpec.tv_sec  += wait_sec;

	    rc = EPS_POSIX_SEM_TIMEDWAIT(&g_Eps_events.events[event_id].event_sema, &WaitSpec);
	}
	
    if  (rc == -1)
        // error occured (e.g. timeout)
    {
        retVal = LSA_RET_ERR_RESOURCE;
    }
	
	return retVal;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
