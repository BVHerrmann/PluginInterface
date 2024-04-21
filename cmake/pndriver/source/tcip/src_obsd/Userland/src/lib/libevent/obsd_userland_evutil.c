/*	$OpenBSD: evutil.c,v 1.3 2010/07/12 18:03:38 nicm Exp $	*/

/*
 * Copyright (c) 2007 Niels Provos <provos@citi.umich.edu>
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
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

/* #include <sys/obsd_kernel_types.h>  removed OBSD_ITGR */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/obsd_kernel_socket.h>
#endif
#ifdef HAVE_UNISTD_H
#include <obsd_userland_unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <obsd_kernel_fcntl.h>
#endif
#ifdef HAVE_STDLIB_H
#include <obsd_userland_stdlib.h>
#endif

#include <obsd_userland_errno.h>
#if defined WIN32 && !defined(HAVE_GETTIMEOFDAY_H)
#include <sys/timeb.h>
#endif
#include <obsd_userland_stdio.h>
#include <obsd_userland_signal.h>

#include <sys/obsd_kernel_queue.h>
#include "obsd_userland_event.h"
#include "obsd_userland_event-internal.h"
#include "obsd_userland_evutil.h"
#include "obsd_userland_log.h"


int
evutil_make_socket_nonblocking(int fd)
{
#ifdef WIN32
	{
		unsigned long nonblocking = 1;
		ioctlsocket(fd, FIONBIO, (unsigned long*) &nonblocking);
	}
#else
	{
		int flags;
		if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
			event_warn("fcntl(%d, F_GETFL)", fd);
			return -1;
		}
		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			event_warn("fcntl(%d, F_SETFL)", fd);
			return -1;
		}
	}
#endif
	return 0;
}


#ifndef HAVE_GETTIMEOFDAY
int
evutil_gettimeofday(struct timeval *tv, struct timezone *tz)
{
	struct _timeb tb;

	if(tv == NULL)
		return -1;

	_ftime(&tb);
	tv->tv_sec = (long) tb.time;
	tv->tv_usec = ((int) tb.millitm) * 1000;
	return 0;
}
#endif

static int
evutil_issetugid(void)
{
#ifdef HAVE_ISSETUGID
	return issetugid();
#else

#ifdef HAVE_GETEUID
	if (getuid() != geteuid())
		return 1;
#endif
#ifdef HAVE_GETEGID
	if (getgid() != getegid())
		return 1;
#endif
	return 0;
#endif
}

const char *
evutil_getenv(const char *varname)
{
	if (evutil_issetugid())
		return NULL;

	OBSD_UNUSED_ARG(varname);  /* gh2289n: avoid compiler warning */
	return NULL;
}
