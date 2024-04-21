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
/*  F i l e               &F: tcip_recv.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NRT receive functionality                                                */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  5
#define TCIP_MODULE_ID     5

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

#if TCIP_INT_CFG_TRACE_ON

static void controller_add_recv_measure_nrt_recv_finished(void);
static void controller_add_recv_point_3(LSA_UINT64 timestamp);

static void controller_add_recv_measure_6(LSA_UINT64 timestamp); /* before do socket events */
static void controller_add_recv_measure_7(); /* after do socket events.  */
static void controller_add_recv_measure_8(); /* after obsd_snmpd_handle_events */

typedef struct statistic_values_type_tag
{
	/* statistic values to calculate: */

	/* delta between point 1 and 2 */
	LSA_UINT64 d_1_2_min;
	LSA_UINT64 d_1_2_max;
	LSA_UINT64 d_1_2_mean;

	/* delta between point 2 (TCIP) and 3 (in OBSD before softintr_dispatch) */
	LSA_UINT64 d_2_3_min;
	LSA_UINT64 d_2_3_max;
	LSA_UINT64 d_2_3_mean;

	/* delta between point 3 between softintr_dispatch and SOCK */
	LSA_UINT64 d_3_4_min;
	LSA_UINT64 d_3_4_max;
	LSA_UINT64 d_3_4_mean;

	/* delta between point 4 (SOCK has data) and 5 (SOCK delivers data to user) */
	LSA_UINT64 d_4_5_min;
	LSA_UINT64 d_4_5_max;
	LSA_UINT64 d_4_5_mean;

	/* delta between 1 and 5 calculated when number_of_packets reaches CONTROLLER_MAX_MEASURES */
	LSA_UINT64 d_all_min;
	LSA_UINT64 d_all_max;
	LSA_UINT64 d_all_mean;

	/* values for receive path: delta between 1 and nrt_recv_finished */
	LSA_UINT64 d_recv_min;
	LSA_UINT64 d_recv_max;
	LSA_UINT64 d_recv_mean;

	LSA_UINT64 d_6_7_min;
	LSA_UINT64 d_6_7_max;
	LSA_UINT64 d_6_7_mean;

	LSA_UINT64 d_7_8_min;
	LSA_UINT64 d_7_8_max;
	LSA_UINT64 d_7_8_mean;

	LSA_UINT64 d_8_recv_min;
	LSA_UINT64 d_8_recv_max;
	LSA_UINT64 d_8_recv_mean;

	LSA_UINT64 d_101_102_min;
	LSA_UINT64 d_101_102_max;
	LSA_UINT64 d_101_102_mean;

} controller_statistic_values_struct;

static controller_statistic_values_struct controller_stat;

/* 4 recv measures */
static controller_measure_struct controller_recv_measure_1; /* TCIP gets packet from EDD */
static controller_measure_struct controller_recv_measure_2; /* TCIP invokes ether_input() */
static controller_measure_struct controller_recv_measure_3; /* point in netintr() */
static controller_measure_struct controller_recv_measure_4; /* before copy into socket buffer */
static controller_measure_struct controller_recv_measure_5; /* after copy; SOCK is notified data is present in socket buffer */
static controller_dropped_struct controller_recv_drops;			/* number of drops */
static controller_measure_struct controller_recv_nrt_recv_finished; /* receive path finished */
static controller_measure_struct controller_recv_measure_6;
static controller_measure_struct controller_recv_measure_7;
static controller_measure_struct controller_recv_measure_8;
static controller_measure_struct controller_recv_measure_101;
static controller_measure_struct controller_recv_measure_102;
static controller_recv_success_struct controller_recv_succ;		/* record number of successful recv system calls: read from socket was successful */

static LSA_VOID controller_calc_recv_statistics(controller_statistic_values_struct* p_controller_stat);
static LSA_VOID controller_print_recv_statistics(controller_statistic_values_struct* p_controller_stat);

static int g_controller_recv_count = -1; /* global counter for all measure points */
static int g_controller_read_count = 0; /* global counter for all successful recv system calls */

void controller_init_recv_measures()
{
	int i;

	g_controller_recv_count = 0; /* reset everything */
	g_controller_read_count = 0;
	controller_recv_drops.drop_count = 0;
	controller_recv_nrt_recv_finished.count = 0;

	for (i=0; i<CONTROLLER_MAX_MEASURES; i++) /* sanity */
	{
		controller_recv_measure_1.m[i] = 0;
		controller_recv_measure_2.m[i] = 0;
		controller_recv_measure_3.m[i] = 0;
		controller_recv_measure_4.m[i] = 0;
		controller_recv_measure_5.m[i] = 0;
		controller_recv_measure_6.m[i] = 0;
		controller_recv_measure_7.m[i] = 0;
		controller_recv_measure_8.m[i] = 0;
		controller_recv_drops.dropped[i] = 0;
		controller_recv_succ.success[i] = 0;
		controller_recv_measure_101.m[i] = 0;
		controller_recv_measure_102.m[i] = 0;
		controller_recv_nrt_recv_finished.m[i] = 0;
	}
}

