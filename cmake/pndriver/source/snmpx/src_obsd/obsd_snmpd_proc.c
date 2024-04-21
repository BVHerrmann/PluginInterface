/*	$OpenBSD: proc.c,v 1.20 2015/12/07 16:05:56 reyk Exp $	*/

/*
 * Copyright (c) 2010 - 2014 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2008 Pierre-Yves Ritschard <pyr@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_proc_act_modul) */
#define LTRC_ACT_MODUL_ID 4054 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_proc_act_modul) */
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_wait.h>
#include <sys/obsd_kernel_tree.h>

#include <net/obsd_kernel_if.h>

#include <obsd_userland_string.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_getopt.h>
#include <obsd_userland_err.h>
#include <obsd_userland_errno.h>
#include <obsd_userland_event.h>
#include <obsd_userland_signal.h>
#include <obsd_userland_unistd.h>

#define OBSD_SNMPD_DEFINE_PRIVSEP_PROCESS /* gh2289n: define privsep_process here in this file    */
#include "obsd_snmpd_snmpd.h"
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

void	 proc_open(struct privsep *, struct privsep_proc *,
	    struct privsep_proc *, size_t);
void	 proc_close(struct privsep *);
int	 proc_ispeer(struct privsep_proc *, unsigned int, enum privsep_procid);
void	 proc_shutdown(struct privsep_proc *);
void	 proc_sig_handler(int, short, void *);
void	 proc_range(struct privsep *, enum privsep_procid, int *, int *);
int	 proc_dispatch_null(int, struct privsep_proc *, struct imsg *);

int
proc_ispeer(struct privsep_proc *procs, unsigned int nproc,
    enum privsep_procid type)
{
	unsigned int	i;

	for (i = 0; i < nproc; i++)
		if (procs[i].p_id == type)
			return (1);
	return (0);
}

void
proc_init(struct privsep *ps, struct privsep_proc *procs, unsigned int nproc)
{
	unsigned int		 i;

	/*
	 * Setup and run the parent and its children
	 */
	privsep_process = PROC_PARENT;
	ps->ps_title[PROC_PARENT] = "parent";
	ps->ps_pid[PROC_PARENT] = getpid();

	for (i = 0; i < nproc; i++) {
		/* Default to 1 process instance */
		ps->ps_title[procs[i].p_id] = procs[i].p_title;
	}

	proc_open(ps, NULL, procs, nproc);

	/* Engage! */
	for (i = 0; i < nproc; i++)
	{
		ps->p = &procs[i];   /* gerlach/gh2289n: PN variant: store the privsep_proc for shutdown later   */
		procs[i].p_ps = ps; /* gh2289n: need to close the server socket during shutdown, so we need the */
		              /*          ps. which holds the socket within it's ps_env                   */
		ps->ps_pid[procs[i].p_id] = (*procs[i].p_init)(ps, &procs[i]);
	}
}

void
proc_kill(struct privsep *ps)
{
	/* gerlach/gh2289n: PN variant: have no child processes, direct call of shutdown function */
	struct privsep_proc	 *p = ps->p;
	proc_shutdown(p);
}

void
proc_open(struct privsep *ps, struct privsep_proc *p,
    struct privsep_proc *procs, size_t nproc)
{
	OBSD_UNUSED_ARG(ps);
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(procs);
	OBSD_UNUSED_ARG(nproc);
}

void
proc_listen(struct privsep *ps, struct privsep_proc *procs, size_t nproc)
{
	u_int i; /* OBSD_ITGR -- avoid warning */

	for (i = 0; i < nproc; i++) 
	{
		procs[i].p_ps = ps;
		procs[i].p_env = ps->ps_env;
	}
}

void
proc_close(struct privsep *ps)
{
	OBSD_UNUSED_ARG(ps);
}

void
proc_shutdown(struct privsep_proc *p)
{
#ifdef OBSD_SNMP_AGENTX
	if (p->p_id == PROC_CONTROL && ps)
		control_cleanup(&ps->ps_csock);
#endif

	if (p->p_shutdown != NULL)
		(*p->p_shutdown)();

	                            /*      so it's not a good idea to destroy our one & only process */
	/* exit(0); */              /* in a single thread environment we had to return via a simple   */
	                            /* exit instead of the _exit() system call. But luckily we can    */
	                            /* return without exit() here, this is true for this version of   */
	                            /* the SNMPD and avoids the exit() so far                         */
}

void
proc_sig_handler(int sig, short event, void *arg)
{
	struct privsep_proc	*p = arg;
	OBSD_UNUSED_ARG(event); /* OBSD_ITGR -- avoid warning */

	switch (sig) {
	case SIGINT:
	case SIGTERM:
		proc_shutdown(p);
		break;
	case SIGCHLD:
	case SIGHUP:
	case SIGPIPE:
	case SIGUSR1:
		/* ignore */
		break;
	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_proc_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "proc_sig_handler: unexpected signal");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_proc_001) */
		fatalx("proc_sig_handler: unexpected signal");
		/* NOTREACHED */
	}
}


pid_t
proc_run(struct privsep *ps, struct privsep_proc *p,
    struct privsep_proc *procs, unsigned int nproc,
    void (*run)(struct privsep *, struct privsep_proc *, void *), void *arg)
{





	OBSD_UNUSED_ARG(p); /* OBSD_ITGR -- avoid warning */
	ps->sc_evbase = event_init();/* gerlach/gh2289n: need to store event_base for shotdown   */


	proc_listen(ps, procs, nproc);


	if (run != NULL)
		run(ps, p, arg);

	event_loop(EVLOOP_NONBLOCK); /* gerlach/gh2289n: PN variant: don't wait here and assume PN    */
	                             /* has it's own cycle that calls event_loop(EVLOOP_NONBLOCK)     */
	                             /*(like provided by obsd_snmpd_handle_events) to check for events*/

	return (0);
}

void
proc_dispatch(int fd, short event, void *arg)
{
	OBSD_UNUSED_ARG(fd);
	OBSD_UNUSED_ARG(event);
	OBSD_UNUSED_ARG(arg);
}

