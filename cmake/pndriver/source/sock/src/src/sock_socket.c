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
/*  F i l e               &F: sock_socket.c                             :F&  */
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

#define LTRC_ACT_MODUL_ID 12
#define SOCK_MODULE_ID    12

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*===========================================================================*/
#if !SOCK_INT_CFG_SOCKET_EVENTS
static LSA_VOID sock_socket_handle_except_mask(SOCK_SOCKET_PTR soc);
static LSA_VOID sock_socket_handle_read_mask(SOCK_SOCKET_PTR soc);
static LSA_VOID sock_socket_handle_write_mask(SOCK_SOCKET_PTR soc);
static LSA_VOID sock_socket_peek_connection(SOCK_SOCKET_PTR soc);
#else
static LSA_VOID sock_socket_do_connected_event(SOCK_SOCKET_PTR soc);
void sock_socket_do_events(int flag, void* soc);
#endif
static LSA_VOID sock_socket_handle_connect_error(SOCK_SOCKET_PTR soc, LSA_INT sock_error);

#if TCIP_INT_CFG_TRACE_ON

int event_list_size_max = 0;

typedef struct select_stats_type_tag
{
	LSA_UINT64 d_1_2_min;
	LSA_UINT64 d_1_2_max;
	LSA_UINT64 d_1_2_mean;

	LSA_UINT64 d_2_3_min;
	LSA_UINT64 d_2_3_max;
	LSA_UINT64 d_2_3_mean;

	LSA_UINT64 d_3_4_min;
	LSA_UINT64 d_3_4_max;
	LSA_UINT64 d_3_4_mean;

	LSA_UINT64 d_all_min;
	LSA_UINT64 d_all_max;
	LSA_UINT64 d_all_mean;
} select_stats_type;

static select_stats_type select_stat;

/* select cycle */
static controller_measure_struct controller_select_measure_1;
static controller_measure_struct controller_select_measure_2;
static controller_measure_struct controller_select_measure_3;
static controller_measure_struct controller_select_measure_4;

static void controller_add_select_measure_1();
static void controller_add_select_measure_2();
static void controller_add_select_measure_3();
static void controller_add_select_measure_4();

static void controller_calc_select_stats(select_stats_type* p_stats);
static void controller_print_select_stats(select_stats_type* p_stats);

static int g_select_measures_initialized = 0;
static int g_controller_select_count = 0;

static void init_select_measures();

void init_select_measures()
{
	int i;

	g_controller_select_count = 0;

	for (i=0; i<CONTROLLER_MAX_MEASURES; i++)
	{
		controller_select_measure_1.m[i] = 0;
		controller_select_measure_2.m[i] = 0;
		controller_select_measure_3.m[i] = 0;
		controller_select_measure_4.m[i] = 0;
	}
}

void controller_add_select_measure_1()
{
	controller_select_measure_1.m[g_controller_select_count] = controller_get_time();
}

void controller_add_select_measure_2()
{
	controller_select_measure_2.m[g_controller_select_count] = controller_get_time();
}

void controller_add_select_measure_3()
{
	controller_select_measure_3.m[g_controller_select_count] = controller_get_time();
}

void controller_add_select_measure_4()
{
	controller_select_measure_4.m[g_controller_select_count] = controller_get_time();

	g_controller_select_count++;

	if (g_controller_select_count == CONTROLLER_MAX_MEASURES)
	{
		controller_calc_select_stats(&select_stat);

		controller_print_select_stats(&select_stat);

		init_select_measures();
	}
}

void controller_calc_select_stats(select_stats_type* p_stats)
{
	int i;

	LSA_UINT64 d_1_2_sum = 0;
	LSA_UINT64 d_2_3_sum = 0;
	LSA_UINT64 d_3_4_sum = 0;
	LSA_UINT64 d_all_sum = 0;

	/* reset delta min, max and mean vlaues */
	p_stats->d_1_2_min  = 0xffffffff; /* 32 Bit UINT MAX should be sufficent */
	p_stats->d_1_2_max  = 0;
	p_stats->d_1_2_mean = 0;
	/* delta between point 2 and 3 */
	p_stats->d_2_3_min = 0xffffffff;
	p_stats->d_2_3_max  = 0;
	p_stats->d_2_3_mean = 0;
	/* delta between point 3 and 4 */
	p_stats->d_3_4_min  = 0xffffffff;
	p_stats->d_3_4_max  = 0;
	p_stats->d_3_4_mean = 0;
	/* delta between point 1 and 4 */
	p_stats->d_all_min  = 0xffffffff;
	p_stats->d_all_max  = 0;
	p_stats->d_all_mean = 0;

	/* first step: differences an mean values for measure1 to measure 5 (all data available) */
	for (i=0; i<g_controller_select_count; i++)
	{
		LSA_UINT64 d_1_2 = 0;
		LSA_UINT64 d_2_3 = 0;
		LSA_UINT64 d_3_4 = 0;
		LSA_UINT64 d_all = 0;

		d_1_2  = controller_select_measure_2.m[i] - controller_select_measure_1.m[i];
		d_2_3  = controller_select_measure_3.m[i] - controller_select_measure_2.m[i];
		d_3_4  = controller_select_measure_4.m[i] - controller_select_measure_3.m[i];
		d_all  = controller_select_measure_4.m[i] - controller_select_measure_1.m[i];

		if (d_1_2) CONTROLLER_MIN(p_stats->d_1_2_min, d_1_2)
		CONTROLLER_MAX(p_stats->d_1_2_max, d_1_2)

		if (d_2_3) CONTROLLER_MIN(p_stats->d_2_3_min, d_2_3)
		CONTROLLER_MAX(p_stats->d_2_3_max, d_2_3)

		if (d_3_4) CONTROLLER_MIN(p_stats->d_3_4_min, d_3_4)
		CONTROLLER_MAX(p_stats->d_3_4_max, d_3_4)

		if (d_all) CONTROLLER_MIN(p_stats->d_all_min, d_all)
		CONTROLLER_MAX(p_stats->d_all_max, d_all)

		d_1_2_sum += d_1_2;
		d_2_3_sum += d_2_3;
		d_3_4_sum += d_3_4;
		d_all_sum += d_all;
	}

	/* calculate mean values */
	p_stats->d_1_2_mean = d_1_2_sum / g_controller_select_count;
	p_stats->d_2_3_mean = d_2_3_sum / g_controller_select_count;
	p_stats->d_3_4_mean = d_3_4_sum / g_controller_select_count;
	p_stats->d_all_mean = d_all_sum / g_controller_select_count;
}

