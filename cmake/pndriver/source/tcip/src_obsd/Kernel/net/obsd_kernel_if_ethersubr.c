/*	$OpenBSD: if_ethersubr.c,v 1.151 2011/07/09 00:47:18 henning Exp $	*/
/*	$NetBSD: if_ethersubr.c,v 1.19 1996/05/07 02:40:30 thorpej Exp $	*/

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
 * Copyright (c) 1982, 1989, 1993
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
 *	@(#)if_ethersubr.c	8.1 (Berkeley) 6/10/93
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ethersubr_act_module) */
#define LTRC_ACT_MODUL_ID 4013 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ethersubr_act_module) */
/*
%%% portions-copyright-nrl-95
Portions of this software are Copyright 1995-1998 by Randall Atkinson,
Ronald Lee, Daniel McDonald, Bao Phan, and Chris Winters. All Rights
Reserved. All rights under this copyright have been assigned to the US
Naval Research Laboratory (NRL). The NRL Copyright Notice and License
Agreement Version 1.1 (January 17, 1995) applies to these portions of the
software.
You should have received a copy of the license with this software. If you
didn't get a copy, you may request one from <license@ipv6.nrl.navy.mil>.
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

#include "obsd_kernel_bpfilter.h"

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_errno.h>
#include <sys/obsd_kernel_syslog.h>
#include <sys/obsd_kernel_timeout.h>

#include <machine/obsd_kernel_cpu.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_netisr.h>
#include <net/obsd_kernel_route.h>
#include <net/obsd_kernel_if_llc.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_if_media.h>
#include <net/obsd_kernel_if_types.h>

#include <netinet/obsd_kernel_in.h>
#ifdef INET
#include <netinet/obsd_kernel_in_var.h>
#endif
#include <netinet/obsd_kernel_if_ether.h>
#include <netinet/obsd_kernel_ip_ipsp.h>
#include <netinet/obsd_kernel_ip_var.h>

#include <dev/obsd_kernel_rndvar.h>

#define OBSD_PN_VLAN_SKIP 1

#if NBPFILTER > 0
#include <net/obsd_kernel_bpf.h>
#endif

#include "obsd_kernel_bridge.h"
#if NBRIDGE > 0
#include <net/obsd_kernel_if_bridge.h>
#endif

#include "obsd_kernel_vlan.h"
#if NVLAN > 0
#include <net/obsd_kernel_if_vlan_var.h>
#endif /* NVLAN > 0 */

#if         OBSD_PN_VLAN_SKIP > 0
#include <net/obsd_kernel_if_vlan_var.h>
#endif   /* OBSD_PN_VLAN_SKIP > 0 */

#include "obsd_kernel_carp.h"
#if NCARP > 0
#include <netinet/obsd_kernel_ip_carp.h>
#endif

#include "obsd_kernel_pppoe.h"
#if NPPPOE > 0
#include <net/obsd_kernel_if_pppoe.h>
#endif

#include "obsd_kernel_trunk.h"
#if NTRUNK > 0
#include <net/obsd_kernel_if_trunk.h>
#endif

#ifdef AOE
#include <net/obsd_kernel_if_aoe.h>
#endif /* AOE */

#ifdef INET6
#ifndef INET
#include <netinet/obsd_kernel_in.h>
#endif
#include <netinet6/obsd_kernel_in6_var.h>
#include <netinet6/obsd_kernel_nd6.h>
#endif

#ifdef PIPEX
#include <net/obsd_kernel_pipex.h>
#endif

#ifdef MPLS
#include <netmpls/obsd_kernel_mpls.h>
#endif /* MPLS */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */
u_char etherbroadcastaddr[ETHER_ADDR_LEN] =
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#define senderr(e) { error = (e); goto bad;}

int	 netisr = 0;

int
ether_ioctl(struct ifnet *ifp, struct arpcom *arp, u_long cmd, caddr_t data)
{
	struct ifreq *ifr = (struct ifreq *)data;
	int error = 0;

	switch (cmd) {
	case SIOCSIFADDR:
		break;

	case SIOCSIFMTU:
		if (ifr->ifr_mtu < ETHERMIN || (u_int32_t)ifr->ifr_mtu > ifp->if_hardmtu) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
			error = EINVAL;
		else
			ifp->if_mtu = ifr->ifr_mtu;
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifp->if_flags & IFF_MULTICAST) {
			error = (cmd == SIOCADDMULTI) ?
			    ether_addmulti(ifr, arp) :
			    ether_delmulti(ifr, arp);
		} else
			error = ENOTTY;
		break;

	default:
		error = ENOTTY;
	}

	return (error);
}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Assumes that ifp is actually pointer to arpcom structure.
 */
