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
/*  F i l e               &F: sock_iniche.c                             :F&  */
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
/*****************************************************************************/
/*lint -esym(750,LTRC_ACT_MODUL_ID) */
#define LTRC_ACT_MODUL_ID 4
/*lint -esym(750,SOCK_MODULE_ID) */
#define SOCK_MODULE_ID    4

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_INTERNICHE
/*****************************************************************************/

/*------------------------------------------------------------------------------
// stack-specific includes
//----------------------------------------------------------------------------*/
/* If SOCK_CFG_INTERNICHE_DEF_FD == 1,
   the necessary header files have to be included in sock_inc.h
*/
#if !SOCK_CFG_INTERNICHE_DEF_FD
#include "interniche_usr.h"
#endif

/*------------------------------------------------------------------------------
// checks
//----------------------------------------------------------------------------*/
#if SOCK_AF_INET != AF_INET /* AP01145901 */
#error SOCK_AF_INET differs from AF_INET
#endif

#ifdef SOCK_CFG_SNMP_ON
#if SOCK_SNMP_BUFFER_SIZE != SNMPSIZ /* sanity (see 216748) */
#error SOCK_SNMP_BUFFER_SIZE must be equal to SNMPSIZ
#endif
#endif

/*------------------------------------------------------------------------------
// native socket descriptor sets, see sock_low_select
//----------------------------------------------------------------------------*/
static fd_set readSocketDescriptorSet;
static fd_set writeSocketDescriptorSet;
static fd_set exceptSocketDescriptorSet;

/*------------------------------------------------------------------------------
// InterNiche IP Stack's socket descriptor type
//----------------------------------------------------------------------------*/
typedef long SOCK_LOW_FD_TYPE;

/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