void controller_print_select_stats(select_stats_type* p_stats)
{
	SOCK_LOG_FCT("controller_print_select_stats")

	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: select statistic:               g_controller_select_count:%8u timestamps", g_controller_select_count);

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: select statistic:  1->2 Min: %8u us,  1->2 Max: %8u us,  1->2 Mean: %8u us"
		, (LSA_UINT32)p_stats->d_1_2_min, (LSA_UINT32)p_stats->d_1_2_max, (LSA_UINT32)p_stats->d_1_2_mean
		);

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: select statistic:  2->3 Min: %8u us,  2->3 Max: %8u us,  2->3 Mean: %8u us"
		, (LSA_UINT32)p_stats->d_2_3_min, (LSA_UINT32)p_stats->d_2_3_max, (LSA_UINT32)p_stats->d_2_3_mean
		);

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: select statistic:  3->4 Min: %8u us,  3->4 Max: %8u us,  3->4 Mean: %8u us"
		, (LSA_UINT32)p_stats->d_3_4_min, (LSA_UINT32)p_stats->d_3_4_max, (LSA_UINT32)p_stats->d_3_4_mean
		);

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: select statistic: Total Min: %8u us, Total Max: %8u us, Total Mean: %8u us"
		, (LSA_UINT32)p_stats->d_all_min, (LSA_UINT32)p_stats->d_all_max, (LSA_UINT32)p_stats->d_all_mean
		);

#ifdef PNTEST_BUILD_WINDOWS

	printf("instrumentation: select statistic:               g_controller_select_count:%8u timestamps\n", g_controller_select_count);

	printf("instrumentation: select statistic:  1->2 Min:%8llu us,   1->2 Max:%8llu us,   1->2 Mean:%8llu us\n"
			, p_stats->d_1_2_min, p_stats->d_1_2_max, p_stats->d_1_2_mean
			);

	printf("instrumentation: select statistic:  2->3 Min:%8llu us,   2->3 Max:%8llu us,   2->3 Mean:%8llu us\n"
			, p_stats->d_2_3_min, p_stats->d_2_3_max, p_stats->d_2_3_mean
			);

	printf("instrumentation: select statistic:  3->4 Min:%8llu us,   3->4 Max:%8llu us,   3->4 Mean:%8llu us\n"
			, p_stats->d_3_4_min, p_stats->d_3_4_max, p_stats->d_3_4_mean
			);

	printf("instrumentation: select statistic: Total Min:%8llu us,  Total Max:%8llu us,  Total Mean:%8llu us\n"
			, p_stats->d_all_min, p_stats->d_all_max, p_stats->d_all_mean
			);

#endif
}

/* user send measures */

typedef struct send_stats_type_tag
{
	LSA_UINT64 d_1_2_min;
	LSA_UINT64 d_1_2_max;
	LSA_UINT64 d_1_2_mean;

	LSA_UINT64 d_all_min;
	LSA_UINT64 d_all_max;
	LSA_UINT64 d_all_mean;
} send_stats_type;

static send_stats_type send_stat;

/* send request cycle */
static controller_measure_struct controller_send_measure_1;
static controller_measure_struct controller_send_measure_2;

static void init_send_measures();

static void controller_calc_send_stats(send_stats_type* p_stats);
static void controller_print_send_stats(send_stats_type* p_stats);

static int g_controller_record_sends = 1;


void init_send_measures()
{
	int i;

	g_controller_record_sends = 1;

	controller_send_measure_1.count = 0;
	controller_send_measure_2.count = 0;

	for (i=0; i<CONTROLLER_MAX_MEASURES; i++)
	{
		controller_send_measure_1.m[i] = 0;
		controller_send_measure_2.m[i] = 0;
	}
}

void controller_add_user_send_measure_1(LSA_VOID_PTR_TYPE rqb)
{
	static int g_send_measures_initialized = 0;

	SOCK_RQB_TYPE* pRQB = (SOCK_RQB_TYPE*)rqb;

	if (!g_send_measures_initialized)
	{
		init_send_measures();
		g_send_measures_initialized = 1;
	}

	if (!g_controller_record_sends) {
		return;
	}

	controller_send_measure_1.m[controller_send_measure_1.count] = controller_get_time();

	pRQB->packet_counter = controller_send_measure_1.count;

	controller_send_measure_1.count++;

	if (controller_send_measure_1.count == CONTROLLER_MAX_MEASURES)
	{
		g_controller_record_sends = 0;
	}
}

void controller_add_user_send_measure_2(LSA_VOID_PTR_TYPE rqb)
{
	SOCK_RQB_TYPE* pRQB = (SOCK_RQB_TYPE*)rqb;

	controller_send_measure_2.m[pRQB->packet_counter] = controller_get_time();

	controller_send_measure_2.count++;

	if (controller_send_measure_2.count == CONTROLLER_MAX_MEASURES)
	{
		controller_calc_send_stats(&send_stat);

		controller_print_send_stats(&send_stat);

		init_send_measures();
	}
}

void controller_calc_send_stats(send_stats_type* p_stats)
{
	int i;
	LSA_UINT64 d_1_2_sum = 0;

	/* reset delta min, max and mean vlaues */
	p_stats->d_1_2_min  = 0xffffffff; /* 32 Bit UINT MAX should be sufficent */
	p_stats->d_1_2_max  = 0;
	p_stats->d_1_2_mean = 0;

	/* difference for all available measures */
	for (i=0; i<controller_send_measure_2.count; i++)
	{
		LSA_UINT64 d_1_2 = 0;

		d_1_2  = controller_send_measure_2.m[i] - controller_send_measure_1.m[i];

		if (d_1_2) CONTROLLER_MIN(p_stats->d_1_2_min, d_1_2)
		CONTROLLER_MAX(p_stats->d_1_2_max, d_1_2)

		d_1_2_sum += d_1_2;
	}

	/* calculate mean values */
	p_stats->d_1_2_mean = d_1_2_sum / controller_send_measure_2.count;
}

void controller_print_send_stats(send_stats_type* p_stats)
{
	SOCK_LOG_FCT("controller_print_send_stats")

	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: user send statistic:               controller_send_measure_2.count:%8u timestamps", controller_send_measure_2.count);

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: user send statistic:  1->2 Min: %8u us,  1->2 Max: %8u us,  1->2 Mean: %8u us"
		, (LSA_UINT32)p_stats->d_1_2_min, (LSA_UINT32)p_stats->d_1_2_max, (LSA_UINT32)p_stats->d_1_2_mean
		);

