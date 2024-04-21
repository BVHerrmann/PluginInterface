/*	$OpenBSD: ip_icmp.c,v 1.94 2011/07/06 01:57:37 dlg Exp $	*/
/*	$NetBSD: ip_icmp.c,v 1.19 1996/02/13 23:42:22 christos Exp $	*/

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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 *	This product includes software developed at the Information
 *	Technology Division, US Naval Research Laboratory.
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

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_act_module) */
#define LTRC_ACT_MODUL_ID 4023 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include "obsd_kernel_carp.h"
#include "obsd_kernel_pf.h"

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_sysctl.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_route.h>

#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_in_var.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_ip_icmp.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_icmp_var.h>

#if NCARP > 0
#include <net/obsd_kernel_if_types.h>
#include <netinet/obsd_kernel_ip_carp.h>
#endif

#if NPF > 0
#include <net/obsd_kernel_pfvar.h>
#endif

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */
/*
 * ICMP routines: error generation, receive packet processing, and
 * routines to turnaround packets back to the originator, and
 * host table maintenance routines.
 */

int	icmpmaskrepl = 0;
int	icmpbmcastecho = 0;
int	icmptstamprepl = 1;
#ifdef ICMPPRINTFS
int	icmpprintfs = 0;
#endif
int	icmperrppslim = 100;
int	icmperrpps_count = 0;
struct timeval icmperrppslim_last;
int	icmp_rediraccept = 0;
int	icmp_redirtimeout = 10 * 60;
static struct rttimer_queue *icmp_redirect_timeout_q = NULL;
struct	icmpstat icmpstat;

int *icmpctl_vars[ICMPCTL_MAXID] = ICMPCTL_VARS;

void icmp_mtudisc_timeout(struct rtentry *, struct rttimer *);
int icmp_ratelimit(const struct in_addr *, const int, const int);
void icmp_redirect_timeout(struct rtentry *, struct rttimer *);

extern	struct protosw inetsw[];

void
icmp_init(void)
{
	/*
	 * This is only useful if the user initializes redirtimeout to
	 * something other than zero.
	 */
	if (icmp_redirtimeout != 0) {
		icmp_redirect_timeout_q =
		    rt_timer_queue_create(icmp_redirtimeout);
	}
}

void icmp_deinit(void)
{
    if (icmp_redirect_timeout_q != NULL)
    {
        rt_timer_queue_destroy(icmp_redirect_timeout_q , TRUE);
    }
}