void
sock_low_init(
	void
) {
	if( sizeof(SOCK_SOCKADDR_TYPE) != sizeof(struct sockaddr) ) {
		SOCK_FATAL();
	}

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
	SOCK_LOW_FD_TYPE s = t_socket(PF_INET, (type == SOCK_LOW_STREAM) ? SOCK_STREAM : SOCK_DGRAM, 0);

	if (s == SOCKET_ERROR)
	{
		*err = 0; /* Interniche has no errno */

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int *err
) {
	if( t_bind((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, sizeof(*address)) < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case ENOMEM:
		case ENP_RESOURCE:
		case ENOBUFS:
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
	if( t_listen((SOCK_LOW_FD_TYPE)lower_handle, backlog) < 0 )
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

		switch(*err)
		{
		case EADDRINUSE:
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
	case EINPROGRESS:
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case ENETUNREACH:
	case ETIMEDOUT:
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
	if( t_connect((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

		return sock_low_map_connect_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE
sock_low_accept(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	int address_len = sizeof(*address);

	SOCK_LOW_FD_TYPE s  = t_accept((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, &address_len);

	if (s == SOCKET_ERROR)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle); /* for caller trace only */

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	if (t_socketclose((SOCK_LOW_FD_TYPE)lower_handle) < 0)
	{
		return SOCK_RSP_ERR_LOC_NET; /* should never happen */
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
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ECONNABORTED:
	case ECONNRESET:
	case ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case EPIPE: /* ETIMEDOUT error in socket, but t_send() and t_recv() yield EPIPE  */
	case ETIMEDOUT: /* connection dropped by stack's timer */
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
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
	/* note: The interniche stack can return 0 instead of error EWOULDBLOCK */

	*sent = t_send( (SOCK_LOW_FD_TYPE)lower_handle
					, buffer
					, length
					, MSG_DONTWAIT
					);

	if (*sent < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

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
	*received = t_recv( (SOCK_LOW_FD_TYPE)lower_handle
						, buffer
						, length
						, (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0 /* only MSG_PEEK needed */
						);

	if (*received < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

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
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENP_RESOURCE:
	case ENOBUFS:
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
	LSA_UNUSED_ARG(local_address); /* not supported */

	*sent = t_sendto( (SOCK_LOW_FD_TYPE)lower_handle
					  , buffer
					  , length
					  , MSG_DONTWAIT
					  , (struct sockaddr*)address
					  , (address) ? sizeof(*address) : 0
					  );

	if (*sent < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

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
	LSA_INT address_len = sizeof(*address);

	LSA_UNUSED_ARG(local_address); /* not supported */

	*received = t_recvfrom( (SOCK_LOW_FD_TYPE)lower_handle
							, buffer
							, length
							, (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
							, (struct sockaddr*)address
							, &address_len
							);

	if (*received < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

		return sock_low_map_udp_error(*err);
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
	int ret = t_getsockopt((SOCK_LOW_FD_TYPE)lower_handle
							, SOL_SOCKET
							, SO_ERROR
							, socket_error
							, sizeof(*socket_error)
							);

	if (ret < 0)
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

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
		ret = t_setsockopt(s, SOL_SOCKET, SO_NONBLOCK, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoDelay:
		ret = t_setsockopt(s, IPPROTO_TCP, TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoAckDelay:
		ret = t_setsockopt(s, IPPROTO_TCP, TCP_NOACKDELAY, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_ReuseAddr:
		ret = t_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_KeepAlive:
		ret = t_setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_DontRoute:
		ret = t_setsockopt(s, SOL_SOCKET, SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_SendBuffer:
		ret = t_setsockopt(s, SOL_SOCKET, SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = t_setsockopt(s, SOL_SOCKET, SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_Linger:
		{
		struct linger linger_value;
		linger_value.l_linger = *(int*)option_value;
		linger_value.l_onoff  = 1;
		ret = t_setsockopt(s, SOL_SOCKET, SO_LINGER, &linger_value, sizeof(linger_value));
		}
		break;

	case sock_low_sockopt_Broadcast:
		ret = t_setsockopt(s, SOL_SOCKET, SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_OOBInline:
		ret = t_setsockopt(s, SOL_SOCKET, SO_OOBINLINE, option_value, sizeof(LSA_INT32));
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
		*err = t_errno(s);

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_RESPONSE_TYPE
sock_low_set_multicast_sockopts(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_UINT32 IfIndex,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
) {
	SOCK_LOG_FCT("sock_low_set_multicast_sockopts")

	struct ip_mreq stMreq;
	LSA_UINT8 mc_loop = 0;

	SOCK_LOW_FD_TYPE s = (SOCK_LOW_FD_TYPE)lower_handle;

	*err = 0;
	stMreq.imr_interface = IfIndex;
	stMreq.imr_multiaddr = group_addr;

	if(t_setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &stMreq, sizeof(stMreq)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_ADD_MEMBERSHIP option");
		*err = t_errno(s);
	}
	else if( t_setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, &IfIndex, sizeof(IfIndex)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_IF option");
		*err = t_errno(s);
	}
	else if(t_setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &mc_loop, sizeof(mc_loop)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_LOOP option");
		*err = t_errno(s);
	}
	else if(t_setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &ip_multicast_ttl, sizeof(ip_multicast_ttl)) != 0)
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "error setting IP_MULTICAST_TTL option");
		*err = t_errno(s);
	}

	switch(*err)
	{
	case 0:
		return SOCK_RSP_OK;

	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE; /* memory allocation failed */

	case EINVAL:
		return SOCK_RSP_ERR_PARAM; /* not a mc address used for IP_ADD_MEMBERSHIP */

	/* should never happen: */
	case EADDRNOTAVAIL:
	case EADDRINUSE:
	case ETOOMANYREFS:
	case EOPNOTSUPP:
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
	int* err
) {

	int address_len = sizeof(struct sockaddr);

	if( t_getsockname((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, &address_len) < 0 )
	{
		*err = t_errno((SOCK_LOW_FD_TYPE)lower_handle);

		switch(*err)
		{
		case ENOMEM:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}

	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32
sock_low_select(
	int timeout
) {
	return t_select( &readSocketDescriptorSet
					, &writeSocketDescriptorSet
					, &exceptSocketDescriptorSet
					, (TPS * timeout) / 1000
					);
}

/*----------------------------------------------------------------------------*/

LSA_VOID sock_low_readset_insert(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet);
}

/*----------------------------------------------------------------------------*/

LSA_VOID sock_low_writeset_insert(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet);
}

/*----------------------------------------------------------------------------*/

LSA_VOID sock_low_exceptset_insert(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL sock_low_in_readset(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL sock_low_in_writeset(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL sock_low_in_exceptset(SOCK_LOW_HANDLE_TYPE lower_handle)
{
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID sock_low_clear_all_sets(LSA_VOID)
{
	FD_ZERO(&readSocketDescriptorSet);
	FD_ZERO(&writeSocketDescriptorSet);
	FD_ZERO(&exceptSocketDescriptorSet);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_NTOHS(LSA_UINT16 parameter)
{
	return ntohs(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_NTOHL(LSA_UINT32 parameter)
{
	return ntohl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_HTONS(LSA_UINT16 parameter)
{
	return htons(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_HTONL(LSA_UINT32 parameter)
{
	return htonl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT8
sock_low_get_reason_code(
	int err
) {
	switch(err)
	{
	case EMSGSIZE: return SOCK_IPSTACK_EMSGSIZE;
	case EADDRNOTAVAIL: /* see in_pcbconnect() */
	case ENP_NO_ROUTE: return SOCK_IPSTACK_EHOSTUNREACH;
	default: return 0;
	}
}

/*****************************************************************************/
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
