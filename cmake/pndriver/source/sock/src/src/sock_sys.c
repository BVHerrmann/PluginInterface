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
/*  F i l e               &F: sock_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  system interface                                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  2
#define SOCK_MODULE_ID     2

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*lint -esym(765,TCIP_SRV_CLUSTER_IP_CONFIG_SET_DONE)*/
/*lint -esym(765,TCIP_SRV_CLUSTER_IP_ADDR_CONFLICT_DETECTED)*/

/* controls whether the keep alive mechanism is used (affects TCP conn only!)*/
LSA_BOOL SOCK_KEEPALIVE_ACTIVE;

#ifdef SOCK_CLEAR_GLOBAL_DATA
SOCK_DATA       sock_data = { 0 };
#else
SOCK_DATA       sock_data;
#endif

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
// initialization of global data
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_init(LSA_VOID)
{
	SOCK_LOG_FCT("sock_init")

	LSA_INT32 i;

	SOCK_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
		"Component SOCK is initialized SOCK_CFG_MAX_SOCKETS = %d", SOCK_CFG_MAX_SOCKETS);

	sock_data.error.error_code[0] = 0;
	sock_data.error.error_code[1] = 0;
	sock_data.error.error_code[2] = 0;
	sock_data.error.error_code[3] = 0;
	sock_data.error.error_data_length = 0;
	sock_data.error.error_data_ptr = LSA_NULL;
	sock_data.error.line = 0;
	sock_data.error.lsa_component_id = 0;
	sock_data.error.module_id = 0;

	sock_data.sock_channels_opened = 0;

	for(i=0; i<SOCK_CFG_MAX_CHANNELS; i++)
	{
		sock_data.sock_channels[i].state = SOCK_CHANNEL_CLOSED;
	}
	sock_data.socket_count = 0;

	sock_data.free_head = 0;
	for(i = 0; i <SOCK_CFG_MAX_SOCKETS; i++)
	{
		sock_data.sockets[i].state = SOCK_CN_STATE_FREE;
		sock_data.sockets[i].dev_handle = i; /* always > 0 */
		sock_data.sockets[i].free_next = i+1; /* for last element free_next is SOCK_CFG_MAX_SOCKETS */
	}

	SOCK_KEEPALIVE_ACTIVE = LSA_TRUE;   /* default is using the "keepalive" mechanism */

#if SOCK_CFG_SNMP_ON && !SOCK_CFG_OPEN_BSD

	sock_data.snmp_intern_auto = LSA_FALSE;
	sock_data.snmp_extern_count = 0;
	sock_data.snmp_dev = LSA_NULL;
	sock_data.snmp_recv_buf = LSA_NULL;
	sock_data.snmp_int_agent_return_buf = LSA_NULL;

	sock_data.snmp_data = LSA_NULL;
	sock_data.snmp_data_length = 0;
	sock_data.snmp_pending_count = 0;

	for(i=0; i<SOCK_MAX_SNMP_AGENTS; i++)
	{
		sock_data.snmp_agents_data[i].state = SOCK_SNMP_INACTIVE;
		sock_data.snmp_agents_data[i].snmp_agent_req  = LSA_NULL;
		sock_data.snmp_agents_data[i].hChannel = SOCK_INVALID_HANDLE;
	}
