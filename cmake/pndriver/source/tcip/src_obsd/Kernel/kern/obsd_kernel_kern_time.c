/*	$OpenBSD: kern_time.c,v 1.71 2010/06/30 01:47:35 tedu Exp $	*/
/*	$NetBSD: kern_time.c,v 1.20 1996/02/18 11:57:06 fvdl Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)kern_time.c	8.4 (Berkeley) 5/26/95
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_time_act_module) */
#define LTRC_ACT_MODUL_ID 4005 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_time_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_vnode.h>
#include <sys/obsd_kernel_signalvar.h>
#ifdef __HAVE_TIMECOUNTER
#include <sys/obsd_kernel_timetc.h>
#endif

#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_syscallargs.h>

#include <machine/obsd_kernel_cpu.h>
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

#ifdef __HAVE_TIMECOUNTER
struct timeval adjtimedelta;		/* unapplied time correction */
#else
int	tickdelta;			/* current clock skew, us. per tick */
long	timedelta;			/* unapplied time correction, us. */
long	bigadj = 1000000;		/* use 10x skew above bigadj us. */
int64_t	ntp_tick_permanent;
int64_t	ntp_tick_acc;
#endif


void	itimerround(struct timeval *);

/* 
 * Time of day and interval timer support.
 *
 * These routines provide the kernel entry points to get and set
 * the time-of-day and per-process interval timers.  Subroutines
 * here provide support for adding and subtracting timeval structures
 * and decrementing interval timers, optionally reloading the interval
 * timers when they expire.
 */

/* This function is used by clock_settime and settimeofday */
#ifdef __HAVE_TIMECOUNTER
int
settime(struct timespec *ts)
{
	struct timespec now;

	/*
	 * Adjtime in progress is meaningless or harmful after
	 * setting the clock. Cancel adjtime and then set new time.
	 */
	adjtimedelta.tv_usec = 0;
	adjtimedelta.tv_sec = 0;

	/*
	 * Don't allow the time to be set forward so far it will wrap
	 * and become negative, thus allowing an attacker to bypass
	 * the next check below.  The cutoff is 1 year before rollover
	 * occurs, so even if the attacker uses adjtime(2) to move
	 * the time past the cutoff, it will take a very long time
	 * to get to the wrap point.
	 *
	 * XXX: we check against INT_MAX since on 64-bit
	 *	platforms, sizeof(int) != sizeof(long) and
	 *	time_t is 32 bits even when atv.tv_sec is 64 bits.
	 */
	if (ts->tv_sec > INT_MAX - 365*24*60*60) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_time_001) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "denied attempt to set clock forward to %ld", ts->tv_sec);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_time_001) */
		printf("denied attempt to set clock forward to %ld\n",
		    ts->tv_sec);
		return (EPERM);
	}
	/*
	 * If the system is secure, we do not allow the time to be
	 * set to an earlier value (it may be slowed using adjtime,
	 * but not set back). This feature prevent interlopers from
	 * setting arbitrary time stamps on files.
	 */
	nanotime(&now);
	if (securelevel > 1 && timespeccmp(ts, &now, <)) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_time_002) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "denied attempt to set clock back %ld seconds", now.tv_sec - ts->tv_sec);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_time_002) */
		printf("denied attempt to set clock back %ld seconds\n",
		    now.tv_sec - ts->tv_sec);
		return (EPERM);
	}

	tc_setclock(ts);
	return (0);
}
#else
int
settime(struct timespec *ts)
{
	struct timeval delta, tvv, *tv;

	/* XXX - Ugh. */
	tv = &tvv;
	tvv.tv_sec = ts->tv_sec;
	tvv.tv_usec = ts->tv_nsec / 1000;

	/*
	 * Don't allow the time to be set forward so far it will wrap
	 * and become negative, thus allowing an attacker to bypass
	 * the next check below.  The cutoff is 1 year before rollover
	 * occurs, so even if the attacker uses adjtime(2) to move
	 * the time past the cutoff, it will take a very long time
	 * to get to the wrap point.
	 *
	 * XXX: we check against INT_MAX since on 64-bit
	 *	platforms, sizeof(int) != sizeof(long) and
	 *	time_t is 32 bits even when atv.tv_sec is 64 bits.
	 */
	if (tv->tv_sec > INT_MAX - 365*24*60*60) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_time_003) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "denied attempt to set clock forward to %ld", tv->tv_sec);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_time_003) */
		printf("denied attempt to set clock forward to %ld\n",
		    tv->tv_sec);
		return (EPERM);
	}
	/*
	 * If the system is secure, we do not allow the time to be
	 * set to an earlier value (it may be slowed using adjtime,
	 * but not set back). This feature prevent interlopers from
	 * setting arbitrary time stamps on files.
	 */
	if (securelevel > 1 && timercmp(tv, &time, <)) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_time_004) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "denied attempt to set clock back %ld seconds", time_second - tv->tv_sec);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_time_004) */
		printf("denied attempt to set clock back %ld seconds\n",
		    time_second - tv->tv_sec);
		return (EPERM);
	}

	/* WHAT DO WE DO ABOUT PENDING REAL-TIME TIMEOUTS??? */
	timersub(tv, &time, &delta);
	time = *tv;
	timeradd(&boottime, &delta, &boottime);

	/*
	 * Adjtime in progress is meaningless or harmful after
	 * setting the clock.
	 */
	tickdelta = 0;
	timedelta = 0;

	resettodr();

	return (0);
}
#endif


