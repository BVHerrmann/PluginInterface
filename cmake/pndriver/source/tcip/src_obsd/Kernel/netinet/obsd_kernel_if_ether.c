/*	$OpenBSD: if_ether.c,v 1.93 2011/09/18 11:17:58 miod Exp $	*/
/*	$NetBSD: if_ether.c,v 1.31 1996/05/11 12:59:58 mycroft Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988, 1993
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
 *	@(#)if_ether.c	8.1 (Berkeley) 6/10/93
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_act_module) */
#define LTRC_ACT_MODUL_ID 4019 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_act_module) */
/*
 * Ethernet address resolution protocol.
 * TODO:
 *	add "inuse/lock" bit (or ref. count) along with valid bit
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

#ifdef INET
#include "obsd_kernel_carp.h"

#include "obsd_kernel_bridge.h"

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_syslog.h>
#include <sys/obsd_kernel_proc.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_route.h>
#include <net/obsd_kernel_if_fddi.h>
#include <net/obsd_kernel_if_types.h>

#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_var.h>
#include <netinet/obsd_kernel_if_ether.h>
#if NCARP > 0
#include <netinet/obsd_kernel_ip_carp.h>
#endif

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

#define SIN(s) ((struct sockaddr_in *)s)
#define SDL(s) ((struct sockaddr_dl *)s)
#define SRP(s) ((struct sockaddr_inarp *)s)

/*
 * ARP trailer negotiation.  Trailer protocol is not IP specific,
 * but ARP request/response use IP addresses.
 */
#define ETHERTYPE_IPTRAILERS ETHERTYPE_TRAIL

/* timer values */
/* gh2289n: PN requires the ARP cache timeout to be 60 seconds, need to check timeouts in a shorter period */
/* OBSD_ITGR -- adjust ARP Cache Timeout and arptimer (RQ 1026760) */
int	arpt_prune = 60/2;      /* OBSD_ITGR -- the arptimer goes through the ARP cache twice as fast as the timeout occurs */
int	arpt_keep  = 60;        /* RQ 1028124: ARP ageout not configurable set to 60 seconds */
int	arpt_down = 20;		/* once declared down, don't send for 20 secs */
#define	rt_expire rt_rmx.rmx_expire

void arptfree(struct llinfo_arp *);
void arptimer(void *);
struct llinfo_arp *arplookup(u_int32_t, int, int, u_int);
void in_arpinput(struct mbuf *);

LIST_HEAD(, llinfo_arp) llinfo_arp;
static int	arp_inuse, arp_allocated;	/* mh2290: LINT 729   Symbol not explicitly initialized */
int	arp_maxtries = 5;
int	useloopback = 0;	/* OBSD_ITGR -- do NOT use loopback interface for local traffic */
int	arpinit_done;
int	la_hold_total;

/* revarp state */
struct in_addr myip, srv_ip;
int myip_initialized;
int revarp_in_progress;
struct ifnet *myip_ifp;

/*
 * Timeout routine.  Age arp_tab entries periodically.
 */
/* ARGSUSED */
void
arptimer(void *arg)
{
	struct timeout *to = (struct timeout *)arg;
	struct llinfo_arp *la, *nla;

	timeout_add_sec(to, arpt_prune);
	for (la = LIST_FIRST(&llinfo_arp); la != LIST_END(&llinfo_arp);
	    la = nla) {
		struct rtentry *rt = la->la_rt;

		nla = LIST_NEXT(la, la_list);
		if (rt->rt_expire && rt->rt_expire <= (u_int)time_second) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<' : signed/unsigned mismatch */
			arptfree(la); /* timer has expired; clear */
	}
}

/*
 * Parallel to llc_rtrequest.
 */