LSA_UINT64 controller_get_time(LSA_VOID)
{
	LSA_UINT64 time_now;

	TCIP_GET_TIMESTAMP(&time_now);

	return time_now;
}

void controller_add_recv_measure_1(void)
{
	controller_recv_measure_1.m[g_controller_recv_count] = controller_get_time();
}

void controller_add_recv_measure_2(void)
{
#if TCIP_CFG_INTERNICHE
	LSA_UINT64 timestamp = controller_get_time();
	controller_recv_measure_2.m[g_controller_recv_count] = timestamp;
#else
	controller_recv_measure_2.m[g_controller_recv_count] = controller_get_time();
#endif

#if TCIP_CFG_INTERNICHE
/* interniche never triggers measure 3. Fill this point pro forma */
	controller_add_recv_point_3(timestamp); /* (!) InterNiche only: point 3 has the same timestamp as point 2 */
#endif


}

void controller_add_recv_point_3(LSA_UINT64 timestamp)
{
	controller_recv_measure_3.m[g_controller_recv_count] = timestamp;
}

void controller_add_recv_measure_3(void)
{
	controller_add_recv_point_3(controller_get_time());
}

void controller_add_recv_measure_4(void)
{
	controller_recv_measure_4.m[g_controller_recv_count] = controller_get_time();
}

void controller_add_recv_timestamp_4(LSA_UINT64 timestamp)
{
	controller_recv_measure_4.m[g_controller_recv_count] = timestamp;
}

void controller_add_recv_measure_5(int dropped)
{
	controller_recv_drops.dropped[g_controller_recv_count] = dropped;

	if (dropped)
	{
		/* record for stat: do not calc MIN between 4 and 5 if dropped is 1 */
		controller_recv_drops.drop_count ++;
	}

	controller_recv_measure_5.m[g_controller_recv_count] = controller_get_time();
}

/* before do socket events */
void controller_add_recv_measure_6(LSA_UINT64 timestamp)
{
	controller_recv_measure_6.m[g_controller_recv_count] = timestamp;
}

/* after do socket events  */
void controller_add_recv_measure_7()
{
	controller_recv_measure_7.m[g_controller_recv_count] = controller_get_time();
}

void controller_add_recv_measure_8()
{
	controller_recv_measure_8.m[g_controller_recv_count] = controller_get_time();
}

void controller_add_recv_measure_nrt_recv_finished(void)
{
	controller_recv_nrt_recv_finished.m[g_controller_recv_count] = controller_get_time();

	g_controller_recv_count++;

	/* buffer full - print stats */
	if (g_controller_recv_count == CONTROLLER_MAX_MEASURES) /* cannot record timestamp */
	{
		controller_calc_recv_statistics(&controller_stat);

		controller_print_recv_statistics(&controller_stat); /* print everything up to measure_4.count */

		controller_init_recv_measures(); /* reset everything */
	}
}

/* before recv system call */
void controller_add_recv_measure_101(LSA_UINT64 timestamp)
{
	if (g_controller_read_count == CONTROLLER_MAX_MEASURES)
	{
		g_controller_read_count = 0; /* sanity */
	}

	controller_recv_measure_101.m[g_controller_read_count] = timestamp;
}

/* after recv system call */
void controller_add_recv_measure_102(void) /* caution: 101 and 102 must be both called sequentially */
{
	controller_recv_measure_102.m[g_controller_read_count] = controller_get_time();

	g_controller_read_count++;
}

