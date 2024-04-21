/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_mgmt_funcs.h
 *
 * @brief       interface of the PNIO mini functions for interface and default route management
 * @details     Interface of the mini management functions
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.05
 * @date        created: 28.05.2013
 */
/* -------------------------------------------------------------------------------------------------- */


#ifndef __OBSD_USERLAND_PNIO_MGMT_FUNCS_H__
#define __OBSD_USERLAND_PNIO_MGMT_FUNCS_H__

extern int obsd_netif_set_ipv4addr(const char *ifname, unsigned long ipv4_address, unsigned long ipv4_subnet_mask);
extern int obsd_netif_del_ipv4addr(const char *ifname);

extern int obsd_netif_set_admin_state(const char *ifname, int up_or_down);
extern int obsd_netif_set_carp_role(const char *ifcarp, unsigned int is_primary);
extern int obsd_netif_get_carp_role(const char *ifcarp, int* out_cr);

extern int obsd_netif_create_if(const char *ifname);
extern int obsd_netif_create_carpif(const char *, unsigned char, const char *);
extern int obsd_netif_destroy_if(const char *ifname);

extern int obsd_pnio_add_route(unsigned long ipv4_address, unsigned long ipv4_mask, unsigned long ipv4_gateway);
extern int obsd_pnio_del_route(unsigned long ipv4_address, unsigned long ipv4_mask);
extern int obsd_pnio_set_max_socket_number(int max_sockets);
extern int obsd_pnio_set_ip_portrange_high(int sock_fd);

extern int obsd_set_ipv4forwarding(unsigned int enable);
extern int obsd_get_ipv4forwarding(unsigned int *out_is_enabled);

extern int obsd_pnio_get_errno(void);

extern void print_routing_table(void);

extern void obsd_execute_callback(void *soc, int flag);
extern int obsd_get_event_list_index(void *soc);
extern void obsd_set_event_list_index(void *soc, int index);
extern int obsd_has_callback_fct(void *soc);

#endif /* __OBSD_USERLAND_PNIO_MGMT_FUNCS_H__ */

