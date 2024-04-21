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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the LSA user-interface                                        */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  2
#define TCIP_MODULE_ID     2

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_CFG_INTERNICHE
/* OBSD_ITGR -- TODO: diese funktion gehört nach tcip_iniche. Klären mit Ulli */
static LSA_VOID tcip_rebuild_route_table(LSA_VOID);
#endif

/*------------------------------------------------------------------------------
//	sets the response of the given request-block and calls the
//        request-done-fct (given in TCIP_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
/*lint -e{818} symbol channel Pointer parameter 'Symbol' (Location) could be declared ptr to const */
LSA_VOID
tcip_callback( TCIP_CHANNEL_CONST_PTR_TYPE channel,
			   LSA_UINT16 response,
			   TCIP_UPPER_RQB_PTR_TYPE *upper_ptr )
{
	TCIP_LOG_FCT("tcip_callback")
	TCIP_UPPER_RQB_PTR_TYPE  rb;

	TCIP_ASSERT (is_not_null (upper_ptr));
	TCIP_ASSERT (is_not_null (*upper_ptr));

	rb = *upper_ptr;
	*upper_ptr = LSA_NULL;		/* sanity - never use this again */

	/*
	 *  memorize successful channel open
	 */
	if( TCIP_OK == response )
	{
		if( TCIP_OPC_OPEN_CHANNEL == TCIP_RQB_GET_OPCODE(rb))
		{
			tcip_data.tcip_channels_opened ++;
		}
	}

	TCIP_RQB_SET_HANDLE (rb, channel->handle_upper);
	TCIP_RQB_SET_RESPONSE (rb, response);

	TCIP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u) f-h(%u)"
		, rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb), TCIP_RQB_GET_RESPONSE (rb), channel->my_handle
		);

	TCIP_REQUEST_UPPER_DONE (channel->callback, rb, channel->sysptr);
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
//	func: tcip_callback_and_free_channel: release all channel specific memory
//----------------------------------------------------------------------------*/

LSA_VOID tcip_callback_and_free_channel(
	TCIP_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 response,
	TCIP_UPPER_RQB_PTR_TYPE rb
) {
	TCIP_LOG_FCT ("tcip_callback_and_free_channel")

	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE  callback = channel->callback;
	TCIP_DETAIL_PTR_TYPE  detailptr = channel->detailptr;
	LSA_SYS_PTR_TYPE  sysptr = channel->sysptr;

	if (tcip_data.tcip_channels_opened > 0)
	{
		tcip_data.tcip_channels_opened --;
	}
	else
	{
		TCIP_FATAL();
	}

	/*
	 * NOTE: the previously used sequence was wrong!
	 *       > tcip_callback(channel, TCIP_OK, &rb);
	 *       > channel->state = TCIP_CST_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to tcip_undo_init() can occur (due to preemption)
	 *     before we have the chance to reset the in-use flag
	 */

	TCIP_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< rqb(0x%08x) h(%u) opc(%u) rsp(%u) f-h(%u) channel-free"
		, rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb), TCIP_RQB_GET_RESPONSE (rb), channel->my_handle
		);


	TCIP_RQB_SET_HANDLE (rb, channel->handle_upper);
	TCIP_RQB_SET_RESPONSE (rb, response);

#if TCIP_DEBUG
	TCIP_MEMSET(channel, 0xDD, sizeof(*channel)); /* wipe out */