#ifdef PNTEST_BUILD_WINDOWS

	printf("instrumentation: user send statistic:               controller_send_measure_2.count:%8u timestamps\n", controller_send_measure_2.count);

	printf("instrumentation: user send statistic:  1->2 Min:%8llu us,   1->2 Max:%8llu us,   1->2 Mean:%8llu us\n"
			, p_stats->d_1_2_min, p_stats->d_1_2_max, p_stats->d_1_2_mean
			);

#endif
}

#endif

/*------------------------------------------------------------------------------
// allocate socket structure for the specified open socket descriptor
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR
sock_socket_alloc(
	SOCK_CHANNEL_PTR_TYPE channel,
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 type
) {
	SOCK_SOCKET_PTR soc = LSA_NULL;

	LSA_INT option_ret_val = sock_socket_set_options(channel, lower_handle, type);

	if (option_ret_val != SOCK_RSP_OK)
	{
		sock_low_close(lower_handle);
		return LSA_NULL;
	}

	if( sock_data.free_head == SOCK_CFG_MAX_SOCKETS) /* end of free list */
	{
		sock_low_close(lower_handle);
		return LSA_NULL;
	}
	else
	{
		soc = &sock_data.sockets[sock_data.free_head];
		sock_data.free_head = soc->free_next;

		soc->lower_handle = lower_handle;
		soc->type = type;

#if SOCK_INT_CFG_SOCKET_EVENTS
		if (!sock_low_set_custom_option(soc->lower_handle, soc, sock_socket_do_events))
		{
			/* callback function for socket event cannot be set */
			sock_low_close(lower_handle);
			return LSA_NULL;
		}
#else
		/* AP01316092, allow optimization. See also sock_socket_execute_select() */
		sock_data.active_sockets_last++;
		sock_data.active_sockets[sock_data.active_sockets_last] = soc; /* removed in sock_socket_free() */
		soc->active_handle = sock_data.active_sockets_last; /* store index, see sock_socket_free() */
#endif

		soc->state = SOCK_CN_STATE_DISABLED; /* initial state */
#if SOCK_INT_CFG_SOCKET_EVENTS
		soc->server_state = SOCK_CN_STATE_DISABLED; /* initial listening socket state, see 2004942 */
#endif

		soc->channel = channel;

		soc->loc_addr.sin_family = 0;
		soc->loc_addr.sin_port = 0;
		soc->loc_addr.sin_addr.s_addr = 0;
		{
		int i;
		for(i=0; i<8; i++)
		{
			soc->loc_addr.sin_zero[i] = 0;
		}
		}
		soc->rem_addr = soc->loc_addr;

		soc->mcast_interface_id = 0;
		soc->mcast_address = 0;

		soc->abort_resp = 0;

		soc->send.bytes_transferred = 0;
		sock_queue_init(&soc->send.queue);
		soc->send.curr = LSA_NULL;

		soc->recv.bytes_transferred = 0;
		sock_queue_init(&soc->recv.queue);
		soc->recv.curr = LSA_NULL;

		soc->connect_rb = LSA_NULL;
		soc->open_close_rb = LSA_NULL;

		sock_queue_init(&soc->accept.queue);
		soc->accept.curr = LSA_NULL;

		soc->exception_rb = LSA_NULL;

		sock_data.socket_count++;

		return soc;
	}
}

/*------------------------------------------------------------------------------
// free socket structure
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_free(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_free")
#if !SOCK_INT_CFG_SOCKET_EVENTS
	/* AP01316092, allow optimization */
	if (soc->active_handle == sock_data.active_sockets_last) /* last entry */
	{
		 sock_data.active_sockets_last--;
	}
	else if (sock_data.active_sockets_last > 0 ) /* between first and last: overwrite with last entry  */
	{
		sock_data.active_sockets[soc->active_handle] = sock_data.active_sockets[sock_data.active_sockets_last];
		sock_data.active_sockets_last--;
		sock_data.active_sockets[soc->active_handle]->active_handle = soc->active_handle; /* (!) adapt active_handle for moved entry */
	}
	else if (sock_data.active_sockets_last < 0)
	{
		SOCK_FATAL();  /* (!) cannot delete from empty array */
	}
#endif

	soc->channel = LSA_NULL;

	if (soc->lower_handle != SOCK_LOW_INVALID_HANDLE)
	{
		sock_low_close(soc->lower_handle); /* soclose sets callback-function to NULL --> no more events are handled */

		soc->lower_handle = SOCK_LOW_INVALID_HANDLE;
	}

	sock_data.socket_count--;

	soc->free_next = sock_data.free_head;
	sock_data.free_head = soc->dev_handle;

	soc->state = SOCK_CN_STATE_FREE;
}


/*------------------------------------------------------------------------------
// get socket structure pointer from index ("handle")
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR
sock_socket_from_handle(
	LSA_INT32 handle
) {
	if( handle >= SOCK_CFG_MAX_SOCKETS )
	{
		return LSA_NULL;
	}

	if( sock_data.sockets[handle].state == SOCK_CN_STATE_FREE )
	{
		return LSA_NULL;
	}

	return &sock_data.sockets[handle];
}


/*------------------------------------------------------------------------------
// get socket structure pointer from index and check channel assignment
//----------------------------------------------------------------------------*/
SOCK_SOCKET_PTR
sock_socket_from_channel(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	LSA_INT32 handle
) {
	SOCK_SOCKET_PTR soc = sock_socket_from_handle(handle);

	if( sock_is_null(soc))
	{
		return LSA_NULL;
	}
	else if( !sock_are_equal(channel, soc->channel) )
	{
		return LSA_NULL;
	}
	else
	{
		return soc;
	}
}


/*------------------------------------------------------------------------------
// sock_socket_do_select
//----------------------------------------------------------------------------*/
#if !SOCK_INT_CFG_SOCKET_EVENTS
LSA_VOID
sock_socket_do_select(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr
) {
	SOCK_RQB_SET_RESPONSE(rqb_ptr, SOCK_RSP_OK_ACTIVE);

	sock_socket_execute_select(0);
}
#endif


