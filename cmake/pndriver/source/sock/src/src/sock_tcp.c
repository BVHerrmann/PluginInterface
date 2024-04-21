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
/*  F i l e               &F: sock_tcp.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Productive part of the Socket-Interface                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 5
#define SOCK_MODULE_ID    5

#include "sock_int.h"

#if SOCK_CFG_OPEN_BSD /* OBSD_ITGR -- listen is defined as obsd_listen in rename_syscalls */
#undef listen
#endif

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*------------------------------------------------------------------------------
// SOCK_OPC_LISTEN: create listening server socket at passed address/port
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_listen(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_listen")

	SOCK_LOW_HANDLE_TYPE lower_handle;
	int native_error;
	LSA_RESPONSE_TYPE response;
	SOCK_RQB_ARGS_LISTEN_PTR_TYPE listen;

	listen = & rqb->args.listen;

	lower_handle = sock_low_socket(SOCK_LOW_STREAM, &native_error);

	if (lower_handle == SOCK_LOW_INVALID_HANDLE)
	{
		response = SOCK_RSP_ERR_RESOURCE;

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_SOCKET_PTR soc = LSA_NULL;

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, lower_handle(0x%x)"
			, lower_handle
			);

		soc = sock_socket_alloc(channel, lower_handle, SOCK_LOW_STREAM);

		if ( sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;

			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct alloc failed, lower_handle(0x%x), response(%d)"
				, lower_handle, response
				);
		}
		else
		{
			LSA_INT16       backlog;
			SOCK_SOCKADDR_TYPE   *loc_addr_p;

			loc_addr_p = &listen->sockaddr;

			response = sock_low_bind(soc->lower_handle, loc_addr_p, &native_error);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
					, "bind() failed for TCP lower_handle(0x%x), native_error(%d) response(%d)"
					, soc->lower_handle, native_error, response
					);

				sock_socket_free(soc);
			}
			else
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "bind() ok for TCP lower_handle(0x%x)"
					, soc->lower_handle
					);

				soc->rem_addr.sin_family = SOCK_AF_INET;

				response = sock_low_get_sockname(soc->lower_handle, &listen->sockaddr, &native_error);

				if (response != SOCK_RSP_OK)
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "getsockname() failed for lower_handle(0x%x), native_error(%d), resonse(%d)"
							, soc->lower_handle, native_error, response
							);

						sock_socket_free(soc);
				}
				else
				{
					listen->sock_fd = soc->dev_handle;     /* return internal handle (not socket descriptor!) */

					soc->loc_addr = listen->sockaddr;

					backlog = listen->backlog;

					response = sock_low_listen(lower_handle, backlog, &native_error);

					if (response != SOCK_RSP_OK)
					{
						SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "listen() failed for lower_handle(0x%x), native_error(%d), resonse(%d)"
							, soc->lower_handle, native_error, response
							);

						sock_socket_free(soc);
					}
					else
					{
						SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
							, "lower_handle: 0x%x SRV LISTEN   OK"
							, lower_handle
							);

						soc->state = SOCK_CN_STATE_LISTENING;
#if SOCK_INT_CFG_SOCKET_EVENTS
						soc->server_state = SOCK_CN_STATE_LISTENING; /* initially, equal to state, see 2004942 */