#endif

	/*
	 * mib2 sysgroup data mirror
	 */
	sock_data.sysName_buf[0] = 0;
	sock_data.sysName.pName = sock_data.sysName_buf;
	sock_data.sysName.Length = 0;
	sock_data.sysName.Remanent = SOCK_NAME_REMANENT_NO;

	sock_data.sysDescr_buf[0] = 0;
	sock_data.sysDescr.pName = sock_data.sysDescr_buf;
	sock_data.sysDescr.Length = 0;
	sock_data.sysDescr.Remanent = SOCK_NAME_REMANENT_NO;

	sock_data.sysContact_buf[0] = 0;
	sock_data.sysContact.pName  = sock_data.sysContact_buf;
	sock_data.sysContact.Length = 0;
	sock_data.sysContact.Remanent = SOCK_NAME_REMANENT_NO;

	sock_data.sysLocation_buf[0] = 0;
	sock_data.sysLocation.pName  = sock_data.sysLocation_buf;
	sock_data.sysLocation.Length = 0;
	sock_data.sysLocation.Remanent = SOCK_NAME_REMANENT_NO;

	/*
	 * address info mirror
	 */
	SOCK_MEMSET(sock_data.addr_info_buf, 0, sizeof(sock_data.addr_info_buf));
	for(i=1; i<=SOCK_MAX_INTERFACE_ID; i++)
	{
		SOCK_ADDR_INFO_PTR_TYPE ai = &sock_data.addr_info[i-1];

		ai->InterfaceID = (LSA_UINT32)i; /* 1..MAX_INTERFACE_ID */

		ai->NoS.pName = sock_data.addr_info_buf[i-1];
		ai->NoS.Length = 0;
		ai->NoS.Remanent = SOCK_NAME_REMANENT_NO;

		ai->CurrentIPSuite.IpAddr.s_addr = 0;
		ai->CurrentIPSuite.NetMask.s_addr = 0;

		ai->ExpectedIPSuite.IpAddr.s_addr = 0;
		ai->ExpectedIPSuite.NetMask.s_addr = 0;

		ai->EmergencyIPSuite.IpAddr.s_addr = 0;
		ai->EmergencyIPSuite.NetMask.s_addr = 0;

		ai->CurrentIPSuiteIsEmergency = SOCK_CURRENT_IPSUITE_IS_EMERGENCY_NO;
		ai->CurrentIPSuiteConflictDetected = SOCK_CURRENT_IPSUITE_CONFLICT_DETECTED_NO;
	}

#if SOCK_CFG_CLUSTER_IP_SUPPORTED
	{
	int interface_id;
	for(interface_id=1; interface_id<=SOCK_MAX_INTERFACE_ID; interface_id++)
	{
		SOCK_CLUSTER_IP_IND_PTR_TYPE ci = &sock_data.cluster_ip.cluster_info[interface_id-1];

		ci->Info.InterfaceID = (LSA_UINT32)interface_id;
		ci->Info.ClusterID = 0;
		ci->Info.ClusterIPState = 0;
		ci->Info.ClusterIPAddress.S_un.S_addr = 0;
		ci->Info.ClusterSubnetMask.S_un.S_addr = 0;

		ci->ClusterIPConflict = 0; /* == SOCK_CLUSTER_IP_NO_CONFLICT */
	}
	}
	sock_data.cluster_ip.cluster_info_count = 0;
	sock_data.cluster_ip.config_set_rqb = LSA_NULL;
#endif

	/*
	 *  (!) for internal use: posted by TCIP_TRIGGER_SELECT() only (!)
	 */
#if !SOCK_INT_CFG_SOCKET_EVENTS
	SOCK_RQB_SET_OPCODE( &sock_data.select_trigger_rqb, SOCK_OPC_SELECT_TRIGGER );
	sock_data.select_trigger_rqb.args.select_trigger.inUse = 0;

	sock_data.active_sockets_last = -1; /* -1 = empty */

	/* flag and timer for periodic tcp connection check */
	sock_data.do_tcp_disconnect_check = LSA_FALSE;
	{
	LSA_UINT16 ret;

	SOCK_ALLOC_TIMER(&ret, &sock_data.tcp_disconnect_check_timer_id, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_1S);

	if (ret != LSA_RET_OK)
	{
		SOCK_FATAL();
	}
	}