struct mbuf *
icmp_do_error(struct mbuf *n, int type, int code, n_long dest, int destmtu)
{
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP  /* gh2289n: TFS RQ1009308 */
	struct ip *oip = mtod(n, struct ip *), *nip;
#else  /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	struct ip *oip = mtod(n, struct ip *);
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	unsigned oiplen = CAL_GET_LOW_NIBBLE_OF_BYTE(oip->ip_v_ip_hl) << 2;
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP  /* gh2289n: TFS RQ1009308 */
	struct icmp *icp;
	struct mbuf *m;
	unsigned icmplen, mblen;
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/

#ifdef ICMPPRINTFS
	if (icmpprintfs)
		printf("icmp_error(%x, %d, %d)\n", oip, type, code);
#endif
	if (type != ICMP_REDIRECT)
		icmpstat.icps_error++;
	/*
	 * Don't send error if not the first fragment of message.
	 * Don't error if the old packet protocol was ICMP
	 * error message, only known informational types.
	 */
	if (oip->ip_off & htons(IP_OFFMASK))
		goto freeit;
	if (oip->ip_p == IPPROTO_ICMP && type != ICMP_REDIRECT &&
	    n->m_len >= oiplen + ICMP_MINLEN &&
	    !ICMP_INFOTYPE(((struct icmp *)
	    ((caddr_t)oip + oiplen))->icmp_type)) {
		icmpstat.icps_oldicmp++;
		goto freeit;
	}

#if       OBSD_PNIO_ICMP_DO_FULL_ICMP  /* gh2289n: TFS RQ1009308 */
	/* Don't send error in response to a multicast or broadcast packet */
	if (n->m_flags & (M_BCAST|M_MCAST))
		goto freeit;

	/*
	 * First, do a rate limitation check.
	 */
	if (icmp_ratelimit(&oip->ip_src, type, code))
		goto freeit;	/* XXX stat */

	/*
	 * Now, formulate icmp message
	 */
	icmplen = oiplen + min(8, ntohs(oip->ip_len));
	/*
	 * Defend against mbuf chains shorter than oip->ip_len:
	 */
	mblen = 0;
	for (m = n; m && (mblen < icmplen); m = m->m_next)
		mblen += m->m_len;
	icmplen = min(mblen, icmplen);

	/*
	 * As we are not required to return everything we have,
	 * we return whatever we can return at ease.
	 *
	 * Note that ICMP datagrams longer than 576 octets are out of spec
	 * according to RFC1812;
	 */

	KASSERT(ICMP_MINLEN <= MCLBYTES);

	if (icmplen + ICMP_MINLEN > MCLBYTES)
		icmplen = MCLBYTES - ICMP_MINLEN - sizeof (struct ip);

	m = m_gethdr(M_DONTWAIT, MT_HEADER);
	if (m && (sizeof (struct ip) + icmplen + ICMP_MINLEN > MHLEN)) {
		MCLGET(m, M_DONTWAIT);
		if ((m->m_flags & M_EXT) == 0) {
			m_freem(m);
			m = NULL;
		}
	}
	if (m == NULL)
		goto freeit;
	/* keep in same domain and rtable (the latter is a bit unclear) */
	m->m_pkthdr.rdomain = n->m_pkthdr.rdomain;
	m->m_len = icmplen + ICMP_MINLEN;
	if ((m->m_flags & M_EXT) == 0)
		MH_ALIGN(m, m->m_len);
	icp = mtod(m, struct icmp *);
	if ((u_int)type > ICMP_MAXTYPE)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "icmp_error");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_001) */
		panic("icmp_error");
	}
	icmpstat.icps_outhist[type]++;
	icp->icmp_type = (u_int8_t)type; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	if (type == ICMP_REDIRECT)
		icp->icmp_gwaddr.s_addr = dest;
	else {
		icp->icmp_void = 0;
		/*
		 * The following assignments assume an overlay with the
		 * zeroed icmp_void field.
		 */
		if (type == ICMP_PARAMPROB) {
			icp->icmp_pptr = (u_int8_t) code;  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			code = 0;
		} else if (type == ICMP_UNREACH &&
		    code == ICMP_UNREACH_NEEDFRAG && destmtu)
			icp->icmp_nextmtu = htons(destmtu);
	}

	icp->icmp_code = (u_int8_t)code; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	m_copydata(n, 0, icmplen, (caddr_t)&icp->icmp_ip);

	/*
	 * Now, copy old ip header (without options)
	 * in front of icmp message.
	 */
	if ((m->m_flags & M_EXT) == 0 &&
	    m->m_data - sizeof(struct ip) < m->m_pktdat)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_002) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "icmp_len");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_002) */
		panic("icmp len");
	}
	m->m_data -= sizeof(struct ip);
	m->m_len += sizeof(struct ip);
	m->m_pkthdr.len = m->m_len;
	m->m_pkthdr.rcvif = n->m_pkthdr.rcvif;
	nip = mtod(m, struct ip *);
	/* ip_v set in ip_output */
	CAL_SET_LOW_NIBBLE_OF_BYTE(nip->ip_v_ip_hl, sizeof(struct ip) >> 2);
	nip->ip_tos = 0;
	nip->ip_len = htons(m->m_len);
	/* ip_id set in ip_output */
	nip->ip_off = 0;
	/* ip_ttl set in icmp_reflect */
	nip->ip_p = IPPROTO_ICMP;
	nip->ip_src = oip->ip_src;
	nip->ip_dst = oip->ip_dst;

	/* move PF_GENERATED to new packet, if existent XXX preserve more? */
	if (n->m_pkthdr.pf.flags & PF_TAG_GENERATED)
		m->m_pkthdr.pf.flags |= PF_TAG_GENERATED;

	m_freem(n);
	return (m);
#else  /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	OBSD_UNUSED_ARG(code);      /* avoid compiler warning */
	OBSD_UNUSED_ARG(dest);
	OBSD_UNUSED_ARG(destmtu);
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/

freeit:
	m_freem(n);
	return (NULL);
}

/*
 * Generate an error packet of type error
 * in response to bad packet ip.
 *
 * The ip packet inside has ip_off and ip_len in host byte order.
 */
void
icmp_error(struct mbuf *n, int type, int code, n_long dest, int destmtu)
{
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308 */
	struct mbuf *m;

	m = icmp_do_error(n, type, code, dest, destmtu);
	if (m != NULL)
		if (!icmp_reflect(m, NULL, NULL))
			icmp_send(m, NULL);
#else /* sado -- Greenhills compiler warning 550 - "m" set but never used */
	icmp_do_error(n, type, code, dest, destmtu); 
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
}

struct sockaddr_in icmpsrc = { sizeof (struct sockaddr_in), AF_INET };
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
static struct sockaddr_in icmpdst = { sizeof (struct sockaddr_in), AF_INET };
static struct sockaddr_in icmpgw = { sizeof (struct sockaddr_in), AF_INET };
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/

/*
 * Process a received ICMP message.
 */