#endif
						SOCK_PROTOCOL_TRACE_03(0,LSA_TRACE_LEVEL_NOTE
							, "listening socket 0x%x for own Port %d with internal ConnectionStructure %d created "
							, soc->lower_handle, SOCK_HTONS(soc->loc_addr.sin_port), soc->dev_handle
							);
					}
				}
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// do remaining work after accept() system call, return rqb
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_tcp_do_accept(
	SOCK_SOCKET_PTR listening_soc,
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE new_addr
) {
	SOCK_LOG_FCT("sock_tcp_do_accept")

	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR new_soc;

	/* check returned address info and available resources */
	if (new_addr.sin_family != SOCK_AF_INET)
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "address family not AF_INET for sock_fd(0x%x) CLT ACCEPT   ERR (invalid address)"
			, lower_handle
			);

		sock_low_close(lower_handle); /* accept notify rqb stays active */
	}
	else
	{
		int native_error;
		SOCK_SOCKADDR_TYPE loc_addr;

		response = sock_low_get_sockname(lower_handle, &loc_addr, &native_error); /* get local address */

		if (response != SOCK_RSP_OK)
		{
			SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
						, "getsockname failed for sock_fd(0x%X), response(%d), native_error(%d), close socket"
						, lower_handle, response, native_error
						);

				sock_low_close(lower_handle); /* accept notify rqb stays active */
		}
		else
		{
			int i;

			/* search InterfaceID in sock_data */
			for(i=0; i<SOCK_MAX_INTERFACE_ID;i++)
			{
				if (sock_data.addr_info[i].CurrentIPSuite.IpAddr.s_addr == loc_addr.sin_addr.s_addr
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
					|| sock_data.cluster_ip.cluster_info[i].Info.ClusterIPAddress.s_addr == loc_addr.sin_addr.s_addr /* check possible connect to Cluster IP */
#endif
					)
				{
					break; /* found */
				}
			}

			if (i == SOCK_MAX_INTERFACE_ID) /* not found -> close socket */
			{
				SOCK_LOWER_TRACE_05(0, LSA_TRACE_LEVEL_ERROR
						, "address(%d.%d.%d.%d) of socket lower_handle(0x%x) not found in sock_data, close socket"
						, loc_addr.sin_addr.S_un.S_un_b.s_b1, loc_addr.sin_addr.S_un.S_un_b.s_b2, loc_addr.sin_addr.S_un.S_un_b.s_b3, loc_addr.sin_addr.S_un.S_un_b.s_b4
						, lower_handle
						);

				sock_low_close(lower_handle); /* accept notify rqb stays active */
			}
			else
			{
				new_soc = sock_socket_alloc(listening_soc->channel, lower_handle, SOCK_LOW_STREAM); /* closes lower_handle if not successful */

				if (sock_is_null(new_soc))
				{
					SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
						, "socket struct allocation failed for new lower_handle(0x%x), socket closed"
						, lower_handle
						);

					/* accept notify rqb stays active */
				}
				else
				{
					SockIF_IPAddr* p_addr;

					SOCK_RQB_ARGS_CONN_PTR_TYPE conn = & (listening_soc->accept.curr->args.conn);

					new_soc->rem_addr = new_addr;
					new_soc->loc_addr = loc_addr;

					new_soc->state = SOCK_CN_STATE_CONNECTED;

					conn->InterfaceID = sock_data.addr_info[i].InterfaceID;

					conn->loc_addr = new_soc->loc_addr;
					conn->rem_addr = new_soc->rem_addr;
					conn->sock_fd = new_soc->dev_handle;

					SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
						, "new connection established, handle(%d)"
						, new_soc->dev_handle
						);

					p_addr = &new_soc->loc_addr.sin_addr;

					SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
						, "local IP(%d.%d.%d.%d), local Port(%d)"
						, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
						, SOCK_HTONS(new_soc->loc_addr.sin_port)
						);

					p_addr = &new_soc->rem_addr.sin_addr;

					SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
						,"remote IP(%d.%d.%d.%d), remote Port(%d)"
						, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
						, SOCK_HTONS(new_soc->rem_addr.sin_port)
						);

					sock_usr_callback(listening_soc->channel, SOCK_RSP_OK, &listening_soc->accept.curr); /* sets accept.curr to LSA_NULL */

					listening_soc->accept.curr = sock_queue_dequeue(&listening_soc->accept.queue);
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
// create new connection, return rqb
//----------------------------------------------------------------------------*/
LSA_VOID
sock_tcp_accept(
	SOCK_SOCKET_PTR listening_soc
) {
	SOCK_LOG_FCT("sock_tcp_accept")

	int native_error;
	SOCK_LOW_HANDLE_TYPE lower_handle;
	SOCK_SOCKADDR_TYPE new_addr;

	if (sock_is_null(listening_soc->accept.curr))
	{
		SOCK_FATAL();
	}

#if SOCK_INT_CFG_SOCKET_EVENTS

	/* SSLyze patch, see 1868586 */

	for(;;)
	{
		native_error = 0;

		lower_handle = sock_low_accept(listening_soc->lower_handle, &new_addr, &native_error);

		if (lower_handle == SOCK_LOW_INVALID_HANDLE
			&& native_error == OBSD_PNIO_ECONNABORTED)
		{
			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN
				, "accept() failed on handle(%d), sock_fd(0x%x), native_error(ECONNABORTED)"
				, listening_soc->dev_handle, listening_soc->lower_handle
				);

			if (listening_soc->server_state == SOCK_CN_STATE_DAMAGED) /* break from accept loop, see 2004942*/
			{
				break; /* server socket was destroyed by address change */
			}

			/* continue loop */
		}
		else
		{
			if (lower_handle == SOCK_LOW_INVALID_HANDLE)
			{
				int option_val;

				#if !SOCK_CFG_OPEN_BSD
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
					, "accept() failed on handle(%d), sock_fd(0x%x), native_error(%d), n o t returning accept rqb"
					, listening_soc->dev_handle, listening_soc->lower_handle, native_error
					);
				#endif

				sock_low_get_socket_error(listening_soc->lower_handle, &option_val, &native_error);  /* eliminate error condition */

				/* connection not accepted, see below */

				break;
			}
			else
			{
				LSA_INT error;

				LSA_INT32 option_val =  listening_soc->channel->detail_ptr->rec_buffer_max_len;

				if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_RecvBuffer, &option_val, &error))
				{
					sock_low_close(lower_handle);

					SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_WARN
						, "setsockopt() failed on sock_fd(0x%x), native_error(%d)"
						, lower_handle, error
						);

					/* continue loop */
				}
				else
				{
					break; /* accept connection, see below */
				}
			}
		}
	}


	/***/

	if (lower_handle != SOCK_LOW_INVALID_HANDLE)
	{
		sock_tcp_do_accept(listening_soc, lower_handle, new_addr);
	}
