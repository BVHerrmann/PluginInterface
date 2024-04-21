/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_obsd.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Portability layer for socket access                                      */
/*                                                                           */
/*  Documentation of the SockIF functions see sock_low.h                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 16
#define SOCK_MODULE_ID    16

/*lint --e{818} */
#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_OPEN_BSD
/*****************************************************************************/

#include <obsd_snmpd_snmpdext.h>
#include <lsa_sock.h>
#include <obsd_kernel_usr.h>
#include <obsd_userland_pnio_syscall_funcs.h>

/*------------------------------------------------------------------------------
// checks
//----------------------------------------------------------------------------*/
#if SOCK_AF_INET != OBSD_PNIO_AF_INET
#error SOCK_AF_INET differs from AF_INET
#endif

/*------------------------------------------------------------------------------
// OpenBSD IP Stack's socket descriptor type
//----------------------------------------------------------------------------*/
typedef int SOCK_LOW_FD_TYPE;

/*------------------------------------------------------------------------------
// native socket descriptor sets, see sock_low_select
//----------------------------------------------------------------------------*/
static obsd_pnio_fd_set readSocketDescriptorSet;
static obsd_pnio_fd_set writeSocketDescriptorSet;
static obsd_pnio_fd_set exceptSocketDescriptorSet;

static SOCK_LOW_FD_TYPE maxfds;

/* FD_SETSIZE see obsd_kernel_select.h */

/*
 * Documentation of the socket function can be found starting here:
 *
 * http://www.openbsd.org/cgi-bin/man.cgi?query=socket&apropos=0&sektion=2&manpath=OpenBSD+5.1&arch=i386&format=html
 * Note the Version 5.1 in the link!
 *
 */

