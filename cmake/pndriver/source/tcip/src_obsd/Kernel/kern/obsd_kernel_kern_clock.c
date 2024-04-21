/*	$OpenBSD: kern_clock.c,v 1.72 2011/03/07 07:07:13 guenther Exp $	*/
/*	$NetBSD: kern_clock.c,v 1.34 1996/06/09 04:51:03 briggs Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *	@(#)kern_clock.c	8.5 (Berkeley) 1/21/94
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_clock_act_module) */
#define LTRC_ACT_MODUL_ID 4001 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_clock_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_dkstat.h>
#include <sys/obsd_kernel_timeout.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_limits.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_user.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_signalvar.h>
#include <sys/obsd_kernel_sysctl.h>
#ifdef __HAVE_TIMECOUNTER
#include <sys/obsd_kernel_timetc.h>
#endif

#include <machine/obsd_kernel_cpu.h>

#ifdef GPROF
#include <sys/obsd_kernel_gmon.h>
#endif

/*
 * Clock handling routines.
 *
 * This code is written to operate with two timers that run independently of
 * each other.  The main clock, running hz times per second, is used to keep
 * track of real time.  The second timer handles kernel and user profiling,
 * and does resource use estimation.  If the second timer is programmable,
 * it is randomized to avoid aliasing between the two clocks.  For example,
 * the randomization prevents an adversary from always giving up the cpu
 * just before its quantum expires.  Otherwise, it would never accumulate
 * cpu ticks.  The mean frequency of the second timer is stathz.
 *
 * If no second timer exists, stathz will be zero; in this case we drive
 * profiling and statistics off the main clock.  This WILL NOT be accurate;
 * do not do it unless absolutely necessary.
 *
 * The statistics clock may (or may not) be run at a higher rate while
 * profiling.  This profile clock runs at profhz.  We require that profhz
 * be an integral multiple of stathz.
 *
 * If the statistics clock is running fast, it must be divided by the ratio
 * profhz/stathz for statistics.  (For profiling, every tick counts.)
 */

/*
 * Bump a timeval by a small number of usec's.
 */
#define BUMPTIME(t, usec) { \
	volatile struct timeval *tp = (t); \
	long us; \
 \
	tp->tv_usec = us = tp->tv_usec + (usec); \
	if (us >= 1000000) { \
		tp->tv_usec = us - 1000000; \
		tp->tv_sec++; \
	} \
}

int	stathz;
int	schedhz;
int	profhz;
int	profprocs;
/* int	ticks; sado -- not used here, updated in obsd_kernel_kern_timeout.c */
/* static int psdiv, pscnt; sado -- Greenhills compiler warning 550 - set, but never used */
int	psratio;			/* ratio: prof / stat */

long cp_time[CPUSTATES];

#ifndef __HAVE_TIMECOUNTER
int	tickfix, tickfixinterval;	/* used if tick not really integral */
static int tickfixcnt;			/* accumulated fractional error */

volatile time_t time_second;
volatile time_t time_uptime;

volatile struct	timeval time
	__attribute__((__aligned__(__alignof__(quad_t))));
volatile struct	timeval mono_time;
#endif

void	*softclock_si;

/*
 * Initialize clock frequencies and start both clocks running.
 */
void
initclocks(void)
{
	int i;
#ifdef __HAVE_TIMECOUNTER
	extern void inittimecounter(void);
#endif

	/*
	 * Set divisors to 1 (normal case) and let the machine-specific
	 * code do its bit.
	 */
	/* psdiv = pscnt = 1; sado -- Greenhills compiler warning 550 - set, but never used */

	cpu_initclocks();

	/*
	 * Compute profhz/stathz, and fix profhz if needed.
	 */
	i = stathz ? stathz : hz;
	if (profhz == 0)
		profhz = i;
	psratio = profhz / i;

	/* For very large HZ, ensure that division by 0 does not occur later */
	if (tickadj == 0)
		tickadj = 1;

#ifdef __HAVE_TIMECOUNTER
	inittimecounter();
#endif
}