#else
	lower_handle = sock_low_accept(listening_soc->lower_handle, &new_addr, &native_error);

	if (lower_handle == SOCK_LOW_INVALID_HANDLE)
	{
		int option_val;

		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
			, "accept() failed on handle(%d), sock_fd(0x%x), native_error(%d), n o t returning accept rqb"
			, listening_soc->dev_handle, listening_soc->lower_handle, native_error
			);

		sock_low_get_socket_error(listening_soc->lower_handle, &option_val, &native_error);  /* eliminate error condition */

		/* accept notify rqb stays active */
	}
	else
	{
		sock_tcp_do_accept(listening_soc, lower_handle, new_addr);
	}
#endif
}


/*------------------------------------------------------------------------------
// SOCK_OPC_OPEN: open socket and allocate socket struct
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_open(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_open")

	SOCK_SOCKET_PTR soc;
	SOCK_LOW_HANDLE_TYPE lower_handle;
	LSA_RESPONSE_TYPE response;
	int native_error;

	soc = LSA_NULL;

	lower_handle = sock_low_socket(SOCK_LOW_STREAM, &native_error);

	if (lower_handle == SOCK_LOW_INVALID_HANDLE)
	{
		response = SOCK_RSP_ERR_RESOURCE; /* not: SOCK_RSP_ERR_LOWER_LAYER (AP01479148) */

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, lower_handle(0x%x)"
			, lower_handle
			);

		soc = sock_socket_alloc(channel, lower_handle, SOCK_LOW_STREAM);

		if (sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;

			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct allocation failed for new lower_handle(0x%x), response(%d)"
				, lower_handle, response
				);
		}
		else
		{
			rqb->args.open.sock_fd = soc->dev_handle;

			soc->state = SOCK_CN_STATE_OPENED;

			response = SOCK_RSP_OK;
		}
	}
	return response;
}


