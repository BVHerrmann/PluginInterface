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
/*  F i l e               &F: cm_nare.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  NARE (name and address resolution) things                                */
/*                                                                           */
/*  - alloc NARE-RQB                                                         */
/*  - free NARE-RQB                                                          */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   30
#define CM_MODULE_ID        30

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION (CM_MODULE_ID)	/* no semicolon */


/*---------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS
static LSA_VOID
cm_nare_request_lower (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	);
#endif

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE
cm_nare_channel_get (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	channel = channel->lower.channel[CM_PATH_TYPE_NARE];

	if (cm_is_null (channel))
	{
		CM_FATAL ();/* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower NARE-RQB that fits for all requests
//---------------------------------------------------------------------------*/
CM_NARE_LOWER_RQB_PTR_TYPE
cm_nare_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_CHANNEL_PTR_TYPE nare_channel = cm_nare_channel_get (channel);

	CM_NARE_LOWER_RQB_PTR_TYPE lower_rqb_ptr;

	/* alloc the RQB */
	CM_NARE_ALLOC_LOWER_RQB (&lower_rqb_ptr, cm_null_user_id, sizeof (CM_NARE_LOWER_RQB_TYPE), nare_channel->sysptr);
	if (cm_is_null (lower_rqb_ptr))
	{
		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc NARE RQB, len(%u)"
			, channel->my_handle, sizeof (CM_NARE_LOWER_RQB_TYPE)
			);
		return LSA_NULL;
	}

#if CM_DEBUG
	CM_NARE_SET_NEXT_RQB_PTR (lower_rqb_ptr, LSA_NULL);/* sanity */
	CM_NARE_SET_PREV_RQB_PTR (lower_rqb_ptr, LSA_NULL);/* sanity */
#endif

	return lower_rqb_ptr;
}


/*-----------------------------------------------------------------------------
//	free the RQB
//---------------------------------------------------------------------------*/
LSA_VOID
cm_nare_free_rqb (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE lower_rqb_ptr
	)
{
	LSA_UINT16 rc;

	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (lower_rqb_ptr));

	CM_NARE_FREE_LOWER_RQB (
		&rc,
		lower_rqb_ptr,
		cm_nare_channel_get (channel)->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG (rc);
}


