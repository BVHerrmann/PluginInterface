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
/*  F i l e               &F: sock_udp.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID 1
#define SOCK_MODULE_ID    1

#include "sock_int.h"
#include "sock_low.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*------------------------------------------------------------------------------
// sock_udp_open_socket: opens udp socket and binds to loc_addr
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_udp_open_socket(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_SOCKADDR_TYPE* loc_addr,
	SOCK_SOCKET_PTR* soc_ptr
) {
	SOCK_LOG_FCT("sock_udp_open_socket")

	LSA_RESPONSE_TYPE response;
	int native_error;

	SOCK_LOW_HANDLE_TYPE lower_handle = sock_low_socket(SOCK_LOW_DGRAM, &native_error);

	if (lower_handle == SOCK_LOW_INVALID_HANDLE)
	{
		SOCK_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_WARN
			, "unable to create UDP socket"
			);
		response = SOCK_RSP_ERR_RESOURCE;
	}
	else
	{
		SOCK_SOCKET_PTR soc = sock_socket_alloc(channel, lower_handle, SOCK_LOW_DGRAM);

		SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "lower_handle: 0x%x UDP SOCKET   OK", lower_handle);

		if ( sock_is_null(soc) )
		{
			SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "lower_handle: 0x%x UDP DEVICE   ERR", lower_handle);

			response = SOCK_RSP_ERR_RESOURCE;
		}
		else
		{
			response = sock_low_bind(soc->lower_handle, loc_addr, &native_error);

			if (response != SOCK_RSP_OK) /* handle bind error (AP00604854) */
			{
				SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
					, "bind() failed for UDP socket lower_handle(0x%x), native_error(%d), response(%d)"
					, soc->lower_handle, native_error, response
					);

				sock_socket_free(soc);
			}
			else
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "lower_handle: 0x%x UDP BIND OK", lower_handle);

				response = sock_low_get_sockname(soc->lower_handle, loc_addr, &native_error);

				if (response != SOCK_RSP_OK)
				{
					SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
						, "getsockname() failed for TCP socket lower_handle(0x%x), native_error(%d), response(%d)"
						, soc->lower_handle, native_error, response
						);
				}
				else
				{
					soc->loc_addr = *loc_addr;

					*soc_ptr = soc;

					SOCK_PROTOCOL_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE
						, "udp socket 0x%x created, port(%d), dev_handle(%d)"
						, soc->lower_handle, SOCK_HTONS(soc->loc_addr.sin_port), soc->dev_handle
						);
				}
			}
		}
	}
	return response;
}

/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_OPEN
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_udp_open(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_udp_open")

	SOCK_SOCKET_PTR soc;

	SOCK_RQB_ARGS_UDP_OPEN_PTR_TYPE open = &rqb->args.udp_open;

	LSA_RESPONSE_TYPE response = sock_udp_open_socket( channel
													   , &open->loc_addr
													   , &soc
													   );

	if( response == SOCK_RSP_OK )
	{
		soc->state = SOCK_CN_STATE_CONNECTED;

		open->sock_fd = soc->dev_handle;
	}

	return response;
}

