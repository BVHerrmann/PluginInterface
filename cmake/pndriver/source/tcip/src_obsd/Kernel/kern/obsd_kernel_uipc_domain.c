/*	$OpenBSD: uipc_domain.c,v 1.32 2011/07/09 00:47:18 henning Exp $	*/
/*	$NetBSD: uipc_domain.c,v 1.14 1996/02/09 19:00:44 christos Exp $	*/

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
 *	@(#)uipc_domain.c	8.2 (Berkeley) 10/18/93
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
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_domain.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_time.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_timeout.h>

#include "obsd_kernel_bluetooth.h"
#include "obsd_kernel_bpfilter.h"
#include "obsd_kernel_pflow.h"

/* includes needed for the (additional) deinitialization code below     */
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_udp.h>
#include <netinet/obsd_kernel_tcp_timer.h>
#include <netinet/obsd_kernel_tcp.h>

#include <net/obsd_kernel_route.h>                      /* route_init(void);       route.c        */
#include <netinet/obsd_kernel_ip_var.h>                 /* ip_init(void);          ip_input.c     */
                                                        /* rip_init(void);         raw_ip.c       */
#include <netinet/obsd_kernel_ip_icmp.h>                /* icmp_init(void);        ip_icmp.c      */
#include <netinet/obsd_kernel_udp_var.h>                /* udp_init(void);         udp_usrreq.c   */
#include <netinet/obsd_kernel_tcp_var.h>                /* tcp_init(void);         tcp_subr.c     */
#include <netinet/obsd_kernel_in_pcb.h>                 /* in_pcbpooldealloc       in_pcb.c       */
#include <net/obsd_kernel_route.h>                      /* route_init(void);       route.c        */

struct	domain *domains;

void		pffasttimo(void *);
void		pfslowtimo(void *);
struct domain *	pffinddomain(int);

#if defined (KEY) || defined (IPSEC) || defined (TCP_SIGNATURE)
int pfkey_init(void);
#endif /* KEY || IPSEC || TCP_SIGNATURE */

#define	ADDDOMAIN(x)	{ \
	extern struct domain __CONCAT(x,domain); \
	__CONCAT(x,domain.dom_next) = domains; \
	domains = &__CONCAT(x,domain); \
}

static void uipc_domain_init_static_vars(void)
{
	domains = NULL; /* reset the Domainlist for a possible restart of the stack (OBSDPN-100)*/
}

void
domaininit(void)
{
	struct domain *dp;
	struct protosw *pr;
	static struct timeout pffast_timeout;
	static struct timeout pfslow_timeout;

	uipc_domain_init_static_vars();

#undef unix
	/*
	 * KAME NOTE: ADDDOMAIN(route) is moved to the last part so that
	 * it will be initialized as the *first* element.  confusing!
	 */
#ifndef lint

#ifdef INET
	ADDDOMAIN(inet);
#endif
#ifdef INET6
	ADDDOMAIN(inet6);
#endif /* INET6 */
#if defined (KEY) || defined (IPSEC) || defined (TCP_SIGNATURE)
	pfkey_init();
#endif /* KEY || IPSEC */
#ifdef MPLS
       ADDDOMAIN(mpls);
#endif
#ifdef NATM
	ADDDOMAIN(natm);
#endif
#ifdef IPSEC
#ifdef __KAME__
	ADDDOMAIN(key);
#endif
#endif
#if NBLUETOOTH > 0
	ADDDOMAIN(bt);
#endif
	ADDDOMAIN(route);
#endif

	for (dp = domains; dp; dp = dp->dom_next) {
		if (dp->dom_init)
			(*dp->dom_init)();
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_init)
				(*pr->pr_init)();
	}

	if (max_linkhdr < 16)		/* XXX */
		max_linkhdr = 16;
	max_hdr = max_linkhdr + max_protohdr;
	max_datalen = MHLEN - max_hdr;
	timeout_set(&pffast_timeout, pffasttimo, &pffast_timeout);
	timeout_set(&pfslow_timeout, pfslowtimo, &pfslow_timeout);
	timeout_add(&pffast_timeout, 1);
	timeout_add(&pfslow_timeout, 1);
}


