/**
 * mib_addon_rfc1213.c
 * Implements additional oids to provide compatibility with RFC1213.
 */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_protosw.h>

#include <net/obsd_kernel_if.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_ip_icmp.h>
#include <netinet/obsd_kernel_icmp_var.h>
#include <netinet/obsd_kernel_udp.h>
#include <netinet/obsd_kernel_udp_var.h>
#include <netinet/obsd_kernel_tcp.h>
#include <netinet/obsd_kernel_tcp_timer.h>
#include <netinet/obsd_kernel_tcp_var.h>

#include <obsd_userland_limits.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_event.h>
#include <obsd_userland_string.h>
#include <obsd_kernel_fcntl.h>

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_libkvmnet.h"
#include "obsd_snmpd_mib_addon_rfc1213.h"


/* private prototypes */
static struct ber_element *
obsd_snmpd_add_ipaddress(struct ber_element *elm, struct in_addr in_a);

static uint16_t
obsd_snmpd_decode_port(struct ber_oid *o, int oid_index);

void
obsd_snmpd_encode_port(struct ber_oid *o, int oid_index, uint16_t port);

static struct in_addr
obsd_snmpd_get_ip_on_if(struct in_addr prefix, int if_index);

static struct ber_oid *
obsd_snmpd_iproutetable_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no);

static int
obsd_snmpd_iproutetable(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);


static int
obsd_snmpd_udp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);

static struct ber_oid *
obsd_snmpd_udp_table_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no);

static int
obsd_snmpd_udp_table(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);

static int
obsd_snmpd_icmp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);

static int
obsd_snmpd_tcp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);

static struct ber_oid *
obsd_snmpd_tcp_table_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no);

static int
obsd_snmpd_obsdstate_to_rfcstate(int obsdstate);

static int
obsd_snmpd_tcp_table(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm);


/* oid declaration */
static struct oid	obsd_snmpd_mib_addon_rfc1213_tree[] =
	MIB_ADDON_RFC1213_TREE;

/* oid implementation */
static struct oid	obsd_snmpd_mib_addon_rfc1213_implementation[] = {
	/* ip group */
	{ MIB(ipMIB),		OID_MIB },
	{ MIB(ipRouteDesc),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteIfIndex),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMetric1),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMetric2),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMetric3),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMetric4),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteNextHop),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteType),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteProto),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteAge),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMask),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteMetric5),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	{ MIB(ipRouteInfo),	OID_TRD,
		obsd_snmpd_iproutetable, NULL, obsd_snmpd_iproutetable_idx },
	/* udp group */

	{ MIB(udpMIB),		OID_MIB },
	{ MIB(udpInDatagrams),	OID_RD,  obsd_snmpd_udp_scalar, NULL },
	{ MIB(udpNoPorts),	OID_RD,  obsd_snmpd_udp_scalar, NULL },
	{ MIB(udpInErrors),	OID_RD,  obsd_snmpd_udp_scalar, NULL },
	{ MIB(udpOutDatagrams),	OID_RD,  obsd_snmpd_udp_scalar, NULL },
	{ MIB(udpLocalAddress),	OID_TRD,
		obsd_snmpd_udp_table, NULL, obsd_snmpd_udp_table_idx },
	{ MIB(udpLocalPort),	OID_TRD,
		obsd_snmpd_udp_table, NULL, obsd_snmpd_udp_table_idx },
	/* icmp group */
	{ MIB(icmpMIB),		OID_MIB },
	{ MIB(icmpInMsgs),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInErrors),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInDestUnreachs),  OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInTimeExcds),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInParmProbs),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInSrcQuenchs),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInRedirects),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInEchos),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInEchoReps),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInTimestamps),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInTimestampReps), OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInAddrMasks),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpInAddrMaskReps),  OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutMsgs),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutErrors),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutDestUnreachs), OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutTimeExcds),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutParmProbs),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutSrcQuenchs),   OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutRedirects),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutEchos),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutEchoReps),	    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutTimestamps),   OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutTimestampReps),OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutAddrMasks),    OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	{ MIB(icmpOutAddrMaskReps), OID_RD,  obsd_snmpd_icmp_scalar, NULL },
	/* tcp group */
	{ MIB(tcpMIB),		OID_MIB },
	{ MIB(tcpRtoAlgorithm),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpRtoMin),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpRtoMax),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpMaxConn),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpActiveOpens),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpPassiveOpens),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpAttemptFails),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpEstabResets),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpCurrEstab),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpInSegs),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpOutSegs),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpRetransSegs),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpInErrs),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpOutErrs),	    OID_RD,  obsd_snmpd_tcp_scalar, NULL },
	{ MIB(tcpConnState),	OID_TRD,
		obsd_snmpd_tcp_table, NULL, obsd_snmpd_tcp_table_idx },
	{ MIB(tcpConnLocalAddress),	OID_TRD,
		obsd_snmpd_tcp_table, NULL, obsd_snmpd_tcp_table_idx },
	{ MIB(tcpConnLocalPort),	OID_TRD,
		obsd_snmpd_tcp_table, NULL, obsd_snmpd_tcp_table_idx },
	{ MIB(tcpConnRemAddress),	OID_TRD,
		obsd_snmpd_tcp_table, NULL, obsd_snmpd_tcp_table_idx },
	{ MIB(tcpConnRemPort),		OID_TRD,
		obsd_snmpd_tcp_table, NULL, obsd_snmpd_tcp_table_idx },
	{ MIBEND }
};

