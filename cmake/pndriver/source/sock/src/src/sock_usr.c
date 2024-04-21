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
/*  F i l e               &F: sock_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Functions of the upper interface of SOCK.                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define SOCK_MODULE_ID     3

#include "sock_int.h"

#if SOCK_CFG_OPEN_BSD /* OBSD_ITGR -- undefine close and listen; rename_syscalls --> obsd_close, obsd_listen */
#undef close
#undef listen
#endif

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*===========================================================================*/

static LSA_VOID sock_usr_close_channel(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb );

static LSA_UINT16 sock_usr_cntrl_timeparams(SOCK_UPPER_RQB_PTR_TYPE rqb);
static LSA_VOID sock_usr_deactivate_tcp_sockets(LSA_VOID);
#if SOCK_CFG_OPEN_BSD
static LSA_VOID sock_usr_abort_tcp_connections(LSA_UINT32 old_address);
#endif
static LSA_UINT16 sock_usr_connect_with_open(SOCK_CHANNEL_CONST_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
static LSA_RESPONSE_TYPE sock_usr_send(SOCK_UPPER_RQB_PTR_TYPE rqb, SOCK_SOCKET_PTR soc);
static LSA_RESPONSE_TYPE sock_usr_receive(SOCK_UPPER_RQB_PTR_TYPE rqb, SOCK_SOCKET_PTR soc);
static LSA_RESPONSE_TYPE sock_usr_transmit(SOCK_CHANNEL_CONST_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb, LSA_OPCODE_TYPE opcode);


/*------------------------------------------------------------------------------
// open communication channel
//----------------------------------------------------------------------------*/
LSA_VOID
sock_open_channel(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_open_channel")

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,
		"requestblock rqb_ptr=0x%x , handle_upper=%d, sys_path=0x%x",
		rqb, rqb->args.channel.handle_upper, rqb->args.channel.sys_path);

	SOCK_ENTER();

	sock_usr_open_channel(rqb);

	SOCK_EXIT();
}


/*------------------------------------------------------------------------------
// open LSA communication channel
//----------------------------------------------------------------------------*/
LSA_VOID
sock_usr_open_channel(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_open_channel")

	LSA_UINT16              rc;
	SOCK_CHANNEL_PTR_TYPE   channel = LSA_NULL;
	LSA_SYS_PATH_TYPE       sys_path;
	LSA_SYS_PTR_TYPE        pSysptr;
	SOCK_DETAIL_PTR_TYPE    pDetail;
	SOCK_RQB_ARGS_CHANNEL_PTR_TYPE open;

	if( sock_is_null(rqb) || SOCK_RQB_GET_OPCODE(rqb) != SOCK_OPC_OPEN_CHANNEL )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "no rqb(%08X) or wrong opcode or no args", rqb);
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, 0, 0, 0, 0, 0, 0); /*lint -unreachable */
	}

#if (SOCK_DEBUG == 1)
	SOCK_RQB_SET_NEXT_RQB_PTR(rqb, LSA_NULL);   /* sanity */
	SOCK_RQB_SET_PREV_RQB_PTR(rqb, LSA_NULL);   /* sanity */
#endif

	open = &rqb->args.channel;

	open->handle = SOCK_INVALID_HANDLE; /* sanity */

	if (open->sock_request_upper_done_ptr == 0) /* AP01382171: dont' use macro sock_is_null() */
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "no callback function given" );
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, 0, 0, 0, 0, 0, 0);
	}

	sys_path = open->sys_path;

	pDetail = LSA_NULL;

	SOCK_GET_PATH_INFO(&rc, &pSysptr, &pDetail, sys_path);

	if (rc != LSA_RET_OK || sock_is_null(pDetail) )
	{
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, 0, 0, 0, 0, 0, 0);
	}

	{
	LSA_HANDLE_TYPE my_handle;

	if ( SOCK_CFG_MAX_CHANNELS >= SOCK_INVALID_HANDLE)
	{
		LSA_UINT16 rCode;

		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, " SOCK_CFG_MAX_CHANNELS(%u) >= SOCK_INVALID_HANDLE(%u)", SOCK_CFG_MAX_CHANNELS, SOCK_INVALID_HANDLE );

		SOCK_RQB_SET_RESPONSE (rqb, SOCK_RSP_ERR_PARAM);
		SOCK_RQB_SET_HANDLE (rqb, open->handle_upper);
		SOCK_REQUEST_UPPER_DONE (open->sock_request_upper_done_ptr, rqb, pSysptr);
		SOCK_RELEASE_PATH_INFO (&rCode, pSysptr, pDetail);
		SOCK_ASSERT (rCode == LSA_RET_OK);
		return;
	}

	for (my_handle = 0; my_handle < SOCK_CFG_MAX_CHANNELS; ++my_handle)
	{
		channel = &sock_data.sock_channels[my_handle];
		if (channel->state == SOCK_CHANNEL_CLOSED )
		{
			#if SOCK_DEBUG
				SOCK_MEMSET(channel, 0xCC, sizeof(*channel)); /* to find uninitialized things */
				/*channel->state = TCIP_CH_FREE;*/
			#endif
			break; /* found */
		}
	}

	if ( my_handle >= SOCK_CFG_MAX_CHANNELS || sock_is_null(channel) /* sanity */ )
	{
		LSA_UINT16 rCode;

		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "no free channel found, SOCK_CFG_MAX_CHANNELS(%u)", SOCK_CFG_MAX_CHANNELS );

		SOCK_RQB_SET_RESPONSE (rqb, SOCK_RSP_ERR_RESOURCE);
		SOCK_RQB_SET_HANDLE (rqb, open->handle_upper);
		SOCK_REQUEST_UPPER_DONE (open->sock_request_upper_done_ptr, rqb, pSysptr);
		SOCK_RELEASE_PATH_INFO (&rCode, pSysptr, pDetail);
		SOCK_ASSERT (rCode == LSA_RET_OK);
		return;
	}



	if( sock_is_null(channel) )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "no free channel found, SOCK_CFG_MAX_CHANNELS(%u)", SOCK_CFG_MAX_CHANNELS );

		SOCK_RQB_SET_RESPONSE(rqb, SOCK_RSP_ERR_RESOURCE);
		SOCK_RQB_SET_HANDLE(rqb, open->handle_upper);
		SOCK_REQUEST_UPPER_DONE(open->sock_request_upper_done_ptr, rqb, pSysptr);
		SOCK_RELEASE_PATH_INFO( &rc, pSysptr, pDetail );

		SOCK_ASSERT(rc == LSA_RET_OK);

		return;
	}

	if( sock_is_not_null(sock_data.close_channel_rqb) )
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "sock_data.close_channel_rqb not LSA_NULL");

		SOCK_RQB_SET_RESPONSE(rqb, SOCK_RSP_ERR_SEQUENCE);
		SOCK_RQB_SET_HANDLE(rqb, open->handle_upper);
		SOCK_REQUEST_UPPER_DONE(open->sock_request_upper_done_ptr, rqb, pSysptr);
		SOCK_RELEASE_PATH_INFO( &rc, pSysptr, pDetail );

		SOCK_ASSERT(rc == LSA_RET_OK);

		return;
	}

	channel->own_handle   = my_handle;

	channel->upper_handle = open->handle_upper;
	channel->req_done_ptr = open->sock_request_upper_done_ptr;

	channel->sys_ptr      = pSysptr;
	channel->detail_ptr   = pDetail;
	channel->state        = SOCK_CHANNEL_OPENED;

	channel->rqb_ptr      = LSA_NULL;   /*rqb; ?!?! */
	channel->rqb_addr_ind = LSA_NULL;
	channel->rqb_mib2_ind = LSA_NULL;

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
	{
		int i;
		for (i=0; i<SOCK_MAX_INTERFACE_ID; i++)
		{
			channel->cluster_ip_ind_rqbs[i] = LSA_NULL;
		}
	}
#endif

	SOCK_RQB_SET_HANDLE(rqb, open->handle_upper);

	open->handle = my_handle;
	}

	sock_data.sock_channels_opened += 1;

#if !SOCK_INT_CFG_SOCKET_EVENTS
	if( sock_data.sock_channels_opened == 1 ) /* 1st channel opens -> start disconnect timer */
	{
		LSA_USER_ID_TYPE user_id;
		LSA_UINT16 ret;
		user_id.uvar32 = 0;

		SOCK_START_TIMER(&ret, sock_data.tcp_disconnect_check_timer_id, user_id, 1);

		if (ret != LSA_RET_OK)
		{
			SOCK_FATAL();
		}
	}
#endif

