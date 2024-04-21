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
/*  F i l e               &F: tcip_edd.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EDDx integration                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  3
#define TCIP_MODULE_ID     3

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
/*===========================================================================*/

static LSA_VOID
tcip_edd_get_params(
	TCIP_CONST_CHANNEL_PTR_TYPE channel
);

static LSA_VOID
tcip_edd_get_portparams(
	TCIP_CONST_CHANNEL_PTR_TYPE channel
);

static LSA_VOID
tcip_edd_nrt_cancel_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
);

#if TCIP_CFG_ENABLE_MULTICAST /* OBSD_ITGR -- avoid warning: 'tcip_edd_srv_multicast_done' declared 'static' but never defined */
static LSA_VOID
tcip_edd_srv_multicast_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_TYPE  *pRQB
);
#endif

/*===========================================================================*/
/*===========================================================================*/

EDD_UPPER_RQB_PTR_TYPE tcip_AllocRQB(LSA_SYS_PTR_TYPE pSys, LSA_UINT16 param_length)
{
	LSA_USER_ID_TYPE         UserId;
	EDD_UPPER_RQB_PTR_TYPE   pRQB;

	UserId.uvar32 = 0;
	TCIP_EDD_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(TCIP_EDD_LOWER_RQB_TYPE), pSys);

	if( is_not_null(pRQB) )
	{
		LSA_VOID *pParam;
		LSA_UINT16 RetVal;

		TCIP_EDD_ALLOC_LOWER_MEM(&pParam, UserId, param_length, pSys);

		if( is_null(pParam) )
		{
			TCIP_EDD_FREE_LOWER_RQB(&RetVal, pRQB, pSys);
            // Return value is not evaluated
            LSA_UNUSED_ARG(RetVal);
			pRQB = LSA_NULL;
		}
		else
		{
			EDD_RQB_SET_PPARAM( pRQB, pParam );
		}

	}

	return ( pRQB );
}

/*===========================================================================*/

LSA_VOID tcip_FreeRQB( LSA_SYS_PTR_TYPE pSys, EDD_RQB_TYPE *pRQB )
{
	TCIP_LOG_FCT("tcip_FreeRQB")

	LSA_VOID_PTR_TYPE pParam = LSA_NULL;
	LSA_UINT16 ret16_val;

	pParam = TCIP_EDD_GET_PPARAM( pRQB, LSA_VOID_PTR_TYPE );

	if ( is_not_null(pParam) )
	{
		TCIP_EDD_FREE_LOWER_MEM(&ret16_val, pParam, pSys);
		TCIP_ASSERT(ret16_val == LSA_RET_OK);
	}

	TCIP_EDD_FREE_LOWER_RQB(&ret16_val, pRQB, pSys);
	TCIP_ASSERT(ret16_val == LSA_RET_OK);
}

/*===========================================================================*/
/*===========================================================================*/

/*
 * open channel for icmp, udp, tcp and arp
 */
LSA_VOID tcip_open_channel_all(TCIP_CONST_CHANNEL_PTR_TYPE channel )
{
	LSA_USER_ID_TYPE                            UserId;
	TCIP_EDD_LOWER_RQB_PTR_TYPE                 pRQB;
	EDD_UPPER_OPEN_CHANNEL_PTR_TYPE             pOpen;

	UserId.uvar32 = 0;
	TCIP_EDD_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(EDD_RQB_TYPE), channel->sysptr );

	if (is_null(pRQB))
	{
		TCIP_FATAL();
	}

	{
	TCIP_EDD_LOWER_MEM_PTR_TYPE tmp; /* AP01308013, use tmp var to prevent GNU compiler warning */
	TCIP_EDD_ALLOC_LOWER_MEM(&tmp, UserId, sizeof(EDD_RQB_OPEN_CHANNEL_TYPE), channel->sysptr);
	if (is_null(tmp))
	{
		TCIP_FATAL();
	}
	pOpen = tmp;
	}

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_OPEN_CHANNEL, 0);
	pRQB->pParam = pOpen;

	pOpen->Cbf = tcip_edd_request_lower_done;

	pOpen->SysPath = channel->syspath;       /* same syspath as "upper (!)  */

	pOpen->HandleUpper = channel->my_handle; /*index of channel in work */

	TCIP_EDD_OPEN_CHANNEL_LOWER(pRQB, channel->sysptr );
}

