/*	$OpenBSD: kern_prot.c,v 1.51 2011/10/15 23:35:29 guenther Exp $	*/
/*	$NetBSD: kern_prot.c,v 1.33 1996/02/09 18:59:42 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1990, 1991, 1993
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
 *	@(#)kern_prot.c	8.6 (Berkeley) 1/21/94
 */

/*
 * System calls related to processes and protection
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
/*#include <sys/acct.h>*/
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_ucred.h>
#include <sys/obsd_kernel_proc.h>

#include <sys/obsd_kernel_pool.h>

/* ARGSUSED */
int
sys_getpid(struct proc *p, void *v, register_t *retval)
{
	OBSD_UNUSED_ARG(v); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */

	*retval = p->p_p->ps_pid;
	return (0);
}

int
sys_getuid(struct proc *p, void *v, register_t *retval)
{
	OBSD_UNUSED_ARG(v); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */

	*retval = p->p_cred->p_ruid;
	return (0);
}

/* ARGSUSED */
int
sys_geteuid(struct proc *p, void *v, register_t *retval)
{
	OBSD_UNUSED_ARG(v); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */

	*retval = p->p_ucred->cr_uid;
	return (0);
}

/*
 * Test whether this process has special user powers.
 * Returns 0 or error.
 */
int
suser(struct proc *p, u_int flags)
{
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(flags); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */
	return 0;   /* currently no user mgmt -> always super user */
}

/*
 * Free a cred structure.
 * Throws away space when ref count gets to 0.
 */
void crfree(struct ucred *cr)
{

	if (--cr->cr_ref == 0)
		pool_put(&ucred_pool, cr);
}

/*
 * Allocate a zeroed cred structure.
 */
struct ucred * crget(void)
{
	struct ucred *cr;

	cr = pool_get(&ucred_pool, PR_NOWAIT|PR_ZERO);
	if (cr == NULL)
		return (struct ucred*)NULL;         /* gh2289n: !!! NOTE: all callers of crget() have to handle this case.  */
		                                    /*              Normally this is not the case !!!                        */
	cr->cr_ref = 1;
	return (cr);
}
