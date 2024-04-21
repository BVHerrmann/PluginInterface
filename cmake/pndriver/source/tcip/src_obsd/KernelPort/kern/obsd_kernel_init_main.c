/*	$OpenBSD: init_main.c,v 1.181 2012/01/01 12:17:33 fgsch Exp $	*/
/*	$NetBSD: init_main.c,v 1.84.4.1 1996/06/02 09:08:06 mrg Exp $	*/

/*
 * Copyright (c) 1995 Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1982, 1986, 1989, 1991, 1992, 1993
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
 *	@(#)init_main.c	8.9 (Berkeley) 1/21/94
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
#include <sys/obsd_kernel_filedesc.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_proc.h>

#include <sys/obsd_kernel_resourcevar.h>

#include <sys/obsd_kernel_signalvar.h>

#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_device.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_user.h>

#ifdef SYSVSHM
#include <sys/obsd_kernel_shm.h>
#endif
#ifdef SYSVSEM
#include <sys/obsd_kernel_sem.h>
#endif
#ifdef SYSVMSG
#include <sys/obsd_kernel_msg.h>
#endif
#include <sys/obsd_kernel_domain.h>
#include <sys/obsd_kernel_mbuf.h>
#include <dev/obsd_kernel_rndvar.h>
#include <machine/obsd_kernel_cpu.h>
#include <net/obsd_kernel_if.h>

#if defined(CRYPTO)
#include <crypto/obsd_kernel_cryptodev.h>
#include <crypto/obsd_kernel_cryptosoft.h>
#endif

#if defined(NFSSERVER) || defined(NFSCLIENT)
extern void nfs_init(void);
#endif

#include "obsd_kernel_init_main.h"              /* own interface                                  */
#include "obsd_kernel_portsys.h"

/* Components of the first process -- never freed. */
struct	pgrp pgrp0;
struct	proc proc0;
struct	process process0;
struct	pcred cred0;
struct	plimit limit0;
struct	sigacts sigacts0;

struct	proc *initproc;

int	cmask = CMASK;
extern	struct user *proc0paddr;

struct	vnode *rootvp, *swapdev_vp;
int	boothowto;
struct	timeval boottime;

/* XXX return int so gcc -Werror won't complain */
int	main(void *);
void	check_console(struct proc *);
void	start_init(void *);
void	start_cleaner(void *);
void	start_update(void *);
void	start_reaper(void *);
void	crypto_init(void);
void	init_exec(void);
void	kqueue_init(void);
void	workq_init(void);

extern char sigcode[], esigcode[];


static int kernel_is_initialized = 0;


/*
 * System startup; initialize the world, create process 0, mount root
 * filesystem, and fork to create init and pagedaemon.  Most of the
 * hard work is done in the lower-level initialization routines including
 * startup(), which does memory initialization and autoconfiguration.
 */