#endif

	channel->state = TCIP_CH_FREE;				/* free the channel ... */

	TCIP_REQUEST_UPPER_DONE (callback, rb, sysptr); /* ... before calling back! */

	{
	LSA_UINT16  rc;
	TCIP_RELEASE_PATH_INFO (& rc, sysptr, detailptr);
	TCIP_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	get channel from handle or LSA_NULL if out-of-range
 */
TCIP_CHANNEL_PTR_TYPE tcip_channel_from_handle( LSA_HANDLE_TYPE handle)
{
	TCIP_LOG_FCT ("tcip_channel_from_handle")

	if( handle < TCIP_MAX_CHANNELS )
	{
		/* see tcip_open_channel() */
		return &tcip_data.tcip_channels[handle];
	}

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "handle(%u) out of range", handle);

	return LSA_NULL;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	 returns arp channel pointer with passed group if found, otherwise LSA_NULL
 *   group id value must match, value itself not important
 */
static TCIP_CHANNEL_PTR_TYPE tcip_find_arp_channel_opened(LSA_UINT8 p_group_id )
{
	TCIP_CHANNEL_PTR_TYPE pChRet = LSA_NULL;
	LSA_HANDLE_TYPE hCh;

	for( hCh = 0; hCh < TCIP_MAX_CHANNELS; hCh++)
	{
		TCIP_CHANNEL_PTR_TYPE pCh = &tcip_data.tcip_channels[hCh];

		if( TCIP_CH_OPEN != pCh->state )
		{
			continue;
		}

		if( ( TCIP_PATH_TYPE_ARP == pCh->detailptr->path_type) &&
			( p_group_id == pCh->detailptr->group_id ) )
		{
			pChRet = pCh;
			break;
		}
	}
	return pChRet;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  returns opt (ICMP, UDP or TCP) channel pointer with passed group id if found, otherwise LSA_NULL
 */
static TCIP_CHANNEL_PTR_TYPE tcip_find_opt_channel_opened(LSA_UINT8 p_group_id)
{
	TCIP_CHANNEL_PTR_TYPE pChRet = LSA_NULL;
	LSA_HANDLE_TYPE hCh;

	for( hCh = 0; hCh < TCIP_MAX_CHANNELS; hCh++)
	{
		TCIP_CHANNEL_PTR_TYPE pCh = &tcip_data.tcip_channels[hCh];
		LSA_UINT8 ch_path_type;

		if( TCIP_CH_OPEN != pCh->state )
		{
			continue;
		}

		ch_path_type = pCh->detailptr->path_type;

		if( ( p_group_id == pCh->detailptr->group_id  )
			&& ( TCIP_PATH_TYPE_ICMP == ch_path_type
				|| TCIP_PATH_TYPE_UDP == ch_path_type
				|| TCIP_PATH_TYPE_TCP == ch_path_type
				)
		  )

		{
			pChRet = pCh;
			break;
		}
	}
	return pChRet;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize send - and receive resources for one channel
 */
static LSA_VOID tcip_init_send_recv_resources(TCIP_CHANNEL_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_init_send_recv_resources")
	LSA_UINT32 i;
	LSA_UINT32 nr_of_recv = channel->detailptr->number_of_recv_resources;
	LSA_UINT32 nr_of_send = channel->detailptr->number_of_send_resources;

	channel->edd.pending_rcv_reqests = 0;   /* initial state*/

	/*
	 * initialize send resources for channel
	 */

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "providing %u send-resources", nr_of_send);

	tcip_initEddSendRQBList((EDD_UPPER_RQB_PTR_TYPE*)&( channel->edd.send_rqbs),
		               nr_of_send,
					   channel->edd.handle_lower,
					   channel->sysptr);


	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "providing %u recv-resources", nr_of_recv);

	for ( i= 0; i < nr_of_recv; i++ )  /* provide receive resources for channel      */
	{
		tcip_AllocRcvBuf(channel);
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize arp channel: create interfaces and send-recv resources
 */
LSA_VOID tcip_init_arp_channel(TCIP_CHANNEL_CONST_PTR_TYPE channel)
{

	if( 0 == tcip_data.tcip_channels_opened ) /* first (of all) ARP channels */
	{

#if TCIP_CFG_INTERNICHE
		tcip_iniche_create_interfaces(channel->p_if_data->nets_count);
#endif

#if TCIP_CFG_OPEN_BSD
		tcip_obsd_create_interfaces(channel->p_if_data->nets_count);
#endif

		/***/

#if TCIP_CFG_INTERNICHE
#if TPS != 10
#error "check TCIP_ALLOC_TIMER(..., 100ms) and TCIP_START_TIMER(..., 1)"
#endif
#endif

#if TCIP_CFG_OPEN_BSD
#if HAL_CLOCK_RATE_HZ != 10
#error "check TCIP_ALLOC_TIMER(..., 100ms) and TCIP_START_TIMER(..., 1)"
#endif
#endif

		{
		LSA_USER_ID_TYPE uUserId;
		LSA_UINT16 retVal;
		uUserId.uvar32 = 0;
		TCIP_START_TIMER(&retVal, tcip_data.tcip_TimerID, uUserId, 1);
		TCIP_ASSERT(LSA_RET_OK == retVal);
		}
	}

	/***/

	channel->edd.pEdd = tcip_AllocRQB(channel->sysptr, sizeof(EDD_RQB_GET_STATISTICS_ALL_TYPE));

	if( is_null(channel->edd.pEdd) )
	{
		TCIP_FATAL(); /* a bug */
	}

	/***/

#if TCIP_CFG_INTERNICHE
	tcip_iniche_activate_interfaces(channel); /* "activate" interfaces */
#endif

#if TCIP_CFG_OPEN_BSD
	tcip_obsd_activate_interfaces(channel);
#endif

	/***/

	tcip_edd_ext_link_status_provide(channel);

	tcip_init_send_recv_resources(channel);
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  initialize optional channels (icmp, udp and tcp):
 *  set interface pointer of arp channel;
 *  set channel pointer in net-structs of interface
 */
LSA_VOID tcip_init_opt_channel(TCIP_CHANNEL_CONST_PTR_TYPE channel)
{
	TCIP_CHANNEL_PTR_TYPE arp_channel = tcip_find_arp_channel_opened(channel->detailptr->group_id);

	if( is_null(arp_channel) )
	{
		TCIP_FATAL();
	}
	else
	{

		/*
		 * set interface struct pointer of arp channel for this channel also
		 */

		channel->p_if_data = arp_channel->p_if_data; /*all channel of one interface have the same pointer*/

		/***/

		tcip_init_send_recv_resources(channel);      /* init send-receive resources for new channel */
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *	deinit arp channel: remove interface entries, release interface struct
 */
static LSA_VOID tcip_deinit_arp_channel(TCIP_CHANNEL_PTR_TYPE channel)
{
#if TCIP_CFG_INTERNICHE
	tcip_iniche_deactivate_interfaces(channel);
#endif

#if TCIP_CFG_OPEN_BSD
	tcip_obsd_deactivate_interfaces(channel);
#endif

	{
	LSA_UINT16 retval;
	TCIP_FREE_LOCAL_MEM(&retval, channel->p_if_data);
	TCIP_ASSERT(LSA_RET_OK == retval);
	}

	channel->p_if_data = LSA_NULL;

	/*
	 *  if last arp channel is closed, stop timer and clear reasm queue
	 */
	if( 1 == tcip_data.tcip_channels_opened )
	{
		{
		LSA_UINT16 retVal;
		TCIP_STOP_TIMER(&retVal, tcip_data.tcip_TimerID);
		TCIP_ASSERT(LSA_RET_OK == retVal);
		}
//TODO: review stopping a cyclic timer, re-read prefix_sys.h

		/***/

#if TCIP_CFG_INTERNICHE
		tcip_iniche_delete_interfaces();
#endif

#if TCIP_CFG_OPEN_BSD
		if (obsd_pnio_delete_all_interfaces() != 0)
		{
			TCIP_FATAL();
		}
#endif
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
// reset channel's interface pointer
 //----------------------------------------------------------------------------*/
static LSA_VOID tcip_deinit_opt_channel(TCIP_CHANNEL_PTR_TYPE channel)
{
	/*
	 * set interface struct pointer to zero
	 */
	channel->p_if_data = LSA_NULL;
}

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
//	create a tcip instance and open a channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID tcip_open_channel ( TCIP_UPPER_RQB_PTR_TYPE  rb )
{
	TCIP_LOG_FCT("tcip_open_channel")

	TCIP_UPPER_OPEN_CHANNEL_PTR_TYPE open;
	TCIP_CHANNEL_PTR_TYPE channel;

	/* sanity checks */

	if (is_null (rb) || TCIP_RQB_GET_OPCODE (rb) != TCIP_OPC_OPEN_CHANNEL)
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "no rqb(%08X) or wrong opcode or no args", rb);
		TCIP_FATAL(); /*lint -unreachable */
		return;
	}

#if (TCIP_DEBUG == 1)
	TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	open = &rb->args.open; /* shortcut */

	open->handle = TCIP_INVALID_HANDLE; /* sanity */

	if (open->tcip_request_upper_done_ptr == 0) /* AP01382171: dont' use macro is_null() */
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "no callback function given" );
		TCIP_FATAL(); /*lint -unreachable */
		return;
	}

	/* get path info and search a free tcip-handle*/

	channel = LSA_NULL;

	{
	TCIP_DETAIL_PTR_TYPE detailptr = LSA_NULL;
	LSA_SYS_PTR_TYPE sysptr;
	LSA_UINT16 rc;
	LSA_HANDLE_TYPE my_handle;
	LSA_RESPONSE_TYPE response;

	TCIP_GET_PATH_INFO (&rc, &sysptr, &detailptr, open->sys_path);

	if (rc != LSA_RET_OK || is_null (detailptr))
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "tcip_get_path_info: rc(%u) detailptr(%08x)", rc, detailptr);
		TCIP_FATAL(); /*lint -unreachable */
		return;
	}

	for (my_handle = 0; my_handle < TCIP_MAX_CHANNELS; ++my_handle)
	{
		channel = &tcip_data.tcip_channels[my_handle];
		if (channel->state == TCIP_CH_FREE) {
			#if TCIP_DEBUG
				TCIP_MEMSET(channel, 0xCC, sizeof(*channel)); /* to find uninitialized things */
				channel->state = TCIP_CH_FREE;
			#endif
			break; /* found */
		}
	}

	if ( my_handle >= TCIP_MAX_CHANNELS || is_null(channel) /* sanity */ )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "no free channel found, TCIP_MAX_CHANNELS(%u)", TCIP_MAX_CHANNELS );

		TCIP_RQB_SET_RESPONSE(rb, TCIP_ERR_RESOURCE);
		TCIP_RQB_SET_HANDLE(rb, open->handle_upper);
		TCIP_REQUEST_UPPER_DONE(open->tcip_request_upper_done_ptr, rb, sysptr);
		TCIP_RELEASE_PATH_INFO(&rc, sysptr, detailptr);
		TCIP_ASSERT(rc == LSA_RET_OK);
	}
	else if (detailptr->number_of_recv_resources == 0) /* receiving over all channels */
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "number_of_recv_resources 0 on path_type(%d), sys_path(%d)", detailptr->path_type, open->sys_path);

		TCIP_RQB_SET_RESPONSE(rb, TCIP_ERR_SYS_PATH);
		TCIP_RQB_SET_HANDLE(rb, open->handle_upper);
		TCIP_REQUEST_UPPER_DONE(open->tcip_request_upper_done_ptr, rb, sysptr);
		TCIP_RELEASE_PATH_INFO(&rc, sysptr, detailptr);
		TCIP_ASSERT(rc == LSA_RET_OK);
	}
	else
	{
		TCIP_CHANNEL_PTR_TYPE pChannelArp = tcip_find_arp_channel_opened(detailptr->group_id);  /*look for open ARP channel with group id of channel to be opened*/

		if (TCIP_PATH_TYPE_ARP == detailptr->path_type) /* ARP channel */
		{
			if (is_not_null(pChannelArp))
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "open ARP channel with same group_id(%d) found, sys_path(%d)", detailptr->group_id, open->sys_path);
				response = TCIP_ERR_SEQUENCE;
			}
			else if (detailptr->number_of_send_resources == 0)
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "number_of_send_resources must not be 0 for ARP channel, sys_path(%d)", open->sys_path);
				response = TCIP_ERR_SYS_PATH;
			}
			else
			{
				response = TCIP_OK;
			}
		}
		else /* ICMP, UDP, TCP channel */
		{
			if (is_null(pChannelArp)) /* ARP-channel must be opened first for channel group, otherwise sequence error */
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "no open ARP channel with group_id(%d) found, sys_path(%d)", detailptr->group_id, open->sys_path);
				response = TCIP_ERR_SEQUENCE;
			}
			else if (detailptr->number_of_send_resources != 0) /* no sending over ICMP, UDP and TCP channel */
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "number_of_send_resources not 0 on path_type(%d), sys_path(%d)", detailptr->path_type, open->sys_path);
				response = TCIP_ERR_SYS_PATH;
			}
			else
			{
				response = TCIP_OK;
			}
		}

		if (response != TCIP_OK)
		{
			TCIP_RQB_SET_RESPONSE (rb, response);
			TCIP_RQB_SET_HANDLE (rb, open->handle_upper);
			TCIP_REQUEST_UPPER_DONE (open->tcip_request_upper_done_ptr, rb, sysptr);
			TCIP_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
			TCIP_ASSERT (rc == LSA_RET_OK);
			return;
		}

		channel->state			= TCIP_CH_ALLOCATED;
		channel->my_handle		= my_handle;
		channel->handle_upper	= open->handle_upper;
		channel->syspath        = open->sys_path;
		channel->callback		= open->tcip_request_upper_done_ptr;
		channel->sysptr			= sysptr;
		channel->detailptr		= detailptr;
		channel->trace_idx		= 0; /* sanity, see below */
		channel->p_if_data      = LSA_NULL;

		/*
		 *   init lower data
		 */
		channel->edd.handle_lower = LSA_NULL;
		channel->edd.pending_rcv_reqests = 0;
		channel->edd.in_work_rcv_rqbs_number = 0;
