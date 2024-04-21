#ifndef _POSIX2WIN_H_
#define _POSIX2WIN_H_
#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: posix2win.h                               :F&  */
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

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
//lint -save -e157 -e114 -e18
#include <winsock2.h>
#include <windows.h>
//lint -restore    

#if (( defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(_CRT_NO_TIME_T)) || defined(lint) )
#include <time.h> /* Visual Studio 2015 has its own "struct timespec" in the included <time.h> */
#endif

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define OS_THREAD_NAME_LEN                              (16 + 1)      // Max thread name length = 16, because Linux does not support more! +1 is for null terminated string

// Scheduling algorithms
#define SCHED_OTHER                                     0   // Another scheduling policy
#define SCHED_FIFO                                      1   // FIFO scheduling policy
#define SCHED_RR                                        2   // Round Robin scheduling policy
#define P2W_SYS_THREAD_DEFAULT_SCHED                    SCHED_RR
#define PTHREAD_EXPLICIT_SCHED                          0x00
#define PTHREAD_INHERIT_SCHED                           0x01
#define PTHREAD_CREATE_JOINABLE                         0x02  ///< Threads created with this attribute will be joinable state
#define EPS_SYS_THREAD_STACK_SIZE_BYTE                  0x20000
/// Minimum size in bytes of thread stack storage
#define PTHREAD_STACK_MIN                               1024                         
#define EPS_SYS_PROC_BYTES                          (32 / 8)
#define CLOCK_REALTIME            0          ///< The identifier of the system-wide realtime clock CLOCK_REALTIME

#define MAX_NANO_SEC              1000000000 ///< Equals 1 second

// signal
#define SIGRT0          1
#define SIGRT1          2
#define SIGRT2          3
#define SIGRT3          4
#define SIGRT4          5
#define SIGRT5          6
#define SIGRT6          7
#define SIGRT7          8
#define SIGRT8          9
#define SIGRT9          10
#define SIGRT10         11
#define SIGRT11         12
#define SIGRT12         13
#define SIGRT13         14
#define SIGRT14         15
#define SIGRT15         16

#define SIGRTMIN        SIGRT0
#define SIGRTMAX        SIGRT15

#define SIGSETMAX       24
  
#define CPU_SETSIZE 32

#define CPU_ZERO(set) ((*set)=0)
#define CPU_FILL(set) ((*set)=~0)
#define CPU_SET(cpu, set) ((*set)|=1<<(cpu))
#define CPU_CLR(cpu, set) ((*set)&=~(1<<(cpu)))
#define CPU_ISSET(cpu, set) ((*set)&(1<<cpu))

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef int32_t             ssize_t;       ///< Used for a count of bytes, can be -1 for errors
//typedef uint32_t            uintptr_t;   /// defined by vargs.h
typedef uint32_t            clockid_t;     ///< Used for clock ID
typedef void*               timer_t;       ///< Used for timer ID
typedef uint32_t            sigset_t;	 
typedef uint32_t            pthread_t;     /// Used to identify a thread          
typedef int32_t             suseconds_t;   ///< Used for time (usec)

#define PTHREAD_PRIO_NONE           0x00   ///< Mutex with this protocol won't protect thread priority
#define PTHREAD_PRIO_PROTECT        0x01   ///< Mutex with this protocol rises thread priority before locking
#define PTHREAD_PRIO_INHERIT        0x02   ///< Not supported in posix2win

#define P2W_INVALID_THREAD_ID       0xFFFF ///< Invalid ThreadId

typedef struct pthread_mutexattr_s 
{
  uint32_t type;                       ///< Mutex type
  int32_t  protocol;                   ///< Protocol for priority handling
  int32_t  prioceiling;                ///< Value of priority ceiling
} pthread_mutexattr_t;                 ///< Used to identify a mutex attr object 

typedef struct pthread_mutex_s
{
  CRITICAL_SECTION    lock;          ///< Windows Mutex for locking CriticalSections in Thread context		
  volatile DWORD      owner;         ///< Handle to thread owning the Mutex
  uint32_t            count;         ///< Recursive count
  int32_t             protocol;      ///< protocol can be PTHREAD_PRIO_NONE/PTHREAD_PRIO_PROTECT
  int32_t             orig_prio;     ///< Store/Restore original priority of thread on PTHREAD_PRIO_PROTECT
} pthread_mutex_t;

/** 
 * Contains the scheduling parameters of each supported scheduling policy
 */
struct sched_param
{
  int sched_priority;     ///< Thread execution scheduling priority 
};

typedef unsigned long cpu_set_t;          ///< Bitfield for SMP processor cores

/**
 *  Stores attributes of a thread 
 */