void
icmp_input(struct mbuf *m, ...)
{
	struct icmp *icp;
	struct ip *ip = mtod(m, struct ip *);
	int icmplen;
	unsigned int i; /* gerlach/gh2289n: changed type from int to unsigned int to avoid compiler warning C4018 */
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	struct in_ifaddr *ia;
	void *(*ctlfunc)(int, struct sockaddr *, u_int, void *);
	int code; /* sado -- Greenhills compiler warning 550 - set but never used */
	extern u_char ip_protox[];
	extern int ipforwarding;
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	int hlen;
	va_list ap;
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	struct rtentry *rt;
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	struct mbuf *opts;

	va_start(ap, m);
	hlen = va_arg(ap, int);
	va_end(ap);

	/*
	 * Locate icmp structure in mbuf, and check
	 * that not corrupted and of at least minimum length.
	 */
	icmplen = ntohs(ip->ip_len) - hlen;
#ifdef ICMPPRINTFS
	if (icmpprintfs) {
		char buf[4 * sizeof("123")];

		strlcpy(buf, inet_ntoa(ip->ip_dst), sizeof buf);
		printf("icmp_input from %s to %s, len %d\n",
		    inet_ntoa(ip->ip_src), buf, icmplen);
	}
#endif
	if (icmplen < ICMP_MINLEN) {
		icmpstat.icps_tooshort++;
		goto freeit;
	}
	i = hlen + min(icmplen, ICMP_ADVLENMIN);
	if (m->m_len < i && (m = m_pullup(m, i)) == NULL) {
		icmpstat.icps_tooshort++;
		return;
	}
	ip = mtod(m, struct ip *);
	m->m_len -= hlen;
	m->m_data += hlen;
	icp = mtod(m, struct icmp *);
	if (in_cksum(m, icmplen)) {
		icmpstat.icps_checksum++;
		goto freeit;
	}
	m->m_len += hlen;
	m->m_data -= hlen;

#ifdef ICMPPRINTFS
	/*
	 * Message type specific processing.
	 */
	if (icmpprintfs)
		printf("icmp_input, type %d code %d\n", icp->icmp_type,
		    icp->icmp_code);
#endif
#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	if (icp->icmp_type > ICMP_MAXTYPE)
		goto raw;

	code = icp->icmp_code; /* sado -- Greenhills compiler warning 550 - set but never used */
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	icmpstat.icps_inhist[icp->icmp_type]++;
	switch (icp->icmp_type) {

#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	case ICMP_UNREACH:
		switch (code) {
		case ICMP_UNREACH_NET:
		case ICMP_UNREACH_HOST:
		case ICMP_UNREACH_PROTOCOL:
		case ICMP_UNREACH_PORT:
		case ICMP_UNREACH_SRCFAIL:
			code += PRC_UNREACH_NET;
			break;

		case ICMP_UNREACH_NEEDFRAG:
			code = PRC_MSGSIZE;
			break;

		case ICMP_UNREACH_NET_UNKNOWN:
		case ICMP_UNREACH_NET_PROHIB:
		case ICMP_UNREACH_TOSNET:
			code = PRC_UNREACH_NET;
			break;

		case ICMP_UNREACH_HOST_UNKNOWN:
		case ICMP_UNREACH_ISOLATED:
		case ICMP_UNREACH_HOST_PROHIB:
		case ICMP_UNREACH_TOSHOST:
		case ICMP_UNREACH_FILTER_PROHIB:
		case ICMP_UNREACH_HOST_PRECEDENCE:
		case ICMP_UNREACH_PRECEDENCE_CUTOFF:
			code = PRC_UNREACH_HOST;
			break;

		default:
			goto badcode;
		}
		goto deliver;

	case ICMP_TIMXCEED:
		if (code > 1)
			goto badcode;
		code += PRC_TIMXCEED_INTRANS;
		goto deliver;

	case ICMP_PARAMPROB:
		if (code > 1)
			goto badcode;
		code = PRC_PARAMPROB;
		goto deliver;

	case ICMP_SOURCEQUENCH:
		if (code)
			goto badcode;
		code = PRC_QUENCH;
	deliver:
		/* Free packet atttributes */
		if (m->m_flags & M_PKTHDR)
			m_tag_delete_chain(m);

		/*
		 * Problem with datagram; advise higher level routines.
		 */
		if (icmplen < ICMP_ADVLENMIN || icmplen < ICMP_ADVLEN(icp) ||
		    CAL_GET_LOW_NIBBLE_OF_BYTE(icp->icmp_ip.ip_v_ip_hl)< (sizeof(struct ip) >> 2)) {
			icmpstat.icps_badlen++;
			goto freeit;
		}
		if (IN_MULTICAST(icp->icmp_ip.ip_dst.s_addr))
			goto badcode;
#ifdef INET6
		/* Get more contiguous data for a v6 in v4 ICMP message. */
		if (icp->icmp_ip.ip_p == IPPROTO_IPV6) {
			if (icmplen < ICMP_V6ADVLENMIN ||
			    icmplen < ICMP_V6ADVLEN(icp)) {
				icmpstat.icps_badlen++;
				goto freeit;
			} else {
				if ((m = m_pullup(m, (ip->ip_hl << 2) +
				    ICMP_V6ADVLEN(icp))) == NULL) {
					icmpstat.icps_tooshort++;
					return;
				}
				ip = mtod(m, struct ip *);
				icp = (struct icmp *)
				    (m->m_data + (ip->ip_hl << 2));
			}
		}
#endif /* INET6 */
#ifdef ICMPPRINTFS
		if (icmpprintfs)
			printf("deliver to protocol %d\n", icp->icmp_ip.ip_p);
#endif
		icmpsrc.sin_addr = icp->icmp_ip.ip_dst;
#if NCARP > 0
		if (m->m_pkthdr.rcvif->if_type == IFT_CARP &&
		    carp_lsdrop(m, AF_INET, &icmpsrc.sin_addr.s_addr,
		    &ip->ip_dst.s_addr))
			goto freeit;
#endif
		/*
		 * XXX if the packet contains [IPv4 AH TCP], we can't make a
		 * notification to TCP layer.
		 */
		ctlfunc = inetsw[ip_protox[icp->icmp_ip.ip_p]].pr_ctlinput;
		if (ctlfunc)
			(*ctlfunc)(code, sintosa(&icmpsrc), m->m_pkthdr.rdomain,
			    &icp->icmp_ip);
		break;

	badcode:
		icmpstat.icps_badcode++;
		break;
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/

	case ICMP_ECHO:
		if (!icmpbmcastecho &&
		    (m->m_flags & (M_MCAST | M_BCAST)) != 0) {
			icmpstat.icps_bmcastecho++;
			break;
		}
		icp->icmp_type = ICMP_ECHOREPLY;
		goto reflect;

#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	case ICMP_TSTAMP:
		if (icmptstamprepl == 0)
			break;

		if (!icmpbmcastecho &&
		    (m->m_flags & (M_MCAST | M_BCAST)) != 0) {
			icmpstat.icps_bmcastecho++;
			break;
		}
		if (icmplen < ICMP_TSLEN) {
			icmpstat.icps_badlen++;
			break;
		}
		icp->icmp_type = ICMP_TSTAMPREPLY;
		icp->icmp_rtime = iptime();
		icp->icmp_ttime = icp->icmp_rtime;	/* bogus, do later! */
		goto reflect;

	case ICMP_MASKREQ:
		if (icmpmaskrepl == 0)
			break;
		if (icmplen < ICMP_MASKLEN) {
			icmpstat.icps_badlen++;
			break;
		}
		/*
		 * We are not able to respond with all ones broadcast
		 * unless we receive it over a point-to-point interface.
		 */
		if (ip->ip_dst.s_addr == INADDR_BROADCAST ||
		    ip->ip_dst.s_addr == INADDR_ANY)
			icmpdst.sin_addr = ip->ip_src;
		else
			icmpdst.sin_addr = ip->ip_dst;
		if (m->m_pkthdr.rcvif == NULL)
			break;
		ia = ifatoia(ifaof_ifpforaddr(sintosa(&icmpdst),
		    m->m_pkthdr.rcvif));
		if (ia == 0)
			break;
		icp->icmp_type = ICMP_MASKREPLY;
		icp->icmp_mask = ia->ia_sockmask.sin_addr.s_addr;
		if (ip->ip_src.s_addr == 0) {
			if (ia->ia_ifp->if_flags & IFF_BROADCAST) {
				if (ia->ia_broadaddr.sin_addr.s_addr)
					ip->ip_src = ia->ia_broadaddr.sin_addr;
				else
					ip->ip_src.s_addr = INADDR_BROADCAST;
			}
			else if (ia->ia_ifp->if_flags & IFF_POINTOPOINT)
				ip->ip_src = ia->ia_dstaddr.sin_addr;
		}
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
reflect:
#if NCARP > 0
#if 0 /* sado -- Greenhills compiler warning 1545 -- address taken of a packed structure member with insufficient alignment */
		if (m->m_pkthdr.rcvif->if_type == IFT_CARP &&
		    carp_lsdrop(m, AF_INET, &ip->ip_src.s_addr,
		    &ip->ip_dst.s_addr))
			goto freeit;
#else
		{
			u_int32_t src_addr = ip->ip_src.s_addr;
			u_int32_t dst_addr = ip->ip_dst.s_addr;
			
			if (m->m_pkthdr.rcvif->if_type == IFT_CARP && 
				carp_lsdrop(m, AF_INET, &src_addr, 
				&dst_addr))
				goto freeit;
		}
#endif
#endif
		/* Free packet atttributes */
		if (m->m_flags & M_PKTHDR)
			m_tag_delete_chain(m);

		icmpstat.icps_reflect++;
		icmpstat.icps_outhist[icp->icmp_type]++;
		/* if (!icmp_reflect(m, &opts, NULL))
			icmp_send(m, opts);					*/
		if (!icmp_reflect(m, &opts, NULL)) {	/* RQ 2252335 - [OBSD] No more memory after 4h test */
			icmp_send(m, opts);
			if (opts)							/* may be icmp_reflect() allocates a opt mbuf */
			{
				m_freem(opts);					/* then free it */
				opts = NULL;
			}
		}
		return;

#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
	case ICMP_REDIRECT:
		/* Free packet atttributes */
		if (m->m_flags & M_PKTHDR)
			m_tag_delete_chain(m);
		if (icmp_rediraccept == 0 || ipforwarding == 1)
			goto freeit;
		if (code > 3)
			goto badcode;
		if (icmplen < ICMP_ADVLENMIN || icmplen < ICMP_ADVLEN(icp) ||
            CAL_GET_LOW_NIBBLE_OF_BYTE(icp->icmp_ip.ip_v_ip_hl)< (sizeof(struct ip) >> 2)) {
			icmpstat.icps_badlen++;
			break;
		}
		/*
		 * Short circuit routing redirects to force
		 * immediate change in the kernel's routing
		 * tables.  The message is also handed to anyone
		 * listening on a raw socket (e.g. the routing
		 * daemon for use in updating its tables).
		 */
		icmpgw.sin_addr = ip->ip_src;
		icmpdst.sin_addr = icp->icmp_gwaddr;
#ifdef	ICMPPRINTFS
		if (icmpprintfs) {
			char buf[4 * sizeof("123")];
			strlcpy(buf, inet_ntoa(icp->icmp_ip.ip_dst),
			    sizeof buf);

			printf("redirect dst %s to %s\n",
			    buf, inet_ntoa(icp->icmp_gwaddr));
		}
#endif
		icmpsrc.sin_addr = icp->icmp_ip.ip_dst;
#if NCARP > 0
		if (m->m_pkthdr.rcvif->if_type == IFT_CARP &&
		    carp_lsdrop(m, AF_INET, &icmpsrc.sin_addr.s_addr,
		    &ip->ip_dst.s_addr))
			goto freeit;
#endif
		rt = NULL;
		rtredirect(sintosa(&icmpsrc), sintosa(&icmpdst),
		    (struct sockaddr *)0, RTF_GATEWAY | RTF_HOST,
		    sintosa(&icmpgw), (struct rtentry **)&rt,
		    m->m_pkthdr.rdomain);
		if (rt != NULL && icmp_redirtimeout != 0) {
			(void)rt_timer_add(rt, icmp_redirect_timeout,
			    icmp_redirect_timeout_q, m->m_pkthdr.rdomain);
		}
		if (rt != NULL)
			rtfree(rt);
		pfctlinput(PRC_REDIRECT_HOST, sintosa(&icmpsrc));
		break;

	/*
	 * No kernel processing for the following;
	 * just fall through to send to raw listener.
	 */
	case ICMP_ECHOREPLY:
	case ICMP_ROUTERADVERT:
	case ICMP_ROUTERSOLICIT:
	case ICMP_TSTAMPREPLY:
	case ICMP_IREQREPLY:
	case ICMP_MASKREPLY:
	case ICMP_TRACEROUTE:
	case ICMP_DATACONVERR:
	case ICMP_MOBILE_REDIRECT:
	case ICMP_IPV6_WHEREAREYOU:
	case ICMP_IPV6_IAMHERE:
	case ICMP_MOBILE_REGREQUEST:
	case ICMP_MOBILE_REGREPLY:
	case ICMP_PHOTURIS:
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/
	default:
		break;
	}

#if       OBSD_PNIO_ICMP_DO_FULL_ICMP /* gh2289n: TFS RQ1009308, not really needed to be removed, but save code */
raw:
	rip_input(m);
	return;
#endif /* OBSD_PNIO_ICMP_DO_FULL_ICMP*/

freeit:
	m_freem(m);
}