/* OID 0.0, some mib implementations need this */
/* This is actually const, but not every function handles this correctly */
static struct ber_oid  obsd_snmpd_zerodotzero = { { 0, 0 }, 2 };

/* set the ipaddress in the specified ber_element */
static struct ber_element *
obsd_snmpd_add_ipaddress(struct ber_element *elm, struct in_addr ina) {
#if 0
	elm = ber_add_nstring(elm, (char*)(&(ina.s_addr)), sizeof(ina.s_addr)); 
#else
	/* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/
	uint32_t addr = ina.s_addr;
	elm = ber_add_nstring(elm, (char *)&addr, sizeof(ina.s_addr)); 
	if (elm == NULL) return NULL; /* RQ 1944871 -- NULL pointer exception */
#endif
	ber_set_header(elm, BER_CLASS_APPLICATION, SNMP_T_IPADDR);

	return elm;
}

static uint16_t
obsd_snmpd_decode_port(struct ber_oid *o, int oid_index)
{
	return htons(o->bo_id[oid_index]);
}

void
obsd_snmpd_encode_port(struct ber_oid *o, int oid_index, uint16_t port)
{
	o->bo_id[oid_index] = ntohs(port);
}

/* get ip address on the specified interface based on the network prefix */
static struct in_addr
obsd_snmpd_get_ip_on_if(struct in_addr nw_prefix, int if_index) {
	struct kif_addr* cur;
	// struct kif_addr* result = NULL; /* sado -- Greenhills compiler warning 550 - variable result set, but never used*/

	struct in_addr notfound;
	notfound.s_addr = 0;

	cur = kr_getaddr(NULL);
	while (cur) {
		if (cur->if_index == if_index) {
			struct in_addr ip_prefix;
			ip_prefix.s_addr = 
				(cur->addr.sin.sin_addr.s_addr &
				cur->mask.sin.sin_addr.s_addr);
			if (ip_prefix.s_addr == nw_prefix.s_addr) {
				// result = cur; /* sado -- Greenhills compiler warning 550 - variable result set, but never used*/
				break;
			}
		}

		cur = kr_getnextaddr(&(cur->addr.sa));
	}

	/* note: return by value, not by reference */
	return (cur?cur->addr.sin.sin_addr:notfound);
}


/* implements the ipRouteTable per RFC 1213 */

