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
/*  F i l e               &F: tcip_send.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NRT send functionality                                                   */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  4
#define TCIP_MODULE_ID     4

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if TCIP_INT_CFG_TRACE_ON

typedef struct tcip_send_stats_type_tag
{
	int g_record_stats;

	controller_measure_struct controller_tcip_send_measure_1;
	controller_measure_struct controller_tcip_send_measure_2;

	LSA_UINT64 d_1_2_min;
	LSA_UINT64 d_1_2_max;
	LSA_UINT64 d_1_2_mean;

}   tcip_send_stats_type;

static tcip_send_stats_type tcip_send_stats;

/* 4 send measures */

/* stop recording stats if send queue is full */

static void controller_calc_tcip_send_statistics(void);
static void controller_print_tcip_send_statistics(void);

void controller_init_tcip_send_measures()
{
	tcip_send_stats.g_record_stats = 1;

	tcip_send_stats.controller_tcip_send_measure_1.count = 0;
	tcip_send_stats.controller_tcip_send_measure_2.count = 0;
}

void controller_add_tcip_send_measure_1(LSA_UINT64 timestamp) /* User gives RQB to SOCK */
{
	if (!tcip_send_stats.g_record_stats)
	{
		return; /*stop statistic*/
	}

	tcip_send_stats.controller_tcip_send_measure_1.m[tcip_send_stats.controller_tcip_send_measure_1.count] = timestamp;
	tcip_send_stats.controller_tcip_send_measure_1.count ++;

	if (tcip_send_stats.controller_tcip_send_measure_1.count == CONTROLLER_MAX_SEND_MEASURES)
	{
		tcip_send_stats.g_record_stats = 0;
		return;
	}
}

void controller_add_tcip_send_measure_2(void) /* copy into socket buffer done */
{
	if (!tcip_send_stats.g_record_stats)
	{
		return; /*stop statistic*/
	}

	tcip_send_stats.controller_tcip_send_measure_2.m[tcip_send_stats.controller_tcip_send_measure_2.count] = controller_get_time();
	tcip_send_stats.controller_tcip_send_measure_2.count ++;

	if (tcip_send_stats.controller_tcip_send_measure_2.count == CONTROLLER_MAX_MEASURES)
	{
		controller_calc_tcip_send_statistics();

		controller_print_tcip_send_statistics();

		controller_init_tcip_send_measures();
	}

	if (tcip_send_stats.controller_tcip_send_measure_2.count == CONTROLLER_MAX_SEND_MEASURES)
	{
		tcip_send_stats.g_record_stats = 0;
	}
}

void controller_calc_tcip_send_statistics(void)
{
	int i;
	LSA_UINT64 d_1_2_sum = 0;

	/* reset delta min, max and mean vlaues */
	tcip_send_stats.d_1_2_min  = 0xffffffff; /* 32 Bit UINT MAX should be sufficent */
	tcip_send_stats.d_1_2_max  = 0;
	tcip_send_stats.d_1_2_mean = 0;

	/* difference for all available measures */
	for (i=0; i<tcip_send_stats.controller_tcip_send_measure_2.count; i++)
	{
		LSA_UINT64 d_1_2 = 0;

		d_1_2  = tcip_send_stats.controller_tcip_send_measure_2.m[i] - tcip_send_stats.controller_tcip_send_measure_1.m[i];

		if (d_1_2) CONTROLLER_MIN(tcip_send_stats.d_1_2_min, d_1_2)
			CONTROLLER_MAX(tcip_send_stats.d_1_2_max, d_1_2)

		d_1_2_sum += d_1_2;
	}

	/* calculate mean values */
	tcip_send_stats.d_1_2_mean = d_1_2_sum / tcip_send_stats.controller_tcip_send_measure_2.count;
}

void controller_print_tcip_send_statistics(void)
{
	TCIP_LOG_FCT("controller_print_send_statistics")

	/* print statistic on trace level FATAL */

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: tcip send statistic:  controller_tcip_send_measure_2.count:%8u timestamps", tcip_send_stats.controller_tcip_send_measure_2.count);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: tcip send statistic: 1->2 Min:%8u us, 1->2 Max:%8u us, 1->2 Mean:%8u us"
		, (LSA_UINT32)tcip_send_stats.d_1_2_min, (LSA_UINT32)tcip_send_stats.d_1_2_max, (LSA_UINT32)tcip_send_stats.d_1_2_mean
		);

