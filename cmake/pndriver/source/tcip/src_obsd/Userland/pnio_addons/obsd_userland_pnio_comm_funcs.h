#ifndef __OBSD_USERLAND_PNIO_COMM_FUNCS_H__
#define __OBSD_USERLAND_PNIO_COMM_FUNCS_H__

#define OBSD_PNIO_IP 1
#define OBSD_PNIO_UDP 2
#define OBSD_PNIO_TCP 4

/* Functions are needed in the EDD Send Queue, Packets are here at the moment queued by their own "next" field */
extern PACKET obsd_pnio_get_next_pkt(PACKET pkt);
extern void	  obsd_pnio_set_next_pkt(PACKET pkt, PACKET next);
extern void	  obsd_pnio_free_packet(PACKET pkt);
extern int	  obsd_pnio_copy_pkt_to_sendbuffer(PACKET pkt, char *sendBuf);
extern char   obsd_pnio_get_protocol_flags(char *sendBuf);

extern char *obsd_copy_pkt_from_recv_buffer(OBSD_IFNET ifp, char *buffer, int len);
#if OBSD_PNIO_CFG_TRACE_ON
extern void  obsd_pnio_ether_input(OBSD_IFNET ifp, char *buffer, int to_count); /* OBSD receive */
#else
extern void  obsd_pnio_ether_input(OBSD_IFNET ifp, char *buffer); /* OBSD receive */
#endif /* OBSD_PNIO_CFG_TRACE_ON */

#ifdef OBSD_MEMORY_TRACING
extern void tcip_trace_statistics(void);
#endif /* OBSD_MEMORY_TRACING */

extern unsigned short obsd_pnio_ntohs(unsigned short x);
extern unsigned int   obsd_pnio_ntohl(unsigned int x);
extern unsigned short obsd_pnio_htons(unsigned short x);
extern unsigned int   obsd_pnio_htonl(unsigned int x);

#endif