/*===========================================================================*/

/*
 * callback for all channel open procedures
 */
static LSA_VOID tcip_edd_open_channel_done(TCIP_CHANNEL_PTR_TYPE ch, TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB)
{
	TCIP_LOG_FCT("tcip_edd_open_channel_done")
	EDD_UPPER_OPEN_CHANNEL_PTR_TYPE pOpen = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_OPEN_CHANNEL_PTR_TYPE);
	LSA_UINT16 uRetval;

	ch->edd.handle_lower = pOpen->HandleLower;  /* EDD HandleLower */

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "tcip_edd_open_channel_done: unexpected return code from EDDx(%X)", TCIP_EDD_GET_RESPONSE(pRQB));
		TCIP_FATAL();
	}

	TCIP_EDD_FREE_LOWER_MEM(&uRetval, pRQB->pParam, ch->sysptr);

	TCIP_EDD_FREE_LOWER_RQB(&uRetval, pRQB, ch->sysptr);
    // Return value is not evaluated
    LSA_UNUSED_ARG(uRetval);

	/*
	 *  query edd for interface params only for arp
	 */
	if( TCIP_PATH_TYPE_ARP == ch->detailptr->path_type )
	{
		tcip_edd_get_params(ch);
	}
	else
	{
		/*
		 * initialize channel data structures and set channel pointer in interface structs
		 */
		tcip_init_opt_channel(ch);

		ch->state = TCIP_CH_OPEN;

		tcip_callback(ch, TCIP_OK, &ch->curr);  /* return rqb now */
	}
}

/*===========================================================================*/

/*
 *  called when edd arp channel was opened successfully
 */