#endif

	sock_data.close_channel_rqb = LSA_NULL;

	sock_low_init();

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
// undo initialization of global data
//----------------------------------------------------------------------------*/
LSA_UINT16
sock_undo_init(LSA_VOID)
{
	LSA_INT i;

	for(i=0; i<SOCK_CFG_MAX_CHANNELS; i++) /* sanity */
	{
		SOCK_ASSERT(sock_data.sock_channels[i].state == SOCK_CHANNEL_CLOSED);
	}

	/***/

#if !SOCK_INT_CFG_SOCKET_EVENTS
	{
	LSA_UINT16 ret_val;
	SOCK_FREE_TIMER(&ret_val, sock_data.tcp_disconnect_check_timer_id);

	if (ret_val != LSA_RET_OK)
	{
		SOCK_FATAL();
	}
	}
#endif

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
// free channel structure
//----------------------------------------------------------------------------*/
LSA_VOID
sock_channel_free(
	SOCK_CHANNEL_PTR_TYPE channel
) {
	channel->own_handle = 0;
	channel->upper_handle = 0;
	channel->rqb_ptr = LSA_NULL;
	channel->req_done_ptr = LSA_NULL;
	channel->detail_ptr = LSA_NULL;
	return;
}


/*------------------------------------------------------------------------------
// get channel from handle
//----------------------------------------------------------------------------*/
SOCK_CHANNEL_PTR_TYPE
sock_channel_from_handle(
	LSA_HANDLE_TYPE handle
) {
	SOCK_LOG_FCT("sock_channel_from_handle")

	if( handle < SOCK_CFG_MAX_CHANNELS )
	{
		/* see sock_open_channel() */
		return &sock_data.sock_channels[handle];
	}

	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
		, "handle(%u) out of range"
		, handle
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// timer callback (see sock_cfg.c)
//----------------------------------------------------------------------------*/
LSA_VOID
sock_timeout(
  LSA_TIMER_ID_TYPE timer_id, /* timer-id */
	LSA_USER_ID_TYPE user_id /* */
)
{
	SOCK_LOG_FCT("sock_timeout")

	LSA_UNUSED_ARG(user_id);

#if !SOCK_INT_CFG_SOCKET_EVENTS
	if (timer_id == sock_data.tcp_disconnect_check_timer_id)
	{
		SOCK_EXCHANGE_TYPE rqb_in_use;
		SOCK_UPPER_RQB_PTR_TYPE tr = &sock_data.select_trigger_rqb;

		sock_data.do_tcp_disconnect_check = LSA_TRUE; /* check all tcp sockets, see sock_socket_exectute_select() */

		rqb_in_use = SOCK_INTERLOCKED_EXCHANGE(&(tr->args.select_trigger.inUse), 1);

		if (rqb_in_use == 0) /* see sock_usr_close_channel() */
		{
			SOCK_REQUEST_LOCAL(tr);
		}
	}
	else
	{
		SOCK_FATAL();
	}
#else
	LSA_UNUSED_ARG(timer_id);
#endif
}


/*------------------------------------------------------------------------------
// fatal error function, no return allowed
//----------------------------------------------------------------------------*/
LSA_VOID
sock_fatal(
	LSA_UINT16 sock_module_id,
	LSA_INT line
) {
	sock_fatal1(sock_module_id, line, 0);
}


LSA_VOID
sock_fatal1(
	LSA_UINT16 sock_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
) {
	SOCK_LOG_FCT("sock_fatal1")

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "module_id(%u) line(%u) ec_0(0x%x)"
		, sock_module_id, line, ec_0
		);

	sock_data.error.lsa_component_id = LSA_COMP_ID_SOCK;
	sock_data.error.module_id        = sock_module_id;
	sock_data.error.line             = (LSA_UINT16)line;

	sock_data.error.error_code[0]    = ec_0;
	sock_data.error.error_code[1]    = 0;
	sock_data.error.error_code[2]    = 0;
	sock_data.error.error_code[3]    = 0;

	sock_data.error.error_data_length = 0;
	sock_data.error.error_data_ptr    = LSA_NULL;

	SOCK_FATAL_ERROR(sizeof(sock_data.error), &sock_data.error);
	/* not reached */
}


/*------------------------------------------------------------------------------
// fatal error function, no return allowed
//----------------------------------------------------------------------------*/
LSA_VOID
socks_fatal_error(
	LSA_UINT16 module,
	LSA_UINT16 line,
	LSA_UINT16 error_data_length,
	LSA_VOID_PTR_TYPE error_data_ptr,
	LSA_UINT32 error_code0,
	LSA_UINT32 error_code1,
	LSA_UINT32 error_code2,
	LSA_UINT32 error_code3
) {
	SOCK_LOG_FCT("socks_fatal_error")
	SOCK_SYSTEM_TRACE_08(0, LSA_TRACE_LEVEL_FATAL
		, "Fatal error has occurred in SOCK: module=0x%x, line=%d, error_data_length=%d, error_data_ptr=0x%x, code0=0x%x, code1=0x%x, code2=0x%x, code3=0x%x"
		, module, line, error_data_length, error_data_ptr, error_code0, error_code1, error_code2, error_code3
		);


	sock_data.error.lsa_component_id = LSA_COMP_ID_SOCK;
	sock_data.error.module_id = (LSA_UINT16)module;
	sock_data.error.line = (LSA_UINT16)line;
	sock_data.error.error_data_length = error_data_length;
	sock_data.error.error_data_ptr = error_data_ptr;
	sock_data.error.error_code[0] = error_code0;
	sock_data.error.error_code[1] = error_code1;
	sock_data.error.error_code[2] = error_code2;
	sock_data.error.error_code[3] = error_code3;

	SOCK_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &sock_data.error);
	/* not reached */
}

