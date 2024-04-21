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
/*  F i l e               &F: eps_locks.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Locking adaption                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20005
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID

#include "eps_sys.h"
#include "eps_trc.h"
#include "eps_locks.h"
#include "eps_rtos.h"


/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Types                                                                     */
/*---------------------------------------------------------------------------*/

typedef struct
{
    LSA_BOOL            is_used; 
    pthread_mutex_t     enter_exit;
}EPS_LOCK_ENTRY_TYPE;


typedef struct
{
    LSA_BOOL                 is_running;                // Initialized yes/no
    pthread_mutex_t          enter_exit;                // Own reentrance lock
    EPS_LOCK_ENTRY_TYPE      locks[EPS_LOCK_ID_MAX+1];   // Lock entry list
}EPS_LOCK_INSTANCE_TYPE;


/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/

static EPS_LOCK_INSTANCE_TYPE g_Eps_locks = {0};

/**
* Initializes the eps_lock module.
* 
* A number of EPS_LOCK_ID_MAX locks are allocated and managed by eps_locks.
* 
* @param LSA_VOID
* @return
*/
LSA_VOID eps_locks_init( LSA_VOID )
{
    LSA_UINT16  entry;
    
    #if (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
    pthread_mutexattr_t mutex_attr;
    EPS_ASSERT( !g_Eps_locks.is_running );
    pthread_mutexattr_init(&mutex_attr);
    EPS_ASSERT(0 == pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE));
    pthread_mutex_init(&g_Eps_locks.enter_exit, &mutex_attr);
	#else   
    int retVal;
    EPS_ASSERT( !g_Eps_locks.is_running );    
    // guard this code section against mutal access
    retVal= EPS_POSIX_PTHREAD_MUTEX_INIT(&g_Eps_locks.enter_exit, 0);
    EPS_ASSERT(0 == retVal);
	#endif
    
    // initialize all locks
    for ( entry = 0; entry <= EPS_LOCK_ID_MAX; entry++ )
    {
        g_Eps_locks.locks[entry].is_used = LSA_FALSE;
    }

    g_Eps_locks.is_running = LSA_TRUE; // Now initialized

    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_locks_init(): locks init done, max locks(%u)", EPS_LOCK_ID_MAX );
}

/**
* Undo initialization of the eps_locks implementation
* @param LSA_VOID
* @return
*/
LSA_VOID eps_locks_undo_init( LSA_VOID )
{
    LSA_UINT16  entry;
    int         retVal;

    EPS_ASSERT( g_Eps_locks.is_running );

    for ( entry = 0; entry <= EPS_LOCK_ID_MAX; entry++ )
    {
        // check all locks are freed 
		EPS_ASSERT(!g_Eps_locks.locks[entry].is_used);
    }

    g_Eps_locks.is_running = LSA_FALSE;

    retVal = EPS_POSIX_PTHREAD_MUTEX_DESTROY( &g_Eps_locks.enter_exit );
    EPS_ASSERT(0 == retVal);

    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_locks_undo_init(): locks cleanup done, max locks(%u)", EPS_LOCK_ID_MAX );
}

