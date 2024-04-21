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
/*  F i l e               &F: posix2win.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS (Posix) Interface port to Windows                                    */
/*                                                                           */
/*****************************************************************************/

#include <eps_sys.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <process.h>   /* _beginthreadex() */
#include "posix2win.h"

#include "eps_rtos.h"


#define P2W_TIMER_COUNT_MAX     (2+10)      // eps standard timer + EPS_TIMER_MAX_SYS_TIMER (eps_timer.c)

// Symbolic priority

typedef struct 
{ 
    UINT              period;
    UINT              timer_ID;	
    LARGE_INTEGER     freq;
    struct sigevent  *pEvp;
} POSIX2WIN_TIMER;

typedef struct
{
    POSIX2WIN_TIMER  posix2win_timer[P2W_TIMER_COUNT_MAX];
    LSA_UINT32       timer_create_count; 
    LARGE_INTEGER    freq; // used in clock_gettime to store the frequency. Used to avoid static variables in the function
    LARGE_INTEGER    base;// used in clock_gettime to store the base time. Used to avoid static variables in the function
} P2W_TIMER_STORE;

typedef struct
{
    LSA_BOOL  bUsed;
    uint32_t  hOsThread;
    HANDLE    hThreadStoppedEvent;
    HANDLE    hSignalEvents[(SIGRTMAX-SIGRTMIN+1)];
    void      *pThreadArgs;
    void      *(*pStartRoutineCbf)(void*);
    pthread_attr_t uAttr;
} P2W_THREAD_TYPE, *P2W_THREAD_PTR_TYPE;

#define P2W_CFG_MAX_THREADS 50

typedef struct
{
    LSA_BOOL         bInitialised;
    pthread_mutex_t  hLocCritical;
    pthread_mutex_t* hCritical;
    P2W_THREAD_TYPE  Thread[P2W_CFG_MAX_THREADS];
} P2W_THREAD_STORE;

static P2W_THREAD_STORE g_Eps_P2wThreads = {0};
static P2W_TIMER_STORE g_Eps_P2wTimer = {0};
//Disbale lint warnings "Function '' whose address was taken has an unprotected access to variable 'g_pEps*'"
//lint -esym(457, g_Eps_P2wThreads)
//lint -esym(457, g_Eps_P2wTimer)

static P2W_THREAD_PTR_TYPE p2w_get_thread_by_handle( pthread_t thread );
static int p2w_map_win_prio_to_posix (int prio_win);
static int p2w_map_posix_prio_to_win (int posix_sched_priority);

/**
 * Determines the length of the string s
 *
 * @param s String to be inspected
 *
 * @return Length of the string s
 */
static size_t eps_lib_strlen(const char *s)
{
    size_t len = 0;

    // Determine length
    while (*s != '\0')
    {
        len++;
        s++;
    }

    return len;
}

/**
 * Check if name contains only allowed characters.
 *
 * @param name Name to check
 *
 * @return 0 if name is ok, -1 otherwise
 */
static int eps_lib_checkname (const char *name)
{
    uint16_t i;
    size_t name_len;

    // Check character set
    name_len = eps_lib_strlen(name);
    for(i=0; i<name_len; i++)
    {
        if (   !((name[i]>='A') && (name[i]<='Z'))
            && !((name[i]>='a') && (name[i]<='z'))
            && !((name[i]>='0') && (name[i]<='9'))
            &&  (name[i]!='.')
            &&  (name[i]!='_')
            &&  (name[i]!='-'))
        {
            // return error code
            return -1;
        }
    }
    return 0;
}

/**
 * Copies the string s2 into the string s1 up to size
 *
 * @param s1   String to fill
 * @param s2   String to copy into s1
 * @param size Number of bytes to copy
 *
 * @return s1
 */
static char *eps_lib_strncpy(char * s1, const char * s2, size_t size)
{
    char *ret = s1;
    size_t cnt = 0;

    while(cnt < size)
    {
        *s1 = *s2;
        if ('\0' == *s2)
        {
            return ret;
        }
        s1++;
        s2++;
        cnt++;
    }
    return ret;
}

/**
 * Description.
 *
 * @param rqtp time to wait
 * @param rmtp timespec structure where remaining time is written to
 *
 * @return
 */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{  
    unsigned int sleep_time_ms;
    LSA_UNUSED_ARG(rmtp);

    if ((rqtp->tv_nsec<0) || (rqtp->tv_nsec>=MAX_NANO_SEC))
    {
        errno=EINVAL;
        return -1;
    }

    sleep_time_ms = (unsigned int) (rqtp->tv_nsec / 1000000 + rqtp->tv_sec * 1000);

    if (sleep_time_ms == 0)
    {
        sleep_time_ms = 1;
    }

    Sleep(sleep_time_ms);
        
    return 0;

}

/**
 * Description.
 *
 * @param mutex Mutex to be destroyed
 *
 * @return 0 or error code
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    DeleteCriticalSection( &mutex->lock );

    // Clear all data
    eps_memset(mutex, 0 ,sizeof(pthread_mutex_t));

    return 0;
}

/*
 * Description.
 *
 * @param mutex Mutex to be initialized
 * @param attr Attribute set for mutex
 *
 * @return 0 or error code
 */
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr)
{  	
    // Clear all data
    eps_memset(mutex, 0 ,sizeof(pthread_mutex_t));

    InitializeCriticalSection( &mutex->lock );

    if(attr != NULL)
    {
        mutex->protocol = attr->protocol;
    }

    return 0;
}

/**
 * Implements a mutex using the windows api using EnterCriticalSection. There is a special handling for mutex with the PROTECTED attribute, see comments in code.
 *
 * @param mutex Mutex to be locked
 *
 * @return 0 or error code
 */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    HANDLE self_handle = GetCurrentThread();    // used to set the prio, this is a pseudo handle and cannot be used as the value mutex->owner
    DWORD self_id = GetCurrentThreadId();       // this is a unique handle that is different for every thread. We use this to store the mutex->owner
    int32_t orig_prio = 0;                      // local variable: We need to store the value of GetThreadPriority here and copy the local value to mutex->orig_prio LATER
    
    P2W_THREAD_PTR_TYPE pThread = NULL;
    pthread_t pthread_this_thread = pthread_self() ; 

    if(mutex->owner != self_id)
    {
        BOOL resultSetPrio;
        /// For PROTECT mode only: We do not have the lock yet, but we raise the prio of this thread to the highest prio. 
        /// This prio is lowered to the original prio at pthread_mutex_unlock
        if(mutex->protocol == PTHREAD_PRIO_PROTECT)
        {
            orig_prio = GetThreadPriority(self_handle); // use the pseudo handle

            resultSetPrio = SetThreadPriority(self_handle, THREAD_PRIORITY_TIME_CRITICAL); // use the pseudo handle
            EPS_ASSERT(TRUE == resultSetPrio);

            // if the thread was not created by posix2win, we can still lock it, but we are unable to save any information into our management
            if( pthread_this_thread != P2W_INVALID_THREAD_ID)
            {
                pThread = p2w_get_thread_by_handle(pthread_this_thread);
                EPS_ASSERT(pThread);

                pThread->uAttr.schedparam.sched_priority = p2w_map_win_prio_to_posix(THREAD_PRIORITY_TIME_CRITICAL);
            }       
        }
    }

    /// Use Windows lock implementation EnterCriticalSection
    EnterCriticalSection( &mutex->lock );
    // now that we have the lock, we can store all the infos into the mutex structure
    if(mutex->owner != self_id)
    {
        mutex->owner = self_id;
        mutex->count = 0;
        mutex->orig_prio = orig_prio;
  
    }
    mutex->count++;

    return 0;
}