#if TCIP_CFG_INTERNICHE /* Task 1560385 (use if_sendq) -- TCIP-EDD-Send-Queue only for Interniche */
		channel->edd.send_pkts.first_out  = LSA_NULL;
		channel->edd.send_pkts.last_pkt   = LSA_NULL;
		channel->edd.send_pkts.cur_len = 0;
		channel->edd.send_pkts.max_len = 0; /* see tcip_edd_get_params_done() */
#endif
		channel->edd.send_rqbs  = LSA_NULL;
		channel->edd.get_statistics_pending = LSA_FALSE;
		channel->edd.pEdd = LSA_NULL;

		TCIP_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "tcip_open_channel: h(%u) group_id(%u) path_type(%u)"
			, channel->my_handle, channel->detailptr->group_id, channel->detailptr->path_type
			);

#if TCIP_INT_CFG_TRACE_ON
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: TCIP channel.details for channel:%d", channel->my_handle);
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:                         group_id:%d", channel->detailptr->group_id);
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:                        path_type:%d", channel->detailptr->path_type);
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:     number_of_recv_resources:%5d", channel->detailptr->number_of_recv_resources);
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation:     number_of_send_resources:%5d", channel->detailptr->number_of_send_resources);
#ifdef PNTEST_BUILD_WINDOWS
		printf("instrumentation: TCIP channel.details for channel:%d\n", channel->my_handle);
		printf("instrumentation:                         group_id:%d\n", channel->detailptr->group_id);
		printf("instrumentation:                        path_type:%d\n", channel->detailptr->path_type);
		printf("instrumentation:     number_of_recv_resources:%5d\n", channel->detailptr->number_of_recv_resources);
		printf("instrumentation:     number_of_send_resources:%5d\n", channel->detailptr->number_of_send_resources);
