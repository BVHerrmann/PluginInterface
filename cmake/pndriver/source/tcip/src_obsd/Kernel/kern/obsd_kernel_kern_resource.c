/*	$OpenBSD: kern_resource.c,v 1.37 2011/03/07 07:07:13 guenther Exp $	*/
/*	$NetBSD: kern_resource.c,v 1.38 1996/10/23 07:19:38 matthias Exp $	*/

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
 *	@(#)kern_resource.c	8.5 (Berkeley) 1/21/94
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

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_pool.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_syscallargs.h>

int
sys_setrlimit(struct proc *p, void *v, register_t *retval)
{
	struct sys_setrlimit_args /* {
		syscallarg(int) which;
		syscallarg(const struct rlimit *) rlp;
	} */ *uap = v;
	struct rlimit alim;
	int error;
	OBSD_UNUSED_ARG(retval); /* gh2289n: prevent compiler warning */

	error = copyin((caddr_t)SCARG(uap, rlp), (caddr_t)&alim,
		       sizeof (struct rlimit));
	if (error)
		return (error);
	return (dosetrlimit(p, SCARG(uap, which), &alim));
}

int
dosetrlimit(struct proc *p, u_int which, struct rlimit *limp)
{
	struct rlimit *alimp;
	rlim_t maxlim;

	if (which >= RLIM_NLIMITS)
		return (EINVAL);

	if (which != RLIMIT_NOFILE)
		return (EINVAL);

	alimp = &p->p_rlimit[which];
	if (limp->rlim_cur > alimp->rlim_max ||
	    limp->rlim_max > alimp->rlim_max)
		return (EINVAL);         /*                  They only use the single thread variant                                 */
	if (p->p_p->ps_limit->p_refcnt > 1) {
		struct plimit *l = p->p_p->ps_limit;

		/* limcopy() can sleep, so copy before decrementing refcnt */
		p->p_p->ps_limit = limcopy(l);
		if (p->p_p->ps_limit == NULL)
			return ENOMEM;
		l->p_refcnt--;
		alimp = &p->p_rlimit[which];
	}

	switch (which) {
	case RLIMIT_NOFILE:
		maxlim = maxfiles;
		break;
	case RLIMIT_NPROC:
		maxlim = maxproc;
		break;
	default:
		maxlim = RLIM_INFINITY;
		break;
	}

	if (limp->rlim_max > maxlim)
		limp->rlim_max = maxlim;
	if (limp->rlim_cur > limp->rlim_max)
		limp->rlim_cur = limp->rlim_max;

	*alimp = *limp;
	return (0);
}

/* ARGSUSED */
int
sys_getrlimit(struct proc *p, void *v, register_t *retval)
{
	struct sys_getrlimit_args /* {
		syscallarg(int) which;
		syscallarg(struct rlimit *) rlp;
	} */ *uap = v;

	OBSD_UNUSED_ARG(retval); /* gerlach/gh2289n: prevent compiler warning C4100: unreferenced formal parameter */

	if (SCARG(uap, which) < 0 || SCARG(uap, which) >= RLIM_NLIMITS)
		return (EINVAL);
	return (copyout((caddr_t)&p->p_rlimit[SCARG(uap, which)],
	    (caddr_t)SCARG(uap, rlp), sizeof (struct rlimit)));
}

struct pool plimit_pool;
static int initialized;

/*
 * Make a copy of the plimit structure.
 * We share these structures copy-on-write after fork,
 * and copy when a limit is changed.
 */
struct plimit *
limcopy(struct plimit *lim)
{
	struct plimit *newlim;

	if (!initialized) {
		pool_init(&plimit_pool, sizeof(struct plimit), 0, 0, 0,
		    "plimitpl", &pool_allocator_nointr);
		initialized = 1;
	}

	newlim = pool_get(&plimit_pool, PR_NOWAIT);
	if (newlim == NULL)
		return (struct plimit*)NULL;        /* gh2289n: !!! NOTE: all callers of limcopy() have to handle this case.  */
		                                    /*              Normally this is not the case !!!                        */
	bcopy(lim->pl_rlimit, newlim->pl_rlimit,
	    sizeof(struct rlimit) * RLIM_NLIMITS);
	newlim->p_refcnt = 1;
	return (newlim);
}

void
limfree(struct plimit *lim)
{
	if (--lim->p_refcnt > 0)
		return;
	pool_put(&plimit_pool, lim);
}

void limpool_destroy(void)
{
	/* assume we are called under spl lock and only after no userland process can use our         */
	/* syscalls (here especially sys_setrlimit)                                                   */
	/* initialized = 0;  no access to the static variable here */
	if (initialized) 
	{   /* destroy the pool only if it was initialized ! */
		pool_flush_pages(&plimit_pool);
		pool_destroy(    &plimit_pool);
		initialized = 0;
	}
}