#if OBSD_PNIO_CFG_TRACE_ON
	void controller_add_obsd_send_measure_5();
#endif /* OBSD_PNIO_CFG_TRACE_ON */

int ether_output(struct ifnet *ifp0, struct mbuf *m0, struct sockaddr *dst, struct rtentry *rt0)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	u_int16_t etype;
	int len, error = 0, hdrcmplt = 0;
	u_char edst[ETHER_ADDR_LEN], esrc[ETHER_ADDR_LEN];
	struct mbuf *m = m0;
	struct rtentry *rt;
	struct mbuf *mcopy = (struct mbuf *)0;
	struct ether_header *eh;
	struct arpcom *ac = (struct arpcom *)ifp0;
	short mflags;
	struct ifnet *ifp = ifp0;

#if NTRUNK > 0
	if (ifp->if_type == IFT_IEEE8023ADLAG)
		senderr(EBUSY);
#endif

#if NCARP > 0
	if (ifp->if_type == IFT_CARP) {
		struct ifaddr *ifa;

		/* loop back if this is going to the carp interface */
		if (dst != NULL && LINK_STATE_IS_UP(ifp0->if_link_state) &&
		    (ifa = ifa_ifwithaddr(dst, ifp->if_rdomain)) != NULL &&
		    ifa->ifa_ifp == ifp0)
			return (looutput(ifp0, m, dst, rt0));

		ifp = ifp->if_carpdev;
		ac = (struct arpcom *)ifp;

		if ((ifp0->if_flags & (IFF_UP|IFF_RUNNING)) !=
		    (IFF_UP|IFF_RUNNING))
			senderr(ENETDOWN);
	}
#endif /* NCARP > 0 */

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
		senderr(ENETDOWN);
	if ((rt = rt0) != NULL) {
		if ((rt->rt_flags & RTF_UP) == 0) {
			if ((rt0 = rt = rtalloc1(dst, RT_REPORT,
			    m->m_pkthdr.rdomain)) != NULL)
				rt->rt_refcnt--;
			else
				senderr(EHOSTUNREACH);
		}

		if (rt->rt_flags & RTF_GATEWAY) {
			if (rt->rt_gwroute == 0)
				goto lookup;
			if (((rt = rt->rt_gwroute)->rt_flags & RTF_UP) == 0) {
				rtfree(rt);
				rt = rt0;
			lookup:
				rt->rt_gwroute = rtalloc1(rt->rt_gateway,
				    RT_REPORT, ifp->if_rdomain);
				if ((rt = rt->rt_gwroute) == NULL)
					senderr(EHOSTUNREACH);
			}
		}
		if (rt->rt_flags & RTF_REJECT)
			if (rt->rt_rmx.rmx_expire == 0 ||
			    (u_int)time_second < rt->rt_rmx.rmx_expire) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<' : signed/unsigned mismatch */
				senderr(rt == rt0 ? EHOSTDOWN : EHOSTUNREACH);
	}
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		if (!arpresolve(ac, rt, m, dst, edst))
			return (0);	/* if not yet resolved */
		/* If broadcasting on a simplex interface, loopback a copy */
		if ((m->m_flags & M_BCAST) && (ifp->if_flags & IFF_SIMPLEX) &&
		    !m->m_pkthdr.pf.routed)
			mcopy = m_copy(m, 0, (int)M_COPYALL);
		etype = htons(ETHERTYPE_IP);
		break;
#endif
#ifdef INET6
	case AF_INET6:
		if (!nd6_storelladdr(ifp, rt, m, dst, (u_char *)edst))
			return (0); /* it must be impossible, but... */
		etype = htons(ETHERTYPE_IPV6);
		break;
#endif
#ifdef MPLS
       case AF_MPLS:
		if (rt)
			dst = rt_key(rt);
		else
			senderr(EHOSTUNREACH);

		if (!ISSET(ifp->if_xflags, IFXF_MPLS))
			senderr(ENETUNREACH);

		switch (dst->sa_family) {
			case AF_LINK:
				if (((struct sockaddr_dl *)dst)->sdl_alen <
				    sizeof(edst))
					senderr(EHOSTUNREACH);
				bcopy(LLADDR(((struct sockaddr_dl *)dst)), edst,
				    sizeof(edst));
				break;
			case AF_INET:
				if (!arpresolve(ac, rt, m, dst, edst))
					return (0); /* if not yet resolved */
				break;
			default:
				senderr(EHOSTUNREACH);
		}
		/* XXX handling for simplex devices in case of M/BCAST ?? */
		if (m->m_flags & (M_BCAST | M_MCAST))
			etype = htons(ETHERTYPE_MPLS_MCAST);
		else
			etype = htons(ETHERTYPE_MPLS);
		break;