/* table oid index iterator
* Determine the oid that follows 'o' and write it to 'no'.
* The table index is the ipRouteDest (prefix) from the kroute table.
* Note that multiple kroute entries with the same prefix might exist,
* but only the first entry can be returned through snmp. This is
* realised in kroute_get_by_prefix() for efficiency reasons.
*/
static struct ber_oid *
obsd_snmpd_iproutetable_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	struct kroute *kr_next;
	unsigned int col;
	struct sockaddr_in row_addr;
	struct oid requested_oid, table_oid;

	/* 'no' always begins with oid base */
	bcopy(&oid->o_id, no, sizeof(*no));

	/* check if oid contains an element. */
	kr_next = kroute_first();
	col = oid->o_oid[OIDIDX_ipRouteColumn];
	bzero(&row_addr, sizeof(row_addr));
	/* pre-check length and skip actual check */
	if (o->bo_n >= oid->o_oidlen) {
		/* prepare compare buffers */
		bzero(&requested_oid, sizeof(requested_oid));
		bcopy(o, &requested_oid.o_id, sizeof(struct ber_oid));
		bzero(&table_oid, sizeof(table_oid));
		bcopy(&(oid->o_id), &table_oid.o_id, sizeof(struct ber_oid));
		table_oid.o_oidlen--; /* select tableEntry, not actual column */
		table_oid.o_flags |= OID_TABLE;

		if (smi_oid_cmp(&requested_oid, &table_oid) == 0) {
			/* 'o' is a valid table subkey of 'oid' */

			/* if 'o' is an exact match, iterate to the next value,
			 * otherwise just return the current value */
			int oid_is_exact = (o->bo_n == OIDIDX_ipRouteRow+4);

			/* parse current row from 'o' */
			mps_decodeinaddr(o, &row_addr.sin_addr,
			    OIDIDX_ipRouteRow);

			/* get the element,
			 * - either non-exact, non-next matching element
			 *   (for semi-defined OIDs)
			 * - or the element following an exact match
                         *   (for fully defined OIDs)
			 */
			kr_next = kroute_get_by_prefix(
				row_addr.sin_addr.s_addr,
				oid_is_exact, oid_is_exact);
		}
	}

	if (kr_next == NULL) {
		/* no more element to iterate to, supply invalid element */
		no->bo_id[OIDIDX_ipRouteColumn] = col;
		no->bo_id[OIDIDX_ipRouteRow] = 256;
		no->bo_n = OIDIDX_ipRouteRow+1;
	} else {
		/* encode kr_next to 'no' */
		no->bo_id[OIDIDX_ipRouteColumn] = col;
		mps_encodeinaddr(no, &(kr_next->prefix), OIDIDX_ipRouteRow);
		/* Note: cannot use smi_oidlen here as ip may be 0.0.0.0 */
		no->bo_n = OIDIDX_ipRouteRow+4;
	}

	return no;
}

static int
obsd_snmpd_iproutetable(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct sockaddr_in	row_prefix;
	struct kroute		*kr_result;
	struct in_addr		nexthop;
	int			routetype;
	int			routeproto;
	struct in_addr		mask;
	ulong			tickdiff;

	OBSD_UNUSED_ARG(oid);

	/* determine table end */
	if (o->bo_id[OIDIDX_ipRouteRow] == 256) {
		return 1;
	}

	/* get requested kroute entry */
	mps_decodeinaddr(o, &row_prefix.sin_addr, OIDIDX_ipRouteRow);
	kr_result = kroute_get_by_prefix(row_prefix.sin_addr.s_addr, 1, 0);

	if (kr_result == NULL) {
		return 1;
	}

	/* get requested column from routing table entry */
	*elm = ber_add_oid(*elm, o);

	if (*elm) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipRouteColumn]) {
		case 1:    /* ipRouteDest */
			*elm = obsd_snmpd_add_ipaddress(*elm, kr_result->prefix);
			break;
		case 2:    /* ipRouteIfIndex*/
			*elm = ber_add_integer(*elm, kr_result->if_index);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:    /* ipRouteMetric1 */
		case 4:    /* ipRouteMetric2 */
		case 5:    /* ipRouteMetric3 */
		case 6:    /* ipRouteMetric4 */
		case 12:   /* ipRouteMetric5 */
			*elm = ber_add_integer(*elm, -1);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 7:    /* ipRouteNextHop */
			nexthop = kr_result->nexthop;
			if (nexthop.s_addr == 0) {
				nexthop = obsd_snmpd_get_ip_on_if(
					kr_result->prefix, kr_result->if_index);
			}
			*elm = obsd_snmpd_add_ipaddress(*elm, nexthop);
			break;
		case 8:    /* ipRouteType 2=invalid, 3=direct, 4=indirect */
			if (kr_result->flags & (F_REJECT | F_BLACKHOLE))
				routetype = 2;
			else if (kr_result->flags & F_CONNECTED)
				routetype = 3;
			else
				routetype = 4;
			*elm = ber_add_integer(*elm, routetype);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 9:    /* ipRouteProto 2=local */
			if (kr_result->priority == RTP_CONNECTED)
				routeproto = 2;
			else if (kr_result->priority == RTP_STATIC)
				routeproto = 2;
			else if (kr_result->priority == RTP_OSPF)
				routeproto = 13;
			else if (kr_result->priority == RTP_ISIS)
				routeproto = 9;
			else if (kr_result->priority == RTP_RIP)
				routeproto = 8;
			else if (kr_result->priority == RTP_BGP)
				routeproto = 14;
			else if (kr_result->flags & F_DYNAMIC)
				routeproto = 4;
			else
				routeproto = 1;

			*elm = ber_add_integer(*elm, routeproto);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 10:   /* ipRouteAge */
			tickdiff = smi_getticks() - kr_result->ticks;
			if (tickdiff < 1)
				tickdiff = 1;
			*elm = ber_add_integer(*elm, (ulong)(tickdiff / (ulong)100));
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 11:   /* ipRouteMask */
			mask.s_addr = htonl(0xffffffff << (32 - kr_result->prefixlen));
			*elm = obsd_snmpd_add_ipaddress(*elm, mask);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 13:   /* ipRouteInfo */
			*elm = ber_add_oid(*elm, &obsd_snmpd_zerodotzero);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return -1;
		}
	}
	else
	{
		return -1;
	}

    return 0;
}

