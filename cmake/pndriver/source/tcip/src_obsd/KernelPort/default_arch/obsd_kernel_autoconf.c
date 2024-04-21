/*	$OpenBSD: autoconf.c,v 1.88 2011/06/26 23:19:11 tedu Exp $	*/
/*	$NetBSD: autoconf.c,v 1.20 1996/05/03 19:41:56 christos Exp $	*/

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
 *	@(#)autoconf.c	7.1 (Berkeley) 5/9/91
 */

/*
 * Setup the system to run on the current machine.
 *
 * cpu_configure() is called at boot time and initializes the vba
 * device tables and the memory controller monitoring.  Available
 * devices are determined (from possibilities mentioned in ioconf.c),
 * and the drivers are initialized.
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

#include <sys/obsd_kernel_device.h>
#include <machine/obsd_kernel_cpu.h>

#include <sys/obsd_kernel_socket.h>         /* gh2289n: needed for obsd_kernel_if.h                */
#include <net/obsd_kernel_if.h>             /* gh2289n: needed for OBSD_IF_USE_LOOPBACK            */

/* because PNIO don't want a static autoconfiguration we have no ioconf.c file, so place the       */
/* pseudo device table here.                                                                       */

#if       OBSD_IF_USE_LOOPBACK      /* gh2289n: loopback device specific code */
extern void loopattach(int);
extern void loopdetach(int);
#endif /* OBSD_IF_USE_LOOPBACK */

#if       OBSD_CARP
extern void carp_init(int);
extern void carp_fini(int);
#endif /* OBSD_CARP */

#if OBSD_CARP || OBSD_IF_USE_LOOPBACK
struct pdevinit pdevinit[] = 
{
#if       OBSD_IF_USE_LOOPBACK
    { loopattach, 1, loopdetach },
#endif /* OBSD_IF_USE_LOOPBACK */
#if       OBSD_CARP
    { carp_init, 1, carp_fini },
#endif /* OBSD_CARP */
    { NULL,       0, NULL }
};
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */


/*
 * Determine i/o configuration for a machine.
 */
void cpu_configure(void)
{
#if 0  /* gh2289n: no fixed configuration via ioconf.,c anymore because of PNIO requirements */
       /*          network interfaces have to be assigned dynamically                        */
	if (config_rootfound("mainbus", NULL) == NULL)
		panic("cpu_configure: mainbus not configured");
#endif /* 0 */
	cold = 0;
}

void device_register(struct device *dev, void *aux)
{
	OBSD_UNUSED_ARG(dev); 
	OBSD_UNUSED_ARG(aux); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
}