/*
 * Reflect the ip packet back to the source
 */
int
icmp_reflect(struct mbuf *m, struct mbuf **op, struct in_ifaddr *ia)
{
	struct ip *ip = mtod(m, struct ip *);
	struct in_addr t;
	struct mbuf *opts = 0;
	int optlen = (CAL_GET_LOW_NIBBLE_OF_BYTE(ip->ip_v_ip_hl) << 2) - sizeof(struct ip);

	if (!in_canforward(ip->ip_src) &&
	    ((ip->ip_src.s_addr & IN_CLASSA_NET) !=
	    htonl(IN_LOOPBACKNET << IN_CLASSA_NSHIFT))) {
		m_freem(m);		/* Bad return address */
		return (EHOSTUNREACH);
	}

#if NPF > 0
	pf_pkt_addr_changed(m);
#endif
	t = ip->ip_dst;
	ip->ip_dst = ip->ip_src;
	/*
	 * If the incoming packet was addressed directly to us,
	 * use dst as the src for the reply.  For broadcast, use
	 * the address which corresponds to the incoming interface.
	 */
	if (ia == NULL) {
		TAILQ_FOREACH(ia, &in_ifaddr, ia_list) {
			if (ia->ia_ifp->if_rdomain !=
			    rtable_l2(m->m_pkthdr.rdomain))
				continue;
			if (t.s_addr == ia->ia_addr.sin_addr.s_addr)
				break;
			if ((ia->ia_ifp->if_flags & IFF_BROADCAST) &&
			    ia->ia_broadaddr.sin_addr.s_addr != 0 &&
			    t.s_addr == ia->ia_broadaddr.sin_addr.s_addr)
				break;
		}
	}
	/*
	 * The following happens if the packet was not addressed to us.
	 * Use the new source address and do a route lookup. If it fails
	 * drop the packet as there is no path to the host.
	 */
	if (ia == NULL) {
		struct sockaddr_in *dst;
		struct route ro;

		bzero((caddr_t) &ro, sizeof(ro));
		dst = satosin(&ro.ro_dst);
		dst->sin_family = AF_INET;
		dst->sin_len = sizeof(*dst);
		dst->sin_addr = ip->ip_src;

		/* keep packet in the original virtual instance */
		ro.ro_rt = rtalloc1(&ro.ro_dst, RT_REPORT,
		     m->m_pkthdr.rdomain);
		if (ro.ro_rt == 0) {
			ipstat.ips_noroute++;
			m_freem(m);
			return (EHOSTUNREACH);
		}

		ia = ifatoia(ro.ro_rt->rt_ifa);
		ro.ro_rt->rt_use++;
		RTFREE(ro.ro_rt);
	}

	t = ia->ia_addr.sin_addr;
	ip->ip_src = t;
	ip->ip_ttl = MAXTTL;

	if (optlen > 0) {
		u_char *cp;
		int opt, cnt;
		u_int len;

		/*
		 * Retrieve any source routing from the incoming packet;
		 * add on any record-route or timestamp options.
		 */
		cp = (u_char *) (ip + 1);
		if (op && (opts = ip_srcroute()) == 0 &&
		    ((opts = m_gethdr(M_DONTWAIT, MT_HEADER)) != NULL))  {    /* gh2289n: added NULL comparison to avoid PN compiler error */
			opts->m_len = sizeof(struct in_addr);
			mtod(opts, struct in_addr *)->s_addr = 0;
		}
		if (op && opts) {
#ifdef ICMPPRINTFS
			if (icmpprintfs)
				printf("icmp_reflect optlen %d rt %d => ",
				    optlen, opts->m_len);
#endif
			for (cnt = optlen; cnt > 0; cnt -= len, cp += len) {
				opt = cp[IPOPT_OPTVAL];
				if (opt == IPOPT_EOL)
					break;
				if (opt == IPOPT_NOP)
					len = 1;
				else {
					if (cnt < IPOPT_OLEN + sizeof(*cp))
						break;
					len = cp[IPOPT_OLEN];
					if (len < IPOPT_OLEN + sizeof(*cp) ||
					    len > (u_int)cnt) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
						break;
				}
				/*
				 * Should check for overflow, but it
				 * "can't happen"
				 */
				if (opt == IPOPT_RR || opt == IPOPT_TS ||
				    opt == IPOPT_SECURITY) {
					bcopy((caddr_t)cp,
					    mtod(opts, caddr_t) + opts->m_len,
					    len);
					opts->m_len += len;
				}
			}
			/* Terminate & pad, if necessary */
			if ((cnt = opts->m_len % 4) != 0)
				for (; cnt < 4; cnt++) {
					*(mtod(opts, caddr_t) + opts->m_len) =
					    IPOPT_EOL;
					opts->m_len++;
				}
#ifdef ICMPPRINTFS
			if (icmpprintfs)
				printf("%d\n", opts->m_len);
#endif
		}
		/*
		 * Now strip out original options by copying rest of first
		 * mbuf's data back, and adjust the IP length.
		 */
		ip->ip_len = htons(ntohs(ip->ip_len) - optlen);
		CAL_SET_LOW_NIBBLE_OF_BYTE(ip->ip_v_ip_hl, sizeof(struct ip) >> 2);
		m->m_len -= optlen;
		if (m->m_flags & M_PKTHDR)
			m->m_pkthdr.len -= optlen;
		optlen += sizeof(struct ip);
		bcopy((caddr_t)ip + optlen, (caddr_t)(ip + 1),
		    (unsigned)(m->m_len - sizeof(struct ip)));
	}
	m->m_flags &= ~(M_BCAST|M_MCAST);
	if (op)
		*op = opts;

	return (0);
}