/**
 * Implements a mutex using the windows api using LeaveCriticalSection. 
 * There is a special handling for mutex with the PROTECTED attribute, see comments in code.
 *
 * @param mutex Mutex to be unlocked
 *
 * @return 0 or error code
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    P2W_THREAD_PTR_TYPE pThread = NULL;
    pthread_t pthread_this_thread = pthread_self(); 
    int32_t orig_prio = mutex->orig_prio;           // save the info from the lock into a local variable as long as we have the lock

    mutex->count--;
    if(mutex->count == 0)
    {
        mutex->owner = 0;
        /// Use Windows lock implementation LeaveCriticalSection
        LeaveCriticalSection( &mutex->lock );

        /// For PROTECT mode only: We have to set the prio of this thread back to the old value stored in orig_prio.
        if(mutex->protocol == PTHREAD_PRIO_PROTECT)
        {
            BOOL resultSetPrio;
            resultSetPrio = SetThreadPriority(GetCurrentThread(), orig_prio); // note: use local variable orig_prio instead of mutex->orig_prio -> we do not longer have the lock, another thread may access mutex->orig_prio!
            EPS_ASSERT(TRUE == resultSetPrio);

            // save infos in thread attributes
            // if the thread was not created by posix2win, we can still lock it, but we are unable to save any information into our management
            if( pthread_this_thread != P2W_INVALID_THREAD_ID)
            {
                pThread = p2w_get_thread_by_handle( pthread_this_thread );
                EPS_ASSERT(pThread);

                pThread->uAttr.schedparam.sched_priority = p2w_map_win_prio_to_posix (orig_prio); // note: use local variable orig_prio instead of mutex->orig_prio -> we do not longer have the lock, another thread may access mutex->orig_prio!
            }
        }
    }
    else
    {
        LeaveCriticalSection( &mutex->lock );
    }

    return 0;
}

/**
 * Description.
 *
 * @param attr Mutex attributes field
 *
 * @return
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    // Clear all data
    eps_memset(attr,0,sizeof(pthread_mutexattr_t));  

    return 0;
}


/**
 * Description.
 *
 * @param attr Mutex attributes field
 *
 * @return
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    // Clear all data
    eps_memset(attr,0,sizeof(pthread_mutexattr_t));  

    // Use default values
    attr->prioceiling = PTHREAD_PRIO_NONE;

    return 0;
}

/**
 * Returns the protocol attribute of a mutex attributes object pointed to by \c attr which was previously
 * created by the function pthread_mutexattr_init().
 *
 * The protocol attribute defines the protocol to be followed in utilizing mutexes.
 * The value of protocol may be one of:
 *  - #PTHREAD_PRIO_NONE
 *  - #PTHREAD_PRIO_PROTECT
 * which are defined in #pthread.h
 *
 * When a thread owns a mutex with the #PTHREAD_PRIO_NONE protocol attribute, its priority and scheduling
 * shall not be affected by its mutex ownership.
 * When a thread owns one or more mutexes initialized with the #PTHREAD_PRIO_PROTECT protocol,
 * it shall execute at the higher of its priority or the highest of the priority ceilings of all the mutexes
 * owned by this thread and initialized with this attribute, regardless of whether other threads are blocked
 * on any of these mutexes or not.
 *
 * @see #pthread_mutexattr_setprotocol
 *
 * @param attr Mutex attributes field
 * @param protocol Location where the protocol is stored
 *
 * @return Always 0
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t * attr,
                                  int * protocol)
{
  // Get value
  *protocol = attr->protocol;

  return 0;
}


/**
 * Sets the protocol attribute of a mutex attributes object pointed to by \c attr which was previously
 * created by the function pthread_mutexattr_init().
 *
 * The protocol attribute defines the protocol to be followed in utilizing mutexes.
 * The value of protocol may be one of:
 *  - #PTHREAD_PRIO_NONE
 *  - #PTHREAD_PRIO_PROTECT
 * which are defined in #pthread.h
 *
 * When a thread owns a mutex with the #PTHREAD_PRIO_NONE protocol attribute, its priority and scheduling
 * shall not be affected by its mutex ownership.
 * When a thread owns one or more mutexes initialized with the #PTHREAD_PRIO_PROTECT protocol,
 * it shall execute at the higher of its priority or the highest of the priority ceilings of all the mutexes
 * owned by this thread and initialized with this attribute, regardless of whether other threads are blocked
 * on any of these mutexes or not.
 *
 * @see #pthread_mutexattr_getprotocol
 *
 * @param attr Mutex attributes field
 * @param protocol Protocol to set. One of
 *    - #PTHREAD_PRIO_NONE
 *    - #PTHREAD_PRIO_PROTECT
 *    - #PTHREAD_PRIO_INHERIT
 *
 * @return 0 upon successful completion, #ENOTSUP if an unsopported protocol was passed.
 *
 * @note The protocols #PTHREAD_PRIO_PROTECT and #PTHREAD_PRIO_PROTECT can only be used if #ADN_MTX_MODE
 *    is set to #ADN_MTX_MODE_PRIO
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
  // Is value for protocol supported?
  if ((protocol != PTHREAD_PRIO_NONE)
      && (protocol != PTHREAD_PRIO_PROTECT)
      )
    {
      return ENOTSUP;
    }

  // Set value
  attr->protocol = protocol;

  return 0;
}



/**
 * Initializes the given thread attributes.
 * The results of otherwise referencing the object after it has been instantiated
 * are undefined.
 * The pthread_attr_init() function shall initialize the thread attribute
 * object attr such as if a pthread_attr_setinheritsched() function  with the
 * inheritsched flag set to PTHREAD_EXPLICIT_SCHED was executed. This specifies that
 * when used by pthread_create() the  thread scheduling attributes shall be set to
 * the  values from this attributes object.
 * All other attributes are not set to a defined value, they must be set explicitly
 * by the corresponding pthread_attr_setstack(), pthread_attr_setschedparam()
 * and pthread_attr_setschedpolicy()
 * \n
 * The cpuset member of the attribute is set so that the thread could run on all
 * CPUs.
 *
 * @see #pthread_attr_setstack
 * @see #pthread_attr_setschedparam
 * @see #pthread_attr_setschedpolicy
 *
 * @param attr Pointer to the thread attribute object to be initialized
 *
 * @return 0 if the creation of the thread was successful, an errorcode otherwise.
 *          Errors:
 *          - #ENOMEM Insufficient memory exists to initialize the thread attributes object.
 */
