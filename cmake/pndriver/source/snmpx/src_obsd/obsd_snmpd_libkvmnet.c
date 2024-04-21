/**
 * libkvmnet.c
 * Provides access to the network (listen) sockets analog to libkvm.
 * Implementation highly depends on the openbsd version and is likely
 * to change in the future.
 */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_sysctl.h>
#define _KERNEL
#include <sys/obsd_kernel_file.h>
#undef _KERNEL

#include <net/obsd_kernel_if.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_udp.h>
#include <netinet/obsd_kernel_udp_var.h>
 
#include <obsd_userland_limits.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_event.h>
#include <obsd_userland_string.h>
#include <obsd_kernel_fcntl.h>
// #include <obsd_userland_kvm.h>

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_libkvmnet.h"

#define OBSD_HAVE_LIBKVM 0  /* gh2289n: currently we have no libkvm    */
#define OBSD_HAVE_QSORT  1  /* gh2289n: currently we have no qsort too */

/* global variable */
static struct obsd_snmpd_udp_listener *obsd_snmpd_udp_listeners = NULL;
static int obsd_snmpd_udp_listener_cnt = 0;

static struct obsd_snmpd_tcp_connection *obsd_snmpd_tcp_connections = NULL;
static int obsd_snmpd_tcp_connection_cnt = 0;

/* internal prototype */
static int obsd_snmpd_udpsort(const void *a, const void *b);
static int obsd_snmpd_tcpsort(const void *a, const void *b);
static char * obsd_snmpd_libkvm_getfiles(size_t esize, int *cnt);



/* compare function for sorting struct obsd_snmpd_udp_listener */
static int
obsd_snmpd_udpsort(const void *a, const void *b)
{
	const struct obsd_snmpd_udp_listener *ka =
		(const struct obsd_snmpd_udp_listener *)a;
	const struct obsd_snmpd_udp_listener *kb =
		(const struct obsd_snmpd_udp_listener *)b;

	if (ntohl(ka->address) > ntohl(kb->address)) return 1;
	if (ntohl(ka->address) < ntohl(kb->address)) return -1;
	if (ntohs(ka->port) > ntohs(kb->port)) return 1;
	if (ntohs(ka->port) < ntohs(kb->port)) return -1;

	return 0;
}

/* compare function for sorting struct obsd_snmpd_tcp_connection */
static int
obsd_snmpd_tcpsort(const void *a, const void *b)
{
	const struct obsd_snmpd_tcp_connection *ka =
		(const struct obsd_snmpd_tcp_connection *)a;
	const struct obsd_snmpd_tcp_connection *kb =
		(const struct obsd_snmpd_tcp_connection *)b;

	if (ntohl(ka->local_address) > ntohl(kb->local_address)) return 1;
	if (ntohl(ka->local_address) < ntohl(kb->local_address)) return -1;
	if (ntohs(ka->local_port) > ntohs(kb->local_port)) return 1;
	if (ntohs(ka->local_port) < ntohs(kb->local_port)) return -1;
	if (ntohl(ka->remote_address) > ntohl(kb->remote_address)) return 1;
	if (ntohl(ka->remote_address) < ntohl(kb->remote_address)) return -1;
	if (ntohs(ka->remote_port) > ntohs(kb->remote_port)) return 1;
	if (ntohs(ka->remote_port) < ntohs(kb->remote_port)) return -1;

	return 0;
}

/* get socket info from kernel */
/* this piece of code is derived from libkvm, kvm_getfile2() */
static char *
obsd_snmpd_libkvm_getfiles(size_t esize, int *cnt)
{
	int mib[6], rv;
	size_t size;
	char * buf;

	mib[0] = CTL_KERN;
	mib[1] = KERN_FILE2;
	mib[2] = KERN_FILE_BYFILE;
	mib[3] = 0;
	mib[4] = esize;
	mib[5] = 0;

	/* find size and alloc buffer */
	rv = sysctl(mib, 6, NULL, &size, NULL, 0);
	if (rv < 0) {
                log_warnx("sysctl for sockets failed");
		return NULL;
	}

	/* get data */
	buf = malloc(size);
	if (buf == 0) {
		return NULL;
	}
	mib[5] = size / esize;
	rv = sysctl(mib, 6, buf, &size, NULL, 0);
	if (rv < 0) {
		free(buf);
                log_warnx("sysctl for sockets failed");
		return NULL;
	}

	*cnt = size / esize;
	return buf;
}