static LSA_VOID
tcip_edd_get_params(
	TCIP_CONST_CHANNEL_PTR_TYPE channel
) {
	TCIP_LOG_FCT("tcip_edd_get_params")

	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB = tcip_AllocRQB(channel->sysptr, sizeof(EDD_RQB_GET_PARAMS_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_FATAL(); /* a bug */
	}

	/***/

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_GET_PARAMS);
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PARAMS (EDDHandle: 0x%X,pRQB: 0x%X)",channel->edd.handle_lower, pRQB );
	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 *  callback for EDD_SRV_GET_PARAMS
 */
static LSA_VOID
tcip_edd_get_params_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	TCIP_LOG_FCT("tcip_edd_get_params_done")

	TCIP_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"<<<: EDD_SRV_GET_PARAMS done (Handle: 0x%X,Rsp: 0x%X)",channel->edd.handle_lower,TCIP_EDD_GET_RESPONSE(pRQB));

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_FATAL();
	}
	else
	{
		EDD_UPPER_GET_PARAMS_PTR_TYPE p_eddpars = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_GET_PARAMS_PTR_TYPE);

		channel->trace_idx = p_eddpars->TraceIdx;

		if (
			p_eddpars->MaxInterfaceCntOfAllEDD < 1
			|| p_eddpars->MaxInterfaceCntOfAllEDD > EDD_CFG_MAX_INTERFACE_CNT
			|| p_eddpars->MaxPortCntOfAllEDD < p_eddpars->MaxPortCnt
			|| p_eddpars->MaxPortCntOfAllEDD > EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE
			|| p_eddpars->MaxPortCnt > EDD_CFG_MAX_PORT_CNT )
		{
			TCIP_FATAL(); /* misconfiguration */
		}
		else
		{
			TCIP_IF_DATA_PTR_TYPE p_if_data;

			/*
			 * create and init interface data struct and store pointer in this channel
			 */
			{
			TCIP_LOCAL_MEM_PTR_TYPE tmp; /* AP01308013, use tmp var to prevent GNU compiler warning */
			TCIP_ALLOC_LOCAL_MEM(&tmp, sizeof(*p_if_data));
			p_if_data = tmp;
			}

			if( is_null(p_if_data) )
			{
				TCIP_FATAL();
			}
			else
			{
				p_if_data->nets_count = p_eddpars->MaxInterfaceCntOfAllEDD + p_eddpars->MaxPortCntOfAllEDD;

				p_if_data->edd_interface_id  = p_eddpars->InterfaceID;

				p_if_data->port_count = p_eddpars->MaxPortCnt;

				p_if_data->port[0].MACAddr = p_eddpars->MACAddr;

				p_if_data->port[0].nominal_bandwidth = 100000000; /* 100MBps is an innocent value */


#if TCIP_CFG_INTERNICHE
				p_if_data->port[0].ifp = LSA_NULL;
				channel->edd.send_pkts.max_len = MAXPACKETS/p_eddpars->MaxInterfaceCntOfAllEDD/4; /* limit send queue len, see 973233; queue should not consume all packets, see 1868605 */
#elif TCIP_CFG_OPEN_BSD
				p_if_data->port[0].ifp = LSA_NULL;
#endif

				channel->p_if_data = p_if_data;

				/***/

				TCIP_UPPER_TRACE_08(0, LSA_TRACE_LEVEL_NOTE_LOW,
					"PortCnt: %u, MAC: %02X-%02X-%02X-%02X-%02X-%02X, nom-bandwidth: %u",
					p_if_data->port_count,
					p_if_data->port[0].MACAddr.MacAdr[0],
					p_if_data->port[0].MACAddr.MacAdr[1],
					p_if_data->port[0].MACAddr.MacAdr[2],
					p_if_data->port[0].MACAddr.MacAdr[3],
					p_if_data->port[0].MACAddr.MacAdr[4],
					p_if_data->port[0].MACAddr.MacAdr[5],
					p_if_data->port[0].nominal_bandwidth
					);

				/***/

				tcip_FreeRQB(channel->sysptr, pRQB );

				tcip_edd_get_portparams(channel);   /* async - will return rqb channel->curr*/
			}
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *  retrieve the port parameters from eddx
 */
static LSA_VOID
tcip_edd_get_portparams(
	TCIP_CONST_CHANNEL_PTR_TYPE channel
) {
	TCIP_LOG_FCT("tcip_edd_get_portparams")

	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB = tcip_AllocRQB(channel->sysptr, sizeof(EDD_RQB_GET_PORT_PARAMS_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_FATAL(); /* a bug */
	}

	/***/

	TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_GET_PORT_PARAMS);
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	{
	EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE pPortParams = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE);

	pPortParams->PortID = 1;   /* start with first port */

	TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (EDDHandle: 0x%X, PortID: %d, pRQB: 0x%X)", 
		channel->edd.handle_lower, pPortParams->PortID, pRQB);
	}

	TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 * callback for EDD_SRV_GET_PORT_PARAMS
 */