void
arp_rtrequest(int req, struct rtentry *rt, struct rt_addrinfo *info)
{
	struct sockaddr *gate = rt->rt_gateway;
	struct llinfo_arp *la = (struct llinfo_arp *)rt->rt_llinfo;
	static struct sockaddr_dl null_sdl = {sizeof(null_sdl), AF_LINK};
	struct in_ifaddr *ia;
	struct ifaddr *ifa;
	struct mbuf *m;

	OBSD_UNUSED_ARG(info); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	if (!arpinit_done) {
		static struct timeout arptimer_to;

		arpinit_done = 1;
		/*
		 * We generate expiration times from time.tv_sec
		 * so avoid accidently creating permanent routes.
		 */
		if (time_second == 0) {
			time_second++;
		}

		timeout_set(&arptimer_to, arptimer, &arptimer_to);
		timeout_add_sec(&arptimer_to, 1);
	}

	if (rt->rt_flags & RTF_GATEWAY) {
		if (req != RTM_ADD)
			return;

		/*
		 * linklayers with particular link MTU limitation.  it is a bit
		 * awkward to have FDDI handling here, we should split ARP from
		 * netinet/if_ether.c like NetBSD does.
		 */
		switch (rt->rt_ifp->if_type) {
		case IFT_FDDI:
			if (rt->rt_ifp->if_mtu > FDDIIPMTU)
				rt->rt_rmx.rmx_mtu = FDDIIPMTU;
			break;
		}

		return;
	}

	switch (req) {

	case RTM_ADD:
		/*
		 * XXX: If this is a manually added route to interface
		 * such as older version of routed or gated might provide,
		 * restore cloning bit.
		 */
		if ((rt->rt_flags & RTF_HOST) == 0 &&
		    SIN(rt_mask(rt))->sin_addr.s_addr != 0xffffffff)
			rt->rt_flags |= RTF_CLONING;
		if (rt->rt_flags & RTF_CLONING) {
			/*
			 * Case 1: This route should come from a route to iface.
			 */
			rt_setgate(rt, rt_key(rt),
			    (struct sockaddr *)&null_sdl,
			    rt->rt_ifp->if_rdomain);
			gate = rt->rt_gateway;
			SDL(gate)->sdl_type = rt->rt_ifp->if_type;
			SDL(gate)->sdl_index = rt->rt_ifp->if_index;
			/*
			 * Give this route an expiration time, even though
			 * it's a "permanent" route, so that routes cloned
			 * from it do not need their expiration time set.
			 */
			rt->rt_expire = time_second;
			/*
			 * linklayers with particular link MTU limitation.
			 */
			switch (rt->rt_ifp->if_type) {
			case IFT_FDDI:
				if ((rt->rt_rmx.rmx_locks & RTV_MTU) == 0 &&
				    (rt->rt_rmx.rmx_mtu > FDDIIPMTU ||
				     (rt->rt_rmx.rmx_mtu == 0 &&
				      rt->rt_ifp->if_mtu > FDDIIPMTU)))
					rt->rt_rmx.rmx_mtu = FDDIIPMTU;
				break;
			}
			break;
		}
		/* Announce a new entry if requested. */
#if 0
		if (rt->rt_flags & RTF_ANNOUNCE)
			arprequest(rt->rt_ifp,
			    &SIN(rt_key(rt))->sin_addr.s_addr,
			    &SIN(rt_key(rt))->sin_addr.s_addr,
			    (u_char *)LLADDR(SDL(gate)));
#else
		/* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/
		if (rt->rt_flags & RTF_ANNOUNCE)
		{
			u_int32_t key_addr = (SIN(rt_key(rt)))->sin_addr.s_addr;
			arprequest(rt->rt_ifp,
				&key_addr,
				&key_addr,
			    (u_char *)LLADDR(SDL(gate)));
		}

#endif
		/*FALLTHROUGH*/
	case RTM_RESOLVE:
		if (gate->sa_family != AF_LINK ||
		    gate->sa_len < sizeof(null_sdl)) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_001) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_NOTE, "arp_rtrequest: bad gateway value");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_001) */
			log(LOG_DEBUG, "arp_rtrequest: bad gateway value\n");
			break;
		}
		SDL(gate)->sdl_type = rt->rt_ifp->if_type;
		SDL(gate)->sdl_index = rt->rt_ifp->if_index;
		if (la != 0)
			break; /* This happens on a route change */
		/*
		 * Case 2:  This route may come from cloning, or a manual route
		 * add with a LL address.
		 */
		R_Malloc(la, struct llinfo_arp *, sizeof(*la));
		rt->rt_llinfo = (caddr_t)la;
		if (la == 0) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_002) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_NOTE, "arp_rtrequest: malloc failed");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_002) */
			log(LOG_DEBUG, "arp_rtrequest: malloc failed\n");
			break;
		}
		arp_inuse++, arp_allocated++;
		Bzero(la, sizeof(*la));
		la->la_rt = rt;
		rt->rt_flags |= RTF_LLINFO;
		LIST_INSERT_HEAD(&llinfo_arp, la, la_list);

		TAILQ_FOREACH(ia, &in_ifaddr, ia_list) {
			if (ia->ia_ifp == rt->rt_ifp &&
			    SIN(rt_key(rt))->sin_addr.s_addr ==
			    (IA_SIN(ia))->sin_addr.s_addr)
				break;
		}
		if (ia) {
			/*
			 * This test used to be
			 *	if (lo0ifp->if_flags & IFF_UP)
			 * It allowed local traffic to be forced through
			 * the hardware by configuring the loopback down.
			 * However, it causes problems during network
			 * configuration for boards that can't receive
			 * packets they send.  It is now necessary to clear
			 * "useloopback" and remove the route to force
			 * traffic out to the hardware.
			 *
			 * In 4.4BSD, the above "if" statement checked
			 * rt->rt_ifa against rt_key(rt).  It was changed
			 * to the current form so that we can provide a
			 * better support for multiple IPv4 addresses on a
			 * interface.
			 */
			rt->rt_expire = 0;
			Bcopy(((struct arpcom *)rt->rt_ifp)->ac_enaddr,
			    LLADDR(SDL(gate)),
			    SDL(gate)->sdl_alen = ETHER_ADDR_LEN);
			if (useloopback)
				rt->rt_ifp = lo0ifp;
			/*
			 * make sure to set rt->rt_ifa to the interface
			 * address we are using, otherwise we will have trouble
			 * with source address selection.
			 */
			ifa = &ia->ia_ifa;
			if (ifa != rt->rt_ifa) {
				IFAFREE(rt->rt_ifa);
				ifa->ifa_refcnt++;
				rt->rt_ifa = ifa;
			}
		}
		break;

	case RTM_DELETE:
		if (la == 0)
			break;
		arp_inuse--;
		LIST_REMOVE(la, la_list);
		rt->rt_llinfo = 0;
		rt->rt_flags &= ~RTF_LLINFO;
		while ((m = la->la_hold_head) != NULL) {
			la->la_hold_head = la->la_hold_head->m_nextpkt;
			la_hold_total--;
			m_freem(m);
		}
		Free((caddr_t)la);
	}
}

/*
 * Broadcast an ARP request. Caller specifies:
 *	- arp header source ip address
 *	- arp header target ip address
 *	- arp header source ethernet address
 */