#ifdef PNTEST_BUILD_WINDOWS
	    /* */
		printf("instrumentation: tcip send statistic:  controller_tcip_send_measure_2.count:%8d timestamps\n", tcip_send_stats.controller_tcip_send_measure_2.count);

		/* edd_obsd Min Max Mean */

		printf("instrumentation: tcip send statistic: 1->2 Min:%8llu us, 1->2 Max:%8llu us, 1->2 Mean:%8llu us\n"
			, tcip_send_stats.d_1_2_min, tcip_send_stats.d_1_2_max, tcip_send_stats.d_1_2_mean
			);
#endif

}

/* ------------------------------------------------------
send statistic in bsd stack
---------------------------------------------------------*/

typedef struct bsd_send_stats_type_tag
{
	int index;
	int counter;
	int drops;

	controller_measure_struct send_measure_1;
	controller_measure_struct send_measure_2;
	controller_measure_struct send_measure_3;
	controller_measure_struct send_measure_4;
	controller_measure_struct send_measure_5;
	controller_measure_struct send_measure_6;

	LSA_UINT64 d_1_2_min;
	LSA_UINT64 d_1_2_max;

	LSA_UINT64 d_2_3_min;
	LSA_UINT64 d_2_3_max;

	LSA_UINT64 d_3_4_min;
	LSA_UINT64 d_3_4_max;

	LSA_UINT64 d_4_5_min;
	LSA_UINT64 d_4_5_max;

	LSA_UINT64 d_5_6_min;
	LSA_UINT64 d_5_6_max;

	LSA_UINT64 d_all_min;
	LSA_UINT64 d_all_max;

}   bsd_send_stats_type;

static bsd_send_stats_type send_stats;

static void controller_calc_send_stats();
static void controller_print_send_stats();

void controller_init_send_measures()
{
	int i;

	send_stats.index = -1;
	send_stats.counter = 0;
	send_stats.drops = 0;

	send_stats.d_1_2_min = 0;
	send_stats.d_1_2_max = 0;

	send_stats.d_2_3_min = 0;
	send_stats.d_2_3_max = 0;

	send_stats.d_3_4_min = 0;
	send_stats.d_3_4_max = 0;

	send_stats.d_4_5_min = 0;
	send_stats.d_4_5_max = 0;

	send_stats.d_5_6_min = 0;
	send_stats.d_5_6_max = 0;

	send_stats.d_all_min = 0;
	send_stats.d_all_max = 0;

	for (i=0; i<CONTROLLER_MAX_MEASURES; i++)
	{
		send_stats.send_measure_1.m[i] = 0;
		send_stats.send_measure_2.m[i] = 0;
		send_stats.send_measure_3.m[i] = 0;
		send_stats.send_measure_4.m[i] = 0;
		send_stats.send_measure_5.m[i] = 0;
		send_stats.send_measure_6.m[i] = 0;
	}
}

void controller_add_obsd_send_measure_1(void)
{
	send_stats.counter++;
	send_stats.index++;

	if(send_stats.index == CONTROLLER_MAX_MEASURES)
	{
		controller_calc_send_stats();

		controller_print_send_stats();

		controller_init_send_measures();

		send_stats.index = 0;
	}

	send_stats.send_measure_1.m[send_stats.index] = controller_get_time();
}

void controller_add_obsd_send_measure_2(void)
{
	send_stats.send_measure_2.m[send_stats.index] = controller_get_time();
}

void controller_add_obsd_send_measure_3(void)
{
	send_stats.send_measure_3.m[send_stats.index] = controller_get_time();
}

void controller_add_obsd_send_measure_4(void)
{
	send_stats.send_measure_4.m[send_stats.index] = controller_get_time();
}

void controller_add_obsd_send_measure_5(void)
{
	send_stats.send_measure_5.m[send_stats.index] = controller_get_time();
}

void controller_add_obsd_send_measure_6(void)
{
	send_stats.send_measure_6.m[send_stats.index] = controller_get_time();
}