static LSA_VOID
tcip_edd_get_portparams_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	TCIP_LOG_FCT("tcip_edd_get_portparams_done")

	if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_FATAL();
	}
	else
	{
		EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE pPortParams = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE);
		TCIP_IF_DATA_PTR_TYPE p_if_data = channel->p_if_data;

		/* save the "port MAC-address" */

		p_if_data->port[pPortParams->PortID].MACAddr = pPortParams->MACAddr;

		/* derive the "nominal bandwith" from the PHY's advertised capabilities */

		{
		LSA_UINT32 nbw;

		if( pPortParams->AutonegCapAdvertised &
			( EDD_AUTONEG_CAP_1000BASEX
			| EDD_AUTONEG_CAP_1000BASEXFD
			| EDD_AUTONEG_CAP_1000BASET
			| EDD_AUTONEG_CAP_1000BASETFD) )
		{
			nbw = 1000000000;
		}
		else if( pPortParams->AutonegCapAdvertised &
			( EDD_AUTONEG_CAP_100BASET4
			| EDD_AUTONEG_CAP_100BASETX
			| EDD_AUTONEG_CAP_100BASETXFD
			| EDD_AUTONEG_CAP_100BASET2
			| EDD_AUTONEG_CAP_100BASET2FD) )
		{
			nbw = 100000000;
		}
		else /* report every thing else as 10MBps */
		{
			nbw = 10000000;
		}

		p_if_data->port[pPortParams->PortID].nominal_bandwidth = nbw;
		}

		/***/

		TCIP_UPPER_TRACE_08(0, LSA_TRACE_LEVEL_NOTE_LOW,
			"Port: %u, MAC: %02X-%02X-%02X-%02X-%02X-%02X, nom-bandwidth: %u",
			pPortParams->PortID,
			pPortParams->MACAddr.MacAdr[0],
			pPortParams->MACAddr.MacAdr[1],
			pPortParams->MACAddr.MacAdr[2],
			pPortParams->MACAddr.MacAdr[3],
			pPortParams->MACAddr.MacAdr[4],
			pPortParams->MACAddr.MacAdr[5],
			p_if_data->port[pPortParams->PortID].nominal_bandwidth
			);

		/***/

		p_if_data->port[pPortParams->PortID].PhyStatus = pPortParams->PhyStatus; /* see 1208466 */

		/*
		 * next port
		 */

		pPortParams->PortID++;

		if( pPortParams->PortID <= p_if_data->port_count )  /* do next port */
		{
			TCIP_EDD_SET_HANDLE(pRQB, 0);
			EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

			TCIP_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_GET_PORT_PARAMS (EDDHandle: 0x%X, PortID: %d, pRQB: 0x%X)", 
				channel->edd.handle_lower, pPortParams->PortID, pRQB);

			TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
		}
		else
		{
			tcip_FreeRQB(channel->sysptr, pRQB);

			/***/

			TCIP_ASSERT( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type );

#if TCIP_CFG_ENABLE_MULTICAST
			channel->trace_idx = channel->trace_idx; // make lint happy

			/* add entry for all hosts group (224.0.0.1 -> MAC: 01-00-5E-00-00-01 */
			{
			LSA_UINT64 address = 0x01005E000001;
			tcip_edd_srv_multicast(channel, 0, address, TCIP_SRV_MULTICAST_ENABLE);
			}
#else
			tcip_init_arp_channel(channel); /* initialization of "first" channel */

			channel->state = TCIP_CH_OPEN;

			tcip_callback(channel, TCIP_OK, &channel->curr);  /* return rqb now */

#endif
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   sends nrt receive rqb and increases of pending requests counter
 */
LSA_VOID tcip_edd_nrt_recv( EDD_RQB_TYPE *pRQB, TCIP_CHANNEL_CONST_PTR_TYPE channel )
{
#if TCIP_CFG_INTERNICHE
	LOCK_NET_RESOURCE(RXQ_RESID);		/* Task 4123456 */
#endif
	channel->edd.pending_rcv_reqests++;
#if TCIP_CFG_INTERNICHE
	UNLOCK_NET_RESOURCE(RXQ_RESID);
#endif

	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);
	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   cancel NRT_RECV requests in channel
 */
LSA_VOID tcip_edd_nrt_cancel(TCIP_CHANNEL_CONST_PTR_TYPE channel )
{
	TCIP_LOG_FCT("tcip_edd_nrt_cancel")

	EDD_UPPER_NRT_CANCEL_PTR_TYPE pParam;

	EDD_UPPER_RQB_PTR_TYPE  pRQB = tcip_AllocRQB( channel->sysptr, sizeof(EDD_RQB_NRT_CANCEL_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_FATAL(); /* a bug */
	}

	channel->edd.pending_rcv_reqests++; /* treat nrt cancel as resource */

	pParam = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_NRT_CANCEL_PTR_TYPE );
	pParam->Mode = EDD_NRT_CANCEL_MODE_ALL;
	pParam->RequestID = 0;

	TCIP_EDD_SET_OPCODE( pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_CANCEL);
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"REQ: EDD_SRV_NRT_CANCEL, handle(%u) ", channel->edd.handle_lower );

	TCIP_EDD_REQUEST_LOWER( pRQB, channel->sysptr);
}