#endif /* MPLS */
	case pseudo_AF_HDRCMPLT:
		hdrcmplt = 1;
		eh = (struct ether_header *)dst->sa_data;
		bcopy((caddr_t)eh->ether_shost, (caddr_t)esrc, sizeof(esrc));
		/* FALLTHROUGH */

	case AF_UNSPEC:
		eh = (struct ether_header *)dst->sa_data;
		bcopy((caddr_t)eh->ether_dhost, (caddr_t)edst, sizeof(edst));
		/* AF_UNSPEC doesn't swap the byte order of the ether_type. */
		etype = eh->ether_type;
		break;

	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ethersubr_001) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR,"interface with index %u: can't handle af%d", ifp->if_index, dst->sa_family);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ethersubr_001) */
		printf("%s: can't handle af%d\n", ifp->if_xname,
			dst->sa_family);
		senderr(EAFNOSUPPORT);
	}

	/* XXX Should we feed-back an unencrypted IPsec packet ? */
	if (mcopy)
		(void) looutput(ifp, mcopy, dst, rt);

	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	M_PREPEND(m, ETHER_HDR_LEN, M_DONTWAIT);
	if (m == 0)
		senderr(ENOBUFS);
	eh = mtod(m, struct ether_header *);
#if 0 /* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment */
	bcopy((caddr_t)&etype,(caddr_t)&eh->ether_type,
		sizeof(eh->ether_type));
#else
		eh->ether_type = etype;
#endif
	bcopy((caddr_t)edst, (caddr_t)eh->ether_dhost, sizeof(edst));
	if (hdrcmplt)
		bcopy((caddr_t)esrc, (caddr_t)eh->ether_shost,
		    sizeof(eh->ether_shost));
	else
		bcopy((caddr_t)ac->ac_enaddr, (caddr_t)eh->ether_shost,
		    sizeof(eh->ether_shost));

#if NCARP > 0
	if (ifp0 != ifp && ifp0->if_type == IFT_CARP)
	    carp_rewrite_lladdr(ifp0, eh->ether_shost);
#endif

#if NBRIDGE > 0
	/*
	 * Interfaces that are bridge members need special handling
	 * for output.
	 */
	if (ifp->if_bridge) {
		struct m_tag *mtag;

		/*
		 * Check if this packet has already been sent out through
		 * this bridge, in which case we simply send it out
		 * without further bridge processing.
		 */
		for (mtag = m_tag_find(m, PACKET_TAG_BRIDGE, NULL); mtag;
		    mtag = m_tag_find(m, PACKET_TAG_BRIDGE, mtag)) {
			if (!bcmp(&ifp->if_bridge, mtag + 1, sizeof(caddr_t)))
				break;
		}
		if (mtag == NULL) {
			/* Attach a tag so we can detect loops */
			mtag = m_tag_get(PACKET_TAG_BRIDGE, sizeof(caddr_t),
			    M_NOWAIT);
			if (mtag == NULL) {
				error = ENOBUFS;
				goto bad;
			}
			bcopy(&ifp->if_bridge, mtag + 1, sizeof(caddr_t));
			m_tag_prepend(m, mtag);
			error = bridge_output(ifp, m, NULL, NULL);
			return (error);
		}
	}
#endif
	mflags = m->m_flags;
	len = m->m_pkthdr.len;
	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	IFQ_ENQUEUE(&ifp->if_snd, m, NULL, error);
	if (error) {
		/* mbuf is already freed */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_WARN, "ether_output: could not enqueue packet, ifp 0x%x, error: %u", (uint32_t)ifp, error);
		return (error);
	}
	if (IF_QWATERMARK(&ifp->if_snd)) {	/* hm Queue with watermark check */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_NOTE, "ether_output: enqueue packet: watermark reached, ifp 0x%x, len %i", (uint32_t)ifp, IF_LEN(&ifp->if_snd));
	}
	ifp->if_obytes += len;
#if NCARP > 0
	if (ifp != ifp0)
		ifp0->if_obytes += len;
#endif /* NCARP > 0 */
	if (mflags & M_MCAST)
		ifp->if_omcasts++;

#if OBSD_PNIO_CFG_TRACE_ON
	controller_add_obsd_send_measure_5();
#endif /* OBSD_PNIO_CFG_TRACE_ON */

	/* Task 1560385 (use if_sendq) -- try to send immediately */
	ifp->if_start(ifp);

	return (error);

bad:
	if (m)
		m_freem(m);
	return (error);
}

/*
 * Process a received Ethernet packet;
 * the packet is in the mbuf chain m without
 * the ether header, which is provided separately.
 */