#if SOCK_INT_CFG_SOCKET_EVENTS
/*-----------------------------------------------------------------------------
// treat "readable" event on socket; disconnect makes socket "readable" also
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_do_readable_event(
	SOCK_SOCKET_PTR soc
	) {

	SOCK_LOG_FCT("sock_socket_do_readable_event")

	if (sock_is_null(soc->open_close_rb))
	{
		if (sock_is_not_null(soc->recv.curr))
		{
			sock_socket_receive(soc);
		}
		else if (sock_is_not_null(soc->connect_rb))
		{
			sock_socket_do_connected_event(soc); /* ETIMEDOUT when connecting, see 1442237 */
		}
		else if( soc->type == SOCK_LOW_STREAM
				 && soc->state == SOCK_CN_STATE_CONNECTED
				 )
		{
			char buf;
			LSA_INT result;
			LSA_INT native_error;
			LSA_RESPONSE_TYPE response;

			SOCK_PROTOCOL_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
                , "(!)no read rqb socket lower_handle(0x%08x) handle(%d), -> peek socket and retrieve possible socket error"
                , soc->lower_handle
                , soc->dev_handle
                );

			/* note: remote abort detection will not work if socket has unread data */

			response = sock_low_recv(soc->lower_handle
                                     , &buf
                                     , sizeof(buf)
                                     , SOCK_LOW_PEEK
                                     , &result
                                     , &native_error
                                     );

			if (response != SOCK_RSP_OK_ACTIVE) /* SOCK_RSP_OK or mapped error code */
			{
				if (result == 0) /* FIN */
				{
					SOCK_PROTOCOL_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
                        , "SOCKIF_Recv -> result(%d) for lower_handle(0x%x), handle(%d), graceful shutdown detected, return resources with ERR_REM_ABORT"
                        , result
                        , soc->lower_handle
                        , soc->dev_handle
                        );

					sock_socket_return_all_resources(soc, SOCK_RSP_ERR_REM_ABORT);  /* 'graceful shutdown' */
				}
				else /* <0: RST; >0: unread data present, but still RST possible. therefore, check socket error in both cases */
				{
					int socket_error = 0;
					int errnom = 0;
					LSA_BOOL has_error;

					has_error = sock_low_has_socket_error(soc->lower_handle
                                                          , &socket_error
                                                          , &errnom
                                                          );

					if (has_error) /* not EWOULDBLOCK or EINPROGRESS */
                    {
                        if (errnom) /* getsockopt() failed -> local problem -> ERR_LOC_NET */
                        {
							SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
                                , "TCP socket handle(%d), lower_handle(0x%x): obsd getsockopt(SO_ERROR) failed with errno(%d), return resources with ERR_LOC_NET"
                                , soc->dev_handle
                                , soc->lower_handle
                                , errnom
                                );

                            sock_socket_return_all_resources(soc, SOCK_RSP_ERR_LOC_NET); /* -> CN_STATE_DISCONNECTING */
                        }
                        else /* socket has error */
                        {
                            response = sock_low_map_tcp_error(socket_error);

                            SOCK_LOWER_TRACE_04(0, LSA_TRACE_LEVEL_WARN
                                , "TCP socket handle(%d), lower_handle(0x%x) readable, but socket_error(%d) in socket, return resources with response(%d)"
                                , soc->dev_handle
                                , soc->lower_handle
                                , socket_error
                                , response
                                );

                            sock_socket_return_all_resources(soc, response); /* -> CN_STATE_DISCONNECTING */
                        }
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------
// treat "writeable" event on socket
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_do_writable_event(
	SOCK_SOCKET_PTR soc
	) {
	SOCK_LOG_FCT("sock_socket_do_writeable_event")

	if (sock_is_null(soc->send.curr))
	{
		SOCK_PROTOCOL_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "(!)no write rqb socket lower_handle(0X%x) handle(%d)"
			, soc->lower_handle, soc->dev_handle
			);
	}

	sock_socket_send(soc);

	/***/

	if( sock_is_not_null(soc->open_close_rb) /* pending close active */
		&& sock_is_null(soc->send.curr)
		)
	{
		SOCK_PROTOCOL_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "(!)pending close active for socket lower_handle(0X%x) handle(%d), close socket now"
			, soc->lower_handle, soc->dev_handle
			);

		sock_socket_close(soc);
	}
}


/*-----------------------------------------------------------------------------
// treat "soisconnected" event on socket (obsd_kernel_uipc_socket2.c)
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_do_connected_event(
	SOCK_SOCKET_PTR soc
	) {
	SOCK_LOG_FCT("sock_socket_do_connected_event")

	if (sock_is_not_null(soc->accept.curr)) /* client connect */
	{
		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "listening socket_fd(0x%x) in read mask"
			, soc->lower_handle
			);

		sock_tcp_accept(soc);
	}
	else if (sock_is_not_null(soc->connect_rb))
	{
		int sock_error;
		int native_error;

		sock_low_get_socket_error(soc->lower_handle, &sock_error, &native_error); /* check error condition on socket also, see AP01418351, AP01493449 */

		if (sock_error)
		{
			sock_socket_handle_connect_error(soc, sock_error);
		}
		else
		{
			sock_tcp_connect_client_socket(soc); /* conclude connect */
		}
	}
	/* else do nothing */
}

void sock_socket_do_events(int flag, void* context)
{
	SOCK_SOCKET_PTR soc = (SOCK_SOCKET_PTR) context;

	if (soc->state != SOCK_CN_STATE_DISABLED)
	{
		if (flag & TCIP_READ_FLAG)
		{
			sock_socket_do_readable_event(soc);
		}

		if (flag & TCIP_WRITE_FLAG)
		{
			sock_socket_do_writable_event(soc);
		}

		if (flag & TCIP_CONNECT_FLAG)
		{
			sock_socket_do_connected_event(soc);
		}
	}
}

#endif