void domaindeinit(void)
{
    struct domain *dp;
    struct protosw *pr;

    /* gh2289n: Normally we would like to iterate over all domains and all protos.                */
    /*          But this would require all domain and all proto definitions to have a deinit      */
    /*          function, which is not the case. So, in a first solution we call the required     */
    /*          deinit functions manually here.                                                   */

    for (dp = domains; dp; dp = dp->dom_next) 
    {
        /* de-initialize the protocols for the domain first */
        for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
        {
            if (pr->pr_init)
            {
                /* (*pr->pr_deinit)();    currently don't have a pr_deinit in OpenBSD */
                if (     pr->pr_init == ip_init  )
                    ip_deinit();
                else if (pr->pr_init == udp_init )
                    udp_deinit();
                else if (pr->pr_init == tcp_init )
                    tcp_deinit();
                else if (pr->pr_init == icmp_init)
                    icmp_deinit();
                else if (pr->pr_init == rip_init )
                    rip_deinit();
                /* currently no need for raw_deinit, igmp_deinit
                 * else if (pr->pr_init == raw_init )
                 *   raw_deinit()
                 * else
                 * {
                 *   int a = 3;
                 * }
                 */
            }
        } /* for */
        /* now de-initialize the domain itself */
        if (dp->dom_init)
        {
            /* (*dp->dom_deinit)();    currently don't have a dom_deinit in OpenBSD */
            if (dp->dom_init == route_init)
                route_deinit();
            /*else
             * {
             *   int a = 3;
             *}
             */
        }
    } /* for */

    /* after the domains are deinitialized we can destroy the inpcb_pool. Normally we would like  */
    /* to place the in_pcbpooldealloc call in the protocol deinit functions. But the inpcb_pool   */
    /* is initialized implicitly by the first protocol. This is hard to do for the deinit         */
    /* because a protocol of a domain doesn't know the other protocols and so it's impossible to  */
    /* decide which protocol is the last one within the protocol deinit itself (last one would    */
    /* have to call in_pcbpooldealloc). So we had to do this one layer higher (here), but this    */
    /* requires us have to call in_pcbpooldealloc). So we had to do this one layer higher (here), */
    /* but this requires us to use the pcb-interface.                                             */
    in_pcbpooldealloc();
}

struct domain *
pffinddomain(int family)
{
	struct domain *dp;

	for (dp = domains; dp != NULL; dp = dp->dom_next)
		if (dp->dom_family == family)
			return (dp);
	return (NULL);
}

struct protosw *
pffindtype(int family, int type)
{
	struct domain *dp;
	struct protosw *pr;

	dp = pffinddomain(family);
	if (dp == NULL)
		return (NULL);

	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
		if (pr->pr_type && pr->pr_type == type)
			return (pr);
	return (NULL);
}

struct protosw *
pffindproto(int family, int protocol, int type)
{
	struct domain *dp;
	struct protosw *pr;
	struct protosw *maybe = NULL;

	if (family == 0)
		return (NULL);

	dp = pffinddomain(family);
	if (dp == NULL)
		return (NULL);

	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++) {
		if ((pr->pr_protocol == protocol) && (pr->pr_type == type))
			return (pr);

		if (type == SOCK_RAW && pr->pr_type == SOCK_RAW &&
		    pr->pr_protocol == 0 && maybe == NULL)
			maybe = pr;
	}
	return (maybe);
}

int
net_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen, struct proc *p)
{
	struct domain *dp;
	struct protosw *pr;
	int family, protocol;

	OBSD_UNUSED_ARG(p); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	/*
	 * All sysctl names at this level are nonterminal.
	 * Usually: next two components are protocol family and protocol
	 *	number, then at least one addition component.
	 */
	if (namelen < 2)
		return (EISDIR);		/* overloaded */
	family = name[0];

	if (family == 0)
		return (0);
#if NBPFILTER > 0
	if (family == PF_BPF)
		return (bpf_sysctl(name + 1, namelen - 1, oldp, oldlenp,
		    newp, newlen));
#endif
#if NPFLOW > 0
	if (family == PF_PFLOW)
		return (pflow_sysctl(name + 1, namelen - 1, oldp, oldlenp,
		    newp, newlen));
#endif
#ifdef PIPEX
	if (family == PF_PIPEX)
		return (pipex_sysctl(name + 1, namelen - 1, oldp, oldlenp,
		    newp, newlen));
#endif
	dp = pffinddomain(family);
	if (dp == NULL)
		return (ENOPROTOOPT);
#ifdef MPLS
	/* XXX WARNING: big fat ugly hack */
	/* stupid net.mpls is special as it does not have a protocol */
	if (family == PF_MPLS)
		return (dp->dom_protosw[0].pr_sysctl(name + 1, namelen - 1,
		    oldp, oldlenp, newp, newlen));
#endif

	if (namelen < 3)
		return (EISDIR);		/* overloaded */
	protocol = name[1];
	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
		if (pr->pr_protocol == protocol && pr->pr_sysctl)
			return ((*pr->pr_sysctl)(name + 2, namelen - 2,
			    oldp, oldlenp, newp, newlen));
	return (ENOPROTOOPT);
}

void
pfctlinput(int cmd, struct sockaddr *sa)
{
	struct domain *dp;
	struct protosw *pr;

	for (dp = domains; dp; dp = dp->dom_next)
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_ctlinput)
				(*pr->pr_ctlinput)(cmd, sa, 0, NULL);
}

void
pfslowtimo(void *arg)
{
	struct timeout *to = (struct timeout *)arg;
	struct domain *dp;
	struct protosw *pr;

	for (dp = domains; dp; dp = dp->dom_next)
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_slowtimo)
				(*pr->pr_slowtimo)();
	timeout_add_msec(to, 500);
}

void
pffasttimo(void *arg)
{
	struct timeout *to = (struct timeout *)arg;
	struct domain *dp;
	struct protosw *pr;

	for (dp = domains; dp; dp = dp->dom_next)
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_fasttimo)
				(*pr->pr_fasttimo)();
	timeout_add_msec(to, 200);
}