LSA_VOID controller_calc_recv_statistics(controller_statistic_values_struct* p_controller_stat)
{
	int i;

	LSA_UINT64 d_1_2_sum = 0;
#if TCIP_CFG_OPEN_BSD
	LSA_UINT64 d_2_3_sum = 0;
#endif
	LSA_UINT64 d_3_4_sum = 0;
	LSA_UINT64 d_4_5_sum = 0;
	LSA_UINT64 d_6_7_sum = 0;
	LSA_UINT64 d_7_8_sum = 0;
	LSA_UINT64 d_8_recv_sum = 0;
	LSA_UINT64 d_all_sum = 0;
	LSA_UINT64 d_recv_sum = 0;
	LSA_UINT64 d_101_102_sum = 0;

	/* reset delta min, max and mean vlaues */
	p_controller_stat->d_1_2_min  = 0xffffffff; /* 32 Bit UINT MAX should be sufficent */
	p_controller_stat->d_1_2_max  = 0;
	p_controller_stat->d_1_2_mean = 0;
	/* delta between point 2 and 3 */
	p_controller_stat->d_2_3_min = 0xffffffff;
	p_controller_stat->d_2_3_max  = 0;
	p_controller_stat->d_2_3_mean = 0;
	/* delta between point 3 and 4 */
	p_controller_stat->d_3_4_min  = 0xffffffff;
	p_controller_stat->d_3_4_max  = 0;
	p_controller_stat->d_3_4_mean = 0;
	/* delta between point 4 and 5 */
	p_controller_stat->d_4_5_min  = 0xffffffff;
	p_controller_stat->d_4_5_max  = 0;
	p_controller_stat->d_4_5_mean = 0;

	/* values calculated when number_of_packets reaches CONTROLLER_MAX_MEASURES */
	p_controller_stat->d_all_min  = 0xffffffff;
	p_controller_stat->d_all_max  = 0;
	p_controller_stat->d_all_mean = 0;
	/*  */
	p_controller_stat->d_recv_min = 0xffffffff;
	p_controller_stat->d_recv_max = 0;
	p_controller_stat->d_recv_mean = 0;

	/* delta between point 6 and 7 */
	p_controller_stat->d_6_7_min  = 0xffffffff;
	p_controller_stat->d_6_7_max  = 0;
	p_controller_stat->d_6_7_mean = 0;	;

	/* delta between point 7 and 8 */
	p_controller_stat->d_7_8_min  = 0xffffffff;
	p_controller_stat->d_7_8_max  = 0;
	p_controller_stat->d_7_8_mean = 0;

	/* delta between point 8 and recv (contains reprovide for BSD) */
	p_controller_stat->d_8_recv_min  = 0xffffffff;
	p_controller_stat->d_8_recv_max  = 0;
	p_controller_stat->d_8_recv_mean = 0;

	/* delta between "upper" point 101 and 102 */
	p_controller_stat->d_101_102_min  = 0xffffffff;
	p_controller_stat->d_101_102_max =  0;
	p_controller_stat->d_101_102_mean = 0;

	/* first step: differences an mean values for measure1 to measure 5 (all data available) */
	for (i=0; i<g_controller_recv_count; i++)
	{
		LSA_UINT64 d_1_2 = 0;
		LSA_UINT64 d_2_3 = 0;
		LSA_UINT64 d_3_4 = 0;
		LSA_UINT64 d_4_5 = 0;
		LSA_UINT64 d_all = 0;
		LSA_UINT64 d_recv = 0;
		LSA_UINT64 d_6_7 = 0;
		LSA_UINT64 d_7_8 = 0;
		LSA_UINT64 d_8_recv = 0;

		/* record 1st delta: edd -> TCIP */
		if (controller_recv_measure_2.m[i])
		{
			d_1_2  = controller_recv_measure_2.m[i] - controller_recv_measure_1.m[i];
		}
#if TCIP_CFG_INTERNICHE
		d_2_3 = 0;
#else
		if (controller_recv_measure_3.m[i] && controller_recv_measure_2.m[i])
		{
			d_2_3  = controller_recv_measure_3.m[i] - controller_recv_measure_2.m[i];
		}
#endif
		if (controller_recv_measure_4.m[i] && controller_recv_measure_3.m[i])
		{
			d_3_4  = controller_recv_measure_4.m[i] - controller_recv_measure_3.m[i];
		}

		if (controller_recv_measure_5.m[i] && controller_recv_measure_4.m[i])
		{
			d_4_5  = controller_recv_measure_5.m[i] - controller_recv_measure_4.m[i];
		}

		if (controller_recv_measure_5.m[i] && controller_recv_measure_1.m[i])
		{
			d_all  = controller_recv_measure_5.m[i] - controller_recv_measure_1.m[i];
		}

		if (controller_recv_nrt_recv_finished.m[i] && controller_recv_measure_1.m[i])
		{
			d_recv = controller_recv_nrt_recv_finished.m[i] - controller_recv_measure_1.m[i];
		}

		if (controller_recv_measure_7.m[i] && controller_recv_measure_6.m[i])
		{
			d_6_7  = controller_recv_measure_7.m[i] - controller_recv_measure_6.m[i];
		}

		if (controller_recv_measure_8.m[i] && controller_recv_measure_7.m[i])
		{
			d_7_8  = controller_recv_measure_8.m[i] - controller_recv_measure_7.m[i];
		}

		if (controller_recv_nrt_recv_finished.m[i] && controller_recv_measure_8.m[i])
		{
			d_8_recv = controller_recv_nrt_recv_finished.m[i] - controller_recv_measure_8.m[i];
		}

		if (d_1_2) CONTROLLER_MIN(p_controller_stat->d_1_2_min, d_1_2);
#if TCIP_CFG_OPEN_BSD
		if (d_2_3) CONTROLLER_MIN(p_controller_stat->d_2_3_min, d_2_3);
#else
		p_controller_stat->d_2_3_min = 0;
#endif
		if (d_3_4) CONTROLLER_MIN(p_controller_stat->d_3_4_min, d_3_4);

		if(!controller_recv_drops.dropped[i]) /* do not look for MIN if dropped (times are close to 0 then) */
		{
			CONTROLLER_MIN(p_controller_stat->d_4_5_min, d_4_5);
		}

		if (d_all) CONTROLLER_MIN(p_controller_stat->d_all_min, d_all);
		if (d_recv) CONTROLLER_MIN(p_controller_stat->d_recv_min, d_recv);
		if (d_6_7) CONTROLLER_MIN(p_controller_stat->d_6_7_min, d_6_7);
		if (d_7_8) CONTROLLER_MIN(p_controller_stat->d_7_8_min, d_7_8);
		if (d_8_recv) CONTROLLER_MIN(p_controller_stat->d_8_recv_min, d_8_recv);

		if (d_1_2) CONTROLLER_MAX(p_controller_stat->d_1_2_max, d_1_2);
#if TCIP_CFG_OPEN_BSD
		if (d_2_3) CONTROLLER_MAX(p_controller_stat->d_2_3_max, d_2_3);
#else
		p_controller_stat->d_2_3_max = 0;
#endif
		if (d_3_4) CONTROLLER_MAX(p_controller_stat->d_3_4_max, d_3_4);

		if(!controller_recv_drops.dropped[i]) /* do not look for MAX if dropped (times are cose to 0 then) */
		{
			if (d_4_5) CONTROLLER_MAX(p_controller_stat->d_4_5_min, d_4_5);
		}

		if (d_all) CONTROLLER_MAX(p_controller_stat->d_all_max, d_all);
		if (d_recv) CONTROLLER_MAX(p_controller_stat->d_recv_max, d_recv);
		if (d_6_7) CONTROLLER_MAX(p_controller_stat->d_6_7_max, d_6_7);
		if (d_7_8) CONTROLLER_MAX(p_controller_stat->d_7_8_max, d_7_8);
		if (d_8_recv) CONTROLLER_MAX(p_controller_stat->d_8_recv_max, d_8_recv);

		/* record for mean value calculation, see below */
		d_1_2_sum  += d_1_2;
#if TCIP_CFG_OPEN_BSD
		d_2_3_sum  += d_2_3;
#endif
		d_3_4_sum  += d_3_4;

		if(!controller_recv_drops.dropped[i]) /* only if no drop */
		{
			d_4_5_sum  += d_4_5;
		}
		d_all_sum  += d_all;
		d_recv_sum += d_recv;
		d_6_7_sum  += d_6_7;
		d_7_8_sum  += d_7_8;
		d_8_recv_sum += d_8_recv;
	}

	for (i=0; i<g_controller_read_count; i++)
	{
		LSA_UINT64 d_101_102 = controller_recv_measure_102.m[i] - controller_recv_measure_101.m[i];

		CONTROLLER_MIN(p_controller_stat->d_101_102_min, d_101_102);

		CONTROLLER_MAX(p_controller_stat->d_101_102_max, d_101_102);

		d_101_102_sum += d_101_102;
	}

	/* calculate mean values (only range up to measure_4 available) */
	if (g_controller_recv_count)
	{
		p_controller_stat->d_1_2_mean  = d_1_2_sum / g_controller_recv_count;
	#if TCIP_CFG_OPEN_BSD
		p_controller_stat->d_2_3_mean  = d_2_3_sum / g_controller_recv_count;
	#else
		p_controller_stat->d_2_3_mean = 0;
	#endif
		p_controller_stat->d_3_4_mean  = d_3_4_sum / g_controller_recv_count;
		p_controller_stat->d_4_5_mean  = d_4_5_sum / (g_controller_recv_count - controller_recv_drops.drop_count); /* drops do not contribute to mean */
		p_controller_stat->d_all_mean  = d_all_sum / g_controller_recv_count;
		p_controller_stat->d_recv_mean = d_recv_sum / g_controller_recv_count; /* secure since in one function */
		p_controller_stat->d_6_7_mean  = d_6_7_sum / (g_controller_recv_count);
		p_controller_stat->d_7_8_mean  = d_7_8_sum / g_controller_recv_count;
		p_controller_stat->d_8_recv_mean = d_8_recv_sum / g_controller_recv_count;
	}
	if (g_controller_read_count)
	{
		p_controller_stat->d_101_102_mean = d_101_102_sum / g_controller_read_count;
	}
	else
	{
		p_controller_stat->d_6_7_mean = 0;
	}
}