/* XXX return int, so gcc -Werror won't complain */
void Kernel_init(void)   /* aka main() in kern\init_main.c */
{
	struct proc *p;
	struct process *pr;
#if OBSD_CARP || OBSD_IF_USE_LOOPBACK  /* gh2289n: loop interface is only available in test variant or when using CARP*/
	struct pdevinit *pdev;
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */
	struct timeval rtv;
	int i;

#if OBSD_CARP || OBSD_IF_USE_LOOPBACK  /* gh2289n: loop interface is only available in test variant or when using CARP */
	extern struct pdevinit pdevinit[];
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */

	/*
	 * Initialize the current process pointer (curproc) before
	 * any possible traps/probes to simplify trap processing.
	 */

	kernel_is_initialized = 0;   /* block syscall access from userland */
	/* initialize all used structures */
	memset(&pgrp0,    0, sizeof(struct pgrp));
	memset(&proc0,    0, sizeof(struct proc));
	memset(&process0, 0, sizeof(struct process));
	memset(&cred0,    0, sizeof(struct pcred));
	memset(&limit0,   0, sizeof(struct plimit));

	procinit();    /* needed for curcpu() */
	p = &proc0;
	p->p_cpu = curcpu();

	/*
	 * Initialize timeouts.
	 */
	timeout_startup();

	random_init();

	/*
	 * Initialize mbuf's.  Do this now because we might attempt to
	 * allocate mbufs or mbuf clusters during autoconfiguration.
	 */
	mbinit();

	/* Initialize sockets. */
	soinit();

    /*
	 * Initialize filedescriptors.
	 */
	filedesc_init();

	/*
	 * Create process 0 (the swapper).
	 */

	process0.ps_mainproc = p;
	process0.ps_refcnt = 1;
	p->p_p = pr = &process0;

	/* Set the default routing table/domain. */
	process0.ps_rtableid = 0;

	p->p_stat = SONPROC;

	/* Init timeouts. */

	/* Create credentials. */
	p->p_cred = &cred0;
	p->p_ucred = crget();
#if    OBSD_POOL_MALLOC_NO_SLEEP != 0       /* gh2289n: PN is single threaded, so don't wait for memory in crget(),  */
                                            /*          that's why we could arrive here without having a valid ucred */
                                            /*          table. That would be very very uncool ...                    */
	if (p->p_ucred == NULL)
		panic("no memory for credentials");             /* OBSD kernel uses p_ucred very frequently, so there's no   */
		                                                /* other chance to handle if we miss our basics, on the      */
		                                                /* other hand it's assumed that this small amount of memory  */
		                                                /* is always available duting stack startup                  */
#endif /* OBSD_POOL_MALLOC_NO_SLEEP */
	p->p_ucred->cr_ngroups = 1;	/* group 0 */

	/* Initialize signal state for process 0. */
	signal_init();
	p->p_sigacts = &sigacts0;
	siginit(p);

	/* Create the file descriptor table. */
	p->p_fd = fdinit(NULL);
#if    OBSD_POOL_MALLOC_NO_SLEEP != 0       /* gh2289n: PN is single threaded, so don't wait for memory in fdinit(), */
                                            /*          that's why we could arrive here without having a valid fd    */
                                            /*          table. That would be very very uncool ...                    */
	if (p->p_fd == NULL)
		panic("no memory for file descriptor table");   /* OBSD userland relies on file descriptors, so there's no   */
		                                                /* other chance to handle if we miss our basics              */
		                                                /* note: !!! because we have no fd's panic can't use any     */
		                                                /* fd's for a panic message                                  */
#endif /* OBSD_POOL_MALLOC_NO_SLEEP */

	/* Create the limits structures. */
	pr->ps_limit = &limit0;
	for (i = 0; i < nitems(p->p_rlimit); i++)
		limit0.pl_rlimit[i].rlim_cur =
		    limit0.pl_rlimit[i].rlim_max = RLIM_INFINITY;

	limit0.pl_rlimit[RLIMIT_NOFILE].rlim_cur = NOFILE;
	limit0.pl_rlimit[RLIMIT_NOFILE].rlim_max = NOFILE_MAX;
	limit0.pl_rlimit[RLIMIT_NPROC].rlim_cur = MAXUPRC;
	limit0.p_refcnt = 1;

	p->p_addr = proc0paddr;				/* XXX */

	/*
	 * We continue to place resource usage info in the
	 * user struct so they're pageable.
	 */
	p->p_stats = &p->p_addr->u_stats;

	random_start();

	/* Initialize the interface/address trees */
	ifinit();

	/* Configure the devices */ /* gh2289n: and enable int's */
	cpu_configure();




	/* Start real time and statistics clocks. */
	initclocks();


#if OBSD_CARP || OBSD_IF_USE_LOOPBACK       /* gh2289n: loop interface is only available in test variant or when using CARP */
	/* Attach pseudo-devices. */
	for (pdev = pdevinit; pdev->pdev_attach != NULL; pdev++)
		if (pdev->pdev_count > 0)
			(*pdev->pdev_attach)(pdev->pdev_count);
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */



	/*
	 * Initialize protocols.  Block reception of incoming packets
	 * until everything is ready.
	 */
	domaininit();
	if_attachdomain();

#ifdef GPROF
	/* Initialize kernel profiling. */
	kmstartup();
#endif

	microtime(&rtv);
	srandom((u_int32_t)(rtv.tv_sec ^ rtv.tv_usec) ^ arc4random());

	kernel_is_initialized = 1;   /* unblock syscall access from userland */
}