/*-----------------------------------------------------------------------------
//	free a NARE_OPC_IDENT-RQB and the attached alias-array
//---------------------------------------------------------------------------*/
LSA_VOID
cm_nare_free_rqb_alias (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE lower_rqb_ptr
	)
{
	CM_ASSERT (cm_is_not_null (channel));
	CM_ASSERT (cm_is_not_null (lower_rqb_ptr));
	CM_ASSERT (CM_NARE_GET_OPCODE (lower_rqb_ptr) == NARE_OPC_IDENT);

	if (cm_is_not_null (lower_rqb_ptr->args.Ident.pStationNameAlias))
	{
		LSA_UINT16 rc;
		CM_NARE_FREE_LOWER_MEM (&rc, lower_rqb_ptr->args.Ident.pStationNameAlias, cm_nare_channel_get (channel)->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);

		lower_rqb_ptr->args.Ident.pStationNameAlias = LSA_NULL;
	}

	cm_nare_free_rqb (channel, lower_rqb_ptr);
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_nare_channel_open (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_NARE_LOWER_RQB_PTR_TYPE nare;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_NARE);

	CM_ASSERT (cm_nare_channel_get (channel) == channel);

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_NARE_INVALID_HANDLE);

	/***/

	nare = cm_nare_alloc_rqb (channel);

	if (cm_is_null (nare))
	{
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	nare->args.channel.handle                      = CM_NARE_INVALID_HANDLE;
	nare->args.channel.handle_upper                = channel->my_handle;
	nare->args.channel.sys_path                    = channel->usr.ch.curr->args.channel.open->sys_path;
	nare->args.channel.nare_request_upper_done_ptr = cm_nare_request_lower_done;

	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, 0, 0);

	CM_NARE_SET_HANDLE (nare, CM_NARE_INVALID_HANDLE);
	CM_NARE_SET_OPCODE (nare, NARE_OPC_OPEN_CHANNEL);

	CM_NARE_OPEN_CHANNEL_LOWER (nare, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_nare_channel_open_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	if (CM_NARE_GET_RESPONSE (nare) == NARE_RSP_OK)
	{
		channel->lower.handle = nare->args.channel.handle;

		CM_ASSERT (channel->lower.handle != CM_NARE_INVALID_HANDLE);

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;

#if CM_CFG_MAX_CLIENTS
		/* provide NARE_OPC_RESOLVE_CONFIG_NOTIFY for "accelerated mode" */
		{
			LSA_UINT16 max = channel->detailptr->u.nare.resolve_notify_resource_count;
			LSA_UINT16 cnt;

			for (cnt = 0; cnt < max; ++cnt)
			{
				CM_NARE_LOWER_RQB_PTR_TYPE n;

				n = cm_nare_alloc_rqb (channel);

				if (cm_is_null (n))
				{
					CM_FATAL ();/* must not ignore this! */
				}

				cm_nare_request_lower (channel, NARE_OPC_IDENT_NOTIFY, 0, 0, n);
			}
		}
#endif
	}
	else
	{
		CM_ASSERT (channel->lower.handle == CM_NARE_INVALID_HANDLE);

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) nare-rsp(%u)"
			, channel->my_handle, CM_NARE_GET_RESPONSE (nare)
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_nare_free_rqb (channel, nare);
	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_nare_channel_close (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_NARE_LOWER_RQB_PTR_TYPE nare;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_NARE);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	nare = cm_nare_alloc_rqb (channel);
	if (cm_is_null (nare))
	{
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, 0, 0);

	CM_NARE_SET_HANDLE (nare, channel->lower.handle);
	CM_NARE_SET_OPCODE (nare, NARE_OPC_CLOSE_CHANNEL);

	CM_NARE_CLOSE_CHANNEL_LOWER (nare, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_nare_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_NARE_GET_RESPONSE (nare);

	cm_nare_free_rqb (channel, nare);

	if (rc == NARE_RSP_OK)
	{
		channel->lower.handle = CM_NARE_INVALID_HANDLE;

		CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else
	{
		/* don't invalidate nare-handle, channel is not closed! */

		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) nare-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS
static LSA_VOID
cm_nare_ident_notify_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	CM_ASSERT (channel->path_type == CM_PATH_TYPE_NARE);

	if (channel->state >= CM_CH_CLOSE)
	{
		CM_NARE_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "channel is closing, freeing rqb"
			);
		CM_ASSERT (
			CM_NARE_GET_RESPONSE (nare) == NARE_RSP_OK
			|| CM_NARE_GET_RESPONSE (nare) == NARE_RSP_OK_CANCEL
			);
		cm_nare_free_rqb (channel, nare);
	}
	else if (CM_NARE_GET_RESPONSE (nare) != NARE_RSP_OK)
	{
		CM_FATAL1 (CM_NARE_GET_RESPONSE (nare));/* what? why? */
	}
	else
	{
		CM_CHANNEL_PTR_TYPE cl_channel          = channel->lower.channel[CM_PATH_TYPE_CLIENT];
		NARE_UPPER_IDENT_NOTIFY_PTR_TYPE notify = &nare->args.IdentNotify;

		if (cm_is_not_null (cl_channel))
		{
			cm_cl_nare_notify_ind (cl_channel, notify);
		}
		else
		{
			/* late, ignore */
		}

		cm_nare_request_lower (channel, NARE_OPC_IDENT_NOTIFY, 0, 0, nare);
	}
}
#endif


/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_VOID
cm_nare_register_client (
	CM_CHANNEL_PTR_TYPE nare_channel,
	CM_CHANNEL_PTR_TYPE cl_channel	/* LSA_NULL..unregister */
	)
{
	CM_ASSERT (nare_channel->path_type == CM_PATH_TYPE_NARE);

	if (cm_is_not_null (cl_channel))
	{
		CM_ASSERT (cm_is_null (nare_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		nare_channel->lower.channel[CM_PATH_TYPE_CLIENT] = cl_channel;	/* register */
	}
	else
	{
		CM_ASSERT (cm_is_not_null (nare_channel->lower.channel[CM_PATH_TYPE_CLIENT]));
		nare_channel->lower.channel[CM_PATH_TYPE_CLIENT] = LSA_NULL;/* unregister */
	}
}

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_BOOL
cm_nare_ident (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 name_length,
	CM_COMMON_MEM_U8_PTR_TYPE name_ptr,
	LSA_UINT16 ident_timeout,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_UINT32 notify_id,
	LSA_VOID_PTR_TYPE cancel_request_id,
	CM_LIST_ENTRY_CONST_PTR_TYPE alias_list,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	NARE_UPPER_ALIAS_PTR_TYPE alias_array;
	LSA_UINT32 alias_cnt;
	LSA_UINT32 size;

	if (cm_is_null (alias_list))
	{
		alias_cnt = 0;
		size      = 0;
	}
	else
	{
		CmListLength (alias_list, &alias_cnt);
		size = sizeof(*alias_array) * alias_cnt;
	}

	if (size > 0xFFFF)
	{
		CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ident: wrong alias_cnt(%u) or size(%u) > 0xFFFF"
			, alias_cnt, size
			);
		return LSA_FALSE;	/* not allowed from nare */
	}

	if (cm_is_null (nare))
	{
		nare = cm_nare_alloc_rqb (channel);
		if (cm_is_null (nare))
		{
			return LSA_FALSE;	/* cannot alloc */
		}
	}

	CM_NARE_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ident: alias_cnt(%u) notify_id(0x%x)"
		, alias_cnt, notify_id
		);

	/***/

	nare->args.Ident.pStationName   = name_ptr;	/* use quasi-static ar_graph-pointer, lifetime of ar */
	nare->args.Ident.StationNameLen = name_length;
	nare->args.Ident.Tolerance      = ident_timeout;
	nare->args.Ident.RequestID      = (LSA_UINT32)cancel_request_id;

	nare->args.Ident.Notify   = (notify_id != 0) ? NARE_NOTIFY_DCP_RESPONSE_YES : NARE_NOTIFY_DCP_RESPONSE_NO;
	nare->args.Ident.NotifyID = notify_id;

	if (alias_cnt == 0 || cm_is_null (alias_list))
	{
		nare->args.Ident.pStationNameAlias = LSA_NULL;	/* see cm_nare_free_rqb_alias() */
		nare->args.Ident.AliasCnt          = 0;	/* see cm_nare_free_rqb_alias() */
	}
	else
	{
#if CM_CFG_MAX_CLIENTS
		CM_NARE_LOWER_MEM_PTR_TYPE lower_mem;
		CM_CL_NARE_ALIAS_PTR_TYPE alias;

		CM_NARE_ALLOC_LOWER_MEM (&lower_mem
			, cm_null_user_id, (LSA_UINT16)size
			, cm_nare_channel_get (channel)->sysptr
			);
		if (cm_is_null (lower_mem))
		{
			cm_nare_free_rqb (channel, nare);
			return LSA_FALSE;	/* cannot alloc */
		}

		alias_array = (NARE_UPPER_ALIAS_PTR_TYPE)lower_mem;
		nare->args.Ident.AliasCnt          = 0;
		nare->args.Ident.pStationNameAlias = alias_array;

		for (CmListForeach (alias, alias_list, CM_CL_NARE_ALIAS_PTR_TYPE))
		{
			CM_ASSERT (alias->name_length <= 255);
			if (alias->name_length > 0)
			{
				alias_array->NameLen       = (LSA_UINT8)alias->name_length;
				alias_array->pName         = alias->name;	/* use quasi-static ar_graph-pointer */
				alias_array               += 1;	/* next array */
				nare->args.Ident.AliasCnt += 1;
			}
		}
#else
		CM_FATAL ();
#endif
	}

	/***/

	nare->args.Ident.AliasUsed = 0;

	nare->args.Ident.DeviceID   = 0;
	nare->args.Ident.VendorID   = 0;
	nare->args.Ident.InstanceID = 0;
	nare->args.Ident.InstanceIDValid = 0;

	/* nare->args.Ident.IdentifiedStationName; ignore */
	nare->args.Ident.IdentifiedStationNameLen = 0;

	CM_MEMSET (nare->args.Ident.IPAddr, 0, NARE_IP_ADDR_SIZE);
	CM_MEMSET (nare->args.Ident.SubMask, 0, NARE_IP_ADDR_SIZE);
	CM_MEMSET (nare->args.Ident.DefRouter, 0, NARE_IP_ADDR_SIZE);
	CM_MEMSET (nare->args.Ident.MACAddr, 0, NARE_MAC_ADDR_SIZE);

	/***/

	cm_nare_request_lower (channel, NARE_OPC_IDENT, device_nr, ar_nr, nare);	/* cm_nare_ident() */

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_nare_cancel (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE cancel_request_id,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
)
{
	CM_NARE_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "nare_cancel: dev(%u) ar(%u) cancel_request_id(%08x)", device_nr, ar_nr, cancel_request_id
	);

	if (cm_is_null (nare))
	{
		nare = cm_nare_alloc_rqb (channel);
		if (cm_is_null (nare))
		{
			CM_NARE_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
				"nare_cancel: cannot alloc nare-rqb"
			);
			return LSA_FALSE;	/* cannot alloc */
		}
	}

	nare->args.Cancel.RequestID = (LSA_UINT32)cancel_request_id;

	cm_nare_request_lower (channel, NARE_OPC_CANCEL, device_nr, ar_nr, nare);	/* cm_nare_cancel() */

	return LSA_TRUE;
}
#endif