#if TCIP_INT_CFG_TRACE_ON
	/* (!) trace detail ponter settings */
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: SOCK channel details for channel:%d", channel->own_handle);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:      socket_option.SO_BROADCAST_:%d", (channel->detail_ptr->socket_option.SO_BROADCAST_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:      socket_option.SO_DONTROUTE_:%d", (channel->detail_ptr->socket_option.SO_DONTROUTE_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:         socket_option.SO_LINGER_:%d", (channel->detail_ptr->socket_option.SO_LINGER_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:         socket_option.SO_RCVBUF_:%d", (channel->detail_ptr->socket_option.SO_RCVBUF_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:         socket_option.SO_SNDBUF_:%d", (channel->detail_ptr->socket_option.SO_SNDBUF_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:      socket_option.SO_REUSEADDR_:%d", (channel->detail_ptr->socket_option.SO_REUSEADDR_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:  socket_option.SO_TCPNOACKDELAY_:%d", (channel->detail_ptr->socket_option.SO_TCPNOACKDELAY_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:     socket_option.SO_TCPNODELAY_:%d", (channel->detail_ptr->socket_option.SO_TCPNODELAY_) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:               sock_close_at_once:%d", (channel->detail_ptr->sock_close_at_once) ? 1 : 0);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:           rec_buffer_max_len:%5d", channel->detail_ptr->rec_buffer_max_len);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:          send_buffer_max_len:%5d", channel->detail_ptr->send_buffer_max_len);
	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:                  linger_time:%5d", channel->detail_ptr->linger_time);
#ifdef PNTEST_BUILD_WINDOWS
	printf("instrumentation: SOCK channel details for channel:%d\n", channel->own_handle);
	printf("instrumentation:      socket_option.SO_BROADCAST_:%d\n", (channel->detail_ptr->socket_option.SO_BROADCAST_) ? 1 : 0);
	printf("instrumentation:      socket_option.SO_DONTROUTE_:%d\n", (channel->detail_ptr->socket_option.SO_DONTROUTE_) ? 1 : 0);
	printf("instrumentation:         socket_option.SO_LINGER_:%d\n", (channel->detail_ptr->socket_option.SO_LINGER_) ? 1 : 0);
	printf("instrumentation:         socket_option.SO_RCVBUF_:%d\n", (channel->detail_ptr->socket_option.SO_RCVBUF_) ? 1 : 0);
	printf("instrumentation:         socket_option.SO_SNDBUF_:%d\n", (channel->detail_ptr->socket_option.SO_SNDBUF_) ? 1 : 0);
	printf("instrumentation:      socket_option.SO_REUSEADDR_:%d\n", (channel->detail_ptr->socket_option.SO_REUSEADDR_) ? 1 : 0);
	printf("instrumentation:  socket_option.SO_TCPNOACKDELAY_:%d\n", (channel->detail_ptr->socket_option.SO_TCPNOACKDELAY_) ? 1 : 0);
	printf("instrumentation:     socket_option.SO_TCPNODELAY_:%d\n", (channel->detail_ptr->socket_option.SO_TCPNODELAY_) ? 1 : 0);
	printf("instrumentation:               sock_close_at_once:%d\n", (channel->detail_ptr->sock_close_at_once) ? 1 : 0);
	printf("instrumentation:           rec_buffer_max_len:%5d\n", channel->detail_ptr->rec_buffer_max_len);
	printf("instrumentation:          send_buffer_max_len:%5d\n", channel->detail_ptr->send_buffer_max_len);
	printf("instrumentation:                  linger_time:%5d\n", channel->detail_ptr->linger_time);
#endif
#endif

	SOCK_RQB_SET_RESPONSE (rqb, SOCK_RSP_OK );

	SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "rqb_ptr=0x%x confirmed with response=0x%x", rqb, SOCK_RQB_GET_RESPONSE(rqb));

	SOCK_REQUEST_UPPER_DONE (rqb->args.channel.sock_request_upper_done_ptr, rqb, pSysptr);
}


/*------------------------------------------------------------------------------
// close communication channel
//----------------------------------------------------------------------------*/
LSA_VOID
sock_close_channel(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_close_channel")

	SOCK_CHANNEL_PTR_TYPE channel;

	SOCK_ASSERT( sock_is_not_null(rqb));

	SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
		"rqb_ptr=0x%x, handle_upper=%d",
		rqb, rqb->args.channel.handle_upper
		);

	SOCK_ENTER();

	/* sanity checks */

	channel = sock_is_null(rqb) ? LSA_NULL : sock_channel_from_handle(SOCK_RQB_GET_HANDLE(rqb));

	if ( sock_is_null(channel) )
	{
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, sizeof(SOCK_RQB_TYPE), rqb,SOCK_RQB_GET_HANDLE(rqb),0,0, 0); /*lint -unreachable */

		SOCK_EXIT();

		return;
	}

	sock_data.sock_channels_opened -= 1;

#if !SOCK_INT_CFG_SOCKET_EVENTS
	if( sock_data.sock_channels_opened == 0 ) /* last user channel close -> stop disconnect timer */
	{
		LSA_UINT16      ret_val;

		SOCK_STOP_TIMER(&ret_val, sock_data.tcp_disconnect_check_timer_id);

		if( ret_val != LSA_RET_OK )
		{
			if( ret_val == LSA_RET_OK_TIMER_NOT_RUNNING ) /* sock_timeout() finished or will be called */
			{
				channel->state = SOCK_CHANNEL_PENDING_CLOSE; /* distinct state */

				sock_data.close_channel_rqb = rqb; /* close channel after timer callback invocation, see sock_timeout() */

				SOCK_EXIT();

				return;
			}
			else /* LSA_RET_ERR_PARAM */
			{
				SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
					, "SOCK_STOP_TIMER returned ret_val(%d)"
					, ret_val
					);

				SOCK_FATAL();
			}
		}

		/* if ret_val == LSA_RET_OK -> close channel immediately */

	}
#endif

	sock_usr_close_channel(channel, rqb);

	SOCK_EXIT();

	return;
}


/*------------------------------------------------------------------------------
// must be called in SOCK_ENTER() context
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_usr_close_channel(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_close_channel")

	LSA_INT i;

	SOCK_ASSERT( sock_is_not_null(channel));

	for (i=0; i<SOCK_CFG_MAX_SOCKETS; i++ )
	{
		if( SOCK_CN_STATE_FREE != sock_data.sockets[i].state
			&& channel == sock_data.sockets[i].channel
		   )
		{
			break; /* found */
		}
	}

	if( i<SOCK_CFG_MAX_SOCKETS )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
			, "close channel handle(%d) failed: open socket associated with channel to close found, return SOCK_RSP_ERR_SEQUENCE"
			, channel->own_handle
			);

		sock_usr_callback(channel, SOCK_RSP_ERR_SEQUENCE, &rqb);
	}
	else
	{
		/* return indication resources */

		if( sock_is_not_null(channel->rqb_addr_ind) ) /* SOCK_OPC_ADDR_INFO_IND */
		{
			sock_usr_callback(channel, SOCK_RSP_OK_WITHDRAW, &channel->rqb_addr_ind);
		}

		if( sock_is_not_null(channel->rqb_mib2_ind) ) /* SOCK_OPC_MIB2_SYSGROUP_DATA_IND */
		{
			sock_usr_callback(channel, SOCK_RSP_OK_WITHDRAW, &channel->rqb_mib2_ind);
		}

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
		{
		int j;
		for (j=0; j<SOCK_MAX_INTERFACE_ID; j++)
		{
			if (sock_is_not_null(channel->cluster_ip_ind_rqbs[j]))
			{
				sock_usr_callback(channel, SOCK_RSP_OK_WITHDRAW, &channel->cluster_ip_ind_rqbs[j]);
			}
		}
		}
#endif

		SOCK_RQB_SET_HANDLE(rqb, channel->upper_handle);
		SOCK_RQB_SET_RESPONSE(rqb, SOCK_RSP_OK);

		SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
			"rqb_ptr=0x%x confirmed with response=0x%x"
			, rqb, SOCK_RQB_GET_RESPONSE(rqb)
			);

		{
		LSA_UINT16 ret_val;
		LSA_SYS_PTR_TYPE     pSys    = channel->sys_ptr;
		SOCK_DETAIL_PTR_TYPE pDetail = channel->detail_ptr;
		SOCK_UPPER_CALLBACK_FCT_PTR_TYPE req_done_fct = channel->req_done_ptr;

		channel->state = SOCK_CHANNEL_CLOSED;
		sock_channel_free(channel);

		SOCK_REQUEST_UPPER_DONE( req_done_fct, rqb, pSys );

		SOCK_RELEASE_PATH_INFO(&ret_val, pSys, pDetail );
		SOCK_ASSERT( ret_val == LSA_RET_OK );
		}
	}
}