void ether_input(struct ifnet *ifp0, struct ether_header *eh, struct mbuf *m) /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	u_int16_t etype;
	int llcfound = 0;
	struct llc *l;
	struct arpcom *ac;
	struct ifnet *ifp = ifp0;
#if NTRUNK > 0
	int i = 0;
#endif
#if NPPPOE > 0
	struct ether_header *eh_tmp;
#endif


	m_cluncount(m, 1);

	/* mark incoming routing domain */
	m->m_pkthdr.rdomain = ifp->if_rdomain;

	if (eh == NULL) {
		eh = mtod(m, struct ether_header *);
		m_adj(m, ETHER_HDR_LEN);
	}

#if NTRUNK > 0
	/* Handle input from a trunk port */
	while (ifp->if_type == IFT_IEEE8023ADLAG) {
		if (++i > TRUNK_MAX_STACKING) {
			m_freem(m);
			return;
		}
		if (trunk_input(ifp, eh, m) != 0)
			return;

		/* Has been set to the trunk interface */
		ifp = m->m_pkthdr.rcvif;
	}
#endif

	if ((ifp->if_flags & IFF_UP) == 0) {
		m_freem(m);
		return;
	}
	if (ETHER_IS_MULTICAST(eh->ether_dhost)) {
		if ((ifp->if_flags & IFF_SIMPLEX) == 0) {
			struct ifaddr *ifa;
			struct sockaddr_dl *sdl = NULL;

			TAILQ_FOREACH(ifa, &ifp->if_addrlist, ifa_list) {
				if (((sdl =
				    (struct sockaddr_dl *)ifa->ifa_addr) != NULL )&&   /* gh2289n: added NULL comparison to avoid PN compiler error */
				    sdl->sdl_family == AF_LINK)
					break;
			}
			/*
			 * If this is not a simplex interface, drop the packet
			 * if it came from us.
			 */
			if (sdl && bcmp(LLADDR(sdl), eh->ether_shost,
			    ETHER_ADDR_LEN) == 0) {
				m_freem(m);
				return;
			}
		}

		if (bcmp((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
		    sizeof(etherbroadcastaddr)) == 0)
			m->m_flags |= M_BCAST;
		else
			m->m_flags |= M_MCAST;
		ifp->if_imcasts++;
#if NTRUNK > 0
		if (ifp != ifp0)
			ifp0->if_imcasts++;
#endif
	}

	ifp->if_ibytes += m->m_pkthdr.len + sizeof(*eh);
#if NTRUNK > 0
	if (ifp != ifp0)
		ifp0->if_ibytes += m->m_pkthdr.len + sizeof(*eh);
#endif

	etype = ntohs(eh->ether_type);

	if (!(netisr & (1 << NETISR_RND_DONE))) {
		add_net_randomness(etype);
		atomic_setbits_int((u_int32_t *)(&netisr), (1 << NETISR_RND_DONE)); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	}

#if NVLAN > 0
	if (((m->m_flags & M_VLANTAG) || etype == ETHERTYPE_VLAN ||
	    etype == ETHERTYPE_QINQ) && (vlan_input(eh, m) == 0))
		return;
#endif

#if         OBSD_PN_VLAN_SKIP > 0   /* gerlach/gh2289n: add VLAN Skipping like requested by PN integration */
                                    /*                  currently we have no general vlan interface and no */
                                    /*                  special PN vlan interface class to, so do all the  */
                                    /*                  necessary things right here                        */
	if (etype == ETHERTYPE_VLAN)    
	{
		if (m->m_len < EVL_ENCAPLEN && (m = m_pullup(m, EVL_ENCAPLEN)) == NULL)
		{
			ifp->if_ierrors++;
			return ;
		}
		m->m_pkthdr.ether_vtag = ntohs(*mtod(m, u_int16_t *));  /* save it for some trouble shooting usage */
		eh->ether_type = mtod(m, u_int16_t *)[1];
		m->m_len -= EVL_ENCAPLEN;
		m->m_data += EVL_ENCAPLEN;
		m->m_pkthdr.len -= EVL_ENCAPLEN;
		ether_input(ifp, eh, m); /* recurse */
		return ;
	}
#endif   /* OBSD_PN_VLAN_SKIP > 0 */

#if NBRIDGE > 0
	/*
	 * Tap the packet off here for a bridge, if configured and
	 * active for this interface.  bridge_input returns
	 * NULL if it has consumed the packet, otherwise, it
	 * gets processed as normal.
	 */
	if (ifp->if_bridge) {
		if (m->m_flags & M_PROTO1)
			m->m_flags &= ~M_PROTO1;
		else {
			m = bridge_input(ifp, eh, m);
			if (m == NULL)
				return;
			/* The bridge has determined it's for us. */
			ifp = m->m_pkthdr.rcvif;
		}
	}
#endif

#if NVLAN > 0
	if ((m->m_flags & M_VLANTAG) || etype == ETHERTYPE_VLAN ||
	    etype == ETHERTYPE_QINQ) {
		/* The bridge did not want the vlan frame either, drop it. */
		ifp->if_noproto++;
		m_freem(m);
		return;
	}
#endif /* NVLAN > 0 */

#if NCARP > 0
	if (ifp->if_carp) {
		if (ifp->if_type != IFT_CARP &&
		    (carp_input(m, (u_int8_t *)&eh->ether_shost,
		    (u_int8_t *)&eh->ether_dhost, eh->ether_type) == 0))
			return;
		/* clear mcast if received on a carp IP balanced address */
		else if (ifp->if_type == IFT_CARP &&
		    m->m_flags & (M_BCAST|M_MCAST) &&
		    carp_our_mcastaddr(ifp, (u_int8_t *)&eh->ether_dhost))
			m->m_flags &= ~(M_BCAST|M_MCAST);
	}
#endif /* NCARP > 0 */

	ac = (struct arpcom *)ifp;

	/*
	 * If packet has been filtered by the bpf listener, drop it now
	 */
	if (m->m_flags & M_FILDROP) {
		m_freem(m);
		return;
	}

	/*
	 * If packet is unicast and we're in promiscuous mode, make sure it
	 * is for us.  Drop otherwise.
	 */
	if ((m->m_flags & (M_BCAST|M_MCAST)) == 0 &&
	    ((ifp->if_flags & IFF_PROMISC) || (ifp0->if_flags & IFF_PROMISC))) {
		if (bcmp(ac->ac_enaddr, (caddr_t)eh->ether_dhost,
		    ETHER_ADDR_LEN)) {
			m_freem(m);
			return;
		}
	}

	/*
	 * Schedule softnet interrupt and enqueue packet within the same spl.
	 */
decapsulate:

	switch (etype) {
#ifdef INET
	case ETHERTYPE_IP:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ethersubr_002) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "ether_input: ETHERTYPE_IP");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ethersubr_002) */
		ipintr(m);
		goto done;

	case ETHERTYPE_ARP:
		if (ifp->if_flags & IFF_NOARP)
			goto dropanyway;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ethersubr_003) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "ether_input: ETHERTYPE_ARP");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ethersubr_003) */
		arpintr(m);
		goto done;

	case ETHERTYPE_REVARP:
		if (ifp->if_flags & IFF_NOARP)
			goto dropanyway;
		revarpinput(m);	/* XXX queue? */
		goto done;

