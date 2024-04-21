/*	$OpenBSD: kern_sig.c,v 1.133 2012/01/25 06:12:13 guenther Exp $	*/
/*	$NetBSD: kern_sig.c,v 1.54 1996/04/22 01:38:32 christos Exp $	*/

/*
 * Copyright (c) 1997 Theo de Raadt. All rights reserved. 
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
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
 *	@(#)kern_sig.c	8.7 (Berkeley) 4/18/94
 */

/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#define	SIGPROP		/* include signal properties table */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_signalvar.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_wait.h>
#include <sys/obsd_kernel_pool.h>
#include <sys/obsd_kernel_sched.h>
#include <sys/obsd_kernel_syscallargs.h>

struct pool sigacts_pool;	/* memory pool for sigacts structures */

static int signals_total = 0;

/*
 * Initialize signal-related data structures.
 */
void
signal_init(void)
{
	pool_init(&sigacts_pool, sizeof(struct sigacts), 0, 0, 0, "sigapl",
	    &pool_allocator_nointr);
}

void signal_deinit(void)
{
	/* assume we are called under splsched */
	/* destroy the pools and free it's ressources */
	pool_flush_pages(&sigacts_pool);
	pool_destroy(    &sigacts_pool);
}
/*
 * Create an initial sigacts structure, using the same signal state
 * as p.
 */
struct sigacts *
sigactsinit(struct proc *p)
{
	struct sigacts *ps;

	ps = pool_get(&sigacts_pool, PR_WAITOK);
	memcpy(ps, p->p_sigacts, sizeof(struct sigacts));
	ps->ps_refcnt = 1;
	return (ps);
}


/*
 * Share a sigacts structure.
 */
struct sigacts *
sigactsshare(struct proc *p)
{
	p->p_sigacts->ps_refcnt++;
	return p->p_sigacts;
}

/*
 * Release a sigacts structure.
 */
void
sigactsfree(struct proc *p)
{
	struct sigacts *ps = p->p_sigacts;

	if (--ps->ps_refcnt > 0)
		return;

	p->p_sigacts = NULL;

	pool_put(&sigacts_pool, ps);
}

/* ARGSUSED */
int
sys_sigaction(struct proc *p, void *v, register_t *retval)
{
	struct sys_sigaction_args /* {
		syscallarg(int) signum;
		syscallarg(const struct sigaction *) nsa;
		syscallarg(struct sigaction *) osa;
	} */ *uap = v;
	struct sigaction vec;
	struct sigaction *sa;
	const struct sigaction *nsa;
	struct sigaction *osa;
	struct sigacts *ps = p->p_sigacts;
	int signum;
	int bit, error;

	OBSD_UNUSED_ARG(retval); /* gh2289n: prevent compiler warning */

	signum = SCARG(uap, signum);
	nsa = SCARG(uap, nsa);
	osa = SCARG(uap, osa);

	if (signum <= 0 || signum >= NSIG ||
	    (nsa && (signum == SIGKILL || signum == SIGSTOP)))
		return (EINVAL);
	sa = &vec;
	if (osa) {
		sa->sa_handler = ps->ps_sigact[signum];
		sa->sa_mask = ps->ps_catchmask[signum];
		bit = sigmask(signum);
		sa->sa_flags = 0;
		if ((ps->ps_sigonstack & bit) != 0)
			sa->sa_flags |= SA_ONSTACK;
		if ((ps->ps_sigintr & bit) == 0)
			sa->sa_flags |= SA_RESTART;
		if ((ps->ps_sigreset & bit) != 0)
			sa->sa_flags |= SA_RESETHAND;
		if ((ps->ps_siginfo & bit) != 0)
			sa->sa_flags |= SA_SIGINFO;
		if (signum == SIGCHLD) {
			if ((ps->ps_flags & SAS_NOCLDSTOP) != 0)
				sa->sa_flags |= SA_NOCLDSTOP;
			if ((ps->ps_flags & SAS_NOCLDWAIT) != 0)
				sa->sa_flags |= SA_NOCLDWAIT;
		}
		if ((sa->sa_mask & bit) == 0)
			sa->sa_flags |= SA_NODEFER;
		sa->sa_mask &= ~bit;
		error = copyout(sa, osa, sizeof (vec));
		if (error)
			return (error);
	}
	if (nsa) {
		error = copyin(nsa, sa, sizeof (vec));
		if (error)
			return (error);
		setsigvec(p, signum, sa);
	}
	return (0);
}