#endif
#endif

		/***/

		open->handle = channel->my_handle;

		channel->curr = rb;

		/*
		 * open the lower layer
		 */

		switch( channel->detailptr->path_type )
		{
		case TCIP_PATH_TYPE_ARP:
		case TCIP_PATH_TYPE_ICMP:
		case TCIP_PATH_TYPE_UDP:
		case TCIP_PATH_TYPE_TCP:
			tcip_open_channel_all(channel);
			break;

		default:
			TCIP_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "wrong path_type(%u)", channel->detailptr->path_type );

			tcip_callback_and_free_channel (channel, TCIP_ERR_SYS_PATH, channel->curr);
			break;
		}
	}
	}
}


/*------------------------------------------------------------------------------
//	close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID
tcip_close_channel (
	TCIP_UPPER_RQB_PTR_TYPE  rb
) {
	TCIP_LOG_FCT("tcip_close_channel")
	TCIP_CHANNEL_PTR_TYPE channel;

	/* sanity checks */

	TCIP_ASSERT (is_not_null (rb));

	channel = is_null(rb) ? LSA_NULL : tcip_channel_from_handle(TCIP_RQB_GET_HANDLE(rb));

	if (is_null(channel))
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "no rqb(%08X) or invalid handle(%u)", rb, (is_null(rb) ? 0 : TCIP_RQB_GET_HANDLE(rb)) );
		TCIP_FATAL(); /*lint -unreachable */
		return;
	}

#if TCIP_DEBUG
	TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	TCIP_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> rqb(0x%08x) h(%u) opc(%u)",
			rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE (rb)
		);

	if ( ( TCIP_RQB_GET_OPCODE (rb) != TCIP_OPC_CLOSE_CHANNEL ) ||
		 ( channel->state != TCIP_CH_OPEN )  )
	{
		TCIP_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
			"h(%u) wrong opcode or state(%u) not ready",
			channel->my_handle, channel->state );
		tcip_callback (channel, TCIP_ERR_SEQUENCE, & rb);
		return;
	}

	channel->curr  = rb;

	/* sanity check: arp channel is the last one to be closed for one channel group */

	if( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type )
	{
		TCIP_CHANNEL_PTR_TYPE pChannelOpt;

		pChannelOpt = tcip_find_opt_channel_opened(channel->detailptr->group_id);

		if( is_not_null(pChannelOpt) )   /* check: optional channel open for this group id ? --> sequence error */
		{
			TCIP_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
				"arp channel close: opt channel with same group_id(%d) found", channel->detailptr->group_id );
			tcip_callback(channel, TCIP_ERR_SEQUENCE, &channel->curr );
			return;
		}
	}

	/***/

	channel->state = TCIP_CH_CLOSING;

