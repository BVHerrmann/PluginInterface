/*	$OpenBSD: kroute.c,v 1.32 2015/11/03 07:48:03 gerhard Exp $	*/

/*
 * Copyright (c) 2007, 2008 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2004 Esben Norby <norby@openbsd.org>
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_act_modul) */
#define LTRC_ACT_MODUL_ID 4052 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_act_modul) */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_tree.h>
#include <sys/obsd_kernel_uio.h>
#include <sys/obsd_kernel_ioctl.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_if_types.h>
#include <net/obsd_kernel_route.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_if_ether.h>
#ifdef IPv6_Genua
#include <netinet/obsd_kernel_icmp6.h>
#include <netinet6/obsd_kernel_nd6.h>
#endif /*IPv6_Genua*/
#include <arpa/obsd_userland_inet.h>

#include <obsd_userland_err.h>
#include <obsd_userland_errno.h>
#include <obsd_kernel_fcntl.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_string.h>
#include <obsd_userland_unistd.h>
#include <obsd_userland_event.h>

#include "obsd_snmpd_snmpd.h"
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

#define inline 

extern struct snmpd	*env;

struct ktable		**krt;
u_int			  krt_size;

struct {
	struct event		 ks_ev;
	u_long			 ks_iflastchange;
	u_long			 ks_nroutes;	/* 4 billions enough? */
	int			 ks_fd;
	int			 ks_ifd;
	u_short			 ks_nkif;
} kr_state;

struct kroute_node {
	RB_ENTRY(kroute_node)	 entry;
	struct kroute		 r;
	struct kroute_node	*next;
};

struct kroute6_node {
	RB_ENTRY(kroute6_node)	 entry;
	struct kroute6		 r;
	struct kroute6_node	*next;
};

struct kif_node {
	RB_ENTRY(kif_node)	 entry;
	TAILQ_HEAD(, kif_addr)	 addrs;
	TAILQ_HEAD(, kif_arp)	 arps;
	struct kif		 k;
};

int	kroute_compare(struct kroute_node *, struct kroute_node *);
int	kroute6_compare(struct kroute6_node *, struct kroute6_node *);
int	kif_compare(struct kif_node *, struct kif_node *);
#ifdef IPv6_Genua
static int cmp_defrouter6(const void *, const void *);
#endif /*IPv6_Genua*/

void			 ktable_init(void);
int			 ktable_new(u_int, u_int);
void			 ktable_free(u_int);
int			 ktable_exists(u_int, u_int *);
struct ktable		*ktable_get(u_int);
int			 ktable_update(u_int);

struct kroute_node	*kroute_find(struct ktable *, in_addr_t, u_int8_t,
			    u_int8_t);
struct kroute_node	*kroute_matchgw(struct kroute_node *,
			    struct sockaddr_in *);
int			 kroute_insert(struct ktable *, struct kroute_node *);
int			 kroute_remove(struct ktable *, struct kroute_node *);
void			 kroute_clear(struct ktable *);

struct kroute6_node	*kroute6_find(struct ktable *, const struct in6_addr *,
			    u_int8_t, u_int8_t);
struct kroute6_node	*kroute6_matchgw(struct kroute6_node *,
			    struct sockaddr_in6 *);
int			 kroute6_insert(struct ktable *, struct kroute6_node *);
int			 kroute6_remove(struct ktable *, struct kroute6_node *);
void			 kroute6_clear(struct ktable *);

struct kif_arp		*karp_find(struct sockaddr *, u_short);
int			 karp_insert(struct kif_node *, struct kif_arp *);
int			 karp_remove(struct kif_node *, struct kif_arp *);

struct kif_node		*kif_find(u_short);
struct kif_node		*kif_insert(u_short);
int			 kif_remove(struct kif_node *);
void			 kif_clear(void);
struct kif		*kif_update(u_short, int, struct if_data *,
			    struct sockaddr_dl *);

int			 ka_compare(struct kif_addr *, struct kif_addr *);
void			 ka_insert(u_short, struct kif_addr *);
struct kif_addr		*ka_find(struct sockaddr *);
int			 ka_remove(struct kif_addr *);

#ifdef IPv6_Genua
static int	cmp_prefix(struct sockaddr_in *, struct sockaddr_in *,
		    struct sockaddr_in *);
static int	cmp_in6_addr(struct in6_addr *, struct in6_addr *);
#endif
u_int8_t	prefixlen_classful(in_addr_t);
in_addr_t	prefixlen2mask(u_int8_t);
struct in6_addr *prefixlen2mask6(u_int8_t);
void		get_rtaddrs(int, struct sockaddr *, struct sockaddr **);
void		if_change(u_short, int, struct if_data *, struct sockaddr_dl *);
void		if_newaddr(u_short, struct sockaddr *, struct sockaddr *,
		    struct sockaddr *);
void		if_deladdr(u_short, struct sockaddr *, struct sockaddr *,
		    struct sockaddr *);
void		if_announce(void *);

int		fetchtable(struct ktable *);
int		fetchifs(u_short);
int		fetcharp(struct ktable *);
void		dispatch_rtmsg(int, short, void *);
int		rtmsg_process(char *, int);
int		dispatch_rtmsg_addr(struct ktable *, struct rt_msghdr *,
		    struct sockaddr *[RTAX_MAX]);

RB_PROTOTYPE(kroute_tree, kroute_node, entry, kroute_compare);
RB_GENERATE(kroute_tree, kroute_node, entry, kroute_compare);

RB_PROTOTYPE(kroute6_tree, kroute6_node, entry, kroute6_compare);
RB_GENERATE(kroute6_tree, kroute6_node, entry, kroute6_compare);

RB_HEAD(kif_tree, kif_node)		kit;
RB_PROTOTYPE(kif_tree, kif_node, entry, kif_compare);
RB_GENERATE(kif_tree, kif_node, entry, kif_compare);

RB_HEAD(ka_tree, kif_addr)		kat;
RB_PROTOTYPE(ka_tree, kif_addr, node, ka_compare);
RB_GENERATE(ka_tree, kif_addr, node, ka_compare);

void
kr_init(void)
{
	int		opt = 0, rcvbuf, default_rcvbuf;
	unsigned int	tid = RTABLE_ANY;
	socklen_t	optlen;

	if ((kr_state.ks_ifd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "kr_init: ioctl socket");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_001) */
		fatal("kr_init: ioctl socket");
	}

	if ((kr_state.ks_fd = socket(AF_ROUTE, SOCK_RAW, 0)) == -1)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_002) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "kr_init: route socket");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_002) */
		fatal("kr_init: route socket");
	}

	/* not interested in my own messages */
	if (setsockopt(kr_state.ks_fd, SOL_SOCKET, SO_USELOOPBACK,
	    &opt, sizeof(opt)) == -1)
	{	/* mh2290: LINT 548   else expected */
		log_warn("%s: SO_USELOOPBACK", __func__);	/* not fatal */
	}	/* mh2290: LINT 548   else expected */

	if (env->sc_rtfilter && setsockopt(kr_state.ks_fd, PF_ROUTE,
	    ROUTE_MSGFILTER, &env->sc_rtfilter, sizeof(env->sc_rtfilter)) == -1)
	{	/* mh2290: LINT 548   else expected */
		log_warn("%s: ROUTE_MSGFILTER", __func__);
	}	/* mh2290: LINT 548   else expected */

	/* grow receive buffer, don't wanna miss messages */
	optlen = sizeof(default_rcvbuf);
	if (getsockopt(kr_state.ks_fd, SOL_SOCKET, SO_RCVBUF,
	    &default_rcvbuf, &optlen) == -1)
	{	/* mh2290: LINT 548   else expected */
		log_warn("%s: SO_RCVBUF", __func__);
	}	/* mh2290: LINT 548   else expected */
	else
	{
		for (rcvbuf = MAX_RTSOCK_BUF;
		    rcvbuf > default_rcvbuf &&
		    setsockopt(kr_state.ks_fd, SOL_SOCKET, SO_RCVBUF,
		    &rcvbuf, sizeof(rcvbuf)) == -1 && errno == ENOBUFS;
		    rcvbuf /= 2)
			;	/* nothing */
	}

	if (setsockopt(kr_state.ks_fd, AF_ROUTE, ROUTE_TABLEFILTER, &tid,
	    sizeof(tid)) == -1)
	{	/* mh2290: LINT 548   else expected */
		log_warn("%s: ROUTE_TABLEFILTER", __func__);
	}	/* mh2290: LINT 548   else expected */

	RB_INIT(&kit);
	RB_INIT(&kat);
	
	/* RQ 1923794 */
	krt = NULL;
	krt_size = 0;

	if (fetchifs(0) == -1)
		fatalx("kr_init: fetchifs");

	ktable_init();

	event_set(&kr_state.ks_ev, kr_state.ks_fd, EV_READ | EV_PERSIST,
	    dispatch_rtmsg, NULL);
	event_add(&kr_state.ks_ev, NULL);
}

