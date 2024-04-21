/*	$OpenBSD: ip_carp.h,v 1.28 2010/04/25 17:38:53 mpf Exp $	*/

/*
 * Copyright (c) 2002 Michael Shalayeff. All rights reserved.
 * Copyright (c) 2003 Ryan McBride. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR HIS RELATIVES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NETINET_IP_CARP_H_
#define _NETINET_IP_CARP_H_

/*
 * The CARP header layout is as follows:
 *
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |Version| Type  | VirtualHostID |    AdvSkew    |    Auth Len   |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |    Demotion   |     AdvBase   |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Counter (1)                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Counter (2)                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (1)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (2)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (3)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (4)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (5)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

#ifdef     BSD_STACKPORT    /* gerlach/gh2289n: packing      */
#define   OBSD_CAL_PACK_SIZE     1  /* byte packing/alignment*/
#include "obsd_platform_cal_pack_on.h"
#endif  /* BSD_STACKPORT */

PNIO_PACKED_ATTRIBUTE_PRE struct carp_header {
#ifndef    BSD_STACKPORT  /* gh2289n: bitfield: ensure alignment for struct tcphdr */
#if _BYTE_ORDER == _LITTLE_ENDIAN
	u_int		carp_type:4,
			carp_version:4;
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
	u_int		carp_version:4,
			carp_type:4;
#endif
#else /* BSD_STACKPORT */
	/* gh2289n: MSVC doesn't pack bitfields at all, whether per default nor using a #pragma pack(1). */
	/* Seems that MSVC 2005 (and 2010 too) always allocates the underlying basic datatype of the     */
	/* bitfield (u_int in our case). So the only way to get a byte reserved for the bitfield (like   */
	/* necessary here) would require using u_char or int8 or u_int8 as underlying type. But using    */
	/* other types as int for a bitfield is not really ANSI C conform.                               */
	/* note: whereas it's OK for MSVC using u_int8_t in ANSI C compatibility mode (option /Za) using */
	/* this with the microsoft Language extensions enabled (default without /Za option) leads to a   */
	/* Level 4 compiler warning (C4214: nonstandard extension used : bit field types other than int).*/
	/* Unfortunately PN SW other than our TCP-Stack requires microsoft language extensions and on    */
	/* the other hand it's required to get no Level 4 compiler warnings. So we can't use a           */
	/* bitfield here. The advantage is that we are independent from the endianness here.             */
	u_int8_t    carp_version_type; /* 1 byte for carp version (higher nibble) & carp type (lower nibble) */
#endif  /* BSD_STACKPORT */
	u_int8_t	carp_vhid;	/* virtual host id */
	u_int8_t	carp_advskew;	/* advertisement skew */
	u_int8_t	carp_authlen;   /* size of counter+md, 32bit chunks */
	u_int8_t	carp_demote;	/* demotion indicator */
	u_int8_t	carp_advbase;	/* advertisement interval */
	u_int16_t	carp_cksum;
	u_int32_t	carp_counter[2];
	unsigned char	carp_md[20];	/* SHA1 HMAC */
} __packed PNIO_PACKED_ATTRIBUTE_POST;

#ifdef     BSD_STACKPORT    /* gerlach/gh2289n: packing      */
#include "obsd_platform_cal_pack_off.h"
#endif  /* BSD_STACKPORT */

#ifdef    BSD_STACKPORT                     /* gh2289n: need a fix for the outgoing interface in case of CARP for H/R-C2 (see RQ2710554) */
#define  OBSD_CARP_IF_SELECTION_PATCH  1    /*          1: ensure that outgoing IP Packets are routed over the CARP IF    */
                                            /*             (instead to be always sent out via the underlying physical     */
                                            /*              interface) if the local IP address is the IP address of the   */
                                            /*              CARP interface                                                */
                                            /*          0: standard OBSD CARP interface behaviour                         */
#endif /* BSD_STACKPORT */

#define	CARP_DFLTTL		255

/* carp_version */
#define	CARP_VERSION		2

/* carp_type */
#define	CARP_ADVERTISEMENT	0x01

#define	CARP_KEY_LEN		20	/* a sha1 hash of a passphrase */

/* carp_advbase */
#define	CARP_DFLTINTV		1

/*
 * Statistics.
 */
struct carpstats {
	u_int64_t	carps_ipackets;		/* total input packets, IPv4 */
	u_int64_t	carps_ipackets6;	/* total input packets, IPv6 */
	u_int64_t	carps_badif;		/* wrong interface */
	u_int64_t	carps_badttl;		/* TTL is not CARP_DFLTTL */
	u_int64_t	carps_hdrops;		/* packets shorter than hdr */
	u_int64_t	carps_badsum;		/* bad checksum */
	u_int64_t	carps_badver;		/* bad (incl unsupp) version */
	u_int64_t	carps_badlen;		/* data length does not match */
	u_int64_t	carps_badauth;		/* bad authentication */
	u_int64_t	carps_badvhid;		/* bad VHID */
	u_int64_t	carps_badaddrs;		/* bad address list */

	u_int64_t	carps_opackets;		/* total output packets, IPv4 */
	u_int64_t	carps_opackets6;	/* total output packets, IPv6 */
	u_int64_t	carps_onomem;		/* no memory for an mbuf */
	u_int64_t	carps_ostates;		/* total state updates sent */

	u_int64_t	carps_preempt;		/* transitions to master */
};

#define CARPDEVNAMSIZ	16
#ifdef IFNAMSIZ
#if CARPDEVNAMSIZ != IFNAMSIZ
#error
#endif
#endif

/*
 * Configuration structure for SIOCSVH SIOCGVH
 */