/* implements the UDP group from RFC 1213 */

static int
obsd_snmpd_udp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	/* get udp stats */
	struct udpstat udpstat;
	int mib[] = { CTL_NET, PF_INET, IPPROTO_UDP, UDPCTL_STATS };
	size_t len = sizeof(udpstat);

	OBSD_UNUSED_ARG(o); /* gh2289n: avoid compiler warning */

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]),
		&udpstat, &len, NULL, 0) < 0)
	{
		log_warnx("sysctl udp stats failed");
		return -1;
	}

	/* get the requested element */
	switch (oid->o_oid[OIDIDX_udpMIB]) {
	case 1:   /* udpInDatagrams */
		*elm = ber_add_integer(*elm, udpstat.udps_ipackets);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 2:   /* udNoPorts */
		/* sum noport counters for regular and broadcast datagrams */
		*elm = ber_add_integer(*elm,
			(udpstat.udps_noport + udpstat.udps_noportbcast));
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 3:   /* udpInErrors */
		/* sum up all in errors */
		*elm = ber_add_integer(*elm, 
			(udpstat.udps_hdrops + udpstat.udps_badlen + 
			 udpstat.udps_badsum + udpstat.udps_nosum +
			 udpstat.udps_fullsock + udpstat.udps_nosec));
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 4:   /* udpOutDatagrams */
		*elm = ber_add_integer(*elm, udpstat.udps_opackets);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	default:
		return -1;
	}

	return 0;
}

/* table oid index iterator
 * Determine the oid that follows 'o' and write it to 'no'. The table
 * index is the combined value of udpLocalAddress and udpLocalPort.
 */
static struct ber_oid *
obsd_snmpd_udp_table_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	/* obsd_snmpd_update_net_sockets();   gh2289n: function call before declarations is no ANSIC and leads to compiler error C2143 */

	struct in_addr address;
	uint16_t port = 0;
	struct obsd_snmpd_udp_listener *ul;
	struct oid requested_oid, table_oid;
	int col;

	obsd_snmpd_update_net_sockets();    /* gh2289n: moved to here, see above */

	/* 'no' always begins with oid base */
	bcopy(&oid->o_id, no, sizeof(*no));
	address.s_addr = 0;
	ul = obsd_snmpd_get_udp_listener(0, 0, 0, 0);  /* first element */

	/* check if oid contains an element. */
	col = oid->o_oid[OIDIDX_udpTableColumn];
	/* pre-check length and skip actual check */
	if (o->bo_n >= oid->o_oidlen) {
		/* prepare compare buffers */
		bzero(&requested_oid, sizeof(requested_oid));
		bcopy(o, &requested_oid.o_id, sizeof(struct ber_oid));
		bzero(&table_oid, sizeof(table_oid));
		bcopy(&(oid->o_id), &table_oid.o_id, sizeof(struct ber_oid));
		table_oid.o_oidlen--; /* select tableEntry, not actual column */
		table_oid.o_flags |= OID_TABLE;

		if (smi_oid_cmp(&requested_oid, &table_oid) == 0) {
			/* 'o' is a valid table subkey of 'oid' */

			/* if 'o' is an exact match, iterate to the next value,
			 * otherwise just return the current value */
			int oid_is_exact = (o->bo_n == OIDIDX_udpTableRow+5);

			/* parse current row from 'o' */
			mps_decodeinaddr(o, &address, OIDIDX_udpTableRow);
			port = obsd_snmpd_decode_port(o, OIDIDX_udpTableRow + 4);

			/* get the element,
			 * - either non-exact, non-next matching element
			 *   (for semi-defined OIDs)
			 * - or the element following an exact match
                         *   (for fully defined OIDs)
			 */
			ul = obsd_snmpd_get_udp_listener(
			    address.s_addr, port, oid_is_exact, oid_is_exact);
		}
	}

	if (ul == NULL) {
		/* no more element to iterate to, supply invalid element */
		no->bo_id[OIDIDX_udpTableColumn] = col;
		no->bo_id[OIDIDX_udpTableRow] = 256;
		no->bo_n = OIDIDX_udpTableRow+1;
	} else {
		/* encode ul to 'no' */
		no->bo_id[OIDIDX_ipRouteColumn] = col;
		address.s_addr = ul->address;
		mps_encodeinaddr(no, &address, OIDIDX_udpTableRow);
		obsd_snmpd_encode_port(no, OIDIDX_udpTableRow + 4, ul->port);
		/* Note: cannot use smi_oidlen; oid may contain trailing 0s */
		no->bo_n = OIDIDX_udpTableRow+5;
	}

	obsd_snmpd_close_net_sockets();

	return no;
}

