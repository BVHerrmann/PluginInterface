#define _KERNEL

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_socket.h>
#include <net/obsd_kernel_if_arp.h>
#include <net/obsd_kernel_if.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_if_ether.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <lib/libkern/obsd_kernel_libkern.h>
#include <obsd_kernel_usr.h>

PACKET obsd_pnio_get_next_pkt(PACKET pkt)
{
	struct mbuf* my_pkt = (struct mbuf *)pkt;
	return (PACKET)my_pkt->m_nextpkt;
}

void obsd_pnio_set_next_pkt(PACKET pkt, PACKET next)
{
	struct mbuf* my_pkt = (struct mbuf *)pkt;
	my_pkt->m_nextpkt = (struct mbuf *)next;
}

void obsd_pnio_free_packet(PACKET pkt)
{
	struct mbuf *my_pkt = (struct mbuf *)pkt;
	m_freem(my_pkt);
}

int obsd_pnio_copy_pkt_to_sendbuffer(PACKET pkt, char *sendBuf)
{
	struct mbuf *my_pkt = (struct mbuf *)pkt;
	int sendLen = my_pkt->m_pkthdr.len;

	if (sendLen > OBSD_PNIO_SENDBUFFER_SIZE)
	{
		sendLen = ETHER_HDR_LEN; // note: must not "return" here
	}

	m_copydata(my_pkt, 0, sendLen, (caddr_t)sendBuf);

	m_freem(my_pkt);
	my_pkt = 0; /* sanity */

	return sendLen;
}

char obsd_pnio_get_protocol_flags(char *sendBuf)
{
	struct ether_header *eth_header = (struct ether_header*)sendBuf;
	int	ethType = ntohs(eth_header->ether_type);
	int ipProt;
	char result = 0;
	
	if ( ethType == ETHERTYPE_IP ) /* IP message */
	{
		result |= OBSD_PNIO_IP;
		ipProt = ((struct ip*)(sendBuf + ETHER_HDR_LEN))->ip_p;

		if(ipProt == IPPROTO_UDP)
		{
			result |= OBSD_PNIO_UDP;
		}
		else if (ipProt == IPPROTO_TCP)
		{
			result |= OBSD_PNIO_TCP;
		}
	}

	return result;
}

char *obsd_copy_pkt_from_recv_buffer(OBSD_IFNET ifp, char *buffer, int len)
{
	struct mbuf *m = m_devget(buffer, len, 0, (struct ifnet *)ifp, 0/*copy-function*/);
	return (char *)m;
}

#if OBSD_PNIO_CFG_TRACE_ON
void controller_add_recv_measure_2(void);
#endif /* OBSD_PNIO_CFG_TRACE_ON */

#if OBSD_PNIO_CFG_TRACE_ON
void  obsd_pnio_ether_input(OBSD_IFNET ifp, char *buffer, int to_count)
#else
void obsd_pnio_ether_input(OBSD_IFNET ifp, char *buffer)
#endif /* OBSD_PNIO_CFG_TRACE_ON */
{
	if (buffer != NULL)
	{
		#if OBSD_PNIO_CFG_TRACE_ON
			/* Messpunkt 2 (UDP Frame given to OBSD) */
			if (to_count)
			{
				controller_add_recv_measure_2();
			}
		#endif /* OBSD_PNIO_CFG_TRACE_ON */

		((struct ifnet *)ifp)->if_ipackets++;
		ether_input((struct ifnet *)ifp, 0, (struct mbuf *)buffer);
	}
	else
	{
		/* OBSD_ITGR -- increase dropped packets count */
		((struct ifnet *)ifp)->if_iqdrops++;
	}

}

#ifdef OBSD_MEMORY_TRACING
void tcip_trace_statistics(void)
{
	pool_statistic();
}
#endif /* OBSD_MEMORY_TRACING */

unsigned short obsd_pnio_ntohs(unsigned short x)
{
	NTOHS(x);
	return x;
}

unsigned int obsd_pnio_ntohl(unsigned int x)
{
	NTOHL(x);
	return x;
}

unsigned short obsd_pnio_htons(unsigned short x)
{
	HTONS(x);
	return x;
}

unsigned int obsd_pnio_htonl(unsigned int x)
{
	HTONL(x);
	return x;
}