void
ktable_init(void)
{
	u_int		 i;

	for (i = 0; i < RT_TABLEID_MAX; i++)
		if (ktable_exists(i, NULL))
			ktable_update(i);
}

int
ktable_new(u_int rtableid, u_int rdomid)
{
	struct ktable	**xkrt;
	struct ktable	 *kt;
	size_t		  newsize, oldsize;

	/* resize index table if needed */
	if (rtableid >= krt_size) {
		if ((xkrt = reallocarray(krt, rtableid + 1,
		    sizeof(struct ktable *))) == NULL) {
			log_warn("%s: realloc", __func__);
			return (-1);
		}
		krt = xkrt;
		oldsize = krt_size * sizeof(struct ktable *);
		krt_size = rtableid + 1;
		newsize = krt_size * sizeof(struct ktable *);
		bzero((char *)krt + oldsize, newsize - oldsize);
	}

	if (krt[rtableid])
		fatalx("ktable_new: table already exists");

	/* allocate new element */
	kt = krt[rtableid] = calloc(1, sizeof(struct ktable));
	if (kt == NULL) {
		log_warn("%s: calloc", __func__);
		return (-1);
	}

	/* initialize structure ... */
	RB_INIT(&kt->krt);
	RB_INIT(&kt->krt6);
	kt->rtableid = rtableid;
	kt->rdomain = rdomid;

	/* ... and load it */
	if (fetchtable(kt) == -1)
		return (-1);
	/* load arp information */
	if (fetcharp(kt) == -1)
		return (-1);

	log_debug("%s: new ktable for rtableid %d", __func__, rtableid);
	return (0);
}

void
ktable_free(u_int rtableid)
{
	struct ktable	*kt;

	if ((kt = ktable_get(rtableid)) == NULL)
		return;

	log_debug("%s: freeing ktable rtableid %u", __func__, kt->rtableid);
	kroute_clear(kt);
	kroute6_clear(kt);

	krt[kt->rtableid] = NULL;
	free(kt);
}

struct ktable *
ktable_get(u_int rtableid)
{
	if (rtableid >= krt_size)
		return (NULL);
	return (krt[rtableid]);
}

int
ktable_update(u_int rtableid)
{
	struct ktable	*kt;
	u_int		 rdomid = 0; /* RQ 2136661 -- rdomid may be uninitialized */

	if (!ktable_exists(rtableid, &rdomid))
		fatalx("ktable_update: table doesn't exist");

	if (rdomid != rtableid) {
		if (ktable_get(rdomid) == NULL &&
		    ktable_new(rdomid, rdomid) != 0)
			return (-1);
	}

	kt = ktable_get(rtableid);
	if (kt == NULL) {
		if (ktable_new(rtableid, rdomid))
			return (-1);
	}
	return (0);
}

int
ktable_exists(u_int rtableid, u_int *rdomid)
{
	size_t			 len;
	struct rt_tableinfo	 info;
	int			 mib[6];

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = 0;
	mib[4] = NET_RT_TABLE;
	mib[5] = rtableid;

	len = sizeof(info);
	if (sysctl(mib, 6, &info, &len, NULL, 0) == -1) {
		if (errno == ENOENT)
			/* table nonexistent */
			return (0);
		log_warn("%s: sysctl", __func__);
		/* must return 0 so that the table is considered non-existent */
		return (0);
	}
	if (rdomid)
		*rdomid = info.rti_domainid;
	return (1);
}

void
kr_shutdown(void)
{
	u_int	i;

	for (i = krt_size; i > 0; i--) ktable_free(i - 1);
	if (krt) free(krt); /* sado -- RQ 1925764 memory left */

	/* RQ 1923794 */
	krt = NULL;
	krt_size = 0;

	kif_clear();
	close(kr_state.ks_fd);
	kr_state.ks_fd  = -1;
	event_del(&kr_state.ks_ev);
	close(kr_state.ks_ifd);
	kr_state.ks_ifd = -1;
}

u_int
kr_ifnumber(void)
{
	return (kr_state.ks_nkif);
}

u_long
kr_iflastchange(void)
{
	return (kr_state.ks_iflastchange);
}

int
kr_updateif(u_int if_index)
{
	return (fetchifs((u_short)if_index)); /* OBSD_ITGR -- omit warning */
}

int
kr_updatearp(u_int if_index)
{
	struct kif_arp	*ka;
	struct kif_node	*kn = kif_find((u_short)if_index);

	if (kn != NULL)
	{
		while ((ka = karp_first((u_short)if_index)) != NULL) /* OBSD_ITGR -- omit warning */
		{
			karp_remove(kn, ka);
		}
	}

	return fetcharp(krt[0]); /* OBSD_ITGR -- omit warning */
}


u_long
kr_routenumber(void)
{
	return (kr_state.ks_nroutes);
}

/* rb-tree compare */
int
kroute_compare(struct kroute_node *a, struct kroute_node *b)
{
	if (ntohl(a->r.prefix.s_addr) < ntohl(b->r.prefix.s_addr))
		return (-1);
	if (ntohl(a->r.prefix.s_addr) > ntohl(b->r.prefix.s_addr))
		return (1);
	if (a->r.prefixlen < b->r.prefixlen)
		return (-1);
	if (a->r.prefixlen > b->r.prefixlen)
		return (1);

	/* if the priority is RTP_ANY finish on the first address hit */
	if (a->r.priority == RTP_ANY || b->r.priority == RTP_ANY)
		return (0);
	if (a->r.priority < b->r.priority)
		return (-1);
	if (a->r.priority > b->r.priority)
		return (1);
	return (0);
}

int
kroute6_compare(struct kroute6_node *a, struct kroute6_node *b)
{
	int i;

	for (i = 0; i < 16; i++) {
		if (a->r.prefix.s6_addr[i] < b->r.prefix.s6_addr[i])
			return (-1);
		if (a->r.prefix.s6_addr[i] > b->r.prefix.s6_addr[i])
			return (1);
	}

	if (a->r.prefixlen < b->r.prefixlen)
		return (-1);
	if (a->r.prefixlen > b->r.prefixlen)
		return (1);

	/* if the priority is RTP_ANY finish on the first address hit */
	if (a->r.priority == RTP_ANY || b->r.priority == RTP_ANY)
		return (0);
	if (a->r.priority < b->r.priority)
		return (-1);
	if (a->r.priority > b->r.priority)
		return (1);
	return (0);
}

int
kif_compare(struct kif_node *a, struct kif_node *b)
{
	return (a->k.if_index - b->k.if_index);
}

int
ka_compare(struct kif_addr *a, struct kif_addr *b)
{
	if (a->addr.sa.sa_family < b->addr.sa.sa_family)
		return (-1);
	if (a->addr.sa.sa_family > b->addr.sa.sa_family)
		return (1);
	return (memcmp(&a->addr.sa, &b->addr.sa, a->addr.sa.sa_len));
}

/* tree management */
struct kroute_node *
kroute_find(struct ktable *kt, in_addr_t prefix, u_int8_t prefixlen,
    u_int8_t prio)
{
	struct kroute_node	s;
	struct kroute_node	*kn, *tmp;

	s.r.prefix.s_addr = prefix;
	s.r.prefixlen = prefixlen;
	s.r.priority = prio;

	kn = RB_FIND(kroute_tree, &kt->krt, &s);
	if (kn && prio == RTP_ANY) {
		tmp = RB_PREV(kroute_tree, &kt->krt, kn);
		while (tmp) {
			if (kroute_compare(&s, tmp) == 0)
				kn = tmp;
			else
				break;
			tmp = RB_PREV(kroute_tree, &kt->krt, kn);
		}
	}
	return (kn);
}

