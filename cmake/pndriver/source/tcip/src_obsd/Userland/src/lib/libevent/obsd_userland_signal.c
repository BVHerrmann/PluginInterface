/*	$OpenBSD: signal.c,v 1.15 2010/07/12 18:03:38 nicm Exp $	*/

/*
 * Copyright 2000-2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
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

#include <sys/obsd_kernel_types.h> /* added OBSD_ITGR -- Types need endianess --> lsa_cfg.h --> _MSC_VER for Visual Studio, but undefined by libevent_config.h */

/*#ifdef    BSD_STACKPORT*/ /* gerlach/gh2289n: would like to use condition here, but PN integrat.*/
                            /* has no chance to set BSD_STACKPORT in some project definitions     */
#include "obsd_userland_libevent_config.h"
/*#endif*/ /* BSD_STACKPORT */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
/* #include <sys/obsd_kernel_types.h>  removed OBSD_ITGR */
#ifdef HAVE_SYS_TIME_H
#include <sys/obsd_kernel_time.h>
#endif
#include <sys/obsd_kernel_queue.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/obsd_kernel_socket.h>
#endif
#include <obsd_userland_signal.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_string.h>
#ifdef HAVE_UNISTD_H
#include <obsd_kernel_unistd.h>
#endif
#include <obsd_userland_errno.h>
#ifdef HAVE_FCNTL_H
#include <obsd_kernel_fcntl.h>
#endif
#include <obsd_userland_assert.h>

#include "obsd_userland_event.h"
#include "obsd_userland_event-internal.h"
#include "obsd_userland_evsignal.h"
#include "obsd_userland_evutil.h"
#include "obsd_userland_log.h"

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_proc.h>

#include "obsd_userland_unistd.h" /* gh2289n: need close() prototype to avoid compiler warning */
                                  /*        needed as long as we use ev_signal_pair (only v5.4)*/

/* gh2289n: can't use global variables, because we have only one instance                  */
#define evsignal_base   (*get_evsignal_base())

struct event_base **get_evsignal_base()
{
    return (struct event_base **)&(obsd_get_upls()->upls_current_base);
}

static void evsignal_handler(int sig);

#ifdef HAVE_SETFD
#define FD_CLOSEONEXEC(x) do { \
        if (fcntl(x, F_SETFD, 1) == -1) \
                event_warn("fcntl(%d, F_SETFD)", x); \
} while (0)
#else
#define FD_CLOSEONEXEC(x)
#endif

int
evsignal_init(struct event_base *base)
{
	int i;

	base->sig.sh_old = NULL;
	base->sig.sh_old_max = 0;
	base->sig.evsignal_caught = 0;
	memset(&base->sig.evsigcaught, 0, sizeof(sig_atomic_t)*NSIG);
	/* initialize the queues for all events */
	for (i = 0; i < NSIG; ++i)
		TAILQ_INIT(&base->sig.evsigevents[i]);

	base->sig.ev_signal.ev_base = base;
	base->sig.ev_signal.ev_flags |= EVLIST_INTERNAL;

	return 0;
}

/* Helper: set the signal handler for evsignal to handler in base, so that
 * we can restore the original handler when we clear the current one. */
int
_evsignal_set_handler(struct event_base *base,
		      int evsignal, void (*handler)(int))
{
#ifdef HAVE_SIGACTION
	struct sigaction sa;
#else
	ev_sighandler_t sh;
#endif
	struct evsignal_info *sig = &base->sig;
	void *p;

	/*
	 * resize saved signal handler array up to the highest signal number.
	 * a dynamic array is used to keep footprint on the low side.
	 */
	if (evsignal >= sig->sh_old_max) {
		int new_max = evsignal + 1;
		event_debug(("%s: evsignal (%d) >= sh_old_max (%d), resizing",
			    __func__, evsignal, sig->sh_old_max));
		p = realloc(sig->sh_old, new_max * sizeof(*sig->sh_old));
		if (p == NULL) {
			event_warn("realloc");
			return (-1);
		}

		memset((char *)p + sig->sh_old_max * sizeof(*sig->sh_old),
		    0, (new_max - sig->sh_old_max) * sizeof(*sig->sh_old));

		sig->sh_old_max = new_max;
		sig->sh_old = p;
	}

	/* allocate space for previous handler out of dynamic array */
	sig->sh_old[evsignal] = malloc(sizeof *sig->sh_old[evsignal]);
	if (sig->sh_old[evsignal] == NULL) {
		event_warn("malloc");
		return (-1);
	}

	/* save previous handler and setup new handler */
#ifdef HAVE_SIGACTION
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);

	if (sigaction(evsignal, &sa, sig->sh_old[evsignal]) == -1) {
		event_warn("sigaction");
		free(sig->sh_old[evsignal]);
		sig->sh_old[evsignal] = NULL;
		return (-1);
	}
