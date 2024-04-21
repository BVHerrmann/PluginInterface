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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Open BSD Stack)           :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_obsd.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Open BSD integration                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define TCIP_MODULE_ID     9

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if TCIP_CFG_OPEN_BSD
/*===========================================================================*/

#if defined(LSA_HOST_ENDIANESS_LITTLE)
#if (OBSD_PNIO_CFG_BIG_ENDIAN != 0)
#error "check endianess"
#endif
#elif defined(LSA_HOST_ENDIANESS_BIG)
#if (OBSD_PNIO_CFG_BIG_ENDIAN != 1)
#error "check endianess"
#endif
#else
#error "check endianess"
#endif

#if defined (TOOL_CHAIN_MICROSOFT)
#ifndef OBSD_PNIO_TOOL_CHAIN_MICROSOFT
#error "different toolchains defined 1"
#endif
#elif defined (TOOL_CHAIN_NONE)
#ifndef OBSD_PNIO_TOOL_CHAIN_NONE
#error "different toolchains defined 2"
#endif
#elif defined (TOOL_CHAIN_TASKING_TRICORE)
#ifndef OBSD_PNIO_TOOL_CHAIN_TASKING_TRICORE
#error "different toolchains defined 3"
#endif
#elif defined (TOOL_CHAIN_GNU)
#ifndef OBSD_PNIO_TOOL_CHAIN_GNU
#error "different toolchains defined 4"
#endif
#elif defined (TOOL_CHAIN_GNU_PPC)
#ifndef OBSD_PNIO_TOOL_CHAIN_GNU_PPC
#error "different toolchains defined 5"
#endif
#elif defined (TOOL_CHAIN_CC386)
#ifndef OBSD_PNIO_TOOL_CHAIN_CC386
#error "different toolchains defined 6"
#endif
#elif defined (TOOL_CHAIN_GREENHILLS_ARM)
#ifndef OBSD_PNIO_TOOL_CHAIN_GREENHILLS_ARM
#error "different toolchains defined 7"
#endif
#elif defined (TOOL_CHAIN_NRK)
#ifndef OBSD_PNIO_TOOL_CHAIN_NRK
#error "different toolchains defined 8"
#endif
#else
#error "no valid toolchain defined"
#endif

#if (TCIP_CFG_MAX_SOCKETS != OBSD_PNIO_CFG_MAX_SOCKETS)
#error "maximum number of sockets must be equal"
#endif

#if (OBSD_PNIO_ETHERMTU != (EDD_IEEE_FRAME_LENGTH - 18))
#error "MTU is defined differently in EDD"
#endif

#if (TCIP_INT_CFG_TRACE_ON != OBSD_PNIO_CFG_TRACE_ON)
#error "CFG_TRACE_ON is configured differently!"
#endif

#if (OBSD_PNIO_CFG_MAX_IF_COUNT != (2 * EDD_CFG_MAX_INTERFACE_CNT) + EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE)
#error "Maximum number of interfaces is configured differently"
#endif

#if (OBSD_PNIO_TCIP_OK != TCIP_OK)
#error (incompatible TCIP_OK)
#endif

#if (OBSD_PNIO_ERR_RESOURCE != TCIP_ERR_RESOURCE)
#error (incompatible TCIP_ERR_RESOURCE)
#endif

#if (OBSD_PNIO_ERR_PARAM != TCIP_ERR_PARAM)
#error (incompatible TCIP_ERR_PARAM)
#endif

#if (OBSD_PNIO_ERR_LOWER_LAYER != TCIP_ERR_LOWER_LAYER)
#error (incompatible TCIP_ERR_LOWER_LAYER)
#endif

/*lint -esym(765,cticks) see obsd_platform_hal_Clock.c */
/*lint -esym(552,cticks)  */
unsigned long cticks;

/*===========================================================================*/

/*
 *  to be called once for ipstack "start"
 */
LSA_VOID tcip_obsd_init(LSA_VOID)
{
	cticks = 0;

	obsd_pnio_check_types();
	OSAL_Init();
	StartStackKernel();

	/* set maximum number of open sockets */
	if (obsd_pnio_set_max_socket_number(TCIP_CFG_MAX_SOCKETS*2 + 100) != 0) /* sockets AND filedescriptors */
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "Error: obsd_pnio_set_max_socket_number failed!");
		TCIP_FATAL();
	}

#ifdef TCIP_CFG_KEEPALIVE_IDLE_TIMEOUT
	if (obsd_pnio_set_tcp_keepidle(TCIP_CFG_KEEPALIVE_IDLE_TIMEOUT) != 0)
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "Error: obsd_pnio_set_tcp_keepidle failed!");
		TCIP_FATAL();
	}
#endif

#ifdef TCIP_CFG_KEEPALIVE_PROBE_INTERVAL
	if (obsd_pnio_set_tcp_keepintvl(TCIP_CFG_KEEPALIVE_PROBE_INTERVAL) != 0)
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "Error: obsd_pnio_set_tcp_keepintvl failed!");
		TCIP_FATAL();
	}
#endif

#ifdef TCIP_CFG_KEEPALIVE_PROBE_COUNT
	if (obsd_pnio_set_tcp_keepcount(TCIP_CFG_KEEPALIVE_PROBE_COUNT) != 0)
	{
		TCIP_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "Error: obsd_pnio_set_tcp_keepcount failed!");
		TCIP_FATAL();
	}
#endif
}

/*===========================================================================*/

/*
 *  to be called once for ipstack "shutdown"
 */
LSA_VOID tcip_obsd_undo_init(LSA_VOID)
{
	StopStackKernel();
	OSAL_Deinit();
}

extern int clockintr(void *);
/*===========================================================================*/