LSA_VOID controller_print_recv_statistics(controller_statistic_values_struct* p_controller_stat)
{
	TCIP_LOG_FCT("controller_print_recv_statistics")


	/* print statistic on trace level FATAL */
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: recv statistic:               g_controller_recv_count:%8u timestamps", g_controller_recv_count);
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: recv statistic:                               :%8u drops", controller_recv_drops.drop_count);
#if TCIP_CFG_OPEN_BSD
	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "instrumentation: recv statistic:                               :%8u maximum size of event list", event_list_size_max);
#endif

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  1->2 Min: %8u us,  1->2 Max: %8u us,  1->2 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_1_2_min, (LSA_UINT32)p_controller_stat->d_1_2_max, (LSA_UINT32)p_controller_stat->d_1_2_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  2->3 Min: %8u us,  2->3 Max: %8u us,  2->3 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_2_3_min, (LSA_UINT32)p_controller_stat->d_2_3_max, (LSA_UINT32)p_controller_stat->d_2_3_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  3->4 Min: %8u us,  3->4 Max: %8u us,  3->4 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_3_4_min, (LSA_UINT32)p_controller_stat->d_3_4_max, (LSA_UINT32)p_controller_stat->d_3_4_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  4->5 Min: %8u us,  4->5 Max: %8u us,  4->5 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_4_5_min, (LSA_UINT32)p_controller_stat->d_4_5_max, (LSA_UINT32)p_controller_stat->d_4_5_mean
		);

	/* all (total time edd->user) Min Max Mean */
	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic: Total Min: %8u us, Total Max: %8u us, Total Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_all_min, (LSA_UINT32)p_controller_stat->d_all_max, (LSA_UINT32)p_controller_stat->d_all_mean
		);

	/* recv (total time nrt recv done -> reqprovide) Min Max Mean */
	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic: Recv  Min: %8u us, Recv  Max: %8u us, Recv  Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_recv_min, (LSA_UINT32)p_controller_stat->d_recv_max, (LSA_UINT32)p_controller_stat->d_recv_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  6->7 Min: %8u us,  6->7 Max: %8u us,  6->7 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_6_7_min, (LSA_UINT32)p_controller_stat->d_6_7_max, (LSA_UINT32)p_controller_stat->d_6_7_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:  7->8 Min: %8u us,  7->8 Max: %8u us,  7->8 Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_7_8_min, (LSA_UINT32)p_controller_stat->d_7_8_max, (LSA_UINT32)p_controller_stat->d_7_8_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:8-recv Min: %8u us,8-recv Max: %8u us,8-recv Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_8_recv_min, (LSA_UINT32)p_controller_stat->d_8_recv_max, (LSA_UINT32)p_controller_stat->d_8_recv_mean
		);

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "instrumentation: recv statistic:101-102 Min:%8u us,101-102Max: %8u us,101-102Mean: %8u us"
		, (LSA_UINT32)p_controller_stat->d_101_102_min, (LSA_UINT32)p_controller_stat->d_101_102_max, (LSA_UINT32)p_controller_stat->d_101_102_mean
		);