static int
obsd_snmpd_udp_table(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct in_addr address;
	uint16_t port;
	struct obsd_snmpd_udp_listener *ul;

	OBSD_UNUSED_ARG(oid); /* gh2289n: avoid compiler warning */

	/* determine table end */
	if (o->bo_id[OIDIDX_udpTableRow] == 256) {
		return 1;
	}

	/* get requested udp listener entry */
	obsd_snmpd_update_net_sockets();
	mps_decodeinaddr(o, &address, OIDIDX_udpTableRow);
	port = obsd_snmpd_decode_port(o, OIDIDX_udpTableRow + 4);

	/* always get an exact match, always exactly the requested element */
	ul = obsd_snmpd_get_udp_listener(address.s_addr, port, 1, 0);

	if (ul == NULL) {
		/* no such element, table end */
		obsd_snmpd_close_net_sockets();
		return 1;
	}

	/* get requested column from routing table entry */
	*elm = ber_add_oid(*elm, o);

	if (*elm) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_udpTableColumn]) {
		case 1:   /* udpLocalAddress */
			address.s_addr = ul->address;
			*elm = obsd_snmpd_add_ipaddress(*elm, address);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 2:   /* udpLocalPort */
			*elm = ber_add_integer(*elm, ntohs(ul->port));
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			obsd_snmpd_close_net_sockets();
			return -1;
		}
	}
	else
	{
		return -1;
	}

	obsd_snmpd_close_net_sockets();
	return 0;
}

/* implements the ICMP group from RFC 1213 */

