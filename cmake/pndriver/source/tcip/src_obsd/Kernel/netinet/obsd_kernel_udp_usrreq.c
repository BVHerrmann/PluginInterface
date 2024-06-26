/*	$OpenBSD: udp_usrreq.c,v 1.145 2011/07/08 18:30:17 yasuoka Exp $	*/
/*	$NetBSD: udp_usrreq.c,v 1.28 1996/03/16 23:54:03 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993
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

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_udp_usrreq_act_module) */
#define LTRC_ACT_MODUL_ID 4029 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_udp_usrreq_act_module) */
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
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_sysctl.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_route.h>

#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_in_var.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_in_pcb.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_ip_icmp.h>
#include <netinet/obsd_kernel_udp.h>
#include <netinet/obsd_kernel_udp_var.h>

/* OBSD_EVENTS_PATCH_START - patch no. (obsd_include_userland_types) */
#include <obsd_userland_pnio_types_def.h>
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_include_userland_types) */

#ifdef IPSEC
#include <netinet/obsd_kernel_ip_ipsp.h>
#include <netinet/obsd_kernel_ip_esp.h>
#endif

#ifdef INET6
#ifndef INET
#include <netinet/obsd_kernel_in.h>
#endif
#include <netinet6/obsd_kernel_ip6protosw.h>

extern int ip6_defhlim;
#endif /* INET6 */

#include "obsd_kernel_faith.h"
#if NFAITH > 0
#include <net/obsd_kernel_if_types.h>
#endif

#include "obsd_kernel_pf.h"
#if NPF > 0
#include <net/obsd_kernel_pfvar.h>
#endif

#ifdef PIPEX 
#include <netinet/obsd_kernel_if_ether.h>
#include <net/obsd_kernel_pipex.h>
#endif

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */
#define OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET 1 /* gh2289n, TFS RQ 1400291*/

#if OBSD_PNIO_CFG_TRACE_ON
void controller_add_recv_measure_4();
void controller_add_recv_measure_5(int dropped);
#endif /* OBSD_PNIO_CFG_TRACE_ON */

/*
 * UDP protocol implementation.
 * Per RFC 768, August, 1980.
 */
int	udpcksum = 1;

u_int	udp_sendspace = 9216;		/* really max datagram size */
u_int	udp_recvspace = 40 * (1024 + sizeof(struct sockaddr_in));
					/* 40 1K datagrams */

int *udpctl_vars[UDPCTL_MAXID] = UDPCTL_VARS;

struct	inpcbtable udbtable;
struct	udpstat udpstat;

void udp_detach(struct inpcb *);
void udp_notify(struct inpcb *, int);

#ifndef UDBHASHSIZE
#define	UDBHASHSIZE	128
#endif
int	udbhashsize = UDBHASHSIZE;

/* from in_pcb.c */
extern	struct baddynamicports baddynamicports;

void
udp_init()
{
	in_pcbinit(&udbtable, udbhashsize);
}

void udp_deinit(void)
{
    in_pcbdeinit(&udbtable);
}

#ifdef INET6
int
udp6_input(struct mbuf **mp, int *offp, int proto)
{
	struct mbuf *m = *mp;

#if NFAITH > 0
	if (m->m_pkthdr.rcvif) {
		if (m->m_pkthdr.rcvif->if_type == IFT_FAITH) {
			/* XXX send icmp6 host/port unreach? */
			m_freem(m);
			return IPPROTO_DONE;
		}
	}
#endif

	udp_input(m, *offp, proto);
	return IPPROTO_DONE;
}
#endif

