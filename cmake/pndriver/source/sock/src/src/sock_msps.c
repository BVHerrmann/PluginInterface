/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: sock_msps.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.13         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-02-26                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Portability layer for socket access                                      */
/*  This module supports a portability layer for socket functions.           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 18
#define SOCK_MODULE_ID    18

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_CUSTOM
/*****************************************************************************/

#include "pnio_msps_os.h"

/*------------------------------------------------------------------------------
// IP Stack's socket descriptor type
//----------------------------------------------------------------------------*/
typedef MSPS_FD_TYPE SOCK_LOW_FD_TYPE;

/*------------------------------------------------------------------------------
// native socket descriptor sets, see sock_low_select
//----------------------------------------------------------------------------*/
static MSPS_SOCK_FD_SET readSocketDescriptorSet;
static MSPS_SOCK_FD_SET writeSocketDescriptorSet;
static MSPS_SOCK_FD_SET exceptSocketDescriptorSet;

static SOCK_LOW_FD_TYPE maxfds;

LSYS_SEM_ID     g_sock_msps_semId;
SOCK_LOW_HANDLE_TYPE g_sock_msps_sock_fd;
LSA_UINT32      g_sock_msps_buffer;
LSA_UINT32      g_sock_msps_length;
SOCK_SOCKADDR_TYPE   g_sock_msps_address;

/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

static LSA_UINT32 sock_low_sendto_task( SOCK_LOW_HANDLE_TYPE sock_fd,LSA_VOID* buffer,LSA_UINT32 length,SOCK_SOCKADDR_TYPE* address )
{
	assert(length==sizeof(LSA_UINT32));
	g_sock_msps_sock_fd=sock_fd;
	g_sock_msps_buffer=*((LSA_UINT32*)buffer);
	g_sock_msps_length=length;
	memcpy(&g_sock_msps_address,address,sizeof(g_sock_msps_address));
	LSYS_SEM_GIVE(g_sock_msps_semId);
	return length;
}

/* TFS-RQ 555728 - avoid blocking of timer task with ISS semaphores */
static LSA_VOID *sock_low_main_task( void *p)
{
	SOCK_LOG_FCT("sock_low_main_task")
	LSA_INT32 ret;

	LSA_UNUSED_ARG(p);

	while (1)
	{
		LSYS_SEM_TAKE(g_sock_msps_semId);
		ret=PnioMspsSockSendto((SOCK_LOW_FD_TYPE)g_sock_msps_sock_fd, (char *)&g_sock_msps_buffer, (int)g_sock_msps_length, 0, (MSPS_SOCK_ADDR_TYPE*)&g_sock_msps_address, sizeof(MSPS_SOCK_ADDR_TYPE));
		if ( ret!=g_sock_msps_length ) {
			/* this can happen in VW-Setup */
			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "error PnioMspsSockSendto (ret!=g_sock_msps_length) 0x%08x!=0x%08x", ret,g_sock_msps_length);
		}
	}
	return NULL;
}


LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_low_init(
    LSA_VOID
)
{
	SOCK_LOG_FCT("sock_low_init")

	int state;
	pthread_attr_t                        attr;
	struct sched_param                    param;
	pthread_t                             tID;

	SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "sock_low_init: Initialize socket interface");


	g_sock_msps_semId = LSYS_SEM_CREATE();
	assert(NULL!=g_sock_msps_semId);

	/* initialize a free-running parallel thread */
	PTHREAD_ATTR_INIT (&attr);
	PTHREAD_ATTR_SETDETACHSTATE (&attr, PTHREAD_CREATE_DETACHED);
	PTHREAD_ATTR_SETINHERITSCHED (&attr, PTHREAD_EXPLICIT_SCHED);
	PTHREAD_ATTR_SETSCHEDPOLICY (&attr, SCHED_PT2POSIX);
	PTHREAD_ATTR_SETNAME(&attr, "SliX");
	PTHREAD_ATTR_SETSTACKSIZE(&attr, 0x20000);

	/* initialize thread's priority value */
	param.sched_priority = 18;
	state = PTHREAD_ATTR_SETSCHEDPARAM (&attr, &param);
	assert(state == 0);

	/* launch the new thread */
	state = PTHREAD_CREATE (&tID, &attr,  sock_low_main_task, (void *) 0);
	assert(state == 0);

	if( sizeof(SOCK_LOW_HANDLE_TYPE) < sizeof(SOCK_LOW_FD_TYPE))
	{
		SOCK_FATAL();
	}
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE  SOCK_LOCAL_FCT_ATTR
sock_low_socket(
	LSA_INT32 type,
	int* err
)
{
	SOCK_LOG_FCT("sock_low_socket")

	SOCK_LOW_FD_TYPE s;

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_socket: type=%d", type);

	s = PnioMspsSockSocket(MSPS_AF_INET, (int)(type == SOCK_LOW_STREAM) ? MSPS_SOCK_STREAM : MSPS_SOCK_DGRAM, 0);

	if (s == MSPS_INVALID_FD)
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "<<< sock_low_socket failed, errno(%d)", errno);

		*err = errno;

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_socket: fd=0x%08x", s);

		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	LSA_INT *err
)
{
	SOCK_LOG_FCT("sock_low_bind")

	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;
	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_bind: fd=0x%08x", lower_handle);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	if (PnioMspsSockBind((SOCK_LOW_FD_TYPE)lower_handle, (MSPS_SOCK_ADDR_TYPE*)address, sizeof(MSPS_SOCK_ADDR_TYPE)) < 0)
	{
		*err = errno;

		switch(*err)
		{
		case EADDRINUSE:
		case MSPS_EADDRINUSE: /* MSPS specific */
			return SOCK_RSP_ERR_ADDR_INUSE;

		case ENOMEM:
		case ENOBUFS:
		case MSPS_ENOMEM: /* MSPS specific */
		/* case MSPS_ENOBUFS: MSPS specific, MSPS_ENOBUFS = MSPS_ENOMEM */
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_bind: fd=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 backlog,
	LSA_INT* err
)
{
	SOCK_LOG_FCT("sock_low_listen")

	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;
	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_listen: fd=0x%08x, backlog=%d", lower_handle, backlog);

	if (PnioMspsSockListen((SOCK_LOW_FD_TYPE)lower_handle, (int)backlog) < 0)
	{
		*err = errno;

		switch(*err)
		{
		case EADDRINUSE:
		case MSPS_EADDRINUSE: /* MSPS specific */
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_listen: fd=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_connect_error(
	int error
) {
	switch(error)
	{
	case EINPROGRESS:
	case MSPS_EINPROGRESS: /* MSPS specific */
	case EWOULDBLOCK:
	case MSPS_EWOULDBLOCK: /* MSPS specific */
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
	case MSPS_ENOMEM: /* MSPS specific */
        /* case MSPS_ENOBUFS: MSPS specific MSPS_ENOBUFS = MSPS_ENOMEM */
		return SOCK_RSP_ERR_RESOURCE;

	case ECONNREFUSED:
	case MSPS_ECONNREFUSED: /* MSPS specific */
		return SOCK_RSP_ERR_REM_ABORT;

	case ENETUNREACH:
	case ETIMEDOUT:
	case MSPS_ENETUNREACH: /* MSPS specific */
	case MSPS_ETIMEDOUT:  /* MSPS specific */
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
)
{
	SOCK_LOG_FCT("sock_low_connect")

	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;
	SOCK_LOWER_TRACE_07(0, LSA_TRACE_LEVEL_NOTE,
						">>> sock_low_connect: fd=0x%08x, fam 0x%x, port 0x%x, ip %u.%u.%u.%u ",
						lower_handle,
						(unsigned int)address->sin_family,
						(unsigned int)address->sin_port,
						(unsigned int)address->sin_addr.S_un.S_un_b.s_b1,
						(unsigned int)address->sin_addr.S_un.S_un_b.s_b2,
						(unsigned int)address->sin_addr.S_un.S_un_b.s_b3,
						(unsigned int)address->sin_addr.S_un.S_un_b.s_b4);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	if (PnioMspsSockConnect((SOCK_LOW_FD_TYPE)lower_handle, (MSPS_SOCK_ADDR_TYPE*)address, sizeof(MSPS_SOCK_ADDR_TYPE)) != 0)
	{
		*err = errno;

		ret = sock_low_map_connect_error(*err);
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_connect: fd=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

SOCK_LOW_HANDLE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_accept(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
)
{
	SOCK_LOG_FCT("sock_low_accept")

	int address_len = sizeof(MSPS_SOCK_ADDR_TYPE);
	SOCK_LOW_FD_TYPE s;

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_accept: fd=0x%08x", lower_handle);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	s = PnioMspsSockAccept((SOCK_LOW_FD_TYPE)lower_handle, (MSPS_SOCK_ADDR_TYPE*)address, &address_len);

	if (s != MSPS_INVALID_FD)
	{
		SOCK_LOWER_TRACE_06(0, LSA_TRACE_LEVEL_NOTE, "sock_low_accept: fam 0x%x, port 0x%x, ip %u.%u.%u.%u ",
							(unsigned int)address->sin_family,
							(unsigned int)address->sin_port,
							(unsigned int)address->sin_addr.S_un.S_un_b.s_b1,
							(unsigned int)address->sin_addr.S_un.S_un_b.s_b2,
							(unsigned int)address->sin_addr.S_un.S_un_b.s_b3,
							(unsigned int)address->sin_addr.S_un.S_un_b.s_b4);

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_accept: ret_fd=0x%08x", s);

		return (SOCK_LOW_HANDLE_TYPE)s;
	}
	else
	{
		*err = errno;

		SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "<<< sock_low_accept: lower_handle=0x%08x failed, errno(%d)", lower_handle, errno);

		return SOCK_LOW_INVALID_HANDLE;
	}
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Shutdown(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 how
)
{
	SOCK_LOG_FCT("SockIF_Shutdown")

	LSA_BOOL ret = LSA_TRUE;
	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>> SockIF_Shutdown: fd=0x%08x, how=%d", lower_handle, how);

	if (PnioMspsSockShutdown((SOCK_LOW_FD_TYPE)lower_handle, (int)how) != 0)
	{
		ret = LSA_FALSE;
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< SockIF_Shutdown: fd=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
)
{
	SOCK_LOG_FCT("sock_low_close")

	LSA_RESPONSE_TYPE ret;
	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_close: fd=0x%08x", lower_handle);

	if (PnioMspsSockClose((SOCK_LOW_FD_TYPE)lower_handle) != 0)
	{
		ret = SOCK_RSP_ERR_LOC_NET;
	}
	else
	{
		ret = SOCK_RSP_OK;
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_close: lower_handle=0x%08x, ret=%d", lower_handle, (int)ret);
	return ret;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_tcp_error(
	int error
) {
	switch(error)
	{
    case EWOULDBLOCK:
    case MSPS_EWOULDBLOCK: /* MSPS specific */
		return SOCK_RSP_OK_ACTIVE;

	case ECONNABORTED:
	case ECONNRESET:
	case ESHUTDOWN:
	case MSPS_ECONNRESET: /* MSPS specific */
		return SOCK_RSP_ERR_REM_ABORT;

	case EPIPE: /* socket is not connected */
	case ETIMEDOUT:
	case MSPS_ETIMEDOUT: /* MSPS specific */
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENOBUFS:
	case MSPS_ENOMEM: /* MSPS specific */
	/* case MSPS_ENOBUFS: MSPS specific MSPS_ENOBUFS = MSPS_ENOMEM */
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
)
{
	*sent = PnioMspsSockSend((SOCK_LOW_FD_TYPE)lower_handle
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

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_recv(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	int* received,
	int* err
)
{
	*received = PnioMspsSockRecv((SOCK_LOW_FD_TYPE)lower_handle
								, buffer
								, length
								, (flags & SOCK_LOW_PEEK) ? MSPS_MSG_PEEK : 0
								);

	if (*received < 0)
	{
		*err = errno;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_udp_error(
	int err
)
{
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case EWOULDBLOCK:
	case MSPS_EWOULDBLOCK: /* MSPS specific */
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
	case MSPS_ENOMEM: /* MSPS specific */
	/* case MSPS_ENOBUFS: MSPS specific MSPS_ENOBUFS = MSPS_ENOMEM */
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
)
{
	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;
	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	if ( 4<length )
	{
		/* RQ 857984: Sock buffer size can be more than 4 bytes, in case of udp_send. */
		*sent = PnioMspsSockSendto((SOCK_LOW_FD_TYPE)lower_handle
									, buffer
									, length
									, 0
									, (MSPS_SOCK_ADDR_TYPE*)address
									, (address) ? sizeof(MSPS_SOCK_ADDR_TYPE) : 0
									);
	}
	else
	{
		/* TFS-RQ 555728 - avoid blocking of timer task with ISS semaphores */
		*sent = sock_low_sendto_task(lower_handle, buffer, length, address);
	}

	if (*sent < 0)
	{
		*err = errno;

		ret = sock_low_map_udp_error(*err);
	}

	return ret;
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
)
{
	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;

	int address_len = sizeof(MSPS_SOCK_ADDR_TYPE);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	*received = PnioMspsSockRecvfrom((SOCK_LOW_FD_TYPE)lower_handle
									, buffer
									, length
									, (flags & SOCK_LOW_PEEK) ? MSPS_MSG_PEEK : 0
									, (MSPS_SOCK_ADDR_TYPE*)address
									, &address_len
									);
	if (*received < 0)
	{
		*err = errno;

		ret = sock_low_map_udp_error(*err);
	}

	return ret;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_getsockopt(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    SOCK_LOW_SOCKOPT_TYPE option_name,
    LSA_VOID* option_value,
	LSA_INT* err
)
{
	SOCK_LOG_FCT("sock_low_getsockopt")

	int len = sizeof(LSA_INT);
	int ret;
	LSA_BOOL bret = LSA_TRUE;

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_getsockopt: fd=0x%08x, option=%d", lower_handle, (int)option_name);

	switch (option_name)
	{
	case sock_low_sockopt_Type:
		ret = PnioMspsSockGetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_TYPE, (char *)option_value, &len);
		break;

	case sock_low_sockopt_Error:
		ret = PnioMspsSockGetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_ERROR, (char *)option_value, &len);
		break;

	case sock_low_sockopt_SendBuffer:
		ret = PnioMspsSockGetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_SNDBUF, (char *)option_value, &len);
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = PnioMspsSockGetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_RCVBUF, (char *)option_value, &len);
		break;

	default:
		ret = -1;
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "option not supported by this function");
		break;
	}

	if (ret < 0)
	{
		*err = errno;
		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "error get socket option %d, errno=%d", option_name, *err);
		bret = LSA_FALSE;
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_getsockopt: fd=0x%08x, ret=%d", lower_handle, (int)bret);
	return (bret);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT* socket_error,
	LSA_INT* err
)
{
	return sock_low_getsockopt(lower_handle, sock_low_sockopt_Error, socket_error, err);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_setsockopt(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    SOCK_LOW_SOCKOPT_TYPE option_name,
    LSA_VOID* option_value,
	LSA_INT* err
)
{
	SOCK_LOG_FCT("sock_low_setsockopt")

	int ret;
	LSA_BOOL bret = LSA_TRUE;

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_setsockopt: fd=0x%08x, option=%d", lower_handle, (int)option_name);

	switch (option_name)
	{
	case sock_low_sockopt_NonBlocking:
		{
			int flags = PnioMspsSockFcntl((SOCK_LOW_FD_TYPE)lower_handle, F_GETFL, 0);

			ret = -1;

			if (flags != -1)
			{
				if (*(LSA_INT32*)option_value)
				{
					ret = PnioMspsSockFcntl((SOCK_LOW_FD_TYPE)lower_handle, F_SETFL, flags | MSPS_SLI_O_NONBLOCK);
				}
				else
				{
					ret = PnioMspsSockFcntl((SOCK_LOW_FD_TYPE)lower_handle, F_SETFL, flags & ~MSPS_SLI_O_NONBLOCK);
				}
			}
		}
		break;

	case sock_low_sockopt_TCPNoDelay:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_IPPROTO_TCP, MSPS_TCP_NODELAY, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoAckDelay:
		ret = 0;
		break;

	case sock_low_sockopt_ReuseAddr:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_REUSEADDR, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_KeepAlive:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_KEEPALIVE, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_DontRoute:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_DONTROUTE, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_SendBuffer:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_SNDBUF, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_RCVBUF, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_Linger:
		{
		MSPS_LINGER_PARMS MspsLinger;

		MspsLinger.LingerOnOff = 1;
		MspsLinger.LingerTimer = *(int*)option_value;

		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_LINGER, (char *)&MspsLinger, sizeof(MspsLinger));
		}
		break;

	case sock_low_sockopt_Broadcast:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_BROADCAST, (char *)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_OOBInline:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_SOL_SOCKET, MSPS_SO_OOBINLINE, (char *)option_value, sizeof(LSA_INT32));
		break;

	#if SOCK_CFG_ENABLE_MULTICASTS
	case SockOpt_AddMulticast:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_IPPROTO_IP, MSPS_IP_ADD_MEMBERSHIP, (char *)option_value, 2*sizeof(LSA_INT32));
		break;

	case SockOpt_DropMulticast:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_IPPROTO_IP, MSPS_IP_DROP_MEMBERSHIP, (char *)option_value, 2*sizeof(LSA_INT32));
		break;

	case SockOpt_SetMulticastIf:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_IPPROTO_IP, MSPS_IP_MULTICAST_IF, (char *)option_value, 2*sizeof(LSA_INT32));
		break;

	case SockOpt_IpMulticastLoop:
		ret = PnioMspsSockSetsockopt((SOCK_LOW_FD_TYPE)lower_handle, MSPS_IPPROTO_IP, MSPS_IP_MULTICAST_LOOP, (char *)option_value, 2*sizeof(LSA_INT32));
		break;
	#endif

	default:
		ret = -1;
		break;
	}

	if (ret < 0)
	{
		*err = errno;

		bret = LSA_FALSE;
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "<<< sock_low_setsockopt: fd=0x%08x option=%d failed errno=%d", lower_handle, option_name, *err);
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_setsockopt: fd=0x%08x, ret=%d", lower_handle, (int)bret);
	return bret;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_get_sockname(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    SOCK_SOCKADDR_TYPE* address,
	LSA_INT* err
)
{
	SOCK_LOG_FCT("sock_low_get_sockname")

	LSA_RESPONSE_TYPE ret = SOCK_RSP_OK;
	int address_len = sizeof(MSPS_SOCK_ADDR_TYPE);
	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_get_sockname: fd=0x%08x", lower_handle);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	if (PnioMspsSockGetsockname((SOCK_LOW_FD_TYPE)lower_handle, (MSPS_SOCK_ADDR_TYPE*)address, &address_len) != 0)
	{
		*err = errno;
		ret = SOCK_RSP_ERR_LOC_NET;
		SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "<<< sock_low_get_sockname: fd=0x%08x failed errno=%d", lower_handle, *err);
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_get_sockname: fd=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_GetPeerName(
    SOCK_LOW_HANDLE_TYPE lower_handle,
    SOCK_SOCKADDR_TYPE* address
)
{
	SOCK_LOG_FCT("SockIF_GetPeerName")

	LSA_BOOL ret = LSA_TRUE;
	int address_len = sizeof(MSPS_SOCK_ADDR_TYPE);
	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> SockIF_GetPeerName: lower_handle=0x%08x", lower_handle);

	/* "SOCK_SOCKADDR_TYPE" and "MSPS_SOCK_ADDR_TYPE" have to be of same size to make cast working */
	SOCK_ASSERT(sizeof(MSPS_SOCK_ADDR_TYPE) == sizeof(SOCK_SOCKADDR_TYPE));

	if (PnioMspsSockGetpeername((SOCK_LOW_FD_TYPE)lower_handle, (MSPS_SOCK_ADDR_TYPE*)address, &address_len) != 0)
	{
		ret = LSA_FALSE;
	}

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "<<< SockIF_GetPeerName: lower_handle=0x%08x, ret=%d", lower_handle, (int)ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_INT32 SOCK_LOCAL_FCT_ATTR
sock_low_select(
	int timeout
)
{
	SOCK_LOG_FCT("sock_low_select")

	LSA_INT32 ret;
	MSPS_TIMEVAL tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>> sock_low_select: timeout=%d, maxfd=%d", timeout, maxfds);

	ret = (LSA_INT32)PnioMspsSockSelect(maxfds /* sizeof(SOCKIF_FD_SET_TYPE) * 8 */,
										&readSocketDescriptorSet,
										&writeSocketDescriptorSet,
										&exceptSocketDescriptorSet,
										&tv);

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<<< sock_low_select: ret=%d", ret);
	return (ret);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_readset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	MSPS_FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	MSPS_FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	MSPS_FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return MSPS_FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return MSPS_FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return MSPS_FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_clear_all_sets(
	LSA_VOID
) {
	MSPS_FD_ZERO(&readSocketDescriptorSet);
	MSPS_FD_ZERO(&writeSocketDescriptorSet);
	MSPS_FD_ZERO(&exceptSocketDescriptorSet);

	maxfds = 0;
}


/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Sleep(
    LSA_INT32 timeout
)
{
	SOCK_LOG_FCT("SockIF_Sleep")

	MSPS_TIMEVAL tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>> SockIF_Sleep: timeout=%d", timeout);

	PnioMspsSockSelect(sizeof(MSPS_SOCK_FD_SET) * 8, NULL, NULL, NULL, &tv);

	SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "<<< SockIF_Sleep");
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

#endif /* SOCK_CFG_MSPS */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