#else
	if ((sh = signal(evsignal, handler)) == SIG_ERR) {
		event_warn("signal");
		free(sig->sh_old[evsignal]);
		sig->sh_old[evsignal] = NULL;
		return (-1);
	}
	*sig->sh_old[evsignal] = sh;
#endif

	return (0);
}

int
evsignal_add(struct event *ev)
{
	int evsignal;
	struct event_base *base = ev->ev_base;
	struct evsignal_info *sig = &ev->ev_base->sig;

	if (ev->ev_events & (EV_READ|EV_WRITE))
		event_errx(1, "%s: EV_SIGNAL incompatible use", __func__);
	evsignal = EVENT_SIGNAL(ev);
	assert(evsignal >= 0 && evsignal < NSIG);
	if (TAILQ_EMPTY(&sig->evsigevents[evsignal])) {
		event_debug(("%s: %p: changing signal handler", __func__, ev));
		if (_evsignal_set_handler(
			    base, evsignal, evsignal_handler) == -1)
			return (-1);

		/* catch signals if they happen quickly */
		evsignal_base = base;

		if (!sig->ev_signal_added) {
			if (event_add(&sig->ev_signal, NULL))
				return (-1);
			sig->ev_signal_added = 1;
		}
	}

	/* multiple events may listen to the same signal */
	TAILQ_INSERT_TAIL(&sig->evsigevents[evsignal], ev, ev_signal_next);

	return (0);
}

int
_evsignal_restore_handler(struct event_base *base, int evsignal)
{
	int ret = 0;
	struct evsignal_info *sig = &base->sig;
#ifdef HAVE_SIGACTION
	struct sigaction *sh;
#else
	ev_sighandler_t *sh;
#endif

	/* restore previous handler */
	sh = sig->sh_old[evsignal];
	sig->sh_old[evsignal] = NULL;
#ifdef HAVE_SIGACTION
	if (sigaction(evsignal, sh, NULL) == -1) {
		event_warn("sigaction");
		ret = -1;
	}
#else
	if (signal(evsignal, *sh) == SIG_ERR) {
		event_warn("signal");
		ret = -1;
	}
#endif
	free(sh);

	return ret;
}

int
evsignal_del(struct event *ev)
{
	struct event_base *base = ev->ev_base;
	struct evsignal_info *sig = &base->sig;
	int evsignal = EVENT_SIGNAL(ev);

	assert(evsignal >= 0 && evsignal < NSIG);

	/* multiple events may listen to the same signal */
	TAILQ_REMOVE(&sig->evsigevents[evsignal], ev, ev_signal_next);

	if (!TAILQ_EMPTY(&sig->evsigevents[evsignal]))
		return (0);

	event_debug(("%s: %p: restoring signal handler", __func__, ev));

	return (_evsignal_restore_handler(ev->ev_base, EVENT_SIGNAL(ev)));
}

static void
evsignal_handler(int sig)
{
	int save_errno = errno;

	if (evsignal_base == NULL) {
		event_warn(
			"%s: received signal %d, but have no base configured",
			__func__, sig);
		return;
	}

	evsignal_base->sig.evsigcaught[sig]++;
	evsignal_base->sig.evsignal_caught = 1;

#ifndef HAVE_SIGACTION
	signal(sig, evsignal_handler);
#endif

	errno = save_errno;
}

void
evsignal_process(struct event_base *base)
{
	struct evsignal_info *sig = &base->sig;
	struct event *ev, *next_ev;
	sig_atomic_t ncalls;
	int i;
	
	base->sig.evsignal_caught = 0;
	for (i = 1; i < NSIG; ++i) {
		ncalls = sig->evsigcaught[i];
		if (ncalls == 0)
			continue;
		sig->evsigcaught[i] -= ncalls;

		for (ev = TAILQ_FIRST(&sig->evsigevents[i]);
		    ev != NULL; ev = next_ev) {
			next_ev = TAILQ_NEXT(ev, ev_signal_next);
			if (!(ev->ev_events & EV_PERSIST))
				event_del(ev);
			event_active(ev, EV_SIGNAL, (short) ncalls);  /* OBSD_ITGR -- omit warning */
		}

	}
}

void
evsignal_dealloc(struct event_base *base)
{
	int i = 0;
	if (base->sig.ev_signal_added) {
		event_del(&base->sig.ev_signal);
		base->sig.ev_signal_added = 0;
	}
	for (i = 0; i < NSIG; ++i) {
		if (i < base->sig.sh_old_max && base->sig.sh_old[i] != NULL)
			_evsignal_restore_handler(base, i);
	}

	base->sig.sh_old_max = 0;

	/* per index frees are handled in evsignal_del() */
	if (base->sig.sh_old) {
		free(base->sig.sh_old);
		base->sig.sh_old = NULL;
	}
}