struct kroute_node *
kroute_matchgw(struct kroute_node *kr, struct sockaddr_in *sa_in)
{
	in_addr_t	nexthop;

	if (sa_in == NULL) {
		log_warnx("%s: no nexthop defined", __func__);
		return (NULL);
	}
	nexthop = sa_in->sin_addr.s_addr;

	while (kr) {
		if (kr->r.nexthop.s_addr == nexthop)
			return (kr);
		kr = kr->next;
	}

	return (NULL);
}

int
kroute_insert(struct ktable *kt, struct kroute_node *kr)
{
	struct kroute_node	*krm;

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_006) */
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "kroute_insert");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_006) */
	if ((krm = RB_INSERT(kroute_tree, &kt->krt, kr)) != NULL) {
		/* multipath route, add at end of list */
		while (krm->next != NULL)
			krm = krm->next;
		krm->next = kr;
		kr->next = NULL; /* to be sure */
	}

	kr_state.ks_nroutes++;
	return (0);
}

int
kroute_remove(struct ktable *kt, struct kroute_node *kr)
{
	struct kroute_node	*krm;

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_008) */
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "kroute_remove");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_008) */
	if ((krm = RB_FIND(kroute_tree, &kt->krt, kr)) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_009) */
		TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "kroute_remove failed to find %u.%u.%u.%u/%u",
			(unsigned)(kr->r.prefix.s_addr&0xff), (unsigned)((kr->r.prefix.s_addr>>8)&0xff),
			(unsigned)((kr->r.prefix.s_addr>>16)&0xff), (unsigned)((kr->r.prefix.s_addr>>24)&0xff), kr->r.prefixlen);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_009) */
		log_warnx("%s: failed to find %s/%u", __func__,
		    inet_ntoa(kr->r.prefix), kr->r.prefixlen);
		return (-1);
	}

	if (krm == kr) {
		/* head element */
		if (RB_REMOVE(kroute_tree, &kt->krt, kr) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_010) */
			TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "kroute_remove failed for %u.%u.%u.%u/%u",
			    (unsigned)(kr->r.prefix.s_addr&0xff), (unsigned)((kr->r.prefix.s_addr>>8)&0xff),
				(unsigned)((kr->r.prefix.s_addr>>16)&0xff), (unsigned)((kr->r.prefix.s_addr>>24)&0xff), kr->r.prefixlen);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_010) */
			log_warnx("%s: failed for %s/%u", __func__,
			    inet_ntoa(kr->r.prefix), kr->r.prefixlen);
			return (-1);
		}
		if (kr->next != NULL) {
			if (RB_INSERT(kroute_tree, &kt->krt, kr->next)
			    != NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_011) */
				TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "kroute_remove failed to add %u.%u.%u.%u/%u",
				    (unsigned)(kr->r.prefix.s_addr&0xff), (unsigned)((kr->r.prefix.s_addr>>8)&0xff), 
					(unsigned)((kr->r.prefix.s_addr>>16)&0xff), (unsigned)((kr->r.prefix.s_addr>>24)&0xff), kr->r.prefixlen);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_011) */
				log_warnx("%s: failed to add %s/%u", __func__,
				    inet_ntoa(kr->r.prefix), kr->r.prefixlen);
				return (-1);
			}
		}
	} else {
		/* somewhere in the list */
		while (krm->next != kr && krm->next != NULL)
			krm = krm->next;
		if (krm->next == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_012) */
			TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_WARN, "kroute_remove multipath list corrupted for %u.%u.%u.%u/%u",
				(unsigned)(kr->r.prefix.s_addr&0xff), (unsigned)((kr->r.prefix.s_addr>>8)&0xff),
				(unsigned)((kr->r.prefix.s_addr>>16)&0xff), (unsigned)((kr->r.prefix.s_addr>>24)&0xff), kr->r.prefixlen);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_012) */
			log_warnx("%s: multipath list corrupted for %s/%u",
			    __func__, inet_ntoa(kr->r.prefix), kr->r.prefixlen);
			return (-1);
		}
		krm->next = kr->next;
	}

	kr_state.ks_nroutes--;
	free(kr);
	return (0);
}

void
kroute_clear(struct ktable *kt)
{
	struct kroute_node	*kr;

	while ((kr = RB_MIN(kroute_tree, &kt->krt)) != NULL)
		kroute_remove(kt, kr);
}

struct kroute6_node *
kroute6_find(struct ktable *kt, const struct in6_addr *prefix,
    u_int8_t prefixlen, u_int8_t prio)
{
	struct kroute6_node	s;
	struct kroute6_node	*kn6, *tmp;

	memcpy(&s.r.prefix, prefix, sizeof(struct in6_addr));
	s.r.prefixlen = prefixlen;
	s.r.priority = prio;

	kn6 = RB_FIND(kroute6_tree, &kt->krt6, &s);
	if (kn6 && prio == RTP_ANY) {
		tmp = RB_PREV(kroute6_tree, &kt->krt6, kn6);
		while (tmp) {
			if (kroute6_compare(&s, tmp) == 0)
				kn6 = tmp;
			else
				break;
			tmp = RB_PREV(kroute6_tree, &kt->krt6, kn6);
		}
	}
	return (kn6);
}

struct kroute6_node *
kroute6_matchgw(struct kroute6_node *kr, struct sockaddr_in6 *sa_in6)
{
	struct in6_addr	nexthop;

	if (sa_in6 == NULL) {
		log_warnx("%s: no nexthop defined", __func__);
		return (NULL);
	}
	memcpy(&nexthop, &sa_in6->sin6_addr, sizeof(nexthop));

	while (kr) {
		if (memcmp(&kr->r.nexthop, &nexthop, sizeof(nexthop)) == 0)
			return (kr);
		kr = kr->next;
	}

	return (NULL);
}

int
kroute6_insert(struct ktable *kt, struct kroute6_node *kr)
{
	struct kroute6_node	*krm;

	if ((krm = RB_INSERT(kroute6_tree, &kt->krt6, kr)) != NULL) {
		/* multipath route, add at end of list */
		while (krm->next != NULL)
			krm = krm->next;
		krm->next = kr;
		kr->next = NULL; /* to be sure */
	}

	kr_state.ks_nroutes++;
	return (0);
}

int
kroute6_remove(struct ktable *kt, struct kroute6_node *kr)
{
	struct kroute6_node	*krm;

	if ((krm = RB_FIND(kroute6_tree, &kt->krt6, kr)) == NULL) {
		log_warnx("%s: failed for %s/%u", __func__,
		    log_in6addr(&kr->r.prefix), kr->r.prefixlen);
		return (-1);
	}

	if (krm == kr) {
		/* head element */
		if (RB_REMOVE(kroute6_tree, &kt->krt6, kr) == NULL) {
			log_warnx("%s: failed for %s/%u", __func__,
			    log_in6addr(&kr->r.prefix), kr->r.prefixlen);
			return (-1);
		}
		if (kr->next != NULL) {
			if (RB_INSERT(kroute6_tree, &kt->krt6, kr->next) !=
			    NULL) {
				log_warnx("%s: failed to add %s/%u", __func__,
				    log_in6addr(&kr->r.prefix),
				    kr->r.prefixlen);
				return (-1);
			}
		}
	} else {
		/* somewhere in the list */
		while (krm->next != kr && krm->next != NULL)
			krm = krm->next;
		if (krm->next == NULL) {
			log_warnx("%s: multipath list corrupted for %s/%u",
			    __func__, log_in6addr(&kr->r.prefix),
			    kr->r.prefixlen);
			return (-1);
		}
		krm->next = kr->next;
	}

	kr_state.ks_nroutes--;
	free(kr);
	return (0);
}

void
kroute6_clear(struct ktable *kt)
{
	struct kroute6_node	*kr;

	while ((kr = RB_MIN(kroute6_tree, &kt->krt6)) != NULL)
		kroute6_remove(kt, kr);
}

static inline int
karp_compare(struct kif_arp *a, struct kif_arp *b)
{
	/* Interface indices are assumed equal */
	if (ntohl(a->addr.sin.sin_addr.s_addr) >
	    ntohl(b->addr.sin.sin_addr.s_addr))
		return (1);
	if (ntohl(a->addr.sin.sin_addr.s_addr) <
	    ntohl(b->addr.sin.sin_addr.s_addr))
		return (-1);
	return (0);
}

