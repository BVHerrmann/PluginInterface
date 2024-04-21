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
/*  F i l e               &F: cm_usr.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the LSA user-interface                                        */
/*                                                                           */
/*  - helper for callback                                                    */
/*  - channel open/close done callback                                       */
/*  - open channel                                                           */
/*  - close channel                                                          */
/*  - request                                                                */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	2
#define CM_MODULE_ID		2

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#ifdef CM_MESSAGE
#pragma CM_MESSAGE("DEFERRED: pragmas with VVs are now under comment. search for VV: to find them.")
/* remove this pragma when the last VV has been realized */
#endif /* CM_MESSAGE */

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("DEFERRED: pragmas with FATAL@xxx converted into comments. search for FATAL@ to find them.")
/* remove this pragma when the last fatal is avoided */
#endif /* CM_MESSAGE */

/*---------------------------------------------------------------------------*/

CM_CHANNEL_TYPE cm_channels[CM_MAX_CHANNELS]; /* the channel table */

/*---------------------------------------------------------------------------*/

LSA_VOID
cm_channels_sys_init(
	LSA_VOID
) {
	LSA_UINT16 i;

		/* init the channel table */
	for (i = 0; i < CM_MAX_CHANNELS; ++i) {
		cm_channels[i].state = CM_CH_FREE;
	}
}


/*---------------------------------------------------------------------------*/

LSA_VOID
cm_channels_sys_undo_init(
	LSA_VOID
) {
	LSA_UINT16 i;

	for (i = 0; i < CM_MAX_CHANNELS; ++i) {
		CM_ASSERT(cm_channels[i].state == CM_CH_FREE);
	}
}


/*------------------------------------------------------------------------------
//	sets the response of the given request-block and calls the
//        request-done-fct (given in CM_OPC_OPEN_CHANNEL)
//----------------------------------------------------------------------------*/
LSA_VOID
cm_callback(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 response,
	CM_UPPER_RQB_PTR_TYPE *upper_ptr
) {
	CM_UPPER_RQB_PTR_TYPE  rb;

	rb = *upper_ptr;
	*upper_ptr = LSA_NULL;

	CM_RQB_SET_HANDLE (rb, channel->handle_upper);
	CM_RQB_SET_RESPONSE (rb, response);

	CM_UPPER_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) f-h(%u)"
		, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb), channel->my_handle
		);

	CM_REQUEST_UPPER_DONE (channel->callback, rb, channel->sysptr);
}


/*------------------------------------------------------------------------------
//	func: cm_callback_and_free_channel
//----------------------------------------------------------------------------*/

static LSA_VOID cm_callback_and_free_channel(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 result,
	CM_UPPER_RQB_PTR_TYPE rb
) {

	cm_res_undo_init(channel);

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: make cleanup of lower table")*/
#endif /* CM_MESSAGE */

#if 0 /* channels are not all zero here! */
	{
	LSA_UINT16 t;

	for( t = 0; t < CM_PATH_TYPE_ANY_MAX; ++t ) {

		CM_ASSERT(channel->lower.channel[t] == LSA_NULL);
	}
	}
#endif

	/***/

	CM_RQB_SET_HANDLE (rb, channel->handle_upper);
	CM_RQB_SET_RESPONSE (rb, result);

	CM_UPPER_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "<<< cm-rqb(0x%08x) h(%u) cm-opc(%u) cm-rsp(%u) channel-free"
		, rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb), CM_RQB_GET_RESPONSE (rb)
		);

	/*
	 * NOTE: the following sequence is wrong!
	 *       > cm_callback(channel, CM_OK, &rb);
	 *       > channel->state = CM_CH_FREE;
	 *
	 * ==> even if the callback posts into a queue, a call
	 *     to cm_undo_init() can occur (due to preemption)
	 *     before we have the chance to reset the in-use flag
	 */

	{
	CM_UPPER_CALLBACK_FCT_PTR_TYPE  callback = channel->callback;
	CM_DETAIL_PTR_TYPE  detailptr = channel->detailptr;
	LSA_SYS_PTR_TYPE  sysptr = channel->sysptr;
	LSA_UINT16  rc;

#if CM_DEBUG
	CM_MEMSET(channel, 0xDD, sizeof(*channel)); /* wipe out */
#endif

	channel->callback = 0;
	channel->detailptr = LSA_NULL;
	channel->state = CM_CH_FREE; /* free the channel ... */

	CM_REQUEST_UPPER_DONE (callback, rb, sysptr); /* ... and then call back! */

	CM_RELEASE_PATH_INFO (& rc, sysptr, detailptr);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}