void
udp_input(struct mbuf *m, ...)
{
	struct ip *ip;
	struct udphdr *uh;
	struct inpcb *inp = NULL;
	struct mbuf *opts = NULL;
    struct ip save_ip = {0}; /* gerlach/gh2289n: added init value to avoid compiler warning C4701: potentially uninitialized local variable used */
	int iphlen, len;
	va_list ap;
	u_int16_t savesum;
	union {
		struct sockaddr sa;
		struct sockaddr_in sin;
#ifdef INET6
		struct sockaddr_in6 sin6;
#endif /* INET6 */
	} srcsa, dstsa;
#ifdef INET6
	struct ip6_hdr *ip6;
#endif /* INET6 */
#ifdef IPSEC
	struct m_tag *mtag;
	struct tdb_ident *tdbi;
	struct tdb *tdb;
	int error, s;
#endif /* IPSEC */
#if       OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET
	int is_broadcast = 0;
#endif /* OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET */

	va_start(ap, m);
	iphlen = va_arg(ap, int);
	va_end(ap);

	udpstat.udps_ipackets++;

	switch (CAL_GET_HIGH_NIBBLE_OF_BYTE(mtod(m, struct ip *)->ip_v_ip_hl)) {
	case 4:
		ip = mtod(m, struct ip *);
#ifdef INET6
		ip6 = NULL;
#endif /* INET6 */
		srcsa.sa.sa_family = AF_INET;
		break;
#ifdef INET6
	case 6:
		ip = NULL;
		ip6 = mtod(m, struct ip6_hdr *);
		srcsa.sa.sa_family = AF_INET6;
		break;
#endif /* INET6 */
	default:
		goto bad;
	}

	IP6_EXTHDR_GET(uh, struct udphdr *, m, iphlen, sizeof(struct udphdr));
	if (!uh) {
		udpstat.udps_hdrops++;
		return;
	}

	/* Check for illegal destination port 0 */
	if (uh->uh_dport == 0) {
		udpstat.udps_noport++;
		goto bad;
	}

	/*
	 * Make mbuf data length reflect UDP length.
	 * If not enough data to reflect UDP length, drop.
	 */
	len = ntohs((u_int16_t)uh->uh_ulen);
	if (ip) {
		if (m->m_pkthdr.len - iphlen != len) {
			if (len > (m->m_pkthdr.len - iphlen) ||
			    len < sizeof(struct udphdr)) {
				udpstat.udps_badlen++;
				goto bad;
			}
			m_adj(m, len - (m->m_pkthdr.len - iphlen));
		}
	}
#ifdef INET6
	else if (ip6) {
		/* jumbograms */
		if (len == 0 && m->m_pkthdr.len - iphlen > 0xffff)
			len = m->m_pkthdr.len - iphlen;
		if (len != m->m_pkthdr.len - iphlen) {
			udpstat.udps_badlen++;
			goto bad;
		}
	}
#endif
	else /* shouldn't happen */
		goto bad;

	/*
	 * Save a copy of the IP header in case we want restore it
	 * for sending an ICMP error message in response.
	 */
	if (ip)
		save_ip = *ip;

	/*
	 * Checksum extended UDP header and data.
	 * from W.R.Stevens: check incoming udp cksums even if
	 *	udpcksum is not set.
	 */
	savesum = uh->uh_sum;
#ifdef INET6
	if (ip6) {
		/* Be proactive about malicious use of IPv4 mapped address */
		if (IN6_IS_ADDR_V4MAPPED(&ip6->ip6_src) ||
		    IN6_IS_ADDR_V4MAPPED(&ip6->ip6_dst)) {
			/* XXX stat */
			goto bad;
		}

		/*
		 * In IPv6, the UDP checksum is ALWAYS used.
		 */
		if (uh->uh_sum == 0) {
			udpstat.udps_nosum++;
			goto bad;
		}
		if ((m->m_pkthdr.csum_flags & M_UDP_CSUM_IN_OK) == 0) {
			if (m->m_pkthdr.csum_flags & M_UDP_CSUM_IN_BAD) {
				udpstat.udps_badsum++;
				udpstat.udps_inhwcsum++;
				goto bad;
			}

			if ((uh->uh_sum = in6_cksum(m, IPPROTO_UDP,
			    iphlen, len))) {
				udpstat.udps_badsum++;
				goto bad;
			}
		} else {
			m->m_pkthdr.csum_flags &= ~M_UDP_CSUM_IN_OK;
			udpstat.udps_inhwcsum++;
		}
	} else
#endif /* INET6 */
	if (uh->uh_sum) {
		if ((m->m_pkthdr.csum_flags & M_UDP_CSUM_IN_OK) == 0) {
			if (m->m_pkthdr.csum_flags & M_UDP_CSUM_IN_BAD) {
				udpstat.udps_badsum++;
				udpstat.udps_inhwcsum++;
				m_freem(m);
				return;
			}

			if (((uh->uh_sum = (u_int16_t)in4_cksum(m, IPPROTO_UDP, /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			    iphlen, len)) != 0)) {      /* gh2289n: added 0 comparison to avoid PN compiler error */
				udpstat.udps_badsum++;
				m_freem(m);
				return;
			}
		} else {
			m->m_pkthdr.csum_flags &= ~M_UDP_CSUM_IN_OK;
			udpstat.udps_inhwcsum++;
		}
	} else
		udpstat.udps_nosum++;

#ifdef IPSEC
	if (udpencap_enable && udpencap_port &&
	    uh->uh_dport == htons(udpencap_port)) {
		u_int32_t spi;
		int skip = iphlen + sizeof(struct udphdr);

		if (m->m_pkthdr.len - skip < sizeof(u_int32_t)) {
			/* packet too short */
			m_freem(m);
			return;
		}
		m_copydata(m, skip, sizeof(u_int32_t), (caddr_t) &spi);
		/*
		 * decapsulate if the SPI is not zero, otherwise pass
		 * to userland
		 */
		if (spi != 0) {
			if ((m = m_pullup(m, skip)) == NULL) {
				udpstat.udps_hdrops++;
				return;
			}

			/* remove the UDP header */
			bcopy(mtod(m, u_char *),
			    mtod(m, u_char *) + sizeof(struct udphdr), iphlen);
			m_adj(m, sizeof(struct udphdr));
			skip -= sizeof(struct udphdr);

			espstat.esps_udpencin++;
			ipsec_common_input(m, skip, offsetof(struct ip, ip_p),
			    srcsa.sa.sa_family, IPPROTO_ESP, 1);
			return;
		}
	}
#endif

	switch (srcsa.sa.sa_family) {
	case AF_INET:
		bzero(&srcsa, sizeof(struct sockaddr_in));
		srcsa.sin.sin_len = sizeof(struct sockaddr_in);
		srcsa.sin.sin_family = AF_INET;
		srcsa.sin.sin_port = uh->uh_sport;
		srcsa.sin.sin_addr = ip->ip_src;

		bzero(&dstsa, sizeof(struct sockaddr_in));
		dstsa.sin.sin_len = sizeof(struct sockaddr_in);
		dstsa.sin.sin_family = AF_INET;
		dstsa.sin.sin_port = uh->uh_dport;
		dstsa.sin.sin_addr = ip->ip_dst;
		break;
#ifdef INET6
	case AF_INET6:
		bzero(&srcsa, sizeof(struct sockaddr_in6));
		srcsa.sin6.sin6_len = sizeof(struct sockaddr_in6);
		srcsa.sin6.sin6_family = AF_INET6;
		srcsa.sin6.sin6_port = uh->uh_sport;
#if 0 /*XXX inbound flowinfo */
		srcsa.sin6.sin6_flowinfo = htonl(0x0fffffff) & ip6->ip6_flow;
#endif
		/* KAME hack: recover scopeid */
		(void)in6_recoverscope(&srcsa.sin6, &ip6->ip6_src,
		    m->m_pkthdr.rcvif);

		bzero(&dstsa, sizeof(struct sockaddr_in6));
		dstsa.sin6.sin6_len = sizeof(struct sockaddr_in6);
		dstsa.sin6.sin6_family = AF_INET6;
		dstsa.sin6.sin6_port = uh->uh_dport;
		/* KAME hack: recover scopeid */
		(void)in6_recoverscope(&dstsa.sin6, &ip6->ip6_dst,
		    m->m_pkthdr.rcvif);
		break;
#endif /* INET6 */
	}

#ifdef INET6
	if ((ip6 && IN6_IS_ADDR_MULTICAST(&ip6->ip6_dst)) ||
	    (ip && IN_MULTICAST(ip->ip_dst.s_addr)) ||
	    (ip && in_broadcast(ip->ip_dst, m->m_pkthdr.rcvif,
	    m->m_pkthdr.rdomain))) {
#else /* INET6 */
	if (IN_MULTICAST(ip->ip_dst.s_addr) ||
#if       OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET
        (is_broadcast = in_broadcast(ip->ip_dst, m->m_pkthdr.rcvif, m->m_pkthdr.rdomain)) == 1) {
#else  /* OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET */
	    in_broadcast(ip->ip_dst, m->m_pkthdr.rcvif, m->m_pkthdr.rdomain)) {
#endif /* OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET */
#endif /* INET6 */
		struct inpcb *last;
		/*
		 * Deliver a multicast or broadcast datagram to *all* sockets
		 * for which the local and remote addresses and ports match
		 * those of the incoming datagram.  This allows more than
		 * one process to receive multi/broadcasts on the same port.
		 * (This really ought to be done for unicast datagrams as
		 * well, but that would cause problems with existing
		 * applications that open both address-specific sockets and
		 * a wildcard socket listening to the same port -- they would
		 * end up receiving duplicates of every unicast datagram.
		 * Those applications open the multiple sockets to overcome an
		 * inadequacy of the UDP socket interface, but for backwards
		 * compatibility we avoid the problem here rather than
		 * fixing the interface.  Maybe 4.5BSD will remedy this?)
		 */

		iphlen += sizeof(struct udphdr);

		/*
		 * Locate pcb(s) for datagram.
		 * (Algorithm copied from raw_intr().)
		 */
		last = NULL;
		CIRCLEQ_FOREACH(inp, &udbtable.inpt_queue, inp_queue) {
			if (inp->inp_socket->so_state & SS_CANTRCVMORE)
				continue;
#ifdef INET6
			/* don't accept it if AF does not match */
			if (ip6 && !(inp->inp_flags & INP_IPV6))
				continue;
			if (!ip6 && (inp->inp_flags & INP_IPV6))
				continue;
#endif
			if (rtable_l2(inp->inp_rtableid) !=
			    rtable_l2(m->m_pkthdr.rdomain))
				continue;
			if (inp->inp_lport != uh->uh_dport)
				continue;
#ifdef INET6
			if (ip6) {
				if (!IN6_IS_ADDR_UNSPECIFIED(&inp->inp_laddr6))
					if (!IN6_ARE_ADDR_EQUAL(&inp->inp_laddr6,
					    &ip6->ip6_dst))
						continue;
			} else
#endif /* INET6 */
			if (inp->inp_laddr.s_addr != INADDR_ANY) {
				if (inp->inp_laddr.s_addr != ip->ip_dst.s_addr)
#if       OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET /* gh2289n */
			{   /* TFS RQ 1400291: special C1 solution for interniche compatibility (see RQ2710554) */
				/* Receive Broadcast messages on an socket that is bound to a local IP-Address and not to INADDR_ANY */
				if (is_broadcast && ((inp->inp_socket->so_options & SO_BROADCAST) != 0))/* only for broadcasts (not multicasts) */
				                                                                        /* and only if SO_BROADCAST is set      */
				{
					int             found = 0;
					struct ifaddr * ifa;

					TAILQ_FOREACH(ifa, &m->m_pkthdr.rcvif->if_addrlist, ifa_list) 
					{   /* check if the local address the socket is bound to is one of the IP adresses of the receive-interface */
						const struct sockaddr_in * ifaddr = (const struct sockaddr_in *)(ifa->ifa_addr);

						if ( (ifaddr->sin_family == AF_INET) && (ifaddr->sin_addr.s_addr == inp->inp_laddr.s_addr))
						{
							found = 1;
							break;
						}
					}
					if (found == 0)
					{
						continue;   /* the receiving interface has no IP addr to which the socket is bound to */
					}
				}
				else
					continue;
			}
#else  /* OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET */
					continue;
#endif /* OBSD_PN_UDP_RECV_BROADCAST_PATCH_ON_FILTERED_SOCKET */
			}
#ifdef INET6
			if (ip6) {
				if (!IN6_IS_ADDR_UNSPECIFIED(&inp->inp_faddr6))
					if (!IN6_ARE_ADDR_EQUAL(&inp->inp_faddr6,
					    &ip6->ip6_src) ||
					    inp->inp_fport != uh->uh_sport)
						continue;
			} else
#endif /* INET6 */
			if (inp->inp_faddr.s_addr != INADDR_ANY) {
				if (inp->inp_faddr.s_addr !=
				    ip->ip_src.s_addr ||
				    inp->inp_fport != uh->uh_sport)
					continue;
			}

			if (last != NULL) {
				struct mbuf *n;

#if OBSD_PNIO_CFG_TRACE_ON
				n = m_copy(m, 0, M_COPYALL);
				if (n != NULL) {
					controller_add_recv_measure_4(); /* before copy into socket buffer */
#else
				if ((n = m_copy(m, 0, M_COPYALL)) != NULL) {
#endif /* OBSD_PNIO_CFG_TRACE_ON */

#ifdef INET6
					if (ip6 && (last->inp_flags &
					    IN6P_CONTROLOPTS ||
					    last->inp_socket->so_options &
					    SO_TIMESTAMP))
						ip6_savecontrol(last, n, &opts);
#endif /* INET6 */
					if (ip && (last->inp_flags &
					    INP_CONTROLOPTS ||
					    last->inp_socket->so_options &
					    SO_TIMESTAMP))
						ip_savecontrol(last, &opts,
						    ip, n);

					m_adj(n, iphlen);
					if (sbappendaddr(
					    &last->inp_socket->so_rcv,
					    &srcsa.sa, n, opts) == 0) {
						m_freem(n);
						if (opts)
							m_freem(opts);

#if OBSD_PNIO_CFG_TRACE_ON
						/* sado: C1 Performance Tracing -- receive buffer full (see RQ2710554) */
						controller_add_recv_measure_5(1); /* for stat integrity: measure in this case also */
						TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "instrumentation: udp_input: so_recv buffer full");
#endif /* OBSD_PNIO_CFG_TRACE_ON */

						udpstat.udps_fullsock++;
					} else
					{
						sorwakeup(last->inp_socket);
#if OBSD_PNIO_CFG_TRACE_ON
						controller_add_recv_measure_5(0); /* after copy into socket buffer */
#endif /* OBSD_PNIO_CFG_TRACE_ON */

/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_udp_usrreq_events_001) */
						obsd_pnio_add_socket_read_event(last->inp_socket);
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_udp_usrreq_events_001) */
					}
					opts = NULL;
				}
			}
			last = inp;
			/*
			 * Don't look for additional matches if this one does
			 * not have either the SO_REUSEPORT or SO_REUSEADDR
			 * socket options set.  This heuristic avoids searching
			 * through all pcbs in the common case of a non-shared
			 * port.  It assumes that an application will never
			 * clear these options after setting them.
			 */
			if ((last->inp_socket->so_options & (SO_REUSEPORT |
			    SO_REUSEADDR)) == 0)
				break;
		}

		if (last == NULL) {
			/*
			 * No matching pcb found; discard datagram.
			 * (No need to send an ICMP Port Unreachable
			 * for a broadcast or multicast datgram.)
			 */
			udpstat.udps_noportbcast++;
			goto bad;
		}

#ifdef INET6
		if (ip6 && (last->inp_flags & IN6P_CONTROLOPTS ||
		    last->inp_socket->so_options & SO_TIMESTAMP))
			ip6_savecontrol(last, m, &opts);
#endif /* INET6 */
		if (ip && (last->inp_flags & INP_CONTROLOPTS ||
		    last->inp_socket->so_options & SO_TIMESTAMP))
			ip_savecontrol(last, &opts, ip, m);

#if OBSD_PNIO_CFG_TRACE_ON
		controller_add_recv_measure_4(); /* before copy into socket buffer */
#endif /* OBSD_PNIO_CFG_TRACE_ON */

		m_adj(m, iphlen);
		if (sbappendaddr(&last->inp_socket->so_rcv,
		    &srcsa.sa, m, opts) == 0) {
#if OBSD_PNIO_CFG_TRACE_ON
			/* sado: C1 Performance Tracing -- receive buffer full (see RQ2710554)*/
			controller_add_recv_measure_5(1); /* for stat integrity: measure in this case also */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "instrumentation:udp_input: so_recv buffer full");
#endif /* OBSD_PNIO_CFG_TRACE_ON */

			udpstat.udps_fullsock++;
			goto bad;
		}
		sorwakeup(last->inp_socket);