static int
obsd_snmpd_icmp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	/* get udp stats */
	struct icmpstat stat;
	int mib[] = { CTL_NET, PF_INET, IPPROTO_ICMP, ICMPCTL_STATS };
	size_t len = sizeof(stat);
	u_long sum;
	int i;

	OBSD_UNUSED_ARG(o); /* gh2289n: avoid compiler warning */

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]),
		&stat, &len, NULL, 0) < 0)
	{
		log_warnx("sysctl udp stats failed");
		return -1;
	}

	/* get the requested element */
	switch (oid->o_oid[OIDIDX_icmpMIB]) {
	case 1:   /* icmpInMsgs */
		sum =
		    stat.icps_badcode +
		    stat.icps_tooshort +
		    stat.icps_checksum +
		    stat.icps_badlen +
		    stat.icps_bmcastecho;
		for (i=0; i<=ICMP_MAXTYPE; i++) {
			sum += stat.icps_inhist[i];
		}
		*elm = ber_add_integer(*elm, sum);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 2:   /* icmpInErrors */
		sum =
		    stat.icps_badcode +
		    stat.icps_tooshort +
		    stat.icps_checksum +
		    stat.icps_badlen +
		    stat.icps_bmcastecho;
		*elm = ber_add_integer(*elm, sum);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 3:   /* icmpInDestUnreachs */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_UNREACH]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 4:   /* icmpInTimeExcds */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_TIMXCEED]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 5:   /* icmpInParmProbs */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_PARAMPROB]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 6:   /* icmpInSrcQuenchs */
		*elm = ber_add_integer(*elm,
			stat.icps_inhist[ICMP_SOURCEQUENCH]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 7:   /* icmpInRedirects */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_REDIRECT]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 8:   /* icmpInEchos */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_ECHO]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 9:   /* icmpInEchoReps */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_ECHOREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 10:  /* icmpInTimestamps */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_TSTAMP]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 11:  /* icmpInTimestampReps */
		*elm = ber_add_integer(*elm,stat.icps_inhist[ICMP_TSTAMPREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 12:  /* icmpInAddrMasks */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_MASKREQ]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 13:  /* icmpInAddrMaskReps */
		*elm = ber_add_integer(*elm, stat.icps_inhist[ICMP_MASKREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;

	case 14:  /* icmpOutMsgs */
		sum =
		    stat.icps_error +
		    stat.icps_oldshort +
		    stat.icps_oldicmp;
		for (i=0; i<=ICMP_MAXTYPE; i++) {
			sum += stat.icps_inhist[i];
		}
		*elm = ber_add_integer(*elm, sum);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 15:  /* icmpOutErrors */
		sum =
		    stat.icps_error +
		    stat.icps_oldshort +
		    stat.icps_oldicmp;
		*elm = ber_add_integer(*elm, sum);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 16:  /* icmpOutDestUnreachs */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_UNREACH]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 17:  /* icmpOutTimeExcds */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_TIMXCEED]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 18:  /* icmpOutParmProbs */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_PARAMPROB]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 19:  /* icmpOutSrcQuenchs */
		*elm = ber_add_integer(*elm,
		    stat.icps_outhist[ICMP_SOURCEQUENCH]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 20:  /* icmpOutRedirects */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_REDIRECT]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 21:  /* icmpOutEchos */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_ECHO]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 22:  /* icmpOutEchoReps */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_ECHOREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 23:  /* icmpOutTimestamps */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_TSTAMP]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 24:  /* icmpOutTimestampReps */
		*elm = ber_add_integer(*elm,
		    stat.icps_outhist[ICMP_TSTAMPREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 25:  /* icmpOutAddrMasks */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_MASKREQ]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 26:  /* icmpOutAddrMaskReps */
		*elm = ber_add_integer(*elm, stat.icps_outhist[ICMP_MASKREPLY]);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	default:
		return -1;
	}

	return 0;
}

/* implements the TCP group from RFC 1213 */

static int
obsd_snmpd_tcp_scalar(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_long sum;
	int cnt_estab;

	/* get tcp stats */
	struct tcpstat stat;
	int mib[] = { CTL_NET, AF_INET, IPPROTO_TCP, TCPCTL_STATS };
	size_t len = sizeof(stat);

    int res = sysctl(mib, sizeof(mib) / sizeof(mib[0]), &stat, &len, NULL, 0);

	OBSD_UNUSED_ARG(o);
	OBSD_UNUSED_ARG(oid);

	if (res < 0)
	{
		log_warnx("sysctl tcp stats failed");
		return -1;
	}

	/* get the requested element */
	switch (oid->o_oid[OIDIDX_icmpMIB]) {
	case 1:   /* tcp retransmission timeout algorithm */
		*elm = ber_add_integer(*elm, 1); /* other = 1 */
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 2:   /* tcp retransmission timeout minimum value */
		*elm = ber_add_integer(*elm, TCPTV_MIN*500); /* TCPTV_MIN 2*500ms */
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 3:   /* tcp retransmission timeout maximum value */
		*elm = ber_add_integer(*elm, TCPTV_REXMTMAX*1000); /* TCPTV_REXMTMAX 64*1s */
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 4:   /* tcpMaxConn */
		*elm = ber_add_integer(*elm, -1); /* unlimited = -1 */
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 5:   /* tcpActiveOpens */
		*elm = ber_add_integer(*elm, stat.tcps_connattempt);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 6:   /* tcpPassiveOpens */
		*elm = ber_add_integer(*elm, stat.tcps_accepts);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 7:   /* tcpAttemptFails */
		  /* Note: tcps_conndrops is not entirely correct, but the
		     best-matching value for tcpAttemptFails */
		*elm = ber_add_integer(*elm, stat.tcps_conndrops);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 8:   /* tcpEstabResets */
		  /* Note: tcps_drops is not entirely correct, but the
		     best-matching value for tcpEstabResets */
		*elm = ber_add_integer(*elm, stat.tcps_drops);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 9:   /* tcpCurrEstab */
		obsd_snmpd_update_net_sockets();
		cnt_estab = obsd_snmpd_count_tcp_established();
		*elm = ber_add_integer(*elm, cnt_estab);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
		obsd_snmpd_close_net_sockets();
		break;
	case 10:  /* tcpInSegs */
		*elm = ber_add_integer(*elm, stat.tcps_rcvtotal);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 11:  /* tcpOutSegs */
		*elm = ber_add_integer(*elm, 
		    (stat.tcps_sndtotal - stat.tcps_sndrexmitpack) );
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 12:  /* tcpReTransSegs */
		*elm = ber_add_integer(*elm, stat.tcps_sndrexmitpack);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	/* Note: elem 13 is a table which is implemented in another function */
	case 14:  /* tcpInErrs */
		sum = /* sum up all possibe errors */
		    stat.tcps_rcvbadsum +
		    stat.tcps_rcvbadoff +
		    stat.tcps_rcvmemdrop +
		    stat.tcps_rcvnosec +
		    stat.tcps_rcvshort;
		*elm = ber_add_integer(*elm, sum);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;
	case 15:  /* tcpOutRsts */
		  /* number of segments sent that contain the RST flag */
		*elm = ber_add_integer(*elm, stat.tcps_sndrst);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
		break;

	default:
		return -1;
	}

	return 0;
}