/*------------------------------------------------------------------------------
//	allocate a channel
//----------------------------------------------------------------------------*/

static CM_CHANNEL_PTR_TYPE
cm_channel_alloc(
	CM_UPPER_OPEN_CHANNEL_CONST_PTR_TYPE open,
	LSA_SYS_PTR_TYPE sysptr,
	CM_DETAIL_PTR_TYPE detailptr
) {
	LSA_HANDLE_TYPE handle;

	CM_ASSERT(CM_MAX_CHANNELS < CM_INVALID_HANDLE); /* not #if because of type-cast! */

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE channel = &cm_channels[handle];

		if( channel->state == CM_CH_FREE ) {

#if CM_DEBUG
			CM_MEMSET(channel, 0xCC, sizeof(*channel)); /* to find uninitialized things */
#endif
			channel->state			= CM_CH_ALLOCATED;
			channel->my_handle		= handle;
			channel->handle_upper	= open->handle_upper;
			channel->callback		= open->cm_request_upper_done_ptr;
			channel->sysptr			= sysptr;
			channel->detailptr		= detailptr;
			channel->path_type		= detailptr->path_type; /* TIA 1678036 */
			channel->trace_idx		= 0; /* sanity, will be set later */

			/* the rest is set up in cm_open_channel() */

			return channel;
		}
	}

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	get channel from handle or LSA_NULL if out-of-range
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_channel_from_handle(
	LSA_HANDLE_TYPE handle
) {

	if( handle < CM_MAX_CHANNELS ) {

		CM_CHANNEL_PTR_TYPE channel = &cm_channels[handle]; /* see cm_channel_alloc() */

		if( channel->state >= CM_CH_OPEN ) {

			return channel; /* note: valid detailptr and callback-function (AP01361477) */
		}
	}

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
//	find a channel for this nic with path_type
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_channel_find_path_type(
	LSA_UINT8  nic_id,
	LSA_UINT8  path_type
) {
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			cm_is_not_null(channel)
			&& channel->state >= CM_CH_OPEN /* always true */
			&& cm_is_not_null(channel->detailptr) /* always true */
		) { /* is open */

			if( channel->detailptr->nic_id == nic_id ) { /* same nic */

				if( channel->path_type == path_type ) {

					return channel; /* found */
				}
			}
		}
	}

	return LSA_NULL; /* not found */
}

/*------------------------------------------------------------------------------
//	check if all system channels are opened
//----------------------------------------------------------------------------*/

static LSA_BOOL
cm_channel_sys_opened(
	CM_CHANNEL_CONST_PTR_TYPE usr_channel
) {
	LSA_UINT32 sys = 0;
	LSA_HANDLE_TYPE handle;

	CM_ASSERT(CM_PATH_TYPE_ANY_MAX < 32); /* see shift-operator */

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			cm_is_not_null(channel)
			&& channel->state >= CM_CH_READY
			&& channel->state < CM_CH_CLOSE
			&& cm_is_not_null(channel->detailptr) /* always true */
		) { /* is open */

			if( channel->detailptr->nic_id == usr_channel->detailptr->nic_id) { /* same nic */

				switch( channel->path_type ) {

				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
				case CM_PATH_TYPE_OHA:

				case CM_PATH_TYPE_NARE:

				case CM_PATH_TYPE_RPC:
					sys |= (1UL << channel->path_type);
					break;

				default:
					break;
				}
			}
		}
	}

	/***/

	{
	LSA_UINT32 bits = 0;

	bits |= (1UL << CM_PATH_TYPE_EDD);
#if CM_CFG_USE_MRP
	bits |= (1UL << CM_PATH_TYPE_MRP);
#endif
#if CM_CFG_USE_GSY
	bits |= (1UL << CM_PATH_TYPE_GSY);
#endif
#if CM_CFG_USE_POF
	bits |= (1UL << CM_PATH_TYPE_POF);
#endif
	bits |= (1UL << CM_PATH_TYPE_OHA);

	switch( usr_channel->path_type ) {
	case CM_PATH_TYPE_PD:
		bits |= (1UL << CM_PATH_TYPE_RPC); /* cm-pd starts the endpoint-mapper */
		break;

	case CM_PATH_TYPE_CLIENT:
		bits |= (1UL << CM_PATH_TYPE_NARE);
		bits |= (1UL << CM_PATH_TYPE_RPC);
		break;

	case CM_PATH_TYPE_MULTICAST:
		bits |= (1UL << CM_PATH_TYPE_NARE);
		break;

	case CM_PATH_TYPE_SERVER:
		bits |= (1UL << CM_PATH_TYPE_NARE);
		bits |= (1UL << CM_PATH_TYPE_RPC);
		break;

	default:
		CM_FATAL();
		break;
	}

	if( (sys & bits) != bits ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "not all system channels open, open(0x%x) needed(0x%x)"
			, sys, bits
			);
		return LSA_FALSE;
	}
	}

	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