#if OBSD_PNIO_CFG_TRACE_ON
		controller_add_recv_measure_5(0);
#endif /* OBSD_PNIO_CFG_TRACE_ON */

/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_udp_usrreq_events_002) */
		obsd_pnio_add_socket_read_event(last->inp_socket);
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_udp_usrreq_events_002) */
		return;
	}
	/*
	 * Locate pcb for datagram.
	 */
#if 0
	if (m->m_pkthdr.pf.statekey)
		inp = ((struct pf_state_key *)m->m_pkthdr.pf.statekey)->inp;
#endif
	if (inp == NULL) {
#ifdef INET6
		if (ip6)
			inp = in6_pcbhashlookup(&udbtable, &ip6->ip6_src,
			    uh->uh_sport, &ip6->ip6_dst, uh->uh_dport);
		else
#endif /* INET6 */
		inp = in_pcbhashlookup(&udbtable, ip->ip_src, uh->uh_sport,
		    ip->ip_dst, uh->uh_dport, m->m_pkthdr.rdomain);
#if NPF > 0
		if (m->m_pkthdr.pf.statekey && inp) {
			((struct pf_state_key *)m->m_pkthdr.pf.statekey)->inp =
			    inp;
			inp->inp_pf_sk = m->m_pkthdr.pf.statekey;
		}
#endif
	}
	if (inp == 0) {
		int	inpl_reverse = 0;
		if (m->m_pkthdr.pf.flags & PF_TAG_TRANSLATE_LOCALHOST)
			inpl_reverse = 1;
		++udpstat.udps_pcbhashmiss;
#ifdef INET6
		if (ip6) {
			inp = in6_pcblookup_listen(&udbtable,
			    &ip6->ip6_dst, uh->uh_dport, inpl_reverse, m);
		} else
#endif /* INET6 */
		inp = in_pcblookup_listen(&udbtable,
		    ip->ip_dst, uh->uh_dport, inpl_reverse, m,
		    m->m_pkthdr.rdomain);
		if (inp == 0) {
			udpstat.udps_noport++;
			if (m->m_flags & (M_BCAST | M_MCAST)) {
				udpstat.udps_noportbcast++;
				goto bad;
			}
#ifdef INET6
			if (ip6) {
				uh->uh_sum = savesum;
				icmp6_error(m, ICMP6_DST_UNREACH,
				    ICMP6_DST_UNREACH_NOPORT,0);
			} else
#endif /* INET6 */
			{
				*ip = save_ip;
				uh->uh_sum = savesum;
				icmp_error(m, ICMP_UNREACH, ICMP_UNREACH_PORT,
				    0, 0);
			}
			return;
		}
	}