/*------------------------------------------------------------------------------
// SNMP callback
//----------------------------------------------------------------------------*/

LSA_VOID
sock_mib2_sysgroup_written(
	char *pSysName,    /* != LSA_NULL if written */
	char *pSysContact, /* != LSA_NULL if written */
	char *pSysLocation /* != LSA_NULL if written */
) {
	LSA_UINT16 len;

	/*
	 *  copy to local storage
	 */
	if( sock_is_not_null(pSysName) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysName);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysName.pName, pSysName, len );
			sock_data.sysName.Length = len;
			sock_data.sysName.Remanent = SOCK_NAME_REMANENT_YES;
		}
	}

	if( sock_is_not_null(pSysContact) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysContact);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysContact.pName, pSysContact, len );
			sock_data.sysContact.Length = len;
			sock_data.sysContact.Remanent = SOCK_NAME_REMANENT_YES;
		}
	}

	if( sock_is_not_null(pSysLocation) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysLocation);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysLocation.pName, pSysLocation, len );
			sock_data.sysLocation.Length = len;
			sock_data.sysLocation.Remanent = SOCK_NAME_REMANENT_YES;
		}
	}

	/*
	 *  return rqb if available
	 */
	sock_mib2_check_sys_changes();
}

/*------------------------------------------------------------------------------
// InterNiche callbacks
//----------------------------------------------------------------------------*/

#if SOCK_CFG_INTERNICHE || SOCK_CFG_OPEN_BSD

/*-----------------------------------------------
// return pending SOCK_OPC_UDP_MULTICAST_OPEN
//----------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_VOID
TCIP_SRV_MULTICAST_DONE(
	LSA_RESPONSE_TYPE result,
	LSA_INT32 dev_handle,
	LSA_UINT8 mode
){
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc = sock_socket_from_handle(dev_handle);

	SOCK_ASSERT(sock_is_not_null(soc));
	SOCK_ASSERT(sock_is_not_null(soc->channel));
	SOCK_ASSERT(sock_is_not_null(soc->open_close_rb));

	if (mode == TCIP_SRV_MULTICAST_ENABLE) /* return pending open rb */
	{
		switch(result)
		{
		case TCIP_OK:
			{
			SOCK_RQB_ARGS_UDP_OPEN_MULTICAST_PTR_TYPE open_multicast = &soc->open_close_rb->args.udp_open_multicast;

			soc->state = SOCK_CN_STATE_CONNECTED; /* was disabled until now */

			soc->mcast_interface_id = open_multicast->interface_id;
			soc->mcast_address = open_multicast->group_addr.s_addr;

			open_multicast->sock_fd = dev_handle;
			}
			response = SOCK_RSP_OK;
			break;

		case TCIP_ERR_PARAM:
			response = SOCK_RSP_ERR_PARAM;
			break;

		case TCIP_ERR_RESOURCE:
			response = SOCK_RSP_ERR_RESOURCE;
			break;

		default:
			response = SOCK_RSP_ERR_LOC_NET;
		}

		sock_usr_callback(soc->channel, response, &soc->open_close_rb);

		if( response != SOCK_RSP_OK )
		{
			sock_socket_free(soc); /* close socket if multicast MAC address was not successfully written */
		}
	}
	else /* TCIP_SRV_MULTICAST_DISABLE: return pending close rb */
	{
		soc->mcast_interface_id = 0; /* mcast resource free now, see sock_socket_close() */

		sock_socket_close(soc); /* return pending rb */
	}
}
#endif


