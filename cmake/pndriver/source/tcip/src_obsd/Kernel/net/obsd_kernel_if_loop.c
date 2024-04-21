/*	$OpenBSD: if_loop.c,v 1.46 2011/07/09 00:47:18 henning Exp $	*/
/*	$NetBSD: if_loop.c,v 1.15 1996/05/07 02:40:33 thorpej Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
 *	@(#)if_loop.c	8.1 (Berkeley) 6/10/93
 */

/*
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

/*
 * Loopback interface driver for protocol testing and timing.
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_loop_act_module) */
#define LTRC_ACT_MODUL_ID 4014 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_loop_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include "obsd_kernel_bpfilter.h"

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_errno.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_time.h>

#include <machine/obsd_kernel_cpu.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_types.h>
#include <net/obsd_kernel_netisr.h>
#include <net/obsd_kernel_route.h>

#ifdef	INET
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_in_var.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_ip_var.h>
#endif

#ifdef INET6
#ifndef INET
#include <netinet/obsd_kernel_in.h>
#endif
#include <netinet6/obsd_kernel_in6_var.h>
#include <netinet/obsd_kernel_ip6.h>
#endif

#ifdef MPLS
#include <netmpls/obsd_kernel_mpls.h>
#endif

#if NBPFILTER > 0
#include <net/obsd_kernel_bpf.h>
#endif

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */


#if       OBSD_IF_USE_LOOPBACK      /* gh2289n: loopback device specific code */
#if defined(LARGE_LOMTU)
#define LOMTU	(131072 +  MHLEN + MLEN)
#else
#define	LOMTU	(32768 +  MHLEN + MLEN)
#endif
  
#ifdef ALTQ
static void lo_altqstart(struct ifnet *);
#endif

int	loop_clone_create(struct if_clone *, int);
int	loop_clone_destroy(struct ifnet *);

struct if_clone loop_cloner =
    IF_CLONE_INITIALIZER("lo", loop_clone_create, loop_clone_destroy);

/* ARGSUSED */
void loopattach(int n)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	OBSD_UNUSED_ARG(n);  /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	/* gh2289n: assume loop_clone_create() will fail with panic() if underlying if_alloc_sadl() will not get it's      */
	/*          memory. So no further error handling here.                                                             */
	(void) loop_clone_create(&loop_cloner, 0);
	if_clone_attach(&loop_cloner);
}

void loopdetach(int n)
{
	OBSD_UNUSED_ARG(n);  /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* loop_clone_destroy(lo0ifp); can't use it because loop_clone_destroy blocks lo0ifp */

	/* only lo0 supported so far */
	if_detach(lo0ifp);
	free(lo0ifp, M_DEVBUF);
	lo0ifp = NULL;
}

int loop_clone_create(struct if_clone *ifc, int unit)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	struct ifnet *ifp;

	ifp = malloc(sizeof(*ifp), M_DEVBUF, M_NOWAIT|M_ZERO);
	if (ifp == NULL)
		return (ENOMEM);

	snprintf(ifp->if_xname, sizeof ifp->if_xname, "lo%d", unit);
	ifp->if_softc = NULL;
	ifp->if_mtu = LOMTU;
	ifp->if_flags = IFF_LOOPBACK | IFF_MULTICAST;
	ifp->if_ioctl = loioctl;
	ifp->if_output = looutput;
	ifp->if_type = IFT_LOOP;
	ifp->if_hdrlen = sizeof(u_int32_t);
	ifp->if_addrlen = 0;
	IFQ_SET_READY(&ifp->if_snd);
#ifdef ALTQ
	ifp->if_start = lo_altqstart;