#ifdef IPSEC
	mtag = m_tag_find(m, PACKET_TAG_IPSEC_IN_DONE, NULL);
	if (mtag != NULL) {
		tdbi = (struct tdb_ident *)(mtag + 1);
		tdb = gettdb(tdbi->rdomain, tdbi->spi,
		    &tdbi->dst, tdbi->proto);
	} else
		tdb = NULL;
	ipsp_spd_lookup(m, srcsa.sa.sa_family, iphlen, &error,
	    IPSP_DIRECTION_IN, tdb, inp);
	if (error) {
		goto bad;
	}

	/* Latch SA only if the socket is connected */
	if (inp->inp_tdb_in != tdb &&
	    (inp->inp_socket->so_state & SS_ISCONNECTED)) {
		if (tdb) {
			tdb_add_inp(tdb, inp, 1);
			if (inp->inp_ipo == NULL) {
				inp->inp_ipo = ipsec_add_policy(inp,
				    srcsa.sa.sa_family, IPSP_DIRECTION_OUT);
				if (inp->inp_ipo == NULL) {
					goto bad;
				}
			}
			if (inp->inp_ipo->ipo_dstid == NULL &&
			    tdb->tdb_srcid != NULL) {
				inp->inp_ipo->ipo_dstid = tdb->tdb_srcid;
				tdb->tdb_srcid->ref_count++;
			}
			if (inp->inp_ipsec_remotecred == NULL &&
			    tdb->tdb_remote_cred != NULL) {
				inp->inp_ipsec_remotecred =
				    tdb->tdb_remote_cred;
				tdb->tdb_remote_cred->ref_count++;
			}
			if (inp->inp_ipsec_remoteauth == NULL &&
			    tdb->tdb_remote_auth != NULL) {
				inp->inp_ipsec_remoteauth =
				    tdb->tdb_remote_auth;
				tdb->tdb_remote_auth->ref_count++;
			}
		} else { /* Just reset */
			TAILQ_REMOVE(&inp->inp_tdb_in->tdb_inp_in, inp,
			    inp_tdb_in_next);
			inp->inp_tdb_in = NULL;
		}
	}