/*
 * Send an icmp packet back to the ip level,
 * after supplying a checksum.
 */
void
icmp_send(struct mbuf *m, struct mbuf *opts)
{
	struct ip *ip = mtod(m, struct ip *);
	int hlen;
	struct icmp *icp;

	hlen = CAL_GET_LOW_NIBBLE_OF_BYTE(ip->ip_v_ip_hl) << 2;
	m->m_data += hlen;
	m->m_len -= hlen;
	icp = mtod(m, struct icmp *);
	icp->icmp_cksum = 0;
	icp->icmp_cksum = (u_int16_t)in_cksum(m, ntohs(ip->ip_len) - hlen); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	m->m_data -= hlen;
	m->m_len += hlen;
#ifdef ICMPPRINTFS
	if (icmpprintfs) {
		char buf[4 * sizeof("123")];

		strlcpy(buf, inet_ntoa(ip->ip_dst), sizeof buf);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_003) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "icmp_send");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_003) */
		printf("icmp_send dst %s src %s\n",
		    buf, inet_ntoa(ip->ip_src));
	}
#endif
	(void)ip_output(m, opts, (void *)NULL, 0, (void *)NULL, (void *)NULL);
}

n_time
iptime(void)
{
	struct timeval atv;
	u_long t;

	microtime(&atv);
	t = (atv.tv_sec % (24*60*60)) * 1000 + atv.tv_usec / 1000;
	return (htonl(t));
}