/*===========================================================================*/

/*
 * treat EDD_SRV_NRT_CANCEL opcode confirmation
 */
static LSA_VOID tcip_edd_nrt_cancel_done(TCIP_CHANNEL_PTR_TYPE channel, TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB)
{
	TCIP_LOG_FCT("tcip_edd_nrt_cancel_done")

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"REQ: EDD_SRV_NRT_CANCEL done RSP(%u/%#x)",
										TCIP_EDD_GET_RESPONSE(pRQB), TCIP_EDD_GET_RESPONSE(pRQB));

	tcip_FreeRQB( channel->sysptr, pRQB );

	/*
	 *  check if last receive resource has been cancelled
	 */

	if( (channel->edd.pending_rcv_reqests != 1) || /* must be equal to 1 now if all other resources were cancelled as expected; see tcip_edd_nrt_cancel() */
		(TCIP_CH_CLOSING != channel->state    )   )
	{
		TCIP_FATAL();
	}

	channel->edd.pending_rcv_reqests = 0; /* all edd recv resources including nrt cancel are confirmed */

	tcip_edd_close_channel(channel);
}

/*===========================================================================*/

/*
 *   cancel resources of this channel
 */
LSA_VOID tcip_edd_close_channel(TCIP_CONST_CHANNEL_PTR_TYPE channel )
{
	if( !channel->edd.get_statistics_pending /* do not close channel unless statistic update is finished and  */
		&& channel->edd.in_work_rcv_rqbs_number == 0 /* interniche stack has returned all receive packets and */
		&& channel->edd.pending_rcv_reqests == 0 /* all edd recv resources including nrt cancel are confirmed */
		)
	{
		TCIP_EDD_LOWER_RQB_PTR_TYPE     pRQB;
		LSA_USER_ID_TYPE                UserId;

		/*
		 * free pre-allocated statistic rqb
		 */
		if( TCIP_PATH_TYPE_ARP == channel->detailptr->path_type )
		{
			tcip_FreeRQB(channel->sysptr, channel->edd.pEdd );
		}

		UserId.uvar32 = 0;

		TCIP_EDD_ALLOC_LOWER_RQB( &pRQB, UserId, sizeof(EDD_RQB_TYPE), channel->sysptr);
#if 0
		TCIP_ASSERT( is_not_null(pRQB) );
#else
		if (is_null(pRQB))
		{
			TCIP_FATAL();
		}
#endif
		TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_CLOSE_CHANNEL, 0);
		TCIP_EDD_SET_HANDLE(pRQB, 0);
		EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

		TCIP_EDD_CLOSE_CHANNEL_LOWER( pRQB, channel->sysptr);
	}
}

/*===========================================================================*/

/*
 *   function: tcip_edd_close_channel_done()
 */
static LSA_VOID tcip_edd_close_channel_done(TCIP_CHANNEL_PTR_TYPE ch, TCIP_EDD_LOWER_RQB_PTR_TYPE  pRQB)
{
	LSA_UINT16          uRetval;

	uRetval = LSA_RQB_GET_RESPONSE(pRQB);

	TCIP_ASSERT(uRetval == EDD_STS_OK);

	TCIP_EDD_FREE_LOWER_RQB(&uRetval, pRQB, ch->sysptr);

	tcip_cancel_send_resources(ch);

	/*
	 *  send channel close confirmation
	 */

	tcip_callback_and_free_channel(ch, TCIP_OK, ch->curr );
}

/*===========================================================================*/

#if TCIP_CFG_ENABLE_MULTICAST
/*
 *   function: tcip_edd_srv_multicast()
 */
