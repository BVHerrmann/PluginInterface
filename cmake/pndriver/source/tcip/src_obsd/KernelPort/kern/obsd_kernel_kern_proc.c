/*	$OpenBSD: kern_proc.c,v 1.47 2011/09/18 23:20:54 miod Exp $	*/
/*	$NetBSD: kern_proc.c,v 1.14 1996/02/09 18:59:41 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
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
 *	@(#)kern_proc.c	8.4 (Berkeley) 1/4/94
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
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_malloc.h>

#include <sys/obsd_kernel_pool.h>

#include "obsd_kernel_init_main.h"   /* get_kernel_is_initialized */
#include "obsd_platform_osal.h"


struct pool ucred_pool;

/*
 * Initialize global process hashing structures.
 */
void procinit(void)
{
	pool_init(&ucred_pool, sizeof(struct ucred), 0, 0, 0, "ucredpl",         &pool_allocator_nointr);
}

void procdeinit(void)
{
	pool_flush_pages(&ucred_pool);
	pool_destroy(    &ucred_pool);
}

/*
 * Locate a proc (thread) by number
 */
struct proc * pfind(pid_t pid)
{
    OBSD_UNUSED_ARG(pid);
    return &proc0;
}

/* intended to be used by the curproc macro within the Kernel only */
struct proc* proc_get_curproc(void)
{
    return &proc0;                 /* return proc0 which is statically allocated in init_main    */
}


/* registers a new (userland) process with it's name. if the process (identified via the          */
/* OSAL-Thread-ID) is unknown, a new process is allocated and associated to the current thread    */
/* and the new proc is returned.                                                                  */
/* if the process exists, the exist this proc is returned and the proc's name is set to the new   */
/* name (if the new name is != NULL)                                                              */
/* attention: function may return NULL (e.g. in case of process table full)!                      */
/* note: function is intended to be used from the userland only (via syscalls)                    */
struct proc* sys_proc_register(const char * proc_name, struct proc * father)
{
    OBSD_UNUSED_ARG(proc_name);
    OBSD_UNUSED_ARG(father);
    return &proc0;
}