void
arprequest(struct ifnet *ifp, u_int32_t *sip, u_int32_t *tip, u_int8_t *enaddr)
{
	struct mbuf *m;
	struct ether_header *eh;
	struct ether_arp *ea;
	struct sockaddr sa;

	if ((m = m_gethdr(M_DONTWAIT, MT_DATA)) == NULL)
		return;
	m->m_len = sizeof(*ea);
	m->m_pkthdr.len = sizeof(*ea);
	m->m_pkthdr.rdomain = ifp->if_rdomain;
	MH_ALIGN(m, sizeof(*ea));
	ea = mtod(m, struct ether_arp *);
	eh = (struct ether_header *)sa.sa_data;
	bzero((caddr_t)ea, sizeof (*ea));
	bcopy((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
	    sizeof(eh->ether_dhost));
	eh->ether_type = htons(ETHERTYPE_ARP);	/* if_output will not swap */
	ea->arp_hrd = htons(ARPHRD_ETHER);
	ea->arp_pro = htons(ETHERTYPE_IP);
	ea->arp_hln = sizeof(ea->arp_sha);	/* hardware address length */
	ea->arp_pln = sizeof(ea->arp_spa);	/* protocol address length */
	ea->arp_op = htons(ARPOP_REQUEST);
	bcopy((caddr_t)enaddr, (caddr_t)eh->ether_shost,
	      sizeof(eh->ether_shost));
	bcopy((caddr_t)enaddr, (caddr_t)ea->arp_sha, sizeof(ea->arp_sha));
	bcopy((caddr_t)sip, (caddr_t)ea->arp_spa, sizeof(ea->arp_spa));
	bcopy((caddr_t)tip, (caddr_t)ea->arp_tpa, sizeof(ea->arp_tpa));
	sa.sa_family = pseudo_AF_HDRCMPLT;
	sa.sa_len = sizeof(sa);
	m->m_flags |= M_BCAST;
	(*ifp->if_output)(ifp, m, &sa, (struct rtentry *)0);
}

/*
 * Resolve an IP address into an ethernet address.  If success,
 * desten is filled in.  If there is no entry in arptab,
 * set one up and broadcast a request for the IP address.
 * Hold onto this mbuf and resend it once the address
 * is finally resolved.  A return value of 1 indicates
 * that desten has been filled in and the packet should be sent
 * normally; a 0 return indicates that the packet has been
 * taken over here, either now or for later transmission.
 */
int
arpresolve(struct arpcom *ac, struct rtentry *rt, struct mbuf *m,
    struct sockaddr *dst, u_char *desten)
{
	struct llinfo_arp *la;
	struct sockaddr_dl *sdl;
	struct mbuf *mh;

	if (m->m_flags & M_BCAST) {	/* broadcast */
		bcopy((caddr_t)etherbroadcastaddr, (caddr_t)desten,
		    sizeof(etherbroadcastaddr));
		return (1);
	}
	if (m->m_flags & M_MCAST) {	/* multicast */
		ETHER_MAP_IP_MULTICAST(&SIN(dst)->sin_addr, desten);
		return (1);
	}
	if (rt) {
		la = (struct llinfo_arp *)rt->rt_llinfo;
		if (la == NULL)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_003) */
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE, "arpresolve %u.%u.%u.%u: route without link local address",
			                   (unsigned)(SIN(dst)->sin_addr.s_addr&0xff),(unsigned)((SIN(dst)->sin_addr.s_addr>>8)&0xff),(unsigned)((SIN(dst)->sin_addr.s_addr>>16)&0xff),(unsigned)(SIN(dst)->sin_addr.s_addr>>24));
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_003) */
			log(LOG_DEBUG, "arpresolve: %s: route without link "
			    "local address\n", inet_ntoa(SIN(dst)->sin_addr));
		}
	} else {
		if ((la = arplookup(SIN(dst)->sin_addr.s_addr, RT_REPORT, 0,
		    ac->ac_if.if_rdomain)) != NULL)
			rt = la->la_rt;
		else
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_004) */
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE, "arpresolve %u.%u.%u.%u: can't allocate llinfo",
				               (unsigned)(SIN(dst)->sin_addr.s_addr&0xff),(unsigned)((SIN(dst)->sin_addr.s_addr>>8)&0xff),(unsigned)((SIN(dst)->sin_addr.s_addr>>16)&0xff),(unsigned)(SIN(dst)->sin_addr.s_addr>>24));
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_004) */
			log(LOG_DEBUG,
			    "arpresolve: %s: can't allocate llinfo\n",
			    inet_ntoa(SIN(dst)->sin_addr));
		}
	}
	if (la == 0 || rt == 0) {
		m_freem(m);
		return (0);
	}
	sdl = SDL(rt->rt_gateway);
	/*
	 * Check the address family and length is valid, the address
	 * is resolved; otherwise, try to resolve.
	 */
	if ((rt->rt_expire == 0 || rt->rt_expire > (u_int)time_second) &&  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<' : signed/unsigned mismatch */
	    sdl->sdl_family == AF_LINK && sdl->sdl_alen != 0) {
		bcopy(LLADDR(sdl), desten, sdl->sdl_alen);
		return 1;
	}
	if (((struct ifnet *)ac)->if_flags & IFF_NOARP) {
		m_freem(m);
		return 0;
	}

	/*
	 * There is an arptab entry, but no ethernet address
	 * response yet. Insert mbuf in hold queue if below limit
	 * if above the limit free the queue without queuing the new packet.
	 */
	if (la_hold_total < MAX_HOLD_TOTAL && la_hold_total < nmbclust / 64) {
		if (la->la_hold_count >= MAX_HOLD_QUEUE) {
			mh = la->la_hold_head;
			la->la_hold_head = la->la_hold_head->m_nextpkt;
			if (mh == la->la_hold_tail)
				la->la_hold_tail = NULL;
			la->la_hold_count--;
			la_hold_total--;
			/*
			 * If an arp request is pending, all incoming packets are added to the hold queue.
			 * Packets are simply dropped, if the hold queue is full. The following trace
			 * indicates that a packet has been dumped.
			 * Please note that the specified IP address in the trace can be either destination 
			 * address or the default gateway (for destinations in other subnets).
			 */
			TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_WARN, "arpresolve: hold queue for ip 0x%X reached limit (%u), drop packet",
				ntohl(SIN(dst)->sin_addr.s_addr),
				MAX_HOLD_QUEUE);
			m_freem(mh);
		}
		if (la->la_hold_tail == NULL)
			la->la_hold_head = m;
		else
			la->la_hold_tail->m_nextpkt = m;
		la->la_hold_tail = m;
		la->la_hold_count++;
		la_hold_total++;
	} else {
		/*
		* If an arp request is pending, all incoming packets are added to the hold queue.
		* In case the limit for all hold queues is reached, all packets stored in the current 
		* queue are dropped. The following trace indicates that loss.
		* Please note that the specified IP address in the trace can be either destination
		* address or the default gateway (for destinations in other subnets).
		*/
		TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_WARN, "arpresolve: maximum for all hold queues (%u) reached; drop all items (%u) in hold queue for ip 0x%X",
			la_hold_total,
			la->la_hold_count + 1,
			ntohl(SIN(dst)->sin_addr.s_addr));
		while ((mh = la->la_hold_head) != NULL) {
			la->la_hold_head =
			    la->la_hold_head->m_nextpkt;
			la_hold_total--;
			m_freem(mh);
		}
		la->la_hold_tail = NULL;
		la->la_hold_count = 0;
		m_freem(m);
	}

	/*
	 * Re-send the ARP request when appropriate.
	 */
	if (rt->rt_expire) {
		rt->rt_flags &= ~RTF_REJECT;
		if (la->la_asked == 0 || rt->rt_expire != (u_int)time_second) { /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4389: '!=' : signed/unsigned mismatch */
			rt->rt_expire = time_second;
#if 0
			if (la->la_asked++ < arp_maxtries)
				arprequest(&ac->ac_if,
				    &(SIN(rt->rt_ifa->ifa_addr)->sin_addr.s_addr),
				    &(SIN(dst)->sin_addr.s_addr),
#else 		/* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/
			if (la->la_asked++ < arp_maxtries)
			{
				u_int32_t rt_addr = (SIN(rt->rt_ifa->ifa_addr))->sin_addr.s_addr;
				u_int32_t dst_addr = (SIN(dst))->sin_addr.s_addr;
				arprequest(&ac->ac_if,
				    &rt_addr,
				    &dst_addr,
#endif
#if NCARP > 0
				    (rt->rt_ifp->if_type == IFT_CARP) ?
					((struct arpcom *) rt->rt_ifp->if_softc
					)->ac_enaddr :
#endif
				    ac->ac_enaddr);
#if 1 		/* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/
			}
#endif
			else {
				rt->rt_flags |= RTF_REJECT;
				rt->rt_expire += arpt_down;
				la->la_asked = 0;
				while ((mh = la->la_hold_head) != NULL) {
					la->la_hold_head =
					    la->la_hold_head->m_nextpkt;
					la_hold_total--;
					m_freem(mh);
				}
				la->la_hold_tail = NULL;
				la->la_hold_count = 0;
			}
		}
	}
	return (0);
}