LSA_VOID
tcip_edd_srv_multicast(
	TCIP_CONST_CHANNEL_PTR_TYPE channel,
	LSA_INT32 dev_handle,
	LSA_UINT64 mc_address,
	LSA_UINT8 mode
) {
	LSA_UINT32 mcaddr_lower = (LSA_UINT32)mc_address;
	LSA_UINT32 mcaddr_upper = (LSA_UINT32)(mc_address >> 24);

	TCIP_LOG_FCT("tcip_edd_srv_multicast")

	EDD_UPPER_MULTICAST_PTR_TYPE pSrvMulti;

	EDD_UPPER_RQB_PTR_TYPE  pRQB = tcip_AllocRQB( channel->sysptr, sizeof(EDD_RQB_MULTICAST_TYPE));

	if( is_null(pRQB) )
	{
		TCIP_FATAL(); /* a bug */
	}

	pSrvMulti = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_MULTICAST_PTR_TYPE );

	if (mode == TCIP_SRV_MULTICAST_ENABLE )
	{
		pSrvMulti->Mode = EDD_MULTICAST_ENABLE;
	}
	else
	{
		pSrvMulti->Mode = EDD_MULTICAST_DISABLE;
	}

	pSrvMulti->MACAddr.MacAdr[0] = (LSA_UINT8)(mcaddr_upper >> 16);
	pSrvMulti->MACAddr.MacAdr[1] = (LSA_UINT8)(mcaddr_upper >> 8);
	pSrvMulti->MACAddr.MacAdr[2] = (LSA_UINT8)(mcaddr_upper);
	pSrvMulti->MACAddr.MacAdr[3] = (LSA_UINT8)(mcaddr_lower >> 16);
	pSrvMulti->MACAddr.MacAdr[4] = (LSA_UINT8)(mcaddr_lower >> 8);
	pSrvMulti->MACAddr.MacAdr[5] = (LSA_UINT8)(mcaddr_lower);


	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_MULTICAST);
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	EDD_RQB_SET_USERID_UVAR32(pRQB, (LSA_UINT32)dev_handle);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: EDD-Request: EDD_SRV_MULTICAST (EDDHandle: 0x%X,pRQB: 0x%X)",channel->edd.handle_lower, pRQB );

	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

/*===========================================================================*/

/*----------------------------------------
// tcip_edd_srv_multicast callback
------------------------------------------*/
static LSA_VOID tcip_edd_srv_multicast_done(
    TCIP_CHANNEL_PTR_TYPE channel,
    TCIP_EDD_LOWER_RQB_TYPE  *pRQB
) {
	TCIP_LOG_FCT("tcip_iniche_srv_multicast_done")

	LSA_RESPONSE_TYPE result;
	LSA_INT32 dev_handle = (LSA_INT32)EDD_RQB_GET_USERID_UVAR32(pRQB);
	EDD_UPPER_MULTICAST_PTR_TYPE pSrvMulti = TCIP_EDD_GET_PPARAM( pRQB, EDD_UPPER_MULTICAST_PTR_TYPE );
	LSA_UINT8 mode;

	if (channel->state == TCIP_CH_ALLOCATED) /* return open channel rqb now, see tcip_edd_get_portparams_done() */
	{
		if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
		{
		TCIP_FATAL(); /* (!) must succeed */
		}

		tcip_init_arp_channel(channel); /* initialization of "first" channel */

		channel->state = TCIP_CH_OPEN;

		tcip_callback(channel, TCIP_OK, &channel->curr);
	}
	else if (channel->state == TCIP_CH_CLOSING) /* proceed with channel close, see tcip_close_channel() */
	{
		if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
		{
			TCIP_FATAL(); /* (!) must succeed */
		}

		tcip_edd_nrt_cancel( channel );
	}
	else
	{
		if (pSrvMulti->Mode == EDD_MULTICAST_ENABLE)
		{
			mode = TCIP_SRV_MULTICAST_ENABLE;
		}
		else
		{
			mode = TCIP_SRV_MULTICAST_DISABLE;
		}

		switch (TCIP_EDD_GET_RESPONSE(pRQB))
		{
		case EDD_STS_OK:
			result = TCIP_OK;
			break;
		case EDD_STS_ERR_PARAM:
			result = TCIP_ERR_PARAM;
			break;
		case EDD_STS_ERR_RESOURCE:
			result = TCIP_ERR_RESOURCE;
			break;
		default:
			result = TCIP_ERR_LOWER_LAYER;
		}

#if TCIP_CFG_OPEN_BSD
		if (dev_handle == TCIP_CARP_DEV_HANDLE)  /* CARP MAC Address*/
		{
			obsd_pnio_change_carp_state(CARP_MULTICAST_DONE, result);
		}
		else /* no CARP MAC Address*/
#endif /* TCIP_CFG_OPEN_BSD */
		{
			if (result != TCIP_OK)
			{
				TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "tcip_srv_multicast_done: unexpected return code from EDDx(%X)", result);
			}

			if (result != TCIP_OK && mode == TCIP_SRV_MULTICAST_DISABLE)
			{
				TCIP_FATAL(); /* (!) must succeed */
			}

			TCIP_SRV_MULTICAST_DONE(result, dev_handle, mode);
		}
	}
	tcip_FreeRQB(channel->sysptr, pRQB);
}