int pthread_attr_init(pthread_attr_t *attr)
{
    char* cStrCpy;
    eps_memset(attr,0,sizeof(pthread_attr_t));  
    // Set the inheritsched to INHERIT. This specifies that
    // when pthread_create() is called, the scheduling attributes are set to the
    // ones of the calling thread. The values specified in this attribute object will have no effect.
    attr->inheritsched = PTHREAD_INHERIT_SCHED;
    // per default all new threads are joinable
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    attr->stacksize = EPS_SYS_THREAD_STACK_SIZE_BYTE;
    // set the stack address to NULL, it will then be allocated using the kernel malloc
    attr->stackaddr=NULL;

    attr->schedparam.sched_priority = EPS_POSIX_THREAD_PRIORITY_NORMAL; // was P2W_SYS_THREAD_DEFAULT_PRIO == WIN_P1
    attr->schedpolicy               = P2W_SYS_THREAD_DEFAULT_SCHED;

    attr->stack_dynamically_allocated = LSA_FALSE;

    cStrCpy = eps_lib_strncpy(attr->name, "", OS_THREAD_NAME_LEN);
    LSA_UNUSED_ARG(cStrCpy);

    return 0;
}

/**
 * Destroys the given thread attributes.
 * The pthread_attr_destroy() function shall destroy a thread attributes object.
 * An implementation may cause pthread_attr_destroy() to set attr to an
 * implementation-defined invalid value. A destroyed attr attributes object
 * can be reinitialized using pthread_attr_init(); the results of otherwise
 * referencing the object after it has been destroyed are undefined.
 *
 * @param attr Pointer to the thread attribute object to be destroyed
 *
 * @return 0 (no errors are defined) 
 */
int pthread_attr_destroy(pthread_attr_t *attr)
{
    char* cStrCpy;
    // set members of attr to invalid values, where possible
    attr->inheritsched                =            -1;
    attr->schedpolicy                 =            -1;
    attr->detachstate                 = (uint32_t) -1;
    attr->stack_dynamically_allocated = (uint32_t) -1;
    attr->schedparam.sched_priority   =       -1;
    attr->stackaddr                   = (void *)   NULL;

    cStrCpy = eps_lib_strncpy(attr->name, "", OS_THREAD_NAME_LEN);
    LSA_UNUSED_ARG(cStrCpy);

    return 0;
}

/* Returns the scheduling policy stored within the given thread attributes object.
 *
 * @param attr The thread attributes object the scheduling policy should be retrieved from
 * @param policy Takes the retrieved scheduling policy
 *
 * @return always 0 since nothing can go wrong
 */
int pthread_attr_getschedpolicy(const pthread_attr_t * attr,
                                int * policy)
{
  *policy = attr->schedpolicy;

  return 0;
}

/**
 * Sets the inheritsched attribute in the \c attr argument.
 * When the attributes objects are used by pthread_create(), the
 * \c inheritsched attribute determines how the other scheduling attributes of
 * the created thread shall be initialized.
 *
 * @param attr The thread attribute object where \c inheritsched should be stored in
 * @param inheritsched The inheritsched value to be stored in \c attr.
 *      Possible values are:
 *          - #PTHREAD_INHERIT_SCHED
 *          - #PTHREAD_EXPLICIT_SCHED
 *
 * @return 0 if the operation was successful, an errorcode otherwise.
 *          Errors:
 *              - #EINVAL The value of inheritsched is not valid
 */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    // only PTHREAD_EXPLICIT_SCHED is used
    // test if the value of inheritsched is valid
    if ((inheritsched != PTHREAD_INHERIT_SCHED ) &&
        (inheritsched != PTHREAD_EXPLICIT_SCHED)  )
    {
        printf("pthread_attr_setinheritsched(EINVAL)");
        return EINVAL;
    }

    attr->inheritsched = inheritsched;

    // everything is ok
    return 0;
}

/**
//  * Set the stacksize of the given thread attributes.
//  *
//  * @param attr The thread attributes object in which the stacksize should be stored
//  * @param stacksize The stacksize to be stored within the given attribute object
//  *
//  * @return 0 if the stacksize was successfully set, an errorcode otherwise
//  *             - #EINVAL if the value of \c stacksize is less than #EPS_SYS_THREAD_STACK_MIN
//  */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (stacksize < PTHREAD_STACK_MIN)
    {
        return EINVAL;
    }

    // check alignment of stacksize
    // NOTE: Stack address is assumed to be aligned due to checks in ptherad_attr_setstack
    if ((stacksize & (EPS_SYS_PROC_BYTES -1)) != 0)
    {
        return EINVAL;
    }

    attr->stacksize = stacksize;

    return 0;
}

/**
 * Sets the given scheduling parameter \c param in the given thread attribute object.
 *
 * @param attr The thread attribute object in which the scheduling parameters should be set
 * @param param The scheduling parameter to be set in \c attr
 *
 * @return 0 if the scheduling parameter was successfully set, otherwise an error number.
 *          - #EINVAL if the value of param.sched_priority is not within 0 and #RT_PMAX
 */
int pthread_attr_setschedparam(pthread_attr_t * attr,
                               const struct sched_param * param)
{
    // test if the value of param.sched_priority is valid
    if (  (param->sched_priority < 0) || (param->sched_priority > EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE)) 
    {
        return ENOTSUP;
    }

    eps_memcpy((void*)&attr->schedparam, (void*)param, sizeof(struct sched_param));

    return 0;
}