void
obsd_snmpd_update_net_sockets(void) {
	struct kinfo_file2 *kf;
	int file_cnt, i, cnt_udp, cur_udp, cnt_tcp, cur_tcp;

	/* free old data */
	if (obsd_snmpd_udp_listeners) {
		free(obsd_snmpd_udp_listeners);
		obsd_snmpd_udp_listeners = NULL;
	}
	obsd_snmpd_udp_listener_cnt = 0;
	if (obsd_snmpd_tcp_connections) {
		free(obsd_snmpd_tcp_connections);
		obsd_snmpd_tcp_connections = NULL;
	}
	obsd_snmpd_tcp_connection_cnt = 0;

	/* open libkvm and get sockets */
	kf = (struct kinfo_file2 *)
		obsd_snmpd_libkvm_getfiles(sizeof(*kf), &file_cnt);

	if (kf == NULL) {
		/* out of memory */
		return;
	}
	
	/* count the relevant entries */
	cnt_udp = 0;
	cnt_tcp = 0;
	for (i = 0; i < file_cnt; i++) {
		if ((kf[i].f_type == DTYPE_SOCKET) &&
		    (kf[i].so_family == AF_INET) &&
		    (kf[i].so_pcb != 0) &&
		    (kf[i].inp_rtableid == 0) &&
		    (kf[i].so_protocol == IPPROTO_UDP) &&
		    (kf[i].inp_faddru[0] == 0) &&
		    (kf[i].inp_fport == 0) &&
		    (kf[i].inp_lport != 0) /* don't show raw socket */
		   )
		{
			cnt_udp++;
		}
		if ((kf[i].f_type == DTYPE_SOCKET) &&
		    (kf[i].so_family == AF_INET) &&
		    (kf[i].so_pcb != 0) &&
		    (kf[i].inp_rtableid == 0) &&
		    (kf[i].so_protocol == IPPROTO_TCP)
		   )
		{
			cnt_tcp++;
		}
	}

	/* allocate memory and copy the entries */
	obsd_snmpd_udp_listeners =
		malloc(cnt_udp * sizeof(struct obsd_snmpd_udp_listener));
	if (obsd_snmpd_udp_listeners == NULL) { return; }
	obsd_snmpd_tcp_connections =
		malloc(cnt_tcp * sizeof(struct obsd_snmpd_tcp_connection));
	if (obsd_snmpd_tcp_connections == NULL) { return; }
	cur_udp = 0;
	cur_tcp = 0;
	for (i = 0; i < file_cnt; i++) {
		/* udp listen socket */
		if ((kf[i].f_type == DTYPE_SOCKET) &&
		    (kf[i].so_family == AF_INET) &&
		    (kf[i].so_pcb != 0) &&
		    (kf[i].inp_rtableid == 0) &&
		    (kf[i].so_protocol == IPPROTO_UDP) &&
		    (kf[i].inp_faddru[0] == 0) &&
		    (kf[i].inp_fport == 0) &&
		    (kf[i].inp_lport != 0) /* don't show raw socket */
		   )
		{
			obsd_snmpd_udp_listeners[cur_udp].address =
			    kf[i].inp_laddru[0];
			obsd_snmpd_udp_listeners[cur_udp].port =
			    (uint16_t)kf[i].inp_lport;

			cur_udp++;
		}
		/* tcp connection */
		if ((kf[i].f_type == DTYPE_SOCKET) &&
		    (kf[i].so_family == AF_INET) &&
		    (kf[i].so_pcb != 0) &&
		    (kf[i].inp_rtableid == 0) &&
		    (kf[i].so_protocol == IPPROTO_TCP)
		   )
		{
			obsd_snmpd_tcp_connections[cur_tcp].local_address = 
			    kf[i].inp_laddru[0];
			obsd_snmpd_tcp_connections[cur_tcp].local_port = 
			    (uint16_t)kf[i].inp_lport;
			obsd_snmpd_tcp_connections[cur_tcp].remote_address = 
			    kf[i].inp_faddru[0];
			obsd_snmpd_tcp_connections[cur_tcp].remote_port = 
			    (uint16_t)kf[i].inp_fport;
			obsd_snmpd_tcp_connections[cur_tcp].state = 
			    kf[i].t_state;

			cur_tcp++;

		}
	}
	obsd_snmpd_udp_listener_cnt = cnt_udp;
	obsd_snmpd_tcp_connection_cnt = cnt_tcp;
	free(kf);

	/* sort the list, _get_udp_listener() needs this */
#if       OBSD_HAVE_QSORT /* gh2289n */
	qsort(obsd_snmpd_udp_listeners, cnt_udp,
		sizeof(struct obsd_snmpd_udp_listener), obsd_snmpd_udpsort);
	qsort(obsd_snmpd_tcp_connections, cnt_tcp,
		sizeof(struct obsd_snmpd_tcp_connection), obsd_snmpd_tcpsort);
#endif /* OBSD_HAVE_QSORT */
}

