#define LTRC_ACT_MODUL_ID 17
#define SOCK_MODULE_ID    17

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_CUSTOM
/*****************************************************************************/

#include <usertype.h>
#include <netmain.h>
#include <socket.h>

/*------------------------------------------------------------------------------
// checks
//----------------------------------------------------------------------------*/
#if SOCK_AF_INET != AF_INET /* AP01145901 */
#error SOCK_AF_INET differs from AF_INET
#endif

#if SOCK_CFG_MAX_SOCKETS > FD_SETSIZE
#error FD_SETSIZE too small and override not possible
#endif

/*------------------------------------------------------------------------------
// IP Stack's socket descriptor type
//----------------------------------------------------------------------------*/
typedef SOCKET SOCK_LOW_FD_TYPE;

/*------------------------------------------------------------------------------
// native socket descriptor sets, see sock_low_select
//----------------------------------------------------------------------------*/
static fd_set readSocketDescriptorSet;
static fd_set writeSocketDescriptorSet;
static fd_set exceptSocketDescriptorSet;

static SOCK_LOW_FD_TYPE maxfds;

/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_init(
	LSA_VOID
) {
	if (sizeof(SOCK_LOW_HANDLE_TYPE) < sizeof(SOCK_LOW_FD_TYPE)) {
		SOCK_FATAL();
}
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_socket(
	LSA_INT32 type,
	int* err
) {
	SOCK_LOW_FD_TYPE s = socket(AF_INET, (type == SOCK_LOW_STREAM) ? SOCK_STREAM : SOCK_DGRAM, 0);

	if (s == INVALID_SOCKET)
	{
		*err = fdError();

		return SOCK_LOW_INVALID_HANDLE;
}
	else
	{
		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT *err
) {
	struct sockaddr addr;

	addr.sa_family = AF_INET; /* always, see sock_low_socket() */
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( bind((SOCK_LOW_FD_TYPE)lower_handle, &addr, sizeof(addr)) < 0 )
	{
		*err = fdError();

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case ENOMEM:
		case ENOBUFS:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 backlog,
	LSA_INT* err
) {
	if( listen((SOCK_LOW_FD_TYPE)lower_handle, backlog) < 0 )
	{
		*err = fdError();

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

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_connect_error
	(int error
) {
	switch(error)
	{
	case EINPROGRESS:
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_connect(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
) {
	struct sockaddr addr;

	addr.sa_family = AF_INET; /* always, see sock_low_socket() */
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( connect((SOCK_LOW_FD_TYPE)lower_handle, &addr, sizeof(addr)) < 0 )
	{
		*err = fdError();

		return sock_low_map_connect_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_accept(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	SOCK_LOW_FD_TYPE s;
	struct sockaddr addr;
	int addr_len = sizeof(addr);

	s = accept((SOCK_LOW_FD_TYPE)lower_handle, &addr, &addr_len);

	if (s == INVALID_SOCKET)
	{
		*err = fdError();

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		address->sin_family = addr.sa_family;
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);

		return (SOCK_LOW_HANDLE_TYPE)s;
}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	if( fdClose((SOCK_LOW_FD_TYPE)lower_handle) < 0 )
	{
		return SOCK_RSP_ERR_LOC_NET;
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
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

	case ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_send(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int* sent,
	int* err
) {
	*sent = send((SOCK_LOW_FD_TYPE)lower_handle
				 , buffer
				 , length
				 , 0
				 );

	if (*sent < 0)
	{
		*err = fdError();

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_recv(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	int* received,
	int* err
) {
	*received = recv((SOCK_LOW_FD_TYPE)lower_handle
					 , buffer
					 , length
					 , (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
					 );

	if (*received < 0)
	{
		*err = fdError();

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_udp_error(
	int err
) {
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_sendto(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	SOCK_SOCKADDR_TYPE* address,
	int* sent,
	int* err
) {
	struct sockaddr addr;
	struct sockaddr* addr_p;

	if (address)
	{
		addr.sa_family = AF_INET; /* always, see sock_low_socket */
		SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

		addr_p = &addr;
	}
	else
	{
		addr_p = 0;
	}

	*sent = sendto((SOCK_LOW_FD_TYPE)lower_handle
				   , buffer
				   , length
				   , 0
				   , addr_p
				   , (addr_p) ? sizeof(*addr_p) : 0
				   );

	if (*sent < 0)
	{
		*err = fdError();

		return sock_low_map_udp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_recvfrom(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	SOCK_SOCKADDR_TYPE* address,
	int* received,
	int* err
) {
	struct sockaddr addr;
	LSA_INT addr_len = sizeof(addr);

	*received = recvfrom((SOCK_LOW_FD_TYPE)lower_handle
						 , buffer
						 , length
						 , (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
						 , &addr
						 , &addr_len
						 );

	if (*received < 0)
	{
		*err = fdError();

		return sock_low_map_udp_error(*err);
	}
	else
	{
		address->sin_family = addr.sa_family; /* see check above */
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
	int* err
) {
	LSA_INT len = sizeof(int);

	int ret = getsockopt((SOCK_LOW_FD_TYPE)lower_handle, SOL_SOCKET, SO_ERROR, socket_error, &len);

	if (ret < 0)
	{
		*err = fdError();

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
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
        {
            int sockblk = 0;
            ret = setsockopt(s,SOL_SOCKET,SO_BLOCKING,&sockblk,sizeof(sockblk));
            break;
        }
	case sock_low_sockopt_TCPNoDelay:
		ret = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoAckDelay:
		ret = 0;
		break;

	case sock_low_sockopt_ReuseAddr:
		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_KeepAlive:
		ret = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_DontRoute:
		ret = setsockopt(s, SOL_SOCKET, SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_SendBuffer:
		ret = setsockopt(s, SOL_SOCKET, SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = setsockopt(s, SOL_SOCKET, SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_Linger:
		{
		struct linger linger_value;
		linger_value.l_linger = *(int*)option_value;
		linger_value.l_onoff  = 1;
		ret = setsockopt(s, SOL_SOCKET, SO_LINGER, (void*)&linger_value, sizeof(linger_value));
		}
		break;

	case sock_low_sockopt_Broadcast:
		ret = setsockopt(s, SOL_SOCKET, SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_OOBInline:
		ret = setsockopt(s, SOL_SOCKET, SO_OOBINLINE, option_value, sizeof(LSA_INT32));
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
		*err = fdError();

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_get_sockname(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
) {
	struct sockaddr addr;
	int addr_len = sizeof(addr);

	if( getsockname((SOCK_LOW_FD_TYPE)lower_handle, &addr, &addr_len) < 0 )
	{
		*err = fdError();

		return SOCK_RSP_ERR_LOC_NET;
	}
	else
	{
		address->sin_family = addr.sa_family;
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32 SOCK_LOCAL_FCT_ATTR
sock_low_select(
	int timeout
) {
	struct timeval tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return fdSelect(maxfds+1
		, &readSocketDescriptorSet
		, &writeSocketDescriptorSet
		, &exceptSocketDescriptorSet
		, &tv);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_readset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}
/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_clear_all_sets(
	LSA_VOID
) {
	FD_ZERO(&readSocketDescriptorSet);
	FD_ZERO(&writeSocketDescriptorSet);
	FD_ZERO(&exceptSocketDescriptorSet);

	maxfds = 0;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_UPPER_IN_FCT_ATTR
SOCK_NTOHS(
	LSA_UINT16 parameter
) {
	return ntohs(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_UPPER_IN_FCT_ATTR
SOCK_NTOHL(
	LSA_UINT32 parameter
) {
	return ntohl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_UPPER_IN_FCT_ATTR
SOCK_HTONS(
	LSA_UINT16 parameter
) {
	return htons(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_UPPER_IN_FCT_ATTR
SOCK_HTONL(
	LSA_UINT32 parameter
) {
	return htonl(parameter);
}

/*----------------------------------------------------------------------------*/

#endif

/*****************************************************************************/
/*  Copyright (C) 2014 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
