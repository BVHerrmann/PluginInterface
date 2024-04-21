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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cl20.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for nare                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   620
#define CM_MODULE_ID        620

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_cl_nare_ident (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	CM_CL_DEVICE_PTR_TYPE dev   = argr->cm.cl.dev;
	LSA_UINT32 notify_id;
	LSA_UINT16 ident_timeout;
	LSA_UINT32 local_ip_addr, local_subnet_mask, local_gateway_ip;
	CM_LIST_ENTRY_PTR_TYPE device_alias_list;
	CM_UPPER_MEM_U8_PTR_TYPE device_name;
	LSA_UINT16 device_name_length;

	if (! cm_oha_get_ip_suite (channel, &local_ip_addr, &local_subnet_mask, &local_gateway_ip))
	{
		CM_CL_DOWN_INFO_TYPE dinfo;
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "local ip_addr is not valid, can't resolve");
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_IPSUITE));
		cm_cl_bindsm_nare_error (argr, &dinfo);
		return;
	}

	if (argr->cm.cl.state == CM_DST_NARE)
	{
		if (channel->usr.cl.nos_assignment == CM_CL_CONTROL_NOS_ASSIGNMENT_ON)
		{
			device_alias_list = &dev->dev_addr.device_alias_list;
		}
		else/* CM_CL_CONTROL_NOS_ASSIGNMENT_OFF */
		{
			device_alias_list = LSA_NULL;
		}

		switch (dev->device_mode & CM_CL_DEVICE_MODE_IS_ACCELERATED_MASK)
		{
		case CM_CL_DEVICE_MODE_IS_ACCELERATED_NO:
			notify_id = 0;
			break;

		case CM_CL_DEVICE_MODE_IS_ACCELERATED_YES:
			if (! dev->bindsm.force_nsu)
			{
				notify_id = CM_CL_U32_SET_DEV_AR (argr->device_nr, argr->ar_nr);
			}
			else
			{
				notify_id = 0;
			}
			break;

		default:
			CM_FATAL1 (dev->device_mode);
			return;
		}
	}
	else
	{
		device_alias_list = LSA_NULL;
		notify_id         = 0;
	}

	{
		LSA_UINT32 scan_cycle = CM_CL_DEV_SCAN_CYCLE_10MS (dev->device_mode);
		if (10 <= scan_cycle && scan_cycle <= 200)
		{
			ident_timeout = (LSA_UINT16) (scan_cycle * 10);
		}
		else
		{
			ident_timeout = 1000;	/* part 6: "Response delay timeout = 400 ms", keep 1000 ms because of compatibility */
		}
	}

	if (dev->dev_addr.tailored_device_name_length > 0)
	{
		device_name        = dev->dev_addr.tailored_device_name;
		device_name_length = dev->dev_addr.tailored_device_name_length;
	}
	else
	{
		device_name        = dev->dev_addr.device_name;
		device_name_length = dev->dev_addr.device_name_length;
	}

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) /nare-ident:  device_name_length(%u) known-ip(0x%x) notify_id(0x%x) ident_timeout(%u)msec"
		, argr->device_nr
		, device_name_length, CM_NTOH32 (dev->dev_addr.device_ip)
		, notify_id, ident_timeout
		);

	if (! cm_nare_ident (channel, CM_CAST_U8 (device_name_length) /* range 1..240 */, device_name
			, ident_timeout, argr->device_nr, argr->ar_nr
			, notify_id, argr	/* cancel_request_id for nare-cancel */
			, device_alias_list	/* may LSA_NULL */
			, LSA_NULL	/* alloc */
			))
	{
		if (argr->cm.cl.state == CM_DST_NARE)
		{
			CM_CL_DOWN_INFO_TYPE dinfo;

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
			cm_cl_bindsm_nare_error (argr, &dinfo);
		}
		else
		{
			/* only alloc(), alias_list == LSA_NULL*/
			CM_FATAL ();
		}
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_ar_set_nare_ip (
	CM_AR_GRAPH_PTR_TYPE argr,
	NARE_UPPER_RQB_MAC_PTR_TYPE mac_addr,
	NARE_UPPER_RQB_IP_PTR_TYPE ip_addr,
	NARE_UPPER_RQB_IP_PTR_TYPE ip_subnet_mask,
	NARE_UPPER_RQB_IP_PTR_TYPE ip_gateway
	)
{
	CM_MAC_TYPE dev_mac_addr;
	LSA_UINT32 dev_ip_addr;
	LSA_UINT32 dev_ip_subnet;
	LSA_UINT32 dev_ip_gateway;

	CM_ASSERT (sizeof(*mac_addr) == sizeof(argr->ar.eth_mac));
	CM_ASSERT (sizeof(*ip_addr) == sizeof(argr->ar.host_ip));

	CM_MEMCPY (&dev_mac_addr, mac_addr, sizeof (dev_mac_addr));
	CM_MEMCPY (&dev_ip_addr,  ip_addr, sizeof (dev_ip_addr));
	CM_MEMCPY (&dev_ip_subnet, ip_subnet_mask, sizeof (dev_ip_subnet));
	CM_MEMCPY (&dev_ip_gateway, ip_gateway, sizeof (dev_ip_gateway));

	cm_cl_ar_event_set_ip (argr, &dev_mac_addr, dev_ip_addr, dev_ip_subnet, dev_ip_gateway);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_nare_set_dinfo (
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_CL_NARE_RSP_TYPE cm_rsp,
	CM_CL_DOWN_INFO_PTR_TYPE dinfo
	)
{
	switch (cm_rsp)
	{
	case CM_CL_NARE_ERR_DCP_STATION_NOT_FOUND:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOD_NOT_FOUND, CM_PNIO_FAULTY_CTLDINA (4 /*DCP no StationName*/));
		break;

	case CM_CL_NARE_ERR_DCP_MULTIPLE_STATIONS:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_NOS_MULTIPLE, CM_PNIO_FAULTY_CTLDINA (
				3 /*DCP multiple RealStationName*/), &argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case CM_CL_NARE_ERR_DCP_SET:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA (
				6 /*DCP set error*/), &argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case CM_CL_NARE_ERR_MULTIPLE_IP_USE:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_MULTIPLE, CM_PNIO_FAULTY_CTLDINA (
				3 /*DCP multiple RealStationName*/), &argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case CM_CL_NARE_ERR_LL:
		CM_CL_DINFO_SETm (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IP_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA (
				0 /*No DCP active*/), &argr->cm.cl.ev.ip_suite.device_mac);
		break;

	case CM_CL_NARE_ERR_STATION_NAME_DIFFERENT:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_NOS_SET_ABORTED, CM_PNIO_FAULTY_CTLDINA (6 /*DCP set error*/));
		break;

	case CM_CL_NARE_ERR_DCP_ID_MISMATCH:
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_VENDORID_ABORTED, CM_PNIO_FAULTY_CTLDINA (6 /*DCP set error*/));
		break;

	case CM_CL_NARE_ERR_FAULTY_TLV:
		CM_CL_DINFO_SET2 (dinfo, CM_AR_REASON_NARE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (
				CM_AR_REASON_NARE), NARE_RSP_ERR_DCP_IDENT_TLV_ERROR);
		break;

	default:
		CM_FATAL1 (cm_rsp);
		break;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_nare_get_ip_suite (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 *dev_ip_addr,
	LSA_UINT32 *dev_subnet_mask,
	LSA_UINT32 *dev_gateway_ip,
	LSA_UINT32 *local_ip_addr
	)
{
	LSA_UINT32 local_subnet_mask, local_gateway_ip;
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;

	*dev_ip_addr = dev->dev_addr.device_ip;

	if (dev->device_mode & CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_YES)
	{
		*dev_subnet_mask = dev->dev_addr.device_subnet_mask;
		*dev_gateway_ip  = dev->dev_addr.device_gateway_ip;
	}
	else if (cm_oha_get_ip_suite (argr->ar_com.channel, local_ip_addr, &local_subnet_mask, &local_gateway_ip))
	{
		*dev_subnet_mask = local_subnet_mask;

		if (local_gateway_ip == *local_ip_addr || local_gateway_ip == 0)/* means "no gateway", see part 6 */
		{
			*dev_gateway_ip = *dev_ip_addr;
		}
		else
		{
			*dev_gateway_ip = local_gateway_ip;
		}
	}
	else
	{
		/* Addresstailoring anytime possible!*/
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
CM_CL_NARE_RSP_TYPE
cm_cl_check_ip_suite (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 ip_addr,
	LSA_UINT32 subnet_mask,
	LSA_UINT32 gateway_ip
	)
{
	LSA_UINT32 dev_ip_addr     = 0;
	LSA_UINT32 dev_subnet_mask = 0;
	LSA_UINT32 dev_gateway_ip  = 0;
	LSA_UINT32 local_ip_addr   = 0;
	LSA_UINT32 nare_mode       = argr->cm.cl.dev->dev_addr.properties & CM_NARE_MODE_MASK;
	CM_CL_NARE_RSP_TYPE cm_rsp = CM_CL_NARE_OK;

	if (! cm_cl_nare_get_ip_suite (argr, &dev_ip_addr, &dev_subnet_mask, &dev_gateway_ip, &local_ip_addr))
	{
		cm_rsp = CM_CL_NARE_ERR_LL;	/* no local ip suite */
	}
	else if (nare_mode == CM_NARE_MODE_DCP_RESOLVE_IP)
	{
		if (ip_addr == 0)
		{
			cm_rsp = CM_CL_NARE_ERR_LL; /* resolve_ip: ip not set, rest don't care */
		}
	}
	else
	{
		if (dev_ip_addr != ip_addr)
		{
			if (argr->cm.cl.state == CM_DST_NARE)
			{
				cm_rsp = CM_CL_NARE_DO_SET;
			}
			else
			{
				cm_rsp = CM_CL_NARE_ERR_LL;
			}
		}
		else if (dev_subnet_mask != subnet_mask || dev_gateway_ip != gateway_ip)
		{
			switch (argr->cm.cl.state)
			{
			case CM_DST_PASSIVE:
				cm_rsp = CM_CL_NARE_OK;
				break;

			case CM_DST_NARE:
				cm_rsp = CM_CL_NARE_DO_SET;
				break;

			default:
				cm_rsp = CM_CL_NARE_ERR_LL;
				break;
			}
		}
	}

	CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "check_ip_suite: dev(%u): ip_addr(0x%08x/0x%08x) subnet_mask(0x%08x/0x%08x) gateway_ip(0x%08x/0x%08x) nare_mode(%u) cm_rsp(%u)"
		, argr->device_nr
		, ip_addr, dev_ip_addr, subnet_mask, dev_subnet_mask, gateway_ip, dev_gateway_ip, nare_mode, cm_rsp);

	return cm_rsp;
}


/*----------------------------------------------------------------------------*/
static CM_CL_NARE_RSP_TYPE
cm_cl_nare_check_ip_suite (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	NARE_IP_TYPE nare_ip_addr,
	NARE_IP_TYPE nare_subnet_mask,
	NARE_IP_TYPE nare_gateway_ip
	)
{
	LSA_UINT32 ip_addr, subnet_mask, gateway_ip;

	CM_ASSERT (NARE_IP_ADDR_SIZE == sizeof(ip_addr));

	CM_MEMCPY (&ip_addr, nare_ip_addr, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (&subnet_mask, nare_subnet_mask, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (&gateway_ip, nare_gateway_ip, NARE_IP_ADDR_SIZE);

	return cm_cl_check_ip_suite (argr, ip_addr, subnet_mask, gateway_ip);
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_nare_check_id_ok (
	CM_CL_DEVICE_CONST_PTR_TYPE dev,
	CM_NARE_LOWER_RQB_CONST_PTR_TYPE nare
	)
{
	if ((dev->device_mode & CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_MASK) == CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_NO)
	{
		return LSA_TRUE;
	}

	if (dev->dev_rpc.vendor_id == nare->args.Ident.VendorID && dev->dev_rpc.device_id == nare->args.Ident.DeviceID)
	{
		if (nare->args.Ident.InstanceIDValid == 0) /* no DCP SubOption Instance ID */
		{
			return LSA_TRUE;
		}
		else if (dev->dev_rpc.instance == nare->args.Ident.InstanceID)
		{
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}

#define CM_TOLOWER(c_) ((c_) >= 'A' && (c_) <= 'Z' ? ((c_) | 0x20) : (c_))

/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_cl_nare_check_name_equal (
	CM_CL_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_MEM_U8_CONST_PTR_TYPE name,
	LSA_UINT8 name_length
	)
{
	CM_UPPER_MEM_U8_PTR_TYPE device_name;
	LSA_UINT16 device_name_length;

	if (dev->dev_addr.tailored_device_name_length > 0)
	{
		device_name        = dev->dev_addr.tailored_device_name;
		device_name_length = dev->dev_addr.tailored_device_name_length;
	}
	else
	{
		device_name        = dev->dev_addr.device_name;
		device_name_length = dev->dev_addr.device_name_length;
	}

	if (name_length == device_name_length)
	{
		LSA_UINT8 i;

		for (i = 0; i < name_length; i++)
		{
			if (CM_TOLOWER (name[i]) != CM_TOLOWER (device_name[i]))
			{
				return LSA_FALSE;
			}
		}

		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}


/*----------------------------------------------------------------------------*/
static CM_CL_NARE_RSP_TYPE
cm_cl_nare_check_name_id (
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	CM_CHANNEL_PTR_TYPE channel              = argr->ar_com.channel;
	LSA_UINT16 device_nr                     = argr->device_nr;
	CM_UPPER_MEM_U8_PTR_TYPE identified_name = nare->args.Ident.IdentifiedStationName;
	LSA_UINT8 identified_name_length         = nare->args.Ident.IdentifiedStationNameLen;
	LSA_UINT16 idev;

	if (identified_name_length > 0)
	{
		if (cm_cl_nare_check_name_equal (argr->cm.cl.dev, identified_name, identified_name_length))
		{
			return CM_CL_NARE_OK;
		}

		if ((argr->cm.cl.dev->device_mode & CM_CL_DEVICE_MODE_NOS_OVERWRITE_MASK) == CM_CL_DEVICE_MODE_NOS_OVERWRITE_NO)
		{
			return CM_CL_NARE_ERR_STATION_NAME_DIFFERENT;
		}

		for (idev = 0; idev < channel->usr.cl.dev_cnt; ++idev)
		{
			CM_CL_DEVICE_PTR_TYPE dev = &channel->usr.cl.dev[idev];

			if (dev->state != CM_CL_DEV_ST_NOI && dev->device_nr != device_nr
				&& cm_cl_nare_check_name_equal (dev, identified_name, identified_name_length))
			{
				return CM_CL_NARE_ERR_STATION_NAME_DIFFERENT;	/* well known station name */
			}
		}
	}

	if (cm_cl_nare_check_id_ok (argr->cm.cl.dev, nare))
	{
		return CM_CL_NARE_DO_SET;
	}
	else
	{
		return CM_CL_NARE_ERR_DCP_ID_MISMATCH;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_ident_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 nare_rsp  = CM_NARE_GET_RESPONSE (nare);
	LSA_UINT16 device_nr = CM_NARE_GET_USER_ID_DEV (nare);
	LSA_UINT8 mode;
	CM_CL_NARE_RSP_TYPE cm_rsp;
	CM_CL_DOWN_INFO_TYPE dinfo;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_cl_ar_from_device (channel, device_nr);

	if (cm_is_null (argr))
	{
		CM_FATAL ();
		return;
	}

	CM_ASSERT (argr->ar_nr == CM_NARE_GET_USER_ID_AR (nare));
	CM_ASSERT (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF));

	mode = 0;

	switch (nare_rsp)
	{
	case NARE_RSP_OK:
		cm_rsp = cm_cl_nare_check_name_id (argr, nare);

		if (cm_rsp == CM_CL_NARE_DO_SET)
		{
			mode = NARE_MODE_CONFIG_NOS;
		}
		else if (cm_rsp != CM_CL_NARE_OK)
		{
			break;
		}

		cm_rsp = cm_cl_nare_check_ip_suite (argr, nare->args.Ident.IPAddr, nare->args.Ident.SubMask, nare->args.Ident.DefRouter);

		if (cm_rsp == CM_CL_NARE_OK)
		{
			cm_cl_ar_set_nare_ip (argr
				, &nare->args.Ident.MACAddr
				, &nare->args.Ident.IPAddr
				, &nare->args.Ident.SubMask
				, &nare->args.Ident.DefRouter
				);

			if (mode != 0)
			{
				cm_rsp = CM_CL_NARE_DO_SET;
			}
		}
		else if (cm_rsp == CM_CL_NARE_DO_SET)
		{
			LSA_UINT32 dev_ip_addr, dev_subnet_mask, dev_gateway_ip, local_ip_addr;
			CM_MAC_TYPE dev_mac_addr;

			if (cm_cl_nare_get_ip_suite (argr, &dev_ip_addr, &dev_subnet_mask, &dev_gateway_ip, &local_ip_addr))
			{
				CM_ASSERT (sizeof (nare->args.Ident.MACAddr) == sizeof (argr->ar.eth_mac));

				CM_MEMCPY (&dev_mac_addr, &nare->args.Ident.MACAddr, sizeof (dev_mac_addr));

				cm_cl_ar_event_set_ip (argr, &dev_mac_addr, dev_ip_addr, dev_subnet_mask, dev_gateway_ip);

				mode |= NARE_MODE_CONFIG_IP;
			}
			else
			{
				cm_rsp = CM_CL_NARE_ERR_LL;
			}
		}
		else
		{
			CM_ASSERT (CM_CL_NARE_ERR_LL);
		}

		break;

	case NARE_RSP_OK_CANCEL:
		cm_rsp = CM_CL_NARE_OK;
		break;

	case NARE_RSP_ERR_PARAM:
		CM_FATAL ();/* a bug */
		return;

	case NARE_RSP_ERR_DCP_STATION_NOT_FOUND:
		switch (argr->cm.cl.state)
		{
		case CM_DST_CONNECT:
		case CM_DST_COM_START:
		case CM_DST_COM_ACTIVATE:
		case CM_DST_PRM:
		case CM_DST_WF_APPL_READY_IND:
		case CM_DST_WF_NEW_DATA:
		case CM_DST_IN_DATA:
			/* FSU only: Identify was sent prior to device startup, we probably need to resend DCP identify */
			cm_rsp = CM_CL_NARE_DO_IDENTIFY_FSU;
			break;

		case CM_DST_PASSIVE:
		case CM_DST_WF_SCHEDULE:
		case CM_DST_NARE:
		case CM_DST_WF_DOWN:
		default:
			cm_rsp = CM_CL_NARE_ERR_DCP_STATION_NOT_FOUND;
			break;
		}

		break;

	case NARE_RSP_ERR_DCP_MULTIPLE_STATIONS:
		cm_rsp = CM_CL_NARE_ERR_DCP_MULTIPLE_STATIONS;
		cm_cl_ar_set_nare_ip (argr
			, &nare->args.Ident.MACAddr
			, &nare->args.Ident.IPAddr
			, &nare->args.Ident.SubMask
			, &nare->args.Ident.DefRouter
			);	/* AP00525178,AP00529108: set conflicting ip-addr */

		argr->cm.cl.dev->bindsm.force_nsu = LSA_TRUE;

		break;

	case NARE_RSP_ERR_DCP_IDENT_TLV_ERROR:
		cm_rsp = CM_CL_NARE_ERR_FAULTY_TLV;
		break;

	case NARE_RSP_ERR_LL:
		cm_rsp = CM_CL_NARE_ERR_LL;
		break;

	default:
		CM_FATAL1 (nare_rsp);
		return;
	}

	switch (cm_rsp)
	{
	case CM_CL_NARE_OK:
		switch (argr->cm.cl.state)
		{
		case CM_DST_PASSIVE:
			/* implicit read */
			CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u)  READ /nare-ident-done  ip(%u.%u.%u.%u)"
				, argr->device_nr
				, nare->args.Ident.IPAddr[0], nare->args.Ident.IPAddr[1]
				, nare->args.Ident.IPAddr[2], nare->args.Ident.IPAddr[3]
				);
			break;

		case CM_DST_NARE:
			/* normal startup */
			CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u)  STARTUP /nare-ident-done  ip(%u.%u.%u.%u)"
				, argr->device_nr
				, nare->args.Ident.IPAddr[0], nare->args.Ident.IPAddr[1]
				, nare->args.Ident.IPAddr[2], nare->args.Ident.IPAddr[3]
				);

			cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_NARE);
			break;

		default:
			/* ASU/FSU, always late */
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) /nare-ident-done and state(%u) => ignore"
				, argr->device_nr, argr->cm.cl.state
				);
			break;
		}

		argr->cm.cl.dev->bindsm.force_nsu = LSA_FALSE;

		cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
		break;

	case CM_CL_NARE_DO_IDENTIFY_FSU:
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) /nare-ident-done and state(%u) => fsu identify"
			, argr->device_nr, argr->cm.cl.state
			);

		cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
		break;

	case CM_CL_NARE_DO_SET:
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) /nare-ident-done and state(%u) => set"
			, argr->device_nr, argr->cm.cl.state
			);

		cm_cl_bindsm_nare_set (argr, mode);
		break;

	default:

		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u) /nare-ident-done  nare_rsp(%u) cm_rsp(%u) state(%u), set to down"
			, argr->device_nr, nare_rsp, cm_rsp, argr->cm.cl.state
			);

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_TRIGGER_LATE_ERR);

		cm_cl_nare_set_dinfo (argr, cm_rsp, &dinfo);

		cm_cl_bindsm_nare_error (argr, &dinfo);
	}

	cm_nare_free_rqb_alias (channel, nare);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_cancel_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 device_nr;
	LSA_UINT16 ar_nr;
	CM_AR_GRAPH_PTR_TYPE argr;

	device_nr = CM_NARE_GET_USER_ID_DEV (nare);
	ar_nr     = CM_NARE_GET_USER_ID_AR (nare);

	cm_nare_free_rqb (channel, nare);
	argr = cm_cl_ar_from_device (channel, device_nr);
	if (cm_is_null (argr))
	{
		CM_FATAL1 (device_nr);
		return;
	}
	CM_ASSERT (argr->ar_nr == ar_nr);

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_NARE_CANCEL_CNF);
	cm_cl_arsm (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_test_ip (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	LSA_UINT32 local_ip_addr, local_subnet_mask, local_gateway_ip;

	/* ///Open issue: PNIO_ASOM V1.2i14 Optimized IP check
	if ((dev->device_mode & CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_MASK) == CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_YES)
	{
	    cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
	    return;
	}
	*/

	if (! cm_oha_get_ip_suite (channel, &local_ip_addr, &local_subnet_mask, &local_gateway_ip))
	{
		CM_CL_DOWN_INFO_TYPE dinfo;
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "local ip_addr is not valid, can't test");
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_IPSUITE));
		cm_cl_bindsm_nare_error (argr, &dinfo);
		return;
	}

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) /nare-test-ip: ip(0x%08x)"
		, argr->device_nr, CM_NTOH32 (argr->cm.cl.ev.ip_suite.device_ip)
		);

	if (! cm_nare_test_ip (channel, local_ip_addr, argr->cm.cl.ev.ip_suite.device_ip, argr->device_nr, argr->ar_nr,
			argr /* cancel_request_id for nare-cancel */))
	{
		if (argr->cm.cl.state == CM_DST_NARE)
		{
			CM_CL_DOWN_INFO_TYPE dinfo;

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
			cm_cl_bindsm_nare_error (argr, &dinfo);
		}
		else
		{
			/* only alloc() */
			CM_FATAL ();
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_test_ip_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 nare_rsp  = CM_NARE_GET_RESPONSE (nare);
	LSA_UINT16 device_nr = CM_NARE_GET_USER_ID_DEV (nare);
	CM_CL_NARE_RSP_TYPE cm_rsp;
	CM_CL_DOWN_INFO_TYPE dinfo;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_cl_ar_from_device (channel, device_nr);

	if (cm_is_null (argr))
	{
		CM_FATAL ();
		return;
	}

	CM_ASSERT (argr->ar_nr == CM_NARE_GET_USER_ID_AR (nare));
	CM_ASSERT (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF));

	switch (nare_rsp)
	{
	case NARE_RSP_OK:
	case NARE_RSP_OK_CANCEL:
		cm_rsp = CM_CL_NARE_OK;
		break;

	case NARE_RSP_ERR_MULTIPLE_IP_USE:
		cm_rsp = CM_CL_NARE_ERR_MULTIPLE_IP_USE;
		break;

	case NARE_RSP_ERR_PARAM:
	case NARE_RSP_ERR_IP_IN_USE:
	default:
		CM_FATAL1 (nare_rsp);
		return;
	}

	/* update argr-data */
	CM_ASSERT (sizeof(nare->args.TestIP.MACAddr) == sizeof(argr->ar.eth_mac));
	CM_MEMCPY (&argr->ar.eth_mac, &nare->args.TestIP.MACAddr, sizeof (argr->ar.eth_mac));
	argr->cm.cl.ev.ip_suite.device_mac = argr->ar.eth_mac;

	switch (cm_rsp)
	{
	case CM_CL_NARE_OK:
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) /nare-test-ip-done"
			, device_nr
			);

		cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
		break;

	default:
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u) /nare-test-ip-done  nare_rsp(%u) cm_rsp(%u), set to down"
			, device_nr, nare_rsp, cm_rsp
			);

		argr->cm.cl.dev->bindsm.force_nsu = LSA_TRUE;

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_TRIGGER_LATE_ERR);

		cm_cl_nare_set_dinfo (argr, cm_rsp, &dinfo);

		cm_cl_bindsm_nare_error (argr, &dinfo);
	}

	cm_nare_free_rqb (channel, nare);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_set (
	CM_AR_GRAPH_PTR_TYPE argr
	)
{
	CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;
	LSA_UINT32 dev_ip_addr, dev_subnet_mask, dev_gateway_ip, local_ip_addr;
	LSA_UINT16 name_length;
	CM_UPPER_MEM_U8_PTR_TYPE name;
	LSA_BOOL set_ip_remanent;
	LSA_UINT8 mode = argr->cm.cl.dev->bindsm.nare_mode;

	if (! cm_cl_nare_get_ip_suite (argr, &dev_ip_addr, &dev_subnet_mask, &dev_gateway_ip, &local_ip_addr))
	{
		CM_CL_DOWN_INFO_TYPE dinfo;
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "local ip_addr is not valid, can't set");
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_IPSUITE));
		cm_cl_bindsm_nare_error (argr, &dinfo);
		return;
	}

	if ((mode & NARE_MODE_CONFIG_IP) == 0)
	{
		/* sanity */
		dev_ip_addr     = 0;
		dev_subnet_mask = 0;
		dev_gateway_ip  = 0;
		set_ip_remanent = 0;
	}
	else
	{
		switch (dev->dev_addr.properties & CM_NARE_MODE_SETIP_MASK)
		{
		case CM_NARE_MODE_SETIP_TEMPORARY:
			set_ip_remanent = LSA_FALSE;
			break;

		case CM_NARE_MODE_SETIP_REMANENT:
			set_ip_remanent = LSA_TRUE;
			break;

		default:
			CM_FATAL1 (dev->dev_addr.properties);
			return;
		}
	}

	if ((mode & NARE_MODE_CONFIG_NOS) == 0)
	{
		/* sanity */
		name_length = 0;
		name        = LSA_NULL;
	}
	else
	{
		if (dev->dev_addr.tailored_device_name_length > 0)
		{
			name        = dev->dev_addr.tailored_device_name;
			name_length = dev->dev_addr.tailored_device_name_length;
		}
		else
		{
			name        = dev->dev_addr.device_name;
			name_length = dev->dev_addr.device_name_length;
		}

		CM_ASSERT (name_length <= 0xFF);
	}


	if (! cm_nare_set (argr->ar_com.channel
			, mode, (name_length & 0xFF), name
			, dev_ip_addr, dev_subnet_mask, dev_gateway_ip, argr->cm.cl.ev.ip_suite.device_mac
			, set_ip_remanent, dev->device_nr, argr->ar_nr, argr /* request id */))
	{
		if (argr->cm.cl.state == CM_DST_NARE)
		{
			CM_CL_DOWN_INFO_TYPE dinfo;

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA (CM_AR_REASON_MEM));
			cm_cl_bindsm_nare_error (argr, &dinfo);
		}
		else
		{
			/* only alloc() */
			CM_FATAL ();
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_set_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 nare_rsp  = CM_NARE_GET_RESPONSE (nare);
	LSA_UINT16 device_nr = CM_NARE_GET_USER_ID_DEV (nare);
	CM_CL_NARE_RSP_TYPE cm_rsp;
	CM_CL_DOWN_INFO_TYPE dinfo;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_cl_ar_from_device (channel, device_nr);

	if (cm_is_null (argr))
	{
		CM_FATAL ();
		return;
	}

	CM_ASSERT (argr->ar_nr == CM_NARE_GET_USER_ID_AR (nare));
	CM_ASSERT (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_NARE_CNF));

	switch (nare_rsp)
	{
	case NARE_RSP_OK:
	case NARE_RSP_OK_CANCEL:
		cm_rsp = CM_CL_NARE_OK;
		break;

	case NARE_RSP_ERR_PARAM:
		CM_FATAL ();/* a bug */
		return;

	case NARE_RSP_ERR_DCP_SET_FAILED:
		cm_rsp = CM_CL_NARE_ERR_DCP_SET;
		break;

	case NARE_RSP_ERR_LL:
		cm_rsp = CM_CL_NARE_ERR_LL;
		break;

	default:
		CM_FATAL1 (nare_rsp);
		return;
	}

	switch (cm_rsp)
	{
	case CM_CL_NARE_OK:
		if (argr->cm.cl.state == CM_DST_NARE)
		{
			cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_NARE);
		}

		cm_cl_bindsm_trigger (argr, CM_CL_WF_NARE_CNF);
		break;

	default:

		CM_CL_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u) /nare-identify-done  nare_rsp(%u) cm_rsp(%u) state(%u), set to down"
			, argr->device_nr, nare_rsp, cm_rsp, argr->cm.cl.state
			);

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_TRIGGER_LATE_ERR);

		cm_cl_nare_set_dinfo (argr, cm_rsp, &dinfo);

		cm_cl_bindsm_nare_error (argr, &dinfo);
	}

	cm_nare_free_rqb (channel, nare);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_cl_nare_notify_ind (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	NARE_UPPER_IDENT_NOTIFY_PTR_TYPE notify
	)
{
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 device_nr;
	CM_CL_NARE_RSP_TYPE cm_rsp;

	if (notify->Ident.MACAddr[0] == 0 && notify->Ident.MACAddr[1] == 0 && notify->Ident.MACAddr[2] == 0
		&&  notify->Ident.MACAddr[3] == 0 && notify->Ident.MACAddr[4] == 0 && notify->Ident.MACAddr[5] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "nare_notify_ind: ignored, no mac-addr");
		return;
	}

	device_nr = CM_CL_U32_GET_DEVICE_NR (notify->NotifyID);

	argr = cm_cl_ar_from_device (channel, device_nr);
	if (cm_is_null (argr) || argr->ar_nr != CM_CL_U32_GET_AR_NR (notify->NotifyID))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "notify.ind to unknown ar, NotifyID(0x%x)", notify->NotifyID
			);
		return;
	}

	cm_rsp = cm_cl_nare_check_ip_suite (argr, notify->Ident.IPAddr, notify->Ident.SubMask, notify->Ident.DefRouter);
	if (cm_rsp != CM_CL_NARE_OK)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "nare_notify_ind: ignored, invalid IP Suite");
		return;
	}

	switch (argr->cm.cl.state)
	{
	case CM_DST_NARE:
		cm_cl_ar_set_nare_ip (argr
			, &notify->Ident.MACAddr
			, &notify->Ident.IPAddr, &notify->Ident.SubMask, &notify->Ident.DefRouter
			);

		/* notify.ind contains the same data as nare_ident.cnf does */
		CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u)  STARTUP /accelerated  ip(%u.%u.%u.%u)"
			, argr->device_nr
			, notify->Ident.IPAddr[0], notify->Ident.IPAddr[1]
			, notify->Ident.IPAddr[2], notify->Ident.IPAddr[3]
			);

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_NARE_ASU);

		cm_cl_bindsm_startup_optimized (argr, LSA_FALSE);
		break;

	default:
		/* ignore notify.ind */
		break;
	}
}

/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