#undef  clock_gettime
#define clock_gettime kernel_clock_gettime 
int
clock_gettime(struct proc *p, clockid_t clock_id, struct timespec *tp)
{
	OBSD_UNUSED_ARG(p);

	switch (clock_id) {
	case CLOCK_REALTIME:
		nanotime(tp);
		break;
	case CLOCK_MONOTONIC:
		nanouptime(tp);
		break;
	case CLOCK_PROF:
		return (EINVAL);
		/* sado -- omit break; Greenhills compiler warning 111 - statement is unreachable */
	default:
		return (EINVAL);
	}
	return (0);
}

/* ARGSUSED */
int
sys_clock_gettime(struct proc *p, void *v, register_t *retval)
{
	struct sys_clock_gettime_args /* {
		syscallarg(clockid_t) clock_id;
		syscallarg(struct timespec *) tp;
	} */ *uap = v;
	struct timespec ats;
	int error;

	OBSD_UNUSED_ARG(retval);
	if ((error = clock_gettime(p, SCARG(uap, clock_id), &ats)) != 0)
		return (error);

	return copyout(&ats, SCARG(uap, tp), sizeof(ats));
}


/* ARGSUSED */
int
sys_gettimeofday(struct proc *p, void *v, register_t *retval)
{
	struct sys_gettimeofday_args /* {
		syscallarg(struct timeval *) tp;
		syscallarg(struct timezone *) tzp;
	} */ *uap = v;
	struct timeval atv;
	struct timeval *tp;
	struct timezone *tzp;
	int error = 0;

	OBSD_UNUSED_ARG(retval); 
	OBSD_UNUSED_ARG(p); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	tp = SCARG(uap, tp);
	tzp = SCARG(uap, tzp);

	if (tp) {
		microtime(&atv);
		if (((error = copyout(&atv, tp, sizeof (atv))) !=0))      /* gh2289n: added 0 comparison to avoid PN compiler error */
			return (error);
	}
	if (tzp)
		error = copyout(&tz, tzp, sizeof (tz));
	return (error);
}

/* gh2289n: itimerfix is used by sys_select und sys_poll */
 
/*
 * Check that a proposed value to load into the .it_value or
 * .it_interval part of an interval timer is acceptable.
 */
int
itimerfix(struct timeval *tv)
{

	if (tv->tv_sec < 0 || tv->tv_sec > 100000000 ||
	    tv->tv_usec < 0 || tv->tv_usec >= 1000000)
		return (EINVAL);

	if (tv->tv_sec == 0 && tv->tv_usec != 0 && tv->tv_usec < tick)
		tv->tv_usec = tick;

	return (0);
}

/* gerlach/gh2289n: ratecheck is needed in subr_pool */

/*
 * ratecheck(): simple time-based rate-limit checking.  see ratecheck(9)
 * for usage and rationale.
 */
int
ratecheck(struct timeval *lasttime, const struct timeval *mininterval)
{
	struct timeval tv, delta;
	int rv = 0;

	getmicrouptime(&tv);

	timersub(&tv, lasttime, &delta);

	/*
	 * check for 0,0 is so that the message will be seen at least once,
	 * even if interval is huge.
	 */
	if (timercmp(&delta, mininterval, >=) ||
	    (lasttime->tv_sec == 0 && lasttime->tv_usec == 0)) {
		*lasttime = tv;
		rv = 1;
	}

	return (rv);
}


/* gerlach/gh2289n: ppsratecheck is needed for icmp and tcp */

/*
 * ppsratecheck(): packets (or events) per second limitation.
 */
int
ppsratecheck(struct timeval *lasttime, int *curpps, int maxpps)
{
	struct timeval tv, delta;
	int rv;

	microuptime(&tv);

	timersub(&tv, lasttime, &delta);

	/*
	 * check for 0,0 is so that the message will be seen at least once.
	 * if more than one second have passed since the last update of
	 * lasttime, reset the counter.
	 *
	 * we do increment *curpps even in *curpps < maxpps case, as some may
	 * try to use *curpps for stat purposes as well.
	 */
	if (maxpps == 0)
		rv = 0;
	else if ((lasttime->tv_sec == 0 && lasttime->tv_usec == 0) ||
	    delta.tv_sec >= 1) {
		*lasttime = tv;
		*curpps = 0;
		rv = 1;
	} else if (maxpps < 0)
		rv = 1;
	else if (*curpps < maxpps)
		rv = 1;
	else
		rv = 0;

#if 1 /*DIAGNOSTIC?*/
	/* be careful about wrap-around */
	if (*curpps + 1 > *curpps)
		*curpps = *curpps + 1;
#else
	/*
	 * assume that there's not too many calls to this function.
	 * not sure if the assumption holds, as it depends on *caller's*
	 * behavior, not the behavior of this function.
	 * IMHO it is wrong to make assumption on the caller's behavior,
	 * so the above #if is #if 1, not #ifdef DIAGNOSTIC.
	 */
	*curpps = *curpps + 1;
#endif

	return (rv);
}