/*
 * Common length and type checks are done here,
 * then the protocol-specific routine is called.
 */
void
arpintr(struct mbuf *m)
{
	struct arphdr *ar;
	unsigned int len; /* gerlach/gh2289n: changed type from int to unsigned int to avoid compiler warning C4018 */

	if (m == NULL)
		return;

	len = sizeof(struct arphdr);
	if (m->m_len < len && (m = m_pullup(m, len)) == NULL)
		return;

	ar = mtod(m, struct arphdr *);
	if (ntohs(ar->ar_hrd) != ARPHRD_ETHER) {
		m_freem(m);
		return;
	}

	len += 2 * (ar->ar_hln + ar->ar_pln);
	if (m->m_len < len && (m = m_pullup(m, len)) == NULL)
		return;

	switch (ntohs(ar->ar_pro)) {
	case ETHERTYPE_IP:
	case ETHERTYPE_IPTRAILERS:
		in_arpinput(m);
		return;
	}
	m_freem(m);
}

/*
 * ARP for Internet protocols on Ethernet.
 * Algorithm is that given in RFC 826.
 * In addition, a sanity check is performed on the sender
 * protocol address, to catch impersonators.
 * We no longer handle negotiations for use of trailer protocol:
 * Formerly, ARP replied for protocol type ETHERTYPE_TRAIL sent
 * along with IP replies if we wanted trailers sent to us,
 * and also sent them in response to IP replies.
 * This allowed either end to announce the desire to receive
 * trailer packets.
 * We no longer reply to requests for ETHERTYPE_TRAIL protocol either,
 * but formerly didn't normally send requests.
 */