void deinitclocks(void)
{
#ifdef __HAVE_TIMECOUNTER
    /* deinittimecounter(); currently nothing to do */
#endif
    cpu_deinitclocks();
}

/*
 * The real-time timer, interrupting hz times per second.
 */
void
hardclock(struct clockframe *frame)
{
	OBSD_UNUSED_ARG(frame); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

#ifndef __HAVE_TIMECOUNTER
	/*
	 * Increment the time-of-day.  The increment is normally just
	 * ``tick''.  If the machine is one which has a clock frequency
	 * such that ``hz'' would not divide the second evenly into
	 * milliseconds, a periodic adjustment must be applied.  Finally,
	 * if we are still adjusting the time (see adjtime()),
	 * ``tickdelta'' may also be added in.
	 */

	delta = tick;

	if (tickfix) {
		tickfixcnt += tickfix;
		if (tickfixcnt >= tickfixinterval) {
			delta++;
			tickfixcnt -= tickfixinterval;
		}
	}
	/* Imprecise 4bsd adjtime() handling */
	if (timedelta != 0) {
		delta += tickdelta;
		timedelta -= tickdelta;
	}

	/*
	 * ntp_tick_permanent accumulates the clock correction each
	 * tick. The unit is ns per tick shifted left 32 bits. If we have
	 * accumulated more than 1us, we bump delta in the right
	 * direction. Use a loop to avoid long long div; typically
	 * the loops will be executed 0 or 1 iteration.
	 */
	if (ntp_tick_permanent != 0) {
		ntp_tick_acc += ntp_tick_permanent;
		while (ntp_tick_acc >= (1000LL << 32)) {
			delta++;
			ntp_tick_acc -= (1000LL << 32);
		}
		while (ntp_tick_acc <= -(1000LL << 32)) {
			delta--;
			ntp_tick_acc += (1000LL << 32);
		}
	}

	BUMPTIME(&time, delta);
	BUMPTIME(&mono_time, delta);
	time_second = time.tv_sec;
	time_uptime = mono_time.tv_sec;
#else
	tc_ticktock();
#endif

	/*
	 * Update real-time timeout queue.
	 * Process callouts at a very low cpu priority, so we don't keep the
	 * relatively high clock interrupt priority any longer than necessary.
	 */
	if (timeout_hardclock_update())
		softclock(NULL);
}

/*
 * Compute number of hz until specified time.  Used to
 * compute the second argument to timeout_add() from an absolute time.
 */
int
hzto(const struct timeval *tv)
{
	struct timeval now;
	unsigned long ticks;
	long sec, usec;

	/*
	 * If the number of usecs in the whole seconds part of the time
	 * difference fits in a long, then the total number of usecs will
	 * fit in an unsigned long.  Compute the total and convert it to
	 * ticks, rounding up and adding 1 to allow for the current tick
	 * to expire.  Rounding also depends on unsigned long arithmetic
	 * to avoid overflow.
	 *
	 * Otherwise, if the number of ticks in the whole seconds part of
	 * the time difference fits in a long, then convert the parts to
	 * ticks separately and add, using similar rounding methods and
	 * overflow avoidance.  This method would work in the previous
	 * case but it is slightly slower and assumes that hz is integral.
	 *
	 * Otherwise, round the time difference down to the maximum
	 * representable value.
	 *
	 * If ints have 32 bits, then the maximum value for any timeout in
	 * 10ms ticks is 248 days.
	 */
	getmicrotime(&now);
	sec = tv->tv_sec - now.tv_sec;
	usec = tv->tv_usec - now.tv_usec;
	if (usec < 0) {
		sec--;
		usec += 1000000;
	}
	if (sec < 0 || (sec == 0 && usec <= 0)) {
		ticks = 0;
	} else if (sec <= LONG_MAX / 1000000)
		ticks = (sec * 1000000 + (unsigned long)usec + (tick - 1))
		    / tick + 1;
	else if (sec <= LONG_MAX / hz)
		ticks = sec * hz
		    + ((unsigned long)usec + (tick - 1)) / tick + 1;
	else
		ticks = LONG_MAX;
	if (ticks > INT_MAX)
		ticks = INT_MAX;
	return ((int)ticks);
}