int
icmp_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen)
{

	/* All sysctl names at this level are terminal. */
	if (namelen != 1)
		return (ENOTDIR);

	switch (name[0]) {
	case ICMPCTL_REDIRTIMEOUT: {
		int error;

		error = sysctl_int(oldp, oldlenp, newp, newlen,
		    &icmp_redirtimeout);
		if (icmp_redirect_timeout_q != NULL) {
			if (icmp_redirtimeout == 0) {
				rt_timer_queue_destroy(icmp_redirect_timeout_q,
				    TRUE);
				icmp_redirect_timeout_q = NULL;
			} else
				rt_timer_queue_change(icmp_redirect_timeout_q,
				    icmp_redirtimeout);
		} else if (icmp_redirtimeout > 0) {
			icmp_redirect_timeout_q =
			    rt_timer_queue_create(icmp_redirtimeout);
		}
		return (error);
		/* sado -- omit break; Greenhills compiler warning 111 - statement unreachable */
	}
	case ICMPCTL_STATS:
		if (newp != NULL)
			return (EPERM);
		return (sysctl_struct(oldp, oldlenp, newp, newlen,
		    &icmpstat, sizeof(icmpstat)));
	default:
		if (name[0] < ICMPCTL_MAXID)
			return (sysctl_int_arr(icmpctl_vars, name, namelen,
			    oldp, oldlenp, newp, newlen));
		return (ENOPROTOOPT);
	}
	/* NOTREACHED */
}