#ifdef PNTEST_BUILD_WINDOWS
		printf("instrumentation: recv statistic:               g_controller_recv_count:%8u timestamps\n", g_controller_recv_count);
		printf("instrumentation: recv statistic:                               :%8u drops\n", controller_recv_drops.drop_count);
#if TCIP_CFG_OPEN_BSD
		printf("instrumentation: recv statistic:                               :%8u maximum size of event list\n", event_list_size_max);
#endif

		/* edd_obsd Min Max Mean */

		printf("instrumentation: recv statistic:  1->2 Min:%8llu us,   1->2 Max:%8llu us,   1->2 Mean:%8llu us\n"
			, p_controller_stat->d_1_2_min, p_controller_stat->d_1_2_max, p_controller_stat->d_1_2_mean
			);

		printf("instrumentation: recv statistic:  2->3 Min:%8llu us,   2->3 Max:%8llu us,   2->3 Mean:%8llu us\n"
			, p_controller_stat->d_2_3_min, p_controller_stat->d_2_3_max, p_controller_stat->d_2_3_mean
			);

		printf("instrumentation: recv statistic:  3->4 Min:%8llu us,   3->4 Max:%8llu us,   3->4 Mean:%8llu us\n"
			, p_controller_stat->d_3_4_min, p_controller_stat->d_3_4_max, p_controller_stat->d_3_4_mean
			);

		printf("instrumentation: recv statistic:  4->5 Min:%8llu us,   4->5 Max:%8llu us,   4->5 Mean:%8llu us\n"
			, p_controller_stat->d_4_5_min, p_controller_stat->d_4_5_max, p_controller_stat->d_4_5_mean
			);

		printf("instrumentation: recv statistic: Total Min:%8llu us,  Total Max:%8llu us,  Total Mean:%8llu us\n"
			, p_controller_stat->d_all_min, p_controller_stat->d_all_max, p_controller_stat->d_all_mean
			);

		printf("instrumentation: recv statistic: Recv  Min:%8llu us,  Recv  Max:%8llu us,  Recv  Mean:%8llu us\n"
			, p_controller_stat->d_recv_min, p_controller_stat->d_recv_max, p_controller_stat->d_recv_mean
			);

		printf("instrumentation: recv statistic:  6->7 Min:%8llu us,   6->7 Max:%8llu us,   6->7 Mean:%8llu us\n"
			, p_controller_stat->d_6_7_min, p_controller_stat->d_6_7_max, p_controller_stat->d_6_7_mean
			);

		printf("instrumentation: recv statistic:  7->8 Min:%8llu us,   7->8 Max:%8llu us,   7->8 Mean:%8llu us\n"
			, p_controller_stat->d_7_8_min, p_controller_stat->d_7_8_max, p_controller_stat->d_7_8_mean
			);

		printf("instrumentation: recv statistic:8-recv Min:%8llu us, 8-recv Max:%8llu us, 8-recv Mean:%8llu us\n"
			, p_controller_stat->d_8_recv_min, p_controller_stat->d_8_recv_max, p_controller_stat->d_8_recv_mean
			);

		printf("instrumentation: recv statistic:101-102Min:%8llu us, 101-102Max:%8llu us, 101-102Mean:%8llu us\n"
			, p_controller_stat->d_101_102_min, p_controller_stat->d_101_102_max, p_controller_stat->d_101_102_mean
			);