/*------------------------------------------------------------------------------
// sock_request handles all sock opcodes except channel open/close
//----------------------------------------------------------------------------*/
LSA_VOID
sock_request(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_request")

	LSA_UINT16      response = SOCK_RSP_OK;
	SOCK_CHANNEL_PTR_TYPE    channel;

	SOCK_ENTER();

	SOCK_RQB_SET_NEXT_RQB_PTR(rqb, LSA_NULL); /* sanity */
	SOCK_RQB_SET_PREV_RQB_PTR(rqb, LSA_NULL); /* sanity */

	/* select trigger request posted with SOCK_REQUEST_LOCAL */

	if ( SOCK_RQB_GET_OPCODE(rqb) == SOCK_OPC_SELECT_TRIGGER )
	{
		SOCK_INTERLOCKED_EXCHANGE(&rqb->args.select_trigger.inUse, 0);

#if SOCK_INT_CFG_SOCKET_EVENTS
		TCIP_DO_SOCKET_EVENTS();
#else
		if( sock_data.sock_channels_opened > 0 ) /* at least one open channel */
		{
			sock_socket_do_select(rqb);
		}
		else /* pending close channel ? */
		{
			if( sock_is_not_null(sock_data.close_channel_rqb) )
			{
				SOCK_UPPER_RQB_PTR_TYPE loc_close_channel_rqb = sock_data.close_channel_rqb;

				sock_data.close_channel_rqb = LSA_NULL; /* sanity */

				channel = sock_channel_from_handle(SOCK_RQB_GET_HANDLE(loc_close_channel_rqb));

				sock_usr_close_channel(
					channel /* handle already checked, see sock_close_channel() */
					, loc_close_channel_rqb
					);
			}
		}
#endif
		SOCK_EXIT();

		return;
	}


	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,  "rqb_ptr=0x%x, handle=%d, opcode=%d", rqb, SOCK_RQB_GET_HANDLE(rqb), SOCK_RQB_GET_OPCODE(rqb) );

	channel = sock_channel_from_handle(SOCK_RQB_GET_HANDLE(rqb));

	if ( sock_is_null(channel) )
	{
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, sizeof(SOCK_RQB_TYPE), rqb, SOCK_RQB_GET_HANDLE(rqb), 0, 0, 0); /*lint -unreachable */
		SOCK_EXIT();
		return;
	}

	if ( channel->state != SOCK_CHANNEL_OPENED)
	{
		socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, sizeof(SOCK_RQB_TYPE), rqb, SOCK_RQB_GET_HANDLE(rqb), 0, 0, 0); /*lint -unreachable */
		SOCK_EXIT();
		return;
	}

	SOCK_RQB_SET_RESPONSE(rqb, SOCK_RSP_OK_ACTIVE);

	switch ( SOCK_RQB_GET_OPCODE(rqb) )
	{
		case SOCK_OPC_SET_MIB2_IFGROUP_DATA:
			response = sock_mib2_set_if_params(rqb);
			break;

		case SOCK_OPC_SET_MIB2_SYSGROUP_DATA:
			response = sock_mib2_set_system_params(rqb);
			break;

	   case SOCK_OPC_MIB2_SYSGROUP_DATA_IND:
			response = sock_mib2_set_sysgroup_data_ind(channel, rqb);
			break;

		case SOCK_OPC_GET_MIB2_IFINDEX:
			response = sock_mib2_get_ifindex(rqb);
			break;

		case SOCK_OPC_LISTEN:
			response = sock_tcp_listen(channel, rqb);
			break;

		case SOCK_OPC_ACCEPT_NOTIFY:
			response = sock_usr_accept_notify(channel, rqb);
			break;

		case SOCK_OPC_EXCEPTION_NOTIFY:
			response = sock_usr_except_notify(channel, rqb);
			break;

		case SOCK_OPC_OPEN:
			response = sock_tcp_open(channel, rqb);
			break;

		case SOCK_OPC_CONNECT_WITH_OPEN:
			response = sock_usr_connect_with_open(channel, rqb);
			break;

		case SOCK_OPC_CONNECT:
			response = sock_tcp_connect(channel, rqb);
			break;

		case SOCK_OPC_SEND:
			response = sock_usr_transmit(channel, rqb, SOCK_OPC_SEND);
			break;

		case SOCK_OPC_RECEIVE:
			response = sock_usr_transmit(channel, rqb, SOCK_OPC_RECEIVE);
			break;

		case SOCK_OPC_CLOSE:
			response = sock_usr_close(channel, rqb);
			break;

		case SOCK_OPC_CANCEL_CONNECTS:
			response = sock_tcp_connect_cancel(channel);
			break;
#if SOCK_CFG_UDP_ON
	case SOCK_OPC_UDP_OPEN:
			response = sock_udp_open(channel, rqb);
			break;
		case SOCK_OPC_UDP_SEND:
			response = sock_usr_transmit(channel, rqb, SOCK_OPC_UDP_SEND);
			break;

		case SOCK_OPC_UDP_RECEIVE:
			response = sock_usr_transmit(channel, rqb, SOCK_OPC_UDP_RECEIVE);
			break;

		case SOCK_OPC_UDP_CONNECT:
			response = sock_usr_udp_connect(channel, rqb);
			break;

#if SOCK_CFG_ENABLE_MULTICASTS
		case SOCK_OPC_UDP_MULTICAST_OPEN:
			response = sock_udp_open_multicast(channel, rqb);
			break;
#endif
#endif

#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD
		case SOCK_OPC_SNMP_AGENT_ACTIVATE:
			response = sock_usr_snmp_activate(channel, rqb);
			break;

		case SOCK_OPC_SNMP_AGENT_DEACTIVATE:
			response = sock_usr_snmp_deactivate(rqb);
			break;

		case SOCK_OPC_SNMP_RECEIVE_PROVIDE:
			response = sock_usr_snmp_receive_provide(rqb);
			break;

		case SOCK_OPC_SNMP_RECEIVE_RSP:
			response = sock_usr_snmp_receive_rsp(rqb);
			break;
#endif

		case SOCK_OPC_CNTRL_TIMEPARAMS:
			response = sock_usr_cntrl_timeparams(rqb);
			break;

		case SOCK_OPC_ADDR_INFO_IND:
			response = sock_usr_addr_info_ind(channel, rqb);
			break;

		case SOCK_OPC_SET_ADDR_INFO:
			response = sock_usr_set_addr_info(rqb);
			break;

		case SOCK_OPC_GET_ANY_MAC_ADDR:
			response = sock_usr_get_any_mac_addr(rqb);
			break;

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
		case SOCK_OPC_CLUSTER_IP_CONFIG_SET:
			response = sock_usr_cluster_ip_config_set(channel, rqb);
			break;

		case SOCK_OPC_CLUSTER_IP_STATE_SET:
			response = sock_usr_cluster_ip_state_set(channel, rqb);
			break;

		case SOCK_OPC_CLUSTER_IP_GET:
			response = sock_usr_cluster_ip_get(rqb);
			break;

		case SOCK_OPC_CLUSTER_IP_IND:
			response = sock_usr_cluster_ip_ind(channel, rqb);
			break;
#endif

		case SOCK_OPC_SET_ROUTE:
			response = sock_usr_set_route(rqb);
			break;

		default:
			response = SOCK_RSP_ERR_SYNTAX;
	}


	if (response != SOCK_RSP_OK_ACTIVE)  /* pass back rqb to user if finished  */
	{
		sock_usr_callback(channel, response, &rqb);
	}

	SOCK_EXIT();
	return;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_GET_ANY_MAC_ADDR: return any valid mac address
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_get_any_mac_addr(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_RQB_ARGS_GET_ANY_MAC_ADDR_PTR_TYPE gmac = &rqb->args.get_any_mac_addr;
	LSA_BOOL ret;

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
	ret = (TCIP_OK == tcip_get_any_interface_mac_addr(&gmac->InterfaceID, gmac->MacAddr.Adr, sizeof(gmac->MacAddr.Adr)) ? LSA_TRUE : LSA_FALSE);
#else
	ret = SOCK_GET_ANY_MAC_ADDR(&gmac->InterfaceID, gmac->MacAddr.Adr, sizeof(gmac->MacAddr.Adr) );
#endif

	if( ret )
	{
		return SOCK_RSP_OK;
	}
	else
	{
		return SOCK_RSP_ERR_PARAM;
	}
}


/*------------------------------------------------------------------------------
// SOCK_OPC_ADDR_INFO_IND
// store indication resource for channel or return immediately if differences
// detected
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_addr_info_ind(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_ADDR_INFO_IND_PTR_TYPE addr_info_ind;

	if( sock_is_not_null(channel->rqb_addr_ind) ) /* only one indication resource allowed */
	{
		return SOCK_RSP_ERR_SEQUENCE;
	}

	addr_info_ind = &p_rqb->args.addr_info_ind;

	/* range check of number of interfaces */

	if( addr_info_ind->InterfacesCount < 1
		|| addr_info_ind->InterfacesCount > SOCK_CFG_MAX_INTERFACE_CNT
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	/* range check of passed InterfaceID */

	{
	int i;

	for( i=0; i<addr_info_ind->InterfacesCount; i++ )
	{
		if( addr_info_ind->AddrInfo[i].InterfaceID < 1
			|| addr_info_ind->AddrInfo[i].InterfaceID > SOCK_MAX_INTERFACE_ID
			)
		{
			return SOCK_RSP_ERR_PARAM;
		}
	}

	/* check all interfaces in indication resource against local mirror */

	addr_info_ind->InterfacesChanged = 0;  /* clear bit field */

	for( i=0; i<addr_info_ind->InterfacesCount; i++ )
	{
		SOCK_ADDR_INFO_PTR_TYPE addr_info = &addr_info_ind->AddrInfo[i];
		SOCK_ADDR_INFO_PTR_TYPE addr_mir = &sock_data.addr_info[addr_info->InterfaceID-1];

		if( sock_usr_compare_ip_suite(addr_info, addr_mir) )
		{
			sock_usr_copy_addr_info(addr_info, addr_mir);

			addr_info_ind->InterfacesChanged |= ((LSA_UINT32)1 << addr_mir->InterfaceID);
		}
	}
	}

	if( addr_info_ind->InterfacesChanged )  /* at least one bit set in bit field -> change detected */
	{
		return SOCK_RSP_OK;
	}
	else /* no change, store rqb for this channel */
	{
		channel->rqb_addr_ind = p_rqb;

		return SOCK_RSP_OK_ACTIVE;
	}
}