#endif
#ifdef INET6
	/*
	 * Schedule IPv6 software interrupt for incoming IPv6 packet.
	 */
	case ETHERTYPE_IPV6:
		schednetisr(NETISR_IPV6);
		inq = &ip6intrq;
		break;
#endif /* INET6 */
#if NPPPOE > 0 || defined(PIPEX)
	case ETHERTYPE_PPPOEDISC:
	case ETHERTYPE_PPPOE:
#ifndef PPPOE_SERVER
		if (m->m_flags & (M_MCAST | M_BCAST)) {
			m_freem(m);
			goto done;
		}
#endif
		M_PREPEND(m, sizeof(*eh), M_DONTWAIT);
		if (m == NULL)
			goto done;

		eh_tmp = mtod(m, struct ether_header *);
		bcopy(eh, eh_tmp, sizeof(struct ether_header));
#ifdef PIPEX
		if (pipex_enable) {
			struct pipex_session *session;

			if ((session = pipex_pppoe_lookup_session(m)) != NULL) {
				pipex_pppoe_input(m, session);
				goto done;
			}
		}
#endif
		if (etype == ETHERTYPE_PPPOEDISC)
			inq = &pppoediscinq;
		else
			inq = &pppoeinq;

		schednetisr(NETISR_PPPOE);
		break;
#endif
#ifdef AOE
	case ETHERTYPE_AOE:
		aoe_input(ifp, m);
		goto done;
#endif /* AOE */
#ifdef MPLS
	case ETHERTYPE_MPLS:
	case ETHERTYPE_MPLS_MCAST:
		inq = &mplsintrq;
		schednetisr(NETISR_MPLS);
		break;