/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_MULTICAST_OPEN
//----------------------------------------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_RESPONSE_TYPE
sock_udp_open_multicast(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_udp_open_multicast")
#if SOCK_CFG_INTERNICHE
	LSA_UINT32 IfIndex = 0;
#else
	LSA_UINT32 interface_addr = 0;
#endif
	SOCK_SOCKET_PTR soc = LSA_NULL;
	SOCK_RQB_ARGS_UDP_OPEN_MULTICAST_PTR_TYPE open_mc = & rqb->args.udp_open_multicast;
	LSA_RESPONSE_TYPE response = SOCK_RSP_OK;

	SOCK_UPPER_TRACE_06(0, LSA_TRACE_LEVEL_NOTE
		, "SOCK_OPC_UDP_MULTICAST_OPEN interface_id(%d) loc_addr.sin_addr.s_addr(%d) loc_addr.sin_port(%d) loc_addr.sin_family(%d) group_addr(0x%x) multicast_ttl(%d)"
		, open_mc->interface_id
		, open_mc->loc_addr.sin_addr.s_addr
		, open_mc->loc_addr.sin_port
		, open_mc->loc_addr.sin_family
		, SOCK_NTOHL(open_mc->group_addr.s_addr)
		, open_mc->multicast_ttl
		);

	if( open_mc->interface_id < 1
		|| open_mc->interface_id > SOCK_MAX_INTERFACE_ID
		|| open_mc->loc_addr.sin_port == 0
		|| open_mc->loc_addr.sin_family != SOCK_AF_INET
		|| open_mc->group_addr.s_addr == 0
		|| open_mc->multicast_ttl == 0
		)
	{
	  SOCK_UPPER_TRACE_05(0, LSA_TRACE_LEVEL_ERROR
	    , "condition(s) violated: 1<=interface_id(%d)<=16, loc_addr.sin_port(%d)!=0, loc_addr.sin_family(%d) != SOCK_AF_INET, group_addr(0x%x)!=0, multicast_ttl(%d)!=0"
	    , open_mc->interface_id
	    , open_mc->loc_addr.sin_port
		, open_mc->loc_addr.sin_family
	    , SOCK_NTOHL(open_mc->group_addr.s_addr)
	    , open_mc->multicast_ttl
	    );

		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
		LSA_BOOL retval;
#if SOCK_CFG_INTERNICHE
		retval = (TCIP_OK == tcip_get_mib2_ifindex(open_mc->interface_id, 0, &IfIndex) ? LSA_TRUE : LSA_FALSE);
#else
		interface_addr = sock_data.addr_info[open_mc->interface_id-1].CurrentIPSuite.IpAddr.s_addr;

		retval = interface_addr != 0 ? LSA_TRUE : LSA_FALSE; /* no IP Suite setting --> interface not valid */
#endif
		if(!retval)
		{
			response = SOCK_RSP_ERR_PARAM;

			SOCK_LOWER_TRACE_01 (0, LSA_TRACE_LEVEL_ERROR
				, "no valid network interface found, response(%d)"
				, response
				);
		}
		else
		{

#if SOCK_CFG_BIND_MULTICAST_SOCKETS_TO_ANY
			open_mc->loc_addr.sin_addr.s_addr = 0; /* ignore input, always bind to ANY */
#endif
			response = sock_udp_open_socket(channel, &open_mc->loc_addr, &soc);

			if (response != SOCK_RSP_OK)
			{
				SOCK_LOWER_TRACE_01 (0, LSA_TRACE_LEVEL_ERROR
					, "sock_udp_open_socket not successful, response(%d)"
					, response
					);
			}
			else
			{
				int native_error;

#if SOCK_CFG_INTERNICHE
				response = sock_low_set_multicast_sockopts(soc->lower_handle, IfIndex, open_mc->group_addr.s_addr, open_mc->multicast_ttl, &native_error);
#else
				response = sock_low_set_multicast_sockopts(soc->lower_handle, interface_addr, open_mc->group_addr.s_addr, open_mc->multicast_ttl, &native_error);
#endif


				if (response != SOCK_RSP_OK)
				{
					sock_socket_free(soc);

					SOCK_LOWER_TRACE_05(0, LSA_TRACE_LEVEL_ERROR
						, "native errno(%d) when setting multicast socket options, dev_handle(%d), interface_id(%d), group_addr(0x%x), response(%d)"
						, native_error
						, soc->dev_handle
						, open_mc->interface_id
						, SOCK_NTOHL(open_mc->group_addr.s_addr)
						, response
						);
				}
				else
				{
					LSA_RESPONSE_TYPE ret = tcip_srv_multicast(soc->dev_handle
																, open_mc->interface_id
																, open_mc->group_addr.s_addr
																, TCIP_SRV_MULTICAST_ENABLE
																);

					if( ret != TCIP_OK )
					{
						SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "tcip_srv_multicast dev_handle(%d), interface_id(%d), group_addr(0x%x) failed"
							, soc->dev_handle
							, open_mc->interface_id
							, SOCK_NTOHL(open_mc->group_addr.s_addr)
							);

						sock_socket_free(soc);

						response = SOCK_RSP_ERR_PARAM;
					}
					else
					{
						soc->open_close_rb = rqb; /* store rqb, see TCIP_EDD_SRV_MULTICAST_DONE */

						response = SOCK_RSP_OK_ACTIVE;
					}
				}
			}
		}
	}

	return response;
}
#endif