#if !SOCK_INT_CFG_SOCKET_EVENTS
/*------------------------------------------------------------------------------
// create read-write-except mask, call select, handle result
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_execute_select(
	LSA_INT32 timeout
) {
	SOCK_LOG_FCT("sock_socket_execute_select")

	LSA_INT32 cnt = 0;

	LSA_INT32 i;
#if TCIP_INT_CFG_TRACE_ON
	if (!g_select_measures_initialized)
	{
		init_select_measures();
		g_select_measures_initialized = 1;
	}
	controller_add_select_measure_1();
#endif

	sock_low_clear_all_sets();

	for (i=0; i<=sock_data.active_sockets_last; i++ )
	{
		SOCK_SOCKET_PTR soc = sock_data.active_sockets[i];

		if ((soc->state & SOCK_CN_STATE_ACTIVE_MASK) != 0) /* connected, connecting, listening */
		{
			if (sock_is_null(soc->open_close_rb))
			{
				if (sock_is_not_null(soc->recv.curr)
				   || (sock_data.do_tcp_disconnect_check /* only if timer has set flag, see AP01325244 */
				       && (soc->type == SOCK_LOW_STREAM)
					   && (soc->state == SOCK_CN_STATE_CONNECTED)
				      ) /* detect TCP RST and FIN, see AP01300966 */
#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD
				   || (sock_are_equal(soc, sock_data.snmp_dev) && (sock_data.snmp_pending_count == 0))
#endif
				   )
				{
					cnt++;
					sock_low_readset_insert(soc->lower_handle);
				}
			}

			if (sock_is_not_null(soc->accept.curr)
				&& (sock_data.socket_count < SOCK_CFG_MAX_SOCKETS)
				)
			{
				cnt++;
				sock_low_readset_insert(soc->lower_handle);
			}

			if(sock_is_not_null(soc->send.curr) /* write mask */
			   || sock_is_not_null(soc->connect_rb)
			   )
			{
				cnt++;
				sock_low_writeset_insert(soc->lower_handle);
			}

			if( (soc->type == SOCK_LOW_STREAM)/*!soc->sock_udp*/
				&& sock_data.do_tcp_disconnect_check
			  )  /* except mask for tcp sockets */
			{
				cnt++;
				sock_low_exceptset_insert(soc->lower_handle);
			}
		}
	}

	if (sock_data.do_tcp_disconnect_check) /* reset flag, see AP01325244 */
	{
		LSA_UINT16 ret;
		LSA_USER_ID_TYPE user_id;

		sock_data.do_tcp_disconnect_check = LSA_FALSE;

		user_id.uvar32 = 0;

		SOCK_START_TIMER(&ret, sock_data.tcp_disconnect_check_timer_id, user_id, 1);

		if (ret != LSA_RET_OK)
		{
			SOCK_FATAL();
		}
	}

	if( 0 == cnt )
	{
		return;
	}

	SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "number of events in masks = %d", cnt);

	{
	LSA_INT32     ret_val;

#if TCIP_INT_CFG_TRACE_ON
	controller_add_select_measure_2();
#endif

	ret_val = sock_low_select(timeout);
#if TCIP_INT_CFG_TRACE_ON
	controller_add_select_measure_3();
#endif

	if (SOCK_LOW_SOCKET_ERROR == ret_val)
	{
		return; /* TODO: error treatment*/
	}
	}

	cnt = 0;

	for (i=0; i<=sock_data.active_sockets_last; i++)
	{
		SOCK_SOCKET_PTR  soc = sock_data.active_sockets[i];

		if ((soc->state & SOCK_CN_STATE_ACTIVE_MASK) != 0)
		{
			if (sock_low_in_exceptset(soc->lower_handle)) /* (!) exception */
			{
				cnt++;
				sock_socket_handle_except_mask(soc);
				if(soc->type == SOCK_LOW_STREAM/*!soc->sock_udp*/)
				{
					continue; /* (!) do not check other masks */
				}
			}

			if (sock_low_in_readset(soc->lower_handle)) /* read mask */
			{
				cnt++;
				sock_socket_handle_read_mask(soc);
			}

			if (sock_low_in_writeset(soc->lower_handle)) /* write mask */
			{
				cnt++;
				sock_socket_handle_write_mask(soc);
			}
		}
	}

#if TCIP_INT_CFG_TRACE_ON
	controller_add_select_measure_4();
#endif

	SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "nunmber of signalled events in masks : %d", cnt);
}
#endif


/*------------------------------------------------------------------------------
// sock_socket_handle_connect_error():
// return mapped connect response and close socket if possible
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_handle_connect_error(
	SOCK_SOCKET_PTR soc,
	LSA_INT sock_error
) {
	SOCK_LOG_FCT("sock_socket_handle_connect_error")

	LSA_OPCODE_TYPE opc = SOCK_RQB_GET_OPCODE(soc->connect_rb); /* (!) */

	LSA_RESPONSE_TYPE response = sock_low_map_connect_error(sock_error);

	SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE
		, "connect failed for socket handle(%d), lower_handle(0x%x),native_error(%d), response(%d)"
		, soc->dev_handle, soc->lower_handle, sock_error, response
		);

	sock_usr_callback(soc->channel, response, &soc->connect_rb);

	sock_socket_return_exception_resource(soc, response);

	if( opc == SOCK_OPC_CONNECT ) /* close socket here since user has no handle */
	{
		sock_socket_free(soc);
	}
	else
	{
		soc->state = SOCK_CN_STATE_OPENED; /* reset to opened state */
	}
}


#if !SOCK_INT_CFG_SOCKET_EVENTS
/*------------------------------------------------------------------------------
// sock_socket_handle_except_mask():
// handle situation if socket is in except mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_handle_except_mask(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_handle_except_mask")

	int sock_error;
	int native_error;

	sock_low_get_socket_error(soc->lower_handle, &sock_error, &native_error); /* erase error from socket */

	if(soc->type == SOCK_LOW_DGRAM)
	{
		return;
	}

	if(sock_is_not_null(soc->connect_rb)) /* connect failed -> return rqb */
	{
		sock_socket_handle_connect_error(soc, sock_error);
	}
	else if(sock_is_not_null(soc->accept.curr)) /* accept() failed -> try again */
	{
		/* if the stack stored the error in the listening socket (InterNiche does this) it was erased before (see getsockopt() above) */

		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
			, "listening socket handle(%d), lower_handle(0x%x), native_error(%d), n o t returning accept rqb"
			, soc->dev_handle, soc->lower_handle, sock_error
			);
	}
	else /* "normal" TCP socket (see above: if UDP -> return) -> cancel send and receive rqb's */
	{
		LSA_RESPONSE_TYPE response = sock_low_map_tcp_error(sock_error);

		SOCK_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_WARN
		  , "exception on TCP socket handle(%d), lower_handle(0x%x), native_error(%d), returning all rqb's"
		  , soc->dev_handle, soc->lower_handle, sock_error
		  );

		sock_socket_return_all_resources(soc, response); /* -> CN_STATE_DISCONNECTING */
	}
}


/*------------------------------------------------------------------------------
// sock_socket_handle_read_mask():
// handle situation if socket is in read mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_handle_read_mask(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_handle_read_mask")

	if(sock_is_not_null(soc->accept.curr)) /* client connect */
	{
		SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "listening socket_fd(0x%x) in read mask"
			, soc->lower_handle
			);

		sock_tcp_accept(soc);
	}
	else
	{
		if( soc->type == SOCK_LOW_STREAM /* tcp sockets: peek to detect TCP RST and FIN, see AP01300966 */
			&& sock_is_null(soc->recv.curr)
			)
		{
			sock_socket_peek_connection(soc);
		}
		else if (sock_is_not_null(soc->recv.curr))
		{
			SOCK_PROTOCOL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
				, "socket_fd(0x%x) in read mask"
				, soc->lower_handle
				);

			sock_socket_receive(soc);
		}