void controller_calc_send_stats()
{
	int i;
	int drop;

	send_stats.d_1_2_min  = 0xffffffff; /* 32 Bit UINT MAX should be sufficent */
	send_stats.d_1_2_max  = 0;

	send_stats.d_2_3_min = 0xffffffff;
	send_stats.d_2_3_max  = 0;

	send_stats.d_3_4_min  = 0xffffffff;
	send_stats.d_3_4_max  = 0;

	send_stats.d_4_5_min  = 0xffffffff;
	send_stats.d_4_5_max  = 0;

	send_stats.d_5_6_min  = 0xffffffff;
	send_stats.d_5_6_max  = 0;

	send_stats.d_all_min  = 0xffffffff;
	send_stats.d_all_max  = 0;

	for (i=0; i<send_stats.index; i++)
	{
		LSA_UINT64 d_1_2 = 0;
		LSA_UINT64 d_2_3 = 0;
		LSA_UINT64 d_3_4 = 0;
		LSA_UINT64 d_4_5 = 0;
		LSA_UINT64 d_5_6 = 0;
		LSA_UINT64 d_all = 0;

		drop = 0;

		if (send_stats.send_measure_2.m[i] && send_stats.send_measure_1.m[i])
			d_1_2 = send_stats.send_measure_2.m[i] - send_stats.send_measure_1.m[i];
		else
			drop++;

		if (send_stats.send_measure_3.m[i] && send_stats.send_measure_2.m[i])
			d_2_3 = send_stats.send_measure_3.m[i] - send_stats.send_measure_2.m[i];
		else
			drop++;

		if (send_stats.send_measure_4.m[i] && send_stats.send_measure_3.m[i])
			d_3_4 = send_stats.send_measure_4.m[i] - send_stats.send_measure_3.m[i];
		else
			drop++;

		if (send_stats.send_measure_5.m[i] && send_stats.send_measure_4.m[i])
			d_4_5 = send_stats.send_measure_5.m[i] - send_stats.send_measure_4.m[i];
		else
			drop++;

		if (send_stats.send_measure_6.m[i] && send_stats.send_measure_5.m[i])
			d_5_6 = send_stats.send_measure_6.m[i] - send_stats.send_measure_5.m[i];
		else
			drop++;

		if (send_stats.send_measure_6.m[i] && send_stats.send_measure_1.m[i])
			d_all = send_stats.send_measure_6.m[i] - send_stats.send_measure_1.m[i];
		else
			drop++;

		/***/

		if (d_1_2) CONTROLLER_MIN(send_stats.d_1_2_min, d_1_2)
		if (d_1_2) CONTROLLER_MAX(send_stats.d_1_2_max, d_1_2)

		if (d_2_3) CONTROLLER_MIN(send_stats.d_2_3_min, d_2_3)
		if (d_2_3) CONTROLLER_MAX(send_stats.d_2_3_max, d_2_3)

		if (d_3_4) CONTROLLER_MIN(send_stats.d_3_4_min, d_3_4)
		if (d_3_4) CONTROLLER_MAX(send_stats.d_3_4_max, d_3_4)

		if (d_4_5) CONTROLLER_MIN(send_stats.d_4_5_min, d_4_5)
		if (d_4_5) CONTROLLER_MAX(send_stats.d_4_5_max, d_4_5)

		if (d_5_6) CONTROLLER_MIN(send_stats.d_5_6_min, d_5_6)
		if (d_5_6) CONTROLLER_MAX(send_stats.d_5_6_max, d_5_6)

		if (d_all) CONTROLLER_MIN(send_stats.d_all_min, d_all)
		if (d_all) CONTROLLER_MAX(send_stats.d_all_max, d_all)

		if (drop)
		{
			send_stats.drops++;
		}
	}
}

void controller_print_send_stats()
{
	TCIP_LOG_FCT("controller_print_send_stats")


	/* print statistic on trace level FATAL */
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: send statistic: %8u timestamps", send_stats.index );
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: send statistic: %8u drops", send_stats.drops );

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic:  1->2 Min: %8u us,  1->2 Max: %8u us"
		, (LSA_UINT32)send_stats.d_1_2_min, (LSA_UINT32)send_stats.d_1_2_max
		);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic:  2->3 Min: %8u us,  2->3 Max: %8u us"
		, (LSA_UINT32)send_stats.d_2_3_min, (LSA_UINT32)send_stats.d_2_3_max
		);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic:  3->4 Min: %8u us,  3->4 Max: %8u us,"
		, (LSA_UINT32)send_stats.d_3_4_min, (LSA_UINT32)send_stats.d_3_4_max
		);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic:  4->5 Min: %8u us,  4->5 Max: %8u us"
		, (LSA_UINT32)send_stats.d_4_5_min, (LSA_UINT32)send_stats.d_4_5_max
		);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic:  5->6 Min: %8u us,  5->6 Max: %8u us"
		, (LSA_UINT32)send_stats.d_5_6_min, (LSA_UINT32)send_stats.d_5_6_max
		);

	/* all (total time edd->user) Min Max Mean */
	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: send statistic: Total Min: %8u us, Total Max: %8u us"
		, (LSA_UINT32)send_stats.d_all_min, (LSA_UINT32)send_stats.d_all_max
		);

