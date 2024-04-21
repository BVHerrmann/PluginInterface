/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Open BSD integration                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  10
#define TCIP_MODULE_ID     10

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

/*lint -esym(765,cticks) see obsd_platform_hal_Clock.c */
/*lint -esym(552,cticks)  */


#if (OBSD_PNIO_CFG_MAX_IF_COUNT != EDD_CFG_MAX_INTERFACE_CNT + EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE)
#error "Maximum number of interfaces is configured differently"
#endif

unsigned long cticks;

//DC
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <linux/route.h> // UFo: added
#include <errno.h>		 // UFo: added (for test)




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
	if (obsd_pnio_set_max_socket_number(TCIP_CFG_MAX_SOCKETS) != 0)
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
		//DC
		printf("DC here were are %d\n", nets_count);
	}
}

/*===========================================================================*/

struct tcip_linux_send_handler_params {
	int tapfd;
	TCIP_CHANNEL_PTR_TYPE arp_channel;
};

struct tcip_linux_send_handler_params send_handler_params;

EDD_RQB_TYPE * tcip_getEddSendRQB (EDD_UPPER_RQB_PTR_TYPE * SendRqbs);
LSA_VOID tcip_putEddSendRQB (EDD_UPPER_RQB_PTR_TYPE *SendRqbs, EDD_RQB_TYPE * actSendRqb);
LSA_VOID tcip_edd_nrt_send (TCIP_CHANNEL_CONST_PTR_TYPE  channel, PACKET  pkt, EDD_UPPER_RQB_PTR_TYPE  pRQB);

void * tcip_linux_send_handler_thread(void * params)
{
	int fd;
	struct tcip_linux_send_handler_params *send_handler_parameter = (struct tcip_linux_send_handler_params *) params;
	//int *fd_pointer = (int*) params;
	TCIP_CHANNEL_PTR_TYPE arp_channel = send_handler_parameter->arp_channel;
	unsigned char buffer[2000];
	int size=0;
	EDD_UPPER_RQB_PTR_TYPE  pRQB = NULL;
	EDD_UPPER_NRT_SEND_PTR_TYPE pParam = NULL;

	fd = send_handler_parameter->tapfd;

	printf("Ich bin der sende-thread für %s ;-)  fd=%d arp_channel_pointer=%p\n", arp_channel->p_if_data->port[0].tapname, fd, arp_channel);

	printf("send_rqbs: %p\n", arp_channel->edd.send_rqbs);
	sleep(10);
	printf("send_rqbs: %p\n", arp_channel->edd.send_rqbs);

	while(1)
	{
		size = read(fd, buffer, 2000);
		printf("send: %02X:%02X:%02X:%02X:%02X:%02X %02X:%02X:%02X:%02X:%02X:%02X size=%d\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], size);
		do
		{
			pRQB = tcip_getEddSendRQB(arp_channel->edd.send_rqbs);
			if(pRQB)
			{
				//printf("RQB=%p\n", pRQB);
				pParam = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SEND_PTR_TYPE);
				pParam->Length = size;
				tcip_edd_nrt_send(arp_channel, buffer, pRQB);
				//tcip_putEddSendRQB(arp_channel->edd.send_rqbs, pRQB);
			}
			else
			{
				printf("leider kein RQB :-(\n");
				usleep(1000);
			}

		} while(!pRQB);
		//sleep(10);
	}

}