#if TCIP_CFG_INTERNICHE
#ifdef IP_FRAGMENTS
	tcip_iniche_flush_reasm_cache(channel);  /* (!)function disregards channel assignment of packets */
#endif
#endif

#if TCIP_CFG_OPEN_BSD
	TCIP_ASSERT(channel->edd.in_work_rcv_rqbs_number == 0);
	/* OBSD_ITGR -- TODO */
#endif

	/***/

	/* edd channel is closing -->  de-init interface functionality */

	if( TCIP_PATH_TYPE_ARP != channel->detailptr->path_type )
	{
		tcip_deinit_opt_channel(channel);
	}
	else
	{
		tcip_deinit_arp_channel(channel);

#if TCIP_CFG_ENABLE_MULTICAST
		/* remove entry for all hosts group (224.0.0.1 -> MAC: 01-00-5E-00-00-01 */
		{
		LSA_UINT64 address = 0x01005E000001;
		tcip_edd_srv_multicast(channel, 0, address, TCIP_SRV_MULTICAST_DISABLE);
		}
		return; /* not yet nrt_cancel, see tcip_edd_srv_multicast_done() */
#endif
	}

	/* close the lower layer; NOTE: EDD indication resources are cancelled before calling back */

	tcip_edd_nrt_cancel( channel );
}


/*------------------------------------------------------------------------------
//	receives a request from the user, dispatches it to the subsystem
//----------------------------------------------------------------------------*/
LSA_VOID
tcip_request (
	TCIP_UPPER_RQB_PTR_TYPE  rb
) {
	TCIP_LOG_FCT("tcip_request")

	if( is_null(rb) )
	{
		TCIP_FATAL();
	}
	else
	{
#if TCIP_DEBUG
		TCIP_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
		TCIP_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

		if( TCIP_RQB_GET_OPCODE(rb) == TCIP_OPC_TIMER )
		{
			TCIP_EXCHANGE_LONG(&rb->args.timer.in_use, 0);

#if TCIP_CFG_INTERNICHE
			tcip_iniche_ctick();
#endif

#if TCIP_CFG_OPEN_BSD
			tcip_obsd_ctick();

#if TCIP_INT_CFG_SOCKET_EVENTS
			TCIP_DO_SOCKET_EVENTS(); /* ETIMEDOUT, see 1412688 */
#endif

#if TCIP_CFG_SNMP_ON
		obsd_snmpd_handle_events(0, LSA_NULL); /* for routing messages */
#endif

#endif
		}
		else
		{
			TCIP_CHANNEL_PTR_TYPE channel = tcip_channel_from_handle(TCIP_RQB_GET_HANDLE(rb));

			if( is_null(channel) )
			{
				TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
					"wrong handle(%u), opc(%u)",
					TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE(rb)
					);

				TCIP_FATAL();
			}
			else
			{
				TCIP_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
					">>> rqb(0x%08x) h(%u) opc(%u)",
					rb, TCIP_RQB_GET_HANDLE (rb), TCIP_RQB_GET_OPCODE(rb)
					);

				TCIP_FATAL(); /* TCIP has no other opcodes */
			}
		}
	}
}

#if TCIP_INT_CFG_SOCKET_EVENTS

static TCIP_SOCKET_EVENT_TYPE obsd_event_list[TCIP_CFG_MAX_SOCKETS];
static int obsd_event_list_size = 0;

#if TCIP_INT_CFG_TRACE_ON
extern int event_list_size_max;
#endif

/*------------------------------------------------------------------------------
// examine event list, call handler indicated by flag
//----------------------------------------------------------------------------*/
LSA_VOID
TCIP_DO_SOCKET_EVENTS(
	LSA_VOID
) {

#if TCIP_INT_CFG_TRACE_ON
	if (obsd_event_list_size > event_list_size_max) event_list_size_max = obsd_event_list_size;
#endif

	while (obsd_event_list_size > 0)
	{
		void *obsd_socket = LSA_NULL;

		obsd_event_list_size --;

		obsd_socket = obsd_event_list[obsd_event_list_size].sock_so;

		if (obsd_socket != LSA_NULL)
		{
			if (obsd_event_list[obsd_event_list_size].flags & TCIP_READ_FLAG)
			{
				obsd_event_list[obsd_event_list_size].flags  &= ~TCIP_READ_FLAG;
				obsd_execute_callback(obsd_socket, TCIP_READ_FLAG);
			}

			if (obsd_event_list[obsd_event_list_size].flags & TCIP_WRITE_FLAG)
			{
				obsd_event_list[obsd_event_list_size].flags &= ~TCIP_WRITE_FLAG;
				obsd_execute_callback(obsd_socket, TCIP_WRITE_FLAG);
			}

			if (obsd_event_list[obsd_event_list_size].flags & TCIP_CONNECT_FLAG)
			{
				obsd_event_list[obsd_event_list_size].flags &= ~TCIP_CONNECT_FLAG;
				obsd_execute_callback(obsd_socket, TCIP_CONNECT_FLAG);
			}

			obsd_event_list[obsd_event_list_size].sock_so = LSA_NULL;
			obsd_event_list[obsd_event_list_size].flags = 0;
			obsd_set_event_list_index(obsd_socket, -1); /* indicates removal from event list */
		}
	}

	/* list is empty now */
}