#endif
	if (unit == 0) {
		lo0ifp = ifp;
		if_attachhead(ifp);
		if_addgroup(lo0ifp, ifc->ifc_name);
	} else
		if_attach(ifp);
	if_alloc_sadl(ifp);
	if (ifp->if_sadl == NULL)
	{   /* gh2289n: hmm, went wrong, so we have to roll back all changes and return an error              */
		/* gh2289n: if we don't get memory for our link level address we could do the following things    */
		/*          (detaching the interface, ...). But doing this could lead us to a crash in            */
		/*          rt_gettable() called by if_detach() -> rt_if_remove() -> rt_gettable() in OBSD's 5.1  */
		/*          kernel code. This is because the routing table will ininialized later after attaching */
		/*          the loopback device(s). We could think about detaching all instances other than the   */
		/*          first one assuming the will be created by management (ifconfig, ...) AFTER            */
		/*          initialiting the routing table. This would be possible / works for the typical        */
		/*          where the first instance lo0 is created via pdevinit and all the other one's later.   */
		/*          But how to be sure nobody changes the number of devices created via pdevinit from 1   */
		/*          > 1 ? So panic in every case here, assuming we will alway's have enough mememory      */
		/*          during system startup and assuming the lox-devices are created during system startup  */
		/*          too. If so we would never see the panic                                               */
		/*          Maybe the situation will be better in newer OBSD versions ...                         */
#if       0
		if (unit == 0)
		{   /* this instance is the first one */
			if_delgroup(lo0ifp, ifc->ifc_name);
			if_detach(ifp);
			lo0ifp = NULL;
		}
		else
			if_detach(ifp);
		free(ifp, M_DEVBUF);
		return (ENOMEM);
#endif /* 0 */
		panic("loop_clone_create got no if_sadl for the interface (no more memory)");
	}

#if NBPFILTER > 0
	bpfattach(&ifp->if_bpf, ifp, DLT_LOOP, sizeof(u_int32_t));
#endif
	return (0);
}

int loop_clone_destroy(struct ifnet *ifp)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	if (ifp == lo0ifp)
		return (EPERM);

	if_detach(ifp);

	free(ifp, M_DEVBUF);
	return (0);
}
#endif /* OBSD_IF_USE_LOOPBACK */

int looutput(struct ifnet *ifp, struct mbuf *m, struct sockaddr *dst, struct rtentry *rt)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	int isr;

	if ((m->m_flags & M_PKTHDR) == 0)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_loop_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "looutput: no header mbuf");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_loop_001) */
		panic("looutput: no header mbuf");
	}
#if NBPFILTER > 0
	/*
	 * only send packets to bpf if they are real loopback packets;
	 * looutput() is also called for SIMPLEX interfaces to duplicate
	 * packets for local use. But don't dup them to bpf.
	 */
	if (ifp->if_bpf && (ifp->if_flags & IFF_LOOPBACK))
		bpf_mtap_af(ifp->if_bpf, htonl(dst->sa_family), m,
		    BPF_DIRECTION_OUT);
#endif
	m->m_pkthdr.rcvif = ifp;

	if (rt && rt->rt_flags & (RTF_REJECT|RTF_BLACKHOLE)) {
		m_freem(m);
		return (rt->rt_flags & RTF_BLACKHOLE ? 0 :
			rt->rt_flags & RTF_HOST ? EHOSTUNREACH : ENETUNREACH);
	}

	ifp->if_opackets++;
	ifp->if_obytes += m->m_pkthdr.len;
#ifdef ALTQ
	/*
	 * altq for loop is just for debugging.
	 * only used when called for loop interface (not for
	 * a simplex interface).
	 */
	if ((ALTQ_IS_ENABLED(&ifp->if_snd) || TBR_IS_ENABLED(&ifp->if_snd))
	    && ifp->if_start == lo_altqstart) {
		int32_t *afp;
	        int error;

		M_PREPEND(m, sizeof(int32_t), M_DONTWAIT);
		if (m == 0)
			return (ENOBUFS);
		afp = mtod(m, int32_t *);
		*afp = (int32_t)dst->sa_family;

		IFQ_ENQUEUE(&ifp->if_snd, m, NULL, error);
		(*ifp->if_start)(ifp);
		return (error);
	}