#endif
	default:
		if (llcfound || etype > ETHERMTU)
			goto dropanyway;
		llcfound = 1;
		l = mtod(m, struct llc *);
		switch (l->llc_dsap) {
		case LLC_SNAP_LSAP:
			if (l->llc_control == LLC_UI &&
			    l->llc_dsap == LLC_SNAP_LSAP &&
			    l->llc_ssap == LLC_SNAP_LSAP) {
				/* SNAP */
				if (m->m_pkthdr.len > etype)
					m_adj(m, etype - m->m_pkthdr.len);
				m_adj(m, 6);
				M_PREPEND(m, sizeof(*eh), M_DONTWAIT);
				if (m == 0)
					goto done;
				*mtod(m, struct ether_header *) = *eh;
				goto decapsulate;
			}
			goto dropanyway;
		dropanyway:
		default:
			m_freem(m);
			goto done;
		}
	}
done:
	return;
}

/*
 * Convert Ethernet address to printable (loggable) representation.
 */
static char digits[] = "0123456789abcdef";

char * ether_sprintf(u_char *ap) /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator */
{
	int i;
	static char etherbuf[ETHER_ADDR_LEN * 3];
	char *cp = etherbuf;

	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		*cp++ = digits[*ap >> 4];
		*cp++ = digits[*ap++ & 0xf];
		*cp++ = ':';
	}
	*--cp = 0;
	return (etherbuf);
}

/*
 * Generate a (hopefully) acceptable MAC address, if asked.
 */
void
ether_fakeaddr(struct ifnet *ifp)
{
	static int unit;
	int rng;

	/* Non-multicast; locally administered address */
	((struct arpcom *)ifp)->ac_enaddr[0] = 0xfe;
	((struct arpcom *)ifp)->ac_enaddr[1] = 0xe1;
	((struct arpcom *)ifp)->ac_enaddr[2] = 0xba;
	((struct arpcom *)ifp)->ac_enaddr[3] = (u_int8_t)(0xd0 | (unit++ & 0xf)); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	rng = cold ? random() ^ (long)ifp : arc4random();
	((struct arpcom *)ifp)->ac_enaddr[4] = (u_int8_t) (rng);      /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	((struct arpcom *)ifp)->ac_enaddr[5] = (u_int8_t) (rng >> 8); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
}

/*
 * Perform common duties while attaching to interface list
 */

/* gh2289n: if if_alloc_sadl() will fail (e.g. because of no memory available) ifp->if_sadl is NULL */
/*          we return with ifp->if_type == 0                                                        */
void ether_ifattach(struct ifnet *ifp)  /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator) */
{
	/*
	 * Any interface which provides a MAC address which is obviously
	 * invalid gets whacked, so that users will notice.
	 */
	if (ETHER_IS_MULTICAST(((struct arpcom *)ifp)->ac_enaddr))
		ether_fakeaddr(ifp);

	ifp->if_type = IFT_ETHER;
	ifp->if_addrlen = ETHER_ADDR_LEN;
	ifp->if_hdrlen = ETHER_HDR_LEN;
	ifp->if_mtu = ETHERMTU;
	ifp->if_output = ether_output;

	if (ifp->if_hardmtu == 0)
		ifp->if_hardmtu = ETHERMTU;

	if_alloc_sadl(ifp);

#if       OBSD_POOL_MALLOC_NO_SLEEP != 0    /* gerlach/gh2289n:  check if if_alloc_sadl could do its work */
	if (ifp->if_sadl == NULL)
	{
		ifp->if_type = 0;                   /* reset the if-type to 0 to signal the error                 */
	}
	else
		bcopy((caddr_t)((struct arpcom *)ifp)->ac_enaddr, LLADDR(ifp->if_sadl), ifp->if_addrlen);
#else  /* OBSD_POOL_MALLOC_NO_SLEEP */
	bcopy((caddr_t)((struct arpcom *)ifp)->ac_enaddr,
	    LLADDR(ifp->if_sadl), ifp->if_addrlen);
#endif /* OBSD_POOL_MALLOC_NO_SLEEP */
	LIST_INIT(&((struct arpcom *)ifp)->ac_multiaddrs);
#if NBPFILTER > 0
	bpfattach(&ifp->if_bpf, ifp, DLT_EN10MB, ETHER_HDR_LEN);
#endif
}

void ether_ifdetach(struct ifnet *ifp) /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator */
{
	struct arpcom *ac = (struct arpcom *)ifp;
	struct ether_multi *enm;

	for (enm = LIST_FIRST(&ac->ac_multiaddrs);
	    enm != LIST_END(&ac->ac_multiaddrs);
	    enm = LIST_FIRST(&ac->ac_multiaddrs)) {
		LIST_REMOVE(enm, enm_list);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_if_ethersubr_004) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "ether_ifdetach: removed address stored in 0x%x from enm_list", (uint32_t)enm);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_if_ethersubr_004) */
		free(enm, M_IFMADDR);
	}