//	assign the system channels
//----------------------------------------------------------------------------*/

static LSA_VOID
cm_channel_sys_assign(
	CM_CHANNEL_PTR_TYPE channel
) {
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	sys_channel = cm_channel_from_handle(handle);

		if(
			cm_is_not_null(sys_channel)
			&& sys_channel->state >= CM_CH_READY
			&& sys_channel->state < CM_CH_CLOSE
			&& cm_is_not_null(sys_channel->detailptr) /* always true */
		) { /* is open */

			if( sys_channel->detailptr->nic_id == channel->detailptr->nic_id ) { /* same nic */

				switch( sys_channel->path_type ) {

				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
				case CM_PATH_TYPE_OHA:

				case CM_PATH_TYPE_NARE:

				case CM_PATH_TYPE_RPC:
					channel->lower.channel[sys_channel->path_type] = sys_channel;
					break;

				default:
					break;
				}
			}
		}
	}
}


/*------------------------------------------------------------------------------
//	connect the CL/MC/SV channel with the PD channel
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_ACP

static LSA_VOID
cm_peer_connect(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_CHANNEL_PTR_TYPE pd = cm_channel_find_path_type(channel->detailptr->nic_id, CM_PATH_TYPE_PD);

	if( cm_is_null(pd) ) {
		CM_FATAL(); /* a bug */
	}

	CM_ASSERT(cm_is_null(channel->lower.channel[channel->path_type]));
	CM_ASSERT(cm_is_null(pd->lower.channel[channel->path_type]));

	channel->lower.channel[channel->path_type] = pd;

	pd->lower.channel[channel->path_type] = channel;

	CM_UPPER_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "usr(%u) path_type(%u) connected to pd(%u) path_type(%u)"
		, channel->my_handle, channel->path_type, pd->my_handle, pd->path_type
		);
}

#endif

/*------------------------------------------------------------------------------
//	disconnect the CL/MC/SV channel from the PD channel
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_ACP

static LSA_VOID
cm_peer_disconnect(
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_CHANNEL_PTR_TYPE pd;

	pd = channel->lower.channel[channel->path_type];

	if( cm_is_null(pd) ) {
		CM_FATAL(); /* a bug */
	}

	pd->lower.channel[channel->path_type] = LSA_NULL; /* disconnect */

	channel->lower.channel[channel->path_type] = LSA_NULL; /* sanity */

	CM_UPPER_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "usr(%u) path_type(%u) disconnected from pd(%u) path_type(%u)"
		, channel->my_handle, channel->path_type, pd->my_handle, pd->path_type
		);
}

#endif