#ifdef PNTEST_BUILD_WINDOWS
		printf("instrumentation: send statistic: %8u timestamps\n", send_stats.index);
		printf("instrumentation: send statistic: %8u drops\n", send_stats.drops);

		/* edd_obsd Min Max Mean */

		printf("instrumentation: send statistic:  1->2 Min:%8llu us,   1->2 Max:%8llu us\n"
			, send_stats.d_1_2_min, send_stats.d_1_2_max
			);

		printf("instrumentation: send statistic:  2->3 Min:%8llu us,   2->3 Max:%8llu us\n"
			, send_stats.d_2_3_min, send_stats.d_2_3_max
			);

		printf("instrumentation: send statistic:  3->4 Min:%8llu us,   3->4 Max:%8llu us\n"
			, send_stats.d_3_4_min, send_stats.d_3_4_max
			);

		printf("instrumentation: send statistic:  4->5 Min:%8llu us,   4->5 Max:%8llu us\n"
			, send_stats.d_4_5_min, send_stats.d_4_5_max
			);

		printf("instrumentation: send statistic:  5->6 Min:%8llu us,   5->6 Max:%8llu us\n"
			, send_stats.d_5_6_min, send_stats.d_5_6_max
			);

		printf("instrumentation: send statistic: Total Min:%8llu us,  Total Max:%8llu us\n"
			, send_stats.d_all_min, send_stats.d_all_max
			);
#endif

}

#endif

#if TCIP_CFG_INTERNICHE

#include "ether.h"

#if ETHHDR_BIAS != 0
#error "ETHHDR_BIAS must be 0, EDD_SRV_NRT_SEND needs 4 byte alignment"
#endif

#define TCIP_SENDBUFFER_SIZE  BIGBUFSIZE /* not EDD_FRAME_BUFFER_LENGTH, see AP00168784 */

#endif

/* OBSD_ITGR */
#if TCIP_CFG_OPEN_BSD
#if TCIP_CFG_COPY_ON_SEND == 0
#error "TCIP_CFG_COPY_ON_SEND must be enabled"
#endif

#endif

/*===========================================================================*/
/*===========================================================================*/

/* Task 1560385 (use if_sendq) -- TCIP-EDD-Send-Queue only for Interniche */
#if TCIP_CFG_INTERNICHE
/*---------------------------------------------------------------------------*/
/* check availability of send rqb on EDD ARP channel, see 1824771            */
/*---------------------------------------------------------------------------*/
int tcip_is_send_resource_available(const void* ifp)
{
	TCIP_CHANNEL_PTR_TYPE channel = (TCIP_CHANNEL_PTR_TYPE)((NET)ifp)->n_local;

	if (channel->edd.send_pkts.cur_len + 30 < channel->edd.send_pkts.max_len)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static LSA_BOOL tcip_put_pkt(TCIP_PACKET_HOLD_PTR_TYPE pkt_list, PACKET pkt)
{
	TCIP_LOG_FCT("tcip_put_pkt")

	LSA_BOOL ret_val = LSA_FALSE;

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"inserted pkt 0x%x",pkt);

	LOCK_NET_RESOURCE (FREEQ_RESID);
	pkt->next = LSA_NULL;

	if (pkt_list->cur_len == pkt_list->max_len)
	{
		ret_val = LSA_FALSE;
	}
	else
	{
		if( is_null(pkt_list->first_out) )
		{
			pkt_list->first_out = pkt;
		}
		else
		{
			((PACKET)(pkt_list->last_pkt))->next = pkt;
		}

		pkt_list->last_pkt = pkt;

		pkt_list->cur_len++;

		ret_val = LSA_TRUE;
	}

	UNLOCK_NET_RESOURCE (FREEQ_RESID);
	return ret_val;
}

/*===========================================================================*/

static PACKET tcip_get_pkt(TCIP_PACKET_HOLD_PTR_TYPE pkt_list)
{
	TCIP_LOG_FCT("tcip_get_pkt")

	PACKET pkt;

	LOCK_NET_RESOURCE (FREEQ_RESID);

	pkt = pkt_list->first_out;

	if (is_not_null(pkt) )
	{
		pkt_list->first_out = pkt->next;
		pkt->next = LSA_NULL; /* AP00936006 */
		pkt_list->cur_len--;
	}

	if( is_null(pkt_list->first_out) )
	{
		pkt_list->last_pkt = LSA_NULL;

		pkt_list->cur_len = 0; /* sanity */
	}

	UNLOCK_NET_RESOURCE (FREEQ_RESID);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"returned pkt 0x%x",pkt);

	return pkt;
}
#endif
/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*  the RQBs are allocated and single linked, last->next == LSA_NULL         */
/*  the list is handled as a LIFO list,                                      */
/*  the latest used RQB will be the next to be used                          */
/*  this allows simple list routines, for the lists are associated to a      */
/*  edd channel, the start anchor is a parameter                             */
/*---------------------------------------------------------------------------*/
/* queue routine to retrieve a RQB from the preallocated list                */
/* INPUT: SendRqbs head of the queue                                         */
/*---------------------------------------------------------------------------*/
static EDD_RQB_TYPE * tcip_getEddSendRQB (EDD_UPPER_RQB_PTR_TYPE * SendRqbs)
{
	TCIP_LOG_FCT("tcip_getEddSendRQB")
	EDD_RQB_TYPE * pEddSendRQB;

#if TCIP_CFG_INTERNICHE
	LOCK_NET_RESOURCE(FREEQ_RESID);
#endif

	pEddSendRQB = *SendRqbs;

	if( is_not_null(pEddSendRQB) )
	{
		*SendRqbs = EDD_RQB_GET_NEXT_RQB_PTR(pEddSendRQB);
	}
	else
	{
		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "tcip_getEddSendRQB: SendRqbs 0x%x, pEddSendRQB 0x%x", SendRqbs, pEddSendRQB);
	}