#endif /*IPSEC */

	opts = NULL;
#ifdef INET6
	if (ip6 && (inp->inp_flags & IN6P_CONTROLOPTS ||
	    inp->inp_socket->so_options & SO_TIMESTAMP))
		ip6_savecontrol(inp, m, &opts);
#endif /* INET6 */
	if (ip && (inp->inp_flags & INP_CONTROLOPTS ||
	    inp->inp_socket->so_options & SO_TIMESTAMP))
		ip_savecontrol(inp, &opts, ip, m);
	if (ip && (inp->inp_flags & INP_RECVDSTPORT)) {
		struct mbuf **mp = &opts;

		while (*mp)
			mp = &(*mp)->m_next;
		{ /* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/
		u_int16_t portnum = uh->uh_dport;

		*mp = sbcreatecontrol((caddr_t)&portnum, sizeof(u_int16_t),
		    IP_RECVDSTPORT, IPPROTO_IP);
		}
	}
#ifdef PIPEX
	if (pipex_enable && inp->inp_pipex) {
		struct pipex_session *session;
		int off = iphlen + sizeof(struct udphdr);
		if ((session = pipex_l2tp_lookup_session(m, off)) != NULL) {
			if ((m = pipex_l2tp_input(m, off, session)) == NULL)
				return; /* the packet is handled by PIPEX */
		}
	}
#endif

	iphlen += sizeof(struct udphdr);
	m_adj(m, iphlen);

#if OBSD_PNIO_CFG_TRACE_ON
	controller_add_recv_measure_4(); /* before copy into socket buffer */
#endif /* OBSD_PNIO_CFG_TRACE_ON */

	if (sbappendaddr(&inp->inp_socket->so_rcv, &srcsa.sa, m, opts) == 0) {

#if OBSD_PNIO_CFG_TRACE_ON
		/* sado: C1 Performance Tracing -- receive buffer full (see RQ2710554) */
		controller_add_recv_measure_5(1); /* for stat integrity: measure in this case also */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "instrumentation: udp_input: so_recv buffer full");
#endif /* OBSD_PNIO_CFG_TRACE_ON */

		udpstat.udps_fullsock++;
		goto bad;
	}
	sorwakeup(inp->inp_socket);

#if OBSD_PNIO_CFG_TRACE_ON
	controller_add_recv_measure_5(0);
#endif /* OBSD_PNIO_CFG_TRACE_ON */

/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_udp_usrreq_events_003) */
	obsd_pnio_add_socket_read_event(inp->inp_socket);
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_udp_usrreq_events_003) */
	return;
bad:
	m_freem(m);
	if (opts)
		m_freem(opts);
}

/*
 * Notify a udp user of an asynchronous error;
 * just wake up so that he can collect error status.
 */
void
udp_notify(struct inpcb *inp, int errno)
{
	inp->inp_socket->so_error = (u_short) errno; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	sorwakeup(inp->inp_socket);
	sowwakeup(inp->inp_socket);
}