#if CM_CFG_MAX_CLIENTS
/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_nare_test_ip (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 local_ip_addr,
	LSA_UINT32 ip_addr,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE cancel_request_id
	)
{
	CM_NARE_LOWER_RQB_PTR_TYPE nare;

	nare = cm_nare_alloc_rqb (channel);

	if (cm_is_null (nare))
	{
		return LSA_FALSE;	/* cannot alloc */
	}

	/***/

	CM_ASSERT (NARE_IP_ADDR_SIZE == sizeof(local_ip_addr));
	CM_MEMCPY (nare->args.TestIP.LocalIPAddr, &local_ip_addr, NARE_IP_ADDR_SIZE);

	CM_ASSERT (NARE_IP_ADDR_SIZE == sizeof(ip_addr));
	CM_MEMCPY (nare->args.TestIP.IPAddr, &ip_addr, NARE_IP_ADDR_SIZE);

	nare->args.TestIP.RequestID = (LSA_UINT32)cancel_request_id;
	nare->args.TestIP.UseDelay  = NARE_USE_DELAY_NO;
	nare->args.TestIP.SniffMode = NARE_SNIFF_MODE_MULTIPLE;	/* sniff for multiple IP usage */

	/***/

	CM_MEMSET (nare->args.TestIP.MACAddr, 0, NARE_MAC_ADDR_SIZE);

	/***/

	cm_nare_request_lower (channel, NARE_OPC_TEST_IP, device_nr, ar_nr, nare);	/* cm_nare_test_ip() */

	return LSA_TRUE;
}