#endif

}

#endif

/*===========================================================================*/

#if TCIP_CFG_INTERNICHE

#include "ether.h"

#if ETHHDR_BIAS != 0
#error "ETHHDR_BIAS must be 0, EDD_SRV_NRT_RECV needs 4 byte alignment"
#endif

#if (ETHHDR_BIAS + EDD_IEEE_FRAME_LENGTH) > EDD_FRAME_BUFFER_LENGTH
#error "check this!"
#endif

#endif

#define TCIP_RECV_BUFFER_SIZE  EDD_FRAME_BUFFER_LENGTH /* see EDD_SRV_NRT_RECV */

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_CFG_INTERNICHE
LSA_VOID tcip_AllocRcvBuf(TCIP_CHANNEL_CONST_PTR_TYPE channel)
{
	EDD_UPPER_RQB_PTR_TYPE pRQB = tcip_AllocRQB( channel->sysptr, sizeof(EDD_RQB_NRT_RECV_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_FATAL(); /* a bug */
	}
	else
	{
		EDD_UPPER_MEM_PTR_TYPE pRxFrame;

		TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM(&pRxFrame, TCIP_RECV_BUFFER_SIZE, channel->sysptr);

		if (is_null(pRxFrame))
		{
			TCIP_FATAL();
		}
		else
		{

			EDD_UPPER_NRT_RECV_PTR_TYPE pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE );
			PACKET                      pkt;

			pParam->pBuffer = pRxFrame;
			pParam->RequestID  = 0; /* not used */

			pkt = (PACKET)NB_ALLOC(sizeof(*pkt)); /* compare to pk_init() */

			if( is_null(pkt) )
			{
				TCIP_FATAL(); /* a bug */
			}
			else
			{
				pkt->pRQB = pRQB; /* store the RQB in the packet */

				TCIP_EDD_SET_USER_ID_PTR(pRQB, pkt); /* store packet in the userID of the RQB */

				TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_RECV);
				TCIP_EDD_SET_HANDLE(pRQB, 0);
				EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

				tcip_edd_nrt_recv(pRQB, channel);
			}
		}
	}
}
#endif

#if TCIP_CFG_OPEN_BSD
LSA_VOID tcip_AllocRcvBuf(TCIP_CHANNEL_CONST_PTR_TYPE channel)
{
	TCIP_LOG_FCT("tcip_AllocRcvBuf")

	EDD_UPPER_RQB_PTR_TYPE pRQB = tcip_AllocRQB( channel->sysptr, sizeof(EDD_RQB_NRT_RECV_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, ">>> tcip_AllocRcvBuf failed"); /* pcLint */
		TCIP_FATAL();
	}
	else
	{
		EDD_UPPER_MEM_PTR_TYPE pRxFrame; /* Pointer to Rx-Frame memory. */
		TCIP_EDD_ALLOC_RX_TRANSFER_BUFFER_MEM(&pRxFrame, TCIP_RECV_BUFFER_SIZE, channel->sysptr);

		if (is_null(pRxFrame))
		{
			TCIP_FATAL();
		}
		else
		{
			EDD_UPPER_NRT_RECV_PTR_TYPE pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE );

			pParam->pBuffer = pRxFrame;
			pParam->RequestID  = 0; /* not used */
			pParam->RequestCnt = 0;

			TCIP_EDD_SET_USER_ID_PTR(pRQB, LSA_NULL); /* unused */

			TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_RECV);
			TCIP_EDD_SET_HANDLE( pRQB, 0);
			EDD_RQB_SET_HANDLE_LOWER( pRQB, channel->edd.handle_lower);

			tcip_edd_nrt_recv(pRQB, channel);
		}
	}
}
#endif