/**
 * Retrieves the scheduling policy and parameters of the given thread.
 * The pthread_getschedparam() function shall retrieve the scheduling policy 
 * and scheduling parameters for the thread whose thread ID is given by 
 * \c thread and shall store those values in \c policy and \c param, respectively. 
 * The priority value returned from pthread_getschedparam() shall be the value 
 * specified by the most recent pthread_setschedprio() or pthread_create() call 
 * affecting the target thread. It shall not reflect any temporary adjustments to 
 * its priority as a result of any priority ceiling functions. 
 * The policy parameter may have the value SCHED_FIFO, SCHED_RR or SCHED_OTHER. 
 * The SCHED_FIFO and SCHED_RR policies shall have a single scheduling parameter priority.
 * 
 * @param thread The thread which scheduling parameter should be retrieved
 * @param policy Takes the scheduling policy of the given thread
 * @param param Takes the scheduling parameter of the given thread
 * 
 * @return 0 if the action finished successful, otherwise
 *      - #ESRCH if the value specified by thread does not refer to an existing thread
 */
int pthread_getschedparam(  pthread_t thread, 
                            int *policy,
                            struct sched_param * param)
{
    P2W_THREAD_PTR_TYPE pThread;
    // check before calling p2w_get_thread_by_handle()
    if(thread == 0)
    {
        return ESRCH;
    }

    pThread = p2w_get_thread_by_handle( thread );

    // thread not found or not in use
    if (pThread == NULL )
    {
        return ESRCH;
    }

    *policy                  = pThread->uAttr.schedpolicy;
    param->sched_priority    = pThread->uAttr.schedparam.sched_priority;

    return 0;
}
/**
 * Sets the thread creation stack attributes \c stackaddr and \c stacksize.
 *
 * @param attr The thread attribute object to store the stack attributes in
 * @param stackaddr The address of the stack
 * @param stacksize The size of the stack
 *
 * @return 0 if successfully completed, an errorcode otherwise
 *              - #EINVAL if the stacksize is less than #ADN_SYS_THREAD_STACK_MIN or exceeds
 *                  an implementation-defined limit
 *              - #EINVAL if the stack begin of end has't got proper alignment
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize)
{
    volatile unsigned long *test_addr;
    // check the size to be at least PTHREAD_STACK_MIN
    if (stacksize < PTHREAD_STACK_MIN)
    {
        return EINVAL;
    }

    // check alignment of stack begin and end
    if (((uintptr_t)stackaddr & (EPS_SYS_PROC_BYTES - 1)) != 0 )
    {
        return EINVAL;
    }

    // check alignment of stacksize
    if ((stacksize & (EPS_SYS_PROC_BYTES -1)) != 0)
    {
        return EINVAL;
    }

    // test begin of stack
    test_addr=(unsigned long*)stackaddr;
    *test_addr=*test_addr;

    // test end of stack
    test_addr=(unsigned long*)((uintptr_t)stackaddr + (stacksize-EPS_SYS_PROC_BYTES));
    *test_addr=*test_addr;

    attr->stackaddr=stackaddr;
    attr->stacksize=stacksize;

    return 0;
}


/**
 * Sets the given scheduling policy within the given thread attribute object.
 *
 * @param attr The thread attributes object in which the scheduling policy should
 *      be set
 * @param policy The scheduling policy to be set within \c attr
 *
 * @return 0 if setting the policy was successful, an errorcode otherwise.
 *          Errors:
 *              - #EINVAL if the value of policy is not valid
 */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    // Test whether the scheduling policy is valid
    if ((policy!=SCHED_FIFO ) &&
        (policy!=SCHED_RR   ) &&
        (policy!=SCHED_OTHER) )
    {
        printf("pthread_attr_setschedpolicy(EINVAL)");
        return EINVAL;
    }

    attr->schedpolicy = policy;

    // everything was ok
    return 0;
}

/**
 * Sets the thread's name.
 *
 * @param attr The thread attribute object to store the name in
 * @param name The name
 *
 * @return 0 if successfully completed, an errorcode otherwise
 *    - #EINVAL The name argument contains illegal characters.
 *    - #ENAMETOOLONG The length of the name argument is longer than {OS_THREAD_NAME_LEN}.
 */
int pthread_attr_setname(pthread_attr_t * attr, const char *name)
{
    char* cStrCpy;
    // Check pointer
    if (name == NULL) 
    {
        // return error code
        return EINVAL;
    }

    // Check name length
    if (eps_lib_strlen(name) >= OS_THREAD_NAME_LEN)
    {
        // return error code
        return ENAMETOOLONG;
    }

    // Check character set
    if (0 != eps_lib_checkname(name))
    {
        // return error code
        return EINVAL;
    }

    // Set name
    cStrCpy = eps_lib_strncpy(attr->name, name, OS_THREAD_NAME_LEN);
    LSA_UNUSED_ARG(cStrCpy);

    return 0;
}

int pthread_attr_setcpuaffinity(pthread_attr_t *attr, const cpu_set_t * cpuset)
{

  LSA_UNUSED_ARG(attr);

  if(*cpuset != 1)  
  {
    EPS_FATAL("Only first Core supported!");
  }
  return 0;
}

/** Getting new Thread from Threadpool
 * 
 * If there are enough Threads left, the P2W_THREAD_PTR_TYPE is returned
 * 
 * @return P2W_THREAD_PTR_TYPE reference to free thread
 * @return LSA_NULL if no thread free and EPS_FATAL
 */
static P2W_THREAD_PTR_TYPE p2w_get_new_thread(LSA_VOID)
{
    LSA_UINT32 i;
    int retValLock;

    retValLock = pthread_mutex_lock(g_Eps_P2wThreads.hCritical);
    EPS_ASSERT(0 == retValLock);

    for(i=0; i<P2W_CFG_MAX_THREADS; i++)
    {
        if(g_Eps_P2wThreads.Thread[i].bUsed == LSA_FALSE)
        {
            g_Eps_P2wThreads.Thread[i].bUsed = LSA_TRUE;

            retValLock = pthread_mutex_unlock(g_Eps_P2wThreads.hCritical);
            EPS_ASSERT(0 == retValLock);
            return &g_Eps_P2wThreads.Thread[i];
        }
    }

    retValLock = pthread_mutex_unlock(g_Eps_P2wThreads.hCritical);
    EPS_ASSERT(0 == retValLock);

    EPS_FATAL("No More Thread Resources");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return LSA_NULL;
}

static P2W_THREAD_PTR_TYPE p2w_get_thread_by_handle( pthread_t thread )
{
    return (P2W_THREAD_PTR_TYPE) thread;
}