static inline struct kif_arp *
karp_search(struct kif_node *kn, struct kif_arp *ka)
{
	struct kif_arp		*pivot;

	TAILQ_FOREACH(pivot, &kn->arps, entry) {
		switch (karp_compare(ka, pivot)) {
		case 0: /* found */
			return (pivot);
		case -1: /* ka < pivot, end the search */
			return (NULL);
		default:		/* mh2290: LINT 744 switch statement has no default */
			break;
		}
	}
	/* looped through the whole list and didn't find */
	return (NULL);
}

struct kif_arp *
karp_find(struct sockaddr *sa, u_short ifindex)
{
	struct kif_node		*kn;
	struct kif_arp		*ka = NULL, s;

	memcpy(&s.addr.sa, sa, sa->sa_len);

	if (ifindex == 0) {
		/*
		 * We iterate manually to handle zero ifindex special
		 * case differently from kif_find, in particular we
		 * want to look for the address on all available
		 * interfaces.
		 */
		RB_FOREACH(kn, kif_tree, &kit) {
			if ((ka = karp_search(kn, &s)) != NULL)
				break;
		}
	} else {
		if ((kn = kif_find(ifindex)) == NULL)
			return (NULL);
		ka = karp_search(kn, &s);
	}
	return (ka);
}

int
karp_insert(struct kif_node *kn, struct kif_arp *ka)
{
	struct kif_arp		*pivot;

	if (ka->if_index == 0)
		return (-1);
	if (!kn && (kn = kif_find(ka->if_index)) == NULL)
		return (-1);
	/* Put entry on the list in the ascending lexical order */
	TAILQ_FOREACH(pivot, &kn->arps, entry) {
		switch (karp_compare(ka, pivot)) {
		case 0: /* collision */
			return (-1);
		case -1: /* ka < pivot */
			TAILQ_INSERT_BEFORE(pivot, ka, entry);
			return (0);
		default:		/* mh2290: LINT 744 switch statement has no default */
			break;
		}
	}
	/* ka is larger than any other element on the list */
	TAILQ_INSERT_TAIL(&kn->arps, ka, entry);
	return (0);
}

int
karp_remove(struct kif_node *kn, struct kif_arp *ka)
{
	if (ka->if_index == 0)
		return (-1);
	if (!kn && (kn = kif_find(ka->if_index)) == NULL)
		return (-1);
	TAILQ_REMOVE(&kn->arps, ka, entry);
	free(ka);
	return (0);
}

struct kif_arp *
karp_first(u_short ifindex)
{
	struct kif_node		*kn;

	if ((kn = kif_find(ifindex)) == NULL)
		return (NULL);
	return (TAILQ_FIRST(&kn->arps));
}

struct kif_arp *
karp_getaddr(struct sockaddr *sa, u_short ifindex, int next)
{
	struct kif_arp		*ka;

	if ((ka = karp_find(sa, ifindex)) == NULL)
		return (NULL);
	return (next ? TAILQ_NEXT(ka, entry) : ka);
}

struct kif_node *
kif_find(u_short if_index)
{
	struct kif_node	s;

	if (if_index == 0)
		return (RB_MIN(kif_tree, &kit));

	bzero(&s, sizeof(s));
	s.k.if_index = if_index;

	return (RB_FIND(kif_tree, &kit, &s));
}

struct kif *
kr_getif(u_short if_index)
{
	struct kif_node	*kn;

	kn = kif_find(if_index);
	if (kn == NULL)
		return (NULL);

	return (&kn->k);
}

struct kif *
kr_getnextif(u_short if_index)
{
	struct kif_node	*kn;

	if ((kn = kif_find(if_index)) == NULL)
		return (NULL);
	if (if_index)
		kn = RB_NEXT(kif_tree, &kit, kn);
	if (kn == NULL)
		return (NULL);

	return (&kn->k);
}

struct kif_node *
kif_insert(u_short if_index)
{
	struct kif_node	*kif;

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_014) */
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "kif_insert interface %u", if_index);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_014) */

	if ((kif = calloc(1, sizeof(struct kif_node))) == NULL)
		return (NULL);

	kif->k.if_index = if_index;
	TAILQ_INIT(&kif->addrs);
	TAILQ_INIT(&kif->arps);

	if (RB_INSERT(kif_tree, &kit, kif) != NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_015) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "kif_insert: RB_INSERT");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_015) */
		fatalx("kif_insert: RB_INSERT");
	}

	kr_state.ks_nkif++;
	kr_state.ks_iflastchange = smi_getticks();

	return (kif);
}

int
kif_remove(struct kif_node *kif)
{
	struct kif_addr	*ka;
	struct kif_arp	*kr;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_017) */
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "kif_remove interface %u", kif->k.if_index);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_017) */

	if (RB_REMOVE(kif_tree, &kit, kif) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_018) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_WARN, "RB_REMOVE(kif_tree, &kit, kif)");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_018) */
		log_warnx("%s: RB_REMOVE failed", __func__);
		return (-1);
	}

	while ((ka = TAILQ_FIRST(&kif->addrs)) != NULL) {
		TAILQ_REMOVE(&kif->addrs, ka, entry);
		ka_remove(ka);
	}
	while ((kr = TAILQ_FIRST(&kif->arps)) != NULL) {
		karp_remove(kif, kr);
	}
	free(kif);

	kr_state.ks_nkif--;
	kr_state.ks_iflastchange = smi_getticks();

	return (0);
}

void
kif_clear(void)
{
	struct kif_node	*kif;

	while ((kif = RB_MIN(kif_tree, &kit)) != NULL)
		kif_remove(kif);
	kr_state.ks_nkif = 0;
	kr_state.ks_iflastchange = smi_getticks();
}

struct kif *
kif_update(u_short if_index, int flags, struct if_data *ifd,
    struct sockaddr_dl *sdl)
{
	struct kif_node		*kif;
	struct ether_addr	*ea;
	struct ifreq		 ifr;

	if ((kif = kif_find(if_index)) == NULL)
		if ((kif = kif_insert(if_index)) == NULL)
			return (NULL);

	kif->k.if_flags = flags;
	bcopy(ifd, &kif->k.if_data, sizeof(struct if_data));
	kif->k.if_ticks = smi_getticks();

	if (sdl && sdl->sdl_family == AF_LINK) {
		if (sdl->sdl_nlen >= sizeof(kif->k.if_name))
			memcpy(kif->k.if_name, sdl->sdl_data,
			    sizeof(kif->k.if_name) - 1);
		else if (sdl->sdl_nlen > 0)
			memcpy(kif->k.if_name, sdl->sdl_data,
			    sdl->sdl_nlen);
		/* string already terminated via calloc() */

		if ((ea = (struct ether_addr *)LLADDR(sdl)) != NULL)
			bcopy(&ea->ether_addr_octet, kif->k.if_lladdr,
			    ETHER_ADDR_LEN);
	}

	bzero(&ifr, sizeof(ifr));
	strlcpy(ifr.ifr_name, kif->k.if_name, sizeof(ifr.ifr_name));
	ifr.ifr_data = (caddr_t)&kif->k.if_descr;
	if (ioctl(kr_state.ks_ifd, SIOCGIFDESCR, (caddr_t)&ifr) == -1)  /* gh2289n: added cast to caddr_t to avoid compiler warning */
		bzero(&kif->k.if_descr, sizeof(kif->k.if_descr));

	return (&kif->k);
}

void
ka_insert(u_short if_index, struct kif_addr *ka)
{
	if (ka->addr.sa.sa_len == 0)
		return;

	ka->if_index = if_index;
	RB_INSERT(ka_tree, &kat, ka);
}

struct kif_addr	*
ka_find(struct sockaddr *sa)
{
	struct kif_addr		ka;

	if (sa == NULL)
		return (RB_MIN(ka_tree, &kat));
	bzero(&ka.addr, sizeof(ka.addr));
	bcopy(sa, &ka.addr.sa, sa->sa_len);
	return (RB_FIND(ka_tree, &kat, &ka));
}

#ifdef IPv6_Genua
struct kroute *
obsd_snmpd_kr_getdefaultrouter(struct sockaddr_in *sa)
{
	struct ktable		*kt = NULL;
	struct kroute_node	*kn = NULL;
	size_t i;

	/* get default routing domain */
	for (i = 0; i < krt_size; i++)
		if (krt[i]->rdomain == 0)
			kt = krt[i];

	if (kt == NULL)
		return NULL;

	RB_FOREACH(kn, kroute_tree, &kt->krt) {
		if (kn->r.prefixlen != 0)	/* skip non-default routes */
			continue;

		if (sa == NULL)			/* get the first one */
			return &kn->r;

		if (kn->r.nexthop.s_addr == sa->sin_addr.s_addr)
			return &kn->r;
	}

	return NULL;
}