/* table oid index iterator
 * Determine the oid that follows 'o' and write it to 'no'. The table
 * index is the combined value of tcpConnLocalAddress, tcpConnLocalPort,
 * tcpConnRemoteAddress and tcpConnRemotePort.
 */
static struct ber_oid *
obsd_snmpd_tcp_table_idx(
	struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	struct in_addr local_address;
	uint16_t       local_port = 0;
	struct in_addr remote_address;
	uint16_t       remote_port = 0;
	struct obsd_snmpd_tcp_connection *tc;
	struct oid requested_oid, table_oid;
	int col;

	obsd_snmpd_update_net_sockets();

	/* 'no' always begins with oid base */
	bcopy(&oid->o_id, no, sizeof(*no));
	local_address.s_addr = 0;
	remote_address.s_addr = 0;
	tc = obsd_snmpd_get_tcp_connection(0, 0, 0, 0, 0, 0); /* first elem */

	/* check if oid contains an element. */
	col = oid->o_oid[OIDIDX_tcpConnTableColumn];
	/* pre-check length and skip actual check */
	if (o->bo_n >= oid->o_oidlen) {
		/* prepare compare buffers */
		bzero(&requested_oid, sizeof(requested_oid));
		bcopy(o, &requested_oid.o_id, sizeof(struct ber_oid));
		bzero(&table_oid, sizeof(table_oid));
		bcopy(&(oid->o_id), &table_oid.o_id, sizeof(struct ber_oid));
		table_oid.o_oidlen--; /* select tableEntry, not actual column */
		table_oid.o_flags |= OID_TABLE;

		if (smi_oid_cmp(&requested_oid, &table_oid) == 0) {
			/* 'o' is a valid table subkey of 'oid' */

			/* if 'o' is an exact match, iterate to the next value,
			 * otherwise just return the current value */
			int oid_is_exact =
			    (o->bo_n == (OIDIDX_tcpConnTableRow+10));

			/* parse current row from 'o' */
			mps_decodeinaddr(o, &local_address,
			    OIDIDX_tcpConnTableRow);
			local_port = obsd_snmpd_decode_port(o,
			    OIDIDX_tcpConnTableRow + 4);
			mps_decodeinaddr(o, &remote_address,
			    OIDIDX_tcpConnTableRow + 5);
			remote_port = obsd_snmpd_decode_port(o,
			    OIDIDX_tcpConnTableRow + 9);

			/* get the element,
			 * - either non-exact, non-next matching element
			 *   (for semi-defined OIDs)
			 * - or the element following an exact match
                         *   (for fully defined OIDs)
			 */
			tc = obsd_snmpd_get_tcp_connection(
			    local_address.s_addr, local_port, 
			    remote_address.s_addr, remote_port,
			    oid_is_exact, oid_is_exact);
		}
	}

	if (tc == NULL) {
		/* no more element to iterate to, supply next scalar OID */
		no->bo_id[OIDIDX_tcpConnTableColumn] = col;
		no->bo_id[OIDIDX_tcpConnTableRow] = 256;
		no->bo_n = OIDIDX_tcpConnTableRow+1;
	} else {
		/* encode tc to 'no' */
		no->bo_id[OIDIDX_ipRouteColumn] = col;
		local_address.s_addr = tc->local_address;
		remote_address.s_addr = tc->remote_address;
		mps_encodeinaddr(no, &local_address, OIDIDX_tcpConnTableRow);
		obsd_snmpd_encode_port(no,
		    OIDIDX_tcpConnTableRow + 4, tc->local_port);
		mps_encodeinaddr(no, &remote_address, OIDIDX_tcpConnTableRow+5);
		obsd_snmpd_encode_port(no,
		    OIDIDX_tcpConnTableRow + 9, tc->remote_port);
		/* Note: cannot use smi_oidlen; oid may contain trailing 0s */
		no->bo_n = OIDIDX_tcpConnTableRow+10;
	}

	obsd_snmpd_close_net_sockets();

	return no;
}