static LSA_VOID p2w_thread_free(P2W_THREAD_PTR_TYPE pThread)
{
    int retValLock;
    EPS_ASSERT(pThread != LSA_NULL);

    retValLock = pthread_mutex_lock(g_Eps_P2wThreads.hCritical);
    EPS_ASSERT(0 == retValLock);
    pThread->bUsed = LSA_FALSE;

    retValLock = pthread_mutex_unlock(g_Eps_P2wThreads.hCritical);
    EPS_ASSERT(0 == retValLock);
}

/**
* Does a mapping between the defines in WinBase.h and our own defines in eps_posix_cfg_windows.h
* @param [in] posix_sched_priority  - EPS posix prio
* @return prio_win                  - the mapped windows prio
*/
static int p2w_map_posix_prio_to_win (int posix_sched_priority)
{
    int prio_win = 0;

    switch (posix_sched_priority)
    {
    case 0: // No Realtime required
            prio_win = THREAD_PRIORITY_LOWEST;
            break;
    case EPS_POSIX_THREAD_PRIORITY_BELOW_NORMAL:
            prio_win = THREAD_PRIORITY_BELOW_NORMAL;
            break;
    case EPS_POSIX_THREAD_PRIORITY_NORMAL: 
            prio_win = THREAD_PRIORITY_NORMAL;
            break;
    case EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL:
            prio_win = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
    case EPS_POSIX_THREAD_PRIORITY_HIGH: 
/*  case EPS_POSIX_THREAD_PRIORITY_TIMER: same prio */
            prio_win = THREAD_PRIORITY_HIGHEST;
            break;
    case EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE: 
            prio_win = THREAD_PRIORITY_TIME_CRITICAL;
            break;
    default:
        EPS_FATAL("Mapping does not exist");
        break;
    }
    return prio_win;
}
 
/**
* Does a mapping between the defines in WinBase.h and our own defines in eps_posix_cfg_windows.h
* @param [in] prio_win              - the windows prio 
* @return prio_win                  - the mapped EPS posix prio
*/
static int p2w_map_win_prio_to_posix (int prio_win)
{
    int posix_sched_priority = 0;
    switch (prio_win)
    {
    case THREAD_PRIORITY_LOWEST:
            posix_sched_priority = 0; // No Realtime required
            break;
    case THREAD_PRIORITY_BELOW_NORMAL:
            posix_sched_priority = EPS_POSIX_THREAD_PRIORITY_BELOW_NORMAL;
            break;
    case THREAD_PRIORITY_NORMAL: 
            posix_sched_priority = EPS_POSIX_THREAD_PRIORITY_NORMAL;
            break;
    case THREAD_PRIORITY_ABOVE_NORMAL:
            posix_sched_priority = EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL;
            break;
    case THREAD_PRIORITY_HIGHEST: 
            posix_sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
            break;
    case THREAD_PRIORITY_TIME_CRITICAL: 
            posix_sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE;
            break;
    default:
        EPS_FATAL("Mapping does not exist");
        break;
    }
    return posix_sched_priority;
}

/**
* Starts the function saved in pArg->pStartRoutineCbf and passes the arguments pArg->pThreadArgs.
* After the function was called, the thread is stopped.
*/
static unsigned __stdcall eps_win_thread_proc(void * arg) 
{
    BOOL bSetEvent;
    P2W_THREAD_PTR_TYPE pArg = (P2W_THREAD_PTR_TYPE)arg;
    void* pStartCbf;

    pStartCbf = pArg->pStartRoutineCbf(pArg->pThreadArgs);
    LSA_UNUSED_ARG(pStartCbf);

    bSetEvent = SetEvent( pArg->hThreadStoppedEvent );
    EPS_ASSERT( bSetEvent );

    // threads ends
    _endthreadex(0);
    return ( 0 );
}

/**
* Deletes the management structure g_Eps_P2wThreads.
*/
static void pthread_init(void)
{
    if(g_Eps_P2wThreads.bInitialised == LSA_FALSE)
    {
        int retValMutexInit;
        g_Eps_P2wThreads.hCritical = &g_Eps_P2wThreads.hLocCritical;
        retValMutexInit = pthread_mutex_init(g_Eps_P2wThreads.hCritical, LSA_NULL);
        EPS_ASSERT(0 == retValMutexInit);

        g_Eps_P2wThreads.bInitialised = LSA_TRUE;
    }
}

/** Creates and starts a new thread in Windows
 * 
 * Fetches a new thread out of pool and sets the specified parameters.
 * Thread gets started after that.
 * 
 * @param thread pointer to created thread
 * @param attr thread_paramteres (e.g. priority)
 * @param start_routine function pointer to start_routine of thread
 * @param arg arguments given to the start_routine
 * @return 0
 */
int pthread_create(pthread_t * thread,
                   const pthread_attr_t * attr,
                   void *(*start_routine)(void*), void * arg)
{
    int ret_val = 0, prio, i;
    BOOL  set_state;
    HANDLE thread_handle;
    P2W_THREAD_PTR_TYPE pThread;

    pthread_init();

    pThread = p2w_get_new_thread();
    EPS_ASSERT(pThread != LSA_NULL);

    /// save the attributes of the thread
    pThread->uAttr = *attr;
    prio = p2w_map_posix_prio_to_win(attr->schedparam.sched_priority);
    
    pThread->hThreadStoppedEvent = CreateEvent( NULL, LSA_FALSE /*auto reset*/, LSA_FALSE, NULL );
    for(i = SIGRTMIN; i <= SIGRTMAX; i++)
    {
        pThread->hSignalEvents[i-SIGRTMIN] = CreateEvent( NULL, LSA_FALSE /*auto reset*/, LSA_FALSE, NULL );
    }
    pThread->pStartRoutineCbf = start_routine;
    pThread->pThreadArgs = arg;
    
    thread_handle = (HANDLE)_beginthreadex( // starting the thread
            NULL,
            attr->stacksize, 
            (eps_win_thread_proc), 
            pThread,
            0, 
      &pThread->hOsThread );


    EPS_ASSERT ( thread_handle != 0 );
    
    // set configured priority
    set_state = SetThreadPriority( thread_handle, prio );
    EPS_ASSERT( set_state );

    *thread = (pthread_t)pThread;

    // return
    return ret_val;
}