#ifdef INET6
void
udp6_ctlinput(int cmd, struct sockaddr *sa, void *d)
{
	struct udphdr uh;
	struct sockaddr_in6 sa6;
	struct ip6_hdr *ip6;
	struct mbuf *m;
	int off;
	void *cmdarg;
	struct ip6ctlparam *ip6cp = NULL;
	struct udp_portonly {
		u_int16_t uh_sport;
		u_int16_t uh_dport;
	} *uhp;
	void (*notify)(struct inpcb *, int) = udp_notify;

	if (sa == NULL)
		return;
	if (sa->sa_family != AF_INET6 ||
	    sa->sa_len != sizeof(struct sockaddr_in6))
		return;

	if ((unsigned)cmd >= PRC_NCMDS)
		return;
	if (PRC_IS_REDIRECT(cmd))
		notify = in_rtchange, d = NULL;
	else if (cmd == PRC_HOSTDEAD)
		d = NULL;
	else if (cmd == PRC_MSGSIZE)
		; /* special code is present, see below */
	else if (inet6ctlerrmap[cmd] == 0)
		return;

	/* if the parameter is from icmp6, decode it. */
	if (d != NULL) {
		ip6cp = (struct ip6ctlparam *)d;
		m = ip6cp->ip6c_m;
		ip6 = ip6cp->ip6c_ip6;
		off = ip6cp->ip6c_off;
		cmdarg = ip6cp->ip6c_cmdarg;
	} else {
		m = NULL;
		ip6 = NULL;
		cmdarg = NULL;
		/* XXX: translate addresses into internal form */
		sa6 = *(struct sockaddr_in6 *)sa;
#ifndef SCOPEDROUTING
		if (in6_embedscope(&sa6.sin6_addr, &sa6, NULL, NULL)) {
			/* should be impossible */
			return;
		}
#endif
	}

	if (ip6cp && ip6cp->ip6c_finaldst) {
		bzero(&sa6, sizeof(sa6));
		sa6.sin6_family = AF_INET6;
		sa6.sin6_len = sizeof(sa6);
		sa6.sin6_addr = *ip6cp->ip6c_finaldst;
		/* XXX: assuming M is valid in this case */
		sa6.sin6_scope_id = in6_addr2scopeid(m->m_pkthdr.rcvif,
		    ip6cp->ip6c_finaldst);
#ifndef SCOPEDROUTING
		if (in6_embedscope(ip6cp->ip6c_finaldst, &sa6, NULL, NULL)) {
			/* should be impossible */
			return;
		}
#endif
	} else {
		/* XXX: translate addresses into internal form */
		sa6 = *(struct sockaddr_in6 *)sa;
#ifndef SCOPEDROUTING
		if (in6_embedscope(&sa6.sin6_addr, &sa6, NULL, NULL)) {
			/* should be impossible */
			return;
		}
#endif
	}

	if (ip6) {
		/*
		 * XXX: We assume that when IPV6 is non NULL,
		 * M and OFF are valid.
		 */
		struct sockaddr_in6 sa6_src;

		/* check if we can safely examine src and dst ports */
		if (m->m_pkthdr.len < off + sizeof(*uhp))
			return;

		bzero(&uh, sizeof(uh));
		m_copydata(m, off, sizeof(*uhp), (caddr_t)&uh);

		bzero(&sa6_src, sizeof(sa6_src));
		sa6_src.sin6_family = AF_INET6;
		sa6_src.sin6_len = sizeof(sa6_src);
		sa6_src.sin6_addr = ip6->ip6_src;
		sa6_src.sin6_scope_id = in6_addr2scopeid(m->m_pkthdr.rcvif,
		    &ip6->ip6_src);
#ifndef SCOPEDROUTING
		if (in6_embedscope(&sa6_src.sin6_addr, &sa6_src, NULL, NULL)) {
			/* should be impossible */
			return;
		}
#endif

		if (cmd == PRC_MSGSIZE) {
			int valid = 0;

			/*
			 * Check to see if we have a valid UDP socket
			 * corresponding to the address in the ICMPv6 message
			 * payload.
			 */
			if (in6_pcbhashlookup(&udbtable, &sa6.sin6_addr,
			    uh.uh_dport, &sa6_src.sin6_addr, uh.uh_sport))
				valid = 1;
#if 0
			/*
			 * As the use of sendto(2) is fairly popular,
			 * we may want to allow non-connected pcb too.
			 * But it could be too weak against attacks...
			 * We should at least check if the local address (= s)
			 * is really ours.
			 */
			else if (in6_pcblookup_listen(&udbtable,
			    &sa6_src.sin6_addr, uh.uh_sport, 0);
				valid = 1;
#endif

			/*
			 * Depending on the value of "valid" and routing table
			 * size (mtudisc_{hi,lo}wat), we will:
			 * - recalculate the new MTU and create the
			 *   corresponding routing entry, or
			 * - ignore the MTU change notification.
			 */
			icmp6_mtudisc_update((struct ip6ctlparam *)d, valid);

			/*
			 * regardless of if we called icmp6_mtudisc_update(),
			 * we need to call in6_pcbnotify(), to notify path
			 * MTU change to the userland (2292bis-02), because
			 * some unconnected sockets may share the same
			 * destination and want to know the path MTU.
			 */
		}

		(void) in6_pcbnotify(&udbtable, (struct sockaddr *)&sa6,
		    uh.uh_dport, (struct sockaddr *)&sa6_src,
		    uh.uh_sport, cmd, cmdarg, notify);
	} else {
		(void) in6_pcbnotify(&udbtable, (struct sockaddr *)&sa6, 0,
		    (struct sockaddr *)&sa6_any, 0, cmd, cmdarg, notify);
	}
}
#endif

void *
udp_ctlinput(int cmd, struct sockaddr *sa, u_int rdomain, void *v)
{
	struct ip *ip = v;
	struct udphdr *uhp;
	struct in_addr faddr;
	struct inpcb *inp;
	extern int inetctlerrmap[];
	void (*notify)(struct inpcb *, int) = udp_notify;
	int errno;

	if (sa == NULL)
		return NULL;
	if (sa->sa_family != AF_INET ||
	    sa->sa_len != sizeof(struct sockaddr_in))
		return NULL;
	faddr = satosin(sa)->sin_addr;
	if (faddr.s_addr == INADDR_ANY)
		return NULL;

	if ((unsigned)cmd >= PRC_NCMDS)
		return NULL;
	errno = inetctlerrmap[cmd];
	if (PRC_IS_REDIRECT(cmd))
		notify = in_rtchange, ip = 0;
	else if (cmd == PRC_HOSTDEAD)
		ip = 0;
	else if (errno == 0)
		return NULL;
	if (ip) {
		uhp = (struct udphdr *)((caddr_t)ip + (CAL_GET_LOW_NIBBLE_OF_BYTE(ip->ip_v_ip_hl) << 2));

#ifdef IPSEC
		/* PMTU discovery for udpencap */
		if (cmd == PRC_MSGSIZE && ip_mtudisc && udpencap_enable &&
		    udpencap_port && uhp->uh_sport == htons(udpencap_port)) {
			udpencap_ctlinput(cmd, sa, rdomain, v);
			return (NULL);
		}
#endif
		inp = in_pcbhashlookup(&udbtable,
		    ip->ip_dst, uhp->uh_dport, ip->ip_src, uhp->uh_sport,
		    rdomain);
		if (inp && inp->inp_socket != NULL)
			notify(inp, errno);
	} else
		in_pcbnotifyall(&udbtable, sa, rdomain, errno, notify);
	return (NULL);
}

#if OBSD_PNIO_CFG_TRACE_ON
	void controller_add_obsd_send_measure_3();
#endif /* OBSD_PNIO_CFG_TRACE_ON */

int
udp_output(struct mbuf *m, ...)
{
	struct inpcb *inp;
	struct mbuf *addr, *control;
	struct udpiphdr *ui;
	struct sockaddr_in src_sin;
	int len = m->m_pkthdr.len;
    struct in_addr laddr = { 0 }; /* gerlach/gh2289n: added init value to avoid compiler warning C4701: potentially uninitialized local variable used */
	int error = 0;
	va_list ap;

	va_start(ap, m);
	inp = va_arg(ap, struct inpcb *);
	addr = va_arg(ap, struct mbuf *);
	control = va_arg(ap, struct mbuf *);
	va_end(ap);

	/*
	 * Compute the packet length of the IP header, and
	 * punt if the length looks bogus.
	 */
	if ((len + sizeof(struct udpiphdr)) > IP_MAXPACKET) {
		error = EMSGSIZE;
		goto release;
	}

	memset(&src_sin, 0, sizeof(src_sin));

	if (control) {
		u_int clen;
		struct cmsghdr *cm;
		caddr_t cmsgs;

		/* Currently, we assume all the optional information is stored in a single mbuf. */
		if (control->m_next) {
			error = EINVAL;
			goto release;
		}

		clen = control->m_len;
		cmsgs = mtod(control, caddr_t);
		do {
			if (clen < CMSG_LEN(0)) {
				error = EINVAL;
				goto release;
			}
			cm = (struct cmsghdr *)cmsgs;
			if (cm->cmsg_len < CMSG_LEN(0) ||
			    CMSG_ALIGN(cm->cmsg_len) > clen) {
				error = EINVAL;
				goto release;
			}
			if (cm->cmsg_len == CMSG_LEN(sizeof(struct in_addr)) &&
			    cm->cmsg_level == IPPROTO_IP &&
			    cm->cmsg_type == IP_SENDSRCADDR) {
				memcpy(&src_sin.sin_addr, CMSG_DATA(cm),
				    sizeof(struct in_addr));
				src_sin.sin_family = AF_INET;
				src_sin.sin_len = sizeof(src_sin);  
				/* gh2289n: the (source) port from the control message is being ignored later, so we don't  */
				/*          need to check if this port would be usable, otherwise we should do this here.   */
			}
			clen -= CMSG_ALIGN(cm->cmsg_len);
			cmsgs += CMSG_ALIGN(cm->cmsg_len);
		} while (clen);
	}

	if (addr) {
		laddr = inp->inp_laddr;
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			error = EISCONN;
			goto release;
		}
		if (src_sin.sin_len > 0 &&
		    src_sin.sin_addr.s_addr != INADDR_ANY &&
		    src_sin.sin_addr.s_addr != inp->inp_laddr.s_addr) 
		{
			if (inp->inp_laddr.s_addr == INADDR_ANY &&
				/* ds: the source IP should not be a broadcast or multicast address */
				!in_broadcast(src_sin.sin_addr, NULL, inp->inp_rtableid) &&
				!IN_MULTICAST(src_sin.sin_addr.s_addr))
			{
				inp->inp_laddr.s_addr = src_sin.sin_addr.s_addr;    /* use the IPv4 address from the control message   */
				                                                    /* note: this will not really overwrite the        */
				                                                    /* inp_laddr of the socket, because it will be     */
				                                                    /* restored below (see bail)                       */
				/* a possible (source) port of the control message is ignored, means the source port will be unchanged */
			}
		}
		/*
		 * Must block input while temporarily connected.
		 */
		error = in_pcbconnect(inp, addr);
		if (error) {
			/* ds: restore inp_laddr here, because it is not restored in release */
			inp->inp_laddr = laddr;
			goto release;
		}
	} else {
		if (inp->inp_faddr.s_addr == INADDR_ANY) {
			error = ENOTCONN;
			goto release;
		}
	}
	/*
	 * Calculate data length and get a mbuf
	 * for UDP and IP headers.
	 */
	M_PREPEND(m, sizeof(struct udpiphdr), M_DONTWAIT);
	if (m == NULL) {
		error = ENOBUFS;
		goto bail;
	}

	/*
	 * Fill in mbuf with extended UDP header
	 * and addresses and length put into network format.
	 */
	ui = mtod(m, struct udpiphdr *);
	bzero(ui->ui_x1, sizeof ui->ui_x1);
	ui->ui_pr = IPPROTO_UDP;
	ui->ui_len = htons((u_int16_t)len + sizeof (struct udphdr));
	ui->ui_src = inp->inp_laddr;
	ui->ui_dst = inp->inp_faddr;
	ui->ui_sport = inp->inp_lport;
	ui->ui_dport = inp->inp_fport;
	ui->ui_ulen = ui->ui_len;

	/*
	 * Compute the pseudo-header checksum; defer further checksumming
	 * until ip_output() or hardware (if it exists).
	 */
	if (udpcksum) {
		m->m_pkthdr.csum_flags |= M_UDP_CSUM_OUT;
		ui->ui_sum = in_cksum_phdr(ui->ui_src.s_addr,
		    ui->ui_dst.s_addr, htons((u_int16_t)len +
		    sizeof (struct udphdr) + IPPROTO_UDP));
	} else
		ui->ui_sum = 0;
	((struct ip *)ui)->ip_len = htons(sizeof (struct udpiphdr) + len);
	((struct ip *)ui)->ip_ttl = inp->inp_ip.ip_ttl;
	((struct ip *)ui)->ip_tos = inp->inp_ip.ip_tos;

	udpstat.udps_opackets++;

	/* force routing domain */
	m->m_pkthdr.rdomain = inp->inp_rtableid;

#if OBSD_PNIO_CFG_TRACE_ON
	controller_add_obsd_send_measure_3();
#endif /* OBSD_PNIO_CFG_TRACE_ON */

	error = ip_output(m, inp->inp_options, &inp->inp_route,
	    inp->inp_socket->so_options &
	    (SO_DONTROUTE | SO_BROADCAST | SO_JUMBO),
	    inp->inp_moptions, inp);
	if (error == EACCES)	/* translate pf(4) error for userland */
		error = EHOSTUNREACH;

bail:
	if (addr) {
		inp->inp_laddr = laddr;
		in_pcbdisconnect(inp);
	}
	if (control)
		m_freem(control);
	return (error);

release:
	m_freem(m);
	if (control)
		m_freem(control);
	return (error);
}