/*-----------------------------------------------------------------------------
// add socket to list of sockets to be examined, store flag read|write|connect
//----------------------------------------------------------------------------*/
LSA_VOID
TCIP_ADD_SOCKET_EVENT(
	void* sock_so,
	LSA_UINT8 flags
) {
	if (sock_so != LSA_NULL)
	{
		int event_list_index = obsd_get_event_list_index(sock_so);

		if (event_list_index >= 0) /* socket is in event list */
		{
			obsd_event_list[event_list_index].flags |= flags;
		}
		else /* insert if not in event list */
		{
			obsd_event_list[obsd_event_list_size].sock_so = sock_so;
			obsd_event_list[obsd_event_list_size].flags = flags; /* READ, WRITE, CONNECT */
			obsd_set_event_list_index(sock_so, obsd_event_list_size); /* see TCIP_DO_SOCKET_EVENTS and sock_socket_close also */
			obsd_event_list_size ++;
		}
	}
}
#endif


/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       sets the ip address for a network interface
 * documentation:  LSA_TCIP_Detailspec.doc
 */
#if TCIP_CFG_INTERNICHE
LSA_UINT8
tcip_set_ip_suite(
    LSA_UINT32  interface_id,
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask
)
{
	int i;

	/* OBSD_ITGR -- TODO: put legacy-coding logic here! leave it in ni_set_ipsuite(), just comment it there */

	/*
	 *  find interface with matching interface id
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch;
		TCIP_IF_DATA_PTR_TYPE p_if_data;

		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN != ch->state
			|| ch->detailptr->path_type != TCIP_PATH_TYPE_ARP
			)
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));
		p_if_data = ch->p_if_data;

		/*
		 *  look for matching interface id
		 */
		if( interface_id == p_if_data->edd_interface_id )
		{
			int port;

			for( port=0; port <= p_if_data->port_count; port ++ )
			{
				{
				NET ifp = p_if_data->port[port].ifp;
				int ret = ni_set_ipsuite(ifp, ip_address, subnet_mask, 0 );

				if( ret != 0 )
				{
					TCIP_FATAL1((LSA_UINT32)ret);
				}
				}
			}

			break;
		}
	}

	/***/
	tcip_rebuild_route_table(); /* AP01256605 */
	/***/

	return TCIP_OK;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       sets the geteway and rebuilds the route table
 * documentation:  LSA_TCIP_Detailspec.doc
 */
LSA_UINT8
tcip_set_route(
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask,
	LSA_UINT32	gateway
)
{
	if (ip_address || subnet_mask)
	{
		return TCIP_ERR_PARAM;
	}
	else
	{
		LSA_UINT8 retval = tcip_iniche_set_gateway(gateway);

		tcip_rebuild_route_table(); /* refresh gateway, see 2256680 and AP01256605 */

		return retval;
	}
}

#endif


#if TCIP_CFG_OPEN_BSD
LSA_UINT8
tcip_set_ip_suite(
	LSA_UINT32  interface_id,
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask
)
{
	int i;

	/*
	*  find interface with matching interface id
	*/
	for (i = 0; i<TCIP_MAX_CHANNELS; i++)
	{
		TCIP_CHANNEL_PTR_TYPE ch;
		TCIP_IF_DATA_PTR_TYPE p_if_data;

		ch = &tcip_data.tcip_channels[i];

		if (TCIP_CH_OPEN != ch->state
			|| ch->detailptr->path_type != TCIP_PATH_TYPE_ARP
			)
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));
		p_if_data = ch->p_if_data;

		/*
		*  look for matching interface id
		*/
		if (interface_id == p_if_data->edd_interface_id)
		{
			int port;

			for (port = 0; port <= p_if_data->port_count; port++)
			{
				if (port == 0) /* OBSD_ITGR -- TODO: until SNMP needs it, set only on "port 0 = interface" */
				{
					OBSD_IFNET ifp = p_if_data->port[port].ifp;
					int ret;

					if (ip_address == 0) /* IP-Address 0.0.0.0 means "no address" - e.g. for ResetToFactory */
					{
						ret = obsd_netif_del_ipv4addr(obsd_pnio_get_if_name(ifp));
					}
					else
					{
						ret = obsd_netif_set_ipv4addr(obsd_pnio_get_if_name(ifp), ip_address, subnet_mask);
					}

					if (ret != 0)
					{
						return TCIP_ERR_PARAM; /* not FATAL, see 1868581 */
					}
				}
			}

		}
	}

	return TCIP_OK;
}

LSA_UINT8
tcip_set_route(
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask,
	LSA_UINT32	gateway
)
{
	int ret = 0;

	if (ip_address || subnet_mask)
	{
		return TCIP_ERR_PARAM;
	}

	if (gateway == 0)
	{
		ret = obsd_pnio_del_route(ip_address, subnet_mask);
	}
	else
	{
		/* delete any existing route first */
		ret = obsd_pnio_del_route(ip_address, subnet_mask);
		/* ignore ret (we get an error if we try to delete an none existing route) */
		ret = obsd_pnio_add_route(ip_address, subnet_mask, gateway);
	}

	if (ret != 0)
	{
		return TCIP_ERR_LOWER_LAYER; /* not FATAL, see 1868581 */
	}

	return TCIP_OK;
}
#endif

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_CFG_INTERNICHE

static LSA_VOID
tcip_add_route(
   RTMIB	rtp,        /* route table entry */
   ip_addr  dest,       /* ultimate destination */
   ip_addr  mask,       /* net mask, 0xFFFFFFFF if dest is host address */
   ip_addr  nexthop,    /* where to forward to */
   long     type,       /* ipRouteType; 3=direct 4=indirect  */
   NET      ifp
) {
   rtp->ifp = ifp;
   rtp->ipRouteDest = dest;
   rtp->ipRouteIfIndex = (long)(if_netnumber(ifp)) + 1; /* set interface number */
   rtp->ipRouteMetric1 = (long)ip_mib.ipDefaultTTL;
   rtp->ipRouteMetric2 = -1;
   rtp->ipRouteMetric3 = -1;
   rtp->ipRouteMetric4 = -1;
   rtp->ipRouteNextHop = nexthop;
   rtp->ipRouteProto = IPRP_LOCAL;                 /* icmp, or whatever */
   rtp->ipRouteType = type;
   rtp->ipRouteAge = cticks;        /* timestamp it */
   rtp->ipRouteMask = mask;
   rtp->ipRouteMetric5 = -1;
}

