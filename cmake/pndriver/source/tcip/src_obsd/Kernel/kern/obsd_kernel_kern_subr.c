/*	$OpenBSD: kern_subr.c,v 1.34 2010/09/07 16:21:47 deraadt Exp $	*/
/*	$NetBSD: kern_subr.c,v 1.15 1996/04/09 17:21:56 ragge Exp $	*/

/*
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
 *	@(#)kern_subr.c	8.3 (Berkeley) 1/21/94
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_subr_act_module) */
#define LTRC_ACT_MODUL_ID 4003 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_subr_act_module) */
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
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_resourcevar.h>
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

int
uiomove(void *cp, int n, struct uio *uio)
{
	struct iovec *iov;
	u_int cnt;
	int error = 0;
	/* struct proc *p; sado -- Greenhills compiler warning 550 - was set but never used */
	/* p = uio->uio_procp; */

	while (n > 0 && uio->uio_resid) {
		iov = uio->uio_iov;
		cnt = (u_int) (iov->iov_len); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
		if (cnt == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			continue;
		}
		if (cnt > (u_int)n) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>=' : signed/unsigned mismatch */
			cnt = n;
		switch (uio->uio_segflg) {

		case UIO_USERSPACE:
			if (uio->uio_rw == UIO_READ)
				error = copyout(cp, iov->iov_base, cnt);
			else
				error = copyin(iov->iov_base, cp, cnt);
			if (error)
				return (error);
			break;

		case UIO_SYSSPACE:
			if (uio->uio_rw == UIO_READ)
				error = kcopy(cp, iov->iov_base, cnt);
			else
				error = kcopy(iov->iov_base, cp, cnt);
			if (error)
				return(error);
		}
		iov->iov_base = (caddr_t)iov->iov_base + cnt;
		iov->iov_len -= cnt;
		uio->uio_resid -= cnt;
		uio->uio_offset += cnt;
		cp = (caddr_t)cp + cnt;
		n -= cnt;
	}
	return (error);
}

/*
 * Give next character to user as result of read.
 */
int
ureadc(int c, struct uio *uio)
{
	struct iovec *iov;

	if (uio->uio_resid == 0)
		return (EINVAL);
again:
	if (uio->uio_iovcnt <= 0)
		return (EINVAL);
	iov = uio->uio_iov;
	if (iov->iov_len <= 0) {
		uio->uio_iovcnt--;
		uio->uio_iov++;
		goto again;
	}
	switch (uio->uio_segflg) {

	case UIO_USERSPACE:
	{
		char tmp = (char) c; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */

		if (copyout(&tmp, iov->iov_base, sizeof(char)) != 0)
			return (EFAULT);
	}
		break;

	case UIO_SYSSPACE:
		*(char *)iov->iov_base = (char) c;  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
		break;
	}
	iov->iov_base = (caddr_t)iov->iov_base + 1;
	iov->iov_len--;
	uio->uio_resid--;
	uio->uio_offset++;
	return (0);
}

/*
 * General routine to allocate a hash table.
 */
void *
hashinit(int elements, int type, int flags, u_long *hashmask)
{
	u_long hashsize, i;
	LIST_HEAD(generic, generic) *hashtbl;

	if (elements <= 0)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_kern_subr_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "hashinit: bad cnt");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_kern_subr_001) */
		panic("hashinit: bad cnt");
	}
	for (hashsize = 1; hashsize < (u_long)elements; hashsize <<= 1) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
		continue;
	hashtbl = malloc(hashsize * sizeof(*hashtbl), type, flags);
	if (hashtbl == NULL)
		return NULL;
	for (i = 0; i < hashsize; i++)
		LIST_INIT(&hashtbl[i]);
	*hashmask = hashsize - 1;
	return (hashtbl);
}

int hashdeinit(void * hashtbl, int type)
{
    free(hashtbl, type);
    return 0;
}

/*
 * "Shutdown/startup hook" types, functions, and variables.
 */

struct hook_desc_head startuphook_list =
    TAILQ_HEAD_INITIALIZER(startuphook_list);
struct hook_desc_head shutdownhook_list =
    TAILQ_HEAD_INITIALIZER(shutdownhook_list);
struct hook_desc_head mountroothook_list =
    TAILQ_HEAD_INITIALIZER(mountroothook_list);

void *
hook_establish(struct hook_desc_head *head, int tail, void (*fn)(void *),
    void *arg)
{
	struct hook_desc *hdp;

	hdp = (struct hook_desc *)malloc(sizeof (*hdp), M_DEVBUF, M_NOWAIT);
	if (hdp == NULL)
		return (NULL);

	hdp->hd_fn = fn;
	hdp->hd_arg = arg;
	if (tail)
		TAILQ_INSERT_TAIL(head, hdp, hd_list);
	else
		TAILQ_INSERT_HEAD(head, hdp, hd_list);

	return (hdp);
}

void
hook_disestablish(struct hook_desc_head *head, void *vhook)
{
	struct hook_desc *hdp;

	hdp = vhook;
	TAILQ_REMOVE(head, hdp, hd_list);
	free(hdp, M_DEVBUF);
}

/*
 * Run hooks.  Startup hooks are invoked right after scheduler_start but
 * before root is mounted.  Shutdown hooks are invoked immediately before the
 * system is halted or rebooted, i.e. after file systems unmounted,
 * after crash dump done, etc.
 */
void
dohooks(struct hook_desc_head *head, int flags)
{
	struct hook_desc *hdp;

	if ((flags & HOOK_REMOVE) == 0) {
		TAILQ_FOREACH(hdp, head, hd_list) {
			(*hdp->hd_fn)(hdp->hd_arg);
		}
	} else {
		while ((hdp = TAILQ_FIRST(head)) != NULL) {
			TAILQ_REMOVE(head, hdp, hd_list);
			(*hdp->hd_fn)(hdp->hd_arg);
			if ((flags & HOOK_FREE) != 0)
				free(hdp, M_DEVBUF);
		}
	}
}