void
in_arpinput(struct mbuf *m)
{
	struct ether_arp *ea;
	struct arpcom *ac = (struct arpcom *)m->m_pkthdr.rcvif;
	struct ether_header *eh;
	struct llinfo_arp *la = 0;
	struct rtentry *rt;
	struct in_ifaddr *ia;
	struct sockaddr_dl *sdl;
	struct sockaddr sa;
	struct in_addr isaddr, itaddr, myaddr;
	struct mbuf *mh, *mt;
	u_int8_t *enaddr = NULL;
#if NCARP > 0
	u_int8_t *ether_shost = NULL;
#endif
	int op;

	ea = mtod(m, struct ether_arp *);
	op = ntohs(ea->arp_op);
	if ((op != ARPOP_REQUEST) && (op != ARPOP_REPLY))
		goto out;

	bcopy((caddr_t)ea->arp_tpa, (caddr_t)&itaddr, sizeof(itaddr));
	bcopy((caddr_t)ea->arp_spa, (caddr_t)&isaddr, sizeof(isaddr));

	/* First try: check target against our addresses */
	TAILQ_FOREACH(ia, &in_ifaddr, ia_list) {
		if (itaddr.s_addr != ia->ia_addr.sin_addr.s_addr)
			continue;

#if NCARP > 0
		if (ia->ia_ifp->if_type == IFT_CARP &&
		    ((ia->ia_ifp->if_flags & (IFF_UP|IFF_RUNNING)) ==
		    (IFF_UP|IFF_RUNNING))) {

			if (ia->ia_ifp == m->m_pkthdr.rcvif) {
				if (op == ARPOP_REPLY)
					break;
				if (carp_iamatch(ia, ea->arp_sha,
				    &enaddr, &ether_shost))
					break;
				else
					goto out;
			}
		} else
#endif
			if (ia->ia_ifp == m->m_pkthdr.rcvif)
				break;
	}

	/* Second try: check source against our addresses */
	if (ia == NULL) {
		TAILQ_FOREACH(ia, &in_ifaddr, ia_list) {
			if (isaddr.s_addr != ia->ia_addr.sin_addr.s_addr)
				continue;
			if (ia->ia_ifp == m->m_pkthdr.rcvif)
				break;
		}
	}

	/* Third try: not one of our addresses, just find an usable ia */
	if (ia == NULL) {
		struct ifaddr *ifa;

		TAILQ_FOREACH(ifa, &m->m_pkthdr.rcvif->if_addrlist, ifa_list) {
			if (ifa->ifa_addr->sa_family == AF_INET)
				break;
		}
		if (ifa)
			ia = (struct in_ifaddr *)ifa;
	}

	if (ia == NULL)
		goto out;

	if (!enaddr)
		enaddr = ac->ac_enaddr;
	myaddr = ia->ia_addr.sin_addr;

	if (!bcmp((caddr_t)ea->arp_sha, enaddr, sizeof (ea->arp_sha)))
		goto out;	/* it's from me, ignore it. */
	if (ETHER_IS_MULTICAST(&ea->arp_sha[0]))
		if (!bcmp((caddr_t)ea->arp_sha, (caddr_t)etherbroadcastaddr,
		    sizeof (ea->arp_sha))) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_005) */
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_ERROR, "arp: ether address is broadcast for IP address %u.%u.%u.%u!",
				              (unsigned)(isaddr.s_addr&0xff),(unsigned)((isaddr.s_addr>>8)&0xff),(unsigned)((isaddr.s_addr>>16)&0xff),(unsigned)(isaddr.s_addr>>24));
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_005) */
			log(LOG_ERR, "arp: ether address is broadcast for "
			    "IP address %s!\n", inet_ntoa(isaddr));
			goto out;
		}
	if (myaddr.s_addr && isaddr.s_addr == myaddr.s_addr) {

#if NCARP > 0
		/* send Indication that duplicate Cluster IP was detected */
		if (ia->ia_ifp->if_type == IFT_CARP &&
			(ia->ia_ifp->if_flags & (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING))
		{
			struct ifnet *carp_if = ia->ia_ifp;
			int primary = 0;

			obsd_netif_get_carp_role(carp_if->if_xname, &primary);
			if (primary != 0)
			{
				struct ifnet *standard_if = carp_if->if_carpdev;
				struct in_ifaddr *standard_addr = NULL;

				TAILQ_FOREACH(standard_addr, &in_ifaddr, ia_list) {
					if (standard_addr->ia_ifp == standard_if) break;
				}

				if (standard_addr != NULL) TCIP_SRV_CLUSTER_IP_ADDR_CONFLICT_DETECTED(standard_addr->ia_addr.sin_addr.s_addr);
			}
		}
#endif
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_006) */
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_ERROR, "duplicate IP address %u.%u.%u.%u",
			(unsigned)(isaddr.s_addr & 0xff), (unsigned)((isaddr.s_addr >> 8) & 0xff), (unsigned)((isaddr.s_addr >> 16) & 0xff), (unsigned)(isaddr.s_addr >> 24));
		TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_ERROR, "duplicate IP address sent by %02x:%02x:%02x:%02x:%02x:%02x",
			ea->arp_sha[0], ea->arp_sha[1], ea->arp_sha[2], ea->arp_sha[3], ea->arp_sha[4], ea->arp_sha[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_006) */
		log(LOG_ERR,
		   "duplicate IP address %s sent from ethernet address %s\n",
		   inet_ntoa(isaddr), ether_sprintf(ea->arp_sha));
		itaddr = myaddr;
		goto reply;
	}
	la = arplookup(isaddr.s_addr, itaddr.s_addr == myaddr.s_addr, 0,
	    rtable_l2(m->m_pkthdr.rdomain));
	if (la && ((rt = la->la_rt) != NULL) && ((sdl = SDL(rt->rt_gateway)) != NULL)) {      /* gh2289n: added NULL comparison to avoid PN compiler error */
		if (sdl->sdl_alen) {
		    if (bcmp(ea->arp_sha, LLADDR(sdl), sdl->sdl_alen)) {
			if (rt->rt_flags & RTF_PERMANENT_ARP) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_007) */
				TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to overwrite permanent entry for %u.%u.%u.%u on interface %u",
					(unsigned)(isaddr.s_addr&0xff),(unsigned)((isaddr.s_addr>>8)&0xff),(unsigned)((isaddr.s_addr>>16)&0xff),(unsigned)(isaddr.s_addr>>24), ac->ac_if.if_index);
				TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to overwrite permanent entry by %02x:%02x:%02x:%02x:%02x:%02x",
					               ea->arp_sha[0], ea->arp_sha[1], ea->arp_sha[2], ea->arp_sha[3], ea->arp_sha[4], ea->arp_sha[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_007) */
				log(LOG_WARNING,
				   "arp: attempt to overwrite permanent "
				   "entry for %s by %s on %s\n",
				   inet_ntoa(isaddr),
				   ether_sprintf(ea->arp_sha),
				   ac->ac_if.if_xname);
				goto out;
			} else if (rt->rt_ifp != &ac->ac_if) {
				if (ac->ac_if.if_type != IFT_CARP)
				{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_008) */
					TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to overwrite entry for %u.%u.%u.%u on interface %u from interface %u",
								      (unsigned)(isaddr.s_addr&0xff),(unsigned)((isaddr.s_addr>>8)&0xff),(unsigned)((isaddr.s_addr>>16)&0xff),(unsigned)(isaddr.s_addr>>24), rt->rt_ifp->if_index, ac->ac_if.if_index);
					TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to overwrite entry by %02x:%02x:%02x:%02x:%02x:%02x",
						               ea->arp_sha[0], ea->arp_sha[1], ea->arp_sha[2], ea->arp_sha[3], ea->arp_sha[4], ea->arp_sha[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_008) */
					log(LOG_WARNING,
					   "arp: attempt to overwrite entry for"
					   " %s on %s by %s on %s\n",
					   inet_ntoa(isaddr),
					   rt->rt_ifp->if_xname,
					   ether_sprintf(ea->arp_sha),
					   ac->ac_if.if_xname);
				}
				goto out;
			} else {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_009) */
				TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "arp info overwritten for %u.%u.%u.%u on interface %u",
				                  (unsigned)(isaddr.s_addr&0xff),(unsigned)((isaddr.s_addr>>8)&0xff),(unsigned)((isaddr.s_addr>>16)&0xff),(unsigned)(isaddr.s_addr>>24), ac->ac_if.if_index);
				TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp info overwritten by %02x:%02x:%02x:%02x:%02x:%02x",
					               ea->arp_sha[0], ea->arp_sha[1], ea->arp_sha[2], ea->arp_sha[3], ea->arp_sha[4], ea->arp_sha[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_009) */
				log(LOG_INFO,
				   "arp info overwritten for %s by %s on %s\n",
				   inet_ntoa(isaddr),
				   ether_sprintf(ea->arp_sha),
				   ac->ac_if.if_xname);
				rt->rt_expire = 1; /* no longer static */
			}
		    }
		} else if (rt->rt_ifp != &ac->ac_if && !(ac->ac_if.if_bridge &&
		    (rt->rt_ifp->if_bridge == ac->ac_if.if_bridge)) &&
		    !(rt->rt_ifp->if_type == IFT_CARP &&
		    rt->rt_ifp->if_carpdev == &ac->ac_if) &&
		    !(ac->ac_if.if_type == IFT_CARP &&
		    ac->ac_if.if_carpdev == rt->rt_ifp)) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_010) */
		    TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to add entry for %u.%u.%u.%u on interface %u from interface %u",
			                   (unsigned)(isaddr.s_addr&0xff),(unsigned)((isaddr.s_addr>>8)&0xff),(unsigned)((isaddr.s_addr>>16)&0xff),(unsigned)(isaddr.s_addr>>24), rt->rt_ifp->if_index, ac->ac_if.if_index);
		    TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_WARN, "arp: attempt to add entry by %02x:%02x:%02x:%02x:%02x:%02x",
				               ea->arp_sha[0], ea->arp_sha[1], ea->arp_sha[2], ea->arp_sha[3], ea->arp_sha[4], ea->arp_sha[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_010) */
		    log(LOG_WARNING,
			"arp: attempt to add entry for %s "
			"on %s by %s on %s\n",
			inet_ntoa(isaddr), rt->rt_ifp->if_xname,
			ether_sprintf(ea->arp_sha),
			ac->ac_if.if_xname);
