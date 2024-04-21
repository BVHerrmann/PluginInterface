#ifndef __OBSD_USERLAND_PNIO_IF_FUNCS_H__
#define __OBSD_USERLAND_PNIO_IF_FUNCS_H__

extern int obsd_pnio_create_loopback_interface(void);
extern int obsd_pnio_delete_loopback_interface(void);
extern int   obsd_pnio_create_interface(void);
extern OBSD_IFNET obsd_pnio_find_free_if_and_activate(void *arp_channel, int *MacAddress, int ifAdminStatus);
extern int   obsd_pnio_deactivate_interface(OBSD_IFNET ifp);
extern int   obsd_pnio_delete_all_interfaces(void);
extern int obsd_pnio_set_carp_step1(OBSD_IFNET ifp, unsigned char vhid);
extern int obsd_pnio_reset_carp_step1(OBSD_IFNET ifp);
extern int obsd_pnio_set_carp_step2(OBSD_IFNET ifp, unsigned long cluster_ip_address, unsigned long cluster_ip_subnet_mask);
extern int obsd_pnio_reset_carp_step2(OBSD_IFNET ifp);
extern int obsd_pnio_set_carp_role(OBSD_IFNET ifp, unsigned int enable);
extern int obsd_pnio_get_carp_role(OBSD_IFNET ifp);
extern void *obsd_pnio_get_tcip_channel(OBSD_IFNET ifp);
extern OBSD_IFNET obsd_pnio_get_carp_if(OBSD_IFNET ifp);
extern	char *obsd_pnio_get_if_name(OBSD_IFNET ifp);
extern char *obsd_pnio_generate_carp_ifDescr(char *ifDescr, int length);
extern int   obsd_pnio_get_if_index(OBSD_IFNET ifp);
extern char *obsd_pnio_get_if_description(OBSD_IFNET ifp);
extern void  obsd_pnio_set_if_description(OBSD_IFNET ifp, char *ifDescr, int ifDescr_len);
extern void  obsd_pnio_set_link_up(OBSD_IFNET ifp);
extern void  obsd_pnio_set_link_down(OBSD_IFNET ifp);
extern void  obsd_pnio_set_baudrate(OBSD_IFNET ifp, long baudrate);

extern void obsd_pnio_set_statistic_counters(OBSD_IFNET ifp, 
								long inOctets, 
								long inUcastPkts, 
								long inNUcastPkts, 
								long inDiscards, 
								long inErrors, 
								long inUnknownProtos, 
								long outOctets, 
								long outUcastPkts, 
								long outNUcastPkts, 
								long outDiscards, 
								long outErrors, 
								long outQLen, 
								unsigned long supportedCounters);

/* carp status */
#define CARP_NEW_REQ 1
#define CARP_IP_STACK_DONE 2
#define CARP_MULTICAST_DONE 3
#define CARP_FILTER_DONE 4

int obsd_pnio_set_cluster_ip(OBSD_IFNET ifp, unsigned int interface_id, unsigned int cluster_ip, unsigned int cluster_subnet_mask, unsigned char cluster_id);

#endif
