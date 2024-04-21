#ifndef _OBSD_SNMPD_MIB_ADDON_RFC1213_H
#define _OBSD_SNMPD_MIB_ADDON_RFC1213_H

#include "obsd_snmpd_mib.h"

/**
 * Extension to base mib in mib.c
 * Implements the following additional values as defined per RFC 1213: 
 *   ipRouteTable from IP MIB  (1.3.6.1.2.1.4.21)
 *   udp*         from UDP MIB (1.3.6.1.2.1.7)
 */

/* ipRoute group */
#define MIB_ipRouteTable		MIB_ipMIB, 21
#define OIDIDX_ipRouteColumn		9
#define OIDIDX_ipRouteRow		10
#define MIB_ipRouteEntry		MIB_ipRouteTable, 1
#define MIB_ipRouteDesc 		MIB_ipRouteEntry, 1
#define MIB_ipRouteIfIndex		MIB_ipRouteEntry, 2
#define MIB_ipRouteMetric1		MIB_ipRouteEntry, 3
#define MIB_ipRouteMetric2		MIB_ipRouteEntry, 4
#define MIB_ipRouteMetric3		MIB_ipRouteEntry, 5
#define MIB_ipRouteMetric4		MIB_ipRouteEntry, 6
#define MIB_ipRouteNextHop		MIB_ipRouteEntry, 7
#define MIB_ipRouteType 		MIB_ipRouteEntry, 8
#define MIB_ipRouteProto 		MIB_ipRouteEntry, 9
#define MIB_ipRouteAge  		MIB_ipRouteEntry, 10
#define MIB_ipRouteMask  		MIB_ipRouteEntry, 11
#define MIB_ipRouteMetric5		MIB_ipRouteEntry, 12
#define MIB_ipRouteInfo 		MIB_ipRouteEntry, 13

/* UDP group */
#define MIB_udpMIB                      MIB_mib_2, 7
#define OIDIDX_udpMIB                   7
#define MIB_udpInDatagrams              MIB_udpMIB, 1
#define MIB_udpNoPorts                  MIB_udpMIB, 2
#define MIB_udpInErrors                 MIB_udpMIB, 3
#define MIB_udpOutDatagrams             MIB_udpMIB, 4
#define MIB_udpTable                    MIB_udpMIB, 5
#define OIDIDX_udpTableColumn           9
#define OIDIDX_udpTableRow              10
#define MIB_udpTableEntry               MIB_udpTable, 1
#define MIB_udpLocalAddress             MIB_udpTableEntry, 1
#define MIB_udpLocalPort                MIB_udpTableEntry, 2

/* ICMP group */
#define MIB_icmpMIB                     MIB_mib_2, 5
#define OIDIDX_icmpMIB                  7
#define MIB_icmpInMsgs                  MIB_icmpMIB, 1
#define MIB_icmpInErrors                MIB_icmpMIB, 2
#define MIB_icmpInDestUnreachs          MIB_icmpMIB, 3
#define MIB_icmpInTimeExcds             MIB_icmpMIB, 4
#define MIB_icmpInParmProbs             MIB_icmpMIB, 5
#define MIB_icmpInSrcQuenchs            MIB_icmpMIB, 6
#define MIB_icmpInRedirects             MIB_icmpMIB, 7
#define MIB_icmpInEchos                 MIB_icmpMIB, 8
#define MIB_icmpInEchoReps              MIB_icmpMIB, 9
#define MIB_icmpInTimestamps            MIB_icmpMIB, 10
#define MIB_icmpInTimestampReps         MIB_icmpMIB, 11
#define MIB_icmpInAddrMasks             MIB_icmpMIB, 12
#define MIB_icmpInAddrMaskReps          MIB_icmpMIB, 13
#define MIB_icmpOutMsgs                 MIB_icmpMIB, 14
#define MIB_icmpOutErrors               MIB_icmpMIB, 15
#define MIB_icmpOutDestUnreachs         MIB_icmpMIB, 16
#define MIB_icmpOutTimeExcds            MIB_icmpMIB, 17
#define MIB_icmpOutParmProbs            MIB_icmpMIB, 18
#define MIB_icmpOutSrcQuenchs           MIB_icmpMIB, 19
#define MIB_icmpOutRedirects            MIB_icmpMIB, 20
#define MIB_icmpOutEchos                MIB_icmpMIB, 21
#define MIB_icmpOutEchoReps             MIB_icmpMIB, 22
#define MIB_icmpOutTimestamps           MIB_icmpMIB, 23
#define MIB_icmpOutTimestampReps        MIB_icmpMIB, 24
#define MIB_icmpOutAddrMasks            MIB_icmpMIB, 25
#define MIB_icmpOutAddrMaskReps         MIB_icmpMIB, 26

/* TCP group */
#define MIB_tcpMIB                      MIB_mib_2, 6
#define OIDIDX_tcpMIB                   7
#define MIB_tcpRtoAlgorithm             MIB_tcpMIB, 1
#define MIB_tcpRtoMin                   MIB_tcpMIB, 2
#define MIB_tcpRtoMax                   MIB_tcpMIB, 3
#define MIB_tcpMaxConn                  MIB_tcpMIB, 4
#define MIB_tcpActiveOpens              MIB_tcpMIB, 5
#define MIB_tcpPassiveOpens             MIB_tcpMIB, 6
#define MIB_tcpAttemptFails             MIB_tcpMIB, 7
#define MIB_tcpEstabResets              MIB_tcpMIB, 8
#define MIB_tcpCurrEstab                MIB_tcpMIB, 9
#define MIB_tcpInSegs                   MIB_tcpMIB, 10
#define MIB_tcpOutSegs                  MIB_tcpMIB, 11
#define MIB_tcpRetransSegs              MIB_tcpMIB, 12
#define MIB_tcpInErrs                   MIB_tcpMIB, 14
#define MIB_tcpOutErrs                  MIB_tcpMIB, 15

