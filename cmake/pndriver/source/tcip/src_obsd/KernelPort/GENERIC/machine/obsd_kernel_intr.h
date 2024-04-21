/*	$OpenBSD: intr.h,v 1.43 2011/07/05 17:11:07 oga Exp $	*/
/*	$NetBSD: intr.h,v 1.5 1996/05/13 06:11:28 mycroft Exp $	*/

/*
 * Copyright (c) 1996 Charles M. Hannum.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Charles M. Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MACHINE_INTR_H_
#define _MACHINE_INTR_H_


/* #include <sys/mutex.h> */
#ifndef   OBSD_KERNEL_INTR_H_LEAN      /* gh2289n: allow includes without including the CPU stuff too */
#include <machine/obsd_kernel_cpu.h>
#endif /* OBSD_KERNEL_INTR_H_LEAN */

#define IPL_NONE        0  /* nothing */
#define IPL_SOFTCLOCK   1  /* timeouts */
#define IPL_SOFTNET     2  /* protocol stacks */

#define IPL_SOFT        IPL_SOFTNET   /* highest soft interrupt level */

#define IPL_BIO         3  /* block I/O */
#define	IPL_NET		    4  /* network */
#define	IPL_VM		    7  /* memory allocation */
#define	IPL_CLOCK       9  /* clock */
#define	IPL_SCHED	IPL_CLOCK
#define IPL_HIGH       10  /* everything, IPL_HIGH must block everything that can manipulate a run queue */
#define NIPL           11  /* gh2289n: IPL count */

#define SI_TO_IRQBIT(x)         (1 << (x))


#endif /* !_MACHINE_INTR_H_ */