int pthread_join(pthread_t thread, void** th_ret)
{
	int i;
    DWORD resultWaitObject;
    BOOL resultCloseHandle;
    P2W_THREAD_PTR_TYPE pThread = (P2W_THREAD_PTR_TYPE)thread;

    EPS_ASSERT(pThread != LSA_NULL);

    LSA_UNUSED_ARG(th_ret); 	//(LaM)ToDo

    // Wait until target thread terminates. 
    resultWaitObject = WaitForSingleObject( pThread->hThreadStoppedEvent, INFINITE );
    LSA_UNUSED_ARG(resultWaitObject);

    resultCloseHandle = CloseHandle( pThread->hThreadStoppedEvent );
    EPS_ASSERT(resultCloseHandle);
    
	for(i = SIGRTMIN; i <= SIGRTMAX; i++)
    {
	    resultCloseHandle = CloseHandle( pThread->hSignalEvents[i-SIGRTMIN] );
	    EPS_ASSERT(resultCloseHandle);
	}

    p2w_thread_free(pThread);

    return 0;
}

pthread_t pthread_self(void)
{
    LSA_UINT32 curId  = GetCurrentThreadId(); 
    LSA_UINT32 i;

    for(i=0; i<P2W_CFG_MAX_THREADS; i++)
    {
        if(g_Eps_P2wThreads.Thread[i].bUsed == LSA_TRUE)
        {
            if(g_Eps_P2wThreads.Thread[i].hOsThread == curId)
            {
                return (LSA_UINT32)(&g_Eps_P2wThreads.Thread[i]);
            }
        }
    }

    return P2W_INVALID_THREAD_ID;
}

static void CALLBACK eps_mmtimer_callback( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
    struct sigevent * evp = (struct sigevent *)dwUser;

    LSA_UNUSED_ARG(uID);
    LSA_UNUSED_ARG(uMsg);
    LSA_UNUSED_ARG(dw1);
    LSA_UNUSED_ARG(dw2);

    evp->sigev_notify_function(evp->sigev_value);
}

/**
 * Description.
 *
 * @param clockid ID of clock
 * @param evp     Event pointer
 * @param timerid ID of timer
 *
 * @return
 */
int timer_create(clockid_t clockid, struct sigevent * evp,
                 timer_t * timerid)
{
    TIMECAPS tc;
    MMRESULT rc;
        
    //LSA_UNUSED_ARG(timerid);
    LSA_UNUSED_ARG(clockid);

    EPS_ASSERT (g_Eps_P2wTimer.timer_create_count < P2W_TIMER_COUNT_MAX);	

    g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].pEvp = (struct sigevent *)malloc(sizeof(struct sigevent));
    EPS_ASSERT(g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].pEvp != LSA_NULL);
    *g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].pEvp = *evp;

    if ( !QueryPerformanceFrequency (& g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].freq )) // get frequency
    {
        g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].freq.QuadPart = 0;
    }

    EPS_ASSERT( g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].freq.QuadPart != 0 );	

    // set granularity to 1msec
    rc = timeGetDevCaps(&tc, sizeof(TIMECAPS));
    EPS_ASSERT (rc == TIMERR_NOERROR);

    EPS_ASSERT( tc.wPeriodMin == 1); /* must: 1 msec ! */

    g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].period = tc.wPeriodMin;
    rc = timeBeginPeriod(g_Eps_P2wTimer.posix2win_timer[g_Eps_P2wTimer.timer_create_count].period);
    EPS_ASSERT (rc == TIMERR_NOERROR);		
    
    *timerid=(timer_t)g_Eps_P2wTimer.timer_create_count;

    g_Eps_P2wTimer.timer_create_count++;

    return 0;
    //lint --e(818) Pointer parameter 'evp' could be declared as pointing to const - we are unable to change the API.
}

/**
 * Description.
 *
 * @param timerid ID of timer
 * @param flags   Flags, TIMER_ABSTIME or 0
 * @param value   structure with new value
 * @param ovalue  structure for old value
 *
 * @return
 */
int timer_settime(timer_t timerid, int flags,
    const struct itimerspec * value,
    struct itimerspec * ovalue)
{
    int ret_val = 0;
    UINT cycleTime;
    MMRESULT rc;

    LSA_UNUSED_ARG(flags);
    LSA_UNUSED_ARG(ovalue);

    // disarm timer
    if (value->it_interval.tv_nsec == 0 && value->it_interval.tv_sec == 0)
    {
        // stop the MultiMediaTimer
        rc = timeKillEvent(g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID);
        EPS_ASSERT(rc == TIMERR_NOERROR);

        g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID = 0; // see MSDN and eps_mmtimer_callback()
    }
    // arm the timer
    else
    {
        cycleTime = (UINT)(value->it_interval.tv_nsec / 1000000 + value->it_interval.tv_sec * 1000);
        EPS_ASSERT(cycleTime != 0);

        // start the MultiMediaTimer
        //lint --e(835) A zero has been given as right argument to operator '|' - TIME_CALLBACK_FUNCTION == 0, give the argument to make clear what we want
        g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)timerid].timer_ID = (UINT)timeSetEvent(
            cycleTime,
            1,
            eps_mmtimer_callback,
            (DWORD_PTR)g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)timerid].pEvp,
            (TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS)
        );
        EPS_ASSERT(g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)timerid].timer_ID != 0);
    }

    return ret_val;
}

/**
 * Description.
 *
 * @param timerid ID of timer to delete
 *
 * @return
 */
int timer_delete(timer_t timerid)
{	
    MMRESULT rc;
    // stop the MultiMediaTimer
    if( g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID != 0)
    {
        rc = timeKillEvent (g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID);
        EPS_ASSERT( rc == TIMERR_NOERROR );

        g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].timer_ID = 0; // see MSDN and eps_mmtimer_callback()
    }

    // reset the granularity
    rc = timeEndPeriod( g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].period );
    EPS_ASSERT(rc == TIMERR_NOERROR);

    free(g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].pEvp);
    g_Eps_P2wTimer.posix2win_timer[(LSA_UINT32)(timerid)].pEvp = LSA_NULL;

    g_Eps_P2wTimer.timer_create_count--;

    return 0;
}

/**
 * Initializes a semaphore.
 *
 * @param sem Semaphore to be initialized
 * @param pshared If non-zero semaphore can be used by more than one process
 * @param value Start value of semaphore
 *
 * @return 0 upon successful completion, -1 otherwise. 
 */
int sem_init(sem_t *sem, int pshared, unsigned value)
{
    LSA_UNUSED_ARG(value);
    LSA_UNUSED_ARG(pshared);

    *sem = CreateEvent( NULL, LSA_FALSE/*auto*/, LSA_FALSE, NULL );
    EPS_ASSERT( *sem != LSA_NULL );

    return 0;
    //lint --e(818) Pointer parameter 'sem' could be declared as pointing to const - we are unable to change the API.
}


/**
 * Marks a semaphore as destroyed
 *
 * @param sem Semaphore to be destroyed
 *
 * @return 0 or error code, if semaphore still in use
 */