/**
* This function allocates a critical section using the POSIX API pthread_mutex_init.
* A recursive lock may be allocated. Recursive means that the same thread may allocate the same lock again without 
* running into a self deadlock. 
* This also means, that eps_enter_critical_section can not be used to guard a critical section against mutal access from the same thread.
* 
* In the current implementation, the Windows implementation always allocates a recursive lock since the API does not 
* support non recursive locks.
* 
* @see eps_free_critical_section     - this function undoes the allocation
* @see pthread_mutex_init            - this function is called by EPS.
* 
* @param [out] lock_id_ptr           - handle to the lock
* @param [in] isPtreadMutexRecursive - LSA_TRUE: Recursive lock, LSA_FALSE: non recursive lock
* @param [in] protectDifferentPrio   - LSA_TRUE: Lock is used by threads of different priority, LSA_FALSE: lock is only used by threads with same priority
* @param [in] prioCeiling            - LSA_TRUE: Raise prio to ceiling for lock, LSA_FALSE: No/Normal prio inheritance is enough for this lock 
* @return #LSA_RET_ERR_RESOURCE      - all possible locks are already taken
* @return #LSA_RET_OK                - Lock was successfully created.
*/
static LSA_RESPONSE_TYPE eps_alloc_critical_section_internal( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive, LSA_BOOL protectDifferentPrio, LSA_BOOL prioCeiling)
{
    LSA_UINT16        	    entry;
    LSA_RESPONSE_TYPE 	    retVal = LSA_RET_ERR_RESOURCE;
	LSA_INT32 			    retTmp;
	pthread_mutexattr_t     mutex_attr;

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    isPtreadMutexRecursive = LSA_TRUE;
    #endif  

    EPS_ASSERT( lock_id_ptr != 0 );
    EPS_ASSERT( g_Eps_locks.is_running );

    retTmp = EPS_POSIX_PTHREAD_MUTEX_LOCK ( &g_Eps_locks.enter_exit );
	EPS_ASSERT(0 == retTmp);

    for ( entry = 1; entry <= EPS_LOCK_ID_MAX; entry++ ) // Note: 0 is reserved (init val)
    {
    	if ( !g_Eps_locks.locks[entry].is_used )
        {
			retTmp = pthread_mutexattr_init(&mutex_attr);
			EPS_ASSERT(0 == retTmp);
            #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			if(protectDifferentPrio)
			{
                LSA_UNUSED_ARG(prioCeiling); // prioCeiling is always used in Windows for protectDifferentPrio!
				retTmp = pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_PROTECT);
				EPS_ASSERT(0 == retTmp);
			}
			LSA_UNUSED_ARG(isPtreadMutexRecursive); // all mutexes in Windows are recursive!
			#else
                #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (ADN_MTX_MODE == ADN_MTX_MODE_PRIO) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
				if(protectDifferentPrio && prioCeiling)
				{
					retTmp = pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_PROTECT);
					EPS_ASSERT(0 == retTmp);
                    #if defined(EPS_USE_RTOS_ADONIS)
                    // RT_MAX is maximum priority in Adonis
					retTmp = pthread_mutexattr_setprioceiling(&mutex_attr, RT_PMAX);
                    #else
                    {
                        // get maximum priority for Linux
                        LSA_INT maxPrio = sched_get_priority_max(SCHED_FIFO);
                        EPS_ASSERT(0 != maxPrio);
                        // set mutex prio ceiling to max prio 
                        retTmp = pthread_mutexattr_setprioceiling(&mutex_attr, maxPrio);
    					EPS_ASSERT(0 == retTmp);
                    }
                    #endif
				}
				else
				{
					retTmp = pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT);
					EPS_ASSERT(0 == retTmp);
				}
                #else
                // TODO Activate mutex prio mode for adonis!
                // for example pntrace mutex can be used by threads of different priority, 
                // -> therefore currently pntrace can lead to priorty inversion in adonis!
				LSA_UNUSED_ARG(protectDifferentPrio);
				LSA_UNUSED_ARG(prioCeiling);
                #endif
        	if(isPtreadMutexRecursive)
            {
                // set mutex attributes to recursive
                retTmp = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
				EPS_ASSERT(0 == retTmp);
            }
            #endif // #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			// init mutex with mutex_attr
			retTmp = EPS_POSIX_PTHREAD_MUTEX_INIT( &g_Eps_locks.locks[entry].enter_exit, &mutex_attr);
			EPS_ASSERT(0 == retTmp);
        	
			g_Eps_locks.locks[entry].is_used = LSA_TRUE;

            *lock_id_ptr = entry;
            retVal = LSA_RET_OK;
            break;
        }     
    }

    retTmp = EPS_POSIX_PTHREAD_MUTEX_UNLOCK ( &g_Eps_locks.enter_exit );
	EPS_ASSERT(0 == retTmp);

    return ( retVal );
}