/*------------------------------------------------------------------------------
// transfer address information for two buffers
//----------------------------------------------------------------------------*/
LSA_VOID
sock_usr_copy_addr_info(
	SOCK_ADDR_INFO_PTR_TYPE p_dest,
	SOCK_ADDR_INFO_CONST_PTR_TYPE p_src
) {
	SOCK_ASSERT(p_dest->InterfaceID == p_src->InterfaceID );  // only for the same interface!

	if(sock_is_not_null(p_dest->NoS.pName)
	   && sock_is_not_null(p_src->NoS.pName)
	   )
	{
		p_dest->NoS.Length = p_src->NoS.Length;
		SOCK_MEMCPY(p_dest->NoS.pName, p_src->NoS.pName, p_src->NoS.Length );
		p_dest->NoS.Remanent = p_src->NoS.Remanent;
	}

	p_dest->CurrentIPSuite = p_src->CurrentIPSuite;
	p_dest->ExpectedIPSuite = p_src->ExpectedIPSuite;
	p_dest->EmergencyIPSuite = p_src->EmergencyIPSuite;

	p_dest->CurrentIPSuiteIsEmergency = p_src->CurrentIPSuiteIsEmergency;
	p_dest->CurrentIPSuiteConflictDetected = p_src->CurrentIPSuiteConflictDetected;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SET_ADDR_INFO: set ip suite for one interface
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_set_addr_info(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_set_addr_info")

	SOCK_ADDR_INFO_PTR_TYPE pInfo = &rqb->args.set_addr_info;
	SOCK_ADDR_INFO_PTR_TYPE ai;
	LSA_UINT16 response = SOCK_RSP_OK;
	LSA_BOOL ret;

	if(	   pInfo->InterfaceID < 1
		|| pInfo->InterfaceID > SOCK_MAX_INTERFACE_ID
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	if ( pInfo->CurrentIPSuiteIsEmergency != SOCK_CURRENT_IPSUITE_IS_EMERGENCY_NO && pInfo->CurrentIPSuiteIsEmergency != SOCK_CURRENT_IPSUITE_IS_EMERGENCY_YES )
	{
		return SOCK_RSP_ERR_PARAM;
	}

	if (pInfo->CurrentIPSuiteConflictDetected != SOCK_CURRENT_IPSUITE_CONFLICT_DETECTED_NO && pInfo->CurrentIPSuiteConflictDetected != SOCK_CURRENT_IPSUITE_CONFLICT_DETECTED_YES)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	ai = &sock_data.addr_info[pInfo->InterfaceID-1];  /* InterfaceID : starts with 1 */

	/*
	 * set ip suite if different to current setting (AP00785918 + AP00832648)
	 */
	if(    ai->CurrentIPSuite.IpAddr.s_addr != pInfo->CurrentIPSuite.IpAddr.s_addr
		|| ai->CurrentIPSuite.NetMask.s_addr != pInfo->CurrentIPSuite.NetMask.s_addr
		)
	{

#if SOCK_CFG_OPEN_BSD
		if(     ai->CurrentIPSuite.IpAddr.s_addr != 0
			&& (ai->CurrentIPSuite.IpAddr.s_addr != pInfo->CurrentIPSuite.IpAddr.s_addr))
		{
			sock_usr_abort_tcp_connections(ai->CurrentIPSuite.IpAddr.s_addr);
		}
#endif
#if SOCK_CFG_INTERNICHE
		ret = (TCIP_OK == tcip_set_ip_suite(pInfo->InterfaceID,
											 pInfo->CurrentIPSuite.IpAddr.s_addr,
											 pInfo->CurrentIPSuite.NetMask.s_addr) ? LSA_TRUE : LSA_FALSE);
#elif SOCK_CFG_OPEN_BSD
		ret = (TCIP_OK == tcip_set_ip_suite(pInfo->InterfaceID,
			pInfo->CurrentIPSuite.IpAddr.s_addr,
			pInfo->CurrentIPSuite.NetMask.s_addr) ? LSA_TRUE : LSA_FALSE);
#else
		ret = SOCK_SET_IPSUITE( pInfo->InterfaceID,
								 pInfo->CurrentIPSuite.IpAddr,
								 pInfo->CurrentIPSuite.NetMask);
#endif
	}
	else
	{
		ret = LSA_TRUE;
	}

	if( ret )
	{
		sock_usr_copy_addr_info(ai, pInfo );  /* copy to local mirror */

		sock_usr_check_addr_info_changes(ai);  /* indicate users */

		response = SOCK_RSP_OK;
	}
	else
	{
		response = SOCK_RSP_ERR_PARAM;
	}

	return response;
}


/*------------------------------------------------------------------------------
// return indication for each channel where user provided indication resource
//----------------------------------------------------------------------------*/
LSA_VOID
sock_usr_check_addr_info_changes(
	SOCK_ADDR_INFO_CONST_PTR_TYPE p_ai_mirror
) {
	SOCK_LOG_FCT("sock_mib2_check_sys_changes")

	int idx;

	for(idx = 0; idx < SOCK_CFG_MAX_CHANNELS; idx++)
	{
		SOCK_CHANNEL_PTR_TYPE channel = &sock_data.sock_channels[idx];

		if( channel->state != SOCK_CHANNEL_OPENED )
		{
			continue;
		}

		/* indicate change to all users which provided SOCK_OPC_MIB2_SYSGROUP_DATA_IND */

		if ( sock_is_not_null(channel->rqb_addr_ind ) )
		{
			int j;

			SOCK_ADDR_INFO_IND_PTR_TYPE aind = & (channel->rqb_addr_ind->args.addr_info_ind);

			for( j=0; j<aind->InterfacesCount; j++ )
			{
				if( aind->AddrInfo[j].InterfaceID == p_ai_mirror->InterfaceID )
				{
					LSA_BOOL bDataChanged = sock_usr_compare_ip_suite(&aind->AddrInfo[j], p_ai_mirror);

					if( bDataChanged )
					{
						sock_usr_copy_addr_info(&aind->AddrInfo[j], p_ai_mirror);

						aind->InterfacesChanged |= ((LSA_UINT32)1 << p_ai_mirror->InterfaceID);

						sock_usr_callback(channel, SOCK_RSP_OK, &channel->rqb_addr_ind); /* rqb_addr_ind <- LSA_NULL */
					}

					break;
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
// compares content of passed indication resource with passed current interface setting
//----------------------------------------------------------------------------*/
LSA_BOOL
sock_usr_compare_ip_suite(
	SOCK_ADDR_INFO_CONST_PTR_TYPE p_ind,
	SOCK_ADDR_INFO_CONST_PTR_TYPE p_curr
) {
	if((p_ind->CurrentIPSuite.IpAddr.s_addr) != (p_curr->CurrentIPSuite.IpAddr.s_addr))
	{
		return LSA_TRUE;
	}
	if((p_ind->CurrentIPSuite.NetMask.s_addr) != (p_curr->CurrentIPSuite.NetMask.s_addr))
	{
		return LSA_TRUE;
	}

	if((p_ind->ExpectedIPSuite.IpAddr.s_addr) != (p_curr->ExpectedIPSuite.IpAddr.s_addr))
	{
		return LSA_TRUE;
	}
	if((p_ind->ExpectedIPSuite.NetMask.s_addr) != (p_curr->ExpectedIPSuite.NetMask.s_addr))
	{
		return LSA_TRUE;
	}

	if((p_ind->EmergencyIPSuite.IpAddr.s_addr) != (p_curr->EmergencyIPSuite.IpAddr.s_addr))
	{
		return LSA_TRUE;
	}
	if((p_ind->EmergencyIPSuite.NetMask.s_addr) != (p_curr->EmergencyIPSuite.NetMask.s_addr))
	{
		return LSA_TRUE;
	}

	if( sock_is_not_null(p_ind->NoS.pName) )
	{
		if((p_ind->NoS.Length) != (p_curr->NoS.Length))
		{
			return LSA_TRUE;
		}
		else
		{
			if (SOCK_MEMCMP(p_ind->NoS.pName, p_curr->NoS.pName, p_ind->NoS.Length))
			{
				return LSA_TRUE;
			}
		}
	}
	return LSA_FALSE;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CONNECT_WITH_OPEN
//----------------------------------------------------------------------------*/
static LSA_UINT16
sock_usr_connect_with_open(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_connect_with_open")

	LSA_UINT16 response;
	SOCK_SOCKET_PTR soc;

	SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE
		, "fd=0x%x: [rqb =0x%X]"
		, rqb->args.connect_with_open.sock_fd,rqb
		);

	soc = sock_socket_from_channel(channel, rqb->args.connect_with_open.sock_fd);

	if ( sock_is_null(soc) )
	{
		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		if (soc->type == SOCK_LOW_DGRAM)
		{
			response = SOCK_RSP_ERR_REF;
		}
		else
		{
			if (soc->state != SOCK_CN_STATE_OPENED) /* see sock_tcp_open() */
			{
				response = SOCK_RSP_ERR_SEQUENCE;
			}
			else
			{
				response = sock_tcp_connect_with_open(soc, rqb);

				/* on error do nothing here, see 1187294 */
			}
		}
	}

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE
		, "fd=0x%x: [rqb =0x%x] response=0x%x"
		, rqb->args.connect_with_open.sock_fd, rqb, response
		);

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_ACCEPT_NOTIFY
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_accept_notify(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_UINT16 response;
	SOCK_SOCKET_PTR soc;

	soc = sock_socket_from_channel(channel, rqb->args.conn.sock_fd);

	if (sock_is_null(soc))
	{
		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		if (soc->type == SOCK_LOW_DGRAM)
		{
			response = SOCK_RSP_ERR_REF;
		}
		else
		{
			if (soc->state != SOCK_CN_STATE_LISTENING )
			{
				response = SOCK_RSP_ERR_SEQUENCE;
			}
			else
			{
				if( sock_is_null(soc->accept.curr))
				{
					soc->accept.curr = rqb;
#if SOCK_CFG_OPEN_BSD
					/* due to the new event mechanism, accept has to be called immediately if no accept resource is available */
					/* otherwise, the event is lost */
					sock_tcp_accept(soc);
#endif
				}
				else
				{
					sock_queue_enqueue(&soc->accept.queue, rqb);
				}

				response = SOCK_RSP_OK_ACTIVE;
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_EXCEPTION_NOTIFY: store exception rqb
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_except_notify(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_except_notify")

	LSA_UINT16 response;
	SOCK_SOCKET_PTR soc;

	soc = sock_socket_from_channel(channel, rqb->args.except_rq.sock_fd);

	if ( sock_is_null(soc) )
	{
		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		if (soc->type == SOCK_LOW_DGRAM)
		{
			response = SOCK_RSP_ERR_REF;
		}
		else
		{
			if (soc->state == SOCK_CN_STATE_DISCONNECTING) /* the connection was abborted */
			{
				response =  soc->abort_resp;
			}
			else
			{
				if ( sock_is_not_null(soc->exception_rb) /* already there */
					 || sock_is_not_null(soc->open_close_rb) /* close active */
				     )
				{
					response = SOCK_RSP_ERR_SEQUENCE;
				}
				else
				{
					SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
						, "SOCK_OPC_EXCEPTION_NOTIFY registered for socket handle(%d)"
						, soc->dev_handle
						);

					soc->exception_rb = rqb;

					response = SOCK_RSP_OK_ACTIVE;
				}
			}
		}
	}
	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_RECEIVE, SOCK_OPC_UDP_RECEIVE, SOCK_OPC_SEND, SOCK_OPC_UDP_SEND
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_usr_transmit(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb,
	LSA_OPCODE_TYPE opcode
) {
	SOCK_LOG_FCT("sock_usr_transmit")

	LSA_RESPONSE_TYPE response;

	SOCK_SOCKET_PTR soc = sock_socket_from_channel(channel, rqb->args.data.sock_fd); /* get dedicated soc */

	if (sock_is_null(soc))
	{
		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "response = %d"
			, SOCK_RSP_ERR_REF
			);

		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		if (soc->state != SOCK_CN_STATE_CONNECTED  /* transmit in connected state only */
			|| sock_is_not_null(soc->open_close_rb) /* pending close (open impossible, see TCIP_SRV_MULTICAST_DONE() */
			)
		{
			if( soc->state == SOCK_CN_STATE_DISCONNECTING )
			{
				response = soc->abort_resp;
			}
			else
			{
				response = SOCK_RSP_ERR_SEQUENCE;
			}

			SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_NOTE
				, "response = %d"
				, response
				);
		}
		else
		{
			if( ( (opcode == SOCK_OPC_RECEIVE) && (soc->type == SOCK_LOW_DGRAM) )
				|| ( (opcode == SOCK_OPC_SEND) && (soc->type == SOCK_LOW_DGRAM) )
				|| ( (opcode == SOCK_OPC_UDP_RECEIVE) && (soc->type == SOCK_LOW_STREAM) )
				|| ( (opcode == SOCK_OPC_UDP_SEND) && (soc->type == SOCK_LOW_STREAM) )
			  )
			{
				response = SOCK_RSP_ERR_REF;
			}
			else
			{
				switch (opcode)
				{
				case SOCK_OPC_RECEIVE:
				case SOCK_OPC_UDP_RECEIVE:
					response = sock_usr_receive(rqb, soc);
					break;

				case SOCK_OPC_SEND:
				case SOCK_OPC_UDP_SEND:
					response = sock_usr_send(rqb, soc);
					break;

				default: response = SOCK_RSP_ERR_REF; /* will never happen, but prevent C4701: potentially uninitialized local variable 'response' used */
					break;
				}
			}
		}
	}

	return response;
}


/*------------------------------------------------------------------------------
//	SOCK_OPC_SEND, SOCK_OPC_UDP_SEND
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_usr_send(
	SOCK_UPPER_RQB_PTR_TYPE rqb,
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_usr_send")

	LSA_RESPONSE_TYPE response;

	SOCK_RQB_ARGS_DATA_PTR_TYPE data = & rqb->args.data;

	if (sock_is_null(data->buffer_ptr) /* invalid buffer pointer */
		|| data->offset + data->data_length > data->buffer_length  /* buffer too small */
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
		|| (soc->type ==  SOCK_LOW_DGRAM /* if UDP send*/
			&& (!(data->flags & SOCK_STANDARD_IP_ONLY) /* and flag not set */
				 && ((data->loc_addr.sin_addr.s_addr == 0 /* and neither addr and family zero nor addr and family not zero*/
					  && data->loc_addr.sin_family != 0
					 )
					 || (data->loc_addr.sin_addr.s_addr != 0
						 && data->loc_addr.sin_family == 0
						)
					)
				)
			)
#endif
		)
	{
		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
#if TCIP_INT_CFG_TRACE_ON
		controller_add_user_send_measure_1(rqb);
#endif
		if( sock_is_null(soc->send.curr))
		{
			soc->send.curr = rqb;
		}
		else
		{
			sock_queue_enqueue(&soc->send.queue, rqb);  /* put rqb into send queue of this device */
		}

		sock_socket_send(soc);

		response = SOCK_RSP_OK_ACTIVE;
	}

	return response;
}


/*------------------------------------------------------------------------------
//	SOCK_OPC_RECEIVE, SOCK_OPC_UDP_RECEIVE
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_usr_receive(
	SOCK_UPPER_RQB_PTR_TYPE rqb,
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_usr_receive")

	LSA_RESPONSE_TYPE response;
	SOCK_RQB_ARGS_DATA_PTR_TYPE data = & rqb->args.data;

	if(sock_is_null(data->buffer_ptr) /* invalid buffer pointer */
			|| data->offset + data->data_length > data->buffer_length /* buffer too small */
			|| ((soc->type == SOCK_LOW_DGRAM) && data->flags & SOCK_READ_ALL) /* SOCK_READ_ALL does not make sense for UDP */
			)
	{
		response = SOCK_RSP_ERR_PARAM;
	}
	else
	{
		if(sock_is_null(soc->recv.curr))
		{
			soc->recv.curr = rqb;
		}
		else
		{
			sock_queue_enqueue(&soc->recv.queue, rqb); /* put rqb into recv queue of this device */
		}

		sock_socket_receive(soc);

		response = SOCK_RSP_OK_ACTIVE;
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CLOSE
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_close(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_close")

	SOCK_SOCKET_PTR soc;
	LSA_UINT16 response;

	SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE
		,"SOCK_OPC_CLOSE for socket handle(0x%x),  rqb(0x%X)"
		, rqb->args.close.sock_fd, rqb
		);

	soc = sock_socket_from_channel(channel, rqb->args.conn.sock_fd);

	if ( sock_is_null(soc) )
	{
		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		if( sock_is_not_null(soc->open_close_rb) ) /* pending rqb -> _ERR_REF */
		{
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "closing connection(%d) with pending rqb active: SOCK_RSP_ERR_REF"
				, rqb->args.close.sock_fd
				);

			response = SOCK_RSP_ERR_REF; /* see AP01533799 */
		}
		else
		{
			soc->open_close_rb = rqb;

			response = SOCK_RSP_OK_ACTIVE;

			if( channel->detail_ptr->sock_close_at_once != 0
				|| sock_is_null(soc->send.curr)
				)
			{
				sock_socket_close(soc);
			}
		}
	}

	return response;
}


#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD
/*------------------------------------------------------------------------------
// SOCK_OPC_SNMP_AGENT_ACTIVATE: activation of snmp agent
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_snmp_activate(
	SOCK_CHANNEL_PTR_TYPE p_cha,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	if (p_rqb->args.snmp_act.type > SOCK_EXTERNER_SNMP_AGENT)  /* 0, 1, 2 allowed */
	{
		return  SOCK_RSP_ERR_PARAM;
	}

	return sock_snmp_activate(p_cha, p_rqb);
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SNMP_AGENT_DEACTIVATE: deactivation of snmp agent
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_snmp_deactivate(
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_RQB_ARGS_SNMP_DEACTIVATE_PTR_TYPE da = &p_rqb->args.snmp_deact;

	/*
	 * valid handle range: 0 .. SOCK_MAX_SNMP_AGENTS-1
	 */
	if( da->snmp_handle >= SOCK_MAX_SNMP_AGENTS )
	{
		return SOCK_RSP_ERR_PARAM;
	}

	/*
	 * check sequence: deactivate only active agent
	 */
	if( sock_data.snmp_agents_data[da->snmp_handle].state != SOCK_SNMP_ACTIVE )
	{
			return SOCK_RSP_ERR_SEQUENCE;
	}

	/*
	 *  check channel affinity: agent can be deactivated only by the same channel which activated the agent
	 */
	if( SOCK_RQB_GET_HANDLE(p_rqb) != sock_data.snmp_agents_data[da->snmp_handle].hChannel )
	{
		return SOCK_RSP_ERR_REF;
	}

	return sock_snmp_deactivate(p_rqb);
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SNMP_RECEIVE_PROVIDE: resource provide from external snmp agent
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_snmp_receive_provide(
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_RQB_ARGS_SNMP_RECEIVE_PTR_TYPE rcv = & p_rqb->args.snmp_recv;
	SNMP_AGENT_DATA_PTR pAgent;

	/*
	 *  valid handle range: 0 .. SOCK_MAX_SNMP_AGENTS-1
	 */
	if( rcv->snmp_handle >= SOCK_MAX_SNMP_AGENTS )
	{
		return SOCK_RSP_ERR_PARAM;
	}

	pAgent = &sock_data.snmp_agents_data[rcv->snmp_handle];

	/*
	 *  not activated before or used after close_channel ?
	 */
	if( pAgent->state != SOCK_SNMP_ACTIVE )
	{
		return SOCK_RSP_ERR_SEQUENCE;
	}

	/*
	 *  check channel affinity: resources can be provided only by the same channel which activated the agent
	 */
	if( SOCK_RQB_GET_HANDLE(p_rqb) != pAgent->hChannel )
	{
		return SOCK_RSP_ERR_REF;
	}

	/* check buffer size: define in sock_usr.h must be used */

	if( rcv->buffer_size != SOCK_SNMP_BUFFER_SIZE )
	{
		return SOCK_RSP_ERR_PARAM;
	}

	/*
	 *  put rqb into agents' queue
	 */
	sock_snmp_recv_put(pAgent, p_rqb);

	return SOCK_RSP_OK_ACTIVE;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SNMP_RECEIVE_RSP: re-provide from external agent
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_usr_snmp_receive_rsp(
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_RQB_ARGS_SNMP_RECEIVE_PTR_TYPE rcv = & p_rqb->args.snmp_recv;
	SNMP_AGENT_DATA_PTR pAgent;

	/*
	 *  valid handle range: 0 .. SOCK_MAX_SNMP_AGENTS-1
	 */
	if( rcv->snmp_handle >= SOCK_MAX_SNMP_AGENTS )
	{
		return SOCK_RSP_ERR_PARAM;
	}

	if (rcv->snmp_done != SOCK_SNMP_DONE_NO && rcv->snmp_done != SOCK_SNMP_DONE_YES)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	pAgent = &sock_data.snmp_agents_data[rcv->snmp_handle];

	/*
	 *  not activated before or used after close_channel ?
	 */
	if( pAgent->state != SOCK_SNMP_ACTIVE )
	{
		return SOCK_RSP_ERR_SEQUENCE;
	}

	/*
	 *  check channel affinity: resources can be provided only by the same channel which activated the agent
	 */
	if( SOCK_RQB_GET_HANDLE(p_rqb) != pAgent->hChannel )
	{
		return SOCK_RSP_ERR_REF;
	}

	sock_snmp_receive_rsp(pAgent, p_rqb);

	return SOCK_RSP_OK_ACTIVE;
}
#endif


/*------------------------------------------------------------------------------
// returns LSA_TRUE in case par == SOCK_NO_CHANGE_TIMEPARMS
// or ( lowRange <= par <= highRange, otherwise LSA_FALSE
//----------------------------------------------------------------------------*/
static LSA_BOOL
sock_usr_iRangeCheck(
	LSA_UINT32 par,
	LSA_UINT32 lowRange,
	LSA_UINT32 highRange
) {
	if(par == SOCK_NO_CHANGE_TIMEPARMS)
	{
		return(LSA_TRUE);
	}
	if( par < lowRange)
	{
		return(LSA_FALSE);
	}
	if( par > highRange)
	{
		return(LSA_FALSE);
	}
	return(LSA_TRUE);
}


/*------------------------------------------------------------------------------
// deactivate all tcp sockets (set state SOCK_CN_STATE_DISCONNECTING,
// return all user resources
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_usr_deactivate_tcp_sockets(
	LSA_VOID
) {
	SOCK_LOG_FCT("sock_usr_deactivate_tcp_sockets")

	SOCK_SOCKET_PTR soc;
	LSA_INT i;

	for (i=0; i<SOCK_CFG_MAX_SOCKETS; i++ )
	{
		if( SOCK_CN_STATE_FREE == sock_data.sockets[i].state
			|| sock_data.sockets[i].type == SOCK_LOW_DGRAM/*sock_data.sockets[i].sock_udp*/ )
		{
			continue;
		}

		soc = & sock_data.sockets[i];

		if (soc->state == SOCK_CN_STATE_CONNECTED)
		{
			sock_socket_return_all_resources(soc, SOCK_RSP_ERR_LOC_ABORT);

			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE
				, "requested dev_handle(%d), socket struct ptr(0x%X), state(0x%x)"
				, soc->dev_handle, soc, soc->state
				);
		}
	}
}

#if SOCK_CFG_OPEN_BSD
/*------------------------------------------------------------------------------
// deactivate all tcp sockets (set state SOCK_CN_STATE_DISCONNECTING,
// return all user resources
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_usr_abort_tcp_connections(
	LSA_UINT32 old_address
) {
	SOCK_LOG_FCT("sock_usr_abort_tcp_connections")

	SOCK_SOCKET_PTR soc;
	LSA_INT i;

	for (i=0; i<SOCK_CFG_MAX_SOCKETS; i++ )
	{
		if( SOCK_CN_STATE_FREE == sock_data.sockets[i].state || sock_data.sockets[i].type == SOCK_LOW_DGRAM)
		{
			continue;
		}

		soc = & sock_data.sockets[i];

		if (soc->loc_addr.sin_addr.s_addr == old_address)
		{
			int native_error;
			LSA_RESPONSE_TYPE response;

			if (SOCK_CN_STATE_LISTENING == soc->state) /* assure break from accept loop, see 2004942*/
			{
				soc->server_state = SOCK_CN_STATE_DAMAGED; /* set server_state to damaged */
			}

			response = sock_low_abort_tcp_session(soc->lower_handle, &native_error);

			soc->lower_handle = SOCK_LOW_INVALID_HANDLE; /* see 2459909 */

			if (response != SOCK_RSP_OK)
			{
				SOCK_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_ERROR
					, "error closing TCP socket; lower_handle(%d), dev_handle(%d), response(%d), native_error(%d)"
					, soc->lower_handle, soc->dev_handle, response, native_error
					);
			}

			if (SOCK_CN_STATE_LISTENING != soc->state)
			{
				sock_socket_return_all_resources(soc, SOCK_RSP_ERR_REM_ABORT); /* HU 25.04.17: assure behaviour as documented, this was triggered by readable event before */
			}
		}
	}
}
#endif