void
setsigvec(struct proc *p, int signum, struct sigaction *sa)
{
	struct sigacts *ps = p->p_sigacts;
	int bit;

	bit = sigmask(signum);
	/*
	 * Change setting atomically.
	 */
	ps->ps_sigact[signum] = sa->sa_handler;
	if ((sa->sa_flags & SA_NODEFER) == 0)
		sa->sa_mask |= sigmask(signum);
	ps->ps_catchmask[signum] = sa->sa_mask &~ sigcantmask;
	if (signum == SIGCHLD) {
		if (sa->sa_flags & SA_NOCLDSTOP)
			atomic_setbits_int((u_int32_t *)&ps->ps_flags, SAS_NOCLDSTOP); /* OBSD_ITGR/gh2289n: cast to u_int32_t* to prevent compiler warning */
		else
			atomic_clearbits_int((u_int32_t *)&ps->ps_flags, SAS_NOCLDSTOP); /* OBSD_ITGR/gh2289n: cast to u_int32_t* to prevent compiler warning */
		/*
		 * If the SA_NOCLDWAIT flag is set or the handler
		 * is SIG_IGN we reparent the dying child to PID 1
		 * (init) which will reap the zombie.  Because we use
		 * init to do our dirty work we never set SAS_NOCLDWAIT
		 * for PID 1.
		 */
		if (initproc->p_sigacts != ps &&
		    ((sa->sa_flags & SA_NOCLDWAIT) ||
		    sa->sa_handler == SIG_IGN))
			atomic_setbits_int((u_int32_t *)&ps->ps_flags, SAS_NOCLDWAIT); /* OBSD_ITGR/gh2289n: cast to u_int32_t* to prevent compiler warning */
		else
			atomic_clearbits_int((u_int32_t *)&ps->ps_flags, SAS_NOCLDWAIT); /* OBSD_ITGR/gh2289n: cast to u_int32_t* to prevent compiler warning */
	}
	if ((sa->sa_flags & SA_RESETHAND) != 0)
		ps->ps_sigreset |= bit;
	else
		ps->ps_sigreset &= ~bit;
	if ((sa->sa_flags & SA_SIGINFO) != 0)
		ps->ps_siginfo |= bit;
	else
		ps->ps_siginfo &= ~bit;
	if ((sa->sa_flags & SA_RESTART) == 0)
		ps->ps_sigintr |= bit;
	else
		ps->ps_sigintr &= ~bit;
	if ((sa->sa_flags & SA_ONSTACK) != 0)
		ps->ps_sigonstack |= bit;
	else
		ps->ps_sigonstack &= ~bit;
	/*
	 * Set bit in ps_sigignore for signals that are set to SIG_IGN,
	 * and for signals set to SIG_DFL where the default is to ignore.
	 * However, don't put SIGCONT in ps_sigignore,
	 * as we have to restart the process.
	 */
	if (sa->sa_handler == SIG_IGN ||
	    (sigprop[signum] & SA_IGNORE && sa->sa_handler == SIG_DFL)) {
		atomic_clearbits_int((u_int32_t *)(&p->p_siglist), bit);	/* OBSD_ITGR/gh2289n: cast to u_int32_t* to prevent compiler warning */
		if (signum != SIGCONT)
			ps->ps_sigignore |= bit;	/* easier in psignal */
		ps->ps_sigcatch &= ~bit;
	} else {
		ps->ps_sigignore &= ~bit;
		if (sa->sa_handler == SIG_DFL)
			ps->ps_sigcatch &= ~bit;
		else
			ps->ps_sigcatch |= bit;
	}
}


/*
 * Initialize signal state for process 0;
 * set to ignore signals that are ignored by default.
 */
void
siginit(struct proc *p)
{
	struct sigacts *ps = p->p_sigacts;
	int i;

	/* OBSD_ITGR -- intitialisation of sigacts */
	for (i = 0; i < NSIG; i++)
		ps->ps_sigact[i] = SIG_DFL;

	for (i = 0; i < NSIG; i++)
		if (sigprop[i] & SA_IGNORE && i != SIGCONT)
			ps->ps_sigignore |= sigmask(i);
	ps->ps_flags = SAS_NOCLDWAIT | SAS_NOCLDSTOP;
}


/*
 * type = SPROCESS	process signal, can be diverted (sigwait())
 *	XXX if blocked in all threads, mark as pending in struct process
 * type = STHREAD	thread signal, but should be propagated if unhandled
 * type = SPROPAGATED	propagated to this thread, so don't propagate again
 */
