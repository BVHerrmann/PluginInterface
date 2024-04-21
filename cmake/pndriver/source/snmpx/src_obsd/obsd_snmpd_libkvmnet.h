/**
 * libkvmnet.h
 * Provides access to the network (listen) sockets through libkvm.
 * Implementation highly depends on the openbsd version and is likely
 * to change in the future.
 */

#ifndef _OBSD_SNMPD_NET_SOCKETS_H
#define _OBSD_SNMPD_NET_SOCKETS_H

struct obsd_snmpd_udp_listener {
	/* all values in network order */
	in_addr_t address;
	uint16_t  port;
};

struct obsd_snmpd_tcp_connection {
	int state;
	/* all values in network order */
	in_addr_t local_address;
	uint16_t  local_port;
	in_addr_t remote_address;
	uint16_t  remote_port;
};

/* Updates the internal data structures, invalidates all previously
 * returned references */
void
obsd_snmpd_update_net_sockets(void);

/* Find the specified udp listener or the next-best matching element,
 * depending on the value of 'require_exact_match'. Returns either the
 * found element of it's successor, depending on the value of 'next'.
 * References are valid until the next call to close_net_sockets() or
 * update_net_sockets().
 * Returns NULL if no element exists.
 */
struct obsd_snmpd_udp_listener *
obsd_snmpd_get_udp_listener(
    in_addr_t address, uint16_t port,
    int require_exact_match, int next);

/* Find the specified tcp connection or the next-best matching element,
 * depending on the value of 'require_exact_match'. Returns either the
 * found element of it's successor, depending on the value of 'next'.
 * References are valid until the next call to close_net_sockets() or
 * update_net_sockets().
 * Returns NULL if no element exists.
 */
struct obsd_snmpd_tcp_connection *
obsd_snmpd_get_tcp_connection(
    in_addr_t src_address, uint16_t src_port, 
    in_addr_t dst_address, uint16_t dst_port, 
    int require_exact_match, int next);

/* Count the number of tcp connections in the state ESTABLISHED
 * or CLOSE-WAIT. This corresponds to tcpCurrEstab */
int
obsd_snmpd_count_tcp_established(void);

/* close the data source, release internal memory */
void
obsd_snmpd_close_net_sockets(void);

#endif