/*------------------------------------------------------------------------------
// SOCK_OPC_CNTRL_TIMEPARAMS
//----------------------------------------------------------------------------*/
static LSA_UINT16
sock_usr_cntrl_timeparams(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_UINT16 response				= LSA_RSP_OK;
	SOCK_RQB_ARGS_TIMEPARAMS_PTR tp	= &rqb->args.timeparam_rq;

	switch(rqb->args.timeparam_rq.timeopt)
	{
	case SOCK_GET_TIMEPARAMS:

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
			tcip_get_timeparams(&tp->keepalivetime, &tp->connectiontimeout, &tp->retranstimeout, &tp->retransmitcount );
#else
			SOCK_TCPIP_GET_TIMEPARAMS(&tp->keepalivetime, &tp->connectiontimeout, &tp->retranstimeout, &tp->retransmitcount);
#endif
		break;

	case SOCK_SET_TIMEPARAMS:
	case SOCK_SET_TIMEPARAMS_CLOSE_TCP_CONN:
		{
			/* check the parameter ranges first times are in msec */
			if(tp->keepalivetime == SOCK_KEEPALIVE_OFF)
			{
				SOCK_KEEPALIVE_ACTIVE = LSA_FALSE;
				break;
			}
			if (!sock_usr_iRangeCheck(tp->retranstimeout, 1000, (65535*1000)))
			{
				return(SOCK_RSP_ERR_PARAM);
			}
			if (!sock_usr_iRangeCheck(tp->connectiontimeout, 1000, (65535*1000)))
			{
				return(SOCK_RSP_ERR_PARAM);
			}
			if (!sock_usr_iRangeCheck(tp->retransmitcount, 1, 12))
			{
				return(SOCK_RSP_ERR_PARAM);
			}
			if (!sock_usr_iRangeCheck(tp->keepalivetime, 1000, (65535*1000)))
			{
				return(SOCK_RSP_ERR_PARAM);
			}
			else
			{   /* keepalive time is in range, so set keepalive to be active (nevertheless it was already on!) */
				SOCK_KEEPALIVE_ACTIVE = LSA_TRUE;
			}

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
			tcip_set_timeparams(tp->keepalivetime, tp->connectiontimeout, tp->retranstimeout, tp->retransmitcount, SOCK_NO_CHANGE_TIMEPARMS);
			tcip_get_timeparams(&tp->keepalivetime, &tp->connectiontimeout, &tp->retranstimeout, &tp->retransmitcount);
#else
			SOCK_TCPIP_SET_TIMEPARAMS(tp->keepalivetime, tp->connectiontimeout, tp->retranstimeout, tp->retransmitcount);
			SOCK_TCPIP_GET_TIMEPARAMS(&tp->keepalivetime, &tp->connectiontimeout, &tp->retranstimeout, &tp->retransmitcount);
#endif
			break;
		}

	default:
		response = SOCK_RSP_ERR_PARAM;
	}

	if( tp->timeopt == SOCK_SET_TIMEPARAMS_CLOSE_TCP_CONN ) /*check for closing the TCP-Connections? */
	{
		sock_usr_deactivate_tcp_sockets();
	}

	if(SOCK_KEEPALIVE_ACTIVE == LSA_FALSE)
	{
		tp->keepalivetime = SOCK_KEEPALIVE_OFF;
	}

	return response;
}