int sem_destroy(sem_t *sem)
{	
    BOOL rc = CloseHandle( *sem );
    EPS_ASSERT( rc );

    return 0;
    //lint --e(818) Pointer parameter 'sem' could be declared as pointing to const - we are unable to change the API.
}

/**
 * Lock a semaphore and wait forever, if it is not available.
 *
 * @param sem Semaphore to be locked
 *
 * @return The and sem_wait() returns zero if the calling process successfully performed 
 *    the semaphore lock operation on the semaphore designated by sem. If the call was 
 *    unsuccessful, the state of the semaphore is unchanged, and the function returns 
 *    a value of -1
 */
int sem_wait(sem_t *sem)
{
    DWORD result;
    int rc=0;
    BOOL retValReset;
        
    result = WaitForSingleObject( *sem, INFINITE );
    //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
    if (result != WAIT_OBJECT_0)
    {
        rc = -1;
    }
    else
    {
        retValReset = ResetEvent( *sem );
        if(retValReset)
        {
            rc = 0;
        }
        else
        {
            rc = -1;
        }
    }

    return rc;
    //lint --e(818) Pointer parameter 'sem' could be declared as pointing to const - we are unable to change the API.
}

/**
 * Lock a semaphore and wait until a definite absolute time, if it is not available.
 *
 * @param sem Semaphore to wait for until a certain absolute time
 * @param abs_timeout Absolute timeout
 *
 * @return The sem_timedwait() function returns zero if the calling process successfully performed the semaphore lock operation 
 *    on the semaphore designated by \csem. 
 *    If the call was unsuccessful, the state of the semaphore is unchanged, and the function shall return a value of -1 
 */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    struct timespec current_time;
    DWORD dwMilliSeconds;
    DWORD result;
    int rc = 0;
    int retValGetTime;

    retValGetTime = clock_gettime( CLOCK_REALTIME , &current_time );
    EPS_ASSERT(0 == retValGetTime);

    dwMilliSeconds = (DWORD) ((abs_timeout->tv_sec - current_time.tv_sec) * 1000);
        
    result = WaitForSingleObject( *sem, dwMilliSeconds );
    //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
    if (result == WAIT_OBJECT_0)
        // wait ok
    {
        rc = ResetEvent( *sem );
        EPS_ASSERT( rc );
    }
    else if(result == WAIT_TIMEOUT)
        // timeout occured
    {
        rc = -1;
    }
    else
        // other errors
    {
        rc = -1;
    }

    return rc;
    //lint --e(818) Pointer parameter 'sem' could be declared as pointing to const - we are unable to change the API.
}

/**
 * Release a semaphore.
 *
 * @param sem Semaphore to be released
 *
 * @return If successful, the sem_post() function returns zero; otherwise, 
 *    the function returns -1  
 */
int sem_post(sem_t *sem)
{
    BOOL rc;

    rc = SetEvent( *sem );
    EPS_ASSERT( rc );

    return 0;
    //lint --e(818) Pointer parameter 'sem' could be declared as pointing to const - we are unable to change the API.
}

/**
 * Adds the individual signal specified by signo to the signal set pointed to by set.
 *
 * @param set The set to which the signal should be added (SIGRTMIN to SIGRTMAX are supported)
 * @param signo The signal that should be added to the set
 *
 * @return 0 upon successful completion, -1 otherwise.
 *
 */
int sigaddset(sigset_t *set, int signo)
{
  if ((signo < SIGRTMIN) || (signo > SIGRTMAX))
  {
    return -1;
  }
  *set |= (LSA_UINT32)1 << (LSA_UINT32)signo;

  return 0;
}


/**
 * The sigemptyset() function initializes the signal set pointed to by set,
 * such that all signals  defined in <signal.h>  are excluded.
 *
 * @param set The set to be initialized
 *
 * @return always 0
 */
int sigemptyset(sigset_t *set)
{
  *set = 0;

  return 0;
}

/**
 * The sigwait() function shall select a pending signal from set, atomically
 * clear it from the system's set of pending signals, and return that signal
 * number in the location referenced by sig.
 * Any of multiple pending signals in the range SIGRTMIN to SIGRTMAX may be
 * selected.
 * If no signal in set is pending at the time of the
 * call, the thread shall be suspended until one or more arrives. The signals
 * defined by set must be blocked at the time of a call to sigwait().
 *
 * @param set Set of signals the thread should wait for (SIGRTMIN to SIGRTMAX are supported)
 * @param sig The signal delivered to and selected by the current thread
 *
 * @return 0 upon successful completion, otherwise -1
 */
int sigwait(const sigset_t *set, int *sig)
{
    DWORD lEventSignaled;
    int rc = -1;
    LSA_UINT32 i, sigCount;
    HANDLE hArrayEvents[((SIGRTMAX-SIGRTMIN)+1)];
    int lArraySignals[((SIGRTMAX - SIGRTMIN) + 1)] = { 0 };
    pthread_t pthread_this_thread = pthread_self();
    P2W_THREAD_PTR_TYPE pThread;
    
    if( pthread_this_thread != P2W_INVALID_THREAD_ID)
    {
        pThread = p2w_get_thread_by_handle(pthread_this_thread);
        EPS_ASSERT(pThread);
        
        // preset
        memset( hArrayEvents,
                0,
                sizeof(hArrayEvents));

		memset(	lArraySignals,
				0,
				sizeof(lArraySignals));
        
        sigCount = 0;
        for(i = SIGRTMIN; i <= SIGRTMAX; i++)
        {
            if((((LSA_UINT32)1<<i) & (*set)) != 0)
            {
                // add every signal event in set to array of wait events
                hArrayEvents[sigCount] = pThread->hSignalEvents[i-SIGRTMIN];
                lArraySignals[sigCount] = (int)i;
                sigCount++;
            }
        }
        
        if(sigCount > 0)
        {
            // wait for events
            lEventSignaled = WaitForMultipleObjects(sigCount,       // number of objects in array
                                                    hArrayEvents,   // array of objects
                                                    FALSE,          // wait for any signaled event (OR, not AND)
                                                    INFINITE);      // indefinite wait
            
            //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
            //lint --e(685) Relational operator '>=' always evaluates to 'true' 
            if(lEventSignaled >= (WAIT_OBJECT_0) &&
               lEventSignaled < ((WAIT_OBJECT_0) + sigCount))
                // event received
            {
                //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
                *sig = lArraySignals[(lEventSignaled - (WAIT_OBJECT_0))];
                rc = 0;
            }
        }
    }

    return rc;
}