LSA_VOID tcip_obsd_activate_interfaces( TCIP_CHANNEL_PTR_TYPE arp_channel )
{
	TCIP_IF_DATA_PTR_TYPE p_if_data = arp_channel->p_if_data;
	LSA_UINT16 port_id;
	char tapname[120];
	int tapfd;
	struct ifreq ifr;

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
			sprintf(tapname, "pnio%d", arp_channel->p_if_data->edd_interface_id);
		}
		else /* ports */
		{
			ifAdminStatus = p_if_data->port[port_id].PhyStatus == EDD_PHY_STATUS_ENABLED ? 1 : 0; /* see 1208466 */
			sprintf(tapname, "pnio%d.p%d", arp_channel->p_if_data->edd_interface_id, port_id);
		}

		if( (tapfd = open("/dev/net/tun", O_RDWR)) < 0 )
		{
			TCIP_FATAL();
		}
		memset(&ifr, 0, sizeof(ifr));

		ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
		strncpy(ifr.ifr_name, tapname, IFNAMSIZ);
		printf("ifr: %s\n", ifr.ifr_name);

		if(ioctl(tapfd, TUNSETIFF, (void *) &ifr) < 0 )
		{
			TCIP_FATAL();
		}


		memset(&ifr, 0, sizeof(ifr));

	    ifr.ifr_hwaddr.sa_data[0] = p_if_data->port[port_id].MACAddr.MacAdr[0];
	    ifr.ifr_hwaddr.sa_data[1] = p_if_data->port[port_id].MACAddr.MacAdr[1];
	    ifr.ifr_hwaddr.sa_data[2] = p_if_data->port[port_id].MACAddr.MacAdr[2];
	    ifr.ifr_hwaddr.sa_data[3] = p_if_data->port[port_id].MACAddr.MacAdr[3];
	    ifr.ifr_hwaddr.sa_data[4] = p_if_data->port[port_id].MACAddr.MacAdr[4];
	    ifr.ifr_hwaddr.sa_data[5] = p_if_data->port[port_id].MACAddr.MacAdr[5];
	    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	    if(ioctl(tapfd, SIOCSIFHWADDR, &ifr)<0)
	    {
			TCIP_FATAL();
	    }


		ifp = obsd_pnio_find_free_if_and_activate((void *)arp_channel, (int *)&p_if_data->port[port_id].MACAddr.MacAdr[0], ifAdminStatus);



		if( is_null(ifp) )
		{
			TCIP_FATAL(); /* a bug, free interfaces must be found */
		}
		else
		{
			p_if_data->port[port_id].tapfd = tapfd;
			strncpy(p_if_data->port[port_id].tapname, tapname, IFNAMSIZ);


			if (port_id == 0)
			{
				obsd_pnio_set_link_up(ifp);
				printf("tapfd=%d\n", p_if_data->port[port_id].tapfd);
				send_handler_params.tapfd = tapfd;
				send_handler_params.arp_channel = arp_channel;
				pthread_create(&(p_if_data->port[port_id].send_thread_id), NULL, tcip_linux_send_handler_thread, (void *) &(send_handler_params));
				//pthread_create(&(p_if_data->port[port_id].send_thread_id), NULL, tcip_linux_send_handler_thread, (void *) &(p_if_data->port[port_id].tapfd));
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

LSA_UINT16 TCIP_UPPER_IN_FCT_ATTR
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

LSA_VOID TCIP_UPPER_IN_FCT_ATTR
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

int linux_netif_set_ipv4addr(char *name, unsigned long ipv4_address, unsigned long ipv4_subnet_mask)
{
	struct ifreq ifr;
	int fd;
	int ret;

	fd = socket(AF_INET, SOCK_DGRAM, 0);


	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	((struct sockaddr_in *)&ifr.ifr_addr)->sin_family      = AF_INET;
	((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = ipv4_address;
	//((struct sockaddr_in *)&ifr.ifr_addr)->sin_len         = sizeof(struct sockaddr_in);

	ret = ioctl (fd, SIOCSIFADDR, &ifr);


	((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = ipv4_subnet_mask;

	ret = ioctl (fd, SIOCSIFNETMASK, &ifr);

	close(fd);

	return ret;
}


/**
    @brief          Deletes the IP(v4)-address and network mask for a given (and known) network interface
    @details        Deletes the IPv4 network by calling linux_netif_set_ipv4addr(ifName,0,0);
                    The network interface must be known within the IP-Stack.
    @param[in]      ifName: The name of the network interface. Typically a device instance name string
                    class name and a unit name. E.g.: eth0, eth1, lo0, lo1, pnio1...
    @note           @li   The Function is intended to be called from the userland level.
    @return         0  in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @sa             man 8 ifconfig
*/
int linux_netif_del_ipv4addr(char *ifName)
{
	int retVal = 0;
	retVal = linux_netif_set_ipv4addr(ifName, 0UL, 0UL);
    return retVal;
}


/**
    @brief          Set the default gateway for linux system.
    @details        Set the IP(v4)-Address for the default gateway. This leads to a new entry in the kernel routing table.
    @param[in]      ipv4_defgw_address: The IPv4 address to set for the default gateway. The address is in network byte order.
    @return         0  in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   The function is intended to be called from the userland level.
                    @li   The function can only set one default gateway.
    @attention      ipv4_defgw_address is in network byte order
    @sideeffects    Deletes an existing default gateway first (if one). This behavior is requested from the PN integration and
                    is different to the route commands (that avoids overwriting a existing default gateway).
*/
int linux_add_default_gw(unsigned long ipv4_defgw_address)
{
    int     sd        = -1;
    int     retVal    =  0;
    struct rtentry     rm;

    struct sockaddr_in *pDest, *pGatw, *pMask;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd >= 0)
    {
    	int err = 0;

    	// clear route
    	memset(&rm, 0, sizeof(rm));

    	// preset structures
    	pDest = ( struct sockaddr_in *)(&(rm.rt_dst));
    	pGatw = ( struct sockaddr_in *)(&(rm.rt_gateway));
    	pMask = ( struct sockaddr_in *)(&(rm.rt_genmask));

    	pDest->sin_family = AF_INET;
    	pGatw->sin_family = AF_INET;
    	pMask->sin_family = AF_INET;

    	pDest->sin_addr.s_addr = INADDR_ANY;
    	pGatw->sin_addr.s_addr = INADDR_ANY;
    	pMask->sin_addr.s_addr = INADDR_ANY;

    	rm.rt_flags = RTF_UP | RTF_GATEWAY;

    	// remove default route
    	err = ioctl(sd, SIOCDELRT, &rm);

    	// Setting data for the new default route
    	pDest->sin_addr.s_addr = INADDR_ANY;
    	pGatw->sin_addr.s_addr = ipv4_defgw_address;
    	pMask->sin_addr.s_addr = INADDR_ANY;
//    	rm.rt_metric =1;                     // Why?
    	rm.rt_flags = RTF_UP | RTF_GATEWAY;

    	// remove this route
    	err = ioctl(sd, SIOCDELRT, &rm);

    	// add the default route
    	if ((err = ioctl(sd, SIOCADDRT, &rm)) < 0)
    	{
    		int errval = errno;
    		printf("linux_route_add_default_gw: ioctl(SIOCADDRT) failed, errno->%s\n",strerror(errval));
    		retVal = -1;
    	}
    	else
    	{
    		printf("linux_route_add_default_gw: ioctl(SIOCADDRT) done");

    	}
        close(sd);
    }
    else
    {
//		printf("linux_route_add_default_gw: could not open socket!\n");
        retVal = -1;
    }
    return retVal;
}

/**
    @brief          Clear the default gateway for linux system.
    @details        Sets the IP(v4)-Address for the default gateway. This leads to a new entry in the kernel routing table.
    @return         0  in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   The function is intended to be called from the userland level.
    @sideeffects    Deletes an existing default gateway first (if one).
*/
int linux_del_default_gw(void)
{
    int     sd        = -1;
    int     retVal    =  0;
    struct rtentry     rm;

    struct sockaddr_in *pDest, *pGatw, *pMask;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd >= 0)
    {
    	int err = 0;

    	// clear route struct
    	memset(&rm, 0, sizeof(rm));

    	// preset structures
    	pDest = ( struct sockaddr_in *)(&(rm.rt_dst));
    	pGatw = ( struct sockaddr_in *)(&(rm.rt_gateway));
    	pMask = ( struct sockaddr_in *)(&(rm.rt_genmask));

    	pDest->sin_family = AF_INET;
    	pGatw->sin_family = AF_INET;
    	pMask->sin_family = AF_INET;

    	pDest->sin_addr.s_addr = INADDR_ANY;
    	pGatw->sin_addr.s_addr = INADDR_ANY;
    	pMask->sin_addr.s_addr = INADDR_ANY;
    	rm.rt_flags = RTF_UP | RTF_GATEWAY;

    	// remove default route
    	if ((err = ioctl(sd, SIOCDELRT, &rm)) < 0)
    	{
    		int errval = errno;
    		printf("linux_del_default_gw: ioctl(SIOCDELRT) failed, errno->%s\n",strerror(errval));
    		retVal = -1;
    	}
    	else
    	{
    		printf("linux_del_default_gw: ioctl(SIOCDELRT) done");
    	}
        close(sd);
    }
    else
    {
		printf("linux_del_default_gw: could not open socket!\n");
        retVal = -1;
    }
    return retVal;

}


int linux_pnio_set_link_up(char *ifName)
{
	int retVal = 0;
	struct ifreq ifr;
	int sd;


	sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd >= 0)
    {
    	int err = 0;

		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, ifName, sizeof(ifr.ifr_name));
		ifr.ifr_flags |= IFF_UP;

		if ((err = ioctl(sd, SIOCSIFFLAGS, &ifr) ) < 0)
		{
    		int errval = errno;
    		printf("linux_pnio_set_link_up: ioctl(SIOCSIFFLAGS) failed, errno->%s\n",strerror(errval));
    		retVal = -1;
		}
		else
		{
    		printf("linux_pnio_set_link_up: ioctl(SIOCDELRT) done");
		}

		close(sd);
    }
    else
    {
    	retVal = -1;
    }
	return retVal;
}


int linux_pnio_set_link_down(char *ifName)
{
	int retVal = 0;
	struct ifreq ifr;
	int sd;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd >= 0)
    {
    	int err = 0;

		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, ifName, sizeof(ifr.ifr_name));
		ifr.ifr_flags &= ~IFF_UP;
		if ((err = ioctl(sd, SIOCSIFFLAGS, &ifr) ) < 0)
		{
    		int errval = errno;
    		printf("linux_pnio_set_link_down: ioctl(SIOCSIFFLAGS) failed, errno->%s\n",strerror(errval));
    		retVal = -1;
		}
		else
		{
    		printf("linux_pnio_set_link_down: ioctl(SIOCDELRT) done");
		}

		close(sd);
    }
    else
    {
    	retVal = -1;
    }
	return retVal;
}

#if 0
void linux_pnio_set_baudrate(OBSD_IFNET ifp, long baudrate)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	my_ifp->if_data.ifi_baudrate = baudrate;
}

#endif


/*===========================================================================*/
#endif // TCIP_CFG_OPEN_BSD
/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