/*------------------------------------------------------------------------------
//	get peer of the CL/MC/SV or PD channel
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_peer_channel_get(
	CM_CHANNEL_PTR_TYPE channel, /* PD, CL, MC or SV */
	LSA_UINT8 peer_path_type
		/* PD: CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_MULTICAST or CM_PATH_TYPE_SERVER
		 * CL: CM_PATH_TYPE_PD, CM_PATH_TYPE_CLIENT
		 * MC: CM_PATH_TYPE_PD
		 * SV: CM_PATH_TYPE_PD
		 */
) {
	CM_CHANNEL_PTR_TYPE pd;

	switch( peer_path_type ) {

	case CM_PATH_TYPE_PD:
		if (channel->path_type == CM_PATH_TYPE_PD) {

			pd = channel;
		}
		else {

			CM_ASSERT(
				channel->path_type == CM_PATH_TYPE_CLIENT
				|| channel->path_type == CM_PATH_TYPE_MULTICAST
				|| channel->path_type == CM_PATH_TYPE_SERVER
			);

			pd = channel->lower.channel[channel->path_type];

			if( cm_is_null(pd) ) {
				CM_FATAL(); /* a bug */
			}
		}
		break;

	case CM_PATH_TYPE_CLIENT:
		if (channel->path_type == CM_PATH_TYPE_CLIENT)
		{
			pd = channel;
		}
		else
		{
			CM_ASSERT(channel->path_type == CM_PATH_TYPE_PD);
			pd = channel->lower.channel[peer_path_type]; /* may be null */
		}
		break;

	case CM_PATH_TYPE_MULTICAST:
	case CM_PATH_TYPE_SERVER:
		CM_ASSERT(channel->path_type == CM_PATH_TYPE_PD);
		pd = channel->lower.channel[peer_path_type]; /* may be null */
		break;

	default:
		CM_FATAL(); /* a bug */
		pd = LSA_NULL;
		break;
	}

	return pd;
}


/*------------------------------------------------------------------------------
//	check if all user channels are closed
//----------------------------------------------------------------------------*/