#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD
		else if(sock_are_equal(soc, sock_data.snmp_dev))  /* snmp device: read snmp request and dispatch request to snmp agent */
		{
			LSA_INT32 bytes_read = 0;

			LSA_RESPONSE_TYPE result = sock_udp_read(&bytes_read, soc, sock_data.snmp_recv_buf, SOCK_SNMP_BUFFER_SIZE, LSA_FALSE, &soc->rem_addr);

			if( result == SOCK_RSP_OK)
			{
				if( bytes_read > 0 )
				{
					sock_data.snmp_pending_count = 1;

					sock_snmp_dispatch_request( sock_data.snmp_recv_buf, (LSA_UINT32)bytes_read );
				}
			}
			else
			{
				SOCK_LOWER_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "error reading snmp socket, result(%d)", result);
			}
		}
#endif
	}
}


/*------------------------------------------------------------------------------
// sock_socket_handle_write_mask():
// handle situation if socket is in write mask after select call
//----------------------------------------------------------------------------*/
static LSA_VOID
sock_socket_handle_write_mask(
	SOCK_SOCKET_PTR soc
){
	SOCK_LOG_FCT("sock_socket_handle_write_mask")

	if( sock_is_not_null(soc->connect_rb) )
	{
		int sock_error;
		int native_error;

		sock_low_get_socket_error(soc->lower_handle, &sock_error, &native_error); /* check error condition on socket also, see AP01418351, AP01493449 */

		if( sock_error )
		{
			sock_socket_handle_connect_error(soc, sock_error);
		}
		else
		{
			sock_tcp_connect_client_socket(soc); /* conclude connect */
		}
	}
	else
	{
		SOCK_PROTOCOL_TRACE_02(0, LSA_TRACE_LEVEL_CHAT
			, "pending writes on lower_handle(0x%x), dev_handle(%d)"
			, soc->lower_handle, soc->dev_handle
			);

		sock_socket_send(soc);

		/***/

		if( sock_is_not_null(soc->open_close_rb) /* pending close active */
			&& sock_is_null(soc->send.curr)
			)
		{
			sock_socket_close(soc);
		}
	}
}
#endif


/*------------------------------------------------------------------------------
// sock_socket_peek_connection(): peeks into open socket to detect RST and FIN
//----------------------------------------------------------------------------*/
#if !SOCK_INT_CFG_SOCKET_EVENTS /* hu: 14.10.: sock_socket_peek_connection not referenced for  */
static LSA_VOID
sock_socket_peek_connection(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_peek_connection")

	char buf;
	LSA_INT result;
	LSA_INT native_error;

	/* note: remote abort detection will not work if socket has unread data */

	LSA_RESPONSE_TYPE response = sock_low_recv(soc->lower_handle, &buf, sizeof(buf), SOCK_LOW_PEEK, &result, &native_error);

	if (response != SOCK_RSP_OK_ACTIVE) /* note: BSD Stack can return would block */
	{
		if( result == SOCK_LOW_SOCKET_ERROR /* RST */
			|| result == 0 /* FIN */
			)
		{
			SOCK_PROTOCOL_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
				, "SOCKIF_Recv -> result(%d) for lower_handle(0x%x), handle(%d), return resources with ERR_REM_ABORT"
				, result, soc->lower_handle, soc->dev_handle
				);

			sock_socket_return_all_resources(soc, SOCK_RSP_ERR_REM_ABORT); /* see SOCK_OPC_EXCEPTION_NOTIFY */

			return;
		}
	}
}
#endif


/*------------------------------------------------------------------------------
// sock_socket_close
// close rb present - return if possible
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_close(
SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_close")

#if SOCK_CFG_ENABLE_MULTICASTS
	if ( soc->mcast_interface_id != 0 ) /* release mcast resource */
	{
		LSA_RESPONSE_TYPE resp = tcip_srv_multicast(soc->dev_handle
							, soc->mcast_interface_id
							, soc->mcast_address
							, TCIP_SRV_MULTICAST_DISABLE
							);

		if( resp != TCIP_OK )
		{
			SOCK_FATAL(); /* must not fail */
		}

		soc->state = SOCK_CN_STATE_DISABLED; /* (!) must not use this socket any more */
		/*sado -- if state is disabled, events in eventlist are not handled (see sock_socket_do_events) */
	}
	else
#endif
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
			, "socket handle(%d) closed by application"
			, soc->dev_handle
			);
#if SOCK_CFG_OPEN_BSD
#if TCIP_INT_CFG_TRACE_ON

#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
		{
		struct obsd_pnio_sb_fill_histogram histogram;
		int err = 0;
		LSA_BOOL haveit = SockIF_GetSockBufHistogram((unsigned int)soc->lower_handle, &histogram, &err);
		if (haveit)
		{
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,socket buf histogram sock_fd:%6d", soc->lower_handle);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,        histogram.ui0ByteCnt:%6d", histogram.ui0ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,      histogram.ui128ByteCnt:%6d", histogram.ui128ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,      histogram.ui256ByteCnt:%6d", histogram.ui256ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,      histogram.ui512ByteCnt:%6d", histogram.ui512ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,     histogram.ui1024ByteCnt:%6d", histogram.ui1024ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,     histogram.ui2048ByteCnt:%6d", histogram.ui2048ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,     histogram.ui4096ByteCnt:%6d", histogram.ui4096ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,     histogram.ui8192ByteCnt:%6d", histogram.ui8192ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: ,    histogram.ui16384ByteCnt:%6d", histogram.ui16384ByteCnt);
			SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: , histogram.uiOverflowByteCnt:%6d", histogram.uiOverflowByteCnt);
#ifdef PNTEST_BUILD_WINDOWS
			printf("instrumentation: ,socket buf histogram sock_fd:%6d\n", soc->sock_fd);
			printf("instrumentation: ,        histogram.ui0ByteCnt:%6d\n", histogram.ui0ByteCnt);
			printf("instrumentation: ,      histogram.ui128ByteCnt:%6d\n", histogram.ui128ByteCnt);
			printf("instrumentation: ,      histogram.ui256ByteCnt:%6d\n", histogram.ui256ByteCnt);
			printf("instrumentation: ,      histogram.ui512ByteCnt:%6d\n", histogram.ui512ByteCnt);
			printf("instrumentation: ,     histogram.ui1024ByteCnt:%6d\n", histogram.ui1024ByteCnt);
			printf("instrumentation: ,     histogram.ui2048ByteCnt:%6d\n", histogram.ui2048ByteCnt);
			printf("instrumentation: ,     histogram.ui4096ByteCnt:%6d\n", histogram.ui4096ByteCnt);
			printf("instrumentation: ,     histogram.ui8192ByteCnt:%6d\n", histogram.ui8192ByteCnt);
			printf("instrumentation: ,    histogram.ui16384ByteCnt:%6d\n", histogram.ui16384ByteCnt);
			printf("instrumentation: , histogram.uiOverflowByteCnt:%6d\n", histogram.uiOverflowByteCnt);
#endif
		}
		else
		{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: error fetching histogram, err(%d)", err);
#ifdef PNTEST_BUILD_WINDOWS
		printf("instrumentation: error fetching histogram, err(%d)");
#endif
		}
		}
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTOGRAM */

#ifdef OBSD_MEMORY_TRACING
		tcip_trace_statistics(); /* sado - OpenBSD Page Memory Tracing */
#endif /* OBSD_MEMORY_TRACING */
#endif
#endif

		if( soc->state != SOCK_CN_STATE_DISCONNECTING ) /* if not pending close */
		{
			sock_socket_return_all_resources(soc, SOCK_RSP_ERR_LOC_ABORT);
		}

		sock_usr_callback(soc->channel, SOCK_RSP_OK, &soc->open_close_rb);

		sock_socket_free(soc);
	}
}


/*------------------------------------------------------------------------------
// sock_socket_receive() :called if
// (1) resource was provided by user (receive request) or (!)
// (2) when select returns and resource is available
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_receive(
	SOCK_SOCKET_PTR soc
) {
	while( sock_is_not_null(soc->recv.curr) )
	{
		LSA_RESPONSE_TYPE read_response;

#if TCIP_INT_CFG_TRACE_ON /* measure time needed for receive system call */
		LSA_UINT64 timestamp = controller_get_time();
#endif

		if( soc->recv.curr->args.data.data_length == 0 ) /* 0 byte read always ok */
		{
			read_response = SOCK_RSP_OK;
		}
		else if( soc->type == SOCK_LOW_DGRAM )
		{
			read_response = sock_udp_receive(soc, &(soc->recv.curr->args.data));
		}
		else
		{
			read_response = sock_tcp_receive(soc, &(soc->recv.curr->args.data));
		}

		if( read_response == SOCK_RSP_OK ) /* return rqb and look for next in queue */
		{
#if TCIP_INT_CFG_TRACE_ON /* measure time needed for receive system call */
		controller_add_recv_measure_101(timestamp);
		controller_add_recv_measure_102();
#endif

			soc->recv.bytes_transferred = 0; /* reset when rqb returns */

			sock_usr_callback(soc->channel, read_response, &soc->recv.curr); /* soc->recv.curr <- LSA_NULL */

			soc->recv.curr = sock_queue_dequeue(&soc->recv.queue);

		}
		else if( read_response == SOCK_RSP_OK_ACTIVE ) /*  would block */
		{
			break;
		}
		else /* return rqb and all resources with error (TCP only) */
		{
			sock_socket_return_all_resources(soc, read_response); /* sets socket to SOCK_CN_STATE_DISCONNECTING */

			break; /* read error on socket -> no reading from this socket */
		}
	}
}


/*------------------------------------------------------------------------------
// called when
//	(1) send rqb was provided or
//  (2) socket is ready to write after select and send rqb is available
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_send(
	SOCK_SOCKET_PTR soc
) {
	SOCK_LOG_FCT("sock_socket_send")

	while( sock_is_not_null(soc->send.curr) )
	{
		LSA_RESPONSE_TYPE send_response;

#if TCIP_INT_CFG_TRACE_ON
		controller_add_user_send_measure_2(soc->send.curr);
#endif

		if( soc->type == SOCK_LOW_DGRAM )
		{
			send_response = sock_udp_send(soc, &(soc->send.curr->args.data));
		}
		else /* tcp */
		{
			if( soc->send.curr->args.data.data_length == 0 ) /* 0 byte write always ok */
			{
				send_response = SOCK_RSP_OK;
			}
			else
			{
				send_response = sock_tcp_send(soc, &(soc->send.curr->args.data));

				if (send_response != SOCK_RSP_OK && send_response != SOCK_RSP_OK_ACTIVE) /* send error on tcp socket -> no sending with this socket */
				{
					sock_socket_return_all_resources(soc, send_response); /* sets socket to SOCK_CN_STATE_DISCONNECTING */

					break;
				}
			}
		}

		if( send_response == SOCK_RSP_OK_ACTIVE ) /* would block */
		{
			break;
		}

		/* return rqb and look for next in queue */

		soc->send.bytes_transferred = 0; /* reset when rqb returns */

		sock_usr_callback(soc->channel, send_response, &soc->send.curr); /* (!) sets soc->send.curr to LSA_NULL */

		soc->send.curr = sock_queue_dequeue(&soc->send.queue);
	}
}


/*------------------------------------------------------------------------------
// set socket struct state to SOCK_CN_STATE_DISCONNECTING
// and pass back all user resources
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_return_all_resources(
	SOCK_SOCKET_PTR dev,
	LSA_UINT16 abort_resp
) {
	SOCK_LOG_FCT("sock_socket_return_all_resources")

	dev->abort_resp = abort_resp; /* used for response if rqb arrives for this socket  */

	dev->send.bytes_transferred = 0;
	dev->recv.bytes_transferred = 0;

	dev->state = SOCK_CN_STATE_DISCONNECTING; /* socket is about to close (user must send SOCK_OPC_CLOSE) and has no resources */


#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD

	if (sock_are_equal(dev, sock_data.snmp_dev))
	{
		if (sock_data.snmp_extern_count == 0)
		{
			sock_snmp_deactivate_int();
		}
		else
		{
			sock_data.snmp_intern_auto = LSA_TRUE;
		}
		return;
	}

#endif

	/* pass back all receive rqbs */
	sock_socket_return_receive_resources(dev, abort_resp);

	/* pass back all send rqbs */
	sock_socket_return_send_resources(dev, abort_resp);

	/* pass back connect rqb */
	sock_socket_return_connect_resource(dev, abort_resp);

	/* pass back accept notify rqbs */
	sock_socket_return_accept_notify_resources(dev, abort_resp);

	/* pass back except_notify rqb */
	sock_socket_return_exception_resource(dev, abort_resp);

	return;
}


