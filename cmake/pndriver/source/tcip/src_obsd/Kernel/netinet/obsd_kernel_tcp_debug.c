/*	$OpenBSD: tcp_debug.c,v 1.20 2004/09/24 15:02:43 markus Exp $	*/
/*	$NetBSD: tcp_debug.c,v 1.10 1996/02/13 23:43:36 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1993
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
 *	@(#)COPYRIGHT	1.1 (NRL) 17 January 1995
 *
 * NRL grants permission for redistribution and use in source and binary
 * forms, with or without modification, of the software and documentation
 * created at NRL provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgements:
 * 	This product includes software developed by the University of
 * 	California, Berkeley and its contributors.
 * 	This product includes software developed at the Information
 * 	Technology Division, US Naval Research Laboratory.
 * 4. Neither the name of the NRL nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THE SOFTWARE PROVIDED BY NRL IS PROVIDED BY NRL AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL NRL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the US Naval
 * Research Laboratory (NRL).
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
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_protosw.h>

#include <net/obsd_kernel_route.h>
#include <net/obsd_kernel_if.h>

#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_in_pcb.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_tcp.h>
#include <netinet/obsd_kernel_tcp_timer.h>
#include <netinet/obsd_kernel_tcp_var.h>
#include <netinet/obsd_kernel_tcpip.h>
#include <netinet/obsd_kernel_tcp_debug.h>
#include <netinet/obsd_kernel_tcp_fsm.h>

#ifdef INET6
#ifndef INET
#include <netinet/obsd_kernel_in.h>
#endif
#include <netinet/obsd_kernel_ip6.h>
#endif /* INET6 */

struct	tcp_debug tcp_debug[TCP_NDEBUG];
int	tcp_debx;

/*
 * Tcp debug routines
 */
void
tcp_trace(short act, short ostate, struct tcpcb *tp, caddr_t headers,
   int req, int len)
{
	struct tcp_debug *td = &tcp_debug[tcp_debx++];
	struct tcpiphdr *ti = (struct tcpiphdr *)headers;
	/* struct tcphdr *th; sado -- Greenhills compiler warning 550 - set but never used */
#ifdef INET6
	struct tcpipv6hdr *ti6 = (struct tcpipv6hdr *)ti;
#endif

	if (tcp_debx == TCP_NDEBUG)
		tcp_debx = 0;
	td->td_time = iptime();
	td->td_act = act;
	td->td_ostate = ostate;
	td->td_tcb = (caddr_t)tp;
	if (tp)
		td->td_cb = *tp;
	else
		bzero((caddr_t)&td->td_cb, sizeof (*tp));
	switch (tp->pf) {
#ifdef INET6
	case PF_INET6:
		if (ti6) {
			/* th = &ti6->ti6_t; sado -- Greenhills compiler warning 550 - set but never used */
			td->td_ti6 = *ti6;
			td->td_ti6.ti6_plen = len;
		} else
			bzero(&td->td_ti6, sizeof(struct tcpipv6hdr));
		break;
#endif /* INET6 */
	case PF_INET:
		if (ti) {
			/* th = &ti->ti_t; sado -- Greenhills compiler warning 550 - set but never used */
			td->td_ti = *ti;
			td->td_ti.ti_len = (u_int16_t)len;  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
		} else
			bzero(&td->td_ti, sizeof(struct tcpiphdr));
		break;
	default:
		return;
	}

	td->td_req = (short)req; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
}