LSA_VOID tcip_FreeRcvBuf(TCIP_CONST_CHANNEL_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_TYPE  *pRQB)
{
	EDD_UPPER_NRT_RECV_PTR_TYPE     pParam = LSA_NULL;
	LSA_UINT16                      ret16_val;

#if TCIP_CFG_INTERNICHE
	{
	PACKET pkt = TCIP_RQB_GET_USER_ID_PTR(pRQB);

	NB_FREE(pkt);
	}
#endif

#if TCIP_CFG_OPEN_BSD
	TCIP_ASSERT(TCIP_RQB_GET_USER_ID_PTR(pRQB) == LSA_NULL);
#endif

	pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		TCIP_EDD_LOWER_MEM_PTR_TYPE pFrame;

		pFrame = pParam->pBuffer;

		if ( is_not_null(pFrame) )
		{
			TCIP_EDD_FREE_RX_TRANSFER_BUFFER_MEM(&ret16_val, pFrame, channel->sysptr);
			TCIP_ASSERT(ret16_val == LSA_RET_OK);
		}
	}

	tcip_FreeRQB(channel->sysptr, pRQB );
}

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID tcip_edd_nrt_recv_done ( TCIP_EDD_LOWER_RQB_TYPE  *pRQB, TCIP_CHANNEL_CONST_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_edd_nrt_recv_done")

#if TCIP_INT_CFG_TRACE_ON
	static int to_count = 0;
#endif

	LSA_UINT16 response = TCIP_EDD_GET_RESPONSE(pRQB);

	if (channel->edd.pending_rcv_reqests > 0)
	{
#if TCIP_CFG_INTERNICHE
		LOCK_NET_RESOURCE(RXQ_RESID);		/* Task 4123456 */
#endif
		channel->edd.pending_rcv_reqests --;
#if TCIP_CFG_INTERNICHE
		UNLOCK_NET_RESOURCE(RXQ_RESID);
#endif
	}
	else
	{
		TCIP_FATAL();
	}

	if( TCIP_CH_OPEN != channel->state ) /* closing */
	{
		TCIP_ASSERT(response == EDD_STS_OK || response == EDD_STS_OK_CANCEL);

		tcip_FreeRcvBuf( channel, pRQB);
	}
	else if( response != EDD_STS_OK )
	{
		TCIP_FATAL(); /* not as documented */
	}
	else /* ok */
	{
		EDD_UPPER_NRT_RECV_PTR_TYPE pRecvParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_RECV_PTR_TYPE);

#if TCIP_INT_CFG_TRACE_ON
	/* Messpunkt 1 (UDP or TCP Frame received from EDD) */
		if( channel->detailptr->path_type == TCIP_PATH_TYPE_UDP
			|| channel->detailptr->path_type == TCIP_PATH_TYPE_TCP)
		{
			to_count = 1;
			controller_add_recv_measure_1();
		}
		else
		{
			to_count = 0;
		}
#endif

		if( pRecvParam->FrameFilter == EDD_NRT_FRAME_ARP )
		{
			/* peek at "opcode" at offset 6, 16 bits in network byte order (ARP reply = 2, see arp.h, struct arp_hdr) */

			if( pRecvParam->UserDataLength >= 8 )
			{
				LSA_UINT8 *op = &pRecvParam->pBuffer[pRecvParam->UserDataOffset + 6];

				if( op[0] == 0 && op[1] == 2 )
				{
					TCIP_ARP_RESPONSE_RECEIVED(channel->detailptr->group_id, pRecvParam->pBuffer, pRecvParam->IOCount);
				}
			}
		}

#if TCIP_CFG_INTERNICHE

		/***/

		if( pRecvParam->FrameFilter == EDD_NRT_FRAME_IP_TCP ) /* see 948166, do not allow TCP frames with broadcast bit */
		{
			LSA_UINT8 mac1 = pRecvParam->pBuffer[ETHHDR_BIAS];

			if (mac1 & 0x01) /* check broadcast bit */
			{
				tcip_edd_nrt_recv(pRQB, channel); /* reprovide */

				return;
			}
		}

		/***/

		{
		PACKET pkt = (PACKET)TCIP_RQB_GET_USER_ID_PTR(pRQB); /* get the packet */

		if( is_null(pkt) )
		{
			TCIP_FATAL();
		}
		else
		{
			pkt->nb_buff   = (char *)pRecvParam->pBuffer;
			pkt->nb_blen   = TCIP_RECV_BUFFER_SIZE; /* must be set to the allocated buffer size AP00832626 */
			pkt->nb_prot   = pkt->nb_buff + ETHHDR_SIZE;
			pkt->nb_plen   = pRecvParam->IOCount - ETHHDR_SIZE;
			pkt->nb_tstamp = (long)cticks;

			pkt->inuse     = 1; /* see pk_alloc() */
			pkt->flags     = 0;

			pkt->net       = channel->p_if_data->port[0/*!*/].ifp; /* not: pRecvParam->PortID, see AP00819817 */

			{ /* AP00788431: NF_NBPROT flag bit set --> set pkt->type and pkt->nb_prot */
			char *   eth;
			eth = (pkt->nb_buff + ETHHDR_BIAS);

#ifdef TCIP_CFG_VLAN /*INICHE_TCIP_ITGR enable support of VLAN tagged ethernet frames (IEEE 802.1 Q)*/
            /* VLAN Info: Tag Protocol Identifier (TPID):    2 Bytes = 0x8100 */
            /*            User_priority:                     3 Bit */
            /*            Canonical Format Indicator (CFI):  1 Bit */
            /*            VLAN Identifier (VID):            12 Bit */
            if((unshort)ET_TYPE_GET(eth) == 0x8100)
            {
               /* skip VLAN Info (4 Bytes) */
               pkt->type = htons((unshort)ET_TYPE_GET(eth + 4));
               pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE /*+ ETHHDR_BIAS see 1221853*/ + 4; /*AP00922404 */
            }
			else
#endif
			{
#ifdef IEEE_802_3
				/* see if it's got snap baggage */
				if (ET_TYPE_GET(eth) <= 0x0600)
				{
				   struct snap_hdr *snap;
				   snap = (struct snap_hdr *)(pkt->nb_buff + ETHHDR_SIZE);
				   pkt->type = (unshort)(snap->type);
				   pkt->nb_prot = pkt->nb_buff + pkt->net->n_lnh;
				}
				else
				{
				   pkt->type = htons((unshort)ET_TYPE_GET(eth));
				   pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE;
				}
#else
			pkt->type = htons((unshort)ET_TYPE_GET(eth));
            pkt->nb_prot = pkt->nb_buff + pkt->net->n_lnh;
#endif
			}
			}

			/***/

			LOCK_NET_RESOURCE(RXQ_RESID);
			putq(&rcvdq, (q_elt)pkt); /* give packet to InterNiche stack */
			channel->edd.in_work_rcv_rqbs_number ++;		/* Task 4123456 */
			UNLOCK_NET_RESOURCE(RXQ_RESID);

#if TCIP_INT_CFG_TRACE_ON
			if (to_count)
			{
				controller_add_recv_measure_2();
			}
#endif
			SignalPktDemux();

			if (channel->edd.pending_rcv_reqests == 0) /* write error trace if all receive rqbs were given to the IP Stack */
			{
				TCIP_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_ERROR
									,"(!) tcip_edd_nrt_recv_done(): all receive rqbs of channel handle(%d), path_type(%d) are in IP Stack, pending_rcv_reqests(%d), in_work_rcv_rqbs_number(%d)"
									, channel->my_handle, channel->detailptr->path_type, channel->edd.pending_rcv_reqests, channel->edd.in_work_rcv_rqbs_number
									);
			}
		}
		}