/*------------------------------------------------------------------------------
// return send resources if present
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_return_send_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_send_resources")

	if( sock_is_not_null(soc->send.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
			, "confirm send rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->send.curr, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->send.curr);

		rb = sock_queue_dequeue(&soc->send.queue);

		while ( sock_is_not_null(rb) )
		{
			SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
				, "confirm send rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
				, rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
				);

			sock_usr_callback(soc->channel, response, &rb);

			rb = sock_queue_dequeue(&soc->send.queue);
		}
	}
}


/*------------------------------------------------------------------------------
// return receive resources if present
//----------------------------------------------------------------------------*/
LSA_INT
sock_socket_return_receive_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_receive_resources")

	LSA_INT num = 0;

	if(sock_is_not_null(soc->recv.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "confirm receive rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->recv.curr, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->recv.curr);

		num++;

		rb = sock_queue_dequeue(&soc->recv.queue);

		while ( sock_is_not_null(rb) )
		{
			SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
				, "confirm receive rqb(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
				, rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
				);

			sock_usr_callback(soc->channel, response, &rb);

			num++;

			rb = sock_queue_dequeue(&soc->recv.queue);
		}
	}

	return num;
}


/*------------------------------------------------------------------------------
// return connect resource if present
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_return_connect_resource(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_connect_resource")

	if ( sock_is_not_null(soc->connect_rb) ) /* pass back connect_rb if present */
	{
		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
			, "SOCK_OPC_CONNECT(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->connect_rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->connect_rb);
	}
}


/*------------------------------------------------------------------------------
// return accept notify resource if present
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_return_accept_notify_resources(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	if(sock_is_not_null(soc->accept.curr))
	{
		SOCK_UPPER_RQB_PTR_TYPE rb;

		sock_usr_callback(soc->channel, response, &soc->accept.curr);

		rb = sock_queue_dequeue(&soc->accept.queue);

		while ( sock_is_not_null(rb) ) /* return pending SOCK_OPC_ACCEPT_NOTIFY */
		{
			sock_usr_callback(soc->channel, response, &rb);

			rb = sock_queue_dequeue(&soc->accept.queue);
		}
	}
}


/*------------------------------------------------------------------------------
// return except resource if present
//----------------------------------------------------------------------------*/
LSA_VOID
sock_socket_return_exception_resource(
	SOCK_SOCKET_PTR soc,
	LSA_UINT16 response
) {
	SOCK_LOG_FCT("sock_socket_return_exception_resource")

	if ( sock_is_not_null(soc->exception_rb) )
	{
		SOCK_PROTOCOL_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW
			, "SOCK_OPC_EXCEPT_NOTIFY(0x%x), socket handle(0x%x), port(%d), error(0x%x)"
			, soc->exception_rb, soc->dev_handle, SOCK_NTOHS(soc->loc_addr.sin_port), response
			);

		sock_usr_callback(soc->channel, response, &soc->exception_rb);
	}
}


/*------------------------------------------------------------------------------
// Sets the socket options for the specified open socket descriptor according
// to details pointers to send_option_val, rcv_option_val, udp may be NULL
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_socket_set_options(
	SOCK_CHANNEL_CONST_PTR_TYPE channel,
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 type
) {
	SOCK_LOG_FCT("sock_socket_set_options")

	LSA_INT32 option_val;
	LSA_INT native_error;

	/* switch socket to non-blocking mode */

	option_val = 1;
	if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_NonBlocking, &option_val, &native_error))
	{
		SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
							, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_NonBlocking, option_val(%d), native_error(%d)"
							, lower_handle, option_val, native_error
							);

		return SOCK_RSP_ERR_LOC_NET;
	}

	if ( (type == SOCK_LOW_STREAM) && channel->detail_ptr->socket_option.SO_TCPNODELAY_) /* don't delay sending data */
	{
		option_val = 1;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_TCPNoDelay, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_TCPNoDelay, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if ( (type == SOCK_LOW_STREAM) && channel->detail_ptr->socket_option.SO_TCPNOACKDELAY_) /* don't delay sending ACKs */
	{
		option_val = 1;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_TCPNoAckDelay, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_TCPNoAckDelay, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_REUSEADDR_)
	{
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_ReuseAddr, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_ReuseAddr, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (type == SOCK_LOW_STREAM) /* set keep alive as requested (in all cases!) AP00938830 */
	{
		if(SOCK_KEEPALIVE_ACTIVE/* == LSA_TRUE */)
		{
			option_val = 1;
		}
		else
		{
			option_val = 0;
		}

		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_KeepAlive, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_KeepAlive, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}

		option_val = 1;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_OOBInline, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_OOBInline, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	option_val = 1;
	if (channel->detail_ptr->socket_option.SO_DONTROUTE_)
	{
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_DontRoute, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_DontRoute, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_SNDBUF_)
	{
		option_val = channel->detail_ptr->send_buffer_max_len;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_SendBuffer, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_SendBuffer, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_RCVBUF_)
	{
		option_val =  channel->detail_ptr->rec_buffer_max_len;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_RecvBuffer, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_RecvBuffer, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (channel->detail_ptr->socket_option.SO_LINGER_)
	{
		int linger_time = (int)channel->detail_ptr->linger_time;

		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_Linger, &linger_time, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_Linger, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	if (type == SOCK_LOW_DGRAM) /* SO_BROADCAST applies only to datagram sockets */
	{
		if (channel->detail_ptr->socket_option.SO_BROADCAST_)
		{
			option_val = 1;

			if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_Broadcast, &option_val, &native_error))
			{
				SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
									, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_Broadcast, option_val(%d), native_error(%d)"
									, lower_handle, option_val, native_error
									);

				return SOCK_RSP_ERR_LOC_NET;
			}
		}
	}

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
	if (type == SOCK_LOW_DGRAM)
	{
		option_val = 1;
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_IPRecvdStAddr, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_IPRecvdStAddr, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}
#endif

#if defined SIO_UDP_CONNRESET
	#if SOCK_CFG_WINSOCK  /* check if option available */
	/*
	Windows XP: Controls whether UDP PORT_UNREACHABLE messages are reported. Set to TRUE to enable reporting. Set to FALSE to disable reporting.
	SIO_UDP_CONNRESET
	*/
	if (type == SOCK_LOW_DGRAM)
	{
		option_val = 0; /* LSA_FALSE */
		if (!sock_low_setsockopt(lower_handle, sock_low_sockopt_Connreset, &option_val, &native_error))
		{
			SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
								, "error sock_low_setsockopt(lower_handle(%d), option=sock_low_sockopt_Connreset, option_val(%d), native_error(%d)"
								, lower_handle, option_val, native_error
								);

			return SOCK_RSP_ERR_LOC_NET;
		}
	}
	#endif
#endif

	return SOCK_RSP_OK;
}


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