struct kroute *
obsd_snmpd_kr_getnextdefaultrouter(struct sockaddr_in *sa)
{
	struct ktable		*kt = NULL;
	struct kroute_node	*kn = NULL;
	size_t i;
	int found = 0;

	/* get default routing domain */
	for (i = 0; i < krt_size; i++)
		if (krt[i]->rdomain == 0)
			kt = krt[i];

	if (kt == NULL)
		return NULL;

	RB_FOREACH(kn, kroute_tree, &kt->krt) {
		if (kn->r.prefixlen != 0)	/* skip non-default routes */
			continue;

		if (sa == NULL || kn->r.nexthop.s_addr == sa->sin_addr.s_addr) {
			found = 1;
			continue;
		}

		if (found == 0)
			continue;

		return &kn->r;
	}

	return NULL;
}

static int
cmp_defrouter6(const void *A, const void *B)
{
	const struct in6_defrouter *a = A;
	const struct in6_defrouter *b = B;
	int i;

	for (i = 0; i < 16; i++)
		if (a->rtaddr.sin6_addr.s6_addr[i] < b->rtaddr.sin6_addr.s6_addr[i])
			return -1;
		else if (a->rtaddr.sin6_addr.s6_addr[i] > b->rtaddr.sin6_addr.s6_addr[i])
			return 1;

	return 0;
}

int
obsd_snmpd_kr_getdefaultrouter6(struct in6_addr *addr,
    struct in6_defrouter *router)
{
	int mib[] = { CTL_NET, PF_INET6, IPPROTO_ICMPV6, ICMPV6CTL_ND6_DRLIST };
	char *buf;
	struct in6_defrouter *p, *ep;
	size_t l;

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, &l, NULL, 0) < 0)
		return -1;

	if (l == 0)
		return -1;

	if ((buf = malloc(l)) == NULL)
		return -1;

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), buf, &l, NULL, 0) < 0)
		goto err;

	qsort(buf, l / sizeof *p, sizeof *p, cmp_defrouter6);

	ep = (struct in6_defrouter *)(buf + l);
	for (p = (struct in6_defrouter *)buf; p < ep; p++) {
		if (addr == NULL || cmp_in6_addr(addr, &p->rtaddr.sin6_addr)) {
			memcpy(router, p, sizeof *router);
			free(buf);
			return 0;
		}
	}
 err:
	free(buf);
	return -1;
}

int
obsd_snmpd_kr_getnextdefaultrouter6(struct in6_addr *addr,
    struct in6_defrouter *router)
{
	int mib[] = { CTL_NET, PF_INET6, IPPROTO_ICMPV6, ICMPV6CTL_ND6_DRLIST };
	char *buf;
	struct in6_defrouter *p, *ep;
	size_t l;
	int found = 0;

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, &l, NULL, 0) < 0)
		return -1;

	if (l == 0)
		return -1;

	if ((buf = malloc(l)) == NULL)
		return -1;

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), buf, &l, NULL, 0) < 0)
		goto err;

	qsort(buf, l / sizeof *p, sizeof *p, cmp_defrouter6);

	ep = (struct in6_defrouter *)(buf + l);
	for (p = (struct in6_defrouter *)buf; p < ep; p++) {
		if (addr == NULL || cmp_in6_addr(addr, &p->rtaddr.sin6_addr)) {
			found = 1;
			continue;
		}

		if (found) {
			memcpy(router, p, sizeof *router);
			free(buf);
			return 0;
		}
	}
 err:
	free(buf);
	return -1;
}

struct kif_addr *
obsd_snmpd_kr_getprefix(uint8_t if_index, struct sockaddr *sa, uint8_t len)
{
	struct kif_node *kif;
	struct kif_addr *ka;
	struct sockaddr_in *addr = (struct sockaddr_in *)sa;
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)sa;

	if ((kif = kif_find(if_index)) == NULL)
		return NULL;

	RB_FOREACH(ka, ka_tree, &kat) {
		if (ka->if_index != if_index)
			continue;

		if (sa == NULL)		/* first address of interface */
			return ka;

		if (sa->sa_family != ka->addr.sa.sa_family)
			continue;

		if (sa->sa_family == AF_INET &&
		    cmp_prefix(addr, &ka->addr.sin, &ka->mask.sin))
			return ka;

		if (sa->sa_family == AF_INET6 &&
		    obsd_snmpd_cmp_prefix6(addr6, &ka->addr.sin6, &ka->mask.sin6))
			return ka;
	}

	return NULL;
}

struct kif_addr *
obsd_snmpd_kr_getnextprefix(uint8_t if_index, struct sockaddr *sa, uint8_t len)
{
	struct kif_addr	*ka;
	struct kif_node	*kif;
	struct sockaddr_in *addr = (struct sockaddr_in *)sa;
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)sa;
	int found = 0;

	if ((kif = kif_find(if_index)) == NULL)
		return NULL;

	RB_FOREACH(ka, ka_tree, &kat) {
		if (ka->if_index != if_index)
			continue;

		if (sa == NULL)
			return ka;

		if (found == 1) {
			if (sa->sa_family != ka->addr.sa.sa_family)
				return ka;

			if (sa->sa_family == AF_INET &&
			    !cmp_prefix(addr, &ka->addr.sin, &ka->mask.sin))
				return ka;

			if (sa->sa_family == AF_INET6 &&
			    !obsd_snmpd_cmp_prefix6(addr6, &ka->addr.sin6,
			    &ka->mask.sin6))
				return ka;

			continue;
		}

		if (sa->sa_family != ka->addr.sa.sa_family)
			continue;

		if (sa->sa_family == AF_INET &&
		    cmp_prefix(addr, &ka->addr.sin, &ka->mask.sin))
			found = 1;

		if (sa->sa_family == AF_INET6 && obsd_snmpd_cmp_prefix6(addr6,
		    &ka->addr.sin6, &ka->mask.sin6))
				found = 1;
	}

	return obsd_snmpd_kr_getnextprefix(if_index + 1, NULL, 0);
}

static int
cmp_prefix(struct sockaddr_in *a, struct sockaddr_in *b,
    struct sockaddr_in *mask)
{
	return ((a->sin_addr.s_addr & mask->sin_addr.s_addr) ==
		(b->sin_addr.s_addr & mask->sin_addr.s_addr));
}

int
obsd_snmpd_cmp_prefix6(struct sockaddr_in6 *a, struct sockaddr_in6 *b,
    struct sockaddr_in6 *mask)
{
	int i;

	for (i = 0; i < 16; i++)
		if ((a->sin6_addr.s6_addr[i] & mask->sin6_addr.s6_addr[i]) !=
		    (b->sin6_addr.s6_addr[i] & mask->sin6_addr.s6_addr[i]))
			return 0;

	return 1;
}

static int
cmp_in6_addr(struct in6_addr *a, struct in6_addr *b)
{
	int i;

	for (i = 0; i < 16; i++)
		if (a->s6_addr[i] != b->s6_addr[i])
			return 0;

	return 1;
}
#endif /*IPv6_Genua*/

int
ka_remove(struct kif_addr *ka)
{
	RB_REMOVE(ka_tree, &kat, ka);
	free(ka);
	return (0);
}

struct kif_addr *
kr_getaddr(struct sockaddr *sa)
{
	return (ka_find(sa));
}

struct kif_addr *
kr_getnextaddr(struct sockaddr *sa)
{
	struct kif_addr	*ka;

	if ((ka = ka_find(sa)) == NULL)
		return (NULL);
	if (sa)
		ka = RB_NEXT(ka_tree, &kat, ka);

	return (ka);
}

/* misc */
u_int8_t
prefixlen_classful(in_addr_t ina)
{
	/* it hurt to write this. */

	if (ina >= 0xf0000000U)		/* class E */
		return (32);
	else if (ina >= 0xe0000000U)	/* class D */
		return (4);
	else if (ina >= 0xc0000000U)	/* class C */
		return (24);
	else if (ina >= 0x80000000U)	/* class B */
		return (16);
	else				/* class A */
		return (8);
}

u_int8_t
obsd_snmpd_mask2prefixlen(in_addr_t ina)
{
	if (ina == 0)
		return (0);
	else
		return (u_int8_t)(33 - ffs(ntohl(ina))); /* OBSD_ITGR -- omit warning */
}