#if 0
	/* moved to if_detach() */
	if_free_sadl(ifp);
#endif
}

#if 0
/*
 * This is for reference.  We have table-driven versions of the
 * crc32 generators, which are faster than the double-loop.
 */
u_int32_t __pure
ether_crc32_le_update(u_int_32_t crc, const u_int8_t *buf, size_t len)
{
	u_int32_t c, carry;
	size_t i, j;

	for (i = 0; i < len; i++) {
		c = buf[i];
		for (j = 0; j < 8; j++) {
			carry = ((crc & 0x01) ? 1 : 0) ^ (c & 0x01);
			crc >>= 1;
			c >>= 1;
			if (carry)
				crc = (crc ^ ETHER_CRC_POLY_LE);
		}
	}

	return (crc);
}

u_int32_t __pure
ether_crc32_be_update(u_int_32_t crc, const u_int8_t *buf, size_t len)
{
	u_int32_t c, carry;
	size_t i, j;

	for (i = 0; i < len; i++) {
		c = buf[i];
		for (j = 0; j < 8; j++) {
			carry = ((crc & 0x80000000U) ? 1 : 0) ^ (c & 0x01);
			crc <<= 1;
			c >>= 1;
			if (carry)
				crc = (crc ^ ETHER_CRC_POLY_BE) | carry;
		}
	}

	return (crc);
}
#else
u_int32_t __pure
ether_crc32_le_update(u_int32_t crc, const u_int8_t *buf, size_t len)
{
	static const u_int32_t crctab[] = {
		0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
		0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
		0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
	};
	size_t i;

	for (i = 0; i < len; i++) {
		crc ^= buf[i];
		crc = (crc >> 4) ^ crctab[crc & 0xf];
		crc = (crc >> 4) ^ crctab[crc & 0xf];
	}

	return (crc);
}

u_int32_t __pure
ether_crc32_be_update(u_int32_t crc, const u_int8_t *buf, size_t len)
{
	static const u_int8_t rev[] = {
		0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
		0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
	};
	static const u_int32_t crctab[] = {
		0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
		0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
		0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
		0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd
	};
	size_t i;
	u_int8_t data;

	for (i = 0; i < len; i++) {
		data = buf[i];
		crc = (crc << 4) ^ crctab[(crc >> 28) ^ rev[data & 0xf]];
		crc = (crc << 4) ^ crctab[(crc >> 28) ^ rev[data >> 4]];
	}

	return (crc);
}
#endif

u_int32_t
ether_crc32_le(const u_int8_t *buf, size_t len)
{
	return ether_crc32_le_update(0xffffffff, buf, len);
}

u_int32_t
ether_crc32_be(const u_int8_t *buf, size_t len)
{
	return ether_crc32_be_update(0xffffffff, buf, len);
}

#ifdef INET
u_char	ether_ipmulticast_min[ETHER_ADDR_LEN] =
    { 0x01, 0x00, 0x5e, 0x00, 0x00, 0x00 };
u_char	ether_ipmulticast_max[ETHER_ADDR_LEN] =
    { 0x01, 0x00, 0x5e, 0x7f, 0xff, 0xff };
#endif

#ifdef INET6
u_char	ether_ip6multicast_min[ETHER_ADDR_LEN] =
    { 0x33, 0x33, 0x00, 0x00, 0x00, 0x00 };
u_char	ether_ip6multicast_max[ETHER_ADDR_LEN] =
    { 0x33, 0x33, 0xff, 0xff, 0xff, 0xff };
#endif

/*
 * Convert a sockaddr into an Ethernet address or range of Ethernet
 * addresses.
 */