#if TCIP_INT_CFG_TRACE_ON
		controller_add_recv_measure_6(controller_get_time()); // dummy for InterNiche
		controller_add_recv_measure_7(); // dummy for InterNiche
		controller_add_recv_measure_8(); // dummy for InterNiche
#endif

#endif

#if TCIP_CFG_OPEN_BSD
		{
		OBSD_IFNET ifp	= channel->p_if_data->port[0/*!*/].ifp; /* not: pRecvParam->PortID, see AP00819817 */

		/* copy packet to buffer, NULL return value is handled by obsd_pnio_ether_input */
		char *buf		= obsd_copy_pkt_from_recv_buffer(ifp, (char *)pRecvParam->pBuffer, pRecvParam->IOCount); 

		/* packet is copied to buf, return RQB to EDD */
		tcip_edd_nrt_recv(pRQB, channel);

		/* process packet */
#if TCIP_INT_CFG_TRACE_ON
		obsd_pnio_ether_input(ifp, buf, to_count);
#else
		obsd_pnio_ether_input(ifp, buf);
#endif

		/***/

#if TCIP_INT_CFG_SOCKET_EVENTS
		if (channel->detailptr->path_type == TCIP_PATH_TYPE_TCP
			|| channel->detailptr->path_type == TCIP_PATH_TYPE_UDP
			)
		{
#if TCIP_INT_CFG_TRACE_ON
			LSA_UINT64 timestamp = controller_get_time();
#endif

			TCIP_DO_SOCKET_EVENTS(); /* when stack has processed TCP or UDP packet, let SOCK examine the event list */

#if TCIP_INT_CFG_TRACE_ON
			controller_add_recv_measure_6(timestamp);
			controller_add_recv_measure_7();
#endif
		}

#endif

#if TCIP_INT_CFG_TRACE_ON
		if (to_count)
		{
			controller_add_recv_measure_8();
		}
#endif
	}
#endif

#if TCIP_INT_CFG_TRACE_ON

	if (to_count)
	{
		controller_add_recv_measure_nrt_recv_finished();
	}
#endif
	}
}


/*---------------------------------------------------------------------------*/
/* returns the given packet after stack processing                           */
/*---------------------------------------------------------------------------*/

#if TCIP_CFG_INTERNICHE

void tcip_iniche_recv_done(void * pkt_) /* called from pk_free() */
{
	PACKET                          pkt = (PACKET)pkt_;
	EDD_RQB_TYPE                    *pRQB = pkt->pRQB;
	TCIP_CHANNEL_PTR_TYPE           channel;

	channel = tcip_channel_from_handle(TCIP_EDD_GET_HANDLE(pRQB));

	if( is_null(channel) || TCIP_EDD_GET_SERVICE(pRQB) != EDD_SRV_NRT_RECV )
	{
		TCIP_FATAL();
	}
	else
	{
		/*
		 *  if channel not open (closing in progress) free rqb
		 */
		if (channel->edd.in_work_rcv_rqbs_number > 0)
		{
#if TCIP_CFG_INTERNICHE
			LOCK_NET_RESOURCE(RXQ_RESID);		/* Task 4123456 */
#endif
			channel->edd.in_work_rcv_rqbs_number --; /* must be 0 before chanel closes */
#if TCIP_CFG_INTERNICHE
			UNLOCK_NET_RESOURCE(RXQ_RESID);
#endif
		}
		else
		{
			TCIP_FATAL();
		}

		if( channel->state == TCIP_CH_CLOSING )
		{
			tcip_FreeRcvBuf(channel, pRQB);

			tcip_edd_close_channel(channel);
		}
		else
		{
			tcip_edd_nrt_recv(pRQB, channel);
		}
	}
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