/**
 * Description.
 *
 * @param set     Set of signals to wait for
 * @param info    Pointer to info structure
 * @param timeout Timeout
 *
 * @return 0 upon successful completion, otherwise -1
 */
int sigtimedwait(const sigset_t *set, siginfo_t *info,
                 const struct timespec *abs_timeout)
{
    struct timespec current_time;
    DWORD dwMilliSeconds;
	DWORD lEventSignaled;
    int rc = -1;
    int retValGetTime;
    LSA_UINT32 i, sigCount;
    HANDLE hArrayEvents[((SIGRTMAX-SIGRTMIN)+1)];
    int lArraySignals[((SIGRTMAX - SIGRTMIN) + 1)] = { 0 };

    pthread_t pthread_this_thread = pthread_self();
    P2W_THREAD_PTR_TYPE pThread;
    
    if( pthread_this_thread != P2W_INVALID_THREAD_ID)
    {
        pThread = p2w_get_thread_by_handle(pthread_this_thread);
        EPS_ASSERT(pThread);
        
        // preset
        memset( hArrayEvents,
                0,
                sizeof(hArrayEvents));

		memset(	lArraySignals,
				0,
				sizeof(lArraySignals));
        
        sigCount = 0;
        for(i = SIGRTMIN; i <= SIGRTMAX; i++)
        {
            if((((LSA_UINT32)1<<i) & (*set)) != 0)
            {
                // add every signal event in set to array of wait events
                hArrayEvents[sigCount] = pThread->hSignalEvents[i-SIGRTMIN];
                lArraySignals[sigCount] = (int)i;
                sigCount++;
            }
        }

        if(sigCount > 0)
        {
            retValGetTime = clock_gettime( CLOCK_REALTIME , &current_time );
            EPS_ASSERT(0 == retValGetTime);
        
            dwMilliSeconds = (DWORD) ((abs_timeout->tv_sec - current_time.tv_sec) * 1000);
                
            // wait for events
            lEventSignaled = WaitForMultipleObjects(sigCount,           // number of objects in array
                                                    hArrayEvents,       // array of objects
                                                    FALSE,              // wait for any signaled event (OR, not AND)
                                                    dwMilliSeconds);    // wait time
            
            //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
            //lint --e(685) Relational operator '>=' always evaluates to 'true' 
            if(lEventSignaled >= (WAIT_OBJECT_0) &&
               lEventSignaled < ((WAIT_OBJECT_0) + sigCount))
                // event received
            {
                //lint --e(835) A zero has been given as left argument to operator '+'. WAIT_OBJECT_0 is defined by WinBase.h
                info->si_signo = lArraySignals[(lEventSignaled - (WAIT_OBJECT_0))];
                rc = 0;
            }
        }
    }

    return rc;
}

/**
 * The pthread_kill() function shall request that a signal be delivered to
 * the specified thread.
 *
 * @param thread Thread the signal should be delivered to
 * @param sig The signal that should be delivered (SIGRTMIN to SIGRTMAX are supported)
 *
 * @return 0 upon successful completion, otherwise -1
 */
int pthread_kill(pthread_t thread, int sig)
{
    BOOL rc;
    
    if ((sig < SIGRTMIN) || (sig > SIGRTMAX))
    {
        return -1;
    }

    P2W_THREAD_PTR_TYPE pThread = (P2W_THREAD_PTR_TYPE)thread;

    EPS_ASSERT(pThread != LSA_NULL);

    rc = SetEvent( pThread->hSignalEvents[sig-SIGRTMIN] );
    EPS_ASSERT( rc );

    return 0;
}



/**
 * Returns the current value \c tp for the specified clock, \c clock_id.
 *
 * @param clock_id ID of the clock which time should be returned
 * @param tp #timespec structure where the current time is stored into
 *
 * @return 0 upon successful completion, -1 otherwise.
 */
int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    LARGE_INTEGER now;
    DOUBLE nanosPerTick;
    BOOL retVatCounter; // Note: Don't check retVatCounter with EPS_ASSERT() as it takes too much time

    LSA_UNUSED_ARG(clock_id);
    
    if( g_Eps_P2wTimer.freq.QuadPart == 0 ) 
    {
        retVatCounter = QueryPerformanceFrequency(&g_Eps_P2wTimer.freq);
        LSA_UNUSED_ARG(retVatCounter);

        retVatCounter = QueryPerformanceCounter(&g_Eps_P2wTimer.base);
        LSA_UNUSED_ARG(retVatCounter);
    }

    retVatCounter = QueryPerformanceCounter(&now);
    LSA_UNUSED_ARG(retVatCounter);
    nanosPerTick = 1.0e9 / (DOUBLE)g_Eps_P2wTimer.freq.QuadPart; // don't loose percision in following multiplication. 
    now.QuadPart = (LONGLONG)((now.QuadPart - g_Eps_P2wTimer.base.QuadPart) * nanosPerTick); // return in nsec

    tp->tv_sec  = (LONG)(now.QuadPart / 1000000000); 
    tp->tv_nsec = (LONG)(now.QuadPart % 1000000000);

    return 0;
}

/**
 * Obtains the current time, expressed as seconds and microseconds since the 
 * Epoch, and store it in the timeval structure pointed to by \c tp. 
 * 
 * If tzp is not a null pointer, the behavior is unspecified. 
 * 
 * @param tp Object where the time should be stored 
 * @param tzp Not used. Should always be \c NULL
 *
 * @return Always 0
 */
int gettimeofday(struct timeval * tp, void * tzp)
{
    LARGE_INTEGER now;
    BOOL retVatCounter; // Note: Don't check retVatCounter with EPS_ASSERT() as it takes too much time

    LSA_UNUSED_ARG(tzp);

    if ( g_Eps_P2wTimer.freq.QuadPart == 0 )
    {
        retVatCounter = QueryPerformanceFrequency(&g_Eps_P2wTimer.freq);
        LSA_UNUSED_ARG(retVatCounter);

        retVatCounter = QueryPerformanceCounter(&g_Eps_P2wTimer.base);
        LSA_UNUSED_ARG(retVatCounter);
    }

    retVatCounter = QueryPerformanceCounter(&now);
    LSA_UNUSED_ARG(retVatCounter);

    now.QuadPart= (now.QuadPart- g_Eps_P2wTimer.base.QuadPart);

    tp->tv_sec  = (LONG)(now.QuadPart / g_Eps_P2wTimer.freq.QuadPart); // devide with freq 1/s -> return in sec
    tp->tv_usec = (LONG)(((now.QuadPart * 1000000) / g_Eps_P2wTimer.freq.QuadPart) % 1000000); // multiply with 1000000, than devide with freq -> return in us

    return 0;
}