#if TCIP_CFG_INTERNICHE
	UNLOCK_NET_RESOURCE(FREEQ_RESID);
#endif

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"returned RQB 0x%x",pEddSendRQB);

	return(pEddSendRQB);
}

/*---------------------------------------------------------------------------*/
/* queue routine to return a RQB back to the preallocated list               */
/* INPUT: SendRqbs head of the queue                                         */
/*        actSendRqb Rqb that is returned, becomes first element of queue    */
/*---------------------------------------------------------------------------*/
static LSA_VOID tcip_putEddSendRQB (EDD_UPPER_RQB_PTR_TYPE *SendRqbs, EDD_RQB_TYPE * actSendRqb)
{
	TCIP_LOG_FCT("tcip_putEddSendRQB")

#if TCIP_CFG_INTERNICHE
	LOCK_NET_RESOURCE(FREEQ_RESID);
#endif

	if( is_null(*SendRqbs) )
	{
		EDD_RQB_SET_NEXT_RQB_PTR(actSendRqb,LSA_NULL);
	}
	else
	{
		EDD_RQB_SET_NEXT_RQB_PTR(actSendRqb,*SendRqbs);
	}

	*SendRqbs = actSendRqb;

#if TCIP_CFG_INTERNICHE
	UNLOCK_NET_RESOURCE(FREEQ_RESID);
#endif

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"inserted RQB 0x%x next = 0x%x",actSendRqb,TCIP_EDD_GET_NEXT_RQB_PTR(actSendRqb));
}

/*
 *  free send buf and RQB
 */
 /*lint -e{818} symbol channel Pointer parameter 'Symbol' (Location) could be declared ptr to const */
static LSA_VOID tcip_FreeSndBuf( TCIP_CHANNEL_CONST_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_TYPE  *pRQB)
{
	TCIP_LOG_FCT("tcip_FreeSndBuf")

	EDD_UPPER_NRT_SEND_PTR_TYPE     pParam = LSA_NULL;
	LSA_UINT16                      ret16_val;

	pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		TCIP_EDD_LOWER_MEM_PTR_TYPE pFrame = pParam->pBuffer;

		if ( is_not_null(pFrame) )
		{
			TCIP_EDD_FREE_TX_TRANSFER_BUFFER_MEM(&ret16_val, pFrame, channel->sysptr);
			TCIP_ASSERT(ret16_val == LSA_RET_OK);
		}
	}

	tcip_FreeRQB(channel->sysptr, pRQB );
}

/*---------------------------------------------------------------------------*/
/* initialisize a queue of EDD Send RQBs                                     */
/* INPUT: SendRqbs head of the queue                                         */
/*        Count  Nr of RQBs to add to queue                                  */
/*        EDD_handle handle of EDD to be used to process the send request    */
/*        pSys sys pointer                                                   */
/*---------------------------------------------------------------------------*/