LSA_VOID tcip_obsd_ctick(LSA_VOID) /* timer callback */
{
	cticks++;
	clockintr(LSA_NULL);  /* call clockintr without stack frame, because we have no one */
}

/*===========================================================================*/

LSA_VOID tcip_obsd_create_interfaces( LSA_UINT32 nets_count )
{

	/*
	 *  create "nets_count" interfaces with admin-status "down"
	 */
	if( nets_count > OBSD_PNIO_CFG_MAX_IF_COUNT)
	{
		TCIP_FATAL();
	}
	else
	{
		LSA_UINT32 cnt;

		for ( cnt = 0; cnt < nets_count; cnt++ )
		{
			if (obsd_pnio_create_interface() != 0)
			{
				TCIP_FATAL();
			}
		}
	}
}

/*===========================================================================*/

LSA_VOID tcip_obsd_activate_interfaces( TCIP_CHANNEL_PTR_TYPE arp_channel )
{
	TCIP_IF_DATA_PTR_TYPE p_if_data = arp_channel->p_if_data;
	LSA_UINT16 port_id;

	/*
	 *  find free net struct for interface and ports
	 */
	for( port_id = 0/*!*/; port_id <= p_if_data->port_count; port_id ++ )
	{
		int ifAdminStatus;
		OBSD_IFNET ifp;

		if (port_id == 0) /* interface */
		{
			ifAdminStatus = 1; /* ifAdminStatus of interface is always up */
		}
		else /* ports */
		{
			ifAdminStatus = p_if_data->port[port_id].PhyStatus == EDD_PHY_STATUS_ENABLED ? 1 : 0; /* see 1208466 */
		}

		ifp = obsd_pnio_find_free_if_and_activate((void *)arp_channel, (int *)&p_if_data->port[port_id].MACAddr.MacAdr[0], ifAdminStatus);

		if( is_null(ifp) )
		{
			TCIP_FATAL(); /* a bug, free interfaces must be found */
		}
		else
		{
			if (port_id == 0)
			{
				obsd_pnio_set_link_up(ifp);
			}
			else
			{
				obsd_pnio_set_link_down(ifp); /* ifOperStatus of ports is down until link indication, see tcip_edd_ext_link_status_indication() */
			}

			obsd_pnio_set_baudrate(ifp, (long)p_if_data->port[port_id].nominal_bandwidth);
			p_if_data->port[port_id].ifp = ifp;
			TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "Interface %u activated", obsd_pnio_get_if_index(ifp));
		}
	}
}

/*===========================================================================*/
/*
 *  remove interfaces of this channel-group
 */
LSA_VOID tcip_obsd_deactivate_interfaces( TCIP_CONST_CHANNEL_CONST_PTR_TYPE arp_channel )
{
	TCIP_IF_DATA_PTR_TYPE p_if_data = arp_channel->p_if_data;
	LSA_UINT16 port_id;

	/*
	 *  reset all nets belonging to this interface
	 */
	for ( port_id = 0/*!*/; port_id <= p_if_data->port_count; port_id++ )
	{
		OBSD_IFNET ifp = p_if_data->port[port_id].ifp;
		if (obsd_pnio_deactivate_interface(ifp) != 0)
		{
			TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "Error: could not deactivate interface %u!", obsd_pnio_get_if_index(ifp));
			TCIP_FATAL();
		}

		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "Interface %u deactivated", obsd_pnio_get_if_index(ifp));
	}

}

/*===========================================================================*/

LSA_UINT16
tcip_set_timeparams(
	LSA_UINT32 keepalivetime,
	LSA_UINT32 connectiontimeout,
	LSA_UINT32 retransmittimeout,
	LSA_UINT32 retransmitcount,
	LSA_UINT32 no_change_value
) {
	/* note: range-check is done by caller */

	int ret = 0;

	if( keepalivetime != no_change_value )
	{
		ret = obsd_pnio_set_tcp_keepidle((int)keepalivetime/1000);

		if (ret != 0)
		{
			return LSA_RET_ERR_PARAM;
		}
	}


	if( connectiontimeout != no_change_value )
	{
		ret = obsd_pnio_set_tcp_keepinittime((int)connectiontimeout/1000);
		if (ret != 0)
		{
			return LSA_RET_ERR_PARAM;
		}
	}

	if( retransmittimeout != no_change_value )
	{
		ret = obsd_pnio_set_tcp_keepintvl((int)retransmittimeout/1000);
		if (ret != 0)
		{
			return LSA_RET_ERR_PARAM;
		}

		ret = obsd_pnio_set_tcp_rexmtmax((int)retransmittimeout/1000);
		if (ret != 0)
		{
			return LSA_RET_ERR_PARAM;
		}
	}

	if( retransmitcount != no_change_value )
	{
		ret = obsd_pnio_set_tcp_keepcount((int)retransmitcount);
		if (ret != 0)
		{
			return LSA_RET_ERR_PARAM;
		}
	}

	return LSA_RET_OK;
}

/*===========================================================================*/

LSA_VOID
tcip_get_timeparams(
	LSA_UINT32 *p_keepalivetime,
	LSA_UINT32 *p_connectiontimeout,
	LSA_UINT32 *p_retransmittimeout,
	LSA_UINT32 *p_retransmitcount
) {
	*p_keepalivetime     = (LSA_UINT32)obsd_pnio_get_tcp_keepidle() * 1000;
	*p_connectiontimeout = (LSA_UINT32)obsd_pnio_get_tcp_keepinittime() * 1000;
	*p_retransmittimeout = (LSA_UINT32)obsd_pnio_get_tcp_keepintvl() * 1000;
	*p_retransmitcount   = (LSA_UINT32)obsd_pnio_get_tcp_keepcount();
}

/*===========================================================================*/
#endif // TCIP_CFG_OPEN_BSD
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
