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
/*  F i l e               &F: sock_sun.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID 11
#define SOCK_MODULE_ID    11

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_CUSTOM
/*****************************************************************************/

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <fcntl.h>

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
typedef LSA_INT32 SOCK_LOW_FD_TYPE;

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

LSA_VOID
sock_low_init(
	void
) {
	if (sizeof(SOCK_LOW_HANDLE_TYPE) < sizeof(SOCK_LOW_FD_TYPE))  {
		SOCK_FATAL();
	}
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE
sock_low_socket(
	int type,
	int* err
) {
	/* type of socket descriptor: see "old" SOCKIF_FD defintions in sock_low.h */
	SOCK_LOW_FD_TYPE s =  socket(PF_INET, (type == SOCK_LOW_STREAM) ? SOCK_STREAM : SOCK_DGRAM, 0);

	if (s < 0)
	{
		*err = errno;

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
	if( bind((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

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

LSA_RESPONSE_TYPE
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int backlog,
	int* err
) {
	if( listen((SOCK_LOW_FD_TYPE)lower_handle, backlog) < 0 )
	{
		*err = errno;

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
	if( connect((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

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

	SOCK_LOW_FD_TYPE s = accept((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, &address_len);

	if (s < 0)
	{
		*err = errno;

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
	int ret = close((SOCK_LOW_FD_TYPE)lower_handle);

	if (ret< 0)
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

	case EPIPE: /* socket is not connected */
	case ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case ENOTCONN: /* -"- */
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
	*sent = send((SOCK_LOW_FD_TYPE)lower_handle
				 , buffer
				 , length
				 , 0
				 );

	if (*sent < 0)
	{
		*err = errno;

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
	*received = recv((SOCK_LOW_FD_TYPE)lower_handle
					 , buffer
					 , length
					 , (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
					 );

	if (*received < 0)
	{
		*err = errno;

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

	*sent = sendto((SOCK_LOW_FD_TYPE)lower_handle
				   , buffer
				   , length
				   , 0
				   , (struct sockaddr*)address
				   , (address) ? sizeof(*address) : 0
				   );

	if (*sent < 0)
	{
		*err = errno;

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
	int address_len = sizeof(*address);

	*received = recvfrom((SOCK_LOW_FD_TYPE)lower_handle
						 , buffer
						 , length
						 , (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
						 , (struct sockaddr*)address
						 , &address_len
						 );
	if (*received < 0)
	{
		*err = errno;

		return sock_low_map_udp_error(*err);
	}

	local_address->sin_family = 0;
	local_address->sin_addr.s_addr = 0; /* local address not supported */

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
	int* err
) {
	int len = sizeof(*socket_error);

	int ret = getsockopt((SOCK_LOW_FD_TYPE)lower_handle
						 , SOL_SOCKET
						 , SO_ERROR
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
		{
			LSA_INT flags = fcntl(s, F_GETFL);

			if (flags != -1)
			{
				if (*(LSA_INT32*)option_value)
				{
					ret = fcntl(s, F_SETFL, flags | O_NONBLOCK);
				}
				else
				{
					ret = fcntl(s, F_SETFL, flags & ~O_NONBLOCK);
				}
			}
			else
			{
				ret = 1;
			}
		}
		break;

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
		struct linger linger_val;
		linger_val.l_linger = *(int*)option_value;
		linger_val.l_onoff  = 1;
		ret = setsockopt(s, SOL_SOCKET, SO_LINGER, &linger_val, sizeof(linger_val));
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
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE
sock_low_get_sockname(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
) {
	int address_len = sizeof(*address);

	if( getsockname((SOCK_LOW_FD_TYPE)lower_handle, (struct sockaddr*)address, &address_len) < 0 )
	{
		*err = errno;

		return SOCK_RSP_ERR_LOC_NET;
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32
sock_low_select(
	int timeout
) {
	struct timeval tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return select(maxfds+1
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
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID
sock_low_clear_all_sets(
	LSA_VOID
) {
	FD_ZERO(&readSocketDescriptorSet);
	FD_ZERO(&writeSocketDescriptorSet);
	FD_ZERO(&exceptSocketDescriptorSet);

	maxfds = 0;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
SOCK_NTOHS(
	LSA_UINT16 parameter
) {
	return ntohs(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32
SOCK_NTOHL(
	LSA_UINT32 parameter
) {
	return ntohl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
SOCK_HTONS(
	LSA_UINT16 parameter
) {
	return htons(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32
SOCK_HTONL(
	LSA_UINT32 parameter
) {
	return htonl(parameter);
}

/*----------------------------------------------------------------------------*/

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