LSA_VOID tcip_initEddSendRQBList(EDD_UPPER_RQB_PTR_TYPE *SendRqbs,
								LSA_UINT32       Count,
								EDD_HANDLE_LOWER_TYPE  EDD_handle,
								LSA_SYS_PTR_TYPE pSys)
{
	TCIP_LOG_FCT("tcip_initEddSendRQBList")
	LSA_UINT32 i;

	*SendRqbs = LSA_NULL;    /* there is no one up to now! */

	for (i = 0; i <Count; i++)
	{

		EDD_UPPER_RQB_PTR_TYPE pRQB = tcip_AllocRQB( pSys, sizeof(EDD_RQB_NRT_SEND_TYPE));

		if( is_null(pRQB) )
		{
			TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"allocation of lower RQB failed check send-RQB configuration");
			TCIP_FATAL();
		}
		else
		{
			EDD_UPPER_NRT_SEND_PTR_TYPE pParam;
			pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);

#if TCIP_CFG_COPY_ON_SEND
			{
			void * pFrame;
			unsigned short sendBufferSize = 0;

#if TCIP_CFG_INTERNICHE
			sendBufferSize = TCIP_SENDBUFFER_SIZE;
#elif TCIP_CFG_OPEN_BSD
			sendBufferSize = OBSD_PNIO_SENDBUFFER_SIZE;
#endif

			TCIP_EDD_ALLOC_TX_TRANSFER_BUFFER_MEM(&pFrame, sendBufferSize, pSys);
			TCIP_ASSERT( pFrame != LSA_NULL );

			pParam->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)pFrame;
			}
#else
			pParam->pBuffer = LSA_NULL;
#endif

			TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_SEND);
			TCIP_EDD_SET_HANDLE( pRQB, 0);
			EDD_RQB_SET_HANDLE_LOWER(pRQB, EDD_handle);

			tcip_putEddSendRQB(SendRqbs,pRQB);
		}
	}
}


/*===========================================================================*/

/*
 * tcip_cancel_send_resources()
 * returns all queued PAKETS to the free pkt-list
 */
LSA_VOID tcip_cancel_send_resources(TCIP_CHANNEL_CONST_PTR_TYPE channel)
{
	EDD_RQB_TYPE *pRQB;

/* Task 1560385 (use if_sendq) -- TCIP-EDD-Send-Queue only for Interniche */
#if TCIP_CFG_INTERNICHE
	PACKET  pkt;
	while((pkt = tcip_get_pkt(&(channel->edd.send_pkts))) != LSA_NULL)
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);
	}
#endif
	while((pRQB = tcip_getEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs))) != LSA_NULL)
	{
		tcip_FreeSndBuf(channel, pRQB);
	}
}

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_INT_CFG_TRACE_ON
#if TCIP_CFG_INTERNICHE
#define UDP_PKT 0x1
#define TCP_PKT 0x2
#endif
#endif

/*lint -e{818} symbol channel Pointer parameter 'Symbol' (Location) could be declared ptr to const */
static LSA_VOID tcip_edd_nrt_send (TCIP_CHANNEL_CONST_PTR_TYPE  channel, PACKET  pkt, EDD_UPPER_RQB_PTR_TYPE  pRQB)
{
	TCIP_LOG_FCT ("tcip_edd_nrt_send")
	EDD_UPPER_NRT_SEND_PTR_TYPE pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);
	int        sendLen;
	LSA_UINT32 txFrmGroup;

#if TCIP_INT_CFG_TRACE_ON && TCIP_CFG_INTERNICHE
	unsigned char proto_flags = 0;
#endif
#if TCIP_INT_CFG_TRACE_ON && TCIP_CFG_OPEN_BSD
	LSA_UINT64 timestamp;
	unsigned char protocol_flags = 0;
#endif


#if TCIP_CFG_INTERNICHE
	LSA_UINT16 ethType;

	{
	LSA_UINT8  *pSendBuf = (LSA_UINT8 *)(pkt->nb_prot  + ETHHDR_BIAS);
	LSA_UINT16 IpTp = IPTP;
	sendLen    = (int)pkt->nb_plen - ETHHDR_BIAS;

	ethType = (LSA_UINT16)ET_TYPE_GET(pSendBuf);

	if ( ethType == htons(IpTp) ) /* IP message */
	{
		u_char ipProt = ((struct ip*)(pkt->nb_prot+ETHHDR_SIZE))->ip_prot;

		if(ipProt == UDP_PROT)
		{
#if TCIP_INT_CFG_TRACE_ON
			proto_flags |= UDP_PKT;
#endif
			txFrmGroup = EDD_NRT_TX_GRP_IP_UDP;
		}
		else if (ipProt == TCPTP)
		{
#if TCIP_INT_CFG_TRACE_ON
			proto_flags |= TCP_PKT;
#endif
			txFrmGroup = EDD_NRT_TX_GRP_IP_TCP;
		}
		else /* ICMP or something else */
		{
			txFrmGroup = EDD_NRT_TX_GRP_IP_ICMP;
		}
	}
	else /* ARP or something else */
	{
		txFrmGroup = EDD_NRT_TX_GRP_ARP;
	}

#if TCIP_INT_CFG_TRACE_ON
	if (proto_flags & (TCP_PKT|UDP_PKT))
	{
		controller_add_tcip_send_measure_1(controller_get_time()); /* (!) before pkt copy into send buf */
	}
#endif

	{

#if TCIP_CFG_COPY_ON_SEND
	TCIP_ASSERT( pParam->pBuffer != LSA_NULL );

	if( sendLen > TCIP_SENDBUFFER_SIZE ) /* do not send */
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "sendLen exceeds TCIP_SENDBUFFER_SIZE: %d", sendLen);
		TCIP_FATAL();
	}
	else
	{
		TCIP_MEMCPY( pParam->pBuffer, pSendBuf, (LSA_UINT)sendLen);
	}