/*------------------------------------------------------------------------------
// returns: LSA_TRUE, if passed address is a Cluster IP address
// 			LSA_FALSE, if passed address is not a Cluster IP address
//----------------------------------------------------------------------------*/
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
static LSA_BOOL sock_udp_is_cluster_ip(LSA_UINT32 address)
{
	int i;

	// find out if destination address equals cluster IP
	for (i=0; i<SOCK_MAX_INTERFACE_ID; i++)
	{
		if (sock_data.cluster_ip.cluster_info[i].Info.ClusterID != 0)
		{
			if (sock_data.cluster_ip.cluster_info[i].Info.ClusterIPAddress.s_addr == address)
			{
				break;
			}
		}
	}

	if (i < SOCK_MAX_INTERFACE_ID) /* Cluster IP found */
	{
		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}
#endif


/*------------------------------------------------------------------------------
// returns: - local address of interface which is in the same net as remote host
// 			- zero if remote host is not in one of the local nets
//----------------------------------------------------------------------------*/
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
static LSA_UINT32 sock_udp_find_local_address(LSA_UINT32 remote_host)
{
	int i;
	LSA_UINT32 local_address = 0;

	for (i=0; i<SOCK_MAX_INTERFACE_ID; i++)
	{
		SOCK_IP_SUITE_TYPE* pIp = &sock_data.addr_info[i].CurrentIPSuite;

		if (pIp->NetMask.s_addr != 0
			&& pIp->IpAddr.s_addr != 0
			&& ((pIp->NetMask.s_addr &  pIp->IpAddr.s_addr) == (remote_host & pIp->NetMask.s_addr))
			)
		{
			local_address = pIp->IpAddr.s_addr;

			break;
		}
	}

	/***/

	return local_address; /* pass 0 to IP stack if no local address found */
}
#endif


/*------------------------------------------------------------------------------
// local function: reads data from udp socket
// returns:  SOCK_RSP_OK	   : data was successfully read from socket
//           SOCK_RSP_OK_ACTIVE: read was blocked
// caution: function returns 0 in *bytes_read if datagram was sent to Cluster IP
//          and local_address is supported by configured IP stack
//----------------------------------------------------------------------------*/
#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD
LSA_RESPONSE_TYPE
sock_udp_read(
	LSA_INT32* bytes_read,
	SOCK_SOCKET_CONST_PTR soc,
	LSA_VOID* buffer,
	LSA_INT length,
	LSA_BOOL peek,
	SOCK_SOCKADDR_TYPE* remote_address
) {
	SOCK_LOG_FCT("sock_udp_read")

	LSA_INT native_error;
	LSA_INT bytes_received;
	LSA_RESPONSE_TYPE  response;
	SOCK_SOCKADDR_TYPE local_address; /* for SNMP in non-BSD configurations : filter Cluster IP */

	local_address.sin_family = 0; /* lower must set sin_family if SOCK_CFG_CLUSTER_IP_SUPPORTED == 1 */

	response = sock_low_recvfrom( soc->lower_handle
								  , (char*)buffer
								  , length
								  , peek ? SOCK_LOW_PEEK: 0
								  , remote_address
								  , &local_address
								  , &bytes_received
								  , &native_error
								  );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response != SOCK_RSP_OK_ACTIVE )
		{
			int sock_option;
			int option_error;

			sock_low_get_socket_error(soc->lower_handle, &sock_option, &option_error); /* reset of so_error -> socket will be available again */

			SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
				, "sock_low_get_socket_error on udp lower_handle(0x%x) because recvfrom() returned native_error(%d), response(%d)"
				, soc->lower_handle, native_error, response
				);

			response = SOCK_RSP_OK_ACTIVE; /* would block -> wait for next select cycle */
		}
	}
	else /* SOCK_RSP_OK */
	{
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
		SOCK_ASSERT(local_address.sin_family != 0); /* (!) lower must support loc_addr */

		if(sock_udp_is_cluster_ip(local_address.sin_addr.s_addr)) /* filter datagrams sent to the Cluster IP address */
		{
			*bytes_read = 0; /* see sock_socket_handle_read_mask() */
		}
		else
		{
			*bytes_read = bytes_received; /* return number of bytes read */
		}
#else
		*bytes_read = bytes_received; /* return number of bytes read */
#endif
	}

	return response;
}
#endif