static LSA_BOOL
cm_channel_usr_closed(
	CM_CHANNEL_CONST_PTR_TYPE sys_channel
) {
	LSA_UINT16 cnt = 0;
	LSA_HANDLE_TYPE handle;

	for( handle = 0; handle < CM_MAX_CHANNELS; ++handle ) {

		CM_CHANNEL_PTR_TYPE	channel = cm_channel_from_handle(handle);

		if(
			cm_is_not_null(channel)
			&& channel->state != CM_CH_FREE /* i.e., detailptr valid */
			&& cm_is_not_null(channel->detailptr) /* always true */
		) { /* is used */

			if( channel->detailptr->nic_id == sys_channel->detailptr->nic_id ) { /* same nic */

				switch(sys_channel->path_type ) {
				case CM_PATH_TYPE_RPC:
				case CM_PATH_TYPE_NARE:

					switch( channel->path_type ) {
					case CM_PATH_TYPE_CLIENT:
					case CM_PATH_TYPE_MULTICAST:
					case CM_PATH_TYPE_SERVER:
						cnt += 1;
						break;
					default:
						break;
					}
					break;

				case CM_PATH_TYPE_OHA:
				case CM_PATH_TYPE_EDD:
#if CM_CFG_USE_MRP
				case CM_PATH_TYPE_MRP:
#endif
#if CM_CFG_USE_GSY
				case CM_PATH_TYPE_GSY:
#endif
#if CM_CFG_USE_POF
				case CM_PATH_TYPE_POF:
#endif
					switch( channel->path_type ) {
					case CM_PATH_TYPE_PD:
					case CM_PATH_TYPE_CLIENT:
					case CM_PATH_TYPE_MULTICAST:
					case CM_PATH_TYPE_SERVER:
						cnt += 1;
						break;
					default:
						break;
					}
					break;

				default:
					break;
				}
			}
		}
	}

	if( cnt != 0 ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE
			, "not all user channels closed, %u still open, sys_path_type(%u)"
			, cnt, sys_channel->path_type
			);
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*------------------------------------------------------------------------------
//	callback for client & server to notify completion of open/close channel
//----------------------------------------------------------------------------*/
LSA_VOID
cm_channel_done (
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT32			response
) {
	if (response != CM_OK) {
		CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) state(%u) rsp(%u)"
			, channel->my_handle, channel->state, response
			);
	}
	else {
		CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "h(%u) state(%u) rsp(%u)"
			, channel->my_handle, channel->state, response
			);
	}

	switch (channel->state) {

/*------------------------------------------------------------------------------
// OPEN
//----------------------------------------------------------------------------*/
	case CM_CH_OPEN:
		if (response != CM_OK) {
			/* transition CM-CH-OPEN --> CM-CH-NOI */
			cm_callback_and_free_channel (channel, CM_ERR_LOWER_LAYER, channel->usr.ch.curr);
		}
		else {
			/* transition CM-CH-OPEN --> CM-CH-INFO */
			channel->state = CM_CH_INFO;

			switch( channel->path_type ) {

			case CM_PATH_TYPE_EDD:
				cm_edd_channel_info(channel);
				break;

#if CM_CFG_USE_MRP
			case CM_PATH_TYPE_MRP:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;
#endif

#if CM_CFG_USE_GSY
			case CM_PATH_TYPE_GSY:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;
#endif

			case CM_PATH_TYPE_NARE:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;

#if CM_CFG_USE_POF
			case CM_PATH_TYPE_POF:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;
#endif

			case CM_PATH_TYPE_OHA:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;

			case CM_PATH_TYPE_RPC:
				cm_rpc_set_interface_param(channel);
				break;

			case CM_PATH_TYPE_PD:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;

#if CM_CFG_USE_ACP
			case CM_PATH_TYPE_MULTICAST:
				cm_channel_done(channel, CM_OK); /* note: recursion */
				break;

			case CM_PATH_TYPE_CLIENT:
			case CM_PATH_TYPE_SERVER:
				cm_acp_channel_info(channel);
				break;
#endif

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		return;

	case CM_CH_INFO:
		if (response != CM_OK) {
			CM_FATAL(); /* fatal, open succeeded */
		}
		else {
			/* transition CM-CH-INFO --> CM-CH-READY */
			channel->state = CM_CH_READY;
			CM_ASSERT (cm_is_not_null(channel->usr.ch.curr));
			CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.ch.curr) == CM_OPC_OPEN_CHANNEL);
			channel->usr.ch.curr->args.channel.open->handle = channel->my_handle; /* set the result */
			cm_callback (channel, CM_OK, & channel->usr.ch.curr);

			/* after channel->usr.ch.curr is no longer used, we call pd/cl/mc-init */

#if CM_DEBUG
			CM_MEMSET(&channel->usr, 0xCC, sizeof(channel->usr)); /* wipe-out */
#endif

			switch( channel->path_type ) {

			case CM_PATH_TYPE_EDD:
				cm_edd_channel_init(channel);
				break;

#if CM_CFG_USE_MRP
			case CM_PATH_TYPE_MRP:
				cm_mrp_channel_init(channel);
				break;
#endif

#if CM_CFG_USE_GSY
			case CM_PATH_TYPE_GSY:
				cm_gsy_channel_init(channel);
				break;
#endif

			case CM_PATH_TYPE_NARE:
				/* nothing */
				break;

#if CM_CFG_USE_POF
			case CM_PATH_TYPE_POF:
				cm_pof_channel_init(channel);
				break;
#endif

			case CM_PATH_TYPE_OHA:
				cm_oha_channel_init(channel);
				break;

			case CM_PATH_TYPE_RPC:
				/* nothing */
				break;

			case CM_PATH_TYPE_PD:
				/* note: channel->usr.pd.* is initialized in pd-create */
				break;

#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				cm_peer_connect(channel);
				break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
			case CM_PATH_TYPE_MULTICAST:
				cm_peer_connect(channel);
				/* note: channel->usr.mc.* is initialized in mc-create */
				break;
#endif

#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				cm_peer_connect(channel);
				/* note: channel->usr.sv.* is initialized in sv-create */
				break;
#endif

			default:
				CM_FATAL(); /* a bug */
				break;
			}
		}
		return;

/*------------------------------------------------------------------------------
// CLOSE
//----------------------------------------------------------------------------*/

	case CM_CH_CLOSE:
		if (response != CM_OK) {
			CM_FATAL(); /* a bug */
		}
		else {
			/* transition CM-CH-CLOSE --> CM-CH-NOI */
			CM_ASSERT (cm_is_not_null(channel->usr.ch.curr));
			CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.ch.curr) == CM_OPC_CLOSE_CHANNEL);

			cm_callback_and_free_channel (channel, CM_OK, channel->usr.ch.curr);
		}
		return;

/*------------------------------------------------------------------------------
// DEFAULT
//----------------------------------------------------------------------------*/
	default:
		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "h(%u) unknown state(%u)"
			, channel->my_handle, channel->state
			);
		CM_FATAL();
		return;
	}
}