#define MIB_tcpConnTable                MIB_tcpMIB, 13
#define OIDIDX_tcpConnTableColumn       9
#define OIDIDX_tcpConnTableRow          10
#define MIB_tcpConnEntry		MIB_tcpConnTable, 1
#define MIB_tcpConnState		MIB_tcpConnEntry, 1
#define MIB_tcpConnLocalAddress		MIB_tcpConnEntry, 2
#define MIB_tcpConnLocalPort		MIB_tcpConnEntry, 3
#define MIB_tcpConnRemAddress		MIB_tcpConnEntry, 4
#define MIB_tcpConnRemPort		MIB_tcpConnEntry, 5



#define MIB_ADDON_RFC1213_TREE		    {		\
	{ MIBDECL(ipRouteTable) },			\
	{ MIBDECL(ipRouteEntry) },			\
	{ MIBDECL(ipRouteDesc) },			\
	{ MIBDECL(ipRouteIfIndex) },			\
	{ MIBDECL(ipRouteMetric1) },			\
	{ MIBDECL(ipRouteMetric2) },			\
	{ MIBDECL(ipRouteMetric3) },			\
	{ MIBDECL(ipRouteMetric4) },			\
	{ MIBDECL(ipRouteNextHop) },			\
	{ MIBDECL(ipRouteType) },			\
	{ MIBDECL(ipRouteProto) },			\
	{ MIBDECL(ipRouteAge) },			\
	{ MIBDECL(ipRouteMask) },			\
	{ MIBDECL(ipRouteMetric5) },			\
	{ MIBDECL(ipRouteInfo) },			\
	{ MIBDECL(udpMIB) },				\
	{ MIBDECL(udpInDatagrams) },			\
	{ MIBDECL(udpNoPorts) },			\
	{ MIBDECL(udpInErrors) },			\
	{ MIBDECL(udpOutDatagrams) },			\
	{ MIBDECL(udpTable) },				\
	{ MIBDECL(udpTableEntry) },			\
	{ MIBDECL(udpLocalAddress) },			\
	{ MIBDECL(udpLocalPort) },			\
	{ MIBDECL(icmpMIB) },				\
	{ MIBDECL(icmpInMsgs) },			\
	{ MIBDECL(icmpInErrors) },			\
	{ MIBDECL(icmpInDestUnreachs) },		\
	{ MIBDECL(icmpInTimeExcds) },			\
	{ MIBDECL(icmpInParmProbs) },			\
	{ MIBDECL(icmpInSrcQuenchs) },			\
	{ MIBDECL(icmpInRedirects) },			\
	{ MIBDECL(icmpInEchos) },			\
	{ MIBDECL(icmpInEchoReps) },			\
	{ MIBDECL(icmpInTimestamps) },			\
	{ MIBDECL(icmpInTimestampReps) },		\
	{ MIBDECL(icmpInAddrMasks) },			\
	{ MIBDECL(icmpInAddrMaskReps) },		\
	{ MIBDECL(icmpOutMsgs) },			\
	{ MIBDECL(icmpOutErrors) },			\
	{ MIBDECL(icmpOutDestUnreachs) },		\
	{ MIBDECL(icmpOutTimeExcds) },			\
	{ MIBDECL(icmpOutParmProbs) },			\
	{ MIBDECL(icmpOutSrcQuenchs) },			\
	{ MIBDECL(icmpOutRedirects) },			\
	{ MIBDECL(icmpOutEchos) },			\
	{ MIBDECL(icmpOutEchoReps) },			\
	{ MIBDECL(icmpOutTimestamps) },			\
	{ MIBDECL(icmpOutTimestampReps) },		\
	{ MIBDECL(icmpOutAddrMasks) },			\
	{ MIBDECL(icmpOutAddrMaskReps) },		\
	{ MIBDECL(tcpMIB) },				\
	{ MIBDECL(tcpRtoAlgorithm) },			\
	{ MIBDECL(tcpRtoMin) },				\
	{ MIBDECL(tcpRtoMax) },				\
	{ MIBDECL(tcpMaxConn) },			\
	{ MIBDECL(tcpActiveOpens) },			\
	{ MIBDECL(tcpPassiveOpens) },			\
	{ MIBDECL(tcpAttemptFails) },			\
	{ MIBDECL(tcpEstabResets) },			\
	{ MIBDECL(tcpCurrEstab) },			\
	{ MIBDECL(tcpInSegs) },				\
	{ MIBDECL(tcpOutSegs) },			\
	{ MIBDECL(tcpRetransSegs) },			\
	{ MIBDECL(tcpInErrs) },				\
	{ MIBDECL(tcpOutErrs) },			\
	{ MIBDECL(tcpConnTable) },			\
	{ MIBDECL(tcpConnEntry) },			\
	{ MIBDECL(tcpConnState) },			\
	{ MIBDECL(tcpConnLocalAddress) },		\
	{ MIBDECL(tcpConnLocalPort) },			\
	{ MIBDECL(tcpConnRemAddress) },			\
	{ MIBDECL(tcpConnRemPort) },			\
	{ MIBEND }					\
}

void	 obsd_snmpd_mib_addon_rfc1213_init(void);

#endif /* _OBSD_SNMPD_MIB_ADDON_RFC1213_H */