void
obsd_snmpd_close_net_sockets(void) {
	/* free old data */
	if (obsd_snmpd_udp_listeners) {
		free(obsd_snmpd_udp_listeners);
		obsd_snmpd_udp_listeners = NULL;
	}
	obsd_snmpd_udp_listener_cnt = 0;
	if (obsd_snmpd_tcp_connections) {
		free(obsd_snmpd_tcp_connections);
		obsd_snmpd_tcp_connections = NULL;
	}
	obsd_snmpd_tcp_connection_cnt = 0;
}

struct obsd_snmpd_udp_listener *
obsd_snmpd_get_udp_listener(
    in_addr_t address, uint16_t port,
    int require_exact_match, int next)
{
	int ul_idx = -1;
	int i;

	/* find the first matching element */
	for (i=0; i < obsd_snmpd_udp_listener_cnt; i++) {
		if ((ntohl(address) <=
		         ntohl(obsd_snmpd_udp_listeners[i].address))
		    &&
		    (ntohs(port) <= ntohs(obsd_snmpd_udp_listeners[i].port)))
		{
			ul_idx = i;
			break;
		}
	}

	/* no element */
	if (ul_idx == -1) {
		return NULL;
	}

	/* ensure exact match if required */
	if (require_exact_match &&
	    ((obsd_snmpd_udp_listeners[ul_idx].port != port) ||
	     (obsd_snmpd_udp_listeners[ul_idx].address != address))
	   )
	{
		return NULL;
	}

	if (next) {
		if ((ul_idx+1) < obsd_snmpd_udp_listener_cnt) {
			/* get next element */
			ul_idx++;
		}
		else {
			/* no next element */
			return NULL;
		}
	}

	/* return by reference, ref is valid until global struct is released */
	return &(obsd_snmpd_udp_listeners[ul_idx]);
}

struct obsd_snmpd_tcp_connection *
obsd_snmpd_get_tcp_connection(
    in_addr_t local_address, uint16_t local_port,
    in_addr_t remote_address, uint16_t remote_port,
    int require_exact_match, int next)
{
	int tc_idx = -1;
	int i;
	struct obsd_snmpd_tcp_connection *
	    tcp_conns = obsd_snmpd_tcp_connections;

	/* find the first matching element */
	for (i=0; i < obsd_snmpd_tcp_connection_cnt; i++) {
		if ((ntohl(local_address) <=
		        ntohl(tcp_conns[i].local_address)) &&
		    (ntohs(local_port) <= ntohs(tcp_conns[i].local_port)) &&
		    (ntohl(remote_address) <=
		        ntohl(tcp_conns[i].remote_address)) &&
		    (ntohs(remote_port) <= ntohs(tcp_conns[i].remote_port))
		   )
		{
			tc_idx = i;
			break;
		}
	}

	/* no element */
	if (tc_idx == -1) {
		return NULL;
	}

	/* ensure exact match if required */
	if (require_exact_match &&
	    ((tcp_conns[tc_idx].local_address != local_address) ||
	     (tcp_conns[tc_idx].local_port != local_port) ||
	     (tcp_conns[tc_idx].remote_address != remote_address) ||
	     (tcp_conns[tc_idx].remote_port != remote_port))
	   )
	{
		return NULL;
	}

	if (next) {
		if ((tc_idx+1) < obsd_snmpd_tcp_connection_cnt) {
			/* get next element */
			tc_idx++;
		}
		else {
			/* no next element */
			return NULL;
		}
	}

	/* return by reference, ref is valid until global struct is released */
	return &(obsd_snmpd_tcp_connections[tc_idx]);
}

int
obsd_snmpd_count_tcp_established(void) {
	int count = 0;
	int i;

	for (i=0; i<obsd_snmpd_tcp_connection_cnt; i++) {
		if ((obsd_snmpd_tcp_connections[i].state == TCPS_ESTABLISHED) ||
		    (obsd_snmpd_tcp_connections[i].state == TCPS_CLOSE_WAIT))
		{
			count++;
		}
	}
	return count;
}