/*ARGSUSED*/
int
udp_usrreq(struct socket *so, int req, struct mbuf *m, struct mbuf *addr,
    struct mbuf *control, struct proc *p)
{
	struct inpcb *inp = sotoinpcb(so);
	int error = 0;

	if (req == PRU_CONTROL) {
#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			return (in6_control(so, (u_long)m, (caddr_t)addr,
			    (struct ifnet *)control, 0));
		else
#endif /* INET6 */
			return (in_control(so, (u_long)m, (caddr_t)addr,
			    (struct ifnet *)control));
	}
	if (inp == NULL && req != PRU_ATTACH) {
		error = EINVAL;
		goto release;
	}
	/*
	 * Note: need to block udp_input while changing
	 * the udp pcb queue and/or pcb addresses.
	 */
	switch (req) {

	case PRU_ATTACH:
		if (inp != NULL) {
			error = EINVAL;
			break;
		}
		error = in_pcballoc(so, &udbtable);
		if (error)
			break;
		error = soreserve(so, udp_sendspace, udp_recvspace);
		if (error)
			break;
#ifdef INET6
		if (((struct inpcb *)so->so_pcb)->inp_flags & INP_IPV6)
			((struct inpcb *) so->so_pcb)->inp_ipv6.ip6_hlim =
			    ip6_defhlim;
		else
#endif /* INET6 */
			((struct inpcb *) so->so_pcb)->inp_ip.ip_ttl = (u_int8_t)ip_defttl; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
		break;

	case PRU_DETACH:
		udp_detach(inp);
		break;

	case PRU_BIND:
#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			error = in6_pcbbind(inp, addr, p);
		else
#endif
			error = in_pcbbind(inp, addr, p);
		break;

	case PRU_LISTEN:
		error = EOPNOTSUPP;
		break;

	case PRU_CONNECT:
#ifdef INET6
		if (inp->inp_flags & INP_IPV6) {
			if (!IN6_IS_ADDR_UNSPECIFIED(&inp->inp_faddr6)) {
				error = EISCONN;
				break;
			}
			error = in6_pcbconnect(inp, addr);
		} else
#endif /* INET6 */
		{
			if (inp->inp_faddr.s_addr != INADDR_ANY) {
				error = EISCONN;
				break;
			}
			error = in_pcbconnect(inp, addr);
		}

		if (error == 0)
			soisconnected(so);
		break;

	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;

	case PRU_ACCEPT:
		error = EOPNOTSUPP;
		break;

	case PRU_DISCONNECT:
#ifdef INET6
		if (inp->inp_flags & INP_IPV6) {
			if (IN6_IS_ADDR_UNSPECIFIED(&inp->inp_faddr6)) {
				error = ENOTCONN;
				break;
			}
		} else
#endif /* INET6 */
		{
			if (inp->inp_faddr.s_addr == INADDR_ANY) {
				error = ENOTCONN;
				break;
			}
		}

#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			inp->inp_laddr6 = in6addr_any;
		else
#endif /* INET6 */
			inp->inp_laddr.s_addr = INADDR_ANY;
		in_pcbdisconnect(inp);

		so->so_state &= ~SS_ISCONNECTED;		/* XXX */
		break;

	case PRU_SHUTDOWN:
		socantsendmore(so);
		break;

	case PRU_SEND:
#ifdef PIPEX
		if (inp->inp_pipex) {
			struct pipex_session *session;
#ifdef INET6
			if (inp->inp_flags & INP_IPV6)
				session =
				    pipex_l2tp_userland_lookup_session_ipv6(
					m, inp->inp_faddr6);
			else
#endif
				session =
				    pipex_l2tp_userland_lookup_session_ipv4(
					m, inp->inp_faddr);
			if (session != NULL)
				if ((m = pipex_l2tp_userland_output(
				    m, session)) == NULL) {
					error = ENOMEM;
					goto release;
				}
		}
#endif

#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			return (udp6_output(inp, m, addr, control));
		else
			return (udp_output(m, inp, addr, control));
#else
		return (udp_output(m, inp, addr, control));
#endif

	case PRU_ABORT:
		soisdisconnected(so);
		udp_detach(inp);
		break;

	case PRU_SOCKADDR:
#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			in6_setsockaddr(inp, addr);
		else
#endif /* INET6 */
			in_setsockaddr(inp, addr);
		break;

	case PRU_PEERADDR:
#ifdef INET6
		if (inp->inp_flags & INP_IPV6)
			in6_setpeeraddr(inp, addr);
		else
#endif /* INET6 */
			in_setpeeraddr(inp, addr);
		break;

	case PRU_SENSE:
		/*
		 * stat: don't bother with a blocksize.
		 */
		/*
		 * Perhaps Path MTU might be returned for a connected
		 * UDP socket in this case.
		 */
		return (0);

	case PRU_SENDOOB:
	case PRU_FASTTIMO:
	case PRU_SLOWTIMO:
	case PRU_PROTORCV:
	case PRU_PROTOSEND:
		error =  EOPNOTSUPP;
		break;

	case PRU_RCVD:
	case PRU_RCVOOB:
		return (EOPNOTSUPP);	/* do not free mbuf's */

	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_udp_usrreq_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "udp_usrreq");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_udp_usrreq_001) */
		panic("udp_usrreq");
	}