/*===========================================================================*/

#if TCIP_CFG_OPEN_BSD

void tcip_edd_srv_arp_filter(
	void *ch,
	unsigned char user_index,
	unsigned long cluster_ip_address
)
{
	TCIP_CHANNEL_PTR_TYPE channel = (TCIP_CHANNEL_PTR_TYPE)ch;
	TCIP_EDD_RQB_SET_ARP_FILTER_PTR_TYPE  pSetArpFilter;

	EDD_UPPER_RQB_PTR_TYPE  pRQB = tcip_AllocRQB(channel->sysptr, sizeof(EDD_RQB_NRT_SET_ARP_FILTER_TYPE));

	if (is_null(pRQB))
	{
		TCIP_FATAL(); /* a bug */
	}

	pSetArpFilter = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE);

	if (cluster_ip_address != 0)
	{
		pSetArpFilter->Mode = EDD_ARP_FILTER_ON;
	}
	else
	{
		pSetArpFilter->Mode = EDD_ARP_FILTER_OFF;
	}

	pSetArpFilter->IPAddr.IPAdr[0] = (LSA_UINT8)cluster_ip_address;
	pSetArpFilter->IPAddr.IPAdr[1] = (LSA_UINT8)(cluster_ip_address >> 8);
	pSetArpFilter->IPAddr.IPAdr[2] = (LSA_UINT8)(cluster_ip_address >> 16);
	pSetArpFilter->IPAddr.IPAdr[3] = (LSA_UINT8)(cluster_ip_address >> 24);

	if (user_index > 1)
	{
		TCIP_FATAL();
	}

	pSetArpFilter->UserIndex = user_index;

	TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_NRT_SET_ARP_FILTER);
	TCIP_EDD_SET_HANDLE(pRQB, 0);
	EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

	EDD_RQB_SET_USERID_UVAR32(pRQB, (LSA_UINT32)user_index);

	TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>>: EDD-Request: EDD_SRV_NRT_SET_ARP_FILTER (EDDHandle: 0x%X,pRQB: 0x%X)", 
		channel->edd.handle_lower, pRQB);

	TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
}