/* gh2289n: current implementation is intended to handle SIGPIPE signals only */
void
ptsignal(struct proc *p, int signum, enum signal_type type)
{
	int prop;
	sig_t action;
	int mask;
	struct process *pr;
	/* struct proc *q; *//* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */
	int wakeparent = 0;

	int do_wakeup_sleeping_process = 0;

	/* Ignore signal if we are exiting */
	if (p->p_flag & P_WEXIT)
		return;

	mask = sigmask(signum);

	pr = p->p_p;


	if (type != SPROPAGATED)
		KNOTE(&pr->ps_klist, NOTE_SIGNAL | signum);

	prop = sigprop[signum];

    {
		/*
		 * If the signal is being ignored,
		 * then we forget about it immediately.
		 * (Note: we don't set SIGCONT in ps_sigignore,
		 * and if it is set to SIG_IGN,
		 * action will be SIG_DFL here.)
		 */
		if (p->p_sigacts->ps_sigignore & mask)
			return;
		if (p->p_sigmask & mask)
			action = SIG_HOLD;
		else if (p->p_sigacts->ps_sigcatch & mask)
			action = SIG_CATCH;
		else {
			action = SIG_DFL;

			if (prop & SA_KILL &&  pr->ps_nice > NZERO)
				 pr->ps_nice = NZERO;

		}

		atomic_setbits_int((u_int32_t *)(&p->p_siglist), mask); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4057 */
	}


	/*
	 * Defer further processing for signals which are held,
	 * except that stopped processes must be continued by SIGCONT.
	 */
	if (action == SIG_HOLD && ((prop & SA_CONT) == 0 || p->p_stat != SSTOP))
		return;

	switch (p->p_stat) {

	case SSLEEP:
		/*
		 * If process is sleeping uninterruptibly
		 * we can't interrupt the sleep... the signal will
		 * be noticed when the process returns through
		 * trap() or syscall().
		 */
		if ((p->p_flag & P_SINTR) == 0)
			goto out;

		/*
		 * All other (caught or default) signals
		 * cause the process to run.
		 */
		goto runfast;
		/*NOTREACHED*/

	case SSTOP:

		/*
		 * Kill signal always sets processes running.
		 */
		if (signum == SIGKILL)
			goto runfast;


		/*
		 * If process is sleeping interruptibly, then simulate a
		 * wakeup so that when it is continued, it will be made
		 * runnable and can look at the signal.  But don't make
		 * the process runnable, leave it stopped.
		 */
		if (p->p_wchan && p->p_flag & P_SINTR)
		{	/* we have no seprate sleep queue and run queue, wha we need to do is to wakeup sleeping processes  */
			/* but: don't doing a wakeup here (we are still under SCHED_LOCK) is no a good idea, because the    */
			/* could cause a thread change by the eternal scheduler. The thread woke up would continue under    */
			/* splsched then (because of SCHED_LOCK)                                                            */
			do_wakeup_sleeping_process = 1;
		}
		goto out;

	default:
		/*
		 * SRUN, SIDL, SZOMB do nothing with the signal,
		 * other than kicking ourselves if we are running.
		 * It will either never be noticed, or noticed very soon.
		 */
		goto out;
	}
	/*NOTREACHED*/

runfast:
	do_wakeup_sleeping_process = 1;

out:
	if (do_wakeup_sleeping_process)
	{
		if(p->p_stat == SSLEEP)
			wakeup(p->p_wchan);
	}

	if (wakeparent)
		wakeup(pr->ps_pptr);
}


/*
 * If the current process has received a signal (should be caught or cause
 * termination, should interrupt current syscall), return the signal number.
 * Stop signals with default action are processed immediately, then cleared;
 * they aren't returned.  This is checked after each entry to the system for
 * a syscall or trap (though this can usually be done without calling issignal
 * by checking the pending signal masks in the CURSIG macro.) The normal call
 * sequence is
 *
 *	while (signum = CURSIG(curproc))
 *		postsig(signum);
 */
