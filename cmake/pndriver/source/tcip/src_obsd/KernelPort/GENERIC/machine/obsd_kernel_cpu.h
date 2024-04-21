/*	$OpenBSD: cpu.h,v 1.121 2011/11/02 23:53:44 jsg Exp $	*/
/*	$NetBSD: cpu.h,v 1.35 1996/05/05 19:29:26 christos Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	@(#)cpu.h	5.4 (Berkeley) 5/9/91
 */

#ifndef _MACHINE_CPU_H_
#define _MACHINE_CPU_H_

/*
 * Definitions unique to i386 cpu support.
 */

#include <machine/obsd_kernel_intr.h>
#include <sys/obsd_kernel_device.h>
#include <sys/obsd_kernel_lock.h>			/* will also get LOCKDEBUG */

#include <sys/obsd_kernel_sched.h>

struct intrsource;

#ifdef _KERNEL
/* XXX stuff to move to cpuvar.h later */
struct cpu_info {
	u_int32_t ci_randseed;
	u_int32_t	ci_ipending;   /* gh2289n: needed for softinterrupt system */
	int		ci_ilevel;         /* gh2289n: not used */
	int		ci_idepth;         /* gh2289n: needed for softinterrupt system */
	u_int32_t	ci_imask[NIPL];
	u_int32_t	ci_iunmask[NIPL];

};

#endif /* _KERNEL */

#define MAXCPUS 1               /* normally in machine/cpu.h           */
extern void hw_initclocks(void);
extern void hw_deinitclocks(void);
extern struct cpu_info *curcpu(void);  

#endif /* !_MACHINE_CPU_H_ */