/*------------------------------------------------------------------------------
// bind a port number to the socket and initiate TCP connection
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_connect_with_open(
	SOCK_SOCKET_PTR soc,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_connect_with_open")

	LSA_INT native_error;
	SOCK_SOCKADDR_TYPE   *rem_addr_p, *loc_addr_p;
	LSA_RESPONSE_TYPE response;
	SOCK_RQB_ARGS_CONNECT_WITH_OPEN_PTR_TYPE con;

	/* see 2628554 */
	if (soc->loc_addr.sin_addr.s_addr == 0
		&& soc->loc_addr.sin_port == 0
		&& soc->channel->detail_ptr->socket_option.SO_REUSEADDR_ 
		)
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "attempt to establish connection with unspecified local port on lower_handle(%d)"
			, soc->lower_handle
			);
	}

	response = SOCK_RSP_OK_ACTIVE; /* (!)default return value */

	con = &rqb->args.connect_with_open;

	soc->state = SOCK_CN_STATE_CONNECTING;

	soc->loc_addr.sin_family = con->loc_addr.sin_family;
	soc->loc_addr.sin_port = con->loc_addr.sin_port;
	soc->loc_addr.sin_addr.S_un.S_addr = con->loc_addr.sin_addr.S_un.S_addr;

	loc_addr_p=&soc->loc_addr;   /* avoid warning of Cadul's CXX V401S */

	response = sock_low_bind(soc->lower_handle, loc_addr_p, &native_error);

	if (response != SOCK_RSP_OK)
	{
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
			, "bind() failed for TCP socket lower_handle(0x%x), native_error(%d), response(%d)"
			, soc->lower_handle, native_error, response
			);
	}
	else
	{
		response = sock_low_get_sockname(soc->lower_handle, &con->loc_addr, &native_error);

		if (response != SOCK_RSP_OK)
		{
			SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
				, "getsockname() failed for TCP socket lower_handle(0x%x), native_error(%d), response(%d)"
				, soc->lower_handle, native_error, response
				);
		}
		else
		{
			soc->loc_addr.sin_family = con->loc_addr.sin_family;
			soc->loc_addr.sin_port = con->loc_addr.sin_port;
			soc->loc_addr.sin_addr.S_un.S_addr = con->loc_addr.sin_addr.S_un.S_addr;

			soc->rem_addr.sin_family = con->rem_addr.sin_family;
			soc->rem_addr.sin_port = con->rem_addr.sin_port;
			soc->rem_addr.sin_addr.S_un.S_addr = con->rem_addr.sin_addr.S_un.S_addr;

			rem_addr_p=&soc->rem_addr; /* avoid warning of Cadul's CXX V401S */

			response = sock_low_connect(soc->lower_handle, rem_addr_p, &native_error);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
					, "lower_handle: 0x%x CLT CONNECT  pending..."
					, soc->lower_handle
					);

				if ((response == SOCK_RSP_OK_ACTIVE))  /* wait for accept by server */
				{
					SockIF_IPAddr *p_addr;

					soc->connect_rb = rqb;

					p_addr = & soc->rem_addr.sin_addr;

					SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
						, "SOCK_OPC_CONNECT to remote IP %d.%d.%d.%d at rem_Port %d pending -> socket goes into write mask"
						, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
						, SOCK_HTONS(soc->rem_addr.sin_port)
						);

					response = SOCK_RSP_OK_ACTIVE; /* map 'in progress' */
				}
				else /* return mapped response code */
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
						, "connect() failed for lower_handle(0x%x), native_error(%d), response(%d)"
						, soc->lower_handle, native_error, response
						);

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
					if (response == SOCK_RSP_ERR_LOC_NET)
					{
						con->reason = sock_low_get_reason_code(native_error); /* see 2533568 */
#if SOCK_CFG_INTERNICHE
						if (rem_addr_p->sin_addr.s_addr == 0 /* ANY, see in_pcbconnect() */
							|| rem_addr_p->sin_addr.s_addr == 0xffffffff /* BROADCAST, see in_pcbconnect() */
							|| rem_addr_p->sin_port == 0 /* see in_pcbconnect() */
						)
						{
							con->reason = 0;
						}
#endif
					}