/*------------------------------------------------------------------------------
// trigger select
//----------------------------------------------------------------------------*/
#if !SOCK_INT_CFG_SOCKET_EVENTS
LSA_VOID
sock_doselect(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr
) {
	SOCK_LOG_FCT("sock_doselect")

	SOCK_ENTER();

	SOCK_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
		,"Call from Select-Mimic"
		);

	sock_socket_do_select( rqb_ptr );

	SOCK_EXIT();
}
#endif


/*------------------------------------------------------------------------------
// trigger select with timeout parameter
//----------------------------------------------------------------------------*/
#if !SOCK_INT_CFG_SOCKET_EVENTS
LSA_VOID
sock_doselect2(
	LSA_INT32 timeout
) {
	SOCK_LOG_FCT("sock_doselect2")

	SOCK_ENTER();

	SOCK_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"Call from Select-Mimic");

	if (sock_data.socket_count > 0 )
	{
		sock_socket_execute_select(timeout);
	}

	SOCK_START_SELECT();

	SOCK_EXIT();
}
#endif

/*------------------------------------------------------------------------------
//	sets the response of the given request-block and calls the
//        request-done-fct (given in SOCK_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID
sock_usr_callback(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 response,
	SOCK_UPPER_RQB_PTR_TYPE *upper_ptr
) {
	SOCK_LOG_FCT("sock_usr_callback")
	SOCK_UPPER_RQB_PTR_TYPE  rb;

	SOCK_ASSERT(sock_is_not_null(channel));
	SOCK_ASSERT(sock_is_not_null(upper_ptr));
	SOCK_ASSERT(sock_is_not_null(*upper_ptr));

	rb = *upper_ptr;
	*upper_ptr = LSA_NULL;		/* sanity - never use this again */

	SOCK_RQB_SET_HANDLE (rb, channel->upper_handle);
	SOCK_RQB_SET_RESPONSE (rb, response);

	SOCK_UPPER_TRACE_05 (0, LSA_TRACE_LEVEL_NOTE
		, "<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u) f-h(%u)"
		, rb, SOCK_RQB_GET_HANDLE (rb), SOCK_RQB_GET_OPCODE (rb), SOCK_RQB_GET_RESPONSE (rb), channel->own_handle
		);

	SOCK_REQUEST_UPPER_DONE (channel->req_done_ptr, rb, channel->sys_ptr);
}