void Kernel_deinit(void)
{
#if OBSD_CARP || OBSD_IF_USE_LOOPBACK  /* gh2289n: loop interface is only available in test variant or when using CARP */
    extern struct pdevinit pdevinit[];
    struct pdevinit *pdev;
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */
	struct proc *p = &proc0;

#ifdef OBSD_MEMORY_TRACING
		tcip_trace_statistics(); /* sado - OpenBSD Page Memory Tracing */
#endif /* OBSD_MEMORY_TRACING */

    /* avoid userland threads from entering the kernel via syscalls (again), but threads may      */
    /* execute kernel code or wait in tsleep                                                      */
    kernel_is_initialized = 0;                              /* block syscall access from userland */

    /* assume here that all userland processes have killed / exited or running outside the kernel */

    /* At this point the receive threads have to be stopped e.g. by calling                       */
    /* NETAL_DestroyIpInterfaces(). Otherwise entering the SPL sema here could cause the receive  */
    /* threads waiting forever to get the SPL sema.                                               */

    /* enter the SPL-sema to avoid other threads working in protected kernel code                 */

    /* starting from this point we assume that we have no userland processes executing kernel code*/


    /* destroy the limits pool                                                                    */
    limpool_destroy();


#if OBSD_CARP || OBSD_IF_USE_LOOPBACK       /* gh2289n: loop interface is only available in test variant or when using CARP*/
    /* This step destroys the loop interface and during this detachment some routes may be        */
    /* deleted. That's why this has to be done before domaindeinit (which calls route_deinit which*/
    /* results in the deletion of the routing table). Without having a valid routing-table the    */
    /* loopdetach / loop_clone_destroy will crash.                                                */
    /* Detach pseudo-devices. */
    for (pdev = pdevinit; pdev->pdev_detach != NULL; pdev++)
        if (pdev->pdev_count > 0)
            (*pdev->pdev_detach)(pdev->pdev_count);
#endif /* OBSD_CARP || OBSD_IF_USE_LOOPBACK */

    /* free the pending fd's */
    fdfree(&proc0);     /* this also closes open sockets like routing socket,so it should be done */
                        /* before detaching the domains (which also causes the deinit of the      */
                        /* routing). Doing this here we avoid (one part of) OBSDPN-79             */

    /* if_dettachdomain();    counterpart for if_attachdomain, but currently no need for this     */
    domaindeinit();
    deinitclocks();             /* disestablish softclock and stop the timer thread               */

    ifdeinit();                 /* free ressources that are not destroyed by OBSD                 */

    signal_deinit();

    if (!SLIST_EMPTY(&p->p_upls.umalloc_list_head))
    {
        /* transfer all pending memory allocations of the process to the port layer */
        AddPendingUserlandMallocs(&p->p_upls.umalloc_list_head);
        SLIST_INIT(&(p->p_upls.umalloc_list_head));             /* process's list is empty now */
    }

    /* destroy the process and proc structures */
    procdeinit();
    /* after this all (explicitly) forked processes are destroyed and all procs created           */
    /* implicitly created by proc_create have "userland" threads without a proc                   */

    filedesc_deinit();
    sodeinit();
    mbdeinit();

    /* after this point all pools should be deinitilized and destroyed */
    /* so deinitialize pool management too (which destroys the phpool) */
    pool_management_deinitialize();
}

int get_kernel_is_initialized(void)
{
    return kernel_is_initialized;
}