#if !SOCK_INT_CFG_SOCKET_EVENTS
LSA_VOID
TCIP_TRIGGER_SELECT(
	LSA_VOID
)
{
	SOCK_EXCHANGE_TYPE rqb_in_use;

	SOCK_UPPER_RQB_PTR_TYPE tr = &sock_data.select_trigger_rqb;

	rqb_in_use = SOCK_INTERLOCKED_EXCHANGE(&(tr->args.select_trigger.inUse), 1);

	if( rqb_in_use == 0 )
	{
		/* send local trigger-RQB in order not to recurse into interniche stack */
		SOCK_REQUEST_LOCAL(tr);
	}
}
#endif

LSA_VOID
TCIP_MIB2_SYSGROUP_DATA_WRITTEN(
	LSA_CHAR * ptr_sysName,
	LSA_CHAR * ptr_sysContact,
	LSA_CHAR * ptr_sysLocation
) {
	sock_mib2_sysgroup_written(ptr_sysName, ptr_sysContact, ptr_sysLocation);
}

#endif


/*------------------------------------------------------------------------------
// copy cluster ip config data to local database
//----------------------------------------------------------------------------*/
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
static LSA_VOID sock_usr_cluster_ip_config_data_store(
	SOCK_CLUSTER_IP_CONFIG_PTR_TYPE pClusterIP
) {
	SOCK_CLUSTER_IP_INFO_PTR_TYPE pClusterInfo;

	SOCK_ASSERT(pClusterIP->InterfaceID >= 1 && pClusterIP->InterfaceID <= SOCK_MAX_INTERFACE_ID);
	SOCK_ASSERT(pClusterIP->InterfaceID == sock_data.cluster_ip.cluster_info[pClusterIP->InterfaceID-1].Info.InterfaceID);

	pClusterInfo = &sock_data.cluster_ip.cluster_info[pClusterIP->InterfaceID-1].Info;

	pClusterInfo->ClusterID = pClusterIP->ClusterID;
	pClusterInfo->ClusterIPAddress = pClusterIP->ClusterIPAddress;
	pClusterInfo->ClusterSubnetMask = pClusterIP->ClusterSubnetMask;

	/* pClusterInfo->ClusterIPState is not set by SOCK_OPC_CLUSTER_IP_CONFIG_SET */
	/* ClusterIPState is SOCK_CLUSTER_IP_STATE_INACTIVE until SOCK_OPC_CLUSTER_IP_STATE_SET is provided with different setting */

	/* after set (or reset) update cluster_info_count */

	{
	int i;

	sock_data.cluster_ip.cluster_info_count = 0;

	for (i=0; i<SOCK_MAX_INTERFACE_ID; i++)
	{
		if (sock_data.cluster_ip.cluster_info[i].Info.ClusterID)
		{
			sock_data.cluster_ip.cluster_info_count++;
		}
	}
	}
}
#endif


/*-----------------------------------------------
// return pending SOCK_OPC_CLUSTER_IP_CONFIG_SET
//----------------------------------------------*/
void TCIP_SRV_CLUSTER_IP_CONFIG_SET_DONE(unsigned int interface_id, int result)
{
#if SOCK_CFG_OPEN_BSD && SOCK_CFG_CLUSTER_IP_SUPPORTED
	LSA_RESPONSE_TYPE response;
	SOCK_CHANNEL_PTR_TYPE channel;
	SOCK_UPPER_RQB_PTR_TYPE rqb = sock_data.cluster_ip.config_set_rqb;

	LSA_UNUSED_ARG(interface_id);

	SOCK_ASSERT(sock_is_not_null(rqb));

	channel = sock_channel_from_handle(SOCK_RQB_GET_HANDLE(rqb));

	SOCK_ASSERT(sock_is_not_null(channel));

	switch(result)
	{
	case TCIP_OK:
		sock_usr_cluster_ip_config_data_store(&rqb->args.cluster_ip_config); /* transfer data to local mirror */
		
		response = SOCK_RSP_OK;
		break;
	case TCIP_ERR_PARAM:
		response = SOCK_RSP_ERR_PARAM;
		break;
	case TCIP_ERR_RESOURCE:
		response = SOCK_RSP_ERR_RESOURCE;
		break;
	case TCIP_ERR_LOWER_LAYER:
	default:
		response = SOCK_RSP_ERR_LOC_NET;
		break;
	}

	sock_usr_cluster_ip_ind_data_changes_check(rqb->args.cluster_ip_config.InterfaceID); /* always indicate users */

	sock_usr_callback(channel, response, &sock_data.cluster_ip.config_set_rqb); /* sets curr to LSA_NULL */
#else
	LSA_UNUSED_ARG(interface_id);
	LSA_UNUSED_ARG(result);
#endif
}