/**
* This function allocates a non prio protected critical section using eps_alloc_critical_section_internal.
* Only Threads with the same priority should use the allocated critical section
* 
* @see eps_alloc_critical_section_internal
* 
* @param [out] lock_id_ptr           - handle to the lock
* @param [in] isPtreadMutexRecursive - LSA_TRUE: Recursive lock, LSA_FALSE: non recursive lock
* @return #LSA_RET_ERR_RESOURCE      - all possible locks are already taken
* @return #LSA_RET_OK                - Lock was successfully created.
*/
LSA_RESPONSE_TYPE eps_alloc_critical_section( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive)
{
	return eps_alloc_critical_section_internal(lock_id_ptr, isPtreadMutexRecursive, LSA_FALSE, LSA_FALSE);
}
/**
* This function allocates a prio protected critical section using eps_alloc_critical_section_internal.
* Priorty of low prio threads may be rised while holding the critical section to protect from priority inversion!
* Threads with different priority can use the allocated critical section.  
* 
* @see eps_alloc_critical_section_internal
* 
* @param [out] lock_id_ptr           - handle to the lock
* @param [in] isPtreadMutexRecursive - LSA_TRUE: Recursive lock, LSA_FALSE: non recursive lock
* @return #LSA_RET_ERR_RESOURCE      - all possible locks are already taken
* @return #LSA_RET_OK                - Lock was successfully created.
*/
LSA_RESPONSE_TYPE eps_alloc_critical_section_prio_protected( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive)
{
	return eps_alloc_critical_section_internal(lock_id_ptr, isPtreadMutexRecursive, LSA_TRUE, LSA_FALSE);
}

/**
* This function allocates a prio ceiling critical section using eps_alloc_critical_section_internal.
* Priorty of threads is always rised to ceiling while holding the critical section to protect from getting interrupted by other EPS threads at all.
* Use the allocated critical section to prevent to be interrupted by other EPS threads.
* 
* @see eps_alloc_critical_section_internal
* 
* @param [out] lock_id_ptr           - handle to the lock
* @param [in] isPtreadMutexRecursive - LSA_TRUE: Recursive lock, LSA_FALSE: non recursive lock
* @return #LSA_RET_ERR_RESOURCE      - all possible locks are already taken
* @return #LSA_RET_OK                - Lock was successfully created.
*/
LSA_RESPONSE_TYPE eps_alloc_critical_section_prio_ceiling( LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive)
{
	return eps_alloc_critical_section_internal(lock_id_ptr, isPtreadMutexRecursive, LSA_TRUE, LSA_TRUE);
}


/*-----------------------------------------------------------------*/
/**
* Releases the allocated lock in eps_alloc_critical_section
* 
* @see eps_alloc_critical_section - alloc function
* 
* @param [in] lock_id             - handle to the lock that must be released
* @return #LSA_RET_OK             - everything is fine
* @return #LSA_RET_ERR_PARAM      - for some reason, this lock was already released.
*/
LSA_RESPONSE_TYPE eps_free_critical_section( LSA_UINT16 lock_id )
{
    LSA_RESPONSE_TYPE   retVal = LSA_RET_OK;
    int                 retValLock;

    EPS_ASSERT( g_Eps_locks.is_running );

    retValLock = EPS_POSIX_PTHREAD_MUTEX_LOCK ( &g_Eps_locks.enter_exit );
    EPS_ASSERT(0 == retValLock);

    if (( lock_id <= EPS_LOCK_ID_MAX ) &&       // in range ?
        ( g_Eps_locks.locks[lock_id].is_used )) // allocated ?
    {
        retValLock = EPS_POSIX_PTHREAD_MUTEX_DESTROY( &g_Eps_locks.locks[lock_id].enter_exit );
        //EPS_ASSERT(0 == retValLock);
        if (retValLock != 0)
        {
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "eps_free_critical_section(): retValLock(%d) != 0 for lock_id(%d)", retValLock, lock_id);
            EPS_FATAL(0);
        }

        g_Eps_locks.locks[lock_id].is_used = LSA_FALSE;

        retVal = LSA_RET_OK;
    }
    else
    {
        retVal = LSA_RET_ERR_PARAM;

        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_free_critical_section(): lock_id(%d) is ouf range or not allocated", lock_id);
    }

    retValLock = EPS_POSIX_PTHREAD_MUTEX_UNLOCK ( &g_Eps_locks.enter_exit );
    EPS_ASSERT(0 == retValLock);

    return ( retVal );
}