int
issignal(struct proc *p)
{
	int signum, mask, prop;
	/*int dolock = (p->p_flag & P_SINTR) == 0; *//* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */
	/*int s; *//* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */

	for (;;) {
		mask = p->p_siglist & ~p->p_sigmask;
		if (p->p_p->ps_flags & PS_PPWAIT)
			mask &= ~stopsigmask;
		if (mask == 0)	 	/* no signal to send */
			return (0);
		signum = ffs((long)mask);
		mask = sigmask(signum);
		atomic_clearbits_int((u_int32_t*)(&p->p_siglist), mask); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4057 */

		/*
		 * We should see pending but ignored signals
		 * only if P_TRACED was on when they were posted.
		 */
		if (mask & p->p_sigacts->ps_sigignore &&
		    (p->p_flag & P_TRACED) == 0)
			continue;
		prop = sigprop[signum];

		/*
		 * Decide whether the signal should be returned.
		 * Return the signal's number, or fall through
		 * to clear it from the pending mask.
		 */
		switch ((long)p->p_sigacts->ps_sigact[signum]) {

		case (long)SIG_DFL:
			/*
			 * Don't take default actions on system processes.
			 */
			if (p->p_pid <= 1) {
				break;		/* == ignore */
			}

			if (prop & SA_IGNORE) {
				/*
				 * Except for SIGCONT, shouldn't get here.
				 * Default action is to ignore; drop it.
				 */
				break;		/* == ignore */
			} else
				goto keep;
			/*NOTREACHED*/

		case (long)SIG_IGN:
			/*
			 * Masking above should prevent us ever trying
			 * to take action on an ignored signal other
			 * than SIGCONT, unless process is traced.
			 */
			if ((prop & SA_CONT) == 0 &&
			    (p->p_flag & P_TRACED) == 0)
			{	/* mh2290: LINT 548   else expected */
				printf("issignal\n");
			}	/* mh2290: LINT 548   else expected */
			break;		/* == ignore */

		default:
			/*
			 * This signal has an action, let
			 * postsig() process it.
			 */
			goto keep;
		}
	}
	/* NOTREACHED */

keep:
	atomic_setbits_int((u_int32_t *)(&p->p_siglist), mask); /*leave the signal for later */ /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4057 */
	return (signum);
}

/*
 * Take the action for the specified signal
 * from the current set of pending signals.
 */
void
postsig(int signum)
{
	struct proc *p = curproc;
	struct sigacts *ps = p->p_sigacts;
	sig_t action;
	/*u_long trapno; */       /* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */
	int mask /*, returnmask*/;/* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */

	mask = sigmask(signum);
	atomic_clearbits_int((u_int32_t*)(&p->p_siglist), mask); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4057 */
	action = ps->ps_sigact[signum];

#ifdef KTRACE
	if (KTRPOINT(p, KTR_PSIG)) {
		/* sado -- moved from above; Greenhills compiler warning 550 - set, but never used */
		union sigval sigval;
		sigval.sival_ptr = 0;
		siginfo_t si;
		
		initsiginfo(&si, signum, trapno, code, sigval);
		ktrpsig(p, signum, action, p->p_flag & P_SIGSUSPEND ?
		    p->p_oldmask : p->p_sigmask, code, &si);
	}
#endif
	if (action == SIG_DFL) {
		/*
		 * Default action, where the default is to kill
		 * the process.  (Other cases were ignored above.)
		 */
		sigexit(p, signum);/* gerlach/gh2289n: in single thread operation sigexit will return without exiting   */
		/* NOTREACHED *//* gerlach/gh2289n: this is true only if OBSD_MULTITHREADING is 1, otherwise sigexit    */
		                /* will return because it makes no sense to exit the one and only thrad we have ...     */
	} else {
		/*
		 * If we get here, the signal must be caught.
		 */
		/*
		 * Set the new mask value and also defer further
		 * occurrences of this signal.
		 *
		 * Special case: user has done a sigpause.  Here the
		 * current mask is not of interest, but rather the
		 * mask from before the sigpause is what we want
		 * restored after the signal processing is completed.
		 */
		p->p_sigmask |= ps->ps_catchmask[signum];
		if ((ps->ps_sigreset & mask) != 0) {
			ps->ps_sigcatch &= ~mask;
			if (signum != SIGCONT && sigprop[signum] & SA_IGNORE)
				ps->ps_sigignore |= mask;
			ps->ps_sigact[signum] = SIG_DFL;
		}
        action(signum);
	}
}

/*
 * Force the current process to exit with the specified signal, dumping core
 * if appropriate.  We bypass the normal tests for masked and caught signals,
 * allowing unrecoverable failures to terminate the process without changing
 * signal state.  Mark the accounting record with the signal termination.
 * If dumping core, save the signal number for the debugger.  Calls exit and
 * does not return.
 */
void
sigexit(struct proc *p, int signum)
{
	/* gh2289n: in case of a single thread we never exit our (single) process, so ignore exit1 here */
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(signum);
}


void
userret(struct proc *p)
{
	int sig;

	while ((sig = CURSIG(p)) != 0)
	{
		signals_total++;
		postsig(sig);
	}
}