/*===========================================================================*/

LSA_BOOL
cm_nare_set (
	/* called by CM-CL-AR */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 mode,
	LSA_UINT8 name_length,
	CM_COMMON_MEM_U8_PTR_TYPE name_ptr,
	LSA_UINT32 ip_addr,
	LSA_UINT32 subnet_mask,
	LSA_UINT32 def_router,
	CM_MAC_TYPE mac_addr,
	LSA_BOOL set_ip_remanent,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE cancel_request_id
	)
{
	CM_NARE_LOWER_RQB_PTR_TYPE nare;

	nare = cm_nare_alloc_rqb (channel);
	if (cm_is_null (nare))
	{
		return LSA_FALSE;	/* cannot alloc */
	}

	nare->args.Set.pStationName = name_ptr;	/* use quasi-static ar_graph-pointer, lifetime of ar */
	nare->args.Set.StationNameLen = name_length;
	nare->args.Set.Mode           = mode;
	nare->args.Set.SetIPRemanent  = set_ip_remanent ? NARE_SET_IP_REMANENT_YES : NARE_SET_IP_REMANENT_NO;
	nare->args.Set.RequestID = (LSA_UINT32)cancel_request_id;

	CM_MEMCPY (nare->args.Set.IPAddr, &ip_addr, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (nare->args.Set.SubMask, &subnet_mask, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (nare->args.Set.DefRouter, &def_router, NARE_IP_ADDR_SIZE);
	CM_MEMCPY (nare->args.Set.MACAddr, &mac_addr, NARE_MAC_ADDR_SIZE);

	cm_nare_request_lower (channel, NARE_OPC_SET, device_nr, ar_nr, nare);	/* cm_nare_set() */

	return LSA_TRUE;
}
#endif


/*===========================================================================*/
/*=== NARE REQUEST LOWER ====================================================*/
/*===========================================================================*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS
static LSA_VOID
cm_nare_request_lower (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	CM_NARE_SET_USER_ID_HANDLE_DEV_AR (nare, channel->my_handle, device_nr, ar_nr);

	CM_NARE_SET_HANDLE (nare, cm_nare_channel_get (channel)->lower.handle);
	CM_NARE_SET_OPCODE (nare, opcode);

	CM_NARE_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> nare-rqb(0x%08x) nare-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, nare, CM_NARE_GET_OPCODE (nare)
		, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
		);

	CM_NARE_REQUEST_LOWER (nare, cm_nare_channel_get (channel)->sysptr);
}
#endif

/*===========================================================================*/
/*=== NARE LOWER DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_nare_request_lower_done (
	CM_NARE_LOWER_RQB_PTR_TYPE nare
	)
{
	CM_CHANNEL_PTR_TYPE channel;

	if (cm_is_null (nare))
	{

		CM_NARE_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"cm_is_null(nare)"
			);

		CM_FATAL ();
		return;
	}

	channel = cm_channel_from_handle (CM_NARE_GET_USER_ID_HANDLE (nare));

	if (cm_is_null (channel))
	{

		CM_NARE_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_NARE_GET_USER_ID_HANDLE (nare)
			);

		CM_FATAL ();
		return;
	}

	switch (CM_NARE_GET_RESPONSE (nare))
	{
	case NARE_RSP_OK:
	case NARE_RSP_OK_CANCEL:
	case NARE_RSP_ERR_DCP_STATION_NOT_FOUND:
		CM_NARE_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< nare-rqb(0x%08x) nare-opc(%u) nare-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, nare, CM_NARE_GET_OPCODE (nare), CM_NARE_GET_RESPONSE (nare)
			, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
			);
		break;

	default:
		CM_NARE_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "<<< nare-rqb(0x%08x) nare-opc(%u) nare-rsp(%u) t-h(%u) t-dev(%u) t-ar(%u)"
			, nare, CM_NARE_GET_OPCODE (nare), CM_NARE_GET_RESPONSE (nare)
			, CM_NARE_GET_USER_ID_HANDLE (nare), CM_NARE_GET_USER_ID_DEV (nare), CM_NARE_GET_USER_ID_AR (nare)
			);
		break;
	}

	switch (channel->path_type)
	{

	case CM_PATH_TYPE_NARE:

		switch (CM_NARE_GET_OPCODE (nare))
		{

		case NARE_OPC_OPEN_CHANNEL:
			cm_nare_channel_open_done (channel, nare);
			break;

		case NARE_OPC_CLOSE_CHANNEL:
			cm_nare_channel_close_done (channel, nare);
			break;

#if CM_CFG_MAX_CLIENTS
		case NARE_OPC_IDENT_NOTIFY:
			cm_nare_ident_notify_done (channel, nare);
			break;
#endif

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:

		switch (CM_NARE_GET_OPCODE (nare))
		{

		case NARE_OPC_IDENT:
			cm_cl_nare_ident_done (channel, nare);
			break;

		case NARE_OPC_CANCEL:
			cm_cl_nare_cancel_done (channel, nare);
			break;

		case NARE_OPC_TEST_IP:
			cm_cl_nare_test_ip_done (channel, nare);
			break;

		case NARE_OPC_SET:
			cm_cl_nare_set_done (channel, nare);
			break;

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:

		switch (CM_NARE_GET_OPCODE (nare))
		{

		case NARE_OPC_IDENT:
			CM_ASSERT (nare->args.Ident.AliasCnt == 0);
			cm_mc_mcons_ident_done (channel, CM_NARE_GET_USER_ID_DEV (nare), nare);
			break;

		case NARE_OPC_CANCEL:
			cm_mc_mcons_ident_cancel_done (channel, CM_NARE_GET_USER_ID_DEV (nare), nare);
			break;

		default:
			CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)", CM_NARE_GET_OPCODE (nare)
				);
			CM_FATAL1 (CM_NARE_GET_OPCODE (nare));
			break;
		}
		break;
#endif

	default:
		CM_NARE_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "unknown path_type(%u)", channel->path_type
			);
		CM_FATAL1 (channel->path_type);
		break;
	}
}


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