/*---------------------------------------------------------------------------*/

/*
 *   tcip_rebuild_route_table()
 *   deletes ip route table and rebuilds it
 */
static LSA_VOID
tcip_rebuild_route_table (
	LSA_VOID
 ) {
	int i;
	unsigned int route_index;

	LSA_BOOL gateway_added = LSA_FALSE;

	for(route_index=0; route_index < ipRoutes; route_index++)
	{
		rt_mib[route_index].ipRouteNextHop = 0;
	}

	route_index = 0;

	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			NET ifp = ch->p_if_data->port[0].ifp; /* 0 = the interface */

			if( ifp->n_ipaddr != 0 )
			{
				if( route_index < ipRoutes) /* sanity */
				{
					tcip_add_route(&rt_mib[route_index] /* append route for subnet */
						, (ifp->n_ipaddr & ifp->snmask)
						, ifp->snmask
						, ifp->n_ipaddr
						, 3 /* direct route */
						, ifp
						);

					route_index++;

					if ( ifp->n_defgw != 0 /* add one default gateway if present */
						 && !gateway_added
						 && route_index < ipRoutes /* sanity */
						)
					{
						tcip_add_route(&rt_mib[route_index] /* append route for default gateway, see AP01256605 */
							, 0
							, 0
							, ifp->n_defgw
							, 4 /* indirect route */
							, ifp
							);

						route_index++;

						gateway_added = LSA_TRUE;
					}
				}
			}
		}
	}
}

#endif

/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       returns the mac address for an existing network interface
 * documentation:  LSA_TCIP_Detailspec.doc
 */
LSA_UINT8
tcip_get_any_interface_mac_addr(
    LSA_UINT32* p_interface_id,
	LSA_UINT8*	p_mac_address,
	LSA_UINT16	mac_address_length
)
{
	int i;

	if( is_null(p_interface_id) )
	{
		return TCIP_ERR_PARAM;
	}

	if( is_null(p_mac_address) )
	{
		return TCIP_ERR_PARAM;
	}

	if( mac_address_length != 6 )
	{
		return TCIP_ERR_PARAM;
	}

	/*
	 *  find any interface
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch;

		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN != ch->state )
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));

		*p_interface_id = ch->p_if_data->edd_interface_id;

		TCIP_MEMCPY(p_mac_address, ch->p_if_data->port[0/*!*/].MACAddr.MacAdr, mac_address_length);

		break;
	}

	if( TCIP_MAX_CHANNELS == i )  /* nothing found */
	{
		return TCIP_ERR_PARAM;
	}

	return TCIP_OK;
}

#if TCIP_CFG_ENABLE_MULTICAST

/*---------------------------------------------------
// enable / disable multicast mac in EDDx MAC table
//--------------------------------------------------*/
LSA_RESPONSE_TYPE
tcip_srv_multicast(
	LSA_INT32 dev_handle,
	LSA_UINT32 interface_id,
	LSA_UINT32 mc_address, /* (!) network byte order */
	LSA_UINT8 mode
) {
	TCIP_LOG_FCT("tcip_srv_multicast")

	int i;
	TCIP_CHANNEL_PTR_TYPE ch = LSA_NULL;

	LSA_UINT64 multicast_address = 0x01005E000000;
	LSA_UINT32 mask = 0x007FFFFF;

#if TCIP_CFG_OPEN_BSD
	multicast_address |= (obsd_pnio_htonl(mc_address) & mask);
#else
	multicast_address |= (htonl(mc_address) & mask);
#endif

	/* find EDDx channel for local address */

	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			if (ch->p_if_data->edd_interface_id == interface_id)
			{
				break; /* found */
			}
		}
	}

	if( i == TCIP_MAX_CHANNELS ) /* not found */
	{
		return TCIP_ERR_PARAM;
	}

	/***/

	/* add entry for group specified by user */

	tcip_edd_srv_multicast(ch, dev_handle, multicast_address, mode);

	return TCIP_OK;
}

#if TCIP_CFG_OPEN_BSD

LSA_RESPONSE_TYPE tcip_srv_cluster_ip_config_set(
	LSA_UINT32 InterfaceID,
	LSA_UINT8  ClusterID,
	LSA_UINT32 ClusterIP,
	LSA_UINT32 ClusterSubnetMask
) {
	int i;
	TCIP_CHANNEL_PTR_TYPE ch = LSA_NULL;

	/* find EDDx channel for local address */

	for (i = 0; i<TCIP_MAX_CHANNELS; i++)
	{
		ch = &tcip_data.tcip_channels[i];

		if (TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			if (ch->p_if_data->edd_interface_id == InterfaceID)
			{
				break; /* found */
			}
		}
	}

	if (i == TCIP_MAX_CHANNELS) /* not found */
	{
		return TCIP_ERR_PARAM;
	}

	return (LSA_RESPONSE_TYPE)obsd_pnio_set_cluster_ip((OBSD_IFNET)ch->p_if_data->port[0].ifp, InterfaceID, ClusterIP, ClusterSubnetMask, ClusterID);
}