/*------------------------------------------------------------------------------
// receive from udp socket
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_udp_receive(
	SOCK_SOCKET_CONST_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_udp_receive")

	LSA_RESPONSE_TYPE response;

	for(;;)
	{
		LSA_INT native_error;
		LSA_INT bytes_read;

		response = sock_low_recvfrom( soc->lower_handle
									, (char*)data->buffer_ptr + data->offset
									, data->data_length
									, (data->flags & SOCK_PEEK) ? 1 : 0
									, &data->rem_addr
									, &data->loc_addr
									, &bytes_read
									, &native_error
									);

		if( response != SOCK_RSP_OK ) /* failed */
		{
			if( response == SOCK_RSP_OK_ACTIVE ) /* next attempt in next read */
			{
				break;
			}
			else  /* remove error condition by getsockopt */
			{
				int sock_option;
				int option_error;

				sock_low_get_socket_error(soc->lower_handle, &sock_option, &option_error); /* reset of so_error -> socket will be available again */

				SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
					, "udp socket handle(%d): recvfrom() returned native_error(%d), response(%d)"
					, soc->lower_handle, native_error, response
					);

				continue;  /* try again immediately */
			}
		}
		else  /* SOCK_RSP_OK */
		{
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
			SOCK_ASSERT(data->loc_addr.sin_family != 0); /* (!) lower must support loc_addr */

			if (sock_data.cluster_ip.cluster_info_count /* cluster is configured */
				&& data->flags & SOCK_STANDARD_IP_ONLY  /* ignore packets with dest_addr equal to Cluster IP */
				)
			{
				if(sock_udp_is_cluster_ip(data->loc_addr.sin_addr.s_addr))
				{
					continue; /* filter datagrams sent to the Cluster IP address */
				}
			}
#else
			data->loc_addr.sin_family = 0; /* indicate that Cluster IP is not supported */
#endif
			data->data_length = (LSA_UINT16)bytes_read;   /* length of this telegram */

			break;
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// send to udp socket
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_udp_send(
	SOCK_SOCKET_CONST_PTR soc,
	SOCK_RQB_ARGS_DATA_PTR_TYPE data
){
	SOCK_LOG_FCT("sock_udp_send")

	SOCK_SOCKADDR_TYPE *rem_addr_p;
	SOCK_SOCKADDR_TYPE *loc_addr_p;
	LSA_INT bytes_written;
	LSA_RESPONSE_TYPE response;
	LSA_INT native_error;

	if( soc->rem_addr.sin_port != 0 /* was "connected", see SOCK_OPC_UDP_CONNECT */
		&& data->rem_addr.sin_port == 0 ) /* use "connected" destination address */
	{
		rem_addr_p = 0;
	}
	else
	{
		rem_addr_p = &data->rem_addr;
	}

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
	if (data->flags & SOCK_STANDARD_IP_ONLY) /* determine source address for UDP packet to send; ignore user data */
	{
		LSA_UINT32 locaddr = sock_udp_find_local_address(rem_addr_p ? rem_addr_p->sin_addr.s_addr : soc->rem_addr.sin_addr.s_addr);

		if (locaddr)
		{
			/* if source  parameter */
			data->loc_addr.sin_addr.s_addr = locaddr;
			data->loc_addr.sin_family = SOCK_AF_INET; /* only this possible */
			loc_addr_p = &data->loc_addr;
		}
		else
		{
			loc_addr_p = 0; /* fall back: IP stack determines source address of packet to send */
		}
	}
	else /* take source address from user data */
	{
		if (data->loc_addr.sin_addr.s_addr)
		{
			loc_addr_p = &data->loc_addr;
		}
		else
		{
			loc_addr_p = 0; /* fall back: IP stack determines source address of packet to send */
		}
	}
#else
	loc_addr_p = 0;
#endif

	response = sock_low_sendto( soc->lower_handle
							  , (char *)data->buffer_ptr + data->offset
							  , data->data_length
							  , rem_addr_p
							  , loc_addr_p
							  , &bytes_written
							  , &native_error
							  );

	if( response != SOCK_RSP_OK ) /* failed */
	{
		if( response == SOCK_RSP_OK_ACTIVE ) /* put socket into select set */
		{
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "udp socket handle(%d): pending send"
				, soc->dev_handle
				);
		}
		else
		{
			int option_val;
			int option_error;

			sock_low_get_socket_error(soc->lower_handle, &option_val, &option_error); /* remove error condition but inform user */

			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
				, "udp socket handle(%d): sendto() returned native_error(%d), response(%d)"
				, soc->dev_handle, native_error, response
				);

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
			if (response == SOCK_RSP_ERR_LOC_NET)
			{
				data->reason = sock_low_get_reason_code(native_error); /* see 2390556, 2390548 */
			}
#endif
		}
	}
	else /* bytes_written >= 0 */
	{
		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d): send ok, bytes_written(%d)"
			, soc->lower_handle, bytes_written
			);

		if ( bytes_written <= data->data_length )
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
				, "socket handle(%d): bytes(%d) of requested(%d) sent, return rqb"
				, soc->dev_handle, bytes_written, data->data_length
				);

			data->data_length = (LSA_UINT16)bytes_written; /* return amount  */
		}
		else /* bytes_written > data->data_length: written more than requested */
		{
			sock_fatal(SOCK_MODULE_ID, SOCK__LINE__);
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_CONNECT
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_udp_connect(
	SOCK_SOCKET_PTR soc,
	SOCK_RQB_ARGS_UDP_CONN_CONST_PTR_TYPE udp_connect
) {
	SOCK_LOG_FCT("sock_udp_connect")

	LSA_RESPONSE_TYPE response;

	if( soc->rem_addr.sin_port != 0 ) /* connected socket has valid remote port */
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "socket handle(%d) is already connected"
			, udp_connect->sock_fd
			);

		response = SOCK_RSP_ERR_SEQUENCE;
	}
	else if( sock_is_not_null(soc->send.curr)
			 || sock_is_not_null(soc->recv.curr) )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "send or receive requests pending for socket handle(%d)"
			, udp_connect->sock_fd
			);

		response = SOCK_RSP_ERR_SEQUENCE;
	}
	else if(udp_connect->rem_addr.sin_family != SOCK_AF_INET
	     || udp_connect->rem_addr.sin_addr.s_addr == 0
			 || udp_connect->rem_addr.sin_port == 0 )
	{
		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "invalid parameter(s): address(0x%x):port(%d)"
			, SOCK_NTOHL(udp_connect->rem_addr.sin_addr.s_addr)
			, SOCK_NTOHS(udp_connect->rem_addr.sin_port)
			);

		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
		LSA_INT native_error;
		SOCK_SOCKADDR_TYPE rem_addr;

		rem_addr.sin_family = udp_connect->rem_addr.sin_family;
		rem_addr.sin_port = udp_connect->rem_addr.sin_port;
		rem_addr.sin_addr.s_addr = udp_connect->rem_addr.sin_addr.s_addr;

		response = sock_low_connect(soc->lower_handle, &rem_addr, &native_error);

		if (response != SOCK_RSP_OK)
		{
			int error_val;
			int option_error;

			sock_low_get_socket_error(soc->lower_handle, &error_val, &option_error); /* eliminate error condtion */

			SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_ERROR
				, "connect of udp socket handle(%d) to address(0x%x):port(%d) failed with native error(%d)"
				, soc->lower_handle
				, SOCK_HTONL(rem_addr.sin_addr.s_addr)
				, SOCK_HTONS(rem_addr.sin_port)
				, error_val
				);

			if( response != SOCK_RSP_ERR_RESOURCE ) /* do not use error mapping */
			{
				response = SOCK_RSP_ERR_LOC_NET; /* for UDP connect, only local reason is possible */
			}
		}
		else
		{
			soc->rem_addr.sin_family = udp_connect->rem_addr.sin_family;
			soc->rem_addr.sin_port = udp_connect->rem_addr.sin_port; /* indicates connected state of udp socket, see above */
			soc->rem_addr.sin_addr.s_addr = udp_connect->rem_addr.sin_addr.s_addr;

			response = SOCK_RSP_OK;
		}
	}

	return response;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