#if ( DISABLE_ARP_CACHE_POISONING_PROTECTION == 0 )
		    goto out;
#endif /* DISABLE_ARP_CACHE_POISONING_PROTECTION */
		}
		bcopy(ea->arp_sha, LLADDR(sdl),
		    sdl->sdl_alen = sizeof(ea->arp_sha));
		if (rt->rt_expire)
			rt->rt_expire = time_second + arpt_keep;
		rt->rt_flags &= ~RTF_REJECT;
		la->la_asked = 0;
		rt_sendmsg(rt, RTM_RESOLVE, rt->rt_ifp->if_rdomain);
		while ((mh = la->la_hold_head) != NULL) {
			if ((la->la_hold_head = mh->m_nextpkt) == NULL)
				la->la_hold_tail = NULL;
			la->la_hold_count--;
			la_hold_total--;
			mt = la->la_hold_tail;

			(*ac->ac_if.if_output)(&ac->ac_if, mh, rt_key(rt), rt);

			if (la->la_hold_tail == mh) {
				/* mbuf is back in queue. Discard. */
				la->la_hold_tail = mt;
				if (la->la_hold_tail)
					la->la_hold_tail->m_nextpkt = NULL;
				else
					la->la_hold_head = NULL;
				la->la_hold_count--;
				la_hold_total--;
				m_freem(mh);
			}
		}
	}