/* convert the obsd tcp state value to the tcp state values
 * specified in RFC1213 */
static int
obsd_snmpd_obsdstate_to_rfcstate(int obsdstate) {
	switch(obsdstate) {
	case TCPS_CLOSED:
		return 1;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_LISTEN:
		return 2;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_SYN_SENT:
		return 3;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_SYN_RECEIVED:
		return 4;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_ESTABLISHED:
		return 5;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_CLOSE_WAIT:
		return 8;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_FIN_WAIT_1:
		return 6;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_CLOSING:
		return 10;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_LAST_ACK:
		return 9;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_FIN_WAIT_2:
		return 7;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	case TCPS_TIME_WAIT:
		return 11;
		/* sado -- omit break; Greenhills compiler warning - statement unreachable */
	default:
		/* state unknown, should never happen */
		return 0;
	}
}

static int
obsd_snmpd_tcp_table(
	struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct in_addr local_address;
	uint16_t       local_port;
	struct in_addr remote_address;
	uint16_t       remote_port;
	struct obsd_snmpd_tcp_connection *tc;

	OBSD_UNUSED_ARG(oid);

	/* determine table end */
	if (o->bo_id[OIDIDX_tcpConnTableRow] == 256) {
		return 1;
	}

	/* get requested udp listener entry */
	obsd_snmpd_update_net_sockets();
	mps_decodeinaddr(o, &local_address, OIDIDX_tcpConnTableRow);
	local_port = obsd_snmpd_decode_port(o, OIDIDX_tcpConnTableRow + 4);
	mps_decodeinaddr(o, &remote_address, OIDIDX_tcpConnTableRow + 5);
	remote_port = obsd_snmpd_decode_port(o, OIDIDX_tcpConnTableRow + 9);

	tc = obsd_snmpd_get_tcp_connection(
	    local_address.s_addr, local_port,
	    remote_address.s_addr, remote_port,
	    1, 0);

	if (tc == NULL) {
		/* no such element, table end */
		obsd_snmpd_close_net_sockets();
		return 1;
	}

	/* get requested column from routing table entry */
	*elm = ber_add_oid(*elm, o);

	if (*elm)
	{
		switch (o->bo_id[OIDIDX_tcpConnTableColumn]) {
		case 1:   /* tcpConnState */
			*elm = ber_add_integer(*elm,
				obsd_snmpd_obsdstate_to_rfcstate(tc->state));
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 2:   /* tcpConnLocalAddr */
			local_address.s_addr = tc->local_address;
			*elm = obsd_snmpd_add_ipaddress(*elm, local_address);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:   /* tcpConnLocalPort */
			*elm = ber_add_integer(*elm, ntohs(tc->local_port));
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 4:   /* tcpConnRemoteAddr */
			remote_address.s_addr = tc->remote_address;
			*elm = obsd_snmpd_add_ipaddress(*elm, remote_address);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 5:   /* tcpConnRemotePort */
			*elm = ber_add_integer(*elm, ntohs(tc->remote_port));
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			obsd_snmpd_close_net_sockets();
			return -1;
		}
	}
	else
	{
		return -1;
	}

	obsd_snmpd_close_net_sockets();
	return 0;
}

void
obsd_snmpd_mib_addon_rfc1213_init(void)
{
	/* register mib declaration */
	smi_mibtree(obsd_snmpd_mib_addon_rfc1213_tree);

	/* register mib */
	smi_mibtree(obsd_snmpd_mib_addon_rfc1213_implementation);
}