typedef struct pthread_attr_s{
  /**
   * specifies whether the scheduling attributes are inherited from the thread that called
   * pthread_create()
   * Note: POSIX API requires integer value here.
   * @see #PTHREAD_INHERIT_SCHED 
   * @see #PTHREAD_EXPLICIT_SCHED 
   */ 
  int inheritsched;    
  /**
   * specifies the scheduling policy of the thread
   * Note: POSIX API requires integer value here.
   * @see #SCHED_RR    
   * @see #SCHED_FIFO
   */
  int schedpolicy;                
  uint32_t detachstate;           ///< Specifies whether a thread created via this attributes will be detached or joinable
  /**
   * Stores the information if the stack has been dynamically allocated during phtread_create()
   * using kmalloc() and has therefore to be freed when thread is destroyed.
   * Possible values are:
    * - #LSA_TRUE
    * - #LSA_FALSE
   */
  uint32_t stack_dynamically_allocated; 
  //oeg cpu_set_t cpuset;              ///< Stores information on which CPUs a thread is allowed to run
  struct sched_param schedparam;    ///< The scheduling parameters.
  void *stackaddr;                  ///< The address of the threads stack
  size_t stacksize;                 ///< The size of the threads stack
  char name[OS_THREAD_NAME_LEN];   ///< Name of thread at creation time
} pthread_attr_t;

// Value of sigev_notify
#define SIGEV_NONE                1          ///< No asynchronous notification
#define SIGEV_THREAD              2          ///< Notification function called
#define SIGEV_SIGNAL_THREAD    0xff
#define CLOCK_MONOTONIC           1          ///< The identifier of the system-wide read only realtime clock CLOCK_MONOTONIC

/// Sigval structure
union sigval
{
  int sival_int;              ///< Integer signal value
  void *sival_ptr;            ///<Pointer signal value
};

/// Sigevent structure. Defines what to be done when an event (timer etc.) occurs.
struct sigevent 
{
  int sigev_notify;                             ///< Kind of notification
  int sigev_signo;                              ///< Signal number
  union sigval sigev_value;                     ///< Signal value
  void (*sigev_notify_function)(union sigval);  ///< Notification function
  pthread_attr_t* sigev_notify_attributes;      ///< Notification attribute
  uint32_t sigev_thread;                       ///< Thread to be notified
  int cycles;                                   ///< Number of allowed overruns
  /**
   * Counts timer expiration or event notification situations where the destination
   * thread is NOT waiting for the signal.
   */
  uint32_t           cycle_count;              
  uint32_t          creation_thread;          ///< Thread that created the event.
};


#if ( defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(_CRT_NO_TIME_T))
    // Visual Studio 2015 has its own "struct timespec" in the included <time.h>
#else
    /// Define the timespec structure
    //lint --e(39) --e(830) --e(631) Lint seems to not recongize the define of _MSC_VER during the lint run. Setting this value in preprocessor is dangerous and also does not work.
    typedef struct timespec
    {
      time_t        tv_sec;         ///< Seconds 
      long          tv_nsec;        ///< Nanoseconds 
    }timespec; 
#endif

/// Define the itimerspec structure
typedef struct itimerspec
{
    struct timespec it_interval;    ///< Timer period
    struct timespec it_value;       ///< Timer expiration
}itimerspec;

typedef HANDLE sem_t;

/// Siginfo structure
typedef struct siginfo_s
{
  int si_signo;               ///< Signal number     
} siginfo_t;

/*****************************************************************************/
/*  Posix-API Prototypes                                                     */
/*****************************************************************************/
pthread_t pthread_self(void);
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_setschedparam(pthread_attr_t *  attr,const struct sched_param * param);
int pthread_getschedparam(pthread_t thread, int * policy, struct sched_param * param);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);	
int pthread_attr_setname(pthread_attr_t * attr, const char *name);
int pthread_attr_setcpuaffinity(pthread_attr_t *attr, const cpu_set_t * cpuset);
int pthread_create(pthread_t * thread,const pthread_attr_t * attr,void *(*start_routine)(void*), void * arg);
int pthread_join(pthread_t thread, void** th_ret);
int pthread_kill(pthread_t thread, int sig);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t * attr, int * protocol);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
int pthread_attr_getschedpolicy(const pthread_attr_t * attr, int * policy);

int timer_create(clockid_t clockid, struct sigevent * evp,timer_t * timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec * value, struct itimerspec * ovalue);
int timer_delete(timer_t timerid);
int sem_init(sem_t *sem, int pshared, unsigned value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_post(sem_t *sem);
int sigaddset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigwait(const sigset_t *set, int *sig);
int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *abs_timeout);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int gettimeofday(struct timeval * tp, void * tzp);
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, pthread_mutexattr_destroy)
//lint -esym(759, pthread_mutexattr_destroy)
//lint -esym(765, pthread_mutexattr_destroy)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of _POSIX2WIN_H_ */