/*
 * Compute number of hz in the specified amount of time.
 */
int
tvtohz(const struct timeval *tv)
{
	unsigned long ticks;
	long sec, usec;

	/*
	 * If the number of usecs in the whole seconds part of the time
	 * fits in a long, then the total number of usecs will
	 * fit in an unsigned long.  Compute the total and convert it to
	 * ticks, rounding up and adding 1 to allow for the current tick
	 * to expire.  Rounding also depends on unsigned long arithmetic
	 * to avoid overflow.
	 *
	 * Otherwise, if the number of ticks in the whole seconds part of
	 * the time fits in a long, then convert the parts to
	 * ticks separately and add, using similar rounding methods and
	 * overflow avoidance.  This method would work in the previous
	 * case but it is slightly slower and assumes that hz is integral.
	 *
	 * Otherwise, round the time down to the maximum
	 * representable value.
	 *
	 * If ints have 32 bits, then the maximum value for any timeout in
	 * 10ms ticks is 248 days.
	 */
	sec = tv->tv_sec;
	usec = tv->tv_usec;
	if (sec < 0 || (sec == 0 && usec <= 0))
		ticks = 0;
	else if (sec <= LONG_MAX / 1000000)
		ticks = (sec * 1000000 + (unsigned long)usec + (tick - 1))
		    / tick + 1;
	else if (sec <= LONG_MAX / hz)
		ticks = sec * hz
		    + ((unsigned long)usec + (tick - 1)) / tick + 1;
	else
		ticks = LONG_MAX;
	if (ticks > INT_MAX)
		ticks = INT_MAX;
	return ((int)ticks);
}

/*
 * Return information about system clocks.
 */
int
sysctl_clockrate(char *where, size_t *sizep, void *newp)
{
	struct clockinfo clkinfo;

	/*
	 * Construct clockinfo structure.
	 */
	clkinfo.tick = tick;
	clkinfo.tickadj = tickadj;
	clkinfo.hz = hz;
	clkinfo.profhz = profhz;
	clkinfo.stathz = stathz ? stathz : hz;
	return (sysctl_rdstruct(where, sizep, newp, &clkinfo, sizeof(clkinfo)));
}

#ifndef __HAVE_TIMECOUNTER
/*
 * Placeholders until everyone uses the timecounters code.
 * Won't improve anything except maybe removing a bunch of bugs in fixed code.
 */

void
getmicrotime(struct timeval *tvp)
{
	*tvp = time;
}

void
nanotime(struct timespec *tsp)
{
	struct timeval tv;

	microtime(&tv);
	TIMEVAL_TO_TIMESPEC(&tv, tsp);
}

void
getnanotime(struct timespec *tsp)
{
	struct timeval tv;

	getmicrotime(&tv);
	TIMEVAL_TO_TIMESPEC(&tv, tsp);
}

void
nanouptime(struct timespec *tsp)
{
	struct timeval tv;

	microuptime(&tv);
	TIMEVAL_TO_TIMESPEC(&tv, tsp);
}


void
getnanouptime(struct timespec *tsp)
{
	struct timeval tv;

	getmicrouptime(&tv);
	TIMEVAL_TO_TIMESPEC(&tv, tsp);
}

void
microuptime(struct timeval *tvp)
{
	struct timeval tv;

	microtime(&tv);
	timersub(&tv, &boottime, tvp);
}

void
getmicrouptime(struct timeval *tvp)
{
	*tvp = mono_time;
}
#endif /* __HAVE_TIMECOUNTER */