in_addr_t
prefixlen2mask(u_int8_t prefixlen)
{
	if (prefixlen == 0)
		return (0);

	return (htonl(0xffffffff << (32 - prefixlen)));
}

u_int8_t
obsd_snmpd_mask2prefixlen6(struct sockaddr_in6 *sa_in6)
{
	u_int8_t	 l = 0, *ap, *ep;

	/*
	 * sin6_len is the size of the sockaddr so substract the offset of
	 * the possibly truncated sin6_addr struct.
	 */
	ap = (u_int8_t *)&sa_in6->sin6_addr;
	ep = (u_int8_t *)sa_in6 + sa_in6->sin6_len;
	for (; ap < ep; ap++) {
		/* this "beauty" is adopted from sbin/route/show.c ... */
		switch (*ap) {
		case 0xff:
			l += 8;
			break;
		case 0xfe:
			l += 7;
			return (l);
		case 0xfc:
			l += 6;
			return (l);
		case 0xf8:
			l += 5;
			return (l);
		case 0xf0:
			l += 4;
			return (l);
		case 0xe0:
			l += 3;
			return (l);
		case 0xc0:
			l += 2;
			return (l);
		case 0x80:
			l += 1;
			return (l);
		case 0x00:
			return (l);
		default:
			fatalx("non contiguous inet6 netmask");
		}
	}

	return (l);
}

struct in6_addr *
prefixlen2mask6(u_int8_t prefixlen)
{
	static struct in6_addr	mask;
	int			i;

	bzero(&mask, sizeof(mask));
	for (i = 0; i < prefixlen / 8; i++)
		mask.s6_addr[i] = 0xff;
	i = prefixlen % 8;
	if (i)
		mask.s6_addr[prefixlen / 8] = (u_int8_t) (0xff00 >> i); /* OBSD_ITGR -- omit warning */

	return (&mask);
}

#define	ROUNDUP(a)	\
    (((a) & (sizeof(long) - 1)) ? (1 + ((a) | (sizeof(long) - 1))) : (a))

void
get_rtaddrs(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
	int	i;

	for (i = 0; i < RTAX_MAX; i++) {
		if (addrs & (1 << i)) {
			rti_info[i] = sa;
			sa = (struct sockaddr *)((char *)(sa) +
			    ROUNDUP(sa->sa_len));
		} else
			rti_info[i] = NULL;

	}
}

void
if_change(u_short if_index, int flags, struct if_data *ifd,
    struct sockaddr_dl *sdl)
{
	if (kif_update(if_index, flags, ifd, sdl) == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_020) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_WARN, "if_change:  kif_update(%u)", (uint32_t)if_index);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_020) */
		log_warn("%s: interface %u update failed", __func__, if_index);
	}
}

void
if_newaddr(u_short if_index, struct sockaddr *ifa, struct sockaddr *mask,
    struct sockaddr *brd)
{
	struct kif_node *kif;
	struct kif_addr *ka;

	if (ifa == NULL)
		return;
	if ((kif = kif_find(if_index)) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_022) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_WARN, "if_newaddr: corresponding if %d not found", (uint32_t)if_index);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_022) */
		log_warnx("%s: corresponding if %u not found", __func__,
		    if_index);
		return;
	}
	if ((ka = ka_find(ifa)) == NULL) {
		if ((ka = calloc(1, sizeof(struct kif_addr))) == NULL)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_023) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "if_newaddr");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_023) */
			fatal("if_newaddr");
		}
		bcopy(ifa, &ka->addr.sa, ifa->sa_len);
		TAILQ_INSERT_TAIL(&kif->addrs, ka, entry);
		ka_insert(if_index, ka);
	}

	if (mask)
		bcopy(mask, &ka->mask.sa, mask->sa_len);
	else
		bzero(&ka->mask, sizeof(ka->mask));
	if (brd)
		bcopy(brd, &ka->dstbrd.sa, brd->sa_len);
	else
		bzero(&ka->dstbrd, sizeof(ka->dstbrd));

}

void
if_deladdr(u_short if_index, struct sockaddr *ifa, struct sockaddr *mask,
    struct sockaddr *brd)
{
	struct kif_node *kif;
	struct kif_addr *ka;

	OBSD_UNUSED_ARG(mask)  /* gh2289n: avoid compiler warning */;
	OBSD_UNUSED_ARG(brd);  /* gh2289n: avoid compiler warning */

	if (ifa == NULL)
		return;
	if ((kif = kif_find(if_index)) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_kroute_024) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_WARN, "if_deladdr: corresponding if %d not found", (uint32_t)if_index);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_kroute_024) */
		log_warnx("%s: corresponding if %u not found", __func__,
		    if_index);
		return;
	}
	if ((ka = ka_find(ifa)) == NULL)
		return;

	TAILQ_REMOVE(&kif->addrs, ka, entry);
	ka_remove(ka);
}

void
if_announce(void *msg)
{
	struct if_announcemsghdr	*ifan;
	struct kif_node			*kif;

	ifan = msg;

	switch (ifan->ifan_what) {
	case IFAN_ARRIVAL:
		kif = kif_insert(ifan->ifan_index);
		strlcpy(kif->k.if_name, ifan->ifan_name,
		    sizeof(kif->k.if_name));
		break;
	case IFAN_DEPARTURE:
		kif = kif_find(ifan->ifan_index);
		kif_remove(kif);
		break;
	default:		/* mh2290: LINT 744 switch statement has no default */
		break;
	}
}

int
fetchtable(struct ktable *kt)
{
	int			 mib[7];
	size_t			 len;
	char			*buf;
	int			 rv;

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;
	mib[6] = kt->rtableid;

	if (sysctl(mib, 7, NULL, &len, NULL, 0) == -1) {
		if (kt->rtableid != 0 && errno == EINVAL)
			/* table nonexistent */
			return (0);
		log_warn("%s: failed to fetch routing table %u size", __func__,
		    kt->rtableid);
		return (-1);
	}
	if (len == 0)
		return (0);
	if ((buf = malloc(len)) == NULL) {
		log_warn("%s: malloc", __func__);
		return (-1);
	}
	if (sysctl(mib, 7, buf, &len, NULL, 0) == -1) {
		log_warn("%s: failed to fetch routing table %u", __func__,
		    kt->rtableid);
		free(buf);
		return (-1);
	}

	rv = rtmsg_process(buf, len);
	free(buf);

	return (rv);
}

int
fetchifs(u_short if_index)
{
	size_t			 len;
	int			 mib[6];
	char			*buf;
	int			 rv;

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = 0;	/* wildcard address family */
	mib[4] = NET_RT_IFLIST;
	mib[5] = if_index;

	if (sysctl(mib, 6, NULL, &len, NULL, 0) == -1) {
		log_warn("%s: failed to fetch address table size for %u",
		    __func__, if_index);
		return (-1);
	}
	/* Empty table? */
	if (len == 0)
		return (0);
	if ((buf = malloc(len)) == NULL) {
		log_warn("%s: malloc", __func__);
		return (-1);
	}
	if (sysctl(mib, 6, buf, &len, NULL, 0) == -1) {
		log_warn("%s: failed to fetch address table for %u",
		    __func__, if_index);
		free(buf);
		return (-1);
	}

	rv = rtmsg_process(buf, len);
	free(buf);

	return (rv);
}

int
fetcharp(struct ktable *kt)
{
	size_t			 len;
	int			 mib[7];
	char			*buf;
	int			 rv;

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_FLAGS;
	mib[5] = RTF_LLINFO;
	mib[6] = kt->rtableid;

	if (sysctl(mib, 7, NULL, &len, NULL, 0) == -1) {
		log_warn("%s: failed to fetch arp table %u size", __func__,
		    kt->rtableid);
		return (-1);
	}
	/* Empty table? */
	if (len == 0)
		return (0);
	if ((buf = malloc(len)) == NULL) {
		log_warn("%s: malloc", __func__);
		return (-1);
	}
	if (sysctl(mib, 7, buf, &len, NULL, 0) == -1) {
		log_warn("%s: failed to fetch arp table %u", __func__,
		    kt->rtableid);
		free(buf);
		return (-1);
	}

	rv = rtmsg_process(buf, len);
	free(buf);

	return (rv);
}