/*------------------------------------------------------------------------------
// SOCK_OPC_UDP_CONNECT
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_usr_udp_connect(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_usr_udp_connect")

	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc;

	SOCK_RQB_ARGS_UDP_CONN_PTR_TYPE udp_conn = &rqb->args.udp_conn;

	soc = sock_socket_from_channel(channel, udp_conn->sock_fd);

	if (sock_is_null(soc))
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "invalid socket handle(%d)"
			, udp_conn->sock_fd
			);

		response = SOCK_RSP_ERR_REF;
	}
	else if( soc->type == SOCK_LOW_STREAM )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "socket handle(%d) does not refer to udp socket"
			, udp_conn->sock_fd
			);

		response = SOCK_RSP_ERR_REF;
	}
	else
	{
		response = sock_udp_connect(soc, udp_conn);
	}

	return response;
}


#if SOCK_CFG_CLUSTER_IP_SUPPORTED
/*------------------------------------------------------------------------------
// compare function for Cluster IP data (except InterfaceID)
// return: 0 .. Cluster IP data is equal
//         1 .. Cluster IP data is not equal
//----------------------------------------------------------------------------*/
static int sock_usr_cluster_ip_ind_data_compare(
	SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterIndLeft,
	SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterIndRight
) {
	if (pClusterIndLeft->Info.ClusterID != pClusterIndRight->Info.ClusterID
		|| pClusterIndLeft->Info.ClusterIPState != pClusterIndRight->Info.ClusterIPState
		|| pClusterIndLeft->Info.ClusterIPAddress.s_addr != pClusterIndRight->Info.ClusterIPAddress.s_addr
		|| pClusterIndLeft->Info.ClusterSubnetMask.s_addr != pClusterIndRight->Info.ClusterSubnetMask.s_addr
		|| pClusterIndLeft->ClusterIPConflict != pClusterIndRight->ClusterIPConflict
		)
	{
		return 1; /* deviation */
	}
	else
	{
		return 0; /* equality */
	}
}


/*------------------------------------------------------------------------------
// copy Cluster IP info data (except InterfaceID)
//----------------------------------------------------------------------------*/
static LSA_VOID sock_usr_cluster_ip_ind_data_copy(
		SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterIndDest,
		SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterIndSrc
) {
	SOCK_ASSERT(pClusterIndDest->Info.InterfaceID == pClusterIndSrc->Info.InterfaceID);

	pClusterIndDest->Info.ClusterID = pClusterIndSrc->Info.ClusterID;

	pClusterIndDest->Info.ClusterIPState = pClusterIndSrc->Info.ClusterIPState;
	pClusterIndDest->Info.ClusterIPAddress.s_addr = pClusterIndSrc->Info.ClusterIPAddress.s_addr;
	pClusterIndDest->Info.ClusterSubnetMask.s_addr = pClusterIndSrc->Info.ClusterSubnetMask.s_addr;

	pClusterIndDest->ClusterIPConflict = pClusterIndSrc->ClusterIPConflict;
}


/*------------------------------------------------------------------------------
// retrieve cluster state from configured IP stack
//----------------------------------------------------------------------------*/
static LSA_UINT8 sock_usr_cluster_ip_state_get(LSA_UINT32 InterfaceID)
{
#if SOCK_CFG_OPEN_BSD
	return tcip_srv_cluster_ip_state_get(InterfaceID);
#else
	return SOCK_CLUSTER_IP_STATE_GET(InterfaceID);
#endif
}


/*------------------------------------------------------------------------------
// indicate users if current setting deviates from indication rqb
//----------------------------------------------------------------------------*/
LSA_VOID sock_usr_cluster_ip_ind_data_changes_check(
	LSA_UINT32 InterfaceID
) {
	int idx;

	SOCK_ASSERT(InterfaceID >= 1 && InterfaceID <= SOCK_MAX_INTERFACE_ID);

	for(idx = 0; idx < SOCK_CFG_MAX_CHANNELS; idx++)
	{
		SOCK_CHANNEL_PTR_TYPE channel = &sock_data.sock_channels[idx];

		if( channel->state != SOCK_CHANNEL_OPENED )
		{
			continue;
		}

		/* indicate change to all users which provided SOCK_OPC_CLUSTER_IP_IND */

		if (sock_is_not_null(channel->cluster_ip_ind_rqbs[InterfaceID - 1]))
		{
			SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterInfoInd = &channel->cluster_ip_ind_rqbs[InterfaceID - 1]->args.cluster_ip_ind;
			SOCK_CLUSTER_IP_IND_PTR_TYPE pLocalClusterInfo = &sock_data.cluster_ip.cluster_info[InterfaceID - 1];

			pLocalClusterInfo->Info.ClusterIPState = sock_usr_cluster_ip_state_get(InterfaceID); /* retrieve current state from configured IP stack */

			if (sock_usr_cluster_ip_ind_data_compare(pClusterInfoInd, pLocalClusterInfo))
			{
				sock_usr_cluster_ip_ind_data_copy(pClusterInfoInd, pLocalClusterInfo);

				sock_usr_callback(channel, SOCK_RSP_OK, &channel->cluster_ip_ind_rqbs[InterfaceID - 1]); /* sets indication pointer to LSA_NULL */
			}
		}
	}

	/* (!) if no indication was present when ClusterIP conflict arised, the event is lost */

	sock_data.cluster_ip.cluster_info[InterfaceID - 1].ClusterIPConflict = SOCK_CLUSTER_IP_NO_CONFLICT; /* reset */
}