#endif /* ALTQ */
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		isr = NETISR_IP;
		break;
#endif
#ifdef INET6
	case AF_INET6:
		isr = NETISR_IPV6;
		break;
#endif /* INET6 */
#ifdef MPLS
	case AF_MPLS:
		isr = NETISR_MPLS;
		break;
#endif /* MPLS */
	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_loop_002) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR,"interface with index %u: can't handle af%d", ifp->if_index, dst->sa_family);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_loop_002) */
		printf("%s: can't handle af%d\n", ifp->if_xname,
			dst->sa_family);
		m_freem(m);
		return (EAFNOSUPPORT);
	}
	if (isr == NETISR_IP) ipintr(m);
	ifp->if_ipackets++;
	ifp->if_ibytes += m->m_pkthdr.len;
	return (0);
}

#if       OBSD_IF_USE_LOOPBACK      /* gh2289n: loopback device specific code */
#ifdef ALTQ
static void
lo_altqstart(ifp)
	struct ifnet *ifp;
{
	struct ifqueue *ifq;
	struct mbuf *m;
	int32_t af, *afp;
	int s, isr;
	
	while (1) {
		s = splnet();
		IFQ_DEQUEUE(&ifp->if_snd, m);
		splx(s);
		if (m == NULL)
			return;

		afp = mtod(m, int32_t *);
		af = *afp;
		m_adj(m, sizeof(int32_t));

		switch (af) {
#ifdef INET
		case AF_INET:
			ifq = &ipintrq;
			isr = NETISR_IP;
			break;
#endif
#ifdef INET6
		case AF_INET6:
			m->m_flags |= M_LOOP;
			ifq = &ip6intrq;
			isr = NETISR_IPV6;
			break;
#endif
#ifdef MPLS
		case AF_MPLS:
			ifq = &mplsintrq;
			isr = NETISR_MPLS;
			break;
#endif
		default:
			printf("lo_altqstart: can't handle af%d\n", af);
			m_freem(m);
			return;
		}

		s = splnet();
		if (IF_QFULL(ifq)) {
			IF_DROP(ifq);
			m_freem(m);
			splx(s);
			return;
		}
		IF_ENQUEUE(ifq, m);
		schednetisr(isr);
		ifp->if_ipackets++;
		ifp->if_ibytes += m->m_pkthdr.len;
		splx(s);
	}
}
#endif /* ALTQ */

/* ARGSUSED */
void lortrequest(int cmd, struct rtentry *rt, struct rt_addrinfo *info)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	OBSD_UNUSED_ARG(info); 
	OBSD_UNUSED_ARG(cmd); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	if (rt)
		rt->rt_rmx.rmx_mtu = LOMTU;
}

/*
 * Process an ioctl request.
 */
/* ARGSUSED */
int loioctl(struct ifnet *ifp, u_long cmd, caddr_t data)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	struct ifaddr *ifa;
	struct ifreq *ifr;
	int s, error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		s = splnet();
		ifp->if_flags |= IFF_RUNNING;
		if_up(ifp);		/* send up RTM_IFINFO */
		splx(s);

		ifa = (struct ifaddr *)data;
		if (ifa != 0)
			ifa->ifa_rtrequest = lortrequest;
		/*
		 * Everything else is done at a higher level.
		 */
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		ifr = (struct ifreq *)data;
		if (ifr == 0) {
			error = EAFNOSUPPORT;		/* XXX */
			break;
		}
		switch (ifr->ifr_addr.sa_family) {

#ifdef INET
		case AF_INET:
			break;
#endif
#ifdef INET6
		case AF_INET6:
			break;
#endif /* INET6 */

		default:
			error = EAFNOSUPPORT;
			break;
		}
		break;

	case SIOCSIFMTU:
		ifr = (struct ifreq *)data;
		ifp->if_mtu = ifr->ifr_mtu;
		break;

	default:
		error = ENOTTY;
	}
	return (error);
}
#endif /* OBSD_IF_USE_LOOPBACK */