reply:
	if (op != ARPOP_REQUEST) {
out:
		m_freem(m);
		return;
	}
	if (itaddr.s_addr == myaddr.s_addr) {
		/* I am the target */
		bcopy(ea->arp_sha, ea->arp_tha, sizeof(ea->arp_sha));
		bcopy(enaddr, ea->arp_sha, sizeof(ea->arp_sha));
	} else {
		la = arplookup(itaddr.s_addr, 0, SIN_PROXY,
		    rtable_l2(m->m_pkthdr.rdomain));
		if (la == 0)
			goto out;
		rt = la->la_rt;
		if (rt->rt_ifp->if_type == IFT_CARP &&
		    m->m_pkthdr.rcvif->if_type != IFT_CARP)
			goto out;
		bcopy(ea->arp_sha, ea->arp_tha, sizeof(ea->arp_sha));
		sdl = SDL(rt->rt_gateway);
		bcopy(LLADDR(sdl), ea->arp_sha, sizeof(ea->arp_sha));
	}

	bcopy(ea->arp_spa, ea->arp_tpa, sizeof(ea->arp_spa));
	bcopy(&itaddr, ea->arp_spa, sizeof(ea->arp_spa));
	ea->arp_op = htons(ARPOP_REPLY);
	ea->arp_pro = htons(ETHERTYPE_IP); /* let's be sure! */
	eh = (struct ether_header *)sa.sa_data;
	bcopy(ea->arp_tha, eh->ether_dhost, sizeof(eh->ether_dhost));
#if NCARP > 0
	if (ether_shost)
		enaddr = ether_shost;
#endif
	bcopy(enaddr, eh->ether_shost, sizeof(eh->ether_shost));

	eh->ether_type = htons(ETHERTYPE_ARP);
	sa.sa_family = pseudo_AF_HDRCMPLT;
	sa.sa_len = sizeof(sa);
	(*ac->ac_if.if_output)(&ac->ac_if, m, &sa, (struct rtentry *)0);
	return;
}

/*
 * Free an arp entry.
 */
void
arptfree(struct llinfo_arp *la)
{
	struct rtentry *rt = la->la_rt;
	struct sockaddr_dl *sdl;
	struct rt_addrinfo info;
	u_int tid = 0;
	int error = 0;

	if (rt == 0)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ether_011) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL,"arptfree");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ether_011) */
		panic("arptfree");
	}
	if (rt->rt_refcnt > 0 && ((sdl = SDL(rt->rt_gateway)) != NULL) &&      /* gh2289n: added NULL comparison to avoid PN compiler error */
	    sdl->sdl_family == AF_LINK) {
		sdl->sdl_alen = 0;
		la->la_asked = 0;
		rt->rt_flags &= ~RTF_REJECT;
		return;
	}
	bzero(&info, sizeof(info));
	info.rti_info[RTAX_DST] = rt_key(rt);
	info.rti_info[RTAX_NETMASK] = rt_mask(rt);
	info.rti_info[RTAX_GATEWAY] = rt->rt_gateway; 
	info.rti_flags = rt->rt_flags;

	if (rt->rt_ifp)
		tid = rt->rt_ifp->if_rdomain;

	error = rtrequest1(RTM_DELETE, &info, rt->rt_priority, &rt, tid);   /* gh2289n: need &rt to avoid that rtrequest1 frees the route */
	rt_missmsg(RTM_DELETE, &info, info.rti_flags, rt->rt_ifp, error, tid); /* gh2289n: assume the the roure referenced by info isn't freed */

	/* Adjust the refcount */
	if ((error == 0) && (rt->rt_refcnt <= 0)) 
	{   /* gh2289n: route rt is not longer needed, so we can free it */
		rt->rt_refcnt++;
		rtfree(rt);
	}
}

/*
 * Lookup or enter a new address in arptab.
 */