/*------------------------------------------------------------------------------
//	create a CM instance and open a channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID
cm_open_channel (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_OPEN_CHANNEL_PTR_TYPE open;
	CM_CHANNEL_PTR_TYPE channel;

		/* sanity checks */

	if (
		cm_is_null (rb)
		|| CM_RQB_GET_OPCODE (rb) != CM_OPC_OPEN_CHANNEL
		|| cm_is_null (rb->args.void_ptr)
	) {
		CM_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "no rqb(%08X) or wrong opcode or no args"
			, rb
			);
		CM_FATAL ();
		/*lint -unreachable */
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	open = rb->args.channel.open; /* shortcut */

	open->handle = CM_INVALID_HANDLE; /* sanity */

	if (open->cm_request_upper_done_ptr == 0) {/* note: must not use cm_is_null(),
											   compiler warning “function/data pointer conversion in expression” (AP01382171 ) */
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL
			, "no callback function given"
			);
		CM_FATAL ();
		/*lint -unreachable */
		return;
	}

		/* get path info and allocate a cm-handle */

	{
	CM_DETAIL_PTR_TYPE detailptr = LSA_NULL;
	LSA_SYS_PTR_TYPE sysptr;
	LSA_UINT16 rc;

	CM_GET_PATH_INFO (&rc, &sysptr, &detailptr, open->sys_path);

	if (rc != LSA_RET_OK || cm_is_null (detailptr)) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "cm_get_path_info: rc(%u) detailptr(%08x)", rc, detailptr
			);
		CM_FATAL (); /* cannot call back, see CM_REQUEST_UPPER_DONE() */
		/*lint -unreachable */
		return;
	}

	channel = cm_channel_alloc(open, sysptr, detailptr);

	if( cm_is_null(channel) ) {

		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "no free channel found"
			);

		CM_RQB_SET_RESPONSE (rb, CM_ERR_RESOURCE);
		CM_RQB_SET_HANDLE (rb, open->handle_upper);
		CM_REQUEST_UPPER_DONE (open->cm_request_upper_done_ptr, rb, sysptr);

		CM_RELEASE_PATH_INFO (&rc, sysptr, detailptr);
		CM_ASSERT (rc == LSA_RET_OK);
		return;
	}
	}

	CM_UPPER_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "cm_open_channel: h(%u) nic_id(%u) path_type(%u)"
		, channel->my_handle, channel->detailptr->nic_id, channel->path_type
		);

	/***/

	channel->usr.ch.curr = LSA_NULL;

	cm_res_init(channel);

	channel->acp.max_alarm_data_length = 0; /* see below */

	/***/

	channel->lower.handle = 0; /* sanity */

	{
	LSA_UINT16 path_type;

	for( path_type = 0; path_type < CM_PATH_TYPE_ANY_MAX; ++path_type ) {

		channel->lower.channel[path_type] = LSA_NULL;

		if( path_type <= CM_PATH_TYPE_SYS_MAX ) {

			if( path_type == channel->path_type ) { /* make a link to 'self' */

				channel->lower.channel[path_type] = channel; /* usage see alloc/free/request-lower */
			}
		}
	}
	}

	/* check for duplicates */

	{
	CM_CHANNEL_PTR_TYPE existing_channel = cm_channel_find_path_type(channel->detailptr->nic_id, channel->path_type);

	if (cm_is_not_null(existing_channel)) {

		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "channel with nic_id(%u) path_type(%u) already exists"
			, channel->detailptr->nic_id, channel->path_type
			);

		cm_callback_and_free_channel(channel, CM_ERR_SEQUENCE, rb);
		return;
	}
	}

	/* check if edd-channel exists */

	if (channel->path_type != CM_PATH_TYPE_EDD) {

		CM_CHANNEL_PTR_TYPE edd_channel = cm_channel_find_path_type(channel->detailptr->nic_id, CM_PATH_TYPE_EDD);

		if (cm_is_null(edd_channel)) {

			CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "edd-channel with nic_id(%u) not opened (see documentation)"
				, channel->detailptr->nic_id
				);

			cm_callback_and_free_channel(channel, CM_ERR_SEQUENCE, rb);
			return;
		}
		else { /* make a link to the edd-channel */

			channel->lower.channel[CM_PATH_TYPE_EDD] = edd_channel;
			channel->trace_idx = edd_channel->trace_idx;
		}
	}

	/***/

	channel->state = CM_CH_OPEN;

	channel->usr.ch.curr = rb; /* keep */

	/*
	 * open the lower layer
	 */

	switch( channel->path_type ) {

	case CM_PATH_TYPE_EDD:
		channel->lower.edd_handle = LSA_NULL; /* TIA 2709127 */
		cm_edd_channel_open (channel);
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		channel->lower.handle = CM_MRP_INVALID_HANDLE;
		cm_mrp_channel_open (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		channel->lower.handle = CM_GSY_INVALID_HANDLE;
		cm_gsy_channel_open (channel);
		break;
#endif

	case CM_PATH_TYPE_NARE:
		channel->lower.handle = CM_NARE_INVALID_HANDLE;
		cm_nare_channel_open (channel);
		break;

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		channel->lower.handle = CM_POF_INVALID_HANDLE;
		cm_pof_channel_open (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		channel->lower.handle = CM_OHA_INVALID_HANDLE;
		cm_oha_channel_open (channel);
		break;

	case CM_PATH_TYPE_RPC:
		channel->lower.handle = CM_RPC_INVALID_HANDLE;
		cm_rpc_channel_open (channel);
		break;

	case CM_PATH_TYPE_PD:
		channel->lower.handle = 0/*CM_xxx_INVALID_HANDLE*/;

		if (! cm_channel_sys_opened(channel)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);

			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_channel_done(channel, CM_OK); /* no lower layer */
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);
			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		channel->acp.max_alarm_data_length = channel->detailptr->u.cl.acp.max_alarm_data_length; /*AP00342171*/

		if (! (CM_ALARM_DATA_LENGTH_MIN <= channel->acp.max_alarm_data_length
				&& channel->acp.max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX))
		{
			CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!(%u <= max_alarm_data_length(%u) <= %u)"
				, CM_ALARM_DATA_LENGTH_MIN, channel->acp.max_alarm_data_length, CM_ALARM_DATA_LENGTH_MAX
				);
			cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_acp_channel_open (channel);
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);

			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_channel_done(channel, CM_OK); /* no lower layer */
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		channel->lower.handle = ACP_INVALID_HANDLE;

		if (! cm_channel_sys_opened(channel)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all system-channels opened"
				);
			cm_callback_and_free_channel (channel, CM_ERR_SEQUENCE, channel->usr.ch.curr);
			break;
		}

		channel->acp.max_alarm_data_length = channel->detailptr->u.sv.acp.max_alarm_data_length;

		if (! (CM_ALARM_DATA_LENGTH_MIN <= channel->acp.max_alarm_data_length
				&& channel->acp.max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX))
		{
			CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "!(%u <= max_alarm_data_length(%u) <= %u)"
				, CM_ALARM_DATA_LENGTH_MIN, channel->acp.max_alarm_data_length, CM_ALARM_DATA_LENGTH_MAX
				);
			cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
			break;
		}

		cm_channel_sys_assign(channel);

		cm_acp_channel_open (channel);
		break;