#else
	pParam->pBuffer    = pSendBuf;
#endif
	}
#endif

#if TCIP_CFG_OPEN_BSD
	{
	LSA_UINT8 *sendBuf = pParam->pBuffer;

#if !TCIP_INT_CFG_TRACE_ON
	unsigned char protocol_flags = 0;
#endif

	if (is_null(sendBuf))
	{
		TCIP_FATAL();
	}

#if TCIP_INT_CFG_TRACE_ON
	timestamp = controller_get_time();
#endif

	sendLen = obsd_pnio_copy_pkt_to_sendbuffer(pkt, (char *)sendBuf);
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "mbuf 0x%x was copied and de-allocated", pkt);

	// TODO should check sendLen (mostly harmless, buffer exists)

	protocol_flags = (unsigned char)obsd_pnio_get_protocol_flags((char *)sendBuf);

	if ((protocol_flags & OBSD_PNIO_IP) != 0) /* IP message */
	{
		if((protocol_flags & OBSD_PNIO_UDP) != 0)

		{
			TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "UDP packet to send");
			txFrmGroup = EDD_NRT_TX_GRP_IP_UDP;
		}
		else if ((protocol_flags & OBSD_PNIO_TCP) != 0)
		{
			TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "TCP packet to send");
			txFrmGroup = EDD_NRT_TX_GRP_IP_TCP;
		}
		else /* ICMP or something else */
		{
			TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ICMP or other IP packet to send");
			txFrmGroup = EDD_NRT_TX_GRP_IP_ICMP;
		}
	}
	else /* ARP or something else */
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "ARP packet to send");
		txFrmGroup = EDD_NRT_TX_GRP_ARP;
	}
#endif

	/***/

	pParam->Length = (LSA_UINT32)sendLen;
	pParam->PortID = EDD_PORT_ID_AUTO;
	pParam->Priority = EDD_NRT_SEND_PRIO_0; /* for EDDI and EDDS, ignored by EDDP */
	pParam->TxFrmGroup = txFrmGroup;  /* for EDDP, ignored by EDDI and EDDS */
	}

	/***/
#if TCIP_CFG_INTERNICHE
	TCIP_EDD_SET_USER_ID_PTR(pRQB, pkt);
#endif
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "RQB 0x%x", pRQB);

#if TCIP_INT_CFG_TRACE_ON
#if TCIP_CFG_OPEN_BSD
	if (protocol_flags & (OBSD_PNIO_UDP|OBSD_PNIO_TCP))
	{
		controller_add_tcip_send_measure_1(timestamp);
		controller_add_tcip_send_measure_2(); /* (!) after pkt copy into send buf, before req EDD */
	}
#endif
#if TCIP_CFG_INTERNICHE
	if (protocol_flags & (TCP_PKT|UDP_PKT))
	{
		controller_add_tcip_send_measure_2();
	}
#endif
#endif

	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);


	return;
}

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_CFG_INTERNICHE

int tcip_iniche_pkt_send(PACKET pkt) /* InterNiche callback */
{
	TCIP_LOG_FCT ("tcip_iniche_pkt_send")
	TCIP_CHANNEL_PTR_TYPE   channel;

	channel = (TCIP_CHANNEL_PTR_TYPE)pkt->net->n_local;

	if( is_null(channel) )
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);

		return ENP_SENDERR;
	}
	else if( channel->state != TCIP_CH_OPEN ) /* late */
	{
		LOCK_NET_RESOURCE(FREEQ_RESID);
		pk_free(pkt);
		UNLOCK_NET_RESOURCE(FREEQ_RESID);

		return ENP_SENDERR;
	}
	else
	{
		EDD_UPPER_RQB_PTR_TYPE  pRQB;

		/*
		 *  get send RQB from channel's rqb pool
		 */
		pRQB = tcip_getEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs));

		/*
		 * if there is currently no free rqb pass packet to pktSendQueue of destination channel
		 */
		if( is_null(pRQB) )
		{
			LSA_BOOL inserted = tcip_put_pkt(&channel->edd.send_pkts, pkt );

			if( !inserted ) /* queue has maximum length -> insertion failed, see 973233 */
			{
				TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "packet queue maximum reached, cannot send!");

				LOCK_NET_RESOURCE(FREEQ_RESID);
				pk_free(pkt);
				UNLOCK_NET_RESOURCE(FREEQ_RESID);

				return ENP_SENDERR;
			}
		}
		else
		{
			tcip_edd_nrt_send(channel, pkt, pRQB);
		}
		return 0; /* ok */
	}
}

