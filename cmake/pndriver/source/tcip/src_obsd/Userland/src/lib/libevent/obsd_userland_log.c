/*	$OpenBSD: log.c,v 1.6 2010/04/21 20:02:40 nicm Exp $	*/

/*
 * log.c
 *
 * Based on err.c, which was adapted from OpenBSD libc *err* *warn* code.
 *
 * Copyright (c) 2005 Nick Mathewson <nickm@freehaven.net>
 *
 * Copyright (c) 2000 Dug Song <dugsong@monkey.org>
 *
 * Copyright (c) 1993
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
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
/* #include <sys/obsd_kernel_types.h>  removed OBSD_ITGR */
#ifdef HAVE_SYS_TIME_H
#include <sys/obsd_kernel_time.h>
#else
#include <sys/_libevent_time.h>
#endif
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_string.h>
#include <obsd_userland_errno.h>
#include "obsd_userland_event.h"

#include "obsd_userland_log.h"
#include "obsd_userland_evutil.h"


void
event_err(int eval, const char *fmt, ...)
{
	OBSD_UNUSED_ARG(fmt);
	exit(eval);
}

void
event_warn(const char *fmt, ...)
{
	OBSD_UNUSED_ARG(fmt);
}

void
event_errx(int eval, const char *fmt, ...)
{
	OBSD_UNUSED_ARG(fmt);
	exit(eval);
}


void
event_msgx(const char *fmt, ...)
{
	OBSD_UNUSED_ARG(fmt);
}