#endif
				}
			}
			else /* server accept */
			{
				soc->connect_rb = rqb;

				sock_tcp_connect_client_socket(soc);     /* obtain address and return rqb */

				response = SOCK_RSP_OK_ACTIVE; /* (!) not SOCK_RSP_OK, see 1187294 */
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CONNECT:bind a port number to the socket and initiate TCP connection
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_connect(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_tcp_connect")

	SOCK_LOW_HANDLE_TYPE lower_handle;
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc = LSA_NULL;
	int native_error;

	lower_handle = sock_low_socket(SOCK_LOW_STREAM, &native_error);

	if (lower_handle == SOCK_LOW_INVALID_HANDLE)
	{
		response = SOCK_RSP_ERR_RESOURCE; /* not: SOCK_RSP_ERR_LOWER_LAYER (AP01479148) */

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
			, "TCP socket open failed, response(%d)"
			, response
			);
	}
	else
	{
		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "TCP socket open ok, lower_handle(0x%x)"
			, lower_handle
			);

		soc = sock_socket_alloc(channel, lower_handle, SOCK_LOW_STREAM);

		if ( sock_is_null(soc) )
		{
			response = SOCK_RSP_ERR_RESOURCE;

			SOCK_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "socket struct alloc failed, lower_handle(0x%x), response(%d)"
				, lower_handle, response
				);
		}
		else
		{
			response = sock_tcp_connect_with_open(soc, rqb);

			if( response != SOCK_RSP_OK_ACTIVE)
			{
				sock_socket_free(soc); /* possible since user has no handle yet */
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// return connect request of client socket
//----------------------------------------------------------------------------*/
LSA_VOID
sock_tcp_connect_client_socket(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_tcp_connect_client_socket")

	SockIF_IPAddr *p_addr;
	SOCK_RQB_ARGS_CONN_PTR_TYPE conn;
	LSA_INT native_error;
	LSA_RESPONSE_TYPE response;

	SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
		, "connect() ok for lower_handle(0x%x)"
		, soc->lower_handle
		);

	soc->state = SOCK_CN_STATE_CONNECTED;

	conn = &soc->connect_rb->args.conn;

	response = sock_low_get_sockname(soc->lower_handle, &soc->loc_addr, &native_error);  /* get local address */

	if (response != SOCK_RSP_OK)
	{
		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
			, "getsockname() failed for TCP socket lower_handle(0x%x), native_error(%d), response(%d)"
			, soc->lower_handle, native_error, response
			);
	}
	else
	{
		conn->loc_addr = soc->loc_addr;

		conn->sock_fd = soc->dev_handle;

		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "tcp connection %d established "
			, soc->dev_handle
			);

		p_addr = &soc->loc_addr.sin_addr;

		SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			, "local IP(%d.%d.%d.%d), local Port(%d)"
			, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
			, SOCK_HTONS(soc->loc_addr.sin_port)
			);

		p_addr = &soc->rem_addr.sin_addr;

		SOCK_PROTOCOL_TRACE_05(0, LSA_TRACE_LEVEL_NOTE
			,"remote IP(%d.%d.%d.%d), remote Port(%d)"
			, p_addr->S_un.S_un_b.s_b1, p_addr->S_un.S_un_b.s_b2, p_addr->S_un.S_un_b.s_b3, p_addr->S_un.S_un_b.s_b4
			, SOCK_HTONS(soc->rem_addr.sin_port)
			);
	}

	sock_usr_callback(soc->channel, response, &soc->connect_rb);

	return;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CANCEL_CONNECTS
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_connect_cancel(
	SOCK_CHANNEL_CONST_PTR_TYPE channel
) {
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc;
	LSA_INT i;

	response = SOCK_RSP_ERR_RESOURCE;

	for (i=0; i<SOCK_CFG_MAX_SOCKETS; i++ )
	{
		if( SOCK_CN_STATE_FREE == sock_data.sockets[i].state )
		{
			continue;
		}

		soc = & sock_data.sockets[i];

		if( sock_are_equal(channel, soc->channel)
			&& soc->state == SOCK_CN_STATE_CONNECTING
			)
		{
			sock_socket_return_connect_resource(soc, SOCK_RSP_ERR_LOC_ABORT);

			response = SOCK_RSP_OK;
		}
	}
	return response;
}