/* ARGSUSED */
void
dispatch_rtmsg(int fd, short event, void *arg)
{
	char			 buf[RT_BUF_SIZE];
	ssize_t			 n;

	OBSD_UNUSED_ARG(event); /* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(arg);   /* gh2289n: avoid compiler warning */

	if ((n = read(fd, &buf, sizeof(buf))) == -1) {
		log_warn("%s: read error", __func__);
		return;
	}

	if (n == 0) {
		log_warnx("%s: routing socket closed", __func__);
		return;
	}

	rtmsg_process(buf, n);
}

int
rtmsg_process(char *buf, int len)
{
	struct ktable		*kt;
	struct rt_msghdr	*rtm;
	struct if_msghdr	 ifm;
	struct ifa_msghdr	*ifam;
	struct sockaddr		*sa, *rti_info[RTAX_MAX];
	int			 offset;
	char			*next;

	for (offset = 0; offset < len; offset += rtm->rtm_msglen) {
		next = buf + offset;
		rtm = (struct rt_msghdr *)next;
		if (rtm->rtm_version != RTM_VERSION)
			continue;

		sa = (struct sockaddr *)(next + rtm->rtm_hdrlen);
		get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

		switch (rtm->rtm_type) {
		case RTM_ADD:
		case RTM_GET:
		case RTM_CHANGE:
		case RTM_DELETE:
		case RTM_RESOLVE:
			if (rtm->rtm_errno)		 /* failed attempts */
				continue;

			if ((kt = ktable_get(rtm->rtm_tableid)) == NULL)
				continue;

			if (dispatch_rtmsg_addr(kt, rtm, rti_info) == -1)
				return (-1);
			break;
		case RTM_IFINFO:
			memcpy(&ifm, next, sizeof(ifm));
			if_change(ifm.ifm_index, ifm.ifm_flags, &ifm.ifm_data,
			    (struct sockaddr_dl *)rti_info[RTAX_IFP]);
			break;
		case RTM_DELADDR:
			ifam = (struct ifa_msghdr *)rtm;
			if ((ifam->ifam_addrs & (RTA_NETMASK | RTA_IFA |
			    RTA_BRD)) == 0)
				break;

			if_deladdr(ifam->ifam_index, rti_info[RTAX_IFA],
			    rti_info[RTAX_NETMASK], rti_info[RTAX_BRD]);
			break;
		case RTM_NEWADDR:
			ifam = (struct ifa_msghdr *)rtm;
			if ((ifam->ifam_addrs & (RTA_NETMASK | RTA_IFA |
			    RTA_BRD)) == 0)
				break;

			if_newaddr(ifam->ifam_index, rti_info[RTAX_IFA],
			    rti_info[RTAX_NETMASK], rti_info[RTAX_BRD]);
			break;
		case RTM_IFANNOUNCE:
			if_announce(next);
			break;
#ifndef BSD_STACKPORT
		case RTM_DESYNC:
			kr_shutdown();
			if (fetchifs(0) == -1)
				fatalx("rtmsg_process: fetchifs");
			ktable_init();
			break;
#endif
		default:
			/* ignore for now */
			break;
		}
	}

	return (offset);
}

int
dispatch_rtmsg_addr(struct ktable *kt, struct rt_msghdr *rtm,
    struct sockaddr *rti_info[RTAX_MAX])
{
	struct sockaddr		*sa, *psa;
	struct sockaddr_in	*sa_in, *psa_in = NULL;
	struct sockaddr_in6	*sa_in6, *psa_in6 = NULL;
	struct sockaddr_dl	*sa_dl;
	struct kroute_node	*kr;
	struct kroute6_node	*kr6;
	struct kif_arp		*ka;
	int			 flags, mpath = 0;
	u_int16_t		 ifindex;
	u_int8_t		 prefixlen;
	u_int8_t		 prio;

	flags = 0;
	ifindex = 0;
	prefixlen = 0;

	if ((psa = rti_info[RTAX_DST]) == NULL)
		return (-1);

	if (rtm->rtm_flags & RTF_STATIC)
		flags |= F_STATIC;
	if (rtm->rtm_flags & RTF_BLACKHOLE)
		flags |= F_BLACKHOLE;
	if (rtm->rtm_flags & RTF_REJECT)
		flags |= F_REJECT;
	if (rtm->rtm_flags & RTF_DYNAMIC)
		flags |= F_DYNAMIC;
#ifdef RTF_MPATH
	if (rtm->rtm_flags & RTF_MPATH)
		mpath = 1;
#endif

	prio = rtm->rtm_priority;
	switch (psa->sa_family) {
	case AF_INET:
		psa_in = (struct sockaddr_in *)psa;
		sa_in = (struct sockaddr_in *)rti_info[RTAX_NETMASK];
		if (sa_in != NULL) {
			if (sa_in->sin_len != 0)
				prefixlen = obsd_snmpd_mask2prefixlen(
				    sa_in->sin_addr.s_addr);
		} else if (rtm->rtm_flags & RTF_HOST)
			prefixlen = 32;
		else
			prefixlen =
			    prefixlen_classful(psa_in->sin_addr.s_addr);
		break;
	case AF_INET6:
		psa_in6 = (struct sockaddr_in6 *)psa;
		sa_in6 = (struct sockaddr_in6 *)rti_info[RTAX_NETMASK];
		if (sa_in6 != NULL) {
			if (sa_in6->sin6_len != 0)
				prefixlen = obsd_snmpd_mask2prefixlen6(sa_in6);
		} else if (rtm->rtm_flags & RTF_HOST)
			prefixlen = 128;
		else
			fatalx("in6 net addr without netmask");
		break;
	default:
		return (0);
	}

	ifindex = rtm->rtm_index;
	if ((sa = rti_info[RTAX_GATEWAY]) != NULL)
		switch (sa->sa_family) {
		case AF_INET:
		case AF_INET6:
			if (rtm->rtm_flags & RTF_CONNECTED) {
				flags |= F_CONNECTED;
				ifindex = rtm->rtm_index;
			}
			mpath = 0;	/* link local stuff can't be mpath */
			break;
		case AF_LINK:
			/*
			 * Traditional BSD connected routes have
			 * a gateway of type AF_LINK.
			 */
			flags |= F_CONNECTED;
			ifindex = rtm->rtm_index;
			mpath = 0;	/* link local stuff can't be mpath */
			break;
		default:		/* mh2290: LINT 744 switch statement has no default */
			break;
		}

	if (rtm->rtm_type == RTM_DELETE) {
		if (sa != NULL && sa->sa_family == AF_LINK &&
		    (rtm->rtm_flags & RTF_HOST) &&
		    psa->sa_family == AF_INET) {
			if ((ka = karp_find(psa, ifindex)) == NULL)
				return (0);
			if (karp_remove(NULL, ka) == -1)
				return (-1);
			return (0);
		} else if (sa == NULL && (rtm->rtm_flags & RTF_HOST) &&
		    psa->sa_family == AF_INET) {
			if ((ka = karp_find(psa, ifindex)) != NULL)
				karp_remove(NULL, ka);
			/* Continue to the route section below  */
		}
		switch (psa->sa_family) {
		case AF_INET:
			sa_in = (struct sockaddr_in *)sa;
			if ((kr = kroute_find(kt, psa_in->sin_addr.s_addr,
			    prefixlen, prio)) == NULL)
				return (0);

			if (mpath)
				/* get the correct route */
				if ((kr = kroute_matchgw(kr, sa_in)) == NULL) {
					log_warnx("%s[delete]: "
					    "mpath route not found", __func__);
					return (0);
				}

			if (kroute_remove(kt, kr) == -1)
				return (-1);
			break;
		case AF_INET6:
			sa_in6 = (struct sockaddr_in6 *)sa;
			if ((kr6 = kroute6_find(kt, &psa_in6->sin6_addr,
			    prefixlen, prio)) == NULL)
				return (0);

			if (mpath)
				/* get the correct route */
				if ((kr6 = kroute6_matchgw(kr6, sa_in6)) ==
				    NULL) {
					log_warnx("%s[delete]: "
					    "IPv6 mpath route not found",
					    __func__);
					return (0);
				}

			if (kroute6_remove(kt, kr6) == -1)
				return (-1);
			break;
		default:		/* mh2290: LINT 744 switch statement has no default */
			break;
		}
		return (0);
	}

	if (sa == NULL && !(flags & F_CONNECTED))
		return (0);

