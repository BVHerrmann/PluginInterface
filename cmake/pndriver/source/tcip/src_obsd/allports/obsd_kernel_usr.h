/* ------------------Kernel Function Prototypes used in SOCK and TCIP -------------------- */
/*                                                                                         */
/* SOCK and TCIP are not part of the kernel, therefore the _KERNEL definition is not set   */
/* In order to use some of the Kernel-Functions of OpenBSD, the function prototypes have   */
/* to be declared here.                                                                    */
/*                                                                                         */
/*-----------------------------------------------------------------------------------------*/
#ifndef   __OBSD_KERNEL_USR_H__
#define   __OBSD_KERNEL_USR_H__

/* sado - internal performance tracing */
#define OBSD_PNIO_CFG_TRACE_ON 0 /* 1 enables performance trace*/
#undef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM /* enable tracing of socket buffer histogram */
#undef OBSD_MEMORY_TRACING /* enables obsd-memory tracing */
#define OBSD_MEMORY_TRACING_DETAILS 0

#define INET
#include <tcip_obsd_cfg.h>
#include <obsd_kernel_kernelport.h>
#include <obsd_platform_osal.h>
#include <obsd_platform_hal.h>
#include <obsd_platform_mibal.h>
#include <obsd_snmpd_snmpdext.h>

typedef void *PACKET; // without OBSD_ prefix...compatibility to interniche
typedef void *OBSD_IFNET;

#include <obsd_userland_pnio_types_def.h> /* this one goes first -- types and definitions */
#include <obsd_userland_pnio_mgmt_funcs.h>
#include <obsd_userland_pnio_comm_funcs.h>
#include <obsd_userland_pnio_if_funcs.h>
#include <obsd_userland_pnio_tcpparams.h>
#include <obsd_userland_pnio_recvsndfromto.h>

void tcp_close_timewait_connections(void);

/* Task 1560385 (use if_sendq) -- OpenBSD routines */
void tcip_obsd_pkt_send(const void *channel, void *pkt, void *pRQB); /* OBSD send */
void *tcip_obsd_get_rqb(OBSD_IFNET ifp); /* get Send RQB */
void tcip_obsd_put_rqb(void *channel, void *pRQB);
void obsd_send_next_packet(OBSD_IFNET ifp, void *pRQB);
void obsd_pnio_change_carp_state(int event, int error);
void tcip_edd_srv_arp_filter(void *ch, unsigned char user_index, unsigned long cluster_ip_address);
void tcip_edd_set_carp_multicast(void *channel, unsigned long long mac_address);
void tcip_edd_reset_carp_multicast(void *channel, unsigned long long mac_address);

#define TCIP_CARP_USER_INDEX 1
#define TCIP_ARP_USER_INDEX 0

void TCIP_SRV_CLUSTER_IP_CONFIG_SET_DONE(unsigned int interface_id, int result);
void TCIP_SRV_CLUSTER_IP_ADDR_CONFLICT_DETECTED(unsigned int address);

#endif