#endif

	default:
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong path_type(%u)", channel->path_type
			);
		cm_callback_and_free_channel (channel, CM_ERR_SYS_PATH, channel->usr.ch.curr);
		break;
	}
}


/*------------------------------------------------------------------------------
//	close the channel after closing the channel to the lower layer
//----------------------------------------------------------------------------*/
LSA_VOID
cm_close_channel (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_CHANNEL_PTR_TYPE channel;
	LSA_UINT16 response;

		/* sanity checks */

	channel = cm_is_null(rb) ? LSA_NULL : cm_channel_from_handle(CM_RQB_GET_HANDLE(rb));

	if (cm_is_null(rb) || cm_is_null(channel)) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "no rqb(%08X) or invalid handle(%u)"
			, rb
			, (cm_is_null(rb) ? 0 : CM_RQB_GET_HANDLE(rb))
			);
		CM_FATAL ();
		/*lint -unreachable */
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> cm-rqb(0x%08x) h(%u) cm-opc(%u)",
			rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE (rb)
		);

	/* note: do not check rb->args! (don't care) */

	if (
		CM_RQB_GET_OPCODE (rb) != CM_OPC_CLOSE_CHANNEL
		|| channel->state != CM_CH_READY
	) {
		CM_UPPER_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) wrong opcode or state(%u) not ready"
			, channel->my_handle, channel->state
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	/*
	 * check if can close
	 */

	if( channel->path_type <= CM_PATH_TYPE_SYS_MAX ) {

		if (! cm_channel_usr_closed(channel)) {
			CM_UPPER_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "not all user-channels closed"
				);
			cm_callback(channel, CM_ERR_SEQUENCE, &rb);
			return;
		}
	}

	response = CM_OK;

	switch( channel->path_type ) {

	case CM_PATH_TYPE_EDD:
		response = cm_edd_channel_undo_init (channel);
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		response = cm_mrp_channel_undo_init (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		response = cm_gsy_channel_undo_init (channel);
		break;
#endif

	case CM_PATH_TYPE_NARE:
		break;

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		response = cm_pof_channel_undo_init (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		response = cm_oha_channel_undo_init (channel);
		break;

	case CM_PATH_TYPE_RPC:
		break;

	case CM_PATH_TYPE_PD:
		/* nothing */
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		/* nothing */
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		/* nothing */
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		/* nothing */
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}

	if( response != CM_OK ) {
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "undo_init returned rsp(%u)"
			, response
			);
		cm_callback(channel, response, &rb);
		return;
	}

#if CM_DEBUG
	CM_MEMSET(&channel->usr, 0xDD, sizeof(channel->usr)); /* wipe-out */
#endif

	/*
	 * close the lower layer
	 *
	 * NOTE: indication resources are cancelled before calling back, see cm_channel_done()
	 */

	channel->state = CM_CH_CLOSE;

	channel->usr.ch.curr = rb; /* keep */

	switch( channel->path_type ) {

	case CM_PATH_TYPE_EDD:
		if( ! cm_edd_set_send_clock (channel, 0) ) { /* must do that before closing the channel */
			CM_FATAL();
		}
		break;

#if CM_CFG_USE_MRP
	case CM_PATH_TYPE_MRP:
		cm_mrp_channel_close (channel);
		break;
#endif

#if CM_CFG_USE_GSY
	case CM_PATH_TYPE_GSY:
		cm_gsy_channel_close (channel);
		break;
#endif

	case CM_PATH_TYPE_NARE:
		cm_nare_channel_close (channel);
		break;

#if CM_CFG_USE_POF
	case CM_PATH_TYPE_POF:
		cm_pof_channel_close (channel);
		break;
#endif

	case CM_PATH_TYPE_OHA:
		cm_oha_channel_close (channel);
		break;

	case CM_PATH_TYPE_RPC:
		cm_rpc_channel_close (channel);
		break;

	case CM_PATH_TYPE_PD:
		cm_channel_done (channel, CM_OK);
		break;


#if CM_CFG_USE_ACP
	case CM_PATH_TYPE_MULTICAST:
		cm_peer_disconnect(channel);
		cm_channel_done (channel, CM_OK);
		break;

	case CM_PATH_TYPE_CLIENT:
	case CM_PATH_TYPE_SERVER:
		cm_peer_disconnect(channel);
		cm_acp_channel_close (channel);
		break;
#endif

	default:
		CM_FATAL(); /* a bug */
		break;
	}
}