release:
	if (control) {
		m_freem(control);
	}
	if (m)
		m_freem(m);
	return (error);
}

void
udp_detach(struct inpcb *inp)
{
	in_pcbdetach(inp);
}

/*
 * Sysctl for udp variables.
 */
int
udp_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen)
{
	/* All sysctl names at this level are terminal. */
	if (namelen != 1)
		return (ENOTDIR);

	switch (name[0]) {
	case UDPCTL_BADDYNAMIC:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_udp_usrreq_002) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "udp_sysctl: UDPCTL_BADDYNAMIC");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_udp_usrreq_002) */
		return (sysctl_struct(oldp, oldlenp, newp, newlen,
		    baddynamicports.udp, sizeof(baddynamicports.udp)));

	case UDPCTL_STATS:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_udp_usrreq_003) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "udp_sysctl: UDPCTL_STATS");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_udp_usrreq_003) */
		if (newp != NULL)
			return (EPERM);
		return (sysctl_struct(oldp, oldlenp, newp, newlen,
		    &udpstat, sizeof(udpstat)));

	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_udp_usrreq_004) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "udp_sysctl: default");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_udp_usrreq_004) */
		if (name[0] < UDPCTL_MAXID)
			return (sysctl_int_arr(udpctl_vars, name, namelen,
			    oldp, oldlenp, newp, newlen));
		return (ENOPROTOOPT);
	}
	/* NOTREACHED */
}