int
ether_multiaddr(struct sockaddr *sa, u_int8_t addrlo[ETHER_ADDR_LEN],
    u_int8_t addrhi[ETHER_ADDR_LEN])
{
#ifdef INET
	struct sockaddr_in *sin;
#endif /* INET */
#ifdef INET6
	struct sockaddr_in6 *sin6;
#endif /* INET6 */

	switch (sa->sa_family) {

	case AF_UNSPEC:
		bcopy(sa->sa_data, addrlo, ETHER_ADDR_LEN);
		bcopy(addrlo, addrhi, ETHER_ADDR_LEN);
		break;

#ifdef INET
	case AF_INET:
		sin = satosin(sa);
		if (sin->sin_addr.s_addr == INADDR_ANY) {
			/*
			 * An IP address of INADDR_ANY means listen to
			 * or stop listening to all of the Ethernet
			 * multicast addresses used for IP.
			 * (This is for the sake of IP multicast routers.)
			 */
			bcopy(ether_ipmulticast_min, addrlo, ETHER_ADDR_LEN);
			bcopy(ether_ipmulticast_max, addrhi, ETHER_ADDR_LEN);
		} else {
			ETHER_MAP_IP_MULTICAST(&sin->sin_addr, addrlo);
			bcopy(addrlo, addrhi, ETHER_ADDR_LEN);
		}
		break;
#endif
#ifdef INET6
	case AF_INET6:
		sin6 = satosin6(sa);
		if (IN6_IS_ADDR_UNSPECIFIED(&sin6->sin6_addr)) {
			/*
			 * An IP6 address of 0 means listen to or stop
			 * listening to all of the Ethernet multicast
			 * address used for IP6.
			 *
			 * (This might not be healthy, given IPv6's reliance on
			 * multicast for things like neighbor discovery.
			 * Perhaps initializing all-nodes, solicited nodes, and
			 * possibly all-routers for this interface afterwards
			 * is not a bad idea.)
			 */

			bcopy(ether_ip6multicast_min, addrlo, ETHER_ADDR_LEN);
			bcopy(ether_ip6multicast_max, addrhi, ETHER_ADDR_LEN);
		} else {
			ETHER_MAP_IPV6_MULTICAST(&sin6->sin6_addr, addrlo);
			bcopy(addrlo, addrhi, ETHER_ADDR_LEN);
		}
		break;
#endif

	default:
		return (EAFNOSUPPORT);
	}
	return (0);
}

/*
 * Add an Ethernet multicast address or range of addresses to the list for a
 * given interface.
 */

int ether_addmulti(struct ifreq *ifr, struct arpcom *ac) /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator */
{
	struct ether_multi *enm;
	u_char addrlo[ETHER_ADDR_LEN];
	u_char addrhi[ETHER_ADDR_LEN];
	int error;

	error = ether_multiaddr(&ifr->ifr_addr, addrlo, addrhi);
	if (error != 0) {
		return (error);
	}

	/*
	 * Verify that we have valid Ethernet multicast addresses.
	 */
	if ((addrlo[0] & 0x01) != 1 || (addrhi[0] & 0x01) != 1) {
		return (EINVAL);
	}
	/*
	 * See if the address range is already in the list.
	 */
	ETHER_LOOKUP_MULTI(addrlo, addrhi, ac, enm);
	if (enm != NULL) {
		/*
		 * Found it; just increment the reference count.
		 */
		++enm->enm_refcount;
		return (0);
	}
	/*
	 * New address or range; malloc a new multicast record
	 * and link it into the interface's multicast list.
	 */
	enm = malloc(sizeof(*enm), M_IFMADDR, M_NOWAIT);
	if (enm == NULL) {
		return (ENOBUFS);
	}
	bcopy(addrlo, enm->enm_addrlo, ETHER_ADDR_LEN);
	bcopy(addrhi, enm->enm_addrhi, ETHER_ADDR_LEN);
	enm->enm_ac = ac;
	enm->enm_refcount = 1;
	LIST_INSERT_HEAD(&ac->ac_multiaddrs, enm, enm_list);
	ac->ac_multicnt++;
	if (bcmp(addrlo, addrhi, ETHER_ADDR_LEN) != 0)
		ac->ac_multirangecnt++;
	/*
	 * Return ENETRESET to inform the driver that the list has changed
	 * and its reception filter should be adjusted accordingly.
	 */
	return (ENETRESET);
}

/*
 * Delete a multicast address record.
 */

int ether_delmulti(struct ifreq *ifr, struct arpcom *ac) /* gerlach/gh2289n: new declarator to avoid compiler warning C4131 (uses old-style declarator */
{
	struct ether_multi *enm;
	u_char addrlo[ETHER_ADDR_LEN];
	u_char addrhi[ETHER_ADDR_LEN];
	int error;

	error = ether_multiaddr(&ifr->ifr_addr, addrlo, addrhi);
	if (error != 0) {
		return (error);
	}

	/*
	 * Look up the address in our list.
	 */
	ETHER_LOOKUP_MULTI(addrlo, addrhi, ac, enm);
	if (enm == NULL) {
		return (ENXIO);
	}
	if (--enm->enm_refcount != 0) {
		/*
		 * Still some claims to this record.
		 */
		return (0);
	}
	/*
	 * No remaining claims to this record; unlink and free it.
	 */
	LIST_REMOVE(enm, enm_list);
	free(enm, M_IFMADDR);
	ac->ac_multicnt--;
	if (bcmp(addrlo, addrhi, ETHER_ADDR_LEN) != 0)
		ac->ac_multirangecnt--;
	/*
	 * Return ENETRESET to inform the driver that the list has changed
	 * and its reception filter should be adjusted accordingly.
	 */
	return (ENETRESET);
}