/*------------------------------------------------------------------------------
// instruct configured IP stack to set the cluster data for a interface
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_usr_cluster_ip_config_set_lower(
	SOCK_CLUSTER_IP_CONFIG_PTR_TYPE pClusterConfig
) {
#if SOCK_CFG_OPEN_BSD
	LSA_RESPONSE_TYPE result = tcip_srv_cluster_ip_config_set(
		pClusterConfig->InterfaceID,
		pClusterConfig->ClusterID,
		pClusterConfig->ClusterIPAddress.S_un.S_addr,
		pClusterConfig->ClusterSubnetMask.S_un.S_addr
		);

	switch(result)
	{
	case TCIP_OK:
		return SOCK_RSP_OK_ACTIVE;

	case TCIP_ERR_PARAM:
		return SOCK_RSP_ERR_PARAM;

	case TCIP_ERR_LOWER_LAYER:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
#else
	LSA_UINT8 result = SOCK_CLUSTER_IP_CONFIG_SET(pClusterConfig->InterfaceID,
		pClusterConfig->ClusterID,
		pClusterConfig->ClusterIPAddress,
		pClusterConfig->ClusterSubnetMask
		);

	switch(result)
	{
	case SOCK_CLUSTER_IP_OK_ACTIVE:
		return SOCK_RSP_OK_ACTIVE;

	case SOCK_CLUSTER_IP_OK:
		return SOCK_RSP_OK;

	case SOCK_CLUSTER_IP_NOK_PARAMETER:
		return SOCK_RSP_ERR_PARAM;

	case SOCK_CLUSTER_IP_NOK_LOC_NET:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
#endif
}

/*------------------------------------------------------------------------------
// SOCK_OPC_CLUSTER_IP_CONFIG_SET
//----------------------------------------------------------------------------*/
LSA_UINT16 sock_usr_cluster_ip_config_set(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_RESPONSE_TYPE response;

	SOCK_CLUSTER_IP_CONFIG_PTR_TYPE pClusterConfig = &rqb->args.cluster_ip_config;

	LSA_UNUSED_ARG(channel);

	if (pClusterConfig->InterfaceID < 1
		|| pClusterConfig->InterfaceID > SOCK_MAX_INTERFACE_ID
		|| ( pClusterConfig->ClusterID == 0
			 /* allow reset */
			 && (pClusterConfig->ClusterIPAddress.s_addr != 0
				 || pClusterConfig->ClusterSubnetMask.s_addr != 0
				)
			)
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	SOCK_ASSERT(pClusterConfig->InterfaceID == sock_data.cluster_ip.cluster_info[pClusterConfig->InterfaceID-1].Info.InterfaceID);

	if (sock_is_not_null(sock_data.cluster_ip.config_set_rqb))
	{
		return SOCK_RSP_ERR_SEQUENCE;
	}

	if (pClusterConfig->ClusterID == 0 /* reset */
		&& sock_data.cluster_ip.cluster_info[pClusterConfig->InterfaceID-1].Info.ClusterID == 0 /* and no cluster configured */
		)
	{
		return SOCK_RSP_OK; /* do nothing */
	}

	response = sock_usr_cluster_ip_config_set_lower(pClusterConfig);

	if (response == SOCK_RSP_OK_ACTIVE)
	{
		sock_data.cluster_ip.config_set_rqb = rqb; /* see xxx_CONFIG_SET_DONE in sock_sys.c */
	}
	else /* config set was rejected */
	{
		sock_usr_cluster_ip_ind_data_changes_check(pClusterConfig->InterfaceID); /* always indicate users */
	}

	return response;
}


/*------------------------------------------------------------------------------
// instruct configured IP stack to set the state for a interface
//----------------------------------------------------------------------------*/
static LSA_RESPONSE_TYPE
sock_usr_cluster_ip_state_set_lower(
	LSA_UINT32 InterfaceID,
	LSA_UINT8 ClusterIPState
) {
#if SOCK_CFG_OPEN_BSD
	{
	LSA_RESPONSE_TYPE result = tcip_srv_cluster_ip_state_set(InterfaceID, ClusterIPState);

	switch (result)
	{
	case TCIP_OK:
		return SOCK_RSP_OK;

	case TCIP_ERR_PARAM:
		return SOCK_RSP_ERR_PARAM;

	case TCIP_ERR_LOWER_LAYER:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
	}
#else
	{
	LSA_UINT8 result = SOCK_CLUSTER_IP_STATE_SET(InterfaceID, ClusterIPState);

	switch(result)
	{
	case SOCK_CLUSTER_IP_OK:
		return SOCK_RSP_OK;

	case SOCK_CLUSTER_IP_NOK_PARAMETER:
		return SOCK_RSP_ERR_PARAM;

	case SOCK_CLUSTER_IP_NOK_LOC_NET:
	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
	}
#endif
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CLUSTER_IP_STATE_SET
//----------------------------------------------------------------------------*/
LSA_UINT16 sock_usr_cluster_ip_state_set(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_RESPONSE_TYPE response;

	SOCK_CLUSTER_IP_INFO_PTR_TYPE pClusterInfo;

	SOCK_CLUSTER_IP_STATE_PTR_TYPE pClusterState = &rqb->args.cluster_ip_state;

	LSA_UNUSED_ARG(channel);

	if (pClusterState->InterfaceID < 1
		|| pClusterState->InterfaceID > SOCK_MAX_INTERFACE_ID
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	if (pClusterState->ClusterIPState > SOCK_CLUSTER_IP_STATE_ACTIVE)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	SOCK_ASSERT(pClusterState->InterfaceID == sock_data.cluster_ip.cluster_info[pClusterState->InterfaceID-1].Info.InterfaceID);

	pClusterInfo = &sock_data.cluster_ip.cluster_info[pClusterState->InterfaceID-1].Info;

	pClusterInfo->ClusterIPState = sock_usr_cluster_ip_state_get(pClusterState->InterfaceID); /* retrieve current state from configured IP stack */

	if (pClusterState->ClusterIPState == pClusterInfo->ClusterIPState) /* interface has already requested state -> do nothing */
	{
		return SOCK_RSP_OK;
	}

	response  = sock_usr_cluster_ip_state_set_lower(pClusterState->InterfaceID, pClusterState->ClusterIPState);

	if (response == SOCK_RSP_OK)
	{
		pClusterInfo->ClusterIPState = pClusterState->ClusterIPState; /* update local mirror info */
	}

	sock_usr_cluster_ip_ind_data_changes_check(pClusterState->InterfaceID); /* always indicate users */

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CLUSTER_IP_GET
//----------------------------------------------------------------------------*/
LSA_UINT16 sock_usr_cluster_ip_get(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_CLUSTER_IP_INFO_PTR_TYPE pInfo;
	SOCK_CLUSTER_IP_INFO_PTR_TYPE pLocalInfo;

	pInfo = (SOCK_CLUSTER_IP_INFO_PTR_TYPE)&rqb->args.cluster_ip_get;

	if (pInfo->InterfaceID < 1
		|| pInfo->InterfaceID > SOCK_MAX_INTERFACE_ID
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}

	pLocalInfo = &sock_data.cluster_ip.cluster_info[pInfo->InterfaceID-1].Info;

	pLocalInfo->ClusterIPState = sock_usr_cluster_ip_state_get(pInfo->InterfaceID); /* retrieve current state from configured IP stack */

	if (pLocalInfo->ClusterID)
	{
		pInfo->ClusterID = pLocalInfo->ClusterID;
		pInfo->ClusterIPState = pLocalInfo->ClusterIPState;
		pInfo->ClusterIPAddress.s_addr = pLocalInfo->ClusterIPAddress.s_addr;
		pInfo->ClusterSubnetMask.s_addr = pLocalInfo->ClusterSubnetMask.s_addr;
	}
	else
	{
		pInfo->ClusterID = 0; /* not part of a cluster */
	}

	return SOCK_RSP_OK;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_CLUSTER_IP_IND
//----------------------------------------------------------------------------*/
LSA_UINT16 sock_usr_cluster_ip_ind(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_CLUSTER_IP_IND_PTR_TYPE pLocalClusterInfo;
	SOCK_CLUSTER_IP_IND_PTR_TYPE pClusterInd;

	LSA_UNUSED_ARG(channel);
	LSA_UNUSED_ARG(rqb);

	pClusterInd = &rqb->args.cluster_ip_ind;

	if (pClusterInd->Info.InterfaceID < 1
		|| pClusterInd->Info.InterfaceID > SOCK_MAX_INTERFACE_ID
		|| pClusterInd->ClusterIPConflict != SOCK_CLUSTER_IP_NO_CONFLICT
		)
	{
		return SOCK_RSP_ERR_PARAM;
	}


	if (sock_is_not_null(channel->cluster_ip_ind_rqbs[pClusterInd->Info.InterfaceID - 1]))
	{
		return SOCK_RSP_ERR_SEQUENCE; /* indication rqb already present for this interface */
	}

	/***/

	pLocalClusterInfo = &sock_data.cluster_ip.cluster_info[pClusterInd->Info.InterfaceID - 1];

	pLocalClusterInfo->Info.ClusterIPState = sock_usr_cluster_ip_state_get(pLocalClusterInfo->Info.InterfaceID); /* retrieve current state from configured IP stack */

	if (sock_usr_cluster_ip_ind_data_compare(pClusterInd, pLocalClusterInfo)) /* compare indication Cluster IP data and local Cluster IP data */
	{
		sock_usr_cluster_ip_ind_data_copy(pClusterInd, pLocalClusterInfo);

		return SOCK_RSP_OK; /* deviation -> return indication immediately */
	}
	else
	{
		channel->cluster_ip_ind_rqbs[pClusterInd->Info.InterfaceID - 1] = rqb;

		return SOCK_RSP_OK_ACTIVE; /* no deviation -> keep rqb for this interface */
	}
}
#endif


/*------------------------------------------------------------------------------
// SOCK_OPC_SET_ROUTE
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_usr_set_route(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_BOOL ret;
	SOCK_SET_ROUTE_PTR_TYPE pSetRoute = & rqb->args.set_route;

	#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD
		ret = tcip_set_route(pSetRoute->route.IpAddr.s_addr, pSetRoute->route.NetMask.s_addr, pSetRoute->route.Gateway.s_addr);

		switch (ret)
		{
		case TCIP_OK: return SOCK_RSP_OK;
		case TCIP_ERR_PARAM: return SOCK_RSP_ERR_PARAM;
		default: return SOCK_RSP_ERR_LOC_NET;
		}
	#else
		ret = SOCK_SET_ROUTE(pSetRoute->route.IpAddr, pSetRoute->route.NetMask, pSetRoute->route.Gateway);

		switch (ret)
		{
		case SOCK_SET_ROUTE_OK: return SOCK_RSP_OK;
		case SOCK_SET_ROUTE_ERR_PARAM: return SOCK_RSP_ERR_PARAM;
		default: return SOCK_RSP_ERR_LOC_NET;
		}
	#endif
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