struct rtentry *
icmp_mtudisc_clone(struct sockaddr *dst, u_int rtableid)
{
	struct rtentry *rt;
	int error;

	rt = rtalloc1(dst, RT_REPORT, rtableid);
	if (rt == 0)
		return (NULL);

	/* Check if the route is actually usable */
	if (rt->rt_flags & (RTF_REJECT | RTF_BLACKHOLE) ||
	    (rt->rt_flags & RTF_UP) == 0)
		return (NULL);

	/* If we didn't get a host route, allocate one */

	if ((rt->rt_flags & RTF_HOST) == 0) {
		struct rtentry *nrt;
		struct rt_addrinfo info;

		bzero(&info, sizeof(info));
		info.rti_info[RTAX_DST] = dst;
		info.rti_info[RTAX_GATEWAY] = rt->rt_gateway;
		info.rti_flags = RTF_GATEWAY | RTF_HOST | RTF_DYNAMIC;

		error = rtrequest1(RTM_ADD, &info, RTP_DEFAULT, &nrt, rtableid);
		if (error) {
			rtfree(rt);
			return (NULL);
		}
		nrt->rt_rmx = rt->rt_rmx;
		rtfree(rt);
		rt = nrt;
	}
	error = rt_timer_add(rt, icmp_mtudisc_timeout, ip_mtudisc_timeout_q,
	    rtableid);
	if (error) {
		rtfree(rt);
		return (NULL);
	}

	return (rt);
}

void
icmp_mtudisc(struct icmp *icp, u_int rtableid)
{
	struct rtentry *rt;
	struct sockaddr *dst = sintosa(&icmpsrc);
	u_long mtu = ntohs(icp->icmp_nextmtu);  /* Why a long?  IPv6 */

	/* Table of common MTUs: */

	static u_short mtu_table[] = {
		65535, 65280, 32000, 17914, 9180, 8166,
		4352, 2002, 1492, 1006, 508, 296, 68, 0
	};

	rt = icmp_mtudisc_clone(dst, rtableid);
	if (rt == 0)
		return;

	if (mtu == 0) {
		int i = 0;

		mtu = ntohs(icp->icmp_ip.ip_len);
		/* Some 4.2BSD-based routers incorrectly adjust the ip_len */
		if (mtu > rt->rt_rmx.rmx_mtu && rt->rt_rmx.rmx_mtu != 0)
			mtu -= (CAL_GET_LOW_NIBBLE_OF_BYTE(icp->icmp_ip.ip_v_ip_hl) << 2);

		/* If we still can't guess a value, try the route */

		if (mtu == 0) {
			mtu = rt->rt_rmx.rmx_mtu;

			/* If no route mtu, default to the interface mtu */

			if (mtu == 0)
				mtu = rt->rt_ifp->if_mtu;
		}

		for (i = 0; i < sizeof(mtu_table) / sizeof(mtu_table[0]); i++)
			if (mtu > mtu_table[i]) {
				mtu = mtu_table[i];
				break;
			}
	}

	/*
	 * XXX:   RTV_MTU is overloaded, since the admin can set it
	 *	  to turn off PMTU for a route, and the kernel can
	 *	  set it to indicate a serious problem with PMTU
	 *	  on a route.  We should be using a separate flag
	 *	  for the kernel to indicate this.
	 */

	if ((rt->rt_rmx.rmx_locks & RTV_MTU) == 0) {
		if (mtu < 296 || mtu > rt->rt_ifp->if_mtu)
			rt->rt_rmx.rmx_locks |= RTV_MTU;
		else if (rt->rt_rmx.rmx_mtu > mtu ||
		    rt->rt_rmx.rmx_mtu == 0)
			rt->rt_rmx.rmx_mtu = mtu;
	}

	rtfree(rt);
}