/*------------------------------------------------------------------------------
//	receives a request from the user, dispatches it to the subsystem
//----------------------------------------------------------------------------*/
LSA_VOID
cm_request (
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_CHANNEL_PTR_TYPE channel;

	if (cm_is_null(rb)) {
		CM_FATAL ();
		/*lint -unreachable */
		return;
	}

#if CM_DEBUG
	CM_RQB_SET_NEXT_RQB_PTR(rb, LSA_NULL); /* sanity */
	CM_RQB_SET_PREV_RQB_PTR(rb, LSA_NULL); /* sanity */
#endif

	/*
	 * timer is special
	 */

	if (CM_RQB_GET_OPCODE (rb) == CM_OPC_TIMER) {
		cm_timeout_synchronized(rb);
		return;
	}

	channel = cm_channel_from_handle(CM_RQB_GET_HANDLE (rb));

	if( cm_is_null(channel) ) {
		CM_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_FATAL
			, "wrong handle(%u), opc(%u)", CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE(rb)
			);
		CM_FATAL ();
		/*lint -unreachable */
		return;
	}

	CM_UPPER_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
			">>> cm-rqb(0x%08x) h(%u) cm-opc(%u)",
			rb, CM_RQB_GET_HANDLE (rb), CM_RQB_GET_OPCODE(rb)
		);

	switch( channel->path_type ) {

	case CM_PATH_TYPE_PD:
		cm_pd_request (channel, rb);
		break;

#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_request (channel, rb);
		break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		cm_mc_request (channel, rb);
		break;
#endif

#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		cm_sv_opc_dispatch(channel, rb);
		break;
#endif

	default:
		CM_UPPER_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "not a user-channel, path_type(%u)", channel->path_type
			);
		CM_FATAL ();
		break;
	}
}


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