struct llinfo_arp *
arplookup(u_int32_t addr, int create, int proxy, u_int tableid)
{
	struct rtentry *rt;
	static struct sockaddr_inarp sin;

	sin.sin_len = sizeof(sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = addr;
	sin.sin_other = proxy ? SIN_PROXY : 0;
	rt = rtalloc1(sintosa(&sin), create, tableid);
	if (rt == 0)
		return (0);
	rt->rt_refcnt--;
	if ((rt->rt_flags & RTF_GATEWAY) || (rt->rt_flags & RTF_LLINFO) == 0 ||
	    rt->rt_gateway->sa_family != AF_LINK) {
		if (create) {
			if (rt->rt_refcnt <= 0 &&
			    (rt->rt_flags & RTF_CLONED) != 0) {
				struct rt_addrinfo info;

				bzero(&info, sizeof(info));
				info.rti_info[RTAX_DST] = rt_key(rt);
				info.rti_info[RTAX_GATEWAY] = rt->rt_gateway;
				info.rti_info[RTAX_NETMASK] = rt_mask(rt);

				rtrequest1(RTM_DELETE, &info, rt->rt_priority,
				    NULL, tableid);
			}
		}
		return (0);
	}
	return ((struct llinfo_arp *)rt->rt_llinfo);
}

void
arp_ifinit(struct arpcom *ac, struct ifaddr *ifa)
{

	/* Warn the user if another station has this IP address. */
#if 0 /* RQ 1401128 -- no gratitious ARPs */
	arprequest(&ac->ac_if,
	    &(IA_SIN(ifa)->sin_addr.s_addr),
	    &(IA_SIN(ifa)->sin_addr.s_addr),
	    ac->ac_enaddr);
#else
	OBSD_UNUSED_ARG(ac);
#endif
	ifa->ifa_rtrequest = arp_rtrequest;
	ifa->ifa_flags |= RTF_CLONING;
}

/*
 * Called from Ethernet interrupt handlers
 * when ether packet type ETHERTYPE_REVARP
 * is received.  Common length and type checks are done here,
 * then the protocol-specific routine is called.
 */
void
revarpinput(struct mbuf *m)
{
	struct arphdr *ar;

	if (m->m_len < sizeof(struct arphdr))
		goto out;
	ar = mtod(m, struct arphdr *);
	if (ntohs(ar->ar_hrd) != ARPHRD_ETHER)
		goto out;
	if (m->m_len < sizeof(struct arphdr) + 2 * (ar->ar_hln + ar->ar_pln))
		goto out;
	switch (ntohs(ar->ar_pro)) {

	case ETHERTYPE_IP:
	case ETHERTYPE_IPTRAILERS:
		in_revarpinput(m);
		return;

	default:
		break;
	}
out:
	m_freem(m);
}

/*
 * RARP for Internet protocols on Ethernet.
 * Algorithm is that given in RFC 903.
 * We are only using for bootstrap purposes to get an ip address for one of
 * our interfaces.  Thus we support no user-interface.
 *
 * Since the contents of the RARP reply are specific to the interface that
 * sent the request, this code must ensure that they are properly associated.
 *
 * Note: also supports ARP via RARP packets, per the RFC.
 */
void
in_revarpinput(struct mbuf *m)
{
	struct ifnet *ifp;
	struct ether_arp *ar;
	int op;

	ar = mtod(m, struct ether_arp *);
	op = ntohs(ar->arp_op);
	switch (op) {
	case ARPOP_REQUEST:
	case ARPOP_REPLY:	/* per RFC */
		in_arpinput(m);
		return;
	case ARPOP_REVREPLY:
		break;
	case ARPOP_REVREQUEST:	/* handled by rarpd(8) */
	default:
		goto out;
	}
	if (!revarp_in_progress)
		goto out;
	ifp = m->m_pkthdr.rcvif;
	if (ifp != myip_ifp) /* !same interface */
		goto out;
	if (myip_initialized)
		goto wake;
	if (bcmp(ar->arp_tha, ((struct arpcom *)ifp)->ac_enaddr,
	    sizeof(ar->arp_tha)))
		goto out;
	bcopy((caddr_t)ar->arp_spa, (caddr_t)&srv_ip, sizeof(srv_ip));
	bcopy((caddr_t)ar->arp_tpa, (caddr_t)&myip, sizeof(myip));
	myip_initialized = 1;
wake:	/* Do wakeup every time in case it was missed. */
	wakeup((caddr_t)&myip);

out:
	m_freem(m);
}

/*
 * Send a RARP request for the ip address of the specified interface.
 * The request should be RFC 903-compliant.
 */
void
revarprequest(struct ifnet *ifp)
{
	struct sockaddr sa;
	struct mbuf *m;
	struct ether_header *eh;
	struct ether_arp *ea;
	struct arpcom *ac = (struct arpcom *)ifp;

	if ((m = m_gethdr(M_DONTWAIT, MT_DATA)) == NULL)
		return;
	m->m_len = sizeof(*ea);
	m->m_pkthdr.len = sizeof(*ea);
	MH_ALIGN(m, sizeof(*ea));
	ea = mtod(m, struct ether_arp *);
	eh = (struct ether_header *)sa.sa_data;
	bzero((caddr_t)ea, sizeof(*ea));
	bcopy((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
	    sizeof(eh->ether_dhost));
	eh->ether_type = htons(ETHERTYPE_REVARP);
	ea->arp_hrd = htons(ARPHRD_ETHER);
	ea->arp_pro = htons(ETHERTYPE_IP);
	ea->arp_hln = sizeof(ea->arp_sha);	/* hardware address length */
	ea->arp_pln = sizeof(ea->arp_spa);	/* protocol address length */
	ea->arp_op = htons(ARPOP_REVREQUEST);
	bcopy((caddr_t)ac->ac_enaddr, (caddr_t)eh->ether_shost,
	   sizeof(ea->arp_tha));
	bcopy((caddr_t)ac->ac_enaddr, (caddr_t)ea->arp_sha,
	   sizeof(ea->arp_sha));
	bcopy((caddr_t)ac->ac_enaddr, (caddr_t)ea->arp_tha,
	   sizeof(ea->arp_tha));
	sa.sa_family = pseudo_AF_HDRCMPLT;
	sa.sa_len = sizeof(sa);
	m->m_flags |= M_BCAST;
	ifp->if_output(ifp, m, &sa, (struct rtentry *)0);
}

/*
 * RARP for the ip address of the specified interface, but also
 * save the ip address of the server that sent the answer.
 * Timeout if no response is received.
 */
int
revarpwhoarewe(struct ifnet *ifp, struct in_addr *serv_in,
    struct in_addr *clnt_in)
{
	int result, count = 20;

	if (myip_initialized)
		return EIO;

	myip_ifp = ifp;
	revarp_in_progress = 1;
	while (count--) {
		revarprequest(ifp);
		result = tsleep((caddr_t)&myip, PSOCK, "revarp", hz/2);
		if (result != EWOULDBLOCK)
			break;
	}
	revarp_in_progress = 0;
	if (!myip_initialized)
		return ENETUNREACH;

	bcopy((caddr_t)&srv_ip, serv_in, sizeof(*serv_in));
	bcopy((caddr_t)&myip, clnt_in, sizeof(*clnt_in));
	return 0;
}

/* For compatibility: only saves interface address. */
int
revarpwhoami(struct in_addr *in, struct ifnet *ifp)
{
	struct in_addr server;
	return (revarpwhoarewe(ifp, &server, in));
}


#endif /* INET */