#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
LSA_BOOL
SockIF_GetSockBufHistogram(
	unsigned int sock_fd, /* in */
	struct obsd_pnio_sb_fill_histogram* histogram, /* out */
	LSA_INT* err /* sanity */
) {
	unsigned int len = sizeof(*histogram);

	int ret = obsd_pnio_getsockopt(sock_fd, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_RB_FILL_HIST, histogram, &len);

	if (ret < 0)
	{
		*err = errno;
		return LSA_FALSE;
	}

	return LSA_TRUE;
}
#endif

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_init(
	LSA_VOID
) {
	if (sizeof(SOCK_LOW_HANDLE_TYPE) < sizeof(SOCK_LOW_FD_TYPE)) /* sanity */
	{
		SOCK_FATAL();
	}
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE
sock_low_socket(
	int type,
	int* err
) {
	/* note on the parameter "domain":
	 * initialized as struct domain inetdomain = { AF_INET, ...
	 * but ip_init() compares ... dom_family == PF_INET ...
	 * luckily PF_INET is the same as AF_INET
	 */
	SOCK_LOG_FCT("sock_low_socket")

	SOCK_LOW_FD_TYPE s = obsd_socket(OBSD_PNIO_AF_INET, (type == SOCK_LOW_STREAM) ? OBSD_PNIO_SOCK_STREAM : OBSD_PNIO_SOCK_DGRAM, 0);

	if (s < 0)
	{
	    *err = errno;

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		int error = obsd_pnio_set_ip_portrange_high(s);

		if (error != 0)
		{
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "Error: could not set IP_PORTRANGE_HIGH on socket %u", s);

			*err = errno;

			return SOCK_LOW_INVALID_HANDLE;
		}
		else
		{
			return (SOCK_LOW_HANDLE_TYPE)s;
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int *err
) {
	SOCK_LOG_FCT("sock_low_bind")

	struct obsd_pnio_sockaddr addr;

	addr.sa_family = (unsigned char)address->sin_family;
	addr.sa_len    = 0;
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( obsd_pnio_bind((SOCK_LOW_FD_TYPE)lower_handle, &addr, sizeof(addr)) < 0 )
	{
		*err = errno;
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "Error: could not bind socket %u", lower_handle);

		switch(*err)
		{
		case OBSD_PNIO_EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case OBSD_PNIO_ENOMEM:
		case OBSD_PNIO_ENOBUFS:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int backlog,
	int* err
) {
	SOCK_LOG_FCT("sock_low_listen")

	if( listen((SOCK_LOW_FD_TYPE)lower_handle, backlog) < 0 )
	{
		*err = errno;
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "Error: cannot listen on socket %u", lower_handle);

		switch(*err)
		{
		case OBSD_PNIO_EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_map_connect_error(
	int error
) {
	switch(error)
	{
	case OBSD_PNIO_EINPROGRESS:
	case OBSD_PNIO_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case OBSD_PNIO_ENOMEM:
	case OBSD_PNIO_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case OBSD_PNIO_ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case OBSD_PNIO_ETIMEDOUT:
	case OBSD_PNIO_ENETUNREACH:
		return SOCK_RSP_ERR_REM_NET;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_connect(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
) {
	SOCK_LOG_FCT("sock_low_connect")

	struct obsd_pnio_sockaddr addr;

	addr.sa_len = 0;
	addr.sa_family = (unsigned char)address->sin_family;
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( obsd_pnio_connect((SOCK_LOW_FD_TYPE)lower_handle, &addr, sizeof(addr)) < 0 )
	{
		*err = errno;

		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "Error: cannot connect with socket %u", lower_handle);

		return sock_low_map_connect_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE
sock_low_accept(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	SOCK_LOG_FCT("sock_low_accept")

	SOCK_LOW_FD_TYPE s;

	struct obsd_pnio_sockaddr addr;
	unsigned int addr_len = sizeof(addr);

	s = obsd_pnio_accept((SOCK_LOW_FD_TYPE)lower_handle, &addr, &addr_len);

	LSA_UNUSED_ARG(err);

	if (s < 0)
	{
		if (errno != OBSD_PNIO_EWOULDBLOCK)
		{
			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN
				, "accept() failed on lower_handle(0x%x), errno(%d)"
				, lower_handle, errno
				);
		}

		*err = errno;

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		address->sin_family = addr.sa_family; /* see check above */
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);

		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	if( close((SOCK_LOW_FD_TYPE)lower_handle) != 0 )
	{
		return SOCK_RSP_ERR_LOC_NET;
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_map_tcp_error(
	int error
) {
	switch(error)
	{
	case OBSD_PNIO_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case OBSD_PNIO_ECONNABORTED:
	case OBSD_PNIO_ECONNRESET:
	case OBSD_PNIO_ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case OBSD_PNIO_EPIPE: /* ETIMEDOUT error in socket, but t_send() and t_recv() yield EPIPE  */
	case OBSD_PNIO_ETIMEDOUT: /* connection dropped by stack's timer */
		return SOCK_RSP_ERR_REM_NET;

	case OBSD_PNIO_ENOMEM:
	case OBSD_PNIO_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_send(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int* sent,
	int* err
) {
	/* note: send() is not a system-call. send(s, buf, len, flags) == sendto(s, buf, len, flags, NULL, 0) */

	*sent = obsd_pnio_sendto((SOCK_LOW_FD_TYPE)lower_handle
							 , buffer
							 , (unsigned long)length
							 , 0
							 , 0 /*NULL*/
							 , 0
							 );

	if (*sent < 0)
	{
		*err = errno;

		*sent = SOCK_LOW_SOCKET_ERROR;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_recv(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	int* received,
	int* err
) {
	/* note: recv() is not a system-call. recv(s, buf, len, flags) == recvfrom(s, buf, len, flags, NULL, 0) */

	*received = obsd_pnio_recvfrom((SOCK_LOW_FD_TYPE)lower_handle
									, buffer
									, (unsigned long)length
									, (flags & SOCK_LOW_PEEK) ? OBSD_PNIO_MSG_PEEK : 0
									, 0 /*NULL*/
									, 0
									);

	if (*received < 0)
	{
		*err = errno;

		*received = SOCK_LOW_SOCKET_ERROR;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE
sock_low_map_udp_error(
	int err
) {
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case OBSD_PNIO_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case OBSD_PNIO_ENOMEM:
	case OBSD_PNIO_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_sendto(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	SOCK_SOCKADDR_TYPE* address,
	SOCK_SOCKADDR_TYPE* local_address,
	int* sent,
	int* err
) {
	struct obsd_pnio_sockaddr to_addr;
	struct obsd_pnio_sockaddr* to_addr_p;

	if (address)
	{
		to_addr.sa_family = (unsigned char)address->sin_family;
		SOCK_MEMCPY(to_addr.sa_data, &address->sin_port, 14);

		to_addr_p = &to_addr;
	}
	else
	{
		to_addr_p = 0;
	}

#if	SOCK_CFG_CLUSTER_IP_SUPPORTED /* use obsd_sendmsg */
	{
	struct obsd_pnio_sockaddr from_addr;
	struct obsd_pnio_sockaddr* from_addr_p;

	if (local_address)
	{
		from_addr.sa_family = (unsigned char)local_address->sin_family;
		SOCK_MEMCPY(from_addr.sa_data, &local_address->sin_port, 14);

		from_addr_p = &from_addr;
	}
	else
	{
		from_addr_p = 0;
	}

	*sent = obsd_pnio_sendfromto((SOCK_LOW_FD_TYPE)lower_handle
								, buffer, length
								, OBSD_PNIO_MSG_DONTWAIT
								, from_addr_p, (from_addr_p) ? sizeof(*from_addr_p) : 0
								, to_addr_p, (to_addr_p) ? sizeof(*to_addr_p) : 0
								);
	}
#else

	LSA_UNUSED_ARG(local_address);

	*sent = obsd_pnio_sendto((SOCK_LOW_FD_TYPE)lower_handle
							 , buffer
							 , (unsigned long)length
							 , OBSD_PNIO_MSG_DONTWAIT
							 , to_addr_p
							 , (to_addr_p) ? sizeof(*to_addr_p) : 0
							 );
#endif

	if (*sent < 0)
	{
		*err = errno;

		*sent = SOCK_LOW_SOCKET_ERROR;

		return sock_low_map_udp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_recvfrom(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	SOCK_SOCKADDR_TYPE* address,
	SOCK_SOCKADDR_TYPE* local_address,
	int* received,
	int* err
) {
	struct obsd_pnio_sockaddr from_addr;
	unsigned int fromlen = sizeof(from_addr);

#if	SOCK_CFG_CLUSTER_IP_SUPPORTED /* use obsd_recvmsg */
	struct obsd_pnio_sockaddr loc_addr;
	unsigned int loclen = sizeof(loc_addr);
	int if_index = -1;

	*received = obsd_pnio_recvfromto((SOCK_LOW_FD_TYPE)lower_handle
										, buffer, length
										, (flags & SOCK_LOW_PEEK) ? OBSD_PNIO_MSG_PEEK : 0
										, (char *)&from_addr, &fromlen
										, (char *)&loc_addr, &loclen
										, &if_index
										);
#else

	LSA_UNUSED_ARG(local_address);

	*received = obsd_pnio_recvfrom((SOCK_LOW_FD_TYPE)lower_handle
									, buffer
									, (unsigned long)length
									, (flags & SOCK_LOW_PEEK) ? OBSD_PNIO_MSG_PEEK : 0
									, &from_addr
									, &fromlen
									);
#endif

	if (*received < 0)
	{
		*err = errno;

		*received = SOCK_LOW_SOCKET_ERROR;

		return sock_low_map_udp_error(*err);
	}
	else
	{
		address->sin_family = from_addr.sa_family;
		SOCK_MEMCPY(&address->sin_port, from_addr.sa_data, 14);

#if	SOCK_CFG_CLUSTER_IP_SUPPORTED
		{
		local_address->sin_family = loc_addr.sa_family;
		SOCK_MEMCPY(&local_address->sin_port, loc_addr.sa_data, 14);
		}
#endif
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
	int* err
) {
	unsigned int len = sizeof(*socket_error);

	int ret = obsd_pnio_getsockopt((SOCK_LOW_FD_TYPE)lower_handle
									, OBSD_PNIO_SOL_SOCKET
									, OBSD_PNIO_SO_ERROR
									, socket_error
									, &len
									);

	if (ret < 0)
	{
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_setsockopt(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_LOW_SOCKOPT_TYPE option_name,
	void* option_value,
	int* err
) {
	SOCK_LOG_FCT("sock_low_setsockopt")

	int ret;
	SOCK_LOW_FD_TYPE s = (SOCK_LOW_FD_TYPE)lower_handle;

	switch (option_name)
	{
	case sock_low_sockopt_NonBlocking:
		ret = obsd_pnio_ioctl(s, OBSD_PNIO_FIONBIO, (char *)option_value);
		break;

	case sock_low_sockopt_TCPNoDelay:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_TCP, OBSD_PNIO_TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoAckDelay:
		/* OBSD_ITGR: OBSD IP Stack does not support TCP_NOACKDELAY */
		ret = 0;
		break;

	case sock_low_sockopt_ReuseAddr:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_KeepAlive:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_DontRoute:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_SendBuffer:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_Linger:
		{
		struct obsd_pnio_linger linger_val;
		linger_val.l_linger = *(int*)option_value;
		linger_val.l_onoff  = 1;
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_LINGER, &linger_val, sizeof(linger_val));
		}
		break;

	case sock_low_sockopt_Broadcast:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_OOBInline:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_OOBINLINE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_IPRecvdStAddr:
		ret = obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_IP, OBSD_PNIO_IP_RECVDSTADDR, option_value, sizeof(LSA_INT32));
		break;

	default:
		ret = -1; /* prevent compiler warning C4701: potentially uninitialized local variable 'ret' used */

		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "option_name(%d) not supported by this function"
			, option_name
			);

		SOCK_FATAL();

		break;
	}

	if (ret < 0)
	{
		if (err) *err = errno;
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

#if SOCK_CFG_ENABLE_MULTICASTS
LSA_RESPONSE_TYPE
sock_low_set_multicast_sockopts(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_UINT32 interface_addr,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
) {
	SOCK_LOG_FCT("sock_low_set_multicast_sockopts")

	struct obsd_pnio_ip_mreq stMreq;
	LSA_UINT8 mc_loop = 0;
	struct obsd_pnio_in_addr ifaddr;

	SOCK_LOW_FD_TYPE s = (SOCK_LOW_FD_TYPE)lower_handle;

	*err = 0;
	stMreq.imr_interface.s_addr = interface_addr;
	stMreq.imr_multiaddr.s_addr = group_addr;

	ifaddr.s_addr = interface_addr;

	if(obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_IP, OBSD_PNIO_IP_ADD_MEMBERSHIP, &stMreq, sizeof(stMreq)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_ADD_MEMBERSHIP option");
		*err = errno;
	}
	else if(obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_IP, OBSD_PNIO_IP_MULTICAST_IF, &ifaddr, sizeof(ifaddr)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_IF option");
		*err = errno;
	}
	else if(obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_IP, OBSD_PNIO_IP_MULTICAST_LOOP, &mc_loop, sizeof(mc_loop)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_LOOP option");
		*err = errno;
	}
	else if(obsd_pnio_setsockopt(s, OBSD_PNIO_IPPROTO_IP, OBSD_PNIO_IP_MULTICAST_TTL, &ip_multicast_ttl, sizeof(ip_multicast_ttl)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_TTL option");
		*err = errno;
	}

	switch(*err)
	{
	case 0:
		return SOCK_RSP_OK;

	case OBSD_PNIO_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE; /* memory allocation failed */

	case OBSD_PNIO_EINVAL:
		return SOCK_RSP_ERR_PARAM; /* not a mc address used for IP_ADD_MEMBERSHIP */

	/* should never happen: */
	case OBSD_PNIO_EADDRNOTAVAIL:
	case OBSD_PNIO_EADDRINUSE:
	case OBSD_PNIO_ETOOMANYREFS:
	case OBSD_PNIO_EOPNOTSUPP:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}
#endif

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_get_sockname(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
) {
	struct obsd_pnio_sockaddr addr;
	unsigned int addr_len = sizeof(addr);

	if( obsd_pnio_getsockname((SOCK_LOW_FD_TYPE)lower_handle, &addr, &addr_len) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case OBSD_PNIO_ENOMEM:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}

	}
	else
	{
		address->sin_family = addr.sa_family;
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32
sock_low_select(
	int timeout
) {
	struct obsd_pnio_timeval tv;

	if( timeout == 0 ) // avoid div/mod
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
	else
	{
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
	}

	return obsd_pnio_select(maxfds+1
		, &readSocketDescriptorSet
		, &writeSocketDescriptorSet
		, &exceptSocketDescriptorSet
		, &tv);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_readset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	OBSD_PNIO_FD_SET((unsigned int)lower_handle, &readSocketDescriptorSet);

	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	OBSD_PNIO_FD_SET((unsigned int)lower_handle, &writeSocketDescriptorSet);

	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	OBSD_PNIO_FD_SET((unsigned int)lower_handle, &exceptSocketDescriptorSet);

	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return OBSD_PNIO_FD_ISSET((unsigned int)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return OBSD_PNIO_FD_ISSET((unsigned int)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return OBSD_PNIO_FD_ISSET((unsigned int)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_clear_all_sets(
	LSA_VOID
) {
	OBSD_PNIO_FD_ZERO(&readSocketDescriptorSet);

	OBSD_PNIO_FD_ZERO(&writeSocketDescriptorSet);

	OBSD_PNIO_FD_ZERO(&exceptSocketDescriptorSet);

	maxfds = 0;
}

/*----------------------------------------------------------------------------*/

#if SOCK_INT_CFG_SOCKET_EVENTS
LSA_BOOL
sock_low_set_custom_option(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	void* sock_socket_ptr,
	void (*event_callback_fct)(int, void*)
) {
	SOCK_LOG_FCT("sock_low_set_custom_option")

	struct obsd_pnio_event_callback event_callback_val;

	event_callback_val.l_usr_callback = event_callback_fct;
	event_callback_val.l_ctx_ptr = sock_socket_ptr;

	if (obsd_pnio_setsockopt((SOCK_LOW_FD_TYPE)lower_handle, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_EVENT_CALLBACK, &event_callback_val, sizeof(event_callback_val)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "error setting IP_CUSTOM option");
		return LSA_FALSE;
	}
	else
	{
		return LSA_TRUE;
	}
}
#endif

/*----------------------------------------------------------------------------*/

#if SOCK_INT_CFG_SOCKET_EVENTS
LSA_BOOL
sock_low_has_socket_error(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    int* socket_error,
    int* err
) {
	SOCK_LOG_FCT("sock_low_has_socket_error")

	unsigned int len = sizeof(socket_error);

	int ret = obsd_pnio_getsockopt((SOCK_LOW_FD_TYPE)lower_handle
									, OBSD_PNIO_SOL_SOCKET
									, OBSD_PNIO_SO_ERROR
									, socket_error
									, &len
									);

	if (ret < 0)
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
							, "obsd_pnio_getsockopt failed, errno(%d) -> socket has error"
							, errno
							);

		*err = errno; /* make return value of getsockopt() available */

		return LSA_TRUE; /* unable to retrieve SO_ERROR option -> socket is considered to be damaged */
	}
	else /* valid *socket_error from socket */
	{
		if (*socket_error
			&& *socket_error != OBSD_PNIO_EWOULDBLOCK
			&& *socket_error != OBSD_PNIO_EINPROGRESS
			)
		{
			return LSA_TRUE;
		}
		else
		{
			return LSA_FALSE;
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/

LSA_UINT16
SOCK_NTOHS(
	LSA_UINT16 parameter
) {
	return obsd_pnio_ntohs(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32
SOCK_NTOHL(
	LSA_UINT32 parameter
) {
	return obsd_pnio_ntohl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
SOCK_HTONS(
	LSA_UINT16 parameter
) {
	return obsd_pnio_htons(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32
SOCK_HTONL(
	LSA_UINT32 parameter
) {
	return obsd_pnio_htonl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_abort_tcp_session(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int *err
) {
	int linger_time = 0;

	sock_low_setsockopt(lower_handle, sock_low_sockopt_Linger, &linger_time, LSA_NULL);
	/* do not consider return value of setsockopt, socket has to be closed anyway */

	if (sock_low_close(lower_handle) != SOCK_RSP_OK)
	{
		*err = errno;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_UINT8
sock_low_get_reason_code(
	int err
) {
	switch(err)
	{
	case OBSD_PNIO_EMSGSIZE: return SOCK_IPSTACK_EMSGSIZE;
	case OBSD_PNIO_EHOSTUNREACH: return SOCK_IPSTACK_EHOSTUNREACH;
	default: return 0;
	}
}

/*****************************************************************************/
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