	/* Add or update an ARP entry */
	if ((rtm->rtm_flags & RTF_LLINFO) && (rtm->rtm_flags & RTF_HOST) &&
	    sa != NULL && sa->sa_family == AF_LINK &&
	    psa->sa_family == AF_INET) {
		sa_dl = (struct sockaddr_dl *)sa;
		/* ignore incomplete entries */
		if (!sa_dl->sdl_alen)
			return (0);
		/* ignore entries that do not specify an interface */
		if (ifindex == 0)
			return (0);
		if ((ka = karp_find(psa, ifindex)) != NULL) {
			memcpy(&ka->target.sdl, sa_dl, sa_dl->sdl_len);
			if (rtm->rtm_flags & RTF_PERMANENT_ARP)
				flags |= F_STATIC;
			ka->flags = (u_short) flags;    /* gerlach/gh2289n: added type cast to u_short to avoid compiler warning C4244 */
		} else {
			if ((ka = calloc(1, sizeof(struct kif_arp))) == NULL) {
				log_warn("%s: calloc", __func__);
				return (-1);
			}
			memcpy(&ka->addr.sa, psa, psa->sa_len);
			memcpy(&ka->target.sdl, sa_dl, sa_dl->sdl_len);
			if (rtm->rtm_flags & RTF_PERMANENT_ARP)
				flags |= F_STATIC;
			ka->flags =  (u_short) flags;   /* gerlach/gh2289n: added type cast to u_short to avoid compiler warning C4244 */
			ka->if_index = ifindex;
			if (karp_insert(NULL, ka)) {
				free(ka);
				log_warnx("%s: failed to insert", __func__);
				return (-1);
			}
		}
		return (0);
	}

	switch (psa->sa_family) {
	case AF_INET:
		sa_in = (struct sockaddr_in *)sa;
		if ((kr = kroute_find(kt, psa_in->sin_addr.s_addr, prefixlen,
		    prio)) != NULL) {
			/* get the correct route */
			if (mpath && rtm->rtm_type == RTM_CHANGE &&
			    (kr = kroute_matchgw(kr, sa_in)) == NULL) {
				log_warnx("%s[change]: "
				    "mpath route not found", __func__);
				return (-1);
			} else if (mpath && rtm->rtm_type == RTM_ADD)
				goto add4;

			if (sa_in != NULL)
			{
				if (sa->sa_family == AF_LINK) /* sado -- RQ 1944372, zero-gateway is added as sockaddr_dl - Part 2 - Aug.2018 RQ3157038  */
				{
					struct sockaddr *temp = calloc(1, sizeof(struct sockaddr));
					bzero(temp, sizeof(struct sockaddr));
					sa_dl = (struct sockaddr_dl*)sa;
					memcpy(temp->sa_data, LLADDR(sa_dl), sa_dl->sdl_alen);

					kr->r.nexthop.s_addr = ((struct sockaddr_in*)temp)->sin_addr.s_addr;
					free(temp);
				}
				else
					kr->r.nexthop.s_addr = sa_in->sin_addr.s_addr;
			}
			else
				kr->r.nexthop.s_addr = 0;
			kr->r.flags = (u_int16_t) flags; /* OBSD_ITGR -- omit warning */
			kr->r.if_index = ifindex;
			kr->r.ticks = smi_getticks();
		} else {
add4:
			if ((kr = calloc(1,
			    sizeof(struct kroute_node))) == NULL) {
				log_warn("%s: calloc", __func__);
				return (-1);
			}
			kr->r.prefix.s_addr = psa_in->sin_addr.s_addr;
			kr->r.prefixlen = prefixlen;

			if (sa_in != NULL)
			{
				if (sa->sa_family == AF_LINK) /* sado -- RQ 1944372, zero-gateway is added as sockaddr_dl */
				{
					struct sockaddr *temp = calloc(1, sizeof(struct sockaddr));
					bzero(temp, sizeof(struct sockaddr));
					sa_dl = (struct sockaddr_dl*)sa;
					memcpy(temp->sa_data, LLADDR(sa_dl), sa_dl->sdl_alen);

					kr->r.nexthop.s_addr = ((struct sockaddr_in*)temp)->sin_addr.s_addr;
					free(temp);
				}
				else
					kr->r.nexthop.s_addr = sa_in->sin_addr.s_addr;
			}
			else
				kr->r.nexthop.s_addr = 0;
			kr->r.flags = (u_int16_t)flags; /* OBSD_ITGR -- omit warning */
			kr->r.if_index = ifindex;
			kr->r.ticks = smi_getticks();
			kr->r.priority = prio;

			kroute_insert(kt, kr);
		}
		break;
	case AF_INET6:
		sa_in6 = (struct sockaddr_in6 *)sa;
		if ((kr6 = kroute6_find(kt, &psa_in6->sin6_addr, prefixlen,
		    prio)) != NULL) {
			/* get the correct route */
			if (mpath && rtm->rtm_type == RTM_CHANGE &&
			    (kr6 = kroute6_matchgw(kr6, sa_in6)) ==
			    NULL) {
				log_warnx("%s[change]: "
				    "IPv6 mpath route not found", __func__);
				return (-1);
			} else if (mpath && rtm->rtm_type == RTM_ADD)
				goto add6;

			if (sa_in6 != NULL)
				memcpy(&kr6->r.nexthop,
				    &sa_in6->sin6_addr,
				    sizeof(struct in6_addr));
			else
				memcpy(&kr6->r.nexthop,
				    &in6addr_any,
				    sizeof(struct in6_addr));

			kr6->r.flags = (u_int16_t)flags; /* OBSD_ITGR -- omit warning */
			kr6->r.if_index = ifindex;
			kr6->r.ticks = smi_getticks();
		} else {
add6:
			if ((kr6 = calloc(1,
			    sizeof(struct kroute6_node))) == NULL) {
				log_warn("%s: calloc", __func__);
				return (-1);
			}
			memcpy(&kr6->r.prefix, &psa_in6->sin6_addr,
			    sizeof(struct in6_addr));
			kr6->r.prefixlen = prefixlen;
			if (sa_in6 != NULL)
				memcpy(&kr6->r.nexthop, &sa_in6->sin6_addr,
				    sizeof(struct in6_addr));
			else
				memcpy(&kr6->r.nexthop, &in6addr_any,
				    sizeof(struct in6_addr));
			kr6->r.flags = (u_int16_t)flags; /* OBSD_ITGR -- omit warning */
			kr6->r.if_index = ifindex;
			kr6->r.ticks = smi_getticks();
			kr6->r.priority = prio;

			kroute6_insert(kt, kr6);
		}
		break;
	default:		/* mh2290: LINT 744 switch statement has no default */
		break;
	}

	return (0);
}

struct kroute *
kroute_first(void)
{
	struct kroute_node	*kn;
	struct ktable		*kt;

	if ((kt = ktable_get(0)) == NULL)
		return (NULL);
	kn = RB_MIN(kroute_tree, &kt->krt);
	if (kn == NULL)		/* mh2290: LINT 794 Conceivable use of null pointer */
		return (NULL);
	return (&kn->r);
}

struct kroute *
kroute_getaddr(in_addr_t prefix, u_int8_t prefixlen, u_int8_t prio, int next)
{
	struct kroute_node	*kn;
	struct ktable		*kt;

	if ((kt = ktable_get(0)) == NULL)
		return (NULL);
	kn = kroute_find(kt, prefix, prefixlen, prio);
	if (kn != NULL && next)
		kn = RB_NEXT(kroute_tree, &kt->krt, kn);
	if (kn != NULL)
		return (&kn->r);
	else
		return (NULL);
}

struct kroute *
kroute_get_by_prefix (in_addr_t prefix, int require_exact_match, int next)
{
        struct kroute_node      ref;
        struct kroute_node      *kn;
        ref.r.prefix.s_addr = prefix;
        ref.r.prefixlen = 0;
        ref.r.priority = 0;

        kn = RB_NFIND(kroute_tree, &(krt[0]->krt), &ref);

	if (kn && require_exact_match && (kn->r.prefix.s_addr != prefix))
	{
	    /* exact match failed */
	    return NULL;
	}

	if (next) {
		/* get next element, skip elements with identical prefix. */
		while (kn && (kn->r.prefix.s_addr == prefix)) {
			kn = RB_NEXT(kroute_tree, krt, kn);
		}
	}

	if (kn != NULL)
		return (&kn->r);
	return (NULL);
}