#endif

/*===========================================================================*/

/* Task 1560385 (use if_sendq) -- OpenBSD routines */
#if TCIP_CFG_OPEN_BSD
void *tcip_obsd_get_rqb(OBSD_IFNET ifp)
{
	TCIP_CHANNEL_PTR_TYPE channel = (TCIP_CHANNEL_PTR_TYPE)obsd_pnio_get_tcip_channel(ifp);

	if(is_null(channel))
	{
		return LSA_NULL;
	}
	else if(channel->state != TCIP_CH_OPEN) /* late */
	{
		return LSA_NULL;
	}
	else
	{
		EDD_UPPER_RQB_PTR_TYPE  pRQB;
		pRQB = tcip_getEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs));
		return (void *)pRQB;
	}
}

void tcip_obsd_put_rqb(void *channel, void *pRQB)
{
	tcip_putEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(((TCIP_CHANNEL_PTR_TYPE)channel)->edd.send_rqbs), (EDD_RQB_TYPE *)pRQB);
}

void tcip_obsd_pkt_send(const void *channel, PACKET pkt, void *pRQB) /* OBSD send routine */
{
	tcip_edd_nrt_send((TCIP_CHANNEL_PTR_TYPE)channel, pkt, (EDD_UPPER_RQB_PTR_TYPE)pRQB);
}

LSA_VOID tcip_edd_nrt_send_done(TCIP_EDD_LOWER_RQB_CONST_PTR_TYPE pRQB, TCIP_CONST_CHANNEL_CONST_PTR_TYPE channel)
{
	if (TCIP_CH_OPEN == channel->state)
	{
		OBSD_IFNET ifp = (OBSD_IFNET)channel->p_if_data->port[0].ifp;
		obsd_send_next_packet(ifp, (void *)pRQB);

#if !TCIP_INT_CFG_SOCKET_EVENTS
		TCIP_TRIGGER_SELECT();
#else
		TCIP_DO_SOCKET_EVENTS(); /* if IP Stack sent TCP or UDP data, let SOCK examine the event list */
#endif
	}
	else /* return Rqb to free Rqb list; NOTE: EDD send resources are cancelled before calling back */
	{
		tcip_obsd_put_rqb((void *)channel, (void *)pRQB);
	}
}

#endif

#if TCIP_CFG_INTERNICHE

LSA_VOID tcip_edd_nrt_send_done( TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB, TCIP_CHANNEL_CONST_PTR_TYPE channel )
{
	TCIP_LOG_FCT ("tcip_edd_nrt_send_done")

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "pRQB: 0x%x, handle 0x%x", pRQB, TCIP_EDD_GET_HANDLE(pRQB));

#if TCIP_CFG_COPY_ON_SEND == 0
	{
	EDD_UPPER_NRT_SEND_PTR_TYPE  pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);
	pParam->pBuffer = LSA_NULL;
	}
#endif

	{
	PACKET pkt = (PACKET) TCIP_RQB_GET_USER_ID_PTR(pRQB);

	LOCK_NET_RESOURCE(FREEQ_RESID);
	pk_free(pkt);
	UNLOCK_NET_RESOURCE(FREEQ_RESID);
	}

	TCIP_EDD_SET_USER_ID_PTR(pRQB, LSA_NULL);

	/***/

	if( TCIP_CH_OPEN == channel->state )
	{
		PACKET pkt = tcip_get_pkt(&(channel->edd.send_pkts));

		if(is_null(pkt)) /* no more packets queued: return Rqb to free Rqb list! */
		{
			tcip_putEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs), pRQB);
		}
		else /* one more packet to send */
		{
			tcip_edd_nrt_send(channel, pkt, pRQB);
		}
#if !TCIP_INT_CFG_SOCKET_EVENTS
		TCIP_TRIGGER_SELECT();
#else
		TCIP_DO_SOCKET_EVENTS(); /* if IP Stack sent TCP or UDP data, let SOCK examine the event list */
#endif
	}
	else /* return Rqb to free Rqb list; NOTE: EDD send resources are cancelled before calling back */
	{
		tcip_putEddSendRQB((EDD_UPPER_RQB_PTR_TYPE *)&(channel->edd.send_rqbs), pRQB);
	}
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