LSA_RESPONSE_TYPE tcip_srv_cluster_ip_state_set(
	LSA_UINT32 InterfaceID,
	LSA_UINT8 ClusterIPState
) {
	int i;
	TCIP_CHANNEL_PTR_TYPE ch = LSA_NULL;

	/* find EDDx channel for local address */

	for (i = 0; i<TCIP_MAX_CHANNELS; i++)
	{
		ch = &tcip_data.tcip_channels[i];

		if (TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			if (ch->p_if_data->edd_interface_id == InterfaceID)
			{
				break; /* found */
			}
		}
	}

	if (i == TCIP_MAX_CHANNELS) /* not found */
	{
		return TCIP_ERR_PARAM;
	}

	if (obsd_pnio_set_carp_role((OBSD_IFNET)ch->p_if_data->port[0].ifp, ClusterIPState) == -1)
	{
		TCIP_UPPER_TRACE_02(ch->trace_idx, LSA_TRACE_LEVEL_WARN
			, "obsd_pnio_set_carp_role failed for interface %i with native error %i"
			, obsd_pnio_get_if_index((OBSD_IFNET)ch->p_if_data->port[0].ifp), obsd_pnio_get_errno()
		);
		return TCIP_ERR_LOWER_LAYER;
	}
	else
	{
		return TCIP_OK;
	}
}

LSA_UINT8 tcip_srv_cluster_ip_state_get(
	LSA_UINT32 InterfaceID
) {
	int i;
	TCIP_CHANNEL_PTR_TYPE ch = LSA_NULL;
	int retval = -1;

	/* find EDDx channel for local address */

	for (i = 0; i<TCIP_MAX_CHANNELS; i++)
	{
		ch = &tcip_data.tcip_channels[i];

		if (TCIP_CH_OPEN == ch->state
			&& ch->detailptr->path_type == TCIP_PATH_TYPE_ARP
			&& is_not_null(ch->p_if_data)
			)
		{
			if (ch->p_if_data->edd_interface_id == InterfaceID)
			{
				break; /* found */
			}
		}
	}

	if (i == TCIP_MAX_CHANNELS) /* not found */
	{
		return TCIP_ERR_PARAM;
	}

	retval = obsd_pnio_get_carp_role((OBSD_IFNET)ch->p_if_data->port[0].ifp);

	switch (retval) {
	case 1:
		return TCIP_CLUSTER_IP_STATE_ACTIVE;
	case 0:
		return TCIP_CLUSTER_IP_STATE_INACTIVE;
	default:
		TCIP_UPPER_TRACE_02(ch->trace_idx, LSA_TRACE_LEVEL_WARN
			, "obsd_pnio_get_carp_role failed for interface %i with native error %i"
			, obsd_pnio_get_if_index((OBSD_IFNET)ch->p_if_data->port[0].ifp), obsd_pnio_get_errno());

		return TCIP_CLUSTER_IP_STATE_INACTIVE;
	}

}
#endif /* TCIP_CFG_OPEN_BSD */

#endif

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_CFG_OPEN_BSD
/*------------------------------------------------------------------------------
// obsd_pnio_add_socket_read_event
// adds 'readable' event entry for socket 'so_' to event list
//----------------------------------------------------------------------------*/
void obsd_pnio_add_socket_read_event(void *so_)
{
#if TCIP_INT_CFG_SOCKET_EVENTS
	if (so_) /* sanity */
	{
		if (obsd_has_callback_fct(so_)) /* is there a context ? */
		{
			TCIP_ADD_SOCKET_EVENT(so_, TCIP_READ_FLAG);
		}
	}
#else
	LSA_UNUSED_ARG(so_);
#endif
}

/*------------------------------------------------------------------------------
// obsd_pnio_add_socket_write_event
// adds 'writable' event entry for socket 'so_' to event list
//----------------------------------------------------------------------------*/
void obsd_pnio_add_socket_write_event(void *so_)
{
#if TCIP_INT_CFG_SOCKET_EVENTS
	if (so_) /* sanity */
	{
		if (obsd_has_callback_fct(so_)) /* is there a context ? */
		{
			TCIP_ADD_SOCKET_EVENT(so_, TCIP_WRITE_FLAG);
		}
	}
#else
	LSA_UNUSED_ARG(so_);
#endif
}

/*------------------------------------------------------------------------------
// obsd_pnio_add_socket_connect_event
// adds 'connect' event entry for socket 'so_' to event list
//----------------------------------------------------------------------------*/
void obsd_pnio_add_socket_connect_event(void *so_)
{
#if TCIP_INT_CFG_SOCKET_EVENTS
	if (so_) /* sanity */
	{
		if (obsd_has_callback_fct(so_))
		{
			TCIP_ADD_SOCKET_EVENT(so_, TCIP_CONNECT_FLAG); /* is there a context ? */
		}
	}
#else
	LSA_UNUSED_ARG(so_);
#endif
}

/*------------------------------------------------------------------------------
// obsd_pnio_remove_from_event_list
// invalidates event list entry for a socket
//----------------------------------------------------------------------------*/
void obsd_pnio_remove_from_event_list(int list_index, const void* socket)
{
#if TCIP_INT_CFG_SOCKET_EVENTS
	if (list_index < obsd_event_list_size /* sanity */
		&& list_index >= 0 /* sanity */
		)
	{
		if (socket == obsd_event_list[list_index].sock_so) /* sanity */
		{
			obsd_event_list[list_index].sock_so = LSA_NULL;
			obsd_event_list[list_index].flags = 0;
		}
	}
#else
	LSA_UNUSED_ARG(list_index);
	LSA_UNUSED_ARG(socket);
#endif
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