/*------------------------------------------------------------------------------
// return pending cluster ip indications
//----------------------------------------------------------------------------*/
#if SOCK_CFG_CLUSTER_IP_SUPPORTED
static void sock_usr_cluster_ip_addr_conflict_detected(LSA_UINT32 address)
{
	int i;

	for (i=0; i<SOCK_MAX_INTERFACE_ID; i++)
	{
		if (sock_data.addr_info[i].CurrentIPSuite.IpAddr.s_addr == address)
		{
			break;
		}
	}

	SOCK_ASSERT(i < SOCK_MAX_INTERFACE_ID); /* must be there */

	sock_data.cluster_ip.cluster_info[sock_data.addr_info[i].InterfaceID - 1].ClusterIPConflict = SOCK_CLUSTER_IP_CONFLICT_DETECTED;

	sock_usr_cluster_ip_ind_data_changes_check(sock_data.addr_info[i].InterfaceID); /* indicate users */
}
#endif


/*-----------------------------------------------
// Cluster IP address conflict detected
//----------------------------------------------*/

void TCIP_SRV_CLUSTER_IP_ADDR_CONFLICT_DETECTED(unsigned int address)
{
#if SOCK_CFG_OPEN_BSD && SOCK_CFG_CLUSTER_IP_SUPPORTED
	sock_usr_cluster_ip_addr_conflict_detected(address);
#else
	LSA_UNUSED_ARG(address);
#endif
}


/*-----------------------------------------------
// return pending SOCK_CLUSTER_IP_CONFIG_SET
//----------------------------------------------*/
#if SOCK_CFG_CUSTOM && SOCK_CFG_CLUSTER_IP_SUPPORTED
void SOCK_CLUSTER_IP_CONFIG_SET_DONE(LSA_UINT32 interface_id, LSA_UINT8 result)
{
	LSA_RESPONSE_TYPE response;
	SOCK_CHANNEL_PTR_TYPE channel;
	SOCK_UPPER_RQB_PTR_TYPE rqb = sock_data.cluster_ip.config_set_rqb;

	LSA_UNUSED_ARG(interface_id);

	SOCK_ASSERT(sock_is_not_null(rqb));

	channel = sock_channel_from_handle(SOCK_RQB_GET_HANDLE(rqb));

	SOCK_ASSERT(sock_is_not_null(channel));

	switch(result)
	{
	case SOCK_CLUSTER_IP_OK:
		sock_usr_cluster_ip_config_data_store(&rqb->args.cluster_ip_config); /* transfer data to local mirror */
		
		response = SOCK_RSP_OK;

		break;

	case SOCK_CLUSTER_IP_NOK_PARAMETER:
		response = SOCK_RSP_ERR_PARAM;

		break;

	case SOCK_CLUSTER_IP_NOK_LOC_NET:
	default:
		response = SOCK_RSP_ERR_LOC_NET;
		break;
	}
	sock_usr_cluster_ip_ind_data_changes_check(rqb->args.cluster_ip_config.InterfaceID); /* always indicate users */

	sock_usr_callback(channel, response, &sock_data.cluster_ip.config_set_rqb); /* sets config_set_rqb to LSA_NULL */
}
#endif


/*-----------------------------------------------
// Cluster IP address conflict detected
//----------------------------------------------*/
#if SOCK_CFG_CUSTOM && SOCK_CFG_CLUSTER_IP_SUPPORTED
void SOCK_CLUSTER_IP_ADDR_CONFLICT_DETECTED(unsigned int address)
{
	sock_usr_cluster_ip_addr_conflict_detected(address);
}
#endif


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