/**
* Enters a critical section and guards it against mutal access from different threads. For recursive locks, this does _not_ guard
* against a reentrance within the same thread (this is explicitly allowed in recursive usecase).
* 
* Check if the lock was successfully entered. Note that for non recursive locks, we may get a EDEADLK as retval if the same thread 
* tries to enter this code section _again_ without unlocking the mutex. EPS assumes that the user of the lock would allocate a 
* recursive lock, in this case a FATAL occurs.
* 
* @param [in] lock_id      - handle to the lock
* @return LSA_VOID
*/
LSA_VOID eps_enter_critical_section( LSA_UINT16 lock_id )
{
    //TODO: LSA_INT retVal = 0; //success code in posix API
    EPS_ASSERT( g_Eps_locks.is_running );

    // EPS_ASSERT( lock_id <= EPS_LOCK_ID_MAX );
    if (lock_id > EPS_LOCK_ID_MAX)
    {
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "eps_enter_critical_section(): lock_id(%d) > EPS_LOCK_ID_MAX(%d)", lock_id, EPS_LOCK_ID_MAX);
        EPS_FATAL(0);
    }

    // EPS_ASSERT( g_Eps_locks.locks[lock_id].is_used );
    if (!g_Eps_locks.locks[lock_id].is_used)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_enter_critical_section(): lock_id(%d) is not used", lock_id);
        EPS_FATAL(0);
    }

    (LSA_VOID)EPS_POSIX_PTHREAD_MUTEX_LOCK( &g_Eps_locks.locks[lock_id].enter_exit );
    
    // check retval. Possible values may be EDEADLK == 25 if a non recursive lock was used.
    // EPS_ASSERT (retVal == 0); //TODO: SelfDeadlock at LOCK_NET_RESOURCE - ASSERT triggers here, commented out.

    //lint --e(454) A thread mutex has been locked but not unlocked - This function is a wrapper for the lock call, this is intended.
}

/**
* Exits a critical section. Note that for recursive locks, a thread has to call the exit function as often as an enter was 
* called before an other thread may access the ciritical section.
* 
* @param [in] lock_id - handle to the lock.
* @return LSA_VOID
*/
LSA_VOID eps_exit_critical_section( LSA_UINT16 lock_id )
{
    EPS_ASSERT( g_Eps_locks.is_running );

    // EPS_ASSERT( lock_id <= EPS_LOCK_ID_MAX );
    if (lock_id > EPS_LOCK_ID_MAX)
    {
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "eps_exit_critical_section(): lock_id(%d) > EPS_LOCK_ID_MAX(%d)", lock_id, EPS_LOCK_ID_MAX);
        EPS_FATAL(0);
    }

    // EPS_ASSERT( g_Eps_locks.locks[lock_id].is_used );
    if (!g_Eps_locks.locks[lock_id].is_used)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_exit_critical_section(): lock_id(%d) is not used", lock_id);
        EPS_FATAL(0);
    }
    //lint --e(455) A thread mutex that had not been locked is being unlocked - This function is a wrapper for the unlock call, this is intended.
    (LSA_VOID)EPS_POSIX_PTHREAD_MUTEX_UNLOCK( &g_Eps_locks.locks[lock_id].enter_exit );    
}
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