void
icmp_mtudisc_timeout(struct rtentry *rt, struct rttimer *r)
{
	if (rt == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_004) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "icmp_mtudisc_timeout:  bad route to timeout");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_004) */
		panic("icmp_mtudisc_timeout:  bad route to timeout");
	}
	if ((rt->rt_flags & (RTF_DYNAMIC | RTF_HOST)) ==
	    (RTF_DYNAMIC | RTF_HOST)) {
		void *(*ctlfunc)(int, struct sockaddr *, u_int, void *);
		extern u_char ip_protox[];
		struct sockaddr_in sa;
		struct rt_addrinfo info;

		bzero(&info, sizeof(info));
		info.rti_info[RTAX_DST] = rt_key(rt);
		info.rti_info[RTAX_NETMASK] = rt_mask(rt);
		info.rti_info[RTAX_GATEWAY] = rt->rt_gateway;
		info.rti_flags = rt->rt_flags;   

		sa = *(struct sockaddr_in *)rt_key(rt);
		rtrequest1(RTM_DELETE, &info, rt->rt_priority, NULL,
		    r->rtt_tableid);

		/* Notify TCP layer of increased Path MTU estimate */
		ctlfunc = inetsw[ip_protox[IPPROTO_TCP]].pr_ctlinput;
		if (ctlfunc)
			(*ctlfunc)(PRC_MTUINC,(struct sockaddr *)&sa,
			    r->rtt_tableid, NULL);
	} else
		if ((rt->rt_rmx.rmx_locks & RTV_MTU) == 0)
			rt->rt_rmx.rmx_mtu = 0;
}

/*
 * Perform rate limit check.
 * Returns 0 if it is okay to send the icmp packet.
 * Returns 1 if the router SHOULD NOT send this icmp packet due to rate
 * limitation.
 *
 * XXX per-destination/type check necessary?
 */
int
icmp_ratelimit(const struct in_addr *dst, const int type, const int code)
{
	OBSD_UNUSED_ARG(dst); 
	OBSD_UNUSED_ARG(type); 
	OBSD_UNUSED_ARG(code); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* PPS limit */
	if (!ppsratecheck(&icmperrppslim_last, &icmperrpps_count,
	    icmperrppslim))
		return 1;

	/*okay to send*/
	return 0;
}

void
icmp_redirect_timeout(struct rtentry *rt, struct rttimer *r)
{
	if (rt == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_005) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "icmp_redirect_timeout:  bad route to timeout");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_005) */
		panic("icmp_redirect_timeout:  bad route to timeout");
	}
	if ((rt->rt_flags & (RTF_DYNAMIC | RTF_HOST)) ==
	    (RTF_DYNAMIC | RTF_HOST)) {
		struct rt_addrinfo info;

		bzero(&info, sizeof(info));
		info.rti_info[RTAX_DST] = rt_key(rt);
		info.rti_info[RTAX_NETMASK] = rt_mask(rt);
		info.rti_info[RTAX_GATEWAY] = rt->rt_gateway;
		info.rti_flags = rt->rt_flags;   

		rtrequest1(RTM_DELETE, &info, rt->rt_priority, NULL, 
		    r->rtt_tableid);
	}
}

int
icmp_do_exthdr(struct mbuf *m, u_int16_t class, u_int8_t ctype, void *buf,
    size_t len)
{
	struct ip *ip = mtod(m, struct ip *);
	int hlen, off;
	struct mbuf *n;
	struct icmp *icp;
	struct icmp_ext_hdr *ieh;
	struct {
		struct icmp_ext_hdr	ieh;
		struct icmp_ext_obj_hdr	ieo;
	} hdr;

	hlen = CAL_GET_LOW_NIBBLE_OF_BYTE(ip->ip_v_ip_hl) << 2;
	icp = (struct icmp *)(mtod(m, caddr_t) + hlen);
	if (icp->icmp_type != ICMP_TIMXCEED && icp->icmp_type != ICMP_UNREACH &&
	    icp->icmp_type != ICMP_PARAMPROB)
		/* exthdr not supported */
		return (0);
	
	if (icp->icmp_length != 0)
		/* exthdr already present, giving up */
		return (0);

	/* the actuall offset starts after the common ICMP header */
	hlen += ICMP_MINLEN;
	/* exthdr must start on a word boundary */
	off = roundup(ntohs(ip->ip_len) - hlen, sizeof(u_int32_t));
	/* ... and at an offset of ICMP_EXT_OFFSET or bigger */
	off = max(off, ICMP_EXT_OFFSET);
	icp->icmp_length = (u_int8_t)(off / sizeof(u_int32_t)); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */

	bzero(&hdr, sizeof(hdr));
	hdr.ieh.ieh_version = ICMP_EXT_HDR_VERSION;
	hdr.ieo.ieo_length = htons(sizeof(struct icmp_ext_obj_hdr) + len);
	hdr.ieo.ieo_cnum = (u_int8_t)class; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	hdr.ieo.ieo_ctype = ctype;

	if (m_copyback(m, hlen + off, sizeof(hdr), &hdr, M_NOWAIT) ||
	    m_copyback(m, hlen + off + sizeof(hdr), (int)len, buf, M_NOWAIT)) {  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
		m_freem(m);
		return (ENOBUFS);
	}

	/* calculate checksum */
	n = m_getptr(m, hlen + off, &off);
	if (n == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_ip_icmp_006) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "icmp_do_exthdr: m_getptr failure");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_ip_icmp_006) */
		panic("icmp_do_exthdr: m_getptr failure");
	}
	/* this is disgusting, in_cksum() is stupid */
	n->m_data += off;
	n->m_len -= off;
	ieh = mtod(n, struct icmp_ext_hdr *);
	ieh->ieh_cksum = (u_int16_t)in_cksum(n, sizeof(hdr) + (int)len); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	n->m_data -= off;
	n->m_len += off;

	ip->ip_len = htons(m->m_pkthdr.len);

	return (0);
}