/*------------------------------------------------------------------------------
// read from tcp socket
//
// returns	SOCK_RSP_OK_ACTIVE:		put socket into select set
//			SOCK_RSP_ERR_REM_ABORT: disable socket
//			SOCK_RSP_ERR_RESOURCE:  disable socket
//			SOCK_RESP_ERR_LOC_NET:  disable socket
//          SOCK_RSP_OK:			send success, return rqb
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_receive(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_tcp_receive")

	LSA_RESPONSE_TYPE response;

	data->rem_addr = soc->rem_addr;

	for(;;)
	{
		LSA_INT native_error;
		LSA_INT bytes_to_read = data->data_length - soc->recv.bytes_transferred;
		LSA_INT bytes_offset  = data->offset      + soc->recv.bytes_transferred;
		LSA_INT bytes_read;

		response = sock_low_recv( soc->lower_handle
								, (char*)data->buffer_ptr + bytes_offset
								, bytes_to_read
								, (data->flags & SOCK_PEEK) ? SOCK_LOW_PEEK : 0
								, &bytes_read
								, &native_error
								);

		if( response != SOCK_RSP_OK ) /* failed */
		{
			if( response == SOCK_RSP_OK_ACTIVE ) /* would block -> attempt read in next select call */
			{
				SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "socket handle(%d) stays in read mask"
				, soc->dev_handle
				);

				break; /* wait for next select cycle */
			}
			else /* tcp socket error */
			{
				SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
					, "sock_low_recv on lower_handle(%d) failed, native_error(%d), response(%dx)"
					, soc->lower_handle, native_error, response
					);

				data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* bytes received so far */

				break;
			}
		}
		else /* SOCK_RSP_OK */
		{
			if( bytes_read == 0 ) /* connection was closed by peer */
			{
				SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_WARN
					, "sock_low_recv returned 0 on tcp socket handle(%d)"
					, soc->dev_handle
					);

				data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* bytes received so far */

					response = SOCK_RSP_ERR_REM_ABORT;

				break;
			}
			else /* bytes_read > 0 */
			{
				soc->recv.bytes_transferred += (LSA_UINT16)bytes_read;

				if( soc->recv.bytes_transferred > data->data_length )
				{
						sock_fatal(SOCK_MODULE_ID, SOCK__LINE__); /* unreachable */
				}
				else if( (data->flags & SOCK_READ_ALL) != 0 )
				{
					if( soc->recv.bytes_transferred == data->data_length )
					{
						SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
							, "socket handle(%d) %d bytes received"
							, soc->dev_handle, data->data_length
							);

						data->data_length = (LSA_UINT16)soc->recv.bytes_transferred; /* AP01228536 */

						break;
					}
					else
					{
						continue; /* more data needed */
					}
				}
				else /* not SOCK_READ_ALL */
				{
					SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
						, "socket handle(%d) %d bytes received -without flag SOCK_READ_ALL-"
						, soc->dev_handle, bytes_read
						);

					data->data_length = (LSA_UINT16)soc->recv.bytes_transferred;

					break;
				}
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// send to tcp socket
//
// returns	SOCK_RSP_OK_ACTIVE:		put socket into select set
//			SOCK_RSP_ERR_REM_ABORT: disable socket
//			SOCK_RSP_ERR_RESOURCE:  disable socket
//			SOCK_RESP_ERR_LOC_NET:  disable socket
//          SOCK_RSP_OK:			send success, return rqb
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_tcp_send(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_tcp_send")

	LSA_INT native_error;
	LSA_INT bytes_to_write = data->data_length - soc->send.bytes_transferred;
	LSA_INT bytes_offset   = data->offset      + soc->send.bytes_transferred;
	LSA_INT bytes_written;

	LSA_RESPONSE_TYPE response = sock_low_send( soc->lower_handle
											  , (char *)data->buffer_ptr + bytes_offset
											  , bytes_to_write
											  , &bytes_written
											  , &native_error
											  );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response == SOCK_RSP_OK_ACTIVE ) /* would block -> put socket into select set */
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
				, "pending send on socket handle(%d), data_length(%d) to send, send returned SockErr_WouldBlock, bytes_transferred(%d) so far"
				, soc->dev_handle, data->data_length, soc->send.bytes_transferred
				);
		}
		else
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
				, "sock_low_send failed on dev_handle(%d), native_error(%d), response(%d)"
				, soc->dev_handle, native_error, response
				);

			data->data_length = (LSA_UINT16)soc->send.bytes_transferred; /* return actually written data to user */
		}
	}
	else /* SOCK_RSP_OK -> bytes_written >= 0 */
	{
		soc->send.bytes_transferred += (LSA_UINT16)bytes_written;

		if (soc->send.bytes_transferred == data->data_length)
		{
			SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d): send returned %d, return rqb"
			, soc->dev_handle, soc->send.bytes_transferred
			);

		}
		else
		{
			if( soc->send.bytes_transferred < data->data_length ) /* treats bytes_written == 0 also, see AP01233761 */
			{
				SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
					, "pending send on socket handle(%d), data_length(%d) to send, send returned %d, bytes_transferred(%d) so far"
					, soc->dev_handle, data->data_length, bytes_written, soc->send.bytes_transferred
					);

				response = SOCK_RSP_OK_ACTIVE; /* would block -> put socket into select set */
			}
			else  /* soc->send.bytes_transferred > data->data_length:  written more than requested */
			{
				sock_fatal(SOCK_MODULE_ID, SOCK__LINE__);
			}
		}
	}

	return response;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