static LSA_VOID tcip_edd_srv_arp_filter_done(
	TCIP_CONST_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_TYPE  *pRQB
) {
	LSA_RESPONSE_TYPE result;
	LSA_INT32 user_index = (LSA_INT32)EDD_RQB_GET_USERID_UVAR32(pRQB);

	if (TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK)
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "tcip_edd_srv_arp_filter_done: unexpected return code from EDDx(%X)", TCIP_EDD_GET_RESPONSE(pRQB));
	}

	switch (TCIP_EDD_GET_RESPONSE(pRQB))
	{
	case EDD_STS_OK:
		result = TCIP_OK;
		break;
	case EDD_STS_ERR_PARAM:
		result = TCIP_ERR_PARAM;
		break;
	case EDD_STS_ERR_RESOURCE:
		result = TCIP_ERR_RESOURCE;
		break;
	default:
		result = TCIP_ERR_LOWER_LAYER;
	}


	if (user_index == TCIP_CARP_USER_INDEX) 
	{
		obsd_pnio_change_carp_state(CARP_FILTER_DONE, result);
	}

	tcip_FreeRQB(channel->sysptr, pRQB);
}

void tcip_edd_set_carp_multicast(void *channel, unsigned long long mac_address)
{
	TCIP_CHANNEL_PTR_TYPE ch = (TCIP_CHANNEL_PTR_TYPE)channel;
	tcip_edd_srv_multicast(ch, TCIP_CARP_DEV_HANDLE, mac_address, TCIP_SRV_MULTICAST_ENABLE);
}

void tcip_edd_reset_carp_multicast(void *channel, unsigned long long mac_address)
{
	TCIP_CHANNEL_PTR_TYPE ch = (TCIP_CHANNEL_PTR_TYPE)channel;
	tcip_edd_srv_multicast(ch, TCIP_CARP_DEV_HANDLE, mac_address, TCIP_SRV_MULTICAST_DISABLE);
}
#endif /* TCIP_CFG_OPEN_BSD */

#endif

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID
tcip_edd_request_lower_done(
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	if( is_null(pRQB) )
	{
		TCIP_FATAL();
	}
	else
	{
		TCIP_CHANNEL_PTR_TYPE ch = tcip_channel_from_handle(TCIP_EDD_GET_HANDLE(pRQB));

		if( is_null(ch) )
		{
			TCIP_FATAL();
		}
		else if( TCIP_EDD_GET_OPCODE(pRQB) == EDD_OPC_REQUEST ) /* fast path */
		{
			switch( TCIP_EDD_GET_SERVICE(pRQB) )
			{
			case EDD_SRV_GET_PARAMS:
				tcip_edd_get_params_done(ch, pRQB);
				break;

			case EDD_SRV_GET_PORT_PARAMS:
				tcip_edd_get_portparams_done(ch, pRQB);
				break;

			case EDD_SRV_NRT_SEND:
				tcip_edd_nrt_send_done(pRQB, ch);
				break;

			case EDD_SRV_NRT_RECV:
				tcip_edd_nrt_recv_done(pRQB, ch);
				break;

			case EDD_SRV_NRT_CANCEL:
				tcip_edd_nrt_cancel_done(ch, pRQB);
				break;

			case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
				tcip_edd_ext_link_status_indication(ch, pRQB);
				break;

#if TCIP_CFG_SNMP_ON
			case EDD_SRV_GET_STATISTICS_ALL:
				tcip_edd_get_mib2_statistics_done(ch, pRQB);
				break;
#endif

#if TCIP_CFG_ENABLE_MULTICAST
			case EDD_SRV_MULTICAST:
				tcip_edd_srv_multicast_done(ch, pRQB);
				break;

#if TCIP_CFG_OPEN_BSD
			case EDD_SRV_NRT_SET_ARP_FILTER:
				tcip_edd_srv_arp_filter_done(ch, pRQB);
				break;
#endif

#endif

			default:
				TCIP_FATAL();
				break;
			}
		}
		else /* slow path */
		{
			switch( TCIP_EDD_GET_OPCODE(pRQB) )
			{
			case EDD_OPC_OPEN_CHANNEL:
				tcip_edd_open_channel_done(ch, pRQB);
				break;

			case EDD_OPC_CLOSE_CHANNEL:
				tcip_edd_close_channel_done(ch, pRQB);
				break;

			default:
				TCIP_FATAL();
				break;
			}
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
