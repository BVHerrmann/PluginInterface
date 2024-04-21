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
/*  F i l e               &F: cm_cl22.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for oha                                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   622
#define CM_MODULE_ID        622

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_cl_ar_set_oha_ip (
	CM_AR_GRAPH_PTR_TYPE argr,
	OHA_MAC_TYPE * mac_addr,
	OHA_IP_TYPE  * ip_addr,
	OHA_IP_TYPE  * ip_subnet_mask,
	OHA_IP_TYPE  * ip_gateway
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
LSA_VOID
cm_cl_oha_hello_ind (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	OHA_UPPER_HELLO_PTR_TYPE hello
	)
{
	CM_AR_GRAPH_PTR_TYPE argr = cm_cl_ar_from_device (channel
		, CM_CL_U32_GET_DEVICE_NR (hello->FilterID)
		);

	if (cm_is_null (argr) || argr->ar_nr != CM_CL_U32_GET_AR_NR (hello->FilterID))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "hello.ind to unknown ar, FilterID(0x%x)", hello->FilterID
			);
		return;
	}

	if (argr->cm.cl.dev->bindsm.force_nsu)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "hello.ind ignored, nsu forced");
		return;
	}

	if (hello->MacAddr[0] == 0 && hello->MacAddr[1] == 0 && hello->MacAddr[2] == 0
		&&  hello->MacAddr[3] == 0 && hello->MacAddr[4] == 0 && hello->MacAddr[5] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "hello.ind ignored, no mac-addr");
		return;
	}

	if (hello->IPAddr[0] == 0 && hello->IPAddr[1] == 0 && hello->IPAddr[2] == 0 && hello->IPAddr[3] == 0)
	{
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "hello.ind ignored, no ip-addr");
		return;
	}

	if (argr->cm.cl.dev->state != CM_CL_DEV_ST_ACTIVE)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u): not active, state(%u), ignore hello.ind"
			, argr->device_nr, argr->cm.cl.dev->state
			);
		return;
	}

	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_SCHEDULE:
	case CM_DST_NARE:
		/* check ip-suite if we know the right one, AP00492906 */
		{
			CM_CL_DEVICE_PTR_TYPE dev = argr->cm.cl.dev;
			LSA_UINT32 ip_addr, subnet_mask, gateway_ip;

			CM_ASSERT (sizeof(hello->IPAddr) == sizeof(ip_addr));

			CM_MEMCPY (&ip_addr,  hello->IPAddr, sizeof (ip_addr));
			CM_MEMCPY (&subnet_mask, hello->SubnetMask, sizeof (subnet_mask));
			CM_MEMCPY (&gateway_ip, hello->DefaultRouterAddr, sizeof (gateway_ip));

			if (cm_cl_check_ip_suite (argr, ip_addr, subnet_mask, gateway_ip) != CM_CL_NARE_OK)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev(%u): got wrong ip-suite, ignore hello.ind"
					, argr->device_nr
					);
				return;
			}

			/* check fsu+at: stationname changed in meantime ? */
			if (dev->run_mode & CM_CL_RMODE__I_HELLO_FILTER_DYNAMIC)
			{
				LSA_BOOL equal;

				if (dev->dev_addr.tailored_device_name_length != hello->StationNameLen)
				{
					equal = LSA_FALSE;
				}
				else if (CM_MEMCMP (dev->dev_addr.tailored_device_name, hello->pStationName, hello->StationNameLen) != 0)
				{
					equal = LSA_FALSE;
				}
				else
				{
					equal = LSA_TRUE;
				}

				if (! equal)
				{
					CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
						, "dev(%u): got wrong tailored_device_name, ignore hello.ind"
						, argr->device_nr
						);
					return;
				}
			}
		}

		cm_cl_ar_set_oha_ip (argr
			, &hello->MacAddr
			, &hello->IPAddr, &hello->SubnetMask, &hello->DefaultRouterAddr
			);

		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u)  STARTUP /fast  ip(%u.%u.%u.%u)"
			, argr->device_nr
			, (LSA_UINT8)(CM_NTOH32 (argr->ar.host_ip) >> 24)
			, (LSA_UINT8)(CM_NTOH32 (argr->ar.host_ip) >> 16)
			, (LSA_UINT8)(CM_NTOH32 (argr->ar.host_ip) >> 8)
			, (LSA_UINT8)(CM_NTOH32 (argr->ar.host_ip) >> 0)
			);

		cm_cl_ar_sustat_sm_trigger (argr, CM_CL_SUSTAT_STATE_NARE_FSU);

		if (argr->cm.cl.state == CM_DST_WF_SCHEDULE)
		{
			cm_cl_schedule_fsu (argr);
		}
		else
		{
			/* hello.ind contains the same data as nare_ident.cnf does */
			cm_cl_bindsm_startup_optimized (argr, LSA_TRUE);
		}
		break;

	default:
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u): hello.ind too late, ignore it in ar_state(%u), wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
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