struct carpreq {
	int		carpr_state;
#define	CARP_STATES	"INIT", "BACKUP", "MASTER"
#define	CARP_MAXSTATE	2
#define	CARP_MAXNODES	32

	char		carpr_carpdev[CARPDEVNAMSIZ];
	u_int8_t	carpr_vhids[CARP_MAXNODES];
	u_int8_t	carpr_advskews[CARP_MAXNODES];
	u_int8_t	carpr_states[CARP_MAXNODES];
#define	CARP_BAL_MODES	"none", "arp", "ip", "ip-stealth", "ip-unicast"
#define CARP_BAL_NONE		0
#define CARP_BAL_ARP		1
#define CARP_BAL_IP		2
#define CARP_BAL_IPSTEALTH	3
#define CARP_BAL_IPUNICAST	4
#define CARP_BAL_MAXID		4
	u_int8_t	carpr_balancing;
	int		carpr_advbase;
	unsigned char	carpr_key[CARP_KEY_LEN];
	struct in_addr	carpr_peer;
};

/*
 * Names for CARP sysctl objects
 */
#define	CARPCTL_ALLOW		1	/* accept incoming CARP packets */
#define	CARPCTL_PREEMPT		2	/* high-pri backup preemption mode */
#define	CARPCTL_LOG		3	/* log bad packets */
#define	CARPCTL_STATS		4	/* CARP stats */
#define	CARPCTL_MAXID		5

#define	CARPCTL_NAMES { \
	{ 0, 0 }, \
	{ "allow", CTLTYPE_INT }, \
	{ "preempt", CTLTYPE_INT }, \
	{ "log", CTLTYPE_INT }, \
	{ "stats", CTLTYPE_STRUCT }, \
}

#if OBSD_CARP_ENABLE_IF_ACCESS
 /* These definitions are moved to the header file, since it is accessed by
 obsd_userland_pnio_if_funcs.c in the TIA context. Accessing kernel interface
 structures is not good idea and should be refactored in a second step. */

/* defining moved struct definitions in header file requires inclusion of 
some additional header files: */
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_if_ether.h>

struct carp_softc {
	struct arpcom sc_ac;
#define	sc_if		sc_ac.ac_if
#define	sc_carpdev	sc_ac.ac_if.if_carpdev
	void *ah_cookie;
	void *lh_cookie;
	struct ip_moptions sc_imo;
#ifdef INET6
	struct ip6_moptions sc_im6o;
#endif /* INET6 */
	TAILQ_ENTRY(carp_softc) sc_list;

	int sc_suppress;
	int sc_bow_out;
	int sc_demote_cnt;

	int sc_sendad_errors;
#define CARP_SENDAD_MAX_ERRORS(sc) (3 * (sc)->sc_vhe_count)
	int sc_sendad_success;
#define CARP_SENDAD_MIN_SUCCESS(sc) (3 * (sc)->sc_vhe_count)

	char sc_curlladdr[ETHER_ADDR_LEN];

	LIST_HEAD(__carp_vhosthead, carp_vhost_entry)	carp_vhosts;
	int sc_vhe_count;
	u_int8_t sc_vhids[CARP_MAXNODES];
	u_int8_t sc_advskews[CARP_MAXNODES];
	u_int8_t sc_balancing;

	int sc_naddrs;
	int sc_naddrs6;
	int sc_advbase;		/* seconds */

						/* authentication */
	unsigned char sc_key[CARP_KEY_LEN];

	u_int32_t sc_hashkey[2];
	u_int32_t sc_lsmask;		/* load sharing mask */
	int sc_lscount;			/* # load sharing interfaces (max 32) */
	int sc_delayed_arp;		/* delayed ARP request countdown */

	struct in_addr sc_peer;

	LIST_HEAD(__carp_mchead, carp_mc_entry)	carp_mc_listhead;
	struct carp_vhost_entry *cur_vhe; /* current active vhe */
};

struct carp_if {
	TAILQ_HEAD(, carp_softc) vhif_vrs;
	int vhif_nvrs;

	struct ifnet *vhif_ifp;
};

#endif /* OBSD_CARP_ENABLE_IF_ACCESS */

#ifdef _KERNEL
void		 carp_ifdetach (struct ifnet *);
void		 carp_proto_input (struct mbuf *, ...);
void		 carp_carpdev_state(void *);
void		 carp_group_demote_adj(struct ifnet *, int, char *);
int		 carp6_proto_input(struct mbuf **, int *, int);
int		 carp_iamatch(struct in_ifaddr *, u_char *, u_int8_t **,
		     u_int8_t **);
int		 carp_iamatch6(struct ifnet *, u_char *, struct sockaddr_dl **);
struct ifnet	*carp_ourether(void *, struct ether_header *, int);
int		 carp_input(struct mbuf *, u_int8_t *, u_int8_t *, u_int16_t);
int		 carp_output(struct ifnet *, struct mbuf *, struct sockaddr *,
		     struct rtentry *);
int		 carp_sysctl(int *, u_int,  void *, size_t *, void *, size_t);
int		 carp_lsdrop(struct mbuf *, sa_family_t, u_int32_t *, u_int32_t *);
void		 carp_rewrite_lladdr(struct ifnet *, u_int8_t *);
int		 carp_our_mcastaddr(struct ifnet *, u_int8_t *);
#if       OBSD_CARP_IF_SELECTION_PATCH
struct ifnet *obsd_carp_if_check_src_ip(caddr_t carp_if, in_addr_t ip_src_addr);
#endif /* OBSD_CARP_IF_SELECTION_PATCH */
#endif /* _KERNEL */
#endif /* _NETINET_IP_CARP_H_ */
